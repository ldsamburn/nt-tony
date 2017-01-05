/*
      Copyright(c)2002 Northrop Grumman Corporation
 
                All Rights Reserved
 
  This material may be reproduced by or for the U.S. Government pursuant to
  the copyright license under the clause at Defense Federal Acquisition
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
 */

package ntcsss.libs;

import java.net.Socket;

import java.io.BufferedOutputStream;
import java.io.BufferedInputStream;

import ntcsss.log.Log;

import ntcsss.libs.structs.SvrStatus;

/**
 * Contains methods for obtaining information about a host's NTCSS daemons
 */
public class SvrStatLib
{
   /**
    * Returns 1 if the NTCSS server on the given host is up regardless of
    *  its state (i.e. awake or idle). Returns 0 if the server isn't up.
    *  Returns -1 if there was an error in the process.
    */
  public static int isServerRunning(String host, int port)
  {
    SvrStatus svrStatus;
    
    try
    {
      if ((svrStatus = getServerStatus(host, port)) == null)
        return -1;
      return (svrStatus.isServerRunning() ? 1 : 0);
    }
    catch (Exception exception)
    {
      Log.excp("SvrStatLib", "isServerRunning", exception.toString());
      return -1;
    }
  }
  
   /**
    * Returns 1 if debug is on, on the given host; 0 if debug is not on;
    *  -1 if there is an error in the process.
    */
  public static int isDebugOn(String host, int port)
  {
    SvrStatus svrStatus;
    
    try
    {
      if ((svrStatus = getServerStatus(host, port)) == null)
        return -1;
      
      return (svrStatus.isDebugOn() ? 1 : 0);
    }
    catch (Exception exception)
    {
      Log.excp("SvrStatLib", "isDebugOn", exception.toString());
      return -1;
    }
  }
  
   /**
    * Returns 1 if the server is awake, 0 if it is not, or
    *  -1 if there is an error in the process.
    */
  public static int isServerAwake(String host, int port)
  {
    SvrStatus svrStatus;
    
    try
    {
      if ((svrStatus = getServerStatus(host, port)) == null)
        return -1;
      
      return (svrStatus.isServerAwake() ? 1 : 0);
    }
    catch (Exception exception)
    {
      Log.excp("SvrStatLib", "isServerAwake", exception.toString());
      return -1;
    }
  }
  
   /**
    * Retrieves status information about the given NTCSS server on the given host
    */
   public static SvrStatus getServerStatus(String host, int port)
   {
      return getServerStatus(host, port, 0);
   }
      
   public static SvrStatus getServerStatus(String host, int port, 
                                           int timeout /* in seconds */)
   {
      Socket socket;
      BufferedOutputStream outputStream;
      BufferedInputStream inputStream;
      byte buffer[];
      int bytesRead;
      final int bufferSize = 100;
      String response;
      String message;   
      Object status[];
      SvrStatus svrStatus;      
      
      try {
         if (host == null)
           return null;
                  
         // Create a socket
         socket = new Socket(host, port);
         outputStream = new BufferedOutputStream(socket.getOutputStream());
         inputStream = new BufferedInputStream(socket.getInputStream());

         // Send the message
         message = StringLib.formatMessage("SERVERSTATUS", 
                            String.valueOf(Constants.SERVER_STATE_CHECK) + "\0");
         outputStream.write(message.getBytes(), 0, message.length());
         outputStream.flush();

         // Read response
         buffer = new byte[bufferSize];
         response = new String();
         
         socket.setSoTimeout(timeout * 1000);         
         do {
            bytesRead = SocketLib.readStream(inputStream, buffer, bufferSize);            
            if (bytesRead > 0) 
               response += new String(buffer, 0, bytesRead);                           
         }
         while (bytesRead > 0);         

         inputStream.close();
         outputStream.close();
         socket.close();
         
         if (bytesRead == -2) // i.e. the read timed out
            return null;
         
         // Parse the response
         if ((response.length() <= Constants.CMD_LEN) ||
             (response.substring(0, Constants.CMD_LEN).trim().equals("GOODSERVERSTATUS") == false))
            return null;
         
         status = new Object[3];
         if (BundleLib.unbundleData(response.substring(Constants.CMD_LEN).trim(), 
                                    "III", status) == false)
            return null;
         
         return new SvrStatus(((Integer)status[0]).intValue(),
                              ((Integer)status[1]).intValue(),
                              ((Integer)status[2]).intValue());                                                   
      }
      catch (Exception exception) {
         Log.excp("SvrStatLib", "getServerStatus", "Error getting status from <" 
                  + host + "> " + exception.toString());
         return null;
      }      
   }
   

   /**
    * Sends the "awaken" message to each NTCSS daemon on the given host.
    *  The number of failures is returned, or -1 if there is an error.
    */
  public static int sendAwakenMessage(String host)
  {
    return sendMessageToDaemons(host, StringLib.formatMessage("SERVERSTATUS",
    String.valueOf(Constants.SERVER_STATE_AWAKE)));
  }
  
   /**
    * Sends the "idle" message to each NTCSS daemon on the given host.
    *  The number of failures is returned, or -1 if there is an error.
    */
  public static int sendIdleMessage(String host)
  {
    return sendMessageToDaemons(host, StringLib.formatMessage("SERVERSTATUS",
    String.valueOf(Constants.SERVER_STATE_IDLE)));
  }
  
   /**
    * Sends the given message to each NTCSS daemon on the given host.
    *  The number of failures is returned, or -1 if there is an error.
    */
  private static int sendMessageToDaemons(String host, String messageToSend)
  {
    int failures = 0;
    
    try
    {
      
      // Send the given message to each daemon port on the specified host
      if (SocketLib.sendFireForgetMessage(host,
      Constants.DB_SVR_PORT, messageToSend) == false)
        failures++;
      if (SocketLib.sendFireForgetMessage(host,
      Constants.DSK_SVR_PORT, messageToSend) == false)
        failures++;
      if (SocketLib.sendFireForgetMessage(host,
      Constants.MSG_SVR_PORT, messageToSend) == false)
        failures++;
      if (SocketLib.sendFireForgetMessage(host,
      Constants.PRTQ_SVR_PORT, messageToSend) == false)
        failures++;
      if (SocketLib.sendFireForgetMessage(host,
      Constants.SPQ_SVR_PORT, messageToSend) == false)
        failures++;
      if (SocketLib.sendFireForgetMessage(host,
      Constants.CMD_SVR_PORT, messageToSend) == false)
        failures++;
      
      return failures;
    }
    catch (Exception exception)
    {
      return -1;
    }
  }
  
}

