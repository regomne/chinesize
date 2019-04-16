// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		ログプログラム
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

// インクルード ------------------------------------------------------------------
#include "DxLog.h"

#ifndef DX_NON_LOG

#include "DxLib.h"
#include "DxFont.h"
#include "DxStatic.h"
#include "DxBaseFunc.h"
#include "DxUseCLib.h"
#include "DxSystem.h"
#include "DxMemory.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

// 構造体定義 --------------------------------------------------------------------

// 内部大域変数宣言 --------------------------------------------------------------

LOGDATA LogData ;

// 関数プロトタイプ宣言-----------------------------------------------------------

#ifndef DX_NON_PRINTF_DX

// ログ出力機能関数
static	int			RefreshLogFont( void ) ;												// ログ処理で使用しているフォントを更新する
static	int			CrLog( void ) ;															// 改行処理
static	int			AddCharLog( const wchar_t *C ) ;										// 一文字入るか調べて、必要なら改行する
static	int			AddLog( const wchar_t *String ) ;										// ログ出力
static	int			ClrLog( void ) ;														// ログの初期化

#endif // DX_NON_PRINTF_DX

// プログラム --------------------------------------------------------------------

// ログファイルパスを作成する
static void CreateErrorLogFilePath( wchar_t *FilePathBuffer, size_t BufferBytes )
{
	int Len ;

	// ログ出力フォルダが指定されていない場合はカレントディレクトリにする
	if( LogData.LogOutDirectory[ 0 ] == L'\0' )
	{
		DX_FGETDIR( FilePathBuffer, BufferBytes ) ; 
	}
	else
	{
		_WCSCPY_S( FilePathBuffer, BufferBytes, LogData.LogOutDirectory ) ;
	}

	Len = ( int )_WCSLEN( FilePathBuffer ) ;
	if( FilePathBuffer[ Len - 1 ] != L'\\' &&
		FilePathBuffer[ Len - 1 ] != L'/' )
	{
		_WCSCAT_S( FilePathBuffer, BufferBytes, L"\\" ) ;
		Len = ( int )_WCSLEN( FilePathBuffer ) ;
	}
	_WCSCPY_S( FilePathBuffer + Len, BufferBytes - Len * sizeof( wchar_t ), LogData.LogFileName ) ;
}

// ログファイルを初期化する
extern int LogFileInitialize( void )
{
	wchar_t LogFilePath[ 1024 ] ;

	// 既に初期化されていたら何もせず終了
	if( LogData.InitializeFlag )
	{
		return 0 ;
	}

	// ログ出力抑制フラグが立っている場合は出力を行わない
	if( LogData.NotLogOutFlag == TRUE )
	{
		return 0 ;
	}

	// ユーザー指定のログファイル名がある場合はそれを使用する
	if( LogData.UserLogFileName[ 0 ] != L'\0' )
	{
		_WCSCPY_S( LogData.LogFileName, sizeof( LogData.LogFileName ), LogData.UserLogFileName ) ;
	}
	else
	// 日付つきのログ出力が指定されている場合はファイル名を作成
	if( LogData.UseLogDateName == TRUE )
	{
		DATEDATA Date ;
		wchar_t String[128] ;

		// 日付を取得
		NS_GetDateTime( &Date ) ;

		// 文字列の作成
		_WCSCPY( LogData.LogFileName, L"Log" ) ;
		_ITOAW( Date.Year, String, 10 ) ;
		_WCSCAT( LogData.LogFileName, String ) ;
		_WCSCAT( LogData.LogFileName, L"_" ) ;
		_ITOAW( Date.Mon, String, 10 ) ;
		_WCSCAT( LogData.LogFileName, String ) ;
		_WCSCAT( LogData.LogFileName, L"_" ) ;
		_ITOAW( Date.Day, String, 10 ) ;
		_WCSCAT( LogData.LogFileName, String ) ;
		_WCSCAT( LogData.LogFileName, L"_" ) ;
		_ITOAW( Date.Hour, String, 10 ) ;
		_WCSCAT( LogData.LogFileName, String ) ;
		_WCSCAT( LogData.LogFileName, L"_" ) ;
		_ITOAW( Date.Min, String, 10 ) ;
		_WCSCAT( LogData.LogFileName, String ) ;
		_WCSCAT( LogData.LogFileName, L"_" ) ;
		_ITOAW( Date.Sec, String, 10 ) ;
		_WCSCAT( LogData.LogFileName, String ) ;
		_WCSCAT( LogData.LogFileName, L".txt" ) ;
	}
	else
	{
		// それ以外の場合は通常のファイル名
		_WCSCPY( LogData.LogFileName, L"Log.txt" ) ;
	}

	// ログファイルのパスを作成
	CreateErrorLogFilePath( LogFilePath, sizeof( LogFilePath ) ) ;

	// 環境依存処理
	if( LogFileInitialize_PF( LogFilePath ) < 0 )
	{
		return -1 ;
	}

	// タブ数を初期化
	LogData.LogFileTabNum = 0 ; 

	// タブ出力ストップフラグを倒す
	LogData.LogFileTabStop = FALSE ;

	// スタート時のタイムスタンプを取得
	LogData.LogStartTime = NS_GetNowCount() ;

	// 初期化フラグを立てる
	LogData.InitializeFlag = TRUE ;

	// 終了
	return 0 ;
}

