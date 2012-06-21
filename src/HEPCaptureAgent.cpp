// Copyright (c) 2012 eZuce, Inc. All rights reserved.
// Contributed to SIPfoundry under a Contributor Agreement
//
// This software is free software; you can redistribute it and/or modify it under
// the terms of the Affero General Public License (AGPL) as published by the
// Free Software Foundation; either version 3 of the License, or (at your option)
// any later version.
//
// This software is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
// details.

#include "sipxhomer/HEPCaptureAgent.h"
#include "os/OsLogger.h"
#include "sqa/ServiceOptions.h"
#include "sqa/sqaclient.h"
#include "resip/stack/SipMessage.hxx"
#include <boost/date_time.hpp>

using namespace resip;

/*
 
 CREATE TABLE `sip_capture` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `date` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `micro_ts` bigint(18) NOT NULL DEFAULT '0',
  `method` varchar(50) NOT NULL DEFAULT '',
  `reply_reason` varchar(100) NOT NULL,
  `ruri` varchar(200) NOT NULL DEFAULT '',
  `ruri_user` varchar(100) NOT NULL DEFAULT '',
  `from_user` varchar(100) NOT NULL DEFAULT '',
  `from_tag` varchar(64) NOT NULL DEFAULT '',
  `to_user` varchar(100) NOT NULL DEFAULT '',
  `to_tag` varchar(64) NOT NULL,
  `pid_user` varchar(100) NOT NULL DEFAULT '',
  `contact_user` varchar(120) NOT NULL,
  `auth_user` varchar(120) NOT NULL,
  `callid` varchar(100) NOT NULL DEFAULT '',
  `callid_aleg` varchar(100) NOT NULL DEFAULT '',
  `via_1` varchar(256) NOT NULL,
  `via_1_branch` varchar(80) NOT NULL,
  `cseq` varchar(25) NOT NULL,
  `diversion` varchar(256) NOT NULL,
  `reason` varchar(200) NOT NULL,
  `content_type` varchar(256) NOT NULL,
  `authorization` varchar(256) NOT NULL,
  `user_agent` varchar(256) NOT NULL,
  `source_ip` varchar(50) NOT NULL DEFAULT '',
  `source_port` int(10) NOT NULL,
  `destination_ip` varchar(50) NOT NULL DEFAULT '',
  `destination_port` int(10) NOT NULL,
  `contact_ip` varchar(60) NOT NULL,
  `contact_port` int(10) NOT NULL,
  `originator_ip` varchar(60) NOT NULL DEFAULT '',
  `originator_port` int(10) NOT NULL,
  `proto` int(5) NOT NULL,
  `family` int(1) DEFAULT NULL,
  `rtp_stat` varchar(256) NOT NULL,
  `type` int(2) NOT NULL,
  `node` varchar(125) NOT NULL,
  `msg` varchar(1500) NOT NULL,
  PRIMARY KEY (`id`,`date`),
  KEY `ruri_user` (`ruri_user`),
  KEY `from_user` (`from_user`),
  KEY `to_user` (`to_user`),
  KEY `pid_user` (`pid_user`),
  KEY `auth_user` (`auth_user`),
  KEY `callid_aleg` (`callid_aleg`),
  KEY `date` (`date`),
  KEY `callid` (`callid`)
) ENGINE=MyISAM AUTO_INCREMENT=1 DEFAULT CHARSET=utf8
PARTITION BY RANGE ( UNIX_TIMESTAMP(`date`) ) (
PARTITION pmax VALUES LESS THAN (MAXVALUE)
);
 
 */

HEPCaptureAgent::HEPCaptureAgent(ServiceOptions& options, HEPDao& dao) :
  _options(options),
  _pWatcher(0),
  _dao(dao),
  _pRunThread(0)
{
  
}

HEPCaptureAgent::~HEPCaptureAgent()
{
  stop();
}

void HEPCaptureAgent::run()
{
  if (_pRunThread)
    return;
  _isRunning = true;
  _pRunThread = new boost::thread(boost::bind(&HEPCaptureAgent::internalRun, this));
}

void HEPCaptureAgent::internalRun()
{
  if (_pWatcher)
    return;
  
  std::string sqaAddress;
  std::string sqaPort;
  
  _options.getOption("sqa-control-address", sqaAddress);
  _options.getOption("sqa-control-port", sqaPort);
  
  _pWatcher = new SQAWatcher("HEPCaptureAgent", sqaAddress.c_str(), sqaPort.c_str(), "CAP", 1);
  
  while(_isRunning)
  {
    SQAEvent* pEvent = _pWatcher->watch();
    if (pEvent)
    {
      pEvent->data;
      std::string data = pEvent->data;
      HEPMessage message;
      if (message.parse(data))
      {
        onReceivedEvent(message);
      }
      delete pEvent;
    }
    else
    {
      _isRunning = false;
      break;
    }
  }
  delete _pWatcher;
  _pWatcher = 0;
}

void HEPCaptureAgent::stop()
{
  _isRunning = false;
  if (_pRunThread)
  {
    _pRunThread->join();
    delete _pRunThread;
    _pRunThread = 0;
  }
}

