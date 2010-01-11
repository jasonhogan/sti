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

    public abstract boolean isAllowedMember(Device device);
    public abstract void addDeviceToGUI(Device device);
    public abstract void removeDeviceFromGUI(Device device);
    public abstract void setDeviceManagerStatus(DeviceManager.DeviceManagerStatus status);
    public abstract void refreshDevice(Device device);
    
    public void addDevice(Device device) {
        //Ensure no duplicates
        if( !getDevices().contains(device) && isAllowedMember(device) ) {
           getDevices().addElement(device);
           addDeviceToGUI(device);
        }
        else {
            //If its a duplicate, remove old version and add the new version.
            removeDevice( device.getTDevice() );
            addDevice(device);
        }
    }
    public void removeDevice(TDevice device) {
        if( getDevices().contains(device) ) {
            removeDeviceFromGUI( getDevice(device) );
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
