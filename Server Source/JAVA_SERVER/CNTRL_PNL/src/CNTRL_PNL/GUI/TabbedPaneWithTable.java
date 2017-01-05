/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package CNTRL_PNL.GUI;

import javax.swing.*;
import javax.swing.table.*;
import java.awt.*;
import java.util.*;
import java.lang.*;

import CNTRL_PNL.INFO.*;
import COMMON.*;

abstract public class TabbedPaneWithTable extends TabbedPaneBase
{
  private JSplitPane    splitPane;
  private JScrollPane   scrollPane;
  private NCPTableModel model;
  private TableSorter   sorter;
  private JTable        table;

  public TabbedPaneWithTable()
  {
    setLayout(new BorderLayout());

    // Set up the scroll pane which will be the bottom pane in the split pane
    scrollPane = new JScrollPane();

    // Setup the split pane and add the scroll pane to the bottom of it
    splitPane = new JSplitPane();
    splitPane.setDividerSize(1);
    splitPane.setOrientation(JSplitPane.VERTICAL_SPLIT);
    splitPane.setBottomComponent(scrollPane);
    splitPane.setBackground(Color.white);

    add(splitPane, BorderLayout.CENTER);
  }

  // Inserts the given panel into the top half of the split pane.
  //  This panel will probably be a member of the class derived from this
  //  class.
  protected void addTopPanel(JPanel panel)
  {
     splitPane.setTopComponent(panel);
  }

  // Adds a row to the table that is displayed on this panel
  protected void addTableRow(Vector rowData)
  {
     model.addRow(rowData);
  }

  // Clears the items displayed in the table on this panel
  protected void clearTable()
  {
     model.clearTable();
  }

  // Retrieves from the table the item at row, column
  protected Object getTableItemAt(int row, int column)
  {
     return model.getValueAt(row, column);
  }

  // Returns the current number of columns in the table
  protected int getTableColumnCount()
  {
     return model.getColumnCount();
  }

  // Returns the current number of rows in the table
  protected int getTableRowCount()
  {
     return model.getRowCount();
  }

  // Used to setup the number of columns and column headings for
  //  the table in the lower half of the split pane and display it
  protected void setupTable(String[] headings, int[] columnWidths,
                              boolean allowRowSelection)
  {    
     model = new NCPTableModel(headings);
     sorter = new TableSorter(model);
     table = new JTable(sorter);

     //Set up column sizes.
     initColumnSizes(columnWidths);

     sorter.addMouseListenerToHeaderInTable(table);
     table.setPreferredScrollableViewportSize(new Dimension(500, 70));
     table.setAutoResizeMode(JTable.AUTO_RESIZE_OFF);

     // Allow/disallow the ability to select rows in the table
     table.setRowSelectionAllowed(allowRowSelection);

     // Default it so that the grid lines in the table are not displayed
     table.setShowHorizontalLines(false);
     table.setShowVerticalLines(false);

     // Make the table column headings visible in a scrollable pane.
     scrollPane.setColumnHeaderView(table.getTableHeader());
     scrollPane.getViewport().add(table, null);
  }

  // If the given parameter is true, then horizontal lines will be displayed
  //  between the rows in the table.
  protected void showTableHorizontalLines(boolean show)
  {
    table.setShowHorizontalLines(show);
    table.repaint();
  }

  // If the given parameter is true, then vertical lines will be displayed
  //  between the columns in the table.
  protected void showTableVerticalLines(boolean show)
  {
    table.setShowVerticalLines(show);
    table.repaint();
  }

  // This method picks good column sizes. If all column heads are wider than
  //  the column's cells' contents, then you can just use
  //  column.sizeWidthToFit().
  private void initColumnSizes(int[] columnWidths)
  {
    TableColumn column = null;
    Component comp = null;
    int headerWidth = 0;
    int cellWidth = 0;    
    JTableHeader th = new JTableHeader(table.getColumnModel());
    TableCellRenderer tcr = th.getDefaultRenderer();

    for (int i = 0; i < model.getColumnCount(); i++) {

      column = table.getColumnModel().getColumn(i);
      
      comp = tcr.getTableCellRendererComponent(table, column.getHeaderValue(),
                                                false, false, 0, 0);            
      //comp = column.getHeaderRenderer().
      //            getTableCellRendererComponent(table, column.getHeaderValue(),
      //                                          false, false, 0, 0);         

      headerWidth = comp.getPreferredSize().width;

      column.setMinWidth(Math.max(headerWidth, columnWidths[i]));

    }

    table.sizeColumnsToFit(JTable.AUTO_RESIZE_ALL_COLUMNS);      
  }
}
