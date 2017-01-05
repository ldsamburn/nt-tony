
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 2005).

Aug 15, 2003		Original Coding		Anthony Amburn

****************************************************************************/ 


/*
 * ini_Fcts.c
 */

#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/param.h>
/*
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
*/
#include <stdlib.h>
#include <ctype.h>

#include <ini_Fcts.h>

#define NONE    0
#define SECTION 1
#define ITEM    2

#define SECTION_BEGIN_DELIMITER	'['
#define SECTION_END_DELIMITER	']'
#define TAG_VALUE_DELIMITER		'='
#define STRINGS_DELIMITER       '%'
#define STRINGS_DELIM_STR       "%"
#define STRINGS_LABEL           "Strings"


typedef struct _INI_Item {
	char item[MAX_INI_ENTRY];
	char value[MAX_INI_ENTRY];
	struct _INI_Item *next_item;
} INI_Item; 

typedef struct _INI_Section  {
	char section[MAX_INI_ENTRY];
	struct _INI_Item *first_item;
	struct _INI_Section *next_section;
} INI_Section;

typedef struct _INI_Instance {
	char fn[MAXPATHLEN];
	struct _INI_Section *head_sect;
	struct _INI_Instance *next_inst;
    int strings_sect_flag;
} INI_Instance;



static struct _INI_Instance *head_inst=NULL;

static INI_Instance  *AddINIInstance(const char*);
static int   AddINIItemLink( const char*, const char*, struct _INI_Instance* );
static int            AddINISectionLink( const char*, struct _INI_Instance* );
static int            DelINIItem(struct _INI_Section*, const char*);
static int            DetermineEntryType( const char*);
static char          *ExpandStrings(char*, struct _INI_Section*);
static INI_Instance  *FindINIInstance(const char*);
static INI_Section   *FindINISection(const char*, struct _INI_Section*);
static struct _INI_Item  *FindINIItem(struct _INI_Section*, const char*);
static int        GetINISection(const char*, char*, int, struct _INI_Section*);
static int        GetINISectionNames(char*, int, struct _INI_Section*);
static int 	  DelINISection(char *,char *);


/*****************************************************************************/

int  LoadINIFile( const char *filename )

{
  FILE  *fp;
  char   ini_entry[MAX_INI_ENTRY];
  char  *ini_entry_ptr;
  int    state_ind;
  char   section[MAX_INI_ENTRY];
  struct _INI_Section   *h_sect;
  struct _INI_Instance  *cur_inst;

/*
	load_indicator = INI_LOADED;
	changes_made = 0;
*/
  h_sect = NULL;

  state_ind = NONE;

  cur_inst = FindINIInstance(filename);
  if (cur_inst)
    return(0);
  else
    {
      cur_inst = AddINIInstance(filename);
	}

  fp = fopen(filename, "r");
  if (fp == NULL)  
    return(0);
	
  while (!feof(fp))
    {
      ini_entry_ptr = fgets(ini_entry, MAX_INI_ENTRY, fp);
      if (ini_entry_ptr == NULL)
        break;	

      if (ini_entry_ptr[strlen(ini_entry_ptr)-1] == '\n')
        ini_entry_ptr[strlen(ini_entry_ptr)-1] = 0;
      switch(DetermineEntryType(ini_entry))
		{
        case SECTION:
          memset(section, '\0', MAX_INI_ENTRY);
          strncpy(section,ini_entry + 1, strlen(ini_entry) - 2);

          AddINISectionLink(ini_entry, cur_inst);
          if (cur_inst->head_sect == NULL)
            {
              printf("Error Adding Section Link.\n");
              fclose(fp);
              return(0);
            }
          state_ind = SECTION;
          break;
        case ITEM:
          if (cur_inst->head_sect == NULL)
            {
              break;
            } 
          if (AddINIItemLink(section, ini_entry, cur_inst))
            {
              printf("Error Adding Item Link.\n");
              fclose(fp);
              return(0);
            }
					
          state_ind = ITEM;
          break;
        default:   	/* must be comment */
          break;
		}
	}
  fclose(fp);
  return(0);
}


/*****************************************************************************/

int  FlushINIFile( const char *fn )

