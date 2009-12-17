/** @file TabbedDocument.java
 *  @author Jason Michael Hogan
 *  @brief Source-file for the class TabbedDocument
 *  @section license License
 *
 *  Copyright (C) 2008 Jason Hogan <hogan@stanford.edu>\n
 *  This file is part of the Stanford Timing Interface (STI).
 *
 *  The STI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The STI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the STI.  If not, see <http://www.gnu.org/licenses/>.
 */

package edu.stanford.atom.sti.client.gui.FileEditorTab;

import edu.stanford.atom.sti.RemoteFileServer.NetworkFileChooser.*;
import javax.swing.JScrollPane;
import javax.swing.*;
import java.io.*;
import javax.swing.text.*;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseListener;
import java.awt.event.MouseEvent;
import javax.swing.KeyStroke;
import javax.swing.text.Keymap;
import java.awt.event.KeyEvent;
import java.awt.event.ActionEvent;
import javax.swing.text.TextAction;
import java.util.HashMap;

public class TabbedDocument extends JScrollPane {

    private String tabString = "    ";
    
    public static int untitledDocCount = 0;
    
    private boolean isLocal = true;
    private File localFile = null;
    
    private NetworkFileSystem nfs = null;
    private String path = null;
    
    private String tabFileName = null;
    private int tabIndex;
    private boolean modified = false;
    
    private AbstractDocument abstractDocumentLocal;
    private JTabbedPane parentTabbedPane;

    HashMap<Object, Action> actions;
    
    public TabbedDocument(String Path, NetworkFileSystem networkFileSystem, JTabbedPane tabbedPane, int TabIndex) {
        this(tabbedPane, TabIndex);
        saveDocument(Path, networkFileSystem);
        mainTextPane.getCaretPosition();

    }
    public TabbedDocument(File file, JTabbedPane tabbedPane, int TabIndex) {
        this(tabbedPane, TabIndex);
        saveDocument(file);
    }
    private TabbedDocument(JTabbedPane tabbedPane, int TabIndex) {
        parentTabbedPane = tabbedPane;
        tabIndex = TabIndex;
        initComponents();
        addDocumentListener();

        MouseListener mouseListener = new MouseAdapter() {
            public void mousePressed(MouseEvent e) {
                maybeShowPopup(e);
            }
            public void mouseReleased(MouseEvent e) {
                maybeShowPopup(e);
            }

            private void maybeShowPopup(MouseEvent e) {
                if (e.isPopupTrigger()) {
                    TabbedDocPopupMenu.show(e.getComponent(),
                            e.getX(), e.getY());
                }
            }
        };

        mainTextPane.addMouseListener(mouseListener);

        //Setup the "Tab" keyboard key so it inserts 4 spaces instead of a tab
        Action tabAction = new TextAction("Tab action") {

            public void actionPerformed(ActionEvent e) {
                insertTextAtCursor(tabString);
            }
        };

        //remove old Tab binding
        mainTextPane.getInputMap().put(KeyStroke.getKeyStroke(KeyEvent.VK_TAB, 0),
                            "Tab key");

        //add new Tab binding
        mainTextPane.getActionMap().put("Tab key",
                             tabAction);

        

        actions = createActionTable(mainTextPane);
        
        cutMenuItem.setAction(getActionByName(DefaultEditorKit.cutAction));
        cutMenuItem.setText("Cut");
        copyMenuItem.setAction(getActionByName(DefaultEditorKit.copyAction));
        copyMenuItem.setText("Copy");
        pasteMenuItem.setAction(getActionByName(DefaultEditorKit.pasteAction));
        pasteMenuItem.setText("Paste");
    }


    public Action getCutAction() {
        return getActionByName(DefaultEditorKit.cutAction);
    }
    public Action getCopyAction() {
        return getActionByName(DefaultEditorKit.copyAction);
    }
    public Action getPasteAction() {
        return getActionByName(DefaultEditorKit.pasteAction);
    }

