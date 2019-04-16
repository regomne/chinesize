// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		非同期読み込み処理プログラム
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

// インクルード ------------------------------------------------------------------
#include "DxASyncLoad.h"

#ifndef DX_NON_ASYNCLOAD

#include "DxStatic.h"
#include "DxBaseFunc.h"
#include "DxSystem.h"
#include "DxMemory.h"
#include "DxLog.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

// 構造体定義 --------------------------------------------------------------------
	
// 内部大域変数宣言 --------------------------------------------------------------

// 非同期読み込み処理が使用するグローバルデータ
ASYNCLOADDATA GASyncLoadData ;

// 関数プロトタイプ宣言-----------------------------------------------------------

static void ASyncLoadThreadFunction( THREAD_INFO *pThreadInfo, void *ASyncLoadThreadData ) ;

// 非同期読み込みを行うスレッドのマクロ
#define ASYNCLOADTHREADFUNCTION_MACRO( Number )		\
	void ASyncLoadThreadFunctionT##Number( THREAD_INFO *pThreadInfo, void *ASyncLoadThreadData )\
{\
	ASyncLoadThreadFunction( pThreadInfo, ASyncLoadThreadData ) ;\
}

ASYNCLOADTHREADFUNCTION_MACRO( 0 )
ASYNCLOADTHREADFUNCTION_MACRO( 1 )
ASYNCLOADTHREADFUNCTION_MACRO( 2 )
ASYNCLOADTHREADFUNCTION_MACRO( 3 )
ASYNCLOADTHREADFUNCTION_MACRO( 4 )
ASYNCLOADTHREADFUNCTION_MACRO( 5 )
ASYNCLOADTHREADFUNCTION_MACRO( 6 )
ASYNCLOADTHREADFUNCTION_MACRO( 7 )
ASYNCLOADTHREADFUNCTION_MACRO( 8 )
ASYNCLOADTHREADFUNCTION_MACRO( 9 )
ASYNCLOADTHREADFUNCTION_MACRO( 10 )
ASYNCLOADTHREADFUNCTION_MACRO( 11 )
ASYNCLOADTHREADFUNCTION_MACRO( 12 )
ASYNCLOADTHREADFUNCTION_MACRO( 13 )
ASYNCLOADTHREADFUNCTION_MACRO( 14 )
ASYNCLOADTHREADFUNCTION_MACRO( 15 )
ASYNCLOADTHREADFUNCTION_MACRO( 16 )
ASYNCLOADTHREADFUNCTION_MACRO( 17 )
ASYNCLOADTHREADFUNCTION_MACRO( 18 )
ASYNCLOADTHREADFUNCTION_MACRO( 19 )
ASYNCLOADTHREADFUNCTION_MACRO( 20 )
ASYNCLOADTHREADFUNCTION_MACRO( 21 )
ASYNCLOADTHREADFUNCTION_MACRO( 22 )
ASYNCLOADTHREADFUNCTION_MACRO( 23 )
ASYNCLOADTHREADFUNCTION_MACRO( 24 )
ASYNCLOADTHREADFUNCTION_MACRO( 25 )
ASYNCLOADTHREADFUNCTION_MACRO( 26 )
ASYNCLOADTHREADFUNCTION_MACRO( 27 )
ASYNCLOADTHREADFUNCTION_MACRO( 28 )
ASYNCLOADTHREADFUNCTION_MACRO( 29 )
ASYNCLOADTHREADFUNCTION_MACRO( 30 )
ASYNCLOADTHREADFUNCTION_MACRO( 31 )

void ( *ASyncLoadThreadFunctionList[ ASYNCLOADTHREAD_MAXNUM ] )( THREAD_INFO *pThreadInfo, void *ASyncLoadThreadData ) =
{
	ASyncLoadThreadFunctionT0,
	ASyncLoadThreadFunctionT1,
	ASyncLoadThreadFunctionT2,
	ASyncLoadThreadFunctionT3,
	ASyncLoadThreadFunctionT4,
	ASyncLoadThreadFunctionT5,
	ASyncLoadThreadFunctionT6,
	ASyncLoadThreadFunctionT7,
	ASyncLoadThreadFunctionT8,
	ASyncLoadThreadFunctionT9,
	ASyncLoadThreadFunctionT10,
	ASyncLoadThreadFunctionT11,
	ASyncLoadThreadFunctionT12,
	ASyncLoadThreadFunctionT13,
	ASyncLoadThreadFunctionT14,
	ASyncLoadThreadFunctionT15,
	ASyncLoadThreadFunctionT16,
	ASyncLoadThreadFunctionT17,
	ASyncLoadThreadFunctionT18,
	ASyncLoadThreadFunctionT19,
	ASyncLoadThreadFunctionT20,
	ASyncLoadThreadFunctionT21,
	ASyncLoadThreadFunctionT22,
	ASyncLoadThreadFunctionT23,
	ASyncLoadThreadFunctionT24,
	ASyncLoadThreadFunctionT25,
	ASyncLoadThreadFunctionT26,
	ASyncLoadThreadFunctionT27,
	ASyncLoadThreadFunctionT28,
	ASyncLoadThreadFunctionT29,
	ASyncLoadThreadFunctionT30,
	ASyncLoadThreadFunctionT31,
} ;

