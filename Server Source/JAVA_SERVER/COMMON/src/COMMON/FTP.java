/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package COMMON;

import java.net.Socket;
import java.net.InetAddress;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileWriter;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.OutputStream;

/**
 * Provides a "get" and "put" method for getting and putting files from/to
 *  a remote host.
 */
public class FTP
{
   // States
   private static final int XFER_INIT_WRITE     = 0;
   private static final int XFER_OPEN_LOCAL     = 1;
   private static final int XFER_OPEN_REMOTE    = 2;
   private static final int XFER_GET_RESP       = 3;
   private static final int XFER_READ_LOCAL     = 4;
   private static final int XFER_WRITE_REMOTE   = 5;
   private static final int XFER_CLOSE          = 6;
   private static final int XFER_DONE           = 7;
   private static final int XFER_INIT_READ      = 8;
   private static final int XFER_READ_REMOTE    = 10;
   private static final int XFER_WRITE_LOCAL    = 11;
   private static final int XFER_CLOSE_LOCAL    = 12;
   
   private static final int XFER_TRANSFER_ERROR = 99;      
   
   private static FileWriter traceFile = null;
   
   /**
    * Gets the specified remote file from the specified remote host and writes
    *  its contents locally to "localFilename"
    */
   public static boolean get(String remoteHostname, String remoteFilename,
                             String localFilename, boolean isText)
   {
      FTP ftp = new FTP();
      
      return ftp.GetFile(remoteHostname, remoteFilename, localFilename, isText);
   }
   
