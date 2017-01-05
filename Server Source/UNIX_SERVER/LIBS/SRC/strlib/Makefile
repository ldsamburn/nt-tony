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
# Makefile for strlib directory 

# Delete the default suffixes for speed/efficiency
.SUFFIXES:

include $(PROJECT_DIR)/$(PROJECT_NAME)/MAKEDEFS/$(MACHINE)_$(OPER_SYS)

ARCHIVE1:= $(INRI_LIB)/libStrInri.a
#ARCHIVE2:= $(CLIBS)/test.a

ARCHIVE1_SRC = \
	StringFcts.c \
	BooleanFcts.c

ARCHIVE2_SRC = \
	test.c

PROGRAM1:= $(CPROGS)/test

OBJ_FILES1:= $(ARCHIVE1_SRC:%.c=$(INRI_OBJ)/strlib_%.o)
#OBJ_FILES2:= $(ARCHIVE2_SRC:%.c=$(INRI_OBJ)/strlib_%.o)

#Protect these targets from removal upon errors:
.PRECIOUS:= $(ARCHIVE1) $(ARCHIVE1_SRC) $(ARCHIVE2_SRC)


all:	libs
libs:	$(ARCHIVE1)
#test:	$(ARCHIVE1) $(PROGRAM1)

####### General rules ################################

$(OBJ_FILES1): $(INRI_OBJ)/strlib_%.o: %.c
	@echo '       Compiling $<'
	@$(CC) -c $(GCC_FLAGS) -Wno-format-y2k $(INCLUDES) $< -o $@

#$(OBJ_FILES2):$(INRI_OBJ)/strlib_%.o:%.c
#	@echo '       Compiling $<'
#	@$(CC) -c $(GCC_FLAGS) $(INCLUDES) $< -o $@

####### Specific Rules ###############################

$(ARCHIVE1): $(OBJ_FILES1)
	@echo '       Building $@'
	@$(AR) $(ARFLAGS) $@ $?

# This didn't compile originally anyway.. it should be removed eventually.
#$(PROGRAM1): $(ARCHIVE1) $(OBJ_FILES2)
#	$(CC) $(LDFLAGS) $(GCC_FLAGS) $(GCC_OPTOMIZE_FLAGS) $(INCLUDES) \
#	-o $@ $(LDPATH) $(STRLIBS) $(OBJ_FILES2) $(STRLIBS)

clean:
	@echo '       Removing .o files and $(ARCHIVE1)'
	@$(RM) -f core $(OBJ_FILES1) $(OBJ_FILES2) $(ARCHIVE1)


