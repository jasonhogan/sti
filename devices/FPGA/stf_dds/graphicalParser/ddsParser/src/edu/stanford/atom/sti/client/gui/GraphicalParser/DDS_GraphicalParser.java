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
import edu.stanford.atom.sti.corba.Types.TValMixed;
/**
 *
 * @author Jason
 */
public class DDS_GraphicalParser implements STIGraphicalParser {
   
    private double[] lastValues = new double[] {0,0,0};

    private HashMap<Short, double[]> endValues = new HashMap<Short, double[]>();

    public int getNumberOfTraces(short channel) {return 3;}

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

            if(isDDSTriplet(events.get(i).value)) {
                addDDSTripletPoint(events.get(i).time / timebase, events.get(i).value, traces);

                if(i + 1 < events.size()) {
                    addDDSTripletPoint(events.get(i + 1).time / timebase, events.get(i).value, traces);
                }
            }

            if(isDDSFrequencySweepSimple(events.get(i).value)) {
                addDDSFrequencySweepSimple(events.get(i).time / timebase, events.get(i).value, traces);
            }

            if(isDDSFrequencySweepArbitrary(events.get(i).value)) {
                addDDSFrequencySweepArbitrary(events.get(i).time / timebase, events.get(i).value, traces);
            }
        }
        if(events != null && events.size() > 0 && lastValues != null && lastValues.length == 3) {
            endValues.put(channel, lastValues);
        }
    }


    private boolean isDDSFrequencySweepArbitrary(TValMixed vec) {
        return (vec != null
                && vec.discriminator() == TValue.ValueVector
                && vec.vector().length == 3
                && vec.vector()[0].discriminator() == TValue.ValueVector
                && vec.vector()[1].discriminator() == TValue.ValueNumber
                && vec.vector()[2].discriminator() == TValue.ValueNumber
                && vec.vector()[0].vector().length > 0
                && vec.vector()[0].vector()[0].discriminator() == TValue.ValueVector
                && vec.vector()[0].vector()[0].vector().length == 3);
    }

    private void addDDSFrequencySweepArbitrary(double time, TValMixed vec, Vector<Trace2DSimple> traces) {
        if(isDDSFrequencySweepArbitrary(vec)) {
            TValMixed[] commandList = vec.vector()[0].vector(); //list of sweep points (start, end, time)

            double startFreq = lastValues[0];
            double endFreq = lastValues[0];
            double sweepTime_ns;
            TValMixed[] triplet;

            double currentTime = time;

            //First amplitude and phase points
            traces.get(1).addPoint(currentTime, 100);              //amplitude
            traces.get(2).addPoint(currentTime, lastValues[2]);    //phase

            for(int j = 0; j < commandList.length; j++) {
                triplet = commandList[0].vector();

                startFreq = triplet[0].number();
                endFreq = triplet[1].number();
                sweepTime_ns = triplet[2].number();

                traces.get(0).addPoint(currentTime, startFreq);
                currentTime += sweepTime_ns;
                traces.get(0).addPoint(currentTime, endFreq);
            }
            lastValues[0] = endFreq;

            //Last amplitude and phase points
            traces.get(1).addPoint(currentTime, 100);              //amplitude
            traces.get(2).addPoint(currentTime, lastValues[2]);    //phase
        }
    }


    private boolean isDDSFrequencySweepSimple(TValMixed vec) {
        return (vec != null
                && vec.discriminator() == TValue.ValueVector
                && vec.vector().length == 3
                && vec.vector()[0].discriminator() == TValue.ValueVector
                && vec.vector()[1].discriminator() == TValue.ValueNumber
                && vec.vector()[2].discriminator() == TValue.ValueNumber
                && vec.vector()[0].vector().length == 3
                && vec.vector()[0].vector()[0].discriminator() == TValue.ValueNumber
                && vec.vector()[0].vector()[1].discriminator() == TValue.ValueNumber
                && vec.vector()[0].vector()[2].discriminator() == TValue.ValueNumber);
    }

    private void addDDSFrequencySweepSimple(double time, TValMixed vec, Vector<Trace2DSimple> traces) {
        if(isDDSFrequencySweepSimple(vec)) {
            double startFreq    = vec.vector()[0].vector()[0].number();
            double endFreq      = vec.vector()[0].vector()[1].number();
            double sweepTime_ns = vec.vector()[0].vector()[2].number();

            //First point of sweep
            traces.get(0).addPoint(time, startFreq);
            traces.get(1).addPoint(time, 100);
            traces.get(2).addPoint(time, lastValues[2]);

            lastValues[0] = endFreq;
            lastValues[1] = 100;    //amplitude
            //phase unchanged

            //Last point of sweep
            for (int i = 0; i < 3; i++) {
                traces.get(i).addPoint(time + sweepTime_ns, lastValues[i]);
            }
        }
    }

    private boolean isDDSTriplet(TValMixed vec) {
        return (vec != null 
                && vec.discriminator() == TValue.ValueVector 
                && vec.vector().length == 3 
                && vec.vector()[0].discriminator() == TValue.ValueNumber
                && vec.vector()[1].discriminator() == TValue.ValueNumber
                && vec.vector()[2].discriminator() == TValue.ValueNumber);
    }

    private void addDDSTripletPoint(double time, TValMixed vec, Vector<Trace2DSimple> traces) {
        if(isDDSTriplet(vec)) {
            for (int i = 0; i < 3; i++) {
                lastValues[i] = vec.vector()[i].number();
                traces.get(i).addPoint(time, lastValues[i]);
            }
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
