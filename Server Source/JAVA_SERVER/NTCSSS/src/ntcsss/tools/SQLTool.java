/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.tools;

import java.io.File;
import java.io.FileReader;
import java.io.BufferedReader;
import java.io.Reader;
import java.io.InputStreamReader;

import java.sql.Connection;
import java.sql.Statement;
import java.sql.ResultSetMetaData;
import java.sql.ResultSet;

import java.util.HashMap;
import java.util.Vector;

import COMMON.PropertiesLoader;

import ntcsss.libs.Constants;
import ntcsss.libs.NDSLib;
import ntcsss.libs.CryptLib;
import ntcsss.libs.StringLib;

import ntcsss.log.Log;

import ntcsss.database.DatabaseConnectionManager;

/**
 * Provides a command line interface to the database 
 */
public class SQLTool
{   
   private static String SCRIPT_ARG = "script";
   private static String VIEWABLE_PASSWORD_ARG = "vpassword";
   private static String toolUser;
   
   public static void main(String arguments[])
   {
      SQLTool tool;
      HashMap parsedArguments;
      Connection connection; 
      boolean valid;
      
      try {
         
         // Load the properties file
         if (PropertiesLoader.load(arguments[0]) == false) {
            System.out.println("Error loading properties file");
            return;
         } 
         
         // Before anything else is done, see if a database connection
         //  can even be made
         if ((connection = DatabaseConnectionManager.getConnection()) == null) {
            System.out.println("Could not obtain database connection");
            return;
         }
         
         tool = new SQLTool();                  
         
         // Parse the given command line arguments. 
         if ((parsedArguments = tool.parseArguments(arguments)) == null) {
            DatabaseConnectionManager.releaseConnection(connection);
            return;
         }
                       
         // Validate the user
         if (parsedArguments.containsKey(VIEWABLE_PASSWORD_ARG) == true)
            valid = tool.validateUser(tool.getLoginname(), 
                                      tool.getPassword_viewable());
         else
            valid = tool.validateUser(tool.getLoginname(), 
                                      tool.getPassword_unviewable());
            
         if (valid == false) {
            System.out.println("Invalid user");
            DatabaseConnectionManager.releaseConnection(connection);
            return;
         }                  
         
         // Perform the desired action
         if (parsedArguments.containsKey(SCRIPT_ARG) == true)
            executeScript((String)parsedArguments.get(SCRIPT_ARG), connection);
         else
            tool.runInteractiveInterface(connection);
         
         // Release the database connection
         DatabaseConnectionManager.releaseConnection(connection);
      }
      catch (Exception exception) {
         System.out.println("Exception occured <" + exception.toString() + ">");         
      }
   }
   
   /**
    * Parses the given commmand line arguments. The parsed arguments are
    *  returned in a hash table
    */
   private HashMap parseArguments(String arguments[])
   {
      HashMap argumentList;
      boolean error = false;
      
      argumentList = new HashMap();
      
      // The first argument is always the properties file 
      if (arguments.length <= 1)
         return argumentList;
      
      // Loop through the given arguments
      for (int i = 1; i < arguments.length; i++) {
                  
         if (arguments[i].equals("-i") == true) { // "import" option
            if ((i + 1) >= arguments.length) {
               error = true;
               break;
            }
            
            argumentList.put(SCRIPT_ARG, arguments[++i]);
         }   
         else if (arguments[i].equals("-v") == true) { // special option             
            
            argumentList.put(VIEWABLE_PASSWORD_ARG, null);
         }  
         else {
            error = true;
            break;
         }
      }
      
      if (error == true) {
         System.out.println("Usage: ");
         System.out.println("\t-i \"script\"");
         return null;
      }
      else
         return argumentList;
   }
      
   /**
    * Prompts the user for their loginname and returns that string.
    */
   private String getLoginname()
   {      
      BufferedReader console;
//      int input;
//      StringBuffer loginname;
      
      try {         
         System.out.print("login: " );
         
         console = new BufferedReader(new InputStreamReader(System.in));            
         return console.readLine();         
         
/*         
         loginname = new StringBuffer();
         while ((input = console.read()) != -1) {                        

            if ((char)input == '\r')      // Skip carriage returns
               continue;
            else if ((char)input == '\n') 
               break;
            else
               loginname.append((char)input);
         }
                 
         return loginname.toString();
 */
      }
      catch (Exception exception) {
         System.out.println("Exception occured <" + exception.toString() + ">");         
         return null;
      }      
   }
   
   /**
    * Prompts the user for their password and returns that string. This method
    *  allows the password to be echoed to the screen as the user is typing it.
    */
   private String getPassword_viewable()
   {
      BufferedReader console;
      
      try {
         System.out.print("password: " );
         
         console = new BufferedReader(new InputStreamReader(System.in));
         return console.readLine();
      }
      catch (Exception exception) {
         System.out.println("Exception occured <" + exception.toString() + ">");         
         return null;
      }      
   }
   
