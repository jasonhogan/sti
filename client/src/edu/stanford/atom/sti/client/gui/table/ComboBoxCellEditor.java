/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.gui.table;

import javax.swing.JComboBox;
import javax.swing.DefaultCellEditor;
import javax.swing.JTextField;

public class ComboBoxCellEditor extends DefaultCellEditor {

    public ComboBoxCellEditor() {
        super(new JTextField());
    }
}
