/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

//Description:  An object used to convieniently send and receive a byte stream
//               of data to/from a multicast IP and port. This is a subclass
//               of the MessengerBase abstract base class.
//
//              See Also: MessengerBase, HostToHostMessenger
/*
    Usage example:

    // To send a byte stream
    String data = "Text to send";
    byte[] byteStr = data.getBytes();
    MulticastMessenger messenger = new MulticastMessenger(IP, port);
    if (messenger.send(byteStr) == false)
       System.out.println("Could not send data");
    messenger.finished();

    // To receive a byte stream
    byte buffer = new byte[dataLength];
    MulticastMessenger messenger = new MulticastMessenger(IP, port);
    if (messenger.receive(buffer, buffer.length) == false)
       System.out.println("Could not receive data");
    messenger.finished();

*/


package COMMON;

import java.net.*;
import java.io.*;

public class MulticastMessenger extends MessengerBase
{
   protected InetAddress group;
   protected MulticastSocket mSocket;
   protected int multicastPort;

   // Sets up a connection to the multicast group
   public MulticastMessenger(String multiIP, int multiPort)
   {
      // Joins the multicast group
      try {
         multicastPort = multiPort;

         group = InetAddress.getByName(multiIP);
         mSocket = new MulticastSocket(multicastPort);
         mSocket.joinGroup(group);
      }
      catch (Exception e) {
         mSocket = null;
      }
   }

   // Sends the given data to the multicast group
   public synchronized boolean send(byte[] data)
   {
      boolean ok = true;
      DatagramPacket packet;

      if (isConnected() == true) {
         try {
            packet = new DatagramPacket(data, data.length, group,
                                        multicastPort);
            mSocket.send(packet);
         }
         catch (Exception e) {
            ok = false;
         }
      }
      else
         ok = false;

      return ok;
   }

   // Retrieves "size" number of bytes from the multicast socket and
   //  stores them in the given buffer
   public synchronized boolean receive(byte[] buffer, int size, int timeout)
                                 throws InterruptedIOException
   {
      boolean ok = true;
      DatagramPacket packet;

      if (isConnected() == true) {
         try {
            packet = new DatagramPacket(buffer, size);

            // Host and port information from the sending host can be retrieved
            //  from the packet
            mSocket.setSoTimeout(timeout * 1000);
            mSocket.receive(packet);
         }
         catch (InterruptedIOException timeoutException) {
            throw timeoutException;
         }
         catch (Exception e) {
            ok = false;
         }
      }
      else
         ok = false;

      return ok;
   }

   // Closes the communication connection and any I/O stream connections
   public synchronized void finished()
   {
      try {
         if (mSocket != null) {
            mSocket.leaveGroup(group);
            mSocket.close();
            mSocket = null;
         }
      }
      catch (Exception e) {}
   }

   // Returns wether or not this messenger has a connection
   protected boolean isConnected()
   {
      if (mSocket != null)
         return true;
      else
         return false;
   }
}