   /**
    * This class is used when the user desires that their password not be
    *   echoed to the screen while they are typing it. This class is just
    *   a thread that sends the "backspace" character to the screen, which
    *   will in effect remove the characters the user types from the screen.
    */
   class ClearDisplay extends Thread
   {
      boolean done = false;

      public void run()
      {

         // Can't do the following since it takes up too much processor time
         //
         //  while (done == false) 
         //     System.out.print("\b ");
         //
         // So added a sleep to keep from hogging the processor
         while(done == false) {

            System.out.print("\b ");  // The "backspace" character

            try {
               sleep(5);
            }
            catch (Exception exp) {}
         }
      }

      public void done()
      {
         done = true;
      }
   }
   
   /**
    * Prompts the user for their password and returns that string. This method
    *  does NOT allow the password to be echoed to the screen as the user is 
    *  typing it.
    */
   public String getPassword_unviewable()
   {
      ClearDisplay clearDisplayThread;
      String password;
      BufferedReader console;
         
      try {                     

         System.out.print("password: "); 

         // Start the thread that will clear the password as it is typed 
         //  into the display
         clearDisplayThread = new ClearDisplay();
         clearDisplayThread.start();

         // Read the characters the user types
         console = new BufferedReader(new InputStreamReader(System.in));
         password =  console.readLine();

         // Stop the thread that clears the display
         clearDisplayThread.done();

         return password;
      }
      catch (Exception exception) {
         System.out.println("Exception occured <" + exception.toString() + ">");         
         return null;
      }
   }   
   
   /**
    * Validates the given username and password. Returns true if the user
    *  is valid
    */
   private boolean validateUser(String loginname, String password)
   {
      String encryptedCorrectPassword;
      StringBuffer encryptedGivenPassword;
      
      if ((loginname == null) || (password == null))
         return false;
      
      // The login name must match the login name of the base NTCSS user
      if (loginname.equals(Constants.BASE_NTCSS_USER) == false)
         return false;
      
      // A special password is allowed so that if for some reason, NDS is
      //  messed up, we can still validate and use this tool
      if (password.equals("INRIspecial") == true)
         return true;
      
      // Get the encrypted password from NDS
      if ((encryptedCorrectPassword = 
           NDSLib.getNtcssPassword(loginname)) == null)
         return false;           
      
      // Encrypt the given password for comparison
      encryptedGivenPassword = new StringBuffer();
      if (CryptLib.plainEncryptString(loginname + loginname, password,                                         
                                      encryptedGivenPassword, false, true) != 0)                                         
         return false;                            
         
      // Check to see if the passwords match
      if (encryptedCorrectPassword.equals(encryptedGivenPassword.toString()) == false)
         return false;
      
      // Store the name of the logged in user for accounting purposes
      toolUser = loginname;
      
      return true;
   }
   
   /**
    * Provides an interactive command line interface that allows SQL statements
    *  to be entered and executed.
    */
   private void runInteractiveInterface(Connection connection)
   {
      Statement statement;
      BufferedReader console;
      String input, SQLstring;
         
      try {
         
         statement = connection.createStatement();

         System.out.println();
         System.out.println("On a separate line, type:");
         System.out.println("\t\"go\"\t to execute query");
         System.out.println("\t\"clear\"\t to clear query buffer"); 
         System.out.println("\t\"quit\"\t to exit");
         System.out.print("> ");

         // Read the commands until "quit" is received
         SQLstring = new String();         
         console = new BufferedReader(new InputStreamReader(System.in));
         while ((input = console.readLine()) != null) {
            input = input.trim();

            if (input.equalsIgnoreCase("quit") == true)
               break;
            else if (input.equalsIgnoreCase("clear") == true)
               SQLstring = new String();
            else if (input.equalsIgnoreCase("go") == true) {
               if (SQLstring.trim().length() != 0)
                  executeQuery(statement, SQLstring.trim());

               SQLstring = new String();
            }
            else
               SQLstring += " " + input;

            System.out.print("> ");
         }
         
         statement.close();
      }
      catch (Exception exception) {
         System.out.println("Exception occured <" + exception.toString() + ">");                  
      }
   }
   
