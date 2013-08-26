/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.gui.table;

/**
 *
 * @author Jason
 */

import info.monitorenter.gui.chart.ZoomableChart;
import java.awt.Component;
import java.util.Vector;
import javax.swing.*;
import javax.swing.table.*;

public class JChart2DCellRenderer implements TableCellRenderer {

    Vector<ZoomableChart> charts = new Vector<ZoomableChart>();

    public JChart2DCellRenderer() {
        super();
    }
    public void addChart(ZoomableChart chart) {
        charts.add(chart);
    }
    
    public void clear() {
        charts.clear();
    }

    public Component getTableCellRendererComponent(
            JTable table,
            Object value,
            boolean isSelected,
            boolean hasFocus,
            int row,
            int column) {
        
        charts.elementAt(table.convertRowIndexToView(row)).repaint();
        
        return charts.elementAt(table.convertRowIndexToView(row));
    }
}
