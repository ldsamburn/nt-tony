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

import java.util.Arrays;

import java.io.InterruptedIOException;
import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;

import COMMON.KillFlag;
import COMMON.FTPPacket;
import COMMON.FTPLib;
import COMMON.PropertiesLoader;

import ntcsss.log.EntityName;
import ntcsss.log.Log;

import ntcsss.libs.Constants;

/**
 * Provides the NTCSS implementation of a network copy server daemon. 
 * 
 * This object implements the Runnable interface so that it could be started 
 *  from another program as a thread.
 */
public class FTPd implements Runnable
{     
   public static KillFlag killFlag;
   
   public static StateFlag debugFlag = new StateFlag(false);
   public static StateFlag metricsFlag = new StateFlag(false);               
   
   public static final int STOP_DAEMON      = -123; 
   public static final int TURN_DEBUG_ON    = -124; 
   public static final int TURN_DEBUG_OFF   = -125; 
   public static final int TURN_METRICS_ON  = -126; 
   public static final int TURN_METRICS_OFF = -127; 
   
   /**
    * The method for starting the daemon.
    */
   public static void main(String arguments[]) 
   {
      FTPd ftpDaemon;
      
      try {      
         
         // The first argument should always be the properties file.
         //  Load the properties file
         if ((arguments.length < 1) || 
             (PropertiesLoader.load(arguments[0]) == false)) {
            System.out.println("Error loading properties file");
            System.exit(1);
         }
         
         ftpDaemon = new FTPd();                  
         
         // Currently, arguments are only used to modify the state of the
         //  running FTP daemon. So if there are any arguments, process
         //  these. Otherwise, the intent of the user must be to start the
         //  daemon
         if (arguments.length > 1)
            ftpDaemon.processArguments(arguments);
                           
         else            
            // Start the FTP daemon
            ftpDaemon.run();     
         
            // To start this FTP daemon as a thread...
            // new Thread(ftpDaemon).start();
         
      }
      catch (Exception exception) {
         System.out.println("Error occurred: [" + exception.toString() + "]");
      }
   }

   /**
    * Constructor
    */
   public FTPd() 
   {
      // Set up so that all threads kicked off from this object will be logged
      //  as the given entity group            
      Log.setDefaultEntityName(EntityName.FTPD_GRP);            
      
      killFlag = new KillFlag();
   }
   
   /**
    * Parses the given arguments
    */
   public void processArguments(String arguments[]) 
   {            
      try {
         
         // Only one argument is allowed on the command line, so just 
         //  look at the first one given         
            
         if (arguments[1].equals("-stop") == true)
            stopDaemon();
         else if (arguments[1].equals("-debugOn") == true)
            modifyDebug(true);
         else if (arguments[1].equals("-debugOff") == true)
            modifyDebug(false);
         else if (arguments[1].equals("-metricsOn") == true)
            modifyMetrics(true);
         else if (arguments[1].equals("-metricsOff") == true)
            modifyMetrics(false);
         else
            System.out.println("Usage: FTPd -stop|-debugOn|-debugOff|-metricsOn|-metricsOff");
      }
      catch (Exception exception) {
      }
   }
   
   /**
    * Starts the FTP daemon
    */
   public void run()
   {
      ServerSocket serverSocket;
      Socket connection = null;
      boolean timeoutOccurred;
      ActiveRequests activeRequests;      
      
      try {
         
         // Create the data structure that will keep track of the number
         //  of requests to this FTP daemon that are being serviced.
         activeRequests = new ActiveRequests();
         
         // Create a listener for connections to this FTP daemon
         serverSocket = new ServerSocket(FTPLib.DAEMON_PORT, 
                                         Constants.SERVER_BACKLOG_QUEUE_SIZE);
                         
         while (killFlag.isSet() == false) {
            
            // Wait for a certain amount of time and then check to see if
            //  a shutdown request has been sent
            timeoutOccurred = false;
            try {
               serverSocket.setSoTimeout(5 * 1000);  // In milliseconds
               connection = serverSocket.accept();
            }
            catch (InterruptedIOException interrupted) { // Timeout occurred
               timeoutOccurred = true;
            }
         
            // If execution gets here and timeout didn't occur, that means
            //  a valid request has been received.
            if (timeoutOccurred == false) {     
                            
               // Create a handler thread to handle the received request
               activeRequests.increment();
               new RequestHandler(connection, activeRequests); 
            }
         }                  
         
         // Close the listener so no new connections can be made to this daemon
         serverSocket.close();
         
         // Wait for the sepecified period of seconds (at most) to give 
         //  outstanding requests an opportunity to finish. 
         activeRequests.waitForCompletion(30);
         
         Log.info("FTPd", "run", "Shutdown");
            
         // If there are any more outstanding requests at this point, they will 
         //  be stopped automatticaly when this method exits since they 
         //  were started as "daemon" threads.
      }
      catch (Exception exception) {
         Log.excp("FTPd", "run", exception.toString());
      }
   }
   
