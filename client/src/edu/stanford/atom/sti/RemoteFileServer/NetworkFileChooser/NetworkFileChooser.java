/*
 * NetworkFileChooser.java
 *
 * Created on September 18, 2008, 12:13 AM
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


/**
 *
 * @author  Jason
 */
public class NetworkFileChooser extends javax.swing.JPanel {

    private String chosenFile = "";   //the file selected for opening
    private Vector<NetworkFileSystem> fileServers = new Vector<NetworkFileSystem>();
    int selectedServerIndex;
    String selectedDirectory;
    Icon fileIcon;
    Icon directoryIcon;
    Icon upFolderIcon = UIManager.getIcon("FileChooser.upFolderIcon");
    
    //sorting and filtering
    private DefaultTableModel fileTableModel;
    private TableRowSorter<DefaultTableModel> sorter;
    private String FileFilterString = "*";
    
    public String getChosenFile() {
        return chosenFile;
    }
    
    /** Creates new form NetworkFileChooser */
    public NetworkFileChooser() {
        initComponents();
        
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


    
        fileTableModel = ((DefaultTableModel)FileTable.getModel());
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

        addFileServer("128.12.175.32", "2809");
    //    addFileServer(new RemoteFileServer("192.168.37.1", "2809"));
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
                serverComboBox.addItem(new String(fileServer.getIP()));
                
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

        // Add files to file table
        for(int i=0; i < files.length; i++) {
            
            fileLength = (files[i].length / 1000);
            
            fileTableModel.addRow(new Object[] {
                selectedServer().shortFileName(files[i]),           //filename
                (files[i].isDirectory ? null : 
                    ((fileLength > 0) ? fileLength : 1) + " KB"),   //file size
                files[i].lastModified                               //file date
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
        return fileServers.elementAt(selectedServerIndex);
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
        TreePane = new javax.swing.JScrollPane();
        DirectoryTree = new javax.swing.JTree();
        SelectionPanel = new javax.swing.JPanel();
        cancelButton = new javax.swing.JButton();
        fileFilterComboBox = new javax.swing.JComboBox();
        openButton = new javax.swing.JButton();
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

        FileTreeSplitPane.setDividerLocation(-10);
        FileTreeSplitPane.setDividerSize(8);
        FileTreeSplitPane.setContinuousLayout(true);
        FileTreeSplitPane.setLastDividerLocation(100);

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

        TreePane.setMinimumSize(new java.awt.Dimension(100, 23));
        TreePane.setPreferredSize(new java.awt.Dimension(100, 275));

        DirectoryTree.setEditable(true);
        DirectoryTree.setMaximumSize(new java.awt.Dimension(100, 64));
        TreePane.setViewportView(DirectoryTree);

        FileTreeSplitPane.setLeftComponent(TreePane);

        cancelButton.setText("Cancel");

        fileFilterComboBox.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "Item 1", "Item 2", "Item 3", "Item 4" }));

        openButton.setText("Open");
        openButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                openButtonActionPerformed(evt);
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
                    .addComponent(selectionTextField, javax.swing.GroupLayout.DEFAULT_SIZE, 386, Short.MAX_VALUE)
                    .addComponent(fileFilterComboBox, 0, 386, Short.MAX_VALUE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(SelectionPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING, false)
                    .addComponent(openButton, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(cancelButton, javax.swing.GroupLayout.DEFAULT_SIZE, 74, Short.MAX_VALUE)))
        );
        SelectionPanelLayout.setVerticalGroup(
            SelectionPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(SelectionPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(SelectionPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(FileNameLabel)
                    .addComponent(openButton)
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

        javax.swing.GroupLayout NavigationPanelLayout = new javax.swing.GroupLayout(NavigationPanel);
        NavigationPanel.setLayout(NavigationPanelLayout);
        NavigationPanelLayout.setHorizontalGroup(
            NavigationPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jSeparator1, javax.swing.GroupLayout.DEFAULT_SIZE, 562, Short.MAX_VALUE)
            .addGroup(NavigationPanelLayout.createSequentialGroup()
                .addGap(4, 4, 4)
                .addGroup(NavigationPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(directoryLabel)
                    .addComponent(serverLabel))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(NavigationPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(serverComboBox, 0, 358, Short.MAX_VALUE)
                    .addComponent(directoryComboBox, 0, 358, Short.MAX_VALUE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(UpDirButton, javax.swing.GroupLayout.PREFERRED_SIZE, 25, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(18, 18, 18)
                .addComponent(jToggleButton1, javax.swing.GroupLayout.PREFERRED_SIZE, 27, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(jToggleButton2, javax.swing.GroupLayout.PREFERRED_SIZE, 27, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(22, 22, 22))
        );
        NavigationPanelLayout.setVerticalGroup(
            NavigationPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(NavigationPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(NavigationPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                    .addGroup(NavigationPanelLayout.createSequentialGroup()
                        .addGroup(NavigationPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                            .addComponent(serverLabel)
                            .addComponent(serverComboBox, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                        .addGap(15, 15, 15)
                        .addGroup(NavigationPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                            .addComponent(directoryLabel)
                            .addComponent(directoryComboBox, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                            .addComponent(jToggleButton1)
                            .addComponent(jToggleButton2)))
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
                    .addComponent(FileTreeSplitPane)
                    .addComponent(SelectionPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addComponent(NavigationPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(FileTreeSplitPane, javax.swing.GroupLayout.DEFAULT_SIZE, 254, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(SelectionPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap())
        );
    }// </editor-fold>//GEN-END:initComponents

private void serverComboBoxActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_serverComboBoxActionPerformed
// TODO add your handling code here:
    serverComboBox.getSelectedIndex();

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
            openFile(selectedFileComplete);
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

    private void openFile(String file) {

        if (selectedServer().isDirectory(file)) {
            setDirectory(file);
        } else {
            //open the file on the server
            chosenFile = file;
        }
    }

    
    private String getSelectedFile() {

        int selectedRow = FileTable.getSelectedRow();
        String selectedFileComplete = "";

        if (selectedRow >= 0 && selectedRow < FileTable.getModel().getRowCount()) {

            String selectedFile = ((TFile) FileTable.getValueAt(selectedRow, 0)).filename;

            selectedFileComplete = selectedDirectory +
                    selectedServer().getSeparator() + selectedFile;
        }

        return selectedFileComplete;
    }

private void FileTableKeyPressed(java.awt.event.KeyEvent evt) {//GEN-FIRST:event_FileTableKeyPressed

    if(evt.getKeyCode() == KeyEvent.VK_ENTER) {
        // pressed enter
        openFile(getSelectedFile());
    }
}//GEN-LAST:event_FileTableKeyPressed

private void UpDirButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_UpDirButtonActionPerformed
    //move up one level of the directory tree
    TFile tFile = selectedServer().getParent(selectedDirectory);

    if(selectedServer().isDirectory(tFile.filename)) {
        setDirectory(tFile.filename);
    }
}//GEN-LAST:event_UpDirButtonActionPerformed

private void openButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_openButtonActionPerformed
// TODO add your handling code here:
    openFile(getSelectedFile());
}//GEN-LAST:event_openButtonActionPerformed




    // Variables declaration - do not modify//GEN-BEGIN:variables
    javax.swing.JTree DirectoryTree;
    javax.swing.JLabel FileNameLabel;
    javax.swing.JScrollPane FilePane;
    javax.swing.JTable FileTable;
    javax.swing.JSplitPane FileTreeSplitPane;
    javax.swing.JLabel FileTypeLabel;
    javax.swing.JPanel NavigationPanel;
    javax.swing.JPanel SelectionPanel;
    javax.swing.JScrollPane TreePane;
    javax.swing.JButton UpDirButton;
    javax.swing.ButtonGroup buttonGroup1;
    javax.swing.JButton cancelButton;
    javax.swing.JComboBox directoryComboBox;
    javax.swing.JLabel directoryLabel;
    javax.swing.JComboBox fileFilterComboBox;
    javax.swing.JSeparator jSeparator1;
    javax.swing.JToggleButton jToggleButton1;
    javax.swing.JToggleButton jToggleButton2;
    javax.swing.JButton openButton;
    javax.swing.JTextField selectionTextField;
    javax.swing.JComboBox serverComboBox;
    javax.swing.JLabel serverLabel;
    // End of variables declaration//GEN-END:variables

}
