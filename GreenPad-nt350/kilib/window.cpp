#include "stdafx.h"
#include "app.h"
#include "window.h"
using namespace ki;



//=========================================================================
// IMEに関するあれこれ
//=========================================================================

IMEManager* IMEManager::pUniqueInstance_;

IMEManager::IMEManager()
#ifdef USEGLOBALIME
	: immApp_( NULL )
	, immMsg_( NULL )
#endif
{
	#ifdef USEGLOBALIME
		// 色々面倒なのでWin95ではGlobalIME無し
		if( !app().isWin95() )
		{
			app().InitModule( App::OLE );
			if( S_OK == ::CoCreateInstance(
					CLSID_CActiveIMM, NULL, CLSCTX_INPROC_SERVER,
					IID_IActiveIMMApp, (void**)&immApp_ ) )
			{
				immApp_->QueryInterface(
					IID_IActiveIMMMessagePumpOwner, (void**)&immMsg_ );
			}
		}
	#endif

	// 唯一のインスタンスは私です
	pUniqueInstance_ = this;
}

IMEManager::~IMEManager()
{
	#ifdef USEGLOBALIME
		if( immMsg_ != NULL )
			immMsg_->Release();
		if( immApp_ != NULL )
			immApp_->Release();
	#endif
}

void IMEManager::EnableGlobalIME( bool enable )
{
	#ifdef USEGLOBALIME
		if( immApp_ )
			if( enable ) immApp_->Activate( TRUE );
			else         immApp_->Deactivate();
	#endif
}

void IMEManager::FilterWindows( ATOM* lst, UINT siz )
{
	#ifdef USEGLOBALIME
		if( immApp_ )
			immApp_->FilterClientWindows( lst, siz );
	#endif
}

inline void IMEManager::TranslateMsg( MSG* msg )
{
	#ifdef USEGLOBALIME
		if( immMsg_ )
			if( S_OK == immMsg_->OnTranslateMessage( msg ) )
				return;
	#endif
	::TranslateMessage( msg );
}

inline LRESULT IMEManager::DefProc( HWND h, UINT m, WPARAM w, LPARAM l )
{
	#ifdef USEGLOBALIME
		if( immApp_ )
		{
			LRESULT res;
			if( S_OK == immApp_->OnDefWindowProc( h,m,w,l,&res ) )
				return res;
		}
	#endif
	return ::DefWindowProc( h, m, w, l );
}

inline void IMEManager::MsgLoopBegin()
{
	#ifdef USEGLOBALIME
		if( immMsg_ )
			immMsg_->Start();
	#endif
}

inline void IMEManager::MsgLoopEnd()
{
	#ifdef USEGLOBALIME
		if( immMsg_ )
			immMsg_->End();
	#endif
}

void IMEManager::SetFont( HWND wnd, const LOGFONT& lf )
{
	HIMC ime;
	LOGFONT* plf = const_cast<LOGFONT*>(&lf);

	#ifdef USEGLOBALIME
	if( immApp_ )
	{
		immApp_->GetContext( wnd, &ime );
		#ifdef _UNICODE
			immApp_->SetCompositionFontW( ime, plf );
		#else
			immApp_->SetCompositionFontA( ime, plf );
		#endif
		immApp_->ReleaseContext( wnd, ime );
	}
	else
	#endif
	{
		ime = ::ImmGetContext( wnd );
		::ImmSetCompositionFont( ime, plf );
		::ImmReleaseContext( wnd, ime );
	}
}

void IMEManager::SetPos( HWND wnd, int x, int y )
{
	HIMC ime;
	COMPOSITIONFORM cf;
	cf.dwStyle = CFS_POINT;
	cf.ptCurrentPos.x  = x;
	cf.ptCurrentPos.y  = y;

	#ifdef USEGLOBALIME
	if( immApp_ )
	{
		immApp_->GetContext( wnd, &ime );
		immApp_->SetCompositionWindow( ime, &cf );
		immApp_->ReleaseContext( wnd, ime );
	}
	else
	#endif
	{
		ime = ::ImmGetContext( wnd );
		::ImmSetCompositionWindow( ime, &cf );
		::ImmReleaseContext( wnd, ime );
	}
}

void IMEManager::GetString( HWND wnd, unicode** str, ulong* len )
{
	*str = NULL;
	HIMC ime;

	#ifdef USEGLOBALIME
	if( immApp_ )
	{
		long s=0;
		immApp_->GetContext( wnd, &ime );
		immApp_->GetCompositionStringW( ime, GCS_RESULTSTR, 0, &s, NULL );
		*str = new unicode[ (*len=s/2)+1 ];
		immApp_->GetCompositionStringW( ime, GCS_RESULTSTR, s, &s, *str );
		immApp_->ReleaseContext( wnd, ime );
	}
	else
	#endif
	{
		ime = ::ImmGetContext( wnd );
		long s = ::ImmGetCompositionStringW( ime,GCS_RESULTSTR,NULL,0 );

		#ifndef _UNICODE
			if( s <= 0 )
			{
				s = ::ImmGetCompositionStringA(ime,GCS_RESULTSTR,NULL,0);
				if( s > 0 )
				{
					char* tmp = new char[s];
					*str = new unicode[*len=s*2];
					::ImmGetCompositionStringA( ime,GCS_RESULTSTR,tmp,s );
					*len = ::MultiByteToWideChar(
						CP_ACP, MB_PRECOMPOSED, tmp, s, *str, *len );
					delete [] tmp;
				}
			}
			else
		#endif
			{
				*str = new unicode[ (*len=s/2)+1 ];
				::ImmGetCompositionStringW( ime, GCS_RESULTSTR, *str, s );
			}

		::ImmReleaseContext( wnd, ime );
	}
}



//=========================================================================
// Windowに関するあれこれ
//=========================================================================

Window::Window()
	: wnd_      (NULL)
	, isLooping_(false)
{
}

void Window::SetHwnd( HWND wnd )
{
	wnd_ = wnd;
}

void Window::MsgLoop()
{
	// thisをメインウインドウとして、
	// メインメッセージループを回す
	isLooping_ = true;
	ime().MsgLoopBegin();

	for( MSG msg; ::GetMessage( &msg, NULL, 0, 0 ); )
		if( !PreTranslateMessage( &msg ) )
		{
			ime().TranslateMsg( &msg );
			::DispatchMessage( &msg );
		}

	ime().MsgLoopEnd();
	isLooping_ = false;
}

void Window::ProcessMsg()
{
	// こっちはグローバル関数。
	// 未処理メッセージを一掃

	ime().MsgLoopBegin();
	for( MSG msg; ::PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ); )
	{
		ime().TranslateMsg( &msg );
		::DispatchMessage( &msg );
	}
	ime().MsgLoopEnd();
}



//-------------------------------------------------------------------------

void Window::SetCenter( HWND hwnd, HWND rel )
{
	// 自分のサイズを取得
	RECT rc,pr;
	::GetWindowRect( hwnd, &rc );

	// 親の位置、ないしは全画面の位置を取得
	if( rel != NULL )
		::GetWindowRect( rel, &pr );
	else
		::SystemParametersInfo( SPI_GETWORKAREA, 0, &pr, 0 );

	// 中央を計算
	::SetWindowPos( hwnd, 0,
		pr.left + ( (pr.right-pr.left)-(rc.right-rc.left) )/2,
		pr.top  + ( (pr.bottom-pr.top)-(rc.bottom-rc.top) )/2,
		0, 0, SWP_NOSIZE|SWP_NOZORDER );
}

void Window::SetFront( HWND hwnd )
{
	// kazubon氏の TClock のソースを参考にしました。感謝！

	if( app().isNewTypeWindows() )
	{
		DWORD pid;
		HWND  fore= ::GetForegroundWindow();
		DWORD th1 = ::GetWindowThreadProcessId( fore, &pid );
		DWORD th2 = ::GetCurrentThreadId();
		::AttachThreadInput( th2, th1, TRUE );
		::SetForegroundWindow( hwnd );
		::AttachThreadInput( th2, th1, FALSE );
		::BringWindowToTop( hwnd );
	}
	else
	{
		::SetForegroundWindow( hwnd );
	}
}

//=========================================================================

WndImpl::WndImpl( LPCTSTR className, DWORD style, DWORD styleEx )
	: className_( className )
	, style_    ( style )
	, styleEx_  ( styleEx )
	, thunk_    ( static_cast<byte*>(
	                ::VirtualAlloc( NULL, THUNK_SIZE, MEM_COMMIT, PAGE_EXECUTE_READWRITE )) )
{
}

