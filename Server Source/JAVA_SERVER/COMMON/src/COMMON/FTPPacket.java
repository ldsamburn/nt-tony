/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package COMMON;

import java.util.Arrays;

/**
 * A data structure that represents a packet that has been sent between the FTP
 *  client and the daemon. These packets contain a "header" section which 
 *  contains information about the packet, and a data section which contains
 *  data from the file that is being transfered.
 */
public class FTPPacket
{
   public static final int XFER_NAMES_BUF_LEN = 100;
   public static final int XFER_INT_BUF_LEN   = 10;
   
   // The maximum size of the data portion of the packet
   public static final int MAX_DATA_SIZE = 262144;   // 256 Kbytes

   public int    cmd;        // FTP command
   public String file_name;  // Name of the file to operate on
   public int    gid;        // The group id (owner) of the file
   public int    uid;        // The user id (owner) of the file
   public int    file_mode;  // Contains file permission information
   public String hostname;   // Client hostname
   public int    len;        // Number of bytes contained in the "buf" field
   public byte   buf[] = new byte[MAX_DATA_SIZE]; // Contains data from the file 
   
   // These are the lengths of the each of the "header" fields when they are 
   //  read from or written to the socket. 
   //
   // NOTE: These fields were made static only so that rawHeaderLength (which 
   //  needed to be static) could be static.
   private static final int cmd_length       = XFER_INT_BUF_LEN;
   private static final int file_name_length = XFER_NAMES_BUF_LEN;
   private static final int gid_length       = XFER_INT_BUF_LEN;
   private static final int uid_length       = XFER_INT_BUF_LEN;
   private static final int file_mode_length = XFER_INT_BUF_LEN;
   private static final int hostname_length  = XFER_NAMES_BUF_LEN;
   private static final int len_length       = XFER_INT_BUF_LEN;   
   
   public static final int RAW_HEADER_SIZE = cmd_length + file_name_length + 
                                             gid_length + uid_length + 
                                             file_mode_length + hostname_length 
                                             + len_length;
   
   public byte rawHeaderInfo[] = new byte[RAW_HEADER_SIZE];
   
   /**
    * "Resets" the fields in this packet object
    */
   public void clear()
   {
      cmd       = -1;
      file_name = null;
      gid       = -1;
      uid       = -1;
      file_mode = -1;
      hostname  = null;
      len       = 0;      
      
      // They "buf" field doesn't really need to be cleaned out since only
      //  "len" bytes will be read out of it each time
   }
   
   /**
    * Parses the information in the given byte buffer into this
    *  data structure's header elements. If no errors occur, true is returned.
    */
   public boolean parseRawHeaderData(byte buffer[])
   {
      String data;
      int size;
      int start;
      
      try {   
         
         // Parse off the integer cmd
         start = 0;
         size = cmd_length;             
         cmd = getInteger(buffer, start, size);         
         
         // Parse off the string file name
         start += size;
         size = file_name_length;         
         file_name = getFilename(buffer, start, size);          
         
         // Parse off the integer gid
         start += size;
         size = gid_length;
         gid = getInteger(buffer, start, size);         
                  
         // Parse off the integer uid
         start += size;
         size = uid_length;
         uid = getInteger(buffer, start, size);         
                  
         // Parse off the integer file mode
         start += size;
         size = file_mode_length;
         file_mode = getInteger(buffer, start, size);         
                  
         // Parse off the string hostname
         start += size;
         size = hostname_length;
         data = new String(buffer, start, size);
         hostname = data.trim();

         // Parse off the integer length of the possible following data
         start += size;
         size = len_length;
         len = getInteger(buffer, start, size);         

         return true;
      }
      catch (Exception exception) {
         return false;
      }
   }
   
