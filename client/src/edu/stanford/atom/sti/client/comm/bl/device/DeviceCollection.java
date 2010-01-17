/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.comm.bl.device;
import edu.stanford.atom.sti.corba.Types.TDevice;
import edu.stanford.atom.sti.client.comm.io.ServerConnectionEvent;
import java.util.Vector;

/**
 *
 * @author Jason
 */
public abstract class DeviceCollection {

    private Vector<Device> devices = new Vector<Device>();
    private Vector<DeviceCollectionListener> listeners = new Vector<DeviceCollectionListener>();

    public abstract boolean isAllowedMember(Device device);

    public synchronized void addDeviceCollectionListener(DeviceCollectionListener listener) {
        if( !listeners.contains(listener) ) {
            listeners.addElement(listener);
        }
    }
    public synchronized void removeDeviceCollectionListener(DeviceCollectionListener listener) {
        listeners.removeElement(listener);
    }

    private synchronized void fireAddDeviceEvent(Device device) {
        for(int i = 0; i < listeners.size(); i++) {
            listeners.elementAt(i).addDevice(device);
        }
    }
    private synchronized void fireRemoveDeviceEvent(Device device) {
        for(int i = 0; i < listeners.size(); i++) {
            listeners.elementAt(i).removeDevice(device);
        }
    }
    private synchronized void fireRefreshDeviceEvent(DeviceEvent evt) {
        evt.getDevice().handleEvent(evt);
        for(int i = 0; i < listeners.size(); i++) {
            listeners.elementAt(i).refreshDevice(evt);
        }
    }
    private synchronized void fireDeviceManagerStatus(DeviceManager.DeviceManagerStatus status) {
        for(int i = 0; i < listeners.size(); i++) {
            listeners.elementAt(i).setDeviceManagerStatus(status);
        }
    }
   
    public void setDeviceManagerStatus(DeviceManager.DeviceManagerStatus status) {
        fireDeviceManagerStatus(status);
    }
    public void refreshDevice(Device device) {
        fireRefreshDeviceEvent(
                new DeviceEvent(device, DeviceEvent.DeviceEventType.Refresh) );
    }
    public void addDevice(Device device) {
        //Ensure no duplicates
        if( !getDevices().contains(device) && isAllowedMember(device) ) {
           getDevices().addElement(device);
           fireAddDeviceEvent(device);
        }
        else {
            //If its a duplicate, remove old version and add the new version.
            removeDevice( device.getTDevice() );
            addDevice(device);
        }
    }
    public void removeDevice(TDevice device) {
        if( getDevices().contains(device) ) {
            fireRemoveDeviceEvent( getDevice(device) );
            getDevices().remove(device);
        }
    }

    public Vector<Device> getDevices() {
        return devices;
    }
    public Device getDevice(TDevice device) {
        if(getDevices().contains(device)) {
            return getDevices().elementAt(
                    getDevices().indexOf(device));
        } else {
            return null;
        }
    }
    
    public void installServer(ServerConnectionEvent event) {
        for(int i = 0; i < getDevices().size(); i++) {
            getDevices().elementAt(i).installSever(event.getServerConnection());
        }
    }
    public void uninstallServer(ServerConnectionEvent event) {
        for(int i = 0; i < getDevices().size(); i++) {
            getDevices().elementAt(i).uninstallSever();
        }
    }
}
