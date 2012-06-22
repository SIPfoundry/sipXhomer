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
    SQLPrepare(mInsert, (SQLCHAR *)insertSql, strlen(insertSql));
}

void HEPDao::save(SipMessage* msg)
{
  if (msg->isInvalid())
    return;

  SQLFreeStmt(mInsert, SQL_UNBIND);

  int i = 1;

  // date
  i++;

  // micro_ts
  i++;

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
    bindString(mInsert,i, cseqMethod);
  }
  i++;

  // reply_reason
  string reason;
  if (msg->exists(h_Reasons))
  {
    reason = msg->const_header(h_Reasons).front().value().c_str();
    bindString(mInsert, i, reason);
  }
  i++;

  string requestLine;
  string statusLine;
  string requestUriUser;
  if (msg->isRequest())
  {
    // ruri
    ostringstream strm;
    msg->const_header(h_RequestLine).uri().encode(strm);
    requestLine = strm.str();
    bindString(mInsert, i, requestLine);

    // ruri_user
    requestUriUser = msg->const_header(h_RequestLine).uri().user().c_str();
    bindString(mInsert, i + 1, requestUriUser);
  }
  else
  {
    ostringstream strm;
    msg->const_header(h_StatusLine).encode(strm);
    statusLine = strm.str();  // where does this go
  }
  i += 2;

  string fromTag;
  string user;
  if (msg->exists(h_From))
  {
    // from_user
    user = msg->const_header(h_From).uri().user().c_str();
    bindString(mInsert, i, user);

    // from_tag
    fromTag = msg->const_header(h_From).exists(p_tag) ? msg->const_header(h_From).param(p_tag).c_str()
        : string();
    bindString(mInsert, i + 1, fromTag);
  }
  i += 2;

  string toTag;
  string toUser;
  if (msg->exists(h_To))
  {
    // to_user
    toUser = msg->const_header(h_To).uri().user().c_str();
     bindString(mInsert, i, toUser);

    // to_tag
    toTag = msg->const_header(h_To).exists(p_tag) ? msg->const_header(h_To).param(p_tag).c_str()
        : string();
    bindString(mInsert, i + 1, toTag);
  }
  i += 2;

  // pid_user
  string pidentity;
  if (msg->exists(h_PAssertedIdentities))
  {
    ostringstream pidStrm;
    msg->const_header(h_PAssertedIdentities).front().uri().encode(pidStrm);
    pidentity = pidStrm.str();
    bindString(mInsert, i, pidentity);
  }
  i++;

  string contactUser;
  string contactHost;
  int contactPort = 0;
  if (msg->exists(h_Contacts))
  {
    // contact_user
    contactUser = msg->const_header(h_Contacts).front().uri().user().c_str();
    bindString(mInsert,i, contactUser);

    //contact_ip
    contactHost = msg->const_header(h_Contacts).front().uri().host().c_str();
    bindString(mInsert, i, contactHost);

    //contact_port
    contactPort = msg->const_header(h_Contacts).front().uri().port();
    bindInt(mInsert, i, contactPort);
  }
  i++;

  // auth_user
  i++;

  // callid
  string callId;
  if (msg->exists(h_CallID))
  {
    callId = msg->const_header(h_CallID).value().c_str();
    bindString(mInsert, i, callId);
  }
  i++;

  // callid_aleg
  i++;

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
    bindString(mInsert, i, viaBranch);
  }
  i++;

  // via_1_branch
  i++;

  // cseq
  string cseq;
  ostringstream cseqStrm;
  if (msg->exists(h_CSeq))
  {
    msg->const_header(h_CSeq).encode(cseqStrm);
    cseq = cseqStrm.str();
    bindString(mInsert, i, cseq);
  }
  i++;

  // diversion
  i++;

  // reason
  i++;

  // content_type
  string contentType;
  if (msg->exists(h_ContentType))
  {
    ostringstream ctypeStrm;
    msg->const_header(h_ContentType).encode(ctypeStrm);
    contentType = ctypeStrm.str();
    bindString(mInsert, i, contentType);
  }
  i++;

  // authorization
  string authorization;
  if (msg->exists(h_Authorizations))
  {
    ostringstream authStrm;
    msg->const_header(h_Authorizations).front().encode(authStrm);
    authorization = authStrm.str();
    bindString(mInsert, i, authorization);
  }
  else if (msg->exists(h_ProxyAuthorizations))
  {
    ostringstream authStrm;
    msg->const_header(h_ProxyAuthorizations).front().encode(authStrm);
    authorization = authStrm.str();
    bindString(mInsert, i, authorization);
  }
  i++;

  // user_agent
  string userAgent;
  if (msg->exists(h_UserAgent))
  {
    userAgent = msg->const_header(h_UserAgent).value().c_str();
    bindString(mInsert, i, userAgent);
  }
  i++;

  // source_ip
  i++;

  // source_port
  i++;

  // destination_ip
  i++;

  // destination_port
  i++;

  // originator_ip
  i++;

  // originator_port
  i++;

  // proto
  i++;

  // family
  i++;

  // rtp_stat
  i++;

  // type
  i++;

  // node
  i++;

  // msg
  i++;

  SQLExecute(mInsert);
}

void HEPDao::bindString(SQLHSTMT hnd, int col, string& val)
{
  SQLLEN indicator;
  SQLBindCol(hnd, col, SQL_C_CHAR, (void *)val.c_str(), val.length(), &indicator);
}

void HEPDao::bindInt(SQLHSTMT hnd, int col, int val)
{
  SQLLEN indicator;
  SQLBindCol(hnd, col, SQL_C_NUMERIC, &val, sizeof(val), &indicator);
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
            throw HEPDaoException("SQL error");
        }
    }
}
