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
# Makefile for the DB_CHECKER project

# Delete the default suffixes for speed/efficiency
.SUFFIXES:

include $(PROJECT_DIR)/$(PROJECT_NAME)/MAKEDEFS/$(MACHINE)_$(OPER_SYS)


#ARCHIVE1 = $(INRI_LIB)/db_checker.a

PROGRAM1_SRC = \
	db_checker.c

PROGRAM1 = $(CPROGS)/db_checker

#Protect these targets from removal upon errors:
.PRECIOUS: $(PROGRAM1_SRC)

all:	progs

progs:	$(PROGRAM1)

$(PROGRAM1): $(PROGRAM1_SRC)
	@echo '       Compiling/Linking  $@'
	@$(CC) $(GCC_FLAGS) $(INCLUDES)  $(LDFLAGS) $(GCC_OPTOMIZE_FLAGS) $? \
		-o $@ $(LDPATH) \
		$(NTCSSLIBS) $(STRLIBS) $(INILIBS) $(SOCKLIBS) $(SYSLIBS)

clean:
	@echo '       Removing programs.'
	@$(RM) -f core $(PROGRAM1)

