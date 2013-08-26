/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.gui.table;

import java.awt.Component;
import java.util.Vector;
import javax.swing.*;
import javax.swing.table.*;

public class RefreshBarCellRenderer implements TableCellRenderer {

    private Vector<JProgressBar> progressBars = 
            new Vector<JProgressBar>(0);

    public RefreshBarCellRenderer() {
      super();
    }

    public synchronized void setNumberOfRows(int rows, JTable table) {
        progressBars.clear();
//        progressBars = new Vector<JProgressBar>();
        for (int i = 0; i < rows; i++) {
            progressBars.addElement(new DisplayableJProgressBar(table));
        }
    }

    public synchronized JProgressBar getProgressBar(int row) {
        if(row < progressBars.size() && row >= 0) {
            return progressBars.elementAt(row);
        } else {
            //Should never happen since methods are synchronized.
            //Something went wrong; there aren't enough progress bars!
            //Probably initialization happened out of order somehow.
            //Fake it by adding a temporary JProgressBar.
            //setNumberOfRows should take care of things eventually.
            progressBars.addElement(new JProgressBar());
            return progressBars.lastElement();
        }
    }

    public Component getTableCellRendererComponent(
            JTable table,
            Object value,
            boolean isSelected,
            boolean hasFocus,
            int row,
            int column) {
    //    return progressBars.elementAt(table.convertRowIndexToView(row));
        return getProgressBar(table.convertRowIndexToModel(row));
        
//        if(rowModel < progressBars.size() && rowModel >= 0) {
//            return progressBars.elementAt(rowModel);
//        } else {
//            
//        }
    }
    
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

        @Override
        public void repaint() {
            //If you have access to the table you can force repaint like this.
            //Otherwise, you could trigger repaint in a timer at some interval
            if (jtable == null) {
                super.repaint();
            } else {
                jtable.repaint();
            }
        }
    }
    
}
