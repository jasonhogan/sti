/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.comm.bl.device;

import java.util.Vector;
import java.util.Arrays;
import edu.stanford.atom.sti.corba.Types.TDevice;
import edu.stanford.atom.sti.client.comm.io.STIServerConnection;
import edu.stanford.atom.sti.client.comm.io.ServerConnectionListener;
import edu.stanford.atom.sti.client.comm.io.ServerConnectionEvent;

import edu.stanford.atom.sti.client.comm.io.DeviceRefreshEventListener;

/**
 *
 * @author Jason
 */
public class DeviceManager implements ServerConnectionListener, DeviceRefreshEventListener {

    private STIServerConnection server = null;
    private Vector<DeviceCollection> deviceCollections = new Vector<DeviceCollection>();
    private Vector<TDevice> devicesOnClient = new Vector<TDevice>();
    
    public enum DeviceManagerStatus {Idle, Refreshing, NotConnected};
    
    public DeviceManager() {
     
    }
    
    public void handleEvent(edu.stanford.atom.sti.corba.Pusher.TDeviceRefreshEvent event) {
        if(event.type == edu.stanford.atom.sti.corba.Pusher.DeviceRefreshEventType.RefreshDeviceList) {
            refreshDeviceLists();
        }
        else {
            forwardDeviceEvent(event);
        }
    }

    private TDevice getTDevice(String deviceID) {
        TDevice device = null;
        for(TDevice dev : devicesOnClient) {
            if(dev.deviceID.equals(deviceID)) {
                device = dev;
                break;
            }
        }
        return device;
    }

    public synchronized void addDeviceCollection(DeviceCollection deviceCollection) {
        if(!deviceCollections.contains(deviceCollection)) {
            deviceCollections.addElement(deviceCollection);
        }
    }
    public synchronized void removeDeviceCollection(DeviceCollection deviceCollection) {
        deviceCollections.removeElement(deviceCollection);
    }

    public synchronized void refreshDeviceLists() {

        fireStatusEvent(DeviceManagerStatus.Refreshing);
        
        int i;
        TDevice[] devices = null;
        
        if (server != null) {
            try {
                devices = server.getRegisteredDevices().devices();
            } catch (Exception e) {
            }
        }

        Vector<TDevice> devicesOnServer = null;

        if(devices != null)
            devicesOnServer = new Vector<TDevice>(Arrays.asList(devices));
        
        //remove dead devices
        boolean findingDeadDevices = true;
        while (findingDeadDevices) {
            findingDeadDevices = false;

            for (i = 0; i < devicesOnClient.size(); i++) {
                if (!TDeviceListContains(devicesOnServer, devicesOnClient.elementAt(i))) {
                    removeDevice(devicesOnClient.elementAt(i));
                    findingDeadDevices = true;
                }
            }
        }

        //add new devices
        for(i = 0; devicesOnServer != null && i < devicesOnServer.size(); i++) {
            if( !TDeviceListContains(devicesOnClient, devicesOnServer.elementAt(i)) ) {
                addDevice( devicesOnServer.elementAt(i) );
            }
        }
        fireStatusEvent(DeviceManagerStatus.Idle);
    }

    public boolean TDeviceListContains(Vector<TDevice> list, TDevice device) {
        if(list == null)
            return false;

        for(int i = 0; i < list.size(); i++) {
            if(list.elementAt(i).deviceID.equals(device.deviceID)) {
                return true;
            }
        }
        return false;
    }

    private synchronized void forwardDeviceEvent(edu.stanford.atom.sti.corba.Pusher.TDeviceRefreshEvent event) {
        TDevice device = getTDevice(event.deviceID);
         
        if(device == null)
            return;
        
        Device dev = null;
        boolean eventSentToDevice = false;
        
         if( TDeviceListContains(devicesOnClient, device) ) {
             for (int i = 0; i < deviceCollections.size(); i++) {
                dev = deviceCollections.elementAt(i).getDevice(device);
                
                if( dev != null ) {
                    
                    DeviceEvent evt = new DeviceEvent(dev, event);
                    
                    if(!eventSentToDevice) {
                        //only send to the device once
                        dev.handleEvent(evt);
                        eventSentToDevice = true;
                    }
                    deviceCollections.elementAt(i).handleDeviceEvent(evt);
                }
             }
         }
         else if(device != null) {
            refreshDeviceLists();
            forwardDeviceEvent(event);
        }
    }

//    private void refreshDevice(TDevice device) {
//        if(device == null)
//            return;
//
//        Device dev = null;
//
//        if( TDeviceListContains(devicesOnClient, device) ) {
//            for (int i = 0; i < deviceCollections.size(); i++) {
//                dev = deviceCollections.elementAt(i).getDevice(device);
//
//                if( dev != null ) {
//                    deviceCollections.elementAt(i).refreshDevice( dev );
//                }
//            }
//        }
//        else if(device != null) {
//            refreshDeviceLists();
//            refreshDevice(device);
//        }
//    }

    public void installServants(ServerConnectionEvent event) {
        server = event.getServerConnection();
        fireStatusEvent(DeviceManagerStatus.Idle);
        forwardInstallServantsEvent(event);
        refreshDeviceLists();
    }
    public void uninstallServants(ServerConnectionEvent event) {
        server = null;
        fireStatusEvent(DeviceManagerStatus.NotConnected);
        forwardUninstallServantsEvent(event);
        refreshDeviceLists();
    }
    
    private synchronized void fireStatusEvent(DeviceManagerStatus status) {
        for(int i = 0; i < deviceCollections.size(); i++) {
            deviceCollections.elementAt(i).setDeviceManagerStatus(status);
        }
    }
    private synchronized void forwardInstallServantsEvent(ServerConnectionEvent event) {
        for(int i = 0; i < deviceCollections.size(); i++) {
            deviceCollections.elementAt(i).installServer(event);
        }        
    }
    private synchronized void forwardUninstallServantsEvent(ServerConnectionEvent event) {
        for(int i = 0; i < deviceCollections.size(); i++) {
            deviceCollections.elementAt(i).uninstallServer(event);
        }        
    }
    private void addDevice(TDevice device) {
        if(device == null)
            return;

        if( !TDeviceListContains(devicesOnClient, device) ) {

            devicesOnClient.addElement(device);
            Device newDevice = new Device(device, server);

            for (int i = 0; i < deviceCollections.size(); i++) {
                deviceCollections.elementAt(i).addDevice(newDevice);  //adds conditionally on the DeviceCollection implementation
            }
        }
        else {
            //If this device is already present on the client, remove it from
            //all collections and add the new version;
            removeDevice(device);
            addDevice(device);
        }
    }
    private void removeDevice(TDevice device) {
        if(device == null)
            return;

        devicesOnClient.removeElement(device);
        
        for(int i = 0; i < deviceCollections.size(); i++) {
            deviceCollections.elementAt(i).removeDevice(device);
        }
    }

}