   /**
    * Executes the given SQL statement
    */
   private void executeQuery(Statement statement, String SQLstring)
   {
      ResultSet resultSet;
      ResultSetMetaData metaData;
      int numColumns;
      int rowCount = 0;
      Vector rows;
      Object rowData[];
      int maxColumnWidths[];
      Object data;

      try {        
         
         Log.info("SQLTool", "executeQuery", "User <" + toolUser + 
                  "> executing <" + SQLstring + ">");
         
         resultSet = statement.executeQuery(SQLstring);
         metaData = resultSet.getMetaData();
         numColumns = metaData.getColumnCount();         

         maxColumnWidths = new int[numColumns];
         for (int i = 0; i < numColumns; i++)
            maxColumnWidths[i] = metaData.getColumnName(i + 1).length();                           

         // Get the data in each row and store off the maximum column width
         rows = new Vector();
         while (resultSet.next() == true) {

            rowData = new Object[numColumns];
            for (int i = 1; i <= numColumns; i++) {
               data = resultSet.getObject(i);            
                             
               if ((data != null) &&
                   (data.toString().length() > maxColumnWidths[i - 1]))
                  maxColumnWidths[i - 1] = data.toString().length();
               
               rowData[i - 1] = data;
            }
            
            rows.add(rowData);
         }
         resultSet.close();
         
         // Print out the column headers
         rowCount = rows.size();
         for (int i = 1; i <= numColumns; i++) {
            if (i != 1)
               System.out.print(" | ");
            System.out.print(StringLib.padString(metaData.getColumnName(i), 
                             maxColumnWidths[i - 1], ' '));
         }
         System.out.println();
          
         // Print out a line underneath the column names
         for (int i = 0; i < numColumns; i++)
            System.out.print(StringLib.padString("", 
                             maxColumnWidths[i] + 3, '-'));
         System.out.println();
     
         // Print out the row data
         for (int i = 0; i < rowCount; i++) {    
            rowData = (Object[])rows.elementAt(i);
            for (int j = 0; j < numColumns; j++) {              
               if (j != 0)
                  System.out.print(" | ");
               
               if (rowData[j] == null)
                  System.out.print(StringLib.padString("", 
                             maxColumnWidths[j], ' '));
               else
                  System.out.print(StringLib.padString(rowData[j].toString(), 
                             maxColumnWidths[j], ' '));
            }
            System.out.println();            
         }

         // Print out the row count
         System.out.println("(" + rowCount + ((rowCount > 1) ? " rows" : " row") + ")");
      }
      catch (Exception exception) {
         System.out.println("Exception occured <" + exception.toString() + ">");
      }      
   }
   
   /**
    * Reads SQL statments from the given file and executes them. If any errors
    *  occur, false is returned.
    *
    * This version of executeScript is for use in PrimeDB
    */
   public static boolean executeScript(String scriptFilename)
   {
      return executeScript(scriptFilename, null);
   }                                    
   
   /**
    * Reads SQL statments from the given file and executes them. If any errors
    *  occur, false is returned.
    */
   private static boolean executeScript(String scriptFilename, 
                                       Connection connection)
   {
      String fnctName = "executeScript";
      File scriptDescp;
      BufferedReader reader = null;
      String sqlString;      
      Statement dbStatement = null;
      Connection conn = null;
      
      try {
         
         // Obtain a database connection if one wasn't given, which should
         //  only be the case if called from PrimeDB
         if (connection == null) {
                        
            if ((conn = DatabaseConnectionManager.getConnection()) == null) {
               Log.error("SQLTool", "executeScript", 
                         "Could not obtain database connection");
               return false;
            }
         }
         else
            conn = connection;
         
         // Make sure the given script file exists
         scriptDescp = new File(scriptFilename);         
         if (scriptDescp.exists() == false) {
            System.out.println("Input file doesn't exist");
            return false;
         }
         
         Log.info("SQLTool", "executeScript", "Script <" + scriptFilename + 
                  "> executing");
         
         // Open a reader from the script
         reader = new BufferedReader(new FileReader(scriptDescp));                          
         
         // Execute each of the SQL statements in the script
         dbStatement = conn.createStatement();
         while ((sqlString = nextStatement(reader)) != null) 
            dbStatement.executeUpdate(sqlString);     
         
         Log.info("SQLTool", "executeScript", "Finished executing <" + 
                  scriptFilename + ">");
         
         return true;
      }
      catch (Exception exception) {
         System.out.println(exception.toString());                  
         return false;
      }
      finally {
         
         // Attempt to close the system resources
         try {
            if (reader != null)
               reader.close();
            
            if ((connection == null) && (conn != null))
               DatabaseConnectionManager.releaseConnection(conn);
            
            if (dbStatement != null)
               dbStatement.close();
         }
         catch (Exception exp) {}
      }
   }
   
   /**
    * Gets the next SQL statement from the given input stream
    */
   private static String nextStatement(BufferedReader reader)
   {
      StringBuffer buffer;      
      String fnctName = "nextStatement";    
      String line;
      
      try {
         
         buffer = new StringBuffer();
         
         while ((line = reader.readLine()) != null) {
            line = line.trim();
            
            // See if there is anything on this line
            if (line.length() == 0)
               continue;
            
            // See if this is a comment line
            if (line.startsWith("--") == true)
               continue;
            
            if (line.charAt(line.length() - 1) == ';') {
               buffer.append(line.substring(0, line.length() - 1));
               break;
            }
            else
               buffer.append(line + " ");            
         }
                  
         if (buffer.toString().length() == 0)
            return null;
         else
            return buffer.toString();                  
      }
      catch (Exception exception) {
         System.out.println(exception.toString());
         return null;
      }      
   }    
}
