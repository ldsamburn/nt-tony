/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.libs;
import ntcsss.log.Log;
import ntcsss.distmgr.*;
import java.sql.Connection;
import java.sql.Statement;
import java.sql.ResultSet;
import java.io.*;
import ntcsss.util.FileOps;
import java.net.Socket;
import ntcsss.database.DatabaseConnectionManager;
import COMMON.INI;
import COMMON.FileString;
import COMMON.FTP;
import java.util.*;
import ntcsss.libs.structs.AdapterStruct;
import ntcsss.libs.structs.PrinterStruct;

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
public class PrintLib
{
    
    static {
        
        // Load the native library
        try {
            System.load(Constants.NATIVE_LIBRARY);
        }
        catch (Exception exception) {
            Log.excp("PrintLib", "static initializer",
            exception.toString());
        }
    }

    /* Retrieves the list of printers from the system, and returns the list
       as a comma delimited list. 
   */
    
 //==============================================================================
    public static native String getSystemPrinterList();
 //==============================================================================
    
 //==============================================================================
    public static native boolean addPrinter(String printername,
                                           String drivername, 
                                           String driver,
                                           String datafile,
                                           String uifile,
                                           String helpfile,
                                           String monitorname,
                                           String monitor,
                                           String copyfiles,
                                           String ntcssPrinterType,
                                           String port,
                                           String driverPath);
 //==============================================================================
    
    
 //==============================================================================
    public static native boolean add2KPrinter (String printername, String filename, 
                                               String drivername, String printingMethod);
 //==============================================================================
                                           
                                           
 //==============================================================================
    public static native boolean deletePrinter (String printername, 
                                                String printingMethod);
 //==============================================================================
                                           
//==============================================================================
    public static int getSystemPrinterList(String host, StringBuffer printerList)
//==============================================================================
{
    Socket socket;
    BufferedInputStream inputStream;
    int input;

    // Try to make a connection to the printer list daemon on the specified host
    try {
        socket = new Socket(host, Constants.PRT_LISTD_PORT);
        inputStream = new BufferedInputStream(socket.getInputStream());
    }
    catch (Exception connectionException) {
        return -1;
    }

    try {

        // Store in the given buffer whatever is read off the socket
        while ((input = inputStream.read()) != -1)
            printerList.append((char)input);

        // Close the resources
        inputStream.close();
        socket.close();

        return 1;
    }
    catch (Exception exception) {
        return -2;
    }
}
    

//==============================================================================  
    protected static String coverNull(String field)
//==============================================================================
    {
        return field==null ? "" : field;
    }

//==============================================================================
    protected static boolean dbFromINI(String section,String iniFilename,
                                   Statement statement, Connection connection)
//==============================================================================
{
//puts the ntprint.ini section into the DB 

    INI ini = new INI();

    try {
        // Get all the print driver's files
        Object[] sqlItems = new Object[3];
        sqlItems[0] = section;
        sqlItems[1] = ini.GetProfileString(section, "MONITORNAME", "",
        iniFilename);
        
        Log.debug("PrintLib", "dbFromINI", StringLib.SQLformat("INSERT INTO " +
        "print_drivers VALUES( " + Constants.ST  + "," +
        Constants.ST + ")", sqlItems)); //debug 
        
        if(statement.executeUpdate(StringLib.SQLformat("INSERT INTO " +
        "print_drivers VALUES( " + Constants.ST  + "," +
        Constants.ST + ")", sqlItems))!=1)
            return false;
        
         if (section.endsWith(Constants.W2K_PRT_DRVR_EXT)) 
             return true; //if 2K driver we just add driver name & blow out of here
        
        //driver file
        sqlItems[1] = new Integer(Constants.DRIVER_DRIVER_TYPE);
        sqlItems[2] = ini.GetProfileString(section, "DRIVER", "", iniFilename);

        statement.executeUpdate(StringLib.SQLformat("INSERT INTO " +
        "print_driver_files VALUES( " + Constants.ST + "," +
        Constants.ST  + "," + Constants.ST + ")", sqlItems));

        //UI file
        sqlItems[1] = new Integer(Constants.UI_DRIVER_TYPE);
        sqlItems[2] = ini.GetProfileString(section, "UI", "", iniFilename);

        statement.executeUpdate(StringLib.SQLformat("INSERT INTO " +
        "print_driver_files VALUES( " + Constants.ST + "," +
        Constants.ST  + "," + Constants.ST + ")", sqlItems));

        //DATA file
        sqlItems[1] = new Integer(Constants.DATA_DRIVER_TYPE);
        sqlItems[2] = ini.GetProfileString(section, "DATA", "", iniFilename);

        statement.executeUpdate(StringLib.SQLformat("INSERT INTO " +
        "print_driver_files VALUES( " + Constants.ST + "," +
        Constants.ST  + "," + Constants.ST + ")", sqlItems));

        //HELP file
        sqlItems[1] = new Integer(Constants.HELP_DRIVER_TYPE);
        sqlItems[2] = ini.GetProfileString(section, "HELP", "", iniFilename);

        statement.executeUpdate(StringLib.SQLformat("INSERT INTO " +
        "print_driver_files VALUES( " + Constants.ST + "," +
        Constants.ST  + "," + Constants.ST + ")", sqlItems));

        //MONITOR file
        sqlItems[1] = new Integer(Constants.MONITOR_DRIVER_TYPE);
        sqlItems[2] = ini.GetProfileString(section, "MONITOR", "", iniFilename);
        
        Log.debug("PrintLib", "dbFromINI", StringLib.SQLformat("INSERT INTO " +
        "print_drivers VALUES( " + Constants.ST  + "," +
        Constants.ST + ")", sqlItems)); //debug 
        
        if(sqlItems[2].toString().length()!=0)
            statement.executeUpdate(StringLib.SQLformat("INSERT INTO " +
            "print_driver_files VALUES( " + Constants.ST + "," +
            Constants.ST  + "," + Constants.ST + ")", sqlItems));

        //COPY FILES stored as COPY_FILE_1 ... n
        sqlItems[1] = new Integer(Constants.NO_DRIVER_TYPE);
        String copyfile = "";
        for (int x=1;;x++) {
            copyfile=ini.GetProfileString(section,Constants.COPY_FILE_PREFIX
            + "_" + x,"",iniFilename);
            if(copyfile.equals(""))
                break;
            sqlItems[2] = copyfile;

            statement.executeUpdate(StringLib.SQLformat("INSERT INTO " +
            "print_driver_files VALUES( " + Constants.ST + "," +
            Constants.ST  + "," + Constants.ST + ")", sqlItems));
        }

        return true;

    }

    catch (Exception exception){
        Log.excp("PrintLib", "dbFromINI",
        exception.toString());
        return false;
    }

} 
    
//==============================================================================
    public static boolean createPrinterIni(String strFileName) {
//==============================================================================
        String SQLString;
        Connection connection=null;
        Statement statement=null;
        ResultSet resultSet=null;
        String adapterIP, adapterHostname;
        
        try {
            
            // Get a connection from the database
            if((connection = DatabaseConnectionManager.getConnection())==null) {
                Log.warn("PrintLib", "CreatePrinterIni",
                ErrorCodes.getErrorMessage(ErrorCodes.ERROR_CANT_OPEN_DB_FILE));
                return false;
            }
            
            //compensate for SQL inadequcy wif HashSet for Printers associated to Otypes
             HashSet otypes = new HashSet(); //for applying access 
             
             statement = connection.createStatement();
             resultSet = statement.executeQuery("SELECT DISTINCT printer_name " +
            "FROM output_printers");
             while(resultSet.next())
                 otypes.add(resultSet.getString(1));
           
            resultSet = statement.executeQuery("SELECT * FROM printers " +
            "ORDER BY printer_name");
            
            //... open file even if there are no printers
            PrintWriter out = new PrintWriter(new BufferedWriter(
            new FileWriter(strFileName)));
            
            if(resultSet.first()) {
                resultSet.beforeFirst();
                //Write out printer=printer
                out.println("[PRINTERS]");
                while(resultSet.next())
                    out.println(resultSet.getString(1) + "=" + resultSet.getString(1));
                
                out.println();
                resultSet.beforeFirst();
                
                while(resultSet.next()) {
                    
                    out.println("[" + resultSet.getString("printer_name") + "]");
                    out.println("HOSTNAME=NODATA");
                    out.println("LOCATION=" + resultSet.getString("location"));
                    out.println("MAXSIZE=" + resultSet.getString("job_max_bytes"));
                    out.println("CLASS=" + resultSet.getString("printer_type"));
                    out.println("SECURITY=" + resultSet.getString("security_class"));
                    out.println("SUSPEND=" + (resultSet.getBoolean("suspend_requests")
                     ? "1":"0"));
                    out.println("QUEUESTATUS=" + (resultSet.getBoolean("queue_enabled")
                     ? "1":"0"));
                    out.println("PRINTERSTATUS=" + (resultSet.getBoolean("printer_enabled")
                     ? "1":"0"));
                    out.println("ACCESSSTATUS=" + (resultSet.getBoolean("general_access")
                     ? "1":"0"));
                    
                    String strMethod=resultSet.getString("printing_method");
                    
                    if(strMethod != "NTSHARE")
                        out.println("PORT=" + coverNull(resultSet.getString("port_name")));
                    else
                        out.println("PORT=" + resultSet.getString("nt_share_name"));
                    
                    out.println("REDIRECTED=" + coverNull(resultSet.getString("redirection_ptr")));
                    
                    if(strMethod == "NTSHARE") //Adapter IP can be 3 things
                        out.println("ADAPTER_IP=" + coverNull(resultSet.getString("nt_host_name")));
                    else if(strMethod == "SERVER")
                        out.println("ADAPTER_IP=" + coverNull(resultSet.getString("host_ip_address")));
                    else
                        out.println("ADAPTER_IP=" + coverNull(resultSet.getString("adapter_ip_address")));
                    
                    out.println("NTDRIVER="+ resultSet.getString("driver_name"));
                    
                    //for otypes change query to have last field a subquery to get this
                    out.println("OTYPES=" + (otypes.contains(resultSet.getString("printer_name"))
                    ? "t" : ""));
                    out.println("APPEND=" + (resultSet.getBoolean
                               ("file_append_flag") ? "0": "1"));
                    out.println("FILENAME=" + coverNull(resultSet.getString("file_name")));
                    out.println("TYPE=" + resultSet.getString("printing_method"));
                    out.println();
                    
                } //while
            } //IF THERE IS ANY PRINTERS DATA
            
            
            resultSet = statement.executeQuery("SELECT printer_type FROM " + 
            "PRINTER_TYPES ORDER BY printer_type");
            
            out.println("[PRINTCLASS]");
            
            while(resultSet.next())
                out.println(resultSet.getString(1) + "=" +
                resultSet.getString(1));
            out.println();
            //////////////////////////////////////////////////////////////////
            
            //write Apps section
           
            resultSet = statement.executeQuery("SELECT app_title FROM apps ORDER BY app_title");
            out.println("[APPS]");
            
            while(resultSet.next())
                out.println(resultSet.getString(1) + "=" +
                resultSet.getString(1));
            out.println();
            //////////////////////////////////////////////////////////////////
            
            //write [ADAPTERS] section
            SQLString = "SELECT * FROM printer_adapters ORDER BY " +
            "adapter_ip_address";
            
            resultSet = statement.executeQuery("SELECT * FROM printer_adapters "
            + "ORDER BY adapter_ip_address");
            
            if(resultSet.first()) {
                resultSet.beforeFirst();
                out.println("[ADAPTERS]");
                
                //write ip=ip
                while(resultSet.next())
                    out.println(resultSet.getString(1) + "=" +
                    resultSet.getString(1));
                out.println();
                
                //write [ip] section
                resultSet.beforeFirst();
                while(resultSet.next()) {
                    adapterIP = resultSet.getString("adapter_ip_address");
                    out.println("[" + adapterIP + "]");
                    adapterHostname = NDSLib.getAdapterHost(adapterIP);
                    out.println("HOSTNAME=" + 
                       ((adapterHostname == null) ? "None" : adapterHostname));
                    out.println("MACHINE_ADDRESS=" + resultSet.
                    getString("ethernet_address"));
                    out.println("LOCATION=" + resultSet.getString("location"));
                    out.println("TYPE=" + resultSet.getString("type"));
                    out.println();
                }
            }
            
            //write [ADAPTERTYPES] section... TODO should this be required!!!!
            
            resultSet = statement.executeQuery("SELECT * FROM adapter_types " +
            "ORDER BY type");
            
            out.println("[ADAPTERTYPES]");
            
            while(resultSet.next()) {
                SQLString=null;
                
                for(int i=1;i<=resultSet.getInt(2);i++)
                    if(SQLString==null)
                        SQLString = "P" + new Integer(i).toString();
                    else
                        SQLString += ",P" + new Integer(i).toString();
                
                for(int i=1;i<=resultSet.getInt(3);i++)
                    if(SQLString==null)
                        SQLString = "S" + new Integer(i).toString();
                    else
                        SQLString += ",S" + new Integer(i).toString();
              
                out.println(resultSet.getString(1) + "=" + SQLString);
            }
            
            out.println();
            
            //write [HOSTS] host=host section
            
            resultSet = statement.executeQuery("SELECT hostname,host_ip_address "
            + "FROM hosts ORDER BY hostname");
            
            out.println("[HOSTS]");
            
            while(resultSet.next())
                out.println(resultSet.getString(1) + "=" +
                resultSet.getString(1));
            
            out.println();
            resultSet.beforeFirst();
            
            while(resultSet.next()) {
                out.println("[" + resultSet.getString(1) + "]");
                out.println("IP=" + resultSet.getString(2));
                out.println();
            }
            
            out.println();
            
            //write [hostname_printers] printer=printer for SERVER printers
            resultSet.beforeFirst();
            StringBuffer SB = new StringBuffer();
           
            while(resultSet.next()) {
                SB.setLength(0);
                if(getSystemPrinterList(resultSet.getString(1), SB)==1)
                    if(SB.length()!=0 && SB!=null) {
                        out.println("[" + resultSet.getString(1) + "_printers]");
                        StringTokenizer tokenizer = new StringTokenizer(SB.toString());
                        while (tokenizer.hasMoreElements() == true){
                            String temp = tokenizer.nextToken();
                            out.println(temp + "=" + temp);
                        }
                        out.println();
                    }
            }
            
            //write [FILELIST] host=host section
            resultSet = statement.executeQuery("SELECT DISTINCT driver_filename "
            + "FROM print_driver_files ORDER BY driver_filename");
            
            if(resultSet.next())
                out.println("[FILELIST]"); 
            
            resultSet.beforeFirst();
            
            while(resultSet.next())
                out.println(resultSet.getString(1 )+ "=" +
                resultSet.getString(1));
            
            out.println();
            out.close();
            
            return true;
            
        } //try
        
        catch (Exception exception){
            Log.excp("PrintLib", "CreatePrinterIni",
            exception.toString());
            return false;
        }
        
        finally {
            try {
                
                if(resultSet!=null)
                    resultSet.close();
                if(statement!=null)
                    statement.close();
                DatabaseConnectionManager.releaseConnection(connection);
            }
            catch (Exception exception){
                Log.excp("PrintLib", "CreatePrinterIni",
                exception.toString());
            }
        }
    }

//==============================================================================
    public synchronized static boolean extractPrinterIni(String strFileName, String strLogFile)
//==============================================================================

{

    INI ini;
    PrintWriter writer = null;
    
    // The other message NewNTPRinter should already have added
    // drivers to tables which should eliminate the need for this... on master!!!

    String Sections;
    StringTokenizer tokenizer, entryTokenizer;
    String Section;
    PrinterInfo printerInfo;
    AdapterInfo adapterInfo;
    Connection connection = null;
    Statement statement = null;
    ResultSet resultSet = null;
    boolean callDm=false;
    int applies = 0;
    String thisHost=StringLib.getHostname();
    ArrayList apps = new ArrayList();  //Use to only do changes for apps
    //on this box

    boolean isMaster = DataLib.isMasterServer();
       
    try {

        //Open files
        writer = new PrintWriter(new FileWriter(
        strLogFile, false));
        ini = new INI();
        
        ////////////////////////////////////////////////
        // Get a database connection
        connection = DatabaseConnectionManager.getConnection();
        if (connection == null) {
            Log.warn("PrintLib","ExtractPrinterIni",
            "Unable to get DB connection for host update");
            return false;
        }

        //If NOT a MASTER SERVER GET A LIST OF apps on this HOST
        //&& call importPrtDrivers to put ntprint.ini into tables
        //this file is put there by the SendToDM from master for any 
        //opps that affected apps on this host
        if(!isMaster) {

            Object Items[] = new Object[1];
            Items[0] = thisHost.toUpperCase();

            statement = connection.createStatement();
            resultSet = statement.executeQuery(StringLib.SQLformat
            ("SELECT app_title FROM apps WHERE upper(hostname) = " +
            Constants.ST +  " ORDER BY app_title", Items));

            while(resultSet.next()) 
                apps.add(resultSet.getString(1));
            
            if(FileOps.Stat(Constants.NT_INI_FILE)) {
                
                if(!PrintLib.importPrtDrivers(Constants.NT_INI_FILE, strFileName, null, connection))
                    Log.warn("PrintLib","ExtractPrinterIni","importPrtDrivers failed!!!");
            }
            else
                Log.warn("PrintLib","ExtractPrinterIni","Did not find " + Constants.NT_INI_FILE  + " !!!");
            
            Log.debug("PrintLib","ExtractPrinterIni","Not master... Apps on this box size -> " + apps.size());
            
            
        }

        // Get all the items in the PRINTERS section
        if((Sections=ini.GetProfileSection("PRINTERS",strFileName))!=null) {
            tokenizer = new StringTokenizer(Sections, INI.STRINGS_DELIMITER);

            // Parse through the items
            while (tokenizer.hasMoreElements() == true) {

                // Find the section that matches the first item
                entryTokenizer = new StringTokenizer(tokenizer.nextToken(),
                INI.TAG_VALUE_DELIMITER);

                Section = entryTokenizer.nextToken();

                // Get the information about the current Printer
                printerInfo = new PrinterInfo();

                Log.debug("PrintLib","ExtractPrinterIni",
                "Loading printer info for <" + Section + ">");

                if (printerInfo.load(ini, Section, strFileName) == false) {
                    Log.warn("PrintLib","ExtractPrinterIni",
                    "Error loading printer info for <" + Section + ">");
                    continue;
                }

                // Commit the operation to the DB
                if(isMaster || printerInfo.isAppOnHost(apps) || 
                   printerInfo.isPrinterOnHost(connection)) {
                    
                     Log.debug("PrintLib","ExtractPrinterIni",
                        "Commiting to the DB ");
                     
                    if (printerInfo.processPrinters(connection,apps) == false) {
                        Log.warn("PrintLib","ExtractPrinterIni",
                        "Error processing info for Printer <" + printerInfo.PRINTERNAME + ">");
                        //write failure to log
                        printerInfo.outputPrinter(writer, false);
                        continue;
                    }

                    else {
                        
                        Log.debug("PrintLib","ExtractPrinterIni",
                        "Successful processing for printer  <" + Section + ">");
                        //write success to log...set DM flag
                        writer.println("Successful processing for printer  <" 
                        + Section + "> on host " + thisHost);
                        printerInfo.outputPrinter(writer, true);
                        //set flag for if DM is neccessary
                        if(isMaster)
                            callDm=true;
                        
                        applies++;
                    }
                }
                
                else //this could be a printer that had it's LAST access removed from this box
                    Log.debug("PrintLib","ExtractPrinterIni", "<" + Section + "> didn't apply to this host");
                    

                Log.debug("PrintLib","ExtractPrinterIni","Processed printers");
            }
        }


        // Get all the items in the ADAPTERS section

        if((Sections=ini.GetProfileSection("ADAPTERS",strFileName))!=null) {
            tokenizer = new StringTokenizer(Sections, INI.STRINGS_DELIMITER);

            // Parse through the items
            while (tokenizer.hasMoreElements() == true) {


                // Find the section that matches the first item
                entryTokenizer = new StringTokenizer(tokenizer.nextToken(),
                INI.TAG_VALUE_DELIMITER);
                Section = entryTokenizer.nextToken();

                // Get the information about the current host
                adapterInfo = new AdapterInfo();

                Log.debug("PrintLib","ExtractPrinterIni",
                "Loading adapter info for <" + Section + ">");

                if (adapterInfo.load(ini, Section, strFileName) == false) {
                    Log.warn("PrintLib","ExtractPrinterIni",
                    "Error loading adapter info for <" + Section + ">");
                    continue;
                }

                // Commit the operation to the DB

                if (adapterInfo.processAdapters(connection) == false) {
                    Log.warn("PrintLib","ExtractPrinterIni",
                    "Error processing info for <" + adapterInfo.IP + ">");
                    adapterInfo.outputAdapter(writer,true);
                    continue;
                }
                else {
                    
                    adapterInfo.outputAdapter(writer,true);
                    applies++;
                }

                Log.debug("PrintLib","ExtractPrinterIni","Processed adapter");

            }
        }
        
        if(applies==0) {   //if nothing worked put something in log file
            
             Log.warn("PrintLib","ExtractPrinterIni",
                    "No changes were applied on " + thisHost);
             
             writer.println("\n\n**********************************************");
             writer.println("Begin response from Host <" + thisHost
             + ">\n\n");
             
             writer.println("No changes were applied to this host\n\n");
              writer.println("End response from Host <" + thisHost
             + ">\n\n");
              
             writer.println("\n\n**********************************************");
            
        }

        if(callDm) 
            sendToDM(thisHost, strFileName, writer, connection);

        return applies!=0;
        
    }

    catch (Exception exception){
        Log.excp("PrintLib", "ExtractPrinterIni",
        exception.toString());
        return false;
    }

    finally {

        try {
            writer.close();

            if(resultSet!=null)
                resultSet.close();

            if(statement!=null)
                statement.close();

            if(connection!=null)
                DatabaseConnectionManager.releaseConnection(connection);
        }
        catch (Exception exception){
            Log.excp("PrintLib", "ExtractPrinterIni",
            exception.toString());
            return true;
        }
    }
}
    
    
//==============================================================================
    public synchronized static boolean extractOtypeINI(String fileName,String logFile)
//==============================================================================

