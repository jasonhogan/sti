/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.comm.bl.device;

public class GenericDeviceCollection extends DeviceCollection {

    public GenericDeviceCollection() {
    }

    public boolean isAllowedMember(Device device) {
        return true;
    }
}
