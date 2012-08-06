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
#include "sqa/ServiceOptions.h"
#include "sipxhomer/HEPCaptureAgent.h"
#include "sipxhomer/HEPTestDriver.h"

#include "resip/stack/SipMessage.hxx"

using namespace std;

int main(int argc , char** argv)
{
  ServiceOptions::daemonize(argc, argv);

  ServiceOptions service(argc, argv, "sipxhomer");
  service.addDaemonOptions();

  if (!service.parseOptions())
  {
    service.displayUsage(std::cerr);
    return -1;
  }
  
  string dbUrl;
  service.getOption("db-url", dbUrl);

  HEPDao dao;
  dao.connect(dbUrl);
  HEPCaptureAgent agent(service, dao);
  agent.run();
  service.waitForTerminationRequest();
  agent.stop();
}
