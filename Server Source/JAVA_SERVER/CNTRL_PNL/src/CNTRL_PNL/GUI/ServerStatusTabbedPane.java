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
import java.util.*;
import java.io.*;

import CNTRL_PNL.INFO.*;
import COMMON.*;

public class ServerStatusTabbedPane extends TabbedPaneWithTable
{
   private ServerStatusTopPanel topPanel;
   private Vector emptyRow;
   private Vector errorRow;
   private String[] columnHeadings = {"Host", "Daemon Name", "Status", "Debug",
                                      "Access"};

   public ServerStatusTabbedPane()
   {    
       // Create and add the panel that is to go at the top of the tabbed pane
       topPanel = new ServerStatusTopPanel(this);
       addTopPanel(topPanel);

       // Create the table that is to go at the bottom of the tabbed pane
       int[] columnWidths = {100, 150, 100, 100, 100};
       setupTable(columnHeadings, columnWidths, false);

       // Create a row of empty columns for the table
       emptyRow = new Vector();
       emptyRow.addElement(" ");
       emptyRow.addElement(" ");
       emptyRow.addElement(" ");
       emptyRow.addElement(" ");
       emptyRow.addElement(" ");

       // Create a row with Error strings for the columns of the table
       errorRow = new Vector();
       errorRow.addElement(" "); // This will be replaced with the hostname
       errorRow.addElement("Error");
       errorRow.addElement("Error");
       errorRow.addElement("Error");
       errorRow.addElement("Error");
   }

   // Called when a host selection is made from the host list panel
   public void hostSelectionMade()
   {
      refresh();
   }

   // Gets information for the currently selected host(s) and sets the
   //  corresponding screen components with the data
   public void refresh()
   {
      String currentHost = (getCurrentHosts())[0];

      (NCPFrame.getHandle()).setCursorType(Cursor.WAIT_CURSOR);

      // Check to see how many hosts were selected
      if (multipleHostsSelected() == true)
         topPanel.refreshForMultipleHosts();
      else {

         // Check to see if the control panel server is up on the specified host
         if ((NtcssServersInfo.getHandle()).isCPServerUp(currentHost) == true) {

            // Check to see if the NTCSS server is up on the specified host
            if ((NtcssServersInfo.getHandle()).isNtcssServerUp(currentHost) == true)
               topPanel.refreshForSingleUp(currentHost);
            else
               topPanel.refreshForSingleDown(currentHost);
         }
         else
            topPanel.refreshForCPServerDown();
      }

      // Refresh the contents of the table
      refreshTable();

      // Refresh selected items in the host list panel
      (NCPFrame.getHandle()).repaintHostList(getCurrentHosts());

      (NCPFrame.getHandle()).setCursorType(Cursor.DEFAULT_CURSOR);
   }

   // Gets, reads, and displays the information for the table
   private void refreshTable()
   {
      String[] selectedHosts = getCurrentHosts();

      // Clear the current contents of the table
      clearTable();

      // For every one of the hosts, get their status file and display it
      for (int i = 0; i < selectedHosts.length; i++) {
         displayHostINIFile(selectedHosts[i]);
         addTableRow(emptyRow);
      }
   }

   // Gets, reads, and displays the status information in the table for the
   //  given host
   private void displayHostINIFile(String host)
   {
      String localFilename = "tmp.ini";

      // get the remote status INI file
      if ((NtcssServersInfo.getHandle()).getStatusINIFile(host, localFilename) == true) {

         // Read the INI file into the table
         displayINIFile(host, localFilename);

         // remove the INI file
         (new File(localFilename)).delete();
      }
      else {
         errorRow.setElementAt(host, 0);
         addTableRow(errorRow);
      }
   }