   /**
    * Gets the specified remote file from the specified remote host and writes
    *  its contents locally to "localFilename".
    *
    * NOTE: This instance method only exists to allow older users of this
    *       FTP class to function.
    */
   public boolean GetFile(String remoteHostname, String remoteFilename,
                          String localFilename, boolean isText)
   {
      Socket socket;
      BufferedInputStream inputStream;
      BufferedOutputStream outputStream;
      FTPPacket packet = new FTPPacket();      
      FileWtr file_ptr = null;
      int state = XFER_INIT_READ;
      boolean done_fopen = false;
      boolean done = false;
      boolean success = true;            
      File localFile;
      String localHostname;      
      
      try { 
         
         openTraceFile();
         trace("ENTERING GetFile to transfer remote file " + remoteFilename +
                  " from host " + remoteHostname + " to local file " +
                  localFilename);
         
         // Check the parameters passed in
         if ((remoteHostname.length() > FTPPacket.XFER_NAMES_BUF_LEN) ||
             (remoteFilename.length() > FTPPacket.XFER_NAMES_BUF_LEN) ||
             (localFilename.length()  > FTPPacket.XFER_NAMES_BUF_LEN)) {
            trace("Host and/or file name length error");
            return false;
         }
         
         // Create a connection to the FTP server
         try {
            socket = new Socket(remoteHostname, FTPLib.DAEMON_PORT);
            outputStream = new BufferedOutputStream(socket.getOutputStream());
            inputStream = new BufferedInputStream(socket.getInputStream());
         }
         catch (Exception exp) {
            trace(exp.toString());
            return false;
         }
                 
         localFile = new File(localFilename);
         localHostname = (InetAddress.getLocalHost()).getHostName();
         
         // Process the request, moving through different "states"
         while (done == false) {
            
            switch (state) {
               
               // Receieve ACK from remote host after XFER_INIT_READ and after
               //  XFER_WRITE_LOCAL (OK_PACK case), here just read data,
               //  actually check contents in XFER_OPEN_LOCAL and WRITE_LOCAL               
               case XFER_GET_RESP:
                  
                  trace("XFER_GET_RESP");
                  
                  if (FTPLib.readPacket(packet, socket, 
                                        inputStream) != FTPLib.ERROR_CODE) {
                     if (!done_fopen)
                        state = XFER_OPEN_LOCAL;
                     else 
                        state = XFER_WRITE_LOCAL;                                             
                  }
                  else {
                     trace("Error reading socket response");
                     state = XFER_TRANSFER_ERROR;
                  }
                  
                  trace("PacketRead done");
                  trace("Command:        " + packet.cmd);
                  trace("File name:      " + packet.file_name);
                  trace("gid:            " + packet.gid);
                  trace("uid:            " + packet.uid);
                  trace("file mode:      " + packet.file_mode);
                  trace("host name:      " + packet.hostname);
                  trace("length:         " + packet.len);
      
                  break;

               // Open remote file
               case XFER_INIT_READ:
                  trace("XFER_INIT_READ");
                  
                  if (sendCommand(packet, outputStream, FTPLib.OPEN_READ_NC, 
                                 remoteFilename, localHostname)) {
                     state = XFER_GET_RESP;
                     packet.file_name = new String(remoteFilename);
                  }
                  else {
                     trace("Error opening remote file <" + remoteFilename + ">");
                     state = XFER_TRANSFER_ERROR;
                  }
                  break;

               // After have done XFER_GET_RESP for the XFER_INIT_READ then get 
               //  to XFER_OPEN_LOCAL to verify status and to open local file 
               //  for write
               case XFER_OPEN_LOCAL:
                  trace("XFER_OPEN_LOCAL");

                  // First make sure is ok
                  if (packet.cmd == FTPLib.OK_PACK) {

                     // If local file exists, try to delete it
                     if (localFile.exists()) {
                        
                        if (localFile.isFile())    // If able to delete
                           localFile.delete();
                        else {
                           trace("Unable to remove file <" + localFilename + ">");
                           state = XFER_TRANSFER_ERROR;
                        } 
                     }

                     // Now open local file for writing and then start reading 
                     //  file from remote host
                     file_ptr = new FileWtr(localFile, isText);
                     if (file_ptr.openError() == false) {
                        done_fopen = true;
                        sendCommand(packet, outputStream, FTPLib.READ_NC, 
                                   remoteFilename, localHostname);
                        state = XFER_GET_RESP;
                     }
                     else {
                        trace("Unable to open file for writing <" +
                                             localFilename + ">");
                        state = XFER_TRANSFER_ERROR;
                     }
                  }
                  else {
                     trace("Error opening remote file <" +
                                          remoteFilename +">");
                     state = XFER_TRANSFER_ERROR;
                  }
                  break;

               // Just got data from remote host, now verify ok and write data 
               //  to local file
               case XFER_WRITE_LOCAL:
                  trace("XFER_WRITE_LOCAL");

                  switch (packet.cmd) {
                     
                     // Standard case, received ok. Write data to local file and
                     //  and ask remote host for more
                     case FTPLib.OK_PACK:
                        trace("XFER_OK_PACK");

                        if (file_ptr.write(packet.buf, packet.len) == true) {
                           state = XFER_GET_RESP;
                           sendCommand(packet, outputStream, FTPLib.READ_NC, 
                                      remoteFilename, localHostname);
                        }
                        else {
                           trace("Local file write error error <" +
                                                 localFilename + ">");
                           sendCommand(packet, outputStream, FTPLib.CLOSE_READ_NC, 
                                      remoteFilename, localHostname);
                           state = XFER_TRANSFER_ERROR;
                        }
                        break;

                     // EOF case, if any data at all came over then write it to
                     //  file and then do XFER_CLOSE_LOCAL
                     case FTPLib.EOF_PACK:
                        trace("XFER_EOF_PACK");
                        state = XFER_CLOSE_LOCAL;

                        if (packet.len != 0) {
                           if (file_ptr.write(packet.buf, packet.len)) {
                              trace("Unexpected end of file");
                              state = XFER_TRANSFER_ERROR;
                           }
                        }
                        break;

                     // Error case, didn't get good ack from remote host
                     case FTPLib.NOK_PACK:
                        trace("XFER_NOK_PACK");
                        trace("File transfer error (invalid server acknowledgement)");
                        state = XFER_TRANSFER_ERROR;
                        break;

                     default:
                        trace("Unexpected server response");
                        state = XFER_TRANSFER_ERROR;
                        break;
                  }
                  break;

               // So far so good, close file and socket then done ok
               case XFER_CLOSE_LOCAL:
                  trace("XFER_CLOSE_LOCAL");
                  file_ptr.close();                  
                  done = true;
                  success = true;
                  break;

               // Errors go through here
               case XFER_TRANSFER_ERROR:
                  trace("XFER_TRANSFER_ERROR");
                  if (file_ptr != null)
                     file_ptr.close();                  
                  done = true;
                  success = false;
                  break;

               // Shouldn't ever be here
               default:
                  trace("File transfer state error");
                  state = XFER_TRANSFER_ERROR;
                  break;
                  
            }  // End switch
         } // End while loop
      
         // Close the system resources
         inputStream.close();
         outputStream.close();
         socket.close();
         closeTraceFile();
         
         return success; 
      }
      catch (Exception exception) {
         trace(exception.toString());
         return false;
      }
   }
   
