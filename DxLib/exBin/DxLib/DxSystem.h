// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		システムプログラムヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXSYSTEM_H__
#define __DXSYSTEM_H__

// インクルード ------------------------------------------------------------------
#include "DxCompileConfig.h"
#include "DxLib.h"
#include "DxStatic.h"
#include "DxBaseFunc.h"

#ifdef __WINDOWS__
#include "Windows/DxSystemWin.h"
#endif

#ifdef __ANDROID__
#include "Android/DxSystemAndroid.h"
#endif // __ANDROID__

#ifdef __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE
		#include "iOS/DxSystemiOS.h"
    #endif // TARGET_OS_IPHONE
#endif // __APPLE__




#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------


// 構造体定義 --------------------------------------------------------------------

// システムデータ構造体
struct DXSYSTEMDATA
{
	int						DxLib_InitializeFlag ;				// ＤＸライブラリの初期化が完了しているかフラグ
	int						DxLib_RunInitializeFlag ;			// ＤＸライブラリの初期化中か、フラグ

	int						NotWinFlag ;						// ＤＸライブラリのウインドウ関連機能を使用しないフラグ
	int						NotDrawFlag ;						// 描画機能を使用しないフラグ
	int						NotSoundFlag ;						// サウンド機能を使用しないフラグ
	int						NotInputFlag ;						// 入力状態取得機能を使用しないフラグ

	int						EndRequestFlag ;					// 終了リクエストフラグ
	int						ASyncLoadFlag ;						// 非同期読み込みフラグ

	int						LastErrorCode ;						// 最後に発生したエラーのエラーコード( DX_ERRORCODE_WIN_24BIT_COLOR など )
	wchar_t					LastErrorMessage[ 4096 ] ;			// 最後に発生したエラーのエラーメッセージ
} ;

// テーブル-----------------------------------------------------------------------

// 内部大域変数宣言 --------------------------------------------------------------

extern DXSYSTEMDATA DxSysData ;

// 関数プロトタイプ宣言-----------------------------------------------------------

// 初期化・終了関係
extern	int			DxLib_SysInit( void ) ;													// DxSysData 関係の初期化を行う
extern	int			DxLib_EndRequest( void ) ;												// 終了リクエストを行う
extern	int			DxLib_GetEndRequest( void ) ;											// 終了リクエストの状態を取得する

// エラー処理関数
extern	int			DxLib_SetLastError(    int ErrorCode/* DX_ERRORCODE_WIN_24BIT_COLOR など */, const wchar_t *ErrorMessage ) ;			// エラーコード・メッセージを設定する

extern	int			DxLib_Error(           const wchar_t *ErrorStr ) ;				// ライブラリのエラー処理を行う
extern	int			DxLib_ErrorUTF16LE(    const char    *ErrorStr ) ;				// ライブラリのエラー処理を行う( UTF16LE版 )
extern	int			DxLib_FmtError(        const wchar_t *FormatString, ... ) ;		// 書式付きライブラリのエラー処理を行う
extern	int			DxLib_FmtErrorUTF16LE( const char    *FormatString, ... ) ;		// 書式付きライブラリのエラー処理を行う( UTF16LE版 )

// スクリーン座標をＤＸライブラリ画面座標に変換する
extern	int			ConvScreenPositionToDxScreenPosition( int ScreenX, int ScreenY, int *DxScreenX, int *DxScreenY ) ;

// 非同期読み込み関係
extern	int			GetASyncLoadFlag( void ) ;												// 非同期読み込みを行うかどうかを取得する

// 環境依存関数
extern	int			GetClipboardText_PF(					TCHAR   *DestBuffer ) ;						// クリップボードに格納されているテキストデータを読み出す( DestBuffer:文字列を格納するバッファの先頭アドレス   戻り値  -1:クリップボードにテキストデータが無い  -1以外:クリップボードに格納されている文字列データのサイズ( 単位:byte ) ) 
extern	int			GetClipboardText_WCHAR_T_PF(			wchar_t *DestBuffer ) ;						// クリップボードに格納されているテキストデータを読み出す( DestBuffer:文字列を格納するバッファの先頭アドレス   戻り値  -1:クリップボードにテキストデータが無い  -1以外:クリップボードに格納されている文字列データのサイズ( 単位:byte ) ) 
extern	int			SetClipboardText_WCHAR_T_PF(			const wchar_t *Text                    ) ;	// クリップボードにテキストデータを格納する

// wchar_t版関数
extern	int			GetClipboardText_WCHAR_T(		    wchar_t *DestBuffer ) ;
extern	int			SetClipboardText_WCHAR_T(		    const wchar_t *Text ) ;
extern	int			SetClipboardTextWithStrLen_WCHAR_T( const wchar_t *Text, size_t TextLength ) ;

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // __DXSYSTEM_H__

