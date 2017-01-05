/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

//Description:  This object implements the "header" part of the packet
//               implemented by the MessageBase class.


package COMMON;

public class MessageHeader
{
   private String header;

   //!!! NOTE: The number of characters in a header string cannot be greater
   //          than headerSize !!!
   public static final int headerSize = 20;

   // Sets the header member
   public void set(String newHdrType)
   {
      header = new String(newHdrType);
   }

   // Returns a copy of the header member
   public String get()
   {
      if (header != null)
         return new String(header);
      else
         return null;
   }

   // Returns a copy of the header that has been padded at the end with spaces
   //  so that its total length is "headerSize"
   public MessageHeader format()
   {
      byte[] tmp;
      MessageHeader newHeader = new MessageHeader();
      int numPadSpaces = headerSize - header.length();

      // Determine if the header needs space padding
      if (numPadSpaces > 0) {

         // Create an array containing space characters
         tmp = new byte[numPadSpaces];
         for (int i = 0; i < numPadSpaces; i++)
            tmp[i] = ' ';

         newHeader.set(get() + new String(tmp));
      }
      else
         newHeader.set(get());

      return newHeader;
   }
}
