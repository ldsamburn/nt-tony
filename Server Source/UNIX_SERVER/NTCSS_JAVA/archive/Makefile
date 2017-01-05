
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

include $(PROJECT_DIR)/$(PROJECT_NAME)/MAKEDEFS/$(MACHINE)_$(OPER_SYS)

########################################################
### Makefile for NTCSS File System Archive
########################################################

### Defined paths ###
NTCSS_JAVA_DIR	= ${PROJECT_DIR}/NTCSSII/NTCSS_JAVA
CLASS_DIR	= $(NTCSS_JAVA_DIR)/classes

### JAVA Tools
JAR	= ${JAVA_HOME}/bin/jar

### Jar File ###
JAR_FILE = $(CLASS_DIR)/ntcss_fs_archive.jar


########################################################
### Targets ###
########################################################

all:
	@cd common; if [ -f Makefile ]; then $(MAKE) all; fi
	@cd client; if [ -f  Makefile ]; then $(MAKE) all; fi
	@cd server; if [ -f Makefile ]; then $(MAKE) all; fi
	@cd $(CLASS_DIR); $(JAR) cf $(JAR_FILE) COMMON archive

clean:
	@cd common; if [ -f Makefile ]; then $(MAKE) clean; fi
	@cd client; if [ -f Makefile ]; then $(MAKE) clean; fi
	@cd server; if [ -f Makefile ]; then $(MAKE) clean; fi
	@echo "       Removing $(JAR_FILE)"
	@rm -f $(JAR_FILE)

