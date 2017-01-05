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
# Makefile for INSTALL 

# Delete the default suffixes for speed/efficiency
.SUFFIXES:

include $(PROJECT_DIR)/$(PROJECT_NAME)/MAKEDEFS/$(MACHINE)_$(OPER_SYS)


ARCHIVE1_SRC = \
	database.c \
	misc_fcts.c \
	grp_fcts.c

PROGRAM1_SRC = \
	install.c

PROGRAM2_SRC = \
	prime_db.c

PROGRAM3_SRC = \
	display_number.c

PROGRAM4_SRC = \
	create_xref_file.c


PROGRAM1_OBJ:= $(PROGRAM1_SRC:%.c=$(INRI_OBJ)/INSTALL_%.o)
PROGRAM2_OBJ:= $(PROGRAM2_SRC:%.c=$(INRI_OBJ)/INSTALL_%.o)
PROGRAM3_OBJ:= $(PROGRAM3_SRC:%.c=$(INRI_OBJ)/INSTALL_%.o)
PROGRAM4_OBJ:= $(PROGRAM4_SRC:%.c=$(INRI_OBJ)/INSTALL_%.o)
ARCHIVE1_OBJ:= $(ARCHIVE1_SRC:%.c=$(INRI_OBJ)/INSTALL_%.o)


PROGRAM1 = $(CPROGS)/install
PROGRAM2 = $(CPROGS)/prime_db
PROGRAM3 = $(CPROGS)/display_number
PROGRAM4 = $(CPROGS)/create_xref_file

# We break with tradition here (by creating an actual archive file)
#  since there are functions/static var references in database.c
#  that have no reference in install.c, but the linker cannot resolve
# in the link stage.  Using a .a resolves the linker's issues.

ARCHIVE1 = $(CLIBS)/install_lib.a

#Protect these targets from removal upon errors:
.PRECIOUS: $(ARCHIVE1_SRC) $(PROGRAM1_SRC) $(PROGRAM2_SRC) \
		 $(PROGRAM3_SRC) $(PROGRAM4_SRC)

all:	libs progs
libs:	$(ARCHIVE1)
progs:	$(PROGRAM1) $(PROGRAM2) $(PROGRAM3) $(PROGRAM4)

$(PROGRAM1_OBJ):$(INRI_OBJ)/INSTALL_%.o:%.c
	@echo '       Compiling $<'
	@$(CC) -c $(GCC_FLAGS) $(INCLUDES) $< -o $@

$(PROGRAM2_OBJ):$(INRI_OBJ)/INSTALL_%.o:%.c
	@echo '       Compiling $<'
	@$(CC) -c $(GCC_FLAGS) $(INCLUDES) $< -o $@

$(PROGRAM3_OBJ):$(INRI_OBJ)/INSTALL_%.o:%.c
	@echo '       Compiling $<'
	@$(CC) -c $(GCC_FLAGS) $(INCLUDES) $< -o $@

$(PROGRAM4_OBJ):$(INRI_OBJ)/INSTALL_%.o:%.c
	@echo '       Compiling $<'
	@$(CC) -c $(GCC_FLAGS) $(INCLUDES) $< -o $@

$(ARCHIVE1_OBJ):$(INRI_OBJ)/INSTALL_%.o:%.c
	@echo '       Compiling $<'
	@$(CC) -c $(GCC_FLAGS) $(INCLUDES) $< -o $@

# The only reason we created an archive is because of an extern int that
# is not used everywhere, but just linking the .o would force resolution.
$(ARCHIVE1):$(ARCHIVE1_OBJ)
	@echo '       Building $@'
	@$(AR) $(ARFLAGS) $@ $^

$(PROGRAM1): $(PROGRAM1_OBJ) $(ARCHIVE1)
	@echo '       Linking $@'
	@$(CC) $(LFLAGS) $(GCC_OPTOMIZE_FLAGS) -o $@ $^ $(LDPATH) \
		$(NTCSSLIBS) $(INILIBS) $(STRLIBS) $(SOCKLIBS) $(SYSLIBS)

$(PROGRAM2): $(PROGRAM2_OBJ) $(ARCHIVE1)
	@echo '       Linking $@'
	@$(CC) $(LFLAGS) $(GCC_OPTOMIZE_FLAGS) -o $@ $^ $(LDPATH) \
		$(NTCSSLIBS) $(INILIBS) $(STRLIBS) $(SOCKLIBS) $(SYSLIBS)

$(PROGRAM3): $(PROGRAM3_OBJ) $(ARCHIVE1)
	@echo '       Linking $@'
	@$(CC) $(LFLAGS) $(GCC_OPTOMIZE_FLAGS) -o $@ $^ $(LDPATH) \
		$(NTCSSLIBS) $(INILIBS) $(STRLIBS) $(SOCKLIBS) $(SYSLIBS)

$(PROGRAM4): $(PROGRAM4_OBJ) $(ARCHIVE1)
	@echo '       Linking $@'
	@$(CC) $(LFLAGS) $(GCC_OPTOMIZE_FLAGS) -o $@ $^ $(LDPATH) \
		$(NTCSSLIBS) $(INILIBS) $(STRLIBS) $(SOCKLIBS) $(SYSLIBS)


clean:
	@echo '       Removing .o files & programs'
	@$(RM) -f core \
		$(ARCHIVE1_OBJ) $(ARCHIVE1) \
		$(PROGRAM1_OBJ) $(PROGRAM1) \
		$(PROGRAM2_OBJ) $(PROGRAM2) \
		$(PROGRAM3_OBJ) $(PROGRAM3) \
		$(PROGRAM4_OBJ) $(PROGRAM4)
