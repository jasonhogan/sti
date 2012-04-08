/** @file STITable.java
 *  @author Jason Michael Hogan
 *  @brief Source-file for the class STITable
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

import javax.swing.JTable;
import javax.swing.table.*;
import javax.swing.event.TableModelEvent;
import javax.swing.ListSelectionModel;
import javax.swing.JCheckBoxMenuItem;
import javax.swing.RowFilter;

import javax.swing.table.TableCellRenderer;
import java.util.HashMap;

public class STITable extends JTable {

    protected STITableModel stiDataModel;

    private HashMap<String, TableCellRenderer> columnRenders = new HashMap<String, TableCellRenderer>();


    private TableRowSorter<STITableModel> tableRowSorter = null;
//    private Vector<javax.swing.JCheckBoxMenuItem>

    /** Creates new form BeanForm */
    public STITable() {
        this(null, null, null);
    }

    public STITable(STITableModel dm, TableColumnModel cm, ListSelectionModel sm) {
        super(dm, cm, sm);

    // Set the model last, that way if the autoCreatColumnsFromModel has
    // been set above, we will automatically populate an empty columnModel
    // with suitable columns for the new model.
        if (dm == null) {
            dm = createDefaultDataModel();
        }
	setModel(dm);
        initializeLocalVars();
        updateUI();
        
        initComponents();
        //tableRowSorter = (TableRowSorter<STITableModel>)getRowSorter();

        tableRowSorter = new TableRowSorter<STITableModel>(getModel());
        setRowSorter(tableRowSorter);
    }

    public void resetFilter() {
        filterTable("");
    }
    public void filterTable(RowFilter filter, int... columns) {
        if (tableRowSorter != null && filter != null) {
            tableRowSorter.setRowFilter(filter);
        }
    }
    public void filterTable(String text, int... columns) {
        RowFilter<STITableModel, Object> filter = null;
        try {
            filter = RowFilter.regexFilter(text, columns);
        } catch (java.util.regex.PatternSyntaxException e) {
            return;
        }
        filterTable(filter, columns);
    }
