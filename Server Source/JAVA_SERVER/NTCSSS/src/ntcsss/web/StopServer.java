/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.web;

import java.net.URL;
import java.net.HttpURLConnection;

import java.io.InputStream;
import java.io.FileNotFoundException;

/**
 * Sends an HTTP request to the web server on the local host which will
 *  indicate to it, that it needs to shut itself down
 */
public class StopServer
{
   public static void main(String args[])
   {
      URL url;
      HttpURLConnection connection;
      InputStream inputStream;
      int value;
      
      try {
         url = new URL("http", "localhost", 8081, "/?A=Exit+All+Servers");
         
         connection = (HttpURLConnection)url.openConnection();
         connection.setRequestProperty("Authorization", "Basic YWRtaW46YWRtaW4=");
         connection.connect();

         // For some reason the data must be read from the input stream
         //  even though we're not interested in it here
         inputStream = connection.getInputStream();         
         while ((value = inputStream.read()) != -1)
            System.out.print((char)value);
         inputStream.close(); 
 
         connection.disconnect();
      }  
      catch (FileNotFoundException fileNotFoundException) {
         // ignore this exception which always occurs
      }
      catch (Exception exception) {
         System.out.println(exception.toString());
      }
   }
}