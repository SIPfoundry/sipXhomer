/*
 * Copyright (C) 2012 eZuce Inc., certain elements licensed under a Contributor Agreement.
 * Contributors retain copyright to elements licensed under a Contributor Agreement.
 * Licensed to the User under the AGPL license.
 *
 * $
 */
package org.sipfoundry.sipxconfig.homer;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.Writer;
import java.util.Collection;
import java.util.Collections;
import java.util.Set;

import org.apache.commons.io.IOUtils;
import org.sipfoundry.sipxconfig.cfgmgt.CfengineModuleConfiguration;
import org.sipfoundry.sipxconfig.cfgmgt.ConfigManager;
import org.sipfoundry.sipxconfig.cfgmgt.ConfigProvider;
import org.sipfoundry.sipxconfig.cfgmgt.ConfigRequest;
import org.sipfoundry.sipxconfig.commserver.Location;
import org.sipfoundry.sipxconfig.feature.Bundle;
import org.sipfoundry.sipxconfig.feature.BundleConstraint;
import org.sipfoundry.sipxconfig.feature.FeatureProvider;
import org.sipfoundry.sipxconfig.feature.GlobalFeature;
import org.sipfoundry.sipxconfig.feature.LocationFeature;
import org.sipfoundry.sipxconfig.setting.BeanWithSettingsDao;
import org.sipfoundry.sipxconfig.site.skin.MessageSourceProvider;
import org.springframework.context.MessageSource;
import org.springframework.context.support.ResourceBundleMessageSource;

public class HomerImpl implements Homer, ConfigProvider, FeatureProvider, MessageSourceProvider {
    private BeanWithSettingsDao<HomerSettings> m_settingsDao;    

    @Override
    public void replicate(ConfigManager manager, ConfigRequest request) throws IOException {
        if (!request.applies(FEATURE)) {
            return;
        }
        
        Set<Location> locations = request.locations(manager);
        HomerSettings settings = getSettings();
        for (Location location : locations) {
            File dir = manager.getLocationDataDirectory(location);
            boolean enabled = manager.getFeatureManager().isFeatureEnabled(FEATURE, location);
            Writer cfg = new FileWriter(new File(dir, "homer.cfdat"));
            try {
                write(cfg, enabled, settings);
            } finally {
                IOUtils.closeQuietly(cfg);
            }
        }
    }
    
    void write(Writer w, boolean enabled, HomerSettings settings) throws IOException {
        CfengineModuleConfiguration cfg = new CfengineModuleConfiguration(w);
        cfg.writeClass("webhomer", enabled);
        if (!enabled) {
            return;
        }
        
        cfg.write("webhomer.", settings.getSettings());
    }

    @Override
    public Collection<GlobalFeature> getAvailableGlobalFeatures() {
        return null;
    }

    @Override
    public Collection<LocationFeature> getAvailableLocationFeatures(Location l) {
        return Collections.singleton(FEATURE);
    }

    @Override
    public HomerSettings getSettings() {
        return m_settingsDao.findOrCreateOne();
    }

    @Override
    public void saveSettings(HomerSettings settings) {
        m_settingsDao.upsert(settings);        
    }

    public void setSettingsDao(BeanWithSettingsDao<HomerSettings> settingsDao) {
        m_settingsDao = settingsDao;
    }

    @Override
    public MessageSource getMessageSource() {
        ResourceBundleMessageSource rb = new ResourceBundleMessageSource();
        rb.setBasename("sipxhomer-plugin");
        return rb;
    }

    @Override
    public void getBundleFeatures(Bundle b) {
        if (b.isRouter()) {
            b.addFeature(FEATURE, BundleConstraint.SINGLE_LOCATION);
        }
    }
}