   // Given the name of a status INI file, this function will display its
   //  contents in the GUI table
   private void displayINIFile(String host, String localFilename)
   {
      INI iniFile = new INI();      
      Vector row;
      StringTokenizer sectionTokenizer;
      String sectionName;      

      // Get the section names from the INI file            
      sectionTokenizer = new StringTokenizer(
             iniFile.GetProfileSectionNames(localFilename),
             INI.STRINGS_DELIMITER);
      
      // Loop through each of the sections adding their information to the
      //  table
      while (sectionTokenizer.hasMoreElements() == true) {
         sectionName = sectionTokenizer.nextToken();
         
         row = new Vector();
         row.addElement(host);
         row.addElement(sectionName);
         
         // Add the values to the row vector such that the status value is
         //  before the debug value and the logins status value
         row.addElement(iniFile.GetProfileString(sectionName, "STATUS", 
                                                 "ERROR", localFilename));
         row.addElement(iniFile.GetProfileString(sectionName, "DEBUG", 
                                                 "ERROR", localFilename));
         if (sectionName.indexOf("esktop") != -1) 
            row.addElement(iniFile.GetProfileString(sectionName, "LOGINS", 
                                                    "ERROR", localFilename));
         else
            row.addElement(" ");            
         
         // Add the information to the table
         addTableRow(row);
      }      
   }

   // Repaints menu
   public void updateMenu()
   {
      (NCPFrame.getHandle()).showMenu(NCPMenuBar.SERVER_STATUS_MENU);
   }

   // Starts daemons
   public void startDaemons()
   {
      topPanel.startDaemons();
   }

   // Stops daemons
   public void stopDaemons()
   {
      topPanel.stopDaemons();
   }

   // Awakens daemons
   public void awakenDaemons()
   {
      topPanel.awakenDaemons();
   }

   // Idle daemons
   public void idleDaemons()
   {
      topPanel.idleDaemons();
   }

   // Turn debug on
   public void turnDebugOn()
   {
      topPanel.turnDebugOn();
   }

   // Turn debug off
   public void turnDebugOff()
   {
      topPanel.turnDebugOff();
   }

   // Enable logins
   public void enableLogins()
   {
      topPanel.enableLogins();
   }

   // Disable logins
   public void disableLogins()
   {
      topPanel.disableLogins();
   }

   // Obtains from the pane a string that represents all the information that
   //  is displayed on the pane.
   public String getPrintableMaterial()
   {
      String printableString;
      final String itemSeperator = "\t";
      final String lineSeperator = "\n";
      final int numRows = getTableRowCount();

      // Add to the printable string the host up-time
      printableString = "Host Up-Time:" + itemSeperator;
      printableString += topPanel.getDisplayedHostUpTime();
      printableString += lineSeperator;

      // Add to the printable string the NTCSS up-time
      printableString += "NTCSS Up-Time:" + itemSeperator;
      printableString += topPanel.getDisplayedNtcssUpTime();
      printableString += lineSeperator + lineSeperator;

      // Add to the printable string the current number of NTCSS users logged in
      printableString += "NTCSS Users Logged In:" + itemSeperator;
      printableString += topPanel.getDisplayedNumUsersLoggedIn();
      printableString += lineSeperator + lineSeperator;

      // Print the table headings
      printableString += paddedString(columnHeadings[0], 10);  // Host name
      printableString += paddedString(columnHeadings[1], 20);  // Daemon name
      printableString += paddedString(columnHeadings[2], 9);   // Daemon status
      printableString += paddedString(columnHeadings[3], 9);   // Debug state
      printableString += columnHeadings[4];                    // Login state
      printableString += lineSeperator;

      // Loop through the table items and concat them to the printable string
      for (int row = 0; row < numRows; row++) {
         // Print the host name
         printableString += paddedString((String)getTableItemAt(row, 0), 10);

         // Print the daemon name
         printableString += paddedString((String)getTableItemAt(row, 1), 20);

         // Print the daemon status
         printableString += paddedString((String)getTableItemAt(row, 2), 9);

         // Print the debug state
         printableString += paddedString((String)getTableItemAt(row, 3), 9);

         // Print the login state
         printableString += (String)getTableItemAt(row, 4);

         // If it is not the last row in the table, add a row seperator
         if (row != (numRows -1))
            printableString += lineSeperator;
      }

      return printableString;
   }
}