   /**
    * Puts the local file "localFilename" to the specified remote host where
    *  the file's contents will be written to the remote file "remoteFilename".
    */
   public static boolean put(String remoteHostname, String remoteFilename,
                             String localFilename, boolean isText, 
                             boolean append)
   {
      FTP ftp = new FTP();
      
      return ftp.PutFile(remoteHostname, remoteFilename, localFilename, isText, 
                         append);
   }
      
   /**
    * Puts the local file "localFilename" to the specified remote host where
    *  the file's contents will be written to the remote file "remoteFilename".
    *
    * NOTE: This instance method only exists to allow older users of this
    *       FTP class to function.
    */
   public boolean PutFile (String remoteHostname, String remoteFilename,
                           String localFilename, boolean isText, boolean append)
   {
      Socket socket;
      BufferedInputStream inputStream;
      BufferedOutputStream outputStream;
      FTPPacket packet = new FTPPacket(); 
      int read_cnt = 0;
      FileRdr file_ptr = null;
      int state = XFER_INIT_WRITE;
      boolean done_fclose = false;
      boolean done = false;
      boolean success = true;      
      File localFile;
      String localHostname;
      
      try {
         
         openTraceFile();
         trace("ENTERING PutFile to transfer local file " + localFilename +
               " to host " + remoteHostname + " file " + remoteFilename);
         
         // Check the parameters passed in
         if ((remoteHostname.length() > FTPPacket.XFER_NAMES_BUF_LEN) ||
             (remoteFilename.length() > FTPPacket.XFER_NAMES_BUF_LEN) ||
             (localFilename.length()  > FTPPacket.XFER_NAMES_BUF_LEN)) {
            trace("Host and/or file name length error");
            return false;
         }
         
         // Create a connection to the FTP server
         try {
            socket = new Socket(remoteHostname, FTPLib.DAEMON_PORT);
            outputStream = new BufferedOutputStream(socket.getOutputStream());
            inputStream = new BufferedInputStream(socket.getInputStream());
         }
         catch (Exception exp) {
            trace(exp.toString());
            return false;
         }
         
         localFile = new File(localFilename);
         localHostname = (InetAddress.getLocalHost()).getHostName();

         // Process the request, moving through different "states"
         while (done == false) {
            
            switch (state) {
               
               // Receieve ACK from remote host after OPEN_REMOTE, after
               //  WRITE_REMOTE and after CLOSE.
               case XFER_GET_RESP:
                  trace("XFER_GET_RESP");
                  
                  if (FTPLib.readPacket(packet, socket, 
                                        inputStream) != FTPLib.ERROR_CODE) {
                     
                     if (packet.cmd == FTPLib.OK_PACK) {
                        
                        // For GET_RESP state next state is always READ_LOCAL until
                        //  the flag done_fclose becomes TRUE
                        if (done_fclose)
                           state = XFER_DONE;
                        else
                           state = XFER_READ_LOCAL;
                     }
                     else {
                        trace("Remote file open/read error on " + remoteFilename);
                        state = XFER_TRANSFER_ERROR;
                     }
                  }
                  else {
                     trace("Error reading socket data response");
        	     state = XFER_TRANSFER_ERROR;
                  }
                  
                  trace("PacketRead done");
                  trace("Command:        " + packet.cmd);
                  trace("File name:      " + packet.file_name);
                  trace("gid:            " + packet.gid);
                  trace("uid:            " + packet.uid);
                  trace("file mode:      " + packet.file_mode);
                  trace("host name:      " + packet.hostname);
                  trace("length:         " + packet.len);
                  
                  break;

               // Make sure local file exists
               case XFER_INIT_WRITE:
                  trace("XFER_INIT_WRITE");
               
                  // Check to see if the file exists locally
                  if (localFile.exists() == true)
                     state = XFER_OPEN_LOCAL;
                  else {
                     state = XFER_TRANSFER_ERROR;
                     trace("File not found or invalid file type <" + 
                           localFilename + ">");
                  }
                  break;

               // Open local file
               case XFER_OPEN_LOCAL:
                  trace("XFER_OPEN_LOCAL");
               
                  file_ptr = new FileRdr(localFile, isText);
                  if (file_ptr.openError() == false)
                     state = XFER_OPEN_REMOTE;
                  else {
                     trace("No such file or directory <" + localFilename + ">");
                     state = XFER_TRANSFER_ERROR;
                  }
                  break;

               // Have remote host open remote file
               case XFER_OPEN_REMOTE:
                  trace("XFER_OPEN_REMOTE");
               
                  if (sendCommand(packet, outputStream, 
                                 append ? FTPLib.OPEN_NC_APPEND : FTPLib.OPEN_NC,
                                 remoteFilename, localHostname) == false) {
                     trace("Unable to open remote file <" + remoteFilename + ">");
                     state = XFER_TRANSFER_ERROR;
                  }
                  else
                     state = XFER_GET_RESP;
                  break;

               // Read another block from the the local file unless hit eof
               case XFER_READ_LOCAL:
                  trace("XFER_READ_LOCAL");

                  read_cnt = file_ptr.read(packet.buf, FTPPacket.MAX_DATA_SIZE);
                  if (read_cnt == -2) {
                     trace("Error in local file read <" + localFilename + ">");
                     state = XFER_TRANSFER_ERROR;
                  }
                  else if (read_cnt == -1)  // i.e. reached EOF - no data read
                     state = XFER_CLOSE;
                  else
                     state = XFER_WRITE_REMOTE;
                  break;

               // Send another block to remote host
               case XFER_WRITE_REMOTE:
                  trace("XFER_WRITE_REMOTE");
               
                  packet.cmd = FTPLib.WRITE_NC;
                  packet.len = read_cnt;
                  packet.hostname = new String(localHostname);

                  if (FTPLib.writePacket(packet, outputStream) == FTPLib.ERROR_CODE) {
                     state = XFER_TRANSFER_ERROR;
	             trace("Socket transfer error.");
    	          }
                  else                                           
            	     state=XFER_GET_RESP;
                                    
                  trace("PacketWrite done");
                  trace("Command:        " + packet.cmd);
                  trace("File name:      " + packet.file_name);
                  trace("gid:            " + packet.gid);
                  trace("uid:            " + packet.uid);
                  trace("file mode:      " + packet.file_mode);
                  trace("host name:      " + packet.hostname);
                  trace("length:         " + packet.len);
                  
                  break;

               // Done, close local file and send close file cmd to remote host
               case XFER_CLOSE:
                  trace("XFER_CLOSE");
                  file_ptr.close();
                  file_ptr = null;
                  sendCommand(packet, outputStream, FTPLib.CLOSE_NC, 
                             remoteFilename, localHostname);
                  state = XFER_GET_RESP;
                  done_fclose = true;
                  break;

               // Finished transfer, everything ok
               case XFER_DONE:
                  trace("XFER_DONE");
                  done = true;
                  success = true;
                  break;

               // Ran into a problem
               case XFER_TRANSFER_ERROR:
                  trace("XFER_TRANSFER_ERROR");
               
                  if (file_ptr != null)
                     file_ptr.close();
                  done = true;
                  success = false;
                  break;

               // Just in case
               default:
                  trace("File transfer state error");
                  state = XFER_TRANSFER_ERROR;
                  break;
                  
            }  // End of switch statement
         }  // End of while loop         
         
         // Close the system resources
         inputStream.close();
         outputStream.close();
         socket.close();
         closeTraceFile();
      
         return success;
      }
      catch (Exception exception) {
         trace(exception.toString());
         return false;
      }
   }      
   
