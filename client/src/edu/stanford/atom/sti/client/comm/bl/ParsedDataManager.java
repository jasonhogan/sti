/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.comm.bl;

import edu.stanford.atom.sti.client.comm.io.ParseEventListener;


public class ParsedDataManager implements ParseEventListener{

    private int numberParsedEvents = 0;
    private int numberParsedVariables = 0;

    public void handleEvent(edu.stanford.atom.sti.corba.Pusher.TParseEvent event) {
        numberParsedEvents = event.numberEvents;
        numberParsedVariables = event.numberVariables;
    }


}
