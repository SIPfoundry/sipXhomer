
#ifndef SQACLIENT_H
#define	SQACLIENT_H

#ifndef EXCLUDE_SQA_INLINES
#include "StateQueueClient.h"
#include <boost/lexical_cast.hpp>
#endif

#include <map>
#include <vector>
#include <string>

#ifdef SWIG
%module sqaclient
%{
#include "sqaclient.h"
%}
%newobject SQAWatcher::watch();
%newobject *::get;
%newobject *::mget;
%newobject SQAWorker::fetchTask();

%include "std_vector.i"
%include "std_string.i"
%include "std_map.i"

namespace std
{
   %template(StringToStringMap) map<string, string>;
}

#endif

class SQAEvent
{
public:
  SQAEvent();
  SQAEvent(const SQAEvent& data);
  ~SQAEvent();

  char* id;
  char* data;
};

class SQAWatcher
{
public:
  SQAWatcher(
    const char* applicationId, // Unique application ID that will identify this watcher to SQA
    const char* serviceAddress, // The IP address of the SQA
    const char* servicePort, // The port where SQA is listening for connections
    const char* eventId, // Event ID of the event being watched. Example: "sqa.not"
    int poolSize // Number of active connections to SQA
  );

  ~SQAWatcher();

  //
  // Returns true if the client is connected to SQA
  //
  bool isConnected();

  //
  // Returns the next event published by SQA.  This Function
  // will block if there is no event in queue
  //
  SQAEvent* watch();

  //
  // Set a value in the event queue workspace
  //
  void set(int workspace, const char* name, const char* data, int expires);
  //
  // Get a value from the event queue workspace
  //
  char* get(int workspace, const char* name);
  //
  // Set the value of a map item.  If the item or map does not exist
  // they will be created
  //
  void mset(int workspace, const char* mapId, const char* dataId, const char* data, int expires);
  //
  // Return a particular map element
  //
  char* mget(int workspace, const char* mapId, const char* dataId);
  //
  // Increment the value of an integer belong to a map
  //
  bool mgeti(int workspace, const char* mapId, const char* dataId, int& data);
  //
  // Get a map of string to string values stored in sqa
  //
  std::map<std::string, std::string> mgetAll(int workspace, const char* name);
private:
  SQAWatcher(const SQAWatcher& copy);
  uintptr_t _connection;
};

class SQAPublisher
{
public:
  SQAPublisher(
    const char* applicationId, // Unique application ID that will identify this watcher to SQA
    const char* serviceAddress, // The IP address of the SQA
    const char* servicePort, // The port where SQA is listening for connections
    int poolSize // Number of active connections to SQA
  );

  ~SQAPublisher();

  bool isConnected();

  bool publish(const char* id, const char* data);

  bool publishAndPersist(int workspace, const char* id, const char* data, int expires);

  //
  // Set a value in the event queue workspace
  //
  void set(int workspace, const char* name, const char* data, int expires);
  //
  // Get a value from the event queue workspace
  //
  char* get(int workspace, const char* name);
  //
  // Set the value of a map item.  If the item or map does not exist
  // they will be created
  //
  void mset(int workspace, const char* mapId, const char* dataId, const char* data, int expires);
  //
  // Return a particular map element
  //
  char* mget(int workspace, const char* mapId, const char* dataId);
  //
  // Increment the value of an integer belong to a map
  //
  bool mgeti(int workspace, const char* mapId, const char* dataId, int& data);
  //
  // Get a map of string to string values stored in sqa
  //
  std::map<std::string, std::string> mgetAll(int workspace, const char* name);
private:
  SQAPublisher(const SQAPublisher& copy);
  uintptr_t _connection;
};

class SQAWorker
{
public:
  SQAWorker(
    const char* applicationId, // Unique application ID that will identify this watcher to SQA
    const char* serviceAddress, // The IP address of the SQA
    const char* servicePort, // The port where SQA is listening for connections
    const char* eventId, // Event ID of the event being watched. Example: "sqa.not"
    int poolSize // Number of active connections to SQA
  );

  ~SQAWorker();

  //
  // Returns true if the client is connected to SQA
  //
  bool isConnected();

