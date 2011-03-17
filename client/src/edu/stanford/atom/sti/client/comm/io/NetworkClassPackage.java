/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.comm.io;
import edu.stanford.atom.sti.corba.Types.TNetworkFile;
/**
 *
 * @author Jason
 */
public class NetworkClassPackage {
    
    TNetworkFile networkFile = null;
    JarByteClassLoader jarClassLoader = null;
    TNetworkFileReader fileReader = null;
    
    public NetworkClassPackage(TNetworkFile file) {
        networkFile = file;
        fileReader = new TNetworkFileReader(networkFile);

        if (fileReader != null && fileReader.read()) {
            jarClassLoader = new JarByteClassLoader(fileReader.getFileData());
            jarClassLoader.loadLocalClasses();
        }
    }

    public JarByteClassLoader getJarClassLoader() {
        return jarClassLoader;
    }

    public java.util.Vector<Class> getAvailableSubClasses(Class superClass) {
        return jarClassLoader.getAvailableSubClasses(superClass);
    }

    public boolean hasClass(String className) {
        return jarClassLoader.hasClass(className);
    }
}
