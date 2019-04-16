// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		内部との出入り口プログラムファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

// インクルード ------------------------------------------------------------------
#include "DxCompileConfig.h"
#include "DxLib.h"
#include "DxModel.h"
#include "DxLog.h"
#include "DxGraphics.h"
#include "DxGraphicsFilter.h"
#include "DxSystem.h"
#include <stdio.h>

#ifndef DX_NON_SOFTIMAGE
#include "DxSoftImage.h"
#endif // DX_NON_SOFTIMAGE

#ifdef __WINDOWS__
#include "Windows/DxWinAPI.h"
#endif // __WINDOWS__

// マクロ定義 --------------------------------------------------------------------

#if defined( DX_THREAD_SAFE ) && defined( __WINDOWS__ )
	#define DXFUNC_BEGIN		CheckConflictAndWaitDxFunction() ;
	#define DXFUNC_END			PostConflictProcessDxFunction() ;
#else
	#define DXFUNC_BEGIN
	#define DXFUNC_END
#endif

#if ( defined( DX_THREAD_SAFE ) || defined( DX_THREAD_SAFE_NETWORK_ONLY ) ) && defined( __WINDOWS__ )
	#define DXFUNC_NET_BEGIN		CheckConflictAndWaitDxFunction() ;
	#define DXFUNC_NET_END			PostConflictProcessDxFunction() ;
#else
	#define DXFUNC_NET_BEGIN
	#define DXFUNC_NET_END
#endif


// プログラム --------------------------------------------------------------------

// DxWin.cpp関数プロトタイプ宣言

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

#if defined( DX_THREAD_SAFE ) || defined( DX_THREAD_SAFE_NETWORK_ONLY )

// 初期化終了系関数
extern int DxLib_Init( void )
{
	int Result ;

	// 既に初期化済みの場合は何もせず終了
	if( DxSysData.DxLib_InitializeFlag == TRUE ) return 0 ;

	DXFUNC_NET_BEGIN
	Result = NS_DxLib_Init() ;
	DXFUNC_NET_END
	return Result ;
}
extern int DxLib_End( void )
{
	int Result ;

	// 既に終了処理が行われているか、そもそも初期化されていない場合は何もしない
	if( DxSysData.DxLib_InitializeFlag == FALSE ) return 0 ;

	DXFUNC_NET_BEGIN
	Result = NS_DxLib_End() ;
	DXFUNC_NET_END

#ifdef __WINDOWS__
	SETUP_WIN_API

	// クリティカルセクションとイベントハンドルを解放する
	if( WinData.DxConflictWaitThreadIDInitializeFlag == TRUE )
	{
		int i ;

		// 初期化フラグを倒す
		WinData.DxConflictWaitThreadIDInitializeFlag = FALSE ;

		// 衝突時に使用するイベントの解放
		for( i = 0 ; i < MAX_THREADWAIT_NUM ; i ++ )
		{
			if( WinData.DxConflictWaitThreadID[i][1] != 0 )
				WinAPIData.Win32Func.CloseHandleFunc( (HANDLE)WinData.DxConflictWaitThreadID[i][1] ) ;
			WinData.DxConflictWaitThreadID[i][1] = 0 ;
		}

		// クリティカルセクションも削除する
		CriticalSection_Delete( &WinData.DxConflictCheckCriticalSection ) ;
	}
#endif // __WINDOWS__

	return Result ;
}

#endif

#ifdef DX_THREAD_SAFE

extern int SetAlwaysRunFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetAlwaysRunFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}

extern int DxLib_GlobalStructInitialize( void )
{
	return NS_DxLib_GlobalStructInitialize();
}

extern BOOL DxLib_IsInit( void )
{
	return NS_DxLib_IsInit();
}

extern int GetLastErrorCode( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetLastErrorCode() ;
	DXFUNC_END
	return Result ;
}

extern int GetLastErrorMessage( TCHAR *StringBuffer, int StringBufferBytes )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetLastErrorMessage( StringBuffer, StringBufferBytes ) ;
	DXFUNC_END
	return Result ;
}

#ifndef DX_NON_LOG

// ログファイル関数
extern int LogFileAdd( const TCHAR *String )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LogFileAdd( String ) ;
	DXFUNC_END
	return Result ;
}
extern int LogFileAddWithStrLen( const TCHAR *String, size_t StringLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LogFileAddWithStrLen( String, StringLength ) ;
	DXFUNC_END
	return Result ;
}
extern int LogFileFmtAdd( const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	
	DXFUNC_BEGIN
	Result = NS_LogFileAdd( String ) ;
	DXFUNC_END
	return Result ;
}
extern int LogFileTabAdd( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LogFileTabAdd() ;
	DXFUNC_END
	return Result ;
}
extern int LogFileTabSub( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LogFileTabSub() ;
	DXFUNC_END
	return Result ;
}
extern int ErrorLogAdd( const TCHAR *String )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ErrorLogAdd( String ) ;
	DXFUNC_END
	return Result ;
}
extern int ErrorLogFmtAdd( const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	
	DXFUNC_BEGIN
	Result = NS_ErrorLogAdd( String ) ;
	DXFUNC_END
	return Result ;
}
extern int ErrorLogTabAdd( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ErrorLogTabAdd() ;
	DXFUNC_END
	return Result ;
}
extern int ErrorLogTabSub( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ErrorLogTabSub() ;
	DXFUNC_END
	return Result ;
}
extern int SetUseTimeStampFlag( int UseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseTimeStampFlag( UseFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int AppLogAdd( const TCHAR *String, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR StringBuf[ 2048 ] ;

	va_start( VaList, String ) ;
	_TVSNPRINTF( StringBuf, sizeof( StringBuf ) / sizeof( TCHAR ), String, VaList ) ;
	va_end( VaList ) ;

	DXFUNC_BEGIN
	Result = NS_LogFileAdd( StringBuf ) ;
	DXFUNC_END
	return Result ;
}

#endif // DX_NON_LOG

// メモリ確保系関数
extern	void 		*DxAlloc( size_t AllocSize, const char *File , int Line  )
{
	void *Result ;
	DXFUNC_BEGIN
	Result = NS_DxAlloc( AllocSize, File , Line  ) ;
	DXFUNC_END
	return Result ;
}
extern	void 		*DxCalloc( size_t AllocSize, const char *File , int Line  )
{
	void *Result ;
	DXFUNC_BEGIN
	Result = NS_DxCalloc(  AllocSize,   File ,  Line  ) ;
	DXFUNC_END
	return Result ;
}
extern	void		*DxRealloc( void *Memory, size_t AllocSize, const char *File , int Line  )
{
	void *Result ;
	DXFUNC_BEGIN
	Result = NS_DxRealloc( Memory,  AllocSize, File , Line ) ;
	DXFUNC_END
	return Result ;
}
extern	void		DxFree( void *Memory )
{
	DXFUNC_BEGIN
	NS_DxFree( Memory ) ;
	DXFUNC_END
}
extern	size_t		DxSetAllocSizeTrap( size_t Size )
{
	size_t Result ;
	DXFUNC_BEGIN
	Result = NS_DxSetAllocSizeTrap( Size ) ;
	DXFUNC_END
	return Result ;
}
extern int DxSetAllocPrintFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DxSetAllocPrintFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern	size_t		DxGetAllocSize( void )
{
	size_t Result ;
	DXFUNC_BEGIN
	Result = NS_DxGetAllocSize( ) ;
	DXFUNC_END
	return Result ;
}
extern int DxGetAllocNum( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DxGetAllocNum(  ) ;
	DXFUNC_END
	return Result ;
}
extern	void		DxDumpAlloc( void )
{
	DXFUNC_BEGIN
	NS_DxDumpAlloc(  ) ;
	DXFUNC_END
}
extern	void		DxDrawAlloc( int x, int y, int Width, int Height )
{
	DXFUNC_BEGIN
	NS_DxDrawAlloc(  x,  y,  Width,  Height ) ;
	DXFUNC_END
}
extern int DxErrorCheckAlloc( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DxErrorCheckAlloc(  ) ;
	DXFUNC_END
	return Result ;
}
extern int DxSetAllocSizeOutFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DxSetAllocSizeOutFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int DxSetAllocMemoryErrorCheckFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DxSetAllocMemoryErrorCheckFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}

extern int GetCharBytes( int CharCodeFormat /* DX_CHARCODEFORMAT_SHIFTJIS 等 */ , const void *String )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetCharBytes( CharCodeFormat , String ) ;
	DXFUNC_END
	return Result ;
}

extern int ConvertStringCharCodeFormat( int SrcCharCodeFormat /* DX_CHARCODEFORMAT_SHIFTJIS 等 */, const void *SrcString, int DestCharCodeFormat /* DX_CHARCODEFORMAT_SHIFTJIS 等 */, void *DestStringBuffer )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ConvertStringCharCodeFormat( SrcCharCodeFormat, SrcString, DestCharCodeFormat, DestStringBuffer ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseCharCodeFormat( int CharCodeFormat /* DX_CHARCODEFORMAT_SHIFTJIS 等 */ )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseCharCodeFormat( CharCodeFormat ) ;
	DXFUNC_END
	return Result ;
}
extern int Get_wchar_t_CharCodeFormat( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_Get_wchar_t_CharCodeFormat( ) ;
	DXFUNC_END
	return Result ;
}
extern	void			strcpyDx(   TCHAR *Dest, const TCHAR *Src )
{
	DXFUNC_BEGIN
	NS_strcpyDx( Dest, Src ) ;
	DXFUNC_END
}
extern	void			strcpy_sDx(   TCHAR *Dest, size_t DestBytes, const TCHAR *Src )
{
	DXFUNC_BEGIN
	NS_strcpy_sDx( Dest, DestBytes, Src ) ;
	DXFUNC_END
}
extern	void			strpcpyDx(    TCHAR *Dest, const TCHAR *Src, int Pos )
{
	DXFUNC_BEGIN
	NS_strpcpyDx(    Dest, Src, Pos ) ;
	DXFUNC_END
}
extern	void			strpcpy_sDx(    TCHAR *Dest, size_t DestBytes, const TCHAR *Src, int Pos )
{
	DXFUNC_BEGIN
	NS_strpcpy_sDx(    Dest, DestBytes, Src, Pos ) ;
	DXFUNC_END
}
extern	void			strpcpy2Dx(   TCHAR *Dest, const TCHAR *Src, int Pos )
{
	DXFUNC_BEGIN
	NS_strpcpy2Dx(   Dest, Src, Pos ) ;
	DXFUNC_END
}
extern	void			strpcpy2_sDx(   TCHAR *Dest, size_t DestBytes, const TCHAR *Src, int Pos )
{
	DXFUNC_BEGIN
	NS_strpcpy2_sDx(   Dest, DestBytes, Src, Pos ) ;
	DXFUNC_END
}
extern	void			strncpyDx(  TCHAR *Dest, const TCHAR *Src, int Num )
{
	DXFUNC_BEGIN
	NS_strncpyDx( Dest, Src, Num ) ;
	DXFUNC_END
}
extern	void			strncpy_sDx(  TCHAR *Dest, size_t DestBytes, const TCHAR *Src, int Num )
{
	DXFUNC_BEGIN
	NS_strncpy_sDx( Dest, DestBytes, Src, Num ) ;
	DXFUNC_END
}
extern	void			strncpy2Dx( TCHAR *Dest, const TCHAR *Src, int Num )
{
	DXFUNC_BEGIN
	NS_strncpy2Dx( Dest, Src, Num ) ;
	DXFUNC_END
}
extern	void			strncpy2_sDx( TCHAR *Dest, size_t DestBytes, const TCHAR *Src, int Num )
{
	DXFUNC_BEGIN
	NS_strncpy2_sDx( Dest, DestBytes, Src, Num ) ;
	DXFUNC_END
}
extern	void			strrncpyDx(   TCHAR *Dest, const TCHAR *Src, int Num )
{
	DXFUNC_BEGIN
	NS_strrncpyDx(   Dest, Src, Num ) ;
	DXFUNC_END
}
extern	void			strrncpy_sDx(   TCHAR *Dest, size_t DestBytes, const TCHAR *Src, int Num )
{
	DXFUNC_BEGIN
	NS_strrncpy_sDx(   Dest, DestBytes, Src, Num ) ;
	DXFUNC_END
}
extern	void			strrncpy2Dx(  TCHAR *Dest, const TCHAR *Src, int Num )
{
	DXFUNC_BEGIN
	NS_strrncpy2Dx(  Dest, Src, Num ) ;
	DXFUNC_END
}
extern	void			strrncpy2_sDx(  TCHAR *Dest, size_t DestBytes, const TCHAR *Src, int Num )
{
	DXFUNC_BEGIN
	NS_strrncpy2_sDx(  Dest, DestBytes, Src, Num ) ;
	DXFUNC_END
}
extern	void			strpncpyDx(   TCHAR *Dest, const TCHAR *Src, int Pos, int Num )
{
	DXFUNC_BEGIN
	NS_strpncpyDx(   Dest, Src, Pos, Num ) ;
	DXFUNC_END
}
extern	void			strpncpy_sDx(   TCHAR *Dest, size_t DestBytes, const TCHAR *Src, int Pos, int Num )
{
	DXFUNC_BEGIN
	NS_strpncpy_sDx(   Dest, DestBytes, Src, Pos, Num ) ;
	DXFUNC_END
}
extern	void			strpncpy2Dx(  TCHAR *Dest, const TCHAR *Src, int Pos, int Num )
{
	DXFUNC_BEGIN
	NS_strpncpy2Dx(  Dest, Src, Pos, Num ) ;
	DXFUNC_END
}
extern	void			strpncpy2_sDx(  TCHAR *Dest, size_t DestBytes, const TCHAR *Src, int Pos, int Num )
{
	DXFUNC_BEGIN
	NS_strpncpy2_sDx(  Dest, DestBytes, Src, Pos, Num ) ;
	DXFUNC_END
}
extern	void			strcatDx(   TCHAR *Dest, const TCHAR *Src )
{
	DXFUNC_BEGIN
	NS_strcatDx( Dest, Src ) ;
	DXFUNC_END
}
extern	void			strcat_sDx(   TCHAR *Dest, size_t DestBytes, const TCHAR *Src )
{
	DXFUNC_BEGIN
	NS_strcat_sDx( Dest, DestBytes, Src ) ;
	DXFUNC_END
}
extern	size_t			strlenDx(   const TCHAR *Str )
{
	size_t Result ;
	DXFUNC_BEGIN
	Result = NS_strlenDx( Str ) ;
	DXFUNC_END
	return Result ;
}
extern	size_t			strlen2Dx(  const TCHAR *Str )
{
	size_t Result ;
	DXFUNC_BEGIN
	Result = NS_strlen2Dx( Str ) ;
	DXFUNC_END
	return Result ;
}
extern	int				strcmpDx(   const TCHAR *Str1, const TCHAR *Str2 )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_strcmpDx( Str1, Str2 ) ;
	DXFUNC_END
	return Result ;
}
extern	int				stricmpDx(  const TCHAR *Str1, const TCHAR *Str2 )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_stricmpDx( Str1, Str2 ) ;
	DXFUNC_END
	return Result ;
}
extern	int				strncmpDx(  const TCHAR *Str1, const TCHAR *Str2, int Size )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_strncmpDx( Str1, Str2, Size ) ;
	DXFUNC_END
	return Result ;
}
extern	int				strncmp2Dx( const TCHAR *Str1, const TCHAR *Str2, int Num )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_strncmp2Dx( Str1, Str2, Num ) ;
	DXFUNC_END
	return Result ;
}
extern	int				strpncmpDx(   const TCHAR *Str1, const TCHAR *Str2, int Pos, int Num )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_strpncmpDx(   Str1, Str2, Pos, Num ) ;
	DXFUNC_END
	return Result ;
}
extern	int				strpncmp2Dx(  const TCHAR *Str1, const TCHAR *Str2, int Pos, int Num )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_strpncmp2Dx(  Str1, Str2, Pos, Num ) ;
	DXFUNC_END
	return Result ;
}
extern	DWORD			strgetchrDx(  const TCHAR *Str, int Pos, int *CharNums )
{
	DWORD Result ;
	DXFUNC_BEGIN
	Result = NS_strgetchrDx(  Str, Pos, CharNums ) ;
	DXFUNC_END
	return Result ;
}
extern	DWORD			strgetchr2Dx( const TCHAR *Str, int Pos, int *CharNums )
{
	DWORD Result ;
	DXFUNC_BEGIN
	Result = NS_strgetchr2Dx( Str, Pos, CharNums ) ;
	DXFUNC_END
	return Result ;
}
extern	int				strputchrDx(  TCHAR *Str, int Pos, DWORD CharCode )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_strputchrDx( Str, Pos, CharCode ) ;
	DXFUNC_END
	return Result ;
}
extern	int				strputchr2Dx( TCHAR *Str, int Pos, DWORD CharCode )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_strputchr2Dx( Str, Pos, CharCode ) ;
	DXFUNC_END
	return Result ;
}
extern	const TCHAR *	strposDx(     const TCHAR *Str, int Pos )
{
	const TCHAR *Result ;
	DXFUNC_BEGIN
	Result = NS_strposDx(     Str, Pos ) ;
	DXFUNC_END
	return Result ;
}
extern	const TCHAR *	strpos2Dx(    const TCHAR *Str, int Pos )
{
	const TCHAR *Result ;
	DXFUNC_BEGIN
	Result = NS_strpos2Dx(    Str, Pos ) ;
	DXFUNC_END
	return Result ;
}
extern	const TCHAR *	strstrDx(   const TCHAR *Str1, const TCHAR *Str2 )
{
	const TCHAR *Result ;
	DXFUNC_BEGIN
	Result = NS_strstrDx(  Str1, Str2 ) ;
	DXFUNC_END
	return Result ;
}
extern	int				strstr2Dx(    const TCHAR *Str1, const TCHAR *Str2 )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_strstr2Dx(  Str1, Str2 ) ;
	DXFUNC_END
	return Result ;
}
extern	const TCHAR *	strchrDx(   const TCHAR *Str, DWORD CharCode )
{
	const TCHAR * Result ;
	DXFUNC_BEGIN
	Result = NS_strchrDx( Str, CharCode ) ;
	DXFUNC_END
	return Result ;
}
extern	int				strchr2Dx(    const TCHAR *Str, DWORD CharCode )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_strchr2Dx(  Str, CharCode ) ;
	DXFUNC_END
	return Result ;
}
extern	const TCHAR *	strrchrDx(  const TCHAR *Str, DWORD CharCode )
{
	const TCHAR * Result ;
	DXFUNC_BEGIN
	Result = NS_strrchrDx( Str, CharCode ) ;
	DXFUNC_END
	return Result ;
}
extern	int				strrchr2Dx(   const TCHAR *Str, DWORD CharCode )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_strrchr2Dx( Str, CharCode ) ;
	DXFUNC_END
	return Result ;
}
extern	TCHAR *			struprDx(   TCHAR *Str )
{
	TCHAR * Result ;
	DXFUNC_BEGIN
	Result = NS_struprDx( Str ) ;
	DXFUNC_END
	return Result ;
}
extern	int				vsprintfDx( TCHAR *Buffer, const TCHAR *FormatString, va_list Arg )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_vsprintfDx( Buffer, FormatString, Arg ) ;
	DXFUNC_END
	return Result ;
}
extern	int				vsnprintfDx( TCHAR *Buffer, size_t BufferSize, const TCHAR *FormatString, va_list Arg )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_vsnprintfDx( Buffer, BufferSize, FormatString, Arg ) ;
	DXFUNC_END
	return Result ;
}
extern	int				sprintfDx(  TCHAR *Buffer, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;

	DXFUNC_BEGIN

	va_start( VaList, FormatString ) ;
	Result = NS_vsprintfDx( Buffer, FormatString, VaList ) ;
	va_end( VaList ) ;

	DXFUNC_END

	return Result ;
}
extern	int				snprintfDx(  TCHAR *Buffer, size_t BufferSize, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;

	DXFUNC_BEGIN

	va_start( VaList, FormatString ) ;
	Result = NS_vsnprintfDx( Buffer, BufferSize, FormatString, VaList ) ;
	va_end( VaList ) ;

	DXFUNC_END

	return Result ;
}
extern	TCHAR *			itoaDx(     int Value, TCHAR *Buffer, int Radix )
{
	TCHAR *Result ;
	DXFUNC_BEGIN
	Result = NS_itoaDx( Value, Buffer, Radix ) ;
	DXFUNC_END
	return Result ;
}
extern	TCHAR *			itoa_sDx(     int Value, TCHAR *Buffer, size_t BufferBytes, int Radix )
{
	TCHAR *Result ;
	DXFUNC_BEGIN
	Result = NS_itoa_sDx( Value, Buffer, BufferBytes, Radix ) ;
	DXFUNC_END
	return Result ;
}
extern	int				atoiDx(     const TCHAR *Str )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_atoiDx( Str ) ;
	DXFUNC_END
	return Result ;
}
extern	double			atofDx(     const TCHAR *Str )
{
	double Result ;
	DXFUNC_BEGIN
	Result = NS_atofDx( Str ) ;
	DXFUNC_END
	return Result ;
}
extern	int				vsscanfDx(  const TCHAR *String, const TCHAR *FormatString, va_list Arg )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_vsscanfDx( String, FormatString, Arg ) ;
	DXFUNC_END
	return Result ;
}
extern	int				sscanfDx(   const TCHAR *String, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;

	DXFUNC_BEGIN

	va_start( VaList, FormatString ) ;
	Result = NS_vsscanfDx( String, FormatString, VaList ) ;
	va_end( VaList ) ;

	DXFUNC_END

	return Result ;
}

#ifndef DX_NON_LOG

#ifndef DX_NON_PRINTF_DX

// ログ出力機能関数
extern int SetLogDrawOutFlag( int DrawFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetLogDrawOutFlag( DrawFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetLogDrawFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetLogDrawFlag(  ) ;
	DXFUNC_END
	return Result ;
}
extern int SetLogFontSize( int Size )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetLogFontSize( Size ) ;
	DXFUNC_END
	return Result ;
}
extern int SetLogFontHandle( int FontHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetLogFontHandle( FontHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetLogDrawArea( int x1, int y1, int x2, int y2 )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetLogDrawArea( x1, y1, x2, y2 ) ;
	DXFUNC_END
	return Result ;
}

// 簡易画面出力関数
extern int printfDx( const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	
	DXFUNC_BEGIN
	Result = printfDxBase( String ) ;
	DXFUNC_END
	return Result ;
}
extern int putsDx( const TCHAR *String, int NewLine )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_putsDx( String, NewLine ) ;
	DXFUNC_END
	return Result ;
}
extern int putsDxWithStrLen( const TCHAR *String, size_t StringLength, int NewLine )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_putsDxWithStrLen( String, StringLength, NewLine ) ;
	DXFUNC_END
	return Result ;
}
extern int clsDx( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_clsDx(  ) ;
	DXFUNC_END
	return Result ;
}

#endif // DX_NON_PRINTF_DX

#endif // DX_NON_LOG

extern int FileRead_open( const TCHAR *FilePath , int ASync )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_FileRead_open( FilePath, ASync ) ;
	DXFUNC_END
	return Result ;
}
extern int FileRead_open_WithStrLen( const TCHAR *FilePath, size_t FilePathLength, int ASync )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_FileRead_open_WithStrLen( FilePath, FilePathLength, ASync ) ;
	DXFUNC_END
	return Result ;
}
extern int FileRead_open_mem( const void *FileImage, size_t FileImageSize )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_FileRead_open_mem( FileImage, FileImageSize ) ;
	DXFUNC_END
	return Result ;
}
extern LONGLONG FileRead_size( const TCHAR *FilePath )
{
	LONGLONG Result ;
	DXFUNC_BEGIN
	Result = NS_FileRead_size( FilePath ) ;
	DXFUNC_END
	return Result ;
}
extern LONGLONG FileRead_size_WithStrLen( const TCHAR *FilePath, size_t FilePathLength )
{
	LONGLONG Result ;
	DXFUNC_BEGIN
	Result = NS_FileRead_size_WithStrLen( FilePath, FilePathLength ) ;
	DXFUNC_END
	return Result ;
}
extern int FileRead_close( int FileHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_FileRead_close( FileHandle ) ;
	DXFUNC_END
	return Result ;
}
extern LONGLONG FileRead_tell( int FileHandle )
{
	LONGLONG Result ;
	DXFUNC_BEGIN
	Result = NS_FileRead_tell( FileHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int FileRead_seek( int FileHandle, LONGLONG Offset, int Origin )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_FileRead_seek(  FileHandle,  Offset,  Origin ) ;
	DXFUNC_END
	return Result ;
}
extern int FileRead_read( void *Buffer, int ReadSize, int FileHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_FileRead_read( Buffer,  ReadSize,  FileHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int FileRead_idle_chk( int FileHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_FileRead_idle_chk( FileHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int FileRead_eof( int FileHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_FileRead_eof( FileHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int FileRead_set_format( int FileHandle, int CharCodeFormat /* DX_CHARCODEFORMAT_SHIFTJIS 等 */ )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_FileRead_set_format( FileHandle, CharCodeFormat ) ;
	DXFUNC_END
	return Result ;
}
extern int FileRead_gets( TCHAR *Buffer, int BufferSize, int FileHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_FileRead_gets( Buffer,  BufferSize,  FileHandle ) ;
	DXFUNC_END
	return Result ;
}
extern TCHAR FileRead_getc( int FileHandle )
{
	TCHAR Result ;
	DXFUNC_BEGIN
	Result = NS_FileRead_getc( FileHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int FileRead_scanf( int FileHandle, const TCHAR *Format, ... )
{
	va_list param;
	int Result;

	DXFUNC_BEGIN

	va_start( param, Format );

	Result = FileRead_scanf_base( FileHandle, Format, param ) ;

	va_end( param );

	DXFUNC_END

	return Result;
}
extern DWORD_PTR FileRead_createInfo( const TCHAR *ObjectPath )
{
	DWORD_PTR Result ;
	DXFUNC_BEGIN
	Result = NS_FileRead_createInfo( ObjectPath ) ;
	DXFUNC_END
	return Result ;
}
extern DWORD_PTR FileRead_createInfo_WithStrLen( const TCHAR *ObjectPath, size_t ObjectPathLength )
{
	DWORD_PTR Result ;
	DXFUNC_BEGIN
	Result = NS_FileRead_createInfo_WithStrLen( ObjectPath, ObjectPathLength ) ;
	DXFUNC_END
	return Result ;
}
extern int FileRead_getInfoNum( DWORD_PTR FileInfoHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_FileRead_getInfoNum( FileInfoHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int FileRead_getInfo( int Index, FILEINFO *Buffer, DWORD_PTR FileInfoHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_FileRead_getInfo( Index, Buffer, FileInfoHandle );
	DXFUNC_END
	return Result ;
}
extern int FileRead_deleteInfo( DWORD_PTR FileInfoHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_FileRead_deleteInfo( FileInfoHandle );
	DXFUNC_END
	return Result ;
}
extern DWORD_PTR FileRead_findFirst( const TCHAR *FilePath, FILEINFO *Buffer )
{
	DWORD_PTR Result ;
	DXFUNC_BEGIN
	Result = NS_FileRead_findFirst( FilePath, Buffer );
	DXFUNC_END
	return Result ;
}
extern DWORD_PTR FileRead_findFirst_WithStrLen( const TCHAR *FilePath, size_t FilePathLength, FILEINFO *Buffer )
{
	DWORD_PTR Result ;
	DXFUNC_BEGIN
	Result = NS_FileRead_findFirst_WithStrLen( FilePath, FilePathLength, Buffer ) ;
	DXFUNC_END
	return Result ;
}
extern int FileRead_findNext( DWORD_PTR FindHandle, FILEINFO *Buffer )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_FileRead_findNext( FindHandle, Buffer );
	DXFUNC_END
	return Result ;
}
extern int FileRead_findClose( DWORD_PTR FindHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_FileRead_findClose( FindHandle );
	DXFUNC_END
	return Result ;
}








extern	int	FileRead_fullyLoad(			const TCHAR *FilePath )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_FileRead_fullyLoad(	FilePath );
	DXFUNC_END
	return Result ;
}
extern int FileRead_fullyLoad_WithStrLen( const TCHAR *FilePath, size_t FilePathLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_FileRead_fullyLoad_WithStrLen( FilePath, FilePathLength ) ;
	DXFUNC_END
	return Result ;
}
extern	int			FileRead_fullyLoad_delete(	int FLoadHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_FileRead_fullyLoad_delete( FLoadHandle );
	DXFUNC_END
	return Result ;
}
extern	const void*		FileRead_fullyLoad_getImage( int FLoadHandle )
{
	const void* Result ;
	DXFUNC_BEGIN
	Result = NS_FileRead_fullyLoad_getImage( FLoadHandle );
	DXFUNC_END
	return Result ;
}
extern	LONGLONG	FileRead_fullyLoad_getSize(	int FLoadHandle )
{
	LONGLONG Result ;
	DXFUNC_BEGIN
	Result = NS_FileRead_fullyLoad_getSize( FLoadHandle );
	DXFUNC_END
	return Result ;
}


// 便利関数
extern int GetResourceInfo( const TCHAR *ResourceName, const TCHAR *ResourceType, void **DataPointerP, int *DataSizeP )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetResourceInfo( ResourceName, ResourceType, DataPointerP, DataSizeP ) ;
	DXFUNC_END
	return Result ;
}
extern int GetResourceInfoWithStrLen( const TCHAR *ResourceName, size_t ResourceNameLength, const TCHAR *ResourceType, size_t ResourceTypeLength, void **DataPointerP , int *DataSizeP )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetResourceInfoWithStrLen( ResourceName, ResourceNameLength, ResourceType, ResourceTypeLength, DataPointerP , DataSizeP ) ;
	DXFUNC_END
	return Result ;
}
extern const TCHAR *GetResourceIDString( int ResourceID )
{
	return NS_GetResourceIDString( ResourceID ) ;
}


#endif

#if defined( DX_THREAD_SAFE ) || defined( DX_THREAD_SAFE_NETWORK_ONLY )

// メッセージ処理関数
extern int ProcessMessage( void )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_ProcessMessage(  ) ;
	DXFUNC_NET_END
	return Result ;
}

#endif

#ifdef DX_THREAD_SAFE

// ウインドウ関係情報取得関数
extern int GetWindowCRect( RECT *RectBuf )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetWindowCRect( RectBuf ) ;
	DXFUNC_END
	return Result ;
}
extern int GetWindowClientRect( RECT *RectBuf )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetWindowClientRect( RectBuf ) ;
	DXFUNC_END
	return Result ;
}
extern int GetWindowFrameRect( RECT *RectBuf )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetWindowFrameRect( RectBuf ) ;
	DXFUNC_END
	return Result ;
}
extern int GetWindowActiveFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetWindowActiveFlag( ) ;
	DXFUNC_END
	return Result ;
}
extern int GetWindowMinSizeFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetWindowMinSizeFlag( ) ;
	DXFUNC_END
	return Result ;
}
extern int GetWindowMaxSizeFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetWindowMaxSizeFlag() ;
	DXFUNC_END
	return Result ;
}

extern	HWND		GetMainWindowHandle( void )
{
	HWND Result ;
	DXFUNC_BEGIN
	Result = NS_GetMainWindowHandle(  ) ;
	DXFUNC_END
	return Result ;
}
extern int GetWindowModeFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetWindowModeFlag(  ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDefaultState( int *SizeX, int *SizeY, int *ColorBitDepth, int *RefreshRate , int *LeftTopX, int *LeftTopY, int *PixelSizeX, int *PixelSizeY )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDefaultState( SizeX, SizeY, ColorBitDepth, RefreshRate , LeftTopX , LeftTopY, PixelSizeX, PixelSizeY ) ;
	DXFUNC_END
	return Result ;
}
extern int GetActiveFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetActiveFlag( ) ;
	DXFUNC_END
	return Result ;
}
extern int GetNoActiveState( int ResetFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetNoActiveState( ResetFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetMouseDispFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetMouseDispFlag(  ) ;
	DXFUNC_END
	return Result ;
}
extern int GetAlwaysRunFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetAlwaysRunFlag(  ) ;
	DXFUNC_END
	return Result ;
}
extern int _GetSystemInfo( int *DxLibVer, int *DirectXVer, int *WindowsVer )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS__GetSystemInfo( DxLibVer, DirectXVer, WindowsVer ) ;
	DXFUNC_END
	return Result ;
}
extern int GetPcInfo( TCHAR *OSString, TCHAR *DirectXString,
								TCHAR *CPUString, int *CPUSpeed/*単位MHz*/,
								double *FreeMemorySize/*単位MByte*/, double *TotalMemorySize,
								TCHAR *VideoDriverFileName, TCHAR *VideoDriverString,
								double *FreeVideoMemorySize/*単位MByte*/, double *TotalVideoMemorySize )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetPcInfo( OSString, DirectXString,
								CPUString, CPUSpeed/*単位MHz*/,
								FreeMemorySize/*単位MByte*/, TotalMemorySize,
								VideoDriverFileName, VideoDriverString,
								FreeVideoMemorySize/*単位MByte*/, TotalVideoMemorySize ) ;
	DXFUNC_END
	return Result ;
}
extern int GetUseMMXFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetUseMMXFlag(  ) ;
	DXFUNC_END
	return Result ;
}
extern int GetUseSSEFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetUseSSEFlag(  ) ;
	DXFUNC_END
	return Result ;
}
extern int GetUseSSE2Flag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetUseSSE2Flag(  ) ;
	DXFUNC_END
	return Result ;
}
extern int GetWindowCloseFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetWindowCloseFlag(  ) ;
	DXFUNC_END
	return Result ;
}
extern	HINSTANCE	GetTaskInstance( void )
{
	HINSTANCE Result ;
	DXFUNC_BEGIN
	Result = NS_GetTaskInstance( ) ;
	DXFUNC_END
	return Result ;
}
extern int GetUseWindowRgnFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetUseWindowRgnFlag(  ) ;
	DXFUNC_END
	return Result ;
}
extern int GetWindowSizeChangeEnableFlag( int *FitScreen )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetWindowSizeChangeEnableFlag( FitScreen ) ;
	DXFUNC_END
	return Result ;
}
extern double GetWindowSizeExtendRate( double *ExRateX, double *ExRateY )
{
	double Result ;
	DXFUNC_BEGIN
	Result = NS_GetWindowSizeExtendRate( ExRateX, ExRateY ) ;
	DXFUNC_END
	return Result ;
}
extern int GetWindowSize( int *Width, int *Height )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetWindowSize( Width, Height ) ;
	DXFUNC_END
	return Result ;
}
extern int GetWindowEdgeWidth( int *LeftWidth, int *RightWidth, int *TopWidth, int *BottomWidth )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetWindowEdgeWidth( LeftWidth, RightWidth, TopWidth, BottomWidth ) ;
	DXFUNC_END
	return Result ;
}
extern int GetWindowPosition( int *x, int *y )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetWindowPosition( x, y ) ;
	DXFUNC_END
	return Result ;
}
extern int GetWindowUserCloseFlag( int StateResetFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetWindowUserCloseFlag( StateResetFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetNotDrawFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetNotDrawFlag() ;
	DXFUNC_END
	return Result ;
}
extern int GetPaintMessageFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetPaintMessageFlag() ;
	DXFUNC_END
	return Result ;
}
extern int GetValidHiPerformanceCounter( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetValidHiPerformanceCounter() ;
	DXFUNC_END
	return Result ;
}
extern TCHAR GetInputSystemChar( int DeleteFlag )
{
	TCHAR Result ;
	DXFUNC_BEGIN
	Result = NS_GetInputSystemChar( DeleteFlag ) ;
	DXFUNC_END
	return Result ;
}



// 設定系関数
extern int ChangeWindowMode( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ChangeWindowMode( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseCharSet( int CharSet /* = DX_CHARSET_SHFTJIS 等 */ )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseCharSet( CharSet ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadPauseGraph( const TCHAR *FileName )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadPauseGraph( FileName ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadPauseGraphWithStrLen( const TCHAR *FileName, size_t FileNameLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadPauseGraphWithStrLen( FileName, FileNameLength ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadPauseGraphFromMem( const void *MemImage, int MemImageSize )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadPauseGraphFromMem( MemImage, MemImageSize ) ;
	DXFUNC_END
	return Result ;
}
extern int SetActiveStateChangeCallBackFunction( int (*CallBackFunction)( int ActiveState, void *UserData ), void *UserData )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetActiveStateChangeCallBackFunction( CallBackFunction, UserData ) ;
	DXFUNC_END
	return Result ;
}
extern int SetWindowText( const TCHAR *WindowText )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetWindowText( WindowText ) ;
	DXFUNC_END
	return Result ;
}
extern int SetWindowTextWithStrLen( const TCHAR *WindowText, size_t WindowTextLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetWindowTextWithStrLen( WindowText, WindowTextLength ) ;
	DXFUNC_END
	return Result ;
}
extern int SetMainWindowText( const TCHAR *WindowText )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetMainWindowText( WindowText ) ;
	DXFUNC_END
	return Result ;
}
extern int SetMainWindowTextWithStrLen( const TCHAR *WindowText, size_t WindowTextLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetMainWindowTextWithStrLen( WindowText, WindowTextLength ) ;
	DXFUNC_END
	return Result ;
}
extern int SetMainWindowClassName( const TCHAR *ClassName )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetMainWindowClassName( ClassName ) ;
	DXFUNC_END
	return Result ;
}
extern int SetMainWindowClassNameWithStrLen( const TCHAR *ClassName, size_t ClassNameLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetMainWindowClassNameWithStrLen( ClassName, ClassNameLength ) ;
	DXFUNC_END
	return Result ;
}

#ifndef DX_NON_LOG
extern int SetOutApplicationLogValidFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetOutApplicationLogValidFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetApplicationLogFileName( const TCHAR *FileName )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetApplicationLogFileName( FileName ) ;
	DXFUNC_END
	return Result ;
}
extern int SetApplicationLogFileNameWithStrLen( const TCHAR *FileName, size_t FileNameLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetApplicationLogFileNameWithStrLen( FileName, FileNameLength ) ;
	DXFUNC_END
	return Result ;
}
extern int SetApplicationLogSaveDirectory( const TCHAR *DirectoryPath )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetApplicationLogSaveDirectory( DirectoryPath ) ;
	DXFUNC_END
	return Result ;
}
extern int SetApplicationLogSaveDirectoryWithStrLen( const TCHAR *DirectoryPath, size_t DirectoryPathLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetApplicationLogSaveDirectoryWithStrLen( DirectoryPath, DirectoryPathLength ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseDateNameLogFile( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseDateNameLogFile( Flag ) ;
	DXFUNC_END
	return Result ;
}
#endif // DX_NON_LOG

extern int SetWindowIconID( int ID )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetWindowIconID( ID ) ;
	DXFUNC_END
	return Result ;
}
extern int SetWindowIconHandle( HICON Icon )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetWindowIconHandle( Icon ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseASyncChangeWindowModeFunction( int Flag, void (*CallBackFunction)(void*), void *Data )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseASyncChangeWindowModeFunction( Flag, CallBackFunction, Data ) ;
	DXFUNC_END
	return Result ;
}
extern int SetShutdownCallbackFunction(	void (* CallbackFunction )( void * ), void *Data, const TCHAR *Message )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetShutdownCallbackFunction( CallbackFunction, Data, Message ) ;
	DXFUNC_END
	return Result ;
}
extern int SetWindowStyleMode( int Mode )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetWindowStyleMode( Mode ) ;
	DXFUNC_END
	return Result ;
}
extern int SetWindowZOrder( int ZType /* = DX_WIN_ZTYPE_TOP 等 */ , int WindowActivateFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetWindowZOrder( ZType, WindowActivateFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetWindowSizeChangeEnableFlag( int Flag, int FitScreen )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetWindowSizeChangeEnableFlag( Flag, FitScreen ) ;
	DXFUNC_END
	return Result ;
}
extern int SetWindowSizeExtendRate( double ExRateX, double ExRateY )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetWindowSizeExtendRate( ExRateX, ExRateY ) ;
	DXFUNC_END
	return Result ;
}
extern int SetWindowSize( int Width, int Height )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetWindowSize(  Width,  Height ) ;
	DXFUNC_END
	return Result ;
}
extern int SetWindowMaxSize( int MaxWidth, int MaxHeight )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetWindowMaxSize(  MaxWidth,  MaxHeight ) ;
	DXFUNC_END
	return Result ;
}
extern int SetWindowMinSize( int MinWidth, int MinHeight )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetWindowMinSize(  MinWidth,  MinHeight ) ;
	DXFUNC_END
	return Result ;
}
extern int SetWindowPosition( int x, int y )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetWindowPosition(  x,  y ) ;
	DXFUNC_END
	return Result ;
}
extern int SetSysCommandOffFlag( int Flag, const TCHAR *HookDllPath  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetSysCommandOffFlag( Flag, HookDllPath  ) ;
	DXFUNC_END
	return Result ;
}
extern int SetSysCommandOffFlagWithStrLen( int Flag , const TCHAR *HookDllPath, size_t HookDllPathLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetSysCommandOffFlagWithStrLen( Flag , HookDllPath, HookDllPathLength ) ;
	DXFUNC_END
	return Result ;
}
extern int SetHookWinProc( WNDPROC WinProc )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetHookWinProc( WinProc ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseHookWinProcReturnValue( int UseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseHookWinProcReturnValue( UseFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetDoubleStartValidFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetDoubleStartValidFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int CheckDoubleStart( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CheckDoubleStart() ;
	DXFUNC_END
	return Result ;
}
extern int AddMessageTakeOverWindow( HWND Window )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_AddMessageTakeOverWindow(  Window ) ;
	DXFUNC_END
	return Result ;
}
extern int SubMessageTakeOverWindow( HWND Window )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SubMessageTakeOverWindow( Window ) ;
	DXFUNC_END
	return Result ;
}

extern int SetWindowInitPosition( int x, int y )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetWindowInitPosition( x, y ) ;
	DXFUNC_END
	return Result ;
}
extern int SetNotWinFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetNotWinFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetNotDrawFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetNotDrawFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetNotSoundFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetNotSoundFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetNotInputFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetNotInputFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetDialogBoxHandle( HWND WindowHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetDialogBoxHandle( WindowHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int ChangeStreamFunction( const STREAMDATASHREDTYPE2 *StreamThread )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ChangeStreamFunction( StreamThread ) ;
	DXFUNC_END
	return Result ;
}
extern int ChangeStreamFunctionW( const STREAMDATASHREDTYPE2W *StreamThreadW )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ChangeStreamFunctionW( StreamThreadW ) ;
	DXFUNC_END
	return Result ;
}
extern int ConvertFullPath( const TCHAR *Src, TCHAR *Dest, const TCHAR *CurrentDir )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ConvertFullPath( Src, Dest, CurrentDir ) ;
	DXFUNC_END
	return Result ;
}
extern int ConvertFullPathWithStrLen( const TCHAR *Src, size_t SrcLength, TCHAR *Dest, const TCHAR *CurrentDir, size_t CurrentDirLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ConvertFullPathWithStrLen( Src, SrcLength, Dest, CurrentDir, CurrentDirLength ) ;
	DXFUNC_END
	return Result ;
}
extern int GetStreamFunctionDefault( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetStreamFunctionDefault( ) ;
	DXFUNC_END
	return Result ;
}
extern int SetWindowVisibleFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetWindowVisibleFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetWindowMinimizeFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetWindowMinimizeFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetWindowUserCloseEnableFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetWindowUserCloseEnableFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetDxLibEndPostQuitMessageFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetDxLibEndPostQuitMessageFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUserWindow( HWND WindowHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUserWindow(  WindowHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUserChildWindow( HWND WindowHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUserChildWindow( WindowHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUserWindowMessageProcessDXLibFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUserWindowMessageProcessDXLibFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseDXArchiveFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseDXArchiveFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetDXArchivePriority( int Priority )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetDXArchivePriority( Priority ) ;
	DXFUNC_END
	return Result ;
}
extern int SetDXArchiveExtension( const TCHAR *Extension )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetDXArchiveExtension( Extension ) ;
	DXFUNC_END
	return Result ;
}
extern int SetDXArchiveExtensionWithStrLen(	const TCHAR *Extension, size_t ExtensionLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetDXArchiveExtensionWithStrLen( Extension, ExtensionLength ) ;
	DXFUNC_END
	return Result ;
}
extern int SetDXArchiveKeyString( const TCHAR *KeyString )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetDXArchiveKeyString( KeyString ) ;
	DXFUNC_END
	return Result ;
}
extern int SetDXArchiveKeyStringWithStrLen(	const TCHAR *KeyString, size_t KeyStringLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetDXArchiveKeyStringWithStrLen( KeyString, KeyStringLength ) ;
	DXFUNC_END
	return Result ;
}
extern int SetBackgroundColor( int Red, int Green, int Blue )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetBackgroundColor(  Red,  Green,  Blue ) ;
	DXFUNC_END
	return Result ;
}
extern int GetBackgroundColor( int *Red, int *Green, int *Blue )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetBackgroundColor(  Red,  Green,  Blue ) ;
	DXFUNC_END
	return Result ;
}

extern int SetUseFPUPreserveFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseFPUPreserveFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetValidMousePointerWindowOutClientAreaMoveFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetValidMousePointerWindowOutClientAreaMoveFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseBackBufferTransColorFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseBackBufferTransColorFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseUpdateLayerdWindowFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseUpdateLayerdWindowFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetResourceModule( HMODULE ResourceModule )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetResourceModule( ResourceModule ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseDxLibWM_PAINTProcess( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseDxLibWM_PAINTProcess( Flag ) ;
	DXFUNC_END
	return Result ;
}
#ifndef DX_NON_ASYNCLOAD
extern int SetUseASyncLoadFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseASyncLoadFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetUseASyncLoadFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetUseASyncLoadFlag() ;
	DXFUNC_END
	return Result ;
}
extern int CheckHandleASyncLoad( int Handle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CheckHandleASyncLoad( Handle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetHandleASyncLoadResult( int Handle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetHandleASyncLoadResult( Handle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetASyncLoadFinishDeleteFlag( int Handle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetASyncLoadFinishDeleteFlag( Handle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetASyncLoadNum( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetASyncLoadNum() ;
	DXFUNC_END
	return Result ;
}
extern int SetASyncLoadThreadNum( int ThreadNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetASyncLoadThreadNum( ThreadNum ) ;
	DXFUNC_END
	return Result ;
}

#endif // DX_NON_ASYNCLOAD

extern int SetDeleteHandleFlag( int Handle, int *DeleteFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetDeleteHandleFlag( Handle, DeleteFlag ) ;
	DXFUNC_END
	return Result ;
}

extern int GetClipboardText( TCHAR *DestBuffer )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetClipboardText( DestBuffer ) ;
	DXFUNC_END
	return Result ;
}
extern int SetClipboardText( const TCHAR *Text )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetClipboardText( Text ) ;
	DXFUNC_END
	return Result ;
}
extern int SetClipboardTextWithStrLen( const TCHAR *Text, size_t TextLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetClipboardTextWithStrLen( Text, TextLength ) ;
	DXFUNC_END
	return Result ;
}


// ドラッグ＆ドロップされたファイル関係
extern int SetDragFileValidFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetDragFileValidFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int DragFileInfoClear( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DragFileInfoClear( ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDragFilePath( TCHAR *FilePathBuffer )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDragFilePath( FilePathBuffer ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDragFileNum( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDragFileNum( ) ;
	DXFUNC_END
	return Result ;
}

// ウインドウ描画領域設定系関数
extern	HRGN		CreateRgnFromGraph( int Width, int Height, const void *MaskData, int Pitch, int Byte )
{
	HRGN Result ;
	DXFUNC_BEGIN
	Result = NS_CreateRgnFromGraph(  Width,  Height, MaskData,  Pitch,  Byte ) ;
	DXFUNC_END
	return Result ;
}
extern	HRGN		CreateRgnFromBaseImage( BASEIMAGE *BaseImage, int TransColorR, int TransColorG, int TransColorB )
{
	HRGN Result ;
	DXFUNC_BEGIN
	Result = NS_CreateRgnFromBaseImage( BaseImage,  TransColorR,  TransColorG,  TransColorB ) ;
	DXFUNC_END
	return Result ;
}
extern int SetWindowRgnGraph( const TCHAR *FileName )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetWindowRgnGraph( FileName ) ;
	DXFUNC_END
	return Result ;
}
extern int SetWindowRgnGraphWithStrLen( const TCHAR *FileName, size_t FileNameLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetWindowRgnGraphWithStrLen( FileName, FileNameLength ) ;
	DXFUNC_END
	return Result ;
}
extern int UpdateTransColorWindowRgn( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_UpdateTransColorWindowRgn() ;
	DXFUNC_END
	return Result ;
}

// ツールバー関係
extern int SetupToolBar( const TCHAR *BitmapName, int DivNum, int ResourceID )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetupToolBar( BitmapName, DivNum, ResourceID ) ;
	DXFUNC_END
	return Result ;
}
extern int SetupToolBarWithStrLen( const TCHAR *BitmapName, size_t BitmapNameLength, int DivNum, int ResourceID )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetupToolBarWithStrLen( BitmapName, BitmapNameLength, DivNum, ResourceID ) ;
	DXFUNC_END
	return Result ;
}
extern int AddToolBarButton( int Type, int State, int ImageIndex, int ID )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_AddToolBarButton(  Type,  State,  ImageIndex,  ID ) ;
	DXFUNC_END
	return Result ;
}
extern int AddToolBarSep( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_AddToolBarSep(  ) ;
	DXFUNC_END
	return Result ;
}
extern int GetToolBarButtonState( int ID )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetToolBarButtonState( ID ) ;
	DXFUNC_END
	return Result ;
}
extern int SetToolBarButtonState( int ID, int State )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetToolBarButtonState(  ID,  State ) ;
	DXFUNC_END
	return Result ;
}
extern int DeleteAllToolBarButton( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DeleteAllToolBarButton( ) ;
	DXFUNC_END
	return Result ;
}


// メニュー関係
extern int SetUseMenuFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseMenuFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseKeyAccelFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseKeyAccelFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}

extern int AddKeyAccel( const TCHAR *ItemName, int ItemID, int KeyCode, int CtrlFlag, int AltFlag, int ShiftFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_AddKeyAccel( ItemName,  ItemID,  KeyCode,  CtrlFlag,  AltFlag,  ShiftFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int AddKeyAccelWithStrLen( const TCHAR *ItemName, size_t ItemNameLength, int ItemID , int KeyCode , int CtrlFlag , int AltFlag , int ShiftFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_AddKeyAccelWithStrLen( ItemName, ItemNameLength, ItemID , KeyCode , CtrlFlag , AltFlag , ShiftFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int AddKeyAccel_Name( const TCHAR *ItemName, int KeyCode, int CtrlFlag, int AltFlag, int ShiftFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_AddKeyAccel_Name( ItemName,  KeyCode,  CtrlFlag,  AltFlag,  ShiftFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int AddKeyAccel_NameWithStrLen( const TCHAR *ItemName, size_t ItemNameLength, int KeyCode , int CtrlFlag , int AltFlag , int ShiftFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_AddKeyAccel_NameWithStrLen( ItemName, ItemNameLength, KeyCode , CtrlFlag , AltFlag , ShiftFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int AddKeyAccel_ID( int ItemID, int KeyCode, int CtrlFlag, int AltFlag, int ShiftFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_AddKeyAccel_ID(  ItemID,  KeyCode,  CtrlFlag,  AltFlag,  ShiftFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int ClearKeyAccel( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ClearKeyAccel(  ) ;
	DXFUNC_END
	return Result ;
}

extern int AddMenuItem( int AddType/*MENUITEM_ADD_CHILD等*/, const TCHAR *ItemName, int ItemID,
									int SeparatorFlag, const TCHAR *NewItemName , int NewItemID  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_AddMenuItem(  AddType, ItemName,  ItemID,
									 SeparatorFlag, NewItemName ,  NewItemID  ) ;
	DXFUNC_END
	return Result ;
}
extern int AddMenuItemWithStrLen( int AddType, const TCHAR *ItemName, size_t ItemNameLength, int ItemID, int SeparatorFlag, const TCHAR *NewItemName, size_t NewItemNameLength, int NewItemID )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_AddMenuItemWithStrLen( AddType, ItemName, ItemNameLength, ItemID, SeparatorFlag, NewItemName, NewItemNameLength, NewItemID ) ;
	DXFUNC_END
	return Result ;
}
extern int DeleteMenuItem( const TCHAR *ItemName, int ItemID )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DeleteMenuItem( ItemName,  ItemID ) ;
	DXFUNC_END
	return Result ;
}
extern int DeleteMenuItemWithStrLen( const TCHAR *ItemName, size_t ItemNameLength, int ItemID )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DeleteMenuItemWithStrLen( ItemName, ItemNameLength, ItemID ) ;
	DXFUNC_END
	return Result ;
}
extern int CheckMenuItemSelect( const TCHAR *ItemName, int ItemID ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CheckMenuItemSelect( ItemName, ItemID )  ;
	DXFUNC_END
	return Result ;
}
extern int CheckMenuItemSelectWithStrLen( const TCHAR *ItemName, size_t ItemNameLength, int ItemID )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CheckMenuItemSelectWithStrLen( ItemName, ItemNameLength, ItemID ) ;
	DXFUNC_END
	return Result ;
}
extern int SetMenuItemEnable( const TCHAR *ItemName, int ItemID, int EnableFlag ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetMenuItemEnable( ItemName, ItemID, EnableFlag )  ;
	DXFUNC_END
	return Result ;
}
extern int SetMenuItemEnableWithStrLen( const TCHAR *ItemName, size_t ItemNameLength, int ItemID, int EnableFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetMenuItemEnableWithStrLen( ItemName, ItemNameLength, ItemID, EnableFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetMenuItemMark( const TCHAR *ItemName, int ItemID, int Mark ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetMenuItemMark( ItemName,  ItemID,  Mark )  ;
	DXFUNC_END
	return Result ;
}
extern int SetMenuItemMarkWithStrLen( const TCHAR *ItemName, size_t ItemNameLength, int ItemID, int Mark )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetMenuItemMarkWithStrLen( ItemName, ItemNameLength, ItemID, Mark ) ;
	DXFUNC_END
	return Result ;
}
extern int CheckMenuItemSelectAll( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CheckMenuItemSelectAll( ) ;
	DXFUNC_END
	return Result ;
}

extern int AddMenuItem_Name( const TCHAR *ParentItemName, const TCHAR *NewItemName ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_AddMenuItem_Name( ParentItemName, NewItemName )  ;
	DXFUNC_END
	return Result ;
}
extern int AddMenuItem_NameWithStrLen( const TCHAR *ParentItemName, size_t ParentItemNameLength, const TCHAR *NewItemName, size_t NewItemNameLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_AddMenuItem_NameWithStrLen( ParentItemName, ParentItemNameLength, NewItemName, NewItemNameLength ) ;
	DXFUNC_END
	return Result ;
}
extern int AddMenuLine_Name( const TCHAR *ParentItemName ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_AddMenuLine_Name( ParentItemName )  ;
	DXFUNC_END
	return Result ;
}
extern int AddMenuLine_NameWithStrLen( const TCHAR *ParentItemName, size_t ParentItemNameLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_AddMenuLine_NameWithStrLen( ParentItemName, ParentItemNameLength ) ;
	DXFUNC_END
	return Result ;
}
extern int InsertMenuItem_Name( const TCHAR *ItemName, const TCHAR *NewItemName ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_InsertMenuItem_Name( ItemName, NewItemName )  ;
	DXFUNC_END
	return Result ;
}
extern int InsertMenuItem_NameWithStrLen( const TCHAR *ItemName, size_t ItemNameLength, const TCHAR *NewItemName, size_t NewItemNameLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_InsertMenuItem_NameWithStrLen( ItemName, ItemNameLength, NewItemName, NewItemNameLength ) ;
	DXFUNC_END
	return Result ;
}
extern int InsertMenuLine_Name( const TCHAR *ItemName ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_InsertMenuLine_Name( ItemName )  ;
	DXFUNC_END
	return Result ;
}
extern int InsertMenuLine_NameWithStrLen( const TCHAR *ItemName, size_t ItemNameLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_InsertMenuLine_NameWithStrLen( ItemName, ItemNameLength ) ;
	DXFUNC_END
	return Result ;
}
extern int DeleteMenuItem_Name( const TCHAR *ItemName ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DeleteMenuItem_Name( ItemName )  ;
	DXFUNC_END
	return Result ;
}
extern int DeleteMenuItem_NameWithStrLen( const TCHAR *ItemName, size_t ItemNameLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DeleteMenuItem_NameWithStrLen( ItemName, ItemNameLength ) ;
	DXFUNC_END
	return Result ;
}
extern int CheckMenuItemSelect_Name( const TCHAR *ItemName ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CheckMenuItemSelect_Name( ItemName )  ;
	DXFUNC_END
	return Result ;
}
extern int CheckMenuItemSelect_NameWithStrLen( const TCHAR *ItemName, size_t ItemNameLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CheckMenuItemSelect_NameWithStrLen( ItemName, ItemNameLength ) ;
	DXFUNC_END
	return Result ;
}
extern int SetMenuItemEnable_Name( const TCHAR *ItemName, int EnableFlag ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetMenuItemEnable_Name( ItemName, EnableFlag )  ;
	DXFUNC_END
	return Result ;
}
extern int SetMenuItemEnable_NameWithStrLen( const TCHAR *ItemName, size_t ItemNameLength, int EnableFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetMenuItemEnable_NameWithStrLen( ItemName, ItemNameLength, EnableFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetMenuItemMark_Name( const TCHAR *ItemName, int Mark ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetMenuItemMark_Name( ItemName, Mark )  ;
	DXFUNC_END
	return Result ;
}
extern int SetMenuItemMark_NameWithStrLen( const TCHAR *ItemName, size_t ItemNameLength, int Mark )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetMenuItemMark_NameWithStrLen( ItemName, ItemNameLength, Mark ) ;
	DXFUNC_END
	return Result ;
}

extern int AddMenuItem_ID( int ParentItemID, const TCHAR *NewItemName, int NewItemID  ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_AddMenuItem_ID( ParentItemID, NewItemName, NewItemID  )  ;
	DXFUNC_END
	return Result ;
}
extern int AddMenuItem_IDWithStrLen( int ParentItemID, const TCHAR *NewItemName, size_t NewItemNameLength, int NewItemID )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_AddMenuItem_IDWithStrLen( ParentItemID, NewItemName, NewItemNameLength, NewItemID ) ;
	DXFUNC_END
	return Result ;
}
extern int AddMenuLine_ID( int ParentItemID ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_AddMenuLine_ID( ParentItemID )  ;
	DXFUNC_END
	return Result ;
}
extern int InsertMenuItem_ID( int ItemID, int NewItemID ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_InsertMenuItem_ID(  ItemID,  NewItemID )  ;
	DXFUNC_END
	return Result ;
}
extern int InsertMenuLine_ID( int ItemID, int NewItemID ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_InsertMenuLine_ID(  ItemID,  NewItemID )  ;
	DXFUNC_END
	return Result ;
}
extern int DeleteMenuItem_ID( int ItemID ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DeleteMenuItem_ID(  ItemID )  ;
	DXFUNC_END
	return Result ;
}
extern int CheckMenuItemSelect_ID( int ItemID ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CheckMenuItemSelect_ID(  ItemID )  ;
	DXFUNC_END
	return Result ;
}
extern int SetMenuItemEnable_ID( int ItemID, int EnableFlag ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetMenuItemEnable_ID(  ItemID,  EnableFlag )  ;
	DXFUNC_END
	return Result ;
}
extern int SetMenuItemMark_ID( int ItemID, int Mark ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetMenuItemMark_ID(  ItemID,  Mark )  ;
	DXFUNC_END
	return Result ;
}

extern int DeleteMenuItemAll( void ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DeleteMenuItemAll( )  ;
	DXFUNC_END
	return Result ;
}
extern int ClearMenuItemSelect( void ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ClearMenuItemSelect( )  ;
	DXFUNC_END
	return Result ;
}
extern int GetMenuItemID( const TCHAR *ItemName ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetMenuItemID( ItemName )  ;
	DXFUNC_END
	return Result ;
}
extern int GetMenuItemIDWithStrLen( const TCHAR *ItemName, size_t ItemNameLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetMenuItemIDWithStrLen( ItemName, ItemNameLength ) ;
	DXFUNC_END
	return Result ;
}
extern int GetMenuItemName( int ItemID, TCHAR *NameBuffer ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetMenuItemName( ItemID, NameBuffer )  ;
	DXFUNC_END
	return Result ;
}
extern int 		LoadMenuResource( int MenuResourceID ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadMenuResource( MenuResourceID )  ;
	DXFUNC_END
	return Result ;
}
extern int SetMenuItemSelectCallBackFunction( void (*CallBackFunction)( const TCHAR *ItemName, int ItemID ) ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetMenuItemSelectCallBackFunction( CallBackFunction )  ;
	DXFUNC_END
	return Result ;
}

extern int SetWindowMenu( int MenuID, int (*MenuProc)( WORD ID ) ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetWindowMenu( MenuID, MenuProc )  ;
	DXFUNC_END
	return Result ;
}
extern int SetDisplayMenuFlag( int Flag ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetDisplayMenuFlag( Flag )  ;
	DXFUNC_END
	return Result ;
}
extern int GetDisplayMenuFlag( void ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDisplayMenuFlag( )  ;
	DXFUNC_END
	return Result ;
}
extern int GetUseMenuFlag( void ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetUseMenuFlag(  )  ;
	DXFUNC_END
	return Result ;
}
extern int SetAutoMenuDisplayFlag( int Flag ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetAutoMenuDisplayFlag( Flag )  ;
	DXFUNC_END
	return Result ;
}

// マウス関係関数
extern int SetMouseDispFlag( int DispFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetMouseDispFlag( DispFlag ) ;
	DXFUNC_END
	return Result ;
}
#ifndef DX_NON_INPUT
extern int GetMousePoint( int *XBuf, int *YBuf )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetMousePoint( XBuf, YBuf ) ;
	DXFUNC_END
	return Result ;
}
extern int SetMousePoint( int PointX, int PointY )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetMousePoint( PointX, PointY ) ;
	DXFUNC_END
	return Result ;
}
extern int GetMouseInput( void ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetMouseInput( )  ;
	DXFUNC_END
	return Result ;
}
extern int GetMouseWheelRotVol( int CounterReset ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetMouseWheelRotVol( CounterReset )  ;
	DXFUNC_END
	return Result ;
}
extern int GetMouseHWheelRotVol( int CounterReset )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetMouseHWheelRotVol( CounterReset )  ;
	DXFUNC_END
	return Result ;
}
extern float GetMouseWheelRotVolF( int CounterReset )
{
	float Result ;
	DXFUNC_BEGIN
	Result = NS_GetMouseWheelRotVolF( CounterReset )  ;
	DXFUNC_END
	return Result ;
}
extern float GetMouseHWheelRotVolF(	int CounterReset )
{
	float Result ;
	DXFUNC_BEGIN
	Result = NS_GetMouseHWheelRotVolF( CounterReset )  ;
	DXFUNC_END
	return Result ;
}
extern int GetMouseInputLog( int *Button, int *ClickX, int *ClickY, int LogDelete )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetMouseInputLog( Button, ClickX, ClickY, LogDelete ) ;
	DXFUNC_END
	return Result ;
}
extern int GetMouseInputLog2( int *Button, int *ClickX, int *ClickY, int *LogType, int LogDelete )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetMouseInputLog2( Button, ClickX, ClickY, LogType, LogDelete ) ;
	DXFUNC_END
	return Result ;
}
#endif // DX_NON_INPUT

// タッチパネル入力関係関数
#ifndef DX_NON_INPUT
extern	int	GetTouchInputNum( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetTouchInputNum() ;
	DXFUNC_END
	return Result ;
}

extern	int	GetTouchInput( int InputNo, int *PositionX, int *PositionY, int *ID , int *Device )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetTouchInput( InputNo, PositionX, PositionY, ID , Device ) ;
	DXFUNC_END
	return Result ;
}

extern	int	GetTouchInputLogNum( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetTouchInputLogNum() ;
	DXFUNC_END
	return Result ;
}

extern	int	ClearTouchInputLog( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ClearTouchInputLog() ;
	DXFUNC_END
	return Result ;
}

extern	TOUCHINPUTDATA	GetTouchInputLogOne( int PeekFlag )
{
	TOUCHINPUTDATA Result ;
	DXFUNC_BEGIN
	Result = NS_GetTouchInputLogOne( PeekFlag ) ;
	DXFUNC_END
	return Result ;
}

extern	int	GetTouchInputLog( TOUCHINPUTDATA *TouchData, int GetNum, int PeekFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetTouchInputLog( TouchData, GetNum, PeekFlag ) ;
	DXFUNC_END
	return Result ;
}

extern	int	GetTouchInputDownLogNum( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetTouchInputDownLogNum() ;
	DXFUNC_END
	return Result ;
}

extern	int	ClearTouchInputDownLog( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ClearTouchInputDownLog() ;
	DXFUNC_END
	return Result ;
}

extern	TOUCHINPUTPOINT	GetTouchInputDownLogOne( int PeekFlag )
{
	TOUCHINPUTPOINT Result ;
	DXFUNC_BEGIN
	Result = NS_GetTouchInputDownLogOne( PeekFlag ) ;
	DXFUNC_END
	return Result ;
}

extern	int	GetTouchInputDownLog( TOUCHINPUTPOINT *PointData, int GetNum, int PeekFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetTouchInputDownLog( PointData, GetNum, PeekFlag ) ;
	DXFUNC_END
	return Result ;
}

extern	int	GetTouchInputUpLogNum( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetTouchInputUpLogNum() ;
	DXFUNC_END
	return Result ;
}

extern	int	ClearTouchInputUpLog( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ClearTouchInputUpLog() ;
	DXFUNC_END
	return Result ;
}

extern	TOUCHINPUTPOINT	GetTouchInputUpLogOne( int PeekFlag )
{
	TOUCHINPUTPOINT Result ;
	DXFUNC_BEGIN
	Result = NS_GetTouchInputUpLogOne( PeekFlag ) ;
	DXFUNC_END
	return Result ;
}

extern	int	GetTouchInputUpLog( TOUCHINPUTPOINT *PointData, int GetNum, int PeekFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetTouchInputUpLog( PointData, GetNum, PeekFlag ) ;
	DXFUNC_END
	return Result ;
}

#endif // DX_NON_INPUT

// ウエイト系関数
extern int WaitTimer( int WaitTime ) 
{
	int Result ;
	Result = NS_WaitTimer( WaitTime )  ;
	return Result ;
}
#ifndef DX_NON_INPUT
extern int WaitKey( void ) 
{
	int Result ;
	Result = NS_WaitKey(  )  ;
	return Result ;
}
#endif // DX_NON_INPUT

// カウンタ及び時刻取得系関数
extern int GetNowCount( int UseRDTSCFlag  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetNowCount( UseRDTSCFlag  ) ;
	DXFUNC_END
	return Result ;
}
extern	LONGLONG	GetNowHiPerformanceCount( int UseRDTSCFlag  )
{
	LONGLONG Result ;
//	DXFUNC_BEGIN
	Result = NS_GetNowHiPerformanceCount( UseRDTSCFlag  ) ;
//	DXFUNC_END
	return Result ;
}
extern int GetDateTime( DATEDATA *DateBuf )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDateTime( DateBuf ) ;
	DXFUNC_END
	return Result ;
}

// 乱数取得
extern int GetRand( int RandMax )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetRand( RandMax ) ;
	DXFUNC_END
	return Result ;
}
extern int SRand( int Seed )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SRand( Seed ) ;
	DXFUNC_END
	return Result ;
}

// バッテリー関連
extern int GetBatteryLifePercent( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetBatteryLifePercent() ;
	DXFUNC_END
	return Result ;
}
#endif

#ifdef DX_THREAD_SAFE

#ifndef DX_NON_NETWORK

// 通信関係
extern int ProcessNetMessage( int RunReleaseProcess  )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_ProcessNetMessage( RunReleaseProcess  ) ;
	DXFUNC_NET_END
	return Result ;
}

extern int GetHostIPbyName( const TCHAR *HostName, IPDATA *IPDataBuf, int IPDataBufLength , int *IPDataGetNum )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_GetHostIPbyName( HostName, IPDataBuf, IPDataBufLength, IPDataGetNum ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int GetHostIPbyNameWithStrLen( const TCHAR *HostName, size_t HostNameLength,	IPDATA *IPDataBuf, int IPDataBufLength , int *IPDataGetNum )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_GetHostIPbyNameWithStrLen( HostName, HostNameLength, IPDataBuf, IPDataBufLength, IPDataGetNum ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int GetHostIPbyName_IPv6( const TCHAR *HostName, IPDATA_IPv6 *IPDataBuf, int IPDataBufLength , int *IPDataGetNum )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_GetHostIPbyName_IPv6( HostName, IPDataBuf, IPDataBufLength, IPDataGetNum ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int GetHostIPbyName_IPv6WithStrLen( const TCHAR *HostName, size_t HostNameLength, IPDATA_IPv6 *IPDataBuf, int IPDataBufLength , int *IPDataGetNum )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_GetHostIPbyName_IPv6WithStrLen( HostName, HostNameLength, IPDataBuf, IPDataBufLength, IPDataGetNum ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int ConnectNetWork( IPDATA IPData, int Port )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_ConnectNetWork( IPData, Port ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int ConnectNetWork_IPv6( IPDATA_IPv6 IPData, int Port )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_ConnectNetWork_IPv6( IPData, Port ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int ConnectNetWork_ASync( IPDATA IPData, int Port )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_ConnectNetWork_ASync( IPData, Port ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int ConnectNetWork_IPv6_ASync( IPDATA_IPv6 IPData, int Port )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_ConnectNetWork_IPv6_ASync( IPData, Port ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int PreparationListenNetWork( int Port  )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_PreparationListenNetWork( Port  ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int PreparationListenNetWork_IPv6( int Port  )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_PreparationListenNetWork_IPv6( Port  ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int StopListenNetWork( void )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_StopListenNetWork(  ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int CloseNetWork( int NetHandle )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_CloseNetWork( NetHandle ) ;
	DXFUNC_NET_END
	return Result ;
}

extern int GetNetWorkAcceptState( int NetHandle )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_GetNetWorkAcceptState( NetHandle ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int GetNetWorkDataLength( int NetHandle )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_GetNetWorkDataLength( NetHandle ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int GetNetWorkSendDataLength( int NetHandle )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_GetNetWorkSendDataLength( NetHandle ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int GetNewAcceptNetWork( void )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_GetNewAcceptNetWork(  ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int GetLostNetWork( void )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_GetLostNetWork(  ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int GetNetWorkIP( int NetHandle, IPDATA *IpBuf )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_GetNetWorkIP( NetHandle, IpBuf ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int GetNetWorkIP_IPv6( int NetHandle, IPDATA_IPv6 *IpBuf )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_GetNetWorkIP_IPv6( NetHandle, IpBuf ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int GetMyIPAddress( IPDATA *IpBuf, int IpBufLength, int *IpNum )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_GetMyIPAddress( IpBuf, IpBufLength, IpNum ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int GetMyIPAddress_IPv6( IPDATA_IPv6 *IpBuf, int IpBufLength, int *IpNum )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_GetMyIPAddress_IPv6( IpBuf, IpBufLength, IpNum ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int SetConnectTimeOutWait( int Time )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_SetConnectTimeOutWait( Time ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int SetUseDXNetWorkProtocol( int Flag )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_SetUseDXNetWorkProtocol( Flag ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int GetUseDXNetWorkProtocol( void )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_GetUseDXNetWorkProtocol(  ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int SetUseDXProtocol( int Flag )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_SetUseDXProtocol( Flag ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int GetUseDXProtocol( void )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_GetUseDXProtocol(  ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int SetNetWorkCloseAfterLostFlag( int Flag )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_SetNetWorkCloseAfterLostFlag( Flag ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int GetNetWorkCloseAfterLostFlag( void )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_GetNetWorkCloseAfterLostFlag(  ) ;
	DXFUNC_NET_END
	return Result ;
}
/*
extern int SetProxySetting( int UseFlag, const TCHAR *Address, int Port )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_SetProxySetting( UseFlag, Address, Port ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int GetProxySetting( int *UseFlagBuffer, TCHAR *AddressBuffer, int *PortBuffer )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_GetProxySetting( UseFlagBuffer, AddressBuffer, PortBuffer ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int SetIEProxySetting( void )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_SetIEProxySetting(  ) ;
	DXFUNC_NET_END
	return Result ;
}
*/

extern int NetWorkRecv( int NetHandle, void *Buffer, int Length )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_NetWorkRecv( NetHandle, Buffer, Length ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int NetWorkRecvToPeek( int NetHandle, void *Buffer, int Length )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_NetWorkRecvToPeek( NetHandle, Buffer, Length ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int NetWorkRecvBufferClear( int NetHandle )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_NetWorkRecvBufferClear( NetHandle ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int NetWorkSend( int NetHandle, const void *Buffer, int Length )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_NetWorkSend( NetHandle, Buffer, Length ) ;
	DXFUNC_NET_END
	return Result ;
}

extern int MakeUDPSocket( int RecvPort )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_MakeUDPSocket(  RecvPort ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int MakeUDPSocket_IPv6( int RecvPort )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_MakeUDPSocket_IPv6(  RecvPort ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int DeleteUDPSocket( int NetUDPHandle )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_DeleteUDPSocket(  NetUDPHandle ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int NetWorkSendUDP( int NetUDPHandle, IPDATA SendIP, int SendPort, const void *Buffer, int Length )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_NetWorkSendUDP(  NetUDPHandle,  SendIP, SendPort, Buffer,  Length ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int NetWorkSendUDP_IPv6( int NetUDPHandle, IPDATA_IPv6 SendIP, int SendPort, const void *Buffer, int Length )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_NetWorkSendUDP_IPv6(  NetUDPHandle,  SendIP, SendPort, Buffer,  Length ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int NetWorkRecvUDP( int NetUDPHandle, IPDATA *RecvIP, int *RecvPort, void *Buffer, int Length, int Peek )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_NetWorkRecvUDP(  NetUDPHandle, RecvIP, RecvPort, Buffer,  Length, Peek ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int NetWorkRecvUDP_IPv6( int NetUDPHandle, IPDATA_IPv6 *RecvIP, int *RecvPort, void *Buffer, int Length, int Peek )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_NetWorkRecvUDP_IPv6(  NetUDPHandle, RecvIP, RecvPort, Buffer,  Length, Peek ) ;
	DXFUNC_NET_END
	return Result ;
}
/*
extern int CheckNetWorkSendUDP( int NetUDPHandle )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_CheckNetWorkSendUDP( NetUDPHandle ) ;
	DXFUNC_NET_END
	return Result ;
}
*/
extern int CheckNetWorkRecvUDP( int NetUDPHandle )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_CheckNetWorkRecvUDP( NetUDPHandle ) ;
	DXFUNC_NET_END
	return Result ;
}

/*
extern int HTTP_FileDownload( const char *FileURL, const char *SavePath ,
										 void **SaveBufferP , int *FileSize ,
										 char **ParamList  )
{
	int Result ;
	Result = NS_HTTP_FileDownload( FileURL, SavePath ,
										 SaveBufferP , FileSize ,
										 ParamList  ) ;
	return Result ;
}
extern int HTTP_GetFileSize( const char *FileURL )
{
	int Result ;
	Result = NS_HTTP_GetFileSize( FileURL ) ;
	return Result ;
}

extern int HTTP_StartFileDownload( const char *FileURL, const char *SavePath, void **SaveBufferP, char **ParamList  )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_HTTP_StartFileDownload( FileURL, SavePath, SaveBufferP, ParamList  ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int HTTP_StartGetFileSize( const char *FileURL )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_HTTP_StartGetFileSize( FileURL ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int HTTP_Close( int HttpHandle )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_HTTP_Close( HttpHandle ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int HTTP_CloseAll( void )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_HTTP_CloseAll(  ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int HTTP_GetState( int HttpHandle )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_HTTP_GetState( HttpHandle ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int HTTP_GetError( int HttpHandle )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_HTTP_GetError( HttpHandle ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int HTTP_GetDownloadFileSize( int HttpHandle )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_HTTP_GetDownloadFileSize( HttpHandle ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int HTTP_GetDownloadedFileSize( int HttpHandle )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_HTTP_GetDownloadedFileSize( HttpHandle ) ;
	DXFUNC_NET_END
	return Result ;
}

extern int fgetsForNetHandle( int NetHandle, char *strbuffer )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_fgetsForNetHandle( NetHandle, strbuffer ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int URLAnalys( const char *URL, char *HostBuf , char *PathBuf ,
												 char *FileNameBuf , int *PortBuf  )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_URLAnalys( URL, HostBuf , PathBuf ,  FileNameBuf , PortBuf  ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int URLConvert( char *URL, int ParamConvert , int NonConvert  )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_URLConvert( URL, ParamConvert , NonConvert  ) ;
	DXFUNC_NET_END
	return Result ;
}
extern int URLParamAnalysis( char **ParamList, char **ParamStringP )
{
	int Result ;
	DXFUNC_NET_BEGIN
	Result = NS_URLParamAnalysis( ParamList, ParamStringP ) ;
	DXFUNC_NET_END
	return Result ;
}
*/
#endif

#endif

#ifdef DX_THREAD_SAFE

#ifndef DX_NON_INPUTSTRING

// 文字コードバッファ操作関係
extern int StockInputChar( TCHAR CharCode )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_StockInputChar( CharCode ) ;
	DXFUNC_END
	return Result ;
}
extern int ClearInputCharBuf( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ClearInputCharBuf(  ) ;
	DXFUNC_END
	return Result ;
}
extern	TCHAR		GetInputChar( int DeleteFlag )
{
	TCHAR Result ;
	DXFUNC_BEGIN
	Result = NS_GetInputChar( DeleteFlag ) ;
	DXFUNC_END
	return Result ;
}
extern	TCHAR		GetInputCharWait( int DeleteFlag )
{
	TCHAR Result ;
	DXFUNC_BEGIN
	Result = NS_GetInputCharWait( DeleteFlag ) ;
	DXFUNC_END
	return Result ;
}

extern int GetOneChar( TCHAR *CharBuffer, int DeleteFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetOneChar( CharBuffer, DeleteFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetOneCharWait( TCHAR *CharBuffer, int DeleteFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetOneCharWait( CharBuffer, DeleteFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetCtrlCodeCmp( TCHAR Char )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetCtrlCodeCmp( Char ) ;
	DXFUNC_END
	return Result ;
}

#endif // DX_NON_INPUTSTRING

#ifndef DX_NON_KEYEX

extern int DrawIMEInputString( int x, int y, int SelectStringNum , int DrawCandidateList )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawIMEInputString( x,  y,  SelectStringNum , DrawCandidateList ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseIMEFlag( int UseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseIMEFlag( UseFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetUseIMEFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetUseIMEFlag() ;
	DXFUNC_END
	return Result ;
}
extern int SetInputStringMaxLengthIMESync( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetInputStringMaxLengthIMESync( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetIMEInputStringMaxLength( int Length )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetIMEInputStringMaxLength( Length ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseTSFFlag( int UseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseTSFFlag( UseFlag ) ;
	DXFUNC_END
	return Result ;
}



#endif

extern int GetStringPoint( const TCHAR *String, int Point )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetStringPoint( String, Point ) ;
	DXFUNC_END
	return Result ;
}
extern int GetStringPointWithStrLen( const TCHAR *String, size_t StringLength, int Point )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetStringPointWithStrLen( String, StringLength, Point ) ;
	DXFUNC_END
	return Result ;
}
extern int GetStringPoint2( const TCHAR *String, int Point )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetStringPoint2( String, Point ) ;
	DXFUNC_END
	return Result ;
}
extern int GetStringPoint2WithStrLen( const TCHAR *String, size_t StringLength, int Point )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetStringPoint2WithStrLen( String, StringLength, Point ) ;
	DXFUNC_END
	return Result ;
}
extern int GetStringLength( const TCHAR *String )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetStringLength( String ) ;
	DXFUNC_END
	return Result ;
}

#ifndef DX_NON_FONT
extern int DrawObtainsString( int x, int y, int AddY, const TCHAR *String, unsigned int StrColor , unsigned int StrEdgeColor , int FontHandle , unsigned int SelectBackColor , unsigned int SelectStrColor , unsigned int SelectStrEdgeColor , int SelectStart , int SelectEnd )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawObtainsString( x, y, AddY, String, StrColor, StrEdgeColor, FontHandle, SelectBackColor, SelectStrColor, SelectStrEdgeColor, SelectStart , SelectEnd ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawObtainsNString( int x, int y, int AddY, const TCHAR *String, size_t StringLength, unsigned int StrColor, unsigned int StrEdgeColor, int FontHandle, unsigned int SelectBackColor, unsigned int SelectStrColor, unsigned int SelectStrEdgeColor, int SelectStart, int SelectEnd )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawObtainsNString( x, y, AddY, String, StringLength, StrColor, StrEdgeColor, FontHandle, SelectBackColor, SelectStrColor, SelectStrEdgeColor, SelectStart, SelectEnd ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawObtainsString_CharClip(	int x, int y, int AddY, const TCHAR *String, unsigned int StrColor, unsigned int StrEdgeColor, int FontHandle, unsigned int SelectBackColor, unsigned int SelectStrColor, unsigned int SelectStrEdgeColor, int SelectStart, int SelectEnd )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawObtainsString_CharClip(	 x,  y,  AddY, String,  StrColor,  StrEdgeColor,  FontHandle,  SelectBackColor,  SelectStrColor,  SelectStrEdgeColor,  SelectStart,  SelectEnd ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawObtainsNString_CharClip( int x, int y, int AddY, const TCHAR *String, size_t StringLength, unsigned int StrColor, unsigned int StrEdgeColor, int FontHandle, unsigned int SelectBackColor, unsigned int SelectStrColor, unsigned int SelectStrEdgeColor, int SelectStart, int SelectEnd )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawObtainsNString_CharClip( x, y, AddY, String, StringLength, StrColor, StrEdgeColor, FontHandle, SelectBackColor, SelectStrColor, SelectStrEdgeColor, SelectStart, SelectEnd ) ;
	DXFUNC_END
	return Result ;
}
extern int GetObtainsStringCharPosition( int x, int y, int AddY, const TCHAR *String, int StrLen, int *PosX, int *PosY, int FontHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetObtainsStringCharPosition( x, y, AddY, String, StrLen, PosX, PosY, FontHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetObtainsStringCharPosition_CharClip( int x, int y, int AddY, const TCHAR *String, int StrLen, int *PosX, int *PosY, int FontHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetObtainsStringCharPosition_CharClip( x, y, AddY, String, StrLen, PosX, PosY, FontHandle ) ;
	DXFUNC_END
	return Result ;
}

#endif // DX_NON_FONT
extern int DrawObtainsBox( int x1, int y1, int x2, int y2, int AddY, unsigned int Color, int FillFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawObtainsBox( x1, y1, x2, y2, AddY, Color, FillFlag ) ;
	DXFUNC_END
	return Result ;
}

#ifndef DX_NON_KEYEX

extern int InputStringToCustom( int x, int y, size_t BufLength, TCHAR *StrBuffer, int CancelValidFlag, int SingleCharOnlyFlag, int NumCharOnlyFlag, int DoubleCharOnlyFlag , int EnableNewLineFlag , int DisplayCandidateList )
{
	int Result ;
	Result = NS_InputStringToCustom( x, y, BufLength, StrBuffer, CancelValidFlag, SingleCharOnlyFlag, NumCharOnlyFlag, DoubleCharOnlyFlag , EnableNewLineFlag , DisplayCandidateList ) ;
	return Result ;
}

extern int KeyInputString( int x, int y, size_t CharMaxLength, TCHAR *StrBuffer, int CancelValidFlag )
{
	int Result ;
	Result = NS_KeyInputString( x, y, CharMaxLength, StrBuffer, CancelValidFlag ) ;
	return Result ;
}
extern int KeyInputSingleCharString( int x, int y, size_t CharMaxLength, TCHAR *StrBuffer, int CancelValidFlag )
{
	int Result ;
	Result = NS_KeyInputSingleCharString( x, y, CharMaxLength, StrBuffer, CancelValidFlag ) ;
	return Result ;
}
extern int KeyInputNumber( int x, int y, int MaxNum, int MinNum, int CancelValidFlag )
{
	int Result ;
	Result = NS_KeyInputNumber( x, y, MaxNum, MinNum, CancelValidFlag ) ;
	return Result ;
}

extern int GetIMEInputModeStr( TCHAR *GetBuffer )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetIMEInputModeStr( GetBuffer ) ;
	DXFUNC_END
	return Result ;
}

extern const IMEINPUTDATA *GetIMEInputData( void )
{
	const IMEINPUTDATA *Result ;
	DXFUNC_BEGIN
	Result = NS_GetIMEInputData() ;
	DXFUNC_END
	return Result ;
}

extern int SetKeyInputStringColor( ULONGLONG NmlStr, ULONGLONG NmlCur, ULONGLONG IMEStrBack, ULONGLONG IMECur, ULONGLONG IMELine, ULONGLONG IMESelectStr, ULONGLONG IMEModeStr , ULONGLONG NmlStrE, ULONGLONG IMESelectStrE, ULONGLONG IMEModeStrE, ULONGLONG IMESelectWinE, ULONGLONG IMESelectWinF, ULONGLONG SelectStrBackColor , ULONGLONG SelectStrColor , ULONGLONG SelectStrEdgeColor, ULONGLONG IMEStr, ULONGLONG IMEStrE )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetKeyInputStringColor(  NmlStr,  NmlCur,  IMEStrBack,  IMECur,  IMELine,  IMESelectStr,  IMEModeStr ,  NmlStrE ,  IMESelectStrE,  IMEModeStrE, IMESelectWinE, IMESelectWinF, SelectStrBackColor ,  SelectStrColor ,  SelectStrEdgeColor, IMEStr, IMEStrE ) ;
	DXFUNC_END
	return Result ;
}

extern int SetKeyInputStringColor2(	int TargetColor /* DX_KEYINPSTRCOLOR_NORMAL_STR 等 */, unsigned int Color )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetKeyInputStringColor2( TargetColor, Color ) ;
	DXFUNC_END
	return Result ;
}

extern int ResetKeyInputStringColor2(	int TargetColor /* DX_KEYINPSTRCOLOR_NORMAL_STR 等 */ )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ResetKeyInputStringColor2( TargetColor ) ;
	DXFUNC_END
	return Result ;
}

extern int SetKeyInputStringFont( int FontHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetKeyInputStringFont( FontHandle ) ;
	DXFUNC_END
	return Result ;
}

extern int SetKeyInputStringEndCharaMode( int EndCharaMode /* DX_KEYINPSTR_ENDCHARAMODE_OVERWRITE 等 */ )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetKeyInputStringEndCharaMode( EndCharaMode ) ;
	DXFUNC_END
	return Result ;
}

extern int DrawKeyInputModeString( int x, int y )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawKeyInputModeString( x, y ) ;
	DXFUNC_END
	return Result ;
}

extern int InitKeyInput( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_InitKeyInput(  ) ;
	DXFUNC_END
	return Result ;
}
extern int MakeKeyInput( size_t MaxStrLength, int CancelValidFlag, int SingleCharOnlyFlag, int NumCharOnlyFlag, int DoubleCharOnlyFlag, int EnableNewLineFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MakeKeyInput( MaxStrLength, CancelValidFlag, SingleCharOnlyFlag, NumCharOnlyFlag, DoubleCharOnlyFlag, EnableNewLineFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DeleteKeyInput( int InputHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DeleteKeyInput( InputHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetActiveKeyInput( int InputHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetActiveKeyInput( InputHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetActiveKeyInput( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetActiveKeyInput() ;
	DXFUNC_END
	return Result ;
}
extern int CheckKeyInput( int InputHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CheckKeyInput( InputHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int ReStartKeyInput( int InputHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReStartKeyInput( InputHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int ProcessActKeyInput( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ProcessActKeyInput(  ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawKeyInputString( int x, int y, int InputHandle , int DrawCandidateList )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawKeyInputString( x, y, InputHandle , DrawCandidateList ) ;
	DXFUNC_END
	return Result ;
}
extern int SetKeyInputDrawArea( int x1, int y1, int x2, int y2, int InputHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetKeyInputDrawArea(  x1,  y1,  x2,  y2,  InputHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetKeyInputSelectArea( int SelectStart, int SelectEnd, int InputHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetKeyInputSelectArea(  SelectStart,  SelectEnd,  InputHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetKeyInputSelectArea( int *SelectStart, int *SelectEnd, int InputHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetKeyInputSelectArea( SelectStart, SelectEnd,  InputHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetKeyInputDrawStartPos( int DrawStartPos, int InputHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetKeyInputDrawStartPos(  DrawStartPos,  InputHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetKeyInputDrawStartPos( int InputHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetKeyInputDrawStartPos( InputHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetKeyInputCursorBrinkTime( int Time )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetKeyInputCursorBrinkTime( Time ) ;
	DXFUNC_END
	return Result ;
}
extern int SetKeyInputCursorBrinkFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetKeyInputCursorBrinkFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetKeyInputString( const TCHAR *String, int InputHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetKeyInputString( String, InputHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetKeyInputStringWithStrLen(	const TCHAR *String, size_t StringLength, int InputHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetKeyInputStringWithStrLen( String, StringLength, InputHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetKeyInputNumber( int Number, int InputHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetKeyInputNumber( Number, InputHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetKeyInputNumberToFloat( float Number, int InputHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetKeyInputNumberToFloat( Number, InputHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetKeyInputString( TCHAR *StrBuffer, int InputHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetKeyInputString( StrBuffer, InputHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetKeyInputNumber( int InputHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetKeyInputNumber( InputHandle ) ;
	DXFUNC_END
	return Result ;
}
extern float GetKeyInputNumberToFloat( int InputHandle )
{
	float Result ;
	DXFUNC_BEGIN
	Result = NS_GetKeyInputNumberToFloat( InputHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetKeyInputCursorPosition( int Position, int InputHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetKeyInputCursorPosition( Position, InputHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetKeyInputCursorPosition( int InputHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetKeyInputCursorPosition( InputHandle ) ;
	DXFUNC_END
	return Result ;
}


#endif














// DxNetwork.cpp関数プロトタイプ宣言

#ifndef DX_NON_NETWORK

extern	int GetWinSockLastError( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetWinSockLastError() ;
	DXFUNC_END
	return Result ;
}

#endif // DX_NON_NETWORK


















// DxInput.cpp関数プロトタイプ宣言

#ifndef DX_NON_INPUT

// 入力状態取得関数
extern int CheckHitKey( int KeyCode )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CheckHitKey( KeyCode ) ;
	DXFUNC_END
	return Result ;
}
extern int CheckHitKeyAll( int CheckType )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CheckHitKeyAll( CheckType ) ;
	DXFUNC_END
	return Result ;
}
extern int GetHitKeyStateAll( char *KeyStateBuf )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetHitKeyStateAll( KeyStateBuf ) ;
	DXFUNC_END
	return Result ;
}
extern int GetJoypadNum( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetJoypadNum(  ) ;
	DXFUNC_END
	return Result ;
}
extern int GetJoypadInputState( int InputType )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetJoypadInputState( InputType ) ;
	DXFUNC_END
	return Result ;
}
extern int GetJoypadAnalogInput( int *XBuf, int *YBuf, int InputType)
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetJoypadAnalogInput( XBuf, YBuf,  InputType) ;
	DXFUNC_END
	return Result ;
}
extern int GetJoypadAnalogInputRight( int *XBuf, int *YBuf, int InputType )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetJoypadAnalogInputRight( XBuf, YBuf, InputType ) ;
	DXFUNC_END
	return Result ;
}
extern	int	GetJoypadDirectInputState( int InputType, DINPUT_JOYSTATE *DInputState )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetJoypadDirectInputState( InputType, DInputState ) ;
	DXFUNC_END
	return Result ;
}
extern	int CheckJoypadXInput( int InputType )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CheckJoypadXInput( InputType ) ;
	DXFUNC_END
	return Result ;
}
extern	int GetJoypadXInputState( int InputType, XINPUT_STATE *XInputState )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetJoypadXInputState( InputType, XInputState ) ;
	DXFUNC_END
	return Result ;
}
extern	int SetJoypadInputToKeyInput( int InputType, int PadInput, int KeyInput1, int KeyInput2, int KeyInput3, int KeyInput4 )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetJoypadInputToKeyInput(  InputType,  PadInput,  KeyInput1,  KeyInput2,  KeyInput3,  KeyInput4 ) ;
	DXFUNC_END
	return Result ;
}
extern	int SetJoypadDeadZone( int InputType, double Zone )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetJoypadDeadZone( InputType, Zone ) ;
	DXFUNC_END
	return Result ;
}
extern	double GetJoypadDeadZone( int InputType )
{
	double Result ;
	DXFUNC_BEGIN
	Result = NS_GetJoypadDeadZone( InputType ) ;
	DXFUNC_END
	return Result ;
}
extern	int StartJoypadVibration( int InputType, int Power, int Time, int EffectIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_StartJoypadVibration(  InputType,  Power,  Time, EffectIndex ) ;
	DXFUNC_END
	return Result ;
}
extern	int StopJoypadVibration( int InputType, int EffectIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_StopJoypadVibration(  InputType, EffectIndex ) ;
	DXFUNC_END
	return Result ;
}
extern	int GetJoypadPOVState( int InputType, int POVNumber )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetJoypadPOVState( InputType, POVNumber ) ;
	DXFUNC_END
	return Result ;
}
extern	int ReSetupJoypad( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReSetupJoypad( );
	DXFUNC_END
	return Result ;
}

extern int SetUseJoypadVibrationFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseJoypadVibrationFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}

#ifdef __WINDOWS__

extern int SetKeyExclusiveCooperativeLevelFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetKeyExclusiveCooperativeLevelFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}

extern int SetKeyboardNotDirectInputFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetKeyboardNotDirectInputFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}

extern int SetUseDirectInputFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseDirectInputFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}

extern int SetUseXInputFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseXInputFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}

extern int GetJoypadGUID( int PadIndex, GUID *GuidInstanceBuffer, GUID *GuidProductBuffer )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetJoypadGUID( PadIndex, GuidInstanceBuffer, GuidProductBuffer ) ;
	DXFUNC_END
	return Result ;
}

extern	int GetJoypadName( int InputType, TCHAR *InstanceNameBuffer, TCHAR *ProductNameBuffer )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetJoypadName( InputType, InstanceNameBuffer, ProductNameBuffer );
	DXFUNC_END
	return Result ;
}

extern int ConvertKeyCodeToVirtualKey( int KeyCode )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ConvertKeyCodeToVirtualKey( KeyCode ) ;
	DXFUNC_END
	return Result ;
}
extern	int ConvertVirtualKeyToKeyCode( int VirtualKey )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ConvertVirtualKeyToKeyCode( VirtualKey ) ;
	DXFUNC_END
	return Result ;
}

#endif // __WINDOWS__

#endif // DX_NON_INPUT









// DxDraw.cpp関数プロトタイプ宣言

// 設定関係関数
extern int SetNotUse3DFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetNotUse3DFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetBasicBlendFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetBasicBlendFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetScreenMemToVramFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetScreenMemToVramFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseNormalDrawShader( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseNormalDrawShader( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseSoftwareRenderModeFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseSoftwareRenderModeFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawFloatCoordType( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawFloatCoordType() ;
	DXFUNC_END
	return Result ;
}

// ＤｉｒｅｃｔＤｒａｗ関係情報提供関数
extern int GetDrawScreenSize( int *XBuf, int *YBuf )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawScreenSize( XBuf, YBuf ) ;
	DXFUNC_END
	return Result ;
}
extern int GetScreenBitDepth( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetScreenBitDepth(  ) ;
	DXFUNC_END
	return Result ;
}
extern int GetColorBitDepth( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetColorBitDepth(  ) ;
	DXFUNC_END
	return Result ;
}
extern int GetChangeDisplayFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetChangeDisplayFlag(  ) ;
	DXFUNC_END
	return Result ;
}
extern const COLORDATA *GetDispColorData( void )
{
	const COLORDATA *Result ;
	DXFUNC_BEGIN
	Result = NS_GetDispColorData( ) ;
	DXFUNC_END
	return Result ;
}
extern int GetScreenMemToSystemMemFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetScreenMemToSystemMemFlag(  ) ;
	DXFUNC_END
	return Result ;
}
extern int GetVideoMemorySize( int *AllSize, int *FreeSize )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetVideoMemorySize( AllSize, FreeSize ) ;
	DXFUNC_END
	return Result ;
}
extern int GetRefreshRate( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetRefreshRate() ;
	DXFUNC_END
	return Result ;
}
extern int GetDisplayNum( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDisplayNum() ;
	DXFUNC_END
	return Result ;
}
extern int GetDisplayInfo( int DisplayIndex, int *DesktopRectX, int *DesktopRectY, int *DesktopSizeX, int *DesktopSizeY, int *IsPrimary )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDisplayInfo( DisplayIndex, DesktopRectX, DesktopRectY, DesktopSizeX, DesktopSizeY, IsPrimary ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDisplayModeNum( int DisplayIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDisplayModeNum( DisplayIndex ) ;
	DXFUNC_END
	return Result ;
}
extern DISPLAYMODEDATA GetDisplayMode( int ModeIndex, int DisplayIndex )
{
	DISPLAYMODEDATA Result ;
	DXFUNC_BEGIN
	Result = NS_GetDisplayMode( ModeIndex, DisplayIndex ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDisplayMaxResolution( int *SizeX, int *SizeY, int DisplayIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDisplayMaxResolution( SizeX, SizeY, DisplayIndex ) ;
	DXFUNC_END
	return Result ;
}


// パレット操作関係関数
extern int GetGraphPalette( int GrHandle, int ColorIndex, int *Red, int *Green, int *Blue )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetGraphPalette(  GrHandle,  ColorIndex,  Red, Green, Blue ) ;
	DXFUNC_END
	return Result ;
}
extern int GetGraphOriginalPalette( int GrHandle, int ColorIndex, int *Red, int *Green, int *Blue )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetGraphOriginalPalette(  GrHandle,  ColorIndex,  Red,  Green,  Blue ) ;
	DXFUNC_END
	return Result ;
}
extern int SetGraphPalette( int GrHandle, int ColorIndex, unsigned int Color )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetGraphPalette(  GrHandle,  ColorIndex,  Color ) ;
	DXFUNC_END
	return Result ;
}
extern int ResetGraphPalette( int GrHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ResetGraphPalette( GrHandle ) ;
	DXFUNC_END
	return Result ;
}

// 簡易グラフィック関係関数
extern unsigned int GetPixel( int x, int y )
{
	unsigned int Result ;
	DXFUNC_BEGIN
	Result = NS_GetPixel( x, y ) ;
	DXFUNC_END
	return Result ;
}
extern COLOR_F GetPixelF( int x, int y )
{
	COLOR_F Result ;
	DXFUNC_BEGIN
	Result = NS_GetPixelF(  x,  y ) ;
	DXFUNC_END
	return Result ;
}
extern int Paint( int x, int y, unsigned int FillColor, ULONGLONG BoundaryColor )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_Paint( x, y, FillColor, BoundaryColor ) ;
	DXFUNC_END
	return Result ;
}

// ウエイト関係関数
extern int WaitVSync( int SyncNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_WaitVSync( SyncNum ) ;
	DXFUNC_END
	return Result ;
}

// 画面操作関係関数
extern int ScreenFlip( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ScreenFlip(  ) ;
	DXFUNC_END
	return Result ;
}
extern int ScreenCopy( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ScreenCopy() ;
	DXFUNC_END
	return Result ;
}
extern int SetGraphMode( int ScreenSizeX, int ScreenSizeY, int ColorBitDepth, int RefreshRate  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetGraphMode( ScreenSizeX, ScreenSizeY, ColorBitDepth, RefreshRate  ) ;
	DXFUNC_END
	return Result ;
}
extern int SetFullScreenResolutionMode( int ResolutionMode )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetFullScreenResolutionMode( ResolutionMode ) ;
	DXFUNC_END
	return Result ;
}
extern int GetFullScreenResolutionMode( int *ResolutionMode, int *UseResolutionMode )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetFullScreenResolutionMode( ResolutionMode, UseResolutionMode ) ;
	DXFUNC_END
	return Result ;
}
extern int SetFullScreenScalingMode( int ScalingMode , int FitScaling )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetFullScreenScalingMode( ScalingMode , FitScaling ) ;
	DXFUNC_END
	return Result ;
}
extern int SetZBufferSize( int ZBufferSizeX, int ZBufferSizeY )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetZBufferSize(  ZBufferSizeX,  ZBufferSizeY ) ;
	DXFUNC_END
	return Result ;
}
extern int SetZBufferBitDepth( int BitDepth )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetZBufferBitDepth( BitDepth ) ;
	DXFUNC_END
	return Result ;
}
extern int SetFullSceneAntiAliasingMode( int Samples, int Quality )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetFullSceneAntiAliasingMode(  Samples,  Quality ) ;
	DXFUNC_END
	return Result ;
}
extern int SetGraphDisplayArea( int x1, int y1, int x2, int y2 )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetGraphDisplayArea(  x1,  y1,  x2,  y2 ) ;
	DXFUNC_END
	return Result ;
}
extern int SetChangeScreenModeGraphicsSystemResetFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetChangeScreenModeGraphicsSystemResetFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}

#ifndef DX_NON_SAVEFUNCTION

// ＢＭＰ保存関数
extern int SaveDrawScreen( int x1, int y1, int x2, int y2, const TCHAR *FileName, int SaveType, int Jpeg_Quality, int Jpeg_Sample2x1, int Png_CompressionLevel )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveDrawScreen( x1, y1, x2, y2, FileName, SaveType, Jpeg_Quality, Jpeg_Sample2x1, Png_CompressionLevel ) ;
	DXFUNC_END
	return Result ;
}
extern int SaveDrawScreenWithStrLen( int x1, int y1, int x2, int y2, const TCHAR *FileName, size_t FileNameLength, int SaveType, int Jpeg_Quality, int Jpeg_Sample2x1, int Png_CompressionLevel )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveDrawScreenWithStrLen( x1, y1, x2, y2, FileName, FileNameLength, SaveType, Jpeg_Quality, Jpeg_Sample2x1, Png_CompressionLevel ) ;
	DXFUNC_END
	return Result ;
}

extern int SaveDrawScreenToBMP( int x1, int y1, int x2, int y2, const TCHAR *FileName )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveDrawScreenToBMP(  x1,  y1,  x2,  y2, FileName ) ;
	DXFUNC_END
	return Result ;
}
extern int SaveDrawScreenToBMPWithStrLen( int x1, int y1, int x2, int y2, const TCHAR *FileName, size_t FileNameLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveDrawScreenToBMPWithStrLen( x1, y1, x2, y2, FileName, FileNameLength ) ;
	DXFUNC_END
	return Result ;
}

extern int SaveDrawScreenToDDS( int x1, int y1, int x2, int y2, const TCHAR *FileName )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveDrawScreenToDDS(  x1,  y1,  x2,  y2, FileName ) ;
	DXFUNC_END
	return Result ;
}
extern int SaveDrawScreenToDDSWithStrLen( int x1, int y1, int x2, int y2, const TCHAR *FileName, size_t FileNameLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveDrawScreenToDDSWithStrLen( x1, y1, x2, y2, FileName, FileNameLength ) ;
	DXFUNC_END
	return Result ;
}

extern int SaveDrawScreenToJPEG( int x1, int y1, int x2, int y2, const TCHAR *FileName, int Quality, int Sample2x1  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveDrawScreenToJPEG(  x1,  y1,  x2,  y2, FileName,  Quality,  Sample2x1  ) ;
	DXFUNC_END
	return Result ;
}
extern int SaveDrawScreenToJPEGWithStrLen( int x1, int y1, int x2, int y2, const TCHAR *FileName, size_t FileNameLength, int Quality, int Sample2x1 )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveDrawScreenToJPEGWithStrLen( x1, y1, x2, y2, FileName, FileNameLength, Quality, Sample2x1 ) ;
	DXFUNC_END
	return Result ;
}

extern int SaveDrawScreenToPNG( int x1, int y1, int x2, int y2, const TCHAR *FileName, int CompressionLevel )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveDrawScreenToPNG(  x1,  y1,  x2,  y2, FileName, CompressionLevel ) ;
	DXFUNC_END
	return Result ;
}
extern int SaveDrawScreenToPNGWithStrLen( int x1, int y1, int x2, int y2, const TCHAR *FileName, size_t FileNameLength, int CompressionLevel )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveDrawScreenToPNGWithStrLen( x1, y1, x2, y2, FileName, FileNameLength, CompressionLevel ) ;
	DXFUNC_END
	return Result ;
}



extern int SaveDrawValidGraph( int GrHandle, int x1, int y1, int x2, int y2, const TCHAR *FileName, int SaveType, int Jpeg_Quality, int Jpeg_Sample2x1, int Png_CompressionLevel )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveDrawValidGraph( GrHandle, x1, y1, x2, y2, FileName, SaveType, Jpeg_Quality, Jpeg_Sample2x1, Png_CompressionLevel ) ;
	DXFUNC_END
	return Result ;
}
extern int SaveDrawValidGraphWithStrLen( int GrHandle, int x1, int y1, int x2, int y2, const TCHAR *FileName, size_t FileNameLength, int SaveType, int Jpeg_Quality, int Jpeg_Sample2x1, int Png_CompressionLevel )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveDrawValidGraphWithStrLen( GrHandle, x1, y1, x2, y2, FileName, FileNameLength, SaveType, Jpeg_Quality, Jpeg_Sample2x1, Png_CompressionLevel ) ;
	DXFUNC_END
	return Result ;
}
extern int SaveDrawValidGraphToBMP( int GrHandle, int x1, int y1, int x2, int y2, const TCHAR *FileName )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveDrawValidGraphToBMP( GrHandle, x1, y1, x2, y2, FileName ) ;
	DXFUNC_END
	return Result ;
}
extern int SaveDrawValidGraphToBMPWithStrLen( int GrHandle, int x1, int y1, int x2, int y2, const TCHAR *FileName, size_t FileNameLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveDrawValidGraphToBMPWithStrLen( GrHandle, x1, y1, x2, y2, FileName, FileNameLength ) ;
	DXFUNC_END
	return Result ;
}
extern int SaveDrawValidGraphToDDS( int GrHandle, int x1, int y1, int x2, int y2, const TCHAR *FileName )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveDrawValidGraphToDDS( GrHandle, x1, y1, x2, y2, FileName ) ;
	DXFUNC_END
	return Result ;
}
extern int SaveDrawValidGraphToDDSWithStrLen( int GrHandle, int x1, int y1, int x2, int y2, const TCHAR *FileName, size_t FileNameLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveDrawValidGraphToDDSWithStrLen( GrHandle, x1, y1, x2, y2, FileName, FileNameLength ) ;
	DXFUNC_END
	return Result ;
}
extern int SaveDrawValidGraphToJPEG( int GrHandle, int x1, int y1, int x2, int y2, const TCHAR *FileName, int Quality, int Sample2x1 )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveDrawValidGraphToJPEG( GrHandle, x1, y1, x2, y2, FileName, Quality, Sample2x1 ) ;
	DXFUNC_END
	return Result ;
}
extern int SaveDrawValidGraphToJPEGWithStrLen( int GrHandle, int x1, int y1, int x2, int y2, const TCHAR *FileName, size_t FileNameLength, int Quality, int Sample2x1 )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveDrawValidGraphToJPEGWithStrLen( GrHandle, x1, y1, x2, y2, FileName, FileNameLength, Quality, Sample2x1 ) ;
	DXFUNC_END
	return Result ;
}
extern int SaveDrawValidGraphToPNG( int GrHandle, int x1, int y1, int x2, int y2, const TCHAR *FileName, int CompressionLevel )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveDrawValidGraphToPNG( GrHandle, x1, y1, x2, y2, FileName, CompressionLevel ) ;
	DXFUNC_END
	return Result ;
}
extern int SaveDrawValidGraphToPNGWithStrLen( int GrHandle, int x1, int y1, int x2, int y2, const TCHAR *FileName, size_t FileNameLength, int CompressionLevel )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveDrawValidGraphToPNGWithStrLen( GrHandle, x1, y1, x2, y2, FileName, FileNameLength, CompressionLevel ) ;
	DXFUNC_END
	return Result ;
}

#endif // DX_NON_SAVEFUNCTION













// Dx3D.cpp関数プロトタイプ宣言


// 特殊用途関係
extern int RenderVertex( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_RenderVertex( ) ;
	DXFUNC_END
	return Result ;
}

extern int GetDrawCallCount( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawCallCount( ) ;
	DXFUNC_END
	return Result ;
}

extern float GetFPS( void )
{
	float Result ;
	DXFUNC_BEGIN
	Result = NS_GetFPS( ) ;
	DXFUNC_END
	return Result ;
}

// 設定関係
extern int SetUseDivGraphFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseDivGraphFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseAlphaImageLoadFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseAlphaImageLoadFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseMaxTextureSize( int Size )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseMaxTextureSize( Size ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseOldDrawModiGraphCodeFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseOldDrawModiGraphCodeFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}

// その他補助関数
extern const COLORDATA * GetTexColorData( int AlphaCh, int AlphaTest, int ColorBitDepth, int DrawValid  )
{
	const COLORDATA * Result ;
	DXFUNC_BEGIN
	Result = NS_GetTexColorData(  AlphaCh,  AlphaTest,  ColorBitDepth,  DrawValid  ) ;
	DXFUNC_END
	return Result ;
}
extern const COLORDATA * GetTexColorData( const IMAGEFORMATDESC *Format )
{
	const COLORDATA * Result ;
	DXFUNC_BEGIN
	Result = NS_GetTexColorData( Format ) ;
	DXFUNC_END
	return Result ;
}
extern const COLORDATA * GetTexColorData( int FormatIndex )
{
	const COLORDATA * Result ;
	DXFUNC_BEGIN
	Result = NS_GetTexColorData(  FormatIndex ) ;
	DXFUNC_END
	return Result ;
}








// DxGraphics関数プロトタイプ宣言

// グラフィック制御関係関数
extern int MakeGraph( int SizeX, int SizeY, int NotUse3DFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MakeGraph( SizeX, SizeY, NotUse3DFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int MakeScreen( int SizeX, int SizeY, int UseAlphaChannel )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MakeScreen( SizeX, SizeY, UseAlphaChannel ) ;
	DXFUNC_END
	return Result ;
}
extern int DeleteGraph( int GrHandle, int LogOutFlag  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DeleteGraph( GrHandle, LogOutFlag  ) ;
	DXFUNC_END
	return Result ;
}
extern int DeleteSharingGraph( int GrHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DeleteSharingGraph( GrHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetGraphNum( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetGraphNum( ) ;
	DXFUNC_END
	return Result ;
}
extern int SetGraphLostFlag( int GrHandle, int *LostFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetGraphLostFlag( GrHandle, LostFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int InitGraph( int LogOutFlag  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_InitGraph( LogOutFlag  ) ;
	DXFUNC_END
	return Result ;
}

extern int BltBmpToGraph( const COLORDATA *BmpColorData, HBITMAP RgbBmp, HBITMAP AlphaBmp, int CopyPointX, int CopyPointY, int GrHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_BltBmpToGraph( BmpColorData, RgbBmp, AlphaBmp, CopyPointX, CopyPointY, GrHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int BltBmpToDivGraph( const COLORDATA *BmpColorData, HBITMAP RgbBmp, HBITMAP AlphaBmp,
										int AllNum, int XNum, int YNum, int Width, int Height, const int *GrHandle, int ReverseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_BltBmpToDivGraph( BmpColorData,  RgbBmp,  AlphaBmp,
										 AllNum,  XNum,  YNum,  Width,  Height, GrHandle, ReverseFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int BltBmpOrGraphImageToGraph( const COLORDATA *BmpColorData, HBITMAP RgbBmp, HBITMAP AlphaBmp,
										int BmpFlag, const BASEIMAGE *RgbBaseImage, const BASEIMAGE *AlphaBaseImage,
										int CopyPointX, int CopyPointY, int GrHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_BltBmpOrGraphImageToGraph( BmpColorData,  RgbBmp,  AlphaBmp,
										 BmpFlag, RgbBaseImage, AlphaBaseImage,
										 CopyPointX, CopyPointY, GrHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int BltBmpOrGraphImageToGraph2( const COLORDATA *BmpColorData, HBITMAP RgbBmp, HBITMAP AlphaBmp,
										int BmpFlag, const BASEIMAGE *RgbBaseImage, const BASEIMAGE *AlphaBaseImage,
										const RECT *SrcRect, int DestX, int DestY, int GrHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_BltBmpOrGraphImageToGraph2( BmpColorData,  RgbBmp,  AlphaBmp,
										 BmpFlag, RgbBaseImage, AlphaBaseImage,
										SrcRect,  DestX,  DestY,  GrHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int BltBmpOrGraphImageToDivGraph( const COLORDATA *BmpColorData, HBITMAP RgbBmp, HBITMAP AlphaBmp,
										int BmpFlag, const BASEIMAGE *RgbBaseImage, const BASEIMAGE *AlphaBaseImage,
										int AllNum, int XNum, int YNum, int Width, int Height, const int *GrHandle, int ReverseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_BltBmpOrGraphImageToDivGraph( BmpColorData,  RgbBmp,  AlphaBmp,
										 BmpFlag, RgbBaseImage, AlphaBaseImage,
										 AllNum,  XNum,  YNum,  Width,  Height, GrHandle, ReverseFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int BltBmpOrGraphImageToDivGraphF( const COLORDATA *BmpColorData, HBITMAP RgbBmp, HBITMAP AlphaBmp,
										int BmpFlag, const BASEIMAGE *RgbBaseImage, const BASEIMAGE *AlphaBaseImage,
										int AllNum, int XNum, int YNum, float Width, float Height, const int *GrHandle, int ReverseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_BltBmpOrGraphImageToDivGraphF( BmpColorData,  RgbBmp,  AlphaBmp,
										 BmpFlag, RgbBaseImage, AlphaBaseImage,
										 AllNum,  XNum,  YNum,  Width,  Height, GrHandle, ReverseFlag ) ;
	DXFUNC_END
	return Result ;
}


extern int ReloadFileGraphAll( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReloadFileGraphAll() ;
	DXFUNC_END
	return Result ;
}








// シャドウマップハンドル関係関数
extern int MakeShadowMap( int SizeX, int SizeY )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MakeShadowMap( SizeX, SizeY ) ;
	DXFUNC_END
	return Result ;
}
extern int DeleteShadowMap( int SmHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DeleteShadowMap( SmHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetShadowMapLightDirection( int SmHandle, VECTOR Direction )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetShadowMapLightDirection( SmHandle, Direction ) ;
	DXFUNC_END
	return Result ;
}
extern int ShadowMap_DrawSetup( int SmHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ShadowMap_DrawSetup( SmHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int ShadowMap_DrawEnd( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ShadowMap_DrawEnd() ;
	DXFUNC_END
	return Result ;
}
extern int SetUseShadowMap( int SlotIndex, int SmHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseShadowMap(  SlotIndex,  SmHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetShadowMapDrawArea( int SmHandle, VECTOR MinPosition, VECTOR MaxPosition )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetShadowMapDrawArea( SmHandle, MinPosition, MaxPosition ) ;
	DXFUNC_END
	return Result ;
}
extern int ResetShadowMapDrawArea( int SmHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ResetShadowMapDrawArea( SmHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetShadowMapAdjustDepth( int SmHandle, float Depth )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetShadowMapAdjustDepth(  SmHandle, Depth ) ;
	DXFUNC_END
	return Result ;
}
extern int GetShadowMapViewProjectionMatrix( int SmHandle, MATRIX *MatrixBuffer )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetShadowMapViewProjectionMatrix( SmHandle, MatrixBuffer ) ;
	DXFUNC_END
	return Result ;
}
extern int SetShadowMapBlurParam( int SmHandle, int Param )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetShadowMapBlurParam( SmHandle, Param ) ;
	DXFUNC_END
	return Result ;
}
extern int SetShadowMapGradationParam( int SmHandle, float Param )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetShadowMapGradationParam( SmHandle, Param ) ;
	DXFUNC_END
	return Result ;
}
extern int TestDrawShadowMap( int SmHandle, int x1, int y1, int x2, int y2 )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_TestDrawShadowMap(  SmHandle,  x1,  y1,  x2,  y2 ) ;
	DXFUNC_END
	return Result ;
}










// グラフィック描画関係関数
extern int ClearDrawScreen( const RECT *ClearRect )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ClearDrawScreen( ClearRect ) ;
	DXFUNC_END
	return Result ;
}

extern int ClearDrawScreenZBuffer( const RECT *ClearRect )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ClearDrawScreenZBuffer( ClearRect ) ;
	DXFUNC_END
	return Result ;
}

extern int ClsDrawScreen( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ClsDrawScreen( ) ;
	DXFUNC_END
	return Result ;
}

extern int LoadGraphScreen( int x, int y, const TCHAR *GraphName, int TransFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadGraphScreen( x,  y, GraphName, TransFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadGraphScreenWithStrLen( int x, int y, const TCHAR *GraphName, size_t GraphNameLength, int TransFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadGraphScreenWithStrLen( x, y, GraphName, GraphNameLength, TransFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawGraph( int x, int y, int GrHandle, int TransFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawGraph( x,  y,  GrHandle,  TransFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawGraphF( float xf, float yf, int GrHandle, int TransFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawGraphF( xf,  yf,  GrHandle,  TransFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawExtendGraph( int x1, int y1, int x2, int y2, int GrHandle, int TransFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawExtendGraph( x1,  y1,  x2,  y2,  GrHandle,  TransFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawExtendGraphF( float x1f, float y1f, float x2f, float y2f, int GrHandle, int TransFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawExtendGraphF( x1f,  y1f,  x2f,  y2f,  GrHandle,  TransFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRotaGraph( int x, int y, double ExRate, double Angle, int GrHandle, int TransFlag, int ReverseXFlag, int ReverseYFlag   )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRotaGraph( x,  y, ExRate, Angle,  GrHandle,  TransFlag,  ReverseXFlag, ReverseYFlag   ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRotaGraphF( float xf, float yf, double ExRate, double Angle, int GrHandle, int TransFlag, int ReverseXFlag, int ReverseYFlag   )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRotaGraphF( xf,  yf, ExRate, Angle,  GrHandle,  TransFlag,  ReverseXFlag, ReverseYFlag   ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRotaGraph2( int x, int y, int cx, int cy, double ExtRate, double Angle, int GrHandle, int TransFlag, int ReverseXFlag, int ReverseYFlag   )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRotaGraph2( x,  y, cx,  cy, ExtRate, Angle,  GrHandle,  TransFlag,  ReverseXFlag, ReverseYFlag   ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRotaGraph2F( float xf, float yf, float cxf, float cyf, double ExtRate, double Angle, int GrHandle, int TransFlag, int ReverseXFlag, int ReverseYFlag   )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRotaGraph2F( xf,  yf, cxf,  cyf, ExtRate, Angle,  GrHandle,  TransFlag,  ReverseXFlag, ReverseYFlag   ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRotaGraph3(  int   x,  int   y,  int   cx,  int   cy,  double ExtRateX, double ExtRateY, double Angle, int GrHandle, int TransFlag, int ReverseXFlag, int ReverseYFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRotaGraph3(     x,     y,     cx,     cy,   ExtRateX,  ExtRateY,  Angle,  GrHandle,  TransFlag,  ReverseXFlag, ReverseYFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRotaGraph3F( float xf, float yf, float cxf, float cyf, double ExtRateX, double ExtRateY, double Angle, int GrHandle, int TransFlag, int ReverseXFlag, int ReverseYFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRotaGraph3F(  xf,  yf,  cxf,  cyf,  ExtRateX,  ExtRateY,  Angle,  GrHandle,  TransFlag,  ReverseXFlag, ReverseYFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRotaGraphFast( int x, int y, float ExRate, float Angle, int GrHandle, int TransFlag, int ReverseXFlag, int ReverseYFlag   )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRotaGraphFast( x,  y, ExRate, Angle,  GrHandle,  TransFlag,  ReverseXFlag, ReverseYFlag   ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRotaGraphFastF( float xf, float yf, float ExRate, float Angle, int GrHandle, int TransFlag, int ReverseXFlag, int ReverseYFlag   )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRotaGraphFastF( xf,  yf, ExRate, Angle,  GrHandle,  TransFlag,  ReverseXFlag, ReverseYFlag   ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRotaGraphFast2( int x, int y, int cx, int cy, float ExtRate, float Angle, int GrHandle, int TransFlag, int ReverseXFlag, int ReverseYFlag   )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRotaGraphFast2( x,  y, cx,  cy, ExtRate, Angle,  GrHandle,  TransFlag,  ReverseXFlag, ReverseYFlag   ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRotaGraphFast2F( float xf, float yf, float cxf, float cyf, float ExtRate, float Angle, int GrHandle, int TransFlag, int ReverseXFlag, int ReverseYFlag   )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRotaGraphFast2F( xf,  yf, cxf,  cyf, ExtRate, Angle,  GrHandle,  TransFlag,  ReverseXFlag, ReverseYFlag   ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRotaGraphFast3(  int   x,  int   y,  int   cx,  int   cy,  float ExtRateX, float ExtRateY, float Angle, int GrHandle, int TransFlag, int ReverseXFlag, int ReverseYFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRotaGraphFast3(     x,     y,     cx,     cy,   ExtRateX,  ExtRateY,  Angle,  GrHandle,  TransFlag,  ReverseXFlag, ReverseYFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRotaGraphFast3F( float xf, float yf, float cxf, float cyf, float ExtRateX, float ExtRateY, float Angle, int GrHandle, int TransFlag, int ReverseXFlag, int ReverseYFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRotaGraphFast3F(  xf,  yf,  cxf,  cyf,  ExtRateX,  ExtRateY,  Angle,  GrHandle,  TransFlag,  ReverseXFlag, ReverseYFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawModiGraph( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int GrHandle, int TransFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawModiGraph( x1,  y1,  x2,  y2,  x3,  y3,  x4,  y4,  GrHandle,  TransFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawModiGraphF( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, int GrHandle, int TransFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawModiGraphF( x1, y1, x2, y2, x3, y3, x4, y4, GrHandle, TransFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawTurnGraph( int x, int y, int GrHandle, int TransFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawTurnGraph( x,  y,  GrHandle,  TransFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawReverseGraph( int x, int y, int GrHandle, int TransFlag, int ReverseXFlag, int ReverseYFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawReverseGraph( x,  y, GrHandle,  TransFlag,  ReverseXFlag, ReverseYFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawTurnGraphF( float xf, float yf, int GrHandle, int TransFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawTurnGraphF( xf,  yf,  GrHandle,  TransFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawReverseGraphF( float xf, float yf, int GrHandle, int TransFlag, int ReverseXFlag, int ReverseYFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawReverseGraphF( xf,  yf,  GrHandle,  TransFlag, ReverseXFlag, ReverseYFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawChipMap( int Sx, int Sy, int XNum, int YNum, const int *MapData, int ChipTypeNum, int MapDataPitch, const int *ChipGrHandle, int TransFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawChipMap( Sx,  Sy,  XNum,  YNum,  MapData,  ChipTypeNum,  MapDataPitch,  ChipGrHandle,  TransFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawChipMap( int MapWidth, int MapHeight, const int *MapData, int ChipTypeNum, const int *ChipGrHandle, int TransFlag,
										int MapDrawPointX, int MapDrawPointY, int MapDrawWidth, int MapDrawHeight, int ScreenX, int ScreenY )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawChipMap(  MapWidth,  MapHeight,  MapData,  ChipTypeNum, ChipGrHandle,  TransFlag,
										 MapDrawPointX,  MapDrawPointY,  MapDrawWidth,  MapDrawHeight,  ScreenX,  ScreenY ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawTile( int x1, int y1, int x2, int y2, int Tx, int Ty,
								double ExtRate, double Angle, int GrHandle, int TransFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawTile(  x1,  y1,  x2,  y2,  Tx,  Ty,
								 ExtRate,  Angle,  GrHandle,  TransFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRectGraph( int DestX, int DestY, int SrcX, int SrcY, int Width, int Height, int GraphHandle, int TransFlag, int ReverseXFlag, int ReverseYFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRectGraph( DestX,  DestY,  SrcX,  SrcY,  Width,  Height,  GraphHandle,  TransFlag,  ReverseXFlag, ReverseYFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRectExtendGraph( int DestX1, int DestY1, int DestX2, int DestY2, int SrcX, int SrcY, int SrcWidth, int SrcHeight, int GraphHandle, int TransFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRectExtendGraph( DestX1,  DestY1,  DestX2,  DestY2,  SrcX,  SrcY,  SrcWidth,  SrcHeight,  GraphHandle,  TransFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRectRotaGraph( int X, int Y, int SrcX, int SrcY, int Width, int Height, double ExtRate, double Angle, int GraphHandle, int TransFlag, int ReverseXFlag, int ReverseYFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRectRotaGraph( X,  Y,  SrcX,  SrcY,  Width,  Height, ExtRate, Angle,  GraphHandle,  TransFlag,  ReverseXFlag, ReverseYFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRectRotaGraph2( int x, int y, int SrcX, int SrcY, int Width, int Height, int cx, int cy, double ExtRate, double Angle, int GraphHandle, int TransFlag, int ReverseXFlag, int ReverseYFlag ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRectRotaGraph2(  x,  y,  SrcX,  SrcY,  Width,  Height,  cx,  cy,  ExtRate,  Angle,  GraphHandle,  TransFlag,  ReverseXFlag, ReverseYFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRectRotaGraph3(  int x,   int y,   int SrcX, int SrcY, int Width, int Height, int cx, int cy, double ExtRateX, double ExtRateY, double Angle, int GraphHandle, int TransFlag, int ReverseXFlag, int ReverseYFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRectRotaGraph3(   x,    y,    SrcX,  SrcY,  Width,  Height,  cx,  cy,  ExtRateX,  ExtRateY,  Angle,  GraphHandle,  TransFlag,  ReverseXFlag, ReverseYFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRectRotaGraphFast( int X, int Y, int SrcX, int SrcY, int Width, int Height, float ExtRate, float Angle, int GraphHandle, int TransFlag, int ReverseXFlag, int ReverseYFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRectRotaGraphFast( X,  Y,  SrcX,  SrcY,  Width,  Height, ExtRate, Angle,  GraphHandle,  TransFlag,  ReverseXFlag, ReverseYFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRectRotaGraphFast2( int x, int y, int SrcX, int SrcY, int Width, int Height, int cx, int cy, float ExtRate, float Angle, int GraphHandle, int TransFlag, int ReverseXFlag, int ReverseYFlag ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRectRotaGraphFast2(  x,  y,  SrcX,  SrcY,  Width,  Height,  cx,  cy,  ExtRate,  Angle,  GraphHandle,  TransFlag,  ReverseXFlag, ReverseYFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRectRotaGraphFast3(  int x,   int y,   int SrcX, int SrcY, int Width, int Height, int cx, int cy, float ExtRateX, float ExtRateY, float Angle, int GraphHandle, int TransFlag, int ReverseXFlag, int ReverseYFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRectRotaGraphFast3(   x,    y,    SrcX,  SrcY,  Width,  Height,  cx,  cy,  ExtRateX,  ExtRateY,  Angle,  GraphHandle,  TransFlag,  ReverseXFlag, ReverseYFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRectModiGraph( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int SrcX, int SrcY, int Width, int Height, int GraphHandle, int TransFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRectModiGraph(  x1,  y1,  x2,  y2,  x3,  y3,  x4,  y4,  SrcX,  SrcY,  Width,  Height,  GraphHandle,  TransFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRectGraphF( float DestX, float DestY, int SrcX, int SrcY, int Width, int Height, int GraphHandle, int TransFlag, int ReverseXFlag, int ReverseYFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRectGraphF(  DestX,  DestY,  SrcX,  SrcY,  Width,  Height,  GraphHandle,  TransFlag,  ReverseXFlag, ReverseYFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRectExtendGraphF( float DestX1, float DestY1, float DestX2, float DestY2, int SrcX, int SrcY, int SrcWidth, int SrcHeight, int GraphHandle, int TransFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRectExtendGraphF(  DestX1,  DestY1,  DestX2,  DestY2,  SrcX,  SrcY,  SrcWidth,  SrcHeight,  GraphHandle,  TransFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRectRotaGraphF( float X, float Y, int SrcX, int SrcY, int Width, int Height, double ExtRate, double Angle, int GraphHandle, int TransFlag, int ReverseXFlag, int ReverseYFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRectRotaGraphF(  X,  Y,  SrcX,  SrcY,  Width,  Height,  ExtRate,  Angle,  GraphHandle,  TransFlag,  ReverseXFlag, ReverseYFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRectRotaGraph2F( float x, float y, int SrcX, int SrcY, int Width, int Height, float cxf, float cyf, double ExtRate, double Angle, int GraphHandle, int TransFlag, int ReverseXFlag, int ReverseYFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRectRotaGraph2F(  x,  y,  SrcX,  SrcY,  Width,  Height,  cxf,  cyf,  ExtRate,  Angle,  GraphHandle,  TransFlag,  ReverseXFlag, ReverseYFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRectRotaGraph3F( float x, float y, int SrcX, int SrcY, int Width, int Height, float cxf, float cyf, double ExtRateX, double ExtRateY, double Angle, int GraphHandle, int TransFlag, int ReverseXFlag, int ReverseYFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRectRotaGraph3F(  x,  y,  SrcX,  SrcY,  Width,  Height,  cxf,  cyf,  ExtRateX,  ExtRateY,  Angle,  GraphHandle,  TransFlag,  ReverseXFlag, ReverseYFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRectRotaGraphFastF( float X, float Y, int SrcX, int SrcY, int Width, int Height, float ExtRate, float Angle, int GraphHandle, int TransFlag, int ReverseXFlag, int ReverseYFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRectRotaGraphFastF(  X,  Y,  SrcX,  SrcY,  Width,  Height,  ExtRate,  Angle,  GraphHandle,  TransFlag,  ReverseXFlag, ReverseYFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRectRotaGraphFast2F( float x, float y, int SrcX, int SrcY, int Width, int Height, float cxf, float cyf, float ExtRate, float Angle, int GraphHandle, int TransFlag, int ReverseXFlag, int ReverseYFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRectRotaGraphFast2F(  x,  y,  SrcX,  SrcY,  Width,  Height,  cxf,  cyf,  ExtRate,  Angle,  GraphHandle,  TransFlag,  ReverseXFlag, ReverseYFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRectRotaGraphFast3F( float x, float y, int SrcX, int SrcY, int Width, int Height, float cxf, float cyf, float ExtRateX, float ExtRateY, float Angle, int GraphHandle, int TransFlag, int ReverseXFlag, int ReverseYFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRectRotaGraphFast3F(  x,  y,  SrcX,  SrcY,  Width,  Height,  cxf,  cyf,  ExtRateX,  ExtRateY,  Angle,  GraphHandle,  TransFlag,  ReverseXFlag, ReverseYFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRectModiGraphF( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, int SrcX, int SrcY, int Width, int Height,         int GraphHandle, int TransFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRectModiGraphF(  x1,  y1,  x2,  y2,  x3,  y3,  x4,  y4,  SrcX,  SrcY,  Width,  Height,          GraphHandle,  TransFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawBlendGraph( int x, int y, int GrHandle, int TransFlag, int BlendGraph, int BorderParam, int BorderRange )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawBlendGraph( x, y, GrHandle, TransFlag,  BlendGraph,  BorderParam,  BorderRange ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawBlendGraphPos( int x, int y, int GrHandle, int TransFlag, int bx, int by, int BlendGraph, int BorderParam, int BorderRange )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawBlendGraphPos(  x,  y,  GrHandle,  TransFlag,  bx,  by,  BlendGraph,  BorderParam,  BorderRange ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawCircleGauge( int CenterX, int CenterY, double Percent, int GrHandle, double StartPercent )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawCircleGauge(  CenterX,  CenterY, Percent,  GrHandle, StartPercent ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawGraphToZBuffer( int X, int Y, int GrHandle, int WriteZMode )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawGraphToZBuffer(  X,  Y,  GrHandle,  WriteZMode );
	DXFUNC_END
	return Result ;
}
extern int DrawTurnGraphToZBuffer( int x, int y, int GrHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawTurnGraphToZBuffer(  x,  y,  GrHandle,  WriteZMode );
	DXFUNC_END
	return Result ;
}
extern int DrawReverseGraphToZBuffer( int x, int y, int GrHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */, int ReverseXFlag, int ReverseYFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawReverseGraphToZBuffer(  x,  y,  GrHandle,  WriteZMode, ReverseXFlag, ReverseYFlag );
	DXFUNC_END
	return Result ;
}
extern int DrawExtendGraphToZBuffer( int x1, int y1, int x2, int y2, int GrHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawExtendGraphToZBuffer(  x1,  y1,  x2,  y2,  GrHandle,  WriteZMode );
	DXFUNC_END
	return Result ;
}
extern int DrawRotaGraphToZBuffer( int x, int y, double ExRate, double Angle, int GrHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */, int ReverseXFlag, int ReverseYFlag  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRotaGraphToZBuffer( x,  y,  ExRate,  Angle,  GrHandle,  WriteZMode, ReverseXFlag, ReverseYFlag  );
	DXFUNC_END
	return Result ;
}
extern int DrawRotaGraph2ToZBuffer(  int x, int y, int cx, int cy, double ExtRate,                   double Angle, int GrHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int ReverseXFlag, int ReverseYFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRotaGraph2ToZBuffer(   x,  y,  cx,  cy,  ExtRate,                    Angle,  GrHandle,  WriteZMode /* DX_ZWRITE_MASK 等 */ ,  ReverseXFlag, ReverseYFlag );
	DXFUNC_END
	return Result ;
}
extern int DrawRotaGraph3ToZBuffer(  int x, int y, int cx, int cy, double ExtRateX, double ExtRateY, double Angle, int GrHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int ReverseXFlag, int ReverseYFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRotaGraph3ToZBuffer(   x,  y,  cx,  cy,  ExtRateX,  ExtRateY,  Angle,  GrHandle,  WriteZMode /* DX_ZWRITE_MASK 等 */ ,  ReverseXFlag, ReverseYFlag );
	DXFUNC_END
	return Result ;
}
extern int DrawRotaGraphFastToZBuffer( int x, int y, float ExRate, float Angle, int GrHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */, int ReverseXFlag, int ReverseYFlag  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRotaGraphFastToZBuffer( x,  y,  ExRate,  Angle,  GrHandle,  WriteZMode, ReverseXFlag, ReverseYFlag  );
	DXFUNC_END
	return Result ;
}
extern int DrawRotaGraphFast2ToZBuffer(  int x, int y, int cx, int cy, float ExtRate,                   float Angle, int GrHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int ReverseXFlag, int ReverseYFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRotaGraphFast2ToZBuffer(   x,  y,  cx,  cy,  ExtRate,                    Angle,  GrHandle,  WriteZMode /* DX_ZWRITE_MASK 等 */ ,  ReverseXFlag, ReverseYFlag );
	DXFUNC_END
	return Result ;
}
extern int DrawRotaGraphFast3ToZBuffer(  int x, int y, int cx, int cy, float ExtRateX, float ExtRateY, float Angle, int GrHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int ReverseXFlag, int ReverseYFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRotaGraphFast3ToZBuffer(   x,  y,  cx,  cy,  ExtRateX,  ExtRateY,  Angle,  GrHandle,  WriteZMode /* DX_ZWRITE_MASK 等 */ ,  ReverseXFlag, ReverseYFlag );
	DXFUNC_END
	return Result ;
}
extern int DrawModiGraphToZBuffer( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int GrHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawModiGraphToZBuffer(  x1,  y1,  x2,  y2,  x3,  y3,  x4,  y4,  GrHandle,  WriteZMode );
	DXFUNC_END
	return Result ;
}
extern int DrawBoxToZBuffer( int x1, int y1, int x2, int y2, int FillFlag, int WriteZMode /* DX_ZWRITE_MASK 等 */ )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawBoxToZBuffer(  x1,  y1,  x2,  y2,  FillFlag,  WriteZMode );
	DXFUNC_END
	return Result ;
}
extern int DrawCircleToZBuffer( int x, int y, int r, int FillFlag, int WriteZMode /* DX_ZWRITE_MASK 等 */ )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawCircleToZBuffer(  x,  y,  r,  FillFlag,  WriteZMode );
	DXFUNC_END
	return Result ;
}

extern int DrawTriangleToZBuffer(    int x1, int y1, int x2, int y2, int x3, int y3, int FillFlag, int WriteZMode /* DX_ZWRITE_MASK 等 */ )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawTriangleToZBuffer(  x1,  y1,  x2,  y2,  x3,  y3,  FillFlag,  WriteZMode ) ;
	DXFUNC_END
	return Result ;
}

extern int DrawQuadrangleToZBuffer(  int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int FillFlag, int WriteZMode /* DX_ZWRITE_MASK 等 */ )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawQuadrangleToZBuffer(   x1,  y1,  x2,  y2,  x3,  y3,  x4,  y4,  FillFlag, WriteZMode ) ;
	DXFUNC_END
	return Result ;
}

extern int DrawRoundRectToZBuffer(   int x1, int y1, int x2, int y2, int rx, int ry, int FillFlag, int WriteZMode /* DX_ZWRITE_MASK 等 */ )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRoundRectToZBuffer(  x1,  y1,  x2,  y2,  rx,  ry,  FillFlag,  WriteZMode ) ;
	DXFUNC_END
	return Result ;
}




extern int DrawPolygon( const VERTEX *Vertex, int PolygonNum, int GrHandle, int TransFlag, int UVScaling )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawPolygon( Vertex,  PolygonNum,  GrHandle,  TransFlag,  UVScaling ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawPolygon2D( const VERTEX2D  *Vertex, int PolygonNum, int GrHandle, int TransFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawPolygon2D( Vertex, PolygonNum, GrHandle, TransFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawPolygon3D( const VERTEX3D *Vertex, int PolygonNum, int GrHandle, int TransFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawPolygon3D( Vertex, PolygonNum, GrHandle, TransFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawPolygonIndexed2D( const VERTEX2D *Vertex, int VertexNum, const unsigned short *Indices, int PolygonNum, int GrHandle, int TransFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawPolygonIndexed2D( Vertex, VertexNum, Indices, PolygonNum, GrHandle, TransFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawPolygonIndexed3D( const VERTEX3D *Vertex, int VertexNum, const unsigned short *Indices, int PolygonNum, int GrHandle, int TransFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawPolygonIndexed3D( Vertex, VertexNum, Indices,  PolygonNum,  GrHandle,  TransFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawPolygonIndexed3DBase( const VERTEX_3D *Vertex, int VertexNum, const unsigned short *Indices, int IndexNum, int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */, int GrHandle, int TransFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawPolygonIndexed3DBase( Vertex, VertexNum, Indices, IndexNum, PrimitiveType,  GrHandle,  TransFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawPolygon3DBase( const VERTEX_3D *Vertex, int VertexNum, int PrimitiveType, int GrHandle, int TransFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawPolygon3DBase( Vertex, VertexNum, PrimitiveType, GrHandle, TransFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawPolygon3D( const VERTEX_3D *Vertex, int PolygonNum, int GrHandle, int TransFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawPolygon3D( Vertex, PolygonNum, GrHandle, TransFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawPolygonBase( const VERTEX *Vertex, int VertexNum, int PrimitiveType, int GrHandle, int TransFlag, int UVScaling )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawPolygonBase( Vertex,  VertexNum,  PrimitiveType,  GrHandle,  TransFlag, UVScaling ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawPrimitive2D( const VERTEX2D *Vertex, int VertexNum, int PrimitiveType, int GrHandle, int TransFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawPrimitive2D( Vertex,  VertexNum,  PrimitiveType,  GrHandle,  TransFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawPrimitive3D( const VERTEX3D *Vertex, int VertexNum, int PrimitiveType, int GrHandle, int TransFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawPrimitive3D( Vertex,  VertexNum,  PrimitiveType,  GrHandle,  TransFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawPrimitiveIndexed2D( const VERTEX2D *Vertex, int VertexNum, const unsigned short *Indices, int IndexNum, int PrimitiveType, int GrHandle, int TransFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawPrimitiveIndexed2D( Vertex,  VertexNum, Indices,  IndexNum,  PrimitiveType,  GrHandle,  TransFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawPrimitiveIndexed3D( const VERTEX3D *Vertex, int VertexNum, const unsigned short *Indices, int IndexNum, int PrimitiveType, int GrHandle, int TransFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawPrimitiveIndexed3D( Vertex,  VertexNum, Indices,  IndexNum,  PrimitiveType,  GrHandle,  TransFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawPolygon3D_UseVertexBuffer( int VertexBufHandle, int GrHandle, int TransFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawPolygon3D_UseVertexBuffer(  VertexBufHandle,  GrHandle,  TransFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawPrimitive3D_UseVertexBuffer( int VertexBufHandle, int PrimitiveType  /* DX_PRIMTYPE_TRIANGLELIST 等 */, int GrHandle, int TransFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawPrimitive3D_UseVertexBuffer(  VertexBufHandle,  PrimitiveType ,  GrHandle,  TransFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawPrimitive3D_UseVertexBuffer2( int VertexBufHandle, int PrimitiveType  /* DX_PRIMTYPE_TRIANGLELIST 等 */, int StartVertex, int UseVertexNum, int GrHandle, int TransFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawPrimitive3D_UseVertexBuffer2(  VertexBufHandle,  PrimitiveType  ,  StartVertex,  UseVertexNum,  GrHandle,  TransFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawPolygonIndexed3D_UseVertexBuffer( int VertexBufHandle, int IndexBufHandle, int GrHandle, int TransFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawPolygonIndexed3D_UseVertexBuffer(  VertexBufHandle,  IndexBufHandle,  GrHandle,  TransFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawPrimitiveIndexed3D_UseVertexBuffer( int VertexBufHandle, int IndexBufHandle, int PrimitiveType  /* DX_PRIMTYPE_TRIANGLELIST 等 */, int GrHandle, int TransFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawPrimitiveIndexed3D_UseVertexBuffer(  VertexBufHandle,  IndexBufHandle,  PrimitiveType ,  GrHandle,  TransFlag );
	DXFUNC_END
	return Result ;
}
extern int DrawPrimitiveIndexed3D_UseVertexBuffer2( int VertexBufHandle, int IndexBufHandle, int PrimitiveType  /* DX_PRIMTYPE_TRIANGLELIST 等 */, int BaseVertex, int StartVertex, int UseVertexNum, int StartIndex, int UseIndexNum, int GrHandle, int TransFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawPrimitiveIndexed3D_UseVertexBuffer2(  VertexBufHandle,  IndexBufHandle,  PrimitiveType ,  BaseVertex, StartVertex,  UseVertexNum,  StartIndex,  UseIndexNum,  GrHandle,  TransFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawGraph3D( float x, float y, float z, int GrHandle, int TransFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawGraph3D(  x,  y,  z,  GrHandle,  TransFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawExtendGraph3D( float x, float y, float z, double ExRateX, double ExRateY, int GrHandle, int TransFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawExtendGraph3D(  x,  y,  z,  ExRateX,  ExRateY,  GrHandle,  TransFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRotaGraph3D( float x, float y, float z, double ExRate, double Angle, int GrHandle, int TransFlag, int ReverseXFlag, int ReverseYFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRotaGraph3D(  x,  y,  z,  ExRate,  Angle,  GrHandle,  TransFlag,  ReverseXFlag, ReverseYFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRota2Graph3D( float x, float y, float z, float cx, float cy, double ExtRateX, double ExtRateY, double Angle, int GrHandle, int TransFlag, int ReverseXFlag, int ReverseYFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRota2Graph3D(  x,  y,  z,  cx,  cy,  ExtRateX,  ExtRateY,  Angle,  GrHandle,  TransFlag,  ReverseXFlag, ReverseYFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawModiBillboard3D( VECTOR Pos, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, int GrHandle, int TransFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawModiBillboard3D(  Pos,  x1,  y1,  x2,  y2,  x3,  y3,  x4,  y4,  GrHandle,  TransFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawBillboard3D( VECTOR Pos, float cx, float cy, float Size, float Angle, int GrHandle, int TransFlag, int ReverseXFlag, int ReverseYFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawBillboard3D( Pos, cx, cy, Size, Angle,  GrHandle,  TransFlag,  ReverseXFlag, ReverseYFlag ) ;
	DXFUNC_END
	return Result ;
}

extern int FillGraph( int GrHandle, int Red, int Green, int Blue, int Alpha )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_FillGraph( GrHandle,  Red,  Green,  Blue,  Alpha ) ;
	DXFUNC_END
	return Result ;
}
extern int FillRectGraph( int GrHandle, int x, int y, int Width, int Height, int Red, int Green, int Blue, int Alpha )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_FillRectGraph( GrHandle, x, y, Width, Height, Red, Green, Blue, Alpha ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawLine( int x1, int y1, int x2, int y2, unsigned int Color, int Thickness )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawLine( x1,  y1,  x2,  y2,  Color, Thickness ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawLineAA( float x1, float y1, float x2, float y2, unsigned int Color, float Thickness )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawLineAA( x1,  y1,  x2,  y2,  Color, Thickness ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawBox( int x1, int y1, int x2, int y2, unsigned int Color, int FillFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawBox( x1,  y1,  x2,  y2,  Color,  FillFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawBoxAA( float x1, float y1, float x2, float y2, unsigned int Color, int FillFlag, float LineThickness )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawBoxAA( x1,  y1,  x2,  y2,  Color,  FillFlag, LineThickness ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawFillBox( int x1, int y1, int x2, int y2, unsigned int Color )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawFillBox( x1,  y1,  x2,  y2,  Color ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawLineBox( int x1, int y1, int x2, int y2, unsigned int Color )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawLineBox( x1,  y1,  x2,  y2,  Color ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawCircle( int x, int y, int r, unsigned int Color, int FillFlag, int LineThickness )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawCircle( x,  y,  r,  Color,  FillFlag, LineThickness ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawCircleAA( float x, float y, float r, int posnum, unsigned int Color, int FillFlag, float LineThickness )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawCircleAA( x,  y,  r, posnum, Color, FillFlag, LineThickness ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawOval( int x, int y, int rx, int ry, unsigned int Color, int FillFlag, int LineThickness )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawOval( x,  y,  rx,  ry,  Color,  FillFlag, LineThickness ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawOvalAA( float x,float y, float rx, float ry, int posnum, unsigned int Color, int FillFlag, float LineThickness )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawOvalAA( x,  y,  rx,  ry, posnum,  Color,  FillFlag, LineThickness ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawOval_Rect( int x1, int   y1, int   x2, int   y2, unsigned int Color, int FillFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawOval_Rect( x1,   y1,  x2,  y2, Color, FillFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawTriangle( int x1, int y1, int x2, int y2, int x3, int y3, unsigned int Color, int FillFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawTriangle( x1,  y1,  x2,  y2,  x3,  y3,  Color,  FillFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawTriangleAA( float x1, float y1, float x2, float y2, float x3, float y3, unsigned int Color, int FillFlag, float LineThickness )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawTriangleAA( x1,  y1,  x2,  y2,  x3,  y3,  Color,  FillFlag, LineThickness ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawQuadrangle( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int Color, int FillFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawQuadrangle(  x1,  y1,  x2,  y2,  x3,  y3,  x4,  y4,  Color,  FillFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawQuadrangleAA( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int Color, int FillFlag, float LineThickness )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawQuadrangleAA(  x1,  y1,  x2,  y2,  x3,  y3,  x4,  y4,  Color,  FillFlag, LineThickness ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRoundRect( int x1, int y1, int x2, int y2, int rx, int ry, unsigned int Color, int FillFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRoundRect(  x1,  y1,  x2,  y2,  rx,  ry,  Color,  FillFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRoundRectAA( float x1, float y1, float x2, float   y2, float rx, float ry, int posnum, unsigned int Color, int FillFlag, float LineThickness )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRoundRectAA(  x1,  y1,  x2,  y2,  rx,  ry, posnum,  Color,  FillFlag, LineThickness ) ;
	DXFUNC_END
	return Result ;
}
extern int BeginAADraw( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_BeginAADraw() ;
	DXFUNC_END
	return Result ;
}
extern int EndAADraw( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_EndAADraw() ;
	DXFUNC_END
	return Result ;
}
extern int DrawPixel( int x, int y, unsigned int Color )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawPixel( x,  y,  Color ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawPixelSet( const POINTDATA *PointData, int Num )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawPixelSet( PointData, Num ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawLineSet( const LINEDATA *LineData, int Num )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawLineSet( LineData, Num ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawPixel3D( VECTOR Pos, unsigned int Color )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawPixel3D( Pos, Color ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawPixel3DD( VECTOR_D Pos, unsigned int Color )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawPixel3DD( Pos, Color ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawLine3D( VECTOR Pos1, VECTOR Pos2, unsigned int Color )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawLine3D(  Pos1,  Pos2, Color ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawLine3DD( VECTOR_D Pos1, VECTOR_D Pos2, unsigned int Color )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawLine3DD(  Pos1,  Pos2, Color ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawTriangle3D( VECTOR Pos1, VECTOR Pos2, VECTOR Pos3, unsigned int Color, int FillFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawTriangle3D(  Pos1,  Pos2,  Pos3, Color, FillFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawTriangle3DD( VECTOR_D Pos1, VECTOR_D Pos2, VECTOR_D Pos3, unsigned int Color, int FillFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawTriangle3DD(  Pos1,  Pos2,  Pos3, Color, FillFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawCube3D( VECTOR Pos1, VECTOR Pos2, unsigned int DifColor, unsigned int SpcColor, int FillFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawCube3D(  Pos1,  Pos2, DifColor, SpcColor, FillFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawCube3DD( VECTOR_D Pos1, VECTOR_D Pos2, unsigned int DifColor, unsigned int SpcColor, int FillFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawCube3DD(  Pos1,  Pos2, DifColor, SpcColor, FillFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawSphere3D( VECTOR CenterPos, float r, int DivNum, unsigned int DifColor, unsigned int SpcColor, int FillFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawSphere3D(  CenterPos,  r,  DivNum, DifColor, SpcColor, FillFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawSphere3DD( VECTOR_D CenterPos, double r, int DivNum, unsigned int DifColor, unsigned int SpcColor, int FillFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawSphere3DD(  CenterPos,  r,  DivNum, DifColor, SpcColor, FillFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawCapsule3D( VECTOR Pos1, VECTOR Pos2, float r, int DivNum, unsigned int DifColor, unsigned int SpcColor, int FillFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawCapsule3D(  Pos1,  Pos2,  r,  DivNum, DifColor, SpcColor, FillFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawCapsule3DD( VECTOR_D Pos1, VECTOR_D Pos2, float r, int DivNum, unsigned int DifColor, unsigned int SpcColor, int FillFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawCapsule3DD(  Pos1,  Pos2,  r,  DivNum, DifColor, SpcColor, FillFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawCone3D( VECTOR TopPos, VECTOR BottomPos, float r, int DivNum, unsigned int DifColor, unsigned int SpcColor, int FillFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawCone3D(  TopPos,  BottomPos,  r,  DivNum, DifColor, SpcColor, FillFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawCone3DD( VECTOR_D TopPos, VECTOR_D BottomPos, float r, int DivNum, unsigned int DifColor, unsigned int SpcColor, int FillFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawCone3DD(  TopPos,  BottomPos,  r,  DivNum, DifColor, SpcColor, FillFlag ) ;
	DXFUNC_END
	return Result ;
}

#ifndef DX_NON_FONT
extern int DrawString( int x, int y, const TCHAR *String, unsigned int Color, unsigned int EdgeColor )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawString( x,  y, String,  Color,  EdgeColor ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawNString( int x, int y, const TCHAR *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawNString( x,  y, String, StringLength,  Color,  EdgeColor ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawVString( int x, int y, const TCHAR *String, unsigned int Color, unsigned int EdgeColor )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawVString( x, y, String,  Color,  EdgeColor ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawNVString( int x, int y, const TCHAR *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawNVString( x, y, String, StringLength,  Color,  EdgeColor ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawFormatString( int x, int y, unsigned int Color, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	
	DXFUNC_BEGIN
	Result = NS_DrawString( x,  y, String,  Color ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawFormatVString( int x, int y, unsigned int Color, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	DXFUNC_BEGIN
	Result = NS_DrawVString( x,  y, String,  Color ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawExtendString( int x, int y, double ExRateX, double ExRateY, const TCHAR *String, unsigned int Color, unsigned int EdgeColor )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawExtendString( x,  y, ExRateX, ExRateY, String,  Color,  EdgeColor ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawExtendNString( int x, int y, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawExtendNString( x,  y, ExRateX, ExRateY, String, StringLength,  Color,  EdgeColor ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawExtendVString( int x, int y, double ExRateX, double ExRateY, const TCHAR *String, unsigned int Color, unsigned int EdgeColor )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawExtendVString( x, y, ExRateX, ExRateY, String,  Color,  EdgeColor ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawExtendNVString( int x, int y, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawExtendNVString( x, y, ExRateX, ExRateY, String, StringLength,  Color,  EdgeColor ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawExtendFormatString( int x, int y, double ExRateX, double ExRateY, unsigned int Color, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	
	DXFUNC_BEGIN
	Result = NS_DrawExtendString( x,  y, ExRateX, ExRateY, String,  Color ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawExtendFormatVString( int x, int y, double ExRateX, double ExRateY, unsigned int Color, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	DXFUNC_BEGIN
	Result = NS_DrawExtendVString( x,  y, ExRateX, ExRateY, String,  Color ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRotaString( int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, unsigned int EdgeColor, int VerticalFlag, const TCHAR *String )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRotaString(  x,  y,  ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle,  Color,  EdgeColor,  VerticalFlag, String ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRotaNString( int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, unsigned int EdgeColor, int VerticalFlag, const TCHAR *String, size_t StringLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRotaNString(  x,  y,  ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle,  Color,  EdgeColor,  VerticalFlag, String, StringLength ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRotaFormatString( int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, unsigned int EdgeColor, int VerticalFlag, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	
	DXFUNC_BEGIN
	Result = NS_DrawRotaString(  x,  y,  ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle,  Color,  EdgeColor,  VerticalFlag, String ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawModiString( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int Color, unsigned int EdgeColor, int VerticalFlag, const TCHAR *String )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawModiString( x1, y1, x2, y2, x3, y3, x4, y4, Color, EdgeColor, VerticalFlag, String ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawModiNString( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int Color, unsigned int EdgeColor, int VerticalFlag, const TCHAR *String, size_t StringLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawModiNString( x1, y1, x2, y2, x3, y3, x4, y4, Color, EdgeColor, VerticalFlag, String, StringLength ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawModiFormatString( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int Color, unsigned int EdgeColor, int VerticalFlag, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	
	DXFUNC_BEGIN
	Result = NS_DrawModiString( x1, y1, x2, y2, x3, y3, x4, y4,  Color,  EdgeColor,  VerticalFlag, String ) ;
	DXFUNC_END
	return Result ;
}



extern int DrawStringF( float x, float y, const TCHAR *String, unsigned int Color, unsigned int EdgeColor )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawStringF( x,  y, String,  Color,  EdgeColor ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawNStringF( float x, float y, const TCHAR *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawNStringF( x,  y, String, StringLength,  Color,  EdgeColor ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawVStringF( float x, float y, const TCHAR *String, unsigned int Color, unsigned int EdgeColor )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawVStringF( x, y, String,  Color,  EdgeColor ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawNVStringF( float x, float y, const TCHAR *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawNVStringF( x, y, String, StringLength,  Color,  EdgeColor ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawFormatStringF( float x, float y, unsigned int Color, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	
	DXFUNC_BEGIN
	Result = NS_DrawStringF( x,  y, String,  Color ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawFormatVStringF( float x, float y, unsigned int Color, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	DXFUNC_BEGIN
	Result = NS_DrawVStringF( x,  y, String,  Color ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawExtendStringF( float x, float y, double ExRateX, double ExRateY, const TCHAR *String, unsigned int Color, unsigned int EdgeColor )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawExtendStringF( x,  y, ExRateX, ExRateY, String,  Color,  EdgeColor ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawExtendNStringF( float x, float y, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawExtendNStringF( x,  y, ExRateX, ExRateY, String, StringLength,  Color,  EdgeColor ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawExtendVStringF( float x, float y, double ExRateX, double ExRateY, const TCHAR *String, unsigned int Color, unsigned int EdgeColor )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawExtendVStringF( x, y, ExRateX, ExRateY, String,  Color,  EdgeColor ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawExtendNVStringF( float x, float y, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawExtendNVStringF( x, y, ExRateX, ExRateY, String, StringLength,  Color,  EdgeColor ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawExtendFormatStringF( float x, float y, double ExRateX, double ExRateY, unsigned int Color, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	
	DXFUNC_BEGIN
	Result = NS_DrawExtendStringF( x,  y, ExRateX, ExRateY, String,  Color ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawExtendFormatVStringF( float x, float y, double ExRateX, double ExRateY, unsigned int Color, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	DXFUNC_BEGIN
	Result = NS_DrawExtendVStringF( x,  y, ExRateX, ExRateY, String,  Color ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRotaStringF(	float x, float y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, unsigned int EdgeColor , int VerticalFlag , const TCHAR *String )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRotaStringF(  x,  y,  ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle,  Color,  EdgeColor ,  VerticalFlag , String ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRotaNStringF(	float x, float y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, unsigned int EdgeColor , int VerticalFlag , const TCHAR *String, size_t StringLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRotaNStringF(  x,  y,  ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle,  Color,  EdgeColor ,  VerticalFlag , String, StringLength ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRotaFormatStringF( float x, float y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, unsigned int EdgeColor , int VerticalFlag , const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;

	DXFUNC_BEGIN
	Result = NS_DrawRotaStringF(  x,  y,  ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle,  Color,  EdgeColor ,  VerticalFlag , String ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawModiStringF( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int Color, unsigned int EdgeColor, int VerticalFlag, const TCHAR *String )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawModiStringF( x1, y1, x2, y2, x3, y3, x4, y4, Color, EdgeColor, VerticalFlag, String ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawModiNStringF( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int Color, unsigned int EdgeColor, int VerticalFlag, const TCHAR *String, size_t StringLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawModiNStringF( x1, y1, x2, y2, x3, y3, x4, y4, Color, EdgeColor, VerticalFlag, String, StringLength ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawModiFormatStringF( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int Color, unsigned int EdgeColor, int VerticalFlag, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;

	DXFUNC_BEGIN
	Result = NS_DrawModiStringF( x1, y1, x2, y2, x3, y3, x4, y4, Color,  EdgeColor ,  VerticalFlag , String ) ;
	DXFUNC_END
	return Result ;
}


extern int DrawNumberToI( int x, int y, int Num, int RisesNum, unsigned int Color ,unsigned int EdgeColor )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawNumberToI( x,  y,  Num,  RisesNum,  Color , EdgeColor ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawNumberToF( int x, int y, double Num, int Length, unsigned int Color ,unsigned int EdgeColor )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawNumberToF( x, y, Num,  Length,  Color , EdgeColor ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawNumberPlusToI( int x, int y, const TCHAR *NoteString, int Num, int RisesNum, unsigned int Color,unsigned int EdgeColor  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawNumberPlusToI( x, y, NoteString, Num, RisesNum, Color,EdgeColor  ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawNumberPlusToF( int x, int y, const TCHAR *NoteString, double Num, int Length, unsigned int Color,unsigned int EdgeColor  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawNumberPlusToF( x, y, NoteString, Num, Length, Color,EdgeColor  ) ;
	DXFUNC_END
	return Result ;
}


extern int DrawStringToZBuffer( int x, int y, const TCHAR *String, int WriteZMode /* DX_ZWRITE_MASK 等 */ )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawStringToZBuffer(  x,  y, String,   WriteZMode );
	DXFUNC_END
	return Result ;
}
extern int DrawNStringToZBuffer( int x, int y, const TCHAR *String, size_t StringLength, int WriteZMode /* DX_ZWRITE_MASK 等 */ )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawNStringToZBuffer(  x,  y, String, StringLength,   WriteZMode );
	DXFUNC_END
	return Result ;
}
extern int DrawVStringToZBuffer( int x, int y, const TCHAR *String, int WriteZMode /* DX_ZWRITE_MASK 等 */ )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawVStringToZBuffer(  x,  y, String,    WriteZMode );
	DXFUNC_END
	return Result ;
}
extern int DrawNVStringToZBuffer( int x, int y, const TCHAR *String, size_t StringLength, int WriteZMode /* DX_ZWRITE_MASK 等 */ )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawNVStringToZBuffer(  x,  y, String, StringLength,    WriteZMode );
	DXFUNC_END
	return Result ;
}
extern int DrawFormatStringToZBuffer( int x, int y, int WriteZMode /* DX_ZWRITE_MASK 等 */, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	
	DXFUNC_BEGIN
	Result = NS_DrawStringToZBuffer( x,  y, String,    WriteZMode ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawFormatVStringToZBuffer( int x, int y, int WriteZMode /* DX_ZWRITE_MASK 等 */, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	
	DXFUNC_BEGIN
	Result = NS_DrawVStringToZBuffer( x,  y, String,    WriteZMode ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawExtendStringToZBuffer( int x, int y, double ExRateX, double ExRateY, const TCHAR *String, int WriteZMode /* DX_ZWRITE_MASK 等 */ )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawExtendStringToZBuffer(  x,  y,  ExRateX,  ExRateY, String,    WriteZMode );
	DXFUNC_END
	return Result ;
}
extern int DrawExtendNStringToZBuffer( int x, int y, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, int WriteZMode /* DX_ZWRITE_MASK 等 */ )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawExtendNStringToZBuffer(  x,  y,  ExRateX,  ExRateY, String, StringLength,    WriteZMode );
	DXFUNC_END
	return Result ;
}
extern int DrawExtendVStringToZBuffer( int x, int y, double ExRateX, double ExRateY, const TCHAR *String, int WriteZMode /* DX_ZWRITE_MASK 等 */ )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawExtendVStringToZBuffer(  x,  y,  ExRateX,  ExRateY, String,    WriteZMode );
	DXFUNC_END
	return Result ;
}
extern int DrawExtendNVStringToZBuffer( int x, int y, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, int WriteZMode /* DX_ZWRITE_MASK 等 */ )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawExtendNVStringToZBuffer(  x,  y,  ExRateX,  ExRateY, String, StringLength,   WriteZMode );
	DXFUNC_END
	return Result ;
}
extern int DrawExtendFormatStringToZBuffer( int x, int y, double ExRateX, double ExRateY, int WriteZMode /* DX_ZWRITE_MASK 等 */, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	
	DXFUNC_BEGIN
	Result = NS_DrawExtendStringToZBuffer(  x,  y,  ExRateX,  ExRateY, String,    WriteZMode );
	DXFUNC_END
	return Result ;
}
extern int DrawExtendFormatVStringToZBuffer( int x, int y, double ExRateX, double ExRateY, int WriteZMode /* DX_ZWRITE_MASK 等 */, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	
	DXFUNC_BEGIN
	Result = NS_DrawExtendVStringToZBuffer(  x,  y,  ExRateX,  ExRateY, String,    WriteZMode );
	DXFUNC_END
	return Result ;
}
extern int DrawRotaStringToZBuffer( int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag , const TCHAR *String )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRotaStringToZBuffer(  x,  y,  ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle,  WriteZMode /* DX_ZWRITE_MASK 等 */ ,  VerticalFlag , String ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRotaNStringToZBuffer( int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag , const TCHAR *String, size_t StringLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRotaNStringToZBuffer(  x,  y,  ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle,  WriteZMode /* DX_ZWRITE_MASK 等 */ ,  VerticalFlag , String, StringLength ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRotaFormatStringToZBuffer( int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag , const TCHAR *FormatString , ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	
	DXFUNC_BEGIN
	Result = NS_DrawRotaStringToZBuffer(  x,  y,  ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle,  WriteZMode /* DX_ZWRITE_MASK 等 */ ,  VerticalFlag , String ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawModiStringToZBuffer( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int WriteZMode, int VerticalFlag , const TCHAR *String )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawModiStringToZBuffer( x1, y1, x2, y2, x3, y3, x4, y4, WriteZMode, VerticalFlag , String ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawModiNStringToZBuffer( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int WriteZMode, int VerticalFlag , const TCHAR *String, size_t StringLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawModiNStringToZBuffer( x1, y1, x2, y2, x3, y3, x4, y4, WriteZMode, VerticalFlag , String, StringLength ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawModiFormatStringToZBuffer( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int WriteZMode, int VerticalFlag , const TCHAR *FormatString , ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	
	DXFUNC_BEGIN
	Result = NS_DrawModiStringToZBuffer( x1, y1, x2, y2, x3, y3, x4, y4,  WriteZMode /* DX_ZWRITE_MASK 等 */ ,  VerticalFlag , String ) ;
	DXFUNC_END
	return Result ;
}



extern int DrawStringToHandle( int x, int y, const TCHAR *String, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawStringToHandle( x,  y, String,  Color,  FontHandle,  EdgeColor,  VerticalFlag  ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawNStringToHandle( int x, int y, const TCHAR *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawNStringToHandle( x,  y, String, StringLength,  Color,  FontHandle,  EdgeColor,  VerticalFlag  ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawVStringToHandle( int x, int y, const TCHAR *String, unsigned int Color, int FontHandle, unsigned int EdgeColor )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawVStringToHandle( x,  y, String,  Color,  FontHandle,  EdgeColor ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawNVStringToHandle( int x, int y, const TCHAR *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawNVStringToHandle( x,  y, String, StringLength,  Color,  FontHandle,  EdgeColor ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawFormatStringToHandle( int x, int y, unsigned int Color, int FontHandle, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	DXFUNC_BEGIN
	Result = NS_DrawStringToHandle( x,  y, String,  Color,  FontHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawFormatVStringToHandle( int x, int y, unsigned int Color, int FontHandle, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	DXFUNC_BEGIN
	Result = NS_DrawVStringToHandle( x,  y, String,  Color,  FontHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawExtendStringToHandle( int x, int y, double ExRateX, double ExRateY, const TCHAR *String, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawExtendStringToHandle( x,  y, ExRateX, ExRateY, String,  Color,  FontHandle,  EdgeColor,  VerticalFlag  ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawExtendNStringToHandle( int x, int y, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawExtendNStringToHandle( x,  y, ExRateX, ExRateY, String, StringLength,  Color,  FontHandle,  EdgeColor,  VerticalFlag  ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawExtendVStringToHandle( int x, int y, double ExRateX, double ExRateY, const TCHAR *String, unsigned int Color, int FontHandle, unsigned int EdgeColor )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawExtendVStringToHandle( x,  y, ExRateX, ExRateY, String,  Color,  FontHandle,  EdgeColor ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawExtendNVStringToHandle( int x, int y, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawExtendNVStringToHandle( x,  y, ExRateX, ExRateY, String, StringLength,  Color,  FontHandle,  EdgeColor ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawExtendFormatStringToHandle( int x, int y, double ExRateX, double ExRateY, unsigned int Color, int FontHandle, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	DXFUNC_BEGIN
	Result = NS_DrawExtendStringToHandle( x, y, ExRateX, ExRateY, String,  Color,  FontHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawExtendFormatVStringToHandle( int x, int y, double ExRateX, double ExRateY, unsigned int Color, int FontHandle, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;

	DXFUNC_BEGIN
	Result = NS_DrawExtendVStringToHandle( x, y, ExRateX, ExRateY, String,  Color,  FontHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRotaStringToHandle( int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, int FontHandle, unsigned int EdgeColor , int VerticalFlag , const TCHAR *String )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRotaStringToHandle(  x,  y,  ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle,  Color, FontHandle,  EdgeColor ,  VerticalFlag , String ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRotaNStringToHandle( int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, int FontHandle, unsigned int EdgeColor , int VerticalFlag , const TCHAR *String, size_t StringLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRotaNStringToHandle(  x,  y,  ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle,  Color, FontHandle,  EdgeColor ,  VerticalFlag , String, StringLength ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRotaFormatStringToHandle( int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, int FontHandle, unsigned int EdgeColor , int VerticalFlag , const TCHAR *FormatString , ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;

	DXFUNC_BEGIN
	Result = NS_DrawRotaStringToHandle(  x,  y,  ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle,  Color, FontHandle,  EdgeColor ,  VerticalFlag , String ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawModiStringToHandle( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag, const TCHAR *String )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawModiStringToHandle( x1, y1, x2, y2, x3, y3, x4, y4, Color, FontHandle, EdgeColor, VerticalFlag, String ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawModiNStringToHandle( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag, const TCHAR *String, size_t StringLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawModiNStringToHandle( x1, y1, x2, y2, x3, y3, x4, y4, Color, FontHandle, EdgeColor, VerticalFlag, String, StringLength ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawModiFormatStringToHandle( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;

	DXFUNC_BEGIN
	Result = NS_DrawModiStringToHandle(  x1, y1, x2, y2, x3, y3, x4, y4,  Color, FontHandle,  EdgeColor ,  VerticalFlag , String ) ;
	DXFUNC_END
	return Result ;
}


extern int DrawStringFToHandle( float x, float y, const TCHAR *String, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawStringFToHandle( x,  y, String,  Color,  FontHandle,  EdgeColor,  VerticalFlag  ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawNStringFToHandle( float x, float y, const TCHAR *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawNStringFToHandle( x,  y, String, StringLength,  Color,  FontHandle,  EdgeColor,  VerticalFlag  ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawVStringFToHandle( float x, float y, const TCHAR *String, unsigned int Color, int FontHandle, unsigned int EdgeColor )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawVStringFToHandle( x,  y, String,  Color,  FontHandle,  EdgeColor ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawNVStringFToHandle( float x, float y, const TCHAR *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawNVStringFToHandle( x,  y, String, StringLength,  Color,  FontHandle,  EdgeColor ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawFormatStringFToHandle( float x, float y, unsigned int Color, int FontHandle, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	DXFUNC_BEGIN
	Result = NS_DrawStringFToHandle( x,  y, String,  Color,  FontHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawFormatVStringFToHandle( float x, float y, unsigned int Color, int FontHandle, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	DXFUNC_BEGIN
	Result = NS_DrawVStringFToHandle( x,  y, String,  Color,  FontHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawExtendStringFToHandle( float x, float y, double ExRateX, double ExRateY, const TCHAR *String, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawExtendStringFToHandle( x,  y, ExRateX, ExRateY, String,  Color,  FontHandle,  EdgeColor,  VerticalFlag  ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawExtendNStringFToHandle( float x, float y, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawExtendNStringFToHandle( x,  y, ExRateX, ExRateY, String, StringLength,  Color,  FontHandle,  EdgeColor,  VerticalFlag  ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawExtendVStringFToHandle( float x, float y, double ExRateX, double ExRateY, const TCHAR *String, unsigned int Color, int FontHandle, unsigned int EdgeColor )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawExtendVStringFToHandle( x,  y, ExRateX, ExRateY, String,  Color,  FontHandle,  EdgeColor ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawExtendNVStringFToHandle( float x, float y, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawExtendNVStringFToHandle( x,  y, ExRateX, ExRateY, String, StringLength,  Color,  FontHandle,  EdgeColor ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawExtendFormatStringFToHandle( float x, float y, double ExRateX, double ExRateY, unsigned int Color, int FontHandle, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	DXFUNC_BEGIN
	Result = NS_DrawExtendStringFToHandle( x, y, ExRateX, ExRateY, String,  Color,  FontHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawExtendFormatVStringFToHandle( float x, float y, double ExRateX, double ExRateY, unsigned int Color, int FontHandle, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;

	DXFUNC_BEGIN
	Result = NS_DrawExtendVStringFToHandle( x, y, ExRateX, ExRateY, String,  Color,  FontHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRotaStringFToHandle(	float x, float y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, int FontHandle, unsigned int EdgeColor , int VerticalFlag , const TCHAR *String )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRotaStringFToHandle(  x,  y,  ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle,  Color, FontHandle,  EdgeColor ,  VerticalFlag , String ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRotaNStringFToHandle(	float x, float y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, int FontHandle, unsigned int EdgeColor , int VerticalFlag , const TCHAR *String, size_t StringLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRotaNStringFToHandle(  x,  y,  ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle,  Color, FontHandle,  EdgeColor ,  VerticalFlag , String, StringLength ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRotaFormatStringFToHandle( float x, float y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, int FontHandle, unsigned int EdgeColor , int VerticalFlag , const TCHAR *FormatString , ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;

	DXFUNC_BEGIN
	Result = NS_DrawRotaStringFToHandle(  x,  y,  ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle,  Color, FontHandle,  EdgeColor ,  VerticalFlag , String ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawModiStringFToHandle( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag, const TCHAR *String )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawModiStringFToHandle( x1, y1, x2, y2, x3, y3, x4, y4, Color, FontHandle, EdgeColor, VerticalFlag, String ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawModiNStringFToHandle( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag, const TCHAR *String, size_t StringLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawModiNStringFToHandle( x1, y1, x2, y2, x3, y3, x4, y4, Color, FontHandle, EdgeColor, VerticalFlag, String, StringLength ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawModiFormatStringFToHandle( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;

	DXFUNC_BEGIN
	Result = NS_DrawModiStringFToHandle( x1, y1, x2, y2, x3, y3, x4, y4,  Color, FontHandle,  EdgeColor ,  VerticalFlag , String ) ;
	DXFUNC_END
	return Result ;
}


extern int DrawNumberToIToHandle( int x, int y, int Num, int RisesNum, unsigned int Color, int FontHandle,unsigned int EdgeColor  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawNumberToIToHandle( x, y, Num, RisesNum, Color, FontHandle,EdgeColor  ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawNumberToFToHandle( int x, int y, double Num, int Length, unsigned int Color, int FontHandle,unsigned int EdgeColor  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawNumberToFToHandle( x, y, Num, Length, Color, FontHandle,EdgeColor  ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawNumberPlusToIToHandle( int x, int y, const TCHAR *NoteString, int Num, int RisesNum, unsigned int Color, int FontHandle,unsigned int EdgeColor  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawNumberPlusToIToHandle( x, y, NoteString, Num, RisesNum, Color, FontHandle,EdgeColor  ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawNumberPlusToFToHandle( int x, int y, const TCHAR *NoteString, double Num, int Length, unsigned int Color, int FontHandle,unsigned int EdgeColor  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawNumberPlusToFToHandle( x, y, NoteString, Num, Length, Color, FontHandle,EdgeColor  ) ;
	DXFUNC_END
	return Result ;
}



extern int DrawStringToHandleToZBuffer( int x, int y, const TCHAR *String, int FontHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawStringToHandleToZBuffer(  x,  y, String,    FontHandle,  WriteZMode ,  VerticalFlag );
	DXFUNC_END
	return Result ;
}
extern int DrawNStringToHandleToZBuffer( int x, int y, const TCHAR *String, size_t StringLength, int FontHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawNStringToHandleToZBuffer(  x,  y, String, StringLength,    FontHandle,  WriteZMode ,  VerticalFlag );
	DXFUNC_END
	return Result ;
}
extern int DrawVStringToHandleToZBuffer( int x, int y, const TCHAR *String, int FontHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawVStringToHandleToZBuffer(  x,  y, String,    FontHandle,  WriteZMode );
	DXFUNC_END
	return Result ;
}
extern int DrawNVStringToHandleToZBuffer( int x, int y, const TCHAR *String, size_t StringLength, int FontHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawNVStringToHandleToZBuffer(  x,  y, String, StringLength,    FontHandle,  WriteZMode );
	DXFUNC_END
	return Result ;
}
extern int DrawFormatStringToHandleToZBuffer( int x, int y, int FontHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	
	DXFUNC_BEGIN
	Result = NS_DrawStringToHandleToZBuffer( x,  y, String,   FontHandle,  WriteZMode ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawFormatVStringToHandleToZBuffer( int x, int y, int FontHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	
	DXFUNC_BEGIN
	Result = NS_DrawVStringToHandleToZBuffer( x,  y, String,   FontHandle,  WriteZMode ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawExtendStringToHandleToZBuffer( int x, int y, double ExRateX, double ExRateY, const TCHAR *String, int FontHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawExtendStringToHandleToZBuffer(  x,  y,  ExRateX,  ExRateY, String,    FontHandle,  WriteZMode ,  VerticalFlag );
	DXFUNC_END
	return Result ;
}
extern int DrawExtendNStringToHandleToZBuffer( int x, int y, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, int FontHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawExtendNStringToHandleToZBuffer(  x,  y,  ExRateX,  ExRateY, String, StringLength,    FontHandle,  WriteZMode ,  VerticalFlag );
	DXFUNC_END
	return Result ;
}
extern int DrawExtendVStringToHandleToZBuffer( int x, int y, double ExRateX, double ExRateY, const TCHAR *String, int FontHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawExtendVStringToHandleToZBuffer(  x,  y,  ExRateX,  ExRateY, String,    FontHandle,  WriteZMode );
	DXFUNC_END
	return Result ;
}
extern int DrawExtendNVStringToHandleToZBuffer( int x, int y, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, int FontHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawExtendNVStringToHandleToZBuffer(  x,  y,  ExRateX,  ExRateY, String, StringLength,    FontHandle,  WriteZMode );
	DXFUNC_END
	return Result ;
}
extern int DrawExtendFormatStringToHandleToZBuffer( int x, int y, double ExRateX, double ExRateY, int FontHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	
	DXFUNC_BEGIN
	Result = NS_DrawExtendStringToHandleToZBuffer(  x,  y,  ExRateX,  ExRateY, String,    FontHandle,  WriteZMode );
	DXFUNC_END
	return Result ;
}
extern int DrawExtendFormatVStringToHandleToZBuffer( int x, int y, double ExRateX, double ExRateY, int FontHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	
	DXFUNC_BEGIN
	Result = NS_DrawExtendVStringToHandleToZBuffer(  x,  y,  ExRateX,  ExRateY, String,    FontHandle,  WriteZMode ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRotaStringToHandleToZBuffer( int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, int FontHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag , const TCHAR *String )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRotaStringToHandleToZBuffer(  x,  y,  ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle,  FontHandle,  WriteZMode /* DX_ZWRITE_MASK 等 */ ,  VerticalFlag , String ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRotaNStringToHandleToZBuffer( int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, int FontHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag , const TCHAR *String, size_t StringLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawRotaNStringToHandleToZBuffer(  x,  y,  ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle,  FontHandle,  WriteZMode /* DX_ZWRITE_MASK 等 */ ,  VerticalFlag , String, StringLength ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawRotaFormatStringToHandleToZBuffer( int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, int FontHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag , const TCHAR *FormatString , ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	
	DXFUNC_BEGIN
	Result = NS_DrawRotaStringToHandleToZBuffer(  x,  y,  ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle,  FontHandle,  WriteZMode /* DX_ZWRITE_MASK 等 */ ,  VerticalFlag , String ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawModiStringToHandleToZBuffer( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int FontHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag, const TCHAR *String )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawModiStringToHandleToZBuffer( x1, y1, x2, y2, x3, y3, x4, y4, FontHandle, WriteZMode /* DX_ZWRITE_MASK 等 */ , VerticalFlag, String ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawModiNStringToHandleToZBuffer( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int FontHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag, const TCHAR *String, size_t StringLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawModiNStringToHandleToZBuffer( x1, y1, x2, y2, x3, y3, x4, y4, FontHandle, WriteZMode /* DX_ZWRITE_MASK 等 */ , VerticalFlag, String, StringLength ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawModiFormatStringToHandleToZBuffer(	int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int FontHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	
	DXFUNC_BEGIN
	Result = NS_DrawModiStringToHandleToZBuffer( x1, y1, x2, y2, x3, y3, x4, y4,  FontHandle,  WriteZMode /* DX_ZWRITE_MASK 等 */ ,  VerticalFlag , String ) ;
	DXFUNC_END
	return Result ;
}

#endif // DX_NON_FONT

extern int CreateVertexBuffer( int VertexNum, int VertexType /* DX_VERTEX_TYPE_NORMAL_3D 等 */ )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateVertexBuffer(  VertexNum,  VertexType ) ;
	DXFUNC_END
	return Result ;
}
extern int DeleteVertexBuffer( int VertexBufHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DeleteVertexBuffer( VertexBufHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int InitVertexBuffer()
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_InitVertexBuffer() ;
	DXFUNC_END
	return Result ;
}
extern int SetVertexBufferData( int SetIndex, const void *VertexData, int VertexNum, int VertexBufHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetVertexBufferData(  SetIndex, VertexData,  VertexNum,  VertexBufHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int CreateIndexBuffer( int IndexNum, int IndexType /* DX_INDEX_TYPE_16BIT 等 */  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateIndexBuffer(  IndexNum,  IndexType  ) ;
	DXFUNC_END
	return Result ;
}
extern int DeleteIndexBuffer( int IndexBufHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DeleteIndexBuffer( IndexBufHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int InitIndexBuffer()
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_InitIndexBuffer() ;
	DXFUNC_END
	return Result ;
}
extern int SetIndexBufferData( int SetIndex, const void *IndexData, int IndexNum, int IndexBufHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetIndexBufferData( SetIndex, IndexData,  IndexNum,  IndexBufHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetMaxPrimitiveCount( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetMaxPrimitiveCount() ;
	DXFUNC_END
	return Result ;
}
extern int GetMaxVertexIndex( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetMaxVertexIndex() ;
	DXFUNC_END
	return Result ;
}

extern	int			GetValidShaderVersion( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetValidShaderVersion(  ) ;
	DXFUNC_END
	return Result ;
}

extern	int			LoadVertexShader( const TCHAR *FileName )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadVertexShader( FileName ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadVertexShaderWithStrLen( const TCHAR *FileName, size_t FileNameLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadVertexShaderWithStrLen( FileName, FileNameLength ) ;
	DXFUNC_END
	return Result ;
}

extern	int			LoadVertexShaderFromMem( const void *ImageAddress, int ImageSize )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadVertexShaderFromMem( ImageAddress,  ImageSize ) ;
	DXFUNC_END
	return Result ;
}
extern	int			LoadPixelShader( const TCHAR *FileName )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadPixelShader( FileName ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadPixelShaderWithStrLen( const TCHAR *FileName, size_t FileNameLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadPixelShaderWithStrLen( FileName, FileNameLength ) ;
	DXFUNC_END
	return Result ;
}
extern	int			LoadPixelShaderFromMem( const void *ImageAddress, int ImageSize )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadPixelShaderFromMem( ImageAddress,  ImageSize ) ;
	DXFUNC_END
	return Result ;
}

extern	int			DeleteShader( int ShaderHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DeleteShader( ShaderHandle ) ;
	DXFUNC_END
	return Result ;
}

extern	int			InitShader( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_InitShader() ;
	DXFUNC_END
	return Result ;
}

extern	int			GetConstIndexToShader( const TCHAR *ConstantName, int ShaderHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetConstIndexToShader( ConstantName,  ShaderHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetConstIndexToShaderWithStrLen( const TCHAR *ConstantName, size_t ConstantNameLength, int ShaderHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetConstIndexToShaderWithStrLen( ConstantName, ConstantNameLength, ShaderHandle ) ;
	DXFUNC_END
	return Result ;
}

extern	int			GetConstCountToShader( const TCHAR *ConstantName, int ShaderHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetConstCountToShader( ConstantName, ShaderHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetConstCountToShaderWithStrLen( const TCHAR *ConstantName, size_t ConstantNameLength, int ShaderHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetConstCountToShaderWithStrLen( ConstantName, ConstantNameLength, ShaderHandle ) ;
	DXFUNC_END
	return Result ;
}

extern	const FLOAT4 *GetConstDefaultParamFToShader( const TCHAR *ConstantName, int ShaderHandle )
{
	const FLOAT4 * Result ;
	DXFUNC_BEGIN
	Result = NS_GetConstDefaultParamFToShader( ConstantName, ShaderHandle ) ;
	DXFUNC_END
	return Result ;
}
extern const FLOAT4 *GetConstDefaultParamFToShaderWithStrLen( const TCHAR *ConstantName, size_t ConstantNameLength, int ShaderHandle )
{
	const FLOAT4 * Result ;
	DXFUNC_BEGIN
	Result = NS_GetConstDefaultParamFToShaderWithStrLen( ConstantName, ConstantNameLength, ShaderHandle ) ;
	DXFUNC_END
	return Result ;
}

extern	int			SetVSConstSF( int ConstantIndex, float Param )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetVSConstSF(  ConstantIndex,  Param ) ;
	DXFUNC_END
	return Result ;
}
extern	int			SetVSConstF( int ConstantIndex, FLOAT4 Param )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetVSConstF(  ConstantIndex,  Param ) ;
	DXFUNC_END
	return Result ;
}
extern	int			SetVSConstFMtx( int ConstantIndex, MATRIX Param )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetVSConstFMtx( ConstantIndex, Param ) ;
	DXFUNC_END
	return Result ;
}
extern	int			SetVSConstFMtxT( int ConstantIndex, MATRIX Param )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetVSConstFMtxT( ConstantIndex, Param ) ;
	DXFUNC_END
	return Result ;
}
extern	int			SetVSConstSI( int ConstantIndex, int Param )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetVSConstSI( ConstantIndex,  Param ) ;
	DXFUNC_END
	return Result ;
}
extern	int			SetVSConstI( int ConstantIndex, INT4 Param )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetVSConstI( ConstantIndex,  Param ) ;
	DXFUNC_END
	return Result ;
}
extern	int			SetVSConstB( int ConstantIndex, BOOL Param )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetVSConstB( ConstantIndex, Param ) ;
	DXFUNC_END
	return Result ;
}
extern	int			SetVSConstSFArray( int ConstantIndex, const float *ParamArray, int ParamNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetVSConstSFArray( ConstantIndex, ParamArray, ParamNum ) ;
	DXFUNC_END
	return Result ;
}
extern	int			SetVSConstFArray( int ConstantIndex, const FLOAT4 *ParamArray, int ParamNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetVSConstFArray( ConstantIndex, ParamArray, ParamNum ) ;
	DXFUNC_END
	return Result ;
}
extern	int			SetVSConstFMtxArray( int ConstantIndex, const MATRIX *ParamArray, int ParamNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetVSConstFMtxArray(  ConstantIndex, ParamArray, ParamNum ) ;
	DXFUNC_END
	return Result ;
}
extern	int			SetVSConstFMtxTArray( int ConstantIndex, const MATRIX *ParamArray, int ParamNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetVSConstFMtxTArray(  ConstantIndex, ParamArray, ParamNum ) ;
	DXFUNC_END
	return Result ;
}
extern	int			SetVSConstSIArray( int ConstantIndex, const int *ParamArray, int ParamNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetVSConstSIArray( ConstantIndex, ParamArray, ParamNum ) ;
	DXFUNC_END
	return Result ;
}
extern	int			SetVSConstIArray( int ConstantIndex, const INT4 *ParamArray, int ParamNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetVSConstIArray( ConstantIndex, ParamArray, ParamNum ) ;
	DXFUNC_END
	return Result ;
}
extern	int			SetVSConstBArray( int ConstantIndex, const BOOL *ParamArray, int ParamNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetVSConstBArray( ConstantIndex, ParamArray, ParamNum ) ;
	DXFUNC_END
	return Result ;
}
extern	int			ResetVSConstF( int ConstantIndex, int ParamNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ResetVSConstF(  ConstantIndex,  ParamNum ) ;
	DXFUNC_END
	return Result ;
}
extern	int			ResetVSConstI( int ConstantIndex, int ParamNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ResetVSConstI(  ConstantIndex,  ParamNum ) ;
	DXFUNC_END
	return Result ;
}
extern	int			ResetVSConstB( int ConstantIndex, int ParamNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ResetVSConstB(  ConstantIndex,  ParamNum ) ;
	DXFUNC_END
	return Result ;
}

extern	int			SetPSConstF( int ConstantIndex, FLOAT4 Param )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetPSConstF( ConstantIndex, Param ) ;
	DXFUNC_END
	return Result ;
}
extern	int			SetPSConstSF( int ConstantIndex, float Param )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetPSConstSF( ConstantIndex, Param ) ;
	DXFUNC_END
	return Result ;
}
extern	int			SetPSConstFMtx( int ConstantIndex, MATRIX Param )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetPSConstFMtx( ConstantIndex, Param ) ;
	DXFUNC_END
	return Result ;
}
extern	int			SetPSConstFMtxT( int ConstantIndex, MATRIX Param )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetPSConstFMtxT( ConstantIndex, Param ) ;
	DXFUNC_END
	return Result ;
}
extern	int			SetPSConstSI( int ConstantIndex, int Param )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetPSConstSI( ConstantIndex, Param ) ;
	DXFUNC_END
	return Result ;
}
extern	int			SetPSConstI( int ConstantIndex, INT4 Param )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetPSConstI( ConstantIndex, Param ) ;
	DXFUNC_END
	return Result ;
}
extern	int			SetPSConstB( int ConstantIndex, BOOL Param )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetPSConstB( ConstantIndex, Param ) ;
	DXFUNC_END
	return Result ;
}
extern	int			SetPSConstSFArray( int ConstantIndex, const float *ParamArray, int ParamNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetPSConstSFArray( ConstantIndex, ParamArray, ParamNum ) ;
	DXFUNC_END
	return Result ;
}
extern	int			SetPSConstFArray( int ConstantIndex, const FLOAT4 *ParamArray, int ParamNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetPSConstFArray( ConstantIndex, ParamArray, ParamNum ) ;
	DXFUNC_END
	return Result ;
}
extern	int			SetPSConstFMtxArray( int ConstantIndex, const MATRIX *ParamArray, int ParamNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetPSConstFMtxArray( ConstantIndex, ParamArray, ParamNum ) ;
	DXFUNC_END
	return Result ;
}
extern	int			SetPSConstFMtxTArray( int ConstantIndex, const MATRIX *ParamArray, int ParamNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetPSConstFMtxTArray( ConstantIndex, ParamArray, ParamNum ) ;
	DXFUNC_END
	return Result ;
}
extern	int			SetPSConstSIArray( int ConstantIndex, const int *ParamArray, int ParamNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetPSConstSIArray( ConstantIndex, ParamArray, ParamNum ) ;
	DXFUNC_END
	return Result ;
}
extern	int			SetPSConstIArray( int ConstantIndex, const INT4 *ParamArray, int ParamNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetPSConstIArray( ConstantIndex, ParamArray, ParamNum ) ;
	DXFUNC_END
	return Result ;
}
extern	int			SetPSConstBArray( int ConstantIndex, const BOOL *ParamArray, int ParamNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetPSConstBArray( ConstantIndex, ParamArray, ParamNum ) ;
	DXFUNC_END
	return Result ;
}
extern	int			ResetPSConstF( int ConstantIndex, int ParamNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ResetPSConstF(  ConstantIndex,  ParamNum ) ;
	DXFUNC_END
	return Result ;
}
extern	int			ResetPSConstI( int ConstantIndex, int ParamNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ResetPSConstI(  ConstantIndex,  ParamNum ) ;
	DXFUNC_END
	return Result ;
}
extern	int			ResetPSConstB( int ConstantIndex, int ParamNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ResetPSConstB(  ConstantIndex,  ParamNum ) ;
	DXFUNC_END
	return Result ;
}


extern	int			SetRenderTargetToShader( int TargetIndex, int DrawScreen, int SurfaceIndex , int MipLevel )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetRenderTargetToShader( TargetIndex, DrawScreen, SurfaceIndex, MipLevel ) ;
	DXFUNC_END
	return Result ;
}
extern	int			SetUseTextureToShader( int StageIndex, int GraphHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseTextureToShader( StageIndex, GraphHandle ) ;
	DXFUNC_END
	return Result ;
}

extern	int			SetUseVertexShader( int ShaderHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseVertexShader( ShaderHandle ) ;
	DXFUNC_END
	return Result ;
}
extern	int			SetUsePixelShader( int ShaderHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUsePixelShader( ShaderHandle ) ;
	DXFUNC_END
	return Result ;
}

extern	int			CalcPolygonBinormalAndTangentsToShader( VERTEX3DSHADER *Vertex, int PolygonNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CalcPolygonBinormalAndTangentsToShader( Vertex, PolygonNum ) ;
	DXFUNC_END
	return Result ;
}
extern	int			CalcPolygonIndexedBinormalAndTangentsToShader( VERTEX3DSHADER *Vertex, int VertexNum, const unsigned short *Indices, int PolygonNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CalcPolygonIndexedBinormalAndTangentsToShader( Vertex, VertexNum, Indices, PolygonNum ) ;
	DXFUNC_END
	return Result ;
}

extern	int			DrawBillboard3DToShader( VECTOR Pos, float cx, float cy, float Size, float Angle, int GrHandle, int TransFlag, int ReverseXFlag, int ReverseYFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawBillboard3DToShader( Pos, cx, cy, Size, Angle, GrHandle, TransFlag, ReverseXFlag, ReverseYFlag ) ;
	DXFUNC_END
	return Result ;
}

extern	int			DrawPolygon2DToShader( const VERTEX2DSHADER *Vertex, int PolygonNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawPolygon2DToShader( Vertex, PolygonNum ) ;
	DXFUNC_END
	return Result ;
}
extern	int			DrawPolygon3DToShader( const VERTEX3DSHADER *Vertex, int PolygonNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawPolygon3DToShader( Vertex, PolygonNum ) ;
	DXFUNC_END
	return Result ;
}
extern	int			DrawPolygonIndexed2DToShader( const VERTEX2DSHADER *Vertex, int VertexNum, const unsigned short *Indices, int PolygonNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawPolygonIndexed2DToShader( Vertex, VertexNum, Indices, PolygonNum ) ;
	DXFUNC_END
	return Result ;
}
extern	int			DrawPolygonIndexed3DToShader( const VERTEX3DSHADER *Vertex, int VertexNum, const unsigned short *Indices, int PolygonNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawPolygonIndexed3DToShader( Vertex, VertexNum, Indices, PolygonNum ) ;
	DXFUNC_END
	return Result ;
}
extern	int			DrawPrimitive2DToShader( const VERTEX2DSHADER *Vertex, int VertexNum, int PrimitiveType )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawPrimitive2DToShader( Vertex,  VertexNum,  PrimitiveType ) ;
	DXFUNC_END
	return Result ;
}
extern	int			DrawPrimitive3DToShader( const VERTEX3DSHADER *Vertex, int VertexNum, int PrimitiveType )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawPrimitive3DToShader( Vertex,  VertexNum,  PrimitiveType ) ;
	DXFUNC_END
	return Result ;
}
extern	int			DrawPrimitiveIndexed2DToShader( const VERTEX2DSHADER *Vertex, int VertexNum, const unsigned short *Indices, int IndexNum, int PrimitiveType )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawPrimitiveIndexed2DToShader( Vertex, VertexNum, Indices,  IndexNum,  PrimitiveType ) ;
	DXFUNC_END
	return Result ;
}
extern	int			DrawPrimitiveIndexed3DToShader( const VERTEX3DSHADER *Vertex, int VertexNum, const unsigned short *Indices, int IndexNum, int PrimitiveType )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawPrimitiveIndexed3DToShader( Vertex,  VertexNum, Indices,  IndexNum,  PrimitiveType ) ;
	DXFUNC_END
	return Result ;
}
extern	int			DrawPolygon3DToShader_UseVertexBuffer( int VertexBufHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawPolygon3DToShader_UseVertexBuffer( VertexBufHandle ) ;
	DXFUNC_END
	return Result ;
}
extern	int			DrawPolygonIndexed3DToShader_UseVertexBuffer( int VertexBufHandle, int IndexBufHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawPolygonIndexed3DToShader_UseVertexBuffer(  VertexBufHandle,  IndexBufHandle ) ;
	DXFUNC_END
	return Result ;
}
extern	int			DrawPrimitive3DToShader_UseVertexBuffer( int VertexBufHandle, int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawPrimitive3DToShader_UseVertexBuffer(  VertexBufHandle,  PrimitiveType ) ;
	DXFUNC_END
	return Result ;
}
extern	int			DrawPrimitive3DToShader_UseVertexBuffer2( int VertexBufHandle, int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */, int StartVertex, int UseVertexNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawPrimitive3DToShader_UseVertexBuffer2(  VertexBufHandle,  PrimitiveType,  StartVertex,  UseVertexNum ) ;
	DXFUNC_END
	return Result ;
}
extern	int			DrawPrimitiveIndexed3DToShader_UseVertexBuffer( int VertexBufHandle, int IndexBufHandle, int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawPrimitiveIndexed3DToShader_UseVertexBuffer(  VertexBufHandle,  IndexBufHandle,  PrimitiveType ) ;
	DXFUNC_END
	return Result ;
}
extern	int			DrawPrimitiveIndexed3DToShader_UseVertexBuffer2( int VertexBufHandle, int IndexBufHandle, int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */, int BaseVertex, int StartVertex, int UseVertexNum, int StartIndex, int UseIndexNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawPrimitiveIndexed3DToShader_UseVertexBuffer2(  VertexBufHandle,  IndexBufHandle,  PrimitiveType,  BaseVertex,  StartVertex,  UseVertexNum,  StartIndex,  UseIndexNum ) ;
	DXFUNC_END
	return Result ;
}



extern	int			InitShaderConstantBuffer( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_InitShaderConstantBuffer( ) ;
	DXFUNC_END
	return Result ;
}

extern	int			CreateShaderConstantBuffer( int BufferSize )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateShaderConstantBuffer( BufferSize ) ;
	DXFUNC_END
	return Result ;
}
extern	int			DeleteShaderConstantBuffer( int SConstBufHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DeleteShaderConstantBuffer( SConstBufHandle ) ;
	DXFUNC_END
	return Result ;
}
extern	void *		GetBufferShaderConstantBuffer(	int SConstBufHandle )
{
	void *Result ;
	DXFUNC_BEGIN
	Result = NS_GetBufferShaderConstantBuffer(	SConstBufHandle ) ;
	DXFUNC_END
	return Result ;
}
extern	int			UpdateShaderConstantBuffer( int SConstBufHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_UpdateShaderConstantBuffer( SConstBufHandle ) ;
	DXFUNC_END
	return Result ;
}
extern	int			SetShaderConstantBuffer( int SConstBufHandle, int TargetShader /* DX_SHADERTYPE_VERTEX など */ , int Slot )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetShaderConstantBuffer( SConstBufHandle, TargetShader, Slot ) ;
	DXFUNC_END
	return Result ;
}

#ifndef DX_NON_FILTER

extern	int			GraphFilter( int GrHandle, int FilterType /* DX_BLTFILTER_GAUSS_H 等 */ , ... )
{
	int Result ;
	va_list VaList ;
	int W, H ;

	va_start( VaList, FilterType ) ;

	DXFUNC_BEGIN

	NS_GetGraphSize( GrHandle, &W, &H ) ;

	Result = GraphFilter_RectBltBase( FALSE, GrHandle, -1, GrHandle, 0, FilterType, 0, 0, W, H, 0, 0, FALSE, 0, 0, VaList ) ;
	DXFUNC_END

	va_end( VaList ) ;

	return Result ;
}
extern	int			GraphFilterBlt( int SrcGrHandle, int DestGrHandle, int FilterType, ... )
{
	int Result ;
	va_list VaList ;
	int SrcW, SrcH ;

	va_start( VaList, FilterType ) ;

	DXFUNC_BEGIN

	NS_GetGraphSize( SrcGrHandle, &SrcW, &SrcH ) ;

	Result = GraphFilter_RectBltBase( FALSE, SrcGrHandle, -1, DestGrHandle, 0, FilterType, 0, 0, SrcW, SrcH, 0, 0, FALSE, 0, 0, VaList ) ;
	DXFUNC_END

	va_end( VaList ) ;

	return Result ;
}
extern	int			GraphFilterRectBlt( int SrcGrHandle, int DestGrHandle, int SrcX1, int SrcY1, int SrcX2, int SrcY2, int DestX, int DestY, int FilterType, ... )
{
	int Result ;
	va_list VaList ;

	va_start( VaList, FilterType ) ;

	DXFUNC_BEGIN
	Result = GraphFilter_RectBltBase( FALSE, SrcGrHandle, -1, DestGrHandle, 0, FilterType, SrcX1, SrcY1, SrcX2, SrcY2, 0, 0, TRUE, DestX, DestY, VaList ) ;
	DXFUNC_END

	va_end( VaList ) ;

	return Result ;
}

extern	int			GraphBlend( int GrHandle, int BlendGrHandle, int BlendRatio, int BlendType, ... )
{
	int Result ;
	va_list VaList ;
	int W, H ;

	va_start( VaList, BlendType ) ;

	DXFUNC_BEGIN

	NS_GetGraphSize( GrHandle, &W, &H ) ;

	Result = GraphFilter_RectBltBase( TRUE, GrHandle, BlendGrHandle, GrHandle, BlendRatio, BlendType, 0, 0, W, H, 0, 0, FALSE, 0, 0, VaList ) ;
	DXFUNC_END

	va_end( VaList ) ;

	return Result ;
}
extern	int			GraphBlendBlt( int SrcGrHandle, int BlendGrHandle, int DestGrHandle, int BlendRatio, int BlendType, ... )
{
	int Result ;
	va_list VaList ;
	int SrcW, SrcH ;

	va_start( VaList, BlendType ) ;

	DXFUNC_BEGIN

	NS_GetGraphSize( SrcGrHandle, &SrcW, &SrcH ) ;

	Result = GraphFilter_RectBltBase( TRUE, SrcGrHandle, BlendGrHandle, DestGrHandle, BlendRatio, BlendType, 0, 0, SrcW, SrcH, 0, 0, FALSE, 0, 0, VaList ) ;
	DXFUNC_END

	va_end( VaList ) ;

	return Result ;
}
extern	int			GraphBlendRectBlt( int SrcGrHandle, int BlendGrHandle, int DestGrHandle, int SrcX1, int SrcY1, int SrcX2, int SrcY2, int BlendX, int BlendY, int DestX, int DestY, int BlendRatio, int BlendType, ... )
{
	int Result ;
	va_list VaList ;

	va_start( VaList, BlendType ) ;

	DXFUNC_BEGIN
	Result = GraphFilter_RectBltBase( TRUE, SrcGrHandle, BlendGrHandle, DestGrHandle, BlendRatio, BlendType, SrcX1, SrcY1, SrcX2, SrcY2, BlendX, BlendY, TRUE, DestX, DestY, VaList ) ;
	DXFUNC_END

	va_end( VaList ) ;

	return Result ;
}

#endif // DX_NON_FILTER



// ３Ｄ描画関係関数

// 描画設定関係関数
extern int SetDrawMode( int DrawMode )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetDrawMode( DrawMode ) ;
	DXFUNC_END
	return Result ;
}
extern int SetMaxAnisotropy( int MaxAnisotropy )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetMaxAnisotropy( MaxAnisotropy ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseLarge3DPositionSupport( int UseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseLarge3DPositionSupport( UseFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetDrawBlendMode( int BlendMode, int BlendParam )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetDrawBlendMode( BlendMode, BlendParam ) ;
	DXFUNC_END
	return Result ;
}
extern int SetDrawAlphaTest( int TestMode, int TestParam )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetDrawAlphaTest(  TestMode,  TestParam ) ;
	DXFUNC_END
	return Result ;
}
extern int SetBlendGraph( int BlendGraph, int BorderParam, int BorderRange )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetBlendGraph( BlendGraph, BorderParam, BorderRange ) ;
	DXFUNC_END
	return Result ;
}
extern int SetBlendGraphParam( int BlendGraph, int BlendType, ... )
{
	int Result ;
	va_list VaList ;

	va_start( VaList, BlendType ) ;

	DXFUNC_BEGIN
	Result = Graphics_DrawSetting_SetBlendGraphParamBase(  BlendGraph,  BlendType, VaList ) ;
	DXFUNC_END

	va_end( VaList ) ;

	return Result ;
}
extern int SetBlendGraphPosition( int x, int y )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetBlendGraphPosition( x, y ) ;
	DXFUNC_END
	return Result ;
}
extern int SetBlendGraphPositionMode( int BlendGraphPositionMode /* DX_BLENDGRAPH_POSMODE_DRAWGRAPH など */ )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetBlendGraphPositionMode( BlendGraphPositionMode ) ;
	DXFUNC_END
	return Result ;
}
extern int SetDrawBright( int RedBright, int GreenBright, int BlueBright )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetDrawBright( RedBright, GreenBright, BlueBright ) ;
	DXFUNC_END
	return Result ;
}
extern int SetIgnoreDrawGraphColor( int EnableFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetIgnoreDrawGraphColor( EnableFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetDrawScreen( int DrawScreen )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetDrawScreen( DrawScreen ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawScreen( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawScreen() ;
	DXFUNC_END
	return Result ;
}
extern int SetDrawZBuffer( int DrawScreen )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetDrawZBuffer( DrawScreen ) ;
	DXFUNC_END
	return Result ;
}
extern int SetDrawArea( int x1, int y1, int x2, int y2 )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetDrawArea( x1, y1, x2, y2 ) ;
	DXFUNC_END
	return Result ;
}
extern int SetDraw3DScale( float Scale )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetDraw3DScale( Scale ) ;
	DXFUNC_END
	return Result ;
}
extern int SetDrawAreaFull( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetDrawAreaFull( ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUse3DFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUse3DFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseHardwareVertexProcessing( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseHardwareVertexProcessing( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUsePixelLighting( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUsePixelLighting( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetRestoreShredPoint( void (* ShredPoint )( void ) )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetRestoreShredPoint( ShredPoint ) ;
	DXFUNC_END
	return Result ;
}
extern int SetRestoreGraphCallback( void ( *Callback )( void ))
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetRestoreGraphCallback( Callback ) ;
	DXFUNC_END
	return Result ;
}
extern int RunRestoreShred( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_RunRestoreShred(  ) ;
	DXFUNC_END
	return Result ;
}
extern int SetGraphicsDeviceRestoreCallbackFunction( void (* Callback )( void *Data ), void *CallbackData )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetGraphicsDeviceRestoreCallbackFunction( Callback, CallbackData ) ;
	DXFUNC_END
	return Result ;
}
extern int SetGraphicsDeviceLostCallbackFunction( void (* Callback )( void *Data ), void *CallbackData )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetGraphicsDeviceLostCallbackFunction( Callback, CallbackData ) ;
	DXFUNC_END
	return Result ;
}
extern int SetTransformTo2D( const MATRIX *Matrix )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetTransformTo2D( Matrix ) ;
	DXFUNC_END
	return Result ;
}
extern int SetTransformTo2DD( const MATRIX_D *Matrix )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetTransformTo2DD( Matrix ) ;
	DXFUNC_END
	return Result ;
}
extern int ResetTransformTo2D( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ResetTransformTo2D() ;
	DXFUNC_END
	return Result ;
}
extern int SetTransformToWorld( const MATRIX *Matrix )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetTransformToWorld( Matrix ) ;
	DXFUNC_END
	return Result ;
}
extern int SetTransformToWorldD( const MATRIX_D *Matrix )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetTransformToWorldD( Matrix ) ;
	DXFUNC_END
	return Result ;
}
extern int SetTransformToView( const MATRIX *Matrix )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetTransformToView( Matrix ) ;
	DXFUNC_END
	return Result ;
}
extern int SetTransformToViewD( const MATRIX_D *Matrix )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetTransformToViewD( Matrix ) ;
	DXFUNC_END
	return Result ;
}
extern int SetTransformToProjection( const MATRIX *Matrix )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetTransformToProjection( Matrix ) ;
	DXFUNC_END
	return Result ;
}
extern int SetTransformToProjectionD( const MATRIX_D *Matrix )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetTransformToProjectionD( Matrix ) ;
	DXFUNC_END
	return Result ;
}
extern int SetTransformToViewport( const MATRIX *Matrix )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetTransformToViewport( Matrix ) ;
	DXFUNC_END
	return Result ;
}
extern int SetTransformToViewportD( const MATRIX_D *Matrix )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetTransformToViewportD( Matrix ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseCullingFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseCullingFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseBackCulling( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseBackCulling( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetUseBackCulling( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetUseBackCulling() ;
	DXFUNC_END
	return Result ;
}
extern int SetTextureAddressMode( int Mode, int Stage )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetTextureAddressMode( Mode, Stage ) ;
	DXFUNC_END
	return Result ;
}
extern int SetTextureAddressModeUV( int ModeU, int ModeV, int Stage )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetTextureAddressModeUV(  ModeU, ModeV,  Stage ) ;
	DXFUNC_END
	return Result ;
}
extern int SetTextureAddressTransform( float TransU, float TransV, float ScaleU, float ScaleV, float RotCenterU, float RotCenterV, float Rotate )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetTextureAddressTransform(  TransU,  TransV,  ScaleU,  ScaleV,  RotCenterU,  RotCenterV,  Rotate ) ;
	DXFUNC_END
	return Result ;
}
extern int SetTextureAddressTransformMatrix( MATRIX Matrix )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetTextureAddressTransformMatrix( Matrix ) ;
	DXFUNC_END
	return Result ;
}
extern int ResetTextureAddressTransform( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ResetTextureAddressTransform(  ) ;
	DXFUNC_END
	return Result ;
}
extern int SetFogEnable( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetFogEnable( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetFogEnable( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetFogEnable() ;
	DXFUNC_END
	return Result ;
}
extern int SetFogMode( int Mode )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetFogMode( Mode ) ;
	DXFUNC_END
	return Result ;
}
extern int GetFogMode( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetFogMode() ;
	DXFUNC_END
	return Result ;
}
extern int SetFogColor( int r, int g, int b )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetFogColor(  r,  g,  b ) ;
	DXFUNC_END
	return Result ;
}
extern int GetFogColor( int *r, int *g, int *b )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetFogColor(  r,  g,  b ) ;
	DXFUNC_END
	return Result ;
}
extern int SetFogStartEnd( float start, float end )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetFogStartEnd(  start,  end ) ;
	DXFUNC_END
	return Result ;
}
extern int GetFogStartEnd( float *start, float *end )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetFogStartEnd(  start,  end ) ;
	DXFUNC_END
	return Result ;
}
extern int SetFogDensity( float density )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetFogDensity( density ) ;
	DXFUNC_END
	return Result ;
}
extern float GetFogDensity( void )
{
	float Result ;
	DXFUNC_BEGIN
	Result = NS_GetFogDensity() ;
	DXFUNC_END
	return Result ;
}
extern int SetUseSystemMemGraphCreateFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseSystemMemGraphCreateFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseVramFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseVramFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int RestoreGraphSystem( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_RestoreGraphSystem(  ) ;
	DXFUNC_END
	return Result ;
}











extern int GetDrawScreenGraph( int x1, int y1, int x2, int y2, int GrHandle, int UseClientFlag  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawScreenGraph( x1, y1, x2, y2, GrHandle, UseClientFlag  ) ;
	DXFUNC_END
	return Result ;
}
extern int BltDrawValidGraph( int TargetDrawValidGrHandle, int x1, int y1, int x2, int y2, int DestX, int DestY, int DestGrHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_BltDrawValidGraph(  TargetDrawValidGrHandle,  x1,  y1,  x2,  y2,  DestX,  DestY,  DestGrHandle ) ;
	DXFUNC_END
	return Result ;
}
extern	const unsigned int *GetFullColorImage( int GrHandle )
{
	const unsigned int *Result ;
	DXFUNC_BEGIN
	Result = NS_GetFullColorImage( GrHandle ) ;
	DXFUNC_END
	return Result ;
}

extern int GraphLock( int GrHandle, int *PitchBuf, void **DataPointBuf, COLORDATA **ColorDataPP, int WriteOnly )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GraphLock( GrHandle, PitchBuf, DataPointBuf, ColorDataPP, WriteOnly ) ;
	DXFUNC_END
	return Result ;
}
extern int GraphUnLock( int GrHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GraphUnLock( GrHandle ) ;
	DXFUNC_END
	return Result ;
}

extern int SetUseGraphZBuffer( int GrHandle, int UseFlag, int BitDepth )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseGraphZBuffer( GrHandle, UseFlag, BitDepth ) ;
	DXFUNC_END
	return Result ;
}

extern int CopyGraphZBufferImage( int DestGrHandle, int SrcGrHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CopyGraphZBufferImage(  DestGrHandle,  SrcGrHandle ) ;
	DXFUNC_END
	return Result ;
}

extern int SetDeviceLostDeleteGraphFlag( int GrHandle, int DeleteFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetDeviceLostDeleteGraphFlag(  GrHandle,  DeleteFlag ) ;
	DXFUNC_END
	return Result ;
}

extern int GetGraphSize( int GrHandle, int *SizeXBuf, int *SizeYBuf )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetGraphSize( GrHandle, SizeXBuf, SizeYBuf ) ;
	DXFUNC_END
	return Result ;
}
extern int GetGraphSizeF( int GrHandle, float *SizeXBuf, float *SizeYBuf )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetGraphSizeF( GrHandle, SizeXBuf, SizeYBuf ) ;
	DXFUNC_END
	return Result ;
}
extern int GetGraphTextureSize( int GrHandle, int *SizeXBuf, int *SizeYBuf )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetGraphTextureSize(  GrHandle, SizeXBuf, SizeYBuf ) ;
	DXFUNC_END
	return Result ;
}
extern int GetGraphUseBaseGraphArea( int GrHandle, int *UseX, int *UseY, int *UseSizeX, int *UseSizeY )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetGraphUseBaseGraphArea( GrHandle, UseX, UseY, UseSizeX, UseSizeY ) ;
	DXFUNC_END
	return Result ;
}
extern int GetGraphMipmapCount( int GrHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetGraphMipmapCount( GrHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetGraphFilePath( int GrHandle, TCHAR *FilePathBuffer )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetGraphFilePath( GrHandle, FilePathBuffer ) ;
	DXFUNC_END
	return Result ;
}
extern int CheckDrawValidGraph( int GrHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CheckDrawValidGraph( GrHandle ) ;
	DXFUNC_END
	return Result ;
}











#ifdef __WINDOWS__
extern int LoadGraphToResource( int ResourceID )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadGraphToResource( ResourceID ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadGraphToResourceWithStrLen( const TCHAR *ResourceName, size_t ResourceNameLength, const TCHAR *ResourceType, size_t ResourceTypeLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadGraphToResourceWithStrLen( ResourceName, ResourceNameLength, ResourceType, ResourceTypeLength ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadDivGraphToResource( int ResourceID, int AllNum, int XNum, int YNum, int XSize, int YSize, int *HandleArray )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadDivGraphToResource( ResourceID, AllNum, XNum,  YNum,  XSize,  YSize, HandleArray ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadDivGraphToResourceWithStrLen( const TCHAR *ResourceName, size_t ResourceNameLength, const TCHAR *ResourceType, size_t ResourceTypeLength, int AllNum, int XNum, int YNum, int   XSize, int   YSize, int *HandleArray )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadDivGraphToResourceWithStrLen( ResourceName, ResourceNameLength, ResourceType, ResourceTypeLength, AllNum, XNum, YNum,   XSize,  YSize, HandleArray ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadDivGraphFToResource( int ResourceID, int AllNum, int XNum, int YNum, float XSize, float YSize, int *HandleArray )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadDivGraphFToResource( ResourceID, AllNum, XNum,  YNum,  XSize,  YSize, HandleArray ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadDivGraphFToResourceWithStrLen( const TCHAR *ResourceName, size_t ResourceNameLength, const TCHAR *ResourceType, size_t ResourceTypeLength, int AllNum, int XNum, int YNum, float XSize, float YSize, int *HandleArray )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadDivGraphFToResourceWithStrLen( ResourceName, ResourceNameLength, ResourceType, ResourceTypeLength, AllNum, XNum, YNum, XSize, YSize, HandleArray ) ;
	DXFUNC_END
	return Result ;
}
extern int CreateGraphFromID3D11Texture2D( const void *pID3D11Texture2D )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateGraphFromID3D11Texture2D( pID3D11Texture2D ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadGraphToResource( const TCHAR *ResourceName, const TCHAR *ResourceType )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadGraphToResource( ResourceName, ResourceType ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadDivGraphToResource( const TCHAR *ResourceName, const TCHAR *ResourceType, int AllNum, int XNum, int YNum, int XSize, int YSize, int *HandleArray )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadDivGraphToResource( ResourceName, ResourceType, AllNum, XNum,  YNum,  XSize,  YSize, HandleArray ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadDivGraphFToResource( const TCHAR *ResourceName, const TCHAR *ResourceType, int AllNum, int XNum, int YNum, float XSize, float YSize, int *HandleArray )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadDivGraphFToResource( ResourceName, ResourceType, AllNum, XNum,  YNum,  XSize,  YSize, HandleArray ) ;
	DXFUNC_END
	return Result ;
}

extern	const void*	GetGraphID3D11Texture2D( int GrHandle )
{
	const void* Result ;
	DXFUNC_BEGIN
	Result = NS_GetGraphID3D11Texture2D( GrHandle ) ;
	DXFUNC_END
	return Result ;
}

extern	const void*	GetGraphID3D11RenderTargetView(	int GrHandle )
{
	const void* Result ;
	DXFUNC_BEGIN
	Result = NS_GetGraphID3D11RenderTargetView( GrHandle ) ;
	DXFUNC_END
	return Result ;
}

extern	const void*	GetGraphID3D11DepthStencilView(	int GrHandle )
{
	const void* Result ;
	DXFUNC_BEGIN
	Result = NS_GetGraphID3D11DepthStencilView( GrHandle ) ;
	DXFUNC_END
	return Result ;
}

extern int BltBackScreenToWindow( HWND Window, int ClientX, int ClientY )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_BltBackScreenToWindow(  Window,  ClientX,  ClientY ) ;
	DXFUNC_END
	return Result ;
}
extern int BltRectBackScreenToWindow( HWND Window, RECT BackScreenRect, RECT WindowClientRect )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_BltRectBackScreenToWindow(  Window,  BackScreenRect,  WindowClientRect ) ;
	DXFUNC_END
	return Result ;
}
extern int SetScreenFlipTargetWindow( HWND TargetWindow, double ScaleX, double ScaleY )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetScreenFlipTargetWindow( TargetWindow, ScaleX, ScaleY ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDesktopScreenGraph( int x1, int y1, int x2, int y2, int GrHandle, int DestX, int DestY )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDesktopScreenGraph( x1, y1, x2, y2, GrHandle, DestX, DestY ) ;
	DXFUNC_END
	return Result ;
}

extern int SetMultiThreadFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetMultiThreadFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseDirectDrawDeviceIndex( int Index )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseDirectDrawDeviceIndex( Index ) ;
	DXFUNC_END
	return Result ;
}
extern int SetAeroDisableFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetAeroDisableFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseDirect3D9Ex( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseDirect3D9Ex( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseDirect3D11( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseDirect3D11( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseDirect3D11MinFeatureLevel( int Level )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseDirect3D11MinFeatureLevel( Level ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseDirect3D11WARPDriver( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseDirect3D11WARPDriver( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseDirect3DVersion( int Version )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseDirect3DVersion( Version ) ;
	DXFUNC_END
	return Result ;
}
extern int GetUseDirect3DVersion( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetUseDirect3DVersion() ;
	DXFUNC_END
	return Result ;
}
extern int GetUseDirect3D11FeatureLevel( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetUseDirect3D11FeatureLevel() ;
	DXFUNC_END
	return Result ;
}
extern int SetUseDirectDrawFlag( int Flag )
{
	return NS_SetUseDirectDrawFlag( Flag ) ;
}
extern int SetUseGDIFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseGDIFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetUseGDIFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetUseGDIFlag( ) ;
	DXFUNC_END
	return Result ;
}
extern int SetDDrawUseGuid( const GUID FAR *Guid )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetDDrawUseGuid( Guid ) ;
	DXFUNC_END
	return Result ;
}
extern const void *GetUseDDrawObj( void )
{
	const void *Result ;
	DXFUNC_BEGIN
	Result = NS_GetUseDDrawObj() ;
	DXFUNC_END
	return Result ;
}
extern const GUID *GetDirectDrawDeviceGUID( int Number )
{
	const GUID *Result ;
	DXFUNC_BEGIN
	Result = NS_GetDirectDrawDeviceGUID( Number ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDirectDrawDeviceDescription( int Number, char *StringBuffer )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDirectDrawDeviceDescription( Number, StringBuffer ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDirectDrawDeviceNum( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDirectDrawDeviceNum(  ) ;
	DXFUNC_END
	return Result ;
}
extern const void* GetUseDirect3DDevice9( void )
{
	const void * Result ;
	DXFUNC_BEGIN
	Result = NS_GetUseDirect3DDevice9( ) ;
	DXFUNC_END
	return Result ;
}
extern const void* GetUseDirect3D9BackBufferSurface( void )
{
	const void * Result ;
	DXFUNC_BEGIN
	Result = NS_GetUseDirect3D9BackBufferSurface( ) ;
	DXFUNC_END
	return Result ;
}
extern const void* GetUseDirect3D11Device( void )
{
	const void * Result ;
	DXFUNC_BEGIN
	Result = NS_GetUseDirect3D11Device( ) ;
	DXFUNC_END
	return Result ;
}
extern const void* GetUseDirect3D11DeviceContext( void )
{
	const void * Result ;
	DXFUNC_BEGIN
	Result = NS_GetUseDirect3D11DeviceContext( ) ;
	DXFUNC_END
	return Result ;
}
extern const void* GetUseDirect3D11BackBufferTexture2D( void )
{
	const void * Result ;
	DXFUNC_BEGIN
	Result = NS_GetUseDirect3D11BackBufferTexture2D( ) ;
	DXFUNC_END
	return Result ;
}
extern const void* GetUseDirect3D11BackBufferRenderTargetView( void )
{
	const void * Result ;
	DXFUNC_BEGIN
	Result = NS_GetUseDirect3D11BackBufferRenderTargetView( ) ;
	DXFUNC_END
	return Result ;
}
extern const void* GetUseDirect3D11DepthStencilTexture2D( void )
{
	const void * Result ;
	DXFUNC_BEGIN
	Result = NS_GetUseDirect3D11DepthStencilTexture2D( ) ;
	DXFUNC_END
	return Result ;
}
extern int SetDrawScreen_ID3D11RenderTargetView( const void *pID3D11RenderTargetView, const void *pID3D11DepthStencilView )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetDrawScreen_ID3D11RenderTargetView( pID3D11RenderTargetView, pID3D11DepthStencilView ) ;
	DXFUNC_END
	return Result ;
}
extern int RefreshDxLibDirect3DSetting( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_RefreshDxLibDirect3DSetting() ;
	DXFUNC_END
	return Result ;
}

#ifndef DX_NON_MEDIA_FOUNDATION
extern int SetUseMediaFoundationFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseMediaFoundationFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
#endif // DX_NON_MEDIA_FOUNDATION

extern int ColorKaiseki( const void *PixelData, COLORDATA * ColorData )
{
	return NS_ColorKaiseki( PixelData, ColorData ) ;
}

#endif // __WINDOWS__













extern int GetMaxGraphTextureSize( int *SizeX, int *SizeY )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetMaxGraphTextureSize( SizeX, SizeY ) ;
	DXFUNC_END
	return Result ;
}
extern int GetScreenState( int *SizeX, int *SizeY, int *ColorBitDepth )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetScreenState( SizeX, SizeY, ColorBitDepth ) ;
	DXFUNC_END
	return Result ;
}
extern int GetUse3DFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetUse3DFlag(  ) ;
	DXFUNC_END
	return Result ;
}
extern int GetValidRestoreShredPoint( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetValidRestoreShredPoint(  ) ;
	DXFUNC_END
	return Result ;
}
extern int GetTransformToViewMatrix( MATRIX *MatBuf )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetTransformToViewMatrix( MatBuf ) ;
	DXFUNC_END
	return Result ;
}
extern int GetTransformToViewMatrixD( MATRIX_D *MatBuf )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetTransformToViewMatrixD( MatBuf ) ;
	DXFUNC_END
	return Result ;
}
extern int GetTransformToWorldMatrix( MATRIX *MatBuf )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetTransformToWorldMatrix( MatBuf ) ;
	DXFUNC_END
	return Result ;
}
extern int GetTransformToWorldMatrixD( MATRIX_D *MatBuf )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetTransformToWorldMatrixD( MatBuf ) ;
	DXFUNC_END
	return Result ;
}
extern int GetTransformToProjectionMatrix( MATRIX *MatBuf )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetTransformToProjectionMatrix( MatBuf ) ;
	DXFUNC_END
	return Result ;
}
extern int GetTransformToProjectionMatrixD( MATRIX_D *MatBuf )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetTransformToProjectionMatrixD( MatBuf ) ;
	DXFUNC_END
	return Result ;
}
extern int GetTransformToViewportMatrix( MATRIX *MatBuf )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetTransformToViewportMatrix( MatBuf ) ;
	DXFUNC_END
	return Result ;
}
extern int GetTransformToViewportMatrixD( MATRIX_D *MatBuf )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetTransformToViewportMatrixD( MatBuf ) ;
	DXFUNC_END
	return Result ;
}
extern int GetTransformToAPIViewportMatrix( MATRIX *MatBuf )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetTransformToAPIViewportMatrix( MatBuf ) ;
	DXFUNC_END
	return Result ;
}
extern int GetTransformToAPIViewportMatrixD( MATRIX_D *MatBuf )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetTransformToAPIViewportMatrixD( MatBuf ) ;
	DXFUNC_END
	return Result ;
}
extern int GetTransformPosition( VECTOR *LocalPos, float *x, float *y )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetTransformPosition( LocalPos, x, y ) ;
	DXFUNC_END
	return Result ;
}
extern int GetTransformPositionD( VECTOR_D *LocalPos, double *x, double *y )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetTransformPositionD( LocalPos, x, y ) ;
	DXFUNC_END
	return Result ;
}
extern float GetBillboardPixelSize( VECTOR WorldPos, float WorldSize )
{
	float Result ;
	DXFUNC_BEGIN
	Result = NS_GetBillboardPixelSize( WorldPos, WorldSize ) ;
	DXFUNC_END
	return Result ;
}
extern double GetBillboardPixelSizeD( VECTOR_D WorldPos, double WorldSize )
{
	double Result ;
	DXFUNC_BEGIN
	Result = NS_GetBillboardPixelSizeD( WorldPos, WorldSize ) ;
	DXFUNC_END
	return Result ;
}
extern VECTOR ConvWorldPosToViewPos( VECTOR WorldPos )
{
	VECTOR Result ;
	DXFUNC_BEGIN
	Result = NS_ConvWorldPosToViewPos( WorldPos ) ;
	DXFUNC_END
	return Result ;
}
extern VECTOR_D ConvWorldPosToViewPosD( VECTOR_D WorldPos )
{
	VECTOR_D Result ;
	DXFUNC_BEGIN
	Result = NS_ConvWorldPosToViewPosD( WorldPos ) ;
	DXFUNC_END
	return Result ;
}
extern VECTOR ConvWorldPosToScreenPos( VECTOR WorldPos )
{
	VECTOR Result ;
	DXFUNC_BEGIN
	Result = NS_ConvWorldPosToScreenPos( WorldPos ) ;
	DXFUNC_END
	return Result ;
}
extern VECTOR_D ConvWorldPosToScreenPosD( VECTOR_D WorldPos )
{
	VECTOR_D Result ;
	DXFUNC_BEGIN
	Result = NS_ConvWorldPosToScreenPosD( WorldPos ) ;
	DXFUNC_END
	return Result ;
}
extern FLOAT4 ConvWorldPosToScreenPosPlusW( VECTOR WorldPos )
{
	FLOAT4 Result ;
	DXFUNC_BEGIN
	Result = NS_ConvWorldPosToScreenPosPlusW( WorldPos ) ;
	DXFUNC_END
	return Result ;
}
extern DOUBLE4 ConvWorldPosToScreenPosPlusWD( VECTOR_D WorldPos )
{
	DOUBLE4 Result ;
	DXFUNC_BEGIN
	Result = NS_ConvWorldPosToScreenPosPlusWD( WorldPos ) ;
	DXFUNC_END
	return Result ;
}
extern VECTOR ConvScreenPosToWorldPos( VECTOR ScreenPos )
{
	VECTOR Result ;
	DXFUNC_BEGIN
	Result = NS_ConvScreenPosToWorldPos( ScreenPos ) ;
	DXFUNC_END
	return Result ;
}
extern VECTOR_D ConvScreenPosToWorldPosD( VECTOR_D ScreenPos )
{
	VECTOR_D Result ;
	DXFUNC_BEGIN
	Result = NS_ConvScreenPosToWorldPosD( ScreenPos ) ;
	DXFUNC_END
	return Result ;
}
extern VECTOR ConvScreenPosToWorldPos_ZLinear( VECTOR ScreenPos )
{
	VECTOR Result ;
	DXFUNC_BEGIN
	Result = NS_ConvScreenPosToWorldPos_ZLinear( ScreenPos ) ;
	DXFUNC_END
	return Result ;
}
extern VECTOR_D ConvScreenPosToWorldPos_ZLinearD( VECTOR_D ScreenPos )
{
	VECTOR_D Result ;
	DXFUNC_BEGIN
	Result = NS_ConvScreenPosToWorldPos_ZLinearD( ScreenPos ) ;
	DXFUNC_END
	return Result ;
}
extern int GetUseVramFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetUseVramFlag(  ) ;
	DXFUNC_END
	return Result ;
}
extern int GetMultiDrawScreenNum( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetMultiDrawScreenNum( ) ;
	DXFUNC_END
	return Result ;
}

extern int GetCreateGraphColorData( COLORDATA *ColorData, IMAGEFORMATDESC *Format )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetCreateGraphColorData( ColorData, Format ) ;
	DXFUNC_END
	return Result ;
}


extern int CreateDXGraph( const BASEIMAGE *RgbBaseImage, const BASEIMAGE *AlphaBaseImage, int TextureFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateDXGraph( RgbBaseImage, AlphaBaseImage, TextureFlag ) ;
	DXFUNC_END
	return Result ;
}
/*
extern int CreateDXDivGraph( BASEIMAGE *RgbBaseImage, BASEIMAGE *AlphaBaseImage, int AllNum, int XNum, int YNum, int SizeX, int SizeY, int *HandleArray, int TextureFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateDXDivGraph( RgbBaseImage, AlphaBaseImage, AllNum, XNum, YNum, SizeX, SizeY, HandleArray, TextureFlag ) ;
	DXFUNC_END
	return Result ;
}
*/
extern int DerivationGraph( int SrcX, int SrcY, int Width, int Height, int SrcGraphHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DerivationGraph( SrcX, SrcY, Width, Height, SrcGraphHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int DerivationGraphF( float SrcX, float SrcY, float Width, float Height, int SrcGraphHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DerivationGraphF( SrcX, SrcY, Width, Height, SrcGraphHandle ) ;
	DXFUNC_END
	return Result ;
}

#ifndef DX_NON_MOVIE
// ムービーグラフィック関係関数
extern int PlayMovie( const TCHAR *FileName, int ExRate, int PlayType )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_PlayMovie( FileName, ExRate, PlayType ) ;
	DXFUNC_END
	return Result ;
}
extern int PlayMovieWithStrLen( const TCHAR *FileName, size_t FileNameLength, int ExRate, int PlayType )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_PlayMovieWithStrLen( FileName, FileNameLength, ExRate, PlayType ) ;
	DXFUNC_END
	return Result ;
}
extern int GetMovieImageSize_File( const TCHAR *FileName, int *SizeX, int *SizeY )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetMovieImageSize_File( FileName, SizeX, SizeY ) ;
	DXFUNC_END
	return Result ;
}
extern int GetMovieImageSize_File_WithStrLen( const TCHAR *FileName, size_t FileNameLength, int *SizeX, int *SizeY )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetMovieImageSize_File_WithStrLen( FileName, FileNameLength, SizeX, SizeY ) ;
	DXFUNC_END
	return Result ;
}
extern int GetMovieImageSize_Mem( const void *FileImage, int FileImageSize, int *SizeX, int *SizeY )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetMovieImageSize_Mem( FileImage, FileImageSize, SizeX, SizeY ) ;
	DXFUNC_END
	return Result ;
}
extern int OpenMovieToGraph( const TCHAR *FileName, int FullColor )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_OpenMovieToGraph( FileName, FullColor ) ;
	DXFUNC_END
	return Result ;
}
extern int OpenMovieToGraphWithStrLen( const TCHAR *FileName, size_t FileNameLength, int FullColor )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_OpenMovieToGraphWithStrLen( FileName, FileNameLength, FullColor ) ;
	DXFUNC_END
	return Result ;
}
extern int PlayMovieToGraph( int GraphHandle, int PlayType, int SysPlay )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_PlayMovieToGraph( GraphHandle, PlayType, SysPlay ) ;
	DXFUNC_END
	return Result ;
}
extern int PauseMovieToGraph( int GraphHandle, int SysPause )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_PauseMovieToGraph( GraphHandle, SysPause ) ;
	DXFUNC_END
	return Result ;
}
extern int AddMovieFrameToGraph( int GraphHandle, unsigned int FrameNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_AddMovieFrameToGraph( GraphHandle, FrameNum ) ;
	DXFUNC_END
	return Result ;
}
extern int SeekMovieToGraph( int GraphHandle, int Time )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SeekMovieToGraph( GraphHandle, Time ) ;
	DXFUNC_END
	return Result ;
}
extern int SetPlaySpeedRateMovieToGraph( int GraphHandle, double SpeedRate )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetPlaySpeedRateMovieToGraph( GraphHandle, SpeedRate ) ;
	DXFUNC_END
	return Result ;
}
extern int GetMovieStateToGraph( int GraphHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetMovieStateToGraph( GraphHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetMovieVolumeToGraph( int Volume, int GraphHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetMovieVolumeToGraph( Volume, GraphHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int ChangeMovieVolumeToGraph( int Volume, int GraphHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ChangeMovieVolumeToGraph( Volume, GraphHandle ) ;
	DXFUNC_END
	return Result ;
}
extern	const BASEIMAGE *GetMovieBaseImageToGraph( int GraphHandle, int *ImageUpdateFlag , int ImageUpdateFlagSetOnly )
{
	const BASEIMAGE *Result ;
	DXFUNC_BEGIN
	Result = NS_GetMovieBaseImageToGraph( GraphHandle, ImageUpdateFlag , ImageUpdateFlagSetOnly ) ;
	DXFUNC_END
	return Result ;
}

extern int GetMovieTotalFrameToGraph( int GraphHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetMovieTotalFrameToGraph( GraphHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int TellMovieToGraph( int GraphHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_TellMovieToGraph( GraphHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int TellMovieToGraphToFrame( int GraphHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_TellMovieToGraphToFrame( GraphHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SeekMovieToGraphToFrame( int GraphHandle, int Frame )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SeekMovieToGraphToFrame( GraphHandle, Frame ) ;
	DXFUNC_END
	return Result ;
}
extern LONGLONG GetOneFrameTimeMovieToGraph( int GraphHandle )
{
	LONGLONG Result ;
	DXFUNC_BEGIN
	Result = NS_GetOneFrameTimeMovieToGraph( GraphHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetLastUpdateTimeMovieToGraph( int GraphHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetLastUpdateTimeMovieToGraph( GraphHandle ) ;
	DXFUNC_END
	return Result ;
}

#endif


// グラフィックハンドル作成系
extern int LoadBmpToGraph( const TCHAR *FileName, int TextureFlag, int ReverseFlag, int SurfaceMode )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadBmpToGraph( FileName, TextureFlag, ReverseFlag, SurfaceMode ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadBmpToGraphWithStrLen( const TCHAR *FileName, size_t FileNameLength, int TextureFlag, int ReverseFlag, int SurfaceMode )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadBmpToGraphWithStrLen( FileName, FileNameLength, TextureFlag, ReverseFlag, SurfaceMode ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadGraph( const TCHAR *FileName, int NotUse3DFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadGraph( FileName, NotUse3DFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadGraphWithStrLen( const TCHAR *FileName, size_t FileNameLength, int NotUse3DFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadGraphWithStrLen( FileName, FileNameLength, NotUse3DFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadReverseGraph( const TCHAR *FileName, int NotUse3DFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadReverseGraph( FileName, NotUse3DFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadReverseGraphWithStrLen( const TCHAR *FileName, size_t FileNameLength, int NotUse3DFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadReverseGraphWithStrLen( FileName, FileNameLength, NotUse3DFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadDivGraph( const TCHAR *FileName, int AllNum, int XNum, int YNum, int XSize, int YSize, int *HandleArray, int NotUse3DFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadDivGraph( FileName, AllNum,  XNum,  YNum,  XSize,  YSize, HandleArray,  NotUse3DFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadDivGraphWithStrLen( const TCHAR *FileName, size_t FileNameLength, int AllNum, int XNum, int YNum, int   XSize, int   YSize, int *HandleArray, int NotUse3DFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadDivGraphWithStrLen( FileName, FileNameLength, AllNum, XNum, YNum, XSize, YSize, HandleArray, NotUse3DFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadDivGraphF( const TCHAR *FileName, int AllNum, int XNum, int YNum, float XSize, float YSize, int *HandleArray, int NotUse3DFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadDivGraphF( FileName, AllNum,  XNum,  YNum,  XSize,  YSize, HandleArray,  NotUse3DFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadDivGraphFWithStrLen( const TCHAR *FileName, size_t FileNameLength, int AllNum, int XNum, int YNum, float XSize, float YSize, int *HandleArray, int NotUse3DFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadDivGraphFWithStrLen( FileName, FileNameLength, AllNum, XNum, YNum, XSize, YSize, HandleArray, NotUse3DFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadDivBmpToGraph( const TCHAR *FileName, int AllNum, int XNum, int YNum, int SizeX, int SizeY, int *HandleArray, int TextureFlag, int ReverseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadDivBmpToGraph( FileName, AllNum,  XNum,  YNum,  SizeX,  SizeY,  HandleArray,  TextureFlag,  ReverseFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadDivBmpToGraphWithStrLen( const TCHAR *FileName, size_t FileNameLength, int AllNum, int XNum, int YNum, int   SizeX, int   SizeY, int *HandleArray, int TextureFlag, int ReverseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadDivBmpToGraphWithStrLen( FileName, FileNameLength, AllNum, XNum, YNum, SizeX, SizeY, HandleArray, TextureFlag, ReverseFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadDivBmpToGraphF( const TCHAR *FileName, int AllNum, int XNum, int YNum, float SizeX, float SizeY, int *HandleArray, int TextureFlag, int ReverseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadDivBmpToGraphF( FileName, AllNum,  XNum,  YNum,  SizeX,  SizeY,  HandleArray,  TextureFlag,  ReverseFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadDivBmpToGraphFWithStrLen( const TCHAR *FileName, size_t FileNameLength, int AllNum, int XNum, int YNum, float SizeX, float SizeY, int *HandleArray, int TextureFlag, int ReverseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadDivBmpToGraphFWithStrLen( FileName, FileNameLength, AllNum, XNum, YNum, SizeX, SizeY, HandleArray, TextureFlag, ReverseFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadReverseDivGraph( const TCHAR *FileName, int AllNum, int XNum, int YNum, int XSize, int YSize, int *HandleArray, int NotUse3DFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadReverseDivGraph( FileName, AllNum,  XNum,  YNum,  XSize,  YSize, HandleArray,  NotUse3DFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadReverseDivGraphWithStrLen( const TCHAR *FileName, size_t FileNameLength, int AllNum, int XNum, int YNum, int   XSize, int   YSize, int *HandleArray, int NotUse3DFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadReverseDivGraphWithStrLen( FileName, FileNameLength, AllNum, XNum, YNum, XSize, YSize, HandleArray, NotUse3DFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadReverseDivGraphF( const TCHAR *FileName, int AllNum, int XNum, int YNum, float XSize, float YSize, int *HandleArray, int NotUse3DFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadReverseDivGraphF( FileName, AllNum,  XNum,  YNum,  XSize,  YSize, HandleArray,  NotUse3DFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadReverseDivGraphFWithStrLen( const TCHAR *FileName, size_t FileNameLength, int AllNum, int XNum, int YNum, float XSize, float YSize, int *HandleArray, int NotUse3DFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadReverseDivGraphFWithStrLen( FileName, FileNameLength, AllNum, XNum, YNum, XSize, YSize, HandleArray, NotUse3DFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadBlendGraph( const TCHAR *FileName )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadBlendGraph( FileName ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadBlendGraphWithStrLen( const TCHAR *FileName, size_t FileNameLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadBlendGraphWithStrLen( FileName, FileNameLength ) ;
	DXFUNC_END
	return Result ;
}

extern int CreateGraphFromMem( const void *RGBFileImage, int RGBFileImageSize, const void *AlphaFileImage , int AlphaFileImageSize , int TextureFlag , int ReverseFlag  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateGraphFromMem( RGBFileImage, RGBFileImageSize,  AlphaFileImage , AlphaFileImageSize , TextureFlag , ReverseFlag  ) ;
	DXFUNC_END
	return Result ;
}
extern int ReCreateGraphFromMem( const void *RGBFileImage, int RGBFileImageSize, int GrHandle, const void *AlphaFileImage , int AlphaFileImageSize , int TextureFlag , int ReverseFlag  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReCreateGraphFromMem( RGBFileImage, RGBFileImageSize, GrHandle,  AlphaFileImage , AlphaFileImageSize , TextureFlag , ReverseFlag  ) ;
	DXFUNC_END
	return Result ;
}
extern int CreateDivGraphFromMem( const void *RGBFileImage, int RGBFileImageSize, int AllNum, int XNum, int YNum, int SizeX, int SizeY, int *HandleArray,int TextureFlag , int ReverseFlag , const void *AlphaFileImage , int AlphaFileImageSize )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateDivGraphFromMem( RGBFileImage, RGBFileImageSize, AllNum, XNum, YNum, SizeX, SizeY, HandleArray,TextureFlag , ReverseFlag ,  AlphaFileImage , AlphaFileImageSize  ) ;
	DXFUNC_END
	return Result ;
}
extern int CreateDivGraphFFromMem( const void *RGBFileImage, int RGBFileImageSize, int AllNum, int XNum, int YNum, float SizeX, float SizeY, int *HandleArray,int TextureFlag , int ReverseFlag , const void *AlphaFileImage , int AlphaFileImageSize )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateDivGraphFFromMem( RGBFileImage, RGBFileImageSize, AllNum, XNum, YNum, SizeX, SizeY, HandleArray,TextureFlag , ReverseFlag ,  AlphaFileImage , AlphaFileImageSize  ) ;
	DXFUNC_END
	return Result ;
}
extern int ReCreateDivGraphFromMem( const void *RGBFileImage, int RGBFileImageSize, int AllNum, int XNum, int YNum, int SizeX, int SizeY, const int *HandleArray,int TextureFlag , int ReverseFlag , const void *AlphaFileImage , int AlphaFileImageSize  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReCreateDivGraphFromMem( RGBFileImage, RGBFileImageSize, AllNum, XNum, YNum, SizeX, SizeY, HandleArray,TextureFlag , ReverseFlag ,  AlphaFileImage , AlphaFileImageSize  ) ;
	DXFUNC_END
	return Result ;
}
extern int ReCreateDivGraphFFromMem( const void *RGBFileImage, int RGBFileImageSize, int AllNum, int XNum, int YNum, float SizeX, float SizeY, const int *HandleArray,int TextureFlag , int ReverseFlag , const void *AlphaFileImage , int AlphaFileImageSize  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReCreateDivGraphFFromMem( RGBFileImage, RGBFileImageSize, AllNum, XNum, YNum, SizeX, SizeY, HandleArray,TextureFlag , ReverseFlag ,  AlphaFileImage , AlphaFileImageSize  ) ;
	DXFUNC_END
	return Result ;
}
extern int CreateGraphFromBmp( const BITMAPINFO *RGBBmpInfo, const void *GraphData, const BITMAPINFO *AlphaInfo , const void *AlphaData , int TextureFlag , int ReverseFlag  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateGraphFromBmp( RGBBmpInfo, GraphData, AlphaInfo , AlphaData , TextureFlag , ReverseFlag  ) ;
	DXFUNC_END
	return Result ;
}
extern int ReCreateGraphFromBmp( const BITMAPINFO *RGBBmpInfo, const void *RGBBmpImage, int GrHandle, const BITMAPINFO *AlphaBmpInfo , const void *AlphaBmpImage , int TextureFlag , int ReverseFlag  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReCreateGraphFromBmp( RGBBmpInfo, RGBBmpImage, GrHandle, AlphaBmpInfo ,  AlphaBmpImage , TextureFlag , ReverseFlag  ) ;
	DXFUNC_END
	return Result ;
}
extern int CreateDivGraphFromBmp( const BITMAPINFO *RGBBmpInfo, const void *RGBBmpImage, int AllNum, int XNum, int YNum, int SizeX, int SizeY, int *HandleArray,int TextureFlag , int ReverseFlag , const BITMAPINFO *AlphaBmpInfo , const void *AlphaBmpImage  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateDivGraphFromBmp( RGBBmpInfo, RGBBmpImage, AllNum, XNum, YNum, SizeX, SizeY, HandleArray,TextureFlag , ReverseFlag , AlphaBmpInfo ,  AlphaBmpImage  ) ;
	DXFUNC_END
	return Result ;
}
extern int CreateDivGraphFFromBmp( const BITMAPINFO *RGBBmpInfo, const void *RGBBmpImage, int AllNum, int XNum, int YNum, float SizeX, float SizeY, int *HandleArray,int TextureFlag , int ReverseFlag , const BITMAPINFO *AlphaBmpInfo , const void *AlphaBmpImage  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateDivGraphFFromBmp( RGBBmpInfo, RGBBmpImage, AllNum, XNum, YNum, SizeX, SizeY, HandleArray,TextureFlag , ReverseFlag , AlphaBmpInfo ,  AlphaBmpImage  ) ;
	DXFUNC_END
	return Result ;
}
extern int ReCreateDivGraphFromBmp( const BITMAPINFO *RGBBmpInfo, const void *RGBBmpImage, int AllNum, int XNum, int YNum, int SizeX, int SizeY, const int *HandleArray,int TextureFlag , int ReverseFlag , const BITMAPINFO *AlphaBmpInfo , const void *AlphaBmpImage  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReCreateDivGraphFromBmp( RGBBmpInfo, RGBBmpImage, AllNum, XNum, YNum, SizeX, SizeY, HandleArray,TextureFlag , ReverseFlag , AlphaBmpInfo ,  AlphaBmpImage  ) ;
	DXFUNC_END
	return Result ;
}
extern int ReCreateDivGraphFFromBmp( const BITMAPINFO *RGBBmpInfo, const void *RGBBmpImage, int AllNum, int XNum, int YNum, float SizeX, float SizeY, const int *HandleArray,int TextureFlag , int ReverseFlag , const BITMAPINFO *AlphaBmpInfo , const void *AlphaBmpImage  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReCreateDivGraphFFromBmp( RGBBmpInfo, RGBBmpImage, AllNum, XNum, YNum, SizeX, SizeY, HandleArray,TextureFlag , ReverseFlag , AlphaBmpInfo ,  AlphaBmpImage  ) ;
	DXFUNC_END
	return Result ;
}
extern int CreateGraphFromGraphImage( const BASEIMAGE *RGBImage, int TextureFlag , int ReverseFlag  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateGraphFromGraphImage( RGBImage, TextureFlag , ReverseFlag  ) ;
	DXFUNC_END
	return Result ;
}
extern int CreateGraphFromGraphImage( const BASEIMAGE *RGBImage, const BASEIMAGE *AlphaBaseImage, int TextureFlag , int ReverseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateGraphFromGraphImage( RGBImage, AlphaBaseImage, TextureFlag , ReverseFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int ReCreateGraphFromGraphImage( const BASEIMAGE *RGBImage, int GrHandle, int TextureFlag , int ReverseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReCreateGraphFromGraphImage( RGBImage, GrHandle, TextureFlag , ReverseFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int ReCreateGraphFromGraphImage( const BASEIMAGE *RGBImage, const BASEIMAGE *AlphaBaseImage, int GrHandle, int TextureFlag , int ReverseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReCreateGraphFromGraphImage( RGBImage, AlphaBaseImage, GrHandle, TextureFlag , ReverseFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int CreateDivGraphFromGraphImage( BASEIMAGE *RGBImage, int AllNum, int XNum, int YNum, int SizeX, int SizeY, int *HandleArray,int TextureFlag , int ReverseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateDivGraphFromGraphImage( RGBImage, AllNum, XNum, YNum, SizeX, SizeY, HandleArray,TextureFlag , ReverseFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int CreateDivGraphFFromGraphImage( BASEIMAGE *RGBImage, int AllNum, int XNum, int YNum, float SizeX, float SizeY, int *HandleArray,int TextureFlag , int ReverseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateDivGraphFFromGraphImage( RGBImage, AllNum, XNum, YNum, SizeX, SizeY, HandleArray,TextureFlag , ReverseFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int CreateDivGraphFromGraphImage( BASEIMAGE *RGBImage, const BASEIMAGE *AlphaBaseImage, int AllNum, int XNum, int YNum, int SizeX, int SizeY, int *HandleArray,int TextureFlag , int ReverseFlag  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateDivGraphFromGraphImage( RGBImage, AlphaBaseImage, AllNum, XNum, YNum, SizeX, SizeY, HandleArray,TextureFlag , ReverseFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int CreateDivGraphFFromGraphImage( BASEIMAGE *RGBImage, const BASEIMAGE *AlphaBaseImage, int AllNum, int XNum, int YNum, float SizeX, float SizeY, int *HandleArray,int TextureFlag , int ReverseFlag  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateDivGraphFFromGraphImage( RGBImage, AlphaBaseImage, AllNum, XNum, YNum, SizeX, SizeY, HandleArray,TextureFlag , ReverseFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int ReCreateDivGraphFromGraphImage( BASEIMAGE *RGBImage, int AllNum, int XNum, int YNum, int SizeX, int SizeY, const int *HandleArray,int TextureFlag , int ReverseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReCreateDivGraphFromGraphImage( RGBImage, AllNum, XNum, YNum, SizeX, SizeY, HandleArray,TextureFlag , ReverseFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int ReCreateDivGraphFFromGraphImage( BASEIMAGE *RGBImage, int AllNum, int XNum, int YNum, float SizeX, float SizeY, const int *HandleArray,int TextureFlag , int ReverseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReCreateDivGraphFFromGraphImage( RGBImage, AllNum, XNum, YNum, SizeX, SizeY, HandleArray,TextureFlag , ReverseFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int ReCreateDivGraphFromGraphImage( BASEIMAGE *RGBImage, const BASEIMAGE *AlphaBaseImage, int AllNum, int XNum, int YNum, int SizeX, int SizeY, const int *HandleArray,int TextureFlag , int ReverseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReCreateDivGraphFromGraphImage( RGBImage, AlphaBaseImage, AllNum, XNum, YNum, SizeX, SizeY, HandleArray,TextureFlag , ReverseFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int ReCreateDivGraphFFromGraphImage( BASEIMAGE *RGBImage, const BASEIMAGE *AlphaBaseImage, int AllNum, int XNum, int YNum, float SizeX, float SizeY, const int *HandleArray,int TextureFlag , int ReverseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReCreateDivGraphFFromGraphImage( RGBImage, AlphaBaseImage, AllNum, XNum, YNum, SizeX, SizeY, HandleArray,TextureFlag , ReverseFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int CreateGraph( int Width, int Height, int Pitch, const void *GraphData, const void *AlphaData , int GrHandle  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateGraph( Width, Height, Pitch, GraphData,  AlphaData , GrHandle  ) ;
	DXFUNC_END
	return Result ;
}
extern int CreateDivGraph( int Width, int Height, int Pitch, const void *GraphData, int AllNum, int XNum, int YNum, int SizeX, int SizeY, int *HandleArray, const void *AlphaData )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateDivGraph(  Width,  Height,  Pitch, GraphData,  AllNum,  XNum,  YNum,  SizeX,  SizeY, HandleArray, AlphaData ) ;
	DXFUNC_END
	return Result ;
}
extern int CreateDivGraphF( int Width, int Height, int Pitch, const void *GraphData, int AllNum, int XNum, int YNum, float SizeX, float SizeY, int *HandleArray, const void *AlphaData )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateDivGraphF(  Width,  Height,  Pitch, GraphData,  AllNum,  XNum,  YNum,  SizeX,  SizeY, HandleArray, AlphaData ) ;
	DXFUNC_END
	return Result ;
}
extern int ReCreateGraph( int Width, int Height, int Pitch, const void *GraphData, int GrHandle, const void *AlphaData  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReCreateGraph( Width, Height, Pitch, GraphData, GrHandle,  AlphaData  ) ;
	DXFUNC_END
	return Result ;
}

#ifndef DX_NON_SOFTIMAGE
extern	int		CreateBlendGraphFromSoftImage( int SIHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateBlendGraphFromSoftImage( SIHandle ) ;
	DXFUNC_END
	return Result ;
}
extern	int		CreateGraphFromSoftImage( int SIHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateGraphFromSoftImage(  SIHandle ) ;
	DXFUNC_END
	return Result ;
}

extern	int		CreateGraphFromRectSoftImage( int SIHandle, int x, int y, int SizeX, int SizeY )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateGraphFromRectSoftImage(  SIHandle,  x,  y,  SizeX,  SizeY ) ;
	DXFUNC_END
	return Result ;
}

extern	int		ReCreateGraphFromSoftImage( int SIHandle, int GrHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReCreateGraphFromSoftImage(  SIHandle,  GrHandle ) ;
	DXFUNC_END
	return Result ;
}

extern	int		ReCreateGraphFromRectSoftImage( int SIHandle, int x, int y, int SizeX, int SizeY, int GrHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReCreateGraphFromRectSoftImage(  SIHandle,  x,  y,  SizeX,  SizeY,  GrHandle ) ;
	DXFUNC_END
	return Result ;
}

extern	int		CreateDivGraphFromSoftImage( int SIHandle, int AllNum, int XNum, int YNum, int SizeX, int SizeY, int *HandleArray )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateDivGraphFromSoftImage(  SIHandle,  AllNum,  XNum,  YNum,  SizeX,  SizeY, HandleArray ) ;
	DXFUNC_END
	return Result ;
}

extern	int		CreateDivGraphFFromSoftImage( int SIHandle, int AllNum, int XNum, int YNum, float SizeX, float SizeY, int *HandleArray )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateDivGraphFFromSoftImage(  SIHandle,  AllNum,  XNum,  YNum,  SizeX,  SizeY, HandleArray ) ;
	DXFUNC_END
	return Result ;
}

#endif // DX_NON_SOFTIMAGE

extern	int		CreateGraphFromBaseImage( const BASEIMAGE *BaseImage )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateGraphFromBaseImage( BaseImage );
	DXFUNC_END
	return Result ;
}
extern	int		CreateGraphFromRectBaseImage( const BASEIMAGE *BaseImage, int x, int y, int SizeX, int SizeY )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateGraphFromRectBaseImage( BaseImage,  x,  y,  SizeX,  SizeY );
	DXFUNC_END
	return Result ;
}
extern	int		ReCreateGraphFromBaseImage( const BASEIMAGE *BaseImage, int GrHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReCreateGraphFromBaseImage( BaseImage, GrHandle );
	DXFUNC_END
	return Result ;
}
extern	int		ReCreateGraphFromRectBaseImage( const BASEIMAGE *BaseImage, int x, int y, int SizeX, int SizeY, int GrHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReCreateGraphFromRectBaseImage( BaseImage,  x,  y,  SizeX,  SizeY,  GrHandle ) ;
	DXFUNC_END
	return Result ;
}

extern	int		CreateDivGraphFromBaseImage( BASEIMAGE *BaseImage, int AllNum, int XNum, int YNum, int SizeX, int SizeY, int *HandleArray )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateDivGraphFromBaseImage( BaseImage,  AllNum,  XNum,  YNum,  SizeX,  SizeY, HandleArray );
	DXFUNC_END
	return Result ;
}
extern	int		CreateDivGraphFFromBaseImage( BASEIMAGE *BaseImage, int AllNum, int XNum, int YNum, float SizeX, float SizeY, int *HandleArray )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateDivGraphFFromBaseImage( BaseImage,  AllNum,  XNum,  YNum,  SizeX,  SizeY, HandleArray );
	DXFUNC_END
	return Result ;
}
extern int ReloadGraph( const TCHAR *FileName, int GrHandle, int ReverseFlag  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReloadGraph( FileName, GrHandle,  ReverseFlag  ) ;
	DXFUNC_END
	return Result ;
}
extern int ReloadGraphWithStrLen( const TCHAR *FileName, size_t FileNameLength, int GrHandle, int ReverseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReloadGraphWithStrLen( FileName, FileNameLength, GrHandle, ReverseFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int ReloadDivGraph( const TCHAR *FileName, int AllNum, int XNum, int YNum, int XSize, int YSize, const int *HandleArray, int ReverseFlag  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReloadDivGraph( FileName, AllNum,  XNum,  YNum,  XSize, YSize, HandleArray,  ReverseFlag  ) ;
	DXFUNC_END
	return Result ;
}
extern int ReloadDivGraphWithStrLen( const TCHAR *FileName, size_t FileNameLength, int AllNum, int XNum, int YNum, int   XSize, int   YSize, const int *HandleArray, int ReverseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReloadDivGraphWithStrLen( FileName, FileNameLength, AllNum, XNum, YNum, XSize, YSize, HandleArray, ReverseFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int ReloadDivGraphF( const TCHAR *FileName, int AllNum, int XNum, int YNum, float XSize, float YSize, const int *HandleArray, int ReverseFlag  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReloadDivGraphF( FileName, AllNum,  XNum,  YNum,  XSize, YSize, HandleArray,  ReverseFlag  ) ;
	DXFUNC_END
	return Result ;
}
extern int ReloadDivGraphFWithStrLen( const TCHAR *FileName, size_t FileNameLength, int AllNum, int XNum, int YNum, float XSize, float YSize, const int *HandleArray, int ReverseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReloadDivGraphFWithStrLen( FileName, FileNameLength, AllNum, XNum, YNum, XSize, YSize, HandleArray, ReverseFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int ReloadReverseGraph( const TCHAR *FileName, int GrHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReloadReverseGraph( FileName, GrHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int ReloadReverseGraphWithStrLen( const TCHAR *FileName, size_t FileNameLength, int GrHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReloadReverseGraphWithStrLen( FileName, FileNameLength, GrHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int ReloadReverseDivGraph( const TCHAR *FileName, int AllNum, int XNum, int YNum, int XSize, int YSize, const int *HandleArray )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReloadReverseDivGraph( FileName, AllNum,  XNum,  YNum,  XSize,  YSize, HandleArray ) ;
	DXFUNC_END
	return Result ;
}
extern int ReloadReverseDivGraphWithStrLen(	const TCHAR *FileName, size_t FileNameLength, int AllNum, int XNum, int YNum, int   XSize, int   YSize, const int *HandleArray )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReloadReverseDivGraphWithStrLen( FileName, FileNameLength, AllNum, XNum, YNum, XSize, YSize, HandleArray ) ;
	DXFUNC_END
	return Result ;
}
extern int ReloadReverseDivGraphF( const TCHAR *FileName, int AllNum, int XNum, int YNum, float XSize, float YSize, const int *HandleArray )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReloadReverseDivGraphF( FileName, AllNum,  XNum,  YNum,  XSize,  YSize, HandleArray ) ;
	DXFUNC_END
	return Result ;
}
extern int ReloadReverseDivGraphFWithStrLen( const TCHAR *FileName, size_t FileNameLength, int AllNum, int XNum, int YNum, float XSize, float YSize, const int *HandleArray )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReloadReverseDivGraphFWithStrLen( FileName, FileNameLength, AllNum, XNum, YNum, XSize, YSize, HandleArray ) ;
	DXFUNC_END
	return Result ;
}

extern	int		DrawBaseImage( int x, int y, BASEIMAGE *BaseImage )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawBaseImage( x, y, BaseImage );
	DXFUNC_END
	return Result ;
}

extern	int SetCameraNearFar( float Near, float Far )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetCameraNearFar(  Near,  Far );
	DXFUNC_END
	return Result ;
}
extern	int SetCameraNearFarD( double Near, double Far )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetCameraNearFarD(  Near,  Far );
	DXFUNC_END
	return Result ;
}

extern	int	SetCameraPositionAndTarget_UpVecY( VECTOR Position, VECTOR Target )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetCameraPositionAndTarget_UpVecY(  Position,  Target );
	DXFUNC_END
	return Result ;
}
extern	int	SetCameraPositionAndTarget_UpVecYD( VECTOR_D Position, VECTOR_D Target )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetCameraPositionAndTarget_UpVecYD(  Position,  Target );
	DXFUNC_END
	return Result ;
}

extern	int	SetCameraPositionAndTargetAndUpVec( VECTOR Position, VECTOR Target, VECTOR Up )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetCameraPositionAndTargetAndUpVec(  Position,  Target,  Up );
	DXFUNC_END
	return Result ;
}
extern	int	SetCameraPositionAndTargetAndUpVecD( VECTOR_D Position, VECTOR_D Target, VECTOR_D Up )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetCameraPositionAndTargetAndUpVecD(  Position,  Target,  Up );
	DXFUNC_END
	return Result ;
}

extern	int	SetCameraPositionAndAngle( VECTOR Position, float VRotate, float HRotate, float TRotate )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetCameraPositionAndAngle(  Position,  VRotate,  HRotate,  TRotate );
	DXFUNC_END
	return Result ;
}
extern	int	SetCameraPositionAndAngleD( VECTOR_D Position, double VRotate, double HRotate, double TRotate )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetCameraPositionAndAngleD(  Position,  VRotate,  HRotate,  TRotate );
	DXFUNC_END
	return Result ;
}

extern	int	SetCameraViewMatrix( MATRIX ViewMatrix )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetCameraViewMatrix( ViewMatrix );
	DXFUNC_END
	return Result ;
}
extern	int	SetCameraViewMatrixD( MATRIX_D ViewMatrix )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetCameraViewMatrixD( ViewMatrix );
	DXFUNC_END
	return Result ;
}

extern	int SetCameraScreenCenter( float x, float y )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetCameraScreenCenter(  x,  y );
	DXFUNC_END
	return Result ;
}

extern	int SetCameraScreenCenterD( double x, double y )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetCameraScreenCenterD(  x,  y );
	DXFUNC_END
	return Result ;
}

extern	int	SetupCamera_Perspective( float Fov )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetupCamera_Perspective( Fov );
	DXFUNC_END
	return Result ;
}
extern	int	SetupCamera_PerspectiveD( double Fov )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetupCamera_PerspectiveD( Fov );
	DXFUNC_END
	return Result ;
}

extern	int	SetupCamera_Ortho( float Size )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetupCamera_Ortho( Size );
	DXFUNC_END
	return Result ;
}
extern	int	SetupCamera_OrthoD( double Size )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetupCamera_OrthoD( Size );
	DXFUNC_END
	return Result ;
}

extern	int	SetupCamera_ProjectionMatrix( MATRIX ProjectionMatrix )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetupCamera_ProjectionMatrix(  ProjectionMatrix );
	DXFUNC_END
	return Result ;
}
extern	int	SetupCamera_ProjectionMatrixD( MATRIX_D ProjectionMatrix )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetupCamera_ProjectionMatrixD(  ProjectionMatrix );
	DXFUNC_END
	return Result ;
}

extern	int	SetCameraDotAspect( float DotAspect )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetCameraDotAspect( DotAspect );
	DXFUNC_END
	return Result ;
}
extern	int	SetCameraDotAspectD( double DotAspect )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetCameraDotAspectD( DotAspect );
	DXFUNC_END
	return Result ;
}

extern	int CheckCameraViewClip( VECTOR CheckPos )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CheckCameraViewClip( CheckPos ) ;
	DXFUNC_END
	return Result ;
}
extern	int CheckCameraViewClipD( VECTOR_D CheckPos )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CheckCameraViewClipD( CheckPos ) ;
	DXFUNC_END
	return Result ;
}

extern	int CheckCameraViewClip_Dir( VECTOR CheckPos )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CheckCameraViewClip_Dir( CheckPos ) ;
	DXFUNC_END
	return Result ;
}
extern	int CheckCameraViewClip_DirD( VECTOR_D CheckPos )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CheckCameraViewClip_DirD( CheckPos ) ;
	DXFUNC_END
	return Result ;
}

extern	int	CheckCameraViewClip_Box( VECTOR BoxPos1, VECTOR BoxPos2 )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CheckCameraViewClip_Box( BoxPos1, BoxPos2 ) ;
	DXFUNC_END
	return Result ;
}
extern	int	CheckCameraViewClip_BoxD( VECTOR_D BoxPos1, VECTOR_D BoxPos2 )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CheckCameraViewClip_BoxD( BoxPos1, BoxPos2 ) ;
	DXFUNC_END
	return Result ;
}

extern	float GetCameraNear( void )
{
	float Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraNear( );
	DXFUNC_END
	return Result ;
}

extern	double GetCameraNearD( void )
{
	double Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraNearD( );
	DXFUNC_END
	return Result ;
}

extern	float GetCameraFar( void )
{
	float Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraFar( );
	DXFUNC_END
	return Result ;
}

extern	double GetCameraFarD( void )
{
	double Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraFarD( );
	DXFUNC_END
	return Result ;
}

extern	VECTOR GetCameraPosition( void )
{
	VECTOR Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraPosition();
	DXFUNC_END
	return Result ;
}

extern	VECTOR_D GetCameraPositionD( void )
{
	VECTOR_D Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraPositionD();
	DXFUNC_END
	return Result ;
}

extern	VECTOR GetCameraTarget( void )
{
	VECTOR Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraTarget();
	DXFUNC_END
	return Result ;
}

extern	VECTOR_D GetCameraTargetD( void )
{
	VECTOR_D Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraTargetD();
	DXFUNC_END
	return Result ;
}

extern	VECTOR GetCameraUpVector( void )
{
	VECTOR Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraUpVector( );
	DXFUNC_END
	return Result ;
}

extern	VECTOR_D GetCameraUpVectorD( void )
{
	VECTOR_D Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraUpVectorD( );
	DXFUNC_END
	return Result ;
}

extern	VECTOR GetCameraDownVector( void )
{
	VECTOR Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraDownVector( );
	DXFUNC_END
	return Result ;
}

extern	VECTOR_D GetCameraDownVectorD( void )
{
	VECTOR_D Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraDownVectorD( );
	DXFUNC_END
	return Result ;
}

extern	VECTOR GetCameraRightVector( void )
{
	VECTOR Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraRightVector( );
	DXFUNC_END
	return Result ;
}

extern	VECTOR_D GetCameraRightVectorD( void )
{
	VECTOR_D Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraRightVectorD( );
	DXFUNC_END
	return Result ;
}

extern	VECTOR GetCameraLeftVector( void )
{
	VECTOR Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraLeftVector( );
	DXFUNC_END
	return Result ;
}

extern	VECTOR_D GetCameraLeftVectorD( void )
{
	VECTOR_D Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraLeftVectorD( );
	DXFUNC_END
	return Result ;
}

extern	VECTOR GetCameraFrontVector( void )
{
	VECTOR Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraFrontVector( );
	DXFUNC_END
	return Result ;
}

extern	VECTOR_D GetCameraFrontVectorD( void )
{
	VECTOR_D Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraFrontVectorD( );
	DXFUNC_END
	return Result ;
}

extern	VECTOR GetCameraBackVector( void )
{
	VECTOR Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraBackVector( );
	DXFUNC_END
	return Result ;
}

extern	VECTOR_D GetCameraBackVectorD( void )
{
	VECTOR_D Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraBackVectorD( );
	DXFUNC_END
	return Result ;
}

extern	float GetCameraAngleHRotate( void )
{
	float Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraAngleHRotate( );
	DXFUNC_END
	return Result ;
}

extern	double GetCameraAngleHRotateD( void )
{
	double Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraAngleHRotateD( );
	DXFUNC_END
	return Result ;
}

extern	float GetCameraAngleVRotate( void )
{
	float Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraAngleVRotate( );
	DXFUNC_END
	return Result ;
}

extern	double GetCameraAngleVRotateD( void )
{
	double Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraAngleVRotateD( );
	DXFUNC_END
	return Result ;
}

extern	float GetCameraAngleTRotate( void )
{
	float Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraAngleTRotate( );
	DXFUNC_END
	return Result ;
}

extern	double GetCameraAngleTRotateD( void )
{
	double Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraAngleTRotateD( );
	DXFUNC_END
	return Result ;
}

extern	MATRIX GetCameraViewMatrix( void )
{
	MATRIX Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraViewMatrix( );
	DXFUNC_END
	return Result ;
}

extern	MATRIX_D GetCameraViewMatrixD( void )
{
	MATRIX_D Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraViewMatrixD( );
	DXFUNC_END
	return Result ;
}

extern	MATRIX GetCameraBillboardMatrix( void )
{
	MATRIX Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraBillboardMatrix();
	DXFUNC_END
	return Result ;
}

extern	MATRIX_D GetCameraBillboardMatrixD( void )
{
	MATRIX_D Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraBillboardMatrixD();
	DXFUNC_END
	return Result ;
}

extern	int GetCameraScreenCenter( float  *x, float  *y )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraScreenCenter( x, y );
	DXFUNC_END
	return Result ;
}

extern	int GetCameraScreenCenterD( double  *x, double  *y )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraScreenCenterD( x, y );
	DXFUNC_END
	return Result ;
}

extern	float GetCameraFov( void )
{
	float Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraFov( );
	DXFUNC_END
	return Result ;
}

extern	double GetCameraFovD( void )
{
	double Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraFovD( );
	DXFUNC_END
	return Result ;
}

extern	float GetCameraSize( void )
{
	float Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraSize( );
	DXFUNC_END
	return Result ;
}

extern	double GetCameraSizeD( void )
{
	double Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraSizeD( );
	DXFUNC_END
	return Result ;
}

extern	MATRIX GetCameraProjectionMatrix( void )
{
	MATRIX Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraProjectionMatrix( );
	DXFUNC_END
	return Result ;
}

extern	MATRIX_D GetCameraProjectionMatrixD( void )
{
	MATRIX_D Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraProjectionMatrixD( );
	DXFUNC_END
	return Result ;
}

extern	float GetCameraDotAspect( void )
{
	float Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraDotAspect( );
	DXFUNC_END
	return Result ;
}

extern	double GetCameraDotAspectD( void )
{
	double Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraDotAspectD( );
	DXFUNC_END
	return Result ;
}

extern	MATRIX GetCameraViewportMatrix( void )
{
	MATRIX Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraViewportMatrix();
	DXFUNC_END
	return Result ;
}

extern	MATRIX_D GetCameraViewportMatrixD( void )
{
	MATRIX_D Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraViewportMatrixD();
	DXFUNC_END
	return Result ;
}

extern	MATRIX GetCameraAPIViewportMatrix( void )
{
	MATRIX Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraAPIViewportMatrix();
	DXFUNC_END
	return Result ;
}

extern	MATRIX_D GetCameraAPIViewportMatrixD( void )
{
	MATRIX_D Result ;
	DXFUNC_BEGIN
	Result = NS_GetCameraAPIViewportMatrixD();
	DXFUNC_END
	return Result ;
}



extern	int			SetUseLighting( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseLighting( Flag ) ;
	DXFUNC_END
	return Result ;
}

extern	int			SetMaterialUseVertDifColor( int UseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetMaterialUseVertDifColor( UseFlag ) ;
	DXFUNC_END
	return Result ;
}

extern	int			SetMaterialUseVertSpcColor( int UseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetMaterialUseVertSpcColor( UseFlag ) ;
	DXFUNC_END
	return Result ;
}

extern	int			SetMaterialParam( MATERIALPARAM Material )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetMaterialParam( Material ) ;
	DXFUNC_END
	return Result ;
}

extern	int			SetUseSpecular( int UseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseSpecular( UseFlag ) ;
	DXFUNC_END
	return Result ;
}

extern	int			SetGlobalAmbientLight( COLOR_F Color )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetGlobalAmbientLight( Color ) ;
	DXFUNC_END
	return Result ;
}

extern	int			ChangeLightTypeDir( VECTOR Direction )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ChangeLightTypeDir( Direction );
	DXFUNC_END
	return Result ;
}

extern	int			ChangeLightTypeSpot( VECTOR Position, VECTOR Direction, float OutAngle, float InAngle, float Range, float Atten0, float Atten1, float Atten2 )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ChangeLightTypeSpot(  Position,  Direction,  OutAngle,  InAngle,  Range,  Atten0,  Atten1,  Atten2 );
	DXFUNC_END
	return Result ;
}

extern	int			ChangeLightTypePoint( VECTOR Position, float Range,  float Atten0, float Atten1, float Atten2 )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ChangeLightTypePoint(  Position, Range, Atten0,  Atten1,  Atten2 );
	DXFUNC_END
	return Result ;
}

extern	int			SetLightEnable( int EnableFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetLightEnable( EnableFlag );
	DXFUNC_END
	return Result ;
}

extern	int			SetLightDifColor( COLOR_F Color )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetLightDifColor( Color );
	DXFUNC_END
	return Result ;
}

extern	int			SetLightSpcColor( COLOR_F Color )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetLightSpcColor( Color );
	DXFUNC_END
	return Result ;
}

extern	int			SetLightAmbColor( COLOR_F Color )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetLightAmbColor( Color );
	DXFUNC_END
	return Result ;
}

extern	int			SetLightDirection( VECTOR Direction )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetLightDirection( Direction );
	DXFUNC_END
	return Result ;
}

extern	int			SetLightPosition( VECTOR Position )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetLightPosition(  Position );
	DXFUNC_END
	return Result ;
}

extern	int			SetLightRangeAtten( float Range, float Atten0, float Atten1, float Atten2 )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetLightRangeAtten(  Range,  Atten0,  Atten1,  Atten2 );
	DXFUNC_END
	return Result ;
}

extern	int			SetLightAngle( float OutAngle, float InAngle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetLightAngle(  OutAngle,  InAngle );
	DXFUNC_END
	return Result ;
}
extern	int			SetLightUseShadowMap( int SmSlotIndex, int UseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetLightUseShadowMap( SmSlotIndex, UseFlag ) ;
	DXFUNC_END
	return Result ;
}

extern	int GetLightType( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetLightType( );
	DXFUNC_END
	return Result ;
}
extern	int	GetLightEnable( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetLightEnable( );
	DXFUNC_END
	return Result ;
}

extern COLOR_F GetLightDifColor( void )
{
	COLOR_F Result ;
	DXFUNC_BEGIN
	Result = NS_GetLightDifColor();
	DXFUNC_END
	return Result ;
}

extern COLOR_F GetLightSpcColor( void )
{
	COLOR_F Result ;
	DXFUNC_BEGIN
	Result = NS_GetLightSpcColor();
	DXFUNC_END
	return Result ;
}

extern COLOR_F GetLightAmbColor( void )
{
	COLOR_F Result ;
	DXFUNC_BEGIN
	Result = NS_GetLightAmbColor();
	DXFUNC_END
	return Result ;
}

extern	VECTOR GetLightDirection( void )
{
	VECTOR Result ;
	DXFUNC_BEGIN
	Result = NS_GetLightDirection( );
	DXFUNC_END
	return Result ;
}

extern	VECTOR GetLightPosition( void )
{
	VECTOR Result ;
	DXFUNC_BEGIN
	Result = NS_GetLightPosition( );
	DXFUNC_END
	return Result ;
}

extern	int GetLightRangeAtten( float *Range, float *Atten0, float *Atten1, float *Atten2 )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetLightRangeAtten( Range, Atten0, Atten1, Atten2 );
	DXFUNC_END
	return Result ;
}

extern	int GetLightAngle( float *OutAngle, float *InAngle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetLightAngle( OutAngle, InAngle /*, NULL */ );
	DXFUNC_END
	return Result ;
}

extern	int			CreateDirLightHandle( VECTOR Direction )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateDirLightHandle( Direction );
	DXFUNC_END
	return Result ;
}

extern	int			CreateSpotLightHandle( VECTOR Position, VECTOR Direction, float OutAngle, float InAngle, float Range, float Atten0, float Atten1, float Atten2 )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateSpotLightHandle(  Position,  Direction,  OutAngle,  InAngle/*,  1.0f*/,  Range,  Atten0,  Atten1,  Atten2 );
	DXFUNC_END
	return Result ;
}

extern	int			CreatePointLightHandle( VECTOR Position, float Range, float Atten0, float Atten1, float Atten2 )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreatePointLightHandle( Position, Range, Atten0,  Atten1,  Atten2 );
	DXFUNC_END
	return Result ;
}

extern	int			DeleteLightHandle( int LHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DeleteLightHandle( LHandle );
	DXFUNC_END
	return Result ;
}

extern	int			DeleteLightHandleAll( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DeleteLightHandleAll();
	DXFUNC_END
	return Result ;
}

extern	int			SetLightTypeHandle( int LHandle, int LightType )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetLightTypeHandle(  LHandle,  LightType ) ;
	DXFUNC_END
	return Result ;
}

extern	int			SetLightEnableHandle( int LHandle, int EnableFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetLightEnableHandle(  LHandle,  EnableFlag );
	DXFUNC_END
	return Result ;
}

extern	int			SetLightDifColorHandle( int LHandle, COLOR_F Color )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetLightDifColorHandle( LHandle, Color );
	DXFUNC_END
	return Result ;
}

extern	int			SetLightSpcColorHandle( int LHandle, COLOR_F Color )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetLightSpcColorHandle(  LHandle, Color );
	DXFUNC_END
	return Result ;
}

extern	int			SetLightAmbColorHandle( int LHandle, COLOR_F Color )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetLightAmbColorHandle(  LHandle, Color );
	DXFUNC_END
	return Result ;
}

extern	int			SetLightDirectionHandle( int LHandle, VECTOR Direction )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetLightDirectionHandle(  LHandle,  Direction );
	DXFUNC_END
	return Result ;
}

extern	int			SetLightPositionHandle( int LHandle, VECTOR Position )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetLightPositionHandle(  LHandle,  Position );
	DXFUNC_END
	return Result ;
}

extern	int			SetLightRangeAttenHandle( int LHandle, float Range, float Atten0, float Atten1, float Atten2 )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetLightRangeAttenHandle( LHandle,  Range,  Atten0,  Atten1,  Atten2 );
	DXFUNC_END
	return Result ;
}

extern	int			SetLightAngleHandle( int LHandle, float OutAngle, float InAngle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetLightAngleHandle(  LHandle,  OutAngle,  InAngle/*,  1.0f*/ );
	DXFUNC_END
	return Result ;
}
extern	int			SetLightUseShadowMapHandle(	int LHandle, int SmSlotIndex, int UseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetLightUseShadowMapHandle(	LHandle, SmSlotIndex, UseFlag ) ;
	DXFUNC_END
	return Result ;
}

extern	int			GetLightTypeHandle( int LHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetLightTypeHandle( LHandle );
	DXFUNC_END
	return Result ;
}

extern	int			GetLightEnableHandle( int LHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetLightEnableHandle(  LHandle );
	DXFUNC_END
	return Result ;
}

extern COLOR_F GetLightDifColorHandle( int LHandle )
{
	COLOR_F Result ;
	DXFUNC_BEGIN
	Result = NS_GetLightDifColorHandle( LHandle );
	DXFUNC_END
	return Result ;
}

extern COLOR_F GetLightSpcColorHandle( int LHandle )
{
	COLOR_F Result ;
	DXFUNC_BEGIN
	Result = NS_GetLightSpcColorHandle( LHandle );
	DXFUNC_END
	return Result ;
}

extern COLOR_F GetLightAmbColorHandle( int LHandle )
{
	COLOR_F Result ;
	DXFUNC_BEGIN
	Result = NS_GetLightAmbColorHandle( LHandle );
	DXFUNC_END
	return Result ;
}

extern	VECTOR		GetLightDirectionHandle( int LHandle )
{
	VECTOR Result ;
	DXFUNC_BEGIN
	Result = NS_GetLightDirectionHandle(  LHandle );
	DXFUNC_END
	return Result ;
}

extern	VECTOR		GetLightPositionHandle( int LHandle )
{
	VECTOR Result ;
	DXFUNC_BEGIN
	Result = NS_GetLightPositionHandle( LHandle );
	DXFUNC_END
	return Result ;
}

extern	int		GetLightRangeAttenHandle( int LHandle, float *Range, float *Atten0, float *Atten1, float *Atten2 )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetLightRangeAttenHandle( LHandle, Range, Atten0, Atten1, Atten2 );
	DXFUNC_END
	return Result ;
}

extern	int		GetLightAngleHandle( int LHandle, float *OutAngle, float *InAngle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetLightAngleHandle( LHandle, OutAngle, InAngle /*, NULL */ );
	DXFUNC_END
	return Result ;
}

extern	int		GetEnableLightHandleNum( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetEnableLightHandleNum();
	DXFUNC_END
	return Result ;
}

extern	int		GetEnableLightHandle( int Index )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetEnableLightHandle( Index );
	DXFUNC_END
	return Result ;
}


// グラフィック関係設定関数
extern int SetGraphColorBitDepth( int ColorBitDepth )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetGraphColorBitDepth( ColorBitDepth ) ;
	DXFUNC_END
	return Result ;
}
extern int GetGraphColorBitDepth( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetGraphColorBitDepth(  ) ;
	DXFUNC_END
	return Result ;
}
extern int SetCreateGraphColorBitDepth( int BitDepth )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetCreateGraphColorBitDepth( BitDepth ) ;
	DXFUNC_END
	return Result ;
}
extern int GetCreateGraphColorBitDepth( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetCreateGraphColorBitDepth(  ) ;
	DXFUNC_END
	return Result ;
}
extern int SetCreateGraphChannelBitDepth( int BitDepth )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetCreateGraphChannelBitDepth( BitDepth ) ;
	DXFUNC_END
	return Result ;
}
extern int GetCreateGraphChannelBitDepth( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetCreateGraphChannelBitDepth( ) ;
	DXFUNC_END
	return Result ;
}
extern int SetDrawValidGraphCreateFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetDrawValidGraphCreateFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawValidGraphCreateFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawValidGraphCreateFlag( ) ;
	DXFUNC_END
	return Result ;
}
extern int SetLeftUpColorIsTransColorFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetLeftUpColorIsTransColorFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUsePaletteGraphFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUsePaletteGraphFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetUseBlendGraphCreateFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetUseBlendGraphCreateFlag( ) ;
	DXFUNC_END
	return Result ;
}
extern int GetUseSystemMemGraphCreateFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetUseSystemMemGraphCreateFlag(  ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseBlendGraphCreateFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseBlendGraphCreateFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseAlphaTestGraphCreateFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseAlphaTestGraphCreateFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseAlphaTestFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseAlphaTestFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseNoBlendModeParam( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseNoBlendModeParam( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetDrawValidFlagOf3DGraph( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetDrawValidFlagOf3DGraph( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetUseAlphaTestFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetUseAlphaTestFlag(  ) ;
	DXFUNC_END
	return Result ;
}
extern int SetCubeMapTextureCreateFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetCubeMapTextureCreateFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetCubeMapTextureCreateFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetCubeMapTextureCreateFlag( ) ;
	DXFUNC_END
	return Result ;
}
extern int GetUseAlphaTestGraphCreateFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetUseAlphaTestGraphCreateFlag(  ) ;
	DXFUNC_END
	return Result ;
}
extern int SetDrawValidAlphaChannelGraphCreateFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetDrawValidAlphaChannelGraphCreateFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawValidAlphaChannelGraphCreateFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawValidAlphaChannelGraphCreateFlag() ;
	DXFUNC_END
	return Result ;
}
extern int SetDrawValidFloatTypeGraphCreateFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetDrawValidFloatTypeGraphCreateFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawValidFloatTypeGraphCreateFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawValidFloatTypeGraphCreateFlag() ;
	DXFUNC_END
	return Result ;
}
extern int SetDrawValidGraphCreateZBufferFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetDrawValidGraphCreateZBufferFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawValidGraphCreateZBufferFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawValidGraphCreateZBufferFlag() ;
	DXFUNC_END
	return Result ;
}
extern int SetCreateDrawValidGraphZBufferBitDepth( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetCreateDrawValidGraphZBufferBitDepth( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetCreateDrawValidGraphZBufferBitDepth( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetCreateDrawValidGraphZBufferBitDepth() ;
	DXFUNC_END
	return Result ;
}
extern int SetCreateDrawValidGraphMipLevels( int MipLevels )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetCreateDrawValidGraphMipLevels( MipLevels ) ;
	DXFUNC_END
	return Result ;
}
extern int GetCreateDrawValidGraphMipLevels( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetCreateDrawValidGraphMipLevels() ;
	DXFUNC_END
	return Result ;
}
extern int SetCreateDrawValidGraphChannelNum( int ChannelNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetCreateDrawValidGraphChannelNum( ChannelNum ) ;
	DXFUNC_END
	return Result ;
}
extern int GetCreateDrawValidGraphChannelNum( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetCreateDrawValidGraphChannelNum() ;
	DXFUNC_END
	return Result ;
}
extern int SetCreateDrawValidGraphMultiSample( int Samples, int Quality )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetCreateDrawValidGraphMultiSample( Samples, Quality ) ;
	DXFUNC_END
	return Result ;
}
extern int SetDrawValidMultiSample( int Samples, int Quality )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetDrawValidMultiSample(  Samples,  Quality ) ;
	DXFUNC_END
	return Result ;
}
extern int GetMultiSampleQuality( int Samples )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetMultiSampleQuality( Samples ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseZBufferFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseZBufferFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetWriteZBufferFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetWriteZBufferFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetZBufferCmpType( int CmpType )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetZBufferCmpType( CmpType ) ;
	DXFUNC_END
	return Result ;
}
extern int SetZBias( int Bias )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetZBias( Bias ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseZBuffer3D( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseZBuffer3D( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetWriteZBuffer3D( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetWriteZBuffer3D( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetZBufferCmpType3D( int CmpType /* DX_CMP_NEVER 等 */ )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetZBufferCmpType3D( CmpType ) ;
	DXFUNC_END
	return Result ;
}
extern int SetZBias3D( int Bias )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetZBias3D( Bias ) ;
	DXFUNC_END
	return Result ;
}
extern int SetDrawZ( float Z )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetDrawZ( Z ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseTransColor( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseTransColor( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseTransColorGraphCreateFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseTransColorGraphCreateFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseAlphaChannelGraphCreateFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseAlphaChannelGraphCreateFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseGraphAlphaChannel( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseGraphAlphaChannel( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetUseAlphaChannelGraphCreateFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetUseAlphaChannelGraphCreateFlag(  ) ;
	DXFUNC_END
	return Result ;
}
extern int GetUseGraphAlphaChannel( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetUseGraphAlphaChannel(  ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseNotManageTextureFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseNotManageTextureFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetTransColor( int Red, int Green, int Blue )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetTransColor( Red, Green, Blue ) ;
	DXFUNC_END
	return Result ;
}
extern int GetTransColor( int *Red, int *Green, int *Blue )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetTransColor( Red, Green, Blue ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawArea( RECT *Rect )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawArea( Rect ) ;
	DXFUNC_END
	return Result ;
}
extern int GetUseNotManageTextureFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetUseNotManageTextureFlag( ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUsePlatformTextureFormat( int PlatformTextureFormat )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUsePlatformTextureFormat( PlatformTextureFormat ) ;
	DXFUNC_END
	return Result ;
}
extern int GetUsePlatformTextureFormat(	void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetUsePlatformTextureFormat() ;
	DXFUNC_END
	return Result ;
}
extern int SetUseBasicGraphDraw3DDeviceMethodFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseBasicGraphDraw3DDeviceMethodFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseDisplayIndex( int Index )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseDisplayIndex( Index ) ;
	DXFUNC_END
	return Result ;
}
extern int SetWindowDrawRect( const RECT *DrawRect )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetWindowDrawRect( DrawRect ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawBlendMode( int *BlendMode, int *BlendParam )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawBlendMode( BlendMode, BlendParam ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawMode( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawMode(  ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawBright( int *Red, int *Green, int *Blue )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawBright( Red, Green, Blue ) ;
	DXFUNC_END
	return Result ;
}
extern int GetActiveGraph( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetActiveGraph(  ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseSetDrawScreenSettingReset( int UseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseSetDrawScreenSettingReset( UseFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetUseSetDrawScreenSettingReset( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetUseSetDrawScreenSettingReset( ) ;
	DXFUNC_END
	return Result ;
}
extern int GetTexFormatIndex( const IMAGEFORMATDESC *Format )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetTexFormatIndex( Format ) ;
	DXFUNC_END
	return Result ;
}
extern int SetWaitVSyncFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetWaitVSyncFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetWaitVSyncFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetWaitVSyncFlag(  ) ;
	DXFUNC_END
	return Result ;
}
extern int SetDefTransformMatrix( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetDefTransformMatrix(  ) ;
	DXFUNC_END
	return Result ;
}
extern int SetEmulation320x240( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetEmulation320x240( Flag ) ;
	DXFUNC_END
	return Result ;
}

extern int SetUseGraphBaseDataBackup( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseGraphBaseDataBackup( Flag ) ;
	DXFUNC_END
	return Result ;
}

extern int GetUseGraphBaseDataBackup( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetUseGraphBaseDataBackup() ;
	DXFUNC_END
	return Result ;
}

#ifndef DX_NON_MASK

// マスク関係
extern int CreateMaskScreen( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateMaskScreen(  ) ;
	DXFUNC_END
	return Result ;
}
extern int DeleteMaskScreen( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DeleteMaskScreen(  ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawMaskToDirectData( int x, int y, int Width, int Height, const void *MaskData , int TransMode )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawMaskToDirectData( x, y, Width, Height, MaskData , TransMode ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawFillMaskToDirectData( int x1, int y1, int x2, int y2,  int Width, int Height, const void *MaskData )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawFillMaskToDirectData( x1, y1, x2, y2,  Width, Height, MaskData ) ;
	DXFUNC_END
	return Result ;
}

extern int SetUseMaskScreenFlag( int ValidFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseMaskScreenFlag( ValidFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetUseMaskScreenFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetUseMaskScreenFlag(  ) ;
	DXFUNC_END
	return Result ;
}
extern int FillMaskScreen( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_FillMaskScreen( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetMaskScreenGraph( int GraphHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetMaskScreenGraph( GraphHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetMaskScreenGraphUseChannel( int UseChannel )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetMaskScreenGraphUseChannel( UseChannel ) ;
	DXFUNC_END
	return Result ;
}

extern int InitMask( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_InitMask(  ) ;
	DXFUNC_END
	return Result ;
}
extern int MakeMask( int Width, int Height )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MakeMask( Width, Height ) ;
	DXFUNC_END
	return Result ;
}
extern int GetMaskSize( int *WidthBuf, int *HeightBuf, int MaskHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetMaskSize( WidthBuf, HeightBuf, MaskHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetDataToMask( int Width, int Height, const void *MaskData, int MaskHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetDataToMask( Width, Height, MaskData, MaskHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int DeleteMask( int MaskHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DeleteMask( MaskHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int BmpBltToMask( HBITMAP Bmp, int BmpPointX, int BmpPointY, int MaskHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_BmpBltToMask( Bmp, BmpPointX, BmpPointY, MaskHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GraphImageBltToMask( const BASEIMAGE *BaseImage, int ImageX, int ImageY, int MaskHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GraphImageBltToMask( BaseImage, ImageX, ImageY, MaskHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadMask( const TCHAR *FileName )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadMask( FileName ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadMaskWithStrLen( const TCHAR *FileName, size_t FileNameLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadMaskWithStrLen( FileName, FileNameLength ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadDivMask( const TCHAR *FileName, int AllNum, int XNum, int YNum, int XSize, int YSize, int *HandleArray )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadDivMask( FileName, AllNum, XNum, YNum, XSize, YSize, HandleArray ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadDivMaskWithStrLen( const TCHAR *FileName, size_t FileNameLength, int AllNum, int XNum, int YNum, int XSize, int YSize, int *HandleArray )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadDivMaskWithStrLen( FileName, FileNameLength, AllNum, XNum, YNum, XSize, YSize, HandleArray ) ;
	DXFUNC_END
	return Result ;
}
extern int CreateMaskFromMem( const void *FileImage, int FileImageSize )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateMaskFromMem( FileImage, FileImageSize ) ;
	DXFUNC_END
	return Result ;
}
extern int CreateDivMaskFromMem( const void *FileImage, int FileImageSize, int AllNum, int XNum, int YNum, int XSize, int YSize, int *HandleArray )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateDivMaskFromMem( FileImage, FileImageSize,  AllNum,  XNum,  YNum,  XSize,  YSize, HandleArray ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawMask( int x, int y, int MaskHandle, int TransMode )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawMask( x, y, MaskHandle, TransMode ) ;
	DXFUNC_END
	return Result ;
}
#ifndef DX_NON_FONT
extern int DrawFormatStringMask( int x, int y, int Flag, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	DXFUNC_BEGIN
	Result = NS_DrawStringMask( x, y, Flag, String ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawFormatStringMaskToHandle( int x, int y, int Flag, int FontHandle, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	DXFUNC_BEGIN
	Result = NS_DrawStringMaskToHandle( x, y, Flag, FontHandle, String ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawStringMask( int x, int y, int Flag, const TCHAR *String )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawStringMask( x, y, Flag, String ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawNStringMask( int x, int y, int Flag, const TCHAR *String, size_t StringLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawNStringMask( x, y, Flag, String, StringLength ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawStringMaskToHandle( int x, int y, int Flag, int FontHandle, const TCHAR *String )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawStringMaskToHandle( x, y, Flag, FontHandle, String ) ;
	DXFUNC_END
	return Result ;
}
extern int DrawNStringMaskToHandle( int x, int y, int Flag, int FontHandle, const TCHAR *String, size_t StringLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawNStringMaskToHandle( x, y, Flag, FontHandle, String, StringLength ) ;
	DXFUNC_END
	return Result ;
}
#endif // DX_NON_FONT
extern int DrawFillMask( int x1, int y1, int x2, int y2, int MaskHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawFillMask( x1, y1, x2, y2, MaskHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetMaskReverseEffectFlag( int ReverseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetMaskReverseEffectFlag( ReverseFlag ) ;
	DXFUNC_END
	return Result ;
}

extern int GetMaskScreenData( int x1, int y1, int x2, int y2, int MaskHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetMaskScreenData( x1, y1, x2, y2, MaskHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetMaskUseFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetMaskUseFlag(  ) ;
	DXFUNC_END
	return Result ;
}

#endif


#ifndef DX_NON_MOVIE

extern int SetMovieRightImageAlphaFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetMovieRightImageAlphaFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}

extern int SetMovieColorA8R8G8B8Flag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetMovieColorA8R8G8B8Flag( Flag ) ;
	DXFUNC_END
	return Result ;
}

extern	int	SetMovieUseYUVFormatSurfaceFlag(	int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetMovieUseYUVFormatSurfaceFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}

#endif // DX_NON_MOVIE



#ifndef DX_NON_FONT

// フォント、文字列描画関係関数
extern int EnumFontName( TCHAR *NameBuffer, int NameBufferNum, int JapanOnlyFlag  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_EnumFontName( NameBuffer, NameBufferNum, JapanOnlyFlag  ) ;
	DXFUNC_END
	return Result ;
}
extern int EnumFontNameEx( TCHAR *NameBuffer, int NameBufferNum, int CharSet )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_EnumFontNameEx( NameBuffer,  NameBufferNum,  CharSet ) ;
	DXFUNC_END
	return Result ;
}
extern int EnumFontNameEx2( TCHAR *NameBuffer, int NameBufferNum, const TCHAR *EnumFontName, int CharSet )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_EnumFontNameEx2( NameBuffer,  NameBufferNum, EnumFontName, CharSet ) ;
	DXFUNC_END
	return Result ;
}
extern int EnumFontNameEx2WithStrLen( TCHAR *NameBuffer, int NameBufferNum, const TCHAR *EnumFontName, size_t EnumFontNameLength, int CharSet )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_EnumFontNameEx2WithStrLen( NameBuffer, NameBufferNum, EnumFontName, EnumFontNameLength, CharSet ) ;
	DXFUNC_END
	return Result ;
}
extern int CheckFontName( const TCHAR *FontName, int CharSet )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CheckFontName( FontName, CharSet ) ;
	DXFUNC_END
	return Result ;
}
extern int CheckFontNameWithStrLen( const TCHAR *FontName, size_t FontNameLength, int CharSet )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CheckFontNameWithStrLen( FontName, FontNameLength, CharSet ) ;
	DXFUNC_END
	return Result ;
}
extern int InitFontToHandle( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_InitFontToHandle(  ) ;
	DXFUNC_END
	return Result ;
}
extern int CreateFontToHandle( const TCHAR *FontName, int Size, int Thick, int FontType , int CharSet , int EdgeSize , int Italic, int Handle  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateFontToHandle( FontName, Size, Thick, FontType , CharSet , EdgeSize , Italic, Handle  ) ;
	DXFUNC_END
	return Result ;
}
extern int CreateFontToHandleWithStrLen( const TCHAR *FontName, size_t FontNameLength, int Size, int Thick, int FontType , int CharSet , int EdgeSize , int Italic , int Handle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateFontToHandleWithStrLen( FontName, FontNameLength, Size, Thick, FontType , CharSet , EdgeSize , Italic , Handle ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadFontDataToHandle( const TCHAR *FileName, int EdgeSize )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadFontDataToHandle( FileName, EdgeSize ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadFontDataToHandleWithStrLen( const TCHAR *FileName, size_t FileNameLength, int EdgeSize )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadFontDataToHandleWithStrLen( FileName, FileNameLength, EdgeSize ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadFontDataFromMemToHandle(	const void *FontDataImage, int FontDataImageSize, int EdgeSize )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadFontDataFromMemToHandle( FontDataImage, FontDataImageSize, EdgeSize ) ;
	DXFUNC_END
	return Result ;
}
extern int SetFontSpaceToHandle( int Pixel, int FontHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetFontSpaceToHandle( Pixel, FontHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetFontLineSpaceToHandle( int Pixel, int FontHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetFontLineSpaceToHandle( Pixel, FontHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetFontCharCodeFormatToHandle( int CharCodeFormat /* DX_CHARCODEFORMAT_SHIFTJIS 等 */ , int FontHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetFontCharCodeFormatToHandle( CharCodeFormat, FontHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetDefaultFontState( const TCHAR *FontName, int Size, int Thick, int FontType, int CharSet, int EdgeSize, int Italic )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetDefaultFontState( FontName, Size, Thick, FontType, CharSet, EdgeSize, Italic ) ;
	DXFUNC_END
	return Result ;
}
extern int SetDefaultFontStateWithStrLen( const TCHAR *FontName, size_t FontNameLength, int Size, int Thick, int FontType , int CharSet , int EdgeSize , int Italic )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetDefaultFontStateWithStrLen( FontName, FontNameLength, Size, Thick, FontType , CharSet , EdgeSize , Italic ) ;
	DXFUNC_END
	return Result ;
}
extern int DeleteFontToHandle( int FontHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DeleteFontToHandle( FontHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetFontLostFlag( int FontHandle, int *LostFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetFontLostFlag( FontHandle, LostFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int AddFontImageToHandle( int FontHandle, const TCHAR *Char, int GrHandle, int DrawX, int DrawY, int AddX )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_AddFontImageToHandle( FontHandle, Char, GrHandle, DrawX, DrawY, AddX ) ;
	DXFUNC_END
	return Result ;
}
extern int AddFontImageToHandleWithStrLen( int FontHandle, const TCHAR *Char, size_t CharLength, int GrHandle, int DrawX, int DrawY, int AddX )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_AddFontImageToHandleWithStrLen( FontHandle, Char, CharLength, GrHandle, DrawX, DrawY, AddX ) ;
	DXFUNC_END
	return Result ;
}
extern int SubFontImageToHandle( int FontHandle, const TCHAR *Char )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SubFontImageToHandle( FontHandle, Char ) ;
	DXFUNC_END
	return Result ;
}
extern int SubFontImageToHandleWithStrLen( int FontHandle, const TCHAR *Char, size_t CharLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SubFontImageToHandleWithStrLen( FontHandle, Char, CharLength ) ;
	DXFUNC_END
	return Result ;
}
extern int AddSubstitutionFontToHandle( int FontHandle, int SubstitutionFontHandle, int DrawX, int DrawY )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_AddSubstitutionFontToHandle( FontHandle, SubstitutionFontHandle, DrawX, DrawY ) ;
	DXFUNC_END
	return Result ;
}
extern int SubSubstitutionFontToHandle( int FontHandle, int SubstitutionFontHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SubSubstitutionFontToHandle( FontHandle, SubstitutionFontHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetFontSize( int FontSize )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetFontSize( FontSize ) ;
	DXFUNC_END
	return Result ;
}
extern int SetFontThickness( int ThickPal )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetFontThickness( ThickPal ) ;
	DXFUNC_END
	return Result ;
}
extern int SetFontSpace( int Pixel )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetFontSpace( Pixel ) ;
	DXFUNC_END
	return Result ;
}
extern int GetFontSpace( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetFontSpace() ;
	DXFUNC_END
	return Result ;
}
extern int SetFontLineSpace( int Pixel )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetFontLineSpace( Pixel ) ;
	DXFUNC_END
	return Result ;
}
extern int GetFontLineSpace( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetFontLineSpace() ;
	DXFUNC_END
	return Result ;
}
extern int SetFontCharCodeFormat( int CharCodeFormat /* DX_CHARCODEFORMAT_SHIFTJIS 等 */ )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetFontCharCodeFormat( CharCodeFormat /* DX_CHARCODEFORMAT_SHIFTJIS 等 */ ) ;
	DXFUNC_END
	return Result ;
}
extern int SetFontCacheToTextureFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetFontCacheToTextureFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetFontChacheToTextureFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetFontChacheToTextureFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int SetFontCacheCharNum( int CharNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetFontCacheCharNum( CharNum ) ;
	DXFUNC_END
	return Result ;
}
extern int ChangeFont( const TCHAR *FontName, int CharSet /*DX_CHARSET_SHFTJIS*/ )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ChangeFont( FontName, CharSet /*DX_CHARSET_SHFTJIS*/ ) ;
	DXFUNC_END
	return Result ;
}
extern int ChangeFontWithStrLen( const TCHAR *FontName, size_t FontNameLength, int CharSet )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ChangeFontWithStrLen( FontName, FontNameLength, CharSet ) ;
	DXFUNC_END
	return Result ;
}
extern int ChangeFontType( int FontType )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ChangeFontType( FontType ) ;
	DXFUNC_END
	return Result ;
}
extern const TCHAR *GetFontName( void )
{
	const TCHAR * Result ;
	DXFUNC_BEGIN
	Result = NS_GetFontName() ;
	DXFUNC_END
	return Result ;
}
extern int FontCacheStringDrawToHandle( int x, int y, const TCHAR *StrData, unsigned int Color, unsigned int EdgeColor,
													BASEIMAGE *DestImage, const RECT *ClipRect/*NULL 可*/, int FontHandle,
													int VerticalFlag , SIZE *DrawSizeP  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_FontCacheStringDrawToHandle(  x,  y, StrData,  Color,  EdgeColor,
													DestImage, ClipRect,  FontHandle,
													 VerticalFlag , DrawSizeP ) ;
	DXFUNC_END
	return Result ;
}
extern int FontCacheStringDrawToHandleWithStrLen( int x, int y, const TCHAR *StrData, size_t StrDataLength, unsigned int Color, unsigned int EdgeColor, BASEIMAGE *DestImage, const RECT *ClipRect, int FontHandle, int VerticalFlag , SIZE *DrawSizeP )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_FontCacheStringDrawToHandleWithStrLen( x, y, StrData, StrDataLength, Color, EdgeColor, DestImage, ClipRect, FontHandle, VerticalFlag , DrawSizeP ) ;
	DXFUNC_END
	return Result ;
}
extern int FontBaseImageBlt( 
						int x, int y, const TCHAR *StrData,
						BASEIMAGE *DestImage, BASEIMAGE *DestEdgeImage,
						int VerticalFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_FontBaseImageBlt( 
						 x,  y, StrData,
						DestImage, DestEdgeImage,
						 VerticalFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int FontBaseImageBltWithStrLen( int x, int y, const TCHAR *StrData, size_t StrDataLength, BASEIMAGE *DestImage, BASEIMAGE *DestEdgeImage, int VerticalFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_FontBaseImageBltWithStrLen( x, y, StrData, StrDataLength, DestImage, DestEdgeImage, VerticalFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int FontBaseImageBltToHandle( 
						int x, int y, const TCHAR *StrData,
						BASEIMAGE *DestImage, BASEIMAGE *DestEdgeImage,
						int FontHandle, int VerticalFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_FontBaseImageBltToHandle( 
						 x,  y, StrData,
						DestImage, DestEdgeImage,
						 FontHandle,  VerticalFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int FontBaseImageBltToHandleWithStrLen( int x, int y, const TCHAR *StrData, size_t StrDataLength, BASEIMAGE *DestImage, BASEIMAGE *DestEdgeImage, int FontHandle, int VerticalFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_FontBaseImageBltToHandleWithStrLen( x, y, StrData, StrDataLength, DestImage, DestEdgeImage, FontHandle, VerticalFlag ) ;
	DXFUNC_END
	return Result ;
}
extern const TCHAR *GetFontNameToHandle( int FontHandle )
{
	const TCHAR * Result ;
	DXFUNC_BEGIN
	Result = NS_GetFontNameToHandle( FontHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetFontMaxWidth( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetFontMaxWidth(  ) ;
	DXFUNC_END
	return Result ;
}
extern int GetFontMaxWidthToHandle( int FontHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetFontMaxWidthToHandle( FontHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetFontAscent( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetFontAscent() ;
	DXFUNC_END
	return Result ;
}
extern int GetFontAscentToHandle( int FontHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetFontAscentToHandle( FontHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetFontCharInfo( int FontHandle, const TCHAR *Char, int *DrawX, int *DrawY, int *NextCharX, int *SizeX, int *SizeY )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetFontCharInfo( FontHandle, Char, DrawX, DrawY, NextCharX, SizeX, SizeY ) ;
	DXFUNC_END
	return Result ;
}
extern int GetFontCharInfoWithStrLen( int FontHandle, const TCHAR *Char, size_t CharLength, int *DrawX, int *DrawY, int *NextCharX, int *SizeX, int *SizeY )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetFontCharInfoWithStrLen( FontHandle, Char, CharLength, DrawX, DrawY, NextCharX, SizeX, SizeY ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawStringWidth( const TCHAR *String, int StrLen, int VerticalFlag  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawStringWidth( String, StrLen, VerticalFlag  ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawNStringWidth( const TCHAR *String, size_t StringLength, int VerticalFlag  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawNStringWidth( String, StringLength, VerticalFlag  ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawFormatStringWidth( const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	DXFUNC_BEGIN
	Result = NS_GetDrawStringWidth( String, ( int )_TSTRLEN( String ) ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawStringWidthToHandle( const TCHAR *String, int StrLen, int FontHandle, int VerticalFlag  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawStringWidthToHandle( String, StrLen, FontHandle, VerticalFlag  ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawNStringWidthToHandle( const TCHAR *String, size_t StringLength, int FontHandle, int VerticalFlag  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawNStringWidthToHandle( String, StringLength, FontHandle, VerticalFlag  ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawFormatStringWidthToHandle( int FontHandle, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	DXFUNC_BEGIN
	Result = NS_GetDrawStringWidthToHandle( String, ( int )_TSTRLEN( String ), FontHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawExtendStringWidth( double ExRateX, const TCHAR *String, int StrLen, int VerticalFlag  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawExtendStringWidth( ExRateX, String, StrLen, VerticalFlag  ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawExtendNStringWidth( double ExRateX, const TCHAR *String, size_t StringLength, int VerticalFlag  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawExtendNStringWidth( ExRateX, String, StringLength, VerticalFlag  ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawExtendFormatStringWidth( double ExRateX, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	DXFUNC_BEGIN
	Result = NS_GetDrawExtendStringWidth( ExRateX, String, ( int )_TSTRLEN( String ) ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawExtendStringWidthToHandle( double ExRateX, const TCHAR *String, int StrLen, int FontHandle, int VerticalFlag  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawExtendStringWidthToHandle( ExRateX, String, StrLen, FontHandle, VerticalFlag  ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawExtendNStringWidthToHandle( double ExRateX, const TCHAR *String, size_t StringLength, int FontHandle, int VerticalFlag  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawExtendNStringWidthToHandle( ExRateX, String, StringLength, FontHandle, VerticalFlag  ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawExtendFormatStringWidthToHandle( double ExRateX, int FontHandle, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	DXFUNC_BEGIN
	Result = NS_GetDrawExtendStringWidthToHandle( ExRateX, String, ( int )_TSTRLEN( String ), FontHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawStringSize( int *SizeX, int *SizeY, int *LineCount, const TCHAR *String, int StrLen, int VerticalFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawStringSize( SizeX, SizeY, LineCount, String, StrLen, VerticalFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawNStringSize( int *SizeX, int *SizeY, int *LineCount, const TCHAR *String, size_t StringLength, int VerticalFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawNStringSize( SizeX, SizeY, LineCount, String, StringLength, VerticalFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawFormatStringSize( int *SizeX, int *SizeY, int *LineCount, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	DXFUNC_BEGIN
	Result = NS_GetDrawFormatStringSize( SizeX, SizeY, LineCount, String, ( int )_TSTRLEN( String ) ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawExtendStringSize( int *SizeX, int *SizeY, int *LineCount, double ExRateX, double ExRateY, const TCHAR *String, int StrLen, int VerticalFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawExtendStringSize( SizeX, SizeY, LineCount, ExRateX, ExRateY, String, StrLen, VerticalFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawExtendNStringSize( int *SizeX, int *SizeY, int *LineCount, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, int VerticalFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawExtendNStringSize( SizeX, SizeY, LineCount, ExRateX, ExRateY, String, StringLength, VerticalFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawExtendFormatStringSize( int *SizeX, int *SizeY, int *LineCount, double ExRateX, double ExRateY, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	DXFUNC_BEGIN
	Result = NS_GetDrawExtendFormatStringSize( SizeX, SizeY, LineCount, ExRateX, ExRateY, String, ( int )_TSTRLEN( String ) ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawStringSizeToHandle( int *SizeX, int *SizeY, int *LineCount, const TCHAR   *String, int StrLen, int FontHandle, int VerticalFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawStringSizeToHandle( SizeX, SizeY, LineCount, String, StrLen, FontHandle, VerticalFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawNStringSizeToHandle( int *SizeX, int *SizeY, int *LineCount, const TCHAR   *String, size_t StringLength, int FontHandle, int VerticalFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawNStringSizeToHandle( SizeX, SizeY, LineCount, String, StringLength, FontHandle, VerticalFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawFormatStringSizeToHandle( int *SizeX, int *SizeY, int *LineCount, int FontHandle, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	DXFUNC_BEGIN
	Result = NS_GetDrawFormatStringSizeToHandle( SizeX, SizeY, LineCount, FontHandle, String, ( int )_TSTRLEN( String ) ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawExtendStringSizeToHandle( int *SizeX, int *SizeY, int *LineCount, double ExRateX, double ExRateY, const TCHAR *String, int StrLen, int FontHandle, int VerticalFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawExtendStringSizeToHandle( SizeX, SizeY, LineCount, ExRateX, ExRateY, String, StrLen, FontHandle, VerticalFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawExtendNStringSizeToHandle( int *SizeX, int *SizeY, int *LineCount, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, int FontHandle, int VerticalFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawExtendNStringSizeToHandle( SizeX, SizeY, LineCount, ExRateX, ExRateY, String, StringLength, FontHandle, VerticalFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawExtendFormatStringSizeToHandle( int *SizeX, int *SizeY, int *LineCount, double ExRateX, double ExRateY, int FontHandle, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	DXFUNC_BEGIN
	Result = NS_GetDrawExtendFormatStringSizeToHandle( SizeX, SizeY, LineCount, ExRateX, ExRateY, FontHandle, String, ( int )_TSTRLEN( String ) ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawStringCharInfo( DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, const TCHAR *String, int StrLen, int VerticalFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawStringCharInfo( InfoBuffer, InfoBufferSize, String, StrLen, VerticalFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawNStringCharInfo( DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, const TCHAR *String, size_t StringLength, int VerticalFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawNStringCharInfo( InfoBuffer, InfoBufferSize, String, StringLength, VerticalFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawFormatStringCharInfo( DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	DXFUNC_BEGIN
	Result = NS_GetDrawStringCharInfo( InfoBuffer, InfoBufferSize, String, ( int )_TSTRLEN( String ) ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawExtendStringCharInfo( DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, double ExRateX, double ExRateY, const TCHAR *String, int StrLen, int VerticalFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawExtendStringCharInfo( InfoBuffer, InfoBufferSize, ExRateX, ExRateY, String, StrLen, VerticalFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawExtendNStringCharInfo( DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, int VerticalFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawExtendNStringCharInfo( InfoBuffer, InfoBufferSize, ExRateX, ExRateY, String, StringLength, VerticalFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawExtendFormatStringCharInfo( DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, double ExRateX, double ExRateY, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	DXFUNC_BEGIN
	Result = NS_GetDrawExtendStringCharInfo( InfoBuffer, InfoBufferSize, ExRateX, ExRateY, String, ( int )_TSTRLEN( String ) ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawStringCharInfoToHandle( DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, const TCHAR *String, int StrLen, int FontHandle, int VerticalFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawStringCharInfoToHandle( InfoBuffer, InfoBufferSize, String, StrLen, FontHandle, VerticalFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawNStringCharInfoToHandle( DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, const TCHAR *String, size_t StringLength, int FontHandle, int VerticalFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawNStringCharInfoToHandle( InfoBuffer, InfoBufferSize, String, StringLength, FontHandle, VerticalFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawFormatStringCharInfoToHandle( DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, int FontHandle, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	DXFUNC_BEGIN
	Result = NS_GetDrawStringCharInfoToHandle( InfoBuffer, InfoBufferSize, String, ( int )_TSTRLEN( String ), FontHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawExtendStringCharInfoToHandle( DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, double ExRateX, double ExRateY, const TCHAR *String, int StrLen, int FontHandle, int VerticalFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawExtendStringCharInfoToHandle( InfoBuffer, InfoBufferSize, ExRateX, ExRateY, String, StrLen, FontHandle, VerticalFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawExtendNStringCharInfoToHandle( DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, int FontHandle, int VerticalFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawExtendNStringCharInfoToHandle( InfoBuffer, InfoBufferSize, ExRateX, ExRateY, String, StringLength, FontHandle, VerticalFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDrawExtendFormatStringCharInfoToHandle( DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, double ExRateX, double ExRateY, int FontHandle, const TCHAR *FormatString, ... )
{
	int Result ;
	va_list VaList ;
	TCHAR String[ 2048 ] ;

	va_start( VaList, FormatString ) ;
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
	va_end( VaList ) ;
	DXFUNC_BEGIN
	Result = NS_GetDrawExtendStringCharInfoToHandle( InfoBuffer, InfoBufferSize, ExRateX, ExRateY, String, ( int )_TSTRLEN( String ), FontHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetFontStateToHandle( TCHAR *FontName, int *Size, int *Thick, int FontHandle, int *FontType , int *CharSet , int *EdgeSize , int *Italic )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetFontStateToHandle( FontName, Size, Thick, FontHandle , FontType , CharSet , EdgeSize , Italic ) ;
	DXFUNC_END
	return Result ;
}
extern int GetDefaultFontHandle( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDefaultFontHandle(  ) ;
	DXFUNC_END
	return Result ;
}
extern int GetFontChacheToTextureFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetFontChacheToTextureFlag(  ) ;
	DXFUNC_END
	return Result ;
}
extern int GetFontCacheToTextureFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetFontCacheToTextureFlag(  ) ;
	DXFUNC_END
	return Result ;
}
extern int SetFontCacheTextureColorBitDepth( int ColorBitDepth )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetFontCacheTextureColorBitDepth( ColorBitDepth ) ;
	DXFUNC_END
	return Result ;
}
extern int GetFontCacheTextureColorBitDepth( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetFontCacheTextureColorBitDepth( ) ;
	DXFUNC_END
	return Result ;
}
extern int CheckFontChacheToTextureFlag( int FontHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CheckFontChacheToTextureFlag( FontHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int CheckFontCacheToTextureFlag( int FontHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CheckFontCacheToTextureFlag( FontHandle ) ;
	DXFUNC_END
	return Result ;
}
/*
extern LPFONTMANAGE GetFontManageDataToHandle( int FontHandle )
{
	LPFONTMANAGE Result ;
	DXFUNC_BEGIN
	Result = NS_GetFontManageDataToHandle( FontHandle ) ;
	DXFUNC_END
	return Result ;
}
*/
extern int CheckFontHandleValid( int FontHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CheckFontHandleValid( FontHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int ClearFontCacheToHandle( int FontHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ClearFontCacheToHandle( FontHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int MultiByteCharCheck( const char *Buf, int CharSet /*DX_CHARSET_SHFTJIS*/ )
{
	return NS_MultiByteCharCheck( Buf, CharSet /*DX_CHARSET_SHFTJIS*/ ) ;
}
extern int GetFontCacheCharNum( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetFontCacheCharNum(  ) ;
	DXFUNC_END
	return Result ;
}
extern int GetFontSize( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetFontSize(  ) ;
	DXFUNC_END
	return Result ;
}
extern int GetFontEdgeSize( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetFontEdgeSize( ) ;
	DXFUNC_END
	return Result ;
}
extern int GetFontSizeToHandle( int FontHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetFontSizeToHandle( FontHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetFontEdgeSizeToHandle( int FontHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetFontEdgeSizeToHandle( FontHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetFontSpaceToHandle( int FontHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetFontSpaceToHandle( FontHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetFontLineSpaceToHandle( int FontHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetFontLineSpaceToHandle( FontHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetFontCacheUsePremulAlphaFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetFontCacheUsePremulAlphaFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetFontCacheUsePremulAlphaFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetFontCacheUsePremulAlphaFlag() ;
	DXFUNC_END
	return Result ;
}
extern int SetFontUseAdjustSizeFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetFontUseAdjustSizeFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetFontUseAdjustSizeFlag( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetFontUseAdjustSizeFlag() ;
	DXFUNC_END
	return Result ;
}

extern HANDLE AddFontFile( const TCHAR *FontFilePath )
{
	HANDLE Result ;
	DXFUNC_BEGIN
	Result = NS_AddFontFile( FontFilePath ) ;
	DXFUNC_END
	return Result ;
}
extern HANDLE AddFontFileWithStrLen( const TCHAR *FontFilePath, size_t FontFilePathLength )
{
	HANDLE Result ;
	DXFUNC_BEGIN
	Result = NS_AddFontFileWithStrLen( FontFilePath, FontFilePathLength ) ;
	DXFUNC_END
	return Result ;
}

extern HANDLE AddFontFileFromMem( const void *FontFileImage, int FontFileImageSize )
{
	HANDLE Result ;
	DXFUNC_BEGIN
	Result = NS_AddFontFileFromMem( FontFileImage, FontFileImageSize ) ;
	DXFUNC_END
	return Result ;
}

extern int RemoveFontFile( HANDLE FontHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_RemoveFontFile( FontHandle ) ;
	DXFUNC_END
	return Result ;
}

#ifndef DX_NON_SAVEFUNCTION

extern int CreateFontDataFile( const TCHAR *SaveFilePath, const TCHAR *FontName, int Size, int BitDepth /* DX_FONTIMAGE_BIT_1等 */ , int Thick, int Italic, int CharSet, const TCHAR *SaveCharaList )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateFontDataFile( SaveFilePath, FontName, Size, BitDepth, Thick, Italic, CharSet, SaveCharaList ) ;
	DXFUNC_END
	return Result ;
}
extern int CreateFontDataFileWithStrLen( const TCHAR *SaveFilePath, size_t SaveFilePathLength, const TCHAR *FontName, size_t FontNameLength, int Size, int BitDepth, int Thick, int Italic, int CharSet, const TCHAR *SaveCharaList, size_t SaveCharaListLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateFontDataFileWithStrLen( SaveFilePath, SaveFilePathLength, FontName, FontNameLength, Size, BitDepth, Thick, Italic, CharSet, SaveCharaList, SaveCharaListLength ) ;
	DXFUNC_END
	return Result ;
}

#endif // DX_NON_SAVEFUNCTION

#endif // DX_NON_FONT

// 基本イメージデータのロード＋ＤＩＢ関係
extern int CreateGraphImageOrDIBGraph( const TCHAR *FileName, const void *DataImage, int DataImageSize, int DataImageType, int BmpFlag, int ReverseFlag, BASEIMAGE *BaseImage, BITMAPINFO **BmpInfo, void **GraphData )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateGraphImageOrDIBGraph( FileName, DataImage, DataImageSize, DataImageType, BmpFlag, ReverseFlag, BaseImage, BmpInfo,  GraphData ) ;
	DXFUNC_END
	return Result ;
}
extern int CreateGraphImageOrDIBGraphWithStrLen( const TCHAR *FileName, size_t FileNameLength, const void *DataImage, int DataImageSize, int DataImageType, int BmpFlag, int ReverseFlag, BASEIMAGE *BaseImage, BITMAPINFO **BmpInfo, void **GraphData )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateGraphImageOrDIBGraphWithStrLen( FileName, FileNameLength, DataImage, DataImageSize, DataImageType, BmpFlag, ReverseFlag, BaseImage, BmpInfo, GraphData ) ;
	DXFUNC_END
	return Result ;
}

extern int CreateGraphImageType2( STREAMDATA *Src, BASEIMAGE *Dest )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateGraphImageType2( Src, Dest ) ;
	DXFUNC_END
	return Result ;
}
extern int CreateBmpInfo( BITMAPINFO *BmpInfo, int Width, int Height, int Pitch, const void *SrcGrData, void **DestGrData )
{
	return NS_CreateBmpInfo( BmpInfo, Width, Height, Pitch, SrcGrData,  DestGrData ) ;
}
extern int GetImageSize_File( const TCHAR *FileName, int *SizeX, int *SizeY )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetImageSize_File( FileName, SizeX, SizeY ) ;
	DXFUNC_END
	return Result ;
}
extern int GetImageSize_FileWithStrLen( const TCHAR *FileName, size_t FileNameLength, int *SizeX, int *SizeY )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetImageSize_FileWithStrLen( FileName, FileNameLength, SizeX, SizeY ) ;
	DXFUNC_END
	return Result ;
}
extern int GetImageSize_Mem( const void *FileImage, int FileImageSize, int *SizeX, int *SizeY )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetImageSize_Mem( FileImage, FileImageSize, SizeX, SizeY ) ;
	DXFUNC_END
	return Result ;
}
extern HBITMAP CreateDIBGraphVer2( const TCHAR *FileName, const void *FileImage, int FileImageSize, int ImageType, int ReverseFlag, COLORDATA *SrcColor )
{
	HBITMAP Result ;
	DXFUNC_BEGIN
	Result = NS_CreateDIBGraphVer2( FileName, FileImage,  FileImageSize,  ImageType,  ReverseFlag, SrcColor ) ;
	DXFUNC_END
	return Result ;
}
extern HBITMAP CreateDIBGraphVer2WithStrLen( const TCHAR *FileName, size_t FileNameLength, const void *MemImage, int MemImageSize, int ImageType, int ReverseFlag, COLORDATA *SrcColor )
{
	HBITMAP Result ;
	DXFUNC_BEGIN
	Result = NS_CreateDIBGraphVer2WithStrLen( FileName, FileNameLength, MemImage, MemImageSize, ImageType, ReverseFlag, SrcColor ) ;
	DXFUNC_END
	return Result ;
}
extern int CreateDIBGraphVer2_plus_Alpha( const TCHAR *FileName, const void *MemImage, int MemImageSize, const void *AlphaImage, int AlphaImageSize, int ImageType, HBITMAP *RGBBmp, HBITMAP *AlphaBmp, int ReverseFlag, COLORDATA *SrcColor )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateDIBGraphVer2_plus_Alpha( FileName, MemImage, MemImageSize,  AlphaImage, AlphaImageSize, ImageType, RGBBmp, AlphaBmp, ReverseFlag, SrcColor ) ;
	DXFUNC_END
	return Result ;
}
extern int CreateDIBGraphVer2_plus_AlphaWithStrLen( const TCHAR *FileName, size_t FileNameLength, const void *MemImage, int MemImageSize, const void *AlphaImage, int AlphaImageSize, int ImageType, HBITMAP *RGBBmp, HBITMAP *AlphaBmp, int ReverseFlag, COLORDATA *SrcColor )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateDIBGraphVer2_plus_AlphaWithStrLen( FileName, FileNameLength, MemImage, MemImageSize, AlphaImage, AlphaImageSize, ImageType, RGBBmp, AlphaBmp, ReverseFlag, SrcColor ) ;
	DXFUNC_END
	return Result ;
}
extern unsigned int GetGraphImageFullColorCode( const BASEIMAGE *GraphImage, int x, int y )
{
	return NS_GetGraphImageFullColorCode( GraphImage,  x,  y ) ;
}
extern int CreateGraphImage_plus_Alpha( const TCHAR *FileName, const void *RgbImage, int RgbImageSize, int RgbImageType,
													const void *AlphaImage, int AlphaImageSize, int AlphaImageType,
													BASEIMAGE *RgbGraphImage, BASEIMAGE *AlphaGraphImage, int ReverseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateGraphImage_plus_Alpha( FileName, RgbImage,  RgbImageSize,  RgbImageType,
													AlphaImage,  AlphaImageSize,  AlphaImageType,
													RgbGraphImage, AlphaGraphImage, ReverseFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int CreateGraphImage_plus_AlphaWithStrLen( const TCHAR *FileName, size_t FileNameLength, const void *RgbImage, int RgbImageSize, int RgbImageType, const void *AlphaImage, int AlphaImageSize, int AlphaImageType, BASEIMAGE *RgbGraphImage, BASEIMAGE *AlphaGraphImage, int ReverseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateGraphImage_plus_AlphaWithStrLen( FileName, FileNameLength, RgbImage, RgbImageSize, RgbImageType, AlphaImage, AlphaImageSize, AlphaImageType, RgbGraphImage, AlphaGraphImage, ReverseFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int ReverseGraphImage( BASEIMAGE *GraphImage )
{
	return NS_ReverseGraphImage( GraphImage ) ;
}
extern int ConvBitmapToGraphImage( const BITMAPINFO *BmpInfo, void *GraphData, BASEIMAGE *GraphImage, int CopyFlag )
{
	return NS_ConvBitmapToGraphImage( BmpInfo, GraphData, GraphImage, CopyFlag ) ;
}
extern int ConvGraphImageToBitmap( const BASEIMAGE *GraphImage, BITMAPINFO *BmpInfo, void **GraphData, int CopyFlag, int FullColorConv )
{
	return NS_ConvGraphImageToBitmap( GraphImage, BmpInfo, GraphData, CopyFlag, FullColorConv ) ;
}
extern	HBITMAP	CreateDIBGraph( const TCHAR *FileName, int ReverseFlag, COLORDATA *SrcColor)
{
	HBITMAP Result ;
	DXFUNC_BEGIN
	Result = NS_CreateDIBGraph( FileName, ReverseFlag, SrcColor) ;
	DXFUNC_END
	return Result ;
}
extern HBITMAP CreateDIBGraphWithStrLen( const TCHAR *FileName, size_t FileNameLength, int ReverseFlag, COLORDATA *SrcColor )
{
	HBITMAP Result ;
	DXFUNC_BEGIN
	Result = NS_CreateDIBGraphWithStrLen( FileName, FileNameLength, ReverseFlag, SrcColor ) ;
	DXFUNC_END
	return Result ;
}
extern	HBITMAP	CreateDIBGraphToMem( const BITMAPINFO *BmpInfo, const void *GraphData, int ReverseFlag, COLORDATA *SrcColor )
{
	return NS_CreateDIBGraphToMem( BmpInfo, GraphData, ReverseFlag, SrcColor ) ;
}
extern int CreateDIBGraph_plus_Alpha( const TCHAR *FileName, HBITMAP *RGBBmp, HBITMAP *AlphaBmp, int ReverseFlag , COLORDATA *SrcColor  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateDIBGraph_plus_Alpha( FileName, RGBBmp, AlphaBmp, ReverseFlag , SrcColor  ) ;
	DXFUNC_END
	return Result ;
}
extern int CreateDIBGraph_plus_AlphaWithStrLen( const TCHAR *FileName, size_t FileNameLength, HBITMAP *RGBBmp, HBITMAP *AlphaBmp, int ReverseFlag, COLORDATA *SrcColor )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateDIBGraph_plus_AlphaWithStrLen( FileName, FileNameLength, RGBBmp, AlphaBmp, ReverseFlag, SrcColor ) ;
	DXFUNC_END
	return Result ;
}

// 補助関係
//extern int AddUserGraphLoadFunction( int ( *UserLoadFunc )( FILE *fp, BITMAPINFO **BmpInfo, void **GraphData ))
//extern int AddUserGraphLoadFunction2( int ( *UserLoadFunc )( void *Image, int ImageSize, int ImageType, BITMAPINFO **BmpInfo, void **GraphData ))
//extern int AddUserGraphLoadFunction3( int ( *UserLoadFunc )( void *DataImage, int DataImageSize, int DataImageType, int BmpFlag, BASEIMAGE *BaseImage, BITMAPINFO **BmpInfo, void **GraphData ))
extern int AddUserGraphLoadFunction4( int ( *UserLoadFunc )( STREAMDATA *Src, BASEIMAGE *BaseImage ))
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_AddUserGraphLoadFunction4( UserLoadFunc ) ;
	DXFUNC_END
	return Result ;
}
//extern int SubUserGraphLoadFunction( int ( *UserLoadFunc )( FILE *fp, BITMAPINFO **BmpInfo, void **GraphData ))
//extern int SubUserGraphLoadFunction2( int ( *UserLoadFunc )( void *Image, int ImageSize, int ImageType, BITMAPINFO **BmpInfo, void **GraphData ))
//extern int SubUserGraphLoadFunction3( int ( *UserLoadFunc )( void *DataImage, int DataImageSize, int DataImageType, int BmpFlag, BASEIMAGE *BaseImage, BITMAPINFO **BmpInfo, void **GraphData ))
extern int SubUserGraphLoadFunction4( int ( *UserLoadFunc )( STREAMDATA *Src, BASEIMAGE *BaseImage ))
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SubUserGraphLoadFunction4( UserLoadFunc ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUseFastLoadFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseFastLoadFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int GetGraphDataShavedMode( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetGraphDataShavedMode(  ) ;
	DXFUNC_END
	return Result ;
}
extern int SetGraphDataShavedMode( int ShavedMode )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetGraphDataShavedMode( ShavedMode ) ;
	DXFUNC_END
	return Result ;
}
extern int SetUsePremulAlphaConvertLoad(  int UseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUsePremulAlphaConvertLoad( UseFlag ) ;
	DXFUNC_END
	return Result ;
}

// 基本イメージデータ機能公開用関数

#ifndef DX_NON_SOFTIMAGE

extern int InitSoftImage( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_InitSoftImage() ;
	DXFUNC_END
	return Result ;
}
extern int LoadSoftImage( const TCHAR *FileName )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadSoftImage( FileName ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadSoftImageWithStrLen( const TCHAR *FileName, size_t FileNameLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadSoftImageWithStrLen( FileName, FileNameLength ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadARGB8ColorSoftImage( const TCHAR *FileName )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadARGB8ColorSoftImage( FileName ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadARGB8ColorSoftImageWithStrLen( const TCHAR *FileName, size_t FileNameLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadARGB8ColorSoftImageWithStrLen( FileName, FileNameLength ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadXRGB8ColorSoftImage( const TCHAR *FileName )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadXRGB8ColorSoftImage( FileName ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadXRGB8ColorSoftImageWithStrLen( const TCHAR *FileName, size_t FileNameLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadXRGB8ColorSoftImageWithStrLen( FileName, FileNameLength ) ;
	DXFUNC_END
	return Result ;
}

extern int LoadSoftImageToMem( const void *FileImage, int FileImageSize )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadSoftImageToMem( FileImage, FileImageSize ) ;
	DXFUNC_END
	return Result ;
}

extern int LoadARGB8ColorSoftImageToMem( const void *FileImage, int FileImageSize )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadARGB8ColorSoftImageToMem( FileImage, FileImageSize ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadXRGB8ColorSoftImageToMem( const void *FileImage, int FileImageSize )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadXRGB8ColorSoftImageToMem( FileImage, FileImageSize ) ;
	DXFUNC_END
	return Result ;
}

extern	int		MakeSoftImage( int SizeX, int SizeY )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MakeSoftImage(  SizeX,  SizeY ) ;
	DXFUNC_END
	return Result ;
}

extern	int		MakeARGBF32ColorSoftImage( int SizeX, int SizeY )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MakeARGBF32ColorSoftImage( SizeX, SizeY ) ;
	DXFUNC_END
	return Result ;
}

extern	int		MakeARGBF16ColorSoftImage( int SizeX, int SizeY )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MakeARGBF16ColorSoftImage( SizeX, SizeY ) ;
	DXFUNC_END
	return Result ;
}

extern	int		MakeARGB8ColorSoftImage( int SizeX, int SizeY )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MakeARGB8ColorSoftImage(  SizeX,  SizeY ) ;
	DXFUNC_END
	return Result ;
}

extern	int		MakeXRGB8ColorSoftImage( int SizeX, int SizeY )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MakeXRGB8ColorSoftImage(  SizeX,  SizeY ) ;
	DXFUNC_END
	return Result ;
}
extern	int		MakeARGB4ColorSoftImage( int SizeX, int SizeY )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MakeARGB4ColorSoftImage(  SizeX,  SizeY ) ;
	DXFUNC_END
	return Result ;
}
extern	int		MakeA1R5G5B5ColorSoftImage( int SizeX, int SizeY )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MakeA1R5G5B5ColorSoftImage(  SizeX,  SizeY ) ;
	DXFUNC_END
	return Result ;
}
extern	int		MakeX1R5G5B5ColorSoftImage( int SizeX, int SizeY )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MakeX1R5G5B5ColorSoftImage(  SizeX,  SizeY ) ;
	DXFUNC_END
	return Result ;
}
extern	int		MakeR5G5B5A1ColorSoftImage( int SizeX, int SizeY )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MakeR5G5B5A1ColorSoftImage(  SizeX,  SizeY ) ;
	DXFUNC_END
	return Result ;
}
extern	int		MakeR5G6B5ColorSoftImage( int SizeX, int SizeY )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MakeR5G6B5ColorSoftImage(  SizeX,  SizeY ) ;
	DXFUNC_END
	return Result ;
}
extern	int		MakeRGB8ColorSoftImage( int SizeX, int SizeY )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MakeRGB8ColorSoftImage(  SizeX,  SizeY ) ;
	DXFUNC_END
	return Result ;
}
extern	int		MakePAL8ColorSoftImage( int SizeX, int SizeY, int UseAlpha )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MakePAL8ColorSoftImage(  SizeX,  SizeY,  UseAlpha ) ;
	DXFUNC_END
	return Result ;
}


extern	int		DeleteSoftImage( int SIHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DeleteSoftImage(  SIHandle ) ;
	DXFUNC_END
	return Result ;
}

extern	int		GetSoftImageSize( int SIHandle, int *Width, int *Height )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetSoftImageSize(  SIHandle, Width, Height ) ;
	DXFUNC_END
	return Result ;
}

extern	int		GetDrawScreenSoftImage( int x1, int y1, int x2, int y2, int SIHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawScreenSoftImage(  x1,  y1,  x2,  y2,  SIHandle ) ;
	DXFUNC_END
	return Result ;
}

extern	int		GetDrawScreenSoftImageDestPos( int x1, int y1, int x2, int y2, int SIHandle, int DestX, int DestY )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawScreenSoftImageDestPos(  x1,  y1,  x2,  y2,  SIHandle,  DestX,  DestY ) ;
	DXFUNC_END
	return Result ;
}

extern	int		UpdateLayerdWindowForSoftImage( int SIHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_UpdateLayerdWindowForSoftImage( SIHandle ) ;
	DXFUNC_END
	return Result ;
}

extern	int		UpdateLayerdWindowForSoftImageRect( int SIHandle, int x1, int y1, int x2, int y2 )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_UpdateLayerdWindowForSoftImageRect(  SIHandle,  x1,  y1,  x2,  y2 ) ;
	DXFUNC_END
	return Result ;
}

extern	int		UpdateLayerdWindowForPremultipliedAlphaSoftImage( int SIHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_UpdateLayerdWindowForPremultipliedAlphaSoftImage( SIHandle ) ;
	DXFUNC_END
	return Result ;
}

extern	int		UpdateLayerdWindowForPremultipliedAlphaSoftImageRect( int SIHandle, int x1, int y1, int x2, int y2 )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_UpdateLayerdWindowForPremultipliedAlphaSoftImageRect(  SIHandle,  x1,  y1,  x2,  y2 ) ;
	DXFUNC_END
	return Result ;
}

extern	int		GetDesktopScreenSoftImage( int x1, int y1, int x2, int y2, int SIHandle, int DestX, int DestY )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDesktopScreenSoftImage( x1, y1, x2, y2, SIHandle, DestX, DestY ) ;
	DXFUNC_END
	return Result ;
}

extern	int		FillSoftImage( int SIHandle, int r, int g, int b, int a )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_FillSoftImage(  SIHandle,  r,  g,  b,  a ) ;
	DXFUNC_END
	return Result ;
}

extern	int		ClearRectSoftImage( int SIHandle, int x, int y, int w, int h )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ClearRectSoftImage(  SIHandle,  x,  y,  w,  h ) ;
	DXFUNC_END
	return Result ;
}

extern	int		CheckPaletteSoftImage( int SIHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CheckPaletteSoftImage( SIHandle ) ;
	DXFUNC_END
	return Result ;
}

extern	int		CheckAlphaSoftImage( int SIHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CheckAlphaSoftImage( SIHandle ) ;
	DXFUNC_END
	return Result ;
}

extern	int		GetPaletteSoftImage( int SIHandle, int PaletteNo, int *r, int *g, int *b, int *a )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetPaletteSoftImage(  SIHandle,  PaletteNo, r, g, b, a ) ;
	DXFUNC_END
	return Result ;
}

extern	int		SetPaletteSoftImage( int SIHandle, int PaletteNo, int  r, int  g, int  b, int  a )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetPaletteSoftImage(  SIHandle,  PaletteNo,   r,   g,   b,   a ) ;
	DXFUNC_END
	return Result ;
}

extern	int		DrawPixelPalCodeSoftImage( int SIHandle, int x, int y, int palNo )
{
	int Result ;
	SOFTIMAGE *SoftImg ;

	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;
	Result = NS_SetPixelPalCodeBaseImage( &SoftImg->BaseImage,  x,  y,  palNo ) ;
	return Result ;
}

extern	int	GetPixelPalCodeSoftImage( int SIHandle, int x, int y )
{
	int Result ;
	SOFTIMAGE *SoftImg ;

	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;
	Result = NS_GetPixelPalCodeBaseImage( &SoftImg->BaseImage,  x,  y ) ;
	return Result ;
}

extern	void	*GetImageAddressSoftImage( int SIHandle )
{
	SOFTIMAGE *SoftImg ;

	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return NULL ;
	return SoftImg->BaseImage.GraphData ;
}

extern	int		GetPitchSoftImage( int SIHandle )
{
	int Result ;
	SOFTIMAGE *SoftImg ;

	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;
	Result = NS_GetPitchSoftImage( SIHandle ) ;
	return Result ;
}

extern	int		DrawPixelSoftImage( int SIHandle, int x, int y, int  r, int  g, int  b, int  a )
{
	int Result ;
	SOFTIMAGE *SoftImg ;

	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;
	Result = NS_SetPixelBaseImage( &SoftImg->BaseImage,  x,  y,   r,   g,   b,   a ) ;
	return Result ;
}

extern	int		DrawPixelSoftImageF( int SIHandle, int x, int y, float  r, float  g, float  b, float  a )
{
	int Result ;
	SOFTIMAGE *SoftImg ;

	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;
	Result = NS_DrawPixelSoftImageF( SIHandle, x, y, r, g, b, a ) ;
	return Result ;
}

extern	void	DrawPixelSoftImage_Unsafe_XRGB8( int SIHandle, int x, int y, int  r, int  g, int  b )
{
	SOFTIMAGE *SoftImg ;

	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return ;
	NS_DrawPixelSoftImage_Unsafe_XRGB8( SIHandle,  x,  y,   r,   g,   b ) ;
}

extern	void	DrawPixelSoftImage_Unsafe_ARGB8( int SIHandle, int x, int y, int  r, int  g, int  b, int a  )
{
	SOFTIMAGE *SoftImg ;

	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return ;
	NS_DrawPixelSoftImage_Unsafe_ARGB8( SIHandle,  x,  y,   r,   g,  b, a ) ;
}

extern	int		GetPixelSoftImage(  int SIHandle, int x, int y, int *r, int *g, int *b, int *a )
{
	int Result ;
	SOFTIMAGE *SoftImg ;

	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;
	Result = NS_GetPixelBaseImage( &SoftImg->BaseImage,  x,  y,  r,  g,  b,  a ) ;
	return Result ;
}
extern	int		GetPixelSoftImageF( int SIHandle, int x, int y, float *r, float *g, float *b, float *a )
{
	int Result ;
	SOFTIMAGE *SoftImg ;

	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;
	Result = NS_GetPixelSoftImageF( SIHandle, x, y, r, g, b, a ) ;
	return Result ;
}
extern	void	GetPixelSoftImage_Unsafe_XRGB8(  int SIHandle, int x, int y, int *r, int *g, int *b )
{
	SOFTIMAGE *SoftImg ;

	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return ;
	NS_GetPixelSoftImage_Unsafe_XRGB8( SIHandle,  x,  y,  r,  g,  b ) ;
}
extern	void	GetPixelSoftImage_Unsafe_ARGB8(  int SIHandle, int x, int y, int *r, int *g, int *b, int *a )
{
	SOFTIMAGE *SoftImg ;

	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return ;
	NS_GetPixelSoftImage_Unsafe_ARGB8( SIHandle,  x,  y,  r,  g,  b,  a ) ;
}
extern	int		DrawLineSoftImage(  int SIHandle, int x1, int y1, int x2, int y2, int r, int g, int b, int a )
{
	int Result ;
	SOFTIMAGE *SoftImg ;

	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;
	Result = NS_DrawLineBaseImage( &SoftImg->BaseImage,  x1,  y1, x2, y2,  r,  g,  b,  a ) ;
	return Result ;
}
extern	int		DrawCircleSoftImage( int SIHandle, int x, int y, int radius, int r, int g, int b, int a, int FillFlag )
{
	int Result ;
	SOFTIMAGE *SoftImg ;

	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;
	Result = NS_DrawCircleBaseImage( &SoftImg->BaseImage, x, y, radius, r, g, b, a, FillFlag ) ;
	return Result ;
}
extern	int		BltSoftImage( int SrcX, int SrcY, int SrcSizeX, int SrcSizeY, int SrcSIHandle, int DestX, int DestY, int DestSIHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_BltSoftImage(  SrcX,  SrcY,  SrcSizeX,  SrcSizeY,  SrcSIHandle,  DestX,  DestY,  DestSIHandle ) ;
	DXFUNC_END
	return Result ;
}
extern	int			BltSoftImageWithTransColor( int SrcX, int SrcY, int SrcSizeX, int SrcSizeY, int SrcSIHandle, int DestX, int DestY, int DestSIHandle, int Tr, int Tg, int Tb, int Ta )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_BltSoftImageWithTransColor(  SrcX,  SrcY,  SrcSizeX,  SrcSizeY,  SrcSIHandle,  DestX,  DestY,  DestSIHandle,  Tr,  Tg,  Tb,  Ta ) ;
	DXFUNC_END
	return Result ;
}
extern	int			BltSoftImageWithAlphaBlend( int SrcX, int SrcY, int SrcSizeX, int SrcSizeY, int SrcSIHandle, int DestX, int DestY, int DestSIHandle, int Opacity )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_BltSoftImageWithAlphaBlend(  SrcX,  SrcY,  SrcSizeX,  SrcSizeY,  SrcSIHandle,  DestX,  DestY,  DestSIHandle,  Opacity ) ;
	DXFUNC_END
	return Result ;
}
extern	int			ReverseSoftImageH( int SIHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReverseSoftImageH( SIHandle ) ;
	DXFUNC_END
	return Result ;
}
extern	int			ReverseSoftImageV( int SIHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReverseSoftImageV( SIHandle ) ;
	DXFUNC_END
	return Result ;
}
extern	int			ReverseSoftImage( int SIHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReverseSoftImage( SIHandle ) ;
	DXFUNC_END
	return Result ;
}
extern	int			CheckPixelAlphaSoftImage( int SIHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CheckPixelAlphaSoftImage( SIHandle ) ;
	DXFUNC_END
	return Result ;
}
#ifndef DX_NON_FONT
extern	int			BltStringSoftImage( 
						int x, int y, const TCHAR *StrData,
						int DestSIHandle, int DestEdgeSIHandle,
						int VerticalFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_BltStringSoftImage( 
						 x,  y, StrData,
						 DestSIHandle,  DestEdgeSIHandle,
						 VerticalFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int BltStringSoftImageWithStrLen( int x, int y, const TCHAR *StrData, size_t StrDataLength, int DestSIHandle, int DestEdgeSIHandle, int VerticalFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_BltStringSoftImageWithStrLen( x, y, StrData, StrDataLength, DestSIHandle, DestEdgeSIHandle, VerticalFlag ) ;
	DXFUNC_END
	return Result ;
}
extern	int			BltStringSoftImageToHandle( 
						int x, int y, const TCHAR *StrData,
						int DestSIHandle, int DestEdgeSIHandle /* 縁が必要ない場合は -1 */,
						int FontHandle, int VerticalFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_BltStringSoftImageToHandle( 
						 x,  y, StrData,
						 DestSIHandle,  DestEdgeSIHandle,
						 FontHandle, VerticalFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int BltStringSoftImageToHandleWithStrLen( int x, int y, const TCHAR *StrData, size_t StrDataLength, int DestSIHandle, int DestEdgeSIHandle, int FontHandle, int VerticalFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_BltStringSoftImageToHandleWithStrLen( x, y, StrData, StrDataLength, DestSIHandle, DestEdgeSIHandle, FontHandle, VerticalFlag ) ;
	DXFUNC_END
	return Result ;
}
#endif // DX_NON_FONT

extern	int		DrawSoftImage( int x, int y, int SIHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawSoftImage(  x,  y,  SIHandle ) ;
	DXFUNC_END
	return Result ;
}

#ifndef DX_NON_SAVEFUNCTION

extern	int		SaveSoftImageToBmp( const TCHAR *FilePath, int SIHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveSoftImageToBmp( FilePath, SIHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SaveSoftImageToBmpWithStrLen( const TCHAR *FilePath, size_t FilePathLength, int SIHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveSoftImageToBmpWithStrLen( FilePath, FilePathLength, SIHandle ) ;
	DXFUNC_END
	return Result ;
}

extern	int		SaveSoftImageToDds( const TCHAR *FilePath, int SIHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveSoftImageToDds( FilePath, SIHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SaveSoftImageToDdsWithStrLen( const TCHAR *FilePath, size_t FilePathLength, int SIHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveSoftImageToDdsWithStrLen( FilePath, FilePathLength, SIHandle ) ;
	DXFUNC_END
	return Result ;
}

#ifndef DX_NON_PNGREAD
extern	int		SaveSoftImageToPng( const TCHAR *FilePath, int SIHandle, int CompressionLevel )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveSoftImageToPng( FilePath,  SIHandle,  CompressionLevel ) ;
	DXFUNC_END
	return Result ;
}
extern int SaveSoftImageToPngWithStrLen( const TCHAR *FilePath, size_t FilePathLength, int SIHandle, int CompressionLevel )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveSoftImageToPngWithStrLen( FilePath, FilePathLength, SIHandle, CompressionLevel ) ;
	DXFUNC_END
	return Result ;
}
#endif

#ifndef DX_NON_JPEGREAD
extern	int		SaveSoftImageToJpeg( const TCHAR *FilePath, int SIHandle, int Quality, int Sample2x1 )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveSoftImageToJpeg( FilePath,  SIHandle,  Quality,  Sample2x1 ) ;
	DXFUNC_END
	return Result ;
}
extern int SaveSoftImageToJpegWithStrLen( const TCHAR *FilePath, size_t FilePathLength, int SIHandle, int Quality, int Sample2x1 )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveSoftImageToJpegWithStrLen( FilePath, FilePathLength, SIHandle, Quality, Sample2x1 ) ;
	DXFUNC_END
	return Result ;
}
#endif

#endif // DX_NON_SAVEFUNCTION

#endif // DX_NON_SOFTIMAGE

//基本イメージデータ構造体関係
extern	int		CreateBaseImage(  const TCHAR *FileName, const void *FileImage, int FileImageSize, int DataType, BASEIMAGE *BaseImage, int ReverseFlag ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateBaseImage(  FileName, FileImage,  FileImageSize,  DataType, BaseImage, ReverseFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int CreateBaseImageWithStrLen( const TCHAR *FileName, size_t FileNameLength, const void *FileImage, int FileImageSize, int DataType, BASEIMAGE *BaseImage,  int ReverseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateBaseImageWithStrLen( FileName, FileNameLength, FileImage, FileImageSize, DataType, BaseImage,  ReverseFlag ) ;
	DXFUNC_END
	return Result ;
}
extern	int		CreateGraphImage( const TCHAR *FileName, const void *DataImage, int DataImageSize, int DataImageType, BASEIMAGE *GraphImage, int ReverseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateGraphImage( FileName, DataImage,  DataImageSize,  DataImageType, GraphImage,  ReverseFlag );
	DXFUNC_END
	return Result ;
}
extern	int		CreateBaseImageToFile( const TCHAR *FileName,               BASEIMAGE *BaseImage, int ReverseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateBaseImageToFile( FileName,  BaseImage, ReverseFlag );
	DXFUNC_END
	return Result ;
}
extern int CreateBaseImageToFileWithStrLen( const TCHAR *FileName, size_t FileNameLength, BASEIMAGE *BaseImage,  int ReverseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateBaseImageToFileWithStrLen( FileName, FileNameLength, BaseImage,  ReverseFlag ) ;
	DXFUNC_END
	return Result ;
}
extern	int		CreateBaseImageToMem(  const void *FileImage, int FileImageSize, BASEIMAGE *BaseImage, int ReverseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateBaseImageToMem(  FileImage, FileImageSize, BaseImage, ReverseFlag );
	DXFUNC_END
	return Result ;
}
extern	int		CreateARGBF32ColorBaseImage( int SizeX, int SizeY, BASEIMAGE *BaseImage )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateARGBF32ColorBaseImage( SizeX, SizeY, BaseImage ) ;
	DXFUNC_END
	return Result ;
}
extern	int		CreateARGBF16ColorBaseImage( int SizeX, int SizeY, BASEIMAGE *BaseImage )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateARGBF16ColorBaseImage( SizeX, SizeY, BaseImage ) ;
	DXFUNC_END
	return Result ;
}
extern	int		CreateARGB8ColorBaseImage( int SizeX, int SizeY, BASEIMAGE *BaseImage )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateARGB8ColorBaseImage(  SizeX,  SizeY, BaseImage );
	DXFUNC_END
	return Result ;
}
extern	int		CreateXRGB8ColorBaseImage( int SizeX, int SizeY, BASEIMAGE *BaseImage )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateXRGB8ColorBaseImage(  SizeX,  SizeY, BaseImage );
	DXFUNC_END
	return Result ;
}
extern	int		CreateRGB8ColorBaseImage( int SizeX, int SizeY, BASEIMAGE *BaseImage )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateRGB8ColorBaseImage(  SizeX,  SizeY, BaseImage );
	DXFUNC_END
	return Result ;
}
extern	int		CreateARGB4ColorBaseImage( int SizeX, int SizeY, BASEIMAGE *BaseImage )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateARGB4ColorBaseImage(  SizeX,  SizeY, BaseImage );
	DXFUNC_END
	return Result ;
}
extern	int		CreateA1R5G5B5ColorBaseImage( int SizeX, int SizeY, BASEIMAGE *BaseImage )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateA1R5G5B5ColorBaseImage( SizeX, SizeY, BaseImage );
	DXFUNC_END
	return Result ;
}
extern	int		CreateX1R5G5B5ColorBaseImage( int SizeX, int SizeY, BASEIMAGE *BaseImage )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateX1R5G5B5ColorBaseImage( SizeX, SizeY, BaseImage );
	DXFUNC_END
	return Result ;
}
extern	int		CreateR5G5B5A1ColorBaseImage( int SizeX, int SizeY, BASEIMAGE *BaseImage )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateR5G5B5A1ColorBaseImage( SizeX, SizeY, BaseImage );
	DXFUNC_END
	return Result ;
}
extern	int		CreateR5G6B5ColorBaseImage( int SizeX, int SizeY, BASEIMAGE *BaseImage )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateR5G6B5ColorBaseImage( SizeX, SizeY, BaseImage );
	DXFUNC_END
	return Result ;
}
extern	int		CreatePAL8ColorBaseImage( int SizeX, int SizeY, BASEIMAGE *BaseImage, int UseAlpha )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreatePAL8ColorBaseImage(  SizeX,  SizeY, BaseImage, UseAlpha );
	DXFUNC_END
	return Result ;
}

extern	int		CreateColorDataBaseImage( int SizeX, int SizeY, const COLORDATA *ColorData, BASEIMAGE *BaseImage )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CreateColorDataBaseImage(  SizeX,  SizeY, ColorData, BaseImage ) ;
	DXFUNC_END
	return Result ;
}

extern	int		GetBaseImageGraphDataSize( const BASEIMAGE *BaseImage )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetBaseImageGraphDataSize( BaseImage ) ;
	DXFUNC_END
	return Result ;
}

extern	int		DerivationBaseImage( const BASEIMAGE *BaseImage, int x1, int y1, int x2, int y2, BASEIMAGE *NewBaseImage )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DerivationBaseImage( BaseImage,  x1,  y1,  x2,  y2, NewBaseImage ) ;
	DXFUNC_END
	return Result ;
}

extern	int		ReleaseBaseImage(  BASEIMAGE *GraphImage )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReleaseBaseImage(  GraphImage );
	DXFUNC_END
	return Result ;
}
extern	int		ReleaseGraphImage( BASEIMAGE *GraphImage )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReleaseGraphImage( GraphImage );
	DXFUNC_END
	return Result ;
}
extern	int		ConvertNormalFormatBaseImage( BASEIMAGE *BaseImage, int ReleaseOrigGraphData )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ConvertNormalFormatBaseImage( BaseImage, ReleaseOrigGraphData );
	DXFUNC_END
	return Result ;
}
extern	int		ConvertPremulAlphaBaseImage( BASEIMAGE *BaseImage )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ConvertPremulAlphaBaseImage( BaseImage );
	DXFUNC_END
	return Result ;
}
extern	int		ConvertInterpAlphaBaseImage( BASEIMAGE *BaseImage )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ConvertInterpAlphaBaseImage( BaseImage );
	DXFUNC_END
	return Result ;
}
extern	int		GetDrawScreenBaseImage( int x1, int y1, int x2, int y2, BASEIMAGE *BaseImage )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawScreenBaseImage(  x1,  y1,  x2,  y2, BaseImage );
	DXFUNC_END
	return Result ;
}
extern	int		GetDrawScreenBaseImageDestPos( int x1, int y1, int x2, int y2, BASEIMAGE *BaseImage, int DestX, int DestY )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDrawScreenBaseImageDestPos(  x1,  y1,  x2,  y2, BaseImage, DestX, DestY ) ;
	DXFUNC_END
	return Result ;
}
extern	int		UpdateLayerdWindowForBaseImage( const BASEIMAGE *BaseImage )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_UpdateLayerdWindowForBaseImage( BaseImage ) ;
	DXFUNC_END
	return Result ;
}
extern	int		UpdateLayerdWindowForBaseImageRect( const BASEIMAGE *BaseImage, int x1, int y1, int x2, int y2 )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_UpdateLayerdWindowForBaseImageRect( BaseImage,  x1,  y1,  x2,  y2 ) ;
	DXFUNC_END
	return Result ;
}
extern	int		UpdateLayerdWindowForPremultipliedAlphaBaseImage( const BASEIMAGE *BaseImage )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_UpdateLayerdWindowForPremultipliedAlphaBaseImage( BaseImage ) ;
	DXFUNC_END
	return Result ;
}
extern	int		UpdateLayerdWindowForPremultipliedAlphaBaseImageRect( const BASEIMAGE *BaseImage, int x1, int y1, int x2, int y2 )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_UpdateLayerdWindowForPremultipliedAlphaBaseImageRect( BaseImage,  x1,  y1,  x2,  y2 ) ;
	DXFUNC_END
	return Result ;
}
extern	int		GetDesktopScreenBaseImage( int x1, int y1, int x2, int y2, BASEIMAGE *BaseImage, int DestX, int DestY )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDesktopScreenBaseImage( x1, y1, x2, y2, BaseImage, DestX, DestY ) ;
	DXFUNC_END
	return Result ;
}
extern	int		FillBaseImage( BASEIMAGE *BaseImage, int r, int g, int b, int a )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_FillBaseImage( BaseImage,  r,  g,  b,  a );
	DXFUNC_END
	return Result ;
}
extern	int		FillRectBaseImage( BASEIMAGE *BaseImage, int x, int y, int w, int h, int r, int g, int b, int a )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_FillRectBaseImage( BaseImage,  x,  y,  w,  h,  r,  g,  b,  a );
	DXFUNC_END
	return Result ;
}
extern	int		ClearRectBaseImage( BASEIMAGE *BaseImage, int x, int y, int w, int h )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ClearRectBaseImage( BaseImage,  x,  y,  w,  h ) ;
	DXFUNC_END
	return Result ;
}

extern	int		GetPaletteBaseImage( const BASEIMAGE *BaseImage, int PaletteNo, int *r, int *g, int *b, int *a )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetPaletteBaseImage( BaseImage,  PaletteNo, r, g, b, a );
	DXFUNC_END
	return Result ;
}

extern	int		SetPaletteBaseImage( BASEIMAGE *BaseImage, int PaletteNo, int  r, int  g, int  b, int  a )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetPaletteBaseImage( BaseImage,  PaletteNo,   r,   g,   b,   a );
	DXFUNC_END
	return Result ;
}
extern	int		SetPixelPalCodeBaseImage( BASEIMAGE *BaseImage, int x, int y, int palNo )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetPixelPalCodeBaseImage( BaseImage,  x,  y,  palNo );
	DXFUNC_END
	return Result ;
}

extern	int		GetPixelPalCodeBaseImage( const BASEIMAGE *BaseImage, int x, int y )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetPixelPalCodeBaseImage( BaseImage,  x,  y );
	DXFUNC_END
	return Result ;
}

extern	int		SetPixelBaseImage( BASEIMAGE *BaseImage, int x, int y, int  r, int  g, int  b, int  a )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetPixelBaseImage( BaseImage,  x,  y,   r,   g,   b,   a );
	DXFUNC_END
	return Result ;
}

extern	int		SetPixelBaseImageF( BASEIMAGE *BaseImage, int x, int y, float  r, float  g, float  b, float  a )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetPixelBaseImageF( BaseImage,  x,  y,   r,   g,   b,   a );
	DXFUNC_END
	return Result ;
}

extern	int		GetPixelBaseImage( const BASEIMAGE *BaseImage, int x, int y, int *r, int *g, int *b, int *a )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetPixelBaseImage( BaseImage,  x,  y,  r,  g,  b,  a );
	DXFUNC_END
	return Result ;
}

extern	int		GetPixelBaseImageF( const BASEIMAGE *BaseImage, int x, int y, float *r, float *g, float *b, float *a )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetPixelBaseImageF( BaseImage,  x,  y,  r,  g,  b,  a );
	DXFUNC_END
	return Result ;
}

extern	int		DrawLineBaseImage( BASEIMAGE *BaseImage, int x1, int y1, int x2, int y2, int r, int g, int b, int a )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawLineBaseImage( BaseImage,  x1,  y1,  x2,  y2,  r,  g,  b,  a );
	DXFUNC_END
	return Result ;
}
extern	int		DrawCircleBaseImage( BASEIMAGE *BaseImage, int x, int y, int radius, int r, int g, int b, int a, int FillFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DrawCircleBaseImage( BaseImage, x, y, radius, r, g, b, a, FillFlag );
	DXFUNC_END
	return Result ;
}
extern	int		BltBaseImage( int SrcX, int SrcY, int SrcSizeX, int SrcSizeY, int DestX, int DestY, BASEIMAGE *SrcBaseImage, BASEIMAGE *DestBaseImage )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_BltBaseImage(  SrcX,  SrcY,  SrcSizeX,  SrcSizeY,  DestX,  DestY, SrcBaseImage, DestBaseImage );
	DXFUNC_END
	return Result ;
}
extern	int		BltBaseImage( int DestX, int DestY, BASEIMAGE *SrcBaseImage, BASEIMAGE *DestBaseImage )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_BltBaseImage(  DestX, DestY, SrcBaseImage, DestBaseImage );
	DXFUNC_END
	return Result ;
}
extern	int		BltBaseImageWithTransColor( int SrcX, int SrcY, int SrcSizeX, int SrcSizeY, int DestX, int DestY, BASEIMAGE *SrcBaseImage, BASEIMAGE *DestBaseImage, int Tr, int Tg, int Tb, int Ta )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_BltBaseImageWithTransColor(  SrcX,  SrcY,  SrcSizeX,  SrcSizeY,  DestX,  DestY, SrcBaseImage, DestBaseImage,  Tr,  Tg,  Tb,  Ta ) ;
	DXFUNC_END
	return Result ;
}
extern	int		BltBaseImageWithAlphaBlend( int SrcX, int SrcY, int SrcSizeX, int SrcSizeY, int DestX, int DestY, BASEIMAGE *SrcBaseImage, BASEIMAGE *DestBaseImage, int Opacity )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_BltBaseImageWithAlphaBlend(  SrcX,  SrcY,  SrcSizeX,  SrcSizeY,  DestX,  DestY, SrcBaseImage, DestBaseImage, Opacity ) ;
	DXFUNC_END
	return Result ;
}
extern	int		ReverseBaseImageH( BASEIMAGE *BaseImage )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReverseBaseImageH( BaseImage ) ;
	DXFUNC_END
	return Result ;
}
extern	int		ReverseBaseImageV( BASEIMAGE *BaseImage )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReverseBaseImageV( BaseImage ) ;
	DXFUNC_END
	return Result ;
}
extern	int		ReverseBaseImage( BASEIMAGE *BaseImage )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReverseBaseImage( BaseImage ) ;
	DXFUNC_END
	return Result ;
}
extern int CheckPixelAlphaBaseImage( const BASEIMAGE *BaseImage )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CheckPixelAlphaBaseImage( BaseImage ) ;
	DXFUNC_END
	return Result ;
}
extern int GetBaseImageUseMaxPaletteNo( const BASEIMAGE *BaseImage )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetBaseImageUseMaxPaletteNo( BaseImage ) ;
	DXFUNC_END
	return Result ;
}

#ifndef DX_NON_JPEGREAD
extern int ReadJpegExif( const TCHAR *JpegFilePath, BYTE *ExifBuffer, size_t ExifBufferSize )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReadJpegExif( JpegFilePath, ExifBuffer, ExifBufferSize ) ;
	DXFUNC_END
	return Result ;
}
extern int ReadJpegExifWithStrLen( const TCHAR *JpegFilePath, size_t JpegFilePathLength, BYTE *ExifBuffer, size_t ExifBufferSize )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReadJpegExifWithStrLen( JpegFilePath, JpegFilePathLength, ExifBuffer, ExifBufferSize ) ;
	DXFUNC_END
	return Result ;
}
#endif // DX_NON_JPEGREAD

#ifndef DX_NON_SAVEFUNCTION

extern int SaveBaseImageToBmp( const TCHAR *FilePath, const BASEIMAGE *BaseImage )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveBaseImageToBmp( FilePath, BaseImage ) ;
	DXFUNC_END
	return Result ;
}
extern int SaveBaseImageToBmpWithStrLen( const TCHAR *FilePath, size_t FilePathLength, const BASEIMAGE *BaseImage )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveBaseImageToBmpWithStrLen( FilePath, FilePathLength, BaseImage ) ;
	DXFUNC_END
	return Result ;
}

extern int SaveBaseImageToDds( const TCHAR *FilePath, const BASEIMAGE *BaseImage )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveBaseImageToDds( FilePath, BaseImage ) ;
	DXFUNC_END
	return Result ;
}
extern int SaveBaseImageToDdsWithStrLen( const TCHAR *FilePath, size_t FilePathLength, const BASEIMAGE *BaseImage )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveBaseImageToDdsWithStrLen( FilePath, FilePathLength, BaseImage ) ;
	DXFUNC_END
	return Result ;
}

#ifndef DX_NON_PNGREAD
extern int SaveBaseImageToPng( const TCHAR *FilePath, BASEIMAGE *BaseImage, int CompressionLevel )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveBaseImageToPng( FilePath, BaseImage, CompressionLevel ) ;
	DXFUNC_END
	return Result ;
}
extern int SaveBaseImageToPngWithStrLen( const TCHAR *FilePath, size_t FilePathLength, BASEIMAGE *BaseImage, int CompressionLevel )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveBaseImageToPngWithStrLen( FilePath, FilePathLength, BaseImage, CompressionLevel ) ;
	DXFUNC_END
	return Result ;
}
#endif

#ifndef DX_NON_JPEGREAD
extern int SaveBaseImageToJpeg( const TCHAR *FilePath, BASEIMAGE *BaseImage, int Quality, int Sample2x1 )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveBaseImageToJpeg( FilePath, BaseImage,  Quality,  Sample2x1 ) ;
	DXFUNC_END
	return Result ;
}
extern int SaveBaseImageToJpegWithStrLen( const TCHAR *FilePath, size_t FilePathLength, BASEIMAGE *BaseImage, int Quality, int Sample2x1 )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveBaseImageToJpegWithStrLen( FilePath, FilePathLength, BaseImage, Quality, Sample2x1 ) ;
	DXFUNC_END
	return Result ;
}
#endif

#endif // DX_NON_SAVEFUNCTION


// カラーマッチングしながらグラフィックデータ間転送を行う Ver2
extern int GraphColorMatchBltVer2( void *DestGraphData, int DestPitch, const COLORDATA *DestColorData,
									const void *SrcGraphData, int SrcPitch, const COLORDATA *SrcColorData,
									const void *AlphaMask, int AlphaPitch, const COLORDATA *AlphaColorData,
									POINT DestPoint, const RECT *SrcRect, int ReverseFlag,
									int TransColorAlphaTestFlag, unsigned int TransColor,
									int ImageShavedMode, int AlphaOnlyFlag, int RedIsAlphaFlag,
									int TransColorNoMoveFlag, int Pal8ColorMatch )
{
	return NS_GraphColorMatchBltVer2( DestGraphData, DestPitch, DestColorData,
									SrcGraphData, SrcPitch, SrcColorData,
									AlphaMask, AlphaPitch, AlphaColorData,
									DestPoint, SrcRect, ReverseFlag,
									TransColorAlphaTestFlag, TransColor,
									ImageShavedMode, AlphaOnlyFlag,
									RedIsAlphaFlag, TransColorNoMoveFlag, Pal8ColorMatch ) ;
}


// 色情報取得関係
extern COLOR_F GetColorF( float Red, float Green, float Blue, float Alpha )
{
	COLOR_F Ret = { Red,  Green,  Blue,  Alpha } ;
	return Ret ;
}
extern COLOR_U8 GetColorU8( int Red, int Green, int Blue, int Alpha )
{
	COLOR_U8 Ret = { ( BYTE )Blue, ( BYTE )Green, ( BYTE )Red, ( BYTE )Alpha } ;
	return Ret ;
}
extern unsigned int GetColor( int Red, int Green, int Blue )
{
	unsigned int Result ;
	DXFUNC_BEGIN
	Result = NS_GetColor( Red, Green, Blue ) ;
	DXFUNC_END
	return Result ;
}
extern int GetColor2( unsigned int Color, int *Red, int *Green, int *Blue )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetColor2( Color, Red, Green, Blue ) ;
	DXFUNC_END
	return Result ;
}
extern unsigned int GetColor3( const COLORDATA * ColorData, int Red, int Green, int Blue, int Alpha )
{
	unsigned int Result ;
	Result = NS_GetColor3( ColorData, Red, Green, Blue, Alpha ) ;
	return Result ;
}
extern unsigned int GetColor4( const COLORDATA * DestColorData, const COLORDATA * SrcColorData, unsigned int SrcColor )
{
	unsigned int Result ;
	Result = NS_GetColor4( DestColorData, SrcColorData, SrcColor ) ;
	return Result ;
}
extern int GetColor5( const COLORDATA * ColorData, unsigned int Color, int *Red, int *Green, int *Blue, int *Alpha  )
{
	int Result ;
	Result = NS_GetColor5( ColorData, Color, Red, Green, Blue, Alpha  ) ;
	return Result ;
}
extern int CreatePaletteColorData( COLORDATA * ColorDataBuf )
{
	int Result ;
	Result = NS_CreatePaletteColorData( ColorDataBuf ) ;
	return Result ;
}
extern int CreateARGBF32ColorData( COLORDATA *ColorDataBuf )
{
	int Result ;
	Result = NS_CreateARGBF32ColorData( ColorDataBuf ) ;
	return Result ;
}
extern int CreateARGBF16ColorData( COLORDATA *ColorDataBuf )
{
	int Result ;
	Result = NS_CreateARGBF16ColorData( ColorDataBuf ) ;
	return Result ;
}
extern int CreateXRGB8ColorData( COLORDATA * ColorDataBuf )
{
	int Result ;
	Result = NS_CreateXRGB8ColorData( ColorDataBuf ) ;
	return Result ;
}
extern int CreateARGB8ColorData( COLORDATA * ColorDataBuf )
{
	int Result ;
	Result = NS_CreateARGB8ColorData( ColorDataBuf ) ;
	return Result ;
}
extern int CreateARGB4ColorData( COLORDATA * ColorDataBuf )
{
	int Result ;
	Result = NS_CreateARGB4ColorData( ColorDataBuf ) ;
	return Result ;
}
extern int CreateA1R5G5B5ColorData( COLORDATA *ColorDataBuf )
{
	int Result ;
	Result = NS_CreateA1R5G5B5ColorData( ColorDataBuf ) ;
	return Result ;
}
extern int CreateX1R5G5B5ColorData( COLORDATA *ColorDataBuf )
{
	int Result ;
	Result = NS_CreateX1R5G5B5ColorData( ColorDataBuf ) ;
	return Result ;
}
extern int CreateR5G5B5A1ColorData( COLORDATA *ColorDataBuf )
{
	int Result ;
	Result = NS_CreateR5G5B5A1ColorData( ColorDataBuf ) ;
	return Result ;
}
extern int CreateR5G6B5ColorData( COLORDATA *ColorDataBuf )
{
	int Result ;
	Result = NS_CreateR5G6B5ColorData( ColorDataBuf ) ;
	return Result ;
}
extern int CreateFullColorData( COLORDATA * ColorDataBuf )
{
	int Result ;
	Result = NS_CreateFullColorData( ColorDataBuf ) ;
	return Result ;
}
extern int CreateGrayColorData( COLORDATA * ColorDataBuf )
{
	int Result ;
	Result = NS_CreateGrayColorData( ColorDataBuf ) ;
	return Result ;
}
extern int CreatePal8ColorData( COLORDATA *ColorDataBuf, int UseAlpha )
{
	int Result ;
	Result = NS_CreatePal8ColorData( ColorDataBuf, UseAlpha ) ;
	return Result ;
}
extern int CreateColorData( COLORDATA * ColorDataBuf, int ColorBitDepth,
								 DWORD RedMask, DWORD GreenMask, DWORD BlueMask, DWORD AlphaMask,
								 int ChannelNum, int ChannelBitDepth, int FloatTypeFlag )
{
	int Result ;
	Result = NS_CreateColorData( ColorDataBuf, ColorBitDepth,
								  RedMask,  GreenMask,  BlueMask,  AlphaMask,
								  ChannelNum, ChannelBitDepth, FloatTypeFlag ) ;
	return Result ;
}

extern void SetColorDataNoneMask( COLORDATA * ColorData )
{
	NS_SetColorDataNoneMask( ColorData ) ;
}
extern int CmpColorData( const COLORDATA * ColorData1, const COLORDATA * ColorData2 )
{
	int Result ;
	Result = NS_CmpColorData( ColorData1, ColorData2 ) ;
	return Result ;
}

























































// DxSound.cpp関数プロトタイプ宣言

#ifndef DX_NON_SOUND

// サウンドデータ管理系関数
extern int InitSoundMem( int LogOutFlag  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_InitSoundMem( LogOutFlag  ) ;
	DXFUNC_END
	return Result ;
}

extern int AddSoundData( int Handle  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_AddSoundData( Handle  ) ;
	DXFUNC_END
	return Result ;
}
extern int AddStreamSoundMem( STREAMDATA *Stream, int LoopNum,  int SoundHandle, int StreamDataType, int *CanStreamCloseFlag, int UnionHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_AddStreamSoundMem( Stream, LoopNum,  SoundHandle, StreamDataType, CanStreamCloseFlag, UnionHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int AddStreamSoundMemToMem( const void *FileImageBuffer, int ImageSize, int LoopNum,  int SoundHandle, int StreamDataType, int UnionHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_AddStreamSoundMemToMem( FileImageBuffer, ImageSize, LoopNum,  SoundHandle, StreamDataType, UnionHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int AddStreamSoundMemToFile( const TCHAR *WaveFile, int LoopNum,  int SoundHandle, int StreamDataType, int UnionHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_AddStreamSoundMemToFile( WaveFile, LoopNum,  SoundHandle, StreamDataType, UnionHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int AddStreamSoundMemToFileWithStrLen( const TCHAR *WaveFile, size_t WaveFilePathLength, int LoopNum,  int SoundHandle, int StreamDataType, int UnionHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_AddStreamSoundMemToFileWithStrLen( WaveFile, WaveFilePathLength, LoopNum,  SoundHandle, StreamDataType, UnionHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetupStreamSoundMem( int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetupStreamSoundMem( SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int PlayStreamSoundMem( int SoundHandle, int PlayType, int TopPositionFlag  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_PlayStreamSoundMem( SoundHandle, PlayType, TopPositionFlag  ) ;
	DXFUNC_END
	return Result ;
}
extern int CheckStreamSoundMem( int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CheckStreamSoundMem( SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int StopStreamSoundMem( int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_StopStreamSoundMem( SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetStreamSoundCurrentPosition( int Byte, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetStreamSoundCurrentPosition( Byte, SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetStreamSoundCurrentPosition( int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetStreamSoundCurrentPosition( SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetStreamSoundCurrentTime( int Time, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetStreamSoundCurrentTime( Time, SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetStreamSoundCurrentTime( int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetStreamSoundCurrentTime( SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int ProcessStreamSoundMem( int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ProcessStreamSoundMem( SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int ProcessStreamSoundMemAll( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ProcessStreamSoundMemAll(  ) ;
	DXFUNC_END
	return Result ;
}


extern int LoadSoundMem2( const TCHAR *WaveName1, const TCHAR *WaveName2 )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadSoundMem2( WaveName1, WaveName2 ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadSoundMem2WithStrLen( const TCHAR *FileName1, size_t FileName1Length, const TCHAR *FileName2, size_t FileName2Length )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadSoundMem2WithStrLen( FileName1, FileName1Length, FileName2, FileName2Length ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadBGM( const TCHAR *WaveName )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadBGM( WaveName ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadBGMWithStrLen( const TCHAR *FileName, size_t FileNameLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadBGMWithStrLen( FileName, FileNameLength ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadSoundMemBase( const TCHAR *WaveName, int BufferNum, int UnionHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadSoundMemBase( WaveName, BufferNum, UnionHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadSoundMemBaseWithStrLen( const TCHAR *FileName, size_t FileNameLength, int BufferNum, int UnionHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadSoundMemBaseWithStrLen( FileName, FileNameLength, BufferNum, UnionHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadSoundMem( const TCHAR *WaveName, int BufferNum, int UnionHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadSoundMem( WaveName, BufferNum, UnionHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadSoundMemWithStrLen( const TCHAR *FileName, size_t FileNameLength, int BufferNum, int UnionHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadSoundMemWithStrLen( FileName, FileNameLength, BufferNum, UnionHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadSoundMemToBufNumSitei( const TCHAR *WaveName, int BufferNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadSoundMemToBufNumSitei( WaveName, BufferNum ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadSoundMemToBufNumSiteiWithStrLen( const TCHAR *FileName, size_t FileNameLength, int BufferNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadSoundMemToBufNumSiteiWithStrLen( FileName, FileNameLength, BufferNum ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadSoundMemByResource( const TCHAR *ResourceName, const TCHAR *ResourceType, int BufferNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadSoundMemByResource( ResourceName, ResourceType, BufferNum ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadSoundMemByResourceWithStrLen( const TCHAR *ResourceName, size_t ResourceNameLength, const TCHAR *ResourceType, size_t ResourceTypeLength, int BufferNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadSoundMemByResourceWithStrLen( ResourceName, ResourceNameLength, ResourceType, ResourceTypeLength, BufferNum ) ;
	DXFUNC_END
	return Result ;
}
extern	int	DuplicateSoundMem( int SrcSoundHandle, int BufferNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DuplicateSoundMem(  SrcSoundHandle,  BufferNum ) ;
	DXFUNC_END
	return Result ;
}

extern int LoadSoundMemByMemImageBase( const void *FileImageBuffer, int ImageSize, int BufferNum, int UnionHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadSoundMemByMemImageBase( FileImageBuffer, ImageSize, BufferNum, UnionHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadSoundMemByMemImage( const void *FileImageBuffer, int ImageSize, int BufferNum, int UnionHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadSoundMemByMemImage( FileImageBuffer, ImageSize, BufferNum, UnionHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadSoundMemByMemImage2( const void *UData, int UDataSize, const WAVEFORMATEX *UFormat, int UHeaderSize )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadSoundMemByMemImage2( UData, UDataSize, UFormat, UHeaderSize ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadSoundMemByMemImageToBufNumSitei( const void *FileImageBuffer, int ImageSize, int BufferNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadSoundMemByMemImageToBufNumSitei( FileImageBuffer, ImageSize, BufferNum ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadSoundMem2ByMemImage( const void *FileImageBuffer1, int ImageSize1, const void *FileImageBuffer2, int ImageSize2 )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadSoundMem2ByMemImage( FileImageBuffer1, ImageSize1, FileImageBuffer2, ImageSize2 ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadSoundMemFromSoftSound( int SoftSoundHandle, int BufferNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadSoundMemFromSoftSound( SoftSoundHandle, BufferNum ) ;
	DXFUNC_END
	return Result ;
}
extern int DeleteSoundMem( int SoundHandle, int LogOutFlag  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DeleteSoundMem( SoundHandle, LogOutFlag  ) ;
	DXFUNC_END
	return Result ;
}

extern int PlaySoundMem( int SoundHandle, int PlayType, int TopPositionFlag  )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_PlaySoundMem( SoundHandle, PlayType, TopPositionFlag  ) ;
	DXFUNC_END
	return Result ;
}
extern int StopSoundMem( int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_StopSoundMem( SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int CheckSoundMem( int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CheckSoundMem( SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetPanSoundMem( int PanPal, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetPanSoundMem( PanPal, SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int ChangePanSoundMem( int PanPal, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ChangePanSoundMem( PanPal, SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetPanSoundMem( int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetPanSoundMem( SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetVolumeSoundMem( int VolumePal, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetVolumeSoundMem( VolumePal, SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int ChangeVolumeSoundMem( int VolumePal, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ChangeVolumeSoundMem( VolumePal, SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetVolumeSoundMem( int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetVolumeSoundMem( SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetVolumeSoundMem2( int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetVolumeSoundMem2( SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetChannelVolumeSoundMem( int Channel, int VolumePal, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetChannelVolumeSoundMem(  Channel,  VolumePal,  SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int ChangeChannelVolumeSoundMem( int Channel, int VolumePal, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ChangeChannelVolumeSoundMem(  Channel,  VolumePal,  SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetChannelVolumeSoundMem( int Channel, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetChannelVolumeSoundMem(  Channel,  SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetChannelVolumeSoundMem2( int Channel, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetChannelVolumeSoundMem2(  Channel,  SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetFrequencySoundMem( int FrequencyPal, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetFrequencySoundMem( FrequencyPal, SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetFrequencySoundMem( int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetFrequencySoundMem( SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int ResetFrequencySoundMem( int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ResetFrequencySoundMem( SoundHandle ) ;
	DXFUNC_END
	return Result ;
}




extern int SetNextPlayPanSoundMem( int PanPal, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetNextPlayPanSoundMem(  PanPal,  SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int ChangeNextPlayPanSoundMem( int PanPal, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ChangeNextPlayPanSoundMem(  PanPal,  SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetNextPlayVolumeSoundMem( int VolumePal, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetNextPlayVolumeSoundMem(  VolumePal,  SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int ChangeNextPlayVolumeSoundMem( int VolumePal, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ChangeNextPlayVolumeSoundMem(  VolumePal,  SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetNextPlayChannelVolumeSoundMem( int Channel, int VolumePal, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetNextPlayChannelVolumeSoundMem(  Channel,  VolumePal,  SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int ChangeNextPlayChannelVolumeSoundMem( int Channel, int VolumePal, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ChangeNextPlayChannelVolumeSoundMem(  Channel,  VolumePal,  SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetNextPlayFrequencySoundMem( int FrequencyPal, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetNextPlayFrequencySoundMem(  FrequencyPal,  SoundHandle ) ;
	DXFUNC_END
	return Result ;
}



extern int SetCurrentPositionSoundMem( int SamplePosition, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetCurrentPositionSoundMem(  SamplePosition,  SoundHandle ) ;
	DXFUNC_END
	return Result ;
}

extern int GetCurrentPositionSoundMem( int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetCurrentPositionSoundMem(  SoundHandle ) ;
	DXFUNC_END
	return Result ;
}

extern int SetSoundCurrentPosition( int Byte, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetSoundCurrentPosition( Byte, SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetSoundCurrentPosition( int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetSoundCurrentPosition( SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetSoundCurrentTime( int Time, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetSoundCurrentTime( Time, SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetSoundCurrentTime( int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetSoundCurrentTime( SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetSoundTotalSample( int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetSoundTotalSample( SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetSoundTotalTime( int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetSoundTotalTime( SoundHandle ) ;
	DXFUNC_END
	return Result ;
}

extern int SetLoopPosSoundMem( int LoopTime, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetLoopPosSoundMem( LoopTime, SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetLoopTimePosSoundMem( int LoopTime, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetLoopTimePosSoundMem( LoopTime, SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetLoopSamplePosSoundMem( int LoopSamplePosition, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetLoopSamplePosSoundMem( LoopSamplePosition, SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetLoopStartTimePosSoundMem( int LoopStartTime, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetLoopStartTimePosSoundMem(  LoopStartTime,  SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetLoopStartSamplePosSoundMem( int LoopStartSamplePosition, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetLoopStartSamplePosSoundMem(  LoopStartSamplePosition,  SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetLoopAreaTimePosSoundMem( int LoopStartTime, int LoopEndTime, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetLoopAreaTimePosSoundMem( LoopStartTime, LoopEndTime, SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetLoopAreaTimePosSoundMem( int *LoopStartTime, int *LoopEndTime, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetLoopAreaTimePosSoundMem( LoopStartTime, LoopEndTime, SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetLoopAreaSamplePosSoundMem( int LoopStartSamplePosition, int LoopEndSamplePosition, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetLoopAreaSamplePosSoundMem( LoopStartSamplePosition, LoopEndSamplePosition, SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetLoopAreaSamplePosSoundMem( int *LoopStartSamplePosition, int *LoopEndSamplePosition, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetLoopAreaSamplePosSoundMem( LoopStartSamplePosition, LoopEndSamplePosition, SoundHandle ) ;
	DXFUNC_END
	return Result ;
}

extern int SetPlayFinishDeleteSoundMem( int DeleteFlag, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetPlayFinishDeleteSoundMem( DeleteFlag, SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int Set3DReverbParamSoundMem( const SOUND3D_REVERB_PARAM *Param, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_Set3DReverbParamSoundMem( Param, SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int Set3DPresetReverbParamSoundMem( int PresetNo /* DX_REVERB_PRESET_DEFAULT 等 */ , int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_Set3DPresetReverbParamSoundMem( PresetNo, SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int Set3DReverbParamSoundMemAll( const SOUND3D_REVERB_PARAM *Param, int PlaySoundOnly )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_Set3DReverbParamSoundMemAll( Param, PlaySoundOnly ) ;
	DXFUNC_END
	return Result ;
}
extern int Set3DPresetReverbParamSoundMemAll( int PresetNo /* DX_REVERB_PRESET_DEFAULT 等 */, int PlaySoundOnly )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_Set3DPresetReverbParamSoundMemAll( PresetNo, PlaySoundOnly ) ;
	DXFUNC_END
	return Result ;
}
extern int Get3DReverbParamSoundMem( SOUND3D_REVERB_PARAM *ParamBuffer, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_Get3DReverbParamSoundMem( ParamBuffer, SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int Get3DPresetReverbParamSoundMem( SOUND3D_REVERB_PARAM *ParamBuffer, int PresetNo /* DX_REVERB_PRESET_DEFAULT 等 */ )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_Get3DPresetReverbParamSoundMem( ParamBuffer, PresetNo ) ;
	DXFUNC_END
	return Result ;
}

extern int Set3DPositionSoundMem( VECTOR Position, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_Set3DPositionSoundMem( Position, SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int 	Set3DRadiusSoundMem( float Radius, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_Set3DRadiusSoundMem( Radius, SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
/*
extern int Set3DInnerRadiusSoundMem( float Radius, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_Set3DInnerRadiusSoundMem( Radius, SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
*/
extern int Set3DVelocitySoundMem( VECTOR Velocity, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_Set3DVelocitySoundMem( Velocity, SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
/*
extern int Set3DFrontPosition_UpVecYSoundMem( VECTOR FrontPosition, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_Set3DFrontPosition_UpVecYSoundMem( FrontPosition, SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int Set3DFrontPositionSoundMem( VECTOR FrontPosition, VECTOR UpVector, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_Set3DFrontPositionSoundMem( FrontPosition, UpVector, SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int Set3DConeAngleSoundMem( float InnerAngle, float OuterAngle, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_Set3DConeAngleSoundMem( InnerAngle, OuterAngle, SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int Set3DConeVolumeSoundMem( float InnerAngleVolume, float OuterAngleVolume, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_Set3DConeVolumeSoundMem( InnerAngleVolume, OuterAngleVolume, SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
*/

extern int SetNextPlay3DPositionSoundMem( VECTOR Position, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetNextPlay3DPositionSoundMem( Position, SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetNextPlay3DRadiusSoundMem( float Radius, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetNextPlay3DRadiusSoundMem( Radius, SoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetNextPlay3DVelocitySoundMem( VECTOR Velocity, int SoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetNextPlay3DVelocitySoundMem( Velocity, SoundHandle ) ;
	DXFUNC_END
	return Result ;
}


// 特殊関数
extern int GetMP3TagInfo( const TCHAR *FileName, TCHAR *TitleBuffer, size_t TitleBufferBytes, TCHAR *ArtistBuffer, size_t ArtistBufferBytes, TCHAR *AlbumBuffer, size_t AlbumBufferBytes, TCHAR *YearBuffer, size_t YearBufferBytes, TCHAR *CommentBuffer, size_t CommentBufferBytes, TCHAR *TrackBuffer, size_t TrackBufferBytes, TCHAR *GenreBuffer, size_t GenreBufferBytes, int *PictureGrHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetMP3TagInfo( FileName, TitleBuffer, TitleBufferBytes, ArtistBuffer, ArtistBufferBytes, AlbumBuffer, AlbumBufferBytes, YearBuffer, YearBufferBytes, CommentBuffer, CommentBufferBytes, TrackBuffer, TrackBufferBytes, GenreBuffer, GenreBufferBytes, PictureGrHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetMP3TagInfoWithStrLen( const TCHAR *FileName, size_t FileNameLength, TCHAR *TitleBuffer, size_t TitleBufferBytes, TCHAR *ArtistBuffer, size_t ArtistBufferBytes, TCHAR *AlbumBuffer, size_t AlbumBufferBytes, TCHAR *YearBuffer, size_t YearBufferBytes, TCHAR *CommentBuffer, size_t CommentBufferBytes, TCHAR *TrackBuffer, size_t TrackBufferBytes, TCHAR *GenreBuffer, size_t GenreBufferBytes, int *PictureGrHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetMP3TagInfoWithStrLen( FileName, FileNameLength, TitleBuffer, TitleBufferBytes, ArtistBuffer, ArtistBufferBytes, AlbumBuffer, AlbumBufferBytes, YearBuffer, YearBufferBytes, CommentBuffer, CommentBufferBytes, TrackBuffer, TrackBufferBytes, GenreBuffer, GenreBufferBytes, PictureGrHandle ) ;
	DXFUNC_END
	return Result ;
}


// 設定関係関数
extern int SetCreateSoundDataType( int SoundDataType )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetCreateSoundDataType( SoundDataType ) ;
	DXFUNC_END
	return Result ;
}
extern	int GetCreateSoundDataType( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetCreateSoundDataType() ;
	DXFUNC_END
	return Result ;
}
extern	int SetCreateSoundPitchRate( float Cents )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetCreateSoundPitchRate( Cents ) ;
	DXFUNC_END
	return Result ;
}
extern	float GetCreateSoundPitchRate( void )
{
	float Result ;
	DXFUNC_BEGIN
	Result = NS_GetCreateSoundPitchRate() ;
	DXFUNC_END
	return Result ;
}
extern	int SetCreateSoundTimeStretchRate( float Rate )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetCreateSoundTimeStretchRate( Rate ) ;
	DXFUNC_END
	return Result ;
}
extern	float GetCreateSoundTimeStretchRate( void )
{
	float Result ;
	DXFUNC_BEGIN
	Result = NS_GetCreateSoundTimeStretchRate() ;
	DXFUNC_END
	return Result ;
}
extern	int	SetCreateSoundLoopAreaTimePos( int  LoopStartTime, int  LoopEndTime )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetCreateSoundLoopAreaTimePos(   LoopStartTime,   LoopEndTime ) ;
	DXFUNC_END
	return Result ;
}
extern	int	GetCreateSoundLoopAreaTimePos( int *LoopStartTime, int *LoopEndTime )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetCreateSoundLoopAreaTimePos( LoopStartTime, LoopEndTime ) ;
	DXFUNC_END
	return Result ;
}
extern	int	SetCreateSoundLoopAreaSamplePos( int  LoopStartSamplePosition, int  LoopEndSamplePosition )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetCreateSoundLoopAreaSamplePos(   LoopStartSamplePosition,   LoopEndSamplePosition ) ;
	DXFUNC_END
	return Result ;
}
extern	int	GetCreateSoundLoopAreaSamplePos( int *LoopStartSamplePosition, int *LoopEndSamplePosition )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetCreateSoundLoopAreaSamplePos( LoopStartSamplePosition, LoopEndSamplePosition ) ;
	DXFUNC_END
	return Result ;
}
extern	int	SetCreateSoundIgnoreLoopAreaInfo( int IgnoreFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetCreateSoundIgnoreLoopAreaInfo( IgnoreFlag ) ;
	DXFUNC_END
	return Result ;
}
extern	int	GetCreateSoundIgnoreLoopAreaInfo( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetCreateSoundIgnoreLoopAreaInfo() ;
	DXFUNC_END
	return Result ;
}
extern	int	SetDisableReadSoundFunctionMask( int Mask )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetDisableReadSoundFunctionMask( Mask ) ;
	DXFUNC_END
	return Result ;
}
extern	int	GetDisableReadSoundFunctionMask( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetDisableReadSoundFunctionMask() ;
	DXFUNC_END
	return Result ;
}
extern	int SetEnableSoundCaptureFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetEnableSoundCaptureFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern	int SetUseSoftwareMixingSoundFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseSoftwareMixingSoundFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern	int SetEnableXAudioFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetEnableXAudioFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern	int SetUseOldVolumeCalcFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetUseOldVolumeCalcFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern	int SetCreate3DSoundFlag( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetCreate3DSoundFlag( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern	int	Set3DSoundOneMetre( float Distance )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_Set3DSoundOneMetre( Distance ) ;
	DXFUNC_END
	return Result ;
}
extern int Set3DSoundListenerPosAndFrontPos_UpVecY( VECTOR Position, VECTOR FrontPosition )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_Set3DSoundListenerPosAndFrontPos_UpVecY( Position, FrontPosition ) ;
	DXFUNC_END
	return Result ;
}
extern int Set3DSoundListenerPosAndFrontPosAndUpVec( VECTOR Position, VECTOR FrontPosition, VECTOR UpVector )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_Set3DSoundListenerPosAndFrontPosAndUpVec( Position, FrontPosition, UpVector ) ;
	DXFUNC_END
	return Result ;
}
extern int Set3DSoundListenerVelocity( VECTOR Velocity )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_Set3DSoundListenerVelocity( Velocity ) ;
	DXFUNC_END
	return Result ;
}
extern int Set3DSoundListenerConeAngle( float InnerAngle, float OuterAngle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_Set3DSoundListenerConeAngle( InnerAngle, OuterAngle ) ;
	DXFUNC_END
	return Result ;
}
extern int Set3DSoundListenerConeVolume( float InnerAngleVolume, float OuterAngleVolume )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_Set3DSoundListenerConeVolume( InnerAngleVolume, OuterAngleVolume ) ;
	DXFUNC_END
	return Result ;
}

// 情報取得系関数
extern const void *GetDSoundObj( void )
{
	const void *Result ;
	DXFUNC_BEGIN
	Result = NS_GetDSoundObj() ;
	DXFUNC_END
	return Result ;
}

#ifndef DX_NON_BEEP
// BEEP音再生用命令
extern int SetBeepFrequency( int Freq )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetBeepFrequency( Freq ) ;
	DXFUNC_END
	return Result ;
}
extern int PlayBeep( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_PlayBeep(  ) ;
	DXFUNC_END
	return Result ;
}
extern int StopBeep( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_StopBeep(  ) ;
	DXFUNC_END
	return Result ;
}
#endif

// ラッパー関数
extern int PlaySoundFile( const TCHAR *FileName, int PlayType )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_PlaySoundFile( FileName, PlayType ) ;
	DXFUNC_END
	return Result ;
}
extern int PlaySoundFileWithStrLen(	const TCHAR *FileName, size_t FileNameLength, int PlayType )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_PlaySoundFileWithStrLen( FileName, FileNameLength, PlayType ) ;
	DXFUNC_END
	return Result ;
}
extern int PlaySound( const TCHAR *FileName, int PlayType )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_PlaySound( FileName, PlayType ) ;
	DXFUNC_END
	return Result ;
}
extern int PlaySoundWithStrLen( const TCHAR *FileName, size_t FileNameLength, int PlayType )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_PlaySoundWithStrLen( FileName, FileNameLength, PlayType ) ;
	DXFUNC_END
	return Result ;
}
extern int CheckSoundFile( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CheckSoundFile( ) ;
	DXFUNC_END
	return Result ;
}
extern int CheckSound( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CheckSound( ) ;
	DXFUNC_END
	return Result ;
}
extern int StopSoundFile( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_StopSoundFile( ) ;
	DXFUNC_END
	return Result ;
}
extern int StopSound( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_StopSound( ) ;
	DXFUNC_END
	return Result ;
}
extern int SetVolumeSoundFile( int VolumePal )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetVolumeSoundFile( VolumePal ) ;
	DXFUNC_END
	return Result ;
}
extern int SetVolumeSound( int VolumePal )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetVolumeSound( VolumePal ) ;
	DXFUNC_END
	return Result ;
}





// ソフトウエア制御サウンド系関数
extern	int			InitSoftSound( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_InitSoftSound( ) ;
	DXFUNC_END
	return Result ;
}
extern	int			LoadSoftSound( const TCHAR *FileName )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadSoftSound( FileName ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadSoftSoundWithStrLen(	const TCHAR *FileName, size_t FileNameLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadSoftSoundWithStrLen( FileName, FileNameLength ) ;
	DXFUNC_END
	return Result ;
}
extern	int			LoadSoftSoundFromMemImage( const void *FileImageBuffer, int FileImageSize )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadSoftSoundFromMemImage( FileImageBuffer, FileImageSize ) ;
	DXFUNC_END
	return Result ;
}
extern	int			MakeSoftSound( int UseFormat_SoftSoundHandle, int SampleNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MakeSoftSound(  UseFormat_SoftSoundHandle,  SampleNum ) ;
	DXFUNC_END
	return Result ;
}
extern	int			MakeSoftSound2Ch16Bit44KHz( int SampleNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MakeSoftSound2Ch16Bit44KHz( SampleNum ) ;
	DXFUNC_END
	return Result ;
}
extern	int			MakeSoftSound2Ch16Bit22KHz( int SampleNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MakeSoftSound2Ch16Bit22KHz( SampleNum ) ;
	DXFUNC_END
	return Result ;
}
extern	int			MakeSoftSound2Ch8Bit44KHz( int SampleNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MakeSoftSound2Ch8Bit44KHz( SampleNum ) ;
	DXFUNC_END
	return Result ;
}
extern	int			MakeSoftSound2Ch8Bit22KHz( int SampleNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MakeSoftSound2Ch8Bit22KHz( SampleNum ) ;
	DXFUNC_END
	return Result ;
}
extern	int			MakeSoftSound1Ch16Bit44KHz( int SampleNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MakeSoftSound1Ch16Bit44KHz( SampleNum ) ;
	DXFUNC_END
	return Result ;
}
extern	int			MakeSoftSound1Ch16Bit22KHz( int SampleNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MakeSoftSound1Ch16Bit22KHz(  SampleNum ) ;
	DXFUNC_END
	return Result ;
}
extern	int			MakeSoftSound1Ch8Bit44KHz( int SampleNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MakeSoftSound1Ch8Bit44KHz( SampleNum ) ;
	DXFUNC_END
	return Result ;
}
extern	int			MakeSoftSound1Ch8Bit22KHz( int SampleNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MakeSoftSound1Ch8Bit22KHz( SampleNum ) ;
	DXFUNC_END
	return Result ;
}
extern	int			MakeSoftSoundCustom( int ChannelNum, int BitsPerSample, int SamplesPerSec, int SampleNum, int IsFloatType )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MakeSoftSoundCustom(  ChannelNum,  BitsPerSample,  SamplesPerSec,  SampleNum, IsFloatType ) ;
	DXFUNC_END
	return Result ;
}
extern	int			DeleteSoftSound( int SoftSoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DeleteSoftSound( SoftSoundHandle ) ;
	DXFUNC_END
	return Result ;
}

#ifndef DX_NON_SAVEFUNCTION

extern	int			SaveSoftSound( int SoftSoundHandle, const TCHAR *FileName )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveSoftSound( SoftSoundHandle, FileName ) ;
	DXFUNC_END
	return Result ;
}
extern int SaveSoftSoundWithStrLen(	int SoftSoundHandle, const TCHAR *FileName, size_t FileNameLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SaveSoftSoundWithStrLen( SoftSoundHandle, FileName, FileNameLength ) ;
	DXFUNC_END
	return Result ;
}

#endif // DX_NON_SAVEFUNCTION

extern	int			GetSoftSoundSampleNum( int SoftSoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetSoftSoundSampleNum( SoftSoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern	int			GetSoftSoundFormat( int SoftSoundHandle, int *Channels, int *BitsPerSample, int *SamplesPerSec, int *IsFloatType )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetSoftSoundFormat(  SoftSoundHandle, Channels, BitsPerSample, SamplesPerSec, IsFloatType ) ;
	DXFUNC_END
	return Result ;
}
extern	int			ReadSoftSoundData( int SoftSoundHandle, int SamplePosition, int *Channel1, int *Channel2 )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReadSoftSoundData(  SoftSoundHandle,  SamplePosition, Channel1, Channel2 ) ;
	DXFUNC_END
	return Result ;
}
extern	int			ReadSoftSoundDataF( int SoftSoundHandle, int SamplePosition, float *Channel1, float *Channel2 )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ReadSoftSoundDataF(  SoftSoundHandle,  SamplePosition, Channel1, Channel2 ) ;
	DXFUNC_END
	return Result ;
}
extern	int			WriteSoftSoundData( int SoftSoundHandle, int SamplePosition, int Channel1, int Channel2 )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_WriteSoftSoundData(  SoftSoundHandle,  SamplePosition,  Channel1,  Channel2 ) ;
	DXFUNC_END
	return Result ;
}
extern	int			WriteSoftSoundDataF( int SoftSoundHandle, int SamplePosition, float Channel1, float Channel2 )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_WriteSoftSoundDataF(  SoftSoundHandle,  SamplePosition,  Channel1,  Channel2 ) ;
	DXFUNC_END
	return Result ;
}
extern	int			WriteTimeStretchSoftSoundData( int SrcSoftSoundHandle, int DestSoftSoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_WriteTimeStretchSoftSoundData( SrcSoftSoundHandle, DestSoftSoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern	int			WritePitchShiftSoftSoundData( int SrcSoftSoundHandle, int DestSoftSoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_WritePitchShiftSoftSoundData( SrcSoftSoundHandle, DestSoftSoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern	void		*GetSoftSoundDataImage( int SoftSoundHandle )
{
	void *Result ;
	DXFUNC_BEGIN
	Result = NS_GetSoftSoundDataImage( SoftSoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int			GetFFTVibrationSoftSound( int SoftSoundHandle, int Channel, int SamplePosition, int SampleNum, float *Buffer, int BufferLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetFFTVibrationSoftSound( SoftSoundHandle, Channel, SamplePosition, SampleNum, Buffer, BufferLength ) ;
	DXFUNC_END
	return Result ;
}
extern	int			GetFFTVibrationSoftSoundBase( int SoftSoundHandle, int Channel, int SamplePosition, int SampleNum, float *RealBuffer_Array, float *ImagBuffer_Array, int BufferLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetFFTVibrationSoftSoundBase( SoftSoundHandle, Channel, SamplePosition, SampleNum, RealBuffer_Array, ImagBuffer_Array, BufferLength ) ;
	DXFUNC_END
	return Result ;
}

extern	int			InitSoftSoundPlayer( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_InitSoftSoundPlayer( ) ;
	DXFUNC_END
	return Result ;
}
extern	int			MakeSoftSoundPlayer( int UseFormat_SoftSoundHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MakeSoftSoundPlayer( UseFormat_SoftSoundHandle ) ;
	DXFUNC_END
	return Result ;
}
extern	int			MakeSoftSoundPlayer2Ch16Bit44KHz( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MakeSoftSoundPlayer2Ch16Bit44KHz( ) ;
	DXFUNC_END
	return Result ;
}
extern	int			MakeSoftSoundPlayer2Ch16Bit22KHz( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MakeSoftSoundPlayer2Ch16Bit22KHz(  ) ;
	DXFUNC_END
	return Result ;
}
extern	int			MakeSoftSoundPlayer2Ch8Bit44KHz( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MakeSoftSoundPlayer2Ch8Bit44KHz(  ) ;
	DXFUNC_END
	return Result ;
}
extern	int			MakeSoftSoundPlayer2Ch8Bit22KHz( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MakeSoftSoundPlayer2Ch8Bit22KHz( ) ;
	DXFUNC_END
	return Result ;
}
extern	int			MakeSoftSoundPlayer1Ch16Bit44KHz( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MakeSoftSoundPlayer1Ch16Bit44KHz(  ) ;
	DXFUNC_END
	return Result ;
}
extern	int			MakeSoftSoundPlayer1Ch16Bit22KHz( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MakeSoftSoundPlayer1Ch16Bit22KHz(  ) ;
	DXFUNC_END
	return Result ;
}
extern	int			MakeSoftSoundPlayer1Ch8Bit44KHz( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MakeSoftSoundPlayer1Ch8Bit44KHz(  ) ;
	DXFUNC_END
	return Result ;
}
extern	int			MakeSoftSoundPlayer1Ch8Bit22KHz( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MakeSoftSoundPlayer1Ch8Bit22KHz(  ) ;
	DXFUNC_END
	return Result ;
}
extern	int			MakeSoftSoundPlayerCustom( int ChannelNum, int BitsPerSample, int SamplesPerSec )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MakeSoftSoundPlayerCustom(  ChannelNum,  BitsPerSample,  SamplesPerSec ) ;
	DXFUNC_END
	return Result ;
}
extern	int			DeleteSoftSoundPlayer( int SSoundPlayerHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DeleteSoftSoundPlayer(  SSoundPlayerHandle ) ;
	DXFUNC_END
	return Result ;
}
extern	int			AddDataSoftSoundPlayer( int SSoundPlayerHandle, int SoftSoundHandle, int AddSamplePosition, int AddSampleNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_AddDataSoftSoundPlayer(  SSoundPlayerHandle,  SoftSoundHandle,  AddSamplePosition,  AddSampleNum ) ;
	DXFUNC_END
	return Result ;
}
extern	int			AddDirectDataSoftSoundPlayer( int SSoundPlayerHandle, const void *SoundData, int AddSampleNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_AddDirectDataSoftSoundPlayer(  SSoundPlayerHandle, SoundData,  AddSampleNum ) ;
	DXFUNC_END
	return Result ;
}
extern	int			AddOneDataSoftSoundPlayer( int SSoundPlayerHandle, int Channel1, int Channel2 )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_AddOneDataSoftSoundPlayer(  SSoundPlayerHandle,  Channel1,  Channel2 ) ;
	DXFUNC_END
	return Result ;
}
extern	int			GetSoftSoundPlayerFormat( int SSoundPlayerHandle, int *Channels, int *BitsPerSample, int *SamplesPerSec )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetSoftSoundPlayerFormat( SSoundPlayerHandle, Channels, BitsPerSample, SamplesPerSec ) ;
	DXFUNC_END
	return Result ;
}
extern	int			StartSoftSoundPlayer( int SSoundPlayerHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_StartSoftSoundPlayer(  SSoundPlayerHandle ) ;
	DXFUNC_END
	return Result ;
}
extern	int			CheckStartSoftSoundPlayer( int SSoundPlayerHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CheckStartSoftSoundPlayer(  SSoundPlayerHandle ) ;
	DXFUNC_END
	return Result ;
}
extern	int			StopSoftSoundPlayer( int SSoundPlayerHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_StopSoftSoundPlayer(  SSoundPlayerHandle ) ;
	DXFUNC_END
	return Result ;
}
extern	int			ResetSoftSoundPlayer( int SSoundPlayerHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ResetSoftSoundPlayer(  SSoundPlayerHandle ) ;
	DXFUNC_END
	return Result ;
}
extern	int			GetStockDataLengthSoftSoundPlayer( int SSoundPlayerHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetStockDataLengthSoftSoundPlayer( SSoundPlayerHandle ) ;
	DXFUNC_END
	return Result ;
}
extern	int			CheckSoftSoundPlayerNoneData( int SSoundPlayerHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CheckSoftSoundPlayerNoneData( SSoundPlayerHandle ) ;
	DXFUNC_END
	return Result ;
}




// ＭＩＤＩ制御関数
extern int DeleteMusicMem( int MusicHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DeleteMusicMem( MusicHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadMusicMem( const TCHAR *FileName )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadMusicMem( FileName ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadMusicMemWithStrLen( const TCHAR *FileName, size_t FileNameLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadMusicMemWithStrLen( FileName, FileNameLength ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadMusicMemByMemImage( const void *FileImageBuffer, int FileImageSize )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadMusicMemByMemImage( FileImageBuffer, FileImageSize ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadMusicMemByResource( const TCHAR *ResourceName, const TCHAR *ResourceType )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadMusicMemByResource( ResourceName, ResourceType ) ;
	DXFUNC_END
	return Result ;
}
extern int LoadMusicMemByResourceWithStrLen( const TCHAR *ResourceName, size_t ResourceNameLength, const TCHAR *ResourceType, size_t ResourceTypeLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_LoadMusicMemByResourceWithStrLen( ResourceName, ResourceNameLength, ResourceType, ResourceTypeLength ) ;
	DXFUNC_END
	return Result ;
}
extern int PlayMusicMem( int MusicHandle, int PlayType )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_PlayMusicMem( MusicHandle, PlayType ) ;
	DXFUNC_END
	return Result ;
}
extern int StopMusicMem( int MusicHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_StopMusicMem( MusicHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int CheckMusicMem( int MusicHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CheckMusicMem( MusicHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int SetVolumeMusicMem( int Volume, int MusicHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetVolumeMusicMem( Volume, MusicHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int GetMusicMemPosition( int MusicHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetMusicMemPosition( MusicHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int InitMusicMem( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_InitMusicMem(  ) ;
	DXFUNC_END
	return Result ;
}
extern int ProcessMusicMem( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_ProcessMusicMem(  ) ;
	DXFUNC_END
	return Result ;
}

extern int PlayMusic( const TCHAR *FileName, int PlayType )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_PlayMusic( FileName, PlayType ) ;
	DXFUNC_END
	return Result ;
}
extern int PlayMusicWithStrLen( const TCHAR *FileName, size_t FileNameLength, int PlayType )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_PlayMusicWithStrLen( FileName, FileNameLength, PlayType ) ;
	DXFUNC_END
	return Result ;
}
extern int PlayMusicByMemImage( const void *FileImageBuffer, int FileImageSize, int PlayType )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_PlayMusicByMemImage( FileImageBuffer, FileImageSize, PlayType ) ;
	DXFUNC_END
	return Result ;
}
extern int PlayMusicByResource( const TCHAR *ResourceName, const TCHAR *ResourceType, int PlayType )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_PlayMusicByResource( ResourceName, ResourceType, PlayType ) ;
	DXFUNC_END
	return Result ;
}
extern int PlayMusicByResourceWithStrLen( const TCHAR *ResourceName, size_t ResourceNameLength, const TCHAR *ResourceType, size_t ResourceTypeLength, int PlayType )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_PlayMusicByResourceWithStrLen( ResourceName, ResourceNameLength, ResourceType, ResourceTypeLength, PlayType ) ;
	DXFUNC_END
	return Result ;
}
extern int SetVolumeMusic( int Volume )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SetVolumeMusic( Volume ) ;
	DXFUNC_END
	return Result ;
}
extern int StopMusic( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_StopMusic(  ) ;
	DXFUNC_END
	return Result ;
}
extern int CheckMusic( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_CheckMusic( ) ;
	DXFUNC_END
	return Result ;
}
extern int GetMusicPosition( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_GetMusicPosition(  ) ;
	DXFUNC_END
	return Result ;
}

extern int SelectMidiMode( int Mode )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_SelectMidiMode( Mode ) ;
	DXFUNC_END
	return Result ;
}

#endif // DX_NON_SOUND









// DxArchive.cpp 関数

extern int DXArchivePreLoad( const TCHAR *FilePath , int ASync )
{
#ifndef DX_NON_DXA
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DXArchivePreLoad( FilePath , ASync ) ;
	DXFUNC_END
	return Result ;
#else
	return -1;
#endif
}
extern int DXArchivePreLoadWithStrLen( const TCHAR *FilePath, size_t FilePathLength, int ASync )
{
#ifndef DX_NON_DXA
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DXArchivePreLoadWithStrLen( FilePath, FilePathLength, ASync ) ;
	DXFUNC_END
	return Result ;
#else
	return -1;
#endif
}
extern int DXArchiveCheckIdle( const TCHAR *FilePath )
{
#ifndef DX_NON_DXA
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DXArchiveCheckIdle( FilePath ) ;
	DXFUNC_END
	return Result ;
#else
	return -1;
#endif
}
extern int DXArchiveCheckIdleWithStrLen( const TCHAR *FilePath, size_t FilePathLength )
{
#ifndef DX_NON_DXA
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DXArchiveCheckIdleWithStrLen( FilePath, FilePathLength ) ;
	DXFUNC_END
	return Result ;
#else
	return -1;
#endif
}

extern int DXArchiveRelease( const TCHAR *FilePath )
{
#ifndef DX_NON_DXA
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DXArchiveRelease( FilePath ) ;
	DXFUNC_END
	return Result ;
#else
	return -1;
#endif
}
extern int DXArchiveReleaseWithStrLen( const TCHAR *FilePath, size_t FilePathLength )
{
#ifndef DX_NON_DXA
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DXArchiveReleaseWithStrLen( FilePath, FilePathLength ) ;
	DXFUNC_END
	return Result ;
#else
	return -1;
#endif
}

extern int DXArchiveCheckFile( const TCHAR *FilePath, const TCHAR *TargetFilePath )
{
#ifndef DX_NON_DXA
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DXArchiveCheckFile( FilePath, TargetFilePath ) ;
	DXFUNC_END
	return Result ;
#else
	return -1;
#endif
}
extern int DXArchiveCheckFileWithStrLen( const TCHAR *FilePath, size_t FilePathLength, const TCHAR *TargetFilePath, size_t TargetFilePathLength )
{
#ifndef DX_NON_DXA
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DXArchiveCheckFileWithStrLen( FilePath, FilePathLength, TargetFilePath, TargetFilePathLength ) ;
	DXFUNC_END
	return Result ;
#else
	return -1;
#endif
}

// メモリ上に展開されたＤＸＡファイルを指定のファイルパスにあることにする
extern int DXArchiveSetMemImage( void *ArchiveImage, int ArchiveImageSize, const TCHAR *EmulateFilePath, int ArchiveImageCopyFlag, int ArchiveImageReadOnly )
{
#ifndef DX_NON_DXA
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DXArchiveSetMemImage( ArchiveImage, ArchiveImageSize, EmulateFilePath, ArchiveImageCopyFlag, ArchiveImageReadOnly ) ;
	DXFUNC_END
	return Result ;
#else
	return -1;
#endif
}
extern int DXArchiveSetMemImageWithStrLen( void *ArchiveImage, int ArchiveImageSize, const TCHAR *EmulateFilePath, size_t EmulateFilePathLength, int ArchiveImageCopyFlag , int ArchiveImageReadOnly )
{
#ifndef DX_NON_DXA
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DXArchiveSetMemImageWithStrLen( ArchiveImage, ArchiveImageSize, EmulateFilePath, EmulateFilePathLength, ArchiveImageCopyFlag , ArchiveImageReadOnly ) ;
	DXFUNC_END
	return Result ;
#else
	return -1;
#endif
}

// DXArchiveSetMemImage の設定を解除する
extern int DXArchiveReleaseMemImage( void *ArchiveImage )
{
#ifndef DX_NON_DXA
	int Result ;
	DXFUNC_BEGIN
	Result = NS_DXArchiveReleaseMemImage( ArchiveImage ) ;
	DXFUNC_END
	return Result ;
#else
	return -1;
#endif
}
















// DxModel.cpp 関数

#ifndef DX_NON_MODEL

// モデルの読み込み( -1:エラー  0以上:モデルハンドル )
extern int MV1LoadModel( const TCHAR *FileName )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1LoadModel( FileName ) ;
	DXFUNC_END
	return Result ;
}
extern int MV1LoadModelWithStrLen( const TCHAR *FileName, size_t FileNameLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1LoadModelWithStrLen( FileName, FileNameLength ) ;
	DXFUNC_END
	return Result ;
}
extern int MV1LoadModelFromMem( const void *FileImage, int FileSize, int (* FileReadFunc )( const TCHAR *FilePath, void **FileImageAddr, int *FileSize, void *FileReadFuncData ), int (* FileReleaseFunc )( void *MemoryAddr, void *FileReadFuncData ), void *FileReadFuncData )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1LoadModelFromMem( FileImage, FileSize, FileReadFunc, FileReleaseFunc, FileReadFuncData ) ;
	DXFUNC_END
	return Result ;
}
extern int MV1CreateCloneModel( int SrcMHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1CreateCloneModel( SrcMHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int MV1DuplicateModel( int SrcMHandle )	
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1DuplicateModel( SrcMHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int MV1DeleteModel( int MHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1DeleteModel( MHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int MV1InitModel( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1InitModel() ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetLoadModelReMakeNormal( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetLoadModelReMakeNormal( Flag ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetLoadModelReMakeNormalSmoothingAngle( float SmoothingAngle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetLoadModelReMakeNormalSmoothingAngle( SmoothingAngle ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetLoadModelIgnoreScaling( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetLoadModelIgnoreScaling( Flag ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetLoadModelPositionOptimize( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetLoadModelPositionOptimize( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int MV1SetLoadModelNotEqNormalSide_AddZeroAreaPolygon( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetLoadModelNotEqNormalSide_AddZeroAreaPolygon( Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int MV1SetLoadModelUsePhysicsMode( int PhysicsMode /* DX_LOADMODEL_PHYSICS_LOADCALC 等 */ )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetLoadModelUsePhysicsMode( PhysicsMode ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetLoadModelPhysicsWorldGravity( float Gravity )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetLoadModelPhysicsWorldGravity( Gravity ) ;
	DXFUNC_END
	return Result ;
}

extern float MV1GetLoadModelPhysicsWorldGravity( void )
{
	float Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetLoadModelPhysicsWorldGravity() ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetLoadCalcPhysicsWorldGravity( int GravityNo, VECTOR Gravity )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetLoadCalcPhysicsWorldGravity( GravityNo, Gravity ) ;
	DXFUNC_END
	return Result ;
}

extern VECTOR MV1GetLoadCalcPhysicsWorldGravity( int GravityNo )
{
	VECTOR Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetLoadCalcPhysicsWorldGravity( GravityNo ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetLoadModelPhysicsCalcPrecision( int Precision )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetLoadModelPhysicsCalcPrecision( Precision ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetLoadModel_PMD_PMX_AnimationFPSMode( int FPSMode )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetLoadModel_PMD_PMX_AnimationFPSMode( FPSMode ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1AddLoadModelDisablePhysicsNameWord( const TCHAR *NameWord )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1AddLoadModelDisablePhysicsNameWord( NameWord ) ;
	DXFUNC_END
	return Result ;
}
extern int MV1AddLoadModelDisablePhysicsNameWordWithStrLen(	const TCHAR *NameWord, size_t NameWordLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1AddLoadModelDisablePhysicsNameWordWithStrLen( NameWord, NameWordLength ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1ResetLoadModelDisablePhysicsNameWord( void )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1ResetLoadModelDisablePhysicsNameWord() ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetLoadModelDisablePhysicsNameWordMode( int DisableNameWordMode )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetLoadModelDisablePhysicsNameWordMode( DisableNameWordMode ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetLoadModelAnimFilePath( const TCHAR *FileName )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetLoadModelAnimFilePath( FileName ) ;
	DXFUNC_END
	return Result ;
}
extern int MV1SetLoadModelAnimFilePathWithStrLen( const TCHAR *FileName, size_t FileNameLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetLoadModelAnimFilePathWithStrLen( FileName, FileNameLength ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetLoadModelUsePackDraw( int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetLoadModelUsePackDraw( Flag ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetLoadModelTriangleListUseMaxBoneNum( int UseMaxBoneNum )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetLoadModelTriangleListUseMaxBoneNum( UseMaxBoneNum ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SaveModelToMV1File( int MHandle, const TCHAR *FileName, int SaveType, int AnimMHandle, int AnimNameCheck, int Normal8BitFlag, int Position16BitFlag, int Weight8BitFlag, int Anim16BitFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SaveModelToMV1File( MHandle, FileName, SaveType, AnimMHandle, AnimNameCheck, Normal8BitFlag, Position16BitFlag, Weight8BitFlag, Anim16BitFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int MV1SaveModelToMV1FileWithStrLen(	int MHandle, const TCHAR *FileName, size_t FileNameLength, int SaveType, int AnimMHandle , int AnimNameCheck, int Normal8BitFlag, int Position16BitFlag, int Weight8BitFlag , int Anim16BitFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SaveModelToMV1FileWithStrLen( MHandle, FileName, FileNameLength, SaveType, AnimMHandle , AnimNameCheck, Normal8BitFlag, Position16BitFlag, Weight8BitFlag , Anim16BitFlag ) ;
	DXFUNC_END
	return Result ;
}

#ifndef DX_NON_SAVEFUNCTION
extern int MV1SaveModelToXFile(   int MHandle, const TCHAR *FileName, int SaveType, int AnimMHandle, int AnimNameCheck )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SaveModelToXFile( MHandle, FileName, SaveType, AnimMHandle, AnimNameCheck ) ;
	DXFUNC_END
	return Result ;
}
extern int MV1SaveModelToXFileWithStrLen( int MHandle, const TCHAR *FileName, size_t FileNameLength, int SaveType, int AnimMHandle , int AnimNameChecke )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SaveModelToXFileWithStrLen( MHandle, FileName, FileNameLength, SaveType, AnimMHandle , AnimNameChecke ) ;
	DXFUNC_END
	return Result ;
}
#endif // DX_NON_SAVEFUNCTION


extern int MV1DrawModel( int MHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1DrawModel( MHandle ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1DrawFrame( int MHandle, int FrameIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1DrawFrame( MHandle, FrameIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1DrawMesh( int MHandle, int MeshIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1DrawMesh( MHandle, MeshIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1DrawTriangleList( int MHandle, int TriangleListIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1DrawTriangleList( MHandle, TriangleListIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1DrawModelDebug( int MHandle, unsigned int Color, int IsNormalLine, float NormalLineLength, int IsPolyLine, int IsCollisionBox )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1DrawModelDebug(  MHandle,  Color,  IsNormalLine,  NormalLineLength,  IsPolyLine, IsCollisionBox ) ;
	DXFUNC_END
	return Result ;
}
extern int MV1SetUseOrigShader( int UseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetUseOrigShader( UseFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int MV1SetSemiTransDrawMode( int DrawMode )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetSemiTransDrawMode( DrawMode ) ;
	DXFUNC_END
	return Result ;
}
extern MATRIX MV1GetLocalWorldMatrix( int MHandle )
{
	MATRIX Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetLocalWorldMatrix(  MHandle ) ;
	DXFUNC_END
	return Result ;
}
extern MATRIX_D MV1GetLocalWorldMatrixD( int MHandle )
{
	MATRIX_D Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetLocalWorldMatrixD(  MHandle ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetPosition( int MHandle, VECTOR Position )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetPosition(  MHandle,  Position ) ;
	DXFUNC_END
	return Result ;
}
extern int MV1SetPositionD( int MHandle, VECTOR_D Position )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetPositionD(  MHandle,  Position ) ;
	DXFUNC_END
	return Result ;
}

extern VECTOR MV1GetPosition( int MHandle )
{
	VECTOR Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetPosition( MHandle ) ;
	DXFUNC_END
	return Result ;
}
extern VECTOR_D MV1GetPositionD( int MHandle )
{
	VECTOR_D Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetPositionD( MHandle ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetScale( int MHandle, VECTOR Scale )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetScale( MHandle, Scale ) ;
	DXFUNC_END
	return Result ;
}

extern VECTOR MV1GetScale( int MHandle ) 
{
	VECTOR Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetScale( MHandle )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetRotationXYZ( int MHandle, VECTOR Rotate ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetRotationXYZ( MHandle, Rotate )  ;
	DXFUNC_END
	return Result ;
}

extern VECTOR MV1GetRotationXYZ( int MHandle ) 
{
	VECTOR Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetRotationXYZ( MHandle )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetRotationZYAxis( int MHandle, VECTOR ZAxisDirection, VECTOR YAxisDirection, float ZAxisTwistRotate )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetRotationZYAxis( MHandle,  ZAxisDirection,  YAxisDirection,  ZAxisTwistRotate )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetRotationMatrix( int MHandle, MATRIX Matrix ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetRotationMatrix( MHandle,  Matrix )  ;
	DXFUNC_END
	return Result ;
}

extern MATRIX MV1GetRotationMatrix( int MHandle ) 
{
	MATRIX Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetRotationMatrix( MHandle )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMatrix( int MHandle, MATRIX Matrix ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMatrix( MHandle,  Matrix )  ;
	DXFUNC_END
	return Result ;
}
extern int MV1SetMatrixD( int MHandle, MATRIX_D Matrix ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMatrixD( MHandle,  Matrix )  ;
	DXFUNC_END
	return Result ;
}

extern MATRIX MV1GetMatrix( int MHandle ) 
{
	MATRIX Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMatrix( MHandle )  ;
	DXFUNC_END
	return Result ;
}
extern MATRIX_D MV1GetMatrixD( int MHandle ) 
{
	MATRIX_D Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMatrixD( MHandle )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetVisible( int MHandle, int VisibleFlag ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetVisible( MHandle,  VisibleFlag )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetVisible( int MHandle ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetVisible( MHandle )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMeshCategoryVisible( int MHandle, int MeshCategory, int VisibleFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMeshCategoryVisible( MHandle, MeshCategory, VisibleFlag ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetMeshCategoryVisible( int MHandle, int MeshCategory )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMeshCategoryVisible(  MHandle,  MeshCategory ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetDifColorScale( int MHandle, COLOR_F Scale ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetDifColorScale( MHandle,  Scale )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetSpcColorScale( int MHandle, COLOR_F Scale ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetSpcColorScale( MHandle,  Scale )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetEmiColorScale( int MHandle, COLOR_F Scale ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetEmiColorScale( MHandle,  Scale )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetAmbColorScale( int MHandle, COLOR_F Scale ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetAmbColorScale( MHandle,  Scale )  ;
	DXFUNC_END
	return Result ;
}

extern COLOR_F MV1GetDifColorScale( int MHandle ) 
{
	COLOR_F Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetDifColorScale( MHandle )  ;
	DXFUNC_END
	return Result ;
}

extern COLOR_F MV1GetSpcColorScale( int MHandle ) 
{
	COLOR_F Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetSpcColorScale( MHandle )  ;
	DXFUNC_END
	return Result ;
}

extern COLOR_F MV1GetEmiColorScale( int MHandle ) 
{
	COLOR_F Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetEmiColorScale( MHandle )  ;
	DXFUNC_END
	return Result ;
}

extern COLOR_F MV1GetAmbColorScale( int MHandle ) 
{
	COLOR_F Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetAmbColorScale( MHandle )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetSemiTransState( int MHandle ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetSemiTransState( MHandle )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetOpacityRate( int MHandle, float Rate ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetOpacityRate( MHandle,  Rate )  ;
	DXFUNC_END
	return Result ;
}

extern float MV1GetOpacityRate( int MHandle ) 
{
	float Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetOpacityRate( MHandle )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetUseDrawMulAlphaColor( int MHandle, int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetUseDrawMulAlphaColor( MHandle, Flag ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetUseDrawMulAlphaColor( int MHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetUseDrawMulAlphaColor( MHandle ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetUseZBuffer( int MHandle, int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetUseZBuffer(  MHandle,  Flag ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetWriteZBuffer( int MHandle, int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetWriteZBuffer(  MHandle,  Flag ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetZBufferCmpType( int MHandle, int CmpType /* DX_CMP_NEVER 等 */ )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetZBufferCmpType(  MHandle,  CmpType /* DX_CMP_NEVER 等 */ ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetZBias( int MHandle, int Bias )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetZBias(  MHandle,  Bias ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetUseVertDifColor( int MHandle, int UseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetUseVertDifColor(  MHandle,  UseFlag ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetUseVertSpcColor( int MHandle, int UseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetUseVertSpcColor(  MHandle,  UseFlag ) ;
	DXFUNC_END
	return Result ;
}
extern int MV1SetSampleFilterMode( int MHandle, int FilterMode )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetSampleFilterMode(  MHandle,  FilterMode ) ;
	DXFUNC_END
	return Result ;
}
extern int MV1SetMaxAnisotropy( int MHandle, int MaxAnisotropy )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMaxAnisotropy(  MHandle,  MaxAnisotropy ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetWireFrameDrawFlag( int MHandle, int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetWireFrameDrawFlag( MHandle, Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int MV1SetPhysicsWorldGravity( int MHandle, VECTOR Gravity )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetPhysicsWorldGravity( MHandle, Gravity ) ;
	DXFUNC_END
	return Result ;
}
extern int MV1RefreshVertColorFromMaterial( int MHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1RefreshVertColorFromMaterial( MHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int MV1PhysicsCalculation( int MHandle, float MillisecondTime )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1PhysicsCalculation( MHandle, MillisecondTime ) ;
	DXFUNC_END
	return Result ;
}
extern int MV1PhysicsResetState( int MHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1PhysicsResetState( MHandle ) ;
	DXFUNC_END
	return Result ;
}
extern int MV1SetUseShapeFlag( int MHandle, int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetUseShapeFlag(  MHandle,  Flag ) ;
	DXFUNC_END
	return Result ;
}
extern int MV1GetMaterialNumberOrderFlag( int MHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMaterialNumberOrderFlag( MHandle ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1AttachAnim( int MHandle, int AnimIndex, int AnimSrcMHandle, int NameCheck ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1AttachAnim( MHandle,  AnimIndex,  AnimSrcMHandle, NameCheck )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1DetachAnim( int MHandle, int AttachIndex ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1DetachAnim( MHandle, AttachIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetAttachAnimTime( int MHandle, int AttachIndex, float Time ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetAttachAnimTime( MHandle, AttachIndex,  Time )  ;
	DXFUNC_END
	return Result ;
}

extern float MV1GetAttachAnimTime( int MHandle, int AttachIndex ) 
{
	float Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetAttachAnimTime( MHandle, AttachIndex )  ;
	DXFUNC_END
	return Result ;
}

extern float MV1GetAttachAnimTotalTime( int MHandle, int AttachIndex ) 
{
	float Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetAttachAnimTotalTime( MHandle, AttachIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetAttachAnimBlendRate( int MHandle, int AttachIndex, float Rate ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetAttachAnimBlendRate( MHandle, AttachIndex,  Rate )  ;
	DXFUNC_END
	return Result ;
}

extern float MV1GetAttachAnimBlendRate( int MHandle, int AttachIndex ) 
{
	float Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetAttachAnimBlendRate( MHandle, AttachIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetAttachAnimBlendRateToFrame( int MHandle, int AttachIndex, int FrameIndex, float Rate, int SetChild )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetAttachAnimBlendRateToFrame(  MHandle,  AttachIndex,  FrameIndex,  Rate, SetChild ) ;
	DXFUNC_END
	return Result ;
}

extern float MV1GetAttachAnimBlendRateToFrame( int MHandle, int AttachIndex, int FrameIndex )
{
	float Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetAttachAnimBlendRateToFrame(  MHandle,  AttachIndex,  FrameIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetAttachAnim( int MHandle, int AttachIndex ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetAttachAnim( MHandle, AttachIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetAttachAnimUseShapeFlag( int MHandle, int AttachIndex, int UseFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetAttachAnimUseShapeFlag( MHandle, AttachIndex, UseFlag ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetAttachAnimUseShapeFlag( int MHandle, int AttachIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetAttachAnimUseShapeFlag( MHandle, AttachIndex ) ;
	DXFUNC_END
	return Result ;
}

extern VECTOR MV1GetAttachAnimFrameLocalPosition( int MHandle, int AttachIndex, int FrameIndex )
{
	VECTOR Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetAttachAnimFrameLocalPosition( MHandle, AttachIndex, FrameIndex ) ;
	DXFUNC_END
	return Result ;
}

extern MATRIX MV1GetAttachAnimFrameLocalMatrix(	int MHandle, int AttachIndex, int FrameIndex )
{
	MATRIX Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetAttachAnimFrameLocalMatrix( MHandle, AttachIndex, FrameIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetAnimNum( int MHandle ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetAnimNum( MHandle )  ;
	DXFUNC_END
	return Result ;
}

extern const TCHAR *MV1GetAnimName( int MHandle, int AnimIndex ) 
{
	const TCHAR * Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetAnimName( MHandle, AnimIndex )  ;
	DXFUNC_END
	return Result ;
}
extern int MV1SetAnimName( int MHandle, int AnimIndex, const TCHAR *AnimName )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetAnimName( MHandle, AnimIndex, AnimName ) ;
	DXFUNC_END
	return Result ;
}
extern int MV1SetAnimNameWithStrLen( int MHandle, int AnimIndex, const TCHAR *AnimName, size_t AnimNameLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetAnimNameWithStrLen( MHandle, AnimIndex, AnimName, AnimNameLength ) ;
	DXFUNC_END
	return Result ;
}
extern int MV1GetAnimIndex( int MHandle, const TCHAR *AnimName ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetAnimIndex( MHandle, AnimName )  ;
	DXFUNC_END
	return Result ;
}
extern int MV1GetAnimIndexWithStrLen( int MHandle, const TCHAR *AnimName, size_t AnimNameLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetAnimIndexWithStrLen( MHandle, AnimName, AnimNameLength ) ;
	DXFUNC_END
	return Result ;
}
extern float MV1GetAnimTotalTime( int MHandle, int AnimIndex ) 
{
	float Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetAnimTotalTime( MHandle, AnimIndex )  ;
	DXFUNC_END
	return Result ;
}
extern int MV1GetAnimLoopFlag( int MHandle, int AnimIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetAnimLoopFlag( MHandle, AnimIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetAnimTargetFrameNum( int MHandle, int AnimIndex ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetAnimTargetFrameNum( MHandle, AnimIndex )  ;
	DXFUNC_END
	return Result ;
}

extern const TCHAR *MV1GetAnimTargetFrameName( int MHandle, int AnimIndex, int AnimFrameIndex ) 
{
	const TCHAR * Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetAnimTargetFrameName( MHandle, AnimIndex, AnimFrameIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetAnimTargetFrame( int MHandle, int AnimIndex, int AnimFrameIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetAnimTargetFrame(  MHandle,  AnimIndex,  AnimFrameIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetAnimTargetFrameKeySetNum( int MHandle, int AnimIndex, int AnimFrameIndex ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetAnimTargetFrameKeySetNum( MHandle, AnimIndex, AnimFrameIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetAnimTargetFrameKeySet( int MHandle, int AnimIndex, int AnimFrameIndex, int Index ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetAnimTargetFrameKeySet( MHandle, AnimIndex, AnimFrameIndex, Index )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetAnimKeySetNum( int MHandle ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetAnimKeySetNum( MHandle )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetAnimKeySetType( int MHandle, int AnimKeySetIndex ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetAnimKeySetType( MHandle, AnimKeySetIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetAnimKeySetDataType( int MHandle, int AnimKeySetIndex ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetAnimKeySetDataType( MHandle, AnimKeySetIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetAnimKeySetTimeType( int MHandle, int AnimKeySetIndex ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetAnimKeySetTimeType( MHandle, AnimKeySetIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetAnimKeySetDataNum( int MHandle, int AnimKeySetIndex ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetAnimKeySetDataNum( MHandle, AnimKeySetIndex )  ;
	DXFUNC_END
	return Result ;
}

extern float MV1GetAnimKeyDataTime( int MHandle, int AnimKeySetIndex, int Index ) 
{
	float Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetAnimKeyDataTime( MHandle, AnimKeySetIndex, Index )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetAnimKeyDataIndexFromTime( int MHandle, int AnimKeySetIndex, float Time )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetAnimKeyDataIndexFromTime(  MHandle,  AnimKeySetIndex,  Time ) ;
	DXFUNC_END
	return Result ;
}

extern FLOAT4 MV1GetAnimKeyDataToQuaternion( int MHandle, int AnimKeySetIndex, int Index ) 
{
	FLOAT4 Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetAnimKeyDataToQuaternion( MHandle, AnimKeySetIndex, Index )  ;
	DXFUNC_END
	return Result ;
}

extern FLOAT4 MV1GetAnimKeyDataToQuaternionFromTime( int MHandle, int AnimKeySetIndex, float Time ) 
{
	FLOAT4 Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetAnimKeyDataToQuaternionFromTime( MHandle, AnimKeySetIndex, Time )  ;
	DXFUNC_END
	return Result ;
}

extern VECTOR MV1GetAnimKeyDataToVector( int MHandle, int AnimKeySetIndex, int Index ) 
{
	VECTOR Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetAnimKeyDataToVector( MHandle, AnimKeySetIndex, Index )  ;
	DXFUNC_END
	return Result ;
}

extern VECTOR MV1GetAnimKeyDataToVectorFromTime( int MHandle, int AnimKeySetIndex, float Time ) 
{
	VECTOR Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetAnimKeyDataToVectorFromTime( MHandle, AnimKeySetIndex, Time )  ;
	DXFUNC_END
	return Result ;
}

extern MATRIX MV1GetAnimKeyDataToMatrix( int MHandle, int AnimKeySetIndex, int Index ) 
{
	MATRIX Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetAnimKeyDataToMatrix( MHandle, AnimKeySetIndex, Index )  ;
	DXFUNC_END
	return Result ;
}

extern MATRIX MV1GetAnimKeyDataToMatrixFromTime( int MHandle, int AnimKeySetIndex, float Time ) 
{
	MATRIX Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetAnimKeyDataToMatrixFromTime( MHandle, AnimKeySetIndex, Time )  ;
	DXFUNC_END
	return Result ;
}

extern float MV1GetAnimKeyDataToFlat( int MHandle, int AnimKeySetIndex, int Index ) 
{
	float Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetAnimKeyDataToFlat( MHandle, AnimKeySetIndex, Index )  ;
	DXFUNC_END
	return Result ;
}

extern float MV1GetAnimKeyDataToFlatFromTime( int MHandle, int AnimKeySetIndex, float Time ) 
{
	float Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetAnimKeyDataToFlatFromTime( MHandle, AnimKeySetIndex, Time )  ;
	DXFUNC_END
	return Result ;
}

extern float MV1GetAnimKeyDataToLinear( int MHandle, int AnimKeySetIndex, int Index ) 
{
	float Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetAnimKeyDataToLinear( MHandle, AnimKeySetIndex, Index )  ;
	DXFUNC_END
	return Result ;
}

extern float MV1GetAnimKeyDataToLinearFromTime( int MHandle, int AnimKeySetIndex, float Time ) 
{
	float Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetAnimKeyDataToLinearFromTime( MHandle, AnimKeySetIndex, Time )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetMaterialNum( int MHandle ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMaterialNum( MHandle )  ;
	DXFUNC_END
	return Result ;
}

extern const TCHAR *MV1GetMaterialName( int MHandle, int MaterialIndex ) 
{
	const TCHAR * Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMaterialName( MHandle,  MaterialIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMaterialType( int MHandle, int MaterialIndex, int Type )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMaterialType(  MHandle,  MaterialIndex,  Type ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetMaterialType( int MHandle, int MaterialIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMaterialType(  MHandle,  MaterialIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMaterialTypeParam( int MHandle, int MaterialIndex, ... )
{
	int Result ;
	va_list VaList ;

	va_start( VaList, MaterialIndex ) ;

	DXFUNC_BEGIN

	Result = MV1SetMaterialTypeParam_Base( MHandle, MaterialIndex, VaList ) ;
	DXFUNC_END

	va_end( VaList ) ;

	return Result ;
}

extern COLOR_F MV1GetMaterialDifColor( int MHandle, int MaterialIndex ) 
{
	COLOR_F Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMaterialDifColor( MHandle,  MaterialIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMaterialDifColor( int MHandle, int MaterialIndex, COLOR_F Color ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMaterialDifColor( MHandle, MaterialIndex,  Color )  ;
	DXFUNC_END
	return Result ;
}

extern COLOR_F MV1GetMaterialSpcColor( int MHandle, int MaterialIndex ) 
{
	COLOR_F Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMaterialSpcColor( MHandle, MaterialIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMaterialSpcColor( int MHandle, int MaterialIndex, COLOR_F Color ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMaterialSpcColor( MHandle, MaterialIndex,  Color )  ;
	DXFUNC_END
	return Result ;
}

extern COLOR_F MV1GetMaterialEmiColor( int MHandle, int MaterialIndex ) 
{
	COLOR_F Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMaterialEmiColor( MHandle, MaterialIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMaterialEmiColor( int MHandle, int MaterialIndex, COLOR_F Color ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMaterialEmiColor( MHandle, MaterialIndex,  Color )  ;
	DXFUNC_END
	return Result ;
}

extern COLOR_F MV1GetMaterialAmbColor( int MHandle, int MaterialIndex ) 
{
	COLOR_F Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMaterialAmbColor( MHandle, MaterialIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMaterialAmbColor( int MHandle, int MaterialIndex, COLOR_F Color ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMaterialAmbColor( MHandle, MaterialIndex,  Color )  ;
	DXFUNC_END
	return Result ;
}

extern float MV1GetMaterialSpcPower( int MHandle, int MaterialIndex ) 
{
	float Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMaterialSpcPower( MHandle, MaterialIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMaterialSpcPower( int MHandle, int MaterialIndex, float Power ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMaterialSpcPower( MHandle, MaterialIndex,  Power )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMaterialDifMapTexture( int MHandle, int MaterialIndex, int TexIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMaterialDifMapTexture(  MHandle,  MaterialIndex,  TexIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetMaterialDifMapTexture( int MHandle, int MaterialIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMaterialDifMapTexture(  MHandle,  MaterialIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMaterialSpcMapTexture( int MHandle, int MaterialIndex, int TexIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMaterialSpcMapTexture(  MHandle,  MaterialIndex,  TexIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetMaterialSpcMapTexture( int MHandle, int MaterialIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMaterialSpcMapTexture(  MHandle,  MaterialIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetMaterialNormalMapTexture( int MHandle, int MaterialIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMaterialNormalMapTexture(  MHandle,  MaterialIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMaterialDifGradTexture( int MHandle, int MaterialIndex, int TexIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMaterialDifGradTexture(  MHandle,  MaterialIndex,  TexIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetMaterialDifGradTexture( int MHandle, int MaterialIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMaterialDifGradTexture(  MHandle,  MaterialIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMaterialSpcGradTexture( int MHandle, int MaterialIndex, int TexIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMaterialSpcGradTexture(  MHandle,  MaterialIndex,  TexIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetMaterialSpcGradTexture( int MHandle, int MaterialIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMaterialSpcGradTexture(  MHandle,  MaterialIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMaterialSphereMapTexture( int MHandle, int MaterialIndex, int TexIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMaterialSphereMapTexture(  MHandle,  MaterialIndex,  TexIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetMaterialSphereMapTexture( int MHandle, int MaterialIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMaterialSphereMapTexture( MHandle, MaterialIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMaterialDifGradBlendType( int MHandle, int MaterialIndex, int BlendType )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMaterialDifGradBlendType(  MHandle,  MaterialIndex,  BlendType ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetMaterialDifGradBlendType( int MHandle, int MaterialIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMaterialDifGradBlendType(  MHandle,  MaterialIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMaterialSpcGradBlendType( int MHandle, int MaterialIndex, int BlendType )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMaterialSpcGradBlendType(  MHandle,  MaterialIndex,  BlendType ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetMaterialSpcGradBlendType( int MHandle, int MaterialIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMaterialSpcGradBlendType(  MHandle,  MaterialIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMaterialSphereMapBlendType( int MHandle, int MaterialIndex, int BlendType )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMaterialSphereMapBlendType(  MHandle,  MaterialIndex,  BlendType ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetMaterialSphereMapBlendType( int MHandle, int MaterialIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMaterialSphereMapBlendType( MHandle, MaterialIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMaterialOutLineWidth( int MHandle, int MaterialIndex, float Width )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMaterialOutLineWidth(  MHandle,  MaterialIndex,  Width ) ;
	DXFUNC_END
	return Result ;
}

extern float MV1GetMaterialOutLineWidth( int MHandle, int MaterialIndex )
{
	float Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMaterialOutLineWidth(  MHandle,  MaterialIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMaterialOutLineDotWidth( int MHandle, int MaterialIndex, float Width )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMaterialOutLineDotWidth(  MHandle,  MaterialIndex,  Width ) ;
	DXFUNC_END
	return Result ;
}

extern float MV1GetMaterialOutLineDotWidth( int MHandle, int MaterialIndex )
{
	float Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMaterialOutLineDotWidth(  MHandle,  MaterialIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMaterialOutLineColor( int MHandle, int MaterialIndex, COLOR_F Color )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMaterialOutLineColor(  MHandle,  MaterialIndex,  Color ) ;
	DXFUNC_END
	return Result ;
}

extern COLOR_F MV1GetMaterialOutLineColor( int MHandle, int MaterialIndex )
{
	COLOR_F Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMaterialOutLineColor(  MHandle,  MaterialIndex ) ;
	DXFUNC_END
	return Result ;
}


extern int MV1SetMaterialDrawBlendMode( int MHandle, int MaterialIndex, int BlendMode ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMaterialDrawBlendMode( MHandle, MaterialIndex,  BlendMode )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetMaterialDrawBlendMode( int MHandle, int MaterialIndex ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMaterialDrawBlendMode( MHandle, MaterialIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetMaterialDrawBlendParam( int MHandle, int MaterialIndex ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMaterialDrawBlendParam( MHandle, MaterialIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMaterialDrawAlphaTest( int MHandle, int MaterialIndex,	int Enable, int Mode, int Param )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMaterialDrawAlphaTest(  MHandle,  MaterialIndex,	 Enable,  Mode,  Param ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetMaterialDrawAlphaTestEnable( int MHandle, int MaterialIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMaterialDrawAlphaTestEnable(  MHandle,  MaterialIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetMaterialDrawAlphaTestMode(	int MHandle, int MaterialIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMaterialDrawAlphaTestMode(	 MHandle,  MaterialIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetMaterialDrawAlphaTestParam( int MHandle, int MaterialIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMaterialDrawAlphaTestParam(  MHandle,  MaterialIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMaterialTypeAll( int MHandle, int Type )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMaterialTypeAll( MHandle, Type ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMaterialTypeParamAll( int MHandle, ... )
{
	int Result ;
	va_list VaList ;

	va_start( VaList, MHandle ) ;

	DXFUNC_BEGIN

	Result = MV1SetMaterialTypeParamAll_Base( MHandle, VaList ) ;
	DXFUNC_END

	va_end( VaList ) ;

	return Result ;
}

extern int MV1SetMaterialDifGradBlendTypeAll(	int MHandle,                    int BlendType )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMaterialDifGradBlendTypeAll(	 MHandle,                     BlendType ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMaterialSpcGradBlendTypeAll(	int MHandle,                    int BlendType )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMaterialSpcGradBlendTypeAll(	 MHandle,                     BlendType ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMaterialSphereMapBlendTypeAll( int MHandle,                    int BlendType )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMaterialSphereMapBlendTypeAll(  MHandle,                     BlendType ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMaterialOutLineWidthAll(		int MHandle,                    float Width )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMaterialOutLineWidthAll(		 MHandle,                    Width ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMaterialOutLineDotWidthAll(	int MHandle,                    float Width )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMaterialOutLineDotWidthAll(	 MHandle,                    Width ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMaterialOutLineColorAll(		int MHandle,                    COLOR_F Color )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMaterialOutLineColorAll(		 MHandle,                    Color ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMaterialDrawBlendModeAll(		int MHandle,                     int BlendMode )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMaterialDrawBlendModeAll(		 MHandle,                     BlendMode ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMaterialDrawBlendParamAll(	int MHandle,                    int BlendParam )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMaterialDrawBlendParamAll(	 MHandle,                     BlendParam ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMaterialDrawAlphaTestAll(		int MHandle,                    int Enable, int Mode, int Param )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMaterialDrawAlphaTestAll(		 MHandle,                     Enable,  Mode,  Param ) ;
	DXFUNC_END
	return Result ;
}



extern int MV1SetMaterialDrawBlendParam( int MHandle, int MaterialIndex, int BlendParam ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMaterialDrawBlendParam( MHandle, MaterialIndex, BlendParam )  ;
	DXFUNC_END
	return Result ;
}


extern int MV1GetTextureNum( int MHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetTextureNum( MHandle ) ;
	DXFUNC_END
	return Result ;
}

extern const TCHAR *MV1GetTextureName( int MHandle, int TexIndex )
{
	const TCHAR *Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetTextureName(  MHandle,  TexIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetTextureColorFilePath( int MHandle, int TexIndex, const TCHAR *FilePath )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetTextureColorFilePath(  MHandle,  TexIndex, FilePath ) ;
	DXFUNC_END
	return Result ;
}
extern int MV1SetTextureColorFilePathWithStrLen( int MHandle, int TexIndex, const TCHAR *FilePath, size_t FilePathLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetTextureColorFilePathWithStrLen( MHandle, TexIndex, FilePath, FilePathLength ) ;
	DXFUNC_END
	return Result ;
}

extern const TCHAR *MV1GetTextureColorFilePath( int MHandle, int TexIndex )
{
	const TCHAR *Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetTextureColorFilePath(  MHandle,  TexIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetTextureAlphaFilePath( int MHandle, int TexIndex, const TCHAR *FilePath )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetTextureAlphaFilePath(  MHandle,  TexIndex, FilePath ) ;
	DXFUNC_END
	return Result ;
}
extern int MV1SetTextureAlphaFilePathWithStrLen( int MHandle, int TexIndex, const TCHAR *FilePath, size_t FilePathLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetTextureAlphaFilePathWithStrLen( MHandle, TexIndex, FilePath, FilePathLength ) ;
	DXFUNC_END
	return Result ;
}

extern const TCHAR *MV1GetTextureAlphaFilePath( int MHandle, int TexIndex )
{
	const TCHAR *Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetTextureAlphaFilePath(  MHandle,  TexIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetTextureGraphHandle( int MHandle, int TexIndex, int GrHandle, int SemiTransFlag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetTextureGraphHandle(  MHandle,  TexIndex,  GrHandle,  SemiTransFlag ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetTextureGraphHandle( int MHandle, int TexIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetTextureGraphHandle(  MHandle,  TexIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetTextureAddressMode( int MHandle, int TexIndex, int AddrUMode, int AddrVMode )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetTextureAddressMode(  MHandle,  TexIndex,  AddrUMode,  AddrVMode ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetTextureAddressModeU( int MHandle, int TexIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetTextureAddressModeU(  MHandle,  TexIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetTextureAddressModeV( int MHandle, int TexIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetTextureAddressModeV(  MHandle,  TexIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetTextureWidth( int MHandle, int TexIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetTextureWidth(  MHandle,  TexIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetTextureHeight( int MHandle, int TexIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetTextureHeight(  MHandle,  TexIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetTextureSemiTransState( int MHandle, int TexIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetTextureSemiTransState(  MHandle,  TexIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetTextureBumpImageFlag( int MHandle, int TexIndex, int Flag )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetTextureBumpImageFlag(  MHandle,  TexIndex,  Flag ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetTextureBumpImageFlag( int MHandle, int TexIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetTextureBumpImageFlag(  MHandle,  TexIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetTextureBumpImageNextPixelLength( int MHandle, int TexIndex, float Length )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetTextureBumpImageNextPixelLength(  MHandle,  TexIndex,  Length ) ;
	DXFUNC_END
	return Result ;
}

extern float MV1GetTextureBumpImageNextPixelLength( int MHandle, int TexIndex )
{
	float Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetTextureBumpImageNextPixelLength(  MHandle,  TexIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetTextureSampleFilterMode( int MHandle, int TexIndex, int FilterMode )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetTextureSampleFilterMode(  MHandle,  TexIndex,  FilterMode ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetTextureSampleFilterMode( int MHandle, int TexIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetTextureSampleFilterMode(  MHandle,  TexIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1LoadTexture( const TCHAR *FilePath )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1LoadTexture( FilePath ) ;
	DXFUNC_END
	return Result ;
}
extern int MV1LoadTextureWithStrLen( const TCHAR *FilePath, size_t FilePathLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1LoadTextureWithStrLen( FilePath, FilePathLength ) ;
	DXFUNC_END
	return Result ;
}




extern int MV1GetFrameNum( int MHandle ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetFrameNum( MHandle )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SearchFrame( int MHandle, const TCHAR *FrameName ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SearchFrame( MHandle, FrameName )  ;
	DXFUNC_END
	return Result ;
}
extern int MV1SearchFrameWithStrLen( int MHandle, const TCHAR *FrameName, size_t FrameNameLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SearchFrameWithStrLen( MHandle, FrameName, FrameNameLength ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SearchFrameChild( int MHandle, int FrameIndex, const TCHAR *ChildName ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SearchFrameChild( MHandle, FrameIndex, ChildName )  ;
	DXFUNC_END
	return Result ;
}
extern int MV1SearchFrameChildWithStrLen( int MHandle, int FrameIndex , const TCHAR *ChildName, size_t ChildNameLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SearchFrameChildWithStrLen( MHandle, FrameIndex , ChildName, ChildNameLength ) ;
	DXFUNC_END
	return Result ;
}

extern const TCHAR *MV1GetFrameName( int MHandle, int FrameIndex ) 
{
	const TCHAR * Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetFrameName( MHandle, FrameIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetFrameName2( int MHandle, int FrameIndex, TCHAR *StrBuffer )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetFrameName2( MHandle, FrameIndex, StrBuffer ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetFrameParent( int MHandle, int FrameIndex ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetFrameParent( MHandle, FrameIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetFrameChildNum( int MHandle, int FrameIndex ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetFrameChildNum( MHandle, FrameIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetFrameChild( int MHandle, int FrameIndex, int ChildIndex ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetFrameChild( MHandle, FrameIndex, ChildIndex )  ;
	DXFUNC_END
	return Result ;
}

extern VECTOR MV1GetFramePosition( int MHandle, int FrameIndex ) 
{
	VECTOR Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetFramePosition( MHandle, FrameIndex )  ;
	DXFUNC_END
	return Result ;
}
extern VECTOR_D MV1GetFramePositionD( int MHandle, int FrameIndex ) 
{
	VECTOR_D Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetFramePositionD( MHandle, FrameIndex )  ;
	DXFUNC_END
	return Result ;
}

extern MATRIX MV1GetFrameBaseLocalMatrix( int MHandle, int FrameIndex ) 
{
	MATRIX Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetFrameBaseLocalMatrix( MHandle, FrameIndex )  ;
	DXFUNC_END
	return Result ;
}
extern MATRIX_D MV1GetFrameBaseLocalMatrixD( int MHandle, int FrameIndex ) 
{
	MATRIX_D Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetFrameBaseLocalMatrixD( MHandle, FrameIndex )  ;
	DXFUNC_END
	return Result ;
}

extern MATRIX MV1GetFrameLocalMatrix( int MHandle, int FrameIndex )
{
	MATRIX Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetFrameLocalMatrix(  MHandle,  FrameIndex ) ;
	DXFUNC_END
	return Result ;
}
extern MATRIX_D MV1GetFrameLocalMatrixD( int MHandle, int FrameIndex )
{
	MATRIX_D Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetFrameLocalMatrixD(  MHandle,  FrameIndex ) ;
	DXFUNC_END
	return Result ;
}

extern MATRIX MV1GetFrameLocalWorldMatrix( int MHandle, int FrameIndex ) 
{
	MATRIX Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetFrameLocalWorldMatrix( MHandle, FrameIndex )  ;
	DXFUNC_END
	return Result ;
}
extern MATRIX_D MV1GetFrameLocalWorldMatrixD( int MHandle, int FrameIndex ) 
{
	MATRIX_D Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetFrameLocalWorldMatrixD( MHandle, FrameIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetFrameUserLocalMatrix( int MHandle, int FrameIndex, MATRIX Matrix )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetFrameUserLocalMatrix(  MHandle,  FrameIndex,  Matrix ) ;
	DXFUNC_END
	return Result ;
}
extern int MV1SetFrameUserLocalMatrixD( int MHandle, int FrameIndex, MATRIX_D Matrix )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetFrameUserLocalMatrixD(  MHandle,  FrameIndex,  Matrix ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1ResetFrameUserLocalMatrix( int MHandle, int FrameIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1ResetFrameUserLocalMatrix(  MHandle,  FrameIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetFrameUserLocalWorldMatrix( int MHandle, int FrameIndex, MATRIX   Matrix )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetFrameUserLocalWorldMatrix( MHandle, FrameIndex, Matrix ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetFrameUserLocalWorldMatrixD( int MHandle, int FrameIndex, MATRIX_D Matrix )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetFrameUserLocalWorldMatrixD( MHandle, FrameIndex, Matrix ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1ResetFrameUserLocalWorldMatrix( int MHandle, int FrameIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1ResetFrameUserLocalWorldMatrix( MHandle, FrameIndex ) ;
	DXFUNC_END
	return Result ;
}

extern VECTOR MV1GetFrameMaxVertexLocalPosition( int MHandle, int FrameIndex ) 
{
	VECTOR Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetFrameMaxVertexLocalPosition( MHandle, FrameIndex )  ;
	DXFUNC_END
	return Result ;
}
extern VECTOR_D MV1GetFrameMaxVertexLocalPositionD( int MHandle, int FrameIndex ) 
{
	VECTOR_D Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetFrameMaxVertexLocalPositionD( MHandle, FrameIndex )  ;
	DXFUNC_END
	return Result ;
}

extern VECTOR MV1GetFrameMinVertexLocalPosition( int MHandle, int FrameIndex ) 
{
	VECTOR Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetFrameMinVertexLocalPosition( MHandle, FrameIndex )  ;
	DXFUNC_END
	return Result ;
}
extern VECTOR_D MV1GetFrameMinVertexLocalPositionD( int MHandle, int FrameIndex ) 
{
	VECTOR_D Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetFrameMinVertexLocalPositionD( MHandle, FrameIndex )  ;
	DXFUNC_END
	return Result ;
}

extern VECTOR MV1GetFrameAvgVertexLocalPosition( int MHandle, int FrameIndex ) 
{
	VECTOR Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetFrameAvgVertexLocalPosition( MHandle, FrameIndex )  ;
	DXFUNC_END
	return Result ;
}
extern VECTOR_D MV1GetFrameAvgVertexLocalPositionD( int MHandle, int FrameIndex ) 
{
	VECTOR_D Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetFrameAvgVertexLocalPositionD( MHandle, FrameIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetFrameTriangleNum( int MHandle, int FrameIndex ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetFrameTriangleNum( MHandle, FrameIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetFrameMeshNum( int MHandle, int FrameIndex ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetFrameMeshNum( MHandle, FrameIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetFrameMesh( int MHandle, int FrameIndex, int Index ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetFrameMesh( MHandle, FrameIndex, Index )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetFrameVisible( int MHandle, int FrameIndex, int VisibleFlag ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetFrameVisible( MHandle, FrameIndex, VisibleFlag )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetFrameVisible( int MHandle, int FrameIndex ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetFrameVisible( MHandle, FrameIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetFrameDifColorScale( int MHandle, int FrameIndex, COLOR_F Scale ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetFrameDifColorScale( MHandle, FrameIndex,  Scale )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetFrameSpcColorScale( int MHandle, int FrameIndex, COLOR_F Scale ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetFrameSpcColorScale( MHandle, FrameIndex,  Scale )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetFrameEmiColorScale( int MHandle, int FrameIndex, COLOR_F Scale ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetFrameEmiColorScale( MHandle, FrameIndex,  Scale )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetFrameAmbColorScale( int MHandle, int FrameIndex, COLOR_F Scale ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetFrameAmbColorScale( MHandle, FrameIndex,  Scale )  ;
	DXFUNC_END
	return Result ;
}

extern COLOR_F MV1GetFrameDifColorScale( int MHandle, int FrameIndex ) 
{
	COLOR_F Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetFrameDifColorScale( MHandle, FrameIndex )  ;
	DXFUNC_END
	return Result ;
}

extern COLOR_F MV1GetFrameSpcColorScale( int MHandle, int FrameIndex ) 
{
	COLOR_F Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetFrameSpcColorScale( MHandle, FrameIndex )  ;
	DXFUNC_END
	return Result ;
}

extern COLOR_F MV1GetFrameEmiColorScale( int MHandle, int FrameIndex ) 
{
	COLOR_F Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetFrameEmiColorScale( MHandle, FrameIndex )  ;
	DXFUNC_END
	return Result ;
}

extern COLOR_F MV1GetFrameAmbColorScale( int MHandle, int FrameIndex ) 
{
	COLOR_F Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetFrameAmbColorScale( MHandle, FrameIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetFrameSemiTransState( int MHandle, int FrameIndex ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetFrameSemiTransState( MHandle, FrameIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetFrameOpacityRate( int MHandle, int FrameIndex, float Rate ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetFrameOpacityRate( MHandle, FrameIndex, Rate )  ;
	DXFUNC_END
	return Result ;
}

extern float MV1GetFrameOpacityRate( int MHandle, int FrameIndex ) 
{
	float Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetFrameOpacityRate( MHandle, FrameIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetFrameBaseVisible( int MHandle, int FrameIndex, int VisibleFlag ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetFrameBaseVisible( MHandle, FrameIndex, VisibleFlag )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetFrameBaseVisible( int MHandle, int FrameIndex ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetFrameBaseVisible( MHandle, FrameIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetFrameTextureAddressTransform( int MHandle, int FrameIndex, float TransU, float TransV, float ScaleU, float ScaleV, float RotCenterU, float RotCenterV, float Rotate )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetFrameTextureAddressTransform(  MHandle,  FrameIndex,  TransU,  TransV,  ScaleU,  ScaleV,  RotCenterU,  RotCenterV,  Rotate ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetFrameTextureAddressTransformMatrix( int MHandle, int FrameIndex, MATRIX Matrix )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetFrameTextureAddressTransformMatrix(  MHandle,  FrameIndex,  Matrix ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1ResetFrameTextureAddressTransform( int MHandle, int FrameIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1ResetFrameTextureAddressTransform(  MHandle,  FrameIndex ) ;
	DXFUNC_END
	return Result ;
}


extern int MV1GetMeshNum( int MHandle ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMeshNum( MHandle )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetMeshMaterial( int MHandle, int MeshIndex ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMeshMaterial( MHandle, MeshIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetMeshTriangleNum( int MHandle, int MeshIndex ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMeshTriangleNum( MHandle, MeshIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMeshVisible( int MHandle, int MeshIndex, int VisibleFlag ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMeshVisible( MHandle, MeshIndex, VisibleFlag )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetMeshVisible( int MHandle, int MeshIndex ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMeshVisible( MHandle, MeshIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMeshDifColorScale( int MHandle, int MeshIndex, COLOR_F Scale ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMeshDifColorScale( MHandle, MeshIndex,  Scale )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMeshSpcColorScale( int MHandle, int MeshIndex, COLOR_F Scale ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMeshSpcColorScale( MHandle, MeshIndex,  Scale )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMeshEmiColorScale( int MHandle, int MeshIndex, COLOR_F Scale ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMeshEmiColorScale( MHandle, MeshIndex,  Scale )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMeshAmbColorScale( int MHandle, int MeshIndex, COLOR_F Scale ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMeshAmbColorScale( MHandle, MeshIndex,  Scale )  ;
	DXFUNC_END
	return Result ;
}

extern COLOR_F MV1GetMeshDifColorScale( int MHandle, int MeshIndex ) 
{
	COLOR_F Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMeshDifColorScale( MHandle, MeshIndex )  ;
	DXFUNC_END
	return Result ;
}

extern COLOR_F MV1GetMeshSpcColorScale( int MHandle, int MeshIndex ) 
{
	COLOR_F Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMeshSpcColorScale( MHandle, MeshIndex )  ;
	DXFUNC_END
	return Result ;
}

extern COLOR_F MV1GetMeshEmiColorScale( int MHandle, int MeshIndex ) 
{
	COLOR_F Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMeshEmiColorScale( MHandle, MeshIndex )  ;
	DXFUNC_END
	return Result ;
}

extern COLOR_F MV1GetMeshAmbColorScale( int MHandle, int MeshIndex ) 
{
	COLOR_F Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMeshAmbColorScale( MHandle, MeshIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMeshOpacityRate( int MHandle, int MeshIndex, float Rate ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMeshOpacityRate( MHandle, MeshIndex,  Rate )  ;
	DXFUNC_END
	return Result ;
}

extern float MV1GetMeshOpacityRate( int MHandle, int MeshIndex ) 
{
	float Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMeshOpacityRate( MHandle, MeshIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMeshDrawBlendMode( int MHandle, int MeshIndex, int BlendMode ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMeshDrawBlendMode( MHandle, MeshIndex,  BlendMode )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMeshDrawBlendParam( int MHandle, int MeshIndex, int BlendParam ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMeshDrawBlendParam( MHandle, MeshIndex,  BlendParam )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetMeshDrawBlendMode( int MHandle, int MeshIndex ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMeshDrawBlendMode( MHandle, MeshIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetMeshDrawBlendParam( int MHandle, int MeshIndex ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMeshDrawBlendParam( MHandle, MeshIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMeshBaseVisible( int MHandle, int MeshIndex, int VisibleFlag ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMeshBaseVisible( MHandle, MeshIndex,  VisibleFlag )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetMeshBaseVisible( int MHandle, int MeshIndex ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMeshBaseVisible( MHandle, MeshIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMeshBackCulling( int MHandle, int MeshIndex, int CullingFlag ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMeshBackCulling( MHandle, MeshIndex,  CullingFlag )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetMeshBackCulling( int MHandle, int MeshIndex ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMeshBackCulling( MHandle, MeshIndex )  ;
	DXFUNC_END
	return Result ;
}

extern VECTOR MV1GetMeshMaxPosition( int MHandle, int MeshIndex ) 
{
	VECTOR Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMeshMaxPosition( MHandle, MeshIndex )  ;
	DXFUNC_END
	return Result ;
}

extern VECTOR MV1GetMeshMinPosition( int MHandle, int MeshIndex ) 
{
	VECTOR Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMeshMinPosition( MHandle, MeshIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetMeshTListNum( int MHandle, int MeshIndex ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMeshTListNum( MHandle, MeshIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetMeshTList( int MHandle, int MeshIndex, int Index ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMeshTList( MHandle, MeshIndex, Index )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetMeshSemiTransState( int MHandle, int MeshIndex ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMeshSemiTransState( MHandle, MeshIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMeshUseVertDifColor( int MHandle, int MeshIndex, int UseFlag ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMeshUseVertDifColor( MHandle, MeshIndex,  UseFlag )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetMeshUseVertSpcColor( int MHandle, int MeshIndex, int UseFlag ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetMeshUseVertSpcColor( MHandle, MeshIndex,  UseFlag )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetMeshUseVertDifColor( int MHandle, int MeshIndex ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMeshUseVertDifColor( MHandle, MeshIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetMeshUseVertSpcColor( int MHandle, int MeshIndex ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMeshUseVertSpcColor( MHandle, MeshIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetMeshShapeFlag( int MHandle, int MeshIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetMeshShapeFlag(  MHandle,  MeshIndex ) ;
	DXFUNC_END
	return Result ;
}


extern int MV1GetShapeNum( int MHandle )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetShapeNum( MHandle ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SearchShape( int MHandle, const TCHAR *ShapeName )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SearchShape( MHandle, ShapeName ) ;
	DXFUNC_END
	return Result ;
}
extern int MV1SearchShapeWithStrLen( int MHandle, const TCHAR *ShapeName, size_t ShapeNameLength )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SearchShapeWithStrLen( MHandle, ShapeName, ShapeNameLength ) ;
	DXFUNC_END
	return Result ;
}

extern const TCHAR *MV1GetShapeName( int MHandle, int ShapeIndex )
{
	const TCHAR *Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetShapeName(  MHandle,  ShapeIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetShapeTargetMeshNum( int MHandle, int ShapeIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetShapeTargetMeshNum(  MHandle,  ShapeIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetShapeTargetMesh( int MHandle, int ShapeIndex, int Index )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetShapeTargetMesh(  MHandle,  ShapeIndex,  Index ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetShapeRate( int MHandle, int ShapeIndex, float Rate, int Type )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetShapeRate(  MHandle,  ShapeIndex, Rate, Type ) ;
	DXFUNC_END
	return Result ;
}

extern float MV1GetShapeRate( int MHandle, int ShapeIndex )
{
	float Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetShapeRate( MHandle, ShapeIndex ) ;
	DXFUNC_END
	return Result ;
}

extern float MV1GetShapeApplyRate( int MHandle, int ShapeIndex )
{
	float Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetShapeApplyRate( MHandle, ShapeIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetTriangleListNum( int MHandle ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetTriangleListNum( MHandle )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetTriangleListVertexType( int MHandle, int TListIndex ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetTriangleListVertexType( MHandle, TListIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetTriangleListPolygonNum( int MHandle, int TListIndex ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetTriangleListPolygonNum( MHandle, TListIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1GetTriangleListVertexNum( int MHandle, int TListIndex ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetTriangleListVertexNum( MHandle, TListIndex )  ;
	DXFUNC_END
	return Result ;
}

extern	int		MV1GetTriangleListLocalWorldMatrixNum(	int MHandle, int TListIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetTriangleListLocalWorldMatrixNum( MHandle, TListIndex ) ;
	DXFUNC_END
	return Result ;
}

extern	int		MV1GetTriangleListPolygonVertexPosition( int MHandle, int TListIndex, int PolygonIndex, VECTOR *VertexPositions , float *MatrixWeights )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetTriangleListPolygonVertexPosition( MHandle, TListIndex, PolygonIndex, VertexPositions , MatrixWeights ) ;
	DXFUNC_END
	return Result ;
}

extern int		MV1GetTriangleListUseMaterial( int MHandle, int TListIndex )
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetTriangleListUseMaterial( MHandle, TListIndex ) ;
	DXFUNC_END
	return Result ;
}

extern	MATRIX	MV1GetTriangleListLocalWorldMatrix(		int MHandle, int TListIndex, int LWMatrixIndex )
{
	MATRIX Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetTriangleListLocalWorldMatrix( MHandle, TListIndex, LWMatrixIndex ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetupCollInfo( int MHandle, int FrameIndex, int XDivNum, int YDivNum, int ZDivNum ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetupCollInfo( MHandle, FrameIndex,  XDivNum,  YDivNum,  ZDivNum )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1TerminateCollInfo( int MHandle, int FrameIndex ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1TerminateCollInfo( MHandle, FrameIndex )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1RefreshCollInfo( int MHandle, int FrameIndex ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1RefreshCollInfo( MHandle, FrameIndex )  ;
	DXFUNC_END
	return Result ;
}

extern MV1_COLL_RESULT_POLY MV1CollCheck_Line( int MHandle, int FrameIndex, VECTOR PosStart, VECTOR PosEnd ) 
{
	MV1_COLL_RESULT_POLY Result ;
	DXFUNC_BEGIN
	Result = NS_MV1CollCheck_Line( MHandle, FrameIndex,  PosStart,  PosEnd )  ;
	DXFUNC_END
	return Result ;
}

extern	MV1_COLL_RESULT_POLY_DIM MV1CollCheck_LineDim( int MHandle, int FrameIndex, VECTOR PosStart, VECTOR PosEnd )
{
	MV1_COLL_RESULT_POLY_DIM Result ;
	DXFUNC_BEGIN
	Result = NS_MV1CollCheck_LineDim(  MHandle,  FrameIndex,  PosStart,  PosEnd ) ;
	DXFUNC_END
	return Result ;
}

extern MV1_COLL_RESULT_POLY_DIM MV1CollCheck_Sphere( int MHandle, int FrameIndex, VECTOR CenterPos, float r ) 
{
	MV1_COLL_RESULT_POLY_DIM Result ;
	DXFUNC_BEGIN
	Result = NS_MV1CollCheck_Sphere( MHandle, FrameIndex,  CenterPos, r )  ;
	DXFUNC_END
	return Result ;
}

extern	MV1_COLL_RESULT_POLY_DIM MV1CollCheck_Capsule( int MHandle, int FrameIndex, VECTOR Pos1, VECTOR Pos2, float r )
{
	MV1_COLL_RESULT_POLY_DIM Result ;
	DXFUNC_BEGIN
	Result = NS_MV1CollCheck_Capsule(  MHandle,  FrameIndex,  Pos1,  Pos2,  r ) ;
	DXFUNC_END
	return Result ;
}

extern	MV1_COLL_RESULT_POLY_DIM MV1CollCheck_Triangle( int MHandle, int FrameIndex, VECTOR Pos1, VECTOR Pos2, VECTOR Pos3 )
{
	MV1_COLL_RESULT_POLY_DIM Result ;
	DXFUNC_BEGIN
	Result = NS_MV1CollCheck_Triangle( MHandle, FrameIndex, Pos1, Pos2, Pos3 ) ;
	DXFUNC_END
	return Result ;
}

extern	MV1_COLL_RESULT_POLY MV1CollCheck_GetResultPoly( MV1_COLL_RESULT_POLY_DIM ResultPolyDim, int PolyNo )
{
	MV1_COLL_RESULT_POLY Result ;
	DXFUNC_BEGIN
	Result = NS_MV1CollCheck_GetResultPoly(  ResultPolyDim,  PolyNo ) ;
	DXFUNC_END
	return Result ;
}

extern int MV1CollResultPolyDimTerminate( MV1_COLL_RESULT_POLY_DIM ResultPolyDim ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1CollResultPolyDimTerminate(  ResultPolyDim )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1SetupReferenceMesh( int MHandle, int FrameIndex, int IsTransform, int IsPositionOnly ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1SetupReferenceMesh( MHandle, FrameIndex,  IsTransform, IsPositionOnly )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1TerminateReferenceMesh( int MHandle, int FrameIndex, int IsTransform, int IsPositionOnly ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1TerminateReferenceMesh( MHandle, FrameIndex,  IsTransform, IsPositionOnly )  ;
	DXFUNC_END
	return Result ;
}

extern int MV1RefreshReferenceMesh( int MHandle, int FrameIndex, int IsTransform, int IsPositionOnly ) 
{
	int Result ;
	DXFUNC_BEGIN
	Result = NS_MV1RefreshReferenceMesh( MHandle, FrameIndex,  IsTransform, IsPositionOnly )  ;
	DXFUNC_END
	return Result ;
}

extern MV1_REF_POLYGONLIST	MV1GetReferenceMesh( int MHandle, int FrameIndex, int IsTransform, int IsPositionOnly ) 
{
	MV1_REF_POLYGONLIST Result ;
	DXFUNC_BEGIN
	Result = NS_MV1GetReferenceMesh( MHandle, FrameIndex,  IsTransform, IsPositionOnly )  ;
	DXFUNC_END
	return Result ;
}


#endif

#endif

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE




