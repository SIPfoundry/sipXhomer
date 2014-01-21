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
import org.sipfoundry.sipxconfig.mysql.MySql;
import org.sipfoundry.sipxconfig.proxy.ProxyManager;
import org.springframework.beans.factory.BeanFactory;
import org.springframework.beans.factory.BeanFactoryAware;
import org.springframework.beans.factory.ListableBeanFactory;
import org.springframework.jdbc.core.JdbcTemplate;

public class HomerDbManager implements BeanFactoryAware, FeatureListener, PostConfigListener, ArchiveProvider {
    private ListableBeanFactory m_beanFactory;
    private JdbcTemplate m_configJdbcTemplate;
    private FeatureManager m_featureManager;
    private String m_serviceScript;

    public void setServiceScript(String homerServiceScript) {
        m_serviceScript = homerServiceScript;
    }

    /**
     * Ensure each proxy is listed in homer's hosts table. 
     * NOTE: Records in node table are not removed, it's unclear if an admin would want that --Douglas 
     */
    public void syncNodes() {
        if (!m_featureManager.isFeatureEnabled(MySql.FEATURE)) {
            return;
        }
        // sync node info
        String sipxSql = "select fqdn || '@' || ip_address from location l inner join feature_local f on l.location_id = f.location_id and f.feature_id = ?";
        List<String> sipxHosts = m_configJdbcTemplate.queryForList(sipxSql, String.class, ProxyManager.FEATURE.getId());

        String homerSql = "select concat(name, '@', host) from homer_hosts";
        JdbcTemplate homerDb = (JdbcTemplate) m_beanFactory.getBean("homerDb");
        List<String> homerHosts = homerDb.queryForList(homerSql, String.class);
                
        sipxHosts.removeAll(homerHosts);
        if (!sipxHosts.isEmpty()) {                
            List<String> add = new ArrayList<String>();
            for (String missingNode : sipxHosts) {
                String[] decode  = StringUtils.split(missingNode, '@');
                String name = decode[0];
                String host = decode[1];
                String checkHostSql = "select count(0) from homer_hosts where host = ?";
                boolean hostExists = homerDb.queryForInt(checkHostSql, host) > 0;
                String sql;
                if (hostExists) {
                    sql = format("update homer_hosts set name='%s' where host='%s'", name, host);
                } else {
                    sql = format("insert into homer_hosts (name, host, status) values ('%s', '%s', 1)", name, host);
                }
                add.add(sql);            
            }
            homerDb.batchUpdate(add.toArray(new String[0]));
        }
        
        // sync db info
        String dbhost = "127.0.0.1";
        int dbport = 3306;
        String dbname = "homer_db";
        String dbuser = "root";
        String dbpass = "";
        String dbtables = "sip_capture";
        String name = "local";
        int status = 1;        
        // must have lowest ID to ensure it's the default node for statistics
        String nodeSql = "select 1 from homer_nodes where host = ? and dbport = ? and dbname = ? and dbpassword = ? "
                + "and dbusername = ? and dbtables = ? and name = ? and status = ? and id = 1";
        List<Integer> found = homerDb.queryForList(nodeSql, Integer.class, dbhost, dbport, dbname, dbpass, dbuser,
                dbtables, name, status);
        if (found.size() == 0) {
            String[] hosts = new String[2];
            hosts[0] = "delete from homer_nodes";
            String addNode = "insert into homer_nodes (id, host, dbport, dbname, dbpassword, dbusername, dbtables, name, status) "
                    + "values (1, '%s',%d,'%s','%s','%s','%s','%s','%d')";
            hosts[1] = format(addNode, dbhost, dbport, dbname, dbpass, dbuser, dbtables, name, status);
            homerDb.batchUpdate(hosts);
        }
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
                    m_serviceScript + " backup %s",
                    m_serviceScript + " restore %s"));
        }
        return null;
    }

    @Override
    public void replicate(ConfigManager manager, ConfigRequest request) throws IOException {
    }

    public void setFeatureManager(FeatureManager featureManager) {
        m_featureManager = featureManager;
    }
}
