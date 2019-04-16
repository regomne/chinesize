// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		スレッド関係プログラムヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXTHREAD_H__
#define __DXTHREAD_H__

// インクルード ------------------------------------------------------------------
#include "DxCompileConfig.h"

#ifdef __WINDOWS__
#include "Windows/DxThreadWin.h"
#endif // __WINDOWS__

#ifdef __ANDROID__
#include "Android/DxThreadAndroid.h"
#endif // __ANDROID__

#ifdef __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE
		#include "iOS/DxThreadiOS.h"
    #endif // TARGET_OS_IPHONE
#endif // __APPLE__




#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

// 優先順位
#define DX_THREAD_PRIORITY_LOWEST			(0)
#define DX_THREAD_PRIORITY_BELOW_NORMAL		(1)
#define DX_THREAD_PRIORITY_NORMAL			(2)
#define DX_THREAD_PRIORITY_HIGHEST			(3)
#define DX_THREAD_PRIORITY_NUM				(4)

#if !defined( __BCC ) || defined( _DEBUG )
	#define CRITICALSECTION_LOCK( csection )			CriticalSection_Lock( (csection), __FILE__, __LINE__ )
#else
	#define CRITICALSECTION_LOCK( csection )			CriticalSection_Lock( (csection) )
#endif

// 型定義 ------------------------------------------------------------------------

// 構造体定義 --------------------------------------------------------------------

// テーブル-----------------------------------------------------------------------

// 内部大域変数宣言 --------------------------------------------------------------

// 関数プロトタイプ宣言-----------------------------------------------------------

extern	void		Thread_Initialize( void ) ;																				// スレッドの処理を初期化する
extern	int			Thread_Create( THREAD_INFO *pThreadInfo, void ( *pFunction )( THREAD_INFO *, void * ), void *pParam ) ;	// スレッドを作成する
extern	void		Thread_Delete( THREAD_INFO *pThreadInfo ) ;																// スレッドの後始末を行う
extern	int			Thread_IsValid( THREAD_INFO *pThreadInfo ) ;															// スレッドが有効かどうかを取得する( 1:有効  0:無効 )
extern	void		Thread_SetPriority( THREAD_INFO *pThreadInfo, int Priority /* DX_THREAD_PRIORITY_LOWEST など */ ) ;		// スレッドの実行優先順位を設定する
extern	DWORD_PTR	Thread_GetCurrentId( void ) ;																			// カレントスレッドのＩＤを取得する
extern	DWORD_PTR	Thread_GetId( THREAD_INFO *pThreadInfo ) ;																// スレッドのＩＤを取得する
extern	void		Thread_Suspend( THREAD_INFO *pThreadInfo ) ;															// スレッドを休止状態にする
extern	int			Thread_Resume( THREAD_INFO *pThreadInfo ) ;																// スレッドの休止状態を解除する( 0:休止状態じゃなかった  1:休止状態だった )

extern	void		Thread_Sleep( DWORD MiliSecond ) ;																		// 指定時間スレッドを停止する

extern	int			CriticalSection_Initialize( DX_CRITICAL_SECTION *pCSection ) ;									// クリティカルセクションの初期化
extern	int			CriticalSection_Delete( DX_CRITICAL_SECTION *pCSection ) ;										// クリティカルセクションの削除

#if !defined( __BCC ) || defined( _DEBUG )
extern	int			CriticalSection_Lock( DX_CRITICAL_SECTION *pCSection, const char *FilePath, int LineNo ) ;		// クリティカルセクションのロックの取得
#else
extern	int			CriticalSection_Lock( DX_CRITICAL_SECTION *pCSection ) ;										// クリティカルセクションのロックの取得
#endif
extern	int			CriticalSection_Unlock( DX_CRITICAL_SECTION *pCSection ) ;										// クリティカルセクションのロックを解放する

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // __DXTHREAD_H__
