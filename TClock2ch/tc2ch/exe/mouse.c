/*-------------------------------------------
  mouse.c
  mouse operation
  KAZUBON 1997-2001
---------------------------------------------*/

#include "tclock.h"

// XButton Messages
#ifndef WM_XBUTTONDOWN
#define WM_XBUTTONDOWN 0x020B
#define WM_XBUTTONUP   0x020C
#define XBUTTON1       0x0001
#define XBUTTON2       0x0002
#endif
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL  0x020A
#endif

static char reg_section[] = "Mouse";
static UINT last_mousedown  = 0;
static WORD last_xmousedown = 0;
static DWORD last_tickcount;
static int num_click = 0;
static int exec_button = -1;
static BOOL timer = FALSE;

static int GetMouseFuncNum(int button, int nclick);

static ATOM atomHotkey[4] = { 0,0,0,0 };
static const char *atomName[4] = {
	"hotkey1_atom_tcklock2ch",
	"hotkey2_atom_tcklock2ch",
	"hotkey3_atom_tcklock2ch",
	"hotkey4_atom_tcklock2ch"
};


/*------------------------------------------------
    convert hotkey flag
--------------------------------------------------*/
BYTE hkf2modf(BYTE hkf)
{
	BYTE modf = 0;
	if (hkf & HOTKEYF_ALT) modf |= MOD_ALT;
	if (hkf & HOTKEYF_SHIFT) modf |= MOD_SHIFT;
	if (hkf & HOTKEYF_CONTROL) modf |= MOD_CONTROL;
	if (hkf & HOTKEYF_EXT) modf |= MOD_WIN;
	return modf;
}

/*------------------------------------------------
    register hotkey
--------------------------------------------------*/
static void InitHotkey(HWND hwnd)
{
	int i;
	char entry[20];
	WORD hotkey;
	for (i = 0; i < 4; i++)
	{
		if (atomHotkey[i]) continue;
		wsprintf(entry, "15%dHotkey", i + 1);
		hotkey = (WORD)GetMyRegLong(reg_section, entry, 0);
		if (!hotkey) continue;
		atomHotkey[i] = GlobalAddAtom(atomName[i]);
		if (!atomHotkey[i]) continue;
		if (!RegisterHotKey(hwnd, atomHotkey[i], hkf2modf(HIBYTE(hotkey)), LOBYTE(hotkey)))
		{
			GlobalDeleteAtom(atomHotkey[i]);
			atomHotkey[i] = 0;
		}
	}
}

/*------------------------------------------------
    unregister hotkey
--------------------------------------------------*/
static void EndHotkey(HWND hwnd)
{
	int i;
	for (i = 0; i < 4; i++)
	{
		if (!atomHotkey[i]) continue;
		UnregisterHotKey(hwnd, atomHotkey[i]);
		GlobalDeleteAtom(atomHotkey[i]);
		atomHotkey[i] = 0;
	}
}

/*------------------------------------------------
    reset hotkey
--------------------------------------------------*/
void ResetHotkey(HWND hwnd)
{
	EndHotkey(hwnd);
	InitHotkey(hwnd);
}

/*------------------------------------------------
    invoke hotkey
--------------------------------------------------*/
void OnHotkey(HWND hwnd, int id)
{
	int i;
	for (i = 0; i < 4; i++)
	{
		if (atomHotkey[i] == id)
			PostMessage(hwnd, WM_COMMAND, IDC_HOTKEY1 + i, 0);
	}
}

