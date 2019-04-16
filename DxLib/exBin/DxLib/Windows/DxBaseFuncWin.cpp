//-----------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		Windows用標準関数の互換関数プログラム
// 
//  	Ver 3.20c
// 
//-----------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

// インクルード ---------------------------------------------------------------
#include "DxBaseFuncWin.h"
#include "DxWinAPI.h"
#include "../DxBaseFunc.h"
#include "../DxChar.h"
#include "../DxLog.h"

#if !defined(CINTERFACE) && defined(__c2__) &&  __clang_major__ == 3 && __clang_minor__ == 8
//To avoid compile error
//C:\Program Files (x86)\Windows Kits\8.1\Include\um\combaseapi.h(229,21): error : unknown type name 'IUnknown'
//          static_cast<IUnknown*>(*pp);    // make sure everyone derives from IUnknown
#define CINTERFACE
#endif
#include <windows.h>

#include <float.h>
#include <math.h>

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 -----------------------------------------------------------------

// 構造体宣言 -----------------------------------------------------------------

// データ定義 -----------------------------------------------------------------

// 関数宣言 -------------------------------------------------------------------

// プログラム -----------------------------------------------------------------

extern void	_SET_DEFAULT_CHARCODEFORMAT( void )
{
	if( _GET_CHARSET() == 0 )
	{
		SETUP_WIN_API

		_SET_WCHAR_T_CHARCODEFORMAT( sizeof( wchar_t ) > 2 ? DX_CHARCODEFORMAT_UTF32LE : DX_CHARCODEFORMAT_UTF16LE ) ;

		switch( WinAPIData.Win32Func.GetOEMCPFunc() )
		{
		case DX_CHARCODEFORMAT_UHC :				_SET_CHARSET( DX_CHARSET_HANGEUL         ) ; _SET_CHAR_CHARCODEFORMAT( DX_CHARCODEFORMAT_UHC             ) ; break ;
		case DX_CHARCODEFORMAT_BIG5 :				_SET_CHARSET( DX_CHARSET_BIG5            ) ; _SET_CHAR_CHARCODEFORMAT( DX_CHARCODEFORMAT_BIG5            ) ; break ;
		case DX_CHARCODEFORMAT_GB2312 :				_SET_CHARSET( DX_CHARSET_GB2312          ) ; _SET_CHAR_CHARCODEFORMAT( DX_CHARCODEFORMAT_GB2312          ) ; break ;
		case DX_CHARCODEFORMAT_WINDOWS_1252 :		_SET_CHARSET( DX_CHARSET_WINDOWS_1252    ) ; _SET_CHAR_CHARCODEFORMAT( DX_CHARCODEFORMAT_WINDOWS_1252    ) ; break ;
		case DX_CHARCODEFORMAT_ISO_IEC_8859_15 :	_SET_CHARSET( DX_CHARSET_ISO_IEC_8859_15 ) ; _SET_CHAR_CHARCODEFORMAT( DX_CHARCODEFORMAT_ISO_IEC_8859_15 ) ; break ;
		case DX_CHARCODEFORMAT_SHIFTJIS :			_SET_CHARSET( DX_CHARSET_SHFTJIS         ) ; _SET_CHAR_CHARCODEFORMAT( DX_CHARCODEFORMAT_SHIFTJIS        ) ; break ;
		default :									_SET_CHARSET( DX_CHARSET_SHFTJIS         ) ; _SET_CHAR_CHARCODEFORMAT( DX_CHARCODEFORMAT_SHIFTJIS        ) ; break ;
		}
	}
}

extern char *_GETCWD( char *Buffer, size_t BufferBytes )
{
	wchar_t TempBuffer[ 1024 ] ;

	SETUP_WIN_API

	WinAPIData.Win32Func.GetCurrentDirectoryWFunc( 1024, TempBuffer ) ;
	ConvString( ( const char * )TempBuffer, -1, WIN32_WCHAR_CHARCODEFORMAT, Buffer, BufferBytes, CHAR_CHARCODEFORMAT ) ;

	return Buffer ;
}

extern wchar_t *_WGETCWD( wchar_t *Buffer, size_t BufferBytes )
{
	wchar_t TempBuffer[ 1024 ] ;

	SETUP_WIN_API

	WinAPIData.Win32Func.GetCurrentDirectoryWFunc( 1024, TempBuffer ) ;
	ConvString( ( const char * )TempBuffer, -1, WIN32_WCHAR_CHARCODEFORMAT, ( char * )Buffer, BufferBytes, WCHAR_T_CHARCODEFORMAT ) ;

	return Buffer ;
}

extern int _CHDIR( const char *DirName )
{
	wchar_t TempBuffer[ 1024 ] ;
	int Result ;

	SETUP_WIN_API

	ConvString( DirName, -1, CHAR_CHARCODEFORMAT, ( char * )TempBuffer, sizeof( TempBuffer ), WIN32_WCHAR_CHARCODEFORMAT ) ;
	Result = WinAPIData.Win32Func.SetCurrentDirectoryWFunc( TempBuffer ) ;

	return Result != 0 ? 0 : -1 ;
}

extern int _WCHDIR(  const wchar_t * DirName )
{
	wchar_t TempBuffer[ 1024 ] ;
	int Result ;

	SETUP_WIN_API

	ConvString( ( const char * )DirName, -1, WCHAR_T_CHARCODEFORMAT, ( char * )TempBuffer, sizeof( TempBuffer ), WIN32_WCHAR_CHARCODEFORMAT ) ;
	Result = WinAPIData.Win32Func.SetCurrentDirectoryWFunc( TempBuffer ) ;

	return Result != 0 ? 0 : -1 ;
}

extern int _FPCLASS( double x )
{
	return _fpclass( x ) ;
}

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

