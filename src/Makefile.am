include $(top_srcdir)/config/utility.am

bin_PROGRAMS = sipxhomer

sipxhomer_CXXFLAGS = \
	-g -D_DEBUG \
	-I$(srdir)

sipxhomer_LDADD = \
	-lodbc \
        -lmyodbc5 \
	@ODBC_LDFLAGS@

noinst_HEADERS = \
	HomerMain.h

sipxhomer_SOURCES =  \
	HomerMain.cpp
