/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.gui.ColorTable;

/**
 *
 * @author Susannah Dickerson
 */

import javax.swing.JTable;
import javax.swing.table.*;
import java.awt.Component;
//import java.awt.Color;

class ColorCellRenderer extends DefaultTableCellRenderer
{
    @Override
    public Component getTableCellRendererComponent(JTable table, Object value, boolean isSelected,
            boolean hasFocus, int row, int column)
    {
        super.getTableCellRendererComponent(table,value,isSelected,hasFocus,row,column);
        ColorTableModel colorTableModel =(ColorTableModel) table.getModel();

        setBackground(colorTableModel.bkgColor.get(row).get(column));

        return this;
    }
}

