
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * Packdata.c
 */

/*#include    <varargs.h>*/
#include    <stdarg.h>
#include    "ourhdr.h"

/* Package data into a character string */

int pack_data(char* str, char* key, ...)
{
	va_list ap;
	char *k, *char_data;
	char format[20], fmt_str[10];
	int int_data, n, leng = 0;

	str[0] = 0;
	k = key;

	va_start(ap, key);
	while (*k) {
/*
 * Pack character string.
 */
		switch (*k) {
			case 'C':
				char_data = va_arg(ap, char *);
				n = strlen(char_data);
				sprintf(format, "%5.5d", n);
				strcat(str, format);
				strcat(str, char_data);
				leng += n + 5;
				break;
/*
 * Pack integer string.
 */
			case 'I':
				int_data = va_arg(ap, int);
				sprintf(format, "%d", int_data);
				n = strlen(format);
				sprintf(fmt_str, "%%1d%%%d.%dd", n, n);
				sprintf(format, fmt_str, n, int_data);
				strcat(str, format);
				leng += n + 1;
				break;
/*
 * Pack date/time string.
 */
			case 'D':
				char_data = va_arg(ap, char *);
				strcat(str, char_data);
				leng += 11;
				break;

			default:
				return(-1);
				break;
		}
		k++;
	}
	va_end(ap);

	return(leng);
}



/* Unpackage data from a character string */

int unpack_data(char* str, char* key, ...)
{
	va_list ap;
	char *s, *k, *char_data;
	char format[20];
	int *int_data, i, n;

	s = str;
	k = key;

	va_start(ap, key);
	while (*k) {
/*
 * Unpack character string.
 */
		switch (*k) {
			case 'C':
				char_data = va_arg(ap, char *);
				for (i = 0; i < 5; i++) {
					if (*s)
						format[i] = *s++;
					else
						return(-1);
				}
				format[5] = 0;
				n = atoi(format);
				for (i = 0; i < n; i++) {
					if (*s)
						char_data[i] = *s++;
					else
						return(-1);
				}
				char_data[n] = 0;
				break;
/*
 * Unpack integer string.
 */
			case 'I':
				int_data = va_arg(ap, int *);
				if (*s)
					format[0] = *s++;
				else
					return(-1);
				format[1] = 0;
				n = atoi(format);
				for (i = 0; i < n; i++) {
					if (*s)
						format[i] = *s++;
					else
						return(-1);
				}
				format[n] = 0;
				*int_data = atoi(format);
				break;
/*
 * Unpack date/time string.
 */
			case 'D':
				char_data = va_arg(ap, char *);
				for (i = 0; i < 11; i++) {
					if (*s)
						char_data[i] = *s++;
					else
						return(-1);
				}
				char_data[11] = 0;
				break;

			default:
				return(-1);
				break;
		}
		k++;
	}
	va_end(ap);

	return(0);
}
