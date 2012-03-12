include $(top_srcdir)/config/utility.am
include $(top_srcdir)/config/java.am

SUBDIRS = \
	.

EXTRA_DIST = \
	$(homer_SRC) \
	$(homer_RESOURCES)

jardir = @SIPX_JAVADIR@/sipXconfig/plugins
JAR_FILE = sipxhomer-config.jar
jar_DATA = $(JAR_FILE)

homer_SRC = $(shell cd $(srcdir); find src -type f -name '*.java')
homer_RESOURCES = \
	$(shell cd $(srcdir); find plugin -type f) \
	sipxplugin.beans.xml \
	sipxhomer-plugin.properties	

homer_PKGS = \
	sipxconfig-neoconf \
	sipxconfig-web \
	commons-io \
	tapestry-annotations \
	tapestry-framework \
	org.springframework.context \
	hivemind

homer_DEPS = \
	$(call JavaDep,@SIPX_JAVADIR@/sipXcommons @SIPX_JAVADIR@/sipXconfig,$(homer_PKGS))

$(JAR_FILE) : javac-homer $(homer_RESOURCES)
	jar cf $@ \
	  $(call JarInclude,$(JAVAROOT),.) \
	  $(call JarInclude,$(srcdir),$(homer_RESOURCES))

style_SRC = $(homer_SRC)

precommit : style validate;

validate : sipxplugin.beans.xml
	xmllint --noout \
	  --schema $(SIPX_DATADIR)/schema/spring-beans-2.0.xsd  \
	$^
