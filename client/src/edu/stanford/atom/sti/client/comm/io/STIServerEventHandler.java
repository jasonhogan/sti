/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.comm.io;
import edu.stanford.atom.sti.corba.Pusher.ServerEventHandlerPOA;
import java.util.Vector;
import java.util.Hashtable;
import java.util.Enumeration;

import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.locks.ReentrantLock;
import java.util.concurrent.locks.Condition;


/**
 *
 * @author Jason
 */

public class STIServerEventHandler extends ServerEventHandlerPOA {

    private Hashtable<Class, Vector<? extends ServerEventListener> > eventListenerClassTable = new Hashtable<Class, Vector<? extends ServerEventListener>>();

    private Vector<PingEventListener>       pingListeners = new Vector<PingEventListener>();
    private Vector<StatusEventListener>     statusListeners  = new Vector<StatusEventListener>();
    private Vector<MessageEventListener>    messageListeners = new Vector<MessageEventListener>();
    private Vector<ParseEventListener>      parseListeners  = new Vector<ParseEventListener>();
    private Vector<FileEventListener>       fileListeners = new Vector<FileEventListener>();
    private Vector<ControllerEventListener> controllerListeners = new Vector<ControllerEventListener>();
    private Vector<DeviceRefreshEventListener>     deviceListeners = new Vector<DeviceRefreshEventListener>();
    private Vector<DeviceDataEventListener> deviceDataListeners = new Vector<DeviceDataEventListener>();


    private final Object messageLock = new Object();
    private LinkedBlockingQueue<edu.stanford.atom.sti.corba.Pusher.TMessageEvent> messageQueue = new LinkedBlockingQueue<edu.stanford.atom.sti.corba.Pusher.TMessageEvent>();
    private ReentrantLock messageReLock = new ReentrantLock();
    private Condition messageCondition = messageReLock.newCondition();

    public STIServerEventHandler() {
        eventListenerClassTable.put(PingEventListener.class, pingListeners);
        eventListenerClassTable.put(StatusEventListener.class, statusListeners);
        eventListenerClassTable.put(MessageEventListener.class, messageListeners);
        eventListenerClassTable.put(ParseEventListener.class, parseListeners);
        eventListenerClassTable.put(FileEventListener.class, fileListeners);
        eventListenerClassTable.put(ControllerEventListener.class, controllerListeners);
        eventListenerClassTable.put(DeviceRefreshEventListener.class, deviceListeners);
        eventListenerClassTable.put(DeviceDataEventListener.class, deviceDataListeners);

        pushMessageEventThread.start();
    }

    public void pushPingEvent (edu.stanford.atom.sti.corba.Pusher.TPingEvent event) {       
        // Run event pusher loop in a separate thread in case any handlers generate
        // server events of their own.
        final edu.stanford.atom.sti.corba.Pusher.TPingEvent evt = event;
        Thread pushEventThread = new Thread(new Runnable() {

            public void run() {

                for (PingEventListener listener : pingListeners) {
                    try {
                        listener.handleEvent(evt);
                    } catch (Exception e) {
                        removeEventListener(listener, pingListeners);
                    }
                }
            }
        });
        pushEventThread.start();

    }
    public void pushStatusEvent (edu.stanford.atom.sti.corba.Pusher.TStatusEvent event) {
        // Run event pusher loop in a separate thread in case any handlers generate
        // server events of their own.
        final edu.stanford.atom.sti.corba.Pusher.TStatusEvent evt = event;
        Thread pushEventThread = new Thread(new Runnable() {

            public void run() {

                for (StatusEventListener listener : statusListeners) {
                    try {
                        listener.handleEvent(evt);
                    } catch (Exception e) {
                        removeEventListener(listener, statusListeners);
                    }
                }
            }
        });
        pushEventThread.start();
  }
    public synchronized void pushMessageEvent(edu.stanford.atom.sti.corba.Pusher.TMessageEvent event) {
        // Run event pusher loop in a separate thread in case any handlers generate
        // server events of their own.
        final edu.stanford.atom.sti.corba.Pusher.TMessageEvent evt = event;

//        for (MessageEventListener listener : messageListeners) {
//                    try {
//                        listener.handleEvent(evt);
//                    } catch (Exception e) {
//                        removeEventListener(listener, messageListeners);
//                    }
//                }

        // Makes sure messages are printing in the order they are received.
        messageReLock.lock();
        try {
            messageQueue.put(event);
            messageCondition.signalAll();
        } catch (InterruptedException ex) {
        } finally {
            messageReLock.unlock();
        }

    }
        //also good to use a new thread so that the client returns quickly
    Thread pushMessageEventThread = new Thread(new Runnable() {

        public void run() {
            while (true) {

                //if queue is empty, sleep until needed
                messageReLock.lock();
                try {
                    if(messageQueue.size() == 0)
                        messageCondition.await();
//                        messageCondition.awaitNanos(1000000000);    //1 second timeout
                } catch (InterruptedException ex) {
                }
                finally {
                    messageReLock.unlock();
                }

                //send events in queue
                while (messageQueue.size() > 0) {
                    for (MessageEventListener listener : messageListeners) {
                        try {
                            listener.handleEvent(messageQueue.take());
                        } catch (InterruptedException ex) {
                        } catch (Exception e) {
                            removeEventListener(listener, messageListeners);
                        }
                    }
                }
            }


            //lock makes sure that messages are handled in the same order as sent
//                synchronized (messageLock) {
//
//                    for (MessageEventListener listener : messageListeners) {
//                        try {
//                            listener.handleEvent(evt);
//                        } catch (Exception e) {
//                            removeEventListener(listener, messageListeners);
//                        }
//                    }
//
//                }
        }
    });
   //     pushEventThread.start();

