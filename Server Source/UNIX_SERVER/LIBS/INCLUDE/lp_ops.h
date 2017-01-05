
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 



/* a few defines */

#define PRINTER_ENABLED         1
#define PRINTER_DISABLED        0
#define QUEUE_ENABLED           1
#define QUEUE_DISABLED          0
#define SCHEDULER_ENABLED       1
#define SCHEDULER_DISABLED      0


/* prototype defs */
#ifdef __cplusplus
 extern "C"{
#endif

int lp_accept(const char*);
int lp_disable(const char*);
int lp_enable(const char*);
int lp_exists(const char*);
int lp_isaccept(const char*);
int lp_isenable(const char*);
int lp_issched(const char*);
int lp_reject(const char*);
int lp_shut(void);
int lp_start(void);

#ifdef __cplusplus
 };
#endif

