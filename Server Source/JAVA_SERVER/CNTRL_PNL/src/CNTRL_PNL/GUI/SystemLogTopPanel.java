/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package CNTRL_PNL.GUI;

import javax.swing.*;
import javax.swing.event.*;
import javax.swing.text.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.Component.*;
import java.lang.Object.*;
import java.util.*;
import java.lang.*;

import CNTRL_PNL.COMMON.*;

public class SystemLogTopPanel extends JPanel{

  private static JComboBox maxEntriesComboBox;
  private static JComboBox blockSizeComboBox;
  private static SystemLogTabbedPane currentBottomPane;
  private static int maximumEntries = 0;
  private static int serverBlockSize = 0;
  private JLabel selectedHost;
  private String INITIAL_SYS_LOG_FILENAME = "/var/adm/syslog/syslog.log";
  private String systemLogFilename = INITIAL_SYS_LOG_FILENAME;
  private JTextField filenameTextField;
  private JCheckBox dateTimeCheckBox;
  private JCheckBox entityCheckBox;
  private String[] currentEntityNames;
  private String[] currentEntityDescriptions;
  private boolean[] selectedEntries = null;
  private int dateFilterCode;
  private String filterStartDate, filterEndDate;
  private JButton refreshButton;

  public SystemLogTopPanel(SystemLogTabbedPane bottomPane, String[] entityNames,
                           String[] entityDescriptions)
  {
    try {
      currentBottomPane = bottomPane;
      currentEntityNames = entityNames;
      currentEntityDescriptions = entityDescriptions;

      dateFilterCode = 0;
      filterStartDate = "not set";
      filterEndDate = "not set";

      jbInit();
    }
    catch (Exception e) {
      e.printStackTrace();
    }
  }

