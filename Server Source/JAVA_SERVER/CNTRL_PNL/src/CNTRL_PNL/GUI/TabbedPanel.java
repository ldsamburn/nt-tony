/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package CNTRL_PNL.GUI;

import javax.swing.*;
import javax.swing.text.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.Component.*;
import java.lang.Object.*;
import java.util.*;

import CNTRL_PNL.INFO.*;
import CNTRL_PNL.COMMON.*;

public class TabbedPanel extends JPanel
{
   private JTabbedPane tabbedPanel = new JTabbedPane();
   private TabbedPaneBase[] paneArray;
   private final int SERVER_STATUS_POSITION = 0;  // These numbers coorespond
   private final int COMMON_DATA_POSITION   = 1;  //  to the order the panes
   private final int LOGIN_HISTORY_POSITION = 2;  //  appear in the tabbed panel.
   private final int SYSTEM_LOG_POSITION    = 3;  //  To change the order, just
   private final int SERVER_CONFIG_POSITION = 4;  //  change these numbers.
   private final int NUMBER_OF_PANES        = 5;

   public TabbedPanel()
   {
      String[] tabHeadings;

      try {
         // Set the tab headings array
         tabHeadings = new String[NUMBER_OF_PANES];
         tabHeadings[SERVER_STATUS_POSITION] = "Server Status";
         tabHeadings[COMMON_DATA_POSITION]   = "Common Data";
         tabHeadings[LOGIN_HISTORY_POSITION] = "Login History";
         tabHeadings[SYSTEM_LOG_POSITION]    = "System Log";
         tabHeadings[SERVER_CONFIG_POSITION] = "Domain Setup";
       
         // Create the tabbed panes
         paneArray = new TabbedPaneBase[NUMBER_OF_PANES];
         paneArray[SERVER_STATUS_POSITION] = new ServerStatusTabbedPane();
         paneArray[COMMON_DATA_POSITION]   = new CommonDataTabbedPane();
         paneArray[LOGIN_HISTORY_POSITION] = new LoginHistoryTabbedPane();
         paneArray[SYSTEM_LOG_POSITION]    = new SystemLogTabbedPane();
         paneArray[SERVER_CONFIG_POSITION] = new ServerConfigTabbedPane();

         // Add the panes in the order they are to appear in the tabbed panel
         for (int i = 0; i < NUMBER_OF_PANES; i++)
            tabbedPanel.addTab(tabHeadings[i], paneArray[i]);

         // Create the listener which listens for the user to click on a tab
         tabbedPanel.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(MouseEvent e)
            {
               tabSelectionMade();
            }
         });

         this.setLayout(new BorderLayout());
         this.add(tabbedPanel, BorderLayout.CENTER);
      }
      catch (Exception e) {
         Log.log(Log.CLT, Log.EXCP, this.getClass().getName(), "TabbedPanel",
                 e.toString());
      }
   }

   // Calls the updateMenu method of the currently selected pane.
   public void updateMenu()
   {
      try {
         paneArray[tabbedPanel.getSelectedIndex()].updateMenu();
      }
      catch (Exception e) {
         Log.log(Log.CLT, Log.EXCP, this.getClass().getName(), "updateMenu",
                 e.toString());
      }
   }

   // Calls the setCurrentHost method of the currently selected pane
   public void hostSelectionMade(String[] hostList)
   {
      try {
         paneArray[tabbedPanel.getSelectedIndex()].setCurrentHost(hostList);
      }
      catch (Exception e) {
         Log.log(Log.CLT, Log.EXCP, this.getClass().getName(),
                 "refreshCurrentTabbedPane", e.toString());
      }
   }

   // Called when the user clicks on one of the tabbed panes
   private void tabSelectionMade()
   {
      int selectedIndex;

      try {
         selectedIndex = tabbedPanel.getSelectedIndex();

         // Decide if the selection mode for the host list needs to be changed
         if ((selectedIndex == SERVER_STATUS_POSITION) ||
             (selectedIndex == SERVER_CONFIG_POSITION))
            (NCPFrame.getHandle()).singleSelectionListMode(false);
         else
            (NCPFrame.getHandle()).singleSelectionListMode(true);

         // Deselect any highlighted items in the list
         (NCPFrame.getHandle()).clearHostListSelections();

         // Update the menu bar to reflect the newly selected tabbed pane
         paneArray[selectedIndex].updateMenu();
      }
      catch (Exception excep) {
         Log.log(Log.CLT, Log.EXCP, this.getClass().getName(),
                 "tabSelectionMade", excep.toString());
      }
   }

   // Returns the Servers Status pane
   public ServerStatusTabbedPane getServerStatusPane()
   {
      return (ServerStatusTabbedPane)paneArray[SERVER_STATUS_POSITION];
   }

   // Returns the Common Data pane
   public CommonDataTabbedPane getCommonDataPane()
   {
      return (CommonDataTabbedPane)paneArray[COMMON_DATA_POSITION];
   }

   // Returns the Login History pane
   public LoginHistoryTabbedPane getLoginHistoryPane()
   {
      return (LoginHistoryTabbedPane)paneArray[LOGIN_HISTORY_POSITION];
   }

   // Returns the System Log pane
   public SystemLogTabbedPane getSystemLogPane()
   {
      return (SystemLogTabbedPane)paneArray[SYSTEM_LOG_POSITION];
   }

   // Returns the Server Configuration pane
   public ServerConfigTabbedPane getServerConfigPane()
   {
      return (ServerConfigTabbedPane)paneArray[SERVER_CONFIG_POSITION];
   }

   // Retrieves information from the current pane which can be printed.
   public String getPrintableMaterialFromCurrentPane()
   {
      String printableMaterial;

      // Try to get the printable material from the current selected pane
      try {
         printableMaterial = paneArray[tabbedPanel.getSelectedIndex()].
                                                        getPrintableMaterial();
      }
      catch (Exception e) {
         Log.log(Log.CLT, Log.EXCP, this.getClass().getName(),
                 "getPrintableMaterialFromCurrentPane", e.toString());
         printableMaterial = null;
      }

      return printableMaterial;
   }
}
