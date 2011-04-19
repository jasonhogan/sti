/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.gui.CellEditTable;

import javax.swing.table.*;
import java.util.*;
/**
 *
 * @author Modified from http://www.javaworld.com/javaworld/javatips/jw-javatip102.html
 */
public class CellEditorModel {
private Hashtable<CompositeKey,TableCellEditor> data;
    public CellEditorModel()
    {
        data = new Hashtable();
    }
    public void addEditorForCell(int row, int col, TableCellEditor e)
    {
        CompositeKey key = new CompositeKey(row,col);
        data.put(key, e);
    }
    public void removeEditorForCell(int row, int col)
    {
        CompositeKey key = new CompositeKey(row,col);
        data.remove(key);
    }
    public TableCellEditor getEditor(int row, int col)
    {
        return (TableCellEditor)data.get(new CompositeKey(row,col));
    }
}
