
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


#include "stdio.h"
#include "Ntcss_gwy.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>

#define INPUT_PIPE_NAME "/usr/local/NTCSSII/spool/pipes/dm_input_pipe"
#define OUTPUT_PIPE_NAME "/usr/local/NTCSSII/spool/pipes/dm_ret_pipe"

#define DM_MSG_LEN 20
#define HOST_LIST_SIZE_LEN 5

extern char *sys_errlist[];


int main (argc, argv)
int argc;
char *argv[];
{
  
  int nContinue = 1;
  char strJunk[1024], ch, strMessage[30];
  int  outputFd, inputFd;

  outputFd = open(INPUT_PIPE_NAME, O_WRONLY);
  if (!outputFd) {
    printf("open failed\n");
  }

  while (nContinue) {  
    printf("Press enter to write to pipe (x to exit)\n");
    gets(strJunk);
    if (strcmp(strJunk, "x") == 0) {
      nContinue = 0;
    } /* END if */
    
    printf("Writing to pipe\n");
    memset(strMessage,'\0', 25);
    sprintf(strMessage, "AUTHUSER");
    if (write(outputFd, strMessage, 20) != 20) {
      printf("write message failed\n");
    }

    /*
    if (write(outputFd, "00015", 5) != 5) {
      printf("write hostlist len failed\n");
    }

    if (write(outputFd, "ntcssdmo, host2", 15) != 15) {
      printf("write ALLHOSTS failed\n");
    }
    */
    
    if (write(outputFd, "00008", 5) != 5) {
      printf("write hostlist len failed\n");
    }

    if (write(outputFd, "ALLHOSTS", 8) != 8) {
      printf("write ALLHOSTS failed\n");
    }
    
    
    if (write(outputFd, "Msg To Distribute  ", 20) != 20) {
      printf("write message to dist failed\n");
    }

    if (write(outputFd, "Data to go with message", 24 ) != 24 ) {
      printf("write data with msg failed\n");
    }

    inputFd = open(OUTPUT_PIPE_NAME, O_RDONLY);
    if (!inputFd) {
      printf("open failed\n");
      nContinue = 0;
    } /* END if */

    printf("Read from pipe: \n");
    ch = '1';
    while (ch != '\0') {
      if (read(inputFd, &ch, 1) != 1) {
	printf("read failed\n");
	ch = '\0';
      } else {
	printf("%c", ch);
      } /* END if */
    } /* END while */
    printf("\n");
    close(inputFd);
  } /* END while */

  close(outputFd);
  exit(0);
}



