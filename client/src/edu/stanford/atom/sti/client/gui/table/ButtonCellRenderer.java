/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.gui.table;

import javax.swing.table.*;
import java.awt.Component;
import javax.swing.*;
import java.util.Vector;

public class ButtonCellRenderer implements TableCellRenderer {
 // private TableCellRenderer __defaultRenderer;

    Vector<JButton> buttons;


  public ButtonCellRenderer(Vector<JButton> jButtons) {
      super();
      buttons = jButtons;
    //__defaultRenderer = renderer;
  }



  public Component getTableCellRendererComponent(JTable table, Object value,
						 boolean isSelected,
						 boolean hasFocus,
						 int row, int column)
  {
//      return this;
      return buttons.elementAt(table.convertRowIndexToView(row));
//    if(value instanceof Component)
//      return (Component)value;
//    return __defaultRenderer.getTableCellRendererComponent(
//	   table, value, isSelected, hasFocus, row, column);
  }
}
