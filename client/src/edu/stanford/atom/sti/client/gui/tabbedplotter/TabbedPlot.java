/** @file   TabbedPlot.java
 *  @author Jonathan David Harvey
 *  @brief Source-file for the class "TabbedPlot"
 *  @section license License
 *
 *  Copyright (C) 2008 Jonathan Harvey <harv@stanford.edu>\n
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
 *Template
 *  You should have received a copy of the GNU General Public License
 *  along with STI.  If not, see <http://www.gnu.org/licenses/>.
 */

package edu.stanford.atom.sti.client.gui.tabbedplotter;

import javax.swing.*;
import javax.swing.text.*;
import java.awt.Color;
import org.jdesktop.swingx.JXGraph;
import javax.swing.JPanel.*;
import java.util.Vector;
import java.awt.geom.Rectangle2D;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
//import javax.swing.JComponent;

// Temporary until we get the other parser working.
import org.nfunk.jep.*;

public class TabbedPlot {

    // Declare Attributes
    private String tabTitle;
    private javax.swing.JScrollPane currentScrollPane;
    private javax.swing.JTextPane textPaneLocal;
    private JXGraph localGraph;
    private Vector<String> localPlotExpressions;
    private JPopupMenu graphContextMenu;
    private int tabIndex;
    private boolean modified;

    private TabbedPlotter currentPlotter;
    private Color[] defaultColors;

    // Constructors
    //
    /**
     * 
     * @param tP
     * @param thisTabIndex
     */
    public TabbedPlot(TabbedPlotter tP, int thisTabIndex, int thisTabCreationIndex) {
        initAttributes();
        
        setTabIndex(thisTabIndex);
        currentPlotter = tP;

        setTabTitle("Tab " + thisTabCreationIndex);
    }
    
    // Initialize Attributes    
    private void initAttributes() {
        localPlotExpressions = new Vector<String>();
        localGraph = new JXGraph();
        textPaneLocal = new javax.swing.JTextPane();
        currentScrollPane = new javax.swing.JScrollPane();
        
        Color[] newColors = {
            Color.RED,
            Color.BLUE,
            Color.CYAN,
            Color.DARK_GRAY,
            Color.GREEN,
            Color.MAGENTA,
            Color.GRAY,
            Color.ORANGE,
            Color.PINK,
            Color.LIGHT_GRAY
        };
        defaultColors = newColors;
    }

    // Accessors
    //
    /**
     * 
     * @return
     */
    public String getTabTitle() {
        return tabTitle;
    }

    /**
     * 
     * @return
     */
    public int getTabIndex() {
        return tabIndex;
    }

    /**
     * 
     * @return
     */
    public JTextPane getTextPane() {
        return textPaneLocal;
    }

    /**
     * Returns this document's scroll pane
     * @return
     */
    public JScrollPane getScrollPane() {
        return currentScrollPane;
    }

    /**
     * 
     * @return
     */
    public boolean getModified() {
        return modified;
    }

    /**
     * 
     * @return
     */
/*    public String getCaretLabelText() {
        return caretListenerLocal.getText();
    }*/

    /**
     * Accessor method for the currentGraph variable.
     * 
     * @return  currentGraph
     * @see     JXGraph
     */
    private Vector<String> getCurrentPlots() {
        //currentGraph.
        return localPlotExpressions;
    }

    /**
     * Returns a Vector<String> from a given String input delimited by semicolons.
     * Intended to form the first, most basic level of parsing from the text box
     * to the math expression.
     * 
     * @param   inputStr    An input string of functions delimited by semicolon followed by space
     * @return              A Vector<String>
     * @see                 Vector
     */
    public String getPlotsAsString() {
        Vector<String> plotContents = getCurrentPlots();
        String returnString = new String();
        // If the plotContents vector was empty the string will just stay null
        if (!plotContents.isEmpty()) {
            int plotCount = plotContents.size();
            // Iterate through the plotContents and add to the string
            for (int j = 0; j < plotCount; j++) {
                // Append the current vector member to the string
                returnString += (String) plotContents.elementAt(j);
                // Append semicolon and space if it wasn't the last member
                if (j != plotCount - 1) {
                    returnString += "; ";
                }
            }
        }
        return returnString;
    }

    // Mutators
    //
    /**
     * 
     * @param index
     */
    public void setTabIndex(int index) {
        tabIndex = index;
    }

    /**
     * Seems like the JXGraph() can't  be resized...
     * @param x
     * @param y
     */
    public void setGraphSize(double x, double y) {

    }

    /**
     * Method to allow the range of the current plot to be changed. This resets
     * the major grid lines to sensical values.
     * 
     * @param xMin      New minimum value for X in the graph
     * @param xMax      New maximum value for X in the graph
     * @param yMin      New minimum value for Y in the graph
     * @param yMax      New maximum value for Y in the graph
     * @see setGraphRange(double xMin, double xMax, double yMin, double yMax, double xGridSpacing, double yGridSpacing)
     */
    public void setGraphRange(double xMin, double xMax,
            double yMin, double yMax) {

        double majorGridX;
        double majorGridY;

        // Factor of 1.5 introduced after testing for more pleasing result
        majorGridX = (int) (1.5 * (xMax - xMin));
        majorGridY = (int) (1.5 * (yMax - yMin));

        majorGridX = majorGridX / 10;
        majorGridY = majorGridY / 10;

        setGraphRange(xMin, xMax, yMin, yMax, majorGridX, majorGridY);
    }

