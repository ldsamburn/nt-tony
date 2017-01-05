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
import CNTRL_PNL.INFO.*;

public class CommonDataTopPanel extends JPanel
{
  BorderLayout borderLayout2 = new BorderLayout();
  JPanel jPanel4 = new JPanel();
  JLabel hostLabel = new JLabel();
  JLabel valueLabel = new JLabel();
  JPanel jPanel1 = new JPanel();
  GridLayout gridLayout2 = new GridLayout();
  JLabel tagLabel = new JLabel();
  JPanel jPanel2 = new JPanel();
  GridLayout gridLayout3 = new GridLayout();
  JTextField valueTextField = new JTextField();
  JTextField tagTextField = new JTextField();
  JLabel hostTextField = new JLabel();
  JButton setButton = new JButton();
  JPanel jPanel3 = new JPanel();
  BorderLayout borderLayout1 = new BorderLayout();
  JPanel padding1 = new JPanel();
  JPanel padding2 = new JPanel();
  JPanel padding3 = new JPanel();

  public CommonDataTopPanel()
  {
     try {
        jbInit();
     }
     catch (Exception e) {
        e.printStackTrace();
     }
  }

  private void jbInit() throws Exception
  {
    hostTextField.setFont(new Font("Helvetica", Font.BOLD  | Font.ITALIC, 13));

    setButton.setText("Set");
    jPanel2.setLayout(gridLayout3);
    gridLayout3.setVgap(5);
    hostTextField.setHorizontalAlignment(JTextField.LEFT);
    setButton.setMinimumSize(new Dimension(20, 20));
    setButton.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        setButton_actionPerformed(e);
      }
    });
    borderLayout1.setHgap(5);
    borderLayout1.setVgap(5);
    setButton.setText("Set");
    gridLayout3.setRows(3);
    hostLabel.setHorizontalAlignment(JLabel.RIGHT);
    hostLabel.setText("Host");
    hostLabel.setMinimumSize(new Dimension(26, 30));
    valueLabel.setText("Value");
    valueLabel.setHorizontalAlignment(JLabel.RIGHT);
    valueLabel.setMaximumSize(new Dimension(31, 50));
    valueLabel.setPreferredSize(new Dimension(31, 50));
    valueLabel.setMinimumSize(new Dimension(31, 30));
    jPanel1.setLayout(gridLayout2);
    jPanel1.setMinimumSize(new Dimension(50, 50));
    jPanel1.setPreferredSize(new Dimension(50, 50));
    gridLayout2.setVgap(5);
    tagLabel.setHorizontalAlignment(JLabel.RIGHT);
    tagLabel.setMaximumSize(new Dimension(21, 50));
    tagLabel.setPreferredSize(new Dimension(21, 50));
    tagLabel.setMinimumSize(new Dimension(21, 30));
    tagLabel.setText("Tag");
    gridLayout2.setRows(3);
    hostLabel.setPreferredSize(new Dimension(26, 50));
    hostLabel.setMaximumSize(new Dimension(26, 50));
    this.setPreferredSize(new Dimension(50, 150));
    borderLayout2.setVgap(5);
    jPanel4.setLayout(borderLayout1);
    borderLayout2.setHgap(5);
    this.setLayout(borderLayout2);

    this.add(jPanel4, BorderLayout.CENTER);
    jPanel4.add(jPanel3, BorderLayout.SOUTH);
    jPanel3.add(setButton, null);
    jPanel4.add(jPanel2, BorderLayout.CENTER);
    jPanel2.add(hostTextField, null);
    jPanel2.add(tagTextField, null);
    jPanel2.add(valueTextField, null);
    jPanel4.add(jPanel1, BorderLayout.WEST);
    jPanel1.add(hostLabel, null);
    jPanel1.add(tagLabel, null);
    jPanel1.add(valueLabel, null);
    this.add(padding1, BorderLayout.NORTH);
    this.add(padding2, BorderLayout.WEST);
    this.add(padding3, BorderLayout.EAST);
  }

  // Refreshes the data displayed in this screen
  public void refresh(String host)
  {
     // Set the component which displays the selected host
     hostTextField.setText(host);

     // Clear the tag/value text fields
     tagTextField.setText(null);
     valueTextField.setText(null);

     // If host is not master server, disable "set" button and the entry fields
     if ((NtcssServersInfo.getHandle()).getServerType(host) !=
                                                 Constants.MASTER_SVR) {
        setButton.setEnabled(false);
        tagTextField.setEditable(false);
        valueTextField.setEditable(false);
     }
     else { 
        setButton.setEnabled(true);
        tagTextField.setEditable(true);
        valueTextField.setEditable(true);
     }
  }

  // This is called when the "set" button is clicked
  void setButton_actionPerformed(ActionEvent e)
  {
     String host, tag, value;

     host = hostTextField.getText();

     if ((host != null) && (host.length() != 0)) {
        (NCPFrame.getHandle()).setCursorType(Cursor.WAIT_CURSOR);

        // Get the user supplied tag and value
        tag = (tagTextField.getText()).trim();
        value = (valueTextField.getText()).trim();

        // Check to make sure that the items have been set
        if ((tag.length() != 0) && (value.length() != 0)) {
           // Attempt to set the tag/value pair on the selected host
           (NtcssServersInfo.getHandle()).setCommonData(host, tag, value);
           (NCPFrame.getHandle()).setCursorType(Cursor.DEFAULT_CURSOR);
        }
        else {
           (NCPFrame.getHandle()).setCursorType(Cursor.DEFAULT_CURSOR);

           // Display a proper dialog box
           if ((tag.length() == 0) && (value.length() != 0))
              JOptionPane.showMessageDialog(NCPFrame.getHandle(),
                                            "Tag field left blank", "Error",
                                            JOptionPane.ERROR_MESSAGE);
           else if ((tag.length() != 0) && (value.length() == 0))
              JOptionPane.showMessageDialog(NCPFrame.getHandle(),
                                            "Value field left blank", "Error",
                                            JOptionPane.ERROR_MESSAGE);
           else
              JOptionPane.showMessageDialog(NCPFrame.getHandle(),
                                            "Tag and Value fields left blank",
                                            "Error",
                                            JOptionPane.ERROR_MESSAGE);
        }
     }
  }
}
