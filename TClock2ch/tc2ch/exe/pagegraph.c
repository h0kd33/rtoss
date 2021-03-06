/*-------------------------------------------
  pagegraph.c
  "graph" page of properties
---------------------------------------------*/

#include "tclock.h"

static void OnInit(HWND hDlg);
static void OnApply(HWND hDlg);
static void OnTrayGraph(HWND hDlg);
static void OnGraphMode(HWND hDlg);
static void InitGraphMode(HWND hDlg);
static void InitGraphType(HWND hDlg);
static void SetColorFromBmp(HWND hDlg, int idCombo, char* fname);

static COMBOCOLOR combocolor[4] = {
	{ IDC_COLSEND, "BackNetColSend", RGB(192, 192, 255) }	,
	{ IDC_COLSR, "BackNetColSR", RGB(255, 224, 255) }		,
	{ IDC_COLRECV, "BackNetColRecv", RGB(255, 192, 192) }
};

__inline void SendPSChanged(HWND hDlg)
{
	g_bApplyTaskbar = TRUE;
	SendMessage(GetParent(hDlg), PSM_CHANGED, (WPARAM)(hDlg), 0);
}

/*------------------------------------------------
   dialog procedure of this page
--------------------------------------------------*/
BOOL CALLBACK PageGraphProc(HWND hDlg, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_INITDIALOG:
			OnInit(hDlg);
			return TRUE;
		case WM_MEASUREITEM:
			OnMeasureItemColorCombo(lParam);
			return TRUE;
		case WM_DRAWITEM:
			OnDrawItemColorCombo(lParam, 16);
			return TRUE;
		case WM_COMMAND:
		{
			WORD id, code;
			id = LOWORD(wParam); code = HIWORD(wParam);
			switch(id)
			{
			//時計に通信状況グラフを表示する
			case IDC_TRAYGRAPH:
				OnTrayGraph(hDlg);
				break;
			case IDC_GRAPHTYPE:
				if(code == CBN_SELCHANGE)SendPSChanged(hDlg);
			case IDC_GRAPHMODE:
				if(code == CBN_SELCHANGE)
				{
					OnGraphMode(hDlg);
					SendPSChanged(hDlg);
				}
				break;
			// コンボボックス
			case IDC_COLRECV:
			case IDC_COLSEND:
			case IDC_COLSR:
				if(code == CBN_SELCHANGE)
					SendPSChanged(hDlg);
				break;
			//「...」色の選択
			case IDC_CHOOSECOLRECV:
			case IDC_CHOOSECOLSEND:
			case IDC_CHOOSECOLSR:
				OnChooseColor(hDlg, id, 16);
				SendPSChanged(hDlg);
				break;
			case IDC_TBGGRAPHRATE:
			case IDC_TBGGRAPHRATE2:
			case IDC_GRAPHLEFT:
			case IDC_SPGRAPHLEFT:
			case IDC_GRAPHTOP:
			case IDC_SPGRAPHTOP:
			case IDC_GRAPHRIGHT:
			case IDC_SPGRAPHRIGHT:
			case IDC_GRAPHBOTTOM:
			case IDC_SPGRAPHBOTTOM:
			case IDC_GINTERVALSPIN:
			case IDC_GINTERVAL:
			case IDC_CPUHIGHSPIN:
			case IDC_CPUHIGH:
			case IDC_LOGGRAPH:
			case IDC_GPTATE:
			case IDC_RVGRAPH:
				SendPSChanged(hDlg);
				g_bApplyClock = TRUE;
				break;
			}
			return TRUE;
		}
		case WM_NOTIFY:
			switch(((NMHDR *)lParam)->code)
			{
				case PSN_APPLY: OnApply(hDlg); break;
				case PSN_HELP: My2chHelp(GetParent(hDlg), 7); break;
			}
			return TRUE;
	}
	return FALSE;
}

