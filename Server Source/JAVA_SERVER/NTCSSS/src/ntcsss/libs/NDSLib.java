/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.libs;

import java.util.Hashtable;
import java.util.Vector;
import java.util.Enumeration;
import java.util.StringTokenizer;
import java.util.Random;
import java.util.ArrayList;

import java.io.File;
import java.io.FileReader;
import java.io.BufferedReader;

import java.sql.Connection;
import java.sql.Statement;
import java.sql.ResultSet;

import javax.naming.Context;
import javax.naming.NameClassPair;
import javax.naming.NameNotFoundException;
import javax.naming.NamingEnumeration;
import javax.naming.directory.InitialDirContext;
import javax.naming.directory.DirContext;
import javax.naming.directory.Attributes;
import javax.naming.directory.BasicAttribute;
import javax.naming.directory.BasicAttributes;
import javax.naming.directory.Attribute;
import javax.naming.directory.ModificationItem;
import javax.naming.directory.SearchResult;

import COMMON.FTP;

import ntcsss.log.Log;

import ntcsss.libs.DataLib;
import ntcsss.libs.structs.AdapterStruct;
import ntcsss.libs.structs.PrinterStruct;

import ntcsss.database.DatabaseConnectionManager;

/**
 * This library wraps up access to the Naming and Directory Services
 */
public class NDSLib
{      
   /**
    * Returns the given user's authentication server from NDS. If an error
    *  occurs, null is returned.
    */
   public static String getAuthServer(String loginname)
   {      
      return (String)getAuthUserAttribute(loginname, "authServerName");
   }
   
   /**
    * Modifies the given user's authentication server in NDS to be the
    *  given authentication server. If an error occurs, false is returned.
    */
   public static boolean modifyAuthServer(String loginname, String authServer)
   {
      return setAuthUserAttribute(loginname, "authServerName", authServer);                                               
   }
   
   /**
    * Returns the given user's NTCSS password from NDS. If an error
    *  occurs, null is returned.
    */
   public static String getNtcssPassword(String loginname)
   {  
/*      
      byte array[];
      
      try {
         array = (byte[])getAuthUserAttribute(loginname, "userPassword");      
         return new String(array);
      }
      catch (Exception exception) {
         return null;
      }
*/ 
      return (String)getAuthUserAttribute(loginname, "ntcssUserPassword");      
   }
   
   /**
    * Modifies the given user's encrypted password in NDS to be the
    *  given password. If an error occurs, false is returned.
    */
   public static boolean modifyNtcssPassword(String loginname, 
                                             String encryptedPassword)
   {
/*      
      return setAuthUserAttribute(loginname, "userPassword", 
                                  encryptedPassword.getBytes());                                               
 */
      return setAuthUserAttribute(loginname, "ntcssUserPassword", encryptedPassword);                                  
   }
   
   /**
    * Returns the given user's SSN from NDS. If an error
    *  occurs, null is returned.
    */
   public static String getSSN(String loginname)
   {  
      return (String)getAuthUserAttribute(loginname, "uniqueIdentifier");
   }
   
   /**
    * Modifies the given user's social security number in NDS to be the
    *  given SSN. If an error occurs, false is returned.
    */
   public static boolean modifySSN(String loginname, String SSN)                                             
   {
      return setAuthUserAttribute(loginname, "uniqueIdentifier", SSN);                                  
   }
   
   /**
    * Adds the given user to the proper NDS table. This method assumes that the
    *  user does not already exist in NDS. If an error occurs, false is
    *  returned.
    */
   public static boolean addUser(String login_name, String encryptedPassword,
                                 String socialSecNum, String authenticationServer)
   {
      DirContext dirContext;           
      BasicAttributes attributes;  
      DirContext tmpContext;
      
      try {
         
         // Create the initial directory context                                  
         if ((dirContext = ContextProvider.getContext()) == null) {
            Log.error("NDSLib", "addUser", "Unable to get context");
            return false;
         }
         
         // Create the LDAP entry
         attributes = new BasicAttributes();
         attributes.put("objectClass", "UserAuthServer");
         attributes.put("uid", login_name);
//         attributes.put("userPassword", encryptedPassword.getBytes());
         attributes.put("ntcssUserPassword", encryptedPassword);
         attributes.put("authServerName", authenticationServer);
         attributes.put("uniqueIdentifier", socialSecNum);                                                          
         
         // Add the new LDAP entry
         tmpContext = dirContext.createSubcontext("uid=" + login_name + ",uid=AUTHSERVERS", 
                                     attributes);
         tmpContext.close();
         
         // Close the directory context
         dirContext.close();
         
         return true;
      }
      catch (Exception exception) {
         Log.excp("NDSLib", "addUser", exception.toString());
         return false;
      }
   }
   
   /**
    * Removes the given user from the proper NDS table. This method assumes 
    *  that the user does exist in the table. If an error occurs, false is
    *  returned.
    */
   public static boolean deleteUser(String login_name)
   {      
      DirContext dirContext;                 
      
      try {
             
         // Create the initial directory context
         if ((dirContext = ContextProvider.getContext()) == null) {
            Log.error("NDSLib", "deleteUser", "Unable to get context");
            return false;
         }
         
         // Remove the LDAP object          
         dirContext.destroySubcontext("uid=" + login_name + ",uid=AUTHSERVERS");                  
                  
         dirContext.close();
         
         return true;
      }
      catch (Exception exception) {
         Log.excp("NDSLib", "deleteUser", exception.toString());
         return false;
      }
   }
   
   /**
    * Returns the given host's type from NDS. If an error
    *  occurs, -1 is returned.
    */
   public static int getNtcssHostType(String hostname)
   {
      Object type;
      
      if ((type = getHostAttribute(hostname, "cssServerType")) != null)      
         return StringLib.parseInt((String)type, -1);
      else
         return -1;             
   }  
   
   /**
    * Returns the given SYS_CONF key's value from NDS. If an error
    *  occurs or the key doesn't exist, null is returned.
    */
   public static String getSysConfValue(String key)
   {
      DirContext dirContext;
      NamingEnumeration objects;
      NameClassPair objectName;      
      Attributes attributes;
      Attribute attribute;
      String modifiedKey;
      String value;
      
      try {
         
         // Replace all the spaces with underscore characters because keys
         //  are inserted into LDAP with this conversion
         modifiedKey = key.replace(' ', '_');
         
         // Create the initial directory context
         if ((dirContext = ContextProvider.getContext()) == null) {
            Log.error("NDSLib", "getSysConfValue", "Unable to get context");
            return null;
         }
                   
         // This is in its own try/catch so that if the key doesn't exist,
         //  an exception isn't logged at the bottom of this method
         try {
            attributes = dirContext.getAttributes("sysConfAttribute=" + 
                                                  modifiedKey + ",uid=SYSCONF");
         }
         catch (Exception exp) {
            attributes = null;
         }
         
         value = null;
         if (attributes != null) {
            
            // The "sysconfData" attribute should be an attribute of this object
            attribute = attributes.get("sysconfData");            
            if (attribute != null)
               value = (String)attribute.get(); 
         }
         
         dirContext.close();
         
         return value;
      }      
      catch (Exception exception) {
         Log.excp("NDSLib", "getSysConfValue", exception.toString());                  
      }
      
      return null;
   }
   
   /**
    * Returns a table of tag/value pairs from NDS. If an error
    *  occurs, null is returned.
    */ 
   public static Hashtable getSysConfData()
   {
      return getSysConfData(null);
   }
   
   private static Hashtable getSysConfData(DirContext context)
   {
      DirContext dirContext;
      NamingEnumeration objects;
      NameClassPair objectName;      
      Attributes attributes;
      Attribute attribute;      
      Hashtable table;
      String tag;
      
      try {
         
         if (context == null) {
            
            // Create the initial directory context
            if ((dirContext = ContextProvider.getContext()) == null) {
               Log.error("NDSLib", "getSysConfData", "Unable to get context");
               return null;
            }
         }
         else
            dirContext = context;
         
         // Create the table to hold all the tag/value pairs
         table = new Hashtable();
                           
         // Get a list of the LDAP objects under this context         
         objects = dirContext.list("uid=SYSCONF");
         while (objects.hasMore() == true) {
            
            objectName = (NameClassPair)objects.next();     
            
            // Get the attributes of this object            
            attributes = dirContext.getAttributes(objectName.getName() + 
                                                  ", uid=SYSCONF");
            
            // The "sysconfData" attribute should be an attribute of this object
            attribute = attributes.get("sysconfData");            
            if (attribute != null) {

               // Because the object name is specified as "sysConfAttribute=key",
               //  parse to get the tag
               tag = objectName.getName().substring(objectName.getName().indexOf("=") + 1);
               
               table.put(tag, (String)attribute.get());                                         
            }
         } 
         
         if (context == null)
            dirContext.close();
         
         return table;
      }      
      catch (Exception exception) {
         Log.excp("NDSLib", "getSysConfData", exception.toString());                  
      }
      
      return null;
   } 
   
