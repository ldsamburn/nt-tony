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
# Makefile for ntcsslib directory 

include $(PROJECT_DIR)/$(PROJECT_NAME)/MAKEDEFS/$(MACHINE)_$(OPER_SYS)

ARCHIVE1 := $(INRI_LIBS_LIB)/libNtcssInri.a

SRC_FILES := NtcssNisFcts.c \
	NtcssSystemSettingsFcts.c \
	NetUserInfo.c \
	NetSvrInfo.c \
	UserFcts.c \
	PrtFcts.c \
	ProgFcts.c \
	LockDB.c \
	FileFcts.c \
	ManipData.c \
	BundleData.c \
	HashDB.c \
	MsgFcts.c \
	NtcssCrypt.c \
	d3des.c \
	lp_ops.c \
	printableOutputCrypt.c \
	Replication.c

OBJ_FILES:= $(SRC_FILES:%.c=$(INRI_OBJ)/ntcsslib_%.o)

.PRECIOUS: $(ARCHIVE1) $(SRC_FILES)

all:	libs
libs:	$(ARCHIVE1)

####### General rules ################################

$(OBJ_FILES):$(INRI_OBJ)/ntcsslib_%.o:%.c
	@echo '       Compiling $<'
	@$(CC) -c $(GCC_FLAGS) -Wno-format-y2k $(INCLUDES) $< -o $@


####### Specific Rules ###############################

$(ARCHIVE1): $(OBJ_FILES)
	@echo '       Building $@'
	@$(AR) $(ARFLAGS) $@ $?

clean:
	@echo '       Removing .o files and $(ARCHIVE1)'
	@$(RM) -f core $(OBJ_FILES) $(ARCHIVE1)