{
  struct _INI_Section   *ts;
  struct _INI_Item      *is;
  struct _INI_Instance  *cur_inst;
  FILE  *fp;

  cur_inst = FindINIInstance(fn);
  if (!cur_inst)
    return(-1);

  fp = fopen(cur_inst->fn, "w");
  if (fp == NULL) 
    return (-1);
	
  if (cur_inst->head_sect == NULL)
    fclose(fp);
  else
    {
      ts = cur_inst->head_sect;
      while (ts != NULL)
        {
          fprintf(fp, "[%s]\n", ts->section);
          is = ts->first_item;
          if (is)
            {
              while (is)
                {
                  fprintf(fp, "%s=%s\n", is->item, is->value);
                  is = is->next_item;
				}
			}
          fprintf(fp, "\n");
          ts = ts->next_section;
		}
	}
  fclose(fp);
  return(0);
}


/*****************************************************************************/

int  GetItemValues( const char  *entry,
                    char        *tag,
                    char        *value )

{
  char  *tmpptr;
  char   entry_copy[MAX_INI_ENTRY];

  strcpy(entry_copy, entry);

  tmpptr = strchr(entry_copy, TAG_VALUE_DELIMITER);
  if (tmpptr)
    {
      strcpy(value, tmpptr + 1);
      *tmpptr = 0;
      strcpy(tag, entry_copy);
      return(0);
	}
  else
    return(-1);
}


/*****************************************************************************/

static int DetermineEntryType( const char *entry )

{
  char  *tmpptr;

  if (entry[0] == SECTION_BEGIN_DELIMITER)
    return(SECTION);

  tmpptr = strchr(entry, TAG_VALUE_DELIMITER);
  if (tmpptr != NULL)
    return(ITEM);

  return(NONE);

}


/*****************************************************************************/

static int  AddINISectionLink( const char            *entry,
                               struct _INI_Instance  *cur_inst )

{
  struct _INI_Section  *ts;


	if (cur_inst->head_sect == NULL)
      {
		cur_inst->head_sect =
          (struct _INI_Section *)malloc(sizeof(struct _INI_Section));
		ts = cur_inst->head_sect;
      }
    else
      {
		ts = cur_inst->head_sect;
		while(ts->next_section) 
          ts = ts->next_section;
		ts->next_section =
          (struct _INI_Section *)malloc(sizeof(struct _INI_Section));
		ts = ts->next_section;
      }

	memset(ts->section,'\0',MAX_INI_ENTRY);
	strncpy(ts->section, entry + 1, strlen(entry) - 2);

	ts->next_section = NULL;
	ts->first_item = NULL;

    if (!strncmp(ts->section, STRINGS_LABEL, strlen(STRINGS_LABEL)))
      cur_inst->strings_sect_flag = 1;

	return(0);
}


/*****************************************************************************/

static int  AddINIItemLink( const char            *section,
                            const char            *entry,
                            struct _INI_Instance  *cur_inst )

{
  struct _INI_Section *ts;
  struct _INI_Item *is;
  struct _INI_Item *is1;

  ts = FindINISection(section, cur_inst->head_sect);
  if (ts == NULL)
    {
      AddINISectionLink(section, cur_inst);
      ts = FindINISection(section, cur_inst->head_sect);
	}
	
	is = ts->first_item;
	if (is == NULL) {
		ts->first_item=(struct _INI_Item *)malloc(sizeof(struct _INI_Item));
		is=ts->first_item;
		is1=is;
	} else {
		is=ts->first_item;
		while(is->next_item)
			is=is->next_item;

		is->next_item=(struct _INI_Item *)malloc(sizeof(struct _INI_Item));
		is1=is;
		is=is->next_item;
	}
	if (GetItemValues(entry, is->item, is->value)) {
		printf("INI File OUT OF Format!\n");
		free(is);
		is=is1;
		return(1);
	}
	is->next_item=NULL;
		
	return(0);
}


/*****************************************************************************/

int  WalkINIEntries( const char  *fn )

