/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.gui.EventsTab;

import java.util.Vector;
import info.monitorenter.gui.chart.traces.Trace2DSimple;
import info.monitorenter.gui.chart.traces.ATrace2D;
import edu.stanford.atom.sti.client.comm.bl.DataManager.EventChannel.MixedEvent;
import java.awt.Color;

/**
 *
 * @author Jason
 */
public interface STIGraphicalParser {

    public static final double timebase = 1000000000;   //(time is in ns)

    public int getNumberOfTraces();
    public void setupEvents(
            Vector<edu.stanford.atom.sti.client.comm.bl.DataManager.EventChannel.MixedEvent> events,
            Vector<Trace2DSimple> traces);
    public double[] getEndingYValues();
    public String[] getTraceLabels();
    public String[] getTraceUnits();
    public java.awt.Color[] getTraceColors();
    
//    public Points[] parseEvents(
//            Vector<edu.stanford.atom.sti.client.comm.bl.DataManager.EventChannel.MixedEvent> events);
//
//    public class Points {
//        private java.util.Vector<Point> points = new java.util.Vector<Point>();
//        public void addPoint(Point point) {
//            points.add(point);
//        }
//        public void addPoint(double time, double value) {
//            points.add(new Point(time, value));
//        }
//        public java.util.Vector<Point> getPoints() {
//            return points;
//        }
//        public void clear() {
//            points.clear();
//        }
//    }
//
//    public class Point {
//        public double time;
//        public double value;
//        public Point(double time, double value) {
//            setPoint(time, value);
//        }
//        public void setPoint(double time, double value) {
//            this.time = time;
//            this.value = value;
//        }
//    }
}
