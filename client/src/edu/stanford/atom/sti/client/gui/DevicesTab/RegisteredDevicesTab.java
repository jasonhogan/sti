/** @file RegisteredDevicesTab.java
 *  @author Jason Michael Hogan
 *  @brief Source-file for the class RegisteredDevicesTab
 *  @section license License
 *
 *  Copyright (C) 2008 Jason Hogan <hogan@stanford.edu>\n
 *  This file is part of the Stanford Timing Interface (STI).
 *
 *  The STI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The STI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the STI.  If not, see <http://www.gnu.org/licenses/>.
 */

package edu.stanford.atom.sti.client.gui.DevicesTab;

import edu.stanford.atom.sti.client.comm.bl.device.Device;
import edu.stanford.atom.sti.client.comm.bl.device.DeviceCollectionListener;
import edu.stanford.atom.sti.client.comm.bl.device.DeviceEvent;
import edu.stanford.atom.sti.client.comm.bl.device.DeviceManager;
import java.util.Enumeration;
import java.util.Hashtable;
import javax.swing.SwingUtilities;
import javax.swing.tree.*;

public class RegisteredDevicesTab extends javax.swing.JPanel implements DeviceCollectionListener, DeviceTabSelectorListener {

    private DeviceManager deviceManager = null;
    private java.lang.Thread refreshThread = null;
    private Hashtable<Device, NewDeviceTab> deviceMap = new Hashtable<Device, NewDeviceTab>();

    private DeviceTabSelectorTreeManager ipTreeManager;
    private DeviceTabSelectorTreeManager groupTreeManager;
    
    public RegisteredDevicesTab() {
        initComponents();
        
        ipTreeManager = new DeviceTabSelectorTreeManager(ipDeviceTree, this) {
            @Override
            public String getTreeGroupName(Device device) {
                 return (device != null) ? 
                        (device.address().toLowerCase()) : "";
            }
        };
        
        groupTreeManager = new DeviceTabSelectorTreeManager(groupDeviceTree, this) {
            @Override
            public String getTreeGroupName(Device device) {
                 return (device != null) ? 
                        (device.address().toLowerCase()) : "";
            }
        };
        
        //Temporarily disable incomplete tabs
        deviceSelectorPane.setEnabledAt(1, false);
        deviceSelectorPane.setEnabledAt(2, false);
        groupDeviceTree.setEnabled(false);
    }

    
    public void registerDeviceManager(DeviceManager manager) {
        deviceManager = manager;
    }
    public synchronized void addDevice(Device device) {
        if( !deviceMap.containsKey(device) ) {
            NewDeviceTab newTab = new NewDeviceTab(device);
            newTab.setTabTitle( generateTabTitle(device) );
            
            deviceMap.put(device, newTab);

            ipTreeManager.addDeviceToTree(device);
            groupTreeManager.addDeviceToTree(device);
            
            deviceTabbedPane.addTab( newTab.getTabTitle(), newTab );

        }
    }

    public synchronized void removeDevice(Device device) {
        ipTreeManager.removeDeviceFromTree(device);
        groupTreeManager.removeDeviceFromTree(device);
        
        deviceTabbedPane.remove( deviceMap.remove(device) );
    }
    public void handleDeviceEvent(DeviceEvent evt) {
        //Pass on event to the relevant device's DeviceTab
        deviceMap.get(evt.getDevice()).handleDeviceEvent(evt);
    }
    public void setDeviceManagerStatus(DeviceManager.DeviceManagerStatus status) {
        switch(status) {
            case Refreshing:
                stopRefreshingButton.setEnabled(true);

                //This updates the GUI and so it must use invokeLater to execute
                //on the event dispatch thread after all repainting is finished.
                SwingUtilities.invokeLater(new Runnable() {
                    public void run() {
                        deviceRefreshingBar.setIndeterminate(true);
                    }
                });
                break;
            case Idle:
            default:
                stopRefreshingButton.setEnabled(false);

                //This updates the GUI and so it must use invokeLater to execute
                //on the event dispatch thread after all repainting is finished.
                SwingUtilities.invokeLater(new Runnable() {

                    public void run() {
                        deviceRefreshingBar.setIndeterminate(false);
                    }
                });
                break;
        }
        
    }
    
    public void toggleDeviceTab(Device device, boolean show) {
        if (device != null) {
            NewDeviceTab tab = deviceMap.get(device);
            if (tab != null) {
                if(show) {
                    deviceTabbedPane.addTab(tab.getTabTitle(), tab);
                } else {
                    deviceTabbedPane.remove(tab);
                }
            }
        }
    }
    
