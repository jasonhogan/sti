/** @file   GuiTableModel.java
 *  @author Jonathan David Harvey
 *  @brief Source-file for the class "GuiTableModel"
 *  @section license License
 *
 *  Copyright (C) 2008 Jonathan Harvey <harv@stanford.edu>\n
 *  Copyright (C) 2008 Jason Hogan <hogan@stanford.edu>\n
 *  Copyright (C) 2008 David Johnson <dmsj@stanford.edu>\n
 *  Copyright (C) 2008 Olaf Mandel <mandel@stanford.edu>\n
 *  This file is part of Stanford Timing Interface (STI).
 *
 *  STI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  STI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with STI.  If not, see <http://www.gnu.org/licenses/>.
 */

package edu.stanford.atom.sti.client.gui.table;

import javax.swing.table.AbstractTableModel;
import java.util.Vector;
//import atomconsole_v2.CreateTable;

class GuiTableModel extends AbstractTableModel {
    
    private String[] columnNames;
    private Vector<Object[]> data;
    //private Object[][] data;
    // Initialize as zero-length array to avoid null pointer exceptions later
    private int[] editableColumns = new int[0];
    
    public void setModelData(Vector<Object[]> vObj, String[] gStr, int[] eCol){
                columnNames = gStr;
                data = vObj;
                editableColumns = eCol;
                /* DEBUGGING: Result, WAI
                for(Object[] o : data)
                    for(Object o1 : o)
                        if(o1 instanceof String)
                            System.out.println("Table Data in Model is: " + (String)o1);
                for(String s : columnNames)
                    System.out.println("Column Name in Model: " + s);
                 */
    }
    /**
     * Mutator method to change the data in the model.
     * 
     * @param vObj
     */
    public void setChangedData(Vector<Object[]> vObj){
        data = vObj;        
    }
    
    /**
     * Accessor method for model data vector
     * 
     * @return
     */
    public Vector<Object[]> getModelData() {
        return data;
    }
            
    
    //private Object[][] tableData = new Object[0][0];
    //tableData = CreateTable.retrieveTableData();
    
    // These could be moved, but will essentially be what the code will fill in
    // in the event that the array size of the data exceeds the named columns.
    private String errorColName = "Undefined Column";    
 
        public int getColumnCount() {
            return columnNames.length;
        }

        public int getRowCount() {
            return data.size();
        }

    @Override
        public String getColumnName(int col){
            return columnNames[col];
        }

        public Object getValueAt(int row, int col) {
            Object[] targetObj = data.get(row);
            return targetObj[col];
        }

        /*
         * JTable uses this method to determine the default renderer/
         * editor for each cell.  If we didn't implement this method,
         * then the last column would contain text ("true"/"false"),
         * rather than a check box.
         */
    @Override
        public Class getColumnClass(int c) {
            return getValueAt(0, c).getClass();
        }

        /*
         * Don't need to implement this method unless your table's
         * editable.
         */
        
    @Override
        public boolean isCellEditable(int row, int col) {
            // Probably there's a much faster way to do this
            for (int i : editableColumns){
                if (col == i) {
                    return true;
                }                
            }            
            return false;
        }
         /* The above code is purely to make this online example different from the standard table model. I don't know
         * that we need to use an abstract table model at all, perhaps a simple one will do.
         * However, an abstract table model is probably more extensible.*/

         

        /*
         * Don't need to implement the following method unless your table's
         * data can change.
         */
    @Override
        public void setValueAt(Object value, int row, int col) {
            Boolean DEBUG = false;
            if (DEBUG) {
                System.out.println("Setting value at " + row + "," + col + " to " + value + " (an instance of " + value.getClass() + ")");
            }
            Object[] targetObj = data.get(row);
            targetObj[col] = value;
            fireTableCellUpdated(row, col);

            if (DEBUG) {
                System.out.println("New value of data:");
                printDebugData();
            }
        }

        private void printDebugData() {
            int numRows = getRowCount();
            int numCols = getColumnCount();

            for (int i = 0; i < numRows; i++) {
                System.out.print("    row " + i + ":");
                for (int j = 0; j < numCols; j++) {
                    System.out.print("  " + data.get(i)[j]);
                }
                System.out.println();
            }
            System.out.println("--------------------------");
        }

}
    
