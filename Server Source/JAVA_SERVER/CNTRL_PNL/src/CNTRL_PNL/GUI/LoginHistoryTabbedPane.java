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
import java.lang.*;
import java.io.*;
import java.net.*;

import CNTRL_PNL.INFO.*;
import COMMON.*;
import CNTRL_PNL.COMMON.*;

public class LoginHistoryTabbedPane extends TabbedPaneWithTable
{
  private final String LOGIN_HISTORY_FILENAME = "load_login_history.";
  private final String[] columnHeadings = {"Entry", "Login Name", "Event Time",
                                           "Client Address", "Event"};
  private LoginHistoryTopPanel loginHistoryTopPanel;

  public LoginHistoryTabbedPane()
  {
     final int[] columnSizes = {10,100,100,100,200};

     try {
        loginHistoryTopPanel = new LoginHistoryTopPanel(this);
        addTopPanel(loginHistoryTopPanel);
        setupTable(columnHeadings, columnSizes, false);
     }
     catch (Exception e) {
        Log.log(Log.CLT, Log.EXCP, this.getClass().getName(),
                "LoginHistoryTabbedPane", e.toString());
     }
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
    String[] selectedHosts = getCurrentHosts();

    try {
      if (selectedHosts != null) {
         (NCPFrame.getHandle()).setCursorType(Cursor.WAIT_CURSOR);

         loginHistoryTopPanel.setDisplayedHostname(selectedHosts[0]);
         clearTable();
         displayLoginHistoryINIFile(selectedHosts);
         loginHistoryTopPanel.disableRefreshButton();

         (NCPFrame.getHandle()).setCursorType(Cursor.DEFAULT_CURSOR);
      }
    }
    catch (Exception e) {
      Log.log(Log.CLT, Log.EXCP, this.getClass().getName(), "refresh",
              e.toString());
      (NCPFrame.getHandle()).setCursorType(Cursor.DEFAULT_CURSOR);
    }
  }

  // Loops the the returned INI file and passes the information to the table to be displayed.
  private void updateTable()
  {
    try {
      final String LOGIN_HISTORY_PROFILE_KEY="LOGIN_HISTORY";
      final String RECORD_COUNT_PROFILE_TAG="RECORD_COUNT";
      final int DEFAULT_INT=-1;
      final String DEFAULT_STRING="  ";
      final String[] profileKeyStrings = {"ENTRY_NUMBER", "LOGIN_NAME",
                                          "EVENT_TIME", "CLIENT_ADDRESS",
                                          "EVENT"};
      final int numberOfColumns = 5;
      String keyString;
      String entry;
      String localHostName;
      String iniFilename;

      INI myini = new INI();

      Vector row;

      int numberOfEntries;
      int entryNumber;

      Integer entryInteger;

      localHostName = InetAddress.getLocalHost().getHostName();
      iniFilename = LOGIN_HISTORY_FILENAME + localHostName;

      numberOfEntries =  myini.GetProfileInt(LOGIN_HISTORY_PROFILE_KEY,RECORD_COUNT_PROFILE_TAG,DEFAULT_INT,iniFilename);

      for (int index1=1; index1<=numberOfEntries; index1++) {
        row = new Vector();
        keyString = new String(LOGIN_HISTORY_PROFILE_KEY + " " + index1);
        for (int index2=0; index2 < numberOfColumns; index2++) {
          if (index2==0){
            entryNumber = myini.GetProfileInt(keyString,profileKeyStrings[index2],DEFAULT_INT,iniFilename);
            entryInteger = new Integer(entryNumber);
            row.addElement(entryInteger);
          } else {
            entry = myini.GetProfileString(keyString,profileKeyStrings[index2],DEFAULT_STRING,iniFilename);
            row.addElement(entry);
          }
        }
        addTableRow(row);
      }
    }
    catch (Exception e) {
      Log.log(Log.CLT, Log.EXCP, this.getClass().getName(), "updateTable",
              e.toString());
    }
  }

