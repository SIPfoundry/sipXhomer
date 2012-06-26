#ifndef HomerProxyPlugin_H_INCLUDED
#define HomerProxyPlugin_H_INCLUDED

#include <net/SipBidirectionalProcessorPlugin.h>
#include <sqa/sqaclient.h>
#include <sipxhomer/HEPMessage.h>
#include <string>

extern "C" SipBidirectionalProcessorPlugin* getTransactionPlugin(const UtlString& pluginName);

class HomerProxyPlugin : public SipBidirectionalProcessorPlugin
{
protected:
  HomerProxyPlugin(
    const UtlString& instanceName, ///< the configured name for this plugin instance
    int priority = 100
  );

public:
  /// destructor
  virtual ~HomerProxyPlugin();

  virtual void initialize();


  /// Read (or re-read) whatever configuration the plugin requires.
  virtual void readConfig( OsConfigDb& configDb /**< a subhash of the individual configuration
                                      * parameters for this instance of this plugin. */
               );

  ///
  /// All incoming Sip Messages will be sent to this virtual function.
  /// Plugins that need to manipulate incoming Sip messages must do so here.
  virtual void handleIncoming(SipMessage& message, const char* address, int port);

  ///
  /// All outgoing Sip Messages will be sent to this virtual function.
  /// Plugins that need to manipulate outgoing Sip messages must do so here.
  virtual void handleOutgoing(SipMessage& message, const char* address, int port);

protected:
  SQAPublisher _sqa;
  std::string _localHost;
  int _localPort;
  friend SipBidirectionalProcessorPlugin* getTransactionPlugin(const UtlString& pluginName);
};

#endif // HomerProxyPlugin_H_INCLUDED
