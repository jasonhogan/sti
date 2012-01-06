/** @file RunTab.java
 *  @author Jason Michael Hogan
 *  @brief Source-file for the class RunTab
 *  @section license License
 *
 *  Copyright (C) 2008 Jason Hogan <hogan@stanford.edu>\n
 *  This file is part of the Stanford Timing Interface (STI).
 *
 *  The STI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The STI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the STI.  If not, see <http://www.gnu.org/licenses/>.
 */

package edu.stanford.atom.sti.client.gui.RunTab;
import edu.stanford.atom.sti.client.comm.bl.*;

import org.fife.ui.rtextarea.*;
import org.fife.ui.rsyntaxtextarea.*;

import javax.swing.JOptionPane;

public class RunTab extends javax.swing.JPanel implements SequenceManagerListener, DataManagerListener {

    private RSyntaxTextArea textArea = new RSyntaxTextArea();
    RTextScrollPane scriptTextPane;

    private SequenceManager sequenceManager;
    private Thread parseThread = null;

    private SimpleSequenceRange simpleSequenceRange = null;

    public RunTab() {
        initComponents();

        simpleSequenceRange = new SimpleSequenceRange();
        simpleSequenceRange.lockIncrement();
        refreshSimpleTableUI();
        
        setupLoopVariablesTable();

        textArea.setSyntaxEditingStyle(SyntaxConstants.SYNTAX_STYLE_PYTHON);

        scriptTextPane = new RTextScrollPane(textArea);
        scriptPanel.add(scriptTextPane);




//        startValText.addFocusListener(new java.awt.event.FocusAdapter() {
//            public void focusLost(java.awt.event.FocusEvent e) {
//                System.out.println(startValText.getText());
//                pythonStringToValue(startValText.getText(), 0);
//            }
//        });
    }

    public void getData(DataManagerEvent event) {
        java.util.Vector <String> variableNames = ((DataManager)event.getSource()).getVariableNames();

        refreshVariableDropdown(variableNames);


    }

    private synchronized void refreshVariableDropdown(java.util.Vector <String> variableNames) {

        //int oldSelection = variableNameCombo.getSelectedIndex();

        Object oldItemSelection = variableNameCombo.getSelectedItem();

        variableNameCombo.removeAllItems();

        boolean varStillInList = false;
        for(int i = 0; i < variableNames.size(); i++) {
            variableNameCombo.addItem(variableNames.elementAt(i));

            if(oldItemSelection != null && 
                    oldItemSelection.toString().equals(variableNames.elementAt(i))) {
                varStillInList = true;
            }
        }

        if(varStillInList) {
            variableNameCombo.setSelectedItem(oldItemSelection);
        } else {
            variableNameCombo.getModel().setSelectedItem(null);
        }


//        if( oldSelection < variableNameCombo.getItemCount()) {
//            variableNameCombo.setSelectedIndex(oldSelection);
//        } else {
//            variableNameCombo.getModel().setSelectedItem(null);
//        }
    }

    private class ParsedPythonDouble {
        private String pythonCode = null;
        private double value;
        private boolean success = false;
        
        public ParsedPythonDouble(String pythonString) {
            pythonCode = pythonString;
            parse(pythonString);
        }
        
        public boolean isValid() {
            return success;
        }
        public double getValue() {
            return value;
        }
        
        private void parse(String code) {            
            if (code == null) {
                pythonCode = "";
                success = false;
                return;
            }

            edu.stanford.atom.sti.corba.Types.TValMixedHolder valMixed = 
                    new edu.stanford.atom.sti.corba.Types.TValMixedHolder();
            
            try {
                sequenceManager.getParser().stringToMixedValue(code, valMixed);
                success = valMixed.value.discriminator() == edu.stanford.atom.sti.corba.Types.TValue.ValueNumber;
                value = valMixed.value.number();
            } catch (Exception e) {
                success = false;
            }
        }
    }
    
    private double pythonStringToValue(String pythonString, double defaultValue) {

        boolean success = false;
        edu.stanford.atom.sti.corba.Types.TValMixedHolder valMixed = new edu.stanford.atom.sti.corba.Types.TValMixedHolder();
//        edu.stanford.atom.sti.corba.Types.TValMixed valMixed = new edu.stanford.atom.sti.corba.Types.TValMixed();

        if(pythonString == null){
//            valMixed.emptyValue(true);
            valMixed.value.emptyValue(true);
            return defaultValue;
        }

        try {
            sequenceManager.getParser().stringToMixedValue(pythonString, valMixed);
            success = valMixed.value.discriminator() == edu.stanford.atom.sti.corba.Types.TValue.ValueNumber;
        } catch(Exception e) {
        }

        if(!success) {
            return defaultValue;
        }

        return valMixed.value.number();
    }

