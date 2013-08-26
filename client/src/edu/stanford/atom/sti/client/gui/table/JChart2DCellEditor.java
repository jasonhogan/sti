/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.stanford.atom.sti.client.gui.table;

import info.monitorenter.gui.chart.ZoomableChart;
import java.awt.event.MouseEvent;
import java.util.Vector;
import javax.swing.DefaultCellEditor;
import javax.swing.JComponent;
import javax.swing.JTable;
import javax.swing.JTextField;
import javax.swing.event.MouseInputAdapter;



/**
 *
 * @author Jason
 */
public class JChart2DCellEditor extends DefaultCellEditor {

    Vector<ZoomableChart> charts = new Vector<ZoomableChart>();

    private int currentRow = 0;

    public JChart2DCellEditor() {
        super(new JTextField());
        setClickCountToStart(1);
    }

    public void addChart(ZoomableChart chart) {

        charts.add(chart);
        charts.lastElement().addMouseListener(new MouseInputAdapter() {
            @Override
            public void mouseReleased(MouseEvent e) {
                fireEditingStopped();
            }
        });
    }

    public void clear() {
        charts.clear();
    }
    
    // This method is called when a cell value is edited by the user.
    @Override
    public JComponent getTableCellEditorComponent(
            JTable table,
            Object value,
            boolean isSelected,
            int rowIndex,
            int vColIndex) {

        // 'value' is value contained in the cell located at (rowIndex, vColIndex)

        if (isSelected) {
            // cell (and perhaps other cells) are selected
        }

        currentRow = table.convertRowIndexToModel(rowIndex);
        
        return charts.elementAt(currentRow);
    }

    // This method is called when editing is completed.
    // It must return the new value to be stored in the cell.
    @Override
    public Object getCellEditorValue() {
        return charts.elementAt(currentRow);
    }
}
