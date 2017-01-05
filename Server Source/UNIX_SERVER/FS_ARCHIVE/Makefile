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
# Makefile for FS_ARCHIVE 
# Delete the default suffixes for speed/efficiency
.SUFFIXES:

include $(PROJECT_DIR)/$(PROJECT_NAME)/MAKEDEFS/$(MACHINE)_$(OPER_SYS)

PROGRAM1_SRC = \
	fs_archive.c \
	callbacks.c \
	widgets.c \
	misc_fcts.c \
	cron_tar_fcts.c \
	file_sel_box.c \
	problem_defs.c \
	NtcssHelp.c

PROGRAM2_SRC = \
	read_header.c

PROGRAM3_SRC = \
	restore_tape.c

PROGRAM5_SRC = \
	backup_tape.c

ARCHIVE1_SRC = \
	tarlist_fcts.c


PROGRAM1_OBJ:= $(PROGRAM1_SRC:%.c=$(INRI_OBJ)/FSA_%.o)
PROGRAM3_OBJ:= $(PROGRAM3_SRC:%.c=$(INRI_OBJ)/FSA_%.o)
PROGRAM5_OBJ:= $(PROGRAM5_SRC:%.c=$(INRI_OBJ)/FSA_%.o)
ARCHIVE1_OBJ:= $(ARCHIVE1_SRC:%.c=$(INRI_OBJ)/FSA_%.o)


PROGRAM1 = $(CPROGS)/fs_archive
PROGRAM2 = $(CPROGS)/read_header
PROGRAM3 = $(CPROGS)/restore_tape
PROGRAM4 = $(CPROGS)/restore_tape.gtar
PROGRAM5 = $(CPROGS)/backup_tape
PROGRAM6 = $(CPROGS)/backup_tape.gtar

#Protect these targets from removal upon errors:
.PRECIOUS: $(ARCHIVE1) $(ARCHIVE1_SRC) $(PROGRAM1_SRC) $(PROGRAM2_SRC) \
		 $(PROGRAM3_SRC) $(PROGRAM5_SRC)

all:	libs progs
libs:	$(ARCHIVE1)

progs:	$(PROGRAM1) $(PROGRAM2) $(PROGRAM3) $(PROGRAM4) \
		$(PROGRAM5) $(PROGRAM6)


$(PROGRAM1_OBJ):$(INRI_OBJ)/FSA_%.o:%.c
	@echo '       Compiling $<'
	@$(CC) -c $(GCC_FLAGS) $(INCLUDES) $< -o $@

$(PROGRAM3_OBJ):$(INRI_OBJ)/FSA_%.o:%.c
	@echo '       Compiling $<'
	@$(CC) -c $(GCC_FLAGS) $(INCLUDES) $< -o $@

$(PROGRAM5_OBJ):$(INRI_OBJ)/FSA_%.o:%.c
	@echo '       Compiling $<'
	@$(CC) -c $(GCC_FLAGS) $(INCLUDES) $< -o $@


$(ARCHIVE1_OBJ):$(INRI_OBJ)/FSA_%.o:%.c
	@echo '       Compiling $<'
	@$(CC) -c $(GCC_FLAGS) $(INCLUDES) $< -o $@





$(PROGRAM1): $(PROGRAM1_OBJ) $(ARCHIVE1_OBJ)
	@echo '       Linking $@'
	@$(CC) $(LFLAGS) $(GCC_OPTOMIZE_FLAGS) -o $@ $^ $(LDPATH) \
		$(NTCSSLIBS) $(INILIBS) $(SOCKLIBS) $(STRLIBS) $(XLIBS) \
		$(SYSLIBS)

$(PROGRAM2): $(PROGRAM2_SRC)
	@echo '       Compiling & Linking $@'
	@$(CC) $(GCC_X_FLAGS) $(INCLUDES) $(LFLAGS) $(GCC_OPTOMIZE_FLAGS) \
		-pipe -o $@ $^ $(LDPATH) \
		$(NTCSSLIBS) $(INILIBS) $(STRLIBS) $(SOCKLIBS) $(SYSLIBS)

$(PROGRAM3): $(PROGRAM3_SRC)
	@echo '       Compiling & Linking $@'
	@$(CC) $(GCC_X_FLAGS) $(INCLUDES) $(LFLAGS) $(GCC_OPTOMIZE_FLAGS) \
		-pipe -o $@ $^ $(LDPATH) \
		$(NTCSSLIBS) $(INILIBS) $(STRLIBS) $(SOCKLIBS) $(SYSLIBS)

$(PROGRAM4): $(PROGRAM3)
	@echo '       Copying $? to $@'
	@cp $(PROGRAM3) $(PROGRAM4)

$(PROGRAM5): $(PROGRAM5_OBJ) $(ARCHIVE1_OBJ)
	@echo '       Linking $@'
	@$(CC) $(LFLAGS) $(GCC_OPTOMIZE_FLAGS) -o $@ $^ $(LDPATH) \
		$(NTCSSLIBS) $(INILIBS) $(STRLIBS) $(SOCKLIBS) $(SYSLIBS)

$(PROGRAM6): $(PROGRAM5)
	@echo '       Copying $? to $@'
	@cp $(PROGRAM5) $(PROGRAM6)

clean:
	@echo '       Removing .o files & programs'
	@$(RM) -f core \
		$(ARCHIVE1_OBJ) \
		$(PROGRAM1_OBJ) $(PROGRAM1) \
		$(PROGRAM2) \
		$(PROGRAM3_OBJ) $(PROGRAM3) \
		$(PROGRAM4) \
		$(PROGRAM5_OBJ) $(PROGRAM5) \
		$(PROGRAM6)
