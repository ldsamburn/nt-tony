/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.tools;

import java.net.Socket;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;

import COMMON.PropertiesLoader;

import ntcsss.libs.StringLib;
import ntcsss.libs.ErrorCodes;
import ntcsss.libs.Constants;
import ntcsss.libs.SocketLib;

/**
 *
 */
public class CustomProc
{
   public static void main(String arguments[])
   {
      String pid, status;
      Socket socket;
      String messageToSend;
      BufferedOutputStream outputStream;
      BufferedInputStream inputStream;
      int bytesRead;
      byte buffer[];
      
      
      try {
         
         // Check the number of arguments
         if (arguments.length != 3) {
            print("USAGE: custom_proc <PID value> <custom process status>");
            System.exit(ErrorCodes.ERROR_INVALID_NUM_ARGS);
         }
         
         // Load the properties file
         if (PropertiesLoader.load(arguments[0]) == false) {
            print("Error loading properties file");
            System.exit(ErrorCodes.ERROR_INVALID_NUM_ARGS);
         } 
         
         // Create a connection to the local process que server daemon
         socket = new Socket("localhost", Constants.SPQ_SVR_PORT);
         outputStream = new BufferedOutputStream(socket.getOutputStream());
         inputStream = new BufferedInputStream(socket.getInputStream());
      
         // Construct the message to send
         pid = StringLib.padString(arguments[1], 
                                   Constants.MAX_PROCESS_ID_LEN, ' ');
         status = StringLib.padString(arguments[2], 
                                      Constants.MAX_CUST_PROC_STAT_LEN, ' ');         
         messageToSend = StringLib.formatMessage("CHANGECUSTPROCSTAT", 
                                                 pid + status + '\0'); 
         
         // Send the message
         outputStream.write(messageToSend.getBytes(), 0, messageToSend.length());
         outputStream.flush();
         
         // Get the response
         buffer = new byte[Constants.CMD_LEN + 10];
         socket.setSoTimeout(5 * 1000);  // 5 seconds
         bytesRead = SocketLib.readStream(inputStream, buffer, buffer.length);                                                 
         
         // Close the system resources
         inputStream.close();
         outputStream.close();
         socket.close();
         
         // Check the response
         if (bytesRead < 0)             
            System.exit(ErrorCodes.ERROR_CANT_TALK_TO_SVR); 
         else
            System.exit(Integer.parseInt(new String(buffer, 0, bytesRead)));
         
      }
      catch (Exception exception) {
         System.exit(ErrorCodes.ERROR_CANT_TALK_TO_SVR);
      }
   }
   
   private static void print(String str)
   {
      System.out.println(str);
   }

}
