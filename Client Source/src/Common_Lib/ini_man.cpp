/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NTCSS_ROOT_DIR\source\comn_lib\ini_man.cpp
//
// WSP, last update: 9/16/96
//                                         
// This is the implementation file for the CIniFileMan class.  
//
// See routine comments below and also the header file for details on 
// processing.
//
//----------------------------------------------------------------------

#include "stdafx.h"
#include "ini_man.h"

#include "genutils.h"

									   // If suspect that CIniFileMan
									   //  is causing problems then
									   //  compile with TAKEOUT_INI_FILE_MAN
									   //  defined and then try
//#define TAKEOUT_INI_FILE_MAN


/////////////////////////////////////////////////////////////////////////////
// CIniFileMan constructor
// _________________________
//
// Initializes all members, if ini_file_name is not null then allocates
// memory and saves it
//

CIniFileMan::CIniFileMan (char *ini_file_name)

#ifdef TAKEOUT_INI_FILE_MAN

{}

#else
{
	m_ok = FALSE;
	
	m_currect_section_name = NULL;
    m_ini_file_name = NULL;
	m_current_section_entry_list = NULL;
    m_section_changed = FALSE;
    
	if ( (ini_file_name == NULL) ||
		 (*ini_file_name == '\0') )
	{
		m_ini_file_name = NULL;
	}
	else
	{
		m_ini_file_name = new char[strlen(ini_file_name)+1];
		if (m_ini_file_name == NULL)
		{
			BailOut();
			return;
		}
	}

	m_ok = TRUE;
} 
#endif // TAKEOUT_INI_FILE_MAN   


/////////////////////////////////////////////////////////////////////////////
// CerrorManager destructor
// ________________________
//
// Must flush first in case any changes are outstanding, the does
// BailOut to be done.
//

CIniFileMan::~CIniFileMan()

#ifdef TAKEOUT_INI_FILE_MAN

{} // Do nothing

#else
{
	Flush();

	BailOut();
}
#endif // TAKEOUT_INI_FILE_MAN   



/////////////////////////////////////////////////////////////////////////////
// BailOut
// _______
//
// Deletes all new'd memory and sets CIniFileMan to 'invalid' state.  
// This routine used for both normal and abnormal terminations.

void CIniFileMan::BailOut()
#ifdef TAKEOUT_INI_FILE_MAN

{} // Do nothing

#else
{
	if (!m_ok)
		return;

	if (m_ini_file_name != NULL)
		delete [] m_ini_file_name;

	if (m_currect_section_name != NULL)
		delete [] m_currect_section_name;

	SIniEntryRec *entry_ptr = m_current_section_entry_list;
	SIniEntryRec *next_ptr;

	while (entry_ptr != NULL)
	{
		next_ptr = entry_ptr->next;
		delete [] entry_ptr;
		entry_ptr = next_ptr;
	}

	m_ok = FALSE;
}
#endif // TAKEOUT_INI_FILE_MAN   



/////////////////////////////////////////////////////////////////////////////
// Flush
// _____
//
// Copies all ini file data from m_ini_file_name file to a temporary file.
// Settings for current section (m_currect_section_name), however, are
// taking from m_current_section_entry_list, not from m_ini_file_name
// file.  Temporary file is then renamed to become the new m_ini_file_name
// file.
//

BOOL CIniFileMan::Flush()
#ifdef TAKEOUT_INI_FILE_MAN

{return(TRUE);} 

