/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.stanford.atom.sti.client.comm.bl;

import java.util.Vector;
import java.util.Arrays;
import edu.stanford.atom.sti.client.comm.io.ServerConnectionListener;
import edu.stanford.atom.sti.client.comm.io.ServerConnectionEvent;
import edu.stanford.atom.sti.client.gui.state.*;

import edu.stanford.atom.sti.corba.Client_Server.*;
import edu.stanford.atom.sti.corba.Types.TRow;
import javax.swing.table.*;
import edu.stanford.atom.sti.client.comm.io.ServerConnectionListener;
import edu.stanford.atom.sti.client.comm.io.ServerConnectionEvent;
import java.lang.Thread;
import edu.stanford.atom.sti.client.gui.table.*;
import edu.stanford.atom.sti.corba.Types.TExpSequenceInfo;
/**
 *
 * @author EP
 */
public class SequenceManager implements ServerConnectionListener, STIStateListener {
    
//    private STITableModel sequenceTableModel = new STITableModel();
    private ExpSequence expSequenceRef = null;
    private Parser parserRef = null;
    private Control controlRef = null;
    private STIStateMachine.State state = STIStateMachine.State.Disconnected;
    private Vector<String> variables = new Vector<String>();
    private Vector< Vector<Object> > sequenceData = new Vector< Vector<Object> >();

    private String corbaErrorText = "Lost connection to server.";

    private Vector<SequenceManagerListener> listeners = new Vector<SequenceManagerListener>();

    public SequenceManager() {
 //       this.sequenceTableModel = sequenceTableModel;
    }

    public synchronized void addSequenceListener(SequenceManagerListener listener) {
        listeners.add(listener);
    }
    public synchronized void removeSequenceListener(SequenceManagerListener listener) {
        listeners.remove(listener);
    }
    private synchronized void fireNewUpdateDataEvent() {
        SequenceManagerEvent event = new SequenceManagerEvent(this);

        for(int i = 0; i < listeners.size(); i++) {
            listeners.elementAt(i).updateData( event );
        }
    }
    private synchronized void fireNewDisplayParsingErrorEvent(String errorText) {
        SequenceManagerEvent event = new SequenceManagerEvent(this, errorText);

        for(int i = 0; i < listeners.size(); i++) {
            listeners.elementAt(i).displayParsingError( event );
        }
    }
    public void installServants(ServerConnectionEvent event) {
        setExpSequence(event.getServerConnection().getExpSequence());
        setParser(event.getServerConnection().getParser());
        setControl(event.getServerConnection().getControl());
    }
    public void uninstallServants(ServerConnectionEvent event) {
        setExpSequence(null);
        setParser(null);
    }

    public void runSequence(boolean documented, TExpSequenceInfo info) {
        //expSequenceRef.variables( (String[])variables.toArray() );

        //setup ExpSequence here to make sure it's current
info.filename = "";
info.serverBaseDirectory = "";
info.trialFilenameBase = "";
        controlRef.runSequence(documented);
    }

    public void updateState(STIStateEvent event) {
        state = event.state();
    }
    public void updateMode(STIStateEvent event){

    }
    public void updateRunType(STIStateEvent event) {
        
    }

    private void setExpSequence(ExpSequence ExpSeq) {
        expSequenceRef = ExpSeq;
    }
    private void setParser(Parser parser) {
        parserRef = parser;
    }

    private void setControl(Control control) {
        controlRef = control;
    }


    public void parseLoopScript(String script) {

        boolean corbaError = false;
        boolean parseError = true;
        String init = "from stipy import *\n\nvariables=[]\nexperiments=[]\n\n";

        // String init = "from timing import *\nfrom numpy import *\n\nvariables=[]\nexperiments=[]\n\n";

        if (!state.equals(STIStateMachine.State.Running)) {
            try {
                parseError = parserRef.parseLoopScript(init + script);
            } catch (Exception e) {
                corbaError= true;
                e.printStackTrace(System.out);
            }

            if (!parseError) {
                TRow[] parsedRowData = null;
                try {
                    parsedRowData = expSequenceRef.experiments();
               
                    variables.clear();
                    variables.addAll(Arrays.asList(expSequenceRef.variables()));
                } catch (Exception e) {
                    corbaError = true;
                    e.printStackTrace(System.out);
                }
                //rowData[row][col]
     //           Object[][] rowData = new Object[parsedRowData.length][parsedRowData[0].val.length + 2];
                //Object[][] rowData =  new Object[parsedRowData[0].val.length + 2][parsedRowData.length];

                int numberOfVariables = 0;
                if(!corbaError) {
                    numberOfVariables = parsedRowData[0].val.length;
                }

                sequenceData.clear();

                for(int i = 0; ( !corbaError && i < parsedRowData.length ); i++) {
                    sequenceData.addElement( new Vector<Object>(numberOfVariables + 2) );

                    sequenceData.lastElement().add(new Integer( i + 1 ));
                    for(int j = 0; j < numberOfVariables; j++) {
                        sequenceData.lastElement().add( parsedRowData[i].val[j] );
                    }
                    sequenceData.lastElement().add(new Boolean(parsedRowData[i].done));

                }

                fireNewUpdateDataEvent();

            }
            else {             //parsing error
                String errorText;
                try {
                    errorText = parserRef.errMsg();
                    fireNewDisplayParsingErrorEvent(errorText);

                } catch (Exception e) {
                    corbaError = true;
                    e.printStackTrace(System.out);
                }
            }
        }

        if (corbaError) {
            fireNewDisplayParsingErrorEvent(corbaErrorText);
        }
    }

    private boolean reloadExpSequence() {

        boolean corbaError = false;

        try {
            expSequenceRef.clear();
        } catch (Exception e) {
            e.printStackTrace(System.out);
        }

        int numberOfVariables = variables.size();
        
        try {
            for (int i = 0; (!corbaError && i < sequenceData.size()); i++) {
                corbaError = expSequenceRef.appendRow(
                        (String[]) sequenceData.lastElement().subList(1, numberOfVariables + 1).toArray());
            }
        } catch (Exception e) {
            corbaError = true;
            e.printStackTrace(System.out);
        }
        return corbaError;
    }

    public Vector< Vector<Object> > getSequenceTableData() {
        return sequenceData;
    }

    public Vector<String> getColumnIndentifiers() {
        Vector<String> columnHeaders = new Vector<String>();

        columnHeaders.add("Trial");
        columnHeaders.addAll(1, variables);
        columnHeaders.add("Done");

        return columnHeaders;
    }
    private class SequenceTableRow {

        //   {"Trial", "var1", "var2", ...,  "Done"}

        private int columns = variables.size() + 2;
        Object[] rowData = new Object[columns];

        public SequenceTableRow(int trial, boolean done) {
            rowData[0] = trial;
            rowData[columns - 1] = done;
        }
        public Vector<Object> getRow() {
            Vector<Object> row = new Vector<Object>(columns);
            for(int i=0; i< rowData.length; i++) {
                row.addElement(rowData[i]);
            }
            return row;
        }


    }
}