// プログラム --------------------------------------------------------------------

// 非同期読み込み処理の初期化と後始末

// 非同期読み込み処理の初期化
extern int InitializeASyncLoad( DWORD_PTR MainThreadID )
{
	// 既に初期化済みの場合は何もしない
	if( GASyncLoadData.InitializeFlag == TRUE )
		return -1 ;

	DXST_LOGFILE_ADDUTF16LE( "\x5e\x97\x0c\x54\x1f\x67\xad\x8a\x7f\x30\xbc\x8f\x7f\x30\xe6\x51\x06\x74\x6e\x30\x1d\x52\x1f\x67\x16\x53\x2e\x00\x2e\x00\x2e\x00\x00"/*@ L"非同期読み込み処理の初期化..." @*/ ) ;  

	// メインスレッドのＩＤを保存する
	GASyncLoadData.MainThreadID = MainThreadID ;

	// 非同期読み込み処理用のクリティカルセクションの初期化
	CriticalSection_Initialize( &GASyncLoadData.CriticalSection ) ;

	DXST_LOGFILE_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"成功\n" @*/ ) ;

	// 初期化完了フラグを立てる
	GASyncLoadData.InitializeFlag = TRUE ;

	// 正常終了
	return 0 ;
}

// 非同期読み込み処理を行うスレッドを立てる
extern int SetupASyncLoadThread( int ProcessorNum )
{
	int i ;
	ASYNCLOADTHREADINFO *AInfo ;

#ifdef DX_ANDROID_DEBUG
	GASyncLoadData.ThreadNum = 2 ;
#else
	GASyncLoadData.ThreadNum = 32 ;
#endif

	// 初期化前に値が設定されていたら NS_SetASyncLoadThreadNum を呼び出す
	if( GASyncLoadData.ThreadMaxResumeNum != 0 )
	{
		NS_SetASyncLoadThreadNum( GASyncLoadData.ThreadMaxResumeNum ) ;
	}
	else
	{
		GASyncLoadData.ThreadMaxResumeNum = ProcessorNum - 1 ;
		if( GASyncLoadData.ThreadMaxResumeNum <= 0 )
		{
			GASyncLoadData.ThreadMaxResumeNum = 1 ;
		}
	}

	_MEMSET( GASyncLoadData.Thread, 0, sizeof( GASyncLoadData.Thread ) ) ;
	AInfo = GASyncLoadData.Thread ;
	for( i = 0 ; i < GASyncLoadData.ThreadNum ; i ++, AInfo ++ )
	{
		if( Thread_Create( &AInfo->ThreadInfo, ASyncLoadThreadFunctionList[ i ], NULL ) == -1 )
			return -1 ;
		Thread_SetPriority( &AInfo->ThreadInfo, DX_THREAD_PRIORITY_LOWEST ) ;
		AInfo->SuspendFlag = TRUE ;
		AInfo->SuspendStartTime = NS_GetNowCount() ;
	}

	// 少し寝て、全部のスレッドが寝るのを待つ
	Thread_Sleep( 16 ) ;

	// 正常終了
	return 0 ;
}

// 非同期読み込み処理のスレッドを閉じる
extern int CloseASyncLoadThread( void )
{
	int i ;
	ASYNCLOADTHREADINFO *AInfo ;

	GASyncLoadData.ThreadEndRequestFlag = TRUE ;

	AInfo = GASyncLoadData.Thread ;
	for( i = 0 ; i < GASyncLoadData.ThreadNum ; i ++, AInfo ++ )
	{
		if( Thread_IsValid( &AInfo->ThreadInfo ) != 0 )
		{
			// スレッドが終了するまで待つ
			while( AInfo->ExitFlag == 0 )
			{
				ProcessASyncLoadRequestMainThread() ;

				if( GASyncLoadData.ThreadResumeNum == 0 )
				{
					ResumeASyncLoadThread() ;
				}

				Thread_Sleep( 1 ) ;
			}

			// スレッドのハンドルを閉じる
			Thread_Delete( &AInfo->ThreadInfo ) ;
		}
	}

	GASyncLoadData.ThreadEndRequestFlag = FALSE ;

	// 正常終了
	return 0 ;
}

// 非同期読み込み処理の後始末
extern int TerminateASyncLoad( void )
{
	// 既に後始末処理済みの場合は何もしない
	if( GASyncLoadData.InitializeFlag == FALSE )
		return -1 ;

	// 非同期読み込み処理用のクリティカルセクションの削除
	CriticalSection_Delete( &GASyncLoadData.CriticalSection ) ;

	// 初期化完了フラグを倒す
	GASyncLoadData.InitializeFlag = FALSE ;

	// 正常終了
	return 0 ;
}














