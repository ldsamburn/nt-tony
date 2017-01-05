/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.log;

import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.PrintWriter;
import java.io.FileOutputStream;
import java.io.File;
import java.io.InterruptedIOException; 
import java.io.RandomAccessFile; 

import java.util.LinkedList;
import java.util.StringTokenizer;
import java.util.Vector;
import java.util.Hashtable;

import COMMON.PropertiesLoader;
import COMMON.KillFlag;
import COMMON.INI;

import ntcsss.libs.Constants;
import ntcsss.libs.StringLib;

/**
 * NTCSS logging daemon
 */
public class Logd
{      
   public static final String STOP_DAEMON_MSG = "STOPLOGGINGDAEMON";
   public static final String CREATE_INI_MSG = "CREATEINI";
   public static KillFlag killFlag = new KillFlag();
   
   private static PrintWriter logFile = null; 
   private static final int MAX_LOG_FILE_SIZE = 524288; // 512 kilobytes
   private static long currentFileSize = 0;
   private static final int LINE_SEPARATOR_SIZE = 
                           System.getProperty("line.separator", "\n").length();
   public static final int MAX_NUM_HISTORY_FILES = 5;
   private static boolean echoOn = false;   
   
   public static void main(String args[])
   {            
      SocketQueue queue;
      Listener listener;
      ActiveLogRequests activeRequests;
      
      try {
         
         // Load the properties file
         if ((args.length < 1) || (PropertiesLoader.load(args[0]) == false)) {
            System.out.println("Error loading properties file");
            System.exit(1);
         }         
         
         // Check to see if it is desired to just shutdown the log daemon
         if ((args.length == 2) &&
             (args[1].equals("-stop") == true)) {      
                
            // Send a message to the log daemon to indicate that it should
            //  shutdown
            stopDaemon();
            return;
         }
         else {  // Start the logging daemon
            
            if ((args.length == 2) &&
                (args[1].equals("-e") == true))
               echoOn = true;
            
            // Create the data structure that will keep track of the number
            //  of requests to this log daemon that are being serviced.
            activeRequests = new ActiveLogRequests();
                  
            // Create a socket listener and a queue into which the listener
            //  will place received connections
            queue = new SocketQueue();         
            listener = new Listener(queue);                           
         
            // Process incoming connections from the queue until the daemon is 
            //  requested to shutdown 
            while (killFlag.isSet() == false) {
           
               // If there is a socket in the queue, remove it and 
               //  process the data from it
               if (queue.isEmpty() == true) 
                  Thread.sleep(100); // milliseconds
               else {
   
                  // Remove the first connection from the queue and process it
                  new Handler(queue.removeFirst(), activeRequests);                                                      
               }
            }      
         
            // Close the socket listener
            listener.close();
         
            // Process any log entries left in the queue
            while (queue.isEmpty() == false)
               new Handler(queue.removeFirst(), activeRequests);
            
            // Wait for the sepecified period of seconds (at most) to give 
            //  outstanding requests an opportunity to finish. 
            activeRequests.waitForCompletion(5);
         
            // Close the log file. This could be done potentially while
            //  threads are still trying to write to the file
            if (logFile != null) {
               logFile.close(); 
               logFile = null;
            }
         }
      }
      catch (Exception exception) {
         System.out.println(exception.toString());
      }
   }
   
