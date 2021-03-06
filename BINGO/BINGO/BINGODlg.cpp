
// CATCHMINDDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "BINGO.h"
#include "BINGODlg.h"
#include "FriendDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	//enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog()
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CCATCHMINDDlg 대화 상자

CBINGODlg::CBINGODlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBINGODlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBINGODlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CBINGODlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_PROFILE, &CBINGODlg::OnProfile)
	ON_COMMAND(ID_FRIEND_VIEW, &CBINGODlg::OnFriendView)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CCATCHMINDDlg 메시지 처리기

BOOL CBINGODlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.
	
	AfxOleInit();
	g_dataBase.Connect();

	m_loginDlg = new CLoginDlg;
	m_loginDlg->Create(CLoginDlg::IDD, this);
	m_loginDlg->CenterWindow();
	m_loginDlg->ShowWindow( SW_SHOW );

	m_watingRoomDlg = new CWatingRoomDlg(this);
	m_watingRoomDlg->Create(CWatingRoomDlg::IDD, this);
	m_watingRoomDlg->CenterWindow();
	m_watingRoomDlg->ShowWindow(SW_HIDE);

	m_gameRoomDlg = new CGameRoomDlg;
	m_gameRoomDlg->Create(CGameRoomDlg::IDD, this);
	m_gameRoomDlg->CenterWindow();
	m_gameRoomDlg->ShowWindow(SW_HIDE);
	
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CBINGODlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CBINGODlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CBINGODlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// 자식 다이얼로그 전환
void CBINGODlg::ChangeDlg(int preDlg, int newDlg)
{
	if(preDlg == LOGIN_ROOM)
		m_loginDlg->ShowWindow(SW_HIDE);
	else if(preDlg == WATING_ROOM)
		m_watingRoomDlg->ShowWindow(SW_HIDE);
	else if(preDlg == GAME_ROOM)
		m_gameRoomDlg->ShowWindow(SW_HIDE);
	
	if(newDlg == LOGIN_ROOM)
		m_loginDlg->ShowWindow(SW_SHOW);
	else if(newDlg == WATING_ROOM)
		m_watingRoomDlg->ShowWindow(SW_SHOW);
	else if(newDlg == GAME_ROOM)
		m_gameRoomDlg->ShowWindow(SW_SHOW);
}


void CBINGODlg::OnProfile()
{
	if(g_member.valid == FALSE)
		return;

	CProfileDlg profileDlg(g_member.nickName, g_member.id);
	profileDlg.DoModal();
}

void CBINGODlg::OnFriendView()
{
	if(g_member.valid == FALSE)
		return;

	CFriendDlg friendDlg;
	friendDlg.DoModal();
}

void CBINGODlg::OnClose()
{
	// delete memory
	delete m_loginDlg;
	delete m_watingRoomDlg;
	delete m_gameRoomDlg;
	CDialog::OnClose();
}