{
  struct _INI_Section   *ts;
  struct _INI_Item      *is;
  struct _INI_Instance  *cur_inst;
  
  if(LoadINIFile(fn) < 0) 
    return(-1);

  cur_inst = FindINIInstance(fn);
	
  if (cur_inst->head_sect == NULL) 
    printf("No Sections Found.\n");
  else
    {
      ts = cur_inst->head_sect;
      while (ts != NULL)
        {
          printf("Section -> %s\n", ts->section);
          is = ts->first_item;
          if (is == NULL)
            printf("No Items to Walk.\n");
          else
            {
              while (is)
                {
                  printf("     Tag=%s, Value=%s\n", is->item,
                         ExpandStrings(is->value, cur_inst->head_sect));
                  is = is->next_item;
				}	
			}
          ts = ts->next_section;
		}
	}
  /*
	DeleteINILinks(fn);
  */
  return(0);
}

		
/*****************************************************************************/

int  DeleteINILinks( const char  *fn )

{
  struct _INI_Section   *ts;
  struct _INI_Section   *ts1;
  struct _INI_Item      *is;
  struct _INI_Item      *is1;
  struct _INI_Instance  *cur_inst;

  cur_inst = FindINIInstance(fn);

  if (!cur_inst)
    return(0);
	
  if (cur_inst->head_sect != NULL)
    {
      ts = cur_inst->head_sect;
      while (ts != NULL)
        {
          ts1 = ts;
          is = ts->first_item;
          if (is != NULL)
            {
              while (is)
                {
                  is1 = is;
                  is = is1->next_item;
                  free(is1);
				}	
			}
          ts = ts1->next_section;
          free(ts1);
		}
      cur_inst->head_sect = NULL;
	}
  return(0);
}


/*****************************************************************************/

static INI_Instance  *FindINIInstance( const char *fn )

{
  struct _INI_Instance  *t;

  t = head_inst;
  if (head_inst != NULL)
    {
      while (t)
        {
          if (!strcmp(fn, t->fn)) 
            break;
          else  
            t = t->next_inst;
		}
	}
  return(t);
}


/*****************************************************************************/

static INI_Instance  *AddINIInstance( const char *fn )

{
  struct _INI_Instance  *t;

  t = NULL;
  if (head_inst == NULL)
    {
      head_inst = (struct _INI_Instance *)malloc(sizeof(struct _INI_Instance));
      head_inst->head_sect = NULL;
      head_inst->next_inst = NULL;
      strcpy(head_inst->fn, fn);
      t=head_inst;
      t->strings_sect_flag = 0;
	}
  else
    {
      t = head_inst;
      while (t->next_inst) 
        t = t->next_inst;

      t->next_inst = (struct _INI_Instance *)malloc(sizeof(struct _INI_Instance));
      t->next_inst->head_sect = NULL;
      t->next_inst->next_inst = NULL;
      strcpy(t->next_inst->fn, fn);
      t = t->next_inst;
      t->strings_sect_flag = 0;
	}
  return(t);
}


/*****************************************************************************/

static INI_Section  *FindINISection( const char           *lpszSection,
                                     struct _INI_Section  *head_sect )

{
  struct _INI_Section  *ts;
  struct _INI_Section  *ts1;

  ts = NULL;
  ts1 = head_sect;

  while (ts1 != NULL)
    {
      if (!strcmp(lpszSection, ts1->section))
        {
          ts = ts1;
          break;
		}
      ts1 = ts1->next_section;
	}
		
  return(ts);
}


/*****************************************************************************/

static struct _INI_Item  *FindINIItem( struct _INI_Section  *lpsection,
                                       const char           *lpszItem )

{
  struct _INI_Item  *is;
  struct _INI_Item  *is1;

  is = NULL;
	
  is1 = lpsection->first_item;
  while(is1)
    {
      if (!strcmp(lpszItem, is1->item))
        {
          is = is1;
          break;
		}
      is1 = is1->next_item;
	}

  return(is);
}


/*****************************************************************************/

static int  DelINIItem( struct _INI_Section  *lpsection,
                        const char           *lpszItem )

{
  struct _INI_Item  *is;
  struct _INI_Item  *is1;

  is = NULL;
	
  is1 = lpsection->first_item;
  while(is1)
    {
      if (!strcmp(lpszItem, is1->item))
        {
          if (is1 == lpsection->first_item)
            {
              lpsection->first_item = is1->next_item;
              free(is1);
              return(0);
			}
          else
            {
              is->next_item = is1->next_item;
              free(is1);
              return(0);
			}
		}
      is = is1;
      is1 = is1->next_item;
	}

  return(0);
}


