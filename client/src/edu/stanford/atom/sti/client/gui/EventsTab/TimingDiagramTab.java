/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * TimingDiagramTab.java
 *
 * Created on Jan 2, 2011, 8:53:40 PM
 */

package edu.stanford.atom.sti.client.gui.EventsTab;


import javax.swing.table.DefaultTableModel;
import javax.swing.event.TableModelEvent;
import javax.swing.event.TableModelListener;

import com.visutools.nav.bislider.BiSlider;
import com.visutools.nav.bislider.BiSliderAdapter;
import com.visutools.nav.bislider.BiSliderEvent;

import info.monitorenter.gui.chart.Chart2D;
import info.monitorenter.gui.chart.ITrace2D;
import info.monitorenter.gui.chart.traces.Trace2DLtd;
import java.awt.Color;
import info.monitorenter.gui.chart.traces.Trace2DSimple;
import info.monitorenter.util.Range;
import info.monitorenter.gui.chart.rangepolicies.RangePolicyFixedViewport;


import info.monitorenter.gui.chart.ZoomableChart;
import java.util.Vector;

import edu.stanford.atom.sti.client.gui.table.JChart2DCellRenderer;
import edu.stanford.atom.sti.client.gui.table.JChart2DCellEditor;

import edu.stanford.atom.sti.client.comm.bl.*;

import java.util.HashMap;
import edu.stanford.atom.sti.client.comm.bl.DataManager.EventChannel;
import edu.stanford.atom.sti.client.comm.bl.DataManager.EventChannel.MixedEvent;
import edu.stanford.atom.sti.corba.Types.TValue;
import edu.stanford.atom.sti.corba.Types.TValMixed;
import edu.stanford.atom.sti.client.gui.VisibleTabListener;
import java.util.Hashtable;
import edu.stanford.atom.sti.client.comm.bl.device.DeviceCollection;
import edu.stanford.atom.sti.client.comm.bl.device.Device;
/**
 *
 * @author Jason
 */
public class TimingDiagramTab extends javax.swing.JPanel implements DataManagerListener, VisibleTabListener {
    
    private DeviceCollection deviceCollection = null;

    private final int chartColumn = 4;
    private final double chartEndTimePaddingFration = 0.05;
//    private final int numberOfSegments = 20;
    private final int fineSliderSegments = 20;
    private final int coarseSliderSegments = 20;
    
    private JChart2DCellRenderer chartRenderer = new JChart2DCellRenderer();
    private JChart2DCellEditor chartEditor = new JChart2DCellEditor();

    private Vector<String> columnHeaders = new Vector(java.util.Arrays.asList(
            new String[]{
                "Row", "Device", "Channel", "Name", "Timing Diagram"
            }));
    private Vector<ChannelTimingDiagram> diagrams = new Vector<ChannelTimingDiagram>();

    private final int maxZoomSlider = 100000000;
    private final int minZoomSlider = 1;

    double logScale = 1;
    double prescaler = 1;       //fine range = prescaler * 10^(logScale * logZoomSlider.getValue())

    double minZoomedRange = 0.000000001 * 100; // 100 ns -- The min fine slider range
    double maxZoomedRange = 1;

    double fineRangeOffset = 5;
    double fineRangeScale = 1;

    double coarseRangeScale = 1;

    double minSizeCoarseSlider = 0.1;
    double coarseSliderMidpoint = 0.5;
    double coarseSliderMinFraction = 0.03;
    double coarseSliderSize = 1;

    enum SIScale {Giga, Mega, Kilo, One, milli, micro, nano};

    private SIScale fineRangeSIScale = SIScale.One;
    private SIScale coarseRangeSIScale = SIScale.One;

    enum ZoomMode {XY, Horizontal, Vertical};
    private ZoomMode zoomMode = ZoomMode.Horizontal;    //the global zoom mode for all charts

    public TimingDiagramTab() {
        initComponents();
        setupZoomSlider();

        //Zoom popup menu
        popupCheckBoxGroup.add(xyZoomCheckBoxMenuItem);
        popupCheckBoxGroup.add(verticalCheckBoxMenuItem);
        popupCheckBoxGroup.add(horizontalZoomCheckBoxMenuItem);
//        horizontalZoomCheckBoxMenuItem.setSelected(true);

        logZoomSlider.setMaximum(maxZoomSlider);
        logZoomSlider.setMinimum(minZoomSlider);

        setupTimeRangeSliders();
        setLogZoomParameters();

        setZoomSize(1);
        setFineTimingLimits();
   
        setCoarseTimingLimits(0, 1);


        //fineTimeSlider.set

        chartTable.getModel().addTableModelListener(new TableModelListener() {
            public void tableChanged(TableModelEvent evt) {
                if(evt.getColumn() == chartColumn) {
//                    System.out.println("Min: "+ ((RangePolicyFixedViewport)diagrams.get(evt.getFirstRow()).chart.getAxisX().getRangePolicy()).getMin(0,0) );
//                    System.out.println("Max: "+ diagrams.get(evt.getFirstRow()).chart.getAxisX().getRangePolicy().getMax(0, 0) );

                    double newMax = diagrams.get(evt.getFirstRow()).chart.getAxisX().getRangePolicy().getMax(0, 0);
                    double newMin = diagrams.get(evt.getFirstRow()).chart.getAxisX().getRangePolicy().getMin(0, 0);

                    setChartsTimeRange(newMin, newMax);
               //     timeRangeSlider.setValues(newMin,newMax);
                    setCoarseSliderValues(newMin / coarseRangeScale, newMax / coarseRangeScale);
                   // timeRangeSlider.setMaximumValue();
                    setZoomSize(Math.abs(newMax - newMin));
                    setFineTimingLimits();


                 //   timingDiagrams.get(evt.getFirstRow()).getAxisX().getMaxValue();
                }
            }});
    }

