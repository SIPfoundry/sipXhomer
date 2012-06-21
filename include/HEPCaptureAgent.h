#ifndef HEPCAPTUREAGENT_H
#define	HEPCAPTUREAGENT_H

#include "ServiceOptions.h"
#include "sqaclient.h"
#include "HEPMessage.h"
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

