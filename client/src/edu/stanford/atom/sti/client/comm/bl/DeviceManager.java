/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.comm.bl;

import edu.stanford.atom.sti.client.comm.io.ServerConnectionListener;
import edu.stanford.atom.sti.client.comm.io.ServerConnectionEvent;
import edu.stanford.atom.sti.corba.Client_Server.ServerCommandLine;
import edu.stanford.atom.sti.corba.Client_Server.DeviceConfigure;
import edu.stanford.atom.sti.client.gui.DevicesTab.DeviceTab;
import java.util.Vector;

import edu.stanford.atom.sti.corba.Types.TDevice;


public class DeviceManager implements ServerConnectionListener {
    
    private DeviceConfigure deviceConfigure = null;
    private ServerCommandLine commandLineRef = null;
    private TDevice[] devices;
    private Vector<DeviceTab> deviceTabs = new Vector<DeviceTab>();
    private Vector<DeviceManagerListener> listeners = new Vector<DeviceManagerListener>();
    
    private boolean stopRefreshing = false;
    private boolean refreshing = false;
    
    public synchronized void addDeviceListener(DeviceManagerListener listener) {
        listeners.add(listener);
    }
    public synchronized void removeDeviceListener(DeviceManagerListener listener) {
        listeners.remove(listener);
    }
    private synchronized void fireNewRefreshDevicesEvent() {
        DeviceManagerEvent event = new DeviceManagerEvent(this, 
                DeviceManagerEvent.DeviceEventType.StartRefresh);
        
        for(int i = 0; i < listeners.size(); i++) {
            listeners.elementAt(i).refreshDevices( event );
        }
    }
    private synchronized void fireNewStopRefreshingEvent() {
        DeviceManagerEvent event = new DeviceManagerEvent(this, 
                DeviceManagerEvent.DeviceEventType.StopRefresh);
        
        for(int i = 0; i < listeners.size(); i++) {
            listeners.elementAt(i).refreshDevices( event );
        }
    }
        
    private synchronized void fireNewAddDeviceEvent(String deviceName, DeviceTab device) {
        DeviceManagerEvent event = new DeviceManagerEvent(this, 
                DeviceManagerEvent.DeviceEventType.AddDevice);
        
        event.addDevice(deviceName,device);
        
        for(int i = 0; i < listeners.size(); i++) {
            listeners.elementAt(i).addDevice( event );
        }
    }
    private synchronized void fireNewRemoveDeviceEvent(DeviceTab device) {
        DeviceManagerEvent event = new DeviceManagerEvent(this, 
                DeviceManagerEvent.DeviceEventType.RemoveDevice);
        
        event.removeDevice(device);
        
        for(int i = 0; i < listeners.size(); i++) {
            listeners.elementAt(i).removeDevice( event );
        }
    }
    public void installServants(ServerConnectionEvent event) {
        //setDeviceConfigure(event.getServerConnection().getDeviceConfigure());
        deviceConfigure = event.getServerConnection().getDeviceConfigure();
        commandLineRef = event.getServerConnection().getCommandLine();
        refreshDevices();
    }
    public void uninstallServants(ServerConnectionEvent event) {
        deviceConfigure = null;
        commandLineRef = null;
        refreshDevices();
    }

    public boolean refreshing() {
        return refreshing;
    }
    
    private void addDeviceTab(TDevice tDevice) {
        if(isUnique(tDevice.deviceID)) {

            DeviceTab newDevice = new DeviceTab();
            // Look for other instances of this deviceName and/or module
            // Devices of the same name and module have tab titles that are numbered sequentially

            int otherInstances = 0;
            boolean verboseName = false;

            for(int i=0; i < deviceTabs.size(); i++) {
                if(tDevice.deviceName.equals(
                        deviceTabs.elementAt(i).getTDevice().deviceName)) {
                    
                    verboseName = true;

                    if(tDevice.moduleNum != deviceTabs.elementAt(i).getTDevice().moduleNum) {
                        
                        deviceTabs.elementAt(i).setTabTitle(
                                deviceTabs.elementAt(i).getTDevice().deviceName 
                                + " Module " + deviceTabs.elementAt(i).getTDevice().moduleNum);
                    }
                    else {
                        // These have the same name AND same module number; add an index
                        otherInstances++;
                        deviceTabs.elementAt(i).setTabTitle(
                                deviceTabs.elementAt(i).getTDevice().deviceName 
                                + " Module " + deviceTabs.elementAt(i).getTDevice().moduleNum
                                + " (" + otherInstances + ")");
                        
                    }
                }
            }

            newDevice.setTabTitle(tDevice.deviceName);

            if (verboseName) {
                newDevice.setTabTitle( newDevice.getTabTitle()
                                + " Module " + tDevice.moduleNum );
            }
            if (otherInstances > 0) {
                newDevice.setTabTitle( newDevice.getTabTitle()
                        + " (" + (otherInstances + 1) + ")" );
            }
            
            deviceTabs.addElement(newDevice);
            deviceTabs.lastElement().registerDevice(tDevice, deviceConfigure, commandLineRef);
            deviceTabs.lastElement().setTabIndex(deviceTabs.size() - 1);
            
            fireNewAddDeviceEvent(deviceTabs.lastElement().getTabTitle(), deviceTabs.lastElement());
        }
    }
    private void removeDeviceTab(DeviceTab device) {
      
        int index = device.getTabIndex();
        deviceTabs.remove( index );
        fireNewRemoveDeviceEvent(device);

        //Reindex so DeviceTab indicies match JTabbedPane indicies
        for (int i = index; i < deviceTabs.size(); i++) {
            deviceTabs.elementAt(i).setTabIndex(i);
        }
    }

    public void refreshDevices() {
        if(refreshing)
            return;

        stopRefreshing = false;
        refreshing = true;
        fireNewRefreshDevicesEvent();
        
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
                        removeDeviceTab(deviceTabs.elementAt(i));
                    }
                }
            }
        //Continue to refresh until no more devices are removed
        } while (!status && !stopRefreshing);
        
        for(int i = 0; i < deviceTabs.size(); i++) {
            deviceTabs.elementAt(i).setCommandLine(commandLineRef);
            deviceTabs.elementAt(i).setDeviceConfigure(deviceConfigure);
            deviceTabs.elementAt(i).setDeviceManager( this );
        }
        
        stopRefreshing = false;
        refreshing = false;
        fireNewStopRefreshingEvent();

    }
    public void stopRefreshing() {
        stopRefreshing = true;
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
}