// ログファイル( Log.txt ) に文字列を出力する
extern int LogFileAdd_WCHAR_T( int ErrorCode/* DX_ERRORCODE_WIN_24BIT_COLOR など*/, const wchar_t *String )
{
	wchar_t			LogFilePath[ FILEPATH_MAX ] ;
	wchar_t *		UseBuffer ;
	wchar_t 		DefaultBuffer[ 1024 ] ;
	wchar_t *		AllocBuffer = NULL ;
	wchar_t *		Dest ;

	// エラーコードが 0 以外だったらエラー情報設定を行う
	if( ErrorCode != 0 )
	{
		DxLib_SetLastError( ErrorCode, String ) ;
	}

	// ログ出力抑制フラグが立っている場合は出力を行わない
	if( LogData.NotLogOutFlag == TRUE )
	{
		return ErrorCode != 0 ? -1 : 0 ;
	}

	// 初期化されていなかったら初期化する
	if( LogData.InitializeFlag == FALSE )
	{
		if( LogFileInitialize() < 0 )
		{
			return -1 ;
		}
	}

	// もしタイムスタンプやタブを含めた文字列がデフォルトのバッファよりも長かったらテンポラリ領域をここで確保する
	{
		DWORD StringLength ;

		StringLength = ( DWORD )_WCSLEN( String ) ;
		if( StringLength * 2 + 16 + LogData.LogFileTabNum > sizeof( DefaultBuffer ) / sizeof( wchar_t ) - 1 )
		{
			AllocBuffer = ( wchar_t * )NS_DxAlloc( ( StringLength * 2 + 16 + LogData.LogFileTabNum ) * sizeof( wchar_t ) ) ;
			UseBuffer   = AllocBuffer ;
		}
		else
		{
			UseBuffer = DefaultBuffer ;
		}
		Dest = UseBuffer ;
	}

	// 規定分だけタブ追加
	if( LogData.LogFileTabStop == FALSE )
	{
		int i ;	

		// タイムスタンプを出力
		if( LogData.NonUseTimeStampFlag == 0 )
		{
			_SWPRINTF( Dest, L"%d:", NS_GetNowCount() - LogData.LogStartTime ) ;
			Dest += _WCSLEN( Dest ) ;
		}

		if( LogData.LogFileTabNum != 0 )
		{
			for( i = 0 ; i < LogData.LogFileTabNum ; i ++, Dest ++ )
			{
				*Dest = L'\t' ;
			}
			*Dest = L'\0' ;
		}
	}

	// \n を \r\n に変換しながら出力文字列をコピー
	{
		const wchar_t *	p1 ;
		const wchar_t *	p2 ;

		p1 = String ;
		p2 = _WCSCHR( p1, L'\n' ) ;
		while( p2 != NULL && ( String == p2 || p2[ -1 ] != L'\r' ) )
		{
			_MEMCPY( Dest, p1, ( p2 - p1 ) * sizeof( wchar_t ) ) ;
			Dest     += p2 - p1 ;
			Dest[ 0 ] = L'\r' ;
			Dest[ 1 ] = L'\n' ;
			Dest[ 2 ] = L'\0' ;
			Dest     += 2 ;
			p1        = p2 + 1 ;
			p2        = _WCSCHR( p1, L'\n' ) ;
		}
		_WCSCPY( Dest, p1 ) ;
	}

	// 最後の文字が改行意外だった場合はタブストップフラグを立てる
	LogData.LogFileTabStop = UseBuffer[ _WCSLEN( UseBuffer ) - 1 ] != L'\n' ? TRUE : FALSE ;

	// ログファイルのパスを作成
	CreateErrorLogFilePath( LogFilePath, sizeof( LogFilePath ) ) ;

	// 環境依存処理
	LogFileAdd_WCHAR_T_PF( LogFilePath, UseBuffer ) ;

	// テンポラリ領域を確保していたら開放する
	if( AllocBuffer != NULL )
	{
		NS_DxFree( AllocBuffer ) ;
	}
	
	// 終了
	return ErrorCode != 0 ? -1 : 0 ;
}















// ログファイルに文字列を書き出す( char版 )
extern int LogFileAddA( const char *String )
{
#ifdef UNICODE
	int Result ;

	SHIFT_JIS_TO_WCHAR_T_STRING_ONE_BEGIN( String, return -1 ) ;

	Result = LogFileAdd_WCHAR_T( 0, UseStringBuffer ) ;

	SHIFT_JIS_TO_WCHAR_T_STRING_END( String ) ;

	return Result ;
#else
	int Result ;

	CHAR_TO_WCHAR_T_STRING_BEGIN( String )
	CHAR_TO_WCHAR_T_STRING_SETUP( String, return -1, DX_CHARCODEFORMAT_SHIFTJIS ) ;

	Result = LogFileAdd_WCHAR_T( 0, UseStringBuffer ) ;

	CHAR_TO_WCHAR_T_STRING_END( String ) ;

	return Result ;
#endif
}

// ログファイルに文字列を書き出す( wchar_t版 )
extern int LogFileAddW( const wchar_t *String )
{
	return LogFileAdd_WCHAR_T( 0, String ) ;
}

