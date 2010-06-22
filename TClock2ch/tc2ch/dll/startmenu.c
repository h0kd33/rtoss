/*-------------------------------------------
  startmenu.c
    customize start menu
    Kazubon 1997-1999
---------------------------------------------*/

#include "tcdll.h"

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL  0x020A
#endif

extern HANDLE hmod;
extern PSTR CreateFullPathName(HINSTANCE hmod, PSTR fname);

static void OnDrawItem(HWND hwnd, DRAWITEMSTRUCT* pdis);
static BOOL IsStartMenu(HMENU hmenu);
void DeleteStartMenuRes(void);

/*------------------------------------------------
�@Globals
--------------------------------------------------*/
static BOOL bStartMenu = FALSE;
static HWND hwndBar = NULL;           // �^�X�N�o�[�̃E�B���h�E�n���h��
static WNDPROC oldWndProcBar = NULL;  // �E�B���h�E�v���V�[�W����ۑ�
static HDC hdcMemMenu = NULL;         // ���j���[�`��p������DC
static HBITMAP hbmpMenu = NULL;       // ���j���[�`��p�r�b�g�}�b�v
static HDC hdcMemMenuLeft = NULL;     // �uWindows95�v�����p������DC
static HBITMAP hbmpMenuLeft = NULL;   // �uWindows95�v�����p�r�b�g�}�b�v
static int hStartMenu = 0;            // ���j���[�̍�����ۑ�
static COLORREF colMenuLeft;          // �F
static BOOL bTile = FALSE;     // ���ׂ�

extern HWND hwndTClockMain;

/*------------------------------------------------
�@�^�X�N�o�[�̃T�u�N���X�v���V�[�W��
--------------------------------------------------*/
LRESULT CALLBACK WndProcBar(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		// ���j���[�̃I�[�i�[�h���[
		case WM_DRAWITEM:
		{
			DRAWITEMSTRUCT* pdis;

			if(!bStartMenu) break;
			pdis = (DRAWITEMSTRUCT*)lParam;
			// �X�^�[�g���j���[�łȂ������炻�̂܂�
			if(wParam || !IsStartMenu((HMENU)pdis->hwndItem)) break;
			OnDrawItem(hwnd, pdis);
			return 1;
		}
		// �V�X�e���F�ύX
		case WM_SYSCOLORCHANGE:
			if(hdcMemMenu) DeleteDC(hdcMemMenu); hdcMemMenu = NULL;
			if(hbmpMenu) DeleteObject(hbmpMenu); hbmpMenu = NULL;
			break;
		// v1.7  �^�X�N�o�[�̕\���������̂�h��
		case WM_EXITSIZEMOVE:
			PostMessage(hwnd, WM_SIZE, SIZE_RESTORED, 0);
			break;

		case WM_DESTROY:
			DeleteStartMenuRes();
			break;
		case WM_MOUSEWHEEL:
			PostMessage(hwndTClockMain, message, wParam, lParam);
			break;
	}
	return CallWindowProc(oldWndProcBar, hwnd, message, wParam, lParam);
}

