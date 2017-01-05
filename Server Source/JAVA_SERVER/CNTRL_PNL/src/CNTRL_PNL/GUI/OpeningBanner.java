/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package CNTRL_PNL.GUI;

import java.awt.*;
import javax.swing.*;
import javax.swing.border.*;
import java.net.*;

import CNTRL_PNL.COMMON.*;

public class OpeningBanner extends Window
{
   public OpeningBanner()
   {
      super(new Frame());

      final int bannerWidth = 370;
      final int bannerHeight = 170;
      Dimension screenSize;
      int desiredX, desiredY;
      JPanel panel;
      Color backgroundColor = new Color(255, 230, 190);

      // Set the desired screen size of the banner and place it in the middle
      //  of the screen
      setSize(new Dimension(bannerWidth, bannerHeight));
      screenSize = (Toolkit.getDefaultToolkit()).getScreenSize();
      desiredX = (screenSize.width/2) - (bannerWidth/2);
      desiredY = (screenSize.height/2) - (bannerHeight/2);
      setLocation(new Point(desiredX, desiredY));

      // Create the panel which all the components will be added to. This will
      //  act as the virtual banner. This panel was needed so that a border
      //  could be added easily.
      panel = new JPanel();

      // Set the background color of the banner
      panel.setBackground(backgroundColor);

      // Set the border for the banner
      Border raisedbevel = BorderFactory.createRaisedBevelBorder();
      Border loweredbevel = BorderFactory.createLoweredBevelBorder();
      Border compound = BorderFactory.createCompoundBorder(raisedbevel, loweredbevel);
      Border colorLine = BorderFactory.createLineBorder(Color.gray);
      panel.setBorder(BorderFactory.createCompoundBorder(colorLine, compound));

      // Set the layout manager for the banner
      panel.setLayout(new BorderLayout());

      // Display the product name
      panel.add(new ProductNamePanel(backgroundColor), BorderLayout.NORTH);

      // Display the "loading" indicator
      panel.add(new LoadingIndicatorPanel(backgroundColor), BorderLayout.SOUTH);

      // Add the virtual banner to the window
      add(panel);
   }

   // Displays the banner on the screen
   public void showBanner()
   {
      setVisible(true);
   }

   // Hides the banner so that it is not displayed on the screen
   public void hideBanner()
   {
      setVisible(false);
   }
}

class ProductNamePanel extends JPanel
{
   public ProductNamePanel(Color backgroundColor)
   {
      JLabel productName, version;

      // Set the layout manager
      setLayout(new GridLayout(2,0));

      // Set the background color
      setBackground(backgroundColor);

      // Display the product name
      productName = new JLabel("NTCSS Control Panel", JLabel.CENTER);
      productName.setForeground(Color.black);
      productName.setFont(new Font("Sanserif", Font.BOLD + Font.ITALIC, 30));
      add(productName);

      // Display the version number
      version = new JLabel("v " + Constants.CNTRL_PNL_VERSION, JLabel.CENTER);
      version.setForeground(Color.black);
      version.setFont(new Font("Sanserif", Font.ITALIC, 14));
      add(version);
   }
}

class LoadingIndicatorPanel extends JPanel
{
   public LoadingIndicatorPanel(Color backgroundColor)
   {
      JLabel loadingIcon;
      JLabel loadingText;
      ImageIcon icon;
      URL url;

      // Set the layout manager
      setLayout(new BorderLayout());

      // Set the background color
      setBackground(backgroundColor);

      // Display the loading text
      loadingText = new JLabel("Loading...", JLabel.CENTER);
      loadingText.setForeground(Color.black);
      loadingText.setFont(new Font("Sanserif", Font.PLAIN, 16));
      add(loadingText, BorderLayout.NORTH);

      // Display the loading icon. Get the "loading" gif from the jar file.
      url = getClass().getResource(Constants.LOADING_GIF);
      if (url != null) {
         icon = new ImageIcon(url);
         loadingIcon = new JLabel(icon, JLabel.CENTER);
         add(loadingIcon, BorderLayout.CENTER);
      }
   }
}