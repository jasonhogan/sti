/*
 * sti_console.java
 *
 * Created on November 13, 2008, 3:43 PM
 */

package edu.stanford.atom.sti.client.gui;

import edu.stanford.atom.sti.client.gui.state.*;
import edu.stanford.atom.sti.client.gui.state.STIStateMachine.State;
import edu.stanford.atom.sti.client.comm.io.STIServerConnection;

import javax.swing.*;
import edu.stanford.atom.sti.client.comm.corba.*;
import org.omg.CosNaming.*;
import org.omg.CosNaming.NamingContextPackage.*;
import org.omg.CORBA.*;
import edu.stanford.atom.sti.client.comm.bl.DataManager;
import java.lang.Thread;


/**
 *
 * @author  Owner
 */

public class sti_console extends javax.swing.JFrame implements STIStateListener {
    
    private String playButtonDisabledToolTip = "Play (A file must be parsed before it can be played.)";
    
    public enum consoleStatus {Connected, NotConnected, Parsing, Running};
    
    private DataManager dataManager = new DataManager();
    private STIStateMachine stateMachine = new STIStateMachine();
    private STIServerConnection serverConnection = new STIServerConnection(stateMachine);
    private Thread connectionThread = null;
    private Thread parseThread = null;
    private Thread playThread = null;
        
    public sti_console() {
        initComponents();
        
        stateMachine.addStateListener(this);
        dataManager.addDataListener(eventsTab1);
        dataManager.addDataListener(variableTab1);
    }

