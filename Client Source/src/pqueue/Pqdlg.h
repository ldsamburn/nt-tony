/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// pqdlg.h : header file
//
#include "cwinsock.hpp" 
#include "ntcsss.h"
#include "pqlistctrl.h"

#define MAX_RECV_BUFFER 10000
#define TMP_BUFFER_SIZE 200 
#define MAX_PRINTER_LIST_SIZE 1000 
#define NUM_TAB_STOPS 10   
#define MAX_SERVER_NAME   50

#define WM_UPDATELIST WM_USER+100 
// Sockets defines
#define DESKTOP_PORT	9121
#define PRINTQ_PORT		9122
#define DB_PORT 		9132



   
/////////////////////////////////////////////////////////////////////////////
// CPqdlg dialog

class CPqdlg : public CDialog
{
// Construction
public:
	CPqdlg(CWnd* pParent = NULL);	// standard constructor
    PopulateApps(void);
    PopulatePrts(void); 
    PopulateQueue(void); 
    void UpdateRequestList(char far *outbuf);
	BOOL RecvDataBuffer(CWinsock *sptr,char *buffer,int *len);
	void EnableDisableButtons(BOOL nEDInd);
	void EnableDisableSelect(BOOL nEDInd);
	char *ConvertFromNTCSSDate(char *srcbuf, char *dstbuf);
	
    CWinsock FAR *m_cmdsock;
    CWinsock FAR *m_spoolsock;
    CWinsock FAR *m_svrsock;
    SOCKET m_sock;
    SOCKET m_sock1;
    int m_appcnt;
    int m_prtcnt;
    int m_reqcnt;
    NtcssUserInfo ui;
    char NtcssServerName[MAX_SERVER_NAME];
    
    struct APP_LIST {
    	char progroup_title[SIZE_PROGROUP_TITLE];
    	char progroup_host[SIZE_HOST_NAME];
    } FAR *m_app_list;
    
    struct PRINTER_LIST_OLD {
    	char prt_name[SIZE_PRT_NAME];
    	int prt_namelen;
    	char hostname[SIZE_HOST_NAME];
    	char location[SIZE_PRT_LOCATION];
    	char device_name[SIZE_PRT_DEV_NAME];
    	int que_enabled;
    	int printer_enabled;
    	char redirect_printer[SIZE_PRT_RED];
		char driver_file[SIZE_PRT_FILE_NAME];
		char ui_file[SIZE_PRT_FILE_NAME];
		char data_file[SIZE_PRT_FILE_NAME];
		char help_file[SIZE_PRT_FILE_NAME];
		char monitor_file[SIZE_PRT_FILE_NAME];
		char monitor_name[NTCSS_SIZE_SERVER_LOCATION];
		char filename[SIZE_PRT_FILE_NAME];
    } FAR *m_prt_list1;

	NtcssPrinterList *m_prt_list;
    
    struct PRINTER_REQUESTS {
    	char request_id[SIZE_PRT_REQ_ID];
    	char progroup_title[SIZE_PROGROUP_TITLE];
    	char prt_file[SIZE_PRT_FILE_NAME];
    	char login_name[SIZE_LOGIN_NAME];
    	char prt_name[SIZE_PRT_NAME];
    	char time_of_request[SIZE_PRT_REQ_TIME];
    	int security_class;
    	int size;
    	char orig_host[SIZE_ORIG_HOST];
    	int status;
//		char filename[SIZE_PRT_FILE_NAME];
    } FAR *m_prt_requests;

    struct GET_APP_LIST_MSG {
    	char message_name[SIZE_MESSAGE_TYPE];
    	char login_name[SIZE_LOGIN_NAME];
    	char token[SIZE_TOKEN];
		char terminator;
    } m_get_app_msg;

    
    struct PRINTER_LIST_MSG {
        char message_name[SIZE_MESSAGE_TYPE];
    	char login_name[SIZE_LOGIN_NAME];
    	char token[SIZE_TOKEN];
    	char progroup_title[SIZE_PROGROUP_TITLE];
    	char printer_access;
		char terminator;
    } m_printer_list_msg;
    
    struct PRINT_QUEUE_MSG {
    	char message_name[SIZE_MESSAGE_TYPE];
    	char login_name[SIZE_LOGIN_NAME];
    	char token[SIZE_TOKEN];
    	char prt_name[SIZE_PRT_NAME];
    	char progroup_title[SIZE_PROGROUP_TITLE];
    	char role;
		char terminator;
    } m_printer_queue_msg; 
    
    struct DELETE_REQUEST_MSG {
    	char message_name[SIZE_MESSAGE_TYPE];
    	char login_name[SIZE_LOGIN_NAME];
    	char token[SIZE_TOKEN];
    	char prt_name[SIZE_PRT_NAME];
        char request_id[SIZE_PRT_REQ_ID];
		char terminator;
    } m_delete_request_msg;
    
    struct INPUT_QUEUE_LIST {
		char buffer[MAX_RECV_BUFFER];
		int buffer_length;
		struct INPUT_QUEUE_LIST far *next_buffer;
	};    	
	struct INPUT_QUEUE_LIST far *head_list,*current_ptr;
    
    int m_TabStops[NUM_TAB_STOPS];
    WORD m_DlgWidthUnits;


///////////////////////////////
typedef struct {
		char message_name[SIZE_MESSAGE_TYPE];
		char login_name[SIZE_LOGIN_NAME];
		char token[SIZE_TOKEN];
		char filename[NTCSS_SIZE_SERVER_LOCATION];
		char first_line[8];
		char last_line[8];
		char terminator;
} SPC_TEST_MSG;
#define TFILE "/usr/local/NTCSSII/logs/printcap.ini"

	///////////////////////////

    
// Dialog Data
	//{{AFX_DATA(CPqdlg)
	enum { IDD = IDD_PQUEUE };
	CPQListCtrl	m_PQListCtrl;
	CString	m_PrinterListEntry;
	CString	m_AppListEntry;
	CString	m_RequestListEntry;
	//}}AFX_DATA

private:
	static void CALLBACK NetEventHandler(WPARAM wParam, LPARAM lParam);
	static CPqdlg *m_dlgptr;
	//static CWinsock FAR *m_cmdsock;

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// Generated message map functions
	//{{AFX_MSG(CPqdlg)
	virtual void OnOK();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCombo1();
	virtual void OnCancel();
	afx_msg void OnSelchangeList1();
	afx_msg void OnSelchangeCombo2();
	afx_msg void OnCdelete();
	afx_msg LONG OnUpdateList(WPARAM wParam, LPARAM lParam);
	afx_msg void OnRelease();
	afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnChelp();
	afx_msg void OnClickListview(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
