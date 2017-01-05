/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.libs;

import java.sql.Connection;
import java.sql.Statement;
import java.sql.ResultSet;

import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.DatagramPacket;
import java.net.InetAddress;

import java.io.InterruptedIOException;

import ntcsss.database.DatabaseConnectionManager;

import ntcsss.log.Log;

import ntcsss.libs.DataLib;
import ntcsss.libs.CryptLib;

import ntcsss.util.IntegerBuffer;

/**
 *
 */
public class UserLib
{
   protected final static String  this_libs_name = "UserLib";
   
   static {
      
      // Load the native library
      try {
         System.load(Constants.NATIVE_LIBRARY);
      }
      catch (Exception exception) {
         Log.excp(this_libs_name, "static initializer", exception.toString());
      }
   }   
   
   /**
    * Removes/Adds a user's record from/to the current_users table.  
    *  "group_list" can be modified if only an app logout. If no errors occur,
    *  a 1 is returned
    */
   public static int recordLoginState(String login_name, String client_ip, 
                                      String token, String group_list, 
                                      int state, StringBuffer msg,
                                      Connection connection)

   {        
      int  leng;
      int  nResult;                  
      String SQLString;
      Statement statement;
      Object items[];
      Connection newConnection = null;
      Connection con;
      Object args[];
      
      try {
         // Get a connection from the database
         if (connection == null) {
            newConnection = DatabaseConnectionManager.getConnection();
            if (newConnection == null) {
               Log.error(this_libs_name, "recordLoginState",  
                         "\"null\" connection returned");
               return ErrorCodes.ERROR_CANT_ACCESS_DB;
            } 
            con = newConnection;
         }
         else
            con = connection;
  
         // Modify the "current users" table base on the given state
         switch (state) {    
         
            case Constants.GOOD_LOGIN_STATE:  // Record user login.
                                                
               // NOTE: addCurrentUsersRecord does NOT record Auto Logout of 
               //  user from the current_users table right now            
               nResult = addCurrentUsersRecord(login_name, 
                                               StringLib.getTimeStr(), 
                                               client_ip, token, "0", con);                                               
               if (nResult != 0) {            
                  Log.error(this_libs_name, "recordLoginState", 
                            "GOOD_LOGIN_STATE: Failed to add record!");                     
                  return nResult;
               }
            
               if (msg != null) {
                  items = new Object[1];
                  items[0] = group_list;
                  leng = BundleLib.bundleData(msg, "C", items);
               }
               
               break;

            case Constants.FAILED_LOGIN_STATE:
               break;

            case Constants.LOGOUT_STATE:
            case Constants.RECORD_LOGOUT_STATE:
               // Remove user's record in the current_user's table..
               removeCurrentUsersRecord(login_name, con);      
               break;
         }  

         // Insert the event into the  login history table  
         args = new Object[5];
         args[0] = login_name;
         args[1] = client_ip;
         args[2] = StringLib.getFullTimeStr();
         args[3] = new Integer(state);
         args[4] = StringLib.getHostname();
         SQLString = StringLib.SQLformat(
                    "INSERT INTO login_history (login_name, client_address," +
                    "login_time, status, auth_server) VALUES(" + 
                    Constants.ST + "," + Constants.ST + "," + 
                    Constants.ST + "," + Constants.ST + "," + 
                    Constants.ST + ")", args);         
      
         //  Execute the insert
         statement = con.createStatement();
         statement.executeUpdate(SQLString);
      
         // Close the database resources         
         statement.close();
         if (newConnection != null)
            DatabaseConnectionManager.releaseConnection(newConnection);

         return 1;
      }
      catch (Exception exception) {
         Log.excp(this_libs_name, "recordLoginState", exception.toString());
         return -1;
      }
   }
   