WndImpl::~WndImpl()
{
	// ウインドウを破棄し忘れてたら閉じる
	// …が、この時点で既に vtable は破棄されかかっているので
	// 正しい on_destroy が呼ばれる保証は全くない。あくまで
	// 緊急脱出用(^^; と考えること。
	Destroy();
	::VirtualFree( thunk_, 0, MEM_RELEASE );
}

void WndImpl::Destroy()
{
	if( hwnd() != NULL )
		::DestroyWindow( hwnd() );
}

ATOM WndImpl::Register( WNDCLASS* cls )
{
	// WndImpl派生クラスで使うWndClassを登録。
	// プロシージャはkilib謹製のものに書き換えちゃいます。
//	cls->cbSize      = sizeof(WNDCLASSEX);
	cls->hInstance   = app().hinst();
	cls->lpfnWndProc = StartProc;
	return ::RegisterClass( cls );
}

struct ThisAndParam
{
	// ユーザ指定のパラメータ以外にも渡したいモノが少々…
	WndImpl* pThis;
	void*   pParam;
};

bool WndImpl::Create(
	LPCTSTR wndName, HWND parent, int x, int y, int w, int h, void* param )
{
	// ここでthisポインタを忍び込ませておく
	ThisAndParam z = { this, param };

	LOGGER("WndImpl::Create before CreateWindowEx API call");

	return (NULL != ::CreateWindowEx(
		styleEx_, className_, wndName, style_,
		x, y, w, h, parent, NULL, app().hinst(), &z
	));
}



//-------------------------------------------------------------------------

LRESULT CALLBACK WndImpl::StartProc(
	HWND wnd, UINT msg, WPARAM wp, LPARAM lp )
{
	// WM_CREATE以外はスルーの方針で
	if( msg != WM_CREATE )
		return ::DefWindowProc( wnd, msg, wp, lp );

	LOGGER("WndImpl::StartProc WM_CREATE kitaaaaa!!");

	// 忍ばせて置いたthisポインタを取り出し
	CREATESTRUCT* cs   = reinterpret_cast<CREATESTRUCT*>(lp);
	ThisAndParam* pz   = static_cast<ThisAndParam*>(cs->lpCreateParams);
	WndImpl*   pThis   = pz->pThis;
	cs->lpCreateParams = pz->pParam;

	// サンク
	pThis->SetUpThunk( wnd );

	// WM_CREATE用メッセージを呼ぶ
	pThis->on_create( cs );
	return 0;
}

void WndImpl::SetUpThunk( HWND wnd )
{
	SetHwnd( wnd );

	// ここで動的にx86の命令列
	//   | mov dword ptr [esp+4] this
	//   | jmp MainProc
	// あるいはAMD64の命令列
	//   | mov rcx this
	//   | mov rax MainProc
	//   | jmp rax
	// を生成し、メッセージプロシージャとして差し替える。
	//
	// これで次回からは、第一引数が hwnd のかわりに
	// thisポインタになった状態でMainProcが呼ばれる
	// …と見なしたプログラムが書ける。
	//
	// 参考資料：ATLのソース

#ifdef _M_AMD64
	*reinterpret_cast<dbyte*>   (thunk_+ 0) = 0xb948;
	*reinterpret_cast<WndImpl**>(thunk_+ 2) = this;
	*reinterpret_cast<dbyte*>   (thunk_+10) = 0xb848;
	*reinterpret_cast<void**>   (thunk_+12) = MainProc;
	*reinterpret_cast<dbyte*>   (thunk_+20) = 0xe0ff;
#else
	*reinterpret_cast<qbyte*>   (thunk_+0) = 0x042444C7;
	*reinterpret_cast<WndImpl**>(thunk_+4) = this;
	*reinterpret_cast< byte*>   (thunk_+8) = 0xE9;
	*reinterpret_cast<qbyte*>   (thunk_+9) =
		reinterpret_cast<byte*>((void*)MainProc)-(thunk_+13);
#endif

	::FlushInstructionCache( ::GetCurrentProcess(), thunk_, THUNK_SIZE );
	::SetWindowLongPtr( wnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&thunk_[0]) );
}