    {
        
        Connection connection=null;
        StringTokenizer tokenizer, entryTokenizer;
        INI ini = new INI();
        OtypeInfo otypeInfo;
        StringBuffer logBuffer;     
        boolean errorOccurred = false;
        FileWriter writer;
        
        try {
            // Get a database connection
            connection = DatabaseConnectionManager.getConnection();
            if (connection == null) {
                Log.warn("PrintLib", "extractOtypeINI",
                         "Unable to get DB connection for host update");
                return false;
            }         
            
            String section=null;
            
            tokenizer = new StringTokenizer(ini.GetProfileSection("OTYPE",fileName), 
            INI.STRINGS_DELIMITER);
            
            // Parse through the items      
            logBuffer = new StringBuffer(0);
            while (tokenizer.hasMoreElements() == true) {
                
                // Find the section that matches the first item
                entryTokenizer = new StringTokenizer(tokenizer.nextToken(), 
                INI.TAG_VALUE_DELIMITER);
                section = entryTokenizer.nextToken();
                
                // Get the information about the current host
                otypeInfo = new OtypeInfo();
                if (otypeInfo.load(ini, section, fileName) == false) {
                    Log.warn("PrintLib", "extractOtypeINI",
                             "Error loading host info for <" + section + ">");
                    continue;
                }
                
                // Perform database operation
                if (otypeInfo.processOtypes(connection, logBuffer) == false) {
                    Log.warn("PrintLib", "extractOtypeINI",
                             "Error processing info for <" + otypeInfo.title + ">");
                    errorOccurred = true;
                    break;
                }
            }
            
            // Dump out the log file
            writer = new FileWriter(logFile);
            
            writer.write("DATE/TIME IS " + StringLib.timeStrToReadable(StringLib.getTimeStr())
                         + "\n\n");
            
            if (errorOccurred == false) {
               writer.write(logBuffer.toString());
               writer.write("ALL CHANGES APPLIED SUCCESSFULLY\n");
            }
            else {
               writer.write("APPLYING CHANGE FAILED BECAUSE OF FAILED DATABASE ACCESS\n");
               writer.write("STOPPED TRYING TO APPLY ANY MORE CHANGES\n");
            }
            
            writer.close();
            
            return true;            
        }
        
        catch (Exception exception){
            Log.excp("PrintLib", "extractOtypeINI",exception.toString());
            return false;
        }
        
        finally {
            
                DatabaseConnectionManager.releaseConnection(connection);
        }
        
    }
    
//==============================================================================
public static boolean reconcile()
//==============================================================================
{
    prtRecon pRec = new prtRecon();
    return pRec.reconcile();
}

//==============================================================================  
    public static boolean createOtypesIni(String strFileName)
//==============================================================================
    {
       Connection connection = null;
       Statement statement = null;
       Statement statement2 = null;
       ResultSet resultSet = null;
       ResultSet resultSet2 = null;
       final String method_name = "createOtypesIni";
       Object sqlItems[];
       INI iniFile;
       Hashtable appList;
       String appTitle;
       int counter;
       String section,section2;   
       String emptySection[] = {""};
       String appPrinterIndices;       
       Vector appPrinters;
       Enumeration apps;      
       String printer;       
       
       try {
          
          // Get a connection from the database
          if((connection = DatabaseConnectionManager.getConnection())==null) {
             Log.warn("PrintLib", method_name,
             ErrorCodes.getErrorMessage(ErrorCodes.ERROR_CANT_OPEN_DB_FILE));
             return false;
          }          
          statement = connection.createStatement();
          
          iniFile = new INI();
          iniFile.WriteProfileSection("OTYPE", emptySection, strFileName);
          
          // Get a list of all the applications
          sqlItems = new Object[3];
          sqlItems[0] = Constants.NTCSS_APP;
          resultSet = statement.executeQuery(StringLib.SQLformat("SELECT " +
               "app_title FROM apps WHERE app_title != " + Constants.ST, sqlItems)); 
                    
          appList = new Hashtable();
          while (resultSet.next() == true) {
             appTitle = resultSet.getString("app_title");             
             iniFile.WriteProfileString("OTYPE", appTitle + "_PROGROUP",
                                        appTitle + "_PROGROUP", strFileName); 
             
             if (statement2 == null)
                statement2 = connection.createStatement();
             
             // Store off all the application printers for each application
             sqlItems[0] = appTitle;
             resultSet2 = statement2.executeQuery(StringLib.SQLformat("SELECT " +
                           "printer_name FROM app_printers WHERE app_title = " + 
                           Constants.ST, sqlItems));
             
             appPrinters = new Vector();
             while(resultSet2.next() == true)
                appPrinters.add(resultSet2.getString("printer_name"));
             
             appList.put(appTitle, appPrinters);
          }
          
          // Write out information about each output_type for the applications
          apps = appList.keys();
          while (apps.hasMoreElements() == true) {         
             appTitle = (String)apps.nextElement();
             appPrinters = (Vector)appList.get(appTitle);
             
             section = appTitle + "_PROGROUP";
             iniFile.WriteProfileSection(section, emptySection, strFileName);
             
             // Get a list of all the application's output_types
             sqlItems[0] = appTitle;
             resultSet = statement.executeQuery(StringLib.SQLformat("SELECT " +
               "* FROM output_types WHERE app_title = " + Constants.ST, sqlItems));
             
             // Write out information about each output_type
             counter = 1;
             while (resultSet.next() == true) {
                section2 = appTitle + "_OTYPE_" + counter;
                iniFile.WriteProfileString(section, section2, section2, strFileName);
                
//                iniFile.WriteProfileSection(section2, emptySection, strFileName);
                
                iniFile.WriteProfileString(section2, "TITLE", resultSet.getString("otype_title"),
                                           strFileName);
                iniFile.WriteProfileString(section2, "DESCRIPTION",
                                    resultSet.getString("description"), strFileName);
                
                // Get the output type's default printer (if there is one)
                sqlItems[0] = resultSet.getString("otype_title");
                sqlItems[1] = appTitle;
                sqlItems[2] = new Boolean(true);
                resultSet2 = statement2.executeQuery(StringLib.SQLformat("SELECT " +
                     "printer_name FROM output_printers WHERE otype_title = " + 
                     Constants.ST + " AND app_title = " + Constants.ST + 
                     " AND default_printer = " + Constants.ST, sqlItems));
                if (resultSet2.next() == false)
                   iniFile.WriteProfileString(section2, "DEFAULTPRT",
                                              "", strFileName);
                else
                   iniFile.WriteProfileString(section2, "DEFAULTPRT",
                                              resultSet2.getString("printer_name"), 
                                              strFileName);
                                
                iniFile.WriteProfileString(section2, "BATCH", 
                   resultSet.getBoolean("batch_flag") ? "Yes" : "No", strFileName);
                
                // For each output_printer associated with the output_type, 
                //  find the index for that printer in the app's appPrinter
                //  list. The comma delimited list of indices is stored in the
                //  PRTLIST field
                appPrinterIndices = null;                
                if (statement2 == null)
                   statement2 = connection.createStatement();
                   
                sqlItems[0] = resultSet.getString("otype_title");
                sqlItems[1] = appTitle;
                resultSet2 = statement2.executeQuery(StringLib.SQLformat("SELECT " + 
                      "printer_name FROM output_printers WHERE otype_title = " + 
                      Constants.ST + " AND app_title = " + Constants.ST, sqlItems));
                while (resultSet2.next() == true) {
                   
                   // Lookup the index of the output_printer in the list of
                   //  app_printers
                   printer = resultSet2.getString("printer_name");
                   
                   for (int i = 0; i < appPrinters.size(); i++) {
                      
                      if (printer.equals((String)appPrinters.elementAt(i)) == false)
                         continue;
                      
                      if (appPrinterIndices == null)
                         appPrinterIndices = String.valueOf(i + 1);                                             
                      else
                         appPrinterIndices += "," + String.valueOf(i + 1);                         
                   }
                }
                                
                iniFile.WriteProfileString(section2, "PRTLIST", appPrinterIndices, 
                                           strFileName);
          
                counter++;
             }
          }
          
          // Write out information about each application's printers
          iniFile.WriteProfileSection("PRINTERS", emptySection, strFileName);
          apps = appList.keys();
          while (apps.hasMoreElements() == true) {         
             appTitle = (String)apps.nextElement();
             appPrinters = (Vector)appList.get(appTitle);                          
             
             iniFile.WriteProfileString("PRINTERS", appTitle + "_PRTS", 
                                        appTitle + "_PRTS", strFileName);
             
             iniFile.WriteProfileSection(appTitle + "_PRTS", emptySection, strFileName);
             
             for (int i = 0; i < appPrinters.size(); i++) {
                
                sqlItems[0] = (String)appPrinters.elementAt(i);
                resultSet = statement.executeQuery(StringLib.SQLformat("SELECT " +
                   "* FROM printers WHERE printer_name = " + Constants.ST, sqlItems)); 
                         
                resultSet.next();

                section = resultSet.getString("printer_name");
                
                iniFile.WriteProfileString(appTitle + "_PRTS", 
                                           section, section, strFileName);                                                                                                  
                
//                iniFile.WriteProfileSection(section, emptySection, strFileName);
                iniFile.WriteProfileString(section, "PRINTER", section, strFileName);
                iniFile.WriteProfileString(section, "SECURITY",
                                    Constants.getSecurityString(resultSet.getInt("security_class")),
                                    strFileName);
                iniFile.WriteProfileString(section, "LOCATION", 
                                           resultSet.getString("location"), strFileName);                                                    
                iniFile.WriteProfileString(section, "MAXSIZE", 
                                           resultSet.getString("job_max_bytes"), strFileName);
                iniFile.WriteProfileString(section, "CLASS", resultSet.getString("printer_type"),
                                           strFileName);
                iniFile.WriteProfileString(section, "SUSPEND",
                                    resultSet.getBoolean("suspend_requests") ? 
                                    "Suspend Requests" : "Permit All Requests",
                                    strFileName);
             }
          }
        
//          iniFile.WriteProfileSection("BATCH", emptySection, strFileName);
          iniFile.WriteProfileString("BATCH", "VALUES", "Yes,No", strFileName);

          iniFile.FlushINIFile(strFileName);
          
          return true;
       }
       catch (Exception exception) {
          Log.excp("PrintLib", method_name,exception.toString());
          return false;
       }        
       finally {
          try {                
                if(resultSet!=null)
                    resultSet.close();
                if(statement!=null)
                    statement.close();
                if(resultSet2!=null)
                    resultSet2.close();
                if(statement2!=null)
                    statement2.close();                
                if (connection != null)
                   DatabaseConnectionManager.releaseConnection(connection);
          }
          catch (Exception exception){
             Log.excp("PrintLib", method_name, exception.toString());
             return true;
          }
        }
    }        

