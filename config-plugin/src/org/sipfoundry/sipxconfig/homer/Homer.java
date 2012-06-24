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
    public static final LocationFeature FEATURE_WEB = new LocationFeature("homer_web");
    public static final LocationFeature FEATURE_CAPTURE_SERVER = new LocationFeature("homer_capture");
    public static final String ARCHIVE = "homer.tar.gz";
        
    public HomerSettings getSettings();
    
    public void saveSettings(HomerSettings settings);
}
