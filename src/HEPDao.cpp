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
#include <string>
#include <sstream>
#include <boost/throw_exception.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <resip/stack/SipMessage.hxx>
#include "sipxhomer/HEPDao.h"
#include "sipxhomer/HEPMessage.h"


using namespace resip;


HEPDao::HEPDao() {
  std::fill_n(mType, _NUM_FIELDS, SQL_C_CHAR);
  mType[DATE] = SQL_C_TIMESTAMP;
  mType[MICRO_TS] = SQL_C_SBIGINT;

  mType[CONTACT_PORT] = SQL_C_LONG;
  mType[SOURCE_PORT] = SQL_C_LONG;
  mType[DEST_PORT] = SQL_C_LONG;
  mType[ORIGINATOR_PORT] = SQL_C_LONG;

  mType[PROTO] = SQL_C_LONG;
  mType[FAMILY] = SQL_C_LONG;
  mType[TYPE] = SQL_C_LONG;
}

HEPDao::~HEPDao()
{
    close();
}

void HEPDao::close()
{
    SQLFreeHandle(SQL_HANDLE_ENV, mEnv);
    SQLFreeHandle(SQL_HANDLE_ENV, mConn);
    SQLFreeHandle(SQL_HANDLE_STMT, mInsert);
}

void HEPDao::reconnect()
{
  OS_LOG_INFO(FAC_NET, "HEPDao::reconnection attempt");
  close();
  connect(connectionUrl);
}

void HEPDao::connect(std::string& connection)
{

    lastConnectionAttempt = boost::posix_time::second_clock::local_time();
    connectionUrl = connection;
    SQLRETURN err;
    err = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &mEnv);
    checkError(err, mEnv, SQL_HANDLE_ENV);

    err = SQLSetEnvAttr(mEnv, SQL_ATTR_ODBC_VERSION, (void*) SQL_OV_ODBC3, 0);
    checkError(err, mEnv, SQL_HANDLE_ENV);

    err = SQLAllocHandle(SQL_HANDLE_DBC, mEnv, &mConn);
    checkError(err, mConn, SQL_HANDLE_DBC);

    err = SQLDriverConnect(mConn, NULL, (SQLCHAR*) connectionUrl.c_str(), SQL_NTS, NULL, 0, NULL,
            SQL_DRIVER_NOPROMPT);
    checkError(err, mConn, SQL_HANDLE_DBC);

    const char* insertSql = "insert into sip_capture ("
        "date, micro_ts, method, reply_reason, ruri, "
        "ruri_user, from_user, from_tag, to_user, to_tag, "
        "pid_user, contact_user, contact_ip, contact_port, auth_user,"
        "callid, callid_aleg, via_1, via_1_branch, cseq,"
        "diversion, reason, content_type, authorization, user_agent, "
        "source_ip, source_port, destination_ip, destination_port, originator_ip,"
        "originator_port, proto, family, rtp_stat, type, "
        "node, msg) values ("
        "?,?,?,?,?,"
        "?,?,?,?,?,"
        "?,?,?,?,?,"
        "?,?,?,?,?,"
        "?,?,?,?,?,"
        "?,?,?,?,?,"
        "?,?,?,?,?,"
        "?,?)";

    err = SQLAllocHandle(SQL_HANDLE_STMT, mConn, &mInsert);
    checkError(err, mConn, SQL_HANDLE_DBC);
    int len = strlen(insertSql);
    SQLPrepare(mInsert, (SQLCHAR *)insertSql, len);
    checkError(err, mInsert, SQL_HANDLE_STMT);

    OS_LOG_INFO(FAC_NET, "HEPDao::connect " << connectionUrl << " SUCCEEDED.");
}