   /**
    * Updates the given packet with the given cmd, filename, and local hostname 
    *  and then sends it out. There is no data sent with the packet. The
    *  packet really just holds the command information.
    */
   private boolean sendCommand(FTPPacket pkt, OutputStream outputStream, int cmd, 
                               String filename, String localHostname)
   {      
      boolean successfulSend;
         
      pkt.cmd = cmd;
      pkt.file_name = new String(filename);
      pkt.gid = 0;
      pkt.uid = 0;
      pkt.file_mode = 0666;
      pkt.hostname = new String(localHostname);
      pkt.len = 0;

      if (FTPLib.writePacket(pkt, outputStream) == FTPLib.ERROR_CODE)
         successfulSend = false;
      else
         successfulSend = true;

      trace("PacketWrite done");
      trace("Command:        " + pkt.cmd);
      trace("File name:      " + pkt.file_name);
      trace("gid:            " + pkt.gid);
      trace("uid:            " + pkt.uid);
      trace("file mode:      " + pkt.file_mode);
      trace("host name:      " + pkt.hostname);
      trace("length:         " + pkt.len);
                  
      return successfulSend;
   }
   
   /**
    * Logs the given string.         
    */
   private static void trace(String str)
   {
      if (traceFile != null)
         
         try {
            traceFile.write(str + System.getProperty("line.separator"));
            traceFile.flush();
         }
         catch (IOException e) {
         }
       
   }   

