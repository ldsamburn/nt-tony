/****************************************************************************

               Copyright (c)2002 Northrop Grumman Corporation

                           All Rights Reserved


     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/

package archive.client;

import javax.swing.*;
import java.awt.*;

import archive.common.*;

/**
 * This class creates an "About" dialog box
 */
public class AboutDialog
{
   /**
    * Constructor
    */
   public AboutDialog()
   {
      JOptionPane pane;
      JDialog dialog;
      /* ImageIcon icon; */

      // Setup the option pane
      /* icon = new ImageIcon(""); */ // Uncomment and set if an icon other than
                                      //  the default is desired
      pane = new JOptionPane(createMessagePanel(),
                             JOptionPane.INFORMATION_MESSAGE,
                             JOptionPane.DEFAULT_OPTION /*, icon */);

      // Create a dialog to place the option pane in
      dialog = pane.createDialog(FSArchive.getHandle(),
                                 "About File System Archiver");

      // Display the dialog box
      dialog.show();
   }

   /**
    * Creates the formatted text for the About box
    */
   private Panel createMessagePanel()
   {
      Panel messagePanel;
      Label product;
      Label version;
      Label copyright;

      product = new Label("File System Archiver");
      version = new Label("Version " + Constants.CLIENT_VERSION);
      copyright = new Label("Copyright (c) 2001");

      messagePanel = new Panel();
      messagePanel.setLayout(new GridLayout(3, 0));

      messagePanel.add(product);
      messagePanel.add(version);
      messagePanel.add(copyright);

      messagePanel.setVisible(true);

      return messagePanel;
   }
}