#else
{
									   // If !m_ok then indicate as error
									   //
	if (!m_ok)
		return(FALSE);
									   // If no changes or no entries
									   //  then is ok but nothing todo
									   //
	if ((!m_section_changed) || (m_current_section_entry_list == NULL))
		return(TRUE);

									   // Get name and create temp file
	char temp_file_name[L_tmpnam];

	if (tmpnam(temp_file_name) == NULL)
		return(FALSE);

	FILE *out_file_ptr;
	if ((out_file_ptr = fopen(temp_file_name, "w")) == NULL)
	{
									   // If can't open file then
									   //  indicate error (still m_ok)
		return(FALSE);
	}
									   // Open ini file for reading
	FILE *in_file_ptr;
	BOOL file_exists = TRUE;
	if ((in_file_ptr = fopen(m_ini_file_name,"r")) == NULL)
	{ 
	                                   // Still okay if file didn't exist,
	                                   //  will make new one
		file_exists = FALSE;
	}

	char tmp_buf[MAX_INI_FILE_LINE_LEN];
	BOOL found = FALSE;
	int cnt;
									   // Loop until section found
									   //  or hit EOF, write each line
									   //  read to the tmp file
	if (file_exists)
	{
		while ((!found) && 
			   ((fgets(tmp_buf,MAX_INI_FILE_LINE_LEN,in_file_ptr) != 0)))
		{
			cnt = strlen(tmp_buf);
									   // If this line is a section
									   //  start see if it is one we're
									   //  after
									   //
			if ((tmp_buf[0]	== SECTION_STING_START_CH) &&
				(tmp_buf[cnt-2]	== SECTION_STING_STOP_CH))
			{
									   // If this is section line then
									   //  see if its the right one
				tmp_buf[cnt-2] = '\0';

				if (strcmp(m_currect_section_name,&(tmp_buf[1])) == 0)
					found = TRUE;
				else
					tmp_buf[cnt-2] = SECTION_STING_STOP_CH;
			}
									   // If didn't find then write line
			if (!found)
				fputs(tmp_buf, out_file_ptr);
		}
	}
									   // Now write the section name
									   //  and the SIniEntryRec's
									   //  to the tmp file
									   // 
	fprintf(out_file_ptr,"[%s]\n",m_currect_section_name);

	SIniEntryRec *entry_ptr = m_current_section_entry_list;

	while (entry_ptr)
	{
		fprintf(out_file_ptr,"%s=%s\n",
				entry_ptr->key, entry_ptr->value);
		
		entry_ptr = entry_ptr->next;		
	}								   
	fprintf(out_file_ptr,"\n");

									   // Now skip over the section
									   //  in the int file
	if (file_exists)
	{
		if (found)
		{
			BOOL done = FALSE;
			while ((!done) && 
		    	   ((cnt=fgets(tmp_buf,MAX_INI_FILE_LINE_LEN,in_file_ptr) 
			    	!= 0)))
									   // If this line is a new section
									   //  then need to output it and
									   //  get out of this skip loop,
									   //  otherwise just keep reading
									   //  and trashing old section
									   //  lines
									   //
			if ((tmp_buf[0]	== SECTION_STING_START_CH) &&
				(tmp_buf[cnt-2]	== SECTION_STING_STOP_CH))
			{
				fputs(tmp_buf, out_file_ptr);
				done = TRUE;
			}
		}
									   // If found now just copy rest of the  
									   //  file lines to the temp file.
									   //
		while (fgets(tmp_buf,MAX_INI_FILE_LINE_LEN,in_file_ptr) != NULL)
			    
			fputs(tmp_buf, out_file_ptr);
	}
									   // Now close the files and
									   //  remove the old ini file
									   //  and rename the tmp file
									   //  to the ini file name
	if (file_exists)
		fclose(in_file_ptr);
		
	fclose(out_file_ptr);

	if (file_exists)
		if (remove(m_ini_file_name) != 0)
			return(FALSE);

	if (rename(temp_file_name, m_ini_file_name) != 0)
		return(FALSE);

									   // Set flag as file matches the
									   //  SIniEntryRec's now
	m_section_changed = FALSE;

	return(TRUE);
}
#endif // TAKEOUT_INI_FILE_MAN   



/////////////////////////////////////////////////////////////////////////////
// GetEntry
// ________
//
// Returns ptr to SIniEntryRec containing data matching the specified
// section and key, unless not found then NULL is returned.
//

SIniEntryRec *CIniFileMan::GetEntry(char *section, char *key)
#ifdef TAKEOUT_INI_FILE_MAN

{return(NULL);} // Do nothing

#else
{
	if (!m_ok)
		return(NULL);				   
									   // First make specified section
									   //  current
	GetSection(section);
									   // Since GetSection may set
									   //  m_ok to FALSE then check
									   //  again
	if (!m_ok)
		return(NULL);

	if (m_current_section_entry_list == NULL)
		return(NULL);

	SIniEntryRec *entry_ptr = m_current_section_entry_list;
	SIniEntryRec *next_ptr;

									   // Now look for entry
	while (entry_ptr != NULL)
	{
		if (strcmp(key, entry_ptr->key) == 0)
			return(entry_ptr);

		next_ptr = entry_ptr->next;
		entry_ptr = next_ptr;
	}
	return(NULL);
}
#endif // TAKEOUT_INI_FILE_MAN   



