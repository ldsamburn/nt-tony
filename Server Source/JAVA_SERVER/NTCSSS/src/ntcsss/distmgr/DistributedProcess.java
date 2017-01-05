/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.distmgr;

import java.net.Socket;
import java.net.UnknownHostException;
import java.net.ConnectException;

import java.io.BufferedOutputStream;
import java.io.BufferedInputStream;
import java.io.InterruptedIOException;
import java.io.IOException;

import COMMON.TimedSocket;

import ntcsss.log.Log;

import ntcsss.libs.Constants;
import ntcsss.libs.SocketLib;
import ntcsss.libs.SvrStatLib;

/**
 * Sends the given data to the desired host and stores the results in the
 *  given DPResults object
 */
public class DistributedProcess extends Thread 
{
   private String message;
   private String host;
   private int port;
   private DPResults results;
   private FinishedDPCount finishedCount;
   private ActiveDPCount activeCount;
   private boolean sendAndForget;
   private int dpTimeoutValue;
   
   private final int DPM_BUFF_SIZE = 2048;
   
   private static ProblemHostList problemHostList = new ProblemHostList();
   
    /** 
     * Constructor 
     *
     * timeout value in seconds
     */
    public DistributedProcess(String message, String host, int port, 
                              DPResults results, FinishedDPCount finishedCount,
                              ActiveDPCount activeCount, boolean sendAndForget,
                              int dpTimeoutValue) 
    {
       this.message = message;
       this.host = host;
       this.port = port;
       this.results = results;
       this.finishedCount = finishedCount;
       this.activeCount = activeCount;
       this.sendAndForget = sendAndForget;
       this.dpTimeoutValue = dpTimeoutValue;
       
       // Make a "daemon" thread so that if the DM stops because
       //  it has waited too long, it doesn't have to continue to wait for all 
       //  the DPs to finish before it returns
       setDaemon(true);
       
       // Start this distributed process
       start();
    }
    
    /**
     * Sends the message to the host/port and waits for a response. The response
     *  is stored in the DPResults object
     */
    public void run()
    {
       Socket socket = null;
       BufferedInputStream inputStream = null;    
       BufferedOutputStream outputStream = null;
       byte buffer[];
       int bytesRead;
       
       try {
          
          // If consecutive attempts to read the response from this host have
          //  failed, try to just send the small "server status" message
          //  instead
          if (problemHostList.isProblemHost(host) == true) {
             
             if (SvrStatLib.getServerStatus(host, port, 5) == null)
                Log.warn("DistributedProcess", "run", "Host <" + host + 
                         "> still not responding!");
             else {
                Log.info("DistributedProcess", "run", "Host <" + host + 
                         "> Cleared!");
                problemHostList.removeHost(host);
             }
             
             throw new GoTo();
          }
               
          // Create a connection to the given host/port
          socket = TimedSocket.getSocket(host, port, 
                                         Constants.DP_CONNECTION_TIMEOUT * 1000);
          outputStream = new BufferedOutputStream(socket.getOutputStream());             
          inputStream = new BufferedInputStream(socket.getInputStream());
          
          // Send the given message
          outputStream.write(message.getBytes(), 0, message.length());
          outputStream.write(0); // Send null byte
          outputStream.flush();
          
          // If this isn't a "fire-and-forget" message, get the response
          if (sendAndForget == false) {
             
             // Create the buffer to read into
             buffer = new byte[DPM_BUFF_SIZE];
             
             // Set the read timeout
             socket.setSoTimeout(dpTimeoutValue * 1000);
             
             // Read the response                                  
             bytesRead = SocketLib.readStream(inputStream, buffer, 
                                              DPM_BUFF_SIZE);                          
             
             // Store the result in the DPResults buffer
             if (bytesRead > 0) {
                results.add(host, new String(buffer, 0, bytesRead));
                
                // Remove the host from the list of "problem" hosts
                problemHostList.removeHost(host);
             }
             else if (bytesRead == -2) {
                Log.error("DistributedProcess", "run", 
                          "Read timeout occurred for host <" + host + ">");
                
                // Increment the count associated with this host
                problemHostList.incrementIncident(host);
             }
             else
                Log.error("DistributedProcess", "run", 
                          "End of stream reached for host <" + host + ">");
          }
          
          // Close the streams
          outputStream.close();
          inputStream.close();
          socket.close();                         
       }
       catch (UnknownHostException unknownHost) {  
          Log.error("DistributedProcess", "run", "Unknown host <" + host + ">");
          problemHostList.removeHost(host);
       }
       catch (ConnectException connectException) {  
          Log.error("DistributedProcess", "run", "Connection failed to host <" + 
                    host + "> :" + connectException.getMessage());
          problemHostList.removeHost(host);
       }
       catch (InterruptedIOException interrupted) {  // Connection timeout occurred
                                                     
          // Try to close the streams
          try {
             if (outputStream != null)
                outputStream.close();
             
             if (inputStream != null)
                inputStream.close();
          
             if (socket != null)
                socket.close();
          }
          catch (IOException ioException) {
             Log.error("DistributedProcess", "run", 
                       "Error closing streams to host <" + host + ">");                      
          }
          
          // No result is returned from this distributed process
          
          Log.warn("DistributedProcess", "run", "Received timeout for <" +
                   host + ">");
          problemHostList.removeHost(host);
       }
       catch (GoTo goTo) {
          // This is not an exception
       }
       catch (Exception exception) {
          Log.excp("DistributedProcess", "run", "Exception occured for host <" + 
                   host + "> <" + exception.toString() + ">");
          problemHostList.removeHost(host);
       }
       
       Log.debug("DistributedProcess", "run", 
                 "Finished process for <" + host + ">");
       
       activeCount.decrement();
       finishedCount.increment();
    }
}

class GoTo extends Throwable
{
}