/*------------------------------------------------
  initialize
--------------------------------------------------*/
void OnInit(HWND hDlg)
{
	DWORD dw;
	int n;

	InitGraphMode(hDlg);
	InitGraphType(hDlg);

	n = GetMyRegLong("", "GraphMode", 1);
	CBSetCurSel(hDlg, IDC_GRAPHMODE, (n-1));

	n = GetMyRegLong("", "GraphType", 1);
	CBSetCurSel(hDlg, IDC_GRAPHTYPE, (n-1));

	CheckDlgButton(hDlg, IDC_LOGGRAPH,
		GetMyRegLong("", "LogGraph", FALSE));

	CheckDlgButton(hDlg, IDC_GPTATE,
		GetMyRegLong("", "GraphTate", FALSE));

	CheckDlgButton(hDlg, IDC_RVGRAPH,
		GetMyRegLong("", "ReverseGraph", FALSE));

	CheckDlgButton(hDlg, IDC_TRAYGRAPH,
		GetMyRegLong("", "BackNet", FALSE));

	OnTrayGraph(hDlg);

	SetDlgItemInt(hDlg, IDC_TBGGRAPHRATE, GetMyRegLong("", "BackNetBand", 100), FALSE);
	SetDlgItemInt(hDlg, IDC_TBGGRAPHRATE2, GetMyRegLong("", "BackNetBand2", 100), FALSE);

	dw = GetMyRegLong(NULL, "GraphInterval", 1);
	if(dw > 60) dw = 60;
	if(dw < 1  ) dw = 1;
	SendDlgItemMessage(hDlg,IDC_GINTERVALSPIN,UDM_SETRANGE,0,
		(LPARAM) MAKELONG((short)60, (short)1));
	SendDlgItemMessage(hDlg, IDC_GINTERVALSPIN, UDM_SETPOS, 0,
		(int)(short)dw);

	dw = GetMyRegLong(NULL, "CpuHigh", 70);
	if(dw > 100) dw = 100;
	if(dw < 1  ) dw = 1;
	SendDlgItemMessage(hDlg,IDC_CPUHIGHSPIN,UDM_SETRANGE,0,
		(LPARAM) MAKELONG((short)100, (short)1));
	SendDlgItemMessage(hDlg, IDC_CPUHIGHSPIN, UDM_SETPOS, 0,
		(int)(short)dw);

	dw = GetMyRegLong(NULL, "GraphLeft", 0);
	if(dw > 1000) dw = 1000;
	if(dw < 0  ) dw = 0;
	SendDlgItemMessage(hDlg,IDC_SPGRAPHLEFT,UDM_SETRANGE,0,
		(LPARAM) MAKELONG((short)1000, (short)0));
	SendDlgItemMessage(hDlg, IDC_SPGRAPHLEFT, UDM_SETPOS, 0,
		(int)(short)dw);

	dw = GetMyRegLong(NULL, "GraphTop", 0);
	if(dw > 1000) dw = 1000;
	if(dw < 0  ) dw = 0;
	SendDlgItemMessage(hDlg,IDC_SPGRAPHTOP,UDM_SETRANGE,0,
		(LPARAM) MAKELONG((short)1000, (short)0));
	SendDlgItemMessage(hDlg, IDC_SPGRAPHTOP, UDM_SETPOS, 0,
		(int)(short)dw);

	dw = GetMyRegLong(NULL, "GraphRight", 0);
	if(dw > 1000) dw = 1000;
	if(dw < 0  ) dw = 0;
	SendDlgItemMessage(hDlg,IDC_SPGRAPHRIGHT,UDM_SETRANGE,0,
		(LPARAM) MAKELONG((short)1000, (short)0));
	SendDlgItemMessage(hDlg, IDC_SPGRAPHRIGHT, UDM_SETPOS, 0,
		(int)(short)dw);

	dw = GetMyRegLong(NULL, "GraphBottom", 0);
	if(dw > 1000) dw = 1000;
	if(dw < 0  ) dw = 0;
	SendDlgItemMessage(hDlg,IDC_SPGRAPHBOTTOM,UDM_SETRANGE,0,
		(LPARAM) MAKELONG((short)1000, (short)0));
	SendDlgItemMessage(hDlg, IDC_SPGRAPHBOTTOM, UDM_SETPOS, 0,
		(int)(short)dw);

//	InitColor(hDlg);
	InitComboColor(hDlg, 4, combocolor, 16, FALSE);
	OnGraphMode(hDlg);
}

