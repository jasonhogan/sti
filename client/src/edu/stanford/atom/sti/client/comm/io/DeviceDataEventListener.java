/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.comm.io;

/**
 *
 * @author Jason
 */
public interface DeviceDataEventListener extends ServerEventListener {
    public void handleEvent(edu.stanford.atom.sti.corba.Pusher.TDeviceDataEvent event);
}