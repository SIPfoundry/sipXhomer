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

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>
#include <sipxunit/TestUtilities.h>
#include <os/OsLogger.h>
#include <string>
#include <sstream>

#include <sqa/StateQueueMessage.h>
#include "sipxhomer/HEPDao.h"
#include "sipxhomer/HEPMessage.h"

using namespace std;
using namespace resip;

class HEPDaoTest : public CppUnit::TestCase
{
   CPPUNIT_TEST_SUITE(HEPDaoTest);
   CPPUNIT_TEST(testSave);
   CPPUNIT_TEST_SUITE_END();

public:

   void testSave()
   {
     HEPDao dao;
     string url("DATABASE=homer_db;SERVER=localhost;UID=root;DRIVER=MySQL;READONLY=0;");
     dao.connect(url);

     std::string sipMessage =
          "OPTIONS sip:homer@192.168.1.11:5060;transport=tcp SIP/2.0\r\n"
          "From: sip:homertest@192.168.1.10:5060;tag=ftag12345\r\n"
          "To: sip:homer@192.168.1.11:5060;tag=ttag6789\r\n"
          "Contact: sip:homertest@192.168.1.10:5060;transport=tcp\r\n"
          "Via: SIP/2.0/TCP 192.168.1.10:5060;branch=z9hG4bK1234\r\n"
          "CSeq: 1 OPTIONS\r\n"
          "User-Agent: sipXecs\r\n"
          "Call-ID: homertest12345\r\n\r\n";

     StateQueueMessage msg;
     msg.set("IpProtoId", (int)HEPMessage::TCP);
     msg.set("Outgoing", 0);
     msg.set("Ip4SrcAddress", "192.168.1.10");
     msg.set("Ip4DestAddress", "192.168.1.11");
     msg.set("SrcPort", 5060);
     msg.set("DestPort", 5060);
     msg.set("TimeStamp", 1000);
     msg.set("TimeStampMicroOffset", (double) 1000);
     msg.set("Data", sipMessage.c_str());

     //ostringstream strm;
     //message.encode(strm);
     //HEPMessage parsed;
     //parsed.parse(strm.str());

     dao.save(msg);     

     CPPUNIT_ASSERT(true);
   }
};

CPPUNIT_TEST_SUITE_REGISTRATION(HEPDaoTest);
