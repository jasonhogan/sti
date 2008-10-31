/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.RemoteFileServer.NetworkFileChooser;

import javax.swing.*;
import javax.swing.table.*;
import edu.stanford.atom.sti.RemoteFileServer.comm.corba.*;
import javax.swing.JFileChooser;
/**
 *
 * @author Jason
 */
public class FileTableCellRenderer extends DefaultTableCellRenderer  {

    Icon fileIcon = UIManager.getIcon("FileView.fileIcon");
    Icon directoryIcon = UIManager.getIcon("FileView.directoryIcon");
     
    @Override
    protected void setValue(Object value) {
        if(value instanceof TFile) {
            TFile tFile = (TFile)value;
            setText(tFile.filename);
            if(tFile.isDirectory) {
                setIcon(directoryIcon);
            }
            else {
                setIcon(fileIcon);
            }
        }
    }
}
