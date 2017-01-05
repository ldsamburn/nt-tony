/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.distmgr;

import java.util.Hashtable;
import java.util.Enumeration;

/**
 * This is the results that are returned from all the distributed processes
 *  started by the distribution manager
 */
public class DPResults
{
   private Hashtable resultList;
   
   /**
    * Constructor
    */
   public DPResults(int size) 
   {
      resultList = new Hashtable(size);
   }
   
   /**
    * Adds the given result for the given host to the result list
    */
   public synchronized void add(String host, String result)
   {
      resultList.put(host, result);
   }
   
   /**
    * Returns the results that have been added to the result list for the
    *  given host
    */
   public synchronized String getResult(String host)
   {
      return (String)resultList.get(host);
   }
   
   /**
    * Returns a list of hosts that have results stored in the list
    */
   public synchronized Enumeration getHosts()
   {
      return resultList.keys();
   }
}
