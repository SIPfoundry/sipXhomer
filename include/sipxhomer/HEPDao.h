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
    ~HEPDao();
    void connect(std::string& connection);
    void save(resip::SipMessage* msg);

private:
    SQLHENV mEnv;
    SQLHDBC mConn;
    SQLHSTMT mInsert;
    void checkError(SQLRETURN err, SQLHANDLE handle, SQLSMALLINT type);
    void bindString(SQLHSTMT hnd, int col, std::string& val);
    void bindInt(SQLHSTMT hnd, int col, int val);
};

#endif
