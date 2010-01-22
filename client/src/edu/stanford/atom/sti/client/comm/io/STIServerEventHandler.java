/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.comm.io;
import edu.stanford.atom.sti.corba.Pusher.ServerEventHandlerPOA;
import java.util.Vector;
import java.util.Hashtable;
import java.util.Enumeration;
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

    public STIServerEventHandler() {
        eventListenerClassTable.put(PingEventListener.class, pingListeners);
        eventListenerClassTable.put(StatusEventListener.class, statusListeners);
        eventListenerClassTable.put(MessageEventListener.class, messageListeners);
        eventListenerClassTable.put(ParseEventListener.class, parseListeners);
        eventListenerClassTable.put(FileEventListener.class, fileListeners);
        eventListenerClassTable.put(ControllerEventListener.class, controllerListeners);
        eventListenerClassTable.put(DeviceRefreshEventListener.class, deviceListeners);
        eventListenerClassTable.put(DeviceDataEventListener.class, deviceDataListeners);
    }

    public void pushPingEvent (edu.stanford.atom.sti.corba.Pusher.TPingEvent event) {       
        for (PingEventListener listener : pingListeners) {
            try {
                listener.handleEvent(event);
            } catch (Exception e) {
                removeEventListener(listener, pingListeners);
            }
        }
    }
    public void pushStatusEvent (edu.stanford.atom.sti.corba.Pusher.TStatusEvent event) {
        for (StatusEventListener listener : statusListeners) {
            try {
                listener.handleEvent(event);
            } catch (Exception e) {
                removeEventListener(listener, statusListeners);
            }
        }
  }
    public void pushMessageEvent (edu.stanford.atom.sti.corba.Pusher.TMessageEvent event) {
        for (MessageEventListener listener : messageListeners) {
            try {
                listener.handleEvent(event);
            } catch (Exception e) {
                removeEventListener(listener, messageListeners);
            }
        }
  }
    public void pushParseEvent (edu.stanford.atom.sti.corba.Pusher.TParseEvent event) {
        for (ParseEventListener listener : parseListeners) {
            try {
                listener.handleEvent(event);
            } catch (Exception e) {
                removeEventListener(listener, parseListeners);
            }
        }
  }
    public void pushFileEvent (edu.stanford.atom.sti.corba.Pusher.TFileEvent event) {
        for (FileEventListener listener : fileListeners) {
            try {
                listener.handleEvent(event);
            } catch (Exception e) {
                removeEventListener(listener, fileListeners);
            }
        }
  }
    public void pushControllerEvent (edu.stanford.atom.sti.corba.Pusher.TControllerEvent event) {
        for (ControllerEventListener listener : controllerListeners) {
            try {
                listener.handleEvent(event);
            } catch (Exception e) {
                removeEventListener(listener, controllerListeners);
            }
        }
  }
    public void pushDeviceRefreshEvent (edu.stanford.atom.sti.corba.Pusher.TDeviceRefreshEvent event) {
        for (DeviceRefreshEventListener listener : deviceListeners) {
            try {
                listener.handleEvent(event);
            } catch (Exception e) {
                removeEventListener(listener, deviceListeners);
            }
        }
    }
    public void pushDeviceDataEvent (edu.stanford.atom.sti.corba.Pusher.TDeviceDataEvent event) {
        for (DeviceDataEventListener listener : deviceDataListeners) {
            try {
                listener.handleEvent(event);
            } catch (Exception e) {
                removeEventListener(listener, deviceDataListeners);
            }
        }
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