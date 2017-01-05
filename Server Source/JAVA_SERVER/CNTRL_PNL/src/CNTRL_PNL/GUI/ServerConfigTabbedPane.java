/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package CNTRL_PNL.GUI;

import javax.swing.table.*;
import javax.swing.*;
import javax.swing.event.*;
import java.awt.*;
import java.util.*;
import java.awt.event.*;

import CNTRL_PNL.INFO.*;
import CNTRL_PNL.COMMON.*;

public class ServerConfigTabbedPane extends TabbedPaneBase
{
   private ServerConfigTableModel tableModel;
   private ServerConfigTable table;
   private JTextField domainField;

   // Constructor
   public ServerConfigTabbedPane()
   {
      JScrollPane scrollPane;
      JButton applyButton;
      JButton removeButton;
      JButton clearButton;
      final int tableWidth = 500;
      final int tableHeight = 250;
      JPanel buttonPanel = new JPanel();
      JPanel domainPanel = new JPanel();
      JLabel domainLabel = new JLabel("Domain Name");

      // Create the table model and the table
      tableModel = new ServerConfigTableModel();
      table = new ServerConfigTable(tableModel);
      table.setPreferredScrollableViewportSize(
                                     new Dimension(tableWidth, tableHeight));

      // Create the buttons
      clearButton = new JButton("Clear");
      clearButton.addActionListener(new java.awt.event.ActionListener()
      {
         public void actionPerformed(ActionEvent e)
         {
            clear();
         }
      });
      removeButton = new JButton("Remove");
      removeButton.addActionListener(new java.awt.event.ActionListener()
      {
         public void actionPerformed(ActionEvent e)
         {
            remove();
         }
      });
      applyButton = new JButton("Apply");
      applyButton.addActionListener(new java.awt.event.ActionListener()
      {
         public void actionPerformed(ActionEvent e)
         {
            apply();
         }
      });
      buttonPanel.add(clearButton);
      buttonPanel.add(removeButton);
      buttonPanel.add(applyButton);

      // Create the domain name text field
      domainField = new JTextField(30);
      domainPanel.add(domainLabel);
      domainPanel.add(domainField);

      // Create the scroll pane and add the table to it.
      scrollPane = new JScrollPane(table);

      // Add the components to this window.
      add(domainPanel, BorderLayout.NORTH);
      add(scrollPane, BorderLayout.CENTER);
      add(buttonPanel, BorderLayout.SOUTH);
   }

   // Called when a host selection is made from the host list panel
   public void hostSelectionMade()
   {
      String[] hostList = getCurrentHosts();

      // Cycle through the list of hosts selected and add their data to the
      //  table
      for (int host = 0; host < hostList.length; host++)
         tableModel.insert((NCPFrame.getHandle()).getHostInfo(hostList[host]));
   }

   // Repaints the menu so that this pane's action menu is included
   public void updateMenu()
   {
      (NCPFrame.getHandle()).showMenu(NCPMenuBar.SERVER_CONFIG_MENU);
   }

   // Obtains from the pane a string that represents all the information that
   //  is displayed in that pane.
   public String getPrintableMaterial()
   {
      String printableString = null;
      final String lineSeperator = "\n";
      String data[][] = tableModel.getData();
      int length;
      String dashes;

      // Add the domain name to the printable string
      printableString = "Domain: " + domainField.getText() + lineSeperator +
                        lineSeperator;

      // Add the column names to the printable string
      for (int i = 0; i < tableModel.getColumnCount(); i++)
         printableString += paddedString(tableModel.getColumnName(i), 20);
      printableString += lineSeperator;

      // Add dashes under the column headings
      for (int i = 0; i < tableModel.getColumnCount(); i++) {
         length = (tableModel.getColumnName(i)).length();
         dashes = "-";
         for (int j = 1; j < length; j++)
            dashes += "-";
         printableString += paddedString(dashes, 20);
      }
      printableString += lineSeperator;

      // Add the data to the printable string
      for (int r = 0; r < data.length; r++) {
         for (int c = 1; c < data[0].length; c++)
            printableString += paddedString(data[r][c], 20);
         printableString += lineSeperator;
      }

      return printableString;
   }