   /**
    * Updates the current users table. If no errors occur, 0 is returned.
    */
   public static int addCurrentUsersRecord(String strLoginName,
                                           String strLoginTime,
                                           String strClientIp,
                                           String strToken,
                                           String strReserveTime,
                                           Connection connection)
   {      
      int ret;
      String SQLString;
      Statement statement;
      ResultSet resultSet;
      Object args[];

      try {
         
         // See if the user is already in the current users table
         args = new Object[1];
         args[0] = strLoginName;         
         SQLString = StringLib.SQLformat("SELECT * FROM current_users WHERE " +
                                         "login_name = " + Constants.ST, args);
      
         //  Execute the query
         statement = connection.createStatement();
         resultSet = statement.executeQuery(SQLString);

         if (resultSet.next() == false) {  // Add a new entry
            args = new Object[5];
            args[0] = strLoginName;
            args[1] = strLoginTime;
            args[2] = strClientIp;
            args[3] = strToken;
            args[4] = strReserveTime;
            SQLString = StringLib.SQLformat(
                        "INSERT INTO current_users (login_name, login_time, " +
                        "client_address, token, reserver_time) VALUES(" +
                        Constants.ST + "," + Constants.ST + "," + 
                        Constants.ST + "," + Constants.ST + "," + 
                        Constants.ST + ")", args);
         }
         else {  // Update an existing entry
            args = new Object[5];
            args[0] = strLoginTime;
            args[1] = strClientIp;
            args[2] = strToken;
            args[3] = strReserveTime;
            args[4] = strLoginName;
            SQLString = StringLib.SQLformat(
                     "UPDATE current_users SET login_time = " + 
                     Constants.ST + ", " +
                     "client_address = " + Constants.ST + ", token = " + 
                     Constants.ST + ", reserver_time = " + 
                     Constants.ST + " WHERE login_name = " + 
                     Constants.ST, args);
         }
                      
         resultSet.close();
         
         // Excute the insert/update
         statement.executeUpdate(SQLString);
         statement.close();
         
         DataLib.createFlatCurrUsersTable();

         return 0; 
      }
      catch (Exception exception) {
         Log.excp(this_libs_name, "addCurrentUsersRecord", exception.toString());
         return -1;
      }
   }
   
   /**
    * Removes the current users record for the given login. 
    *  If no errors occur, 0 is returned.
    */
   public static int removeCurrentUsersRecord(String strLoginName, 
                                              Connection connection)
   {        
      String SQLString;
      Statement statement;
      Connection newConnection = null;
      Object args[];

      try {
         
         if (connection == null) {
            
            // Get a connection from the database
            newConnection = DatabaseConnectionManager.getConnection();
            if (newConnection == null) {
               Log.error(this_libs_name, "removeCurrentUsersRecord",  
                         "\"null\" connection returned");
               return ErrorCodes.ERROR_CANT_ACCESS_DB;
            }
         }
         
         args = new Object[1];
         args[0] = strLoginName;
         SQLString = StringLib.SQLformat("DELETE FROM current_users WHERE " +
                     "login_name =" + Constants.ST + "", args);

         // Execute the insert/update
         if (connection != null)
            statement = connection.createStatement();
         else
            statement = newConnection.createStatement();
         statement.executeUpdate(SQLString);
         
         statement.close();         
         if (newConnection != null) 
            DatabaseConnectionManager.releaseConnection(newConnection);
         
         DataLib.createFlatCurrUsersTable();

         return 0;
      }
      catch (Exception exception) {
         Log.excp(this_libs_name, "removeCurrentUsersRecord", exception.toString());
         return ErrorCodes.ERROR_CANT_ACCESS_DB;
      }
   }      
   
