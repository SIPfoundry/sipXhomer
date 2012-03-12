/*
 * Copyright (C) 2012 eZuce Inc., certain elements licensed under a Contributor Agreement.
 * Contributors retain copyright to elements licensed under a Contributor Agreement.
 * Licensed to the User under the AGPL license.
 *
 * $
 */
package org.sipfoundry.sipxconfig.web.plugin;

import org.apache.tapestry.annotations.Bean;
import org.apache.tapestry.annotations.InjectObject;
import org.apache.tapestry.event.PageBeginRenderListener;
import org.apache.tapestry.event.PageEvent;
import org.sipfoundry.sipxconfig.components.PageWithCallback;
import org.sipfoundry.sipxconfig.components.SipxValidationDelegate;
import org.sipfoundry.sipxconfig.homer.Homer;
import org.sipfoundry.sipxconfig.homer.HomerSettings;

public abstract class HomerPage extends PageWithCallback implements PageBeginRenderListener {
    public static final String PAGE = "plugin/HomerPage";

    @Bean
    public abstract SipxValidationDelegate getValidator();

    public abstract HomerSettings getSettings();

    public abstract void setSettings(HomerSettings settings);

    @InjectObject("spring:homer")
    public abstract Homer getHomer();

    @Override
    public void pageBeginRender(PageEvent arg0) {
        if (getSettings() == null) {
            setSettings(getHomer().getSettings());
        }
    }

    public void apply() {
        getHomer().saveSettings(getSettings());
    }
}