    public void updateDoneStatus(int experimentNumber, boolean done) {
        loopVariablesTable.getModel().setValueAt(done, experimentNumber, loopVariablesTable.getModel().getColumnCount() - 1);
   
        refreshSequenceProgressBar();
    }

    private void refreshSequenceProgressBar() {
        int rows = loopVariablesTable.getModel().getRowCount();
        int doneColumn = loopVariablesTable.getModel().getColumnCount() - 1;    //Always the last column
        int progress = 0;
        for(int i = 0; i < rows; i++) {
            if((Boolean)loopVariablesTable.getModel().getValueAt(i, doneColumn)) {
                progress++;
            }
        }
        seriesProgressBar.setMinimum(0);
        seriesProgressBar.setMaximum(rows);
        seriesProgressBar.setValue(progress);

        sequenceCounterLabel.setText(progress + " / " + rows);
    }

    public void updateData(SequenceManagerEvent event) {
        loopVariablesTable.getModel().setDataVector( event.getSequenceTableData(),
                event.getSequenceTableColumnIdentifiers() );

        boolean[] editable = new boolean[event.getSequenceTableColumnIdentifiers().size()];

        for(int i = 0; i < editable.length; i++) {
            editable[i] = true;
        }
        editable[0] = false;

        loopVariablesTable.getModel().setEditableColumns(editable);
        refreshSequenceProgressBar();

    }
    public void displayParsingError(SequenceManagerEvent event) {
        JOptionPane.showMessageDialog(this,
                            event.getErrorText(),
                            "Parsing Error",
                            JOptionPane.ERROR_MESSAGE);
    }

    public void setupLoopVariablesTable() {

        loopVariablesTable.getModel().setDataVector(new Object[][]{},
                 new String[]{"Trial", "Done"});
        
        loopVariablesTable.getModel().setEditableColumns(
                new boolean[] {
            false, false});
    }

    public void setSequenceManager(SequenceManager sequenceManager) {
        this.sequenceManager = sequenceManager;
    }

    public class SimpleSequenceRange {
        private boolean startLocked;
        private boolean endLocked;
        private boolean incrementLocked;
        private boolean numberLocked;

        private ParsedPythonDouble pyStart;
        private ParsedPythonDouble pyEnd;
        private ParsedPythonDouble pyIncrement;
        
        private boolean rangeIsValid = false;
        private double start = 0;
        private double end = 0;
        private double increment = 0;
        private int number = 0;

        public SimpleSequenceRange() {
            refresh();
        }
        public boolean isValid() {
            return rangeIsValid;
        }

        private void refresh() {
            startLocked = startLockCheck.isSelected();
            endLocked = endLockCheck.isSelected();
            incrementLocked = incrementLockCheck.isSelected();
            numberLocked = numberLockCheck.isSelected();
        }

        public int getLockCount() {
            refresh();
            return (startLocked ? 1 :0) + (endLocked ? 1 :0) + 
                    (incrementLocked ? 1 :0) + (numberLocked ? 1 :0);
        }

        private boolean allLocked() {
            return (startLocked && endLocked && incrementLocked && numberLocked);
        }
        
        private void conditionallyDisableLastCheck() {
            if(getLockCount() < 3)
                return;
            
            if(!startLocked) {
                startValText.setText("");
                startLockCheck.setSelected(false);
                startLockCheck.setEnabled(false);
                startValText.setEnabled(false);

            }
            else if(!endLocked) {
                endValText.setText("");
                endLockCheck.setSelected(false);
                endLockCheck.setEnabled(false);
                endValText.setEnabled(false);

            }
            else if(!incrementLocked) {
                incrementValText.setText("");
                incrementLockCheck.setSelected(false);
                incrementLockCheck.setEnabled(false);
                incrementValText.setEnabled(false);
            }
            else {
                numberLockCheck.setSelected(false);
                numberLockCheck.setEnabled(false);
                numberSpinner.setEnabled(false);
            }
        }

        public void lockStart() {
            refresh();
            startLockCheck.setSelected(!allLocked());
            conditionallyDisableLastCheck();
        }
        public void lockEnd() {
            refresh();
            endLockCheck.setSelected(!allLocked());
            conditionallyDisableLastCheck();
        }
        public void lockIncrement() {
            refresh();
            incrementLockCheck.setSelected(!allLocked());
            conditionallyDisableLastCheck();
        }

