
// SERVERDlg.h : 헤더 파일
//

#pragma once
#include "ListenSocket.h"
#include "afxwin.h"
#include "afxcmn.h"

// CSERVERDlg 대화 상자
class CSERVERDlg : public CDialog
{
// 생성입니다.
public:
	CSERVERDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CListenSocket serverSocket;

	CListBox ctrl_listLog;
	CIPAddressCtrl ctrl_ipAdress;
	CEdit ctrl_editPort;

	void AddLogString(CString str);
	
	afx_msg void OnBnClickedBtnOppen();
	afx_msg void OnBnClickedBtnLogSave();
	afx_msg void OnBnClickedBtnSend();
};