   /**
    * Puts the given SYS_CONF key/value pair into NDS. If an error
    *  occurs, false is returned.
    */
   public static boolean putSysConfValue(String key, String value)
   {
      return putSysConfValue(key, value, null);
   }
   
   private static boolean putSysConfValue(String key, String value, 
                                          DirContext context)
   {
      DirContext dirContext;
      String modifiedKey;
      ModificationItem modItems[];
      BasicAttributes attributes;      
      NamingEnumeration enumeration;
      DirContext tmpContext;
      
      try {
         
         // Replace all the spaces with underscore characters because LDAP 
         //  doesn't replicate keys with spaces well.
         modifiedKey = key.replace(' ', '_');
         
         // Create the initial directory context
         if (context == null) {
            
            if ((dirContext = ContextProvider.getContext()) == null) {
               Log.error("NDSLib", "putSystConfValue", "Unable to get context");
               return false;
            }
         }
         else
            dirContext = context;
                           
         // Search for the key
         attributes = new BasicAttributes("sysconfAttribute", modifiedKey);
         enumeration = dirContext.search("uid=SYSCONF", attributes);
         
         // If the key doesn't exist, create it
         if (enumeration.hasMore() == false)  {           
            
            // Create a set of attributes to give this new LDAP object
            attributes = new BasicAttributes();
            attributes.put("objectClass", "sysConf");
            attributes.put("sysconfAttribute", modifiedKey);
            attributes.put("sysconfData", value);          
         
            // Insert the new LDAP object
            tmpContext = dirContext.createSubcontext("sysConfAttribute=" + modifiedKey +              
                              ",uid=SYSCONF", attributes);        
            tmpContext.close();
         }
         else { // Update the entry
            
            enumeration.close();
            
            // Create the modification items    
            modItems = new ModificationItem[1];
            modItems[0] = new ModificationItem(DirContext.REPLACE_ATTRIBUTE,
                                     new BasicAttribute("sysconfData", value));                                              
                  
            // Update the LDAP object's attributes
            dirContext.modifyAttributes("sysConfAttribute=" + modifiedKey + 
                                        ",uid=SYSCONF", modItems); 
         }
         
         if (context == null)
            dirContext.close();
         
         return true;
      }      
      catch (Exception exception) {
         Log.excp("NDSLib", "putSysConfValue", exception.toString());                  
         return false;
      }            
   }
      
   /**
    * Refreshes the NDS SYSCONF data with the data from the local database. If
    *  no errors occur, true is returned.
    */
   public static boolean refreshSysConfData()
   {
      /*
      String fnct_name = "refreshSysConfData";
      Connection connection;
      Statement statement;
      ResultSet resultSet;
      String tag, value;     
      DirContext context;      
      NamingEnumeration objects;
      NameClassPair objectEntry; 
      BasicAttributes attributes;
      DirContext tmpContext;
       */
      
      String fnct_name = "refreshSysConfData";
      Hashtable ldapData;
      Enumeration sysConfKeys;
      Connection connection;
      Statement statement;
      ResultSet resultSet;
      String tag, value;
      Boolean deleteFlag;
      DirContext context;
      
      try {
 
/*         
         // Create the initial directory context
         if ((context = ContextProvider.getContext()) == null) {
            Log.error("NDSLib", fnct_name, "Unable to get context");
            return false;
         }
         
         // Get a database connection
         connection = DatabaseConnectionManager.getConnection();
         if (connection == null) {
            Log.error("NDSLib", fnct_name, "Received null connection");            
            return false;
         }                 
                  
         // Delete all of the LDAP objects under this context         
         objects = context.list("uid=SYSCONF");
         while (objects.hasMore() == true) {
                        
            // The entry should look like   "sysConfAttribute=key"
            objectEntry = (NameClassPair)objects.next();    
            
            context.destroySubcontext(objectEntry.getName() + ",uid=SYSCONF");                        
         }
                  
         // Get the SYSCONF data from the database
         statement = connection.createStatement();
         resultSet = statement.executeQuery("SELECT tag, value FROM sys_conf");                  
         
         // Insert the database data into NDS
         while (resultSet.next() == true) {
            
            tag = resultSet.getString("tag");
            value = resultSet.getString("value"); 
            
            // Replace all the spaces with underscore characters because keys
            //  are inserted into LDAP with this conversion
            tag = tag.replace(' ', '_');
                                                  
            // Create a set of attributes to give this new LDAP object
            attributes = new BasicAttributes();
            attributes.put("objectClass", "sysConf");
            attributes.put("sysconfAttribute", tag);
            attributes.put("sysconfData", value);          
         
            // Insert the new LDAP object
            tmpContext = context.createSubcontext("sysConfAttribute=" + tag + ",uid=SYSCONF", 
                                     attributes);                                   
            tmpContext.close();
         }
         
         // Release the database resources
         resultSet.close();
         statement.close();
         DatabaseConnectionManager.releaseConnection(connection);                  
         
         // Close the directory context
         context.close();
         
         return true;
         
*/
         
         // Get a database connection
         connection = DatabaseConnectionManager.getConnection();
         if (connection == null) {
            Log.error("NDSLib", fnct_name, "Received null connection");
            return false;
         }
         
         // Create the initial directory context
         if ((context = ContextProvider.getContext()) == null) {
            Log.error("NDSLib", fnct_name, "Unable to get context");
            return false;
         }
         
         // Store the current NDS list of sys conf keys in a structure
         sysConfKeys = getSysConfData(context).keys();
         ldapData = new Hashtable();
         while (sysConfKeys.hasMoreElements() == true)
            ldapData.put((String)sysConfKeys.nextElement(), new Boolean(true));
         
         // Get the sys conf data from the database
         statement = connection.createStatement();
         resultSet = statement.executeQuery("SELECT tag, value FROM sys_conf");                  
         
         // Insert/Update the database data into NDS
         while (resultSet.next() == true) {
            
            tag = resultSet.getString("tag");
            value = resultSet.getString("value");
            
            if (ldapData.containsKey(tag) == false) { // Insert item
                          
               if (putSysConfValue(tag, value, context) == false)
                  Log.error("NDSLib", fnct_name, "Error adding sys conf item <" + 
                            tag + ">");
            }
            else {  // Update item
               
               if (putSysConfValue(tag, value, context) == false)
                  Log.error("NDSLib", fnct_name, "Error updating sys conf item <" + 
                            tag + ">");
               
               // Update the delete flag
               ldapData.put(tag, new Boolean(false)); 
            }
         }
         
         // Release the database resources
         resultSet.close();
         statement.close();
         DatabaseConnectionManager.releaseConnection(connection);
         
         // Remove from NDS the sys conf items that aren't in the database
         sysConfKeys = ldapData.keys();
         while (sysConfKeys.hasMoreElements() == true) {
            tag = (String)sysConfKeys.nextElement();
            deleteFlag = (Boolean)ldapData.get(tag);
                  
            if (deleteFlag.booleanValue() == true) { 
               context.destroySubcontext("sysConfAttribute=" + 
                                         tag + ",uid=SYSCONF");
            }
         }
         
         // Close the directory context
         context.close();
         
         return true;       
      }
      catch (Exception exception) {
         Log.excp("NDSLib", fnct_name, exception.toString());
         return false;
      }
   }
            
