/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.libs;

import java.net.Socket;
import java.net.DatagramSocket;
import java.net.DatagramPacket;
import java.net.InetAddress;

import java.io.BufferedOutputStream;
import java.io.InputStream;
import java.io.InterruptedIOException;
import java.io.IOException;
import java.io.BufferedInputStream;

import ntcsss.log.Log;

/**
 * A library of socket supporting methods
 */
public class SocketLib
{   
   /**
    * Sends the given message to the given host and port as a TCP message. 
    *  This method doesn't wait for a response. If an error occurs, false is 
    *  returned. Otherwise, true is returned.
    */
   public static boolean sendFireForgetMessage(String host, int port, 
                                               String message)
   {
      Socket socket;
      BufferedOutputStream socketStream;
      
      try {
         // Create a socket to the given host and port
         socket = new Socket(host, port);
         socketStream = new BufferedOutputStream(socket.getOutputStream());
                                 
         // Send the given message
         socketStream.write(message.getBytes(), 0, message.length());
         
         // Close the socket
         socketStream.close();
         socket.close();
         
         return true;
      }          
      catch (Exception exception) {
         Log.excp("SocketLib", "sendFireForgetMessage", "Error sending to <" + 
                  host + "> " + exception.toString());
         return false;
      }
   }           
   
   /**
    * Reads data off of the given socket stream storing the result in the given 
    *  buffer. The total number of bytes read is returned or -1 if an error 
    *  occurred. If a read timeout occurs, -2 is returned.
    */
   public static int readStream(InputStream stream, byte  buffer[],  
                                int maxBytesToRead)
   {
      String fnct_name = "readStream";            
      int bytesRead      = 0;
      int totalBytesRead = 0;
      int bytesLeftToRead;            
      
      // Check the given parameters
      if ((stream == null) || (buffer == null) || (maxBytesToRead < 0) || 
          (maxBytesToRead > buffer.length)) {
         Log.error("SocketLib", fnct_name, "Invalid parameter");
         return -1;
      }
       
      while (totalBytesRead < maxBytesToRead) {
    
         bytesLeftToRead = (maxBytesToRead - totalBytesRead);

         // Read the data
         try {
            bytesRead = stream.read(buffer, totalBytesRead, bytesLeftToRead);
         }
         catch (InterruptedIOException interrupted) {
            Log.debug("SocketLib", fnct_name, "Received timeout on read.");
            return -2;
         }
         catch (Exception exception) {
            Log.debug("SocketLib", fnct_name, exception.toString());
            return -1;
         }

         // Check to see if the end of the stream has been reached
         if (bytesRead == -1)         
            break;         
         else {
            
            totalBytesRead += bytesRead;            
            
            // If read more than the header and the last character read was a 
            //  null, then break
            if ((totalBytesRead > Constants.CMD_LEN) &&
                (buffer[totalBytesRead - 1] == '\0'))
               break;                            
         }         
      }
      
      return totalBytesRead;
   }
   
   /**
    * Sends the given message as a UDP message to the given host and port.
    *  This method doesn't wait for a response. If an error occurs, false is 
    *  returned. Otherwise, true is returned.
    */
   public static boolean sendFireAndForgetDatagram(String host, int port, 
                                                   String message)                 
   {      
      DatagramSocket socket;
      DatagramPacket packet;      
      byte msg[];            
      
      try {
         // Create a UDP socket
         socket = new DatagramSocket();         
                                                
         // Build the message         
         msg = message.getBytes();
         packet = new DatagramPacket(msg, msg.length, 
                                     InetAddress.getByName(host), port);                                               
         
         // Send the message
         socket.send(packet);                                    
                        
         // Close the socket
         socket.close();

         return true;
      }   
      catch (Exception exception) {
         Log.excp("SocketLib", "sendFireAndForgetDatagram", "Error sending to <" 
                  + host + ">" + exception.toString());
         return false;
      }
   }
   
       ////////////////////////////////////////////////////////////////////////////////
    //                        SendServerMessage
    //                        If you just want response pass null in last
    //                        parameter... if you want to read more put #
    //                        bytes in data String & they will come back in that string
    
    public static String SendServerMessage(String strMessage, String strHost, 
                                    int nPort, String data)
    {
        Socket socket;
        int bytesRead=0;
        
        try {
            
            // Create a socket to the given host and port
            socket = new Socket(strHost, nPort);
            
            BufferedOutputStream outputStream = new BufferedOutputStream
            (socket.getOutputStream());
            
            BufferedInputStream inputStream = new BufferedInputStream
            (socket.getInputStream());
            
            // Send the given message
            outputStream.write(strMessage.getBytes(), 0, strMessage.length());
            outputStream.write(0); // Send null byte
            outputStream.flush();
            
            // Create the buffer to read into
            byte buffer[];
            buffer = new byte[Constants.CMD_LEN];
            
            // Set the read timeout
            socket.setSoTimeout(Constants.SOCKET_READ_TIMEOUT);
            
            // Read the response
            bytesRead = readStream(inputStream, buffer,
            Constants.CMD_LEN);
            
            String response=new String(buffer, 0, bytesRead);
            
            //now if there is a value in data read that much more data
            if(data!=null) {
                byte dataBuffer[];
                dataBuffer = new byte[new Integer(data).intValue()];
                bytesRead = readStream(inputStream, dataBuffer,
                new Integer(data).intValue());
                data=new String(dataBuffer, 0, bytesRead).toString();
            }
            
            // Close the socket
            outputStream.close();
            inputStream.close();
            socket.close();
            
            return response;
            
        }
        
        catch (Exception exception){
            Log.excp("SocketLib", "SendServerMessage",
            exception.toString());
            return null;
        }
    }
} 