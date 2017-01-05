/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package CNTRL_PNL.GUI;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.*;

import CNTRL_PNL.INFO.*;
import CNTRL_PNL.COMMON.*;

// NOTE: This class is a singleton
public class NCPFrame extends JFrame
{
  private static NCPFrame frame = new NCPFrame(); // The single instance of this
                                                  //  class in existence.

  private NtcssServersInfo ntcssServersInfo;
  private NCPMenuBar       menuBar;
  private NCPStatusBar     statusBar;
  private JSplitPane       splitPane;
  private HostListPanel    hostListPanel;
  private TabbedPanel      tabbedPanel;

  private NCPFrame()
  {
     enableEvents(AWTEvent.WINDOW_EVENT_MASK);
  }

  // Creates the frame components and then displays the frame
  public void createWindow()
  {
    final int frameWidth = 800;
    final int frameHeight = 500;
    Dimension screenSize;
    int desiredX, desiredY;

    try {
      // Create the components of this frame
      statusBar = new NCPStatusBar();
      splitPane = new JSplitPane();
      hostListPanel = new HostListPanel();
      tabbedPanel = new TabbedPanel();
      menuBar = new NCPMenuBar();

      // Set the parameters for this frame
      this.getContentPane().setLayout(new BorderLayout());
      this.setTitle("NTCSS Control Panel");

      // Place the frame in the middle of the screen
      this.setSize(new Dimension(frameWidth, frameHeight));
      screenSize = (Toolkit.getDefaultToolkit()).getScreenSize();
      desiredX = (screenSize.width/2) - (frameWidth/2);
      desiredY = (screenSize.height/2) - (frameHeight/2);
      setLocation(new Point(desiredX, desiredY));

      // Setup the proper menu and add it to the frame
      tabbedPanel.updateMenu();
      this.setJMenuBar((JMenuBar)menuBar);

      // Add the status bar to this frame
      this.getContentPane().add(statusBar, BorderLayout.SOUTH);

      // Add the "guts" of the frame which is a split pane that contains
      //  the host list and the tabbed panel
      splitPane.setOrientation(JSplitPane.HORIZONTAL_SPLIT);
      splitPane.setLeftComponent(hostListPanel);
      splitPane.setRightComponent(tabbedPanel);
      this.getContentPane().add(splitPane, BorderLayout.CENTER);

      // Show the frame and validate it
      this.setVisible(true);
      this.validate();
    }
    catch (Exception e) {
      Log.log(Log.CLT, Log.EXCP, this.getClass().getName(), "createWindow",
              e.toString());
    }
  }

  // Returns the only instance of this class
  public static NCPFrame getHandle()
  {
     return frame;
  }

  //Overriden so we can exit on System Close
  protected void processWindowEvent(WindowEvent e)
  {
    try {
      super.processWindowEvent(e);

      if (e.getID() == WindowEvent.WINDOW_CLOSING) {
         try {
            Log.log(Log.CLT, Log.INFO, this.getClass().getName(), "processWindowEvent",
                            "NTCSS Control Panel stopped");
            System.exit(0);
         }
         catch (Exception excep) {
            Log.log(Log.CLT, Log.EXCP, this.getClass().getName(),
                                 "processWindowEvent", excep.toString());
         }
      }
    }
    catch (Exception excep) {
      Log.log(Log.CLT, Log.EXCP, this.getClass().getName(),
              "processWindowEvent", excep.toString());
    }
  }

  // Will send the host list to the currently selected tabbed pane
  public void hostSelectionMade(String[] hostList)
  {
    try {
      tabbedPanel.hostSelectionMade(hostList);
    }
    catch (Exception e) {
      Log.log(Log.CLT, Log.EXCP, this.getClass().getName(),
              "hostSelectionMade", e.toString());
    }
  }

  // Causes any hosts that are highlighted in the host list panel to be
  //  deselected.
  public void clearHostListSelections()
  {
     hostListPanel.clearSelections();
  }

  // If "single" is true, then the host list is set to single selection mode.
  //  Otherwise, the list is set to multiple selection mode which allows
  //  multiple items in the list to be selected.
  public void singleSelectionListMode(boolean single)
  {
     hostListPanel.singleSelectionMode(single);
  }

  // Changes the menu bar to the indicated menu
  public void showMenu(int menu)
  {
     try {
        menuBar.constructMenu(menu);
        setJMenuBar(menuBar);
     }
     catch (Exception e) {
        Log.log(Log.CLT, Log.EXCP, this.getClass().getName(),
                "showMenu", e.toString());
     }
  }

