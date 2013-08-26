/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * DocumentationTab.java
 *
 * Created on Dec 17, 2009, 5:05:45 PM
 */

package edu.stanford.atom.sti.client.gui.RunTab;

import edu.stanford.atom.sti.client.comm.io.*;
import edu.stanford.atom.sti.corba.Client_Server.DocumentationSettings;
import edu.stanford.atom.sti.corba.Types.TExpSequenceInfo;
import java.awt.Dialog.ModalityType;
import java.awt.event.*;

/**
 *
 * @author EP
 */
public class DocumentationTab extends javax.swing.JPanel implements ServerConnectionListener {

    private TExpSequenceInfo experimentSeqInfo = new TExpSequenceInfo();

    private DocumentationSettings docSettings = null;

    String TimingFilesRelDir;
    String DataFilesRelDir;
    String ExperimentFilesRelDir;
    String SequenceFilesRelDir;

    private boolean playAfterEnterDescription = false;

    /** Creates new form DocumentationTab */
    public DocumentationTab() {
        initComponents();


        seqDecriptionDialog.setModalityType(ModalityType.APPLICATION_MODAL);
        seqDecriptionDialog.setTitle("Enter Sequence Description");


        baseDirField.addFocusListener(new FocusAdapter() {

            @Override
            public void focusLost(FocusEvent e) {
                baseDirFieldActionPerformed(new ActionEvent(experimentDirField, ActionEvent.ACTION_PERFORMED, ""));
            }
        });
        dtdDirField.addFocusListener(new FocusAdapter() {

            @Override
            public void focusLost(FocusEvent e) {
                dtdDirFieldActionPerformed(new ActionEvent(experimentDirField, ActionEvent.ACTION_PERFORMED, ""));
            }
        });
        timingDirField.addFocusListener(new FocusAdapter() {

            @Override
            public void focusLost(FocusEvent e) {
                timingDirFieldActionPerformed(new ActionEvent(experimentDirField, ActionEvent.ACTION_PERFORMED, ""));
            }
        });
        dataDirField.addFocusListener(new FocusAdapter() {

            @Override
            public void focusLost(FocusEvent e) {
                dataDirFieldActionPerformed(new ActionEvent(experimentDirField, ActionEvent.ACTION_PERFORMED, ""));
            }
        });
        experimentDirField.addFocusListener(new FocusAdapter() {

            @Override
            public void focusLost(FocusEvent e) {

                experimentDirFieldActionPerformed(new ActionEvent(experimentDirField, ActionEvent.ACTION_PERFORMED, ""));
                //experimentDirField.dispatchEvent(new ActionEvent(experimentDirField, ActionEvent.ACTION_PERFORMED, ""));
            }
        });
        sequenceDirField.addFocusListener(new FocusAdapter() {

            @Override
            public void focusLost(FocusEvent e) {
                sequenceDirFieldActionPerformed(new ActionEvent(experimentDirField, ActionEvent.ACTION_PERFORMED, ""));
            }
        });
    }

    public void installServants(ServerConnectionEvent event) {
        docSettings = event.getServerConnection().getDocumentationSettings();

        try {
            baseDirField.setText(docSettings.DocumentationBaseAbsDir());
            dtdDirField.setText(docSettings.DTDFileAbsDir());
            
            timingDirField.setText(docSettings.TimingFilesRelDir());
            dataDirField.setText(docSettings.DataFilesRelDir());
            experimentDirField.setText(docSettings.ExperimentFilesRelDir());
            sequenceDirField.setText(docSettings.SequenceFilesRelDir());
        } catch (Exception e) {
            e.printStackTrace();
        }

    }
    public void uninstallServants(ServerConnectionEvent event) {
        docSettings = null;
    }
    
    public boolean promptForSeqDescription() {
        return promptForSeqCheck.isSelected();
    }
    public String getSeqDescription() {
        return sequenceDescription.getText();
    }
    
    //returns true if play should proceed, false if cancelled.
    public boolean showDialogForSeqDescriptionBeforePlay() {
        boolean play = false;
        
        javax.swing.ImageIcon playIcon = new javax.swing.ImageIcon(getClass().getResource("/edu/stanford/atom/sti/client/resources/Play16.gif"));
        
        Object[] options = {null};

        //javax.swing.jo
        //JOptionPane
//        Object dialogResult = JOptionPane.showInternalInputDialog(this,
//                "Enter a Sequence Description:",
//                "Sequence Description",
//                JOptionPane.PLAIN_MESSAGE,
//                playIcon,
//                options,
//                sequenceDescription.getText());


        playAfterEnterDescription = false;
        seqDescDialogTextArea.setText(sequenceDescription.getText());
        seqDecriptionDialog.setVisible(true);
        play = playAfterEnterDescription;

        //if "Play" was pressed, save this sequence description to the server.
        if (play) {
            if (docSettings != null) {
                try {
                    docSettings.setSequenceDescription(sequenceDescription.getText());
                } catch (Exception e) {
                }
            }
        }
        playAfterEnterDescription = false;

        return play;
    }

