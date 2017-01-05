/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package CNTRL_PNL.GUI;

import javax.swing.JTable;
import javax.swing.table.AbstractTableModel;
import javax.swing.JScrollPane;
import javax.swing.JFrame;
import javax.swing.SwingUtilities;
import java.awt.*;
import java.awt.event.*;
import java.util.*;

public class NCPTableModel extends AbstractTableModel
{
  private String[] columnNames;
  private Vector rowData;
  private int numColumns;
  private int numRows;

  public NCPTableModel(String[] columnHeadings)
  {
    columnNames = columnHeadings;
    rowData = new Vector();
    numColumns = columnNames.length;
    numRows = 0;
  }

  public int getColumnCount()
  {
    return columnNames.length;
  }

  public String getColumnName(int col)
  {
    return columnNames[col];
  }

  public Object getValueAt(int row, int col)
  {
    Vector selectedRowData = new Vector();

    selectedRowData = (Vector)rowData.elementAt(row);

    return selectedRowData.elementAt(col);
  }

  public int getRowCount()
  {
    return numRows;
  }

  /*
   * JTable uses this method to determine the default renderer/
   * editor for each cell.  If we didn't implement this method,
   * then the last column would contain text ("true"/"false"),
   * rather than a check box.
   */                    
  public Class getColumnClass(int c)
  {
    return getValueAt(0, c).getClass();
  }

  public void addRow(Vector newRowData)
  {
    if (newRowData == null) {
      newRowData = new Vector(getColumnCount());
    } else {
      newRowData.setSize(getColumnCount());
    }
    rowData.addElement(newRowData);
    ++numRows;
    fireTableRowsInserted(numRows-1, numRows);
  }

  public void clearTable()
  {
    fireTableRowsDeleted(0,numRows);
    rowData = new Vector();
    numRows = 0;
  }
}

