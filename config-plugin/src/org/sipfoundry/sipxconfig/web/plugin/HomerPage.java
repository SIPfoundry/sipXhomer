/*
 * Copyright (C) 2012 eZuce Inc., certain elements licensed under a Contributor Agreement.
 * Contributors retain copyright to elements licensed under a Contributor Agreement.
 * Licensed to the User under the AGPL license.
 *
 * $
 */
package org.sipfoundry.sipxconfig.web.plugin;

import org.apache.tapestry.annotations.InjectObject;
import org.apache.tapestry.html.BasePage;
import org.sipfoundry.sipxconfig.homer.Homer;

public abstract class HomerPage extends BasePage {

    @InjectObject("spring:homer")
    public abstract Homer getHomer();

    public String getTestNoValue() {
        Homer h = getHomer();
        return getMessages().format("msg.hello",  h.hello());
    }
}
