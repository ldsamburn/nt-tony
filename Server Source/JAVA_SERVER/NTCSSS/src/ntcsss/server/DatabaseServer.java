/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/
package ntcsss.server;

import java.net.Socket;

import java.sql.Connection;
import java.sql.Statement;
import java.sql.ResultSet;

import java.util.StringTokenizer;
import java.util.Vector;

import java.lang.Runtime;
import java.lang.Character;

import java.util.List;

import COMMON.INI;
import COMMON.FileString;
import java.io.BufferedReader;
import java.io.File;
//import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.FileReader;
//import java.io.InputStreamReader;
import java.io.StringReader;

import ntcsss.log.Log;
import ntcsss.log.EntityName;

import ntcsss.database.DatabaseConnectionManager;
import ntcsss.database.PrimeDB;

import ntcsss.libs.ErrorCodes;
import ntcsss.libs.Constants;
import ntcsss.libs.StringLib;
import ntcsss.libs.UserLib;
import ntcsss.libs.NDSLib;
import ntcsss.libs.CryptLib;
import ntcsss.libs.DataLib;
import ntcsss.libs.BundleLib;
import ntcsss.libs.PrintLib;
import ntcsss.libs.ReplicationLib;
import ntcsss.libs.SvrStatLib;
import ntcsss.distmgr.DistributionManager;
import ntcsss.distmgr.DPResults;
import ntcsss.tools.NtcssCmd;
import ntcsss.util.FileOps;
import ntcsss.util.IntegerBuffer;
import java.util.List;

/**
 * This server handles message-based information requests and modifications that
 * are forwarded on to ODBC-compatible database backend.
 * @author  byersde
 * @version
 */
public class DatabaseServer extends ServerBase
{
  
  private static final String  this_servers_name = "DatabaseServer";
  private static int           this_servers_role;
  private static StringBuffer  master_servers_name = new StringBuffer();
  
  // This object is used as a mutex for synchronized areas.
  private static Object db_lock = new Object();
  
 /**
  * Constructor for the NTCSS Database Server
  * This should ALWAYS call the ServerBase class constructor
  **/
  public DatabaseServer( String       messageData,
                         Socket       socket,
                         ServerState  serverState )
  {
    super(messageData, socket, serverState);
    try
    {
      this_servers_role = DataLib.getHostType(StringLib.getHostname());
      DataLib.getMasterServer(master_servers_name);
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_servers_name, exception.toString());
    }
  }
  
  /**
   * Returns an object that contains information about this server including
   *  a list of message handlers this class contains.
   */
  public static ServerDescription getServerDescription()
  {
    Class serverClass;
    
    try
    {
      // Set the local variable to the class object of this server.
      serverClass = DatabaseServer.class;
      
      // DEPRECATED MESSAGES:
      //ADMINLOCKOPS
      //GETSYSMONINI
      //GETUSRADMINI
      //GETNISUPDATE  AKA MT_NISMAPUPDATE
      //GETPROCLIMITS
      
      // This is not used anywhere in our clients or servers.
      // Tony said SPQ may ask for these during dialogs with the operators but a search on
      //Jeff's dir didn't reveal this string anywhere.
      //GETCONFIGFILES
      
      //GETUSERADMINUSRSTR
      
      //NOT SURE ABOUT:
      //COPYMASTERFILES
      //PUTCONFIGFILES
      //CURRENTAPPDATA  --deprecated? couldnt find this anywhere.
      //GETBOOTJOBLIST -- removed over into the server code that makes the call..
      
      
      MessageHandlerDescription handlers[] =
      {
        new MessageHandlerDescription("ADMINLOCKOPS",
                 serverClass.getMethod("handleAdminLockoperations", null)),
        new MessageHandlerDescription("CHECKUCSSN",
                 serverClass.getMethod("checkForSSNdupes", null)),
        new MessageHandlerDescription("CHANGEUSERPASSWD",
                 serverClass.getMethod("handleChangeUserPassword", null)),
        new MessageHandlerDescription("SERVERSTATUS",
                 serverClass.getMethod("serverStatus", null)),
        new MessageHandlerDescription("CHANGEAPPDATA",
                 serverClass.getMethod("handleChangeCustomAppuserData", null)),
        new MessageHandlerDescription("COPYMASTERFILES",
                 serverClass.getMethod("handleCopyBackups", null)),
        new MessageHandlerDescription("GETAPPDATA",
                 serverClass.getMethod("handleGetCustomAppData", null)),
        new MessageHandlerDescription("GETDEFPRTOTYPE",
                 serverClass.getMethod("handleGetOtypeDefPrinter", null)),
        new MessageHandlerDescription("GETBATCHUSER",
                 serverClass.getMethod("handleGetBatchUser", null)),
        new MessageHandlerDescription("GETCOMMONDB",
                 serverClass.getMethod("handleGetSysConData", null)),
        new MessageHandlerDescription("GETACCESSROLE",
                 serverClass.getMethod("handleReturnAccessRoleDummy", null)),
       /* This is now handled in the process reaper..
        new MessageHandlerDescription("GETAPPENVINFO",
                 serverClass.getMethod("handleGetAppEnvInfo", null)),
        */
        new MessageHandlerDescription("GETAPPLIST",
                 serverClass.getMethod("handleGetAppList", null)),
        new MessageHandlerDescription("GETAPPSIGINFO",
                 serverClass.getMethod("handleGetAppKillSignal", null)),
        new MessageHandlerDescription("GETAPPUSERINFO",
                 serverClass.getMethod("handleGetAppuserInfo", null)),
        new MessageHandlerDescription("GETAPPUSERLIST",
                 serverClass.getMethod("handleGetAppuserList", null)),
        new MessageHandlerDescription("GETLINKDATA",
                 serverClass.getMethod("handleGetLinkData", null)),
        new MessageHandlerDescription("DPPROCESSPRTCHANGES",
                 serverClass.getMethod("handlePrinterINIextract", null)),
        new MessageHandlerDescription("GETNISUPDATE",
                 serverClass.getMethod("handleRetrieveNDSupdates", null)),
        new MessageHandlerDescription("GETNTPRINTER",
                 serverClass.getMethod("handleAddPrinterDriver", null)),
        new MessageHandlerDescription("GETOTYPEINI",
                 serverClass.getMethod("handleCreateOtypeINI", null)),
        new MessageHandlerDescription("GETPDJINI",
                 serverClass.getMethod("handleCreatePDJini", null)),
        new MessageHandlerDescription("GETPDJLIST",
                 serverClass.getMethod("handleGetAppPDJs", null)),
        new MessageHandlerDescription("GETUCPROGROUPLIST",
                 serverClass.getMethod("handleCreateUCappList", null)),
        new MessageHandlerDescription("GETUCUSERINFO",
                 serverClass.getMethod("handleGetUCuserInfo", null)),
        new MessageHandlerDescription("GETUSERPRTLIST",
                 serverClass.getMethod("handleGetUsersPrinterList", null)),
        new MessageHandlerDescription("GETUSERDATA",
                 serverClass.getMethod("handleGetAPIuserData", null)),
        new MessageHandlerDescription("GETPRTLIST",
                 serverClass.getMethod("handleGetPrinterList", null)),
        new MessageHandlerDescription("GETUCUSERLIST",
                 serverClass.getMethod("handleGetUCUserList", null)),
        new MessageHandlerDescription("NEWNTPRINTER",
                 serverClass.getMethod("handleAddPrinterDriver", null)),
        new MessageHandlerDescription("PUTOTYPEINI",
                 serverClass.getMethod("handleUpdateOtypes", null)),
        new MessageHandlerDescription("PUTPDJINI",
                 serverClass.getMethod("handleUpdatePDJs", null)),
        new MessageHandlerDescription("REGISTERAPPUSER",
                 serverClass.getMethod("handleAppuserRegistration", null)),
        new MessageHandlerDescription("SETCOMMONDB",
                 serverClass.getMethod("handleSetCommonDBitem", null)),
        new MessageHandlerDescription("SETLINKDATA",
                 serverClass.getMethod("handleSetlinkdata", null)),
        new MessageHandlerDescription("PROCESSUSRCHANGES",
                 serverClass.getMethod("handleUserAdminChanges", null)),
        new MessageHandlerDescription("DPPROCESSUSRCHANGES",
                serverClass.getMethod("ProcessDistributedUsrAdminChanges", null)),
        new MessageHandlerDescription("VALIDATEPRT",
                 serverClass.getMethod("handleValidatePrinter", null)),

        //Did limited testing on this since the LDAP directory structure
        // did not have an userAuthServer entry required by validate..
        new MessageHandlerDescription("VALIDATEUSER",
                 serverClass.getMethod("handleValidateUser", null)),
        new MessageHandlerDescription("IMPORTENV",
                 serverClass.getMethod("handleImportEnvironment", null)),
        new MessageHandlerDescription("GETCURRENTUSERCNT",
                 serverClass.getMethod("handleGetNumLogins", null)),
        new MessageHandlerDescription("GETSYSTEMINFO",
                 serverClass.getMethod("handleGetSystemInfo", null))
      };
      
      // Return the information concerning this class
      return new ServerDescription("Database Server", serverClass, handlers,
      EntityName.DB_SVR_GRP);
    }
    catch (Exception exception)
    {
      Log.excp("DatabaseServer", "getServerDescription", exception.toString());
      return null;
    }
  }
  
  
  /////////////////////////////////////////////////////////////////////////////
   /**
    * This functionality is now deprecated so we just hand back something that
    * looks valid.
    */
  
  public void handleAdminLockoperations()
  {
    final String   this_methods_name = "handleAdminLockoperations";
    try
    {
       if (isServerIdled() == true) {
          sendErrorMessage("NOADMINLOCKOPS", ErrorCodes.ERROR_SERVER_IDLED);
          return;
       }
       
      // send back the bundled-integer equivalent of pid=9999
      sendFormattedServerMsg("GOODADMINLOCKOPS", "149999");
      
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg("NOADMINLOCKOPS",
      StringLib.valueOf(exception.toString().length(), 5)
      + exception.toString());
    }
  }


  /////////////////////////////////////////////////////////////////////////////
   /**
    * Usually called from the User Config program.  This message handler checks
    * to see if the given Social Security Number already exists in the NTCSS
    * database.
    */
  public void checkForSSNdupes()
  {
    Object      arg_list[];
    Connection  db_connection = null;
    ResultSet   db_result_set = null;
    Statement   db_statement = null;
    int         msg_item_lengths[];
    String      msg_items[];
    String      SQL_string;
    String      login_name;
    String      token;
    final String   err_header = "NOCHECKUCSSN";
    final String   this_methods_name = "checkForSSNdupes";

    try
    {
      if (isServerIdled() == true)
        {
          sendErrorMessage("ERROR", ErrorCodes.ERROR_SERVER_IDLED);
          return;
        }

      // Parse the client data
      msg_item_lengths = new int[3];
      msg_item_lengths[0] = Constants.MAX_LOGIN_NAME_LEN;
      msg_item_lengths[1] = Constants.MAX_TOKEN_LEN;
      msg_item_lengths[2] = Constants.MAX_SS_NUM_LEN;
      msg_items = new String[3];

      if (parseData(msg_item_lengths, msg_items) == false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not parse incoming message!");
          sendErrorMessage("ERROR", ErrorCodes.ERROR_CANT_PARSE_MSG);
          return;
        }

      login_name = msg_items[0];
      token = msg_items[1];

      // Check the token & Validate requestor's ID
      int n;
      if (msg_items[1].regionMatches(0, Constants.SERVER_TOKEN_STRING, 0,
                                     Constants.SERVER_TOKEN_STRING.length())
          == true)
        n = doServerValidate(msg_items[0], msg_items[1], getClientAddress());
      else
        {
          IntegerBuffer junk = null;
          n = isUserValid(msg_items[0], msg_items[1], junk);
        }

      if (n < 0)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not validate user " + msg_items[0]
                     + " and/or token.");
          sendErrorMessage(err_header, n);
          return;
        }

      // Get a connection from the database
      db_connection = DatabaseConnectionManager.getConnection();
      if (db_connection == null)
        {
          Log.error(this_servers_name, this_methods_name,
                    "\"null\" database connection returned");
          sendErrorMessage("DB_ERROR", ErrorCodes.ERROR_CANT_ACCESS_DB);
          return;
        }

      // Build the database request:
      arg_list = new Object[1];
      arg_list[0] = msg_items[2];  // The SSN number.
      SQL_string = StringLib.SQLformat("SELECT login_name FROM users WHERE "
                                      + "ss_number=" + Constants.ST, arg_list);
      db_statement = db_connection.createStatement();
      db_result_set = db_statement.executeQuery(SQL_string);

      String response_message;
      // Check if a record is returned....
      if (db_result_set.next() == false)
        {
          response_message = "Provided SSN " + msg_items[2]
                              + " was not found.";
          // We assume the SSN does not exist..
          sendFormattedServerMsg(err_header,
                                 StringLib.valueOf(response_message.length(), 5)
                                  + response_message + "\0");
          Log.debug(this_servers_name, this_methods_name, response_message);
        }
      else
        {
          response_message = "Provided SSN ("
                     + msg_items[2] + ") is currently assigned to user "
                     + db_result_set.getString("login_name");
          Log.debug(this_servers_name, this_methods_name, response_message);
          sendFormattedServerMsg("GOODCHECKUCSSN",
                                 StringLib.valueOf(response_message.length(), 5)
                                  + response_message + "\0");
        }
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg("ERROR",
                            StringLib.valueOf(exception.toString().length(), 5)
                             + exception.toString());
    }
    finally
    {
      // Close the database resources
      try
      {
        if (db_result_set != null)
          db_result_set.close();
        if (db_statement != null)
          db_statement.close();
        if (db_connection != null)
          DatabaseConnectionManager.releaseConnection(db_connection);
      }
      catch (Exception exception)
      {
        Log.excp(this_servers_name, this_methods_name,
                 exception.toString());
      }
    }
  }


  /////////////////////////////////////////////////////////////////////////////
   /**
    * This method changes the specified user's password to a provided value.
    * We don't need a token for validation since the current encrypted
    * password is enough authentication.
    */

  public void handleChangeUserPassword()
  {
    String      msg_items[];
    int         msg_item_lengths[];
    final String   err_header = "NOPASSWDCHANGE ";
    final String   this_methods_name = "changeUserPassword";
    String      newPassword;
    String      returnMessage;

    try
    {
      if (isServerIdled() == true)
        {
          sendErrorMessage(err_header, ErrorCodes.ERROR_SERVER_IDLED);
          return;
        }

      if (this_servers_role != Constants.NTCSS_MASTER)
        {
          sendErrorMessage(err_header, ErrorCodes.ERROR_HOST_IS_NOT_MASTER);
          return;
        }

      msg_item_lengths = new int[3];
      msg_item_lengths[0] = Constants.MAX_LOGIN_NAME_LEN;
      msg_item_lengths[1] = Constants.MAX_PASSWORD_LEN
      + Constants.MAX_PASSWORD_LEN;
      msg_item_lengths[2] = msg_item_lengths[1];

      msg_items = new String[3];

      // Get user's login name, and the old & new passwords.
      if (parseData(msg_item_lengths, msg_items) == false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not parse incoming message!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_PARSE_MSG);
          return;
        }

      int temp;
      newPassword = CryptLib.trimEncryptedPasswordTerminator(msg_items[2]);
      temp = changeUserPassword(msg_items[0], 
            CryptLib.trimEncryptedPasswordTerminator(msg_items[1]), 
            newPassword);
      if (temp < 0)
        {
          sendErrorMessage(err_header, temp);
          return;
        }
      
      // Decrypt the new password
      StringBuffer decryptedNewPassword = new StringBuffer();
      if (CryptLib.plainDecryptString(msg_items[0] + msg_items[0], newPassword,
                                      decryptedNewPassword, false, true) != 0) {
         Log.error(this_servers_name, this_methods_name, "User conversion failed");                                         
         sendErrorMessage(err_header, ErrorCodes.ERROR_INCORRECT_PASSWORD);
         return;                                         
      }
      
      // Change the user's password in the system
      if (UserLib.ChangeSystemAccountPassword(msg_items[0],
                                      decryptedNewPassword.toString()) != 0) {
         sendErrorMessage(err_header, ErrorCodes.ERROR_INCORRECT_PASSWORD);
         return;                                         
      }
      
      // No need to check if this host is the master since this check is
      //  already done at the beginning of this message handler
      if (NDSLib.modifyNtcssPassword(msg_items[0], newPassword) == false) {
         Log.error(this_servers_name, this_methods_name, "Error changing " +
                   "password for <" + msg_items[0] + "> in NDS");
         sendErrorMessage(err_header, ErrorCodes.ERROR_CHANGE_PASSWD_FAILED);
         return;
      }

      // Everything went fine..
      returnMessage = "Password successfully changed.";
      sendFormattedServerMsg("GOODPASSWDCHANGE", 
                 StringLib.valueOf(returnMessage.length(), 5) + returnMessage);
      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg(err_header,
                             StringLib.valueOf(exception.toString().length(), 5)
                              + exception.toString());
      return;
    }
  }


  /////////////////////////////////////////////////////////////////////////////
   /**
    * This method changes the specified user's
    *
    */

  public void handleChangeCustomAppuserData()
  {
    String         msg_items[];
    int            msg_item_lengths[];
    final String   err_header = "NOAPPDATACHANGE";
    final String   this_methods_name = "handleChangeCustomAppuserData";
    
    try
    {
       if (isServerIdled() == true) {
          sendErrorMessage(err_header, ErrorCodes.ERROR_SERVER_IDLED);
          return;
       }
       
      msg_item_lengths = new int[4];
      msg_items = new String[4];
      
      msg_item_lengths[0] = Constants.MAX_LOGIN_NAME_LEN;
      msg_item_lengths[1] = Constants.MAX_TOKEN_LEN;
      msg_item_lengths[2] = Constants.MAX_PROGROUP_TITLE_LEN;
      msg_item_lengths[3] = Constants.MAX_APP_DATA_LEN;
      if (parseData(msg_item_lengths, msg_items) == false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not parse incoming message!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_PARSE_MSG);
          return;
        }

      // Validate requestor's ID
      int n;
      if (msg_items[1].regionMatches(0, Constants.SERVER_TOKEN_STRING, 0,
                                     Constants.SERVER_TOKEN_STRING.length())
          == true)
        n = doServerValidate(msg_items[0], msg_items[1], getClientAddress());
      else
        {
          IntegerBuffer junk = null;
          n = isUserValid(msg_items[0], msg_items[1], junk);
        }

      if (n < 0)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not validate user " + msg_items[0]
                     + " and/or token.");
          sendErrorMessage(err_header, n);
          return;
        }

      n = changeCustomAppuserData(msg_items[0], msg_items[2], msg_items[3]);
      if (n < 0)
        {
          sendErrorMessage(err_header, n);
          return;
        }

      sendFormattedServerMsg("GOODAPPDATACHANGE",
                             "Application user data successfully changed.");
      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg(err_header,
                             StringLib.valueOf(exception.toString().length(), 5)
                              + exception.toString());
      return;
    }
  }


  /////////////////////////////////////////////////////////////////////////////
   /**
    * This method retrieves user/appuser information for the specified
    * appuser and app.  This is initiated by several server-side APIs.
    * A user token or server token is required for authentication.
    */
  
  public void handleGetAPIuserData()
  {
    String         msg_items[];
    int            msg_item_lengths[];
    Connection     db_connection = null;
    Statement      SQL_statement = null;
    ResultSet      db_result_set = null;
    final String   err_header = "NOGETUSERDATA";
    final String   this_methods_name = "handleGetAPIuserData";
    
    try
    {
      if (isServerIdled() == true) {
         sendErrorMessage(err_header, ErrorCodes.ERROR_SERVER_IDLED);
         return;
      }
      
      msg_item_lengths = new int[4];
      msg_items = new String[4];
      
      msg_item_lengths[0] = Constants.MAX_LOGIN_NAME_LEN;
      msg_item_lengths[1] = Constants.MAX_TOKEN_LEN;
      msg_item_lengths[2] = Constants.MAX_LOGIN_NAME_LEN;
      msg_item_lengths[3] = Constants.MAX_PROGROUP_TITLE_LEN;
      if (parseData(msg_item_lengths, msg_items) == false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not parse incoming message!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_PARSE_MSG);
          return;
        }

      // Validate requestor's ID
      int n;
      if (msg_items[1].regionMatches(0, Constants.SERVER_TOKEN_STRING, 0,
                                     Constants.SERVER_TOKEN_STRING.length())
                                        == true)
        n = doServerValidate(msg_items[0], msg_items[1], getClientAddress());
      else
        {
          IntegerBuffer junk = null;
          n = isUserValid(msg_items[0], msg_items[1], junk);
        }
      if (n < 0)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not validate user " + msg_items[0]
                     + " and/or token.");
          sendErrorMessage(err_header, n);
          return;
        }

      // Establish the DB connection.
      db_connection = DatabaseConnectionManager.getConnection();
      if (db_connection == null)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Null DB connection returned!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_ACCESS_DB);
          return;
        }

      Object  arg_list[] = new Object[2];
      arg_list[0] = msg_items[2];
      arg_list[1] = msg_items[3];
      String  SQL_query_string = StringLib.SQLformat(
            "SELECT real_name,ss_number,phone_number,security_class,unix_id,"
             + "custom_app_data,app_passwd,registered_user "
             + "FROM users, app_users WHERE users.login_name=" + Constants.ST
             + " AND app_title=" + Constants.ST 
             + " AND users.login_name = app_users.login_name",
            arg_list);

      // Run the query
      SQL_statement = db_connection.createStatement();
      db_result_set = SQL_statement.executeQuery(SQL_query_string);
      
      String result;
      if (db_result_set.next() == false)
        {
          result = "No database information for this app";
          sendFormattedServerMsg(err_header,
                                 StringLib.valueOf(result.length(), 5)
                                  + result);
          return;
        }
      
      Object  bundle_items[] = new Object[10];
      bundle_items[0] = db_result_set.getString("real_name");
      bundle_items[1] = db_result_set.getString("ss_number");
      bundle_items[2] = db_result_set.getString("phone_number");
      bundle_items[3] = new Integer(db_result_set.getInt("security_class"));
      bundle_items[4] = new Integer(db_result_set.getInt("unix_id"));
      bundle_items[5] = new Integer(0); //user_role    No longer used.
      bundle_items[6] = db_result_set.getString("custom_app_data");
      bundle_items[7] = db_result_set.getString("app_passwd");
      if (db_result_set.getBoolean("registered_user"))
        bundle_items[8] = new Integer(1);
      else
        bundle_items[8] = new Integer(0);

      // Now we have to get a comma-delimited list of all the appuser's roles..
      SQL_query_string = StringLib.SQLformat(
            "SELECT role_name FROM user_app_roles WHERE login_name="
             + Constants.ST + " AND app_title=" + Constants.ST,
            arg_list);
      db_result_set = SQL_statement.executeQuery(SQL_query_string);
      StringBuffer str_roles = new StringBuffer();
      str_roles.setLength(0);
      if (db_result_set.next() == false)
        {
          str_roles.insert(0, "No Access Allowed");
        }
      else
        {
          // Create a comma delimited list of user's roles
           do
            {
              if (str_roles.length() > 0)
                str_roles.append(",");
              str_roles.append(db_result_set.getString(1));
            }
           while (db_result_set.next());
        }
      bundle_items[9] = str_roles.toString();

      StringBuffer bundled_data = new StringBuffer();
      BundleLib.bundleData(bundled_data, "CCCIIICCIC", bundle_items);

      String str_temp = bundled_data.toString();

      bundle_items[0] = new Integer(str_temp.length());
      bundled_data.setLength(0);
      BundleLib.bundleData(bundled_data, "I", bundle_items);

      sendFormattedServerMsg("GOODGETUSERDATA",
                             bundled_data.toString() + str_temp);

      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg(err_header,
                             StringLib.valueOf(exception.toString().length(), 5)
                             + exception.toString());
      return;
    }
    finally
    {
      // Close the database resources
      try
      {
        if (db_result_set != null)
          db_result_set.close();
        if (SQL_statement != null)
          SQL_statement.close();
        if (db_connection != null)
          DatabaseConnectionManager.releaseConnection(db_connection);
      }
      catch (Exception exception)
      {
        Log.excp(this_servers_name, this_methods_name,
                 exception.toString());
      }
    }
  }


  /////////////////////////////////////////////////////////////////////////////
   /**
    * This method retrieves the default printer name for the
    * given printer output type.
    * This is initiated by a server-side API.
    * No tokens are required.
    */
  
  public void handleGetOtypeDefPrinter()
  {
    String         msg_items[];
    int            msg_item_lengths[];
    Connection     db_connection = null;
    Statement      SQL_statement = null;
    ResultSet      db_result_set = null;
    final String   err_header = "NODEFPRTOTYPE";
    final String   this_methods_name = "handleGetOtypeDefPrinter";
    
    try
    {
       if (isServerIdled() == true) {
          sendErrorMessage(err_header, ErrorCodes.ERROR_SERVER_IDLED);
          return;
       }
       
      msg_item_lengths = new int[2];
      msg_items = new String[2];
      
      msg_item_lengths[0] = Constants.MAX_PROGROUP_TITLE_LEN;
      msg_item_lengths[1] = Constants.MAX_OUT_TYPE_TITLE_LEN;
      if (parseData(msg_item_lengths, msg_items) == false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not parse incoming message!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_PARSE_MSG);
          return;
        }

      // Establish the DB connection.
      db_connection = DatabaseConnectionManager.getConnection();
      if (db_connection == null)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Null DB connection returned!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_ACCESS_DB);
          return;
        }

/*      
      Object  arg_list[] = new Object[3];
      arg_list[0] = msg_items[0];
      arg_list[1] = msg_items[1];
      arg_list[2] = new Boolean(false);         // Must never be general access
      String  SQL_query_string = StringLib.SQLformat(
            "SELECT nt_host_name,default_printer FROM printers,output_types"
             + " WHERE app_title=" + Constants.ST
             + " AND otype_title=" + Constants.ST
             + " AND output_types.default_printer=printers.printer_name"
             + " AND general_access=" + Constants.ST,
            arg_list);
 */
      Object  arg_list[] = new Object[4];
      arg_list[0] = msg_items[0];
      arg_list[1] = msg_items[1];
      arg_list[2] = new Boolean(true);
      arg_list[3] = new Boolean(false);         // Must never be general access
      String  SQL_query_string = StringLib.SQLformat(
           "SELECT nt_host_name,output_printers.printer_name FROM printers,output_printers"
           + " WHERE app_title = " + Constants.ST 
           + " AND otype_title = " + Constants.ST
           + " AND default_printer = " + Constants.ST  
           + " AND output_printers.printer_name = printers.printer_name" 
           + " AND general_access = " + Constants.ST,
          arg_list);


      // Run the query
      SQL_statement = db_connection.createStatement();
      db_result_set = SQL_statement.executeQuery(SQL_query_string);
      
      Object  bundle_items[] = new Object[3];
      if (db_result_set.next() == false)
        {
          bundle_items[0] = new Integer(0);
          bundle_items[1] = "";
          bundle_items[2] = "";
        }
      else
        {
          bundle_items[0] = new Integer(1);
//        bundle_items[1] = db_result_set.getString("default_printer");          
          bundle_items[1] = db_result_set.getString("printer_name");
          bundle_items[2] = db_result_set.getString("nt_host_name");
        }

      StringBuffer bundled_data = new StringBuffer();
      BundleLib.bundleData(bundled_data, "ICC", bundle_items);

      sendFormattedServerMsg("GOODDEFPRTOTYPE", bundled_data.toString() + "\0");
      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg(err_header,
                             StringLib.valueOf(exception.toString().length(), 5)
                             + exception.toString() + "\0");
      return;
    }
    finally
    {
      // Close the database resources
      try
      {
        if (db_result_set != null)
          db_result_set.close();
        if (SQL_statement != null)
          SQL_statement.close();
        if (db_connection != null)
          DatabaseConnectionManager.releaseConnection(db_connection);
      }
      catch (Exception exception)
      {
        Log.excp(this_servers_name, this_methods_name,
                 exception.toString());
      }
    }
  }


  /////////////////////////////////////////////////////////////////////////////
   /**
    * This method retrieves the custom_app_data field from the appusers table
    * for the specified appuser and app.
    */
  
  public void handleGetCustomAppData()
  {
    String         msg_items[];
    int            msg_item_lengths[];
    final String   err_header = "NOGETAPPDATA";
    final String   this_methods_name = "handleGetCustomAppData";
    
    try
    {
       if (isServerIdled() == true) {
          sendErrorMessage(err_header, ErrorCodes.ERROR_SERVER_IDLED);
          return;
      }
       
      msg_item_lengths = new int[3];
      msg_items = new String[3];
      
      msg_item_lengths[0] = Constants.MAX_LOGIN_NAME_LEN;
      msg_item_lengths[1] = Constants.MAX_TOKEN_LEN;
      msg_item_lengths[2] = Constants.MAX_PROGROUP_TITLE_LEN;
      if (parseData(msg_item_lengths, msg_items) == false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not parse incoming message!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_PARSE_MSG);
          return;
        }

      // Validate requestor's ID
      int n;
      if (msg_items[1].regionMatches(0, Constants.SERVER_TOKEN_STRING, 0,
                                     Constants.SERVER_TOKEN_STRING.length())
                                        == true)
        n = doServerValidate(msg_items[0], msg_items[1], getClientAddress());
      else
        {
          IntegerBuffer junk = null;
          n = isUserValid(msg_items[0], msg_items[1], junk);
        }
      if (n < 0)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not validate user " + msg_items[0]
                     + " and/or token.");
          sendErrorMessage(err_header, n);
          return;
        }

      StringBuffer appusers_custom_data_field = new StringBuffer();
      n = getCustomAppuserData(msg_items[0], msg_items[2],
                               appusers_custom_data_field);
      if (n < 0)
        {
          sendErrorMessage(err_header, n);
          return;
        }

      Object  bundle_items[] = new Object[2];
      bundle_items[0] = new Integer(appusers_custom_data_field.length() + 5);
      bundle_items[1] = appusers_custom_data_field.toString();
      StringBuffer bundled_data = new StringBuffer();
      BundleLib.bundleData(bundled_data, "IC", bundle_items);

      sendFormattedServerMsg("GOODGETAPPDATA", bundled_data.toString() + "\0");

      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg(err_header,
                             StringLib.valueOf(exception.toString().length(), 5)
                             + exception.toString());
      return;
    }
  }


  /////////////////////////////////////////////////////////////////////////////
   /**
    * This method retrieves the batch user's name for the given app.
    */

  public void handleGetBatchUser()
  {
    String         msg_items[];
    int            msg_item_lengths[];
    final String   err_header = "NOGETBATCHUSER";
    final String   this_methods_name = "handleGetBatchUser";

    try
    {
      if (isServerIdled() == true) {
         sendErrorMessage(err_header, ErrorCodes.ERROR_SERVER_IDLED);
         return;
      }
      
      msg_item_lengths = new int[4];
      msg_items = new String[4];

      msg_item_lengths[0] = Constants.MAX_LOGIN_NAME_LEN;
      msg_item_lengths[1] = Constants.MAX_TOKEN_LEN;
      msg_item_lengths[2] = Constants.MAX_PROGROUP_TITLE_LEN;  // unused
      msg_item_lengths[3] = Constants.MAX_PROGROUP_TITLE_LEN;
      if (parseData(msg_item_lengths, msg_items) == false)
        {
          Log.error(this_servers_name, this_methods_name,
          "Could not parse incoming message!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_PARSE_MSG);
          return;
        }

      // Validate requestor's ID
      int n;
      if (msg_items[1].regionMatches(0, Constants.SERVER_TOKEN_STRING, 0,
                                     Constants.SERVER_TOKEN_STRING.length())
            == true)
        n = doServerValidate(msg_items[0], msg_items[1], getClientAddress());
      else
        {
          IntegerBuffer junk = null;
          n = isUserValid(msg_items[0], msg_items[1], junk);
        }
      if (n < 0)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not validate user " + msg_items[0]
                    + " and/or token.");
          sendErrorMessage(err_header, n);
          return;
        }

      StringBuffer  batch_user_name = new StringBuffer();
      n = getAppBatchUserName(msg_items[0], msg_items[3], batch_user_name);
      if (n < 0)
        {
          sendErrorMessage(err_header, n);
          return;
        }

      sendFormattedServerMsg("GOODGETBATCHUSER", batch_user_name.toString());
      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg(err_header,
                             StringLib.valueOf(exception.toString().length(), 5)
                              + exception.toString());
      return;
    }
  }



  /////////////////////////////////////////////////////////////////////////////
   /**
    * This method just returns a dummy value since it has been deprecated.
    * No authentication is required.
    */

  public void handleReturnAccessRoleDummy()
  {
    final String   this_methods_name = "handleReturnAccessRoleDummy";

    try
    {
      // for future reference, access role will alway be zero from now on.
      sendFormattedServerMsg("GOODGETACCESSROLE", "10\0");
      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg("NOGETACCESSROLE",
                             StringLib.valueOf(exception.toString().length(), 5)
                              + exception.toString());
      return;
    }
  }


  /////////////////////////////////////////////////////////////////////////////
   /**
    * This method retrieves a string value from the SYS_CONF table
    *(now in LDAP) for the key value given.
    */
  
  public void handleGetSysConData()
  {
    String         msg_items[];
    int            msg_item_lengths[];
    final String   err_header = "NOGETCOMMONDB";
    final String   this_methods_name = "handleGetSysConData";
    int errorCode;
    
    try
    {
      msg_item_lengths = new int[3];
      msg_items = new String[3];
      
      msg_item_lengths[0] = Constants.MAX_LOGIN_NAME_LEN;
      msg_item_lengths[1] = Constants.MAX_TOKEN_LEN;
      msg_item_lengths[2] = Constants.MAX_COMDB_TAG_LEN;
      if (parseData(msg_item_lengths, msg_items) == false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not parse incoming message!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_PARSE_MSG);
          return;
        }

      // Validate requestor's ID
      int n;
      if (msg_items[1].regionMatches(0, Constants.SERVER_TOKEN_STRING, 0,
                                     Constants.SERVER_TOKEN_STRING.length())
            == true)
        n = doServerValidate(msg_items[0], msg_items[1], getClientAddress());
      else
        {
          IntegerBuffer junk = null;
          n = isUserValid(msg_items[0], msg_items[1], junk);
        }
      if (n < 0)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not validate user " + msg_items[0]
                    + " and/or token.");
          sendErrorMessage(err_header, n);
          return;
        }

      StringBuffer temp = new StringBuffer();
      if ((errorCode = DataLib.getNtcssSysConfData(msg_items[2], temp)) == 0)
         sendFormattedServerMsg("GOODGETCOMMONDB", StringLib.valueOf(temp.length(), 5) + temp.toString() + "\0"); 
      else
         sendErrorMessage("NOGETCOMMONDB", errorCode);
      
      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg(err_header,
      StringLib.valueOf(exception.toString().length(), 5)
      + exception.toString());
      return;
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  /**
   * This method retrieves a string value from the SYS_CONF table
   *(now in LDAP) for the key value given.
   * NOTE: This was commented out since it is now being handled by the original
   * caller..
   *
  
  public void handleGetAppEnvInfo()
  {
    String         msg_items[];
    int            msg_item_lengths[];
    String     SQL_query_string;
    Connection db_connection = null;
    Statement  SQL_statement = null;
    ResultSet  db_result_set = null;
    Object     arg_list[];
    Object     bundle_items[];
    final String   err_header = "NOGETAPPENVINFO";
    final String   this_methods_name = "handleGetAppEnvInfo";

    try
    {
      if (isServerIdled() == true) {
         sendErrorMessage(err_header, ErrorCodes.ERROR_SERVER_IDLED);
         return;
      }
   
      msg_item_lengths = new int[1];
      msg_items = new String[1];
      
      msg_item_lengths[0] = Constants.MAX_PROGROUP_TITLE_LEN;
      if (parseData(msg_item_lengths, msg_items) == false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not parse incoming message!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_PARSE_MSG);
          return;
        }

      db_connection = DatabaseConnectionManager.getConnection();
      if (db_connection == null)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Null DB connection returned!");
          sendFormattedServerMsg(err_header,
                  ErrorCodes.getErrorMessage(ErrorCodes.ERROR_CANT_ACCESS_DB));
          return;
        }

      // The data given may be the name of the app or its group name..
      arg_list = new Object[2];
      arg_list[0] = msg_items[0];  // app_title
      arg_list[1] = msg_items[0];  // os_group name

      SQL_query_string = StringLib.SQLformat(
                          "SELECT env_filename, psp_filename FROM apps "
                           + "WHERE app_title=" + Constants.ST
                           + " OR os_group =" + Constants.ST, arg_list);

      SQL_statement = db_connection.createStatement();
      db_result_set = SQL_statement.executeQuery(SQL_query_string);

      if (db_result_set.next() == false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Failed to get data for app " + msg_items[0] + "!");
          sendFormattedServerMsg(err_header, "App title/group name "
                                  + msg_items[0] + " not found");
        }

      bundle_items = new Object[2];
      bundle_items[0] = db_result_set.getString("env_filename");
      bundle_items[1] = db_result_set.getString("psp_filename");

      StringBuffer bundled_data = new StringBuffer();
      BundleLib.bundleData(bundled_data, "CC", bundle_items);
      sendFormattedServerMsg("GOODGETAPPENVINFO",
                             StringLib.valueOf(bundled_data.length(), 5)
                              + bundled_data.toString());
      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg(err_header,
                             StringLib.valueOf(exception.toString().length(), 5)
                              + exception.toString());
      return;
    }
    finally
    {
      // Close the database resources
      try
      {
        if (db_result_set != null)
          db_result_set.close();
        if (SQL_statement != null)
          SQL_statement.close();
        if (db_connection != null)
          DatabaseConnectionManager.releaseConnection(db_connection);
      }
      catch (Exception exception)
      {
        Log.excp(this_servers_name, this_methods_name,
                 exception.toString());
      }
    }
  }


  /////////////////////////////////////////////////////////////////////////////
   /**
    * This method sends a list of all the application/host bundled pairs back
    * to the client.
    */

  public void handleGetAppList()
  {
    Object         bundle_items[];
    String         msg_items[];
    int            msg_item_lengths[];
    String         SQL_query_string;
    Connection     db_connection = null;
    Statement      SQL_statement = null;
    ResultSet      db_result_set = null;
    Object         arg_list[];
    final String   err_header = "NOAPPLIST";
    final String   this_methods_name = "handleGetAppList";

    try
    {
      if (isServerIdled() == true) {
        sendErrorMessage(err_header, ErrorCodes.ERROR_SERVER_IDLED);
        return;
      }
      
      msg_item_lengths = new int[2];
      msg_items = new String[2];

      msg_item_lengths[0] = Constants.MAX_LOGIN_NAME_LEN;
      msg_item_lengths[1] = Constants.MAX_TOKEN_LEN;
      if (parseData(msg_item_lengths, msg_items) == false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not parse incoming message!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_PARSE_MSG);
          return;
        }

      // Validate requestor's ID
      int n;
      if (msg_items[1].regionMatches(0, Constants.SERVER_TOKEN_STRING, 0,
                                     Constants.SERVER_TOKEN_STRING.length())
                              == true)
        n = doServerValidate(msg_items[0], msg_items[1], getClientAddress());
      else
        {
          IntegerBuffer junk = null;
          n = isUserValid(msg_items[0], msg_items[1], junk);
        }
      if (n < 0)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not validate user " + msg_items[0]
                     + " and/or token.");
          sendErrorMessage(err_header, n);
          return;
        }

      // Establish the DB connection.
      db_connection = DatabaseConnectionManager.getConnection();
      if (db_connection == null)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Null DB connection returned!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_ACCESS_DB);
          return;
        }

      SQL_query_string = "SELECT app_title, hostname FROM apps";

      // Run the query
      SQL_statement = db_connection.createStatement();
      db_result_set = SQL_statement.executeQuery(SQL_query_string);

      // Set up to bundle up the query results
      bundle_items = new Object[2];
      StringBuffer sub_bundled_data = new StringBuffer(0);
      StringBuffer bundled_data = new StringBuffer(0);
      int number_of_pairs = 0;
      while (db_result_set.next())
        {
          bundle_items[0] = db_result_set.getString("app_title");
          bundle_items[1] = db_result_set.getString("hostname");
          sub_bundled_data.setLength(0);
          BundleLib.bundleData(sub_bundled_data, "CC", bundle_items);
          bundled_data.append(sub_bundled_data);
          number_of_pairs++;
        }

      // prepend the bundled number of app pairs..
      bundle_items[0] = new Integer(number_of_pairs);
      sub_bundled_data.setLength(0);
      BundleLib.bundleData(sub_bundled_data, "I", bundle_items);

      sendFormattedServerMsg("GOODAPPLIST", sub_bundled_data.toString() +
      bundled_data.toString() + "\0");

      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg(err_header,
                             StringLib.valueOf(exception.toString().length(), 5)
                              + exception.toString());
      return;
    }
    finally
    {
      // Close the database resources
      try
      {
        if (db_result_set != null)
          db_result_set.close();
        if (SQL_statement != null)
          SQL_statement.close();
        if (db_connection != null)
          DatabaseConnectionManager.releaseConnection(db_connection);
      }
      catch (Exception exception)
      {
        Log.excp(this_servers_name, this_methods_name,
                 exception.toString());
      }
    }
  }


  /////////////////////////////////////////////////////////////////////////////
   /**
    * This method returns the OS signal(used to kill programs of the specified
    * app with)  to the client.  No authentication is required.
    */

  public void handleGetAppKillSignal()
  {
    Object         bundle_items[];
    String         msg_items[];
    int            msg_item_lengths[];
    String         SQL_query_string;
    Connection     db_connection = null;
    Statement      SQL_statement = null;
    ResultSet      db_result_set = null;
    Object         arg_list[];
    final String   err_header = "NOGETAPPSIGINFO";
    final String   this_methods_name = "handleGetAppKillSignal";

    try
    {
      msg_item_lengths = new int[1];
      msg_items = new String[1];

      msg_item_lengths[0] = Constants.MAX_PROGROUP_TITLE_LEN;
      if (parseData(msg_item_lengths, msg_items) == false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not parse incoming message!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_PARSE_MSG);
          return;
        }

      // Establish the DB connection.
      db_connection = DatabaseConnectionManager.getConnection();
      if (db_connection == null)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Null DB connection returned!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_ACCESS_DB);
          return;
        }

      arg_list = new Object[1];
      arg_list[0] = msg_items[0];
      SQL_query_string = StringLib.SQLformat("SELECT termination_signal "
                       + "FROM apps WHERE app_title =" + Constants.ST,
                      arg_list);
      // Run the query
      SQL_statement = db_connection.createStatement();
      db_result_set = SQL_statement.executeQuery(SQL_query_string);

      String result;
      if (db_result_set.next() == false)
        {
          result = "No database information for this app";
          sendFormattedServerMsg(err_header,
                                 StringLib.valueOf(result.length(), 5)
                                 + result);
          return;
        }

      result = db_result_set.getString("termination_signal");

      sendFormattedServerMsg("GOODGETAPPSIGINFO",
      StringLib.valueOf(result.length(), 5) + result);

      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg(err_header,
                             StringLib.valueOf(exception.toString().length(), 5)
                              + exception.toString());
      return;
    }
    finally
    {
      // Close the database resources
      try
      {
        if (db_result_set != null)
          db_result_set.close();
        if (SQL_statement != null)
          SQL_statement.close();
        if (db_connection != null)
          DatabaseConnectionManager.releaseConnection(db_connection);
      }
      catch (Exception exception)
      {
        Log.excp(this_servers_name, this_methods_name,
                 exception.toString());
      }
    }
  }


  /////////////////////////////////////////////////////////////////////////////
   /**
    * This method returns a special set of data back to the client regarding the
    * appuser represented by the given login_name and app_title.  Only user
    * tokens are accepted for validation.
    */

  public void handleGetAppuserInfo()
  {
    Object         bundle_items[];
    String         msg_items[];
    int            msg_item_lengths[];
    String         SQL_query_string;
    Connection     db_connection = null;
    Statement      SQL_statement = null;
    ResultSet      db_result_set = null;
    Object         arg_list[];
    final String   err_header = "NOAPPUSERINFO";
    final String   this_methods_name = "handleGetAppuserInfo";

    try
    {
       if (isServerIdled() == true) {
          sendErrorMessage(err_header, ErrorCodes.ERROR_SERVER_IDLED);
          return;
       }
       
//Log.error(this_servers_name, this_methods_name,"HERE1");
      msg_item_lengths = new int[4];
      msg_items = new String[4];
      msg_item_lengths[0] = Constants.MAX_LOGIN_NAME_LEN;
      msg_item_lengths[1] = Constants.MAX_TOKEN_LEN;
      msg_item_lengths[2] = Constants.MAX_LOGIN_NAME_LEN;
      msg_item_lengths[3] = Constants.MAX_PROGROUP_TITLE_LEN;
      if (parseData(msg_item_lengths, msg_items) == false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not parse incoming message!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_PARSE_MSG);
          return;
        }
//Log.error(this_servers_name, this_methods_name,"HERE2");
      // Validate requestor's ID
      IntegerBuffer junk = null;
      if (isUserValid(msg_items[0], msg_items[1], junk) < 0)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not validate user " + msg_items[0]
                     + " and/or token.");
          sendErrorMessage(err_header, ErrorCodes.ERROR_USER_NOT_VALID);
          return;
        }
//Log.error(this_servers_name, this_methods_name,"HERE3");
      // Establish the DB connection.
      db_connection = DatabaseConnectionManager.getConnection();
      if (db_connection == null)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Null DB connection returned!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_ACCESS_DB);
        }
//Log.error(this_servers_name, this_methods_name,"HERE4");
      arg_list = new Object[3];
      arg_list[0] = msg_items[3];
      arg_list[1] = msg_items[2];
      arg_list[2] = msg_items[2];
      SQL_query_string = StringLib.SQLformat("SELECT real_name,ss_number,"
                      + "phone_number,security_class,registered_user,"
                      + "unix_id,shared_db_passwd FROM app_users a,"
                      + "users b where a.app_title=" + Constants.ST
                      + " AND a.login_name=" + Constants.ST
                      + " AND b.login_name=" + Constants.ST,
                     arg_list);
//Log.error(this_servers_name, this_methods_name,"HERE5");
      // Run the query
      SQL_statement = db_connection.createStatement();
      db_result_set = SQL_statement.executeQuery(SQL_query_string);

      if (db_result_set.next() == false)
        {
          String message = "No database information for appuser "
                                                                + msg_items[2];
          sendFormattedServerMsg(err_header,
                                 StringLib.valueOf(message.length(), 5)
                                  + message);
          return;
        }
//Log.error(this_servers_name, this_methods_name,"HERE6");
      // Set up to bundle up the query results
      bundle_items = new Object[8];

      bundle_items[0] = db_result_set.getString("real_name");
      bundle_items[1] = db_result_set.getString("ss_number");
      bundle_items[2] = db_result_set.getString("phone_number");
      bundle_items[3] = db_result_set.getObject("security_class");
      bundle_items[4] = UserLib.getRoleString(msg_items[2], msg_items[3]);
      if (db_result_set.getBoolean("registered_user") == true)
        bundle_items[5] = new Integer(0);
      else
        bundle_items[5] = new Integer(1);
      bundle_items[6] = new Integer(db_result_set.getInt("unix_id"));
      bundle_items[7] = db_result_set.getString("shared_db_passwd");
//Log.error(this_servers_name, this_methods_name,"HERE7");
      StringBuffer bundled_data = new StringBuffer(0);
      BundleLib.bundleData(bundled_data, "CCCICIIC", bundle_items);
//Log.error(this_servers_name, this_methods_name,"HERE8");
      sendFormattedServerMsg("GOODAPPUSERINFO", bundled_data.toString() + "\0");
//Log.error(this_servers_name, this_methods_name,"HERE9");
      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg(err_header,
                             StringLib.valueOf(exception.toString().length(), 5)
                              + exception.toString());
      return;
    }
    finally
    {
      // Close the database resources
      try
      {
        if (db_result_set != null)
          db_result_set.close();
        if (SQL_statement != null)
          SQL_statement.close();
        if (db_connection != null)
          DatabaseConnectionManager.releaseConnection(db_connection);
      }
      catch (Exception exception)
      {
        Log.excp(this_servers_name, this_methods_name,
                 exception.toString());
      }
    }
  }


  /////////////////////////////////////////////////////////////////////////////
   /**
    * This method returns a list of all appusers back to the client for the
    * specified app_title.  Only user tokens are accepted for validation.
    */

  public void handleGetAppuserList()
  {
    Object         bundle_items[];
    String         msg_items[];
    int            msg_item_lengths[];
    String         SQL_query_string;
    Connection     db_connection = null;
    Statement      SQL_statement = null;
    ResultSet      db_result_set = null;
    Object         arg_list[];
    final String   err_header = "NOAPPUSERLIST";
    final String   this_methods_name = "handleGetAppuserList";

    try
    {
      if (isServerIdled() == true) {
         sendErrorMessage(err_header, ErrorCodes.ERROR_SERVER_IDLED);
         return;
      }
       
      msg_item_lengths = new int[3];
      msg_items = new String[3];

      msg_item_lengths[0] = Constants.MAX_LOGIN_NAME_LEN;
      msg_item_lengths[1] = Constants.MAX_TOKEN_LEN;
      msg_item_lengths[2] = Constants.MAX_PROGROUP_TITLE_LEN;
      if (parseData(msg_item_lengths, msg_items) == false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not parse incoming message!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_PARSE_MSG);
          return;
        }

      // Validate requestor's ID
      IntegerBuffer junk = null;
      if (isUserValid(msg_items[0], msg_items[1], junk) < 0)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not validate user " + msg_items[0]
                     + " and/or token.");
          sendErrorMessage(err_header, ErrorCodes.ERROR_USER_NOT_VALID);
          return;
        }
      
      int   n_number_of_pairs = 0;
      StringBuffer appuser_string = new StringBuffer(0);
      
      // For some reason, in the C flat file code, the NTCSS group user list
      //  was not returned.
      if (msg_items[2].equals(Constants.NTCSS_APP) == false) {

      // Establish the DB connection.
      db_connection = DatabaseConnectionManager.getConnection();
      if (db_connection == null)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Null DB connection returned!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_ACCESS_DB);
          return;
        }

      arg_list = new Object[1];
      arg_list[0] = msg_items[2];
      SQL_query_string = StringLib.SQLformat(
                           "SELECT login_name,registered_user "
                            + "FROM app_users where app_title=" + Constants.ST,
                           arg_list);

      // Run the query
      SQL_statement = db_connection.createStatement();
      db_result_set = SQL_statement.executeQuery(SQL_query_string);
      
      while (db_result_set.next() != false)
        {
          //concatenate the retreived login_name, padded out with spaces.
          appuser_string.append(
                                StringLib.padString(
                                         db_result_set.getString("login_name"),
                                         Constants.MAX_LOGIN_NAME_LEN, ' '));
          //Add in the registered_user flag
          if (db_result_set.getBoolean("registered_user") == true)
            appuser_string.append(StringLib.valueOf(1, 2));
          else
            appuser_string.append(StringLib.valueOf(0, 2));
          n_number_of_pairs++;
        }
      
      } // End of "if (msg_items[2].equals(Constants.NTCSS_APP) == false)"

      bundle_items = new Object[1];
      StringBuffer sub_bundled_data = new StringBuffer(0);

      // prepend a bundled int representing the number of data pairs
      // in the string..
      bundle_items[0] = new Integer(n_number_of_pairs);
      BundleLib.bundleData(sub_bundled_data, "I", bundle_items);
      appuser_string.insert(0, sub_bundled_data);

      // Prepend a bundled int that represents the new total string size
      sub_bundled_data.setLength(0);
      bundle_items[0] = new Integer(appuser_string.length());
      BundleLib.bundleData(sub_bundled_data, "I", bundle_items);
      appuser_string.insert(0, sub_bundled_data);
      
      Log.debug(this_servers_name, this_methods_name, "Returning data <" + 
                appuser_string.toString() + ">");

      sendFormattedServerMsg("GOODAPPUSERLIST", appuser_string.toString());
      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg(err_header,
                             StringLib.valueOf(exception.toString().length(), 5)
                             + exception.toString());
      return;
    }
    finally
    {
      // Close the database resources
      try
      {
        if (db_result_set != null)
          db_result_set.close();
        if (SQL_statement != null)
          SQL_statement.close();
        if (db_connection != null)
          DatabaseConnectionManager.releaseConnection(db_connection);
      }
      catch (Exception exception)
      {
        Log.excp(this_servers_name, this_methods_name,
                 exception.toString());
      }
    }
  }


  /////////////////////////////////////////////////////////////////////////////
   /**
    * This method returns a bundled string with the link_data for the specified
    * application title back to the client.
    * A server or user token is required for authentication.
    */

  public void  handleGetLinkData()
  {
    Object         bundle_items[];
    String         msg_items[];
    int            msg_item_lengths[];
    String         SQL_query_string;
    Connection     db_connection = null;
    Statement      SQL_statement = null;
    ResultSet      db_result_set = null;
    Object         arg_list[];
    final String   err_header = "NOGETLINKDATA";
    final String   this_methods_name = "handleGetLinkData";

    try
    {
      if (isServerIdled() == true) {
         sendErrorMessage(err_header, ErrorCodes.ERROR_SERVER_IDLED);
         return;
      }
      
      msg_item_lengths = new int[3];
      msg_items = new String[3];

      msg_item_lengths[0] = Constants.MAX_LOGIN_NAME_LEN;
      msg_item_lengths[1] = Constants.MAX_TOKEN_LEN;
      msg_item_lengths[2] = Constants.MAX_PROGROUP_TITLE_LEN;
      if (parseData(msg_item_lengths, msg_items) == false)
      {
        Log.error(this_servers_name, this_methods_name,
                  "Could not parse incoming message!");
        sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_PARSE_MSG);
        return;
      }

      // Validate requestor's ID
      int n;
      if (msg_items[1].regionMatches(0, Constants.SERVER_TOKEN_STRING, 0,
                                     Constants.SERVER_TOKEN_STRING.length())
                              == true)
        n = doServerValidate(msg_items[0], msg_items[1], getClientAddress());
      else
        {
          IntegerBuffer junk = null;
          n = isUserValid(msg_items[0], msg_items[1], junk);
        }
      if (n < 0)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not validate user " + msg_items[0]
                     + " and/or token.");
          sendErrorMessage(err_header, n);
          return;
        }

      // Establish the DB connection.
      db_connection = DatabaseConnectionManager.getConnection();
      if (db_connection == null)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Null DB connection returned!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_ACCESS_DB);
          return;
        }

      arg_list = new Object[1];
      arg_list[0] = msg_items[2];
      SQL_query_string = StringLib.SQLformat(
                              "SELECT link_data FROM apps WHERE app_title="
                              + Constants.ST,
                             arg_list);
     
      // Run the query
      SQL_statement = db_connection.createStatement();
      db_result_set = SQL_statement.executeQuery(SQL_query_string);

      // Set up to bundle up the query results
      if ((db_result_set.next() == false) ||
          (db_result_set.getString("link_data") == null) ||
          (db_result_set.getString("link_data").trim().length() == 0))
        {
          String message = "No database link data for app " + msg_items[2];
          sendFormattedServerMsg(err_header,
                                 StringLib.valueOf(message.length(), 5)
                                 + message);
          return;
        }

      bundle_items = new Object[2];
      StringBuffer response_string = new StringBuffer(0);
      String link_data = db_result_set.getString("link_data");

      if (link_data == null)
        link_data = "";        // So we can use the .length() method on this.

      // Unencrypt the link data 
      StringBuffer  decrypted_string = new StringBuffer(); 
      if (CryptLib.plainDecryptString( msg_items[2] + msg_items[2],
                                       link_data,
                                       decrypted_string, false, true) != 0)
        {
          String message = "Unable to decrypt DB password for " + msg_items[0];
          Log.error(this_servers_name, this_methods_name, message);
          sendFormattedServerMsg(err_header,
                                 StringLib.valueOf(message.length(), 5)
                                 + message);
          return;
        }

      bundle_items[0] = new Integer(decrypted_string.length() + 5);
      bundle_items[1] = decrypted_string.toString();
      BundleLib.bundleData(response_string, "IC", bundle_items);

      sendFormattedServerMsg("GOODGETLINKDATA", response_string.toString());

      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg(err_header,
                             StringLib.valueOf(exception.toString().length(), 5)
                             + exception.toString());
      return;
    }
    finally
    {
      // Close the database resources
      try
      {
        if (db_result_set != null)
          db_result_set.close();
        if (SQL_statement != null)
          SQL_statement.close();
        if (db_connection != null)
          DatabaseConnectionManager.releaseConnection(db_connection);
      }
      catch (Exception exception)
      {
        Log.excp(this_servers_name, this_methods_name,
                 exception.toString());
      }
    }
  }


  /////////////////////////////////////////////////////////////////////////////
   /**
    * This method takes the given printer driver information and adds it to
    * the print_driver_files table.  All data will be inserted as new records
    * and will not overwrite old ones.  If an error occurs during any insert,
    * it is ignored until attmeps to insert all the records have been completed.
    * A server or user token is required for authentication.
    */

  public void  handleAddPrinterDriver()
  {
    String         msg_items[];
    String         error_message = null;
    int            msg_item_lengths[];
    String         SQL_query_string;
    Connection     db_connection = null;
    Statement      SQL_statement = null;
    Object         arg_list[];
    final String   err_header = "NONEWNTPRINTER";
    final String   this_methods_name = "handleAddPrinterDriver";

    try
    {
      if (isServerIdled() == true) {
         sendErrorMessage(err_header, ErrorCodes.ERROR_SERVER_IDLED);
         return;
      }
      
      msg_item_lengths = new int[10];
      msg_items = new String[10];

      msg_item_lengths[0] = Constants.MAX_LOGIN_NAME_LEN;
      msg_item_lengths[1] = Constants.MAX_TOKEN_LEN;
      //drivername driver_file_group
      msg_item_lengths[2] = Constants.MAX_PRT_DEVICE_NAME_LEN;
      msg_item_lengths[3] = Constants.MAX_PRT_FILE_LEN; //driver file
      msg_item_lengths[4] = Constants.MAX_PRT_FILE_LEN; //ui file
      msg_item_lengths[5] = Constants.MAX_PRT_FILE_LEN; //data file
      msg_item_lengths[6] = Constants.MAX_PRT_FILE_LEN; //help file
      msg_item_lengths[7] = Constants.MAX_PRT_FILE_LEN; //monitor file
      msg_item_lengths[8] = Constants.MAX_PRT_FILE_LEN; //monitor string
      msg_item_lengths[9] = Constants.MAX_COPY_FILE_LENGTH; //copy files

      if (parseData(msg_item_lengths, msg_items) == false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not parse incoming message!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_PARSE_MSG);
          return;
        }

      // Validate requestor's ID
      int  n;
      if (msg_items[1].regionMatches(0, Constants.SERVER_TOKEN_STRING, 0,
                                     Constants.SERVER_TOKEN_STRING.length())
                         == true)
         n = doServerValidate(msg_items[0], msg_items[1], getClientAddress());
      else
        {
          IntegerBuffer junk = null;
          n = isUserValid(msg_items[0], msg_items[1], junk);
        }
      if (n < 0)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not validate user " + msg_items[0]
                     + " and/or token.");
          sendErrorMessage(err_header, n);
          return;
        }

      // Establish the DB connection.
      db_connection = DatabaseConnectionManager.getConnection();
      if (db_connection == null)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Null DB connection returned!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_ACCESS_DB);
          return;
        }

      SQL_statement = db_connection.createStatement();

      arg_list = new Object[3];
      arg_list[0] = msg_items[2];  // driver_file_group(stays constant)

      // first check to make sure the driver doesn't already exist.  If it does,
      // we just return a success.
      SQL_query_string = StringLib.SQLformat(
            "SELECT COUNT(driver_name) from print_drivers WHERE driver_name="
             + Constants.ST, arg_list);
      
      ResultSet  db_result_set;
      db_result_set = SQL_statement.executeQuery(SQL_query_string);
      if (!db_result_set.next())
        {
          Log.error(this_servers_name, this_methods_name, "Query failed: "
                      + SQL_query_string);
          String errorMsg = "SQL query failed";
          
          sendFormattedServerMsg(err_header, StringLib.valueOf(
                                 errorMsg.length(), 5) + errorMsg);
          return;
        }
      
      if (db_result_set.getInt(1) > 0)          
        {
          sendFormattedServerMsg("GOODNEWNTPRINTER", "00000");
          return;
        }

      arg_list[1] = msg_items[8];  // monitor string

      //INSERT INTO MAIN TABLE FIRST
      SQL_query_string = StringLib.SQLformat(
            "INSERT INTO print_drivers(driver_name,nt_monitor_name) VALUES ("
             + Constants.ST + "," + Constants.ST + ")",
            arg_list);

      // Run the query
      String  last_warning_msg = "Unknown Error";
      int     i_temp = 0;
      try
        {
          i_temp = SQL_statement.executeUpdate(SQL_query_string);
          if (i_temp < 0)
            {
              last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
              Log.error(this_servers_name, this_methods_name, last_warning_msg);
            }
        }
      catch(Exception exception)
        {
          i_temp = -1;   // Just set this so it will be seen below.
          last_warning_msg = exception.toString();
          Log.excp(this_servers_name, this_methods_name, last_warning_msg);
          Log.error(this_servers_name, this_methods_name, SQL_query_string);
        }
        
      // If the drive name ends in the Windows 2000 special extension, then
      //  the driver files don't need to be placed in the print_driver_files table      
//     if (msg_items[2].endsWith(Constants.W2K_PRT_DRVR_EXT) == false) {

      // Insert the "FILENAME" driver:
      // Retain the first entry from the first array and overwrite the rest..
      arg_list[1] = new Integer(Constants.DRIVER_DRIVER_TYPE);
      arg_list[2] = msg_items[3];
      SQL_query_string = StringLib.SQLformat(
                    "INSERT INTO print_driver_files(driver_name,type,"
                     + "driver_filename) VALUES(" + Constants.ST
                     + "," + Constants.ST + "," + Constants.ST + ")",
                    arg_list);
      // Run the query
      last_warning_msg = "Unknown error";
      try
        {
          i_temp = SQL_statement.executeUpdate(SQL_query_string);
          if (i_temp < 0)
            {
              last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
              Log.error(this_servers_name, this_methods_name, last_warning_msg);
            }
        }
      catch(Exception exception)
        {
          i_temp = -1;   // Just set this so it will be seen below.
          last_warning_msg = exception.toString();
          Log.excp(this_servers_name, this_methods_name, last_warning_msg);
        }

      // Insert the "UI" driver name:
      arg_list[1] = new Integer(Constants.UI_DRIVER_TYPE);
      arg_list[2] = msg_items[4];
      SQL_query_string = StringLib.SQLformat(
                "INSERT INTO print_driver_files(driver_name,type,"
                 + "driver_filename) VALUES(" + Constants.ST
                 + "," + Constants.ST + "," + Constants.ST + ")",
                arg_list);
      // Run the query
      last_warning_msg = "Unknwon error";
      try
        {
          i_temp = SQL_statement.executeUpdate(SQL_query_string);
          if (i_temp < 0)
            {
              last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
              Log.error(this_servers_name, this_methods_name, last_warning_msg);
            }
        }
      catch(Exception exception)
        {
          i_temp = -1;   // Just set this so it will be seen below.
          last_warning_msg = exception.toString();
          Log.excp(this_servers_name, this_methods_name, last_warning_msg);
        }

      // Insert the "data" driver name:
      arg_list[1] = new Integer(Constants.DATA_DRIVER_TYPE);
      arg_list[2] = msg_items[5];
      SQL_query_string = StringLib.SQLformat(
          "INSERT INTO print_driver_files(driver_name,type,"
           + "driver_filename) VALUES(" + Constants.ST
           + "," + Constants.ST + "," + Constants.ST + ")",
          arg_list);
      last_warning_msg = "Unknown error";
      try
        {
          i_temp = SQL_statement.executeUpdate(SQL_query_string);
          if (i_temp < 0)
            {
              last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
              Log.error(this_servers_name, this_methods_name, last_warning_msg);
            }
        }
      catch(Exception exception)
        {
          i_temp = -1;   // Just set this so it will be seen below.
          last_warning_msg = exception.toString();
          Log.error(this_servers_name, this_methods_name, last_warning_msg);
        }

      // Insert the "helpfile" driver name:
      arg_list[1] = new Integer(Constants.HELP_DRIVER_TYPE);
      arg_list[2] = msg_items[6];

      SQL_query_string = StringLib.SQLformat(
          "INSERT INTO print_driver_files(driver_name,type,"
           + "driver_filename) VALUES(" + Constants.ST
           + "," + Constants.ST + "," + Constants.ST + ")",
          arg_list);
      last_warning_msg = "Unknown Error";
      try
        {
          i_temp = SQL_statement.executeUpdate(SQL_query_string);
          if (i_temp < 0)
            {
              last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
              Log.error(this_servers_name, this_methods_name, last_warning_msg);
            }
        }
      catch(Exception exception)
        {
          i_temp = -1;   // Just set this so it will be seen below.
          last_warning_msg = exception.toString();
          Log.excp(this_servers_name, this_methods_name, last_warning_msg);
        }

      // Insert the "monitor" driver name:
      arg_list[1] = new Integer(Constants.MONITOR_DRIVER_TYPE);
      arg_list[2] = msg_items[7];

      SQL_query_string = StringLib.SQLformat(
          "INSERT INTO print_driver_files(driver_name,type,"
           + "driver_filename) VALUES(" + Constants.ST
           + "," + Constants.ST + "," + Constants.ST + ")",
          arg_list);
      last_warning_msg = "Unknown Error";
      try
        {
          i_temp = SQL_statement.executeUpdate(SQL_query_string);
          if (i_temp < 0)
            {
              last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
              Log.error(this_servers_name, this_methods_name, last_warning_msg);
            }
        }
      catch(Exception exception)
        {
          i_temp = -1;   // Just set this so it will be seen below.
          last_warning_msg = exception.toString();
          Log.excp(this_servers_name, this_methods_name, last_warning_msg);
        }

      // Add the driver files...
      if(msg_items[9].length()>0) 
        {
          arg_list[1] = new Integer(Constants.NO_DRIVER_TYPE);
          StringTokenizer tokenizer = new StringTokenizer(msg_items[9],",");
          Log.debug(this_servers_name, this_methods_name, "Copy String-> "
                     + msg_items[9]);

          while (tokenizer.hasMoreElements() == true)
            {
              arg_list[2] = tokenizer.nextToken();
 
              SQL_query_string = StringLib.SQLformat(
                   "INSERT INTO print_driver_files(driver_name,type,"
                    + "driver_filename) VALUES(" + Constants.ST
                    + "," + Constants.ST + "," + Constants.ST + ")",
                   arg_list);

              last_warning_msg = "Unknown Error";
              try
                {
                  i_temp = SQL_statement.executeUpdate(SQL_query_string);
                  if (i_temp < 0)
                    {
                      last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
                      Log.error(this_servers_name, this_methods_name,
                                last_warning_msg);
                    }
                }
              catch(Exception exception)
                {
                  i_temp = -1;   // Just set this so it will be seen below.
                  last_warning_msg = exception.toString();
                  Log.excp(this_servers_name, this_methods_name,
                            last_warning_msg);
                }
           }
       }
        
      //      } // end of "if (msg_items.endsWith(W2K_PRT_DRVR_EXT) == false) "

      // We need this to drag the legless soldier with
      //IF we merge baselines we can lose this
      //It creates a NTPRINT.INI that can be sent with the drivers
      //for backwards compatibilty
      StringBuffer sb = new StringBuffer();
      if(PrintLib.exportPrtDrivers(Constants.NT_INI_FILE, sb))
          sendFormattedServerMsg("GOODNEWNTPRINTER", "00000");
      else
        {
          String errorMsg = "Couldn't create " + Constants.NT_INI_FILE +
                         "... " + sb.toString();
          
          Log.warn(this_servers_name, this_methods_name, errorMsg);
          sendFormattedServerMsg(err_header, StringLib.valueOf(
                                 errorMsg.length(), 5) + errorMsg);
        }

      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg(err_header, StringLib.valueOf(
                                           exception.toString().length(), 5)
                                                       + exception.toString());
      return;
    }
    finally
    {
      try 
      {
        if (SQL_statement!=null)
           SQL_statement.close();
        if (db_connection != null)
         DatabaseConnectionManager.releaseConnection(db_connection);
      }
      catch (Exception exception)
      {
        Log.excp(this_servers_name, this_methods_name,
                 exception.toString());
      }
    }
  }


  /////////////////////////////////////////////////////////////////////////////
   /**
    * This method creates an otype.ini file and returns the
    * name of the .INI file back to the client.
    * Only a user token is required.
    */

  public void handleCreatePDJini()
  {
    Object         bundle_items[];
    String         msg_items[];
    int            msg_item_lengths[];
    String         SQL_query_string;
    Connection     db_connection = null;
    Statement      SQL_statement = null;
    ResultSet      db_result_set = null;
    Object         arg_list[];
    final String   err_header = "NOPDJINI";
    final String   this_methods_name = "handleCreatePDJini";
    IntegerBuffer  junk = null;
    String         response_msg;
    File           pdj_ini_file;
    INI            ini_writer;
    String         pdjDataFilename = Constants.NTCSS_PDJ_INI_FILE;
    Vector         appList;
    String         current_app_title = null;
    String         cursor_app_title = null;
    int            num_apps_pdjs = 0;
    String         job_type;
    int            numApps;
    String[]       elements = new String[0];
    String         current_pdj = null;
    String         cursor_pdj = null;
    StringBuffer   roles;
    String         sectionName;
    Vector         pdjList;
    Object         sqlItems[];
    int            numPdjs;
    String         current_pdj_title;

    try
    {
      
      // Check to see if the server is idled
      if (isServerIdled() == true) {
         sendErrorMessage(err_header, ErrorCodes.ERROR_SERVER_IDLED);
         return;
      }
      
      msg_item_lengths = new int[2];
      msg_items = new String[2];

      msg_item_lengths[0] = Constants.MAX_LOGIN_NAME_LEN;
      msg_item_lengths[1] = Constants.MAX_TOKEN_LEN;      
      if (parseData(msg_item_lengths, msg_items) == false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not parse incoming message!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_PARSE_MSG);
          return;
        }

      // Validate requestor's ID      
      if (isUserValid(msg_items[0], msg_items[1], junk) < 0)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not validate user " + msg_items[0]
                     + " and/or token.");
          sendErrorMessage(err_header, ErrorCodes.ERROR_USER_NOT_VALID);
          return;
        }

      response_msg = StringLib.valueOf(
                           pdjDataFilename.length(), 5) + pdjDataFilename;

      // If pdj file already exists, delete it.
      pdj_ini_file = new File(pdjDataFilename);
      if (pdj_ini_file.exists() == true)
         pdj_ini_file.delete();      

      ini_writer = new INI();

      // Establish the DB connection.
      db_connection = DatabaseConnectionManager.getConnection();
      if (db_connection == null)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Null DB connection returned!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_ACCESS_DB);
          return;
        }
      SQL_statement = db_connection.createStatement();
      
      // Get a list of all the applications
      SQL_query_string = "SELECT app_title FROM apps";
      db_result_set = SQL_statement.executeQuery(SQL_query_string);
      if (db_result_set.next() == false)
        {
          Log.error(this_servers_name, this_methods_name, "No apps available!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_CREATE_INI_FILE);
          return;
        }

      // Put all the app titles in the [PDJ] section..     
      appList = new Vector();
      do
        {
          ini_writer.WriteProfileString("PDJ",
                               db_result_set.getString("app_title"),
                               db_result_set.getString("app_title"),
                               pdjDataFilename);   
          appList.add(db_result_set.getString("app_title"));
        }
      while (db_result_set.next() != false);

      // For each app, create an app section which lists all of its predefined
      //  jobs, and create a section of each predefined job
      numApps = appList.size();      
      roles = new StringBuffer();
      pdjList = new Vector();      
      sqlItems = new Object[2];
      for (int e = 0; e < numApps; e++) {
         
          current_app_title = (String)appList.elementAt(e);
          
          // Make sure a section gets written out for each application even
          //  though it may not have any predefined jobs 
          ini_writer.WriteProfileSection(current_app_title, elements, 
                                         pdjDataFilename);
          
          pdjList.clear();          
          
          // Get a list of this app's predefined jobs
          db_result_set = SQL_statement.executeQuery("SELECT pdj_title FROM " +
               "predefined_jobs WHERE app_title = '" + current_app_title + "'");                      
          while (db_result_set.next() != false) 
             pdjList.add(db_result_set.getString("pdj_title"));
          
          numPdjs = pdjList.size();
          
          for (int f = 0; f < numPdjs; f++) {
             
             current_pdj_title = (String)pdjList.elementAt(f);
             
             sqlItems[0] = current_app_title;
             sqlItems[1] = current_pdj_title;
             db_result_set = SQL_statement.executeQuery(
                    StringLib.SQLformat("SELECT * FROM " +
                       "predefined_jobs WHERE app_title = " + Constants.ST + 
                       " AND pdj_title = " + Constants.ST, sqlItems));
             
             db_result_set.next();
                          
             sectionName = current_app_title + "_" + (f + 1);
             ini_writer.WriteProfileString(current_app_title,
                               sectionName, sectionName, pdjDataFilename);
          
             ini_writer.WriteProfileString(sectionName, "TITLE", 
                               current_pdj_title, pdjDataFilename);
             ini_writer.WriteProfileString(sectionName, "COMMAND", 
                               db_result_set.getString("command_line"),
                               pdjDataFilename);
             ini_writer.WriteProfileString(sectionName, "SCHEDULE", 
                               db_result_set.getString("schedule_str"),
                               pdjDataFilename);         

             switch(db_result_set.getInt("job_type")) {            
                case Constants.REGULAR_JOB:
                   job_type = "Regular";
                   break;
                case Constants.BOOT_JOB:
                   job_type = "Boot";
                   break;
                case Constants.SCHEDULED_JOB:
                   job_type = "Scheduled";
                   break;
                default:
                   job_type = "";
                   break;
             }
            
             ini_writer.WriteProfileString(sectionName, "JOBCLASS", job_type, 
                                              pdjDataFilename);
             
             sqlItems[0] = current_app_title;
             sqlItems[1] = current_pdj_title;
             db_result_set = SQL_statement.executeQuery(
                    StringLib.SQLformat("SELECT role_name FROM " +
                       "pdj_app_roles WHERE app_title = " + Constants.ST + 
                       " AND pdj_title = " + Constants.ST, sqlItems));
             
             roles.setLength(0);
             while (db_result_set.next() != false) 
                if (roles.length() > 0)
                   roles.append("," + db_result_set.getString("role_name"));
                else
                   roles.insert(0, db_result_set.getString("role_name"));
             
             ini_writer.WriteProfileString(sectionName, "ROLES", roles.toString(), 
                                              pdjDataFilename);
          }
          
          
      } // End for loop                                                   

      // Write out the [ROLES] section information
      SQL_query_string = "SELECT app_title,role_name FROM app_roles " +
                         "GROUP BY app_title,role_name";      
      db_result_set = SQL_statement.executeQuery(SQL_query_string);

      if (db_result_set.next() == false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "No app pdj roles found!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_CREATE_INI_FILE);
          return;
        }
      
      cursor_app_title = null;
      String  previous_app_title = null;
      StringBuffer  pdj_roles = new StringBuffer();

      do
        {
          cursor_app_title = db_result_set.getString("app_title");
          // Have we switched to a new app title?

          if (!cursor_app_title.equals(previous_app_title))
            {
              // If this isn't the first time round, we need to write out the
              // roles string we just built.
              if (previous_app_title != null)                
                ini_writer.WriteProfileString("ROLES", previous_app_title, 
                                    pdj_roles.toString(), pdjDataFilename);
          
              pdj_roles.setLength(0);
              previous_app_title = cursor_app_title; //For future comparisons.
            }
          // Concatenate the next role onto the roles string..
          if (pdj_roles.length() == 0)
            pdj_roles.append(db_result_set.getString("role_name"));
          else
            pdj_roles.append("," + db_result_set.getString("role_name"));
        }
      while (db_result_set.next() != false);
      
      
      //Specify the PDJ type ID strings to be used..      
      ini_writer.WriteProfileString("JOBCLASS", "CLASSES", 
                                    "Regular,Boot,Scheduled", pdjDataFilename);

      ini_writer.FlushINIFile(pdjDataFilename);      

      sendFormattedServerMsg("GOODPDJINI", response_msg);
      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg(err_header,
                             StringLib.valueOf(exception.toString().length(), 5)
                             + exception.toString());
      return;
    }
    finally
    {
      // Close the database resources
      try
      {
        if (db_result_set != null)
          db_result_set.close();
        if (SQL_statement != null)
          SQL_statement.close();
        if (db_connection != null)
          DatabaseConnectionManager.releaseConnection(db_connection);
      }
      catch (Exception exception)
      {
        Log.excp(this_servers_name, this_methods_name,
                 exception.toString());
      }
    }
  }


  /////////////////////////////////////////////////////////////////////////////
   /**
    * This method creates an PDJ INI file and returns the
    * name of the .INI file back to the client.
    * A user or server token is required.
    */

  public void handleGetAppPDJs()
  {
    Object         bundle_items[];
    String         msg_items[];
    int            msg_item_lengths[];
    String         SQL_query_string;
    Connection     db_connection = null;
    Statement      SQL_statement = null;
    ResultSet      db_result_set = null;
    Object         arg_list[];
    final String   err_header = "NOGETPDJLIST";
    final String   this_methods_name = "handleGetAppPDJs";
    int            roleMask;
    Vector         pdjList;
    StringBuffer   bundled_pdjs, bundled_numPdjs;
    String         pdjTitle;  
    String         bundleLengthStr;

    try
    {
      if (isServerIdled() == true) {
         sendErrorMessage(err_header, ErrorCodes.ERROR_SERVER_IDLED);
         return;
      }
      
      msg_item_lengths = new int[4];
      msg_item_lengths[0] = Constants.MAX_LOGIN_NAME_LEN;
      msg_item_lengths[1] = Constants.MAX_TOKEN_LEN;
      msg_item_lengths[2] = Constants.MAX_PROGROUP_TITLE_LEN;
      msg_item_lengths[3] = Constants.MAX_NUM_APP_ROLES_PER_PROGROUP;
      msg_items = new String[4];
      if (parseData(msg_item_lengths, msg_items) == false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not parse incoming message!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_PARSE_MSG);
          return;
        }     
      
      Log.debug(this_servers_name, this_methods_name, "Received role <" + 
                                                   msg_items[3] + ">");      

      // Validate requestor's ID
      int n;
      if (msg_items[1].regionMatches(0, Constants.SERVER_TOKEN_STRING, 0,
                                     Constants.SERVER_TOKEN_STRING.length())
                              == true)
        n = doServerValidate(msg_items[0], msg_items[1], getClientAddress());
      else
        {
          IntegerBuffer junk = null;
          n = isUserValid(msg_items[0], msg_items[1], junk);
        }
      if (n < 0)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not validate user " + msg_items[0]
                     + " and/or token.");
          sendErrorMessage(err_header, n);
          return;
        }

      // Establish the DB connection.
      db_connection = DatabaseConnectionManager.getConnection();
      if (db_connection == null)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Null DB connection returned!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_ACCESS_DB);
          return;
        }

      SQL_statement = db_connection.createStatement();                     
      
      // Get a list of "Regular" type predefined jobs for the given application
      arg_list = new Object[1];
      arg_list[0] = msg_items[2];
      SQL_query_string = StringLib.SQLformat("SELECT pj.pdj_title, " + 
                     "command_line, job_type, role_number FROM predefined_jobs " +
                     "AS pj, pdj_app_roles AS par, app_roles AS ar WHERE " + 
                     "pj.app_title = " + Constants.ST + " AND job_type = 0 " +
                     "AND pj.pdj_title=par.pdj_title AND " + 
                     "pj.app_title=par.app_title AND par.app_title = " +
                     "ar.app_title AND par.role_name=ar.role_name ORDER BY " +
                     "pj.pdj_title", arg_list);
      db_result_set = SQL_statement.executeQuery(SQL_query_string);
      
      // Bundle the pdj information
      roleMask = StringLib.parseInt(msg_items[3], 0);      
      pdjList = new Vector();
      bundle_items = new Object[3];
      bundled_pdjs = new StringBuffer(0);
      while (db_result_set.next() != false) {                     
         
         // Only include predefined jobs that have roles that are included
         //  in the passed in role mask
         if ((roleMask & (1 << (db_result_set.getInt("role_number") + 1))) != 0) {                        
            
            pdjTitle = db_result_set.getString("pj.pdj_title");
            if (pdjList.contains(pdjTitle) == false) {
               
               Log.debug(this_servers_name, this_methods_name,                 
                                         "Returning PDJ <" + pdjTitle  + ">");
               
               bundle_items[0] = pdjTitle;
               bundle_items[1] = db_result_set.getString("command_line");
               bundle_items[2] = new Integer(db_result_set.getInt("job_type"));
               BundleLib.bundleData(bundled_pdjs, "CCI", bundle_items);
            
               pdjList.add(pdjTitle);
            }
         }                          
      }   // End while loop         
      
      bundled_numPdjs = new StringBuffer(0);
      bundle_items[0] = new Integer(pdjList.size());
      BundleLib.bundleData(bundled_numPdjs, "I", bundle_items);
      
      Log.debug(this_servers_name, this_methods_name, "Total number of " +
                "PDJs returned <" + pdjList.size() + ">");

      if (pdjList.size() > 0) {         
         bundleLengthStr = String.valueOf(bundled_numPdjs.length() + 
                                          bundled_pdjs.length());         
         sendFormattedServerMsg("GOODGETPDJLIST", 
                String.valueOf(bundleLengthStr.length()) + bundleLengthStr +
                bundled_numPdjs.toString() + bundled_pdjs.toString() + "\0");
      }
      else
         sendFormattedServerMsg("GOODGETPDJLIST", "10" + "\0");               

      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg(err_header,
      StringLib.valueOf(exception.toString().length(), 5) +
      exception.toString());
      return;
    }
    finally
    {
      // Close the database resources
      try
      {
        if (db_result_set != null)
          db_result_set.close();
        if (SQL_statement != null)
          SQL_statement.close();
        if (db_connection != null)
          DatabaseConnectionManager.releaseConnection(db_connection);
      }
      catch (Exception exception)
      {
        Log.excp(this_servers_name, this_methods_name,
                 exception.toString());
      }
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  /**
   * This method creates a list of app information specifically for user
   * config GUI.
   * A user or server token is required for authentication.
   */

  public void  handleCreateUCappList()
  {
    Object         bundle_items[];
    String         msg_items[];
    int            msg_item_lengths[];
    String         SQL_query_string;
    Connection     db_connection = null;
    Statement      SQL_statement = null;
    ResultSet      db_result_set = null;
    Object         arg_list[];
    final String   err_header = "NOUCPROGROUPLIST";
    final String   this_methods_name = "handleCreateUCappList";

    try
    {
      if (isServerIdled() == true) {
         sendErrorMessage(err_header, ErrorCodes.ERROR_SERVER_IDLED);
         return;
      }
      
      msg_item_lengths = new int[2];
      msg_items = new String[2];

      msg_item_lengths[0] = Constants.MAX_LOGIN_NAME_LEN;
      msg_item_lengths[1] = Constants.MAX_TOKEN_LEN;
      if (parseData(msg_item_lengths, msg_items) == false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not parse incoming message!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_PARSE_MSG);
          return;
        }

      // Validate requestor's ID
      int n;
      if (msg_items[1].regionMatches(0, Constants.SERVER_TOKEN_STRING, 0,
                                     Constants.SERVER_TOKEN_STRING.length())
                            == true)
        n = doServerValidate(msg_items[0], msg_items[1], getClientAddress());
      else
        {
          IntegerBuffer junk = null;
          n = isUserValid(msg_items[0], msg_items[1], junk);
        }
      if (n < 0)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not validate user " + msg_items[0]
                    + " and/or token.");
          sendErrorMessage(err_header, n);
          return;
        }

      // Establish the DB connection.
      db_connection = DatabaseConnectionManager.getConnection();
      if (db_connection == null)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Null DB connection returned!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_ACCESS_DB);
          return;
        }

      SQL_query_string = "SELECT apps.app_title,hostname,app_lock,"
          + "version_number,release_date,role_name FROM apps,app_roles WHERE "
          + "apps.app_title=app_roles.app_title ORDER BY apps.app_title";

      // Run the query
      SQL_statement = db_connection.createStatement();
      db_result_set = SQL_statement.executeQuery(SQL_query_string);

      // Set up to bundle up the query results
      if (db_result_set.next() == false)
        {
          String message = "No database information!";
          sendFormattedServerMsg(err_header,
                                 StringLib.valueOf(message.length(), 5)
                                  + message);
          return;
        }

      String        role_name;
      String        cursor_app_title = null;
      String        previous_app_title = null;
      StringBuffer  response_string = new StringBuffer();

      //Assume we have a record since *.next() was already checked above..
      cursor_app_title = db_result_set.getString("app_title");
      do
        {
          // Assume new app.
          response_string.append(cursor_app_title + "," +
          db_result_set.getString("hostname") + "," +
          db_result_set.getString("version_number") + "," +
          //     db_result_set.getString("release_date") + "," +
          db_result_set.getString("release_date") + ",");
          if (db_result_set.getBoolean("app_lock"))
            response_string.append("Locked");
          else
            response_string.append("N/A");

          // Assuming all the rest of this app's records
          // immediately follow, take each record's role and append it to
          // the list of string we just concatenated.  We just keep looping
          // here until we encounter the records for a different app.
          do
            {
              role_name = db_result_set.getString("role_name");
              if (role_name.length() != 0)
                response_string.append("," + role_name);
              previous_app_title = cursor_app_title; //For future comparisons.
              if (db_result_set.next() != false)
                cursor_app_title = db_result_set.getString("app_title");
              else
                cursor_app_title = null; // break us out of both loops.
            }
          while (previous_app_title.equals(cursor_app_title));
          response_string.append("\n");
        }
      while (cursor_app_title != null);

      // Now we need to insert the(bundled) data size at the beginning of
      // the response message.
      bundle_items = new Object[1];
      StringBuffer  bundle_buffer = new StringBuffer();

      bundle_items[0] = new Integer(response_string.length());
      BundleLib.bundleData(bundle_buffer, "I", bundle_items);

      response_string.insert(0, bundle_buffer.toString());

      // Now bundle in the size of the new length...
      bundle_buffer.setLength(0);
      bundle_items = new Object[1];

      bundle_items[0] = new Integer(response_string.length());
      BundleLib.bundleData(bundle_buffer, "I", bundle_items);
      response_string.insert(0, bundle_buffer.toString());

      sendFormattedServerMsg("GOODUCPROGROUPLIST",
      response_string.toString());

      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg(err_header,
                             StringLib.valueOf(exception.toString().length(), 5)
                             + exception.toString());
      return;
    }
    finally
    {
      // Close the database resources
      try
      {
        if (db_result_set != null)
          db_result_set.close();
        if (SQL_statement != null)
          SQL_statement.close();
        if (db_connection != null)
          DatabaseConnectionManager.releaseConnection(db_connection);
      }
      catch (Exception exception)
      {
        Log.excp(this_servers_name, this_methods_name,
                 exception.toString());
      }
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  /**
   * This method relays information back to the client about the specified
   * user.  This is mainly used by the user config GUI.
   * A user or server token is required for authentication.
   */

  public void  handleGetUCuserInfo()
  {
    String         msg_items[];
    int            msg_item_lengths[];
    String         SQL_query_string;
    Connection     db_connection = null;
    Statement      SQL_statement1 = null;
    Statement      SQL_statement2 = null;
    ResultSet      db_result_set1 = null;
    ResultSet      db_result_set2 = null;
    StringBuffer   bundle_buffer;
    Object         arg_list[];
    Object         bundle_items[];
    final String   err_header = "NOUCUSERINFO";
    final String   this_methods_name = "handleGetUCuserInfo";

    try
    {
      if (isServerIdled() == true) {
         sendErrorMessage(err_header, ErrorCodes.ERROR_SERVER_IDLED);
         return;
      }
      
      msg_item_lengths = new int[3];
      msg_items = new String[3];

      msg_item_lengths[0] = Constants.MAX_LOGIN_NAME_LEN;
      msg_item_lengths[1] = Constants.MAX_TOKEN_LEN;
      msg_item_lengths[2] = Constants.MAX_LOGIN_NAME_LEN;
      if (parseData(msg_item_lengths, msg_items) == false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not parse incoming message!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_PARSE_MSG);
          return;
        }

      // Validate requestor's ID
      int n;
      if (msg_items[1].regionMatches(0, Constants.SERVER_TOKEN_STRING, 0,
                                     Constants.SERVER_TOKEN_STRING.length())
                            == true)
        n = doServerValidate(msg_items[0], msg_items[1], getClientAddress());
      else
        {
          IntegerBuffer junk = null;
          n = isUserValid(msg_items[0], msg_items[1], junk);
        }
      if (n < 0)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not validate user " + msg_items[0]
                     + " and/or token.");
          sendErrorMessage(err_header, n);
          return;
        }

      // Establish the DB connection.
      db_connection = DatabaseConnectionManager.getConnection();
      if (db_connection == null)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Null DB connection returned!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_ACCESS_DB);
          return;
        }

      arg_list = new Object[2];
      arg_list[0] = msg_items[2];             // login name passed in.
      SQL_query_string = StringLib.SQLformat(
              "SELECT login_name, real_name, ss_number, phone_number,"
               + "auth_server, unix_id, security_class, password FROM users "
               + "WHERE login_name=" + Constants.ST,
              arg_list);

      // Run the query
      SQL_statement1 = db_connection.createStatement();
      db_result_set1 = SQL_statement1.executeQuery(SQL_query_string);

      if (db_result_set1.next() == false)
        {
          Log.debug(this_servers_name, this_methods_name, "Client requested "
                     + " information for non-existant user: " + msg_items[2]);
          String message = "Unknown user!";
          sendFormattedServerMsg(err_header,
                                 StringLib.valueOf(message.length(), 5)
                                  + message);
          return;
        }

      // Put all this user's infromation into a single string...
      StringBuffer response_string = new StringBuffer();
      response_string.insert(0, msg_items[2] +(char) 1
          + db_result_set1.getString("real_name") + (char) 1
          + db_result_set1.getString("ss_number") + (char) 1
          + db_result_set1.getString("phone_number") +(char) 1
          + db_result_set1.getString("auth_server") + (char) 1
          + db_result_set1.getString("unix_id") + (char) 1
          + new Integer(db_result_set1.getInt("security_class")).toString()
          + (char) 1
          + db_result_set1.getString("password") + (char) 2);

      // Now get all of this user's apps.
      SQL_query_string = StringLib.SQLformat(
                            "SELECT app_title FROM app_users where login_name="
                             + Constants.ST, arg_list);

      db_result_set1 = SQL_statement1.executeQuery(SQL_query_string);
      if (db_result_set1.next() == false)
        {
          Log.debug(this_servers_name, this_methods_name, "User" + msg_items[2]
                    + " is not a memeber of any application!");
          // Bundle up what we have so far and send it..
          bundle_items = new Object[1];
          bundle_items[0] = new Integer(response_string.length());
          bundle_buffer = new StringBuffer();
          BundleLib.bundleData(bundle_buffer, "I", bundle_items);
          // Prepend this to the string..
          response_string.insert(0, bundle_buffer.toString());

          bundle_items[0] = new Integer(response_string.length());
          bundle_buffer = new StringBuffer();
          BundleLib.bundleData(bundle_buffer, "I", bundle_items);
          // Prepend this to the string..
          response_string.insert(0, bundle_buffer.toString());

          sendFormattedServerMsg("GOODUCUSERINFO", response_string.toString());
          return;
        }

      // For each app this user is a member of, get their app roles.
      boolean        first_role;
      String         cursor_app_title = null;
      SQL_statement2 = db_connection.createStatement();
      do
        {
          // Assume we're pointing at the first app..
          cursor_app_title = db_result_set1.getString("app_title");
          response_string.append(cursor_app_title + ",");

          arg_list[1] = cursor_app_title;    // login_name was already set.
          SQL_query_string = StringLib.SQLformat("SELECT role_name "
              + " FROM user_app_roles WHERE login_name=" + Constants.ST
              + " AND app_title=" + Constants.ST, arg_list);
          db_result_set2 = SQL_statement2.executeQuery(SQL_query_string);
          if (db_result_set2.next() == false)
            {
              Log.info(this_servers_name, this_methods_name, "App user " 
                        + arg_list[0] + " has no roles for app "
                        + cursor_app_title);
              response_string.append((char) 2);
              continue;  // on to the next app..
            }

          first_role = true;
          do
            {
              if (first_role)
                {
                  response_string.append(db_result_set2.getString("role_name"));
                  first_role = false;
                }
              else
                response_string.append(","
                                      + db_result_set2.getString("role_name"));
            }
          while (db_result_set2.next() != false);
          response_string.append((char) 2);  // terminate the roles string.
        }
      while (db_result_set1.next() != false);

      // Now bundle up some sizes...
      bundle_items = new Object[1];
      bundle_buffer = new StringBuffer();

      // This is the size of the string..
      bundle_items[0] = new Integer(response_string.length());
      BundleLib.bundleData(bundle_buffer, "I", bundle_items);

      // Prepend this to the string..
      response_string.insert(0, bundle_buffer.toString());

      // Now bundle in the size of the string + bundled string size...
      bundle_buffer.setLength(0);

      bundle_items[0] = new Integer(response_string.length());
      BundleLib.bundleData(bundle_buffer, "I", bundle_items);
      // Prepend this to the string..
      response_string.insert(0, bundle_buffer.toString());

      sendFormattedServerMsg("GOODUCUSERINFO", response_string.toString());

      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg(err_header,
                             StringLib.valueOf(exception.toString().length(), 5)
                             + exception.toString());
      return;
    }
    finally
    {
      // Close the database resources
      try
      {
        if (db_result_set1 != null)
          db_result_set1.close();
        if (db_result_set2 != null)
          db_result_set2.close();
        if (SQL_statement1 != null)
          SQL_statement1.close();
        if (SQL_statement2 != null)
          SQL_statement2.close();
        if (db_connection != null)
          DatabaseConnectionManager.releaseConnection(db_connection);
      }
      catch (Exception exception)
      {
        Log.excp(this_servers_name, this_methods_name,
                 exception.toString());
      }
    }
  }


  /////////////////////////////////////////////////////////////////////////////
   /**
    * This method returns a list of printer information according to specified
    * printer-type filters.
    * A user or server token is required for authentication.
    */

  public void  handleGetUsersPrinterList()
  {
     String         msg_items[];
     int            msg_item_lengths[];
     final String   this_methods_name = "handleGetUsersPrinterList";
     final String   err_header = "NOPRTLIST";
     int            securityClass;
     boolean        defaultPrtOnly;
     boolean        generalAccessPtrs;
     int            n;
     StringBuffer   result;
     Connection     connection = null;
     Statement      statement = null;
     Statement      statement2 = null;
     ResultSet      resultSet = null;
     ResultSet      resultSet2 = null;
     Object         sqlItems[];
     String         appHostname;
     int            numPrinters;
     Object         bundledItems[];     
     StringBuffer   tmpBuf;
     String         SQLstring;   
     String         copyFiles;
     
     try {
        
        // See if the server is idled
        if (isServerIdled() == true) {
           sendErrorMessage(err_header, ErrorCodes.ERROR_SERVER_IDLED);
           return;
        }
        
        msg_item_lengths = new int[7];
        msg_items = new String[7];
        msg_item_lengths[0] = Constants.MAX_LOGIN_NAME_LEN;
        msg_item_lengths[1] = Constants.MAX_TOKEN_LEN;
        msg_item_lengths[2] = Constants.MAX_PROGROUP_TITLE_LEN;
        msg_item_lengths[3] = Constants.MAX_OUT_TYPE_TITLE_LEN;
        msg_item_lengths[4] = 1;     // security class
        msg_item_lengths[5] = 1;     // wants default printer only
        msg_item_lengths[6] = 1;     // ==1 when only want general_access prtr.

        if (parseData(msg_item_lengths, msg_items) == false) {        
           Log.error(this_servers_name, this_methods_name,
                    "Could not parse incoming message!");
           sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_PARSE_MSG);
           return;
        }
        
        securityClass = StringLib.parseInt(msg_items[4], Constants.UNCLASSIFIED_CLEARANCE);
        defaultPrtOnly = (StringLib.parseInt(msg_items[5], -1) != 1) ? false : true;
        generalAccessPtrs = (StringLib.parseInt(msg_items[6], -1) != 1) ? false : true;
        
        // Validate the user
        if (msg_items[1].regionMatches(0, Constants.SERVER_TOKEN_STRING, 0,
                                       Constants.SERVER_TOKEN_STRING.length()) == true)
           n = doServerValidate(msg_items[0], msg_items[1], getClientAddress());
        else        
           n = isUserValid(msg_items[0], msg_items[1], null);        
        
        if (n < 0) {        
          Log.error(this_servers_name, this_methods_name,
                    "Could not validate user " + msg_items[0] + " and/or token.");
          sendErrorMessage(err_header, n);
          return;
        }
        
        // Get a database connection.        
        if ((connection = DatabaseConnectionManager.getConnection()) == null)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Null DB connection returned!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_ACCESS_DB);
          return;
        }
        statement = connection.createStatement();
        
        // Get the hostname that the given app is on
        sqlItems = new Object[1];
        sqlItems[0] = msg_items[2];
        resultSet = statement.executeQuery(StringLib.SQLformat("SELECT " + 
             "hostname FROM apps WHERE app_title = " + Constants.ST, sqlItems));
        resultSet.next();
        appHostname = resultSet.getString("hostname");                
                
        // Query a list of printers from the database
        if (generalAccessPtrs) {
           sqlItems = new Object[3];   // OutputType
           sqlItems[0] = msg_items[0]; // Login
           sqlItems[1] = new Integer(securityClass);
           sqlItems[2] = new Boolean(true);
           SQLstring = StringLib.SQLformat("SELECT " + 
                       "printer_name, location, driver_name, printer_type, " +
                       "queue_enabled, printer_enabled, printing_method, " + 
                       "file_name, redirection_ptr, nt_host_name, nt_share_name " +
                     "FROM printers, users " +
                     "WHERE " +
                           "driver_name != null AND " +
                           "printer_type != null AND " +
                           "users.login_name = " + Constants.ST + " AND " +
                           Constants.ST + " <= printers.security_class AND " +
                           "printers.security_class <= users.security_class AND " +
                           "general_access = " + Constants.ST, sqlItems);           
        }
        else {
           if (defaultPrtOnly == true) {
/*              
              sqlItems = new Object[4];
              sqlItems[0] = msg_items[3];
              sqlItems[1] = msg_items[2];
              sqlItems[2] = msg_items[0];
              sqlItems[3] = new Integer(securityClass);
              SQLstring = StringLib.SQLformat("SELECT " +
                   "printers.printer_name, location, driver_name, " +
                   "printer_type, queue_enabled, printer_enabled, " +
                   "printing_method, file_name, redirection_ptr " +
                 "FROM output_types, printers, users " +
                 "WHERE " +
                 "output_types.otype_title = " + Constants.ST + " AND " +
                 "output_types.app_title = " + Constants.ST + " AND " + 
                 "output_types.default_printer = printers.printer_name AND " +
                 "printers.driver_name != null AND " + 
                 "printers.printer_type != null AND " +
                 "users.login_name = " + Constants.ST + " AND " +
                 Constants.ST + " <= printers.security_class AND " +
                 "printers.security_class <= users.security_class", sqlItems);
*/
              sqlItems = new Object[5];
              sqlItems[0] = msg_items[3]; // OutputType
              sqlItems[1] = msg_items[2]; // AppTitle
              sqlItems[2] = new Boolean(true); 
              sqlItems[3] = msg_items[0]; // Login
              sqlItems[4] = new Integer(securityClass);
              SQLstring = StringLib.SQLformat("SELECT " +
                   "printers.printer_name, location, driver_name, " +
                   "printer_type, queue_enabled, printer_enabled, " +
                   "printing_method, file_name, redirection_ptr, nt_host_name, "
                   + "nt_share_name " +
                 "FROM output_printers, printers, users " +
                 "WHERE " +
                 "output_printers.otype_title = " + Constants.ST + " AND " +
                 "output_printers.app_title = " + Constants.ST + " AND " + 
                 "output_printers.default_printer = " + Constants.ST + " AND " +
                 "output_printers.printer_name = printers.printer_name AND " +
                 "printers.driver_name != null AND " + 
                 "printers.printer_type != null AND " +
                 "users.login_name = " + Constants.ST + " AND " +
                 Constants.ST + " <= printers.security_class AND " +
                 "printers.security_class <= users.security_class", sqlItems);
           }
           else {
/*              
              sqlItems = new Object[4];
              sqlItems[0] = msg_items[3]; // OutputType
              sqlItems[1] = msg_items[2]; // AppTitle
              sqlItems[2] = msg_items[0]; // Login
              sqlItems[3] = new Integer(securityClass);
              SQLstring = StringLib.SQLformat("SELECT printers.printer_name, " +
                   "location, driver_name, printer_type, queue_enabled, " +
                   "printer_enabled, printing_method, file_name, " +
                   "printers.printer_name = output_types.default_printer as default_printer, " +
                   "redirection_ptr " +
                "FROM output_printers, printers, users, output_types " +
                "WHERE " + 
                   "output_printers.otype_title = " + Constants.ST + " AND " +
                   "output_printers.app_title = " + Constants.ST + " AND " +
                   "output_printers.printer_name = printers.printer_name AND " +
                   "printers.driver_name != null AND " + 
                   "printers.printer_type != null AND " + 
                   "users.login_name = " + Constants.ST + " AND " +
                   Constants.ST + " <= printers.security_class AND " +
                   "printers.security_class <= users.security_class AND " +
                   "output_printers.otype_title = output_types.otype_title AND " +
                   "output_printers.app_title = output_types.app_title", sqlItems);
 */
              sqlItems = new Object[4];
              sqlItems[0] = msg_items[3]; // OutputType
              sqlItems[1] = msg_items[2]; // AppTitle
              sqlItems[2] = msg_items[0]; // Login
              sqlItems[3] = new Integer(securityClass);
              SQLstring = StringLib.SQLformat("SELECT printers.printer_name, " +
                   "location, driver_name, printer_type, queue_enabled, " +
                   "printer_enabled, printing_method, file_name, " +
                   "default_printer, redirection_ptr, nt_host_name, nt_share_name " +
                "FROM output_printers, printers, users " +
                "WHERE " + 
                   "output_printers.otype_title = " + Constants.ST + " AND " +
                   "output_printers.app_title = " + Constants.ST + " AND " +
                   "output_printers.printer_name = printers.printer_name AND " +
                   "printers.driver_name != null AND " + 
                   "printers.printer_type != null AND " + 
                   "users.login_name = " + Constants.ST + " AND " +
                   Constants.ST + " <= printers.security_class AND " +
                   "printers.security_class <= users.security_class", sqlItems);
           }
        }                
        
        Log.debug(this_servers_name, this_methods_name, SQLstring);
                
        // Bundle up the data for each printer
        bundledItems = new Object[17];
        result = new StringBuffer(0);
        numPrinters = 0;
        
        resultSet = statement.executeQuery(SQLstring);
        while (resultSet.next() == true) {
           
           if (statement2 == null)
              statement2 = connection.createStatement();
           
           // Get the driver_name of the printer's printer_type
           sqlItems = new Object[2];
           sqlItems[0] = resultSet.getString("printer_type");
           resultSet2 = statement2.executeQuery(StringLib.SQLformat("SELECT " +
                       "driver_name FROM printer_types WHERE printer_type = " + 
                       Constants.ST, sqlItems));
           resultSet2.next();
           
           bundledItems[0] = resultSet.getString("printer_name");
           bundledItems[1] = appHostname;
           bundledItems[2] = resultSet.getString("location");
           
           if (generalAccessPtrs == true)
              bundledItems[3] = new Integer(0);  
           else {
              if (defaultPrtOnly == true)
                 bundledItems[3] = new Integer(1);  
              else
                 bundledItems[3] = resultSet.getBoolean("default_printer") ? new Integer(1) : new Integer(0);  
           }
                      
           bundledItems[4] = resultSet.getString("driver_name");
           bundledItems[5] = resultSet2.getString("driver_name");
           bundledItems[6] = resultSet.getBoolean("queue_enabled") ? new Integer(1) : new Integer(0);
           bundledItems[7] = resultSet.getBoolean("printer_enabled") ? new Integer(1) : new Integer(0);
           if ((bundledItems[8] = resultSet.getString("redirection_ptr")) == null)
             bundledItems[8] = "***"; 
           
           // Get the driver files
           sqlItems[0] = resultSet.getString("driver_name");
           sqlItems[1] = new Integer(Constants.DRIVER_DRIVER_TYPE);
           resultSet2 = statement2.executeQuery(StringLib.SQLformat("SELECT " +
               "driver_filename FROM print_driver_files WHERE driver_name = " +  
               Constants.ST + " AND type = " + Constants.ST, sqlItems));
           if (resultSet2.next() == true)           
              bundledItems[9] = resultSet2.getString("driver_filename");
           else
              bundledItems[9] = "NONE";
           
           sqlItems[1] = new Integer(Constants.DATA_DRIVER_TYPE);
           resultSet2 = statement2.executeQuery(StringLib.SQLformat("SELECT " +
               "driver_filename FROM print_driver_files WHERE driver_name = " +  
               Constants.ST + " AND type = " + Constants.ST, sqlItems));
           if (resultSet2.next() == true)           
              bundledItems[10] = resultSet2.getString("driver_filename");
           else
              bundledItems[10] = "NONE";
           
           sqlItems[1] = new Integer(Constants.UI_DRIVER_TYPE);
           resultSet2 = statement2.executeQuery(StringLib.SQLformat("SELECT " +
               "driver_filename FROM print_driver_files WHERE driver_name = " +  
               Constants.ST + " AND type = " + Constants.ST, sqlItems));
           if (resultSet2.next() == true)           
              bundledItems[11] = resultSet2.getString("driver_filename");
           else
              bundledItems[11] = "NONE";
           
           sqlItems[1] = new Integer(Constants.HELP_DRIVER_TYPE);
           resultSet2 = statement2.executeQuery(StringLib.SQLformat("SELECT " +
               "driver_filename FROM print_driver_files WHERE driver_name = " +  
               Constants.ST + " AND type = " + Constants.ST, sqlItems));
           if (resultSet2.next() == true)           
              bundledItems[12] = resultSet2.getString("driver_filename");
           else
              bundledItems[12] = "NONE";
           
           sqlItems[1] = new Integer(Constants.MONITOR_DRIVER_TYPE);
           resultSet2 = statement2.executeQuery(StringLib.SQLformat("SELECT " +
               "driver_filename FROM print_driver_files WHERE driver_name = " +  
               Constants.ST + " AND type = " + Constants.ST, sqlItems));
           if (resultSet2.next() == true)           
              bundledItems[13] = resultSet2.getString("driver_filename");
           else
              bundledItems[13] = "NONE";
                      
           resultSet2 = statement2.executeQuery(StringLib.SQLformat("SELECT " +
               "nt_monitor_name FROM print_drivers WHERE driver_name = " +  
               Constants.ST, sqlItems));
           resultSet2.next();
           if ((bundledItems[14] = resultSet2.getString("nt_monitor_name")) == null)                         
              bundledItems[14] = "NONE";
           
           sqlItems[1] = new Integer(Constants.NO_DRIVER_TYPE);
           resultSet2 = statement2.executeQuery(StringLib.SQLformat("SELECT " +
               "driver_filename FROM print_driver_files WHERE driver_name = " +  
               Constants.ST + " AND type = " + Constants.ST, sqlItems));
           copyFiles = null;
           while (resultSet2.next() == true) {
              if (copyFiles == null)
                 copyFiles = resultSet2.getString("driver_filename");
              else
                 copyFiles += "," + resultSet2.getString("driver_filename");
           }
              
           bundledItems[15] = copyFiles;
           
           if (resultSet.getString("printing_method").equals(Constants.FILE_PRINTER) == true)
              bundledItems[16] = resultSet.getString("file_name");
           else
              bundledItems[16] = " ";
           
         //Check if we're NT & IF printing method is "SHARE"
        //we need to put hostname&share name in PLACE OF the hostname... also
        //TODO: we don't need to send drivers if NT either
      
           if(Constants.IS_WINDOWS_OS &&
              resultSet.getString("printing_method").equals("NTSHARE")) {
               //bundledItems[0] = resultSet.getString("nt_share_name");
               bundledItems[1] = resultSet.getString("nt_host_name") + "," +
               resultSet.getString("nt_share_name");
           }

           // Bundle the collected data for this printers
           BundleLib.bundleData(result, "CCCICCIICCCCCCCCC", bundledItems);           
           numPrinters++;
        }        
        
        Log.debug(this_servers_name, this_methods_name, "Number of printers <" + 
                  numPrinters + ">");
        
        // Format the return message
        
        tmpBuf = new StringBuffer(0);
        bundledItems[0] = new Integer(numPrinters);
        BundleLib.bundleData(tmpBuf, "I", bundledItems);
        result.insert(0, tmpBuf.toString());
        
        tmpBuf.setLength(0);
        bundledItems[0] = new Integer(result.length());
        BundleLib.bundleData(tmpBuf, "I", bundledItems);
        result.insert(0, tmpBuf.toString());
                
        sendFormattedServerMsg("GOODPRTLIST", result.toString() + "\0");        
     }
     catch (Exception exception) {
        Log.excp(this_servers_name, this_methods_name, exception.toString());
        sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_CREATE_QUERY);
     }
     finally {
        try {
           if (resultSet2 != null)
              resultSet2.close();                     
           if (resultSet != null)
              resultSet.close();          
           if (statement2 != null)
              statement2.close();
           if (statement != null)
              statement.close();
           if (connection != null)
              DatabaseConnectionManager.releaseConnection(connection);
        }
        catch (Exception exception) {
        }
     }
  }  

  /////////////////////////////////////////////////////////////////////////////
  /**
   * This method gets a lsit of all printers and associated apps for the SPQ
   * server.
   * A user token is required for authentication.
   */

  public void  handleGetPrinterList()
  {
    final String   err_header = "NOPRTLIST";
    final String   this_methods_name = "handleGetPrinterList";
    Connection     db_connection = null;
    Statement      SQL_statement = null;
    Statement      SQL_statement2 = null;
    ResultSet      db_result_set = null;
    ResultSet      db_result_set2 = null;

    try
    {
      if (isServerIdled() == true) {
         sendErrorMessage(err_header, ErrorCodes.ERROR_SERVER_IDLED);
         return;
      }
      
      int  msg_item_lengths[]  = new int[4];
      String msg_items[]  = new String[4];

      msg_item_lengths[0] = Constants.MAX_LOGIN_NAME_LEN;
      msg_item_lengths[1] = Constants.MAX_TOKEN_LEN;
      msg_item_lengths[2] = Constants.MAX_PROGROUP_TITLE_LEN;
      msg_item_lengths[3] = 1;   // ==1 when only want general_access prtr.

      if (parseData(msg_item_lengths, msg_items) == false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not parse incoming message!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_PARSE_MSG);
          return;
        }

      // Validate requestor's ID
      IntegerBuffer junk = null;
      int n = isUserValid(msg_items[0], msg_items[1], junk);
      if (n < 0)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not validate user " + msg_items[0]
                     + " and/or token.");
          sendErrorMessage(err_header, n);
          return;
        }

      // Establish the DB connection.
      db_connection = DatabaseConnectionManager.getConnection();
      if (db_connection == null)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Null DB connection returned!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_ACCESS_DB);
          return;
        }

      Object arg_list[] = new Object[2];
      arg_list[0] = msg_items[2];  // app_name
      arg_list[1] = msg_items[2];  // app_name

      // Create the core of the SQL statement..
      StringBuffer SQL_query_buffer = new StringBuffer();
      SQL_query_buffer.insert(0, StringLib.SQLformat(
                "SELECT a.printer_name,location,queue_enabled,printer_enabled,"
                 + " hostname,redirection_ptr,file_name, d.driver_name"
                 + " FROM app_printers a, apps b, printers c, printer_types d"
                 + " WHERE a.app_title=" + Constants.ST
                 + " AND b.app_title=" + Constants.ST
                 + " AND c.printer_name=a.printer_name"
                 + " AND d.printer_type=c.printer_type",
                arg_list));

      if (msg_items[3].equals("1"))       // Only get general access printers
        SQL_query_buffer.append(" AND general_access");
      else
        SQL_query_buffer.append(" AND general_access=false");

      // Run the query
      SQL_statement = db_connection.createStatement();
      SQL_statement2 = db_connection.createStatement();

      db_result_set = SQL_statement.executeQuery(SQL_query_buffer.toString());
      if (db_result_set.next() == false)
        {
          // Return an empty printer list..
          sendFormattedServerMsg("GOODPRTLIST", "10\0");
          return;
        }

      // Set up to bundle up the query results
      StringBuffer  response_string = new StringBuffer();
      String        temp;
      String        driver_name;
      StringBuffer  bundle_buffer = new StringBuffer();
      int           printer_count = 0;

      // 8 objects should be enough to reuse through the rest of the code.
      Object  bundle_items[] = new Object[8];
      String  SQL_query_string;
      // Assume we have a record since *.next() was already checked above..
      do
        {
          driver_name = db_result_set.getString("driver_name");
          if (driver_name == null)
            continue; //skip this printer and look at next one.

          bundle_items[0] = db_result_set.getString("printer_name");
          bundle_items[1] = db_result_set.getString("hostname");
          bundle_items[2] = db_result_set.getString("location");
          bundle_items[3] = driver_name; // driver_name;
          // This seems backwards but may be there for backwards compatibility
          if (db_result_set.getBoolean("queue_enabled"))
            bundle_items[4] = new Integer(1);
          else
            bundle_items[4] = new Integer(0);
          if (db_result_set.getBoolean("printer_enabled"))
            bundle_items[5] = new Integer(1);
          else
            bundle_items[5] = new Integer(0);
          // Add in the printer redirect field..
          temp = db_result_set.getString("redirection_ptr");
          if (temp == null)
            bundle_items[6] = "***";
          else
            bundle_items[6] = temp;

          //Bundle what we just got..
          bundle_buffer.setLength(0);
          BundleLib.bundleData(bundle_buffer, "CCCCIIC", bundle_items);
          response_string.append(bundle_buffer.toString());

          // Get the driver filename...
          arg_list[0] = driver_name;
          arg_list[1] = new Integer(Constants.DRIVER_DRIVER_TYPE);
          SQL_query_string = StringLib.SQLformat(
                "SELECT driver_filename FROM print_driver_files WHERE "
                 + "driver_name=" + Constants.ST + " AND type=" + Constants.ST,
                arg_list);

          db_result_set2 = SQL_statement2.executeQuery(SQL_query_string);
          if (db_result_set2.next() == true)
            bundle_items[0] = db_result_set2.getString("driver_filename");
          else
            bundle_items[0] = "NONE";
          bundle_buffer.setLength(0);
          BundleLib.bundleData(bundle_buffer, "C", bundle_items);
          response_string.append(bundle_buffer.toString());

          // Get the data filename...
          arg_list[1] = new Integer(Constants.DATA_DRIVER_TYPE);
          SQL_query_string = StringLib.SQLformat(
                 "SELECT driver_filename FROM print_driver_files WHERE "
                  + "driver_name=" + Constants.ST + " AND type=" + Constants.ST,
                 arg_list);

          db_result_set2 = SQL_statement2.executeQuery(SQL_query_string);
          if (db_result_set2.next() == true)
            bundle_items[0] = db_result_set2.getString("driver_filename");
          else
            bundle_items[0] = "NONE";
          bundle_buffer.setLength(0);
          BundleLib.bundleData(bundle_buffer, "C", bundle_items);
          response_string.append(bundle_buffer.toString());

          // Get the ui filename...
          arg_list[1] = new Integer(Constants.UI_DRIVER_TYPE);
          SQL_query_string = StringLib.SQLformat(
                 "SELECT driver_filename FROM print_driver_files WHERE "
                  + "driver_name=" + Constants.ST + " AND type=" + Constants.ST,
                 arg_list);

          db_result_set2 = SQL_statement2.executeQuery(SQL_query_string);
          if (db_result_set2.next() == true)
            bundle_items[0] = db_result_set2.getString("driver_filename");
          else
            bundle_items[0] = "NONE";
          bundle_buffer.setLength(0);
          BundleLib.bundleData(bundle_buffer, "C", bundle_items);
          response_string.append(bundle_buffer.toString());

          // Get the help filename...
          arg_list[1] = new Integer(Constants.HELP_DRIVER_TYPE);
          SQL_query_string = StringLib.SQLformat(
                "SELECT driver_filename FROM print_driver_files WHERE "
                 + "driver_name=" + Constants.ST + " AND type=" + Constants.ST,
                arg_list);

          db_result_set2 = SQL_statement2.executeQuery(SQL_query_string);
          if (db_result_set2.next() == true)
            bundle_items[0] = db_result_set2.getString("driver_filename");
          else
            bundle_items[0] = "NONE";
          bundle_buffer.setLength(0);
          BundleLib.bundleData(bundle_buffer, "C", bundle_items);
          response_string.append(bundle_buffer.toString());

          // Get the monitor filename...
          arg_list[1] = new Integer(Constants.MONITOR_DRIVER_TYPE);
          SQL_query_string = StringLib.SQLformat(
                 "SELECT driver_filename FROM print_driver_files WHERE "
                  + "driver_name=" + Constants.ST + " AND type=" + Constants.ST,
                 arg_list);
          db_result_set2 = SQL_statement2.executeQuery(SQL_query_string);
          if (db_result_set2.next() == true)
            bundle_items[0] = db_result_set2.getString("driver_filename");
          else
            bundle_items[0] = "NONE";
          bundle_buffer.setLength(0);
          BundleLib.bundleData(bundle_buffer, "C", bundle_items);
          response_string.append(bundle_buffer.toString());

          // Get the NT monitor name...
          SQL_query_string = StringLib.SQLformat(
                          "SELECT nt_monitor_name FROM print_drivers WHERE "
                           + "driver_name=" + Constants.ST,
                          arg_list);

          db_result_set2 = SQL_statement2.executeQuery(SQL_query_string);
          if (db_result_set2.next() == true)
            bundle_items[0] = db_result_set2.getString("nt_monitor_name");
          else
            bundle_items[0] = "NONE";
          bundle_buffer.setLength(0);
          BundleLib.bundleData(bundle_buffer, "C", bundle_items);
          response_string.append(bundle_buffer.toString());

          // Now get all the extra driver files required.  These files will
          // not have a type associated with them.
          SQL_query_string = StringLib.SQLformat(
                "SELECT driver_filename FROM print_driver_files WHERE "
                 + "driver_name=" + Constants.ST + "AND type=null", arg_list);

          db_result_set2 = SQL_statement2.executeQuery(SQL_query_string);
          StringBuffer temp_buff = new StringBuffer();
          if (db_result_set2.next() == false)
            bundle_items[0] = "NONE";
          else
            {
              // Build a comma delimited list of the filenames.
              do
                {
                  if (temp_buff.length() != 0)
                    temp_buff.append(",");
                  temp_buff.append(db_result_set2.getString("driver_filename"));
                }
              while (db_result_set2.next() == true);
              bundle_items[0] = temp_buff.toString();
            }
          // bundle up the list of extraneous filenames we just created..
          bundle_buffer.setLength(0);
          BundleLib.bundleData(bundle_buffer, "C", bundle_items);
          response_string.append(bundle_buffer.toString());

          temp = db_result_set.getString("file_name");
          if (temp == null)
            bundle_items[0] = "NONE";
          else
            bundle_items[0] = temp;
          bundle_buffer.setLength(0);
          BundleLib.bundleData(bundle_buffer, "C", bundle_items);
          response_string.append(bundle_buffer.toString());

          printer_count++;
        }
      while (db_result_set.next() == true);

      // Prepend in the(bundled) number of printers found..
      bundle_items[0] = new Integer(printer_count);
      bundle_buffer.setLength(0);
      BundleLib.bundleData(bundle_buffer, "I", bundle_items);
      response_string.insert(0, bundle_buffer.toString());

      /*
  // Prepend the(bundled) size of the string..
  bundle_items[0] = new Integer(response_string.length());
  bundle_buffer.setLength(0);
  BundleLib.bundleData(bundle_buffer, "I", bundle_items);
  response_string.insert(0, bundle_buffer.toString());
       */
      response_string.append("\0");  // Cap off the response string..

      sendFormattedServerMsg("GOODPRTLIST", response_string.toString());

      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg(err_header,
                             StringLib.valueOf(exception.toString().length(), 5)
                             + exception.toString());
      return;
    }
    finally
    {
      // Close the database resources
      try
      {
        if (db_result_set != null)
          db_result_set.close();
        if (SQL_statement != null)
          SQL_statement.close();
        if (db_result_set2 != null)
          db_result_set2.close();
        if (SQL_statement2 != null)
          SQL_statement2.close();
        if (db_connection != null)
          DatabaseConnectionManager.releaseConnection(db_connection);
      }
      catch (Exception exception)
      {
        Log.excp(this_servers_name, this_methods_name,
                 exception.toString());
      }
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  /**
   * This method creates or sends(or both) a(cache) file containing all
   * the user information initially required by the User Config GUI.
   * A server or user token is required for authentication.
   */

  public void  handleGetUCUserList()
  {
    Object         bundle_items[];
    String         msg_items[];
    int            msg_item_lengths[];
    String         SQL_query_string;
    Connection     db_connection = null;
    Statement      SQL_statement = null;
    ResultSet      db_result_set = null;
    Object         arg_list[];
    final String   err_header = "NOUCUSERLIST";
    final String   this_methods_name = "handleGetUCUserList";

    try
    {
      if (isServerIdled() == true) {
         sendErrorMessage(err_header, ErrorCodes.ERROR_SERVER_IDLED);
         return;
      }
      
      msg_item_lengths = new int[3];
      msg_items = new String[3];

      msg_item_lengths[0] = Constants.MAX_LOGIN_NAME_LEN;
      msg_item_lengths[1] = Constants.MAX_TOKEN_LEN;
      msg_item_lengths[2] = 2;
      if (parseData(msg_item_lengths, msg_items) == false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not parse incoming message!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_PARSE_MSG);
          return;
        }

      // Validate requestor's ID
      int n;
      if (msg_items[1].regionMatches(0, Constants.SERVER_TOKEN_STRING, 0,
                                     Constants.SERVER_TOKEN_STRING.length())
                              == true)
        n = doServerValidate(msg_items[0], msg_items[1], getClientAddress());
      else
        {
          IntegerBuffer junk = null;
          n = isUserValid(msg_items[0], msg_items[1], junk);
        }
      if (n < 0)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not validate user " + msg_items[0]
                     + " and/or token.");
          sendErrorMessage(err_header, n);
          return;
        }

      // This section was sychronized since we didn't want one admin reading
      // the cache file while another was rewriting it.
      synchronized (db_lock)
      {
        if (msg_items[2].equals("1"))  // Recreate only if it doesn't exist..
          {
            // Only create a new chace file if it doesn't already exist.
            File cache_file = new File(Constants.USER_CONFIG_USERS_CACHE_FILE);
            if (!cache_file.exists())
              {
                // Cache file doesn't exist so we create it..
                StringBuffer data_buffer = new StringBuffer();
                Log.debug(this_servers_name, this_methods_name, "Creating a new"
                           + " user config cache file of users.");
                n = CreateUCUserCacheFile(data_buffer);
                if (n == 0)
                  sendFormattedServerMsg("GOODUCUSERLIST", data_buffer.toString());
                else
                  sendErrorMessage(err_header, n);
                return;
              }
            Log.debug(this_servers_name, this_methods_name, "Using existing"
                       + " user config cache file.");
            // Otherwise we just send the pre-existing cache file..
            sendServerMsgFile("GOODUCUSERLIST", "",
                              Constants.USER_CONFIG_USERS_CACHE_FILE, 0);
          }
        // Recreate a fresh file, but respond immediately.
        if (msg_items[2].equals("2"))
          {
            // Client doesn't want to wait on us...
            sendFormattedServerMsg("GOODUCUSERLIST", "00000");
            Log.debug(this_servers_name, this_methods_name, "Creating a new"
                       + " user config cache file of users.");
            n = CreateUCUserCacheFile(null);
            return;
          }
        // Recreate a fresh file, but return result.
        if (msg_items[2].equals("3"))
          {
            StringBuffer data_buffer = new StringBuffer();
            Log.debug(this_servers_name, this_methods_name, "Creating a new"
                       + " user config cache file of users.");
            n = CreateUCUserCacheFile(data_buffer);
            if (n == 0)
              sendFormattedServerMsg("GOODUCUSERLIST", data_buffer.toString());
            else
              sendErrorMessage(err_header, n);
            return;
          }
      }  // End of synchronized section.
      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg(err_header,
                             StringLib.valueOf(exception.toString().length(), 5)
                             + exception.toString());
      return;
    }
    finally
    {
      // Close the database resources
      try
      {
        if (db_result_set != null)
          db_result_set.close();
        if (SQL_statement != null)
          SQL_statement.close();
        if (db_connection != null)
          DatabaseConnectionManager.releaseConnection(db_connection);
      }
      catch (Exception exception)
      {
        Log.excp(this_servers_name, this_methods_name,
                 exception.toString());
      }
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  /**
   * This method puts the given information in the ntprint.ini file.
   * A user or server token is required for authentication.
   */

  public void  handlePutPrinterInINI()
  {
    final String   err_header = "NONEWNTPRINTER";
    String         msg_items[];
    int            msg_item_lengths[];
    final String   this_methods_name = "handlePutPrinterInINI";

    try
    {
      msg_item_lengths = new int[10];
      msg_items = new String[10];

      msg_item_lengths[0] = Constants.MAX_LOGIN_NAME_LEN;
      msg_item_lengths[1] = Constants.MAX_TOKEN_LEN;
      msg_item_lengths[2] = Constants.MAX_PRT_DEVICE_NAME_LEN;  // printer name
      msg_item_lengths[3] = Constants.MAX_PRT_FILE_LEN;  // driver file
      msg_item_lengths[4] = Constants.MAX_PRT_FILE_LEN;  // UI file
      msg_item_lengths[5] = Constants.MAX_PRT_FILE_LEN;  // data file
      msg_item_lengths[6] = Constants.MAX_PRT_FILE_LEN;  // help file
      msg_item_lengths[7] = Constants.MAX_PRT_FILE_LEN;  // monitor file
      msg_item_lengths[8] = Constants.MAX_CMD_LINE_ARGS_LEN;  // monitor name
      msg_item_lengths[9] = Constants.MAX_CMD_LEN;  // additional driver files

      if (parseData(msg_item_lengths, msg_items) == false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not parse incoming message!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_PARSE_MSG);
          return;
        }

      // Validate requestor's ID
      int n;
      if (msg_items[1].regionMatches(0, Constants.SERVER_TOKEN_STRING, 0,
                                     Constants.SERVER_TOKEN_STRING.length())
                            == true)
        n = doServerValidate(msg_items[0], msg_items[1], getClientAddress());
      else
        {
          IntegerBuffer junk = null;
          n = isUserValid(msg_items[0], msg_items[1], junk);
        }
      if (n < 0)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not validate user " + msg_items[0]
                     + " and/or token.");
          sendErrorMessage(err_header, n);
          return;
        }

      // Dump the message contents to the NT printers INI file...
      INI ini = new INI();
      ini.WriteProfileString("PRINTERS", msg_items[2], msg_items[2],
                             Constants.NT_INI_FILE);
      ini.WriteProfileString(msg_items[2], "DRIVER", msg_items[3],
                             Constants.NT_INI_FILE);
      ini.WriteProfileString(msg_items[2], "UI", msg_items[4],
                             Constants.NT_INI_FILE);
      ini.WriteProfileString(msg_items[2], "DATA", msg_items[5],
                             Constants.NT_INI_FILE);
      ini.WriteProfileString(msg_items[2], "HELP", msg_items[6],
                             Constants.NT_INI_FILE);
      ini.WriteProfileString(msg_items[2], "MONITOR", msg_items[7],
                             Constants.NT_INI_FILE);
      ini.WriteProfileString(msg_items[2], "MONITORNAME", msg_items[8],
                             Constants.NT_INI_FILE);

      // See if additional driver files need to be parsed out..
      if (!msg_items[2].equals("NOCOPYFILES"))
        {
          int      i;
          int      token_number = 1;
          boolean  found;
          String   filename;
          StringTokenizer  driverstring = new StringTokenizer(msg_items[9], ",",
          false);
          // Write out each extra filename as a name-value pair...
          while (driverstring.hasMoreTokens())
            {
              filename = driverstring.nextToken();
              found = false;
              // First check to make sure this filename wasn't already specified
              for (i = 3; i < 9; i++)
                {
                  if (msg_items[i].equals(filename))
                    {
                      found = true;
                      break;
                    }
                }
              if (found == false)
                ini.WriteProfileString(msg_items[2],
                                       Constants.COPY_FILE_PREFIX + "_"
                                        + token_number,
                                       filename, Constants.NT_INI_FILE);
              token_number++;
            }
        }

      ini.FlushINIFile(Constants.NT_INI_FILE);

      sendFormattedServerMsg("GOODNEWNTPRINTER", "");
      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg(err_header,
                             StringLib.valueOf(exception.toString().length(), 5)
                             + exception.toString());
      return;
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  /**
   * This method registers the specified appuser.
   * A user token is required for authentication.
   */

  public void  handleAppuserRegistration()
  {
    String         msg_items[];
    int            msg_item_lengths[];
    String         SQL_query_string;
    Connection     db_connection = null;
    Statement      SQL_statement = null;
    Object         arg_list[];
    final String   err_header = "NOREGAPPUSER";
    final String   this_methods_name = "handleAppuserRegistration";

    try
    {
      if (isServerIdled() == true) {
         sendErrorMessage(err_header, ErrorCodes.ERROR_SERVER_IDLED);
         return;
      }
      
      msg_item_lengths = new int[5];
      msg_items = new String[5];

      msg_item_lengths[0] = Constants.MAX_LOGIN_NAME_LEN;
      msg_item_lengths[1] = Constants.MAX_TOKEN_LEN;
      msg_item_lengths[2] = Constants.MAX_LOGIN_NAME_LEN;
      msg_item_lengths[3] = Constants.MAX_PROGROUP_TITLE_LEN;
      msg_item_lengths[4] = 1;

      if (parseData(msg_item_lengths, msg_items) == false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not parse incoming message!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_PARSE_MSG);
          return;
        }

      // Validate requestor's ID
      int n;
      IntegerBuffer junk = null;
      n = isUserValid(msg_items[0], msg_items[1], junk);
      if (n < 0)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not validate user " + msg_items[0]
                     + " and/or token.");
          sendErrorMessage(err_header, n);
          return;
        }

      // Establish the DB connection.
      db_connection = DatabaseConnectionManager.getConnection();
      if (db_connection == null)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Null DB connection returned!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_ACCESS_DB);
          return;
        }

      arg_list = new Object[3];
      if (msg_items[4].equals("1"))
        arg_list[0] = new Boolean(true);                   // Register appuser
      else
        arg_list[0] = new Boolean(false);                  // Unregister appuser
      arg_list[1] = msg_items[2];             // login name
      arg_list[2] = msg_items[3];             // app name
      SQL_query_string = StringLib.SQLformat(
                        "UPDATE app_users set registered_user=" + Constants.ST
                         + " WHERE login_name=" + Constants.ST
                         + " AND app_title=" + Constants.ST,
                        arg_list);

      // Run the query
      SQL_statement = db_connection.createStatement();
      String last_warning_msg ="";
      int i_temp = 0;
      try
      {
        i_temp = SQL_statement.executeUpdate(SQL_query_string);
        if (i_temp < 0)
          {
            last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
            Log.error(this_servers_name, this_methods_name, last_warning_msg);
          }
      }
      catch(Exception exception)
      {
        i_temp = -1;   // Just set this so it will be seen below.
        last_warning_msg = exception.toString();
        Log.excp(this_servers_name, this_methods_name, last_warning_msg);
      }

      if (i_temp < 0)
        {
          sendErrorMessage(err_header, ErrorCodes.ERROR_DB_UPDATE_FAILED);
          return;
        }
      else if (i_temp == 0) 
        {
          Log.error(this_servers_name, this_methods_name, "No such app user <" + 
              msg_items[2] + "> for <" + msg_items[3] + ">");
          sendErrorMessage(err_header, ErrorCodes.ERROR_NO_SUCH_APP_USER);
          return; 
        }

      if (msg_items[4].equals("1"))
        sendFormattedServerMsg("GOODREGAPPUSER",
                               "Registered user with application.");
      else
        sendFormattedServerMsg("GOODREGAPPUSER",
                               "Unregistered user with application.");
      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg(err_header,
                             StringLib.valueOf(exception.toString().length(), 5)
                             + exception.toString());
      return;
    }
    finally
    {
      // Close the database resources
      try
      {
        if (SQL_statement != null)
          SQL_statement.close();
        if (db_connection != null)
          DatabaseConnectionManager.releaseConnection(db_connection);
      }
      catch (Exception exception)
      {
        Log.excp(this_servers_name, this_methods_name,
                 exception.toString());
      }
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  /**
   * This method sets a sys_conf tag-value pair to the specified value(s).
   * A user token is required for authentication.
   */

  public void   handleSetCommonDBitem()
  {
    String         msg_items[];
    int            msg_item_lengths[];
//    Connection     db_connection = null;
//    Statement      SQL_statement = null;
//    Object         arg_list[];
    final String   err_header = "NOSETCOMMONDB";
    final String   this_methods_name = "handleSetCommonDBitem";
    int            error_code;
    String         message;

    try
    {
      msg_item_lengths = new int[4];
      msg_items = new String[4];

      msg_item_lengths[0] = Constants.MAX_LOGIN_NAME_LEN;
      msg_item_lengths[1] = Constants.MAX_TOKEN_LEN;
      msg_item_lengths[2] = Constants.MAX_COMDB_TAG_LEN;
      msg_item_lengths[3] = Constants.MAX_COMDB_DATA_LEN;

      if (parseData(msg_item_lengths, msg_items) == false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not parse incoming message!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_PARSE_MSG);
          return;
        }

      // Validate requestor's ID
      int n;
      IntegerBuffer junk = null;
      n = isUserValid(msg_items[0], msg_items[1], junk);
      if (n < 0)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not validate user " + msg_items[0]
                    + " and/or token.");
          sendErrorMessage(err_header, n);
          return;
        }

      // Make sure this is the master host..
      if (this_servers_role != Constants.NTCSS_MASTER)
        {
          sendErrorMessage(err_header, ErrorCodes.ERROR_HOST_IS_NOT_MASTER);
          return;
        }

      // Check sizes/integrity of the data given
      if (!DataLib.checkCommonDbTag(msg_items[2].toString()))
        {
          Log.error(this_servers_name, this_methods_name, "Invalid tag!");
          sendErrorMessage(err_header,
                           ErrorCodes.ERROR_INVALID_COMDB_TAG_VALUE);
          return;
        }

      int   i;
      char  test_char;

      if (!DataLib.checkCommonDbItem(msg_items[3].toString()))
        {
          Log.error(this_servers_name, this_methods_name, "Invalid value!");
          sendErrorMessage(err_header,
                           ErrorCodes.ERROR_INVALID_COMDB_ITEM_VALUE);
          return;
        }

/*
      // Establish the DB connection.
      db_connection = DatabaseConnectionManager.getConnection();
      if (db_connection == null)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Null DB connection returned!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_ACCESS_DB);
          return;
        }

      SQL_statement = db_connection.createStatement();

      arg_list = new Object[2];
      String   SQL_query_string1;
      String   SQL_query_string2;

      // First delete the tag in case it already exists..
      arg_list[0] = msg_items[2];             // tag
      SQL_query_string1 = StringLib.SQLformat("DELETE FROM sys_conf WHERE tag="
                                              + Constants.ST,  arg_list);

      arg_list[1] = msg_items[3];             // value, tag is already set..
      SQL_query_string2 = StringLib.SQLformat("INSERT INTO sys_conf(tag,value) "
                                + " VALUES(" + Constants.ST + ","
                                + Constants.ST + ")", arg_list);
      String last_warning_msg ="";
      int i_temp = 0;
      try
      {
        // We don't worry about the first return value (of the delete) since
        // the tag may exist or may not.
        i_temp = SQL_statement.executeUpdate(SQL_query_string1);
        i_temp = SQL_statement.executeUpdate(SQL_query_string2);
        if (i_temp < 0)
          {
            last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
            Log.error(this_servers_name, this_methods_name, last_warning_msg);
          }
      }
      catch(Exception exception)
      {
        i_temp = -1;   // Just set this so it will be seen below.
        last_warning_msg = exception.toString();
        Log.excp(this_servers_name, this_methods_name, last_warning_msg);
      }
      if (i_temp < 1)
        {
          String error_message = "SQL operation failed!";
          Log.error(this_servers_name, this_methods_name, error_message);
          sendFormattedServerMsg(err_header,
                                 StringLib.valueOf(error_message.length(), 5)
                                  + error_message);
          Log.error(this_servers_name, this_methods_name,
                    "Null DB connection returned!");
        }
      else
        sendFormattedServerMsg("GOODSETCOMMONDB", "Common Data set.");
 */

      if ((error_code = DataLib.putNtcssSysConfData(msg_items[2], 
                                                    msg_items[3])) == 0) {
         message = "Common data record successfully registered.";
         sendFormattedServerMsg("GOODSETCOMMONDB", 
                    StringLib.valueOf(message.length(), 5) + message + "\0");
      }
      else 
         sendErrorMessage("NOSETCOMMONDB", error_code);
      
      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg(err_header,
                             StringLib.valueOf(exception.toString().length(), 5)
                             + exception.toString());
      return;
    }
/*    
    finally
    {  
      // Close the database resources
      try
      {
        if (SQL_statement != null)
          SQL_statement.close();
        if (db_connection != null)
          DatabaseConnectionManager.releaseConnection(db_connection);
      }
      catch (Exception exception)
      {
        Log.excp(this_servers_name, this_methods_name,
                 exception.toString());
      } 
    }
 */
  }


  /////////////////////////////////////////////////////////////////////////////
  /**
   * This method sets the link_data for all the apps on the host specified, or
   * all the apps on the same host as the specified app.
   * This poses a problem if differnt apps use different passwords on the same
   * host or in the case where two database engines are running with different
   * passwords.  However, this is the way the API has always worked.
   * A user or server token is required for authentication.
   */

  public void  handleSetlinkdata()
  {
    String         msg_items[];
    int            msg_item_lengths[];
    String         SQL_query_string;
    ResultSet      db_result_set = null;
    Connection     db_connection = null;
    Statement      SQL_statement = null;
    Object         arg_list[];
    final String   err_header = "NOSETLINKDATA";
    final String   this_methods_name = "handleSetlinkdata";

    try
    {
      if (isServerIdled() == true) {
         sendErrorMessage(err_header, ErrorCodes.ERROR_SERVER_IDLED);
         return;
      }
      
      msg_item_lengths = new int[4];
      msg_items = new String[4];

      msg_item_lengths[0] = Constants.MAX_LOGIN_NAME_LEN;
      msg_item_lengths[1] = Constants.MAX_TOKEN_LEN;
      msg_item_lengths[2] = Constants.MAX_PROGROUP_TITLE_LEN; //app or hostname
      msg_item_lengths[3] = Constants.MAX_APP_PASSWD_LEN;  // link data

      if (parseData(msg_item_lengths, msg_items) == false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not parse incoming message!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_PARSE_MSG);
          return;
        }

      // Validate requestor's ID
      boolean  is_server_api_call = false;
      int n = 0;
      if (msg_items[1].regionMatches(0, Constants.SERVER_TOKEN_STRING, 0,
                                     Constants.SERVER_TOKEN_STRING.length())
                              == true)
        {
          if (msg_items[0].compareTo("root") != 0)
           n = doServerValidate(msg_items[0], msg_items[1], getClientAddress());
          is_server_api_call = true;
        }
      else
        {
          IntegerBuffer junk = null;
          n = isUserValid(msg_items[0], msg_items[1], junk);
        }
      if (n < 0)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not validate user " + msg_items[0]
                     + " and/or token.");
          sendErrorMessage(err_header, n);
          return;
        }

      // Establish a DB connection.
      db_connection = DatabaseConnectionManager.getConnection();
      if (db_connection == null)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Null DB connection returned!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_ACCESS_DB);
          return;
        }

      // First we have to get all the apps on the host specified or the
      // provided app's siblings on the same host.

      arg_list = new Object[1];
      arg_list[0] = msg_items[2];  // app_title;
      if (!is_server_api_call)
        {
          // We only have an app title to work with...
          SQL_query_string = StringLib.SQLformat( "SELECT app_title FROM apps "
                        + "WHERE hostname=(SELECT hostname FROM apps "
                        + "WHERE app_title=" + Constants.ST + ")", arg_list);
        }
      else
        {
          // We only have a hostname to work with...
          SQL_query_string = StringLib.SQLformat( "SELECT app_title FROM apps "
                                + "WHERE UPPER(hostname)=UPPER(" + 
                                Constants.ST + ")", arg_list);
        }
      SQL_statement = db_connection.createStatement();

      db_result_set = SQL_statement.executeQuery(SQL_query_string);
      if (db_result_set.next() == false)
        {
          // This may be ok if a host has no apps, but is a rather unusual
          // situation so we just report an error.
          String  message = "No apps were found to be associated with "
                              + msg_items[2]
                              + ".  No link data has been changed.";
          Log.error(this_servers_name, this_methods_name, message);
          sendFormattedServerMsg(err_header,
                                 StringLib.valueOf(message.length(), 5)
                                  + message);
          return;
        }

      // Now set this same link data for each of the apps on the target host..
      int  i_temp = 0;
      do
        {
          if (SetLinkDataForApp(msg_items[3], db_result_set.getString(1)) < 0)
            i_temp++;
        }
      while (db_result_set.next());

      if (i_temp != 0)
        {         
          sendErrorMessage(err_header, ErrorCodes.ERROR_DB_UPDATE_FAILED);
          return;
        }

      sendFormattedServerMsg("GOODSETLINKDATA", "\0");
      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg(err_header,
                             StringLib.valueOf(exception.toString().length(), 5)
                             + exception.toString());
      return;
    }
    finally
    {
      // Close the database resources
      try
      {
        if (db_result_set != null)
          db_result_set.close();
        if (SQL_statement != null)
          SQL_statement.close();
        if (db_connection != null)
          DatabaseConnectionManager.releaseConnection(db_connection);
      }
      catch (Exception exception)
      {
        Log.excp(this_servers_name, this_methods_name,
                 exception.toString());
      }
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  /**
   * This method Checks to make sure the specified printer is in the NTCSS
   * database.
   * No authentication is required.
   */

  public void  handleValidatePrinter()
  {
    String         msg_items[];
    int            msg_item_lengths[];
    String         SQL_query_string;
    Connection     db_connection = null;
    Statement      SQL_statement = null;
    ResultSet      db_result_set = null;
    Object         arg_list[];
    final String   err_header = "NOPRINTERVALIDATE";
    final String   this_methods_name = "handleValidatePrinter";

    try
    {
      if (isServerIdled() == true) {
         sendFormattedServerMsg("RETPRTVALIDATE", 
                   StringLib.valueOf(ErrorCodes.ERROR_SERVER_IDLED, 5) + "\0");
         return;
      }
      
      msg_item_lengths = new int[1];
      msg_items = new String[1];
      // The old "host" value is now deprecated.

      msg_item_lengths[0] = Constants.MAX_PRT_NAME_LEN;          // printer name

      if (parseData(msg_item_lengths, msg_items) == false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not parse incoming message!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_PARSE_MSG);
          return;
        }

      // Establish the DB connection.
      db_connection = DatabaseConnectionManager.getConnection();
      if (db_connection == null)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Null DB connection returned!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_ACCESS_DB);
          return;
        }

      arg_list = new Object[1];
      arg_list[0] = msg_items[0];             // printer name

      SQL_query_string = StringLib.SQLformat(
                        "SELECT printer_name FROM printers WHERE printer_name="
                         + Constants.ST, arg_list);

      // Run the query
      SQL_statement = db_connection.createStatement();
      db_result_set = SQL_statement.executeQuery(SQL_query_string);
      if (db_result_set.next())
        {
          // Reponse was originally a "1" in bundled format...
          sendFormattedServerMsg("RETPRTVALIDATE", "11");
          return;
        }

      // Otherwise we didn't find the printer specified...
      // Bundle up the proper negative response code.
      Object  bundle_items[] = new Object[1];
      StringBuffer  bundle_buffer = new StringBuffer();
      bundle_items[0] = new Integer(ErrorCodes.ERROR_NO_PRT_INFO_FOUND);
      BundleLib.bundleData(bundle_buffer, "I", bundle_items);

      sendFormattedServerMsg("RETPRTVALIDATE", bundle_buffer.toString());
      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg(err_header,
                             StringLib.valueOf(exception.toString().length(), 5)
                             + exception.toString());
      return;
    }
    finally
    {
      // Close the database resources
      try
      {
        if (db_result_set != null)
          db_result_set.close();
        if (SQL_statement != null)
          SQL_statement.close();
        if (db_connection != null)
          DatabaseConnectionManager.releaseConnection(db_connection);
      }
      catch (Exception exception)
      {
        Log.excp(this_servers_name, this_methods_name,
                 exception.toString());
      }
    }
  }


  /////////////////////////////////////////////////////////////////////////////
   /**
    * This method checks the given username, password, and app membership
    * A user or server token is required for authentication.
    */
  
  public void handleValidateUser()
  {
    String         msg_items[];
    int            msg_item_lengths[];
    String         SQL_query_string;
    Connection     db_connection = null;
    Statement      SQL_statement = null;
    ResultSet      db_result_set = null;
    Object         arg_list[];
    final String   err_header = "NOVALIDATEUSER";
    final String   this_methods_name = "handleValidateUser";
    
    try
    {

      if (isServerIdled() == true) {
         sendErrorMessage(err_header, ErrorCodes.ERROR_SERVER_IDLED);
         return;
      }
      
      msg_item_lengths = new int[5];
      msg_items = new String[5];
      // The old "host" value is now deprecated.

      msg_item_lengths[0] = Constants.MAX_LOGIN_NAME_LEN;     // login name
      msg_item_lengths[1] = Constants.MAX_TOKEN_LEN;          // token
      msg_item_lengths[2] = Constants.MAX_LOGIN_NAME_LEN;     // user name
      msg_item_lengths[3] = 2 * Constants.MAX_APP_PASSWD_LEN; // user password
      msg_item_lengths[4] = Constants.MAX_PROGROUP_TITLE_LEN; // user's app

      if (parseData(msg_item_lengths, msg_items) == false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not parse incoming message!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_PARSE_MSG);
          return;
        }

      // Validate requestor's ID
      int n;
      if (msg_items[1].regionMatches(0, Constants.SERVER_TOKEN_STRING, 0,
                                     Constants.SERVER_TOKEN_STRING.length())
                            == true)
        n = doServerValidate(msg_items[0], msg_items[1], getClientAddress());
      else
        {
          IntegerBuffer junk = null;
          n = isUserValid(msg_items[0], msg_items[1], junk);
        }
      if (n < 0)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not validate user " + msg_items[0]
                     + " and/or token.");
          sendErrorMessage(err_header, n);
          return;
        }

      // Establish the DB connection.
      db_connection = DatabaseConnectionManager.getConnection();
      if (db_connection == null)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Null DB connection returned!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_ACCESS_DB);
          return;
        }

      // First check the specified user's app_user status...
      arg_list = new Object[2];
      arg_list[0] = msg_items[2];            // user's login name
      arg_list[1] = msg_items[4];            // user's app
      SQL_query_string = StringLib.SQLformat("SELECT app_title FROM app_users"
                                      + " WHERE login_name=" + Constants.ST
                                      + " AND app_title="  + Constants.ST,
                                     arg_list);
      // Run the query
      SQL_statement = db_connection.createStatement();
      db_result_set = SQL_statement.executeQuery(SQL_query_string);

      if (!db_result_set.next())
        {
          sendErrorMessage(err_header, ErrorCodes.ERROR_NO_SUCH_APP_USER);
          return;
        }

      StringBuffer  decrypted_password = new StringBuffer(); 
      if (CryptLib.plainDecryptString(msg_items[2] + msg_items[2],
                         CryptLib.trimEncryptedPasswordTerminator(msg_items[3]),
                                          decrypted_password, false, true) != 0)
        {
          sendErrorMessage(err_header, ErrorCodes.ERROR_INCORRECT_PASSWORD);
          return;
        }

      StringBuffer  ss_num = new StringBuffer();
      if (UserLib.checkNtcssUserPassword(msg_items[2],
                                         decrypted_password.toString(),
                                         ss_num) < 0)
        {
          sendErrorMessage(err_header, ErrorCodes.ERROR_INCORRECT_PASSWORD);
          return;
        }

      Object  bundle_items[] = new Object[2];
      StringBuffer  bundle_buffer = new StringBuffer();
      // bundle in total expected string size..
      bundle_items[0] = new Integer(Constants.MAX_SS_NUM_LEN + 5);
      bundle_items[1] = ss_num.toString();
      BundleLib.bundleData(bundle_buffer, "IC", bundle_items);

      sendFormattedServerMsg("GOODVALIDATEUSER",
                             bundle_buffer.toString() + "\0");
           
      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg(err_header,
                             StringLib.valueOf(exception.toString().length(), 5)
                             + exception.toString() + "\0");
      return;
    }
    finally
    {
      // Close the database resources
      try
      {
        if (db_result_set != null)
          db_result_set.close();
        if (SQL_statement != null)
          SQL_statement.close();
        if (db_connection != null)
          DatabaseConnectionManager.releaseConnection(db_connection);
      }
      catch (Exception exception)
      {
        Log.excp(this_servers_name, this_methods_name,
                 exception.toString());
      }
    }
  }


  /////////////////////////////////////////////////////////////////////////////
   /**
    * This method repimes the NTCSS database with a new progrps.inin file.
    * A user or server token is required for authentication.
    */

  public void handleImportEnvironment()
  {
    String         msg_items[];
    int            msg_item_lengths[];
    final String   err_header = "NOVALIDATEUSER";
    final String   this_methods_name = "handleImportEnvironment";
    //Connection     db_connection = null;
    //Statement      SQL_statement = null;

    try
    {
      msg_item_lengths = new int[2];
      msg_items = new String[2];
      // The old "host" value is now deprecated.

      msg_item_lengths[0] = Constants.MAX_LOGIN_NAME_LEN;     // login name
      msg_item_lengths[1] = Constants.MAX_TOKEN_LEN;          // token

      if (parseData(msg_item_lengths, msg_items) == false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not parse incoming message!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_PARSE_MSG);
          return;
        }

      // Validate requestor's ID
      int n;
      if (msg_items[1].regionMatches(0, Constants.SERVER_TOKEN_STRING, 0,
                                     Constants.SERVER_TOKEN_STRING.length())
                            == true)
        n = doServerValidate(msg_items[0], msg_items[1], getClientAddress());
      else
        {
          IntegerBuffer junk = null;
          n = isUserValid(msg_items[0], msg_items[1], junk);
        }
      if (n < 0)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not validate user " + msg_items[0]
                     + " and/or token.");
          sendErrorMessage(err_header, n);
          return;
        }

      /*** No longer needed....
      // Establish the DB connection.
      db_connection = DatabaseConnectionManager.getConnection();
      if (db_connection == null)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Null DB connection returned!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_ACCESS_DB);
          return;
        }
      ****/

      // Make sure the permissions are set correctly on the new INI file...
      // TO BE IMPLEMENTED(?) - the old code chown'd to root and set
      // permissions to 600

      //Copy the */logs/progroups.ini to the */init_data directory for prime...
      if (!FileOps.copy(Constants.NTCSS_LOGS_DIR + "/progrps.ini",
                        Constants.NTCSS_PROGRP_INIT_FILE))
        {
          Log.error(this_servers_name, this_methods_name,
                    "Unable to copy the progrps.ini file!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_OPEN_FILE);
          return;
        }

      // Make sure the permissions are set correctly on the progrps.ini file.

      /** This should be removed since prime doesn't toucht he current_users
       * table anymore...
      // First we make a copy of the current users table because the
      // subsequent priming process clears them out out.
      String SQL_query_string =
                           "SELECT * INTO tmp_current_users FROM current_users";
      // Run the query
      SQL_statement = db_connection.createStatement();
      if (!SQL_statement.execute(SQL_query_string))
        {
          Log.error(this_servers_name, this_methods_name,
                    "Failed to create the tmp_current_users table");
          sendErrorMessage(err_header, ErrorCodes.ERROR_DB_QUERY_FAILED);
          return;
        }
      ***/

      // Prime the database with the new INI file...
      PrimeDB prime = new PrimeDB();

      NtcssCmd.handleIdle();
      boolean  prime_ok = prime.start();
      NtcssCmd.handleAwaken();

      /**  Not done anymore since prime no longer touches this table.
      // Restore the current_users table...
      SQL_query_string = "SELECT * INTO current_users FROM tmp_current_users";
      if (!SQL_statement.execute(SQL_query_string))
        Log.error(this_servers_name, this_methods_name,
                  "Failed to restore the current_users table!");
      ***/
      if (prime_ok)
        {
          sendFormattedServerMsg("GOODIMPORTENV", "00007Success");
          return;
        }

      // Otherwise we have some cleanup to do...
      Log.error(this_servers_name, this_methods_name,
                "Failed to prime the database");

      // Restore the old prograoups.ini in the database directory to the one in
      // init_data so that when the daemons start next time
      // they don't try to re-prime with the newer bad file again.
      if (!FileOps.copy(Constants.NTCSS_PROGRP_DB_FILE,
                        Constants.NTCSS_PROGRP_INIT_FILE))
        {
          Log.error(this_servers_name, this_methods_name,
                    "Failed to restore the original progroups.ini file!  Prime "
                     + "will run and fail on next startup if this is not "
                     + "corrected manually!");
        }

      sendErrorMessage(err_header, ErrorCodes.ERROR_DB_UPDATE_FAILED);
      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg(err_header,
                             StringLib.valueOf(exception.toString().length(), 5)
                             + exception.toString());
      return;
    }
    finally
    {
      /** disabled for now....
      // Close the database resources
      try
      {
        if (SQL_statement != null)
          SQL_statement.close();
        if (db_connection != null)
          DatabaseConnectionManager.releaseConnection(db_connection);
      }
      catch (Exception exception)
      {
        Log.excp(this_servers_name, this_methods_name,
                 exception.toString());
      }
      ***/
    }
  }


  /////////////////////////////////////////////////////////////////////////////
   /**
    * This method sends the number of users currently logged in back to the
    * client.
    * No authentication required.
    */

  public void handleGetNumLogins()
  {
    String         msg_items[];
    int            msg_item_lengths[];
    final String   err_header = "NOGETCURRUSRCNT";
    final String   this_methods_name = "handleGetNumLogins";
    Statement      SQL_statement = null;
    ResultSet      db_result_set = null;
    Connection     db_connection = null;

    try
    {
      // Establish the DB connection.
      db_connection = DatabaseConnectionManager.getConnection();
      if (db_connection == null)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Null DB connection returned!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_ACCESS_DB);
          return;
        }

      String SQL_query_string = "SELECT COUNT(login_name) FROM current_users";

      SQL_statement = db_connection.createStatement();
      db_result_set = SQL_statement.executeQuery(SQL_query_string);

      if (!db_result_set.next())
        {
          sendErrorMessage(err_header, ErrorCodes.ERROR_DB_QUERY_FAILED);
          return;
        }
/*
      Object  bundle_items[] = new Object[1];
      StringBuffer  bundle_buffer = new StringBuffer();
      bundle_items[0] = new Integer(db_result_set.getInteger(1));
      BundleLib.bundleData(bundle_buffer, "I", bundle_items);
 **/
      String temp = db_result_set.getString(1);
      String response = temp.length() + temp;

      //sendFormattedServerMsg("GOODVALIDATEUSER", bundle_buffer.toString());
      sendFormattedServerMsg("GOODGETCURRUSRCNT", response);
      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg(err_header,
                             StringLib.valueOf(exception.toString().length(), 5)
                             + exception.toString());
      return;
    }
    finally
    {
      // Close the database resources
      try
      {
        if (db_result_set != null)
          db_result_set.close();
        if (SQL_statement != null)
          SQL_statement.close();
        if (db_connection != null)
          DatabaseConnectionManager.releaseConnection(db_connection);
      }
      catch (Exception exception)
      {
        Log.excp(this_servers_name, this_methods_name,
                 exception.toString());
      }
    }
  }


  /////////////////////////////////////////////////////////////////////////////
   /**
    * This method applies PDJ updates from a provided INI file
    * A user token is required for authentication.
    */

  public void handleUpdatePDJs()
  {
    String         msg_items[];
    int            msg_item_lengths[];
    final String   err_header = "NOPDJINI";
    final String   this_methods_name = "handleUpdatePDJs";
    Statement      SQL_statement = null;
    Connection     db_connection = null;
    String         iniFilename;

    try
    {
      if (isServerIdled() == true) {
         sendErrorMessage(err_header, ErrorCodes.ERROR_SERVER_IDLED);
         return;
      }
      
      msg_item_lengths = new int[3];
      msg_items = new String[3];
      // The old "host" value is now deprecated.

      msg_item_lengths[0] = Constants.MAX_LOGIN_NAME_LEN;     // login name
      msg_item_lengths[1] = Constants.MAX_TOKEN_LEN;          // token
      msg_item_lengths[2] = Constants.MAX_PATH_LEN;           // INI filename

      if (parseData(msg_item_lengths, msg_items) == false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not parse incoming message!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_PARSE_MSG);
          return;
        }

      // Validate requestor's ID
      IntegerBuffer junk = null;
      int n;
      n = isUserValid(msg_items[0], msg_items[1], junk);
      if (n < 0)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not validate user " + msg_items[0]
                    + " and/or token.");
          sendErrorMessage(err_header, n);
          return;
        }

      // Open the operational log file to track the following PDJ changes.
      File fileDescp = new File(Constants.NTCSS_PDJ_CHANGE_FILE);
      if (fileDescp.getParentFile().exists() == false) 
         fileDescp.getParentFile().mkdirs();      
      
      FileWriter  log_file_writer =
      new FileWriter(fileDescp);

      // Make sure the INI file exists...
      iniFilename = msg_items[2];
      if (msg_items[2].startsWith("/tmp/") == true)
         iniFilename = Constants.NTCSS_TMP_DIR + "/" + msg_items[2].substring(5);
         
      fileDescp = new File(iniFilename);
      if ((fileDescp.exists() == false) || !fileDescp.canRead())
        {
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_OPEN_FILE);
          return;
        }

      INI ini = new INI();

      // Get the roles section for the given application
      String   pdjs_section;
      pdjs_section = ini.GetProfileSection("PDJ", iniFilename);
      if (pdjs_section == null)
        {
          // Assume no PDJs were listed for changes.
          log_file_writer.write("ALL CHANGES APPLIED SUCCESSFULLY\n");
          log_file_writer.close();
          sendFormattedServerMsg("GOODPDJINI", "");
          return;
        }

      // Establish the DB connection.
      db_connection = DatabaseConnectionManager.getConnection();
      if (db_connection == null)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Null DB connection returned!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_ACCESS_DB);
          return;
        }

      StringBuffer log_file_buffer = new StringBuffer();  // Queues up results
      String  requested_change;
      String  SQL_query_string;
      String  temp;
      String  pdjSection;
      Object  arg_list[];
      int     job_class;
      int     num_changes = 0;
      int     num_errors = 0;
      String  last_warning_msg ="";
      int     i_temp = 0;

      SQL_statement = db_connection.createStatement();

      // Split the lines in the PDJ section into tokens..
      StringTokenizer  pdjsTokenizer;
      StringTokenizer  role_tokenizer;
      StringTokenizer  entryTokenizer;
      pdjsTokenizer = new StringTokenizer(pdjs_section, INI.STRINGS_DELIMITER);
      while (pdjsTokenizer.hasMoreTokens() == true)
        {
          log_file_buffer.setLength(0);

        // Now split each line into tag/value tokens...
        entryTokenizer = new StringTokenizer(pdjsTokenizer.nextToken(),
                                             INI.TAG_VALUE_DELIMITER);
        // Get the section name that describes this PDJ's database fields...
        pdjSection  = entryTokenizer.nextToken();

        requested_change = ini.GetProfileString(pdjSection, "OPERATION",
                                                "ERROR", iniFilename);
        if (requested_change.compareTo("ERROR") == 0)
          {
            Log.error(this_servers_name, this_methods_name,
                      "No Operation tag for section " + pdjSection);
            log_file_buffer.append("Unknown change requested for PDJ "
                                     + pdjSection);
            log_file_writer.write(log_file_buffer.toString() + "\n");
            num_errors++;
            continue;
          }

        // Determine the job classification
        job_class = Constants.REGULAR_JOB;   // default (regular)
        temp = ini.GetProfileString(pdjSection, "JOBCLASS", "\0",
        iniFilename);
        if (temp.compareTo("Scheduled") == 0)
          job_class = Constants.SCHEDULED_JOB;
        if (temp.compareTo("Boot") == 0)
          job_class = Constants.BOOT_JOB;

        log_file_buffer.setLength(0);  // reset this for the next change..

        // Add a new PDJ...
        if (requested_change.compareTo("ADD") == 0)
          {
            arg_list = new Object[5];
            arg_list[0] = ini.GetProfileString(pdjSection, "TITLE", "\0",
                                               iniFilename);
            arg_list[1] = ini.GetProfileString(pdjSection, "GROUP", "\0",
                                               iniFilename);
            arg_list[2] = ini.GetProfileString(pdjSection, "COMMAND", "\0",
                                               iniFilename);

            arg_list[3] = new Integer(job_class);
            arg_list[4] = ini.GetProfileString(pdjSection, "SCHEDULE", "\0",
                                               iniFilename);
            // Make a note in the log file about the schedule, iff scheduled.
            if (job_class == Constants.SCHEDULED_JOB)
              if (((String)arg_list[4]).length() < 1)
                log_file_buffer.append("SCHEDULED ITEMS INCLUDED\n");
              else
                log_file_buffer.append("NO SCHEDULE ITEMS\n");

            // Set up the generic status message...
            log_file_buffer.setLength(0);
            log_file_buffer.append("PDJ " + arg_list[0].toString()
                                    + " for app " + arg_list[1].toString());

            SQL_query_string = StringLib.SQLformat(
                   "INSERT INTO predefined_jobs(pdj_title,"
                    + "app_title,command_line,job_type,schedule_str) VALUES("
                    + Constants.ST + "," + Constants.ST + "," + Constants.ST
                    + "," + Constants.ST + "," + Constants.ST + ")",
                   arg_list);

            last_warning_msg ="";
            i_temp = 0;
            db_connection.setAutoCommit(false);
            try
            {
              i_temp = SQL_statement.executeUpdate(SQL_query_string);
              if (i_temp < 0)
                {
                  last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
                  Log.error(this_servers_name, this_methods_name,
                            last_warning_msg);
                }
            }
            catch(Exception exception)
            {
              i_temp = -1;   // Just set this so it will be seen below.
              last_warning_msg = exception.toString();
              Log.excp(this_servers_name, this_methods_name,
                        last_warning_msg);
            }
            if (i_temp < 1)
              {
                db_connection.rollback();
                db_connection.setAutoCommit(true);
                log_file_buffer.insert(0, "Failed to add ");
                log_file_writer.write(log_file_buffer.toString() + "\n");
                Log.error(this_servers_name, this_methods_name,
                log_file_buffer.toString());
                num_errors++;
                continue;
              }

            // Add in the roles...
            role_tokenizer = new StringTokenizer(
            ini.GetProfileString(pdjSection, "ROLELIST", "", iniFilename),
                                 ",");
            boolean error_occured = false;
            while (role_tokenizer.hasMoreTokens() == true)
              {
                // we will reuse the args we already set...
                arg_list[2] = role_tokenizer.nextToken();
                SQL_query_string = StringLib.SQLformat(
                      "INSERT INTO pdj_app_roles(pdj_title,app_title,"
                       + "role_name) VALUES(" + Constants.ST + ","
                       + Constants.ST + "," + Constants.ST + ")",
                      arg_list);

                last_warning_msg ="";
                i_temp = 0;
                try
                {
                  i_temp = SQL_statement.executeUpdate(SQL_query_string);
                  if (i_temp < 0)
                    {
                      last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
                      Log.error(this_servers_name, this_methods_name,
                                last_warning_msg);
                    }
                }
                catch(Exception exception)
                {
                  i_temp = -1;   // Just set this so it will be seen below.
                  last_warning_msg = exception.toString();
                  Log.excp(this_servers_name, this_methods_name,
                            last_warning_msg);
                }
                if (i_temp < 1)
                  {
                    log_file_buffer.insert(0, "Failed to add role "
                                           + arg_list[2].toString() + " to ");
                    log_file_writer.write(log_file_buffer.toString() + "\n");
                    Log.error(this_servers_name, this_methods_name,
                              log_file_buffer.toString());
                    num_errors++;
                    error_occured = true;
                  }
              }
            if (error_occured)
              {
                db_connection.rollback();
                db_connection.setAutoCommit(true);
              }
            else
              {
                db_connection.setAutoCommit(true);
                log_file_buffer.insert(0, "Successfully added ");
                log_file_writer.write(log_file_buffer.toString() + "\n");
              }
          }

        // Update the PDJ with new possibly values...
        if (requested_change.compareTo("EDIT") == 0)
          {
            arg_list = new Object[5];
            arg_list[3] = ini.GetProfileString(pdjSection, "TITLE", "\0",
                                               iniFilename);
            arg_list[4] = ini.GetProfileString(pdjSection, "GROUP", "\0",
                                               iniFilename);
            arg_list[0] = ini.GetProfileString(pdjSection, "COMMAND", "\0",
                                               iniFilename);
            arg_list[1] = new Integer(job_class);
            arg_list[2] = ini.GetProfileString(pdjSection, "SCHEDULE", "\0",
                                               iniFilename);
            SQL_query_string = StringLib.SQLformat(
                        "UPDATE predefined_jobs SET command_line="
                         + Constants.ST + ",job_type=" + Constants.ST
                         + ",schedule_str=" + Constants.ST + " WHERE pdj_title="
                         + Constants.ST + " AND app_title=" + Constants.ST,
                        arg_list);

            // Set up the generic status message...
            log_file_buffer.setLength(0);
            log_file_buffer.append("PDJ " + arg_list[3].toString()
                                   + " for app " + arg_list[4].toString());

            last_warning_msg ="";
            i_temp = 0;
            db_connection.setAutoCommit(false);
            try
            {
              i_temp = SQL_statement.executeUpdate(SQL_query_string);
              if (i_temp < 0)
                {
                  last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
                  Log.error(this_servers_name, this_methods_name,
                            last_warning_msg);
                }
            }
            catch(Exception exception)
            {
              i_temp = -1;   // Just set this so it will be seen below.
              last_warning_msg = exception.toString();
              Log.excp(this_servers_name, this_methods_name,
              last_warning_msg);
            }
            if (i_temp < 1)
              {
                db_connection.rollback();
                db_connection.setAutoCommit(true);
                log_file_buffer.insert(0, "Failed to update ");
                log_file_writer.write(log_file_buffer.toString() + "\n");
                Log.error(this_servers_name, this_methods_name,
                          log_file_buffer.toString());
                num_errors++;
                continue;
              }

            // Add in the roles...
            role_tokenizer = new StringTokenizer(
            ini.GetProfileString(pdjSection, "ROLELIST",
                                 "", iniFilename), ",");
            // First clean out all the pre-exisitng roles....
            arg_list[0] = ini.GetProfileString(pdjSection, "TITLE", "\0",
                                               iniFilename);
            arg_list[1] = ini.GetProfileString(pdjSection, "GROUP", "\0",
                                               iniFilename);
            SQL_query_string = StringLib.SQLformat(
                           "DELETE FROM pdj_app_roles WHERE pdj_title="
                            + Constants.ST + " AND app_title=" + Constants.ST,
                           arg_list);

            last_warning_msg ="";
            i_temp = 0;
            try
            {
              i_temp = SQL_statement.executeUpdate(SQL_query_string);
              if (i_temp < 0)
                {
                  last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
                  Log.error(this_servers_name, this_methods_name,
                            last_warning_msg);
                }
            }
            catch(Exception exception)
            {
              i_temp = -1;   // Just set this so it will be seen below.
              last_warning_msg = exception.toString();
              Log.excp(this_servers_name, this_methods_name,
              last_warning_msg);
            }
            if (i_temp < 1)
              {
              db_connection.rollback();
              db_connection.setAutoCommit(true);
              log_file_buffer.insert(0, "Failed to update roles for ");
              log_file_writer.write(log_file_buffer.toString() + "\n");
              Log.error(this_servers_name, this_methods_name,
                        log_file_buffer.toString());
              num_errors++;
              continue;
            }

            // Add in the roles...
            role_tokenizer = new StringTokenizer(
                                    ini.GetProfileString(pdjSection, "ROLELIST",
                                    "", iniFilename), ",");
            // set these up for the while loop...
            arg_list[0] = ini.GetProfileString(pdjSection, "TITLE", "\0",
                                               iniFilename);
            arg_list[1] = ini.GetProfileString(pdjSection, "GROUP", "\0",
                                               iniFilename);
            // Now insert the roles from the ini...
            boolean error_occured = false;
            while (role_tokenizer.hasMoreTokens() == true)
              {
                // we will reuse the ones we already set...
                arg_list[2] = role_tokenizer.nextToken();
                SQL_query_string = StringLib.SQLformat(
                          "INSERT INTO pdj_app_roles(pdj_title,app_title,"
                           + "role_name) VALUES(" + Constants.ST + ","
                           + Constants.ST + "," + Constants.ST + ")",
                          arg_list);
                last_warning_msg ="";
                i_temp = 0;
                try
                {
                  i_temp = SQL_statement.executeUpdate(SQL_query_string);
                  if (i_temp < 0)
                    {
                      last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
                      Log.error(this_servers_name, this_methods_name,
                                last_warning_msg);
                    }
                }
                catch(Exception exception)
                {
                  i_temp = -1;   // Just set this so it will be seen below.
                  last_warning_msg = exception.toString();
                  Log.excp(this_servers_name, this_methods_name,
                  last_warning_msg);
                }
                if (i_temp < 1)
                  {
                    log_file_buffer.insert(0, "Failed to update role "
                                           + arg_list[2].toString() + " for ");
                    log_file_buffer.append(". Aborting all changes!");
                    log_file_writer.write(log_file_buffer.toString() + "\n");
                    Log.error(this_servers_name, this_methods_name,
                    log_file_buffer.toString());
                    error_occured = true;
                    num_errors++;
                    break;
                  }
              }
            if (error_occured)
              {
                db_connection.rollback();
                db_connection.setAutoCommit(true);
              }
            else
              {
                db_connection.setAutoCommit(true);
                log_file_buffer.insert(0, "Successfully updated ");
                log_file_writer.write(log_file_buffer.toString() + "\n");
              }
            // Finished updating
            continue;
          }
        if (requested_change.compareTo("DELETE") == 0)
          {
            arg_list = new Object[2];
            arg_list[0] = ini.GetProfileString(pdjSection, "TITLE",
                                               "\0", iniFilename);
            arg_list[1] = ini.GetProfileString(pdjSection, "GROUP",
                                               "\0", iniFilename);
            // Set up te default message..
            log_file_buffer.setLength(0);
            log_file_buffer.append("PDJ " + arg_list[0].toString()
                                   + " for app " + arg_list[1].toString());

//            SQL_query_string = StringLib.SQLformat(
//                        "DELETE FROM predefined_jobs WHERE pdj_title="
//                         + Constants.ST + " AND app_title=" + Constants.ST,
//                        arg_list);
            SQL_query_string = StringLib.SQLformat(
                        "pdj_title="
                         + Constants.ST + " AND app_title=" + Constants.ST,
                        arg_list);            
            last_warning_msg = "Unknown Error";
            i_temp = 0;
            try
            {
//              i_temp = SQL_statement.executeUpdate(SQL_query_string);
              i_temp = DataLib.cascadeDelete(SQL_statement, "predefined_jobs",
                                             SQL_query_string);
              if (i_temp < 0)
                {
                  last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
                  Log.error(this_servers_name, this_methods_name,
                            last_warning_msg);
                }
            }
            catch(Exception exception)
            {
              i_temp = -1;   // Just set this so it will be seen below.
              last_warning_msg = exception.toString();
              Log.excp(this_servers_name, this_methods_name,
                        last_warning_msg);
            }
            if (i_temp < 1)
              {
                log_file_buffer.insert(0, "Failed to delete ");
                Log.error(this_servers_name, this_methods_name,
                          log_file_buffer.toString());
                num_errors++;
              }
            else
              log_file_buffer.insert(0, "Successfully deleted ");
          
            log_file_writer.write(log_file_buffer.toString() + "\n");
            // Finished deleting...
            continue;
          }
      } // end of while loop..

      log_file_writer.write("Finished.\n" + num_errors + " errors reported.\n");
      log_file_writer.close();

      //Do this processing here.
      Object  bundle_items[] = new Object[1];
      StringBuffer  bundle_buffer = new StringBuffer();
      bundle_items[0] = Constants.NTCSS_PDJ_CHANGE_FILE;
      BundleLib.bundleData(bundle_buffer, "C", bundle_items);

      sendFormattedServerMsg("GOODPDJINI", bundle_buffer.toString());
      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg(err_header,
                             StringLib.valueOf(exception.toString().length(), 5)
                             + exception.toString());
      return;
    }
    finally
    {
      // Close the database resources
      try
      {
        if (SQL_statement != null)
          SQL_statement.close();
        if (db_connection != null)
          DatabaseConnectionManager.releaseConnection(db_connection);
      }
      catch (Exception exception)
      {
        Log.excp(this_servers_name, this_methods_name,
                 exception.toString());
      }
    }
  }
  
  
  /////////////////////////////////////////////////////////////////////////////
   /**
    * This method creates the otype.ini file.
    * A user token is required for authentication.
    */
  
  public void handleCreateOtypeINI()
  {
    String         msg_items[];
    int            msg_item_lengths[];
    final String   err_header = "NOOTYPEINI";
    final String   this_methods_name = "handleCreateOtypeINI";

    try
    {
      if (isServerIdled() == true) {
         sendErrorMessage(err_header, ErrorCodes.ERROR_SERVER_IDLED);
         return;
      }
      
      msg_item_lengths = new int[2];
      msg_items = new String[2];

      msg_item_lengths[0] = Constants.MAX_LOGIN_NAME_LEN;     // login name
      msg_item_lengths[1] = Constants.MAX_TOKEN_LEN;          // token
      
      if (parseData(msg_item_lengths, msg_items) == false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not parse incoming message!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_PARSE_MSG);
          return;
        }

      // Check the token
      IntegerBuffer junk = null;
      if (isUserValid(msg_items[0], msg_items[1], junk) < 0)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not validate user " + msg_items[0]
                     + " and/or token.");
          sendErrorMessage(err_header, ErrorCodes.ERROR_USER_NOT_VALID);
          return;
        }

      // Remove any pre-existing INI file...
      File ini_file = new File(Constants.NTCSS_OTYPE_INI_FILE);
      if (ini_file.exists())
        ini_file.delete();

      if(!PrintLib.createOtypesIni(Constants.NTCSS_OTYPE_INI_FILE))
        {
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_CREATE_INI_FILE);
          return;
        }

      Object  bundle_items[] = new Object[1];
      StringBuffer  bundle_buffer = new StringBuffer();
      bundle_items[0] = Constants.NTCSS_OTYPE_INI_FILE;
      BundleLib.bundleData(bundle_buffer, "C", bundle_items);

      sendFormattedServerMsg("GOODOTYPEINI", bundle_buffer.toString());

      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg(err_header,
                             StringLib.valueOf(exception.toString().length(), 5)
                             + exception.toString());
      return;
    }
  }


  /////////////////////////////////////////////////////////////////////////////
   /**
    * This method applies changes to the database from the given INI file.
    * A user token is required for authentication.
    */

  public void handleUpdateOtypes()
  {
    String         msg_items[];
    int            msg_item_lengths[];
    final String   err_header = "NOOTYPEINI";
    final String   this_methods_name = "handleUpdateOtypes";

    try
    {
      if (isServerIdled() == true) {
         sendErrorMessage(err_header, ErrorCodes.ERROR_SERVER_IDLED);
         return;
      }
      
      msg_item_lengths = new int[3];
      msg_items = new String[3];

      msg_item_lengths[0] = Constants.MAX_LOGIN_NAME_LEN;     // login name
      msg_item_lengths[1] = Constants.MAX_TOKEN_LEN;          // token
      msg_item_lengths[2] = Constants.MAX_PATH_LEN;           // INI location

      if (parseData(msg_item_lengths, msg_items) == false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not parse incoming message!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_PARSE_MSG);
          return;
        }

      // Check the token
      IntegerBuffer junk = null;
      if (isUserValid(msg_items[0], msg_items[1], junk) < 0)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not validate user " + msg_items[0] +
                    " and/or token.");
          sendErrorMessage(err_header, ErrorCodes.ERROR_USER_NOT_VALID);
          return;
        }

      // Make sure the specified INI file exists...
      File ini_file = new File(msg_items[2]);
      if (!ini_file.exists())
        {
          Log.error(this_servers_name, this_methods_name, "Prvided INI file "
                    + "does not exist");
          sendErrorMessage(err_header, ErrorCodes.ERROR_NO_SUCH_FILE);
          return;
        }

      if(!PrintLib.extractOtypeINI(msg_items[2],
                                   Constants.NTCSS_OTYPE_CHANGE_FILE))
        {
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_EXTRACT_INI_FILE);
          return;
        }

      Object  bundle_items[] = new Object[1];
      StringBuffer  bundle_buffer = new StringBuffer();
      bundle_items[0] = Constants.NTCSS_OTYPE_CHANGE_FILE;
      BundleLib.bundleData(bundle_buffer, "C", bundle_items);

      sendFormattedServerMsg("GOODOTYPEINI", bundle_buffer.toString());

      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg(err_header,
                             StringLib.valueOf(exception.toString().length(), 5)
                             + exception.toString());
      return;
    }
  }


  /////////////////////////////////////////////////////////////////////////////
   /**
    * This method applies Printer modifications (specified in a provided INI) to
    * the database.  This method is synchronized since we never added a user ID
    * to the Apply or Log files like we did for user admin.  This should prevent
    * multiple instances overwriting each other's INI files should more than one
    * person be editing pritners at the same time.
    * No authentication is required.
    */

  public synchronized void handlePrinterINIextract()
  {
    final String   err_header = "BADDPPROCPRTADMIN";
    final String   this_methods_name = "handlePrinterINIextract";

    try
    {
      // Write the entire message payload contents to file..
      FileWriter file_writer = new FileWriter(Constants.PRT_ADMIN_APPLY_FILE,
                                              false);
      file_writer.write(GetMessagePayloadString());
      file_writer.close();

      boolean tmp = PrintLib.extractPrinterIni(Constants.PRT_ADMIN_APPLY_FILE,
                                            Constants.PRT_ADMIN_APPLY_LOG_FILE);
      if (!tmp)
        {
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_EXTRACT_INI_FILE);
          return;
        }

      StringBuffer  log_buffer = new StringBuffer(0);

      // Try to open the log file for reading
      FileReader  file_reader;
      try
        {
          file_reader = new FileReader(Constants.PRT_ADMIN_APPLY_LOG_FILE);
        }
      catch (Exception exception)
        {
          Log.excp(this_servers_name, this_methods_name,
                    "Unable to open " + Constants.PRT_ADMIN_APPLY_LOG_FILE);
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_OPEN_FILE);
          return;
        }

      BufferedReader buffered_reader = new BufferedReader(file_reader);
      if (!buffered_reader.ready())
        {
          Log.error(this_servers_name, this_methods_name,
                    "Unable to read " + Constants.PRT_ADMIN_APPLY_LOG_FILE);
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_READ_VALUE);
          return;
        }

      // prime the temp variable..
      String  next_line = buffered_reader.readLine();
      while (next_line != null)
        {
          log_buffer.append(next_line + '\n');
          // Read in the next line from the INI "file" string
          try
            {
              next_line = buffered_reader.readLine();
            }
          catch (Exception exception)
            {
              Log.excp(this_servers_name, this_methods_name,
                       exception.toString());
              sendErrorMessage(err_header, ErrorCodes.ERROR_CAUGHT_EXCEPTION);
              return;
            }
        }

      // Send back the log file..
      sendFormattedServerMsg("GOODDPPROCPRTADMIN",
                             log_buffer.toString() + '\0');
      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg(err_header,
                             StringLib.valueOf(exception.toString().length(), 5)
                             + exception.toString());
      return;
    }
  }


  /////////////////////////////////////////////////////////////////////////////
   /**
    * This method applies User admin modifications (specified in a provided INI)
    * to the database.  The message is expected from a DP.
    * No authentication is required.
    */

  public synchronized void ProcessDistributedUsrAdminChanges()
  {
    final String   err_header = "BADUSRADMAPPLY";
    final String   this_methods_name = "ProcessDistributedUsrAdminChanges";

    try
    {
      File  ini_path = new File(Constants.NTCSS_LOGS_DIR);
      // Create a temp file to store the INI contents in.
      File  input_ini_file = File.createTempFile("UserAdminApply", ".INI",
                                                 ini_path);
      // Write the entire message payload contents to file..
      FileWriter file_writer = new FileWriter(input_ini_file);
      file_writer.write(GetMessagePayloadString());
      file_writer.close();

      // Create a temp Log file for the extraction process...
      File  log_file = File.createTempFile("UserAdminLog", ".INI", ini_path);

      Log.debug(this_servers_name, this_methods_name, "Input file: "
          + input_ini_file.getCanonicalPath() + " Log file: "
          + log_file.getCanonicalPath());

      int  n;
      UserAdmin user_info = new UserAdmin(input_ini_file.getCanonicalPath(),
                                          log_file.getCanonicalPath());

      if (user_info.last_error != 0)
        {
          String err_message = "Class returned error " + user_info.last_error;
          Log.error(this_servers_name, this_methods_name, err_message);
          sendFormattedServerMsg(err_header,
                                 StringLib.valueOf(err_message.length(), 5)
                                                                 + err_message);
          return;
        }

      n = user_info.ExtractUserINIchanges();

      // Now we get the contents of the log file and stream it back to the DP..
      StringBuffer  log_buffer = new StringBuffer(0);
      FileReader  file_reader;
      try
        {
          file_reader = new FileReader(log_file);
        }
      catch (Exception exception)
        {
          Log.excp(this_servers_name, this_methods_name, exception.toString());
          String err_message = "Class returned error " + user_info.last_error;
          sendFormattedServerMsg(err_header,
                             StringLib.valueOf(exception.toString().length(), 5)
                             + exception.toString());
          return;
        }

      BufferedReader buffered_reader = new BufferedReader(file_reader);
      if (!buffered_reader.ready())
        {
          String err_message = "Unable to read log file!";
          Log.error(this_servers_name, this_methods_name, err_message);
          sendFormattedServerMsg(err_header,
                                 StringLib.valueOf(err_message.length(), 5)
                                                                 + err_message);
          return;
        }

      // prime the temp variable..
      String  next_line = buffered_reader.readLine();
      while (next_line != null)
        {
          log_buffer.append(next_line + '\n');
          // Read in the next line from the INI "file" string
          try
            {
              next_line = buffered_reader.readLine();
            }
          catch (Exception exception)
            {
              Log.excp(this_servers_name, this_methods_name, exception.toString());
              sendFormattedServerMsg(err_header,
                             StringLib.valueOf(exception.toString().length(), 5)
                             + exception.toString());
              return;
            }
        }

      // Send back the log file..
      sendFormattedServerMsg("GOODDPPROCUSRADMIN", log_buffer.toString() + '\0');
      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg(err_header,
                             StringLib.valueOf(exception.toString().length(), 5)
                             + exception.toString());
      return;
    }
  }


  /////////////////////////////////////////////////////////////////////////////
   /**
    * This method applies user modifications (specified in a provided INI) to
    * the database.
    * A user token is required for authentication.
    */

  public void handleUserAdminChanges()
  {
    String         msg_items[];
    int            msg_item_lengths[];
    final String   err_header = "BADUSRADMAPPLY";
    final String   this_methods_name = "handleUserAdminChanges";

    try
    {
      msg_item_lengths = new int[2];
      msg_items = new String[2];

      msg_item_lengths[0] = Constants.MAX_LOGIN_NAME_LEN;     // login name
      msg_item_lengths[1] = Constants.MAX_TOKEN_LEN;          // token

      if (parseData(msg_item_lengths, msg_items) == false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not parse incoming message!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_PARSE_MSG);
          return;
        }

      int  n;

      UserAdmin user_info = new UserAdmin(Constants.USR_ADMIN_APPLY_FILE
                                           + "." + msg_items[0],
                                          Constants.USR_ADMIN_APPLY_LOG_FILE
                                           + "." + msg_items[0]);

      if (user_info.last_error != 0)
        {
          sendErrorMessage(err_header,  user_info.last_error);
          Log.error(this_servers_name, this_methods_name,
                    ErrorCodes.getErrorMessage(user_info.last_error));
          return;
        }

      n = user_info.ExtractUserINIchanges();

      // Get rid of the original apply file...
      if (!Log.isDebugOn())
        {
          File ini_file = new File(Constants.USR_ADMIN_APPLY_FILE
                                    + "." + msg_items[0]);
          ini_file.delete();
        }

      if (n != 0)
        sendErrorMessage(err_header, user_info.last_error);
      else
        sendFormattedServerMsg("GOODUSRADMAPPLY",
                               Constants.USR_ADMIN_APPLY_LOG_FILE
                                + "." + msg_items[0]);
      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg(err_header,
                             StringLib.valueOf(exception.toString().length(), 5)
                             + exception.toString());
      return;
    }
  }


  /////////////////////////////////////////////////////////////////////////////
   /**
    * This method returns information about what OS and hardware we are running
    * on.
    * A user token is required for authentication.
    */

  public void handleGetSystemInfo()
  {
    String         msg_items[];
    int            msg_item_lengths[];
    final String   err_header = "BADGETSYSTEMINFO";
    final String   this_methods_name = "handleGetSystemInfo";

    try
    {
      msg_item_lengths = new int[2];
      msg_items = new String[2];

      msg_item_lengths[0] = Constants.MAX_LOGIN_NAME_LEN;     // login name
      msg_item_lengths[1] = Constants.MAX_TOKEN_LEN;          // token

      if (parseData(msg_item_lengths, msg_items) == false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not parse incoming message!");
          sendErrorMessage(err_header, ErrorCodes.ERROR_CANT_PARSE_MSG);
          return;
        }

      // Validate requestor's ID
      int n;
      if (msg_items[1].regionMatches(0, Constants.SERVER_TOKEN_STRING, 0,
                                     Constants.SERVER_TOKEN_STRING.length())
                            == true)
        n = doServerValidate(msg_items[0], msg_items[1], getClientAddress());
      else
        {
          IntegerBuffer junk = null;
          n = isUserValid(msg_items[0], msg_items[1], junk);
        }
      if (n < 0)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not validate user " + msg_items[0]
                     + " and/or token.");
          sendErrorMessage(err_header, n);
          return;
        }

      // Get the system info....
      Object  bundle_items[] = new Object[7];
      bundle_items[0] = "CCCCCC";
      bundle_items[1] = System.getProperty("os.name");
      bundle_items[2] = System.getProperty("os.arch");
      bundle_items[3] = System.getProperty("os.version");
      bundle_items[4] = System.getProperty("file.separator");
      bundle_items[5] = System.getProperty("path.separator");
      bundle_items[6] = System.getProperty("line.separator");

      sendFormattedServerMsg("GOODGETSYSTEMINFO",
                             BundleLib.smartBundle(bundle_items) + "\0");
      
      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg(err_header,
                             StringLib.valueOf(exception.toString().length(), 5)
                             + exception.toString() + "\0");
      return;
    }
  }


  /////////////////////////////////////////////////////////////////////////////
   /**
    * This method enacts a backup copy to take palce between the master and other
    * hosts.
    * No authentication is required.
    */

  public void handleCopyBackups()
  {
    final String   err_header = "BADCOPYMASTERFILES";
    final String   this_methods_name = "handleCopyBackups";

    try
    {
      int  n;
      n = ReplicationLib.copyMasterBackupFiles();
      if (n !=0)
        sendFormattedServerMsg("GOODCOPYMASTERFILES", n + "\0");
      else
        sendFormattedServerMsg(err_header, n + "\0");

      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg(err_header,
                             StringLib.valueOf(exception.toString().length(), 5)
                             + exception.toString());
      return;
    }
  }


  /////////////////////////////////////////////////////////////////////////////
   /**
    * This method causes the local server to pull new maps from the master NDS
    * server.  At current, this is only needed for NIS backwards compatibility
    * and is pretty much here to prevent error meessages in legacy code when NIS
    * is no longer used.  Locally, it doesn't do anything
    * but reply with an "OK".
    */
  public void handleRetrieveNDSupdates()
  {
    String         msg_items[];
    int            msg_item_lengths[];

    final String   err_header = "BADNISUPDATE";
    final String   this_methods_name = "handleRetrieveNDSupdates";

    try
    {
      // Note: a null char is always appended to the end of the message
      // by the DPs.
      Log.debug(this_servers_name, this_methods_name,
                GetMessagePayloadString());        

        sendFormattedServerMsg("GOODNISUPDATE", "");
        return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      sendFormattedServerMsg(err_header,
                             StringLib.valueOf(exception.toString().length(), 5)
                             + exception.toString());
      return;
    }
  }





  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
  // The following functions are only called by the message handlers above.
  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////




  /////////////////////////////////////////////////////////////////////////////
   /**
    * Replaces the specified user's password in the ntcss database with the
    * value specified.  The os /etc/password file no longer contains ntcss
    * passwords.
    */
  private static int changeUserPassword( String     login_name,
                                         String     old_password,
                                         String     new_password)
  {
    final String  this_methods_name = "changeUserPassword";
    String     SQL_query_string;
    Connection db_connection = null;
    Statement  SQL_statement = null;
    ResultSet  db_result_set = null;
    Object     arg_list[];
    //int errorCode = false
    String     db_password;

    try
    {
      db_connection = DatabaseConnectionManager.getConnection();
      if (db_connection == null)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Null DB connection returned!");
          return ErrorCodes.ERROR_CANT_ACCESS_DB;
        }

      arg_list = new Object[1];
      arg_list[0] = login_name;

      SQL_query_string = StringLib.SQLformat(
              "SELECT password FROM users WHERE login_name=" + Constants.ST,
              arg_list);

      SQL_statement = db_connection.createStatement();
      db_result_set = SQL_statement.executeQuery(SQL_query_string);

      if (db_result_set.next() == false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Failed to get data for user " + login_name + "!");
          return ErrorCodes.ERROR_NO_SUCH_USER;
        }

      db_password = db_result_set.getString("password");

      // See if the provided password matches the one already
      // in the database..
      if (! db_password.equals(old_password))
        {
          // Something bad happened so we get out of here..
          Log.error(this_servers_name, this_methods_name,
                    "Incorrect password supplied!");
          return ErrorCodes.ERROR_INCORRECT_OLD_PASSWD;
        }

      // Now change the password in the RDBMS...
      arg_list = new Object[3];
      arg_list[0] = new_password;
      arg_list[1] = StringLib.getTimeStr();
      arg_list[2] = login_name;
      SQL_query_string = StringLib.SQLformat(
                            "UPDATE users SET password=" + Constants.ST
                             + ", pw_change_time = " + Constants.ST 
                             + " WHERE login_name=" + Constants.ST,
                            arg_list);
      int tmp;
      String last_warning_msg ="";
      int i_temp = 0;
      try
      {
        i_temp = SQL_statement.executeUpdate(SQL_query_string);
        if (i_temp < 0)
          {
            last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
            Log.error(this_servers_name, this_methods_name, last_warning_msg);
          }
      }
      catch(Exception exception)
      {
        i_temp = -1;   // Just set this so it will be seen below.
        last_warning_msg = exception.toString();
        Log.excp(this_servers_name, this_methods_name, last_warning_msg);
      }
      if (i_temp < 1)  // Should have at least update one record
        {
          Log.error(this_servers_name, this_methods_name,
                    "DB update failed for user " + login_name + "!");
          return ErrorCodes.ERROR_DB_UPDATE_FAILED;
        }
      return 0;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      return ErrorCodes.ERROR_PASSWORD_CHANGE_FAILED;
    }
    finally
    {
      // Close the database resources
      try
      {
        if (db_result_set != null)
          db_result_set.close();
        if (SQL_statement != null)
          SQL_statement.close();
        if (db_connection != null)
          DatabaseConnectionManager.releaseConnection(db_connection);
      }
      catch (Exception exception)
      {
        Log.excp(this_servers_name, this_methods_name,
                 exception.toString());
      }
    }
  }


  /////////////////////////////////////////////////////////////////////////////
   /**
    * Replaces the specified appuser's custom data field with the value given.
    */

  private static int changeCustomAppuserData( String   login_name,
                                              String   app_title,
                                              String   new_custom_data )
  {
    final String  this_methods_name = "changeCustomAppuserData";
    String     SQL_query_string;
    Connection db_connection = null;
    Statement  SQL_statement = null;
    ResultSet  db_result_set = null;
    Object     arg_list[];
    
    try
    {
      db_connection = DatabaseConnectionManager.getConnection();
      if (db_connection == null)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Null DB connection returned!");
          return ErrorCodes.ERROR_CANT_ACCESS_DB;
        }

      // Make sure that the target app lives on this host.
      arg_list = new Object[2];
      arg_list[0] = app_title;
      arg_list[1] = StringLib.getHostname();

      // We just request app_lock since it's a small piece of data.
      SQL_query_string = StringLib.SQLformat(
                  "SELECT app_lock FROM apps WHERE app_title ="
                   + Constants.ST + " AND UPPER(hostname)=UPPER("
                   + Constants.ST + ")", arg_list);

      SQL_statement = db_connection.createStatement();
      db_result_set = SQL_statement.executeQuery(SQL_query_string);

      if (db_result_set.next()== false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "App " + app_title + "does not reside on this host");
          return ErrorCodes.ERROR_WRONG_HOST;
        }

      arg_list = new Object[3];
      arg_list[0] = new_custom_data;
      arg_list[1] = app_title;
      arg_list[2] = login_name;

      SQL_query_string = StringLib.SQLformat("Update app_users SET "
                          + "custom_app_data=" + Constants.ST
                          + " WHERE app_title =" + Constants.ST
                          + " AND login_name=" + Constants.ST,
                         arg_list);
      int tmp;
      String last_warning_msg ="";
      int i_temp = 0;
      try
      {
        i_temp = SQL_statement.executeUpdate(SQL_query_string);
        if (i_temp < 0)
          {
            last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
            Log.error(this_servers_name, this_methods_name, last_warning_msg);
          }
      }
      catch(Exception exception)
      {
        i_temp = -1;   // Just set this so it will be seen below.
        last_warning_msg = exception.toString();
        Log.excp(this_servers_name, this_methods_name, last_warning_msg);
      }
      if (i_temp < 1)
        {
          Log.error(this_servers_name, this_methods_name, "DB update failed "
                + " for appuser " + login_name + ", app = " + app_title);
          return ErrorCodes.ERROR_DB_UPDATE_FAILED;
        }
      return 0;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      return ErrorCodes.ERROR_DB_UPDATE_FAILED;
    }
    finally
    {
      // Close the database resources
      try
      {
        if (db_result_set != null)
          db_result_set.close();
        if (SQL_statement != null)
          SQL_statement.close();
        if (db_connection != null)
          DatabaseConnectionManager.releaseConnection(db_connection);
      }
      catch (Exception exception)
      {
        Log.excp(this_servers_name, this_methods_name,
                 exception.toString());
      }
    }
  }

  /////////////////////////////////////////////////////////////////////////////
   /**
    * Retrieves the specified appuser's custom data field for the app_title given.
    */

  private static int getCustomAppuserData( String        login_name,
                                           String        app_title,
                                           StringBuffer  custom_app_data)
  {
    final String  this_methods_name = "getCustomAppuserData";
    String     SQL_query_string;
    Connection db_connection = null;
    Statement  SQL_statement = null;
    ResultSet  db_result_set = null;
    Object     arg_list[];

    try
    {
      if (custom_app_data == null)
        {
          Log.error(this_servers_name, this_methods_name, "null StringBuffer");
          return ErrorCodes.ERROR_BAD_METHOD_ARGUMENT;
        }

      custom_app_data.setLength(0);             // Clear out previous contents

      db_connection = DatabaseConnectionManager.getConnection();
      if (db_connection == null)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Null DB connection returned!");
          return ErrorCodes.ERROR_CANT_ACCESS_DB;
        }

      // Make sure that the target app lives on this host.
      arg_list = new Object[2];
      arg_list[0] = app_title;
      arg_list[1] = StringLib.getHostname();

      // We just request app_lock since it's a small piece of data.
      SQL_query_string = StringLib.SQLformat(
              "SELECT app_lock FROM apps WHERE app_title =" + Constants.ST +
              " AND UPPER(hostname)=UPPER(" + Constants.ST + ")", arg_list);

      SQL_statement = db_connection.createStatement();
      db_result_set = SQL_statement.executeQuery(SQL_query_string);

      if (db_result_set.next()== false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "App " + app_title + " does not reside on this host");
          return ErrorCodes.ERROR_WRONG_HOST;
        }

      arg_list = new Object[2];
      arg_list[0] = app_title;
      arg_list[1] = login_name;

      SQL_query_string = StringLib.SQLformat(
                "SELECT custom_app_data FROM app_users WHERE app_title ="
                 + Constants.ST + " AND login_name=" + Constants.ST, arg_list);

      db_result_set = SQL_statement.executeQuery(SQL_query_string);

      if (db_result_set.next() == false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Failed to get the data for appuser " + login_name + "!");
          return ErrorCodes.ERROR_NO_SUCH_APP_USER;
        }

      // We don't check for what's returned since empty strings are OK too.
      custom_app_data.insert(0, db_result_set.getString("custom_app_data"));

      return 0;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      return ErrorCodes.ERROR_DB_QUERY_FAILED;
    }
    finally
    {
      // Close the database resources
      try
      {
        if (db_result_set != null)
          db_result_set.close();
        if (SQL_statement != null)
          SQL_statement.close();
        if (db_connection != null)
          DatabaseConnectionManager.releaseConnection(db_connection);
      }
      catch (Exception exception)
      {
        Log.excp(this_servers_name, this_methods_name,
                 exception.toString());
      }
    }
  }
  
  /////////////////////////////////////////////////////////////////////////////
  /**
   * Retrieves the specified appuser's custom data field for the app_title given.
   */

  public static int getAppBatchUserName( String        login_name,
                                         String        app_title,
                                         StringBuffer  batch_user_name)
  {
    final String  this_methods_name = "getAppBatchUserName";
    String     SQL_query_string;
    Connection db_connection = null;
    Statement  SQL_statement = null;
    ResultSet  db_result_set = null;
    Object     arg_list[];

    try
    {
      if (batch_user_name == null)
        {
          Log.error(this_servers_name, this_methods_name, "null StringBuffer");
          return ErrorCodes.ERROR_BAD_METHOD_ARGUMENT;
        }

      batch_user_name.setLength(0);             // Clear out previous contents

      db_connection = DatabaseConnectionManager.getConnection();
      if (db_connection == null)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Null DB connection returned!");
          return ErrorCodes.ERROR_CANT_ACCESS_DB;
        }

      // Make sure that the target app lives on this host.
      arg_list = new Object[2];
      arg_list[0] = app_title;
      arg_list[1] = StringLib.getHostname();

      // We just request app_lock since it's a small piece of data.
      SQL_query_string = StringLib.SQLformat(
                   "SELECT app_lock FROM apps WHERE app_title =" + Constants.ST
                    + " AND UPPER(hostname)=UPPER(" + Constants.ST + ")",
                    arg_list);

      SQL_statement = db_connection.createStatement();
      db_result_set = SQL_statement.executeQuery(SQL_query_string);

      if (db_result_set.next()== false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "App " + app_title + " does not reside on this host");
          return ErrorCodes.ERROR_WRONG_HOST;
        }

      arg_list = new Object[1];
      arg_list[0] = app_title;

      SQL_query_string = StringLib.SQLformat("SELECT login_name FROM "
                          + "app_users WHERE app_title =" + Constants.ST
                          + "' AND batch_user = 1",
                         arg_list);

      db_result_set = SQL_statement.executeQuery(SQL_query_string);

      if (db_result_set.next() == false)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Failed to locate a batch user for app" + app_title);
          return ErrorCodes.ERROR_NO_SUCH_USER;
        }

      // We don't check for what's returned since empty strings are OK too.
      batch_user_name.insert(0, db_result_set.getString("login_name"));

      return 0;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      return ErrorCodes.ERROR_DB_QUERY_FAILED;
    }
    finally
    {
      // Close the database resources
      try
      {
        if (db_result_set != null)
          db_result_set.close();
        if (SQL_statement != null)
          SQL_statement.close();
        if (db_connection != null)
          DatabaseConnectionManager.releaseConnection(db_connection);
      }
      catch (Exception exception)
      {
        Log.excp(this_servers_name, this_methods_name,
                 exception.toString());
      }
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  /**
   * Creates the User Config GUI's user list file.
   * If the StringBuffer passed in is not null, the information is stored in it.
   */

  public static int CreateUCUserCacheFile( StringBuffer provided_buffer)
  {
    final String  this_methods_name = "CreateUCUserCacheFile";
    String        SQL_query_string;
    Connection    db_connection = null;
    Statement     SQL_statement = null;
    ResultSet     db_result_set = null;
    Object        arg_list[];
    Object        bundle_items[];
    StringBuffer  bundle_buffer = new StringBuffer();
    StringBuffer  data_buffer;
    
    try
    {
      // Establish the DB connection.
      db_connection = DatabaseConnectionManager.getConnection();
      if (db_connection == null)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Null DB connection returned!");
          return ErrorCodes.ERROR_CANT_ACCESS_DB;
        }

      // Make sure the data buffer passed in is OK...
      if (provided_buffer != null)
        data_buffer = provided_buffer;
      else
        data_buffer = new StringBuffer();

      SQL_statement = db_connection.createStatement();

      // First we'll create the user list since it comes first in the output..
      SQL_query_string =  "SELECT login_name,real_name,login_lock,ss_number,"
                    + "phone_number,auth_server FROM users ORDER BY login_name";
      db_result_set = SQL_statement.executeQuery(SQL_query_string);

      int counter = 0;
      while (db_result_set.next() == true)
        {
          data_buffer.append(db_result_set.getString("login_name") +(char) 1
                              + db_result_set.getString("real_name") +(char) 1);
          if (db_result_set.getBoolean("login_lock"))
            data_buffer.append("Disabled" +(char) 1);
          else
            data_buffer.append("Idle" +(char) 1);
          data_buffer.append(db_result_set.getString("ss_number") +(char) 1 +
          db_result_set.getString("phone_number") +(char) 1 +
          db_result_set.getString("auth_server") + "\n");
          counter++;
        }

      // Prepend in the bundled number of users we just got..
      bundle_items = new Object[1];

      bundle_items[0] = new Integer(counter);
      bundle_buffer.setLength(0);
      BundleLib.bundleData(bundle_buffer, "I", bundle_items);
      data_buffer.insert(0, bundle_buffer.toString());

      // Get necessary sys_conf data..
      arg_list = new Object[1];
      arg_list[0] = "NTUSERDIR";
      SQL_query_string = StringLib.SQLformat(
                      "SELECT value FROM sys_conf WHERE tag=" + Constants.ST,
                      arg_list);

      //StringBuffer temp_buffer = new StringBuffer();
      //String       temp_sting;
      // Run the query
      db_result_set = SQL_statement.executeQuery(SQL_query_string);
      if (db_result_set.next() == true)
        {
          // probably isn't necessary anymore so it was commented out..
          // It just replaces any "^" with "\\"
          /*
          temp_string = db_result_set.getString(1);
          int end_index = temp_string.indexOf('^', 0);
          int start_index = 0
          temp_buffer.setLength(0);
          while (end_index != -1) // means no further matches
            {
              temp_buffer.insert(temp_string.substring(start_index, end_index));
              start_index = end_index + 1;  // move just beyond the carat.
              end_index = temp_string.indexOf('^', start_index);
            }
          temp_buffer.insert(temp_string.substring(start_index,
                                                   temp_string.length()));
          ini_writer.write(temp_buffer.toString()");
           */
          //bypasses the above junk...
          data_buffer.append(db_result_set.getString(1));
        }

      // Store the NTTMPUSER value...
      arg_list[0] = "NTTMPUSER";
      SQL_query_string = StringLib.SQLformat(
                      "SELECT value FROM sys_conf WHERE tag=" + Constants.ST,
                     arg_list);
      db_result_set = SQL_statement.executeQuery(SQL_query_string);
      if (db_result_set.next() == true)
        data_buffer.append( "%" + db_result_set.getString(1));
      else
        data_buffer.append("%");           // delimit the null value.

      // Store the NTPROFDIR value...
      arg_list[0] = "NTPROFDIR";
      SQL_query_string = StringLib.SQLformat(
                        "SELECT value FROM sys_conf WHERE tag=" + Constants.ST,
                       arg_list);

      // Run the query
      db_result_set = SQL_statement.executeQuery(SQL_query_string);
      if (db_result_set.next() == true)
        {
          // probably isn't necessary anymore so it was commented out..
          // It just replaces any "^" with "\\"
            /*
            temp_string = db_result_set.getString(1);
            int end_index = temp_string.indexOf('^', 0);
            int start_index = 0
            temp_buffer.setLength(0);
            while (end_index != -1) // means no further matches
              {
                temp_buffer.insert(temp_string.substring(start_index, end_index));
                start_index = end_index + 1;  // move just beyond the carat.
                end_index = temp_string.indexOf('^', start_index);
              }
            temp_buffer.insert(temp_string.substring(start_index,
                                                     temp_string.length()));
            ini_writer.write(temp_buffer.toString() + "\n");
             */
          data_buffer.append( "%" + db_result_set.getString(1) + "\n");
        }
      else
        data_buffer.append("%\n");

      // Get all the non-editable users...
      // Find out which apps this user has:
      arg_list[0] = new Boolean(false);
      SQL_query_string = StringLib.SQLformat(
                            "SELECT login_name FROM users WHERE editable_user="
                             + Constants.ST, arg_list);

      db_result_set = SQL_statement.executeQuery(SQL_query_string);

      boolean  first_one = true;
      while (db_result_set.next() == true)
        {
          if (first_one)
            {
              data_buffer.append(db_result_set.getString(1));
              first_one = false;
            }
          else
            data_buffer.append("%" + db_result_set.getString(1));
        }
      data_buffer.append("\n");

      // Append in the authserver user loads...
/*    This sql statement not only returns the wrong load numbers but also
      doesn't include auth servers which don't have any users associated with them.
 *  
      SQL_query_string = "SELECT auth_server, COUNT(auth_server) as n_users "
                          + "FROM users a, hosts b, apps c "
                          + "WHERE a.auth_server=c.hostname "
                          +   "AND c.hostname=b.hostname AND b.type<>1 "
                          + "GROUP BY auth_server";
 */
      SQL_query_string = "SELECT hostname AS name, COUNT(auth_server) AS " +
                 "n_users FROM hosts LEFT OUTER JOIN users ON hostname = " +
                 "auth_server WHERE type != 1 GROUP BY hostname";

      db_result_set = SQL_statement.executeQuery(SQL_query_string);

      first_one = true;
      while (db_result_set.next() == true)
        {
          if (first_one)
            {
              data_buffer.append(db_result_set.getString("name") + "="
                                          + db_result_set.getString("n_users"));
              first_one = false;
            }
          else
            data_buffer.append("%" + db_result_set.getString("name")
                                   + "=" + db_result_set.getString("n_users"));
        }
      data_buffer.append("\n");

      // prepend in the bundled length of everything so far..
      bundle_items[0] = new Integer(data_buffer.length());
      bundle_buffer.setLength(0);
      BundleLib.bundleData(bundle_buffer, "I", bundle_items);
      data_buffer.insert(0, bundle_buffer.toString());

      // prepend in the bundled length of everything so far..
      //(yes, this does appear redundant!)

      // prepend in the bundled length of everything so far..
      bundle_items[0] = new Integer(data_buffer.length());
      bundle_buffer.setLength(0);
      BundleLib.bundleData(bundle_buffer, "I", bundle_items);
      data_buffer.insert(0, bundle_buffer.toString());

      // Now write everything out to the file..
      File users_file = new File(Constants.USER_CONFIG_USERS_CACHE_FILE);
      FileWriter file_writer = new FileWriter(users_file);

      file_writer.write(data_buffer.toString());
      file_writer.close();

      return 0;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      return ErrorCodes.ERROR_DB_QUERY_FAILED;
    }
    finally
    {
      // Close the database resources
      try
      {
        if (db_result_set != null)
          db_result_set.close();
        if (SQL_statement != null)
          SQL_statement.close();
        if (db_connection != null)
          DatabaseConnectionManager.releaseConnection(db_connection);
      }
      catch (Exception exception)
      {
        Log.excp(this_servers_name, this_methods_name,
                 exception.toString());
      }
    }
  }

  /////////////////////////////////////////////////////////////////////////////
   /**
    * Replaces the database password (link_data) for the app specified.
    */
  private static int SetLinkDataForApp( String     link_data,
                                        String     app_title)
  {
    Connection     db_connection = null;
    Statement      SQL_statement = null;
    final String  this_methods_name = "SetLinkDataForApp";

    try
    {
      Log.debug(this_servers_name, this_methods_name,
                "Was called with app_title=<" + app_title + "> and link_data <"
                + link_data + ">");
      // Encrypt the link data before we store it.. Useless but for consistency.
      // We use the app name for the key..
      StringBuffer  encrypted_string = new StringBuffer(); 
      if (CryptLib.plainEncryptString( app_title + app_title, link_data,
                                       encrypted_string, false, true) != 0)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Unable to encrypt DB password for " + app_title);
          return -1;  // -1 For lack of a specific error code....
        }

      String         SQL_query_string;
      Object         arg_list[];

      // Establish a DB connection.
      db_connection = DatabaseConnectionManager.getConnection();
      if (db_connection == null)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Null DB connection returned!");
          return ErrorCodes.ERROR_CANT_ACCESS_DB;
        }

      arg_list = new Object[2];
      arg_list[0] = encrypted_string.toString();
      arg_list[1] = app_title;             // app name or hostname

      SQL_query_string = StringLib.SQLformat( "UPDATE apps SET link_data="
                            + Constants.ST + " WHERE app_title=" + Constants.ST,
                            arg_list);
      Log.debug(this_servers_name, this_methods_name, "SQL = " 
                                                            + SQL_query_string);
      SQL_statement = db_connection.createStatement();
      String  last_warning_msg = "";
      int i_temp = 0;

      try
      {
        i_temp = SQL_statement.executeUpdate(SQL_query_string);
        if (i_temp < 0)
          {
            last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
            Log.error(this_servers_name, this_methods_name, last_warning_msg);
          }
      }
      catch(Exception exception)
      {
        i_temp = -1;   // Just set this so it will be seen below.
        last_warning_msg = exception.toString();
        Log.excp(this_servers_name, this_methods_name, last_warning_msg);
      }
      if (i_temp < 1)
        return  ErrorCodes.ERROR_DB_UPDATE_FAILED;

      return 0;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      return ErrorCodes.ERROR_CAUGHT_EXCEPTION;
    }
    finally
    {
      // Close the database resources
      try
      {
        if (SQL_statement != null)
          SQL_statement.close();
        if (db_connection != null)
          DatabaseConnectionManager.releaseConnection(db_connection);
      }
      catch (Exception exception)
      {
        Log.excp(this_servers_name, this_methods_name,
                 exception.toString());
      }
    }
  }

}  // End of Database Server class definition.



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// The following classes/methods are all related to user admininistration.
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/**
 * Lowest level structure.  These flags record what happened for a single type
 *  of change.
 **/
class ChangeResult
{
  static final int UNDO_REQUESTED  =  1;
  static final int UNDO_SUCCESSFUL =  2;
  static final int UNDO_FAILED     = -1;
  static final int NONE            =  0;

  boolean  f_requested = false;
  boolean  f_failed    = false;

  // UNDO_REQUESTED/UNDO_SUCCESSFUL/UNDO_FAILED
  int      undo_status  = NONE;
  boolean  f_skip       = false;
}


/** Second lowest structure.  This class records what happened on each type of
 * server the the changes get distributed to.  The auth's part usually is only
 * relevant on the master server applying the changes.
 **/
class ChangesStatus
{
  ChangeResult  NDS_part      = new ChangeResult();
  ChangeResult  local_os_part = new ChangeResult();
  ChangeResult  local_db_part = new ChangeResult();
  ChangeResult  auths_db_part = new ChangeResult();
  ChangeResult  auths_os_part = new ChangeResult();
}


/** Used to track all the possible changes that need to be made for a single
 *   INI file.
 * If another change entry is added, increase the NUM_CHANGE_TYPES define.
 **/
class ChangeTypes
{
  ChangesStatus  add_user          = new ChangesStatus();
  ChangesStatus  remove_user       = new ChangesStatus();
  ChangesStatus  change_ssn        = new ChangesStatus();
  ChangesStatus  change_ph_num     = new ChangesStatus();
  ChangesStatus  change_sec_class  = new ChangesStatus();
  ChangesStatus  change_password   = new ChangesStatus();
  ChangesStatus  change_roles      = new ChangesStatus();
  int            apps_changed = 0;
  ChangesStatus  change_full_name  = new ChangesStatus();
  ChangesStatus  change_auth_server = new ChangesStatus();
}

/** Just a simple class to hold a single app_role and what to do with it.
 **/
class AppRoles
{
  String   name;
  int      action;
}

/** This class contians information about what to do with appuser changes,
 * before and after (on the master server) the change is made.
 **/
class RoleChanges
{
  String   app_server_name;
  String   app_title;
  boolean  no_log_file;
  boolean  skipped;      // Something about the app failed on the master.
  int      app_server_type;
  boolean  new_app;
  boolean  remove_appuser;
  //int      roles_changed;
  String   group_name;
  Vector        role_info = new Vector(0, 1);
  ChangeResult  users     = new ChangeResult(); // Have to add user's record.
  ChangeResult  appusers  = new ChangeResult(); // Have to add appuser's record.
  ChangeResult  appuser_roles = new ChangeResult();// Have to add user role rec.
  ChangeResult  GID       = new ChangeResult();  // Have to add user to OS group
}


/** This class holds all the user's original data (if it exists) so that it
 * can be restored if something goes wrong in the middle of the many different
 * types of changes.
 **/
class OldUserData
{
  String   str_full_name = "";
  String   str_passwd_enc = "";  // Used in undoPassword.
  int      i_os_id = -1;
  String   str_ssn = "";
  String   str_phone_number = "";
  int      i_security_class = -1;
  String   str_pw_change_time = "";
  boolean  login_lock = false;
  String   str_db_passwd = "";
  String   str_auth_server = "";
  boolean  f_editable_user = true;
  boolean  ntcss_super_user = false;
}


/** This class contains all the data about the target user.  The fields hold
 * all the new userdata given in the apply file, and maintains a link to the
 * original user data in case of any undo operation needed in the event of an
 * any error on a rmote auth server (from the master's point of view).
 **/
class UserData
{
  String    str_login_name;
  String    str_full_name;
  String    str_passwd_plaintext;
  String    str_passwd_enc;
  int       i_os_id;
  String    str_ssn;
  String    str_phone_number;
  int       i_security_class;
  String    str_auth_server;
  String    str_old_pw_change_time;
  int       n_apps = 0;                // # of apps user has lefton this server.
}

class UserAdmin
{
  private static final String  this_servers_name = "DatabaseServer";
  private final String   this_hosts_name;
  private final int      this_servers_role;
  private StringBuffer   master_servers_name = new StringBuffer();
  private Connection     db_connection = null;
  private final  String  error_log_filename;
  private final  String  input_filename;
  private INI            ini_files = null;

  //private static final int ......
  private final int NUM_CHANGE_TYPES = 9;
  private final int NONE =             0;
  private final int UNDO_REQUESTED  =  1;
  private final int UNDO_SUCCESSFUL =  2;
  private final int UNDO_FAILED     = -1;

  private final int ROLE_ORIGINAL  = 0;
  private final int ROLE_PRESERVED = 1;
  private final int ROLE_ADDED     = 2;
  private final int ROLE_REMOVED   = 3;
  private final int ROLE_IGNORE    = 4;

  private final String GOODUARESPONSE = "GOODDPPROCUSRADMIN";
  private final String BADUARESPONSE  = "BADDPPROCUSRADMIN";
  protected static int last_error = 0;
  private String       last_warning_msg = "";

  // The containers hold data from/for the operations below...
  private ChangeTypes changes           = new ChangeTypes();
  private Vector      remote_svr_list   = new Vector(0, 1);
  private Vector      role_changes_info = new Vector(0, 1);
  private UserData    updated_user_info = new UserData();
  private OldUserData old_user_data     = new OldUserData();
  private List        other_affected_hosts = new Vector(0, 1);

  private int     error_count = 0;
  private boolean f_users_auth = false; // true iff this is the user's auth svr.


  //////////////////////////////////////////////////////////////////////////////
  /**
   * Constructor for the User Admin class.
   * Mainly instantiates a database connection and initializes a result-tracking
   * structure.
   */
  public UserAdmin ( String  apply_filename,
                     String  log_filename )
  {
    final String  this_methods_name = "UserAdmin() constructor:";

    error_log_filename = log_filename;
    input_filename     = apply_filename;
    Statement     SQL_statement = null;
    ResultSet     db_result_set = null;
    // this_hosts_name gets permanently set later...
    String        temp_host_name = StringLib.getHostname();
    this_servers_role  = DataLib.getHostType(temp_host_name);

    try
    {

      last_error = 0;
      // Get/set some host information....
      //this_hosts_name = StringLib.getHostname();
      //this_servers_role = DataLib.getHostType(this_hosts_name);
      DataLib.getMasterServer(master_servers_name);

      // Clear out the log file....
      ini_files = new INI();

      if (ini_files == null)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not instantiate an INI object!  "
                    + ini_files.GetLastErrorMsg());
          last_error = ErrorCodes.ERROR_CANT_CREATE_INI_FILE;
          return;
        }

      // Make sure the input INI file exists..
      File input_file = new File(input_filename);
      if (!input_file.exists())
        {
          Log.error(this_servers_name, this_methods_name,
                    "Could not create the apply file: " + input_filename);
          ini_files.WriteProfileString("ALL CHANGES", "General_Error_Msg",
                                       "Failed to open " + input_filename,
                                       error_log_filename);
          last_error = ErrorCodes.ERROR_CANT_OPEN_FILE;
          return;
        }

      // Clear out the (old) log files if it exists..
      File output_file = new File(log_filename);
      if (output_file.exists())
        output_file.delete();

      // Establish the database connection..
      db_connection = DatabaseConnectionManager.getConnection();
      if (db_connection == null)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Null DB connection returned!");
          ini_files.WriteProfileString("ALL CHANGES", "General_Error_Msg",
                                      "Failed to obtain a database connection!",
                                      error_log_filename);
          last_error = ErrorCodes.ERROR_CANT_ACCESS_DB;
          return;
        }

      // Since we have had problems with case in the host names, we just get
      // whatever is in our local database..
      String  SQL_query_string;
      Object  arg_list[] = new Object[1];
      arg_list[0] = temp_host_name.toUpperCase();
      
      SQL_query_string = StringLib.SQLformat("SELECT hostname FROM hosts WHERE "
                             + "UPPER(hostname)=UPPER(" + Constants.ST + ")",
                            arg_list);
      SQL_statement = db_connection.createStatement();
      db_result_set = SQL_statement.executeQuery(SQL_query_string);
      if (db_result_set.next())
        temp_host_name = db_result_set.getString("hostname");
      
      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
    }
    finally
    {
      this_hosts_name = temp_host_name;
      try
      {
        if (db_result_set != null)
          db_result_set.close();
        if (SQL_statement != null)
          SQL_statement.close();
      }
      catch (Exception exception)
      {
        Log.excp(this_servers_name, this_methods_name,
                 exception.toString());
      }
      if (ini_files == null)
        {
          Log.error(this_servers_name, this_methods_name,
                    "No log file will be created since the INI class was "
                     + "not instantiated!");
        }
      else
        ini_files.FlushINIFile(error_log_filename);
    }
  }  // End of the constructor..


  //////////////////////////////////////////////////////////////////////////////
  /**
   * This method is what is called to extract all the requested changes in
   * the INI file.  It also closes the global database connection automatically.
   **/
  public int ExtractUserINIchanges()
  {
    final String  this_methods_name = "ExtractUserINIchanges";

    try
    {
      int n;
      int x;
      // Figure out what we're supposed to do..
      // (getRequestedChanges() != 0)
      x = getRequestedChanges();
      if (x!= 0)
        {
           return -1;
        }

      // Grab the new user data from apply file...
      // (initUserData() != 0)
      x = initUserData();
      if (x!= 0)
        {
          return -2;
        }

      // Make sure the information we have is intact..
      n = CheckUserData();
      if (n != 0)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Corrupt user data encountered");
          return -3;
        }

      ReconcileRolesForDeletes();

      // Make sure the auth server(s) is up b4 making any user edits that would
      // affect the auth server's records.
      // This includes a user addition.
      if ( (this_servers_role == Constants.NTCSS_MASTER)
           && !f_users_auth
           && (changes.add_user.local_db_part.f_requested
              || (!changes.add_user.local_db_part.f_requested
                  && (   changes.remove_user.auths_db_part.f_requested
                      || changes.change_password.auths_db_part.f_requested
                      || changes.change_full_name.auths_db_part.f_requested
                      || changes.change_auth_server.auths_db_part.f_requested
                      || changes.change_ssn.auths_db_part.f_requested
                      || changes.change_ph_num.auths_db_part.f_requested
                      || changes.change_sec_class.auths_db_part.f_requested
                      || changes.add_user.auths_db_part.f_requested
                      || changes.remove_user.auths_db_part.f_requested
                      || changes.remove_user.auths_os_part.f_requested))))
        {
          x = SvrStatLib.isServerRunning(updated_user_info.str_auth_server,
                                         Constants.DB_SVR_PORT);

          if (x != 1)
            {
              ini_files.WriteProfileString("ALL CHANGES", "General_Error_Msg",
                            "Host " + updated_user_info.str_auth_server
                            + " (Auth server) is not responding, status="
                            + x, error_log_filename);
              last_error = ErrorCodes.ERROR_SERVER_NOT_RESPONDING;
              return -4;
            }
        }

      int i_error_count = 0;
      x = StartApplyingChanges();
      if (x != 0)
        i_error_count++;

      if (this_servers_role != Constants.NTCSS_MASTER)
        {
          // This is where all the non-master servers finish up..
          if (x != 0)
            {
              ini_files.WriteProfileString(this_hosts_name, "Problems", "Y",
                                           error_log_filename);
              ini_files.WriteProfileString(this_hosts_name, "GUI_Directive",
                                           "REFRESH_INI", error_log_filename);
              return -6;
            }
          else
            {
              ini_files.WriteProfileString(this_hosts_name, "Problems", "None",
                                           error_log_filename);
            }
          return(i_error_count);
        }

      //  Only the master server should continue beyond this point..
      // See what other servers need to get these changes sent to them.
      DetermineAffectedServers();

      if (other_affected_hosts.size() > 0)
        {
          if (AddUserDataToINI() != 0)
            i_error_count++;

          // Instruct the other affected servers to apply the requested changes.
          if (ForwardChangesToOtherServers() != 0)
            i_error_count++;

          // See what errors occured on the remote applies..
          if (InterpretErrorINIfiles() != 0)
            i_error_count++;
        }
      // Depending on what the other affected servers accomplished, address
      //  any problems/undo's on this master server..

      if (FinishApplyingChanges() != 0)
        i_error_count++;

      // Report any problem to the .ini file.
      if (i_error_count != 0)
        {
          ini_files.WriteProfileString(this_hosts_name, "Problems", "Y",
                                       error_log_filename);
        }
      else
        {
          ini_files.WriteProfileString(this_hosts_name, "Problems", "None",
                                       error_log_filename);
        }

      return(i_error_count);
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
               last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
    finally
    {
      ini_files.FlushINIFile(error_log_filename);
      if (db_connection != null)
        try
        {
          db_connection.close();
        }
        catch (Exception exception)
        {}
    }
  }


  //////////////////////////////////////////////////////////////////////////////
  /**
   * This method loads the storage classes with all the needed information in
   * the INI file,  and also initializes the legacy Change_User_Data field in
   * the log file for which (only) the older UNIX versions on the master depend.
   **/
  private int getRequestedChanges()
  {
    String  temp;
    final String   this_methods_name = "getRequestedChanges";

    try
    {
      temp = ini_files.GetProfileString("CHANGES", "ChangeAppRoles", "",
                                        input_filename);
      if (temp.compareTo("true") == 0)
        {
          Log.debug(this_servers_name, this_methods_name,
                    "App role changes requested");
          changes.change_roles.local_os_part.f_requested = true;
          changes.change_roles.local_db_part.f_requested = true;
          // We don't set the os flag yet since we don't know if
          // apps will be added removed, or just changed.
          // Or if the apps are local or not.
        }

      temp = ini_files.GetProfileString("CHANGES", "ChangePassword", "",
                                        input_filename);
      //Log.debug(this_servers_name, this_methods_name,
      //            "ChangePassword = "+temp);
      if (temp.compareTo("true") == 0)
        {
          Log.debug(this_servers_name, this_methods_name,
                    "Password change requested");
          if (this_servers_role == Constants.NTCSS_MASTER)
            changes.change_password.NDS_part.f_requested = true;
          changes.change_password.local_os_part.f_requested = true;
          changes.change_password.local_db_part.f_requested = true;
          changes.change_password.auths_db_part.f_requested = true;
        }

      temp = ini_files.GetProfileString("CHANGES", "ChangeRealName", "",
                                        input_filename);
      if (temp.compareTo("true") == 0)
        {
          Log.debug(this_servers_name, this_methods_name,
                    "Full name change requested");
          changes.change_full_name.local_os_part.f_requested = true;
          changes.change_full_name.local_db_part.f_requested = true;
          changes.change_full_name.auths_db_part.f_requested = true;
        }

      temp = ini_files.GetProfileString("CHANGES", "ChangeAuthServer", "",
                                        input_filename);
      if (temp.compareTo("true") == 0)
        {
          Log.debug(this_servers_name, this_methods_name,
                    "Auth server change requested");
          if (this_servers_role == Constants.NTCSS_MASTER)
            changes.change_auth_server.NDS_part.f_requested = true;
          changes.change_auth_server.local_os_part.f_requested = true;
          changes.change_auth_server.local_db_part.f_requested = true;
          changes.change_auth_server.auths_db_part.f_requested = true;
        }

      temp = ini_files.GetProfileString("CHANGES", "ChangeSSN", "",
                                        input_filename);
      if (temp.compareTo("true") == 0)
        {
          Log.debug(this_servers_name, this_methods_name,
                    "SSN change requested");
          changes.change_ssn.local_db_part.f_requested = true;
          if (this_servers_role == Constants.NTCSS_MASTER)
            {
              changes.change_ssn.NDS_part.f_requested = true;
              if (!f_users_auth)
                changes.change_ssn.auths_db_part.f_requested = true;
            }
        }

      temp = ini_files.GetProfileString("CHANGES", "ChangePhoneNum", "",
                                        input_filename);
      if (temp.compareTo("true") == 0)
        {
          Log.debug(this_servers_name, this_methods_name,
                    "Phone number change requested");
          changes.change_ph_num.local_db_part.f_requested = true;
          changes.change_ph_num.auths_db_part.f_requested = true;
        }

      temp = ini_files.GetProfileString("CHANGES", "ChangeSecClass", "",
                                        input_filename);
      if (temp.compareTo("true") == 0)
        {
          Log.debug(this_servers_name, this_methods_name,
                    "Security classification change requested");
          changes.change_sec_class.local_db_part.f_requested = true;
          changes.change_sec_class.auths_db_part.f_requested = true;
        }

      // IMPORTANT!!! This must come after all the user updates since it
      // clears the other redundant change flags if they were set.
      temp = ini_files.GetProfileString("CHANGES", "AddUser", "",
                                        input_filename);
      if (temp.compareTo("true") == 0)
        {
          Log.debug(this_servers_name, this_methods_name,
                    "New user requested, unflagging the previous requests");
          if (this_servers_role == Constants.NTCSS_MASTER)
            changes.add_user.NDS_part.f_requested = true;
          changes.add_user.local_os_part.f_requested = true;
          changes.add_user.local_db_part.f_requested = true;
          changes.add_user.auths_db_part.f_requested = true;
          changes.add_user.auths_os_part.f_requested = true;

          // We don't care if we're not the master, either,
          //  since the remote flags will just get ignored if we're not the master.

          changes.change_password.local_os_part.f_requested = false;
          changes.change_password.local_db_part.f_requested = false;
          changes.change_password.auths_db_part.f_requested = false;
          changes.change_full_name.local_os_part.f_requested = false;
          changes.change_full_name.local_db_part.f_requested = false;
          changes.change_full_name.auths_db_part.f_requested = false;
          changes.change_ssn.local_db_part.f_requested = false;
          changes.change_ssn.auths_db_part.f_requested = false;
          changes.change_ssn.NDS_part.f_requested = false;
          changes.change_ph_num.local_db_part.f_requested = false;
          changes.change_ph_num.auths_db_part.f_requested = false;
          changes.change_sec_class.local_db_part.f_requested = false;
          changes.change_sec_class.auths_db_part.f_requested = false;
          changes.change_auth_server.local_db_part.f_requested = false;
          changes.change_auth_server.auths_db_part.f_requested = false;
        }

      // IMPORTANT!!! This must come after all the user updates since it
      // clears certain redundant change flag(s) if set.

      temp = ini_files.GetProfileString("CHANGES", "RemoveUser", "",
                                        input_filename);
      if (temp.compareTo("true") == 0)
        {
          Log.debug(this_servers_name, this_methods_name,
                    "User removal requsted");
          changes.remove_user.local_os_part.f_requested = true;

          // We temporarily skip the os account removal on the master since
          // we catch this part in FinishApplyingChanges() after a successful
          //  remote removal.  The main reason for this is to preserve this
          // user's original OS user ID, since undoing on a
          // failure would require us
          // to add a new account with a different OS ID.
          if (this_servers_role == Constants.NTCSS_MASTER)
            {
              changes.remove_user.local_os_part.f_skip = true;
              changes.remove_user.NDS_part.f_requested = true;
            }

          changes.remove_user.local_db_part.f_requested = true;
          changes.remove_user.auths_db_part.f_requested = true;
          changes.remove_user.auths_os_part.f_requested = true;

          // This may not be necessary anymore - check with Jeff J.
          if (changes.change_auth_server.local_db_part.f_requested)
            changes.change_auth_server.local_db_part.f_requested = false;

          changes.add_user.local_os_part.f_requested = false;
          changes.add_user.local_db_part.f_requested = false;
          changes.add_user.auths_db_part.f_requested = false;
          changes.change_password.local_os_part.f_requested = false;
          changes.change_password.local_db_part.f_requested = false;
          changes.change_password.auths_db_part.f_requested = false;
          changes.change_full_name.local_os_part.f_requested = false;
          changes.change_full_name.local_db_part.f_requested = false;
          changes.change_full_name.auths_db_part.f_requested = false;
          changes.change_ssn.local_db_part.f_requested = false;
          changes.change_ssn.auths_db_part.f_requested = false;
          changes.change_ph_num.local_db_part.f_requested = false;
          changes.change_ph_num.auths_db_part.f_requested = false;
          changes.change_sec_class.local_db_part.f_requested = false;
          changes.change_sec_class.auths_db_part.f_requested = false;
          changes.change_auth_server.local_db_part.f_requested = false;
          changes.change_auth_server.auths_db_part.f_requested = false;
        }
      return 0;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
  }


  //////////////////////////////////////////////////////////////////////////////
  /**
   * This method loads the storage classes with all the needed information in
   * the INI file.  This method expects the change flags to have been already
   * set.
   **/

  private int initUserData()
  {
    final String  this_methods_name = "initUserData";

    try
    {
      String  temp;
      Log.debug(this_servers_name, this_methods_name,
                "Extracting INI information about the target user");

      // Get the user's login name.  This is used for everything, and necessary
      // for the quesry in storeOldUserData().
      updated_user_info.str_login_name = ini_files.GetProfileString("USER_INFO",
                                                              "login_name", "",
                                                              input_filename);
      if (updated_user_info.str_login_name.compareTo("") == 0)
        {
          Log.error(this_servers_name, this_methods_name,
                    "No login name provided in file " + input_filename);
          ini_files.WriteProfileString("ALL CHANGES", "General_Error_Msg",
                                    "No login name provided in the apply file!",
                                    error_log_filename);
          last_error = ErrorCodes.ERROR_CANT_FIND_VALUE;
          return -1;
        }

      // If this is a new user, there won't be any old user data to collect.
      if ( changes.change_password.local_db_part.f_requested
          || changes.change_full_name.local_db_part.f_requested
          || changes.change_auth_server.local_db_part.f_requested
          || changes.change_ssn.local_db_part.f_requested
          || changes.change_ph_num.local_db_part.f_requested
          || changes.change_sec_class.local_db_part.f_requested
          || changes.remove_user.local_db_part.f_requested )
        {
          int z = storeOldUserData();
          if (z != 0)
            {
              // If new user, we wouldn't expect to have information for them.
              ini_files.WriteProfileString("ALL CHANGES", "General_Error_Msg",
                             "No information found in local database for user.",
                             error_log_filename);
              Log.error(this_servers_name, this_methods_name, "User "
                          + updated_user_info.str_login_name
                          + " is not in the local database (yet).");
              // We don't bother aborting here since it could be due to adding
              // a user to his/her first app on this server (and might not be
              // that user's auth server either)
            }
        }
      
      updated_user_info.str_full_name = ini_files.GetProfileString("USER_INFO",
                                         "real_name",
                                         old_user_data.str_full_name,
                                         input_filename);
      updated_user_info.str_ssn = ini_files.GetProfileString("USER_INFO",
                                          "ssn",
                                          old_user_data.str_ssn,
                                          input_filename);
      updated_user_info.str_phone_number = ini_files.GetProfileString("USER_INFO",
                                          "phone_number",
                                          old_user_data.str_phone_number,
                                          input_filename);
      updated_user_info.i_security_class = ini_files.GetProfileInt("USER_INFO",
                                          "security_class",
                                          old_user_data.i_security_class,
                                          input_filename);
      updated_user_info.str_auth_server = ini_files.GetProfileString("USER_INFO",
                                          "auth_server",
                                          old_user_data.str_auth_server,
                                          input_filename);
      updated_user_info.i_os_id = ini_files.GetProfileInt("USER_INFO",
                                          "unix_id",
                                          old_user_data.i_os_id,
                                          input_filename);
      updated_user_info.str_passwd_enc = ini_files.GetProfileString("USER_INFO",
                                          "password",
                                          old_user_data.str_passwd_enc,
                                          input_filename);

      // See if this host is supposed to be the user's auth server...
      if (updated_user_info.str_auth_server.compareTo(this_hosts_name) == 0)
        f_users_auth = true;

      // We don't always need the password so we check to make sure we
      //  have something before throwing an error trying to decrypt it.
      // An underscore is used a placeholder on some servers but this will cause
      //  an error if decryption is attempted so we treat it as a special case.
      if ((updated_user_info.str_passwd_enc.compareTo("") != 0)
          && (updated_user_info.str_passwd_enc.charAt(0) != '_') )
        {
          StringBuffer  decrypted_password = new StringBuffer();
          if (CryptLib.plainDecryptString( updated_user_info.str_login_name
                                            + updated_user_info.str_login_name,
                               CryptLib.trimEncryptedPasswordTerminator(
                                            updated_user_info.str_passwd_enc),
                                          decrypted_password, false, true) != 0)
            {
              Log.error(this_servers_name, this_methods_name,
                        "Error decrypting user's password");
              ini_files.WriteProfileString("ALL CHANGES", "General_Error_Msg",
                                           "Failed to decrypt user's password",
                                           error_log_filename);
              last_error = ErrorCodes.ERROR_INCORRECT_PASSWORD;
              return -3;
            }
          updated_user_info.str_passwd_plaintext = decrypted_password.toString();
        }

      return 0;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
  }

  //////////////////////////////////////////////////////////////////////////////
  /**
   * This method reads the user's data from the NTCSS database, if the user
   * already exists.  This data is used to restore old values when undos are
   * required.
   **/

  private int storeOldUserData()
  {
    final String  this_methods_name = "storeOldUserData";
    Statement  SQL_statement = null;
    ResultSet   db_result_set = null;

    try
    {
      Log.debug(this_servers_name, this_methods_name, "Gettting currently-known"
              + " information about user " + updated_user_info.str_login_name);
      Object arg_list[] = new Object[2];
      arg_list[0] = updated_user_info.str_login_name;

      String     SQL_query_string;
      SQL_query_string = StringLib.SQLformat(
                                      "SELECT * FROM users WHERE login_name="
                                       + Constants.ST, arg_list);

      SQL_statement = db_connection.createStatement();
      db_result_set = SQL_statement.executeQuery(SQL_query_string);

      // if we don't get anything back, it may be a new user to this box...
      if (!db_result_set.next())
        {
          Log.debug(this_servers_name, this_methods_name, "No pre-existing "
                                        + "record found for user "
                                        + updated_user_info.str_login_name);
            return -1;
        }

      old_user_data.str_full_name = db_result_set.getString("real_name");
      old_user_data.str_passwd_enc = db_result_set.getString("password");
      old_user_data.i_os_id = db_result_set.getInt("unix_id");
      old_user_data.str_ssn = db_result_set.getString("ss_number");
      old_user_data.str_phone_number = db_result_set.getString("phone_number");
      old_user_data.i_security_class = db_result_set.getInt("security_class");
      old_user_data.str_pw_change_time = db_result_set.getString("pw_change_time");
      old_user_data.login_lock = db_result_set.getBoolean("login_lock");
      old_user_data.str_db_passwd = db_result_set.getString("shared_db_passwd");
      old_user_data.str_auth_server = db_result_set.getString("auth_server");
      old_user_data.f_editable_user = db_result_set.getBoolean("editable_user");

      // See if this user has apps on this host.
      // Currently this value is only used to determine if a new user directory
      // should be added if new apps are added on a
      //  (non-master server or server which is not this user's auth). Usually,
      //  there is only one app per server. So on an add, this would normally
      // be zero, resulting in a new local home directory.

      //arg_list[0] = updated_user_info.login_name; Already set above..
      SQL_query_string = StringLib.SQLformat(
                                  "SELECT COUNT(*) FROM app_users WHERE "
                                   + "login_name=" + Constants.ST, arg_list);
      db_result_set = SQL_statement.executeQuery(SQL_query_string);
      if (!db_result_set.next())
        {
          Log.error(this_servers_name, this_methods_name, "Failed to get a"
                    + " count of apps this user is in!");
          ini_files.WriteProfileString("ALL CHANGES", "General_Error_Msg",
                              "Failed to get a count of apps this user is in!",
                              error_log_filename);
                              return -2;
        }

      updated_user_info.n_apps = db_result_set.getInt(1);
      return 0;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      ini_files.WriteProfileString("ALL CHANGES", "General_Error_Msg",
                              exception.toString(), error_log_filename);
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
    finally
    {
      try
      {
        if (db_result_set != null)
          db_result_set.close();
        if (SQL_statement != null)
          SQL_statement.close();
      } catch (Exception exception)
      {}
    }
  }


  //////////////////////////////////////////////////////////////////////////////
  /**
   * This method checks the data we tried to read in from the given INI file.
   **/

  private int CheckUserData()
  {
    final String  this_methods_name = "CheckUserData";

    try
    {
      Log.debug(this_servers_name, this_methods_name, "Checking integrity"
                + " of the provided data");
      int num_errors = 0;
      // Login_name should already have been checked by InitUserData()

      if (changes.add_user.NDS_part.f_requested
          || changes.add_user.local_os_part.f_requested
          || changes.add_user.local_db_part.f_requested
          || changes.change_full_name.local_os_part.f_requested
          || changes.change_full_name.local_db_part.f_requested
          || changes.change_full_name.NDS_part.f_requested
          || changes.change_auth_server.local_os_part.f_requested
          || changes.change_auth_server.local_db_part.f_requested
          || changes.change_auth_server.NDS_part.f_requested )
        {
          if (updated_user_info.str_login_name.length() == 0)
            {
              Log.error(this_servers_name, this_methods_name,
                        "Provided login_name is blank!");
              num_errors++;
            }
        }

      if ((changes.add_user.local_db_part.f_requested
            || changes.change_password.local_db_part.f_requested)
          && (updated_user_info.str_passwd_enc.length() < 2))
        {
          Log.error(this_servers_name, this_methods_name,
                        "Provided password is corrupt!");
          num_errors++;
        }

      if ((changes.add_user.local_db_part.f_requested 
            || changes.change_auth_server.local_db_part.f_requested)
          && (updated_user_info.str_auth_server.length() < 1))
        {
          Log.error(this_servers_name, this_methods_name,
                        "No auth_server information provided!");
          num_errors++;
        }

      if (num_errors > 0)
        ini_files.WriteProfileString("ALL CHANGES", "General_Error_Msg",
                             num_errors + " bad pieces of user data encoutered",
                             error_log_filename);
      return num_errors;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      ini_files.WriteProfileString("ALL CHANGES", "General_Error_Msg",
                                   exception.toString(), error_log_filename);
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
  }


  //////////////////////////////////////////////////////////////////////////////
  /** This only runs on the master.
   * When we are asked to remove a user, we have to remove all the user's app
   * roles as well.  Since the GUI does not explicitly tell us to remove any
   * roles in this case (or even specify which roles the user has),
   * we have to assemble this information on the master server and insert
   * the user's apps and changeRoles tag into
   * the INI so any other affected servers can see them too and handle it.
   * We also trip the role changes flag, primarily so that the
   * ParseAppRolesSection() method is called.  In case of a failure to
   * remove appusers on all affected remote hosts, 
   * that method will have built a list of all current roles which will
   * be used to restore the appuser's original roles on the master.
   **/

  private int ReconcileRolesForDeletes()
  {
    final String  this_methods_name = "ReconcileRolesForDeletes";

    Statement  SQL_statement = null;
    ResultSet   db_result_set = null;
    try
    {
      if ((this_servers_role != Constants.NTCSS_MASTER) ||
      (!changes.remove_user.auths_db_part.f_requested))
        return 0;

      Log.debug(this_servers_name, this_methods_name, "Checking and preparing"
                + " for implied app user removals.");

      // Make sure these flags are tripped so that the implied appuser removals
      // take place everywhere.
      changes.change_roles.local_db_part.f_requested = true;
      changes.change_roles.auths_db_part.f_requested = true;

      // Find out which apps this user has:
      Object arg_list[] = new Object[1];
      arg_list[0] = updated_user_info.str_login_name;

      String     SQL_query_string;
      SQL_query_string = StringLib.SQLformat(
                            "SELECT app_title FROM app_users WHERE login_name="
                             + Constants.ST, arg_list);

      SQL_statement = db_connection.createStatement();
      db_result_set = SQL_statement.executeQuery(SQL_query_string);

      if (!db_result_set.next())
        {
          // We'll turn these flags back off since there's no appuser records.
          changes.change_roles.local_db_part.f_requested = false;
          changes.change_roles.auths_db_part.f_requested = false;
          Log.debug(this_servers_name, this_methods_name, "This user has no"
                    + " apps - no removals are being automatically requested.");
          return 0;
        }

      ini_files.WriteProfileString("CHANGES", "ChangeAppRoles", "true",
                                   input_filename);
      do
        {
          // Add in an empty App listing which will cause it to be deleted..
          ini_files.WriteProfileString("ROLE CHANGES",
                                       db_result_set.getString(1), "",
                                       input_filename);
        }
      while(db_result_set.next());

      ini_files.FlushINIFile(input_filename);

      changes.change_roles.local_os_part.f_requested = true;
      changes.change_roles.local_db_part.f_requested = true;

      return 0;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
    finally
    {
      try
      {
        if (db_result_set != null)
          db_result_set.close();
        if (SQL_statement != null)
          SQL_statement.close();
      }
      catch (Exception exception)
      {}
    }
  }


  //////////////////////////////////////////////////////////////////////////////
  /**
   * This function takes all the user information and calls the necessary
   *  functions.  To eliminate inherent overlap of function, some changes
   *  may be skipped. The order of changes done below should not be modified!
   **/

  private int StartApplyingChanges()
  {
    final String  this_methods_name = "StartApplyingChanges";
    int   i_error_count = 0;

    try
    {
      int x;
      // ADDING A NEW USER:
      // We only do this on the master server and the auth server. This is the
      // first change type to be made since role changes may have been
      // requested.
      if (changes.add_user.local_os_part.f_requested
          || changes.add_user.local_db_part.f_requested
          || changes.add_user.NDS_part.f_requested )
        {
          x = ApplyAddUser();
          if (x != 0) // Usually means that a (non-critical) os change failed.
            {
              i_error_count++;
              if (x < 0)    // Fatal error.
                return -1; // Since we can't even add a user, exit immediately.
            }
        }                                        // End of the ADD USER section.

      // This marker is just set for the legacy UNIX user admin.. newer
      // java versions ignore this flag and do more specific checks.
      // We set this here since all the following edits set it again if failed.
      ini_files.WriteProfileString(this_hosts_name, "Change_User_Data",
                                  "OK", error_log_filename);

      // SSN CHANGE
      if (changes.change_ssn.local_db_part.f_requested
          || changes.change_ssn.NDS_part.f_requested)
        {
          if (ApplySSNchange() != 0)
            i_error_count++;
        }

      // PHONE # CHANGE
      if (changes.change_ph_num.local_db_part.f_requested
          || changes.change_ph_num.local_os_part.f_requested)
        {
          if (ApplyPhoneNumChange() != 0)
            i_error_count++;
        }

      // SECURITY CLASS CHANGE
      if (changes.change_sec_class.local_db_part.f_requested)
        {
          if (ApplySecClassChange() != 0)
            i_error_count++;
        }

      // FULL NAME CHANGE
      if (changes.change_full_name.local_db_part.f_requested
          || changes.change_full_name.local_os_part.f_requested)
        {
          if (ApplyFullNameChange() != 0)
            i_error_count++;
        }

      // PASSWORD CHANGE
      if (changes.change_password.local_db_part.f_requested
          || changes.change_password.local_os_part.f_requested)
        {
          if (ApplyPasswordChange() != 0)
            i_error_count++;
        }

      // ROLE CHANGES
      if (changes.change_roles.local_db_part.f_requested
          || changes.change_roles.local_os_part.f_requested)
        {
          if (ApplyRoleChanges() != 0)
            i_error_count++;
        }

      // REMOVE USER
      // All this user's app access roles should be listed and blank if this
      //  flag is set so that appuser and GID entries were (already) removed.
      // This code should only be run on an auth server: The master runs its
      //  equivalent portion of code in the FinishApplyingChanges() function.
      // We allow returns here since we should never do anything after this.

      if (changes.remove_user.local_db_part.f_requested
          || changes.remove_user.local_os_part.f_requested)
        {
          // If all the appuser records weren't successfully removed, then we cannot
          //  remove the user.  We don't really care about the
          //  OS group modification failures at this point.

          if (changes.change_roles.local_db_part.f_failed)
            {
              changes.remove_user.local_db_part.f_failed = true;
              changes.remove_user.local_os_part.f_failed = true;
              changes.remove_user.auths_db_part.f_skip = true;
              ini_files.WriteProfileString(this_hosts_name,
                                "Remove_Ntcss_DB_Account",
                                "Failed to remove all app roles for this user!",
                                error_log_filename);
              // Just return now since the only thing left to do are auth server
              //  changes.
              return -5;
            }
          // Setting the following flag prevents the master's os account from
          // being removed at this point - it is only actually removed after
          // a successful db operation on this user's auth server:
          if (this_servers_role == Constants.NTCSS_MASTER)
            changes.remove_user.local_os_part.f_skip = true;
          //changes.remove_user.auths_db_part.f_skip = 1;
          Log.debug(this_servers_name, this_methods_name, "Attempting to remove"
                     + " user...");
          if (ApplyUserRemoval() != 0)
            i_error_count++;
          // The only thing left to do is auth_erver changes but that flag should
          // never get tripped on a delete so we pass over it to record the error.
        }

      // CHANGE AUTH SERVERS
      if (changes.change_auth_server.local_db_part.f_requested
          || changes.change_auth_server.local_os_part.f_requested )
        {
          if (ApplyAuthServerChange() != 0)
            i_error_count++;
        }
      // The master server calls this in its FinishApplyingChanges section.
      if (f_users_auth && (this_servers_role != Constants.NTCSS_MASTER))
        {
          x = EnsureUsersAuthServer();
          if (x != 0)
            {
              ini_files.WriteProfileString(this_hosts_name,
                                           "GUI_Directive", "REFRESH_INI",
                                           error_log_filename);
              if (x < 0)
                i_error_count++;  // Only concerned about serious errors here.
            }
        }

      return i_error_count;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
  }


  //===========================================================================
  /** Adds a new user to the Ntcss environment.  Writes appropriate error
   * messages to an error log file.
   * Note that plain_password is only needed here because it is passed to os
   * crypt for adding to the /etc/passwd file line.
   * Also note that os_id gets set here and passed back up.
   */

  private int ApplyAddUser()
  {
    final String  this_methods_name = "ApplyAddUser";
    Statement  SQL_statement = null;
    ResultSet   db_result_set = null;

    try
    {
      int    i_error_count = 0;
      int    x;

      // First add this into LDAP because we can't even log in (for now)
      // until it's there.
      if ( (this_servers_role == Constants.NTCSS_MASTER)
           && (changes.add_user.NDS_part.f_requested))
        {
          Log.debug(this_servers_name, this_methods_name, "Adding user "
                    + updated_user_info.str_login_name + " to NDS...");
          if (!NDSLib.addUser(updated_user_info.str_login_name,
                              updated_user_info.str_passwd_enc,
                              updated_user_info.str_ssn,
                              updated_user_info.str_auth_server))
            {
              ini_files.WriteProfileString(this_hosts_name, "Add_User_to_NDS",
                                           "Failed to add user!",
                                           error_log_filename);
              // Since we can't add user into LDAP, we essentially abort
              changes.add_user.NDS_part.f_failed = true;
              changes.add_user.local_os_part.f_skip = true;
              changes.add_user.auths_db_part.f_skip = true;
              Log.error(this_servers_name, this_methods_name, "NDS failed!");
              return -1;  // Essentially the error count.
            }
          else
            {
              ini_files.WriteProfileString(this_hosts_name, "Add_User_to_NDS",
                                           "OK", error_log_filename);
            }
        }

      // On the master server, We create the OS account first so we can obtain
      //   the user's UID to set in the ntcss_users table.
      IntegerBuffer  uid = new IntegerBuffer();
      uid.setValue(0);

      Log.debug(this_servers_name, this_methods_name, "Creating an OS account "
                 + "for user " + updated_user_info.str_login_name);
      // Create the os-related user account stuff..
      if ((this_servers_role == Constants.NTCSS_MASTER)
          && (changes.add_user.NDS_part.f_requested))
        // Call a os script to create a new os account for a user on a master
        // server (can include /etc/passwd entry and a user directory)
        // The password passed in should just be the plain text of the password.
        x = UserLib.CreateMasterSvrAccount(updated_user_info.str_login_name,
                                      updated_user_info.str_passwd_plaintext,
                                      Constants.NTCSS_UNIX_USERS_DIR,
                                      updated_user_info.str_full_name,
                                      updated_user_info.str_auth_server,
                                      uid);
      else
        x = UserLib.CreateAuthSvrAccount(updated_user_info.str_login_name,
                                      updated_user_info.i_os_id + "",
                                      Constants.NTCSS_UNIX_USERS_DIR);

      if (x != 0)
        {
          ini_files.WriteProfileString(this_hosts_name, "Add_os_Account",
                                       "Returned error " + x,
                                       error_log_filename);
          changes.add_user.local_os_part.f_failed = true;
          changes.add_user.NDS_part.undo_status = UNDO_REQUESTED;
          changes.add_user.auths_db_part.f_skip = true;
          i_error_count++;  // Non fatal, but should be noted in the return.
        }
      else
        {
          ini_files.WriteProfileString(this_hosts_name, "Add_os_Account",
                                       "OK", error_log_filename);
        }
      // Finished creating an OS account.

      // Create a unique shared db password for this user.
      String args[] = new String[1];

      // Create a seed for the random number generator..
      args[0] = StringLib.getFullTimeStr().substring(5);
      StringBuffer db_passwd = new StringBuffer();
      StringLib.getRandomString(db_passwd, Constants.MAX_APP_PASSWD_LEN, args);
      StringLib.removePunctuation(db_passwd);

      Object arg_list[] = new Object[14];
      arg_list[0] = updated_user_info.str_login_name;
      arg_list[1] = updated_user_info.str_full_name;
      arg_list[2] = updated_user_info.str_passwd_enc;
      arg_list[3] = new Integer(updated_user_info.i_os_id);
      arg_list[4] = updated_user_info.str_ssn;
      arg_list[5] = updated_user_info.str_phone_number;
      arg_list[6] = new Integer(updated_user_info.i_security_class);
      arg_list[7] = StringLib.getTimeStr();
      arg_list[8] = new Boolean("false");
      arg_list[9] = db_passwd.toString();
      arg_list[10] = updated_user_info.str_auth_server;
      arg_list[11] = new Boolean("true");
      arg_list[12] = new Boolean("false");
      // We don't use this yet, so we just stuff the login_name in here for
      // something that's guaranteed unique..
      arg_list[13] = updated_user_info.str_login_name;

      // Prepare the SQL string..
      String  SQL_query_string =
      StringLib.SQLformat("INSERT INTO USERS(login_name,real_name,password,"
          + "unix_id,ss_number,phone_number,security_class,pw_change_time,"
          + "login_lock,shared_db_passwd,auth_server,editable_user,"
          + "ntcss_super_user,long_login_name) VALUES(" + Constants.ST + ","
          + Constants.ST + "," + Constants.ST + "," + Constants.ST + ","
          + Constants.ST + "," + Constants.ST + "," + Constants.ST + ","
          + Constants.ST + "," + Constants.ST + "," + Constants.ST + ","
          + Constants.ST + "," + Constants.ST + "," + Constants.ST + ","
          + Constants.ST + ")",
         arg_list);

      SQL_statement = db_connection.createStatement();

      last_warning_msg = "";
      int   i_temp = 0;
      try
      {
        Log.debug(this_servers_name, this_methods_name, "Adding user "
                    + updated_user_info.str_login_name + " to the database...");
        
        i_temp = SQL_statement.executeUpdate(SQL_query_string);
        if (i_temp < 0)
          {
            last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
            Log.error(this_servers_name, this_methods_name, last_warning_msg);
          }
      }
      catch(Exception exception)
      {
        i_temp = -1;   // Just set this so it will be seen below.
        last_warning_msg = exception.toString();
        Log.excp(this_servers_name, this_methods_name, last_warning_msg);
      }

      //  Have to record the problem and do a roll-back.
      if (i_temp < 1)
        {
          ini_files.WriteProfileString(this_hosts_name, "Add_Ntcss_DB_Account",
                      last_warning_msg, error_log_filename);
          changes.add_user.NDS_part.undo_status = UNDO_REQUESTED;
          changes.add_user.local_db_part.f_failed = true;
          changes.add_user.auths_db_part.f_skip = true;

          // Since the only other thing we did was add a os account..
          changes.add_user.local_os_part.undo_status = UNDO_REQUESTED;

          Log.error(this_servers_name, this_methods_name, "Failed to add user "
                    + updated_user_info.str_login_name + " to the database! "
                    + " Undoing the previous account operations...");
          UndoAddUser();
          return i_error_count++;
        }

      ini_files.WriteProfileString(this_hosts_name, "Add_Ntcss_DB_Account",
                                   "OK", error_log_filename);

      // add this new user to the appropriate BB on the master server..
      if (this_servers_role == Constants.NTCSS_MASTER)
        {
          Log.debug(this_servers_name, this_methods_name, "Adding user to the"
                     + " standard bulletin boards...");
          // Admin memebership is addressed in apply_role_changes()
          if (DataLib.setupStandardBBUserInfo(updated_user_info.str_login_name,
                                              false, db_connection))
            ini_files.WriteProfileString(this_hosts_name, "Add_To_Standard_BBs",
                                         "OK", error_log_filename);
          else
            // This error is not really critical so we don't do anything
            // but report it...
            ini_files.WriteProfileString(this_hosts_name, "Add_To_Standard_BBs",
                                    "FAILED", error_log_filename);
        }

      return i_error_count;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
    finally
    {
      try
      {
        if (SQL_statement != null)
          SQL_statement.close();
      }
      catch (Exception exception)
      {}
    }
  }


  //=============================================================================
  /**
   * Changes the user's ntcss password and related os password.
   **/

  private int  ApplyPasswordChange()
  {
    final String  this_methods_name = "ApplyPasswordChange";
    Statement  SQL_statement = null;

    try
    {
      int    i_error_count = 0;

      // First do local db change...
      if (changes.change_password.local_db_part.f_requested)
        {
          Log.debug(this_servers_name, this_methods_name,
                    "Changing the db password..");
          Object arg_list[] = new Object[3];
          arg_list[0] = updated_user_info.str_passwd_enc;
          arg_list[1] = StringLib.getTimeStr();
          arg_list[2] = updated_user_info.str_login_name;

          // Prepare the SQL string..
          String  SQL_query_string;
          SQL_query_string = StringLib.SQLformat("UPDATE USERS set password="
                    + Constants.ST + ",pw_change_time=" + Constants.ST
                    + " WHERE login_name=" + Constants.ST,
                   arg_list);

          SQL_statement = db_connection.createStatement();
          // Assume the user's record exists. If an error occurs it must have
          // something to do the the RDBMS subsytem.
          last_warning_msg =  "";
          int   i_temp = 0;
          try
          {
            i_temp = SQL_statement.executeUpdate(SQL_query_string);
            if (i_temp < 0)
              {
                last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
                Log.error(this_servers_name, this_methods_name, last_warning_msg);
              }
          }
          catch(Exception exception)
          {
            i_temp = -1;   // Just set this so it will be seen below.
            last_warning_msg = exception.toString();
            Log.excp(this_servers_name, this_methods_name, last_warning_msg);
          }
          if (i_temp < 1)
            {
              // This string is only for the legacy UNIX user admin.. newer
              // java versions ignore this flag and do more specific checks.
              ini_files.WriteProfileString(this_hosts_name, "Change_User_Data",
                                           last_warning_msg,
                                           error_log_filename);
              ini_files.WriteProfileString(this_hosts_name,
                                      "Change_DB_Password",  last_warning_msg,
                                      error_log_filename);
              // Keep this problem from propagating...
              ini_files.RemoveProfileString("CHANGES", "ChangePassword",
                                            input_filename);
              ini_files.FlushINIFile(input_filename); // Get this to disk ASAP.
              changes.change_password.local_db_part.f_failed = true;
              // Since there's no reason to change anything else, we just nip
              // this process in the bud..
              changes.change_password.auths_db_part.f_skip = true;
              changes.change_password.NDS_part.f_skip = true;
              changes.change_password.local_os_part.f_skip = true;
              i_error_count++;
            }
          else
            {
              ini_files.WriteProfileString(this_hosts_name,
                                           "Change_DB_Password",
                                           "OK", error_log_filename);
            }
          SQL_statement.close();
        }

      if (changes.change_password.local_os_part.f_requested)
        {
          Log.debug(this_servers_name, this_methods_name,
                    "Changing the OS password..");

          if (UserLib.ChangeSystemAccountPassword(
                                   updated_user_info.str_login_name,
                                   updated_user_info.str_passwd_plaintext) != 0)
            {
              last_warning_msg = "Error changing user's OS account password";
              Log.error(this_servers_name, this_methods_name, last_warning_msg);
              // This string is only for the legacy UNIX user admin.. newer
              // java versions ignore this flag and do more specific checks.
              ini_files.WriteProfileString(this_hosts_name, "Change_User_Data",
                                           last_warning_msg,
                                           error_log_filename);
              ini_files.WriteProfileString(this_hosts_name,
                                     "Change_os_Password",
                                     "Failed change user's OS account password",
                                     error_log_filename);
              // Keep this problem from propagating...
              ini_files.RemoveProfileString("CHANGES", "ChangePassword",
                                            input_filename);
              ini_files.FlushINIFile(input_filename); // Get this to disk ASAP.
              changes.change_password.local_os_part.f_failed = true;
              changes.change_password.local_db_part.undo_status
                                                               = UNDO_REQUESTED;
              changes.change_password.auths_db_part.f_skip = true;
              changes.change_password.NDS_part.f_skip = true;
              i_error_count++;
            }
          else
            {
              ini_files.WriteProfileString(this_hosts_name, "Change_os_Password",
                                           "OK", error_log_filename);
            }
        }

      if (changes.change_password.NDS_part.f_requested)
        {
          Log.debug(this_servers_name, this_methods_name,
                    "Changing the user's NDS password..");
          if (!NDSLib.modifyNtcssPassword(updated_user_info.str_login_name,
                                          updated_user_info.str_passwd_enc))
            {
              ini_files.WriteProfileString(this_hosts_name,
                                           "Change_NDS_Password",
                                           "Unknown failure",
                                           error_log_filename);
              changes.change_password.NDS_part.f_failed = true;
              i_error_count++;
            }
          else
            {
              ini_files.WriteProfileString(this_hosts_name,
                                           "Change_NDS_Password",
                                           "OK", error_log_filename);
            }
        }
      return i_error_count;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
    finally
    {
      try
      {
        if (SQL_statement != null)
          SQL_statement.close();
      }
      catch (Exception exception)
      {}
    }
  }


  //=============================================================================
  /**
   * Handles the full name update in the local users table and takes care
   * of the user's GECOS field in the os etc/passwd file.
   **/

  private int  ApplyFullNameChange()
  {
    final String  this_methods_name = "ApplyFullNameChange";
    Statement  SQL_statement = null;

    try
    {
      int    i_error_count = 0;

      // First do local db change...
      if (changes.change_full_name.local_db_part.f_requested)
        {
          Log.debug(this_servers_name, this_methods_name,
                    "Changing the the user's real_name in the database..");
          Object arg_list[] = new Object[2];
          arg_list[0] = updated_user_info.str_full_name;
          arg_list[1] = updated_user_info.str_login_name;

          // Prepare the SQL string..
          String  SQL_query_string;
          SQL_query_string = StringLib.SQLformat("UPDATE USERS set real_name="
                                          + Constants.ST + " WHERE login_name="
                                          + Constants.ST, arg_list);

          SQL_statement = db_connection.createStatement();
          last_warning_msg = "";
          int i_temp = 0;
          try
          {
            i_temp = SQL_statement.executeUpdate(SQL_query_string);
            if (i_temp < 0)
              {
                last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
                Log.error(this_servers_name, this_methods_name,
                          last_warning_msg);
              }
          }
          catch(Exception exception)
          {
            i_temp = -1;   // Just set this so it will be seen below.
            last_warning_msg = exception.toString();
            Log.excp(this_servers_name, this_methods_name, last_warning_msg);
          }
          if (i_temp < 1)
            {
              ini_files.WriteProfileString(this_hosts_name, "Change_User_Data",
                                           last_warning_msg,
                                           error_log_filename);
              ini_files.WriteProfileString(this_hosts_name,
                                           "Change_DB_Full_Name",
                                           last_warning_msg,
                                           error_log_filename);
              // Keep this problem from propagating...
              ini_files.RemoveProfileString("CHANGES", "ChangeRealName",
                                            input_filename);
              ini_files.FlushINIFile(input_filename); // Get this to disk ASAP.
              changes.change_full_name.local_db_part.f_failed = true;
              // Since there's no reason to change anything else, we just nip
              // this process in the bud..
              changes.change_full_name.auths_db_part.f_skip = true;
              changes.change_full_name.NDS_part.f_skip = true;
              changes.change_full_name.local_os_part.f_skip = true;
              i_error_count++;
            }
          else
            ini_files.WriteProfileString(this_hosts_name, "Change_DB_Full_Name",
                                         "OK", error_log_filename);
            SQL_statement.close();
        }

      if (changes.change_full_name.local_os_part.f_requested)
        {
          Log.debug(this_servers_name, this_methods_name,
                    "Changing the user's full name in the OS files...");
          if (UserLib.ChangeSystemAccountRealName(
                                          updated_user_info.str_login_name,
                                          updated_user_info.str_full_name) != 0)
            {
              Log.error(this_servers_name, this_methods_name,
                        "Error changing user's OS account full name");
              ini_files.WriteProfileString(this_hosts_name,
                                   "Change_OS_Full_Name",
                                   "Failed change user's OS account full name ",
                                   error_log_filename);
              changes.change_full_name.local_os_part.f_failed = true;
              // No need to set skip flag for other places since this isn't a
              // critical error.
              i_error_count++;
            }
          else
            ini_files.WriteProfileString(this_hosts_name, "Change_OS_Full_Name",
                                         "OK", error_log_filename);
        }
      return i_error_count;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
    finally
    {
      try
      {
        if (SQL_statement != null)
          SQL_statement.close();
      }
      catch (Exception exception)
      {}
    }
  }


  //=============================================================================
  /**
   * This function examines the role changes section in the specified INI file
   * and processes each progroup line. If the progroup tag is
   * specified but no roles, then the user's appuser entry for that app is
   * removed.  Accordingly, the user also gets added or removed from the
   * correct group for the particular OS.
   **/
  private int  ApplyRoleChanges()
  {
    final String  this_methods_name = "ApplyRoleChanges";
    Statement  SQL_statement = null;

    try
    {
      int    i_error_count = 0;
      int    i_temp;

      // In the case that we failed to add a new user previously, we shouldn't
      // even try to add any new roles...
      if ( changes.add_user.local_db_part.f_requested
           && changes.add_user.local_db_part.f_failed)
        {
          changes.change_roles.local_db_part.f_requested = false;
          changes.change_roles.NDS_part.f_requested = false;
          changes.change_roles.auths_db_part.f_requested = false;
          changes.change_roles.auths_os_part.f_requested = false;
          changes.change_roles.local_os_part.f_requested = false;
          return 0;
        }

      Log.debug(this_servers_name, this_methods_name, "Examining role changes");
      int x = ParseAppRolesSection();
      if (x != 0)
        {
          ini_files.WriteProfileString(this_hosts_name, "Change_Appuser_Roles",
                                "Failed to completely examine the Role Changes"
                                + " section!",
                                error_log_filename);
          // Only one of these really needs to be set (if at all)..
          changes.change_roles.local_db_part.f_failed = true;
          changes.change_roles.local_os_part.f_failed = true;

          // Since no roles can get applied anywhere, we skip the remote stuff too:
          changes.change_roles.auths_db_part.f_skip = true;

          // If role removals were not locally successful then we cannot delete
          //  this user.  We just set these flags regardless of whether the
          //  remove user change was specified or not.
          changes.remove_user.local_db_part.f_skip = true;
          changes.remove_user.auths_db_part.f_skip = true;
          return -1;
        }

      //  This is the "outer loop" referred to in comments below.
      //  We loop through each app, making all its role changes individually:
      // We also expect that each app change left, is one that has to be dealt
      // with (after filtering out the moot ones in GetAppRoleInfo)
      for (int i_app_index = 0; i_app_index < role_changes_info.size();
           i_app_index++)
        {
          // REMOVE USER FROM APP:        (No roles implies removal of appuser)
          if (((RoleChanges)role_changes_info.elementAt(i_app_index))
                                                                .remove_appuser
              && !((RoleChanges)role_changes_info.elementAt(i_app_index))
                                                                      .skipped)
            {
              Log.debug(this_servers_name, this_methods_name,
                            "No roles specified for user "
                            + updated_user_info.str_login_name
                            + ".  Removing this user from app "
                            +((RoleChanges)(role_changes_info
                                          .elementAt(i_app_index))).app_title);
              x = ApplyRemoveAppUser(
                        (RoleChanges)role_changes_info.elementAt(i_app_index));
              if (x != 0)
                i_error_count++;
              continue;    //    back to the outermost for() loop.
            }              // End of the REMOVE APP USER section.

          // SET UP FOR A NEW APP
          // Now we need to make sure that this user has an
          // ntcss_users entry if this host is one of the auth
          // servers but NOT this user's designated auth server (because it
          // should already have this entry and we do not want to
          // overwrite it.
          // This is added on these servers regardless of whether the entry
          // is already in the ntcss_users table or not, just to be sure
          // it is.
          if (((RoleChanges)role_changes_info.elementAt(i_app_index)).new_app)
            {
              Log.debug(this_servers_name, this_methods_name,
                            "User " + updated_user_info.str_login_name
                            + " is not currently not a member of app "
                            + ((RoleChanges)(role_changes_info
                                            .elementAt(i_app_index))).app_title
                            + " and is being added to it.");

              x = ApplyAddAppUser((RoleChanges)role_changes_info
                                                      .elementAt(i_app_index));
              if (x != 0)
                {
                  i_error_count++;
                  ((RoleChanges)role_changes_info.elementAt(i_app_index))
                                                                .skipped = true;
                  // Only master should continue past here:
                  if (this_servers_role != Constants.NTCSS_MASTER)
                    continue;

                  // Remove from the ini file to prevent further application!
                  ini_files.RemoveProfileString("ROLE CHANGES",
                                      ((RoleChanges)role_changes_info
                                          .elementAt(i_app_index)).app_title,
                                                input_filename);
                  ini_files.FlushINIFile(input_filename);
                  continue;   // Back to outermost loop for next app.
                }
            }

          // ADD/MODIFY ROLES
          // If we get to this point we assume that there are roles to be
          //  modified.

          // Set a marker so we can roll back any changes to this stage..
          String  SQL_query_string;
          SQL_statement = db_connection.createStatement();
          // Turn off auto commiting so we can control it ourself.
          db_connection.setAutoCommit(false);

          // Make shortcut to the long reference so code doesn't
          // appear confusing
          RoleChanges app_info;
          app_info = (RoleChanges) role_changes_info.elementAt(i_app_index);

          // Create this for all the SQL builds to follow..
          Object arg_list[] = new Object[3];

          for ( int i_role_index = 0;
                i_role_index < app_info.role_info.size();
                i_role_index++ )
            {
              AppRoles role_info;
              role_info = (AppRoles) app_info.role_info.elementAt(i_role_index);
              // Set up the SQL command:

              arg_list[0] = updated_user_info.str_login_name;
              arg_list[1] = app_info.app_title;
              arg_list[2] = role_info.name;

              if (role_info.action == ROLE_ADDED)
                {
                  Log.debug(this_servers_name, this_methods_name,
                            "Adding user " + updated_user_info.str_login_name
                            + " to role " + role_info.name + " for app "
                            + app_info.app_title);
                  SQL_query_string = StringLib.SQLformat(
                       "INSERT INTO user_app_roles(login_name,"
                        + "app_title,role_name)VALUES(" + Constants.ST + ","
                        + Constants.ST + "," + Constants.ST + ")",
                       arg_list);

                  // Add this user to the admin BB for this app if they
                  // are a new administrator...
                  if ( (this_servers_role == Constants.NTCSS_MASTER)
                    && (role_info.name.compareTo("App Administrator") == 0))
                    {
                      if (DataLib.subscribeToAppAdminBulletinBoard(
                                             updated_user_info.str_login_name,
                                             app_info.app_title, db_connection))
                        ini_files.WriteProfileString(this_hosts_name,
                                         "Add_to_" + app_info.app_title
                                          + "_Admin_BB", "OK",
                                          error_log_filename);
                      else
                        // This error is not really critical so we don't
                        // do anything but report it...
                        ini_files.WriteProfileString(this_hosts_name,
                                         "Add_to_" + app_info.app_title
                                          + "_Admin_BB", "FAILED",
                                          error_log_filename);
                    }
                }
              else
                if (role_info.action == ROLE_REMOVED)
                 {
                   Log.debug(this_servers_name, this_methods_name,
                             "Removing user " + updated_user_info.str_login_name
                              + " from role " + role_info.name + " in app "
                              + app_info.app_title);
                   SQL_query_string = StringLib.SQLformat(
                           "DELETE FROM user_app_roles WHERE "
                            +  "login_name=" + Constants.ST + " AND app_title="
                            + Constants.ST + " AND role_name=" + Constants.ST,
                           arg_list);

                   // Remove this user from the admin BB for this app if they
                  // are an administrator...
                  if ( (this_servers_role == Constants.NTCSS_MASTER)
                    && (role_info.name.compareTo("App Administrator") == 0))
                    {
                      if (DataLib.unsubscribeFromAppAdminBulletinBoard(
                                             updated_user_info.str_login_name,
                                             app_info.app_title, db_connection))
                        ini_files.WriteProfileString(this_hosts_name,
                                         "Remove_From_" + app_info.app_title
                                          + "_Admin_BB", "OK",
                                          error_log_filename);
                      else
                        // This error is not really critical so we don't
                        // do anything but report it...
                        ini_files.WriteProfileString(this_hosts_name,
                                         "Remove_From_" + app_info.app_title
                                          + "_Admin_BB", "FAILED",
                                          error_log_filename);
                    }
                  }
                else
                  // Must just be a no-change entry.
                  continue;

              // set up a special try/catch so exceptions don't prevent us from
              // recording which app role change blew up.
              last_warning_msg = "";
              try
              {
                i_temp = SQL_statement.executeUpdate(SQL_query_string);
                if (i_temp < 0)
                  {
                    last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
                    Log.error(this_servers_name, this_methods_name,
                              last_warning_msg);
                  }
              }
              catch(Exception exception)
              {
                last_warning_msg = exception.toString();
                i_temp = -1;   // Just set this so it will be seen below.
              }
              if (i_temp > 0)
                continue;    // ..On to the next role if this applied OK...

              // Error occured adding this role:
              changes.change_roles.local_db_part.f_failed = true;
              app_info.skipped = true;
              app_info.appuser_roles.f_failed = true;
              Log.error(this_servers_name, this_methods_name,
                    "Problems with app role " + role_info.name + " for appuser "
                     + updated_user_info.str_login_name + "\n"
                     + last_warning_msg);
              ini_files.WriteProfileString(this_hosts_name, app_info.app_title,
                                           last_warning_msg,
                                           error_log_filename);

              // Undo all the role transactions for this app
              db_connection.rollback();
              i_error_count++;

              if (this_servers_role != Constants.NTCSS_MASTER)
                break;                            // Out of this app's loop.

              ini_files.RemoveProfileString("ROLE CHANGES", app_info.app_title,
                                            input_filename);
              ini_files.FlushINIFile(input_filename); // Get this to disk ASAP.
              break;  // out this app's iteration
            }

          // Check that above loop(s) went OK before we commit all changes:
          if ( !app_info.users.f_failed
              && !app_info.appusers.f_failed
              && !app_info.appuser_roles.f_failed
              && !app_info.GID.f_failed
              && !app_info.skipped )
            {
              // Commit all the changes we just made for this app:
              db_connection.commit();
              db_connection.setAutoCommit(true);           // turn this back on.
              ini_files.WriteProfileString(this_hosts_name, app_info.app_title,
                                           "OK", error_log_filename);
              continue;                                  // On to the next app.
            }
          db_connection.setAutoCommit(true);
        }                                 // End of the "outer" (per app) loop.
      
      if (i_error_count != 0)
        {
          ini_files.WriteProfileString(this_hosts_name, "Change_Appuser_Roles",
                                       "Problems encountered!",
                                       error_log_filename);
          // If role removals were not locally successful then we cannot delete
          //  this user.  We just set these flags regardless of whether the
          //  remove user change was specified or not.
          changes.remove_user.local_db_part.f_skip = true;
          changes.remove_user.auths_db_part.f_skip = true;
        }
      else
        ini_files.WriteProfileString(this_hosts_name, "Change_Appuser_Roles",
                                     "OK", error_log_filename);

      return i_error_count;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
    finally
    {
      try
      {
        db_connection.setAutoCommit(true);  // turn this back on just in case.
        if (SQL_statement != null)
          SQL_statement.close();
      }
      catch (Exception exception)
      {
        Log.excp(this_servers_name, this_methods_name, exception.toString());
        last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
        return last_error;
      }
    }
  }


  //=============================================================================
  /**
   * Removes a user's USERS table entry and his/her OS account.
   **/

  private int  ApplyUserRemoval()
  {
    final String  this_methods_name = "ApplyUserRemoval";
    Statement  SQL_statement = null;

    try
    {
      // Tony says it is more preferential for a user to be in our db and not in
      // etc/passwd than vice versa so we make sure the OS portion is successful
      // before continuing:
      Log.debug(this_servers_name, this_methods_name, "Checking flags...");

      int n_errors = 0;

      if ( !changes.remove_user.local_os_part.f_skip
          && changes.remove_user.local_os_part.f_requested )
        // We check the skipped flag because the os part may be skipped until
        // we're sure any remote portion was successful.
        {
          Log.debug(this_servers_name, this_methods_name,
                    "Removing user " + updated_user_info.str_login_name
                    + " from the OS...");
          int x;
          if (this_servers_role == Constants.NTCSS_MASTER)
            x = UserLib.RemoveMasterSvrAccount(
                                              updated_user_info.str_login_name);
          else
            x = UserLib.RemoveAuthSvrAccount(updated_user_info.str_login_name,
                                             Constants.NTCSS_UNIX_USERS_DIR);

          if (x != 0)
            {
              ini_files.WriteProfileString(this_hosts_name, "Remove_os_Account",
                                           "Failed with error " + x,
                                           error_log_filename);
              // Take this out before it gets forwarded to other servers..
              ini_files.RemoveProfileString("CHANGES", "RemoveUser",
                                            input_filename);
              ini_files.FlushINIFile(input_filename); // Get this to disk ASAP.
              changes.remove_user.local_os_part.f_failed = true;
              changes.remove_user.local_db_part.f_failed = true;
              changes.remove_user.auths_db_part.f_skip = true;
              return -1;
            }
          else
            {
              ini_files.WriteProfileString(this_hosts_name, "Remove_os_Account",
                                           "OK", error_log_filename);
            }
        }

      // Reset this flag so this gets done in FinishAppl.. on the master.
      if (this_servers_role == Constants.NTCSS_MASTER)
        changes.remove_user.local_os_part.f_skip = false;
      
      // If role removals were not locally successful then we cannot delete
      //  this user
      if ( changes.change_roles.local_db_part.f_requested
         && changes.change_roles.local_db_part.f_failed )
        {
          // prevent any further processing of the remove user change:
          changes.remove_user.local_db_part.f_skip = true;
          changes.remove_user.auths_db_part.f_skip = true;
        }
 
      if ( !changes.remove_user.local_db_part.f_requested
          || changes.remove_user.local_db_part.f_skip )
        // We won't remove from LDAP unless it's gone from the DB
        return 0;

      Log.debug(this_servers_name, this_methods_name,
                "Removing user " + updated_user_info.str_login_name
                + " from the the local database...");
      Object arg_list[] = new Object[1];
      arg_list[0] = updated_user_info.str_login_name;

      // Prepare the SQL string..
      String  SQL_query_string;
      SQL_query_string = StringLib.SQLformat("login_name=" + Constants.ST,
                                             arg_list);

      SQL_statement = db_connection.createStatement();
      last_warning_msg = "No matching records";
      int i_temp = 0;
      try
      {
        //i_temp = SQL_statement.executeUpdate(SQL_query_string);
        // This cascade takes out the BB subsciptions as well.
        i_temp = DataLib.cascadeDelete(SQL_statement, "users",
                                       SQL_query_string);
        if (i_temp < 0)
          {
            last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
            Log.error(this_servers_name, this_methods_name, last_warning_msg);
          }
      }
      catch(Exception exception)
      {
        i_temp = -1;   // Just set this so it will be seen below.
        last_warning_msg = exception.toString();
        Log.excp(this_servers_name, this_methods_name, last_warning_msg);
      }
      if (i_temp < 1)
        {
          ini_files.WriteProfileString(this_hosts_name,
                                       "Remove_Ntcss_DB_Account", i_temp + " "
                                        + last_warning_msg, error_log_filename);
          changes.remove_user.local_db_part.f_failed = true;
          // We just leave the os account deleted for now.  The next time this
          // user tries to log onto this server, the users record will be
          // deleted since there should be no assoc. app roles.

          // We also don't want to prevent this change from reaching the remote
          // auth server so we don't set the skip flag.
          return -2;
        }
      else
        ini_files.WriteProfileString(this_hosts_name, "Remove_Ntcss_DB_Account",
                                     "OK", error_log_filename);
      SQL_statement.close();

      // Remove user from LDAP
      if ((changes.remove_user.NDS_part.f_requested)
          && (this_servers_role == Constants.NTCSS_MASTER)
          && ((!changes.remove_user.local_db_part.f_failed)
               && (!changes.remove_user.local_db_part.f_skip)))
        {
          Log.debug(this_servers_name, this_methods_name, "Removing user "
                    + updated_user_info.str_login_name + " from NDS...");

          if (!NDSLib.deleteUser(updated_user_info.str_login_name))
            {
              ini_files.WriteProfileString(this_hosts_name,
                                           "Remove_User_From_NDS", "Failed",
                                           error_log_filename);
              Log.error(this_servers_name, this_methods_name, "Error occured"
                         + " while removing user "
                         + updated_user_info.str_login_name + " from LDAP");
            }
          else
            {
              ini_files.WriteProfileString(this_hosts_name,
                                           "Remove_User_From_NDS", "OK",
                                           error_log_filename);
            }
        }

      // Remove this new user from the appropriate BB on the master server..
      if (this_servers_role == Constants.NTCSS_MASTER)
        {
          if (!changes.remove_user.local_db_part.f_failed)
            {
              // This not only clears this user from any remaining subscriptions
              // but also removes the bulletin board created with the user's
              // name.
              if (DataLib.clearStandardBBUserInfo(
                                              updated_user_info.str_login_name,
                                              false, db_connection))
                // Admin memebership is addressed in apply_role_changes()
                ini_files.WriteProfileString(this_hosts_name,
                                             "Remove_From_NTCSS_BB", "OK",
                                             error_log_filename);
              else
                // This error is not really critical so we don't do anything
                // but report it...
                ini_files.WriteProfileString(this_hosts_name,
                                             "Remove_From_NTCSS_BB", "FAILED",
                                             error_log_filename);
            }
          else
            // This error is not really critical so we don't do anything
            // but report it...
            ini_files.WriteProfileString(this_hosts_name,
                                         "Remove_From_NTCSS_BB", "FAILED",
                                         error_log_filename);
        }

      return 0;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
    finally
    {
      try
      {
        if (SQL_statement != null)
          SQL_statement.close();
      }
      catch (Exception exception)
      {}
    }
  }


  //=============================================================================
  /**
   * Takes a list of servers and sends the user changes .INI file to them
   * to be processed.  Any miscommunications or absent .INI responses are
   * recorded to be dealt with later.
   **/

  private int  ForwardChangesToOtherServers()
  {
    final String  this_methods_name = "ForwardChangesToOtherServers";

    try
    {
      int   i_error_count = 0;
      int   num_bad_responses = 0;  // Number of NACKs recieved by the DM.

      Log.debug(this_servers_name, this_methods_name,
                 "Passing the INI file to the other servers...");

      // Open the given file and get ready to read in the contents
      FileString datafile = new FileString(input_filename);

      DPResults  results;
      results = DistributionManager.sendMessage(
                             StringLib.padString("DPPROCESSUSRCHANGES",
                                                 Constants.CMD_LEN, '\0')
                              + datafile.toString(),  // reads in the INI file
                             other_affected_hosts,
                             Constants.DB_SVR_PORT, false,
                             Constants.DP_USERADMINCHANGES_TIMEOUT);

      if (results == null)
        {
          Log.error(this_servers_name, this_methods_name,
                    "No status messages/logs returned by the DM!");
          // For each host section, set the error messages since the DM failed"
          // to get any of the changes ditributed..
          for (int i = 0; i < other_affected_hosts.size(); i++)
            {
              ini_files.WriteProfileString((String)other_affected_hosts.get(i),
                                           "Problems", "Y",
                                           error_log_filename);
              ini_files.WriteProfileString((String)other_affected_hosts.get(i),
                                            "Log_File", "None",
                                           error_log_filename);
              ini_files.WriteProfileString((String)other_affected_hosts.get(i),
                                           "Host_Responding", "DM Failure",
                                           error_log_filename);
            }
          last_error = ErrorCodes.ERROR_BAD_DATA_FOUND;
          return -1;
        }

      // See what we have from each host..
      String  host_response;
      for (int i = 0; i < other_affected_hosts.size(); i++)
        {
          host_response =results.getResult((String)other_affected_hosts.get(i));

          // Find out which hosts responded.
          if (host_response == null)
            {
              Log.debug(this_servers_name, this_methods_name,
                        "No response received from server "
                         + (String)other_affected_hosts.get(i));

              ini_files.WriteProfileString((String)other_affected_hosts.get(i),
                                            "Problems", "Y",
                                            error_log_filename);
              ini_files.WriteProfileString((String)other_affected_hosts.get(i),
                                           "Log_File", "None",
                                           error_log_filename);
              ini_files.WriteProfileString((String)other_affected_hosts.get(i),
                                           "Host_Responding", "NO",
                                           error_log_filename);
                                           num_bad_responses++;
              continue;   // Check the next host's response..
            }

          ini_files.WriteProfileString((String)other_affected_hosts.get(i),
                                       "Host_Responding", "Y",
                                       error_log_filename);

          // Make sure we got some sort of logging information..
          if (host_response.length() <= Constants.CMD_LEN)
            {
              Log.debug(this_servers_name, this_methods_name,
                        "No log file received from server "
                         + (String)other_affected_hosts.get(i));
              ini_files.WriteProfileString((String)other_affected_hosts.get(i),
                                           "Log_File", "None",
                                           error_log_filename);
              num_bad_responses++;
              continue;  // On to next host..
            }

          // Since we got something beyond the header, we'll take a look at it..
          ini_files.WriteProfileString((String)other_affected_hosts.get(i),
                                       "Log_File", "Y",
                                       error_log_filename);

          // See if the logging indicates something wrong...
          if (host_response.substring(0, Constants.CMD_LEN)
                                                .compareTo(BADUARESPONSE) == 0)
            {
              Log.debug(this_servers_name, this_methods_name,
                        "server " + (String)other_affected_hosts.get(i)
                        + " had problems apply the requested changes!");

              ini_files.WriteProfileString((String)other_affected_hosts.get(i),
                                           "Problems", "Y",
                                           error_log_filename);
              num_bad_responses++;
            }

          // We don't want to blanket overwrite everything in our master log file,
          // so we load the results in to a temporary INI instance, then pick
          // out the pieces we want to write to the master's log file.
          File  temp_file;
          temp_file = File.createTempFile("temp", "temp");
          ini_files.LoadINIString(host_response.substring(Constants.CMD_LEN),
                                  temp_file.getCanonicalPath());
          // Retrieve the remote host's [host] section...
          String  temp_profile_section;
          temp_profile_section = ini_files.GetProfileSection(
                                          (String)other_affected_hosts.get(i),
                                          temp_file.getCanonicalPath());
              
          // ...append this section to the master's [host] section...
          ini_files.UpdateProfileSection((String)(other_affected_hosts.get(i)),
                                        temp_profile_section,
                                        error_log_filename);
          // Now retrieve the [ALL CHANGES] section from the remote host's
          // log file...
          temp_profile_section = ini_files.GetProfileSection(
                                          "[ALL CHANGES]",
                                          temp_file.getCanonicalPath());
          // ...append this section to the master's [host] section...
          ini_files.UpdateProfileSection((String)(other_affected_hosts.get(i)),
                                        temp_profile_section, 
                                        error_log_filename);
          temp_file.delete();  // Just in case.
        }
      return num_bad_responses;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
  }


  //=============================================================================
  /**
   * This function was only intended to be run on the master server.  It
   *  examines the error sections returned from the app/auth servers and places
   *  them in the master's error INI.  Appropriate failure flags
   *  are set in the provided structures for further processing.
   *  In the absence of a log file, we assume all the remote changes failed.
   **/

  private int  InterpretErrorINIfiles()
  {
    final String  this_methods_name = "InterpretErrorINIfiles";
    try
    {
      boolean   f_no_log_file;
      int       num_errors = 0;

      // Create a temp cariable so references aren't so long...
      String  current_affected_host;

      // Examine the results section from every remote server:
      for (int i = 0; i < other_affected_hosts.size(); i++)
        {
          current_affected_host = (String)other_affected_hosts.get(i);

          // Make sure we skip the (master) host this is running on.
          if (current_affected_host.compareTo(this_hosts_name) == 0)
            continue;

          // Absense of any remote log file should be recorded in this host's
          //  .INI by now.
          String temp_str;
          f_no_log_file = false;
          temp_str = ini_files.GetProfileString(current_affected_host,
                                                "Log_File", "NO INFO",
                                                error_log_filename);
          if (temp_str.compareTo("Y") != 0)
            {
              Log.debug(this_servers_name, this_methods_name, "No logfile for "
                          + "host " + current_affected_host);
              f_no_log_file = true;
              num_errors++;
            }

          Log.debug(this_servers_name, this_methods_name,
                        "Comparing master's requests with remote successes or "
                        + " failures on server " + current_affected_host);

          // Check remote ROLE CHANGE results:
          if (changes.change_roles.local_db_part.f_requested)
            {
              Log.debug(this_servers_name, this_methods_name, "Checking role"
                          + " change failures...");
              // Figure out the role changes that should have been
              //  done by this server.
              RoleChanges  app_info;
              for (int j = 0; j < role_changes_info.size(); j++)
                {
                  // Do this assignemnt so further accesses don't look so long..
                  app_info = (RoleChanges)role_changes_info.elementAt(j);

                  // If this role change was not forwarded to its remote host..
                  if (app_info.skipped)
                  {
                    Log.debug(this_servers_name, this_methods_name,
                              "Skipping " + app_info.app_title);
                    continue;
                  }
                  // If the app's host isn't the remote host we're currently
                  //  looking at, skip it:
                  if ( app_info.app_server_name.compareTo(current_affected_host)
                        != 0)
                  {
                    Log.debug(this_servers_name, this_methods_name,
                              "app " + app_info.app_title
                                + " does not reside on this host.");
                    continue;
                  }

                  // At this point, assume we are looking at the role
                  //   changes made on the current remote server.
                  // We need to indicate whether or not a log was present for
                  // this app's operations (for future reference)
                  app_info.no_log_file = f_no_log_file;
                  Log.debug(this_servers_name, this_methods_name,
                              "Local app found: Looking closer at "
                                + app_info.app_title);
                  // Figure out what happened on the remote server's apply,
                  // and determine if/what we need to do locally in reponse..
                  if (InterpretRoleChangeResults(app_info) != 0)
                    num_errors++;
                }
            }
        
          // Since the rest of the changes should only occur on the user's
          //  designated auth server, we skip to the next server
          if (current_affected_host.compareTo(
                                        updated_user_info.str_auth_server) != 0)
            continue;                           // inner for loop.

          // FYI: If we didn't get a log file back from the auth server,
          // we have to assume that no changes were successfully made.

          // Examine remote ADD USER change results
          if ( changes.add_user.auths_db_part.f_requested
               && !changes.add_user.auths_db_part.f_skip )
            {
              int  i_error = -1;
              if (!f_no_log_file)
                {
                  temp_str = ini_files.GetProfileString(current_affected_host,
                                                        "Add_Ntcss_DB_Account",
                                                        "NO INFO",
                                                        error_log_filename);
                  i_error = temp_str.compareTo("OK");
                }
              if (i_error != 0)
                {
                  Log.debug(this_servers_name, this_methods_name,
                            "Add_Ntcss_DB_Account result: " + temp_str);
                  changes.add_user.auths_db_part.f_failed = true;
                  // We check all these flags just in case of future changes.
                  if (changes.add_user.local_db_part.f_requested
                      && !changes.add_user.local_db_part.f_skip
                      && !changes.add_user.local_db_part.f_failed)
                    {
                      changes.add_user.local_db_part.undo_status
                                                              = UNDO_REQUESTED;
                    }
                  if (changes.add_user.local_os_part.f_requested
                      && !changes.add_user.local_os_part.f_skip
                      && !changes.add_user.local_os_part.f_failed)
                    {
                      changes.add_user.local_os_part.undo_status
                                                              = UNDO_REQUESTED;
                    }
                  num_errors++;
                }
            }                           // End of the ADD USER section.
        
          // Examine remote REMOVE USER change results
          if ( changes.remove_user.auths_db_part.f_requested
              && !changes.remove_user.auths_db_part.f_skip )
            {
              int  i_error = -1;
              if (!f_no_log_file)
                {
                  temp_str = ini_files.GetProfileString(current_affected_host,
                                                    "Remove_Ntcss_DB_Account",
                                                    "NO INFO",
                                                    error_log_filename);
                  i_error = temp_str.compareTo("OK");
                }
              if (i_error != 0)
                {
                  changes.remove_user.auths_db_part.f_failed = true;
                  if (changes.remove_user.local_db_part.f_requested
                      && !changes.remove_user.local_db_part.f_failed
                      && !changes.remove_user.local_db_part.f_skip)
                    {
                      changes.remove_user.local_db_part.undo_status
                                                              = UNDO_REQUESTED;
                    }
                  // FYI: The os portion is locally skipped until after the
                  //  change is applied remotely -so there's nothing to
                  //  undo for it.
                  num_errors++;
                }
              else
                {
                  // If role changes failed remotely, then we have to undo the
                  // local remove-user change!
                  if (changes.change_roles.auths_db_part.f_failed
                      && changes.remove_user.local_db_part.f_requested
                      && !changes.remove_user.local_db_part.f_failed
                      && !changes.remove_user.local_db_part.f_skip )
                    {
                      changes.remove_user.local_db_part.undo_status
                                                              = UNDO_REQUESTED;
                    }
                  else
                    // FinishApplyingChanges will actually remove the account
                    // after this since this operation was succesful on the
                    //  remote server and this flag is cleared.
                    changes.remove_user.local_os_part.f_skip = false;
                }
            }                  // End of remove user section.

          // Examine remote AUTH SERVER change results:
          if (changes.change_auth_server.auths_db_part.f_requested
              && !changes.change_auth_server.auths_db_part.f_skip)
            {
              int  i_error = -1;
              if (!f_no_log_file)
                {
                  temp_str = ini_files.GetProfileString(current_affected_host,
                                                        "Change_Auth_Server",
                                                        "NO INFO",
                                                        error_log_filename);
                  i_error = temp_str.compareTo("OK");
                }
              if (i_error != 0)
                {
                  Log.debug(this_servers_name, this_methods_name,
                            "Change_Auth_Server result: " + temp_str);
                  changes.change_auth_server.auths_db_part.f_failed = true;
                  if (changes.change_auth_server.local_db_part.f_requested
                      && !changes.change_auth_server.local_db_part.f_failed
                      && !changes.change_auth_server.local_db_part.f_skip )
                    {
                      changes.change_auth_server.local_db_part.undo_status
                                                               = UNDO_REQUESTED;
                    }
                  num_errors++;
                }
            }                             // End of auth server change section.
        
        
          // We exclude auth switches for many of the following checks,
          // since if an auth change failed, at least
          // the change will take place on the original auth server.
          
          // Examine remote FULL NAME change results:
          if (changes.change_full_name.auths_db_part.f_requested
              && !changes.change_full_name.auths_db_part.f_skip
              && !changes.change_auth_server.auths_db_part.f_requested)
            {
              int  i_error = -1;
              if (!f_no_log_file)
                {
                  temp_str = ini_files.GetProfileString(current_affected_host,
                                                        "Change_DB_Full_Name",
                                                        "NO INFO",
                                                        error_log_filename);
                  i_error = temp_str.compareTo("OK");
                }
              if (i_error != 0)
                {
                  Log.debug(this_servers_name, this_methods_name,
                            "Change_DB_Full_Name result: " + temp_str);
                  changes.change_full_name.auths_db_part.f_failed = true;
                  if ( changes.change_full_name.local_db_part.f_requested
                       && !changes.change_full_name.local_db_part.f_failed
                       && !changes.change_full_name.local_db_part.f_skip )
                    {
                      changes.change_full_name.local_db_part.undo_status
                                                              = UNDO_REQUESTED;
                    }

                  if ( changes.change_full_name.local_os_part.f_requested
                      && !changes.change_full_name.local_os_part.f_failed
                      && !changes.change_full_name.local_os_part.f_skip )
                    {
                      changes.change_full_name.local_os_part.undo_status
                                                              = UNDO_REQUESTED;
                     }
                  num_errors++;
                }
            }                                 // End of full name change section.
        
          // Examine remote PASSWORD change results:
          if ( changes.change_password.auths_db_part.f_requested
              && !changes.change_password.auths_db_part.f_skip )
            {
              int  i_error = -1;
              if (!f_no_log_file)
                {
                  temp_str = ini_files.GetProfileString(current_affected_host,
                                                        "Change_DB_Password",
                                                        "NO INFO",
                                                        error_log_filename);
                  i_error = temp_str.compareTo("OK");
                }
              if (i_error != 0)
                {
                  Log.debug(this_servers_name, this_methods_name,
                            "Change_DB_Password result: " + temp_str);
                  changes.change_password.auths_db_part.f_failed = true;
                  if ( changes.change_password.local_db_part.f_requested
                      && !changes.change_password.local_db_part.f_failed
                      && !changes.change_password.local_db_part.f_skip )
                    {
                      changes.change_password.local_db_part.undo_status
                                                              = UNDO_REQUESTED;
                    }
                  if ( changes.change_password.local_os_part.f_requested
                      && !changes.change_password.local_os_part.f_failed
                      && !changes.change_password.local_os_part.f_skip )
                    {
                      changes.change_password.local_os_part.undo_status
                                                              = UNDO_REQUESTED;
                    }
                  num_errors++;
                }
            }                                // End of password change section.

          // Examine remote SSN change results:
          if ( changes.change_ssn.auths_db_part.f_requested
              && !changes.change_ssn.auths_db_part.f_skip )
            {
              int  i_error = -1;
              if (!f_no_log_file)
                {
                  temp_str = ini_files.GetProfileString(current_affected_host,
                                                        "Change_SSN",
                                                        "NO INFO",
                  error_log_filename);
                  i_error = temp_str.compareTo("OK");
                }
              if (i_error != 0)
                {
                  Log.debug(this_servers_name, this_methods_name,
                            "Change_SSN result: " + temp_str);
                  changes.change_ssn.auths_db_part.f_failed = true;
                  if ( changes.change_ssn.local_db_part.f_requested
                      && !changes.change_ssn.local_db_part.f_failed
                      && !changes.change_ssn.local_db_part.f_skip )
                    {
                      changes.change_ssn.local_db_part.undo_status
                                                              = UNDO_REQUESTED;
                    }
                  num_errors++;
                }
            }                                 // End of SSN change section.
        
          // Examine remote PHONE NUMBER change results:
          if ( changes.change_ph_num.auths_db_part.f_requested
              && !changes.change_ph_num.auths_db_part.f_skip )
            {
              int  i_error = -1;
              if (!f_no_log_file)
                {
                  temp_str = ini_files.GetProfileString(current_affected_host,
                                                        "Change_Phone_Num",
                                                        "NO INFO",
                                                        error_log_filename);
                  i_error = temp_str.compareTo("OK");
                }
              if (i_error != 0)
                {
                  Log.debug(this_servers_name, this_methods_name,
                            "Change_Phone_Num result: " + temp_str);
                  changes.change_ph_num.auths_db_part.f_failed = true;
                  if ( changes.change_ph_num.local_db_part.f_requested
                      && !changes.change_ph_num.local_db_part.f_failed
                      && !changes.change_ph_num.local_db_part.f_skip )
                    {
                      changes.change_ph_num.local_db_part.undo_status
                                                              = UNDO_REQUESTED;
                    }
                  num_errors++;
                }
            }                             // End of PHONE NUMBER change section.
        
          // Examine remote SECURITY CLASS change results:
          if ( changes.change_sec_class.auths_db_part.f_requested
              && !changes.change_sec_class.auths_db_part.f_skip )
            {
              int  i_error = -1;
              if (!f_no_log_file)
                {
                  temp_str = ini_files.GetProfileString(current_affected_host,
                                                        "Change_Sec_Class",
                                                        "NO INFO",
                                                        error_log_filename);
                  i_error = temp_str.compareTo("OK");
                }
              if (i_error != 0)
                {
                  Log.debug(this_servers_name, this_methods_name,
                            "Change_Sec_Class result: " + temp_str);
                  changes.change_sec_class.auths_db_part.f_failed = true;
                  if ( changes.change_sec_class.local_db_part.f_requested
                      && !changes.change_sec_class.local_db_part.f_failed
                      && !changes.change_sec_class.local_db_part.f_skip )
                    {
                      changes.change_sec_class.local_db_part.undo_status
                                                              = UNDO_REQUESTED;
                    }
                  num_errors++;
                }
            }                           // End of SECURITY CLASS change section.
        }  // end of for (each remote server) loop.
      
      return(num_errors);
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
  }


  //=============================================================================
  /**
   *  This function builds a list of the servers that need to make changes to
   *  their databases.  This includes any auth servers for role changes, and
   *  usually the auth server for this user.  This function was ONLY intended
   *  to RUN on the MASTER SERVER.
   *  Each change type is checked to see if it was requested AND if it was
   *  subsequently skipped  - because of a local (master) failure to successfully
   *  apply the change.
   **/

  private int  DetermineAffectedServers()
  {
    final String  this_methods_name = "DetermineAffectedServers";

    try
    {
      boolean  f_add_auth_server = false;

      // Bail if this isn't the master!
      if (this_servers_role != Constants.NTCSS_MASTER)
        return 0;

      ini_files.WriteProfileString(this_hosts_name, "Type", "Master",
                                   error_log_filename);
      ini_files.WriteProfileString("HOSTS", this_hosts_name, this_hosts_name,
                                   error_log_filename);

      // Last minute logic checks:
      if ( changes.change_roles.local_db_part.f_requested
          && changes.change_roles.local_db_part.f_failed )
          {
            // If role removals were not locally successful then we shouldn't
            // delete this user anywhere else for now.  We just set these
            // flags regardless of whether the
            //  remove user change was specified or not.
            changes.remove_user.auths_db_part.f_skip = true;
          }

        // DECIDE ADD USER
        if ( changes.add_user.auths_db_part.f_requested
            && !changes.add_user.auths_db_part.f_skip )
          f_add_auth_server = true;

        // DECIDE REMOVE USER
        if ( changes.remove_user.auths_db_part.f_requested
            && !changes.remove_user.auths_db_part.f_skip )
          f_add_auth_server = true;

        // DECIDE CHANGE PASSWORD
        if ( changes.change_password.auths_db_part.f_requested
            && !changes.change_password.auths_db_part.f_skip )
          f_add_auth_server = true;

        // DECIDE CHANGE FULL NAME
        if ( changes.change_full_name.auths_db_part.f_requested
            && !changes.change_full_name.auths_db_part.f_skip )
          f_add_auth_server = true;

        // DECIDE SSN CHANGE
        if ( changes.change_ssn.auths_db_part.f_requested
            && !changes.change_ssn.auths_db_part.f_skip )
          f_add_auth_server = true;

        // DECIDE PHONE # CHANGE
        if ( changes.change_ph_num.auths_db_part.f_requested
            && !changes.change_ph_num.auths_db_part.f_skip )
          f_add_auth_server = true;

        // DECIDE SECURITY CLASS CHANGE
        if ( changes.change_sec_class.auths_db_part.f_requested
            && !changes.change_sec_class.auths_db_part.f_skip )
          f_add_auth_server = true;

        // DECIDE CHANGE AUTH SERVERS
        if ( changes.change_auth_server.auths_db_part.f_requested
            && !changes.change_auth_server.auths_db_part.f_skip )
          {
            f_add_auth_server = true;
            // Add the old auth server into the list as well.
            // Make sure the old and new auth server are different
            // to prevent dupes..
            if ( (updated_user_info.str_auth_server
                                .compareTo(old_user_data.str_auth_server) != 0)
                 &&
                  // Don't want to add in this host's name either..
                (old_user_data.str_auth_server.compareTo(this_hosts_name) != 0))
              {
                Log.debug(this_servers_name, this_methods_name,
                    "Adding old auth server,  " + old_user_data.str_auth_server
                    + ", to the server distribution list..");
                other_affected_hosts.add(old_user_data.str_auth_server);
                ini_files.WriteProfileString("HOSTS",
                                             old_user_data.str_auth_server,
                                             old_user_data.str_auth_server,
                                             error_log_filename);
              }
            // NOTE: This insertion is risky if any other future modifications
            // add a server into this list since we're not checking for
            // duplicates here like we are in the role changes section below.
          }

        // If the auth server is needed, and this master is not this user's
        //   auth server, we add it to the list.
        if (( f_add_auth_server && !f_users_auth )
            && (updated_user_info.str_auth_server
                                              .compareTo(this_hosts_name) != 0))
          {
            other_affected_hosts.add(updated_user_info.str_auth_server);
            Log.debug(this_servers_name, this_methods_name,
                      "Added new auth server, "
                      + updated_user_info.str_auth_server
                      + ", to the server distribution list..");
            if (this_servers_role == Constants.NTCSS_MASTER)
              ini_files.WriteProfileString(this_hosts_name, "Type",
                                           "Master & Auth. Server",
                                            error_log_filename);
            else
              ini_files.WriteProfileString(this_hosts_name, "Type",
                                            "Auth. Server", error_log_filename);
            ini_files.WriteProfileString("HOSTS",
                                         updated_user_info.str_auth_server,
                                         updated_user_info.str_auth_server,
                                         error_log_filename);
          }

        // THE REST OF THIS METHOD IS FOR ROLES CHANGES.

        // The changes.change_roles.auths_db_part flags are not set anywhere but
        //  here so that is why we use the .change_roles.local_db_part flag:
        if (changes.change_roles.local_db_part.f_requested)
          {
            RoleChanges  app_info;
            boolean      f_found = false;

            // Record each affected app's host (only if it's also an auth server)
            for (int i = 0; i < role_changes_info.size(); i++)
              {
                // Skip over the app server if it happens to be this master, or
                // an enslaved app server,
                // or a master-side failure caused an abort of this app change:
                app_info = (RoleChanges)(role_changes_info.elementAt(i));
                if ( (app_info.app_server_name.compareTo(this_hosts_name) == 0)
                    // This next test is obsolete now, just here for ref.
                    || (app_info.app_server_type == Constants.NTCSS_APP_SERVER)
                    || app_info.skipped
                    || other_affected_hosts.contains(app_info.app_server_name))
                  {
                    continue;
                  }
          /***
                f_found = false;
                // Make sure we don't already have this app server listed..
                for (int j = 0; j < role_changes_info.size(); j++)
                  {
                    // If we find a match already in our list, skip this hostname
                    if (app_info.app_server_name.compareTo(
                                    ((String)other_affected_hosts.get(j))) == 0)
                      {
                        f_found = true;
                        break; // From this innermost loop into the outer loop..
                      }
                  }
                if (f_found)
                  {
                    continue;
                  }
           ***/
                // Server is not already in list so we add it:
                Log.debug(this_servers_name, this_methods_name,
                          "Adding server, " + app_info.app_server_name
                           + ", to the server distribution list..");

                other_affected_hosts.add(app_info.app_server_name);

                changes.change_roles.auths_db_part.f_requested = true;
                ini_files.WriteProfileString("HOSTS", app_info.app_server_name,
                                             app_info.app_server_name,
                                             error_log_filename);
              }              // End of per-app loop.
          }            // End of role changes section.
        return 0;
      }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
  }


  //=============================================================================
  /**
   * This function calls the Undo*** functions to address any undo request flags
   * after local changes (as in ApplyUserRemoval) or remote changes.  The
   * Undo** functions usually just return if there's nothing to undo, so we
   * just call all of them.  This code was only meant for the master server to
   * execute AFTER hearing back the status of the changes from auth servers.
   **/

  private int  FinishApplyingChanges()
  {
    final String  this_methods_name = "FinishApplyingChanges";

    try
    {
      int i_error_count = 0;

      // The check for failed remote role changes and subsequent setting of
      //  the remove_user undo flag has already been made by now.
      if ( changes.change_roles.auths_db_part.f_failed
          && changes.remove_user.local_db_part.f_requested
          && !changes.remove_user.local_db_part.f_failed
          && !changes.remove_user.local_db_part.f_skip )
        {
          Log.debug(this_servers_name, this_methods_name,
                    "Failed to remove appuser from all apps, requesting an "
                     + " undo remove user account operation...");
          changes.remove_user.local_db_part.undo_status = UNDO_REQUESTED;
          // This flag usually gets unset for the OS account removal
          //  to happen next so we make sure we skip it instead.
          changes.remove_user.local_os_part.f_skip = true;
        }

      // Remove the os account on the master.
      // Set these flags since these two changes were done the first time around
      changes.remove_user.local_db_part.f_skip = true;
      changes.remove_user.NDS_part.f_skip = true;
      ApplyUserRemoval();

      // NOTE: This function must come before the UndoRoleChanges() call
      //  since this will re-add the prerequisite Users table record for it.
      if (UndoRemoveUser() != 0)
        {
          ini_files.WriteProfileString(this_hosts_name, "GUI_Directive",
                                       "REFRESH_INI", error_log_filename);
          i_error_count++;
        }

      // NOTE: This function must come after the ApplyUserRemoval() call so
      // that the prerequisite Users table record will have been added back in
      //  so we can re-add appuser records.
      if (UndoRoleChanges() != 0)
        {
          ini_files.WriteProfileString(this_hosts_name, "GUI_Directive",
                                       "REFRESH_INI", error_log_filename);
          i_error_count++;
        }

      if (UndoAuthServerChange() != 0)
        {
          ini_files.WriteProfileString(this_hosts_name, "GUI_Directive",
                                       "REFRESH_INI", error_log_filename);
          i_error_count++;
        }

      if (UndoPasswordChange() != 0)
        {
          ini_files.WriteProfileString(this_hosts_name, "GUI_Directive",
                                       "REFRESH_INI", error_log_filename);
          i_error_count++;
        }

      if (UndoFullNameChange() != 0)
        {
          ini_files.WriteProfileString(this_hosts_name, "GUI_Directive",
                                       "REFRESH_INI", error_log_filename);
          i_error_count++;
        }

      if (UndoSecClassChange() != 0)
        {
          ini_files.WriteProfileString(this_hosts_name, "GUI_Directive",
                                       "REFRESH_INI", error_log_filename);
          i_error_count++;
        }

      if (UndoPhoneNumChange() != 0)
        {
          ini_files.WriteProfileString(this_hosts_name, "GUI_Directive",
                                       "REFRESH_INI", error_log_filename);
          i_error_count++;
        }

      if (UndoSSNChange() != 0)
        {
          ini_files.WriteProfileString(this_hosts_name, "GUI_Directive",
                                       "REFRESH_INI", error_log_filename);
          i_error_count++;
        }

      // NOTE: Should be after the role changes..  since you cannot
      //       delete a users record without first
      //       removing their app_users records.
      if (UndoAddUser() != 0)
        {
          ini_files.WriteProfileString(this_hosts_name, "GUI_Directive",
                                       "REFRESH_INI", error_log_filename);
          i_error_count++;
        }

      // NOTE: EnsureUsersAuthServer() should come after any auth server
      //       changes or roles changes.  It is assumed that only
      //       the master server will run this portion of code here.
      if (EnsureUsersAuthServer() != 0)
        {
          ini_files.WriteProfileString(this_hosts_name, "GUI_Directive",
                                       "REFRESH_INI", error_log_filename);
          i_error_count++;
        }

      return(i_error_count);
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
  }


  //=============================================================================
  /**
   * This function was only meant to be run on the master server after app
   * servers failed to apply role changes to their databases.
   **/
  private int  UndoRoleChanges()
  {
    final String  this_methods_name = "UndoRoleChanges";
    Statement  SQL_statement = null;

    try
    {
      int    i_app_index;           // Loop control.
      int    i_role_index;          // Loop control.
      int    n_errors = 0;           // Running error counter.
      int    n_rc_errors = 0;       // Running error counter for roles changes.

      // Make a shortcut aliases for vector elements:
      RoleChanges  current_app;
      AppRoles     current_role;

      // For use later on..
      Object arg_list[] = new Object[3];
      String  SQL_query_string;
      SQL_statement = db_connection.createStatement();

      Log.debug(this_servers_name, this_methods_name,
                "Checking for any undo-approle requests.  We have " + role_changes_info.size()+" app(s) to check...");
      // Check each app change and see if we need to undo anything.
      for (i_app_index = 0;
           i_app_index < role_changes_info.size();
           i_app_index++)
        {
          current_app = (RoleChanges)role_changes_info.elementAt(i_app_index);
          Log.debug(this_servers_name, this_methods_name, "Looking at app "
                      + current_app.app_title);
          if (current_app.skipped)
            {
              Log.debug(this_servers_name, this_methods_name, "App "
                          + current_app.app_title + " was skipped");
              continue;
            }

          if ((current_app.appusers.undo_status != UNDO_REQUESTED)
              && (current_app.users.undo_status != UNDO_REQUESTED)
              && (current_app.appuser_roles.undo_status != UNDO_REQUESTED))
            {
              // Nothing was requested to be (un)done for this app.
              Log.debug(this_servers_name, this_methods_name, "Change for app "
                       + current_app.app_title + " went OK, no un-do's needed");
              continue;  // Check next app.
            }

          // FYI: Deleting an appuser also cascade deletes for their roles.
          if ((current_app.appusers.undo_status == UNDO_REQUESTED)
              && current_app.new_app)
            //  This is a new app so a new appuser record must have been added.
            {
              Log.debug(this_servers_name, this_methods_name,
                        "Added appuseappuser in app " + current_app.app_title);
              if (UndoAddAppUser(current_app) != 0)
                // We don't continue yet because we still might have to remove
                //  user_app_roles records if the appuser was not removed.
                n_errors++;
              else
                continue;    //    back to the outermost for loop.
            }

          // If this appuser was removed, we re-add the appuser record..
          if (!current_app.new_app           // Old app and..
              && current_app.remove_appuser
              && (current_app.appusers.undo_status == UNDO_REQUESTED))
            {
              Log.debug(this_servers_name, this_methods_name,
                          "Failed to remove user from app "
                            + current_app.app_title);
              if (UndoRemoveAppUser(current_app) != 0)
                {
                  // If there were any role changes to follow..
                  if (current_app.appuser_roles.undo_status == UNDO_REQUESTED)
                    {
                      // ...then we canot add any new roles since there is
                      // no prerequisite app_user record!
                      current_app.appuser_roles.undo_status = UNDO_FAILED;
                      ini_files.WriteProfileString(this_hosts_name,
                                   current_app.app_title + "_Role_Changes_Undo",
                                   "Failed to remove an app_user record!",
                                   error_log_filename);
                    }
                  n_errors++;
                  Log.debug(this_servers_name, this_methods_name, "Failed to"
                            + " Re-add the appuser!"
                            + " Continuing on to next app then..");
                  continue;     // Nothing more we can do for this app.
                }
            }

          Log.debug(this_servers_name, this_methods_name,"About to restore"
                  + " user's original roles for app " + current_app.app_title);
          // Add all the app roles for this app back in.
          String  last_warning_msg;
          int     i_temp;
          for ( i_role_index = 0;
                i_role_index < current_app.role_info.size();
                i_role_index++ )
            {
              current_role = (AppRoles)current_app.role_info
                                                      .elementAt(i_role_index);
              //Log.debug(this_servers_name, this_methods_name,
              //"Role #"+i_role_index+ " is " +current_role.name+" for app "
              //+current_app.app_title);
              //Log.debug(this_servers_name, this_methods_name,
              //"This role has an action value of "+current_role.action);
              // Set up the SQL command:
              arg_list[0] = updated_user_info.str_login_name;
              arg_list[1] = current_app.app_title;
              arg_list[2] = current_role.name;

              if (current_role.action == ROLE_REMOVED)
                {
                  Log.debug(this_servers_name, this_methods_name,
                     "Restoring old app role " +current_role.name + " for user "
                       + updated_user_info.str_login_name
                       + " in app " + current_app.app_title);
                  SQL_query_string = StringLib.SQLformat(
                                  "INSERT INTO user_app_roles(login_name,"
                                   + "app_title,role_name)VALUES("
                                   + Constants.ST + "," + Constants.ST + ","
                                   + Constants.ST + ")",
                                  arg_list);
                  
                  // Add this user to the admin BB for this app if they
                  // were an administrator...
                  if ( (this_servers_role == Constants.NTCSS_MASTER)
                    && (current_role.name.compareTo("App Administrator") == 0))
                    {
                      Log.debug(this_servers_name, this_methods_name,
                                "Appuser "+ updated_user_info.str_login_name
                                + " was an adminsitrator and is being added"
                                + " back into the " + current_app.app_title
                                + " Admin bulletin board.");
                      if (DataLib.subscribeToAppAdminBulletinBoard(
                                         updated_user_info.str_login_name,
                                         current_app.app_title, db_connection))
                        ini_files.WriteProfileString(this_hosts_name,
                                         "Re-Add_to_" + current_app.app_title
                                          + "_Admin_BB", "OK",
                                          error_log_filename);
                      else
                        // This error is not really critical so we don't
                        // do anything but report it...
                        ini_files.WriteProfileString(this_hosts_name,
                                         "Re-Add_to_" + current_app.app_title
                                          + "_Admin_BB", "FAILED",
                                          error_log_filename);
                    }
                }
              else
                if (current_role.action == ROLE_ADDED)
                  {
                    Log.debug(this_servers_name, this_methods_name,
                         "Removing user " + updated_user_info.str_login_name
                          + " from new role " + current_role.name + " in app "
                          + current_app.app_title);
                    SQL_query_string = StringLib.SQLformat(
                                        "DELETE FROM user_app_roles"
                                         + " WHERE login_name=" + Constants.ST
                                         + " AND app_title=" + Constants.ST
                                         + " AND role_name=" + Constants.ST,
                                         arg_list);
                  // Remove this user from the admin BB for this app if they
                  // we'rent originally an administrator...
                  if ((this_servers_role == Constants.NTCSS_MASTER)
                     && (current_role.name.compareTo("App Administrator") == 0))
                    {
                      Log.debug(this_servers_name, this_methods_name,
                                "Appuser "+ updated_user_info.str_login_name
                                + " is an adminsitrator and is being removed"
                                + " from the " + current_app.app_title
                                + " Admin bulletin board.");
                      if (DataLib.unsubscribeFromAppAdminBulletinBoard(
                                          updated_user_info.str_login_name,
                                          current_app.app_title, db_connection))
                        ini_files.WriteProfileString(this_hosts_name,
                                         "Undo_Add_" + current_app.app_title
                                          + "_Admin_BB", "OK",
                                          error_log_filename);
                      else
                        // This error is not really critical so we don't
                        // do anything but report it...
                        ini_files.WriteProfileString(this_hosts_name,
                                         "Undo_Add_" + current_app.app_title
                                          + "_Admin_BB", "FAILED",
                                          error_log_filename);
                    }
                  }
                else
                  {
                    Log.debug(this_servers_name, this_methods_name,
                              "No change for user role " + current_role.name
                              + " for app " + current_app.app_title);
                    // Must just be a no-change entry.
                    continue;
                  }
          
              last_warning_msg = "";
              i_temp = 0;
              try
              {
                i_temp = SQL_statement.executeUpdate(SQL_query_string);
                if (i_temp < 0)
                  {
                    last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
                    Log.error(this_servers_name, this_methods_name,
                              last_warning_msg);
                  }
              }
              catch(Exception exception)
              {
                i_temp = -1;   // Just set this so it will be seen below.
                last_warning_msg = exception.toString();
              }
              if (i_temp < 1)
                {
                  Log.error(this_servers_name, this_methods_name,
                         "Problems reverting back to original app_user's roles!"
                          + last_warning_msg);
                         n_errors++;
                  current_app.appuser_roles.undo_status = UNDO_FAILED;
                  n_rc_errors++;
                }
            }                                           // End of role inserts.

          // Report any problems in the last app's loop
          if (n_rc_errors != 0)
            {
              ini_files.WriteProfileString(this_hosts_name,
                                           current_app.app_title
                                            + "_Role_Changes_Undo",
                                           "Problems encountered inserting the"
                                            + " original roles!",
                                           error_log_filename);
              current_app.appuser_roles.undo_status = UNDO_FAILED;
            }
          else
            {
              ini_files.WriteProfileString(this_hosts_name,
                                           current_app.app_title
                                            + "_Role_Changes_Undo",
                                           "OK", error_log_filename);
              current_app.appuser_roles.undo_status = UNDO_SUCCESSFUL;
            }

          n_errors += n_rc_errors;
        }      // ..on to check the next app's roles.

      SQL_statement.close();
      return(n_errors);
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
    finally
    {
      try
      {
        if (SQL_statement != null)
          SQL_statement.close();
      }
      catch (Exception exception)
      {}
    }
  }


  //=============================================================================
  /**
   *  Changes the user's auth server to a new one.  Also reflects this in the
   *  NDS tables.
   **/

  private int  ApplyAuthServerChange()
  {
    final String  this_methods_name = "ApplyAuthServerChange";
    Statement  SQL_statement = null;

    try
    {
      if (!changes.change_auth_server.local_db_part.f_requested
          || changes.change_auth_server.local_db_part.f_skip )

        // We decided to just go ahead and allow the change on any server this
        // happens across so at least any old auth server doesn't think it's
        // supposed to authenticate this user.
        //(changes.change_auth_server.local_db_part.f_skip == 1) ||
        //((p_local_host->f_users_auth != 1) &&
        //(p_local_host->i_this_hosts_type != NTCSS_MASTER)) )
        return 0;

      // We assume a user's record already exists on this server
      // Everything in the users record gets replaced just in case anything
      // in it happens to be out of date for this user.
      Object arg_list[] = new Object[13];
      arg_list[0] = updated_user_info.str_auth_server;
      arg_list[1] = updated_user_info.str_full_name;
      arg_list[2] = updated_user_info.str_passwd_enc;
      arg_list[3] = new Integer(updated_user_info.i_os_id);
      arg_list[4] = updated_user_info.str_ssn;
      arg_list[5] = updated_user_info.str_phone_number;
      arg_list[6] = new Integer(updated_user_info.i_security_class);
      arg_list[7] = old_user_data.str_pw_change_time;
      arg_list[8] = new Boolean(old_user_data.login_lock);
      arg_list[9] = old_user_data.str_db_passwd;
      arg_list[10] = new Boolean (old_user_data.f_editable_user);
      arg_list[11] = new Boolean(old_user_data.ntcss_super_user);
      arg_list[12] = updated_user_info.str_login_name;

      
      Log.debug(this_servers_name, this_methods_name, "Changing user "
                + updated_user_info.str_login_name + "'s auth server to "
                + updated_user_info.str_auth_server + " in the database...");
      // Prepare the SQL string..
      String  SQL_query_string;
      SQL_query_string = StringLib.SQLformat(
               "UPDATE users SET auth_server=" + Constants.ST
                + ",real_name=" + Constants.ST + ",password=" + Constants.ST
                + ",unix_id=" + Constants.ST + ",ss_number=" + Constants.ST
                + ",phone_number=" + Constants.ST + ",security_class="
                + Constants.ST + ",pw_change_time=" + Constants.ST
                + ",login_lock=" + Constants.ST + ",shared_db_passwd="
                + Constants.ST + ",editable_user=" + Constants.ST
                + ",ntcss_super_user=" + Constants.ST + " WHERE login_name="
                + Constants.ST,
               arg_list);

      SQL_statement = db_connection.createStatement();
      last_warning_msg = "";
      int  i_temp = 0;

      try
      {
        i_temp = SQL_statement.executeUpdate(SQL_query_string);
        if (i_temp < 0)
          {
            last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
            Log.error(this_servers_name, this_methods_name, last_warning_msg);
          }
      }
      catch(Exception exception)
      {
        i_temp = -1;   // Just set this so it will be seen below.
        last_warning_msg = exception.toString();
        Log.excp(this_servers_name, this_methods_name, last_warning_msg);
      }
      if (i_temp < 1)
        {
          // Only For the legacy UNIX masters...
          ini_files.WriteProfileString(this_hosts_name, "Change_User_Data",
                                       last_warning_msg,
                                       error_log_filename);
          ini_files.WriteProfileString(this_hosts_name, "Change_Auth_Server",
                                       last_warning_msg, error_log_filename);
          // Keep this problem from propagating...
          ini_files.RemoveProfileString("CHANGES", "ChangeAuthServer",
                                        input_filename);
          ini_files.FlushINIFile(input_filename); // Get this to disk ASAP.
          changes.change_auth_server.local_db_part.f_failed = true;
          changes.change_auth_server.auths_db_part.f_skip = true;
          SQL_statement.close();
          return -1;
        }

      ini_files.WriteProfileString(this_hosts_name, "Change_Auth_Server", "OK",
                                   error_log_filename);

      // Now handle the NDS part...
      if (!changes.change_auth_server.NDS_part.f_skip
          && (this_servers_role == Constants.NTCSS_MASTER))
        {
          Log.debug(this_servers_name, this_methods_name, "Changing user "
                + updated_user_info.str_login_name + "'s auth server to "
                + updated_user_info.str_auth_server + " in NDS...");

          if (!NDSLib.modifyAuthServer(updated_user_info.str_login_name,
                                       updated_user_info.str_auth_server))
            {
              changes.change_auth_server.NDS_part.f_failed = true;
              ini_files.WriteProfileString(this_hosts_name,
                                           "Change_NDS_Auth_Server", "Failed",
                                           error_log_filename);
            }
          else
            ini_files.WriteProfileString(this_hosts_name,
                                         "Change_NDS_Auth_Server",
                                         "OK", error_log_filename);
        }

      SQL_statement.close();
      return 0;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
    finally
    {
      try
      {
        if (SQL_statement != null)
          SQL_statement.close();
      } catch (Exception exception)
      {}
    }
  }


  //=============================================================================
  /**
   * This function parses out the information in the role changes section
   * of the .ini file into a more accessible structure if the app resides
   * locally.
   **/

  private int  ParseAppRolesSection()
  {
    final String  this_methods_name = "ParseAppRolesSection";
    Statement  SQL_statement = null;
    ResultSet  db_result_set = null;

    try
    {
      int     i_error_count = 0;
      String  profile_section;

      Log.debug(this_servers_name, this_methods_name, "Parsing role changes");
      profile_section = ini_files.GetProfileSection("ROLE CHANGES",
                                                    input_filename);

      // Make sure there are actually some roles to process...
      if (profile_section.length() < 1)
        {
          Log.error(this_servers_name, this_methods_name,
                    "[ROLE CHANGES] section is empty!");
          ini_files.WriteProfileString("ALL CHANGES", "General_Error_Msg",
                                   "Error reading the [ROLE CHANGES] section",
                                   error_log_filename);
          changes.change_roles.local_db_part.f_failed = true;
          changes.change_roles.auths_db_part.f_skip = true;
          changes.change_roles.auths_os_part.f_skip = true;
          changes.change_roles.local_os_part.f_skip = true;
          return -1;
        }

      String            profile_string;
      StringTokenizer   profile_tokens;
      StringTokenizer   app_tokens = new StringTokenizer(profile_section,
                                                        INI.STRINGS_DELIMITER);

      // Get all the app names listed in the INI file and create an object
      // for each..
      int app_ndx = 0;
      RoleChanges   app_change_info;  // Just for clarity...
      while (app_tokens.hasMoreTokens())
        {
          profile_string = app_tokens.nextToken();
          Log.debug(this_servers_name, this_methods_name,
                    "App roles specification =" + profile_string);
          // Get the app name out of the string...
          profile_tokens = new StringTokenizer(profile_string, "=");

          // Add new role change object to the vector list and record app name..
          role_changes_info.add(new RoleChanges());

          // We'll just assign the vector element a name to keep the following
          // code from looking messy..
          app_change_info = (RoleChanges)(role_changes_info.elementAt(app_ndx));

          app_change_info.app_title = profile_tokens.nextToken();

          // Init these for the Apply functions.
          app_change_info.appusers.f_failed = false;
          app_change_info.appusers.undo_status = NONE;
          app_change_info.users.f_failed = false;
          app_change_info.users.undo_status = NONE;

          app_change_info.GID.f_failed = false;
          app_change_info.GID.undo_status = NONE;
          app_change_info.appuser_roles.f_failed = false;
          app_change_info.appuser_roles.undo_status = NONE;
          app_change_info.skipped = false;
          app_change_info.remove_appuser = false;

          // Init this flag because we set it just below.
          app_change_info.remove_appuser = false;
          app_ndx++;
        }

      // Get all the apps' specific information so we can figure out which ones
      // need to be applied on this host..
      Object     arg_list[] = new Object[1];
      String     SQL_query_string;

      SQL_statement = db_connection.createStatement();
      RoleChanges  current_app;     // just a temporary "pointer"
      for (int i = 0; i < role_changes_info.size(); i++)
        {
          current_app = (RoleChanges)(role_changes_info.elementAt(i));
          arg_list[0] = current_app.app_title;

          // Get all the app-specific info for each app..
          SQL_query_string = StringLib.SQLformat("SELECT hostname, unix_group "
                                  + "FROM apps WHERE app_title=" + Constants.ST,
                                 arg_list);

          last_warning_msg = "No matching record(s) found!";
          db_result_set = SQL_statement.executeQuery(SQL_query_string);
          if (!db_result_set.next())
            {
              last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
              ini_files.WriteProfileString(this_hosts_name,
                                          current_app.app_title,
                                          last_warning_msg, error_log_filename);
              Log.error(this_servers_name, this_methods_name,
                        "Failed to get host and group for app "
                          + current_app.app_title);
              current_app.skipped = true;
              i_error_count++;
              if (this_servers_role != Constants.NTCSS_MASTER)
                continue;  // Go get next app's info

              // On a master server, we get rid of this problem before it
              // propagates to other possible auth servers..
              ini_files.RemoveProfileString("ROLE CHANGES",
                                            current_app.app_title,
                                            input_filename);
              ini_files.FlushINIFile(input_filename);
                continue;
            }  // End fo the SQL error handling stuff
        
          current_app.app_server_name = db_result_set.getString("hostname");

          // If this app doesn't reside on this server, then we skip it unless
          // this is the master server
          if ((this_servers_role != Constants.NTCSS_MASTER)
              && (current_app.app_server_name.compareToIgnoreCase(this_hosts_name)
                                                        != 0))
            {
              current_app.skipped = true;
            }
          else
            // Store the group name.
            current_app.group_name = db_result_set.getString("unix_group");
          Log.debug(this_servers_name, this_methods_name, "DWB App " + current_app.app_title + " is on this host");
        }

      // We'll go through and remove all the skipped apps from our app vector
      // list.  This shouldn't be necessary, but just keeps things cleaner.
      for (int i = 0; i < role_changes_info.size(); i++)
        {
          if (((RoleChanges)(role_changes_info.elementAt(i))).skipped)
            {
              Log.debug(this_servers_name, this_methods_name, "App "
                     + ((RoleChanges)(role_changes_info.elementAt(i))).app_title
                     + " was skipped and is being removed from the list.");
              role_changes_info.removeElementAt(i);
              // Reset the counter back one to check the element that just
              // just moved into the current vacancy.
              i--;
            }
        }

      // Allocate vector elements for and get all possible appuser roles for
      // each app (on this host) that's specified in the INI file..
      for (int i = 0; i < role_changes_info.size(); i++)
        {
          current_app = (RoleChanges)(role_changes_info.elementAt(i));
          // Get all the possbile roles for this app from the db...
          if (GetAndStoreAppRoles(current_app) != 0)
            {
              Log.debug(this_servers_name, this_methods_name, "App "
                        + current_app.app_title
                        + " was skipped.. couldn't get app's roles!");
              current_app.skipped = true;
              i_error_count++;
              ini_files.RemoveProfileString("ROLE CHANGES",
                                            current_app.app_title,
                                            input_filename);
              ini_files.FlushINIFile(input_filename);
              continue;
            }
          else
            current_app.skipped = false;

          // Get all this appuser's current roles
          if (DetermineOldAppuserRoles(current_app) != 0)
            {
              Log.debug(this_servers_name, this_methods_name, "App "
                     + current_app.app_title
                     + " was skipped since we couldn't get this user's roles!");
              current_app.skipped = true;
              i_error_count++;
              ini_files.RemoveProfileString("ROLE CHANGES",
                                            current_app.app_title,
                                            input_filename);
              ini_files.FlushINIFile(input_filename);
              continue;
            }
          else
            current_app.skipped = false;
        }
      
      // Now get the updated role list (from INI) for this user and reconcile
      // them with previous/non roles..
      String  temp;
      AppRoles  current_role;  // Just a temprorary reference for cleanliness
      for (int app_index = 0; app_index < role_changes_info.size(); app_index++)
        {
          current_app = (RoleChanges)(role_changes_info.elementAt(app_index));
          // This test shouldn't be necessary but is here for completeness..
          if (current_app.skipped)
            {
              Log.debug(this_servers_name, this_methods_name, "App "
                        + current_app.app_title + " was skipped. ");
              continue;
            }
          temp = ini_files.GetProfileString("ROLE CHANGES",
                                            current_app.app_title,
                                            "", input_filename);
          temp.trim();

          // Handle the case of an app with no roles (appuser removal implied)..
          if (temp.length() < 1)
            {
              Log.debug(this_servers_name, this_methods_name, "User "
                          + updated_user_info.str_login_name
                          + " will be removed from app "
                          + current_app.app_title);
              current_app.remove_appuser = true;
              // OS group entry must be removed.
              changes.change_roles.local_os_part.f_requested = true;
              
              //continue;  // On to the next app.
              // Removed the above continue so that the original/non roles
              // will get marked for undo oprtations in case
              // of a remote failure.
            }

          // Parse out this app's role string into tokens
          StringTokenizer  role_tokens = new StringTokenizer(temp, ",");

          // Get each specified role and reconcile it with the complete list..
          while (role_tokens.hasMoreTokens())
            {
              temp = role_tokens.nextToken();

              // Now reconcile this role with the user's original & available
              // roles...
              for (int i = 0; i < current_app.role_info.size(); i++)
                {
                  current_role = (AppRoles)current_app.role_info.elementAt(i);

                  // find the requested role in the complete list..
                  if (temp.compareTo(current_role.name) == 0)
                    {
                      // Figure out which action to take on this role..
                      switch (current_role.action)
                      { 
                        case ROLE_IGNORE:       // Default initialization value.
                          // This is a new role for this appuser..
                          {
                            Log.debug(this_servers_name, this_methods_name,
                                      "User " + updated_user_info.str_login_name
                                      + " is now scheduled to get role " + temp
                                      + " for app " + current_app.app_title);
                            current_role.action = ROLE_ADDED;
                            break;  // from switch
                          }
                        case ROLE_ORIGINAL:  // The appuser still has this role
                          {
                            Log.debug(this_servers_name, this_methods_name,
                                      "User already had role "
                                      + current_role.name + " for app "
                                      + current_app.app_title
                                      +", so no changes are required.");
                            current_role.action = ROLE_PRESERVED;
                            break;  // from switch
                          }
                        default:
                          break;
                      }
                      break;  // out of inner search loop for current token
                    }
                }
            } // Finished parsing out the individual roles..

          // Now go through and check all the roles that the appuser had, but
          // were not specified in the INI file, implying that these appuser
          // roles should be deleted.
          for (int i = 0; i < current_app.role_info.size(); i++)
            {
              current_role = (AppRoles)current_app.role_info.elementAt(i);
              if (current_role.action == ROLE_ORIGINAL)
                {
                  current_role.action = ROLE_REMOVED;
                  Log.debug(this_servers_name, this_methods_name, "User "
                              + updated_user_info.str_login_name
                              + " is now scheduled to be removed from role "
                              + current_role.name + " in app "
                              + current_app.app_title);
                }
              // Also check for adding a new user directory, OS group for app..
              if ((this_servers_role == Constants.NTCSS_MASTER)
                  && (current_app.remove_appuser || current_app.new_app))
                changes.change_roles.local_os_part.f_requested = true;
            }
        }  // Finished with this app..
      SQL_statement.close();
      return i_error_count;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
    finally
    {
      try
      {
        if (SQL_statement != null)
          SQL_statement.close();
        if (db_result_set != null)
          db_result_set.close();
      } catch (Exception exception)
      {}
    }
  }


  //=============================================================================
  /**  This function was primarily designed to handle adding a new app_user to
   *  a server.  In some cases, a USERS table record has to be added so that
   *  the associated (foreign key = login_name) app_users record can be added.
   **/

  private int  ApplyAddAppUser (RoleChanges appuser_info)

  {
    final String  this_methods_name = "ApplyAddAppUser";
    Statement  SQL_statement = null;
    ResultSet  db_result_set = null;

    try
    {
      Log.debug(this_servers_name, this_methods_name, "Adding app user "
                + updated_user_info.str_login_name);

      // First check if this appuser has a prerequisite USERS record..
      int   i_temp;
      Object arg_list[] = new Object[14];
      arg_list[0] = updated_user_info.str_login_name;

      // Prepare the SQL string..
      String  SQL_query_string = StringLib.SQLformat(
                                     "Select COUNT(login_name) FROM users "
                                      + "WHERE login_name=" + Constants.ST,
                                     arg_list);
      Log.debug(this_servers_name, this_methods_name, "Determining if user "
                + updated_user_info.str_login_name + " already exists...");

      SQL_statement = db_connection.createStatement();
      last_warning_msg = "Unknown error";

      db_result_set = SQL_statement.executeQuery(SQL_query_string);
      if (!db_result_set.next())
        {
          appuser_info.users.f_failed = true;
          appuser_info.appusers.f_failed = true;
          appuser_info.skipped = true;
          changes.change_roles.local_db_part.f_failed = true;
          Log.error(this_servers_name, this_methods_name,
                    "Error decrypting user's password");
          last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
          ini_files.WriteProfileString(this_hosts_name, appuser_info.app_title,
                        last_warning_msg, error_log_filename);
          return -1;
        }

      // We'll use this to store a new random password for the users' table app
      // password, and also the appusers' table password.
      StringBuffer strb_random = new StringBuffer();
      // Begin creating the shared DB password, by creating a seed for
       //  the random number generator..
       String str_list[] =
         {
           StringLib.getFullTimeStr().substring(5),
           updated_user_info.str_login_name,
           updated_user_info.str_full_name
         };
        StringLib.getRandomString(strb_random, Constants.MAX_APP_PASSWD_LEN,
                                  str_list);
        StringLib.removePunctuation(strb_random);

      if (db_result_set.getInt(1) == 0) // User record not found so add one..
        {
          Log.debug(this_servers_name, this_methods_name, "User "
                + updated_user_info.str_login_name + " does not yet exist, so"
                + " we are adding him/her to the local database...");

          arg_list[0] = updated_user_info.str_login_name;
          arg_list[1] = updated_user_info.str_full_name;
          arg_list[2] = "_";             // password
          arg_list[3] = new Integer(0);  // uid
          arg_list[4] = updated_user_info.str_ssn;
          arg_list[5] = updated_user_info.str_phone_number;
          arg_list[6] = new Integer(updated_user_info.i_security_class);
          arg_list[7] = StringLib.getTimeStr(); // pw_change_time
          arg_list[8] = new Boolean(false);     // login_lock
          arg_list[9] = strb_random.toString(); // db_password
          // Normally we would use a "_" as a placeholder for the auth_server
          //  entry (if this wasn't the user's auth server or master server)
          //  but that field is foreign keyed.
          arg_list[10] = updated_user_info.str_auth_server;
          arg_list[11] = new Boolean(true);     // editable user
          arg_list[12] = new Boolean(false);    // super user
          // We don't use the long_login_name just yet so we just put the unique
          // login_name in instead since it fits the unique constraint req.
          arg_list[13] = updated_user_info.str_login_name;
          
          SQL_query_string = StringLib.SQLformat("INSERT INTO USERS(login_name,"
            + "real_name,password,unix_id,ss_number,phone_number,"
            + "security_class,pw_change_time,login_lock,shared_db_passwd,"
            + "auth_server,editable_user,ntcss_super_user,long_login_name) "
            + "VALUES("
            + Constants.ST + "," + Constants.ST + "," + Constants.ST + ","
            + Constants.ST + "," + Constants.ST + "," + Constants.ST + ","
            + Constants.ST + "," + Constants.ST + "," + Constants.ST + ","
            + Constants.ST + "," + Constants.ST + "," + Constants.ST + ","
            + Constants.ST + "," + Constants.ST + ")",  arg_list);

          Log.debug(this_servers_name, this_methods_name, "Adding user "
                + updated_user_info.str_login_name + " to the users table "
                + " so we can subsequently add them to the appusers table...");
          last_warning_msg ="";
          try
            {
              i_temp = SQL_statement.executeUpdate(SQL_query_string);
              if (i_temp < 0)
                {
                  last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
                  Log.error(this_servers_name, this_methods_name,
                            last_warning_msg);
                }
              if (i_temp > 0)
                {
                  Log.debug(this_servers_name, this_methods_name, "User "
                            + updated_user_info.str_login_name
                            + " was sucessfully added to the database.");
                }
            }
          catch(Exception exception)
            {
              i_temp = -1;   // Just set this so it will be seen below.
              last_warning_msg = exception.toString();
              Log.error(this_servers_name, this_methods_name, SQL_query_string);
            }
          if (i_temp < 1)
            {
              changes.change_roles.local_db_part.f_failed = true;
              appuser_info.users.f_failed = true;
              appuser_info.skipped = true;
              Log.error(this_servers_name, this_methods_name,
                        "Failed to add a new users record for subsequent role"
                        + "changes!" + last_warning_msg);
              ini_files.WriteProfileString(this_hosts_name,
                                           appuser_info.app_title,
                                           last_warning_msg,
                                           error_log_filename);
              SQL_statement.close();
              return -2;
            }

          // Since this sort of change (requiring a anew user record) should
          // only occur on an auth server, we should create an OS account as
          // well for UNIX hosts so that the user can run jobs there.
          int  x;
          x = UserLib.CreateAuthSvrAccount(updated_user_info.str_login_name,
                                           updated_user_info.i_os_id + "",
                                           Constants.NTCSS_UNIX_USERS_DIR);
          if (x != 0)
            {
              ini_files.WriteProfileString(this_hosts_name, "Add_os_Account",
                                           "Returned error " + x,
                                           error_log_filename);
              changes.add_user.local_os_part.f_failed = true;
              changes.add_user.NDS_part.undo_status = UNDO_REQUESTED;
              changes.add_user.auths_db_part.f_skip = true;
            }
          else
            {
              ini_files.WriteProfileString(this_hosts_name, "Add_os_Account",
                                           "OK", error_log_filename);
            }
        }

      // Add in the appusers record:
      arg_list[0] = appuser_info.app_title;
      arg_list[1] = updated_user_info.str_login_name;
      arg_list[2] = null;             // custom app data
      arg_list[3] = strb_random.toString(); // app_passwd
      arg_list[4] = new Boolean(false);     // login_lock
      arg_list[5] = new Boolean(false);     // batch_user

      SQL_query_string = StringLib.SQLformat("INSERT INTO APP_USERS(app_title,"
                + "login_name,custom_app_data,app_passwd,"
                + "registered_user,batch_user) VALUES("
                + Constants.ST + "," + Constants.ST + "," + Constants.ST + ","
                + Constants.ST + "," + Constants.ST + "," + Constants.ST + ")",
               arg_list);

      Log.debug(this_servers_name, this_methods_name, "Adding user "
                + updated_user_info.str_login_name
                + " to the app_users table...");
      last_warning_msg = "Unknown error";
      try
      {
        i_temp = SQL_statement.executeUpdate(SQL_query_string);
        if (i_temp < 0)
          {
            last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
            Log.error(this_servers_name, this_methods_name, last_warning_msg);
          }
      }
      catch(Exception exception)
      {
        i_temp = -1;   // Just set this so it will be seen below.
        last_warning_msg = exception.toString();
        Log.excp(this_servers_name, this_methods_name, "Caught an exception "
                  + " trying to add app user "
                  + updated_user_info.str_login_name + " to the database!");
        Log.error(this_servers_name, this_methods_name, SQL_query_string);
      }
      if (i_temp < 1)
        {
          changes.change_roles.local_db_part.f_failed = true;
          appuser_info.appusers.f_failed = true;
          appuser_info.skipped = true;
          Log.error(this_servers_name, this_methods_name,
                    "Failed to add a new appuser for role changes!\n"
                    + last_warning_msg);
          ini_files.WriteProfileString(this_hosts_name, appuser_info.app_title,
                                       last_warning_msg, error_log_filename);
          return -3;
        }

      // A user directory should already exist if this was the master or this
      // user's auth server.  But on other servers, it may not since we never
      // really add a user account through any other means.
      // But if not, we create one so root can "su -" as this user on Solaris..
      if ((updated_user_info.n_apps < 1)
          && !f_users_auth
          && (this_servers_role != Constants.NTCSS_MASTER))
        {
          Log.debug(this_servers_name, this_methods_name,
                    "Creating an Auth Server OS account for "
                    + updated_user_info.str_login_name
                    + " so user can run PDJs on this host");

          if (UserLib.CreateAuthSvrAccount(updated_user_info.str_login_name,
                                           updated_user_info.i_os_id + "",
                                        // This may be changed for NT servers...
                                           Constants.NTCSS_UNIX_USERS_DIR) == 0)
            // We make this conditional since if the above doesn't work,
            // any more app adds will give us additional tries for
            // success at the following.
            updated_user_info.n_apps++;
          else
            {
              // Non-fatal but a hassle nonetheless..
              ini_files.WriteProfileString(this_hosts_name, "General_Error_Msg",
                                       "Problems creating user's local "
                                        + "directory during an appuser "
                                        + "addition.  User directory must now "
                                        + "be created manually!",
              error_log_filename);
            }
        }
      SQL_statement.close();
      if (this_servers_role != Constants.NTCSS_MASTER)
        return 0;

      // We make OS chages after all db transactions since it would be riskier
      //  to undo it if any db transactions failed afterwards.  The caller should
      //  handle any roll-backs.
      Log.debug(this_servers_name, this_methods_name,
                "Adding user " + updated_user_info.str_login_name
                 + " to OS group " + appuser_info.group_name);

      if (UserLib.AddUserToOSGroup(updated_user_info.str_login_name,
                                   appuser_info.group_name) != 0)
        {
          changes.change_roles.local_os_part.f_failed = true;
          appuser_info.GID.f_failed = true;
          ini_files.WriteProfileString(this_hosts_name, appuser_info.app_title,
                                       "Unable to Add user to OS group!",
                                       error_log_filename);
          return -4;
        }

      if (NDSLib.addUserToGroup(updated_user_info.str_login_name,
                                appuser_info.group_name))
        ini_files.WriteProfileString(this_hosts_name,
                                     appuser_info.app_title + "_Add_To_NDS",
                                     "OK", error_log_filename);
      else
        ini_files.WriteProfileString(this_hosts_name,
                                     appuser_info.app_title + "_Add_To_NDS",
                                     "Failed", error_log_filename);

      // Add this user to the app's general message bulletin board, unless
      // the app happens to be NTCSS (since that is handled only via user
      // additions/removal.
      if ( (this_servers_role == Constants.NTCSS_MASTER)
           && (appuser_info.app_title.compareTo("NTCSS") != 0))
        {
          Log.debug(this_servers_name, this_methods_name, "Adding user to "
                     + "bulletin board for app " + appuser_info.app_title);
          // Admin membership is addressed in apply_role_changes()
          if (DataLib.subscribeToAppBulletinBoard(
                                               updated_user_info.str_login_name,
                                               appuser_info.app_title, 
                                               db_connection))
            ini_files.WriteProfileString(this_hosts_name, "Add_To_"
                                          + appuser_info.app_title + "_BB",
                                          "OK", error_log_filename);
          else
            // This error is not really critical so we don't do anything
            // but report it...
            ini_files.WriteProfileString(this_hosts_name, "Add_To_"
                                          + appuser_info.app_title + "_BB",
                                          "FAILED", error_log_filename);
        }


      return 0;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
    finally
    {
      try
      {
        if (SQL_statement != null)
          SQL_statement.close();
        if (db_result_set != null)
          db_result_set.close();
      } catch (Exception exception)
      {}
    }
  }


  //=============================================================================
  /**
   * This function was designed to be called from ApplyRoleChanges() to remove
   * an appuser record.
   **/

  private int  ApplyRemoveAppUser(RoleChanges app_data)
  {
    final String  this_methods_name = "ApplyRemoveAppUser";
    Statement  SQL_statement = null;
    ResultSet  db_result_set = null;
    int        n_error_count = 0;

    try
    {
      // First we have to make sure this appuser is not registered with an
      //  the app's database before we try to delete them.
      Object args[] = new Object[3];
      args[0] = updated_user_info.str_login_name;
      args[1] = app_data.app_title;
      args[2] = new Boolean(true);

      Log.debug(this_servers_name, this_methods_name,
                "Checking if user " + updated_user_info.str_login_name
                 + " is registered to app " + app_data.app_title);
      String    SQL_query_string;
      SQL_query_string = StringLib.SQLformat("SELECT COUNT(*) as x FROM "
            + " app_users WHERE login_name=" + Constants.ST + " AND app_title="
            + Constants.ST + " AND registered_user=" + Constants.ST, args);

      int n_found = 1;  // Assume app user is registered by default
      SQL_statement = db_connection.createStatement();

      db_result_set = SQL_statement.executeQuery(SQL_query_string);

      if (db_result_set.next())
        n_found = db_result_set.getInt(1);

      db_result_set.close();

      if (n_found > 0)
        { 
          Log.error(this_servers_name, this_methods_name,
                    "User " + updated_user_info.str_login_name
                     + " is currently registered within app "
                     + app_data.app_title + ".  Aborting removal.");

          ini_files.WriteProfileString(this_hosts_name, app_data.app_title,
                                       "Cannot delete a registered user!",
                                       error_log_filename);
          app_data.appusers.f_failed = true;
          app_data.skipped = true;
          changes.change_roles.local_db_part.f_failed = true;

          if (this_servers_role == Constants.NTCSS_MASTER)
            {
              // Remove from the ini file to prevent further application!
              ini_files.RemoveProfileString("ROLE CHANGES", app_data.app_title,
                                            input_filename);
              ini_files.FlushINIFile(input_filename); // Get this to disk ASAP.
              // Store this to disk ASAP!
            }
          SQL_statement.close();
          return -1;
        }

      Log.debug(this_servers_name, this_methods_name,
                "Removing app user " + updated_user_info.str_login_name
                 + " from app " + app_data.app_title + " in the database...");

      // Temprorary cascade delete fix...
      SQL_query_string = StringLib.SQLformat("login_name=" + Constants.ST
                                      + " AND app_title=" + Constants.ST, args);
      // Now delete this appuser..
      //SQL_query_string = StringLib.SQLformat("DELETE FROM app_users WHERE "
      //        + "login_name=" + Constants.ST + " AND app_title=" + Constants.ST,
      //       args);

      last_warning_msg = "No matching record(s) found.";
      int i_temp = 0;
      try
      {
        i_temp = DataLib.cascadeDelete(SQL_statement, "app_users",
                                       SQL_query_string);
        //i_temp = SQL_statement.executeUpdate(SQL_query_string);
        if (i_temp < 0)
          {
            last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
            Log.error(this_servers_name, this_methods_name, last_warning_msg);
          }
      }

      catch(Exception exception)
      {
        i_temp = -1;   // Just set this so it will be seen below.
        last_warning_msg = exception.toString();
        Log.excp(this_servers_name, this_methods_name, last_warning_msg);
      }
      if (i_temp < 1)
        {
          ini_files.WriteProfileString(this_hosts_name, app_data.app_title,
                                       last_warning_msg, error_log_filename);
          app_data.appusers.f_failed = true;
          app_data.skipped = true;
          changes.change_roles.local_db_part.f_failed = true;
          ini_files.WriteProfileString(this_hosts_name, app_data.app_title
                                       + "_Remove_Appuser", last_warning_msg,
                                       error_log_filename);

          if (this_servers_role == Constants.NTCSS_MASTER)
            {
              // Remove from the ini file to prevent further application!
              ini_files.RemoveProfileString("ROLE CHANGES", app_data.app_title,
                                            input_filename);
              ini_files.FlushINIFile(input_filename); // Get this to disk ASAP.
            }
          SQL_statement.close();
          return -2;
        }
      else
        {
          ini_files.WriteProfileString(this_hosts_name, app_data.app_title
                                       + "_Remove_Appuser", "OK",
                                       error_log_filename);
        }
      SQL_statement.close();

      // If we got here, we've sucessfully deleted this app.
      updated_user_info.n_apps--;

      // If this was the last app to be removed for this user, we remove the
      // user directory as well -- unless this is the master server or user's
      // auth server (which should do this in a different operation)
      if ((updated_user_info.n_apps < 1) && (!f_users_auth)
          && (this_servers_role != Constants.NTCSS_MASTER))
        {
          changes.remove_user.local_db_part.f_requested = true;
          changes.remove_user.local_os_part.f_requested = true;
          Log.debug(this_servers_name, this_methods_name,
               "Set flags to remove the OS account for user "
                + updated_user_info.str_login_name
                + " since they're last app has been deleted.");
        } // End of user directory stuff.

      // We don't worry about removing the ntcss_users entry for this user if it
      // is not their auth/master server..
      // This should be removed (if necessary) by the LRS-related login
      // functions when it finds an ntcss_users record not associated
      // with any apps on this auth server.  (May have changed since 3.07)

      if (this_servers_role != Constants.NTCSS_MASTER)
        {
          ini_files.WriteProfileString(this_hosts_name, app_data.app_title,
                                       "OK", error_log_filename);
          return 0;
        }

      // The following is only run on the master server.
      int x = UserLib.RemoveUserFromOSGroup(updated_user_info.str_login_name,
                                            app_data.group_name);
      if (x != 0)
        {
          app_data.GID.f_failed = true;
          changes.change_roles.local_os_part.f_failed = true;
          ini_files.WriteProfileString(this_hosts_name,
                                "Remove_from_" + app_data.group_name + "_GID",
                                x + "", error_log_filename);
          n_error_count++;
        }

      ini_files.WriteProfileString(this_hosts_name, app_data.app_title,
                                   "OK", error_log_filename);
      if (NDSLib.removeUserFromGroup(updated_user_info.str_login_name,
                                     app_data.group_name))
        ini_files.WriteProfileString(this_hosts_name,
                                   app_data.app_title + "_Remove_From_NDS",
                                   "OK", error_log_filename);
       else
        {
           ini_files.WriteProfileString(this_hosts_name,
                                    app_data.app_title + "_Remove_From_NDS",
                                    "Failed", error_log_filename);
           n_error_count++;
        }

      // Remove this app user from the appropriate BB on the master server..
      if ( (this_servers_role == Constants.NTCSS_MASTER)
           && (app_data.app_title.compareTo("NTCSS") != 0))
        {
          // Admin membership is addressed in apply_role_changes()
          if (DataLib.unsubscribeFromAppBulletinBoard(
                                             updated_user_info.str_login_name,
                                             app_data.app_title, db_connection))
            ini_files.WriteProfileString(this_hosts_name,
                                         "Remove_From_" + app_data.app_title
                                          + "_BB", "OK",
                                         error_log_filename);
          else
            // This error is not really critical so we don't do anything
            // but report it...
            ini_files.WriteProfileString(this_hosts_name,
                                         "Remove_From_" + app_data.app_title
                                          + "_BB", "FAILED",
                                         error_log_filename);
        }

      return n_error_count;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
    finally
    {
      try
      {
        if (SQL_statement != null)
          SQL_statement.close();
        if (db_result_set != null)
          db_result_set.close();
      } catch (Exception exception)
      {}
    }
  }


  //=============================================================================
  /**
   * This function was designed to be called from UndoRoleChanges() to add back
   * an appuser that was deleted.
   **/

  private int  UndoRemoveAppUser( RoleChanges app_info )
  {
    final String  this_methods_name = "UndoRemoveAppUser";
    Statement  SQL_statement = null;

    try
    {
      int  n_errors = 0;

      SQL_statement = db_connection.createStatement();
      // Assume that the User's record is still present or has been added back
      // in before this function was called during the sequence of undo's.
      // (This undo should come after the undo-remove-user)
      if (app_info.appusers.undo_status == UNDO_REQUESTED)
        {
          //sprintf(str_tmp_tag100, "%s_Remove_Appuser_Undo",
          //app_info.app_title);
          Object arg_list[] = new Object[6];
          // NOTE: We do not bother restoring the app_password here since this
          //  appuser is supposed to have been deleted anyway.
          arg_list[0] = app_info.app_title;
          arg_list[1] = updated_user_info.str_login_name;
          arg_list[2] = null;             // custom app data
          arg_list[3] = old_user_data.str_db_passwd;
          arg_list[4] = new Boolean(false);     // registered user
          arg_list[5] = new Boolean(false);     // batch user

          Log.debug(this_servers_name, this_methods_name,
               "Restoring user " + updated_user_info.str_login_name
                + " to app " + app_info.app_title + " in the database...");
          String SQL_query_string;
          SQL_query_string = StringLib.SQLformat(
                "INSERT INTO APP_USERS(app_title,"
                 + "login_name,custom_app_data,app_passwd,"
                 + "registered_user,batch_user) VALUES("
                 + Constants.ST + "," + Constants.ST + "," + Constants.ST + ","
                 + Constants.ST + "," + Constants.ST + "," + Constants.ST + ")",
                arg_list);

          last_warning_msg = "Unknown error";
          int   i_temp = 0;
          try
          {
            i_temp = SQL_statement.executeUpdate(SQL_query_string);
             if (i_temp < 0)
                {
                  last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
                  Log.error(this_servers_name, this_methods_name,
                            last_warning_msg);
                }
          }
          catch(Exception exception)
          {
            i_temp = -1;   // Just set this so it will be seen below.
            last_warning_msg = exception.toString();
            Log.excp(this_servers_name, this_methods_name, last_warning_msg);
          }
          if (i_temp < 1)
            {
              app_info.appusers.undo_status = UNDO_FAILED;
              ini_files.WriteProfileString(this_hosts_name,
                                           app_info.app_title
                                           + "_Remove_Appuser_Undo",
                                           last_warning_msg,
                                           error_log_filename);
              n_errors++;
            }
          else    // All went OK.
            {
              app_info.appusers.undo_status = UNDO_SUCCESSFUL;
              ini_files.WriteProfileString(this_hosts_name,
                                           app_info.app_title
                                           + "_Remove_Appuser_Undo", "OK",
                                           error_log_filename);

              // A user directory should already exist if this was the master
              // or this user's auth server.  But on other servers, it may not
              // since we never really add a user account through any other
              // means.  But if not, we create one so root can "su -" as
              // this user on Solaris..
              if ((updated_user_info.n_apps < 1)
                  && !f_users_auth
                  && (this_servers_role != Constants.NTCSS_MASTER))
                {
                  Log.debug(this_servers_name, this_methods_name,
                           "Recreating authserver OS account and/or directory"
                            + " for user " + updated_user_info.str_login_name);
                  if (UserLib.CreateAuthSvrAccount(
                                          updated_user_info.str_login_name,
                                          String.valueOf(old_user_data.i_os_id),
                                          Constants.NTCSS_UNIX_USERS_DIR) == 0)
                    // We make this conditional since if it doesn't work,
                    // any other app additions will give us additional tries
                    // for success.
                    updated_user_info.n_apps++;
                  else
                    {
                      // Non-fatal but a hassle nonetheless..
                      ini_files.WriteProfileString(this_hosts_name,
                                         "General_Error_Msg",
                                         "Problems creating user's local"
                                          + " directory during an undo appuser"
                                          + " addition.  Must now be created "
                                          + "manually!",
                                         error_log_filename);
                      n_errors++;
                    }
                } // End of user directory mods
            } // End of successful SQL apply
        }
      SQL_statement.close();

      // Since the only thing left to do is the os GID change on the master...
      if (this_servers_role != Constants.NTCSS_MASTER)
        return(n_errors);

      if (app_info.GID.undo_status == UNDO_REQUESTED)
        {
          Log.debug(this_servers_name, this_methods_name,
                   "Restoring user " + updated_user_info.str_login_name
                   + " to OS group " + app_info.group_name);
          if (UserLib.AddUserToOSGroup(updated_user_info.str_login_name,
                                       app_info.group_name) != 0)
            {
              app_info.GID.undo_status = UNDO_FAILED;
              ini_files.WriteProfileString(this_hosts_name,
                                           app_info.app_title
                                           + "_Remove_Appuser_Undo",
                                           "Unable to Add user to OS group!",
                                           error_log_filename);
              n_errors++;
            }
          else
            {
              app_info.GID.undo_status = UNDO_SUCCESSFUL;
              ini_files.WriteProfileString(this_hosts_name,
                                           app_info.app_title
                                           + "_Remove_Appuser_Undo", "OK",
                                           error_log_filename);
            }

          if (NDSLib.addUserToGroup(updated_user_info.str_login_name,
                                    app_info.group_name))
            ini_files.WriteProfileString(this_hosts_name,
                               app_info.app_title + "_Undo_Remove_From_NDS",
                               "OK", error_log_filename);
          else
            {
               ini_files.WriteProfileString(this_hosts_name,
                               app_info.app_title + "_Undo_Remove_From_NDS",
                               "Failed", error_log_filename);
               n_errors++;
            }
        }

      // Re-add this user to the app's general message bulletin board, unless
      // the app happens to be NTCSS (since that is handled only via user
      // additions/removals.
      if ( (this_servers_role == Constants.NTCSS_MASTER)
           && (app_info.app_title.compareTo("NTCSS") != 0))
        {
          Log.debug(this_servers_name, this_methods_name, "Re-adding user to "
                     + "the NTCSS bulletin board");
          // Admin membership is addressed in apply_role_changes()
          if (DataLib.subscribeToAppBulletinBoard(
                                               updated_user_info.str_login_name,
                                               app_info.app_title, 
                                               db_connection))
            ini_files.WriteProfileString(this_hosts_name, "Re-add_To_"
                                          + app_info.app_title + "_BB",
                                          "OK", error_log_filename);
          else
            // This error is not really critical so we don't do anything
            // but report it...
            ini_files.WriteProfileString(this_hosts_name, "Re-add_To_"
                                          + app_info.app_title + "_BB",
                                          "FAILED", error_log_filename);
        }

      return n_errors;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
    finally
    {
      try
      {
        if (SQL_statement != null)
          SQL_statement.close();
      }
      catch (Exception exception)
      {}
    }
  }


  //============================================================================
  /**
   * Changes a users SSN number in NDS and local database.
   **/

  private int  ApplySSNchange()
  {
    final String  this_methods_name = "ApplySSNchange";
    Statement  SQL_statement = null;

    try
    {
      int  i_error_count = 0;
      // Complete the NDS change first (for no particular reason)
      if ((this_servers_role == Constants.NTCSS_MASTER)
          && changes.change_ssn.NDS_part.f_requested)
        {
          Log.debug(this_servers_name, this_methods_name,
                   "Changing NDS SSN for user "
                    + updated_user_info.str_login_name);
          if (!NDSLib.modifySSN(updated_user_info.str_login_name,
                                updated_user_info.str_ssn))
            {
              ini_files.WriteProfileString(this_hosts_name, "Change_SSN_NDS",
                                           "NDS operation failed!",
                                           error_log_filename);
              changes.change_ssn.NDS_part.f_failed = true;
              i_error_count++;
            }
          else
            ini_files.WriteProfileString(this_hosts_name, "Change_SSN_NDS",
                                         "OK", error_log_filename);
        }

      Object arg_list[] = new Object[2];
      arg_list[0] = updated_user_info.str_ssn;
      arg_list[1] = updated_user_info.str_login_name;

      Log.debug(this_servers_name, this_methods_name,
                   "Changing the database SSN for "
                     + updated_user_info.str_login_name);
      // Prepare the SQL string..
      String  SQL_query_string = StringLib.SQLformat(
                            "UPDATE USERS set ss_number=" + Constants.ST
                             + "WHERE login_name=" + Constants.ST,
                            arg_list);

      SQL_statement = db_connection.createStatement();
      last_warning_msg = "No matching record(s) found!";
      int  i_temp = 0;
      try
      {
        i_temp = SQL_statement.executeUpdate(SQL_query_string);
        if (i_temp < 0)
          {
            last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
            Log.error(this_servers_name, this_methods_name, last_warning_msg);
          }
      }
      catch(Exception exception)
      {
        i_temp = -1;   // Just set this so it will be seen below.
        last_warning_msg = exception.toString();
        Log.excp(this_servers_name, this_methods_name, last_warning_msg);
      }
      if (i_temp < 1)
        {
          // Just for the legacy master UNIX servers...
          ini_files.WriteProfileString(this_hosts_name, "Change_User_Data",
                                       last_warning_msg,
                                       error_log_filename);
          ini_files.WriteProfileString(this_hosts_name, "Change_SSN",
                                       last_warning_msg, error_log_filename);
          // Since this is benign.. don't let a SSN change failure stop the rest
          // of the changes that may get forwarded to other auth servers...
          ini_files.RemoveProfileString("CHANGES", "ChangeSSN", input_filename);
          ini_files.FlushINIFile(input_filename); // Get this to disk ASAP.
          changes.change_ssn.local_db_part.f_failed = true;
          changes.change_ssn.auths_db_part.f_skip = true;
          changes.change_ssn.NDS_part.undo_status = UNDO_REQUESTED;
          i_error_count++;
        }
      else
        ini_files.WriteProfileString(this_hosts_name, "Change_SSN", "OK",
                                     error_log_filename);

      SQL_statement.close();
      return i_error_count;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
    finally
    {
      try
      {
        if (SQL_statement != null)
          SQL_statement.close();
      }
      catch (Exception exception)
      {}
    }
  }


  //============================================================================
  /**
   *  Changes the user's phone number in the RDBMS
   */
  private int  ApplyPhoneNumChange()
  {
    final String  this_methods_name = "ApplyPhoneNumChange";
    Statement  SQL_statement = null;
    
    try
    {
      int  i_error_count = 0;
      String args[] = new String[2];
      args[0] = updated_user_info.str_phone_number;
      args[1] = updated_user_info.str_login_name;

      Log.debug(this_servers_name, this_methods_name,
                   "Changing the (database) phone number for user "
                     + updated_user_info.str_login_name);
      String     SQL_query_string;
      SQL_query_string = StringLib.SQLformat(
                              "UPDATE USERS set phone_number=" + Constants.ST
                               + " WHERE login_name=" + Constants.ST, args);

      SQL_statement = db_connection.createStatement();
      last_warning_msg =  "";
      int  i_temp = 0;
      try
      {
        i_temp = SQL_statement.executeUpdate(SQL_query_string);
        if (i_temp < 0)
          {
            last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
            Log.error(this_servers_name, this_methods_name, last_warning_msg);
          }
      }
      catch(Exception exception)
      {
        i_temp = -1;   // Just set this so it will be seen below.
        last_warning_msg = exception.toString();
        Log.excp(this_servers_name, this_methods_name, last_warning_msg);
      }
      if (i_temp < 1)
        {
          // Just for the legacy master UNIX servers...
          ini_files.WriteProfileString(this_hosts_name, "Change_User_Data",
                                       last_warning_msg,
                                       error_log_filename);
          ini_files.WriteProfileString(this_hosts_name, "Change_Phone_Num",
                                       last_warning_msg, error_log_filename);
          ini_files.RemoveProfileString("CHANGES", "ChangePhoneNum",
                                        input_filename);
          ini_files.FlushINIFile(input_filename); // Get this to disk ASAP.
          changes.change_ph_num.local_db_part.f_failed = true;
          changes.change_ph_num.auths_db_part.f_skip = true;
          i_error_count++;
        }
      else
        ini_files.WriteProfileString(this_hosts_name, "Change_Phone_Num", "OK",
                                     error_log_filename);

      SQL_statement.close();
      return i_error_count;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
    finally
    {
      try
      {
        if (SQL_statement != null)
          SQL_statement.close();
      }
      catch (Exception exception)
      {}
    }
  }


  //============================================================================
  /*
   * This method changes the user's security class in the RDBMS
   **/

  private int  ApplySecClassChange()
  {
    final String  this_methods_name = "ApplySecClassChange";
    Statement  SQL_statement = null;

    try
    {
      int  i_error_count = 0;
      Object arg_list[] = new Object[2];
      arg_list[0] = new Integer(updated_user_info.i_security_class);
      arg_list[1] = updated_user_info.str_login_name;

      Log.debug(this_servers_name, this_methods_name,
                   "Changing the (database) security class to "
                    + updated_user_info.i_security_class
                    + " for user " + updated_user_info.str_login_name);
      // Prepare the SQL string..
      String  SQL_query_string = StringLib.SQLformat("UPDATE USERS set "
                                      + "security_class=" + Constants.ST
                                      + " WHERE login_name=" + Constants.ST,
                                     arg_list);

      SQL_statement = db_connection.createStatement();
      last_warning_msg = "No matching record(s) found!";
      int  i_temp = 0;
      try
      {
        i_temp = SQL_statement.executeUpdate(SQL_query_string);
        if (i_temp < 0)
          {
            last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
            Log.error(this_servers_name, this_methods_name, last_warning_msg);
          }
      }
      catch(Exception exception)
      {
        i_temp = -1;   // Just set this so it will be seen below.
        last_warning_msg = exception.toString();
        Log.excp(this_servers_name, this_methods_name, last_warning_msg);
      }
      if (i_temp < 1)
        {
          // Just for the legacy master UNIX servers...
          ini_files.WriteProfileString(this_hosts_name, "Change_User_Data",
                                       last_warning_msg,
                                       error_log_filename);
          ini_files.WriteProfileString(this_hosts_name, "Change_Sec_Class",
                                       last_warning_msg, error_log_filename);
          ini_files.RemoveProfileString("CHANGES", "ChangeSecClass",
                                        input_filename);
          ini_files.FlushINIFile(input_filename); // Get this to disk ASAP.
          changes.change_sec_class.local_db_part.f_failed = true;
          changes.change_sec_class.auths_db_part.f_skip = true;
          i_error_count++;
        }
      else
        ini_files.WriteProfileString(this_hosts_name, "Change_Sec_Class", "OK",
                                     error_log_filename);

      SQL_statement.close();
      return i_error_count;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
    finally
    {
      try
      {
        if (SQL_statement != null)
          SQL_statement.close();
      }
      catch (Exception exception)
      {}
    }
  }


  //============================================================================
  /**
   * Basically tries to remove the user account and db record for this user that
   * was just added.
   **/

  private int  UndoAddUser()
  {
    final String  this_methods_name = "UndoAddUser";
    Statement  SQL_statement = null;

    try
    {
      
      int  i_error_count = 0;
      int x;
      // First do some integrity checking on the flags:
      // If this is not the master OR if the previous OS part was never done:
      if (changes.add_user.local_os_part.undo_status == UNDO_REQUESTED)
        {
          // If the os account was never added, there's no os stuff to undo:
          if (changes.add_user.local_os_part.f_failed)
            changes.add_user.local_os_part.undo_status = NONE;
        }

      // If the db change failed, there's no db part to undo:
      if ( (changes.add_user.local_db_part.undo_status == UNDO_REQUESTED)
            && changes.add_user.local_db_part.f_failed)
        changes.add_user.local_db_part.undo_status = NONE;

      // Remove the user from NDS
      if ((this_servers_role == Constants.NTCSS_MASTER)
          && (changes.add_user.NDS_part.undo_status == UNDO_REQUESTED))
        {
          Log.debug(this_servers_name, this_methods_name,
                   " Removing new user " + updated_user_info.str_login_name
                    + " from NDS...");

          if (!NDSLib.deleteUser(updated_user_info.str_login_name))
            {
              ini_files.WriteProfileString(this_hosts_name,
                              "Add_User_to_NDS_Undo", "Failed to remove user!",
                              error_log_filename);
              changes.add_user.NDS_part.undo_status = UNDO_FAILED;
              i_error_count++;
            }
          else
            {
              ini_files.WriteProfileString(this_hosts_name,
                                           "Add_User_to_NDS_Undo",
                                           "OK", error_log_filename);
              changes.add_user.NDS_part.undo_status = UNDO_SUCCESSFUL;
            }
        }

      // Remove the os account:
      if (changes.add_user.local_os_part.undo_status == UNDO_REQUESTED)
        {
          Log.debug(this_servers_name, this_methods_name,
                    " Removing new user " + updated_user_info.str_login_name
                     + " from OS..");
          if (this_servers_role == Constants.NTCSS_MASTER)
            {
              Log.debug(this_servers_name, this_methods_name,
                   " Removing new user " + updated_user_info.str_login_name
                    + " from the OS...");
              x = UserLib.RemoveMasterSvrAccount(
                                             updated_user_info.str_login_name);
            }
          else
            x = UserLib.RemoveAuthSvrAccount(updated_user_info.str_login_name,
                                             Constants.NTCSS_UNIX_USERS_DIR);
          if (x != 0)
            {
              ini_files.WriteProfileString(this_hosts_name,
                                           "Add_os_Account_Undo",
                                           "Returned error " + x,
              error_log_filename);
              changes.add_user.local_os_part.undo_status = UNDO_FAILED;
              i_error_count++;
            }
          else
            {
              ini_files.WriteProfileString(this_hosts_name,
                                           "Add_os_Account_Undo",
                                           "OK", error_log_filename);
              changes.add_user.local_os_part.undo_status = UNDO_SUCCESSFUL;
            }
        }

      // Remove the local db record.
      if (changes.add_user.local_db_part.undo_status == UNDO_REQUESTED)
        {
          Object arg_list[] = new Object[1];
          arg_list[0] = updated_user_info.str_login_name;

          // Prepare the SQL string..
          String  SQL_query_string =
                  StringLib.SQLformat("login_name=" + Constants.ST, arg_list);

          Log.debug(this_servers_name, this_methods_name,
                   " Removing new user " + updated_user_info.str_login_name
                    + " from the local database...");
          SQL_statement = db_connection.createStatement();
          last_warning_msg = "No matching record(s) found!";
          int  i_temp = 0;
          try
          {
            //i_temp = SQL_statement.executeUpdate(SQL_query_string);
            i_temp = DataLib.cascadeDelete(SQL_statement, "users",
                                           SQL_query_string);
            if (i_temp < 0)
              {
                last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
                Log.error(this_servers_name, this_methods_name,
                          last_warning_msg);
              }
          }
          catch(Exception exception)
          {
            i_temp = -1;   // Just set this so it will be seen below.
            last_warning_msg = exception.toString();
            Log.excp(this_servers_name, this_methods_name, last_warning_msg);
          }
          if (i_temp < 1)
            {
              ini_files.WriteProfileString(this_hosts_name,
                                           "Add_Ntcss_DB_Account_Undo",
                                           last_warning_msg,
                                           error_log_filename);
              changes.add_user.local_db_part.undo_status = UNDO_FAILED;
              i_error_count++;
            }
          else
            {
              ini_files.WriteProfileString(this_hosts_name,
                                           "Add_Ntcss_DB_Account_Undo",
                                           "OK", error_log_filename);
              changes.add_user.local_db_part.undo_status = UNDO_SUCCESSFUL;
            }
          SQL_statement.close();
        }

      // This user is already removed from the BB tables due to cascade delete
      // that should have already been done just above.  However, we need to
      // remove the BB that bears the user's name as well.
      if (this_servers_role == Constants.NTCSS_MASTER)
        {
          // The fact that this flag is set like this indicates an undo
          // was attempted and successful
          if (changes.add_user.local_db_part.undo_status == UNDO_SUCCESSFUL)
            {
              // This not only clears this user from any remaining subscriptions
              // but also removes the bulletin board created with the user's
              // name.
              Log.debug(this_servers_name, this_methods_name,
                        "Removing user's BB from the list due to a failed"
                        + " add_user operation.");
              if (DataLib.clearStandardBBUserInfo(
                                              updated_user_info.str_login_name,
                                              false, db_connection))
                // Admin memebership is addressed in apply_role_changes()
                ini_files.WriteProfileString(this_hosts_name,
                                             "Undo_Add_to_NTCSS_BB", "OK",
                                             error_log_filename);
              else
                // This error is not really critical so we don't do anything
                // but report it...
                ini_files.WriteProfileString(this_hosts_name,
                                             "Undo_Add_to_NTCSS_BB", "FAILED",
                                             error_log_filename);
            }
          // Since the user change failed, we won't do anything either...
          if (changes.add_user.local_db_part.undo_status == UNDO_FAILED)
            // This error is not really critical so we don't do anything
            // but report it...
            ini_files.WriteProfileString(this_hosts_name,
                                         "Undo_Add_to_NTCSS_BB", "FAILED",
                                         error_log_filename);
        }

      return i_error_count;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
    finally
    {
      try
      {
        if (SQL_statement != null)
          SQL_statement.close();
      }
      catch (Exception exception)
      {}
    }
  }


  //============================================================================
  /**
   * Restores the original SS number (from the .ini file) for this user.
   */

  private int  UndoSSNChange()
  {
    final String  this_methods_name = "UndoSSNChange";
    Statement  SQL_statement = null;

    try
    {
      int  i_error_count = 0;
      // Make sure this undo was really requested:
      if (changes.change_ssn.local_db_part.undo_status == UNDO_REQUESTED)
        {
          Object arg_list[] = new Object[2];
          arg_list[0] = old_user_data.str_ssn;
          arg_list[1] = updated_user_info.str_login_name;

          Log.debug(this_servers_name, this_methods_name,
                    " Restoring old database SSN for user "
                     + updated_user_info.str_login_name);

          // Prepare the SQL string..
          String  SQL_query_string = StringLib.SQLformat("UPDATE USERS set"
                                    + " ss_number=" + Constants.ST
                                    + " WHERE login_name=" + Constants.ST,
                                   arg_list);

          SQL_statement = db_connection.createStatement();
          last_warning_msg = "No matching record(s) found!";
          int i_temp = 0;
          try
          {
            i_temp = SQL_statement.executeUpdate(SQL_query_string);
            if (i_temp < 0)
              {
                last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
                Log.error(this_servers_name, this_methods_name,
                          last_warning_msg);
              }
          }
          catch(Exception exception)
          {
            i_temp = -1;   // Just set this so it will be seen below.
            last_warning_msg = exception.toString();
            Log.excp(this_servers_name, this_methods_name, last_warning_msg);
          }
          if (i_temp < 1)
            {
              ini_files.WriteProfileString(this_hosts_name, "Change_SSN_Undo",
                                           last_warning_msg,
                                           error_log_filename);
              changes.change_ssn.local_db_part.undo_status = UNDO_FAILED;
              i_error_count++;
            }
          else
            {
              ini_files.WriteProfileString(this_hosts_name, "Change_SSN_Undo",
                                           "OK", error_log_filename);
              changes.change_ssn.local_db_part.undo_status = UNDO_SUCCESSFUL;
            }
        }

      // Now take care of the NDS change..
      if (changes.change_ssn.NDS_part.undo_status == UNDO_REQUESTED)
        {
          Log.debug(this_servers_name, this_methods_name,
                    " Restoring old NDS SSN for user "
                     + updated_user_info.str_login_name);

          if (!NDSLib.modifySSN(updated_user_info.str_login_name,
                                old_user_data.str_ssn))
            {
              ini_files.WriteProfileString(this_hosts_name,
                                           "Change_SSN_NDS_Undo",
                                           "Failed to change SSN back!",
                                           error_log_filename);
              changes.change_ssn.NDS_part.undo_status = UNDO_FAILED;
              i_error_count++;
            }
          else
            {
              ini_files.WriteProfileString(this_hosts_name,
                                           "Change_SSN_NDS_Undo",
                                           "OK", error_log_filename);
              changes.change_ssn.NDS_part.undo_status = UNDO_SUCCESSFUL;
            }
        }
      return i_error_count;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
    finally
    {
      try
      {
        if (SQL_statement != null)
          SQL_statement.close();
      }
      catch (Exception exception)
      {}
    }
  }


  //============================================================================
  /**
   * Restores the original phone number for this user.
   **/

  private int  UndoPhoneNumChange()
  {
    final String  this_methods_name = "UndoPhoneNumChange";
    Statement  SQL_statement = null;

    try  // Make sure this undo was really requested:
    {
      int   i_error_count = 0;

      if (changes.change_ph_num.local_db_part.undo_status != UNDO_REQUESTED)
        return 0;

      Object arg_list[] = new Object[2];
      arg_list[0] = old_user_data.str_phone_number;
      arg_list[1] = updated_user_info.str_login_name;

      Log.debug(this_servers_name, this_methods_name,
                    " Restoring old (database) phone number for user "
                     + updated_user_info.str_login_name);
      // Prepare the SQL string..
      String  SQL_query_string = StringLib.SQLformat("UPDATE USERS set"
                                       + " phone_number=" + Constants.ST
                                       + " WHERE login_name=" + Constants.ST,
                                      arg_list);

      SQL_statement = db_connection.createStatement();
      last_warning_msg = "No matching record(s) found!";
      int  i_temp = 0;
      try
      {
        i_temp = SQL_statement.executeUpdate(SQL_query_string);
        if (i_temp < 0)
          {
            last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
            Log.error(this_servers_name, this_methods_name, last_warning_msg);
          }
      }
      catch(Exception exception)
      {
        i_temp = -1;   // Just set this so it will be seen below.
        last_warning_msg = exception.toString();
        Log.excp(this_servers_name, this_methods_name, last_warning_msg);
      }
      if (i_temp < 1)
        {
          ini_files.WriteProfileString(this_hosts_name, "Change_Phone_Num_Undo",
                                       last_warning_msg, error_log_filename);
          changes.change_ph_num.local_db_part.undo_status = UNDO_FAILED;
          i_error_count++;
        }
      else
        {
          ini_files.WriteProfileString(this_hosts_name, "Change_Phone_Num_Undo",
                                       "OK", error_log_filename);
          changes.change_ph_num.local_db_part.undo_status = UNDO_SUCCESSFUL;
        }
      SQL_statement.close();

      return i_error_count;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
    finally
    {
      try
      {
        if (SQL_statement != null)
          SQL_statement.close();
      }
      catch (Exception exception)
      {}
    }
  }


  //============================================================================
  /**
   * Restores the original security classification (from the .ini file) for
   * this user.
   **/

  private int  UndoSecClassChange()
  {
    final String  this_methods_name = "UndoSecClassChange";
    Statement  SQL_statement = null;

    try
    {
      int   i_error_count = 0;
      // Make sure this undo was really requested:
      if (changes.change_sec_class.local_db_part.undo_status != UNDO_REQUESTED)
        return 0;

      Log.debug(this_servers_name, this_methods_name,
                " Restoring old (database) security classification for user "
                  + updated_user_info.str_login_name);
      Object arg_list[] = new Object[2];
      arg_list[0] = new Integer(old_user_data.i_security_class);
      arg_list[1] = updated_user_info.str_login_name;

      // Prepare the SQL string..
      String  SQL_query_string = StringLib.SQLformat("UPDATE USERS set"
                                        + " security_class=" + Constants.ST
                                        + " WHERE login_name=" + Constants.ST,
                                       arg_list);

      SQL_statement = db_connection.createStatement();
      last_warning_msg = "No matching record(s) found!";
      int  i_temp = 0;
      try
      {
        i_temp = SQL_statement.executeUpdate(SQL_query_string);
        if (i_temp < 0)
          {
            last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
            Log.error(this_servers_name, this_methods_name, last_warning_msg);
          }
      }
      catch(Exception exception)
      {
        i_temp = -1;   // Just set this so it will be seen below.
        last_warning_msg = exception.toString();
        Log.excp(this_servers_name, this_methods_name, last_warning_msg);
      }
      if (i_temp < 1)
        {
          ini_files.WriteProfileString(this_hosts_name, "Change_Sec_Class_Undo",
                                       last_warning_msg, error_log_filename);
          changes.change_sec_class.local_db_part.undo_status = UNDO_FAILED;
          i_error_count++;
        }
      else
        {
          ini_files.WriteProfileString(this_hosts_name, "Change_Sec_Class_Undo",
                                       "OK", error_log_filename);
          changes.change_sec_class.local_db_part.undo_status = UNDO_SUCCESSFUL;
        }
      SQL_statement.close();

      return i_error_count;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
    finally
    {
      try
      {
        if (SQL_statement != null)
          SQL_statement.close();
      }
      catch (Exception exception)
      {}
    }
  }


  //============================================================================
  /**
   * Restores the original full/real name for this user.
   **/

  private int  UndoFullNameChange()
  {
    final String  this_methods_name = "UndoFullNameChange";
    Statement  SQL_statement = null;

    try
    {
      int    x;
      int    i_error_count = 0;

      // Make sure we never try to make a os change on a non-master:
      if (this_servers_role == Constants.NTCSS_MASTER)
        if (changes.change_full_name.local_os_part.undo_status
                                                              == UNDO_REQUESTED)
          changes.change_full_name.local_os_part.undo_status = NONE;

      // The os part only involves editing the /etc/passwd GECOS field.
      if (changes.change_full_name.local_os_part.undo_status == UNDO_REQUESTED)
        {
          Log.debug(this_servers_name, this_methods_name,
                    " Restoring old OS long user name for user "
                     + updated_user_info.str_login_name);

          if (UserLib.ChangeSystemAccountRealName(
                                              updated_user_info.str_login_name,
                                              old_user_data.str_full_name) !=0)
            {
              ini_files.WriteProfileString(this_hosts_name,
                                           "Change_System_Full_Name_Undo",
                                           "Failed to update the OS's records",
                                           error_log_filename);
              changes.change_full_name.local_os_part.undo_status = UNDO_FAILED;
              i_error_count++;
            }
          else
            {
              ini_files.WriteProfileString(this_hosts_name,
                                           "Change_System_Full_Name_Undo", "OK",
                                           error_log_filename);
              changes.change_full_name.local_os_part.undo_status
                                                              = UNDO_SUCCESSFUL;
            }
        }
      
      if (changes.change_full_name.local_db_part.undo_status == UNDO_REQUESTED)
        {
          Log.debug(this_servers_name, this_methods_name,
                    " Restoring old (database) long name for user "
                     + updated_user_info.str_login_name);
          Object arg_list[] = new Object[2];
          arg_list[0] = old_user_data.str_full_name;
          arg_list[1] = updated_user_info.str_login_name;

          // Prepare the SQL string..
          String  SQL_query_string = StringLib.SQLformat(
                                            "UPDATE USERS set real_name="
                                            + Constants.ST + "WHERE login_name="
                                            + Constants.ST, arg_list);

          SQL_statement = db_connection.createStatement();
          last_warning_msg = "No matching record(s) found!";
          int  i_temp = 0;
          try
          {
            i_temp = SQL_statement.executeUpdate(SQL_query_string);
            if (i_temp < 0)
              {
                last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
                Log.error(this_servers_name, this_methods_name,
                          last_warning_msg);
              }
          }
          catch(Exception exception)
          {
            i_temp = -1;   // Just set this so it will be seen below.
            last_warning_msg = exception.toString();
            Log.excp(this_servers_name, this_methods_name, last_warning_msg);
          }
          if (i_temp < 1)
            {
              ini_files.WriteProfileString(this_hosts_name,
                                           "Change_DB_Full_Name_Undo",
                                           last_warning_msg,
                                           error_log_filename);
              changes.change_full_name.local_db_part.undo_status = UNDO_FAILED;
              i_error_count++;
            }
          else
            {
              ini_files.WriteProfileString(this_hosts_name,
                                           "Change_DB_Full_Name_Undo", "OK",
                                           error_log_filename);
              changes.change_full_name.local_db_part.undo_status
                                                              = UNDO_SUCCESSFUL;
            }
          SQL_statement.close();
        }
      return i_error_count;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
    finally
    {
      try
      {
        if (SQL_statement != null)
          SQL_statement.close();
      }
      catch (Exception exception)
      {}
    }
  }


  //=============================================================================
  /**
   * Restores the original password for this user.
   **/

  private int  UndoPasswordChange()
  {
    final String  this_methods_name = "UndoPasswordChange";
    Statement  SQL_statement = null;
    int        i_error_count = 0;

    try
    {
      if ((changes.change_password.local_db_part.undo_status != UNDO_REQUESTED)
          && (changes.change_password.local_os_part.undo_status
                                                            != UNDO_REQUESTED))
        return 0;

      // Make the db change
      if (changes.change_password.local_db_part.undo_status == UNDO_REQUESTED)
        {
          Log.debug(this_servers_name, this_methods_name,
                    " Restoring old (database) password for user "
                     + updated_user_info.str_login_name);
          Object arg_list[] = new Object[2];
          arg_list[0] = old_user_data.str_passwd_enc;
          arg_list[1] = updated_user_info.str_login_name;

          // Prepare the SQL string..
          String  SQL_query_string;
          SQL_query_string = StringLib.SQLformat("UPDATE USERS set password="
                                          + Constants.ST + " WHERE login_name="
                                          + Constants.ST, arg_list);

          SQL_statement = db_connection.createStatement();
          last_warning_msg = "No matching record(s) found!";
          int     i_temp = 0;
          try
          {
            i_temp = SQL_statement.executeUpdate(SQL_query_string);
            if (i_temp < 0)
              {
                last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
                Log.error(this_servers_name, this_methods_name,
                          last_warning_msg);
              }
          }
          catch(Exception exception)
          {
            i_temp = -1;   // Just set this so it will be seen below.
            last_warning_msg = exception.toString();
            Log.excp(this_servers_name, this_methods_name, last_warning_msg);
          }
          if (i_temp < 1)
            {
              ini_files.WriteProfileString(this_hosts_name,
                                           "Change_DB_Password_Undo",
                                           last_warning_msg, error_log_filename);
              changes.change_password.local_db_part.undo_status = UNDO_FAILED;
              i_error_count++;
            }
          else
            {
              ini_files.WriteProfileString(this_hosts_name,
                                           "Change_DB_Password_Undo", "OK",
                                           error_log_filename);
              changes.change_password.local_db_part.undo_status
                                                              = UNDO_SUCCESSFUL;
            }
          SQL_statement.close();
        }

      // Do an integrity check: OS changes should never happen on a non-master.
      if ((this_servers_role != Constants.NTCSS_MASTER)
          && (changes.change_password.local_os_part.undo_status
                                                             == UNDO_REQUESTED))
        changes.change_password.local_os_part.undo_status = NONE;

      if (changes.change_password.local_os_part.undo_status == UNDO_REQUESTED)
        {
          // First we have to get the plaintext back:
          StringBuffer  decrypted_password = new StringBuffer();
          if (CryptLib.plainDecryptString(updated_user_info.str_login_name
                          + updated_user_info.str_login_name,
                          CryptLib.trimEncryptedPasswordTerminator(
                                                  old_user_data.str_passwd_enc),
                          decrypted_password, false, true) != 0)
            {
              Log.error(this_servers_name, this_methods_name,
                        "Error decrypting user's password");
              ini_files.WriteProfileString(this_hosts_name,
                                        "Change_os_Password_Undo",
                                        "Failed to decrypt user's old password",
                                        error_log_filename);
              last_error = ErrorCodes.ERROR_INCORRECT_PASSWORD;
              changes.change_password.local_os_part.undo_status = UNDO_FAILED;
              i_error_count++;
            }
          else
            {
              Log.debug(this_servers_name, this_methods_name,
                    " Restoring old (OS) password for user "
                     + updated_user_info.str_login_name);
              // Change the OS password....
              if (UserLib.ChangeSystemAccountPassword(
                                          updated_user_info.str_login_name,
                                          decrypted_password.toString()) != 0)
                {
                  Log.error(this_servers_name, this_methods_name,
                            "Error reverting user's OS account password");
                  ini_files.WriteProfileString(this_hosts_name,
                                      "Change_os_Password_Undo",
                                      "Failed to decrypt user's old password",
                                      error_log_filename);
                  changes.change_password.local_os_part.undo_status
                                                                  = UNDO_FAILED;
                  i_error_count++;
                }
              else  // password change was successful
                {
                  ini_files.WriteProfileString(this_hosts_name,
                                               "Change_os_Password_Undo", "OK",
                                               error_log_filename);
                  changes.change_password.local_os_part.undo_status
                                                              = UNDO_SUCCESSFUL;
                }
            }
        } // Finished changing the OS password.
      
      // Now do the NDS reversion
      if (changes.change_password.NDS_part.undo_status  == UNDO_REQUESTED)
        {
          Log.debug(this_servers_name, this_methods_name,
                    " Restoring old (NDS) password for user "
                     + updated_user_info.str_login_name);
          if (!NDSLib.modifyNtcssPassword(updated_user_info.str_login_name,
                                          old_user_data.str_passwd_enc))
            {
              ini_files.WriteProfileString(this_hosts_name,
                                           "Change_NDS_Password_Undo",
                                           "Unknown failure", error_log_filename);
              changes.change_password.NDS_part.undo_status = UNDO_FAILED;
              i_error_count++;
            }
          else
            {
              ini_files.WriteProfileString(this_hosts_name,
                                           "Change_NDS_Password_Undo", "OK",
                                           error_log_filename);
              changes.change_password.NDS_part.undo_status = UNDO_SUCCESSFUL;
            }
        }
      
      return i_error_count;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
    finally
    {
      try
      {
        if (SQL_statement != null)
          SQL_statement.close();
      }
      catch (Exception exception)
      {}
    }
  }


  //=============================================================================
  /**
   *  Removes an appuser that was just added.
   **/

  private int  UndoAddAppUser( RoleChanges app_info )
  {
    final String  this_methods_name = "UndoAddAppUser";
    Statement  SQL_statement = null;

    try
    {
      int  n_errors = 0;

      if (app_info.appusers.undo_status == UNDO_REQUESTED)
        {
          Log.debug(this_servers_name, this_methods_name,
                    " Removing new app user "
                     + updated_user_info.str_login_name + "from app "
                     + app_info.app_title);
          Object args[] = new Object[2];
          args[0] = updated_user_info.str_login_name;
          args[1] = app_info.app_title;

          String    SQL_query_string;
          SQL_query_string = StringLib.SQLformat(
                                     " login_name=" + Constants.ST
                                     + " AND app_title=" + Constants.ST, args);

          SQL_statement = db_connection.createStatement();
          last_warning_msg = "No matching record(s) found!";
          int     i_temp = 0;
          try
          {
            //i_temp = SQL_statement.executeUpdate(SQL_query_string);
            i_temp = DataLib.cascadeDelete(SQL_statement, "app_users",
                                             SQL_query_string);
            if (i_temp < 0)
              {
                last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
                Log.error(this_servers_name, this_methods_name,
                          last_warning_msg);
              }
          }
          catch(Exception exception)
          {
            i_temp = -1;   // Just set this so it will be seen below.
            last_warning_msg = exception.toString();
            Log.excp(this_servers_name, this_methods_name, last_warning_msg);
          }
          if (i_temp < 1)
            {
              ini_files.WriteProfileString(this_hosts_name,
                                  app_info.app_title + "_Add_App_User_Undo",
                                  last_warning_msg, error_log_filename);
              app_info.appusers.undo_status = UNDO_FAILED;
              n_errors++;
            }
          else
            {
              app_info.appusers.undo_status = UNDO_SUCCESSFUL;
              ini_files.WriteProfileString(this_hosts_name,
                                    app_info.app_title + "_Add_App_User_Undo",
                                    "OK", error_log_filename);
              // We unset any undo requests for the roles since they
              //  are automagically removed when removing the appuser by the
              //  foreign key relationship.

              app_info.appuser_roles.undo_status = NONE;
              // Reduce the count of apps for which we retain this user's
              // home dir.
              updated_user_info.n_apps--;

              // If this was the last app to be removed for this user, we
              //remove the user directory as well -- unless this is the master
              //server or user's auth server (which should already do this anyway)
              if ( (updated_user_info.n_apps < 1)
                && !f_users_auth
                && (this_servers_role != Constants.NTCSS_MASTER))
                {
                  Log.debug(this_servers_name, this_methods_name,
                    " Removing new app user "
                     + updated_user_info.str_login_name + "'s OS account...");
                  if (UserLib.RemoveAuthSvrAccount(
                                          updated_user_info.str_login_name,
                                          Constants.NTCSS_UNIX_USERS_DIR) != 0)
                    {
                      // Non-fatal but a hassle nonetheless..
                      ini_files.WriteProfileString(this_hosts_name,
                                     "General_Error_Msg",
                                     "Problems encountered while removing "
                                      + " user's local directory during an app"
                                      + " additoin undo.  This user's account"
                                      + " must be removed manually!",
                                     error_log_filename);
                      n_errors++;
                    }
                } // End of user dir manipulation
            } // End of successful role removal.
          SQL_statement.close();
        }

      // We don't worry about removing the ntcss_users entry for this user if it
      // is not their auth/master server..
      // This should be removed (if necessary) by the LRS-related login
      // functions when it finds an ntcss_users record not associated
      // with any apps on this auth server.  (May have changed since 3.07)

      if (this_servers_role != Constants.NTCSS_MASTER)
        return n_errors;

      if (app_info.GID.undo_status != UNDO_REQUESTED)
        return n_errors;

      Log.debug(this_servers_name, this_methods_name,
                    " Removing new (app) user "
                     + updated_user_info.str_login_name + "from OS group "
                     + app_info.group_name);
      // The following is only run on the master server.
      int x = UserLib.RemoveUserFromOSGroup(updated_user_info.str_login_name,
                                            app_info.group_name);
      if (x != 0)
        {
          app_info.GID.undo_status = UNDO_FAILED;
          ini_files.WriteProfileString(this_hosts_name,
                                app_info.app_title + "_Add_App_User_GID_Undo",
                                "Error=" + x, error_log_filename);
          n_errors++;
        }
      else
        {
          ini_files.WriteProfileString(this_hosts_name,
                                app_info.app_title + "_Add_App_User_GID_Undo",
                                "OK", error_log_filename);
                                app_info.GID.undo_status = UNDO_SUCCESSFUL;
        }

      if (NDSLib.removeUserFromGroup(updated_user_info.str_login_name,
                                     app_info.group_name))
         ini_files.WriteProfileString(this_hosts_name,
                                  app_info.app_title + "_Undo_Add_To_NDS",
                                  "OK", error_log_filename);
      else
        {
           ini_files.WriteProfileString(this_hosts_name,
                                  app_info.app_title + "_Undo_Add_To_NDS",
                                  "Failed", error_log_filename);
           n_errors++;
        }

      // Remove this new user from the appropriate BB on the master server..
      if ( (this_servers_role == Constants.NTCSS_MASTER)
           && (app_info.app_title.compareTo(Constants.NTCSS_APP) == 0))
        {
          Log.debug(this_servers_name, this_methods_name, "Removing user from"
                    + " the " + app_info.app_title + " due to failed app role"
                    + " operation.");
          // Admin membership is addressed in apply_role_changes()
          if (DataLib.unsubscribeFromAppBulletinBoard(
                                             updated_user_info.str_login_name,
                                             app_info.app_title, db_connection))
            ini_files.WriteProfileString(this_hosts_name,
                                         "Undoing_Add_to_" + app_info.app_title
                                          + "_BB", "OK",
                                         error_log_filename);
          else
            // This error is not really critical so we don't do anything
            // but report it...
            ini_files.WriteProfileString(this_hosts_name,
                                         "Undoing_Add_to_" + app_info.app_title
                                          + "_BB", "FAILED",
                                         error_log_filename);
        }

      return n_errors;
    }

    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
    finally
    {
      try
      {
        if (SQL_statement != null)
          SQL_statement.close();
      }
      catch (Exception exception)
      {}
    }
  }


  //=============================================================================
  /**
   * Re-adds the original user back into the Ntcss environment.
   * Note that plain_password is only needed here because it is passed to os
   * crypt for adding to the /etc/passwd file line.
   * Also note that os_id gets set here and passed back up.
   **/

  private int  UndoRemoveUser()
  {
    final String  this_methods_name = "UndoRemoveUser";
    Statement  SQL_statement = null;

    try
    {
      int  n_errors = 0;

      // On the master server, We create the os account first so we can obtain
      //   the user's UID to set in the ntcss_users table.
      IntegerBuffer  uid = new IntegerBuffer();
      uid.setValue(0);
      if ((this_servers_role == Constants.NTCSS_MASTER)
          && (changes.remove_user.local_os_part.undo_status == UNDO_REQUESTED))
        {
          // We need to get the plaintext of the user's password back so we can
          // let the OS encrypt how it sees fit..
          StringBuffer  decrypted_password = new StringBuffer();
          if (CryptLib.plainDecryptString( updated_user_info.str_login_name
                               + updated_user_info.str_login_name,
                               CryptLib.trimEncryptedPasswordTerminator(
                                              updated_user_info.str_passwd_enc),
                               decrypted_password, false, true) != 0)
            {
              Log.error(this_servers_name, this_methods_name,
                        "Error decrypting user's old password");
              ini_files.WriteProfileString(this_hosts_name,
                                      "Remove_os_Account_Undo",
                                      "Failed to decrypt user's old password",
                                      error_log_filename);
              last_error = ErrorCodes.ERROR_INCORRECT_PASSWORD;
              n_errors++;
              return n_errors;
            }

          Log.debug(this_servers_name, this_methods_name,
                    " Restoring OS account for user "
                    + updated_user_info.str_login_name);
          if (UserLib.CreateMasterSvrAccount(updated_user_info.str_login_name,
                                             decrypted_password.toString(),
                                             Constants.NTCSS_UNIX_USERS_DIR,
                                             old_user_data.str_full_name,
                                             old_user_data.str_auth_server,
                                             uid) != 0)
            {
              ini_files.WriteProfileString(this_hosts_name,
                                          "Remove_os_Account_Undo",
                                          "Returned error", error_log_filename);
              changes.remove_user.local_os_part.undo_status = UNDO_FAILED;
              n_errors++;       // Non fatal, but should be noted in the return.
            }
          else
            {
              ini_files.WriteProfileString(this_hosts_name,
                                           "Remove_os_Account_Undo",
                                           "OK", error_log_filename);
              changes.add_user.local_os_part.undo_status = UNDO_SUCCESSFUL;
            }
          // Update the uids with the new value..
          old_user_data.i_os_id = uid.getValue();
          updated_user_info.i_os_id = uid.getValue();
        }           // Finished re-creating OS account.

      // This is all that's still left to do:
      if (changes.remove_user.local_db_part.undo_status != UNDO_REQUESTED)
        return n_errors;

      Log.debug(this_servers_name, this_methods_name,
                    " Restoring user "
                     + updated_user_info.str_login_name + " to the database..");
      // We assume that all the user fields contain the original values
      //  so we can just use them to restore the USERS table record for
      // this user.
      Object arg_list[] = new Object[14];
      arg_list[0] = updated_user_info.str_login_name;
      arg_list[1] = old_user_data.str_full_name;
      arg_list[2] = old_user_data.str_passwd_enc;
      arg_list[3] = new Integer(old_user_data.i_os_id);
      arg_list[4] = old_user_data.str_ssn;
      arg_list[5] = old_user_data.str_phone_number;
      arg_list[6] = new Integer(old_user_data.i_security_class);
      arg_list[7] = StringLib.getTimeStr();
      arg_list[8] = new Boolean(old_user_data.login_lock);
      arg_list[9] = old_user_data.str_db_passwd;
      arg_list[10] = old_user_data.str_auth_server;
      arg_list[11] = new Boolean(old_user_data.f_editable_user);
      arg_list[12] = new Boolean(old_user_data.ntcss_super_user);
      arg_list[13] = updated_user_info.str_login_name;

      // Prepare the SQL string..
      String  SQL_query_string =
      StringLib.SQLformat("INSERT INTO USERS(login_name,real_name,password,"
              + "unix_id,ss_number,phone_number,security_class,pw_change_time,"
              + "login_lock,shared_db_passwd,auth_server,editable_user,"
              + "ntcss_super_user, long_login_name) VALUES("
              + Constants.ST + "," + Constants.ST + "," + Constants.ST + ","
              + Constants.ST + "," + Constants.ST + "," + Constants.ST + ","
              + Constants.ST + "," + Constants.ST + "," + Constants.ST + ","
              + Constants.ST + "," + Constants.ST + "," + Constants.ST + ","
              + Constants.ST + "," + Constants.ST + ")", arg_list);

      SQL_statement = db_connection.createStatement();

      last_warning_msg = "Unknown Error";
      int     i_temp = 0;
      try
      {
        i_temp = SQL_statement.executeUpdate(SQL_query_string);
        if (i_temp < 0)
          {
            last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
            Log.error(this_servers_name, this_methods_name, last_warning_msg);
          }
      }
      catch(Exception exception)
      {
        i_temp = -1;   // Just set this so it will be seen below.
        last_warning_msg = exception.toString();
        Log.excp(this_servers_name, this_methods_name, last_warning_msg);
      }
      if (i_temp < 1)
        {
          ini_files.WriteProfileString(this_hosts_name,
                                       "Remove_Ntcss_DB_Account_Undo",
                                       last_warning_msg, error_log_filename);
          changes.remove_user.local_db_part.undo_status = UNDO_FAILED;
          n_errors++;
          // We return now since we can't re-add this user to the BB's if they
          // didn't get back into the users table.
          return n_errors;
        }
      else
        {
          ini_files.WriteProfileString(this_hosts_name,
                                       "Remove_Ntcss_DB_Account_Undo",
                                       "OK", error_log_filename);
          changes.remove_user.local_db_part.undo_status = UNDO_SUCCESSFUL;
        }

      // Re-add this user to the appropriate BB on the master server..
      if (this_servers_role == Constants.NTCSS_MASTER)
        {
          Log.debug(this_servers_name, this_methods_name, "Re-adding user to"
                    + " the standard BBs in the process of undoing a user"
                    + " removal.");
          // Admin memebership is addressed in apply_role_changes()
          if (DataLib.setupStandardBBUserInfo(updated_user_info.str_login_name,
                                              false, db_connection))
            ini_files.WriteProfileString(this_hosts_name,
                                         "Re-add_To_Standard_BBs", "OK",
                                         error_log_filename);
          else
            // This error is not really critical so we don't do anything
            // but report it...
            ini_files.WriteProfileString(this_hosts_name,
                                         "Re-add_To_Standard_BBs", "FAILED",
                                         error_log_filename);
        }
      return n_errors;

   }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
    finally
    {
      try
      {
        if (SQL_statement != null)
          SQL_statement.close();
      }
      catch (Exception exception)
      {}
    }
  }


  //=============================================================================
  /**
   * Self-explanatory.  Part of the undo set of functions.
   **/

  private int  UndoAuthServerChange()
  {
    final String  this_methods_name = "UndoAuthServerChange";
    Statement  SQL_statement = null;

    try
    {
      if (changes.change_auth_server.local_db_part.undo_status
                                                              != UNDO_REQUESTED)
        return 0;

      Log.debug(this_servers_name, this_methods_name,
                " Reverting (database) user "
                  + updated_user_info.str_login_name
                  + " to original auth server "
                  + old_user_data.str_auth_server);
      Object arg_list[] = new Object[2];
      arg_list[0] = old_user_data.str_auth_server;
      arg_list[1] = updated_user_info.str_login_name;

      // Prepare the SQL string..
      String  SQL_query_string =
              StringLib.SQLformat("UPDATE users SET auth_server=" + Constants.ST
                              + " WHERE login_name=" + Constants.ST, arg_list);

      SQL_statement = db_connection.createStatement();

      last_warning_msg = "No matching record(s) found!";
      int     i_temp = 0;
      try
      {
        i_temp = SQL_statement.executeUpdate(SQL_query_string);
        if (i_temp < 0)
          {
            last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
            Log.error(this_servers_name, this_methods_name, last_warning_msg);
          }
      }
      catch(Exception exception)
      {
        i_temp = -1;   // Just set this so it will be seen below.
        last_warning_msg = exception.toString();
        Log.excp(this_servers_name, this_methods_name, last_warning_msg);
      }
      if (i_temp < 1)
        {
          ini_files.WriteProfileString(this_hosts_name,
                                       "Change_Auth_Server_Undo",
                                       last_warning_msg, error_log_filename);
          changes.change_auth_server.local_db_part.undo_status = UNDO_FAILED;
          return -1;
        }

      SQL_statement.close();
      ini_files.WriteProfileString(this_hosts_name, "Change_Auth_Server_Undo",
                                   "OK", error_log_filename);
      changes.change_auth_server.local_db_part.undo_status = UNDO_SUCCESSFUL;

      // Handle the NDS portion of the change if needed..
      if (!changes.change_auth_server.NDS_part.f_skip
          && (this_servers_role == Constants.NTCSS_MASTER)
          && (changes.change_auth_server.NDS_part.undo_status == UNDO_REQUESTED))
        {
          Log.debug(this_servers_name, this_methods_name,
                    " Reverting (NDS) user "
                      + updated_user_info.str_login_name
                      + " to original auth server "
                      + old_user_data.str_auth_server);
          if (!NDSLib.modifyAuthServer(updated_user_info.str_login_name,
                                       old_user_data.str_auth_server))
            {
              changes.change_auth_server.NDS_part.undo_status = UNDO_FAILED;
              ini_files.WriteProfileString(this_hosts_name,
                                           "Change_NDS_Auth_Server_Undo",
                                           "Failed", error_log_filename);
            }
          else
            ini_files.WriteProfileString(this_hosts_name,
                                         "Change_NDS_Auth_Server_Undo",
                                         "OK", error_log_filename);
        }

      return 0;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      ini_files.WriteProfileString(this_hosts_name, "General_Error_Msg",
                                   exception.toString(), error_log_filename);

      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
    finally
    {
      try
      {
        if (SQL_statement != null)
          SQL_statement.close();
      }
      catch (Exception exception)
      {}
    }
  }


  //=============================================================================
  /**
   * Examines the particular sections of a remote appl;ication host to determine
   * what (if any) error occured and set the appropriate UNDO_REQUESTED flags
   * that in turn should be processed on the local host.
   * We assume this role change has not been skipped on the master and depend
   * on the calling function to handle this special case.
   **/

  private int  InterpretRoleChangeResults(RoleChanges app_info)
  {
    final String  this_methods_name = "InterpretRoleChangeResults";
    Statement  SQL_statement = null;

    try
    {
      // Only do the following if there IS a log file for this remote app host:
      String  temp_str;
      if (!app_info.no_log_file)
        {
          // First check the value for the app name tag:
          temp_str = ini_files.GetProfileString(app_info.app_server_name,
                                                app_info.app_title,
                                                "NO INFO", error_log_filename);

          if (temp_str.compareTo("OK") == 0)
            {
              Log.debug(this_servers_name, this_methods_name,
                            "Role changes for app " + app_info.app_title
                            + " on host " + app_info.app_server_name
                            + " were sucessful.");
              // This particular app's role change went fine so we just return.
              return 0;
            }

          // This is the main flag under the hostname section.
          temp_str = ini_files.GetProfileString(app_info.app_server_name,
                                                "Change_Appuser_Roles",
                                                "NO INFO",  error_log_filename);
          if (temp_str.compareTo("OK") == 0)
            {
              Log.debug(this_servers_name, this_methods_name,
                        "All role changes on host "
                         + app_info.app_server_name
                         + " were sucessful.");
              // Assume this means ALL remote app role changes went fine.
              return 0;
            }
        }

      // Beyond this point we assume a complete change falure occured
      // on the remote server and that nothing was actually applied on it.

      Log.debug(this_servers_name, this_methods_name, "Something went wrong"
                + " when applying role changes for app " + app_info.app_title
                + " on host " + app_info.app_server_name);
      // Change these general status flags - we don't really need them though.
      changes.change_roles.auths_db_part.f_failed = true;
      changes.change_roles.local_db_part.undo_status = UNDO_REQUESTED;
      // Assume there is no OS-part failure to report on the general structure:
      // changes.change_roles.auths_os_part.f_failed = 1;

      // Trip the undo roles flag for this app:
        app_info.appuser_roles.undo_status = UNDO_REQUESTED;

      // For all of the following, we first check to make sure the app change was
      //  requested and successfully applied locally:

      // Trip the local GID undo flag:
      if ( app_info.GID.f_requested
          && !app_info.GID.f_failed )
        {
          app_info.GID.undo_status = UNDO_REQUESTED;
        }

      // Trip the local appuser record db part undo flag if already requested or
      // if the user was supposed to be completely deleted.

      if (app_info.new_app && !app_info.appusers.f_failed)
        {
          app_info.appusers.undo_status = UNDO_REQUESTED;
          // If this is a new app, then the undoaddappuser method removes
          // all the roles in its cascaded delete before undorolechanges gets
          // around to it, cuasing an exception.  Thus, we unset
          // the undoapproles flag here.
          app_info.appuser_roles.undo_status = NONE;
        }

      // If the appuser record was removed, then it should be restored.
      if (app_info.remove_appuser && !app_info.appusers.f_failed)
        {
          app_info.appusers.undo_status = UNDO_REQUESTED;
        }

      if (app_info.appusers.f_requested
          && !app_info.appusers.f_failed)
        {
          app_info.appusers.undo_status = UNDO_REQUESTED;
        }
            
      if (changes.remove_user.local_db_part.f_requested
           && !changes.remove_user.local_db_part.f_failed
           && !changes.remove_user.local_db_part.f_skip)
        {
          app_info.appusers.undo_status = UNDO_REQUESTED;
        }

      // We don't trip the app_info.users undo flags when a user record was
      // added on another app's host (prerequisite to the insertion of a
      // new appuser record).  The users record will be deleted anyway, iff
      // there are no assiciated appuser records the next time the user
      // logs in.  Additionally, there could have been another appuser record
      // for this user added on the same host and requesting that the user
      // record be deleted would cause the other (possibly successful)
      // roles changes to inadvertantly get wiped out due to foreign
      // key relationships.

/********
      // Now trip all the undo flags for each individual role...
      AppRoles   current_role;
      for ( int i_role_index = 0;
            i_role_index < app_info.role_info.size();
            i_role_index++ )
        {
          current_role = (AppRoles)(app_info.role_info.elementAt(i_role_index));
          switch (current_role.action)
            { 
              case ROLE_REMOVED:  // The appuser still has this role
               {
Log.debug(this_servers_name, this_methods_name, "DWB App " + current_app.app_title + " role " + current_role.name + " is not new, so no change");
                  current_role.action = ROLE_PRESERVED;
                            break;  // from switch
                          }
                        default:
                          break;
                      }
                      break;  // out of inner search loop for current token
                    }

          // Setting this value will cause it to be restored in UndoRoleChanges
          ().action
                                                                 = ROLE_REMOVED;
        }
*********/
      return 1;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
    finally
    {
      try
      {
        if (SQL_statement != null)
          SQL_statement.close();
      }
      catch (Exception exception)
      {}
    }
  }


  //=============================================================================
  /**
   * This function checks to make sure this user is still a member of some apps
   * on their auth server (Because if they don't, they may get deleted on
   * the next login!) In case the user doesn't, then the auth server field is
   * changed to point to the master server.
   **/

  private int  EnsureUsersAuthServer()
  {
    final String  this_methods_name = "EnsureUsersAuthServer";
    Statement  SQL_statement = null;
    ResultSet   db_result_set = null;

    try
    {
      // If this is already the master, and is this user's auth server,
      // then this check is redundant.
      if ( (this_servers_role == Constants.NTCSS_MASTER)
          && f_users_auth)
        return 0;

      // If user has been removed oor an udo failed, calling Ensure..()
      // is redundant.
      if ( changes.remove_user.local_db_part.f_requested
           && ((changes.remove_user.local_db_part.undo_status == UNDO_FAILED)
               || (changes.remove_user.local_db_part.undo_status == NONE)))
        // User was not resurrected, or everything went fine everywhere else
        // so no use checking.
        return 0;

      if ( changes.add_user.local_db_part.f_requested
           && changes.add_user.local_db_part.f_failed)
        // User was never added so no use checking.
        return 0;

      Object arg_list[] = new Object[2];
      arg_list[0] = updated_user_info.str_auth_server;
      arg_list[1] = updated_user_info.str_login_name;

      String  SQL_query_string;
      SQL_query_string = StringLib.SQLformat(
                             "SELECT COUNT(*) FROM app_users, apps"
                              + " WHERE UPPER(apps.hostname)=UPPER("
                              + Constants.ST
                              + ") AND app_users.app_title=apps.app_title"
                              + " AND app_users.login_name=" + Constants.ST,
                             arg_list);

      SQL_statement = db_connection.createStatement();
      last_warning_msg = "";
      
      db_result_set = SQL_statement.executeQuery(SQL_query_string);
      if (!db_result_set.next())
        {
          last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
          Log.error(this_servers_name, this_methods_name, last_warning_msg);
          return -1;
        }
      else
        // Get the count of apps this user has..
        if (db_result_set.getInt(1) != 0)
          {
            // User must still have roles on their auth server, all is well.
            return(0);
          }

      // At this point we assume this user has no roles on their auth server,
      // so we point it to the master server
      arg_list[0] = master_servers_name.toString();
      // #2 was already set above

      Log.debug(this_servers_name, this_methods_name,
                " Reverting auth server to the master for user "
                  + updated_user_info.str_login_name);
      SQL_query_string = StringLib.SQLformat( "UPDATE users SET auth_server="
                                          + Constants.ST
                                          + " WHERE login_name=" + Constants.ST,
                                         arg_list);

      last_warning_msg = "No matching record(s) found!";
      int     i_temp = 0;
      try
      {
        i_temp = SQL_statement.executeUpdate(SQL_query_string);
        if (i_temp < 0)
          {
            last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
            Log.error(this_servers_name, this_methods_name, last_warning_msg);
          }
      }
      catch(Exception exception)
      {
        i_temp = -1;   // Just set this so it will be seen below.
        last_warning_msg = exception.toString();
        Log.excp(this_servers_name, this_methods_name, last_warning_msg);
      }
      if (i_temp < 1)
        {
          Log.error(this_servers_name, this_methods_name,
                    "Failed to change auth server to "
                     + master_servers_name.toString() + " for user "
                     + updated_user_info.str_login_name + "  "
                     + last_warning_msg);
          ini_files.WriteProfileString(this_hosts_name,
                                       "Rechanging_Auth_server",
                                       last_warning_msg, error_log_filename);
          return -2;
        }

      ini_files.WriteProfileString(this_hosts_name, "Rechanging_Auth_server",
                                   "OK", error_log_filename);

      return 0;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
    finally
    {
      try
      {
        if (SQL_statement != null)
          SQL_statement.close();
        if (db_result_set != null)
          db_result_set.close();
      }
      catch (Exception exception)
      {}
    }
  }


  //=============================================================================
  /**
   *  This method retreives and stores all the user roles available under an app.
   **/

  private int  GetAndStoreAppRoles( RoleChanges  appuser_info )
  {
    final String  this_methods_name = "GetAndStoreAppRoles";
    Statement  SQL_statement = null;
    ResultSet   db_result_set = null;

    try
    {
      int      i_num_roles;
      Object   arg_list[] = new Object[1];
      arg_list[0] = appuser_info.app_title;

      // Prepare the SQL string..
      String SQL_query_string = StringLib.SQLformat("SELECT role_name "
                + "FROM app_roles WHERE app_title =" + Constants.ST, arg_list);

      SQL_statement = db_connection.createStatement();
      last_warning_msg = "No matching record(s) found!";
      
      db_result_set = SQL_statement.executeQuery(SQL_query_string);

      if (!db_result_set.next())
        {
          last_warning_msg
                      = SQL_statement.getWarnings().getNextWarning().toString();
          ini_files.WriteProfileString(this_hosts_name, appuser_info.app_title,
                        last_warning_msg, error_log_filename);
          Log.error(this_servers_name, this_methods_name,
                    "Query failed to get roles for app "
                    + appuser_info.app_title + "  " + last_warning_msg);

          // Keep this problem from propagating...
          ini_files.RemoveProfileString("ROLE CHANGES", appuser_info.app_title,
                                        input_filename);
          ini_files.FlushINIFile(input_filename); // Get this to disk ASAP.
          return -1;
        }

      i_num_roles = 0;
      do  // Should already be pointing at a new record
        {
          // Store all the role names we just queried and set the markers..
          appuser_info.role_info.add(new AppRoles());
          ((AppRoles)(appuser_info.role_info.elementAt(i_num_roles))).name
                                        = db_result_set.getString("role_name");

          // Init the default action value for each new role name:
          ((AppRoles)(appuser_info.role_info.elementAt(i_num_roles))).action
                                                                  = ROLE_IGNORE;
          i_num_roles++;
        }
      while(db_result_set.next());

      return 0;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
    finally
    {
      try
      {
        if (SQL_statement != null)
          SQL_statement.close();
        if (db_result_set != null)
          db_result_set.close();
      }
      catch (Exception exception)
      {}
    }
  }


  //=============================================================================
  /**
   * Looks at this appuser's current roles for the given app and sets the
   * marker flags to signify which of all the roles this user already had.
   **/


  private int DetermineOldAppuserRoles( RoleChanges  appuser_info )
  {
    final String  this_methods_name = "DetermineOldAppuserRoles";
    Statement  SQL_statement = null;
    ResultSet   db_result_set = null;

    try
    {
      Object   arg_list[] = new Object[2];
      arg_list[0] = appuser_info.app_title;
      arg_list[1] = updated_user_info.str_login_name;

      // First we figure out if this is a new appuser or not...
      String SQL_query_string = StringLib.SQLformat("SELECT login_name "
                  + "FROM app_users WHERE app_title=" + Constants.ST
                  + " AND login_name=" + Constants.ST,
                 arg_list);

      SQL_statement = db_connection.createStatement();
      db_result_set = SQL_statement.executeQuery(SQL_query_string);

      if (!db_result_set.next())
        {
          Log.debug(this_servers_name, this_methods_name, "User "
                 + updated_user_info.str_login_name + " isn't a member of app "
                 + appuser_info.app_title);
          // No appuser recs so this must really be a new app for this user.
          appuser_info.new_app = true;
          return 0;
        }
      else
        appuser_info.new_app = false;

      // Now we determine which roles this appuser has...
      SQL_query_string = StringLib.SQLformat("SELECT role_name "
                  + "FROM user_app_roles WHERE app_title=" + Constants.ST
                  + " AND login_name=" + Constants.ST,
                 arg_list);

      db_result_set = SQL_statement.executeQuery(SQL_query_string);

      if (!db_result_set.next())
        {
          Log.debug(this_servers_name, this_methods_name, "User "
                  + updated_user_info.str_login_name + " had no roles for app "
                  + appuser_info.app_title);
          return 0;
        }

      // The user had pre-existing roles for this app so we need to indicate
      // the pre-existing roles in the ocmplate list...
      appuser_info.new_app = false;

      int i;
      String  temp_role;
      do
        {
          // We expect that all roles have already been marked "IGNORE" by
          // default..
          temp_role = db_result_set.getString("role_name");
          Log.debug(this_servers_name, this_methods_name, "User "
                  + updated_user_info.str_login_name + " had role " + temp_role
                  + " in app " + appuser_info.app_title);
          
          // Now find this role in the master list and set its flag...
          for (i = 0; i < appuser_info.role_info.size(); i++)
            if (temp_role.compareTo(
                          ((AppRoles)appuser_info.role_info.elementAt(i)).name
                              ) == 0)
              {
                ((AppRoles)appuser_info.role_info.elementAt(i)).action
                                                                = ROLE_ORIGINAL;
              }
        }
      while (db_result_set.next());

      return 0;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
    finally
    {
      try
      {
        if (SQL_statement != null)
          SQL_statement.close();
        if (db_result_set != null)
          db_result_set.close();
      }
      catch (Exception exception)
      {}
    }
  }


  //=============================================================================
  /**
   * Since the User Admin GUI does not always include all the user's information
   * needed for some changes, auth server for instance,
   * we make sure this info gets into the master's copy
   * before it forwards it to all the other affected servers.
   **/

  private int  AddUserDataToINI()
  {
    final String  this_methods_name = "AddUserDataToINI";

    try
    {
      ini_files.WriteProfileString("USER_INFO", "real_name",
                                   updated_user_info.str_full_name,
                                   input_filename);
      ini_files.WriteProfileString("USER_INFO", "ssn",
                                   updated_user_info.str_ssn,
                                   input_filename);
      ini_files.WriteProfileString("USER_INFO", "phone_number",
                                   updated_user_info.str_phone_number,
                                   input_filename);
      ini_files.WriteProfileString("USER_INFO", "security_class",
                                   updated_user_info.i_security_class + "",
                                   input_filename);
      ini_files.WriteProfileString("USER_INFO", "auth_server",
                                   updated_user_info.str_auth_server,
                                   input_filename);
      ini_files.WriteProfileString("USER_INFO", "unix_id",
                                   updated_user_info.i_os_id + "",
                                   input_filename);
      ini_files.WriteProfileString("USER_INFO", "password",
                                   updated_user_info.str_passwd_enc,
                                   input_filename);

      // Get this data into the file for the other servers.
      ini_files.FlushINIFile(input_filename);

      return 0;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      last_error = ErrorCodes.ERROR_CAUGHT_EXCEPTION;
      return last_error;
    }
  }


  //=============================================================================
  /**  
   * This is just for debugging purposes.  Uncomment the end of this statement
   * to activate this.
   *
   **/
  private void debugFlags(ChangesStatus change, String tree_name)
  {
    final String  this_methods_name = "dumpFlags";

    try
    {
      Log.debug(this_servers_name, this_methods_name, "Flags for " + tree_name);
      Log.debug(this_servers_name, this_methods_name, "\tNDS:       "
                + "Requested="
                + (change.NDS_part.f_requested ? "yes, " : "no,  ")
                + "Failed=" + (change.NDS_part.f_failed ? "yes, " : "no,  ")
                + "Skipped=" + (change.NDS_part.f_skip ? "yes, " : "no,  ")
                + "Undo status="
                + ((change.NDS_part.undo_status == UNDO_REQUESTED)
                                                      ? "Undo requested" : "")
                + ((change.NDS_part.undo_status == UNDO_SUCCESSFUL)
                                                      ? "Undo succeeded" : "")
                + ((change.NDS_part.undo_status == UNDO_FAILED)
                                                      ? "Undo failed" : "")
                + ((change.NDS_part.undo_status == NONE)
                                                      ? "Nothing to do" : ""));
      Log.debug(this_servers_name, this_methods_name, "\tLocal OS:  "
                + "Requested="
                + (change.local_os_part.f_requested ? "yes, " : "no,  ")
                + "Failed="
                + (change.local_os_part.f_failed ? "yes, " : "no,  ")
                + "Skipped=" + (change.local_os_part.f_skip ? "yes, " : "no,  ")
                + "Undo status="
                + ((change.local_os_part.undo_status == UNDO_REQUESTED)
                                                      ? "Undo requested" : "")
                + ((change.local_os_part.undo_status == UNDO_SUCCESSFUL)
                                                      ? "Undo succeeded" : "")
                + ((change.local_os_part.undo_status == UNDO_FAILED)
                                                      ? "Undo failed" : "")
                + ((change.local_os_part.undo_status == NONE)
                                                      ? "Nothing to do" : ""));
      Log.debug(this_servers_name, this_methods_name, "\tLocal DB:  "
                + "Requested="
                + (change.local_db_part.f_requested ? "yes, " : "no,  ")
                + "Failed="
                + (change.local_db_part.f_failed ? "yes, " : "no,  ")
                + "Skipped=" + (change.local_db_part.f_skip ? "yes, " : "no,  ")
                + "Undo status="
                + ((change.local_db_part.undo_status == UNDO_REQUESTED)
                                                      ? "Undo requested" : "")
                + ((change.local_db_part.undo_status == UNDO_SUCCESSFUL)
                                                      ? "Undo succeeded" : "")
                + ((change.local_db_part.undo_status == UNDO_FAILED)
                                                      ? "Undo failed" : "")
                + ((change.local_db_part.undo_status == NONE)
                                                      ? "Nothing to do" : ""));
      Log.debug(this_servers_name, this_methods_name, "\tAuth's DB:"
                + " Requested="
                + (change.auths_db_part.f_requested ? "yes, " : "no,  ")
                + "Failed="
                + (change.auths_db_part.f_failed ? "yes, " : "no,  ")
                + "Skipped=" + (change.auths_db_part.f_skip ? "yes, " : "no,  ")
                + "Undo status="
                + ((change.auths_db_part.undo_status == UNDO_REQUESTED)
                                                      ? "Undo requested" : "")
                + ((change.auths_db_part.undo_status == UNDO_SUCCESSFUL)
                                                      ? "Undo succeeded" : "")
                + ((change.auths_db_part.undo_status == UNDO_FAILED)
                                                      ? "Undo failed" : "")
                + ((change.auths_db_part.undo_status == NONE)
                                                      ? "Nothing to do" : ""));
      Log.debug(this_servers_name, this_methods_name, "\tAuth's OS: "
                + "Requested="
                + (change.auths_os_part.f_requested ? "yes, " : "no,  ")
                + "Failed="
                + (change.auths_os_part.f_failed ? "yes, " : "no,  ")
                + "Skipped=" + (change.auths_os_part.f_skip ? "yes, " : "no,  ")
                + "Undo status="
                + ((change.auths_os_part.undo_status == UNDO_REQUESTED)
                                                      ? "Undo requested" : "")
                + ((change.auths_os_part.undo_status == UNDO_SUCCESSFUL)
                                                      ? "Undo succeeded" : "")
                + ((change.auths_os_part.undo_status == UNDO_FAILED)
                                                      ? "Undo failed" : "")
                + ((change.auths_os_part.undo_status == NONE)
                                                      ? "Nothing to do" : ""));
      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      return;
    }
  }


  //=============================================================================
  
    /**  
   * This is just for debugging purposes.  Uncomment the end of this statement
   * to activate this.
   *
   **/
  private void debugAppInfo(RoleChanges app, boolean include_roles)
  {
    final String  this_methods_name = "dumpAppInfo";

    try
    {
      Log.debug(this_servers_name, this_methods_name, "Info for "
                                                      + app.app_title + ":");
      Log.debug(this_servers_name, this_methods_name,
                "\tLog file=" + (app.no_log_file ? "none, " : "yes,  ")
                + "Skipped=" + (app.skipped ? "yes, " : "no,  ")
                + "New app=" + (app.new_app ? "yes, " : "no,  ")
                + "Remove appuser=" + (app.remove_appuser ? "yes, " : "no,  ")
                + "OS group=" + app.group_name
                + "host=" + app.app_server_name
                + ", Server type="
                + ((app.app_server_type == Constants.NTCSS_MASTER)
                                                               ? "Master" : "")
                + ((app.app_server_type == Constants.NTCSS_AUTH_SERVER)
                                                               ? "Auth" : "")
                + ((app.app_server_type == Constants.NTCSS_APP_SERVER)
                                                               ? "App" : ""));
      Log.debug(this_servers_name, this_methods_name,
                "\tAdd user to local DB:      Requested="
                + (app.users.f_requested ? "yes, " : "no,  ")
                + "Failed=" + (app.users.f_failed ? "yes, " : "no,  ")
                + "Skipped=" + (app.users.f_skip ? "yes, " : "no,  ")
                + "Undo status="
                + ((app.users.undo_status == UNDO_REQUESTED)
                                                        ? "Undo requested" : "")
                + ((app.users.undo_status == UNDO_SUCCESSFUL)
                                                        ? "Undo succeeded" : "")
                + ((app.users.undo_status == UNDO_FAILED) ? "Undo failed" : "")
                + ((app.users.undo_status == NONE) ? "Nothing to do" : ""));
      Log.debug(this_servers_name, this_methods_name,
                "\tAdd app user to local DB:  Requested="
                + (app.appusers.f_requested ? "yes, " : "no,  ")
                + "Failed=" + (app.appusers.f_failed ? "yes, " : "no,  ")
                + "Skipped=" + (app.appusers.f_skip ? "yes, " : "no,  ")
                + "Undo status="
                + ((app.appusers.undo_status == UNDO_REQUESTED)
                                                      ? "Undo requested" : "")
                + ((app.appusers.undo_status == UNDO_SUCCESSFUL)
                                                      ? "Undo succeeded" : "")
                + ((app.appusers.undo_status == UNDO_FAILED)
                                                      ? "Undo failed" : "")
                + ((app.appusers.undo_status == NONE)
                                                      ? "Nothing to do" : ""));
      Log.debug(this_servers_name, this_methods_name,
                "\tAdd user to OS group:      Requested="
                + (app.GID.f_requested ? "yes, " : "no,  ")
                + "Failed=" + (app.GID.f_failed ? "yes, " : "no,  ")
                + "Skipped=" + (app.GID.f_skip ? "yes, " : "no,  ")
                + "Undo status="
                + ((app.GID.undo_status == UNDO_REQUESTED)
                                                      ? "Undo requested" : "")
                + ((app.GID.undo_status == UNDO_SUCCESSFUL)
                                                      ? "Undo succeeded" : "")
                + ((app.GID.undo_status == UNDO_FAILED)
                                                      ? "Undo failed" : "")
                + ((app.GID.undo_status == NONE)
                                                      ? "Nothing to do" : ""));
      if (!include_roles)
        return;

      // Go through and spit out all the roles and associated flags...
      StringBuffer output = new StringBuffer();
      output.append("\t" + app.app_title + " Roles requests:");
      
       for ( int i_role_index = 0;
             i_role_index < app.role_info.size();
             i_role_index++ )
          {
            AppRoles role_info;
            role_info = (AppRoles) app.role_info.elementAt(i_role_index);
            output.append(" " + role_info.name + "="
                    + ((role_info.action == ROLE_ORIGINAL) ? "already has" : "")
                    + ((role_info.action == ROLE_PRESERVED) ? "preserve" : "")
                    + ((role_info.action == ROLE_ADDED) ? "add" : "")
                    + ((role_info.action == ROLE_IGNORE) ? "skip" : "")
                    + ((role_info.action == ROLE_REMOVED) ? "remove" : ""));
          }
      Log.debug(this_servers_name, this_methods_name, output.toString());

      return;
    }
    catch (Exception exception)
    {
      Log.excp(this_servers_name, this_methods_name, exception.toString());
      return;
    }
  }


  //=============================================================================
  //=============================================================================
  //=============================================================================
  //=============================================================================

} // End of the UserAdmin class definition.