    public TExpSequenceInfo getTExpSequenceInfo() {
        experimentSeqInfo.sequenceDescription = sequenceDescription.getText();
        return experimentSeqInfo;
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        seqDecriptionDialog = new javax.swing.JDialog();
        jScrollPane2 = new javax.swing.JScrollPane();
        seqDescDialogTextArea = new javax.swing.JTextArea();
        seqDescDialogPlayButton = new javax.swing.JButton();
        seqDescDialogCancelButton = new javax.swing.JButton();
        jScrollPane1 = new javax.swing.JScrollPane();
        sequenceDescription = new javax.swing.JTextArea();
        jPanel1 = new javax.swing.JPanel();
        baseDirField = new javax.swing.JTextField();
        jLabel1 = new javax.swing.JLabel();
        jLabel2 = new javax.swing.JLabel();
        dtdDirField = new javax.swing.JTextField();
        jButton1 = new javax.swing.JButton();
        jButton2 = new javax.swing.JButton();
        timingDirField = new javax.swing.JTextField();
        jLabel3 = new javax.swing.JLabel();
        jSeparator1 = new javax.swing.JSeparator();
        jCheckBox1 = new javax.swing.JCheckBox();
        jSeparator2 = new javax.swing.JSeparator();
        jLabel4 = new javax.swing.JLabel();
        dataDirField = new javax.swing.JTextField();
        jLabel5 = new javax.swing.JLabel();
        experimentDirField = new javax.swing.JTextField();
        jLabel6 = new javax.swing.JLabel();
        jLabel7 = new javax.swing.JLabel();
        jLabel8 = new javax.swing.JLabel();
        sequenceDirField = new javax.swing.JTextField();
        jLabel9 = new javax.swing.JLabel();
        jLabel10 = new javax.swing.JLabel();
        promptForSeqCheck = new javax.swing.JCheckBox();

        seqDecriptionDialog.setAlwaysOnTop(true);
        seqDecriptionDialog.setMinimumSize(new java.awt.Dimension(397, 200));
        seqDecriptionDialog.setModalityType(java.awt.Dialog.ModalityType.APPLICATION_MODAL);
        seqDecriptionDialog.setName("Enter Sequence Description"); // NOI18N
        seqDecriptionDialog.setResizable(false);

        seqDescDialogTextArea.setColumns(20);
        seqDescDialogTextArea.setRows(5);
        jScrollPane2.setViewportView(seqDescDialogTextArea);

        seqDescDialogPlayButton.setIcon(new javax.swing.ImageIcon(getClass().getResource("/edu/stanford/atom/sti/client/resources/Play16.gif"))); // NOI18N
        seqDescDialogPlayButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                seqDescDialogPlayButtonActionPerformed(evt);
            }
        });

        seqDescDialogCancelButton.setText("Cancel");
        seqDescDialogCancelButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                seqDescDialogCancelButtonActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout seqDecriptionDialogLayout = new javax.swing.GroupLayout(seqDecriptionDialog.getContentPane());
        seqDecriptionDialog.getContentPane().setLayout(seqDecriptionDialogLayout);
        seqDecriptionDialogLayout.setHorizontalGroup(
            seqDecriptionDialogLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(seqDecriptionDialogLayout.createSequentialGroup()
                .addGap(120, 120, 120)
                .addComponent(seqDescDialogPlayButton, javax.swing.GroupLayout.PREFERRED_SIZE, 68, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(18, 18, 18)
                .addComponent(seqDescDialogCancelButton)
                .addContainerGap(126, Short.MAX_VALUE))
            .addComponent(jScrollPane2, javax.swing.GroupLayout.DEFAULT_SIZE, 397, Short.MAX_VALUE)
        );
        seqDecriptionDialogLayout.setVerticalGroup(
            seqDecriptionDialogLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(seqDecriptionDialogLayout.createSequentialGroup()
                .addComponent(jScrollPane2, javax.swing.GroupLayout.PREFERRED_SIZE, 127, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(seqDecriptionDialogLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(seqDescDialogCancelButton, javax.swing.GroupLayout.DEFAULT_SIZE, 32, Short.MAX_VALUE)
                    .addComponent(seqDescDialogPlayButton, javax.swing.GroupLayout.PREFERRED_SIZE, 32, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addContainerGap())
        );

        jScrollPane1.setBorder(javax.swing.BorderFactory.createTitledBorder("Sequence Description"));

        sequenceDescription.setColumns(20);
        sequenceDescription.setRows(5);
        jScrollPane1.setViewportView(sequenceDescription);

        jPanel1.setBorder(javax.swing.BorderFactory.createTitledBorder("Directory Settings"));

        baseDirField.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                baseDirFieldActionPerformed(evt);
            }
        });

        jLabel1.setText("Base Directory (Absolute)");

        jLabel2.setText("DTD Directory (Absolute)");

        dtdDirField.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                dtdDirFieldActionPerformed(evt);
            }
        });

        jButton1.setText("...");

        jButton2.setText("...");

        timingDirField.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                timingDirFieldActionPerformed(evt);
            }
        });

        jLabel3.setText("Timing Files (Relative)");

        jCheckBox1.setSelected(true);
        jCheckBox1.setText("Save files in date-specific subdirectories ( i.e., <Base>\\YYYY\\MM\\DD\\ )");
        jCheckBox1.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jCheckBox1ActionPerformed(evt);
            }
        });

        jLabel4.setText("Data Files (Relative)");

        dataDirField.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                dataDirFieldActionPerformed(evt);
            }
        });

        jLabel5.setText("Experiment Files (Relative)");

        experimentDirField.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                experimentDirFieldActionPerformed(evt);
            }
        });

        jLabel6.setText("( *.py )");

        jLabel7.setText("( *.xml )");

        jLabel8.setText("( *.xml )");

        sequenceDirField.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                sequenceDirFieldActionPerformed(evt);
            }
        });

        jLabel9.setText("Sequence Files (Relative)");

        jLabel10.setText("( *.tif, *.dat, *.xml, ... )");

        javax.swing.GroupLayout jPanel1Layout = new javax.swing.GroupLayout(jPanel1);
        jPanel1.setLayout(jPanel1Layout);
        jPanel1Layout.setHorizontalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel1Layout.createSequentialGroup()
                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(jPanel1Layout.createSequentialGroup()
                        .addContainerGap()
                        .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(jCheckBox1)
                            .addGroup(jPanel1Layout.createSequentialGroup()
                                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                                    .addComponent(jLabel1)
                                    .addComponent(jLabel2))
                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                                    .addComponent(dtdDirField, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, 371, Short.MAX_VALUE)
                                    .addComponent(baseDirField, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, 371, Short.MAX_VALUE))
                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING, false)
                                    .addComponent(jButton2, 0, 0, Short.MAX_VALUE)
                                    .addComponent(jButton1, javax.swing.GroupLayout.PREFERRED_SIZE, 29, Short.MAX_VALUE)))
                            .addComponent(jSeparator1, javax.swing.GroupLayout.DEFAULT_SIZE, 537, Short.MAX_VALUE)
                            .addComponent(jSeparator2, javax.swing.GroupLayout.DEFAULT_SIZE, 537, Short.MAX_VALUE)))
                    .addGroup(jPanel1Layout.createSequentialGroup()
                        .addGap(52, 52, 52)
                        .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                            .addGroup(jPanel1Layout.createSequentialGroup()
                                .addComponent(jLabel9)
                                .addGap(16, 16, 16)
                                .addComponent(sequenceDirField))
                            .addGroup(jPanel1Layout.createSequentialGroup()
                                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                                    .addComponent(jLabel5)
                                    .addComponent(jLabel4)
                                    .addComponent(jLabel3))
                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                                    .addComponent(timingDirField, javax.swing.GroupLayout.PREFERRED_SIZE, 166, javax.swing.GroupLayout.PREFERRED_SIZE)
                                    .addComponent(dataDirField, javax.swing.GroupLayout.PREFERRED_SIZE, 166, javax.swing.GroupLayout.PREFERRED_SIZE)
                                    .addComponent(experimentDirField, javax.swing.GroupLayout.PREFERRED_SIZE, 166, javax.swing.GroupLayout.PREFERRED_SIZE))))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                        .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(jLabel7)
                            .addComponent(jLabel10)
                            .addComponent(jLabel6)
                            .addComponent(jLabel8))))
                .addContainerGap())
        );
        jPanel1Layout.setVerticalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel1Layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jLabel1)
                    .addComponent(baseDirField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jButton1))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jLabel2)
                    .addComponent(dtdDirField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jButton2))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jSeparator1, javax.swing.GroupLayout.PREFERRED_SIZE, 6, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(jCheckBox1)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(jSeparator2, javax.swing.GroupLayout.PREFERRED_SIZE, 10, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(18, 18, 18)
                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jLabel3)
                    .addComponent(timingDirField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jLabel6))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jLabel4)
                    .addComponent(dataDirField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jLabel10))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jLabel5)
                    .addComponent(experimentDirField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jLabel7))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jLabel9)
                    .addComponent(sequenceDirField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jLabel8))
                .addContainerGap())
        );

        promptForSeqCheck.setText("Prompt for description on \"Play\"");
        promptForSeqCheck.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                promptForSeqCheckActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(layout.createSequentialGroup()
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                            .addComponent(jScrollPane1, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, 569, Short.MAX_VALUE)
                            .addComponent(jPanel1, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                        .addContainerGap())
                    .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                        .addComponent(promptForSeqCheck)
                        .addGap(43, 43, 43))))
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addGap(36, 36, 36)
                .addComponent(jScrollPane1, javax.swing.GroupLayout.PREFERRED_SIZE, 109, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(promptForSeqCheck)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jPanel1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(48, Short.MAX_VALUE))
        );
    }// </editor-fold>//GEN-END:initComponents

    private void jCheckBox1ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jCheckBox1ActionPerformed
        // TODO add your handling code here:
    }//GEN-LAST:event_jCheckBox1ActionPerformed

    private void baseDirFieldActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_baseDirFieldActionPerformed

        if (docSettings != null) {
            try {
                docSettings.DocumentationBaseAbsDir(baseDirField.getText());
            } catch(Exception e) {
            }
        }
    }//GEN-LAST:event_baseDirFieldActionPerformed

    private void dtdDirFieldActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_dtdDirFieldActionPerformed
        if (docSettings != null) {
            try {
                docSettings.DTDFileAbsDir(dtdDirField.getText());
            } catch(Exception e) {
            }
        }
    }//GEN-LAST:event_dtdDirFieldActionPerformed

    private void timingDirFieldActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_timingDirFieldActionPerformed
        if (docSettings != null) {
            try {
                docSettings.TimingFilesRelDir(timingDirField.getText());
            } catch(Exception e) {
            }
        }
    }//GEN-LAST:event_timingDirFieldActionPerformed

    private void dataDirFieldActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_dataDirFieldActionPerformed
        if (docSettings != null) {
            try {
                docSettings.DataFilesRelDir(dataDirField.getText());
            } catch(Exception e) {
            }
        }
    }//GEN-LAST:event_dataDirFieldActionPerformed

    private void experimentDirFieldActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_experimentDirFieldActionPerformed
        if (docSettings != null) {
            try {
                docSettings.ExperimentFilesRelDir(experimentDirField.getText());
            } catch(Exception e) {
            }
        }
    }//GEN-LAST:event_experimentDirFieldActionPerformed

    private void sequenceDirFieldActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_sequenceDirFieldActionPerformed
        if (docSettings != null) {
            try {
                docSettings.SequenceFilesRelDir(sequenceDirField.getText());
            } catch(Exception e) {
            }
        }
    }//GEN-LAST:event_sequenceDirFieldActionPerformed

    private void promptForSeqCheckActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_promptForSeqCheckActionPerformed
        //showDialogForSeqDescriptionBeforePlay();
    }//GEN-LAST:event_promptForSeqCheckActionPerformed

    private void seqDescDialogCancelButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_seqDescDialogCancelButtonActionPerformed
        playAfterEnterDescription = false;
        seqDecriptionDialog.setVisible(false);
    }//GEN-LAST:event_seqDescDialogCancelButtonActionPerformed

    private void seqDescDialogPlayButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_seqDescDialogPlayButtonActionPerformed
        sequenceDescription.setText(seqDescDialogTextArea.getText());
        playAfterEnterDescription = true;
        seqDecriptionDialog.setVisible(false);
    }//GEN-LAST:event_seqDescDialogPlayButtonActionPerformed


    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JTextField baseDirField;
    private javax.swing.JTextField dataDirField;
    private javax.swing.JTextField dtdDirField;
    private javax.swing.JTextField experimentDirField;
    private javax.swing.JButton jButton1;
    private javax.swing.JButton jButton2;
    private javax.swing.JCheckBox jCheckBox1;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jLabel10;
    private javax.swing.JLabel jLabel2;
    private javax.swing.JLabel jLabel3;
    private javax.swing.JLabel jLabel4;
    private javax.swing.JLabel jLabel5;
    private javax.swing.JLabel jLabel6;
    private javax.swing.JLabel jLabel7;
    private javax.swing.JLabel jLabel8;
    private javax.swing.JLabel jLabel9;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JScrollPane jScrollPane2;
    private javax.swing.JSeparator jSeparator1;
    private javax.swing.JSeparator jSeparator2;
    private javax.swing.JCheckBox promptForSeqCheck;
    private javax.swing.JDialog seqDecriptionDialog;
    private javax.swing.JButton seqDescDialogCancelButton;
    private javax.swing.JButton seqDescDialogPlayButton;
    private javax.swing.JTextArea seqDescDialogTextArea;
    private javax.swing.JTextArea sequenceDescription;
    private javax.swing.JTextField sequenceDirField;
    private javax.swing.JTextField timingDirField;
    // End of variables declaration//GEN-END:variables

}
