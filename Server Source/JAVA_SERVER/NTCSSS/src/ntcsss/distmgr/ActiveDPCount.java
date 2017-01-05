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
 * A counter for the number of currently active distributed processes
 */
public class ActiveDPCount
{
   private int activeCount;
   private boolean activeCountAtMax;
   private final int maxActive;

   /** 
    * Constructor
    */
   public ActiveDPCount(int maxActive) 
   {
      this.maxActive = maxActive;
      
      activeCount = 0;
      activeCountAtMax = false;
   }
   
   /**
    * The distribution manager will call this method when it starts a 
    *  distributed process.
    */
   public synchronized void increment()
   {      
      if (++activeCount == maxActive)
         activeCountAtMax = true;                  
   }
   
   /**
    * A distributed process will call this method when they exit
    */
   public synchronized void decrement()
   {
      
      if (--activeCount == (maxActive - 1)) {  
         
         // Wake up any threads which are waiting for all the distributed
         //  processes to complete
         notify();     
         activeCountAtMax = false;
      }
   }
   
   /**
    * The distribution manager will call this method to wait until the 
    *  count of active distibuted processes is less than the maximum allowed.
    */
   public synchronized void waitForSlot()
   {
      try {         
         while (activeCountAtMax == true)         
            wait();
      }
      catch (Exception exception) {
         Log.excp("ActiveDPCount", "waitForSlot", exception.toString());
      }
   }
}
