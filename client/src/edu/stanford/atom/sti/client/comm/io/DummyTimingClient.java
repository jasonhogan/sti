/*
 * Minimalistic CORBA client.  Resolves a ModeHandler object from the 
 * naming service.  
 * <strikeout>Currently the server IP and port are hard coded (jhogan2:2809)!</strikeout>
 * Currently the port is hard-coded to the default omniORB NameServer port 2809
 * and the host is also hard-coded; these are in two static String attributes at
 * the beginning of the class. Changing them there affects it everywhere necessary.
 */

package edu.stanford.atom.sti.client.comm.io;

import edu.stanford.atom.sti.client.comm.corba.*;
import org.omg.CosNaming.*;
import org.omg.CosNaming.NamingContextPackage.*;
import org.omg.CORBA.*;

/**
 *
 * @author Jason Hogan, Jonathan Harvey
 */
public class DummyTimingClient {

    /**
     * @param args the command line arguments
     */
    
    static String hostAddr = "171.64.58.36";
    static String corbaServerPort = "2809";
    
    public static void main(String[] args) {

        try {
            String[] extendedArgs = {"-ORBInitialPort", corbaServerPort, "-ORBInitialHost", hostAddr};
            // create and initialize the ORB
            ORB orb = ORB.init(extendedArgs, null);
            getFromParser(orb);
            getFromModeHandler(orb);
        } catch (Exception e) {
            System.out.println("ERROR : " + e);
            e.printStackTrace(System.out);
        }
    }

    /**
     * getFromParser()
     * 
     * A proof-of-concept method that gets information from the specified Parser.Object
     * and spits it out to the system output.
     * -- Jonathan Harvey (harv_at_stanford_dot_edu)
     * 
     * @param orb   The orb to which this will attach
     */
    static void getFromParser(ORB orb) {
        org.omg.CORBA.Object parserObj = orb.string_to_object(
                    "corbaname::" + hostAddr + ":" + corbaServerPort + 
                    "#Timing_Server.Context/Parser.Object");
        
        Parser cParse = ParserHelper.narrow(parserObj);

        System.out.println("Parsed File: " + cParse.mainFile().toString());
        java.lang.String[] newArray = cParse.files();

        if (newArray.length > 0) {
            for(int i=0; i < newArray.length; i++){
                System.out.println("File #" + i + ": " + newArray[i]);
            }
        }
        
        org.omg.CORBA.Object controlObj = orb.string_to_object(
                "corbaname::" + hostAddr + ":" + corbaServerPort + 
                "#Timing_Server.Context/Control.Object");
        Control control = ControlHelper.narrow(controlObj);
            
        System.out.println("Testing Control Error Message: " + control.errMsg());
        
        System.out.println("\nChannels: ");

        TChannel[] structSeq = cParse.channels();
        for(int i=0; i<structSeq.length; i++)
        {
            System.out.println("Channel " + structSeq[i].channel);
            System.out.println("Board Type: " + structSeq[i].board.boardType);
            System.out.println("Board Address: " + structSeq[i].board.address);
            System.out.println("Module Type: " + structSeq[i].board.moduleType);
        }

        System.out.println("\nEvents: ");
        TEvent[] evtSeq = cParse.events();
        for(int i=0; i<evtSeq.length; i++)
        {
            System.out.println("Channel: " + evtSeq[i].channel);
            System.out.println("Time: " + evtSeq[i].time);
            System.out.println("File #: " + evtSeq[i].pos.file);
            System.out.println("Line #: " + evtSeq[i].pos.line);
            /*System.out.println("Value String: " + evtSeq[i].value.stringVal());
            System.out.println("Value Number: " + evtSeq[i].value.number());
            System.out.println("Amplitude: " + evtSeq[i].value.triplet().ampl);
            System.out.println("Frequency: " + evtSeq[i].value.triplet().freq);
            System.out.println("Phase: " + evtSeq[i].value.triplet().phase);*/
        }    
    }
 
    /**
     * getFromModeHandler()
     * 
     * A proof-of-concept method that gets information from the specified ModeHandler.Object
     * and spits it out to the system output.
     * -- Jason Hogan (hogan/at/stanford.edu), Jonathan Harvey (harv/at/stanford.edu)
     * 
     * @param orb   The orb to which this will attach
     */
    static void getFromModeHandler(ORB orb) {
    org.omg.CORBA.Object modeHandlerObj = orb.string_to_object(
                "corbaname::" + hostAddr + ":" + corbaServerPort + 
                "#Timing_Server.Context/ModeHandler.Object");
    ModeHandler modeHandle = ModeHandlerHelper.narrow(modeHandlerObj);
    if (modeHandle.requestPending()) {
        System.out.println(modeHandle.requesterName().toString());
    }
    }
}
