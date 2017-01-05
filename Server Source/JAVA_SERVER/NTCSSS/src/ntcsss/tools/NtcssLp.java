/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.tools;

import java.util.Vector;
import java.util.StringTokenizer;

import java.sql.Connection;
import java.sql.Statement;
import java.sql.ResultSet;

import java.io.File;
import java.io.FileWriter;

import COMMON.PropertiesLoader;

import ntcsss.log.Log;
import ntcsss.log.EntityName;

import ntcsss.libs.DataLib;
import ntcsss.libs.StringLib;
import ntcsss.libs.Constants;

import ntcsss.database.DatabaseConnectionManager;

/**
 * Handles printing of files
 */
public class NtcssLp
{
   private String printerName;
   private String outputType;
   private int numCopies;
   private String userName;
   private String appName;
   private int securityLevel;
   private Vector optionList;
   private Vector fileList;  
   private String filePtrOutputFile;
   private boolean printBanner;
   private boolean portraitOrientation;
   private int paperSize;

   private final String NTCSS_PRT_JOB_PREFIX = "NTCSS_PRINTREQ:";
   private final String LP_TEMP_PREFIX = "NTLP_";
         
   public static void main(String arguments[])
   {
      NtcssLp lp;
      Vector fileList;
      Vector optionsList;      
      String printerName = null, outputType = null, userName = null, 
             appName = null;
      int numCopies = -1;
      int securityLevel = -1;
      String securityStr;
      String option;    
      String usageStr;
      
      try {
         
         // The first argument should always be the properties file.
         //  Load the properties file
         if ((arguments.length < 1) || 
             (PropertiesLoader.load(arguments[0]) == false)) {
            System.out.println("Error loading properties file");
            System.exit(1);
         }
         
         Log.setDefaultEntityName(EntityName.NTCSS_LP); 
         
         usageStr = "USAGE: ntcss_lp -S<security option> -a<program group> " +
               "(-d<printer> or -O<output_type>) -n<number of copies> " +
               "-u<username> [-o<options>] <files ...>";
         
         // Make sure arguments were passed in. If not, print the usage.
         if (arguments.length <= 1) {      
            System.out.println(usageStr);
            System.exit(1);
         }                                    
         
         // Collect the command line arguments
         fileList = new Vector();
         optionsList = new Vector();
         for (int i = 1; i < arguments.length; i++) {
            
            if (arguments[i].charAt(0) != '-')   // This must be a filename
               fileList.add(arguments[i]);
            else {
               
               switch (arguments[i].charAt(1)) {
                  
                  case 'd':  // Destination
                     printerName = arguments[i].substring(2);                                          
                     break;
                     
                  case 'O':  // Output type
                     outputType = arguments[i].substring(2);                                          
                     break;
                     
                  case 'n':  // Number of copies
                     numCopies = StringLib.parseInt(arguments[i].substring(2), -1);
                     break;
                     
                  case 'u':  // User
                     userName = arguments[i].substring(2);                     
                     break;
                     
                  case 'o':  // Option                      
                     optionsList.add(arguments[i].substring(2));
                     break;
                     
                  case 'a':  // Application
                     appName = arguments[i].substring(2);                     
                     break;
                     
                  case 'S':  // Security
                     securityStr = arguments[i].substring(2);
                     if (securityStr.equals("U") == true) 
                        securityLevel = Constants.UNCLASSIFIED_CLEARANCE;
                     else if (securityStr.equals("US") == true) 
                        securityLevel = Constants.UNCLASSIFIED_SENSITIVE_CLEARANCE;
                     else if (securityStr.equals("C") == true) 
                        securityLevel = Constants.CONFIDENTIAL_CLEARANCE;
                     else if (securityStr.equals("S") == true) 
                        securityLevel = Constants.SECRET_CLEARANCE;
                     else if (securityStr.equals("TS") == true) 
                        securityLevel = Constants.TOPSECRET_CLEARANCE;
                     else {
                        System.out.println("Invalid security supplied!");
                        System.out.println("Security is one of the following:");
                        System.out.println("\tU(UNCLASS)");
                        System.out.println("\tUS(SENSITIVE)");
                        System.out.println("\tC(CONFID)");
                        System.out.println("\tS(SECRET)");
                        System.out.println("\tTS(TOP SECRET)");
                        return;
                     }                                          
                     break;
                     
                  default:                     
                     System.out.println("Invalid parameter");
                     System.out.println(usageStr);
                     return;                  
               }
            }
         }
         
         // Validate the parameters, and if they are correct, print the file(s)
         lp = new NtcssLp(printerName, outputType, numCopies, userName, appName,
                          securityLevel, optionsList, fileList, false, true, 1);
         lp.print();            
      }
      catch (Exception exception) {
         System.out.println("Exception occurred <" + exception.toString() + ">");
      }
   }
   
