/** @file EventsTab.java
 *  @author Jason Michael Hogan
 *  @brief Source-file for the class EventsTab
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

package edu.stanford.atom.sti.client.gui.EventsTab;

import java.text.ParseException;
import javax.swing.event.DocumentListener;
import javax.swing.event.DocumentEvent;
import edu.stanford.atom.sti.client.comm.bl.*;
import edu.stanford.atom.sti.client.comm.bl.device.*;
import javax.swing.RowFilter;
import edu.stanford.atom.sti.client.gui.table.STITableModel;

import com.visutools.nav.bislider.BiSliderAdapter;
import com.visutools.nav.bislider.BiSliderEvent;

import java.beans.PropertyChangeListener;

import java.util.List;
import java.util.ArrayList;

public class EventsTab extends javax.swing.JPanel implements DataManagerListener, DeviceCollectionListener {
    
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

    private EventTableBiSliderListener eventTableBiSliderListener = new EventTableBiSliderListener();

    private java.beans.PropertyChangeListener timeFieldPropertyChangeListener = new PropertyChangeListener() {

        @Override
        public void propertyChange(java.beans.PropertyChangeEvent evt) {
            updateTimeSliderValues();
        }
    };

    public EventsTab() {
        initComponents();

        
        setupEventsTable();
        setupFilter();
        deviceComboBox.addItem("All Devices");

        startTimeTextField.addPropertyChangeListener("value", timeFieldPropertyChangeListener);
        endTimeTextField.addPropertyChangeListener("value", timeFieldPropertyChangeListener);

        setupTimeFilter();
        deviceFilter = RowFilter.regexFilter("", filterColumns);
        textFilter = RowFilter.regexFilter("", filterColumns);

        filtersInitialized = true;
//        {"Time", "Value","Device","Address", "Module","Channel","I/O","Type","File","Line","All"};



        eventsTable.setTableCellRenderer(0, stiTableNumberFormat);
//        eventsTable.getColumn("Time").setPreferredWidth(100);
//        eventsTable.getColumn("Time").setMinWidth(100);

//        columnSelectComboBox.setSelectedIndex(columnSelectComboBox.getItemCount() - 1); //set to "All"
    }

    public void getData(DataManagerEvent event) {

        eventsTable.getModel().setDataVector( event.getEventTableData() );


        double startTime = 0;
        double endTime = 1;
        double time;
        if(eventsTable.getModel().getRowCount() > 0) {
            for(int i = 0; i < eventsTable.getModel().getRowCount(); i++) {
                time = Double.parseDouble(eventsTable.getModel().getValueAt(i, 0).toString());
                if(time > endTime) {
                    endTime = time;
                }
            }
        }
        minimumTime = startTime;
        maximumTime = endTime;

//        if(eventsTable.getModel().getRowCount() > 0 && eventsTable.getColumnCount() > 0) {
//            eventsTable.getColumn("Time").setCellRenderer(stiTableNumberFormat);
//        }
        startTimeTextField.setValue(startTime);
        endTimeTextField.setValue(endTime);

        timeBiSlilder.addBiSliderListener(eventTableBiSliderListener);

        setupTimeFilter();

        eventsTable.filterTable(getJointFilter(), filterColumns);
    }

    public void setupTimeFilter() {
        try {
            timeFiltersOr1.clear();
            timeFiltersOr2.clear();
            timeFilters.clear();

            timeFilterAfter = RowFilter.numberFilter(RowFilter.ComparisonType.AFTER,
                    convertStringToTime(startTimeTextField.getText(), minimumTime), 0);
            timeFilterBefore = RowFilter.numberFilter(RowFilter.ComparisonType.BEFORE,
                    convertStringToTime(endTimeTextField.getText(), maximumTime), 0);
            timeFilterEqualStart = RowFilter.numberFilter(RowFilter.ComparisonType.EQUAL,
                    convertStringToTime(startTimeTextField.getText(), minimumTime), 0);
            timeFilterEqualEnd = RowFilter.numberFilter(RowFilter.ComparisonType.EQUAL,
                    convertStringToTime(endTimeTextField.getText(), maximumTime), 0);

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


    private class EventTableBiSliderListener extends BiSliderAdapter {

        @Override
        public void newValues(BiSliderEvent evt) {
            if (!updatingSlider) {
                double newStart = minimumTime + (maximumTime - minimumTime) * (evt.getMinimum() / 100);
                double newEnd = minimumTime + (maximumTime - minimumTime) * (evt.getMaximum() / 100);
                startTimeTextField.setValue(newStart);
                endTimeTextField.setValue(newEnd);

            }
            setupTimeFilter();

            eventsTable.filterTable(getJointFilter(), filterColumns);
        }
    }

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

    private void updateTimeSliderValues() {
        
        double start = 0;
        double end = 100;

        start = convertStringToTime(startTimeTextField.getText(), minimumTime);
        end = convertStringToTime(endTimeTextField.getText(), maximumTime);

        if(start < minimumTime || start > maximumTime) {
            if(start < minimumTime) start = minimumTime;
            if(start > maximumTime) start = maximumTime;
        }
        if(end < minimumTime || end > maximumTime) {
            if(end < minimumTime) end = minimumTime;
            if(end > maximumTime) end = maximumTime;
        }

        if(start > end || end < start) {
            endTimeTextField.setText(startTimeTextField.getText());
            end = start;
        }

        updatingSlider = true;
        double range = maximumTime - minimumTime;
        if(range > 0) {
            timeBiSlilder.setMinimumColoredValue(100*start / maximumTime);
            timeBiSlilder.setMaximumColoredValue(100*end / maximumTime);
        }
        try {
            startTimeTextField.setText(stiNumberFormat.valueToString(start));
        } catch (java.text.ParseException e) {
            startTimeTextField.setText(String.valueOf(minimumTime));
        }
        try {
            endTimeTextField.setText(stiNumberFormat.valueToString(end));
        } catch (java.text.ParseException e) {
            endTimeTextField.setText(String.valueOf(maximumTime));
        }
        updatingSlider = false;
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

    public void setupEventsTable() {

        eventsTable.getModel().setDataVector(new Object[][]{},
                new String[]{
            "Time", "Value", "Device", "Address","Module",
            "Channel", "Name" ,"I/O", "Type", "File", "Line"});

        eventsTable.getModel().setEditableColumns(
                new boolean[] {
            false, false, false, false, false, 
            false, false, false, false, false, false});

        eventsTable.addColumnSelectionPopupMenu();

        eventsTable.getModel().setVisible(3, false);    // Address
        eventsTable.getModel().setVisible(7, false);    // I/O
        eventsTable.getModel().setVisible(8, false);    // Type

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

                            eventsTable.filterTable(
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
        eventsTable = new edu.stanford.atom.sti.client.gui.table.STITable();
        filterPanel = new javax.swing.JPanel();
        filterTextField = new javax.swing.JTextField();
        columnSelectComboBox = new javax.swing.JComboBox();
        resetButton = new javax.swing.JButton();
        deviceComboBox = new javax.swing.JComboBox();
        jPanel1 = new javax.swing.JPanel();
        timeBiSlilder = new com.visutools.nav.bislider.BiSlider();
        endTimeTextField = new javax.swing.JFormattedTextField(new STINumberFormat());
        startTimeTextField = new javax.swing.JFormattedTextField(new STINumberFormat());

        eventPanel.setBorder(javax.swing.BorderFactory.createTitledBorder("Events"));

        eventsTable.setAutoResizeMode(javax.swing.JTable.AUTO_RESIZE_NEXT_COLUMN);
        eventsTable.setColumnSelectionAllowed(true);
        jScrollPane1.setViewportView(eventsTable);
        eventsTable.getColumnModel().getSelectionModel().setSelectionMode(javax.swing.ListSelectionModel.SINGLE_INTERVAL_SELECTION);

        javax.swing.GroupLayout eventPanelLayout = new javax.swing.GroupLayout(eventPanel);
        eventPanel.setLayout(eventPanelLayout);
        eventPanelLayout.setHorizontalGroup(
            eventPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 537, Short.MAX_VALUE)
        );
        eventPanelLayout.setVerticalGroup(
            eventPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 282, Short.MAX_VALUE)
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

        jPanel1.setBorder(javax.swing.BorderFactory.createTitledBorder("Displayed Time Interval"));
        jPanel1.setPreferredSize(new java.awt.Dimension(550, 63));

        timeBiSlilder.setPrecise(true);

        endTimeTextField.setPreferredSize(new java.awt.Dimension(125, 20));
        endTimeTextField.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                endTimeTextFieldActionPerformed(evt);
            }
        });

        startTimeTextField.setPreferredSize(new java.awt.Dimension(125, 20));
        startTimeTextField.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                startTimeTextFieldActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout jPanel1Layout = new javax.swing.GroupLayout(jPanel1);
        jPanel1.setLayout(jPanel1Layout);
        jPanel1Layout.setHorizontalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel1Layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(startTimeTextField, javax.swing.GroupLayout.PREFERRED_SIZE, 130, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(timeBiSlilder, javax.swing.GroupLayout.DEFAULT_SIZE, 245, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(endTimeTextField, javax.swing.GroupLayout.PREFERRED_SIZE, 130, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap())
        );
        jPanel1Layout.setVerticalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.CENTER)
                .addComponent(timeBiSlilder, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addComponent(endTimeTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addComponent(startTimeTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
        );

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(eventPanel, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(filterPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(jPanel1, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 549, Short.MAX_VALUE))
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(eventPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jPanel1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
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
        eventsTable.filterTable(getJointFilter(), filterColumns);
    }
}//GEN-LAST:event_columnSelectComboBoxActionPerformed

private void resetButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_resetButtonActionPerformed
    filterTextField.setText("");
    timeBiSlilder.setMinimumColoredValue(0);
    timeBiSlilder.setMaximumColoredValue(100);
    deviceComboBox.setSelectedIndex(0);
}//GEN-LAST:event_resetButtonActionPerformed

private void deviceComboBoxActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_deviceComboBoxActionPerformed

    if (!filtersInitialized) {
        return;
    }
    if(deviceComboBox.getSelectedIndex() == 0) {
        //All Devices
        deviceFilter = RowFilter.regexFilter("", filterColumns);
        eventsTable.filterTable(getJointFilter(), filterColumns);
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
    eventsTable.filterTable(getJointFilter(), filterColumns);
}//GEN-LAST:event_deviceComboBoxActionPerformed

private void startTimeTextFieldActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_startTimeTextFieldActionPerformed
    updateTimeSliderValues();
}//GEN-LAST:event_startTimeTextFieldActionPerformed

private void endTimeTextFieldActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_endTimeTextFieldActionPerformed
    updateTimeSliderValues();
}//GEN-LAST:event_endTimeTextFieldActionPerformed


    // Variables declaration - do not modify//GEN-BEGIN:variables
    javax.swing.JComboBox columnSelectComboBox;
    javax.swing.JComboBox deviceComboBox;
    javax.swing.JFormattedTextField endTimeTextField;
    javax.swing.JPanel eventPanel;
    edu.stanford.atom.sti.client.gui.table.STITable eventsTable;
    javax.swing.JPanel filterPanel;
    javax.swing.JTextField filterTextField;
    javax.swing.JPanel jPanel1;
    javax.swing.JScrollPane jScrollPane1;
    javax.swing.JButton resetButton;
    javax.swing.JFormattedTextField startTimeTextField;
    com.visutools.nav.bislider.BiSlider timeBiSlilder;
    // End of variables declaration//GEN-END:variables
}
