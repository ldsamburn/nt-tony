/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package CNTRL_PNL.GUI;

import javax.swing.event.*;
import javax.swing.*;
import javax.swing.text.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.Component.*;
import java.util.*;
import java.net.*;

import CNTRL_PNL.INFO.*;
import CNTRL_PNL.COMMON.*;

public class HostListPanel extends JPanel
{
  private DefaultListModel model;
  private JScrollPane scrollPane;
  private JButton selectButton;
  private JList hostList;
  private HostInfoList hostInfoList;

  public HostListPanel()
  {
    try {

      // Create the "Select" button
      selectButton = new JButton("Select");
      selectButton.setMnemonic('s');
      selectButton.setEnabled(false);

      // Collect a list of all the hosts currently running the control panel
      //  server
      hostInfoList = (NtcssServersInfo.getHandle()).collectHostsInfo();

    	// Create a slot for each host in the list model
      model = new DefaultListModel();
      for(int i = 0; i < hostInfoList.size(); i++)
         model.addElement(new Integer(i));

      // Create the displayable version of the host list
      hostList = new JList(model);
      hostList.setCellRenderer(new HostListCellRenderer(hostList));
      hostList.setSelectionMode(ListSelectionModel.MULTIPLE_INTERVAL_SELECTION);

      // Setup the look and size of this panel
      this.setLayout(new BorderLayout());
      this.setMinimumSize(new Dimension(110, 300));

      // Add the scroll panel and "Select" button to this panel
      scrollPane = new JScrollPane(hostList);
      this.add(scrollPane, BorderLayout.CENTER);
      this.add(selectButton, BorderLayout.SOUTH);

      // Create listener for the "Select" button
      selectButton.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            selectButton.setEnabled(false);
            updateScreenForSelectedHosts();
         }
      });

      // Create listener for the host list
      hostList.addListSelectionListener(new ListSelectionListener() {
        public void valueChanged(ListSelectionEvent e)  {
          int[] selectedIndexList = hostList.getSelectedIndices();

          // Enable the "Select" button
          if (selectedIndexList.length > 0)
            selectButton.setEnabled(true);
          else
            selectButton.setEnabled(false);
        }
      });

    }
    catch (Exception e) {
      Log.log(Log.CLT, Log.EXCP, this.getClass().getName(), "HostListPanel",
              e.toString());
    }
  }

  // Causes any hosts that are highlighted in the list to be deselected.
  public void clearSelections()
  {
     hostList.validate();

     hostList.clearSelection();

     hostList.validate();
  }

  // If "single" is true, then the list is set to single selection mode.
  //  Otherwise, the list is set to multiple selection mode which allows
  //  multiple items to be selected
  public void singleSelectionMode(boolean single)
  {
     if (single == true)
        hostList.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
     else
        hostList.setSelectionMode(ListSelectionModel.MULTIPLE_INTERVAL_SELECTION);
  }

  // Called when the "Select" button is pressed
  private void updateScreenForSelectedHosts()
  {
    HostInfoListEntry entry;

    try {
      int[] selectedIndexList = hostList.getSelectedIndices();
      String[] selectedHostList = new String[selectedIndexList.length];

      // Create a list of the hosts that were selected
      for (int i = 0; i < selectedIndexList.length; i++) {
        entry = hostInfoList.getEntry(selectedIndexList[i]);
        if (entry != null)
           selectedHostList[i] = entry.getHostname();
      }

      // Pass the list of selected hosts to the current tabbed pane
      if (selectedHostList.length > 0)
         (NCPFrame.getHandle()).hostSelectionMade(selectedHostList);
      else
         JOptionPane.showMessageDialog((NCPFrame.getHandle()),
                                      "You must select at least 1 host.");
    }
    catch (Exception e) {
      Log.log(Log.CLT, Log.EXCP, this.getClass().getName(),
              "updateScreenForSelectedHosts", e.toString());
    }
  }

  // Collects the list of hosts running control panel servers and then
  //  displays the list
  public void refreshHostList()
  {
     (NCPFrame.getHandle()).setCursorType(Cursor.WAIT_CURSOR);

     // Collect the list of hosts
     hostInfoList = (NtcssServersInfo.getHandle()).collectHostsInfo();

     // Create a slot for each host in the list model
     model.removeAllElements();
	   for(int i = 0; i < hostInfoList.size(); i++)
       model.addElement(new Integer(i));

     // Create the displayable version of the host list
  	 hostList.setModel(model);

     (NCPFrame.getHandle()).setCursorType(Cursor.DEFAULT_CURSOR);
  }

  // Repaints the items in the host list
  public void repaintList(String[] hosts)
  {
     NtcssServersInfo svrInfo = NtcssServersInfo.getHandle();
     int index;
     String host;
     HostInfoListEntry entry;

     // Re-collect some information from the specified hosts
     for (int i = 0; i < hosts.length; i++) {

        host = hosts[i];

        entry = hostInfoList.getEntry(host);
        if (entry != null) {

           // Check to see if the control panel server is up on the host first
           if (svrInfo.isCPServerUp(host) == false) {

              // Set in the info list to indicate that that host's control
              //  panel server is down.
              entry.setCPServerState(NtcssServersInfo.CP_SVR_DOWN);
           }
           else {

              // Get and set the NTCSS server's status
              if (svrInfo.isNtcssServerUp(host) == true)
                 entry.setServerStatus(NtcssServersInfo.NTCSS_SVR_UP);
              else
                 entry.setServerStatus(NtcssServersInfo.NTCSS_SVR_DOWN);

              // Get and set the NTCSS server's type
              entry.setServerType(svrInfo.getServerType(host));
           }
        }
     }

     // Repaint the list
     hostList.repaint();
  }

  // Returns the initial data retrieved from the specified host
  public HostInfoListEntry getHostInfo(String host)
  {
     return hostInfoList.getEntry(host);
  }

  // Used in the displaying of each element in the host list
  class HostListCellRenderer extends JLabel implements ListCellRenderer
  {
     private NtcssServersInfo svrInfo;
     private ImageIcon masterSvr;
     private ImageIcon authSvr;
     private ImageIcon appSvr;
     private ImageIcon unknownSvr;
     private ImageIcon serverDown;

     public HostListCellRenderer(JList listBox)
     {
        URL url;

        setOpaque(true);

        svrInfo = NtcssServersInfo.getHandle();

        // Get the gif from the jar file which signifies that this host is a
        //  master server from
        url = getClass().getResource(Constants.MASTER_SVR_GIF);
        if (url != null)
           masterSvr = new ImageIcon(url);

        // Get the gif from the jar file which signifies that this host is an
        //  authenication server
        url = getClass().getResource(Constants.AUTH_SVR_GIF);
        if (url != null)
           authSvr = new ImageIcon(url);

        // Get the gif from the jar file which signifies that this host is an
        //  application server
        url = getClass().getResource(Constants.APP_SVR_GIF);
        if (url != null)
           appSvr = new ImageIcon(url);

        // Get the gif from the jar file which signifies that this host's server
        //  type could not be determined
        url = getClass().getResource(Constants.UNKNOWN_SVR_GIF);
        if (url != null)
           unknownSvr = new ImageIcon(url);

        // Get the gif from the jar file which signifies that the control panel
        //  server on this host is down
        url = getClass().getResource(Constants.SERVER_DOWN_GIF);
        if (url != null)
           serverDown = new ImageIcon(url);
     }

     // This is called whenever a cell needs repainting
     public Component getListCellRendererComponent(JList list, Object value,
                                                   int index, boolean isSelected,
                                                   boolean cellHasFocus)
     {
        String host;
        int svrType;
        int svrStatus;
        int cpSvrState;
        HostInfoListEntry entry;

        try {
           entry = hostInfoList.getEntry(index);

           host = entry.getHostname();
           svrType = entry.getServerType();
           svrStatus = entry.getServerStatus();
           cpSvrState = entry.getCPServerState();

           setText(host);

           // Check to see if the control panel server on the host is up first
           if (cpSvrState == NtcssServersInfo.CP_SVR_UP) {

              // Base the font style on wether the NTCSS server is up on the
              //  specified host or not
              if (svrStatus == NtcssServersInfo.NTCSS_SVR_UP) {
                 setFont(new Font("TimesRoman", Font.BOLD, 14));
                 setForeground(new Color(0, 186, 0));  // Green
              }
              else {
                 setFont(new Font("TimesRoman", Font.BOLD + Font.ITALIC, 14));
                 setForeground(Color.red);
              }

              // Base the icon on the NTCSS server type of the host
              if (svrType == Constants.MASTER_SVR)
                 setIcon(masterSvr);
              else if (svrType == Constants.AUTH_SVR)
                 setIcon(authSvr);
              else if (svrType == Constants.APP_SVR)
                 setIcon(appSvr);
              else
                 setIcon(unknownSvr);
           }
           else {
              setFont(new Font("TimesRoman", Font.PLAIN, 14));
              setForeground(Color.black);
              setIcon(serverDown);
           }

           // Set the selection highlighting look
           if (isSelected)
              setBackground(list.getSelectionBackground());
           else
              setBackground(list.getBackground());
        }
        catch (Exception e) {
           Log.log(Log.CLT, Log.EXCP, this.getClass().getName(),
                   "getListCellRendererComponent", e.toString());
        }

        return this;
     }
  }
}

