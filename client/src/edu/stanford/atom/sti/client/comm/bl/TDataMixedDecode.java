/** @file TDataMixedDecode.java
 *  @author Jason Michael Hogan
 *  @brief Source-file for the class TDataMixedDecode
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
import java.text.DecimalFormat;

public class TDataMixedDecode {
    private TDataMixed data = null;
    private String strData = null;

    public TDataMixedDecode(TDataMixed data) {
        this.data = data;
        decode();
    }
    
    public boolean isEmpty() {
        return data.discriminator().value() == TData._DataNone;
    }

    @Override
    public String toString() {
        return strData;
    }

    private void decode() {
        strData = print(data);
    }

    private String doubleToString(double d)
    {
        DecimalFormat formatDec = new DecimalFormat("##0.##");
        DecimalFormat formatSci = new DecimalFormat("##0.#E0");

        if (Math.abs(d) < 1000 && Math.abs(d) >= .01) {
            return formatDec.format(d);
        }
        else {
            return formatSci.format(d);
        }
    }

    private String print(TDataMixed dataMixed) {
        String result = "";
        switch(dataMixed.discriminator().value()) {
            case TData._DataVector:
                result += "(";
                for(int i = 0; i < dataMixed.vector().length; i++) {
                    if(i > 0) {
                        result += ",";
                    }
                    result += print( dataMixed.vector()[i] );
                }
                result += ")";
                break;
            case TData._DataBoolean:
                result = "" + dataMixed.booleanVal();
                break;
            case TData._DataDouble:
                result = doubleToString(dataMixed.doubleVal());
                break;
            case TData._DataFile:
                result = "" + dataMixed.file().fileName;
                break;
            case TData._DataLong:
                result = "" + dataMixed.longVal();
                break;
            case TData._DataOctet:
                result = "" + dataMixed.octetVal();
                break;
            case TData._DataPicture:
                result = "<picture>";
                break;
            case TData._DataString:
                result = dataMixed.stringVal();
                break;
            case TData._DataNone:
                result = "<Empty>";
                break;
            default:
                result = "Error";
                break;
        }
        return result;

    }
}
