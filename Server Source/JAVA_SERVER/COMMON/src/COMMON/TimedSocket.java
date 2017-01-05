/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package COMMON;

import java.net.Socket;

import java.io.InterruptedIOException;
import java.io.IOException;

/**
 * Provides a method for obtaining a socket connection within a specified 
 *  time period.
 */
public class TimedSocket
{
   
   /**
    * Attempts to connect to the given host and port within the timeout period
    *  (in milliseconds). If an error occurs, or the timeout period expires, 
    *  an exception is thrown. Otherwise, the socket connection is returned.
    */
   public static Socket getSocket(String host, int port, long timeout)
        throws InterruptedIOException, IOException
   {      
      ConnectingAgent agent;      
     
      // Create an agent which will try to connect to the given host and port
      agent = new ConnectingAgent(host, port);

      // Sleep for the specified timeout period, or until the agent calls 
      //  its notify method
      try {
         agent.doWait(timeout);   
      }
      catch (InterruptedException interruptedExcp) {         
      }

      // The agent will call its notify method if an exception occurs or it
      //  was able to make a connection. So check if either of these conditions 
      //  were the case. If neither are the case, then the "wait" must have 
      //  timed out.
      if (agent.connectionMade() == true)         
         return agent.getSocket();
      else if (agent.exceptionOccurred() == true) 
         throw agent.getException();      
      else  // Timeout occurred
         throw new InterruptedIOException("Could not connect to <" + host + 
                        ":" + port + "> within the given timeout period <" + 
                        timeout + ">");
      
      // NOTE: It is possible that connecting to a host takes longer than
      //  the specified timeout. Or if the host is down, it will take a 
      //  considerable amount of time for a connection error to be reported.      
      //  There unfortunatly is no graceful, recommended way to stop a thread. 
      //  So even though this method has returned in these situation, the agent 
      //  thread (which can't be stopped) may still be trying to connect, but 
      //  will eventually die on its own.  
      //
      //  This should not cause any issues, but it needed to be noted.
   }
}

/**
 * A thread that attempts to connect to a given host and port
 */
class ConnectingAgent extends Thread
{
   private String host;
   private int port;
   private Socket socket;   
   private IOException exception;
   private boolean notified;
   private boolean timeoutOccurred;

   public ConnectingAgent(String host, int port)
   {
      this.host = host;
      this.port = port;      
            
      socket    = null;
      exception = null;
      notified  = false;      
         
      // Create as a daemon thread so that if the parent thread who started
      //  this thread dies, then this thread will also die.
      setDaemon(true);

      start();
   }

   public void run()
   {
      try {
         
         // Try to connect to the given host and port
         setSocket(new Socket(host, port));         
      }
      catch (IOException exception) {                       
         saveException(exception);         
      }             
      
      doNotify();
      
      // NOTE: There is the possiblity that it could take a long time to 
      //  connect to a host (longer than the timeout value specified). If that
      //  is ever the case, this ConnectingAgent thread will continue past
      //  the timeout to try and connect. Although the getSocket method will
      //  have already returned an exception to it's caller, there will still
      //  be an open socket left here!
   }
   
   public synchronized boolean connectionMade()
   {
      return ((socket != null) ? true : false);
   }
   
   public synchronized void setSocket(Socket skt)
   {
      socket = skt;
   }
      
   public synchronized Socket getSocket()
   {
      return socket;
   }
   
   public synchronized boolean exceptionOccurred()
   {      
      return ((exception != null) ? true : false);
   }
   
   private synchronized void saveException(IOException excp)
   {
      exception = excp;
   }
   
   public synchronized IOException getException()
   {
      return exception;
   }
   
   /**
    * This method will cause the calling thread to wait until the "notify"
    *  method of this class is called or the given timeout period 
    *  (in milliseconds) has elapsed. 
    */
   public synchronized void doWait(long timeout) throws InterruptedException
   {      
      // Only need to "wait" if the notify method has not been called yet         
      if (notified == false)                         
         wait(timeout);                                                 
   }
   
   /**  "Awakens" the thread which has been waiting for this event to happen,
    *    if it is still waiting.
    */
   private synchronized void doNotify()
   {            
      notified = true;
      notify();                             
   } 
}

