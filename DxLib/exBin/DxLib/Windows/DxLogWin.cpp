//-----------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		Windows用ログプログラム
// 
//  	Ver 3.20c
// 
//-----------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

#include "../DxCompileConfig.h"

// インクルード ---------------------------------------------------------------
#include "DxLogWin.h"

#ifndef DX_NON_LOG

#include "../DxStatic.h"
#include "../DxLib.h"
#include "../DxLog.h"
#include "../DxBaseFunc.h"
#include "../DxMemory.h"
#include "../DxGraphics.h"
#include "DxWindow.h"
#include "DxWinAPI.h"
#include "DxGraphicsWin.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 -----------------------------------------------------------------

// 構造体宣言 -----------------------------------------------------------------

// データ定義 -----------------------------------------------------------------

// 関数宣言 -------------------------------------------------------------------

// プログラム -----------------------------------------------------------------

// ログファイルを初期化する処理の環境依存部分
extern int LogFileInitialize_PF( const wchar_t *LogFilePath )
{
	HANDLE fp ;

	SETUP_WIN_API

	// 使用するキャラクターコードが UTF8形式の場合のみ出力するファイルも UTF8形式にする
	LogData.PF.UTF8Mode = _GET_CHAR_CHARCODEFORMAT() == DX_CHARCODEFORMAT_UTF8 ? TRUE : FALSE ;

	// エラーログファイルを再作成する
	WinAPIData.Win32Func.DeleteFileWFunc( LogFilePath ) ;
	fp = WinAPIData.Win32Func.CreateFileWFunc( LogFilePath, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL ) ;

	// UTF8形式と Unicode形式で BOM が異なる
	if( LogData.PF.UTF8Mode )
	{
		BYTE BOM_UTF8[ 3 ] = { 0xef, 0xbb, 0xbf } ;
		DWORD WriteSize ;
		WinAPIData.Win32Func.WriteFileFunc( fp, &BOM_UTF8, 3, &WriteSize, NULL ) ;
	}
	else
	{
		BYTE BOM_Unicode[ 2 ] = { 0xff, 0xfe } ;
		DWORD WriteSize ;
		WinAPIData.Win32Func.WriteFileFunc( fp, &BOM_Unicode, 2, &WriteSize, NULL ) ;
	}

	if( fp != NULL )
	{
		WinAPIData.Win32Func.CloseHandleFunc( fp ) ;
	}

	// 終了
	return 0 ;
}

// ログファイルの後始末の環境依存部分
extern int LogFileTerminate_PF( void )
{
	return 0 ;
}

// ログファイルへ文字列を書き出す処理の環境依存部分
extern int LogFileAdd_WCHAR_T_PF( const wchar_t *LogFilePath, const wchar_t *ErrorStr )
{
	HANDLE	fp ;
	DWORD	WriteSize ;

	SETUP_WIN_API

	if( WinAPIData.Win32Func.WinMMDLL == NULL )
	{
		return -1 ;
	}

	// エラーログファイルを開く
	fp = WinAPIData.Win32Func.CreateFileWFunc( LogFilePath, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ) ;
	if( fp != NULL )
	{
		// ファイルアクセス位置をファイルの末端に移動
		WinAPIData.Win32Func.SetFilePointerFunc( fp, 0, NULL, FILE_END ) ;

		// エラーログファイルに書き出す
		if( LogData.PF.UTF8Mode )
		{
			// UTF8 に変換した文字列を格納するためのメモリ領域を確保
			size_t UTF8StringBufferBytes = ( _WCSLEN( ErrorStr ) + 1 ) * 8 ;
			char *UTF8StringBuffer = ( char * )DXALLOC( UTF8StringBufferBytes ) ;
			if( UTF8StringBuffer == NULL )
			{
				return -1 ;
			}

			// 文字列を UTF8 形式に変換
			int UTF8Bytes = ConvString( ( char * )ErrorStr, -1, WCHAR_T_CHARCODEFORMAT, UTF8StringBuffer, UTF8StringBufferBytes, DX_CHARCODEFORMAT_UTF8 ) ;

			// ファイルに書き出し
			if( UTF8Bytes > 1 )
			{
				WinAPIData.Win32Func.WriteFileFunc( fp, UTF8StringBuffer, ( DWORD )UTF8Bytes - 1, &WriteSize, NULL ) ;
			}

			// メモリの解放
			DXFREE( UTF8StringBuffer ) ;
			UTF8StringBuffer = NULL ;
			UTF8StringBufferBytes = 0 ;
		}
		else
		{
			WinAPIData.Win32Func.WriteFileFunc( fp, ErrorStr, ( DWORD )( _WCSLEN( ErrorStr ) * sizeof( wchar_t ) ), &WriteSize, NULL ) ;
		}

		// エラーログをアウトプットに書き出す
		WinAPIData.Win32Func.OutputDebugStringWFunc( ErrorStr ) ;

		// ファイルを閉じる
		WinAPIData.Win32Func.CloseHandleFunc( fp ) ;
	}
	
	// 終了
	return 0 ;
}

// ログ機能の初期化を行うかどうかを取得する
extern int IsInitializeLog( void )
{
	if( WinData.MainWindow == NULL )
	{
		return FALSE ;
	}

#ifndef DX_NON_GRAPHICS
	// ハードウエア機能を使っているかどうかで処理を分岐
	if( GSYS.Setting.ValidHardware )
	{
		switch( GRAWIN.Setting.UseGraphicsAPI )
		{
#ifndef DX_NON_DIRECT3D11
		case GRAPHICS_API_DIRECT3D11_WIN32 :
			return D3D11Device_IsValid() ? TRUE : FALSE ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
		case GRAPHICS_API_DIRECT3D9_WIN32 :
			return NS_GetUseDDrawObj() == NULL ? FALSE : TRUE ;
#endif // DX_NON_DIRECT3D9

		default :
			return FALSE ;
		}
	}
	else
	{
		// ＤＸライブラリが終了状態に入っていたら初期化しない
		return NS_GetUseDDrawObj() == NULL || WinData.MainWindow == NULL ? FALSE : TRUE ;
	}
#else // DX_NON_GRAPHICS
	// ＤＸライブラリが終了状態に入っていたら初期化しない
	return WinData.MainWindow == NULL ? FALSE : TRUE ;
#endif // DX_NON_GRAPHICS
}


#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_NON_LOG

