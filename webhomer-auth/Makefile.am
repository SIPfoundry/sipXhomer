include $(top_srcdir)/config/utility.am

authdir = @wwwdir@/webhomer-auth-sipx
settingsdir = @wwwdir@/webhomer-auth-sipx

dist_auth_DATA = \
	auth.php \
	index.html
	
dist_settings_DATA = \
	settings.php	
	
EXTRA_DIST = \
	$(dist_settings_DATA:=.in)	

$(dist_settings_DATA) : % : %.in Makefile
	@$(call SearchAndReplace,$<,$@)