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
import javax.swing.JButton;
import javax.swing.DefaultCellEditor;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;


public class ButtonCellEditor extends DefaultCellEditor  {
    // This is the component that will handle the editing of the cell value

    public ButtonCellEditor() {
        super(new JTextField());
    }

    private JComponent component = new JTextField();

    private JComponent compon;

    private int currentRow;

    private Vector<Object[]> buttonCells = new Vector<Object[]>();

    public void installButtonEditor(int rowIndex, JButton button) {
//        comboBoxData.add(new Object[]{rowIndex, colIndex, values});
          
        
        buttonCells.add(new Object[]{rowIndex, button});

          setClickCountToStart(1);

          ((JButton)buttonCells.lastElement()[1]).addActionListener(new ActionListener() {

              public void actionPerformed(ActionEvent e) {
                  fireEditingStopped();
              }
          });

          //((JButton)buttonCells.lastElement()[1]).setText(name);

//          final int row = rowIndex;
//
//          ((JButton)buttonCells.lastElement()[1]).addActionListener(
//                  new java.awt.event.ActionListener() {
//            public void actionPerformed(java.awt.event.ActionEvent evt) {
//
//                deviceTab.measureButtonPressed(row);
//            }
//          });


    }

    // This method is called when a cell value is edited by the user.
    @Override
    public JComponent getTableCellEditorComponent(JTable table, Object value,
           boolean isSelected, int rowIndex, int vColIndex) {
            // 'value' is value contained in the cell located at (rowIndex, vColIndex)

            if (isSelected) {
                // cell (and perhaps other cells) are selected
            }

            currentRow = table.convertRowIndexToModel(rowIndex);

     //       System.out.println("(" + rowIndex + ", " + currentRow + ")");

            //look to see if it's a combo box
            for(int i = 0; i < buttonCells.size(); i++) {
                    if (currentRow == buttonCells.elementAt(i)[0]) {
          //              compon = new JComboBox((String[])comboBoxData.elementAt(i)[2]);

                        compon = (JButton)buttonCells.elementAt(i)[1];
            //            ((JComboBox)compon).setSelectedIndex(((JComboBox)compon).getSelectedIndex());

                        return compon;
                    }
           }

          ((JTextField)component).setText((String)value);
          // Return the configured component
          return component;

        }

    // This method is called when editing is completed.
    // It must return the new value to be stored in the cell.
    @Override
    public Object getCellEditorValue() {

        for(int i = 0; i < buttonCells.size(); i++) {
            if (currentRow == buttonCells.elementAt(i)[0]) {
                return ((JButton)compon);
            }
        }

        return ((JTextField)component).getText();

    }
}
