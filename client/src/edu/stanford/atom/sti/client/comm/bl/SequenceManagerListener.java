/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.comm.bl;

/**
 *
 * @author EP
 */
public interface SequenceManagerListener {
    public void updateData(SequenceManagerEvent event);
    public void displayParsingError(SequenceManagerEvent event);
    public void updateDoneStatus(int experimentNumber, boolean done);
}
