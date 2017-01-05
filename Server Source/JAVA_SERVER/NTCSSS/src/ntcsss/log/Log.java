/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.log;

import java.text.SimpleDateFormat;

import java.util.Date;
import java.util.Vector;
import java.util.Enumeration;

import java.net.Socket;
import java.net.UnknownHostException;

import java.io.BufferedOutputStream;
import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.InterruptedIOException;

import ntcsss.libs.Constants;
import ntcsss.libs.StringLib;

/**
 * An interface for sending log entries to the logging daemon
 */
public class Log
{   
   public static final char ENTRY_DELIMITER = '\03';
   public static final char MSG_FIELD_DELIMITER = '\04';
   
   // Log entry types
   private static final String DEBUG = "DEBUG";
   private static final String INFO  = "INFO";
   private static final String WARN  = "WARNING";
   private static final String ERR   = "ERROR";   
   private static final String EXCP  = "EXCEPTION";   
   
   private static EntityName defaultEntityName = EntityName.UNKNOWN;
   
   private static boolean debugOn = false;
   
   private static SimpleDateFormat dateFormatter = 
                              new SimpleDateFormat("MMM d HH:mm:ss");
   
   private static LogDaemonConnection logDaemonConnection = null;
   private static Object lock = new Object();
   
   static {      
      // When the JVM stops, the connection to the log daemon will be closed
      Runtime.getRuntime().addShutdownHook(new DaemonConnectionShutdownHook());
   }
      
   
   /**
    * Methods for logging different entry types
    */
   
   public static void debug(String className, String methodName, String text)
   {
      log(DEBUG, className, methodName, text);
   }
   
   public static void info(String className, String methodName, String text)
   {
      log(INFO, className, methodName, text);
   }
   
   public static void warn(String className, String methodName, String text)
   {
      log(WARN, className, methodName, text);
   }
   
   public static void error(String className, String methodName, String text)
   {
      log(ERR, className, methodName, text);
   } 
   
   public static void excp(String className, String methodName, String text)
   {
      log(EXCP, className, methodName, text);
   }
   
   /**
    * Methods for controlling wether debug entries should be logged
    */
   
   public synchronized static void turnDebugOn()
   {
      info("Log", "turnDebugOn", "Turning on debugging");
      debugOn = true;
   }
   
   public synchronized static void turnDebugOff()
   {
      info("Log", "turnDebugOff", "Turning off debugging");
      debugOn = false;
   }
   
   public synchronized static boolean isDebugOn()
   {
      return debugOn;
   }
   
   /**
    * Sets the default entity name to be used for logging
    */
   public static void setDefaultEntityName(EntityName defEntityName)
   {
      defaultEntityName = defEntityName;
   }
   
   /**
    * Logs the given information. 
    *
    *  It was preferable to use a single shared log daemon socket connection per
    *  JVM, as opposed to creating a socket connection to the log daemon for
    *  every time something was logged. This was because the max "backlog" queue 
    *  size for ServerSocket objects on Windows NT Workstation was fixed to only
    *  5. So it would be possible that if the log daemon was flooded with logging
    *  requests, its queue would fill up and subsequent logging requests would
    *  be denied since the queue was full.
    */
   private static void log(String type, String className, 
                                          String methodName, String text)
   {      
      String threadGroupName;
      String entity;      
      String logEntry = null;
      
      try {
         
         // If this is a debug log entry type and debug is not turned on, don't 
         //  log this entry
         if ((type.equals(DEBUG) == true) && (isDebugOn() == false))
            return;
                  
         // Get the name of the thread group the current thread is executing in
         threadGroupName = ((Thread.currentThread()).getThreadGroup()).getName();
         
         // If the thread group is "main", use the default entity name, else
         //  use the thread group name as the name of the entity
         if (threadGroupName.equals("main") == true)
            entity = defaultEntityName.get();
         else
            entity = threadGroupName;
         
         // Create the log entry from the given information
         logEntry = getCurrentTime() + " " + StringLib.padString(entity, 12, ' ') + 
                 StringLib.padString(type, 11, ' ') + 
                 className + "." + methodName + " [" + text + "]";
         
         // Send the log entry to the log daemon
         synchronized (lock) {
            if (logDaemonConnection == null)
               logDaemonConnection = new LogDaemonConnection();
            
            logDaemonConnection.send(logEntry);
         }         
      }
      catch (Exception exception) {       
         System.out.println("LOGGINGERROR " + logEntry);
      }
   }   
   
   /**
    * Closes the connection to the logging daemon
    */
   public static void closeDaemonConnection()
   {
      try {
         synchronized (lock) {
            if (logDaemonConnection != null) {
               Log.info("Log", "closeDaemonConnection", 
                        "Closing log daemon connection");
               logDaemonConnection.close();            
               logDaemonConnection = null;
            }
         }
      }
      catch (Exception e) {
      }
   }
   
