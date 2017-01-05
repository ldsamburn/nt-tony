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
# Makefile for socklib directory 

include $(PROJECT_DIR)/$(PROJECT_NAME)/MAKEDEFS/$(MACHINE)_$(OPER_SYS)


ARCHIVE1 = $(INRI_LIBS_LIB)/libSockInri.a

SRC_FILES = \
	cnt_client.c \
	cnt_server.c \
	read_sock.c \
	sock_init.c \
	send_msg.c \
	get_master.c \
	net_xfer.c \
	pack_ops.c \
	DBUF_from_text.c \
	DBUF_to_text.c

OBJ_FILES:= $(SRC_FILES:%.c=$(INRI_OBJ)/socklib_%.o)

.PRECIOUS: $(ARCHIVE1) $(SRC_FILES)

all:	libs
libs:	$(ARCHIVE1)


####### General rules ################################

$(OBJ_FILES):$(INRI_OBJ)/socklib_%.o:%.c
	@echo '       Compiling $<'
	@$(CC) -c $(GCC_FLAGS) $(INCLUDES) $< -o $@

####### Specific Rules ###############################

$(ARCHIVE1): $(OBJ_FILES)
	@echo '       Building $@'
	@$(AR) $(ARFLAGS) $@ $?

clean:
	@echo '       Removing .o files and $(ARCHIVE1)'
	@$(RM) -f core $(OBJ_FILES) $(ARCHIVE1)