// ログファイルに文字列を書き出す( UTF16LE版 )
extern int LogFileAddUTF16LE( const char *String )
{
	int Result ;

	CHAR_TO_WCHAR_T_STRING_BEGIN( String )
	CHAR_TO_WCHAR_T_STRING_SETUP( String, return -1, DX_CHARCODEFORMAT_UTF16LE ) ;

	Result = LogFileAdd_WCHAR_T( 0, UseStringBuffer ) ;

	CHAR_TO_WCHAR_T_STRING_END( String ) ;

	return Result ;
}

// ログファイルに書式付き文字列を書き出す( char版 )
extern int LogFileFmtAddA( const char *FormatString , ... )
{
	va_list VaList ;
	char String[ 2048 ] ;
	
	// ログ出力用のリストをセットする
	va_start( VaList , FormatString ) ;

	// 編集後の文字列を取得する
	CL_vsnprintf( DX_CHARCODEFORMAT_SHIFTJIS, FALSE, DX_CHARCODEFORMAT_SHIFTJIS, WCHAR_T_CHARCODEFORMAT, String , sizeof( String ), FormatString , VaList ) ;
	
	// 可変長リストのポインタをリセットする
	va_end( VaList ) ;

	// 改行文字を追加する
	_STRCAT_S( String, sizeof( String ), "\n" ) ;

	// ログ出力する
	return LogFileAddA( String ) ;
}

// ログファイルに書式付き文字列を書き出す( wchar_t版 )
extern int LogFileFmtAddW( const wchar_t *FormatString , ... )
{
	va_list VaList ;
	wchar_t String[ 2048 ] ;
	
	// ログ出力用のリストをセットする
	va_start( VaList , FormatString ) ;

	// 編集後の文字列を取得する
	_VSWNPRINTF( String, sizeof( String ) / 2, FormatString, VaList ) ;
	
	// 可変長リストのポインタをリセットする
	va_end( VaList ) ;

	// 改行文字を追加する
	_WCSCAT_S( String, sizeof( String ), L"\n" ) ;

	// ログ出力する
	return LogFileAddW( String ) ;
}

// ログファイルに書式付き文字列を書き出す( UTF16LE版 )
extern int LogFileFmtAddUTF16LE( const char *FormatString , ... )
{
	va_list VaList ;
	char String[ 2048 ] ;
	BYTE UTF16LE_EN_N[ 4 ] = { '\n', 0, 0, 0 } ;
	
	// ログ出力用のリストをセットする
	va_start( VaList , FormatString ) ;

	// 編集後の文字列を取得する
	CL_vsnprintf( DX_CHARCODEFORMAT_UTF16LE, TRUE, CHAR_CHARCODEFORMAT, DX_CHARCODEFORMAT_UTF16LE, String, sizeof( String ) / 2, FormatString, VaList ) ;
	
	// 可変長リストのポインタをリセットする
	va_end( VaList ) ;

	// 改行文字を追加する
	CL_strcat_s( DX_CHARCODEFORMAT_UTF16LE, String, sizeof( String ), ( const char * )UTF16LE_EN_N ) ;

	// ログ出力する
	return LogFileAddUTF16LE( String ) ;
}










// ログファイルに文字列を書き出す、エラーコード設定つき( char版 )
extern int LogFileAddWithErrorCode_A( int ErrorCode/* DX_ERRORCODE_WIN_24BIT_COLOR など*/, const char *String )
{
#ifdef UNICODE
	int Result ;

	SHIFT_JIS_TO_WCHAR_T_STRING_ONE_BEGIN( String, return -1 ) ;

	Result = LogFileAdd_WCHAR_T( ErrorCode, UseStringBuffer ) ;

	SHIFT_JIS_TO_WCHAR_T_STRING_END( String ) ;

	return Result ;
#else
	int Result ;

	CHAR_TO_WCHAR_T_STRING_BEGIN( String )
	CHAR_TO_WCHAR_T_STRING_SETUP( String, return -1, DX_CHARCODEFORMAT_SHIFTJIS ) ;

	Result = LogFileAdd_WCHAR_T( ErrorCode, UseStringBuffer ) ;

	CHAR_TO_WCHAR_T_STRING_END( String ) ;

	return Result ;
#endif
}

// ログファイルに文字列を書き出す、エラーコード設定つき( wchar_t版 )
extern int LogFileAddWithErrorCode_W( int ErrorCode/* DX_ERRORCODE_WIN_24BIT_COLOR など*/, const wchar_t *String )
{
	return LogFileAdd_WCHAR_T( ErrorCode, String ) ;
}

// ログファイルに文字列を書き出す、エラーコード設定つき( UTF16LE版 )
extern int LogFileAddWithErrorCode_UTF16LE( int ErrorCode/* DX_ERRORCODE_WIN_24BIT_COLOR など*/, const char *String )
{
	int Result ;

	CHAR_TO_WCHAR_T_STRING_BEGIN( String )
	CHAR_TO_WCHAR_T_STRING_SETUP( String, return -1, DX_CHARCODEFORMAT_UTF16LE ) ;

	Result = LogFileAdd_WCHAR_T( ErrorCode, UseStringBuffer ) ;

	CHAR_TO_WCHAR_T_STRING_END( String ) ;

	return Result ;
}

