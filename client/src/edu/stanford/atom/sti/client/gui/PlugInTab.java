/** @file PlugInTab.java
 *  @author Jason Michael Hogan
 *  @brief Source-file for the class PlugInTab
 *  @section license License
 *
 *  Copyright (C) 2008 Jason Hogan <hogan@stanford.edu>\n
 *  This file is part of the Stanford Timing Interface (STI).
 *
 *  The STI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The STI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the STI.  If not, see <http://www.gnu.org/licenses/>.
 */

package edu.stanford.atom.sti.client.gui;

import javax.swing.JToolBar;
import javax.swing.JPanel;
import java.util.Vector;


public class PlugInTab extends JToolBar implements 
        edu.stanford.atom.sti.client.comm.io.PingEventListener,
        edu.stanford.atom.sti.client.comm.io.DeviceDataEventListener {
    
    private Vector<VisibleTabListener> visibleTabListeners = new Vector<VisibleTabListener>();

    public void handleEvent(edu.stanford.atom.sti.corba.Pusher.TPingEvent event) {
        
    }
    public void handleEvent(edu.stanford.atom.sti.corba.Pusher.TDeviceDataEvent event) {

    }
    private String tabName = null;
    private int tabIndex = -1;  //uninitialized value
    private JPanel panel;
    private boolean isVisible = false;

    /** Creates new PlugInTab
     * @param TabName The name of the tab in the tabbed pane.  If not null, this overrides
     * the name given in JTabbedPane.addTab(...).
     * @param ToolBarName The name of the toolbar when detached from the tabbed pane.
     */
    public PlugInTab(String TabName, String ToolBarName) {
        super();
        setTabName(TabName);
        setToolBarName(ToolBarName);
        initComponents();
    }
    public PlugInTab(String name) {
        this(name, name);
    }
    public PlugInTab() {
        this(null, null);
    }
    public boolean tabIsVisible() {
        return isVisible;
    }
    public void setTabVisibility(boolean visible) {
  //      System.out.println("vis: "+visible);
        isVisible = visible;
        fireNewVisibleTabEvent();
    }
    public synchronized void addVisibleTabListener(VisibleTabListener listener) {
        visibleTabListeners.add(listener);
    }
    public synchronized void removeVisibleTabListener(VisibleTabListener listener) {
        visibleTabListeners.remove(listener);
    }

    private synchronized void fireNewVisibleTabEvent() {
        for(int i = 0; i < visibleTabListeners.size(); i++) {
            if(tabIsVisible()) {
                visibleTabListeners.elementAt(i).tabIsVisible();
            } else {
                visibleTabListeners.elementAt(i).tabIsHidden();
            }
        }
    }

    @Override
    public void setEnabled(boolean enabled) {
        super.setEnabled(enabled);
        panel.setEnabled(enabled);
    }
    public void setTabName(String TabName) {
        tabName = TabName;
    }
    public String getTabName() {
        return tabName;
    }
    public void setToolBarName(String ToolBarName) {
        setName(ToolBarName);
    }
    public String getToolBarName() {
        return getName();
    }
    public int getTabIndex() {
        return tabIndex;
    }
    public void setTabIndex(int index) {
        tabIndex = index;
    }
    public JPanel getContentPanel() {
        return panel;
    }
    public void setContentPanel(JPanel contentPanel) {
        panel = contentPanel;
    }
    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        setMinimumSize(new java.awt.Dimension(13, 800));
    }// </editor-fold>//GEN-END:initComponents
    
    
    // Variables declaration - do not modify//GEN-BEGIN:variables
    // End of variables declaration//GEN-END:variables
    
}
