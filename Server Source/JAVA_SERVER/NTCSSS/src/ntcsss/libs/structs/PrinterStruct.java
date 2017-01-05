/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.libs.structs;

import java.util.ArrayList;

public class PrinterStruct
{
   public String printerName;
   public String printingMethod;
   public String location;
   public int jobMaxBytes;
   public String printerType;
   public String hostIPAddress;
   public int securityClass;
   public boolean queueEnabled;
   public boolean printerEnabled;
   public boolean generalAccess;
   public String portName;
   public boolean suspendRequests;
   public String redirectionPtr;
   public String fileName;
   public String adapterIPAddress;
   public boolean fileAppendFlag;
   public String ntHostName;
   public String ntShareName;
   public String driverName;
   public ArrayList appList;
}