/////////////////////////////////////////////////////////////////////////////
// GetSection
// __________
//
// Reads into m_current_section_entry_list all entries for the given
// section.  If errors occur then m_ok is set to FALSE.  If just unable
// to find the given section in the m_ini_file_name file then m_ok remains
// TRUE but m_current_section_entry_list will be NULL.
//

void CIniFileMan::GetSection(char *section)

#ifdef TAKEOUT_INI_FILE_MAN

{} // Do nothing

#else
{
	if (!m_ok)
		return;
									   // If section is already current
									   //  then nothing to do
									   //
	if ( (m_currect_section_name != NULL) &&
		 (strcmp(m_currect_section_name, section) == 0) )
	{
		 return;
	}								   
									   // If have outstanding changes
									   //  then have to Flush
	if (m_section_changed)
	{
		Flush();
	}
									   // Now can trash the current 
									   //  section
	TrashCurrentSectionEntries();
									   // Open ini file
	FILE *file_ptr;
	if ((file_ptr = fopen(m_ini_file_name, "r")) == NULL)
	{
									   // If can't open file still m_ok, NULL
									   //  m_current_section_entry_list
									   //  signifies problem
		return;
	}

	char tmp_buf[MAX_INI_FILE_LINE_LEN];
	BOOL found = FALSE;
	int cnt;
									   // Loop until section found
									   //  or hit EOF
	while ((!found) && 
		   ((fgets(tmp_buf,MAX_INI_FILE_LINE_LEN,file_ptr) != 0)))
	{
		trim(tmp_buf);
									   // If this line isn't a section
									   //  start then go to next line
									   //
		cnt = strlen(tmp_buf);

		if ((tmp_buf[0]	!= SECTION_STING_START_CH) &&
			(tmp_buf[cnt-1]	!= SECTION_STING_STOP_CH))
		{
			continue;
		}							   
									   // If this is section line then
									   //  see if its the right one
		tmp_buf[cnt-1] = '\0';

		if (strcmp(section,&(tmp_buf[1])) == 0)
			found = TRUE;

	}
									   // If didn't find still m_ok
									   //  and NULL	m_currect_section_name
									   //  signifies problem
	if (!found)
	{
		fclose(file_ptr);
		return;
	}
                                       // Have already gotten rid of old
                                       //  m_currect_section_name, now 
                                       //  new space for this section and
                                       //  copy to m_currect_section_name
                                       //
	m_currect_section_name = new char[(strlen(section)+1)];
	if (m_currect_section_name == NULL)
	{
		BailOut();
		return;
	}
	strcpy(m_currect_section_name,section);
	
	BOOL done = FALSE;
	char *split_ptr;
	int key_len,val_len;
	SIniEntryRec *entry_ptr;
	SIniEntryRec *prev_entry_ptr = NULL;

									   // Have found section name, must
									   //  read keys and values	now
	while ((!done) && 
		   ((cnt=fgets(tmp_buf,MAX_INI_FILE_LINE_LEN,file_ptr) != 0)))
	{
									   // Trash extra white space
		trim(tmp_buf);

									   // If this line is start of a
									   //  new section or blank then stop
		if ((tmp_buf[0] == '\0') ||	   
			(tmp_buf[0] == SECTION_STING_START_CH))
		{
			done = TRUE;
			continue;
		}
									   // Find separator between key
									   //  and value, if not there then
									   //  line will be lost!
									   //
		split_ptr = strchr(tmp_buf, KEY_VAL_SPLIT_CH);
		if (split_ptr == NULL)
		{
			continue;
		}							   
									   // Nullify the key and trim it
		*split_ptr = '\0';
		trim(tmp_buf);
									   // Now find len of key
									   //
		key_len = split_ptr - tmp_buf + 1;

									   // Now nullify the value and trim it
		split_ptr++;
		trim(tmp_buf);
									   // Now find len of value
		val_len = strlen(split_ptr) + 1;

									   // Get space for entry data
		entry_ptr = (SIniEntryRec *)
					 new char[sizeof(SIniEntryRec) + key_len + val_len];

									   // If problems then bail
		if (entry_ptr == NULL)
		{
			BailOut();
			fclose(file_ptr);
			return;
		}
									   // Now put key text just after
									   //  the SIniEntryRec 
									   //
		entry_ptr->key = (char *)(entry_ptr+1);
		strcpy(entry_ptr->key,tmp_buf);

									   // The value text goes just after
									   //  the key text's NULL char 
									   //
		entry_ptr->value = entry_ptr->key + key_len;
		strcpy(entry_ptr->value,split_ptr);

									   // Now set up pointers, either
									   //  set list head or fix up 
									   //  prev	SIniEntryRec's next
									   //
		if (m_current_section_entry_list == NULL)
		{
			m_current_section_entry_list = entry_ptr;
		}
		else
		{
			prev_entry_ptr->next = entry_ptr;
		}
		prev_entry_ptr = entry_ptr;
		entry_ptr->next = NULL;

	}
									   // Done, close file
	fclose(file_ptr);
}
#endif // TAKEOUT_INI_FILE_MAN   




