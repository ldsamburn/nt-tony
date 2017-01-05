/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.log;

public class EntityName
{
   public static EntityName UNKNOWN       = new EntityName("UNKNOWN");
   public static EntityName NTCSS_START   = new EntityName("NTCSS_INIT");
   public static EntityName NTCSS_STOP    = new EntityName("NTCSS_STOP");   
   public static EntityName NTCSS_CMD     = new EntityName("NTCSS_CMD");
   public static EntityName FTPD_GRP      = new EntityName("NCSERVER");
   public static EntityName PRT_LISTD     = new EntityName("PRT_LISTD");
   public static EntityName NTCSS_LP      = new EntityName("NTCSS_LP");
   
   public static ThreadGroup PRTQ_SVR_GRP = new ThreadGroup("PRTQ_SVR");
   public static ThreadGroup DB_SVR_GRP   = new ThreadGroup("DB_SVR");
   public static ThreadGroup DSK_SVR_GRP  = new ThreadGroup("DSK_SVR");
   public static ThreadGroup CMD_SVR_GRP  = new ThreadGroup("CMD_SVR");
   public static ThreadGroup SPQ_SVR_GRP  = new ThreadGroup("SPQ_SVR");
   public static ThreadGroup MSG_SVR_GRP  = new ThreadGroup("MSG_SVR");      
   
   private String name;
   
   protected EntityName(String name)
   {
      this.name = name;
   }
   
   /**
    * Returns the text of this entity name
    */
   public String get()
   {
      return name;
   }
}


