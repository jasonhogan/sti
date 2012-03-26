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

public class ButtonCellEditor extends DefaultCellEditor {
    // This is the defaultComponent that will handle the editing of the cell value

    public ButtonCellEditor() {
        super(new JTextField());
    }
    private JComponent defaultComponent = new JTextField();
//    private JComponent button;
    private int currentRow;
//    private Vector<Object[]> buttonCells = new Vector<Object[]>();
    private Vector<JButton> buttonCells = new Vector<JButton>();

    public void installButtonEditor(int rowIndex, JButton button) {

//        buttonCells.add(new Object[]{rowIndex, button});
        buttonCells.add(button);

        setClickCountToStart(1);

//        ((JButton) buttonCells.lastElement()[1]).addActionListener(new ActionListener() {
        buttonCells.lastElement().addActionListener(new ActionListener() {

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

        //Find the button
        if (currentRow < buttonCells.size() && currentRow >= 0) {
            JButton button = buttonCells.elementAt(currentRow);
            if (button != null) {
                return button;
            }
        }
//        for (int i = 0; i < buttonCells.size(); i++) {
//            if (currentRow == buttonCells.elementAt(i)[0]) {
//                button = (JButton) buttonCells.elementAt(i)[1];
//                return button;
//            }
//        }

        ((JTextField) defaultComponent).setText((String) value);
        // Return the configured defaultComponent
        return defaultComponent;

    }

    // This method is called when editing is completed.
    // It must return the new value to be stored in the cell.
    @Override
    public Object getCellEditorValue() {

        //Find the button
        if (currentRow < buttonCells.size() && currentRow >= 0) {
            JButton button = buttonCells.elementAt(currentRow);
            if (button != null) {
                return button;
            }
        }
//        for (int i = 0; i < buttonCells.size(); i++) {
//            if (currentRow == buttonCells.elementAt(i)[0]) {
//                return ((JButton) button);
//            }
//        }

        return ((JTextField) defaultComponent).getText();

    }
}
