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
# Makefile for APIS 

# Delete the default suffixes for speed/efficiency
.SUFFIXES:

include $(PROJECT_DIR)/$(PROJECT_NAME)/MAKEDEFS/$(MACHINE)_$(OPER_SYS)


ARCHIVE1 = $(CLIBS)/libNtcssApi.sl
ARCHIVE6 = $(CLIBS)/libNtcssApi.a

ARCHIVE1_SRC = \
	NtcssApi.c \
	NtcssError.c \
	NtcssServerSocket.c \
	NtcssTimeoutAlarm.c \
	NtcssMessage.c
ARCHIVE6_SRC = \
	NtcssApi.c \
	NtcssError.c \
	NtcssServerSocket.c \
	NtcssTimeoutAlarm.c \
	NtcssMessage.c
PROGRAM1_SRC = \
	test_api.c
PROGRAM2_SRC = \
	interactive_test_api.c
PROGRAM3_SRC = \
	start_boot_jobs.c
PROGRAM5_SRC = \
	NtcssStartRemoteProcess.c
PROGRAM6_SRC = \
	NtcssStartRemoteProcessAsUser.c
PROGRAM8_SRC = \
	tapi.c

PROGRAM1_OBJ:= $(PROGRAM1_SRC:%.c=$(INRI_OBJ)/API_%.o)
PROGRAM3_OBJ:= $(PROGRAM3_SRC:%.c=$(INRI_OBJ)/API_%.o)
PROGRAM5_OBJ:= $(PROGRAM5_SRC:%.c=$(INRI_OBJ)/API_%.o)
PROGRAM8_OBJ:= $(PROGRAM8_SRC:%.c=$(INRI_OBJ)/API_%.o)

ARCHIVE1_OBJ:= $(ARCHIVE1_SRC:%.c=$(INRI_OBJ)/API_%_SL.o)
ARCHIVE6_OBJ:= $(ARCHIVE6_SRC:%.c=$(INRI_OBJ)/API_%.o)

PROGRAM1 = $(CPROGS)/test_api
PROGRAM2 = $(CPROGS)/interactive_test_api
PROGRAM3 = $(CPROGS)/start_boot_jobs
PROGRAM5 = $(CPROGS)/NtcssStartRemoteProcess
PROGRAM6 = $(CPROGS)/NtcssStartRemoteProcessAsUser
PROGRAM8 = $(CPROGS)/tapi

#Protect these targets from removal upon errors:
.PRECIOUS: $(ARCHIVE1) $(ARCHIVE1_SRC) $(PROGRAM1_SRC) $(PROGRAM2_SRC) \
		 $(PROGRAM3_SRC) $(PROGRAM5_SRC) $(ARCHIVE6_SRC) \
		 $(PROGRAM6_SRC) $(PROGRAM8_SRC)

all:	libs progs
libs:	$(ARCHIVE1) $(ARCHIVE6)
progs:	$(PROGRAM1) $(PROGRAM2) $(PROGRAM3) $(PROGRAM5) $(PROGRAM6) \
	$(PROGRAM8)


####### General rules ################################

$(PROGRAM1_OBJ): $(PROGRAM1_SRC)
	@echo '       Compiling $<'
	@$(CC) -c $(GCC_FLAGS) $(INCLUDES) $< -o $@

$(PROGRAM3_OBJ): $(PROGRAM3_SRC)
	@echo '       Compiling $<'
	@$(CC) -c $(GCC_FLAGS) $(INCLUDES) $< -o $@

$(PROGRAM5_OBJ): $(PROGRAM5_SRC)
	@echo '       Compiling $<'
	@$(CC) -c $(GCC_FLAGS) $(INCLUDES) $< -o $@

$(PROGRAM8_OBJ): $(PROGRAM8_SRC)
	@echo '       Compiling $<'
	@$(CC) -c $(GCC_FLAGS) $(INCLUDES) $< -o $@

$(ARCHIVE1_OBJ): $(INRI_OBJ)/API_%_SL.o: %.c
	@echo '       Compiling (shared) $< '
	@$(CC) -c -fpic $(GCC_FLAGS) $(INCLUDES) $< -o $@