   /**
    * Logs the given string. This method is synchronized to prevent multiple
    *  threads from writing to the log file at the same time.
    */
   public static synchronized void log(String logEntry)
   {
      int entrySize;
      File fileDescp;
           
      try {
         
         // Get the size of the log file if it hasn't been opened yet
         if (logFile == null) {
            fileDescp = new File(Constants.NTCSS_LOG_FILE);
            if (fileDescp.exists() == true)
               currentFileSize = fileDescp.length();
            else
               currentFileSize = 0;
         }
         
         // Check to see if the log file has reached its max size. If it has
         //  update the history files
         entrySize = logEntry.length() + LINE_SEPARATOR_SIZE;
         if ((currentFileSize + entrySize) > MAX_LOG_FILE_SIZE) {
            
            // Close the log file if it is open
            if (logFile != null) {
               logFile.close();
               logFile = null;
            }
                        
            // Create the history files. The maximum number of history files
            //  there should be at any time is MAX_NUM_HISTORY_FILES (this
            //  number does not include the log file itself).
            for (int i = MAX_NUM_HISTORY_FILES; i > 0; i--) {
               
               fileDescp = new File(Constants.NTCSS_LOG_FILE + "_" + i);
               
               if (fileDescp.exists() == true) {
                  if (i == MAX_NUM_HISTORY_FILES)
                     fileDescp.delete();
                  else
                     fileDescp.renameTo(new File(Constants.NTCSS_LOG_FILE + 
                                        "_" + (i + 1)));
               }
            }            
            fileDescp = new File(Constants.NTCSS_LOG_FILE);
            fileDescp.renameTo(new File(Constants.NTCSS_LOG_FILE + "_1"));            
            
            // Reset the size count of the log file
            currentFileSize = 0;
         }
         
         // If the log file hasn't been opened for writing, open it
         if (logFile == null)                                                
            logFile = new PrintWriter(
                         new FileOutputStream(Constants.NTCSS_LOG_FILE, true));         
         
         
         // Write the given string to the log file
         logFile.println(logEntry);
         logFile.flush();
         
         if (echoOn == true)
            System.out.println(logEntry);
                  
         // Update the size count of the log file
         currentFileSize += entrySize;
      }
      catch (Exception exception) {
         
         // If there is any exception, just write the log entry to standard out
         System.out.println("LOGERROR " + logEntry);
      }
   }     
   
   /**
    * Creates an INI file from the entries in the log file. A response is
    *  sent back over the given socket output stream.
    */
   public static synchronized void createINIFile(BufferedOutputStream outputStream, 
                                                 String data)
   {
      String response;
      StringTokenizer tokenizer;
      String outputFile;
      int blockSize;
      int maxRecordCount; 
      int dateFilterCode;
      boolean filterByEntity;
      String startDate, endDate;      
      Vector entityList = null;
      OutputINICreator iniCreator;
      
      try {         
         
         // Parse the given input parameters
         tokenizer = new StringTokenizer(data, String.valueOf(Log.MSG_FIELD_DELIMITER));
         tokenizer.nextToken(); // Skip this token, "CREATE"
         outputFile = tokenizer.nextToken();
         blockSize = Integer.parseInt(tokenizer.nextToken());
         maxRecordCount = Integer.parseInt(tokenizer.nextToken());
         dateFilterCode = Integer.parseInt(tokenizer.nextToken());
         filterByEntity = (Integer.parseInt(tokenizer.nextToken()) == 1 ? true : false);
         startDate = tokenizer.nextToken();
         endDate = tokenizer.nextToken();         

         if (filterByEntity == true) {
            tokenizer = new StringTokenizer(tokenizer.nextToken(), ",");
            entityList = new Vector();
            while (tokenizer.hasMoreElements() == true)
               entityList.add(tokenizer.nextToken());
         }             
         
         // Attempt to create the INI file of log entries
         iniCreator = new OutputINICreator(outputFile, blockSize, 
                                           maxRecordCount, dateFilterCode, 
                                           filterByEntity, startDate, endDate,
                                           entityList);
         if (iniCreator.create() == true)
            response = "SUCCESS";
         else
            response = "ERROR";
                           
         // Send a response back
         outputStream.write(response.getBytes());
         outputStream.flush();
      }
      catch (Exception exception) {
         try {
            response = "ERROR";
            outputStream.write(response.getBytes());
            outputStream.flush();
         }
         catch (Exception exp) {
         }
      }
   }  
   
   /**
    * Stops this logging daemon
    */
   private static void stopDaemon()
   {
      Socket connection;
      BufferedOutputStream outputStream;
      
      try {
         
         // Send a message to the logging daemon to indicate that it
         //  should shutdown
         connection = new Socket("localhost", Constants.LOG_DAEMON_PORT);         
         outputStream = new BufferedOutputStream(connection.getOutputStream());         
         outputStream.write(STOP_DAEMON_MSG.getBytes());         
         outputStream.close();
         connection.close();
      }
      catch (Exception exception) {                  
         System.out.println("Exception occured: " + exception.toString());
      }
   }
}