// 非同期読み込み関係

// 非同期読み込みデータを格納するメモリ領域を確保する
extern ASYNCLOADDATA_COMMON *AllocASyncLoadDataMemory( int AddAllocSize )
{
	ASYNCLOADDATA_COMMON *ASyncData ;

	// メモリの確保
	ASyncData = ( ASYNCLOADDATA_COMMON * )DXALLOC( sizeof( ASYNCLOADDATA_COMMON ) + AddAllocSize ) ;
	if( ASyncData == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x5e\x97\x0c\x54\x1f\x67\xad\x8a\x7f\x30\xbc\x8f\x7f\x30\xc7\x30\xfc\x30\xbf\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x20\x00\x25\x00\x64\x00\x42\x00\x79\x00\x74\x00\x65\x00\x20\x00\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"非同期読み込みデータを格納するメモリ領域 %dByte の確保に失敗しました" @*/, AddAllocSize + sizeof( ASYNCLOADDATA_COMMON ) )) ;
		return NULL ;
	}

	// アドレスを返す
	return ASyncData ;
}

// 非同期読み込みデータのパラメータに構造体パラメータを追加
extern	void	AddASyncLoadParamStruct( BYTE *Data, int *Addr, const void *Param, int Size )
{
	if( Data )
	{
		*( ( WORD * )( Data + *Addr ) ) = ( WORD )( Size + sizeof( WORD ) ) ;
		_MEMCPY( Data + *Addr + sizeof( WORD ), Param, ( size_t )Size ) ;
	}
	*Addr += Size + sizeof( WORD ) ;
}

// 非同期読み込みデータのパラメータに void 型ポインタのパラメータを追加
extern	void	AddASyncLoadParamConstVoidP( BYTE *Data, int *Addr, const void *Param )
{
	if( Data )
	{
		*( ( const void ** )( Data + *Addr ) ) = Param ;
	}
	*Addr += sizeof( const void * ) ;
}

// 非同期読み込みデータのパラメータに DWORD_PTR 型のパラメータを追加
extern	void	AddASyncLoadParamDWORD_PTR( BYTE *Data, int *Addr, DWORD_PTR Param )
{
	if( Data )
	{
		*( ( DWORD_PTR * )( Data + *Addr ) ) = Param ;
	}
	*Addr += sizeof( DWORD_PTR ) ;
}

// 非同期読み込みデータのパラメータに LONGLONG 型のパラメータを追加
extern void AddASyncLoadParamLONGLONG( BYTE *Data, int *Addr, LONGLONG Param )
{
	if( Data )
	{
		*( ( LONGLONG * )( Data + *Addr ) ) = Param ;
	}
	*Addr += sizeof( LONGLONG ) ;
}

// 非同期読み込みデータのパラメータに int 型のパラメータを追加
extern	void	AddASyncLoadParamInt( BYTE *Data, int *Addr, int Param )
{
	if( Data )
	{
		*( ( int * )( Data + *Addr ) ) = Param ;
	}
	*Addr += sizeof( int ) ;
}

// 非同期読み込みデータのパラメータに BYTE 型のパラメータを追加
extern	void	AddASyncLoadParamByte( BYTE *Data, int *Addr, BYTE Param )
{
	if( Data )
	{
		*( ( BYTE * )( Data + *Addr ) ) = Param ;
	}
	*Addr += sizeof( BYTE ) ;
}

// 非同期読み込みデータのパラメータに float 型のパラメータを追加
extern	void	AddASyncLoadParamFloat( BYTE *Data, int *Addr, float Param )
{
	if( Data )
	{
		*( ( float * )( Data + *Addr ) ) = Param ;
	}
	*Addr += sizeof( float ) ;
}

// 非同期読み込みデータのパラメータに文字列パラメータを追加
extern	void	AddASyncLoadParamString( BYTE *Data, int *Addr, const wchar_t *Param )
{
	DWORD StrLen ;

	if( Param == NULL )
	{
		StrLen = 0 ;
	}
	else
	{
		StrLen = ( DWORD )( ( _WCSLEN( Param ) + 1 ) * sizeof( wchar_t ) ) ;
	}

	if( Data )
	{
		*( ( WORD * )( Data + *Addr ) ) = ( WORD )( StrLen + sizeof( WORD ) ) ;
		if( StrLen > 0 )
		{
			_MEMCPY( ( Data + *Addr ) + sizeof( WORD ), Param, StrLen ) ;
		}
	}
	*Addr += sizeof( WORD ) + StrLen ;
}

