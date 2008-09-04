/*
 * DeviceTab.java
 *
 * Created on August 15, 2008, 10:18 PM
 */

package edu.stanford.atom.sti.client.gui.DeviceManager;

import javax.swing.table.TableModel;
import javax.swing.table.DefaultTableModel;
//import javax.swing.table.TableColumn;
import javax.swing.event.TableModelEvent;
import javax.swing.event.TableModelListener;

import edu.stanford.atom.sti.client.comm.corba.DeviceConfigure;
import edu.stanford.atom.sti.device.comm.corba.TDevice;
import edu.stanford.atom.sti.client.comm.corba.TAttribute;

import edu.stanford.atom.sti.client.comm.corba.TChannel;
import edu.stanford.atom.sti.device.comm.corba.TChannelType;
import edu.stanford.atom.sti.device.comm.corba.TData;
import edu.stanford.atom.sti.device.comm.corba.TValue;
import edu.stanford.atom.sti.client.gui.table.STITableCellEditor;

/**
 *
 * @author  Jason
 */
public class DeviceTab extends javax.swing.JPanel {

    private DefaultTableModel AttributeTableModel;
    private DefaultTableModel ChannelTableModel;
    
    private STITableCellEditor stiTableCellEditor = new STITableCellEditor();
    private DeviceConfigure deviceConfigure;
    private TDevice tDevice;

    private TChannel[] channels;
    private TAttribute[] attributes;
    private boolean refreshingAttributeTable = false;
    
    private String tabTitle;
    private int tabIndex;
    