/////////////////////////////////////////////////////////////////////////////
// GetBool
// _______

BOOL CIniFileMan::GetBool(char *section, char *key, BOOL defval)
#ifdef TAKEOUT_INI_FILE_MAN

{return(defval);} // Just return defval

#else
{
	SIniEntryRec *entry_ptr = GetEntry(section, key);

	if (entry_ptr != NULL)
	{
		if (strcmp(key,"TRUE") == 0)
			return(TRUE);

		else if (strcmp(key,"FALSE") == 0)
			return(FALSE);
	}

	return(defval);
}

#endif // TAKEOUT_INI_FILE_MAN   



/////////////////////////////////////////////////////////////////////////////
// GetInt
// _______

int CIniFileMan::GetInt (char *section, char *key, int defval)
#ifdef TAKEOUT_INI_FILE_MAN

{return(defval);} // Just return defval

#else
{
	SIniEntryRec *entry_ptr = GetEntry(section, key);

	if (entry_ptr != NULL)
	{
		int int_val = atoi(entry_ptr->value);
		return(int_val);
	}
	else
		return(defval);
}
#endif // TAKEOUT_INI_FILE_MAN   



/////////////////////////////////////////////////////////////////////////////
// GetString
// _________

const char *CIniFileMan::GetString
							(char *section, char *key, char *defval)
#ifdef TAKEOUT_INI_FILE_MAN

{return(defval);} // Just return defval

#else
{
	SIniEntryRec *entry_ptr = GetEntry(section, key);

	if (entry_ptr != NULL)

		return(entry_ptr->value);

	else
		return(defval);
}
#endif // TAKEOUT_INI_FILE_MAN   




/////////////////////////////////////////////////////////////////////////////
// PutEntry
// ________
//
// Overwrites or adds a new SIniEntryRec to the m_current_section_entry_list
// for the new data.  Note that PutEntry does not write to the ini file -
// a Flush must be called to handle this.
// 

BOOL CIniFileMan::PutEntry(char *section, char *key, char *value)

#ifdef TAKEOUT_INI_FILE_MAN

{return(TRUE);} // Do nothing

#else
{
	if (!m_ok)
		return(FALSE);

	trim(key);
	trim(value);

									   // Now find len of key and value
									   //
	int key_len = strlen(key) + 1;

	int val_len = strlen(value) + 1;

									   // Get space for entry data
	SIniEntryRec *new_entry_ptr = 
				(SIniEntryRec *)
				new char[sizeof(SIniEntryRec) + key_len + val_len];

									   // If problems then bail
	if (new_entry_ptr == NULL)
	{
		BailOut();
		return(FALSE);
	}

									   // Now put key text just after
									   //  the SIniEntryRec 
									   //
	new_entry_ptr->key = (char *)(new_entry_ptr+1);
	strcpy(new_entry_ptr->key,key);

									   // The value text goes just after
									   //  the key text's NULL char 
									   //
	new_entry_ptr->value = new_entry_ptr->key + key_len;
	strcpy(new_entry_ptr->value, value);

									   // Read in the section
	GetSection(section);

									   // Bail if not m_ok at this point
	if (!m_ok)
	{
		BailOut();
		return(FALSE);
	}
									   // Set changed flag as will make
									   //  updates to section for sure
	m_section_changed = TRUE;
									   // If no section entries then is
									   //  easy, make this entry the
									   //  first in the list
									   //
	if (m_current_section_entry_list == NULL)
	{
		m_currect_section_name = new char[(strlen(section)+1)];
		if (m_currect_section_name == NULL)
		{
			BailOut();
			return(FALSE);
		}
		strcpy(m_currect_section_name, section);
		
		m_current_section_entry_list = new_entry_ptr;	
		new_entry_ptr->next = NULL;
		return(TRUE);
	}
										// Otherwise have to look for
										//  where to put this entry
										//
	SIniEntryRec *entry_ptr = m_current_section_entry_list;
	SIniEntryRec *prev_ptr = NULL;
	SIniEntryRec *next_ptr;
	BOOL found = FALSE;

	while ((!found) && (entry_ptr != NULL))
	{
		if (strcmp(key, entry_ptr->key) == 0)
		{
			found = TRUE;
			continue;
		}

		prev_ptr = entry_ptr;
		next_ptr = entry_ptr->next;
		entry_ptr = next_ptr;
	}
									   // If didn't find then will put
									   //  at the end of the list
	if (!found)
	{
		prev_ptr->next = new_entry_ptr;
		new_entry_ptr->next = NULL;
	}
									   // Else found
	else 
	{
									   // Here put at start of list
		if (prev_ptr == NULL)
		{
			m_current_section_entry_list = new_entry_ptr;
			new_entry_ptr->next = entry_ptr->next;
		}
									   // Or else put at middle/end
		else
		{
			prev_ptr->next = new_entry_ptr;
			new_entry_ptr->next = entry_ptr->next;
		}
									   // Old record no longer any good,
									   //  delete it
		delete []entry_ptr;

	}

									   // Done now
	return(TRUE);
}
#endif // TAKEOUT_INI_FILE_MAN   




