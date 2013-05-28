include $(top_srcdir)/config/utility.am

libexecdir = $(SIPX_LIBEXECDIR)
libexec_PROGRAMS = sipxhomer

check_PROGRAMS = test_HEPDao

sipxhomer_CXXFLAGS = \
	-DSIPX_LOGDIR=\"@SIPX_LOGDIR@\" \
	-DSIPX_CONFDIR=\"@SIPX_CONFDIR@\" \
	-I$(top_srcdir)/include

sipxhomer_LDADD = \
	-lsqaclient \
	-lodbc \
	-lmyodbc5 \
	-lresip \
	-lrutil \
	-lssl \
	-lcrypto \
	-lpthread \
	-lmongoclient \
	@ODBC_LDFLAGS@ \
	@SIPXPORT_LIBS@ \
	@SIPXCOMMSERVER_LIBS@ \
	@SIPXSQA_LIBS@


sipxhomer_LDFLAGS = \
    -shared \
    -rdynamic

sipxhomer_SOURCES =  \
	main.cpp \
	HEPCaptureAgent.cpp \
	HEPTestDriver.cpp \
	HEPDao.cpp

test_HEPDao_CXXFLAGS = \
	$(sipxhomer_CXXFLAGS) \
	-ggdb -O0

test_HEPDao_LDADD = \
	$(sipxhomer_LDADD) \
	@SIPXPORT_LIBS@ \
	@SIPXUNIT_LIBS@

test_HEPDao_SOURCES = \
	HEPDao.cpp \
	HEPDaoTest.cpp

