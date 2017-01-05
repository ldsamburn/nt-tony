/****************************************************************************

               Copyright (c)2002 Northrop Grumman Corporation

                           All Rights Reserved


     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define XREF_RS (size_t) 128
#define GROUP_SIZE (size_t)   32 

#include <StringFcts.h>
#include <Ntcss.h>
#include <LibNtcssFcts.h>
#include <ini_Fcts.h>
#include <TextDBP.h>
#include <ExtraProtos.h>
#include <NetCopy.h>
#include <SockFcts.h>

int main(int,char**);
/* Remove brackets from buffer and pack left */
int remove_brackets(char*, char*, int);
/* Get left half of equation */
int split_left(char*,char*);
/* Get right half of equation */
int split_right(char*,char*);
/* Scan settings file for left argument and return into right argument */
int read_ntcss_system_settings(const char*,char*);
/* Check return status from reading settings file */
void settings_status(int,char*);
/* Get locations of NTCSSII directories */
void get_paths(char*,char*);

int main( int argc, char **argv )
{
FILE *fp,*ofp,*uxfp,*appusersfp,*ntcss_usersfp,*masterxreffp;
FILE *finalfp,*userlistfp;
char filename_pg[XREF_RS],filename_nux[XREF_RS];
char filename_unx[XREF_RS],filename_appusers[XREF_RS];
char filename_ntcss_users[XREF_RS],filename_master_xref[XREF_RS];
char filename_final[XREF_RS],filename_user_list[XREF_RS];
char results_buf[XREF_RS];
char inbuf[XREF_RS],current_app[XREF_RS];
char leftbuf[XREF_RS];
char db_dir[XREF_RS],progrps_file[XREF_RS],userid[XREF_RS];
char lastuser[XREF_RS],ng[XREF_RS],ug[XREF_RS],appbuf[MAX_INI_ENTRY * 100];
char *fr,*tptr,*savetptr,group[XREF_RS],uid[XREF_RS],uc_path[1024],ntcss_home_dir[128];
char *env;
int i,j,s,t,x,len,wait_for_group,first_time,ignore;

    /* Just to avoid compiler warnings...*/
    if (argc != argc)
      argv = argv;
/*
* Set path
*/
    x=read_ntcss_system_settings("HOME_DIR",ntcss_home_dir);
    ntcss_home_dir[ strlen(ntcss_home_dir) - 1 ] = 0;
    if (x < 0)
    {
        printf("WARNING: (Init_Database) Cannot determine NTCSS home"
                " directory for reconciling users.\n");
        exit(-1);
    }
    memset(uc_path,'\0',1024);
    env=getenv("PATH");
    if(strstr(env,ntcss_home_dir)==NULL)
    {
        sprintf(uc_path,"%s:%s/bin",env,ntcss_home_dir);
        putenv(uc_path);
        printf("ENV: (%s)\n",uc_path);
    }

    get_paths(progrps_file,db_dir);
    wait_for_group=0;

    strcpy(filename_pg,progrps_file);          /* progrps.ini file path */
    strcat(filename_nux,db_dir);               /* "/h/NTCSSS/database/"  */
    strcpy(filename_unx,db_dir);               /* "/h/NTCSSS/database/" */
    strcpy(filename_appusers,db_dir);          /* "/h/NTCSSS/database/" */
    strcpy(filename_ntcss_users,db_dir);       /* "/h/NTCSSS/database/" */
    strcpy(filename_master_xref,db_dir);       /* "/h/NTCSSS/database/" */
    strcpy(filename_final,db_dir);             /* "/h/NTCSSS/database/" */
    strcpy(filename_user_list,db_dir);         /* "/h/NTCSSS/database/" */
    strcat(filename_nux,"ntcss_to_unix_group_xref.tmp"); /* Add filename */
    strcat(filename_unx,"user_to_ntcss_group_xref.tmp"); /* Add filenamme*/
    strcat(filename_appusers,"appusers");      /* Add filename */
    strcat(filename_ntcss_users,"ntcss_users");/* Add filename */
    strcat(filename_user_list,"userlist.tmp");  /* Add filename */
    strcat(filename_master_xref,"ntcss_master_xref.tmp");/* Add filename */
    strcat(filename_final,"final.tmp");        /* Add filename */

    fp=fopen(filename_pg, "r");
    if(fp==NULL)
    {
        printf("Bad open of progrps.ini\n");
        exit (-1);
    }
    ofp=fopen(filename_nux, "w");
    if(ofp==NULL)
    {
        printf("Bad open of ntcss_to_unix_group_xref.tmp\n");
        exit (-1);
    }
    uxfp=fopen(filename_unx, "w");
    if(uxfp==NULL)
    {
        printf("Bad open of user_to_ntcss_group_xref.tmp file\n");
        exit (-1);
    }
    appusersfp=fopen(filename_appusers, "r");
    if(appusersfp==NULL)
    {
        printf("Bad open of appusers file\n");
        exit (-1);
    }
    ntcss_usersfp=fopen(filename_ntcss_users, "r");
    if(ntcss_usersfp==NULL)
    {
        printf("Bad open of ntcss_users file\n");
        exit (-1);
    }
    masterxreffp=fopen(filename_master_xref, "w");
    if(masterxreffp==NULL)
    {
        printf("Bad open of ntcss_master_xref.tmp\n");
        exit (-1);
    }
    finalfp=fopen(filename_final, "w");
    if(finalfp==NULL)
    {
        printf("Bad open of final.tmp\n");
        exit (-1);
    }

    userlistfp=fopen(filename_user_list, "w");
    if(userlistfp==NULL)
    {
        printf("Bad open of userlist.tmp\n");
        exit (-1);
    }

    /* collect apps on this host */
    memset(appbuf,NULL,MAX_INI_ENTRY * 100);
    GetSectionValues("APPS",appbuf,MAX_INI_ENTRY * 100,NTCSS_PROGRP_ID_FILE);
    tptr=appbuf;

    /* Create list of users from ntcss_users file */
    while(1)
    {
        fr=fgets(inbuf,(int)XREF_RS,ntcss_usersfp); /* Throw away unix #*/
        memset(inbuf,0,XREF_RS);
        fr=fgets(inbuf,(int)XREF_RS,ntcss_usersfp);  /* Get User name */
        if(fr == NULL)
            break;
        fprintf(userlistfp,"%s",inbuf);
        for(i=0;i<11;i++)
        {
            fr=fgets(inbuf,(int)XREF_RS,ntcss_usersfp); /*Throw away fill*/
        }
    }
    printf("USER_INTEG: User list created\n");

    /* Create ntcss_to_unix_xref.tmp file */
    /* Read progrps.ini file and throw away first record ([APPS] line) */
    fr=fgets(inbuf,(int)XREF_RS,fp);        
    while (1)
    {
        memset(inbuf,0,XREF_RS);
        fr=fgets(inbuf,(int)XREF_RS,fp);        
        if(fr == NULL)
        {
            /* printf("EOF detected while reading progrps.ini\n"); */
            break;
        }
        if(wait_for_group==1) /* Means brackets have been encountered */
        {
            len=strlen(inbuf);
            s=split_left(inbuf,leftbuf);
            if(strcmp(leftbuf,"UNIX_GROUP")==0)
            {
                wait_for_group=0;
                s=split_right(inbuf,group);
                /* printf("%s %s",current_app,group); */
                fprintf(ofp,"%s %s",current_app,group);
            }
        }
        ignore=0;
        savetptr=tptr;
        j=remove_brackets(inbuf,tptr,ignore);
        tptr=savetptr;
        if(j!=0)
        {
            strcpy(current_app,inbuf);
            wait_for_group=1;
        }
    } /* End for */
    printf("USER_INTEG: NTCSS groups extracted from progrps.ini\n");

    /* Done with extracting ntcss groups to unix groups xref */

    /* Now do users to ntcss group xref */
    /* Format: 'userid ntcssgroup'  */

    while (1)
    {
        memset(userid,0,XREF_RS);
        fr=fgets(group,(int)XREF_RS,appusersfp); /* Get NTCSS group */
        if(fr == NULL)
        {
            /* printf("EOF detected while reading appusers\n"); */
            break;
        }
        strcpy(userid,group);
        fr=fgets(userid,(int)XREF_RS,appusersfp); /* Get User ID */
        userid[strlen(userid)-1]=' ';
        strcat(userid,group);
        fprintf(uxfp,"%s",userid);    
        for(j=0;j<6;j++) /* Filler lines */
        {
            fr=fgets(inbuf,(int)XREF_RS,appusersfp);
            if(fr == NULL)
            {
                printf("EOF detected while reading filler records "
                       "from appusers file\n");
                break;
            }
        } /* End for */
    } /* End for */

    /* Now boil it down to "UserId:ntcssgroup unixgroup1 unixgroup2 etc... */

    fclose(uxfp);
    fclose(ofp);
    uxfp=fopen(filename_unx, "r");
    if(uxfp==NULL)
    {
        printf("Bad re-open of user_to_ntcss_group_xref.tmp in (read mode)\n");
        exit (-1);
    }
    ofp=fopen(filename_nux, "r");
    if(ofp==NULL)
    {
        printf("Bad re-open of ntcss_to_unix_group_xref.tmp in (read mode)\n");
        exit (-1);
    }

    s=0;
    while(s >= 0) /* Scan user_to_ntcss_group_xref.tmp file */
    {
        /* Get record from User->Ntcss group file */
        /* Record format: "UserID Ntcss_Group" */
        /* printf("Getting next record\n"); */
        s=fscanf(uxfp,"%s%s",uid,group);
        rewind(ofp);
        t=0;
        while(t>=0) /* Scan ntcss_to_unix_group_xref.tmp file */
        {
            t=fscanf(ofp,"%s%s",ng,ug); /* Format: NtcssGroup UnixGroup */     
            /* Match group and x-reference to unix group */
            if(strcmp(group,ng)==0) /* If match between Ntcss and unix groups */
            {
                fprintf(masterxreffp,"%s %s\n",uid,ug); /* Write master rec */
                /* printf("%s %s\n",uid,ug); */
                break;
            }
        } /* End of read of ntcss_to_unix_group_xref.tmp file,break comes here*/
    } /* End read of user_to_ntcss_group_xref.tmp file */
    printf("USER_INTEG: Master Xref file created\n");

    /* Reduce each user: "UserID:Unix_Group-0 Unix_Group-1 etc...*/

    fclose(masterxreffp);
    masterxreffp=fopen(filename_master_xref, "r");
    if(masterxreffp==NULL)
    {
        printf("Bad open of ntcss_master_xref.tmp in read mode\n");
        exit (-1);
    }
    
    strcpy(lastuser,"--");
    strcpy(group,"--");
    first_time=1;

    s=0;
    while(s >= 0)
    {
        s=fscanf(masterxreffp,"%s%s",uid,ug);
        if(s < 0)
        {
            sprintf(results_buf,"%s ",ug); /* 'Unix_Group-A Unix_Group-B */
            strcat(inbuf,results_buf);     /* 'UID:UG UG-A UG-B etc.' */
            /* printf("EOF Detected on master_xref file\n"); */
            break;
        }
        if(first_time==1) 
        {
            first_time=0;                 /* First time, 'UserID:Unix_Group' */ 
            sprintf(inbuf,"%s:%s ",uid,ug);
            strcpy(group,ug);
        }
        else                              /* Not first time */
        {
            if(strcmp(uid,lastuser)==0)   /* Same UserID as last time? */
            {
                if(strcmp(ug,group)!=0) /* Same group as last time? */
                { /* different group */
                    strcat(inbuf," ");  /* Same UserID */
                    strcat(inbuf,ug);   /* Add in this unix group */
                    strcpy(group,ug);   /* group is now last group processed */
                }
            }
                else                     /* Different UserID */
            {
            fprintf(finalfp,"%s\n",inbuf);
            memset(inbuf,0,XREF_RS);
            sprintf(inbuf,"%s:%s",uid,ug);
            strcpy(group,ug);            /* New user gets new group */
            }    
        } /* End 'not first time' */
        strcpy(lastuser,uid);
    } /* End reading master xref file */
    printf("USER_INTEG: Final list created\n");
    
    fclose(fp);
    fclose(ofp);
    fclose(uxfp);
    fclose(appusersfp);
    fclose(ntcss_usersfp);
    fclose(masterxreffp);
    fclose(finalfp);

    exit (0);
}

