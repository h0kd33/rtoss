/*-------------------------------------------
  utl.c
    ���̑��̊֐�
    KAZUBON 1997-1998
---------------------------------------------*/

#include "tclock.h"


#if defined(_MSC_VER) && (_MSC_VER >= 1200)
typedef LARGE_INTEGER TC_SINT64;
typedef ULARGE_INTEGER TC_UINT64;
#else
typedef union _TC_SINT64 {
    struct {
        DWORD LowPart;
        LONG HighPart;
    } u;
    LONGLONG QuadPart;
} TC_SINT64;
typedef union _TC_UINT64 {
    struct {
        DWORD LowPart;
        DWORD HighPart;
    } u;
    ULONGLONG QuadPart;
} TC_UINT64;
#endif

/*-------------------------------------------
  �����^�C���֐��̑�p
---------------------------------------------*/
void r_memcpy(void *d, const void *s, size_t l)
{
	size_t i;
	for (i = 0; i < l; i++) ((char *)d)[i] = ((const char *)s)[i];
}

void r_memset(void *d, int c, size_t l)
{
	size_t i;
	for (i = 0; i < l; i++) ((char *)d)[i] = (char)c;
}

int r_atoi(const char *p)
{
	int r = 0;
	while(*p)
	{
		if('0' <= *p && *p <= '9')
			r = r * 10 + *p - '0';
		p++;

	}
	return r;
}

int atox(const char *p)
{
	int r = 0;
	while(*p)
	{
		if('0' <= *p && *p <= '9')
			r = r * 16 + *p - '0';
		else if('A' <= *p && *p <= 'F')
			r = r * 16 + *p - 'A' + 10;
		else if('a' <= *p && *p <= 'f')
			r = r * 16 + *p - 'a' + 10;
		p++;

	}
	return r;
}

__inline int r_toupper(int c)
{
	if('a' <= c && c <= 'z')
		c -= 'a' - 'A';
	return c;
}

int r_strnicmp(const char* d, const char* s, size_t n)
{
	int c1, c2;
	unsigned int i;
	for(i = 0; i < n; i++)
	{
		if(*s == 0 && *d == 0) break;
		c1 = r_toupper(*d); c2 = r_toupper(*s);
		if(c1 != c2) return (c1 - c2);
		d++; s++;
	}
	return 0;
}

int r_stricmp(const char* d, const char* s)
{
	int c1, c2;
	for (;;)
	{
		if(*s == 0 && *d == 0) break;
		c1 = r_toupper(*d); c2 = r_toupper(*s);
		if(c1 != c2) return (c1 - c2);
		d++; s++;
	}
	return 0;
}

/*-------------------------------------------
�@�p�X���Ƀt�@�C����������
---------------------------------------------*/
void add_title(char *path, char *title)
{
	char *p;

	p = path;

	if(*p == 0) ;
	else if(*title && *(title + 1) == ':') ;
	else if(*title == '\\')
	{
		if(*p && *(p + 1) == ':') p += 2;
	}
	else
	{
		while(*p)
		{
			if((*p == '\\' || *p == '/') && *(p + 1) == 0)
			{
				break;
			}
			p = CharNext(p);
		}
		*p++ = '\\';
	}
	while(*title) *p++ = *title++;
	*p = 0;
}

/*-------------------------------------------
�@�p�X������t�@�C�������Ƃ�̂���
---------------------------------------------*/
void del_title(char *path)
{
	char *p, *ep;

	p = ep = path;
	while(*p)
	{
		if(*p == '\\' || *p == '/')
		{
			if(p > path && *(p - 1) == ':') ep = p + 1;
			else ep = p;
		}
		p = CharNext(p);
	}
	*ep = 0;
}

/*-------------------------------------------
�@�p�X������t�@�C�����𓾂�
---------------------------------------------*/
void get_title(char* dst, const char *path)
{
	const char *p, *ep;

	p = ep = path;
	while(*p)
	{
		if(*p == '\\' || *p == '/')
		{
			if(p > path && *(p - 1) == ':') ep = p + 1;
			else ep = p;
		}
		p = CharNext(p);
	}

	if(*ep == '\\' || *ep == '/') ep++;

	while(*ep) *dst++ = *ep++;
	*dst = 0;
}