/**
 * A data structure that keeps track of the number of requests being processed
 *  by the log daemon
 */
class ActiveLogRequests
{    
   private int activeCount;
   
   /**
    * Constructor
    */
   public ActiveLogRequests()
   {
      activeCount = 0;
   }
   
   /**
    * Increments the active request count by one
    */
   public synchronized void increment()
   {      
      activeCount++;
   }
   
   /**
    * Decrements the active request count by one
    */
   public synchronized void decrement()
   {      
      activeCount--;
      
      // If the active count is now zero, notify anyone who may be waiting
      //  for this state
      if (activeCount == 0)
         notify();
   }
      
   /**
    * This method will cause the calling thread to wait until all the 
    *  requests have been processed or the given timeout
    *  period (in seconds) has elapsed.  
    *
    * NOTE: This method assumes that the "increment" method will not be called 
    *       any more.
    */
   public synchronized void waitForCompletion(int timeout)
   {
      try {
         
         // Only need to wait if the active count is greater than 0
         if (activeCount > 0) {
            
            Log.info("ActiveLogRequests", "waitForCompletion", "Waiting for <" + activeCount 
                     + "> request(s) to finish ...");
            
            // "Sleep" for the given period of time (in milliseconds)
            wait((long)timeout * 1000);  
            
            // Check to see if all the requests have been processed
            if (activeCount > 0) 
               Log.warn("ActiveLogRequests", "waitForCompletion", "<" + activeCount + 
                        "> request(s) still processing");                           
         } 
                  
      }
      catch (Exception exception) {
         Log.excp("ActiveLogRequests", "waitForCompletion", exception.toString());                           
      }
   }      
}

/**
 * A queue which contains socket objects
 */ 
class SocketQueue
{
   private LinkedList queue;

   /**
    * Constructor
    */
   public SocketQueue()
   {
      queue = new LinkedList();
   }

   /**
    * Removes the first item in the queue
    */
   public synchronized Socket removeFirst()
   {
      return (Socket)queue.removeFirst();
   }

   /**
    * Checks to see if the queue is empty
    */
   public synchronized boolean isEmpty()
   {
      return queue.isEmpty();
   }

   /**
    * Places a socket at the end of the queue
    */
   public synchronized void addLast(Socket socket)
   {
      queue.addLast(socket);
   }
}

/**
 * Listens for incoming connections to this daemon. When a connection
 *  is received, it is placed in a queue.
 */
class Listener extends Thread
{
   private SocketQueue queue;
   private ServerSocket listener;
   
   public Listener(SocketQueue queue)
   {
      this.queue = queue;
      listener = null;
      start();
   }
   
   public void run()
   {            
      try {
         listener = new ServerSocket(Constants.LOG_DAEMON_PORT, 
                                     Constants.SERVER_BACKLOG_QUEUE_SIZE);
         
         while (true)         
            queue.addLast(listener.accept());
      } 
      catch (SocketException sktException) {
         // This will be thrown when the close method is called       
      }
      catch (Exception exception) {        
         System.out.println(exception.toString());
      }
   }
   
   public void close()
   {
      try {
         if (listener != null)
            listener.close();
      }           
      catch (Exception exception) {        
         System.out.println(exception.toString());
      }
   }
}

/**
 * Reads the log entries from the given socket connection until the
 *  stream is closed
 */
class Handler extends Thread
{
   Socket connection;
   ActiveLogRequests activeRequests;
   
   public Handler(Socket connection, ActiveLogRequests activeRequests)
   { 
      this.connection = connection;
      this.activeRequests = activeRequests;
      
      this.activeRequests.increment();
      
      // Make this a "daemon" thread so that when the log daemon is stopped, 
      //  it won't wait for any of these worker threads to finish if they are 
      //  still processing.
      setDaemon(true); 
         
      start();
   }
   