   /**
    * Sends a "stop" message to the FTP daemon on the local host indicating
    *  that it should shutdown
    */
   public static void stopDaemon()
   {
      FTPPacket packet;      
      byte rawPacket[];
      
      try {
         
         // Create a packet that will indicate that the daemon should shutdown
         packet = new FTPPacket();
         packet.cmd = FTPd.STOP_DAEMON;
         packet.len = 0;         
         
         if ((rawPacket = packet.getRawHeaderData()) != null)                                    
            sendStateUpdate(rawPacket);         
      }
      catch (Exception exception) {   
      }
   }
   
   /**
    * Sends a "debugOn" or "debugOff" message to the FTP daemon on the local 
    *  host indicating that it should modify its debug state
    */
   public static void modifyDebug(boolean turnDebugOn)
   {
      FTPPacket packet;      
      byte rawPacket[];
      
      try {
         
         // Create a packet that will be used modify the local FTP daemon's
         //  debug state
         packet = new FTPPacket();
         if (turnDebugOn == true)
            packet.cmd = FTPd.TURN_DEBUG_ON;
         else
            packet.cmd = FTPd.TURN_DEBUG_OFF;
         packet.len = 0;         
         
         if ((rawPacket = packet.getRawHeaderData()) != null)                                    
            sendStateUpdate(rawPacket);         
      }
      catch (Exception exception) {   
      }
   }
   
   /**
    * Sends a "metricsOn" or "metricsOff" message to the FTP daemon on the local 
    *  host indicating that it should modify its metric logging state
    */
   public static void modifyMetrics(boolean turnMetricsOn)
   {
      FTPPacket packet;      
      byte rawPacket[];
      
      try {
         
         // Create a packet that will be used modify the local FTP daemon's
         //  metric logging state state
         packet = new FTPPacket();
         if (turnMetricsOn == true)
            packet.cmd = FTPd.TURN_METRICS_ON;
         else
            packet.cmd = FTPd.TURN_METRICS_OFF;
         packet.len = 0;         
         
         if ((rawPacket = packet.getRawHeaderData()) != null)                                    
            sendStateUpdate(rawPacket);         
      }
      catch (Exception exception) {   
      }
   }
   
   /**
    * Sends the header portion of the given raw FTP packet to the FTP daemon
    *  running on the local host.
    */
   private static void sendStateUpdate(byte rawPacket[]) 
   {
      Socket socket;
      BufferedOutputStream outputStream;
      
      try {
         // Create a connection to the local FTP daemon
         socket = new Socket("localhost", FTPLib.DAEMON_PORT);
         outputStream = new BufferedOutputStream(socket.getOutputStream());                  
         
         // Send the packet        
         outputStream.write(rawPacket, 0, FTPPacket.RAW_HEADER_SIZE);
         outputStream.flush();
         
         // Close the system resources
         outputStream.close();
         socket.close();         
      }
      catch (Exception exception) {   
         System.out.println(exception.toString());
      }
   }
}

/**
 * A simple thread-safe flag
 */
class StateFlag
{
   private boolean flag;

   public StateFlag(boolean initialValue)
   {
      flag = initialValue;
   }
   
   public synchronized void on()
   {
      flag = true;
   }
   
   public synchronized void off()
   {
      flag = false;
   }
   
   public synchronized boolean isOn()
   {
      return flag;
   }
}

/**
 * A data structure that keeps track of the number of requests being processed
 *  by the FTP daemon
 */