// ログファイルに書式付き文字列を書き出す、エラーコード設定つき( char版 )
extern int LogFileFmtAddWithErrorCode_A( int ErrorCode/* DX_ERRORCODE_WIN_24BIT_COLOR など*/, const char *FormatString , ... )
{
	va_list VaList ;
	char String[ 2048 ] ;
	
	// ログ出力用のリストをセットする
	va_start( VaList , FormatString ) ;

	// 編集後の文字列を取得する
	_VSNPRINTF( String , sizeof( String ), FormatString , VaList ) ;
	
	// 可変長リストのポインタをリセットする
	va_end( VaList ) ;

	// 改行文字を追加する
	_STRCAT_S( String, sizeof( String ), "\n" ) ;

	// ログ出力する
	return LogFileAddWithErrorCode_A( ErrorCode, String ) ;
}

// ログファイルに書式付き文字列を書き出す、エラーコード設定つき( wchar_t版 )
extern int LogFileFmtAddWithErrorCode_W( int ErrorCode/* DX_ERRORCODE_WIN_24BIT_COLOR など*/, const wchar_t *FormatString , ... )
{
	va_list VaList ;
	wchar_t String[ 2048 ] ;
	
	// ログ出力用のリストをセットする
	va_start( VaList , FormatString ) ;

	// 編集後の文字列を取得する
	_VSWNPRINTF( String, sizeof( String ) / 2, FormatString, VaList ) ;
	
	// 可変長リストのポインタをリセットする
	va_end( VaList ) ;

	// 改行文字を追加する
	_WCSCAT_S( String, sizeof( String ), L"\n" ) ;

	// ログ出力する
	return LogFileAddWithErrorCode_W( ErrorCode, String ) ;
}

// ログファイルに書式付き文字列を書き出す、エラーコード設定つき( UTF16LE版 )
extern int LogFileFmtAddWithErrorCode_UTF16LE( int ErrorCode/* DX_ERRORCODE_WIN_24BIT_COLOR など*/, const char *FormatString , ... )
{
	va_list VaList ;
	char String[ 2048 ] ;
	BYTE UTF16LE_EN_N[ 4 ] = { '\n', 0, 0, 0 } ;
	
	// ログ出力用のリストをセットする
	va_start( VaList , FormatString ) ;

	// 編集後の文字列を取得する
	CL_vsnprintf( DX_CHARCODEFORMAT_UTF16LE, TRUE, CHAR_CHARCODEFORMAT, DX_CHARCODEFORMAT_UTF16LE, String, sizeof( String ) / 2, FormatString, VaList ) ;
	
	// 可変長リストのポインタをリセットする
	va_end( VaList ) ;

	// 改行文字を追加する
	CL_strcat_s( DX_CHARCODEFORMAT_UTF16LE, String, sizeof( String ), ( const char * )UTF16LE_EN_N ) ;

	// ログ出力する
	return LogFileAddWithErrorCode_UTF16LE( ErrorCode, String ) ;
}
















// LogFileAdd の旧名称関数
extern int NS_ErrorLogAdd( const TCHAR *String )
{
	NS_LogFileAdd( String ) ;
	return -1 ;
}

// ログファイル( Log.txt ) に文字列を出力する
extern int NS_LogFileAdd( const TCHAR *String )
{
#ifdef UNICODE
	return LogFileAdd_WCHAR_T( 0, String ) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( String, return -1 ) ;

	Result = LogFileAdd_WCHAR_T( 0, UseStringBuffer ) ;

	TCHAR_TO_WCHAR_T_STRING_END( String )

	return Result ;
#endif
}

// ログファイル( Log.txt ) に文字列を出力する
extern int NS_LogFileAddWithStrLen( const TCHAR *String, size_t StringLength )
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( String, StringLength, return -1 )
	Result = LogFileAdd_WCHAR_T( 0, UseStringBuffer ) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( String )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( String, StringLength, return -1 )
	Result = LogFileAdd_WCHAR_T( 0, UseStringBuffer ) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( String )
#endif
	return Result ;
}

// LogFileFmtAdd の旧名称関数
extern int NS_ErrorLogFmtAdd( const TCHAR *FormatString , ... )
{
	TCHAR_FORMATSTRING_SETUP

	// 改行文字を追加する
	_TSTRCAT_S( String, sizeof( String ), _T( "\n" ) ) ;

	// ログ出力する
	NS_LogFileAdd( String ) ;
	
	return -1 ;
}

// 書式付きで ログファイル( Log.txt ) に文字列を出力する( 書式は printf と同じ )
extern int NS_LogFileFmtAdd( const TCHAR *FormatString , ... )
{
	int Result ;

	TCHAR_FORMATSTRING_SETUP

	// 改行文字を追加する
	_TSTRCAT_S( String, sizeof( String ), _T( "\n" ) ) ;

	// ログ出力する
	Result = NS_LogFileAdd( String ) ;
	
	return Result ;
}

// 書式付きで ログファイル( Log.txt ) に文字列を出力する( 書式は printf と同じ )
extern int LogFileFmt_WCHAR_T( int ErrorCode/* DX_ERRORCODE_WIN_24BIT_COLOR など*/, const wchar_t *FormatString , ... )
{
	int Result ;

	WCHAR_T_FORMATSTRING_SETUP

	// 改行文字を追加する
	_WCSCAT_S( String, sizeof( String ), L"\n" ) ;

	// ログ出力する
	Result = LogFileAdd_WCHAR_T( ErrorCode, String ) ;
	
	return Result ;
}