/*------------------------------------------------
�@���j���[�̃I�[�i�[�h���[
--------------------------------------------------*/
void OnDrawItem(HWND hwnd, DRAWITEMSTRUCT* pdis)
{
	HDC hdc;
	RECT rcBox, rcItem;
	HBRUSH hbr;
	BITMAP bmp;

	hdc = pdis->hDC;
	CopyRect(&rcItem, &(pdis->rcItem));
	GetClipBox(hdc, &rcBox); // ���j���[�S�̂̑傫��

	// �`��p������DC�ƃr�b�g�}�b�v�̍쐬
	if(hdcMemMenu == NULL ||
		(hStartMenu != rcBox.bottom && rcBox.left == 0))
	{
		if(hdcMemMenu) DeleteDC(hdcMemMenu);
		if(hbmpMenu) DeleteObject(hbmpMenu);

		hdcMemMenu = CreateCompatibleDC(hdc);
		hbmpMenu = CreateCompatibleBitmap(hdc, rcBox.right, rcBox.bottom);
		SelectObject(hdcMemMenu, hbmpMenu);
		hbr = CreateSolidBrush(GetSysColor(COLOR_MENU));
		FillRect(hdcMemMenu, &rcBox, hbr);
		hStartMenu = rcBox.bottom;
	}

	SelectObject(hdcMemMenu, (HFONT)GetCurrentObject(hdc, OBJ_FONT));

	// �w�i�F�A�����F�̐ݒ�
	if(pdis->itemState & ODS_FOCUS)
	{
		SetTextColor(hdcMemMenu, GetSysColor(COLOR_HIGHLIGHTTEXT));
		SetBkColor(hdcMemMenu, GetSysColor(COLOR_HIGHLIGHT));
	}
	else
	{
		SetTextColor(hdcMemMenu, GetSysColor(COLOR_MENUTEXT));
		SetBkColor(hdcMemMenu, GetSysColor(COLOR_MENU));
	}

	// ������DC�Ƀf�t�H���g�̕`���������
	pdis->hDC = hdcMemMenu;
	CallWindowProc(oldWndProcBar, hwnd, WM_DRAWITEM, 0, (LPARAM)pdis);

	// �uWindows95�v�̕����Apdis->rcItem.left�ɓ����Ă���
	rcItem.right = pdis->rcItem.left;

	if(rcItem.right > 0)
	{
		COLORREF col;

		if(!bTile)
		{
			//�u�F�v�œh��Ԃ�
			col = colMenuLeft;
			if(col & 0x80000000) col = GetSysColor(col & 0x00ffffff);
			hbr = CreateSolidBrush(col);
			FillRect(hdcMemMenu, &rcItem, hbr);
			DeleteObject(hbr);
		}

		if(hbmpMenuLeft)
			GetObject(hbmpMenuLeft, sizeof(BITMAP), &bmp);

		if(hbmpMenuLeft)
		// && rcItem.bottom > rcBox.bottom - bmp.bmHeight)
		{
			int i, j;
			// �r�b�g�}�b�v�`��
			for(i = 0; ; i++)
			{
				int y, ysrc, h, x, w;
				for(j = 0; ; j++)
				{
					y = rcBox.bottom - ((i + 1) * bmp.bmHeight);
					ysrc = 0;
					h = bmp.bmHeight;
					if(y < 0)
					{
						y = 0;
						ysrc = ((i + 1) * bmp.bmHeight) - rcBox.bottom;
						h -= ysrc;
					}
					x = j * bmp.bmWidth; w = bmp.bmWidth;
					if(x + w > rcItem.right)
					{
						w -= ((j + 1) * bmp.bmWidth) - rcItem.right;
					}
					if(w > 0 && h > 0)
						BitBlt(hdcMemMenu, x, y, w, h,
							hdcMemMenuLeft, 0, ysrc, SRCCOPY);
					if(!bTile || w < bmp.bmWidth) break;
				}
				if(!bTile || y == 0) break;
			}
		}
	}

	// �{����DC�ɂ܂Ƃ߂ĕ`��
	BitBlt(hdc, 0, rcItem.top,
		pdis->rcItem.right, rcItem.bottom - rcItem.top,
		hdcMemMenu, 0, rcItem.top, SRCCOPY);
	pdis->hDC = hdc;
}

/*--------------------------------------------------
�@�X�^�[�g���j���[�����̏�����
----------------------------------------------------*/
void SetStartMenu(HWND hwndClock)
{
	HWND hwndTray;

	char fname[1024];

	EndStartMenu();

	// �^�X�N�o�[�̃T�u�N���X���iv1.7���K���T�u�N���X���j
	hwndTray = GetParent(hwndClock); // TrayNotifyWnd
	if(hwndTray == NULL)
		return;
	hwndBar = GetParent(hwndTray);   // Shell_TrayWnd
	if(hwndBar == NULL)
		return;
	oldWndProcBar = (WNDPROC)GetWindowLong(hwndBar, GWL_WNDPROC);
	SetWindowLong(hwndBar, GWL_WNDPROC, (LONG)WndProcBar);

	bStartMenu = GetMyRegLong(NULL, "StartMenu", FALSE);

	if(!bStartMenu) return;

	colMenuLeft = GetMyRegLong(NULL, "StartMenuCol",
		RGB(128, 128, 128));

	bTile = GetMyRegLong(NULL, "StartMenuTile", FALSE);

	GetMyRegStr(NULL, "StartMenuBmp", fname, 1024, "");

	if(fname[0]) // �uWindows95�v�����p�̃�����DC�ƃr�b�g�}�b�v�쐬
	{
		PSTR path;
		path = CreateFullPathName(hmod, fname);
		if (path == NULL) {
			hbmpMenuLeft = ReadBitmap(hwndBar, fname, FALSE);
		} else {
			hbmpMenuLeft = ReadBitmap(hwndBar, path, FALSE);
			free(path);
		}
		if(hbmpMenuLeft)
		{
			HDC hdc;
			hdc = GetDC(hwndBar);
			hdcMemMenuLeft = CreateCompatibleDC(hdc);
			SelectObject(hdcMemMenuLeft, hbmpMenuLeft);
			ReleaseDC(hwndBar, hdc);
		}
	}
}

