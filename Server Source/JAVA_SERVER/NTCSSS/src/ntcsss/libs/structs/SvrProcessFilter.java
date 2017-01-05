/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.libs.structs;

/**
 * A filter used for selecting particular server processes
 */
public class SvrProcessFilter 
{
   public String commandLine;
   public String custProcStatus;
   public String loginName;
   public String unixOwner;
   public String progroupTitle;
   public String jobDescription;
   public String originatingHost;
   public int priority;
   
   public SvrProcessFilter(String commandLine, String custProcStatus, 
                           String loginName, String unixOwner, 
                           String progroupTitle, String jobDescription, 
                           String originatingHost, int priority)
   {
      this.commandLine = commandLine;
      this.custProcStatus = custProcStatus;
      this.loginName = loginName;
      this.unixOwner = unixOwner;
      this.progroupTitle = progroupTitle;
      this.jobDescription = jobDescription;
      this.originatingHost = originatingHost;
      this.priority = priority;
   }
}