LRESULT CALLBACK WndImpl::MainProc(
	WndImpl* ptr, UINT msg, WPARAM wp, LPARAM lp )
{
	if( msg == WM_COMMAND )
	{
		if( !ptr->on_command( LOWORD(wp), (HWND)lp ) )
			return ::DefWindowProc( ptr->hwnd(), msg, wp, lp );
	}
	else if( msg == WM_DESTROY )
	{
		ptr->on_destroy();
		::SetWindowLongPtr( ptr->hwnd(), GWLP_WNDPROC,
			reinterpret_cast<LONG_PTR>(StartProc) );
		if( ptr->isMainWnd() )
			::PostQuitMessage( 0 );
		ptr->SetHwnd(NULL);
	}
	else
	{
		return ptr->on_message( msg, wp, lp );
	}

	return 0;
}



//-------------------------------------------------------------------------

void WndImpl::on_create( CREATESTRUCT* cs )
{
	// 何もしない
}

void WndImpl::on_destroy()
{
	// 何もしない
}

bool WndImpl::on_command( UINT, HWND )
{
	// 何もしない
	return false;
}

LRESULT WndImpl::on_message( UINT msg, WPARAM wp, LPARAM lp )
{
	// 何もしない
	return ime().DefProc( hwnd(), msg, wp, lp );
}

bool WndImpl::PreTranslateMessage( MSG* )
{
	// 何もしない
	return false;
}



//=========================================================================

DlgImpl::DlgImpl( UINT id )
	: rsrcID_( id )
{
}

DlgImpl::~DlgImpl()
{
	// ウインドウを破棄し忘れてたら閉じる
	if( hwnd() != NULL )
		End( IDCANCEL );
}

void DlgImpl::End( UINT code )
{
	endCode_ = code;

	if( type() == MODAL )
		::EndDialog( hwnd(), code );
	else
		::DestroyWindow( hwnd() );
}

void DlgImpl::GoModal( HWND parent )
{
	type_ = MODAL;
	::DialogBoxParam( app().hinst(), MAKEINTRESOURCE(rsrcID_), parent,
		(DLGPROC)MainProc, reinterpret_cast<LPARAM>(this) );
}

void DlgImpl::GoModeless( HWND parent )
{
	type_ = MODELESS;
	::CreateDialogParam( app().hinst(), MAKEINTRESOURCE(rsrcID_), parent,
		(DLGPROC)MainProc, reinterpret_cast<LPARAM>(this) );
}



//-------------------------------------------------------------------------

BOOL CALLBACK DlgImpl::MainProc(
	HWND dlg, UINT msg, WPARAM wp, LPARAM lp )
{
	if( msg == WM_INITDIALOG )
	{
		::SetWindowLongPtr( dlg, GWLP_USERDATA, lp );

		DlgImpl* ptr = reinterpret_cast<DlgImpl*>(lp);
		ptr->SetHwnd( dlg );
		ptr->on_init();
		return FALSE;
	}

	DlgImpl* ptr =
		reinterpret_cast<DlgImpl*>(::GetWindowLongPtr(dlg,GWLP_USERDATA));

	if( ptr != NULL )
		switch( msg )
		{
		case WM_COMMAND:
			switch( LOWORD(wp) )
			{
			case IDOK:
				if( ptr->on_ok() )
					ptr->End( IDOK );
				return TRUE;

			case IDCANCEL:
				if( ptr->on_cancel() )
					ptr->End( IDCANCEL );
				return TRUE;

			default:
				return ptr->on_command( HIWORD(wp), LOWORD(wp),
					reinterpret_cast<HWND>(lp) ) ? TRUE : FALSE;
			}

		case WM_DESTROY:
			ptr->on_destroy();
			if( ptr->isMainWnd() )
				::PostQuitMessage( 0 );
			ptr->SetHwnd(NULL);
			break;

		default:
			return ptr->on_message( msg, wp, lp ) ? TRUE : FALSE;
		}

	return FALSE;
}



//-------------------------------------------------------------------------

void DlgImpl::on_init()
{
	// 何もしない
}

void DlgImpl::on_destroy()
{
	// 何もしない
}

bool DlgImpl::on_ok()
{
	// 何もしない
	return true;
}

bool DlgImpl::on_cancel()
{
	// 何もしない
	return true;
}

bool DlgImpl::on_command( UINT, UINT, HWND )
{
	// 何もしない
	return false;
}

bool DlgImpl::on_message( UINT, WPARAM, LPARAM )
{
	// 何もしない
	return false;
}

bool DlgImpl::PreTranslateMessage( MSG* msg )
{
	// モードレスの時用。ダイアログメッセージ処理。
	return (FALSE != ::IsDialogMessage( hwnd(), msg ));
}
