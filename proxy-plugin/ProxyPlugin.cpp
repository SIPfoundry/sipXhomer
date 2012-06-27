#include "ProxyPlugin.h"
#include <cassert>
#include <utl/UtlString.h>
#include <sys/time.h>
#include <sstream>
#include <net/NetBase64Codec.h>

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
  HEPMessage hep;
  hep.setIpProtoFamily(HEPMessage::IpV4);
  hep.setIpProtoId(HEPMessage::TCP);
  hep.setIp4SrcAddress(address);
  hep.setIp4DestAddress(_localHost.c_str());
  hep.setSrcPort(port);
  hep.setDestPort(_localPort);
  hep.setTimeStamp(now.tv_sec);
  hep.setTimeStampMicroOffset(now.tv_usec);
  hep.setProtocolType(HEPMessage::SIP);
  hep.setData(msg);

  std::ostringstream buff;
  hep.encode(buff);
  _sqa.publish("CAP", buff.str().data(), buff.str().size());
}

void HomerProxyPlugin::handleOutgoing(SipMessage& message, const char* address, int port)
{
  struct timeval now;
  gettimeofday(&now, NULL);

  std::string msg;
  msg = message.getBytes();
  HEPMessage hep;
  hep.setIpProtoFamily(HEPMessage::IpV4);
  hep.setIpProtoId(HEPMessage::TCP);
  hep.setIp4SrcAddress(_localHost.c_str());
  hep.setIp4DestAddress(address);
  hep.setSrcPort(_localPort);
  hep.setDestPort(port);
  hep.setTimeStamp(now.tv_sec);
  hep.setTimeStampMicroOffset(now.tv_usec);
  hep.setProtocolType(HEPMessage::SIP);
  hep.setData(msg);

  std::ostringstream buff;
  hep.encode(buff);
  _sqa.publish("CAP", buff.str().data(), buff.str().size());
}


