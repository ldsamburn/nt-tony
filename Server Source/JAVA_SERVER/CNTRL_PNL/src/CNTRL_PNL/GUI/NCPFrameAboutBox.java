/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package CNTRL_PNL.GUI;

import javax.swing.*;
import java.awt.*;

import CNTRL_PNL.COMMON.*;

public class NCPFrameAboutBox
{
   public NCPFrameAboutBox()
   {
      JOptionPane pane;
      JDialog dialog;
      ImageIcon icon;

      // Setup the option pane
      //icon = new ImageIcon("");
      pane = new JOptionPane(new Message(), JOptionPane.INFORMATION_MESSAGE,
      //                       JOptionPane.DEFAULT_OPTION, icon);
                             JOptionPane.DEFAULT_OPTION);

      // Create a dialog to place the option pane in
      dialog = pane.createDialog(NCPFrame.getHandle(), "About NTCSS Control Panel");

      // Display the dialog box
      dialog.show();
   }
}

class Message extends Panel
{
   private Label product;
   private Label version;
   private Label copyright;

   public Message()
   {
      product = new Label("NTCSS Control Panel");
      version = new Label("Version " + Constants.CNTRL_PNL_VERSION);
      copyright = new Label("Copyright (c) 1999");

      setLayout(new GridLayout(3, 0));

      add(product);
      add(version);
      add(copyright);

      setVisible(true);
   }
}