   /**
    * Refreshes the NDS host data with the data from the local database. If
    *  no errors occur, true is returned.
    */
   public static boolean refreshHostData()
   {
      String fnct_name = "refreshHostData";
      Hashtable ldapData;
      Enumeration hostList;
      Connection connection;
      Statement statement;
      ResultSet resultSet;
      String hostname, IPAddress;
      Boolean deleteFlag;
      int type, maxRunningJobs;
      boolean replication;
      DirContext context;
      
      try {
         
         // Get a database connection
         connection = DatabaseConnectionManager.getConnection();
         if (connection == null) {
            Log.error("NDSLib", fnct_name, "Received null connection");
            return false;
         }
         
         // Create the initial directory context
         if ((context = ContextProvider.getContext()) == null) {
            Log.error("NDSLib", fnct_name, "Unable to get context");
            return false;
         }
         
         // Store the current NDS list of NTCSS hosts in a structure
         hostList = getNtcssHostList(context);
         ldapData = new Hashtable();
         while (hostList.hasMoreElements() == true)
            ldapData.put((String)hostList.nextElement(), new Boolean(true));
         
         // Get the host data from the database
         statement = connection.createStatement();
         resultSet = statement.executeQuery("SELECT hostname, host_ip_address,"
                      + " type, replication_ind, max_running_jobs FROM hosts");                  
         
         // Insert/Update the database data into NDS
         while (resultSet.next() == true) {
            
            hostname = resultSet.getString("hostname");
            IPAddress = resultSet.getString("host_ip_address");
            type = resultSet.getInt("type");
            replication = resultSet.getBoolean("replication_ind");
            maxRunningJobs = resultSet.getInt("max_running_jobs");
            
            if (ldapData.containsKey(hostname) == false) { // Insert host
                          
               if (insertHostEntry(hostname, IPAddress, type, replication, 
                                   maxRunningJobs, context) == false)
                  Log.error("NDSLib", fnct_name, "Error adding host <" + 
                            hostname + ">");
            }
            else {  // Update host
               
               if (updateHostEntry(hostname, IPAddress, type, replication, 
                                   maxRunningJobs, context) == false)
                  Log.error("NDSLib", fnct_name, "Error updating host <" + 
                            hostname + ">");
               
               // Update the delete flag
               ldapData.put(hostname, new Boolean(false)); 
            }
         }
         
         // Release the database resources
         resultSet.close();
         statement.close();
         DatabaseConnectionManager.releaseConnection(connection);
         
         // Remove from NDS the hosts that aren't in the database
         hostList = ldapData.keys();
         while (hostList.hasMoreElements() == true) {
            hostname = (String)hostList.nextElement();
            deleteFlag = (Boolean)ldapData.get(hostname);
                  
            if (deleteFlag.booleanValue() == true) {     
               if (deleteHostEntry(hostname, context) == false)
                  Log.error("NDSLib", fnct_name, "Error deleting host <" + 
                            hostname + ">");
            }
         }
         
         // Close the directory context
         context.close();
         
         return true;
      }
      catch (Exception exception) {
         Log.excp("NDSLib", fnct_name, exception.toString());
         return false;
      }
   }
   
   /**
    * Inserts the given host information into NDS. Assumes entry doesn't
    *  already exist.
    */
   private static boolean insertHostEntry(String hostname, String IP, int type,    
                                          boolean replication, 
                                          int max_running_jobs, 
                                          DirContext context)
   {
      String fnct_name = "insertHostEntry";
      DirContext dirContext;
      BasicAttributes attributes;
      DirContext tmpContext;
      
      try {
         
         // Create a set of attributes to give this new LDAP object
         attributes = new BasicAttributes();
         attributes.put("objectClass", "cssHosts");
         attributes.put("cssServerName", hostname);
         attributes.put("cssServerIP", IP);
         attributes.put("cssServerType", String.valueOf(type));
         attributes.put("cssReplicationStatus", replication ? "1" : "0");
         attributes.put("cssServerMaxBatch", String.valueOf(max_running_jobs));
         
         // Create the initial directory context
         if (context == null)
            dirContext = ContextProvider.getContext();                                            
         else
            dirContext = context;                  
         
         if (dirContext == null) {
            Log.error("NDSLib", fnct_name, "Unable to get context");
            return false;
         }
         
         // Insert the new LDAP object
         tmpContext = dirContext.createSubcontext("cssServerName=" + hostname + ",uid=HOSTS", 
                                     attributes);
         tmpContext.close();
         
         if (context == null)
            dirContext.close();         
         
         return true;
      }
      catch (Exception exception) {
         Log.excp("NDSLib", fnct_name, exception.toString());
         return false;
      }
   }
      
   /**
    * Updates the given host information in NDS. Assumes the entry exists.
    */
   private static boolean updateHostEntry(String hostname, String IP, int type, 
                                          boolean replication, 
                                          int max_running_jobs,
                                          DirContext context)
   {
      String fnct_name = "updateHostEntry";
      DirContext dirContext;      
      ModificationItem modItems[];
      
      try {
         
         // Create the initial directory context
         if (context == null)
            dirContext = ContextProvider.getContext();  
         else 
            dirContext = context;
         
         if (dirContext == null) {
            Log.error("NDSLib", fnct_name, "Unable to get context");
            return false;
         }
         
         // Create a set of modification items
         modItems = new ModificationItem[4];
         modItems[0] = new ModificationItem(DirContext.REPLACE_ATTRIBUTE,
                                     new BasicAttribute("cssServerIP", IP));
         modItems[1] = new ModificationItem(DirContext.REPLACE_ATTRIBUTE,
                                     new BasicAttribute("cssServerType", 
                                     String.valueOf(type)));
         modItems[2] = new ModificationItem(DirContext.REPLACE_ATTRIBUTE,
                                     new BasicAttribute("cssReplicationStatus", 
                                     replication ? "1" : "0"));
         modItems[3] = new ModificationItem(DirContext.REPLACE_ATTRIBUTE,
                                     new BasicAttribute("cssServerMaxBatch", 
                                     String.valueOf(max_running_jobs)));                                     
                  
         // Update the LDAP object's attributes
         dirContext.modifyAttributes("cssServerName=" + hostname + ",uid=HOSTS", 
                                     modItems);      
         
         if (context == null)
            dirContext.close();                  
         
         return true;
      }
      catch (Exception exception) {
         Log.excp("NDSLib", fnct_name, exception.toString());
         return false;
      }
   }
   
   /**
    * Updates the given host's NTCSS host type in NDS. Assumes the entry exists.
    */
   public static boolean modifyHostType(String hostname, int type)
   {
      String fnct_name = "modifyHostType";
      DirContext dirContext;      
      ModificationItem modItems[];
      
      try {
         
         // Create the initial directory context         
         if ((dirContext = ContextProvider.getContext()) == null) {
            Log.error("NDSLib", fnct_name, "Unable to get context");
            return false;
         }
         
         // Create a set of modification items
         modItems = new ModificationItem[1];
         modItems[0] = new ModificationItem(DirContext.REPLACE_ATTRIBUTE,
                                     new BasicAttribute("cssServerType", 
                                     String.valueOf(type)));
                  
         // Update the LDAP object's attributes
         dirContext.modifyAttributes("cssServerName=" + hostname + ",uid=HOSTS", 
                                     modItems);      
                  
         dirContext.close();                  
         
         return true;
      }
      catch (Exception exception) {
         Log.excp("NDSLib", fnct_name, exception.toString());
         return false;
      }
   }
   
   /**
    * Updates the given host's process limit in NDS. Assumes the entry exists.
    */
   public static boolean modifyHostProcessLimit(String hostname, int limit)
   {
      String fnct_name = "modifyHostProcessLimit";
      DirContext dirContext;      
      ModificationItem modItems[];
      
      try {
         
         // Create the initial directory context         
         if ((dirContext = ContextProvider.getContext()) == null) {
            Log.error("NDSLib", fnct_name, "Unable to get context");
            return false;
         }
         
         // Create a set of modification items
         modItems = new ModificationItem[1];
         modItems[0] = new ModificationItem(DirContext.REPLACE_ATTRIBUTE,
                                     new BasicAttribute("cssServerMaxBatch", 
                                     String.valueOf(limit)));
                  
         // Update the LDAP object's attributes
         dirContext.modifyAttributes("cssServerName=" + hostname + ",uid=HOSTS", 
                                     modItems);      
                  
         dirContext.close();                  
         
         return true;
      }
      catch (Exception exception) {
         Log.excp("NDSLib", fnct_name, exception.toString());
         return false;
      }
   }
   
