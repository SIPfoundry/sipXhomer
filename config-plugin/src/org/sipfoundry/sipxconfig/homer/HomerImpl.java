/*
 * Copyright (C) 2012 eZuce Inc., certain elements licensed under a Contributor Agreement.
 * Contributors retain copyright to elements licensed under a Contributor Agreement.
 * Licensed to the User under the AGPL license.
 *
 * $
 */
package org.sipfoundry.sipxconfig.homer;


import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.List;

import org.sipfoundry.sipxconfig.address.Address;
import org.sipfoundry.sipxconfig.address.AddressManager;
import org.sipfoundry.sipxconfig.address.AddressProvider;
import org.sipfoundry.sipxconfig.address.AddressType;
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
import org.sipfoundry.sipxconfig.snmp.ProcessDefinition;
import org.sipfoundry.sipxconfig.snmp.ProcessProvider;
import org.sipfoundry.sipxconfig.snmp.SnmpManager;

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
public class HomerImpl implements Homer, FeatureProvider, ProcessProvider {
    private BeanWithSettingsDao<HomerSettings> m_settingsDao;    

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
        
        // just so we don't have to configure capture to connected to remote mysql server
        validator.requiredOnSameHost(FEATURE_CAPTURE_SERVER, FEATURE_WEB);
    }

    @Override
    public void featureChangePostcommit(FeatureManager manager, FeatureChangeRequest request) {
    }

    @Override
    public Collection<ProcessDefinition> getProcessDefinitions(SnmpManager manager, Location location) {
        FeatureManager featureManager = manager.getFeatureManager();
        if (!featureManager.isFeatureEnabled(FEATURE_CAPTURE_SERVER, location)) {
            return null;
        }
        
        ProcessDefinition def = new ProcessDefinition("sipxhomer");
        def.setSipxServiceName("sipxhomer");
        return Collections.singleton(def);
    }
}