    public void installDeviceCollection(DeviceCollection collection) {
        deviceCollection = collection;
        
    }

    private boolean visible = false;
    private boolean upToDate = false;
    
    public void tabIsVisible() {
        visible = true;
        if(!upToDate && recentEvent != null) {
 //           System.out.println("Charts.");
            loadEventData(recentEvent.getEventsByChannel());
            setupEventTable();
            upToDate = true;
        }
    }

    public void tabIsHidden() {
        visible = false;
    }

    private void setBiSliderColoredValues(BiSlider slider, double min, double max) {
        if(max < slider.getMaximumValue()) {
            slider.setMaximumColoredValue(max);
        } else {
            slider.setMaximumColoredValue(slider.getMaximumValue() - 1E-8);
         //   slider.setMaximumColoredValue(slider.getMaximumValue());

        }
        if(min > slider.getMinimumValue()) {
            slider.setMinimumColoredValue(min);
        } else {
            slider.setMinimumColoredValue(slider.getMinimumValue() + 1E-8);
        }
    }

    private void setCoarseSliderValues(double min, double max) {
        setBiSliderColoredValues(coarseTimeSlider, min, max);
    }
    private void setFineSliderValues(double min, double max) {
        setBiSliderColoredValues(fineTimeSlider, min, max);
    }

    private DataManagerEvent recentEvent = null;

    public void getData(DataManagerEvent event) {
  //      System.out.println("Is vis: " + visible);
        recentEvent = event;
        upToDate = false;
        if(visible) {
//            System.out.println("Charts.");
            loadEventData(event.getEventsByChannel());
            setupEventTable();
            upToDate = true;
        }
    }

    private void setupEventTable() {
         Vector< Vector<Object> > tableData = new Vector< Vector<Object> >();

         for(int i = 0; i < diagrams.size(); i++) {
             tableData.add(new Vector<Object>());

             tableData.lastElement().insertElementAt(i, 0);
             tableData.lastElement().insertElementAt(diagrams.get(i).eventChannel.deviceName, 1);
             tableData.lastElement().insertElementAt(diagrams.get(i).eventChannel.channel, 2);
             tableData.lastElement().insertElementAt("", 3);
             tableData.lastElement().insertElementAt(diagrams.get(i).chart, 4);
         }

        ((DefaultTableModel)chartTable.getModel()).setDataVector(tableData, columnHeaders);

        chartTable.getColumnModel().getColumn(chartColumn).setCellRenderer(chartRenderer);
        chartTable.getColumnModel().getColumn(chartColumn).setCellEditor(chartEditor);
        chartTable.repaint();

            chartTable.getModel().addTableModelListener(new TableModelListener() {
            public void tableChanged(TableModelEvent evt) {
                if(evt.getColumn() == chartColumn) {
//                    System.out.println("Min: "+ ((RangePolicyFixedViewport)diagrams.get(evt.getFirstRow()).chart.getAxisX().getRangePolicy()).getMin(0,0) );
//                    System.out.println("Max: "+ diagrams.get(evt.getFirstRow()).chart.getAxisX().getRangePolicy().getMax(0, 0) );

                    double newMax = diagrams.get(evt.getFirstRow()).chart.getAxisX().getRangePolicy().getMax(0, 0);
                    double newMin = diagrams.get(evt.getFirstRow()).chart.getAxisX().getRangePolicy().getMin(0, 0);

    //                diagrams.get(evt.getFirstRow()).chart.

                    setChartsTimeRange(newMin, newMax);
               //     timeRangeSlider.setValues(newMin,newMax);
                    coarseTimeSlider.setColoredValues(newMin, newMax);
                   // timeRangeSlider.setMaximumValue();

                 //   timingDiagrams.get(evt.getFirstRow()).getAxisX().getMaxValue();
                }
            }});

    //    setTimingDiagramRange(0, 10);
        chartTable.getColumnModel().getColumn(0).setPreferredWidth(10);
        chartTable.getColumnModel().getColumn(1).setPreferredWidth(10);
        chartTable.getColumnModel().getColumn(2).setPreferredWidth(10);
        chartTable.getColumnModel().getColumn(3).setPreferredWidth(10);
        chartTable.getColumnModel().getColumn(chartColumn).setPreferredWidth(600);
    }

