/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/


package CNTRL_PNL.SERVER;

import java.net.*;
import java.io.*;
import java.lang.*;

import COMMON.*;
import CNTRL_PNL.COMMON.*;

public class MulticastListenerThread extends Thread
{
   private HandlerCounter numOfPThreads;
   private boolean stop = false;
   private boolean shutdown = false;

   public MulticastListenerThread()
   {
      numOfPThreads = new HandlerCounter();

      setPriority(Thread.MAX_PRIORITY);
      
      // Don't set this thread to be a daemon thread so that server will have to
      //  wait to shutdown for this thread to stop

      // Listen for messages from the multicast group
      this.start();
   }

   public void run()
   {
      // Setup for communication with the multicast group
      MessageWithData msg = null;
      String msgHeader;
      boolean ok;
      boolean reconnect = true;

      while (stop == false) {
         
         if (reconnect == true)
            msg = new MessageWithData(Constants.MULTICAST_PORT,
                                      Constants.MULTICAST_IP);

         // Block until there is data sent to the multicast group. "Wakeup"
         //  periodically from the receive to see if this daemon should stop
         try {
            ok = msg.receive(5);
         }
         catch (InterruptedIOException timeoutException) {
            reconnect = false;
            continue;
         }
         
         if (ok == true)
            // Process the data received
            new ProcessRequestThread(msg, numOfPThreads);
         else {
            msg.finished();
            Log.log(Log.SVR, Log.ERR, this.getClass().getName(), "run",
                           "Error receiving message from the Multicast group");
         }
         
         reconnect = true;
      } // end of while loop
      
      // Make sure the message mechanism is cleaned up
      msg.finished();
      
      // Wait at most 5 seconds for the "process" threads to finish
      if (numOfPThreads.waitForCompletion(5) == false)
         Log.log(Log.SVR, Log.ERR, this.getClass().getName(), "run",
                    "Process handlers for multicast listener still executing");
      
      notifyOfCompletion();
   }

   public synchronized void stopMCListener()
   {
      stop = true;
      
      if (shutdown == false)
         try {
            wait();
         }
         catch (Exception exception) {}
   }
   
   private synchronized void notifyOfCompletion()
   {
      notify();
      shutdown = true;
   }
}
