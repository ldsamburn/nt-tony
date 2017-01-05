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
import javax.swing.*;
import javax.swing.event.*;
import java.util.*;

import CNTRL_PNL.INFO.*;
import CNTRL_PNL.COMMON.*;
import COMMON.*;

public class NCPMenuBar extends JMenuBar
{
   private FileMenu fileMenu;
   private HelpMenu helpMenu;

   private ServerStatusMenu serverStatusMenu;
   private CommonDataMenu   commonDataMenu;
   private LoginHistoryMenu loginHistoryMenu;
   private SystemLogMenu    systemLogMenu;
   private ServerConfigMenu serverConfigMenu;

   public static final int DEFAULT_MENU       = 0;
   public static final int SERVER_STATUS_MENU = 1;
   public static final int COMMON_DATA_MENU   = 2;
   public static final int LOGIN_HISTORY_MENU = 3;
   public static final int SYSTEM_LOG_MENU    = 4;
   public static final int SERVER_CONFIG_MENU = 5;

   public NCPMenuBar()
   {
      try {
         // Create all the menus
         fileMenu = new FileMenu();
         helpMenu = new HelpMenu();
         serverStatusMenu = new ServerStatusMenu();
         commonDataMenu   = new CommonDataMenu();
         loginHistoryMenu = new LoginHistoryMenu();
         systemLogMenu    = new SystemLogMenu();
         serverConfigMenu = new ServerConfigMenu();

         // Set up the default menu bar
         constructMenu(DEFAULT_MENU);
      }
      catch (Exception e) {
         Log.log(Log.CLT, Log.EXCP, this.getClass().getName(), "NCPMenuBar",
                 e.toString());
      }
   }

   // Constructs the requested menu bar
   public void constructMenu(int menu)
   {
      // Clear the currently displayed menu bar
      while (getMenuCount() > 0)
         remove(getMenu(0));

      // Add the file menu back
      add(fileMenu);

      // Add the desired menu
      switch (menu) {
         case SERVER_STATUS_MENU:
            add(serverStatusMenu);
            break;
         case COMMON_DATA_MENU:
            add(commonDataMenu);
            break;
         case LOGIN_HISTORY_MENU:
            add(loginHistoryMenu);
            break;
         case SYSTEM_LOG_MENU:
            add(systemLogMenu);
            break;
         case SERVER_CONFIG_MENU:
            add(serverConfigMenu);
            break;
         default:
            break;
      }

      // Add the help menu back
      add(helpMenu);
   }
}

// Contains data for the File menu
class FileMenu extends JMenu
{
   private JMenuItem printMenuItem;
   private JMenuItem refreshHostListMenuItem;
   private JMenuItem exitMenuItem;

   public FileMenu()
   {
      super("File");
      setMnemonic('F');

      // Create the items in this menu
      exitMenuItem = new JMenuItem("Exit");
      exitMenuItem.setMnemonic('x');
      refreshHostListMenuItem = new JMenuItem("Refresh Host List");
      refreshHostListMenuItem.setMnemonic('f');
      printMenuItem = new JMenuItem("Print...");
      printMenuItem.setMnemonic('P');

      // Create the menu item listeners
      exitMenuItem.addActionListener(new ActionListener()  {
         public void actionPerformed(ActionEvent e)
         {
            exitSelected();
         }
      });

      refreshHostListMenuItem.addActionListener(new ActionListener()  {
         public void actionPerformed(ActionEvent e)
         {
            (NCPFrame.getHandle()).refreshHostList();
         }
      });

      printMenuItem.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e)
         {
           printSelected();
         }
      });

      // Add the menu items to the menu
      add(printMenuItem);
      add(refreshHostListMenuItem);
      addSeparator();
      add(exitMenuItem);
   }

   // Called when File | Exit menu selected
   private void exitSelected()
   {
      try {
         Log.log(Log.CLT, Log.INFO, this.getClass().getName(), "exitSelected",
                            "NTCSS Control Panel stopped");
         System.exit(0);
      }
      catch (Exception ex) {
         Log.log(Log.CLT, Log.EXCP, this.getClass().getName(),
                 "exitSelected", ex.toString());
      }
   }

   // This is called when the Print menu item is selected. It retrieves the
   //  information that is printable from the current top pane and sends it
   //  to a user selected printer (selected through a displayed GUI).
   private void printSelected()
   {
      String data;
      HardcopyWriter out;
      String pageTitle = "Print Output";
      int fontSize = 10;
      double leftMargin   = .75; // Inches
      double rightMargin  = .75; // Inches
      double topMargin    = .75; // Inches
      double bottomMargin = .75; // Inches
      char[] buffer = null;

      // Get the printable material from the current top pane
      data = (NCPFrame.getHandle()).getPrintableMaterialFromCurrentPane();

      if (data != null) {

         // Convert the material into an array of characters
         buffer = new char[data.length()];
         data.getChars(0, data.length(), buffer, 0);

         // Try to create a stream that will send data to a printer
         try {
            out = new HardcopyWriter(NCPFrame.getHandle(), pageTitle,
                                     fontSize, leftMargin, rightMargin, topMargin,
                                     bottomMargin);
         }
         catch (HardcopyWriter.PrintCanceledException e) { // Thrown when user
            out = null;                                    //  clicks on "Cancel"
         }                                                 //  button in print
                                                           //  job window.
         // Send the data to be printed
         if (out != null) {

            // Write the data to the printer stream
            out.write(buffer, 0, buffer.length);

            // Close the stream forcing the data to be printed
            out.close();
         }
      }
      else {
         // Display a dialog box which indicates that there is no printable
         //  material on the current pane
         JOptionPane.showMessageDialog(NCPFrame.getHandle(),
                                    "No printable material available for tab",
                                    "Print Request",
                                    JOptionPane.PLAIN_MESSAGE);
      }
   }
}

