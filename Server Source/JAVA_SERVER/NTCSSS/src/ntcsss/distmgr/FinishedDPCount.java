/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.distmgr;

import ntcsss.log.Log;

/**
 * A counter for the number of distributed processes that have
 *  completed processing
 */
public class FinishedDPCount
{
   private int finalCount;
   private int count;     
   private boolean notified;
   
   /** 
    * Constructor
    */
   public FinishedDPCount(int finalCount) 
   {
      this.finalCount = finalCount;      
      
      count = 0;
      notified = false;
   }

   /**
    * This method increments the count of the number of distributed processes
    *  that have completed their processing. When "count" equals "finalCount",
    *  notify is called which will "awaken" any threads which have been
    *  waiting for this event to happen.
    */
   public synchronized void increment()
   {
      // Increment the count and see if this matches the number we're waiting
      //  on.
      if (++count == finalCount) {  
         
         // Wake up any threads which are waiting for all the distributed
         //  processes to complete
         notifyAll();     
         notified = true;
      }
   } 
   
   /**
    * This method will cause the calling thread to wait until all the 
    *  distributed processes have finished processing or the given timeout
    *  period (in seconds) has elapsed. If the timeout period has elapsed, 
    *  "false" is returned. Otherwise, "true" is returned.
    */
   public synchronized boolean wait(int timeout)
   {
      try {
         // Only need to "wait" if the notify method has not been called
         //  i.e. all the distributed processes have not completed.
         if (notified == false) {
            
            // "Sleep" for the given period of time
            wait((long)timeout * 1000);
            
            // If the notify method has not been called, and we are at this 
            //  point, then the timeout has occurred.
            if (notified == false)
               return false;
         }
         
         return true;
      }
      catch (Exception exception) {
         Log.excp("FinishedDPCount", "wait", exception.toString());
         
         return true;
      }
   }
}
