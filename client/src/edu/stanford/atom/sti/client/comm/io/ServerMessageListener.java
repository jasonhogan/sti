/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.comm.io;

import edu.stanford.atom.sti.corba.Client_Server.MessengerPOA;
/**
 *
 * @author Owner
 */
public abstract class ServerMessageListener extends MessengerPOA {
    public abstract void sendMessage(String message);
}

//public interface ServerMessageListener extends edu.stanford.atom.sti.client.comm.corba.Messenger {
//    public abstract void sendMessage(String message);
//}