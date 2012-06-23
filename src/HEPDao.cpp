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
#include <resip/stack/SipMessage.hxx>
#include "sipxhomer/HEPDao.h"

using namespace std;
using namespace resip;

HEPDao::HEPDao() {
  fill_n(mType, sizeof(mType), SQL_C_CHAR);
  mType[DATE] = SQL_C_TIMESTAMP;
  mType[MICRO_TS] = SQL_C_SBIGINT;
  mType[CONTACT_PORT] = SQL_C_LONG;
  mType[SOURCE_PORT] = SQL_C_LONG;
  mType[DEST_PORT] = SQL_C_LONG;
  mType[ORIGINATOR_PORT] = SQL_C_LONG;
}

HEPDao::~HEPDao()
{
    SQLFreeHandle(SQL_HANDLE_ENV, mEnv);
    SQLFreeHandle(SQL_HANDLE_ENV, mConn);
    SQLFreeHandle(SQL_HANDLE_STMT, mInsert);
}

void HEPDao::connect(string& connection)
{
    SQLRETURN err;
    err = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &mEnv);
    checkError(err, mEnv, SQL_HANDLE_ENV);

    err = SQLSetEnvAttr(mEnv, SQL_ATTR_ODBC_VERSION, (void*) SQL_OV_ODBC3, 0);
    checkError(err, mEnv, SQL_HANDLE_ENV);

    err = SQLAllocHandle(SQL_HANDLE_DBC, mEnv, &mConn);
    checkError(err, mConn, SQL_HANDLE_DBC);

    err = SQLDriverConnect(mConn, NULL, (SQLCHAR*) connection.c_str(), SQL_NTS, NULL, 0, NULL,
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
}


