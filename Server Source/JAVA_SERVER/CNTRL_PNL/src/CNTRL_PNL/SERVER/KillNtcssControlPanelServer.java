/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package CNTRL_PNL.SERVER;

import java.net.*;

import COMMON.*;
import CNTRL_PNL.COMMON.*;

public class KillNtcssControlPanelServer
{
   public static void main(String[] args)
   {
      int numArgs = args.length;

      // Enable the Just-In-Time compiler if it is available
      Compiler.enable();
      
      // Make sure at least the properties file was passed in
      if (numArgs == 0) {
         System.out.println("Error reading properties file! Cannot continue!!");
         System.exit(1);
      }

      if (numArgs == 1)
         sendKillMessage(args[0], localHostname());
      else if (numArgs == 2)
         sendKillMessage(args[0], args[1]);
      else
         System.out.println("usage: KillNtcssControlPanelServer [host]");
   }

   // Sends a message to the control panel server on the given host indicating
   //  that it should shut down
   public static void sendKillMessage(String propertiesFile, String host)
   {
      try {
         MessageWithData msg;

         // Try to load in the properties file
         if (PropertiesLoader.load(propertiesFile) == true) {

            msg = new MessageWithData(host, Constants.CP_SERVER_PORT);

            // Set up the message
            msg.setHeader(CPMessageHeaders.KILL);

            // Send the message
            if (msg.send() != true)
               System.out.println("Error sending kill message to server");

            // Clean up the message mechanism
            msg.finished();
         }
         else
            System.out.println("Error reading properties file! Cannot continue!!");
      }
      catch (Exception e) {
         System.out.println("Error killing control panel server on " + host +
                            ": " + e);
      }
   }

   // Returns the name of the local host
   private static String localHostname()
   {
      String hostname = null;

      try {
         hostname = InetAddress.getLocalHost().getHostName();
         hostname = hostname.substring(0, hostname.indexOf("."));
      }
      catch (UnknownHostException e) {
         System.out.println("Error getting local hostname: " + e);
      }
      finally {
         return hostname;
      }
   }
}