        public void lockNumber() {
            refresh();
            numberLockCheck.setSelected(!allLocked());
            conditionallyDisableLastCheck();
        }
        public void unlock() {
            if(getLockCount() < 3) {
                startLockCheck.setEnabled(true);
                startValText.setEnabled(true);

                endLockCheck.setEnabled(true);
                endValText.setEnabled(true);

                incrementLockCheck.setEnabled(true);
                incrementValText.setEnabled(true);

                numberLockCheck.setEnabled(true);
                numberSpinner.setEnabled(true);
            }
        }
        
        private void setStart() {
            start = pyEnd.getValue() - number * pyIncrement.getValue();
        }
        
        public void calculateRange() {
            //Convert "python" code in the text boxes into numbers;
            //this may fail if the code is invalid.
            rangeIsValid = false;
            
            pyStart = new ParsedPythonDouble(startValText.getText());
            pyEnd = new ParsedPythonDouble(endValText.getText());
            pyIncrement = new ParsedPythonDouble(incrementValText.getText());
            number = ((Integer) numberSpinner.getValue()).intValue();
            
            int validCount = (pyStart.isValid() ? 1 : 0) 
                    + (pyEnd.isValid() ? 1 : 0) 
                    + (pyIncrement.isValid() ? 1 : 0)
                    + (number > 0 ? 1 : 0);
            
            if(validCount < 3) {
                rangeIsValid = false;
            } else if (validCount == 3) {
                rangeIsValid = setRange3valid();
            } else {
                rangeIsValid = setRange4valid();
            }

            if(rangeIsValid)
                System.out.println( "(" + start + "," + end + "," + increment + "," + number + ")" );
        }


        private boolean setRange3valid() {
            boolean success = false;

            if(!pyStart.isValid() && pyEnd.isValid() && pyIncrement.isValid() && number > 0) {
                end = pyEnd.getValue();
                increment = pyIncrement.getValue();
                start = end - increment * number;
                success = true;
            }
            else if(pyStart.isValid() && !pyEnd.isValid() && pyIncrement.isValid() && number > 0) {
                start = pyStart.getValue();
                increment = pyIncrement.getValue();
                end = start + increment * number;
                success = true;
            }
            else if(pyStart.isValid() && pyEnd.isValid() && !pyIncrement.isValid() && number > 0) {
                start = pyStart.getValue();
                end = pyEnd.getValue();
                increment = (end - start) / number;
                success = true;
            }
            else {
                start = pyStart.getValue();
                end = pyEnd.getValue();
                increment = pyIncrement.getValue();
                if(increment > 0) {
                    number = (int) Math.abs((end - start) / increment);
                    success = true;
                }
            }
            return success;
        }
                
        private boolean setRange4valid() {
            switch(getLockCount()) {
                case 0:
                    break;
            }
            return false;
        }
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        lockButtonGroup = new javax.swing.ButtonGroup();
        loopPanel = new javax.swing.JPanel();
        loopCountLabel = new javax.swing.JLabel();
        continuousCheckBox = new javax.swing.JCheckBox();
        loopCountTextField = new javax.swing.JTextField();
        repeatSlashLabel = new javax.swing.JLabel();
        loopCountSpinner = new javax.swing.JSpinner();
        repeatCheckBox = new javax.swing.JCheckBox();
        loopEditorSplitPane = new javax.swing.JSplitPane();
        jTabbedPane1 = new javax.swing.JTabbedPane();
        simplePanel = new javax.swing.JPanel();
        variableNameCombo = new javax.swing.JComboBox();
        numberSpinner = new javax.swing.JSpinner();
        startValText = new javax.swing.JTextField();
        jLabel1 = new javax.swing.JLabel();
        jLabel2 = new javax.swing.JLabel();
        endValText = new javax.swing.JTextField();
        jLabel7 = new javax.swing.JLabel();
        incrementValText = new javax.swing.JTextField();
        jLabel8 = new javax.swing.JLabel();
        jLabel9 = new javax.swing.JLabel();
        startLockCheck = new javax.swing.JCheckBox();
        endLockCheck = new javax.swing.JCheckBox();
        incrementLockCheck = new javax.swing.JCheckBox();
        numberLockCheck = new javax.swing.JCheckBox();
        generateTableButton = new javax.swing.JButton();
        shuffleButton = new javax.swing.JButton();
        jSplitPane1 = new javax.swing.JSplitPane();
        scriptParsePanel = new javax.swing.JPanel();
        variablesLabel1 = new javax.swing.JLabel();
        experiementsLabel1 = new javax.swing.JLabel();
        parseButton = new javax.swing.JButton();
        scriptPanel = new javax.swing.JPanel();
        jScrollPane1 = new javax.swing.JScrollPane();
        loopVariablesTable = new edu.stanford.atom.sti.client.gui.table.STITable();
        jPanel2 = new javax.swing.JPanel();
        jLabel3 = new javax.swing.JLabel();
        jLabel4 = new javax.swing.JLabel();
        seriesProgressBar = new javax.swing.JProgressBar();
        seriesProgressBar1 = new javax.swing.JProgressBar();
        sequenceCounterLabel = new javax.swing.JLabel();