/*------------------------------------------------
�@�t�@�C���̊g���q�̔�r
--------------------------------------------------*/
int ext_cmp(const char *fname, const char *ext)
{
	const char* p, *sp;

	sp = NULL; p = fname;
	while(*p)
	{
		if(*p == '.') sp = p;
		else if(*p == '\\' || *p == '/') sp = NULL;
		p = CharNext(p);
	}

	if(sp == NULL) sp = p;
	if(*sp == '.') sp++;

	for (;;)
	{
		if(*sp == 0 && *ext == 0) return 0;
		if(r_toupper(*sp) != r_toupper(*ext))
			return (r_toupper(*sp) - r_toupper(*ext));
		sp++; ext++;
	}
	//return 0;
}

/*------------------------------------------------
	�J���}�ŋ�؂�ꂽ����������o��
--------------------------------------------------*/
void parse(char *dst, char *src, int n)
{
	char *dp;
	int i;

	for(i = 0; i < n; i++)
	{
		while(*src && *src != ',') src++;
		if(*src == ',') src++;
	}
	if(*src == 0)
	{
		*dst = 0; return;
	}

	while(*src == ' ') src++;

	dp = dst;
	while(*src && *src != ',') *dst++ = *src++;
	*dst = 0;

	while(dst != dp)
	{
		dst--;
		if(*dst == ' ') *dst = 0;
		else break;
	}
}

/*------------------------------------------------
	�����ŋ�؂�ꂽ����������o��
--------------------------------------------------*/
void parsechar(char *dst, char *src, char ch, int n)
{
	char *dp;
	int i;

	for(i = 0; i < n; i++)
	{
		while(*src && *src != ch) src++;
		if(*src == ch) src++;
	}
	if(*src == 0)
	{
		*dst = 0; return;
	}

	while(*src == ' ') src++;

	dp = dst;
	while(*src && *src != ch) *dst++ = *src++;
	*dst = 0;

	while(dst != dp)
	{
		dst--;
		if(*dst == ' ') *dst = 0;
		else break;
	}
}

/*------------------------------------------------
�@'\0'�ŏI�����镶�����ǉ�����
�@�Ō��"\0\0"�ŏI��
--------------------------------------------------*/
void str0cat(char* dst, const char* src)
{
	char* p;
	p = dst;
	while(*p) { while(*p) p++; p++; }
	strcpy(p, src);
	while(*p) p++; p++; *p = 0;
}

/*-------------------------------------------
  returns a resource string
---------------------------------------------*/
char* MyString(UINT id)
{
	static char buf[MAX_PATH];
	HINSTANCE hInst;

	buf[0] = 0;
	hInst = GetLangModule();
	if(hInst) LoadString(hInst, id, buf, MAX_PATH);

	return buf;
}

/*-------------------------------------------
  �A�C�R�������b�Z�[�W�{�b�N�X
---------------------------------------------*/
int MyMessageBox(HWND hwnd, char* msg, char* title, UINT uType, UINT uBeep)
{
	MSGBOXPARAMS mbp;

	mbp.cbSize = sizeof(MSGBOXPARAMS);
	mbp.hwndOwner = hwnd;
	mbp.hInstance = g_hInst;
	mbp.lpszText = msg;
	mbp.lpszCaption = title;
	mbp.dwStyle = MB_USERICON | uType;
	mbp.lpszIcon = MAKEINTRESOURCE(IDI_ICON1);
	mbp.dwContextHelpId = 0;
	mbp.lpfnMsgBoxCallback = NULL;
	mbp.dwLanguageId = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);
	if(uBeep != 0xFFFFFFFF)
		MessageBeep(uBeep);
	return MessageBoxIndirect(&mbp);
}