// NOTE: This is not the most elegant code however, some concessions were made to
// make this as fast as possible as you could get hundreds of inserts per second
void HEPDao::save(SipMessage* msg)
{
  SQLRETURN err;
  if (msg->isInvalid())
    return;

  err = SQLFreeStmt(mInsert, SQL_UNBIND);
  checkError(err, mInsert, SQL_HANDLE_STMT);
  mFieldIndex = 0;

  // date
  TIMESTAMP_STRUCT date;
  date.year = 2012;
  date.month = 6;
  date.day = 22;
  date.hour = 0;
  date.minute = 0;
  date.second = 0;
  date.fraction = 0;
  bind(DATE, &date, sizeof(date));

  // micro_ts
  long long microTs = 0;
  bind(MICRO_TS, &microTs, sizeof(microTs));

  // method
  string cseqMethod;
  if (msg->exists(h_CSeq))
  {
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
    default:
      cseqMethod = "????";
      break;
    }
    bind(METHOD, (void *) cseqMethod.data(), cseqMethod.length());
  }

  // reply_reason
  string reason;
  if (msg->exists(h_Reasons))
  {
    reason = msg->const_header(h_Reasons).front().value().c_str();
    bind(REPLY_REASON, (void *) reason.data(), reason.length());
  }

  string requestLine;
  string statusLine;
  string requestUriUser;
  if (msg->isRequest())
  {
    // ruri
    ostringstream strm;
    msg->const_header(h_RequestLine).uri().encode(strm);
    requestLine = strm.str();
    bind(RESPONSE_URI, (void *) requestLine.data(), requestLine.length());

    // ruri_user
    requestUriUser = msg->const_header(h_RequestLine).uri().user().c_str();
    bind(RESPONSE_USER, (void *) requestUriUser.data(), requestUriUser.length());
  }
  else
  {
    ostringstream strm;
    msg->const_header(h_StatusLine).encode(strm);
    statusLine = strm.str();  // where does this go
  }

  string fromTag;
  string fromUser;
  if (msg->exists(h_From))
  {
    // from_user
    fromUser = msg->const_header(h_From).uri().user().c_str();
    bind(FROM_USER, (void *) fromUser.data(), fromUser.length());

    // from_tag
    fromTag = msg->const_header(h_From).exists(p_tag) ? msg->const_header(h_From).param(p_tag).c_str()
        : string();
    bind(FROM_TAG, (void *) fromTag.data(), fromTag.length());
  }

  string toTag;
  string toUser;
  if (msg->exists(h_To))
  {
    // to_user
    toUser = msg->const_header(h_To).uri().user().c_str();
    bind(TO_USER, (void *) toUser.data(), toUser.length());

    // to_tag
    toTag = msg->const_header(h_To).exists(p_tag) ? msg->const_header(h_To).param(p_tag).c_str()
        : string();
    bind(TO_TAG, (void *) toTag.data(), toTag.length());
  }

  // pid_user
  string pidentity;
  if (msg->exists(h_PAssertedIdentities))
  {
    ostringstream pidStrm;
    msg->const_header(h_PAssertedIdentities).front().uri().encode(pidStrm);
    pidentity = pidStrm.str();
    bind(PID_USER, (void *) pidentity.data(), pidentity.length());
  }

  string contactUser;
  string contactHost;
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

  // auth_user

  // callid
  string callId;
  if (msg->exists(h_CallID))
  {
    callId = msg->const_header(h_CallID).value().c_str();
    bind(CALL_ID, (void *) callId.data(), callId.length());
  }

  // callid_aleg

  string viaBranch;
  string via;
  if (msg->exists(h_Vias))
  {
    // via_1
    Via& frontVia = msg->header(h_Vias).front();
    ostringstream viaStrm;
    frontVia.encode(viaStrm);
    via = viaStrm.str();

    if (frontVia.param(p_branch).hasMagicCookie())
      viaBranch = "z9hG4bK";
    viaBranch += frontVia.param(p_branch).getTransactionId().c_str();
    bind(VIA_1, (void *) viaBranch.data(), viaBranch.length());
  }

  // via_1_branch

  // cseq
  string cseq;
  ostringstream cseqStrm;
  if (msg->exists(h_CSeq))
  {
    msg->const_header(h_CSeq).encode(cseqStrm);
    cseq = cseqStrm.str();
    bind(CSEQ, (void *) cseq.data(), cseq.length());
  }

  // diversion

  // reason

  // content_type
  string contentType;
  if (msg->exists(h_ContentType))
  {
    ostringstream ctypeStrm;
    msg->const_header(h_ContentType).encode(ctypeStrm);
    contentType = ctypeStrm.str();
    bind(CONTENT_TYPE, (void *) contentType.data(), contentType.length());
  }

  // authorization
  string authorization;
  if (msg->exists(h_Authorizations))
  {
    ostringstream authStrm;
    msg->const_header(h_Authorizations).front().encode(authStrm);
    authorization = authStrm.str();
    bind(AUTH, (void *) authorization.data(), authorization.length());
  }
  else if (msg->exists(h_ProxyAuthorizations))
  {
    ostringstream authStrm;
    msg->const_header(h_ProxyAuthorizations).front().encode(authStrm);
    authorization = authStrm.str();
    bind(AUTH, (void *) authorization.data(), authorization.length());
  }

  // user_agent
  string userAgent;
  if (msg->exists(h_UserAgent))
  {
    userAgent = msg->const_header(h_UserAgent).value().c_str();
    bind(USER_AGENT, (void *) userAgent.data(), userAgent.length());
  }

  // source_ip

  // source_port

  // destination_ip

  // destination_port

  // originator_ip

  // originator_port

  // proto

  // family
  // only field in schema allowed to be NULL
  bind(FAMILY, NULL, 0);

  // rtp_stat

  // type

  // node

  // NOTE: Need to always bind last column or you get SQL unbound cols error

  // msg
  string body = "";
  bind(MSG, (void *) body.data(), body.length());

  err = SQLExecute(mInsert);
  checkError(err, mInsert, SQL_HANDLE_STMT);
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
    void *nil = (mType[c] == SQL_C_CHAR ? (void *)blank : NULL);
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
            throw HEPDaoException((char *) text);
        }
        else
        {
          throw HEPDaoException("SQL error ");
        }
    }
}