// NOTE: This is not the most elegant code however, some concessions were made to
// make this as fast as possible as you could get hundreds of inserts per second
void HEPDao::save(StateQueueMessage& object)
{
  int outgoing = 0;
  int ipProtoId = HEPMessage::TCP;
  std::string ip4SrcAddress;
  std::string ip4DestAddress;
  int srcPort = 0;
  int destPort = 0;
  double timeStamp = 0;
  double timeStampMicroOffset = 0;
  std::string data;

  if (!object.get("Outgoing", outgoing))
    return;

#if 0 //  For now we determine this using the VIA header since the sipstack
      //  does not include the transport type in the processor callback
  if (!object.get("IpProtoId", ipProtoId))
    return;
#endif

  if (!object.get("Ip4SrcAddress", ip4SrcAddress))
    return;

  if (!object.get("Ip4DestAddress", ip4DestAddress))
    return;

  if (!object.get("SrcPort", srcPort))
    return;

  if (!object.get("DestPort", destPort))
    return;

  if (!object.get("TimeStamp", timeStamp))
    return;

  if (!object.get("TimeStampMicroOffset", timeStampMicroOffset))
    return;

  if (!object.get("Data", data))
    return;


  Data buffer(data.c_str());

  SipMessage* msg = SipMessage::make(buffer);

  if (!msg)
    return;

  if (msg->isInvalid())
  {
    delete msg;
    return;
  }

  SQLRETURN err = SQLFreeStmt(mInsert, SQL_UNBIND);
  checkError(err, mInsert, SQL_HANDLE_STMT);
  mFieldIndex = 0;

  struct timeval now;
  now.tv_sec = timeStamp;
  now.tv_usec = timeStampMicroOffset;
  time_t timeNow = now.tv_sec;

  // gmt is ideal, but cannot be gmt because homer does not expect it as such
  // unfortunately that means change system tz means times become wrong.
  struct tm* ltime = localtime (&timeNow);

  // date
  TIMESTAMP_STRUCT date;
  date.year = ltime->tm_year + 1900;
  date.month = ltime->tm_mon + 1;
  date.day = ltime->tm_mday;
  date.hour = ltime->tm_hour;
  date.minute = ltime->tm_min;
  date.second = ltime->tm_sec;
  date.fraction = 0;
  bind(DATE, &date, sizeof(date));

  // micro_ts
  unsigned long long microTs = (unsigned long long)now.tv_sec*1000000+now.tv_usec;
  bind(MICRO_TS, &microTs, sizeof(unsigned long long));


  // method

  //
  // Note: homer method is not the transaction method.  For requests, it is the
  // actual method.  For responses it is the reason code.
  //
  std::string statusCode;
  if (msg->isRequest())
  {
     bind(METHOD, (void *) msg->methodStr().data(), msg->methodStr().size());
  }
  else
  {
    try
    {
      statusCode = boost::lexical_cast<std::string>(msg->const_header(h_StatusLine).responseCode());
      bind(METHOD, (void *) statusCode.data(), statusCode.size());
    }
    catch(...)
    {
    }
  }

  std::string reply_reason;
  if (!msg->isRequest())
  {
    // reply_reason -  This is the reason phrase for response
    reply_reason = msg->const_header(h_StatusLine).reason().c_str();
    bind(REPLY_REASON, (void *) reply_reason.data(), reply_reason.length());
  }

  std::string requestLine;
  std::string statusLine;
  std::string requestUriUser;

  const char* direction = outgoing ? "Outgoing" : "Incoming";

  if (msg->isRequest())
  {
    // ruri
    std::ostringstream strm;
    msg->const_header(h_RequestLine).uri().encode(strm);
    requestLine = strm.str();
    bind(REQUESTURI, (void *) requestLine.data(), requestLine.length());

    // ruri_user
    requestUriUser = msg->const_header(h_RequestLine).uri().user().c_str();
    bind(REQUESTURI_USER, (void *) requestUriUser.data(), requestUriUser.length());

    OS_LOG_INFO(FAC_NET, "HEPDao::save " << direction << ": " << msg->methodStr().data() << " "
            << ip4SrcAddress << ":" << srcPort << "->"
            << ip4DestAddress << ":" << destPort );
  }
  else
  {
    std::ostringstream strm;
    msg->const_header(h_StatusLine).encode(strm);
    statusLine = strm.str();  // where does this go
    OS_LOG_INFO(FAC_NET, "HEPDao::save " << direction << ": " << statusLine << " "
            << ip4SrcAddress << ":" << srcPort << "->"
            << ip4DestAddress << ":" << destPort);
  }

  OS_LOG_DEBUG(FAC_NET, "HEPDao::save SIP Message " << data.c_str());

  std::string fromTag;
  std::string fromUser;
  if (msg->exists(h_From))
  {
    // from_user
    fromUser = msg->const_header(h_From).uri().user().c_str();
    bind(FROM_USER, (void *) fromUser.data(), fromUser.length());

    // from_tag
    fromTag = msg->const_header(h_From).exists(p_tag) ? msg->const_header(h_From).param(p_tag).c_str()
        : std::string();
    bind(FROM_TAG, (void *) fromTag.data(), fromTag.length());
  }

  std::string toTag;
  std::string toUser;
  if (msg->exists(h_To))
  {
    // to_user
    toUser = msg->const_header(h_To).uri().user().c_str();
    bind(TO_USER, (void *) toUser.data(), toUser.length());

    // to_tag
    toTag = msg->const_header(h_To).exists(p_tag) ? msg->const_header(h_To).param(p_tag).c_str()
        : std::string();
    bind(TO_TAG, (void *) toTag.data(), toTag.length());
  }

  // pid_user
  std::string pidentity;
  if (msg->exists(h_PAssertedIdentities))
  {
    std::ostringstream pidStrm;
    msg->const_header(h_PAssertedIdentities).front().uri().encode(pidStrm);
    pidentity = pidStrm.str();
    bind(PID_USER, (void *) pidentity.data(), pidentity.length());
  }

  std::string contactUser;
  std::string contactHost;
  int contactPort = 0;
  if (msg->exists(h_Contacts))
  {
    // contact_user
    contactUser = msg->const_header(h_Contacts).front().uri().user().c_str();
    bind(CONTACT_USER, (void *) contactUser.data(), contactUser.length());

    //contact_ip
    contactHost = msg->const_header(h_Contacts).front().uri().host().c_str();
    bind(CONTACT_IP, (void *) contactHost.data(), contactHost.length());

    //contact_port
    contactPort = msg->const_header(h_Contacts).front().uri().port();
    bind(CONTACT_PORT, (void *) &contactPort, sizeof(contactPort));
  }
  else
  {
    //
    // There is no contact but homer requires it cant be null
    //
    // contact_user
    bind(CONTACT_USER, (void *) contactUser.data(), contactUser.length());
    //contact_ip
    bind(CONTACT_IP, (void *) contactHost.data(), contactHost.length());
    //contact_port
    bind(CONTACT_PORT, (void *) &contactPort, sizeof(contactPort));
  }

  // auth_user

  // callid
  std::string callId;
  if (msg->exists(h_CallID))
  {
    callId = msg->const_header(h_CallID).value().c_str();
    bind(CALL_ID, (void *) callId.data(), callId.length());
  }

  // callid_aleg

  std::string viaBranch;
  std::string via;
  std::string viaProtocol;
  if (msg->exists(h_Vias))
  {
    // via_1
    Via& frontVia = msg->header(h_Vias).front();
    std::ostringstream viaStrm;
    frontVia.encode(viaStrm);
    via = viaStrm.str();
    bind(VIA_1, (void *) via.data(), via.length());

    // via_1_branch
    if (frontVia.param(p_branch).hasMagicCookie())
      viaBranch = "z9hG4bK";
    viaBranch += frontVia.param(p_branch).getTransactionId().c_str();
    bind(VIA_1_BRANCH, (void *) viaBranch.data(), viaBranch.length());

    //
    // Get the protocol string to be used for determining the transport type
    //
    viaProtocol = frontVia.transport().data();
    boost::to_upper(viaProtocol);
  }


  // cseq
  std::string cseq;
  std::ostringstream cseqStrm;
  if (msg->exists(h_CSeq))
  {
    msg->const_header(h_CSeq).encode(cseqStrm);
    cseq = cseqStrm.str();
    bind(CSEQ, (void *) cseq.data(), cseq.length());
  }

  // diversion


  // reason
  std::string reason;
  if (msg->exists(h_Reasons))
  {
    reason = msg->const_header(h_Reasons).front().value().c_str();
    bind(REPLY_REASON, (void *) reason.data(), reason.length());
  }


  // content_type
  std::string contentType;
  if (msg->exists(h_ContentType))
  {
    std::ostringstream ctypeStrm;
    msg->const_header(h_ContentType).encode(ctypeStrm);
    contentType = ctypeStrm.str();
    bind(CONTENT_TYPE, (void *) contentType.data(), contentType.length());
  }

  // authorization
  std::string authorization;
  if (msg->exists(h_Authorizations))
  {
    std::ostringstream authStrm;
    msg->const_header(h_Authorizations).front().encode(authStrm);
    authorization = authStrm.str();
    bind(AUTH, (void *) authorization.data(), authorization.length());
  }
  else if (msg->exists(h_ProxyAuthorizations))
  {
    std::ostringstream authStrm;
    msg->const_header(h_ProxyAuthorizations).front().encode(authStrm);
    authorization = authStrm.str();
    bind(AUTH, (void *) authorization.data(), authorization.length());
  }

  // user_agent
  std::string userAgent;
  if (msg->exists(h_UserAgent))
  {
    userAgent = msg->const_header(h_UserAgent).value().c_str();
    bind(USER_AGENT, (void *) userAgent.data(), userAgent.length());
  }

  // source_ip
  bind(SOURCE_IP, (void *) ip4SrcAddress.c_str(), ip4SrcAddress.length());

  // source_port
  bind(SOURCE_PORT, (void*)&srcPort, sizeof(srcPort));

  // destination_ip
  bind(DEST_IP, (void *) ip4DestAddress.c_str(), ip4DestAddress.length());

  // destination_port
  bind(DEST_PORT, (void*)&destPort, sizeof(destPort));

  // originator_ip

  // originator_port
  int zero = 0;
  bind(ORIGINATOR_PORT, (void*)&zero, sizeof(zero));

  // proto
  if (!viaProtocol.empty())
  {
    if (viaProtocol == "TCP")
      ipProtoId = HEPMessage::TCP;
    else
      ipProtoId= HEPMessage::UDP;
  }
  bind(PROTO, (void*)&ipProtoId, sizeof(ipProtoId));

  // family
  // only field in schema allowed to be NULL
  int protoFamily = HEPMessage::IpV4;
  bind(FAMILY, (void*)&protoFamily, sizeof(protoFamily));

  // rtp_stat

  // type
  int protocolType = HEPMessage::SipX;
  bind(TYPE, (void*)&protocolType, sizeof(protocolType));
  // node

  // NOTE: Need to always bind last column or you get SQL unbound cols error

  // msg
  bind(MSG, (void *) data.c_str(), data.length());

  err = SQLExecute(mInsert);
  checkError(err, mInsert, SQL_HANDLE_STMT);

  delete msg;
}