/*------------------------------------------------
  get locale info for 95/NT
--------------------------------------------------*/
int GetLocaleInfoWA(int ilang, LCTYPE LCType, char* dst, int n)
{
	int r;
	LCID Locale;

	*dst = 0;
	Locale = MAKELCID((WORD)ilang, SORT_DEFAULT);
	if(GetVersion() & 0x80000000) // 95
		r = GetLocaleInfoA(Locale, LCType, dst, n);
	else  // NT
	{
		WCHAR* pw;
		pw = (WCHAR*)GlobalAllocPtr(GHND, sizeof(WCHAR)*(n+1));
		r = GetLocaleInfoW(Locale, LCType, pw, n);
		if(r)
			WideCharToMultiByte(CP_ACP, 0, pw, -1, dst, n,
				NULL, NULL);
		GlobalFreePtr(pw);
	}
	return r;
}

/*-------------------------------------------
  32bit x 32bit = 64bit
---------------------------------------------*/
DWORDLONG M32x32to64(DWORD a, DWORD b)
{
	TC_UINT64 r;
	DWORD *p1, *p2, *p3;
	r.QuadPart = 0;
	p1 = &r.u.LowPart;
	p2 = (DWORD*)((BYTE*)p1 + 2);
	p3 = (DWORD*)((BYTE*)p2 + 2);
	*p1 = LOWORD(a) * LOWORD(b);
	*p2 += LOWORD(a) * HIWORD(b) + HIWORD(a) * LOWORD(b);
	*p3 += HIWORD(a) * HIWORD(b);
	return *(DWORDLONG*)(&r);
}

/*-------------------------------------------
  SetForegroundWindow for Windows98
---------------------------------------------*/
void SetForegroundWindow98(HWND hwnd)
{
	DWORD dwVer;

	dwVer = GetVersion();
	if(((dwVer & 0x80000000) &&
	       LOBYTE(LOWORD(dwVer)) >= 4 && HIBYTE(LOWORD(dwVer)) >= 10) ||
	   (!(dwVer & 0x80000000) && LOBYTE(LOWORD(dwVer)) >= 5)) // Win98/2000
	{
		DWORD thread1, thread2;
		DWORD pid;
		thread1 = GetWindowThreadProcessId(
			GetForegroundWindow(), &pid);
		thread2 = GetCurrentThreadId();
		AttachThreadInput(thread2, thread1, TRUE);
		SetForegroundWindow(hwnd);
		AttachThreadInput(thread2, thread1, FALSE);
		BringWindowToTop(hwnd);
	}
	else  // Win95/NT
		SetForegroundWindow(hwnd);
}

/*-------------------------------------------
  for debugging
---------------------------------------------*/
void WriteDebug(const char* s)
{
	HFILE hf;
	char fname[] = "DEBUG.TXT";

	hf = _lopen(fname, OF_WRITE);
	if(hf == HFILE_ERROR)
		hf = _lcreat(fname, 0);
	if(hf == HFILE_ERROR) return;
	_llseek(hf, 0, 2);
	_lwrite(hf, s, strlen(s));
	_lwrite(hf, "\x0d\x0a", 2);
	_lclose(hf);
}

//TClock�p�̃��W�X�g���̃L�[
//char mykey[] = "Software\\Kazubon\\TClock";
//char mykey2[] = "Software\\Kazubon";
#define mykey "Software\\Kazubon\\TClock"
#define mykey2 "Software\\Kazubon"
/*----------------------------------------------
/*------------------------------------------------
�@�����̃��W�X�g�����當����𓾂�
--------------------------------------------------*/
int GetMyRegStr(char* section, char* entry, char* val, int cbData,
	char* defval)
{
	char key[80];
	HKEY hkey;
	DWORD regtype;
	DWORD size;
	BOOL b;
	int r = 0;

	if(g_bIniSetting) key[0] = 0;
	else strcpy(key, mykey);

	if(section && *section)
	{
		if(!g_bIniSetting) strcat(key, "\\");
		strcat(key, section);
	}
	else
	{
		if(g_bIniSetting) strcpy(key, "Main");
	}

	if(g_bIniSetting)
	{
		r = GetPrivateProfileString(key, entry, defval, val,
			cbData, g_inifile);
	}
	else
	{
		b = FALSE;
		if(RegOpenKey(HKEY_CURRENT_USER, key, &hkey) == 0)
		{
			size = cbData;
			if(RegQueryValueEx(hkey, entry, 0, &regtype,
				(LPBYTE)val, &size) == 0)
			{
				if(size == 0) *val = 0;
				r = size;
				b = TRUE;
			}
			RegCloseKey(hkey);
		}
		if(b == FALSE)
		{
			strcpy(val, defval);
			r = strlen(defval);
		}
	}

	return r;
}