    public void saveDocument(String Path, NetworkFileSystem networkFileSystem) {
        path = Path;
        nfs = networkFileSystem;
        isLocal = false;
        setTabTitle();
    }    
    public void saveDocument(File file) {
        localFile = file;
        isLocal = true;
        setTabTitle();
    }
    private void addDocumentListener() {
        StyledDocument styledDocTemp = mainTextPane.getStyledDocument();
        
        if (styledDocTemp instanceof AbstractDocument) {
            abstractDocumentLocal = (AbstractDocument)styledDocTemp;
            abstractDocumentLocal.addDocumentListener(
                    new TabbedDocumentListener(this));
        } else {
            System.err.println("Text pane's document isn't an AbstractDocument.");
        }        
    }
    public void setModifed(boolean Modified) {
        modified = Modified;
        if(modified) {
            parentTabbedPane.setTitleAt(tabIndex, getTabTitle() + " *");
        }
        else {
            parentTabbedPane.setTitleAt(tabIndex, getTabTitle());
        }
    }
    
    public boolean isModifed() {
        return modified;
    }
    
    public boolean isLocalFile() {
        return isLocal;
    }
    
    public boolean isNetworkFile() {
        return !isLocalFile();
    }
    
    public boolean isNewFile() {
        return (localFile == null && nfs == null);
    }
    
    public boolean canWrite() {
        if(isLocal) {
            if(localFile != null)
                return localFile.canWrite();
            else
                return true;
        }
        else {      //network file
            return ( !nfs.isReadOnly(path) );
        }
    }
    
    public void setTabTitle() {

        if(isLocal) {
            if(localFile != null)
                tabFileName = localFile.getName();
        }
        else {
            if(nfs != null && path != null)
                tabFileName = nfs.shortFileName(path);
        }
        
        if(isNewFile()) {
            untitledDocCount++;
            tabFileName = "Untitled"
                    + Integer.toString(untitledDocCount);
        }
    }

    public String getTabTitle() {
        // Place <> around tab name to distiguish read only
        if(!canWrite())
            return ("<" + tabFileName + ">");
        else if(isLocalFile())
            return (tabFileName + "(local)");
        else
            return tabFileName;
    }

    public String getFileName() {
        return tabFileName;
    }

    public int getTabIndex() {
        return tabIndex;
    }
    public void setTabIndex(int index) {
        tabIndex = index;
    }

    public String getServerAddress() {
        if(isLocalFile())
            return null;
        else
            return nfs.getFullAddress();
    }
    
    public File getFile() {
        return localFile;
    }
    public String getPath() {
        return path;
    }
    @Override
    public String toString() {
        return getTabTitle();
    }
    public NetworkFileSystem getNetworkFileSystem() {
        return nfs;
    }
    public javax.swing.JTextPane getTextPane() {
        return mainTextPane;

    }
    
    public void insertTextAtCursor(String text) {
        try {
            mainTextPane.getStyledDocument().
                    insertString(mainTextPane.getCaretPosition(), text, null);
        } catch (Exception e) {
        }
    }


    public void commentSelection() {
        
        int startLine = -1;
        int endLine = -1;
        int startOfLine = -1;
        
        try {
            startLine = mainTextPane.getLineOfOffset( mainTextPane.getSelectionStart() );
            endLine = mainTextPane.getLineOfOffset(  mainTextPane.getSelectionEnd() );
        } catch(Exception e) {
            return;
        }
        
        if (startLine > -1 && endLine > -1 && startLine <= endLine) {
            
            //insert a '#' at the start of each line
            
            for(int i = startLine; i <= endLine; i++) {
                try {
                    startOfLine = mainTextPane.getLineStartOffset(i);
                    mainTextPane.getStyledDocument().insertString(startOfLine, "#", null);
                } catch (Exception e) {
                }
            }
        }
    }