/*****************************************************************************/

static int  GetINISection( const char           *lpszSection,
                           char                 *lpszReturnBuffer,
                           int                   cchBuffer,
                           struct _INI_Section  *head_sect )

{
  struct _INI_Section  *ts;
  struct _INI_Item     *is;
  char    tbuf[MAX_INI_ENTRY];
  char   *lpszPtr;
  int     tcnt;


  memset(lpszReturnBuffer, '\0', cchBuffer);
  memset(tbuf, '\0', MAX_INI_ENTRY);
  tcnt = 0;

  ts = FindINISection(lpszSection, head_sect);
	
  if (ts == NULL)
    return (-1);

  is = ts->first_item;
  lpszPtr = lpszReturnBuffer;
  while(is)
    {
      sprintf(tbuf, "%s=%s", is->item,ExpandStrings(is->value, head_sect));
      if ((tcnt + strlen(tbuf) + 1) >= (size_t)cchBuffer)
        break;
      strcat(lpszPtr, tbuf);
      lpszPtr += strlen(tbuf) + 1;
      tcnt += strlen(tbuf) + 1;
      is = is->next_item;
	}
	
  return(tcnt);
}


/*****************************************************************************/

static int  GetINISectionNames( char                 *lpszReturnBuffer,
                                int                   cchBuffer,
                                struct _INI_Section  *head_sect)

{
  struct _INI_Section  *ts;
  char  *lpszPtr;
  int    tcnt;

  memset(lpszReturnBuffer, '\0', cchBuffer);
  tcnt = 0;

  ts = head_sect;
  
  if (ts == NULL)
    return (-1);

  lpszPtr = lpszReturnBuffer;
  while(ts)
    {
      if ((tcnt + strlen(ts->section) + 1) >= (size_t)cchBuffer)
        break;
      if (strcmp(ts->section,STRINGS_LABEL))
        {
          strcat(lpszPtr, ts->section);
          lpszPtr += strlen(ts->section) + 1;
          tcnt += strlen(ts->section) + 1;
        }
      ts = ts->next_section;
	}
	
  return(tcnt);
}


/*****************************************************************************/

int  GetProfileSection( const char  *lpszSection,
                        char        *lpszReturnBuffer,
                        int          cchBuffer,
                        const char  *fn ) 

{
  int  ret_code = 0;
  struct _INI_Instance  *cur_inst;

	if (LoadINIFile(fn) < 0) 
		return(-1);

	if (lpszReturnBuffer == NULL)
		return (-2);

	if (strlen(lpszSection) == 0 || strlen(lpszSection) > MAX_INI_ENTRY)
		return(-3);

	cur_inst=FindINIInstance(fn);
	if (!cur_inst) {
		printf("Unable to find INI Instance for %s\n",fn);
		return(-4);
	}

	ret_code=GetINISection(lpszSection,lpszReturnBuffer,cchBuffer,
				cur_inst->head_sect);
	return(ret_code);

}


/*****************************************************************************/

int  GetProfileSectionNames( char       *lpszReturnBuffer,
                             int         cchBuffer,
                             const char *fn )

{
  int   ret_code;
  struct _INI_Instance  *cur_inst;

  if (cchBuffer < 0)
    return(-1);

  if (LoadINIFile(fn) < 0) 
    return(-1);
  cur_inst = FindINIInstance(fn);
  if (!cur_inst)
    {
      printf("Unable to find INI Instance for %s\n", fn);
      return(-1);
	}
  ret_code = GetINISectionNames(lpszReturnBuffer,cchBuffer,
                                cur_inst->head_sect);
  return(ret_code);
}


/*****************************************************************************/

