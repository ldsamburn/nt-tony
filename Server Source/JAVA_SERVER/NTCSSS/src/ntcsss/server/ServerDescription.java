/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.server;

/**
 * A container for information about a server
 */
public class ServerDescription
{
   private MessageHandlerDescription[] messageHandlers;
   private String serverName;
   private ThreadGroup logEntityGroup;
   private Class serverClass;  
   
   /**
    * Constructor
    */
   public ServerDescription(String serverName, Class serverClass,
                            MessageHandlerDescription[] messageHandlers,
                            ThreadGroup logEntityGroup)
   {
      this.messageHandlers = messageHandlers;
      this.serverName = serverName;
      this.logEntityGroup = logEntityGroup;
      this.serverClass = serverClass;
   }

   /**
    * Returns the server name
    */ 
   public String getServerName()
   {
      return serverName;
   }

   /**
    * Returns the message handlers
    */ 
   public MessageHandlerDescription[] getMessageHandlers()
   {
      return messageHandlers;
   }
   
   /**
    * Returns the log entity group
    */ 
   public ThreadGroup getLogEntityGroup()
   {
      return logEntityGroup;
   }    
   
   /**
    * Returns the server class
    */ 
   public Class getServerClass()
   {
      return serverClass;
   }
}