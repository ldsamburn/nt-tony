
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * test.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <StringFcts.h>
#include <BooleanFcts.h>


int main()
{
	char word[300], buf[100];
	int i, n;
	char strg[25][80], *strgptr[25];
	static char *str = "	test \"sentence to\" to be ";
	char *s;
	char *ss, timestr[TIME_STR_LEN + 1];

	i = 0;
	while((i = stripWord(i, str, word)) != -1)
		printf("%3d -%s- -%s-\n", i, str, word);

	for (i = 0; i < 15; i++)
		strgptr[i] = &strg[i][0];
	strcpy(strgptr[0], "");
	strcpy(strgptr[1], "eric");
	strcpy(strgptr[2], "");
	strcpy(strgptr[3], "bs");
	strcpy(strgptr[4], "ts");
	strcpy(strgptr[5], "bs");
	strcpy(strgptr[6], "");
	strcpy(strgptr[7], "eric");
	strcpy(strgptr[8], "");
	strcpy(strgptr[9], "ds");
	strcpy(strgptr[10], "its");
	strcpy(strgptr[11], "");
	strcpy(strgptr[12], "eric");
	strcpy(strgptr[13], "bs ");
	strcpy(strgptr[14], "bs ");
	for (i = 0; i < 15; i++)
		printf("%3d -%s-\n", i, strgptr[i]);
	n = removeDuplicates(strgptr, 15);
	printf("\n");
	for (i = 0; i < n; i++)
		printf("%3d -%s-\n", i, strgptr[i]);

	printf("-%s-\n", stripPath("oo.s"));
	printf("-%s-\n", stripPath("/sksks/sksks/debdudu/kssko.s"));
	printf("-%s-\n", stripPath("sksks/sksks/debdudu/eric.c"));
	printf("-%s-\n", stripPath("sksks/sksks/debdudu/"));

	printf("--%s--\n", getenv("LOGNAME"));


	replacePattern("xx", "ABEL", "ksks sksxx ksksksxxksksxx", word);
	printf("%s\n", "ksks sksxx ksksksxxksksxx");
	printf("%s\n\n", word);


	strcpy(word, "!1111|0000&1");
	strcpy(word, "000|1|000&!0");
	strcpy(word, "!1|0|1&1|0&1");
	strcpy(word, "111111110111");
	strcpy(word, "111111111111");
	strcpy(word, "!111111&1111");
	strcpy(word, "!|11111&1111");
	strcpy(word, "!0&1|11&1111");
	strcpy(word, "!0&1|11&!111");
	strcpy(word, "(!0&1|11&!111)|1");
	strcpy(word, "(!(((!1|0)&1(1|0)|(00|!00))&!0))");
	strcpy(word, "(!(((!1|0)&(01|0)|(00|!00))&!0))");
	strcpy(word, "!(((!1|0)&(1|0)|(00|!00))&!0)");
	printf("   -%s-\n", word);
	printf("%2d -%s-\n", booleanEval(word), word); 

	printf("\n");
	getTimeStr(timestr);
	strcpy(word, "931203.1411");
	if (isValidTimeStr(timestr)) {
		if (timeStrCmp(timestr, word) < 0)
			printf("%s is later than %s\n", timestr, word);
		else if (timeStrCmp(timestr, word) > 0)
			printf("%s is earlier than %s\n", timestr, word);
		else
			printf("%s is equal to %s\n", timestr, word);
	} else
		printf("%s is not a valid time str\n", timestr);

	strcpy(word, "	");
	printf("-%s- is blank if this value is one: %d\n", word, isBlank(word));
	strcpy(word, "		d	");
	printf("-%s- is blank if this value is one: %d\n\n", word, isBlank(word));

	(void) timeStrToReadable("931203.1411", buf);
	printf("-%s- to readable -%s-\n", "931203.1411", buf);

	(void) readableToTimeStr("2 Mar 98 00:22", buf);
	printf("-%s- to time str -%s-\n", "2 Mar 98 00:22", buf);

	(void) readableToTimeStr("2 Jan 58 20:22", buf);
	printf("-%s- to time str -%s-\n\n", "2 Jan 58 20:22", buf);

	for (i = 0; i < 25; i++)
		strgptr[i] = &strg[i][0];

	if ((n = parseArgs(str, strgptr)) != -1) {
		for (i = 0; i < n; i++)
			printf(" %d -%s-\n", i, strgptr[i]);
	} else
		printf("could not parse args\n");

	i = getWeekDayOfTimeStr("700101.0000");
	printf("\nday is %d\n", i);
	i = getWeekDayOfTimeStr("700102.0000");
	printf("day is %d\n", i);
	i = getWeekDayOfTimeStr("951215.1001");
	printf("day is %d\n", i);
	i = getWeekDayOfTimeStr("940723.0001");
	printf("day is %d\n", i);
	i = getWeekDayOfTimeStr("830707.1101");
	printf("day is %d\n", i);
	i = getWeekDayOfTimeStr("960602.1301");
	printf("day is %d\n", i);
	i = getWeekDayOfTimeStr("011031.2131");
	printf("day is %d\n\n", i);

	(void) getNextFullTimeStr("950228.100159", buf, DAY_TYPE, 1);
	printf("next day   950228.100159  %s\n", buf);
	(void) getNextFullTimeStr("960228.100159", buf, DAY_TYPE, 1);
	printf("next day   960228.100159  %s\n", buf);
	(void) getNextFullTimeStr("951231.100159", buf, MONTH_TYPE, 2);
	printf("2 months   951231.100159  %s\n", buf);
	(void) getNextFullTimeStr("960130.100159", buf, MONTH_TYPE, 1);
	printf("next month 960130.100159  %s\n", buf);
	(void) getNextFullTimeStr("990228.100159", buf, YEAR_TYPE, 1);
	printf("next year  990228.100159  %s\n", buf);
	(void) getNextFullTimeStr("990225.100159", buf, WEEK_TYPE, 1);
	printf("next week  990225.100159  %s\n", buf);
	(void) getNextFullTimeStr("990225.230159", buf, HOUR_TYPE, 3);
	printf("3 hours    990225.230159  %s\n", buf);
	(void) getNextFullTimeStr("990225.235959", buf, MINUTE_TYPE, 1);
	printf("next min   990225.235959  %s\n", buf);
	(void) getNextFullTimeStr("991231.235959", buf, MINUTE_TYPE, 2);
	printf("2 mins     991231.235959  %s\n", buf);
	printf("\n");

	(void) fullTimeStrToReadable("931203.141159", buf);
	printf("-%s- to readable -%s-\n", "931203.141159", buf);

	strcpy(buf, "991231.021159");
	strcpy(word, "991231.021157");
	if (fullTimeStrCmp(buf, word) < 0)
		printf("%s is later than %s\n", buf, word);
	else if (fullTimeStrCmp(buf, word) > 0)
		printf("%s is earlier than %s\n", buf, word);
	else
		printf("%s is equal to %s\n", buf, word);

	memset(word, ' ', 30);
	word[30] = 0;

	memcpy(word, "eric", 4);
	memcpy(&word[10], "bill", 4);
	memcpy(&word[20], "tonie", 5);
	printf("01234567890123456789012345678901234567890\n");
	printf("%s\n", word);

	convertScheduleString("H592303112", word);
	printf("\npretty -%s- for H592303112\n", word);
	convertScheduleString("W592303112", word);
	printf("pretty -%s- for W592303112\n", word);
	convertScheduleString("E592303112", word);
	printf("pretty -%s- for E592303112\n", word);


	printf("\n\n");
	strcpy(word, "abcdefghij\n2llalalala\n3sksksksksk\n");
	s = word;
	i = 1;
	while ((ss = memGet(buf, 65, s)) != NULL) {
		s = ss;
		printf("line %d-%s-", i++, buf);
	}


	printf("\n%d is ret for 204\n", getChildProcess(204));

        return(0);

}