// 非同期読み込みデータのパラメータから構造体パラメータを取得
extern	void *	GetASyncLoadParamStruct( BYTE *Data, int *Addr )
{
	void *Ret ;

	Ret = Data + *Addr + sizeof( WORD ) ;
	*Addr += *( ( WORD * )( Data + *Addr ) ) ;

	return Ret ;
}

// 非同期読み込みデータのパラメータから void 型ポインタのパラメータを取得
extern	void *	GetASyncLoadParamVoidP( BYTE *Data, int *Addr )
{
	void *Ret ;

	Ret = *( ( void ** )( Data + *Addr ) ) ;
	*Addr += sizeof( void * ) ;

	return Ret ;
}

// 非同期読み込みデータのパラメータから DWORD_PTR 型のパラメータを取得
extern	DWORD_PTR GetASyncLoadParamDWORD_PTR( BYTE *Data, int *Addr )
{
	DWORD_PTR Ret ;

	Ret = *( ( DWORD_PTR * )( Data + *Addr ) ) ;
	*Addr += sizeof( DWORD_PTR ) ;

	return Ret ;
}

// 非同期読み込みデータのパラメータから LONGLONG 型のパラメータを取得
extern LONGLONG GetASyncLoadParamLONGLONG( BYTE *Data, int *Addr )
{
	LONGLONG Ret ;

	Ret = *( ( LONGLONG * )( Data + *Addr ) ) ;
	*Addr += sizeof( LONGLONG ) ;

	return Ret ;
}

// 非同期読み込みデータのパラメータから int 型のパラメータを取得
extern	int		GetASyncLoadParamInt( BYTE *Data, int *Addr )
{
	int Ret ;

	Ret = *( ( int * )( Data + *Addr ) ) ;
	*Addr += sizeof( int ) ;

	return Ret ;
}

// 非同期読み込みデータのパラメータから BYTE 型のパラメータを取得
extern	BYTE	GetASyncLoadParamByte( BYTE *Data, int *Addr )
{
	BYTE Ret ;

	Ret = *( ( BYTE * )( Data + *Addr ) ) ;
	*Addr += sizeof( BYTE ) ;

	return Ret ;
}

// 非同期読み込みデータのパラメータから float 型のパラメータを取得
extern	float	GetASyncLoadParamFloat( BYTE *Data, int *Addr )
{
	float Ret ;

	Ret = *( ( float * )( Data + *Addr ) ) ;
	*Addr += sizeof( float ) ;

	return Ret ;
}

// 非同期読み込みデータのパラメータから文字列パラメータを取得
extern	wchar_t *	GetASyncLoadParamString( BYTE *Data, int *Addr )
{
	wchar_t *Ret ;
	DWORD StrLen ;

	Ret = ( wchar_t * )( ( Data + *Addr ) + sizeof( WORD ) ) ;
	StrLen = *( ( WORD * )( Data + *Addr ) ) - sizeof( WORD ) ;
	*Addr += *( ( WORD * )( Data + *Addr ) ) ;

	return StrLen == 0 ? NULL : Ret ;
}

// 非同期読み込みデータを追加する
extern int AddASyncLoadData( ASYNCLOADDATA_COMMON *ASyncData )
{
	int NewIndex ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &GASyncLoadData.CriticalSection ) ;

	// 既に非同期読み込みデータが最大数に達している場合はエラー
	if( GASyncLoadData.DataNum == ASYNCLOADDATA_MAXNUM )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &GASyncLoadData.CriticalSection ) ;

		return DXST_LOGFILE_ADDUTF16LE( "\x5e\x97\x0c\x54\x1f\x67\xad\x8a\x7f\x30\xbc\x8f\x7f\x30\xc7\x30\xfc\x30\xbf\x30\x6e\x30\x70\x65\x4c\x30\x31\x8a\xb9\x5b\xcf\x91\x92\x30\x8a\x8d\x48\x30\x7e\x30\x57\x30\x5f\x30\x42\x7d\x86\x4e\x57\x30\x7e\x30\x59\x30\x00"/*@ L"非同期読み込みデータの数が許容量を越えました終了します" @*/ ) ;
	}

	// 使用されていない配列要素を探す
	if( GASyncLoadData.DataArea != ASYNCLOADDATA_MAXNUM )
	{
		NewIndex = GASyncLoadData.DataArea ;
	}
	else
	{
		for( NewIndex = 0 ; NewIndex < ASYNCLOADDATA_MAXNUM && GASyncLoadData.Data[ NewIndex ] != NULL ; NewIndex ++ ){}
	}

	// 初期化内容が確定しているメンバ変数のみ初期化
	ASyncData->Index = NewIndex ;
	ASyncData->Run = FALSE ;

	// 登録時間を保存
	ASyncData->StartTime = NS_GetNowCount() ;

	// 使用されているポインタが存在する範囲を更新する
	if( GASyncLoadData.DataArea == NewIndex ) GASyncLoadData.DataArea ++ ;

	// データポインタの保存
	GASyncLoadData.Data[ NewIndex ] = ASyncData ;

	// 非同期読み込みデータの数を増やす
	GASyncLoadData.DataNum ++ ;

	// 読み込みスレッドを一つ稼動状態にする
	ResumeASyncLoadThread() ;

	// クリティカルセクションの解放
	CriticalSection_Unlock( &GASyncLoadData.CriticalSection ) ;

	// 正常終了
	return 0 ;
}