   /**
    * Reads the log entries from the socket connection until the
    *  stream is closed. Each log entry (delimited by a special character
    *  sequence) is logged.
    */
   public void run()
   {      
      BufferedInputStream inputStream;
      BufferedOutputStream outputStream;
      int input;      
      StringBuffer logEntry = new StringBuffer();
      
      try {
         
         // Create streams from the socket
         inputStream = new BufferedInputStream(connection.getInputStream());
         outputStream = new BufferedOutputStream(connection.getOutputStream());
                  
         // As long as the end of the stream hasn't been reached, read
         //  each log entry and send it to be logged.
         logEntry.setLength(0);
         while ((input = inputStream.read()) != -1) {
               
            // If this is the log entry delimiter, log this log entry
            if (input == Log.ENTRY_DELIMITER) {
               
               if (logEntry.toString().equals(Logd.STOP_DAEMON_MSG) == true) {
                  Logd.killFlag.set();  // Indicate that the daemon should shut down
                  logEntry.setLength(0);
                  break;
               }
               else if (logEntry.toString().startsWith(Logd.CREATE_INI_MSG) == true) {
                  Logd.createINIFile(outputStream, logEntry.toString());
                  logEntry.setLength(0);
               }
               else {
                  Logd.log(logEntry.toString());
                  logEntry.setLength(0);
               }                              
            }
            else
               logEntry.append((char)input);
         }   
         
         // This would be the case if a log entry was sent and the connection
         //  was closed without sending a delimiting sequence
         if (logEntry.length() != 0) {
            if (logEntry.toString().equals(Logd.STOP_DAEMON_MSG) == true)
               Logd.killFlag.set(); // Indicate that the daemon should shut down               
            else
               Logd.log(logEntry.toString());
         }
            
         // Close the system resources
         inputStream.close();
         outputStream.close();
         connection.close();
      }
      catch (SocketException sktException) {     
         // Ignore these exceptions which could be caused by a logging client
         //  exiting without closing its socket connection
      }      
      catch (Exception exception) {        
         System.out.println(exception.toString());
      }
      
      // Decrement the count of active log requests
      activeRequests.decrement();
   }      
}

/**
 * Creates an INI file with the most recent log entries that meet given 
 *  filter criterias
 */
class OutputINICreator
{   
   // Input parameters
   private final String  outputFile;
   private final int     blockSize;           // The number of bytes to read out 
   private final int     maxRecordCount;      //  of the log file at one time 
   private final int     dateFilterCode;
   private final boolean filterByEntity;
   private final String  startDate, endDate;      
   private final Vector  entityList;
   
   // A buffer to hold a chunk of bytes read from the log file
   private byte dataBlock[];     
   
   // A buffer to hold a fragmented log entry found at the very beginning or
   //  end of a block of data read from log file
   private final StringBuffer fragmentedEntry; 
   
   // Number of records which pass the filters
   private int recordCount;
   
   private INI ini;
   private String localHostname;
   private String section[];
   private Hashtable monthList;
   
   /**
    * Constructor
    */
   public OutputINICreator(String outputFile, int blockSize, int maxRecordCount,
                           int dateFilterCode, boolean filterByEntity,
                           String startDate, String endDate, Vector entityList)
   {
      this.outputFile     = outputFile;
      this.blockSize      = blockSize * 1024;
      this.maxRecordCount = maxRecordCount; 
      this.dateFilterCode = dateFilterCode;
      this.filterByEntity = filterByEntity;
      this.startDate      = startDate;
      this.endDate        = endDate;      
      this.entityList     = entityList;    
      
      dataBlock = null;
      fragmentedEntry = new StringBuffer();
      recordCount = 0;
      
      ini = new INI();
      localHostname = StringLib.getHostname();
      section = new String[6];
      
      // Create a table of all the months
      monthList = new Hashtable();
      monthList.put("Jan", "01");
      monthList.put("Feb", "02");
      monthList.put("Mar", "03");
      monthList.put("Apr", "04");
      monthList.put("May", "05");
      monthList.put("Jun", "06");
      monthList.put("Jul", "07");
      monthList.put("Aug", "08");
      monthList.put("Sep", "09");
      monthList.put("Oct", "10");
      monthList.put("Nov", "11");
      monthList.put("Dec", "12");
   }
   
