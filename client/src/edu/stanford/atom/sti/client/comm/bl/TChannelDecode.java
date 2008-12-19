/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.stanford.atom.sti.client.comm.bl;

import edu.stanford.atom.sti.device.comm.corba.*;
import edu.stanford.atom.sti.client.comm.corba.*;
/**
 *
 * @author Owner
 */
public class TChannelDecode {

    private TChannel channel = null;
    
    String InputType = null;
    String OutputType = null;
    
    String ioType = null;
    String channelType = null;    
    
    
    public TChannelDecode(TChannel Channel) {
        channel = Channel;
        decode();
    }
    
    public String IOType() {
        return ioType;
    }
    
    public String ChannelType() {
        return channelType;
    }
    
    private void decode() {

        
        switch (channel.inputType.value()) {
            case TValue._ValueDDSTriplet:
                InputType = "DDS";
                break;
            case TValue._ValueMeas:
                InputType = "Null";
                break;
            case TValue._ValueNumber:
                InputType = "Number";
                break;
            case TValue._ValueString:
                InputType = "String";
                break;
            default:
                InputType = "Unknown";
                break;
        }
        switch (channel.outputType.value()) {
            case TData._DataNone:
                OutputType = "Null";
                break;
            case TData._DataNumber:
                OutputType = "Number";
                break;
            case TData._DataPicture:
                OutputType = "Picture";
                break;
            case TData._DataString:
                OutputType = "String";
                break;
            default:
                OutputType = "Unknown";
                break;
        }
        // set channel type
        switch (channel.type.value()) {
            case TChannelType._Input:
                ioType = "Input";
                channelType = InputType;
                break;
            case TChannelType._Output:
                ioType = "Output";
                channelType = OutputType;
                break;
            case TChannelType._BiDirectional:
                ioType = "Input/Output";
                channelType = InputType + "/" + OutputType;
                break;
            default:
                ioType = "Unknown";
                channelType = InputType + "/" + OutputType;
                break;
        }
    }
}
