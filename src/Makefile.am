include $(top_srcdir)/config/utility.am

bin_PROGRAMS = sipxhomer

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
	-lares \
	-lpthread \
	@ODBC_LDFLAGS@

sipxhomer_LDFLAGS = \
    -shared \
    -rdynamic

sipxhomer_SOURCES =  \
	main.cpp \
	HEPCaptureAgent.cpp \
	HEPTestDriver.cpp
	
#	HomerDao.cpp