   /**
    * Reads through the log file (and any history files) for the most recent
    *  log entries which match the given filter criteria.  When these log 
    *  entries are found, they are written out to an INI file. If any errors 
    *  occur, false is returned.
    */
   public boolean create()
   {
      File fileDescp;           
      
      try {
         
         // Delete the output INI file if it exists
         fileDescp = new File(outputFile);
         if (fileDescp.exists() == true)
            fileDescp.delete();
         
         // Make sure the log file exists
         fileDescp = new File(Constants.NTCSS_LOG_FILE);
         if (fileDescp.exists() == false)
            throw new GoTo();
         
         // Create the buffer into which blocks of file data will be read into
         dataBlock = new byte[blockSize];
         
         // Process the log entries in the main log file
         processFile(Constants.NTCSS_LOG_FILE);                  
         
         // Check to see if the maximum number of log entries have been retrieved
         if (maxRecordsRetrieved() == true)
            throw new GoTo();
         
         // Process the log entries in any history log files
         for (int i = 1; i <= Logd.MAX_NUM_HISTORY_FILES; i++) {
            
            // Make sure the log file exists
            fileDescp = new File(Constants.NTCSS_LOG_FILE + "_" + i);
            if (fileDescp.exists() == false)
               break;
         
            // Process the log entries in this history file
            processFile(Constants.NTCSS_LOG_FILE + "_" + i);
         
            // Check to see if the maximum number of log entries has been 
            //  retrieved
            if (maxRecordsRetrieved() == true)
               break;
         }                           
         
         throw new GoTo();
      }
      catch (GoTo goTo) {
         
         // Help out the garbage collector by signaling that this 
         //  potentially large object is no longer in use
         if (dataBlock != null)
            dataBlock = null;
         
         // Write out to the INI file the number of log entries retrieved
         ini.WriteProfileString("S", "RECORD_COUNT", 
                                String.valueOf(recordCount), outputFile);  
         ini.FlushINIFile(outputFile);
         
         return true;
      }
      catch (Exception exception) {
         System.out.println("Creation exception <" + exception.toString() + ">");         
         return false;
      }
   }
   
   /**
    * Reads through the given log file looking for the most recent log entries 
    *  that match the given criteria.
    *
    * Blocks of data are read, one at a time, from the file starting at the 
    *  bottom and moving towards the top (since the most recent log entries 
    *  would be at the bottom of the file).
    *
    * Each block of data is then parsed into log entries, which are then checked
    *  against the filters to see if they are to be added to the output INI file.        
    *
    * If an error occurs, false is returned.
    */
   private boolean processFile(String logFilename)
   {      
      RandomAccessFile file = null;      
      long filePosition;      
      int bytesRead;
      int bytesToRead;      
      boolean lastBlockReached = false;      
                          
      try {                                   

         // Open the log file for reading
         file = new RandomAccessFile(logFilename, "r");
         
         // Go to the very end of the file
         filePosition = file.length();

         do {

            // If the "last" block in this file (the one at the very top of the
            //  file, since we're going bottom to top) has been read, 
            //  then we're finished
            if (lastBlockReached == true) {

               // Process any fragmented (now complete) log entry left in the
               //  buffer
               if (fragmentedEntry.length() > 0) {
                  processLogEntry(fragmentedEntry.reverse().toString());         
                  fragmentedEntry.setLength(0);
               }
               
               break;
            }                                     

            // Determine the file position to read the next block from
            filePosition -= blockSize;

            // Determine how many bytes to read. Usually this number is 
            //  "blockSize". If filePosition is negative, that means we are at 
            //  the top of the file and there is less than a block worth of 
            //  data there at the top left to read.
            if (filePosition < 0) {

               // Determine the size of this smaller block of data to read
               // Note: "filePosition" is negative here
               bytesToRead = blockSize + (int)filePosition;  

               // Set the file position to be at the beginning of the file
               filePosition = 0;
            }
            else                               
               bytesToRead = blockSize;            
            
            // If filePosition is 0, that means we are at the top of the file,
            //  and this is the last block of data that needs to be processed.
            if (filePosition == 0)
               lastBlockReached = true;
            
            // Go to the new file position in the file to read the next 
            //  block of data
            file.seek(filePosition);

            // Read the next block of data
            bytesRead = file.read(dataBlock, 0, bytesToRead);                                                  
        
            // Process this block of data
            processBlock(bytesRead);
            
            // If the maximum number of log entries have been retrieved, we're
            //  done
            if (maxRecordsRetrieved() == true)
               break;

         } while (true);   
         
         // Close the log file
         file.close();
         file = null;
                  
         return true;
      }
      catch (Exception exception) {
         System.out.println("File processing exception <" + 
                            exception.toString() + ">"); 

         // Try to close the log file
         if (file != null)
            try {
               file.close();
               file = null;
            }
            catch (Exception exp) {
            }
         
         return false;
      }
   }  
      
