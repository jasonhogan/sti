/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.comm.bl.device;
import edu.stanford.atom.sti.corba.Types.TDevice;
import edu.stanford.atom.sti.client.comm.io.STIServerConnection;
import edu.stanford.atom.sti.corba.Types.TAttribute;
import edu.stanford.atom.sti.corba.Types.TChannel;


/**
 *
 * @author Jason
 */
public class Device {

    private TDevice tDevice;
    private STIServerConnection server = null;

    public Device(TDevice tDevice) {
        this.tDevice = tDevice;
    }

    public String name() {
        return tDevice.deviceName;
    }
    public String address() {
        return tDevice.address;
    }
    public short module() {
        return tDevice.moduleNum;
    }
    public boolean status() {
        boolean alive = false;
        
        try {
            alive = server.getDeviceConfigure().deviceStatus(tDevice.deviceID);
        } catch(Exception e) {
        }
        
        return alive;
    }
    public long ping() {
        long ping = -1;
        
        try {
            ping = server.getDeviceConfigure().devicePing(tDevice.deviceID);
        } catch(Exception e) {
        }
        
        return ping;
    }

    public boolean setAttribute(String key, String value) {
        boolean success = false;
        
        try {
            server.getDeviceConfigure().setDeviceAttribute(tDevice.deviceID, key, value);
        } catch(Exception e) {
        }
        
        return success;
    }
    public TAttribute[] getAttributes() {
        TAttribute[] attributes = null;
        
        try {
            attributes = server.getDeviceConfigure().getDeviceAttributes(tDevice.deviceID);
        } catch(Exception e) {
        }
        
        return attributes;
    }
    public TChannel[] getChannels() {
        TChannel[] channels = null;
        
        try {
            channels = server.getDeviceConfigure().getDeviceChannels(tDevice.deviceID);
        } catch(Exception e) {
        }
        
        return channels;
    }
    public String execute(String args) {
        String result = null;

        try {
            result = server.getCommandLine().executeArgs(tDevice.deviceID, args);
        } catch(Exception e) {
        }

        return result;
    }
    public void kill() {        
        try {
            server.getDeviceConfigure().killDevice(tDevice.deviceID);
        } catch(Exception e) {
        }
    }

    public void installSever(STIServerConnection server) {
        this.server = server;
    }
    public void uninstallSever() {
        server = null;
    }

    public boolean equals(Device device) {
        return equals(device.getTDevice());
    }
    public boolean equals(TDevice device) {
        return (
                (device.address.compareTo(tDevice.address) == 0)             && 
                (device.deviceContext.compareTo(tDevice.deviceContext) == 0) &&
                (device.deviceID.compareTo(tDevice.deviceID) == 0)           &&
                (device.deviceName.compareTo(tDevice.deviceName) == 0)       &&
                (device.moduleNum == tDevice.moduleNum)
                );
    }

    public TDevice getTDevice() {
        return tDevice;
    }
    
}