   /**
    * Creates a raw data string made up of the "header" information in this
    *  packet data structure. If any errors occur, null is returned. Otherwise, 
    *  the raw data string is returned.
    */
   public byte[] getRawHeaderData()
   {      
      String tmpStr;      
      int start, size;
      
      try {
            
         // Add the cmd field
         tmpStr = padString(String.valueOf(cmd), cmd_length, '\0');         
         start = 0;
         size = cmd_length;
         System.arraycopy(tmpStr.getBytes(), 0, rawHeaderInfo, start, size);

         // Add the filename field
         if (file_name != null)
            tmpStr = padString(file_name, file_name_length, '\0');         
         else
            tmpStr = padString("", file_name_length, '\0');               
         start += size;
         size = file_name_length;
         System.arraycopy(tmpStr.getBytes(), 0, rawHeaderInfo, start, size);
                
         // Add the gid field
         tmpStr = padString(String.valueOf(gid), gid_length, '\0');         
         start += size;
         size = gid_length;
         System.arraycopy(tmpStr.getBytes(), 0, rawHeaderInfo, start, size);
         
         // Add the uid field
         tmpStr = padString(String.valueOf(uid), uid_length, '\0');         
         start += size;
         size = uid_length;
         System.arraycopy(tmpStr.getBytes(), 0, rawHeaderInfo, start, size);
         
         // Add the file mode field
         tmpStr = padString(String.valueOf(file_mode), file_mode_length, '\0');         
         start += size;
         size = file_mode_length;
         System.arraycopy(tmpStr.getBytes(), 0, rawHeaderInfo, start, size);
             
         // Add the hostname field
         if (hostname != null)
            tmpStr = padString(hostname, hostname_length, '\0');         
         else
            tmpStr = padString("", hostname_length, '\0');         
         start += size;
         size = hostname_length;
         System.arraycopy(tmpStr.getBytes(), 0, rawHeaderInfo, start, size);
               
         // Add the len field
         tmpStr = padString(String.valueOf(len), len_length, '\0');         
         start += size;
         size = len_length;
         System.arraycopy(tmpStr.getBytes(), 0, rawHeaderInfo, start, size);                                         
                
         return rawHeaderInfo;
      }
      catch (Exception exception) {
         return null;
      }
   }   
   
   /**
    * Adds padding to the end of the given string so that the total length is
    *  equal to the given length. If the length of the string is
    *  greater than or equal to the given length parameter, then no padding
    *  is added to the end of the string. The resulting string is returned.
    */
   private static String padString(String string, int length, char padCharacter)
   {
      char padding[];
      
      try {
         
         // Check to see if the given string even needs padding
         if ((string == null) || (string.length() >= length))
            return string;
         
         // Create a padding string
         padding = new char[length - string.length()];
         Arrays.fill(padding, padCharacter);
         
         return string + new String(padding);
      }
      catch (Exception exception) {                  
         return string;
      }
   }
   
   /**
    * This method reads the bytes from "startIndex" to "startIndex + size" in 
    *  "buffer" and returns the integer version of this string.
    */
   private int getInteger(byte buffer[], int startIndex, int size)
   {
      int endIndex;
      byte newBuffer[];
      int position = 0;
      String tmpStr;
      
      try {       
         newBuffer = new byte[size];                  
         
         // Allow for the first byte to be a '-' indicating a negative number
         position = 0;
         if ((buffer[startIndex] != '-') &&
             ((buffer[startIndex] < 48) || (buffer[startIndex] > 57)))
            return -1;
         else
            newBuffer[position++] = buffer[startIndex];
         
         endIndex = startIndex + size;         
         for (int i = startIndex + 1; i < endIndex; i++) {
            if ((buffer[i] >= 48) && (buffer[i] <= 57))
               newBuffer[position++] = buffer[i];
            else
               break;
         }                  
        
         tmpStr = new String(newBuffer);
         return Integer.parseInt(tmpStr.trim());
      }
      catch (Exception exception) {       
         return -1;
      }
   }
   
   /**
    * Returns the data in this field as a string. The main reason for this
    *  method is for checking for path seperators that might cause problems
    *  and correct them here.
    */
   private String getFilename(byte buffer[], int startIndex, int size)
   {
      int endIndex;
      byte newBuffer[];
      int newBufIndex;
      
      try {
         
         newBuffer = new byte[size];
         
         endIndex = startIndex + size;         
         newBufIndex = 0;
         for (int index = startIndex; index < endIndex; index++) {
            if (buffer[index] == (byte)0)
               break;
            
            // Change any backward slashes to forward slashes
            if (buffer[index] == '\\')
               newBuffer[newBufIndex++] = '/';
            else
               newBuffer[newBufIndex++] = buffer[index];
         }
         
         return (new String(newBuffer, 0, newBufIndex)).trim();
      }
      catch (Exception exception) {
         return null;
      }
   }
}