int remove_brackets( char* buffer, char* appsptr, int ig )
{
    int i,j,len,appfound;
    char wbuf[XREF_RS],*newtptr;

    len=strlen(buffer);
    if(strlen(buffer)<2)
        return(0);
    if(buffer[0]!='[')
        return(0);
    j=1;
    for(i=0;i<len;i++)
    {
       wbuf[i]=buffer[j++]; 
    } 
    for(i=len;i>0;i--)
    {
       if(wbuf[i]==']')
       {
           wbuf[i]='\0';
           break;
       }
    }
    wbuf[XREF_RS]='\0';
    for(i=0;i<len;i++)
    {
        buffer[i]=wbuf[i];
        if(buffer[i]=='\0')
            break;
    }
    buffer[i+1]='\0';
    if(ig == 1)
        return(0);
    newtptr=appsptr;
    appfound=0;
    while (strlen(newtptr) > 0)
    {
        if(strcmp(buffer,newtptr) == 0)
	{
            appfound=1;
            break;
	}
        newtptr += strlen(newtptr) + 1;
    }
    if(appfound==1) {
        return(1);
    }
    return(0);
}

int split_left(char *inbuf, char *retbuf)
{
/*
   Return left half of equation contained in inbuf back in retbuf
   Returns 1 if brackets removed
   Returns 0 if no brackets removed
*/

    int i,len;
    len=strlen(inbuf);
    if(len<2)
        return(0);
    for(i=0;i<len;i++)
    {
        retbuf[i]=inbuf[i];
        if(inbuf[i]=='=')
        {
            retbuf[i]=(char)0;
            return (1);
        }
    }
    return (0);
}


