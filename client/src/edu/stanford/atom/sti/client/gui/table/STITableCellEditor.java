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

import java.util.Vector;
//import java.awt.Component;
import javax.swing.JTextField;
import javax.swing.JComponent;
import javax.swing.JTable;
import javax.swing.JComboBox;
import javax.swing.DefaultCellEditor;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

public class STITableCellEditor extends DefaultCellEditor  {
    // This is the component that will handle the editing of the cell value
    
    public STITableCellEditor() {
        super(new JTextField());
    }
    
    private String[] values = new String[]{"item1", "item2", "item3"};

    private JComponent component = new JTextField();
    private JComponent comboComponent = new JComboBox(values);
    
    private JComponent compon;
    
    private int currentRow;
    
    private Vector<Object[]> comboBoxData = new Vector<Object[]>();
    
    private Object[] test = new Object[]{1,2,new String[]{"1","2"}};
    
    public void installComboBoxEditor(int rowIndex, int colIndex, String[] values) {
//        comboBoxData.add(new Object[]{rowIndex, colIndex, values});
          comboBoxData.add(new Object[]{rowIndex, colIndex, new JComboBox(values)});
          
          setClickCountToStart(1);
/*           
        ((JComboBox) comboBoxData.lastElement()[2]).addItemListener(new ItemListener() {

            public void itemStateChanged(ItemEvent e) {
                System.out.println("item event");
                fireEditingStopped();
            }
        });
*/                  
     ((JComboBox)comboBoxData.lastElement()[2]).addActionListener(new ActionListener() {
    
              public void actionPerformed(ActionEvent e) {
                  fireEditingStopped();
              }
          });


          
          
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
            for(int i = 0; i < comboBoxData.size(); i++) {
                    if (currentRow == comboBoxData.elementAt(i)[0]) {
          //              compon = new JComboBox((String[])comboBoxData.elementAt(i)[2]);
                        
                        compon = (JComboBox)comboBoxData.elementAt(i)[2];
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

        for(int i = 0; i < comboBoxData.size(); i++) {
            if (currentRow == comboBoxData.elementAt(i)[0]) {
                return ((JComboBox)compon).getSelectedItem();
            }
        }
        
        return ((JTextField)component).getText();

    }
}
