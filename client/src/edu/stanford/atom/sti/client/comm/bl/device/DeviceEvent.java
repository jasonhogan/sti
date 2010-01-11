/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.comm.bl.device;
import java.util.EventObject;


public class DeviceEvent extends EventObject {

    public enum DeviceEventType { Refresh, AttributeRefresh, ChannelRefresh };
    DeviceEventType type = null;

    public DeviceEvent(Device source, DeviceEventType type) {
        super(source);
        this.type = type;
    }
    public Device getDevice() {
        return (Device) super.getSource();
    }
    public DeviceEventType getType() {
        return type;
    }
}
