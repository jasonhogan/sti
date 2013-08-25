/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.stanford.atom.sti.client.gui.DevicesTab;

import edu.stanford.atom.sti.client.comm.bl.device.Device;
import edu.stanford.atom.sti.client.gui.checktree.CheckTreeManager;
import edu.stanford.atom.sti.client.gui.checktree.CheckTreePathEvent;
import edu.stanford.atom.sti.client.gui.checktree.CheckTreePathListener;
import java.util.Enumeration;
import java.util.Hashtable;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreePath;
import javax.swing.JTree;
import javax.swing.tree.MutableTreeNode;

/**
 *
 * @author Jason
 */
public class DeviceTabSelectorTreeManager {
    
    private JTree deviceTree;
    private CheckTreeManager checkTreeManager;
    private Hashtable<TreePath, Device> treeMap = new Hashtable<TreePath, Device>();
    private DeviceTabSelectorListener deviceTabSelector;

    
    public DeviceTabSelectorTreeManager(JTree deviceTree,
            DeviceTabSelectorListener deviceTabSelector) {
        this.deviceTree = deviceTree;
        checkTreeManager = new CheckTreeManager(deviceTree);
        this.deviceTabSelector = deviceTabSelector;

        addListeners();
    }
        
    private void addListeners() {
        //Listen for check box change events on the IP tree
        checkTreeManager.addCheckTreePathListener(new CheckTreePathListener() {
            public void pathChanged(CheckTreePathEvent evt) {
                if (!evt.isSelected()) {
                    toggleDeviceTab(evt.getSourceTreePath(), false);
                } else {
                    toggleDeviceTab(evt.getSourceTreePath(), true);
                }
            }
        });
        
        //Listen for double click events on the IP tree
        java.awt.event.MouseListener ml = new java.awt.event.MouseAdapter() {
            @Override
            public void mousePressed(java.awt.event.MouseEvent e) {
                int selRow = deviceTree.getRowForLocation(e.getX(), e.getY());
                TreePath selPath = deviceTree.getPathForLocation(e.getX(), e.getY());
                if (selRow != -1) {
                    if (e.getClickCount() == 2) {
                        boolean checkboxIsChecked = isTreePathChecked(selPath);

                        //If not checked, pass the mouse event on to the tree (hacky...)
                        if (!checkboxIsChecked) {
                            //Move the mouse event to the checkbox location
                            e.translatePoint(deviceTree.getPathBounds(selPath).x - e.getX(), 0);      //(delta X, delta Y)
                            checkTreeManager.mouseClicked(e);

                            selectDeviceTabUsingTreePath(selPath);
                        }
                    }
                }
            }
        };
        deviceTree.addMouseListener(ml);
    }
        
    public void selectDeviceTabUsingTreePath(TreePath path) {
        Device device = treeMap.get(path);
        deviceTabSelector.selectDeviceTabByDevice(device);
    }
    
    private void toggleDeviceTab(TreePath path, boolean show) {
        MutableTreeNode aNode = (MutableTreeNode)path.getLastPathComponent();
        if(aNode.getChildCount() == 0) {
            deviceTabSelector.toggleDeviceTab(treeMap.get(path), show);
        } else {
            Enumeration e = aNode.children();
            MutableTreeNode node;
            while (e.hasMoreElements()) {
                TreePath subPath = new TreePath(((DefaultMutableTreeNode)(e.nextElement())).getPath());
                toggleDeviceTab(subPath, show);
            }
        }
    }
    
