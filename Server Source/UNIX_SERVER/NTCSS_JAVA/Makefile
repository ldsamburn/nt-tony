
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

# The output of this Makefile is the ntcss_cntrl_pnl.jar and 
#  ntcss_fs_archive.jar jar files in the NTCSS_JAVA/classes directory
#  The COMMON directory is compiled into the CNTRL_PNL and archive
#  jar files.

# Only need to make on one platform. Since sol27 has a java 1.1.6 compiler,
#  we just compile on that platform. In MAKEDEFS/solaris_2.7, the variable
#  JAVA_HOME should point to the 1.1.6 jdk directory (probably /usr/java).

all:
	@if [ $(MACH_OS) = sol27 ]; then \
	  cd ${PROJECT_DIR}/NTCSSII/NTCSS_JAVA/COMMON; if [ -f Makefile ]; then $(MAKE) -f Makefile all; fi; \
	  cd ${PROJECT_DIR}/NTCSSII/NTCSS_JAVA/CNTRL_PNL; if [ -f Makefile ]; then $(MAKE) -f Makefile all; fi; \
	  cd ${PROJECT_DIR}/NTCSSII/NTCSS_JAVA/archive; if [ -f Makefile ]; then $(MAKE) -f Makefile all; fi; \
	fi

clean:
	@if [ $(MACH_OS) = sol27 ]; then \
	  cd ${PROJECT_DIR}/NTCSSII/NTCSS_JAVA/COMMON; if [ -f Makefile ]; then $(MAKE) -f Makefile clean; fi; \
	  cd ${PROJECT_DIR}/NTCSSII/NTCSS_JAVA/CNTRL_PNL; if [ -f Makefile ]; then $(MAKE) -f Makefile clean; fi; \
	  cd ${PROJECT_DIR}/NTCSSII/NTCSS_JAVA/archive; if [ -f Makefile ]; then $(MAKE) -f Makefile clean; fi; \
	fi
