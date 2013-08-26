/** @file DataManagerEvent.java
 *  @author Jason Michael Hogan
 *  @brief Source-file for the class DataManagerEvent
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

import edu.stanford.atom.sti.client.comm.bl.DataManager.EventChannel;
import edu.stanford.atom.sti.client.gui.FileEditorTab.TextTag;
import edu.stanford.atom.sti.corba.Pusher.ParseEventType;
import java.util.EventObject;
import java.util.HashMap;
import java.util.Vector;

public class DataManagerEvent extends EventObject {
    
    private ParseEventType parseEventType = null;

    public DataManagerEvent(Object source) {
        this(source, ParseEventType.ParseTimingFile);
    }
    
    public DataManagerEvent(Object source, ParseEventType type) {
        super(source);
        parseEventType = type;
    }


    public ParseEventType getParseEventType() {
        return parseEventType;
    }

    public Vector< TextTag > getTagData() {
        return ( (DataManager)getSource() ).getTags();
    }
    
    public Vector< DataManager.VariablesTableRow > getVariablesTableData() {
        return ( (DataManager)getSource() ).getVariablesTableData();
    }

    public Vector< DataManager.OverwrittenTableRow > getOverwrittenTableData() {
        return ( (DataManager)getSource() ).getOverwrittenTableData();
    }

    public Vector< DataManager.EventTableRow > getEventTableData() {
        return ( (DataManager)getSource() ).getEventTableData();
    }
//    public Vector< DataManager.EventTableRow > getEventTableRowData() {
//        return ( (DataManager)getSource() ).getEventTableRowData();
//    }
    public HashMap<Integer, DataManager.EventChannel> getEventsByChannel() {
        return ( (DataManager)getSource() ).getEventsByChannel();
    }
    public HashMap<Integer, DataManager.StateTableRow> getStateTableData() {
        return ( (DataManager)getSource() ).getStateTableData();
    }
}
