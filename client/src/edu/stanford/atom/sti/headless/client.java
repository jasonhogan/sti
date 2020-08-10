/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.stanford.atom.sti.headless;

// STI shizzle
// TODO: A class to access this should have *no* knowledge of anything GUI.
// In the future, I should be forking this...
import edu.stanford.atom.sti.headless.*;
import edu.stanford.atom.sti.client.gui.state.*; // Bad!
import edu.stanford.atom.sti.client.comm.io.STIServerConnection;
import edu.stanford.atom.sti.client.comm.io.STIServerEventHandler;

//import edu.stanford.atom.sti.client.comm.bl.device.*; // Device
import edu.stanford.atom.sti.corba.Client_Server.*; // registeredDevices
import edu.stanford.atom.sti.corba.Types.*; // TDevice, etc.

// Java duders
import java.util.*;


/**
 *
 * @author bklopfer
 */
public class client {
    edu.stanford.atom.sti.corba.Types.TExpRunInfo experimentRunInfo;

    private STIServerEventHandler eventHandler = new STIServerEventHandler();
    private STIStateMachine stateMachine = new STIStateMachine();
    private STIServerConnection serverConnection = new STIServerConnection(stateMachine, eventHandler);
    
    // Map from the device's canonical name to the device itself.
    private Map<String, HeadlessDevice> cname_map = null;
    private HashMap<String, HashMap<String, Short> > channelMap = null;
    
    public void connect(String str) {
        serverConnection.setServerAddress(str);
        serverConnection.run();
        
        // Populate our variables.
        refresh();
    }
    
   public RegisteredDevices getRegisteredDevices() {
        return serverConnection.getRegisteredDevices();
    }
  
    public TDevice[] getRegisteredTDevices() {
        return serverConnection.getRegisteredDevices().devices();
    }
    
    public HeadlessDevice devicesyo(int idx) {
        TDevice[] tdevices = this.getRegisteredTDevices();
       // Device dev = new Device(tdevices[idx]);
        return new HeadlessDevice(tdevices[idx], serverConnection);
    }
    
    public String[] listDevices() {
        List<String> cnames = new ArrayList<String>();
        Iterator it = cname_map.entrySet().iterator();
        while (it.hasNext()) {
            Map.Entry pairs = (Map.Entry)it.next();
            cnames.add(pairs.getKey().toString());
        }
        
        // MATLAB prefers String[] to lists.
        String[] str = new String[cnames.size()];
        cnames.toArray(str);
        
        return str;
    }
    
    public void refresh() {
        this.nameDeviceMap();
        this.nameChannelMap();
    }
    
    // TODO: make private, invoke intelligently
    public void nameDeviceMap() {
        TDevice[] tds = this.getRegisteredTDevices();
        Map<String, HeadlessDevice> map = new HashMap<String, HeadlessDevice>();
        for (int i = 0; i < tds.length; i++) {
            HeadlessDevice dev = new HeadlessDevice(tds[i], serverConnection);
            map.put(dev.name(), dev);
        }        
        
        this.cname_map = map;
    }
    
    public void nameChannelMap() {
        HashMap<String, HashMap<String, Short> > map =
                new HashMap<String, HashMap<String, Short> >();
        for (HeadlessDevice dev : this.cname_map.values()) {
            HashMap<String, Short> chanmap = new HashMap<String, Short>();
            for (TChannel chan : dev.getChannels()) {
                chanmap.put(chan.channelName, chan.channel);
            }
            map.put(dev.name(), chanmap);
        }
        
        this.channelMap = map;
    }
    
    public short channelNum(String dev, String channel) {
        return channelMap.get(dev).get(channel);
    }
    
    /******* Reads *******/
 
    public TDataMixed read(String ident, short channel, TValMixed tval) {
        return this.cname_map.get(ident).read(channel, tval);
    }
    
    public TDataMixed read(String ident, short channel, double val) {
        TValMixed tval = new TValMixed();
        tval.number(val);
        
        return this.cname_map.get(ident).read(channel, tval);
    }
    
    public TDataMixed read(String ident, short channel, String val) {
        TValMixed tval = new TValMixed();
        tval.stringVal(val);
     //   tval.number(123);
        return this.cname_map.get(ident).read(channel, tval);
    }
    
    public TDataMixed read(String ident, short channel, double val[]) {
        TValMixed[] tvals = new TValMixed[val.length];
        for (int i = 0; i < val.length; i++) {
            tvals[i] = new TValMixed();
            tvals[i].number(val[i]);
        }
        
        TValMixed tval = new TValMixed();
        tval.vector(tvals);
        
        return this.cname_map.get(ident).read(channel, tval);
    }
    
    // For reading with no input arguments
    public TDataMixed read(String ident, short channel) {
        TValMixed tval = new TValMixed();
        tval.emptyValue(true);
        
        return this.cname_map.get(ident).read(channel, tval);
    }
    
    // Useful to refer to channels by their name
    public TDataMixed read(String ident, String channel, TValMixed val) {
        return this.read(ident, channelNum(ident, channel), val);
    }
    
    public TDataMixed read(String ident, String channel, double val) {
        return this.read(ident, channelNum(ident, channel), val);
    }
    
    public TDataMixed read(String ident, String channel, String val) {
        return this.read(ident, channelNum(ident, channel), val);
    }
    
    public TDataMixed read(String ident, String channel, double val[]) {
        return this.read(ident, channelNum(ident, channel), val);
    }
    public TDataMixed read(String ident, String channel) {
        return this.read(ident, channelNum(ident, channel));
    }
    
    public TValMixed getTValMixed() {
        return new TValMixed();
    }
    
    /******* Writes *******/
    
    public void write(String ident, short channel, double val) {
        TValMixed tval = new TValMixed();
        tval.number(val);
        this.cname_map.get(ident).write(channel, tval);
    }
    
    public void write(String ident, short channel, double val[]) {
        TValMixed[] tvals = new TValMixed[val.length];
        for (int i = 0; i < val.length; i++) {
            tvals[i] = new TValMixed();
            tvals[i].number(val[i]);
        }
        
        TValMixed tval = new TValMixed();
        tval.vector(tvals);
        
        this.cname_map.get(ident).write(channel, tval);
    }
    
    // By name
    public void write(String ident, String channel, double val) {
        this.write(ident, channelNum(ident, channel), val);
    }
    
    public void write(String ident, String channel, double val[]) {
        this.write(ident, channelNum(ident, channel), val);
    }
    
    /******* Execute *******/
    
    public String execute(String ident, String cmd) {
        return this.cname_map.get(ident).execute(cmd);
    }
    
    /******* Attributes *******/
    
    public boolean setAttribute(String ident, String key, String value) {
        return this.cname_map.get(ident).setAttribute(key, value);
    }
    
    public ArrayList<String[]> dumpAttributes() {
        ArrayList<String[]> attrStrings = new ArrayList<String[]>();
        for (HeadlessDevice dev : this.cname_map.values()) {
            TAttribute[] attrs = dev.getAttributes();
            for (TAttribute attr : attrs) {
                String devAttrStr[] = {dev.name(), attr.key, attr.value};
              //  System.out.println(dev.name());
                //System.out.println(attrs.toString());
                
                attrStrings.add(devAttrStr);
            }
        }
        
        
        return attrStrings;
    }
}