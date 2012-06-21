#ifndef HEPCAPTUREAGENT_H
#define	HEPCAPTUREAGENT_H

#include "sqa/ServiceOptions.h"
#include "sqa/sqaclient.h"
#include "sipxhomer/HEPMessage.h"
#include <boost/thread.hpp>

class HEPCaptureAgent
{
public:
  HEPCaptureAgent(ServiceOptions& options);
  ~HEPCaptureAgent();
  void run();
  void stop();
  void onReceivedEvent(HEPMessage& event);
  ServiceOptions& options();
private:
  void internalRun();
  ServiceOptions& _options;
  SQAWatcher* _pWatcher;
  bool _isRunning;
  boost::thread* _pRunThread;
};


//
// Inlines
//

inline ServiceOptions& HEPCaptureAgent::options()
{
  return _options;
}

#endif	/* HEPCAPTUREAGENT_H */