// 指定の非同期読み込みデータを削除する
extern int DeleteASyncLoadData( int DeleteIndex, int MainThread )
{
	ASYNCLOADDATA_COMMON *ASyncData ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &GASyncLoadData.CriticalSection ) ;

	// データのアドレスを取得
	ASyncData = GASyncLoadData.Data[ DeleteIndex ] ;

	// 指定のインデックスが既に解放されていたら何もしない
	if( ASyncData == NULL )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &GASyncLoadData.CriticalSection ) ;
		return -1 ;
	}

	// 既に処理が走っていたらエラー
	if( MainThread && ASyncData->Run )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &GASyncLoadData.CriticalSection ) ;
		return -2 ;
	}

	// 非同期読み込みデータの総数を減らす
	GASyncLoadData.DataNum -- ;

	// 有効な非同期読み込みデータが存在する範囲の更新
	if( DeleteIndex == GASyncLoadData.DataArea - 1 )
	{
		if( GASyncLoadData.DataNum == 0 ) GASyncLoadData.DataArea = 0 ;
		else
		{
			int DataArea = GASyncLoadData.DataArea - 1 ;
			while( GASyncLoadData.Data[ DataArea - 1 ] == NULL ) DataArea -- ; 
			GASyncLoadData.DataArea = DataArea ;
		}
	}

	// データ領域を解放する
	DXFREE( ASyncData ) ;

	// テーブルに NULL をセットする
	GASyncLoadData.Data[ DeleteIndex ] = NULL ;

	// クリティカルセクションの解放
	CriticalSection_Unlock( &GASyncLoadData.CriticalSection ) ;

	// 正常終了
	return 0 ;
}


// 非同期読み込みの処理を行う
extern int ProcessASyncLoad( int ThreadNumber )
{
	int i ;
	ASYNCLOADDATA_COMMON *Data ;
	ASYNCLOADTHREADINFO *AInfo ;

	AInfo = &GASyncLoadData.Thread[ ThreadNumber ] ;

	// データが無かったら何もしない
	if( GASyncLoadData.DataArea == 0 )
		return -2 ;

	// 処理されていないデータを探す

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &GASyncLoadData.CriticalSection ) ;

	// データの範囲のみループ
	Data = NULL ;
	for( i = 0 ; i < GASyncLoadData.DataArea ; i ++ )
	{
		// データが無かったら何もしない
		if( GASyncLoadData.Data[ i ] == NULL ) continue ;

		Data = GASyncLoadData.Data[ i ] ;

		// 既にどれかのスレッドが処理している場合は何もしない
		if( Data->Run )
		{
			Data = NULL ;
			continue ;
		}

		break ;
	}

	// 処理を実行中フラグを立てる
	if( Data )
	{
		Data->Run = TRUE ;
	}

	// 仕事中フラグを立てる
	AInfo->JobFlag = TRUE ;

	// クリティカルセクションの解放
	CriticalSection_Unlock( &GASyncLoadData.CriticalSection ) ;

	if( Data )
	{
		// 読み込み処理
		Data->ProcessFunction( Data ) ;

		// 読み込み処理が終了したら削除する
		DeleteASyncLoadData( i ) ;
	}

	// 正常終了
	return 0 ;
}

// 現在のスレッドがメインスレッドかどうかを取得する( TRUE:メインスレッド  FALSE:それ以外のスレッド )
extern int CheckMainThread( void )
{
	return Thread_GetCurrentId() == GASyncLoadData.MainThreadID ? TRUE : FALSE ;
}

