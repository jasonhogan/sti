/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.comm.bl.device;

import edu.stanford.atom.sti.client.comm.bl.device.DeviceEvent.DeviceEventType;
import edu.stanford.atom.sti.client.comm.io.NetworkClassPackage;
import edu.stanford.atom.sti.client.gui.PlugInTab;
import edu.stanford.atom.sti.client.gui.application.STIApplicationPanel;
import edu.stanford.atom.sti.corba.Types.TLabeledData;
import java.util.HashSet;
import java.util.Hashtable;
import javax.swing.JTabbedPane;

/**
 *
 * @author Jason
 */
public class ApplicationManager implements DeviceCollectionListener {

    private Hashtable<Device, ApplicationTab> plugInMap = new Hashtable<Device, ApplicationTab>();
    private HashSet<Device> uninitializedApps = new HashSet<Device>();

    private class ApplicationTab {
        public ApplicationTab(PlugInTab tab, STIApplicationPanel panel) {
            container = tab;
            customPanel = panel;
        }
        public STIApplicationPanel customPanel;
        public PlugInTab container;
    }

    JTabbedPane tabbedPane;

    public ApplicationManager(JTabbedPane tabbedPane) {
        this.tabbedPane = tabbedPane;
    }

    public void addDevice(Device device) {
        if(!isApplication(device)) {
            return;
        }
        if(!isInitialized(device)) {
            uninitializedApps.remove(device);   //just to be sure we save the most recent Device
            uninitializedApps.add(device);
            return;
        }
        addApplicationTab(device);
    }

    private void addApplicationTab(Device device) {

        boolean success = false;

        PlugInTab newTab = new PlugInTab(device.getTDevice().deviceName);
        STIApplicationPanel appPanel;
        ApplicationTab appTab = null;

        java.util.Vector<Class> applicationGUIs = null;
        Class applicationClass = null;

        TLabeledData guiData = device.getLabeledData("JavaGUI");
        String guiClassPath = guiData.data.vector()[0].stringVal();

        NetworkClassPackage guiPackage =
                new NetworkClassPackage(guiData.data.vector()[1].file().networkFile);

        if(guiClassPath != null && guiPackage.hasClass(guiClassPath)) {
            try {
                applicationClass = Class.forName(guiClassPath, true, guiPackage.getJarClassLoader());
            } catch (Exception e) {
                e.printStackTrace();
            }
        } else {
            applicationGUIs =
                    guiPackage.getAvailableSubClasses(STIApplicationPanel.class);
            if (applicationGUIs.size() > 0) {
                applicationClass = applicationGUIs.get(0);
            }
        }

        if (applicationClass != null) {
            try {
                appPanel = (STIApplicationPanel) applicationClass.newInstance();
                appPanel.setDevice(device);
                success = true;
                newTab.add(appPanel);
                newTab.addVisibleTabListener(appPanel);
                appTab = new ApplicationTab(newTab, appPanel);
            } catch (Exception e) {
                // appPanel = new STIApplicationPanel();
                e.printStackTrace();
            }

            if (success) {
                plugInMap.put(device, appTab);
                newTab.setFloatable(false);

                //add the new tab to the console, preserving currently the selected tab
                int selected = tabbedPane.getSelectedIndex();
                tabbedPane.addTab(newTab.getTabName(), newTab);
                tabbedPane.setSelectedIndex(selected);

                uninitializedApps.remove(device);
            }
        }
    }

    public void removeDevice(Device device) {
        PlugInTab oldTab = plugInMap.remove(device).container;

        java.awt.Container container = oldTab.getParent();
        container.remove(oldTab);
        container.repaint();
    }
    
    public void handleDeviceEvent(DeviceEvent evt) {
        if (evt.type == DeviceEventType.AttributeRefresh) {
            if (uninitializedApps.contains(evt.getDevice())) {
                addDevice(evt.getDevice());
            }
        }
        plugInMap.get(evt.getDevice()).customPanel.handleDeviceEvent(evt);
    }
    public void setDeviceManagerStatus(DeviceManager.DeviceManagerStatus status) {

    }

    private boolean isInitialized(Device device) {
        try {
            edu.stanford.atom.sti.corba.Types.TAttribute[] attributes =
                    device.getAttributes();
            return attributes.length > 0;
        } catch (Exception e) {
            return false;
        }
    }

    public boolean isApplication(Device device) {

        String response = device.execute("sti isApplication");
        if(response != null && response.equals("yes")) {
            return true;
        }
        return false;
    }
}