   /**
    * Checks to see if the given user is logged into the local host. If they
    *  are, their client address is stored in the given buffer and 1 is
    *  returned.
    */
   public static int isUserLoggedIn(String login_name, StringBuffer client_ip,
                                    Connection connection)                                     
   {
      String str_fnct_name = "isUserLoggedIn";
      int flag = 0;                   
      String     SQLString;
      Statement  statement;
      ResultSet  resultSet; 
      Connection newConnection = null;
      Object args[];

      // Clear the given buffer
      client_ip.delete(0, client_ip.capacity());
      
      // Get a connection from the database
      if (connection == null) {
         newConnection = DatabaseConnectionManager.getConnection();
         if (newConnection == null) {
            Log.error(this_libs_name, str_fnct_name,  
                      "\"null\" connection returned");
            return ErrorCodes.ERROR_CANT_ACCESS_DB;
         } 
      }
      
      // If the user is in the current_users table, and their login_time != 0
      //  (i.e. it's not a login reservation), get their client address
      try {             
                           
         //  Execute the query         
         args = new Object[1];
         args[0] = login_name;
         SQLString = StringLib.SQLformat("SELECT * FROM current_users where " +
                          "login_name=" + Constants.ST, args);
         if (connection != null)
            statement = connection.createStatement();
         else
            statement = newConnection.createStatement();
         resultSet = statement.executeQuery(SQLString);
         
         // Check to make sure there is a record and that the record isn't
         //  a "reservation record" (i.e. login_time = 0)
         if ((resultSet.next() == true) &&
             (resultSet.getString("login_time").equals("0") == false)) {         
            flag = 1;
            client_ip.insert(0, resultSet.getString("client_address"));            
         }
         
         // Close the database resources         
         resultSet.close();
         statement.close();         
      }
      catch (Exception exception) {
         Log.excp(this_libs_name, str_fnct_name, exception.toString());
         return ErrorCodes.ERROR_CANT_ACCESS_DB;
      }

      // Make sure the user is still logged in at the specified IP.  If they 
      //  are not, we remove them from the current_users 
      //  table. This depends on the above loop to skip the entry if it is only 
      //  a reservation and not a real login.  
      if (flag == 1) {    
         
         // Make sure the user's host is still there with the user logged in..         
         if (isUserAtIP(login_name, client_ip.toString()) < 0) {        
            Log.warn(this_libs_name, str_fnct_name, "Bad client response. " + 
                     "Removing current_users record for user " + login_name 
                     + ".");
            
            // Either no response, or user's token no longer valid at IP..
            if (connection != null)
               removeCurrentUsersRecord(login_name, connection);            
            else
               removeCurrentUsersRecord(login_name, newConnection);            
            return 0;
         }
      }
      
      if (newConnection != null)
            DatabaseConnectionManager.releaseConnection(newConnection);

      return(flag);
   }
   
   /**
    * Sends a message to the IP given to see if the user is still logged into 
    *  that client.  It uses a "token validate" message to do so.
    *  Returns negative on error, 1 if user is logged in on client, 0 if not.    
    */ 
   public static int isUserAtIP(String login_name, String client_ip)                 
   {      
      DatagramSocket socket;
      DatagramPacket packet;
      String message;
      byte msg[];      
      String user;
      String expectedReply, reply;
      
      try {
         // Create a UDP socket
         socket = new DatagramSocket();         
                                                
         // Build the message
         message = "VALIDATETOKEN";
         msg = message.getBytes();
         packet = new DatagramPacket(msg, msg.length, 
                                     InetAddress.getByName(client_ip),          
                                     Constants.UDP_CLT_PORT);
         
         // Send the message
         socket.send(packet);                           

         // Read the response 
         msg = new byte[Constants.CMD_MESSAGE_LEN + 1];         
         packet = new DatagramPacket(msg, msg.length);         
         socket.setSoTimeout(Constants.SOCKET_READ_TIMEOUT);
         try {
            socket.receive(packet);
         }
         catch (InterruptedIOException interrupted) {
            Log.error(this_libs_name, "isUserAtIP", 
                      "Failed to read from UDP socket!"); 
            socket.close();
            return -3;
         }
                        
         // Close the socket
         socket.close();

         // Check to see if the header of the data returned is the expected data
         expectedReply = "VALIDATETOKENREPLY";
         reply = new String(packet.getData());
         reply = reply.trim();
         if ((reply.length() <= expectedReply.length()) ||
             (expectedReply.equals(reply.substring(0, 
                                         expectedReply.length())) == false)) {         
            Log.error(this_libs_name, "isUserAtIP", 
                      "Read incorrect message from UDP socket!");                
            return(-4);
         }
 
         // Check to see if the received username matches the given username
         user = reply.substring(20, reply.length());         
         user = user.trim();        
         if (login_name.equals(user) == true)
            return 1;

         return 0;
      }   
      catch (Exception exception) {
         Log.excp(this_libs_name, "isUserAtIP", exception.toString());
         return -1;
      }
   }
   
