/** @file DeviceTab.java
 *  @author Jason Michael Hogan
 *  @brief Source-file for the class DeviceTab
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

package edu.stanford.atom.sti.client.gui.DevicesTab;

import javax.swing.table.DefaultTableModel;
import javax.swing.event.TableModelEvent;
import javax.swing.event.TableModelListener;
import edu.stanford.atom.sti.corba.Client_Server.ServerCommandLine;
import edu.stanford.atom.sti.corba.Client_Server.DeviceConfigure;
import edu.stanford.atom.sti.corba.Types.TDevice;
import edu.stanford.atom.sti.corba.Types.TAttribute;
import edu.stanford.atom.sti.corba.Types.TChannel;
//import edu.stanford.atom.sti.corba.Types.TChannelType;
//import edu.stanford.atom.sti.corba.Types.TData;
//import edu.stanford.atom.sti.corba.Types.TValue;
import edu.stanford.atom.sti.client.gui.table.STITableCellEditor;
import edu.stanford.atom.sti.client.comm.bl.DeviceManager;
import java.lang.Thread;
import java.util.Vector;
import edu.stanford.atom.sti.client.comm.bl.TChannelDecode;

public class DeviceTab extends javax.swing.JPanel {

    private String statusText = "Status:  ";
    private String pingText = "Ping:  ";

    private DefaultTableModel AttributeTableModel;
    private DefaultTableModel ChannelTableModel;
    
    private STITableCellEditor stiTableCellEditor = new STITableCellEditor();
    private DeviceConfigure deviceConfigure;
    private ServerCommandLine commandLineRef;
    private DeviceManager deviceManager = null;

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

    private Vector<DeviceTabListener> deviceTabListeners = new Vector<DeviceTabListener>();
    
    private java.lang.Thread refreshAttributesThread = null;
    private java.lang.Thread refreshChannelsThread = null;

    public DeviceTab() {
        initComponents();
        setEnabledDeviceTab(false);
    }
    
    public void setDeviceManager(DeviceManager deviceManager) {
        this.deviceManager = deviceManager;
    }

    public void setCommandLine(ServerCommandLine commandLine) {
        commandLineRef = commandLine;
    }
    public void setDeviceConfigure(DeviceConfigure deviceConfigure) {
        this.deviceConfigure = deviceConfigure;
    }
   
    private void initTables() {
        
        AttributeTableModel = (DefaultTableModel)AttributeTable.getModel();
        ChannelTableModel = (DefaultTableModel)ChannelTable.getModel();
    
        AttributeTable.getColumnModel().getColumn(1).setCellEditor(stiTableCellEditor);

        //Must finish refreshing the attributes before adding TableModelListener
        refreshAttributesThread();
        refreshChannelsThread();
        
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
    public void registerDevice(TDevice device, DeviceConfigure deviceConfig, ServerCommandLine commandLine) {
        tDevice = device;
        deviceConfigure = deviceConfig;
        commandLineRef = commandLine;
//        setTabTitle(tDevice.deviceName);
        
        if (deviceStatus()) {

            Thread initThread = new Thread(new Runnable() {

                public void run() {
                    initTables();
                    setDeviceInfo();
                }
            });

            initThread.start();
        }
    }
    public boolean deviceStatus() {
        
        boolean device_status = false;
        long ping = -1;

        try {
            device_status = deviceConfigure.deviceStatus(tDevice.deviceID);
            ping = deviceConfigure.devicePing(tDevice.deviceID);
            
        } catch(Exception e) {
            device_status = false;
            errTextArea.append(e.toString() + "\n");
        }
        status = device_status ? "Ready" : "Comm Error";
        statusLabel.setText(statusText + status);
        pingLabel.setText(pingText + 
                ((ping == 0) ? "< 1" : ping ) + " ms" );
        
        setEnabledDeviceTab(device_status);
        return device_status;
    }
    public void setEnabledDeviceTab(boolean enabled) {
        AttributeTable.setEnabled(enabled);
        ChannelTable.setEnabled(enabled);
    }

    public void refreshAttributes() {

        refreshAttributesThread = new Thread(new Runnable() {
       
            public void run() {
                refreshAttributesThread();
            }
        });

        refreshAttributesThread.start();
    }
    public void refreshChannels() {

        refreshChannelsThread = new Thread(new Runnable() {

            public void run() {
                refreshChannelsThread();
            }
        });

        refreshChannelsThread.start();
    }

    private void refreshAttributesThread() {

        refreshingAttributeTable = true;    // avoids an infinite loop with TableModelListener.tableChanged()
        
        if(deviceStatus()) {
            try {
                attributes = deviceConfigure.getDeviceAttributes(tDevice.deviceID);
                AttributeTableModel.setRowCount(attributes.length);
                
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
    private void refreshChannelsThread() {
        
        String channelType;
        String ioType;
        
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

                    TChannelDecode channelDecode = new TChannelDecode(channels[i]);

                    ioType = channelDecode.IOType();
                    channelType = channelDecode.ChannelType();

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
        deviceName = tDevice.deviceName;
        deviceAddress = tDevice.address;
        deviceModule = "" + tDevice.moduleNum;
       
        deviceLabel.setText("Device:   " + tDevice.deviceName);
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
        fireNewTableTitleChangedEvent(tabIndex, title);
    }
    
    private synchronized void fireNewTableTitleChangedEvent(int index, String title) {
        for(int i = 0; i < deviceTabListeners.size(); i++) {
            deviceTabListeners.elementAt(i).tabTitleChanged(index, title);
        }
    }

    public synchronized void addDeviceTabListener(DeviceTabListener listener) {

        //First get rid of any old instances of this listener
        while(deviceTabListeners.removeElement(listener)) {}

        deviceTabListeners.addElement(listener);
    }

    public synchronized void removeDeviceTabListener(DeviceTabListener listener) {
        deviceTabListeners.removeElement(listener);
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
        jScrollPane2 = new javax.swing.JScrollPane();
        jSplitPane6 = new javax.swing.JSplitPane();
        jSplitPane5 = new javax.swing.JSplitPane();
        jSplitPane3 = new javax.swing.JSplitPane();
        commandLineTextField = new javax.swing.JTextField();
        jLabel2 = new javax.swing.JLabel();
        jTabbedPane1 = new javax.swing.JTabbedPane();
        cmdScrollPane = new javax.swing.JScrollPane();
        cmdTextArea = new javax.swing.JTextArea();
        errScrollPane = new javax.swing.JScrollPane();
        errTextArea = new javax.swing.JTextArea();
        jSplitPane4 = new javax.swing.JSplitPane();
        jSplitPane1 = new javax.swing.JSplitPane();
        deviceInfoPanel = new javax.swing.JPanel();
        deviceLabel = new javax.swing.JLabel();
        addressLabel = new javax.swing.JLabel();
        moduleLabel = new javax.swing.JLabel();
        ControlPanel = new javax.swing.JPanel();
        refreshButton = new javax.swing.JButton();
        statusLabel = new javax.swing.JLabel();
        partnerButton = new javax.swing.JButton();
        killButton = new javax.swing.JButton();
        pingLabel = new javax.swing.JLabel();
        jSplitPane2 = new javax.swing.JSplitPane();
        jPanel2 = new javax.swing.JPanel();
        attribScrollPanel = new javax.swing.JScrollPane();
        AttributeTable = new javax.swing.JTable();
        jPanel3 = new javax.swing.JPanel();
        chScrollPane = new javax.swing.JScrollPane();
        ChannelTable = new javax.swing.JTable();

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

        setMinimumSize(new java.awt.Dimension(600, 200));
        setPreferredSize(new java.awt.Dimension(600, 600));

        jSplitPane6.setDividerLocation(300);
        jSplitPane6.setDividerSize(8);
        jSplitPane6.setOrientation(javax.swing.JSplitPane.VERTICAL_SPLIT);
        jSplitPane6.setResizeWeight(0.5);
        jSplitPane6.setOneTouchExpandable(true);

        jSplitPane5.setDividerLocation(260);
        jSplitPane5.setDividerSize(0);
        jSplitPane5.setOrientation(javax.swing.JSplitPane.VERTICAL_SPLIT);
        jSplitPane5.setResizeWeight(1.0);
        jSplitPane5.setMaximumSize(new java.awt.Dimension(2147483647, 100));
        jSplitPane5.setMinimumSize(new java.awt.Dimension(600, 100));
        jSplitPane5.setPreferredSize(new java.awt.Dimension(300, 100));

        jSplitPane3.setBorder(javax.swing.BorderFactory.createEmptyBorder(1, 1, 1, 1));
        jSplitPane3.setDividerLocation(20);
        jSplitPane3.setDividerSize(0);
        jSplitPane3.setMaximumSize(new java.awt.Dimension(2147483647, 24));

        commandLineTextField.setFont(commandLineTextField.getFont().deriveFont(commandLineTextField.getFont().getSize()+3f));
        commandLineTextField.setBorder(null);
        commandLineTextField.setMaximumSize(new java.awt.Dimension(2147483647, 17));
        commandLineTextField.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                commandLineTextFieldActionPerformed(evt);
            }
        });
        jSplitPane3.setRightComponent(commandLineTextField);

        jLabel2.setFont(jLabel2.getFont().deriveFont(jLabel2.getFont().getStyle() | java.awt.Font.BOLD, jLabel2.getFont().getSize()+7));
        jLabel2.setText(">");
        jSplitPane3.setLeftComponent(jLabel2);

        jSplitPane5.setRightComponent(jSplitPane3);

        cmdScrollPane.setBorder(null);

        cmdTextArea.setColumns(20);
        cmdTextArea.setEditable(false);
        cmdTextArea.setFont(cmdTextArea.getFont().deriveFont(cmdTextArea.getFont().getSize()+3f));
        cmdTextArea.setRows(5);
        cmdTextArea.setBorder(null);
        cmdTextArea.setMaximumSize(new java.awt.Dimension(80, 30));
        cmdTextArea.setMinimumSize(new java.awt.Dimension(80, 18));
        cmdScrollPane.setViewportView(cmdTextArea);

        jTabbedPane1.addTab("Command Line", cmdScrollPane);

        errTextArea.setColumns(20);
        errTextArea.setRows(5);
        errScrollPane.setViewportView(errTextArea);

        jTabbedPane1.addTab("Error Stream", errScrollPane);

        jSplitPane5.setLeftComponent(jTabbedPane1);

        jSplitPane6.setBottomComponent(jSplitPane5);

        jSplitPane4.setDividerLocation(90);
        jSplitPane4.setDividerSize(4);
        jSplitPane4.setOrientation(javax.swing.JSplitPane.VERTICAL_SPLIT);
        jSplitPane4.setMaximumSize(new java.awt.Dimension(2147483647, 200));
        jSplitPane4.setPreferredSize(new java.awt.Dimension(602, 100));

        jSplitPane1.setDividerLocation(290);
        jSplitPane1.setDividerSize(0);
        jSplitPane1.setResizeWeight(0.5);
        jSplitPane1.setMinimumSize(new java.awt.Dimension(600, 3));
        jSplitPane1.setPreferredSize(new java.awt.Dimension(600, 150));

        deviceInfoPanel.setBorder(javax.swing.BorderFactory.createEtchedBorder());
        deviceInfoPanel.setMinimumSize(new java.awt.Dimension(300, 0));

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
                .addContainerGap(252, Short.MAX_VALUE))
        );
        deviceInfoPanelLayout.setVerticalGroup(
            deviceInfoPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(deviceInfoPanelLayout.createSequentialGroup()
                .addGap(11, 11, 11)
                .addComponent(deviceLabel)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 8, Short.MAX_VALUE)
                .addComponent(addressLabel)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(moduleLabel)
                .addGap(16, 16, 16))
        );

        jSplitPane1.setLeftComponent(deviceInfoPanel);

        ControlPanel.setBorder(javax.swing.BorderFactory.createEtchedBorder());
        ControlPanel.setMinimumSize(new java.awt.Dimension(300, 0));

        refreshButton.setText("Refresh");
        refreshButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                refreshButtonActionPerformed(evt);
            }
        });

        statusLabel.setText("Status:  ");

        partnerButton.setText("Partners...");
        partnerButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                partnerButtonActionPerformed(evt);
            }
        });

        killButton.setText("Kill");
        killButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                killButtonActionPerformed(evt);
            }
        });

        pingLabel.setText("Ping:   ");

        javax.swing.GroupLayout ControlPanelLayout = new javax.swing.GroupLayout(ControlPanel);
        ControlPanel.setLayout(ControlPanelLayout);
        ControlPanelLayout.setHorizontalGroup(
            ControlPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(ControlPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(ControlPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(refreshButton, javax.swing.GroupLayout.PREFERRED_SIZE, 99, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(statusLabel))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(ControlPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(ControlPanelLayout.createSequentialGroup()
                        .addComponent(partnerButton, javax.swing.GroupLayout.PREFERRED_SIZE, 103, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(killButton, javax.swing.GroupLayout.PREFERRED_SIZE, 77, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addComponent(pingLabel))
                .addContainerGap(25, Short.MAX_VALUE))
        );
        ControlPanelLayout.setVerticalGroup(
            ControlPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(ControlPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(ControlPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(statusLabel)
                    .addComponent(pingLabel))
                .addGap(15, 15, 15)
                .addGroup(ControlPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(refreshButton)
                    .addComponent(partnerButton)
                    .addComponent(killButton))
                .addContainerGap(20, Short.MAX_VALUE))
        );

        jSplitPane1.setRightComponent(ControlPanel);

        jSplitPane4.setLeftComponent(jSplitPane1);

        jSplitPane2.setDividerLocation(350);
        jSplitPane2.setResizeWeight(0.5);

        jPanel2.setBorder(javax.swing.BorderFactory.createTitledBorder("Attributes"));

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

        javax.swing.GroupLayout jPanel2Layout = new javax.swing.GroupLayout(jPanel2);
        jPanel2.setLayout(jPanel2Layout);
        jPanel2Layout.setHorizontalGroup(
            jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(attribScrollPanel, javax.swing.GroupLayout.DEFAULT_SIZE, 351, Short.MAX_VALUE)
        );
        jPanel2Layout.setVerticalGroup(
            jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(attribScrollPanel, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 176, Short.MAX_VALUE)
        );

        jSplitPane2.setLeftComponent(jPanel2);

        jPanel3.setBorder(javax.swing.BorderFactory.createTitledBorder("Channels"));

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

        javax.swing.GroupLayout jPanel3Layout = new javax.swing.GroupLayout(jPanel3);
        jPanel3.setLayout(jPanel3Layout);
        jPanel3Layout.setHorizontalGroup(
            jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(chScrollPane, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 263, Short.MAX_VALUE)
        );
        jPanel3Layout.setVerticalGroup(
            jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(chScrollPane, javax.swing.GroupLayout.DEFAULT_SIZE, 176, Short.MAX_VALUE)
        );

        jSplitPane2.setRightComponent(jPanel3);

        jSplitPane4.setRightComponent(jSplitPane2);

        jSplitPane6.setTopComponent(jSplitPane4);

        jScrollPane2.setViewportView(jSplitPane6);

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jScrollPane2, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 653, Short.MAX_VALUE)
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jScrollPane2, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 566, Short.MAX_VALUE)
        );
    }// </editor-fold>//GEN-END:initComponents

    private void refreshButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_refreshButtonActionPerformed
        // TODO add your handling code here:
        if( deviceStatus() ) {
            refreshAttributes();
            refreshChannels();
        }
    }//GEN-LAST:event_refreshButtonActionPerformed

    private void jButton1ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton1ActionPerformed
        // TODO add your handling code here:
        jDialog1.setVisible(false);
    }//GEN-LAST:event_jButton1ActionPerformed

    private void partnerButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_partnerButtonActionPerformed
        // TODO add your handling code here:
        jDialog1.setVisible(true);
    }//GEN-LAST:event_partnerButtonActionPerformed

    private void commandLineTextFieldActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_commandLineTextFieldActionPerformed
        // TODO add your handling code here:
        String command = commandLineTextField.getText();
        commandLineTextField.setText("");
        cmdTextArea.append(">"+command+"\n");
        cmdTextArea.setCaretPosition(cmdTextArea.getDocument().getLength());
        
        try {
            cmdTextArea.append(
                    commandLineRef.executeArgs(tDevice.deviceID, command) + "\n" );
        } catch (Exception e) {
            System.out.println("Not working");
        }
        cmdTextArea.setCaretPosition(cmdTextArea.getDocument().getLength());
}//GEN-LAST:event_commandLineTextFieldActionPerformed

    private void killButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_killButtonActionPerformed

        deviceConfigure.killDevice( getDeviceID() );
        deviceManager.refreshDevices();
    }//GEN-LAST:event_killButtonActionPerformed
    
    
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JTable AttributeTable;
    private javax.swing.JTable ChannelTable;
    private javax.swing.JPanel ControlPanel;
    private javax.swing.JLabel addressLabel;
    private javax.swing.JScrollPane attribScrollPanel;
    private javax.swing.JScrollPane chScrollPane;
    private javax.swing.JScrollPane cmdScrollPane;
    private javax.swing.JTextArea cmdTextArea;
    private javax.swing.JTextField commandLineTextField;
    private javax.swing.JPanel deviceInfoPanel;
    private javax.swing.JLabel deviceLabel;
    private javax.swing.JScrollPane errScrollPane;
    private javax.swing.JTextArea errTextArea;
    private javax.swing.JButton jButton1;
    private javax.swing.JDialog jDialog1;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jLabel2;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JPanel jPanel2;
    private javax.swing.JPanel jPanel3;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JScrollPane jScrollPane2;
    private javax.swing.JSplitPane jSplitPane1;
    private javax.swing.JSplitPane jSplitPane2;
    private javax.swing.JSplitPane jSplitPane3;
    private javax.swing.JSplitPane jSplitPane4;
    private javax.swing.JSplitPane jSplitPane5;
    private javax.swing.JSplitPane jSplitPane6;
    private javax.swing.JTabbedPane jTabbedPane1;
    private javax.swing.JTable jTable1;
    private javax.swing.JButton killButton;
    private javax.swing.JLabel moduleLabel;
    private javax.swing.JButton partnerButton;
    private javax.swing.JLabel pingLabel;
    private javax.swing.JButton refreshButton;
    private javax.swing.JLabel statusLabel;
    // End of variables declaration//GEN-END:variables
    
}
