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

#include "sipXhomer/HomerDao.h"

using namespace std;

HomerDao::~HomerDao()
{
    SQLFreeHandle(SQL_HANDLE_ENV, mEnv);
    SQLFreeHandle(SQL_HANDLE_ENV, mConn);
}

void HomerDao::connect(string& connection)
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
}

void HomerDao::checkError(SQLRETURN err, SQLHANDLE handle, SQLSMALLINT type)
{
    SQLINTEGER i = 0;
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
            throw new string((char *) text);
        }
        else
        {
            throw new string("SQL error");
        }
    }
}
