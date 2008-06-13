/*
 * A test Client that gets Device information.
 */

package nbproject2;

import org.omg.CosNaming.*;
import org.omg.CosNaming.NamingContextPackage.*;
import org.omg.CORBA.*;

import edu.stanford.atom.sti.device.*;
import edu.stanford.atom.sti.client.*;

/**
 *
 * @author Owner
 */
public class Main {

    /**
     * @param args the command line arguments
     */
   public static void main(String[] args) {

        try {
            String[] extendedArgs = {"-ORBInitialPort", "2809", "-ORBInitialHost", "jhogan"};
            ORB orb = ORB.init(extendedArgs, null);
            
            //aquire servants
            org.omg.CORBA.Object obj = orb.string_to_object(
                    "corbaname::jhogan:2809#STI/Client/ModeHandler.Object");
            ModeHandler modeHandle = ModeHandlerHelper.narrow(obj);

            org.omg.CORBA.Object obj2 = orb.string_to_object(
                    "corbaname::jhogan:2809#STI/Client/Parser.Object");
            Parser parser = ParserHelper.narrow(obj2);
            
            org.omg.CORBA.Object obj3 = orb.string_to_object(
                    "corbaname::jhogan:2809#STI/Client/DeviceConfigure.Object");
            DeviceConfigure config = DeviceConfigureHelper.narrow(obj3);  
           

            //examples
            System.out.println(modeHandle.requesterName().toString());
            System.out.println(parser.files()[1]);                         
            System.out.println(modeHandle.requesterName().toString());

            //getting device information
            edu.stanford.atom.sti.device.TDevice[] reg_devs = config.devices();
            String deviceID1 = reg_devs[0].deviceID;
            System.out.println(config.getDeviceAttributes(deviceID1)[0].key 
                    + " = " + config.getDeviceAttributes(deviceID1)[0].value);
            System.out.println(config.getDeviceChannels(deviceID1)[0].channel);
            
        } catch (Exception e) {
            System.out.println("ERROR : " + e);
            e.printStackTrace(System.out);
        }
    }
}
