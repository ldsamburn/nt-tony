/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.server;

import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.InterruptedIOException;

import java.util.LinkedList;

import java.lang.reflect.Method;
import java.lang.reflect.Constructor;

import ntcsss.libs.Constants;
import ntcsss.libs.SocketLib;

import ntcsss.log.Log;

/**
 * A class that listens for connections from a socket and performs
 *  a specific task based on the data that was received from the
 *  socket.
 */
public class GenericServer extends Thread
{
   private Listener listener;
   private SocketQueue queue;
   private ServerDescription serverDescription;
   private ServerState serverState;
   private int port;   
   private ActiveWorkers activeWorkers;
   private ActiveServers activeServers;

   /**
    * Constructor
    */
   public GenericServer(int port, ServerDescription serverDescription,
                        ActiveServers activeServers)
   {
      // Set up so that all threads kicked off from this thread will be logged
      //  as the given entity group
      super(serverDescription.getLogEntityGroup(), 
            serverDescription.getServerName());
      
      try {                  
         this.serverDescription = serverDescription;
         this.port = port;         
         this.activeServers = activeServers;

         // This will be used to indicate the state of this server
         serverState = new ServerState();
         
         // Create the data structure that will keep track of count of active 
         //  worker threads
         activeWorkers = new ActiveWorkers();

         // Create the queue the server socket listener will
         //  place received connections into
         queue = new SocketQueue();         
      }
      catch (Exception exception) {
         Log.excp("GenericServer", "GenericServer", exception.toString()); 
      }
   }

   /**
    * Starts the socket listener and the consumer that will process
    *  the data from the socket
    */
   public void run()
   {
      Socket socket;
      Worker worker;

      try {
         
         // Create and start the server socket listener          
         listener = new Listener(port, queue, serverDescription.getServerName());                  
         listener.start();

         // Process incoming connections until a "die" message is sent
         while (serverState.isDie() == false) {
           
            // If there is a socket in the queue, remove it and 
            //  process the data from it
            if (queue.isEmpty() == true) 
               sleep(100); // milliseconds
            else {
           
               // Remove the first connection from the queue
               socket = queue.removeFirst();               
           
               // Create a worker to process the data from the socket               
               worker = new Worker(socket, serverDescription, serverState, 
                                   activeWorkers);
               activeWorkers.increment();
               worker.start();
            }          
         }

         // Shutdown this server
         shutdown();
      }
      catch (Exception exception) {
         Log.excp("GenericServer", "run", exception.toString()); 
      }
      
      activeServers.decrement();
   }

   /**
    * Gracefully shuts down this server
    */
   private void shutdown()
   {
      Socket socket;
      Worker worker;
      Class serverClass;
      Method finalizationMethod;

      try {
         Log.info("GenericServer", "shutdown", "Shutting down " +
                             serverDescription.getServerName() + " ...");

         // Stop the listener to prevent any new connections
         listener.stopListener();

         // Process any connections that are left in the queue
         while (queue.isEmpty() == false) {
            socket = queue.removeFirst();

            // Create a worker to process the data from the socket            
            worker = new Worker(socket, serverDescription, serverState,
                                activeWorkers);
            activeWorkers.increment();
            worker.start();
         }

         // Wait for any outstanding threads that are processing message 
         //  requests to finish. NOTE: This will only wait for the specified 
         //  period of time (in seconds), at most, for them to finish.
         activeWorkers.waitForCompletion(2 * 60);
         
         // Run any code the server has included that is to be run
         //  when it shutsdown
         Log.debug("GenericServer", "shutdown", "Running finalization for " +
                             serverDescription.getServerName() + " ...");                  
         serverClass = serverDescription.getServerClass();
         finalizationMethod = serverClass.getMethod("runFinalization", null);                     
         finalizationMethod.invoke(null, null);  
      }
      catch (Exception exception) {
         Log.excp("GenericServer", "shutdown", exception.toString());
      }
   }      
}

/**
 * A queue of sockets
 */ 
class SocketQueue
{
   private LinkedList queue;

   /**
    * Constructor
    */
   public SocketQueue()
   {
      queue = new LinkedList();
   }

   /**
    * Removes the first item in the queue
    */
   public synchronized Socket removeFirst()
   {
      return (Socket)queue.removeFirst();
   }

