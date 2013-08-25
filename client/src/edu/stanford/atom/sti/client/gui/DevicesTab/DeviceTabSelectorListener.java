/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.stanford.atom.sti.client.gui.DevicesTab;

import edu.stanford.atom.sti.client.comm.bl.device.Device;

/**
 *
 * @author Jason
 */
public interface DeviceTabSelectorListener {
    public void selectDeviceTabByDevice(Device device);
    public void toggleDeviceTab(Device device, boolean show);
}