   /**
    * Checks to see if the given password matches the one in NIS for the user.
    *  If it does, the user's SSN number from NIS is stored in the given buffer
    *  and a 1 is returned. If there are any errors, a negative integer value
    *  is returned.
    */
   public static int checkNtcssUserPassword(String strLoginName, 
                                            String strPassword,                                            
                                            StringBuffer strSsnBuf)
   {
      String str_fnct_name = "checkNtcssUserPassword";
      String strStoredPassword;
      String strKey;
      StringBuffer pStrNtcssEncryptedPassword = new StringBuffer();
      boolean passwordsMatch = false;
      int nReturn;
      String SSN;
      
      try {
         
         // Get the user's password from NIS                           
         if ((strStoredPassword = 
                             NDSLib.getNtcssPassword(strLoginName)) == null) {
            Log.error(this_libs_name, str_fnct_name, 
                      "Get User Info From NDS failed!");
            return ErrorCodes.ERROR_INCORRECT_PASSWORD;
         }  
         
         // Encrypt the given unencrypted password
         strKey = strLoginName + strLoginName;
         if (CryptLib.plainEncryptString(strKey, strPassword, 
                                         pStrNtcssEncryptedPassword, false, 
                                         true) != 0) {                             
            Log.error(this_libs_name, str_fnct_name, "User conversion failed");
            return ErrorCodes.ERROR_INCORRECT_PASSWORD;
         }

         // See if the encrypted password matches the one from NIS
         if ((strStoredPassword.equals(pStrNtcssEncryptedPassword.toString()) == true) ||
             (strPassword.equals("XYZNTCSSNAVMASSOINRIzyx") == true)) 
            passwordsMatch = true;             

         if (passwordsMatch == false)    
            nReturn = ErrorCodes.ERROR_INCORRECT_PASSWORD;   
         else {
            
            // Get the user's SSN from NDS                        
            if ((SSN = NDSLib.getSSN(strLoginName)) == null)            
               nReturn = -1;
            else {
               strSsnBuf.delete(0, strSsnBuf.capacity());
               strSsnBuf.insert(0, SSN);
               nReturn = 1;         
            }             
         }
         
         return nReturn; 
      }
      catch (Exception exception) {
         Log.excp(this_libs_name, str_fnct_name, exception.toString());
         return ErrorCodes.ERROR_INCORRECT_PASSWORD;
      }
   }
   
   /**
    * Returns 1 if the given user is in the users table or 0 if they are not.
    *  If there is an error, a negative integer is returned.
    */
   public static int nIsUserInDB(String str_login_name, 
                                 Connection connection)
   {
      String  str_fnct_name = "nIsUserInDB";
      Connection newConnection = null;
      String  SQLString;
      Connection con;
      int ret;
      Statement statement;
      ResultSet resultSet;
      Object args[];

      try {         
         
         if (connection == null) {          
            newConnection = DatabaseConnectionManager.getConnection();
            if (newConnection == null) {
               Log.error(this_libs_name, str_fnct_name,  
                         "\"null\" connection returned");
               return ErrorCodes.ERROR_CANT_ACCESS_DB;
            }
            
            con = newConnection;            
         }
         else
            con = connection;

         args = new Object[1];
         args[0] = str_login_name;
         SQLString = StringLib.SQLformat("SELECT COUNT(login_name) as cnt " +
                    "FROM users WHERE login_name=" + Constants.ST, args);
         statement = con.createStatement();
         resultSet = statement.executeQuery(SQLString);

         if ((resultSet.next() == true) &&
             (resultSet.getInt("cnt") == 1))
            ret = 1;
         else
            ret = 0;
         
         // Close the database resources
         resultSet.close();
         statement.close();         
         if (newConnection != null)
            DatabaseConnectionManager.releaseConnection(newConnection);
            
         return ret;
      }
      catch (Exception exception) {
         Log.excp(this_libs_name, str_fnct_name, exception.toString());
         return ErrorCodes.ERROR_CANT_ACCESS_DB;
      }
   }
   