int  GetSectionValues( const char  *lpszSection,
                       char        *lpszReturnBuffer,
                       int          cchBuffer,
                       const char  *fn)
{
  char  *ptr;
  char  *tptr;
  int    tcnt;
  char   tag[MAX_INI_ENTRY];
  char   value[MAX_INI_ENTRY];
  struct _INI_Section   *ts;
  struct _INI_Instance  *cur_inst;

  if (cchBuffer < 0)
    return(-1);

  if (LoadINIFile(fn) < 0) 
    return(-1);

  cur_inst = FindINIInstance(fn);
  if (!cur_inst)
    {
      printf("Unable to find INI Instance for %s\n", fn);
      return(-1);
	}

  memset(lpszReturnBuffer, '\0', cchBuffer);

  ts = FindINISection(lpszSection, cur_inst->head_sect);
	
  if (ts == NULL)
    return(-1);

  ptr = (char *)malloc(cchBuffer);
  memset(ptr, '\0', cchBuffer);
  tcnt = 0;
  
  tptr = ptr;
  GetINISection(lpszSection, tptr, cchBuffer, cur_inst->head_sect);

  while(strlen(tptr))
    {
      memset(tag, '\0', MAX_INI_ENTRY);
      memset(value, '\0', MAX_INI_ENTRY);
      GetItemValues(tptr, tag, value);
      strcpy(&lpszReturnBuffer[tcnt],
             ExpandStrings(value, cur_inst->head_sect));
      tcnt += strlen(value) + 1;
      tptr += strlen(tptr) + 1;
	}
  free(ptr);

  return(tcnt);
}


/*****************************************************************************/

int  GetProfileInt( const char  *lpszSection,
                    const char  *lpszKey,
                    int          dwDefault,
                    const char  *fn ) 
{
  struct _INI_Item      *is;
  struct _INI_Section   *ts;
  struct _INI_Instance  *cur_inst;
  int  i;

	

  cur_inst = FindINIInstance(fn);
  if (!cur_inst)
    {
      printf("Unable to find INI Instance for %s\n", fn);
      return(-1);
	}
  ts = FindINISection(lpszSection, cur_inst->head_sect);
  
  if (ts == NULL)
    return(dwDefault);

  is = FindINIItem(ts, lpszKey);

  if (is == NULL)
    return(dwDefault);

  for (i = 0; (size_t)i < strlen(is->value); i++)
    {
      if (!isdigit((int)is->value[i]))
        return(dwDefault);
	}
  
  return(atoi(is->value));
	
}


/*****************************************************************************/

int  GetProfileString( const char  *lpszSection,
                       const char  *lpszKey,
                       const char  *lpszDefault,
                       char        *lpszReturnBuffer,
                       int          cchReturnBuffer,
                       const char  *fn )

{
  struct _INI_Item      *is;
  struct _INI_Section   *ts;
  struct _INI_Instance  *cur_inst;

  if (LoadINIFile(fn) < 0) 
    return(-1);

  cur_inst=FindINIInstance(fn);
  if (!cur_inst)
    {
      printf("Unable to find INI Instance for %s\n",fn);
      return(-1);
	}

  ts = FindINISection(lpszSection,cur_inst->head_sect);
	
  if (ts == NULL)
    {
      strcpy(lpszReturnBuffer, lpszDefault);
      return(strlen(lpszDefault));
	}

  is = FindINIItem(ts, lpszKey);

  if (is == NULL)
    {
      strcpy(lpszReturnBuffer, lpszDefault);
      return(strlen(lpszDefault));
	}

  strncpy(lpszReturnBuffer, ExpandStrings(is->value, cur_inst->head_sect),
          cchReturnBuffer);
  return(strlen(is->value));

}


/*****************************************************************************/

int  WriteProfileSection( const char *lpszSection,
                          const char *lpszKeyAndValues,
                          const char *fn )

{
  struct _INI_Item      *is;
  struct _INI_Item      *is1;
  struct _INI_Section   *ts;
  struct _INI_Instance  *cur_inst;

  char         tbuf[MAX_INI_ENTRY];
  const char  *lpszKeys;
	

  if (LoadINIFile(fn) < 0) 
    return(-1);

  cur_inst = FindINIInstance(fn);
  if (!cur_inst)
    {
      printf("Unable to find INI Instance for %s\n", fn);
      return(-1);
	}

  ts = FindINISection(lpszSection, cur_inst->head_sect);

  if (ts == NULL)
    {
      BuildSectionLabel(tbuf, lpszSection);
      /* OLD CODE sprintf(tbuf,"[%s]",lpszSection);*/
     
      AddINISectionLink(tbuf, cur_inst);
      ts = FindINISection(lpszSection, cur_inst->head_sect);
	}
  else
    {
      if (ts->first_item)
        {
          is = ts->first_item;
          while (is)
            {
              is1 = is->next_item;
              free(is);
              is = is1;
			}
		}
	}
  ts->first_item = NULL;

  is = NULL;
  is1 = NULL;
  lpszKeys = lpszKeyAndValues;
	
  while (1)
    {
      memset(tbuf, '\0', MAX_INI_ENTRY);
      strcpy(tbuf, lpszKeys);
      
      if (strlen(tbuf) == 0)
        break;
      else
        {
          AddINIItemLink(lpszSection, tbuf, cur_inst);
		}
      lpszKeys = lpszKeys + strlen(tbuf) + 1;
	}
  return(0);	
}