    private String status;
    private String deviceName;
    private String deviceAddress;
    private String deviceModule;
    
    
    /** Creates new form DeviceTab */
    public DeviceTab() {
        initComponents();
        setEnabledDeviceTab(false);
    }
    private void initTables() {
        
        AttributeTableModel = (DefaultTableModel)AttributeTable.getModel();
        ChannelTableModel = (DefaultTableModel)ChannelTable.getModel();
    
        AttributeTable.getColumnModel().getColumn(1).setCellEditor(stiTableCellEditor);

        refreshAttributes();
        refreshChannels();
        
        AttributeTableModel.addTableModelListener(new TableModelListener() {
            public void tableChanged(TableModelEvent evt) {
                errTextArea.append("Changed: ("+evt.getFirstRow()+", "+AttributeTable.convertRowIndexToModel(evt.getFirstRow())+") " +
                                
                                AttributeTable.getValueAt(AttributeTable.convertRowIndexToView(evt.getFirstRow()), 
                                AttributeTable.convertColumnIndexToView(1)).toString() + "\n");
                
                if(evt.getType() == TableModelEvent.UPDATE && !refreshingAttributeTable) {
                    try {
                        if(! deviceConfigure.setDeviceAttribute(tDevice.deviceID, 
                                attributes[evt.getFirstRow()].key, 
                                AttributeTable.getValueAt(AttributeTable.convertRowIndexToView(evt.getFirstRow()), 
                                AttributeTable.convertColumnIndexToView(1)).toString())) 
                        {
                            // failed to set device attribute
                            errTextArea.append(attributes[evt.getFirstRow()].key + " = " +
                                    AttributeTable.getValueAt(AttributeTable.convertRowIndexToView(evt.getFirstRow()), 
                                AttributeTable.convertColumnIndexToView(1)).toString()
                                + " failed:  Value not allowed.\n");
                        }
                    } catch(Exception e) {
                        errTextArea.append(e.toString() + " : Failed to setDeviceAttribute().\n");
                    }
                    refreshAttributes();
                }
            }
        });
    }
    public void registerDevice(TDevice device, DeviceConfigure deviceConfig) {
        tDevice = device;
        deviceConfigure = deviceConfig;
        setTabTitle(tDevice.deviceType);
        
        if(deviceStatus()) {
            initTables();
            setDeviceInfo();
        }
    }
    public boolean deviceStatus() {
        
        boolean device_status = false;
        try {
            device_status = deviceConfigure.deviceStatus(tDevice.deviceID);
        } catch(Exception e) {
            device_status = false;
            errTextArea.append(e.toString() + "\n");
        }
        status = device_status ? "Ready" : "Comm Error";
        statusLabel.setText("Status:  " + status);
        
        setEnabledDeviceTab(device_status);
        return device_status;
    }
    public void setEnabledDeviceTab(boolean enabled) {
        AttributeTable.setEnabled(enabled);
        ChannelTable.setEnabled(enabled);
        CommandLinePanel.setEnabled(enabled);
    }
    public void refreshAttributes() {
       
        refreshingAttributeTable = true;    // avoids an infinite loop with TableModelListener.tableChanged()
        
        if(deviceStatus()) {
            System.out.println("Refreshing Attributes: ");
            try {
                attributes = deviceConfigure.getDeviceAttributes(tDevice.deviceID);
                AttributeTableModel.setRowCount(attributes.length);
                System.out.println("...done refreshing attributes!");

                // populate Attribute table
                for(int i=0; i < attributes.length; i++) {
                    // set key
                    AttributeTable.setValueAt(attributes[i].key, 
                            AttributeTable.convertRowIndexToView(i), 
                            AttributeTable.convertColumnIndexToView(0) );

                    // setup JComboBox editor if allowed values are given
                    if(attributes[i].values.length > 0) {
                        stiTableCellEditor.installComboBoxEditor(i, 1, 
                                attributes[i].values);
                    }

                    // set value
                    AttributeTable.setValueAt(attributes[i].value, 
                            AttributeTable.convertRowIndexToView(i), 
                            AttributeTable.convertColumnIndexToView(1) );
                }
                
            } catch(Exception e) {
                errTextArea.append("refreshAttributes(): " + e.toString() + "\n" );
                e.printStackTrace();
            }
        }
        refreshingAttributeTable = false;
    }
    public void refreshChannels() {
        
        String channelType;
        String ioType;
        String InputType;
        String OutputType;
        
        if(deviceStatus()) {
            try {
                channels = deviceConfigure.getDeviceChannels(tDevice.deviceID);
                ChannelTableModel.setRowCount(channels.length);

                // populate Channel table
                for(int i=0; i < channels.length; i++) {
                    // set channel
                    ChannelTable.setValueAt(channels[i].channel, 
                            ChannelTable.convertRowIndexToModel(i), 
                            ChannelTable.convertColumnIndexToModel(0) );

                    switch(channels[i].inputType.value()) {
                        case TValue._ValueDDSTriplet:
                            InputType = "DDS";
                            break;
                        case TValue._ValueMeas:
                            InputType = "Null";
                            break;
                        case TValue._ValueNumber:
                            InputType = "Number";
                            break;
                        case TValue._ValueString:
                            InputType = "String";
                            break;
                        default:
                            InputType = "Unknown";
                            break;
                    }
                    switch(channels[i].outputType.value()) {
                        case TData._DataNone:
                            OutputType = "Null";
                            break;
                        case TData._DataNumber:
                            OutputType = "Number";
                            break;
                        case TData._DataPicture:
                            OutputType = "Picture";
                            break;
                        case TData._DataString:
                            OutputType = "String";
                            break;
                        default:
                            OutputType = "Unknown";
                            break;
                    }
                    // set channel type
                    switch(channels[i].type.value()) {
                        case TChannelType._Input:
                            ioType = "Input";
                            channelType = InputType;
                            break;
                        case TChannelType._Output:
                            ioType = "Output";
                            channelType = OutputType;
                            break;
                        case TChannelType._BiDirectional:
                            ioType = "Input/Output";
                            channelType = InputType + "/" + OutputType;
                            break;
                        default:
                            ioType = "Unknown";
                            channelType = InputType + "/" + OutputType;
                            break;
                    }
                    ChannelTable.setValueAt(ioType, 
                            ChannelTable.convertRowIndexToModel(i), 
                            ChannelTable.convertColumnIndexToModel(1) );
                    ChannelTable.setValueAt(channelType, 
                            ChannelTable.convertRowIndexToModel(i), 
                            ChannelTable.convertColumnIndexToModel(2) );
                }
            } catch(Exception e) {
                errTextArea.append("refreshChannels(): " + e.toString() + "\n" );
                e.printStackTrace();
            }
        }
    }
    public void setDeviceInfo() {
        deviceName = tDevice.deviceType;
        deviceAddress = tDevice.address;
        deviceModule = "" + tDevice.moduleNum;
       
        deviceLabel.setText("Device:   " + tDevice.deviceType);
        addressLabel.setText("Address: " + tDevice.address);
        moduleLabel.setText("Module:   " + tDevice.moduleNum);
    }
    
