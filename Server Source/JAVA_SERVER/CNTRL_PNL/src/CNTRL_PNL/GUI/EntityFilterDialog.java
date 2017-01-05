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
import java.beans.*;
import java.util.*;
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.table.*;
import javax.swing.plaf.metal.*;

import COMMON.*;

public class EntityFilterDialog extends JDialog
{

  private NCPTableModel ncpTableModel;
  private TableSorter tableSorter;
  private JTable jTable;
  private JScrollPane jScrollPane;

  private final static String[] columnHeadings = {"Entity", "Description"};
  private final static int[] columnSizes = {100,300};
  private boolean[] selectedEntities;
  private String[] currentEntityNames;
  private String[] currentEntityDescriptions;
  private boolean okSelected = false;

  public EntityFilterDialog(String[] entityNames, String[] entityDescriptions)
  {
    super(NCPFrame.getHandle(), "Entity Filter", true);
 
    currentEntityNames = entityNames;
    currentEntityDescriptions = entityDescriptions;
    selectedEntities = new boolean[currentEntityNames.length];
    // Initialize the return selectedEntries based on what was selected.
    for (int index=0; index<selectedEntities.length; index++) {
      selectedEntities[index] = false;
    }

    JPanel container = new JPanel();
    JPanel filters = buildFilterPanel();
    JPanel buttonPanel = buildButtonPanel();

    container.setLayout( new BorderLayout() );
    container.setBorder(BorderFactory.createTitledBorder(""));
    container.setPreferredSize(new Dimension(420, 200));
    container.add(filters, BorderLayout.CENTER);
    container.add(buttonPanel, BorderLayout.SOUTH);

    getContentPane().add(container);
    pack();
    centerDialog();
    this.setVisible(true);
  }

  public JPanel buildFilterPanel()
  {
    Vector entityFilters;
    JPanel filters = new JPanel();
    filters.setLayout(new BorderLayout());

    jScrollPane = new JScrollPane();
    jScrollPane.setPreferredSize(new Dimension(100, 100));
    jScrollPane.setBackground(Color.white);
    jScrollPane.setMinimumSize(new Dimension(0, 0));
    filters.add(jScrollPane, BorderLayout.CENTER);
    setupTable(columnHeadings, columnSizes);

    for (int index=0; index<currentEntityNames.length; index++) {
      entityFilters = new Vector();
      entityFilters.addElement(currentEntityNames[index]);
      entityFilters.addElement(currentEntityDescriptions[index]);
      addTableRow(entityFilters);
    }

    return filters;
  }

  public JPanel buildButtonPanel()
  {
	  JPanel buttons = new JPanel();
	  buttons.setLayout ( new FlowLayout(FlowLayout.CENTER) );

	  JButton cancel = new JButton("Cancel");
	  cancel.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent e) {
				CancelPressed();
			}
    });


	  JButton ok = new JButton("OK");
	  ok.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent e) {
				OKPressed();
			}
    });

	  buttons.add( ok );
    buttons.add( cancel );

	  getRootPane().setDefaultButton(ok);
    return buttons;
  }

  protected void centerDialog()
  {
    Dimension screenSize = this.getToolkit().getScreenSize();
	  Dimension size = this.getSize();
  	screenSize.height = screenSize.height/2;
	  screenSize.width = screenSize.width/2;
  	size.height = size.height/2;
	  size.width = size.width/2;
  	int y = screenSize.height - size.height;
	  int x = screenSize.width - size.width;
  	this.setLocation(x,y);
  }

  public void CancelPressed()
  {
    for (int index=0; index<selectedEntities.length; index++) {
      selectedEntities[index] = false;
    }
    okSelected = false;
    this.setVisible(false);
  }

  public void OKPressed()
  {
    for (int index=0; index<selectedEntities.length; index++) {
      if (jTable.isRowSelected(index))
        selectedEntities[index] = true;
      else
        selectedEntities[index] = false;
    }
    okSelected = true;
    this.setVisible(false);
  }

  // Adds a row to the table on this panel
  public void addTableRow(Vector rowData)
  {
    ncpTableModel.addRow(rowData);
  }

  // Clears the table on this panel
  public void clearTable()
  {
    ncpTableModel.clearTable();
  }

  // Used to setup the number of columns and column headings for
  //  the table in the lower half of the split pane and display it
  protected void setupTable(String[] headings, int[] columnWidths)
  {    
    ncpTableModel = new NCPTableModel(headings);
    tableSorter = new TableSorter(ncpTableModel);
    jTable = new JTable(tableSorter);

    //Set up column sizes.
    initColumnSizes(jTable, ncpTableModel, columnWidths);   

    tableSorter.addMouseListenerToHeaderInTable(jTable);
    jTable.setPreferredScrollableViewportSize(new Dimension(500, 70));
    jTable.setAutoResizeMode(JTable.AUTO_RESIZE_OFF);
    jTable.setShowHorizontalLines(false);
    jTable.setShowVerticalLines(false);


    // Make the table column headers visible in a scrollable pane.
    jScrollPane.setColumnHeaderView(jTable.getTableHeader());
    jScrollPane.getViewport().add(jTable, null);
  }

  protected void showTableHorizontalLines(boolean flag)
  {
    jTable.setShowHorizontalLines(flag);
    jTable.repaint();
  }

  protected void showTableVerticalLines(boolean flag)
  {
    jTable.setShowVerticalLines(flag);
    jTable.repaint();
  }

  /*
   * This method picks good column sizes.
   * If all column heads are wider than the column's cells'
   * contents, then you can just use column.sizeWidthToFit().
   */
  private void initColumnSizes(JTable table,
      NCPTableModel ncpTableModel,
      int[] columnWidths)
  {
    TableColumn column = null;
    Component comp = null;
    int headerWidth = 0;
    int cellWidth = 0;
    
    JTableHeader th = new JTableHeader(table.getColumnModel());
    TableCellRenderer tcr = th.getDefaultRenderer();

    for (int i = 0; i < ncpTableModel.getColumnCount(); i++) {
      column = table.getColumnModel().getColumn(i);
      
      comp = tcr.getTableCellRendererComponent(table, column.getHeaderValue(),
                                                false, false, 0, 0); 
      //comp = column.getHeaderRenderer().
      //    getTableCellRendererComponent(
      //        table, column.getHeaderValue(),
      //        false, false, 0, 0);
      
      headerWidth = comp.getPreferredSize().width;
      
      column.setMinWidth(Math.max(headerWidth, columnWidths[i]));
    }
    
    table.sizeColumnsToFit(JTable.AUTO_RESIZE_ALL_COLUMNS);
  }

  public boolean[] getSelectedEntities()
  {
    return selectedEntities;
  }

  public boolean wasOkSelected()
  {
    return okSelected;
  }

}