  // Returns the Server Status Tabbed Pane
  public ServerStatusTabbedPane getServerStatusPane()
  {
     ServerStatusTabbedPane sstPane = null;

     try {
        sstPane = tabbedPanel.getServerStatusPane();
     }
     catch (Exception e) {
        Log.log(Log.CLT, Log.EXCP, this.getClass().getName(),
                "getServerStatusPane", e.toString());
     }

     return sstPane;
  }

  // Returns the Common Data Tabbed Pane
  public CommonDataTabbedPane getCommonDataPane()
  {
     CommonDataTabbedPane cdPane = null;

     try {
        cdPane = tabbedPanel.getCommonDataPane();
     }
     catch (Exception e) {
        Log.log(Log.CLT, Log.EXCP, this.getClass().getName(),
                "getCommonDataPane", e.toString());
     }

     return cdPane;
  }

  // Returns the Login History Tabbed Pane
  public LoginHistoryTabbedPane getLoginHistoryPane()
  {
     LoginHistoryTabbedPane lhPane = null;

     try {
        lhPane = tabbedPanel.getLoginHistoryPane();
     }
     catch (Exception e) {
        Log.log(Log.CLT, Log.EXCP, this.getClass().getName(),
                "getLoginHistoryPane", e.toString());
     }

     return lhPane;
  }

  // Returns the System Log Tabbed Pane
  public SystemLogTabbedPane getSystemLogPane()
  {
     SystemLogTabbedPane slPane = null;

     try {
        slPane = tabbedPanel.getSystemLogPane();
     }
     catch (Exception e) {
        Log.log(Log.CLT, Log.EXCP, this.getClass().getName(),
                "getSystemLogPane", e.toString());
     }

     return slPane;
  }

  // Returns the Server Config Tabbed Pane
  public ServerConfigTabbedPane getServerConfigPane()
  {
     ServerConfigTabbedPane scPane = null;

     try {
        scPane = tabbedPanel.getServerConfigPane();
     }
     catch (Exception e) {
        Log.log(Log.CLT, Log.EXCP, this.getClass().getName(),
                "getServerConfigPane", e.toString());
     }

     return scPane;
  }

  // Recollects participating hosts and displays them in the host list panel
  public void refreshHostList()
  {
     try {
        hostListPanel.refreshHostList();
     }
     catch (Exception e) {
        Log.log(Log.CLT, Log.EXCP, this.getClass().getName(), "refreshHostList",
                e.toString());
     }
  }

  // Repaints the host list panel. NOTE: This does not re-collect a list of
  //  hosts to display in the panel. This merely forces the items in the host
  //  list to be redrawn in the list.
  public void repaintHostList(String[] hosts)
  {
     try {
        hostListPanel.repaintList(hosts);
     }
     catch (Exception e) {
        Log.log(Log.CLT, Log.EXCP, this.getClass().getName(), "repaintHostList",
                e.toString());
     }
  }

  // Changes the cursor to the specified type
  public void setCursorType(int type)
  {
    try {
      Cursor appCursor = this.getCursor();
      this.setCursor(appCursor.getPredefinedCursor(type));
    }
    catch (Exception e) {
      Log.log(Log.CLT, Log.EXCP, this.getClass().getName(), "setCursorType",
              e.toString());
    }
  }

  // Changes the text on the status bar
  public void setStatusBarText(String newText)
  {
     try {
        statusBar.setStatusText(newText);
        statusBar.repaint();
     }
     catch (Exception e) {
        Log.log(Log.CLT, Log.EXCP, this.getClass().getName(), "setStatusBarText",
                e.toString());
     }
  }

  // Clears the text on the status bar
  public void clearStatusBar()
  {
     try {
        statusBar.setStatusText(" ");
        statusBar.repaint();
     }
     catch (Exception e) {
        Log.log(Log.CLT, Log.EXCP, this.getClass().getName(), "clearStatusBar",
                e.toString());
     }
  }

  // Retrieves information from the current pane which can be printed.
  public String getPrintableMaterialFromCurrentPane()
  {
     return tabbedPanel.getPrintableMaterialFromCurrentPane();
  }


  // Returns the initial data retrieved from the specified host
  public HostInfoListEntry getHostInfo(String host)
  {
     return hostListPanel.getHostInfo(host);
  }
}