  //
  // Returns the next event published by SQA.  This Function
  // will block if there is no event in queue
  //
  SQAEvent* fetchTask();
  //
  // Set a value in the event queue workspace
  //
  void set(int workspace, const char* name, const char* data, int expires);
  //
  // Get a value from the event queue workspace
  //
  char* get(int workspace, const char* name);
  //
  // Set the value of a map item.  If the item or map does not exist
  // they will be created
  //
  void mset(int workspace, const char* mapId, const char* dataId, const char* data, int expires);
  //
  // Return a particular map element
  //
  char* mget(int workspace, const char* mapId, const char* dataId);
  //
  // Increment the value of an integer belong to a map
  //
  bool mgeti(int workspace, const char* mapId, const char* dataId, int& data);
  //
  // Get a map of string to string values stored in sqa
  //
  std::map<std::string, std::string> mgetAll(int workspace, const char* name);

private:
  SQAWorker(const SQAWorker& copy);
  uintptr_t _connection;
};

class SQADealer
{
public:
  SQADealer(
    const char* applicationId, // Unique application ID that will identify this watcher to SQA
    const char* serviceAddress, // The IP address of the SQA
    const char* servicePort, // The port where SQA is listening for connections
    const char* eventId, // Event ID of the event being watched. Example: "sqa.not"
    int poolSize // Number of active connections to SQA
  );

  ~SQADealer();

  //
  // Returns true if the client is connected to SQA
  //
  bool isConnected();


  //
  // Deal a new task
  //
  bool deal(const char* data, int expires);

  //
  // Deal and publish a task
  //
  bool dealAndPublish(const char* data, int expires);

  //
  // Set a value in the event queue workspace
  //
  void set(int workspace, const char* name, const char* data, int expires);
  //
  // Get a value from the event queue workspace
  //
  char* get(int workspace, const char* name);
  //
  // Set the value of a map item.  If the item or map does not exist
  // they will be created
  //
  void mset(int workspace, const char* mapId, const char* dataId, const char* data, int expires);
  //
  // Return a particular map element
  //
  char* mget(int workspace, const char* mapId, const char* dataId);
  //
  // Increment the value of an integer belong to a map
  //
  bool mgeti(int workspace, const char* mapId, const char* dataId, int& data);
  //
  // Get a map of string to string values stored in sqa
  //
  std::map<std::string, std::string> mgetAll(int workspace, const char* name);
private:
  SQADealer(const SQADealer& copy);
  uintptr_t _connection;
};


#ifndef EXCLUDE_SQA_INLINES
//
// Inline implementation of SQAEvent class
//
inline SQAEvent::SQAEvent() :
  id(0),
  data(0)
{
}

inline SQAEvent::~SQAEvent()
{
  free(id);
  free(data);
}

inline SQAEvent::SQAEvent(const SQAEvent& ev)
{
  id = (char*)malloc(strlen(ev.id) + 1);
  ::memset(id, 0x00, strlen(ev.id) + 1);
  ::memcpy(id, ev.id, strlen(ev.id) + 1);

  data = (char*)malloc(strlen(ev.data) + 1);
  ::memset(data, 0x00, strlen(ev.data) + 1);
  ::memcpy(data, ev.data, strlen(ev.data) + 1);
}

//
// Inline implementation for SQAWatcher class
//
inline SQAWatcher::SQAWatcher(
  const char* applicationId, // Unique application ID that will identify this watcher to SQA
  const char* serviceAddress, // The IP address of the SQA
  const char* servicePort, // The port where SQA is listening for connections
  const char* eventId, // Event ID of the event being watched. Example: "reg"
  int poolSize // Number of active connections to SQA
)
{
  _connection = (uintptr_t)(new StateQueueClient(
          StateQueueClient::Watcher,
          applicationId,
          serviceAddress,
          servicePort,
          eventId,
          poolSize));
}

inline SQAWatcher::SQAWatcher(const SQAWatcher& copy)
{
}

inline SQAWatcher::~SQAWatcher()
{
  delete reinterpret_cast<StateQueueClient*>(_connection);
}

inline bool SQAWatcher::isConnected()
{
  return reinterpret_cast<StateQueueClient*>(_connection)->isConnected();
}