    public void updateState(STIStateEvent event) {

        switch( event.state() ) {
            case Disconnected:
                connectButton.setText("Connect");
                parseButton.setEnabled(false);
                jProgressBar1.setIndeterminate(false);
                jProgressBar1.setValue(0);
                statusTextField.setText("Not connected to server");
                serverAddressTextField.setEditable(true);
                playButton.setEnabled(false);
                pauseButton.setEnabled(false);
                stopButton.setEnabled(false);
                playButton.setToolTipText(playButtonDisabledToolTip);
//                serverAddressTextField.setText(serverAddress);
//                connectionThread.interrupt();
                serverConnection.disconnectFromServer();
                break;
            case Connecting:
                connectButton.setText("Disconnect");
                parseButton.setEnabled(false);
                jProgressBar1.setIndeterminate(true);
                statusTextField.setText("Connecting...");
                serverAddressTextField.setEditable(false);
                serverConnection.setServerAddress(serverAddressTextField.getText());
                playButton.setEnabled(false);
                pauseButton.setEnabled(false);
                stopButton.setEnabled(true);
                playButton.setToolTipText(playButtonDisabledToolTip);
                
                connectionThread = new Thread(serverConnection);
                connectionThread.start();
                break;
            case IdleUnparsed:
                connectButton.setText("Disconnect");
                parseButton.setEnabled(true);
                jProgressBar1.setIndeterminate(false);
                jProgressBar1.setValue(0);
                statusTextField.setText("Ready");
                serverAddressTextField.setEditable(false);
                serverAddressTextField.setText(serverConnection.getServerAddress());
                playButton.setEnabled(false);
                pauseButton.setEnabled(false);
                stopButton.setEnabled(false);
                playButton.setToolTipText(playButtonDisabledToolTip);
                
                attachServants();
                break;
            case Parsing:
                connectButton.setText("Disconnect");
                parseButton.setEnabled(false);
                jProgressBar1.setIndeterminate(true);
                statusTextField.setText("Parsing...");
                serverAddressTextField.setEditable(false);
                serverAddressTextField.setText(serverConnection.getServerAddress());
                playButton.setEnabled(false);
                pauseButton.setEnabled(false);
                stopButton.setEnabled(true);
                playButton.setToolTipText(playButtonDisabledToolTip);
                
                parseThread = new Thread(new Runnable() {

                    public void run() {
                        boolean success = tabbedEditor1.parseFile();
                        stateMachine.finishParsing(success);
                        dataManager.getParsedData();
                    }
                });
                parseThread.start();
                break;
            case IdleParsed:
                connectButton.setText("Disconnect");
                parseButton.setEnabled(true);
                jProgressBar1.setIndeterminate(false);
                jProgressBar1.setValue(0);
                statusTextField.setText("Ready");
                serverAddressTextField.setEditable(false);
                serverAddressTextField.setText(serverConnection.getServerAddress());
                playButton.setEnabled(true);
                pauseButton.setEnabled(false);
                stopButton.setEnabled(false);
                playButton.setToolTipText("Play");
                break;
            case Running:
                connectButton.setText("Disconnect");
                parseButton.setEnabled(false);
                jProgressBar1.setIndeterminate(false);
                jProgressBar1.setValue(0);
                statusTextField.setText("Running...");
                serverAddressTextField.setEditable(false);
                serverAddressTextField.setText(serverConnection.getServerAddress());
                playButton.setEnabled(false);
                pauseButton.setEnabled(true);
                stopButton.setEnabled(true);
                playButton.setToolTipText("Play");
                
                playThread = new Thread(new Runnable() {

                    public void run() {
                        serverConnection.getControl().runSingle();
                        stateMachine.finishRunning();
                    }
                });
                playThread.start();
                break;
            case Paused:
                connectButton.setText("Disconnect");
                parseButton.setEnabled(false);
                jProgressBar1.setIndeterminate(false);
                jProgressBar1.setValue(0);
                statusTextField.setText("Paused");
                serverAddressTextField.setEditable(false);
                serverAddressTextField.setText(serverConnection.getServerAddress());
                playButton.setEnabled(true);
                pauseButton.setEnabled(false);
                stopButton.setEnabled(true);
                playButton.setToolTipText("Play");

                break;
            default:
                break;
        }
    }
    
    
    private void attachServants() {

        dataManager.setParser(serverConnection.getParser());

        sTIDeviceManager1.setDeviceConfigure(serverConnection.getDeviceConfigure());

        runTab1.setExpSequence(serverConnection.getExpSequence());

        runTab1.setParser(serverConnection.getParser());
        tabbedEditor1.setParser(serverConnection.getParser());
    //    eventsTab1.setParser(parser);

    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        buttonGroup1 = new javax.swing.ButtonGroup();
        buttonGroup2 = new javax.swing.ButtonGroup();
        jSplitPane2 = new javax.swing.JSplitPane();
        jSplitPane1 = new javax.swing.JSplitPane();
        plugInManager = new edu.stanford.atom.sti.client.gui.PlugInManager();
        plugInTab3 = new edu.stanford.atom.sti.client.gui.PlugInTab("Editor");
        tabbedEditor1 = new edu.stanford.atom.sti.client.gui.FileEditorTab.TabbedEditor();
        plugInTab4 = new edu.stanford.atom.sti.client.gui.PlugInTab("Variables");
        variableTab1 = new edu.stanford.atom.sti.client.gui.VariablesTab.VariableTab();
        plugInTab5 = new edu.stanford.atom.sti.client.gui.PlugInTab("Events");
        eventsTab1 = new edu.stanford.atom.sti.client.gui.EventsTab.EventsTab();
        plugInTab1 = new edu.stanford.atom.sti.client.gui.PlugInTab("Devices", "Devices");
        sTIDeviceManager1 = new edu.stanford.atom.sti.client.gui.DeviceManager.STIDeviceManager();
        plugInTab2 = new edu.stanford.atom.sti.client.gui.PlugInTab("Run", "Run");
        runTab1 = new edu.stanford.atom.sti.client.gui.RunTab.RunTab();
        jPanel1 = new javax.swing.JPanel();
        jToolBar1 = new javax.swing.JToolBar();
        jSplitPane3 = new javax.swing.JSplitPane();
        jSplitPane5 = new javax.swing.JSplitPane();
        jPanel3 = new javax.swing.JPanel();
        jRadioButton2 = new javax.swing.JRadioButton();
        jRadioButton1 = new javax.swing.JRadioButton();
        jSeparator7 = new javax.swing.JSeparator();
        jPanel5 = new javax.swing.JPanel();
        playButton = new javax.swing.JButton();
        pauseButton = new javax.swing.JButton();
        stopButton = new javax.swing.JButton();
        jPanel7 = new javax.swing.JPanel();
        parseButton = new javax.swing.JButton();
        jComboBox2 = new javax.swing.JComboBox();
        jLabel3 = new javax.swing.JLabel();
        jSplitPane4 = new javax.swing.JSplitPane();
        jPanel4 = new javax.swing.JPanel();
        jComboBox1 = new javax.swing.JComboBox();
        jLabel1 = new javax.swing.JLabel();
        jSeparator1 = new javax.swing.JSeparator();
        jSeparator3 = new javax.swing.JSeparator();
        jPanel6 = new javax.swing.JPanel();
        jLabel2 = new javax.swing.JLabel();
        serverAddressTextField = new javax.swing.JTextField();
        connectButton = new javax.swing.JButton();
        jPanel2 = new javax.swing.JPanel();
        statusTextField = new javax.swing.JTextField();
        jProgressBar1 = new javax.swing.JProgressBar();
        jMenuBar2 = new javax.swing.JMenuBar();
        fileMenu = new javax.swing.JMenu();
        newMenuItem = new javax.swing.JMenuItem();
        jSeparator4 = new javax.swing.JSeparator();
        openMenuItem = new javax.swing.JMenuItem();
        openLocalMenuItem = new javax.swing.JMenuItem();
        closeMenuItem = new javax.swing.JMenuItem();
        jSeparator5 = new javax.swing.JSeparator();
        saveMenuItem = new javax.swing.JMenuItem();
        saveAsMenuItem = new javax.swing.JMenuItem();
        saveAsLocalMenuItem = new javax.swing.JMenuItem();
        saveAllMenuItem1 = new javax.swing.JMenuItem();
        jSeparator6 = new javax.swing.JSeparator();
        exitMenuItem = new javax.swing.JMenuItem();
        jMenu5 = new javax.swing.JMenu();
        jMenu6 = new javax.swing.JMenu();
        jRadioButtonMenuItem1 = new javax.swing.JRadioButtonMenuItem();
        jRadioButtonMenuItem2 = new javax.swing.JRadioButtonMenuItem();
        jRadioButtonMenuItem3 = new javax.swing.JRadioButtonMenuItem();
        jRadioButtonMenuItem4 = new javax.swing.JRadioButtonMenuItem();
        jMenu7 = new javax.swing.JMenu();
        jMenuItem1 = new javax.swing.JMenuItem();
        jSeparator2 = new javax.swing.JSeparator();
        jMenuItem2 = new javax.swing.JMenuItem();

        setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);
        setTitle("STI Console");

