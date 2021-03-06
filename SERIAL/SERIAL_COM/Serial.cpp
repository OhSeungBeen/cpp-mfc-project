// Serial.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SERIAL_COM.h"
#include "Serial.h"
#include "SERIAL_COMDlg.h"

// CSerial

IMPLEMENT_DYNAMIC(CSerial, CCmdTarget)


CSerial::CSerial()
{

}

CSerial::~CSerial()
{
}


BEGIN_MESSAGE_MAP(CSerial, CCmdTarget)
END_MESSAGE_MAP()

// 컴포트 설정
void CSerial::SetComPort(CString port, DWORD baudRate, BYTE dataBit, BYTE stopBit, BYTE parity )
{
	m_port = port;
	m_baudRate = baudRate;
	m_dataBit = dataBit;
	m_stopBit = stopBit;
	m_parity = parity;
}

// 컴포트 정보 생성
BOOL CSerial::CreateComInfo()
{
	// offset: 파일 내부에서 시작할 파일 위치
	// offsethight: 파일 위치의 상위 단어(장치 파일일 경우 사용 그외에는 0)
	m_osWrite.Offset = 0;
	m_osWrite.OffsetHigh = 0;
	m_osRead.Offset = 0;
	m_osRead.OffsetHigh = 0;
	
	// 이벤트 설정
	m_osRead.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL ) ; 
	if (m_osRead.hEvent == NULL) 
	{
		return FALSE ; 
	}
	m_osWrite.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL ) ; 
	if (NULL == m_osWrite.hEvent) 
	{ 
		CloseHandle(m_osRead.hEvent) ; 
		return FALSE; 
	}
	
	return TRUE;
}

// 컴포트 열고 연결시도
BOOL CSerial::OpenComPort()
{
	// 개체에 대한 읽기/쓰기 접근을 지정한다.
	m_comHandle = CreateFile(m_port, 
		GENERIC_READ | GENERIC_WRITE, // 접근모드 (읽기/쓰기 설정)
		0, // 다른 프로그램과 공유 여부
		NULL, // 보안에 관한 속성
		OPEN_EXISTING, // 어떻게 열것인가
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // 파일 속성
		NULL); // 템플릿 파일 핸들러

	//컴포트에서 데이터를 교환하는 방법을 char 단위를 기본으로 설정 
	//하자. 
	SetCommMask( m_comHandle, EV_RXCHAR );
	// COM 포트 버퍼 크기 설정 (장치 내부 입력/출력 버퍼 byte단위)
	SetupComm(m_comHandle, 4096, 4096);
	//지정된 통신 자원의 입출력 버퍼의 모든 데이터를 삭제
	// 통신자원의 핸들,
	// 모든 미해결된 Overlapped 쓰기 작업을 종료하고 쓰기 작업이 완료가 되지 않더라도 즉시 반환
	// 모든 미해결된 Overlapped 읽기 작업을 종료하고 쓰기 작업이 완료가 되지 않더라도 즉시 반환
	// 출력 버퍼 클리어
	// 입력 버퍼 클리어 
	PurgeComm( m_comHandle, PURGE_TXABORT | PURGE_RXABORT | 
		PURGE_TXCLEAR | PURGE_RXCLEAR );

	COMMTIMEOUTS CommTimeOuts;
	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
	CommTimeOuts.ReadTotalTimeoutConstant = 1000;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
	CommTimeOuts.WriteTotalTimeoutConstant = 1000;
	SetCommTimeouts(m_comHandle, &CommTimeOuts );

	BOOL connectionState;
	DCB dcb;
	GetCommState(m_comHandle, &dcb);
	dcb.BaudRate = m_baudRate;
	dcb.ByteSize = m_dataBit;
	dcb.StopBits = m_stopBit;
	dcb.Parity = m_parity;
	connectionState = SetCommState(m_comHandle, &dcb);

	if(connectionState)
	{
		AfxBeginThread((AFX_THREADPROC)ComThread,(LPVOID)this); 
		AfxMessageBox("COM Port Open !");
		m_connected = TRUE;
	}
	else
	{
		m_connected = FALSE;
		CloseHandle(m_comHandle);
	}
	return connectionState;
}

