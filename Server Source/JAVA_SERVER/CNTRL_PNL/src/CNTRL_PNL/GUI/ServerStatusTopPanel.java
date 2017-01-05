/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package CNTRL_PNL.GUI;

import java.awt.*;
import javax.swing.*;
import java.awt.event.*;
import javax.accessibility.*;
import java.util.*;

import CNTRL_PNL.INFO.*;

public class ServerStatusTopPanel extends JPanel
{
  private ServerStatusTabbedPane serverStatusPanel;

  GridLayout gridLayout8 = new GridLayout();
  JPanel top = new JPanel();
  JPanel jPanel8 = new JPanel();
  JTextField usersLoggedInTextField = new JTextField();
  JLabel usersLoggedInLabel = new JLabel();
  JPanel bottom = new JPanel();
  JLabel daemonsLabel = new JLabel();
  JLabel debugLabel = new JLabel();
  JLabel loginsLabel = new JLabel();
  JPanel actionPanel = new JPanel();
  JPanel jPanel9 = new JPanel();
  JPanel jPanel2 = new JPanel();
  JPanel jPanel3 = new JPanel();
  JPanel jPanel4 = new JPanel();
  JRadioButton debugOnRadioButton = new JRadioButton();
  JRadioButton stopDaemonsRadioButton = new JRadioButton();
  GridLayout gridLayout1 = new GridLayout();
  GridLayout gridLayout2 = new GridLayout();
  JRadioButton debugOffRadioButton = new JRadioButton();
  GridLayout gridLayout3 = new GridLayout();
  GridLayout w1 = new GridLayout();
  GridLayout gridLayout5 = new GridLayout();
  JRadioButton idleDaemonsRadioButton = new JRadioButton();
  JRadioButton startDaemonsRadioButton = new JRadioButton();
  JRadioButton awakenDaemonsRadioButton = new JRadioButton();
  JLabel stateLabel = new JLabel();
  BorderLayout borderLayout1 = new BorderLayout();
  JPanel paddingEast = new JPanel();
  JPanel paddingWest = new JPanel();
  GridLayout gridLayout9 = new GridLayout();
  JPanel jPanel1 = new JPanel();
  JPanel jPanel7 = new JPanel();
  GridLayout gridLayout7 = new GridLayout();
  JTextField ntcssUpTimeTextField = new JTextField();
  JTextField hostUpTimeTextField = new JTextField();
  JLabel hostUpTimeLabel = new JLabel();
  JPanel jPanel6 = new JPanel();
  GridLayout gridLayout6 = new GridLayout();
  JLabel ntcssUpTimeLabel = new JLabel();
  BorderLayout borderLayout2 = new BorderLayout();
  JRadioButton disableLoginsRadioButton = new JRadioButton();
  JRadioButton enableLoginsRadioButton = new JRadioButton();

  public ServerStatusTopPanel(ServerStatusTabbedPane ssp)
  {
    serverStatusPanel = ssp;

    try
    {
      jbInit();
    }
    catch (Exception ex)
    {
      ex.printStackTrace();
    }
  }

