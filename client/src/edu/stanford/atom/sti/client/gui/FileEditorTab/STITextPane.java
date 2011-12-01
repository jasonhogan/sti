/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.gui.FileEditorTab;

//import java.awt.*;
//import java.awt.event.*;
import javax.swing.text.*;
//import javax.swing.plaf.*;
//import javax.accessibility.*;

import org.fife.ui.rsyntaxtextarea.RSyntaxTextArea;

/**
 *
 * @author Owner
 */

//javax.swing.JTextPane
import org.fife.ui.rsyntaxtextarea.RSyntaxDocument;

public class STITextPane extends RSyntaxTextArea {
        
    public STITextPane() {
        super();
    }
    
    public STITextPane(RSyntaxDocument doc) {
        super(doc);
    }

//    @Override
//    public boolean getScrollableTracksViewportWidth() {
//        return false ; // Returning false for non-wrapping
//    }

    public int getLineNumber() {
        int lineNumber = 1;
        try {
            int position = getCaretPosition();
            lineNumber = getLineOfOffset(position);
            //columnNumber = position - textPane.getLineStartOffset(lineNumber);
            lineNumber += 1;
        } catch (Exception ex) {
        }
        return lineNumber;
    }

    public int getColumnNumber() {
        int columnNumber = 1;
        try {
            int position = getCaretPosition();
            columnNumber = position - getLineStartOffset( getLineOfOffset(position) );
            columnNumber += 1;
        } catch (Exception ex) {
        }
        return columnNumber;
    }

    /**
     * Determines the number of lines contained in the area.
     *
     * @return the number of lines > 0
     */
    public int getLineCount() {
        Element map = getDocument().getDefaultRootElement();
        return map.getElementCount();
    }
    
    /**
     * Translates an offset into the components text to a 
     * line number.
     *
     * @param offset the offset >= 0
     * @return the line number >= 0
     * @exception BadLocationException thrown if the offset is
     *   less than zero or greater than the document length.
     */
    public int getLineOfOffset(int offset) throws BadLocationException {
        Document doc = getDocument();
        if (offset < 0) {
            throw new BadLocationException("Can't translate offset to line", -1);
        } else if (offset > doc.getLength()) {
            throw new BadLocationException("Can't translate offset to line", doc.getLength()+1);
        } else {
            Element map = getDocument().getDefaultRootElement();
            return map.getElementIndex(offset);
        }
    }
    
    /**
     * Determines the offset of the start of the given line.
     *
     * @param line  the line number to translate >= 0
     * @return the offset >= 0
     * @exception BadLocationException thrown if the line is
     * less than zero or greater or equal to the number of
     * lines contained in the document (as reported by 
     * getLineCount).
     */
    public int getLineStartOffset(int line) throws BadLocationException {
        int lineCount = getLineCount();
        if (line < 0) {
            throw new BadLocationException("Negative line", -1);
        } else if (line >= lineCount) {
            throw new BadLocationException("No such line", getDocument().getLength()+1);
        } else {
            Element map = getDocument().getDefaultRootElement();
            Element lineElem = map.getElement(line);
            return lineElem.getStartOffset();
        }
    }


    /**
     * Determines the offset of the end of the given line.
     *
     * @param line  the line number to translate >= 0
     * @return the offset >= 0
     * @exception BadLocationException thrown if the line is
     * less than zero or greater or equal to the number of
     * lines contained in the document (as reported by
     * getLineCount).
     */
    public int getLineEndOffset(int line) throws BadLocationException {
        int lineCount = getLineCount();
        if (line < 0) {
            throw new BadLocationException("Negative line", -1);
        } else if (line >= lineCount) {
            throw new BadLocationException("No such line", getDocument().getLength()+1);
        } else {
            Element map = getDocument().getDefaultRootElement();
            Element lineElem = map.getElement(line);
            return lineElem.getEndOffset();
        }
    }


}
