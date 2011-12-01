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

import org.fife.ui.rtextarea.RTextScrollPane;
import org.fife.ui.rsyntaxtextarea.*;
import org.fife.ui.rsyntaxtextarea.RSyntaxDocument;
import org.fife.ui.rtextarea.SearchEngine;

public class TabbedDocument extends RTextScrollPane {

    private String tabString = "    ";
    
    public static int untitledDocCount = 0;
    
    private boolean isLocal = true;
    private File localFile = null;
    
    private NetworkFileSystem nfs = null;
    private String path = null;
    
    private String tabFileName = null;
    private int tabIndex;
    private boolean modified = false;

    private int fontSize = 14;
    
    private AbstractDocument abstractDocumentLocal;
    private JTabbedPane parentTabbedPane;

    HashMap<Object, Action> actions;

//    private RSyntaxTextArea syntaxTextArea = new RSyntaxTextArea();
    
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

        //mainTextPane.undoLastAction();
        //((org.fife.ui.rtextarea.RTextArea)mainTextPane).canUndo();
        initComponents();
        mainTextPane.setSyntaxEditingStyle(SyntaxConstants.SYNTAX_STYLE_PYTHON);

//        mainTextPane.getComponentPopupMenu().addSeparator();
//        mainTextPane.getComponentPopupMenu().add(stipycmdsMenu);
//
//        mainTextPane.getComponentPopupMenu().addSeparator();
//        mainTextPane.getComponentPopupMenu().add(commentMenuItem);
//        mainTextPane.getComponentPopupMenu().add(uncommentMenuItem);



        //((RSyntaxDocument)mainTextPane.getDocument()).;
//        org.fife.ui.rsyntaxtextarea.modes.PythonTokenMaker tokenMaker = new org.fife.ui.rsyntaxtextarea.modes.PythonTokenMaker();
        
//        String test = "event";
//        tokenMaker.a
        //tokenMaker.addToken(test.toCharArray(), WIDTH, WIDTH, tabIndex, tabIndex);
//        mainTextPane.undoLastAction();
//        setViewportView(syntaxTextArea);


