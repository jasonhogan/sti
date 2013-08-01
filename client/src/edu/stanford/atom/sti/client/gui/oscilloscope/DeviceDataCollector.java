/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.gui.oscilloscope;

import info.monitorenter.gui.chart.io.ADataCollector;
import info.monitorenter.gui.chart.*;
import edu.stanford.atom.sti.client.comm.bl.device.*;
import edu.stanford.atom.sti.corba.Types.TValMixed;
import edu.stanford.atom.sti.corba.Types.TDataMixed;

/**
 *
 * @author Susannah Dickerson
 */
public class DeviceDataCollector extends ADataCollector{

    private Device device;
    private short channelID;
    private TValMixed valueIn;
    private long startTime;


    public DeviceDataCollector(final ITrace2D trace, final int latency, long oscStartTime, Device dev, short chan, TValMixed val) {
        super(trace,latency);
        device = dev;
        channelID = chan;
        valueIn = val;
        startTime = oscStartTime;
    }
    
    public ITracePoint2D collectData(){
        TDataMixed data = device.read(channelID, valueIn);
        return new TracePoint2D(((double) System.currentTimeMillis() - this.startTime), data.doubleVal());
    }

}
