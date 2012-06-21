#include "sipxhomer/HEPTestDriver.h"
#include "sipxhomer/HEPMessage.h"
#include "sqa/UnitTest.h"
#include <string>
#include <sstream>
#include "resip/stack/SipMessage.hxx"

//
// DEFINE_UNIT_TEST - Define a Test Group.  Must be called prior to DEFINE_TEST
// DEFINE_TEST - Define a new unit test belonging to a defined group
// DEFINE_RESOURCE - Register a resource that is accessible to unit tests in the same group
// GET_RESOURCE - Get the value of the resource that was previously created by DEFINE_RESOURCE
// ASSERT_COND(cond) - Assert if the logical condition is false
// ASSERT_STR_EQ(var1, var2) - Assert that two strings are  equal
// ASSERT_STR_CASELESS_EQ(var1, var2) - Assert that two strings are equal but ignoring case comparison
// ASSERT_STR_NEQ(var1, var2) - Asserts that two strings are not eual
// ASSERT_EQ(var1, var2) - Asserts that the two values are equal
// ASSERT_NEQ(var1, var2) - Asserts that the the values are not equal
// ASSERT_LT(var1, var2) - Asserts that the value var1 is less than value of var2
// ASSERT_GT(var1, var2)  Asserts that the value var1 is greater than value of var2
//

DEFINE_UNIT_TEST(TestDriver);

