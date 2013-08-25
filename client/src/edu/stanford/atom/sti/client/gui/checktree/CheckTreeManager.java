/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.gui.checktree;

import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import javax.swing.JTree;
import javax.swing.JCheckBox;
import javax.swing.tree.*;
import javax.swing.event.*;
import java.util.Vector;

/**
 *
 * @author Santhosh Kumar T - santhosh@in.fiorano.com
 * Original author.
 */

/**
 * 
 * @author Jason
 * 8-24-2013:  Added CheckTreePathEvent and CheckTreePathEventListener mechanism
 * for responding to changes in the tree check boxes status.
 * Also added the checkChanged() function to simulate the effect of a mouse click
 * on the selection model.
 */

public class CheckTreeManager extends MouseAdapter implements TreeSelectionListener{
    private CheckTreeSelectionModel selectionModel;
    private JTree tree = new JTree();
    int hotspot = new JCheckBox().getPreferredSize().width;

    private Vector<CheckTreePathListener> listeners = new Vector<CheckTreePathListener>();
    
    public CheckTreeManager(JTree tree){
        this.tree = tree;
        selectionModel = new CheckTreeSelectionModel(tree.getModel());
        tree.setCellRenderer(new CheckTreeCellRenderer(tree.getCellRenderer(), selectionModel));
        tree.addMouseListener(this);
        selectionModel.addTreeSelectionListener(this);
    }

    //Call when the checkbox associated with TreePath path
    //has changed state. Fires CheckTreePathEvents to all listeners
    public void checkChanged(TreePath path) {
        if(path == null) {
            return;
        }
        
        boolean selected = selectionModel.isPathSelected(path, true);
        selectionModel.removeTreeSelectionListener(this);

        try{
            if(selected) {
                selectionModel.removeSelectionPath(path);
                fireCheckTreePathChange(new CheckTreePathEvent(path, false));
            } else {
                selectionModel.addSelectionPath(path);
                fireCheckTreePathChange(new CheckTreePathEvent(path, true));
            }
        } finally{
            selectionModel.addTreeSelectionListener(this);
            tree.treeDidChange();
        }
    }
    
    public void mouseClicked(MouseEvent me){
        TreePath path = tree.getPathForLocation(me.getX(), me.getY());
        if(path == null) {
            return;
        }
        if(me.getX() > tree.getPathBounds(path).x + hotspot) {
            return;
        }
        
        checkChanged(path);
    }
    
    private synchronized void fireCheckTreePathChange(CheckTreePathEvent evt) {
        for(CheckTreePathListener listener : listeners) {
            listener.pathChanged(evt);
        }
    }
    public synchronized void addCheckTreePathListener(CheckTreePathListener listener) {
        listeners.add(listener);
    }
    public synchronized void removeCheckTreePathListener(CheckTreePathListener listener) {
        listeners.remove(listener);
    }
    
    public CheckTreeSelectionModel getSelectionModel(){
        return selectionModel;
    }

    public void valueChanged(TreeSelectionEvent e){
        tree.treeDidChange();
    }
}
