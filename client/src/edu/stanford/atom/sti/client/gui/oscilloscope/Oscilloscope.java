/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.gui.oscilloscope;

import info.monitorenter.gui.chart.Chart2D;
import info.monitorenter.gui.chart.ITrace2D;
import info.monitorenter.gui.chart.io.ADataCollector;
import info.monitorenter.gui.chart.io.RandomDataCollectorOffset;
import info.monitorenter.gui.chart.traces.Trace2DLtd;

import java.awt.Color;
import java.util.ArrayList;

import edu.stanford.atom.sti.client.comm.bl.device.*;
import edu.stanford.atom.sti.corba.Types.TValMixed;
import edu.stanford.atom.sti.client.comm.bl.TChannelDecode;



/**
 *
 * @author Susannah Dickerson
 */
public class Oscilloscope {

    public Chart2D chart;
    public ArrayList<DeviceMixedDataCollector> collectors = new ArrayList<DeviceMixedDataCollector>();
    private Boolean running = false;
    private long m_starttime;
    private Color currentColor;

    public Oscilloscope() {
        chart = new Chart2D();
        m_starttime = System.currentTimeMillis();
        currentColor = Color.GREEN;
    }

    public void addChannel(Device device, short channelID, TValMixed valueIn) {

        DeviceMixedDataCollector newCollector = new DeviceMixedDataCollector(chart, m_starttime, currentColor, device, channelID, valueIn);
        collectors.add(newCollector);
        currentColor = newCollector.getColor();

        if (running)
            start();
    }

    public void removeAllTraces() {
        chart.removeAllTraces();
        collectors.clear();
        currentColor = Color.GREEN;
    }

    public void resetTimer() {
        m_starttime = System.currentTimeMillis();
    }

    public void start() {
        //Run only if collecters is not empty
        if (!collectors.isEmpty())
        {
            for(DeviceMixedDataCollector item : collectors)
                    item.start();

            running = true;
        }
    }

    public void stop() {
        if (running && !collectors.isEmpty())
        {
            for(DeviceMixedDataCollector item : collectors)
                item.stop();
        }

        running = false;
    }
}
