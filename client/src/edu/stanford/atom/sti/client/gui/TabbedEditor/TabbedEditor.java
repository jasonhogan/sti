/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.stanford.atom.sti.client.gui.TabbedEditor;

import javax.swing.*;
import java.util.Vector;
import java.io.*;
import javax.swing.text.*;

/**
 *
 * @author Jason Hogan
 */
public class TabbedEditor {

    private enum fileError {

        ReadOnly, ReadError, FileIsOpen, NoError
    }

    // Function which when called req's 3 vars to be defined, not sure what the
    // javax.swing. stuff does yet
    public TabbedEditor() {}
    
    public TabbedEditor(javax.swing.JTabbedPane jTabbedPane,
            javax.swing.JFrame jFrame, javax.swing.JLabel caretPositionLabel) {

        untitledDocCount = 0;
        /* It looks like all registerFrame does is set variable jFrameLocal, which
         * has undefined scope, to be equal to the variable used to call the
         * function. If this is all these are intended to do, should be repl. with
         * variable setting of desired scope directly here.
         */
        // set jFrameLocal to the calling variable, jFrame
        registerFrame(jFrame);
        // set jTabbedPaneLocal to the calling variable
        registerTabbedPane(jTabbedPane);
        // set caretPositionLabelLocal to the calling variable
        registerCaretPositionLabel(caretPositionLabel);

        // Listen for changes on the called var jTabbedPane and perform a fcn
        jTabbedPane.addChangeListener(new javax.swing.event.ChangeListener() {
            // Currently appears to do nothing, also, why public?
            public void stateChanged(javax.swing.event.ChangeEvent evt) {
                tabbedPaneStateChanged(evt);
            }
        });


    }

    public void registerTabbedPane(javax.swing.JTabbedPane jTabbedPane) {
        jTabbedPaneLocal = jTabbedPane;
    }

    public void registerFrame(javax.swing.JFrame jFrame) {
        jFrameLocal = jFrame;
    }

    public void registerCaretPositionLabel(javax.swing.JLabel caretPositionLabel) {
        caretPositionLabelLocal = caretPositionLabel;
    }

    //Tab methods    
    private void tabbedPaneStateChanged(javax.swing.event.ChangeEvent evt) {
    // TODO add your handling code here:
    //     System.out.println("Tab Index: " + jTabbedPaneLocal.getSelectedIndex()+ " TabbedDocument Index: "  + tabbedDocumentVector.elementAt(jTabbedPaneLocal.getSelectedIndex()).getTabIndex());
    }

    public void tabIsModified(int tabIndex)
            throws ArrayIndexOutOfBoundsException {

        if (tabIndex >= tabbedDocumentVector.size() || tabIndex < 0) {
            // Creates exception if the position of the caret is outside of the
            // size of the document?
            throw new ArrayIndexOutOfBoundsException(tabIndex);
        }

        jTabbedPaneLocal.setTitleAt(tabIndex,
                tabbedDocumentVector.elementAt(tabIndex).getTabTitle() + " *");
        tabbedDocumentVector.elementAt(tabIndex).setModifed(true);
    }

    public void tabIsNotModified(int tabIndex)
            throws ArrayIndexOutOfBoundsException {

        if (tabIndex >= tabbedDocumentVector.size() || tabIndex < 0) {
            throw new ArrayIndexOutOfBoundsException(tabIndex);
        }
        jTabbedPaneLocal.setTitleAt(tabIndex,
                tabbedDocumentVector.elementAt(tabIndex).getTabTitle());
        tabbedDocumentVector.elementAt(tabIndex).setModifed(false);
    }

    public void addEditorTab(TabbedDocument tabbedDocument) {
        jTabbedPaneLocal.addTab(tabbedDocument.getTabTitle(),
                tabbedDocument.generateTab());
    }

    public void removeEditorTab() {
        removeEditorTab(jTabbedPaneLocal.getSelectedIndex());
    }

    public void removeEditorTab(int tabIndex)
            throws ArrayIndexOutOfBoundsException {

        if (tabIndex >= tabbedDocumentVector.size() || tabIndex < 0) {
            throw new ArrayIndexOutOfBoundsException(tabIndex);
        }
        jTabbedPaneLocal.removeTabAt(tabIndex);
        tabbedDocumentVector.remove(tabIndex);
        //Reindex so TabbedDocument indicies match JTabbedPane indicies
        for (int i = tabIndex; i < tabbedDocumentVector.size(); i++) {
            tabbedDocumentVector.elementAt(i).setTabIndex(i);
        }
    }

    //Close files
    public void closeFileInActiveTab() {
        closeFileInTab(jTabbedPaneLocal.getSelectedIndex());
    }