DEFINE_TEST(TestDriver, TestHEPParser)
{
  using namespace resip;
  
  std::string sipMessage =
          "OPTIONS sip:homer@192.168.1.11:5060;transport=tcp SIP/2.0\r\n"
          "From: sip:homertest@192.168.1.10:5060;tag=ftag12345\r\n"
          "To: sip:homer@192.168.1.11:5060;tag=ttag6789\r\n"
          "Contact: sip:homertest@192.168.1.10:5060;transport=tcp\r\n"
          "Via: SIP/2.0/TCP 192.168.1.10:5060;branch=z9hG4bK1234\r\n"
          "CSeq: 1 OPTIONS\r\n"
          "User-Agent: sipXecs\r\n"
          "Call-ID: homertest12345\r\n\r\n";

  HEPMessage message;
  message.setIpProtoFamily(HEPMessage::IpV4);
  message.setIpProtoId(HEPMessage::TCP);
  message.setIp4SrcAddress("192.168.1.10");
  message.setIp4DestAddress("192.168.1.11");
  message.setSrcPort(5060);
  message.setDestPort(5060);
  message.setTimeStamp(1000);
  message.setTimeStampMicroOffset(1000);
  message.setProtocolType(HEPMessage::SIP);
  message.setData(sipMessage);

  std::ostringstream strm;
  message.encode(strm);

  HEPMessage parsed;
  ASSERT_COND(parsed.parse(strm.str()));
  ASSERT_EQ(parsed.getIpProtoFamily(), HEPMessage::IpV4);
  ASSERT_EQ(parsed.getIpProtoId(), HEPMessage::TCP);
  ASSERT_STR_EQ(parsed.getIp4SrcAddress().to_string().c_str(), "192.168.1.10");
  ASSERT_STR_EQ(parsed.getIp4DestAddress().to_string().c_str(), "192.168.1.11");
  ASSERT_EQ(parsed.getSrcPort(), 5060);
  ASSERT_EQ(parsed.getDestPort(), 5060);
  ASSERT_EQ(parsed.getTimeStamp(), 1000);
  ASSERT_EQ(parsed.getTimeStampMicroOffset(), 1000);
  ASSERT_EQ(parsed.getProtocolType(), HEPMessage::SIP);
  ASSERT_STR_EQ(parsed.getData().c_str(), sipMessage.c_str());



  Data buffer(parsed.getData().c_str());
  SipMessage* msg = SipMessage::make(buffer);
  ASSERT_COND(msg != 0);
  ASSERT_COND(!msg->isInvalid());
  if (msg && !msg->isInvalid())
  {
    NameAddr remoteNameAddr;
    NameAddr localNameAddr;
    std::string fromTag;
    std::string toTag;
    std::string fromUser;
    std::string toUser;

    ASSERT_COND(msg->exists(h_To));
    if (msg->exists(h_To))
    {
      toUser = msg->const_header(h_To).uri().user().c_str();
      toTag = msg->const_header(h_To).exists(p_tag) ? msg->const_header(h_To).param(p_tag).c_str() : std::string();
      ASSERT_STR_EQ(toUser.c_str(), "homer");
      ASSERT_STR_EQ(toTag.c_str(), "ttag6789");
    }

    ASSERT_COND(msg->exists(h_From));
    if (msg->exists(h_From))
    {
      fromUser = msg->const_header(h_From).uri().user().c_str();
      fromTag = msg->const_header(h_From).exists(p_tag) ? msg->const_header(h_From).param(p_tag).c_str() : std::string();

      ASSERT_STR_EQ(fromUser.c_str(), "homertest");
      ASSERT_STR_EQ(fromTag.c_str(), "ftag12345");
    }

    std::string requestLine;
    std::string statusLine;
    std::string requestUriUser;
    
    ASSERT_COND(msg->isRequest());
    if (msg->isRequest())
    {
      std::ostringstream strm;
      msg->const_header(h_RequestLine).uri().encode(strm);
      requestLine = strm.str();
      requestUriUser = msg->const_header(h_RequestLine).uri().user().c_str();
    }
    else
    {
      std::ostringstream strm;
      msg->const_header(h_StatusLine).encode(strm);
      statusLine = strm.str();
    }

    std::string callId;
    ASSERT_COND(msg->exists(h_CallID));
    if (msg->exists(h_CallID))
    {
      callId = msg->const_header(h_CallID).value().c_str();
    }
    ASSERT_STR_EQ(callId.c_str(), "homertest12345");

    std::string viaBranch;
    std::string via;
    ASSERT_COND(msg->exists(h_Vias));
    if (msg->exists(h_Vias))
    {
      Via& frontVia = msg->header(h_Vias).front();
      std::ostringstream viaStrm;
      frontVia.encode(viaStrm);
      via = viaStrm.str();

      if (frontVia.param(p_branch).hasMagicCookie())
        viaBranch = "z9hG4bK";
      viaBranch += frontVia.param(p_branch).getTransactionId().c_str();
    }
    ASSERT_STR_EQ(viaBranch.c_str(), "z9hG4bK1234");

    std::string contactUser;
    std::string contactHost;
    int contactPort = 0;
    ASSERT_COND(msg->exists(h_Contacts));
    if (msg->exists(h_Contacts))
    {
      contactUser = msg->const_header(h_Contacts).front().uri().user().c_str();
      contactHost = msg->const_header(h_Contacts).front().uri().host().c_str();
      contactPort = msg->const_header(h_Contacts).front().uri().port();
    }
    ASSERT_STR_EQ(contactUser.c_str(), "homertest");
    ASSERT_STR_EQ(contactHost.c_str(), "192.168.1.10");
    ASSERT_EQ(contactPort, 5060);

    std::string pidentity;
    if (msg->exists(h_PAssertedIdentities))
    {
      std::ostringstream pidStrm;
      msg->const_header(h_PAssertedIdentities).front().uri().encode(pidStrm);
      pidentity = pidStrm.str();
    }

    std::string cseq;
    std::ostringstream cseqStrm;
    int cseqNumber = 0;
    std::string cseqMethod;

    if (msg->exists(h_CSeq))
    {
      msg->const_header(h_CSeq).encode(cseqStrm);
      cseq = cseqStrm.str();
      cseqNumber = msg->const_header(h_CSeq).sequence();
      switch(msg->const_header(h_CSeq).method())
      {
        case ACK:
          cseqMethod = "ACK";
          break;
        case BYE:
          cseqMethod = "BYE";
          break;
        case CANCEL:
          cseqMethod = "CANCEL";
          break;
        case INVITE:
          cseqMethod = "INVITE";
          break;
        case NOTIFY:
          cseqMethod = "NOTIFY";
          break;
        case OPTIONS:
          cseqMethod = "OPTIONS";
          break;
        case REFER:
          cseqMethod = "REFER";
          break;
        case REGISTER:
          cseqMethod = "REGISTER";
          break;
        case SUBSCRIBE:
          cseqMethod = "SUBSCRIBE";
          break;
        case MESSAGE:
          cseqMethod = "MESSAGE";
          break;
        case INFO:
          cseqMethod = "INFO";
          break;
        case PRACK:
          cseqMethod = "PRACK";
          break;
        case PUBLISH:
          cseqMethod = "PUBLISH";
          break;
        case SERVICE:
          cseqMethod = "SERVICE";
          break;
        case UPDATE:
          cseqMethod = "UPDATE";
          break;
      }
    }

    ASSERT_STR_EQ(cseqMethod.c_str(), "OPTIONS");
    ASSERT_EQ(cseqNumber, 1);

    std::string reason;
    if (msg->exists(h_Reasons))
    {
      reason = msg->const_header(h_Reasons).front().value().c_str();
    }

    std::string authorization;
    if (msg->exists(h_Authorizations))
    {
      std::ostringstream authStrm;
      msg->const_header(h_Authorizations).front().encode(authStrm);
      authorization = authStrm.str();
    }
    else if (msg->exists(h_ProxyAuthorizations))
    {
      std::ostringstream authStrm;
      msg->const_header(h_ProxyAuthorizations).front().encode(authStrm);
      authorization = authStrm.str();
    }

    std::string contentType;
    if (msg->exists(h_ContentType))
    {
      std::ostringstream ctypeStrm;
      msg->const_header(h_ContentType).encode(ctypeStrm);
      contentType = ctypeStrm.str();
    }

    std::string userAgent;
    if (msg->exists(h_UserAgent))
    {
      userAgent = msg->const_header(h_UserAgent).value().c_str();
    }
  }
  delete msg;
}

HEPTestDriver::HEPTestDriver(HEPCaptureAgent& agent) :
  _agent(agent)
{
}

HEPTestDriver::~HEPTestDriver()
{
}

bool HEPTestDriver::runTests()
{
  DEFINE_RESOURCE(TestDriver, "HEPCaptureAgent", &_agent);
  VERIFY_TEST(TestDriver, TestHEPParser);
  END_UNIT_TEST(TestDriver);
  return TEST_RESULT(TestDriver);
}
