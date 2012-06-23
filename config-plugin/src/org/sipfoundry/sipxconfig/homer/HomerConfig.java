/**
 * Copyright (c) 2012 eZuce, Inc. All rights reserved.
 * Contributed to SIPfoundry under a Contributor Agreement
 *
 * This software is free software; you can redistribute it and/or modify it under
 * the terms of the Affero General Public License (AGPL) as published by the
 * Free Software Foundation; either version 3 of the License, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
 * details.
 */
package org.sipfoundry.sipxconfig.homer;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.Writer;
import java.util.Set;

import org.apache.commons.io.IOUtils;
import org.sipfoundry.sipxconfig.address.Address;
import org.sipfoundry.sipxconfig.address.AddressManager;
import org.sipfoundry.sipxconfig.cfgmgt.CfengineModuleConfiguration;
import org.sipfoundry.sipxconfig.cfgmgt.ConfigManager;
import org.sipfoundry.sipxconfig.cfgmgt.ConfigProvider;
import org.sipfoundry.sipxconfig.cfgmgt.ConfigRequest;
import org.sipfoundry.sipxconfig.cfgmgt.KeyValueConfiguration;
import org.sipfoundry.sipxconfig.commserver.Location;
import org.sipfoundry.sipxconfig.commserver.LocationsManager;
import org.sipfoundry.sipxconfig.feature.FeatureChangeRequest;
import org.sipfoundry.sipxconfig.feature.FeatureChangeValidator;
import org.sipfoundry.sipxconfig.feature.FeatureListener;
import org.sipfoundry.sipxconfig.feature.FeatureManager;
import org.sipfoundry.sipxconfig.networkqueue.NetworkQueueManager;
import org.sipfoundry.sipxconfig.proxy.ProxyManager;

public class HomerConfig implements ConfigProvider, FeatureListener {
    private Homer m_homer;
    private AddressManager m_addressManager;
    private LocationsManager m_locationsManager;

    @Override
    public void replicate(ConfigManager manager, ConfigRequest request) throws IOException {
        if (!request.applies(Homer.FEATURE_CAPTURE_SERVER, Homer.FEATURE_WEB, NetworkQueueManager.FEATURE)) {
            return;
        }
        
        // make a design decision here: client capture is on if there is one or more capture
        // servers configured.  Otherwise, if client was on a capture server was off, queue might
        // build up
        boolean clientOn = manager.getFeatureManager().isFeatureEnabled(Homer.FEATURE_CAPTURE_SERVER);
        
        Set<Location> locations = request.locations(manager);
        HomerSettings settings = m_homer.getSettings();
        for (Location location : locations) {
            File dir = manager.getLocationDataDirectory(location);
            Writer cfdat = new FileWriter(new File(dir, "homer.cfdat"));
            boolean serverOn = manager.getFeatureManager().isFeatureEnabled(Homer.FEATURE_CAPTURE_SERVER, location);
            boolean webOn = manager.getFeatureManager().isFeatureEnabled(Homer.FEATURE_WEB, location);
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
        cfg.write("sqa-control-port", sqa.getCanonicalPort());
        cfg.write("sqa-control-address", sqa.getAddress());
    }
    
    void writeCfdat(Writer w, HomerSettings settings, boolean clientOn, boolean serverOn, boolean webOn) throws IOException {
        CfengineModuleConfiguration cfg = new CfengineModuleConfiguration(w);
        cfg.writeClass(Homer.FEATURE_WEB.getId(), webOn);
        cfg.writeClass(Homer.FEATURE_CAPTURE_SERVER.getId(), serverOn);
        cfg.writeClass("homer", clientOn);
        // these settings are really only meant for servers and contain passwords so
        // no need copying them w/o reason.
        if (serverOn || webOn) {
            cfg.writeSettings(Homer.FEATURE_WEB.getId() + '.', settings.getSettings());
        }
    }

    public void setHomer(Homer homer) {
        m_homer = homer;
    }

    @Override
    public void featureChangePrecommit(FeatureManager manager, FeatureChangeValidator validator) {
    }

    @Override
    public void featureChangePostcommit(FeatureManager manager, FeatureChangeRequest request) {
        if (!request.getAllNewlyEnabledFeatures().contains(ProxyManager.FEATURE)) {
            return;
        }
        
        HomerSettings settings = m_homer.getSettings();
        Location primary = m_locationsManager.getPrimaryLocation();
        Address db = m_addressManager.getSingleAddress(Homer.HOMER_DB, primary);
        
        
        
    }

    public void setAddressManager(AddressManager addressManager) {
        m_addressManager = addressManager;
    }
}