  void jbInit() throws Exception
  {
    usersLoggedInTextField.setMinimumSize(new Dimension(50, 19));
    usersLoggedInTextField.setHorizontalAlignment(JTextField.CENTER);
    usersLoggedInTextField.setText("NA");
    usersLoggedInTextField.setEditable(false);
    usersLoggedInLabel.setText("NTCSS Users Logged In");
    bottom.setLayout(borderLayout1);
    daemonsLabel.setText("Daemons");
    debugLabel.setText("Debug");
    loginsLabel.setText("Logins");
    actionPanel.setPreferredSize(new Dimension(1000, 66));
    actionPanel.setMinimumSize(new Dimension(1000, 66));
    jPanel9.setPreferredSize(new Dimension(150, 66));
    jPanel9.setMinimumSize(new Dimension(150, 66));
    jPanel2.setPreferredSize(new Dimension(150, 66));
    jPanel2.setMinimumSize(new Dimension(150, 66));
    jPanel3.setPreferredSize(new Dimension(150, 66));
    jPanel3.setMinimumSize(new Dimension(150, 66));
    jPanel4.setPreferredSize(new Dimension(150, 66));
    jPanel4.setMinimumSize(new Dimension(150, 66));
    debugOnRadioButton.setText("On");
    stopDaemonsRadioButton.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        stopDaemonsRadioButton_actionPerformed(e);
      }
    });
    stopDaemonsRadioButton.setText("Stop");
    gridLayout1.setRows(3);
    gridLayout2.setRows(3);
    debugOffRadioButton.setText("Off");
    gridLayout3.setRows(3);
    w1.setRows(3);
    gridLayout5.setHgap(75);
    idleDaemonsRadioButton.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        idleDaemonsRadioButton_actionPerformed(e);
      }
    });
    idleDaemonsRadioButton.setText("Idle");
    startDaemonsRadioButton.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        startDaemonsRadioButton_actionPerformed(e);
      }
    });
    startDaemonsRadioButton.setText("Start");
    awakenDaemonsRadioButton.setText("Awaken");
    stateLabel.setText("State");
    paddingEast.setPreferredSize(new Dimension(50, 10));
    paddingEast.setMinimumSize(new Dimension(50, 10));
    paddingWest.setPreferredSize(new Dimension(50, 10));
    paddingWest.setMinimumSize(new Dimension(50, 10));
    gridLayout9.setColumns(2);
    jPanel1.setLayout(borderLayout2);
    jPanel7.setLayout(gridLayout7);
    jPanel7.setMinimumSize(new Dimension(350, 43));
    gridLayout7.setRows(2);
    gridLayout7.setVgap(5);
    ntcssUpTimeTextField.setText("NA");
    ntcssUpTimeTextField.setPreferredSize(new Dimension(100, 19));
    ntcssUpTimeTextField.setHorizontalAlignment(JTextField.CENTER);
    ntcssUpTimeTextField.setEditable(false);
    hostUpTimeTextField.setText("NA");
    hostUpTimeTextField.setEditable(false);
    hostUpTimeLabel.setText("Host Up-Time");
    hostUpTimeLabel.setHorizontalAlignment(SwingConstants.LEFT);
    jPanel6.setPreferredSize(new Dimension(110, 43));
    jPanel6.setMinimumSize(new Dimension(110, 43));
    gridLayout6.setRows(2);
    ntcssUpTimeLabel.setText("NTCSS Up-Time");
    ntcssUpTimeLabel.setHorizontalAlignment(SwingConstants.LEFT);
    borderLayout2.setHgap(10);
    disableLoginsRadioButton.setText("Disable");
    enableLoginsRadioButton.setText("Enable");
    enableLoginsRadioButton.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        enableLoginsRadioButton_actionPerformed(e);
      }
    });
    disableLoginsRadioButton.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        disableLoginsRadioButton_actionPerformed(e);
      }
    });
    jPanel6.setLayout(gridLayout6);
    hostUpTimeTextField.setPreferredSize(new Dimension(50, 19));
    hostUpTimeTextField.setMinimumSize(new Dimension(20, 19));
    hostUpTimeTextField.setHorizontalAlignment(JTextField.CENTER);
    jPanel7.setPreferredSize(new Dimension(350, 43));
    awakenDaemonsRadioButton.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        awakenDaemonsRadioButton_actionPerformed(e);
      }
    });
    gridLayout5.setColumns(4);
    debugOffRadioButton.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        debugOffRadioButton_actionPerformed(e);
      }
    });
    debugOnRadioButton.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        debugOnRadioButton_actionPerformed(e);
      }
    });
    jPanel4.setLayout(w1);
    jPanel3.setLayout(gridLayout3);
    jPanel2.setLayout(gridLayout2);
    jPanel9.setLayout(gridLayout1);
    actionPanel.setLayout(gridLayout5);
    usersLoggedInTextField.setPreferredSize(new Dimension(50, 19));
    gridLayout8.setRows(2);
    gridLayout8.setVgap(5);
    top.setLayout(gridLayout9);
    this.setLayout(gridLayout8);
    this.add(top, null);
    top.add(jPanel8, null);
    jPanel8.add(usersLoggedInLabel, null);
    jPanel8.add(usersLoggedInTextField, null);
    top.add(jPanel1, null);
    jPanel1.add(jPanel6, BorderLayout.WEST);
    jPanel6.add(hostUpTimeLabel, null);
    jPanel6.add(ntcssUpTimeLabel, null);
    jPanel1.add(jPanel7, BorderLayout.CENTER);
    jPanel7.add(hostUpTimeTextField, null);
    jPanel7.add(ntcssUpTimeTextField, null);
    this.add(bottom, null);
    bottom.add(actionPanel, BorderLayout.CENTER);
    actionPanel.add(jPanel9, null);
    jPanel9.add(daemonsLabel, null);
    jPanel9.add(startDaemonsRadioButton, null);
    jPanel9.add(stopDaemonsRadioButton, null);
    actionPanel.add(jPanel2, null);
    jPanel2.add(stateLabel, null);
    jPanel2.add(awakenDaemonsRadioButton, null);
    jPanel2.add(idleDaemonsRadioButton, null);
    actionPanel.add(jPanel3, null);
    jPanel3.add(debugLabel, null);
    jPanel3.add(debugOnRadioButton, null);
    jPanel3.add(debugOffRadioButton, null);
    actionPanel.add(jPanel4, null);
    jPanel4.add(loginsLabel, null);
    jPanel4.add(enableLoginsRadioButton, null);
    jPanel4.add(disableLoginsRadioButton, null);
    bottom.add(paddingEast, BorderLayout.EAST);
    bottom.add(paddingWest, BorderLayout.WEST);
  }

  // Returns what is displayed in the Users Logged In field
  public String getDisplayedNumUsersLoggedIn()
  {
     return usersLoggedInTextField.getText();
  }

  // Returns what is displayed in the Host Up-Time field
  public String getDisplayedHostUpTime()
  {
     return hostUpTimeTextField.getText();
  }

  // Returns what is displayed in the NTCSS Up-Time field
  public String getDisplayedNtcssUpTime()
  {
     return ntcssUpTimeTextField.getText();
  }

  // Used for the "look" when multiple hosts are selected
  public void refreshForMultipleHosts()
  {
     usersLoggedInTextField.setText("NA");
     hostUpTimeTextField.setText("NA");
     ntcssUpTimeTextField.setText("NA");

     startDaemonsRadioButton.setEnabled(true);
     startDaemonsRadioButton.setSelected(false);

     stopDaemonsRadioButton.setEnabled(true);
     stopDaemonsRadioButton.setSelected(false);

     awakenDaemonsRadioButton.setEnabled(true);
     awakenDaemonsRadioButton.setSelected(false);

     idleDaemonsRadioButton.setEnabled(true);
     idleDaemonsRadioButton.setSelected(false);

     debugOnRadioButton.setEnabled(true);
     debugOnRadioButton.setSelected(false);

     debugOffRadioButton.setEnabled(true);
     debugOffRadioButton.setSelected(false);

     enableLoginsRadioButton.setEnabled(true);
     enableLoginsRadioButton.setSelected(false);

     disableLoginsRadioButton.setEnabled(true);
     disableLoginsRadioButton.setSelected(false);
  }

  // Used for the "look" when a single host is selected and the Ntcss
  //  server on that host is up
  public void refreshForSingleUp(String currentHost)
  {
     String str;
     NtcssServersInfo svrInfo = NtcssServersInfo.getHandle();

     str = svrInfo.getHostUpTime(currentHost);
     if (str != null)
         hostUpTimeTextField.setText(str);
     else
         hostUpTimeTextField.setText("Error");

     str = svrInfo.getNumCurrentUsers(currentHost);
     if (str != null)
        usersLoggedInTextField.setText(str);
     else
        usersLoggedInTextField.setText("Error");

     str = svrInfo.getNtcssUpTime(currentHost);
     if (str != null)
        ntcssUpTimeTextField.setText(str);
     else
        ntcssUpTimeTextField.setText("Error");

     startDaemonsRadioButton.setSelected(true);
     startDaemonsRadioButton.setEnabled(true);

     stopDaemonsRadioButton.setSelected(false);
     stopDaemonsRadioButton.setEnabled(true);

     awakenDaemonsRadioButton.setEnabled(true);
     idleDaemonsRadioButton.setEnabled(true);
     if (svrInfo.isServerAwake(currentHost) == true) {
        awakenDaemonsRadioButton.setSelected(true);
        idleDaemonsRadioButton.setSelected(false);
     }
     else {
        awakenDaemonsRadioButton.setSelected(false);
        idleDaemonsRadioButton.setSelected(true);
     }

     debugOnRadioButton.setEnabled(true);
     debugOffRadioButton.setEnabled(true);
     if (svrInfo.isDebugOn(currentHost) == true) {
        debugOnRadioButton.setSelected(true);
        debugOffRadioButton.setSelected(false);
     }
     else {
        debugOnRadioButton.setSelected(false);
        debugOffRadioButton.setSelected(true);
     }

     enableLoginsRadioButton.setEnabled(true);
     disableLoginsRadioButton.setEnabled(true);
     if (svrInfo.areLoginsAllowed(currentHost) == true) {
        enableLoginsRadioButton.setSelected(true);
        disableLoginsRadioButton.setSelected(false);
     }
     else {
        enableLoginsRadioButton.setSelected(false);
        disableLoginsRadioButton.setSelected(true);
     }
  }

  // Used for the "look" when a single host is selected and the Ntcss
  //  server on that host is down
  public void refreshForSingleDown(String currentHost)
  {
     String str;

     str = (NtcssServersInfo.getHandle()).getHostUpTime(currentHost);
     if (str != null)
        hostUpTimeTextField.setText(str);
     else
        hostUpTimeTextField.setText("Error");

     usersLoggedInTextField.setText("NA");
     ntcssUpTimeTextField.setText("NA");

     startDaemonsRadioButton.setSelected(false);
     startDaemonsRadioButton.setEnabled(true);

     stopDaemonsRadioButton.setSelected(true);
     stopDaemonsRadioButton.setEnabled(true);

     awakenDaemonsRadioButton.setEnabled(false);
     awakenDaemonsRadioButton.setSelected(false);

     idleDaemonsRadioButton.setEnabled(false);
     idleDaemonsRadioButton.setSelected(false);

     debugOnRadioButton.setEnabled(false);
     debugOnRadioButton.setSelected(false);

     debugOffRadioButton.setEnabled(false);
     debugOffRadioButton.setSelected(false);

     enableLoginsRadioButton.setEnabled(false);
     enableLoginsRadioButton.setSelected(false);

     disableLoginsRadioButton.setEnabled(false);
     disableLoginsRadioButton.setSelected(false);
  }

  // Used for the "look" when a single host is selected and the Control Panel
  //  server on that host is down
  public void refreshForCPServerDown()
  {
     usersLoggedInTextField.setText("Error");
     hostUpTimeTextField.setText("Error");
     ntcssUpTimeTextField.setText("Error");

     startDaemonsRadioButton.setSelected(false);
     startDaemonsRadioButton.setEnabled(false);

     stopDaemonsRadioButton.setSelected(false);
     stopDaemonsRadioButton.setEnabled(false);

     awakenDaemonsRadioButton.setSelected(false);
     awakenDaemonsRadioButton.setEnabled(false);

     idleDaemonsRadioButton.setSelected(false);
     idleDaemonsRadioButton.setEnabled(false);

     debugOnRadioButton.setSelected(false);
     debugOnRadioButton.setEnabled(false);

     debugOffRadioButton.setSelected(false);
     debugOffRadioButton.setEnabled(false);

     enableLoginsRadioButton.setSelected(false);
     enableLoginsRadioButton.setEnabled(false);

     disableLoginsRadioButton.setSelected(false);
     disableLoginsRadioButton.setEnabled(false);
  }

  private boolean multipleHostsSelected()
  {
     return serverStatusPanel.multipleHostsSelected();
  }

  // Called when the user clicks on the "start daemons" radio button
  private void startDaemonsRadioButton_actionPerformed(ActionEvent e)
  {
     startDaemonsRadioButton.setSelected(false);

     if (serverStatusPanel.getCurrentHosts() != null &&
        serverStatusPanel.getCurrentHosts().length != 0) {

        if (multipleHostsSelected())
           startDaemons();
        else {
           if (!stopDaemonsRadioButton.isSelected())
              startDaemonsRadioButton.setSelected(true);
           else
              startDaemons();
        }
     }
  }

  // Does the actual work of calling for the daemons to be started
  public void startDaemons()
  {
     String parameters;
     boolean ret;
     NCPFrame mainWindow = NCPFrame.getHandle();

     // Display a dialog box to get the ntcss_init parameters
     parameters = JOptionPane.showInputDialog(NCPFrame.getHandle(),
                                  "Type init parameters:  (e.g. \"-noboot\")",
                                  "Ntcss Init Parameters",
                                  JOptionPane.QUESTION_MESSAGE);

     // Display a dialog box to get confirmation for this action
     if (JOptionPane.showConfirmDialog(NCPFrame.getHandle(),
                                       "Start the server(s)?",
                                       "Start Confirmation",
                                       JOptionPane.YES_NO_OPTION,
                                       JOptionPane.QUESTION_MESSAGE)
                                                     == JOptionPane.YES_OPTION) {
        mainWindow.setStatusBarText("Starting Ntcss server...");
        mainWindow.setCursorType(Cursor.WAIT_CURSOR);
        ret = (NtcssServersInfo.getHandle()).startServers(
                                serverStatusPanel.getCurrentHosts(), parameters);
        mainWindow.setCursorType(Cursor.DEFAULT_CURSOR);
        mainWindow.clearStatusBar();
        serverStatusPanel.refresh();
     }
  }

  // Called when the user clicks on the "stop daemons" radio button
  private void stopDaemonsRadioButton_actionPerformed(ActionEvent e)
  {
     stopDaemonsRadioButton.setSelected(false);

     if (serverStatusPanel.getCurrentHosts() != null &&
        serverStatusPanel.getCurrentHosts().length != 0) {

        if (multipleHostsSelected())
           stopDaemons();
        else {
           if (!startDaemonsRadioButton.isSelected())
              stopDaemonsRadioButton.setSelected(true);
           else
              stopDaemons();
        }
     }
  }

  // Does the actual work of calling for the daemons to be stopped
  public void stopDaemons()
  {
     boolean ret;
     NCPFrame mainWindow = NCPFrame.getHandle();

     // Display a dialog box to get confirmation for this action
     if (JOptionPane.showConfirmDialog(NCPFrame.getHandle(),
                                       "Stop the server(s)?",
                                       "Stop Confirmation",
                                       JOptionPane.YES_NO_OPTION,
                                       JOptionPane.QUESTION_MESSAGE)
                                                     == JOptionPane.YES_OPTION) {
        mainWindow.setStatusBarText("Stopping Ntcss server...");
        mainWindow.setCursorType(Cursor.WAIT_CURSOR);
        ret = (NtcssServersInfo.getHandle()).haltServers(
                                            serverStatusPanel.getCurrentHosts());
        mainWindow.setCursorType(Cursor.DEFAULT_CURSOR);
        mainWindow.clearStatusBar();
        serverStatusPanel.refresh();
     }
  }

  // Called when the user clicks on the "awaken daemons" radio button
  private void awakenDaemonsRadioButton_actionPerformed(ActionEvent e)
  {
     awakenDaemonsRadioButton.setSelected(false);

     if (serverStatusPanel.getCurrentHosts() != null &&
         serverStatusPanel.getCurrentHosts().length != 0) {

        if (multipleHostsSelected())
           awakenDaemons();
        else {
           if (!idleDaemonsRadioButton.isSelected())
              awakenDaemonsRadioButton.setSelected(true);
           else
              awakenDaemons();
        }
     }
  }

  // Does the actual work of calling for the daemons to be awakened
  public void awakenDaemons()
  {
     boolean ret;
     NCPFrame mainWindow = NCPFrame.getHandle();

     // Display a dialog box to get confirmation for the action
     if (JOptionPane.showConfirmDialog(NCPFrame.getHandle(),
                                       "Awaken the server(s)?",
                                       "Awaken Confirmation",
                                       JOptionPane.YES_NO_OPTION,
                                       JOptionPane.QUESTION_MESSAGE)
                                                     == JOptionPane.YES_OPTION) {
        mainWindow.setStatusBarText("Awaking Ntcss server...");
        mainWindow.setCursorType(Cursor.WAIT_CURSOR);
        ret = (NtcssServersInfo.getHandle()).awakenServers(
                                            serverStatusPanel.getCurrentHosts());
        mainWindow.setCursorType(Cursor.DEFAULT_CURSOR);
        mainWindow.clearStatusBar();
        serverStatusPanel.refresh();
     }
  }

  // Called when the user clicks on the "idle daemons" radio button
  private void idleDaemonsRadioButton_actionPerformed(ActionEvent e)
  {
     idleDaemonsRadioButton.setSelected(false);

     if (serverStatusPanel.getCurrentHosts() != null &&
         serverStatusPanel.getCurrentHosts().length != 0) {

        if (multipleHostsSelected())
           idleDaemons();
        else {
           if (!awakenDaemonsRadioButton.isSelected())
              idleDaemonsRadioButton.setSelected(true);
           else
              idleDaemons();
        }
     }
  }

  // Does the actual work of calling for the daemons to be idled
  public void idleDaemons()
  {
     boolean ret;
     NCPFrame mainWindow = NCPFrame.getHandle();

     // Display a dialog box to get confirmation for this action
     if (JOptionPane.showConfirmDialog(NCPFrame.getHandle(),
                                       "Idle the server(s)?",
                                       "Idle Confirmation",
                                       JOptionPane.YES_NO_OPTION,
                                       JOptionPane.QUESTION_MESSAGE)
                                                     == JOptionPane.YES_OPTION) {
        mainWindow.setStatusBarText("Idling Ntcss server...");
        mainWindow.setCursorType(Cursor.WAIT_CURSOR);
        ret = (NtcssServersInfo.getHandle()).idleServers(
                                            serverStatusPanel.getCurrentHosts());
        mainWindow.setCursorType(Cursor.DEFAULT_CURSOR);
        mainWindow.clearStatusBar();
        serverStatusPanel.refresh();
     }
  }

  // Called when the user clicks on the "debug on" radio button
  private void debugOnRadioButton_actionPerformed(ActionEvent e)
  {
     debugOnRadioButton.setSelected(false);

     if (serverStatusPanel.getCurrentHosts() != null &&
         serverStatusPanel.getCurrentHosts().length != 0) {

        if (multipleHostsSelected())
           turnDebugOn();
        else {
           if (!debugOffRadioButton.isSelected())
              debugOnRadioButton.setSelected(true);
           else
              turnDebugOn();
        }
     }
  }

  // Does the actual work of calling for the debug to be turned on
  public void turnDebugOn()
  {
     boolean ret;
     NCPFrame mainWindow = NCPFrame.getHandle();

     // Display a dialog box to get confirmation for this action
     if (JOptionPane.showConfirmDialog(NCPFrame.getHandle(),
                                       "Turn Debugging On?",
                                       "Debug Confirmation",
                                       JOptionPane.YES_NO_OPTION,
                                       JOptionPane.QUESTION_MESSAGE)
                                                     == JOptionPane.YES_OPTION) {
        mainWindow.setStatusBarText("Turning debugging on...");
        mainWindow.setCursorType(Cursor.WAIT_CURSOR);
        ret = (NtcssServersInfo.getHandle()).turnDebuggingOn(
                                           serverStatusPanel.getCurrentHosts());
        mainWindow.setCursorType(Cursor.DEFAULT_CURSOR);
        mainWindow.clearStatusBar();
        serverStatusPanel.refresh();
     }
  }

  // Called when the user clicks on the "debug off" radio button
  private void debugOffRadioButton_actionPerformed(ActionEvent e)
  {
     debugOffRadioButton.setSelected(false);

     if (serverStatusPanel.getCurrentHosts() != null &&
         serverStatusPanel.getCurrentHosts().length != 0) {

        if (multipleHostsSelected())
           turnDebugOff();
        else {
           if (!debugOnRadioButton.isSelected())
              debugOffRadioButton.setSelected(true);
           else
              turnDebugOff();
        }
     }
  }

  // Does the actual work of calling for the debug to be turned off
  public void turnDebugOff()
  {
     boolean ret;
     NCPFrame mainWindow = NCPFrame.getHandle();

     // Display dialog box to get confirmation for this action
     if (JOptionPane.showConfirmDialog(NCPFrame.getHandle(),
                                       "Turn Debugging Off?",
                                       "Debug Confirmation",
                                       JOptionPane.YES_NO_OPTION,
                                       JOptionPane.QUESTION_MESSAGE)
                                                     == JOptionPane.YES_OPTION) {
        mainWindow.setStatusBarText("Turning debugging off...");
        mainWindow.setCursorType(Cursor.WAIT_CURSOR);
        ret = (NtcssServersInfo.getHandle()).turnDebuggingOff(
                                           serverStatusPanel.getCurrentHosts());
        mainWindow.setCursorType(Cursor.DEFAULT_CURSOR);
        mainWindow.clearStatusBar();
        serverStatusPanel.refresh();
     }
  }

  // Called when the user clicks on the "enable logins" radio button
  private void enableLoginsRadioButton_actionPerformed(ActionEvent e)
  {
     enableLoginsRadioButton.setSelected(false);

     if (serverStatusPanel.getCurrentHosts() != null &&
         serverStatusPanel.getCurrentHosts().length != 0) {

        if (multipleHostsSelected())
           enableLogins();
        else {
           if (!disableLoginsRadioButton.isSelected())
              enableLoginsRadioButton.setSelected(true);
           else
              enableLogins();
        }
     }
  }

  // Does the actual work of calling for NTCSS logins to be enabled
  public void enableLogins()
  {
     boolean ret;
     NCPFrame mainWindow = NCPFrame.getHandle();

     // Display dialog box to get confirmation for this action
     if (JOptionPane.showConfirmDialog(NCPFrame.getHandle(),
                                       "Enable Ntcss Logins?",
                                       "Ntcss Login Confirmation",
                                       JOptionPane.YES_NO_OPTION,
                                       JOptionPane.QUESTION_MESSAGE)
                                                     == JOptionPane.YES_OPTION) {
        mainWindow.setStatusBarText("Enabling Ntcss logins...");
        mainWindow.setCursorType(Cursor.WAIT_CURSOR);
        ret = (NtcssServersInfo.getHandle()).enableServerLogins(
                                            serverStatusPanel.getCurrentHosts());
        mainWindow.setCursorType(Cursor.DEFAULT_CURSOR);
        mainWindow.clearStatusBar();
        serverStatusPanel.refresh();
     }
  }

  // Called when the user clicks on the "disable logins" radio button
  private void disableLoginsRadioButton_actionPerformed(ActionEvent e)
  {
     disableLoginsRadioButton.setSelected(false);

     if (serverStatusPanel.getCurrentHosts() != null &&
         serverStatusPanel.getCurrentHosts().length != 0) {

        if (multipleHostsSelected())
           disableLogins();
        else {
           if (!enableLoginsRadioButton.isSelected())
              disableLoginsRadioButton.setSelected(true);
           else
              disableLogins();
        }
     }
  }

  // Does the actual work of calling for NTCSS logins to be disabled
  public void disableLogins()
  {
     boolean ret;
     NCPFrame mainWindow = NCPFrame.getHandle();

     // Display dialog box to get confirmation for this action
     if (JOptionPane.showConfirmDialog(NCPFrame.getHandle(),
                                       "Disable Ntcss Logins?",
                                       "Ntcss Login Confirmation",
                                       JOptionPane.YES_NO_OPTION,
                                       JOptionPane.QUESTION_MESSAGE)
                                                     == JOptionPane.YES_OPTION) {
        mainWindow.setStatusBarText("Disabling Ntcss logins...");
        mainWindow.setCursorType(Cursor.WAIT_CURSOR);
        ret = (NtcssServersInfo.getHandle()).disableServerLogins(
                                            serverStatusPanel.getCurrentHosts());
        mainWindow.setCursorType(Cursor.DEFAULT_CURSOR);
        mainWindow.clearStatusBar();
        serverStatusPanel.refresh();
     }
  }
}

