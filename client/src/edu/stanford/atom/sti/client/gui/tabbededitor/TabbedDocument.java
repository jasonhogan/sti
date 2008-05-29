/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.gui.tabbededitor;

import javax.swing.*;
import java.io.*;
import javax.swing.text.*;

/**
 *
 * @author Owner
 */
public class TabbedDocument {
    

    public TabbedDocument(File file, TabbedEditor tabbedEditor, int TabIndex) {

        fileLocal = file;
        setTabIndex(TabIndex);
        tabbedEditorLocal = tabbedEditor;
        
        setTabTitle(file);
        
        textPaneLocal = new javax.swing.JTextPane();
        caretListenerLocal = 
                new CaretListenerLabel(tabbedEditor.caretPositionLabelLocal, 
                textPaneLocal);       
        
        StyledDocument styledDocTemp = textPaneLocal.getStyledDocument();
        if (styledDocTemp instanceof AbstractDocument) {
            abstractDocumentLocal = (AbstractDocument)styledDocTemp;
            abstractDocumentLocal.addDocumentListener(
                    new tabDocumentListener(tabbedEditorLocal, this));
        } else {
            System.err.println("Text pane's document isn't an AbstractDocument!");
            System.exit(-1);
        } 

    
    }

    public javax.swing.JScrollPane generateTab() {

        scrollPaneLocal = new javax.swing.JScrollPane();
        scrollPaneLocal.setViewportView(textPaneLocal);
        textPaneLocal.addCaretListener(caretListenerLocal);    
        return scrollPaneLocal;
    }
    public void setFile(File file) {
        fileLocal = file;
    }
    public File getFile() {
        return fileLocal;
    }
    public void setTabTitle(File file) {

        if (fileLocal == null) {
            tabFileName = "Untitled"
                    + Integer.toString(tabbedEditorLocal.getUntitledDocCount());
        } else {
            if (file.canWrite() == false) {
                // Place <> around tab name to distiguish read only
                tabFileName = "<" + file.getName() + ">";
            } else {
                tabFileName = file.getName();
            }
            
        }
    }
    public String getTabTitle() {
        return tabFileName;
    }
    public int getTabIndex() {
        return tabIndex;
    }
    public void setTabIndex(int index) {
        tabIndex = index;
    }
    public javax.swing.JTextPane getTextPane() {
        return textPaneLocal;
    }
    
    /**
     * Returns this document's scroll pane
     * @return
     */
    public javax.swing.JScrollPane getScrollPane() {
        return scrollPaneLocal;
    }
    
    public void setModifed(boolean Modified) {
        modified = Modified;
    }
    
    public boolean isModifed() {
        return modified;
    }
    
    public String getCaretLabelText() {
        return caretListenerLocal.getText();
    }
    
    
    private String tabFileName;
    private javax.swing.JScrollPane scrollPaneLocal = new javax.swing.JScrollPane();
    private javax.swing.JTextPane textPaneLocal = new javax.swing.JTextPane();
    
    private File fileLocal;
    private int tabIndex;
    private boolean modified;
    
    private CaretListenerLabel caretListenerLocal;
    private AbstractDocument abstractDocumentLocal;
    
    private TabbedEditor tabbedEditorLocal;
    
}