    private void loadEventData(HashMap<Integer, EventChannel> events) {

        diagrams.clear();
        chartRenderer.clear();
        chartEditor.clear();

        java.util.Set<Integer> keys = events.keySet();
        java.util.Iterator<Integer> iter = keys.iterator();
        while(iter.hasNext()) {
            diagrams.add( new ChannelTimingDiagram( events.get(iter.next()) ) );

            //Register this chart as the renderer and editor for its JTable cell
            chartRenderer.addChart(diagrams.lastElement().chart);
            chartEditor.addChart(diagrams.lastElement().chart);
        }

        int i;
        double min, max;
        if(diagrams.size() > 0) {
            min = diagrams.firstElement().eventChannel.getMinTime();
            max = diagrams.firstElement().eventChannel.getMaxTime();
        } else {
            min = 0;
            max = 0;
        }

        for(i = 0; i < diagrams.size(); i++) {
            if(diagrams.get(i).eventChannel.getMinTime() < min) {
                min = diagrams.get(i).eventChannel.getMinTime();
            }
            if(diagrams.get(i).eventChannel.getMaxTime() > max) {
                max = diagrams.get(i).eventChannel.getMaxTime();
            }
        }
        for(i = 0; i < diagrams.size(); i++) {
            diagrams.get(i).addEndEvent(max);
        }


        //pad end of chart
        double padMax = max*(1 + Math.signum(max) * chartEndTimePaddingFration);

        setTimeRangeSeconds(min / ChannelTimingDiagram.timebase, padMax / ChannelTimingDiagram.timebase);

    }

    private void setChartsTimeRange(double min, double max) {
        for (int i = 0; i < diagrams.size(); i++) {
            diagrams.get(i).chart.zoom(min, max);
        }
        chartTable.repaint();
    }
    
    private void setTimeRangeSeconds(double min_s, double max_s) {
        setCoarseTimingLimits(min_s, max_s);
        setChartsTimeRange(min_s, max_s);
//        coarseTimeSlider.setMinimumValue(min_s);
//        coarseTimeSlider.setMaximumValue(max_s);
//        coarseTimeSlider.setSegmentSize(Math.abs(max_s - min_s) / coarseSliderSegments);
        coarseTimeSlider.setUnit("");
        //timeRangeSlider.getDecimalFormater().setMaximumIntegerDigits(2);
     //   coarseTimeSlider.getDecimalFormater().setMaximumFractionDigits(2);
     //   fineTimeSlider.getDecimalFormater().setMaximumFractionDigits(0);
    //    fineTimeSlider.getDecimalFormater().applyLocalizedPattern("#00;(#00)");
      //  fineTimeSlider.getDecimalFormater().setMaximumIntegerDigits(2);
      //  fineTimeSlider.getDecimalFormater().setRoundingMode(java.math.RoundingMode.UP);
        
    }

    private void setupTimeRangeSliders() {
        
        coarseTimeSlider.setVisible(true);
    //    coarseTimeSlider.setMinimumValue(0);
    //    coarseTimeSlider.setMaximumValue(1);
    //    coarseTimeSlider.setSegmentSize(0.1);
        coarseTimeSlider.setUnit("");
        coarseTimeSlider.setPrecise(true);





        coarseTimeSlider.addBiSliderListener(new BiSliderAdapter() {

            @Override
            public void newValues(BiSliderEvent BiSliderEvent_Arg) {
                if (sliderMinimunReached()) {
                    //      timeRangeSlider.setMaximumValue(timeRangeSlider.getMaximumValue() * 0.9);
            //        coarseTimeSlider.setMaximumColoredValue(coarseTimeSlider.getMinimumColoredValue() + 0.05);

                    setCoarseSliderValues(
                            coarseSliderMidpoint - (minSizeCoarseSlider * (1.01) / 2.0),
                            coarseSliderMidpoint + (minSizeCoarseSlider * (1.01) / 2.0));
                }

                double oldCoarseSliderSize = coarseSliderSize;
                coarseSliderSize = (coarseTimeSlider.getMaximumColoredValue()
                - coarseTimeSlider.getMinimumColoredValue());


                if( coarseSliderSize != oldCoarseSliderSize) {
                    setZoomSize(coarseSliderSize * coarseRangeScale);
                }


                double oldCoarseSliderMidpoint = coarseSliderMidpoint;

                coarseSliderMidpoint = (coarseTimeSlider.getMaximumColoredValue()
                + coarseTimeSlider.getMinimumColoredValue()) / 2.0;

                if(oldCoarseSliderMidpoint != coarseSliderMidpoint) {
                    setChartsRangeToCurrent();
                }

                //setChartsTimeRange(BiSliderEvent_Arg.getMinimum(), BiSliderEvent_Arg.getMaximum());

            }
        });

        fineTimeSlider.addBiSliderListener(new BiSliderAdapter() {

            @Override
            public void newValues(BiSliderEvent BiSliderEvent_Arg) {
               setChartsRangeToCurrent();
            }
        });

        chartTable.addMouseListener(new java.awt.event.MouseAdapter() {

            @Override
            public void mouseClicked(java.awt.event.MouseEvent e) {
                if (e.getButton() == java.awt.event.MouseEvent.BUTTON3 &&
                        chartTable.columnAtPoint(e.getPoint()) == chartColumn) {
                    chartPopupMenu.show(chartTable, e.getX(), e.getY());
                }
            }
        });

    }

