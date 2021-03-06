#include "StdAfx.h"
#include "Capture.h"

CCapture::CCapture(void)
{
	g_capturePath = "";
	g_captureName = "default.jpeg";
}

CCapture::~CCapture(void)
{
}

void CCapture::Capture(HWND hwnd)
{
	CImage image;

	CRect rect;
	GetClientRect(hwnd, &rect);

	int cx = rect.Width();
	int cy = rect.Height();

	HDC hdc = ::GetDC(hwnd);
	// 현재 화면의 픽셀당 컬러 비트수 구하기
	int color_depth = GetDeviceCaps(hdc, BITSPIXEL);

	// 크기, 컬러비트 수를 이용한 이미지 생성
	image.Create(cx, cy, color_depth, 0);

	// 이미지 dc에 화면 dc의 내용을 복사
	BitBlt(image.GetDC(), 0, 0, cx, cy, hdc, 0, 0, SRCCOPY);

	// 이미지를 jpeg로 저장
	image.Save(g_capturePath + "\\" + g_captureName, Gdiplus::ImageFormatJPEG);
	//image.Save(strPath, Gdiplus::ImageFormatBMP);

	// 윈도우 dc 해제
	::ReleaseDC(NULL, hdc);
	image.ReleaseDC();
	AfxMessageBox(g_capturePath + "\\" + g_captureName + "에 캡처되었습니다!");
}

BOOL CCapture::SetCapturePath(HWND hwnd)
{
	BROWSEINFO brInfo;
	TCHAR szBuffer[512];
	::ZeroMemory(&brInfo,sizeof(BROWSEINFO));
	::ZeroMemory(szBuffer, 512);
	brInfo.hwndOwner = hwnd;
	brInfo.lpszTitle = "폴더를 선택하세요";
	brInfo.ulFlags = BIF_NEWDIALOGSTYLE | BIF_EDITBOX | BIF_RETURNONLYFSDIRS;
	LPITEMIDLIST pItemIdList = ::SHBrowseForFolder(&brInfo);
	if(pItemIdList == NULL) // 선택 X
		return FALSE;
	::SHGetPathFromIDList(pItemIdList, szBuffer); // 폴더경로

	CString folderPath;
	CFileFind finder;
	folderPath.Format("%s", szBuffer);
	g_capturePath = folderPath;
	return TRUE;
}