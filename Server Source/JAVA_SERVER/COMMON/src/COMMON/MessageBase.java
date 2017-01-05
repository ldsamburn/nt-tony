/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

//Description:  An abstract base class which creates the framework for an
//               object which represents a packet of data that will be sent to
//               or received from a specified host and port. There are methods
//               for setting up a packet and for sending it, and for receiving
//               a packet and retrieving the data from it. The packet is made
//               up of just a "header" section. The header section can be used
//               to identify the message. The dervied class "MessageWithData"
//               extends this class by adding a "data" section to message.
//
//              This object uses a derived class of the MessengerBase class to
//               actually deliver the packet.
//
//              See also: MessageWithData, MessengerBase


package COMMON;


public abstract class MessageBase
{
   protected byte[] buffer; // Used for holding the data for sending/receiving
   protected MessengerBase messenger;
   protected MessageHeader header;

   public MessageBase()
   {
      // Instantiate messenger in derived class

      header = new MessageHeader();
   }

   // Sets the header portion of the message
   public void setHeader(String newHeader)
   {
      header.set(newHeader);
   }

   // Gets the header portion of the message
   public String getHeader()
   {
      return header.get();
   }

   // Returns true if the header equals the given header
   public boolean headerEquals(String hdr)
   {
      return (getHeader().equals(hdr));
   }

   // Trys to send messge and returns success or failure
   public abstract boolean send();

   // Trys to receive message and returns success or failure
   public abstract boolean receive();

   // Used to perform any clean-up especially Messenger cleanup
   public void finished()
   {
      messenger.finished();
   }
}