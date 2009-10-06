/** @file TValMixedDecode.java
 *  @author Jason Michael Hogan
 *  @brief Source-file for the class TValMixedDecode
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

import edu.stanford.atom.sti.device.comm.corba.*;
//import edu.stanford.atom.sti.client.comm.corba.*;

public class TValMixedDecode {

    private edu.stanford.atom.sti.device.comm.corba.TValMixed value = null;
    private String strValue = null;
    
    public TValMixedDecode(edu.stanford.atom.sti.device.comm.corba.TValMixed Value) {
        value = Value;
        decode();
    }
    
    @Override
    public String toString() {
        return strValue;
    }
    
    public Double getDouble() {
        Double tempDouble = null;

        if( value.discriminator().value() == TValue._ValueNumber) {
            tempDouble = Double.valueOf( value.number() );
        }
        else {
            tempDouble = Double.valueOf(0.0 / 0.0);  //NaN
        }

        return tempDouble;
    }
    
        
    public Integer getInteger() {
        Integer tempInt = null;

        if( value.discriminator().value() == TValue._ValueNumber) {
            try {
                tempInt = Integer.valueOf( new String( "" + value.number() ) );
            } catch(NumberFormatException e) {
                tempInt = null;
            }
        }

        return tempInt;
    }
    
    private void decode() {
        switch(value.discriminator().value()) {
            case TValue._ValueDDSTriplet:
                strValue = "(" + 
                        value.triplet().ampl + "," + 
                        value.triplet().freq + "," +
                        value.triplet().phase + ")";
                break;
            case TValue._ValueNumber:
                strValue = "" + value.number();
                break;
            case TValue._ValueString:
                strValue = value.stringVal();
                break;
            case TValue._ValueMeas:
                strValue = "n/a";
            default:
                strValue = "Error";
                break;
        }
    }
}
