/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package CNTRL_PNL.GUI;

import javax.swing.*;

public class NCPStatusBar extends JLabel
{
   public NCPStatusBar()
   {
      setStatusText(" ");
   }

   // Changes the displayed text to "newText"
   public void setStatusText(String newText)
   {
      setText(newText);
      validate();
   }
}
