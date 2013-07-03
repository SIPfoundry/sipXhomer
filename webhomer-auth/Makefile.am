include $(top_srcdir)/config/utility.am

authdir = @wwwdir@/webhomer-auth-sipx


dist_auth_DATA = \
	auth.php \
	index.html \
	settings.php

settings.php: settings.php.in
	@$(call SearchAndReplace,$<,$@)