/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.util;

import java.net.ServerSocket;
import java.net.Socket;

import java.io.BufferedOutputStream;
import java.io.BufferedInputStream;
import java.io.InterruptedIOException;
import java.io.File;

import COMMON.PropertiesLoader;
import COMMON.KillFlag;

import ntcsss.log.Log;
import ntcsss.log.EntityName;

import ntcsss.libs.Constants;
import ntcsss.libs.PrintLib;

/**
 * A simple dameon that, when a connection is made to it, 
 *  returns a comma delimited list of printers found on the OS
 */
public class PrinterListDaemon
{
/*   
   private static KillFlag killFlag = new KillFlag();
   
   public static void main(String arguments[])
   {
      ServerSocket listener;   
      Socket connection;
      BufferedOutputStream outputStream;
      
      try {
         
         Log.setDefaultEntityName(EntityName.PRT_LISTD);
         
         // The first argument should always be the properties file.
         //  Load the properties file
         if ((arguments.length < 1) || 
             (PropertiesLoader.load(arguments[0]) == false)) {
            Log.error("PrinterListDaemon", "main", 
                      "Error loading properties file. Exiting");
            System.exit(1);
         }
         
         // If the "-stop" command line parameter is sent, inform the
         //  daemon to shutdown, and then just exit
         if ((arguments.length == 2) &&
             (arguments[1].equals("-stop") == true)) {
            try {
               connection = new Socket("localhost", Constants.PRT_LISTD_PORT);
               outputStream = new BufferedOutputStream(connection.getOutputStream());
                       
               outputStream.write(3);
               outputStream.flush();
               outputStream.close();
               connection.close();
            }
            catch (Exception exception) {
               System.out.println(exception.toString());
            }
            return;                
         }
         
         // Create a listener for incoming connections
         listener = new ServerSocket(Constants.PRT_LISTD_PORT,
                                     Constants.SERVER_BACKLOG_QUEUE_SIZE);         
         listener.setSoTimeout(5000);         
         while (killFlag.isSet() == false) {
            
            // When a connection is made, thread off to handle the request
            try {
               new Worker(listener.accept());
            }
            catch (InterruptedIOException interrupted) {
               // A timeout occurred so we could wake up and check the kill
               //  flag
            }
         }
         listener.close();

         Log.info("PrinterListDaemon", "main", "Shutdown");
      }
      catch (Exception exception) {
         Log.excp("PrinterListDaemon", "main", exception.toString());
      }
   }   
   
   / **
    * Processes an incoming connection to the printer list daemon
    * /
   static class Worker extends Thread
   {
      Socket connection;
   
      public Worker(Socket connection)
      {
         this.connection = connection;
         
         // Make this a "daemon" thread so that the printer list daemon doesn't
         //  have to wait for all these threads it created to finish before 
         //  it returns
         setDaemon(true);
       
         start();
      }
   
      public void run()
      {
         String printerList;
         BufferedOutputStream outputStream = null;
         BufferedInputStream inputStream;
      
         try {
            
            // Get the printer list and send it back to the other end of the 
            //  socket
            if ((printerList = PrintLib.getSystemPrinterList()) != null) {
               outputStream = new BufferedOutputStream(connection.getOutputStream());
               outputStream.write(printerList.getBytes());
               outputStream.flush();               
            }
            
            // Normally, a client of this daemon will not send any data; it
            //  will just connect and read whatever data is sent. When this
            //  daemon is to be shutdown, a client is created which actually
            //  sends some data. So we must check to see if we got any data
            //  sent to us, which means that the daemon is to shutdown. 
            //
            // NOTE: It makes more logical sense to put this check before
            //  the printer list is sent, that way if it is determined that
            //  the daemon is to shutdown (i.e. data was received) then there
            //  is no need to send the printing list also to the client. But
            //  because of timing issues, this check may report that no data 
            //  is available yet even when a client has sent data (it just
            //  hasn't gotten here yet). So the sending of the printer list is
            //  kind of like a NOOP to give us enough time to receive whatever
            //  data is coming in. 
            inputStream = new BufferedInputStream(connection.getInputStream());
            if (inputStream.available() != 0) { 
               Log.info("PrinterListDaemon", "run", "Received shutdown request");   
               killFlag.set();
            }

            // Close all the resources
            if (outputStream != null)
               outputStream.close();
            inputStream.close();
            connection.close();
         }
         catch (Exception exception) {
            Log.excp("PrinterListDaemon", "run", exception.toString());
         }
      }
   }
 */
   public static void main(String arguments[])
   {
      ServerSocket listener;   
      File stopFile;
      
      try {
         
         Log.setDefaultEntityName(EntityName.PRT_LISTD);
         
         // The first argument should always be the properties file.
         //  Load the properties file
         if ((arguments.length < 1) || 
             (PropertiesLoader.load(arguments[0]) == false)) {
            Log.error("PrinterListDaemon", "main", 
                      "Error loading properties file. Exiting");
            System.exit(1);
         }
         
         // If the "-stop" command line parameter is sent, inform the
         //  daemon to shutdown, and then just exit
         if ((arguments.length == 2) &&
             (arguments[1].equals("-stop") == true)) {
            try {
               stopFile = new File(Constants.NTCSS_LOGS_DIR + "/stopprt");
               stopFile.createNewFile();
            }
            catch (Exception exception) {
               System.out.println(exception.toString());
            }
            return;                
         }
         
         // Remove the stop file if it exists
         stopFile = new File(Constants.NTCSS_LOGS_DIR + "/stopprt");
         if (stopFile.exists() == true)
            stopFile.delete();
         
         // Create a listener for incoming connections
         listener = new ServerSocket(Constants.PRT_LISTD_PORT,
                                     Constants.SERVER_BACKLOG_QUEUE_SIZE);         
         listener.setSoTimeout(5000);         
         while (stopFile.exists() == false) {
            
            // When a connection is made, thread off to handle the request
            try {
               new Worker(listener.accept());
            }
            catch (InterruptedIOException interrupted) {
               // A timeout occurred so we could wake up and check the kill
               //  flag
            }
         }
         listener.close();
         
         // Remove the stop file if it exists
         if (stopFile.exists() == true)
            stopFile.delete();

         Log.info("PrinterListDaemon", "main", "Shutdown");
      }
      catch (Exception exception) {
         Log.excp("PrinterListDaemon", "main", exception.toString());
      }
   }   
   
   /**
    * Processes an incoming connection to the printer list daemon
    */
   static class Worker extends Thread
   {
      Socket connection;
   
      public Worker(Socket connection)
      {
         this.connection = connection;
         
         // Make this a "daemon" thread so that the printer list daemon doesn't
         //  have to wait for all these threads it created to finish before 
         //  it returns
         setDaemon(true);
       
         start();
      }
   
      public void run()
      {
         String printerList;
         BufferedOutputStream outputStream = null;         
      
         try {
            
            // Get the printer list and send it back to the other end of the 
            //  socket
            if ((printerList = PrintLib.getSystemPrinterList()) != null) {
               outputStream = new BufferedOutputStream(connection.getOutputStream());
               outputStream.write(printerList.getBytes());
               outputStream.flush();
               outputStream.close();
            }
            
            connection.close();
         }
         catch (Exception exception) {
            Log.excp("PrinterListDaemon", "run", exception.toString());
         }
      }
   }
   
}