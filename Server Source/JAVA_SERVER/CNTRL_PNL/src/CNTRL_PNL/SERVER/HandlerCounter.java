/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package CNTRL_PNL.SERVER;

public class HandlerCounter
{
   private int numberOfHandlers;
      
   public HandlerCounter() 
   {
      numberOfHandlers = 0;
   }

   public synchronized void increment()
   {
      numberOfHandlers++;
   }
   
   public synchronized void decrement()
   {
      numberOfHandlers--;
      if (numberOfHandlers == 0)
         notifyAll();
   }
   
   /**
    * Timeout in seconds
    */
   public synchronized boolean waitForCompletion(int timeout)
   {
      try {
         
         // Only need to "wait" if the number of handlers is more than 0         
         if (numberOfHandlers > 0) {
            
            // "Sleep" for the given period of time
            wait((long)timeout * 1000);
            
            // If the count is not at zero, and we are at this 
            //  point, then the timeout has occurred.
            if (numberOfHandlers != 0)
               return false;
         }
         
         return true;
      }
      catch (Exception exception) {
         return false;
      }
   }
}
