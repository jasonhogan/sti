/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.comm.io;

//import java.lang.Thread;

import edu.stanford.atom.sti.client.comm.corba.*;
import org.omg.CosNaming.*;
import org.omg.CosNaming.NamingContextPackage.*;
import org.omg.CORBA.*;

import edu.stanford.atom.sti.client.gui.state.STIStateMachine;

/**
 *
 * @author Owner
 */
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
