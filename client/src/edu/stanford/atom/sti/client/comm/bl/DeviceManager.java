/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.comm.bl;

import edu.stanford.atom.sti.client.comm.io.ServerConnectionListener;
import edu.stanford.atom.sti.client.comm.io.ServerConnectionEvent;
import edu.stanford.atom.sti.client.comm.corba.ServerCommandLine;
import edu.stanford.atom.sti.client.comm.corba.DeviceConfigure;
import edu.stanford.atom.sti.client.gui.DevicesTab.DeviceTab;
import java.util.Vector;
import edu.stanford.atom.sti.device.comm.corba.TDevice;

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
                fireNewAddDeviceEvent(tDevice.deviceName + " " + instances, deviceTabs.lastElement());
            } else {
                fireNewAddDeviceEvent(tDevice.deviceName, deviceTabs.lastElement());
            }
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
