AC_PREREQ(2.57)
AC_INIT(sipxhomer, 0.0.4.5.2, sipx-dev@list.sipfoundry.org)
AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE(foreign tar-ustar)
m4_include([config/general.m4])
m4_include([config/sipXlib.m4])
AC_CONFIG_SRCDIR([src/main.cpp])
AM_CONFIG_HEADER([src/config.h])
AM_DISABLE_STATIC
AC_PROG_LIBTOOL
AC_PROG_CXX
AC_PROG_CC
AX_COMPILER_VENDOR
SFAC_INIT_FLAGS
SFAC_LIB_PORT
CHECK_ODBC

# java projects - typical macros
m4_include([config/java.m4])
m4_include([config/java2.m4])
AC_PROG_JAVA_CC([javac])
AC_PROG_JAVA([java])
PROG_JAVA_DEP

AC_CONFIG_FILES([
    Makefile
    etc/Makefile
    bin/Makefile
    src/Makefile
    include/Makefile
    config-plugin/Makefile
    webhomer-auth/Makefile
])
AC_OUTPUT
