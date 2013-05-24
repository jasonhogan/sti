/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.stanford.atom.sti.client.gui.FileEditorTab;

import edu.stanford.atom.sti.corba.Types.TTag;
/**
 *
 * @author Jason
 */
public class TextTag extends java.lang.Object {
    
    public TextTag(TTag tag, String filenameFullPath, String mainFilenameFullPath) {
        name = tag.name;
        filename = filenameFullPath;
        mainFilename = mainFilenameFullPath;
        lineNumber = tag.pos.line;
        time = tag.time;
    }
    @Override
    public String toString() {
        return name;
    }
    public String getName() {
        return name;
    }
    public String getFilename() {
        return filename;
    }
    public String getMainFilename() {
        return mainFilename;
    }
    public int getLineNumber() {
        return lineNumber;
    }
    public double getTime() {
        return time;
    }
    
    private String name = null;
    private String filename = null;
    private String mainFilename = null;
    private int lineNumber = 0;
    private double time = 0;
    
}
