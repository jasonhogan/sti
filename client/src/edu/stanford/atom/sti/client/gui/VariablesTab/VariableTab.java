/** @file VariableTab.java
 *  @author Jason Michael Hogan
 *  @brief Source-file for the class VariableTab
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

package edu.stanford.atom.sti.client.gui.VariablesTab;

import edu.stanford.atom.sti.client.comm.bl.*;
import edu.stanford.atom.sti.client.comm.io.ServerConnectionEvent;
import edu.stanford.atom.sti.client.comm.io.ServerConnectionListener;
import edu.stanford.atom.sti.corba.Client_Server.Parser;
import edu.stanford.atom.sti.corba.Types.TOverwritten;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import javax.swing.event.TableModelEvent;
import javax.swing.event.TableModelListener;

public class VariableTab extends javax.swing.JPanel implements DataManagerListener, ServerConnectionListener {
    
    private int[] filterColumnsVariables = new int[] {0};
    private int[] filterColumnsOverwritten = new int[] {0};
    private boolean filterOverwritten = true;
    private Parser parserRef = null;
    private boolean refreshingVariables = false;

    private int varNameCol = 0;
    private int varValueCol = 1;

    public VariableTab() {
        initComponents();

        setupVariablesTable();
        setupOverwrittenTable();
        setupFilter();
    }
    public void installServants(ServerConnectionEvent event) {
        parserRef = event.getServerConnection().getParser();
    }
    public void uninstallServants(ServerConnectionEvent event) {
        parserRef = null;
    }
    public void getData(DataManagerEvent event) {
        refreshingVariables = true;
        variablesTable.getModel().setDataVector( event.getVariablesTableData() );
        overwrittenTable.getModel().setDataVector( event.getOverwrittenTableData() );
        refreshingVariables = false;
    }
    
    public final void setupVariablesTable() {
        variablesTable.getModel().setDataVector(new Object[][]{},
                new String[]{"Name", "Value", "Type", "File", "Line"});

        variablesTable.getModel().setEditableColumns(
                new boolean[] {false, true, false, false, false});
        
        variablesTable.addColumnSelectionPopupMenu();

        variablesTable.getModel().addTableModelListener(new TableModelListener() {

            public synchronized void tableChanged(TableModelEvent evt) {
                if (evt.getType() == TableModelEvent.UPDATE && !refreshingVariables) {
//                    refreshingVariables = true;
                    if (evt.getColumn() == varValueCol) {
                        String val = (String) variablesTable.getModel().getValueAt(evt.getFirstRow(), varValueCol);
                        String name = (String) variablesTable.getModel().getValueAt(evt.getFirstRow(), varNameCol);

                        addOverwrittenVariable(name, val);
//                        variablesTable.getModel().setValueAt(val, evt.getFirstRow(), varValueCol);
//                        refreshingVariables = false;
                    }
                }
            }
        });
    }
    private void addOverwrittenVariable(String name, String val) {
        java.util.Vector data = overwrittenTable.getModel().getDataVector();
     
        int rows = overwrittenTable.getModel().getRowCount();
        
        int loc = -1;
        for(int i = 0; i < rows; i++) {
            if(overwrittenTable.getModel().getValueAt(i, 0).toString().equals(name)) {
                loc = i;
            }
        }
        TOverwritten[] over = null;
        if(loc < 0) {
            //not in the table
            over = new TOverwritten[rows + 1];
            over[rows] = new TOverwritten(name, val);
        } else {
            over = new TOverwritten[rows];
        }

        //populate the rest of the old table
        for(int i = 0; i < rows; i++) {
            over[i] = new TOverwritten();
            over[i].name = overwrittenTable.getModel().getValueAt(i, 0).toString();
            if(i == loc){
                over[i].value = val;
            } else {
                over[i].value = overwrittenTable.getModel().getValueAt(i, 1).toString();
            }
        }
        
        if(parserRef != null) {
            parserRef.overwritten(over);
        }
    }


    private void removeOverwrittenVariables(int[] removedRows) {
        int rows = overwrittenTable.getModel().getRowCount();

        int[] rowsToRemove = new int[removedRows.length];
        int numValid = 0;

        for (int i = 0; i < rowsToRemove.length; i++) {
            if (removedRows[i] < rows && removedRows[i] >= 0) {
                rowsToRemove[i] = removedRows[i];
                numValid++;
            } else {
                //invalid row
                rowsToRemove[i] = -1;
            }
        }
        if(numValid == 0) {
            return;
        }

        TOverwritten[] over = new TOverwritten[rows - numValid];

        int j = 0;
        boolean removeRow = false;
        for(int i = 0; i < rows; i++) {
            removeRow = false;
            for(int k = 0; k < rowsToRemove.length; k++) {
                if(i == rowsToRemove[k]) {
                    removeRow = true;
                }
            }
            if(!removeRow) {
                over[j] = new TOverwritten();
                over[j].name = overwrittenTable.getModel().getValueAt(i, 0).toString();
                over[j].value = overwrittenTable.getModel().getValueAt(i, 1).toString();
                j++;
            }
        }
        if(parserRef != null) {
            parserRef.overwritten(over);
        }
    }

    public final void setupOverwrittenTable() {
        overwrittenTable.getModel().setDataVector(new Object[][]{},
                new String[]{"Name", "Value"});

        overwrittenTable.getModel().setEditableColumns(
                new boolean[] {false, true});
        
        overwrittenTable.addColumnSelectionPopupMenu();
        
        overwrittenTable.addKeyListener(new java.awt.event.KeyAdapter() {
            @Override
            public void keyPressed(java.awt.event.KeyEvent e) {
                int[] selections = overwrittenTable.getSelectedRows();
                if (e.getKeyCode() == java.awt.event.KeyEvent.VK_DELETE) {
                    for (int i = 0; i < selections.length; i++) {
                        removeOverwrittenVariables(selections);
                    }
                }
            }
        });
        overwrittenTable.getModel().addTableModelListener(new TableModelListener() {

            public void tableChanged(TableModelEvent evt) {

                if (evt.getType() == TableModelEvent.UPDATE && !refreshingVariables) {
                    if (evt.getColumn() == 1) {
                        String val = (String) overwrittenTable.getModel().getValueAt(evt.getFirstRow(), 1);
                        String name = (String) overwrittenTable.getModel().getValueAt(evt.getFirstRow(), 0);

                        addOverwrittenVariable(name, val);
//                        System.out.println(name + ", " + val.toString());
                    }
                }
            }
        });

    }
    
    public final void setupFilter() {
        filterTextField.getDocument().addDocumentListener(
                new DocumentListener() {

                    public void changedUpdate(DocumentEvent e) {
                        update();
                    }

                    public void insertUpdate(DocumentEvent e) {
                        update();
                    }

                    public void removeUpdate(DocumentEvent e) {
                        update();
                    }
                    private void update() {
                        variablesTable.filterTable(filterTextField.getText(), filterColumnsVariables);
                        if(filterOverwritten) {
                            overwrittenTable.filterTable(filterTextField.getText(), filterColumnsOverwritten);
                        }
                    }
                });
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        filterPanel = new javax.swing.JPanel();
        filterTextField = new javax.swing.JTextField();
        columnSelectComboBox = new javax.swing.JComboBox();
        resetButton = new javax.swing.JButton();
        jSplitPane1 = new javax.swing.JSplitPane();
        jPanel2 = new javax.swing.JPanel();
        jScrollPane1 = new javax.swing.JScrollPane();
        variablesTable = new edu.stanford.atom.sti.client.gui.table.STITable();
        jPanel3 = new javax.swing.JPanel();
        jScrollPane2 = new javax.swing.JScrollPane();
        overwrittenTable = new edu.stanford.atom.sti.client.gui.table.STITable();
        clearOverwrittenButton = new javax.swing.JButton();

        filterPanel.setBorder(javax.swing.BorderFactory.createTitledBorder("Filter"));
        filterPanel.setMinimumSize(new java.awt.Dimension(100, 0));

        columnSelectComboBox.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "Name", "Value", "Type", "File", "Line", "All" }));
        columnSelectComboBox.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                columnSelectComboBoxActionPerformed(evt);
            }
        });

        resetButton.setText("Reset");
        resetButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                resetButtonActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout filterPanelLayout = new javax.swing.GroupLayout(filterPanel);
        filterPanel.setLayout(filterPanelLayout);
        filterPanelLayout.setHorizontalGroup(
            filterPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, filterPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(filterTextField, javax.swing.GroupLayout.DEFAULT_SIZE, 284, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(columnSelectComboBox, javax.swing.GroupLayout.PREFERRED_SIZE, 139, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(resetButton)
                .addContainerGap())
        );
        filterPanelLayout.setVerticalGroup(
            filterPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(filterPanelLayout.createSequentialGroup()
                .addGroup(filterPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(resetButton)
                    .addComponent(columnSelectComboBox, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(filterTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        jSplitPane1.setBorder(null);
        jSplitPane1.setDividerLocation(280);
        jSplitPane1.setDividerSize(7);
        jSplitPane1.setOrientation(javax.swing.JSplitPane.VERTICAL_SPLIT);
        jSplitPane1.setResizeWeight(0.5);
        jSplitPane1.setMinimumSize(new java.awt.Dimension(3, 0));
        jSplitPane1.setOneTouchExpandable(true);

        jPanel2.setBorder(javax.swing.BorderFactory.createTitledBorder("Variables"));

        variablesTable.setAutoResizeMode(javax.swing.JTable.AUTO_RESIZE_NEXT_COLUMN);
        variablesTable.setColumnSelectionAllowed(true);
        jScrollPane1.setViewportView(variablesTable);
        variablesTable.getColumnModel().getSelectionModel().setSelectionMode(javax.swing.ListSelectionModel.SINGLE_INTERVAL_SELECTION);

        javax.swing.GroupLayout jPanel2Layout = new javax.swing.GroupLayout(jPanel2);
        jPanel2.setLayout(jPanel2Layout);
        jPanel2Layout.setHorizontalGroup(
            jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jScrollPane1, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 524, Short.MAX_VALUE)
        );
        jPanel2Layout.setVerticalGroup(
            jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 253, Short.MAX_VALUE)
        );

        jSplitPane1.setTopComponent(jPanel2);

        jPanel3.setBorder(javax.swing.BorderFactory.createTitledBorder("Overwritten Variables"));

        jScrollPane2.setViewportView(overwrittenTable);

        clearOverwrittenButton.setText("Clear");
        clearOverwrittenButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                clearOverwrittenButtonActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout jPanel3Layout = new javax.swing.GroupLayout(jPanel3);
        jPanel3.setLayout(jPanel3Layout);
        jPanel3Layout.setHorizontalGroup(
            jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, jPanel3Layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(clearOverwrittenButton))
            .addComponent(jScrollPane2, javax.swing.GroupLayout.DEFAULT_SIZE, 524, Short.MAX_VALUE)
        );
        jPanel3Layout.setVerticalGroup(
            jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel3Layout.createSequentialGroup()
                .addComponent(clearOverwrittenButton)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jScrollPane2, javax.swing.GroupLayout.DEFAULT_SIZE, 127, Short.MAX_VALUE))
        );

        jSplitPane1.setBottomComponent(jPanel3);

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                    .addComponent(filterPanel, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(jSplitPane1, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, 536, Short.MAX_VALUE))
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jSplitPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 470, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(filterPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap())
        );
    }// </editor-fold>//GEN-END:initComponents

    private void columnSelectComboBoxActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_columnSelectComboBoxActionPerformed

        int index = columnSelectComboBox.getSelectedIndex();

        if (index >= 0) {
            if (index < 5) {
                filterColumnsVariables = new int[]{index};
                filterOverwritten = false;
            }
            if (index < 2) {
                filterColumnsOverwritten = new int[]{index};
                filterOverwritten = true;
            }

            if (index == 5) {    //"All" selected
                filterColumnsOverwritten = new int[]{};
                filterColumnsVariables = new int[]{};
                filterOverwritten = true;
            }

            //Apply the current filter text to the newly selected column
            variablesTable.filterTable(filterTextField.getText(), filterColumnsVariables);
            if (filterOverwritten) {
                overwrittenTable.filterTable(filterTextField.getText(), filterColumnsOverwritten);
            }
        }
        
    }//GEN-LAST:event_columnSelectComboBoxActionPerformed

    private void resetButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_resetButtonActionPerformed
        filterTextField.setText("");
    }//GEN-LAST:event_resetButtonActionPerformed

    private void clearOverwrittenButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_clearOverwrittenButtonActionPerformed
        TOverwritten[] over = new TOverwritten[0];
        if(parserRef != null) {
            parserRef.overwritten(over);
        }
    }//GEN-LAST:event_clearOverwrittenButtonActionPerformed


    // Variables declaration - do not modify//GEN-BEGIN:variables
    javax.swing.JButton clearOverwrittenButton;
    javax.swing.JComboBox columnSelectComboBox;
    javax.swing.JPanel filterPanel;
    javax.swing.JTextField filterTextField;
    javax.swing.JPanel jPanel2;
    javax.swing.JPanel jPanel3;
    javax.swing.JScrollPane jScrollPane1;
    javax.swing.JScrollPane jScrollPane2;
    javax.swing.JSplitPane jSplitPane1;
    edu.stanford.atom.sti.client.gui.table.STITable overwrittenTable;
    javax.swing.JButton resetButton;
    edu.stanford.atom.sti.client.gui.table.STITable variablesTable;
    // End of variables declaration//GEN-END:variables

}