    private void setChartsRangeToCurrent() {
         double min_s = (coarseSliderMidpoint * coarseRangeScale)
                 + (fineTimeSlider.getMinimumColoredValue() * fineRangeScale);
         double max_s = (coarseSliderMidpoint * coarseRangeScale)
                 + (fineTimeSlider.getMaximumColoredValue() * fineRangeScale);
         setChartsTimeRange(min_s, max_s);
    }
    private boolean sliderMinimunReached() {

//        double size = (coarseTimeSlider.getMaximumValue()-coarseTimeSlider.getMinimumValue());
     //   return false;
        return (
               (coarseTimeSlider.getMaximumColoredValue() - coarseTimeSlider.getMinimumColoredValue())
                ) < (minSizeCoarseSlider);

    }


    private class ChannelTimingDiagram {

        //This subclass allows for a selectable, constrainted zoom
        private class XYZoomableChart extends ZoomableChart {
            @Override
            public void zoom(double xmin, double xmax, double ymin, double ymax) {
                switch(zoomMode) {
                    case Horizontal:
                        super.zoom(xmin, xmax);
                        break;
                    case Vertical:
                        getAxisX().getRangePolicy().getMax(0, 0);
                        super.zoom(
                                getAxisX().getRangePolicy().getMin(0, 0),
                                getAxisX().getRangePolicy().getMax(0, 0),
                                ymin, ymax);
                        break;
                    case XY:
                        super.zoom(xmin, xmax, ymin, ymax);
                        break;
                }
           }
        };

        public XYZoomableChart chart = new XYZoomableChart();
        //public ITrace2D trace = new Trace2DSimple();
        public EventChannel eventChannel;
        public static final double timebase = 1000000000;   //(time is in ns)

        private STIGraphicalParser parser = null;
        private Vector<Trace2DSimple> traces = null;
        private int numTraces = 0;

        private short channel;

        public ChannelTimingDiagram(EventChannel eventChannel) {
            this.eventChannel = eventChannel;

            channel = eventChannel.channel;
      //      trace.setColor(Color.BLUE);
      //      trace.setName("");

            Device device = deviceCollection.getDevice(this.eventChannel.tDevice);
            
            if(device != null) {
                parser = device.getGraphicalParser();
            }

            if(parser != null) {
                numTraces = parser.getNumberOfTraces(channel);
                traces = new Vector<Trace2DSimple>(numTraces);

                chart.getAxesYLeft().clear();

                for(int i = 0; i < numTraces; i++) {
                    traces.add(new Trace2DSimple());
                    traces.get(i).setName(parser.getTraceLabels(channel)[i]);
                    traces.get(i).setPhysicalUnits("s", parser.getTraceUnits(channel)[i]);
                    traces.get(i).setColor(parser.getTraceColors(channel)[i]);

                    chart.addAxisYLeft(new info.monitorenter.gui.chart.axis.AxisLinear());

                    chart.addTrace(traces.get(i),
                            chart.getAxesXBottom().get(0),
                            chart.getAxesYLeft().get(i));
                    chart.repaint();
                    chart.getAxesYLeft().get(i).getAxisTitle().setTitle(parser.getTraceUnits(channel)[i]);
                    chart.getAxesYLeft().get(i).getAxisTitle().setTitleColor(parser.getTraceColors(channel)[i]);

                }


                parser.setupEvents(channel, eventChannel.getEvents(), traces);
             //   eventChannel.getEvents().lastElement()
                chart.repaint();
            }

          //  chart.addTrace(trace);
          //  chart.getAxisX().getAxisTitle().setTitle("");



            //  System.out.println("title: " + chart.getAxisX().getAxisTitle().getTitle());
            //trace.setPhysicalUnits("s", "");



        //    setupPoints(eventChannel.getEvents());

        }

