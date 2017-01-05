/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

#ifndef _LibFcts_h
#define _LibFcts_h

int getHostType(LPCTSTR);
int net_copy(LPCTSTR, LPCTSTR, LPCTSTR, LPCTSTR, int);
int getNtcssSysConfValueFromNis(LPTSTR*, LPCTSTR);
int getNtcssAuthServerByLoginNameFromNis(LPCTSTR, LPTSTR);
int stripWord(int, LPCTSTR, LPTSTR);
int GetProfileString(LPTSTR, LPTSTR, LPTSTR, LPTSTR, int, LPTSTR);
int LIB_get_master_server(LPTSTR);
extern "C" int unbundleData(char *str, char *key,...);
int getOverallLength( const char  *key, ... );
int plainEncryptString(const char  *key, const char  *data, char **outStringPtr, int use_compression,
                       int use_scrambling);


#endif /* _LibFcts_h */