   /**
    * Constructor
    */
   public NtcssLp(String printerName, String outputType, int numCopies, 
                  String userName, String appName, int securityLevel, 
                  Vector opList, Vector fileList, boolean printBanner,
                  boolean portraitOrientation, int paperSize)
   {
      int index;
      String option;
            
      // Strip off anything after the @ character (including the @ character)
      //  if it exists
      if ((printerName != null) && ((index = printerName.indexOf('@')) != -1))
         this.printerName = printerName.substring(0, index);
      else
         this.printerName = printerName;
      
      // Check the options list for special options that need to be replaced
      this.optionList = opList;
      if (optionList != null) {
         
         for (int i = 0; i < optionList.size(); i++) {
            option = (String)optionList.elementAt(i);
            if ((option.equalsIgnoreCase("h") == true) ||
                (option.equalsIgnoreCase("-h") == true))
               optionList.setElementAt("BSDh", i);            
         }
      }
      
      this.outputType = outputType;
      this.numCopies = numCopies;
      this.userName = userName;
      this.appName = appName;
      this.securityLevel = securityLevel;      
      this.fileList = fileList;   
      this.printBanner = printBanner;
      this.portraitOrientation = portraitOrientation;
      this.paperSize = paperSize;
      
      this.filePtrOutputFile = null;
      
      // Load the native library
      try {
         System.load(Constants.NATIVE_LIBRARY);
      }
      catch (Exception exception) {
         Log.excp("NtcssLp", "Constructor", exception.toString());
      }
   }
   
   /**
    * Constructor
    *
    *  Takes a single file instead of a list of files to be printed.
    */
   public NtcssLp(String printerName, String outputType, int numCopies, 
                  String userName, String appName, int securityLevel, 
                  Vector opList, String file, boolean printBanner,
                  boolean portraitOrientation, int paperSize)
   {
      this(printerName, outputType, numCopies, userName, appName, 
           securityLevel, opList, new Vector(), printBanner, 
           portraitOrientation, paperSize);            
      
      if (file != null)          
         fileList.add(file);                  
   }
   
   /**
    * Prints the given file(s) to the specified printer
    */
   public boolean print()
   {      
      String fileListStr = null;
      String optionListStr = null;
      boolean firstAdded;
      boolean successful;
      String message;
      
      // Make sure the given parameters are valid
      if (validateParameters() == false)
         return false;
      
      // Since just printing locally for now, remove the special prefix at the
      //  front of the printer name
      if (printerName.startsWith(NTCSS_PRT_JOB_PREFIX) == true)
         printerName = printerName.substring(NTCSS_PRT_JOB_PREFIX.length());
                  
      // Create a comma delimited list of files to be printed 
      firstAdded = false;
      for (int i = 0; i < fileList.size(); i++) {
         if (firstAdded == false) {
            fileListStr = checkFile((String)fileList.elementAt(i));
            firstAdded = true;
         }
         else
            fileListStr += "," + checkFile((String)fileList.elementAt(i));
      }
      
      // Create a comma delimited list of all the special options      
      if ((optionList != null) && (optionList.size() > 0)) {
         
         optionListStr = new String();
         firstAdded = false;
         for (int i = 0; i < optionList.size(); i++) {
            
            if (firstAdded == false) {
               optionListStr += optionList.elementAt(i);
               firstAdded = true;
            }
            else
               optionListStr += "," + optionList.elementAt(i);
         }
      }
            
      // Send the files off to be printed      
      successful = printTextFilesToLocalPrinter(printerName, fileListStr,
                                                optionListStr, numCopies, 
                                                filePtrOutputFile, printBanner,
                                                portraitOrientation, paperSize,
                                                userName, appName, 
                                                Constants.getSecurityString(securityLevel));
      
      // Remove any temporary files that were created for empty or 0 length
      //  files that were requested to be printed
      removeTempFiles(fileListStr);
      
      // Print a message
      if (successful == true)  {
         message = "Printed job on printer " + printerName + ".";        
         Log.info("NtcssLp", "print", message);            
      }
      else {
         message = "Error: Cannot print job on printer " + printerName + "!"; 
         Log.error("NtcssLp", "print", message);            
      }
      System.out.println(message);      
      
      return successful;
   }
   