  private void jbInit() throws Exception
  {
    try {

      // Create the components for the panel
      maxEntriesComboBox = new JComboBox();
      maxEntriesComboBox.setEditable(true);
      maxEntriesComboBox.addItem(new String("50"));
      maxEntriesComboBox.addItem(new String("100"));
      maxEntriesComboBox.addItem(new String("150"));
      maxEntriesComboBox.addItem(new String("200"));
      maxEntriesComboBox.addItem(new String("250"));
      maxEntriesComboBox.addItem(new String("300"));
      maxEntriesComboBox.addItem(new String("350"));
      maxEntriesComboBox.addItem(new String("400"));
      maxEntriesComboBox.addItem(new String("450"));
      maxEntriesComboBox.addItem(new String("500"));
      maxEntriesComboBox.getAccessibleContext().setAccessibleName("String");
      maxEntriesComboBox.getAccessibleContext().setAccessibleDescription("Demonstration editable ComboBox with numbers 1-500");
      maxEntriesComboBox.setSelectedIndex(1);
      maximumEntries = 100;

      blockSizeComboBox = new JComboBox();
      blockSizeComboBox.setEditable(true);
      blockSizeComboBox.addItem(new String("2"));
      blockSizeComboBox.addItem(new String("10"));
      blockSizeComboBox.addItem(new String("20"));
      blockSizeComboBox.addItem(new String("30"));
      blockSizeComboBox.addItem(new String("40"));
      blockSizeComboBox.addItem(new String("50"));
      blockSizeComboBox.addItem(new String("60"));
      blockSizeComboBox.addItem(new String("70"));
      blockSizeComboBox.addItem(new String("80"));
      blockSizeComboBox.addItem(new String("90"));
      blockSizeComboBox.addItem(new String("100"));
      blockSizeComboBox.getAccessibleContext().setAccessibleName("String");
      blockSizeComboBox.getAccessibleContext().setAccessibleDescription("Demonstration editable ComboBox with numbers 1-500");
      blockSizeComboBox.setSelectedIndex(0);
      serverBlockSize = 2;

      refreshButton = new JButton("Refresh");

      filenameTextField = new JTextField();
      filenameTextField.setText(INITIAL_SYS_LOG_FILENAME);
      selectedHost = new JLabel("", SwingConstants.LEFT);
      selectedHost.setFont(new Font("Helvetica", Font.BOLD | Font.ITALIC, 13));

      dateTimeCheckBox = new JCheckBox("Enable Date Filter");
      entityCheckBox = new JCheckBox("Enable Entity Filter");

      // Layout the components in the panel
      JPanel p = new JPanel();
      p.setLayout(new GridLayout(3, 4, 0, 10));

      p.add(new JLabel("Host  ", SwingConstants.RIGHT));
      p.add(selectedHost);
      p.add(new JLabel("Log File  ", SwingConstants.RIGHT));
      p.add(filenameTextField);

      p.add(new JLabel("Max Entries  ", SwingConstants.RIGHT));
      p.add(maxEntriesComboBox);
      p.add(new JLabel("Server Block Size  ", SwingConstants.RIGHT));
      p.add(blockSizeComboBox);

      p.add(new JLabel());
      p.add(dateTimeCheckBox);
      p.add(new JLabel());
      p.add(entityCheckBox);

      setLayout(new BorderLayout());
      add(p, BorderLayout.CENTER);

      JPanel p2 = new JPanel();
      p2.add(refreshButton);
      add(p2, BorderLayout.SOUTH);

      // Setup the listeners for the fields
      maxEntriesComboBox.addActionListener(new ActionListener() {
	      public void actionPerformed(ActionEvent e) {
          int maxEntries;
          maxEntries = getJComboBoxInt((String)maxEntriesComboBox.getSelectedItem(), -1);
          if ((maxEntries < 1) || (maxEntries > 500))
		        JOptionPane.showMessageDialog(NCPFrame.getHandle(), "You must select a number between 1 and 500.");
          else {
            maximumEntries = maxEntries;
            refreshButton.setEnabled(true);
          }
  	    }
	    });

      blockSizeComboBox.addActionListener(new ActionListener() {
	      public void actionPerformed(ActionEvent e) {
          int blockSize;
          blockSize = getJComboBoxInt((String)blockSizeComboBox.getSelectedItem(), -1);
          if ((blockSize < 2) || (blockSize > 100))
		        JOptionPane.showMessageDialog(NCPFrame.getHandle(), "You must select a number between 2 and 100.");
          else {
            serverBlockSize = blockSize;
            refreshButton.setEnabled(true);
          }
  	    }
	    });

      filenameTextField.addCaretListener(new CaretListener() {
         public void caretUpdate(CaretEvent e) {
            systemLogFilename = filenameTextField.getText();
         }
      });

      filenameTextField.addActionListener(new ActionListener() {
	      public void actionPerformed(ActionEvent e) {
           systemLogFilename = filenameTextField.getText();
           refreshButton.setEnabled(true);
  	    }
	    });

      dateTimeCheckBox.addActionListener(new ActionListener() {
	      public void actionPerformed(ActionEvent e) {
          if (dateTimeCheckBox.isSelected()) {    // The check box was selected
            DateTimeFilterDialog dateTimeFilterDialog = new DateTimeFilterDialog();
            if (!dateTimeFilterDialog.wasOkSelected()) {
              dateTimeCheckBox.setSelected(false);
              dateFilterCode = 0;
              filterStartDate = "not set";
              filterEndDate = "not set";
            }
            else {
              dateFilterCode = dateTimeFilterDialog.getDateFilterCode();
              filterStartDate = dateTimeFilterDialog.getFilterStartDate();
              filterEndDate = dateTimeFilterDialog.getFilterEndDate();
              refreshButton.setEnabled(true);
            }

            /*
            Log.log(Log.CLT, Log.INFO, this.getClass().getName(), "jbInit",
              "Date Filter " + dateFilterCode);
            Log.log(Log.CLT, Log.INFO, this.getClass().getName(), "jbInit",
              "Filter Start Date " + filterStartDate);
            Log.log(Log.CLT, Log.INFO, this.getClass().getName(), "jbInit",
              "Filter End Date   " + filterEndDate);         */
          }
          else {     // The check box was unselected
             dateFilterCode = 0;
             filterStartDate = "not set";
             filterEndDate = "not set";
             refreshButton.setEnabled(true);
          }
  	    }
	    });

      entityCheckBox.addActionListener(new ActionListener() {
	      public void actionPerformed(ActionEvent e) {
          if (entityCheckBox.isSelected()) {  // The check box was selected
            EntityFilterDialog entityFilterDialog =
              new EntityFilterDialog(currentEntityNames, currentEntityDescriptions);
            if (!entityFilterDialog.wasOkSelected()) {
              entityCheckBox.setSelected(false);
              selectedEntries = null;
            }
            else {
              selectedEntries = entityFilterDialog.getSelectedEntities();
              refreshButton.setEnabled(true);
            }
          }
          else {  // The check box was unselected
             selectedEntries = null;
             refreshButton.setEnabled(true);
          }
  	    }
	    });

       refreshButton.setEnabled(false);

       refreshButton.addActionListener(new java.awt.event.ActionListener()
       {
         public void actionPerformed(ActionEvent e)
         {
            refreshButtonSelected();
         }
       });
    }
    catch (Exception e) {
      Log.log(Log.CLT, Log.EXCP, this.getClass().getName(), "jbInit",
              e.toString());
    }
  }