class ActiveRequests
{    
   private int activeCount;
   
   /**
    * Constructor
    */
   public ActiveRequests()
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
            
            Log.info("ActiveRequests", "waitForCompletion", "Waiting for <" + activeCount 
                     + "> request(s) to finish ...");
            
            // "Sleep" for the given period of time (in milliseconds)
            wait((long)timeout * 1000);  
            
            // Check to see if all the requests have been processed
            if (activeCount > 0) 
               Log.warn("ActiveRequests", "waitForCompletion", "<" + activeCount + 
                        "> request(s) still processing");                           
         } 
                  
      }
      catch (Exception exception) {
         Log.excp("ActiveRequests", "waitForCompletion", exception.toString());                           
      }
   }      
}

/**
 * A thread that handles a single FTP request sent to this daemon. A request
 *  (i.e. a "get" or "put" request) is made up of an exchange of packets
 *  between the FTP client and this FTP thread.
 */
class RequestHandler extends Thread
{
   private Socket connection;
   private ActiveRequests activeRequests;
   private FTPPacket packet;
   private BufferedInputStream inputStream;
   private BufferedOutputStream outputStream;
   private int metric_bytesIn  = 0;   
   private int metric_bytesOut = 0;   
   private long metric_requestStartTime;
   private boolean fileOpened;
   private FileInputStream fileReader;
   private FileOutputStream fileWriter;             
   
   /**
    * Constructor
    */
   public RequestHandler(Socket socket, ActiveRequests activeRequests) 
   {
      try {
         
         connection = socket;
         this.activeRequests = activeRequests;  
         fileReader = null;
         fileWriter = null;
         
         // Create the streams to read and write from/to the socket
         inputStream = new BufferedInputStream(connection.getInputStream());
         outputStream = new BufferedOutputStream(connection.getOutputStream());
      
         // Create a packet object into which packets will read into from the
         //  socket, and read from to send to the socket.
         packet = new FTPPacket();
      
         // Make this a "daemon" thread so that when the FTP daemon is stopped, 
         //  it won't wait for any of these worker threads to finish if they are 
         //  still processing.
         setDaemon(true);      
      
         start();
      }
      catch (Exception exception) {
         Log.excp("RequestHandler", "RequestHandler", exception.toString());
         this.activeRequests.decrement();
      }
   }
   
   /**
    * Processes the request that was sent to the FTP daemon
    */
   public void run()
   {      
      boolean cont;
      int bytesRead;
      
      try {
         
         // Read packets off the socket and process them until the request
         //  has been satisfied.
         do {
            
            // Clear the packet that data will be read into
            packet.clear();                        
            
            // Read the next packet off the socket  
            bytesRead = FTPLib.readPacket(packet, connection, inputStream);
            if ((bytesRead != FTPLib.ERROR_CODE) &&
                (bytesRead != 0)) {            
               
               // Update the metrics               
               if (FTPd.metricsFlag.isOn() == true) 
                  metric_bytesIn += bytesRead;                                                           
               
               // Process the data in the packet
               cont = processPacket();
            }   
            else
               cont = false;
            
         } while (cont == true);               
         
         // Close the system resources
         inputStream.close();
         outputStream.close();
         connection.close();
      }
      catch (Exception exception) {
         Log.excp("RequestHandler", "run", exception.toString());
      }
            
      // Decrement the count of active FTP requests
      activeRequests.decrement();
   }
   