   // This function is executed when the "clear" action is selected
   public void clear()
   {
      domainField.setText("");   // Clears the domain name text field
      tableModel.clear();        // Removes all the entries from the table
      table.clearSelection();
   }

   // This function is executed when the "remove" action is selected
   public void remove()
   {
      int selectedRows[] = table.getSelectedRows();

      if (selectedRows.length <= 0)
         JOptionPane.showConfirmDialog(NCPFrame.getHandle(),
                                       "No host selected", "Remove error",
                                       JOptionPane.DEFAULT_OPTION,
                                       JOptionPane.ERROR_MESSAGE);
      else {
         if (tableModel.remove(selectedRows) == true)
            table.clearSelection();
      }
   }

   // This function is executed when the "apply" action is selected
   public void apply()
   {
      String data[][];
      String domainname;
      int masterIndex = -1;

      // Get the data from the table
      data = tableModel.getData();

      // Make sure hosts have been selected
      if (data.length <= 0) {
         JOptionPane.showConfirmDialog(NCPFrame.getHandle(),
                                       "No hosts have been selected",
                                       "Apply error",
                                       JOptionPane.DEFAULT_OPTION,
                                       JOptionPane.ERROR_MESSAGE);
         return;
      }

      // Get the requested domain name and make sure it is filled in
      domainname = domainField.getText();
      if ((domainname.trim()).length() <= 0) {
         JOptionPane.showConfirmDialog(NCPFrame.getHandle(),
                                   "Domain Name field is empty", "Apply error",
                                       JOptionPane.DEFAULT_OPTION,
                                       JOptionPane.ERROR_MESSAGE);
         return;
      }

      // Look through the data to make sure one and only one host was selected
      //  as NTCSS master
      for (int i = 0; i < data.length; i++) {
         if (data[i][3].equalsIgnoreCase("Master")) {
            if (masterIndex != -1) { // i.e. a host has already been selected
               masterIndex = -1;     //  as Master
               break;
            }
            else
               masterIndex = i;
         }
      }
      if (masterIndex == -1) {   // i.e. one NTCSS master wasn't selected
         JOptionPane.showConfirmDialog(NCPFrame.getHandle(),
                          "One host must be selected as the NTCSS Master",
                          "Apply error",
                          JOptionPane.DEFAULT_OPTION,
                          JOptionPane.ERROR_MESSAGE);
         return;
      }

      // Make sure the user wants to apply the changes
      if (JOptionPane.showConfirmDialog(NCPFrame.getHandle(),
                                "Are you sure you want to create this domain?",
                                "Apply Confirmation",
                                JOptionPane.YES_NO_OPTION,
                                JOptionPane.WARNING_MESSAGE)
                                                   != JOptionPane.YES_OPTION) {
         return;
      }

      // Make sure the user REALLY wants to apply the changes
      if (JOptionPane.showConfirmDialog(NCPFrame.getHandle(),
                        "Continuing will force the host(s) to reconfigure!\n" +
                             "Do you want to continue?",
                             "Apply Confirmation",
                             JOptionPane.YES_NO_OPTION,
                             JOptionPane.WARNING_MESSAGE)
                                                   != JOptionPane.YES_OPTION) {
            return;
      }

      // Change the cursor to the wait cursor
      (NCPFrame.getHandle()).setCursorType(Cursor.WAIT_CURSOR);

      // Replace the string version of the server types with the constant
      //  integer representation
      for (int i = 0; i < data.length; i++) {
         if (data[i][3].equalsIgnoreCase("Master"))
            data[i][3] = String.valueOf(Constants.MASTER_SVR);
         else if (data[i][3].equalsIgnoreCase("Authentication"))
            data[i][3] = String.valueOf(Constants.AUTH_SVR);
         else if (data[i][3].equalsIgnoreCase("Application"))
            data[i][3] = String.valueOf(Constants.APP_SVR);
         else
            data[i][3] = String.valueOf(Constants.UNKNOWN_SVR_TYPE);
      }

      // Send a message to the host selected as the NTCSS master with a list of
      //  the selected hosts and their information
      (NtcssServersInfo.getHandle()).makeNTCSSMasterSvr(data[masterIndex][0],
                                                        data[masterIndex][1],
                                                        data[masterIndex][2],
                                                        domainname,
                                                        data);

      // Send a message to all of the other hosts to notify them to make the
      //  requested changes
      (NtcssServersInfo.getHandle()).reconfigureSlaves(data, domainname,
                                                       masterIndex);

      // Recycle the NTCSS daemons on the new master server
      (NtcssServersInfo.getHandle()).recycleNtcssDaemons(data[masterIndex][1]);

      // Change the cursor to the default cursor
      (NCPFrame.getHandle()).setCursorType(Cursor.DEFAULT_CURSOR);

      // Refresh the displayed host list
      (NCPFrame.getHandle()).refreshHostList();
   }
}