int split_right(char *inbuf, char *retbuf)
{
/*
   Return right half of equation contained in inbuf back in retbuf
   Returns 1 if something extracted
   Returns 0 if nothing extracted
*/

    int i,j,rptr,len;
    len=strlen(inbuf);
    if(len<2)
        return (0);
    memset(retbuf,0,XREF_RS);
    for(i=0;i<len;i++)
    {
        if(inbuf[i]=='=')
            break;
    }
    if(i==len)
    {
        retbuf[XREF_RS]=(char)0;
        return (0);
    }
    rptr=0;
    for(j=i+1;j<len;j++)
    {
        retbuf[rptr++]=inbuf[j];
        retbuf[XREF_RS]=(char)0;
    }
    return (1);
}

int read_ntcss_system_settings(const char *search_pattern, char *results_buf)
{
/* Look up search_pattern, return answer in results, like 'GetProfileString'
   Returns 1 if pattern not found
   Returns 0 if pattern found
*/
    int j,len,ignore;
    char inbuf[XREF_RS],local_results[XREF_RS],*fr,dummy[7];
    FILE *settingsfp;

    settingsfp=fopen("/etc/ntcss_system_settings.ini","r");
    if(settingsfp==NULL)
    {
        printf("Bad open of /etc/ntcss_system_settings.ini file\n");
        exit (-1);
    }
    while (1)
    {
        memset(inbuf,0,XREF_RS);
        fr=fgets(inbuf,(int)XREF_RS,settingsfp);
        if(fr == NULL)
        {
            return(1);
        }
        ignore=1;
        j=remove_brackets(inbuf,dummy,ignore); /* Ignore j */
        len=strlen(search_pattern);
        j=split_left(inbuf,local_results);
        if(strcmp(local_results,search_pattern)==0)        
        {
            j=split_right(inbuf,local_results);
            break;
        }
    }
    strcpy(results_buf,local_results);
    fclose(settingsfp);
    return(0);
}