  // Calls getLoginHistoryINIFile to send the parameter file over,
  //  runs unix_ini on it and gets the output file from unix_ini.
  private void displayLoginHistoryINIFile(String[] hostList)
  {
    try {
      final String PARAM_LOGIN_HISTORY_FILENAME="param_login_history.ini";
      String host = hostList[0];
      String localHostName;
      String localReturnedIniFilename;
      String localSendIniFilename;

      localHostName = InetAddress.getLocalHost().getHostName();
      localReturnedIniFilename = LOGIN_HISTORY_FILENAME + localHostName;
      localSendIniFilename = PARAM_LOGIN_HISTORY_FILENAME;

      // Build the file which the server will use to create the login history
      //  INI file
      if (!buildParameterFile(host, localReturnedIniFilename, localSendIniFilename))
        return;

      // Get the remote login history INI file
      if ((NtcssServersInfo.getHandle()).getLoginHistoryINIFile(host,
                                         localSendIniFilename,
                                         localReturnedIniFilename) == true) {
        // Read the INI file into the table
        updateTable();

        // Remove the INI files
        (new File(localSendIniFilename)).delete();
        (new File(localReturnedIniFilename)).delete();
      }
    }
    catch (Exception e) {
      Log.log(Log.CLT, Log.EXCP, this.getClass().getName(),
              "displayLoginHistoryINIFile", e.toString());
    }
  }

  // Builds the parameter file to be used by unix_ini on the server
  private boolean buildParameterFile(String host,
                                     String localReturnedIniFilename,
                                     String localSendIniFilename)
  {
    try {
      String returnIniFilename;
      String maxEntriesString;
      String remoteINIDir = (NtcssServersInfo.getHandle()).getINIDirectory(host);

      INI myini = new INI();

      int myInt;
      int maxEntries = 0;

      returnIniFilename = remoteINIDir + localReturnedIniFilename;

      if (loginHistoryTopPanel != null) {
        if (loginHistoryTopPanel.getMaxEntries(-1)>0) {
          maxEntries = loginHistoryTopPanel.getMaxEntries(-1);
        }
      }
      maxEntriesString = Integer.toString(maxEntries);

      myInt = myini.WriteProfileString("LOGIN_HISTORY_OPTIONS","MAX_RECORD_COUNT",maxEntriesString,localSendIniFilename);
      if (myInt < 0) {
        return false;
      }
      myInt = myini.WriteProfileString("NTCSS_INI_PARAMETERS","OUTPUT_INI_FILENAME",returnIniFilename,localSendIniFilename);
      if (myInt < 0) {
        return false;
      }
      myInt = myini.WriteProfileString("NTCSS_INI_PARAMETERS","CONTENT_CODE","05",localSendIniFilename);
      if (myInt < 0) {
        return false;
      }
      myini.FlushINIFile(localSendIniFilename);
      return true;
    }
    catch (Exception e) {
      Log.log(Log.CLT, Log.EXCP, this.getClass().getName(),
              "buildParameterFile", e.toString());
      return false;
    }
  }

  // Repaints menu
  public void updateMenu()
  {
    try {
      (NCPFrame.getHandle()).showMenu(NCPMenuBar.LOGIN_HISTORY_MENU);
    }
    catch (Exception e) {
      Log.log(Log.CLT, Log.EXCP, this.getClass().getName(), "updateMenu",
              e.toString());
    }
  }

  // Obtains from the pane a string that represents all the information that
  //  is displayed.
  public String getPrintableMaterial()
  {
     String[] hosts = getCurrentHosts();
     String printableString;
     final String itemSeperator = "\t";
     final String lineSeperator = "\n";
     final int numRows = getTableRowCount();

     // Add the current host
     printableString = "Host" + itemSeperator + hosts[0] +
                        lineSeperator + lineSeperator;

     // Add the headers
     printableString += paddedString(columnHeadings[0], 7);  // Entry
     printableString += paddedString(columnHeadings[1], 12); // Login name
     printableString += paddedString(columnHeadings[2], 16); // Event time
     printableString += paddedString(columnHeadings[3], 17); // Client address
     printableString += columnHeadings[4];                   // Event
     printableString += lineSeperator;

     // Loop through the table items and concat them to the printable string
     for (int row = 0; row < numRows; row++) {

        // Add the entry
        printableString += paddedString(
                              ((Integer)getTableItemAt(row, 0)).toString(), 7);

        // Add the login name
        printableString += paddedString((String)getTableItemAt(row, 1), 12);

        // Add the event time
        printableString += paddedString((String)getTableItemAt(row, 2), 16);

        // Add the client address
        printableString += paddedString((String)getTableItemAt(row, 3), 17);

        // Add the event
        printableString += (String)getTableItemAt(row, 4);

        // If it is not the last row in the table, add a row seperator
        if (row != (numRows -1))
           printableString += lineSeperator;
     }

     return printableString;
  }
}