//    public void filterTable(String text, int... columns) {
//
//        if (tableRowSorter != null) {
//            RowFilter<STITableModel, Object> filter = null;
//
//            try {
//                filter = RowFilter.regexFilter(text, columns);
//            } catch (java.util.regex.PatternSyntaxException e) {
//                return;
//            }
//            tableRowSorter.setRowFilter(filter);
//        }
//        //andFilter
//        //numberFilter
////        ComparisonType.AFTER
////        ComparisonType.BEFORE
//    }

    private class ColumnCheckBoxMenuItem extends JCheckBoxMenuItem {
        private int menuIndex = -1;
        
        public ColumnCheckBoxMenuItem(String text, int index, boolean enabled) {
            super(text, enabled);
            setMenuIndex(index);
        }

        public void setMenuIndex(int index) {
            menuIndex = index;
        }

        public int getMenuIndex() {
            return menuIndex;
        }
    }
    
    private void addColumnSelectionCheckBox(String name, int index, boolean enabled) {
        if (columnPopupMenu != null) {
            ColumnCheckBoxMenuItem menuItem = new ColumnCheckBoxMenuItem(name, index, enabled);
            menuItem.addItemListener(new java.awt.event.ItemListener() {

                public void itemStateChanged(java.awt.event.ItemEvent evt) {
                    checkBoxMenuItemItemStateChanged(evt);
                }
            });
            
            columnPopupMenu.add(menuItem);
        }
    }


    public void addColumnSelectionPopupMenu() {
        getTableHeader().addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                tableHeaderMouseClicked(evt);
            }
        });

    }

    //Displays the column-visible popup menu when the table header is right clicked
    private void tableHeaderMouseClicked(java.awt.event.MouseEvent evt) {
        if (evt.getButton() == java.awt.event.MouseEvent.BUTTON3) {
            columnPopupMenu.show(getTableHeader(), evt.getX(), evt.getY());
        }
    }


    //Event listener for column-visible popup menu
    private void checkBoxMenuItemItemStateChanged(java.awt.event.ItemEvent evt) {
        ColumnCheckBoxMenuItem item = ((ColumnCheckBoxMenuItem)evt.getItem());
        
        //If trying to hide a column, at least one column must remain visible
        if( !item.getState() ) {
            int numberVisible = 0;
            
            for(int i = 0; i < getModel().getColumnCount(); i++) {
                if(getModel().isColumnVisible(i))
                    numberVisible++;
            }
            if(numberVisible < 2) {
                item.setState(true);
                return;
            }
        }
        getModel().setVisible(item.getMenuIndex(), item.getState());
       // getColumnModel().getColumn(item.getMenuIndex()).getCellEditor();
    }
    
    @Override
    public STITableModel getModel() {
        return stiDataModel;
    }
    public void setTableCellRenderer(int column, TableCellRenderer renderer) {
        if(renderer == null) {
            return;
        }
        columnRenders.put(getColumnName(column), renderer);
        getColumn(getColumnName(column)).setCellRenderer(renderer);
   //     eventsTable.getColumn("Time").setCellRenderer(stiTableNumberFormat);
    }
    public TableCellRenderer getTableCellRenderer(int column) {
        return columnRenders.get(getColumnName(column));
    }

    public void setModel(STITableModel stiDataModel) {
        if (stiDataModel == null) {
            throw new IllegalArgumentException("Cannot set a null STITableModel");
	}
        if (this.stiDataModel != stiDataModel) {
	    STITableModel old = this.stiDataModel;
            if (old != null) {
                old.removeTableModelListener(this);
	    }
            this.stiDataModel = stiDataModel;
            stiDataModel.addTableModelListener(this);

            tableChanged(new TableModelEvent(stiDataModel, TableModelEvent.HEADER_ROW));

	    firePropertyChange("model", old, stiDataModel);

            if (getAutoCreateRowSorter()) {
                setRowSorter(new TableRowSorter<STITableModel>(stiDataModel));
            }
        }
    }

    @Override
    public void setAutoCreateRowSorter(boolean autoCreateRowSorter) {
        if(this.getRowSorter() == null) {
            super.setAutoCreateRowSorter(autoCreateRowSorter);
        }
    }
    
    @Override
    protected STITableModel createDefaultDataModel() {
        return new STITableModel();
    }

    @Override
    public void createDefaultColumnsFromModel() {
        STITableModel m = getModel();
        if (m != null) {
            // Remove any current columns
            TableColumnModel cm = getColumnModel();
            while (cm.getColumnCount() > 0) {
                cm.removeColumn(cm.getColumn(0));
	    }

            //clear the column popup menu before refreshing it
            if (columnPopupMenu != null) {
                columnPopupMenu.removeAll();
            }

            // Create new columns from the data model info
            for (int i = 0; i < m.getColumnCount(); i++) {
                if(m.isColumnVisible(i)) {
                    TableColumn newColumn = new TableColumn(i);
                    addColumn(newColumn);

                    TableCellRenderer renderer = columnRenders.get(getColumnName(convertColumnIndexToView(i)));
                    if (renderer != null) {
                        getColumn(getColumnName(convertColumnIndexToView(i))).setCellRenderer(renderer);
                    }
                   
                }
                addColumnSelectionCheckBox(m.getColumnName(i), i, m.isColumnVisible(i));
            }
        }
    }
    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        columnPopupMenu = new javax.swing.JPopupMenu();

        setModel(new javax.swing.table.DefaultTableModel(
            new Object [][] {

            },
            new String [] {

            }
        ));
    }// </editor-fold>//GEN-END:initComponents


    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JPopupMenu columnPopupMenu;
    // End of variables declaration//GEN-END:variables

}
