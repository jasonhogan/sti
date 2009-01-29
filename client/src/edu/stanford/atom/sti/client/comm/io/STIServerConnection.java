/** @file STIServerConnection.java
 *  @author Jason Michael Hogan
 *  @brief Source-file for the class STIServerConnection
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

package edu.stanford.atom.sti.client.comm.io;

import edu.stanford.atom.sti.client.comm.corba.*;
import org.omg.CosNaming.*;
import org.omg.CosNaming.NamingContextPackage.*;
import org.omg.CORBA.*;
import edu.stanford.atom.sti.client.gui.state.STIStateMachine;

public class STIServerConnection implements Runnable {
    
    private STIStateMachine stateMachine_ = null;
    private ORB orb = null;
    private String serverAddress = null;

    //servants
    private DeviceConfigure deviceConfigure = null;   
    private ExpSequence expSequence = null;
    private Parser parser = null;
    private Control control = null;

    public STIServerConnection(STIStateMachine stateMachine) {
        stateMachine_ = stateMachine;
    }
    
    public void run() {
        if(serverAddress != null) {
            connectToServer(serverAddress);
        }
    }
    
    public void setServerAddress(String address) {
        serverAddress = address;
    }
    
    public String getServerAddress() {
        return serverAddress;
    }
    
    public DeviceConfigure getDeviceConfigure() {
        return deviceConfigure;
    }
    
    public ExpSequence getExpSequence() {
        return expSequence;
    }
    
    public Parser getParser() {
        return parser;
    }
    
    public Control getControl() {
        return control;
    }
    
    public void disconnectFromServer() {
        if( referencesAreNotNull() ) {
            try {
                deviceConfigure._release();
                expSequence._release();
                parser._release();
                control._release();
            } catch (Exception e) {
                e.printStackTrace(System.out);
            }
        }
    }
    
    private void connectToServer(String address) {

        String[] serverAddr = address.split(":");
        
        if(serverAddr.length == 1) {    //missing port
            serverAddr = new String[] {serverAddr[0], new String("")};  //use blank port
        }
        else if(serverAddr.length == 0) {
            serverAddr = new String[] {"localhost", new String("")};
        }

        boolean connectionSuccess = false;
        
        try {
            String[] extendedArgs = {"-ORBInitialPort", serverAddr[1], "-ORBInitialHost", serverAddr[0]};
            // create and initialize the ORB
            orb = ORB.init(extendedArgs, null);
               
            org.omg.CORBA.Object deviceObj = orb.string_to_object(
                    "corbaname::" + serverAddr[0] + ":" + serverAddr[1] + 
                    "#STI/Client/DeviceConfigure.Object");    
            deviceConfigure = DeviceConfigureHelper.narrow(deviceObj);
                        
            org.omg.CORBA.Object expSeqObj = orb.string_to_object(
                    "corbaname::" + serverAddr[0] + ":" + serverAddr[1] +
                    "#STI/Client/ExpSequence.Object");
            
            expSequence = ExpSequenceHelper.narrow(expSeqObj);
            
            org.omg.CORBA.Object parserObj = orb.string_to_object(
                    "corbaname::" + serverAddr[0] + ":" + serverAddr[1]+
                    "#STI/Client/Parser.Object");
            
            parser = ParserHelper.narrow(parserObj);
            
            org.omg.CORBA.Object controlObj = orb.string_to_object(
                    "corbaname::" + serverAddr[0] + ":" + serverAddr[1]+
                    "#STI/Client/Control.Object");
            
            control = ControlHelper.narrow(controlObj);
            
            connectionSuccess = checkServerReferences();

        } catch (Exception e) {
            e.printStackTrace(System.out);
        }
        
        if(connectionSuccess) {
            stateMachine_.finishConnecting();
            serverAddress = serverAddr[0] + ":" + serverAddr[1];
        }
        else {
            stateMachine_.disconnect();
        }
    }
    
    private boolean referencesAreNotNull() {
        return (deviceConfigure != null && expSequence != null
                && parser != null && control != null);
    }
    
    private boolean checkServerReferences() {
        boolean alive = false;
        if( referencesAreNotNull() ) {
            try {
                alive  = !deviceConfigure._non_existent();
                alive &= !expSequence._non_existent();
                alive &= !parser._non_existent();
                alive &= !control._non_existent();
            } catch (Exception e) {
                alive = false;
                e.printStackTrace(System.out);
            }
        }
        return alive;
    }
   
}