// This is the table model for the table which is displayed in the
//  Server Config tab
class ServerConfigTableModel extends AbstractTableModel
{
   // The names of the columns in the table
   private final String[] columnNames = {"Hostname",
                                         "IP Address",
                                         "NTCSS Server Type"};

   // This contains all the data represented in the table cells
   private Vector table;
   private int nextEmptyRow = 0;
   private final int minNumRows = 20;

   // Constructor
   public ServerConfigTableModel()
   {
      // Initialize the structure which will actually hold the data for
      //  the table
      table = new Vector(minNumRows);
      for (int r = 0; r < minNumRows; r++)
         table.addElement(emptyRow());
   }


   // Returns the value of the object at the specified row and column in the
   //  table
   public Object getValueAt(int row, int col)
   {
      return ((Vector)table.elementAt(row)).elementAt(col + 1);
   }

   // Returns the number of columns in the table
   public int getColumnCount()
   {
      return columnNames.length;
   }

   // Returns the number of rows in the table
   public int getRowCount()
   {
      return table.size();
   }

   // Returns the name of the column at the specified index
   public String getColumnName(int col)
   {
      return columnNames[col];
   }

   // Returns true if the specified cell is editable or not
   public boolean isCellEditable(int row, int col)
   {
      // If the row selected by the user does not have data for a host
      //  in it, do not allow any of the fields to be edited
      if (row >= nextEmptyRow)
         return false;
      else
         return true;
   }

   // Sets the value at the specified cell
   public void setValueAt(Object value, int row, int col)
   {
      ((Vector)table.elementAt(row)).setElementAt(value, col + 1);

      // Inform the table that a cell has changed so that it can redraw itself
      fireTableDataChanged();
   }

   // Returns an empty row which can be inserted into the table structure
   private Vector emptyRow()
   {
      final int numColumns = getColumnCount() + 1;
      Vector row = new Vector(numColumns);

      for (int c = 0; c < numColumns; c++)
         row.addElement(" ");

      return row;
   }

   // Clears all the entries from the table
   public void clear()
   {
      // Remove all the entries from the table
      for (int i = 0; i < nextEmptyRow; i++) 
            table.removeElementAt(0);

      // Add empty rows at the bottom of the table. I couldn't get the
      //  background of the table to be white, so enough empty rows are
      //  added to the end of the table to give the display the right
      //  color.
      for (int r = getRowCount(); r < minNumRows; r++)
         table.addElement(emptyRow());

      nextEmptyRow = 0;

      // Tell the table the data has changed
      fireTableDataChanged();
   }

