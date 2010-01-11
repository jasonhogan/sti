/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.comm.bl;

import java.util.Hashtable;

public class RemoteClassLoader extends ClassLoader {

    private Hashtable loadedRemoteClasses = new Hashtable();
    private Hashtable remoteClasses = new Hashtable();
    
    public RemoteClassLoader() {
    }

    public RemoteClassLoader(ClassLoader parent) {
        super(parent);
    }

    /**
     * 
     * @param fullClassName The fully qualified binary name of the class, including 
     * the package information.  Do not include the '.class' extension.
     * Package syntax must use '.' instead of '/' to separate subpackages.
     * @param ClassBytes The binary data tha represents this class.
     */
    public void addRemoteClass(String fullClassName, byte[] ClassBytes) {
  //      remoteClasses.add( new RemoteClassBytes(fullClassName, ClassBytes) );
        remoteClasses.put(fullClassName, ClassBytes);
    }
    
    @Override
    public Class loadClass(String className) throws ClassNotFoundException {
        return (loadClass(className, true));
    }

    @Override
    public synchronized Class loadClass(String className,
            boolean resolveIt) throws ClassNotFoundException {

        byte[] classBytes = null;
        Class result;

        //----- Check our local cache of classes
        result = (Class) loadedRemoteClasses.get(className);
        if (result != null) {
            if (resolveIt) {
                resolveClass(result);
            }
            return result;
        }

        //----- Check with the primordial class loader
        try {
            result = super.findSystemClass(className);
            if (resolveIt) {
                resolveClass(result);
            }
            return result;
        } catch (ClassNotFoundException e) {

        }

        //----- Check the cache of remote classes
        
        classBytes = (byte[]) remoteClasses.get(className);
        
        if (classBytes == null) {
            throw new ClassNotFoundException();
        }

        //----- Define it (parse the class file)

        result = defineClass(className, classBytes, 0, classBytes.length);
        if (result == null) {
            throw new ClassFormatError();
        }

        //----- Resolve if necessary
        if (resolveIt) {
            resolveClass(result);
        }

        loadedRemoteClasses.put(className, result);

        return result;
    }
}

        