inline SQAEvent* SQAWatcher::watch()
{
  std::string id;
  std::string data;
  if (!reinterpret_cast<StateQueueClient*>(_connection)->watch(id, data))
    return 0;

  SQAEvent* pEvent = new SQAEvent();
  pEvent->id = (char*)malloc(id.size() + 1);
  ::memset(pEvent->id, 0x00, id.size() + 1);
  ::memcpy(pEvent->id, id.c_str(), id.size() + 1);
  pEvent->data = (char*)malloc(data.size() + 1);
  ::memset(pEvent->data, 0x00, data.size() + 1);
  ::memcpy(pEvent->data, data.c_str(), data.size() + 1);
  return pEvent;
}

//
  // Set a value in the event queue workspace
  //
inline void SQAWatcher::set(int workspace, const char* name, const char* data, int expires)
{
  reinterpret_cast<StateQueueClient*>(_connection)->set(workspace, name, data, expires);
}
  //
  // Get a value from the event queue workspace
  //
inline char* SQAWatcher::get(int workspace, const char* name)
{
  std::string data;
  if (!reinterpret_cast<StateQueueClient*>(_connection)->get(workspace, name, data))
    return 0;
  char* buff = (char*)malloc(data.size() + 1);
  ::memset(buff, 0x00, data.size() + 1);
  ::memcpy(buff, data.c_str(), data.size() + 1);
  return buff;
}

inline void SQAWatcher::mset(int workspace, const char* mapId, const char* dataId, const char* data, int expires)
{
  reinterpret_cast<StateQueueClient*>(_connection)->mset(workspace, mapId, dataId, data, expires);
}

inline char* SQAWatcher::mget(int workspace, const char* mapId, const char* dataId)
{
  std::string data;
  if (!reinterpret_cast<StateQueueClient*>(_connection)->mget(workspace, mapId, dataId, data))
    return 0;
  char* buff = (char*)malloc(data.size() + 1);
  ::memset(buff, 0x00, data.size() + 1);
  ::memcpy(buff, data.c_str(), data.size() + 1);
  return buff;
}

inline bool SQAWatcher::mgeti(int workspace, const char* mapId, const char* dataId, int& incremented)
{
  std::string data;
  if (!reinterpret_cast<StateQueueClient*>(_connection)->mgeti(workspace, mapId, dataId, data))
    return 0;
  try
  {
    incremented = boost::lexical_cast<int>(data);
    return true;
  }
  catch(...)
  {
    return false;
  }
}

inline std::map<std::string, std::string> SQAWatcher::mgetAll(int workspace, const char* name)
{
  std::map<std::string, std::string> smap;
  if (!reinterpret_cast<StateQueueClient*>(_connection)->mgetm(workspace, name, smap));
  return smap;
}

//
// Inline implmentation of the SQA Publisher class
//
inline SQAPublisher::SQAPublisher(
  const char* applicationId, // Unique application ID that will identify this watcher to SQA
  const char* serviceAddress, // The IP address of the SQA
  const char* servicePort, // The port where SQA is listening for connections
  int poolSize // Number of active connections to SQA
)
{
  _connection = (uintptr_t)(new StateQueueClient(
          StateQueueClient::Publisher,
          applicationId,
          serviceAddress,
          servicePort,
          "publisher",
          poolSize));
}

inline SQAPublisher::SQAPublisher(const SQAPublisher& copy)
{
}

inline SQAPublisher::~SQAPublisher()
{
  delete reinterpret_cast<StateQueueClient*>(_connection);
}

inline bool SQAPublisher::isConnected()
{
  return reinterpret_cast<StateQueueClient*>(_connection)->isConnected();
}

inline bool SQAPublisher::publish(const char* id, const char* data)
{
  return reinterpret_cast<StateQueueClient*>(_connection)->publish(id, data);
}

inline bool SQAPublisher::publishAndPersist(int workspace, const char* id, const char* data, int expires)
{
  return reinterpret_cast<StateQueueClient*>(_connection)->publishAndPersist(workspace, id, data, expires);
}

inline void SQAPublisher::set(int workspace, const char* name, const char* data, int expires)
{
  reinterpret_cast<StateQueueClient*>(_connection)->set(workspace, name, data, expires);
}
  //
  // Get a value from the event queue workspace
  //
