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

public class STIStateMachine {

    public static enum State { Disconnected, Connecting, IdleUnparsed, Parsing, IdleParsed, Running, Paused };
    public static enum Mode { Direct, Documented, Testing, Monitor };

    private State state = State.Disconnected;
    private Mode mode = Mode.Monitor;
    
    private Vector<STIStateListener> listeners = new Vector<STIStateListener>();


    public STIStateMachine() {
        listeners.clear();
    }
    
    public synchronized void addStateListener(STIStateListener listener) {
        listeners.add(listener);
        listener.updateState( new STIStateEvent(this, state) );
    }
    public synchronized void removeStateListener(STIStateListener listener) {
        listeners.remove(listener);
    }
    
    private synchronized void fireStateChangedEvent() {
        STIStateEvent event = new STIStateEvent(this, state);
        
        for(int i = 0; i < listeners.size(); i++) {
            listeners.elementAt(i).updateState( event );
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
                        newState.equals(State.Parsing);
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
                        newState.equals(State.Running);
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
                        newState.equals(State.Running);
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

    public void changeMode(Mode newMode) {
        
    }
    public State getState() {
        return state;
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
    public void changeMainFile() {
        if( !changeState(State.IdleUnparsed))
            fireStateChangedEvent();    //always force update 
    }
    public synchronized void play() {
        changeState(State.Running);
    }
    public synchronized void pause() {
        changeState(State.Paused);
    }
    public synchronized void finishRunning() {
        changeState(State.IdleParsed);
    }
    public synchronized void changeParseFile() {
        changeState(State.IdleUnparsed);
    }


}
