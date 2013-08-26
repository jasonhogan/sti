/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.stanford.atom.sti.client.comm.bl;

import java.text.ParseException;

/**
 *
 * @author Jason
 */
public class STINumberFormat extends javax.swing.text.NumberFormatter {

    @Override
    public Object stringToValue(String text) throws ParseException {
        String scrubbedNumber = text.replaceAll("ns|us|ms|s|\\|", "");
        return super.stringToValue(scrubbedNumber);
    }

    @Override
    public String valueToString(Object value) throws ParseException {
        String commaFormated = super.valueToString(value);
        if (commaFormated.contains(".")) {
            String[] split = commaFormated.split("\\.");
            commaFormated = split[0];    //"rounds" to nearest ns
        }
        String unitFormated = "";
        // temp.replaceAll(",", ":");
        String[] dividedByUnits = commaFormated.split(",");
        int lastUnit = dividedByUnits.length - 1;
        if (dividedByUnits.length >= 1) {
            unitFormated = dividedByUnits[lastUnit - 0] + "ns" + unitFormated;
        }
        if (dividedByUnits.length >= 2) {
            unitFormated = dividedByUnits[lastUnit - 1] + "us|" + unitFormated;
        }
        if (dividedByUnits.length >= 3) {
            unitFormated = dividedByUnits[lastUnit - 2] + "ms|" + unitFormated;
        }
        if (dividedByUnits.length >= 4) {
            unitFormated = dividedByUnits[lastUnit - 3] + "s|" + unitFormated;
        }
        if (dividedByUnits.length >= 5) {
            for (int j = 4; j < dividedByUnits.length; j++) {
                unitFormated = dividedByUnits[lastUnit - j] + "," + unitFormated;
            }
        }
        return unitFormated;
    }
}
