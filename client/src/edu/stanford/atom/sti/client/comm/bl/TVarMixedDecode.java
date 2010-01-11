/** @file TVarMixedDecode.java
 *  @author Jason Michael Hogan
 *  @brief Source-file for the class TVarMixedDecode
 *  @section license License
 *
 *  Copyright (C) 2008 Jason Hogan <hogan@stanford.edu>\n
 *  This file is part of the Stanford Timing Interface (STI).
 *
 *  The STI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The STI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the STI.  If not, see <http://www.gnu.org/licenses/>.
 */

package edu.stanford.atom.sti.client.comm.bl;

import edu.stanford.atom.sti.corba.Types.*;

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
