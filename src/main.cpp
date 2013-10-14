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

#include "sipxhomer/HEPMessage.h"
#include "sipxhomer/HEPCaptureAgent.h"
#include "sipxhomer/HEPTestDriver.h"

#include "resip/stack/SipMessage.hxx"

#include <sipXecsService/SipXApplication.h>

using namespace std;

#define SIPXHOMER_APP_NAME              "sipxhomer"

int main(int argc , char** argv)
{
  SipXApplicationData homerData =
  {
      SIPXHOMER_APP_NAME,
      "",
      "",
      "",
      "",
      "1.0",
      "Ezuce Inc. All Rights Reserved",
      false, // do not check mongo connection
      true, // increase application file descriptor limits
      SipXApplicationData::CmdLineOptDefaults, // command line default options: parse command line and show version and help
      SipXApplicationData::ConfigFileFormatIni, // format type for configuration file
      SipXApplicationData::DaemonizeInitTypeOsServiceOptions,// Daemonize type service options
#ifdef SERVICE_NO_LOGGER
      SipXApplicationData::LoggerInitTypeNone,               // No logger
#else
      SipXApplicationData::LoggerInitTypeOsServiceOptions,   // Logger init type service options
#endif
      OsMsgQShared::QUEUE_LIMITED, //limited queue
  };

  SipXApplication& sipXApplication = SipXApplication::instance();
  OsServiceOptions& osServiceOptions = sipXApplication.getConfig();

  if (!sipXApplication.init(argc, argv, homerData))
  {
    sipXApplication.displayUsage(std::cerr);
    return -1;
  }
  else
  {
    string dbUrl;
    osServiceOptions.getOption("db-url", dbUrl);
    HEPDao dao;
    dao.connect(dbUrl);
    HEPCaptureAgent agent(osServiceOptions, dao);
    agent.run();
    sipXApplication.waitForTerminationRequest();
    agent.stop();
  }

  return 0;
}