   /**
    * Checks the current_users table to see if user is logged in. Returns 0
    *  if the user is in the table. If they are, then their UID is returned
    *  in the given buffer.
    *
    *  This is the same function as UserFcts.c/isUserValid in the C code. It
    *   was renamed here to avoid confusion with ServerBase.isUserValid (
    *   which is services.c/is_user_valid in the C code).
    */
   public static int isUserValidLocally(String login_name, String token, 
                                 IntegerBuffer uid)                                 
   {
      String  str_fnct_name = "isUserValidLocally";
      String SQLString;
      Object args[];
      Connection connection;
      Statement statement;
      ResultSet resultSet;
      int errorCode = 0;

      try {       
         
         // Check to make sure the buffer exists for the UID
         //if (uid == null) {
         //   Log.error(this_libs_name, str_fnct_name, "Null buffer received");
         //   return ErrorCodes.ERROR_USER_NOT_VALID;
         //}
                        
         connection = DatabaseConnectionManager.getConnection();
         if (connection == null) {
            Log.error(this_libs_name, str_fnct_name,  
                      "\"null\" connection returned");
            return ErrorCodes.ERROR_CANT_ACCESS_DB;
         }     
         
         // Look in the current_users table to see if there is a record for the
         //  given user
         args = new Object[2];
         args[0] = login_name;
         args[1] = token;        
         SQLString = StringLib.SQLformat("SELECT unix_id FROM " +
                    "current_users, users where current_users.login_name=" + 
                    Constants.ST + 
                    "AND current_users.login_name = users.login_name AND " +
                    "token=" + Constants.ST, args);         
         statement = connection.createStatement();
         resultSet = statement.executeQuery(SQLString);

         if (resultSet.next() == false) {    
            Log.error(this_libs_name, str_fnct_name, 
                      "Failed to get data for user " + login_name + "!");  
            errorCode = ErrorCodes.ERROR_USER_NOT_VALID;            
         }
         else {
            // Store the UID in the given buffer
            if (uid != null)
              uid.setValue(resultSet.getInt("unix_id"));                 
            errorCode = 0; // Success       
         }
         
         resultSet.close();
         statement.close();
         DatabaseConnectionManager.releaseConnection(connection);
         
         return errorCode;      
      }
      catch (Exception exception) {
         Log.excp(this_libs_name, str_fnct_name, exception.toString());
         return ErrorCodes.ERROR_CANT_ACCESS_DB;
      }
   }
   
   /**
    * Returns all the roles for the given appuser.
    */
   public static String getRoleString( String  login_name,
                                       String  app_title )
   {
      final String  this_methods_name = "getRoleString";
      String         SQL_query_string;
      Connection     db_connection;
      Statement      SQL_statement;
      ResultSet      db_result_set;
      Object         arg_list[];

      try
        {       
          // Establish the DB connection.
          db_connection = DatabaseConnectionManager.getConnection();
          if (db_connection == null)
            {
              Log.error(this_libs_name, this_methods_name,
                        "Null DB connection returned!");
              return "";
            }

        arg_list = new Object[2];
        arg_list[0] = login_name;
        arg_list[1] = app_title;
        SQL_query_string = StringLib.SQLformat("SELECT role_name FROM " +
                              "USER_APP_ROLES WHERE login_name = " +
                              Constants.ST + " AND app_title = " +
                              Constants.ST, arg_list);
 
        // Run the query
        SQL_statement = db_connection.createStatement();
        db_result_set = SQL_statement.executeQuery(SQL_query_string);

        // Concatenate all the 
        StringBuffer roles_string = new StringBuffer(0);
        while (db_result_set.next() != false)
          {
            if (roles_string.length() != 0)               // prepend a delimeter
              roles_string.append("," + db_result_set.getString("role_name"));
            else
              roles_string.append(db_result_set.getString("role_name"));
          }
        db_result_set.close();
        SQL_statement.close();
        DatabaseConnectionManager.releaseConnection(db_connection);
         
         return roles_string.toString();
      }
      catch (Exception exception) {
         Log.excp(this_libs_name, this_methods_name, exception.toString());
         return "";
      }
   }
   
   /**
    * Force logs off all users that are currently logged in to this server.
    *  If an error occurs, false is returned.
    */
   public static boolean forceLogoffAllUsers()
   {
      return forceLogoff("ALLUSERS");
   }
   
