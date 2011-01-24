/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.comm.io;

import edu.stanford.atom.sti.corba.Types.TNetworkFile;
import edu.stanford.atom.sti.corba.Types.TOctetSeqHolder;
/**
 *
 * @author Jason
 */
public class TNetworkFileReader {

    private TNetworkFile networkFile = null;

    private int length = 0;
    private boolean accessible = false;
    private byte[] data = null;
    private String filename = null;

    public TNetworkFileReader(TNetworkFile file) {
        networkFile = file;

        try {
            filename = networkFile.fileName();
            accessible = true;
        } catch (Exception e) {
            e.printStackTrace(System.out);
            accessible = false;
        }
    }

    public boolean read() {

        try {
            length = networkFile.length();
            filename = networkFile.fileName();
            accessible = true;
        } catch (Exception e) {
            e.printStackTrace(System.out);
            accessible = false;
        }

        if(!accessible || length <= 0)
            return false;

	data = new byte[length];

        boolean success = true;
        int i = 0;
        int packetSize = 1000;
        int timeout = 3;

        TOctetSeqHolder tOctet = new TOctetSeqHolder();

        while (i < length && timeout > 0) {
            try {
                networkFile.getBytes(i, packetSize, tOctet);
                success = true;
                accessible = true;
            } catch (Exception e) {
                e.printStackTrace(System.out);
                success = false;
                accessible = false;
                timeout--;
            }

            if (success && tOctet != null) {

                for (int j = 0; j < tOctet.value.length; j++) {
                    data[i + j] = tOctet.value[j];
                }
                i += packetSize;
            }
        }

        return success;
    }

    public byte[] getFileData() {
        return data;
    }

    public boolean isAccessible() {
        return accessible;
    }

    public int fileLength() {
        return length;
    }

    public String fileName() {
        return filename;
    }

}