   /**
    * Deletes the given host information from NDS. Assumes the entry exists.
    */
   private static boolean deleteHostEntry(String hostname, DirContext context)
   {
      String fnct_name = "deleteHostEntry";
      DirContext dirContext;                 
      
      try {
             
         // Create the initial directory context
         if (context == null)
            dirContext = ContextProvider.getContext();  
         else
            dirContext = context;     
         
         if (dirContext == null) {
            Log.error("NDSLib", fnct_name, "Unable to get context");
            return false;
         }
         
         // Removes the LDAP object          
         dirContext.destroySubcontext("cssServerName=" + hostname + ",uid=HOSTS");                  
         
         if (context == null)
            dirContext.close();
         
         return true;
      }
      catch (Exception exception) {
         Log.excp("NDSLib", fnct_name, exception.toString());
         return false;
      }
   }
   
   /**
    * Retrieves the NTCSS master hostname from NDS and stores the result in
    *  the given buffer.
    * Returns 1 if current host is the master, 0 if not, negative value if error.    
    */
   public static int getMasterServer(StringBuffer masterName)
   {
      DirContext dirContext;
      BasicAttributes attributes;
      NamingEnumeration enumeration;      
      int ret;
      StringTokenizer tokenizer;
      
      try {
         
         // Create the initial directory context
         if ((dirContext = ContextProvider.getContext()) == null) {
            Log.error("NDSLib", "getMasterServer", "Unable to get context");
            return ErrorCodes.ERROR_CANT_DETER_MASTER;
         }
         
         // Search for the host with the server type of "NTCSS_MASTER"
         attributes = new BasicAttributes("cssServerType", 
                                          new Integer(Constants.NTCSS_MASTER));
         enumeration = dirContext.search("uid=HOSTS", attributes);
         
         // Atleast one record should be returned
         if (enumeration.hasMore() == true) {

            // The result is of the form "cssServerName=host"
            tokenizer = new StringTokenizer(
                            ((SearchResult)enumeration.next()).getName(), "="); 
            tokenizer.nextToken();  // skip "cssServerName"
            
            // Store the name of the master in the given buffer
            masterName.setLength(0);
            masterName.append(tokenizer.nextToken());
            
            // See if this local host is the master
            if (masterName.toString().equalsIgnoreCase(StringLib.getHostname()) == true)
               ret = 1;
            else
               ret = 0;
            
            enumeration.close();
         }
         else 
            ret = ErrorCodes.ERROR_CANT_DETER_MASTER;         
         
         // Close the directory context
         dirContext.close();
         
         return ret;
      }      
      catch (Exception exception) {
         Log.excp("NDSLib", "getMasterServer", exception.toString());                  
         return ErrorCodes.ERROR_CANT_DETER_MASTER;
      }            
   }
   
   /**
    * Checks to make sure a connection to a NDS server is possible. If a 
    *  connection can be made, true is returned, otherwise false is returned.
    */
   public static boolean checkConnection()
   {
      String fnct_name = "checkConnection";
      DirContext dirContext;
      final int numConnectionAttempts = 3;
      
      try {
       
         // Try several times to make a connection until one is made, or the
         //  number of attempts has exceeded numConnectionAttempts
         for (int i = 0; i < numConnectionAttempts; i++) {
            
            // Try to get a connection
            if ((dirContext = ContextProvider.getContext()) != null) {
               dirContext.close();
               return true;
            }
            
            // Pause for a period of time before trying to connect again
            if (i != (numConnectionAttempts - 1)) { // No need to sleep after 
               Thread.sleep(10000);                 //  last connection attempt
               ContextProvider.createServerList();
            }
         }
         
         // Must not have been able to make a connection
         return false;
      }
      catch (Exception exception) {
         Log.excp("NDSLib", fnct_name, exception.toString());
         return false;
      }
   }
   
   /**
    * Re-configures the ContextProvider with the latest NDS server information.
    */
   public static void refreshContextInfo()
   {
      ContextProvider.createServerList();
   }
   
   /**
    * Adds the given printer to NDS. Returns true if successful
    */
   public static boolean addPrinter(String printerName, String location, 
                      int maxSize, String printerType, int securityClass,
                      boolean suspendRequests, String redirectionPrinter, 
                      String driverName, ArrayList appList, String printingMethod, 
                      String hostIPAddress, String portName, String filename, 
                      String adapterIPAddress, String ntHostname, 
                      String ntShareName, boolean queueEnabled, 
                      boolean printerEnabled, boolean generalAccess, boolean fileAppend)
   {
      DirContext dirContext;           
      BasicAttributes attributes;  
      DirContext tmpContext;
      int status = 0;
      String appListString = null;
      
      try {
                          
         // Create the LDAP entry
         attributes = new BasicAttributes();
         attributes.put("objectClass", "cssPrinter");
         attributes.put("printerName", printerName);              
         attributes.put("printerSuspendFlag", suspendRequests ? "1" : "0");
         attributes.put("printerSecurityClass", String.valueOf(securityClass));         
         attributes.put("printerMaxBytes", String.valueOf(maxSize));
         attributes.put("printerLocation", location);
         attributes.put("printerInterfaceFile", printerType);
         attributes.put("printerHostName", "NODATA");
         
         // If any of these fields are empty, set their values to "None" in NDS         
         attributes.put("printerNTDriver", ((driverName == null) ||
                                            (driverName.length() == 0)) ? "None" : driverName);
         attributes.put("printerRedirectedPrinter", 
                        ((redirectionPrinter == null) || 
                         (redirectionPrinter.trim().length() == 0)) ? "None" : redirectionPrinter);
         if ((appList == null) || (appList.size() == 0))
            appListString = "None";            
         else {
            
            // Create a comma delimited list of applications
            for (int i = 0; i < appList.size(); i++) {
               if (appListString == null)
                  appListString = (String)appList.get(i);
               else
                  appListString += "," + (String)appList.get(i);
            }
         }
         attributes.put("printerApps", appListString);
         
         if (queueEnabled == true)
            status |= 0x1;
         if (printerEnabled == true)
            status |= 0x2;
         if (generalAccess == true)
            status |= 0x4;
         if (fileAppend == true)
            status |= 0x8;         
         attributes.put("printerStatus", String.valueOf(status));                                    
                  
         if (printingMethod.equals(Constants.REGULAR_PRINTER) == true) {
            attributes.put("printerIP", adapterIPAddress);         
            attributes.put("printerPort", portName);                                                
            attributes.put("printerFilename", "None");
         }
         else if (printingMethod.equals(Constants.FILE_PRINTER) == true) {
            attributes.put("printerIP", "None");         
            attributes.put("printerPort", "FILE");                                                
            attributes.put("printerFilename", filename);
         }
         else if (printingMethod.equals(Constants.UNIX_PRINTER) == true) {
            attributes.put("printerIP", hostIPAddress);         
            attributes.put("printerPort", portName);                                                
            attributes.put("printerFilename", "None");
         }
         else if (printingMethod.equals(Constants.NT_SHARE_PRINTER) == true) {
            attributes.put("printerIP", ntHostname);         
            attributes.put("printerPort", ntShareName);                                                
            attributes.put("printerFilename", "None");
         }
         else {
            Log.error("NDSLib", "addPrinter", "Unknown printing method <" + 
                      printingMethod + ">");
            return false;
         }
            
         // Create the initial directory context                                  
         if ((dirContext = ContextProvider.getContext()) == null) {
            Log.error("NDSLib", "addPrinter", "Unable to get context");
            return false;
         }
         
         // Add the new LDAP entry
         tmpContext = dirContext.createSubcontext("printerName=" + printerName + ",uid=PRINTERS", 
                                     attributes);
         tmpContext.close();
         
         // Close the directory context
         dirContext.close();
         
         return true;
      }
      catch (Exception exception) {
         Log.excp("NDSLib", "addPrinter", exception.toString());
         return false;
      }
   }
   
