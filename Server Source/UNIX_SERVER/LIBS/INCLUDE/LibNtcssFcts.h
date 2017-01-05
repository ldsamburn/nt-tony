
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/* LibNtcssFcts.h  */

/* This file should only contain prototypes and other info for the .c files in
   the ntcsslib directory!
*/

#ifndef _LibNtcssFcts_h
#define _LibNtcssFcts_h


/** INRI INCLUDES **/


/** PROTOS **/


/* This was needed when we started using c++ compilations to prevent "name
	decorating" in the .o files from alienating the standard C style naming
	conventions.
*/
#ifdef __cplusplus
 extern "C"{
#endif

/** PrtFcts.c **/
int          changePrtStatus(const char*, const char*, const char*, int, int);
int          checkAdapterLocation(char*);
int          checkEthernetAddress(char*);
int          checkHostName(char*);
int          checkIpAddress(char*);
int          checkMaxSize(char*);
int          checkOutputTypeDescription(char*);
int          checkOutputTypeTitle(char*);
int          checkPrtClassTitle(char*);
int          checkPrtDeviceName(char*);
int          checkPrtDriverName(char*);
int          checkPrtLocation(char*);
int          checkPrtName(char*);
void         disablePrtStatus(int*);
void         disableQueueStatus(int*);
void         enablePrtStatus(int*);
void         enableQueueStatus(int*);
int          getAppHostname(const char*, char*, const char*);
int          getAppPrtLockId(const char*, const char*);
int          getDefPrtOtype(const char*, const char*, char*, char*,
                            const char*);
int          getDirListing(const char*, char*, char*, int);
const char  *getNoYesValue(int);
int          getNoYesValueNum( const char*);
const char  *getPrtAccess(int);
int          getPrtAccessNum(const char*);
int          getPrtList(const char*, int, char*, const char*);
int          getPrtRedirectionStatus(const char*, char*);
const char  *getPrtStatus(int);
const char  *getPrtSuspendState(int);
int          getPrtSuspendStateNum(const char*);
const char  *getQueueStatus(int);
int          getUserPrtList(const char*, const char*, const char*, int, int,
                            int, char*, const char*);
int          IsNotTextFile( const char * );
int          isPrtAccessGeneral(int);
int          isPrtEnabled(int);
int          isQueueEnabled(int);
void         makePrtAccessByapp(int*);
void         makePrtAccessGeneral(int*);
int          returnPrtStatus(const char*, const char*, const char*);
int          sendPrinterAdminToDm(const char*, const char*, char*);
int          validatePrt(const char*, const char*, const char*);


/** NetSvrInfo.c **/
#ifdef __cplusplus
 };
#endif

#include <NetSvrInfo.h>