    /**
     * Generic close file method.
     * @param tabIndex The index of the JTabbedPane that contains the 
     * document to close
     * @return true if the document closed successfully, false otherwise.
     * @throws java.lang.ArrayIndexOutOfBoundsException
     */
    public boolean closeFileInTab(int tabIndex)
            throws ArrayIndexOutOfBoundsException {

        if (tabIndex >= tabbedDocumentVector.size() || tabIndex < 0) {
            throw new ArrayIndexOutOfBoundsException(tabIndex);
        }

        if (tabbedDocumentVector.elementAt(tabIndex).isModifed()) {
            Object[] options = {"Yes", "No", "Cancel"};
            int fileOpenDialogResult = JOptionPane.showOptionDialog(jFrameLocal,
                    "Do you want to save changes you made to " 
                    + tabbedDocumentVector.elementAt(tabIndex).getTabTitle() 
                    + "?",
                    "Unsaved Changes",
                    JOptionPane.YES_NO_CANCEL_OPTION,
                    JOptionPane.WARNING_MESSAGE,
                    null,
                    options,
                    options[2]);
            switch (fileOpenDialogResult) {
                case JOptionPane.YES_OPTION:
                    //"Yes" -- Save and close
                    if (saveTab(tabIndex)) {
                        removeEditorTab(tabIndex);
                        return true;
                    } else {
                        break;
                    }
                case JOptionPane.NO_OPTION:
                    //"No" -- Don't save
                    System.out.println("Don't Save");
                    removeEditorTab(tabIndex);
                    return true;
                case JOptionPane.CANCEL_OPTION:
                    //cancel
                    break;
                }
            return false;
        } else {
            // document is not modified
            removeEditorTab(tabIndex);
            return true;
        }

    }

    //Open files
    public void openFileInNewTab() {
        openFileInNewTab(openFileDialog());
    }

    public void openFileInNewTab(File file) {
        openFileInTab(file, tabbedDocumentVector.size());
    }

    public void openFileInActiveTab() {
        openFileInActiveTab(openFileDialog());
    }

    public void openFileInActiveTab(File file) {
        openFileInTab(file, jTabbedPaneLocal.getSelectedIndex());
    }

    /**
     * Generic open file method. If tabIndex is equal to the current number of 
     * open tabs (the current size() of the Vector of TabbedDoccuments) then a new
     * tab is created and a new TabbedDoccument element is added to the Vector.
     * @param file The File to open
     * @param tabIndex The index in the JTabbedPane for the opened file
     * @throws java.lang.ArrayIndexOutOfBoundsException
     * Throws an exception if tabIndex is less than zero or if tabIndex is 
     * greater than the size of the Vector of TabbedDocuments
     */
    public void openFileInTab(File file, int tabIndex)
            throws ArrayIndexOutOfBoundsException {

        // creates a new tab if tabIndex == tabbedDocumentVector.size()
        if (tabIndex > tabbedDocumentVector.size() || tabIndex < 0) {
            throw new ArrayIndexOutOfBoundsException(tabIndex);
        }

        fileError errorMessage = fileError.NoError;

        if (file == null) {
            //no file selected
            errorMessage = fileError.NoError;
        } else {

            if (fileIsOpen(file) == false) {
                if (file.canRead()) {

                    String text = readFile(file);

                    if (text == null) {
                        errorMessage = fileError.ReadError;
                    } else {
                        if (file.canWrite() == false) {
                            errorMessage = fileError.ReadOnly;
                        }

                        // Create a new TabbedDocument
                        if (tabIndex == tabbedDocumentVector.size()) {
                            //create a new tab
                            tabbedDocumentVector.addElement(
                                    new TabbedDocument(file, this,
                                    tabIndex));
                            addEditorTab(tabbedDocumentVector.lastElement());

                        } else {
                            //use an already existing tab
                            tabbedDocumentVector.setElementAt(
                                    new TabbedDocument(file, this, tabIndex),
                                    tabIndex);
                        }

                        // Ensure the new tab is active
                        jTabbedPaneLocal.setSelectedIndex(tabIndex);

                        tabbedDocumentVector.elementAt(tabIndex).
                                getTextPane().setText(text);

                        tabIsNotModified(tabIndex);


                        //new
                        // Ensure the new tab is active
                        jTabbedPaneLocal.setSelectedIndex(
                                tabbedDocumentVector.lastElement().
                                getTabIndex());

                        tabbedDocumentVector.lastElement().getTextPane().
                                setText(text);

                        tabIsNotModified(tabbedDocumentVector.lastElement().
                                getTabIndex());


                        //active
                        int selectedIndex = jTabbedPaneLocal.getSelectedIndex();


                    }

                } else {
                    // Cannot read file
                    errorMessage = fileError.ReadError;
                }
            } else {
                //file is open
                if (tabbedDocumentVector.elementAt(jTabbedPaneLocal.getSelectedIndex()).getFile().equals(file) &&
                        tabbedDocumentVector.elementAt(jTabbedPaneLocal.getSelectedIndex()).isModifed() == false) {
                    // file is open in active tab and active tab is unmodified
                    //ok to re-open file
                    errorMessage = fileError.NoError;

                } else {
                    errorMessage = fileError.FileIsOpen;
                }
            }
        }
        switch (errorMessage) {
            case ReadError:
                JOptionPane.showMessageDialog(jFrameLocal,
                        "Error reading from file.",
                        "File Read Error",
                        JOptionPane.ERROR_MESSAGE);
                break;
            case ReadOnly:
                JOptionPane.showMessageDialog(jFrameLocal,
                        "The selected file \"" + file.getName() +
                        "\" is marked as read only. \n " +
                        "Any changes made to this file must be saved " +
                        "using a new file name.",
                        "Read Only",
                        JOptionPane.INFORMATION_MESSAGE);
                break;
            case FileIsOpen:
                Object[] options = {"Yes", "Cancel"};
                int fileOpenDialogResult = JOptionPane.showOptionDialog(jFrameLocal,
                        "Warning: This file is already open.\n " +
                        "Do you want to revert to the saved version? " +
                        "(All unsaved changes will be lost.)",
                        "File Already Open",
                        JOptionPane.OK_CANCEL_OPTION,
                        JOptionPane.WARNING_MESSAGE,
                        null,
                        options,
                        options[1]);
                switch (fileOpenDialogResult) {
                    case JOptionPane.OK_OPTION:
                        //"Yes" -- Revert
                        int index = getIndex(file);
                        removeEditorTab(index);
                        openFileInNewTab(file);
                        System.out.println("Revert");
                        break;
                    case JOptionPane.CANCEL_OPTION:
                        //"Cancel"
                        System.out.println("Cancel");
                        break;
                }
                break;
            case NoError:
                break;
        }

    }