// 非同期読み込みを行うスレッド
void ASyncLoadThreadFunction( THREAD_INFO *pThreadInfo, void * /*ASyncLoadThreadData*/ )
{
	int i ;
	int Index = 0 ;
//	int Ret ;
	int ThreadNumber ;
	ASYNCLOADDATA_COMMON *Data ;
	ASYNCLOADTHREADINFO *AInfo ;

	// スレッドの番号を取得しておく
	AInfo = GASyncLoadData.Thread ;
	for( ThreadNumber = 0 ; ThreadNumber < ASYNCLOADTHREAD_MAXNUM ; ThreadNumber ++, AInfo ++ )
	{
		if( &AInfo->ThreadInfo == pThreadInfo )
			break ;
	}

	for(;;)
	{
		// スレッドを終了すべきというフラグが立ったらループから外れる
		if( GASyncLoadData.ThreadEndRequestFlag == TRUE ) goto ENDLABEL ;

		// データが無かったら何もしない
		if( GASyncLoadData.DataArea == 0 )
		{
SLEEP_LABEL :
			// クリティカルセクションの取得
			CRITICALSECTION_LOCK( &GASyncLoadData.CriticalSection ) ;

			// スレッドを寝かす準備
			AInfo->SuspendFlag = TRUE ;
			AInfo->SuspendStartTime = NS_GetNowCount() ;
			GASyncLoadData.ThreadResumeNum -- ;

			// クリティカルセクションの解放
			CriticalSection_Unlock( &GASyncLoadData.CriticalSection ) ;

			// 休止状態にする
			Thread_Suspend( pThreadInfo ) ;

			// 最初に戻る
			continue ;
		}

		// 処理されていない一番古いデータを探す

		// クリティカルセクションの取得
		CRITICALSECTION_LOCK( &GASyncLoadData.CriticalSection ) ;

		// データの範囲のみループ
		Data = NULL ;
		for( i = 0 ; i < GASyncLoadData.DataArea ; i ++ )
		{
			if( GASyncLoadData.Data[ i ] == NULL || GASyncLoadData.Data[ i ]->Run )
				continue ;

			if( Data != NULL && Data->StartTime < GASyncLoadData.Data[ i ]->StartTime )
				continue ;

			Data = GASyncLoadData.Data[ i ] ;
			Index = i ;
		}
		if( Data == NULL )
		{
			// クリティカルセクションの解放
			CriticalSection_Unlock( &GASyncLoadData.CriticalSection ) ;

			// 誰か居たらスレッドを起こす
			ResumeASyncLoadThread( 1 ) ;

			// 寝る
			goto SLEEP_LABEL ;
		}

		// データを処理中フラグを立てる
		Data->Run = TRUE ;

		// 仕事中フラグを立てる
		AInfo->JobFlag = TRUE ;

		// 担当のデータのアドレスを保存
		AInfo->Data = Data ;

		// クリティカルセクションの解放
		CriticalSection_Unlock( &GASyncLoadData.CriticalSection ) ;

		// 読み込み処理
		Data->ProcessFunction( Data ) ;

		// クリティカルセクションの取得
		CRITICALSECTION_LOCK( &GASyncLoadData.CriticalSection ) ;

		// 読み込み処理が終了したら削除する
		DeleteASyncLoadData( Index ) ;

		// 仕事中フラグを倒す
		AInfo->JobFlag = FALSE ;

		// 担当のデータのアドレスを初期化
		AInfo->Data = NULL ;

		// クリティカルセクションの解放
		CriticalSection_Unlock( &GASyncLoadData.CriticalSection ) ;
	}

ENDLABEL :
	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &GASyncLoadData.CriticalSection ) ;

	// スレッドが終了したことを示すフラグを立てる
	AInfo->ExitFlag = TRUE ;
	GASyncLoadData.ThreadResumeNum -- ;

	// クリティカルセクションの解放
	CriticalSection_Unlock( &GASyncLoadData.CriticalSection ) ;
}

// 寝ているスレッドを一つ起こす
extern int ResumeASyncLoadThread( int AddMaxThreadNum )
{
	int i ;
	int SelectNo ;
	int LastSelectNo ;
	int LastSelectNoCount ;
	ASYNCLOADTHREADINFO *AInfo ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &GASyncLoadData.CriticalSection ) ;

	// 起きているスレッドが一定数以上だったら起こさない
	if( GASyncLoadData.ThreadResumeNum >= GASyncLoadData.ThreadMaxResumeNum + AddMaxThreadNum )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &GASyncLoadData.CriticalSection ) ;

		return 0 ;
	}

	LastSelectNo = -1 ;
	LastSelectNoCount = 0 ;
	for(;;)
	{
		// 仕事をしていてメインスレッド処理待ちではない一番古いデータを担当しているスレッドを検索する
		SelectNo = -1 ;
		AInfo = GASyncLoadData.Thread ;
		for( i = 0 ; i < GASyncLoadData.ThreadNum ; i ++, AInfo ++ )
		{
			if( AInfo->ExitFlag == TRUE ||
				AInfo->SuspendFlag == FALSE ||
				AInfo->JobFlag == FALSE ||
				AInfo->MainThreadRequest == TRUE )
				continue ;

			if( SelectNo == -1 ||
				AInfo->Data->StartTime < GASyncLoadData.Thread[ SelectNo ].Data->StartTime )
			{
				SelectNo = i ;
			}
		}

		// 仕事をしているメインスレッド処理待ちではないスレッドが無かったら一番寝ている時間の長いスレッドを検索する
		if( SelectNo == -1 )
		{
			SelectNo = -1 ;
			AInfo = GASyncLoadData.Thread ;
			for( i = 0 ; i < GASyncLoadData.ThreadNum ; i ++, AInfo ++ )
			{
				if( AInfo->ExitFlag == TRUE ||
					AInfo->SuspendFlag == FALSE ||
					AInfo->MainThreadRequest == TRUE )
					continue ;

				if( SelectNo == -1 ||
					AInfo->SuspendStartTime < GASyncLoadData.Thread[ SelectNo ].SuspendStartTime )
				{
					SelectNo = i ;
				}
			}
		}

		// それも無かったらスレッドを起こすのを断念する
		if( SelectNo == -1 )
			break ;

		// 起こす
		AInfo = &GASyncLoadData.Thread[ SelectNo ] ;
		if( Thread_Resume( &AInfo->ThreadInfo ) == 0 )
		{
			// 既に起きていたらやり直す

			// 同じスレッドに対して３回連続でここに来ていたらスレッドを起こすのを断念する
			if( LastSelectNo != SelectNo )
			{
				LastSelectNo = SelectNo ;
				LastSelectNoCount = 1 ;
			}
			else
			{
				LastSelectNoCount ++ ;
				if( LastSelectNoCount == 3 )
				{
					break ;
				}
			}

			continue ;
		}

		// 寝ているフラグを倒す
		AInfo->SuspendFlag = FALSE ;
		GASyncLoadData.ThreadResumeNum ++ ;

		// ループから抜ける
		break ;
	}

	// クリティカルセクションの解放
	CriticalSection_Unlock( &GASyncLoadData.CriticalSection ) ;

	// 終了
	return 0 ;
}

