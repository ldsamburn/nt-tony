
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * BooleanFcts.c
 */

/** SYSTEM INCLUDES **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** INRI INCLUDES **/

#include <BooleanFcts.h>

/** LOCAL INCLUDES **/

#include "BooleanFctsP.h"



/** FUNCTION PROTOTYPES **/

int booleanEval(char*);
static int check_boolean_str(char*);
static int single_paren_eval(char*, char*);
static int no_paren_eval(char*, char*);
int isBooleanChar(char, int);
static int do_pair_operation(char, int, int, int*, int*);



/** FUNCTION DEFINITIONS **/

int booleanEval(str)
char *str;
{
	static int check = 1;
	char *op = 0, *s = str;
	int value, found_op = 0;
	
	if (check) {
		if (!check_boolean_str(str))
			return(-1);
		check = 0;
	}

	while (*s) {
		switch (*s) {
			case OP_SYM:
				op = s;
				found_op = 1;
				break;

			case CP_SYM:
				if (!found_op)
					return(-1);
				if ((value = single_paren_eval(op, s)) == -1)
					return(-1);
				return(booleanEval(str));
		}
		s++;
	}
	if (found_op)
		return(-1);

	return(no_paren_eval(str, str + (strlen(str) - 1)));
}



/*+
 * Returns 1 if string "str" is a valid
 * boolean expression else returns 0.
 */
static int check_boolean_str(str)
char *str;
{
	char *s = str;

	while (*s) {
		if (!isBooleanChar(*s, 1))
			return(0);
		s++;
	}

	if (strstr(str, "()") != NULL || strstr(str, ")(") != NULL)
		return(0);
	if (strstr(str, ")0") != NULL || strstr(str, "0(") != NULL)
		return(0);
	if (strstr(str, ")1") != NULL || strstr(str, "1(") != NULL)
		return(0);

	return(1);
}



static int single_paren_eval(s, e)
char *s, *e;
{
	char *sp, *em, *ee;
	int value;

	ee = em = s;
	ee++;
	sp = ee;
	while (ee != e) {
		em++;
		ee++;
	}

	value = no_paren_eval(sp, em);
	switch (value) {
		case 0:
			*s = ZERO_SYM;
			*e = ZERO_SYM;
			return(0);

		case 1:
			*s = ONE_SYM;
			*e = ONE_SYM;
			return(1);

		default:
			return(-1);
	}
}


/*+
 * Evaluate a boolean string expression of
 * nothing but boolean symbols and ones and
 * zeros.  Return 1 if true, 0 if false, and
 * -1 if syntactically incorrect. "str" should
 * point at the first char in the string and
 * "end_str" should point at the last char.
 */
static int no_paren_eval(str, end_str)
char *str, *end_str;
{
	char *s, *f, last_token, operation;
	int not1, not2, value;

	value = -1;
	s = f = str;
	operation = last_token = BEG_SYM;
	not1 = not2 = 0;

	while (1) {
		switch (*s) {
			case ZERO_SYM:
				switch (last_token) {
					case ZERO_SYM:
						break;

					case BEG_SYM:
						value = 0;
						break;

					case NOT_SYM:
						if (value == -1) value = 0;
					case AND_SYM:
					case OR_SYM:
						if ((value = do_pair_operation(operation, value, 0, &not1, &not2)) == -1)
							return(-1);
						break;

					default:
						return(-1);
				}
				break;

			case ONE_SYM:
				switch (last_token) {
					case ONE_SYM:
						break;

					case BEG_SYM:
						value = 1;
						break;

					case NOT_SYM:
						if (value == -1) value = 1;
					case AND_SYM:
					case OR_SYM:
						if ((value = do_pair_operation(operation, value, 1, &not1, &not2)) == -1)
							return(-1);
						break;

					default:
						return(-1);
				}
				break;

			case NOT_SYM:
				switch (last_token) {
					case BEG_SYM:
						not1 = 1;
						break;

					case ZERO_SYM:
					case ONE_SYM:
					case AND_SYM:
					case OR_SYM:
						not2 = 1;
						break;

					default:
						return(-1);
				}
				break;

			case AND_SYM:
			case OR_SYM:
				switch (last_token) {
					case ZERO_SYM:
					case ONE_SYM:
						break;

					default:
						return(-1);
				}
				operation = *s;
				break;

			default:
				return(-1);
		}
		last_token = *s;

		if (s == end_str || s == NULL || *s == 0)
			break;

		s++;
	}

/*
 * Turn string to all ones or all zeros.
 */
	while (1) {
		if (value)
			*f = ONE_SYM;
		else
			*f = ZERO_SYM;

		if (f == end_str || f == NULL || *f == 0)
			break;

		f++;
	}

	return(value);
}




/*+
 * Determine if charcter "c" is a boolean
 * symbol.  If "include_nmbers" is 1 then
 * function will return 1 if character is
 * a one or zero.
 */
int isBooleanChar(char c, int include_numbers)
{
	switch (c) {
		case AND_SYM:
		case OR_SYM:
		case NOT_SYM:
		case CP_SYM:
		case OP_SYM:
			return(1);

		case ZERO_SYM:
		case ONE_SYM:
			if (include_numbers)
				return(1);
			else
				return(0);

		default:
			return(0);
	}
}



/*+
 * Evaluate a single boolean expression.
 */
static int do_pair_operation(char operation, int val1, int val2, int* not1p, 
                            int* not2p)
{
	int value;

	switch (operation) {
		case BEG_SYM:
			value = *not1p^val1;
			break;

		case OR_SYM:
			value = (*not1p^val1)|(*not2p^val2);
			break;

		case AND_SYM:
			value = (*not1p^val1)&(*not2p^val2);
			break;

		default:
			return(-1);
	}
	*not1p = *not2p = 0;

	return(value);
}
