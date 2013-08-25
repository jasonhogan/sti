/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.stanford.atom.sti.client.gui.checktree;

/**
 *
 * @author Jason
 */
public class CheckTreePathEvent extends java.util.EventObject {
    private boolean selected  = false;
    public CheckTreePathEvent(javax.swing.tree.TreePath source, boolean selected) {
        super(source);
        this.selected = selected;
    }
    public boolean isSelected() {
        return selected;
    }
    public javax.swing.tree.TreePath getSourceTreePath() {
        return (javax.swing.tree.TreePath) super.source;
    }
}
