/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

//Description:  An abstract base class which creates the framework for an
//               object which can send and receive a byte stream over a socket.
//
//              See Also: HostToHostMessenger, MulticastMessenger


package COMMON;

import java.io.InterruptedIOException;

public abstract class MessengerBase
{
   // Sends the given data through the proper communication socket
   public abstract boolean send(byte[] data);

   // Retrieves "size" number of bytes from the communication stream and
   //  stores them in the given buffer. The timeout value is in seconds
   public abstract boolean receive(byte[] buffer, int size, int timeout) throws InterruptedIOException;

   // Closes the communication connection and any I/O stream connections
   public abstract void finished();

   // Determines if the messenger has a connection or not
   protected abstract boolean isConnected();
} 