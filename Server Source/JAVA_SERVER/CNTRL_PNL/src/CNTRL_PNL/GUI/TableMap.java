/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

/**
 * http://java.sun.com/docs/books/tutorial/uiswing/components/example-swing/TableMap.java
 *
 * In a chain of data manipulators some behaviour is common. TableMap
 * provides most of this behavour and can be subclassed by filters
 * that only need to override a handful of specific methods. TableMap
 * implements TableModel by routing all requests to its model, and
 * TableModelListener by routing all events to its listeners. Inserting
 * a TableMap which has not been subclassed into a chain of table filters
 * should have no effect.
 *
 * @version 1.4 12/17/97
 * @author Philip Milne
 */

package CNTRL_PNL.GUI;

import javax.swing.table.*;
import javax.swing.event.TableModelListener;
import javax.swing.event.TableModelEvent;

import CNTRL_PNL.COMMON.*; 

public class TableMap extends AbstractTableModel implements TableModelListener
{
  protected TableModel model;

  public TableModel  getModel() {
    try {
      return model;
    }
    catch (Exception e) {
      Log.log(Log.CLT, Log.EXCP, this.getClass().getName(), "TableModel",
              e.toString());
      return null;
    }
  }

  public void  setModel(TableModel model) {
    try {
      this.model = model;
      model.addTableModelListener(this);
    }
    catch (Exception e) {
      Log.log(Log.CLT, Log.EXCP, this.getClass().getName(), "setModel",
              e.toString());
    }
  }

  // By default, Implement TableModel by forwarding all messages
  // to the model.

  public Object getValueAt(int aRow, int aColumn) {
    try {
      return model.getValueAt(aRow, aColumn);
    }
    catch (Exception e) {
      Log.log(Log.CLT, Log.EXCP, this.getClass().getName(), "getValueAt",
              e.toString());
      return null;
    }
  }

  public void setValueAt(Object aValue, int aRow, int aColumn) {
    try {
      model.setValueAt(aValue, aRow, aColumn);
    }
    catch (Exception e) {
      Log.log(Log.CLT, Log.EXCP, this.getClass().getName(), "setValueAt",
              e.toString());
    }
  }

  public int getRowCount() {
    try {
      return (model == null) ? 0 : model.getRowCount();
    }
    catch (Exception e) {
      Log.log(Log.CLT, Log.EXCP, this.getClass().getName(), "getRowCount",
              e.toString());
      return 0;
    }

  }

  public int getColumnCount() {
    try {
      return (model == null) ? 0 : model.getColumnCount();
    }
    catch (Exception e) {
      Log.log(Log.CLT, Log.EXCP, this.getClass().getName(), "getColumnCount",
              e.toString());
      return 0;
    }
  }

  public String getColumnName(int aColumn) {
    try {
      return model.getColumnName(aColumn);
    }
    catch (Exception e) {
      Log.log(Log.CLT, Log.EXCP, this.getClass().getName(), "getColumnName",
              e.toString());
      return null;
    }
  }

  public Class getColumnClass(int aColumn) {
    try {
      return model.getColumnClass(aColumn);
    }
    catch (Exception e) {
      Log.log(Log.CLT, Log.EXCP, this.getClass().getName(), "getColumnClass",
              e.toString());
      return null;
    }
  }

  public boolean isCellEditable(int row, int column) {
    try {
      return model.isCellEditable(row, column);
    }
    catch (Exception e) {
      Log.log(Log.CLT, Log.EXCP, this.getClass().getName(), "isCellEditable",
              e.toString());
      return false;
    }
  }

  //
  // Implementation of the TableModelListener interface,
  //
  // By default forward all events to all the listeners.
  public void tableChanged(TableModelEvent e) {
    try {
      fireTableChanged(e);
    }
    catch (Exception excep) {
      Log.log(Log.CLT, Log.EXCP, this.getClass().getName(), "tableChanged",
              excep.toString());
    }
  }
}
