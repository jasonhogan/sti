/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.application.pdmonitor;

/**
 *
 * @author Susannah Dickerson
 */
public class PhotoDetector {
    public short deviceChannel = 0;
    public int rowPosition = 0;
    public int columnPosition = 0;
    public String label = "";
    public Double power = 0.0;
    public Double setPoint = 0.0;

    public PhotoDetector(){

    }

    public PhotoDetector(short _deviceChannel){
        deviceChannel = _deviceChannel;
    }

    public PhotoDetector(short _deviceChannel, int _rowPosition,
            int _columnPosition, String _label, Double _setPoint){
        deviceChannel = _deviceChannel;
        rowPosition = _rowPosition;
        columnPosition = _columnPosition;
        label = _label;
        setPoint = _setPoint;
    }

}
