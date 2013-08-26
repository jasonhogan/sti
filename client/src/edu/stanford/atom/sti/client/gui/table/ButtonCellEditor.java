/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.stanford.atom.sti.client.gui.table;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Vector;
import javax.swing.DefaultCellEditor;
import javax.swing.JButton;
import javax.swing.JComponent;
import javax.swing.JTable;
import javax.swing.JTextField;
import java.util.Hashtable;

public class ButtonCellEditor extends DefaultCellEditor {
    // This is the defaultComponent that will handle the editing of the cell value

    public ButtonCellEditor() {
        super(new JTextField());
    }
    private JComponent defaultComponent = new JTextField();
    private int currentRow;
//    private Vector<JButton> buttonCells = new Vector<JButton>();
    private Hashtable<Integer, JButton> buttonCells = new Hashtable<Integer, JButton>();

        
    public void clear() {
        buttonCells.clear();
    }
    
    public void installButtonEditor(int modelRowIndex, JButton button) {

        buttonCells.put(modelRowIndex, button);
//        buttonCells.add(button);

        setClickCountToStart(1);

        button.addActionListener(new ActionListener() {
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

//        if (isSelected) {
//            // cell (and perhaps other cells) are selected
//        }

        

        //Find the button
        if (rowIndex < table.getModel().getRowCount() && rowIndex >= 0) {
            currentRow = table.convertRowIndexToModel(rowIndex);
            
            //JButton button = buttonCells.elementAt(currentRow);
            JButton button = buttonCells.get(currentRow);
            if (button != null) {
                return button;
            }
        }

        ((JTextField) defaultComponent).setText((String) value);
        // Return the configured defaultComponent
        return defaultComponent;

    }

    // This method is called when editing is completed.
    // It must return the new value to be stored in the cell.
    @Override
    public Object getCellEditorValue() {
        
        //Find the button
        JButton button = buttonCells.get(currentRow);
        if (button != null) {
            return button;
        }
        
//        //Find the button
//        if (currentRow < buttonCells.size() && currentRow >= 0) {
//            JButton button = buttonCells.elementAt(currentRow);
//            if (button != null) {
//                return button;
//            }
//        }

        return ((JTextField) defaultComponent).getText();

    }
}