   /**
    * Sets up the FTP trace file
    *
    *  WARNING!: There is not locking here, so other threads or processes
    *           could be writting to the same file
    */
   private static void openTraceFile()
   {
      try {
         traceFile = new FileWriter("traceFile");
      }
      catch (IOException e) {
      }
   }

   /**
    * Closes the FTP trace file
    */
   private static void closeTraceFile()
   {
      if (traceFile != null)
         try {
            traceFile.close();
         }
         catch (IOException e) {
         }
         
      traceFile = null;
   } 
}


/**
 * Used to open and read from a binary file or text file
 */
class FileRdr
{
   private BufferedInputStream stream;
   private boolean is_text;

   /**
    * Creates an input stream from the specified file
    */
   public FileRdr(File file, boolean isText)
   {
      is_text = isText;

      try {
         stream = new BufferedInputStream(new FileInputStream(file));
      }
      catch (FileNotFoundException e) {
         stream = null;
      }
   }

   /**
    * Checks to see if the file was opened ok
    */
   public boolean openError()
   {
      if (stream != null)
         return false;
      else
         return true;
   }

   /**
    * Reads from the input stream into the given buffer
    */
   public int read(byte[] buffer, int length)
   {
      int bytesRead = 0;
      int item;

      try {
/*         
         for (int i = 0; i < length;) {
            item = stream.read();
            if (item == -1) { // No item read; EOF reached
               if (bytesRead == 0)
                  bytesRead = -1;
               break;
            }
            else {
               if (is_text == true) {
                  if (item != 13) { // skip over Carriage Returns
                     bytesRead++;
                     buffer[i++] = (byte)item;
                  }
               }
               else {
                  buffer[i++] = (byte)item;
                  bytesRead++;
               }
            }
         }
 */
         bytesRead = stream.read(buffer, 0, length);
      }
      catch (IOException e) {
         bytesRead = -2;
      }

      return bytesRead;
   }

   /**
    * Closes the file input stream
    */
   public void close()
   {
      try {
         stream.close();
      }
      catch (Exception e) {
      }
   }

}

/**
 * Used to open and write to a binary file or text file
 */
class FileWtr
{
   private FileOutputStream stream;
   private boolean is_text;
   
   byte[] lineSeparator = (System.getProperty("line.separator")).getBytes();

   /**
    * Creates an output stream from the specified file
    */
   public FileWtr(File file, boolean isText)
   {
      is_text = isText;

      try {
         stream = new FileOutputStream(file);
      }
      catch (IOException e) {
         stream = null;
      }
   }

   /**
    * Checks to see if the file was opened ok
    */
   public boolean openError()
   {
      if (stream != null)
         return false;
      else
         return true;
   }

   /**
    * Writes the given data to the output stream
    */
   public boolean write(byte[] data, int length)
   {      
      try {
/*         
         for (int i = 0; i < length; i++) {
            if ((is_text == true) && (data[i] == 10))  // Line feed character
               stream.write(lineSeparator, 0, lineSeparator.length);
            else
               stream.write((int)data[i]);
         }
 */
         stream.write(data, 0, length);
         
         stream.flush();

         return true;
      }
      catch (IOException e) {
         return false;
      }
   }

   /**
    * Closes the file output stream
    */
   public void close()
   {
      try {
         stream.close();
      }
      catch (IOException e) {
      }
   }
}