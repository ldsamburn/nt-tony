/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.distmgr;

import java.util.Hashtable;

import COMMON.Counter;

/**
 * Contains a list of hosts and their "incident number". The incident number
 *  is incremented when an attempt to read a response from the host fails.
 *
 *  The methods are synchronized since multiple threads (DistributedProcess objects)
 *  could be accessing this list at the same time.
 */
public class ProblemHostList
{
   private Hashtable list;
   
   public ProblemHostList() 
   {
      list = new Hashtable();
   }
   
   public synchronized boolean isProblemHost(String host)
   {
      Counter count;
      
      if (((count = (Counter)list.get(host)) != null) &&
          (count.getValue() > 1))
         return true;      
      
      return false;
   }
   
   public synchronized void removeHost(String host)
   {
      list.remove(host);
   }
   
   public synchronized void incrementIncident(String host)
   {
      Counter count;      
      
      if ((count = (Counter)list.get(host)) != null)
         count.increment();      
      else
         list.put(host, new Counter(1));
   }
}