    public void uncommentSelection() {
        int startLine = -1;
        int endLine = -1;
        int startOfLine = -1;
        int endOfLine = -1;
        String line = "";
        int commentLoc = -1;
        boolean commentFound = false;

        try {
            startLine = mainTextPane.getLineOfOffset( mainTextPane.getSelectionStart() );
            endLine = mainTextPane.getLineOfOffset(  mainTextPane.getSelectionEnd() );
        } catch(Exception e) {
            return;
        }

        if (startLine > -1 && endLine > -1 && startLine <= endLine) {

            //look for a '#' near the start of each line
            for(int i = startLine; i <= endLine; i++) {
                try {
                    startOfLine = mainTextPane.getLineStartOffset(i);
                    endOfLine = mainTextPane.getLineEndOffset(i);
                    line = mainTextPane.getStyledDocument().getText(startOfLine, endOfLine - startOfLine);

                    commentLoc = line.indexOf("#");

                    commentFound = (commentLoc >= 0);
                    //make sure any chars before the '#' are blank
                    for(int j = 0; j < commentLoc; j++ ) {
                        commentFound &= line.substring(j, j + 1).contentEquals(" ");
                    }

                    if(commentFound) {
                        mainTextPane.getStyledDocument().remove(startOfLine + commentLoc, 1);
                    }

                } catch (Exception e) {
                }
            }
        }

    }

    public boolean equals(File file) {
        if(file != null && localFile != null)
            return localFile.equals(file);
        return false;
    }
    public boolean equals(NetworkFileSystem networkFileSystem, String file) {
        if(nfs != null && path != null && networkFileSystem != null && file != null) {
            System.out.println("Network equals: " + nfs.equals(networkFileSystem) + ", " + path.equals(file));
            return (nfs.equals(networkFileSystem) && path.equals(file));
        }
        return false;
    }



    /**
     * Adds assist code and then sets the position of the insertion caret relative
     * to the inserted code.  This is useful for placing the caret inside
     * the inserted code so the user can continue typing.
     *
     * @param relativeCaretPosition the position of the caret relative to the
     *                              start of the inserted code.
     */
    private void insertCodeAssistText(String code, int relativeCaretPosition) {

        int iniPosition = mainTextPane.getCaretPosition();
        insertTextAtCursor(code);
        mainTextPane.setCaretPosition(iniPosition + relativeCaretPosition);
    }

    
        
    private HashMap<Object, Action> createActionTable(JTextComponent textComponent) {
        HashMap<Object, Action> actionsHash = new HashMap<Object, Action>();
        Action[] actionsArray = textComponent.getActions();
        for (int i = 0; i < actionsArray.length; i++) {
            Action a = actionsArray[i];
            actionsHash.put(a.getValue(Action.NAME), a);
        }
	return actionsHash;
    }

    private Action getActionByName(String name) {
        return actions.get(name);
    }



    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        TabbedDocPopupMenu = new javax.swing.JPopupMenu();
        stipycmdsMenu = new javax.swing.JMenu();
        setvarMenuItem = new javax.swing.JMenuItem();
        eventMenuItem = new javax.swing.JMenuItem();
        channelMenuItem = new javax.swing.JMenuItem();
        deviceMenuItem = new javax.swing.JMenuItem();
        jSeparator1 = new javax.swing.JSeparator();
        cutMenuItem = new javax.swing.JMenuItem();
        copyMenuItem = new javax.swing.JMenuItem();
        pasteMenuItem = new javax.swing.JMenuItem();
        jSeparator2 = new javax.swing.JSeparator();
        commentMenuItem = new javax.swing.JMenuItem();
        uncommentMenuItem = new javax.swing.JMenuItem();
        mainTextPane = new edu.stanford.atom.sti.client.gui.FileEditorTab.STITextPane();

        TabbedDocPopupMenu.setMinimumSize(new java.awt.Dimension(10, 10));

        stipycmdsMenu.setMinimumSize(new java.awt.Dimension(10, 10));

