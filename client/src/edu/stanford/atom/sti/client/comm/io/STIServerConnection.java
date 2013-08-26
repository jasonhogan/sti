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

import edu.stanford.atom.sti.client.gui.state.STIStateMachine;
import edu.stanford.atom.sti.corba.Client_Server.*;
import java.util.Vector;
import java.util.prefs.Preferences;
import org.omg.CORBA.*;
import org.omg.PortableServer.POA;
import org.omg.PortableServer.POAHelper;

public class STIServerConnection implements Runnable, edu.stanford.atom.sti.client.comm.io.PingEventListener {
    
    private static final String STISERVERADDRESS = "";
    Preferences addressPref = Preferences.userNodeForPackage(this.getClass());

    private STIServerEventHandler eventHandler;
    private edu.stanford.atom.sti.corba.Pusher.ServerCallback serverCallback = null;

    private STIStateMachine stateMachine_ = null;
    private ORB orb = null;
    private POA poa = null;
    private String serverAddress = null;

    //servants
    private RegisteredDevices registeredDevices = null;
    private ExpSequence expSequence = null;
    private Parser parser = null;
    private ServerTimingSeqControl serverTimingSeqControl = null;
    private ServerCommandLine commandLine = null;
    private ClientBootstrap bootstrap = null;
    private DocumentationSettings documentationSettings = null;

    private Vector<ServerConnectionListener> listeners = new Vector<ServerConnectionListener>();
    
    public STIServerConnection(STIStateMachine stateMachine, STIServerEventHandler eventHandler) {
        stateMachine_ = stateMachine;
        this.eventHandler = eventHandler;

        setServerAddress( addressPref.get(STISERVERADDRESS, "localhost:2809") );
    }
    
    public void handleEvent(edu.stanford.atom.sti.corba.Pusher.TPingEvent event) {
        serverCallback = event.callBack;
        
        serverCallback.pingServer();
    }

    public synchronized void addServerConnectionListener(ServerConnectionListener listener) {
        listeners.add(listener);
    }
    public synchronized void removeServerConnectionListener(ServerConnectionListener listener) {
        listeners.remove(listener);
    }
    private synchronized void fireServerConnectedEvent() {
        ServerConnectionEvent event = new ServerConnectionEvent(this);
        
        for (int i = 0; i < listeners.size(); i++) {
            listeners.elementAt(i).installServants(event);
        }
    }
    private synchronized void fireServerDisconnectedEvent() {
        ServerConnectionEvent event = new ServerConnectionEvent(this);
        
        for (int i = 0; i < listeners.size(); i++) {
            listeners.elementAt(i).uninstallServants(event);
        }
    } 

    public void run() {
        if(serverAddress != null) {
            connectToServer(serverAddress);
        }
    }
    
    public final void setServerAddress(String address) {
        serverAddress = address;
    }
    
    public String getServerAddress() {
        return serverAddress;
    }
    
    public RegisteredDevices getRegisteredDevices() {
        return registeredDevices;
    }
    public ExpSequence getExpSequence() {
        return expSequence;
    }
    public Parser getParser() {
        return parser;
    }
    public ServerTimingSeqControl getServerTimingSeqControl() {
        return serverTimingSeqControl;
    }
    public ServerCommandLine getCommandLine() {
        return commandLine;
    }
    
    public DocumentationSettings getDocumentationSettings() {
        return documentationSettings;
    }

    public void disconnectFromServer() {
        if( referencesAreNotNull() ) {
            try {
                registeredDevices._release();
                expSequence._release();
                parser._release();
                serverTimingSeqControl._release();
                commandLine._release();
                documentationSettings._release();
            } catch (Exception e) {
                e.printStackTrace(System.out);
            }
        }
        registeredDevices = null;
        expSequence = null;
        parser = null;
        serverTimingSeqControl = null;
        commandLine = null;
        documentationSettings = null;
        
        if( serverCallback != null ) {
            try {
                serverCallback.disconnectFromServer();
            } catch (Exception e) {
                e.printStackTrace(System.out);
            }
        }

        fireServerDisconnectedEvent();
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
            
            poa = POAHelper.narrow(orb.resolve_initial_references("RootPOA"));
            poa.the_POAManager().activate();

            //TIMEOUTS -- The default java ORB doesn't seem to support this...
//            PolicyManager policyManager = PolicyManagerHelper.narrow(
//                    orb.resolve_initial_references("ORBPolicyManager"));
//            Policy p2 = new org.omg.CORBA
//            Policy p = new org.jacorb.orb.policies.RelativeRoundtripTimeoutPolicy(objtimeout
//                    * 10000000);
//            policyManager.set_policy_overrides(new Policy[]{p}, SetOverrideType.ADD_OVERRIDE);

            //TIMEOUTS


            org.omg.CORBA.Object bootstrapObj = orb.string_to_object(
                    "corbaname::" + serverAddr[0] + ":" + serverAddr[1] + 
                    "#STI/Client/ClientBootstrap.Object");    
        //    bootstrapObj._get_policy(1)
            bootstrap = ClientBootstrapHelper.narrow(bootstrapObj);

            bootstrap.connect(eventHandler._this(orb));

            registeredDevices = bootstrap.getRegisteredDevices();
            expSequence = bootstrap.getExpSequence();
            parser = bootstrap.getParser();
            serverTimingSeqControl = bootstrap.getServerTimingSeqControl();
            commandLine = bootstrap.getServerCommandLine();

            org.omg.CORBA.Object documentationSettingsObj = orb.string_to_object(
                    "corbaname::" + serverAddr[0] + ":" + serverAddr[1]+
                    "#STI/Client/DocumentationSettings.Object");

            documentationSettings = DocumentationSettingsHelper.narrow(documentationSettingsObj);

            connectionSuccess = checkServerReferences();

        } catch (Exception e) {
            e.printStackTrace(System.out);
        }
        
        if (connectionSuccess) {
            stateMachine_.finishConnecting();
            serverAddress = serverAddr[0] + ":" + serverAddr[1];
            addressPref.put(STISERVERADDRESS, serverAddress);
            fireServerConnectedEvent();
        } else {
            stateMachine_.disconnect();
            fireServerDisconnectedEvent();
        }
    }
    
    private boolean referencesAreNotNull() {
        return (registeredDevices != null && expSequence != null
                && parser != null && serverTimingSeqControl != null && commandLine != null && documentationSettings != null);
    }
    public boolean checkServerReferences() {
        boolean alive = false;
        if (referencesAreNotNull()) {
            try {
                alive = !registeredDevices._non_existent();
                alive &= !expSequence._non_existent();
                alive &= !parser._non_existent();
                alive &= !serverTimingSeqControl._non_existent();
                alive &= !commandLine._non_existent();
                alive &= !documentationSettings._non_existent();
            } catch (Exception e) {
                alive = false;
                e.printStackTrace(System.out);
            }
        }
        return alive;
    }
   
}
