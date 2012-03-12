/*
 * Copyright (C) 2012 eZuce Inc., certain elements licensed under a Contributor Agreement.
 * Contributors retain copyright to elements licensed under a Contributor Agreement.
 * Licensed to the User under the AGPL license.
 *
 * $
 */

package org.sipfoundry.sipxconfig.homer;

import org.sipfoundry.sipxconfig.feature.LocationFeature;


public interface Homer {
    public static final LocationFeature FEATURE = new LocationFeature("homer");
    
    public HomerSettings getSettings();
    
    public void saveSettings(HomerSettings settings);
}