// 非同期読み込みスレッドからメインスレッドへの処理依頼を処理する
extern int ProcessASyncLoadRequestMainThread( void )
{
	int i ;
	ASYNCLOAD_MAINTHREAD_REQUESTINFO *Data ;
	int StartTime ;
	int RunNum ;

	// 初期化されていなかったら何もせず終了
	if( GASyncLoadData.InitializeFlag == FALSE )
	{
		return -1 ;
	}

	// メインスレッド以外から呼ばれたら何もしない
	if( Thread_GetCurrentId() != GASyncLoadData.MainThreadID )
	{
		return -1 ;
	}

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &GASyncLoadData.CriticalSection ) ;

	// 依頼がある場合のみ依頼を処理
	if( GASyncLoadData.MainThreadRequestInfoNum != 0 )
	{
		// 依頼の数だけ繰り返し
		StartTime = NS_GetNowCount() ;
		for( i = 0 ; i < GASyncLoadData.MainThreadRequestInfoNum ; i ++ )
		{
			if( NS_GetNowCount() - StartTime > 2 )
				break ;

			Data = GASyncLoadData.MainThreadRequestInfo[ i ] ;
			Data->Result = Data->Function( Data ) ;
			GASyncLoadData.Thread[ Data->ThreadNo ].MainThreadRequest = FALSE ;

			// スレッドを再開する
			ResumeASyncLoadThread() ;
		}

		// 残ったら次回に持ち越し
		if( i != GASyncLoadData.MainThreadRequestInfoNum )
		{
			int Num ;

			Num = GASyncLoadData.MainThreadRequestInfoNum - i ;
			_MEMMOVE( &GASyncLoadData.MainThreadRequestInfo[ 0 ], &GASyncLoadData.MainThreadRequestInfo[ i ], sizeof( ASYNCLOAD_MAINTHREAD_REQUESTINFO * ) * Num ) ;
			GASyncLoadData.MainThreadRequestInfoNum = Num ;
		}
		else
		{
			// 依頼の数をリセット
			GASyncLoadData.MainThreadRequestInfoNum = 0 ;
		}
	}

	// データはあるのに動いているスレッドが少ない場合はスレッドを動かす
	RunNum = GASyncLoadData.ThreadMaxResumeNum - 1 ;
	if( RunNum == 0 )
	{
		RunNum = 1 ;
	}
	if( RunNum > GASyncLoadData.DataNum )
	{
		RunNum = GASyncLoadData.DataNum ;
	}
	if( GASyncLoadData.DataNum != 0 && RunNum > GASyncLoadData.ThreadResumeNum )
	{
		for( i = GASyncLoadData.ThreadResumeNum ; i < RunNum ; i ++ )
		{
			ResumeASyncLoadThread() ;
		}
	}

	// クリティカルセクションの解放
	CriticalSection_Unlock( &GASyncLoadData.CriticalSection ) ;

	// 正常終了
	return 0 ;
}