/*------------------------------------------------
�@�����̃��W�X�g�����當����𓾂�
--------------------------------------------------*/
int GetMyRegStrEx(char* section, char* entry, char* val, int cbData,
	char* defval, int confno)
{
	char key[80];
	char sectionEx[80];
	HKEY hkey;
	DWORD regtype;
	DWORD size;
	BOOL b;
	int r = 0;

	if(g_bIniSetting) key[0] = 0;
	else strcpy(key, mykey);

	if (confno > 1)
	{
		if(section && *section)
		{
			if(!g_bIniSetting)
				wsprintf(sectionEx,"\\%s%d",section,confno);
			else
				wsprintf(sectionEx,"%s%d",section,confno);
			strcat(key, sectionEx);
		}
		else
		{
			if(!g_bIniSetting)
				wsprintf(sectionEx,"\\%s%d","Main",confno);
			else
				wsprintf(sectionEx,"%s%d","Main",confno);
			strcat(key, sectionEx);
		}
	}
	else
	{
		if(section && *section)
		{
			if(!g_bIniSetting) strcat(key, "\\");
			strcat(key, section);
		}
		else
		{
			if(g_bIniSetting) strcpy(key, "Main");
		}
	}


	if(g_bIniSetting)
	{
		r = GetPrivateProfileString(key, entry, defval, val,
			cbData, g_inifile);
	}
	else
	{
		b = FALSE;
		if(RegOpenKey(HKEY_CURRENT_USER, key, &hkey) == 0)
		{
			size = cbData;
			if(RegQueryValueEx(hkey, entry, 0, &regtype,
				(LPBYTE)val, &size) == 0)
			{
				if(size == 0) *val = 0;
				r = size;
				b = TRUE;
			}
			RegCloseKey(hkey);
		}
		if(b == FALSE)
		{
			strcpy(val, defval);
			r = strlen(defval);
		}
	}

	return r;
}

/*------------------------------------------------
�@�����̃��W�X�g������LONG�l�𓾂�
--------------------------------------------------*/
LONG GetMyRegLong(char* section, char* entry, LONG defval)
{
	char key[80];
	HKEY hkey;
	DWORD regtype;
	DWORD size;
	BOOL b;
	LONG r = 0;

	if(g_bIniSetting) key[0] = 0;
	else strcpy(key, mykey);

	if(section && *section)
	{
		if(!g_bIniSetting) strcat(key, "\\");
		strcat(key, section);
	}
	else
	{
		if(g_bIniSetting) strcpy(key, "Main");
	}

	if(g_bIniSetting)
	{
		r = GetPrivateProfileInt(key, entry, defval, g_inifile);
	}
	else
	{
		b = FALSE;
		if(RegOpenKey(HKEY_CURRENT_USER, key, &hkey) == 0)
		{
			size = 4;
			if(RegQueryValueEx(hkey, entry, 0, &regtype,
				(LPBYTE)&r, &size) == 0)
			{
				if(size == 4) b = TRUE;
			}
			RegCloseKey(hkey);
		}
		if(b == FALSE) r = defval;
	}
	return r;
}

