
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * BundleData.c
 */

/** SYSTEM INCLUDES **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*#include <varargs.h>*/
#include <stdarg.h>
#include <sys/file.h>
#include <syslog.h>

/** INRI INCLUDES **/

#include <StringFcts.h>
#include <Ntcss.h>
#include <LibNtcssFcts.h>


/** FUNCTION PROTOTYPES **/

/** FUNCTION DEFINITIONS **/
/*****************************************************************************/

int  bundleData( char        *str,
                 const char  *key,
                 ... )

{
	va_list ap;
	const char *k;
    char *char_data;
	char format[20], fmt_str[10];
	int int_data, n, leng = 0;

	k = key;

	va_start(ap, key);
	while (*k) {
		switch (*k) {
/*
 * Bundle character string.
 */
			case 'C':
				char_data = va_arg(ap, char *);
				n = strlen(char_data);
				sprintf(format, "%5.5d", n);
				strcat(str, format);
				strcat(str, char_data);
				leng += n + 5;
				break;
/*
 * Bundle character string without newlines.
 */
			case 'N':
				char_data = va_arg(ap, char *);
				n = strlen(char_data);
				if (char_data[n - 1] == '\n')
					n--;
				sprintf(format, "%5.5d", n);
				strcat(str, format);
				strncat(str, char_data, n);
				leng += n + 5;
				break;
/*
 * Bundle integer string.
 */
			case 'I':
				int_data = va_arg(ap, int);
				sprintf(format, "%d", int_data);
				n = strlen(format);
				if (int_data < 0)
					sprintf(fmt_str, "%%1d%%%d.%dd", n - 1, n - 1);
				else
					sprintf(fmt_str, "%%1d%%%d.%dd", n, n);
				sprintf(format, fmt_str, n, int_data);
				strcat(str, format);
				leng += n + 1;
				break;
/*
 * Bundle date/time string.
 */
			case 'D':
				char_data = va_arg(ap, char *);
				strcat(str, char_data);
				leng += 11;
				break;
		}
		k++;
	}
	va_end(ap);

	return(leng);
}


/*****************************************************************************/

int  bundleQuery( char        *file,
                  const char  *key,
                  ... )
{
	va_list ap;
	const char *k;
        char *char_data;
	char format[20], fmt_str[10];
	int i, int_data, n, leng = 0;
	FILE *fp;

	if ((fp = fopen(file, "a+")) == NULL) {
		syslog(LOG_WARNING, "Unable to append data to file \"%s\".", file);
		return(0);
	}

	k = key;

	va_start(ap,key);
	while (*k) {
		switch (*k) {
/*
 * Bundle character string.
 */
			case 'C':
				char_data = va_arg(ap, char *);
				n = strlen(char_data);
				fprintf(fp, "%5.5d%s", n, char_data);
				leng += n + 5;
				break;
/*
 * Bundle character string without newlines.
 */
			case 'N':
				char_data = va_arg(ap, char *);
				n = strlen(char_data);
				while (char_data[n - 1] == '\n')
					n--;
				fprintf(fp, "%5.5d", n);
				for (i = 0; i < n; i++)
					putc(char_data[i], fp);
				leng += n + 5;
				break;
/*
 * Bundle integer string.
 */
			case 'I':
				int_data = va_arg(ap, int);
				sprintf(format, "%d", int_data);
				n = strlen(format);
				if (int_data < 0)
					sprintf(fmt_str, "%%1d%%%d.%dd", n - 1, n - 1);
				else
					sprintf(fmt_str, "%%1d%%%d.%dd", n, n);
				fprintf(fp, fmt_str, n, int_data);
				leng += n + 1;
				break;
/*
 * Bundle date/time string.
 */
			case 'D':
				char_data = va_arg(ap, char *);
				for (i = 0; i < 11; i++)
					putc(char_data[i], fp);
				leng += 11;
				break;
		}
		k++;
	}
	va_end(ap);
	fclose(fp);

	return(leng);
}


/*****************************************************************************/

int  unbundleData( char        *str,
                   const char  *key,
                   ... )
{
	va_list ap;
	char *s, *char_data;
        const char *k;
	char format[20];
	int *int_data, i, n;

	s = str;
	k = key;

	va_start(ap, key);
	while (*k) {
		switch (*k) {
/*
 * Unbundle character string.
 */
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
 * Unbundle integer string.
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
 * Unbundle date/time string.
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
		}
		k++;
	}
	va_end(ap);

	return(0);
}


/*****************************************************************************/

int  checkDataLengths( const char  *str,
                       const char  *key,
                       ... )
{
	va_list      ap;
	const char  *s;
    const char  *k;
	char    format[20];
	int    *int_data;
    int     i;
    int     n;

	s = str;
	k = key;

	va_start(ap, key);
	while (*k) {
		switch (*k) {
/*
 * Get length of character string.
 */
			case 'C':
				int_data = va_arg(ap, int *);
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
						s++;
					else
						return(-1);
				}
				*int_data = n;
				break;
/*
 * Skip over integer string.
 */
			case 'I':
				if (*s)
					format[0] = *s++;
				else
					return(-1);
				format[1] = 0;
				n = atoi(format);
				for (i = 0; i < n; i++) {
					if (*s)
						s++;
					else
						return(-1);
				}
				break;
/*
 * Get length of date/time string.
 */
			case 'D':
				int_data = va_arg(ap, int *);
				for (i = 0; i < 11; i++) {
					if (*s)
						s++;
					else
						return(-1);
				}
				*int_data = 11;
				break;
		}
		k++;
	}
	va_end(ap);

	return(0);
}


/*****************************************************************************/

int  getOverallLength( const char  *key,
                       ... )
{
	va_list ap;
	const char *k;
        char *char_data;
	char format[20];
	int int_data, leng = 0;

	k = key;

	va_start(ap, key);
	while (*k) {
		switch (*k) {
/*
 * Get length of character string.
 */
			case 'C':
				char_data = va_arg(ap, char *);
				leng += strlen(char_data) + 5;
				break;
/*
 * Get length of integer.
 */
			case 'I':
				int_data = va_arg(ap, int);
				sprintf(format, "%d", int_data);
				leng += strlen(format) + 1;
				break;
/*
 * Get length of date/time string.
 */
			case 'D':
				char_data = va_arg(ap, char *);
				leng += strlen(char_data);
				break;
		}
		k++;
	}
	va_end(ap);

	return(leng);
}


/*****************************************************************************/
