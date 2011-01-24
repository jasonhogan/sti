/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.comm.bl.device;

import edu.stanford.atom.sti.client.gui.PlugInTab;

import edu.stanford.atom.sti.client.comm.bl.device.Device;
import edu.stanford.atom.sti.client.comm.bl.device.DeviceCollectionListener;
import edu.stanford.atom.sti.client.comm.bl.device.DeviceEvent;
import edu.stanford.atom.sti.client.comm.bl.device.DeviceManager;
import edu.stanford.atom.sti.corba.Types.TLabeledData;

import edu.stanford.atom.sti.client.comm.io.TNetworkFileReader;
import java.awt.event.ContainerEvent;

import java.util.Hashtable;
import java.util.HashSet;
import javax.swing.JTabbedPane;

import edu.stanford.atom.sti.client.gui.application.STIApplicationPanel;
import edu.stanford.atom.sti.client.comm.io.STIRemoteClassLoader;
import edu.stanford.atom.sti.client.comm.io.JarByteClassLoader;
import javax.swing.event.ChangeEvent;
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
        if(!isApplication(device))
            return;
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

        TLabeledData guiData = device.getLabeledData("JavaGUI");
        TNetworkFileReader guiFile =
                new TNetworkFileReader(guiData.data.vector()[1].file().networkFile);

        if (guiFile.read()) {
            JarByteClassLoader jarClassLoader = new JarByteClassLoader(guiFile.getFileData());

//            STIRemoteClassLoader classLoader =
//                    new STIRemoteClassLoader(guiData.data.vector()[0].stringVal(),
//                    guiFile.getFileData());

            try {
                //Class cls = Class.forName(guiData.data.vector()[0].stringVal(), true, classLoader);
                Class cls = Class.forName(guiData.data.vector()[0].stringVal(), true, jarClassLoader);
                appPanel = (STIApplicationPanel) cls.newInstance();
                appPanel.setDevice(device);
                success = true;
                newTab.add(appPanel);
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
//        plugInMap.get(device).container.setFloatable(false);
        PlugInTab oldTab = plugInMap.remove(device).container;
 //       oldTab.setFloatable(false);
//        oldTab.getParent().remove(oldTab);
//        oldTab.setVisible(false);

//        java.awt.Container container = oldTab.getParent();
//        java.awt.Container lastContainer = container;
//        while(container != null) {
//            lastContainer = container;
//            container = container.getParent();
//        }
//        lastContainer.setVisible(false);
      //  oldTab.getParent().setVisible(false);
        java.awt.Container container = oldTab.getParent();
        container.remove(oldTab);
        container.repaint();
   //     oldTab.getParent().repaint();

        
//        tabbedPane.addContainerListener(new java.awt.event.ContainerListener() {
//
//            PlugInTab tab = oldTab;
//
//            public void componentAdded(ContainerEvent e) {
//                if(tab.equals(e.getComponent())) {
//                    tabbedPane.remove(tab);
//                }
//            }
//
//            public void componentRemoved(ContainerEvent e) {
//                throw new UnsupportedOperationException("Not supported yet.");
//            }
//        });

    //    tabbedPane.remove(oldTab);
    //    tabbedPane.remove( plugInMap.remove(device).container );

    }
    public void handleDeviceEvent(DeviceEvent evt) {
        if(evt.type == evt.type.AttributeRefresh) {
            if(uninitializedApps.contains(evt.getDevice())) {
                addDevice(evt.getDevice());
            }
        }
        plugInMap.get(evt.getDevice()).customPanel.handleDeviceEvent(evt);
    //    plugInMap.get(evt.getDevice()).handleDeviceEvent(evt);
    }
    public void setDeviceManagerStatus(DeviceManager.DeviceManagerStatus status) {

    }

    private boolean isInitialized(Device device) {
        try {
            edu.stanford.atom.sti.corba.Types.TAttribute[] attributes =
                        device.getAttributes();
            return attributes.length > 0;
        } catch(Exception e) {
            return false;
        }
    }

    public boolean isApplication(Device device) {

        String response = device.execute("isApplication");
        if(response != null && response.equals("yes")) {
            return true;
        }
        return false;
//        edu.stanford.atom.sti.corba.Types.TAttribute[] attributes =
//                        device.getAttributes();
//                for(int i = 0; i < attributes.length; i++) {
//                    if(attributes[i].key.equals("STI_Application")) {
//                        return true;
//                    }
//                }
//        return false;
    }


}
