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
# Makefile for DIST_MGR 

# Delete the default suffixes for speed/efficiency
.SUFFIXES:

include $(PROJECT_DIR)/$(PROJECT_NAME)/MAKEDEFS/$(MACHINE)_$(OPER_SYS)


PROGRAM1_SRC = \
	ntcss_msg_dist.c

PROGRAM2_SRC = \
	ntcss_msg_dist_mngr.c

ARCHIVE3_SRC = \
	ntcss_msg_dist_common.c

PROGRAM4_SRC = \
	dm_resource_monitor.c

PROGRAM1_OBJ:= $(PROGRAM1_SRC:%.c=$(INRI_OBJ)/DM_%.o)
PROGRAM2_OBJ:= $(PROGRAM2_SRC:%.c=$(INRI_OBJ)/DM_%.o)
ARCHIVE3_OBJ:= $(ARCHIVE3_SRC:%.c=$(INRI_OBJ)/DM_%.o)
PROGRAM4_OBJ:= $(PROGRAM4_SRC:%.c=$(INRI_OBJ)/DM_%.o)

PROGRAM1 = $(CPROGS)/ntcss_msg_dist
PROGRAM2 = $(CPROGS)/ntcss_msg_dist_mngr
PROGRAM4 = $(CPROGS)/dm_resource_monitor


.PRECIOUS: $(PROGRAM1_SRC) $(PROGRAM2_SRC) $(ARCHIVE3_SRC) $(PROGRAM4_SRC)


all:	libs progs
libs:	$(ARCHIVE3)

progs:	$(PROGRAM1) $(PROGRAM2) $(PROGRAM4)


$(PROGRAM1_OBJ): $(INRI_OBJ)/DM_%.o:%.c
	@echo '       Compiling $<'
	@$(CC) -c $(GCC_FLAGS) $(FLAGS) $(INCLUDES) $< -o $@

$(PROGRAM2_OBJ): $(INRI_OBJ)/DM_%.o:%.c
	@echo '       Compiling $<'
	@$(CC) -c $(GCC_FLAGS) $(FLAGS) $(INCLUDES) $< -o $@

$(ARCHIVE3_OBJ): $(INRI_OBJ)/DM_%.o:%.c
	@echo '       Compiling $<'
	@$(CC) -c $(GCC_FLAGS) $(FLAGS) $(INCLUDES) $< -o $@

$(PROGRAM4_OBJ): $(INRI_OBJ)/DM_%.o:%.c
	@echo '       Compiling $<'
	@$(CC) -c $(GCC_FLAGS) $(FLAGS) $(INCLUDES) $< -o $@


$(PROGRAM1): $(PROGRAM1_OBJ) $(ARCHIVE3_OBJ)
	@echo '       Linking $@'
	@$(CC) $(GCC_OPTOMIZE_FLAGS) $(LFLAGS) -o $@ $^ $(LDPATH) \
		$(NTCSSLIBS) $(SOCKLIBS) $(STRLIBS) $(INILIBS) \
		$(SYSLIBS)

$(PROGRAM2): $(PROGRAM2_OBJ) $(ARCHIVE3_OBJ) $(PROGRAM2_OBJ)
	@@echo '       Linking $@'
	@$(CC) $(GCC_OPTOMIZE_FLAGS) $(LFLAGS) -o $@ $^ $(LDPATH) \
		$(NTCSSLIBS) $(STRLIBS) $(SOCKLIBS) $(INILIBS) \
		$(SYSLIBS)

$(PROGRAM4): $(PROGRAM4_OBJ) $(ARCHIVE3_OBJ)
	@@echo '       Linking $@'
	@$(CC) $(GCC_OPTOMIZE_FLAGS) $(GCC_FLAGS) $(FLAGS) $(LFLAGS) \
		$(INCLUDES) -o $@ $^ $(LDPATH) \
		$(NTCSSLIBS) $(STRLIBS) $(SOCKLIBS) $(INILIBS) \
		$(SYSLIBS)

clean:
	@echo '       Removing .o files & programs'
	@$(RM) -f core *.o \
		$(PROGRAM1_OBJ) $(PROGRAM1) \
		$(PROGRAM2_OBJ) $(PROGRAM2) \
		$(ARCHIVE3_OBJ) \
		$(PROGRAM4_OBJ) $(PROGRAM4)

