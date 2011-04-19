/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.gui.CellEditTable;

/**
 *
 * @author Jim Yingst at http://www.coderanch.com/t/372313/java/java/Dimensional-Hashtable
 */
public class CompositeKey {
     private final Object key1;
     private final Object key2;
     public CompositeKey(Object key1, Object key2) {
         this.key1 = key1;
         this.key2 = key2;
     }
    @Override
     public boolean equals(Object obj) {
        if (!(obj instanceof CompositeKey))
            return false;
        CompositeKey other = (CompositeKey) obj;
        return this.key1.equals(other.key1)
            && this.key2.equals(other.key2);
     }
    @Override
     public int hashCode() {
        int result = 17;
        result = 37 * result + key1.hashCode();
        result = 37 * result + key2.hashCode();
        return result;
     }
}