        public void addEndEvent(double time) {

            double[] endValues = parser.getEndingYValues(channel);

            for(int i = 0; i < endValues.length; i++) {
                traces.get(i).addPoint(time / parser.timebase, endValues[i]);
            }

            //eventChannel.addEndEvent(time);
            //addPoint(eventChannel.getEvents().lastElement().time / timebase,
            //        eventChannel.getEvents().lastElement().value);

        }

//        private void addPoint(double time, TValMixed value) {
//            switch (value.discriminator().value()) {
//                case TValue._ValueNumber:
//                    trace.addPoint(time,
//                            value.number());
//                    break;
//                case TValue._ValueVector:
//                    break;
//                default:
//                    break;
//            }
//        }

//        private void setupPoints(Vector<MixedEvent> events) {
//
//            for (int i = 0; i < events.size(); i++) {
//                if (i > 0) {
//                    //holds the last value until next event; less 1 ns for ordering
//                    addPoint((events.elementAt(i).time - 1) / timebase, events.elementAt(i - 1).value);
//                }
//                addPoint(events.elementAt(i).time / timebase, events.elementAt(i).value);
//            }
//        }
    }


    private void setZoomSize(double size_s) {
        
        //setLogZoomParameters();
        logZoomSlider.setValue((int)Math.round(Math.log10(size_s / prescaler) / logScale));


        //setFineTimingLimits();
    }

    private SIScale getSIScale(double newRange) {
        SIScale scale;
        if(Math.log10(Math.abs(newRange)) > 9) {
            scale = SIScale.Giga;
        } else if(Math.log10(Math.abs(newRange)) > 6) {
            scale = SIScale.Mega;
        } else if(Math.log10(Math.abs(newRange)) > 3) {
            scale = SIScale.Kilo;
        } else if(Math.log10(Math.abs(newRange)) > 0) {
            scale = SIScale.One;
        } else if(Math.log10(Math.abs(newRange)) > -3) {
            scale = SIScale.milli;
        } else if(Math.log10(Math.abs(newRange)) > -6) {
            scale = SIScale.micro;
        } else {
            scale = SIScale.nano;
        }
        return scale;
    }
    
    private double getSImulitplier(SIScale scale) {
        switch (scale) {
            case Giga:
                return Math.pow(10.0, 9);
            case Mega:
                return Math.pow(10.0, 6);
            case Kilo:
                return Math.pow(10.0, 3);
            case One:
                return Math.pow(10.0, 0);
            case milli:
                return Math.pow(10.0, -3);
            case micro:
                return Math.pow(10.0, -6);
            case nano:
                return Math.pow(10.0, -9);
            default:
                return Math.pow(10.0, 0);
        }
    }
    
    private String getSIprefix(SIScale scale) {
        switch (scale) {
            case Giga:
                return "G";
            case Mega:
                return "M";
            case Kilo:
                return "k";
            case One:
                return "";
            case milli:
                return "m";
            case micro:
                return "u";
            case nano:
                return "n";
            default:
                return "";
        }
    }



    private void setFineRangeScale(double newRange) {
        fineRangeSIScale = getSIScale(newRange);
        fineRangeScale = getSImulitplier(fineRangeSIScale);
        fineUnitsLabel.setText(getSIprefix(fineRangeSIScale) + "s");
    }

    private void setCoarseRangeScale(double newRange) {
        coarseRangeSIScale = getSIScale(newRange);
        coarseRangeScale = getSImulitplier(coarseRangeSIScale);
        coarseUnitsLabel.setText(getSIprefix(coarseRangeSIScale) + "s");
    }
    //double fineRange = 1;

    private void setFineTimingLimits() {

        double oldFineRange = fineTimeSlider.getMaximumValue() - fineTimeSlider.getMinimumValue();
    //    double oldFineScale = fineRangeScale;
        double fineColoredRange = (fineTimeSlider.getMaximumColoredValue()
                - fineTimeSlider.getMinimumColoredValue());
        double fineColoredMidpoint = (fineTimeSlider.getMaximumColoredValue()
                + fineTimeSlider.getMinimumColoredValue()) / 2.0;
        
        

        double newFineRange = prescaler * Math.pow(10.0, logScale * logZoomSlider.getValue());

        setFineRangeScale(newFineRange);

//        fineTimeSlider.setValues(-1*(newFineRange * fineRangeScale / 2.0),
//                (newFineRange * fineRangeScale / 2.0));

        //fineTimeSlider.getMaximumColor().getBlue();


        fineTimeSlider.setMinimumValue(-1*((newFineRange / fineRangeScale) / 2.0));
        fineTimeSlider.setMaximumValue(((newFineRange / fineRangeScale) / 2.0));
//        try {
//        fineTimeSlider.setValues(-1*((newFineRange / fineRangeScale) / 2.0),
//                ((newFineRange / fineRangeScale) / 2.0));
//        fineTimeSlider.setMaximumColor(Color.BLUE);
//        fineTimeSlider.setMinimumColor(Color.RED);
//        } catch(IllegalArgumentException e) {
//
//        }
//* (fineRangeScale / oldFineScale )
        double newScaledMidpoint = (fineColoredMidpoint / oldFineRange) * newFineRange;
        double newScaledFineColoredRange = (fineColoredRange / oldFineRange) * newFineRange;

        setFineSliderValues((newScaledMidpoint - (newScaledFineColoredRange / 2.0)) / fineRangeScale,
                (newScaledMidpoint + (newScaledFineColoredRange / 2.0)) / fineRangeScale);

        fineTimeSlider.setSegmentSize((newFineRange / fineRangeScale) / fineSliderSegments);
//        fineTimeSlider.setMinimumColoredValue((newScaledMidpoint - (newScaledFineColoredRange / 2.0)) / fineRangeScale);
//        fineTimeSlider.setMaximumColoredValue((newScaledMidpoint + (newScaledFineColoredRange / 2.0)) / fineRangeScale);
    }
    


//            double coarseColoredMidpoint = (coarseTimeSlider.getMaximumColoredValue()
//                + coarseTimeSlider.getMinimumColoredValue()) / 2.0;
    
