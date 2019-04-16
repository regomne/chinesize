// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		ログプログラムヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXLOG_H__
#define __DXLOG_H__

// インクルード ------------------------------------------------------------------

#include "DxCompileConfig.h"
#include "DxFile.h"

#ifdef __WINDOWS__
#include "Windows/DxLogWin.h"
#endif // __WINDOWS__

#ifdef __ANDROID__
#include "Android/DxLogAndroid.h"
#endif // __ANDROID__

#ifdef __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE
		#include "iOS/DxLogiOS.h"
    #endif // TARGET_OS_IPHONE
#endif // __APPLE__




#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

//ＤＸライブラリ内のリテラル文字列を極力減らすかどうかの定義を反映する為のマクロ
#if !defined( DX_NON_LITERAL_STRING ) && !defined( DX_NON_LOG )
	#define DXSTRING( x )									x
	#define DXST_LOGFILE_ADDA( x )							LogFileAddA( x )
	#define DXST_LOGFILE_ADDW( x )							LogFileAddW( x )
	#define DXST_LOGFILE_ADDUTF16LE( x )					LogFileAddUTF16LE( x )
	#define DXST_LOGFILEFMT_ADDA( x )						LogFileFmtAddA x
	#define DXST_LOGFILEFMT_ADDW( x )						LogFileFmtAddW x
	#define DXST_LOGFILEFMT_ADDUTF16LE( x )					LogFileFmtAddUTF16LE x
	#define DXST_LOGFILE_ERRCODE_ADDA( ercode, x )			LogFileAddWithErrorCode_A( ercode, x )
	#define DXST_LOGFILE_ERRCODE_ADDW( ercode, x )			LogFileAddWithErrorCode_W( ercode, x )
	#define DXST_LOGFILE_ERRCODE_ADDUTF16LE( ercode, x )	LogFileAddWithErrorCode_UTF16LE( ercode, x )
	#define DXST_LOGFILEFMT_ERRCODE_ADDA( x )				LogFileFmtAddWithErrorCode_A x
	#define DXST_LOGFILEFMT_ERRCODE_ADDW( x )				LogFileFmtAddWithErrorCode_W x
	#define DXST_LOGFILEFMT_ERRCODE_ADDUTF16LE( x )			LogFileFmtAddWithErrorCode_UTF16LE x
	#define DXST_LOGFILE_TABADD								NS_LogFileTabAdd()
	#define DXST_LOGFILE_TABSUB								NS_LogFileTabSub()
	#define DXERRORNETLOG_ADD( x )							ErrorNetLogAdd( x )
	#define DXERRORNETWORK( x )								ErrorNetWork x
#else
	#define DXSTRING( x )									" "
	#define DXST_LOGFILE_ADDA( x )							(-1)
	#define DXST_LOGFILE_ADDW( x )							(-1)
	#define DXST_LOGFILE_ADDUTF16LE( x )					(-1)
	#define DXST_LOGFILEFMT_ADDA( x )						(-1)
	#define DXST_LOGFILEFMT_ADDW( x )						(-1)
	#define DXST_LOGFILEFMT_ADDUTF16LE( x )					(-1)
	#define DXST_LOGFILE_ERRCODE_ADDA( ercode, x )			(-1)
	#define DXST_LOGFILE_ERRCODE_ADDW( ercode, x )			(-1)
	#define DXST_LOGFILE_ERRCODE_ADDUTF16LE( ercode, x )	(-1)
	#define DXST_LOGFILEFMT_ERRCODE_ADDA( x )				(-1)
	#define DXST_LOGFILEFMT_ERRCODE_ADDW( x )				(-1)
	#define DXST_LOGFILEFMT_ERRCODE_ADDUTF16LE( x )			(-1)
	#define DXST_LOGFILE_TABADD								(0)
	#define DXST_LOGFILE_TABSUB								(0)
	#define DXERRORNETLOG_ADD( x )							(-1)
	#define DXERRORNETWORK( x )								(-1)
#endif


#ifndef DX_NON_LOG