        MouseListener mouseListener = new MouseAdapter() {
            public void mousePressed(MouseEvent e) {
                maybeShowPopup(e);
            }
            public void mouseReleased(MouseEvent e) {
                maybeShowPopup(e);
            }

            private void maybeShowPopup(MouseEvent e) {
                if (e.isPopupTrigger()) {
//                    mainTextPane.setCaretPosition(0);
  //                  mainTextPane.getCaret().isSelectionVisible();
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

    public void setFontSize(int size) {
        mainTextPane.setFont(new java.awt.Font("Times New Roman", 0, size));
        fontSize = size;
    }

    public int getFontSize() {
        return fontSize;
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

    public void installOpenedTextIntoDoc(String text) {
        mainTextPane.setText(text);
        mainTextPane.discardAllEdits();
    }

    public void resetUndoBuffer() {
        mainTextPane.discardAllEdits();
    }

    public void saveDocument(String Path, NetworkFileSystem networkFileSystem) {
        path = Path;
        nfs = networkFileSystem;
        isLocal = false;
        setTabTitle();
        mainTextPane.discardAllEdits();
    }    
    public void saveDocument(File file) {
        localFile = file;
        isLocal = true;
        setTabTitle();
        mainTextPane.discardAllEdits();
    }
    private void addDocumentListener() {
        RSyntaxDocument styledDocTemp = (RSyntaxDocument) mainTextPane.getDocument();


        if (styledDocTemp instanceof RSyntaxDocument) {
            abstractDocumentLocal = (RSyntaxDocument)styledDocTemp;
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
    
    public javax.swing.JTextArea getTextPane() {
        return mainTextPane;

    }
    
    public void insertTextAtCursor(String text) {
        try {
            mainTextPane.getDocument().
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
                    mainTextPane.getDocument().insertString(startOfLine, "#", null);
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
                    line = mainTextPane.getDocument().getText(startOfLine, endOfLine - startOfLine);

                    commentLoc = line.indexOf("#");

                    commentFound = (commentLoc >= 0);
                    //make sure any chars before the '#' are blank
                    for(int j = 0; j < commentLoc; j++ ) {
                        commentFound &= line.substring(j, j + 1).contentEquals(" ");
                    }

                    if(commentFound) {
                        mainTextPane.getDocument().remove(startOfLine + commentLoc, 1);
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
        jSeparator3 = new javax.swing.JPopupMenu.Separator();
        undoMenuItem = new javax.swing.JMenuItem();
        redoMenuItem = new javax.swing.JMenuItem();
        jSeparator6 = new javax.swing.JPopupMenu.Separator();
        cutMenuItem = new javax.swing.JMenuItem();
        copyMenuItem = new javax.swing.JMenuItem();
        pasteMenuItem = new javax.swing.JMenuItem();
        jSeparator5 = new javax.swing.JPopupMenu.Separator();
        findMenuItem = new javax.swing.JMenuItem();
        replaceMenuItem = new javax.swing.JMenuItem();
        jSeparator4 = new javax.swing.JPopupMenu.Separator();
        commentMenuItem = new javax.swing.JMenuItem();
        uncommentMenuItem = new javax.swing.JMenuItem();
        findDialog = new javax.swing.JDialog();
        findTextField = new javax.swing.JTextField();
        findPreviousButton = new javax.swing.JButton();
        findNextButton = new javax.swing.JButton();
        matchCaseCheck = new javax.swing.JCheckBox();
        wholeWordCheck = new javax.swing.JCheckBox();
        findCurrentFileButton = new javax.swing.JRadioButton();
        findAllFilesButton = new javax.swing.JRadioButton();
        regexCheck = new javax.swing.JCheckBox();
        findContextButtonGroup = new javax.swing.ButtonGroup();
        replaceDialog = new javax.swing.JDialog();
        replaceFindTextField = new javax.swing.JTextField();
        jLabel1 = new javax.swing.JLabel();
        jLabel2 = new javax.swing.JLabel();
        replaceReplaceTextField = new javax.swing.JTextField();
        replaceMatchCaseCheck = new javax.swing.JCheckBox();
        replaceWholeWordCheck = new javax.swing.JCheckBox();
        replaceRegexCheck = new javax.swing.JCheckBox();
        replaceWrapAroundCheck = new javax.swing.JCheckBox();
        incrementalSearchCheck = new javax.swing.JCheckBox();
        findButton = new javax.swing.JButton();
        replaceButton = new javax.swing.JButton();
        replaceAllButton = new javax.swing.JButton();
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
        TabbedDocPopupMenu.add(jSeparator3);

        undoMenuItem.setText("Undo");
        undoMenuItem.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                undoMenuItemActionPerformed(evt);
            }
        });
        TabbedDocPopupMenu.add(undoMenuItem);

        redoMenuItem.setText("Redo");
        redoMenuItem.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                redoMenuItemActionPerformed(evt);
            }
        });
        TabbedDocPopupMenu.add(redoMenuItem);
        TabbedDocPopupMenu.add(jSeparator6);

        cutMenuItem.setText("Cut");
        cutMenuItem.setToolTipText("");
        TabbedDocPopupMenu.add(cutMenuItem);

        copyMenuItem.setText("Copy");
        TabbedDocPopupMenu.add(copyMenuItem);

        pasteMenuItem.setText("Paste");
        TabbedDocPopupMenu.add(pasteMenuItem);
        TabbedDocPopupMenu.add(jSeparator5);

        findMenuItem.setText("Find...");
        findMenuItem.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                findMenuItemActionPerformed(evt);
            }
        });
        TabbedDocPopupMenu.add(findMenuItem);

