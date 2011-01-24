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

import edu.stanford.atom.sti.corba.Types.*;
import edu.stanford.atom.sti.corba.Client_Server.Parser;

import java.util.Vector;
import java.util.HashMap;
import edu.stanford.atom.sti.client.comm.io.ServerConnectionListener;
import edu.stanford.atom.sti.client.comm.io.ServerConnectionEvent;

import edu.stanford.atom.sti.client.comm.io.ParseEventListener;

public class DataManager implements ServerConnectionListener, ParseEventListener {

    private TEvent[] events = null;
    private TChannel[] channels = null;
    private String[] files = null;
    private TOverwritten[] overwritten = null;
    private TVariable[] variables = null;

    private Parser parserRef = null;
    
    private Vector<DataManagerListener> listeners = new Vector<DataManagerListener>();
        
    public DataManager() {
        
    }
    
    public void handleEvent(edu.stanford.atom.sti.corba.Pusher.TParseEvent event) {
        getParsedData();
    }

    public synchronized void addDataListener(DataManagerListener listener) {
        listeners.add(listener);
    }
    public synchronized void removeDataListener(DataManagerListener listener) {
        listeners.remove(listener);
    }
    private synchronized void fireNewParsedDataEvent() {
        DataManagerEvent event = new DataManagerEvent(this);
        
        for(int i = 0; i < listeners.size(); i++) {
            listeners.elementAt(i).getData( event );
        }
    }

    public void installServants(ServerConnectionEvent event) {
        setParser(event.getServerConnection().getParser());
    }
    public void uninstallServants(ServerConnectionEvent event) {
        setParser(null);
        fireNewParsedDataEvent();
    }
    
    private void setParser(Parser parser) {
        parserRef = parser;
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

                success = true;
            }
        } catch (Exception e) {
            success = false;
        }

        if(success) {
            fireNewParsedDataEvent();
        }
        else {
            events = null;
            channels = null;
            files = null;
            overwritten = null;
            variables = null;
        }
    }

    public Vector< Vector<Object> > getVariablesTableData() {
        //{"Name", "Value", "Type", "File", "Line"}
        Vector< Vector<Object> > variablesData = null;
        VariablesTableRow rowData = new VariablesTableRow();

        
        if(variables != null && files != null) {
            variablesData = new Vector< Vector<Object> >(variables.length);
            
            int fileNumber = -1;
            String fileName = "";
            TVarMixedDecode varDecode = null;
            
            for(int i = 0; i < variables.length; i++) {
                rowData.clear();
                
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

                variablesData.addElement(rowData.getRow());
            }
        }
        return variablesData;
    }
    
    public Vector< Vector<Object> > getOverwrittenTableData() {
        Vector< Vector<Object> > overwrittenData = null;
        OverwrittenTableRow rowData = new OverwrittenTableRow();
        
        if(overwritten != null) {
            overwrittenData = new Vector< Vector<Object> >(overwritten.length);
            
            for(int i = 0; i < overwritten.length; i++) {
                rowData.clear();
                
                rowData.setName(overwritten[i].name);
                rowData.setValue(overwritten[i].value);
                
                overwrittenData.addElement(rowData.getRow());
            }
        }
        return overwrittenData;
    }

    public class EventChannel {

        public short channel;
        public short module;
        public String deviceName;
        public TChannelType ioType;
        public TValue outputType;
        public TData inputType;
        public TDevice tDevice;

        private Vector<MixedEvent> events = new Vector<MixedEvent>();

        public EventChannel(TChannel channel, TEvent event) {
            this.channel = channel.channel;
            module = channel.device.moduleNum;
            deviceName = channel.device.deviceName;
            ioType = channel.type;
            outputType = channel.outputType;
            inputType = channel.inputType;
            tDevice = channel.device;

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
        for (int i = 0; i < events.length; i++) {
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
        
    public Vector< Vector<Object> > getEventTableData() {
        
        Vector< Vector<Object> > eventData = null;
        EventTableRow rowData = new EventTableRow();
        
        if(events != null && channels != null && files != null) {
            
            eventData = new Vector< Vector<Object> >(events.length);
            
            int fileNumber = -1;
            int channelNumber = -1;
            TChannel tempChannel = null;
            String fileName = "";
            TChannelDecode channelDecode = null;
            TValMixedDecode valueDecode = null;

            for (int i = 0; i < events.length; i++) {
                
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

                // the parser-assigned channel number
                channelNumber = events[i].channel;

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
                rowData.setFile(fileName);
                // File Line
                rowData.setLine(events[i].pos.line);
                
                // Add row
                eventData.addElement(rowData.getRow());
            }
        }
        return eventData;
    }
    
    private class VariablesTableRow {
        
        Object[] rowData = new Object[10];
        
        public void clear() {
            rowData = new Object[5];
        }
        public Vector<Object> getRow() {
            Vector<Object> row = new Vector<Object>(10);
            for(int i=0; i< rowData.length; i++) {
                row.addElement(rowData[i]);
            }
            return row;
        }
        public void setName(String name) {
            rowData[0] = name;
        }
        public void setValue(String value) {
            rowData[1] = value;
        }
        public void setType(String type) {
            rowData[2] = type;
        }
        public void setFile(String file) {
            rowData[3] = file;
        }
        public void setLine(int line) {
            rowData[4] = line;
        }
    }
    
    private class OverwrittenTableRow {
        String[] rowData = new String[2];
        
        public void clear() {
            rowData = new String[2];
        }
        public Vector<Object> getRow() {
            Vector<Object> row = new Vector<Object>(10);
            for(int i=0; i< rowData.length; i++) {
                row.addElement(rowData[i]);
            }
            return row;
        }
        public void setName(String name) {
            rowData[0] = name;
        }
        public void setValue(String value) {
            rowData[1] = value;
        }
    }
    
    private class EventTableRow {
        
        //   {"Time", "Value", "Device", "Address","Module",
        //    "Channel", "I/O", "Type", "File", "Line"}
        
        Object[] rowData = new Object[10];
        
        public EventTableRow() {
        }
        public void clear() {
            rowData = new Object[10];
        }
        public Vector<Object> getRow() {
            Vector<Object> row = new Vector<Object>(10);
            for(int i=0; i< rowData.length; i++) {
                row.addElement(rowData[i]);
            }
            return row;
        }
        public void setTime(double time) {
            rowData[0] = time;
        }
        public void setValue(String value) {
            rowData[1] = value;
        }
        public void setDevice(String device) {
            rowData[2] = device;
        }
        public void setAddress(String address) {
            rowData[3] = address;
        }
        public void setModule(short module) {
            rowData[4] = module;
        }
        public void setChannel(short channel) {
            rowData[5] = channel;
        }
        public void setIO(String io) {
            rowData[6] = io;
        }
        public void setType(String type) {
            rowData[7] = type;
        }
        public void setFile(String file) {
            rowData[8] = file;
        }
        public void setLine(int line) {
            rowData[9] = line;
        }
    }
    
    
}
