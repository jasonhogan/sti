/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.comm.bl.device;
import edu.stanford.atom.sti.corba.Types.TDevice;
import edu.stanford.atom.sti.client.comm.io.STIServerConnection;
import edu.stanford.atom.sti.corba.Types.TAttribute;
import edu.stanford.atom.sti.corba.Types.TChannel;
import edu.stanford.atom.sti.corba.Types.TPartner;
import edu.stanford.atom.sti.corba.Types.TDataMixed;
import edu.stanford.atom.sti.corba.Types.TValMixed;

/**
 *
 * @author Jason
 */
public class Device {

    private TDevice tDevice;
    private STIServerConnection server = null;

    private boolean attributesFresh = false;
    private boolean channelsFresh = false;
    private boolean partnersFresh = false;

    private TAttribute[] attributes = null;
    private TChannel[] channels = null;
    private TPartner[] partners = null;
    
    public Device(TDevice tDevice, STIServerConnection server) {
        this.tDevice = tDevice;
        this.server = server;
        
        refreshDevice();
    }

    public synchronized void handleEvent(DeviceEvent evt) {
        if(evt.type == evt.type.AttributeRefresh || evt.type == evt.type.Refresh) {
            attributesFresh = false;
        }
        if(evt.type == evt.type.PartnerRefresh || evt.type == evt.type.Refresh) {
            partnersFresh = false;
        }
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
            alive = server.getRegisteredDevices().deviceStatus(tDevice.deviceID);
        } catch(Exception e) {
        }
        
        return alive;
    }
    public long ping() {
        long ping = -1;
        
        try {
            ping = server.getRegisteredDevices().devicePing(tDevice.deviceID);
        } catch(Exception e) {
        }
        
        return ping;
    }

    public synchronized boolean setAttribute(String key, String value) {
        boolean success = false;
        
        try {
            success = server.getRegisteredDevices().setDeviceAttribute(tDevice.deviceID, key, value);
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
    public synchronized TPartner[] getPartners() {
        getPartnersFromServer();
        if(!partnersFresh) {
            
        }
        return partners;
    }
    
    private synchronized void getAttributesFromServer() {
        attributesFresh = true;

        try {
            attributes = server.getRegisteredDevices().getDeviceAttributes(tDevice.deviceID);
        } catch(Exception e) {
            attributesFresh = false;
            attributes = null;
        }
    }
    private synchronized void getChannelsFromServer() {
        channelsFresh = true;

        try {
            channels = server.getRegisteredDevices().getDeviceChannels(tDevice.deviceID);
        } catch(Exception e) {
            channelsFresh = false;
            channels = null;
        }
    }
    private synchronized void getPartnersFromServer() {
        partnersFresh = true;

        try {
            partners = server.getRegisteredDevices().getDevicePartners(tDevice.deviceID);
        } catch(Exception e) {
            partnersFresh = false;
            partners = null;
        }
    }
    private void refreshDevice() {
        getAttributesFromServer();
        getChannelsFromServer();
        getPartnersFromServer();
    }

    public TValMixed pythonStringToMixedValue(String pythonString) {
        boolean success = false;
        edu.stanford.atom.sti.corba.Types.TValMixedHolder valMixed = new edu.stanford.atom.sti.corba.Types.TValMixedHolder();
//        edu.stanford.atom.sti.corba.Types.TValMixed valMixed = new edu.stanford.atom.sti.corba.Types.TValMixed();

        if(pythonString == null){
//            valMixed.emptyValue(true);
            valMixed.value.emptyValue(true);
            return valMixed.value;
        }

        try {
            success = server.getParser().stringToMixedValue(pythonString, valMixed);
        } catch(Exception e) {
        }

        if(success) {
            try {
                valMixed.value.discriminator();
                /*
                switch(valMixed.value.discriminator().value()) {
                    case edu.stanford.atom.sti.corba.Types.TValue._ValueNone:
                        valueOut.emptyValue(true);
                        break;
                    case edu.stanford.atom.sti.corba.Types.TValue._ValueNumber:
                        valueOut.number(valMixed.value.number());
                        break;
                    case edu.stanford.atom.sti.corba.Types.TValue._ValueString:
                        valueOut.stringVal(valMixed.value.stringVal());
                        break;
                    case edu.stanford.atom.sti.corba.Types.TValue._ValueVector:
                        valueOut.vector(valMixed.value.vector());
                        break;
                }
                 */
            } catch (org.omg.CORBA.BAD_OPERATION b) {
                valMixed.value.emptyValue(true);
            }
        } else {
            valMixed.value.emptyValue(true);
        }

        return valMixed.value;
    }

    public TDataMixed read(short channel, TValMixed valueIn) {
        boolean success = false;
    
        edu.stanford.atom.sti.corba.Types.TDataMixedHolder data = new edu.stanford.atom.sti.corba.Types.TDataMixedHolder();

        try {
            success = server.getCommandLine().readChannel(tDevice.deviceID, channel, valueIn, data);
        } catch(Exception e) {
        }

        if(success) {
            try{
                data.value.discriminator();
            } catch(org.omg.CORBA.BAD_OPERATION b){
                data.value.outVal(true);
            }
        } else {
            data.value.outVal(true);
        }

        return data.value;
    }

    public boolean write(short channel,edu.stanford.atom.sti.corba.Types.TValMixed value) {
        boolean success = false;

        try {
            success = server.getCommandLine().writeChannel(tDevice.deviceID, channel, value);
        } catch(Exception e) {
        }

        return success;
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
            server.getRegisteredDevices().killDevice(tDevice.deviceID);
        } catch(Exception e) {
            e.printStackTrace();
        }
    }
    public void stop() {

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
