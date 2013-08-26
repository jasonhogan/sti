/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.stanford.atom.sti.client.gui.table;

import edu.stanford.atom.sti.client.comm.bl.STINumberFormat;

/**
 *
 * @author Jason
 */
public class STITableNumberRenderer extends javax.swing.table.DefaultTableCellRenderer {

    STINumberFormat formatter;

    public STITableNumberRenderer() {
        super();
        setHorizontalAlignment(javax.swing.JLabel.RIGHT);
    }

    @Override
    public void setValue(Object value) {
        if (formatter == null) {
            formatter = new STINumberFormat();
        }
        try {
            setText((value == null) ? "" : formatter.valueToString(value));
        } catch (Exception e) {
            setText("");
        }
    }
}
