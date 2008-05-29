/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.stanford.atom.sti.client.gui.tabbededitor;

//import javax.swing.*;
import javax.swing.text.*;
import javax.swing.event.*;

/**
 *
 * @author Owner
 */

//And this one listens for any changes to the document.
public class tabDocumentListener
        implements DocumentListener {

    public tabDocumentListener(TabbedEditor tabbedEditor, 
            TabbedDocument tabbedDocument){
        tabbedEditorLocal = tabbedEditor;
        tabbedDocumentLocal = tabbedDocument;
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
        tabbedEditorLocal.tabIsModified(tabbedDocumentLocal.getTabIndex());
        tabbedDocumentLocal.setModifed(true);
    }
    
    private void displayEditInfo(DocumentEvent e) {
        Document document = e.getDocument();
        int changeLength = e.getLength();
    }
    
    private TabbedEditor tabbedEditorLocal;
    private TabbedDocument tabbedDocumentLocal;
}
