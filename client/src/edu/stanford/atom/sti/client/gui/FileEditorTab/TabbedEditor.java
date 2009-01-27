/*
 * TabbedEditor.java
 *
 * Created on November 17, 2008, 10:05 AM
 */

package edu.stanford.atom.sti.client.gui.FileEditorTab;

import javax.swing.*;
import java.util.Vector;
import java.io.*;
import javax.swing.text.*;
import edu.stanford.atom.sti.RemoteFileServer.NetworkFileChooser.*;
import edu.stanford.atom.sti.client.comm.corba.*;

/**
 *
 * @author  Owner
 */
public class TabbedEditor extends javax.swing.JPanel {

    private enum fileError {ReadOnly, ReadError, FileIsOpen, NoError}
    private Parser parserRef = null;
    
    private Vector<TabbedDocument> tabbedDocumentVector = new Vector<TabbedDocument>();
    JFileChooser localFileChooser = null;
    
    /** Creates new form TabbedEditor */
    public TabbedEditor() {
        initComponents();
    }
    
    public void setParser(Parser parser) {
        parserRef = parser;
    }
    
    public boolean parseFile() {

        TabbedDocument doc = getSelectedTabbedDocument();
        
        if(doc == null) {
            return false;
        }
        
        while(doc.isLocalFile()) {
            Object[] options = {"Save As Network File...", "Cancel"};
            int dialogResult = JOptionPane.showOptionDialog(this,
                    "File '" 
                    + doc.getFileName()
                    + "' is a local file."
                    + "\nOnly network files can be parsed."
                    + "\n\nDo you want to save this file on the network?",
                    "Cannot parse local file",
                    JOptionPane.OK_CANCEL_OPTION,
                    JOptionPane.WARNING_MESSAGE,
                    null,
                    options,
                    options[1]);
            switch (dialogResult) {
                case JOptionPane.OK_OPTION:
                    //"Save As Network File..."
                    if( !saveAsNetworkActiveTab() ) {
                        return false;
                    }
                case JOptionPane.CANCEL_OPTION:
                //"Cancel"
                default:
                    return false;
                }
        }
        
        if (parserRef == null) {
            return false;
        }
        
        while (doc.isModifed()) {
            if( !saveActiveTab() ) {
                return false;
            }
        }

        try {
            //The server ip address is not needed since
            //currently the python parser only looks for files on the server.
            String errorMessages = "";
            boolean parseSuccess = true;
            if (parserRef.parseFile(doc.getPath())) {
                //python returned errors
                errorMessages = "\n\n" + parserRef.errMsg();
                parseSuccess = false;
            }
            String messages = parserRef.outMsg() + errorMessages;
            parserTextArea.setText(messages);
            return parseSuccess;
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
    }
    
    public TabbedDocument getSelectedTabbedDocument() {
        return (TabbedDocument) textEditorTabbedPane.getSelectedComponent();
    }
    
    public void addNetworkFileServer(String IPAddress, String port) {
        networkFileChooser1.addFileServer(IPAddress, port);
    }
    public void tabIsNotModified(int tabIndex)
            throws ArrayIndexOutOfBoundsException {

        if (tabIndex >= tabbedDocumentVector.size() || tabIndex < 0) {
            throw new ArrayIndexOutOfBoundsException(tabIndex);
        }

        tabbedDocumentVector.elementAt(tabIndex).setModifed(false);
    }
    
    public void addEditorTab(TabbedDocument tabbedDocument) {
        textEditorTabbedPane.addTab(tabbedDocument.getTabTitle(),
                tabbedDocument);
    }
    
    public void insertEditorTab(TabbedDocument tabbedDocument, int index) {
        if(index >=0 && index < textEditorTabbedPane.getTabCount()) {
            textEditorTabbedPane.insertTab(tabbedDocument.getTabTitle(), null, 
                    tabbedDocument, null, index);
        } else {
            addEditorTab(tabbedDocument);
        }
    }
    public void removeEditorTab() {
        removeEditorTab(textEditorTabbedPane.getSelectedIndex());
    }

    public void removeEditorTab(int tabIndex)
            throws ArrayIndexOutOfBoundsException {

        if (tabIndex >= tabbedDocumentVector.size() || tabIndex < 0) {
            throw new ArrayIndexOutOfBoundsException(tabIndex);
        }
        textEditorTabbedPane.removeTabAt(tabIndex);
        tabbedDocumentVector.remove(tabIndex);
        //Reindex so TabbedDocument indicies match JTabbedPane indicies
        for (int i = tabIndex; i < tabbedDocumentVector.size(); i++) {
            tabbedDocumentVector.elementAt(i).setTabIndex(i);
        }
    }

    /**
     * Creates a blank new document in a new tab with title Untitled#, where # 
     * is a number starting from 1 that increments each time a new file is made. 
     * The File associated with the newly created TabbedDocument is null.
     */
   
    public void createNewFile() {

        tabbedDocumentVector.addElement(
                new TabbedDocument(null, textEditorTabbedPane,
                tabbedDocumentVector.size()));

        addEditorTab(tabbedDocumentVector.lastElement());

        // Ensure the new tab is active
        textEditorTabbedPane.setSelectedIndex(
                tabbedDocumentVector.lastElement().getTabIndex());

        tabbedDocumentVector.lastElement().getTextPane().setText("");

        tabIsNotModified(tabbedDocumentVector.lastElement().getTabIndex());
    }
    
    public boolean closeFileInActiveTab() {
        return closeFileInTab(textEditorTabbedPane.getSelectedIndex());
    }
        
    /**
     * Generic close file method.
     * @param tabIndex The index of the JTabbedPane that contains the 
     * document to close
     * @return true if the document closed successfully, false otherwise.
     * @throws java.lang.ArrayIndexOutOfBoundsException
     */
    public boolean closeFileInTab(int tabIndex) {

        if (tabIndex >= tabbedDocumentVector.size() || tabIndex < 0) {
            return false;
        }

        if (tabbedDocumentVector.elementAt(tabIndex).isModifed()) {
            Object[] options = {"Yes", "No", "Cancel"};
            int fileOpenDialogResult = JOptionPane.showOptionDialog(this,
                    "Do you want to save changes you made to " 
                    + tabbedDocumentVector.elementAt(tabIndex).getFileName() 
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
                    if (save(tabIndex)) {
                        removeEditorTab(tabIndex);
                        return true;
                    } 
                    else
                        break;
                case JOptionPane.NO_OPTION:
                    //"No" -- Don't save
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
    public void openLocalFile() {
        openLocalFile(openLocalFileDialog());
    }
        
    public void openNetworkFile() {

        if (openNetworkFileDialog()) {
            String filePath = networkFileChooser1.getChoosenFullFilePath();
            NetworkFileSystem nfs = networkFileChooser1.getChosenNetworkFileSystem();

            if (nfs != null || filePath != null) {
                openNetworkFile(nfs, filePath);
            }
        }
    }

    public void openLocalFile(File file) {
        
        fileError errorMessage = openLocalFileInTab(file, tabbedDocumentVector.size());
        
        switch (errorMessage) {
            case ReadError:
                JOptionPane.showMessageDialog(this,
                        "Error reading from file.",
                        "File Read Error",
                        JOptionPane.ERROR_MESSAGE);
                break;
            case ReadOnly:
                JOptionPane.showMessageDialog(this,
                        "The selected file \"" + file.getName() +
                        "\" is marked as read only. \n " +
                        "Any changes made to this file must be saved " +
                        "using a new file name.",
                        "Read Only",
                        JOptionPane.INFORMATION_MESSAGE);
                break;
            case FileIsOpen:
                Object[] options = {"Yes", "Cancel"};
                int fileOpenDialogResult = JOptionPane.showOptionDialog(this,
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
                        if(index >= 0 && index < textEditorTabbedPane.getTabCount()) {
                            removeEditorTab(index);
                            openLocalFileInTab(file, index);
                        }
                        break;
                    case JOptionPane.CANCEL_OPTION:
                        //"Cancel"
                        break;
                }
                break;
            case NoError:
                break;
        }
    }

    public void openNetworkFile(NetworkFileSystem networkFileSystem, String file) {
        
        fileError errorMessage = openNetworkFileInTab(networkFileSystem, file, tabbedDocumentVector.size());
        
        switch (errorMessage) {
            case ReadError:
                JOptionPane.showMessageDialog(this,
                        "Error reading from file.",
                        "File Read Error",
                        JOptionPane.ERROR_MESSAGE);
                break;
            case ReadOnly:
                JOptionPane.showMessageDialog(this,
                        "The selected file \"" + file +
                        "\" is marked as read only. \n " +
                        "Any changes made to this file must be saved " +
                        "using a new file name.",
                        "Read Only",
                        JOptionPane.INFORMATION_MESSAGE);
                break;
            case FileIsOpen:
                Object[] options = {"Yes", "Cancel"};
                int fileOpenDialogResult = JOptionPane.showOptionDialog(this,
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
                        int index = getIndex(networkFileSystem, file);
                        if(index >= 0 && index < textEditorTabbedPane.getTabCount()) {
                            removeEditorTab(index);
                            openNetworkFileInTab(networkFileSystem, file, index);
                        }
                        break;
                    case JOptionPane.CANCEL_OPTION:
                        //"Cancel"
                        break;
                }
                break;
            case NoError:
                break;
        }
    }
    
    private File openLocalFileDialog() {

        if(localFileChooser == null) {
            localFileChooser = new JFileChooser();
            localFileChooser.setCurrentDirectory(new File("."));
        }

        int result = localFileChooser.showOpenDialog(this);

        if (result == JFileChooser.CANCEL_OPTION) {
            return null;
        } else if (result == JFileChooser.APPROVE_OPTION) {
            return localFileChooser.getSelectedFile();
        } else {
            return null;
        }
    }
        
    private boolean openNetworkFileDialog() {
        int result = networkFileChooser1.showOpenDialog(this);

        if (result == NetworkFileChooser.CANCEL_OPTION) {
            return false;
        } else if (result == NetworkFileChooser.APPROVE_OPTION) {
            return true;
        }
        return false;
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
    private fileError openLocalFileInTab(File file, int tabIndex) {

        // creates a new tab if tabIndex == tabbedDocumentVector.size()
        if (tabIndex > tabbedDocumentVector.size() || tabIndex < 0) {
            return fileError.NoError;
        }

        if (file == null) //no file selected
            return fileError.NoError;
        
        if ( fileIsOpen(file) )  {
            int alreadyOpenIndex = getIndex(file);
            if(alreadyOpenIndex >= 0 && alreadyOpenIndex < textEditorTabbedPane.getTabCount()) {
                if(tabbedDocumentVector.elementAt(alreadyOpenIndex).isModifed())
                    return fileError.FileIsOpen;
            }
        }
    
        if (!file.canRead())
            return fileError.ReadError;       //Cannot read file

        String text = readLocalFile(file);

        if (text == null) 
            return fileError.ReadError;

        if (!file.canWrite())
            return fileError.ReadOnly;

        // Create a new TabbedDocument
        if (tabIndex == tabbedDocumentVector.size()) {
            //create a new tab at the end
            tabbedDocumentVector.addElement(
                    new TabbedDocument(file, textEditorTabbedPane, tabIndex));
            addEditorTab(tabbedDocumentVector.lastElement());

        } else {
            //insert a new tab
            tabbedDocumentVector.insertElementAt(
                    new TabbedDocument(file, textEditorTabbedPane, tabIndex), 
                    tabIndex);
            insertEditorTab(tabbedDocumentVector.elementAt(tabIndex), tabIndex);
        }

        // Ensure the new tab is active
        textEditorTabbedPane.setSelectedIndex(tabIndex);

        tabbedDocumentVector.elementAt(tabIndex).
                getTextPane().setText(text);

        tabbedDocumentVector.elementAt(tabIndex).
                getTextPane().setCaretPosition(0);
        
        tabIsNotModified(tabIndex);
        return fileError.NoError;
    }
    
    private fileError openNetworkFileInTab(NetworkFileSystem networkFileSystem, 
            String file, int tabIndex) {
        
        if (tabIndex > tabbedDocumentVector.size() || tabIndex < 0) {
            return fileError.NoError;
        }

        if (networkFileSystem == null || file == null) //no file selected
            return fileError.NoError;
        
        if ( fileIsOpen(networkFileSystem, file) )  {
            int alreadyOpenIndex = getIndex(networkFileSystem, file);
            if(alreadyOpenIndex >= 0 && alreadyOpenIndex < textEditorTabbedPane.getTabCount()) {
                if(tabbedDocumentVector.elementAt(alreadyOpenIndex).isModifed()) {
                    return fileError.FileIsOpen;
                } else {       
                    //not modified; close and then open again to get a refreshed copy
                    removeEditorTab(alreadyOpenIndex);
                    openNetworkFileInTab(networkFileSystem, file, alreadyOpenIndex);
                    return fileError.NoError;
                }
            }
        }

        String text = readNetworkFile(networkFileSystem, file);

        if (text == null) 
            return fileError.ReadError;

        if (networkFileSystem.isReadOnly(file))
            return fileError.ReadOnly;

        // Create a new TabbedDocument
        if (tabIndex == tabbedDocumentVector.size()) {
            //create a new tab at the end
            tabbedDocumentVector.addElement(
                    new TabbedDocument(file, networkFileSystem, 
                    textEditorTabbedPane, tabIndex));
            addEditorTab(tabbedDocumentVector.lastElement());

        } else {
            //insert a new tab
            tabbedDocumentVector.insertElementAt(
                    new TabbedDocument(file, networkFileSystem, 
                    textEditorTabbedPane, tabIndex), 
                    tabIndex);
            insertEditorTab(tabbedDocumentVector.elementAt(tabIndex), tabIndex);
        }

        // Ensure the new tab is active
        textEditorTabbedPane.setSelectedIndex(tabIndex);

        tabbedDocumentVector.elementAt(tabIndex).
                getTextPane().setText(text);
        
        tabbedDocumentVector.elementAt(tabIndex).
                getTextPane().setCaretPosition(0);
        
        tabIsNotModified(tabIndex);
        return fileError.NoError;
    }
    //Save files
    public boolean saveActiveTab() {
        if(textEditorTabbedPane.getTabCount() == 0)
            return false;
            
        return save(textEditorTabbedPane.getSelectedIndex());
    }

    /**
     * Generic save method.  Calls saveAs(tabIndex) if the document has a null
     * File or is marked as read only.
     * @param tabIndex The index of the JTabbedPane that contains the document to save
     * @return true if the document is sucessfully saved, false otherwise.
     * @throws java.lang.ArrayIndexOutOfBoundsException
     */
    public boolean save(int tabIndex) {

        if (tabIndex >= tabbedDocumentVector.size() || tabIndex < 0)
            return false;

        if (tabbedDocumentVector.elementAt(tabIndex).isNewFile())
            return saveAsNetwork(tabIndex);     // default to network save
        else if (tabbedDocumentVector.elementAt(tabIndex).isLocalFile())
            return saveLocal(tabIndex);
        else
            return saveNetwork(tabIndex);
    }

    public boolean saveLocal(int tabIndex) {

        if (tabIndex >= tabbedDocumentVector.size() || tabIndex < 0)
            return false;
    
        // "Untitled" file; default to network save
        if (tabbedDocumentVector.elementAt(tabIndex).isNewFile())
            saveAsNetwork(tabIndex);

        if (!tabbedDocumentVector.elementAt(tabIndex).isLocalFile()) {
            // Not a local file! Prompt to Save As local.
            Object[] options = {"Save As Local File...", "Cancel"};
            int dialogResult = JOptionPane.showOptionDialog(this,
                    "File '" 
                    + tabbedDocumentVector.elementAt(tabIndex).getFileName()
                    + "' is NOT local and is currently stored on server"
                    + tabbedDocumentVector.elementAt(tabIndex).getServerAddress()
                    + "\nAre you sure you want to save as a local file?",
                    "Not a local file",
                    JOptionPane.OK_CANCEL_OPTION,
                    JOptionPane.WARNING_MESSAGE,
                    null,
                    options,
                    options[1]);
            switch (dialogResult) {
                case JOptionPane.OK_OPTION:
                    //"Save As Local File..."
                    return saveAsLocal(tabIndex);
                case JOptionPane.CANCEL_OPTION:
                //"Cancel"
                default:
                    return false;
                }
        }

        File file = tabbedDocumentVector.elementAt(tabIndex).getFile();

        if (file == null) {
            // This should not happen.
            return saveAsLocal(tabIndex);
        }
        if (tabbedDocumentVector.elementAt(tabIndex).canWrite()) {
            if(tabbedDocumentVector.elementAt(tabIndex).isModifed()) {
                tabIsNotModified(tabIndex);
                return writeFileLocal(file,
                    tabbedDocumentVector.elementAt(tabIndex).
                    getTextPane().getText());
            }
            else
                return true;
           
        } 
        else {
            // file is read only
            Object[] options = {"Save as...", "Cancel"};
            int readOnlyDialogResult = JOptionPane.showOptionDialog(this,
                    "File '" 
                    + tabbedDocumentVector.elementAt(tabIndex).getFileName() 
                    + "' is marked as read only.\n " +
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
                    return saveAsLocal(tabIndex);
                case JOptionPane.CANCEL_OPTION:
                //"Cancel"
                default:
                    return false;
                }
        }
    }
    public boolean saveNetwork(int tabIndex) {

        if (tabIndex >= tabbedDocumentVector.size() || tabIndex < 0)
            return false;
    
        // "Untitled" file
        if (tabbedDocumentVector.elementAt(tabIndex).isNewFile()) {
            saveAsNetwork(tabIndex);
        }
        
        if (tabbedDocumentVector.elementAt(tabIndex).isLocalFile()) {
            // This is a local file; Prompt to Save As Network.
            Object[] options = {"Save As Network File...", "Cancel"};
            int dialogResult = JOptionPane.showOptionDialog(this,
                    "File '" 
                    + tabbedDocumentVector.elementAt(tabIndex).getFileName()
                    + "' is a local file."
                    + "\nAre you sure you want to save as a network file?",
                    "Not a network file",
                    JOptionPane.OK_CANCEL_OPTION,
                    JOptionPane.WARNING_MESSAGE,
                    null,
                    options,
                    options[1]);
            switch (dialogResult) {
                case JOptionPane.OK_OPTION:
                    //"Save As Network File..."
                    return saveAsNetwork(tabIndex);
                case JOptionPane.CANCEL_OPTION:
                //"Cancel"
                default:
                    return false;
                }
        }
              
        if (!tabbedDocumentVector.elementAt(tabIndex).isModifed()) {
            return true;    //unmodified!
        }
        
        if (tabbedDocumentVector.elementAt(tabIndex).canWrite()) {
            tabIsNotModified(tabIndex);
            return writeFileNetwork(
                    tabbedDocumentVector.elementAt(tabIndex).
                    getNetworkFileSystem(),
                    tabbedDocumentVector.elementAt(tabIndex).getPath(),
                    tabbedDocumentVector.elementAt(tabIndex).
                    getTextPane().getText());
        }
        else {
            // file is read only
            Object[] options = {"Save as...", "Cancel"};
            int readOnlyDialogResult = JOptionPane.showOptionDialog(this,
                    "File '" 
                    + tabbedDocumentVector.elementAt(tabIndex).getFileName() 
                    + "' is marked as read only.\n " +
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
                    return saveAsNetwork(tabIndex);
                case JOptionPane.CANCEL_OPTION:
                //"Cancel"
                default:
                    return false;
                }
        }
    }
    
    public boolean saveAsNetworkActiveTab() {
        if(textEditorTabbedPane.getTabCount() == 0)
            return false;
        return saveAsNetwork(textEditorTabbedPane.getSelectedIndex());
    }
    
    public boolean saveAsLocalActiveTab() {
        if(textEditorTabbedPane.getTabCount() == 0)
            return false;
        return saveAsLocal(textEditorTabbedPane.getSelectedIndex());
    }

    /**
     * Generic Local Save As method.  Prompts for a file name and presents an overwrite 
     * confirmation dialog if the file already exists.
     * @param tabIndex The index of the JTabbedPane that contains the document
     * to be saved.
     * @return true if the document is successfully saved, false otherwise.
     * @throws java.lang.ArrayIndexOutOfBoundsException
     */
    public boolean saveAsLocal(int tabIndex) {

        if (tabIndex >= tabbedDocumentVector.size() || tabIndex < 0) {
            return false;
        }

        // Show the tab that is going to be saved
        textEditorTabbedPane.setSelectedIndex(tabIndex);

        if(localFileChooser == null) {
            localFileChooser = new JFileChooser();
            localFileChooser.setCurrentDirectory(new File("."));
        }

        int result = localFileChooser.showSaveDialog(this);

        if (result == JFileChooser.CANCEL_OPTION) {
            return false;
        } else if (result == JFileChooser.APPROVE_OPTION) {
            File file = localFileChooser.getSelectedFile();

            if (file.exists()) {
                int response = JOptionPane.showConfirmDialog(null,
                        "Overwrite existing file?",
                        "Overwrite?", JOptionPane.OK_CANCEL_OPTION,
                        JOptionPane.QUESTION_MESSAGE);

                if (response == JOptionPane.CANCEL_OPTION) {
                    return false;
                }
            }

            if (writeFileLocal(file,
                    tabbedDocumentVector.elementAt(tabIndex).
                    getTextPane().getText())) {

                tabbedDocumentVector.elementAt(tabIndex).saveDocument(file);
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
    
    public boolean saveAsNetwork(int tabIndex) {

        if (tabIndex >= tabbedDocumentVector.size() || tabIndex < 0) {
            return false;
        }
        // Show the tab that is going to be saved
        textEditorTabbedPane.setSelectedIndex(tabIndex);
        
        int result = networkFileChooser1.showSaveDialog(this);
        
        if (result == NetworkFileChooser.CANCEL_OPTION) {
            return false;
        } else if (result == NetworkFileChooser.APPROVE_OPTION) {
            String filePath = networkFileChooser1.getChoosenFullFilePath();
            NetworkFileSystem nfs = networkFileChooser1.getChosenNetworkFileSystem();

            if(nfs == null || filePath == null)
                return false;
            
            if (nfs.fileExists(filePath)) {
                int response = JOptionPane.showConfirmDialog(null,
                        "Overwrite existing file?",
                        "Overwrite?", JOptionPane.OK_CANCEL_OPTION,
                        JOptionPane.QUESTION_MESSAGE);

                if (response == JOptionPane.CANCEL_OPTION) {
                    return false;
                }
            }
            
            if (writeFileNetwork(nfs, filePath,
                    tabbedDocumentVector.elementAt(tabIndex).
                    getTextPane().getText())) {

                tabbedDocumentVector.elementAt(tabIndex).
                        saveDocument(filePath, nfs);
                tabIsNotModified(tabIndex);
                return true;
            }
            //write failed for some reason
            return false;
        } else {
            //network file chooser returned without "Cancel" or "Approve"
            return false;
        }
    }
    
    private boolean writeFileLocal(File file, String data) {
        try {
            PrintWriter out =
                    new PrintWriter(new BufferedWriter(new FileWriter(file)));
            out.print(data);
            out.flush();
            out.close();
        } catch (IOException e) {
            e.printStackTrace();
            return false;
        }
        return true;
    }
    
    private boolean writeFileNetwork(NetworkFileSystem fileSystem, String filePath, String data) {
        if(fileSystem != null)
            return fileSystem.writeToFile(filePath, data);
        else
            return false;
    }

    private String readLocalFile(File file) {

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
    private String readNetworkFile(NetworkFileSystem networkFileSystem, String filePath) {
        if(networkFileSystem == null || filePath == null)
            return null;
        return networkFileSystem.readFromFile(filePath);
    }
        
    /**
     * Checks to see if a particular TabbedDocument is already open in the TabbedEditor
     * @param file The particular TabbedDocument to check the status of.
     * @return true if the the TabbedDocument is already open.  
     * 
     * false if the File is not open or if the File is null
     */
    public boolean fileIsOpen(File file) {
        for (TabbedDocument docIter : tabbedDocumentVector) {
            if (docIter.equals(file))
                return true;
        }
        return false;
    }

    public boolean fileIsOpen(NetworkFileSystem networkFileSystem, String path) {
        for (TabbedDocument docIter : tabbedDocumentVector) {
            if (docIter.equals(networkFileSystem, path))
                return true;
        }
        return false;
    }
    //Returns -1 if the file is not currently open in a tab
    public int getIndex(File file) {
        for (TabbedDocument docIter : tabbedDocumentVector) {
            if (docIter.equals(file))
                return docIter.getTabIndex();
        }
        return -1;
    }   
    //Returns -1 if the file is not currently open in a tab
    public int getIndex(NetworkFileSystem networkFileSystem, String path) {
        for (TabbedDocument docIter : tabbedDocumentVector) {
            if (docIter.equals(networkFileSystem, path))
                return docIter.getTabIndex();
        }
        return -1;
    }
    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        networkFileChooser1 = new edu.stanford.atom.sti.RemoteFileServer.NetworkFileChooser.NetworkFileChooser();
        toolbarPanel = new javax.swing.JPanel();
        newButton = new javax.swing.JButton();
        openButton = new javax.swing.JButton();
        saveButton = new javax.swing.JButton();
        jSeparator1 = new javax.swing.JSeparator();
        jSplitPane2 = new javax.swing.JSplitPane();
        mainFileLabel = new javax.swing.JLabel();
        filesComboBox = new javax.swing.JComboBox();
        parseButton = new javax.swing.JButton();
        textEditorSplitPane = new javax.swing.JSplitPane();
        textEditorTabbedPane = new javax.swing.JTabbedPane();
        parserScrollPane = new javax.swing.JScrollPane();
        parserTextArea = new javax.swing.JTextArea();

        setPreferredSize(new java.awt.Dimension(600, 600));

        newButton.setText("New");
        newButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                newButtonActionPerformed(evt);
            }
        });

        openButton.setText("Open");

        saveButton.setText("Save");
        saveButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                saveButtonActionPerformed(evt);
            }
        });

        jSeparator1.setOrientation(javax.swing.SwingConstants.VERTICAL);

        jSplitPane2.setBorder(null);
        jSplitPane2.setDividerSize(2);
        jSplitPane2.setOrientation(javax.swing.JSplitPane.VERTICAL_SPLIT);

        mainFileLabel.setText("Main File:");
        jSplitPane2.setTopComponent(mainFileLabel);

        jSplitPane2.setBottomComponent(filesComboBox);

        parseButton.setText("Parse");

        javax.swing.GroupLayout toolbarPanelLayout = new javax.swing.GroupLayout(toolbarPanel);
        toolbarPanel.setLayout(toolbarPanelLayout);
        toolbarPanelLayout.setHorizontalGroup(
            toolbarPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(toolbarPanelLayout.createSequentialGroup()
                .addGap(6, 6, 6)
                .addComponent(newButton, javax.swing.GroupLayout.PREFERRED_SIZE, 53, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(openButton, javax.swing.GroupLayout.PREFERRED_SIZE, 59, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(saveButton)
                .addGap(14, 14, 14)
                .addComponent(jSeparator1, javax.swing.GroupLayout.PREFERRED_SIZE, 11, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(parseButton)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(jSplitPane2, javax.swing.GroupLayout.PREFERRED_SIZE, 219, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(94, Short.MAX_VALUE))
        );
        toolbarPanelLayout.setVerticalGroup(
            toolbarPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(toolbarPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(toolbarPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(toolbarPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                        .addComponent(newButton, javax.swing.GroupLayout.PREFERRED_SIZE, 34, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addComponent(openButton, javax.swing.GroupLayout.PREFERRED_SIZE, 34, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addComponent(saveButton, javax.swing.GroupLayout.PREFERRED_SIZE, 34, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addGroup(toolbarPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING, false)
                        .addComponent(parseButton, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                        .addComponent(jSplitPane2, javax.swing.GroupLayout.DEFAULT_SIZE, 36, Short.MAX_VALUE)))
                .addContainerGap())
            .addComponent(jSeparator1, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 58, Short.MAX_VALUE)
        );

        textEditorSplitPane.setDividerLocation(440);
        textEditorSplitPane.setDividerSize(4);
        textEditorSplitPane.setOrientation(javax.swing.JSplitPane.VERTICAL_SPLIT);
        textEditorSplitPane.setResizeWeight(1.0);

        textEditorTabbedPane.setTabPlacement(javax.swing.JTabbedPane.BOTTOM);
        textEditorTabbedPane.setMinimumSize(new java.awt.Dimension(1, 1));
        textEditorSplitPane.setTopComponent(textEditorTabbedPane);

        parserScrollPane.setBorder(javax.swing.BorderFactory.createTitledBorder("Messages"));
        parserScrollPane.setMinimumSize(new java.awt.Dimension(23, 50));

        parserTextArea.setColumns(20);
        parserTextArea.setRows(5);
        parserScrollPane.setViewportView(parserTextArea);

        textEditorSplitPane.setRightComponent(parserScrollPane);

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(toolbarPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
            .addComponent(textEditorSplitPane, javax.swing.GroupLayout.DEFAULT_SIZE, 600, Short.MAX_VALUE)
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addComponent(toolbarPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(textEditorSplitPane, javax.swing.GroupLayout.DEFAULT_SIZE, 536, Short.MAX_VALUE))
        );
    }// </editor-fold>//GEN-END:initComponents

    private void newButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_newButtonActionPerformed
        createNewFile();
    }//GEN-LAST:event_newButtonActionPerformed

    private void saveButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_saveButtonActionPerformed
        // TODO add your handling code here:
        saveActiveTab();
    }//GEN-LAST:event_saveButtonActionPerformed
    
    
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JComboBox filesComboBox;
    private javax.swing.JSeparator jSeparator1;
    private javax.swing.JSplitPane jSplitPane2;
    private javax.swing.JLabel mainFileLabel;
    private edu.stanford.atom.sti.RemoteFileServer.NetworkFileChooser.NetworkFileChooser networkFileChooser1;
    private javax.swing.JButton newButton;
    private javax.swing.JButton openButton;
    private javax.swing.JButton parseButton;
    private javax.swing.JScrollPane parserScrollPane;
    private javax.swing.JTextArea parserTextArea;
    private javax.swing.JButton saveButton;
    private javax.swing.JSplitPane textEditorSplitPane;
    private javax.swing.JTabbedPane textEditorTabbedPane;
    private javax.swing.JPanel toolbarPanel;
    // End of variables declaration//GEN-END:variables
    
}