    private File openFileDialog() {

        JFileChooser fc = new JFileChooser();
        fc.setCurrentDirectory(new File("."));

        int result = fc.showOpenDialog(jFrameLocal);

        if (result == JFileChooser.CANCEL_OPTION) {
            return null;
        } else if (result == JFileChooser.APPROVE_OPTION) {
            return fc.getSelectedFile();
        } else {
            return null;
        }
    }

    /**
     * Finds the tab index in the JTabbedPane of a particular File currently 
     * open in the TabbedEditor.
     * @param file The File of interest
     * @return The index of the tab in the JTabbedPane for this File.
     */
    public int getIndex(File file) {
        //Returns -1 if the file is not currently open in a tab

        int index = -1;
        for (int i = 0; i < tabbedDocumentVector.size(); i++) {
            if (tabbedDocumentVector.elementAt(i).getFile() != null) {
                if (tabbedDocumentVector.elementAt(i).getFile().equals(file)) {
                    index = i;
                }
            }
        }
        return index;
    }

    /**
     * Checks to see if a particular File is already open in the TabbedEditor
     * @param file The particular File to check the status of.
     * @return true if the the File is already open.  
     * 
     * false if the File is not open or if the File is null
     */
    public boolean fileIsOpen(File file) {
        for (TabbedDocument docIter : tabbedDocumentVector) {
            if (docIter.getFile() != null) {
                if (docIter.getFile().equals(file)) {
                    return true;
                }
            }
        }
        return false;
    }

    public String readFile(File file) {

        StringBuffer fileBuffer;
        String fileString;
        String line;

        try {
            FileReader in = new FileReader(file);
            BufferedReader br = new BufferedReader(in);
            fileBuffer = new StringBuffer();

            while ((line = br.readLine()) != null) {
                fileBuffer.append(line + "\n");
            }
            in.close();
            fileString = fileBuffer.toString();
        } catch (IOException e) {
            e.printStackTrace();
            return null;
        }
        return fileString;
    }

    //Save files
    public boolean saveActiveTab() {
        return saveTab(jTabbedPaneLocal.getSelectedIndex());
    }

