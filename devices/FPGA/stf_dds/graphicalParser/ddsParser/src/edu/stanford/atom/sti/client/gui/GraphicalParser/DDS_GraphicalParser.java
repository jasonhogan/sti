/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.gui.GraphicalParser;

import edu.stanford.atom.sti.client.gui.EventsTab.STIGraphicalParser;
import java.util.Vector;
import info.monitorenter.gui.chart.traces.Trace2DSimple;
import edu.stanford.atom.sti.corba.Types.TValue;
import java.util.HashMap;
/**
 *
 * @author Jason
 */
public class DDS_GraphicalParser implements STIGraphicalParser {
    //private double[] endValues = new double[] {};
    private HashMap<Short, double[]> endValues = new HashMap<Short, double[]>();

    public int getNumberOfTraces2(short channel) {return 3;}

    public String[] getTraceLabels(short channel) {
        return new String[] {"Frequency","Amplitude","Phase"};
    }
    public String[] getTraceUnits(short channel) {
        return new String[] {"MHz", "%", "Deg"};
    }
    public java.awt.Color[] getTraceColors(short channel) {
        return new java.awt.Color[] 
        {java.awt.Color.BLUE, java.awt.Color.RED, java.awt.Color.ORANGE};
    }
    public void setupEvents(short channel,
            Vector<edu.stanford.atom.sti.client.comm.bl.DataManager.EventChannel.MixedEvent> events,
            Vector<Trace2DSimple> traces) {

        for (int i = 0; i < events.size(); i++) {
            if (events.get(i).value.discriminator() == TValue.ValueVector) {
                if (i > 0) {
                    traces.get(0).addPoint(events.get(i - 1).time / timebase, events.get(i).value.vector()[0].number());
                    traces.get(1).addPoint(events.get(i - 1).time / timebase, events.get(i).value.vector()[1].number());
                    traces.get(2).addPoint(events.get(i - 1).time / timebase, events.get(i).value.vector()[2].number());
                }
                traces.get(0).addPoint(events.get(i).time / timebase, events.get(i).value.vector()[0].number());
                traces.get(1).addPoint(events.get(i).time / timebase, events.get(i).value.vector()[1].number());
                traces.get(2).addPoint(events.get(i).time / timebase, events.get(i).value.vector()[2].number());
            }
        }
        if(events != null && events.size() > 0) {
            endValues.put(channel, new double[]{
                    events.lastElement().value.vector()[0].number(),
                    events.lastElement().value.vector()[1].number(),
                    events.lastElement().value.vector()[2].number()});
        }
    }

    public double[] getEndingYValues(short channel) {
        if(endValues != null && endValues.containsKey(channel)) {
            return endValues.get(channel);
        } else {
            return new double[] {};
        }
    }
}
