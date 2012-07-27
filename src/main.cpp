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
  ServiceOptions service(argc, argv, "sipxhomer");
  service.addDaemonOptions();

  service.addOptionFlag("test-driver", ": Run internal test for proper HEP v3 operations.");

  if (!service.parseOptions())
  {
    service.displayUsage(std::cerr);
    return -1;
  }
  
  HEPDao* pDao = new HEPDao();
  string dbUrl;
  service.getOption("db-url", dbUrl);
  pDao->connect(dbUrl);

  HEPCaptureAgent* pAgent = new HEPCaptureAgent(service, *pDao);
  pAgent->run();


  if (service.hasOption("test-driver"))
  {
    HEPTestDriver test(*pAgent);
    if (!test.runTests())
      return -1;
    pAgent->stop();
    return 0;
  }

  service.waitForTerminationRequest();

  pAgent->stop();
  delete pAgent;
  delete pDao;
}
