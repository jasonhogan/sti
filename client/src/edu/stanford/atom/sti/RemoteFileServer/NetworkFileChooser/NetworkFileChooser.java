/** @file NetworkFileChooser.java
 *  @author Jason Michael Hogan
 *  @brief Source-file for the class NetworkFileChooser
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

package edu.stanford.atom.sti.RemoteFileServer.NetworkFileChooser;

import javax.swing.*;
import javax.swing.RowFilter;
import edu.stanford.atom.sti.RemoteFileServer.comm.corba.*;
import edu.stanford.atom.sti.RemoteFileServer.comm.corba.TFile;
import java.util.Vector;
import javax.swing.table.*;
import javax.swing.event.*;
import javax.swing.tree.*;
import java.awt.event.KeyEvent;
import java.awt.Component;
import java.awt.event.*;

import java.sql.Time;
import java.util.Date;

import java.util.prefs.Preferences;

public class NetworkFileChooser extends javax.swing.JPanel {
    
    enum DialogType {SAVE_DIALOG, OPEN_DIALOG};
    public static final int CANCEL_OPTION = 1;
    public static final int APPROVE_OPTION = 0;
    public static final int ERROR_OPTION = -1;
    
    private int returnValue = CANCEL_OPTION;
    
    private JDialog dialog = null;
    
    private String fileName = null;   //the file selected
    private String selectedDirectory = null;
    
    private Vector<NetworkFileSystem> fileServers = new Vector<NetworkFileSystem>();
    int selectedServerIndex;

    long dateOffset = 1264320000000l;

    Icon fileIcon;
    Icon directoryIcon;
    Icon upFolderIcon = UIManager.getIcon("FileChooser.upFolderIcon");
    
    //sorting and filtering
    private DefaultTableModel fileTableModel;
    private TableRowSorter<DefaultTableModel> sorter;
    private String FileFilterString = "*";

    private static final String STIFILESERVERADDRESS = "";
    Preferences fileServerAddressPref = Preferences.userNodeForPackage(this.getClass());
    private String fileServerAddress = "localhost:2809";
    
    /** Creates new form NetworkFileChooser */
    public NetworkFileChooser() {
        initComponents();

        fileServerAddress = fileServerAddressPref.get(STIFILESERVERADDRESS, "localhost:2809");  //default value is only used if persistent prefs cannot be found

        selectionTextField.setText("");

        Icon test = UIManager.getIcon("Tree.closedIcon");

        directoryIcon = UIManager.getIcon("FileView.directoryIcon");
        fileIcon = UIManager.getIcon("FileView.fileIcon");
        UIManager.getIcon("FileView.computerIcon");
        UIManager.getIcon("FileView.hardDriveIcon");
        UIManager.getIcon("FileView.floppyDriveIcon");

        UIManager.getIcon("FileChooser.newFolderIcon");

        UIManager.getIcon("FileChooser.homeFolderIcon");
        UIManager.getIcon("FileChooser.detailsViewIcon");
        UIManager.getIcon("FileChooser.listViewIcon");

        //check that test icon is not null.


        fileTableModel = ((DefaultTableModel) FileTable.getModel());
        sorter = new TableRowSorter<DefaultTableModel>(fileTableModel);
        FileTable.setRowSorter(sorter);

        //      FileTable.setDefaultRenderer(TFile.class, new FileTableCellRenderer());

        FileTable.getColumnModel().getColumn(0).setCellRenderer(new FileTableCellRenderer());

        ListSelectionModel rowSM = FileTable.getSelectionModel();
        rowSM.addListSelectionListener(new ListSelectionListener() {
            public void valueChanged(ListSelectionEvent e) {
                int index = FileTable.getSelectedRow();
                if (index >= 0 && index < FileTable.getModel().getRowCount()) {
                    TFile tFile = (TFile) FileTable.getValueAt(index, 0);
                    if (tFile.isDirectory) {
                        selectionTextField.setText("");
                    } else {
                        selectionTextField.setText(tFile.filename);
                    }
                }


            }
        });

    //    addFileServer("128.12.175.32", "2809");
    //    addFileServer(new RemoteFileServer("192.168.37.1", "2809"));
    }
        
    public String getChoosenFullFilePath() {
        if(selectedDirectory != null && fileName != null && selectedServer() != null)
            return selectedServer().longFileName(selectedDirectory, fileName);
        else
            return null;
    }
    
    public NetworkFileSystem getChosenNetworkFileSystem() {
        return selectedServer();
    }
    public void setOKButtonText(String button) {
        selectButton.setText(button);
    }
    public int showSaveDialog(Component parent) {
	return showDialog(parent, DialogType.SAVE_DIALOG);
    }
    
    public int showOpenDialog(Component parent) {
	return showDialog(parent, DialogType.OPEN_DIALOG);
    }
    
    private int showDialog(Component parent, DialogType type) {

        //aquire a file server if none exist
        while(!serversFound()) {
            String newServer = (String)JOptionPane.showInputDialog(
                    this,
                    "No STI file servers were found.\n\n" +
                    "Please enter the IP address and port number " +
                    "of a valid STI file server in the form\n" +
                    "Address:Port",
                    "File Server Not Found",
                    JOptionPane.INFORMATION_MESSAGE,
                    null,
                    null, fileServerAddress);
            
            if(newServer == null) {
                returnValue = CANCEL_OPTION;
                return returnValue;
            }

            String[] serverParams =  newServer.split(":");
            if(serverParams.length == 2) {
                addFileServer(serverParams[0], serverParams[1]);
            }
        }
        
        switch(type) {
            case SAVE_DIALOG:
                dialog = createDialog(parent, "Save As");
                setOKButtonText("Save");
                break;
            case OPEN_DIALOG:
            default:
                dialog = createDialog(parent, "Open");
                setOKButtonText("Open");
                break;
        }
        
	dialog.addWindowListener(new WindowAdapter() {
            @Override
	    public void windowClosing(WindowEvent e) {
		returnValue = CANCEL_OPTION;
	    }
	});
        setDirectory(selectedDirectory);        //refresh contents
        dialog.setVisible(true);
        return returnValue;
    }
    protected JDialog createDialog(Component parent, String title) {

        dialog = new JDialog(JOptionPane.getFrameForComponent(parent), title, true);
        
        javax.swing.GroupLayout jDialog1Layout = new javax.swing.GroupLayout(dialog.getContentPane());
        dialog.getContentPane().setLayout(jDialog1Layout);
        jDialog1Layout.setHorizontalGroup(
            jDialog1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(this, javax.swing.GroupLayout.DEFAULT_SIZE, 582, Short.MAX_VALUE)
        );
        jDialog1Layout.setVerticalGroup(
            jDialog1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(this, javax.swing.GroupLayout.DEFAULT_SIZE, 433, Short.MAX_VALUE)
        );
        this.setSize(this.getPreferredSize());
        dialog.setMinimumSize(this.getMinimumSize());
        dialog.setSize(this.getSize());
        
        return dialog;
    }
    
    private void fileFilter() {
        RowFilter<DefaultTableModel, java.lang.Object> rf = null;
        //If current expression doesn't parse, don't update.
        try {
            rf = RowFilter.regexFilter(FileFilterString, 0);
        } catch (java.util.regex.PatternSyntaxException e) {
            return;
        }
        sorter.setRowFilter(rf);
    }
    
    public void addFileServer(String IPAddress, String port) {
        
        NetworkFileSystem fileServer = new NetworkFileSystem(IPAddress, port);
        
        if (fileServer.isAlive()) {
            if (isUnique(fileServer)) {
                fileServers.addElement(fileServer);
                //serverComboBox.addItem(new String(fileServer.getIP()));
                
                serverComboBox.addItem(new Object() { public String toString() { return "test"; } });
                
                fileServerAddressPref.put(STIFILESERVERADDRESS, IPAddress + ":" + port);
                
                //automatically select the newly added file server
                selectServer(fileServers.size() - 1);
                
            }
        }
    }
    
    private void selectServer(int serverIndex) {
        if (serverIndex >= 0 && serverIndex < fileServers.size()){
            selectedServerIndex = serverIndex;

            serverComboBox.setSelectedIndex(selectedServerIndex);
            setDirectory(selectedServer().homeDirectory());
            setTree(selectedServer().homeDirectory());
        }        
    }
    
    public void setTree(String dir) {

        DefaultMutableTreeNode root = new DefaultMutableTreeNode("c:");

        selectedServer().getRootDirectory();
    }
    
    public void setDirectory(String dir) {
        selectedDirectory = dir;
        
        selectionTextField.setText("");
        directoryComboBox.removeAllItems();
        directoryComboBox.addItem(dir);
        
        // Get network files from remote server
        TFile[] files = selectedServer().getFiles(dir);
        
        fileTableModel.setRowCount(0);

        int fileLength = 0;

        Date date = new Date(-1);


        // Add files to file table
        for(int i=0; i < files.length; i++) {
            
            fileLength = (files[i].length / 1000);
            


            date.setTime(files[i].lastModified * 1000);     //lastModified is in microseconds, setTime takes nanoseconds


            fileTableModel.addRow(new Object[] {
                selectedServer().shortFileName(files[i]),           //filename
                (files[i].isDirectory ? null : 
                    ((fileLength > 0) ? fileLength : 1) + " KB"),   //file size
                (files[i].isDirectory ? null : date.toString())     //file date
            });
        }
       fileFilter();
        
    }
    

    public boolean serversFound() {
        refreshRemoteServers();
        return !fileServers.isEmpty();
    }
    
    public void refreshRemoteServers() {

        boolean allAlive = true;
        
        do {
            allAlive = true;
            
            for(int i = 0; i < fileServers.size(); i++) {
                if( !fileServers.elementAt(i).isAlive() ) {
                    
                    fileServers.removeElementAt(i);
                    serverComboBox.removeItemAt(i);
                    
                    allAlive = false;
                    break;
                }
            }
        } while( !allAlive );
        
    }
    
    private boolean isUnique(NetworkFileSystem fileServer) {

        refreshRemoteServers();
        
        boolean found = false;
        
        for(int i = 0; i < fileServers.size(); i++) {
            found |= fileServers.elementAt(i).equals(fileServer);
        }
        
        return !found;
    }
    
    private NetworkFileSystem selectedServer() {
        if(selectedServerIndex >= 0 && selectedServerIndex < fileServers.size())
            return fileServers.elementAt(selectedServerIndex);
        else
            return null;
    }
    
    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        buttonGroup1 = new javax.swing.ButtonGroup();
        FileTreeSplitPane = new javax.swing.JSplitPane();
        FilePane = new javax.swing.JScrollPane();
        FileTable = new javax.swing.JTable();
        jPanel1 = new javax.swing.JPanel();
        SelectionPanel = new javax.swing.JPanel();
        cancelButton = new javax.swing.JButton();
        fileFilterComboBox = new javax.swing.JComboBox();
        selectButton = new javax.swing.JButton();
        selectionTextField = new javax.swing.JTextField();
        FileNameLabel = new javax.swing.JLabel();
        FileTypeLabel = new javax.swing.JLabel();
        NavigationPanel = new javax.swing.JPanel();
        directoryLabel = new javax.swing.JLabel();
        jSeparator1 = new javax.swing.JSeparator();
        directoryComboBox = new javax.swing.JComboBox();
        UpDirButton = new javax.swing.JButton();
        jToggleButton1 = new javax.swing.JToggleButton();
        jToggleButton2 = new javax.swing.JToggleButton();
        serverLabel = new javax.swing.JLabel();
        serverComboBox = new javax.swing.JComboBox();
        addFileServerButton = new javax.swing.JButton();

        setMinimumSize(new java.awt.Dimension(200, 200));
        setPreferredSize(new java.awt.Dimension(600, 450));

        FileTreeSplitPane.setDividerLocation(10);
        FileTreeSplitPane.setDividerSize(8);
        FileTreeSplitPane.setContinuousLayout(true);

        FilePane.setBackground(new java.awt.Color(255, 255, 255));

        FileTable.setAutoCreateRowSorter(true);
        FileTable.setModel(new javax.swing.table.DefaultTableModel(
            new Object [][] {
                {null, null, null},
                {null, null, null},
                {null, null, null},
                {null, null, null}
            },
            new String [] {
                "Name", "Size", "Date"
            }
        ) {
            boolean[] canEdit = new boolean [] {
                false, false, false
            };

            public boolean isCellEditable(int rowIndex, int columnIndex) {
                return canEdit [columnIndex];
            }
        });
        FileTable.setAutoscrolls(false);
        FileTable.setShowHorizontalLines(false);
        FileTable.setShowVerticalLines(false);
        FileTable.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                FileTableMouseClicked(evt);
            }
        });
        FileTable.addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyPressed(java.awt.event.KeyEvent evt) {
                FileTableKeyPressed(evt);
            }
        });
        FilePane.setViewportView(FileTable);
        FileTable.getColumnModel().getColumn(0).setPreferredWidth(125);
        FileTable.getColumnModel().getColumn(1).setPreferredWidth(25);
        FileTable.getColumnModel().getColumn(2).setPreferredWidth(50);

        FileTreeSplitPane.setRightComponent(FilePane);

        javax.swing.GroupLayout jPanel1Layout = new javax.swing.GroupLayout(jPanel1);
        jPanel1.setLayout(jPanel1Layout);
        jPanel1Layout.setHorizontalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 9, Short.MAX_VALUE)
        );
        jPanel1Layout.setVerticalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 266, Short.MAX_VALUE)
        );

        FileTreeSplitPane.setLeftComponent(jPanel1);

        cancelButton.setText("Cancel");
        cancelButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                cancelButtonActionPerformed(evt);
            }
        });

        fileFilterComboBox.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "Item 1", "Item 2", "Item 3", "Item 4" }));

        selectButton.setText("Open");
        selectButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                selectButtonActionPerformed(evt);
            }
        });

        selectionTextField.setText("jTextField1");

        FileNameLabel.setText("File name:");

        FileTypeLabel.setText("Files of type:");

        javax.swing.GroupLayout SelectionPanelLayout = new javax.swing.GroupLayout(SelectionPanel);
        SelectionPanel.setLayout(SelectionPanelLayout);
        SelectionPanelLayout.setHorizontalGroup(
            SelectionPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(SelectionPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(SelectionPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(FileNameLabel)
                    .addComponent(FileTypeLabel))
                .addGap(23, 23, 23)
                .addGroup(SelectionPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(selectionTextField, javax.swing.GroupLayout.DEFAULT_SIZE, 400, Short.MAX_VALUE)
                    .addComponent(fileFilterComboBox, 0, 400, Short.MAX_VALUE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(SelectionPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING, false)
                    .addComponent(selectButton, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(cancelButton, javax.swing.GroupLayout.DEFAULT_SIZE, 74, Short.MAX_VALUE)))
        );
        SelectionPanelLayout.setVerticalGroup(
            SelectionPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(SelectionPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(SelectionPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(FileNameLabel)
                    .addComponent(selectButton)
                    .addComponent(selectionTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(SelectionPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(FileTypeLabel)
                    .addComponent(cancelButton)
                    .addComponent(fileFilterComboBox, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        directoryLabel.setText("Directory:");

        directoryComboBox.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "Item 1", "Item 2", "Item 3", "Item 4" }));

        UpDirButton.setForeground(new java.awt.Color(236, 233, 216));
        UpDirButton.setIcon(upFolderIcon);
        UpDirButton.setToolTipText("Up");
        UpDirButton.setBorder(javax.swing.BorderFactory.createEmptyBorder(1, 1, 1, 1));
        UpDirButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                UpDirButtonActionPerformed(evt);
            }
        });

        buttonGroup1.add(jToggleButton1);
        jToggleButton1.setSelected(true);
        jToggleButton1.setText("jToggleButton1");

        buttonGroup1.add(jToggleButton2);
        jToggleButton2.setText("jToggleButton2");

        serverLabel.setText("File Server:  ");

        serverComboBox.setModel(new DefaultComboBoxModel());
        serverComboBox.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                serverComboBoxActionPerformed(evt);
            }
        });

        addFileServerButton.setText("Add...");
        addFileServerButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                addFileServerButtonActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout NavigationPanelLayout = new javax.swing.GroupLayout(NavigationPanel);
        NavigationPanel.setLayout(NavigationPanelLayout);
        NavigationPanelLayout.setHorizontalGroup(
            NavigationPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jSeparator1, javax.swing.GroupLayout.DEFAULT_SIZE, 576, Short.MAX_VALUE)
            .addGroup(NavigationPanelLayout.createSequentialGroup()
                .addGap(4, 4, 4)
                .addGroup(NavigationPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(directoryLabel)
                    .addComponent(serverLabel))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(NavigationPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(serverComboBox, 0, 338, Short.MAX_VALUE)
                    .addComponent(directoryComboBox, 0, 338, Short.MAX_VALUE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(NavigationPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(NavigationPanelLayout.createSequentialGroup()
                        .addComponent(UpDirButton, javax.swing.GroupLayout.PREFERRED_SIZE, 25, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addGap(18, 18, 18)
                        .addComponent(jToggleButton1, javax.swing.GroupLayout.PREFERRED_SIZE, 27, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                        .addComponent(jToggleButton2, javax.swing.GroupLayout.PREFERRED_SIZE, 27, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addComponent(addFileServerButton))
                .addGap(56, 56, 56))
        );
        NavigationPanelLayout.setVerticalGroup(
            NavigationPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(NavigationPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(NavigationPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(serverLabel)
                    .addComponent(serverComboBox, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(addFileServerButton))
                .addGap(13, 13, 13)
                .addGroup(NavigationPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                    .addGroup(NavigationPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                        .addComponent(directoryLabel)
                        .addComponent(directoryComboBox, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addComponent(jToggleButton1)
                        .addComponent(jToggleButton2))
                    .addComponent(UpDirButton, javax.swing.GroupLayout.PREFERRED_SIZE, 25, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jSeparator1, javax.swing.GroupLayout.DEFAULT_SIZE, 7, Short.MAX_VALUE))
        );

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(NavigationPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(FileTreeSplitPane, javax.swing.GroupLayout.DEFAULT_SIZE, 576, Short.MAX_VALUE)
                    .addComponent(SelectionPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addComponent(NavigationPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(FileTreeSplitPane, javax.swing.GroupLayout.DEFAULT_SIZE, 268, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(SelectionPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap())
        );
    }// </editor-fold>//GEN-END:initComponents

private void serverComboBoxActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_serverComboBoxActionPerformed
    selectServer(serverComboBox.getSelectedIndex());
}//GEN-LAST:event_serverComboBoxActionPerformed

private void FileTableMouseClicked(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_FileTableMouseClicked

    int selectedRow = FileTable.getSelectedRow();

    if (selectedRow >= 0 && selectedRow < FileTable.getModel().getRowCount()) {

        TFile tFile = (TFile) FileTable.getValueAt(selectedRow, 0);
        String selectedFile = tFile.filename;

        String selectedFileComplete = selectedDirectory +
                selectedServer().getSeparator() + selectedFile;

        if (evt.getClickCount() == 2) {
            //double clicked
            if(selectFile(selectedFileComplete))
                approveSelection();
        }

        if (evt.getClickCount() == 1) {
            // single click selection
            if (tFile.isDirectory) {
                selectionTextField.setText("");
            } else {
                selectionTextField.setText(selectedFile);
            }
        }
    }
}//GEN-LAST:event_FileTableMouseClicked
// Returns true if 'file' is actually a file
    private boolean selectFile(String file) {

        if (selectedServer() == null) {
            fileName = null;
            return false;
        } else {
            if (selectedServer().isDirectory(file)) {
                setDirectory(file);
                return false;
            } else {
                fileName = selectedServer().shortFileName(file);
                selectionTextField.setText(fileName);
                return true;
            }
        }
    }

    
    private String getSelectedFile() {

        int selectedRow = FileTable.getSelectedRow();
        String selectedFileComplete = "";

        if (selectedRow >= 0 && selectedRow < FileTable.getModel().getRowCount()) {

            String selectedFile = ((TFile) FileTable.getValueAt(selectedRow, 0)).filename;
            selectedFileComplete = selectedServer().longFileName(selectedDirectory, selectedFile);
        }

        return selectedFileComplete;
    }

    private void closeDialog() {
        if(dialog != null) {
           dialog.setVisible(false);
        }
    }
    private void approveSelection() {
        returnValue = APPROVE_OPTION;
        closeDialog();
    }
private void FileTableKeyPressed(java.awt.event.KeyEvent evt) {//GEN-FIRST:event_FileTableKeyPressed

    if(evt.getKeyCode() == KeyEvent.VK_ENTER) {
        // pressed enter
        if(selectFile(getSelectedFile()))
            approveSelection();
    }
}//GEN-LAST:event_FileTableKeyPressed

private void UpDirButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_UpDirButtonActionPerformed
    //move up one level of the directory tree
    if (selectedServer() != null) {
        TFile tFile = selectedServer().getParent(selectedDirectory);

        if (selectedServer().isDirectory(tFile.filename)) {
            setDirectory(tFile.filename);
        }
    }
}//GEN-LAST:event_UpDirButtonActionPerformed

private void selectButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_selectButtonActionPerformed
// TODO add your handling code here:
    if(selectedServer() != null) {
        if(selectedDirectory != null) {
            if(selectFile( selectionTextField.getText() )) {
                approveSelection();
            }
        }
    }

}//GEN-LAST:event_selectButtonActionPerformed

private void cancelButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_cancelButtonActionPerformed
    returnValue = CANCEL_OPTION;
    closeDialog();
}//GEN-LAST:event_cancelButtonActionPerformed

private void addFileServerButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_addFileServerButtonActionPerformed

    String newServer = (String)JOptionPane.showInputDialog(
                    this,
                    "Please enter the IP address and port number " +
                    "of a valid STI file server in the form\n" +
                    "Address:Port",
                    "Add New File Server",
                    JOptionPane.INFORMATION_MESSAGE,
                    null,
                    null, fileServerAddress);

    String[] serverParams =  newServer.split(":");

    if(serverParams.length == 2) {
        addFileServer(serverParams[0], serverParams[1]);
    }
}//GEN-LAST:event_addFileServerButtonActionPerformed




    // Variables declaration - do not modify//GEN-BEGIN:variables
    javax.swing.JLabel FileNameLabel;
    javax.swing.JScrollPane FilePane;
    javax.swing.JTable FileTable;
    javax.swing.JSplitPane FileTreeSplitPane;
    javax.swing.JLabel FileTypeLabel;
    javax.swing.JPanel NavigationPanel;
    javax.swing.JPanel SelectionPanel;
    javax.swing.JButton UpDirButton;
    javax.swing.JButton addFileServerButton;
    javax.swing.ButtonGroup buttonGroup1;
    javax.swing.JButton cancelButton;
    javax.swing.JComboBox directoryComboBox;
    javax.swing.JLabel directoryLabel;
    javax.swing.JComboBox fileFilterComboBox;
    javax.swing.JPanel jPanel1;
    javax.swing.JSeparator jSeparator1;
    javax.swing.JToggleButton jToggleButton1;
    javax.swing.JToggleButton jToggleButton2;
    javax.swing.JButton selectButton;
    javax.swing.JTextField selectionTextField;
    javax.swing.JComboBox serverComboBox;
    javax.swing.JLabel serverLabel;
    // End of variables declaration//GEN-END:variables

}
