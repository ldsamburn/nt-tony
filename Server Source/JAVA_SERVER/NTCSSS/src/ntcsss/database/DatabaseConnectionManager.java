/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.database;

import java.sql.DriverManager;
import java.sql.Connection;
import java.sql.SQLException;

import java.util.Stack;

import ntcsss.libs.Constants;
import ntcsss.log.Log;

/**
 * Manages connections to the database
 */
public class DatabaseConnectionManager
{
   private static Stack connectionPool;   
   private static Object lock = new Object();

   /**
    * Performs some initial tasks that must be done before connections can
    *  be made
    */
   static
   {
      try {

         // Load the database driver
         Class.forName(Constants.DB_JDBC_DRIVER).newInstance();

/*
         // Create a pool of database connections
         synchronized (lock) {

            connectionPool = new Stack();

            for (int i = 0; i < Constants.DB_CONNECTION_POOL_SIZE; i++) 
               connectionPool.push(DriverManager.getConnection(Constants.DB_URL, 
                                                             Constants.DB_USER, 
                                                             Constants.DB_PASSWORD));            
         }
*/         
      }
      catch (Exception exception) {
         Log.excp("DatabaseConnectionManager", "static initializer", 
                   exception.toString());         
      }            
   }
   
   /**
    * Returns true if the database engine is running. This does NOT check
    *  to see if the NTCSS database is an instance in the engine.
    */
   public static boolean isEngineRunning()
   {            
      Connection testConnection;
      String exceptionStr = null;
      final int sleepTime = 5000; // milliseconds
      
      // At system boot time, it is necessary to allow the database engine
      //  enough time to startup before the server daemons are started. So if
      //  a database connection can't be made, pause for a period of time and 
      //  then try to connect again.
      for (int attempt = 0; attempt < Constants.DB_CONNECTION_ATTEMPTS; attempt++) {
         
         // Sleep for a period 
         if (attempt > 0) {
            
            try {
               Log.warn("DatabaseConnectionManager", "isEngineRunning", 
                        "Couldn't obtain connection on attempt " + (attempt + 1) + " of " + 
                        Constants.DB_CONNECTION_ATTEMPTS + ". Sleeping...");
               Thread.sleep(sleepTime);
            }
            catch (Exception exp) {
               Log.excp("DatabaseConnectionManager", "isEngineRunning", 
                        "Exception waiting for database <" + 
                        exp.toString() + ">"); 
               return false;
            }
         }
         
         // Try to get a database connection as a test to see if the engine
         //  is up
         try {                        
            testConnection = DriverManager.getConnection(Constants.DB_URL, 
                                   Constants.DB_USER, Constants.DB_PASSWORD);                                   
            testConnection.close();
                                   
            return true;                                  
         }
         catch (Exception exception) {    
            
            // The database must not be up yet
            exceptionStr = exception.toString();                        
         }
      }
      
      // If control gets here, then a connection must not have been made
      Log.excp("DatabaseConnectionManager", "isEngineRunning", exceptionStr); 
      
      return false;
   }

   /**
    * Returns a connection to the database
    */
   public static Connection getConnection()
   {
/*      
      try {

         // Find a free connection in the pool of connections
         while (true) {

            synchronized (lock) {

               if (connectionPool.empty() == false)
                  return (Connection)connectionPool.pop();
            }  
 
            Log.info("DatabaseConnectionManager", "getConnection", 
                     "Waiting for free connection...");
            Thread.sleep(15); // milliseconds          
         }
      }
      catch (Exception exception) {
         Log.excp("DatabaseConnectionManager", "getConnection",
                             exception.toString());

         return null;
      }
*/
      
    // It is possible (especially on system startup) that a connection maybe
    //  be requested before the database engine has had a chance to start up.
    //  If a connection attempt fails, attempt to connect a few more times,
    //  sleeping between each attempt to allow the engine enough time to come
    //  up.
    for (int i = 0; i < Constants.DB_CONNECTION_ATTEMPTS; i++) {
         
         try {
            return DriverManager.getConnection(Constants.DB_URL, Constants.DB_USER, 
                                               Constants.DB_PASSWORD);
         }
         catch (SQLException sqlException) {
            
            if (sqlException.getErrorCode() == 0) { // Connection refused
               
               try {
                  Thread.sleep(5000); // in milliseconds              
               }
               catch (InterruptedException interrupted) {
                  Log.excp("DatabaseConnectionManager", "getConnection",
                             interrupted.toString());
                  return null;
               } 
            }
            else {
               Log.excp("DatabaseConnectionManager", "getConnection",
                             sqlException.toString());
               return null;
            }
         }
         catch (Exception exception) {
            Log.excp("DatabaseConnectionManager", "getConnection",
                             exception.toString());
            return null;
         }
      }  // end "for" loop
    
      Log.error("DatabaseConnectionManager", "getConnection", 
                "Could not connect to database");
      return null;
   }

   /**
    * Frees up the given database connection
    */
   public static void releaseConnection(Connection connection)
   {
      try {

connection.close();

/*
         // Add the connection back to the pool of connections
         synchronized (lock) {
            connection.commit();
            connectionPool.push(connection);
         }
*/
      }
      catch (Exception exception) {
         Log.excp("DatabaseConnectionManager", "releaseConnection",
                             exception.toString());
      }
   }
}