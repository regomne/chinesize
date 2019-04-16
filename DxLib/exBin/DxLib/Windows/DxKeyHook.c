// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		キーボードフックＤＬＬプログラムファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#define _WIN32_WINNT 0x0400

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

static HINSTANCE InstanceDLL;				// DLL のインスタンスハンドル
static HHOOK     KeyboardHookHandle;		// キーボードフックハンドル
//static HHOOK     GetMessageHookHandle;		// メッセージフックハンドル


// 低レベルキーボードフックプロージャ
LRESULT CALLBACK Dx_LowLevelKeyboardProc( int Code, WPARAM WParam, LPARAM LParam )
{
	/*
	char String[ 1024 ] ;

	sprintf( String, "Code:%x wParam:%x lParam:%x\n", Code, WParam, LParam ) ;
	OutputDebugString( String ) ;
	*/

	if( Code == HC_ACTION )
	{
		KBDLLHOOKSTRUCT *KeyHookStruct = ( KBDLLHOOKSTRUCT * )LParam ;

		/*
		sprintf( String, "vkCode:%x scanCode:%x flags:%x time:%d dwExtraInfo:%x\n",
			KeyHookStruct->vkCode,
			KeyHookStruct->scanCode,
			KeyHookStruct->flags,
			KeyHookStruct->dwExtraInfo ) ;
		OutputDebugString( String ) ;
		*/

		// 左Windows キー or 右Windows キー キャンセル
		if( KeyHookStruct->vkCode == 91 || KeyHookStruct->vkCode == 92 )
			return TRUE ;

		// CTRL + ESC キャンセル
		if( KeyHookStruct->vkCode == VK_ESCAPE && 
			( GetAsyncKeyState( VK_CONTROL ) >> ( ( sizeof( SHORT ) * 8 ) - 1 ) ) != 0 &&
			( GetAsyncKeyState( VK_SHIFT ) >> ( ( sizeof( SHORT ) * 8 ) - 1 ) ) == 0 )
			return TRUE ;

		/*
		// ALT + TAB キャンセル
		if( ( KeyHookStruct->flags & LLKHF_ALTDOWN ) && KeyHookStruct->vkCode == VK_TAB )
			return TRUE ;
		*/

		/*
		// ALT + ESC キャンセル
		if( ( KeyHookStruct->flags & LLKHF_ALTDOWN ) && KeyHookStruct->vkCode == VK_ESCAPE )
			return TRUE ;
		*/
	}

	// 次のフックプロージャに飛ばす
	return CallNextHookEx( KeyboardHookHandle, Code, WParam, LParam );
}
/*
// メッセージフックプロージャ
LRESULT CALLBACK Dx_MsgHookProc(int Code, WPARAM WParam, LPARAM LParam)
{
	MSG *pmsg;

	pmsg = (MSG *)LParam;
	if( pmsg->message == WM_USER + 260 )
	{
		pmsg->message = WM_NULL;
	}

	// 次のフックプロージャに飛ばす
	return CallNextHookEx( GetMessageHookHandle, Code, WParam, LParam );
}
*/
// 低レベルキーボードフックの開始を行う関数
__declspec(dllexport) BOOL SetMSGHookDll( HWND MainWindowHandle, HHOOK *pKeyboardHookHandle/*, HHOOK *pGetMessageHookHandle*/, int Enable )
{
	MainWindowHandle = MainWindowHandle;

	if( Enable )
	{
		KeyboardHookHandle = SetWindowsHookEx( WH_KEYBOARD_LL, Dx_LowLevelKeyboardProc, InstanceDLL, 0 );
	//	GetMessageHookHandle = SetWindowsHookEx( WH_GETMESSAGE, Dx_MsgHookProc, InstanceDLL, 0 );

		if( pKeyboardHookHandle )
		{
			*pKeyboardHookHandle = KeyboardHookHandle ;
		}
	/*
		if( pGetMessageHookHandle )
		{
			*pGetMessageHookHandle = GetMessageHookHandle ;
		}
	*/
		return KeyboardHookHandle == NULL /*|| GetMessageHookHandle == NULL*/ ? FALSE : TRUE ;
	}
	else
	{
		UnhookWindowsHookEx( *pKeyboardHookHandle ) ;

		return TRUE ;
	}
}

// DllMain
BOOL APIENTRY DllMain( HINSTANCE Instance, DWORD Reason, LPVOID Reserved )
{
	if( Reason == DLL_PROCESS_ATTACH )
		InstanceDLL = Instance;

	return TRUE;
}



