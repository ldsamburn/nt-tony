/*
 * BundleData.c
 */

/** SYSTEM INCLUDES **/

#include <stdlib.h> 
#include <windows.h>
#include <string.h>
//#include <ctype.h>
//#include <varargs.h>

/** FUNCTION PROTOTYPES **/

/** FUNCTION DEFINITIONS **/

int bundleData(char *str, char *key,...)
{
	va_list ap;
	char *k, *char_data;
	char format[20], fmt_str[10];
	int int_data, n, leng = 0;

	k = key;
	
	va_start(ap, key);
	while (*k) {
/*
 * Bundle character string.
 */
		switch (*k) {
			case 'C':
				char_data = va_arg(ap, char *);
				n = strlen(char_data);
				wsprintf(format, "%5.5d", n);
				strcat(str, format);
				strcat(str, char_data);
				leng += n + 5;
				break;
/*
 * Bundle integer string.
 */
			case 'I':
				int_data = va_arg(ap, int);
				wsprintf(format, "%d", int_data);
				n = strlen(format);
				wsprintf(fmt_str, "%%1d%%%d.%dd", n, n);
				wsprintf(format, fmt_str, n, int_data);
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

			default:
				return(-1);
				break;
		}
		k++;
	}
	va_end(ap);

	return(leng);
}



char *unbundleData(char *str, char *key,... )
{
	va_list ap;
	char *s, *k, *char_data;
	char format[20];
	int *int_data, i, n;

	s = str;
	k = key;

	va_start(ap,key);
	while (*k) {
/*
 * Unbundle character string.
 */
		memset(format,NULL,20);
		switch (*k) {
			case 'C':
				char_data = va_arg(ap, char *);
				for (i = 0; i < 5; i++) {
					if (*s && isdigit((int)*s))
						format[i] = *s++;
					else
						return(NULL);
				}
				format[5] = 0;
				n = atoi(format);
				for (i = 0; i < n; i++) {
					if (*s)
						char_data[i] = *s++;
					else
						return(NULL);
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
					return(NULL);

				format[1] = 0;
				n = atoi(format);
				if (n > 20)
					return(NULL);

				for (i = 0; i < n; i++) {
					if (*s)
						format[i] = *s++;
					else
						return(NULL);
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
						return(NULL);
				}
				char_data[11] = 0;
				break;

			default:
				return(NULL);
				break;
		}
		k++;
	}
	va_end(ap);

	return(s);
}