   /**
    * Modifies the given printer's data in NDS with the given values. If 
    *  an error occurs, false is returned.
    */
   public static boolean editPrinter(String printerName, String location, 
                      int maxSize, String printerType, int securityClass,
                      boolean suspendRequests, String redirectionPrinter, 
                      String driverName, ArrayList appList, String printingMethod, 
                      String hostIPAddress, String portName, String filename, 
                      String adapterIPAddress, String ntHostname, 
                      String ntShareName, boolean queueEnabled, 
                      boolean printerEnabled, boolean generalAccess, 
                      boolean fileAppend)
   {  
      DirContext dirContext;            
      ModificationItem modItems[];
      int status;
      String IP, port, filenameStr;
      String appListString = null;
      
      try {                  
           
         // Create a structure describing the change
         modItems = new ModificationItem[12];
         modItems[0] = new ModificationItem(DirContext.REPLACE_ATTRIBUTE,
                                      new BasicAttribute("printerLocation", location));
         modItems[1] = new ModificationItem(DirContext.REPLACE_ATTRIBUTE,
                                      new BasicAttribute("printerMaxBytes", String.valueOf(maxSize)));
         modItems[2] = new ModificationItem(DirContext.REPLACE_ATTRIBUTE,
                                      new BasicAttribute("printerInterfaceFile", printerType));
         modItems[3] = new ModificationItem(DirContext.REPLACE_ATTRIBUTE,
                                      new BasicAttribute("printerSecurityClass", String.valueOf(securityClass)));
         modItems[4] = new ModificationItem(DirContext.REPLACE_ATTRIBUTE,
                                      new BasicAttribute("printerSuspendFlag", suspendRequests ? "1" : "0"));
         
         if (printingMethod.equals(Constants.REGULAR_PRINTER) == true) {
            IP          = adapterIPAddress;
            port        = portName;                                                
            filenameStr = "None";
         }
         else if (printingMethod.equals(Constants.FILE_PRINTER) == true) {
            IP          = "None";         
            port        = "FILE";                                                
            filenameStr = filename;
         }
         else if (printingMethod.equals(Constants.UNIX_PRINTER) == true) {
            IP          = hostIPAddress;         
            port        = portName;                                                
            filenameStr = "None";
         }
         else if (printingMethod.equals(Constants.NT_SHARE_PRINTER) == true) {
            IP          = ntHostname;         
            port        = ntShareName;                                                
            filenameStr = "None";
         }
         else {
            Log.error("NDSLib", "editPrinter", "Unknown printing method <" + 
                      printingMethod + ">");
            return false;
         }
                   
         modItems[5] = new ModificationItem(DirContext.REPLACE_ATTRIBUTE,
                                      new BasicAttribute("printerIP", IP));                                      
         modItems[6] = new ModificationItem(DirContext.REPLACE_ATTRIBUTE,
                                      new BasicAttribute("printerPort", port));                                                                                     
         modItems[7] = new ModificationItem(DirContext.REPLACE_ATTRIBUTE,
                                      new BasicAttribute("printerFilename", filenameStr));
         
         status = queueEnabled ? 0x1 : 0;
         status |= printerEnabled ? 0x2 : 0;
         status |= generalAccess ? 0x4 : 0;
         status |= fileAppend ? 0x8 : 0;
         modItems[8] = new ModificationItem(DirContext.REPLACE_ATTRIBUTE,
                                      new BasicAttribute("printerStatus", String.valueOf(status)));
         
         // If any of these fields are empty, set their values to "None" in NDS         
         modItems[9] = new ModificationItem(DirContext.REPLACE_ATTRIBUTE,
                 new BasicAttribute("printerNTDriver", ((driverName == null) ||
                                            (driverName.length() == 0)) ? "None" : driverName));
         modItems[10] = new ModificationItem(DirContext.REPLACE_ATTRIBUTE,
                 new BasicAttribute("printerRedirectedPrinter", 
                        ((redirectionPrinter == null) || 
                         (redirectionPrinter.trim().length() == 0)) ? "None" : redirectionPrinter));
         if ((appList == null) || (appList.size() == 0))
            appListString = "None";            
         else {
            
            // Create a comma delimited list of applications
            for (int i = 0; i < appList.size(); i++) {
               if (appListString == null)
                  appListString = (String)appList.get(i);
               else
                  appListString += "," + (String)appList.get(i);
            }
         }
         modItems[11] = new ModificationItem(DirContext.REPLACE_ATTRIBUTE,
                 new BasicAttribute("printerApps", appListString));         
  
         
         // Create the initial directory context
         if ((dirContext = ContextProvider.getContext()) == null) {
            Log.error("NDSLib", "editPrinter", "Unable to get context");
            return false;
         }
         
         // Change the attributes
         dirContext.modifyAttributes("printerName=" + printerName + ",uid=PRINTERS", 
                                     modItems);
         
         // Close the directory context
         dirContext.close();
         
         return true;
      }
      catch (NameNotFoundException notFound) {
         Log.error("NDSLib", "editPrinter", "Value for " + printerName + 
                   " not found");         
      }
      catch (Exception exception) {
         Log.excp("NDSLib", "editPrinter", exception.toString());                  
      }
      
      return false;
   }
   
   /**
    * Removes the given printer from NDS. Returns true if successful
    */
   public static boolean deletePrinter(String printerName)
   {
      DirContext dirContext;                 
      
      try {
             
         // Create the initial directory context
         if ((dirContext = ContextProvider.getContext()) == null) {
            Log.error("NDSLib", "deletePrinter", "Unable to get context");
            return false;
         }
         
         // Remove the LDAP object          
         dirContext.destroySubcontext("printerName=" + printerName + ",uid=PRINTERS");                  
                  
         dirContext.close();
         
         return true;
      }
      catch (Exception exception) {
         Log.excp("NDSLib", "deletePrinter", exception.toString());
         return false;
      }      
   }
   
