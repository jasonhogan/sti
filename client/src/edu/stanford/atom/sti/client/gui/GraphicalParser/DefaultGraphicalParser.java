/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.gui.GraphicalParser;

import edu.stanford.atom.sti.client.gui.EventsTab.STIGraphicalParser;
import edu.stanford.atom.sti.corba.Types.TValMixed;
import edu.stanford.atom.sti.corba.Types.TValue;
import java.util.Vector;
import java.util.HashMap;
import info.monitorenter.gui.chart.traces.Trace2DSimple;
/**
 *
 * @author Jason
 */
public class DefaultGraphicalParser implements STIGraphicalParser {

//    double[] endValues = new double[] {};

    private HashMap<Short, double[]> endValues = new HashMap<Short, double[]>();
//    private Vector<double[]> endValues = new Vector<double[]>();

    public int getNumberOfTraces(short channel) {return 1;}
    public void setupEvents(short channel,
            Vector<edu.stanford.atom.sti.client.comm.bl.DataManager.EventChannel.MixedEvent> events, 
            Vector<Trace2DSimple> traces) {
                
        for (int i = 0; i < events.size(); i++) {
            if (i > 0) {
                //holds the last value until next event; less 1 ns for ordering
                addPoint(traces.get(0), (events.get(i).time - 1) / timebase, events.get(i - 1).value);
            }
            addPoint(traces.get(0), events.get(i).time / timebase, events.get(i).value);
        }
       
        Double end = getValue(events.lastElement().value);

        if(end != null) {
            endValues.put(channel, new double[] {end});
      //      endValues.add(new double[] {end});
//            endValues = new double[] { end.doubleValue() };
        }
    }

    public double[] getEndingYValues(short channel) {
        if(endValues != null && endValues.containsKey(channel)) {
            return endValues.get(channel);
        } else {
            return new double[] {};
        }
    }

    public String[] getTraceLabels(short channel) {
        return new String[] {""};
    }
    public String[] getTraceUnits(short channel) {
        return new String[] {""};
    }
    public java.awt.Color[] getTraceColors(short channel) {
        return new java.awt.Color[]
        {java.awt.Color.BLUE};
    }
       private void addPoint(Trace2DSimple trace, double time, TValMixed value) {

        Double value_d = getValue(value);
        if(value_d != null) {
            trace.addPoint(time, value_d.doubleValue());
        }
    }

    private Double getValue(TValMixed value) {
        Double result = null;
        switch (value.discriminator().value()) {
            case TValue._ValueNumber:
                result = new Double(value.number());
                break;
            case TValue._ValueVector:
                if(value.vector() != null && value.vector().length > 0) {
                    result = getValue(value.vector()[0]);   //arbitrarily take the first entry
                }
                break;
            case TValue._ValueString:
                result = 0.0;
                for (int i = 0;
                        value.stringVal() != null && i < value.stringVal().length();
                        i++) {
                    result += value.stringVal().charAt(i);
                }
                result = null;
                break;
            default:
                break;
        }
        return result;
    }
}
