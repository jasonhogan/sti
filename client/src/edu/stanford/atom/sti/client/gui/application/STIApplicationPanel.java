
package edu.stanford.atom.sti.client.gui.application;

import edu.stanford.atom.sti.client.gui.VisibleTabListener;
import edu.stanford.atom.sti.client.comm.bl.device.Device;
import javax.swing.JPanel;
import edu.stanford.atom.sti.corba.Types.TDataMixed;
import edu.stanford.atom.sti.corba.Types.TValMixed;
import java.lang.reflect.*;
import java.util.Vector;

public abstract class STIApplicationPanel extends JPanel implements VisibleTabListener {

    private Device device = null;

    public STIApplicationPanel() {

        try {
            Class c = Class.forName(this.getName());
            Method m[] = c.getDeclaredMethods();
            for (int i = 0; i < m.length; i++) {
                System.out.println(m[i].toString());
            }
        } catch (Throwable e) {
            System.err.println(e);
        }
    }

    public abstract void handleDeviceEvent(edu.stanford.atom.sti.client.comm.bl.device.DeviceEvent evt);

    public void setDevice(Device device) {
        this.device = device;
    }

    protected Device getDevice() {
        return device;
    }

    private TValMixed parseArg(Object arg) {
        Class c = arg.getClass();
        TValMixed value = new TValMixed();
        TValMixed[] array;

        if (c.cast(arg) instanceof String) {
            value.stringVal((String) c.cast(arg));
        } else if (c.cast(arg) instanceof Integer) {
            value.number(((Integer) c.cast(arg)).doubleValue());
        } else if (c.cast(arg) instanceof Double) {
            value.number((Double) c.cast(arg));
        } else if (c.cast(arg) instanceof Boolean) {
            value.number(((Boolean) c.cast(arg)) ? 1 : 0);
        } else if (c.cast(arg) instanceof Vector<?>) {
            Vector<?> vec = ((Vector<?>) c.cast(arg));
            array = new TValMixed[vec.size()];
            for (int i = 0; i < vec.size(); i++) {
                array[i] = parseArg(vec.get(i));
            }
            value.vector(array);
        } else if (c.isArray()) {
            int size = Array.getLength(arg);
            array = new TValMixed[size];
            for (int i = 0; i < size; i++) {
                array[i] = parseArg(((Object[]) arg)[i]);
            }
        } else {
            value.emptyValue(true);
        }

        return value;
    }

    public TDataMixed call(String function, Object... args) {
        TValMixed argsMixed = new TValMixed();
        argsMixed.vector(new TValMixed[args.length]);

        for (int i = 0; i < args.length; i++) {
            argsMixed.vector()[i] = parseArg(args[i]);
        }

        return call(function, argsMixed);
    }

    public TDataMixed call(String function, TValMixed arguments) {
        TValMixed augmentedArgs = new TValMixed();
        augmentedArgs.vector(new TValMixed[]{new TValMixed(), arguments});
        augmentedArgs.vector()[0].stringVal(function);

        return device.read(0, augmentedArgs);
    }
}