/*------------------------------------------------
�@�����̃��W�X�g������LONG�l�𓾂�̊g����
--------------------------------------------------*/
LONG GetMyRegLongEx(char* section, char* entry, LONG defval, int confno)
{
	char key[80];
	char sectionEx[80];
	HKEY hkey;
	DWORD regtype;
	DWORD size;
	BOOL b;
	LONG r = 0;

	if(g_bIniSetting) key[0] = 0;
	else strcpy(key, mykey);

	if (confno > 1)
	{
		if(section && *section)
		{
			if(!g_bIniSetting)
				wsprintf(sectionEx,"\\%s%d",section,confno);
			else
				wsprintf(sectionEx,"%s%d",section,confno);
			strcat(key, sectionEx);
		}
		else
		{
			if(!g_bIniSetting)
				wsprintf(sectionEx,"\\%s%d","Main",confno);
			else
				wsprintf(sectionEx,"%s%d","Main",confno);
			strcat(key, sectionEx);
		}
	}
	else
	{
		if(section && *section)
		{
			if(!g_bIniSetting) strcat(key, "\\");
			strcat(key, section);
		}
		else
		{
			if(g_bIniSetting) strcpy(key, "Main");
		}
	}

	if(g_bIniSetting)
	{
		r = GetPrivateProfileInt(key, entry, defval, g_inifile);
	}
	else
	{
		b = FALSE;
		if(RegOpenKey(HKEY_CURRENT_USER, key, &hkey) == 0)
		{
			size = 4;
			if(RegQueryValueEx(hkey, entry, 0, &regtype,
				(LPBYTE)&r, &size) == 0)
			{
				if(size == 4) b = TRUE;
			}
			RegCloseKey(hkey);
		}
		if(b == FALSE) r = defval;
	}
	return r;
}

/*------------------------------------------------
  get DWORD value from registry
--------------------------------------------------*/
LONG GetRegLong(HKEY rootkey, char*subkey, char* entry, LONG defval)
{
	HKEY hkey;
	DWORD regtype;
	DWORD size;
	BOOL b;
	int r = 0;

	b = FALSE;
	if(RegOpenKey(rootkey, subkey, &hkey) == 0)
	{
		size = 4;
		if(RegQueryValueEx(hkey, entry, 0, &regtype,
			(LPBYTE)&r, &size) == 0)
		{
			if(size == 4) b = TRUE;
		}
		RegCloseKey(hkey);
	}
	if(b == FALSE) r = defval;
	return r;
}

/*------------------------------------------------
�@���W�X�g�����當����𓾂�
--------------------------------------------------*/
int GetRegStr(HKEY rootkey, char*subkey, char* entry,
	char* val, int cbData, char* defval)
{
	HKEY hkey;
	DWORD regtype;
	DWORD size;
	BOOL b;
	int r = 0;

	b = FALSE;
	if(RegOpenKey(rootkey, subkey, &hkey) == 0)
	{
		size = cbData;
		if(RegQueryValueEx(hkey, entry, 0, &regtype,
			(LPBYTE)val, &size) == 0)
		{
			if(size == 0) *val = 0;
			b = TRUE;
		}
		RegCloseKey(hkey);
	}
	if(b == FALSE)
	{
		strcpy(val, defval);
		r = strlen(defval);
	}
	return r;
}

/*-------------------------------------------
�@���W�X�g���ɕ��������������
---------------------------------------------*/
BOOL SetMyRegStr(char* section, char* entry, char* val)
{
	HKEY hkey;
	BOOL r;
	char key[80];

	if(g_bIniSetting) key[0] = 0;
	else strcpy(key, mykey);

	if(section && *section)
	{
		if(!g_bIniSetting) strcat(key, "\\");
		strcat(key, section);
	}
	else
	{
		if(g_bIniSetting) strcpy(key, "Main");
	}

	if(g_bIniSetting)
	{
		char *chk_val;
		BOOL b_chkflg = FALSE;
		char saveval[1024];

		r = FALSE;
		chk_val = val;
		while(*chk_val)
		{
			if (*chk_val == '\"' || *chk_val == '\'' || *chk_val == ' '  )
				b_chkflg = TRUE;
			chk_val++;
		}

		if (b_chkflg)
		{
			strcpy(saveval,"\"");
			strcat(saveval,val);
			strcat(saveval,"\"");
		}
		else
			strcpy(saveval,val);

		if(WritePrivateProfileString(key, entry, saveval, g_inifile))
			r = TRUE;
	}
	else
	{
		r = FALSE;
		if(RegCreateKey(HKEY_CURRENT_USER, key, &hkey) == 0)
		{
			if(RegSetValueEx(hkey, entry, 0, REG_SZ,
				(CONST BYTE*)val, strlen(val)) == 0)
			{
				r = TRUE;
			}
			RegCloseKey(hkey);
		}
	}
	return r;
}