   /**
    * Returns a list of printers from NDS. If an error occurs, null is returned.
    */
   public static Hashtable getPrinters()
   {
      DirContext dirContext;   
      PrinterStruct struct;
      Hashtable list;
      Attributes attributes;
      NamingEnumeration objects; 
      String ipAddress, portName, fileName, apps;
      int status;
      StringTokenizer tokenizer;
      
      try {
             
         // Create the initial directory context
         if ((dirContext = ContextProvider.getContext()) == null) {
            Log.error("NDSLib", "getPrinters", "Unable to get context");
            return null;
         }
         
         list = new Hashtable();
         
         // Get a list of the LDAP objects under this context         
         objects = dirContext.list("uid=PRINTERS");
         while (objects.hasMore() == true) {                                                     
            
            // Get the attributes of this object            
            attributes = dirContext.getAttributes(((NameClassPair)objects.next()).getName() + 
                                                  ", uid=PRINTERS"); 
            
            struct = new PrinterStruct();            
            struct.printerName = (String)attributes.get("printerName").get();
            struct.location = (String)attributes.get("printerLocation").get();
            struct.jobMaxBytes = Integer.parseInt((String)attributes.get("printerMaxBytes").get());
            struct.printerType = (String)attributes.get("printerInterfaceFile").get();
            struct.securityClass = Integer.parseInt((String)attributes.get("printerSecurityClass").get());
            struct.redirectionPtr = (String)attributes.get("printerRedirectedPrinter").get();
            if (struct.redirectionPtr.equals("None") == true)
               struct.redirectionPtr = null;            
            struct.suspendRequests = ((String)attributes.get("printerSuspendFlag").get()).equals("1") ? true : false;            
            struct.driverName = (String)attributes.get("printerNTDriver").get();         
            if (struct.driverName.equals("None") == true)
               struct.driverName = null;
            
            // Data in "printerHostName" attribute not used
                                    
            struct.appList = new ArrayList();
            apps = (String)attributes.get("printerApps").get();            
            if (apps.equals("None") == false) {
               tokenizer = new StringTokenizer(apps, ",");
               while (tokenizer.hasMoreTokens() == true)
                  struct.appList.add(tokenizer.nextToken());
            }
            
            status = Integer.parseInt((String)attributes.get("printerStatus").get());
            struct.queueEnabled = ((status & 0x1) == 0x1) ? true : false;               
            struct.printerEnabled = ((status & 0x2) == 0x2) ? true : false;
            struct.generalAccess = ((status & 0x4) == 0x4) ? true : false;
            struct.fileAppendFlag = ((status & 0x8) == 0x8) ? true : false;
            
            ipAddress = (String)attributes.get("printerIP").get();
            portName = (String)attributes.get("printerPort").get();
            fileName = (String)attributes.get("printerFilename").get();            
            
            if (ipAddress.equals("None") == true) {   // FILE type
               struct.printingMethod = Constants.FILE_PRINTER;
               struct.hostIPAddress = null;         
               struct.portName = null;
               struct.fileName = fileName;
               struct.adapterIPAddress = null;   
               struct.ntHostName = null;
               struct.ntShareName = null; 
            }   
            
            // If a REGULAR type, port name will be P# or S#
            else if (((portName.charAt(0) == 'P') || (portName.charAt(0) == 'S')) &&
                     (StringLib.parseInt(portName.substring(1, portName.length()), -1) != -1)) {
               struct.printingMethod = Constants.REGULAR_PRINTER;
               struct.hostIPAddress = null;         
               struct.portName = portName;
               struct.fileName = null;
               struct.adapterIPAddress = ipAddress;   
               struct.ntHostName = null;
               struct.ntShareName = null;                         
            }
            
            // If a SERVER type ipAddress will contain an IP address
            else if (ipAddress.indexOf('.') != -1) {  
               struct.printingMethod = Constants.UNIX_PRINTER;
               struct.hostIPAddress = ipAddress;         
               struct.portName = portName;
               struct.fileName = null;
               struct.adapterIPAddress = null;   
               struct.ntHostName = null;
               struct.ntShareName = null;                                        
            }
            else {  // Assume NTSHARE type
               struct.printingMethod = Constants.NT_SHARE_PRINTER;
               struct.hostIPAddress = null;         
               struct.portName = null;
               struct.fileName = null;
               struct.adapterIPAddress = null;   
               struct.ntHostName = ipAddress;
               struct.ntShareName = portName;                                                       
            }
                  
            list.put(struct.printerName, struct);   
         }           
                  
         dirContext.close();
         
         return list;
      }
      catch (Exception exception) {
         Log.excp("NDSLib", "getPrinters", exception.toString());
         return null;
      }      
   }

   
   /**
    * Adds the given adapter to NDS. Returns true if successful
    */
   public static boolean addAdapter(String adapterIP, String adapterHost,
                                    String adapterType, String ethernetAddress, 
                                    String location)
   {
      DirContext dirContext;           
      BasicAttributes attributes;  
      DirContext tmpContext;
      
      try {
         
         // Create the initial directory context                                  
         if ((dirContext = ContextProvider.getContext()) == null) {
            Log.error("NDSLib", "addAdapter", "Unable to get context");
            return false;
         }
         
         // Create the LDAP entry
         attributes = new BasicAttributes();
         attributes.put("objectClass", "adapters");
         attributes.put("adapterIP", adapterIP);
         attributes.put("adapterLocation", location);
         attributes.put("adapterMAC", ethernetAddress);
         attributes.put("adapterType", adapterType);                                                                   
         attributes.put("adapterName", adapterHost);
         
         // Add the new LDAP entry
         tmpContext = dirContext.createSubcontext("adapterIP=" + adapterIP + ",uid=ADAPTERS", 
                                     attributes);
         tmpContext.close();
         
         // Close the directory context
         dirContext.close();
         
         return true;
      }
      catch (Exception exception) {
         Log.excp("NDSLib", "addAdapter", exception.toString());
         return false;
      }
   }
   
   /**
    * Modifies the given adapter's data in NDS. Returns true if successful
    */
   public static boolean editAdapter(String adapterIP, String adapterHost,
                                     String adapterType, String ethernetAddress, 
                                     String location)
   {
      DirContext dirContext;  
      ModificationItem modItems[];
      
      try {
         
         // Create the initial directory context                                  
         if ((dirContext = ContextProvider.getContext()) == null) {
            Log.error("NDSLib", "addAdapter", "Unable to get context");
            return false;
         }
         
         modItems = new ModificationItem[4];
         modItems[0] = new ModificationItem(DirContext.REPLACE_ATTRIBUTE,
                                      new BasicAttribute("adapterLocation", location));
         modItems[1] = new ModificationItem(DirContext.REPLACE_ATTRIBUTE,
                                      new BasicAttribute("adapterMAC", ethernetAddress));
         modItems[2] = new ModificationItem(DirContext.REPLACE_ATTRIBUTE,
                                      new BasicAttribute("adapterType", adapterType));
         modItems[3] = new ModificationItem(DirContext.REPLACE_ATTRIBUTE,
                                      new BasicAttribute("adapterName", adapterHost));
                  
         // Change the attributes
         dirContext.modifyAttributes("adapterIP=" + adapterIP + ",uid=ADAPTERS", 
                                     modItems);
         
         // Close the directory context
         dirContext.close();
         
         return true;
      }
      catch (Exception exception) {
         Log.excp("NDSLib", "editAdapter", exception.toString());
         return false;
      }
   }
   
   /**
    * Removes the given adapter from NDS. Returns true if successful
    */
   public static boolean deleteAdapter(String adapterIP)
   {
      DirContext dirContext;                 
      
      try {
             
         // Create the initial directory context
         if ((dirContext = ContextProvider.getContext()) == null) {
            Log.error("NDSLib", "deleteAdapter", "Unable to get context");
            return false;
         }
         
         // Remove the LDAP object          
         dirContext.destroySubcontext("adapterIP=" + adapterIP + ",uid=ADAPTERS");                  
                  
         dirContext.close();
         
         return true;
      }
      catch (Exception exception) {
         Log.excp("NDSLib", "deleteAdapter", exception.toString());
         return false;
      }      
   }
   
   /**
    * Returns a list of adapters from NDS. If an error occurs, null is returned.
    */
   public static Hashtable getAdapters()
   {
      DirContext dirContext;   
      AdapterStruct struct;
      Hashtable list;
      Attributes attributes;
      NamingEnumeration objects;
      
      try {
             
         // Create the initial directory context
         if ((dirContext = ContextProvider.getContext()) == null) {
            Log.error("NDSLib", "getAdapters", "Unable to get context");
            return null;
         }
         
         list = new Hashtable();
         
         // Get a list of the LDAP objects under this context         
         objects = dirContext.list("uid=ADAPTERS");
         while (objects.hasMore() == true) {                                                     
            
            // Get the attributes of this object            
            attributes = dirContext.getAttributes(((NameClassPair)objects.next()).getName() + 
                                                  ", uid=ADAPTERS"); 
            
            struct = new AdapterStruct();
            struct.adapterIPAddress = (String)attributes.get("adapterIP").get();
            struct.ethernetAddress = (String)attributes.get("adapterMAC").get();
            struct.type = (String)attributes.get("adapterType").get();
            struct.location = (String)attributes.get("adapterLocation").get();            
            struct.host = (String)attributes.get("adapterName").get();            
               
            list.put(struct.adapterIPAddress, struct);
         }           
                  
         dirContext.close();
         
         return list;
      }
      catch (Exception exception) {
         Log.excp("NDSLib", "getAdapters", exception.toString());
         return null;
      }      
   }
   
   /**
    * Returns the given adapter's hostname from NDS. If an error
    *  occurs, null is returned.
    */
   public static String getAdapterHost(String adapterIP)
   {  
      DirContext dirContext;
      Attributes attributes;
      Attribute attribute;
      String value = null;
      
      try {
         
         // Create the initial directory context
         if ((dirContext = ContextProvider.getContext()) == null) {
            Log.error("NDSLib", "getAdapterHost", "Unable to get context");
            return null;
         }
                           
         // Get the desired adapter's attribute
         try {
            attributes = dirContext.getAttributes("adapterIP=" + adapterIP + 
                                               ", uid=ADAPTERS");
            attribute = attributes.get("adapterName");
            
            value = (String)attribute.get();
         }
         catch (NameNotFoundException notFound) {
            Log.error("NDSLib", "getAdapterHost", "Value for " + adapterIP + 
                   " not found");
         }
         
         dirContext.close();
         
         return value;
      }
      catch (Exception exception) {
         Log.excp("NDSLib", "getAdapterHost", exception.toString());                  
      }
      
      return null;
   }
   