   /**
    * Reads through the block of data retreived from the log file looking for
    *  the most recent log entries that meet the given criteria.
    *
    * Since we're interested in the most recent log entries, we start at bottom 
    *  of block and move upwards.
    *
    * If an error occurs, false is returned.
    */
   
   private boolean processBlock(int bytesRead)
   {
      int currentPosition = bytesRead - 1;  
      StringBuffer entry = new StringBuffer();  
      
      try {
    
         // It is possible to have a fragmented log entry at the very end
         //  (and beginning) of this block of file data. If the very last 
         //  character in this block is not a terminator character ('\n' or '\r'),
         //  which indicate the end of a log entry, then that means the last
         //  log entry in this block is fragmented.                  
         if (isTerminatorChar(dataBlock[currentPosition]) == false) {

            // Store this fragmented log entry in the "fragmented entry" buffer
            while ((currentPosition >= 0) &&
                   (isTerminatorChar(dataBlock[currentPosition]) == false))                   
               fragmentedEntry.append((char)dataBlock[currentPosition--]);
            
            // If we've reached the top of this block of data, we've finished
            //  processing it
            if (currentPosition < 0)
               return true;            
         }

         // If there is any data in the "fragmented entry" buffer, process it
         //  first
         if (fragmentedEntry.length() > 0) {
            processLogEntry(fragmentedEntry.reverse().toString());         
            fragmentedEntry.setLength(0);
            
            // If the maximum number of log entries have been retrieved, we're
            //  done
            if (maxRecordsRetrieved() == true)
               return true;
         }
         
         // Read through the rest of the block of file data
         do {
            
            // Skip over termniator characters ('\n' or '\r')
            while ((currentPosition >= 0) &&
                   (isTerminatorChar(dataBlock[currentPosition]) == true))                   
               currentPosition--;
         
            // If we've reached the top of this block of data, we've finished
            //  processing it
            if (currentPosition < 0)
               break;
         
            // Read off the next log entry
            while ((currentPosition >= 0) &&
                   (isTerminatorChar(dataBlock[currentPosition]) == false))                   
               entry.append((char)dataBlock[currentPosition--]);
         
            // We know we've reached the beginning of a log entry if the next
            //  character is a terminator character. If we're at the top of 
            //  the block of data, the current log entry we've been reading may
            //  be fragmented, so store it off in the "fragmented entry" buffer.
            if (currentPosition < 0) {
               fragmentedEntry.append(entry.toString());
               break;
            }
            else {
               
               // We've got a complete log entry, so process it
               processLogEntry(entry.reverse().toString());         
               entry.setLength(0);
               
               // If the maximum number of log entries have been retrieved, we're
               //  done
               if (maxRecordsRetrieved() == true)
                  break;
            }
            
         } while (true);                   
         
         return true;
      }
      catch (Exception exception) {
         System.out.println("Block processing exception <" + 
                            exception.toString() + ">");
         return false;
      }
   }
   
