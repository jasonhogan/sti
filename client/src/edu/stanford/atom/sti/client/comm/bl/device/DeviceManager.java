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

/**
 *
 * @author Jason
 */
public class DeviceManager implements ServerConnectionListener {

    private STIServerConnection server = null;
    private Vector<DeviceCollection> deviceCollections = new Vector<DeviceCollection>();
    private Vector<TDevice> devicesOnClient = new Vector<TDevice>();
    
    public enum DeviceManagerStatus {Idle, Refreshing, NotConnected};
    
    public DeviceManager() {
     
    }
    
    public synchronized void addDeviceCollection(DeviceCollection deviceCollection) {
        if(!deviceCollections.contains(deviceCollection)) {
            deviceCollections.addElement(deviceCollection);
        }
    }
    public synchronized void removeDeviceCollection(DeviceCollection deviceCollection) {
        deviceCollections.removeElement(deviceCollection);
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
        if( !devicesOnClient.contains(device) ) {

            devicesOnClient.addElement(device);
            Device newDevice = new Device(device);

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
        devicesOnClient.removeElement(device);
        
        for(int i = 0; i < deviceCollections.size(); i++) {
            deviceCollections.elementAt(i).removeDevice(device);
        }
    }

    public synchronized void refreshDeviceLists() {

        fireStatusEvent(DeviceManagerStatus.Refreshing);
        
        int i;
        TDevice[] devices = new TDevice[0];
        
        if (server != null) {
            try {
                devices = server.getDeviceConfigure().devices();
            } catch (Exception e) {
            }
        }

        Vector<TDevice> devicesOnServer = new Vector<TDevice>(Arrays.asList(devices));
        
        //remove dead devices
        for(i = 0; i < devicesOnClient.size(); i++) {
            if( !devicesOnServer.contains( devicesOnClient.elementAt(i) ) ) {
                removeDevice( devicesOnClient.elementAt(i) );
            }
        }
        //add new devices
        for(i = 0; i < devicesOnServer.size(); i++) {
            if( !devicesOnClient.contains(devicesOnServer.elementAt(i)) ) {
                addDevice( devicesOnServer.elementAt(i) );
            }
        }
        fireStatusEvent(DeviceManagerStatus.Idle);
    }

    public void refreshDevice(TDevice device) {
        Device dev = null;
        
        if( devicesOnClient.contains(device) ) {
            for (int i = 0; i < deviceCollections.size(); i++) {
                dev = deviceCollections.elementAt(i).getDevice(device);
                
                if( dev != null ) {
                    deviceCollections.elementAt(i).refreshDevice( dev );
                }
            }
        }
        else {
            refreshDeviceLists();
            refreshDevice(device);
        }
    }
    
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
}
