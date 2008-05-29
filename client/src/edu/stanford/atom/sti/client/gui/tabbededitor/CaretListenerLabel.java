/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.stanford.atom.sti.client.gui.tabbededitor;

import javax.swing.*;

import java.awt.*;
import java.awt.event.*;
import java.util.HashMap;

import javax.swing.text.*;
import javax.swing.event.*;
import javax.swing.undo.*;

/**
 *
 * @author Owner
 * 
 * The Sun Java Tutorials -- "TextComponentDemo.java"
 */

//This listens for and reports caret movements.
public class CaretListenerLabel
        implements CaretListener {

    public CaretListenerLabel(JLabel jLabel, JTextPane jTextPane) {
    //    super(label);
        jLabelLocal = jLabel;
        jTextPaneLocal = jTextPane;
    }

    //Might not be invoked from the event dispatch thread.
    public void caretUpdate(CaretEvent e) {
        generateCaretString(e.getDot(), e.getMark());
   //     generateSelectionInfoString(e.getDot(), e.getMark());
        jLabelLocal.setText(getText());
    }

    private void generateCaretString(int dot, int mark) {
        if (dot == mark) {  // no selection
            try {
                Rectangle caretCoords = jTextPaneLocal.modelToView(dot);
                //Convert it to view coordinates.
          //      setText(dot + " [" + caretCoords.x + ", " + caretCoords.y + "]" + newline);
                // Is this because the caret position labeller is broken?
                setText("broken");
            } catch (BadLocationException ble) {
                setText("caret: text position: " + dot + newline);
            }
        } else if (dot < mark) {
            setText(dot + " to " + mark + newline);
        } else {
            setText(mark + " to " + dot + newline);
        }
    }
    
    //This method can be invoked from any thread.  It 
    //invokes the setText and modelToView methods, which 
    //must run on the event dispatch thread. We use
    //invokeLater to schedule the code for execution
    //on the event dispatch thread.
    protected void generateSelectionInfoString(final int dot,
            final int mark) {
        SwingUtilities.invokeLater(new Runnable() {

            public void run() {
                if (dot == mark) {  // no selection
                    try {
                        Rectangle caretCoords = jTextPaneLocal.modelToView(dot);
                        //Convert it to view coordinates.
                        setText(dot + " [" + caretCoords.x + ", " + caretCoords.y + "]" + newline);
                    } catch (BadLocationException ble) {
                        setText("caret: text position: " + dot + newline);
                    }
                } else if (dot < mark) {
                    setText(dot + " to " + mark + newline);
                } else {
                    setText(mark + " to " + dot + newline);
                }
            }
            });
    
    }
    public void setText(String text) {
        caretString = text;
    }
    
    public String getText() {
        return caretString;
    }
    
    private String caretString = "none";
    private javax.swing.JTextPane jTextPaneLocal;
    private javax.swing.JLabel jLabelLocal;
    private String newline = "\n";
    }