   /**
    * Adds the given user to the given group. If the group doesn't exist,
    *  it is created. True is returned if there are no errors. If the user
    *  already belongs to the group, they are not re-added.
    */
   public static boolean addUserToGroup(String user, String group)
   {
      DirContext dirContext;
      Attributes attributes = null;
      Attribute attribute;
      String userList;
      boolean groupExists;
      ModificationItem modItems[];
      DirContext tmpContext;
      StringTokenizer tokenizer;
      boolean alreadyMember;
      
      try {
         
         // Create the initial directory context
         if ((dirContext = ContextProvider.getContext()) == null) {
            Log.error("NDSLib", "addUserToGroup", "Unable to get context");
            return false;
         }
                           
         // Get the desired group's user list, if it exists
         try {
            attributes = dirContext.getAttributes("groupName=" + group + 
                                                  ", uid=GROUPS");
            groupExists = true;
         }
         catch (NameNotFoundException notFound) {            
            groupExists = false;
         }
         
         // If the group doesn't exist, then a new group entry needs to be
         //  created. Otherwise the group entry is just updated.
         if (groupExists == false) {
            
            // Create the LDAP entry
            attributes = new BasicAttributes();
            attributes.put("groupName", group);
            attributes.put("uidString", user);
            attributes.put("objectClass", "applicationGroup");
            
         
            // Add the new LDAP entry
            tmpContext = dirContext.createSubcontext("groupName=" + 
                                     group + ",uid=GROUPS", attributes);
            tmpContext.close();
         }
         else {
            
            // Get the old user list
            attribute = attributes.get("uidString");            
            userList = (String)attribute.get();
            
            // If the user already exists, don't re-add them
            tokenizer = new StringTokenizer(userList, ",");
            alreadyMember = false;
            while (tokenizer.hasMoreTokens() == true) {
               if (user.equals(tokenizer.nextToken()) == true)  {
                  alreadyMember = true;
                  break;
               }
            }
            
            // If the user isn't alreay in the group, add them
            if (alreadyMember == false) {
               
               // Create a set of modification items
               modItems = new ModificationItem[1];
               modItems[0] = new ModificationItem(DirContext.REPLACE_ATTRIBUTE,
                                     new BasicAttribute("uidString", 
                                     userList + "," + user));
                  
               // Update the LDAP object's attributes
               dirContext.modifyAttributes("groupName=" + group + ",uid=GROUPS", 
                                        modItems);  
            }
         }
         
         dirContext.close();
         
         return true;
      }
      catch (Exception exception) {
         Log.excp("NDSLib", "addUserToGroup", exception.toString());
         return false;
      }
   }
   
   /**
    * Removes the given user from the given group. True is returned if there 
    *  are no errors. After the user is removed from the group, if the group
    *  is now empty, it is removed.
    */
   public static boolean removeUserFromGroup(String user, String group)
   {
      DirContext dirContext;
      Attributes attributes;
      Attribute attribute;
      StringBuffer userList;
      ModificationItem modItems[];
      StringTokenizer tokenizer;
      String currentUser;
      
      try {
         
         // Create the initial directory context
         if ((dirContext = ContextProvider.getContext()) == null) {
            Log.error("NDSLib", "removeUserFromGroup", "Unable to get context");
            return false;
         }
                           
         // Get the desired group's user list       
         attributes = dirContext.getAttributes("groupName=" + group + 
                                               ", uid=GROUPS"); 
         attribute = attributes.get("uidString"); 
         
         // Create a new user list that doesn't have the given user in it
         tokenizer = new StringTokenizer((String)attribute.get(), ",");
         userList = new StringBuffer();
         while (tokenizer.hasMoreTokens() == true) {
            currentUser = tokenizer.nextToken();
            if (currentUser.equals(user) == false) {
               if (userList.length() > 0)
                  userList.append(",");
               userList.append(currentUser);
            }
         }         
         
         // If the new user list is empty, just remove the group
         if (userList.length() != 0) {
            // Create a set of modification items
            modItems = new ModificationItem[1];
            modItems[0] = new ModificationItem(DirContext.REPLACE_ATTRIBUTE,
                                     new BasicAttribute("uidString", 
                                     userList.toString()));
                  
            // Update the LDAP object's attributes
            dirContext.modifyAttributes("groupName=" + group + ",uid=GROUPS", 
                                        modItems);           
         }
         else   // Remove the LDAP object          
            dirContext.destroySubcontext("groupName=" + group + ",uid=GROUPS");          
         
         dirContext.close();
         
         return true;
      }
      catch (Exception exception) {
         Log.excp("NDSLib", "removeUserFromGroup", exception.toString());
         return false;
      }
   }

   /**
    * Returns a list of NTCSS hosts from NDS
    */
   private static Enumeration getNtcssHostList(DirContext dirContext)
   {
      String fnct_name = "getNtcssHostList";      
      NamingEnumeration objects;
      NameClassPair objectEntry;      
      Attributes attributes;
      Attribute attribute;
      String objectName;
      StringTokenizer tokenizer;
      Vector list;
      
      try {
         list = new Vector();
         
         // Get a list of the LDAP objects under this context         
         objects = dirContext.list("uid=HOSTS");
         while (objects.hasMore() == true) {
                        
            // The entry should look like   "cssServerName= hostname"
            objectEntry = (NameClassPair)objects.next();    
            
            tokenizer = new StringTokenizer(objectEntry.getName(), "=");

            objectName = tokenizer.nextToken();
            if (objectName.equals("cssServerName") == true)
               list.add(tokenizer.nextToken());
         }                  
         
         return list.elements();
      }
      catch (Exception exception) {
         Log.excp("NDSLib", fnct_name, exception.toString());
         return null;
      }
   }
                  
   /**
    * Returns the given host's attribute from NDS. If an error
    *  occurs, null is returned.
    */
   private static Object getHostAttribute(String hostname, String attributeName)
   {
      DirContext dirContext;
      Attributes attributes;
      Attribute attribute;
      
      try {
         
         // Create the initial directory context
         if ((dirContext = ContextProvider.getContext()) == null) {
            Log.error("NDSLib", "getHostAttribute", "Unable to get context");
            return null;
         }
                           
         // Get the desired host's attribute
         attributes = dirContext.getAttributes("cssServerName=" + hostname + 
                                               ", uid=HOSTS");
         attribute = attributes.get(attributeName);
         
         dirContext.close();
         
         return attribute.get();
      }
      catch (NameNotFoundException notFound) {
         Log.error("NDSLib", "getHostAttribute", "Value for " + hostname + 
                   " not found");         
      }
      catch (Exception exception) {
         Log.excp("NDSLib", "getHostAttribute", exception.toString());                  
      }
      
      return null;
   }
   
   /**
    * Returns the given user's attribute from NDS. If an error
    *  occurs, null is returned.
    */
   private static Object getAuthUserAttribute(String loginname, 
                                              String attributeName)
   {  
      DirContext dirContext;
      Attributes attributes;
      Attribute attribute;
      Object value = null;
      
      try {
         
         // Create the initial directory context
         if ((dirContext = ContextProvider.getContext()) == null) {
            Log.error("NDSLib", "getAuthUserAttribute", "Unable to get context");
            return null;
         }
                           
         // Get the desired user's attribute
         try {
            attributes = dirContext.getAttributes("uid=" + loginname + 
                                               ", uid=AUTHSERVERS");
            attribute = attributes.get(attributeName);
            
            value = attribute.get();
         }
         catch (NameNotFoundException notFound) {
            Log.error("NDSLib", "getAuthUserAttribute", "Value for " + loginname + 
                   " not found");
         }
         
         dirContext.close();
         
         return value;
      }
      catch (Exception exception) {
         Log.excp("NDSLib", "getAuthUserAttribute", exception.toString());                  
      }
      
      return null;
   }
   
   /**
    * Modifies the given user's desired NDS attribute with the given value. If 
    *  an error occurs, false is returned.
    */
   private static boolean setAuthUserAttribute(String loginname, 
                                               String attributeName,
                                               Object value)
   {  
      DirContext dirContext;      
      BasicAttribute attribute;
      ModificationItem modItems[];
      
      try {
         
         // Create the initial directory context
         if ((dirContext = ContextProvider.getContext()) == null) {
            Log.error("NDSLib", "setAuthUserAttribute", "Unable to get context");
            return false;
         }
           
         // Create a structure describing the change
         modItems = new ModificationItem[1];
         modItems[0] = new ModificationItem(DirContext.REPLACE_ATTRIBUTE,
                                      new BasicAttribute(attributeName, value));
         
         // Change the desired attribute
         dirContext.modifyAttributes("uid=" + loginname + ",uid=AUTHSERVERS", 
                                     modItems);
         
         // Close the directory context
         dirContext.close();
         
         return true;
      }
      catch (NameNotFoundException notFound) {
         Log.error("NDSLib", "setAuthUserAttribute", "Value for " + loginname + 
                   " not found");         
      }
      catch (Exception exception) {
         Log.excp("NDSLib", "setAuthUserAttribute", exception.toString());                  
      }
      
      return false;
   }      
}

