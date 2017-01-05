/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

//Description:  This class represents a packet of data that will be sent to or
//               received from a specified host and port. There are methods
//               for setting up a packet and for sending it, and for receiving
//               a packet and retrieving the data from it. The packet is made
//               up of two sections - the "header" section and the "data"
//               section. The header section can be used to identify the
//               message, and the data section can be used to attach any other
//               information to the message.
//
//              This object uses a derived class of the MessengerBase class to
//               actually deliver the packet.
//
//              See also: MessageBase, MessengerBase
/*
      Usage example:

      // Setup the message
      MessageWithData msg = new MessageWithData(host, port);
      msg.setHeader("header");
      msg.setData("data");

      // Send the message
      if (msg.send() == true) {

         // Wait for a response
         if (msg.receive() == true) {

            // Check the response
            if (msg.headerEquals("good"))
               System.out.println("Received response `good`");
            else if (msg.headerEquals("bad"))
               System.out.println("Received response `bad`");

            System.out.println(msg.getData());
         }
         else
            System.out.println("Unable to receive response");
      }
      else
         System.out.println("Unable to send message");

      // Clean up the message mechanism
      msg.finished();
*/


package COMMON;

import java.net.*;
import java.io.InterruptedIOException;

public class MessageWithData extends MessageBase
{
   protected byte[] data;
   protected final int dataLengthFieldSize = 5;
   public static final String dataFieldDelimiter = "/";

   // Used to set up a message to be sent/received to/from a single specified
   //  host
   public MessageWithData(String host, int port)
   {
      messenger = new HostToHostMessenger(host, port);
      data = null;
   }

   // Used to set up a message to be sent/received to/from a single host
   //  that's on the other end of the given socket
   public MessageWithData(Socket socket)
   {
      messenger = new HostToHostMessenger(socket);
      data = null;
   }

   // Used to set up a message to be sent/received to/from a multicast group
   public MessageWithData(int multicastPort, String multicastIP)
   {
      messenger = new MulticastMessenger(multicastIP, multicastPort);
      data = null;
   }

   // Sets the data portion of the message
   public void setData(String newData)
   {
      if (newData != null)
         data = newData.getBytes();
      else
         data = null;
   }

   // Sets the data portion of the message
   public void setData(byte[] newData)
   {
      data = newData;
   }

   // Gets the data portion of the message
   public String getData()
   {
      if (data != null)
         return new String(data);
      else
         return null;
   }

   // Formats the message and sends it to the previously specified destination
   public boolean send()
   {
      MessageHeader formattedHeader = header.format();
      int bufferIndex = 0;
      int index;
      String dataLength;

      // Figure out the size the send buffer needs to be
      if (data != null) {
         dataLength = Integer.toString(data.length);
         buffer = new byte[MessageHeader.headerSize + dataLengthFieldSize +
                           data.length];
      }
      else {
         dataLength = "0";
         buffer = new byte[MessageHeader.headerSize + dataLengthFieldSize];
      }

      // Copy the message header into the buffer
      byte[] header_buf = (formattedHeader.get()).getBytes();
      for (index = 0; index < MessageHeader.headerSize; index++)
         buffer[bufferIndex++] = header_buf[index];

      // Create the data length field and copy it into the buffer
      while (dataLength.length() < dataLengthFieldSize)
         dataLength = "0" + dataLength;
      byte[] length_buf = dataLength.getBytes();
      for (index = 0; index < dataLengthFieldSize; index++)
         buffer[bufferIndex++] = length_buf[index];

      // Check to see if there is any data to be copied into the buffer
      if (data != null) {
         for (index = 0; index < data.length; index++)
            buffer[bufferIndex++] = data[index];
      }

      // Send the message
      return messenger.send(buffer);
   }

   // Reads a message with data from the previously specified destination.
   //  NOTE: If any new messenger types are created, then they need to be
   //        added to the "if-else" statements below!
   public boolean receive()
   {
      try {
         return receive(0); 
      }
      // This exception should never be thrown since the timeout value is infinite
      catch (InterruptedIOException timeoutException) {
         return false;
      }
   }
   
   // Timeout in milliseconds. Throws the "timeout" exception if the timeout
   //  expires.
   public boolean receive(int timeout) throws InterruptedIOException
   {
      boolean ret = false;

      // This had to be split up due to the nature of the Multicast socket
      //  communication
      if (messenger instanceof HostToHostMessenger)
         ret = receiveFromHost(timeout);
      else if (messenger instanceof MulticastMessenger)
         ret = receiveFromMulticast(timeout);

      return ret;
   }

   // Reads a message with data from the previously specified host
   private boolean receiveFromHost(int timeout) throws InterruptedIOException
   {
      int dataLength;

      // Get the header information and set the header member
      buffer = new byte[MessageHeader.headerSize];
      if (messenger.receive(buffer, buffer.length, timeout) == false)
         return false;
      setHeader((new String(buffer)).trim());

      // Get the data length field of the message
      buffer = new byte[dataLengthFieldSize];
      if (messenger.receive(buffer, buffer.length, timeout) == false)
         return false;
      dataLength = Integer.parseInt(new String(buffer));

      // If there is any data, retreive it and set the data member
      if (dataLength > 0) {
         buffer = new byte[dataLength];
         if (messenger.receive(buffer, buffer.length, timeout) == false)
            return false;
         setData(buffer);
      }
      else
         data = null;

      return true;
   }

   // Reads a message with data from the multicast group
   private boolean receiveFromMulticast(int timeout) throws InterruptedIOException
   {
      int dataLength;
      String tmp;
      int maxDataBytes;
      int bufferIndex = 0;
      int index;
      byte[] tmpBuffer;

      // Figure out the largest amount of data (besides the header and data
      //  length fields) that can be received
      tmp = "9";
      for (int i = 0; i < dataLengthFieldSize - 1; i++)
         tmp = tmp + "9";
      maxDataBytes = Integer.parseInt(tmp);

      // Create the biggest buffer possible and read data into it
      buffer = new byte[MessageHeader.headerSize + dataLengthFieldSize +
                        maxDataBytes];
      if (messenger.receive(buffer, buffer.length, timeout) == false)
         return false;

      // Get the header information and set the header member
      tmpBuffer = new byte[MessageHeader.headerSize];
      for (index = 0; index < MessageHeader.headerSize; index++)
         tmpBuffer[index] = buffer[bufferIndex++];
      setHeader((new String(tmpBuffer)).trim());

      // Get the data length field of the message
      tmpBuffer = new byte[dataLengthFieldSize];
      for (index = 0; index < dataLengthFieldSize; index++)
         tmpBuffer[index] = buffer[bufferIndex++];
      dataLength = Integer.parseInt(new String(tmpBuffer));

      // If there is any data, get it from the buffer and set the data member
      if (dataLength > 0) {
         tmpBuffer = new byte[maxDataBytes];
         for (index = 0; index < maxDataBytes; index++)
            tmpBuffer[index] = buffer[bufferIndex++];
         setData(tmpBuffer);
      }
      else
         data = null;

      return true;
   }
}