// Contains data for the Help menu
class HelpMenu extends JMenu
{
   private JMenuItem aboutMenuItem;

   public HelpMenu()
   {
      super("Help");
      setMnemonic('H');

      // Create the items in this menu
      aboutMenuItem = new JMenuItem("About...");
      aboutMenuItem.setMnemonic('A');

      // Create the menu item listeners
      aboutMenuItem.addActionListener(new ActionListener()  {
         public void actionPerformed(ActionEvent e) {
            helpAbout_actionPerformed(e);
         }
      });

      // Add the menu items to the menu
      add(aboutMenuItem);
   }

   // Called when Help | About menu selected
   private void helpAbout_actionPerformed(ActionEvent e) {
      try {
         NCPFrameAboutBox dlg = new NCPFrameAboutBox();
      }
      catch (Exception excep) {
         Log.log(Log.CLT, Log.EXCP, this.getClass().getName(),
                 "helpAbout_actionPerformed", excep.toString());
      }
   }
}

// Contains data for the Server Status menu
class ServerStatusMenu extends JMenu
{
   private ServerStatusTabbedPane serverStatusPanel;

   private JMenuItem startServerMenuItem;
   private JMenuItem haltServerMenuItem;
   private JMenuItem awakenServerMenuItem;
   private JMenuItem idleServerMenuItem;
   private JMenuItem debugOnMenuItem;
   private JMenuItem debugOffMenuItem;
   private JMenuItem enableLoginsMenuItem;
   private JMenuItem disableLoginsMenuItem;
   private JMenuItem refreshMenuItem;

   public ServerStatusMenu()
   {
      super("Server Status");
      setMnemonic('v');

      serverStatusPanel = (NCPFrame.getHandle()).getServerStatusPane();

      // Create a menu listener for this menu
      addMenuListener(new MenuListener()  {
         public void menuCanceled(MenuEvent e) {}
         public void menuDeselected(MenuEvent e) {}

         public void menuSelected(MenuEvent e)
         {
            refreshMenu();
         }
      });

      // Create the items in this menu
      startServerMenuItem = new JMenuItem("Start Server");
      startServerMenuItem.setMnemonic('r');
      haltServerMenuItem = new JMenuItem("Stop Server");
      haltServerMenuItem.setMnemonic('p');
      awakenServerMenuItem = new JMenuItem("Awaken Server");
      awakenServerMenuItem.setMnemonic('A');
      idleServerMenuItem = new JMenuItem("Idle Server");
      idleServerMenuItem.setMnemonic('I');
      debugOnMenuItem = new JMenuItem("Turn Debug On");
      debugOnMenuItem.setMnemonic('n');
      debugOffMenuItem = new JMenuItem("Turn Debug Off");
      debugOffMenuItem.setMnemonic('f');
      enableLoginsMenuItem = new JMenuItem("Enable Logins");
      enableLoginsMenuItem.setMnemonic('E');
      disableLoginsMenuItem = new JMenuItem("Disable Logins");
      disableLoginsMenuItem.setMnemonic('D');
      refreshMenuItem = new JMenuItem("Refresh");
      refreshMenuItem.setMnemonic('h');


      // Create the menu item listeners
      startServerMenuItem.addActionListener(new ActionListener()  {
         public void actionPerformed(ActionEvent e)
         {
            serverStatusPanel.startDaemons();
         }
      });

      haltServerMenuItem.addActionListener(new ActionListener()  {
         public void actionPerformed(ActionEvent e)
         {
            serverStatusPanel.stopDaemons();
         }
      });

      awakenServerMenuItem.addActionListener(new ActionListener()  {
         public void actionPerformed(ActionEvent e)
         {
            serverStatusPanel.awakenDaemons();
         }
      });

      idleServerMenuItem.addActionListener(new ActionListener()  {
         public void actionPerformed(ActionEvent e)
         {
            serverStatusPanel.idleDaemons();
         }
      });

      debugOnMenuItem.addActionListener(new ActionListener()  {
         public void actionPerformed(ActionEvent e)
         {
            serverStatusPanel.turnDebugOn();
         }
      });

      debugOffMenuItem.addActionListener(new ActionListener()  {
         public void actionPerformed(ActionEvent e)
         {
            serverStatusPanel.turnDebugOff();
         }
      });

      enableLoginsMenuItem.addActionListener(new ActionListener()  {
         public void actionPerformed(ActionEvent e)
         {
            serverStatusPanel.enableLogins();
         }
      });

      disableLoginsMenuItem.addActionListener(new ActionListener()  {
         public void actionPerformed(ActionEvent e)
         {
            serverStatusPanel.disableLogins();
         }
      });

      refreshMenuItem.addActionListener(new ActionListener()  {
         public void actionPerformed(ActionEvent e)
         {
            serverStatusPanel.refresh();
         }
      });

      // Add the menu items to the menu
      add(startServerMenuItem);
      add(haltServerMenuItem);
      addSeparator();
      add(awakenServerMenuItem);
      add(idleServerMenuItem);
      addSeparator();
      add(debugOnMenuItem);
      add(debugOffMenuItem);
      addSeparator();
      add(enableLoginsMenuItem);
      add(disableLoginsMenuItem);
      addSeparator();
      add(refreshMenuItem);
   }

