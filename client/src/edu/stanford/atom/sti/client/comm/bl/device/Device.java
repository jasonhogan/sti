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

    private boolean attributesFresh = false;
    private boolean channelsFresh = false;

    private TAttribute[] attributes = null;
    private TChannel[] channels = null;
    
    public Device(TDevice tDevice) {
        this.tDevice = tDevice;
        getAttributesFromServer();
        getChannelsFromServer();
    }

    public synchronized void handleEvent(DeviceEvent evt) {
        attributesFresh = false;
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

    public synchronized boolean setAttribute(String key, String value) {
        boolean success = false;
        
        try {
            server.getDeviceConfigure().setDeviceAttribute(tDevice.deviceID, key, value);
        } catch(Exception e) {
        }
        
        if (success) {
            attributesFresh = false;
        }

        return success;
    }
    public synchronized TAttribute[] getAttributes() {
        if(!attributesFresh) {
            getAttributesFromServer();
        }
        return attributes;
    }
    public synchronized TChannel[] getChannels() {
        if(!channelsFresh) {
            getChannelsFromServer();
        }
        return channels;        
    }
    private synchronized void getAttributesFromServer() {
        attributesFresh = true;

        try {
            attributes = server.getDeviceConfigure().getDeviceAttributes(tDevice.deviceID);
        } catch(Exception e) {
            attributes = null;
        }
    }
    private synchronized void getChannelsFromServer() {
        channelsFresh = true;

        try {
            channels = server.getDeviceConfigure().getDeviceChannels(tDevice.deviceID);
        } catch(Exception e) {
            channels = null;
        }
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
