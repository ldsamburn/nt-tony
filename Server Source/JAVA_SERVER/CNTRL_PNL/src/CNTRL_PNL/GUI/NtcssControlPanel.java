/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package CNTRL_PNL.GUI;

import javax.swing.*;
import java.awt.*;
import java.net.*;

import COMMON.*;
import CNTRL_PNL.COMMON.*;

public class NtcssControlPanel
{
  public static void main(String[] args)
  {
    boolean ok;
    OpeningBanner banner;

    // Enable the Just-In-Time compiler if it is available
    Compiler.enable();

    try  {

      // Try to load in the properties file
      if (args.length == 0)
         ok = false;
      else
         ok = PropertiesLoader.load(args[0]);
      
      // If a -v command line argument was given, print out the client version
      //  and exit
      if ((args.length == 2) &&
          (args[1].equals("-v") == true)) {
         System.out.println("Version " + Constants.CNTRL_PNL_VERSION);
         System.exit(0);
      }            
          
      // Setup the desired logging stream
      if ((Constants.CLT_LOG_FILE != null) &&
                               ((Constants.CLT_LOG_FILE.trim()).length() != 0))
         Log.setLogFile(Constants.CLT_LOG_FILE);            

      if (ok == true) {
         
         // Set the look and feel to the native environment's look
         UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
      
         // Create the openning banner
         banner = new OpeningBanner();
         banner.showBanner();
         
         // Create the main window
         (NCPFrame.getHandle()).createWindow(); 
         banner.hideBanner();

         Log.log(Log.CLT, Log.INFO, "NtcssControlPanel", "main",
                            "NTCSS Control Panel started");
      }
      else {
         Log.log(Log.CLT, Log.ERR, "NtcssControlPanel", "main",
                            "Error reading properties file! Cannot continue!!");
      }
    }
    catch (Exception e) {
      Log.log(Log.CLT, Log.ERR, "NtcssControlPanel", "main", e.toString());
    }
  }
}

