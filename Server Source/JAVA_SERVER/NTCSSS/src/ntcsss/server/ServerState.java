/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.server;

import ntcsss.libs.Constants;

/*
 * This class contains the "state" of the server
 */
public class ServerState
{
   private int state;
   private boolean loginLock;

   /*
    * Initializes the state of the server to "awake" and logins lock to "off"
    */
   public ServerState() 
   {
      state = Constants.SERVER_STATE_AWAKE;
      loginLock = false;
   }
   
   /**
    * Sets the "state" member to the given state
    */
   private synchronized void setState(int newState)
   {
      state = newState;
   }      
   
   /**
    * Returns the current state
    */
   public synchronized int getState()
   {
      return state;
   }
   
   /**
    * Sets the "state" member to the "idle" state
    */
   public void idle()
   {
      setState(Constants.SERVER_STATE_IDLE);
   }
   
   /**
    * Returns true if the state is "idle"
    */
   public boolean isIdle()
   {
      return (getState() == Constants.SERVER_STATE_IDLE);         
   }
   
   /**
    * Sets the "state" member to the "awake" state
    */
   public void awaken()
   {
      setState(Constants.SERVER_STATE_AWAKE);
   }
   
   /**
    * Returns true if the state is "awake"
    */
   public boolean isAwake()
   {
      return (getState() == Constants.SERVER_STATE_AWAKE);         
   }
   
   /**
    * Sets the "state" member to the "die" state
    */
   public void die()
   {
      setState(Constants.SERVER_STATE_DIE);
   }
   
   /**
    * Returns true if the state is "die"
    */
   public boolean isDie()
   {
      return (getState() == Constants.SERVER_STATE_DIE);         
   }
   
   /**
    * Turns on login lock
    */
   public synchronized void turnOnLoginLock()
   {
      loginLock = true;
   }
   
   /**
    * Turns off login lock
    */
   public synchronized void turnOffLoginLock()
   {
      loginLock = false;
   }
   
   /**
    * Checks whether logins are allowed
    */
   public synchronized boolean isLoginLockOn()
   {
      return loginLock;
   }
}
