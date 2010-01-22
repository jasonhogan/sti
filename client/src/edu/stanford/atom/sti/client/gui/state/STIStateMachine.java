/** @file STIStateMachine.java
 *  @author Jason Michael Hogan
 *  @brief Source-file for the class STIStateMachine
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

package edu.stanford.atom.sti.client.gui.state;

import java.util.Vector;

public class STIStateMachine implements edu.stanford.atom.sti.client.comm.io.StatusEventListener {
    
    public static enum ServerState {                     EventsEmpty,  PreparingEvents, EventsReady, RequestingPlay, PlayingEvents, Paused, Waiting };
    public static enum State { Disconnected, Connecting, IdleUnparsed, Parsing,         IdleParsed,                  Running,       Paused,          RunningDirect };
    public static enum Mode { Direct, Documented, Testing, Monitor };
    public static enum RunType { Single, Sequence };

    private State state = State.Disconnected;
    private Mode mode = Mode.Monitor;
    private RunType runType = RunType.Single;
    
    private Vector<STIStateListener> listeners = new Vector<STIStateListener>();

    public STIStateMachine() {
        listeners.clear();
    }
    
    public void handleEvent(edu.stanford.atom.sti.corba.Pusher.TStatusEvent event) {
        System.out.println("Server state: " + event.state.toString());
    }

    public synchronized void addStateListener(STIStateListener listener) {
        listeners.add(listener);
        listener.updateState( new STIStateEvent(this, state, mode, runType) );
        listener.updateMode( new STIStateEvent(this, state, mode, runType) );
        listener.updateRunType( new STIStateEvent(this, state, mode, runType) );
    }
    public synchronized void removeStateListener(STIStateListener listener) {
        listeners.remove(listener);
    }
    private synchronized void fireStateChangedEvent() {
        STIStateEvent event = new STIStateEvent(this, state, mode, runType);
        
        for(int i = 0; i < listeners.size(); i++) {
            listeners.elementAt(i).updateState( event );
        }
    }
    private synchronized void fireModeChangedEvent() {
        STIStateEvent event = new STIStateEvent(this, state, mode, runType);
        
        for(int i = 0; i < listeners.size(); i++) {
            listeners.elementAt(i).updateMode( event );
        }
    }
    private synchronized void fireRunTypeChangedEvent() {
        STIStateEvent event = new STIStateEvent(this, state, mode, runType);
        
        for(int i = 0; i < listeners.size(); i++) {
            listeners.elementAt(i).updateRunType( event );
        }
    }
    
    private synchronized boolean changeState(State newState) {
        boolean allowedTransition = false;
        
        switch(state) {
            case Disconnected:
                allowedTransition = 
                        newState.equals(State.Connecting);
                break;
            case Connecting:
                allowedTransition = 
                        newState.equals(State.Disconnected) || 
                        newState.equals(State.IdleUnparsed);
                break;
            case IdleUnparsed:
                allowedTransition = 
                        newState.equals(State.Disconnected) || 
                        newState.equals(State.Parsing) ||
                        (newState.equals(State.RunningDirect) && runningDirectAllowed() );
                break;
            case Parsing:
                allowedTransition = 
                        newState.equals(State.Disconnected) || 
                        newState.equals(State.IdleParsed) ||
                        newState.equals(State.IdleUnparsed);
                break;
            case IdleParsed:
                allowedTransition = 
                        newState.equals(State.Disconnected) || 
                        newState.equals(State.Parsing) ||
                        newState.equals(State.IdleUnparsed) ||
                        (newState.equals(State.Running) && runningAllowed()) ||
                        (newState.equals(State.RunningDirect) && runningDirectAllowed() );
                break;
            case Running:
                allowedTransition = 
                        newState.equals(State.Disconnected) || 
                        newState.equals(State.IdleParsed) ||
                        newState.equals(State.Paused);
                break;
            case Paused:
                allowedTransition = 
                        newState.equals(State.Disconnected) || 
                        (newState.equals(State.Running) && runningAllowed()) ||
                        newState.equals(State.IdleParsed);
                break;
            case RunningDirect:
                allowedTransition = 
                        newState.equals(State.Disconnected) || 
                        newState.equals(State.IdleUnparsed);
                break;
            default:
                break;
        }

        if(allowedTransition) {
            state = newState;
            fireStateChangedEvent();
        }
        else {
//            fireStateChangeErrorMessage();
        }
        
        return allowedTransition;
    }

    private synchronized boolean runningAllowed() {
        return (!mode.equals(Mode.Direct));
    }
    private synchronized boolean runningDirectAllowed() {
        return mode.equals(Mode.Direct);
    }

    public synchronized void changeMode(Mode newMode) {
        boolean allowedTransition = false;
        
        if (newMode.equals(mode)) //check for nonsense
            return;
        
        switch(mode) {
            case Direct:
                if( !state.equals(State.RunningDirect) ) {
                    allowedTransition = true;
                }
                break;
            case Documented:
                allowedTransition = 
                        (newMode.equals(Mode.Direct) && directModeAllowed()) ||
                        newMode.equals(Mode.Testing) || 
                        newMode.equals(Mode.Monitor);
                break;
            case Testing:
                allowedTransition = 
                        (newMode.equals(Mode.Direct) && directModeAllowed()) ||
                        newMode.equals(Mode.Documented) || 
                        newMode.equals(Mode.Monitor);
                break;
            case Monitor:
                boolean hasControl = requestControl();

                if (hasControl) {
                    allowedTransition = 
                            (newMode.equals(Mode.Direct) && directModeAllowed()) ||
                            newMode.equals(Mode.Documented) || 
                            newMode.equals(Mode.Testing);
                }
                break;
            default:
                break;
        }

        if (allowedTransition) {
            mode = newMode;
        }
        fireModeChangedEvent();
    }
    public synchronized void changeRunType(RunType newRunType) {
        
        if(newRunType.equals(runType))
            return;
        
        if(state.equals(State.Running) || 
                state.equals(State.Disconnected) || 
                state.equals(State.Connecting) || 
                state.equals(State.Paused) ||
                mode.equals(Mode.Monitor) ||
                mode.equals(Mode.Direct)) {
            //change not allowed
        }
        else {
            runType = newRunType;
        }
        
        fireRunTypeChangedEvent();
    }
    
    private synchronized boolean directModeAllowed() {
        return ( state.equals(State.IdleUnparsed) );
    } 
    private synchronized boolean requestControl() {
        return true;
    }
    
    public synchronized State getState() {
        return state;
    }
    public synchronized Mode getMode() {
        return mode;
    }
    public synchronized RunType getRunType() {
        return runType;
    }

    public synchronized void connect() {
        changeState(State.Connecting);
    }
    public synchronized void finishConnecting() {
        changeState(State.IdleUnparsed);
    }
    public synchronized void disconnect() {
        changeState(State.Disconnected);
    }
    public synchronized void parse() {
        changeState(State.Parsing);
    }
    public synchronized void finishParsing(boolean success) {
        if(success) {
            changeState(State.IdleParsed);
        }
        else {
            changeState(State.IdleUnparsed);
        }
    }
    public synchronized void changeMainFile() {
        if( !changeState(State.IdleUnparsed))
            fireStateChangedEvent();    //always force update 
    }
    public synchronized void play() {
        changeState(State.Running);
    }
    public synchronized void pause() {
        changeState(State.Paused);
    }
    public synchronized void stop() {
        if(state.equals(State.Running)) {
            changeState(State.IdleParsed);
        }
        if(state.equals(State.RunningDirect)) {
            changeState(State.IdleUnparsed);
        }
        if(state.equals(State.Parsing)) {
            changeState(State.IdleUnparsed);
        }
        if(state.equals(State.Paused)) {
            changeState(State.IdleParsed);
        }
    }
    public synchronized void finishRunning() {
        if(state.equals(State.Running)) {
            changeState(State.IdleParsed);
        }
    }
    public synchronized void changeParseFile() {
        changeState(State.IdleUnparsed);
    }
    public synchronized void clearParsedData() {
        changeState(State.IdleUnparsed);
    }
    public synchronized void runDirect() {
        changeState(State.RunningDirect);
    }


}