   /**
    * Makes sure the given parameters are valid. If they are, true is returned.
    */
   private boolean validateParameters()
   {
      boolean userOptionFound, appOptionFound, securityOptionFound, 
              otOptionFound, destOptionFound;
      String fnctName = "validateParameters";
      String errorStr; 
      Connection connection = null; 
      Statement statement = null;
      ResultSet resultSet;
      boolean error;
      Object sqlItems[];
      String redirectedPrinterName;
      
      try {
         
         userOptionFound = ((userName != null) ? true : false);
         appOptionFound = ((appName != null) ? true : false);
         otOptionFound = ((outputType != null) ? true : false);
         destOptionFound = ((printerName != null) ? true : false);
         securityOptionFound = ((securityLevel != -1) ? true : false); 
         
         // Make sure files have been specified to be printed
         if ((fileList == null) || (fileList.size() == 0)) {
            errorStr = "No files specified for printing";
            System.out.println(errorStr);
            Log.error("NtcssLp", fnctName, errorStr);
            return false;
         }
         
         // Make sure all the parameters are given
         if ((appOptionFound == false) ||
             (securityOptionFound == false) ||
             (userOptionFound == false) ||
             ((otOptionFound == false) && (destOptionFound == false))) {
      
            errorStr = "NTCSS print job must have destination(-d<PRINTER>), " +
                          "application(-a<APPNAME>),security(-S<SECURITY>) " + 
                          "and user(-u<user>) options specified";
            System.out.println(errorStr);
            Log.error("NtcssLp", fnctName, errorStr);
            return false;
         }                  
      
         // Both the output type AND printer options cannot be specified
         if ((otOptionFound == true) && (destOptionFound == true)) {
            errorStr = "Cannot set both Output Type and printer!";
            System.out.println(errorStr);
            Log.error("NtcssLp", fnctName, errorStr);
            return false;
         }       
      
         // Determine the printer to print to. If a printer was specified, make
         //  sure it is a valid printer. Otherwise, an output type was specified,
         //  so try to get the default printer for that output type.
      
         // Get a database connection
         if ((connection = DatabaseConnectionManager.getConnection()) == null) {
            errorStr = "Unable to obtain database connection";
            System.out.println(errorStr);
            Log.error("NtcssLp", fnctName, errorStr);
            return false;
         }
         statement = connection.createStatement();
       
         // If a printer is specified, make sure the printer is in the local database
         if (destOptionFound == true) {         
                               
            // See if there is a record for this printer in the printers database
            //  table. Perform case insensitive search, because there is no
            //  telling the case Windows will return for its printers
            //  (e.g. \\CAPECOD\QE2 or \\capecod\qe2)
            sqlItems = new Object[1];
            sqlItems[0] = printerName.toUpperCase();
            resultSet = statement.executeQuery(StringLib.SQLformat("SELECT " +
                  "redirection_ptr FROM printers WHERE upper(printer_name) = " + 
                  Constants.ST, sqlItems));
            if (resultSet.next() == false) {               
               errorStr = "printer " + printerName + " is not an NTCSS printer!";
               System.out.println(errorStr);
               Log.error("NtcssLp", fnctName, errorStr);
               error = true;
            }
            else {
               error = false;
               
               // Change printer name to its redirection printer if there is one
               redirectedPrinterName = resultSet.getString("redirection_ptr");
               if ((redirectedPrinterName != null) && 
                   (redirectedPrinterName.equalsIgnoreCase("NONE") == false))
                  printerName = redirectedPrinterName;
            }            
            resultSet.close();
                     
            if (error == true)
               return false;                     
         }
         else {  // An output type must have been specified      
         
            // Make sure the application name was specified
            if ((appOptionFound == false) || (appName.length() == 0)) {          
               errorStr = "Output Type Specification Requires an App Name!";
               System.out.println(errorStr);
               Log.info("NtcssLp", fnctName, errorStr); 
               return false;
            }
         
            // Get the default printer for the given outputType/application
/*            
            sqlItems = new Object[3];
            sqlItems[0] = outputType;
            sqlItems[1] = appName;
            sqlItems[2] = new Boolean(false);
            resultSet = statement.executeQuery(StringLib.SQLformat("SELECT " +
             "default_printer FROM output_types, printers WHERE otype_title " +
             "= " + Constants.ST + " AND app_title = " + Constants.ST + " AND " +
             "default_printer = printer_name AND general_access = " + Constants.ST, 
             sqlItems));
 */
            sqlItems = new Object[4];
            sqlItems[0] = outputType;
            sqlItems[1] = appName;
            sqlItems[2] = new Boolean(true);
            sqlItems[3] = new Boolean(false);
            resultSet = statement.executeQuery(StringLib.SQLformat("SELECT " + 
              "output_printers.printer_name FROM output_printers, printers " +
              "WHERE otype_title = " + Constants.ST + " AND app_title = " + 
              Constants.ST + " AND default_printer = " + Constants.ST + 
              " AND output_printers.printer_name = printers.printer_name " + 
              "AND general_access = " + Constants.ST, sqlItems));
            if (resultSet.next() == false) {
               errorStr = "Invalid Output Type/App! --  " + outputType + "/" + 
                          appName;
               System.out.println(errorStr);
               Log.error("NtcssLp", fnctName, errorStr);
               
               error = true;
            }
            else {
               error = false;
//               printerName = resultSet.getString("default_printer");
               printerName = resultSet.getString("output_printers.printer_name");
            }
            resultSet.close();
            
            if (error == true)
               return false;                                  
         }
         
         // If this is a "file" type printer, get the name of the output file.         
         // If this printer is an NTSHARE type printer, exchange its name with
         //  its NT host and printer share name.        
         sqlItems = new Object[1];
         sqlItems[0] = printerName;
         resultSet = statement.executeQuery(StringLib.SQLformat("SELECT " +
             "printing_method, file_name, nt_host_name, nt_share_name FROM printers WHERE printer_name " +
             "= " + Constants.ST, sqlItems));
         resultSet.next();  // We've already established that there is a record
                            //  or else we would not get this far
         if (resultSet.getString("printing_method").equals(Constants.FILE_PRINTER) == true)
            filePtrOutputFile = resultSet.getString("file_name");
         else if (resultSet.getString("printing_method").equals(Constants.NT_SHARE_PRINTER) == true)
            printerName = "\\\\" + resultSet.getString("nt_host_name") +
                          "\\" + resultSet.getString("nt_share_name"); 
         
         resultSet.close();                           
         
         // Append to the beginning of the printer name a special string that
         //  will indicate to the NTCSS LPD that it is a NTCSS print job
         //  that it just received.
         printerName = NTCSS_PRT_JOB_PREFIX + printerName;                  
      
         return true;
      }
      catch (Exception exception) {
         errorStr = "An exception occurred <" + exception.toString() + ">";
         System.out.println(errorStr);
         Log.excp("NtcssLp", fnctName, errorStr);
         return false;
      }
      finally {
         
         // Close the database resources
         try {
            if (statement != null)
               statement.close();
            
            if (connection != null)
               DatabaseConnectionManager.releaseConnection(connection);
         }
         catch (Exception exp) {
            return false;
         }
         
      }
   }       
   
