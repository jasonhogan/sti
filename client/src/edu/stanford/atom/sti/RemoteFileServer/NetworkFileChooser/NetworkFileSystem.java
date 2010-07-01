/** @file NetworkFileSystem.java
 *  @author Jason Michael Hogan
 *  @brief Source-file for the class NetworkFileSystem
 *  @section license License
 *
 *  Copyright (C) 2008 Jason Hogan <hogan@stanford.edu>\n\
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

package edu.stanford.atom.sti.RemoteFileServer.NetworkFileChooser;

import edu.stanford.atom.sti.RemoteFileServer.comm.corba.*;
import org.omg.CORBA.*;

public class NetworkFileSystem {
    
    private String ipAddress = null;
    private String portNumber = null;
    private ORB orb;
    
    private FileServer remoteFileServer = null;
    
    public static NetworkFileSystem getRemoteFileServer(String IPAddress, String PortNumber) {
        return new NetworkFileSystem(IPAddress, PortNumber);
    }
    
    public NetworkFileSystem(String IPAddress, String PortNumber) {

        ipAddress = IPAddress;
        portNumber = PortNumber;

        try {
            String[] extendedArgs = {"-ORBInitialPort", portNumber, "-ORBInitialHost", ipAddress};
            // create and initialize the ORB
            orb = ORB.init(extendedArgs, null);

            org.omg.CORBA.Object serverObj = orb.string_to_object(
                    "corbaname::" + ipAddress + ":" + portNumber +
                    "#STI/FileServer/FileServer.obj");
            
            remoteFileServer = FileServerHelper.narrow(serverObj);

        } catch (Exception e) {
            ipAddress = null;
            portNumber = null;
            e.printStackTrace(System.out);
        }
    }
    
    public boolean writeToFile(String path, String data) {
        try {
            return remoteFileServer.writeData(path, data);
        } catch (Exception e) {
            return false;
        }
    }
    public String readFromFile(String path) {
        String data = null;

        try {
            data = remoteFileServer.readData(path);
        } catch (Exception e) {
        }
        return data;
    }
    
    public String getIP() {
        return ipAddress;
    }
    public String getFullAddress() {
        return (ipAddress + ":" + portNumber);
    }    
    public boolean isAlive() {
        boolean nonExistent;
        if(remoteFileServer == null) {
            return false;
        }
        try {
            nonExistent = remoteFileServer._non_existent();
        } 
        catch (Exception e) {
            return false;
        }
        try {
            remoteFileServer.homeDirectory();
        }
        catch (Exception e) {
            return false;
        }
        return true;
    }
    
    public boolean equals(NetworkFileSystem networkFileSystem) {
        return (ipAddress.equals(networkFileSystem.getIP()));
    }
    
    public String homeDirectory() {
        try {
            return remoteFileServer.homeDirectory();
        } catch(Exception e) {
            return null;
        }
    }

    public TFile[] getFiles(String dir) {
        try {
            return remoteFileServer.getFiles(dir);
        } catch(Exception e) {
            return null;
        }
    }
    public boolean fileExists(String file) {
        try {
            return remoteFileServer.exists(file);
        } catch(Exception e) {
            return false;
        }
    }
    public boolean isDirectory(String dir) {
        try {
            return remoteFileServer.isDirectory(dir);
        } catch(Exception e) {
            return false;
        }
    }
    
    public boolean isReadOnly(String dir) {
        return false;
    }
    
    public String getRootDirectory() {
        String slash = getSeparator();
        String home = homeDirectory();
        
        int index = home.indexOf(slash);
        

        if(index > 0)
            return home.substring(0, index);
        else
            return home;
    }
    public String longFileName(String path, String file) {
        String separator = getSeparator();
        if(path == null || file == null || separator == null)
            return null;
        if( path.lastIndexOf(separator) < (path.length() - 1) ) {
            return (path + separator + file);
        }
        return (path + file);
    }
    
    public String shortFileName(String file) {
        int slashIndex = file.lastIndexOf(remoteFileServer.getSeparator());
        return file.substring(slashIndex + 1);
    }
    
    public TFile shortFileName(TFile tFile) {
        int slashIndex = tFile.filename.lastIndexOf(getSeparator());
        tFile.filename = tFile.filename.substring(slashIndex + 1);
        return tFile;
    }
    
    public String getSeparator() {
        try {
            return remoteFileServer.getSeparator();
        } catch(Exception e) {
            return null;
        }
    }
    public TFile getParent(String dir) {
        TFile tFile = new TFile();
        tFile.filename = dir;
        try {
            return remoteFileServer.getParentFile(tFile);
        } catch(Exception e) {
            return null;
        }
    }
    
}
