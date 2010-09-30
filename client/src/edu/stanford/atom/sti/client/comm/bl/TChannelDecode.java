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

        
        switch (channel.outputType.value()) {
            case TValue._ValueVector:
                OutputType = "Vector";
                break;
            case TValue._ValueNone:
                OutputType = "Null";
                break;
            case TValue._ValueNumber:
                OutputType = "Number";
                break;
            case TValue._ValueString:
                OutputType = "String";
                break;
            default:
                OutputType = "Unknown";
                break;
        }
        switch (channel.inputType.value()) {

            case TData._DataNone:
                InputType = "Null";
                break;
            case TData._DataEmpty:
                InputType = "Empty";
                break;
            case TData._DataBoolean:
                InputType = "Boolean";
                break;
            case TData._DataFile:
                InputType = "File";
                break;
            case TData._DataLong:
            case TData._DataDouble:
                InputType = "Number";
                break;
            case TData._DataVector:
                InputType = "Vector";
                break;
            case TData._DataString:
                InputType = "String";
                break;
            default:
                InputType = "Unknown";
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
