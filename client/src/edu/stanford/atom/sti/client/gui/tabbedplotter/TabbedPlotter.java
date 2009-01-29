/** @file   TabbedPlotter.java
 *  @author Jonathan David Harvey
 *  @author Jason Michael Hogan
 *  @brief Source-file for the class TabbedPlotter
 *  @section license License
 *
 *  Copyright (C) 2008 Jonathan Harvey <harv@stanford.edu>\n
 *  Copyright (C) 2008 Jason Hogan <hogan@stanford.edu>\n
 *  This file is part of Stanford Timing Interface (STI).
 *
 *  STI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  STI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with STI.  If not, see <http://www.gnu.org/licenses/>.
 */

package edu.stanford.atom.sti.client.gui.tabbedplotter;

import javax.swing.*;
import java.util.Vector;
import java.io.*;
import javax.swing.text.*;

/**
 * Designed to provide a simple multi-tabbed interface within
 * a Java GUI which outputs as many graphs as the user desires in as many tabs
 * as they require. This class is a substantial modification of Tabbed Editor, obtained from
 * Jason Hogan.
 * 
 * @author Jonathan Harvey
 * @author Jason Hogan
 */
public class TabbedPlotter {
    
    private Vector<TabbedPlot> tabbedPlotVector = new Vector<TabbedPlot>();
    private int tabCount = 0;
    public javax.swing.JLabel caretPositionLabelLocal;
    private javax.swing.JTabbedPane outputPaneLocal;
    private javax.swing.JFrame jFrameLocal;
    private javax.swing.JTextArea inputTextAreaLocal;
    
    /**
     * This constructor expects a frame and pane 
     * 
     * 
     * @param inputTextArea     The TextArea in which the user can enter input 
     *                          to be graphed
     * @param outputPane        the pane in which the tabs will be generated
     * @param jFrame            The jFrame in which the tabs will live
     */
    public TabbedPlotter(javax.swing.JTextArea inputTextArea, 
            javax.swing.JTabbedPane outputPane,
            javax.swing.JFrame jFrame){
        
        tabCount = 0;
        registerFrame(jFrame);
        registerTabbedPane(outputPane);
        registerInputTextArea(inputTextArea);

        outputPane.addChangeListener(new javax.swing.event.ChangeListener() {

            public void stateChanged(javax.swing.event.ChangeEvent evt) {
                tabbedPaneStateChanged(evt);
            }
        });
    }
    /** 
     * Simple accessor method to return the number of tabs which have been
     * created. Note: this is not the number of tabs currently active, or the
     * net number of tabs, but how many have been created thus far.
     * 
     * @return  tabCount    Gross number of tabs which have been created
     */
    public int getTabCount() {
        return tabCount;
    }    
    /**
     * Defines the class-global attribute outputPaneLocal according to the 
     * outputPane used in the constructor. 
     * 
     * @param jTabbedPane
     */
    public void registerTabbedPane(javax.swing.JTabbedPane jTabbedPane) {
        outputPaneLocal = jTabbedPane;
    }
    /**
     * Defines the class-global attribute inputTextAreaLocal according to the
     * text area specified in the constructor.
     *  
     * @param jTextArea
     */
    public void registerInputTextArea(javax.swing.JTextArea jTextArea){
        inputTextAreaLocal = jTextArea;
    }
    /**
     * Defines the class-global attribute jFrameLocal according to the jFrame
     * specified in the constructor. 
     * 
     * @param jFrame
     */
    public void registerFrame(javax.swing.JFrame jFrame) {
        jFrameLocal = jFrame;
    }
    /**
     * Deprecated method which seems no longer to have any use, but was part of
     * TabbedEditor.
     * 
     * @param caretPositionLabel
     * @deprecated
     */
    public void registerCaretPositionLabel(javax.swing.JLabel caretPositionLabel) {
        caretPositionLabelLocal = caretPositionLabel;
    }

    /**
     * Method to be called if the state of the Tabbed Pane changes, e.g. if a 
     * new pane is selected, which calls updateInputTextArea.
     * 
     * @param evt
     */
    private void tabbedPaneStateChanged(javax.swing.event.ChangeEvent evt) {
        updateInputTextArea();
    }
    /**
     * If there is any active tab, set the input text area to the value from
     * the Vector<String> corresponding to the functions plotted on that graph.
     * Otherwise, if there is no active tab, set the input text area to null.
     * 
     */
    private void updateInputTextArea() {
        if(outputPaneLocal.getSelectedIndex() > -1){
            inputTextAreaLocal.replaceRange(tabbedPlotVector.elementAt(
                outputPaneLocal.getSelectedIndex()).getPlotsAsString(), 0, 
                    inputTextAreaLocal.getText().length());
        } else if(outputPaneLocal.getSelectedIndex() == -1) {
            inputTextAreaLocal.replaceRange(null, 0, inputTextAreaLocal.getText().length());
        }
    }

    /**
     * Creates a new tab. First increments tab count (no tab will be the zeroth
     * in the naming scheme), then adds a new plot to the tabbedPlotVector,
     * 
     * @param input     An input string to be parsed and plotted.
     */
    public void createPlotterTab(String input) {
        tabCount++;

        tabbedPlotVector.addElement(
                new TabbedPlot(this,
                tabbedPlotVector.size(), tabCount));
        
        outputPaneLocal.addTab(tabbedPlotVector.lastElement().getTabTitle(),
                tabbedPlotVector.lastElement().createTab(input));

        // Ensure the new tab is active
        outputPaneLocal.setSelectedIndex(
                tabbedPlotVector.lastElement().getTabIndex());
        
        //tabbedPlotVector.lastElement().setGraphRange(-15, 15, -2, 8);
    }
    
    /**
     * Function to update the plot in the currently selected plotter tab based
     * on an input string, to be provided via the Input Text Area.
     */
    public void updatePlotterTab(String inputString) {
        int i = outputPaneLocal.getSelectedIndex();
        if(i >= 0){
            TabbedPlot tabbedPlot = tabbedPlotVector.elementAt(i);
            if(tabbedPlot != null) {
                tabbedPlot.setActivePlot(inputString);
                //updateInputTextArea();
            }
        }   
    }
    
    /**
     * If removePlotterTab is called without any argument, remove the currently
     * active tab.
     */
    public void removePlotterTab() {
        if(outputPaneLocal.getSelectedIndex() > -1) { 
            removePlotterTab(outputPaneLocal.getSelectedIndex());
        }
    }
    /**
     * Removes the desired tab from the display and deletes the information from
     * the plot vector, then re-indexes the plot vector tabIndex attributes. This
     * means that if you remove a middle tab the tabbedPlotVector indices will
     * still match the indices in the pane. It does not renumber the titles,
     * however, and new tabs will continue incrementing as if this one had not
     * been removed.
     * 
     * @param   tabIndex    The integer index of the tab to be removed
     * @throws  java.lang.ArrayIndexOutOfBoundsException
     */
    public void removePlotterTab(int tabIndex)
            throws ArrayIndexOutOfBoundsException {

        if (tabIndex >= tabbedPlotVector.size() || tabIndex < 0) {
            throw new ArrayIndexOutOfBoundsException(tabIndex);
        }
        outputPaneLocal.removeTabAt(tabIndex);
        tabbedPlotVector.remove(tabIndex);
        //Reindex so TabbedDocument indicies match JTabbedPane indicies
        for (int i = tabIndex; i < tabbedPlotVector.size(); i++) {
            tabbedPlotVector.elementAt(i).setTabIndex(i);
        }
    }
}