    /**
     * Method to allow the range of the current plot to be changed. Also resets
     * the major grid lines to sensical values.
     * 
     * @param xMin
     * @param xMax
     * @param yMin
     * @param yMax
     * @param xGridSpacing
     * @param yGridSpacing
     */
    public void setGraphRange(double xMin, double xMax,
            double yMin, double yMax, double xGridSpacing, double yGridSpacing) {
        Rectangle2D currentBounds = new Rectangle2D.Double(); // = localGraph.getBounds();
        //double currentMaxX = currentBounds.getMaxX();
        //double currentMinX = currentBounds.getMinX();
        //double currentMaxY = currentBounds.getMaxY();
        //double currentMinY = currentBounds.getMinY();
        //double currentHeight = currentBounds.getHeight();

        currentBounds.setRect(xMin, yMin, xMax - xMin, yMax - yMin);

        localGraph.setView(currentBounds);
        localGraph.setMajorX(xGridSpacing);
        localGraph.setMajorY(yGridSpacing);
    }

    /**
     * 
     * @param inputFunction
     */
    public void setActivePlot(String inputFunction) {
        localGraph.removeAllPlots();
        localPlotExpressions.clear();
        createAllPlotsFromString(inputFunction);
    }

    /**
     * 
     * @param tabName
     */
    public void setTabTitle(String tabName) {
        // Eventually, tabs should be renameable by double-clicking or something

        tabTitle = tabName;
    }

    /**
     * 
     * @param Modified
     */
    public void setModifed(boolean Modified) {
        modified = Modified;
    }

    // Other Methods
    //
    /**
     * 
     * @param inputFunction
     * @return
     */
    public javax.swing.JScrollPane createTab(String inputFunction) {
        currentScrollPane = new javax.swing.JScrollPane();

        createAllPlotsFromString(inputFunction);

        currentScrollPane.setViewportView(localGraph);
        return currentScrollPane;
    }

    /**
     * 
     * @param inputFunction
     */
    public void createAllPlotsFromString(String inputFunction) {
        Vector<String> inputFunctions = inputToVector(inputFunction);
        int i = 0;
        int maxI = defaultColors.length - 1;
        for (String s : inputFunctions) {
            singlePlotFromString(defaultColors[i], s, "x");
            if (i >= maxI) {
                i = 0;
            } else {
                i++;
            }
        }
    }

    /**
     * 
     * @param plotColor
     * @param expression
     * @param variable
     */
    private void singlePlotFromString(Color plotColor, final String expression,
            final String variable) {
        localGraph.addPlots(plotColor, new JXGraph.Plot() {

            public double compute(double value) {
                return parseStringAsMath(expression, variable, value);
            }
        });

        localPlotExpressions.add(expression);
    }

    /**
     * 
     * @param   inputFunction
     * @param   independentVariable
     * @param   independentVariableValue
     * @return  double
     * @see     org.nfunk.jep.JEP
     */
    private double parseStringAsMath(String inputFunction, String independentVariable,
            double independentVariableValue) {
        double evalDouble = 1;
        JEP myJepParser = new JEP();
        myJepParser.addStandardFunctions();
        myJepParser.addStandardConstants();
        myJepParser.addVariable(independentVariable, independentVariableValue);
        try {
            myJepParser.parseExpression(inputFunction);
            evalDouble = myJepParser.getValue();
        //double objDouble = myJepParser.evaluateD();
        } catch (Exception e) {
            System.err.println("Error: " + e);
        }
        return evalDouble;
    }

    /**
     * 
     * @param inputStr
     * @return
     */
    private Vector<String> inputToVector(String inputStr) {

        Vector<String> outVector = new Vector<String>();
        String[] splitArray = inputStr.split("; ");
        for (String s : splitArray) {
            outVector.add(s);
        }
        return outVector;
    }
    
    private void initContextMenu(JComponent thisComponent, 
            Vector<JMenuItem> menuItems) {
        graphContextMenu = new JPopupMenu();
        for(JMenuItem jM : menuItems) {
            graphContextMenu.add(jM);
        }
        
        class PopupListener extends MouseAdapter {
            @Override
            public void mousePressed(MouseEvent e) {
                maybeShowPopup(e);
            }
            
            @Override
            public void mouseReleased(MouseEvent e) {
                maybeShowPopup(e);
            }

            private void maybeShowPopup(MouseEvent e) {
                if (e.isPopupTrigger()) {
                    graphContextMenu.show(e.getComponent(),
                               e.getX(), e.getY());
                }
            }
        }
    }
}