// メインスレッドで処理してほしいデータの情報を追加する
extern int AddASyncLoadRequestMainThreadInfo( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	int i ;
	DWORD_PTR CurrentThreadId ;
	ASYNCLOADTHREADINFO *AInfo ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &GASyncLoadData.CriticalSection ) ;

	// 既にメインスレッドへの処理依頼数が最大数に達している場合はエラー
	if( GASyncLoadData.MainThreadRequestInfoNum == ASYNCLOADDATA_MAXNUM )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x5e\x97\x0c\x54\x1f\x67\xad\x8a\x7f\x30\xbc\x8f\x7f\x30\xb9\x30\xec\x30\xc3\x30\xc9\x30\x4b\x30\x89\x30\xe1\x30\xa4\x30\xf3\x30\xb9\x30\xec\x30\xc3\x30\xc9\x30\x78\x30\x6e\x30\xe6\x51\x06\x74\x9d\x4f\x3c\x98\x70\x65\x4c\x30\x31\x8a\xb9\x5b\xcf\x91\x92\x30\x8a\x8d\x48\x30\x7e\x30\x57\x30\x5f\x30\x42\x7d\x86\x4e\x57\x30\x7e\x30\x59\x30\x00"/*@ L"非同期読み込みスレッドからメインスレッドへの処理依頼数が許容量を越えました終了します" @*/ ) ;
		goto ERR ;
	}

	// 情報を追加
	GASyncLoadData.MainThreadRequestInfo[ GASyncLoadData.MainThreadRequestInfoNum ] = Info ;
	Info->Result = 0 ;

	// スレッド番号検出
	CurrentThreadId = Thread_GetCurrentId() ;
	AInfo = GASyncLoadData.Thread ;
	for( i = 0 ; i < ASYNCLOADTHREAD_MAXNUM && Thread_GetId( &AInfo->ThreadInfo ) != CurrentThreadId ; i ++, AInfo ++ ){}
	if( i == ASYNCLOADTHREAD_MAXNUM )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x5e\x97\x0c\x54\x1f\x67\xad\x8a\x7f\x30\xbc\x8f\x7f\x30\xb9\x30\xec\x30\xc3\x30\xc9\x30\xe5\x4e\x16\x59\x4b\x30\x89\x30\xe1\x30\xa4\x30\xf3\x30\xb9\x30\xec\x30\xc3\x30\xc9\x30\x78\x30\x6e\x30\xe6\x51\x06\x74\x9d\x4f\x3c\x98\x4c\x30\x4c\x88\x8f\x30\x8c\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"非同期読み込みスレッド以外からメインスレッドへの処理依頼が行われました" @*/ ) ;
		goto ERR ;
	}

	// イベントを取得
//	Info->EndEvent = GASyncLoadData.ThreadEvent[ i ] ;

	// スレッド番号を保存
	Info->ThreadNo = i ;

	// メインスレッドへのリクエストを行っているフラグを立てる
	AInfo->MainThreadRequest = TRUE ;

	// データの数をインクリメント
	GASyncLoadData.MainThreadRequestInfoNum ++ ;

	// 寝る代わりにスレッドを一つ起こす
	ResumeASyncLoadThread( 1 ) ;

	// スレッドを寝かす準備
	AInfo->SuspendFlag = TRUE ;
	AInfo->SuspendStartTime = NS_GetNowCount() ;
	GASyncLoadData.ThreadResumeNum -- ;

	// クリティカルセクションの解放
	CriticalSection_Unlock( &GASyncLoadData.CriticalSection ) ;

	// スレッドを止める
	Thread_Suspend( &AInfo->ThreadInfo ) ;

	// 処理終了待ち
//	WinAPIData.Win32Func.WaitForSingleObjectFunc( Info->EndEvent, INFINITE ) ;

	// 正常終了
	return Info->Result ;

ERR :
	// クリティカルセクションの解放
	CriticalSection_Unlock( &GASyncLoadData.CriticalSection ) ;

	// エラー終了
	return -1 ;
}





// 非同期読み込み中の処理の数を取得する
extern int NS_GetASyncLoadNum( void )
{
	int Result ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &GASyncLoadData.CriticalSection ) ;

	Result = GASyncLoadData.DataNum ;

	// クリティカルセクションの解放
	CriticalSection_Unlock( &GASyncLoadData.CriticalSection ) ;

	return Result ;
}

// 非同期読み込み処理を行うスレッドの数を設定する
extern int NS_SetASyncLoadThreadNum( int ThreadNum )
{
	// 未初期化の場合は値だけを保存する
	if( GASyncLoadData.InitializeFlag == FALSE )
	{
		GASyncLoadData.ThreadMaxResumeNum = ThreadNum ;
	}
	else
	{
		if( ThreadNum <= 0 )
		{
			ThreadNum = 1 ;
		}
		if( ThreadNum > GASyncLoadData.ThreadNum )
		{
			ThreadNum = GASyncLoadData.ThreadNum ;
		}

		// クリティカルセクションの取得
		CRITICALSECTION_LOCK( &GASyncLoadData.CriticalSection ) ;

		GASyncLoadData.ThreadMaxResumeNum = ThreadNum ;

		// クリティカルセクションの解放
		CriticalSection_Unlock( &GASyncLoadData.CriticalSection ) ;
	}

	return 0 ;
}


#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_NON_ASYNCLOAD