    /**
     * Generic save method.  Calls saveAs(tabIndex) if the document has a null
     * File or is marked as read only.
     * @param tabIndex The index of the JTabbedPane that contains the document to save
     * @return true if the document is sucessfully saved, false otherwise.
     * @throws java.lang.ArrayIndexOutOfBoundsException
     */
    public boolean saveTab(int tabIndex) throws ArrayIndexOutOfBoundsException {

        if (tabIndex >= tabbedDocumentVector.size() || tabIndex < 0) {
            throw new ArrayIndexOutOfBoundsException(tabIndex);
        }

        File file = tabbedDocumentVector.elementAt(tabIndex).getFile();

        if (file == null) {
            // "Untitled" file
            return saveAsTab(tabIndex);
        }
        if (file.canWrite()) {
            tabIsNotModified(tabIndex);
            return writeFile(file,
                    tabbedDocumentVector.elementAt(tabIndex).
                    getTextPane().getText());
        } else {
            // file is read only
            Object[] options = {"Save as...", "Cancel"};
            int readOnlyDialogResult = JOptionPane.showOptionDialog(jFrameLocal,
                    "This file is marked as read only.\n " +
                    "Changes must be saved using a different file name.",
                    "Read Only",
                    JOptionPane.OK_CANCEL_OPTION,
                    JOptionPane.QUESTION_MESSAGE,
                    null,
                    options,
                    options[1]);
            switch (readOnlyDialogResult) {
                case JOptionPane.OK_OPTION:
                    //"Save as..."
                    if (saveAsTab(tabIndex)) {
                        return true;

                    } else {
                        return false;
                    }
                case JOptionPane.CANCEL_OPTION:
                //"Cancel"
                default:
                    return false;
                }
        }

    }

    public boolean saveAsActiveTab() {
        return saveAsTab(jTabbedPaneLocal.getSelectedIndex());
    }

    /**
     * Generic SaveAs method.  Prompts for a file name and presents an overwrite 
     * confirmation dialog if the file already exists.
     * @param tabIndex The index of the JTabbedPane that contains the document
     * to be saved.
     * @return true if the document is successfully saved, false otherwise.
     * @throws java.lang.ArrayIndexOutOfBoundsException
     */
    public boolean saveAsTab(int tabIndex)
            throws ArrayIndexOutOfBoundsException {

        if (tabIndex >= tabbedDocumentVector.size() || tabIndex < 0) {
            throw new ArrayIndexOutOfBoundsException(tabIndex);
        }

        // Show the tab that is going to be saved
        jTabbedPaneLocal.setSelectedIndex(tabIndex);

        JFileChooser fc = new JFileChooser();
        fc.setCurrentDirectory(new File("."));

        int result = fc.showSaveDialog(jFrameLocal);

        if (result == JFileChooser.CANCEL_OPTION) {
            return false;
        } else if (result == JFileChooser.APPROVE_OPTION) {
            File file = fc.getSelectedFile();

            if (file.exists()) {
                int response = JOptionPane.showConfirmDialog(null,
                        "Overwrite existing file?",
                        "Overwrite?", JOptionPane.OK_CANCEL_OPTION,
                        JOptionPane.QUESTION_MESSAGE);

                if (response == JOptionPane.CANCEL_OPTION) {
                    return false;
                }
            }

            if (writeFile(file,
                    tabbedDocumentVector.elementAt(tabIndex).
                    getTextPane().getText())) {

                tabbedDocumentVector.elementAt(tabIndex).setFile(file);
                tabbedDocumentVector.elementAt(tabIndex).setTabTitle(file);
                tabIsNotModified(tabIndex);
                return true;
            }
            //write failed for some reason
            return false;
        } else {
            //file chooser returned without "Cancel" or "OK"
            return false;
        }
    }

    private boolean writeFile(File file, String dataString) {
        try {
            PrintWriter out =
                    new PrintWriter(new BufferedWriter(new FileWriter(file)));
            out.print(dataString);
            out.flush();
            out.close();
        } catch (IOException e) {
            e.printStackTrace();
            return false;
        }
        return true;
    }

    //New files
    /**
     * Creates a blank new document in a new tab with title Untitled#, where # 
     * is a number starting from 1 that increments each time a new file is made. 
     * The File associated with the newly created TabbedDocument is null.
     */
    
    // called by EditorGUI.java
    public void createNewFile() {

        untitledDocCount++;

        tabbedDocumentVector.addElement(
                new TabbedDocument(null, this,
                tabbedDocumentVector.size()));

        addEditorTab(tabbedDocumentVector.lastElement());

        // Ensure the new tab is active
        jTabbedPaneLocal.setSelectedIndex(
                tabbedDocumentVector.lastElement().getTabIndex());

        tabbedDocumentVector.lastElement().getTextPane().setText("");

        tabIsNotModified(tabbedDocumentVector.lastElement().getTabIndex());
    }

    public int getUntitledDocCount() {
        return untitledDocCount;
    }
    private Vector<TabbedDocument> tabbedDocumentVector = new Vector<TabbedDocument>();
    private int untitledDocCount = 0;
    public javax.swing.JLabel caretPositionLabelLocal;
    private javax.swing.JTabbedPane jTabbedPaneLocal;
    private javax.swing.JFrame jFrameLocal;
}


