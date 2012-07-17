#include "ProxyPlugin.h"
#include <cassert>
#include <utl/UtlString.h>
#include <sys/time.h>
#include <sstream>
#include <net/NetBase64Codec.h>
#include "sqa/json/reader.h"
#include "sqa/json/writer.h"
#include "sqa/json/elements.h"

/// Factory used by PluginHooks to dynamically link the plugin instance
extern "C" SipBidirectionalProcessorPlugin* getTransactionPlugin(const UtlString& pluginName)
{
   return new HomerProxyPlugin(pluginName);
}

HomerProxyPlugin::HomerProxyPlugin(const UtlString& instanceName, int priority) :
  SipBidirectionalProcessorPlugin(instanceName, priority),
  _sqa("HomerProxyPlugin", 1),
  _localPort(0)
{
}

HomerProxyPlugin::~HomerProxyPlugin()
{
}

void HomerProxyPlugin::readConfig(OsConfigDb& configDb)
{
}

void HomerProxyPlugin::initialize()
{
  assert(_pUserAgent);
  UtlString host;
  int port;
  if (_pUserAgent->getLocalAddress(&host, &port))
  {
    _localHost = host.data();
    _localPort = port;
  }
}

void HomerProxyPlugin::handleIncoming(SipMessage& message, const char* address, int port)
{
  struct timeval now;
  gettimeofday(&now, NULL);

  std::string msg;
  msg = message.getBytes();

  json::Object object;
  object["IpProtoId"] = json::Number(HEPMessage::TCP);
  object["Ip4SrcAddress"] = json::String(address);
  object["Ip4DestAddress"] = json::String(_localHost.c_str());
  object["SrcPort"] = json::Number(port);
  object["DestPort"] = json::Number(_localPort);
  object["TimeStamp"] = json::Number(now.tv_sec);
  object["TimeStampMicroOffset"] = json::Number(now.tv_usec);
  object["Data"] = json::String(msg.c_str());

  try
  {
    std::ostringstream strm;
    json::Writer::Write(object, strm);
    _sqa.publish("CAP", strm.str().c_str());
  }
  catch(std::exception& error)
  {
  }

}

void HomerProxyPlugin::handleOutgoing(SipMessage& message, const char* address, int port)
{
  struct timeval now;
  gettimeofday(&now, NULL);

  std::string msg;
  msg = message.getBytes();

  json::Object object;
  object["IpProtoId"] = json::Number(HEPMessage::TCP);
  object["Ip4SrcAddress"] = json::String(_localHost.c_str());
  object["Ip4DestAddress"] = json::String(address);
  object["SrcPort"] = json::Number(_localPort);
  object["DestPort"] = json::Number(port);
  object["TimeStamp"] = json::Number(now.tv_sec);
  object["TimeStampMicroOffset"] = json::Number(now.tv_usec);
  object["Data"] = json::String(msg.c_str());

  try
  {
    std::ostringstream strm;
    json::Writer::Write(object, strm);
    _sqa.publish("CAP", strm.str().c_str());
  }
  catch(std::exception& error)
  {
  }
}