    public void selectDeviceTabByDevice(Device device) {
        if(device == null) {
            return;
        }
        NewDeviceTab tab = deviceMap.get(device);
        try {
            deviceTabbedPane.setSelectedComponent(tab);
        } catch(java.lang.IllegalArgumentException e) {
//            e.printStackTrace();
        }
    }

    private String generateTabTitle(Device device) {           
        // Look for other instances of this deviceName and/or module
        // Devices of the same name and module have tab titles that are numbered sequentially

        int otherInstances = 0;
        boolean verboseName = false;

        Enumeration<Device> devs = deviceMap.keys();
        Device currentDevice;

        while (devs.hasMoreElements()) {
            currentDevice = devs.nextElement();
            if (device.name().equals(currentDevice.name())) {
                verboseName = true;

                if (device.module() != currentDevice.module()) {
                    //Use the module number to distinguish the tab titles
                    deviceMap.get(currentDevice).setTabTitle(
                            currentDevice.name()
                            + " Module "
                            + currentDevice.module());
                } else {
                    // These have the same name AND same module number; add an index
                    otherInstances++;
                    deviceMap.get(currentDevice).setTabTitle(
                            currentDevice.name()
                            + " Module "
                            + currentDevice.module()
                            + " (" + otherInstances + ")");
                }
            }
        }

        String tabTitle = device.name();

        if (verboseName) {
            tabTitle += " Module " + device.module();
        }
        if (otherInstances > 0) {
            tabTitle += " Module " + device.module() 
                    + " (" + (otherInstances + 1) + ")";
        }
        return tabTitle;
    }
   
    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jPanel1 = new javax.swing.JPanel();
        refreshButton = new javax.swing.JButton();
        stopRefreshingButton = new javax.swing.JButton();
        deviceRefreshingBar = new javax.swing.JProgressBar();
        jSplitPane1 = new javax.swing.JSplitPane();
        deviceSelectorPane = new javax.swing.JTabbedPane();
        ipTreePanel = new javax.swing.JPanel();
        ipTree = new javax.swing.JScrollPane();
        ipDeviceTree = new javax.swing.JTree();
        jPanel3 = new javax.swing.JPanel();
        jPanel4 = new javax.swing.JPanel();
        jScrollPane1 = new javax.swing.JScrollPane();
        groupDeviceTree = new javax.swing.JTree();
        deviceTabbedPane = new javax.swing.JTabbedPane();

        setMinimumSize(new java.awt.Dimension(500, 800));

