/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.gui.checktree;

/**
 *
 * @author Heinz M. Kabutz and David Wright
 * * http://www.javaspecialists.co.za/archive/Issue145.html
 */
public enum TristateState {
  SELECTED {
    public TristateState next() {
      return INDETERMINATE;
    }
  },
  INDETERMINATE {
    public TristateState next() {
      return DESELECTED;
    }
  },
  DESELECTED {
    public TristateState next() {
      return SELECTED;
    }
  };

  public abstract TristateState next();
}
