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
# Makefile for NTCSSII Main

# Delete the default suffixes for speed/efficiency
.SUFFIXES:

all: libs_all bcst_server_all install_all \
	ntcss_lp_all ntcss_netcopy_all ntcss_server_all ntlpd_all \
	apis_all release_req_all \
	unix_ini_all \
	dist_mgr_all ntlppd_all print_ntcss_system_setting_all \
	ntcss_nis_programs_all progd_all tools_all java_all print_que_all \
	fs_archive_all
clean: libs_clean bcst_server_clean \
	install_clean \
	ntcss_lp_clean ntcss_netcopy_clean ntcss_server_clean ntlpd_clean \
	apis_clean release_req_clean \
	unix_ini_clean \
	dist_mgr_clean ntlppd_clean \
	print_ntcss_system_setting_clean ntcss_nis_programs_clean progd_clean \
	tools_clean java_clean print_que_clean fs_archive_clean

libs_all:
	@cd LIBS/SRC; if [ -f Makefile ]; then $(MAKE) -f Makefile all; fi
libs_clean:
	@cd LIBS/SRC; if [ -f Makefile ]; then $(MAKE) -f Makefile clean; fi

bcst_server_all:
	@cd BCST_SERVER; if [ -f Makefile ]; then $(MAKE) -f Makefile all; fi
bcst_server_clean:
	@cd BCST_SERVER; if [ -f Makefile ]; then $(MAKE) -f Makefile clean; fi

install_all:
	@cd INSTALL; if [ -f Makefile ]; then $(MAKE) -f Makefile all; fi
install_clean:
	@cd INSTALL; if [ -f Makefile ]; then $(MAKE) -f Makefile clean; fi

ntcss_lp_all:
	@cd NTCSS_LP; if [ -f Makefile ]; then $(MAKE) -f Makefile all; fi
ntcss_lp_clean:
	@cd NTCSS_LP; if [ -f Makefile ]; then $(MAKE) -f Makefile clean; fi

ntcss_netcopy_all:
	@cd NTCSS_NETCOPY; if [ -f Makefile ]; then $(MAKE) -f Makefile all; fi
ntcss_netcopy_clean:
	@cd NTCSS_NETCOPY; if [ -f Makefile ]; then $(MAKE) -f Makefile clean; fi

ntcss_server_all:
	@cd NTCSS_SERVER; if [ -f Makefile ]; then $(MAKE) -f Makefile all; fi
ntcss_server_clean:
	@cd NTCSS_SERVER; if [ -f Makefile ]; then $(MAKE) -f Makefile clean; fi

ntlpd_all:
	@cd NTLPD; if [ -f Makefile ]; then $(MAKE) -f Makefile all; fi
ntlpd_clean:
	@cd NTLPD; if [ -f Makefile ]; then $(MAKE) -f Makefile clean; fi

apis_all:
	@cd APIS; if [ -f Makefile ]; then $(MAKE) -f Makefile all; fi
apis_clean:
	@cd APIS; if [ -f Makefile ]; then $(MAKE) -f Makefile clean; fi

release_req_all:
	@cd RELEASE_REQ; if [ -f Makefile ]; then $(MAKE) -f Makefile all; fi
release_req_clean:
	@cd RELEASE_REQ; if [ -f Makefile ]; then $(MAKE) -f Makefile clean; fi

srv_dev_all:
	@cd SRV_DEV; if [ -f Makefile ]; then $(MAKE) -f Makefile all; fi
srv_dev_clean:
	@cd SRV_DEV; if [ -f Makefile ]; then $(MAKE) -f Makefile clean; fi

tools_all:
	@cd TOOLS; if [ -f Makefile ]; then $(MAKE) -f Makefile all; fi
tools_clean:
	@cd TOOLS; if [ -f Makefile ]; then $(MAKE) -f Makefile clean; fi

unix_ini_all:
	@cd UNIX_INI; if [ -f Makefile ]; then $(MAKE) -f Makefile all; fi
unix_ini_clean:
	@cd UNIX_INI; if [ -f Makefile ]; then $(MAKE) -f Makefile clean; fi

dist_mgr_all:
	@cd DIST_MGR; if [ -f Makefile ]; then $(MAKE) -f Makefile all; fi
dist_mgr_clean:
	@cd DIST_MGR; if [ -f Makefile ]; then $(MAKE) -f Makefile clean; fi

ntlppd_all:
	@cd NTLPPD; if [ -f Makefile ]; then $(MAKE) -f Makefile all; fi
ntlppd_clean:
	@cd NTLPPD; if [ -f Makefile ]; then $(MAKE) -f Makefile clean; fi

print_ntcss_system_setting_all:
	@cd PRINT_NTCSS_SYSTEM_SETTING; if [ -f Makefile ]; then $(MAKE) -f Makefile all; fi
print_ntcss_system_setting_clean:
	@cd PRINT_NTCSS_SYSTEM_SETTING; if [ -f Makefile ]; then $(MAKE) -f Makefile clean; fi

fs_archive_all:
	@cd FS_ARCHIVE; if [ -f Makefile ]; then $(MAKE) -f Makefile all; fi
fs_archive_clean:
	@cd FS_ARCHIVE; if [ -f Makefile ]; then $(MAKE) -f Makefile clean; fi

print_que_all:
	@cd PRINT_QUE; if [ -f Makefile ]; then $(MAKE) -f Makefile all; fi
print_que_clean:
	@cd PRINT_QUE; if [ -f Makefile ]; then $(MAKE) -f Makefile clean; fi

ntcss_nis_programs_all:
	@cd NTCSS_NIS_PROGRAMS; if [ -f Makefile ]; then $(MAKE) -f Makefile all; fi
ntcss_nis_programs_clean:
	@cd NTCSS_NIS_PROGRAMS; if [ -f Makefile ]; then $(MAKE) -f Makefile clean; fi

progd_all:
	@cd PROGD; if [ -f Makefile ]; then $(MAKE) -f Makefile all; fi
progd_clean:
	@cd PROGD; if [ -f Makefile ]; then $(MAKE) -f Makefile clean; fi

java_all:
	@cd NTCSS_JAVA; if [ -f Makefile ]; then $(MAKE) -f Makefile all; fi
java_clean:
	@cd NTCSS_JAVA; if [ -f Makefile ]; then $(MAKE) -f Makefile clean; fi

