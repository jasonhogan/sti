/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.comm.io;


import java.util.Hashtable;

/**
 *
 * @author Jason
 */
public class STIRemoteClassLoader extends ClassLoader {

    private byte[] classBytes = null;
    private String classQualifiedName = null;

    private Hashtable classes = new Hashtable();

    public STIRemoteClassLoader(String qualifiedName, byte[] bytes) {
        super( ClassLoader.getSystemClassLoader() );
        setRemoteClass(qualifiedName, bytes);
    }

    public void setRemoteClass(String qualifiedName, byte[] bytes) {
        classQualifiedName = qualifiedName;
        classBytes = bytes;
    }

    @Override
    public Class loadClass(String className) throws ClassNotFoundException {
        return (loadClass(className, true));
    }

    @Override
    public synchronized Class loadClass(String className,
            boolean resolveIt) throws ClassNotFoundException {

        Class result;

        // Check the local cache of classes
        result = (Class) classes.get(className);
        if (result != null) {
            return result;
        }

        // Check with the primordial class loader
        try {
            result = super.findSystemClass(className);
            return result;
        } catch (ClassNotFoundException e) {
        }


        if ( !className.equals(classQualifiedName) || classBytes == null) {
            throw new ClassNotFoundException();
        }

        //This className is the remote class stored in classBytes

        //parse the class file
        result = defineClass(classQualifiedName, classBytes, 0, classBytes.length);

        if (result == null) {
            throw new ClassFormatError();
        }

        if (resolveIt) {
            resolveClass(result);
        }

        classes.put(className, result);

        return result;
    }
}

