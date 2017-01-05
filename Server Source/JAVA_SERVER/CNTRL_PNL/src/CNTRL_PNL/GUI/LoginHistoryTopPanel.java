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

import CNTRL_PNL.COMMON.*;

public class LoginHistoryTopPanel extends JPanel
{
  private JComboBox comboBox;
  private LoginHistoryTabbedPane loginHistoryPane;
  private int maximumEntries;
  private JLabel selectedHost;
  private JButton refreshButton;

  public LoginHistoryTopPanel(LoginHistoryTabbedPane tabbedPane)
  {
    try {
      loginHistoryPane = tabbedPane;

      comboBox = new JComboBox();
      comboBox.setEditable(true);
      comboBox.addItem(new String("50"));
      comboBox.addItem(new String("100"));
      comboBox.addItem(new String("150"));
      comboBox.addItem(new String("200"));
      comboBox.addItem(new String("250"));
      comboBox.addItem(new String("300"));
      comboBox.addItem(new String("350"));
      comboBox.addItem(new String("400"));
      comboBox.addItem(new String("450"));
      comboBox.addItem(new String("500"));

      // Setup the default max entries shown
      comboBox.setSelectedIndex(1);  // 100
      maximumEntries = 100;

      comboBox.addActionListener(new java.awt.event.ActionListener()
      {
         public void actionPerformed(ActionEvent e)
         {
            maxEntriesSelected();
         }
      });

      refreshButton = new JButton("Refresh");
      refreshButton.setEnabled(false);

      refreshButton.addActionListener(new java.awt.event.ActionListener()
      {
         public void actionPerformed(ActionEvent e)
         {
            refreshButtonSelected();
         }
      });

      selectedHost = new JLabel("", SwingConstants.LEFT);
      selectedHost.setFont(new Font("Helvetica", Font.BOLD | Font.ITALIC, 13));

      // Layout the components in the panel
      setLayout(new BorderLayout());

      JPanel p = new JPanel();
      p.setLayout(new GridLayout(3, 5, 0, 10));

      p.add(new JLabel("  ", SwingConstants.RIGHT));
      p.add(new JLabel("Host  ", SwingConstants.RIGHT));
      p.add(selectedHost);
      p.add(new JLabel("  ", SwingConstants.RIGHT));
      p.add(new JLabel("  ", SwingConstants.RIGHT));

      p.add(new JLabel("  ", SwingConstants.RIGHT));
      p.add(new JLabel("Max Entries  ", SwingConstants.RIGHT));
      p.add(comboBox);
      p.add(new JLabel("  ", SwingConstants.RIGHT));
      p.add(new JLabel("  ", SwingConstants.RIGHT));

      p.add(new JLabel("  ", SwingConstants.RIGHT));
      p.add(new JLabel("  ", SwingConstants.RIGHT));
      p.add(new JLabel("  ", SwingConstants.RIGHT));
      p.add(new JLabel("  ", SwingConstants.RIGHT));
      p.add(new JLabel("  ", SwingConstants.RIGHT));

      add(p, BorderLayout.CENTER);
      JPanel p2 = new JPanel();
      p2.add(refreshButton);

      add(p2, BorderLayout.SOUTH);
    }
    catch (Exception e) {
      Log.log(Log.CLT, Log.EXCP, this.getClass().getName(),
              "LoginHistoryTopPanel", e.toString());
    }
  }

  // This function is called when the refresh button is selected
  public void refreshButtonSelected()
  {
     loginHistoryPane.refresh();
  }

  // Called when the user changes the state of the combo box
  public void maxEntriesSelected()
  {
    int maxEntries;

    try {
       maxEntries = parseSelectedValue((String)comboBox.getSelectedItem(), -1);

       if ((maxEntries < 1) || (maxEntries > 500))
  		   JOptionPane.showMessageDialog(NCPFrame.getHandle(),
                                 "You must select a number between 1 and 500.");
       else
         refreshButton.setEnabled(true);
    }
    catch (Exception e) {
      Log.log(Log.CLT, Log.EXCP, this.getClass().getName(), "maxEntriesSelected",
              e.toString());
    }
  }

  // The parameter that is passed in is the one the user has entered in for the
  //  max entries field. This function parses that parameter making sure it is
  //  valid and then returns the integer representation of it.
  private int parseSelectedValue(String selectedItem, int defaultReturnValue)
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

      maximumEntries = returnInt.intValue();
      return (returnInt.intValue());
    }
    catch (Exception e) {
      Log.log(Log.CLT, Log.EXCP, this.getClass().getName(), "parseSelectedValue",
              e.toString());
	  	return(defaultReturnValue);
    }
  }

  // Returns the value of the "maximumEntries" member variable. If the member
  //  is less than zero , than the specified default return value is returned.
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

  // Disables the refresh button
  public void disableRefreshButton()
  {
     refreshButton.setEnabled(false);
  }

}