/*------------------------------------------------
  apply
--------------------------------------------------*/
void OnApply(HWND hDlg)
{
	char s[20];
	DWORD dw;
	int n;

	dw = CBGetItemData(hDlg, IDC_COLSEND, CBGetCurSel(hDlg, IDC_COLSEND));
	SetMyRegLong("", "BackNetColSend", dw);
	dw = CBGetItemData(hDlg, IDC_COLSR, CBGetCurSel(hDlg, IDC_COLSR));
	SetMyRegLong("", "BackNetColSR", dw);
	dw = CBGetItemData(hDlg, IDC_COLRECV, CBGetCurSel(hDlg, IDC_COLRECV));
	SetMyRegLong("", "BackNetColRecv", dw);

	SetMyRegLong("", "BackNet",
		IsDlgButtonChecked(hDlg, IDC_TRAYGRAPH));
	SetMyRegLong("", "LogGraph",
		IsDlgButtonChecked(hDlg, IDC_LOGGRAPH));
	SetMyRegLong("", "GraphTate",
		IsDlgButtonChecked(hDlg, IDC_GPTATE));
	SetMyRegLong("", "ReverseGraph",
		IsDlgButtonChecked(hDlg, IDC_RVGRAPH));

	dw = GetDlgItemInt(hDlg, IDC_GINTERVAL, NULL, FALSE);
	if(dw > 60) dw = 60;
	if(dw < 1  ) dw = 1;
	SetDlgItemInt(hDlg, IDC_GINTERVAL, dw, FALSE);
	SetMyRegLong("", "GraphInterval", dw);

	dw = GetDlgItemInt(hDlg, IDC_CPUHIGH, NULL, FALSE);
	if(dw > 100) dw = 100;
	if(dw < 1  ) dw = 1;
	SetDlgItemInt(hDlg, IDC_CPUHIGH, dw, FALSE);
	SetMyRegLong("", "CpuHigh", dw);

	GetDlgItemText(hDlg, IDC_TBGGRAPHRATE, s, 20);
	SetMyRegLong("", "BackNetBand", atoi(s));

	GetDlgItemText(hDlg, IDC_TBGGRAPHRATE2, s, 20);
	SetMyRegLong("", "BackNetBand2", atoi(s));

	n = CBGetCurSel(hDlg, IDC_GRAPHMODE);
	SetMyRegLong("", "GraphMode", (n+1));

	n = CBGetCurSel(hDlg, IDC_GRAPHTYPE);
	SetMyRegLong("", "GraphType", (n+1));

	dw = GetDlgItemInt(hDlg, IDC_GRAPHLEFT, NULL, FALSE);
	if(dw > 1000) dw = 1000;
	if(dw < 0  ) dw = 0;
	SetDlgItemInt(hDlg, IDC_GRAPHLEFT, dw, FALSE);
	SetMyRegLong("", "GraphLeft", dw);

	dw = GetDlgItemInt(hDlg, IDC_GRAPHTOP, NULL, FALSE);
	if(dw > 1000) dw = 1000;
	if(dw < 0  ) dw = 0;
	SetDlgItemInt(hDlg, IDC_GRAPHTOP, dw, FALSE);
	SetMyRegLong("", "GraphTop", dw);

	dw = GetDlgItemInt(hDlg, IDC_GRAPHRIGHT, NULL, FALSE);
	if(dw > 1000) dw = 1000;
	if(dw < 0  ) dw = 0;
	SetDlgItemInt(hDlg, IDC_GRAPHRIGHT, dw, FALSE);
	SetMyRegLong("", "GraphRight", dw);

	dw = GetDlgItemInt(hDlg, IDC_GRAPHBOTTOM, NULL, FALSE);
	if(dw > 1000) dw = 1000;
	if(dw < 0  ) dw = 0;
	SetDlgItemInt(hDlg, IDC_GRAPHBOTTOM, dw, FALSE);
	SetMyRegLong("", "GraphBottom", dw);

}