/*****************************************************************************/

int  WriteProfileString( const char  *lpszSection,
                         const char  *lpszKey,
                         const char  *lpszString,
                         const char  *fn )

{
  char  tmp_buff[MAX_INI_ENTRY];
  struct _INI_Item     *is;
  struct _INI_Section  *ts;
  struct _INI_Instance *cur_inst;
	
  if (LoadINIFile(fn) < 0) 
    return(-1);

  cur_inst = FindINIInstance(fn);
  if (!cur_inst)
    {
      printf("Unable to find INI Instance for %s\n", fn);
      return(-2);
	}

  ts = FindINISection(lpszSection, cur_inst->head_sect);

  if (ts == NULL)                             /* Section does not pre-exist. */
    {
      BuildSectionLabel(tmp_buff, lpszSection);
      /* OLD CODE   sprintf(tmp_buff, "[%s]", lpszSection);*/

      AddINISectionLink(tmp_buff, cur_inst);
      ts = FindINISection(lpszSection, cur_inst->head_sect);
	}
  is = FindINIItem(ts, lpszKey);

  BuildProfileString(tmp_buff, lpszKey, lpszString);
  /*  sprintf(tmp_buff, "%s=%s", lpszKey, lpszString);*/

  if (is)
    {
      if(GetItemValues(tmp_buff, is->item, is->value))
        return(0);
      
	}
  else
    {
      AddINIItemLink(lpszSection, tmp_buff, cur_inst);
	}

  return(0);
}


/*****************************************************************************/

int  RemoveProfileString( const char  *lpszSection,
                          const char  *lpszKey,
                          const char  *fn )

{
  struct _INI_Section   *ts;
  struct _INI_Instance  *cur_inst;
  char   tbuf[MAX_INI_ENTRY];
	
  if (LoadINIFile(fn) < 0) 
    return(-1);

  cur_inst = FindINIInstance(fn);
  if (!cur_inst)
    {
      printf("Unable to find INI Instance for %s\n",fn);
      return(-1);
	}
  ts = FindINISection(lpszSection, cur_inst->head_sect);

  if (ts == NULL)
    {
      sprintf(tbuf, "[%s]", lpszSection);
      AddINISectionLink(tbuf, cur_inst);
      ts = FindINISection(lpszSection, cur_inst->head_sect);
	}

  DelINIItem(ts, lpszKey);

  return(0);
}


/*****************************************************************************/
/*  Expand embedded item strings from the strings section  */

char  *ExpandStrings( char                 *item,
                      struct _INI_Section  *head_sect )

{
  struct _INI_Item    *is;
  struct _INI_Section *ts;
  char   tbuf[MAX_INI_ENTRY * 2];
  char   new_buf[MAX_INI_ENTRY * 2];
  char   searchbuf[MAX_INI_ENTRY];
  char  *bptr;
  char  *tptr;
  char  *eptr;
  char  *nptr;
  char  *sptr;
  int    i;
  int    notdone;
  int    startfound;

  ts = FindINISection(STRINGS_LABEL, head_sect);
  if (ts == NULL)
    return(item);
  
  memset(tbuf, '\0', MAX_INI_ENTRY * 2); 
  memset(new_buf, '\0', MAX_INI_ENTRY * 2); 
  strcpy(tbuf, item); 
  notdone = 1;
  tptr = NULL;
  bptr = NULL;
  eptr = NULL;
  nptr = NULL;

  /* have to find all pairs in the string */
  nptr = new_buf;
  tptr = tbuf;
  startfound = 0;
  sptr = 0;