    private void setLogZoomParameters() {
        //fine range = prescaler * 10^(logScale * logZoomSlider.getValue())     
        logScale = Math.log10(maxZoomedRange / minZoomedRange) /
                (maxZoomSlider - minZoomSlider);
        prescaler = minZoomedRange / (Math.pow(10.0, logScale * minZoomSlider));

//        ((javax.swing.JLabel)logZoomSlider.getLabelTable().get(minZoomSlider)).
  //              setText(getSIprefix(getSIScale(minZoomedRange)) + "s");
                
        ((javax.swing.JLabel)logZoomSlider.getLabelTable().get(minZoomSlider)).
                setText("ns");
        ((javax.swing.JLabel)logZoomSlider.getLabelTable().get(maxZoomSlider)).
                setText(getSIprefix(getSIScale(maxZoomedRange)) + "s");
    }

    private void setupZoomSlider() {
        logZoomSlider.setPaintLabels(false);
//        while(logZoomSlider.getLabelTable().keys().hasMoreElements()) {
//            logZoomSlider.getLabelTable().remove(
//                    logZoomSlider.getLabelTable().keys().nextElement());
//        }
        Hashtable<Integer, javax.swing.JLabel> table = new Hashtable<Integer, javax.swing.JLabel>();
        //logZoomSlider.setLabelTable(new java.util.Hashtable<Integer, javax.swing.JLabel>());
        table.put(minZoomSlider,
                new javax.swing.JLabel(""));

        ((javax.swing.JLabel)table.get(minZoomSlider)).setPreferredSize(
                new java.awt.Dimension(
                ((javax.swing.JLabel)table.get(minZoomSlider)).getPreferredSize().width*2,
                ((javax.swing.JLabel)table.get(minZoomSlider)).getPreferredSize().height));
        table.put(maxZoomSlider,
                new javax.swing.JLabel(""));
        logZoomSlider.setLabelTable(table);
        logZoomSlider.setPaintLabels(true);
    }

    private void setCoarseTimingLimits(double min_s, double max_s) {

        double range = Math.abs(max_s - min_s);

        setCoarseRangeScale(range);

        minSizeCoarseSlider = range * coarseSliderMinFraction / coarseRangeScale;



        coarseTimeSlider.setValues(min_s / coarseRangeScale, max_s / coarseRangeScale);
        coarseTimeSlider.setSegmentSize((range / coarseSliderSegments) / coarseRangeScale);
        
        //Log zoom range
        maxZoomedRange = Math.abs(max_s);
        setLogZoomParameters();
        //setZoomSize(max_s);
        setFineTimingLimits();

        setFineSliderValues(fineTimeSlider.getMinimumValue(), fineTimeSlider.getMaximumValue());



    }

    private void updateZoomMode() {
        if(xyZoomCheckBoxMenuItem.isSelected()) {
            zoomMode = ZoomMode.XY;
        }
        if(horizontalZoomCheckBoxMenuItem.isSelected()) {
            zoomMode = ZoomMode.Horizontal;
        }
        if(verticalCheckBoxMenuItem.isSelected()) {
            zoomMode = ZoomMode.Vertical;
        }
    }
    
    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    
    
    
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        chartPopupMenu = new javax.swing.JPopupMenu();
        horizontalZoomCheckBoxMenuItem = new javax.swing.JCheckBoxMenuItem();
        xyZoomCheckBoxMenuItem = new javax.swing.JCheckBoxMenuItem();
        verticalCheckBoxMenuItem = new javax.swing.JCheckBoxMenuItem();
        jSeparator1 = new javax.swing.JPopupMenu.Separator();
        zoomOutMenuItem = new javax.swing.JMenuItem();
        popupCheckBoxGroup = new javax.swing.ButtonGroup();
        jSplitPane1 = new javax.swing.JSplitPane();
        jPanel1 = new javax.swing.JPanel();
        jPanel3 = new javax.swing.JPanel();
        logZoomSlider = new javax.swing.JSlider();
        fineUnitsLabel = new javax.swing.JLabel();
        fineTimeSlider = new com.visutools.nav.bislider.BiSlider();
        jPanel4 = new javax.swing.JPanel();
        coarseTimeSlider = new com.visutools.nav.bislider.BiSlider();
        coarseUnitsLabel = new javax.swing.JLabel();
        jPanel2 = new javax.swing.JPanel();
        jScrollPane1 = new javax.swing.JScrollPane();
        chartTable = new javax.swing.JTable();