    public String getDeviceID() {
        return tDevice.deviceID;
    }
    public TDevice getTDevice() {
        return tDevice;
    }
    public String getTabTitle() {
        return tabTitle;
    } 
    public void setTabTitle(String title) {
        tabTitle = title;
    }
    
    public int getTabIndex() {
        return tabIndex;
    } 
    public void setTabIndex(int index) {
        tabIndex = index;
    }
    
    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jDialog1 = new javax.swing.JDialog();
        jPanel1 = new javax.swing.JPanel();
        jButton1 = new javax.swing.JButton();
        jScrollPane1 = new javax.swing.JScrollPane();
        jTable1 = new javax.swing.JTable();
        jLabel1 = new javax.swing.JLabel();
        deviceInfoPanel = new javax.swing.JPanel();
        deviceLabel = new javax.swing.JLabel();
        addressLabel = new javax.swing.JLabel();
        moduleLabel = new javax.swing.JLabel();
        ControlPanel = new javax.swing.JPanel();
        refreshButton = new javax.swing.JButton();
        removeButton = new javax.swing.JButton();
        statusLabel = new javax.swing.JLabel();
        tablePanel = new javax.swing.JPanel();
        attributeLabel = new javax.swing.JLabel();
        channelLabel = new javax.swing.JLabel();
        attribScrollPanel = new javax.swing.JScrollPane();
        AttributeTable = new javax.swing.JTable();
        chScrollPane = new javax.swing.JScrollPane();
        ChannelTable = new javax.swing.JTable();
        jSeparator1 = new javax.swing.JSeparator();
        CommandLinePanel = new javax.swing.JPanel();
        cmdScrollPane = new javax.swing.JScrollPane();
        cmdTextArea = new javax.swing.JTextArea();
        CommandLineName = new javax.swing.JLabel();
        jTextField1 = new javax.swing.JTextField();
        partnerButton = new javax.swing.JButton();
        errScrollPane = new javax.swing.JScrollPane();
        errTextArea = new javax.swing.JTextArea();

