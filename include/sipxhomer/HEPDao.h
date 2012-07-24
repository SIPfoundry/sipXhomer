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

#ifndef HEPDAO_H
#define HEPDAO_H

#include <sql.h>
#include <sqlext.h>
#include <resip/stack/SipMessage.hxx>
#include "sqa/sqaclient.h"

struct HEPDaoException : std::exception
{
public:
    HEPDaoException(char const* what) : mWhat(what) {
    }

    ~HEPDaoException() throw() {
    }

    char const* what() const throw() {
        return mWhat.c_str();
    }

private:
    std::string mWhat;
};

class HEPDao
{
public:
    HEPDao();
    ~HEPDao();
    void connect(std::string& connection);
    void save(StateQueueMessage& object);
private:
    enum Capture {
      DATE,
      MICRO_TS,
      METHOD,
      REPLY_REASON,
      REQUESTURI,

      REQUESTURI_USER,
      FROM_USER,
      FROM_TAG,
      TO_USER,
      TO_TAG,

      PID_USER,
      CONTACT_USER,
      CONTACT_IP,
      CONTACT_PORT,
      AUTH_USER,

      CALL_ID,
      CALLID_ALEG,
      VIA_1,
      VIA_1_BRANCH,
      CSEQ,

      DIVERSION,
      REASON,
      CONTENT_TYPE,
      AUTH,
      USER_AGENT,

      SOURCE_IP,
      SOURCE_PORT,
      DEST_IP,
      DEST_PORT,
      ORIGINATOR_IP,

      ORIGINATOR_PORT,
      PROTO,
      FAMILY,
      RTP_STAT,
      TYPE,

      NODE,
      MSG,

      _NUM_FIELDS
    };
    SQLHENV mEnv;
    SQLHDBC mConn;
    SQLHSTMT mInsert;
    SQLSMALLINT mType[_NUM_FIELDS];
    int mFieldIndex;
    void checkError(SQLRETURN err, SQLHANDLE handle, SQLSMALLINT type);
    void bind(Capture c, void *data, int len);
    SQLSMALLINT sqlType(SQLSMALLINT t);
    SQLLEN* sqlLen(SQLSMALLINT t, void *data);
};

#endif
