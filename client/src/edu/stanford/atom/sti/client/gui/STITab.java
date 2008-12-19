/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.gui;

/**
 *
 * @author Owner
 */
public interface STITab {
    
    public enum STIStatus {Ready, ParseError};
    public STIStatus status = STIStatus.Ready;
    
    public static boolean newParsedDataReady = false;
    
    public void parseFile();
//    void play();
//    void pause();
//    void stop();
}