#define LOG_MAXHEIGHT				(80)				// １画面に入るログの最大行
#define LOG_MAXLENGTH				(255)				// １行のログに使える文字列の長さ

// ログ処理用定義
#define LOG_FONTSIZE				(16)				// ログに使うフォントのサイズ
#define LOG_FONTTICK				(4)					// ログ表示に使うフォントの太さ
#define LOG_TABWIDTH				(4)					// タブ一つ分の幅

// 構造体定義 --------------------------------------------------------------------

// ログ処理データ構造体
struct LOGDATA
{
	int						InitializeFlag ;					// 初期化完了フラグ

	int						NonUseTimeStampFlag ;				// タイムスタンプを出力しないかどうかのフラグ
	int						NotLogOutFlag ;						// ログ出力を行うかフラグ(TRUEで行わない)
	int						LogStartTime ;						// 起動時のタイムスタンプ
	wchar_t					UserLogFileName[ 256 ] ;			// SetApplicationLogFileName で設定されたログファイル名
	wchar_t					LogOutDirectory[ FILEPATH_MAX ] ;	// ログ出力を行うファイルを保存するディレクトリ
	int						UseLogDateName ;					// ログ出力を行うファイルに日付をつけるかどうかのフラグ
	wchar_t					LogFileName[256] ;					// ログ出力を行うファイルの名前
	int						LogFileTabNum ;						// ログ出力時のタブの数
	int						LogFileTabStop ;					// ログ出力時のタブ出力ストップフラグ

#ifndef DX_NON_PRINTF_DX
	int						LogInitializeFlag ;					// ログデータが初期化されているかフラグ
	int						LogUserFontHandle ;					// ユーザー指定のログ描画用フォントハンドル
	int						LogFontHandle ;						// ログ描画用フォントハンドル
	int						LogFontHandleLostFlag ;				// フォントハンドルが削除された際に立つフラグ
	int						LogX ,LogY ;						// 現在のログのカーソル位置
	wchar_t					LogString[LOG_MAXHEIGHT][LOG_MAXLENGTH] ; // ログ文字列
	int						LogDrawFlag ;						// ログを画面上に表示するか、フラグ
	int						LogFontSize ;						// ログフォントのサイズ
	int						LogTabWidth ;						// ログのタブの幅
	int						LogDrawWidth ;						// 現在カーソルがいる行の描画幅
	int						LogFileOutFlag ;					// ログをファイル出力するか、フラグ
	RECT					LogDrawArea ;						// ログを描画する領域
#endif // DX_NON_PRINTF_DX


	LOGDATA_PF				PF ;								// 環境依存情報
} ;

// 内部大域変数宣言 --------------------------------------------------------------

extern LOGDATA LogData ;

// 関数プロトタイプ宣言-----------------------------------------------------------

// ログ出力機能関数
extern	int			LogFileInitialize(		void ) ;										// ログファイルを初期化する
extern	int			LogFileTerminate(		void ) ;										// ログファイルの後始末

extern	int			LogFileAddA(			const char    *String ) ;						// ログファイルに文字列を書き出す( char版 )
extern	int			LogFileAddW(			const wchar_t *String ) ;						// ログファイルに文字列を書き出す( wchar_t版 )
extern	int			LogFileAddUTF16LE(		const char    *String ) ;						// ログファイルに文字列を書き出す( UTF16LE版 )
extern 	int			LogFileFmtAddA(			const char    *FormatString , ... ) ;			// ログファイルに書式付き文字列を書き出す( char版 )
extern 	int			LogFileFmtAddW(			const wchar_t *FormatString , ... ) ;			// ログファイルに書式付き文字列を書き出す( wchar_t版 )
extern 	int			LogFileFmtAddUTF16LE(	const char    *FormatString , ... ) ;			// ログファイルに書式付き文字列を書き出す( UTF16LE版 )