    //==========================================================================
       private static void sendToDM(String localHost, String fn,
                                 PrintWriter logFile, Connection connection)
    //==========================================================================
    {
        
        //... only called on master
        //this handles all applies in 1 hit...
        
        INI ini=new INI();
        String currentHost;
        ArrayList hostList= new ArrayList();
        
        try {
            
            StringTokenizer tokenizer = new StringTokenizer
            (ini.GetSectionValues("APPLY_HOSTS",fn));
            while (tokenizer.hasMoreElements() == true) {
                currentHost = tokenizer.nextToken();
                if(currentHost.equalsIgnoreCase(localHost))
                    continue;
                
                //////////////////////////////////////////////////////
                //Get the HOST_PRINTERS section values... and go through these
                StringTokenizer tokenizer2 = new StringTokenizer
                (ini.GetSectionValues(currentHost + "_PRINTERS",fn));
                while (tokenizer2.hasMoreElements() == true) {
                    
                    ////////////////////////////////////////////////
                    // Get the information about the current Printer
                    PrinterInfo printerInfo = new PrinterInfo();
                    
                    if (printerInfo.load(ini,tokenizer.nextToken(),fn) == false)
                        continue;
                    
                    //need to make a UNIQUE HOST LIST & ONLY SEND RELEVENT DRIVERS
                    if((printerInfo.OPERATION.equalsIgnoreCase("DELETE")))
                        if(!hostList.contains(currentHost))
                            hostList.add(currentHost);
                        else {
                            
                            if(sendDriverFiles(currentHost,printerInfo.NTDRIVER,
                            connection)) //need to send drivers evreytime  
                                        // && NTPRINT.INI ONCE!!!
                                if(!hostList.contains(currentHost)) {
                                    if(FTP.put(currentHost,Constants.NT_INI_FILE,
                                        Constants.NT_INI_FILE, true, false)) 
                                       hostList.add(currentHost);
                                }
                        }
                }
                
            }
            
            if(hostList.size()==0)
                return;
            
            //else... send to DMs
            
            // Open the given file and get ready to read in the contents
            FileString fs = new FileString(fn);
            
            DPResults  results;
            
            results = DistributionManager.sendMessage(
            StringLib.padString("DPPROCESSPRTCHANGES",
            Constants.MAX_CMD_LEN, '\0')
            + fs.toString(),  // reads in the INI file
            hostList,
            Constants.CMD_SVR_PORT, false,
            Constants.DP_USERADMINCHANGES_TIMEOUT);
            
            if (results == null) {
                String hosts = "";
                for(int i=0;i<hostList.size();i++) {
                    if(hosts.length()==0)
                        hosts += ",";
                    hosts += hostList.get(i);
                }
                
                Log.error("PrintLib", "callDM",
                "Could not forward the user changes to the other servers!");
                logFile.println("WARINING: None of the specified servers have responded: "
                + "<" + hosts + ">");
                return;
            }
            
            //Print out the host responses
            String  host_response;
            for (int i = 0; i < hostList.size(); i++) {
                
                host_response = results.getResult((String)hostList.get(i));
                
                logFile.println("\n******** Begin Response From Server: " +
                (String)hostList.get(i) + " ********\n\n");
                
                // Find out which hosts responded.
                if (host_response == null) {
                    logFile.println("STATUS: Failed, No Log file available\n");
                    continue;   // Check the next host's response..
                }
                
                // See if we got a good response
                if (host_response.substring(0, Constants.MAX_CMD_LEN)
                .compareTo("GOODDPPROCPRTADMIN") == 0) {
                    
                    if (host_response.length() > Constants.MAX_CMD_LEN) {
                        logFile.println("STATUS: Success, Log file follows\n");
                        logFile.println(host_response.substring(Constants.MAX_CMD_LEN));
                    }
                    
                    else
                        logFile.println("STATUS: Success, No Log file follows\n\n");
                    
                    continue;
                }
                
                else
                    if (host_response.substring(0, Constants.MAX_CMD_LEN)
                    .compareTo("BADDPPROCPRTADMIN") == 0) {
                        
                        if (host_response.length() > Constants.MAX_CMD_LEN) {
                            logFile.println("STATUS: Failed, Log file follows\n");
                            logFile.println(host_response.substring(Constants.MAX_CMD_LEN));
                        }
                        
                        else
                            logFile.println("STATUS: Failed, No Log file follows\n\n");
                        
                        continue;
                    }
                    else
                        if (host_response.substring(0, Constants.MAX_CMD_LEN)
                        .compareTo("NORPLYDPPROCPRTADMN") == 0) {
                            
                            logFile.println("STATUS: Server is not responding \n");
                            continue;
                        }
                        else
                            logFile.println("STATUS: Failed unknown response \n");
            } //for
        }
        
        catch (Exception exception) {
            Log.excp("PrintLib", "sendToDM",exception.toString());
        }
    }

//==========================================================================
    private static boolean sendDriverFiles(String host, String driverName,
                                           Connection connection)
//==========================================================================


{

    Statement statement=null;
    ResultSet resultSet=null;

    try {

        Object[] Items = new Object[1];
        Items[0] = driverName;

        statement = connection.createStatement();

        resultSet = statement.executeQuery(StringLib.SQLformat("SELECT " +
        "nt_monitor_name FROM print_drivers WHERE driver_name = " +
        Constants.ST ,Items));

        String monitorName = resultSet.getString(1);

        resultSet = statement.executeQuery(StringLib.SQLformat("SELECT " +
        "driver_filename FROM print_driver_files WHERE driver_name = "
        + Constants.ST, Items));

        String fn = "";

        while(resultSet.next()) {
            fn = Constants.NTCSS_NTDRIVER_DIR + "/" + resultSet.getString(1);
            if (!FTP.put(host, fn, fn, false, false)){
                Log.error("PrintLib", "sendDriverFiles", "Failed to push " +
                "Print Driver <" + fn + "> to host <" + host + ">");
                return false;
            }
        }

        //Send NTPRINTINI 
        return true;

    }
    catch (Exception exception) {
        Log.excp("PrintLib", "sendDriverFiles", exception.toString());
        return false;
    }

    finally {
        try {
            if(resultSet!=null)
                resultSet.close();
            if(statement!=null)
                statement.close();
        }
        catch (Exception exception){
            Log.excp("PrintLib", "sendDriverFiles",
            exception.toString());
            return true;
        }

    }
}

   /*
    * Exports the contents of the drivers database tables to an INI file in the
    *  "ntprint.ini" format. If an exception occurs, the exception is stored 
    *  in the given StringBuffer, and false is returned. Only happens on Master
    *  in reponse to a NEWNTPRINTER MSG
    */
 //============================================================================   
   public static boolean exportPrtDrivers(String iniFilename,
                                          StringBuffer exceptionStr)
 //============================================================================                                          
   { 
      Connection connection;
      Statement statement;
      ResultSet resultSet;                               
      INI iniFile;      
      Vector driverNames;
      String driverName;
      Object sqlItems[];
      int copyFileIndex;
      int type;      
      File fileDescp;
      
      try {
         
         // If the INI file exists, delete it
         fileDescp = new File(iniFilename);
         if (fileDescp.exists() == true)
            fileDescp.delete();
                                    
         // Create the "export" file for writing to it                  
         iniFile = new INI();
         
         // Get a connection from the database
         if((connection = DatabaseConnectionManager.getConnection())==null) {
            Log.error("PrintLib", "exportPrtDrivers", "Can't get database connection");    
            
            // Record error
            if (exceptionStr != null) {
               exceptionStr.setLength(0);
               exceptionStr.append("Can't get database connection");
            }
            
            return false;
         }
         statement = connection.createStatement();
         
         // Write out the "PRINTERS" section of the INI file
         resultSet = statement.executeQuery("SELECT driver_name FROM print_drivers");
         driverNames = new Vector();
         while (resultSet.next() == true) {                    
            driverName = resultSet.getString("driver_name");
            driverNames.add(driverName);
            iniFile.WriteProfileString("PRINTERS", driverName, driverName, 
                                       iniFilename);                        
         }
         resultSet.close();                  
         
         // Write out a section for each print driver
         sqlItems = new Object[1];
         for (int i = 0; i < driverNames.size(); i++) {
            
            // Get all the print driver's files
            sqlItems[0] = (String)driverNames.elementAt(i);            
            resultSet = statement.executeQuery(StringLib.SQLformat("SELECT " + 
                        "type, driver_filename FROM print_driver_files WHERE " + 
                        "driver_name = " +  Constants.ST, sqlItems));         
            copyFileIndex = 1;
            while (resultSet.next() == true) {
                              
               type = resultSet.getInt("type");
               if (type == Constants.DRIVER_DRIVER_TYPE)
                  iniFile.WriteProfileString((String)sqlItems[0], "DRIVER", 
                                       resultSet.getString("driver_filename"), 
                                       iniFilename);
               else if (type == Constants.UI_DRIVER_TYPE)
                  iniFile.WriteProfileString((String)sqlItems[0], "UI", 
                                       resultSet.getString("driver_filename"), 
                                       iniFilename);
               else if (type == Constants.DATA_DRIVER_TYPE)
                  iniFile.WriteProfileString((String)sqlItems[0], "DATA", 
                                       resultSet.getString("driver_filename"), 
                                       iniFilename);
               else if (type == Constants.HELP_DRIVER_TYPE)
                  iniFile.WriteProfileString((String)sqlItems[0], "HELP", 
                                       resultSet.getString("driver_filename"), 
                                       iniFilename);
               else if (type == Constants.MONITOR_DRIVER_TYPE)
                  iniFile.WriteProfileString((String)sqlItems[0], "MONITOR", 
                                       resultSet.getString("driver_filename"), 
                                       iniFilename);
               
               //COPY FILE section WRITEN COPY_FILE_1 ... n
               else if (type == Constants.NO_DRIVER_TYPE)
                  iniFile.WriteProfileString((String)sqlItems[0], 
                            Constants.COPY_FILE_PREFIX + "_" + copyFileIndex++, 
                            resultSet.getString("driver_filename"), 
                            iniFilename);                  
            }
            
            resultSet.close();
            
            // Write out the print driver's monitor name
            resultSet = statement.executeQuery(StringLib.SQLformat("SELECT " + 
                        "nt_monitor_name FROM print_drivers WHERE " + 
                        "driver_name = " +  Constants.ST, sqlItems));
            if ((resultSet.next() == false) || 
                (resultSet.getString("nt_monitor_name") == null))
               iniFile.WriteProfileString((String)sqlItems[0], "MONITORNAME",  
                                          " ", iniFilename);                  
            else
               iniFile.WriteProfileString((String)sqlItems[0], "MONITORNAME", 
                            resultSet.getString("nt_monitor_name"), 
                            iniFilename);                  
            resultSet.close();
         }
         
         // Write out the DRIVERLINK section
         sqlItems[0] = null;
         resultSet = statement.executeQuery(StringLib.SQLformat("SELECT " +
                 "printer_name, driver_name FROM printers WHERE driver_name != " 
                 + Constants.ST, sqlItems));
         while (resultSet.next() == true) 
            iniFile.WriteProfileString("DRIVERLINK", 
                            resultSet.getString("printer_name"), 
                            resultSet.getString("driver_name"), 
                            iniFilename); 
         
         resultSet.close();
         statement.close();
         DatabaseConnectionManager.releaseConnection(connection);
         
         // Write out the contents of this INI data structure
         iniFile.FlushINIFile(iniFilename);                                           
         
         return true;
      }
      catch (Exception exception) {
         
         Log.excp("PrintLib", "exportPrtDrivers", exception.toString());
         
         // Record the exception in the given buffer
         if (exceptionStr != null) {
            exceptionStr.setLength(0);
            exceptionStr.append(exception.toString());
         }
         
         return false;
      }
      
   }
    

