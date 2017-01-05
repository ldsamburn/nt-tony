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

public class SystemLogTabbedPane extends TabbedPaneWithTable
{
  private SystemLogTopPanel systemLogTopPanel;
  private final String SYSTEM_LOG_FILENAME = "load_system_log.";
  private final String[] columnHeadings = {"Entry",
      "Date",
      "Host",
      "Entity",
      "PID",
      "Event"};
  private final String[] entityNames = {
    "telnetd",
    "ftpd",
    "sendmail",
    "upsd",
    "NTCSS_INIT",
    "NCSERVER",
    "LPRPROXY",
    "BCSERVER",
    "PRTQ_SVR",
    "DB_SVR",
    "CMD_SVR",
    "SPQ_SVR",
    "DSK_SVR",
    "MSG_SVR",
    "SYS_MON",
    "USER_ADMIN",
    "FS_ARCH",
    "SRV_DEV",
    "APP_PRT_CNF",
    "PRT_CONF",
    "DMResMon",
    "DM",
    "NTCSS_CMD",
    "NTCSS_INITDB"};
  private  final String[] entityDescriptions = {
    "Telnet Daemon",
    "File Transfer Protocol Daemon",
    "Sendmail Deamon",
    "UPS Deamon",
    "NTCSS Startup Program",
    "NTCSS Net Copy Server",
    "NTCSS LPR Proxy Program",
    "NTCSS Broadcast Server",
    "NTCSS Print Queue Server",
    "NTCSS Database Server",
    "NTCSS Command Server",
    "NTCSS Server Process Queue Server",
    "NTCSS Desktop Server",
    "NTCSS Message Server",
    "NTCSS System Monitor",
    "NTCSS User Administration Utility",
    "NTCSS File System Backup/Restore Utility",
    "NTCSS Server Device Interface Utility",
    "NTCSS Application Printer Configuration Utility",
    "NTCSS Printer Configuration Utility",
    "NTCSS DM Resource Monitor",
    "NTCSS Distribution Manager",
    "NTCSS Command Utility",
    "NTCSS Database Initialization Program"};

