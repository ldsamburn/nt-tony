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
# Makefile for BCST_SERVER

# Delete the default suffixes for speed/efficiency
.SUFFIXES:

include $(PROJECT_DIR)/$(PROJECT_NAME)/MAKEDEFS/$(MACHINE)_$(OPER_SYS)

PROGRAM1_SRC = bcserver.c
PROGRAM2_SRC = plist.c

PROGRAM1 = $(CPROGS)/bcserver
PROGRAM2 = $(CPROGS)/plist

#Protect these targets from removal upon errors:
.PRECIOUS: $(PROGRAM1_SRC) $(PROGRAM2_SRC)

all:	progs
progs:	$(PROGRAM1) $(PROGRAM2)



$(PROGRAM1): $(PROGRAM1_SRC)
	@echo '       Compiling/Linking $@'
	@$(CC) $(LDFLAGS) $(GCC_FLAGS) $(INCLUDES) -pipe $< -o $@ $(LDPATH) \
	$(ARCHIVE1) $(SYSLIBS)

$(PROGRAM2): $(PROGRAM2_SRC)
	@echo '       Compiling/Linking $@'
	@$(CC) $(LDFLAGS) $(GCC_FLAGS) $(INCLUDES) -pipe $< -o $@ $(LDPATH) \
	$(ARCHIVE2) $(SYSLIBS) $(NTCSSLIBS) $(INILIBS) $(STRLIBS) $(SOCKLIBS)

clean:
	@echo '       Removing programs.'
	@$(RM) -f core $(PROGRAM1) $(PROGRAM2)