// LogFileTabAdd の旧名称関数
extern int NS_ErrorLogTabAdd( void )
{
	return NS_LogFileTabAdd() ;
}

// ログファイル( Log.txt ) に出力する文字列の前に付けるタブの数を一つ増やす
extern int NS_LogFileTabAdd( void )
{
	LogData.LogFileTabNum ++ ;

	return 0 ;
}

// LogFileTabSub の旧名称関数
extern int NS_ErrorLogTabSub( void )
{
	return NS_LogFileTabSub() ;
}

// ログファイル( Log.txt ) に出力する文字列の前に付けるタブの数を一つ減らす
extern int NS_LogFileTabSub( void )
{
	if( LogData.LogFileTabNum != 0 ) LogData.LogFileTabNum -- ;

	return 0 ;
}

// タイムスタンプの有無を設定する
extern int NS_SetUseTimeStampFlag( int UseFlag )
{
	LogData.NonUseTimeStampFlag = UseFlag != 0 ? 0 : 1 ;

	return 0 ;
}

// 書式付きログ文字列を書き出す
extern int NS_AppLogAdd( const TCHAR *String, ... )
{
	va_list VaList ;
	TCHAR StringBuf[ 2048 ] ;

	va_start( VaList, String ) ;
	_TVSNPRINTF( StringBuf, sizeof( StringBuf ) / sizeof( TCHAR ), String, VaList ) ;
	va_end( VaList ) ;

	return NS_ErrorLogAdd( StringBuf ) ;
}

// ログファイルの後始末
extern int LogFileTerminate( void )
{
	// 環境依存処理を行う
	LogFileTerminate_PF() ;

	// 終了
	return 0 ; 
}

// ログファイル名に日付をつけるかどうかをセットする
extern int NS_SetUseDateNameLogFile( int Flag )
{
	LogData.UseLogDateName = Flag ;

	// 終了
	return 0 ;
}

// ログ出力を行うか否かのセット
extern int NS_SetOutApplicationLogValidFlag( int Flag )
{
	// フラグセット
	LogData.NotLogOutFlag = !Flag ;

	// 終了
	return 0 ;
}

// ログファイルを保存するディレクトリパスを設定する
extern int NS_SetApplicationLogSaveDirectory( const TCHAR *DirectoryPath )
{
#ifdef UNICODE
	return SetApplicationLogSaveDirectory_WCHAR_T( DirectoryPath ) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( DirectoryPath, return -1 )

	Result = SetApplicationLogSaveDirectory_WCHAR_T( UseDirectoryPathBuffer ) ;

	TCHAR_TO_WCHAR_T_STRING_END( DirectoryPath )

	return Result ;
#endif
}

// ログファイル( Log.txt ) を保存するディレクトリパスを設定する
extern int NS_SetApplicationLogSaveDirectoryWithStrLen( const TCHAR *DirectoryPath, size_t DirectoryPathLength )
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( DirectoryPath, DirectoryPathLength, return -1 )
	Result = SetApplicationLogSaveDirectory_WCHAR_T( UseDirectoryPathBuffer ) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( DirectoryPath )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( DirectoryPath, DirectoryPathLength, return -1 )
	Result = SetApplicationLogSaveDirectory_WCHAR_T( UseDirectoryPathBuffer ) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( DirectoryPath )
#endif
	return Result ;
}

// ログファイルを保存するディレクトリパスを設定する
extern int SetApplicationLogSaveDirectory_WCHAR_T( const wchar_t *DirectoryPath )
{
	// パスの長さがバッファのサイズを超える場合はエラー
	if( ( _WCSLEN( DirectoryPath ) + 1 ) * sizeof( wchar_t ) > sizeof( LogData.LogOutDirectory ) )
	{
		return -1 ;
	}

	// パスの保存
	_WCSCPY( LogData.LogOutDirectory, DirectoryPath ) ;

	// 終了
	return 0 ;
}

// ログファイルの名前を設定する( Log.txt 以外にしたい場合に使用 )
extern int NS_SetApplicationLogFileName( const TCHAR *FileName )
{
#ifdef UNICODE
	return SetApplicationLogFileName_WCHAR_T( FileName ) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, return -1 )

	Result = SetApplicationLogFileName_WCHAR_T( UseFileNameBuffer ) ;

	TCHAR_TO_WCHAR_T_STRING_END( FileName )

	return Result ;
#endif
}

// ログファイルの名前を設定する( Log.txt 以外にしたい場合に使用 )
extern int NS_SetApplicationLogFileNameWithStrLen( const TCHAR *FileName, size_t FileNameLength )
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = SetApplicationLogFileName_WCHAR_T( UseFileNameBuffer ) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = SetApplicationLogFileName_WCHAR_T( UseFileNameBuffer ) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#endif
	return Result ;
}

// ログファイルの名前を設定する( Log.txt 以外にしたい場合に使用 )
extern int SetApplicationLogFileName_WCHAR_T( const wchar_t *FileName )
{
	// ファイル名がバッファのサイズを超える場合はエラー
	if( ( _WCSLEN( FileName ) + 1 ) * sizeof( wchar_t ) > sizeof( LogData.UserLogFileName ) )
	{
		return -1 ;
	}

	// ファイル名の保存
	_WCSCPY( LogData.UserLogFileName, FileName ) ;

	// 終了
	return 0 ;
}









#ifndef DX_NON_PRINTF_DX