/*--------------------------------------------------
�@���ɖ߂�
----------------------------------------------------*/
void DeleteStartMenuRes(void)
{
	if(hdcMemMenu) DeleteDC(hdcMemMenu); hdcMemMenu = NULL;
	if(hbmpMenu) DeleteObject(hbmpMenu); hbmpMenu = NULL;
	if(hdcMemMenuLeft) DeleteDC(hdcMemMenuLeft); hdcMemMenuLeft = NULL;
	if(hbmpMenuLeft) DeleteObject(hbmpMenuLeft); hbmpMenuLeft = NULL;
}
void EndStartMenu(void)
{
	if(hwndBar && IsWindow(hwndBar))
	{
#if ENABLE_CHECK_SUBCLASS_NESTING
		if (oldWndProcBar && (WNDPROC)WndProcBar == (WNDPROC)GetWindowLong(hwndBar, GWL_WNDPROC))
#else
		if (oldWndProcBar)
#endif
			SetWindowLong(hwndBar, GWL_WNDPROC, (LONG)oldWndProcBar);
	}
	hwndBar = NULL; oldWndProcBar = NULL;

	DeleteStartMenuRes();
}

/*--------------------------------------------------
�@�X�^�[�g���j���[���ǂ�������
----------------------------------------------------*/
BOOL IsStartMenu(HMENU hmenu)
{
	int i, count, id;

	count = GetMenuItemCount(hmenu);
	for(i = 0; i < count; i++)
	{
		id = GetMenuItemID(hmenu, i);
		// �u�w���v�v������΃X�^�[�g���j���[
		if(id == 503) return TRUE;
	}
	return FALSE;
}

// ------------------------------------------------------
// �ȉ��AIE4�p

LRESULT CALLBACK WndProcStartMenu(HWND hwnd, UINT message,
	WPARAM wParam, LPARAM lParam);
HWND hwndStartMenu = NULL; // �X�^�[�g���j���[�̃E�B���h�E�n���h��
WNDPROC oldWndProcStartMenu = NULL;
void OnPaintStartmenuIE4(HWND hwnd, HDC hdc, BOOL bPrint);

/*--------------------------------------------------
�@�X�^�[�g���j���[�E�B���h�E�̃T�u�N���X��
----------------------------------------------------*/
void InitStartMenuIE4(void)
{
	HWND hwnd, hwndChild;
	HWND hwndFound;
	char classname[80];
	RECT rc1, rc2;

	ClearStartMenuIE4();
	if(!bStartMenu) return;
	// �X�^�[�g���j���[��T��
	hwnd = GetDesktopWindow();
	hwnd = GetWindow(hwnd, GW_CHILD);
	hwndFound = NULL;
	while(hwnd)
	{
		GetClassName(hwnd, classname, 80);
		if(lstrcmpi(classname, "BaseBar") == 0)
		{
			if(GetWindowThreadProcessId(hwnd, NULL) ==
				GetCurrentThreadId())
			{
				hwndChild = GetWindow(hwnd, GW_CHILD);
				GetClientRect(hwnd, &rc1);
				GetClientRect(hwndChild, &rc2);
				if(rc1.right - rc2.right == 21 || rc2.right == 0)
				{
					if(hwndFound == NULL
						|| (int)hwndFound > (int)hwnd)
						hwndFound = hwnd;
				}
			}
		}
		hwnd = GetWindow(hwnd, GW_HWNDNEXT);
	}
	hwnd = hwndFound;
	if(hwnd == NULL) return;
	// �T�u�N���X��
	oldWndProcStartMenu = (WNDPROC)GetWindowLong(hwnd, GWL_WNDPROC);
	SetWindowLong(hwnd, GWL_WNDPROC, (LONG)WndProcStartMenu);
	hwndStartMenu = hwnd;
}