   /**
    * Performs the requested action that is stored in the global packet object. 
    *  False is returned if an error occurs, or when the FTP request has been 
    *  satisfied.
    */
   private boolean processPacket()
   {      
      final String fnct_name = "processPacket";
      File fileDescp;
      int numBytesReadFromFile;
      boolean append = false;      
      String translatedFilename;
      
      try  {                  
         
         if (FTPd.debugFlag.isOn() == true) {    
            Log.info("RequestHandler", fnct_name, "HEADER BUFFER IN (CHARS) :");
            Log.info("RequestHandler", fnct_name, "   cmd : " + packet.cmd);
            Log.info("RequestHandler", fnct_name, "   fn  : " + packet.file_name);
            Log.info("RequestHandler", fnct_name, "   gid : " + packet.gid);
            Log.info("RequestHandler", fnct_name, "   uid : " + packet.uid);
            Log.info("RequestHandler", fnct_name, "   fim : " + packet.file_mode);
            Log.info("RequestHandler", fnct_name, "   hn  : " + packet.hostname);
            Log.info("RequestHandler", fnct_name, "   len : " + packet.len);
         }                  
         
         // Determine the requested action
         switch (packet.cmd) {
            
            // Opens the given file locally for writing to it
            case FTPLib.OPEN_NC_APPEND:
               append = true;
               
            case FTPLib.OPEN_NC:
               
               translatedFilename = translateFilename(packet.file_name);
               
               if (FTPd.debugFlag.isOn() == true ) {
                  Log.info("RequestHandler", fnct_name, "Opening file " + 
                           translatedFilename);
                  Log.info("RequestHandler", fnct_name, "   CMD = " + 
                           packet.cmd);
                  Log.info("RequestHandler", fnct_name, "   UID = " + 
                           packet.uid);
                  Log.info("RequestHandler", fnct_name, "   GID = " + 
                           packet.gid);
                  Log.info("RequestHandler", fnct_name, "   LEN = " + 
                           packet.len);
                  Log.info("RequestHandler", fnct_name, "   FIM = " + 
                           packet.file_mode);
                  Log.info("RequestHandler", fnct_name, "   Hostname = " + 
                           packet.hostname);
                  Log.info("RequestHandler", fnct_name, "   Filename = " +
                           packet.file_name);
               }
               
               // See if the given file already exists
               fileDescp = new File(translatedFilename);
               if (fileDescp.exists() == true) {
                  
                  // See if the given file is indeed a file
                  if (fileDescp.isFile() == true) 
                     packet.len = 0;                  
                  else {
                  
                     // Send an error response back to the client
                     sendResponse(FTPLib.NOK_PACK, -1);                     
                     return false;
                  }
               }               
               
               // Try to open the given file for writing to it
               try {
                  fileWriter = new FileOutputStream(translatedFilename, append);
               }
               catch (Exception fileOpenError) {
                  
                  // Send an error response back to the client
                  sendResponse(FTPLib.NOK_PACK, -1);
                  return false;
               }
               
               // Send response back to the client
               sendResponse(FTPLib.OK_PACK, 0);
               fileOpened = true;
               
               // Update metrics
               if (FTPd.metricsFlag.isOn() == true) 
                  metric_requestStartTime = System.currentTimeMillis();                                                                

               break;                           
               
            // Opens the given file locally for reading from it
            case FTPLib.OPEN_READ_NC:
               
               translatedFilename = translateFilename(packet.file_name);
               
               // Check to make sure the given file exists
               fileDescp = new File(translatedFilename);               
               if (fileDescp.isFile() == true) {

                  // Get the user/group ownership of the file
                  //packet.gid = getgid();
                  //packet.uid = getuid();
                  // For now just keep the uid/gid that was given, that way
                  //  the file is written with the uid/gid that initiated the
                  //  read...
                  
                  packet.len = 0;
                  
                  // Get the file mode of the desired file
                  packet.file_mode = 0;
                  packet.file_mode |= (fileDescp.canRead() ? 256 : 0);                  
                  packet.file_mode |= (fileDescp.canWrite() ? 128 : 0);                  
                  
                  if (FTPd.debugFlag.isOn() == true) {           
                     Log.info("RequestHandler", fnct_name, 
                              "READ_NC:Opening file " + translatedFilename);
                     Log.info("RequestHandler", fnct_name, "   CMD = " + 
                              packet.cmd);
                     Log.info("RequestHandler", fnct_name, "   UID = " + 
                              packet.uid);
                     Log.info("RequestHandler", fnct_name, "   GID = " + 
                              packet.gid);
                     Log.info("RequestHandler", fnct_name, "   LEN = " + 
                              packet.len);
                     Log.info("RequestHandler", fnct_name, "   FIM = " + 
                              packet.file_mode);
                     Log.info("RequestHandler", fnct_name, "   Hostname = " +
                              packet.hostname);                      
                  }
                  
                  // Try to open the file for reading
                  try {          
                     fileReader = new FileInputStream(translatedFilename);
                  }
                  catch (Exception fileOpenError) {
                     
                     // Send an error response to the client
                     sendResponse(FTPLib.NOK_PACK, -1);
                     fileOpened = false;
                     return false;
                  }
                  
                  // Send a good response back to the client
                  sendResponse(FTPLib.OK_PACK, 0);
                  
                  // Set the flag to indicate the file is opened for reading
                  fileOpened = true;
                  
                  // Update metrics
                  if (FTPd.metricsFlag.isOn() == true) 
                     metric_requestStartTime = System.currentTimeMillis();                                                         
                  
               }
               else {  // File doesn't exist

                  // Send an error response back to the client
                  sendResponse(FTPLib.NOK_PACK, -1);
                  fileOpened = false;
                  return false;
               }
               
               break;
               
            // Writes the given block of data to the local file
            case FTPLib.WRITE_NC:
               
               if (FTPd.debugFlag.isOn() == true)
                  Log.info("RequestHandler", fnct_name, "State WRITE_NC:");
                
               // Make sure the file is opened
               if (fileOpened == false) {
                   
                  if (FTPd.debugFlag.isOn() == true)
                     Log.info("RequestHandler", fnct_name, 
                              "Error, cannot write to an un-opened file");
                   
                  // Send an error response back to the client
                  sendResponse(FTPLib.NOK_PACK, -1);
                  
                  return false;
               }
               else {
                   
                  // Try to write the data to the file
                  try {
                     fileWriter.write(packet.buf, 0, packet.len);                      
                  }
                  catch (Exception fileWriteError) {
                      
                     // Send an error response back to the client
                     sendResponse(FTPLib.NOK_PACK, 0);
                     return false;
                  }
                   
                  // Send a good response back to the client
                  sendResponse(FTPLib.OK_PACK, 0);
               }
                
               break;
               
            // Reads a block of data from the local file and sends it to the 
            //  client
            case FTPLib.READ_NC:
               
               // Check to see if the file has been opened
               if (fileOpened == false) {      
                  
                  if (FTPd.debugFlag.isOn() == true)
                     Log.info("RequestHandler", fnct_name, 
                              "Error, cannot read an unopened file.");
                              
                  // Send a response back to the client
                  sendResponse(FTPLib.NOK_PACK, 0);
                  return false;
               }
               else {
                         
                  // Read a block of data from the file
                  numBytesReadFromFile = fileReader.read(packet.buf, 0, 
                                                         FTPPacket.MAX_DATA_SIZE);
                  
                  // Check to see if the end of the file has been reached
                  if (numBytesReadFromFile < 0) {                                          

                     // Send response back to the client indicating EOF reached
                     sendResponse(FTPLib.EOF_PACK, packet.len);

                     dumpMetrics();
                     
                     // Set the flag to indicate the file is closed, and
                     //  close the file
                     fileOpened = false;
                     if (fileReader != null) 
                        fileReader.close();  
              
                     return false;  // Finished the "get" request
                  }
                  else {

                     // Send a good response back to the client
                     sendResponse(FTPLib.OK_PACK, numBytesReadFromFile);
                  }
               }                    

               break;
               
            // Closes the local file. Writing to it is finished.
            case FTPLib.CLOSE_NC:
               
               if (FTPd.debugFlag.isOn() == true)
                  Log.info("RequestHandler", fnct_name, "Closing Files");
                                             
               // Send a good response back to the client
               sendResponse(FTPLib.OK_PACK, 0);
               
               dumpMetrics();

               // Close the file
               if (fileWriter != null) {        
                  fileWriter.close();
                  
                  // Set the owner/group permissions
                  //chown(file_name, hdblock.dbuf.uid, hdblock.dbuf.gid);
                  //chmod(file_name, file_mode);
               }
               fileOpened = false;
        
               return false;  // Finished "put" request                 
               
            // This is sent from the client if they have an error writing
            //  out the file
            case FTPLib.CLOSE_READ_NC:
               
               // Close the file
               if (fileReader != null) 
                  fileReader.close();                         
               fileOpened = false;
      
               return false;  // "Get" request aborted by the client
               
            // Special daemon shutdown case
            case FTPd.STOP_DAEMON:
               Log.info("RequestHandler", "processPacket", 
                        "Received shutdown request");                           
               FTPd.killFlag.set();
               return false;
               
            // Special case for turning debug on
            case FTPd.TURN_DEBUG_ON: 
               FTPd.debugFlag.on();
               return false;
            
               // Special case for turning debug off
            case FTPd.TURN_DEBUG_OFF:
               FTPd.debugFlag.off();
               return false;
               
            // Special case for turning metrics on
            case FTPd.TURN_METRICS_ON:
               FTPd.metricsFlag.on();
               return false;
               
            // Special case for turning metrics off
            case FTPd.TURN_METRICS_OFF:
               FTPd.metricsFlag.off();
               return false;
               
            default:
               Log.error("RequestHandler", fnct_name, "Invalid command <" + 
                         packet.cmd + ">");
               return false;
         }         
         
         return true;
      }
      catch (Exception exception) {
         Log.excp("RequestHandler", fnct_name, exception.toString());
         return false;
      }
   }   