void settings_status(int i, char *value)
{
/*
   Check results of read from /etc/ntcss_system_settings.ini file
*/
    if(i!=0)
    {
        printf("Unable to read %s from /etc/ntcss_system_settings.ini "
               "file",value);
        exit(-1);
    }
}

void get_paths(char *pg_file, char *db_dir) 
{
/*
   Assemble file path names
*/
    int i;
    char home_dir[XREF_RS],id_dir[XREF_RS],save_home_dir[XREF_RS];
    char save_db_dir[XREF_RS], save_id_dir[XREF_RS],lookup[XREF_RS];
    strcpy(lookup,"HOME_DIR");
    i=read_ntcss_system_settings(lookup,save_home_dir);
    settings_status(i,lookup);
    if(i==0)
        strcpy(home_dir,save_home_dir);   /* Save home_dir */
    strcpy(lookup,"DB_DIR");
    i=read_ntcss_system_settings(lookup,save_db_dir);
    settings_status(i,lookup);
    strcpy(lookup,"INIT_DATA");
    i=read_ntcss_system_settings(lookup,save_id_dir);
    settings_status(i,lookup);
    /* Assemble progrps.ini file full path */
    memset(pg_file,0,XREF_RS);
    memset(db_dir,0,XREF_RS);
    memset(id_dir,0,XREF_RS);
    strcat(pg_file,home_dir); /* Home dir first */
    pg_file[strlen(pg_file)-1]='/';/* Base path assembled with slash at end */
    strcpy(id_dir,pg_file);        /* Start init_data dir base path */
    strcat(pg_file,save_id_dir);   /* Add init_data directory */
    pg_file[strlen(pg_file)-1]='/';/* Add slash at end */
    strcat(pg_file,"progrps.ini"); /* Add file progrps.ini file name */
    strcat(db_dir,save_db_dir);    /* Add database dir to base path */
    db_dir[strlen(db_dir)-1]='/';  /* Add slash at end */
    return;
}