   //Called in extract on non-master to import any drivers IT NEEDS to process
   //the CURRENT Apply file. The ntprint.ini in total is dropped by the master
   //and the drivers should have been ftp'd
   

  //============================================================================
   public static boolean importPrtDrivers(String NTPrintINI, String ApplyINI,
   StringBuffer exceptionStr, Connection connection)
   //============================================================================
   
   {
       
       Statement statement = null;
       ResultSet resultset = null;
       String section,flag;
       
       
       class prtdriver {
           
           public String section,printer,driver;
           
           
           prtdriver(INI ini, String sectionname, String fn) {
               
               section = sectionname;
               printer = ini.GetProfileString(sectionname, "PRINTERNAME","", fn);
               driver = ini.GetProfileString(sectionname, "NTDRIVER","", fn);
               
           }
           
           void remove(INI ini, String fn) {
               ini.RemoveProfileString(StringLib.getHostname() + "_PRINTERS" ,section, fn);
               ini.RemoveProfileSection(section, fn);
           }
           
       }
       
       
       try {
           
           statement = connection.createStatement();
           
           INI ntprintini = new INI();
           INI applyini = new INI();
           String drivers=null;
           
           statement = connection.createStatement();
           HashMap driversNeeded = new HashMap();
           HashMap printersMap = new HashMap();
           
           //Get all the drivers that will be needed for this apply from the apply file
           //Maybe keep track & remove any printer from apply that we can't get drivers for
           
           // Get all the PRINTERS & stick them in 1st hash map wrapped in prtDriver Class
           Log.debug("PrintLib", "importPrtDrivers()", StringLib.getHostname().toLowerCase() + "_PRINTERS"); //debug
           if((section=applyini.GetSectionValues(StringLib.getHostname().toLowerCase() + "_PRINTERS", ApplyINI))!=null) {
               StringTokenizer tokenizer = new StringTokenizer(section, INI.STRINGS_DELIMITER);
               while (tokenizer.hasMoreElements() == true) {
                   StringTokenizer tokenizer2 = new StringTokenizer(tokenizer.nextToken(), "=");
                   prtdriver prt = new prtdriver(applyini, tokenizer2.nextToken(), ApplyINI);
                   printersMap.put(prt.printer,prt);
               }
           }
           
           else {
               
               Log.debug("PrintLib", "importPrtDrivers()",
               "Found no printers to apply to this Host");
               return false;
           }
           
           
           //Now loop trough this map & make a unique list of drivers we need to have
           
           prtdriver p = null;
           String sql = null;
           ArrayList tempKeys = new ArrayList(printersMap.keySet());
           for (int i = 0; i < tempKeys.size(); i++) {
               p=(prtdriver)printersMap.get(tempKeys.get(i));
               if(p.driver.length()!=0)
                   driversNeeded.put((String)p.driver, "t");
               
               //make a string of drivers for an IN string to what we have already
               if(sql == null)
                   sql = "'" + p.driver + "'";
               else
                   sql += ",'" + p.driver + "'";
           }
           
           p = null;
           tempKeys = null;
           
           resultset=statement.executeQuery("SELECT driver_name FROM  " +
           "print_drivers WHERE driver_name IN ("  + sql + ")");
           
           
           //here... it's in the DB & we assume the driverfiles are OK
           //set them to found in the driver MAP
           while(resultset.next())
               driversNeeded.put(resultset.getString("driver_name"),"f");
           
           resultset.close();
           
           //Check if we got them all
           boolean bRet = true;
           
           tempKeys = new ArrayList(driversNeeded.keySet());
           for (int i = 0; i < tempKeys.size(); i++) {
               flag=(String)driversNeeded.get(tempKeys.get(i));
               if(flag.equals("t")) {
                   bRet = false;
                   break;
               }
           }
           
           if(bRet) {
               Log.debug("PrintLib", "importPrtDrivers()", "The DB had ALL the Drivers!!!");
               return true;
           }
           
           Log.debug("PrintLib", "importPrtDrivers()", "Getting drivers from NTPRINT!!!"); //debug 
           
           //if we don't have them all we need to get them from Ntprint.ini
           tempKeys = new ArrayList(driversNeeded.keySet());
           for (int i = 0; i < tempKeys.size(); i++) {
               flag=(String)driversNeeded.get(tempKeys.get(i));
               if(flag.equals("t") &&
               dbFromINI((String) tempKeys.get(i), NTPrintINI, statement, connection))
                   driversNeeded.put(tempKeys.get(i),"f");
           }
           
           //Now for any we don't have we need to take them out of the Apply file
           tempKeys = new ArrayList(driversNeeded.keySet());
           for (int i = 0; i < tempKeys.size(); i++) {
               flag=(String)driversNeeded.get(tempKeys.get(i));
               if(flag.equals("t")) {
                   
                   Log.warn("PrintLib", "importPrtDrivers()", "Failed to get this "
                   + " driver -> " + (String) tempKeys.get(i) + " removing affected "
                   + "printers from applyfile");
                   
                   ArrayList tempKeys2 = new ArrayList(printersMap.keySet());
                   
                   for (int x = 0; x < tempKeys2.size(); x++) {
                       p=(prtdriver)printersMap.get(tempKeys2.get(x));
                       p.remove(applyini, ApplyINI);
                       //take them out of map so we know if there is anything to add
                       printersMap.remove(tempKeys2.get(x));
                   }
               }
           }
           
           Log.debug("PrintLib", "importPrtDrivers()", "Adding " + printersMap.size() + " printers");
           return printersMap.size()>0;
           
       }
       catch (Exception exception) {
           
           Log.excp("PrintLib", "importPrtDrivers", exception.toString());
           
           // Record the exception in the given buffer
           if (exceptionStr != null) {
               exceptionStr.setLength(0);
               exceptionStr.append(exception.toString());
           }
           
           return false;
       }
       
       finally {
           try {
               if(resultset!=null)
                   resultset.close();
               if(statement!=null)
                   statement.close();
           }
           catch (Exception exception){
               Log.excp("PrintLib", "importPrtDrivers", exception.toString());
               return true;
           }
           
           
       }
   }

} ///:~   //printlib

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

   class OtypeInfo
   {
 
       public String operation, defprt, batchStr, descrip;
       public String title, group, printers, printerList;
       public boolean batch;
       
      /**
       * Loads a Otype's information from the apply.ini file
       */
//==============================================================================
    public boolean load(INI ini, String section, String iniFile)
//==============================================================================     
{
   final String defVal = "NONE";

   try {

       //section comes in like [change1]...[change...n]
       operation = ini.GetProfileString(section, "OPERATION", defVal,
       iniFile);

       group = ini.GetProfileString(section, "GROUP", defVal, iniFile);

       title = ini.GetProfileString(section, "TITLE", defVal,
       iniFile);

       descrip = ini.GetProfileString(section, "DESCRIPTION", defVal,
       iniFile);

       defprt = ini.GetProfileString(section, "DEFAULTPRT", defVal, 
       iniFile);
       if ((defprt != null) && (defprt.trim().length() == 0))
          defprt = null;

       batchStr = ini.GetProfileString(section, "BATCH", defVal, iniFile);
       if (batchStr.equalsIgnoreCase("Yes") == true)
          batch = true;
       else
          batch = false;

       //printers references another section [changeNprinter]
       printers = ini.GetProfileString(section, "PRINTERS", defVal, 
       iniFile);

       //get new section values delimited by a ~
       printerList=ini.GetSectionValues(printers,iniFile);
       return true;
   }
   catch (Exception exception) {
       Log.warn("OtypeInfo", "load","Could not load adapter INI file");
       return false;
   }
}
       
       /**
        * Updates, inserts or deletes printer record
        */
       
//==============================================================================
    public boolean processOtypes(Connection connection, StringBuffer logBuffer)
//==============================================================================
{
    Statement statement=null;
    Object Items[];
    String SQLstring;
    
    try {
        
        statement = connection.createStatement();
        Items = new Object[5];
        Items[0] = group;
        Items[1] = title;
        
        if(operation.equals("DELETE")) {
/*           
            statement.executeUpdate(StringLib.SQLformat(
            "DELETE FROM output_types WHERE app_title = " +
            Constants.ST + " AND otype_title = " + Constants.ST,
            Items)); //this cascades to output printers
 */
          DataLib.cascadeDelete("output_types", StringLib.SQLformat(
            "app_title = " + Constants.ST + " AND otype_title = " + Constants.ST,
            Items), connection); //this cascades to output printers
          
          logBuffer.append("OUTPUT TYPE DELETED\nNAME:        " + title + "\nAPPLICATION: " + group + "\n\n");
        }
        else if(operation.equals("EDIT")){
            
            Items[0] = group;
            Items[1] = title;
            
            //DELETE OLD OUTPUT PRINTERS FIRST!!!
            
            statement.executeUpdate(StringLib.SQLformat(
            "DELETE FROM output_printers WHERE app_title = " +
            Constants.ST + " AND otype_title = " + Constants.ST,
            Items));
            
            Items[0] = descrip;            
            Items[1] = new Boolean(batch);
            Items[2] = title;
            Items[3] = group;
            
            statement.executeUpdate(StringLib.SQLformat(
            "UPDATE output_types SET description=" + Constants.ST +
            ",batch_flag=" + Constants.ST
            + " WHERE otype_title=" + Constants.ST + " AND app_title=" +
            Constants.ST,Items));
            
            logBuffer.append("OUTPUT TYPE MODIFIED\n");            
        }
        
        else if(operation.equals("ADD")) {
            
            Items[0] = title;
            Items[1] = group;
            Items[2] = descrip;            
            Items[3] = new Boolean(batch);
            
            SQLstring = StringLib.SQLformat("INSERT INTO " +
            "output_types (otype_title,app_title,description," +
            "batch_flag) VALUES (" + Constants.ST +
            "," + Constants.ST + "," + Constants.ST + 
            "," + Constants.ST + ")",Items);
            
            Log.debug("OtypeInfo", "processOtypes", SQLstring);
            
            statement.executeUpdate(SQLstring);
            
            logBuffer.append("OUTPUT TYPE ADDED\n");
        }
        
        if (operation.equals("ADD") || operation.equals("EDIT")) {
            
            Items[0] = title;
            Items[1] = group;                        
            
            if ((defprt == null) || (defprt.length() == 0))
               logBuffer.append("NAME:            " + title + 
                                "\nAPPLICATION:     " + group + 
                                "\nDESCRIPTION:     " + descrip + 
                                "\nDEFAULT PRINTER: None\nBATCH PRINTING:  " + 
                                batchStr + "\n");
            else
               logBuffer.append("NAME:            " + title + 
                                "\nAPPLICATION:     " + group + 
                                "\nDESCRIPTION:     " + descrip + 
                                "\nDEFAULT PRINTER: " + defprt + 
                                "\nBATCH PRINTING:  " + 
                                batchStr + "\n");
            
            logBuffer.append("OUTPUT TYPE LINKED TO THESE PRINTERS\n");
            
            if ((printerList != null) && (printerList.trim().length() > 0)) {
               StringTokenizer st = new StringTokenizer(printerList,
                                                        INI.STRINGS_DELIMITER);
               while (st.hasMoreTokens()) {
                  String printer=st.nextToken();
                  Items[2] = new String(printer.substring(printer.indexOf("=")+1));
                  
                  if ((defprt != null) && (defprt.equals(Items[2]) == true))
                     Items[3] = new Boolean(true);
                  else
                     Items[3] = new Boolean(false);
                
                  SQLstring = StringLib.SQLformat("INSERT INTO " +
                  "output_printers (otype_title,app_title,printer_name, default_printer)" +
                  " VALUES (" + Constants.ST +  "," + Constants.ST + "," +
                  Constants.ST + "," + Constants.ST + ")",Items);
                
                  Log.debug("OtypeInfo", "processOtypes", SQLstring);
                  statement.executeUpdate(SQLstring);
                
                  logBuffer.append(Items[2] + "\n");
               }
            }
            
            logBuffer.append("\n");
        }
        
        statement.close();
        return true;
        
    } //try
    
    catch (Exception exception) {
        Log.excp("OtypeInfo", "processOtypes",exception.toString());
        return false;
    }
    ///
    finally {
        try {
            if(statement!=null)
                statement.close();
        }
        catch (Exception exception){
            Log.excp("OtypeInfo", "processOtypes",
            exception.toString());
        }
    }
    
    
    ///
}
   } ///:~  OtypeInfo
   
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
   class PrinterInfo   
   {
      public String OPERATION, PRINTERNAME, LOCATION, CLASS;
      public String PORT, REDIRECTED;
      public String ADAPTER_IP, TYPE, NTDRIVER, FILENAME;
      public int MAXSIZE,SECURITY;
      public boolean QUEUESTATUS, PRINTERSTATUS, ACCESSSTATUS, SUSPEND, APPEND;
      ArrayList access = new ArrayList(); //for applying access 
      
      /**
       * Loads a printers's information from the apply.ini file
       */
      
//==============================================================================
      public boolean load(INI ini, String printerSection, String iniFile)
//==============================================================================
      {
         final String defVal = "NONE";
         
         try {  
             
            OPERATION = ini.GetProfileString(printerSection, "OPERATION", defVal, 
                                            iniFile);
            
            PRINTERNAME = ini.GetProfileString(printerSection, "PRINTERNAME",defVal, 
                                            iniFile);
            
            LOCATION = ini.GetProfileString(printerSection, "LOCATION", defVal, 
                                            iniFile);
            
            MAXSIZE = ini.GetProfileInt(printerSection, "MAXSIZE", 0, iniFile);
            
            CLASS = ini.GetProfileString(printerSection, "CLASS", defVal, iniFile);
            
            SUSPEND = ini.GetProfileInt(printerSection, "SUSPEND", 0, 
                                            iniFile)==1 ? true : false;
            
            SECURITY = ini.GetProfileInt(printerSection, "SECURITY", 0, iniFile);
            
            QUEUESTATUS = ini.GetProfileInt(printerSection, "QUEUESTATUS", 0, 
                                            iniFile)==1 ? true : false;
            
            PRINTERSTATUS = ini.GetProfileInt(printerSection, "PRINTERSTATUS", 0, 
                                            iniFile)==1 ? true : false;
            
            ACCESSSTATUS = ini.GetProfileInt(printerSection, "ACCESSSTATUS", 0, 
                                            iniFile)==1 ? true : false;
            
            PORT = ini.GetProfileString(printerSection, "PORT", defVal, iniFile);
           
            REDIRECTED = ini.GetProfileString(printerSection, "REDIRECTED", defVal, 
                                            iniFile);
            
            if(REDIRECTED.equals("None")) //Don't want this junk in the DB
                REDIRECTED=null;
            
            ADAPTER_IP = ini.GetProfileString(printerSection, "ADAPTER_IP", defVal, 
                                            iniFile);
            
            TYPE = ini.GetProfileString(printerSection, "TYPE", "REGULAR", iniFile);
            
            NTDRIVER = ini.GetProfileString(printerSection, "NTDRIVER", defVal, 
                                            iniFile);
            
            APPEND = ini.GetProfileInt(printerSection, "APPEND", 0, iniFile)
                                       ==1 ? true : false;
            
            FILENAME = ini.GetProfileString(printerSection, "FILENAME", defVal, iniFile);
            
            //Need to get printer access in here
            String Sections;
            String app,key;
            
            Log.debug("PrintLib", "printer.load","Making access arraylist with -> " + printerSection + "_ACCESS"); //debug
            /////////////////
            StringTokenizer tokenizer = new StringTokenizer
            (ini.GetSectionValues(printerSection + "_ACCESS",iniFile),INI.STRINGS_DELIMITER);
            
            while (tokenizer.hasMoreElements() == true) {
                StringTokenizer tokenizer2 = new StringTokenizer(tokenizer.nextToken(), "=");
                if(tokenizer2.hasMoreElements() == true) 
                    access.add(tokenizer2.nextToken());
            }
            
            Log.debug("PrintLib", "printer.load","Made access arraylist size is  -> " + access.size()); //debug

            return true;
         }
         catch (Exception exception) {
            Log.excp("PrintLib", "printer.load","Could not load printer INI file");
           return false;
         }
      }
            
 ///////////////////////////////////////////////////////////////////////////////
 //Overloaded to take a class of LDAP values to be used in reconcile  
      
//==============================================================================
      public boolean load(PrinterStruct v, String operation)
//==============================================================================

      {         
         try {  
             
            OPERATION = operation;
            PRINTERNAME = v.printerName;  
            LOCATION = v.location; 
            MAXSIZE = v.jobMaxBytes;
            CLASS = v.printerType;    
            SUSPEND = v.suspendRequests; 
            SECURITY = v.securityClass;
            QUEUESTATUS = v.queueEnabled; 
            PRINTERSTATUS = v.printerEnabled;
            ACCESSSTATUS = v.generalAccess;  
            PORT = v.portName;  
            REDIRECTED = v.redirectionPtr;  
            ADAPTER_IP = v.adapterIPAddress;  
            TYPE = v.printingMethod; 
            NTDRIVER = v.driverName;  
            APPEND = v.fileAppendFlag;  
            FILENAME = v.fileName;  
            
            //put in access for this printer...
            access = v.appList;
     
            return true;
         }
         catch (Exception exception) {
            Log.excp("PrintLib", "printer.load","Could not load LDAP");
           return false;
         }
      }
      
      
       /**
       * Updates, inserts or deletes printer record
       */
//==============================================================================
      public boolean processPrinters(Connection connection, ArrayList appsOnHost) {
//==============================================================================

          Statement statement=null;
          ResultSet resultSet=null;
          Object printerItems[];
          int ret = 0;
          boolean printeradded = false;
          String SQLstring;

          Log.debug("PrintLib", "printer.processPrinters","NAME is -> " + PRINTERNAME); //debug
          Log.debug("PrintLib", "printer.processPrinters","Type is -> " + TYPE + " and FILENAME is-> " + FILENAME); //debug
          Log.debug("PrintLib", "printer.processPrinters","NTDRIVER is -> " + NTDRIVER ); //debug
          Log.debug("PrintLib", "printer.processPrinters","OPERATION is -> " + OPERATION ); //debug

          try {
              
              statement = connection.createStatement();
              printerItems = new Object[19];
              printerItems[0] = PRINTERNAME; //PRINTERNAME
              
              //If operation is an EDIT on MASTER it might need to be processed as an add
              //here... so check if it's really an edit
              if(OPERATION.equals("EDIT")) {
                  
                
                  resultSet=statement.executeQuery(StringLib.SQLformat(
                      "SELECT COUNT(*) AS cnt FROM printers WHERE printer_name = " +
                      Constants.ST , printerItems));
                  
                  if(resultSet.first())
                  {
                      Log.debug("PrintLib", "printer.processPrinters","Got a resultset"); //debug
                       if(resultSet.getString(1).equals("0"))
                          OPERATION = "ADD";
                  }
                  
                  
              }
              
              Log.debug("PrintLib", "printer.processPrinters","PAST EDIT CHK Opeation now is -> " + OPERATION ); //debug
              
              
              if(OPERATION.equals("DELETE")) {
                 
                 // Try to remove from the system first
                 Log.debug("PrintLib","Printer.process", "Calling Native Printer Delete");
                 if (PrintLib.deletePrinter(getPrinterName(ADAPTER_IP,PORT), TYPE) == true) {
                    
                    // Remove from database
                    ret = DataLib.cascadeDelete(statement,"printers", StringLib.SQLformat(
                        "printer_name = " + Constants.ST , printerItems));
                    
                    // Remove printer from NDS if this host is the NTCSS master
                    if (DataLib.isMasterServer() == true) {
                       if (NDSLib.deletePrinter(PRINTERNAME) == false) 
                          Log.error("PrintLib", "printer.processPrinters", 
                                    "Error deleting <" + PRINTERNAME + "> from NDS");
                    }
                    
                    return (ret == 1);
                 }
                 else {
                    Log.debug("PrintLib","Printer.process", "Native Printer Delete Failed!!");
                    return false;
                 }
              }

              else { //EDIT OR ADD
                  
                  //remap overloaded fields into the new fields...
                  String NTSHARE=null;
                  String NTHOST=null;
                  String UNIXHOST=null;
                  
                  if(TYPE.equals(Constants.REGULAR_PRINTER)) {
                      FILENAME=null;
                      APPEND=false;
                  }
                  else
                      if(TYPE.equals(Constants.FILE_PRINTER)) {
                          PORT=null;
                          ADAPTER_IP=null;
                      }
                      else
                          if(TYPE.equals(Constants.NT_SHARE_PRINTER)) {
                              NTHOST=ADAPTER_IP;
                              NTSHARE=PORT;
                              FILENAME=null;
                              PORT=null;
                              ADAPTER_IP=null;
                              APPEND=false;
                          }
                          else
                              if(TYPE.equals(Constants.UNIX_PRINTER)) {
                                  FILENAME=null;
                                  UNIXHOST=ADAPTER_IP;
                                  ADAPTER_IP=null;
                                  APPEND=false;
                              }
                  
                  if(OPERATION.equals("EDIT")){
                      
                      printerItems[0] = TYPE;        //PRINTING METHOD
                      printerItems[1] = LOCATION;    //LOCATION
                      printerItems[2] = new Integer(MAXSIZE); //JOB_MAX_BYTES
                      printerItems[3] = CLASS;       //PRINTER_TYPE
                      printerItems[4] = UNIXHOST;      //HOST IP
                      printerItems[5] = new Integer(SECURITY);      //SECURITY_CLASS
                      printerItems[6] = new Boolean(QUEUESTATUS);
                      printerItems[7] = new Boolean(PRINTERSTATUS);
                      printerItems[8] = new Boolean(ACCESSSTATUS);
                      printerItems[9] = PORT;          //PORT_NAME
                      printerItems[10] = new Boolean(SUSPEND);       //SUSPEND_REQUESTS
                      printerItems[11] = REDIRECTED;
                      printerItems[12] = FILENAME;
                      printerItems[13] = ADAPTER_IP;
                      printerItems[14] = new Boolean(APPEND);
                      printerItems[15] = NTHOST;        //NT_HOST_NAME
                      printerItems[16] = NTSHARE;       //NT_SHARE_NAME
                      printerItems[17] = NTDRIVER;      //NEW... DRIVERNAME
                      printerItems[18] = PRINTERNAME;   //PRINTERNAME                      
                      
                      ret=statement.executeUpdate(StringLib.SQLformat("UPDATE printers"
                      + " SET printing_method= " + Constants.ST + " ,location= "
                      + Constants.ST + " ,job_max_bytes= " + Constants.ST +
                      " ,printer_type= " + Constants.ST + " ,host_ip_address= "
                      + Constants.ST + " ,security_class= " + Constants.ST +
                      " ,queue_enabled= " + Constants.ST + " ,printer_enabled= "
                      + Constants.ST + " ,general_access= " + Constants.ST +
                      " ,port_name= " + Constants.ST + " ,suspend_requests= " +
                      Constants.ST + " ,redirection_ptr= " + Constants.ST +
                      " ,file_name= " + Constants.ST + " ,adapter_ip_address= "
                      + Constants.ST + " ,file_append_flag= " + Constants.ST +
                      " ,nt_host_name= " + Constants.ST + " ,nt_share_name= "
                      + Constants.ST +  " ,driver_name= " + Constants.ST + 
                      "WHERE printer_name= " + Constants.ST,printerItems));
                      
                      // If this host is the NTCSS master, update NDS
                      if (DataLib.isMasterServer() == true) {
                         if (NDSLib.editPrinter(PRINTERNAME, LOCATION, 
                                               MAXSIZE, CLASS, SECURITY,
                                               SUSPEND, REDIRECTED, 
                                               NTDRIVER, access, TYPE, 
                                               UNIXHOST, PORT, FILENAME, 
                                               ADAPTER_IP, NTHOST, 
                                               NTSHARE, QUEUESTATUS, 
                                               PRINTERSTATUS, ACCESSSTATUS, APPEND) == false)
                            Log.error("PrintLib","Printer.process", "Error editing <" + 
                                       PRINTERNAME + "> in NDS");
                      }
                      
                  }
                  
                  else if(OPERATION.equals("ADD")) { 
                      
                      Log.debug("PrintLib", "printer.processPrinters","In op = add -> " + OPERATION ); //debug
                      
                      printerItems[0] = PRINTERNAME; //PRINTERNAME
                      printerItems[1] = TYPE;        //PRINTING METHOD
                      printerItems[2] = LOCATION;    //LOCATION
                      printerItems[3] = new Integer(MAXSIZE);     //JOB_MAX_BYTES
                      printerItems[4] = CLASS;       //PRINTER_TYPE
                      printerItems[5] = UNIXHOST;      //HOST IP
                      printerItems[6] = new Integer(SECURITY);      //SECURITY_CLASS
                      printerItems[7] = new Boolean(QUEUESTATUS);
                      printerItems[8] = new Boolean(PRINTERSTATUS);
                      printerItems[9] = new Boolean(ACCESSSTATUS);
                      printerItems[10] = PORT;          //PORT_NAME
                      printerItems[11] = new Boolean(SUSPEND);       //SUSPEND_REQUESTS
                      printerItems[12] = REDIRECTED;
                      printerItems[13] = FILENAME;
                      printerItems[14] = ADAPTER_IP;
                      printerItems[15] = new Boolean(APPEND);
                      printerItems[16] = NTHOST;       //NT_HOST_NAME
                      printerItems[17] = NTSHARE;       //NT_SHARE_NAME
                      printerItems[18] = NTDRIVER;       //NT_SHARE_NAME
                      
                      //On NT the NATIVE guy will control what Type of Printers are added 
                      
                      Log.debug("PrintLib","Printer.process", "NTDRIVER is->  " + NTDRIVER ); //debug
                      
                      //We want NULLS in DB for irrelevant fields... but they will blowup JNI
                      //so we check for that in Param 
                      
                      if (NTDRIVER.endsWith(Constants.W2K_PRT_DRVR_EXT)) {
                              
                             Log.debug("PrintLib","Printer.process", "Doing 2K add driver name is-> " + 
                                        NTDRIVER.substring(0, NTDRIVER.lastIndexOf(Constants.W2K_PRT_DRVR_EXT))); //debug
                             if(!add2KPrinter(getPrinterName(NTHOST,NTSHARE), FILENAME == null ? "NONE" : FILENAME ,NTDRIVER.substring(0, 
                                              NTDRIVER.lastIndexOf(Constants.W2K_PRT_DRVR_EXT)),
                                              TYPE))
                                 return false;
                             printeradded=true;
                              
                      }
                      else
                      {                              
                              Log.debug("PrintLib","Printer.process", "Calling Native Printer Guy passing port as-> " + FILENAME ); //debug

                              if(!addPrinter(getPrinterName(NTHOST,NTSHARE), NTDRIVER, TYPE, FILENAME == null ? "NONE" : FILENAME ,connection)) {
                                  Log.debug("PrintLib","Printer.process", "Native Printer Add Failed!!!");
                                  return false;
                              }
                              
                              printeradded=true;
                      }
                          
                      Log.debug("PrintLib","Printer.process", "Native Printer add sucessful "); //debug
                                                
                      
                      SQLstring = StringLib.SQLformat("INSERT INTO printers " +
                      "VALUES (" + Constants.ST + " , " + Constants.ST + " , " + Constants.ST
                      + " , " + Constants.ST +  " , " + Constants.ST + " , " + Constants.ST
                      + " , " + Constants.ST + " , " + Constants.ST + " , " + Constants.ST
                      + " , " + Constants.ST + " , " + Constants.ST + " , " + Constants.ST
                      + " , " + Constants.ST + " , " + Constants.ST + " , " + Constants.ST
                      + " , " + Constants.ST + " , " + Constants.ST + " , " + Constants.ST
                      + " , " + Constants.ST + " )", printerItems);
                      Log.debug("PrintLib","Printer.process", "Inserting INTO printers-> " +                      
                                SQLstring);
                                            
                      ret=statement.executeUpdate(SQLstring);
                      
                      if(ret!=1) {
                          Log.debug("PrintLib","Printer.process", "Insert into Printers Failed!!!!!!!!! ");
                          PrintLib.deletePrinter(PRINTERNAME, TYPE);
                          return false;
                      }
                      
                      // If on NTCSS master, add the information to NDS
                      if (DataLib.isMasterServer() == true) {
                         if (NDSLib.addPrinter(PRINTERNAME, LOCATION, 
                                               MAXSIZE, CLASS, SECURITY,
                                               SUSPEND, REDIRECTED, 
                                               NTDRIVER, access, TYPE, 
                                               UNIXHOST, PORT, FILENAME, 
                                               ADAPTER_IP, NTHOST, 
                                               NTSHARE, QUEUESTATUS, 
                                               PRINTERSTATUS, ACCESSSTATUS, APPEND) == false)
                            Log.error("PrintLib","Printer.process", "Error adding <" + 
                                       PRINTERNAME + "> to NDS");
                     }                          
                  }                
                                    
                  // Get a list of applications that this printer is an app_printer
                  //  for
                  printerItems[0] = PRINTERNAME;                  
                  resultSet = statement.executeQuery(StringLib.SQLformat(
                      "SELECT app_title FROM app_printers WHERE printer_name = " + 
                      Constants.ST, printerItems));
                  ArrayList currentAppList = new ArrayList();
                  while (resultSet.next() == true) 
                     currentAppList.add(resultSet.getString("app_title"));   
              
                  // Remove any unecessary app printers
                  for (int i = 0; i < currentAppList.size(); i++) {
                     if (access.contains(currentAppList.get(i)) == false) {
                        printerItems[0] = currentAppList.get(i); //APP
                        printerItems[1] = PRINTERNAME;   //PRINTERNAME
                        
                        Log.debug("PrintLib","Printer.process", "Doing App Printer cascade " 
                        + " delete for " + currentAppList.get(i));
                            
                        DataLib.cascadeDelete(statement, "app_printers", StringLib.SQLformat(
                              "app_title = " + Constants.ST + " AND printer_name = " + 
                              Constants.ST,printerItems));
                     }
                  }
                  
                  // Add any new app printers
                  for (int i = 0; i < access.size(); i++) {
                     if (currentAppList.contains(access.get(i)) == false) {
                        printerItems[0] = access.get(i); //APP
                        printerItems[1] = PRINTERNAME;   //PRINTERNAME
                              
                        if ((DataLib.isMasterServer() == true) || 
                            (appsOnHost.contains(access.get(i)) == true))
                           statement.executeUpdate(StringLib.SQLformat("INSERT INTO "
                              + "app_printers (app_title,printer_name) VALUES (" + 
                              Constants.ST + " , " + Constants.ST + " )",printerItems));                                                   
                     }
                  }
                                    
              } //else add or edit
              
              return ret==1;
              
          } //try
          
          catch (Exception exception) {
              Log.excp("PrintLib","Printer.process", exception.toString());
              return false;
          }
          
          finally {
              try {
                  if (resultSet != null)
                     resultSet.close();
                  
                  if(statement!=null)
                      statement.close();
              }
              catch (Exception exception){
                  Log.excp("PrintLib", "printer.process",
                  exception.toString());
              }
          }
      }
      
      //used to add too the log file
//==============================================================================
      public void outputPrinter(PrintWriter writer, boolean status)
//==============================================================================

      {

         try {  
             writer.println("\n\n**********************************************");
             writer.println("Begin response from Host <" + StringLib.getHostname()
             + ">\n");
             writer.println("OPERATION " + (status ? "SUCCESSFUL" : "FAILED"));
             writer.println("OPERATION:\t\t" + OPERATION);
             writer.println("NAME:\t\t" + PRINTERNAME);
             writer.println("CLASSIFICATION:\t\t" + Constants.getSecurityString(SECURITY));
             writer.println("PRINTER ACCESS:\t\t" + 
             (ACCESSSTATUS ? "By Application" : "General"));
             writer.println("LOCATION:\t\t" + LOCATION);
             writer.println("MAX REQUEST SIZE:\t\t" + MAXSIZE);
             writer.println("PRINTER CLASS:\t\t" + TYPE);
             writer.println("ADAPTER IP:\t\t" + ADAPTER_IP);
             writer.println("ADAPTER PORT:\t\t" + PORT);
             writer.println("PRINTER ACCESS:\t\t" + 
             (ACCESSSTATUS ? "Permit All Requests" : 
                                          "Suspend Requests"));
             writer.println("REDIRECT PRINTER:\t\t" + REDIRECTED);
             writer.println();
             writer.println();
             writer.flush();
             writer.close();

         }
         catch (Exception exception) {
            Log.excp("PrintLib", "printer.outputPrinter",exception.toString());
         }
      }
      
//------------------------------------------------------------------------------
    protected boolean isAppOnHost(ArrayList list)
//------------------------------------------------------------------------------
    //called in Extract && only on a Nonmaster
    //pass in a list of apps on this host & bounce this off list stored in PrinterInfo
    //returns true if it contains an app on this host & TAKES THE OTHERS OUT!!!!
    {
        
       boolean ret=false;
        
       if(ACCESSSTATUS == true) { //general access
           access.clear();
           return true;
       }
        
       for(int i=0;i<access.size();) 
            if(list.contains(access.get(i))) {
                ret=true;
                i++;
            }
            else
                access.remove(i);
        return ret;
    }
  
    //------------------------------------------------------------------------------
    protected boolean isPrinterOnHost(Connection connection)
    //------------------------------------------------------------------------------
    //neccessary to detect when the change is an edit that has taken away ALL printer access
    //will only be called when there is no more access, so operation is converted to a delete 
    {
        
        Statement statement=null;
        ResultSet resultSet=null;
        Object printerItems[];
        boolean ret=false;
        
        try {
            
            Log.debug("PrintLib","Printer.isPrinterOnHost", "In There ");
            
            statement = connection.createStatement();
            printerItems = new Object[1];
            
            printerItems[0] = PRINTERNAME;
            
            Log.debug("PrintLib","Printer.isPrinterOnHost", StringLib.SQLformat(
            "SELECT COUNT(*) as cnt FROM printers WHERE printer_name = " +
            Constants.ST, printerItems));
            
            resultSet = statement.executeQuery(StringLib.SQLformat(
            "SELECT COUNT(*) as cnt FROM printers WHERE printer_name = " +
            Constants.ST, printerItems));
            
            if(resultSet.first())
                ret = resultSet.getInt("cnt")==1;
            else
                return ret;
            
            if(ret==true) {
                
                 Log.debug("PrintLib","Printer.isPrinterOnHost", "All printer "
                 + " access removed changing operation to delete");
                 OPERATION="DELETE";
            }
            
            return ret;
            
        }
        
        catch (Exception exception) {
            Log.excp("PrintLib","Printer.isPrinterOnHost", exception.toString());
            return false;
        }
        
        finally {
            try {
                if (resultSet != null)
                    resultSet.close();
                
                if(statement!=null)
                    statement.close();
            }
            catch (Exception exception){
                Log.excp("PrintLib", "printer.isPrinterOnHost",
                exception.toString());
            }
        }
    }
   
    
//------------------------------------------------------------------------------
protected boolean add2KPrinter(String printername, String filename, 
                               String drivername, String printingMethod)
//------------------------------------------------------------------------------
{
    return PrintLib.add2KPrinter(printername, filename, drivername,
                                 printingMethod);
}

    
//------------------------------------------------------------------------------
protected boolean addPrinter(String printername, String driver, String type, 
                             String port, Connection connection)
//------------------------------------------------------------------------------
// Implements the Native Printer Add

{

    Statement statement=null;
    ResultSet resultSet=null;
    Object printerItems[];
    String monitor="";
    String copyfiles="";
    if(port==null)
        port="";

    try {
        
        statement = connection.createStatement();
        Object [] Items = new Object[2];
        Items[0] = driver;
        
        //get monitor
        resultSet = statement.executeQuery(StringLib.SQLformat(
        "SELECT nt_monitor_name FROM print_drivers WHERE driver_name = " +
        Constants.ST,Items)); 
        
        Log.debug("PrintLib","Printer.addPrinter", "Printername <" + printername + 
        "> driver  <" + driver + "> type  <" + type + "> port <" + port + ">"); //debug
        
        if(resultSet.next())
            monitor=resultSet.getString("nt_monitor_name");
        else
            monitor="";
        
        //get copystring
        Items[1]=new Integer(5);
        resultSet = statement.executeQuery(StringLib.SQLformat(
        "SELECT * FROM print_driver_files WHERE driver_name = " +
        Constants.ST + " AND type = " + Constants.ST, Items)); 
        
        while(resultSet.next())
            if(copyfiles.length()==0)
                copyfiles=resultSet.getString("driver_filename");
            else
                copyfiles += "," + resultSet.getString("driver_filename");
        
        if(copyfiles==null)
            copyfiles="";
        
        //get the main files 
        resultSet = statement.executeQuery(StringLib.SQLformat(
        "SELECT * FROM print_driver_files WHERE driver_name = " +
        Constants.ST + " AND type < 5" + Constants.ST +  " ORDER BY type",Items)); 
        
        HashMap mainDrivers= new HashMap();
        int ntype;
        
        while(resultSet.next())
            if((ntype=resultSet.getInt("type"))<5)
                mainDrivers.put(new Integer(ntype).toString(),
                                resultSet.getString("driver_filename"));
        
        //check if there was a monitor file
        //if not make sure we don't pass a null to JINI
        if(mainDrivers.size()!=5)
            mainDrivers.put("4","");
            
        Log.debug("PrintLib","Printer.addPrinter", printername + " - " +
        driver + "-" + (String)mainDrivers.get("0") + "-" + 
        (String)mainDrivers.get("2") + " - " + 
        (String)mainDrivers.get("1") + " - " + 
        (String)mainDrivers.get("3") + " - " + 
        (String)mainDrivers.get("4") + " - " + copyfiles + " - " + type + " - " + port); //debug 
        
        //Call native method
        return PrintLib.addPrinter(printername, driver, (String)mainDrivers.get("0"),
        (String)mainDrivers.get("2"), (String)mainDrivers.get("1"),
        (String)mainDrivers.get("3"), monitor,
        (String)mainDrivers.get("4")==null ? "" : (String)mainDrivers.get("4"), 
        copyfiles, type, port, Constants.NTCSS_NTDRIVER_DIR);
    }

    catch (Exception exception) {
        Log.excp("PrintLib","Printer.process", exception.toString());
        return false;
    }

    finally {
        try {

            if(resultSet != null)
                resultSet.close();

            if(statement != null)
                statement.close();
        }
        catch (Exception exception){
            Log.excp("PrintLib", "printer.addPrinter",
            exception.toString());
        }
    }
}

    //returns either the printername (if file Prt) or a UNC if it's a share
    protected String getPrinterName(String host, String share)
    {
        if(TYPE.equals(Constants.NT_SHARE_PRINTER) == true)
            return ("\\\\" + host + "\\" + share); 
        else
            return PRINTERNAME;
    }
    
} ///:~ Printerinfo
   
