/** @file STITableCellEditor.java
 *  @author Jason Michael Hogan
 *  @brief Source-file for the class STITableCellEditor
 *  @section license License
 *
 *  Copyright (C) 2008 Jason Hogan <hogan@stanford.edu>\n
 *  This file is part of the Stanford Timing Interface (STI).
 *
 *  The STI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The STI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the STI.  If not, see <http://www.gnu.org/licenses/>.
 */

package edu.stanford.atom.sti.client.gui.table;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Hashtable;
import java.util.Vector;
import javax.swing.DefaultCellEditor;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JTable;
import javax.swing.JTextField;

public class STITableCellEditor extends DefaultCellEditor  {
    //Handles the editing of the cell value using either a JTextField or
    //a JComboBox.  Keeps track of which cell in the table uses which editor
    //type.
    //By default uses the JTextField. Users must register for a JComboBox
    //using the installComboBoxEditor function.
    
    public STITableCellEditor() {
        super(new JTextField());
    }
    
    private JTextField textFieldEditor = new JTextField();
    private JComboBox comboBoxEditor;
    private int currentRow;
    
//    private final int comboBoxIndex = 2;
//    private final int rowIndexIndex = 0;
    
//    private Vector<Object[]> comboBoxData = new Vector<Object[]>();
    private Hashtable<Integer, JComboBox> comboBoxData = new Hashtable<Integer, JComboBox>();
    
    public void clear() {
        comboBoxData.clear();
    }
    
    public void installComboBoxEditor(int modelRowIndex, int modelColIndex, String[] values) {

        JComboBox comboBox =  new JComboBox(values);

//        comboBoxData.add(new Object[] {rowIndex, colIndex, new JComboBox(values)});

        setClickCountToStart(1);    //single click invokes the cell editor

        comboBox.addActionListener(
                new ActionListener() {
                    public void actionPerformed(ActionEvent e) {
                        fireEditingStopped();
                    }
                });

        comboBoxData.put(modelRowIndex, comboBox);
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

//        if (isSelected) {
//            // cell (and perhaps other cells) are selected
//        }
//
//        currentRow = table.convertRowIndexToModel(rowIndex);
//
//        //look to see if it's a combo box
//        for (int i = 0; i < comboBoxData.size(); i++) {
//            if (currentRow == comboBoxData.elementAt(i)[rowIndexIndex]) {
//                comboBoxEditor = (JComboBox) comboBoxData.elementAt(i)[comboBoxIndex];
//                return comboBoxEditor;
//            }
//        }
                
        //Find the button
        if (rowIndex < table.getModel().getRowCount() && rowIndex >= 0) {
            currentRow = table.convertRowIndexToModel(rowIndex);

            //JButton button = buttonCells.elementAt(currentRow);
            comboBoxEditor = comboBoxData.get(currentRow);
            if (comboBoxEditor != null) {
                return comboBoxEditor;
            }
        }
        
        comboBoxEditor = null;
        
        textFieldEditor.setText((String) value);
        // Return the configured component
        return textFieldEditor;

    }
    
    // This method is called when editing is completed.
    // It must return the new value to be stored in the cell.
    @Override
    public Object getCellEditorValue() {

        if(comboBoxEditor != null) {
            return comboBoxEditor.getSelectedItem();
        }
        
//        for (int i = 0; i < comboBoxData.size(); i++) {
//            if (currentRow == comboBoxData.elementAt(i)[rowIndexIndex]) {
//                return ((JComboBox) comboBoxEditor).getSelectedItem();
//            }
//        }

        return textFieldEditor.getText();

    }
}