inline char* SQAPublisher::get(int workspace, const char* name)
{
  std::string data;
  if (!reinterpret_cast<StateQueueClient*>(_connection)->get(workspace, name, data))
    return 0;
  char* buff = (char*)malloc(data.size() + 1);
  ::memset(buff, 0x00, data.size() + 1);
  ::memcpy(buff, data.c_str(), data.size() + 1);
  return buff;
}

inline void SQAPublisher::mset(int workspace, const char* mapId, const char* dataId, const char* data, int expires)
{
  reinterpret_cast<StateQueueClient*>(_connection)->mset(workspace, mapId, dataId, data, expires);
}

inline char* SQAPublisher::mget(int workspace, const char* mapId, const char* dataId)
{
  std::string data;
  if (!reinterpret_cast<StateQueueClient*>(_connection)->mget(workspace, mapId, dataId, data))
    return 0;
  char* buff = (char*)malloc(data.size() + 1);
  ::memset(buff, 0x00, data.size() + 1);
  ::memcpy(buff, data.c_str(), data.size() + 1);
  return buff;
}

inline bool SQAPublisher::mgeti(int workspace, const char* mapId, const char* dataId, int& incremented)
{
  std::string data;
  if (!reinterpret_cast<StateQueueClient*>(_connection)->mgeti(workspace, mapId, dataId, data))
    return 0;
  try
  {
    incremented = boost::lexical_cast<int>(data);
    return true;
  }
  catch(...)
  {
    return false;
  }
}

inline std::map<std::string, std::string> SQAPublisher::mgetAll(int workspace, const char* name)
{
  std::map<std::string, std::string> smap;
  if (!reinterpret_cast<StateQueueClient*>(_connection)->mgetm(workspace, name, smap));
  return smap;
}
//
// Inline implementation for SQA Dealer class
//
inline SQADealer::SQADealer(
  const char* applicationId, // Unique application ID that will identify this watcher to SQA
  const char* serviceAddress, // The IP address of the SQA
  const char* servicePort, // The port where SQA is listening for connections
  const char* eventId, // Event ID of the event being watched. Example: "sqa.not"
  int poolSize // Number of active connections to SQA
)
{
  _connection = (uintptr_t)(new StateQueueClient(
          StateQueueClient::Publisher,
          applicationId,
          serviceAddress,
          servicePort,
          eventId,
          poolSize));
}

inline SQADealer::SQADealer(const SQADealer& copy)
{
}

inline SQADealer::~SQADealer()
{
  delete reinterpret_cast<StateQueueClient*>(_connection);
}

inline bool SQADealer::isConnected()
{
  return reinterpret_cast<StateQueueClient*>(_connection)->isConnected();
}

inline bool SQADealer::deal(const char* data, int expires)
{
  return reinterpret_cast<StateQueueClient*>(_connection)->enqueue(data, expires);
}

inline bool SQADealer::dealAndPublish(const char* data, int expires)
{
  return reinterpret_cast<StateQueueClient*>(_connection)->enqueue(data, expires, true);
}

inline void SQADealer::set(int workspace, const char* name, const char* data, int expires)
{
  reinterpret_cast<StateQueueClient*>(_connection)->set(workspace, name, data, expires);
}
  //
  // Get a value from the event queue workspace
  //
inline char* SQADealer::get(int workspace, const char* name)
{
  std::string data;
  if (!reinterpret_cast<StateQueueClient*>(_connection)->get(workspace, name, data))
    return 0;
  char* buff = (char*)malloc(data.size() + 1);
  ::memset(buff, 0x00, data.size() + 1);
  ::memcpy(buff, data.c_str(), data.size() + 1);
  return buff;
}

inline void SQADealer::mset(int workspace, const char* mapId, const char* dataId, const char* data, int expires)
{
  reinterpret_cast<StateQueueClient*>(_connection)->mset(workspace, mapId, dataId, data, expires);
}

inline char* SQADealer::mget(int workspace, const char* mapId, const char* dataId)
{
  std::string data;
  if (!reinterpret_cast<StateQueueClient*>(_connection)->mget(workspace, mapId, dataId, data))
    return 0;
  char* buff = (char*)malloc(data.size() + 1);
  ::memset(buff, 0x00, data.size() + 1);
  ::memcpy(buff, data.c_str(), data.size() + 1);
  return buff;
}