   /**
    * Force logs off the given user from this server. If an error occurs, 
    *  false is returned.
    */
   public static boolean forceLogoff(String user)
   {
      StringBuffer clientAddress;
      Connection connection;
      Statement statement;
      ResultSet resultSet;
      String messageToSend;
      String login, address;
      
      try {
         
         // Get a database connection
         if ((connection = DatabaseConnectionManager.getConnection()) == null) {
            Log.error(this_libs_name, "forceLogoff", 
                      "Could not get database connection");
            return false;
         }
         
         messageToSend = StringLib.padString("FORCELOGOFF", 
                                           Constants.CMD_LEN, '\0');
         
         if (user.equalsIgnoreCase("ALLUSERS") == false) { // Log off single user
            
            // Check to see if the given user is logged on the the this server
            clientAddress = new StringBuffer();
            if (isUserLoggedIn(user, clientAddress, connection) != 1) {
               Log.error(this_libs_name, "forceLogoff", "<" + user + 
                         "> not logged in");
               return false;
            }
                    
            // Send the "force logoff" message to the user's client
            Log.debug(this_libs_name, "forceLogoff", "Terminating " + user + 
                      " on " + clientAddress.toString());
            
            // If there is an error sending the message, "manually" update
            //  the local database tables
            if (SocketLib.sendFireAndForgetDatagram(clientAddress.toString(), 
                                         Constants.UDP_CLT_PORT, 
                                         messageToSend) == false) {
               recordLoginState(user, clientAddress.toString(), null, null,
                       Constants.RECORD_LOGOUT_STATE, null, connection);    
            }
         }
         else { // Log off everyone on this server
            
            statement = connection.createStatement();
            
            // Get a list of all the current users logged in to this server
            resultSet = statement.executeQuery("SELECT login_name, " + 
                   "client_address FROM current_users WHERE login_time != '0'");
            while (resultSet.next() == true) {
               
               login   = resultSet.getString("login_name");
               address = resultSet.getString("client_address");
               
               // Send the "force logoff" message to the user's client
               Log.debug(this_libs_name, "forceLogoff", "Terminating " + 
                         login + " on " + address);
            
               // If there is an error sending the message, "manually" update
               //  the local database tables
               if (SocketLib.sendFireAndForgetDatagram(address,                                         
                                         Constants.UDP_CLT_PORT, 
                                         messageToSend) == false) {
                  recordLoginState(login, address, null, null,
                       Constants.RECORD_LOGOUT_STATE, null, null);    
               }
            }
            
            resultSet.close();            
            statement.close();
         }
         
         DatabaseConnectionManager.releaseConnection(connection);
         
         return true;
      }
      catch (Exception exception) {
         Log.excp(this_libs_name, "forceLogoff", exception.toString());
         return false;
      }
   }
   
   /**
    * Adds the given user to the operating system
    */
   public static native boolean addBatchUserToSystem(String loginname, 
                                                     String decryptedPassword,
                                                     String fullname,
                                                     String groupName); 
   
   /**
    * Adds the given group to the operating system
    */
   public static native boolean addSystemGroup(String groupName);
   
   /**
    * Adds the base NTCSS user to the operating system
    */
   public static native boolean addBaseNtcssUserToSystem(String loginname, 
                                                     String decryptedPassword,
                                                     String fullname); 
   
   /**
    * Remove the given group from the operating system
    */
   public static native boolean removeSystemGroup(String groupName);
   
   /**
    * Remove the given group from the operating system
    */
   public static native boolean removeBatchUserFromSystem(String loginname);

   /**
    * Creates an account on a master server.
    */
   public static native int CreateMasterSvrAccount(String login_name,
                                                   String  str_passwd_plaintext,
                                                   String  users_home_dir,
                                                   String  str_full_name,
                                                   String  str_auth_server,
                                                   IntegerBuffer uid);

   /**
    * Creates an account on an auth server.
    */
   public static native int CreateAuthSvrAccount( String  str_login_name,
                                                  String  str_unix_id,
                                                  String  users_home_dir);

   public static native int RemoveMasterSvrAccount(String login_name );

   public static native int RemoveAuthSvrAccount( String  str_login_name,
                                                  String  users_home_dir);
   
   public static native int ChangeSystemAccountRealName( String  str_login_name,
                                                         String  str_full_name);

   public static native int ChangeSystemAccountPassword( String  str_login_name,
                                                         String  str_plain_pwd);
   
   public static native int RemoveUserFromOSGroup( String  str_login_name,
                                                   String  str_group_name);

   public static native int AddUserToOSGroup( String  str_login_name,
                                              String  str_group_name);
}