extern	int			LogFileAddWithErrorCode_A(			int ErrorCode/* DX_ERRORCODE_WIN_24BIT_COLOR など*/, const char    *ErrorStr ) ;					// ログファイルに文字列を書き出す、エラーコード設定つき( char版 )
extern	int			LogFileAddWithErrorCode_W(			int ErrorCode/* DX_ERRORCODE_WIN_24BIT_COLOR など*/, const wchar_t *ErrorStr ) ;					// ログファイルに文字列を書き出す、エラーコード設定つき( wchar_t版 )
extern	int			LogFileAddWithErrorCode_UTF16LE(	int ErrorCode/* DX_ERRORCODE_WIN_24BIT_COLOR など*/, const char    *ErrorStr ) ;					// ログファイルに文字列を書き出す、エラーコード設定つき( UTF16LE版 )
extern 	int			LogFileFmtAddWithErrorCode_A(		int ErrorCode/* DX_ERRORCODE_WIN_24BIT_COLOR など*/, const char	   *FormatString , ... ) ;			// ログファイルに書式付き文字列を書き出す、エラーコード設定つき( char版 )
extern 	int			LogFileFmtAddWithErrorCode_W(		int ErrorCode/* DX_ERRORCODE_WIN_24BIT_COLOR など*/, const wchar_t *FormatString , ... ) ;			// ログファイルに書式付き文字列を書き出す、エラーコード設定つき( wchar_t版 )
extern 	int			LogFileFmtAddWithErrorCode_UTF16LE(	int ErrorCode/* DX_ERRORCODE_WIN_24BIT_COLOR など*/, const char    *FormatString , ... ) ;			// ログファイルに書式付き文字列を書き出す、エラーコード設定つき( UTF16LE版 )

// wchar_t版関数
extern	int			LogFileAdd_WCHAR_T(		int ErrorCode/* DX_ERRORCODE_WIN_24BIT_COLOR など*/, const wchar_t *ErrorStr ) ;					// ログファイル( Log.txt ) に文字列を出力する
extern 	int			LogFileFmtAdd_WCHAR_T(	int ErrorCode/* DX_ERRORCODE_WIN_24BIT_COLOR など*/, const wchar_t *FormatString , ... ) ;			// 書式付きで ログファイル( Log.txt ) に文字列を出力する( 書式は printf と同じ )

extern	int			SetApplicationLogSaveDirectory_WCHAR_T(	const wchar_t *DirectoryPath );	// ログファイル( Log.txt ) を保存するディレクトリパスを設定する
extern	int			SetApplicationLogFileName_WCHAR_T(      const wchar_t *FileName ) ;		// ログファイルの名前を設定する( Log.txt 以外にしたい場合に使用 )


// 環境依存関数
extern	int			LogFileInitialize_PF( const wchar_t *LogFilePath ) ;					// ログファイルを初期化する処理の環境依存部分
extern	int			LogFileTerminate_PF( void ) ;											// ログファイルの後始末の環境依存部分
extern	int			LogFileAdd_WCHAR_T_PF( const wchar_t *LogFilePath, const wchar_t *ErrorStr ) ;	// ログファイルへ文字列を書き出す処理の環境依存部分



#ifndef DX_NON_PRINTF_DX

extern	int			IsInitializeLog( void ) ;												// ログ機能の初期化を行うかどうかを取得する
extern	int			InitializeLog( void ) ;													// ログ機能の初期化
extern	int			TerminateLog( void ) ;													// ログ処理の後始末を行う
extern	int			DrawLog( void ) ;														// ログを描画する
extern 	int			printfDxBase( const TCHAR *String ) ;									// printfDx の書式文字列ではない文字列を引数とするバージョン

// wchar_t版関数
extern 	int			printfDx_WCHAR_T(		 const wchar_t *FormatString , ... ) ;				// printf と同じ引数で画面に文字列を表示するための関数
extern 	int			printfDxBase_WCHAR_T(	 const wchar_t *String ) ;							// printfDx_WCHAR_T の書式文字列ではない文字列を引数とするバージョン
extern 	int			putsDx_WCHAR_T(			 const wchar_t *String , int NewLine ) ;			// puts と同じ引数で画面に文字列を表示するための関数

#endif // DX_NON_PRINTF_DX

#endif // DX_NON_LOG

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // __DXLOG_H__

