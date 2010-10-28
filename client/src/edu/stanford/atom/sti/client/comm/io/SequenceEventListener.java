/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.comm.io;

/**
 *
 * @author Jason
 */
public interface SequenceEventListener extends ServerEventListener {
    public void handleEvent(edu.stanford.atom.sti.corba.Pusher.TSequenceEvent event);
}