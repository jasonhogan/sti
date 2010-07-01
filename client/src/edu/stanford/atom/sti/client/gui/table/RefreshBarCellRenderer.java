/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.gui.table;


import javax.swing.table.*;
import java.awt.Component;
import javax.swing.*;
import java.util.Vector;

public class RefreshBarCellRenderer implements TableCellRenderer {
 // private TableCellRenderer __defaultRenderer;


    public class DisplayableJProgressBar extends JProgressBar {
        private JTable jtable;
        public DisplayableJProgressBar(JTable table) {
            super();
            jtable = table;
        }
        
        @Override
        public boolean isDisplayable() {
            return true;
        }
        public void repaint() {
// If you have access to the table you can force repaint like this.
//Otherwize, you could trigger repaint in a timer at some interval
            if(jtable == null)
                super.repaint();
            else
                jtable.repaint();

        }
    }

    private Vector<DisplayableJProgressBar> progressBars = new Vector<DisplayableJProgressBar>(0);

  public RefreshBarCellRenderer() {
      super();

    //__defaultRenderer = renderer;
  }

  public void setNumberOfRows(int rows, JTable table) {
      progressBars.clear();
      progressBars = new Vector<DisplayableJProgressBar>();
      for(int i = 0; i < rows; i++) {
        progressBars.addElement(new DisplayableJProgressBar(table));
      }
  }
  
  public JProgressBar getProgressBar(int row) {
      return progressBars.elementAt(row);
  }

  public Component getTableCellRendererComponent(JTable table, Object value,
						 boolean isSelected,
						 boolean hasFocus,
						 int row, int column)
  {

      return progressBars.elementAt(table.convertRowIndexToView(row));
//    if(value instanceof Component)
//      return (Component)value;
//    return __defaultRenderer.getTableCellRendererComponent(
//	   table, value, isSelected, hasFocus, row, column);
  }
}
