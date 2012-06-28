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

#if 0
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


  UtlString hepMsq;
  NetBase64Codec::encode(buff.str().size(), buff.str().data(), hepMsq);
  _sqa.publish("CAP", hepMsq.data());
#else

  json::Object object;
  object["IpProtoFamily"] = json::Number(HEPMessage::IpV4);
  object["IpProtoId"] = json::Number(HEPMessage::TCP);
  object["Ip4SrcAddress"] = json::String(address);
  object["Ip4DestAddress"] = json::String(_localHost.c_str());
  object["SrcPort"] = json::Number(port);
  object["DestPort"] = json::Number(_localPort);
  object["TimeStamp"] = json::Number(now.tv_sec);
  object["ProtocolType"] = json::Number(HEPMessage::SIP);
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

#endif

}

void HomerProxyPlugin::handleOutgoing(SipMessage& message, const char* address, int port)
{
  struct timeval now;
  gettimeofday(&now, NULL);

  std::string msg;
  msg = message.getBytes();

#if 0
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

  UtlString hepMsq;
  NetBase64Codec::encode(buff.str().size(), buff.str().data(), hepMsq);
  _sqa.publish("CAP", hepMsq.data());
#else
  json::Object object;
  object["IpProtoFamily"] = json::Number(HEPMessage::IpV4);
  object["IpProtoId"] = json::Number(HEPMessage::TCP);
  object["Ip4SrcAddress"] = json::String(_localHost.c_str());
  object["Ip4DestAddress"] = json::String(address);
  object["SrcPort"] = json::Number(_localPort);
  object["DestPort"] = json::Number(port);
  object["TimeStamp"] = json::Number(now.tv_sec);
  object["ProtocolType"] = json::Number(HEPMessage::SIP);
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
#endif
}