    public void pushParseEvent(edu.stanford.atom.sti.corba.Pusher.TParseEvent event) {
        // Run event pusher loop in a separate thread in case any handlers generate
        // server events of their own.
        final edu.stanford.atom.sti.corba.Pusher.TParseEvent evt = event;
        Thread pushEventThread = new Thread(new Runnable() {

            public void run() {

                for (ParseEventListener listener : parseListeners) {
                    try {
                        listener.handleEvent(evt);
                    } catch (Exception e) {
                        removeEventListener(listener, parseListeners);
                    }
                }
            }
        });
        pushEventThread.start();
    }
    public void pushFileEvent(edu.stanford.atom.sti.corba.Pusher.TFileEvent event) {
        // Run event pusher loop in a separate thread in case any handlers generate
        // server events of their own.
        final edu.stanford.atom.sti.corba.Pusher.TFileEvent evt = event;
        Thread pushEventThread = new Thread(new Runnable() {

            public void run() {

                for (FileEventListener listener : fileListeners) {
                    try {
                        listener.handleEvent(evt);
                    } catch (Exception e) {
                        removeEventListener(listener, fileListeners);
                    }
                }
            }
        });
        pushEventThread.start();
    }

    public void pushControllerEvent (edu.stanford.atom.sti.corba.Pusher.TControllerEvent event) {
        // Run event pusher loop in a separate thread in case any handlers generate
        // server events of their own.
        final edu.stanford.atom.sti.corba.Pusher.TControllerEvent evt = event;
        Thread pushEventThread = new Thread(new Runnable() {
            public void run() {

                for (ControllerEventListener listener : controllerListeners) {
            try {
                listener.handleEvent(evt);
            } catch (Exception e) {
                removeEventListener(listener, controllerListeners);
            }
        }
            }
        });
        pushEventThread.start();
    }
    public void pushDeviceRefreshEvent (edu.stanford.atom.sti.corba.Pusher.TDeviceRefreshEvent event) {
        // Run event pusher loop in a separate thread in case any handlers generate
        // server events of their own.
        final edu.stanford.atom.sti.corba.Pusher.TDeviceRefreshEvent evt = event;
        Thread pushEventThread = new Thread(new Runnable() {
            public void run() {

                for (DeviceRefreshEventListener listener : deviceListeners) {
                    try {
                        listener.handleEvent(evt);
                    } catch (Exception e) {
                        removeEventListener(listener, deviceListeners);
                    }
                }

            }
        });
        pushEventThread.start();
    }
    public void pushDeviceDataEvent (edu.stanford.atom.sti.corba.Pusher.TDeviceDataEvent event) {
        // Run event pusher loop in a separate thread in case any handlers generate
        // server events of their own.
        final edu.stanford.atom.sti.corba.Pusher.TDeviceDataEvent evt = event;
        Thread pushEventThread = new Thread(new Runnable() {
            public void run() {

                for (DeviceDataEventListener listener : deviceDataListeners) {
            try {
                listener.handleEvent(evt);
            } catch (Exception e) {
                removeEventListener(listener, deviceDataListeners);
            }
        }

            }
        });
        pushEventThread.start();
    }

    @SuppressWarnings("unchecked")
    public synchronized <T extends ServerEventListener> void addEventListener(T listener) {
        Class[] interfaces = listener.getClass().getInterfaces();

        Class currentClass = null;
        Enumeration<Class> listenerClasses = null;

        for(int i = 0; i < interfaces.length; i++) {
            listenerClasses = eventListenerClassTable.keys();

            while(listenerClasses.hasMoreElements()) {
                currentClass = listenerClasses.nextElement();

                if(interfaces[i].equals(currentClass)) {
                    addEventListener(listener,
                            (Vector<ServerEventListener>) eventListenerClassTable.get(currentClass));
                }
            }
        }

    }
    @SuppressWarnings("unchecked")
    public synchronized <T extends ServerEventListener> void removeEventListener(T listener) {
        Class[] interfaces = listener.getClass().getInterfaces();

        Class currentClass = null;
        Enumeration<Class> listenerClasses = null;

        for(int i = 0; i < interfaces.length; i++) {
            listenerClasses = eventListenerClassTable.keys();

            while(listenerClasses.hasMoreElements()) {
                currentClass = listenerClasses.nextElement();

                if(interfaces[i].equals(currentClass)) {
                    removeEventListener(listener, 
                            (Vector<ServerEventListener>) eventListenerClassTable.get(currentClass));
                }
            }
        }

    }

    private synchronized <T extends ServerEventListener> void addEventListener(T listener, Vector<T> listeners) {
        if( !listeners.contains(listener) ) {
            listeners.add(listener);
        }
    }
    private synchronized <T extends ServerEventListener> void removeEventListener(T listener, Vector<T> listeners) {
        listeners.remove(listener);
    }

//    private synchronized <T> void fireEvent(T event, Vector<? extends ServerEventListener<T>> listeners) {
//        for (ServerEventListener<T> listener : listeners) {
//            try {
//                listener.handleEvent(event);
//            } catch (Exception e) {
//                removeEventListener(listener, (Vector<ServerEventListener<T>>) listeners);
//            }
//        }
//    }

}