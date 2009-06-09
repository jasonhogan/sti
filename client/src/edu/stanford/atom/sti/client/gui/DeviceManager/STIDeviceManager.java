/** @file STIDeviceManager.java
 *  @author Jason Michael Hogan
 *  @brief Source-file for the class STIDeviceManager
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

package edu.stanford.atom.sti.client.gui.DeviceManager;

import java.util.Vector;
import edu.stanford.atom.sti.client.comm.corba.ServerCommandLine;
import edu.stanford.atom.sti.client.comm.corba.DeviceConfigure;
import edu.stanford.atom.sti.device.comm.corba.TDevice;
import edu.stanford.atom.sti.client.gui.DeviceManager.DeviceTab;
import edu.stanford.atom.sti.client.comm.io.ServerConnectionListener;
import edu.stanford.atom.sti.client.comm.io.ServerConnectionEvent;
import java.lang.Thread;

public class STIDeviceManager extends javax.swing.JPanel implements ServerConnectionListener {

    private Vector<DeviceTab> deviceTabs = new Vector<DeviceTab>();
    private DeviceConfigure deviceConfigure = null;
    private ServerCommandLine commandLineRef = null;
    private TDevice[] devices;

    private Thread refreshThread = null;
    private boolean stopRefreshing = false;

    public STIDeviceManager(){
       initComponents();
    }

    public void installServants(ServerConnectionEvent event) {
        setDeviceConfigure(event.getServerConnection().getDeviceConfigure());
        commandLineRef = event.getServerConnection().getCommandLine();
        refreshDevices();
    }
    public void uninstallServants(ServerConnectionEvent event) {
        setDeviceConfigure(null);
        commandLineRef = null;
    }

    private void setDeviceConfigure(DeviceConfigure deviceConfig) {
        deviceConfigure = deviceConfig;
        refreshDevices();
    }
    private boolean isUnique(String device_id) {
        for(int i=0; i < deviceTabs.size(); i++) {
            if( device_id.equals(deviceTabs.elementAt(i).getDeviceID())) {
                return false;   // DeviceTab is already registered for this device
            }
        }
        return true;
    }

    private boolean isOnServer(String device_id) {
        for(int i=0; i < devices.length; i++) {
            if(device_id.equals(devices[i].deviceID)) {
                return true;
            }
        }
        return false;
    }
    
    private void addDeviceTab(TDevice tDevice) {
        if(isUnique(tDevice.deviceID)) {
            deviceTabs.addElement(new DeviceTab());
            deviceTabs.lastElement().registerDevice(tDevice, deviceConfigure, commandLineRef);
            deviceTabs.lastElement().setTabIndex(deviceTabs.size()-1);

            // look for other instances of this deviceName
            int instances = 0;  // will find itself
            for(int i=0; i < deviceTabs.size(); i++) {
                if(tDevice.deviceName.equals(
                        deviceTabs.elementAt(i).getTDevice().deviceName)) {
                    instances++;
                }
            }
            // Devices of the same type have tab titles that are numbered sequentially
            if(instances > 1) {
                deviceTabbedPane.addTab(tDevice.deviceName + " " + instances, deviceTabs.lastElement());
            } else {
                deviceTabbedPane.addTab(tDevice.deviceName, deviceTabs.lastElement());
            }
        }
    }

    private void removeDeviceTab(int index) {
        deviceTabbedPane.removeTabAt(index);
        deviceTabs.remove(index);

        //Reindex so DeviceTab indicies match JTabbedPane indicies
        for (int i = index; i < deviceTabs.size(); i++) {
            deviceTabs.elementAt(i).setTabIndex(i);
        }
    }
    
    private void refreshDevices() {
        
        stopRefreshingButton.setEnabled(true);
        deviceRefreshingBar.setIndeterminate(true);
        
        boolean status;

        do {
            status = true;

            for (int i = 0; i < deviceTabs.size() && !stopRefreshing; i++) {
                // Check if device is still alive -- dead devices will
                // automatically be removed by the STI Server.
                status &= deviceTabs.elementAt(i).deviceStatus();
            }

            if( deviceConfigure != null) {
                try {
                    devices = deviceConfigure.devices();
                } catch (Exception e) {
                    devices = new TDevice[0];
                    deviceConfigure = null;
                }
            }
            else {
                devices = new TDevice[0];
            }
            
            // add tabs for any new devices
            for (int i = 0; i < devices.length; i++) {
                addDeviceTab(devices[i]);
            }

            // remove tabs for any dead devices
            if (devices.length < deviceTabs.size()) {
                for (int i = 0; i < deviceTabs.size() && !stopRefreshing; i++) {
                    if (!isOnServer(deviceTabs.elementAt(i).getDeviceID())) {
                        // this device is no longer registered with the server
                        removeDeviceTab(deviceTabs.elementAt(i).getTabIndex());
                    }
                }
            }
        //Continue to refresh until no more devices are removed
        } while (!status && !stopRefreshing);
        
        for(int i = 0; i < deviceTabs.size(); i++) {
            deviceTabs.elementAt(i).setCommandLine(commandLineRef);
        }
        
        stopRefreshingButton.setEnabled(false);
        deviceRefreshingBar.setIndeterminate(false);
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
        deviceRefreshingBar = new javax.swing.JProgressBar();
        refreshButton = new javax.swing.JButton();
        stopRefreshingButton = new javax.swing.JButton();

        setMinimumSize(new java.awt.Dimension(500, 800));

        deviceTabbedPane.setBorder(javax.swing.BorderFactory.createTitledBorder("Registered Devices"));
        deviceTabbedPane.setTabPlacement(javax.swing.JTabbedPane.BOTTOM);

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
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 341, Short.MAX_VALUE)
                .addComponent(deviceRefreshingBar, javax.swing.GroupLayout.PREFERRED_SIZE, 185, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap())
        );
        jPanel1Layout.setVerticalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, jPanel1Layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                    .addComponent(deviceRefreshingBar, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, 23, Short.MAX_VALUE)
                    .addGroup(javax.swing.GroupLayout.Alignment.LEADING, jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
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
      //  refreshDevices();
        
        refreshThread = new Thread(new Runnable() {
            public void run() {
                refreshDevices();
            }
        });
        stopRefreshing = false;
        refreshThread.start();
}//GEN-LAST:event_refreshButtonActionPerformed

    private void stopRefreshingButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_stopRefreshingButtonActionPerformed
        // TODO add your handling code here:
        stopRefreshing = true;
}//GEN-LAST:event_stopRefreshingButtonActionPerformed


    // Variables declaration - do not modify//GEN-BEGIN:variables
    public javax.swing.JProgressBar deviceRefreshingBar;
    public javax.swing.JTabbedPane deviceTabbedPane;
    public javax.swing.JPanel jPanel1;
    public javax.swing.JButton refreshButton;
    public javax.swing.JButton stopRefreshingButton;
    // End of variables declaration//GEN-END:variables

}
