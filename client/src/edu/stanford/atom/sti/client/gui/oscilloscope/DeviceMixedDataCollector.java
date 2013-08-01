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
import edu.stanford.atom.sti.corba.Types.TData;
import edu.stanford.atom.sti.client.comm.bl.TChannelDecode;
import info.monitorenter.gui.chart.traces.Trace2DLtd;

import edu.stanford.atom.sti.client.gui.oscilloscope.DeviceDataCollector;

import java.awt.Color;
import java.util.ArrayList;

/**
 *
 * @author Susannah Dickerson
 */
public class DeviceMixedDataCollector {
    private Device device;
    private short channelID;
    private TValMixed valueIn;
    private long startTime;
    private ArrayList<DeviceDataCollector> collectors = new ArrayList<DeviceDataCollector>();
    private Color currentColor;
    private int traceLength = 200;
    private int latency = 100;


    public DeviceMixedDataCollector(Chart2D chart, long oscStartTime, Color color, Device dev, short chan, TValMixed val) {
        device = dev;
        channelID = chan;
        valueIn = val;
        startTime = oscStartTime;
        currentColor = color;

        TDataMixed data = device.read(chan, valueIn);
        addTraces(chart, data);
    }

    public Color getColor(){
        return currentColor;
    }

    public void start() {
        if (collectors.isEmpty())
            return;

        for (DeviceDataCollector item : collectors)
            if (!item.isRunning())
                item.start();
    }

    public void stop() {
        if (collectors.isEmpty())
                return;

        for (DeviceDataCollector item : collectors)
            if (item.isRunning())
                item.stop();
    }

    public boolean isRunning()
    {
        boolean running = true;

        if (collectors.isEmpty())
            return false;

        for (DeviceDataCollector item : collectors)
        {
            running &= item.isRunning();
        }

        return running;

    }

    private Color nextColor()
    {
        if (currentColor.equals(Color.BLUE))
            currentColor = Color.MAGENTA;
        else if (currentColor.equals(Color.MAGENTA))
            currentColor = Color.YELLOW;
        else if (currentColor.equals(Color.YELLOW))
            currentColor = Color.GREEN;
        else
            currentColor = Color.BLUE;

        return currentColor;
    }

    private void addTraces(Chart2D chart, TDataMixed data) {
        TData tData = data.discriminator();
        ITrace2D trace;
        DeviceDataCollector newCollector;


        switch (tData.value()) {
            case TData._DataLong:
                trace = new Trace2DLtd(traceLength);
                trace.setColor(nextColor());
                chart.addTrace(trace);
                newCollector = new DeviceDataCollector(trace, latency, startTime, device, channelID, valueIn);

                collectors.add(newCollector);
                break;
            case TData._DataDouble:
                trace = new Trace2DLtd(traceLength);
                trace.setColor(nextColor());

                chart.addTrace(trace);
                newCollector = new DeviceDataCollector(trace, latency, startTime, device, channelID, valueIn);

                collectors.add(newCollector);
                break;
            case TData._DataVector:
                TDataMixed[] mixedVector = data.vector();
                for (TDataMixed item : mixedVector)
                {
                    addTraces(chart, item);
                }
                break;
            default:
                //Don't add traces for other types
                break;
        }

    }
}