   // This is called whenever this menu is selected. It disables any menu items
   //  which don't need to be enabled
   private void refreshMenu()
   {
      String[] selectedHosts = serverStatusPanel.getCurrentHosts();
      String singleHost;
      NtcssServersInfo svrInfo = NtcssServersInfo.getHandle();

      // If there are any hosts selected
      if (selectedHosts != null && selectedHosts.length != 0) {

         // If multiple hosts have been selected
         if (selectedHosts.length > 1) {
            startServerMenuItem.setEnabled(true);
            haltServerMenuItem.setEnabled(true);

            awakenServerMenuItem.setEnabled(true);
            idleServerMenuItem.setEnabled(true);

            debugOnMenuItem.setEnabled(true);
            debugOffMenuItem.setEnabled(true);

            enableLoginsMenuItem.setEnabled(true);
            disableLoginsMenuItem.setEnabled(true);

            refreshMenuItem.setEnabled(true);
         }
         else { // Only a single host has been selected
            singleHost = selectedHosts[0];

            // If the ControlPanel server on that host is up
            if (svrInfo.isCPServerUp(singleHost) == true) {

               // If the Ntcss server on that host is up
               if (svrInfo.isNtcssServerUp(singleHost) == true) {
                  startServerMenuItem.setEnabled(false);
                  haltServerMenuItem.setEnabled(true);

                  // Check the awake/idle state
                  if (svrInfo.isServerAwake(singleHost) == true) {
                     awakenServerMenuItem.setEnabled(false);
                     idleServerMenuItem.setEnabled(true);
                  }
                  else {
                     awakenServerMenuItem.setEnabled(true);
                     idleServerMenuItem.setEnabled(false);
                  }

                  // Check the debug state
                  if (svrInfo.isDebugOn(singleHost) == true) {
                     debugOnMenuItem.setEnabled(false);
                     debugOffMenuItem.setEnabled(true);
                  }
                  else {
                     debugOnMenuItem.setEnabled(true);
                     debugOffMenuItem.setEnabled(false);
                  }

                  // Check the login state
                  if (svrInfo.areLoginsAllowed(singleHost) == true) {
                     enableLoginsMenuItem.setEnabled(false);
                     disableLoginsMenuItem.setEnabled(true);
                  }
                  else {
                     enableLoginsMenuItem.setEnabled(true);
                     disableLoginsMenuItem.setEnabled(false);
                  }

                  refreshMenuItem.setEnabled(true);
               }
               else { // The Ntcss server on the singly selected host is down
                  startServerMenuItem.setEnabled(true);
                  haltServerMenuItem.setEnabled(false);

                  awakenServerMenuItem.setEnabled(false);
                  idleServerMenuItem.setEnabled(false);

                  debugOnMenuItem.setEnabled(false);
                  debugOffMenuItem.setEnabled(false);

                  enableLoginsMenuItem.setEnabled(false);
                  disableLoginsMenuItem.setEnabled(false);

                  refreshMenuItem.setEnabled(true);
               }
            }  // The control panel server is down
            else {
               startServerMenuItem.setEnabled(false);
               haltServerMenuItem.setEnabled(false);

               awakenServerMenuItem.setEnabled(false);
               idleServerMenuItem.setEnabled(false);

               debugOnMenuItem.setEnabled(false);
               debugOffMenuItem.setEnabled(false);

               enableLoginsMenuItem.setEnabled(false);
               disableLoginsMenuItem.setEnabled(false);

               refreshMenuItem.setEnabled(false);
            }
         }
      }
      else {  // No hosts have been selected
         startServerMenuItem.setEnabled(false);
         haltServerMenuItem.setEnabled(false);

         awakenServerMenuItem.setEnabled(false);
         idleServerMenuItem.setEnabled(false);

         debugOnMenuItem.setEnabled(false);
         debugOffMenuItem.setEnabled(false);

         enableLoginsMenuItem.setEnabled(false);
         disableLoginsMenuItem.setEnabled(false);

         refreshMenuItem.setEnabled(false);
      }
   }
}

