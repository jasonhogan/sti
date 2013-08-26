/** @file DataManager.java
 *  @author Jason Michael Hogan
 *  @brief Source-file for the class DataManager
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

package edu.stanford.atom.sti.client.comm.bl;

import edu.stanford.atom.sti.client.comm.io.ParseEventListener;
import edu.stanford.atom.sti.client.comm.io.ServerConnectionEvent;
import edu.stanford.atom.sti.client.comm.io.ServerConnectionListener;
import edu.stanford.atom.sti.client.gui.FileEditorTab.TextTag;
import edu.stanford.atom.sti.corba.Client_Server.Parser;
import edu.stanford.atom.sti.corba.Pusher.ParseEventType;
import edu.stanford.atom.sti.corba.Types.*;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.Vector;

public class DataManager implements ServerConnectionListener, ParseEventListener {

    private TEvent[] events = null;
    private TChannel[] channels = null;
    private String[] files = null;
    private TOverwritten[] overwritten = null;
    private TVariable[] variables = null;
    private TTag[] tags = null;

    private Parser parserRef = null;
    
    private Vector<DataManagerListener> listeners = new Vector<DataManagerListener>();
    
    //cached results
//    private Vector< Vector<Object> > eventTableData = null;
    private Vector< EventTableRow > eventTableRowData = null;        
    private boolean eventDataIsUpToDate = false;
    
    Vector<OverwrittenTableRow> overwrittenData = null;
        private boolean overwrittenDataIsUpToDate = false;
    
    public DataManager() {
        
    }
    
    public void handleEvent(edu.stanford.atom.sti.corba.Pusher.TParseEvent event) {
        if (event.type == ParseEventType.ParseTimingFile) {
            getParsedData();
        } else if (event.type == ParseEventType.RefreshOverwrittenVars) {
            getOverwrittenVars();
        }

//        getParsedData();
    }

    public synchronized void addDataListener(DataManagerListener listener) {
        listeners.add(listener);
    }
    public synchronized void removeDataListener(DataManagerListener listener) {
        listeners.remove(listener);
    }
    private synchronized void fireNewParsedDataEvent(ParseEventType type) {
        DataManagerEvent event = new DataManagerEvent(this, type);
        
        for(int i = 0; i < listeners.size(); i++) {
            listeners.elementAt(i).getData( event );
        }
    }

    public void installServants(ServerConnectionEvent event) {
        setParser(event.getServerConnection().getParser());
    }
    public void uninstallServants(ServerConnectionEvent event) {
        setParser(null);
        fireNewParsedDataEvent(ParseEventType.ParseTimingFile);
    }
    
    private void setParser(Parser parser) {
        parserRef = parser;
    }

    private void getOverwrittenVars() {
        boolean success = false;

        try {
            if(parserRef == null || parserRef._non_existent()) {
                return;
            }
        } catch (Exception e) {
            parserRef = null;
        }

        try {
            if (parserRef != null) {
                overwritten = parserRef.overwritten();
                success = true;
            }
        } catch (Exception e) {
            success = false;
        }

        if(success) {
            fireNewParsedDataEvent(ParseEventType.RefreshOverwrittenVars);
        }
        else {
            events = null;
            channels = null;
            files = null;
            overwritten = null;
            variables = null;
            tags = null;
        }

    }
    private void getParsedData() {
       
        boolean success = false;
        
        try {
            if(parserRef == null || parserRef._non_existent()) {
                return;
            }
        } catch (Exception e) {
            parserRef = null;
        }

        try {
            if (parserRef != null) {
                events = parserRef.events();
                channels = parserRef.channels();
                files = parserRef.files();
                overwritten = parserRef.overwritten();
                variables = parserRef.variables();
                tags = parserRef.tags();

                success = true;
                eventDataIsUpToDate = false;
                overwrittenDataIsUpToDate = false;
            }
        } catch (Exception e) {
            success = false;
        }

        if(success) {
            fireNewParsedDataEvent(ParseEventType.ParseTimingFile);
        }
        else {
            events = null;
            channels = null;
            files = null;
            overwritten = null;
            variables = null;
            tags = null;
        }
    }

    public Vector<TextTag> getTags() {
        Vector< TextTag> tagVec = new Vector< TextTag>();
        
        if (tags != null && files != null && files.length > 0) {

            for (int i = 0; i < tags.length; i++) {
                if (tags[i].pos.file < files.length) {
                    tagVec.addElement(new TextTag(
                            tags[i],
                            files[tags[i].pos.file],
                            files[0]));
                }
            }
        }
        System.out.println("# Tags: " + tagVec.size());
        return tagVec;
    }

    public Vector < String > getVariableNames() {
        Vector < String > vars = new Vector < String >();
        if(variables != null) {
            for(int i = 0; i < variables.length; i++) {
                vars.addElement(variables[i].name);
            }
        }
        return vars;
    }

    public Vector<VariablesTableRow> getVariablesTableData() {
        //{"Name", "Value", "Type", "File", "Line"}
        Vector<VariablesTableRow> variablesData = null;
        VariablesTableRow rowData;
        
        if(variables != null && files != null && overwritten != null) {
            variablesData = new Vector<VariablesTableRow>(variables.length);
            
            int fileNumber = -1;
            String fileName = "";
            TVarMixedDecode varDecode = null;
            
            for(int i = 0; i < variables.length; i++) {
                rowData = new VariablesTableRow();
               
                fileNumber = -1;
                fileName = "";
                varDecode = null;
                
                rowData.setName(variables[i].name);
                varDecode = TVarMixedDecode.
                        createTVarMixedDecode(variables[i].value, channels);

                rowData.setValue(varDecode.getValue());
                rowData.setType(varDecode.getType());
                
                // the parser-assigned file number
                fileNumber = variables[i].pos.file;

                if (fileNumber < files.length && fileNumber >= 0) {
                    fileName = files[fileNumber];
                }

                rowData.setFile(fileName);
                rowData.setLine(variables[i].pos.line);
            
                variablesData.addElement(rowData);
            }
            getOverwrittenTableData();
            
            //Replace overwritten variables in variablesData
            for (OverwrittenTableRow overRow : overwrittenData) {
                for (VariablesTableRow varRow : variablesData) {
                    if (overRow.getName().compareTo(varRow.getName()) == 0) {
                        //Found a variable in the overwritten table.
                        //Modify this entry in the variable table.
                        varRow.setValue(overRow.getValue());
                    }
                }
            }
        }
        return variablesData;
    }
    
    public synchronized Vector<OverwrittenTableRow> getOverwrittenTableData() {
        overwrittenDataIsUpToDate = false;  //bypass queueing for now...
        if(!overwrittenDataIsUpToDate) {
            overwrittenDataIsUpToDate = setupOverwrittenTableData();
        }
        return overwrittenData;
    }
    
    private boolean setupOverwrittenTableData() {

        OverwrittenTableRow rowData;
        
        overwrittenData = null;

        if (overwritten != null) {
            overwrittenData = new Vector<OverwrittenTableRow>(overwritten.length);

            for (int i = 0; i < overwritten.length; i++) {
                rowData = new OverwrittenTableRow();
                
                rowData.setName(overwritten[i].name);
                rowData.setValue(overwritten[i].value);
                
                overwrittenData.addElement(rowData);
            }
        }
        return (overwrittenData != null);
    }

    public class EventChannel {

        public short channel;
        public short module;
        public String deviceName;
        public TChannelType ioType;
        public TValue outputType;
        public TData inputType;
        public TDevice tDevice;
        public String channelName;

        private Vector<MixedEvent> events = new Vector<MixedEvent>();

        public EventChannel(TChannel channel, TEvent event) {
            this.channel = channel.channel;
            module = channel.device.moduleNum;
            deviceName = channel.device.deviceName;
            ioType = channel.type;
            outputType = channel.outputType;
            inputType = channel.inputType;
            tDevice = channel.device;
            channelName = channel.channelName;

            addEvent(event);
        }

        public Vector<MixedEvent> getEvents() {
            return events;
        }
        
        public double getMaxTime() {
            if(events != null && events.size() > 0) {
                return events.lastElement().time;
            } else {
                return 0;
            }
        }
        public double getMinTime() {
            if(events != null && events.size() > 0) {
                return events.firstElement().time;
            } else {
                return 0;
            }
        }

        public void addEndEvent(double time) {
            if(events != null && events.size() > 0) {
                events.add(new MixedEvent(time, events.lastElement().value));
            }
        }

        public void addEvent(TEvent event) {
            if(events.size() != 0 && events.lastElement().time > event.time) {
                for(int i = 0; i < events.size(); i++) {
                    if(events.get(i).time > event.time) {
                        events.insertElementAt(new MixedEvent(event.time, event.value), i);
                        return;
                    }
                }
            }
            events.add(new MixedEvent(event.time, event.value));
        }

        public class MixedEvent {
            public double time;
            public TValMixed value;
            public MixedEvent(double Time, TValMixed Value) {
                time = Time;
                value = Value;
            }
        }
    }


    public HashMap<Integer, EventChannel> getEventsByChannel() {
        HashMap<Integer, EventChannel> eventData = new HashMap<Integer, EventChannel>();

        EventChannel eventChannel;
        int ch;
        for (int i = 0; (events != null && i < events.length); i++) {
            ch = Integer.valueOf(events[i].channel);
            eventChannel = eventData.get( ch );

            if(eventChannel == null) {
                eventData.put(ch, new EventChannel(channels[ch], events[i]));
            } else {
                eventChannel.addEvent(events[i]);
            }
        }
        return eventData;
    }
    
    public HashMap<Integer, StateTableRow> getStateTableData() {
        HashMap<Integer, StateTableRow> stateTableData = new HashMap<Integer, StateTableRow>();

        StateTableRow row;
        for(int i = 0; (channels != null && i < channels.length); i++) {
            row = new StateTableRow(channels[i]);
            stateTableData.put(i, row);
        }

        return stateTableData;
    }

    
    public synchronized Vector< EventTableRow > getEventTableData() {
        if(!eventDataIsUpToDate) {
            eventDataIsUpToDate = setupEventTableData();
        }
        return eventTableRowData;
    }

    private boolean setupEventTableData() {
        
        EventTableRow rowData = null;

        if(events != null && channels != null && files != null) {
            
//            eventTableData = new Vector< Vector<Object> >(events.length);
            eventTableRowData = new Vector<EventTableRow>(events.length);
            
            int fileNumber = -1;
            int channelNumber = -1;
            TChannel tempChannel = null;
            String fileName = "";
            TChannelDecode channelDecode = null;
            TValMixedDecode valueDecode = null;

            for (int i = 0; i < events.length; i++) {
                
                //Construct a new row.
                eventTableRowData.addElement( new EventTableRow() );
                rowData = eventTableRowData.lastElement();

                fileNumber = -1;
                channelNumber = -1;
                tempChannel = null;
                fileName = "";
                channelDecode = null;
                valueDecode = null;

                rowData.clear();

                //Time
                rowData.setTime(events[i].time);
                //Value
          //      edu.stanford.atom.sti.device.comm.corba.TValMixed temp = events[i].value;
                valueDecode = new TValMixedDecode(events[i].value);
                rowData.setValue( valueDecode.toString() );

                // the parser-assigned unique channel number
                channelNumber = events[i].channel;
                rowData.setParserChannelNumber(channelNumber);

                if (channelNumber < channels.length && channelNumber >= 0) {
                    tempChannel = channels[channelNumber];
                }

                if (tempChannel != null) {
                    //Device
                    rowData.setDevice(tempChannel.device.deviceName);
                    //Address
                    rowData.setAddress(tempChannel.device.address);
                    //Module
                    rowData.setModule(tempChannel.device.moduleNum);
                    //Channel
                    rowData.setChannel(tempChannel.channel);
                    //Channel Name
                    rowData.setName(tempChannel.channelName);

                    channelDecode = new TChannelDecode(tempChannel);

                    //IO Type
                    rowData.setIO(channelDecode.IOType());
                    //Format Type
                    rowData.setType(channelDecode.ChannelType());

                }

                // the parser-assigned file number
                fileNumber = events[i].pos.file;

                if (fileNumber < files.length && fileNumber >= 0) {
                    fileName = files[fileNumber];
                }
                // File Name
                String[] path = fileName.split("\\\\");
                rowData.setFile(path[path.length - 1]);
                // File Line
                rowData.setLine(events[i].pos.line);

            }
            
            //sort by time
            Collections.sort(eventTableRowData, new Comparator<EventTableRow>() {
                public int compare(EventTableRow left, EventTableRow right) {
                    return Double.compare(left.getTime(), right.getTime());
//                    return left.compareToIgnoreCase(right);
                }
            });
            return true;
        }
        return false;
    }
    
    private class TableRow extends Vector {
        private final int cols;

//        Object[] rowData = null;
        
        public TableRow(int columns) {
            clear();
            cols = columns;
            super.setSize(cols);
            
//            for(int i = 0; i < cols; i++) {
//                add(new Object());
//            }
        }
        public void clear() {
            super.clear();
            super.setSize(cols);
//            rowData = new Object[cols];
        }
//        public Vector<Object> getRow() {
//            Vector<Object> row = new Vector<Object>(cols);
//            for(int i = 0; i < rowData.length; i++) {
//                row.addElement(rowData[i]);
//            }
//            return row;
//        }
//        public Object[] getRowData() {
//            return rowData;
//        }
    }
    
    public class VariablesTableRow extends TableRow {

        VariablesTableRow() {
            super(5);
        }
        public void setName(String name) {
            set(0, name);
//            rowData[0] = name;
        }
        public String getName() {
            return (String) get(0);
        }
        public void setValue(String value) {
            set(1, value);
//            rowData[1] = value;
        }
        public void setType(String type) {
            set(2, type);
//            rowData[2] = type;
        }
        public void setFile(String file) {
            set(3, file);
//            rowData[3] = file;
        }
        public void setLine(int line) {
            set(4, line);
//            rowData[4] = line;
        }
    }
    
    public class OverwrittenTableRow extends TableRow {
        
        OverwrittenTableRow() {
            super(2);
        }
        public void setName(String name) {
            set(0, name);
           // rowData[0] = name;
        }
        public void setValue(String value) {
            set(1, value);
//            rowData[1] = value;
        }
        public String getName() {
            return (String) get(0);
        }
        public String getValue() {
            return (String) get(1);
        }
    }
    
    public class EventTableRow extends TableRow {
        
        //   {"Time", "Value", "Device", "Address","Module",
        //    "Channel", "I/O", "Type", "File", "Line"}

        private int parserChannelNumber;

        public EventTableRow() {
            super(11);  //number of columns
        }
        
        public void setParserChannelNumber(int parserChannelNum) {
            parserChannelNumber = parserChannelNum;
        }
        public int getParserChannelNumber() {
            return parserChannelNumber;
        }

        public double getTime() {
            return ((Double) get(0));
        }
        public void setTime(double time) {
            set(0, time);
//            rowData[0] = time;
        }
        public String getValue() {
            return (String) get(1);
        }
        public void setValue(String value) {
            set(1, value);
//            rowData[1] = value;
        }
        public void setDevice(String device) {
            set(2, device);
//            rowData[2] = device;
        }
        public void setAddress(String address) {
            set(3, address);
//            rowData[3] = address;
        }
        public void setModule(short module) {
            set(4, module);
//            rowData[4] = module;
        }
        public void setChannel(short channel) {
            set(5, channel);
//            rowData[5] = channel;
        }
        public void setName(String name) {
            set(6, name);
//            rowData[6] = name;
        }
        public void setIO(String io) {
            set(7, io);
//            rowData[7] = io;
        }
        public void setType(String type) {
            set(8, type);
//            rowData[8] = type;
        }
        public void setFile(String file) {
            set(9, file);
//            rowData[9] = file;
        }
        public void setLine(int line) {
            set(10, line);
//            rowData[10] = line;
        }
    }

    public class StateTableRow extends TableRow {
        
        // "Name", "Device", "Address", "Module", "Channel", "Value", "Time at State", "Last Value"
        private boolean timeInitialized = false;
        private boolean lastValueInitialized = false;

        public StateTableRow(TChannel channel) {
            super(8);

            reset();

            setName(channel.channelName);
            setDevice(channel.device.deviceName);
            setAddress(channel.device.address);
            setModule(channel.device.moduleNum);
            setChannel(channel.channel);
        }
        public void reset() {
            setLastValue("Undefined");
            setValue("Undefined");
            setTimeAtState(0);

            lastValueInitialized = false;
            timeInitialized = false;
        }
        public final void setName(String name) {
            set(0, name);
//            rowData[0] = name;
        }
        public void setDevice(String device) {
            set(1, device);
//            rowData[1] = device;
        }
        public void setAddress(String address) {
            set(2, address);
//            rowData[2] = address;
        }
        public void setModule(short module) {
            set(3, module);
//            rowData[3] = module;
        }
        public void setChannel(short channel) {
            set(4, channel);
//            rowData[4] = channel;
        }
        public void setValue(Object value) {
            set(5, value);
//            rowData[5] = value;
        }
        public void setTimeAtState(double time) {
            timeInitialized = true;
            set(6, time);
//            rowData[6] = time;
        }
        public void setLastValue(Object value) {
            lastValueInitialized = true;
            set(7, value);
//            rowData[7] = value;
        }
        public boolean timeAtStateInitialized() {
            return timeInitialized;
        }
        public boolean lastValueInitialized() {
            return lastValueInitialized;
        }
    }
    
}
