#include "sipxhomer/HEPMessage.h"
#include "sqa/ServiceOptions.h"
#include "sipxhomer/HEPCaptureAgent.h"
#include "sipxhomer/HEPTestDriver.h"

#include "resip/stack/SipMessage.hxx"

int main(int argc , char** argv)
{
  ServiceOptions service(argc, argv, "SessionStateWatcher");
  service.addDaemonOptions();

  service.addOptionString("sqa-control-port", ": Port where to send control commands.");
  service.addOptionString("sqa-control-address", ": Address where to send control commands.");
  service.addOptionFlag("test-driver", ": Run internal test for proper HEP v3 operations.");

  if (!service.parseOptions())
  {
    service.displayUsage(std::cerr);
    return -1;
  }
  
  HEPCaptureAgent agent(service);
  agent.run();


  if (service.hasOption("test-driver"))
  {
    HEPTestDriver test(agent);
    if (!test.runTests())
      return -1;
    agent.stop();
    return 0;
  }

  service.waitForTerminationRequest();
  agent.stop();
  
}
