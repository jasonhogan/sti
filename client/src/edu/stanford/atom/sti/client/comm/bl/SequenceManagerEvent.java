/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.comm.bl;

import java.util.EventObject;
import java.util.Vector;

public class SequenceManagerEvent extends EventObject {

    private String errorText = null;

    public SequenceManagerEvent(Object source) {
        super(source);
    }

    public SequenceManagerEvent(Object source, String errorText) {
        super(source);
        this.errorText = errorText;
    }
    public String getErrorText() {
        return errorText;
    }

    public Vector< Vector<Object> > getSequenceTableData() {
        return ( (SequenceManager) getSource() ).getSequenceTableData();
    }

    public Vector<String> getSequenceTableColumnIdentifiers() {
        return ( (SequenceManager) getSource() ).getColumnIndentifiers();
    }
}
