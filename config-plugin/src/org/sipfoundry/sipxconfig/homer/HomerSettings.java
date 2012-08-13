/*
 * Copyright (C) 2012 eZuce Inc., certain elements licensed under a Contributor Agreement.
 * Contributors retain copyright to elements licensed under a Contributor Agreement.
 * Licensed to the User under the AGPL license.
 *
 * $
 */
package org.sipfoundry.sipxconfig.homer;

import java.util.Arrays;
import java.util.Calendar;
import java.util.Collection;

import org.sipfoundry.sipxconfig.cfgmgt.DeployConfigOnEdit;
import org.sipfoundry.sipxconfig.feature.Feature;
import org.sipfoundry.sipxconfig.setting.PersistableSettings;
import org.sipfoundry.sipxconfig.setting.Setting;
import org.sipfoundry.sipxconfig.setting.SettingEntry;

public class HomerSettings extends PersistableSettings implements DeployConfigOnEdit {
    
    public HomerSettings() {
        addDefaultBeanSettingHandler(new Defaults());
    }
    
    public static class Defaults {
        @SettingEntry(path = "homer_web/timezone")
        public String getTimeZone() {
            // Cannot use TimeZoneManager because doesn't expose the ID
            return Calendar.getInstance().getTimeZone().getID();            
        }
    }

    @Override
    public String getBeanId() {
        return "homerSettings";
    }

    @Override
    protected Setting loadSettings() {
        return getModelFilesContext().loadModelFile("sipxhomer/sipxhomer.xml");
    }

    @Override
    public Collection<Feature> getAffectedFeaturesOnChange() {
        return Arrays.asList((Feature) Homer.FEATURE_CAPTURE_SERVER, Homer.FEATURE_WEB);
    }
    
    public boolean isSuperadminAuth() {
        return "sipx".equals(getSettingValue("homer_web/auth"));
    }
}