/*--------------------------------------------------
�@�X�^�[�g���j���[�E�B���h�E�����ɖ߂�
----------------------------------------------------*/
void ClearStartMenuIE4(void)
{
	if(hwndStartMenu && IsWindow(hwndStartMenu))
	{
#if ENABLE_CHECK_SUBCLASS_NESTING
		if (oldWndProcStartMenu && (WNDPROC)WndProcStartMenu == (WNDPROC)GetWindowLong(hwndStartMenu, GWL_WNDPROC))
#else
		if (oldWndProcStartMenu)
#endif
			SetWindowLong(hwndStartMenu, GWL_WNDPROC, (LONG)oldWndProcStartMenu);
	}
	hwndStartMenu = NULL; oldWndProcStartMenu = NULL;
}

/*------------------------------------------------
�@�X�^�[�g���j���[�̃T�u�N���X�v���V�[�W��
--------------------------------------------------*/
LRESULT CALLBACK WndProcStartMenu(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		// ���j���[�̕`��
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
			if(!bStartMenu) break;
			hdc = BeginPaint(hwnd, &ps);
			OnPaintStartmenuIE4(hwnd, hdc, FALSE);
			EndPaint(hwnd, &ps);
			return 0;
		}
		case WM_PRINT:
		{
			LRESULT r;
			if(!bStartMenu) break;
			r = CallWindowProc(oldWndProcStartMenu, hwnd, message, wParam, lParam);
			OnPaintStartmenuIE4(hwnd, (HDC)wParam, TRUE);
			return r;
		}
		// �V�X�e���F�ύX
		case WM_SYSCOLORCHANGE:
			break;
	}
	return CallWindowProc(oldWndProcStartMenu, hwnd, message, wParam, lParam);
}

/*------------------------------------------------
�@�X�^�[�g���j���[�̕`��
--------------------------------------------------*/
void OnPaintStartmenuIE4(HWND hwnd, HDC hdc, BOOL bPrint)
{
	RECT rc, rcWin, rcChild;
	POINT pt;
	COLORREF col;
	HBRUSH hbr;
	BITMAP bmp;
	int hClient, wClient;

	GetWindowRect(GetWindow(hwnd, GW_CHILD), &rcChild);
	GetWindowRect(hwnd, &rcWin);
	GetClientRect(hwnd, &rc);
	pt.x = 0; pt.y = 0;
	ClientToScreen(hwnd, &pt);
	if(pt.x == rcChild.left) return;

	rc.right = 21;
	wClient = rc.right; hClient = rc.bottom;
	if(bPrint)
	{
		int dx, dy;
		dx = pt.x - rcWin.left; dy = pt.y - rcWin.top;
		rc.left += dx; rc.right += dx;
		rc.top += dy; rc.bottom += dy;
	}
	//�u�F�v�œh��Ԃ�
	col = colMenuLeft;
	if(col & 0x80000000) col = GetSysColor(col & 0x00ffffff);
	hbr = CreateSolidBrush(col);
	FillRect(hdc, &rc, hbr);
	DeleteObject(hbr);
	if(hbmpMenuLeft)
	{
		int i, j;
		GetObject(hbmpMenuLeft, sizeof(BITMAP), &bmp);
		// �r�b�g�}�b�v�`��
		for(i = 0; ; i++)
		{
			int y, ysrc, h, x, w;
			for(j = 0; ; j++)
			{
				y = hClient - ((i + 1) * bmp.bmHeight);
				ysrc = 0;
				h = bmp.bmHeight;
				if(y < 0)
				{
					y = 0;
					ysrc = ((i + 1) * bmp.bmHeight) - hClient;
					h -= ysrc;
				}
				x = j * bmp.bmWidth; w = bmp.bmWidth;
				if(x + w > wClient)
				{
					w -= ((j + 1) * bmp.bmWidth) - wClient;
				}
				if(w > 0 && h > 0)
					BitBlt(hdc, rc.left + x, rc.top + y, w, h,
						hdcMemMenuLeft, 0, ysrc, SRCCOPY);
				if(!bTile || w < bmp.bmWidth) break;
			}
			if(!bTile || y == 0) break;
		}
	}
}