        setvarMenuItem.setText("setvar()");
        setvarMenuItem.setToolTipText("setvar(Name, Value)");
        setvarMenuItem.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                setvarMenuItemActionPerformed(evt);
            }
        });
        stipycmdsMenu.add(setvarMenuItem);

        eventMenuItem.setText("event()");
        eventMenuItem.setToolTipText("event(ch(...), Time, Value)");
        eventMenuItem.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                eventMenuItemActionPerformed(evt);
            }
        });
        stipycmdsMenu.add(eventMenuItem);

        channelMenuItem.setText("ch()");
        channelMenuItem.setToolTipText("ch(dev(...), Channel)");
        channelMenuItem.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                channelMenuItemActionPerformed(evt);
            }
        });
        stipycmdsMenu.add(channelMenuItem);

        deviceMenuItem.setText("dev()");
        deviceMenuItem.setToolTipText("dev(Name, IPAddress, Module)");
        deviceMenuItem.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                deviceMenuItemActionPerformed(evt);
            }
        });
        stipycmdsMenu.add(deviceMenuItem);

        stipycmdsMenu.setText("STI Py commands");

        TabbedDocPopupMenu.add(stipycmdsMenu);
        TabbedDocPopupMenu.add(jSeparator1);

        cutMenuItem.setText("Cut");
        cutMenuItem.setToolTipText("");
        TabbedDocPopupMenu.add(cutMenuItem);

        copyMenuItem.setText("jMenuItem1");
        TabbedDocPopupMenu.add(copyMenuItem);

        pasteMenuItem.setText("jMenuItem2");
        TabbedDocPopupMenu.add(pasteMenuItem);
        TabbedDocPopupMenu.add(jSeparator2);

        commentMenuItem.setText("Comment Selection");
        commentMenuItem.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                commentMenuItemActionPerformed(evt);
            }
        });
        TabbedDocPopupMenu.add(commentMenuItem);

        uncommentMenuItem.setText("Uncomment Selection");
        uncommentMenuItem.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                uncommentMenuItemActionPerformed(evt);
            }
        });
        TabbedDocPopupMenu.add(uncommentMenuItem);

        setMinimumSize(new java.awt.Dimension(1, 23));
        setViewportView(mainTextPane);
    }// </editor-fold>//GEN-END:initComponents

    private void setvarMenuItemActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_setvarMenuItemActionPerformed
        insertCodeAssistText("setvar()", 7);
    }//GEN-LAST:event_setvarMenuItemActionPerformed

    private void eventMenuItemActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_eventMenuItemActionPerformed
        insertCodeAssistText("event()", 6);
    }//GEN-LAST:event_eventMenuItemActionPerformed

    private void channelMenuItemActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_channelMenuItemActionPerformed
        insertCodeAssistText("ch()", 3);
    }//GEN-LAST:event_channelMenuItemActionPerformed

    private void deviceMenuItemActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_deviceMenuItemActionPerformed
        insertCodeAssistText("dev()", 4);
    }//GEN-LAST:event_deviceMenuItemActionPerformed

    private void commentMenuItemActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_commentMenuItemActionPerformed
        commentSelection();
    }//GEN-LAST:event_commentMenuItemActionPerformed

    private void uncommentMenuItemActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_uncommentMenuItemActionPerformed
        uncommentSelection();
    }//GEN-LAST:event_uncommentMenuItemActionPerformed
    
    
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JPopupMenu TabbedDocPopupMenu;
    private javax.swing.JMenuItem channelMenuItem;
    private javax.swing.JMenuItem commentMenuItem;
    private javax.swing.JMenuItem copyMenuItem;
    private javax.swing.JMenuItem cutMenuItem;
    private javax.swing.JMenuItem deviceMenuItem;
    private javax.swing.JMenuItem eventMenuItem;
    private javax.swing.JSeparator jSeparator1;
    private javax.swing.JSeparator jSeparator2;
    private edu.stanford.atom.sti.client.gui.FileEditorTab.STITextPane mainTextPane;
    private javax.swing.JMenuItem pasteMenuItem;
    private javax.swing.JMenuItem setvarMenuItem;
    private javax.swing.JMenu stipycmdsMenu;
    private javax.swing.JMenuItem uncommentMenuItem;
    // End of variables declaration//GEN-END:variables
    
}
