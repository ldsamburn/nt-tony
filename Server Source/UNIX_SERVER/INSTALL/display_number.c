/****************************************************************************

               Copyright (c)2002 Northrop Grumman Corporation

                           All Rights Reserved


     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int,char**);

/* Arg-1 = User ID */
/* Arg-2 = Current value */
/* Arg-3 = Total value   */
/* Compute and display: (current) and (percentage) */

int main(int argc,char **argv)
{
int current,total;
double percentage;
char userid[10];

    if(argc<3)
    {
        printf("Display arguments missing\n\n");
        printf("Format: display_number {UserID} {small#} {large#}\n\n");
        printf("Gives: {UserID} {small#} {large#} {percentage}\n");
        exit (-1);
    }

    memset(userid,0,10);
    strcpy(userid,argv[1]);
    current=atoi(argv[2]);
    total=atoi(argv[3]);    
    percentage = (double) current / (double) total * 100.;

    printf("--> (%s) (%d) of (%d) -->(%.0f percent)                    \r",
           userid,current,total,percentage);
    exit (0);

}