$(ARCHIVE6_OBJ): $(INRI_OBJ)/API_%.o: %.c
	@echo '       Compiling (static) $< '
	@$(CC) -c $(GCC_FLAGS) $(INCLUDES) $< -o $@


####### Specific Rules ###############################

$(ARCHIVE1): $(ARCHIVE1_OBJ)
	@echo '       Building (shared) $@'
	@$(LD) -o $@ $(ARCHIVE1_OBJ) $(LD_EXTRA_LIBS) 

$(ARCHIVE6): $(ARCHIVE6_OBJ)
	@echo '       Building (static) $@'
	@$(AR) $(ARFLAGS) $@ $(?:.c=.o)

####################

$(PROGRAM1): $(PROGRAM1_OBJ) $(ARCHIVE6)
	@echo '       Linking $@'
	@$(CC) $(GCC_FLAGS) $(LDFLAGS) -o $@ $^ $(LDPATH) \
	$(USE_SHARED_LIB) $(APILIBS) $(USE_DEFAULT_LIB) \
	$(NTCSSLIBS) $(INILIBS) $(STRLIBS) $(SOCKLIBS)  $(SYSLIBS)

$(PROGRAM2): $(PROGRAM2_SRC)
	@echo '       Compiling/Linking $@'
	@$(CC) $(GCC_FLAGS) $(INCLUDES) $(LDFLAGS) -o $@ $^ $(LDPATH) \
	$(USE_ARCHIVED_LIB) $(APILIBS) $(USE_DEFAULT_LIB) $(NTCSSLIBS) \
	$(INILIBS) $(STRLIBS) $(SOCKLIBS) $(SYSLIBS)

$(PROGRAM3): $(PROGRAM3_OBJ) $(ARCHIVE6)
	@echo '       Linking $@'
	@$(CC) $(GCC_FLAGS) $(LDFLAGS) -o $@ $^ $(LDPATH) \
	$(USE_ARCHIVED_LIB) $(APILIBS) $(USE_DEFAULT_LIB) \
	$(NTCSSLIBS) $(INILIBS) $(STRLIBS) $(SOCKLIBS) \
	$(SYSLIBS)

$(PROGRAM5): $(PROGRAM5_OBJ) $(ARCHIVE6)
	@echo '       Linking $@'
	@$(CC) $(GCC_FLAGS) $(LDFLAGS) -o $@ $^ $(LDPATH) \
	$(USE_ARCHIVED_LIB) $(APILIBS) $(USE_DEFAULT_LIB) \
	$(NTCSSLIBS) $(INILIBS) $(STRLIBS) $(SOCKLIBS) $(SYSLIBS)

$(PROGRAM6): $(PROGRAM6_SRC)
	@echo '       Compiling/Linking $@'
	@$(CC) $(GCC_FLAGS) $(INCLUDES) $(LDFLAGS) -o $@ $^ $(LDPATH) \
	$(USE_ARCHIVED_LIB) $(APILIBS) $(USE_DEFAULT_LIB) \
	$(NTCSSLIBS) $(INILIBS) $(STRLIBS) $(SOCKLIBS) $(SYSLIBS)

$(PROGRAM8): $(PROGRAM8_OBJ) $(ARCHIVE6)
	@echo '       Linking $@'
	@$(CC) $(LDFLAGS) $(GCC_FLAGS) -o $@ $^ $(LDPATH) \
	$(USE_SHARED_LIB) $(APILIBS) $(USE_DEFAULT_LIB) \
	$(NTCSSLIBS) $(INILIBS) $(STRLIBS) $(SOCKLIBS) \
	$(SYSLIBS)

clean:
	@echo '       Removing .o files, programs and $(ARCHIVE1)'
	@$(RM) -f core *.o \
		$(ARCHIVE1_OBJ) $(ARCHIVE1) \
		$(ARCHIVE6_OBJ) $(ARCHIVE6) \
		$(PROGRAM1_OBJ) $(PROGRAM1) \
		$(PROGRAM2) \
		$(PROGRAM3_OBJ) $(PROGRAM3) \
		$(PROGRAM5_OBJ) $(PROGRAM5) \
		$(PROGRAM6) \
		$(PROGRAM8_OBJ) $(PROGRAM8)

