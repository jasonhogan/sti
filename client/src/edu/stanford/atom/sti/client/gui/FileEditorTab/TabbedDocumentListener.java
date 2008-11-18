/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.stanford.atom.sti.client.gui.FileEditorTab;

//import javax.swing.*;
import javax.swing.text.*;
import javax.swing.event.*;

/**
 *
 * @author Owner
 */

public class TabbedDocumentListener
        implements DocumentListener {

    private TabbedDocument document;
    
    public TabbedDocumentListener(TabbedDocument tabbedDocument) {
        document = tabbedDocument;
    }
    
    public void insertUpdate(DocumentEvent e) {
        documentIsChanged(e);
    }

    public void removeUpdate(DocumentEvent e) {
        documentIsChanged(e);
    }

    public void changedUpdate(DocumentEvent e) {
        documentIsChanged(e);
    }

    private void documentIsChanged(DocumentEvent e) {
       // tabbedEditorLocal.tabIsModified(tabbedDocumentLocal.getTabIndex());
        document.setModifed(true);
    }
}
