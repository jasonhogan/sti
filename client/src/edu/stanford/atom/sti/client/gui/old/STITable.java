/** @file STITable.java
 *  @author Jonathan David Harvey
 *  @brief Source-file for the class "STITable"
 *  @section license License
 *
 *  Copyright (C) 2008 Jonathan Harvey <harv@stanford.edu>\n
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
 * 
 * @version 1.0
 * @see javax.swing
 * @see javax.swing.JTable
 */

package edu.stanford.atom.sti.client.gui.old;

import edu.stanford.atom.sti.client.gui.*;
import javax.swing.JTable;
import javax.swing.JTextField;
import javax.swing.JComboBox;
//import javax.swing.DefaultComboBoxModel;

import javax.swing.ListSelectionModel;
import javax.swing.RowFilter;

import javax.swing.table.TableRowSorter;
import javax.swing.text.Document;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;

import java.awt.Dimension;

import java.util.Vector;
//import java.awt.event.ActionEvent;
//import java.awt.event.ActionListener;

//import java.lang.reflect.Array.*;

//import atomconsole_v2.STITableModel;

public class STITable extends JTable {
       
    private Vector<Object[]> tableData;
    private Object[][] columnNamesObj;
    private Object[][] tableOptions;
    
    private String[] columnNames;
    
    private STITableModel stiTableModel = new STITableModel(
            new Object [][] {
                {null, null},
                {null, null},
                {null, null},
                {null, null}
            },
            new String [] {
                "Keys2", "Values2"
            });
            
    private TableRowSorter<STITableModel> newSorter;
    
    private JTextField filterField;
    private int filterByColumn = 0;
    

    public STITable() {
        super();
        setModel(stiTableModel);
        stiTableModel.setModelData(tableData, null , null);
    }


    public void setColumnNames(String[] colNames) {
        columnNames = colNames;
        stiTableModel.setModelData(tableData, columnNames, editableColumns());
    }
    
    public STITable(Vector<Object[]> tableDataIn){
        this(tableDataIn, new Object[0][0], new Object[0][0]);
        this.columnNamesObj = defaultColumnNames(tableDataIn.size());
        this.tableOptions = defaultOptions();
    }
    

    public STITable(Vector<Object[]> tableDataIn, String[] colNamesIn){
        this(tableDataIn, new Object[0][0], new Object[0][0]);
        this.columnNamesObj = columnNamesStrToObj(colNamesIn);
        this.tableOptions = defaultOptions();
        this.stiTableModel.setModelData(tableDataIn, colNamesIn, 
                new int[colNamesIn.length]);
    }


    public STITable(Vector<Object[]> tableDataIn, Object[][] colNamesIn){
        this(tableDataIn, colNamesIn, new Object[0][0]);
        this.tableOptions = defaultOptions();
    }
             

    public STITable(Vector<Object[]> tableDataIn, Object[][] colNamesIn,
            Object[][] tableOptionsIn) {
        super();
        
        
        // Set the parent class variables to the variables used in the constructor
        columnNamesObj = colNamesIn;
        tableData = tableDataIn;
        tableOptions = tableOptionsIn;
               

        stiTableModel.setModelData(tableData, retrieveColumnNames(), editableColumns());
        
        super.setModel(stiTableModel);
        
        int optionsLength = tableOptions.length;
        try {
            for(int i=0; i < optionsLength; i++) {
                String thisString = tableOptions[i][0].toString();
                if(thisString.equals("dimensionInPixels")){
                    if(tableOptions[i][1] instanceof Dimension) {
                        super.setPreferredScrollableViewportSize((Dimension)tableOptions[i][1]);
                    }
                } else if (thisString.equals("isSortable")){
                    if(tableOptions[i][1] instanceof Boolean) {
                        if((Boolean)tableOptions[i][1]){
                            newSorter = new TableRowSorter<STITableModel>(stiTableModel);
                            super.setRowSorter(newSorter);
                        }
                    }
                } else if(thisString.equals("isScrollableX")){
                    if(tableOptions[i][1] instanceof Boolean) {
                       //(Boolean)tableOptions[i][1]
                    }
                } else if(thisString.equals("isScrollableY")){
                    if(tableOptions[i][1] instanceof Boolean) {
                        //(Boolean)tableOptions[i][1]
                    }
                } else if(thisString.equals("isFillViewport")){
                    if(tableOptions[i][1] instanceof Boolean) {
                        super.setFillsViewportHeight((Boolean)tableOptions[i][1]);
                    }
                } else if(thisString.equals("isSingleSelection")){
                    if(tableOptions[i][1] instanceof Boolean) {
                        if((Boolean)tableOptions[i][1]){
                            super.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
                        }
                    }
                } /*else if(thisString.equals("isFilterable")){
                    if(tableOptions[i][1] instanceof javax.swing.JTextField) {
                        super.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
                        
                    }
                }*/
            }
        } catch(ArrayIndexOutOfBoundsException e) {
            System.out.println("Error: " + e);
        }
        
        int colLength = columnNamesObj.length;
        try {
            for(int i=0; i < colLength; i++) {
                if(columnNamesObj[i][1] instanceof Boolean){
                    if((Boolean)columnNamesObj[i][1]){
                        // Set that column or that column's cells to be editable
                        //super.set
                    }
                }
            }
        } catch(ArrayIndexOutOfBoundsException e) {
            System.out.println("Error: " + e);
        }    
    }
    
    
    public void addRow(Object[] newRow){
        
        tableData.add(newRow);
//        Vector<Object[]> currentData = stiTableModel.getModelData();
        
  //      currentData.add(newRow);
        stiTableModel.setChangedData(tableData);
    }
    
