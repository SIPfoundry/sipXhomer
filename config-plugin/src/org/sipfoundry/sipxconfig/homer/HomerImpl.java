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
import java.util.Arrays;
import java.util.Collection;
import java.util.Set;

import org.apache.commons.io.IOUtils;
import org.sipfoundry.sipxconfig.address.Address;
import org.sipfoundry.sipxconfig.cfgmgt.CfengineModuleConfiguration;
import org.sipfoundry.sipxconfig.cfgmgt.ConfigManager;
import org.sipfoundry.sipxconfig.cfgmgt.ConfigProvider;
import org.sipfoundry.sipxconfig.cfgmgt.ConfigRequest;
import org.sipfoundry.sipxconfig.cfgmgt.KeyValueConfiguration;
import org.sipfoundry.sipxconfig.commserver.Location;
import org.sipfoundry.sipxconfig.feature.Bundle;
import org.sipfoundry.sipxconfig.feature.FeatureChangeRequest;
import org.sipfoundry.sipxconfig.feature.FeatureChangeValidator;
import org.sipfoundry.sipxconfig.feature.FeatureManager;
import org.sipfoundry.sipxconfig.feature.FeatureProvider;
import org.sipfoundry.sipxconfig.feature.GlobalFeature;
import org.sipfoundry.sipxconfig.feature.LocationFeature;
import org.sipfoundry.sipxconfig.networkqueue.NetworkQueueManager;
import org.sipfoundry.sipxconfig.setting.BeanWithSettingsDao;

/**
 * backup
 * setup - initial migration
 * ui to edit aliases,db's
 * sync nodes ? what are nodes? edit nodes?
 * firewall mysql
 * edit settings
 * separate location homer web (primary only) and global homer feature
 * mysql C++ odbc daemon code. (get code from joegen for parsing)  
 */
public class HomerImpl implements Homer, ConfigProvider, FeatureProvider {
    private BeanWithSettingsDao<HomerSettings> m_settingsDao;    

    @Override
    public void replicate(ConfigManager manager, ConfigRequest request) throws IOException {
        if (!request.applies(FEATURE_CAPTURE_SERVER, FEATURE_WEB)) {
            return;
        }
        
        // make a design decision here: client capture is on if there is one or more capture
        // servers configured.  Otherwise, if client was on a capture server was off, queue might
        // build up
        boolean clientOn = manager.getFeatureManager().isFeatureEnabled(FEATURE_CAPTURE_SERVER);
        
        Set<Location> locations = request.locations(manager);
        HomerSettings settings = getSettings();
        for (Location location : locations) {
            File dir = manager.getLocationDataDirectory(location);
            Writer cfdat = new FileWriter(new File(dir, "homer.cfdat"));
            boolean serverOn = manager.getFeatureManager().isFeatureEnabled(FEATURE_CAPTURE_SERVER, location);
            boolean webOn = manager.getFeatureManager().isFeatureEnabled(FEATURE_WEB, location);
            try {
                writeCfdat(cfdat, settings, clientOn, serverOn, webOn);                
            } finally {
                IOUtils.closeQuietly(cfdat);
            }
            
            if (serverOn) {
                Writer capCfg = new FileWriter(new File(dir, "sipxhomer.ini.part"));
                Address sqa = manager.getAddressManager().getSingleAddress(NetworkQueueManager.CONTROL_ADDRESS);
                try {
                    writeConfig(capCfg, sqa, settings);
                } finally {
                    IOUtils.closeQuietly(capCfg);
                }
            }
        }
    }
    
    void writeConfig(Writer w, Address sqa, HomerSettings settings) throws IOException {
        KeyValueConfiguration cfg = KeyValueConfiguration.equalsSeparated(w);
        cfg.write("sqa-control-port", sqa.getPort());
        cfg.write("sqa-control-address", sqa.getAddress());
    }
    
    void writeCfdat(Writer w, HomerSettings settings, boolean clientOn, boolean serverOn, boolean webOn) throws IOException {
        CfengineModuleConfiguration cfg = new CfengineModuleConfiguration(w);
        cfg.writeClass(FEATURE_WEB.getId(), webOn);
        cfg.writeClass(FEATURE_CAPTURE_SERVER.getId(), serverOn);
        cfg.writeClass("homer", clientOn);
        // these settings are really only meant for servers and contain passwords so
        // no need copying them w/o reason.
        if (serverOn || webOn) {
            cfg.writeSettings(FEATURE_WEB.getId() + '.', settings.getSettings());
        }
    }

    @Override
    public Collection<GlobalFeature> getAvailableGlobalFeatures(FeatureManager featureManager) {
        return null;
    }

    @Override
    public Collection<LocationFeature> getAvailableLocationFeatures(FeatureManager featureManager, Location l) {
        return Arrays.asList(FEATURE_CAPTURE_SERVER, FEATURE_WEB);
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
    public void getBundleFeatures(FeatureManager featureManager, Bundle b) {
        if (b == Bundle.EXPERIMENTAL) {
            b.addFeature(FEATURE_CAPTURE_SERVER);
            b.addFeature(FEATURE_WEB);
        }
    }

    @Override
    public void featureChangePrecommit(FeatureManager manager, FeatureChangeValidator validator) {

        // just because apache is already there, otherwise no restrictions
        validator.primaryLocationOnly(FEATURE_WEB);
        
        validator.requiresAtLeastOne(FEATURE_CAPTURE_SERVER, NetworkQueueManager.FEATURE);
    }

    @Override
    public void featureChangePostcommit(FeatureManager manager, FeatureChangeRequest request) {
    }
}
