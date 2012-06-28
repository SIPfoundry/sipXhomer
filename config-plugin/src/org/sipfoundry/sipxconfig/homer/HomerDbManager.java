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


import static java.lang.String.format;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.List;

import org.apache.commons.lang.StringUtils;
import org.sipfoundry.sipxconfig.backup.ArchiveDefinition;
import org.sipfoundry.sipxconfig.backup.ArchiveProvider;
import org.sipfoundry.sipxconfig.backup.BackupManager;
import org.sipfoundry.sipxconfig.backup.BackupSettings;
import org.sipfoundry.sipxconfig.cfgmgt.ConfigManager;
import org.sipfoundry.sipxconfig.cfgmgt.ConfigRequest;
import org.sipfoundry.sipxconfig.cfgmgt.PostConfigListener;
import org.sipfoundry.sipxconfig.commserver.Location;
import org.sipfoundry.sipxconfig.commserver.LocationsManager;
import org.sipfoundry.sipxconfig.feature.FeatureChangeRequest;
import org.sipfoundry.sipxconfig.feature.FeatureChangeValidator;
import org.sipfoundry.sipxconfig.feature.FeatureListener;
import org.sipfoundry.sipxconfig.feature.FeatureManager;
import org.sipfoundry.sipxconfig.proxy.ProxyManager;
import org.springframework.beans.factory.BeanFactory;
import org.springframework.beans.factory.BeanFactoryAware;
import org.springframework.beans.factory.ListableBeanFactory;
import org.springframework.jdbc.core.JdbcTemplate;

public class HomerDbManager implements BeanFactoryAware, FeatureListener, PostConfigListener, ArchiveProvider {
    private ListableBeanFactory m_beanFactory;
    private JdbcTemplate m_configJdbcTemplate;

    /**
     * Ensure each proxy is listed in homer's nodes table. 
     * NOTE: Records in node table are not removed, it's unclear if an admin would want that --Douglas 
     */
    public void syncNodes() {        
        String sipxSql = "select fqdn || '@' || ip_address from location l inner join feature_local f on l.location_id = f.location_id and f.feature_id = ?";
        List<String> sipxNodes = m_configJdbcTemplate.queryForList(sipxSql, String.class, ProxyManager.FEATURE.getId());

        String homerSql = "select concat(name, '@', host) from homer_nodes";
        JdbcTemplate homerDb = (JdbcTemplate) m_beanFactory.getBean("homerDb");
        List<String> homerNodes = homerDb.queryForList(homerSql, String.class);
                
        sipxNodes.removeAll(homerNodes);
        if (sipxNodes.isEmpty()) {
            return;
        }
                
        List<String> add = new ArrayList<String>();
        for (String missingNode : sipxNodes) {
            String[] decode  = StringUtils.split(missingNode, '@');
            String sql = format("insert into homer_nodes (name, host, status) values ('%s', '%s', 1)", decode[0], decode[1]);
            add.add(sql);            
        }
        homerDb.batchUpdate(add.toArray(new String[0]));
    }
    
    @Override
    public void setBeanFactory(BeanFactory factory) {
        m_beanFactory = (ListableBeanFactory) factory;
    }

    public void setBeanFactory(ListableBeanFactory beanFactory) {
        m_beanFactory = beanFactory;
    }

    public void setConfigJdbcTemplate(JdbcTemplate masterCopy) {
        m_configJdbcTemplate = masterCopy;
    }
    
    @Override
    public void featureChangePrecommit(FeatureManager manager, FeatureChangeValidator validator) {
    }

    @Override
    public void featureChangePostcommit(FeatureManager manager, FeatureChangeRequest request) {
        if (request.getAllNewlyEnabledFeatures().contains(ProxyManager.FEATURE)) {
            if (manager.isFeatureEnabled(Homer.FEATURE_WEB) || manager.isFeatureEnabled(Homer.FEATURE_CAPTURE_SERVER)) {
                syncNodes();
            }
        }        
    }

    @Override
    public void postReplicate(ConfigManager manager, ConfigRequest request) throws IOException {
        if (!request.applies(Homer.FEATURE_CAPTURE_SERVER, Homer.FEATURE_WEB, ProxyManager.FEATURE, LocationsManager.FEATURE)) {
            return;
        }
        syncNodes();
    }

    @Override
    public Collection<ArchiveDefinition> getArchiveDefinitions(BackupManager manager, Location location,
            BackupSettings manualSettings) {
        FeatureManager fm = manager.getFeatureManager();
        if (fm.isFeatureEnabled(Homer.FEATURE_CAPTURE_SERVER, location) || fm.isFeatureEnabled(Homer.FEATURE_WEB, location)) {
            return Collections.singleton(new ArchiveDefinition(Homer.ARCHIVE,
                    "$(sipx.SIPX_SERVICEDIR)/sipxhomer backup %s",
                    "$(sipx.SIPX_SERVICEDIR)/sipxhomer restore %s"));
        }
        return null;
    }
}
