/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.server;

import java.lang.reflect.Method;

public class MessageHandlerDescription
{
   private String message;
   private Method handler;

   public MessageHandlerDescription(String message, Method handler)
   {
      this.message = message;
      this.handler = handler;
   }

   public String getMessage()
   {
      return message;
   }

   public Method getHandler()
   {
      return handler;
   }
}