////////////////////////////////////////////////////////////////////////////////   
   class AdapterInfo
   {
       public String OPERATION, IP, TYPE, LOCATION, MAC, HOST;
       
      /**
       * Loads a adapters's information from the apply.ini file
       */
       
//------------------------------------------------------------------------------
    public boolean load(INI ini, String adapterSection, String iniFile)
//------------------------------------------------------------------------------
{
   final String defVal = "NONE";

   try {

       OPERATION = ini.GetProfileString(adapterSection, "OPERATION", defVal,
       iniFile);

       IP = ini.GetProfileString(adapterSection, "IPADDRESS", defVal, iniFile);

       MAC = ini.GetProfileString(adapterSection, "MACHINE_ADDRESS", defVal,
       iniFile);

       LOCATION = ini.GetProfileString(adapterSection, "LOCATION", defVal,
       iniFile);

       TYPE = ini.GetProfileString(adapterSection, "TYPE", "REGULAR", iniFile);
       
       HOST = ini.GetProfileString(adapterSection, "HOSTNAME", "None", iniFile);

       return true;
   }
   catch (Exception exception) {
       Log.excp("PrintLib", "adapter.load","Could not load adapter INI file");
       return false;
   }
}


 //Overloaded to take a class of LDAP values to be used in reconcile  
//------------------------------------------------------------------------------
    public boolean load(AdapterStruct v, String operation)
//------------------------------------------------------------------------------
{   
   try {

       OPERATION = operation;
       IP = v.adapterIPAddress;
       MAC = v.ethernetAddress;
       TYPE = v.type;
       LOCATION = v.location;
       HOST = v.host;

       return true;
   }
   catch (Exception exception) {
       Log.excp("PrintLib", "adapter.load","Could not load adapter class");
       return false;
   }
}
       /**
        * Updates, inserts or deletes printer record
        */
//------------------------------------------------------------------------------
    public boolean processAdapters(Connection connection)
//------------------------------------------------------------------------------
       {
           Statement statement=null;
           Object adapterItems[];
           int ret=0;
           
           try {
               
               statement = connection.createStatement();
               adapterItems = new Object[4];
               adapterItems[0] = IP;
               
               if(OPERATION.equals("DELETE")) {
                  
                   // Before the adapter is removed from the database,
                   //  set all of it's printers adpater_ip_address field to null
                   adapterItems[0] = null;
                   adapterItems[1] = IP;
                   ret = statement.executeUpdate(StringLib.SQLformat(
                   "UPDATE printers SET adapter_ip_address = " +
                   Constants.ST + "WHERE adapter_ip_address = " + Constants.ST, 
                   adapterItems));
                   
                   // Now remove the adapter record from the database
                   adapterItems[0] = IP;
                   ret=statement.executeUpdate(StringLib.SQLformat(
                   "DELETE FROM printer_adapters WHERE adapter_ip_address = " +
                   Constants.ST, adapterItems));
                   
                   // If this host is the NTCSS master, update NDS
                   if (DataLib.isMasterServer() == true) {
                      if (NDSLib.deleteAdapter(IP) == false)
                         Log.error("PrintLib","Adapters.process", "Error deleting <" + 
                                       IP + "> from NDS");
                   }
               }
               
               else if(OPERATION.equals("EDIT")){
                   
                   adapterItems[0] = MAC;
                   adapterItems[1] = TYPE;
                   adapterItems[2] = LOCATION;
                   adapterItems[3] = IP;
                   
                   ret=statement.executeUpdate(StringLib.SQLformat("UPDATE " +
                   "printer_adapters SET ethernet_address= " + Constants.ST +
                   " ,type= " + Constants.ST + " ,location= " + Constants.ST +
                   " WHERE adapter_ip_address= " + Constants.ST,adapterItems));
                   
                   // If this host is the NTCSS master, update NDS
                   if (DataLib.isMasterServer() == true) {
                      if (NDSLib.editAdapter(IP, HOST, TYPE, MAC, LOCATION) == false)
                         Log.error("PrintLib","Adapters.process", "Error editing <" + 
                                       IP + "> in NDS");
                   }
               }
               
               else if(OPERATION.equals("ADD")) {
                   
                   adapterItems[0] = IP;
                   adapterItems[1] = MAC;
                   adapterItems[2] = TYPE;
                   adapterItems[3] = LOCATION;
                   
                   ret=statement.executeUpdate(StringLib.SQLformat("INSERT INTO "
                   + "printer_adapters VALUES (" + Constants.ST + " , " +
                   Constants.ST + " , " + Constants.ST + " , " + Constants.ST
                   + ")",adapterItems));
                   
                   // If this host is the NTCSS master, update NDS
                   if (DataLib.isMasterServer() == true) {
                      if (NDSLib.addAdapter(IP, HOST, TYPE, MAC, LOCATION) == false)
                         Log.error("PrintLib","Adapters.process", "Error adding <" + 
                                       IP + "> to NDS");
                   }
               }
               
               statement.close();
               return ret==1;
               
           } //try
           catch (Exception exception) {
               Log.excp("PrintLib", "Adapters.process",exception.toString());
               return false;
           }
           
            finally {
            try {
                
                if(statement!=null)
                    statement.close();
            }
            catch (Exception exception){
                Log.excp("PrintLib", "Adapters.process",
                exception.toString());
            }
        }
       }
       
//------------------------------------------------------------------------------
      public void outputAdapter(PrintWriter writer, boolean status)
//------------------------------------------------------------------------------
      {
          
         try {  
            
             if(OPERATION.equalsIgnoreCase("DELETE")) {
                 writer.println((status ? "OUTPUT TYPE DELETED" :
                                          "OUTPUT TYPE DELETION FAILED"));
                 writer.println("IP ADDRESS:\t\t" + IP);
                 writer.println();
                 
             }
             
             if(OPERATION.equalsIgnoreCase("ADD") ||
             OPERATION.equalsIgnoreCase("EDIT"))  {
                 if(OPERATION.equalsIgnoreCase("ADD"))
                 writer.println((status ? "OUTPUT TYPE ADDED" :
                     "OUTPUT TYPE ADD FAILED"));
                 else
                     writer.println((status ? "OUTPUT TYPE MODIFED" :
                         "OUTPUT TYPE MODIFIED FAILED"));
                         
                         writer.println("IP ADDRESS:\t\t" + IP);
                         writer.println("MACHINE ADDRESS:\t\t" + MAC);
                         writer.println("LOCATION:\t\t" + LOCATION);
                         writer.println("TYPE:\t\t" + TYPE);
                         writer.println();
             }

         }
         catch (Exception exception) {
            Log.excp("PrintLib", "printer.outputAdapter",exception.toString());
         }
      }
   } ///:~  AdapterInfo
   
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
class prtRecon
{
      Map nisPrinters = new HashMap();      
      ArrayList appOnThisHost = new ArrayList(); //Put the apps on this box in here
      private static final String className = "prtRecon";

     
//==============================================================================
   public boolean reconcile()
//==============================================================================
   {

      Connection connection = null;
      Statement statement = null;
      ResultSet resultSet = null;
      
    //  Log.turnDebugOn(); //debug

       try {                                

           // Get a connection from the database
           if((connection = DatabaseConnectionManager.getConnection())==null) {
               Log.error(className, "reconcile", "Can't get database connection");
           return false;
           }
           
           //Build the appOnThisHost ArrayList
           String thisHost = StringLib.getHostname();

            Object Items[] = new Object[1];
            Items[0] = thisHost.toUpperCase();

            statement = connection.createStatement();
            resultSet = statement.executeQuery(StringLib.SQLformat
            ("SELECT app_title FROM apps WHERE upper(hostname) = " +
            Constants.ST +  " ORDER BY app_title", Items));

            while(resultSet.next()) 
                appOnThisHost.add(resultSet.getString(1));
            
           resultSet.close();
           statement.close();
           
           Log.info(className, "reconcile", "reconciling adapters...");
           ///////////////////////
           //TOTAL ADAPTER RECONCILE...
           ///////////////////////
           adapterReconcile(connection);
           
           Log.info(className, "reconcile", "Making printer map...");
           
            if(!makePrinterMap(connection)) {
                Log.warn(className, "reconcile", "No LDAP printers or " +
                "Couldn't make map");
                return false;
            }
           
            Log.debug(className, "reconcile","Made printer map.. size is -> " 
            + nisPrinters.size()); 
            
            Log.info(className, "reconcile", "Doing printer deletes...");
            
            int x = doDeletes(connection); 
            
           Log.info(className, "reconcile", "Doing printer modifications...");
           
           x = doUpdates(connection);
           
           Log.info(className, "reconcile", "Updated <" + x + "> Printers");
           Log.info(className, "reconcile", "Doing printer adds...");
           
           x = doAdds(connection, appOnThisHost);
           
           Log.info(className, "reconcile", "Added <" + x + "> Printers");
            
           return true;

       }
       
       catch (Exception exception) {
           Log.excp(className, "reconcile", exception.toString());
           return false;
       }
       finally {
           try {
               if(resultSet != null)
                   resultSet.close();

               if(statement != null)
                   statement.close();

               if (connection != null)
                  DatabaseConnectionManager.releaseConnection(connection);
           }

           catch (Exception exception) {
               Log.excp(className, "reconcile", exception.toString());
               return true;
           }
       }
   }
       
//==========================================================================       
  boolean makePrinterMap(Connection connection)
//========================================================================== 
//this guy has to be called 1st... will put ALL the printers in 1st & then AFTER 
//deletes we'll remove the guys that don't apply
   {

       try {

           Hashtable printersList = NDSLib.getPrinters();
           PrinterStruct struct;
           Enumeration keys = printersList.keys();
           int i = 0;
           while (keys.hasMoreElements() == true) {
              struct = (PrinterStruct)printersList.get(keys.nextElement());
              
              //this makes sure they pertain to this box
              if(printerAppliesToThisBox(struct.appList, struct.generalAccess)) {                             
                 nisPrinters.put(struct.printerName, struct);
                 i++;
              }
           }
           
           return i>=0;
       }
       catch (Exception exception){
           Log.excp(className, "makePrinterMap", exception.toString());
           return false;
       }
   }
       
//==========================================================================        
//  protected boolean printerAppliesToThisBox(String apps, String status)
   protected boolean printerAppliesToThisBox(ArrayList apps, boolean generalAccess)
//========================================================================== 
   //Checks if General Access OR App on this box SO ONLY THESE are considered 
   //called when making the main NIS Printer Map... TODO THIS NEEDS TO BE APPLIED
  //differently or initially do all & after deletes apply this to map!!!
  
