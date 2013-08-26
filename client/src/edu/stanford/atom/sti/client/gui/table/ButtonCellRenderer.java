/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.gui.table;

import java.awt.Component;
import java.util.Vector;
import javax.swing.*;
import javax.swing.table.*;

public class ButtonCellRenderer implements TableCellRenderer {

    private Vector<JButton> buttons;

    public ButtonCellRenderer(Vector<JButton> jButtons) {
        super();
        buttons = jButtons;
    }

    public Component getTableCellRendererComponent(
            JTable table,
            Object value,
            boolean isSelected,
            boolean hasFocus,
            int row,
            int column) {
//        return buttons.elementAt(table.convertRowIndexToView(row));
        return buttons.elementAt(table.convertRowIndexToModel(row));
    }
}