        loopPanel.setBorder(javax.swing.BorderFactory.createEtchedBorder());

        loopCountLabel.setText("Loop Count");
        loopCountLabel.setEnabled(false);

        continuousCheckBox.setText("Continuous");
        continuousCheckBox.setEnabled(false);

        loopCountTextField.setText("0");
        loopCountTextField.setEnabled(false);

        repeatSlashLabel.setText("/");
        repeatSlashLabel.setEnabled(false);

        loopCountSpinner.setEnabled(false);
        loopCountSpinner.setOpaque(false);
        loopCountSpinner.setValue(1);

        repeatCheckBox.setText("Repeat Sequence");
        repeatCheckBox.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                repeatCheckBoxActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout loopPanelLayout = new javax.swing.GroupLayout(loopPanel);
        loopPanel.setLayout(loopPanelLayout);
        loopPanelLayout.setHorizontalGroup(
            loopPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(loopPanelLayout.createSequentialGroup()
                .addComponent(repeatCheckBox)
                .addGap(150, 150, 150))
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, loopPanelLayout.createSequentialGroup()
                .addGap(6, 6, 6)
                .addComponent(loopCountLabel)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(loopCountTextField, javax.swing.GroupLayout.PREFERRED_SIZE, 39, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(repeatSlashLabel)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(loopCountSpinner, javax.swing.GroupLayout.DEFAULT_SIZE, 48, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(continuousCheckBox)
                .addContainerGap())
        );
        loopPanelLayout.setVerticalGroup(
            loopPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(loopPanelLayout.createSequentialGroup()
                .addComponent(repeatCheckBox)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(loopPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(continuousCheckBox)
                    .addComponent(repeatSlashLabel)
                    .addComponent(loopCountTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(loopCountSpinner, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(loopCountLabel))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        loopEditorSplitPane.setDividerLocation(160);
        loopEditorSplitPane.setDividerSize(8);
        loopEditorSplitPane.setOrientation(javax.swing.JSplitPane.VERTICAL_SPLIT);
        loopEditorSplitPane.setMinimumSize(new java.awt.Dimension(10, 56));

        jTabbedPane1.setTabPlacement(javax.swing.JTabbedPane.BOTTOM);

        variableNameCombo.setEditable(true);
        variableNameCombo.setModel(new javax.swing.DefaultComboBoxModel(new String[] { }));

        numberSpinner.setToolTipText("The number of trials in this sequence");
        numberSpinner.addChangeListener(new javax.swing.event.ChangeListener() {
            public void stateChanged(javax.swing.event.ChangeEvent evt) {
                numberSpinnerStateChanged(evt);
            }
        });

        startValText.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                startValTextActionPerformed(evt);
            }
        });

        jLabel1.setFont(new java.awt.Font("Tahoma", 1, 12)); // NOI18N
        jLabel1.setText("Start");

        jLabel2.setFont(new java.awt.Font("Tahoma", 1, 12)); // NOI18N
        jLabel2.setText("End");

        endValText.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                endValTextActionPerformed(evt);
            }
        });

        jLabel7.setFont(new java.awt.Font("Tahoma", 1, 12)); // NOI18N
        jLabel7.setText("Increment");

        jLabel8.setFont(new java.awt.Font("Tahoma", 1, 12)); // NOI18N
        jLabel8.setText("Number");

        jLabel9.setFont(new java.awt.Font("Tahoma", 1, 12));
        jLabel9.setText("Variable");