   {

      if (generalAccess == true)
        return true;
      
      if (apps == null)
         return false;
      
      for (int i = 0; i < apps.size(); i++) {
         if (appOnThisHost.contains(apps.get(i)))
            return true;
      }
      
      return false;
   }
       
   //==========================================================================
   protected boolean adapterReconcile(Connection connection)
   //==========================================================================
   //These just need to match whats in LDAP... the master server & LDAP drive this
   //nothing is really App specific
   {
       
       Hashtable ndsAdapters;
       Statement statement = null;
       ResultSet resultSet = null;
       String sql2 = null;
       int ret=0;
       AdapterStruct v;
       
       try {
           

           ndsAdapters = NDSLib.getAdapters();
           
           Log.debug(className, "adapterReconcile", "Adapter Map made && contains " +
           ndsAdapters.size() + " Adapters");
           
           if(ndsAdapters.size()==0)
               return true;
           ///////////////////
           // do deletes first
           ////////////////////
           statement = connection.createStatement();
           
           //make not in String

           Enumeration tempKeys = ndsAdapters.keys();
           while (tempKeys.hasMoreElements() == true) {                         
               if(sql2 == null)
                   sql2 = "'" + (String)tempKeys.nextElement() + "'";
               else
                   sql2 += ",'" + (String)tempKeys.nextElement() + "'";
           }
           
           sql2 += ")";
          
           //then delete any that aren't in this NOT IN STRING
           ret=statement.executeUpdate("DELETE FROM printer_adapters"
           + " WHERE adapter_ip_address NOT IN ("  + sql2);
           
           Log.info(className, "adapterReconcile","DELETED <" + ret + "> " +
           "ADAPTERS");
           
           ////////////////////
           // do updates next
           ////////////////////
           
           resultSet=statement.executeQuery("SELECT * FROM printer_adapters");
           Object[] adapterItems = new Object[4];
           ret=0;
           
           Log.info(className, "adapterReconcile","Editing Adaptors");
           
           while(resultSet.next()) {

               if((v=(AdapterStruct)ndsAdapters.get(resultSet.getString("adapter_ip_address")))==null)
                   continue;
               if (!resultSet.getString("ethernet_address").equals(v.ethernetAddress)
               || !resultSet.getString("type").equals(v.type)
               || !resultSet.getString("location").equals(v.location)) {
                   AdapterInfo ai = new AdapterInfo();
                   ai.load(v,"EDIT");
                   if(ai.processAdapters(connection))
                       ret++;
               }
              
           }
           
           Log.info(className, "adapterReconcile","MODIFIED <" + ret + "> " +
           "ADAPTERS");
           
           ////////////////////
           // adds last
           ////////////////////
           HashSet dbAdapters= new HashSet();
           resultSet=statement.executeQuery("SELECT adapter_ip_address FROM printer_adapters");
           while(resultSet.next()) 
               dbAdapters.add(resultSet.getString(1));
           
           resultSet.close();
           ret=0;
           
           Log.info(className, "adapterReconcile","Adding Adaptors");
           
           tempKeys = ndsAdapters.keys();
           while (tempKeys.hasMoreElements() == true) {
              v = (AdapterStruct)ndsAdapters.get(tempKeys.nextElement());
              if (!dbAdapters.contains(v.adapterIPAddress)) {
                   AdapterInfo ai = new AdapterInfo();
                   ai.load(v,"ADD");
                   if(ai.processAdapters(connection))
                       ret++;                 
              }
           }
           
           Log.info(className, "adapterReconcile","ADDED <" + ret + "> " +
           "ADAPTERS");
           
       }
       catch (Exception exception) {
           Log.excp(className, "adapterReconcile", exception.toString());
           return false;
       }
       finally {
           try {
               if(resultSet != null)
                   resultSet.close();
               
               if(statement != null)
                   statement.close();
           }
           
           catch (Exception exception) {
               Log.excp(className, "adapterReconcile", exception.toString());
               return true;
           }
       }
       
       return true;
   }