        jSplitPane2.setDividerLocation(725);
        jSplitPane2.setDividerSize(2);
        jSplitPane2.setOrientation(javax.swing.JSplitPane.VERTICAL_SPLIT);
        jSplitPane2.setResizeWeight(1.0);
        jSplitPane2.setMinimumSize(new java.awt.Dimension(102, 30));
        jSplitPane2.setPreferredSize(new java.awt.Dimension(102, 780));

        jSplitPane1.setDividerLocation(50);
        jSplitPane1.setDividerSize(3);
        jSplitPane1.setOrientation(javax.swing.JSplitPane.VERTICAL_SPLIT);
        jSplitPane1.setMinimumSize(new java.awt.Dimension(91, 0));

        plugInManager.setMinimumSize(new java.awt.Dimension(626, 600));
        plugInManager.setPreferredSize(new java.awt.Dimension(100, 600));

        plugInTab3.setRollover(true);
        plugInTab3.add(tabbedEditor1);

        plugInManager.addTab("Editor", plugInTab3);

        plugInTab4.setRollover(true);
        plugInTab4.add(variableTab1);

        plugInManager.addTab("Variables", plugInTab4);

        plugInTab5.setRollover(true);
        plugInTab5.add(eventsTab1);

        plugInManager.addTab("Events", plugInTab5);

        plugInTab1.setRollover(true);
        plugInTab1.setMinimumSize(new java.awt.Dimension(500, 570));

        sTIDeviceManager1.setMinimumSize(new java.awt.Dimension(500, 500));
        plugInTab1.add(sTIDeviceManager1);

        plugInManager.addTab("Devices", plugInTab1);

        plugInTab2.setRollover(true);
        plugInTab2.add(runTab1);

        plugInManager.addTab("Run", plugInTab2);

        plugInManager.setSelectedIndex(0);

        jSplitPane1.setRightComponent(plugInManager);

        jToolBar1.setRollover(true);
        jToolBar1.setMaximumSize(new java.awt.Dimension(800, 50));
        jToolBar1.setMinimumSize(new java.awt.Dimension(800, 50));
        jToolBar1.setPreferredSize(new java.awt.Dimension(800, 50));

        jSplitPane3.setBorder(null);
        jSplitPane3.setDividerSize(0);

        jSplitPane5.setBorder(null);
        jSplitPane5.setDividerSize(0);

