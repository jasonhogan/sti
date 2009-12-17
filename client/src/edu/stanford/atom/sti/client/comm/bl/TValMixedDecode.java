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

import edu.stanford.atom.sti.corba.Types.*;

public class TValMixedDecode {

    private TValMixed value = null;
    private String strValue = null;
    
    public TValMixedDecode(TValMixed Value) {
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
        strValue = print(value);
    }

    private String print(TValMixed valMixed) {
        
        String result = "";
        switch(valMixed.discriminator().value()) {
            case TValue._ValueVector:
                result += "(";
                for(int i = 0; i < valMixed.vector().length; i++) {
                    if(i > 0) {
                        result += ",";
                    }
                    result += print( valMixed.vector()[i] );
                }
                result += ")";
                break;
            case TValue._ValueNumber:
                result = "" + valMixed.number();
                break;
            case TValue._ValueString:
                result = valMixed.stringVal();
                break;
            case TValue._ValueMeas:
                result = "n/a";
            default:
                result = "Error";
                break;
        }
        return result;

    }
}