// ログ出力機能関数

// ログ機能の初期化
extern int InitializeLog( void )
{
	if( IsInitializeLog() == FALSE ) return -1 ;

	// 既に初期化されていたら何もしない
	if( LogData.LogInitializeFlag == TRUE ) return 0 ;

	// 初期化フラグを立てる
	LogData.LogInitializeFlag = TRUE ;

	// ログフォントのサイズをセット
	LogData.LogFontSize = LOG_FONTSIZE ;

	// フォントの作成
	LogData.LogFontHandle = -1 ;
	LogData.LogFontHandleLostFlag = TRUE ;

	// ログの有り無しフラグをセット
	LogData.LogDrawFlag = FALSE ;

	// ログのファイル出力有り無しフラグをセット
	LogData.LogFileOutFlag = FALSE ;

	// ログの幅をセット
	LogData.LogTabWidth = LOG_TABWIDTH ;

	// カーソルの位置をセット
	LogData.LogX = LogData.LogY = 0 ;

	// ストリングを初期化
	_MEMSET( LogData.LogString, 0, sizeof( LogData.LogString ) ) ;

	// 終了
	return 0 ;
}

// ログ処理の後始末を行う
extern int TerminateLog( void )
{
	// ログ処理が初期化されていなかったなにもしない
	if( LogData.LogInitializeFlag == FALSE ) return 0 ;

	// フォントハンドルを削除する
	if( LogData.LogFontHandleLostFlag == FALSE && LogData.LogFontHandle > 0 )
	{
		NS_DeleteFontToHandle( LogData.LogFontHandle ) ;
	}
	LogData.LogFontHandleLostFlag = TRUE ;
	LogData.LogFontHandle = -1 ;

	// 初期化フラグを倒す
	LogData.LogInitializeFlag = FALSE ;

	// 終了
	return 0 ;
}

// ログ処理で使用しているフォントを更新する
static int RefreshLogFont( void )
{
	if( LogData.LogInitializeFlag == FALSE )
	{
		if( InitializeLog() < 0 )
		{
			return 0 ;
		}
	}

	if( LogData.LogFontHandleLostFlag == FALSE )
	{
		return 0 ;
	}

	// フォントハンドルを作成する
	CREATEFONTTOHANDLE_GPARAM GParam ;
	InitCreateFontToHandleGParam( &GParam ) ;
	LogData.LogFontHandle = CreateFontToHandle_UseGParam( &GParam, NULL, LogData.LogFontSize, LOG_FONTTICK, DX_FONTTYPE_EDGE, -1, -1, FALSE, -1, FALSE ) ;
	if( LogData.LogFontHandle >= 0 )
	{
		// フォントハンドルが削除された際に立つフラグのポインタをセットする
		NS_SetFontLostFlag( LogData.LogFontHandle, &LogData.LogFontHandleLostFlag  ) ;
	}
	else
	{
		LogData.LogFontHandle = DX_DEFAULT_FONT_HANDLE ;
	}

	LogData.LogFontHandleLostFlag = FALSE ;

	return 0 ;
}

// 改行処理
static int CrLog( void )
{
	int ScWidth, ScHeight, StrHeight ;
	int FontSize = -1 ;

	if( LogData.LogInitializeFlag == FALSE )
	{
		if( InitializeLog() < 0 )
		{
			return 0 ;
		}
	}

	// フォントが削除されている場合は再作成する
	RefreshLogFont() ;

	// 画面のサイズを取得
	NS_GetDrawScreenSize( &ScWidth , &ScHeight ) ;
	
	// 画面内に収まる行数を算出
	if( LogData.LogUserFontHandle > 0 )
	{
		FontSize = NS_GetFontSizeToHandle( LogData.LogUserFontHandle ) ;
	}
	if( FontSize < 0 )
	{
		LogData.LogUserFontHandle = 0 ;
		FontSize = LogData.LogFontSize ;
	}
	StrHeight = ScHeight / FontSize ;
	if( StrHeight > LOG_MAXHEIGHT ) StrHeight = LOG_MAXHEIGHT ;

	// 改行したら画面から外れてしまわないか調べ、外れるかどうかで処理を分岐
	if( StrHeight <= LogData.LogY + 1 )
	{
		// 外れる場合は一行分データを全て上げる

		// 文字列を１行分ずらす
		_MEMMOVE( LogData.LogString[0], LogData.LogString[1], sizeof( wchar_t ) * LOG_MAXLENGTH * LogData.LogY ) ;
	}
	else
	{
		// Ｙ座標をインクリメント
		LogData.LogY ++ ;
	}

	// 新しい行の文字列を初期化する
	_MEMSET( LogData.LogString[ LogData.LogY ], 0, sizeof( wchar_t ) * LOG_MAXLENGTH ) ;

	// 描画幅を初期化
	LogData.LogDrawWidth = 0 ;

	// Ｘ座標を初期化する
	LogData.LogX = 0 ;

	// 終了
	return 0 ;
}

