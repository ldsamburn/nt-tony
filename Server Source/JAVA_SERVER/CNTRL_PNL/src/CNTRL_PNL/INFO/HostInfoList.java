/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package CNTRL_PNL.INFO;

import java.util.*;

public class HostInfoList
{
   private Vector list;   // This is how the host list is implemented

   public HostInfoList()
   {
      // Create a new empty list
      list = new Vector();
   }

   // Adds an entry to the host list
   public synchronized void addEntry(String hostname, String IPaddress,
                                     int serverType, int serverStatus)
   {
      // Create a new list entry
      HostInfoListEntry listEntry = new HostInfoListEntry(hostname, IPaddress,
                                                          serverType,
                                                          serverStatus);

      // Add the entry to the list
      list.addElement(listEntry);
   }

   // Returns the list entry at the specified index
   public synchronized HostInfoListEntry getEntry(int index)
   {
      HostInfoListEntry entry = null;

      if (index < size())
         entry = (HostInfoListEntry)list.elementAt(index);

      return entry;
   }

   // Returns the list entry with the specified hostname
   public synchronized HostInfoListEntry getEntry(String hostname)
   {
      HostInfoListEntry entry = null;
      HostInfoListEntry current;

      for (int i = 0; i < size(); i++) {
         current = getEntry(i);
         if (hostname.equals(current.getHostname()) == true) {
            entry = current;
            break;
         }
      }

      return entry;
   }

   // Clears the host info list
   public synchronized void clearList()
   {
      list.removeAllElements();
   }

   // Returns the current size of the host info list
   public synchronized int size()
   {
      return list.size();
   }
}
