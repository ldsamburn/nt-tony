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
# Makefile for dblib directory 

include $(PROJECT_DIR)/$(PROJECT_NAME)/MAKEDEFS/$(MACHINE)_$(OPER_SYS)


ARCHIVE1 = $(INRI_LIBS_LIB)/libDbInri.a

SRC_FILES = \
	alloc.c \
	checkfree.c \
	close.c \
	delete.c \
	dodelete.c \
	error.c \
	fetch.c \
	find.c \
	findfree.c \
	free.c \
	hash.c \
	lockreg.c \
	nextrec.c \
	open.c \
	readdat.c \
	readidx.c \
	readptr.c \
	rewind.c \
	store.c \
	writedat.c \
	writeidx.c \
	writeptr.c

OBJ_FILES:= $(SRC_FILES:%.c=$(INRI_OBJ)/dblib_%.o)

.PRECIOUS: $(ARCHIVE1) $(ARCHIVE1_SRC)

all:	libs
libs:	$(ARCHIVE1)

####### General rules ################################

$(OBJ_FILES):$(INRI_OBJ)/dblib_%.o:%.c
	@echo '       Compiling $<'
	@$(CC) -c $(GCC_FLAGS) $(INCLUDES) $< -o $@

####### Specific Rules ###############################

$(ARCHIVE1):$(OBJ_FILES)
	@echo '       Building $@'
	@$(AR) $(ARFLAGS) $@ $?

clean:
	@echo '       Removing .o files and $(ARCHIVE1)'
	@$(RM) -f core $(OBJ_FILES) $(ARCHIVE1)
