/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */


package edu.stanford.atom.sti.client.gui.ColorTable;

import javax.swing.JTable;
import javax.swing.table.*;
import java.awt.Color;
import java.util.ArrayList;
import java.util.Vector;
/*
 *
 * @author Susannah Dickerson
 * Modified from http://www.codeguru.com/forum/archive/index.php/t-34478.html
 * by ananth100
 */
public class ColorTableModel extends AbstractTableModel {
    //bkgColor stores background colors of all cells in table
    public ArrayList<ArrayList<Color> > bkgColor = new ArrayList<ArrayList<Color> >();
    //isEditable stores true for editable cells and false for all others
    private ArrayList<ArrayList<Boolean> > isEditable = new ArrayList<ArrayList<Boolean> >();
    private Color baseColor;
    private Boolean defaultIsEditable = false;
    private DefaultTableModel tableModel;

    public ColorTableModel(JTable jTable, Color color)
    {
        jTable.setDefaultRenderer(Object.class, new ColorCellRenderer());

        tableModel = (DefaultTableModel) jTable.getModel();
        baseColor = color;
        int rowCount = jTable.getRowCount();
        int colCount = jTable.getColumnCount();

        for(int i = 0; i < rowCount; i++)
        {
            ArrayList<Color> tempColorList = new ArrayList<Color>();
            ArrayList<Boolean> tempIsEditable = new ArrayList<Boolean>();
            for(int j = 0; j < colCount; j++)
            {
                tempColorList.add(color);
                tempIsEditable.add(defaultIsEditable); // Non-editable by default

            }
            bkgColor.add(tempColorList);
            isEditable.add(tempIsEditable);
        }

    }


    public void addRow(Object[] rowData)
    {
        tableModel.addRow(rowData);
        fireTableRowsInserted(tableModel.getRowCount(), tableModel.getRowCount());

        //Add color
        int colCount = tableModel.getColumnCount();
        ArrayList<Color> tempColorList = new ArrayList<Color>();
        ArrayList<Boolean> tempIsEditable = new ArrayList<Boolean>();
        for (int i = 0; i < colCount; i++) {
            tempColorList.add(baseColor);
            tempIsEditable.add(defaultIsEditable);
        }
        bkgColor.add(tempColorList);
        isEditable.add(tempIsEditable);
    }


    public void addColumn(Object columnName)
    {
        tableModel.addColumn(columnName);
        fireTableStructureChanged();

        //Add color
        int rowCount = tableModel.getRowCount();
        for (int i = 0; i < rowCount; i++) {
            bkgColor.get(i).add(baseColor);
            isEditable.get(i).add(defaultIsEditable);
        }
    }


    public void addColumn(Object columnName, Object[] columnData)
    {
        tableModel.addColumn(columnName, columnData);
        fireTableStructureChanged();

        //Add color
        int rowCount = tableModel.getRowCount();
        for (int i = 0; i < rowCount; i++)
        {
            bkgColor.get(i).add(baseColor);
            isEditable.get(i).add(defaultIsEditable);
        }
    }


    public void insertRow(int row, Object[] rowData)
    {
        tableModel.insertRow(row, rowData);
        fireTableRowsInserted(row, row);

        int colCount = tableModel.getColumnCount();
        ArrayList<Color> tempList = new ArrayList<Color>();
        ArrayList<Boolean> tempIsEditable = new ArrayList<Boolean>();
        for (int i = 0; i < colCount; i++) {
            tempList.add(baseColor);
            tempIsEditable.add(defaultIsEditable);
        }
        bkgColor.add(row, tempList);
        isEditable.add(row, tempIsEditable);

    }

    public void moveRow(int start, int end, int to)
    {
        if (end < start)
            return;

        tableModel.moveRow(start, end, to);
        fireTableDataChanged();

        ArrayList<ArrayList <Color> > movingColorRows = new ArrayList<ArrayList <Color> >();
        ArrayList<ArrayList <Boolean> > movingEditRows = new ArrayList<ArrayList <Boolean> >();
        for (int i = end; i >= start; i--)
        {
            movingColorRows.add(bkgColor.remove(i));
            movingEditRows.add(isEditable.remove(i));
        }

        //add the
        while (!movingColorRows.isEmpty()){
            bkgColor.add(to,movingColorRows.remove(movingColorRows.size() - 1));
            isEditable.add(to,movingEditRows.remove(movingEditRows.size() - 1));
        }

    }


    public void removeRow(int row)
    {
        tableModel.removeRow(row);
        fireTableDataChanged();

        bkgColor.remove(row);
        isEditable.remove(row);
    }

    private void removeColumn(int col)
    {
        if (bkgColor.isEmpty())
            return;

        int rowCount = bkgColor.size();

        for (int i = 0; i < rowCount; i++) {
            bkgColor.get(i).remove(col);
            isEditable.get(i).remove(col);
        }

    }


    public void setColumnCount(int columnCount)
    {
        int currentCount = tableModel.getColumnCount();

        if (currentCount < columnCount)
        {
            for (int i = 0; i < (columnCount - currentCount); i++)
                this.addColumn("");
        } else if (currentCount > columnCount)
        {
            for (int i = 0; i < (currentCount - columnCount); i++)
            {
                this.removeColumn(currentCount - 1 - i);
            }
            tableModel.setColumnCount(columnCount);
            fireTableStructureChanged();
        }
    }


    public void setRowCount(int rowCount)
    {
        int currentCount = tableModel.getRowCount();

        if (currentCount < rowCount)
        {
            Object[] newRow = null;
            tableModel.addRow(newRow);
            for (int i = 0; i < (rowCount - currentCount); i++)
                this.addRow(newRow);
        } else if (currentCount > rowCount)
        {
            for (int i = 0; i < (currentCount - rowCount); i++)
            {
                this.removeRow(currentCount - 1 - i);
            }
        }
    }

    public void setEditable(int row, int column, java.lang.Boolean editable)
    {
        isEditable.get(row).set(column, editable);
    }

    public void setValueAt(Object aValue, int row, int column, Color color, Boolean editable)
    {
        this.setValueAt(aValue, row, column, color);
        setEditable(row, column, editable);
    }

    public void setValueAt(Object aValue, int row, int column, Color color)
    {
        tableModel.setValueAt(aValue,row,column);
        fireTableCellUpdated(row, column);
        
        bkgColor.get(row).set(column, color);
    }

    @Override
    public void setValueAt(Object aValue, int row, int column)
    {
        Color color = bkgColor.get(row).get(column);
        this.setValueAt(aValue,row,column,color);
    }

    public Object getValueAt(int row, int column)
    {
        return tableModel.getValueAt(row, column);
    }

    public int getRowCount() {
        return tableModel.getRowCount();
    }

    public int getColumnCount() {
        return tableModel.getColumnCount();
    }

    @Override
    public boolean isCellEditable(int row, int column)
    {
        if (isEditable.get(row).get(column))
            return true;
        else
            return false;

        //return isEditable.get(row).get(column);
    }
}
