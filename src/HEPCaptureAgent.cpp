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

#include <net/NetBase64Codec.h>
#include "os/OsLogger.h"
#include "sqa/ServiceOptions.h"
#include "sqa/sqaclient.h"
#include <boost/date_time.hpp>
#include "sipxhomer/HEPCaptureAgent.h"


using namespace resip;

HEPCaptureAgent::HEPCaptureAgent(ServiceOptions& options, HEPDao& dao) :
  _options(options),
  _pWatcher(0),
  _dao(dao),
  _pRunThread(0)
{
  OS_LOG_ERROR(FAC_NET, "HEPCaptureAgent CREATED");
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
  
 
  _pWatcher = new SQAWatcher("HEPCaptureAgent", "CAP", 1);

  OS_LOG_INFO(FAC_NET, "HEPCaptureAgent::internalRun started capturing events");
  while(_isRunning)
  {
    SQAEvent* pEvent = _pWatcher->watch();
    if (pEvent)
    {
#if 0
      std::string buff = std::string(pEvent->data, pEvent->data_len);


      OS_LOG_INFO(FAC_NET, "HEPCaptureAgent::internalRun received raw event " << buff);

      char decoded[8092];
      int decodedSize;
      NetBase64Codec::decode(buff.length(), buff.data(), decodedSize, decoded);
      std::string data(decoded, decodedSize);

      OS_LOG_INFO(FAC_NET, "HEPCaptureAgent::internalRun Netbase64 output total bytes = " <<  decodedSize);

      HEPMessage message;
      if (message.parse(data))
      {
        onReceivedEvent(message);
      }
      else
      {
        OS_LOG_ERROR(FAC_NET, "HEPCaptureAgent::internalRun unable to parse event.  Throwing away.");
      }
      delete pEvent;
#else
      try
      {
        std::string buff = std::string(pEvent->data, pEvent->data_len);
        std::stringstream strm;
        strm << buff;
        json::Object object;
        json::Reader::Read(object, strm);

        json::Number ipProtoFamily = object["IpProtoFamily"]; // = json::Number(HEPMessage::IpV4);
        json::Number ipProtoId = object["IpProtoId"]; // = json::Number(HEPMessage::TCP);
        json::String ip4SrcAddress = object["Ip4SrcAddress"]; // = json::String(_localHost.c_str());
        json::String ip4DestAddress = object["Ip4DestAddress"]; // = json::String(address);
        json::Number srcPort = object["SrcPort"]; // = json::Number(_localPort);
        json::Number destPort = object["DestPort"]; // = json::Number(port);
        json::Number timeStamp = object["TimeStamp"]; // = json::Number(now.tv_sec);
        json::Number protocolType = object["ProtocolType"]; // = json::Number(HEPMessage::SIP);
        json::String data = object["Data"]; // = json::String(msg.c_str());

        Data buffer(data.Value().c_str());
        OS_LOG_INFO(FAC_NET, "HEPCaptureAgent::onReceivedEvent SIP Message " << data.Value().c_str());
        SipMessage* msg = SipMessage::make(buffer);
        if (msg)
        {
          OS_LOG_INFO(FAC_NET, "HEPCaptureAgent::onReceivedEvent received valid event.  Pushing to HEPDao.");
          _dao.save(msg);
          delete msg;
        }
        else
        {
          OS_LOG_ERROR(FAC_NET, "HEPCaptureAgent::onReceivedEvent received invalid SIPMessage.");
        }

      }
      catch(std::exception& error)
      {
      }
#endif
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
  OS_LOG_INFO(FAC_NET,  "event.getIp4SrcAddress: " <<  event.getIp4SrcAddress().to_string());
  OS_LOG_INFO(FAC_NET,  "event.getIp4DestAddress: " << event.getIp4DestAddress().to_string());
  OS_LOG_INFO(FAC_NET,  "event.getSrcPort: " << event.getSrcPort());
  OS_LOG_INFO(FAC_NET,  "event.getDestPort: " << event.getDestPort());

  Data buffer(event.getData().c_str());
  OS_LOG_INFO(FAC_NET, "HEPCaptureAgent::onReceivedEvent SIP Message " << event.getData());
  SipMessage* msg = SipMessage::make(buffer);
  if (msg)
  {
    OS_LOG_INFO(FAC_NET, "HEPCaptureAgent::onReceivedEvent received valid event.  Pushing to HEPDao.");
    _dao.save(msg);
    delete msg;
  }
  else
  {
    OS_LOG_ERROR(FAC_NET, "HEPCaptureAgent::onReceivedEvent received invalid SIPMessage.");
  }
}
