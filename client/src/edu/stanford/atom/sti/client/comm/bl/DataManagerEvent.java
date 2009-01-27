/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.comm.bl;

import java.util.EventObject;
import java.util.Vector;

/**
 *
 * @author Owner
 */
public class DataManagerEvent extends EventObject {
    
    public DataManagerEvent(Object source) {
        super(source);
    }

    public Vector< Vector<Object> > getVariablesTableData() {
        return ( (DataManager)getSource() ).getVariablesTableData();
    }

    public Vector< Vector<Object> > getOverwrittenTableData() {
        return ( (DataManager)getSource() ).getOverwrittenTableData();

    }

    public Vector< Vector<Object> > getEventTableData() {
        return ( (DataManager)getSource() ).getEventTableData();
    }
}
