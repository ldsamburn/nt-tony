/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 
/******************************************************************************
  NtcssMacros.h                                                               
******************************************************************************/
#include  "NtcssError.h"

#define True    1
#define False   0

/*#define ASSERT(condition, error_code, cleanup_code) \
     if (!(condition)) { \
     NtcssError.set_error_code(error_code); \
     cleanup_code }
 #define ERROR_CONDITION              NtcssError.occurred()
 #define IF_ERROR(cleanup_code)       if (ERROR_CONDITION) cleanup_code
 #define RETURN_SUCCESS               return(True)
 #define RETURN_FAILURE               return(False)
 #define RETURN_FAILURE_IF_ERROR      if (ERROR_CONDITION) RETURN_FAILURE
 #define RETURN_IF_ERROR              if (ERROR_CONDITION) return
 #define RETURN_SUCCESS_OR_FAILURE    return(!(ERROR_CONDITION))
 #define RETURN_NEGATIVE1_IF_ERROR    if (ERROR_CONDITION) return(-1)
 #define DEFINE_STRING(STRING_NAME, STRING_SIZE)  char STRING_NAME[(STRING_SIZE) + 1]
 #define CHECK_ASSOCIATION(object)     if (this == NULL) this = &object
 */