#ifdef __cplusplus
 extern "C"{
#endif


/** UserFcts.c **/
int    addCurrentUsersRecord(const char*, const char*, const char*,
                             const char*, const char*, const char*, int);
int    changeAppLock(const char*, int, const char*);
int    changeAppuserInfo(const char*, const char*, const char*, const char*);
int    changeUserLoginLock(const char*, int, const char*);
int    changeUserPassword(const char*, const char*, const char*, const char*);
int    checkLoginName(char*);
int    checkNtcssUserPassword(char*, const char*, char*, const char*,
                              int);
int    checkRealName(char*, char*, char*, char*);
int    checkPhoneNumber(char*);
int    checkSSNumber(char*);
int    CheckSSN(const char* ssn);
int    checkUserPassword(char*);
int    clearAppLogins(const char*, const char*);
int    createDmPipeSemaphore(void);
void   freeCurrentUsersRecord(CurrentUsersItem*);
void   freeUserProgroupAndRoleInfo(AppuserProgroupRoleItem**, int);
int    getappdata(const char*, const char*, char*, const char*);
int    getAppPassword(char*, const char*, const char*, char*, const char*);
int    getAppUserData(const char*, const char*, char*, const char*);
int    getAppUserInfo(const char*, const char*, char*, const char*);
int    getAppUserList(const char*, const char*, char*, const char*);
int    getAppUserRoleMask(AppuserItem**, const char*, int, const char*,
                          unsigned int*, int*);
int    GetUserList(char *);
int    GetUserInfoAndRoles(const char*,const char*);
const char  *getClassification(int);
int    getClassificationNum(const char*);
int    getCurrentUserList(const char*, const char*);
int    getCurrentUsersRecord(const char*, CurrentUsersItem*, int*,
                             const char*);
int    getDmPid(void);
int    setDmPid(int);
int    getDmResourceMonitorPid(void);
int    setDmResourceMonitorPid(int);
/*int    getNextLoggedIn(char*, char*, char*);*/
AppuserProgroupRoleItem  **getUserProgroupAndRoleInfoForLogin(const char*,
                                                              int*,
                                                              const char*);
char  *getUserRole(int);
int    getUserRoleNum(const char*);  /* deprecated */
const char  *getUnixStatus(int);
int    getUnixStatusNum(const char*);
/*int    getUserLoginInfo(char*, char*, char*, char*, char*, char*, int*);*/
int    getUsersAccessRole(const char*, const char*); /* deprecated */
int    getUserSharedPassword(const char*, char*);
int    howManyUsersLoggedIn(const char*);
int    isUserLoggedIn(const char*, char*, const char*);
int    isUserPartOfProgroup(const char*, const char*);
int    isUserValid(const char*, const char*, const char*, int*);
int    isValidNtcssUser(const char*, const char*);
int    MakeAppRoleMask( AccessRoleItem**, int, char *, const char*, int* );
int    makeLoginReservation(const char*, const char*, const char*, const char*,
                            int);
int    MakeUserAccessRoleString( AccessRoleItem**, int, const char*, int,
                                 char*);
int    MakeUserAdminDataString(const char*, char*, char*, int*);
int    MakeUserClearanceString(char*, int);
int    old_changeUserPassword(const char*, const char*, char*,
                              const char*); /* deprecated */
int    old_checkNtcssUserPassword(const char*, const char*, char*,
                                  const char*); /* deprecated */
int    recordLoginState(const char*, int, const char*, const char*,
                        const char*, int, const char*, char*);
int    registerAppUser(const char*, const char*, int, const char*);
int    releaseDmPipe(void);
int    removeCurrentUsersRecord(const char*, const char*);
int    removeDmPipeSemaphore(void);
int    removeLoginReservation(const char*, const char*);
int    requestAndWaitForDmPipe(void);
int    sendUserAdminToDm(const char*, const char*, char*);
int    signalDmResourceMonitorStartWatch(void);
int    signalDmResourceMonitorStopWatch(void);
int    verifyUnixUserInfo(const char*, const char*);


/** ManipData.c **/
/* Several of these functions assume that the proper database locks are
   engaged when reading or writing to the needed DB files. */
CurrentUsersItem** getCurrentUsersData(const char*, int*);
void               writeCurrentUsersData(CurrentUsersItem**, int, FILE*);
void               freeCurrentUsersData(CurrentUsersItem**, int);
int    encryptDecryptTurnedOn(void);
FILE*  decryptAndOpen(const char*, char*, const char*);
int    closeAndEncrypt(FILE*, const char*);
void  doClearBit(int*, int);
void  doSetBit(int*, int);
int   isBitSet(int, int);
void  convFromSql(DBRecord*, int);
void  convToSql(DBRecord*, int);
int  getNtcssSysConfData(const char*, char*, const char*);
int  putNtcssSysConfData(const char*, char*, const char*);
const char  *getCurrentAppsTableName(void);
const char *getCurrentUsersTableName(void);
int   getCurrentUsersNumRecs(void);
const char *getLoginHistoryTableName(void);
int   getLoginHistoryNumRecs(void);
UserItem **getUserData(int*, const char*);
void       writeUserData(UserItem**, int, FILE*);
void       freeUserData(UserItem**, int);
AppItem  **getAppData(int*, const char*, int);
void       writeAppData(AppItem**, int, FILE*);
void       freeAppData(AppItem**, int);
ProgramItem  **getProgramData(int*, const char*, int);
void           freeProgramData(ProgramItem**, int);
AppuserItem  **getAppuserData(int*, const char*, int);
void           writeAppuserData(AppuserItem**, int, FILE*);
void           freeAppuserData(AppuserItem**, int);
AccessRoleItem  **getAccessRoleData(int*, const char*);
void              freeAccessRoleData(AccessRoleItem**, int);
PrtItem  **getPrtData(int*, const char*, AppItem**, int);
PrtItem  **getPrtDataFromDb(int*, const char*, AppItem**, int);
void       writePrtData(PrtItem**, int, FILE*);
void       freePrtData(PrtItem**, int);
PrtItem  **getAppPrtData(int*, const char*, const char*);
void       freeAppPrtData(PrtItem**, int);
AdapterItem  **getAdapterData(int*, const char*);
AdapterItem  **getAdapterDataFromDb(int*, const char*);
void           freeAdapterData(AdapterItem**, int);
PrtclassItem  **getPrtclassData(int*, const char*);
void            freePrtclassData(PrtclassItem**, int);
AdaptypeItem  **getAdaptypeData(int*, const char*);
void            freeAdaptypeData(AdaptypeItem**, int);
OutputTypeItem  **getOutputTypeData(int*, const char*);
void              freeOutputTypeData(OutputTypeItem**, int);
OutputTypeItem  **getAppOutputTypeData(int*, const char*, const char*,
                                       PrtItem**, int);
void              freeAppOutputTypeData(OutputTypeItem**, int);
PredefinedJobItem  **getPredefinedJobData(int*, const char*);
void                 freePredefinedJobData(PredefinedJobItem**, int);
PredefinedJobItem  **getAppPredefinedJobData(int*, const char*, const char*);
void                 freeAppPredefinedJobData(PredefinedJobItem**, int);
PredefinedJobItem  **getAppPredefinedJobDataByRole(int*, const char*,
                                                   const char*, int);
void                 freeAppPredefinedJobDataByRole(PredefinedJobItem**, int);
PredefinedJobItem  **getServerBootJobs(int*, const char*, const char*);
void                 freeServerBootJobs(PredefinedJobItem**, int);
HostItem  **getHostData(int*, const char*);
void        writeHostData(HostItem**, int, FILE*);
void        freeHostData(HostItem**, int);
AppItem **getCurrentAppData(int*, const char*);
void  writeCurrentAppData(AppItem**, int, FILE*);
void  freeCurrentAppData(AppItem**, int);
int   clearCurrentAppsTable( const char * );
void  incrementCurrentAppDataCount( const char*);
void  decrementCurrentAppDataCount(const char*);
void  incrementCurrentAppsFromList(const char*,const char*);
void  decrementCurrentAppsFromList(const char*,const char*);
DeviceDataItem  **getDeviceData(int*,  const char*);
void  freeDeviceData(DeviceDataItem**, int);

int   addProgroupUseInfo( ProgroupUseStruct*, const char*, int, int );
int   AddUserToUnixGroup(const char*, const char*);
int   applyDatabaseChanges(const char*, const char*, int);
int   CalculateServerUserLoads( UserItem**, int, userLoadStruct*, int* );
int   changeAppUserArrayRole(const char*, const char*, int,AppuserItem**, int);
int   ChangeUnixPassword(const char*, const char*);
int   CreateUnixAccount(const char*,const char*,const char*,const char*,
			const char*);
int   CreateUnixBatchUserAccount(const char*,const char*,const char*,
				 const char*,const char*);
int   copy_database_files(const char*, int);
int   delAppUser(DBRecord*, const char*);
int   delNtcssUser(DBRecord*, const char*);
int   delNtcssUserByLoginName(const char*, int);
int   doesProgramGroupExist(const char*, const char*);
int   editNtcssProgram(DBRecord*, const char*);
int   editNtcssUser(DBRecord*, const char*, const char*);
int   EditUnixGecosField(const char*, const char*);
int   ensure_database_integrity(const char*);
int   getBatchUser(const char*, char*, const char*);            
const char  *getErrorMessage(int);        
int   getHostType(char*, const char*);
void  get_progroup_titles_for_local_host_and_app_servers(const char*,
							 char**, int*);
int   getRecord(DBRecord*, FILE*, int);
int   getUnixGroupForProgroup(const char*, const char*, char*);
int   getUserDataByLoginName( UserItem*[], const char*, const char* );
int   IAmMaster( void );
int   InitializeCurrentAppsDB(const char*);
int   InitializeRoleMap(int[MAX_NUM_PROGROUPS]
	                   [2 * MAX_NUM_APP_ROLES_PER_PROGROUP][3], AppItem*[],
                        int, const char*);
int   initProgroupUseStruct( ProgroupUseStruct*, int );
int   IsAppLocked(const char*);
int   isAppOnThisHost(const char*, AppItem**, int);
int   IsAppRoleLockOverride(const char*, int, AccessRoleItem**, int);
void  manipStripNewLine(char*);       
int   modAppUsers(DBRecord*, const char*);
int   ModifyNTCSSAdminProgArgs(const char*);
int   modProgramGroup(DBRecord*, const char*);         
void  nullifyDomain( char* );
int   RemoveApplessAppusers(const char*,const char*,AppItem*[],int,
			    AppuserItem*[],int,int);
int   RemoveApplessPrtAccessRecs(AppItem*[], int, const char*);
int   RemoveClasslessPrinters(PrtItem*[], int, const char*);
int   RemovePrinterlessPrtAccessRecs(PrtItem*[], int, const char*);
int   RemovePunct( char * );
int   RemoveUnixAccount(const char*);
int   RemoveUserFromUnixGroup(const char*, const char*); 
int   RemoveUserlessAppuserRecs(const char*);
int   ResolveAppUserAccessRoleNumShifts(const char*,const char*,
                                        int[MAX_NUM_PROGROUPS]
                                        [2 * MAX_NUM_APP_ROLES_PER_PROGROUP][3],
					AppItem*[],int,AppuserItem*[],int,int);
int   timestampDatabase( const char*);
int   updateAppUserName( const char*, const char*, const char* );
int   UpdateOutputPrintersDB(PrtItem*[], int, OutputTypeItem*[],
                                   int, AppItem*[], int, const char*);
int   UpdateOutputTypesDB(PrtItem*[], int, const char*);
int   UpdatePDJs(int[MAX_NUM_PROGROUPS][2 * MAX_NUM_APP_ROLES_PER_PROGROUP][3],
                 AppItem*[], int, const char*);
const char  *getAccessRolesTableName( void );
const char  *getCurrentUsersTableName( void );
const char  *getLoginHistoryTableName( void );



/** ProgFcts.c **/
int    checkUnixGroup(char*, char*);
const char  *getProgramType(int);
int    checkProgramType(char*, char*);
char  *getProgramAccess(int, const char*, AppItem**, int);
int    getAppInfo(char*, const char*);
int    checkProgroupTitle(char*, char*);
int    checkHost(const char*, const char*);
int    checkIconFile(char*, char*);
int    checkIconIndex(char*, char*);
int    checkVersionNumber(char*, char*);
int    checkClientLocation(char*, char*);
int    checkServerLocation(char*, char*);
int    checkProgramTitle(char*, char*);
int    checkProgramFile(char*, char*);
int    checkCmdLineArgs(char*, char*);
int    checkClassification(char*, char*);
int    checkProgramAccess(int, char*, char*);
int    checkNoYes(char*, char*);
int    checkPredefinedJobClass(char*, char*);
int    checkWorkingDir(char*, char*);
int    updateAdminProcs(const char*, int, const char*);
int    checkAdminProc(const char*, int, const char*);
int    getAppProcessLimit(const char*, const char*);
int    getHostProcessLimit(const char*, const char*);
int    setProcessLimit(const char*, int, int, const char*); /* deprecated */
int    checkCommonDbTag(char*);
int    checkCommonDbItem(char*);
int    getChildPid(int, const char*, const char*);
int    checkPredefinedJobTitle(char*, char*);
int    checkCommandLine(char*, char*);
int    getAppLinkData(const char*, char*, const char*);
int    setAppLinkData(const char*, const char*, const char*, int);
int    getVersionString(char*);
int    GetProgGroupList(char* query);
int    checkFileExistenceAndStats(const char*, const char*, mode_t);
int    checkPathExistence(const char*);
int    unpack_master_files(void);
int    setAppProcessLimit(const char*, int, const char*);
int    setHostProcessLimit(const char*, int, const char*); 

/** LockDB.c **/
int   createDBLock(int);
int   setReadDBLock(int, int);
void  unsetReadDBLock(int);
int   setWriteDBLock(int, int);
void  unsetWriteDBLock(int);
void  removeDBLock(int);
int   createLock(int);
int   setLock(int, int);
void  unsetLock(int);
void  removeLock(int);
int   setFileLock(char*, int*, int);
int   unsetFileLock(int);

/** FileFcts.c **/
int  is_file_older_than(long, const char*, const char*);

/** HashDB.c **/
int    initLocalDBFiles(const char*);
int    addPrtRequest(const char*, PrtRequestItem*);
int    delPrtRequest(const char*, const char*, char*, char*);
int    checkPrtRequestSize(const char*, PrtRequestItem*);
int    getPrtRequestQueueList(const char*, const char*, const char*, int,
                              const char*, char*, const char*);
int    getNextServerProcess(const char*, char*, char*, int, int, int);
int    okLaunchServerProcess(const char*, int, int, const char*);
char  *getServerProcessApps(const char*);
int    getScheduledServerProcs(const char*, const char*, const char*);
int    getServerProcessData(const char*, const char*, ServerProcessItem*, int);
int    addServerProcess(const char*, ServerProcessItem*, int);
int    delServerProcess(const char*, const char*, const char*, int, int);
int    changeServerProcess(const char*, const char*, int, int, int, int,
                           const char*, const char*, const char*, const char*,
                           char*, int, int);
int    getServerProcessQueueList(const char*, const char*, int, char*, char*,
                                 const char*, int, int, ProcessFilterData*);
int    getServerProcessId(int, const char*, const char*, const char*);
int    killActiveServerProcesses(const char*, int*, int*, const char*, int);
int    validateServerProcessId(const char*, const char*);
char  *getRunningServerProcessApps(const char*);


/** MsgFcts.c **/
int   changeBulBrd(const char*, const char*, const char*, int);
void  cleanUpOldBulBrdMsgs(int, const char*, const char*);
int   createMsgFile(const char*, const char*, const char*);
int   getAllBulBrds(const char*, const char*, char*, const char*);
void  getBulBrdMsgPath(const char*, char*, const char*);
int   getBulBrdMsgs(const char*, const char*, char*, char*, const char*);
int   getBulBrdType(const char*);
int   getOwnBulBrdMsgs(const char*, char*, char*, const char*);
int   getSubscribedBulBrds(const char*, char*, char*, const char*);
int   getUnsubscribedBulBrds(const char*, char*, char*, const char*);
int   isUserSubscribed(const char*, const char*, const char*);
int   markMsgRead(const char*, const char*, const char*);
void  returnDbFilename(int, int, char*, const char * );
int   returnLockId(int, int );
int   sendBulBrdMsg(const char*, const char*, const char*);
int   sendSystemBulBrdMsg(const char*, const char*, int, const char*,
                          const char*, char*, int*);
int   subscribeBulBrd(const char*, const char*, int, const char*);


/** lp_ops.c  definened in lp_ops.h **/
/*
  int  lp_accept(char*);
  int  lp_disable(char*);
  int  lp_enable(char*);
  int  lp_isaccept(char*);
  int  lp_isenable(char*);
  int  lp_reject(char*);
*/

/** NtcssNisFcts.c **/
int   buildAndPushNtcssNisMaps(void);
int   buildNtcssNisMaps(void);
int   buildAndPushSystemNisMaps(void);
AdapterItem  **getAdapterDataFromNis(int*);
HostItem     **getHostDataFromNis(int*);
int   GetNISmap(const char*, const char*);
int   getNtPrinterDriverNameFromNis(char*, char*);
int   getNtcssAuthServerByLoginNameFromNis(char*, char*);
/*   NOTE: These 2 functions will replace the current HostData functions */
int   getNtcssHostItemByIpFromNis(HostItem*, char*);
int   getNtcssHostItemByNameFromNis(HostItem*, char*);
int   getNtcssPasswordByLoginNameFromNis( char*, char* );
int   getNtcssSSNByLoginNameFromNis( char*, char* );
int   getPrinterAccessListStringFromNis(char*, char*);
PrtItem  **getPrtDataFromNis(int*, AppItem**, int);
int   getNtcssSysConfValueFromNis(char**, const char*);
int   NISremoveTimeFiles( void );
int   lockDBForNISMake(void);


/* NtcssSystemSettingsFcts.c */
int   freeNtcssSystemSettingsShm(void);
const char *getNtcssSystemSettingTempPtr(const char*, const char*);
int   loadNtcssSystemSettingsIntoShm(void);
int   printNtcssSystemSettingsFromShm(int);
int   SystemSettingsAreInitialized(void);

/** printableOutputCrypt.c **/
int  plainDecryptString( const char*, const char*, char**, int, int );
int  plainEncryptString( const char*, const char*, char**, int, int );


/** BundleData.c **/
int  bundleData(char*, const char*, ...);
int  bundleQuery(char*, const char*, ...);
int  checkDataLengths(const char*, const char*, ...);
int  getOverallLength(const char*, ...);
int  unbundleData(char*, const char*, ...);


/** Replication.c **/
int   BuildMasterDBbackupFiles(void);
int   copyMasterBackupFiles(void);
int   sendMasterFilesToReplServers(const char*);
int   VerifyCopiedMasterBackupFile( void );


/** NetUserInfo.c **/
int  IsUserAtIP(const char*,const char*,int);

/****NEW BY ACA/JJ ****/
AppuserItem  **getAppuserDataByName(const char *, int  *, const char *, int);
AppuserItem  **getAppuserDataByApp(const char *, int  *, const char *, int);
UserItem  **getUserDataByName(const char *, int *, const char *);



#ifdef __cplusplus
 };
#endif /* __cplusplus */

#endif /* _LibNtcssFcts_h */