        startLockCheck.setSelected(true);
        startLockCheck.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                startLockCheckActionPerformed(evt);
            }
        });

        endLockCheck.setSelected(true);
        endLockCheck.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                endLockCheckActionPerformed(evt);
            }
        });

        incrementLockCheck.setSelected(true);
        incrementLockCheck.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                incrementLockCheckActionPerformed(evt);
            }
        });

        numberLockCheck.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                numberLockCheckActionPerformed(evt);
            }
        });

        generateTableButton.setFont(new java.awt.Font("Tahoma", 1, 11)); // NOI18N
        generateTableButton.setText("Generate Table");
        generateTableButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                generateTableButtonActionPerformed(evt);
            }
        });

        shuffleButton.setText("Random Shuffle");
        shuffleButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                shuffleButtonActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout simplePanelLayout = new javax.swing.GroupLayout(simplePanel);
        simplePanel.setLayout(simplePanelLayout);
        simplePanelLayout.setHorizontalGroup(
            simplePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(simplePanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(simplePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(simplePanelLayout.createSequentialGroup()
                        .addGroup(simplePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(jLabel9)
                            .addComponent(variableNameCombo, javax.swing.GroupLayout.PREFERRED_SIZE, 168, javax.swing.GroupLayout.PREFERRED_SIZE))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                        .addGroup(simplePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                            .addGroup(simplePanelLayout.createSequentialGroup()
                                .addComponent(jLabel1)
                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                                .addComponent(startLockCheck))
                            .addComponent(startValText, javax.swing.GroupLayout.PREFERRED_SIZE, 94, javax.swing.GroupLayout.PREFERRED_SIZE))
                        .addGap(18, 18, 18)
                        .addGroup(simplePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                            .addGroup(simplePanelLayout.createSequentialGroup()
                                .addComponent(jLabel2)
                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                                .addComponent(endLockCheck))
                            .addComponent(endValText, javax.swing.GroupLayout.PREFERRED_SIZE, 94, javax.swing.GroupLayout.PREFERRED_SIZE))
                        .addGap(18, 18, 18)
                        .addGroup(simplePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                            .addGroup(simplePanelLayout.createSequentialGroup()
                                .addComponent(jLabel7)
                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                                .addComponent(incrementLockCheck, javax.swing.GroupLayout.PREFERRED_SIZE, 21, javax.swing.GroupLayout.PREFERRED_SIZE))
                            .addComponent(incrementValText, javax.swing.GroupLayout.PREFERRED_SIZE, 94, javax.swing.GroupLayout.PREFERRED_SIZE))
                        .addGap(18, 18, 18)
                        .addGroup(simplePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                            .addGroup(simplePanelLayout.createSequentialGroup()
                                .addComponent(jLabel8)
                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                                .addComponent(numberLockCheck))
                            .addComponent(numberSpinner)))
                    .addGroup(simplePanelLayout.createSequentialGroup()
                        .addComponent(generateTableButton)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                        .addComponent(shuffleButton)))
                .addContainerGap(193, Short.MAX_VALUE))
        );
        simplePanelLayout.setVerticalGroup(
            simplePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(simplePanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(simplePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                    .addGroup(simplePanelLayout.createSequentialGroup()
                        .addGroup(simplePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(jLabel2)
                            .addComponent(endLockCheck, javax.swing.GroupLayout.PREFERRED_SIZE, 14, javax.swing.GroupLayout.PREFERRED_SIZE))
                        .addGap(5, 5, 5)
                        .addComponent(endValText, javax.swing.GroupLayout.PREFERRED_SIZE, 26, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addGroup(simplePanelLayout.createSequentialGroup()
                        .addGroup(simplePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                            .addComponent(incrementLockCheck, javax.swing.GroupLayout.Alignment.CENTER, javax.swing.GroupLayout.PREFERRED_SIZE, 16, javax.swing.GroupLayout.PREFERRED_SIZE)
                            .addComponent(numberLockCheck, javax.swing.GroupLayout.Alignment.CENTER)
                            .addComponent(jLabel8, javax.swing.GroupLayout.Alignment.CENTER))
                        .addGap(5, 5, 5)
                        .addComponent(numberSpinner, javax.swing.GroupLayout.PREFERRED_SIZE, 25, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addGroup(simplePanelLayout.createSequentialGroup()
                        .addComponent(jLabel7)
                        .addGap(5, 5, 5)
                        .addComponent(incrementValText, javax.swing.GroupLayout.PREFERRED_SIZE, 26, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addGroup(simplePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                        .addGroup(javax.swing.GroupLayout.Alignment.LEADING, simplePanelLayout.createSequentialGroup()
                            .addGroup(simplePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                                .addComponent(jLabel1, javax.swing.GroupLayout.Alignment.CENTER)
                                .addComponent(startLockCheck, javax.swing.GroupLayout.PREFERRED_SIZE, 13, javax.swing.GroupLayout.PREFERRED_SIZE))
                            .addGap(5, 5, 5)
                            .addComponent(startValText, javax.swing.GroupLayout.PREFERRED_SIZE, 26, javax.swing.GroupLayout.PREFERRED_SIZE))
                        .addGroup(simplePanelLayout.createSequentialGroup()
                            .addComponent(jLabel9)
                            .addGap(5, 5, 5)
                            .addComponent(variableNameCombo, javax.swing.GroupLayout.PREFERRED_SIZE, 26, javax.swing.GroupLayout.PREFERRED_SIZE))))
                .addGap(18, 18, 18)
                .addGroup(simplePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(generateTableButton)
                    .addComponent(shuffleButton))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        jTabbedPane1.addTab("Simple Sequence", simplePanel);

        jSplitPane1.setDividerLocation(35);
        jSplitPane1.setDividerSize(1);
        jSplitPane1.setOrientation(javax.swing.JSplitPane.VERTICAL_SPLIT);

        variablesLabel1.setText(" variables = []");

        experiementsLabel1.setText(" experiments = []");

        parseButton.setFont(new java.awt.Font("Tahoma", 0, 14));
        parseButton.setText("Parse");
        parseButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                parseButtonActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout scriptParsePanelLayout = new javax.swing.GroupLayout(scriptParsePanel);
        scriptParsePanel.setLayout(scriptParsePanelLayout);
        scriptParsePanelLayout.setHorizontalGroup(
            scriptParsePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(scriptParsePanelLayout.createSequentialGroup()
                .addGroup(scriptParsePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(variablesLabel1)
                    .addComponent(experiementsLabel1))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 427, Short.MAX_VALUE)
                .addComponent(parseButton, javax.swing.GroupLayout.PREFERRED_SIZE, 94, javax.swing.GroupLayout.PREFERRED_SIZE))
        );
        scriptParsePanelLayout.setVerticalGroup(
            scriptParsePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(scriptParsePanelLayout.createSequentialGroup()
                .addGroup(scriptParsePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(scriptParsePanelLayout.createSequentialGroup()
                        .addComponent(variablesLabel1)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(experiementsLabel1))
                    .addComponent(parseButton, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 34, Short.MAX_VALUE))
                .addContainerGap())
        );

        jSplitPane1.setTopComponent(scriptParsePanel);
        scriptParsePanel.getAccessibleContext().setAccessibleParent(jSplitPane1);

        scriptPanel.setLayout(new java.awt.BorderLayout());
        jSplitPane1.setRightComponent(scriptPanel);

        jTabbedPane1.addTab("Python Loop Script", jSplitPane1);

        loopEditorSplitPane.setTopComponent(jTabbedPane1);
        jTabbedPane1.getAccessibleContext().setAccessibleParent(loopEditorSplitPane);

        jScrollPane1.setViewportView(loopVariablesTable);

        loopEditorSplitPane.setRightComponent(jScrollPane1);

        jLabel3.setFont(new java.awt.Font("Tahoma", 1, 11));
        jLabel3.setText("Trial");

        jLabel4.setFont(new java.awt.Font("Tahoma", 1, 11));
        jLabel4.setText("Sequence");

        seriesProgressBar.setMaximumSize(new java.awt.Dimension(32767, 10));
        seriesProgressBar.setMinimumSize(new java.awt.Dimension(10, 10));
        seriesProgressBar.setPreferredSize(new java.awt.Dimension(146, 10));

        seriesProgressBar1.setMaximumSize(new java.awt.Dimension(32767, 10));
        seriesProgressBar1.setMinimumSize(new java.awt.Dimension(10, 10));
        seriesProgressBar1.setPreferredSize(new java.awt.Dimension(146, 10));

        sequenceCounterLabel.setHorizontalAlignment(javax.swing.SwingConstants.LEFT);
        sequenceCounterLabel.setText("0 / 0");

        javax.swing.GroupLayout jPanel2Layout = new javax.swing.GroupLayout(jPanel2);
        jPanel2.setLayout(jPanel2Layout);
        jPanel2Layout.setHorizontalGroup(
            jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel2Layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                    .addComponent(jLabel3)
                    .addComponent(jLabel4))
                .addGap(18, 18, 18)
                .addGroup(jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(seriesProgressBar1, javax.swing.GroupLayout.DEFAULT_SIZE, 171, Short.MAX_VALUE)
                    .addComponent(seriesProgressBar, javax.swing.GroupLayout.DEFAULT_SIZE, 171, Short.MAX_VALUE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(sequenceCounterLabel, javax.swing.GroupLayout.DEFAULT_SIZE, 57, Short.MAX_VALUE)
                .addContainerGap())
        );
        jPanel2Layout.setVerticalGroup(
            jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel2Layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(sequenceCounterLabel, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 18, Short.MAX_VALUE)
                    .addComponent(jLabel4, javax.swing.GroupLayout.DEFAULT_SIZE, 18, Short.MAX_VALUE)
                    .addComponent(seriesProgressBar, javax.swing.GroupLayout.DEFAULT_SIZE, 18, Short.MAX_VALUE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(jLabel3, javax.swing.GroupLayout.Alignment.TRAILING)
                    .addComponent(seriesProgressBar1, javax.swing.GroupLayout.DEFAULT_SIZE, 19, Short.MAX_VALUE))
                .addContainerGap())
        );

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                    .addComponent(loopEditorSplitPane, javax.swing.GroupLayout.DEFAULT_SIZE, 614, Short.MAX_VALUE)
                    .addGroup(layout.createSequentialGroup()
                        .addComponent(jPanel2, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                        .addGap(18, 18, 18)
                        .addComponent(loopPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)))
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                    .addComponent(jPanel2, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(loopPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(loopEditorSplitPane, javax.swing.GroupLayout.DEFAULT_SIZE, 521, Short.MAX_VALUE)
                .addContainerGap())
        );
    }// </editor-fold>//GEN-END:initComponents

    

    private void parseButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_parseButtonActionPerformed
        parseThread = new Thread(new Runnable() {
            public void run() {
                sequenceManager.parseLoopScript( scriptTextPane.getTextArea().getText() );
            }
        });
        parseThread.start();
}//GEN-LAST:event_parseButtonActionPerformed

    private void repeatCheckBoxActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_repeatCheckBoxActionPerformed
        boolean enabled = repeatCheckBox.isSelected();

        loopCountLabel.setEnabled(enabled);
        continuousCheckBox.setEnabled(enabled);
        loopCountTextField.setEnabled(enabled);
        repeatSlashLabel.setEnabled(enabled);
        loopCountSpinner.setEnabled(enabled);
    }//GEN-LAST:event_repeatCheckBoxActionPerformed

    private void shuffleButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_shuffleButtonActionPerformed
        generateSimpleScript(true);
    }//GEN-LAST:event_shuffleButtonActionPerformed

    private void startValTextActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_startValTextActionPerformed
        //System.out.println(startValText.getText());
    }//GEN-LAST:event_startValTextActionPerformed

    private void endValTextActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_endValTextActionPerformed
        // TODO add your handling code here:
    }//GEN-LAST:event_endValTextActionPerformed

    private void startLockCheckActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_startLockCheckActionPerformed
        if (startLockCheck.isSelected()) {
            simpleSequenceRange.lockStart();
        } else {
            simpleSequenceRange.unlock();
        }

        refreshSimpleTableUI();
    }//GEN-LAST:event_startLockCheckActionPerformed

    private void endLockCheckActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_endLockCheckActionPerformed
        if(endLockCheck.isSelected()) {
            simpleSequenceRange.lockEnd();
        } else {
            simpleSequenceRange.unlock();
        }

        refreshSimpleTableUI();
    }//GEN-LAST:event_endLockCheckActionPerformed

    private void incrementLockCheckActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_incrementLockCheckActionPerformed
        if(incrementLockCheck.isSelected()) {
            simpleSequenceRange.lockIncrement();
        } else {
            simpleSequenceRange.unlock();
        }

        refreshSimpleTableUI();
    }//GEN-LAST:event_incrementLockCheckActionPerformed

    private void numberLockCheckActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_numberLockCheckActionPerformed
        if(numberLockCheck.isSelected()) {
            simpleSequenceRange.lockNumber();
        } else {
            simpleSequenceRange.unlock();
        }
        
        refreshSimpleTableUI();
    }//GEN-LAST:event_numberLockCheckActionPerformed

    private void refreshSimpleTableUI() {
        if(simpleSequenceRange.getLockCount() == 3) {
            enableSimpleTableGeneratorUI(true);
        } else {
            enableSimpleTableGeneratorUI(false);
        }
    }

    private void enableSimpleTableGeneratorUI(boolean enable) {
        shuffleButton.setEnabled(enable);
        generateTableButton.setEnabled(enable);
    }

    private void numberSpinnerStateChanged(javax.swing.event.ChangeEvent evt) {//GEN-FIRST:event_numberSpinnerStateChanged

        int value = 0;
        try {
            value = Integer.decode(numberSpinner.getValue().toString()).intValue();
        } catch(NumberFormatException e) {
            value = 0;
        }
        if(value < 0) {
            value = 0;
        }
        numberSpinner.setValue(value);
    }//GEN-LAST:event_numberSpinnerStateChanged

    private String getPythonSimpleLoopScript(boolean shuffle) {
        
        String script;

        String unitDef = "\n\nns = 1.0\nus = 1000.0\nms = 1000000.0\ns = 1000000000.0\n\n";

        String variable = "variables.append('" + variableNameCombo.getSelectedItem().toString() + "')\n\n";

        String rangeVars = getSimpleRangePython();

        String rangeDef = "iRange = range(0, number)\n";

        String forLoop = "for i in iRange :\n    experiments.append([start + i*delta])\n\n";
        
        if(shuffle) {
            script = "import random" + unitDef + variable + rangeVars + rangeDef 
                    + "random.shuffle(iRange)\n\n" + forLoop;
        } else {
            script = unitDef + variable + rangeVars + rangeDef + forLoop;
        }

        return script;
    }

    private void generateTableButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_generateTableButtonActionPerformed
        generateSimpleScript(false);
    }//GEN-LAST:event_generateTableButtonActionPerformed

    private void generateSimpleScript(final boolean shuffle) {

        parseThread = new Thread(new Runnable() {
            public void run() {
                sequenceManager.parseLoopScript( getPythonSimpleLoopScript(shuffle) );
            }
        });
        parseThread.start();
    }

    private String getSimpleRangePython() {
        String range = "";
        
        if(!startLockCheck.isSelected()) {
            range = "\nend = " + endValText.getText()
                + "\nnumber = " + numberSpinner.getValue().toString()
                + "\ndelta = " + incrementValText.getText()
                + "\nstart = end - ((number - 1)*delta)"
                + "\n\n";
        }
        else if(!endLockCheck.isSelected()) {
            range = "start = " + startValText.getText()
                + "\nnumber = " + numberSpinner.getValue().toString()
                + "\ndelta = " + incrementValText.getText()
                + "\nend = start + (number*delta)"
                + "\n\n";
        }        
        else if(!incrementLockCheck.isSelected()) {
            range = "start = " + startValText.getText()
                + "\nend = " + endValText.getText()
                + "\nnumber = " + numberSpinner.getValue().toString()
                + "\ndelta = (end - start)/(number - 1)"
                + "\n\n";
        }
        else if(!numberLockCheck.isSelected()) {
            range = "start = " + startValText.getText()
                + "\nend = " + endValText.getText()
                + "\ndelta = " + incrementValText.getText()
                + "\nnumber = ((end - start)/delta) + 1"
                + "\n\n";
        }
        else {
            range = "start = 0\nend = 0\ndelta=0\nnumber=0\n\n";
        }
        return range;
    }


    // Variables declaration - do not modify//GEN-BEGIN:variables
    javax.swing.JCheckBox continuousCheckBox;
    javax.swing.JCheckBox endLockCheck;
    javax.swing.JTextField endValText;
    javax.swing.JLabel experiementsLabel1;
    javax.swing.JButton generateTableButton;
    javax.swing.JCheckBox incrementLockCheck;
    javax.swing.JTextField incrementValText;
    javax.swing.JLabel jLabel1;
    javax.swing.JLabel jLabel2;
    javax.swing.JLabel jLabel3;
    javax.swing.JLabel jLabel4;
    javax.swing.JLabel jLabel7;
    javax.swing.JLabel jLabel8;
    javax.swing.JLabel jLabel9;
    javax.swing.JPanel jPanel2;
    javax.swing.JScrollPane jScrollPane1;
    javax.swing.JSplitPane jSplitPane1;
    javax.swing.JTabbedPane jTabbedPane1;
    javax.swing.ButtonGroup lockButtonGroup;
    javax.swing.JLabel loopCountLabel;
    javax.swing.JSpinner loopCountSpinner;
    javax.swing.JTextField loopCountTextField;
    javax.swing.JSplitPane loopEditorSplitPane;
    javax.swing.JPanel loopPanel;
    edu.stanford.atom.sti.client.gui.table.STITable loopVariablesTable;
    javax.swing.JCheckBox numberLockCheck;
    javax.swing.JSpinner numberSpinner;
    javax.swing.JButton parseButton;
    javax.swing.JCheckBox repeatCheckBox;
    javax.swing.JLabel repeatSlashLabel;
    javax.swing.JPanel scriptPanel;
    javax.swing.JPanel scriptParsePanel;
    javax.swing.JLabel sequenceCounterLabel;
    javax.swing.JProgressBar seriesProgressBar;
    javax.swing.JProgressBar seriesProgressBar1;
    javax.swing.JButton shuffleButton;
    javax.swing.JPanel simplePanel;
    javax.swing.JCheckBox startLockCheck;
    javax.swing.JTextField startValText;
    javax.swing.JComboBox variableNameCombo;
    javax.swing.JLabel variablesLabel1;
    // End of variables declaration//GEN-END:variables

}
