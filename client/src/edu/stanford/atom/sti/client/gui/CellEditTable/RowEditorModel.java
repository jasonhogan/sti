/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.gui.CellEditTable;

/**
 * 
 * @author http://www.javaworld.com/javaworld/javatips/jw-javatip102.html
 */

import javax.swing.table.*;
import java.util.*;


public class RowEditorModel {
    private Hashtable<Integer,TableCellEditor> data;
    public RowEditorModel()
    {
        data = new Hashtable();
    }
    public void addEditorForRow(int row, TableCellEditor e)
    {
        data.put(row, e);
    }
    public void removeEditorForRow(int row)
    {
        data.remove(row);
    }
    public TableCellEditor getEditor(int row)
    {
        return (TableCellEditor)data.get(new Integer(row));
    }
}
