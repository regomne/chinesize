// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		非同期読み込み処理プログラムヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXASYNCLOAD_H__
#define __DXASYNCLOAD_H__

// インクルード ------------------------------------------------------------------
#include "DxCompileConfig.h"

#ifndef DX_NON_ASYNCLOAD

#include "DxLib.h"
#include "DxThread.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

// 非同期読み込み処理のジョブタイプ
#define ASYNCLOAD_JOBTYPE_ADD_DATA					(0)		// データ追加
#define ASYNCLOAD_JOBTYPE_DEL_DATA					(1)		// データ削除
#define ASYNCLOAD_JOBTYPE_ERROR_DATA				(2)		// データエラー
#define ASYNCLOAD_JOBTYPE_RELEASE_OTHER_THREAD		(3)		// その他スレッド担当扱いの解除

// 非同期読み込みデータの最大ストック数
#define ASYNCLOADDATA_MAXNUM				(32768)

// 非同期読み込みスレッドの最大数
#define ASYNCLOADTHREAD_MAXNUM				(32)

// 構造体定義 --------------------------------------------------------------------

// 非同期読み込み処理用共通データ構造体
struct ASYNCLOADDATA_COMMON
{
	int						Index ;								// 非同期読み込みデータの配列番号
	int						Run ;								// 読み込み処理を開始しているかどうか
	int						StartTime ;							// データを登録した際の時間
	void					( *ProcessFunction )( struct ASYNCLOADDATA_COMMON *Data ) ;
	BYTE					Data[ 4 ] ;
} ;

// メインスレッドへの処理依頼データ構造体
struct ASYNCLOAD_MAINTHREAD_REQUESTINFO
{
	int						( *Function )( struct ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info ) ;		// 依頼処理を行う関数
	int						ThreadNo ;																// 依頼を出したスレッドの番号
	volatile int			Result ;																// 処理結果
	volatile DWORD_PTR		Data[ 16 ] ;															// 関数で使用する情報
} ;

// 非同期読み込みスレッドに必要な情報を纏めた構造体
struct ASYNCLOADTHREADINFO
{
	THREAD_INFO				ThreadInfo ;						// スレッドの情報
	DWORD					ExitFlag ;							// スレッドが終了時に立てるフラグ
	int						JobFlag ;							// 現在仕事をしているかどうかのフラグ
	int						MainThreadRequest ;					// メインスレッドへのリクエストを行っている最中かのフラグ
	int						SuspendFlag ;						// スレッドがとまっているかどうかのフラグ
	int						SuspendStartTime ;					// スレッドがとまったときの時間
	struct ASYNCLOADDATA_COMMON *Data ;							// 処理中のデータ
} ;

// 非同期読み込み処理が使用するグローバルデータの構造体
struct ASYNCLOADDATA
{
	int						InitializeFlag ;								// 初期化フラグ

	DWORD_PTR				MainThreadID ;									// メインスレッドのＩＤ
	int						ThreadEndRequestFlag ;							// 非同期読み込み処理の終了を促すフラグ
	int						ThreadNum ;										// 非同期読み込み処理を行うスレッドの数
	int						ThreadMaxResumeNum ;							// 非同期読み込み処理を同時に行うスレッドの数
	ASYNCLOADTHREADINFO		Thread[ ASYNCLOADTHREAD_MAXNUM ] ;				// 非同期読み込み処理を行うスレッドの情報
	volatile int			ThreadResumeNum ;								// 動作している非同期読み込みスレッドの数
	DX_CRITICAL_SECTION		CriticalSection ;								// 非同期読み込み処理全般( 非同期読み込みスレッドへのメッセージ＋非同期読み込みデータアクセス時使用 )で使用するクリティカルセクション

	ASYNCLOAD_MAINTHREAD_REQUESTINFO	*MainThreadRequestInfo[ ASYNCLOADTHREAD_MAXNUM ] ;		// メインスレッドへの処理依頼
	volatile int						MainThreadRequestInfoNum ;								// メインスレッドへの処理依頼の数

	ASYNCLOADDATA_COMMON	*Data[ ASYNCLOADDATA_MAXNUM ] ;					// 非同期読み込みデータのポインタ配列へのポインタ
	int						DataNum ;										// 非同期読み込みデータの数
	int						DataArea ;										// 非同期読み込みデータがある範囲
} ;

// 内部大域変数宣言 --------------------------------------------------------------

// 非同期読み込み処理が使用するグローバルデータ
extern ASYNCLOADDATA GASyncLoadData ;

// 関数プロトタイプ宣言-----------------------------------------------------------