/*-------------------------------------------
�@���W�X�g���ɕ��������������
---------------------------------------------*/
BOOL SetMyRegStrEx(char* section, char* entry, char* val, int confno)
{
	HKEY hkey;
	BOOL r;
	char key[80];
	char sectionEx[80];

	if(g_bIniSetting) key[0] = 0;
	else strcpy(key, mykey);

	if (confno > 1)
	{
		if(section && *section)
		{
			if(!g_bIniSetting)
				wsprintf(sectionEx,"\\%s%d",section,confno);
			else
				wsprintf(sectionEx,"%s%d",section,confno);
			strcat(key, sectionEx);
		}
		else
		{
			if(!g_bIniSetting)
				wsprintf(sectionEx,"\\%s%d","Main",confno);
			else
				wsprintf(sectionEx,"%s%d","Main",confno);
			strcat(key, sectionEx);
		}
	}
	else
	{
		if(section && *section)
		{
			if(!g_bIniSetting) strcat(key, "\\");
			strcat(key, section);
		}
		else
		{
			if(g_bIniSetting) strcpy(key, "Main");
		}
	}

	if(g_bIniSetting)
	{
		char *chk_val;
		BOOL b_chkflg = FALSE;
		char saveval[1024];

		r = FALSE;
		chk_val = val;
		while(*chk_val)
		{
			if (*chk_val == '\"' || *chk_val == '\'' || *chk_val == ' '  )
				b_chkflg = TRUE;
			chk_val++;
		}

		if (b_chkflg)
		{
			strcpy(saveval,"\"");
			strcat(saveval,val);
			strcat(saveval,"\"");
		}
		else
			strcpy(saveval,val);

		if(WritePrivateProfileString(key, entry, saveval, g_inifile))
			r = TRUE;
	}
	else
	{
		r = FALSE;
		if(RegCreateKey(HKEY_CURRENT_USER, key, &hkey) == 0)
		{
			if(RegSetValueEx(hkey, entry, 0, REG_SZ,
				(CONST BYTE*)val, strlen(val)) == 0)
			{
				r = TRUE;
			}
			RegCloseKey(hkey);
		}
	}
	return r;
}

/*-------------------------------------------
�@���W�X�g���ɕ��������������
---------------------------------------------*/
BOOL SetRegStr(HKEY rootkey, char* subkey, char* entry, char* val)
{
	HKEY hkey;
	BOOL r = FALSE;

	if(RegCreateKey(rootkey, subkey, &hkey) == 0)
	{
		if(RegSetValueEx(hkey, entry, 0, REG_SZ,
			(CONST BYTE*)val, strlen(val)) == 0)
		{
			r = TRUE;
		}
		RegCloseKey(hkey);
	}
	return r;
}

/*-------------------------------------------
�@���W�X�g����DWORD�l����������
---------------------------------------------*/
BOOL SetMyRegLong(char* section, char* entry, DWORD val)
{
	HKEY hkey;
	BOOL r;
	char key[80];

	if(g_bIniSetting) key[0] = 0;
	else strcpy(key, mykey);

	if(section && *section)
	{
		if(!g_bIniSetting) strcat(key, "\\");
		strcat(key, section);
	}
	else
	{
		if(g_bIniSetting) strcpy(key, "Main");
	}

	if(g_bIniSetting)
	{
		char s[20];
		wsprintf(s, "%d", val);
		r = FALSE;
		if(WritePrivateProfileString(key, entry, s, g_inifile))
			r = TRUE;
	}
	else
	{
		r = FALSE;
		if(RegCreateKey(HKEY_CURRENT_USER, key, &hkey) == 0)
		{
			if(RegSetValueEx(hkey, entry, 0, REG_DWORD,
				(CONST BYTE*)&val, 4) == 0)
			{
				r = TRUE;
			}
			RegCloseKey(hkey);
		}
	}
	return r;
}

