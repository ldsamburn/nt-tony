#****************************************************************************
# 
#              Copyright (c)2002 Northrop Grumman Corporation
#
#                          All Rights Reserved
#
#
#    This material may be reproduced by or for the U.S. Government pursuant
#    to the copyright license under the clause at Defense Federal Acquisition
#    Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
#**************************************************************************** 
# Makefile for NTCSSII Libraries 

# Delete the default suffixes for speed/efficiency
.SUFFIXES:

all: str_all db_all ntcss_all sock_all ini_all x_all
clean: str_clean db_clean ntcss_clean sock_clean ini_clean x_clean

str_all:
	@cd strlib; if [ -f Makefile ]; then $(MAKE) -f Makefile all; fi
str_clean:
	@cd strlib; if [ -f Makefile ]; then $(MAKE) -f Makefile clean; fi

db_all:
	@cd dblib; if [ -f Makefile ]; then $(MAKE) -f Makefile all; fi
db_clean:
	@cd dblib; if [ -f Makefile ]; then $(MAKE) -f Makefile clean; fi

ntcss_all:
	@cd ntcsslib; if [ -f Makefile ]; then $(MAKE) -f Makefile all; fi
ntcss_clean:
	@cd ntcsslib; if [ -f Makefile ]; then $(MAKE) -f Makefile clean; fi

ini_all:
	@cd inilib; if [ -f Makefile ]; then $(MAKE) -f Makefile all; fi
ini_clean:
	@cd inilib; if [ -f Makefile ]; then $(MAKE) -f Makefile clean; fi

sock_all:
	@cd socklib; if [ -f Makefile ]; then $(MAKE) -f Makefile all; fi
sock_clean:
	@cd socklib; if [ -f Makefile ]; then $(MAKE) -f Makefile clean; fi

x_all:
	@cd xlib; if [ -f Makefile ]; then $(MAKE) -f Makefile all; fi
x_clean:
	@cd xlib; if [ -f Makefile ]; then $(MAKE) -f Makefile clean; fi

