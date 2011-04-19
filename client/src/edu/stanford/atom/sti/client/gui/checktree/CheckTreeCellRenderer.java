/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.gui.checktree;

import javax.swing.tree.*;
import javax.swing.JPanel;
import javax.swing.JTree;
import java.awt.BorderLayout;
import java.awt.Component;

/**
 *
 * @author Santhosh Kumar T - santhosh@in.fiorano.com
 */
public class CheckTreeCellRenderer extends JPanel implements TreeCellRenderer{
    private CheckTreeSelectionModel selectionModel;
    private TreeCellRenderer delegate;
    private TristateCheckBox checkBox = new TristateCheckBox("");

    public CheckTreeCellRenderer(TreeCellRenderer delegate, CheckTreeSelectionModel selectionModel){
        this.delegate = delegate;
        this.selectionModel = selectionModel;
        setLayout(new BorderLayout());
        setOpaque(false);
        checkBox.setOpaque(false);
    }


    public Component getTreeCellRendererComponent(JTree tree, Object value, boolean selected, boolean expanded, boolean leaf, int row, boolean hasFocus){
        Component renderer = delegate.getTreeCellRendererComponent(tree, value, selected, expanded, leaf, row, hasFocus);

        TreePath path = tree.getPathForRow(row);
        if(path!=null){
            if(selectionModel.isPathSelected(path, true))
                checkBox.getTristateModel().setSelected(true);
                //checkBox.setState(Boolean.TRUE);
            else if (selectionModel.isPartiallySelected(path))
                checkBox.getTristateModel().setIndeterminate();
            else
                checkBox.getTristateModel().setSelected(false);
        }
        removeAll();
        add(checkBox, BorderLayout.WEST);
        add(renderer, BorderLayout.CENTER);
        return this;
    }
}
