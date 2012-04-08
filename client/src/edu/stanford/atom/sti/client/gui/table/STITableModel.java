/** @file STITableModel.java
 *  @author Jason Michael Hogan
 *  @brief Source-file for the class STITableModel
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

import javax.swing.table.DefaultTableModel;
import javax.swing.table.TableModel;
import java.util.Vector;

public class STITableModel extends DefaultTableModel implements TableModel {

    private Vector<Boolean> dataEditable = new Vector<Boolean>();
    private Vector<Boolean> dataVisible = new Vector<Boolean>();
    
    public STITableModel() {
        super();
    }

    public STITableModel(Vector columnNames, int rowCount) {
        super(columnNames, rowCount);
    }
    
    public boolean isColumnVisible(int column) {
        if(column >= 0 && column < dataVisible.size()) {
            return dataVisible.elementAt(column);
        }
        return false;
    }



    @Override
    public Class getColumnClass(int column) {
        Class columnClass = null;
        Object obj = null;
        if (dataVector.size() > 0 && column >= 0) {
            if (((Vector) dataVector.elementAt(0)).size() > column) {
                obj = getValueAt(0, column);
                if(obj != null) {
                   columnClass = obj.getClass();
                }
            }
        }
        if (columnClass != null) {
            return columnClass;
        }
        return Object.class;
    }
    
    public void removeAllRows() {
        this.dataVector.clear();
        fireTableDataChanged();
    }
        
    @Override
    public boolean isCellEditable(int row, int column) {
        if(column >= 0 && column < dataEditable.size())
            return dataEditable.elementAt(column);
        return false;
    }
    
    public void setEditableColumns(boolean[] isEditable) {
       for(int i = 0; (i < dataEditable.size() && i < isEditable.length); i++) {
           dataEditable.setElementAt(isEditable[i], i);
       }
    }

    public void setEditable(int column, boolean isEditable) {
        if(column >= 0 && column < dataEditable.size()) {
            dataEditable.setElementAt(isEditable, column);
        }
    }
    
    public void setVisible(int column, boolean isVisible) {
        if(column >= 0 && column < dataEditable.size()) {
            dataVisible.setElementAt(isVisible, column);
        }
        fireTableStructureChanged();
    }
    
    @Override
    public void addColumn(Object columnName, Vector columnData) {
        super.addColumn(columnName, columnData);
        dataEditable.addElement(true);
        dataVisible.addElement(true);
        fireTableStructureChanged();
    }
    
    @Override
    public int getRowCount() {
        return dataVector.size();
    }
    public void setDataVector(Vector dataVector) {
        super.setDataVector(dataVector, columnIdentifiers);
    }
    
    @Override
    public void setDataVector(Vector dataVector, Vector columnIdentifiers) {

        if(dataEditable != null)
            dataEditable.clear();
        if(dataVisible != null)
            dataVisible.clear();

        // All columns are editable by default
        for(int i = 0; i < columnIdentifiers.size(); i++) {
            dataEditable.addElement(true);
            dataVisible.addElement(true);
        }
        
        super.setDataVector(dataVector, columnIdentifiers);
    }
}