   /**
    * Logs metrics obtained during this FTP request
    */
   private void dumpMetrics()
   {
      long requestEndTime;
      
      try {
         
         if (FTPd.metricsFlag.isOn() == true) {
            requestEndTime = System.currentTimeMillis();
            
            Log.info("RequestHandler", "dumpMetrics" ,"NC_SERVER METRICS ==>  Delta => Secs: " +
              (requestEndTime - metric_requestStartTime)/1000 + ", milliSecs: " + 
              (requestEndTime - metric_requestStartTime)%1000 + ".  Data => In: " + 
               metric_bytesIn + "  Out: " + metric_bytesOut);
         }
      }
      catch (Exception exception) {
      }
   }
   
   /**
    * Updates the global packet with the given cmd and len fields 
    *  and then sends it out
    */
   private boolean sendResponse(int cmd, int len)                              
   {            
      int bytesWritten;
         
      packet.cmd = cmd;
      packet.len = len;

      if ((bytesWritten = FTPLib.writePacket(packet, 
                                         outputStream)) != FTPLib.ERROR_CODE) {         
         
         // Update metrics
         if (FTPd.metricsFlag.isOn() == true)
            metric_bytesOut += bytesWritten;
         
         return true;
      }
      else
         return false;
   }
   
   /**
    * If a "unix style" NTCSS root path (i.e. /h/NTCSSS) is specified at the 
    *  beginning of the given string, it is remove and replaced with the NTCSS 
    *  root that is specified for this host.         
    *
    * e.g. If a FTP client requested from this daemon a file named
    *      "/h/NTCSSS/database/current_users" and this daemon happend to be
    *      running on a non-Unix platform, the path wouldn't make sense. So
    *      the NTCSS root part of the path is replaced with the NTCSS root
    *      for this system so it will make sense on this system (which in
    *      this example would be a non-Unix style path, 
    *      like e:/Program Files/NTCSSS).
    *
    * If the "unix style" tmp directory path (i.e. /tmp) is specified at the
    *  beginnnig of the given string, and this is running on the Windows
    *  platform, it is removed and replaced with the tmp directory under
    *  the NTCSS installed directory.
    *
    * The translated string is returned. If the none of the special paths 
    *  occur at the beginning, the given string is returned.
    */
   private String translateFilename(String originalFilename)
   {
      final String unixNtcssRoot = "/h/NTCSSS/";
      final String unixTmpDir = "/tmp/";
      File dir;
      
      try {
         if (originalFilename.startsWith(unixNtcssRoot) == true) 
            return Constants.NTCSS_ROOT_DIR + "/" + 
                   originalFilename.substring(unixNtcssRoot.length()); 
         else if ((originalFilename.startsWith(unixTmpDir) == true) &&
                  (Constants.IS_WINDOWS_OS == true)) {
            
            // If the tmp directory doesn't exist, create it
            dir = new File(Constants.NTCSS_TMP_DIR);
            if (dir.exists() == false)
               dir.mkdirs();
                        
            return Constants.NTCSS_TMP_DIR + "/" + 
                   originalFilename.substring(unixTmpDir.length());
         }
         else
            return originalFilename;
      }
      catch (Exception exception) {
         return null;
      }
   }
}