        horizontalZoomCheckBoxMenuItem.setSelected(true);
        horizontalZoomCheckBoxMenuItem.setText("Horizontal Zoom");
        horizontalZoomCheckBoxMenuItem.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                horizontalZoomCheckBoxMenuItemActionPerformed(evt);
            }
        });
        chartPopupMenu.add(horizontalZoomCheckBoxMenuItem);

        xyZoomCheckBoxMenuItem.setText("2D Zoom");
        xyZoomCheckBoxMenuItem.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                xyZoomCheckBoxMenuItemActionPerformed(evt);
            }
        });
        chartPopupMenu.add(xyZoomCheckBoxMenuItem);

        verticalCheckBoxMenuItem.setText("Vertical Zoom");
        verticalCheckBoxMenuItem.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                verticalCheckBoxMenuItemActionPerformed(evt);
            }
        });
        chartPopupMenu.add(verticalCheckBoxMenuItem);
        chartPopupMenu.add(jSeparator1);

        zoomOutMenuItem.setText("Zoom Out");
        zoomOutMenuItem.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                zoomOutMenuItemActionPerformed(evt);
            }
        });
        chartPopupMenu.add(zoomOutMenuItem);

        jSplitPane1.setDividerLocation(125);
        jSplitPane1.setOrientation(javax.swing.JSplitPane.VERTICAL_SPLIT);

        jPanel1.setPreferredSize(new java.awt.Dimension(700, 119));

        jPanel3.setBorder(javax.swing.BorderFactory.createTitledBorder("Zoom"));

        logZoomSlider.setPaintLabels(true);
        logZoomSlider.setValue(100);
        logZoomSlider.addChangeListener(new javax.swing.event.ChangeListener() {
            public void stateChanged(javax.swing.event.ChangeEvent evt) {
                logZoomSliderStateChanged(evt);
            }
        });

        fineUnitsLabel.setFont(new java.awt.Font("Tahoma", 1, 12));
        fineUnitsLabel.setText("s");
        fineUnitsLabel.setPreferredSize(new java.awt.Dimension(20, 15));

        fineTimeSlider.setInterpolationMode(com.visutools.nav.bislider.BiSlider.CENTRAL);
        fineTimeSlider.setMinimumColoredValue(-100.0);
        fineTimeSlider.setMinimumValue(-100.0);
        fineTimeSlider.setPrecise(true);
        fineTimeSlider.setSegmentSize(20.0);

        javax.swing.GroupLayout jPanel3Layout = new javax.swing.GroupLayout(jPanel3);
        jPanel3.setLayout(jPanel3Layout);
        jPanel3Layout.setHorizontalGroup(
            jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel3Layout.createSequentialGroup()
                .addComponent(logZoomSlider, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(fineTimeSlider, javax.swing.GroupLayout.DEFAULT_SIZE, 379, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(fineUnitsLabel, javax.swing.GroupLayout.PREFERRED_SIZE, 18, javax.swing.GroupLayout.PREFERRED_SIZE))
        );
        jPanel3Layout.setVerticalGroup(
            jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel3Layout.createSequentialGroup()
                .addComponent(fineUnitsLabel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(32, Short.MAX_VALUE))
            .addComponent(logZoomSlider, javax.swing.GroupLayout.DEFAULT_SIZE, 47, Short.MAX_VALUE)
            .addComponent(fineTimeSlider, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 47, Short.MAX_VALUE)
        );

        coarseTimeSlider.setPrecise(true);

        coarseUnitsLabel.setFont(new java.awt.Font("Tahoma", 1, 12));
        coarseUnitsLabel.setText("s");

        javax.swing.GroupLayout jPanel4Layout = new javax.swing.GroupLayout(jPanel4);
        jPanel4.setLayout(jPanel4Layout);
        jPanel4Layout.setHorizontalGroup(
            jPanel4Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, jPanel4Layout.createSequentialGroup()
                .addComponent(coarseTimeSlider, javax.swing.GroupLayout.DEFAULT_SIZE, 597, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(coarseUnitsLabel, javax.swing.GroupLayout.PREFERRED_SIZE, 18, javax.swing.GroupLayout.PREFERRED_SIZE))
        );
        jPanel4Layout.setVerticalGroup(
            jPanel4Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(coarseUnitsLabel)
            .addComponent(coarseTimeSlider, javax.swing.GroupLayout.PREFERRED_SIZE, 44, javax.swing.GroupLayout.PREFERRED_SIZE)
        );

        javax.swing.GroupLayout jPanel1Layout = new javax.swing.GroupLayout(jPanel1);
        jPanel1.setLayout(jPanel1Layout);
        jPanel1Layout.setHorizontalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jPanel4, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
            .addComponent(jPanel3, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
        );
        jPanel1Layout.setVerticalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel1Layout.createSequentialGroup()
                .addComponent(jPanel4, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jPanel3, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        jSplitPane1.setLeftComponent(jPanel1);

        chartTable.setModel(new javax.swing.table.DefaultTableModel(
            new Object [][] {

            },
            new String [] {
                "Row", "Device", "Channel", "Name", "Timing Diagram"
            }
        ) {
            boolean[] canEdit = new boolean [] {
                false, false, false, true, true
            };

            public boolean isCellEditable(int rowIndex, int columnIndex) {
                return canEdit [columnIndex];
            }
        });
        chartTable.setAutoResizeMode(javax.swing.JTable.AUTO_RESIZE_LAST_COLUMN);
        chartTable.setColumnSelectionAllowed(true);
        chartTable.setRowHeight(100);
        jScrollPane1.setViewportView(chartTable);
        chartTable.getColumnModel().getSelectionModel().setSelectionMode(javax.swing.ListSelectionModel.SINGLE_SELECTION);
        chartTable.getColumnModel().getColumn(2).setResizable(false);
        chartTable.getColumnModel().getColumn(4).setResizable(false);
        chartTable.getColumnModel().getColumn(4).setPreferredWidth(400);

        javax.swing.GroupLayout jPanel2Layout = new javax.swing.GroupLayout(jPanel2);
        jPanel2.setLayout(jPanel2Layout);
        jPanel2Layout.setHorizontalGroup(
            jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 619, Short.MAX_VALUE)
        );
        jPanel2Layout.setVerticalGroup(
            jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 561, Short.MAX_VALUE)
        );

        jSplitPane1.setRightComponent(jPanel2);

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jSplitPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 621, Short.MAX_VALUE)
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jSplitPane1, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 692, Short.MAX_VALUE)
        );
    }// </editor-fold>//GEN-END:initComponents

    private void logZoomSliderStateChanged(javax.swing.event.ChangeEvent evt) {//GEN-FIRST:event_logZoomSliderStateChanged
        setFineTimingLimits();
    }//GEN-LAST:event_logZoomSliderStateChanged

    private void zoomOutMenuItemActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_zoomOutMenuItemActionPerformed
        for (int i = 0; i < diagrams.size(); i++) {
            diagrams.get(i).chart.zoomAll();
        }
        setCoarseSliderValues(coarseTimeSlider.getMinimumValue(), coarseTimeSlider.getMaximumValue());
    }//GEN-LAST:event_zoomOutMenuItemActionPerformed

    private void xyZoomCheckBoxMenuItemActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_xyZoomCheckBoxMenuItemActionPerformed
        updateZoomMode();
    }//GEN-LAST:event_xyZoomCheckBoxMenuItemActionPerformed

    private void horizontalZoomCheckBoxMenuItemActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_horizontalZoomCheckBoxMenuItemActionPerformed
        updateZoomMode();
    }//GEN-LAST:event_horizontalZoomCheckBoxMenuItemActionPerformed

    private void verticalCheckBoxMenuItemActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_verticalCheckBoxMenuItemActionPerformed
        updateZoomMode();
    }//GEN-LAST:event_verticalCheckBoxMenuItemActionPerformed


    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JPopupMenu chartPopupMenu;
    private javax.swing.JTable chartTable;
    private com.visutools.nav.bislider.BiSlider coarseTimeSlider;
    private javax.swing.JLabel coarseUnitsLabel;
    private com.visutools.nav.bislider.BiSlider fineTimeSlider;
    private javax.swing.JLabel fineUnitsLabel;
    private javax.swing.JCheckBoxMenuItem horizontalZoomCheckBoxMenuItem;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JPanel jPanel2;
    private javax.swing.JPanel jPanel3;
    private javax.swing.JPanel jPanel4;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JPopupMenu.Separator jSeparator1;
    private javax.swing.JSplitPane jSplitPane1;
    private javax.swing.JSlider logZoomSlider;
    private javax.swing.ButtonGroup popupCheckBoxGroup;
    private javax.swing.JCheckBoxMenuItem verticalCheckBoxMenuItem;
    private javax.swing.JCheckBoxMenuItem xyZoomCheckBoxMenuItem;
    private javax.swing.JMenuItem zoomOutMenuItem;
    // End of variables declaration//GEN-END:variables

}
