/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package COMMON;

import java.net.Socket;

import java.io.OutputStream;
import java.io.InputStream;
import java.io.InterruptedIOException;

/**
 * Common methods used during FTP 
 */
public class FTPLib
{
   
   public static final int DAEMON_PORT = 1926;             
      
   // File transfer Primitive codes
   public static final int OPEN_NC        = 0;
   public static final int CLOSE_NC       = 1;
   public static final int OPEN_READ_NC   = 2;
   public static final int WRITE_NC       = 3;
   public static final int READ_NC        = 4;
   public static final int CLOSE_READ_NC  = 5;
   public static final int OPEN_NC_APPEND = 8;
   
   // Response codes
   public static final int OK_PACK  = 0;
   public static final int NOK_PACK = 1;
   public static final int EOF_PACK = 2;
   
   public static final int ERROR_CODE = -1;
   
   private static final int MAX_BYTES_READ = 8192;


   /**
    * Sends the data in the given packet data structure. If an error occurs,
    *  -1 is returned. Otherwise, the number of bytes sent is returned.
    */
   public static int writePacket(FTPPacket packet, OutputStream outputStream)
   {      
      int headerBytesSent = 0;
      int dataBytesSent = 0;
      
      try {                                                  
      
         // Send the header part of the packet
         if (writeRawData(outputStream, packet.getRawHeaderData(), 
                          FTPPacket.RAW_HEADER_SIZE) == false)
            return FTPLib.ERROR_CODE;

         headerBytesSent = FTPPacket.RAW_HEADER_SIZE;                      
         
         // If the header indicates that there is data also, send the data
         if (packet.len > 0) {
            
            // Send the data
            if (writeRawData(outputStream, packet.buf, packet.len) == false)
               return FTPLib.ERROR_CODE;
            
            dataBytesSent = packet.len;                        
         }                                   
                  
         return headerBytesSent + dataBytesSent;
      }
      catch (Exception exception) {
         return FTPLib.ERROR_CODE;        
      }
   }      
   
   /**
    * Reads a FTP packet from the socket into the given packet data structure. 
    *  If an error occurs, -1 is returned. Otherwise, the number of bytes read
    *  is returned.
    */
   public static int readPacket(FTPPacket packet, Socket socket, 
                                InputStream inputStream)
   {      
      int headerBytesRead = 0;
      int dataBytesRead = 0;
      
      try {                  
                  
         // Get the packet's "header" information from the socket
         headerBytesRead = readRawData(socket, inputStream, packet.rawHeaderInfo, 
                                       FTPPacket.RAW_HEADER_SIZE);   
         
         // Check for error
         if (headerBytesRead == FTPLib.ERROR_CODE)
            return FTPLib.ERROR_CODE;
         
         // Fill in the packet "header" fields from the raw data
         packet.parseRawHeaderData(packet.rawHeaderInfo);
         
         // If the header indicates that there is data to follow, get that data
         if (packet.len > 0) 
            dataBytesRead = readRawData(socket, inputStream, packet.buf, 
                                        packet.len);                               
         
         // Check for errors
         if (dataBytesRead == FTPLib.ERROR_CODE)
            return FTPLib.ERROR_CODE;
         
         return headerBytesRead + dataBytesRead;
      }
      catch (Exception exception) {         
         return FTPLib.ERROR_CODE;
      }
   }
   
   /**
    * Writes "numBytesToSend" bytes from "buffer" to the socket. If no
    *  errors occur, true is returned.
    */
   private static boolean writeRawData(OutputStream outputStream, byte buffer[], 
                                       int numBytesToSend)
   {      
      try {   
         
         if (buffer.length < numBytesToSend)
            return false;
         
         outputStream.write(buffer, 0, numBytesToSend);
         outputStream.flush();
         
         return true;
      }
      catch (Exception exception) {
         return false;
      }
   }
   
   /**
    * Reads "totalBytesToRead" bytes from the socket into the given buffer. A 
    *  maximum of MAX_BYTES_READ are read at a time. If any errors
    *  occur, -1 is returned. Otherwise, the total number of bytes read
    *  is returned.
    */ 
   private static int readRawData(Socket socket, InputStream inputStream, 
                                  byte buffer[], int totalBytesToRead) 
   {      
      int numBytesToRead, totalBytesRead;
      int bytesRead = 0;

      try {   
         
         // Make sure the given buffer is big enough
         if (totalBytesToRead > buffer.length)
            return -1;
         
         totalBytesRead = 0;
         
         do {
            
            // Determine how many bytes to read from the stream
            if (totalBytesToRead - totalBytesRead >= MAX_BYTES_READ)
               numBytesToRead = MAX_BYTES_READ;
            else
               numBytesToRead = totalBytesToRead - totalBytesRead;

            // Set the timeout for the socket and read the data
            socket.setSoTimeout(30 * 1000);  // In milliseconds                                
            try {
               bytesRead = -1;
               bytesRead = inputStream.read(buffer, totalBytesRead, numBytesToRead);               
            }
            catch (InterruptedIOException interrupted) {
               if (bytesRead == -1)
                  return -1;
            }
            catch (Exception exception) {               
               return -1;
            }
                                 
            if (bytesRead == -1) // i.e. End of stream reached
               break;
            else
               totalBytesRead += bytesRead;
            
         } while (totalBytesRead < totalBytesToRead);
                  
         return totalBytesRead;
      }
      catch (Exception exception) {         
         return FTPLib.ERROR_CODE;
      }
   }   
}
