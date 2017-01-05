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
import java.util.*;

import CNTRL_PNL.INFO.*;

abstract public class TabbedPaneBase extends JPanel
{
   // This is the current host(s) that has been selected by the user from
   //  the host list panel
   private String[] currentHosts;

   // Sets the currentHost member with the new list of selected hosts
   public void setCurrentHost(String[] newHosts)
   {
      currentHosts = newHosts;
      hostSelectionMade();
   }

   // Returns a local copy of the list of host(s) that were selected in the
   //  host list panel when this pane was on top
   public String[] getCurrentHosts()
   {
      return currentHosts;
   }

   // Returns whether multiple hosts have been selected from the host list panel
   public boolean multipleHostsSelected()
   {
      return (currentHosts.length > 1);
   }

   // A call to this function forces the particular screen to handle the new
   //  host selection
   public abstract void hostSelectionMade();

   // Repaints the menu so that this pane's action menu is included
   public abstract void updateMenu();

   // Obtains from the pane a string that represents all the information that
   //  is displayed in that pane.
   public abstract String getPrintableMaterial();

   // Given a text string, this function will pad the end of the string with
   //  spaces so that the final length is "desiredLength". The final string
   //  is returned. This is useful within the getPrintableMaterial method.
   protected String paddedString(String text, int desiredLength)
   {
      int numPadding;

      if ((text != null) && (text.length() < desiredLength)) {
         numPadding = desiredLength - text.length();

         for (int i = 0; i < numPadding; i++)
            text += " ";
      }

      return text;
   }
}