    public void addDeviceToTree(Device device) {
        
        DefaultTreeModel treeModel = (DefaultTreeModel) deviceTree.getModel();
        DefaultMutableTreeNode rootNode = (DefaultMutableTreeNode) treeModel.getRoot();
        
        DefaultMutableTreeNode newDeviceNode = 
                new DefaultMutableTreeNode(getTreeDeviceName(device));
        DefaultMutableTreeNode groupNode = new DefaultMutableTreeNode(getTreeGroupName(device));
        
        int groupIndex = findChildNodeIndexByString(rootNode, groupNode);
        if(groupIndex < 0) {
            //Not found; adding
            rootNode.add(groupNode);
            treeModel.reload(rootNode);
        } else if(groupIndex < rootNode.getChildCount()) {
            groupNode = (DefaultMutableTreeNode) rootNode.getChildAt(groupIndex);
        } else {
            groupNode = null;
        }
        
        if(groupNode != null) {
            groupNode.add(newDeviceNode);
            treeModel.reload(groupNode);
            
            TreePath path = new TreePath(newDeviceNode.getPath());
            treeMap.put(path, device);
            
            checkTreeManager.checkChanged(path);
            
            boolean selected = isTreePathChecked(path);
            try {
                if(!selected) {
                    checkTreeManager.getSelectionModel().addSelectionPath(path);
                }
            } finally {
                deviceTree.treeDidChange();
            }
        }

    }
    
    public void removeDeviceFromTree(Device device) {
        DefaultTreeModel treeModel = (DefaultTreeModel) deviceTree.getModel();
        DefaultMutableTreeNode rootNode = (DefaultMutableTreeNode) treeModel.getRoot();

        DefaultMutableTreeNode deviceNode = new DefaultMutableTreeNode(getTreeDeviceName(device));
        DefaultMutableTreeNode groupNode = new DefaultMutableTreeNode(getTreeGroupName(device));
        
        int groupIndex = findChildNodeIndexByString(rootNode, groupNode);
        if(groupIndex < 0) {
            groupNode = null;
        } else {
            groupNode = (DefaultMutableTreeNode) rootNode.getChildAt(groupIndex);
        }
        
        if(groupNode != null) {            
            int deviceIndex = findChildNodeIndexByString(groupNode, deviceNode);
            
            if(groupNode.getChildCount() < 2 && deviceIndex >= 0) {
                //When asked to remove the last device under this group, just remove the group.
                TreePath path = new TreePath(((DefaultMutableTreeNode)groupNode.getChildAt(deviceIndex)).getPath());
                treeMap.remove(path);
                
                rootNode.remove(groupIndex);
                treeModel.reload(rootNode);
                deviceTree.treeDidChange();
            }
            else if (deviceIndex >= 0 && deviceIndex < groupNode.getChildCount()) {
                //Remove the device (unselect then remove from tree)
                
                TreePath path = new TreePath(
                        ((DefaultMutableTreeNode)groupNode.getChildAt(deviceIndex)).getPath());
                treeMap.remove(path);
                
               //Unselect
                boolean selected = isTreePathChecked(path);
                try {
                    if (selected) {
                        checkTreeManager.getSelectionModel().removeSelectionPath(path);
                    }
                } finally {
                    deviceTree.treeDidChange();
                }
                
                groupNode.remove(deviceIndex);
                treeModel.reload(groupNode);
            }
        }
    }
    
    public String getTreeDeviceName(Device device) {
        if(device != null) {
            return device.name() + " (Module " + device.module() + ")";
        } else {
            return "";
        }
    }
    public String getTreeGroupName(Device device) {
        if(device != null) {
            return device.address().toLowerCase();
        } else {
            return "";
        }
    }
        
    public boolean isTreePathChecked(TreePath path) {
        return checkTreeManager.getSelectionModel().isPathSelected(path, true);
    }
       
    private int findChildNodeIndexByString(DefaultMutableTreeNode parentNode, DefaultMutableTreeNode newNode) {
        
        int index = -1; //default = not found
        
        if (newNode.toString() == null)   {
            return index;
        }
        
        DefaultMutableTreeNode node = null;
        Enumeration e = parentNode.children();
        
        int i = 0;
        while (e.hasMoreElements()) {
            node = (DefaultMutableTreeNode) e.nextElement();
            if (newNode.toString().equals(node.toString())) {
                index = i;
                break;
            }
            i++;
        }

        return index;
    }

}
