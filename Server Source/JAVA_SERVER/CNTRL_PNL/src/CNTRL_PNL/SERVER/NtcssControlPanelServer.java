/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package CNTRL_PNL.SERVER;

import java.util.*;
import java.net.*;
import java.io.*;

import COMMON.*;
import CNTRL_PNL.COMMON.*;

public class NtcssControlPanelServer implements Runnable
{
   private MulticastListenerThread mcLThread;
   private ServerSocket svrSocket;
   private HandlerCounter numOfPThreads;
   private KillFlag killFlag;

   public static void main(String[] args)
   {
      boolean ok;

      // Enable the Just-In-Time compiler if it is available
      Compiler.enable();

      // Try to load in the properties file      
      if (args.length == 0)
         ok = false;
      else
         ok = PropertiesLoader.load(args[0]);  
      
      // If a -v command line argument was given, print out the server version
      //  and exit
      if ((args.length == 2) &&
          (args[1].equals("-v") == true)) {
         System.out.println("Version " + Constants.CNTRL_PNL_SVR_VERSION);
         System.exit(0);
      }

      // Setup the desired logging stream
      if ((Constants.SVR_LOG_FILE != null) &&
                               ((Constants.SVR_LOG_FILE.trim()).length() != 0))
         Log.setLogFile(Constants.SVR_LOG_FILE);

      if (ok == true) {
         NtcssControlPanelServer server = new NtcssControlPanelServer();

         Log.log(Log.SVR, Log.INFO, "NtcssControlPanelServer", "main",
                            "NTCSS Control Panel Server started");

         server.run();

         // This is needed to ensure that the server will exit. This is to take
         //  care of the situation when a ProcessRequestThread thread starts a
         //  program which forks. If that forked process is to live on
         //  "indefinitly", it may cause problems when this server is to be shut
         //  down. Even though the ProcessRequestThread thread may finish, the
         //  server still has some notion of that forked process that is still
         //  running.  Because of this, the server will not exit unless this
         //  statement is left in.  This doesn't seem to have any effect on the
         //  forked process (which is desirable), so that process will continue
         //  to run.
         System.exit(0);
      }
      else {
         Log.log(Log.SVR, Log.ERR, "NtcssControlPanelServer", "main",
                            "Error reading properties file! Cannot continue!!");
      }
   }

   public void run()
   {
      boolean shutDown = false;
      Socket socket;
      MessageWithData msg;

      try {
         mcLThread = new MulticastListenerThread();
         numOfPThreads = new HandlerCounter();
         killFlag = new KillFlag();

         // Create the "listener" for the server
         svrSocket = new ServerSocket(Constants.CP_SERVER_PORT, 
                                      Constants.SERVER_BACKLOG_QUEUE_SIZE);

         // Look for messages from control panels
         while (shutDown == false) {

            // Wait for a message to come in (i.e. blocks until a socket
            //  connection is requested)
            socket = waitForRequest();

            // Process the request
            if (socket != null)
               new ProcessRequestThread(socket, numOfPThreads, killFlag);
            else {
               String str = "NTCSSS Control Panel Server shutting down...";
               System.out.println(str);
               Log.log(Log.SVR, Log.INFO, this.getClass().getName(), "run", str);

               // Close down the server gracefully
               shutdownServer();

               str = "NTCSS Control Panel Server shutdown complete.";
               System.out.println(str);
               Log.log(Log.SVR, Log.INFO, this.getClass().getName(), "run", str);

               shutDown = true;
            }
         }
      }
      catch (BindException e) {
         System.out.println("Error: Ntcss Control Panel Server may already be running.");
         shutdownServer();
      }
      catch (Exception e) {
         Log.log(Log.SVR, Log.EXCP, this.getClass().getName(), "run",
                 e.toString());
         shutdownServer();
      }
   }

   // This function waits for a connection request to be made to this server.
   //  When a request is made, the connecting socket is returned. While waiting,
   //  it periodically checks to see if the server has received a kill message.
   //  If a kill message has been received or an error occurs, "null" is
   //  immediatly returned.
   private Socket waitForRequest()
   {
      Socket skt = null;
      boolean cont = true;

      while (cont == true) {
         try {
            svrSocket.setSoTimeout(1000);
            skt = svrSocket.accept();
            cont = false;
         }
         catch (InterruptedIOException e) {
            if (killFlag.isSet() == true) {
               skt = null;
               cont = false;
            }
         }
         catch (Exception e) {
            skt = null;
            cont = false;
         }
      }

      return skt;
   }

   // Disallows any more connections to be made to this server and then waits
   //  for any active process threads to complete
   private void shutdownServer()
   {
      try {
         // Close the server "listener"
         if (svrSocket != null)
            svrSocket.close();

         // End the multicast listener thread
         if (mcLThread != null)
            mcLThread.stopMCListener();

         // Wait at most 5 seconds for the "process" threads to finish
         if (numOfPThreads.waitForCompletion(5) == false)
            Log.log(Log.SVR, Log.ERR, this.getClass().getName(), "shutdownServer",
                    "Process handlers still executing");
      }
      catch (Exception e) {
         Log.log(Log.SVR, Log.EXCP, this.getClass().getName(), "shutdownServer",
                 e.toString());
      }
   }
}
