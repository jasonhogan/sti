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
import  javax.swing.SwingUtilities;
import java.util.Hashtable;
import java.util.Enumeration;
import java.util.Vector;

public class RegisteredDevicesTab extends javax.swing.JPanel implements DeviceCollectionListener {

    //private DeviceManager deviceManager = null;
    private java.lang.Thread refreshThread = null;
    private boolean initFinished = false;
    private Hashtable<Device, DeviceTab> deviceMap = new Hashtable<Device, DeviceTab>();

    private BTreeNode<Device> deviceNameTree = new BTreeNode<Device>(); //for keeping track of device tab names

    public RegisteredDevicesTab(){
       initComponents();
       initFinished = true;
    }
    public void addDevice(Device device) {
        if( deviceMap.containsKey(device) ) {
            DeviceTab newTab = new DeviceTab(device);
            newTab.setTabTitle( generateTabTitle(device) );
            
            deviceMap.put(device, newTab);

            //addToNameTree(device);

            deviceTabbedPane.addTab( newTab.getTabTitle(), newTab );
            
//            newTab.setTabIndex(deviceTabbedPane.getTabCount() - 1);
        }
    }
    public void removeDevice(Device device) {
        deviceTabbedPane.remove( deviceMap.get(device) );
    }
    public void refreshDevice(DeviceEvent evt) {
        deviceMap.get(evt.getDevice()).refreshAttributes();
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
    private String generateTabTitle(Device device) {           
        // Look for other instances of this deviceName and/or module
        // Devices of the same name and module have tab titles that are numbered sequentially

        int otherInstances = 0;
        boolean verboseName = false;

        Enumeration<Device> devs = deviceMap.keys();
        Device currentDevice = null;

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
   
    private void addToNameTree(Device device) {
        BTreeNode<Device> treeNode = deviceNameTree;
        Vector<BTreeNode> leaves = new Vector<BTreeNode>();

        boolean walking = true;
        while(walking) {
            if(treeNode.hasLeaves()) {
                leaves = treeNode.getLeaves();
            }
        }

    }
    private class BTreeNode<T> {
        private T data = null;
        private Vector<BTreeNode> leaves = new Vector<BTreeNode>();

        public BTreeNode() {
        }
        public BTreeNode(T data) {
            this.data = data;
        }
        public void addLeaf(T leaf) {
            if(data != null) {
                leaves.addElement( new BTreeNode(data) );
                data = null;
            }
            if(leaves.size() == 0) {
                data = leaf;
            } else {
                leaves.addElement( new BTreeNode(leaf) );
            }
        }
        public boolean hasLeaves() {
            return (leaves.size() != 0);
        }
        public Vector<BTreeNode> getLeaves() {
            return leaves;
        }
        public T getData() {
            return data;
        }
    }
   
    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        deviceTabbedPane = new javax.swing.JTabbedPane();
        jPanel1 = new javax.swing.JPanel();
        refreshButton = new javax.swing.JButton();
        stopRefreshingButton = new javax.swing.JButton();
        deviceRefreshingBar = new javax.swing.JProgressBar();

        setMinimumSize(new java.awt.Dimension(500, 800));

        deviceTabbedPane.setBorder(javax.swing.BorderFactory.createTitledBorder("Registered Devices"));

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
            .addGroup(jPanel1Layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(refreshButton)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(stopRefreshingButton, javax.swing.GroupLayout.PREFERRED_SIZE, 113, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 327, Short.MAX_VALUE)
                .addComponent(deviceRefreshingBar, javax.swing.GroupLayout.PREFERRED_SIZE, 199, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap())
        );
        jPanel1Layout.setVerticalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel1Layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(deviceRefreshingBar, javax.swing.GroupLayout.DEFAULT_SIZE, 23, Short.MAX_VALUE)
                    .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                        .addComponent(refreshButton, javax.swing.GroupLayout.DEFAULT_SIZE, 23, Short.MAX_VALUE)
                        .addComponent(stopRefreshingButton)))
                .addContainerGap())
        );

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jPanel1, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
            .addComponent(deviceTabbedPane, javax.swing.GroupLayout.DEFAULT_SIZE, 780, Short.MAX_VALUE)
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addComponent(jPanel1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(deviceTabbedPane, javax.swing.GroupLayout.DEFAULT_SIZE, 738, Short.MAX_VALUE)
                .addContainerGap())
        );
    }// </editor-fold>//GEN-END:initComponents

    private void refreshButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_refreshButtonActionPerformed

        refreshThread = new Thread(new Runnable() {
            public void run() {
            //    deviceManager.refreshDevices();
            }
        });
        refreshThread.start();
}//GEN-LAST:event_refreshButtonActionPerformed

    private void stopRefreshingButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_stopRefreshingButtonActionPerformed
        //deviceManager.stopRefreshing();
}//GEN-LAST:event_stopRefreshingButtonActionPerformed


    // Variables declaration - do not modify//GEN-BEGIN:variables
    public javax.swing.JProgressBar deviceRefreshingBar;
    public javax.swing.JTabbedPane deviceTabbedPane;
    public javax.swing.JPanel jPanel1;
    public javax.swing.JButton refreshButton;
    public javax.swing.JButton stopRefreshingButton;
    // End of variables declaration//GEN-END:variables

}
