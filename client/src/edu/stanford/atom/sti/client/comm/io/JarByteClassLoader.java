/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.comm.io;

import java.util.Hashtable;

import java.io.ByteArrayInputStream;
import java.util.jar.JarInputStream;
import java.util.jar.JarEntry;
import java.io.IOException;


public class JarByteClassLoader extends ClassLoader {

    private ByteArrayInputStream jarByteStream;
    private JarInputStream jarStream;

    private Hashtable<String, byte[]> byteResourceTable = new Hashtable<String, byte[]>();
    private Hashtable<String, Class> classTable = new Hashtable<String, Class>();

    public JarByteClassLoader(byte[] jarBytes) {
        super( ClassLoader.getSystemClassLoader() );

        jarByteStream = new ByteArrayInputStream(jarBytes);

        try {
            jarStream = new JarInputStream(jarByteStream);
            parseJar();
        } catch(IOException e) {
            e.printStackTrace();
        }
    }

    private void parseJar() throws IOException {
        //populates byteResourceTable by walking though the Jar stream and looking for named resources

        JarEntry entry = null;
        byte[] resource = null;
        String name;

        while((entry = jarStream.getNextJarEntry()) != null) {
            if (!entry.isDirectory() && entry.getName().endsWith(".class")) {
                name = entry.getName();
                //name.replaceAll("\\/", ".");

                resource = extractResource(entry);

                byteResourceTable.put(stripClass(name.replaceAll("\\/", ".")), resource);
            }
        }
//        byteResourceTable.keys();
    }

    private byte[] extractResource(JarEntry entry) throws IOException {
        int size = (int) entry.getSize();
        byte[] resource = new byte[size];

        //see http://www.codase.com/search/call?owner=java.util.jar.JarInputStream

        int count = 0;
        int current = 0;
        while (((current = jarStream.read(resource, count, size - count)) != -1)
                && (count < size)) {
            count += current;
        }

        return resource;
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
        result = classTable.get(className);
        if (result != null) {
            return result;
        }

        // Check with the primordial class loader
        try {
            result = super.findSystemClass(className);
            return result;
        } catch (ClassNotFoundException e) {
        }

        // Check if the class is in the resource table:
        byte[] classBytes = byteResourceTable.get(className);

        if ( classBytes == null) {
            throw new ClassNotFoundException();
        }



        //parse the class file
        result = defineClass(stripClass(className), classBytes, 0, classBytes.length);

        if (result == null) {
            throw new ClassFormatError();
        }

        if (resolveIt) {
            resolveClass(result);
        }

        classTable.put(className, result);

        return result;
    }

    public synchronized void loadLocalClasses() {
        java.util.Enumeration<String> keys = byteResourceTable.keys();
        while(keys.hasMoreElements()) {
            try{
                loadClass(keys.nextElement());
            } catch(ClassNotFoundException e) {
                e.printStackTrace();
            }
        }
    }

    public java.util.Vector<Class> getAvailableSubClasses(Class superClass) {

        Class nextClass;

        java.util.Vector<Class> classes = new java.util.Vector<Class>();
        java.util.Enumeration<String> keys = classTable.keys();

        while(keys.hasMoreElements()) {
            nextClass = classTable.get(keys.nextElement());
            if(superClass.isAssignableFrom(nextClass)) {
                classes.add(nextClass);
            }
        }

        return classes;
    }

    public boolean hasClass(String className) {
        String name;
        if(className.endsWith(".class")) {
            name = className.substring(0, className.lastIndexOf("."));
        } else {
            name = className;
        }
        return classTable.containsKey(name);
    }

    private String stripClass(String name) {
        if(name.endsWith(".class")) {
            return name.substring(0, name.lastIndexOf("."));
        }
        return name;
    }
}