   /**
    * Informs the log daemon to create an INI file containing log file entries.
    *  If any errors occur, false is returned.
    */
   public static boolean createSystemLogINI(String outputFile, int blockSize, 
                                            int maxRecordCount, 
                                            int dateFilterCode, 
                                            boolean filterByEntity,
                                            String startDate, String endDate, 
                                            Vector entityList) 
   {   
      String message;
      Enumeration entities;
      String response = null;
      
      try {
         
         // Construct the message to send to the log daemon
         message = Logd.CREATE_INI_MSG + MSG_FIELD_DELIMITER +
                   outputFile + MSG_FIELD_DELIMITER +
                   blockSize + MSG_FIELD_DELIMITER + 
                   maxRecordCount + MSG_FIELD_DELIMITER +
                   dateFilterCode + MSG_FIELD_DELIMITER +
                   (filterByEntity ? 1 : 0) + MSG_FIELD_DELIMITER + 
                   startDate + MSG_FIELD_DELIMITER +
                   endDate + MSG_FIELD_DELIMITER;
                                      
         entities = entityList.elements();
         while (entities.hasMoreElements() == true) 
            message += entities.nextElement() + ",";                    

         synchronized (lock) {
            
            // Create a connection to the log daemon if one hasn't been created
            //  yet
            if (logDaemonConnection == null)
               logDaemonConnection = new LogDaemonConnection();
            
            try {               

               // Send the message to the log daemon
               logDaemonConnection.send(message);
               
               // Wait for a response
               response = logDaemonConnection.receive();
            }
            catch (IOException ioException) {
               return false;
            }                        
         }
                  
         // Check the response
         if ((response == null) ||
             (response.equals("SUCCESS") == false))
            return false;
         else         
            return true;
      }
      catch (Exception exception) {
         
         Log.excp("Log", "createSystemLogINI", exception.toString());
         return false;
      }
   }
   
   /**
    * Returns a string specifing the current time in the format:
    *  Month Data Time
    */
   private static String getCurrentTime()
   {
      String date;

      try {
         date = dateFormatter.format(new Date());
      }
      catch (Exception e) {
         date = " ";
      }

      return date;
   }      
}

/**
 * Represents a connection to the logging daemon.
 *
 *  It was important that this socket connection get closed properly when the JVM 
 *  shutsdown, because if it wasn't, logging requests might get dropped, losing
 *  important logging entries. Overriding the Object.finalize method wasn't
 *  a good solution because, according to the java 1.3.1 documentation, it is 
 *  not guaranteed that all the object's finalize methods would be called on
 *  a system shutdown (and testing of this showed it to be the case). So 
 *  a "shutdown" hook was employed to take care of closing this connection
 *  on JVM shutdown.  
 *
 *  Using this method (and even the Object.finalize method) would prevent
 *  having to explicitly call a "close connection" method for every class 
 *  with a "main" method which used this Log class.
 */
class LogDaemonConnection
{
   private Socket socket;
   private BufferedOutputStream outputStream;
   private BufferedInputStream inputStream;
   private boolean constructorError = false;
   
   /**
    * Creates a connection to the logging daemon
    */
   public LogDaemonConnection()
   {      
      try {   
         socket = new Socket("localhost", Constants.LOG_DAEMON_PORT);    
         outputStream = new BufferedOutputStream(socket.getOutputStream());
         inputStream = new BufferedInputStream(socket.getInputStream());
      }
      catch (Exception exception) {
         constructorError = true;
      }
   }
   
   /**
    * Closes the connection to the logging daemon
    */
   public void close() throws IOException
   {      
      if (constructorError == false) {
         inputStream.close();
         outputStream.close();
         socket.close();
      }
   }
   
   /**
    * Sends the given log entry to the logging daemon
    */
   public void send(String logEntry) throws IOException
   {      
      if (constructorError == false) {
         outputStream.write(logEntry.getBytes());
         outputStream.write(Log.ENTRY_DELIMITER);
         outputStream.flush();           
      }
      else
         throw new IOException("Log daemon connection not opened");
   }
   
   /**
    * Receives a piece of data from the logging daemon
    */
   public String receive() throws IOException
   {      
      int bytesRead = 0;
      byte buffer[];
      String receivedData;
      
      if (constructorError == false) {  
                 
         buffer = new byte[20];
         socket.setSoTimeout(30 * 1000);

         try {
            bytesRead = inputStream.read(buffer, 0, buffer.length);
            receivedData = new String(buffer, 0, bytesRead).trim();
         }
         catch (InterruptedIOException interrupted) {                                                
            receivedData = null;
         }
            
         socket.setSoTimeout(0);  // Clear the timer
      
         if ((receivedData != null) && 
             (receivedData.charAt(receivedData.length() - 1) == (char)Log.ENTRY_DELIMITER))  
            return receivedData.substring(0, receivedData.length() - 1);
         else
            return receivedData;
      }
      else
         throw new IOException("Log daemon connection not opened");
   }
}

/**
 * A shutdown hook for closing the connection to the logging daemon 
 */
class DaemonConnectionShutdownHook extends Thread
{      
   public void run()
   {      
      Log.closeDaemonConnection();      
   }
}
