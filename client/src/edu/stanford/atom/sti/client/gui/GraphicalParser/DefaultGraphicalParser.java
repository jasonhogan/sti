/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.gui.GraphicalParser;

import edu.stanford.atom.sti.client.gui.EventsTab.STIGraphicalParser;
import edu.stanford.atom.sti.corba.Types.TValMixed;
import edu.stanford.atom.sti.corba.Types.TValue;
import java.util.Vector;
import info.monitorenter.gui.chart.traces.Trace2DSimple;
/**
 *
 * @author Jason
 */
public class DefaultGraphicalParser implements STIGraphicalParser {

    double[] endValues = new double[] {};

    public int getNumberOfTraces() {return 1;}
    public void setupEvents(
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
            endValues = new double[] { end.doubleValue() };
        }
    }

    public double[] getEndingYValues() {
        return endValues;
    }

    public String[] getTraceLabels() {
        return new String[] {""};
    }
    public String[] getTraceUnits() {
        return new String[] {""};
    }
    public java.awt.Color[] getTraceColors() {
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
                break;
            default:
                break;
        }
        return result;
    }
}