/*------------------------------------------------
　「時計に通信状況グラフを表示する」
--------------------------------------------------*/
void OnTrayGraph(HWND hDlg)
{
	BOOL b;
	int i;
	HDC hdc;

	b = IsDlgButtonChecked(hDlg, IDC_TRAYGRAPH);
	for(i = IDC_GRAPHMODE; i <= IDC_MODE26; i++)
		EnableDlgItem(hDlg, i, b);
	SendPSChanged(hDlg);

	//環境が256色以下のときは、色の選択を無効に
	hdc = CreateIC("DISPLAY", NULL, NULL, NULL);
	if(GetDeviceCaps(hdc, BITSPIXEL) <= 8)
	{
		EnableDlgItem(hDlg, IDC_CHOOSECOLSEND, FALSE);
		EnableDlgItem(hDlg, IDC_CHOOSECOLSR, FALSE);
		EnableDlgItem(hDlg, IDC_CHOOSECOLRECV, FALSE);
	}
	DeleteDC(hdc);
	OnGraphMode(hDlg);
}

void OnGraphMode(HWND hDlg)
{
	int n, i;

	n = CBGetCurSel(hDlg, IDC_GRAPHMODE);
	n++;
	if (n == 1)
	{
		for(i=IDC_MODE21;i<=IDC_MODE24;i++)
			ShowDlgItem(hDlg, i, FALSE);
		for(i=IDC_MODE11;i<=IDC_MODE17;i++)
			ShowDlgItem(hDlg, i, TRUE);
		ShowDlgItem(hDlg, IDC_COLSEND, TRUE);
		ShowDlgItem(hDlg, IDC_CHOOSECOLSEND, TRUE);
		ShowDlgItem(hDlg, IDC_CPUHIGH, FALSE);
		ShowDlgItem(hDlg, IDC_CPUHIGHSPIN, FALSE);
		ShowDlgItem(hDlg, IDC_TBGGRAPHRATE, TRUE);
		ShowDlgItem(hDlg, IDC_TBGGRAPHRATE2, TRUE);
	}
	else if (n == 2)
	{
		for(i=IDC_MODE21;i<=IDC_MODE24;i++)
			ShowDlgItem(hDlg, i, TRUE);
		for(i=IDC_MODE11;i<=IDC_MODE17;i++)
			ShowDlgItem(hDlg, i, FALSE);
		ShowDlgItem(hDlg, IDC_COLSEND, FALSE);
		ShowDlgItem(hDlg, IDC_CHOOSECOLSEND, FALSE);
		ShowDlgItem(hDlg, IDC_CPUHIGH, TRUE);
		ShowDlgItem(hDlg, IDC_CPUHIGHSPIN, TRUE);
		ShowDlgItem(hDlg, IDC_TBGGRAPHRATE, FALSE);
		ShowDlgItem(hDlg, IDC_TBGGRAPHRATE2, FALSE);
	}
}

/*------------------------------------------------
   Select "Color" combo box automatically.
--------------------------------------------------*/
#define WIDTHBYTES(i) ((i+31)/32*4)

