/** @file NewDeviceTab.java
 *  @author Jason Michael Hogan
 *  @brief Source-file for the class DeviceTab
 *  @section license License
 *
 *  Copyright (C) 2010 Jason Hogan <hogan@stanford.edu>\n
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

import edu.stanford.atom.sti.client.comm.bl.device.Device;
import edu.stanford.atom.sti.client.comm.bl.device.DeviceEvent;
import edu.stanford.atom.sti.corba.Types.TAttribute;
import edu.stanford.atom.sti.corba.Types.TChannel;
import edu.stanford.atom.sti.corba.Types.TPartner;
import edu.stanford.atom.sti.client.comm.bl.TChannelDecode;
import edu.stanford.atom.sti.client.comm.bl.TDataMixedDecode;
import javax.swing.table.DefaultTableModel;
import edu.stanford.atom.sti.client.gui.table.STITableCellEditor;
import edu.stanford.atom.sti.client.gui.table.ButtonCellEditor;
import edu.stanford.atom.sti.client.gui.table.ButtonCellRenderer;
import edu.stanford.atom.sti.client.gui.table.*;
import javax.swing.event.TableModelEvent;
import javax.swing.event.TableModelListener;
import javax.swing.SwingUtilities;
import javax.swing.JButton;
import java.util.Vector;

public class NewDeviceTab extends javax.swing.JPanel {

    private Vector<JButton> ioButtons = new Vector<JButton>();
    
    private RefreshBarCellRenderer refreshCellRenderer = new RefreshBarCellRenderer();
    private ButtonCellEditor buttonTableCellEditor = new ButtonCellEditor();
    private ButtonCellRenderer buttonCellRenderer = new ButtonCellRenderer(ioButtons);

    private Device device;
    private String tabTitle = "";
    private boolean refreshingAttributeTable = false;
    private boolean refreshingChannels = false;

    private STITableCellEditor stiTableCellEditor = new STITableCellEditor();

    /** Creates new form NewDeviceTab */
    public NewDeviceTab(Device device) {
        this.device = device;
        initComponents();
        setDeviceInfo();
        setupTables();

        refreshAttributes();
        refreshChannels();
        refreshPartners();
//        updateUI();

//        buttonTableCellEditor.refresh();

 //       channelsTable.doLayout();
    }

    public void measureButtonPressed(int rowIndex) {

        short channel = Short.decode((channelsTable.getValueAt(

                channelsTable.convertRowIndexToView(rowIndex),
                channelsTable.convertColumnIndexToView(0)

                )).toString());

        System.out.println("Measure channel: " + channel
                );

    //    ioButtons.elementAt(rowIndex).setText("Stop");

        //a test
        setIndeterminateLater(refreshCellRenderer.getProgressBar(rowIndex),
                !refreshCellRenderer.getProgressBar(rowIndex).isIndeterminate());
   //     setIndeterminateLater(refreshCellRenderer.getProgressBar(channelsTable.convertRowIndexToView(rowIndex)),
     //           !refreshCellRenderer.getProgressBar(channelsTable.convertRowIndexToView(rowIndex)).isIndeterminate());

    //    device.read(channel, null, null);

    }
    
    
    private void setIndeterminateLater(final javax.swing.JProgressBar progressBar, final boolean status) {

        SwingUtilities.invokeLater(new Runnable() {

            public void run() {
                progressBar.setIndeterminate(status);
            }
        });
    }

    private void setupTables() {


        partnersTable.getModel().setDataVector(new Object[][]{}, new String[] {
                    "Device Name", "IP Address", "Module", "Required", "Registered", "Event Target", "Mutual"
                });
        partnersTable.getModel().setEditableColumns(
                new boolean[] {false, false, false, false, false, false, false});

        
        channelsTable.getModel().setDataVector(new Object[][]{},
                new String[]{"Channel", "Name", "Type", "Value", "I/O", "Status", "Data"});

        channelsTable.getModel().setEditableColumns(
                new boolean[] {false, true, false, true, true, false, false});

//        channelsTable.addColumnSelectionPopupMenu();

        channelsTable.getColumnModel().getColumn(4).setCellEditor(buttonTableCellEditor);
        channelsTable.getColumnModel().getColumn(4).setCellRenderer(buttonCellRenderer);
        channelsTable.getColumnModel().getColumn(5).setCellRenderer(refreshCellRenderer);


//        channelsTable.getModel().fireTableDataChanged();


        channelsTable.getModel().addTableModelListener(new TableModelListener() {
            public void tableChanged(TableModelEvent evt) {

                if (evt.getType() == TableModelEvent.UPDATE && !refreshingChannels) {
//        channelsTable.getColumnModel().getColumn(channelsTable.convertColumnIndexToModel(4)).setCellEditor(buttonTableCellEditor);
//        channelsTable.getColumnModel().getColumn(channelsTable.convertColumnIndexToModel(4)).setCellRenderer(buttonCellRenderer);
//        channelsTable.getColumnModel().getColumn(channelsTable.convertColumnIndexToModel(5)).setCellRenderer(refreshCellRenderer);
       //             refreshChannels();
         //           channelsTable.repaint();
                }
            }
        });

        attributesTable.getColumnModel().getColumn(1).setCellEditor(stiTableCellEditor);
        attributesTable.getModel().addTableModelListener(new TableModelListener() {
            public void tableChanged(TableModelEvent evt) {

                if (evt.getType() == TableModelEvent.UPDATE && !refreshingAttributeTable) {

                    final int row = evt.getFirstRow();

                    Thread setAttributeThread = new Thread(new Runnable() {
                        public void run() {
                            String key = attributesTable.getValueAt(attributesTable.convertRowIndexToView(row),
                                    attributesTable.convertColumnIndexToView(0)).toString();
                            String value = attributesTable.getValueAt(attributesTable.convertRowIndexToView(row),
                                    attributesTable.convertColumnIndexToView(1)).toString();
                            
                            setIndeterminateLater(deviceStatusBar, true);

                            device.setAttribute(key, value);
                            refreshAttributes();
                            
                            setIndeterminateLater(deviceStatusBar, false);
                      }
                    });

                 //   setAttributeThread.run();
                    setAttributeThread.start();
                }
            }
        });

    }


    private void refreshAttributes() {
        refreshingAttributeTable = true;
        TAttribute[] attributes = device.getAttributes();
        String[] allowedValues;
        
        ((DefaultTableModel)attributesTable.getModel()).setRowCount(attributes.length);
        
        for(int i = 0; i < attributes.length; i++) {
            // set key
            attributesTable.setValueAt(attributes[i].key, 
                            attributesTable.convertRowIndexToView(i), 
                            attributesTable.convertColumnIndexToView(0) );
            
            allowedValues = attributes[i].values;
            if(allowedValues.length > 0) {
                stiTableCellEditor.installComboBoxEditor(i, 1, allowedValues);
            }
            // set value
            attributesTable.setValueAt(attributes[i].value, 
                            attributesTable.convertRowIndexToView(i), 
                            attributesTable.convertColumnIndexToView(1) );
        }
        refreshingAttributeTable = false;
    }

    private void refreshChannels() {
        refreshingChannels = true;



        TChannel[] channels = device.getChannels();

        refreshCellRenderer.setNumberOfRows(channels.length, channelsTable);

        channelsTable.getModel().setRowCount(channels.length);

        ioButtons.clear();

        String channelType;
        String ioType;
        // populate Channel table
        for (int i = 0; i < channels.length; i++) {



            // set channel
            channelsTable.setValueAt(channels[i].channel,
                    channelsTable.convertRowIndexToModel(i),
                    channelsTable.convertColumnIndexToModel(0));

            TChannelDecode channelDecode = new TChannelDecode(channels[i]);

            ioType = channelDecode.IOType();
            final boolean isOutputChannel;
            if(ioType.equals("Output")) {
                ioButtons.addElement(new JButton("Write"));
                isOutputChannel = true;
            }
            else {
                ioButtons.addElement(new JButton("Read"));
                isOutputChannel = false;
            }

            channelType = channelDecode.ChannelType();

 //           channelsTable.setValueAt(ioType,
   //                 channelsTable.convertRowIndexToModel(i),
     //               channelsTable.convertColumnIndexToModel(4));
            channelsTable.setValueAt(channelType,
                    channelsTable.convertRowIndexToModel(i),
                    channelsTable.convertColumnIndexToModel(2));
            
            buttonTableCellEditor.installButtonEditor(i, ioButtons.lastElement());

            //Add button action listener
            final int rowI = i;
            final short channelI = channels[i].channel;
            ioButtons.lastElement().addActionListener(
                    new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {

               // measureButtonPressed(rowI);
//                setIndeterminateLater(refreshCellRenderer.getProgressBar(rowI),
//                !refreshCellRenderer.getProgressBar(rowI).isIndeterminate());

                final JButton thisButton = ((JButton)evt.getSource());
                
                if(refreshCellRenderer.getProgressBar(rowI).isIndeterminate()) {
                    device.stop();
                    return;
                }

                Thread ioChannelThread = new Thread(new Runnable() {
                        public void run() {
                            String oldText = thisButton.getText();



                            setIndeterminateLater(refreshCellRenderer.getProgressBar(rowI), true);

                            channelsTable.convertRowIndexToModel(rowI);
                            channelsTable.convertColumnIndexToModel(3);
//                            short channel = Short.decode((channelsTable.getValueAt(
//                                    channelsTable.convertRowIndexToView(rowI),
//                                    channelsTable.convertColumnIndexToView(0))).toString());
                            boolean success = true;
                            //String stringValue = (String)channelsTable.getModel().getValueAt(channelsTable.convertRowIndexToView(rowI), channelsTable.convertColumnIndexToView(3));

                            String stringValue = (String)channelsTable.getModel().getValueAt(rowI, 3);

                            System.out.println("" + rowI + " " + stringValue);


                            if(stringValue == null)
                                success = false;

                            thisButton.setText("Stop");
                            //device.write(channelI, );
                            edu.stanford.atom.sti.corba.Types.TValMixed parsedMixedVal = new edu.stanford.atom.sti.corba.Types.TValMixed();
                            
                            if(success)
                                parsedMixedVal = device.pythonStringToMixedValue(stringValue);
                            //device.setAttribute("", tabTitle);

                            //edu.stanford.atom.sti.client.comm.bl.TValMixedDecode temp = (new edu.stanford.atom.sti.client.comm.bl.TValMixedDecode(parsedMixedVal));
                            //System.out.println(temp.toString());

                            if(success) {
                                if(isOutputChannel) {
                                    success = device.write(channelI, parsedMixedVal);
                                }
                                else {
                                    edu.stanford.atom.sti.corba.Types.TDataMixed dataMixed = new edu.stanford.atom.sti.corba.Types.TDataMixed();
                                    dataMixed = device.read(channelI, parsedMixedVal);
//                                   channelsTable.getModel().setValueAt(dataMixed.doubleVal(), channelsTable.convertRowIndexToView(rowI), channelsTable.convertColumnIndexToView(6));

                                    TDataMixedDecode dataDecode = new TDataMixedDecode(dataMixed);
                                    channelsTable.getModel().setValueAt(dataDecode.toString(), rowI, 6);
                                }
                            }

                            if(!success) {
                                //report error
                            }

                            setIndeterminateLater(refreshCellRenderer.getProgressBar(rowI), false);
                            thisButton.setText(oldText);
                        }
                });
                ioChannelThread.start();
            }
          });
        }
        refreshingChannels = false;
        channelsTable.repaint();

    }

    private void refreshPartners() {
        TPartner[] partners = device.getPartners();

        partnersTable.getModel().setRowCount(partners.length);

        if(partners == null)
            return;

        for(int row = 0; row < partners.length; row++) {
            partnersTable.getModel().setValueAt(partners[row].deviceName,
                    partnersTable.convertRowIndexToView(row),
                    partnersTable.convertColumnIndexToView(0));

            partnersTable.getModel().setValueAt(partners[row].ipAddress,
                    partnersTable.convertRowIndexToView(row),
                    partnersTable.convertColumnIndexToView(1));

            partnersTable.getModel().setValueAt(partners[row].moduleNum,
                    partnersTable.convertRowIndexToView(row),
                    partnersTable.convertColumnIndexToView(2));

            partnersTable.getModel().setValueAt(partners[row].isRequired,
                    partnersTable.convertRowIndexToView(row),
                    partnersTable.convertColumnIndexToView(3));

            partnersTable.getModel().setValueAt(partners[row].isRegistered,
                    partnersTable.convertRowIndexToView(row),
                    partnersTable.convertColumnIndexToView(4));

            partnersTable.getModel().setValueAt(partners[row].isEventTarget,
                    partnersTable.convertRowIndexToView(row),
                    partnersTable.convertColumnIndexToView(5));

            partnersTable.getModel().setValueAt(partners[row].isMutual,
                    partnersTable.convertRowIndexToView(row),
                    partnersTable.convertColumnIndexToView(6));
        }
    }

    public void handleDeviceEvent(DeviceEvent evt) {
        
        switch(evt.getType()) {
            case AttributeRefresh:
                refreshAttributes();
                break;
            case ChannelRefresh:
                refreshChannels();
                break;
            case PartnerRefresh:
                refreshPartners();
                break;
            case Refresh:
                refreshAttributes();
                refreshChannels();
                refreshPartners();
                break;
            case ErrorStream:
                errorStreamTextArea.append(evt.getErrorMessage() + "\n");
                break;
        }
    }

    public String getTabTitle() {
        return tabTitle;
    }
    public void setTabTitle(String title) {
        tabTitle = title;
    }
    public void setEnabledDeviceTab(boolean enabled) {
    }

    private void setDeviceInfo() {
        deviceNameTextField.setText(device.name());
        addressTextField.setText(device.address());
        moduleTextField.setText( String.valueOf(device.module()) );

        boolean device_status = device.status();
        statusTextField.setText( (device_status ? "Ready" : "Comm Error") );
        setEnabledDeviceTab(device_status);

        long ping = device.ping();
        pingTextField.setText(((ping == 0) ? "< 1" : String.valueOf(ping) )
                + " ms");
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jSplitPane1 = new javax.swing.JSplitPane();
        topSplitPane = new javax.swing.JSplitPane();
        jTabbedPane1 = new javax.swing.JTabbedPane();
        jPanel2 = new javax.swing.JPanel();
        jScrollPane2 = new javax.swing.JScrollPane();
        attributesTable = new javax.swing.JTable();
        jPanel3 = new javax.swing.JPanel();
        jScrollPane1 = new javax.swing.JScrollPane();
        channelsTable = new edu.stanford.atom.sti.client.gui.table.STITable();
        jPanel4 = new javax.swing.JPanel();
        jScrollPane3 = new javax.swing.JScrollPane();
        partnersTable = new edu.stanford.atom.sti.client.gui.table.STITable();
        jPanel1 = new javax.swing.JPanel();
        jPanel8 = new javax.swing.JPanel();
        moduleTextField = new javax.swing.JTextField();
        addressTextField = new javax.swing.JTextField();
        deviceNameTextField = new javax.swing.JTextField();
        jLabel1 = new javax.swing.JLabel();
        jLabel2 = new javax.swing.JLabel();
        jLabel3 = new javax.swing.JLabel();
        jSeparator1 = new javax.swing.JSeparator();
        jPanel9 = new javax.swing.JPanel();
        jLabel4 = new javax.swing.JLabel();
        jLabel5 = new javax.swing.JLabel();
        deviceStatusBar = new javax.swing.JProgressBar();
        refreshButton = new javax.swing.JButton();
        killButton = new javax.swing.JButton();
        statusTextField = new javax.swing.JTextField();
        pingTextField = new javax.swing.JTextField();
        jSplitPane2 = new javax.swing.JSplitPane();
        jTabbedPane2 = new javax.swing.JTabbedPane();
        jPanel5 = new javax.swing.JPanel();
        jScrollPane4 = new javax.swing.JScrollPane();
        outputTextArea = new javax.swing.JTextArea();
        jPanel6 = new javax.swing.JPanel();
        jScrollPane5 = new javax.swing.JScrollPane();
        errorStreamTextArea = new javax.swing.JTextArea();
        jPanel7 = new javax.swing.JPanel();
        jLabel6 = new javax.swing.JLabel();
        commandTextField = new javax.swing.JTextField();

        jSplitPane1.setDividerLocation(250);
        jSplitPane1.setOrientation(javax.swing.JSplitPane.VERTICAL_SPLIT);
        jSplitPane1.setMinimumSize(new java.awt.Dimension(302, 100));
        jSplitPane1.setPreferredSize(new java.awt.Dimension(579, 200));

        topSplitPane.setDividerLocation(80);
        topSplitPane.setDividerSize(1);
        topSplitPane.setOrientation(javax.swing.JSplitPane.VERTICAL_SPLIT);
        topSplitPane.setMinimumSize(new java.awt.Dimension(200, 200));
        topSplitPane.setPreferredSize(new java.awt.Dimension(579, 250));

        jTabbedPane1.setMinimumSize(new java.awt.Dimension(64, 30));

        attributesTable.setModel(new javax.swing.table.DefaultTableModel(
            new Object [][] {
                {null, null},
                {null, null},
                {null, null},
                {null, null}
            },
            new String [] {
                "Key", "Value"
            }
        ) {
            Class[] types = new Class [] {
                java.lang.String.class, java.lang.Object.class
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
        jScrollPane2.setViewportView(attributesTable);

        javax.swing.GroupLayout jPanel2Layout = new javax.swing.GroupLayout(jPanel2);
        jPanel2.setLayout(jPanel2Layout);
        jPanel2Layout.setHorizontalGroup(
            jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jScrollPane2, javax.swing.GroupLayout.DEFAULT_SIZE, 537, Short.MAX_VALUE)
        );
        jPanel2Layout.setVerticalGroup(
            jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jScrollPane2, javax.swing.GroupLayout.DEFAULT_SIZE, 139, Short.MAX_VALUE)
        );

        jTabbedPane1.addTab("Attributes", jPanel2);

        channelsTable.setAutoCreateRowSorter(true);
        channelsTable.setCellSelectionEnabled(true);
        jScrollPane1.setViewportView(channelsTable);
        channelsTable.getColumnModel().getSelectionModel().setSelectionMode(javax.swing.ListSelectionModel.SINGLE_INTERVAL_SELECTION);

        javax.swing.GroupLayout jPanel3Layout = new javax.swing.GroupLayout(jPanel3);
        jPanel3.setLayout(jPanel3Layout);
        jPanel3Layout.setHorizontalGroup(
            jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 537, Short.MAX_VALUE)
        );
        jPanel3Layout.setVerticalGroup(
            jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 139, Short.MAX_VALUE)
        );

        jTabbedPane1.addTab("Channels", jPanel3);

        jScrollPane3.setViewportView(partnersTable);

        javax.swing.GroupLayout jPanel4Layout = new javax.swing.GroupLayout(jPanel4);
        jPanel4.setLayout(jPanel4Layout);
        jPanel4Layout.setHorizontalGroup(
            jPanel4Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jScrollPane3, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 537, Short.MAX_VALUE)
        );
        jPanel4Layout.setVerticalGroup(
            jPanel4Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jScrollPane3, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 139, Short.MAX_VALUE)
        );

        jTabbedPane1.addTab("Partners", jPanel4);

        topSplitPane.setBottomComponent(jTabbedPane1);

        jPanel1.setMaximumSize(new java.awt.Dimension(65544, 80));
        jPanel1.setMinimumSize(new java.awt.Dimension(10, 80));
        jPanel1.setPreferredSize(new java.awt.Dimension(577, 80));
        jPanel1.setLayout(new javax.swing.BoxLayout(jPanel1, javax.swing.BoxLayout.LINE_AXIS));

        jPanel8.setPreferredSize(new java.awt.Dimension(300, 94));

        moduleTextField.setBackground(new java.awt.Color(212, 208, 200));
        moduleTextField.setEditable(false);
        moduleTextField.setBorder(null);

        addressTextField.setBackground(new java.awt.Color(212, 208, 200));
        addressTextField.setEditable(false);
        addressTextField.setBorder(null);

        deviceNameTextField.setEditable(false);
        deviceNameTextField.setBorder(null);
        deviceNameTextField.setMinimumSize(new java.awt.Dimension(150, 20));
        deviceNameTextField.setPreferredSize(new java.awt.Dimension(150, 14));

        jLabel1.setText("Device:");

        jLabel2.setText("Address:");

        jLabel3.setText("Module:");

        javax.swing.GroupLayout jPanel8Layout = new javax.swing.GroupLayout(jPanel8);
        jPanel8.setLayout(jPanel8Layout);
        jPanel8Layout.setHorizontalGroup(
            jPanel8Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel8Layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(jPanel8Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(jLabel1)
                    .addComponent(jLabel2)
                    .addComponent(jLabel3))
                .addGap(18, 18, 18)
                .addGroup(jPanel8Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(addressTextField, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 162, Short.MAX_VALUE)
                    .addComponent(moduleTextField, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 162, Short.MAX_VALUE)
                    .addComponent(deviceNameTextField, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 162, Short.MAX_VALUE))
                .addContainerGap())
        );
        jPanel8Layout.setVerticalGroup(
            jPanel8Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel8Layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(jPanel8Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.CENTER)
                    .addComponent(deviceNameTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jLabel1))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(jPanel8Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.CENTER)
                    .addComponent(addressTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jLabel2))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(jPanel8Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.CENTER)
                    .addComponent(moduleTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jLabel3))
                .addContainerGap())
        );

        jPanel1.add(jPanel8);

        jSeparator1.setOrientation(javax.swing.SwingConstants.VERTICAL);
        jSeparator1.setMaximumSize(new java.awt.Dimension(10, 32767));
        jSeparator1.setMinimumSize(new java.awt.Dimension(10, 10));
        jSeparator1.setPreferredSize(new java.awt.Dimension(10, 10));
        jPanel1.add(jSeparator1);

        jPanel9.setPreferredSize(new java.awt.Dimension(300, 96));

        jLabel4.setText("Status:");

        jLabel5.setText("Ping:");

        deviceStatusBar.setMinimumSize(new java.awt.Dimension(100, 16));

        refreshButton.setText("Refresh");
        refreshButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                refreshButtonActionPerformed(evt);
            }
        });

        killButton.setText("Kill");
        killButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                killButtonActionPerformed(evt);
            }
        });

        statusTextField.setBackground(new java.awt.Color(212, 208, 200));
        statusTextField.setEditable(false);
        statusTextField.setBorder(null);

        pingTextField.setBackground(new java.awt.Color(212, 208, 200));
        pingTextField.setEditable(false);
        pingTextField.setBorder(null);

        javax.swing.GroupLayout jPanel9Layout = new javax.swing.GroupLayout(jPanel9);
        jPanel9.setLayout(jPanel9Layout);
        jPanel9Layout.setHorizontalGroup(
            jPanel9Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, jPanel9Layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(jPanel9Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                    .addGroup(jPanel9Layout.createSequentialGroup()
                        .addGroup(jPanel9Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(jLabel4)
                            .addComponent(jLabel5))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                        .addGroup(jPanel9Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                            .addComponent(pingTextField)
                            .addComponent(statusTextField, javax.swing.GroupLayout.DEFAULT_SIZE, 142, Short.MAX_VALUE)))
                    .addComponent(deviceStatusBar, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 8, Short.MAX_VALUE)
                .addGroup(jPanel9Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                    .addComponent(refreshButton)
                    .addComponent(killButton, javax.swing.GroupLayout.PREFERRED_SIZE, 73, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addContainerGap())
        );
        jPanel9Layout.setVerticalGroup(
            jPanel9Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel9Layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(jPanel9Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(killButton)
                    .addGroup(jPanel9Layout.createSequentialGroup()
                        .addGroup(jPanel9Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                            .addComponent(jLabel4)
                            .addComponent(statusTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addGroup(jPanel9Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                            .addComponent(jLabel5)
                            .addComponent(pingTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addGroup(jPanel9Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.CENTER)
                            .addComponent(refreshButton)
                            .addComponent(deviceStatusBar, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        jPanel1.add(jPanel9);

        topSplitPane.setLeftComponent(jPanel1);

        jSplitPane1.setTopComponent(topSplitPane);

        jSplitPane2.setDividerLocation(135);
        jSplitPane2.setDividerSize(0);
        jSplitPane2.setOrientation(javax.swing.JSplitPane.VERTICAL_SPLIT);
        jSplitPane2.setResizeWeight(1.0);
        jSplitPane2.setMinimumSize(new java.awt.Dimension(90, 190));
        jSplitPane2.setPreferredSize(new java.awt.Dimension(577, 192));

        jTabbedPane2.setMinimumSize(new java.awt.Dimension(87, 0));
        jTabbedPane2.setPreferredSize(new java.awt.Dimension(575, 150));

        jPanel5.setPreferredSize(new java.awt.Dimension(570, 150));

        outputTextArea.setColumns(20);
        outputTextArea.setEditable(false);
        outputTextArea.setRows(5);
        jScrollPane4.setViewportView(outputTextArea);

        javax.swing.GroupLayout jPanel5Layout = new javax.swing.GroupLayout(jPanel5);
        jPanel5.setLayout(jPanel5Layout);
        jPanel5Layout.setHorizontalGroup(
            jPanel5Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jScrollPane4, javax.swing.GroupLayout.DEFAULT_SIZE, 537, Short.MAX_VALUE)
        );
        jPanel5Layout.setVerticalGroup(
            jPanel5Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jScrollPane4, javax.swing.GroupLayout.DEFAULT_SIZE, 106, Short.MAX_VALUE)
        );

        jTabbedPane2.addTab("Output Stream", jPanel5);

        errorStreamTextArea.setColumns(20);
        errorStreamTextArea.setEditable(false);
        errorStreamTextArea.setRows(5);
        jScrollPane5.setViewportView(errorStreamTextArea);

        javax.swing.GroupLayout jPanel6Layout = new javax.swing.GroupLayout(jPanel6);
        jPanel6.setLayout(jPanel6Layout);
        jPanel6Layout.setHorizontalGroup(
            jPanel6Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jScrollPane5, javax.swing.GroupLayout.DEFAULT_SIZE, 537, Short.MAX_VALUE)
        );
        jPanel6Layout.setVerticalGroup(
            jPanel6Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jScrollPane5, javax.swing.GroupLayout.DEFAULT_SIZE, 106, Short.MAX_VALUE)
        );

        jTabbedPane2.addTab("Error Stream", jPanel6);

        jSplitPane2.setLeftComponent(jTabbedPane2);

        jPanel7.setBackground(new java.awt.Color(255, 255, 255));
        jPanel7.setMaximumSize(new java.awt.Dimension(2147483647, 22));
        jPanel7.setMinimumSize(new java.awt.Dimension(19, 22));
        jPanel7.setPreferredSize(new java.awt.Dimension(488, 22));
        jPanel7.setLayout(new java.awt.BorderLayout());

        jLabel6.setBackground(new java.awt.Color(255, 255, 255));
        jLabel6.setFont(new java.awt.Font("Tahoma", 0, 18));
        jLabel6.setText("> ");
        jPanel7.add(jLabel6, java.awt.BorderLayout.WEST);

        commandTextField.setFont(new java.awt.Font("Tahoma", 0, 14));
        commandTextField.setToolTipText("Runs STI_Device::execute(...)");
        commandTextField.setAutoscrolls(false);
        commandTextField.setBorder(null);
        commandTextField.setMaximumSize(new java.awt.Dimension(2147483647, 22));
        commandTextField.setMinimumSize(new java.awt.Dimension(6, 22));
        commandTextField.setPreferredSize(new java.awt.Dimension(200, 22));
        commandTextField.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                commandTextFieldActionPerformed(evt);
            }
        });
        jPanel7.add(commandTextField, java.awt.BorderLayout.CENTER);

        jSplitPane2.setRightComponent(jPanel7);

        jSplitPane1.setBottomComponent(jSplitPane2);

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jSplitPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 546, Short.MAX_VALUE)
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jSplitPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 410, Short.MAX_VALUE)
                .addContainerGap())
        );
    }// </editor-fold>//GEN-END:initComponents

    private void commandTextFieldActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_commandTextFieldActionPerformed
        String command = commandTextField.getText();
        commandTextField.setText("");
        outputTextArea.append(">"+command+"\n");
        outputTextArea.setCaretPosition(outputTextArea.getDocument().getLength());

        try {
            outputTextArea.append( device.execute(command) + "\n" );
        } catch (Exception e) {
            outputTextArea.append( "<Network error>" );
            System.out.println("<Network error>");
        }
        outputTextArea.setCaretPosition(outputTextArea.getDocument().getLength());
    }//GEN-LAST:event_commandTextFieldActionPerformed

    private void killButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_killButtonActionPerformed
        device.kill();
    }//GEN-LAST:event_killButtonActionPerformed

    private void refreshButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_refreshButtonActionPerformed
        // TODO add your handling code here:
        refreshPartners();
        refreshAttributes();
    }//GEN-LAST:event_refreshButtonActionPerformed


    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JTextField addressTextField;
    private javax.swing.JTable attributesTable;
    private edu.stanford.atom.sti.client.gui.table.STITable channelsTable;
    private javax.swing.JTextField commandTextField;
    private javax.swing.JTextField deviceNameTextField;
    private javax.swing.JProgressBar deviceStatusBar;
    private javax.swing.JTextArea errorStreamTextArea;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jLabel2;
    private javax.swing.JLabel jLabel3;
    private javax.swing.JLabel jLabel4;
    private javax.swing.JLabel jLabel5;
    private javax.swing.JLabel jLabel6;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JPanel jPanel2;
    private javax.swing.JPanel jPanel3;
    private javax.swing.JPanel jPanel4;
    private javax.swing.JPanel jPanel5;
    private javax.swing.JPanel jPanel6;
    private javax.swing.JPanel jPanel7;
    private javax.swing.JPanel jPanel8;
    private javax.swing.JPanel jPanel9;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JScrollPane jScrollPane2;
    private javax.swing.JScrollPane jScrollPane3;
    private javax.swing.JScrollPane jScrollPane4;
    private javax.swing.JScrollPane jScrollPane5;
    private javax.swing.JSeparator jSeparator1;
    private javax.swing.JSplitPane jSplitPane1;
    private javax.swing.JSplitPane jSplitPane2;
    private javax.swing.JTabbedPane jTabbedPane1;
    private javax.swing.JTabbedPane jTabbedPane2;
    private javax.swing.JButton killButton;
    private javax.swing.JTextField moduleTextField;
    private javax.swing.JTextArea outputTextArea;
    private edu.stanford.atom.sti.client.gui.table.STITable partnersTable;
    private javax.swing.JTextField pingTextField;
    private javax.swing.JButton refreshButton;
    private javax.swing.JTextField statusTextField;
    private javax.swing.JSplitPane topSplitPane;
    // End of variables declaration//GEN-END:variables


}
