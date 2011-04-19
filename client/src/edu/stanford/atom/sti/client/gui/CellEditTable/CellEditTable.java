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
 * @author Modifed from http://www.javaworld.com/javaworld/javatips/jw-javatip102.html
 */
public class CellEditTable extends JTable {

    protected CellEditorModel cm;

    public CellEditTable()
    {
        super();
        cm = null;
    }

    public CellEditTable(TableModel tm)
    {
        super(tm);
        cm = null;
    }

    public CellEditTable(TableModel tm, TableColumnModel cm)
    {
        super(tm,cm);
        cm = null;
    }

    public CellEditTable(TableModel tm, TableColumnModel cm,
         ListSelectionModel sm)
    {
        super(tm, cm, sm);
        cm = null;
    }

    public CellEditTable(int rows, int cols)
    {
        super(rows,cols);
        cm = null;
    }

    public CellEditTable(final Vector rowData, final Vector columnNames)
    {
        super(rowData,columnNames);
        cm = null;
    }

    public CellEditTable(final Object[][] rowData, final Object[] colNames)
    {
        super(rowData,colNames);
        cm = null;
    }

    public CellEditTable(TableModel tm, CellEditorModel cm)
    {
        super(tm, null, null);
        this.cm = cm;
    }

    public void setCellEditorModel(CellEditorModel cm)
    {
        this.cm = cm;
    }

    public CellEditorModel getCellEditorModel()
    {
        return cm;
    }

    @Override
    public TableCellEditor getCellEditor(int row, int col)
    {
        TableCellEditor tmpEditor = null;
           if (cm!=null)
               tmpEditor = cm.getEditor(row,col);
           if (tmpEditor!=null)
               return tmpEditor;
           return super.getCellEditor(row,col);

    }

}
