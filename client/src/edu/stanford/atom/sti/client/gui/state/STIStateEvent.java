/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.gui.state;

import java.util.EventObject;


/**
 *
 * @author Owner
 */
public class STIStateEvent extends EventObject {

    STIStateMachine.State state_;
    
    public STIStateEvent(Object source, STIStateMachine.State state) {
        super(source);
        state_ = state;
    }
    
    public STIStateMachine.State state() {
        return state_;
    }
}