/*-------------------------------------------
�@���W�X�g����DWORD�l����������
---------------------------------------------*/
BOOL SetMyRegLongEx(char* section, char* entry, DWORD val, int confno)
{
	HKEY hkey;
	BOOL r;
	char key[80];
	char sectionEx[80];

	if(g_bIniSetting) key[0] = 0;
	else strcpy(key, mykey);

	if (confno > 1)
	{
		if(section && *section)
		{
			if(!g_bIniSetting)
				wsprintf(sectionEx,"\\%s%d",section,confno);
			else
				wsprintf(sectionEx,"%s%d",section,confno);
			strcat(key, sectionEx);
		}
		else
		{
			if(!g_bIniSetting)
				wsprintf(sectionEx,"\\%s%d","Main",confno);
			else
				wsprintf(sectionEx,"%s%d","Main",confno);
			strcat(key, sectionEx);
		}
	}
	else
	{
		if(section && *section)
		{
			if(!g_bIniSetting) strcat(key, "\\");
			strcat(key, section);
		}
		else
		{
			if(g_bIniSetting) strcpy(key, "Main");
		}
	}

	if(g_bIniSetting)
	{
		char s[20];
		wsprintf(s, "%d", val);
		r = FALSE;
		if(WritePrivateProfileString(key, entry, s, g_inifile))
			r = TRUE;
	}
	else
	{
		r = FALSE;
		if(RegCreateKey(HKEY_CURRENT_USER, key, &hkey) == 0)
		{
			if(RegSetValueEx(hkey, entry, 0, REG_DWORD,
				(CONST BYTE*)&val, 4) == 0)
			{
				r = TRUE;
			}
			RegCloseKey(hkey);
		}
	}
	return r;
}

/*-------------------------------------------
�@���W�X�g���̒l���폜
---------------------------------------------*/
BOOL DelMyReg(char* section, char* entry)
{
	BOOL r;
	char key[80];
	HKEY hkey;

	if(g_bIniSetting) key[0] = 0;
	else strcpy(key, mykey);

	if(section && *section)
	{
		if(!g_bIniSetting) strcat(key, "\\");
		strcat(key, section);
	}
	else
	{
		if(g_bIniSetting) strcpy(key, "Main");
	}

	if(g_bIniSetting)
	{
		r = FALSE;
		if(WritePrivateProfileString(key, entry, NULL, g_inifile))
			r = TRUE;
	}
	else
	{
		r = FALSE;
		if(RegOpenKey(HKEY_CURRENT_USER, key, &hkey) == 0)
		{
			if(RegDeleteValue(hkey, entry) == 0)
				r = TRUE;
			RegCloseKey(hkey);
		}
	}
	return r;
}

/*-------------------------------------------
�@���W�X�g���̃L�[���폜
---------------------------------------------*/
BOOL DelMyRegKey(char* section)
{
	BOOL r;
	char key[80];

	if(g_bIniSetting) key[0] = 0;
	else strcpy(key, mykey);

	if(section && *section)
	{
		if(!g_bIniSetting) strcat(key, "\\");
		strcat(key, section);
	}
	else
	{
		if(g_bIniSetting) strcpy(key, "Main");
	}

	if(g_bIniSetting)
	{
		r = FALSE;
		if(WritePrivateProfileSection(key, NULL, g_inifile))
			r = TRUE;
	}
	else
	{
		r = FALSE;
		if(RegDeleteKey(HKEY_CURRENT_USER, key) == 0)
			r = TRUE;
	}
	return r;
}

void MoveRegToIni(char *section)
{
	char key[80], section2[80], section3[80], name[80], value[1024];
	DWORD cbName, cbValue, type;
	HKEY hkey;
	int i;

	if(!g_bIniSetting) return;

	strcpy(key, mykey);
	if(section == NULL)
	{
		strcpy(section2, "Main");
	}
	else
	{
		strcat(key, "\\"); strcat(key, section);
		strcpy(section2, section);
	}

	if(RegOpenKey(HKEY_CURRENT_USER, key, &hkey) == 0)
	{
		for(i = 0; ; i++)
		{
			cbName = 80; cbValue = 1024;
			type = 0;
			if(RegEnumValue(hkey, i, name, &cbName, 0, &type,
				(LPBYTE)value, &cbValue) != ERROR_SUCCESS) break;

			if(type == REG_DWORD)
			{
				char s[20];
				wsprintf(s, "%d", *(int *)value);
				WritePrivateProfileString(section2, name, s, g_inifile);
			}
			else if(type == REG_SZ)
			{
				value[cbValue] = 0;
				WritePrivateProfileString(section2, name, value, g_inifile);
			}
		}

		for(i = 0; ; i++)
		{
			if(RegEnumKey(hkey, i, name, cbName) != ERROR_SUCCESS)
				break;

			section3[0] = 0;
			if(section != NULL)
			{
				strcpy(section3, section); strcat(section3, "\\");
			}
			strcat(section3, name);
			MoveRegToIni(section3);
		}

		RegCloseKey(hkey);
	}
}