   /**
    * Checks to see if the given file exists or if it is empty. If either of
    *  these cases applies, a temporary file is created with an error message
    *  as its data, and that file is then designated to be printed. The
    *  name of the file to be printed is returned.
    */
   private String checkFile(String filename)
   {
      StringBuffer errorMessage;      
      String sysConfTag;      
      File fileDescp, tmpFileDescp;      
      FileWriter outputStream;
      boolean emptyFile = false;
      boolean noFile = false;
                             
      try {                                    
      
         // Check to see if the file exists or if it is empty
         fileDescp = new File(filename);
         if (fileDescp.exists() == true) {
            
            // Check the file's size
            if (fileDescp.length() == 0)   
               emptyFile = true;            
         }
         else  // File doesn't exist
            noFile = true;         
      
         // If the file doesn't exist or if it is empty, create a temporary
         //  file to print instead. Write out to the temporary file an
         //  error message
         if ((emptyFile == true) || (noFile == true)) {
            
            // Look in SYS CONF for the appropriate error message to print
            errorMessage = new StringBuffer();
            if (emptyFile == true)
               sysConfTag = "LPNODATA_MSG";
            else
               sysConfTag = "LPNOFILE_MSG";
            if (DataLib.getNtcssSysConfData(sysConfTag, errorMessage) != 0) {
               errorMessage.setLength(0);
               if (emptyFile == true)
                  errorMessage.append("No Data To Print");
               else
                  errorMessage.append("No File To Print");
            }
            
            // Create the temporary file
            tmpFileDescp = File.createTempFile(LP_TEMP_PREFIX, null, 
                                            new File(Constants.NTCSS_LOGS_DIR));                                                      
            
            // Write the error message to the temporary file
            outputStream = new FileWriter(tmpFileDescp);
            outputStream.write("============ NT-CSS II ===========\n");
            outputStream.write("Printing Problem Detected:\n");
            outputStream.write("    User        : " + userName + "\n");
            outputStream.write("    Application : " + appName + "\n");
            outputStream.write("    Filename    : " + filename + "\n");
            outputStream.write("    Error       : " + errorMessage.toString() + "\n");
            outputStream.write("==================================\n");
            outputStream.write("\n");                        
            outputStream.close();
            
            return tmpFileDescp.getAbsolutePath();
         }
         else
            return filename;
      }
      catch (Exception exception) {         
         Log.excp("NtcssLp", "checkFile", exception.toString());
         return filename;
      }     
   }
   
