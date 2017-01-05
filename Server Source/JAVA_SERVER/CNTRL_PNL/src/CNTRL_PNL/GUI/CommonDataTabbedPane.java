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

public class CommonDataTabbedPane extends TabbedPaneWithTable
{
  private CommonDataTopPanel commonDataTopPanel;
  private Vector errorRow;
  private String[] columnHeadings = {"Tag", "Value"};

  public CommonDataTabbedPane()
  {
     // Create and add the panel that is to go at the top of the tabbed pane
     commonDataTopPanel = new CommonDataTopPanel();
     addTopPanel(commonDataTopPanel);

     // Create the table that is to go at the bottom of the tabbed pane
     int[] columnWidths = {250,375};
     setupTable(columnHeadings, columnWidths, false);

     // Create a row with Error strings for the columns of the table
     errorRow = new Vector();
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
     String[] selectedHosts = getCurrentHosts();

     if (selectedHosts != null) {
        (NCPFrame.getHandle()).setCursorType(Cursor.WAIT_CURSOR);

        // Refresh the information in the top panel
        commonDataTopPanel.refresh(selectedHosts[0]);

        // Refresh the data in the table
        refreshTable();

        (NCPFrame.getHandle()).setCursorType(Cursor.DEFAULT_CURSOR);
     }
  }

  // Gets, reads, and displays the information for the table
  private void refreshTable()
  {
     String[] selectedHosts = getCurrentHosts();

     // Clear the current contents of the table
     clearTable();

     // Get updated data and display it in the table
     displayCommonDataINIFile(selectedHosts[0]);
  }

  // Gets, reads, and displays the common data in the table for the
  //  given host
  private void displayCommonDataINIFile(String host)
  {
     String localFilename = "tmp.ini";

     // get the remote common data INI file
     if ((NtcssServersInfo.getHandle()).getCommonDataINIFile(host,
                                                      localFilename) == true) {

        // Read the INI file into the table
        displayINIFile(localFilename);

        // remove the INI file
        (new File(localFilename)).delete();
     }
     else
        addTableRow(errorRow);
  }

  // Given the name of a common data INI file, this function will display its
  //  contents in the GUI table
  private void displayINIFile(String filename)
  {
     INI ini;
     StringTokenizer sectionTokenizer, itemTokenizer;
     String output, sectionName, token;
     Vector row;

     ini = new INI();

     // Get the section name which contains the common data. This should
     //  be the first (and only) section in the file
     output = ini.GetProfileSectionNames(filename);
     sectionName = output.substring(0, output.indexOf(INI.STRINGS_DELIMITER));

     // Get the common data section
     output = ini.GetProfileSection(sectionName, filename);

     // Parse through the section and add the tag/value pairs to the table
     sectionTokenizer = new StringTokenizer(output, INI.STRINGS_DELIMITER);
     while (sectionTokenizer.hasMoreTokens()) {
        token = sectionTokenizer.nextToken();
        itemTokenizer = new StringTokenizer(token, INI.TAG_VALUE_DELIMITER);
        row = new Vector();
        row.addElement(itemTokenizer.nextToken());  // Tag
        row.addElement(itemTokenizer.nextToken());  // Value
        addTableRow(row);
     }
  }

  // Repaints menu to show the Common Data menu
  public void updateMenu()
  {
     try {
        (NCPFrame.getHandle()).showMenu(NCPMenuBar.COMMON_DATA_MENU);
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
      printableString += paddedString(columnHeadings[0], 30);  // Tag
      printableString += columnHeadings[1];                    // Value
      printableString += lineSeperator;

      // Loop through the table items and concat them to the printable string
      for (int row = 0; row < numRows; row++) {
         // Add the tag
         printableString += paddedString((String)getTableItemAt(row, 0), 30);

         // Add the value
         printableString += (String)getTableItemAt(row, 1);

         // If it is not the last row in the table, add a row seperator
         if (row != (numRows -1))
            printableString += lineSeperator;
      }

      return printableString;
   }
}