  public SystemLogTabbedPane()
  {
    final int[] columnSizes = {10,100,100,100,50,400};

    try {
      systemLogTopPanel = new SystemLogTopPanel(this,
        entityNames, entityDescriptions);
      addTopPanel(systemLogTopPanel);
      setupTable(columnHeadings, columnSizes, false);
    }
    catch (Exception e) {
      Log.log(Log.CLT, Log.EXCP, this.getClass().getName(),
              "SystemLogTabbedPane", e.toString());
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
         systemLogTopPanel.setDisplayedHostname(selectedHosts[0]);
         clearTable();
         displaySystemLogINIFile(selectedHosts[0]);
         systemLogTopPanel.disableRefreshButton();
         (NCPFrame.getHandle()).setCursorType(Cursor.DEFAULT_CURSOR);
      }
    }
    catch (Exception e) {
      Log.log(Log.CLT, Log.EXCP, this.getClass().getName(), "refresh",
              e.toString());
      (NCPFrame.getHandle()).setCursorType(Cursor.DEFAULT_CURSOR);
    }
  }

  // Reads the system log INI file into the table
  private void updateTable()
  {
    try {
      final String SYSTEM_LOG_PROFILE_KEY = "S";
      final String RECORD_COUNT_PROFILE_TAG = "RECORD_COUNT";
      final int DEFAULT_INT = -1;
      final String DEFAULT_STRING = "  ";
      final String[] profileKeyStrings = {"A","B","C","D","E","F"};
      final int numberOfColumns = 6;
      String localHostName;
      String iniFilename;
      String keyString;
      String entry;
      INI myini = new INI();
      Vector row;
      int numberOfEntries;
      int entryNumber;
      Integer entryInteger;

      localHostName = InetAddress.getLocalHost().getHostName();
      iniFilename = SYSTEM_LOG_FILENAME + localHostName;

      numberOfEntries =  myini.GetProfileInt(SYSTEM_LOG_PROFILE_KEY,
                                             RECORD_COUNT_PROFILE_TAG,
                                             DEFAULT_INT,iniFilename);

      for (int index1 = 1; index1 <= numberOfEntries; index1++) {
        row = new Vector();
        keyString = new String(SYSTEM_LOG_PROFILE_KEY + " " + index1);

        for (int index2 = 0; index2 < numberOfColumns; index2++) {
          if ((index2 == 0) || (index2 ==  4)){
            entryNumber = myini.GetProfileInt(keyString,
                                              profileKeyStrings[index2],
                                              DEFAULT_INT, iniFilename);
            entryInteger = new Integer(entryNumber);
            row.addElement(entryInteger);
          } else {
            entry = myini.GetProfileString(keyString, profileKeyStrings[index2],
                                           DEFAULT_STRING, iniFilename);
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

  // Gets, reads, and displays the System log  in the table for the
  //  given host
  private void displaySystemLogINIFile(String host) {
    try {
      final String PARAM_SYSTEM_LOG_FILENAME = "param_system_log.ini";
      String localHostName;
      String localReturnedIniFilename;
      String localSendIniFilename;

      localHostName = InetAddress.getLocalHost().getHostName();
      localReturnedIniFilename = SYSTEM_LOG_FILENAME + localHostName;
      localSendIniFilename = PARAM_SYSTEM_LOG_FILENAME;

      // Build the file which the server will use to create the system log
      //  INI file
      if (!buildParameterFile(host, localReturnedIniFilename,localSendIniFilename))
        return;

      // Get the remote system log INI file
      if ((NtcssServersInfo.getHandle()).getSystemLogINIFile(host,
                                      PARAM_SYSTEM_LOG_FILENAME,
                                      localReturnedIniFilename) == true) {

        // Read the INI file into the table
        updateTable();

        // Remove the INI file
        (new File(PARAM_SYSTEM_LOG_FILENAME)).delete();
        (new File(localReturnedIniFilename)).delete();
      }
    }
    catch (Exception e) {
      Log.log(Log.CLT, Log.EXCP, this.getClass().getName(),
              "displaySystemLogINIFile", e.toString());
    }
  }

  // Builds the parameter file to be used by unix_ini on the server
  private boolean buildParameterFile(String host, String localReturnedIniFilename,
                                     String localSendIniFilename) {
    try {
      final String SYSLOG_FILTER_PROFILE_TAG="SYSLOG_FILTER";
      final String ENTITY_FILTERS_PROFILE_TAG="ENTITY_FILTERS";
      final String NTCSS_INI_PARAMETERS_PROFILE_TAG="NTCSS_INI_PARAMETERS";
      String returnIniFilename;
      String maxEntriesString;
      String serverBlockSizeString;
      String systemLogFilename = "/var/adm/syslog/syslog.log";
      String remoteINIDir = (NtcssServersInfo.getHandle()).getINIDirectory(host);

      INI myini = new INI();

      int myInt;
      int maxEntries = 0;
      int serverBlockSize = 0;

      returnIniFilename = remoteINIDir + localReturnedIniFilename;

      if (systemLogTopPanel != null) {
        maxEntries = systemLogTopPanel.getMaxEntries(-1);
        if (maxEntries<0) {
          maxEntries = 0;
        }
        serverBlockSize = systemLogTopPanel.getServerBlockSize(-1);
        if (serverBlockSize<0) {
          serverBlockSize = 0;
        }
        systemLogFilename = systemLogTopPanel.getSystemLogFilename(" ");
        if (systemLogFilename.equalsIgnoreCase(" "))
          systemLogFilename = "/var/adm/syslog/syslog.log";
      }
      maxEntriesString = Integer.toString(maxEntries);
      serverBlockSizeString = Integer.toString(serverBlockSize);

      myInt = myini.WriteProfileString(SYSLOG_FILTER_PROFILE_TAG,"SYSLOG_FILE",systemLogFilename,localSendIniFilename);
      if (myInt < 0) {
        return false;
      }
      myInt = myini.WriteProfileString(SYSLOG_FILTER_PROFILE_TAG,"BLOCK_SIZE",serverBlockSizeString,localSendIniFilename);
      if (myInt < 0) {
        return false;
      }
      myInt = myini.WriteProfileString(SYSLOG_FILTER_PROFILE_TAG,"MAX_RECORD_COUNT",maxEntriesString,localSendIniFilename);
      if (myInt < 0) {
        return false;
      }


      myInt = myini.WriteProfileString(SYSLOG_FILTER_PROFILE_TAG,
                                       "DATE_FILTER_CODE",
                                       String.valueOf(systemLogTopPanel.getDateFilterCode()),
                                       localSendIniFilename);
      if (myInt < 0) {
        return false;
      }

      myInt = myini.WriteProfileString(SYSLOG_FILTER_PROFILE_TAG, "START_DATE",
                                       systemLogTopPanel.getFilterStartDate(),
                                       localSendIniFilename);
      if (myInt < 0) {
        return false;
      }

      myInt = myini.WriteProfileString(SYSLOG_FILTER_PROFILE_TAG, "END_DATE",
                                       systemLogTopPanel.getFilterEndDate(),
                                       localSendIniFilename);
      if (myInt < 0) {
        return false;
      }

      boolean[] selectedEntities = systemLogTopPanel.getEntitySelections();
      if (selectedEntities == null) {
        myInt = myini.WriteProfileString(SYSLOG_FILTER_PROFILE_TAG,"ENTITY_FILTER_CODE","0",localSendIniFilename);
        if (myInt < 0) {
          return false;
        }
        for (int index=0; index<entityNames.length; index++) {
          myInt = myini.WriteProfileString(ENTITY_FILTERS_PROFILE_TAG,entityNames[index],"1",localSendIniFilename);
          if (myInt < 0) {
            return false;
          }
        }
      } else {
        myInt = myini.WriteProfileString(SYSLOG_FILTER_PROFILE_TAG,"ENTITY_FILTER_CODE","1",localSendIniFilename);
        if (myInt < 0) {
          return false;
        }
        for (int index = 0; index < entityNames.length; index++) {
          if (selectedEntities[index] == true) {
            myInt = myini.WriteProfileString(ENTITY_FILTERS_PROFILE_TAG,entityNames[index],"1",localSendIniFilename);
            if (myInt < 0) {
              return false;
            }
          }

        }
      }

      myInt = myini.WriteProfileString(NTCSS_INI_PARAMETERS_PROFILE_TAG,"OUTPUT_INI_FILENAME",returnIniFilename,localSendIniFilename);
      if (myInt < 0) {
        return false;
      }
      myInt = myini.WriteProfileString(NTCSS_INI_PARAMETERS_PROFILE_TAG,"CONTENT_CODE","06",localSendIniFilename);
      if (myInt < 0) {
        return false;
      }

      myini.FlushINIFile(localSendIniFilename);
      return true;
    }
    catch (Exception e) {
      Log.log(Log.CLT, Log.EXCP, this.getClass().getName(), "buildParameterFile",
              e.toString());
      return false;
    }
  }

  // Repaints menu
  public void updateMenu()
  {
    try {
      (NCPFrame.getHandle()).showMenu(NCPMenuBar.SYSTEM_LOG_MENU);
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
     printableString = "Host" + itemSeperator + hosts[0] + lineSeperator;

     // Add the system log filename
     printableString += "System Log Filename" + itemSeperator +
                        systemLogTopPanel.getSystemLogFilename("?") +
                        lineSeperator + lineSeperator;

     // Add the headers
     printableString += paddedString(columnHeadings[0], 7);  // Entry
     printableString += paddedString(columnHeadings[1], 16); // Date
     printableString += paddedString(columnHeadings[2], 12); // Host
     printableString += paddedString(columnHeadings[3], 10); // Entity
     printableString += paddedString(columnHeadings[4], 7);  // PID
     printableString += columnHeadings[5];                   // Event
     printableString += lineSeperator;

     // Loop through the table items and concat them to the printable string
     for (int row = 0; row < numRows; row++) {

        // Add the entry
        printableString += paddedString(
                              ((Integer)getTableItemAt(row, 0)).toString(), 7);

        // Add the date
        printableString += paddedString((String)getTableItemAt(row, 1), 16);

        // Add the host
        printableString += paddedString((String)getTableItemAt(row, 2), 12);

        // Add the entity
        printableString += paddedString((String)getTableItemAt(row, 3), 10);

        // Add the PID
        printableString += paddedString(
                              ((Integer)getTableItemAt(row, 4)).toString(), 7);

        // Add the event
        printableString += (String)getTableItemAt(row, 5);

        // If it is not the last row in the table, add a row seperator
        if (row != (numRows -1))
           printableString += lineSeperator;
     }

     return printableString;
  }
}