/*------------------------------------------------
    initialize registry data
--------------------------------------------------*/
void InitMouseFunction(HWND hwnd)
{
	int i;
	LONG n;
	char *old_entry[] = { "LClick", "LDblClick" };
	char entry[20];
	char s[256];

	last_tickcount = GetTickCount();

	if(GetMyRegLong(reg_section, "ver230", 0) == 0)
	{
		SetMyRegLong(reg_section, "ver230", 1);
		if(GetMyRegLong(reg_section, "02", -1) < 0)
			SetMyRegLong(reg_section, "02", 0);
	}

	// save old data with new format
	for(i = 0; i < 2; i++)
	{
		n = GetMyRegLong(reg_section, old_entry[i], -1);
		if(n < 0) continue;

		DelMyReg(reg_section, old_entry[i]);
		wsprintf(entry, "0%d", i + 1);
		SetMyRegLong(reg_section, entry, n);
		if(n == 6)
		{
			GetMyRegStr(reg_section, "ClipFormat", s, 256, "");
			if(s[0])
			{
				DelMyReg(reg_section, "ClipFormat");
				wsprintf(entry, "0%dClip", i + 1);
				SetMyRegStr(reg_section, entry, s);
			}
		}
		else if(n == 100)
		{
			strcpy(entry, old_entry[i]); strcat(entry, "File");
			GetMyRegStr(reg_section, entry, s, 256, "");
			if(s[0])
			{
				DelMyReg(reg_section, entry);
				wsprintf(entry, "0%dFile", i + 1);
				SetMyRegStr(reg_section, entry, s);
			}
		}
	}
	InitHotkey(hwnd);
}

void EndMouseFunction(HWND hwnd)
{
	EndHotkey(hwnd);
}

/*------------------------------------------------
   when files dropped to the clock
--------------------------------------------------*/
void OnDropFiles(HWND hwnd, HDROP hdrop)
{
	char fname[MAX_PATH], sname[MAX_PATH];
	char app[1024];
	SHFILEOPSTRUCT shfos;
	char *buf, *p;
	int i, num;
	int nType;

	nType = GetMyRegLong(reg_section, "DropFiles", 0);

	num = DragQueryFile(hdrop, (UINT)-1, NULL, 0);
	if(num <= 0) return;
	buf = malloc(num*MAX_PATH);
	if(buf == NULL) return;
	p = buf;
	for(i = 0; i < num; i++)
	{
		DragQueryFile(hdrop, i, fname, MAX_PATH);
		if(nType == 1 || nType == 3 || nType == 4)  // ���ݔ��A�R�s�[�A�ړ�
		{                             // '\0'�ŋ�؂�ꂽ�t�@�C����
			strcpy(p, fname); p += strlen(p) + 1;
		}
		else if(nType == 2) //�v���O�����ŊJ���F
		{                   //�X�y�[�X�ŋ�؂�ꂽ�Z���t�@�C����
			if(num > 1) GetShortPathName(fname, sname, MAX_PATH);
			else wsprintf(sname, "\"%s\"", fname);
			strcpy(p, sname);
			p += strlen(p);
			if(num > 1 && i < num - 1) { *p = ' '; p++; }
		}
	}
	*p = 0;
	DragFinish(hdrop);

	GetMyRegStr(reg_section, "DropFilesApp", app, 1024, "");

	if(nType == 1 || nType == 3 || nType == 4)  // ���ݔ��A�R�s�[�A�ړ�
	{
		memset(&shfos, 0, sizeof(SHFILEOPSTRUCT));
		shfos.hwnd = NULL;
		if(nType == 1) shfos.wFunc = FO_DELETE;
		else if(nType == 3) shfos.wFunc = FO_COPY;
		else if(nType == 4) shfos.wFunc = FO_MOVE;
		shfos.pFrom = buf;
		if(nType == 3 || nType == 4) shfos.pTo = app;
		shfos.fFlags = FOF_ALLOWUNDO|FOF_NOCONFIRMATION;
		SHFileOperation(&shfos);
	}
	else if(nType == 2) //�t�@�C���ŊJ��
	{
		char *command;
		DWORD len = lstrlen(app) + 1 + lstrlen(buf) + 1;
		command = malloc(len);
		if (command)
		{
			strcpy(command, app);
			strcat(command, " ");
			strcat(command, buf);
			ExecFile(hwnd, command);
			free(command);
		}
	}
	free(buf);
}

