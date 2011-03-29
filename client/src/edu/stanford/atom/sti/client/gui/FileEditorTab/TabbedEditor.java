/** @file TabbedEditor.java
 *  @author Jason Michael Hogan
 *  @brief Source-file for the class TabbedEditor
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

import javax.swing.*;
import javax.swing.event.CaretListener;
import javax.swing.event.CaretEvent;
import java.util.Vector;
import java.io.*;
import javax.swing.text.*;

import java.awt.Component;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseListener;
import java.awt.event.MouseEvent;

import edu.stanford.atom.sti.RemoteFileServer.NetworkFileChooser.*;
import edu.stanford.atom.sti.corba.Client_Server.*;
import edu.stanford.atom.sti.client.gui.state.*;

import edu.stanford.atom.sti.client.comm.io.STIServerConnection;

import edu.stanford.atom.sti.client.comm.io.ParseEventListener;
import edu.stanford.atom.sti.client.comm.io.MessageEventListener;

public class TabbedEditor extends javax.swing.JPanel implements MessageEventListener, STIStateListener {

    private enum fileError {ReadOnly, ReadError, FileIsOpen, NoError}
    private Parser parserRef = null;
    
    private Vector<TabbedDocument> tabbedDocumentVector = new Vector<TabbedDocument>();
    JFileChooser localFileChooser = null;
    
    private TabbedDocument mainFile = null;
    
    /** Creates new form TabbedEditor */
    public TabbedEditor() {
        initComponents();
        lineLabel.setText("");
        fontSizeSpinner.getModel().setValue(14);
        fontSizeSpinner.setEnabled(false);

    }
    
    public synchronized void handleEvent(edu.stanford.atom.sti.corba.Pusher.TMessageEvent event) {
        if(event.type == edu.stanford.atom.sti.corba.Pusher.MessageType.ParsingMessage ) {

            if(event.clearFirst)
                parserTextArea.setText("");



            if(event.linesBack > 0 || event.charsBack > 0) {
                int offset = 0;
                int lines = parserTextArea.getLineCount();


                try {
                    offset = parserTextArea.getLineEndOffset(lines - event.linesBack - 1);
//                    String text = parserTextArea.getText();
                    offset--;   //for newline
                 //   char test = text.charAt(offset);
                    offset -= event.charsBack;
//                    System.out.println("offset = " + offset + ", lines = " + lines);
                    parserTextArea.insert(event.message, offset);
                } catch(BadLocationException e) {
                    parserTextArea.append(event.message);
                }
            } else {
                parserTextArea.append(event.message);               
            }

            parserTextArea.setCaretPosition(
                    parserTextArea.getDocument().getLength());
        }
    }

    public void selectMainFile() {
        if (mainFile != null)
        {
            textEditorTabbedPane.setSelectedIndex(mainFile.getTabIndex());
        }
    }
    
    public boolean saveMainFile() {
        if(mainFile != null)
            return saveNetwork(mainFile.getTabIndex());
        return false;
    }
 
    public void updateMode(STIStateEvent event) {
    }
    public void updateRunType(STIStateEvent event) {
    }
    
    public void updateState(STIStateEvent event) {
        switch( event.state() ) {
            case Disconnected:
            case IdleParsed:
            case IdleUnparsed:
                networkFileComboBox.setEnabled( mainFileIsValid() );
                break;
            case Parsing:
//                parserTextArea.setText("");
            case Connecting:
            case Running:
            case Paused:
            default:
                networkFileComboBox.setEnabled(false);
                break;
        }
    }
    public void setMainFileComboBoxModel(javax.swing.ComboBoxModel model) {
        networkFileComboBox.setModel(model);
    }
    
    public String getMainFilePath() {
        if(mainFile == null)
            return null;
        return mainFile.getPath();
    }
    public boolean mainFileIsValid() {
        
        int escape = 10;    //avoids infinte loops...

        TabbedDocument doc = mainFile;
        
        if(doc == null) {
            return false;
        }
        
        while(doc.isLocalFile() && (escape--) > 0) {
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
        while (doc.isModifed() && (escape--) > 0) {
            if( !save(doc.getTabIndex()) ) {
                return false;
            }
        }
        return true;
    }
    public void setParser(Parser parser) {
        parserRef = parser;
    }
    
    //this function belongs somewhere else, not in this GUI class.
    public boolean parseFile(STIServerConnection connection) {

        boolean parseSuccess = false;

        if (parserRef == null) {
            return false;
        }

        //Create a callback servant so the server can post messages to the 
        //'Messages' text area.
//        Messenger parserListener = connection.getServerMessenger(
//                new ServerMessageListener() {
//
//                    public void sendMessage(String message) {
//                        parserTextArea.append(message);
//                        parserTextArea.setCaretPosition(
//                                parserTextArea.getDocument().getLength());
//                    }
//                });
//
//        parserTextArea.setText("");

        try {
             //The server ip address is not needed since
            //currently the python parser only looks for files on the server.
            parseSuccess = !parserRef.parseFile( getMainFilePath() );
 
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }

        return parseSuccess;
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
//        textEditorTabbedPane.setTabComponentAt(textEditorTabbedPane.getTabCount() - 1,
//                new JLabel(tabbedDocument.getTabTitle())
//                );

        Component tabTitleComponent = textEditorTabbedPane.
                getTabComponentAt(textEditorTabbedPane.getTabCount() - 1);
        
        tabTitleComponent.addMouseListener(
                new MouseAdapter() {

                    public void mousePressed(MouseEvent e) {
                        maybeShowPopup(e);
                    }

                    public void mouseReleased(MouseEvent e) {
                        maybeShowPopup(e);
                    }

                    private void maybeShowPopup(MouseEvent e) {
                        if (e.isPopupTrigger()) {
                            tabPopupMenu.show(e.getComponent(),
                                    e.getX(), e.getY());

                        }
                        int index = textEditorTabbedPane.indexOfTabComponent(e.getComponent());

                        if(index != -1) {
                            textEditorTabbedPane.setSelectedIndex(index);
                        }
                    }
                });


        tabbedDocument.getTextPane().addCaretListener(new CaretListener() {
             public void caretUpdate(CaretEvent e) {
                 STITextPane textPane = (STITextPane)e.getSource();
                 
                 lineLabel.setText("Ln " + textPane.getLineNumber());
                 columnLabel.setText("Col " + textPane.getColumnNumber());
             }
        });
    }
    
    public void insertEditorTab(TabbedDocument tabbedDocument, int index) {
        if(index >=0 && index < textEditorTabbedPane.getTabCount()) {
            textEditorTabbedPane.insertTab(tabbedDocument.getTabTitle(), null, 
                    tabbedDocument, null, index);
            reIndexTabbedDocuments();
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
        if(tabbedDocumentVector.elementAt(tabIndex).isNetworkFile()) {
            networkFileComboBox.removeItem(tabbedDocumentVector.elementAt(tabIndex));
           // consoleMainFileComboBox.removeItem(tabbedDocumentVector.elementAt(tabIndex));
        }

        textEditorTabbedPane.removeTabAt(tabIndex);
        tabbedDocumentVector.remove(tabIndex);
        reIndexTabbedDocuments(tabIndex);
    }

    private void reIndexTabbedDocuments() {
        reIndexTabbedDocuments(0);
    }
    private void reIndexTabbedDocuments(int start) {
        //Reindex so TabbedDocument indicies match JTabbedPane indicies
        for (int i = start; i < tabbedDocumentVector.size(); i++) {
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
        
        // By default, the Main File is the first network file opened or saved.
        if(mainFile == null) {
            mainFile = tabbedDocumentVector.elementAt(tabIndex);
        }
        networkFileComboBox.addItem(tabbedDocumentVector.elementAt(tabIndex));
        
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
            boolean writeSuccess = false;
            
            if( !tabbedDocumentVector.elementAt(tabIndex).
                    getNetworkFileSystem().isAlive() ) {
                //file server is dead
                writeSuccess = false;
            }
            else {
               writeSuccess = writeFileNetwork(
                       tabbedDocumentVector.elementAt(tabIndex).
                       getNetworkFileSystem(),
                       tabbedDocumentVector.elementAt(tabIndex).getPath(),
                       tabbedDocumentVector.elementAt(tabIndex).
                       getTextPane().getText());
            }
            if(writeSuccess) {
                tabIsNotModified(tabIndex);
            }
            else {
                //failed to write to the network file server
                JOptionPane.showMessageDialog(this,"Failed to save file '" +
                    tabbedDocumentVector.elementAt(tabIndex).getFileName() +
                    "' to the network.\n " +
                    "Could not write to the network file server.",
                    "Network File Server Error",
                    JOptionPane.ERROR_MESSAGE);
                networkFileChooser1.refreshRemoteServers();
            }
            return writeSuccess;
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

        TabbedDocument doc = tabbedDocumentVector.elementAt(tabIndex);

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

            //Check to see if this file is already open
            if (fileIsOpen(file)) {
                int alreadyOpenIndex = getIndex(file);
                if (alreadyOpenIndex >= 0 && alreadyOpenIndex < textEditorTabbedPane.getTabCount()) {
                    //This file is already open.  Discard changes.

                    int response = JOptionPane.showConfirmDialog(null,
                        "This file is currently open. \nOverwrite existing file?",
                        "Overwrite open file?", JOptionPane.OK_CANCEL_OPTION,
                        JOptionPane.WARNING_MESSAGE);

                    if (response == JOptionPane.CANCEL_OPTION) {
                        return false;
                    }
                    
                    //remove the overwritten document
                    removeEditorTab(alreadyOpenIndex);

                }
            }
            else if (file.exists()) {
                int response = JOptionPane.showConfirmDialog(null,
                        "Overwrite existing file?",
                        "Overwrite?", JOptionPane.OK_CANCEL_OPTION,
                        JOptionPane.QUESTION_MESSAGE);

                if (response == JOptionPane.CANCEL_OPTION) {
                    return false;
                }
            }

            if (writeFileLocal(file,
                    doc.getTextPane().getText())) {

                doc.saveDocument(file);
                tabIsNotModified(doc.getTabIndex());
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

        TabbedDocument doc = tabbedDocumentVector.elementAt(tabIndex);

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

            //Check to see if this file is already open
            if (fileIsOpen(nfs, filePath)) {
                int alreadyOpenIndex = getIndex(nfs, filePath);
                if (alreadyOpenIndex >= 0 && alreadyOpenIndex < textEditorTabbedPane.getTabCount()) {
                    //This file is already open.  Discard changes.

                    int response = JOptionPane.showConfirmDialog(null,
                        "This file is currently open. \nOverwrite existing file?",
                        "Overwrite open file?", JOptionPane.OK_CANCEL_OPTION,
                        JOptionPane.WARNING_MESSAGE);

                    if (response == JOptionPane.CANCEL_OPTION) {
                        return false;
                    }
                    
                    //remove the overwritten document
                    removeEditorTab(alreadyOpenIndex);

                }
            }
            else if (nfs.fileExists(filePath)) {

                int response = JOptionPane.showConfirmDialog(null,
                        "Overwrite existing file?",
                        "Overwrite?", JOptionPane.OK_CANCEL_OPTION,
                        JOptionPane.WARNING_MESSAGE);

                if (response == JOptionPane.CANCEL_OPTION) {
                    return false;
                }
            }
            
            if (writeFileNetwork(nfs, filePath,
                    doc.getTextPane().getText())) {

                //Check if the original tab is already in the Main File combo box
                boolean alreadyInMainFileList = doc.isNetworkFile();

                doc.saveDocument(filePath, nfs);
                tabIsNotModified(doc.getTabIndex());
                
                // By default, the Main File is the first network file opened or saved.
                if (mainFile == null) {
                    mainFile = doc;
                }
                if(!alreadyInMainFileList) {
                    networkFileComboBox.addItem(doc);
                }
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
        tabPopupMenu = new javax.swing.JPopupMenu();
        popupSaveMenuItem = new javax.swing.JMenuItem();
        popupCloseMenuItem = new javax.swing.JMenuItem();
        toolbarPanel = new javax.swing.JPanel();
        newButton = new javax.swing.JButton();
        openButton = new javax.swing.JButton();
        saveButton = new javax.swing.JButton();
        jSeparator1 = new javax.swing.JSeparator();
        jSplitPane2 = new javax.swing.JSplitPane();
        mainFileLabel = new javax.swing.JLabel();
        networkFileComboBox = new javax.swing.JComboBox();
        jPanel1 = new javax.swing.JPanel();
        lineLabel = new javax.swing.JLabel();
        columnLabel = new javax.swing.JLabel();
        jSeparator2 = new javax.swing.JSeparator();
        fontSizeSpinner = new javax.swing.JSpinner();
        jSeparator3 = new javax.swing.JSeparator();
        jLabel1 = new javax.swing.JLabel();
        textEditorSplitPane = new javax.swing.JSplitPane();
        textEditorTabbedPane = new STITabbedPane();
        parserScrollPane = new javax.swing.JScrollPane();
        parserTextArea = new javax.swing.JTextArea();

        popupSaveMenuItem.setText("Save");
        popupSaveMenuItem.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                popupSaveMenuItemActionPerformed(evt);
            }
        });
        tabPopupMenu.add(popupSaveMenuItem);

        popupCloseMenuItem.setText("Close");
        popupCloseMenuItem.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                popupCloseMenuItemActionPerformed(evt);
            }
        });
        tabPopupMenu.add(popupCloseMenuItem);

        setPreferredSize(new java.awt.Dimension(600, 600));

        newButton.setIcon(new javax.swing.ImageIcon(getClass().getResource("/edu/stanford/atom/sti/client/resources/New24.gif"))); // NOI18N
        newButton.setToolTipText("Create new file");
        newButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                newButtonActionPerformed(evt);
            }
        });

        openButton.setIcon(new javax.swing.ImageIcon(getClass().getResource("/edu/stanford/atom/sti/client/resources/Open24.gif"))); // NOI18N
        openButton.setToolTipText("Open file");
        openButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                openButtonActionPerformed(evt);
            }
        });

        saveButton.setIcon(new javax.swing.ImageIcon(getClass().getResource("/edu/stanford/atom/sti/client/resources/Save24.gif"))); // NOI18N
        saveButton.setToolTipText("Save file");
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

        networkFileComboBox.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                networkFileComboBoxActionPerformed(evt);
            }
        });
        jSplitPane2.setBottomComponent(networkFileComboBox);

        lineLabel.setText("Ln ");

        columnLabel.setText("Col");

        javax.swing.GroupLayout jPanel1Layout = new javax.swing.GroupLayout(jPanel1);
        jPanel1.setLayout(jPanel1Layout);
        jPanel1Layout.setHorizontalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel1Layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(lineLabel, javax.swing.GroupLayout.DEFAULT_SIZE, 75, Short.MAX_VALUE)
                    .addComponent(columnLabel, javax.swing.GroupLayout.DEFAULT_SIZE, 75, Short.MAX_VALUE))
                .addContainerGap())
        );
        jPanel1Layout.setVerticalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel1Layout.createSequentialGroup()
                .addComponent(lineLabel)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(columnLabel)
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        jSeparator2.setOrientation(javax.swing.SwingConstants.VERTICAL);

        fontSizeSpinner.setValue(14);
        fontSizeSpinner.addChangeListener(new javax.swing.event.ChangeListener() {
            public void stateChanged(javax.swing.event.ChangeEvent evt) {
                fontSizeSpinnerStateChanged(evt);
            }
        });

        jSeparator3.setOrientation(javax.swing.SwingConstants.VERTICAL);

        jLabel1.setText("Font Size");

        javax.swing.GroupLayout toolbarPanelLayout = new javax.swing.GroupLayout(toolbarPanel);
        toolbarPanel.setLayout(toolbarPanelLayout);
        toolbarPanelLayout.setHorizontalGroup(
            toolbarPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(toolbarPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(newButton, javax.swing.GroupLayout.PREFERRED_SIZE, 35, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(openButton, javax.swing.GroupLayout.PREFERRED_SIZE, 35, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(saveButton, javax.swing.GroupLayout.PREFERRED_SIZE, 35, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(jSeparator1, javax.swing.GroupLayout.PREFERRED_SIZE, 11, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jSplitPane2, javax.swing.GroupLayout.DEFAULT_SIZE, 129, Short.MAX_VALUE)
                .addGap(32, 32, 32)
                .addComponent(jSeparator2, javax.swing.GroupLayout.PREFERRED_SIZE, 17, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jLabel1, javax.swing.GroupLayout.PREFERRED_SIZE, 48, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(fontSizeSpinner, javax.swing.GroupLayout.PREFERRED_SIZE, 69, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(18, 18, 18)
                .addComponent(jSeparator3, javax.swing.GroupLayout.PREFERRED_SIZE, 5, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(9, 9, 9)
                .addComponent(jPanel1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap())
        );
        toolbarPanelLayout.setVerticalGroup(
            toolbarPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, toolbarPanelLayout.createSequentialGroup()
                .addGroup(toolbarPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                    .addGroup(javax.swing.GroupLayout.Alignment.LEADING, toolbarPanelLayout.createSequentialGroup()
                        .addContainerGap()
                        .addGroup(toolbarPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(saveButton, javax.swing.GroupLayout.PREFERRED_SIZE, 35, javax.swing.GroupLayout.PREFERRED_SIZE)
                            .addComponent(openButton, javax.swing.GroupLayout.PREFERRED_SIZE, 35, javax.swing.GroupLayout.PREFERRED_SIZE)
                            .addComponent(newButton, javax.swing.GroupLayout.PREFERRED_SIZE, 35, javax.swing.GroupLayout.PREFERRED_SIZE)
                            .addComponent(jSplitPane2, javax.swing.GroupLayout.PREFERRED_SIZE, 36, javax.swing.GroupLayout.PREFERRED_SIZE))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 8, Short.MAX_VALUE))
                    .addComponent(jSeparator1, javax.swing.GroupLayout.DEFAULT_SIZE, 55, Short.MAX_VALUE)
                    .addGroup(javax.swing.GroupLayout.Alignment.LEADING, toolbarPanelLayout.createSequentialGroup()
                        .addContainerGap()
                        .addGroup(toolbarPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                            .addComponent(fontSizeSpinner, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                            .addComponent(jLabel1)))
                    .addGroup(javax.swing.GroupLayout.Alignment.LEADING, toolbarPanelLayout.createSequentialGroup()
                        .addGap(5, 5, 5)
                        .addGroup(toolbarPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(jSeparator2, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 50, Short.MAX_VALUE)
                            .addComponent(jSeparator3, javax.swing.GroupLayout.DEFAULT_SIZE, 50, Short.MAX_VALUE)
                            .addComponent(jPanel1, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))))
                .addContainerGap())
        );

        textEditorSplitPane.setDividerLocation(360);
        textEditorSplitPane.setDividerSize(4);
        textEditorSplitPane.setOrientation(javax.swing.JSplitPane.VERTICAL_SPLIT);
        textEditorSplitPane.setResizeWeight(1.0);
        textEditorSplitPane.setMinimumSize(new java.awt.Dimension(25, 1));

        textEditorTabbedPane.setTabPlacement(javax.swing.JTabbedPane.BOTTOM);
        textEditorTabbedPane.setMinimumSize(new java.awt.Dimension(1, 1));
        textEditorTabbedPane.addChangeListener(new javax.swing.event.ChangeListener() {
            public void stateChanged(javax.swing.event.ChangeEvent evt) {
                textEditorTabbedPaneStateChanged(evt);
            }
        });
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
                .addComponent(toolbarPanel, javax.swing.GroupLayout.PREFERRED_SIZE, 54, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(textEditorSplitPane, javax.swing.GroupLayout.DEFAULT_SIZE, 540, Short.MAX_VALUE))
        );
    }// </editor-fold>//GEN-END:initComponents

    private void newButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_newButtonActionPerformed
        createNewFile();
    }//GEN-LAST:event_newButtonActionPerformed

    private void saveButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_saveButtonActionPerformed
        // TODO add your handling code here:
        saveActiveTab();
    }//GEN-LAST:event_saveButtonActionPerformed

    private void openButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_openButtonActionPerformed
        // TODO add your handling code here:
        openNetworkFile();
    }//GEN-LAST:event_openButtonActionPerformed

    private void popupCloseMenuItemActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_popupCloseMenuItemActionPerformed
        closeFileInActiveTab();
    }//GEN-LAST:event_popupCloseMenuItemActionPerformed

    private void popupSaveMenuItemActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_popupSaveMenuItemActionPerformed
        saveActiveTab();
    }//GEN-LAST:event_popupSaveMenuItemActionPerformed

    private void networkFileComboBoxActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_networkFileComboBoxActionPerformed
        // TODO add your handling code here:
    }//GEN-LAST:event_networkFileComboBoxActionPerformed

    private void fontSizeSpinnerStateChanged(javax.swing.event.ChangeEvent evt) {//GEN-FIRST:event_fontSizeSpinnerStateChanged
        int fontSize = (Integer)fontSizeSpinner.getModel().getValue();

        int minFont = 1;
        int maxFont = 100;

        if(fontSize < minFont) {
            ((javax.swing.JSpinner) evt.getSource()).setValue(minFont);
        }
        else if (fontSize > maxFont) {
            ((javax.swing.JSpinner) evt.getSource()).setValue(maxFont);
        }
        fontSize = (Integer)fontSizeSpinner.getModel().getValue();

        TabbedDocument doc = getSelectedTabbedDocument();

        if(doc != null) {
            doc.setFontSize(fontSize);
        }
        
    }//GEN-LAST:event_fontSizeSpinnerStateChanged

    private void textEditorTabbedPaneStateChanged(javax.swing.event.ChangeEvent evt) {//GEN-FIRST:event_textEditorTabbedPaneStateChanged

        fontSizeSpinner.setEnabled(textEditorTabbedPane.getComponentCount() > 0);

        TabbedDocument doc = getSelectedTabbedDocument();
        if(doc != null) {
            fontSizeSpinner.getModel().setValue(doc.getFontSize());
        } else {
            fontSizeSpinner.setEnabled(false);
        }

    }//GEN-LAST:event_textEditorTabbedPaneStateChanged

    public void mainFileComboBoxActionPerformed(java.awt.event.ActionEvent evt) {
        //called from an action listener initially attached to the main file combo box in the sti_console class
        networkFileComboBox.setSelectedItem(
                ( (javax.swing.JComboBox)evt.getSource() ).getSelectedItem() );
        
        if(networkFileComboBox.getComponentCount() > 0) {
            mainFile = (TabbedDocument) networkFileComboBox.getSelectedItem();
        }
    }    



    
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JLabel columnLabel;
    private javax.swing.JSpinner fontSizeSpinner;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JSeparator jSeparator1;
    private javax.swing.JSeparator jSeparator2;
    private javax.swing.JSeparator jSeparator3;
    private javax.swing.JSplitPane jSplitPane2;
    private javax.swing.JLabel lineLabel;
    private javax.swing.JLabel mainFileLabel;
    private edu.stanford.atom.sti.RemoteFileServer.NetworkFileChooser.NetworkFileChooser networkFileChooser1;
    private javax.swing.JComboBox networkFileComboBox;
    private javax.swing.JButton newButton;
    private javax.swing.JButton openButton;
    private javax.swing.JScrollPane parserScrollPane;
    private javax.swing.JTextArea parserTextArea;
    private javax.swing.JMenuItem popupCloseMenuItem;
    private javax.swing.JMenuItem popupSaveMenuItem;
    private javax.swing.JButton saveButton;
    private javax.swing.JPopupMenu tabPopupMenu;
    private javax.swing.JSplitPane textEditorSplitPane;
    private javax.swing.JTabbedPane textEditorTabbedPane;
    private javax.swing.JPanel toolbarPanel;
    // End of variables declaration//GEN-END:variables
    
}