        jButton1.setText("Close");
        jButton1.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton1ActionPerformed(evt);
            }
        });

        jTable1.setModel(new javax.swing.table.DefaultTableModel(
            new Object [][] {

            },
            new String [] {
                "Device Name", "Status"
            }
        ) {
            boolean[] canEdit = new boolean [] {
                false, false
            };

            public boolean isCellEditable(int rowIndex, int columnIndex) {
                return canEdit [columnIndex];
            }
        });
        jScrollPane1.setViewportView(jTable1);

        jLabel1.setText("The following partner devices are required by this device");

        javax.swing.GroupLayout jPanel1Layout = new javax.swing.GroupLayout(jPanel1);
        jPanel1.setLayout(jPanel1Layout);
        jPanel1Layout.setHorizontalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel1Layout.createSequentialGroup()
                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(jPanel1Layout.createSequentialGroup()
                        .addGap(155, 155, 155)
                        .addComponent(jButton1, javax.swing.GroupLayout.PREFERRED_SIZE, 88, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addGroup(jPanel1Layout.createSequentialGroup()
                        .addContainerGap()
                        .addComponent(jLabel1))
                    .addGroup(jPanel1Layout.createSequentialGroup()
                        .addContainerGap()
                        .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 366, Short.MAX_VALUE)))
                .addContainerGap())
        );
        jPanel1Layout.setVerticalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, jPanel1Layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jLabel1)
                .addGap(18, 18, 18)
                .addComponent(jScrollPane1, javax.swing.GroupLayout.PREFERRED_SIZE, 120, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addComponent(jButton1)
                .addContainerGap())
        );

        javax.swing.GroupLayout jDialog1Layout = new javax.swing.GroupLayout(jDialog1.getContentPane());
        jDialog1.getContentPane().setLayout(jDialog1Layout);
        jDialog1Layout.setHorizontalGroup(
            jDialog1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jPanel1, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
        );
        jDialog1Layout.setVerticalGroup(
            jDialog1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jDialog1Layout.createSequentialGroup()
                .addComponent(jPanel1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        setMinimumSize(new java.awt.Dimension(100, 600));

        deviceInfoPanel.setBorder(javax.swing.BorderFactory.createEtchedBorder());

        deviceLabel.setText("Device:   ");

        addressLabel.setText("Address: ");

        moduleLabel.setText("Module:   ");

        javax.swing.GroupLayout deviceInfoPanelLayout = new javax.swing.GroupLayout(deviceInfoPanel);
        deviceInfoPanel.setLayout(deviceInfoPanelLayout);
        deviceInfoPanelLayout.setHorizontalGroup(
            deviceInfoPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(deviceInfoPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(deviceInfoPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(deviceLabel)
                    .addComponent(addressLabel)
                    .addComponent(moduleLabel))
                .addContainerGap(195, Short.MAX_VALUE))
        );
        deviceInfoPanelLayout.setVerticalGroup(
            deviceInfoPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(deviceInfoPanelLayout.createSequentialGroup()
                .addGap(11, 11, 11)
                .addComponent(deviceLabel)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addComponent(addressLabel)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(moduleLabel)
                .addGap(16, 16, 16))
        );

        ControlPanel.setBorder(javax.swing.BorderFactory.createEtchedBorder());

        refreshButton.setText("Refresh");
        refreshButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                refreshButtonActionPerformed(evt);
            }
        });

        removeButton.setText("Remove");

        statusLabel.setText("Status:  ");

        javax.swing.GroupLayout ControlPanelLayout = new javax.swing.GroupLayout(ControlPanel);
        ControlPanel.setLayout(ControlPanelLayout);
        ControlPanelLayout.setHorizontalGroup(
            ControlPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(ControlPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(ControlPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(ControlPanelLayout.createSequentialGroup()
                        .addComponent(refreshButton, javax.swing.GroupLayout.PREFERRED_SIZE, 99, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(removeButton, javax.swing.GroupLayout.PREFERRED_SIZE, 106, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addComponent(statusLabel))
                .addContainerGap(59, Short.MAX_VALUE))
        );
        ControlPanelLayout.setVerticalGroup(
            ControlPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(ControlPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(statusLabel)
                .addGap(18, 18, 18)
                .addGroup(ControlPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(removeButton)
                    .addComponent(refreshButton))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        attributeLabel.setText("Attributes");

        channelLabel.setText("Channels");

        AttributeTable.setAutoCreateRowSorter(true);
        AttributeTable.setModel(new javax.swing.table.DefaultTableModel(
            new Object [][] {

            },
            new String [] {
                "Key", "Value"
            }
        ) {
            Class[] types = new Class [] {
                java.lang.String.class, java.lang.String.class
            };
            boolean[] canEdit = new boolean [] {
                false, true
            };

            public Class getColumnClass(int columnIndex) {
                return types [columnIndex];
            }

            public boolean isCellEditable(int rowIndex, int columnIndex) {
                return canEdit [columnIndex];
            }
        });
        attribScrollPanel.setViewportView(AttributeTable);

        chScrollPane.setAutoscrolls(true);

        ChannelTable.setAutoCreateRowSorter(true);
        ChannelTable.setModel(new javax.swing.table.DefaultTableModel(
            new Object [][] {

            },
            new String [] {
                "Channel", "I/O", "Type"
            }
        ) {
            boolean[] canEdit = new boolean [] {
                false, false, true
            };

            public boolean isCellEditable(int rowIndex, int columnIndex) {
                return canEdit [columnIndex];
            }
        });
        ChannelTable.setMaximumSize(new java.awt.Dimension(200, 200));
        ChannelTable.setMinimumSize(new java.awt.Dimension(45, 10));
        chScrollPane.setViewportView(ChannelTable);

        jSeparator1.setOrientation(javax.swing.SwingConstants.VERTICAL);

        CommandLinePanel.setBorder(javax.swing.BorderFactory.createTitledBorder(null, "Command Line", javax.swing.border.TitledBorder.DEFAULT_JUSTIFICATION, javax.swing.border.TitledBorder.DEFAULT_POSITION, new java.awt.Font("Tahoma", 0, 11), new java.awt.Color(0, 0, 0)));

        cmdScrollPane.setBorder(null);

        cmdTextArea.setColumns(20);
        cmdTextArea.setEditable(false);
        cmdTextArea.setRows(5);
        cmdTextArea.setBorder(null);
        cmdTextArea.setMaximumSize(new java.awt.Dimension(80, 50));
        cmdTextArea.setMinimumSize(new java.awt.Dimension(80, 18));
        cmdScrollPane.setViewportView(cmdTextArea);

        CommandLineName.setText("Name: ");

        jTextField1.setText("jTextField1");
        jTextField1.setBorder(null);
        jTextField1.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jTextField1ActionPerformed(evt);
            }
        });

        partnerButton.setText("Partners...");
        partnerButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                partnerButtonActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout CommandLinePanelLayout = new javax.swing.GroupLayout(CommandLinePanel);
        CommandLinePanel.setLayout(CommandLinePanelLayout);
        CommandLinePanelLayout.setHorizontalGroup(
            CommandLinePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(CommandLinePanelLayout.createSequentialGroup()
                .addGroup(CommandLinePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(CommandLinePanelLayout.createSequentialGroup()
                        .addGap(10, 10, 10)
                        .addComponent(CommandLineName)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 351, Short.MAX_VALUE)
                        .addComponent(partnerButton))
                    .addGroup(CommandLinePanelLayout.createSequentialGroup()
                        .addContainerGap()
                        .addComponent(cmdScrollPane, javax.swing.GroupLayout.DEFAULT_SIZE, 470, Short.MAX_VALUE))
                    .addGroup(CommandLinePanelLayout.createSequentialGroup()
                        .addContainerGap()
                        .addComponent(jTextField1, javax.swing.GroupLayout.DEFAULT_SIZE, 470, Short.MAX_VALUE)))
                .addGap(42, 42, 42))
        );
        CommandLinePanelLayout.setVerticalGroup(
            CommandLinePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(CommandLinePanelLayout.createSequentialGroup()
                .addGroup(CommandLinePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(partnerButton)
                    .addComponent(CommandLineName))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(cmdScrollPane, javax.swing.GroupLayout.PREFERRED_SIZE, 96, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(0, 0, 0)
                .addComponent(jTextField1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(28, Short.MAX_VALUE))
        );

        javax.swing.GroupLayout tablePanelLayout = new javax.swing.GroupLayout(tablePanel);
        tablePanel.setLayout(tablePanelLayout);
        tablePanelLayout.setHorizontalGroup(
            tablePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(tablePanelLayout.createSequentialGroup()
                .addGroup(tablePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(tablePanelLayout.createSequentialGroup()
                        .addComponent(attribScrollPanel, javax.swing.GroupLayout.PREFERRED_SIZE, 247, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addGap(18, 18, 18)
                        .addComponent(jSeparator1, javax.swing.GroupLayout.PREFERRED_SIZE, 14, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addComponent(attributeLabel))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(tablePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(channelLabel)
                    .addComponent(chScrollPane, javax.swing.GroupLayout.DEFAULT_SIZE, 273, Short.MAX_VALUE)))
            .addGroup(tablePanelLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(CommandLinePanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addContainerGap())
        );
        tablePanelLayout.setVerticalGroup(
            tablePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(tablePanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(tablePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(channelLabel)
                    .addComponent(attributeLabel))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(tablePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(chScrollPane, javax.swing.GroupLayout.DEFAULT_SIZE, 191, Short.MAX_VALUE)
                    .addComponent(jSeparator1, javax.swing.GroupLayout.DEFAULT_SIZE, 191, Short.MAX_VALUE)
                    .addComponent(attribScrollPanel, 0, 0, Short.MAX_VALUE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(CommandLinePanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap())
        );

        errTextArea.setColumns(20);
        errTextArea.setRows(5);
        errScrollPane.setViewportView(errTextArea);

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(errScrollPane, javax.swing.GroupLayout.DEFAULT_SIZE, 558, Short.MAX_VALUE)
                    .addGroup(layout.createSequentialGroup()
                        .addComponent(deviceInfoPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addGap(18, 18, 18)
                        .addComponent(ControlPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                    .addComponent(tablePanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                    .addComponent(deviceInfoPanel, 0, 81, Short.MAX_VALUE)
                    .addComponent(ControlPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(errScrollPane, javax.swing.GroupLayout.PREFERRED_SIZE, 88, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(tablePanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(53, 53, 53))
        );
    }// </editor-fold>//GEN-END:initComponents

    private void refreshButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_refreshButtonActionPerformed
        // TODO add your handling code here:
        refreshAttributes();
        refreshChannels();
    }//GEN-LAST:event_refreshButtonActionPerformed

    private void jButton1ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton1ActionPerformed
        // TODO add your handling code here:
        jDialog1.setVisible(false);
    }//GEN-LAST:event_jButton1ActionPerformed

    private void partnerButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_partnerButtonActionPerformed
        // TODO add your handling code here:
        jDialog1.setVisible(true);
    }//GEN-LAST:event_partnerButtonActionPerformed

    private void jTextField1ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jTextField1ActionPerformed
        // TODO add your handling code here:
    }//GEN-LAST:event_jTextField1ActionPerformed
    
    
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JTable AttributeTable;
    private javax.swing.JTable ChannelTable;
    private javax.swing.JLabel CommandLineName;
    private javax.swing.JPanel CommandLinePanel;
    private javax.swing.JPanel ControlPanel;
    private javax.swing.JLabel addressLabel;
    private javax.swing.JScrollPane attribScrollPanel;
    private javax.swing.JLabel attributeLabel;
    private javax.swing.JScrollPane chScrollPane;
    private javax.swing.JLabel channelLabel;
    private javax.swing.JScrollPane cmdScrollPane;
    private javax.swing.JTextArea cmdTextArea;
    private javax.swing.JPanel deviceInfoPanel;
    private javax.swing.JLabel deviceLabel;
    private javax.swing.JScrollPane errScrollPane;
    private javax.swing.JTextArea errTextArea;
    private javax.swing.JButton jButton1;
    private javax.swing.JDialog jDialog1;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JSeparator jSeparator1;
    private javax.swing.JTable jTable1;
    private javax.swing.JTextField jTextField1;
    private javax.swing.JLabel moduleLabel;
    private javax.swing.JButton partnerButton;
    private javax.swing.JButton refreshButton;
    private javax.swing.JButton removeButton;
    private javax.swing.JLabel statusLabel;
    private javax.swing.JPanel tablePanel;
    // End of variables declaration//GEN-END:variables
    
}