/*------------------------------------------------------------
   when the clock clicked

   registry format
   name    value
   03      3           left button triple click -> Minimize All
   32      100         x-1 button  double click -> Run Notepad
   32File  C:\Windows\notepad.exe
--------------------------------------------------------------*/
void OnMouseMsg(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LONG n_func;
	int button;
	UINT doubleclick_time;
	char s[10];
	int i;
	BOOL bDown = FALSE;

	if(timer) KillTimer(hwnd, IDTIMER_MOUSE);
	timer = FALSE;

	if(message == WM_LBUTTONDOWN || message == WM_LBUTTONUP)
	{
		BOOL b;
		b = GetMyRegLong("", "StartButtonHide", FALSE);
		if(b) b = GetMyRegLong("", "StartMenuClock", FALSE);
		if(b) return;
	}

	switch(message)
	{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
			if (wParam & MK_CONTROL)
				button = 5;
			else if (wParam & MK_SHIFT)
				button = 10;
			else
				button = 0;
			break;
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
			if (wParam & MK_CONTROL)
				button = 6;
			else if (wParam & MK_SHIFT)
				button = 11;
			else
				button = 1;
			break;
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
			if (wParam & MK_CONTROL)
				button = 7;
			else if (wParam & MK_SHIFT)
				button = 12;
			else
				button = 2;
			break;
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
			if(HIWORD(wParam) == XBUTTON1)
			{
				if (LOWORD(wParam) & MK_CONTROL)
					button = 8;
				else if (LOWORD(wParam) & MK_SHIFT)
					button = 13;
				else
					button = 3;
			}
			else if(HIWORD(wParam) == XBUTTON2)
			{
				if (LOWORD(wParam) & MK_CONTROL)
					button = 9;
				else if (LOWORD(wParam) & MK_SHIFT)
					button = 14;
				else
					button = 4;
			}
			else return;
			break;
		case WM_MOUSEWHEEL:
			{
				int zDelta, xPos, yPos;
				RECT rcClock;

				GetWindowRect(g_hwndClock, &rcClock);
				xPos = GET_X_LPARAM(lParam);
				yPos = GET_Y_LPARAM(lParam);
				if (!( xPos >= rcClock.left && xPos <= rcClock.right && yPos >= rcClock.top && yPos <= rcClock.bottom ))
					return;
				zDelta = (short) HIWORD(wParam);
				if (zDelta > 0)
				{
				if (LOWORD(wParam) & MK_CONTROL)
					ExecuteMouseFunction(hwnd, -1, 18, 1);
				else if (LOWORD(wParam) & MK_SHIFT)
					ExecuteMouseFunction(hwnd, -1, 20, 1);
				else
					ExecuteMouseFunction(hwnd, -1, 16, 1);
				}
				else
				{
				if (LOWORD(wParam) & MK_CONTROL)
					ExecuteMouseFunction(hwnd, -1, 19, 1);
				else if (LOWORD(wParam) & MK_SHIFT)
					ExecuteMouseFunction(hwnd, -1, 21, 1);
				else
					ExecuteMouseFunction(hwnd, -1, 17, 1);
				}
			return;
			break;
			}
		default: return;
	}

	switch(message)
	{
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_XBUTTONDOWN:
			if(last_mousedown != message) num_click = 0;
			last_mousedown = message;
			if(last_mousedown == WM_XBUTTONDOWN)
				last_xmousedown = HIWORD(wParam);
			bDown = TRUE;
			break;
		case WM_LBUTTONUP:
			if(last_mousedown != WM_LBUTTONDOWN) last_mousedown = 0;
			break;
		case WM_RBUTTONUP:
			if(last_mousedown != WM_RBUTTONDOWN) last_mousedown = 0;
			break;
		case WM_MBUTTONUP:
			if(last_mousedown != WM_MBUTTONDOWN) last_mousedown = 0;
			break;
		case WM_XBUTTONUP:
			if(last_mousedown != WM_XBUTTONDOWN ||
				last_xmousedown != HIWORD(wParam))
			{
				last_mousedown = 0; last_xmousedown = 0;
			}
			break;
	}

	if(last_mousedown == 0) { num_click = 0; return; }

	// Mouse double click speed
	GetMyRegStr(reg_section, "DoubleClickSpeed", s, 10, "");
	if(s[0]) doubleclick_time = atoi(s);
	else     doubleclick_time = GetDoubleClickTime();

	if(GetTickCount() - last_tickcount > doubleclick_time)
		num_click = 0;
	last_tickcount = GetTickCount();

	if(bDown)
	{
		n_func = GetMouseFuncNum(button, num_click + 1);
		if(n_func >= 0 && n_func != MOUSEFUNC_SCREENSAVER)
		{
			for(i = num_click + 2; i <= 4; i++)
			{
				n_func = GetMouseFuncNum(button, i);
				if(n_func >= 0) return;
			}
			num_click++;
			exec_button = button;
			OnTimerMouse(hwnd);
		}
		return;
	}

	num_click++;

	n_func = GetMouseFuncNum(button, num_click);
	if(n_func < 0) return;

	for(i = num_click + 1; i <= 4; i++)
	{
		n_func = GetMouseFuncNum(button, i);
		if(n_func >= 0)
		{
			exec_button = button;
			timer = TRUE;
			SetTimer(hwnd, IDTIMER_MOUSE, doubleclick_time, 0);
			return;
		}
	}

	exec_button = button;
	OnTimerMouse(hwnd);
}

