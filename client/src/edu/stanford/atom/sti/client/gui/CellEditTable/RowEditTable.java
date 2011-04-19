/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.gui.CellEditTable;

import javax.swing.*;
import javax.swing.table.*;
import java.util.Vector;


/**
 *
 * @author Susannah Dickerson
 */
public class RowEditTable extends JTable {

    protected RowEditorModel rm;

    public RowEditTable()
    {
        super();
        rm = null;
    }

    public RowEditTable(TableModel tm)
    {
        super(tm);
        rm = null;
    }

    public RowEditTable(TableModel tm, TableColumnModel cm)
    {
        super(tm,cm);
        rm = null;
    }

    public RowEditTable(TableModel tm, TableColumnModel cm,
         ListSelectionModel sm)
    {
        super(tm, cm, sm);
        rm = null;
    }

    public RowEditTable(int rows, int cols)
    {
        super(rows,cols);
        rm = null;
    }

    public RowEditTable(final Vector rowData, final Vector columnNames)
    {
        super(rowData,columnNames);
        rm = null;
    }

    public RowEditTable(final Object[][] rowData, final Object[] colNames)
    {
        super(rowData,colNames);
        rm = null;
    }

    public RowEditTable(TableModel tm, RowEditorModel rm)
    {
        super(tm, null, null);
        this.rm = rm;
    }

    public void setRowEditorModel(RowEditorModel rm)
    {
        this.rm = rm;
    }

    public RowEditorModel getRowEditorModel()
    {
        return rm;
    }

    @Override
    public TableCellEditor getCellEditor(int row, int col)
    {
        TableCellEditor tmpEditor = null;
           if (rm!=null)
               tmpEditor = rm.getEditor(row);
           if (tmpEditor!=null)
               return tmpEditor;
           return super.getCellEditor(row,col);

    }

}