// 一文字ログに追加する、必要なら改行する
static int AddCharLog( const wchar_t *C )
{
	int ScWidth, ScHeight ;
	int Width = 0, Length, i ;

	if( LogData.LogInitializeFlag == FALSE )
	{
		if( InitializeLog() < 0 )
		{
			return 0 ;
		}
	}

	// フォントが削除されている場合は再作成する
	RefreshLogFont() ;

	// 画面のサイズを取得
	NS_GetDrawScreenSize( &ScWidth , &ScHeight ) ;

	// 新しい文字の描画幅を取得
	if( CHECK_WCHAR_T_DOUBLE( *C ) == TRUE )
	{
		Length = 2 ;
	}
	else
	{
		Length = 1 ;
	}

	// 文字列がバッファをオーバーしないか調べる、出る場合は改行する
	if( LogData.LogX + Length >= LOG_MAXLENGTH )
	{
		CrLog() ;
	}
	else
	{
		// 画面外に出ないか調べる、出る場合は改行する
		Width = GetDrawStringWidthToHandle_WCHAR_T( C, 0, Length, LogData.LogFontHandle, FALSE ) ;
		if( Width + LogData.LogDrawWidth >= ScWidth ) 
		{
			CrLog() ;
		}
	}

	// 描画幅を加算
	LogData.LogDrawWidth += Width ;

	// 文字を追加
	for( i = 0 ; i < Length ; i ++ )
	{
		LogData.LogString[ LogData.LogY ][ LogData.LogX + i ] = C[ i ]  ;
	}
	LogData.LogString[ LogData.LogY ][ LogData.LogX + i ] = L'\0' ;

	// カーソル位置をインクリメント
	LogData.LogX += Length  ;

	// 終了
	return Length ;
}


// ログ出力
static int AddLog( const wchar_t *String )
{
	int StrLen ;

	if( LogData.LogInitializeFlag == FALSE )
	{
		if( InitializeLog() < 0 )
		{
			return 0 ;
		}
	}

	// フォントが削除されている場合は再作成する
	RefreshLogFont() ;

	// 一文字づつ追加していく
	{
		int i ;
		const wchar_t *C ;

		// 文字が途切れるまで繰り返し
		StrLen = ( int )_WCSLEN( String ) ;
		for( i = 0 ; i < StrLen ; i ++ )
		{
			C = &String[ i ] ;

			if( i == 43 )
			{
				i = 43 ;
			}

			// 次の文字のタイプによって処理を分岐
			switch( *C )
			{
			case L'\n' :
				// 改行コードの場合は改行処理を行う
				CrLog() ;
				break ;

			case L'\t' :
				{
					int j ;

					// タブの場合はタブの文字数分スペースを追加
					for( j = 0 ; j < LogData.LogTabWidth ; j ++ )
						AddCharLog( L" " )  ;
				}
				break ;

			case L'\a' : case L'\b' : case L'\?' : 
			case L'\f' : case L'\r' : case L'\v' :
				break ;

			default :
				// それ以外の場合は普通に追加
				if( AddCharLog( C ) == 2 ) i ++ ;
				break ;
			}
		}
	}

	// 終了
	return 0 ;
}

// ログの初期化
static int ClrLog( void )
{
	if( LogData.LogInitializeFlag == FALSE )
	{
		if( InitializeLog() < 0 )
		{
			return 0 ;
		}
	}

	// フォントが削除されている場合は再作成する
	RefreshLogFont() ;

	// ログを初期化
	_MEMSET( LogData.LogString, 0, sizeof( LogData.LogString ) ) ;

	// カーソル位置を初期化
	LogData.LogX = LogData.LogY = 0 ;
	LogData.LogDrawWidth = 0 ;

	// 終了
	return 0 ;
}

// ログ出力フラグをセットする
extern int NS_SetLogDrawOutFlag( int DrawFlag )
{
	// ログ描画出力フラグをセットする
	LogData.LogDrawFlag = DrawFlag ;

	// 終了
	return 0 ;
}

// ログ出力をするかフラグの取得
extern int NS_GetLogDrawFlag( void )
{
	return LogData.LogDrawFlag ;
}

// printfDx で画面に出力するログフォントのサイズを変更する
extern int NS_SetLogFontSize( int Size )
{
	if( Size < 0 ) Size = LOG_FONTSIZE;

	LogData.LogFontSize = Size;

	if( LogData.LogFontHandle >= 0 )
	{
		DeleteFontToHandle( LogData.LogFontHandle );
		LogData.LogFontHandle = -1;
		LogData.LogFontHandleLostFlag = TRUE;
	}

	RefreshLogFont();

	// 終了
	return 0;
}

// printfDx の結果を画面に出力する際に使用するフォントのハンドルを変更する
extern int NS_SetLogFontHandle( int FontHandle )
{
	LogData.LogUserFontHandle = FontHandle ;

	// 終了
	return 0 ;
}

// printfDx の結果を画面に出力する際の描画する領域を設定する
extern int NS_SetLogDrawArea( int x1, int y1, int x2, int y2 )
{
	LogData.LogDrawArea.left   = x1 ;
	LogData.LogDrawArea.top    = y1 ;
	LogData.LogDrawArea.right  = x2 ;
	LogData.LogDrawArea.bottom = y2 ;

	// 終了
	return 0 ;
}

