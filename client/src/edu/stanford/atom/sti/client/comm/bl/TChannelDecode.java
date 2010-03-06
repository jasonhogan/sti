/** @file TChannelDecode.java
 *  @author Jason Michael Hogan
 *  @brief Source-file for the class TChannelDecode
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

public class TChannelDecode {

    private TChannel channel = null;
    
    private String InputType = null;
    private String OutputType = null;
    
    private String ioType = null;
    private String channelType = null;
    
    
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
            case TValue._ValueVector:
                InputType = "Vector";
                break;
            case TValue._ValueNone:
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
            case TData._DataDouble:
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
