/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.comm.bl;

import edu.stanford.atom.sti.device.comm.corba.*;
//import edu.stanford.atom.sti.client.comm.corba.*;

/**
 *
 * @author Owner
 */
public class TValMixedDecode {

    private edu.stanford.atom.sti.client.comm.corba.TValMixed value = null;
    private String strValue = null;
    
    public TValMixedDecode(edu.stanford.atom.sti.client.comm.corba.TValMixed Value) {
        value = Value;
        decode();
    }
    
    public String getValue() {
        return strValue;
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