  // Called when the refresh button is selected
  public void refreshButtonSelected()
  {
     currentBottomPane.refresh();
  }

  // Disables the refresh button
  public void disableRefreshButton()
  {
     refreshButton.setEnabled(false);
  }

  // Returns the value of the dateFilterCode member
  public int getDateFilterCode()
  {
     if (dateTimeCheckBox.isSelected())
        return dateFilterCode;
     else
        return 0;
  }

  // Returns the filterStartDate member
  public String getFilterStartDate()
  {
     return filterStartDate;
  }

  // Returns the filterEndDate member
  public String getFilterEndDate()
  {
     return filterEndDate;
  }

  public int getJComboBoxInt(String selectedItem, int defaultReturnValue)
  {
    try {
      char [] returnValue;
      Integer returnInt;

      returnValue = new char[selectedItem.length()];
      returnValue = selectedItem.toCharArray();
      for (int i = 0; i < selectedItem.length(); i++) {
        if (!Character.isDigit(returnValue[i]))
          return(defaultReturnValue);
  	  }
      returnInt = new Integer(selectedItem);

      if (returnInt == null)
  	  	return(defaultReturnValue);

      return (returnInt.intValue());
    }
    catch (Exception e) {
      Log.log(Log.CLT, Log.EXCP, this.getClass().getName(), "getMaxEntries",
              e.toString());
	  	return(defaultReturnValue);
    }
  }

  public int getMaxEntries(int defaultReturnValue)
  {
    try {
      if (maximumEntries  < 0)
        return (defaultReturnValue);
      else
        return (maximumEntries);
    }
    catch (Exception e) {
      Log.log(Log.CLT, Log.EXCP, this.getClass().getName(), "getMaxEntries",
              e.toString());
	  	return(defaultReturnValue);
    }
  }

  public int getServerBlockSize(int defaultReturnValue)
  {
    try {
      if (serverBlockSize  < 0)
        return (defaultReturnValue);
      else
        return (serverBlockSize);
    }
    catch (Exception e) {
      Log.log(Log.CLT, Log.EXCP, this.getClass().getName(), "getServerBlockSize",
              e.toString());
	  	return(defaultReturnValue);
    }
  }

  // Sets the displayed hostname to "newHost"
  public void setDisplayedHostname(String newHost)
  {
    try {
      selectedHost.setText(newHost);
      selectedHost.validate();
    }
    catch (Exception e) {
      Log.log(Log.CLT, Log.EXCP, this.getClass().getName(), "setDisplayedHostname",
              e.toString());
    }
  }

  public String getSystemLogFilename(String defaultReturnValue)
  {
    try {
      if (systemLogFilename.length()  <= 0)
        return (defaultReturnValue);
      else
        return (systemLogFilename);
    }
    catch (Exception e) {
      Log.log(Log.CLT, Log.EXCP, this.getClass().getName(), "getSystemLogFilename",
              e.toString());
	  	return(defaultReturnValue);
    }
  }

  // Gets the selected entity filters
  public boolean[] getEntitySelections()
  {
    try {
       if (entityCheckBox.isSelected())
          return (selectedEntries);
       else
          return null;
    }
    catch (Exception e) {
       return null;
    }
  }
}