// Contains data for the Common Data menu
class CommonDataMenu extends JMenu
{
   private CommonDataTabbedPane commonDataPanel;
   private JMenuItem refreshMenuItem;

   public CommonDataMenu()
   {
      super("Common Data");
      setMnemonic('C');

      commonDataPanel = (NCPFrame.getHandle()).getCommonDataPane();

      // Create the items in this menu
      refreshMenuItem = new JMenuItem("Refresh");
      refreshMenuItem.setMnemonic('h');

      // Create the menu item listeners
      refreshMenuItem.addActionListener(new ActionListener()  {
         public void actionPerformed(ActionEvent e)
         {
            commonDataPanel.refresh();
         }
      });

      // Add the menu items to the menu
      add(refreshMenuItem);
   }
}

// Contains data for the Login History menu
class LoginHistoryMenu extends JMenu
{
   private LoginHistoryTabbedPane loginHistoryPanel;
   private JMenuItem refreshMenuItem;

   public LoginHistoryMenu()
   {
      super("Login History");
      setMnemonic('L');

      loginHistoryPanel = (NCPFrame.getHandle()).getLoginHistoryPane();

      // Create the items in this menu
      refreshMenuItem = new JMenuItem("Refresh");
      refreshMenuItem.setMnemonic('h');

      // Create the menu item listeners
      refreshMenuItem.addActionListener(new ActionListener()  {
         public void actionPerformed(ActionEvent e)
         {
            loginHistoryPanel.refresh();
         }
      });

      // Add the menu items to the menu
      add(refreshMenuItem);
   }
}

// Contains data for the System Log menu
class SystemLogMenu extends JMenu
{
   private SystemLogTabbedPane systemLogPanel;
   private JMenuItem refreshMenuItem;

   public SystemLogMenu()
   {
      super("System Log");
      setMnemonic('S');

      systemLogPanel = (NCPFrame.getHandle()).getSystemLogPane();

      // Create the items in this menu
      refreshMenuItem = new JMenuItem("Refresh");
      refreshMenuItem.setMnemonic('h');

      // Create the menu item listeners
      refreshMenuItem.addActionListener(new ActionListener()  {
         public void actionPerformed(ActionEvent e)
         {
            systemLogPanel.refresh();
         }
      });

      // Add the menu items to the menu
      add(refreshMenuItem);
   }
}

// Contains data for the Server Config menu
class ServerConfigMenu extends JMenu
{
   private ServerConfigTabbedPane serverConfigPanel;
   private JMenuItem clearMenuItem;
   private JMenuItem removeMenuItem;
   private JMenuItem applyMenuItem;

   public ServerConfigMenu()
   {
      super("Domain Setup");
      setMnemonic('m');

      serverConfigPanel = (NCPFrame.getHandle()).getServerConfigPane();

      // Create the items in this menu
      clearMenuItem = new JMenuItem("Clear");
      clearMenuItem.setMnemonic('C');
      removeMenuItem = new JMenuItem("Remove");
      removeMenuItem.setMnemonic('v');
      applyMenuItem = new JMenuItem("Apply");
      applyMenuItem.setMnemonic('A');

      // Create the menu item listeners
      clearMenuItem.addActionListener(new ActionListener()  {
         public void actionPerformed(ActionEvent e)
         {
            serverConfigPanel.clear();
         }
      });

      removeMenuItem.addActionListener(new ActionListener()  {
         public void actionPerformed(ActionEvent e)
         {
            serverConfigPanel.remove();
         }
      });

      applyMenuItem.addActionListener(new ActionListener()  {
         public void actionPerformed(ActionEvent e)
         {
            serverConfigPanel.apply();
         }
      });

      // Add the menu items to the menu
      add(clearMenuItem);
      add(removeMenuItem);
      add(applyMenuItem);
   }
}
