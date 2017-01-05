/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// Packaging prototypes

extern "C" {
	int FAR bundleData(char far *str, char far *key, ...); 
	char far * FAR unbundleData(char far *str, char far *key, ...);
}