/////////////////////////////////////////////////////////////////////////////
// PutBool
// _______

BOOL  CIniFileMan::PutBool(char *section, char *entry, BOOL value)
#ifdef TAKEOUT_INI_FILE_MAN

{return(TRUE);} // Do nothing

#else
{		   
	return(PutEntry(section, entry,
				    (value ? "TRUE" : "FALSE")));
}
#endif // TAKEOUT_INI_FILE_MAN   



/////////////////////////////////////////////////////////////////////////////
// PutInt
// ______

BOOL  CIniFileMan::PutInt(char *section, char *key, int value)
#ifdef TAKEOUT_INI_FILE_MAN

{return(TRUE);} // Do nothing

#else
{
	char tmp_buf[80];

	sprintf(tmp_buf,"%d",value);

	return(PutEntry(section,key,tmp_buf));
}
#endif // TAKEOUT_INI_FILE_MAN   



/////////////////////////////////////////////////////////////////////////////
// PutString (two versions)
// _________

BOOL  CIniFileMan::PutString(char *section, char *key, CString value)
#ifdef TAKEOUT_INI_FILE_MAN

{return(TRUE);} // Do nothing

#else
{		   
	return(PutEntry(section,key,value.GetBuffer(value.GetLength())));
}
#endif // TAKEOUT_INI_FILE_MAN   


BOOL  CIniFileMan::PutString(char *section, char *key, char *value)
#ifdef TAKEOUT_INI_FILE_MAN

{return(TRUE);} // Do nothing

#else
{
	return(PutEntry(section,key,value));
}
#endif // TAKEOUT_INI_FILE_MAN   



/////////////////////////////////////////////////////////////////////////////
// SetIniFile
// __________
//
// Changes the current name of the init file (m_ini_file_name) that is
// being worked with.
//

void CIniFileMan::SetIniFile(char *file_name)
#ifdef TAKEOUT_INI_FILE_MAN

{} // Do nothing

#else
{
	if (!m_ok)
		return;

	Flush();
	TrashCurrentSectionEntries();

	if (m_ini_file_name != NULL)
		delete [] m_ini_file_name;

	m_ini_file_name = new char[strlen(file_name)+1];
	if (m_ini_file_name == NULL)
	{
		BailOut();
		return;
	}

	strcpy(m_ini_file_name, file_name);
}
#endif // TAKEOUT_INI_FILE_MAN   



/////////////////////////////////////////////////////////////////////////////
// TrashCurrentSectionEntries
// __________________________
//
// Does deletes on list of SIniEntryRec's and also m_currect_section_name.
//

void CIniFileMan::TrashCurrentSectionEntries()
#ifdef TAKEOUT_INI_FILE_MAN

{} // Do nothing

#else
{
	SIniEntryRec *entry_ptr = m_current_section_entry_list;
	SIniEntryRec *next_ptr;

	while (entry_ptr != NULL)
	{
		next_ptr = entry_ptr->next;
		delete [] entry_ptr;
		entry_ptr = next_ptr;
	}
	m_current_section_entry_list = NULL;

	if (m_currect_section_name != NULL)
	{
		delete [] m_currect_section_name;
	}
	m_currect_section_name = NULL;
}
#endif // TAKEOUT_INI_FILE_MAN   


