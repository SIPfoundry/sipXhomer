
include $(top_srcdir)/config/project.am
include $(top_srcdir)/config/utility.am
## Process this file with automake to produce Makefile.in

sipXhomerProxyPlugin_COMMON_CXX_FLAGS = \
	-I$(top_builddir)/config \
	-I$(top_srcdir)/include

## Libtool version info - update only immediately before a release
# Current - increment if any interfaces are added, removed, or changed
version_Current=1
# Revision - set to 0 if Current was incremented
#            increment if any implementation has changed
version_Revision=0
# Age - set to 0 of any interfaces have been removed
#       increment if any interfaces have been added
version_Age=0

sipXhomerProxyPlugin_COMMON_LD_FLAGS = \
    -module \
    -version-info ${version_Current}:${version_Revision}:${version_Age}

sipXhomerProxyPlugin_LDADD = \
	-lsqaclient

nobase_lib_LTLIBRARIES = \
    transactionplugins/libsipXhomerProxyPlugin.la

transactionplugins_libsipXhomerProxyPlugin_la_SOURCES = \
	ProxyPlugin.h \
	ProxyPlugin.cpp

transactionplugins_libsipXhomerProxyPlugin_la_CXXFLAGS = \
    $(sipXhomerProxyPlugin_COMMON_CXX_FLAGS) \
    -DSIPX_CONFDIR=\"@SIPX_CONFDIR@\"

transactionplugins_libsipXhomerProxyPlugin_la_LDFLAGS = \
    $(sipXhomerProxyPlugin_COMMON_LD_FLAGS)


transactionplugins_libsipXhomerProxyPlugin_la_LIBADD = \
	@SIPXCOMMSERVER_LIBS@
	




