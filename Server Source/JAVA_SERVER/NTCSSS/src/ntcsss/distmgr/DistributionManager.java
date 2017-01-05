/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.distmgr;

import java.util.List;
import java.util.Iterator;
import java.util.Enumeration;

import ntcsss.log.Log;
import ntcsss.libs.Constants;

/**
 * The distribution manager can send a single message to a set of
 *  hosts and obtain their responses through the use of distributed processes.
 */
public class DistributionManager
{
   /**
    * Because this method is specified as "synchronized", only one thread
    *  can be executing this method at a time, ensuring serialized access to
    *  the distribution manager services. The timeout value is in seconds.
    */
   public static synchronized DPResults sendMessage(String message, 
                                                    List hostList, int port,
                                                    boolean sendAndForget,
                                                    int dpTimeoutValue)                                                     
   {      
      DPResults dpResults;
      Iterator iterator;       
      FinishedDPCount finishedCount;      
      ActiveDPCount activeCount;
       
      try {
          
         // Makes sure the given host list isn't empty
         if ((hostList == null) || (hostList.size() == 0)) {
            Log.error("DistributionManager", "sendMessage", 
                      "Host list is empty");
            return null;
         }
          
         // Create an object for the distributed processes to place their
         //  results in
         dpResults = new DPResults(hostList.size());
         
         // Create the counter of distributed processes that have finished
         finishedCount = new FinishedDPCount(hostList.size());     
         
         // Create the counter that will keep track of the number of active
         //  distributed processes
         activeCount = new ActiveDPCount(Constants.MAX_NUM_ACTIVE_DPS);     
          
         // Start the distributed processes          
         iterator = hostList.iterator();
         while (iterator.hasNext() == true) {             
             
            // Only a limited number of distributed processes should be running
            //  at one time to prevent overloading the CPU with threads. So,
            //  this method will block until it is ok to create a new thread
            //  (distributed process).
            activeCount.waitForSlot();
            
            // Increment the number of active distributed processes. This
            //  count is decremented by the distributed processes
            activeCount.increment();
             
            // Create and start a distributed process
            new DistributedProcess(message, (String)iterator.next(), port,
                                   dpResults, finishedCount, activeCount,
                                   sendAndForget, dpTimeoutValue);                                                         
         }
          
         // Wait for the distributed processes to finish
         Log.debug("DistributionManager","sendMessage", 
                   "Waiting for processes to finish");          
         if (finishedCount.wait((dpTimeoutValue + 2) * 2) == true) 
            Log.debug("DistributionManager","sendMessage", "Processes complete");                                          
         else 
            Log.error("DistributionManager","sendMessage", "Timeout occurred");                                                         
         
         // Return whatever has been returned from the hosts
         return dpResults;
      }
      catch (Exception exception) {
         Log.excp("DistributionManager", "sendMessage", exception.toString());
         return null;
      }       
   }     
}