// ログを描画する
extern int DrawLog( void )
{
	int StrHeight , i ;
	int DrawX, DrawY ;
	int DrawW, DrawH ;
	int FontSize = -1 ;
	int FontHandle ;
	int Color ;
	int EdgeColor ;

	if( LogData.LogInitializeFlag == FALSE )
	{
		if( InitializeLog() < 0 )
		{
			return 0 ;
		}
	}

	// 有効な描画範囲が設定されていたら使用する
	DrawX = LogData.LogDrawArea.left ;
	DrawY = LogData.LogDrawArea.top ;
	DrawW = LogData.LogDrawArea.right  - LogData.LogDrawArea.left ;
	DrawH = LogData.LogDrawArea.bottom - LogData.LogDrawArea.top ;
	if( DrawW == 0 || DrawH == 0 )
	{
		// 標準では描画先は画面左上
		DrawX = 0 ;
		DrawY = 0 ;

		// 画面のサイズを取得する
		NS_GetDrawScreenSize( &DrawW , &DrawH ) ;
	}

	// フォントが削除されている場合は再作成する
	RefreshLogFont() ;
	
	// 画面にログを描画する
	if( LogData.LogUserFontHandle > 0 )
	{
		FontSize = NS_GetFontSizeToHandle( LogData.LogUserFontHandle ) ;
	}
	if( FontSize < 0 )
	{
		LogData.LogUserFontHandle = 0 ;
		FontSize = LogData.LogFontSize ;
		FontHandle = LogData.LogFontHandle ;
	}
	else
	{
		FontHandle = LogData.LogUserFontHandle ;
	}
	StrHeight = DrawH / FontSize  ;
	if( StrHeight >= LOG_MAXHEIGHT ) StrHeight = LOG_MAXHEIGHT ;

	Color = NS_GetColor( 255, 255, 255 ) ;
	EdgeColor = NS_GetColor( 0, 0, 0 ) ;
	for( i = 0 ; i < StrHeight ; i ++ )
	{
		DrawStringToHandle_WCHAR_T( DrawX , DrawY + i * FontSize , LogData.LogString[ i ] , _WCSLEN( LogData.LogString[ i ] ) , Color, FontHandle , EdgeColor, FALSE ) ;
	}

	// 終了
	return 0 ;
}






// 簡易画面出力関数
extern int printfDxBase( const TCHAR *String )
{
	int Result ;

#ifdef UNICODE
	Result = printfDxBase_WCHAR_T( String ) ;
#else
	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( String, return -1 )

	Result = printfDxBase_WCHAR_T( UseStringBuffer ) ;

	TCHAR_TO_WCHAR_T_STRING_END( String )
#endif

	// 終了
	return Result ;
}

// 簡易画面出力関数
extern int NS_printfDx( const TCHAR *FormatString , ... )
{
	int Result ;

	TCHAR_FORMATSTRING_SETUP

#ifdef UNICODE
	Result = printfDxBase_WCHAR_T( String ) ;
#else
	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( String, return -1 )

	Result = printfDxBase_WCHAR_T( UseStringBuffer ) ;

	TCHAR_TO_WCHAR_T_STRING_END( String )
#endif

	// 終了
	return Result ;
}

// 簡易画面出力関数
extern int printfDx_WCHAR_T( const wchar_t *FormatString , ... )
{
	int Result ;

	WCHAR_T_FORMATSTRING_SETUP

	// ログ出力フラグを立てる
	LogData.LogDrawFlag = TRUE ;

	// ログ出力関数を呼ぶ
	Result = AddLog( String ) ;

	// 終了
	return Result ;
}

// printfDx_WCHAR_T の書式文字列ではない文字列を引数とするバージョン
extern int printfDxBase_WCHAR_T( const wchar_t *String )
{
	int Result ;

	// ログ出力フラグを立てる
	LogData.LogDrawFlag = TRUE ;

	// ログ出力関数を呼ぶ
	Result = AddLog( String ) ;

	// 終了
	return Result ;
}

// puts と同じ引数で画面に文字列を表示するための関数
extern int NS_putsDx( const TCHAR *String, int NewLine )
{
#ifdef UNICODE
	return putsDx_WCHAR_T( String, NewLine ) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( String, return -1 )

	Result = putsDx_WCHAR_T( UseStringBuffer, NewLine ) ;

	TCHAR_TO_WCHAR_T_STRING_END( String )

	return Result ;
#endif
}

// puts と同じ引数で画面に文字列を表示するための関数
extern int NS_putsDxWithStrLen( const TCHAR *String, size_t StringLength, int NewLine )
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( String, StringLength, return -1 )
	Result = putsDx_WCHAR_T( UseStringBuffer, NewLine ) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( String )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( String, StringLength, return -1 )
	Result = putsDx_WCHAR_T( UseStringBuffer, NewLine ) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( String )
#endif
	return Result ;
}

// puts と同じ引数で画面に文字列を表示するための関数
extern int putsDx_WCHAR_T( const wchar_t *String , int NewLine )
{
	int Result ;

	// ログ出力フラグを立てる
	LogData.LogDrawFlag = TRUE ;

	// ログ出力関数を呼ぶ
	Result = AddLog( String ) ;
	if( Result < 0 )
	{
		return -1 ;
	}

	// 改行を追加
	if( NewLine )
	{
		Result = AddLog( L"\n" ) ;
	}

	// 終了
	return Result ;
}

// 簡易画面出力をクリアする
extern int NS_clsDx( void )
{
	// ログを消す
	ClrLog() ;

	// ログ出力フラグを倒す
	LogData.LogDrawFlag = FALSE ;

	// 終了
	return 0 ;
}

#endif // DX_NON_PRINTF_DX


#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_NON_LOG