   /**
    * Checks to see if the queue is empty
    */
   public synchronized boolean isEmpty()
   {
      return queue.isEmpty();
   }

   /**
    * Places a socket at the end of the queue
    */
   public synchronized void addLast(Socket socket)
   {
      queue.addLast(socket);
   }
}

/**
 * Listens for incoming socket connections and places received connections
 *  in a queue
 */
class Listener extends Thread
{
   private SocketQueue queue;
   private ServerSocket socketListener;

   /**
    * Constructor
    */
   public Listener(int port, SocketQueue queue, String serverName)
   {
      // Set the name of this thread
      super(serverName + " listener");
      
      try {
         this.queue = queue;

         setPriority(Thread.MAX_PRIORITY);

         socketListener = new ServerSocket(port, 
                                           Constants.SERVER_BACKLOG_QUEUE_SIZE);
      }
      catch (Exception exception) {
         Log.excp("Listener", "Listener", exception.toString());
      }
   }

   /**
    * This method starts the socket listener and will continue
    *  until the "stopListener" method is called
    */
   public void run()
   {
      try {

         // As soon as a connection is received, store it in the queue
         //  and return back to listening
         while (true)         
            queue.addLast(socketListener.accept());
      }
      catch (SocketException sktException) {
         //Log.info("Listener", "run", "Listener shutdown");
      }
      catch (Exception exception) {         
         Log.excp("Listener", "run", exception.toString());
      }
   }

   /**
    * Stops this listener
    */
   public void stopListener()
   {
      try {
         socketListener.close();
      }
      catch (Exception exception) {
         Log.excp("Listener", "stopListener", exception.toString());
      }
   }
}

/**
 * A class that reads data off a socket and performs a specific
 *  function based on the data it read
 */
class Worker extends Thread
{
   private Socket socket;
   private BufferedInputStream inputStream;
   private ServerDescription serverDescription;
   private ServerState serverState;
   private ActiveWorkers activeWorkers;

   /**
    * Constructor
    */      
   public Worker(Socket socket, ServerDescription serverDescription,
                 ServerState serverState, ActiveWorkers activeWorkers)
   {  
      // Set the name of this thread
      super(serverDescription.getServerName() + " worker");
      
      try {
         this.serverDescription = serverDescription;
         this.socket = socket;
         this.serverState = serverState;
         this.activeWorkers = activeWorkers;
         
         // Make this a "daemon" thread so that when the server is stopped, 
         //  it doesn't have to wait for any of these worker threads to finish 
         //  if they are still processing.
         setDaemon(true);

         // Create read stream from the socket
         inputStream = new BufferedInputStream(this.socket.getInputStream());
      }
      catch (Exception exception) {
         Log.excp("Worker", "Worker", exception.toString());
      }
   }        

   /**
    * Does the work of this thread
    */
   public void run()
   {
      String socketData;
      Method messageHandler;
      String header;    
      int nullCharIndex;
      String trimmedHeader;

      try {
                                            
         // Read the request off the socket
         socketData = readSocketData();         

         if (socketData != null) {
            
            // Get the header
            header = socketData.substring(0, Constants.CMD_LEN);
            if ((nullCharIndex = header.indexOf("\0")) != -1)
               trimmedHeader = header.substring(0, nullCharIndex).trim();
            else
               trimmedHeader = header.trim();
            
            Log.debug("Worker", "run", "Received <" + trimmedHeader + ">");
            
            // Look for the message handler for the message
            messageHandler = getMessageHandler(trimmedHeader);

            if (messageHandler != null) {  
            
               // Invoke the message handler
               invokeHandler(messageHandler,
                  socketData.substring(Constants.CMD_LEN, socketData.length()));               
            }
            else {   // i.e. Handler not found for message
               Log.error("Worker", "run", serverDescription.getServerName() + 
                          " handler not found for message <" + trimmedHeader + ">");               
            }
         }

         // Close the socket connection 
         inputStream.close();
         socket.close();
      }
      catch (Exception exception) {
         Log.excp("Worker", "run", exception.toString());
      }
      
      // Decrement the count of active worker threads
      activeWorkers.decrement();
   }