/**
 * Decides which NDS server to connect to and returns an InitialDirContext
 *  to that host.
 */
class ContextProvider
{
   private static ListIndex currentListIndex;
   private static Vector serverList;
   private static int NDS_MASTER_INDEX;
   private static int NUM_SERVERS;    
   private static boolean initializationError;
   
   /**
    * Create the list of NDS servers 
    */
   static {    
      createServerList();
   }
   
   /**
    * Decides which NDS server to connect to and returns an InitialDirContext
    *  to that host. If a connection can't be made, null is returned.
    *
    * This method is synchronized so that it is not called at the same time
    *  as the createServerList method.
    */
   public static synchronized InitialDirContext getContext()
   {
      InitialDirContext dirContext;      
      int connectionAttempts;
      int currentPos;
      
      try {
         
         // Check to see if there was an error initializing the list of servers
         if (initializationError == true) {
            Log.error("ContextProvider", "getContext", "Initialzation error");
            return null;
         }
       
         // If this is the NTCSS master server, we must connect to the LDAP
         //  master server. 
         // NOTE: This call to getMasterServer must be done
         //  everytime to allow for a host to be configured to be the master
         //  dynamically (which will be reflected in the output of this method).
         if (DataLib.getMasterServer(new StringBuffer()) == 1)             
            return attemptConnect((String)serverList.elementAt(NDS_MASTER_INDEX));         
         
             
         // Try to connect to each of the LDAP servers until either a connection is
         //  made, or every server has been tried. If can't connect to any of 
         //  the servers, null is returned.
         
         // Keep using a server until an attempt to connect to it fails.          
         connectionAttempts = 0;
         currentPos = currentListIndex.get();  
         do {
            
            // Try to connect to the ldap server on the given host
            if ((dirContext = attemptConnect((String)serverList.elementAt(currentPos))) == null)
               currentPos = (currentPos + 1) % NUM_SERVERS;
            else {  
               
               // "Remember" the index of this host, so that future
               //  requests for contexts will first try to connect to the host at
               //  this index.
               currentListIndex.set(currentPos);

               Log.debug("ContextProvider", "getContext", 
                      "Returned connection to <" + 
                      (String)serverList.elementAt(currentPos) + ">");
            
               return dirContext;
            }
                      
         } while (++connectionAttempts < NUM_SERVERS);
         
         // We must not have found a connection
         return null;          
      }
      catch (Exception exception) {
         Log.excp("ContextProvider", "getContext", exception.toString());
         return null;
      }
   }

   /**
    * Creates a list of NDS servers. If an error occurs, the initializationError
    *  member is set to true. 
    *
    * This method should be called from the static initializer of this class.
    *  It can also be called later if it is desired that the server list needs 
    *  to be refreshed.
    *
    * This method is synchronized so that it is not called at the same time
    *  as the getContext method.
    */
   public static synchronized void createServerList()
   {
      File tmpFile;
      Random randomNumGen;      
      
      try {
         
         serverList = new Vector();
         currentListIndex = new ListIndex();
         
         initializationError = false;
         
         // If the ntcss.nds_server property is set in the properties file, just
         //  use the host specified there. This property should only be set
         //  in our development environment - never in the field.
         if (Constants.NDS_SERVER != null) {
            
            // Just assume that the host specified in the constant is the
            //  NDS master
            NDS_MASTER_INDEX = 0;
            
            // Add the default host specified in the properties file
            serverList.add(Constants.NDS_SERVER);
         }
         else {  // Attempt to download the server list file and create 
                 //  the list of NDS servers from it
            
            tmpFile = File.createTempFile("nds", "tmp", 
                                         new File(Constants.NTCSS_LOGS_DIR));
            
            if ((getServerListFile(tmpFile.getAbsolutePath()) == false) ||
                (importServerList(tmpFile.getAbsolutePath()) == false))
               initializationError = true;            
            
            tmpFile.delete();
         }
         
         if (initializationError == false) {
            
            // Record the number of servers in the list
            NUM_SERVERS = serverList.size(); 
         
            // Set the current index to the table to a random number. This 
            //  will help to distribute the load on the servers.
            randomNumGen = new Random();
            currentListIndex.set(Math.abs(randomNumGen.nextInt() % NUM_SERVERS));
         }
      }
      catch (Exception exception) {      
         Log.excp("ContextProvider", "createServerList", exception.toString());
         initializationError = true;
      }
   }
      
   /**
    * Retrieves the file which contains the list of
    *  all the LDAP servers, from the NTCSS master
    */
   private static boolean getServerListFile(String tmpFilename)
   {
      StringBuffer masterName = new StringBuffer();
      boolean ret;
      
      // Look for the name of the NTCSS master in the database first. If that
      //  fails use the "default master" constant.
      if (DataLib.getMasterServer(masterName) == ErrorCodes.ERROR_CANT_DETER_MASTER) {
         
         if (Constants.NTCSS_DEFAULT_MASTER == null) {
            Log.error("ContextProvider", "getServerListFile", "Unable to " + 
                      "master server");
            return false;
         }
         else
            masterName.insert(0, Constants.NTCSS_DEFAULT_MASTER);
      }
      
      if ((ret = FTP.get(masterName.toString(), "/h/NTCSSS/ldap/ldapservers",
                     tmpFilename, false)) == false)
         Log.error("ContextProvider", "getServerListFile", "Unable to " + 
                  "retreive server list from <" + masterName.toString() + ">");
      
      return ret;    
   }
     
   /**
    * Reads the contents of the given file, and stores the host names in 
    *  the NDS server list object. If an error occurs, false is returned.
    */
   private static boolean importServerList(String filename)
   {      
      BufferedReader reader;
      String line;
      int serverIndex = 0;
    
      
      try {
               
         NDS_MASTER_INDEX = -1;
         
         // Read LDAP servers from the file into the vector
         reader = new BufferedReader(new FileReader(filename));                 
                 
         while ((line = reader.readLine()) != null) {
 
            line = line.trim();
            
            // Skip empty lines, and lines that don't contain complete info
            if (line.length() > 1) {
               serverList.add(line.substring(1));
           
               // Record the index of LDAP master server
               if ((line.charAt(0) == 'M') || (line.charAt(0) == 'm'))
                  NDS_MASTER_INDEX = serverIndex;    
            
               serverIndex++;  
            }
         }
               
         reader.close();
 
         return true;         
      }
      catch (Exception exception) {
         Log.excp("ContextProvider", "importServerList", exception.toString());
         return false;
      }
   }
   
   /**
    * Attempts to make a connection to the ldap server on the given host.
    *  If a connection was made, the InitialDirContext object that was 
    *  created, is returned. Otherwise, null is returned.
    */
   private static InitialDirContext attemptConnect(String hostName)
   {
      Hashtable contextParameters;
      
      try {
         
         // Setup the parameters for the directory services context
         contextParameters = new Hashtable();
         contextParameters.put(Context.INITIAL_CONTEXT_FACTORY,
                               "com.sun.jndi.ldap.LdapCtxFactory");
         contextParameters.put(Context.PROVIDER_URL, 
                               "ldap://" + hostName + 
                               ":" + Constants.NDS_SERVER_PORT + 
                               "/o=" + Constants.NDS_ROOT);                  
      
         return new InitialDirContext(contextParameters);
      }
      catch (Exception exception) {
         Log.warn("ContextProvider", "attemptConnect", 
                  "Connect failed to host <" + hostName+ ">");
         return null;
      }
   }
 
   
   /**
    * A synhronization object representing an integer
    */
   static class ListIndex
   {
      private int index;
      
      public synchronized void set(int val)
      {
         index = val;
      }
      
      public synchronized int get()
      {
         return index;
      }
   }
}
