/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.libs.structs;

import ntcsss.libs.Constants;

/**
 * A data structure for holding state information about a server. This is 
 *  mainly used in conjunction with the SvrStatLib
 */
public class SvrStatus
{
    
   private int serverState, loginStatus, debugState;
   
   public SvrStatus(int serverState, int loginStatus, int debugState)
   {
      this.serverState = serverState;
      this.loginStatus = loginStatus;
      this.debugState = debugState;
   }
   
   public boolean isServerAwake()
   {
      if (serverState == Constants.SERVER_STATE_AWAKE)
         return true;
      else
         return false;
   }
   
   public boolean isServerIdled()
   {
      if (serverState == Constants.SERVER_STATE_IDLE)
         return true;
      else
         return false;
   }
   
   /**
    * Returns true if the server is up regardless of its state
    */
   public boolean isServerRunning()
   {
      if ((serverState == Constants.SERVER_STATE_AWAKE) ||
          (serverState == Constants.SERVER_STATE_IDLE))
         return true;
      else
         return false;
   }
   
   public boolean areLoginsAllowed()
   {            
      if (loginStatus == Constants.SERVER_STATE_OFFLOCK)      
         return true;
      else
         return false;
   }
   
   public boolean areLoginsBlocked()
   {            
      if (loginStatus == Constants.SERVER_STATE_ONLOCK)      
         return true;
      else
         return false;
   }
            
   public boolean isDebugOn()
   {
      if (debugState == Constants.SERVER_STATE_ONDEBUG)      
         return true;
      else
         return false;
   }
   
   public boolean isDebugOff()
   {
      if (debugState == Constants.SERVER_STATE_OFFDEBUG)      
         return true;
      else
         return false;
   }
} 

