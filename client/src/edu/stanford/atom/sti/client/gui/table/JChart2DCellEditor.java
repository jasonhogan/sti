/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.stanford.atom.sti.client.gui.table;

import java.util.Vector;
//import java.awt.Component;
import javax.swing.JTextField;
import javax.swing.JComponent;
import javax.swing.JTable;
import javax.swing.DefaultCellEditor;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import info.monitorenter.gui.chart.ZoomableChart;

import javax.swing.event.MouseInputAdapter;
import java.awt.event.MouseEvent;



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
//        comboBoxData.add(new Object[]{rowIndex, colIndex, values});

        charts.add(chart);
        charts.lastElement().addMouseListener(new MouseInputAdapter() {
            @Override
            public void mouseReleased(MouseEvent e) {
                fireEditingStopped();
            }
        });


//        ((JButton) buttonCells.lastElement()[1]).addActionListener(new ActionListener() {
//
//            public void actionPerformed(ActionEvent e) {
//                fireEditingStopped();
//            }
//        });

    }

        // This method is called when a cell value is edited by the user.

    public void clear() {
        charts.clear();
    }
    
    @Override
    public JComponent getTableCellEditorComponent(JTable table, Object value,
           boolean isSelected, int rowIndex, int vColIndex) {
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