   /**
    * Removes any temporary files that were created for empty or 0 length
    *  files that were requested to be printed
    */
   private void removeTempFiles(String fileListStr)
   {
      StringTokenizer tokenizer;
      String filename;
      File fileDescp;
      
      try {
         
         // The given list of files is comman delimited
         tokenizer = new StringTokenizer(fileListStr, ",");
         while (tokenizer.hasMoreTokens() == true) {
            filename = tokenizer.nextToken();
            
            // See if this was one of the temporary files created
            if (filename.indexOf(LP_TEMP_PREFIX) != -1) {
               
               fileDescp = new File(filename);
               if ((fileDescp.exists() == true) && (fileDescp.length() > 0))
                  fileDescp.delete();
            }
         }
      }
      catch (Exception exception) {
         Log.excp("NtcssLp", "removeTempFiles", exception.toString());
      }
   }
   
   /**
    * Sends the text files off to be printed to the specified local printer. 
    *  The fileListStr parameter is a comma delimited list of files to be
    *  printed. The optionListStr is a comma delimited list options that are
    *  specific to the platform. Returns true if the operation is successful.
    */
   private native boolean printTextFilesToLocalPrinter(String printerName, 
                     String fileListStr, String optionListStr, int numCopies,
                     String filePtrOutputFile, boolean printBanner,
                     boolean portraitOrientation, int paperSize, String login,
                     String classificationStr, String appName);                
}