        jPanel3.setMaximumSize(new java.awt.Dimension(107, 48));
        jPanel3.setMinimumSize(new java.awt.Dimension(107, 48));
        jPanel3.setPreferredSize(new java.awt.Dimension(107, 48));

        buttonGroup1.add(jRadioButton2);
        jRadioButton2.setText("Single Run");
        jRadioButton2.setFocusable(false);
        jRadioButton2.setHorizontalTextPosition(javax.swing.SwingConstants.RIGHT);
        jRadioButton2.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        jRadioButton2.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jRadioButton2ActionPerformed(evt);
            }
        });

        buttonGroup1.add(jRadioButton1);
        jRadioButton1.setSelected(true);
        jRadioButton1.setText("Sequence Run");
        jRadioButton1.setFocusable(false);
        jRadioButton1.setHorizontalTextPosition(javax.swing.SwingConstants.RIGHT);
        jRadioButton1.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);

        jSeparator7.setOrientation(javax.swing.SwingConstants.VERTICAL);
        jSeparator7.setMaximumSize(new java.awt.Dimension(10, 48));
        jSeparator7.setMinimumSize(new java.awt.Dimension(10, 48));
        jSeparator7.setPreferredSize(new java.awt.Dimension(10, 48));

        javax.swing.GroupLayout jPanel3Layout = new javax.swing.GroupLayout(jPanel3);
        jPanel3.setLayout(jPanel3Layout);
        jPanel3Layout.setHorizontalGroup(
            jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel3Layout.createSequentialGroup()
                .addGroup(jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(jRadioButton1)
                    .addComponent(jRadioButton2))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jSeparator7, javax.swing.GroupLayout.PREFERRED_SIZE, 10, javax.swing.GroupLayout.PREFERRED_SIZE))
        );
        jPanel3Layout.setVerticalGroup(
            jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel3Layout.createSequentialGroup()
                .addComponent(jRadioButton2)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jRadioButton1)
                .addGap(2, 2, 2))
            .addGroup(jPanel3Layout.createSequentialGroup()
                .addComponent(jSeparator7, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addContainerGap())
        );

        jSplitPane5.setRightComponent(jPanel3);

        playButton.setIcon(new javax.swing.ImageIcon(getClass().getResource("/toolbarButtonGraphics/media/Play16.gif"))); // NOI18N
        playButton.setToolTipText("Play");
        playButton.setFocusable(false);
        playButton.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        playButton.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        playButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                playButtonActionPerformed(evt);
            }
        });

        pauseButton.setIcon(new javax.swing.ImageIcon(getClass().getResource("/toolbarButtonGraphics/media/Pause16.gif"))); // NOI18N
        pauseButton.setToolTipText("Pause");
        pauseButton.setFocusable(false);
        pauseButton.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        pauseButton.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);

        stopButton.setIcon(new javax.swing.ImageIcon(getClass().getResource("/toolbarButtonGraphics/media/Stop16.gif"))); // NOI18N
        stopButton.setToolTipText("Stop");
        stopButton.setFocusable(false);
        stopButton.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        stopButton.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);

        javax.swing.GroupLayout jPanel5Layout = new javax.swing.GroupLayout(jPanel5);
        jPanel5.setLayout(jPanel5Layout);
        jPanel5Layout.setHorizontalGroup(
            jPanel5Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, jPanel5Layout.createSequentialGroup()
                .addComponent(playButton, javax.swing.GroupLayout.PREFERRED_SIZE, 30, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(pauseButton, javax.swing.GroupLayout.PREFERRED_SIZE, 30, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(stopButton, javax.swing.GroupLayout.PREFERRED_SIZE, 30, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );
        jPanel5Layout.setVerticalGroup(
            jPanel5Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, jPanel5Layout.createSequentialGroup()
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addGroup(jPanel5Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(stopButton, javax.swing.GroupLayout.PREFERRED_SIZE, 30, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(pauseButton, javax.swing.GroupLayout.PREFERRED_SIZE, 30, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(playButton, javax.swing.GroupLayout.PREFERRED_SIZE, 30, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addContainerGap())
        );

        jSplitPane5.setLeftComponent(jPanel5);

        jSplitPane3.setLeftComponent(jSplitPane5);

        jPanel7.setMaximumSize(new java.awt.Dimension(180, 48));
        jPanel7.setMinimumSize(new java.awt.Dimension(180, 48));
        jPanel7.setPreferredSize(new java.awt.Dimension(180, 48));

        parseButton.setText("Parse");
        parseButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                parseButtonActionPerformed(evt);
            }
        });

        jLabel3.setText("Main File:");

        javax.swing.GroupLayout jPanel7Layout = new javax.swing.GroupLayout(jPanel7);
        jPanel7.setLayout(jPanel7Layout);
        jPanel7Layout.setHorizontalGroup(
            jPanel7Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel7Layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(jPanel7Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                    .addGroup(jPanel7Layout.createSequentialGroup()
                        .addComponent(jLabel3)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                        .addComponent(parseButton, javax.swing.GroupLayout.PREFERRED_SIZE, 74, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addComponent(jComboBox2, javax.swing.GroupLayout.PREFERRED_SIZE, 152, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addContainerGap(18, Short.MAX_VALUE))
        );
        jPanel7Layout.setVerticalGroup(
            jPanel7Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel7Layout.createSequentialGroup()
                .addGroup(jPanel7Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jLabel3, javax.swing.GroupLayout.PREFERRED_SIZE, 19, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(parseButton, javax.swing.GroupLayout.PREFERRED_SIZE, 24, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addGap(2, 2, 2)
                .addComponent(jComboBox2, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap())
        );

        jSplitPane3.setRightComponent(jPanel7);

        jToolBar1.add(jSplitPane3);

        jSplitPane4.setBorder(null);
        jSplitPane4.setDividerSize(0);

        jComboBox1.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "Direct", "Documented", "Testing", "Monitor" }));
        jComboBox1.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jComboBox1ActionPerformed(evt);
            }
        });

        jLabel1.setText("Mode:");

        jSeparator1.setOrientation(javax.swing.SwingConstants.VERTICAL);

        jSeparator3.setOrientation(javax.swing.SwingConstants.VERTICAL);

        javax.swing.GroupLayout jPanel4Layout = new javax.swing.GroupLayout(jPanel4);
        jPanel4.setLayout(jPanel4Layout);
        jPanel4Layout.setHorizontalGroup(
            jPanel4Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel4Layout.createSequentialGroup()
                .addComponent(jSeparator1, javax.swing.GroupLayout.PREFERRED_SIZE, 8, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(jPanel4Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(jLabel1)
                    .addComponent(jComboBox1, javax.swing.GroupLayout.PREFERRED_SIZE, 90, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jSeparator3, javax.swing.GroupLayout.DEFAULT_SIZE, 13, Short.MAX_VALUE))
        );
        jPanel4Layout.setVerticalGroup(
            jPanel4Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel4Layout.createSequentialGroup()
                .addGap(6, 6, 6)
                .addComponent(jLabel1, javax.swing.GroupLayout.PREFERRED_SIZE, 14, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(2, 2, 2)
                .addComponent(jComboBox1, javax.swing.GroupLayout.PREFERRED_SIZE, 18, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap())
            .addComponent(jSeparator1, javax.swing.GroupLayout.DEFAULT_SIZE, 51, Short.MAX_VALUE)
            .addComponent(jSeparator3, javax.swing.GroupLayout.DEFAULT_SIZE, 51, Short.MAX_VALUE)
        );

        jSplitPane4.setLeftComponent(jPanel4);

        jLabel2.setText("Server Address: ");

        serverAddressTextField.setText("localhost:2809");
        serverAddressTextField.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                serverAddressTextFieldActionPerformed(evt);
            }
        });

        connectButton.setText("Connect");
        connectButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                connectButtonActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout jPanel6Layout = new javax.swing.GroupLayout(jPanel6);
        jPanel6.setLayout(jPanel6Layout);
        jPanel6Layout.setHorizontalGroup(
            jPanel6Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel6Layout.createSequentialGroup()
                .addGap(2, 2, 2)
                .addComponent(connectButton)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(jPanel6Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(jLabel2, javax.swing.GroupLayout.PREFERRED_SIZE, 97, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(serverAddressTextField, javax.swing.GroupLayout.PREFERRED_SIZE, 171, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addContainerGap(25, Short.MAX_VALUE))
        );
        jPanel6Layout.setVerticalGroup(
            jPanel6Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel6Layout.createSequentialGroup()
                .addGroup(jPanel6Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(jPanel6Layout.createSequentialGroup()
                        .addComponent(jLabel2)
                        .addGap(24, 24, 24))
                    .addGroup(jPanel6Layout.createSequentialGroup()
                        .addGap(17, 17, 17)
                        .addGroup(jPanel6Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                            .addComponent(serverAddressTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                            .addComponent(connectButton, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))))
                .addContainerGap())
        );

        jSplitPane4.setRightComponent(jPanel6);

        jToolBar1.add(jSplitPane4);

        javax.swing.GroupLayout jPanel1Layout = new javax.swing.GroupLayout(jPanel1);
        jPanel1.setLayout(jPanel1Layout);
        jPanel1Layout.setHorizontalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jToolBar1, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
        );
        jPanel1Layout.setVerticalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jToolBar1, javax.swing.GroupLayout.DEFAULT_SIZE, 50, Short.MAX_VALUE)
        );

        jSplitPane1.setTopComponent(jPanel1);

        jSplitPane2.setLeftComponent(jSplitPane1);

        jPanel2.setMaximumSize(new java.awt.Dimension(32767, 20));
        jPanel2.setMinimumSize(new java.awt.Dimension(100, 20));
        jPanel2.setPreferredSize(new java.awt.Dimension(100, 20));

        statusTextField.setBackground(new java.awt.Color(236, 233, 216));
        statusTextField.setEditable(false);
        statusTextField.setText("Not connected to server");
        statusTextField.setBorder(null);

        javax.swing.GroupLayout jPanel2Layout = new javax.swing.GroupLayout(jPanel2);
        jPanel2.setLayout(jPanel2Layout);
        jPanel2Layout.setHorizontalGroup(
            jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, jPanel2Layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(statusTextField, javax.swing.GroupLayout.PREFERRED_SIZE, 266, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 258, Short.MAX_VALUE)
                .addComponent(jProgressBar1, javax.swing.GroupLayout.PREFERRED_SIZE, 264, javax.swing.GroupLayout.PREFERRED_SIZE))
        );
        jPanel2Layout.setVerticalGroup(
            jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel2Layout.createSequentialGroup()
                .addGroup(jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING, false)
                    .addComponent(statusTextField, javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(jProgressBar1, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                .addContainerGap())
        );

        jSplitPane2.setBottomComponent(jPanel2);

        fileMenu.setText("File");

        newMenuItem.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_N, java.awt.event.InputEvent.CTRL_MASK));
        newMenuItem.setText("New File");
        newMenuItem.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                newMenuItemActionPerformed(evt);
            }
        });
        fileMenu.add(newMenuItem);
        fileMenu.add(jSeparator4);

        openMenuItem.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_O, java.awt.event.InputEvent.CTRL_MASK));
        openMenuItem.setText("Open File...");
        openMenuItem.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                openMenuItemActionPerformed(evt);
            }
        });
        fileMenu.add(openMenuItem);

        openLocalMenuItem.setText("Open Local file...");
        openLocalMenuItem.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                openLocalMenuItemActionPerformed(evt);
            }
        });
        fileMenu.add(openLocalMenuItem);

        closeMenuItem.setText("Close File");
        closeMenuItem.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                closeMenuItemActionPerformed(evt);
            }
        });
        fileMenu.add(closeMenuItem);
        fileMenu.add(jSeparator5);

        saveMenuItem.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_S, java.awt.event.InputEvent.CTRL_MASK));
        saveMenuItem.setText("Save");
        saveMenuItem.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                saveMenuItemActionPerformed(evt);
            }
        });
        fileMenu.add(saveMenuItem);

        saveAsMenuItem.setText("Save As...");
        saveAsMenuItem.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                saveAsMenuItemActionPerformed(evt);
            }
        });
        fileMenu.add(saveAsMenuItem);

        saveAsLocalMenuItem.setText("Save As Local...");
        saveAsLocalMenuItem.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                saveAsLocalMenuItemActionPerformed(evt);
            }
        });
        fileMenu.add(saveAsLocalMenuItem);

        saveAllMenuItem1.setText("Save All");
        fileMenu.add(saveAllMenuItem1);
        fileMenu.add(jSeparator6);

        exitMenuItem.setText("Exit");
        fileMenu.add(exitMenuItem);

        jMenuBar2.add(fileMenu);

        jMenu5.setText("Edit");
        jMenuBar2.add(jMenu5);

        jMenu6.setText("Mode");

        buttonGroup2.add(jRadioButtonMenuItem1);
        jRadioButtonMenuItem1.setText("Direct");
        jMenu6.add(jRadioButtonMenuItem1);

        buttonGroup2.add(jRadioButtonMenuItem2);
        jRadioButtonMenuItem2.setText("Documented");
        jMenu6.add(jRadioButtonMenuItem2);

        buttonGroup2.add(jRadioButtonMenuItem3);
        jRadioButtonMenuItem3.setText("Testing");
        jMenu6.add(jRadioButtonMenuItem3);

        buttonGroup2.add(jRadioButtonMenuItem4);
        jRadioButtonMenuItem4.setSelected(true);
        jRadioButtonMenuItem4.setText("Monitor");
        jMenu6.add(jRadioButtonMenuItem4);

        jMenuBar2.add(jMenu6);

        jMenu7.setText("Help");

        jMenuItem1.setText("About");
        jMenu7.add(jMenuItem1);
        jMenu7.add(jSeparator2);

        jMenuItem2.setText("License");
        jMenu7.add(jMenuItem2);

        jMenuBar2.add(jMenu7);

        setJMenuBar(jMenuBar2);

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jSplitPane2, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 800, Short.MAX_VALUE)
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jSplitPane2, javax.swing.GroupLayout.DEFAULT_SIZE, 749, Short.MAX_VALUE)
        );

        pack();
    }// </editor-fold>//GEN-END:initComponents

    private void playButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_playButtonActionPerformed
        stateMachine.play();
}//GEN-LAST:event_playButtonActionPerformed

    private void jRadioButton2ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jRadioButton2ActionPerformed
        // TODO add your handling code here:
    }//GEN-LAST:event_jRadioButton2ActionPerformed

    private void connectButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_connectButtonActionPerformed
        
        if(stateMachine.getState() == State.Disconnected) {
            stateMachine.connect();
        } 
        else {
            //show disconnection warning dialog
            java.lang.Object[] options = {"Disconnect", "Cancel"};
            int fileOpenDialogResult = JOptionPane.showOptionDialog(this,
                    "Are you sure you want to disconnect from\n " +
                    "the STI server at " + serverConnection.getServerAddress() + " ?",
                    "Disconnect from server",
                    JOptionPane.OK_CANCEL_OPTION,
                    JOptionPane.WARNING_MESSAGE,
                    null,
                    options,
                    options[1]);
            switch (fileOpenDialogResult) {
                case JOptionPane.OK_OPTION:
                    //"Yes" -- Disconnect
                    stateMachine.disconnect();
                    break;
                case JOptionPane.CANCEL_OPTION:
                    //"Cancel"
                    break;
                }
        }

}//GEN-LAST:event_connectButtonActionPerformed

    private void serverAddressTextFieldActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_serverAddressTextFieldActionPerformed
        // TODO add your handling code here:
        connectButtonActionPerformed(evt);
}//GEN-LAST:event_serverAddressTextFieldActionPerformed

    private void newMenuItemActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_newMenuItemActionPerformed
        tabbedEditor1.createNewFile();
    }//GEN-LAST:event_newMenuItemActionPerformed

    private void saveAsMenuItemActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_saveAsMenuItemActionPerformed
        tabbedEditor1.saveAsNetworkActiveTab();
    }//GEN-LAST:event_saveAsMenuItemActionPerformed

    private void saveAsLocalMenuItemActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_saveAsLocalMenuItemActionPerformed
        tabbedEditor1.saveAsLocalActiveTab();
    }//GEN-LAST:event_saveAsLocalMenuItemActionPerformed

    private void saveMenuItemActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_saveMenuItemActionPerformed
        tabbedEditor1.saveActiveTab();
    }//GEN-LAST:event_saveMenuItemActionPerformed

    private void openMenuItemActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_openMenuItemActionPerformed
        tabbedEditor1.openNetworkFile();
    }//GEN-LAST:event_openMenuItemActionPerformed

    private void openLocalMenuItemActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_openLocalMenuItemActionPerformed
        tabbedEditor1.openLocalFile();
    }//GEN-LAST:event_openLocalMenuItemActionPerformed

    private void closeMenuItemActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_closeMenuItemActionPerformed
        tabbedEditor1.closeFileInActiveTab();
    }//GEN-LAST:event_closeMenuItemActionPerformed

    private void jComboBox1ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jComboBox1ActionPerformed
        // TODO add your handling code here:
    }//GEN-LAST:event_jComboBox1ActionPerformed

    private void parseButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_parseButtonActionPerformed

        stateMachine.parse();
}//GEN-LAST:event_parseButtonActionPerformed
    
    
    /**
     * @param args the command line arguments
     */
    public static void main(String args[]) {
        try {
            UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
        } catch (Exception unused) {
            unused.printStackTrace(); // Ignore exception because we can't do anything.  Will use default.
        }

        java.awt.EventQueue.invokeLater(new Runnable() {

            public void run() {
                new sti_console().setVisible(true);
            }
        });
    }
    
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.ButtonGroup buttonGroup1;
    private javax.swing.ButtonGroup buttonGroup2;
    private javax.swing.JMenuItem closeMenuItem;
    private javax.swing.JButton connectButton;
    private edu.stanford.atom.sti.client.gui.EventsTab.EventsTab eventsTab1;
    private javax.swing.JMenuItem exitMenuItem;
    private javax.swing.JMenu fileMenu;
    private javax.swing.JComboBox jComboBox1;
    private javax.swing.JComboBox jComboBox2;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jLabel2;
    private javax.swing.JLabel jLabel3;
    private javax.swing.JMenu jMenu5;
    private javax.swing.JMenu jMenu6;
    private javax.swing.JMenu jMenu7;
    private javax.swing.JMenuBar jMenuBar2;
    private javax.swing.JMenuItem jMenuItem1;
    private javax.swing.JMenuItem jMenuItem2;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JPanel jPanel2;
    private javax.swing.JPanel jPanel3;
    private javax.swing.JPanel jPanel4;
    private javax.swing.JPanel jPanel5;
    private javax.swing.JPanel jPanel6;
    private javax.swing.JPanel jPanel7;
    private javax.swing.JProgressBar jProgressBar1;
    private javax.swing.JRadioButton jRadioButton1;
    private javax.swing.JRadioButton jRadioButton2;
    private javax.swing.JRadioButtonMenuItem jRadioButtonMenuItem1;
    private javax.swing.JRadioButtonMenuItem jRadioButtonMenuItem2;
    private javax.swing.JRadioButtonMenuItem jRadioButtonMenuItem3;
    private javax.swing.JRadioButtonMenuItem jRadioButtonMenuItem4;
    private javax.swing.JSeparator jSeparator1;
    private javax.swing.JSeparator jSeparator2;
    private javax.swing.JSeparator jSeparator3;
    private javax.swing.JSeparator jSeparator4;
    private javax.swing.JSeparator jSeparator5;
    private javax.swing.JSeparator jSeparator6;
    private javax.swing.JSeparator jSeparator7;
    private javax.swing.JSplitPane jSplitPane1;
    private javax.swing.JSplitPane jSplitPane2;
    private javax.swing.JSplitPane jSplitPane3;
    private javax.swing.JSplitPane jSplitPane4;
    private javax.swing.JSplitPane jSplitPane5;
    private javax.swing.JToolBar jToolBar1;
    private javax.swing.JMenuItem newMenuItem;
    private javax.swing.JMenuItem openLocalMenuItem;
    private javax.swing.JMenuItem openMenuItem;
    private javax.swing.JButton parseButton;
    private javax.swing.JButton pauseButton;
    private javax.swing.JButton playButton;
    private edu.stanford.atom.sti.client.gui.PlugInManager plugInManager;
    private edu.stanford.atom.sti.client.gui.PlugInTab plugInTab1;
    private edu.stanford.atom.sti.client.gui.PlugInTab plugInTab2;
    private edu.stanford.atom.sti.client.gui.PlugInTab plugInTab3;
    private edu.stanford.atom.sti.client.gui.PlugInTab plugInTab4;
    private edu.stanford.atom.sti.client.gui.PlugInTab plugInTab5;
    private edu.stanford.atom.sti.client.gui.RunTab.RunTab runTab1;
    private edu.stanford.atom.sti.client.gui.DeviceManager.STIDeviceManager sTIDeviceManager1;
    private javax.swing.JMenuItem saveAllMenuItem1;
    private javax.swing.JMenuItem saveAsLocalMenuItem;
    private javax.swing.JMenuItem saveAsMenuItem;
    private javax.swing.JMenuItem saveMenuItem;
    private javax.swing.JTextField serverAddressTextField;
    private javax.swing.JTextField statusTextField;
    private javax.swing.JButton stopButton;
    private edu.stanford.atom.sti.client.gui.FileEditorTab.TabbedEditor tabbedEditor1;
    private edu.stanford.atom.sti.client.gui.VariablesTab.VariableTab variableTab1;
    // End of variables declaration//GEN-END:variables
    
}