        replaceMenuItem.setText("Replace...");
        replaceMenuItem.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                replaceMenuItemActionPerformed(evt);
            }
        });
        TabbedDocPopupMenu.add(replaceMenuItem);

        jSeparator4.setPreferredSize(new java.awt.Dimension(0, 2));
        TabbedDocPopupMenu.add(jSeparator4);

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

        findDialog.setTitle("Find");
        findDialog.setAlwaysOnTop(true);
        findDialog.setMinimumSize(new java.awt.Dimension(540, 130));
        findDialog.setName("Find"); // NOI18N
        findDialog.setResizable(false);

        findTextField.setFont(new java.awt.Font("Tahoma", 0, 12)); // NOI18N
        findTextField.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                findTextFieldActionPerformed(evt);
            }
        });

        findPreviousButton.setText("Find Previous");
        findPreviousButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                findPreviousButtonActionPerformed(evt);
            }
        });

        findNextButton.setText("Find Next");
        findNextButton.setMaximumSize(new java.awt.Dimension(97, 23));
        findNextButton.setMinimumSize(new java.awt.Dimension(97, 23));
        findNextButton.setPreferredSize(new java.awt.Dimension(97, 23));
        findNextButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                findNextButtonActionPerformed(evt);
            }
        });

        matchCaseCheck.setText("Match Case");

        wholeWordCheck.setText("Whole Word");

        findContextButtonGroup.add(findCurrentFileButton);
        findCurrentFileButton.setSelected(true);
        findCurrentFileButton.setText("Current File");
        findCurrentFileButton.setToolTipText("Search in the currently selected file.");
        findCurrentFileButton.setFocusable(false);
        findCurrentFileButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                findCurrentFileButtonActionPerformed(evt);
            }
        });

        findContextButtonGroup.add(findAllFilesButton);
        findAllFilesButton.setText("All Open Files");
        findAllFilesButton.setEnabled(false);
        findAllFilesButton.setFocusable(false);

        regexCheck.setText("Regular Expression");

        javax.swing.GroupLayout findDialogLayout = new javax.swing.GroupLayout(findDialog.getContentPane());
        findDialog.getContentPane().setLayout(findDialogLayout);
        findDialogLayout.setHorizontalGroup(
            findDialogLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(findDialogLayout.createSequentialGroup()
                .addGroup(findDialogLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(findDialogLayout.createSequentialGroup()
                        .addGap(356, 356, 356)
                        .addComponent(findCurrentFileButton)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                        .addComponent(findAllFilesButton))
                    .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, findDialogLayout.createSequentialGroup()
                        .addContainerGap()
                        .addGroup(findDialogLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                            .addComponent(findTextField, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, 607, Short.MAX_VALUE)
                            .addGroup(javax.swing.GroupLayout.Alignment.LEADING, findDialogLayout.createSequentialGroup()
                                .addComponent(findPreviousButton)
                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                                .addComponent(findNextButton, javax.swing.GroupLayout.PREFERRED_SIZE, 91, javax.swing.GroupLayout.PREFERRED_SIZE)
                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                                .addComponent(matchCaseCheck)
                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                                .addComponent(wholeWordCheck)
                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                                .addComponent(regexCheck)
                                .addGap(33, 33, 33)))))
                .addContainerGap())
        );
        findDialogLayout.setVerticalGroup(
            findDialogLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(findDialogLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(findDialogLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(findAllFilesButton)
                    .addComponent(findCurrentFileButton))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(findTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(findDialogLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                    .addGroup(findDialogLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                        .addComponent(wholeWordCheck)
                        .addComponent(regexCheck))
                    .addComponent(matchCaseCheck)
                    .addComponent(findPreviousButton)
                    .addComponent(findNextButton, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addContainerGap(99, Short.MAX_VALUE))
        );

        replaceDialog.setMinimumSize(new java.awt.Dimension(520, 220));
        replaceDialog.setName("Replace"); // NOI18N
        replaceDialog.setResizable(false);

        replaceFindTextField.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                replaceFindTextFieldActionPerformed(evt);
            }
        });

        jLabel1.setText("Find What:");

        jLabel2.setText("Replace With:");

        replaceReplaceTextField.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                replaceReplaceTextFieldActionPerformed(evt);
            }
        });

        replaceMatchCaseCheck.setText("Match Case");

        replaceWholeWordCheck.setText("Whole Word");

        replaceRegexCheck.setText("Regular Expression");

        replaceWrapAroundCheck.setSelected(true);
        replaceWrapAroundCheck.setText("Wrap Around");

        incrementalSearchCheck.setSelected(true);
        incrementalSearchCheck.setText("Incremental Search");

        findButton.setText("Find");
        findButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                findButtonActionPerformed(evt);
            }
        });

        replaceButton.setText("Replace");
        replaceButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                replaceButtonActionPerformed(evt);
            }
        });

        replaceAllButton.setText("Replace All");
        replaceAllButton.setEnabled(false);

        javax.swing.GroupLayout replaceDialogLayout = new javax.swing.GroupLayout(replaceDialog.getContentPane());
        replaceDialog.getContentPane().setLayout(replaceDialogLayout);
        replaceDialogLayout.setHorizontalGroup(
            replaceDialogLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(replaceDialogLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(replaceDialogLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(jLabel2)
                    .addComponent(jLabel1))
                .addGap(18, 18, 18)
                .addGroup(replaceDialogLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(replaceRegexCheck)
                    .addGroup(replaceDialogLayout.createSequentialGroup()
                        .addGroup(replaceDialogLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addGroup(replaceDialogLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                                .addComponent(replaceReplaceTextField)
                                .addComponent(replaceFindTextField, javax.swing.GroupLayout.DEFAULT_SIZE, 310, Short.MAX_VALUE))
                            .addGroup(replaceDialogLayout.createSequentialGroup()
                                .addGroup(replaceDialogLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                                    .addComponent(replaceMatchCaseCheck)
                                    .addComponent(replaceWholeWordCheck))
                                .addGap(67, 67, 67)
                                .addGroup(replaceDialogLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                                    .addComponent(incrementalSearchCheck)
                                    .addComponent(replaceWrapAroundCheck))))
                        .addGap(10, 10, 10)
                        .addGroup(replaceDialogLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                            .addComponent(replaceAllButton, javax.swing.GroupLayout.DEFAULT_SIZE, 95, Short.MAX_VALUE)
                            .addComponent(replaceButton, javax.swing.GroupLayout.DEFAULT_SIZE, 95, Short.MAX_VALUE)
                            .addComponent(findButton, javax.swing.GroupLayout.DEFAULT_SIZE, 95, Short.MAX_VALUE))))
                .addContainerGap())
        );
        replaceDialogLayout.setVerticalGroup(
            replaceDialogLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(replaceDialogLayout.createSequentialGroup()
                .addGap(28, 28, 28)
                .addGroup(replaceDialogLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jLabel1)
                    .addComponent(replaceFindTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(findButton))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(replaceDialogLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(replaceReplaceTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jLabel2)
                    .addComponent(replaceButton))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(replaceDialogLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(replaceMatchCaseCheck)
                    .addComponent(replaceWrapAroundCheck)
                    .addComponent(replaceAllButton))
                .addGap(5, 5, 5)
                .addGroup(replaceDialogLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.CENTER)
                    .addComponent(incrementalSearchCheck)
                    .addComponent(replaceWholeWordCheck))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(replaceRegexCheck)
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        setMinimumSize(new java.awt.Dimension(1, 23));

        mainTextPane.setFont(new java.awt.Font("Times New Roman", 0, 14));
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

    private void undoMenuItemActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_undoMenuItemActionPerformed
        mainTextPane.undoLastAction();
    }//GEN-LAST:event_undoMenuItemActionPerformed

    private void redoMenuItemActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_redoMenuItemActionPerformed
        mainTextPane.redoLastAction();
    }//GEN-LAST:event_redoMenuItemActionPerformed

    private void findMenuItemActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_findMenuItemActionPerformed

        findDialog.setVisible(true);
    }//GEN-LAST:event_findMenuItemActionPerformed

    private void findPreviousButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_findPreviousButtonActionPerformed
        boolean success = find(findTextField.getText(), false,
                matchCaseCheck.isSelected(), wholeWordCheck.isSelected(), regexCheck.isSelected());
    }//GEN-LAST:event_findPreviousButtonActionPerformed

    private void findCurrentFileButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_findCurrentFileButtonActionPerformed
        // TODO add your handling code here:
    }//GEN-LAST:event_findCurrentFileButtonActionPerformed

    private void findNextButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_findNextButtonActionPerformed
        boolean success = find(findTextField.getText(), true,
                matchCaseCheck.isSelected(), wholeWordCheck.isSelected(), regexCheck.isSelected());
    }//GEN-LAST:event_findNextButtonActionPerformed

    private void findTextFieldActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_findTextFieldActionPerformed
        // TODO add your handling code here:
    }//GEN-LAST:event_findTextFieldActionPerformed

    private void replaceFindTextFieldActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_replaceFindTextFieldActionPerformed
        // TODO add your handling code here:
    }//GEN-LAST:event_replaceFindTextFieldActionPerformed

    private void replaceReplaceTextFieldActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_replaceReplaceTextFieldActionPerformed
        // TODO add your handling code here:
    }//GEN-LAST:event_replaceReplaceTextFieldActionPerformed

    private void replaceMenuItemActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_replaceMenuItemActionPerformed
        replaceDialog.setVisible(true);
    }//GEN-LAST:event_replaceMenuItemActionPerformed

    private void findButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_findButtonActionPerformed

        boolean success;

        success = find(replaceFindTextField.getText(), true,
                replaceMatchCaseCheck.isSelected(), replaceWholeWordCheck.isSelected(), replaceRegexCheck.isSelected());

        if(!success && replaceWrapAroundCheck.isSelected()) {
            int oldPosition = findTextField.getCaretPosition();
            findTextField.setCaretPosition(0);

            success = find(findTextField.getText(), true,
                    matchCaseCheck.isSelected(), wholeWordCheck.isSelected(), regexCheck.isSelected());

            if(!success) {
                findTextField.setCaretPosition(oldPosition);
            }
        }
    }//GEN-LAST:event_findButtonActionPerformed

    private void replaceButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_replaceButtonActionPerformed
        if(mainTextPane.getSelectedText() != null) {
            mainTextPane.replaceSelection(replaceReplaceTextField.getText());

            if(incrementalSearchCheck.isSelected()) {
                findButtonActionPerformed(evt);
            }
        }
    }//GEN-LAST:event_replaceButtonActionPerformed

    private boolean find(String text, boolean forward,
            boolean matchCase, boolean wholeWord, boolean regex) {

        return SearchEngine.find(mainTextPane, text, forward,
               matchCase, wholeWord, regex);
    }
    
    
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JPopupMenu TabbedDocPopupMenu;
    private javax.swing.JMenuItem channelMenuItem;
    private javax.swing.JMenuItem commentMenuItem;
    private javax.swing.JMenuItem copyMenuItem;
    private javax.swing.JMenuItem cutMenuItem;
    private javax.swing.JMenuItem deviceMenuItem;
    private javax.swing.JMenuItem eventMenuItem;
    private javax.swing.JRadioButton findAllFilesButton;
    private javax.swing.JButton findButton;
    private javax.swing.ButtonGroup findContextButtonGroup;
    private javax.swing.JRadioButton findCurrentFileButton;
    private javax.swing.JDialog findDialog;
    private javax.swing.JMenuItem findMenuItem;
    private javax.swing.JButton findNextButton;
    private javax.swing.JButton findPreviousButton;
    private javax.swing.JTextField findTextField;
    private javax.swing.JCheckBox incrementalSearchCheck;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jLabel2;
    private javax.swing.JPopupMenu.Separator jSeparator3;
    private javax.swing.JPopupMenu.Separator jSeparator4;
    private javax.swing.JPopupMenu.Separator jSeparator5;
    private javax.swing.JPopupMenu.Separator jSeparator6;
    private edu.stanford.atom.sti.client.gui.FileEditorTab.STITextPane mainTextPane;
    private javax.swing.JCheckBox matchCaseCheck;
    private javax.swing.JMenuItem pasteMenuItem;
    private javax.swing.JMenuItem redoMenuItem;
    private javax.swing.JCheckBox regexCheck;
    private javax.swing.JButton replaceAllButton;
    private javax.swing.JButton replaceButton;
    private javax.swing.JDialog replaceDialog;
    private javax.swing.JTextField replaceFindTextField;
    private javax.swing.JCheckBox replaceMatchCaseCheck;
    private javax.swing.JMenuItem replaceMenuItem;
    private javax.swing.JCheckBox replaceRegexCheck;
    private javax.swing.JTextField replaceReplaceTextField;
    private javax.swing.JCheckBox replaceWholeWordCheck;
    private javax.swing.JCheckBox replaceWrapAroundCheck;
    private javax.swing.JMenuItem setvarMenuItem;
    private javax.swing.JMenu stipycmdsMenu;
    private javax.swing.JMenuItem uncommentMenuItem;
    private javax.swing.JMenuItem undoMenuItem;
    private javax.swing.JCheckBox wholeWordCheck;
    // End of variables declaration//GEN-END:variables
    
}