inline bool SQADealer::mgeti(int workspace, const char* mapId, const char* dataId, int& incremented)
{
  std::string data;
  if (!reinterpret_cast<StateQueueClient*>(_connection)->mgeti(workspace, mapId, dataId, data))
    return 0;
  try
  {
    incremented = boost::lexical_cast<int>(data);
    return true;
  }
  catch(...)
  {
    return false;
  }
}

inline std::map<std::string, std::string> SQADealer::mgetAll(int workspace, const char* name)
{
  std::map<std::string, std::string> smap;
  if (!reinterpret_cast<StateQueueClient*>(_connection)->mgetm(workspace, name, smap));
  return smap;
}

//
// Inline implementation for SQAWorker class
//
inline SQAWorker::SQAWorker(
  const char* applicationId, // Unique application ID that will identify this watcher to SQA
  const char* serviceAddress, // The IP address of the SQA
  const char* servicePort, // The port where SQA is listening for connections
  const char* eventId, // Event ID of the event being watched. Example: "sqa.not"
  int poolSize // Number of active connections to SQA
)
{
  _connection = (uintptr_t)(new StateQueueClient(
          StateQueueClient::Worker,
          applicationId,
          serviceAddress,
          servicePort,
          eventId,
          poolSize));
}

inline SQAWorker::SQAWorker(const SQAWorker& copy)
{
}

inline SQAWorker::~SQAWorker()
{
  delete reinterpret_cast<StateQueueClient*>(_connection);
}

inline bool SQAWorker::isConnected()
{
  return reinterpret_cast<StateQueueClient*>(_connection)->isConnected();
}

inline SQAEvent* SQAWorker::fetchTask()
{
  std::string id;
  std::string data;
  if (!reinterpret_cast<StateQueueClient*>(_connection)->pop(id, data))
    return 0;

  SQAEvent* pEvent = new SQAEvent();
  pEvent->id = (char*)malloc(id.size() + 1);
  ::memset(pEvent->id, 0x00, id.size() + 1);
  ::memcpy(pEvent->id, id.c_str(), id.size() + 1);
  pEvent->data = (char*)malloc(data.size() + 1);
  ::memset(pEvent->data, 0x00, data.size() + 1);
  ::memcpy(pEvent->data, data.c_str(), data.size() + 1);
  return pEvent;
}

inline void SQAWorker::set(int workspace, const char* name, const char* data, int expires)
{
  reinterpret_cast<StateQueueClient*>(_connection)->set(workspace, name, data, expires);
}
  //
  // Get a value from the event queue workspace
  //
inline char* SQAWorker::get(int workspace, const char* name)
{
  std::string data;
  if (!reinterpret_cast<StateQueueClient*>(_connection)->get(workspace, name, data))
    return 0;
  char* buff = (char*)malloc(data.size() + 1);
  ::memset(buff, 0x00, data.size() + 1);
  ::memcpy(buff, data.c_str(), data.size() + 1);
  return buff;
}

inline void SQAWorker::mset(int workspace, const char* mapId, const char* dataId, const char* data, int expires)
{
  reinterpret_cast<StateQueueClient*>(_connection)->mset(workspace, mapId, dataId, data, expires);
}

inline char* SQAWorker::mget(int workspace, const char* mapId, const char* dataId)
{
  std::string data;
  if (!reinterpret_cast<StateQueueClient*>(_connection)->mget(workspace, mapId, dataId, data))
    return 0;
  char* buff = (char*)malloc(data.size() + 1);
  ::memset(buff, 0x00, data.size() + 1);
  ::memcpy(buff, data.c_str(), data.size() + 1);
  return buff;
}

inline bool SQAWorker::mgeti(int workspace, const char* mapId, const char* dataId, int& incremented)
{
  std::string data;
  if (!reinterpret_cast<StateQueueClient*>(_connection)->mgeti(workspace, mapId, dataId, data))
    return 0;
  try
  {
    incremented = boost::lexical_cast<int>(data);
    return true;
  }
  catch(...)
  {
    return false;
  }
}

inline std::map<std::string, std::string> SQAWorker::mgetAll(int workspace, const char* name)
{
  std::map<std::string, std::string> smap;
  if (!reinterpret_cast<StateQueueClient*>(_connection)->mgetm(workspace, name, smap));
  return smap;
}

#endif //EXCLUDE_SQA_INLINES

#endif	/* SQACLIENT_H */

