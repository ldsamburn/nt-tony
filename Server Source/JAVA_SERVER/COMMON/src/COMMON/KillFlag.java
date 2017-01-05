/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package COMMON;

public class KillFlag
{
   private boolean flag;

   public KillFlag()
   {
      flag = false;
   }

   // Sets the flag to "kill"
   public synchronized void set()
   {
      flag = true;
   }

   // Returns the state of the flag
   public synchronized boolean isSet()
   {
      return flag;
   }
}