   /**
    * Reads the data off the socket and returns what it got
    */
   private String readSocketData()
   {
      byte buffer[];
      int bytesRead;

      try {
         
         // Create the buffer to store the data in
         buffer = new byte[Constants.CMD_MESSAGE_LEN + 1];

         // Set the timeout for the socket
         socket.setSoTimeout(Constants.SOCKET_READ_TIMEOUT);
         
         // Read the data from the socket         
         bytesRead = SocketLib.readStream(inputStream, buffer, 
                                          Constants.CMD_MESSAGE_LEN);

         if (bytesRead == -2) {
            Log.error("Worker", "readSocketData", "Received timeout during read");
            return null;
         }
         else if (bytesRead == -1) {
            Log.error("Worker", "readSocketData", "Error reading data");
            return null;
         }
         else
            return new String(buffer, 0, bytesRead);
      }
      catch (Exception exception) {
         Log.excp("Worker", "readSocketData", exception.toString());  
      }

      return null;
   }   

   /**
    * Looks up in the given set of message handlers for the handler of
    *  this message
    */
   private Method getMessageHandler(String header)
   {
      MessageHandlerDescription messageHandlers[];

      try {    
         messageHandlers = serverDescription.getMessageHandlers();

         for (int i = 0; i < messageHandlers.length; i++) {
            if (messageHandlers[i].getMessage().equals(header) == true) 
               return messageHandlers[i].getHandler();
         }
      }
      catch (Exception exception) {
         Log.excp("Worker", "getMessageHandler", exception.toString());                            
      }

      return null;
   }

   /**
    * Invokes the given message handler
    */
   private void invokeHandler(Method messageHandler, String messageData)
   {
      Class serverClass;
      ServerBase serverInstance;
      Object arguments[];
      Class parameters[];
      Constructor constructor;

      try {
         
         // Create an instance of the specific server
         serverClass = messageHandler.getDeclaringClass();

         parameters = new Class[3];
         parameters[0] = String.class;
         parameters[1] = Socket.class;
         parameters[2] = ServerState.class;
         constructor = serverClass.getConstructor(parameters);
    
         arguments = new Object[3];
         arguments[0] = messageData;
         arguments[1] = socket;
         arguments[2] = serverState;
         serverInstance = (ServerBase)constructor.newInstance(arguments);

         // Invoke the message handler
         messageHandler.invoke(serverInstance, null);
         
         // Make sure to clean up any resources used by the server instance
         serverInstance.cleanup();
      }
      catch (Exception exception) {
         Log.excp("Worker", "invokeHandler", exception.toString());                            
      }
   }
}

/**
 * A data structure that keeps track of the number of requests being processed
 *  by the server
 */
class ActiveWorkers
{    
   private int activeCount;
   
   /**
    * Constructor
    */
   public ActiveWorkers()
   {
      activeCount = 0;
   }
   
   /**
    * Increments the active worker count by one
    */
   public synchronized void increment()
   {      
      activeCount++;
   }
   
   /**
    * Decrements the active worker count by one
    */
   public synchronized void decrement()
   {      
      activeCount--;
      
      // If the active count is now zero, notify anyone who may be waiting
      //  for this state
      if (activeCount == 0)
         notify();
   }
      
   /**
    * This method will cause the calling thread to wait until all the 
    *  requests have been processed or the given timeout period (in seconds)
    *  has elapsed.  
    *
    * NOTE: This method assumes that the "increment" method will not be called 
    *       any more.
    */
   public synchronized void waitForCompletion(int timeout)
   {
      try {
         
         // Only need to wait if the active count is greater than 0
         if (activeCount > 0) {
            
            Log.info("ActiveWorkers", "waitForCompletion", "Waiting for <" + 
                     activeCount + "> request(s) to finish ...");
            
            // "Sleep" for the specified period of time (in milliseconds)
            wait((long)(timeout * 1000));  
            
            // Check to see if all the requests have been processed
            if (activeCount > 0) 
               Log.warn("ActiveWorkers", "waitForCompletion", "<" + activeCount + 
                        "> request(s) still processing");                           
         } 
                  
      }
      catch (Exception exception) {
         Log.excp("ActiveWorkers", "waitForCompletion", exception.toString());                           
      }
   }      
}