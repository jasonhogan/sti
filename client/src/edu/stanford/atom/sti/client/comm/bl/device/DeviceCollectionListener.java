/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.comm.bl.device;

/**
 *
 * @author Jason
 */
public interface DeviceCollectionListener {
    public void addDevice(Device device);
    public void removeDevice(Device device);
    public void handleDeviceEvent(DeviceEvent evt);
    public void setDeviceManagerStatus(DeviceManager.DeviceManagerStatus status);
}
