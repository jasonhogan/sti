/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.comm.bl;

import edu.stanford.atom.sti.client.comm.corba.*;
/**
 *
 * @author Owner
 */
public class TVarMixedDecode {

    private TVarMixed variable = null;
    private String strValue = null;
    private String type = null;
    private int channel = -1;  //only used if the Var is a channel
    TChannel[] channels = null;
    
    public TVarMixedDecode(TVarMixed variable, TChannel[] channels) {
        this.variable = variable;
        this.channels = channels;
        decode();
    }
    
    public String getValue() {
        return strValue;
    }
    public String getType() {
        return type;
    }
    public int getChannel() {
        return channel;
    }
    
    public static TVarMixedDecode createTVarMixedDecode(TVarMixed variable, TChannel[] channels) {
        return new TVarMixedDecode(variable, channels);
    }
    
    private void decode() {
        switch(variable.discriminator().value()) {
            case TType._TypeChannel:
                channel = variable.channel();
                if(channel >= 0 && channel < channels.length) {
                    TChannel tChannel = channels[channel];
                    strValue = "ch(dev('" 
                            + tChannel.device.deviceName + "', "
                            + tChannel.device.address + ", "
                            + tChannel.device.moduleNum + "), "
                            + tChannel.channel + ")";
                } else {
                    strValue = "ch(?,?)";
                }
                type = "Channel";
                break;
            case TType._TypeNumber:
                type = "Number";
                strValue = "" + variable.number();
                break;
            case TType._TypeString:
                type = "String";
                strValue = variable.stringVal();
                break;
            case TType._TypeObject:
                type = "Object";
                strValue = variable.objectVal();
                break;
            case TType._TypeList:
                type = "List";
                TVarMixed[] list = variable.list();
                
                strValue = "[";
                for(int i = 0; i < list.length; i++) {
                    if(i != 0) {
                        strValue += ", ";
                    }
                    strValue += TVarMixedDecode.
                            createTVarMixedDecode(list[i], channels).getValue();
                }
                strValue += "]";
                break;
            default:
                strValue = "Error";
                type = "Error";
                break;
        }
    }
    
}