BOOL CALLBACK doKyu(HWND handle,LPARAM height)
{
	RECT rect;

	GetWindowRect(handle, &rect);

	if(!IsZoomed(handle) && IsWindowVisible(handle) && (rect.top < height))
		MoveWindow(handle,rect.left,(int)height,rect.right-rect.left,rect.bottom-rect.top,1);

	return TRUE;
}

/*------------------------------------------------
   execute mouse function
--------------------------------------------------*/
void ExecuteMouseFunction(HWND hwnd, LONG fnc, int btn, int clk)
{
	if(fnc < 0)
	{
		fnc = GetMouseFuncNum(btn, clk);
		if(fnc < 0) return;
	}
	switch (fnc)
	{
		case MOUSEFUNC_DATETIME:
		case MOUSEFUNC_EXITWIN:
		case MOUSEFUNC_RUNAPP:
		case MOUSEFUNC_MINALL:
		{
			WPARAM wParam = 0;
			HWND hwndTray;
			if(fnc == MOUSEFUNC_DATETIME)
				wParam = IDC_DATETIME;
			else if(fnc == MOUSEFUNC_EXITWIN)
				wParam = IDC_EXITWIN;
			else if(fnc == MOUSEFUNC_RUNAPP)
				wParam = IDC_RUNAPP;
			else if(fnc == MOUSEFUNC_MINALL)
				wParam = IDC_MINALL;
			hwndTray = FindWindow("Shell_TrayWnd", NULL);
			if(hwndTray) PostMessage(hwndTray, WM_COMMAND, wParam, 0);
			break;
		}
		case MOUSEFUNC_SYNCTIME:
			StartSyncTime(hwnd, NULL, 0); break;
		case MOUSEFUNC_TIMER:
			DialogTimer(hwnd); break;
		case MOUSEFUNC_CLIPBOARD:
		{
			LPARAM lParam;
			lParam = MAKELONG((WORD)btn, (WORD)clk);
			PostMessage(g_hwndClock, CLOCKM_COPY, 0, lParam);
			break;
		}
		case MOUSEFUNC_SCREENSAVER:
		{
			SendMessage(GetDesktopWindow(), WM_SYSCOMMAND, SC_SCREENSAVE, 0);
			break;
		}
		case MOUSEFUNC_KYU:
		{
			RECT rect;
			GetWindowRect(FindWindow("Shell_TrayWnd",0), &rect);
			if(rect.bottom < GetSystemMetrics(SM_CYSCREEN))
				EnumWindows(doKyu, rect.bottom);
			break;
		}
		case MOUSEFUNC_DELRECDOCS:
		{
			SHAddToRecentDocs(SHARD_PATH, NULL);
			break;
		}
		case MOUSEFUNC_PROPERTY:
		{
			MyPropertyDialog();
			break;
		}
		case MOUSEFUNC_CALENDAR:
		{
			DialogCalender(hwnd);
			break;
		}
		case MOUSEFUNC_OPENFILE:
		{
			char fname[1024];
			char entry[20];
			wsprintf(entry, "%d%dFile", btn, clk);
			GetMyRegStr(reg_section, entry, fname, 1024, "");
			if(fname[0]) ExecFile(hwnd, fname);
			break;
		}
		case MOUSEFUNC_STARTMENU:
		{
			KEYEVENT	key[]={
					{VK_LWIN, 0},					//Windows�L�[������
					{VK_LWIN, KEYEVENTF_KEYUP},		//Windows�L�[�𗣂�
					{0xff,0},
			};
			PushKeybd(key);
			break;
		}
		case MOUSEFUNC_TASKSW:
		{
			KEYEVENT	key[]={
					{VK_MENU, 0},					//Alt�L�[������
					{VK_SHIFT, 0},					//SHIFT�L�[������
					{VK_TAB, 0},					//TAB�L�[������
					{VK_TAB, KEYEVENTF_KEYUP},		//TAB�L�[�𗣂�
					{VK_SHIFT, KEYEVENTF_KEYUP},	//SHIFT�L�[�𗣂�
					{VK_MENU, KEYEVENTF_KEYUP},		//Alt�L�[�𗣂�
					{0xff,0},
			};
			PushKeybd(key);
			break;
		}
		case MOUSEFUNC_SHOWDESK:
		{
			KEYEVENT	key[]={
					{VK_LWIN, 0},					//Windows�L�[������
					{'D', 0},						//D�L�[������
					{'D', KEYEVENTF_KEYUP},			//D�L�[�𗣂�
					{VK_LWIN, KEYEVENTF_KEYUP},		//Windows�L�[�𗣂�
					{0xff,0},
			};
			PushKeybd(key);
			break;
		}
		case MOUSEFUNC_LOCKPC:
		{
			KEYEVENT	key[]={
					{VK_LWIN, 0},					//Windows�L�[������
					{'L', 0},						//L�L�[������
					{'L', KEYEVENTF_KEYUP},			//L�L�[�𗣂�
					{VK_LWIN, KEYEVENTF_KEYUP},		//Windows�L�[�𗣂�
					{0xff,0},
			};
			PushKeybd(key);
			break;
		}
		case MOUSEFUNC_MENU:
		{
			POINT tPoint;

			//���݂̃}�E�X�J�[�\���ʒu���擾
			GetCursorPos(&tPoint);

			OnContextMenu(hwnd,0,tPoint.x,tPoint.y);
			//PostMessage(g_hwndMain, WM_CONTEXTMENU, (WPARAM)hwnd, (LPARAM)MAKELONG(rcClock.right,rcClock.top));
			break;
		}
		case MOUSEFUNC_CHANGECONF:
		{
			int confno, confmax;

			confno = GetMyRegLong(NULL,"ConfigNo",1);
			confmax = GetMyRegLong(NULL,"ConfigMax",1);
			confno++;
			if(confno > confmax)
				confno = 1;
			SetMyRegLong(NULL,"ConfigNo",confno);
			PostMessage(g_hwndClock, CLOCKM_REFRESHCLOCK, 0, 0);
			break;
		}
		case MOUSEFUNC_VOLMUTE:
		{
			ReverseMasterMute();
			PostMessage(g_hwndClock, CLOCKM_VOLCHANGE, TRUE, TRUE);
			break;
		}
		case MOUSEFUNC_VOLSET:
		{
			int vol;
			char entry[20];
			wsprintf(entry, "%d%dVol", btn, clk);
			vol = GetMyRegLong(reg_section, entry, 10);
			SetMasterVolume(vol);
			PostMessage(g_hwndClock, CLOCKM_VOLCHANGE, TRUE, TRUE);
			break;
		}
		case MOUSEFUNC_VOLUD:
		{
			int vol;
			char entry[20];
			wsprintf(entry, "%d%dVol", btn, clk);
			vol = GetMyRegLong(reg_section, entry, 10);
			UpDownMasterVolume(vol);
			PostMessage(g_hwndClock, CLOCKM_VOLCHANGE, TRUE, TRUE);
			break;
		}
		case MOUSEFUNC_MONOFF:
		{
			int delay;
			char entry[20];
			wsprintf(entry, "%d%dDelay", btn, clk);
			delay = GetMyRegLong(reg_section, entry, 0);
			if (delay == 0)
				SendMessage(GetDesktopWindow(), WM_SYSCOMMAND, SC_MONITORPOWER, 2);
			else
				SetTimer(hwnd, IDTIMER_MONOFF, delay * 1000, NULL);
			break;
		}
		case MOUSEFUNC_CDOPEN:
		{
			char cmd[100], drv;
			char entry[20];
			int iDrv;

			wsprintf(entry, "%d%dDrv", btn, clk);
			iDrv= GetMyRegLong(reg_section, entry, 4);
			if (iDrv < 0)
				iDrv = 0;
			else if (iDrv > 26)
				iDrv = 26;
			if ( iDrv > 0 )
			{
				drv = (char)( 'A' - 1 + iDrv );
				wsprintf(cmd,"open %c: type cdaudio alias cdromdrive",drv);
				//�h���C�u�I��
				mciSendString(cmd, NULL, 0, 0);
				mciSendString("set cdromdrive door open",NULL,0,NULL);
				mciSendString("close cdromdrive",NULL,0,NULL);
			}
			else
			{
				mciSendString("set cdaudio door open",NULL,0,NULL);
			}
			break;
		}
		case MOUSEFUNC_CDCLOSE:
		{
			char cmd[100], drv;
			char entry[20];
			int iDrv;

			wsprintf(entry, "%d%dDrv", btn, clk);
			iDrv= GetMyRegLong(reg_section, entry, 4);
			if (iDrv < 0)
				iDrv = 0;
			else if (iDrv > 26)
				iDrv = 26;
			if ( iDrv > 0 )
			{
				drv = (char)( 'A' - 1 + iDrv );
				wsprintf(cmd,"open %c: type cdaudio alias cdromdrive",drv);
				//�h���C�u�I��
				mciSendString(cmd, NULL, 0, 0);
				mciSendString("set cdromdrive door closed",NULL,0,NULL);
				mciSendString("close cdromdrive",NULL,0,NULL);
			}
			else
			{
				mciSendString("set cdaudio door closed",NULL,0,NULL);
			}
			break;
		}
		case MOUSEFUNC_NETINIT:
		{
			PostMessage(g_hwndClock, CLOCKM_NETINIT, TRUE, TRUE);
			break;
		}
		case MOUSEFUNC_DELUS:
		{
			SendMessage(g_hwndClock, CLOCKM_DELUSRSTR, 0, 0);
			PostMessage(g_hwndClock, CLOCKM_REFRESHCLOCK,0,0);
			break;
		}
		case MOUSEFUNC_FILELIST:
		{
			POINT tPoint;

			//���݂̃}�E�X�J�[�\���ʒu���擾
			GetCursorPos(&tPoint);

			showUserMenu(hwnd, 0, tPoint.x, tPoint.y, btn, clk);
			break;
		}
	}
}

/*------------------------------------------------
   mouse timer event
--------------------------------------------------*/
void OnTimerMouse(HWND hwnd)
{
	int button;

	button = exec_button;
	if(timer) KillTimer(hwnd, IDTIMER_MOUSE); timer = FALSE;

	ExecuteMouseFunction(hwnd, -1, button, num_click);
}

int GetMouseFuncNum(int button, int nclick)
{
	char entry[20];
	wsprintf(entry, "%d%d", button, nclick);
	return GetMyRegLong(reg_section, entry, -1);
}

void PushKeybd(LPKEYEVENT lpkey)
{
	while(lpkey->key != 0xff){
		keybd_event(lpkey->key,(BYTE)MapVirtualKey(lpkey->key,0),lpkey->flag, 0);
		lpkey++;
	}
}