void HEPCaptureAgent::onReceivedEvent(HEPMessage& event)
{ 
  //
  // Dump to database
  //
  Data buffer(event.getData().c_str());
  SipMessage* msg = SipMessage::make(buffer);
  if (msg && !msg->isInvalid())
  {
    NameAddr remoteNameAddr;
    NameAddr localNameAddr;
    std::string fromTag;
    std::string toTag;
    std::string fromUser;
    std::string toUser;

    if (msg->exists(h_To))
    {
      toUser = msg->const_header(h_To).uri().user().c_str();
      toTag = msg->const_header(h_To).exists(p_tag) ? msg->const_header(h_To).param(p_tag).c_str() : std::string();
    }

    if (msg->exists(h_From))
    {
      fromUser = msg->const_header(h_From).uri().user().c_str();
      fromTag = msg->const_header(h_From).exists(p_tag) ? msg->const_header(h_From).param(p_tag).c_str() : std::string();
    }

    std::string requestLine;
    std::string statusLine;
    std::string requestUriUser;

    if (msg->isRequest())
    {
      std::ostringstream strm;
      msg->const_header(h_RequestLine).uri().encode(strm);
      requestLine = strm.str();
      requestUriUser = msg->const_header(h_RequestLine).uri().user().c_str();
    }
    else
    {
      std::ostringstream strm;
      msg->const_header(h_StatusLine).encode(strm);
      statusLine = strm.str();
    }

    std::string callId;
    if (msg->exists(h_CallID))
    {
      callId = msg->const_header(h_CallID).value().c_str();
    }

    std::string viaBranch;
    std::string via;
    if (msg->exists(h_Vias))
    {
      Via& frontVia = msg->header(h_Vias).front();
      std::ostringstream viaStrm;
      frontVia.encode(viaStrm);
      via = viaStrm.str();

      if (frontVia.param(p_branch).hasMagicCookie())
        viaBranch = "z9hG4bK";
      viaBranch += frontVia.param(p_branch).getTransactionId().c_str();
    }

    std::string contactUser;
    std::string contactHost;
    int contactPort = 0;
    if (msg->exists(h_Contacts))
    {
      contactUser = msg->const_header(h_Contacts).front().uri().user().c_str();
      contactHost = msg->const_header(h_Contacts).front().uri().host().c_str();
      contactPort = msg->const_header(h_Contacts).front().uri().port();
    }

    std::string pidentity;
    if (msg->exists(h_PAssertedIdentities))
    {
      std::ostringstream pidStrm;
      msg->const_header(h_PAssertedIdentities).front().uri().encode(pidStrm);
      pidentity = pidStrm.str();
    }

    std::string cseq;
    std::ostringstream cseqStrm;
    int cseqNumber = 0;
    std::string cseqMethod;

    if (msg->exists(h_CSeq))
    {
      msg->const_header(h_CSeq).encode(cseqStrm);
      cseq = cseqStrm.str();
      cseqNumber = msg->const_header(h_CSeq).sequence();
      switch(msg->const_header(h_CSeq).method())
      {
        case ACK:
          cseqMethod = "ACK";
          break;
        case BYE:
          cseqMethod = "BYE";
          break;
        case CANCEL:
          cseqMethod = "CANCEL";
          break;
        case INVITE:
          cseqMethod = "INVITE";
          break;
        case NOTIFY:
          cseqMethod = "NOTIFY";
          break;
        case OPTIONS:
          cseqMethod = "OPTIONS";
          break;
        case REFER:
          cseqMethod = "REFER";
          break;
        case REGISTER:
          cseqMethod = "REGISTER";
          break;
        case SUBSCRIBE:
          cseqMethod = "SUBSCRIBE";
          break;
        case MESSAGE:
          cseqMethod = "MESSAGE";
          break;
        case INFO:
          cseqMethod = "INFO";
          break;
        case PRACK:
          cseqMethod = "PRACK";
          break;
        case PUBLISH:
          cseqMethod = "PUBLISH";
          break;
        case SERVICE:
          cseqMethod = "SERVICE";
          break;
        case UPDATE:
          cseqMethod = "UPDATE";
          break;
      }
    }

    std::string reason;
    if (msg->exists(h_Reasons))
    {
      reason = msg->const_header(h_Reasons).front().value().c_str();
    }

    std::string authorization;
    if (msg->exists(h_Authorizations))
    {
      std::ostringstream authStrm;
      msg->const_header(h_Authorizations).front().encode(authStrm);
      authorization = authStrm.str();
    }
    else if (msg->exists(h_ProxyAuthorizations))
    {
      std::ostringstream authStrm;
      msg->const_header(h_ProxyAuthorizations).front().encode(authStrm);
      authorization = authStrm.str();
    }

    std::string contentType;
    if (msg->exists(h_ContentType))
    {
      std::ostringstream ctypeStrm;
      msg->const_header(h_ContentType).encode(ctypeStrm);
      contentType = ctypeStrm.str();
    }

    std::string userAgent;
    if (msg->exists(h_UserAgent))
    {
      userAgent = msg->const_header(h_UserAgent).value().c_str();
    }
  }
  delete msg;
}