   // Adds a row to the end of the table structure
   public void insert(HostInfoListEntry rowData)
   {
      Vector row;
      int serverType;

      if (isHostAlreadyInTable(rowData.getHostname()) == false) {
         row = new Vector(getColumnCount() + 1);
         serverType = rowData.getServerType();

         row.addElement(rowData.getHostname());           // Hostname

         row.addElement(rowData.getHostname());           // Hostname

         row.addElement(rowData.getIPaddress());          // IP Address

         if (serverType == Constants.MASTER_SVR)          // NTCSS Server Type
            row.addElement("Master");
         else if (serverType == Constants.AUTH_SVR)
            row.addElement("Authentication");
         else if (serverType == Constants.APP_SVR)
            row.addElement("Application");
         else
            row.addElement(" ");

         // Insert the row into the table
         if (nextEmptyRow < getRowCount())
            table.setElementAt(row, nextEmptyRow); // Just overwrite "empty" row
         else
            table.addElement(row);                 // Add to end of vector

         nextEmptyRow++;

         // Indicate to the table object that the table model has been updated
         fireTableDataChanged();
      }
   }

   // Removes the specified row from the table structure. The parameter
   //  passed in is an array containing row indices that are to be deleted.
   public boolean remove(int[] rows)
   {
      int row;
      boolean rowRemoved = false;

      // Loop through the list of the rows passed in which are to be removed
      for (int w = 0; w < rows.length; w++) {

         // Determine which row number to remove
         row = rows[w] - w;
         if ((row < nextEmptyRow) && (row >= 0)) {

            // Remove the row
            table.removeElementAt(row);
            nextEmptyRow--;
            rowRemoved = true;

            // Add empty rows at the bottom of the table. I couldn't get the
            //  background of the table to be white, so enough empty rows are
            //  added to the end of the table to give the display the right
            //  color.
            for (int r = getRowCount(); r < minNumRows; r++)
               table.addElement(emptyRow());

            // Indicate to the table object that the table model has
            //  been updated
            fireTableDataChanged();
         }
      }

      return rowRemoved;
   }

   // Checks to see if this host is already in the table
   private boolean isHostAlreadyInTable(String host)
   {
      Vector row;
      boolean found = false;

      for (int r = 0; r < getRowCount(); r++) {
         row = (Vector)table.elementAt(r);
         if (host.equalsIgnoreCase((String)row.elementAt(0))) {
            found = true;
            break;
         }
      }

      return found;
   }

   // This function returns the data in the table in a 2D array
   public String[][] getData()
   {
      String data[][] = new String[nextEmptyRow][getColumnCount() + 1];

      // Collect the data from the table
      for (int r = 0; r < nextEmptyRow; r++)
         for (int c = 0; c < getColumnCount() + 1; c++)
            data[r][c] = (String)((Vector)table.elementAt(r)).elementAt(c);

      return data;
   }
}

// This is the table which is displayed in the panel. It extends JTable
//  just so there was a clean way to set up the "special" columns (the
//  columns which use combo boxes as their renderers).
class ServerConfigTable extends JTable
{
   // Constructor
   public ServerConfigTable(TableModel tm)
   {
      super(tm);

      // Default it so that the grid lines in the table are not displayed
      setShowGrid(false);

      // Setup the columns which use combo boxes for their cells
      setUpNtcssHostTypeColumn(getColumnModel().getColumn(2));
   }

   // Sets up the Ntcss Host Type column so that its cells are a combo box
   private void setUpNtcssHostTypeColumn(TableColumn column)
   {
      JComboBox comboBox = new JComboBox();
      DefaultTableCellRenderer renderer = new DefaultTableCellRenderer();

      // Set up the editor for the cells in the "Ntcss Host Type" column
      comboBox.addItem("Master");
      comboBox.addItem("Authentication");
      column.setCellEditor(new DefaultCellEditor(comboBox));

      // Set up tool tips for the cells in this column
      renderer.setToolTipText("Click for server types");
      column.setCellRenderer(renderer);
   }
}
