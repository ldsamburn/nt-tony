/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

//Description:  An object used to convieniently send and receive a byte stream
//               of data to/from a specified host and port. This is a subclass
//               of the MessengerBase abstract base class.
//
//              NOTE: There must be some sort of socket "listener" on the host
//                    and port specified to be able to send and receive any
//                    data.
//
//              See Also: MessengerBase, MulticastMessenger
/*
    Usage example:

    // To send a byte stream
    String data = "Text to send";
    byte[] byteStr = data.getBytes();
    HostToHostMessenger messenger = new HostToHostMessenger(host, port);
    if (messenger.send(byteStr) == false)
       System.out.println("Could not send data");
    messenger.finished();

    // To receive a byte stream
    byte buffer = new byte[dataLength];
    HostToHostMessenger messenger = new HostToHostMessenger(host, port);
    if (messenger.receive(buffer, buffer.length) == false)
       System.out.println("Could not receive data");
    messenger.finished();

*/


package COMMON;

import java.net.*;
import java.io.*;

public class HostToHostMessenger extends MessengerBase
{
   protected Socket rSocket;
   protected BufferedInputStream input;
   protected BufferedOutputStream output;

   // Sets up a regular socket connection between the local host and the
   //  given host
   public HostToHostMessenger(String host, int port)
   {
      try {
         rSocket = new Socket(host, port);
         output = new BufferedOutputStream(rSocket.getOutputStream());
         input = new BufferedInputStream(rSocket.getInputStream());
      }
      catch (Exception e) {
         rSocket = null;
      }
   }

   // Sets up a regular socket connection between the local host and the host
   //  on the other end of the given socket
   public HostToHostMessenger(Socket skt)
   {
      try {
         rSocket = skt;
         output = new BufferedOutputStream(rSocket.getOutputStream());
         input = new BufferedInputStream(rSocket.getInputStream());
      }
      catch (Exception e) {
         rSocket = null;
      }
   }

   // Sends the given data through the regular socket stream
   public synchronized boolean send(byte[] data)
   {
      boolean ok = true;

      if (isConnected() == true) {
         try {
            output.write(data, 0, data.length);
            output.flush();
         }
         catch (Exception e) {
            ok = false;
         }
      }
      else
         ok = false;

      return ok;
   }

   // Retrieves "size" number of bytes from the regular socket stream and
   //  stores them in the given buffer
   public synchronized boolean receive(byte[] buffer, int size, int timeout) 
                                                  throws InterruptedIOException
   {
      int bufferIndex = 0;
      int totalBytesRead = 0;
      int numBytesToRead;
      int numBytesRead;
      boolean ok = true;

      if (isConnected() == true) {
         do {
            numBytesToRead = size - totalBytesRead;

            try {
               // Try to read at most "numBytesToRead" bytes
               rSocket.setSoTimeout(timeout * 1000);
               numBytesRead = input.read(buffer, bufferIndex, numBytesToRead);
               totalBytesRead += numBytesRead;
               bufferIndex += numBytesRead;
            }
            catch (InterruptedIOException timeoutException) {
               throw timeoutException;
            }
            catch (Exception e) {
               ok = false;
               break;
            }
         } while (totalBytesRead < size);
      }
      else
         ok = false;

      return ok;
   }

   // Closes the communication connection and any I/O stream connections
   public synchronized void finished()
   {
      try {
         if (rSocket != null) {
            rSocket.close();
            rSocket = null;
         }
         if (input != null) {
            input.close();
            input = null;
         }
         if (output != null) {
            output.close();
            output = null;
         }
      }
      catch (Exception e) {}
   }

   // Returns wether or not this messenger has a connection
   protected boolean isConnected()
   {
      if (rSocket != null)
         return true;
      else
         return false;
   }
}