void HEPDao::bind(Capture c, void *data, int len) {
  static const char* blank = "";

  if (mFieldIndex > c) {
    throw HEPDaoException("Programming error, binding columns out of order");
  }

  // null fields up to  col
  while (mFieldIndex < c) {

    // Homer schema uses empty strings instead or null with few exceptions
    // if you have an exception call explicitly for field
    //    bind(FIELD_ID, NULL, 0);
    //
    void *nil = (mType[mFieldIndex] == SQL_C_CHAR ? (void *)blank : NULL);
    bind((Capture) mFieldIndex, nil, 0);
  }

  SQLSMALLINT cType = mType[c];
  SQLLEN* indicator = sqlLen(cType, data);
  SQLRETURN err = SQLBindParameter(mInsert, mFieldIndex + 1, SQL_PARAM_INPUT, cType, sqlType(cType), len, 0, data, 0, indicator);
  checkError(err, mInsert, SQL_HANDLE_STMT);
  mFieldIndex++;
}

SQLLEN* HEPDao::sqlLen(SQLSMALLINT cType, void *data) {
  static SQLLEN ntsLen = SQL_NTS;
  static SQLLEN nilLen = SQL_NULL_DATA;
  static SQLLEN intLen = 32;
  static SQLLEN bigIntLen = 64;
  static SQLLEN timestampLen = sizeof(TIMESTAMP_STRUCT);

  if (data == NULL) {
    return &nilLen;
  }
  switch (cType) {
  case SQL_C_SBIGINT:
    return &bigIntLen;
  case SQL_C_LONG:
    return &intLen;
  case SQL_C_TIMESTAMP:
    return &timestampLen;
  default:
    return &ntsLen;
  }
}

