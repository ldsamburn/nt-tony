/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package CNTRL_PNL.INFO;

public class HostInfoListEntry
{
   private String hostname;
   private String IPaddress;
   private int serverType;
   private int serverStatus;
   private int cpServerState;

   public HostInfoListEntry(String hostname, String IPaddress, int serverType,
                            int serverStatus)
   {
      // Fill in the fields for this list entry
      this.hostname = hostname;
      this.IPaddress = IPaddress;
      setServerType(serverType);
      setServerStatus(serverStatus);
      setCPServerState(NtcssServersInfo.CP_SVR_UP);
   }

   // Returns the hostname field
   public String getHostname()
   {
      return hostname;
   }

   // Returns the IP address field
   public String getIPaddress()
   {
      return IPaddress;
   }

   // Returns the server type field
   public int getServerType()
   {
      return serverType;
   }

   // Sets the server type field to the specified type
   public void setServerType(int type)
   {
      serverType = type;
   }

   // Returns the server status field
   public int getServerStatus()
   {
      return serverStatus;
   }

   // Sets the server status field to the specified status
   public void setServerStatus(int status)
   {
      serverStatus = status;
   }

   // Returns the control panel server state
   public int getCPServerState()
   {
      return cpServerState;
   }

   // Sets the control panel server state to the specified state
   public void setCPServerState(int state)
   {
      cpServerState = state;
   }
}