int DelRegAll(void)
{
	char key[80], name[80], class[80];
	DWORD cbName, cbClass;
	BOOL r;
	HKEY hkey;
	FILETIME ft;
	int i;

	if(!g_bIniSetting)
		return 0;

	strcpy(key, mykey);

	if(RegOpenKey(HKEY_CURRENT_USER, key, &hkey) == 0)
	{
		for(i = 0; ; i++)
		{
			cbName = 80; cbClass = 80;
			if(RegEnumKeyEx(hkey, i, name, &cbName, NULL, class, &cbClass, &ft) != ERROR_SUCCESS)
				break;
			strcpy(key, mykey);
			strcat(key, "\\");
			strcat(key, name);
			RegDeleteKey(HKEY_CURRENT_USER, key);
		}
		RegCloseKey(hkey);
	}

	r = FALSE;
	RegDeleteKey(HKEY_CURRENT_USER, mykey);
	if(RegDeleteKey(HKEY_CURRENT_USER, mykey2) == 0)
		r = TRUE;
	if (r)
		return 1;
	else
		return 2;
}

/*-------------------------------------------
�@�R���{�{�b�N�X�̃��X�g���ڂ̕\�������w�肷��
---------------------------------------------*/

void AdjustConboBoxDropDown(HWND hComboBox, int nDropDownCount)
{
	int nCount, nItemHeight;
	RECT rect;

	nCount = SendMessage(hComboBox, CB_GETCOUNT, 0, 0);
	if (nCount > nDropDownCount) nCount = nDropDownCount;
	if (nCount < 1) nCount = 1;

	GetWindowRect(hComboBox, &rect);
	nItemHeight = SendMessage(hComboBox, CB_GETITEMHEIGHT, 0, 0);
	SetWindowPos(hComboBox, 0, 0, 0, rect.right - rect.left,
		nItemHeight * nCount + rect.bottom - rect.top + 2,
		SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW | SWP_SHOWWINDOW);
}

// �^����ꂽ�t�@�C���������΃p�X�Ȃ��
// TClock�̃t�H���_����̐�΃p�X�ɕϊ�
PSTR CreateFullPathName(HINSTANCE hmod, PSTR fname)
{
	int len;
	int tlen;
	char szTClockPath[MAX_PATH];
	PSTR pstr;

	if (hmod == NULL) {
		return NULL;
	}
	if (fname == NULL) {
		return NULL;
	}
	if (*fname == '\0') {
		return NULL;
	}

	// \\NAME\C\path\path\filename.txt
	// C:\path\path\filename.txt
	// �ȏ�̐�΃p�X�ȊO�𑊑΃p�X�Ɣ��f����
	// ���̑O��TClock�̃p�X����f�B���N�g���Ƃ��ĕt��
	len = strlen(fname);
	if (len >= 2) {
		if ((*fname == '\\') && (*(fname + 1) == '\\')) {
			//UNC name
			return NULL;
		} else if (*(fname + 1) == ':') {
			return NULL;
		}
	}

	// TClock�̈ʒu����p�X�Ƃ��Ďw�蕶����𑊑΃p�X�Ƃ��Ēǉ�
	if (GetModuleFileName(hmod, szTClockPath, MAX_PATH) == 0) {
		return NULL;
	}
	del_title(szTClockPath);
	tlen = strlen(szTClockPath);

	pstr = malloc(tlen + len + 2);
	if (pstr == NULL) {
		return NULL;
	}
	strcpy(pstr, szTClockPath);
	add_title(pstr, fname);

	return pstr;
}