        refreshButton.setText("Refresh Devices");
        refreshButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                refreshButtonActionPerformed(evt);
            }
        });

        stopRefreshingButton.setText("Stop");
        stopRefreshingButton.setEnabled(false);
        stopRefreshingButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                stopRefreshingButtonActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout jPanel1Layout = new javax.swing.GroupLayout(jPanel1);
        jPanel1.setLayout(jPanel1Layout);
        jPanel1Layout.setHorizontalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, jPanel1Layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(refreshButton)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(stopRefreshingButton, javax.swing.GroupLayout.PREFERRED_SIZE, 113, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 339, Short.MAX_VALUE)
                .addComponent(deviceRefreshingBar, javax.swing.GroupLayout.PREFERRED_SIZE, 199, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap())
        );
        jPanel1Layout.setVerticalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, jPanel1Layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                    .addComponent(deviceRefreshingBar, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(stopRefreshingButton, javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(refreshButton, javax.swing.GroupLayout.PREFERRED_SIZE, 23, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addContainerGap())
        );

        jSplitPane1.setDividerLocation(240);

        ipDeviceTree.setModel(new DefaultTreeModel(new DefaultMutableTreeNode("STI Root")));
        ipDeviceTree.addTreeSelectionListener(new javax.swing.event.TreeSelectionListener() {
            public void valueChanged(javax.swing.event.TreeSelectionEvent evt) {
                ipDeviceTreeValueChanged(evt);
            }
        });
        ipTree.setViewportView(ipDeviceTree);

        javax.swing.GroupLayout ipTreePanelLayout = new javax.swing.GroupLayout(ipTreePanel);
        ipTreePanel.setLayout(ipTreePanelLayout);
        ipTreePanelLayout.setHorizontalGroup(
            ipTreePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(ipTree, javax.swing.GroupLayout.DEFAULT_SIZE, 234, Short.MAX_VALUE)
        );
        ipTreePanelLayout.setVerticalGroup(
            ipTreePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(ipTree, javax.swing.GroupLayout.DEFAULT_SIZE, 719, Short.MAX_VALUE)
        );

        deviceSelectorPane.addTab("IP Tree", ipTreePanel);

        javax.swing.GroupLayout jPanel3Layout = new javax.swing.GroupLayout(jPanel3);
        jPanel3.setLayout(jPanel3Layout);
        jPanel3Layout.setHorizontalGroup(
            jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 234, Short.MAX_VALUE)
        );
        jPanel3Layout.setVerticalGroup(
            jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 719, Short.MAX_VALUE)
        );

        deviceSelectorPane.addTab("Name", jPanel3);

        groupDeviceTree.setModel(new DefaultTreeModel(new DefaultMutableTreeNode("STI Root")));
        groupDeviceTree.addTreeSelectionListener(new javax.swing.event.TreeSelectionListener() {
            public void valueChanged(javax.swing.event.TreeSelectionEvent evt) {
                groupDeviceTreeValueChanged(evt);
            }
        });
        jScrollPane1.setViewportView(groupDeviceTree);

        javax.swing.GroupLayout jPanel4Layout = new javax.swing.GroupLayout(jPanel4);
        jPanel4.setLayout(jPanel4Layout);
        jPanel4Layout.setHorizontalGroup(
            jPanel4Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 234, Short.MAX_VALUE)
        );
        jPanel4Layout.setVerticalGroup(
            jPanel4Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 719, Short.MAX_VALUE)
        );

        deviceSelectorPane.addTab("Group Tree", jPanel4);

        jSplitPane1.setLeftComponent(deviceSelectorPane);
        jSplitPane1.setRightComponent(deviceTabbedPane);

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jPanel1, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jSplitPane1)
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addComponent(jPanel1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jSplitPane1))
        );
    }// </editor-fold>//GEN-END:initComponents

    private void refreshButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_refreshButtonActionPerformed

        refreshThread = new Thread(new Runnable() {
            public void run() {
                if( deviceManager != null) {
                    deviceManager.refreshDeviceListOnServer();
                    deviceManager.refreshDeviceLists();
                }
            }
        });
        refreshThread.start();

}//GEN-LAST:event_refreshButtonActionPerformed

    private void stopRefreshingButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_stopRefreshingButtonActionPerformed
        deviceManager.stopRefreshing();
}//GEN-LAST:event_stopRefreshingButtonActionPerformed


    
    private void ipDeviceTreeValueChanged(javax.swing.event.TreeSelectionEvent evt) {//GEN-FIRST:event_ipDeviceTreeValueChanged
        TreePath path = evt.getNewLeadSelectionPath();

        boolean checkboxIsChecked = ipTreeManager.isTreePathChecked(path);
        if(checkboxIsChecked) {
            ipTreeManager.selectDeviceTabUsingTreePath(path);
        }
    }//GEN-LAST:event_ipDeviceTreeValueChanged

    private void groupDeviceTreeValueChanged(javax.swing.event.TreeSelectionEvent evt) {//GEN-FIRST:event_groupDeviceTreeValueChanged
        TreePath path = evt.getNewLeadSelectionPath();

        boolean checkboxIsChecked = groupTreeManager.isTreePathChecked(path);
        if(checkboxIsChecked) {
            groupTreeManager.selectDeviceTabUsingTreePath(path);
        }
    }//GEN-LAST:event_groupDeviceTreeValueChanged


    // Variables declaration - do not modify//GEN-BEGIN:variables
    public javax.swing.JProgressBar deviceRefreshingBar;
    public javax.swing.JTabbedPane deviceSelectorPane;
    public javax.swing.JTabbedPane deviceTabbedPane;
    public javax.swing.JTree groupDeviceTree;
    public javax.swing.JTree ipDeviceTree;
    public javax.swing.JScrollPane ipTree;
    public javax.swing.JPanel ipTreePanel;
    public javax.swing.JPanel jPanel1;
    public javax.swing.JPanel jPanel3;
    public javax.swing.JPanel jPanel4;
    public javax.swing.JScrollPane jScrollPane1;
    public javax.swing.JSplitPane jSplitPane1;
    public javax.swing.JButton refreshButton;
    public javax.swing.JButton stopRefreshingButton;
    // End of variables declaration//GEN-END:variables

}