    public String[] retrieveColumnNames() {
        
        int objArrayLength = columnNamesObj.length;
        String[] columnNamesString = new String[objArrayLength];
        
        // Not as elegant as it could be but it should still work
        for(int i=0; i<objArrayLength; i++) {
            if(columnNamesObj[i][0] instanceof String){
                columnNamesString[i] = (String)columnNamesObj[i][0];
            }
        }        
        return columnNamesString;
   }
    
    private int[] editableColumns() {
        //There's probably a MUCH better way to do this but I'll have to think about it
        
        int[] edColsInt;
        int edColsCounter = 0;
        int edColsPlacement = 0;

        // Determine the length that the array we're going to return has to be.
        // I'm doing it in an array b/c I'm used to Perl, in which arrays don't
        // totally suck like they apparently do in Java.
        for(int i=0; i<columnNamesObj.length; i++) {
            if(columnNamesObj[i][1] instanceof Boolean){
                if((Boolean)columnNamesObj[i][1]){
                    edColsCounter++;
                }
            }
        }
        
        // Initialize the edColsInt array now that we know the length
        edColsInt = new int[edColsCounter];

        // Now that we have an array to return, re-cycle through the object array
        // and populate the edColsInt array with the column #'s to be editable
        for(int i=0; i<columnNamesObj.length; i++) {
            if(columnNamesObj[i][1] instanceof Boolean){
                if((Boolean)columnNamesObj[i][1]){
                    edColsInt[edColsPlacement] = i;
                    edColsPlacement++;
                }
            }
        }
        
        return edColsInt;
    }
    
    private Object[][] defaultColumnNames(int arraySize) {
        Object[][] dCN = new Object[arraySize][2];
        for(int i=0; i<arraySize; i++) {
            dCN[i][0] = "Column #" + i;
            dCN[i][1] = new Boolean(false);
        }
        return dCN;
    }
    
    private Object[][] columnNamesStrToObj(String[] colNamesStr){
        // Also here, probably a better way to do this but this is how I know
        Object[][] oA = new Object[colNamesStr.length][2];
        for(int i=0; i<colNamesStr.length; i++){
            oA[i][0] = colNamesStr[i];
            oA[i][1] = new Boolean(false);
        }
        return oA;
    }
    
    private Object[][] defaultOptions(){
        Object[][] tO = {
            {"dimensionInPixels", new Dimension(500, 70)},
            {"isSortable", new Boolean(true)},
            {"isScrollableX", new Boolean(false)},
            {"isScrollableY", new Boolean(false)},
            {"isFillViewport", new Boolean(true)},
            {"isSingleSelection", new Boolean(true)}
        };
        return tO;
    }
    

    public void setFilterField(JTextField jTF, Document filterDocument) {
        filterField = jTF;
        //System.out.println("Filtering Document: " + filterDocument.TitleProperty);
        filterDocument.addDocumentListener(
            new DocumentListener() {
                public void changedUpdate(DocumentEvent e) {
                    newFilter();
                }
                public void insertUpdate(DocumentEvent e) {
                    newFilter();
                }
                public void removeUpdate(DocumentEvent e) {
                    newFilter();
                }
        });
    }
    
    private void newFilter() {
        RowFilter<STITableModel, java.lang.Object> rf = null;
        //If current expression doesn't parse, don't update.
        if(filterField != null){
            try {
                // Case sensitive, which is less than ideal, but difficult to fix here
                rf = RowFilter.regexFilter(filterField.getText(), filterByColumn);
            } catch (java.util.regex.PatternSyntaxException e) {
                System.err.println("Error in CreateTable.newFilter(): " + e);
                return;
            }
            try {
                newSorter.setRowFilter(rf);
            } catch(Exception e) {
                System.err.println("Error in CreateTable.newFilter(): " +
                        "newsorter.setRowFilter threw exception:\n" + e);
            }
        }
    }
    
    public void filterColumnSelectionEvent(java.awt.event.ActionEvent evt) {
        if(evt.getSource() instanceof JComboBox) {
                JComboBox cb = (JComboBox)evt.getSource();
                if(cb.getSelectedIndex() > -1) {
                    filterByColumn = cb.getSelectedIndex();
                }
        }
    }
    
}