   /**
    * Checks to see if the given log entry meets the given criteria. If it 
    *  does, it is written out to the output INI file. If an error occurs,
    *  false is returned.
    */
   private boolean processLogEntry(String logEntry)
   {
      StringTokenizer tokenizer;
      String month, day, time, entity, data, monthInt;      
      
      try {
         
         // If the maximum number of log entries have been retrieved, we're
         //  done
         if (maxRecordsRetrieved() == true)
            return true;
         
         // Check the length of this log entry   
         if ((logEntry == null) ||
             (logEntry.trim().length() == 0))
            return true;
      
         // Trim any whitespace from the log entry
         logEntry = logEntry.trim();
         
         // Get the pieces of information from this log entry
         tokenizer = new StringTokenizer(logEntry, " ");
         if (tokenizer.hasMoreElements() == false)
            return true;
         
         month = tokenizer.nextToken();
         if (tokenizer.hasMoreElements() == false)
            return true;
         
         day = tokenizer.nextToken();
         if (tokenizer.hasMoreElements() == false)
            return true;
         
         time = tokenizer.nextToken();
         if (tokenizer.hasMoreElements() == false)
            return true;
         
         entity = tokenizer.nextToken();
         if (tokenizer.hasMoreElements() == false)
            return true;  
         
         // Set the delimiter to some character that is guaranteed not to 
         //  be in the string
         data = tokenizer.nextToken(String.valueOf('\0'));
         
         // Convert the month to its numeric equivalent
         monthInt = (String)monthList.get(month);
         if (monthInt == null)
            monthInt = "00";
         
         // If a date filter is given, check the date
         if ((dateFilterCode > 0) && (checkDate(monthInt, day, time) == false))
            return true;
                  
         // If an entity filter is given, check the entity
         if ((filterByEntity == true) && (checkEntity(entity) == false))
            return true;                    
         
         // Increment the number of records that have meet the filter criteria
         recordCount++;
         
         // Add the information about this log entry to the INI file
         section[0] = "A=" + String.valueOf(recordCount);
         section[1] = "B=" + monthInt + "/" + day + " " + time;
         section[2] = "C=" + localHostname;
         section[3] = "D=" + entity;
         section[4] = "E=" + "N/A";   // PID field
         section[5] = "F=" + data;
         ini.WriteProfileSection("S " + recordCount, section, outputFile);                                                           
      
         return true;
      }
      catch (Exception exception) {
         System.out.println("Entry processing exception <" + 
                            exception.toString() + ">");
         return false;
      }
   } 
   
   /**
    * Returns true if the given character is a terminator character ('\n' or '\r')
    */
   private boolean isTerminatorChar(byte chara)
   {
      if ((chara == '\n') || (chara == '\r'))
         return true;
      else
         return false;
   }   
   
   /**
    * Checks to see if the number of retrieved log entries that match the
    *  given criteria is equal to the number of records desired for the output
    *  file.
    */
   private boolean maxRecordsRetrieved()
   {
      if (recordCount >= maxRecordCount)
         return true;
      else
         return false;
   }
   
   /**
    * Checks to see if the given date passes the date filter. Returns true
    *  if the given date passes the filter.
    */
   private boolean checkDate(String month, String day, String time)
   {
      String date;
      StringTokenizer tokenizer;
      boolean ret = false;
      
      try {
         
         tokenizer = new StringTokenizer(time, ":");
         date = month + StringLib.valueOf(Integer.parseInt(day), 2) + 
                StringLib.valueOf(Integer.parseInt(tokenizer.nextToken()), 2) + 
                tokenizer.nextToken() + 
                tokenizer.nextToken();
         
         switch (dateFilterCode) {
            case 1:  // Before
               ret = (date.compareTo(startDate) <= 0); 
               break;
            case 2:  // Between
               ret = ((date.compareTo(startDate) >= 0) &&
                      (date.compareTo(endDate) <= 0));
               break;
            case 3:  // After
               ret = (date.compareTo(endDate) >= 0);
               break;
         }                                                    
         
         tokenizer = null;
         date = null;
         
         return ret;
      }
      catch (Exception exception) {
         return false;
      }
   }
   
   /**
    * Checks to see if the given entity passes the entity filter. Returns true
    *  if the given entity passes the filter.
    */
   private boolean checkEntity(String entity)
   {      
      if (entityList != null)
         return entityList.contains(entity);
      else
         return false;
   }
   
   /**
    * A simple class to make program flow control a little easier
    */
   class GoTo extends Throwable
   {
   }
}