SQLSMALLINT HEPDao::sqlType(SQLSMALLINT cType) {
  switch (cType) {
  case SQL_C_SBIGINT:
    return SQL_BIGINT;
  case SQL_C_LONG:
    return SQL_INTEGER;
  case SQL_C_TIMESTAMP:
    return SQL_TIMESTAMP;
  default:
    return SQL_CHAR;
  }
}

void HEPDao::checkError(SQLRETURN err, SQLHANDLE handle, SQLSMALLINT type)
{
    SQLINTEGER native;
    SQLCHAR state[7];
    SQLCHAR text[256];
    SQLSMALLINT len;
    SQLRETURN ret;

    if (!SQL_SUCCEEDED(err))
    {
        ret = SQLGetDiagRec(type, handle, 1, state, &native, text, sizeof(text), &len);
        if (SQL_SUCCEEDED(ret))
        {
  	  // XX-?? ODBC driver will not reconnect if mysql is restarted on CentOS 6 despite using 
  	  //  ..;OPTION=4194304.  Seems to work on Fedora 16 though. 
	  // 
	  // Here we're attempting to reconnect with throttle so we do not flood system.
	  //  
          OS_LOG_INFO(FAC_NET, "HEPDao::mysql error " << native);

	  // For error codes, see
	  //   http://dev.mysql.com/doc/refman/5.0/en/error-messages-client.html
	  if (native >= 2001 && native <= 2006) {

            OS_LOG_INFO(FAC_NET, "HEPDao::mysql down");
	    boost::posix_time::ptime now  = boost::posix_time::second_clock::local_time();
	    boost::posix_time::time_duration diff = now - lastConnectionAttempt;
	    // reconnect every 5 seconds seems reasonable default
	    if (diff.total_seconds() > 5) {
	      reconnect();
	    }	    
	  }
	  throw HEPDaoException((char *) text);
        }
        else
        {
          throw HEPDaoException("SQL error ");
        }
    }
}