  for (i = 0; (size_t)i <= strlen(item); i++)
    {
      if (item[i] == STRINGS_DELIMITER)
        {
          if (startfound)
            {
              startfound = 0;

              is = FindINIItem(ts, searchbuf);
              /* if there is not strings item, leave it alone */
              if (is == NULL)
                {
                  strcat(nptr, STRINGS_DELIM_STR);
                  strcat(nptr, searchbuf);
                  strcat(nptr, STRINGS_DELIM_STR);          
                  nptr += strlen(searchbuf) + 2;
                }
              else 
                { /* otherwise, replace it */
                  strcat(nptr, is->value);
                  nptr += strlen(is->value);
                }
              continue;
            }
          else
            {
              startfound = 1;
              memset(searchbuf, '\0', MAX_INI_ENTRY);
              sptr = searchbuf;
              continue;
            }
        }
      if (startfound && sptr)
        {
          *sptr=item[i];
          sptr++;
        }
      else
        {
          *nptr = item[i];
          nptr++;
        }
    }

  /* Found a start delim but never an ending, so 
     add back the delim and the text saved in searchbuf */
  if (startfound)
    {
      strcat(new_buf, STRINGS_DELIM_STR);
      strcat(new_buf, searchbuf);
    }
  strcpy(item, new_buf);
  return(item);
}


/*****************************************************************************/
/* Builds the section name within the size limitations. This is done the hard
   way for efficiency.
 */

int  BuildSectionLabel( char        *output_str,
                        const char  *section_name )
{
  int  i, j;

  output_str[0] = '[';
  i = 1;
  j = 0;
  /* Limit of string length is deducted by 2 for the ']' and NULL chars. */

  while( (section_name[j] != '\0') && (i < (MAX_INI_ENTRY - 2)) )
    {
      output_str[i] = section_name[j];
      i++;
      j++;
    }
  output_str[i] = ']';
  output_str[i + 1] = '\0';

  return(0);
}


/*****************************************************************************/
/* Builds a complete tag/value pair line within the normal size constraints. */

int  BuildProfileString( char        *output_str,
                         const char  *key_str,
                         const char  *value_str )
{
  int  i, j;

  i = 0;

  /* Limit of string length is deducted by 2 for the the '=' and NULL chars. */

  /* Insert the key first.. */
  while( (key_str[i] != '\0') && (i < (MAX_INI_ENTRY - 2)) )
    {
      output_str[i] = key_str[i];
      i++;
    }
  output_str[i] = '=';
  i++;

  /* Insert the value next.. */
  j = 0;
  while( (value_str[j] != '\0') && (i < (MAX_INI_ENTRY - 2)) )
    {
      output_str[i] = value_str[j];
      i++;
      j++;
    }

  output_str[i] = '\0';

  return(0);
}


/*****************************************************************************/

int  RemoveProfileSection( char *lpszSection,
			   char *fn)
{
  DelINISection(lpszSection, fn);
  return 0;
}

/*****************************************************************************/

static int DelINISection(lpszSection,fn)
char *lpszSection;
char *fn;
{
struct _INI_Section *ts,*ts1,*tsprev;
struct _INI_Item *is,*is1;
struct _INI_Instance *cur_inst;


        if (LoadINIFile(fn) < 0)
                return(-1);

        cur_inst=FindINIInstance(fn);
        if (!cur_inst) {
                printf("Unable to find INI Instance for %s\n",fn);
                return(-1);
        }

        ts=tsprev=NULL;
        ts1=cur_inst->head_sect;

        while (ts1 != NULL) {
                if (!strcmp(lpszSection,ts1->section)) {
                        ts=ts1;
                        break;
                }
                tsprev=ts1;
                ts1=ts1->next_section;
        }


        /* now process the section */
        if (ts != NULL) {
                /* delete all items */
                is=is1=ts->first_item;
                while (is) {
                        is1=is->next_item;
                        free(is);
                        is=is1;
                }
                ts->first_item=NULL;

                if (ts == cur_inst->head_sect) {
                        cur_inst->head_sect=ts->next_section;
                } else {
                        tsprev->next_section=ts->next_section;
                }
                free(ts);

        }

        return(0);

}
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