   //==========================================================================
   public int doDeletes(Connection connection)
   //==========================================================================
   {
       
       Statement statement=null;
       ResultSet resultset=null;
       Object Items[];
       int ret=0;
       int numDeletes;
       String printerName;
       Items = new Object[1];
       String sql2 = null; //for NOT IN SQL String
       
       class delprtInfo {
           
           public String prtName, NTSharename, prtMethod;
           
           delprtInfo(ResultSet resultset) {
               
               try {
                   
                   prtName = resultset.getString("printer_name");
                   prtMethod = resultset.getString("printing_method");
                   
                   if(prtMethod.equalsIgnoreCase(Constants.NT_SHARE_PRINTER))
                       NTSharename = "\\\\" + resultset.getString("nt_host_name") + "\\" +
                       resultset.getString("nt_share_name");
                   else
                       NTSharename = resultset.getString("printer_name");
               }
               
               catch (Exception exception) {
                   Log.excp(className, "adapterReconcile", exception.toString());
               }
               
               
           }
           
           public String OSname() {
               
               return prtMethod.equalsIgnoreCase(Constants.NT_SHARE_PRINTER) ?
               NTSharename : prtName;
               
           }
       }
       
       try {
           
           statement = connection.createStatement();
           
           //handle the case where there are NO Printers pertaining to this box
           if(nisPrinters.size()==0) {
               
               Log.debug(className, "doDeletes","Doing Deletes where no Printers");
               
               resultset=statement.executeQuery("SELECT printer_name, printing_method " +
               ", nt_host_name, nt_share_name FROM printers");
           }
           else {
               
               Iterator keys = nisPrinters.keySet().iterator();
               while (keys.hasNext() == true) {
                   if(sql2 == null)
                       sql2 = "'" + (String)keys.next() + "'";
                   else
                       sql2 += ",'" + (String)keys.next() + "'";
               }
               
               sql2 += ")";
               
               resultset=statement.executeQuery("SELECT printer_name, printing_method" +
               ", nt_host_name, nt_share_name FROM printers " +
               "WHERE printer_name NOT IN (" + sql2);
           }
           
           //save the printers we need to TRY to physicaly delete
           HashMap printersToDelete  = new HashMap();
           while(resultset.next()) {
               
               delprtInfo prt=new delprtInfo(resultset);
               
               //printersMap.put(prt.printer,prt);
               printersToDelete.put(prt.prtName,prt);
           }
           resultset.close();
           
           if (nisPrinters.size() == 0)
               ret = DataLib.cascadeDelete(statement, "app_printers", null);
           else
               ret = DataLib.cascadeDelete(statement, "app_printers",
               "printer_name NOT IN ("  + sql2);
           
           Log.info(className, "doDeletes","Deleted <" + ret + "> application printers");
           
           numDeletes = 0;
           
           ArrayList tempKeys = new ArrayList(printersToDelete.keySet());
           
           delprtInfo p = null;
           
           for (int x = 0; x < tempKeys.size(); x++) {
               p=(delprtInfo)printersToDelete.get(tempKeys.get(x));
               
               if (PrintLib.deletePrinter(p.OSname() ,p.prtMethod) == true) {
                   
                   // Delete printer from the database
                   Items[0] = p.prtName;
                   Log.info(className,"doDeletes","calling Cascade delete for printer <" + p.prtName + ">");
                   DataLib.cascadeDelete(statement, "printers", StringLib.SQLformat(
                   "printer_name = " + Constants.ST, Items));
                   numDeletes++;
               }
               else
                   Log.error(className, "doDeletes","Unable to delete <" +
                   p.prtName + "> from system");
           }
           
           Log.info(className, "doDeletes","Deleted <" + numDeletes + "> printers");
           
           return ret;
           
           
       }
       
       
       
       catch (Exception exception){
           Log.excp(className, "doDeletes", exception.toString());
           return ret;
       }
       
       finally {
           try {
               if(resultset!=null)
                   resultset.close();
               if(statement!=null)
                   statement.close();
           }
           catch (Exception exception){
               Log.excp(className, "doDeletes", exception.toString());
               return ret;
           }
           
       }
   }
       
//==========================================================================
 public int doAdds(Connection connection, ArrayList appsOnThisHosts)
//==========================================================================
{
    
    Statement statement=null;
    ResultSet resultSet=null;
    PrinterInfo printerInfo;
    Object Items[];
    int ret=0;
    
    try {
        
        statement = connection.createStatement();
        Items = new Object[2];
        Items[0] = new String(StringLib.getHostname()).toUpperCase();       
        
        ArrayList tempKeys = new ArrayList(nisPrinters.keySet());
        
        //Add printers that are in LDAP but not in DB FOR THIS BOX
        
        HashSet printersDB = new HashSet();
        ArrayList printersToAdd = new ArrayList();
        Vector driversWeHave;
//        ArrayList adds = new ArrayList();
        
        //Get ALL the applicable DB printers into hashset for minipulation
        resultSet=statement.executeQuery(StringLib.SQLformat("SELECT " +
        "a.printer_name FROM printers a, " +
        "app_printers b, apps c, hosts d WHERE general_access=true OR " +
        "(a.printer_name = b.printer_name AND b.app_title = c.app_title " +
        "AND UPPER(c.hostname) = UPPER(d.hostname) AND UPPER(d.hostname) = "
        + Constants.ST + ")", Items));
        
        //Stick the result set in Hashset
        while(resultSet.next())
            printersDB.add(resultSet.getString(1));
                
        //Put all the Printers that we will TRY to add into another HashSet
        PrinterStruct struct;
        for (int i = 0; i < tempKeys.size(); i++) {
         
           if(!printersDB.contains(tempKeys.get(i))) {
              
                // Check to see if the printer has a driver
                struct = (PrinterStruct)nisPrinters.get(tempKeys.get(i));
                if (struct.driverName != null)
                   printersToAdd.add(tempKeys.get(i));
                else
                   Log.error(className, "doAdds", "No driver for <" + 
                        struct.printerName + ">. Not added");
           }

        }
        
        if(printersToAdd.size()==0) //nothing to add... were done
            return ret;
        
        //only add guys we got drivers for...
        //this guy makes a HashSet of drivers we have
        driversWeHave=getDrivers(printersToAdd, connection);
        
        //And now we've got to get the driver files && stick them into DB
        //IF we fail, take them out of the printersToAdd HashMap
        //This is probably THE MOST inefficient proceess in the whole System
        //a message based solution could help allot                
        
        for (int i = 0; i < printersToAdd.size(); i++) {

            struct = (PrinterStruct)nisPrinters.get(printersToAdd.get(i));
            if(driversWeHave.contains(struct.driverName)) { //If we have driver add it
                printerInfo = new PrinterInfo();
                printerInfo.load(struct,"ADD");
                if(printerInfo.processPrinters(connection, appsOnThisHosts)) {
                    ret++;
//                    adds.add(printersToAdd.get(i));
                }
                else
                    Log.warn(className, "doAdds","Add Failed for " + printersToAdd.get(i));               
            }        
            else
               Log.error(className, "doAdds", "Couldn't get driver for <" + 
                         struct.printerName + ">. Not added.");
        }
        

        return ret;
    }
    
    catch (Exception exception){
        Log.excp(className, "doAdds",exception.toString());
        return 0;
    }
    finally {
        try {
            if(resultSet!=null)
                resultSet.close();
            if(statement!=null)
                statement.close();
        }
        catch (Exception exception){
            Log.excp(className, "doAdds", exception.toString());
        }
    }
}

//==========================================================================
    protected int doUpdates(Connection connection)
//==========================================================================
//Only Printers are Updated
{
    
    Statement statement = null;
    ResultSet resultSet = null;
    PrinterInfo printerInfo;
    Object Items[];
    int ret=0;
    boolean update;
    
    try {
        
        statement = connection.createStatement();
        
        Items = new Object[1];
        Items[0] = new String(StringLib.getHostname()).toUpperCase();                               
        resultSet = statement.executeQuery(StringLib.SQLformat("SELECT DISTINCT " +
        "a.printer_name, printing_method, location, job_max_bytes, " +
        "printer_type, a.host_ip_address, security_class, queue_enabled, " +
        "printer_enabled, general_access, port_name, suspend_requests, " +
        "redirection_ptr, file_name, adapter_ip_address, file_append_flag, " +
        "nt_host_name, nt_share_name, driver_name FROM " +
        "printers a, app_printers b, apps c, hosts d WHERE general_access=true " +
        "OR (a.printer_name = b.printer_name AND b.app_title = c.app_title " +
        "AND UPPER(c.hostname) = UPPER(d.hostname) AND UPPER(d.hostname) = " +
        Constants.ST + ")", Items));

        
        ArrayList requireSystemUpdate = new ArrayList(); 
        
        while(resultSet.next()) {
           update = false;
                      
           if(printCompare(resultSet) == false) { // System update required?
              requireSystemUpdate.add(resultSet.getString("printer_name"));                 
              update = true;
           }
           else if (benignPrintCompare(resultSet) == false) 
              update = true;           
           
           if (update == true) {
              printerInfo = new PrinterInfo();
              printerInfo.load((PrinterStruct)nisPrinters.get(resultSet.getString("printer_name")), "EDIT");
              if(printerInfo.processPrinters(connection, appOnThisHost))
                 ret++;
           }
        }
        
        // Now peform necessary system operations to those printers that
        //  require system updates. This printers are stored in the 
        //  requireSystemUpdate list.
        
        return ret;
    }
    
    catch (Exception exception){
        Log.excp(className, "doUpdates",exception.toString());
        return ret;
    }
    
    finally {
        try {
            
            if(resultSet!=null)
                resultSet.close();
            if(statement!=null)
                statement.close();
        }
        catch (Exception exception){
            Log.excp(className, "doUpdates", exception.toString());
        }
    }
}


       
//==========================================================================
    private Vector getDrivers(ArrayList printersToAdd, Connection connection)
//==========================================================================
//Physically gets the drivers && adds them to the driver DB for these printers
//If they can't be obtained they are taken out of the returned HashSet && NOT added
//This is probably THE MOST inefficient process there is... ESPECIALLY IF the
//master ISN'T UP
{
    ResultSet resultset=null;
    Statement statement=null;
    Vector driversToGet = new Vector();
    HashSet drivers = new HashSet();
    String driver = "";
    Vector v=null;
    File temp=null;
    
    try {
        
        //Build a unique List of drivers we need
        PrinterStruct struct;
        for (int i = 0; i < printersToAdd.size(); i++) {
           
           struct = (PrinterStruct)nisPrinters.get(printersToAdd.get(i));           
           drivers.add(struct.driverName);
        }
        
        //Next find out what drivers we all ready have on this host
        statement = connection.createStatement();
        resultset = statement.executeQuery("SELECT driver_name FROM "
        + "print_drivers");
        
        HashSet dbDrivers = new HashSet();
        while(resultset.next())
            dbDrivers.add(resultset.getString(1));
                
        //Now put the guys we need & aren't here in an another ArrayList
        Iterator x = drivers.iterator();
        
        while(x.hasNext()) {
            driver = (String)x.next();
            if(!dbDrivers.contains(driver))
                driversToGet.add(driver);
        }
        
        //Give Garbage Collector a hint
        drivers = null;               
        
        //TRY TO GET THE INI INFO FROM MASTER...
        //HOPEFULLY THIS WILL BE THE CASE OR WE HAVE THE OSWALD
        //PRISON TRANSFER
        StringBuffer sb=new StringBuffer();
        DataLib.getMasterServer(sb);
        temp = File.createTempFile("ntcss","tmp");
                
                              
        boolean found=false;
        if(FTP.get(sb.toString(),"/h/NTCSSS/database/ntprint.ini",temp.toString(),true))
                found=true;
        if(found)
           for (int i = 0; i < driversToGet.size(); ) {            
                driver = (String)driversToGet.elementAt(i);
               
                //get drivers AND put them in Local DB
                if(!pullDrivers(sb.toString(), driver, temp.toString()) ||
                   ! PrintLib.dbFromINI(driver, temp.toString(), statement, connection))
                    driversToGet.remove(i); //if something fails remove it
                else {
                   Log.debug(className, "getDrivers", "Got driver <" + driver + ">");
                   i++;
                }
            }
        else {
            Log.warn(className, "getDrivers", "Failed to get Master INI file");
            driversToGet.clear(); //TODO: GET THIS OUT WHEN WE GOTO OTHER HOSTS!!!
        }
        
        temp.delete();
        
        // Now add back to the list, the drivers we already had in the database.
        //  This entire list is expected from the caller of this method
        x = dbDrivers.iterator();
        while(x.hasNext()) {
            driver = (String)x.next();
            if(driversToGet.contains(driver) == false)
                driversToGet.add(driver);
        }
        
        //Give Garbage Collector a hint        
        dbDrivers = null; 
        
        return driversToGet;
        
        //TODO: deal with getting drivers with master down LATER... SOON
        //else { deal with getting this stuff with nonMaster LATER... SOON
        
    }
    
    catch (Exception exception){
        temp.delete();
        Log.excp(className, "getDrivers", exception.toString());
        return driversToGet;
    }
}


   
//==============================================================================
/**
 * Compares those printer fields that don't require any kind of system update
 */
protected boolean benignPrintCompare(ResultSet resultSet)
//==============================================================================
{

   try {
      String field;

       if(nisPrinters.containsKey(resultSet.getString("printer_name"))) {


           //get Data from Map
           PrinterStruct v = (PrinterStruct)nisPrinters.get(resultSet.getString("printer_name"));           

           if (resultSet.getString("location").equals(v.location) == false)
               return false;
           if (resultSet.getInt("job_max_bytes") != v.jobMaxBytes)
               return false;
           if (resultSet.getInt("security_class") != v.securityClass)
               return false;           
           if (resultSet.getBoolean("queue_enabled") != v.queueEnabled)
              return false;
           if (resultSet.getBoolean("printer_enabled") != v.printerEnabled)
              return false;
           if (resultSet.getBoolean("general_access") != v.generalAccess)
              return false;
           if (resultSet.getBoolean("suspend_requests") != v.suspendRequests)
              return false;
           if (resultSet.getBoolean("file_append_flag") != v.fileAppendFlag)
              return false;                                                                                       
           
           // This field can be null           
           if ((field = resultSet.getString("redirection_ptr")) != null) {
              if(field.equals(v.redirectionPtr) == false)
                 return false;
           }
           else {
              if (v.redirectionPtr != null)
                 return false;
           }
           
           // This field can be null           
           if ((field = resultSet.getString("printer_type")) != null) {
              if(field.equals(v.printerType) == false)
                 return false;
           }
           else {
              if (v.printerType != null)
                 return false;
           }

           // This field can be null           
           if ((field = resultSet.getString("host_ip_address")) != null) {
              if(field.equals(v.hostIPAddress) == false)
                 return false;
           }
           else {
              if (v.hostIPAddress != null)
                 return false;
           }
           
           // This field can be null           
           if ((field = resultSet.getString("file_name")) != null) {
              if(field.equals(v.fileName) == false)
                 return false;
           }
           else {
              if (v.fileName != null)
                 return false;
           }                                  
           
           // This field can be null           
           if ((field = resultSet.getString("nt_host_name")) != null) {
              if(field.equals(v.ntHostName) == false)
                 return false;
           }
           else {
              if (v.ntHostName != null)
                 return false;
           }
           
           // This field can be null           
           if ((field = resultSet.getString("nt_share_name")) != null) {
              if(field.equals(v.ntShareName) == false)
                 return false;
           }
           else {
              if (v.ntShareName != null)
                 return false;
           }
           
           // This field can be null           
           if ((field = resultSet.getString("driver_name")) != null) {
              if(field.equals(v.driverName) == false)
                 return false;
           }
           else {
              if (v.driverName != null)
                 return false;
           }
           
           // Check the app list
           Object sqlItems[] = new Object[2];
           sqlItems[0] = v.printerName;
           Statement statement = ((resultSet.getStatement()).getConnection()).createStatement();
           ResultSet resultSet2 = statement.executeQuery(StringLib.SQLformat(
                      "SELECT app_title FROM app_printers WHERE printer_name = " + 
                      Constants.ST, sqlItems));
           ArrayList currentAppList = new ArrayList();
           while (resultSet2.next() == true) 
              currentAppList.add(resultSet2.getString("app_title"));           
           resultSet2.close();
           statement.close();
           
           if ((v.appList.containsAll(currentAppList) == false) || 
               (currentAppList.containsAll(v.appList) == false))
              return false;
       }

       return true;

   }

   catch (Exception exception){
       Log.excp(className, "benignPrintCompare", exception.toString());
       return true;
   }
}

//==============================================================================
/**
 * Checks those printer fields that will require a system update also
 */
protected boolean printCompare(ResultSet resultSet)
//==============================================================================
{
   String field;

   try {

       if(nisPrinters.containsKey(resultSet.getString("printer_name"))) {


           //get data from Map
           PrinterStruct v = (PrinterStruct)nisPrinters.get(resultSet.getString("printer_name"));           

           if(!resultSet.getString("printing_method").equals(v.printingMethod))
               return false;
                          
           // This database field can be null
           if ((field = resultSet.getString("adapter_ip_address")) == null) {
              if (v.adapterIPAddress != null)
                 return false;
           }
           else {
              if (field.equals(v.adapterIPAddress) == false)
                 return false;
           }
           
           // This database field can be null
           if ((field = resultSet.getString("port_name")) == null) {
              if (v.portName != null)
                 return false;
           }
           else {
              if (field.equals(v.portName) == false)
                 return false;           
           }
           
           return true;
       }

       else {
           Log.warn(className,"printCompare","Printer " + 
           resultSet.getString("printer_name") + " not in LDAP!!!"); 
           return true;
       }
   }

   catch (Exception exception){
       Log.excp(className, "printCompare", exception.toString());
       return true;
   }
}
     
//==============================================================================
    protected boolean pullDrivers(String host, String section, String iniFilename)
//==============================================================================

{
    //FTPs ALL the driver files for this Driver to this HOST
    INI ini = new INI();
    ArrayList driverfiles = new ArrayList();
    
    try {
        
        //driver file
        driverfiles.add(ini.GetProfileString(section, "DRIVER", "", iniFilename));
        //UI file
        driverfiles.add(ini.GetProfileString(section, "UI", "", iniFilename));
        //DATA file
        driverfiles.add(ini.GetProfileString(section, "DATA", "", iniFilename));
        //HELP file
        driverfiles.add(ini.GetProfileString(section, "HELP", "", iniFilename));
        //MONITOR file
        driverfiles.add(ini.GetProfileString(section, "MONITOR", "", iniFilename));
        
        //COPY FILES stored as COPY_FILE_1 ... n
        String copyfile = "";
        for (int x=1;;x++) {
            copyfile=ini.GetProfileString(section,Constants.COPY_FILE_PREFIX
            + "_" + x,"",iniFilename);
            if(copyfile.equals(""))
                break;
            driverfiles.add(copyfile);
        }
        
        Log.debug(className,"pullDrivers","Getting files for driver <" + section + ">");
        
        //now get the drivers
        String fn="";
        for(int i=0;i<driverfiles.size();i++) {
           fn = Constants.NTCSS_NTDRIVER_DIR + "/" + driverfiles.get(i);
        
           if(!FTP.get(host, "/h/NTCSSS/spool/ntdrivers/" + driverfiles.get(i), fn, false)) {
              Log.error(className,"pullDrivers","Could not get " +
                   "driver </h/NTCSSS/spool/ntdrivers/" + driverfiles.get(i) + ">");
              return false;
           }
        }
        
        Log.debug(className,"pullDrivers","Got ALL THE Files for driver <" + section + "> !!!");
        return true;
    }
    
    catch (Exception exception){
        Log.excp(className, "pull drivers", exception.toString());
        return false;
    }
}
   } //:~ prtRecon
   
