/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.comm.bl.device;
import java.util.EventObject;


public class DeviceEvent extends EventObject {

    public enum DeviceEventType { Refresh, AttributeRefresh, ChannelRefresh, PartnerRefresh, ErrorStream };
    DeviceEventType type = null;
    String message = "";

    public DeviceEvent(Device source, edu.stanford.atom.sti.corba.Pusher.TDeviceRefreshEvent event) {
        super(source);
        switch(event.type.value()){
            case edu.stanford.atom.sti.corba.Pusher.DeviceRefreshEventType._RefreshAttributes:
                type = DeviceEventType.AttributeRefresh;
                break;
            case edu.stanford.atom.sti.corba.Pusher.DeviceRefreshEventType._RefreshChannels:
                type = DeviceEventType.ChannelRefresh;
                break;
            case edu.stanford.atom.sti.corba.Pusher.DeviceRefreshEventType._RefreshPartners:
                type = DeviceEventType.PartnerRefresh;
                break;
            case edu.stanford.atom.sti.corba.Pusher.DeviceRefreshEventType._DeviceErrorStream:
                type = DeviceEventType.ErrorStream;
                message = event.errorMessage;
                break;
            case edu.stanford.atom.sti.corba.Pusher.DeviceRefreshEventType._RefreshDevice:
                type = DeviceEventType.Refresh;
                break;
            default:
                type = DeviceEventType.Refresh;
                break;
        }

    }
    public Device getDevice() {
        return (Device) super.getSource();
    }
    public DeviceEventType getType() {
        return type;
    }
    public String getErrorMessage() {
        return message;
    }
}
