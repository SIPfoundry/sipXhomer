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


#include <stdio.h>
#include <sql.h>
#include <sqlext.h>
#include <string>
#include <iostream>
#include <HomerMain.h>

using namespace std;

int homer()
{
    HomerDao dao;
    string conn("DATABASE=homer_db;SERVER=localhost;UID=root;DRIVER=MySQL;READONLY=0;");
    try
    {
        dao.connect(conn);
    } catch (string* e)
    {
        std::cout << e->c_str();
    }
}

void signal_handler(int sig)
{
    switch (sig)
    {
    case SIGPIPE:
        Os::Logger::instance().log(FAC_SIP, PRI_INFO, "SIGPIPE caught. Ignored.");
        break;

    case SIGHUP:
        Os::Logger::instance().log(FAC_SIP, PRI_INFO, "SIGHUP caught. Ignored.");
        break;

    case SIGTERM:
        gShutdownFlag = TRUE;
        Os::Logger::instance().log(FAC_SIP, PRI_INFO, "SIGTERM caught. Shutting down.");
        break;
    }
}

int main(int argc, char *argv[])
{
    char* pidFile = NULL;
    for (int i = 1; i < argc; i++)
    {
        if (strncmp("-v", argv[i], 2) == 0)
        {
            std::cout << "Version: " << PACKAGE_VERSION << PACKAGE_REVISION << std::endl;
            exit(0);
        }
        else
        {
            pidFile = argv[i];
        }
    }
    if (pidFile)
    {
        daemonize(pidFile);
    }
    signal(SIGHUP, signal_handler); // catch hangup signal
    signal(SIGTERM, signal_handler); // catch kill signal
    signal(SIGPIPE, signal_handler); // r/w socket failure

    homer();
}

