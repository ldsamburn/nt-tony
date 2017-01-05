/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NTCSS_ROOT_DIR\source\include\ini_man.h                    
//
// WSP, last update: 9/16/96
//                                        
// This is the header file for the CIniFileMan class.  This class provides
// facilities similar to the Windows 'Private Profile APIs'.  The class
// was developed because of memory errors/GPFs that I was experiencing
// when making heavy use of the	'Private Profile APIs' in the TestMan
// application. WSP
//
// See implementation file for details.
//----------------------------------------------------------------------

#ifndef _INCLUDE_INI_MAN_H_
#define _INCLUDE_INI_MAN_H_

									   // INI file lines longer than
									   //  this will be truncating
									   //
const int MAX_INI_FILE_LINE_LEN    = 256;

const char SECTION_STING_START_CH  = '[';
const char SECTION_STING_STOP_CH   = ']';
const char KEY_VAL_SPLIT_CH		   = '=';


typedef struct SIniEntryRec
{
    char		 *key;
    char		 *value;
	SIniEntryRec *next;
} SIniEntryRec;


class CIniFileMan
{
private:
	BOOL m_ok;
	BOOL m_section_changed;
	char *m_currect_section_name;
    char *m_ini_file_name;

	SIniEntryRec *m_current_section_entry_list;

	void BailOut();

	SIniEntryRec *GetEntry(char *section, char *key);

	void GetSection(char *section);

	BOOL PutEntry(char *section, char *key, char *value);

	void PutSection();

	void TrashCurrentSectionEntries();


public:
    CIniFileMan (char *ini_file_name = NULL); 
    
	~CIniFileMan(); 

	BOOL  Flush();

    int   GetInt(char *section, char *key, int defval=0);

    const char *GetString(char *section, char *key, char *defval=NULL);

    BOOL  GetBool(char *section, char *key, BOOL defval=FALSE);

    BOOL  PutBool(char *section, char *key, BOOL val);

    BOOL  PutString(char *section, char *entry, char *val);

    BOOL  PutString(char *section, char *entry, CString val);

    BOOL  PutInt(char *section, char *entry, int val);

    void  SetIniFile(char *file_name);
};

#endif // _INCLUDE_INI_MAN_H_