void SetColorFromBmp(HWND hDlg, int idCombo, char* fname)
{
	HFILE hf;
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bmih;
	int numColors;
	BYTE pixel[3];
	COLORREF col;
	int i, index;
	HDC hdc;

	hf = _lopen(fname, OF_READ);
	if(hf == HFILE_ERROR) return;

	if(_lread(hf, &bmfh, sizeof(bmfh)) != sizeof(bmfh) ||
		bmfh.bfType != *(WORD*)"BM" ||
		_lread(hf, &bmih, sizeof(bmih)) != sizeof(bmih) ||
		bmih.biSize != sizeof(bmih) ||
		bmih.biCompression != BI_RGB ||
		!(bmih.biBitCount <= 8 || bmih.biBitCount == 24))
	{
		_lclose(hf); return;
	}
	numColors = bmih.biClrUsed;
	if(numColors == 0)
	{
		if(bmih.biBitCount <= 8) numColors = 1 << bmih.biBitCount;
		else numColors = 0;
	}
	if(numColors > 0 &&
		_llseek(hf, sizeof(RGBQUAD)*numColors, FILE_CURRENT) == HFILE_ERROR)
	{
		_lclose(hf); return;
	}
	if(_llseek(hf,
			WIDTHBYTES(bmih.biWidth*bmih.biBitCount)*(bmih.biHeight-1),
			FILE_CURRENT) == HFILE_ERROR ||
		_lread(hf, pixel, sizeof(pixel)) != sizeof(pixel))
	{
		_lclose(hf); return;
	}
	if(bmih.biBitCount < 24)
	{
		index = -1;
		if(bmih.biBitCount == 8) index = pixel[0];
		else if(bmih.biBitCount == 4)
			index = (pixel[0] & 0xF0) >> 4;
		else if(bmih.biBitCount == 1)
			index = (pixel[0] & 0x80) >> 7;
		if(_llseek(hf, sizeof(bmfh)+sizeof(bmih)+sizeof(RGBQUAD)*index,
			FILE_BEGIN) == HFILE_ERROR ||
			_lread(hf, pixel, sizeof(pixel)) != sizeof(pixel))
		{
			index = -1;
		}
	}
	_lclose(hf);
	if(index == -1) return;
	col = RGB(pixel[2], pixel[1], pixel[0]);

	for(i = 0; i < 16; i++)
	{
		if(col == (COLORREF)CBGetItemData(hDlg, idCombo, i)) break;
	}
	if(i == 16)
	{
		int screencolor;
		hdc = CreateIC("DISPLAY", NULL, NULL, NULL);
		screencolor = GetDeviceCaps(hdc, BITSPIXEL);
		DeleteDC(hdc);
		if(screencolor <= 8) return;

		if(CBGetCount(hDlg, idCombo) == 16)
			CBAddString(hDlg, idCombo, col);
		else CBSetItemData(hDlg, idCombo, 16, col);
	}
	CBSetCurSel(hDlg, idCombo, i);
}

void InitGraphMode(HWND hDlg)
{
	int index;

	index = CBAddString(hDlg, IDC_GRAPHMODE, (LPARAM)MyString(IDS_GMNET));
	CBSetItemData(hDlg, IDC_GRAPHMODE, index, 1);
	index = CBAddString(hDlg, IDC_GRAPHMODE, (LPARAM)MyString(IDS_GMCPU));
	CBSetItemData(hDlg, IDC_GRAPHMODE, index, 2);
	//リスト項目の表示数を指定
	AdjustDlgConboBoxDropDown(hDlg, IDC_GRAPHMODE, 2);
}

void InitGraphType(HWND hDlg)
{
	int index;

	index = CBAddString(hDlg, IDC_GRAPHTYPE, (LPARAM)MyString(IDS_GTBAR));
	CBSetItemData(hDlg, IDC_GRAPHTYPE, index, 1);
	index = CBAddString(hDlg, IDC_GRAPHTYPE, (LPARAM)MyString(IDS_GTLINE));
	CBSetItemData(hDlg, IDC_GRAPHTYPE, index, 2);
	//リスト項目の表示数を指定
	AdjustDlgConboBoxDropDown(hDlg, IDC_GRAPHTYPE, 2);
}
