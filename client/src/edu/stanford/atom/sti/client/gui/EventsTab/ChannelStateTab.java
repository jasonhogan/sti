/** @file ChannelStateTab.java
 *  @author Jason Michael Hogan
 *  @brief Source-file for the class ChannelStateTab
 *  @section license License
 *
 *  Copyright (C) 2012 Jason Hogan <hogan@stanford.edu>\n
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

package edu.stanford.atom.sti.client.gui.EventsTab;

import java.text.ParseException;
import javax.swing.event.ChangeEvent;
import javax.swing.event.DocumentListener;
import javax.swing.event.DocumentEvent;
import edu.stanford.atom.sti.client.comm.bl.*;
import edu.stanford.atom.sti.client.comm.bl.device.*;
import javax.swing.RowFilter;
import edu.stanford.atom.sti.client.gui.table.STITableModel;
import java.util.Vector;
import java.util.HashMap;

import java.beans.PropertyChangeListener;

import java.util.List;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;

public class ChannelStateTab extends javax.swing.JPanel implements DataManagerListener, DeviceCollectionListener {
    
    private int[] filterColumns = new int[] {};

    private RowFilter<STITableModel, Object> timeFilterAfter = null;
    private RowFilter<STITableModel, Object> timeFilterBefore = null;
    private RowFilter<STITableModel, Object> timeFilterEqualStart = null;
    private RowFilter<STITableModel, Object> timeFilterOr1 = null;
    private RowFilter<STITableModel, Object> timeFilterOr2 = null;
    private RowFilter<STITableModel, Object> timeFilterEqualEnd = null;
    private RowFilter<STITableModel, Object> timeFilterAnd = null;
    private List<RowFilter<STITableModel, Object>> timeFiltersOr1 = new ArrayList<RowFilter<STITableModel, Object>>(2);
    private List<RowFilter<STITableModel, Object>> timeFiltersOr2 = new ArrayList<RowFilter<STITableModel, Object>>(2);
    private List<RowFilter<STITableModel, Object>> timeFilters = new ArrayList<RowFilter<STITableModel, Object>>(2);

    private RowFilter<STITableModel, Object> textFilter = RowFilter.regexFilter("", filterColumns);

    private RowFilter<STITableModel, Object> deviceNameFilter = null;
    private RowFilter<STITableModel, Object> deviceModuleFilter = null;
    private RowFilter<STITableModel, Object> deviceAddressFilter = null;
    private List<RowFilter<STITableModel, Object>> deviceFilters = new ArrayList<RowFilter<STITableModel, Object>>(3);
    private RowFilter<STITableModel, Object>  deviceFilter = RowFilter.regexFilter("", filterColumns);

    private RowFilter<STITableModel, Object> totalFilter = null;
    private List<RowFilter<STITableModel, Object>> allFilters = new ArrayList<RowFilter<STITableModel, Object>>(3);

    private boolean filtersInitialized = false;

    private double minimumTime = 0;
    private double maximumTime = 1;
    private boolean updatingSlider = false;
    private STINumberFormat stiNumberFormat = new STINumberFormat();
    private STITableNumberFormat stiTableNumberFormat = new STITableNumberFormat();

    private int timeAtStateCol = 6;
    
    private Vector< DataManager.EventTableRow > events = null;
    
//    private EventTableBiSliderListener eventTableBiSliderListener = new EventTableBiSliderListener();

    private java.beans.PropertyChangeListener timeFieldPropertyChangeListener = new PropertyChangeListener() {

        @Override
        public void propertyChange(java.beans.PropertyChangeEvent evt) {
            updateTimeSliderValue();
        }
    };

    public ChannelStateTab() {
        initComponents();

        
        setupStateTable();
        setupFilter();

        timeSlider.addChangeListener(new javax.swing.event.ChangeListener() {

            public void stateChanged(javax.swing.event.ChangeEvent e) {
//                ((ChangeEvent)e);
                //throw new UnsupportedOperationException("Not supported yet.");
                //System.out.println("Changed: " + timeSlider.getValue());
                if(!updatingSlider) {
                    double newTime = getSliderTime();
                    timeTextField.setValue(newTime);
                }
                if(!timeSlider.getValueIsAdjusting()) {
                    updateTimeSliderValue();
                }
            }
        });

        deviceComboBox.addItem("All Devices");

        timeTextField.addPropertyChangeListener("value", timeFieldPropertyChangeListener);

//        setupTimeFilter();
        deviceFilter = RowFilter.regexFilter("", filterColumns);
        textFilter = RowFilter.regexFilter("", filterColumns);

        filtersInitialized = true;


        //time formater
        stateTable.setTableCellRenderer(stateTable.convertColumnIndexToView(timeAtStateCol), stiTableNumberFormat);



     //        stateTable.getColumn("Time").setPreferredWidth(100);
//        stateTable.getColumn("Time").setMinWidth(100);

//        columnSelectComboBox.setSelectedIndex(columnSelectComboBox.getItemCount() - 1); //set to "All"
    }

    private double getSliderTime() {
        return getMinimumTime() + (getMaximumTime() - getMinimumTime()) * (timeSlider.getValue() / 100.0);
    }



    private HashMap<Integer, DataManager.StateTableRow> stateTableData = null;
    
    private void calculateChannelStates(double time) {
        if(events == null)
            return;
        
        //sort events ascending
        Collections.sort(events, new Comparator<DataManager.EventTableRow>() {
            public int compare(DataManager.EventTableRow left, DataManager.EventTableRow right) {
                double diff = left.getTime() - right.getTime();
                if (diff > 0) return 1;
                if (diff < 0) return -1;
                return 0;   //equal
            }
        });

        int channelsDefined = 0;
        DataManager.StateTableRow row = null;
        boolean found = false;

        for(DataManager.StateTableRow rowI : stateTableData.values()) {
            rowI.reset();
        }
        
        //Counts down through the event list, defining the state of each channel
        //based on the value of the two most recent events (for "value" and "last value").
        //Stops when all channels' states are defined or their are no more events
        //to check (in which case some channels are Undefined.
        for(int i = events.size() - 1; i >= 0 && channelsDefined < stateTableData.size(); i--) {

            //find the first event that happened before "time"
            if(events.elementAt(i).getTime() <= time) {
                found = true;
            }
            if(found) {
                //each channel is represented exactly once in the state table
                row = stateTableData.get(events.elementAt(i).getParserChannelNumber());
                
                if(row != null && row.timeAtStateInitialized() && !row.lastValueInitialized()) {
                    //The second time this channel came up; get the previous value.
                    row.setLastValue(events.elementAt(i).getValue());
                    channelsDefined++;  //this channel is fully defined
                }
                if(row != null && !row.timeAtStateInitialized()) {
                    //The first time this channel came up; get the time and value.
                    row.setTimeAtState(time - events.elementAt(i).getTime());
                    row.setValue(events.elementAt(i).getValue());
                }
            }
        }
    }
    
    public void getData(DataManagerEvent event) {

        events = event.getEventTableRowData();
        stateTableData = event.getStateTableData();

//        stateTable.getModel().setDataVector( event.getStateTableData(minimumTime) );

        double startTime = 0;
        double endTime = 1;
        double time;
        if(events != null && events.size() > 0) {
            for(int i = 0; i < events.size(); i++) {
                time = events.elementAt(i).getTime();
                if(time > endTime) {
                    endTime = time;
                }
            }
        }
        minimumTime = startTime;
        maximumTime = endTime;
        
        timeTextField.setValue(getSliderTime());
        
        
        
//        setupTimeFilter();

//        stateTable.filterTable(getJointFilter(), filterColumns);
    }

    private void installStateDataInTable() {
        if(stateTableData == null) {
            return;
        }
        Vector< Vector<Object> > tableData = new Vector< Vector<Object> >();
        int i = 0;
        for(DataManager.StateTableRow row : stateTableData.values()) {
            tableData.addElement(row.getRow());
            i++;
        }
        stateTable.getModel().setDataVector(tableData);
    }


    private double getMinimumTime() {
        return minimumTime;
    }
    private double getMaximumTime() {
        return maximumTime;
    }
    public void setupTimeFilter() {
        try {
            timeFiltersOr1.clear();
            timeFiltersOr2.clear();
            timeFilters.clear();


            timeFilterBefore = RowFilter.numberFilter(RowFilter.ComparisonType.BEFORE,
                    convertStringToTime(timeTextField.getText(), maximumTime), 0);

            timeFilterEqualEnd = RowFilter.numberFilter(RowFilter.ComparisonType.EQUAL,
                    convertStringToTime(timeTextField.getText(), maximumTime), 0);

            timeFiltersOr1.add(timeFilterAfter);
            timeFiltersOr1.add(timeFilterEqualStart);
            timeFiltersOr2.add(timeFilterBefore);
            timeFiltersOr2.add(timeFilterEqualEnd);

            timeFilterOr1 = RowFilter.orFilter(timeFiltersOr1);
            timeFilterOr2 = RowFilter.orFilter(timeFiltersOr2);

            timeFilters.add(timeFilterOr1);
            timeFilters.add(timeFilterOr2);

            timeFilterAnd = RowFilter.andFilter(timeFilters);

        } catch (java.util.regex.PatternSyntaxException e) {
            return;
        }
    }        
    public RowFilter<STITableModel, Object> getTimeFilter() {
            return timeFilterAnd;
    }


//    private class EventTableBiSliderListener extends BiSliderAdapter {
//
//        @Override
//        public void newValues(BiSliderEvent evt) {
//            if (!updatingSlider) {
//                double newStart = minimumTime + (maximumTime - minimumTime) * (evt.getMinimum() / 100);
//                double newEnd = minimumTime + (maximumTime - minimumTime) * (evt.getMaximum() / 100);
//                timeTextField.setValue(newEnd);
//
//            }
//            setupTimeFilter();
//
//            stateTable.filterTable(getJointFilter(), filterColumns);
//        }
//    }

    class STITableNumberFormat extends javax.swing.table.DefaultTableCellRenderer {
	STINumberFormat formatter;
	public STITableNumberFormat() {
            super();
            setHorizontalAlignment(javax.swing.JLabel.RIGHT);
        }

        @Override
	public void setValue(Object value) {
	    if (formatter == null) {
		formatter = new STINumberFormat();
	    }
            try {
                setText((value == null) ? "" : formatter.valueToString(value));
            } catch(Exception e) {
                setText("");
            }
	}
    }
    class STINumberFormat extends javax.swing.text.NumberFormatter {

        @Override
        public Object stringToValue(String text) throws ParseException {
            String scrubbedNumber = text.replaceAll("ns|us|ms|s|\\|", "");
            return super.stringToValue(scrubbedNumber);
        }

            @Override
            public String valueToString(Object value) throws ParseException {
                String commaFormated =  super.valueToString(value);
                if(commaFormated.contains(".")) {
                    String[] split = commaFormated.split("\\.");
                    commaFormated = split[0];    //"rounds" to nearest ns
                }
                String unitFormated = "";
               // temp.replaceAll(",", ":");
                String[] dividedByUnits = commaFormated.split(",");
                int lastUnit = dividedByUnits.length - 1;
                if(dividedByUnits.length >= 1) { unitFormated = dividedByUnits[lastUnit - 0] + "ns" + unitFormated;}
                if(dividedByUnits.length >= 2) { unitFormated = dividedByUnits[lastUnit - 1] + "us|" + unitFormated;}
                if(dividedByUnits.length >= 3) { unitFormated = dividedByUnits[lastUnit - 2] + "ms|" + unitFormated;}
                if(dividedByUnits.length >= 4) { unitFormated = dividedByUnits[lastUnit - 3] + "s|" + unitFormated;}
                if(dividedByUnits.length >= 5) {
                    for(int j = 4; j < dividedByUnits.length; j++) {
                        unitFormated = dividedByUnits[lastUnit - j] + "," + unitFormated;
                    }
                }
                return unitFormated;
            }
       }
    private class DeviceComboBoxEntry {

        private Device device_;
        public DeviceComboBoxEntry(Device device) {
            device_ = device;
        }
        public String getAddress() {
            return device_.address();
        }
        public String getName() {
            return device_.name();
        }
        public String getModule() {
            return "" + device_.module();
        }
        @Override
        public String toString() {
            return "<" + device_.name() + ", Module_" + device_.module() + ", " + device_.address() + ">";
        }
    }

    private void updateTimeSliderValue() {
        
        double time = 0;

        time = convertStringToTime(timeTextField.getText(), maximumTime);

        if(time < minimumTime) time = minimumTime;
        if(time > maximumTime) time = maximumTime;

        updatingSlider = true;
        double range = maximumTime - minimumTime;
        if(range > 0) {
            timeSlider.setValue(  (int)(100.0*(time - minimumTime) / range));
        } else {
            timeSlider.setValue(100);
        }
        try {
            timeTextField.setText(stiNumberFormat.valueToString(time));
        } catch (java.text.ParseException e) {
            timeTextField.setText(String.valueOf(maximumTime));
        }
        updatingSlider = false;

        if(!timeSlider.getValueIsAdjusting()) {
            calculateChannelStates( convertStringToTime(timeTextField.getText(), maximumTime) );
            installStateDataInTable();
        }
    }


    public double convertStringToTime(String time, double defaultValue) {
        double convert = defaultValue;
        try {
            convert = Double.parseDouble(stiNumberFormat.stringToValue(time).toString());
        } catch (Exception e) {
        }
        return convert;
    }

    public void addDevice(Device device) {
        DeviceComboBoxEntry newEntry = new DeviceComboBoxEntry(device);
        deviceComboBox.removeItem(newEntry);
        deviceComboBox.addItem(newEntry);
    }
    public void removeDevice(Device device) {
        DeviceComboBoxEntry newEntry = new DeviceComboBoxEntry(device);
        deviceComboBox.removeItem(newEntry);
    }
    public void handleDeviceEvent(DeviceEvent evt) {
    }
    public void setDeviceManagerStatus(DeviceManager.DeviceManagerStatus status) {
    }

    public void setupStateTable() {

        stateTable.getModel().setDataVector(new Object[][]{},
                new String[]{
            "Name", "Device", "Address", "Module", "Channel", "Value", "Time at State", "Last Value"});

        stateTable.getModel().setEditableColumns(
                new boolean[] {
            false, false, false, false, false, 
            false, false, false});

        stateTable.addColumnSelectionPopupMenu();

        stateTable.getModel().setVisible(2, false);    // Address
        stateTable.getModel().setVisible(3, false);    // Module

    }

    public void setupFilter() {
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
                        try {
                            textFilter = RowFilter.regexFilter(filterTextField.getText(), filterColumns);

                            stateTable.filterTable(
                                getJointFilter(), filterColumns);
                        } catch(Exception e) {
                        }

                    }

        });
    }

    private RowFilter<STITableModel, Object> getJointFilter() {

        allFilters.clear();
        allFilters.add(textFilter);
        allFilters.add(getTimeFilter());
        allFilters.add(deviceFilter);
        totalFilter = RowFilter.andFilter(allFilters);
        
        return totalFilter;

    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        eventPanel = new javax.swing.JPanel();
        jScrollPane1 = new javax.swing.JScrollPane();
        stateTable = new edu.stanford.atom.sti.client.gui.table.STITable();
        filterPanel = new javax.swing.JPanel();
        filterTextField = new javax.swing.JTextField();
        columnSelectComboBox = new javax.swing.JComboBox();
        resetButton = new javax.swing.JButton();
        deviceComboBox = new javax.swing.JComboBox();
        jPanel1 = new javax.swing.JPanel();
        timeTextField = new javax.swing.JFormattedTextField(new STINumberFormat());
        timeSlider = new javax.swing.JSlider();

        eventPanel.setBorder(javax.swing.BorderFactory.createTitledBorder("Channel State"));

        stateTable.setAutoResizeMode(javax.swing.JTable.AUTO_RESIZE_NEXT_COLUMN);
        stateTable.setColumnSelectionAllowed(true);
        jScrollPane1.setViewportView(stateTable);
        stateTable.getColumnModel().getSelectionModel().setSelectionMode(javax.swing.ListSelectionModel.SINGLE_INTERVAL_SELECTION);

        javax.swing.GroupLayout eventPanelLayout = new javax.swing.GroupLayout(eventPanel);
        eventPanel.setLayout(eventPanelLayout);
        eventPanelLayout.setHorizontalGroup(
            eventPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 557, Short.MAX_VALUE)
        );
        eventPanelLayout.setVerticalGroup(
            eventPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 293, Short.MAX_VALUE)
        );

        filterPanel.setBorder(javax.swing.BorderFactory.createTitledBorder("Filter"));
        filterPanel.setMinimumSize(new java.awt.Dimension(100, 0));

        columnSelectComboBox.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "Time", "Value", "Device", "Address", "Module", "Channel", "Name", "I/O", "Type", "File", "Line", "All" }));
        columnSelectComboBox.setSelectedIndex(columnSelectComboBox.getItemCount() - 1);
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

        deviceComboBox.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                deviceComboBoxActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout filterPanelLayout = new javax.swing.GroupLayout(filterPanel);
        filterPanel.setLayout(filterPanelLayout);
        filterPanelLayout.setHorizontalGroup(
            filterPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(filterPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(filterPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(deviceComboBox, 0, 517, Short.MAX_VALUE)
                    .addGroup(filterPanelLayout.createSequentialGroup()
                        .addComponent(filterTextField, javax.swing.GroupLayout.DEFAULT_SIZE, 297, Short.MAX_VALUE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                        .addComponent(columnSelectComboBox, javax.swing.GroupLayout.PREFERRED_SIZE, 139, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                        .addComponent(resetButton)))
                .addContainerGap())
        );
        filterPanelLayout.setVerticalGroup(
            filterPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(filterPanelLayout.createSequentialGroup()
                .addGroup(filterPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(resetButton)
                    .addComponent(columnSelectComboBox, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(filterTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(deviceComboBox, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        jPanel1.setBorder(javax.swing.BorderFactory.createTitledBorder("Time"));
        jPanel1.setPreferredSize(new java.awt.Dimension(550, 63));

        timeTextField.setPreferredSize(new java.awt.Dimension(125, 20));
        timeTextField.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                timeTextFieldActionPerformed(evt);
            }
        });

        timeSlider.setPaintTicks(true);

        javax.swing.GroupLayout jPanel1Layout = new javax.swing.GroupLayout(jPanel1);
        jPanel1.setLayout(jPanel1Layout);
        jPanel1Layout.setHorizontalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, jPanel1Layout.createSequentialGroup()
                .addComponent(timeSlider, javax.swing.GroupLayout.DEFAULT_SIZE, 411, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(timeTextField, javax.swing.GroupLayout.PREFERRED_SIZE, 130, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap())
        );
        jPanel1Layout.setVerticalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel1Layout.createSequentialGroup()
                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(timeTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(timeSlider, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addContainerGap())
        );

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jPanel1, javax.swing.GroupLayout.DEFAULT_SIZE, 569, Short.MAX_VALUE)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(filterPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addContainerGap())
            .addComponent(eventPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                .addComponent(jPanel1, javax.swing.GroupLayout.PREFERRED_SIZE, 69, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(eventPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(filterPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap())
        );
    }// </editor-fold>//GEN-END:initComponents

private void columnSelectComboBoxActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_columnSelectComboBoxActionPerformed

    int index = columnSelectComboBox.getSelectedIndex();

    if (index >= 0) {
        if (index < 11) {
            filterColumns = new int[]{index};
        }
        if (index == 11) {    //"All" selected
            filterColumns = new int[]{};
        }
        //Apply the current filter text to the newly selected column
        textFilter = RowFilter.regexFilter(filterTextField.getText(), filterColumns);
        stateTable.filterTable(getJointFilter(), filterColumns);
    }
}//GEN-LAST:event_columnSelectComboBoxActionPerformed

private void resetButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_resetButtonActionPerformed
    filterTextField.setText("");

    deviceComboBox.setSelectedIndex(0);
}//GEN-LAST:event_resetButtonActionPerformed

private void deviceComboBoxActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_deviceComboBoxActionPerformed

    if (!filtersInitialized) {
        return;
    }
    if(deviceComboBox.getSelectedIndex() == 0) {
        //All Devices
        deviceFilter = RowFilter.regexFilter("", filterColumns);
        stateTable.filterTable(getJointFilter(), filterColumns);
        return;
    }

    DeviceComboBoxEntry deviceEntry = (DeviceComboBoxEntry) deviceComboBox.getSelectedItem();
    //{"Time", "Value","Device","Address", "Module","Channel","I/O","Type","File","Line","All"}
    deviceNameFilter = RowFilter.regexFilter(deviceEntry.getName(), 2);
    deviceModuleFilter = RowFilter.regexFilter(deviceEntry.getModule(), 4);
    deviceAddressFilter = RowFilter.regexFilter(deviceEntry.getAddress(), 3);

    deviceFilters.clear();
    deviceFilters.add(deviceNameFilter);
    deviceFilters.add(deviceModuleFilter);
    deviceFilters.add(deviceAddressFilter);

    deviceFilter = RowFilter.andFilter(deviceFilters);
    stateTable.filterTable(getJointFilter(), filterColumns);
}//GEN-LAST:event_deviceComboBoxActionPerformed

private void timeTextFieldActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_timeTextFieldActionPerformed
    updateTimeSliderValue();
}//GEN-LAST:event_timeTextFieldActionPerformed


    // Variables declaration - do not modify//GEN-BEGIN:variables
    javax.swing.JComboBox columnSelectComboBox;
    javax.swing.JComboBox deviceComboBox;
    javax.swing.JPanel eventPanel;
    javax.swing.JPanel filterPanel;
    javax.swing.JTextField filterTextField;
    javax.swing.JPanel jPanel1;
    javax.swing.JScrollPane jScrollPane1;
    javax.swing.JButton resetButton;
    edu.stanford.atom.sti.client.gui.table.STITable stateTable;
    javax.swing.JSlider timeSlider;
    javax.swing.JFormattedTextField timeTextField;
    // End of variables declaration//GEN-END:variables
}