// 非同期読み込み処理の初期化と後始末
extern	int		InitializeASyncLoad( DWORD_PTR MainThreadID ) ;										// 非同期読み込み処理の初期化
extern	int		SetupASyncLoadThread( int ProcessorNum ) ;											// 非同期読み込み処理を行うスレッドを立てる
extern	int		CloseASyncLoadThread( void ) ;														// 非同期読み込み処理のスレッドを閉じる
extern	int		TerminateASyncLoad( void ) ;														// 非同期読み込み処理の後始末


// 非同期読み込み関係
extern	ASYNCLOADDATA_COMMON *AllocASyncLoadDataMemory( int AddAllocSize ) ;						// 非同期読み込みデータを格納するメモリ領域を確保する
extern	void		AddASyncLoadParamStruct( BYTE *Data, int *Addr, const void *Param, int Size ) ;	// 非同期読み込みデータのパラメータに構造体パラメータを追加
extern	void		AddASyncLoadParamConstVoidP( BYTE *Data, int *Addr, const void *Param ) ;		// 非同期読み込みデータのパラメータに void 型ポインタのパラメータを追加
extern	void		AddASyncLoadParamDWORD_PTR( BYTE *Data, int *Addr, DWORD_PTR Param ) ;			// 非同期読み込みデータのパラメータに DWORD_PTR 型のパラメータを追加
extern	void		AddASyncLoadParamLONGLONG( BYTE *Data, int *Addr, LONGLONG Param ) ;			// 非同期読み込みデータのパラメータに LONGLONG 型のパラメータを追加
extern	void		AddASyncLoadParamInt( BYTE *Data, int *Addr, int Param ) ;						// 非同期読み込みデータのパラメータに int 型のパラメータを追加
extern	void		AddASyncLoadParamByte( BYTE *Data, int *Addr, BYTE Param ) ;					// 非同期読み込みデータのパラメータに BYTE 型のパラメータを追加
extern	void		AddASyncLoadParamFloat( BYTE *Data, int *Addr, float Param ) ;					// 非同期読み込みデータのパラメータに float 型のパラメータを追加
extern	void		AddASyncLoadParamString( BYTE *Data, int *Addr, const wchar_t *Param ) ;		// 非同期読み込みデータのパラメータに文字列パラメータを追加
extern	void *		GetASyncLoadParamStruct( BYTE *Data, int *Addr ) ;								// 非同期読み込みデータのパラメータから構造体パラメータを取得
extern	void *		GetASyncLoadParamVoidP( BYTE *Data, int *Addr ) ;								// 非同期読み込みデータのパラメータから void 型ポインタのパラメータを取得
extern	DWORD_PTR	GetASyncLoadParamDWORD_PTR( BYTE *Data, int *Addr ) ;							// 非同期読み込みデータのパラメータから DWORD_PTR 型のパラメータを取得
extern	LONGLONG	GetASyncLoadParamLONGLONG( BYTE *Data, int *Addr ) ;							// 非同期読み込みデータのパラメータから LONGLONG 型のパラメータを取得
extern	int			GetASyncLoadParamInt( BYTE *Data, int *Addr ) ;									// 非同期読み込みデータのパラメータから int 型のパラメータを取得
extern	BYTE		GetASyncLoadParamByte( BYTE *Data, int *Addr ) ;								// 非同期読み込みデータのパラメータから BYTE 型のパラメータを取得
extern	float		GetASyncLoadParamFloat( BYTE *Data, int *Addr ) ;								// 非同期読み込みデータのパラメータから float 型のパラメータを取得
extern	wchar_t *	GetASyncLoadParamString( BYTE *Data, int *Addr ) ;								// 非同期読み込みデータのパラメータから文字列パラメータを取得
extern	int			AddASyncLoadData( ASYNCLOADDATA_COMMON *ASyncData ) ;							// 非同期読み込みデータを追加する
extern	int			DeleteASyncLoadData( int DeleteIndex, int MainThread = FALSE ) ;				// 指定の非同期読み込みデータを削除する
extern	int			ProcessASyncLoad( int ThreadNumber ) ;											// 非同期読み込みの処理を行う
extern	int			CheckMainThread( void ) ;														// 現在のスレッドがメインスレッドかどうかを取得する( TRUE:メインスレッド  FALSE:それ以外のスレッド )

extern	int			ResumeASyncLoadThread( int AddMaxThreadNum = 0 ) ;								// 寝ているスレッドを一つ起こす

extern	int			ProcessASyncLoadRequestMainThread( void ) ;										// 非同期読み込みスレッドからメインスレッドへの処理依頼を処理する

extern	int			AddASyncLoadRequestMainThreadInfo( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info ) ;	// メインスレッドで処理してほしいデータの情報を追加する

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_NON_ASYNCLOAD

#endif // __DXASYNCLOAD_H__