// 컴포트 해제
BOOL CSerial::DestoryCom()
{
	if(m_connected)
	{
		SetCommMask(m_comHandle, 0);
		EscapeCommFunction(m_comHandle, CLRDTR);
		PurgeComm(m_comHandle, PURGE_TXABORT | PURGE_RXABORT |
			PURGE_TXCLEAR | PURGE_RXCLEAR);
		CloseHandle(m_comHandle);
		m_connected = FALSE;
	}

	CloseHandle(m_osRead.hEvent);
	CloseHandle(m_osWrite.hEvent);
	return TRUE;
}


DWORD ComThread(LPVOID lpData)
{
	DWORD dwEvtMask; 
	OVERLAPPED os; 
	CSerial* pSerial = (CSerial*)lpData; 
	char data[1024]; 
	int length; 

	//핸들에 아무런 컴포트가 안 붙어 있으면 에러 리턴 
	if (pSerial == NULL || !pSerial->IsKindOf( RUNTIME_CLASS(CSerial))) 
		return (DWORD)-1; 
	memset( &os, 0, sizeof(OVERLAPPED)); 
	os.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL);
	if (os.hEvent == NULL) 
	{ 
		AfxMessageBox("Failed to create event for thread! comm Error!"); 
		return FALSE; 
	} 
	if (!SetCommMask(pSerial->m_comHandle, EV_RXCHAR)) 
		return FALSE;
	while (pSerial->m_connected) 
	{ 
		dwEvtMask = 0; 
		WaitCommEvent(pSerial->m_comHandle, &dwEvtMask, NULL); 
		if ((dwEvtMask & EV_RXCHAR) == EV_RXCHAR) 
		{ 
			do 
			{ 
				memset(data,0,1024); 
				if (length = pSerial->ReadCom((LPSTR)data, 1024)) 
				{
					CString strCurr;
					CString strTemp;
					for(int i = 0; i < length; i++){
						char charData = data[i];

						CWnd *pWnd = AfxGetMainWnd();
						if(((CButton*)pWnd->GetDlgItem(IDC_RECV_FORMAT2))->GetCheck())
						{
							if(charData > 0x0f) strTemp.Format("0x%2x ", (charData));
							else strTemp.Format("0x0%1x ", (charData));
						}
						else{
							char buf[10];
							sprintf_s(buf, "%c", charData);
							strTemp = buf;
						}
						strCurr += strTemp;
					}
					//AfxMessageBox(strCurr);
					pSerial->SetReadData(strCurr);
				} 
			} 
			while ( length > 0 ) ; 
		} 
	} 
	CloseHandle( os.hEvent ) ; 
	return TRUE;
}


// 컴포트로부터 데이터 수신
int CSerial::ReadCom(LPSTR lpszData, int maxLength)
{
	BOOL readState;
	COMSTAT comStat;
	DWORD dwError;
	DWORD dwLength;

	ClearCommError(m_comHandle, &dwError, &comStat);
	dwLength = min((DWORD)maxLength, comStat.cbInQue);
	if(dwLength > 0)
	{
		readState = ReadFile(m_comHandle, lpszData, dwLength, &dwLength, &m_osRead);
		if(!readState)
		{
			AfxMessageBox("Read Error");
		}
	}
	return dwLength;
}

// 컴포트로부터 데이터 송신
BOOL CSerial::WriteCom(LPSTR lpByte, DWORD dwBytesToWrite)
{
	BOOL writeState;
	DWORD dwBytesWritten;
	writeState = WriteFile(m_comHandle, lpByte, dwBytesToWrite, &dwBytesWritten, &m_osWrite);
	
	if(writeState)
	{
		AfxMessageBox("Write Error");
	}
	return TRUE;
}

void CSerial::SetReadData(CString data)
{
	m_data = data;
	SendMessage(m_hWnd, WM_RECEIVEDATA, 0, 0);
}

void CSerial::SetHwnd(HWND hwnd)
{
	m_hWnd = hwnd;
}

// CSerial 메시지 처리기입니다.
