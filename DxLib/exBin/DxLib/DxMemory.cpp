// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		メモリ関連プログラム
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

// インクルード ------------------------------------------------------------------
#include "DxMemory.h"
#include "DxLib.h"
#include "DxStatic.h"
#include "DxBaseFunc.h"
#include "DxSystem.h"
#include "DxLog.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

#define DXMEMORY_ALIGNED				(16)						// メモリのアライン


#if defined( DX_THREAD_SAFE ) || defined( DX_THREAD_SAFE_NETWORK_ONLY )

#define DX_ALLOC_ENTER_CRITICAL_SECTION\
	if( MemData.InitMemoryAllocCriticalSection == 0 )\
	{\
		/* メモリ確保、解放処理用クリティカルセクションの初期化 */\
		CriticalSection_Initialize( &MemData.MemoryAllocCriticalSection ) ;\
		MemData.InitMemoryAllocCriticalSection = 1 ;\
	}\
\
	/* メモリ確保関係の処理を実行中の場合は待つ */\
	CRITICALSECTION_LOCK( &MemData.MemoryAllocCriticalSection ) ;

#define DX_ALLOC_LEAVE_CRITICAL_SECTION\
	/* この関数を使用中、フラグを倒す */\
	CriticalSection_Unlock( &MemData.MemoryAllocCriticalSection ) ;

#else

#define DX_ALLOC_ENTER_CRITICAL_SECTION
#define DX_ALLOC_LEAVE_CRITICAL_SECTION

#endif

// 構造体定義 --------------------------------------------------------------------

// テーブル-----------------------------------------------------------------------

// 内部大域変数宣言 --------------------------------------------------------------

MEMORYDATA MemData ;

// 関数プロトタイプ宣言-----------------------------------------------------------

static	void		DxPrintAllocSize( void ) ;													// メモリの確保総量を出力する

// プログラム --------------------------------------------------------------------

// メモリの確保総量を出力する
static void DxPrintAllocSize( void )
{
	DXST_LOGFILEFMT_ADDW(( L"\tTotal size:%d(%.3fkb)  Alloc num:%d",MemData.AllocMemorySize, MemData.AllocMemorySize / 1024.0F, MemData.AllocMemoryNum )) ;
}

// メモリ処理の初期化を行う
extern int MemoryInitialize( void )
{
	DX_ALLOC_ENTER_CRITICAL_SECTION

	// 既に初期化されていたら何もしない
	if( MemData.InitializeFlag )
	{
		DX_ALLOC_LEAVE_CRITICAL_SECTION
		return 0 ;
	}

	// 初期化フラグを立てる
	MemData.InitializeFlag = TRUE ;

	// 環境依存処理
	if( MemoryInitialize_PF() < 0 )
	{
		DX_ALLOC_LEAVE_CRITICAL_SECTION
		return 1 ;
	}

	// 小さいメモリ確保用のヒープを初期化
	InitializeHeap(
		&MemData.SmallHeap,
		ALLOCMEMTYPE_TLSF,
		DXMEMORY_ALIGNED,
		TRUE,
		DXMEMORY_SMALLHEAP_UNITSIZE,
		0,
		NormalMemory_AutoAlloc_CreateHeapCallback_PF,
		NormalMemory_AutoAlloc_GetHeapAddressCallback_PF,
		NormalMemory_AutoAlloc_GetHeapSizeCallback_PF,
		NormalMemory_AutoAlloc_DeleteHeapCallback_PF
	) ;

	// 大きいメモリ確保用のヒープを初期化
	InitializeHeap(
		&MemData.BigHeap,
		ALLOCMEMTYPE_TLSF,
		DXMEMORY_ALIGNED,
		TRUE,
		DXMEMORY_BIGHEAP_UNITSIZE,
		0,
		NormalMemory_AutoAlloc_CreateHeapCallback_PF,
		NormalMemory_AutoAlloc_GetHeapAddressCallback_PF,
		NormalMemory_AutoAlloc_GetHeapSizeCallback_PF,
		NormalMemory_AutoAlloc_DeleteHeapCallback_PF
	) ;

	DX_ALLOC_LEAVE_CRITICAL_SECTION

	// 終了
	return 0 ;
}

// メモリ処理の後始末を行う
extern int MemoryTerminate( void )
{
	DX_ALLOC_ENTER_CRITICAL_SECTION

	// 既に後始末されていたら何もしない
	if( MemData.InitializeFlag == FALSE )
	{
		DX_ALLOC_LEAVE_CRITICAL_SECTION
		return 0 ;
	}

	// 初期化フラグを倒す
	MemData.InitializeFlag = FALSE ;

	// 小さいメモリ確保用のヒープの後始末
	TerminateHeap( &MemData.SmallHeap ) ;

	// 大きいメモリ確保用のヒープの後始末
	TerminateHeap( &MemData.BigHeap ) ;

	// 環境依存処理
	MemoryTerminate_PF() ;

	DX_ALLOC_LEAVE_CRITICAL_SECTION

	// 終了
	return 0 ;
}

// メモリ処理の周期的処理
extern int MemoryProcess( void )
{
	int Time ;
	int NowTime ;
	WORD NowSec ;

	DX_ALLOC_ENTER_CRITICAL_SECTION

	Time = NS_GetNowCount() ;
	if( Time < MemData.StartTime )
	{
		NowTime = ( 0x7fffffff - MemData.StartTime ) + Time ;
	}
	else
	{
		NowTime = Time - MemData.StartTime ;
	}
	NowSec = ( WORD )( NowTime / 1000 ) ;

	// ヒープに現在の秒数をセット
	SetHeapTime( &MemData.SmallHeap, NowSec ) ;
	SetHeapTime( &MemData.BigHeap,   NowSec ) ;

	// 環境依存処理
	if( MemoryProcess_PF() < 0 )
	{
		DX_ALLOC_LEAVE_CRITICAL_SECTION
		return -1 ;
	}

	DX_ALLOC_LEAVE_CRITICAL_SECTION

	// 終了
	return 0 ;
}

// メモリの確保状況を描画する
extern int MemoryDrawAllocInfo( int x, int y, int Width, int Height )
{
	ALLOCMEM_SIZE_TYPE TotalSize ;
	ALLOCMEM_SIZE_TYPE SmallSize ;
	ALLOCMEM_SIZE_TYPE BigSize ;
	int SmallHeight ;
	int BigHeight ;

	SmallSize = GetHeapTotalMemorySize( &MemData.SmallHeap ) ;
	BigSize   = GetHeapTotalMemorySize( &MemData.BigHeap ) ;
	TotalSize = SmallSize + BigSize ;

	SmallHeight = ( int )( SmallSize * ( ALLOCMEM_SIZE_TYPE )Height / TotalSize ) ;
	BigHeight   = Height - SmallHeight ;

	HeapDrawAllocInfo( &MemData.SmallHeap, x, y,               Width, SmallHeight ) ;
	HeapDrawAllocInfo( &MemData.BigHeap,   x, y + SmallHeight, Width, BigHeight   ) ;

	return 0 ;
}









// メモリ確保系関数

// DxAlloc の内部処理用関数
extern void *DxAllocBase( size_t AllocSize, size_t Aligned, const char *File, int Line, int NoMemoryDump )
{
	void *ReturnAddress ;

	DX_ALLOC_ENTER_CRITICAL_SECTION

	{
		HEAPINFO *Heap ;

		// メモリの初期化が行われていなかったら初期化を行う
		if( MemData.InitializeFlag == FALSE )
		{
			if( MemoryInitialize() < 0 )
			{
				DX_ALLOC_LEAVE_CRITICAL_SECTION
				return NULL ;
			}
		}

		// メモリの確保
		Heap = AllocSize > DXMEMORY_SMALL_MAX_SIZE ? &MemData.BigHeap : &MemData.SmallHeap ;
		ReturnAddress = AllocMemory( Heap, AllocSize, Aligned, FALSE, File, Line, NoMemoryDump ) ;
		if( ReturnAddress == NULL )
		{
			goto ERR ;
		}

		// 条件が揃っている場合はログを出力する
		if( ( int )MemData.AllocTrapSize < 0 || MemData.AllocTrapSize == AllocSize || MemData.AllocMemoryPrintFlag == TRUE )
		{
			DXST_LOGFILE_ADDW( L"mem alloc  " ) ;
			PrintInfoMemory( ReturnAddress ) ;
		}
	}

	// 確保したメモリの総量と総数を加算する
	MemData.AllocMemorySize += GetAllocSize( ReturnAddress ) ;
	MemData.AllocMemoryNum ++ ;

	// 確保しているメモリの総量を出力する
	if( MemData.AllocMemorySizeOutFlag == TRUE )
	{
		DxPrintAllocSize() ;
	}

	// メモリ破壊のチェック
	if( MemData.AllocMemoryErrorCheckFlag == TRUE )
	{
		NS_DxErrorCheckAlloc() ;
	}

	DX_ALLOC_LEAVE_CRITICAL_SECTION

	// メモリアドレスを返す
	return ReturnAddress ;

ERR :
	// メモリが足りなかったらその時のメモリをダンプする
	NS_DxDumpAlloc() ;

	// エラー情報も出力する
	NS_DxErrorCheckAlloc() ;

	DX_ALLOC_LEAVE_CRITICAL_SECTION

	return NULL ;
}


// メモリを確保する
extern void *NS_DxAlloc( size_t AllocSize, const char *File, int Line )
{
	return DxAllocBase( AllocSize, 0, File, Line ) ;
}

// メモリを確保して０で初期化する
extern void *NS_DxCalloc( size_t AllocSize, const char *File, int Line )
{
	void *buf ;

	// メモリの確保
	buf = NS_DxAlloc( AllocSize, File, Line ) ;
	if( buf == NULL )
	{
		return NULL ;
	}
	
	// 確保したメモリを初期化
	_MEMSET( buf, 0, AllocSize ) ;

	// 確保したメモリのアドレスを返す
	return buf ;
}

// DxRealloc の内部処理用関数
extern void *DxReallocBase( void *Memory, size_t AllocSize, size_t Aligned, const char *File, int Line )
{
	void *ReturnAddress ;

	if( Memory == NULL )
	{
		ReturnAddress = DxAllocBase( AllocSize, Aligned, File, Line ) ;

		return ReturnAddress ;
	}

	DX_ALLOC_ENTER_CRITICAL_SECTION

	ALLOCMEM_SIZE_TYPE PrevSize = GetAllocSize( Memory ) ;

	ReturnAddress = ReallocMemory( Memory, FALSE, AllocSize, Aligned, File, Line ) ;
	if( ReturnAddress == NULL )
	{
		goto ERR ;
	}

	// 確保したメモリの総量の修正
	MemData.AllocMemorySize -= PrevSize ;
	MemData.AllocMemorySize += GetAllocSize( ReturnAddress ) ;

	// 条件が揃っている場合はログを出力する
	if( (int)MemData.AllocTrapSize < 0 || MemData.AllocTrapSize == AllocSize || MemData.AllocMemoryPrintFlag == TRUE )
	{
		DXST_LOGFILE_ADDW( L"mem realloc  " ) ;
		PrintInfoMemory( Memory ) ;
	}

	// 確保しているメモリの総量を出力する
	if( MemData.AllocMemorySizeOutFlag == TRUE )
	{
		DxPrintAllocSize() ;
	}

	// メモリ破壊のチェック
	if( MemData.AllocMemoryErrorCheckFlag == TRUE )
	{
		NS_DxErrorCheckAlloc() ;
	}

	DX_ALLOC_LEAVE_CRITICAL_SECTION

	// 確保したメモリアドレスを返す
	return ReturnAddress ;

ERR :
	// メモリが足りなかったらその時のメモリをダンプする
	NS_DxDumpAlloc() ;

	DX_ALLOC_LEAVE_CRITICAL_SECTION

	return NULL ;
}

// メモリの再確保を行う
extern void *NS_DxRealloc( void *Memory, size_t AllocSize, const char *File, int Line )
{
	return DxReallocBase( Memory, AllocSize, 0, File, Line ) ;
}

// メモリを解放する
extern void NS_DxFree( void *Memory )
{
	// NULL が渡された場合は何もしない
	if( Memory == NULL )
	{
		return ;
	}

	DX_ALLOC_ENTER_CRITICAL_SECTION

	// メモリ破壊のチェック
	if( MemData.AllocMemoryErrorCheckFlag == TRUE )
	{
		NS_DxErrorCheckAlloc() ;
	}

	ALLOCMEM_SIZE_TYPE AllocSize = GetAllocSize( Memory ) ;

	// 解放するメモリの分だけ確保したメモリの総量と数を減らす
	MemData.AllocMemorySize -= AllocSize ;
	MemData.AllocMemoryNum -- ;

	// 条件が揃っている場合はログを出力する
	if( (int)MemData.AllocTrapSize < 0 || MemData.AllocTrapSize == AllocSize || MemData.AllocMemoryPrintFlag == TRUE )
	{
		DXST_LOGFILE_ADDW( L"mem free  " ) ;
		PrintInfoMemory( Memory ) ;
	}

	// メモリの解放
	FreeMemory( Memory ) ;

	// 確保しているメモリの総量を出力する
	if( MemData.AllocMemorySizeOutFlag == TRUE )
	{
		DxPrintAllocSize() ;
	}

	DX_ALLOC_LEAVE_CRITICAL_SECTION
}

// 列挙対象にするメモリの確保容量をセットする
extern size_t NS_DxSetAllocSizeTrap( size_t Size )
{
	size_t trapsize ;

	trapsize = MemData.AllocTrapSize ;
	MemData.AllocTrapSize = Size ;

	return trapsize ;
}

// ＤＸライブラリ内でメモリ確保が行われる時に情報を出力するかどうかをセットする
extern int NS_DxSetAllocPrintFlag( int Flag )
{
	int printflag ;
	
	printflag = MemData.AllocMemoryPrintFlag ;

	MemData.AllocMemoryPrintFlag = Flag ;

	return printflag ;
}

// 確保しているメモリサイズを取得する
extern size_t NS_DxGetAllocSize( void )
{
	return MemData.AllocMemorySize ;
}

// 確保しているメモリの数を取得する
extern int NS_DxGetAllocNum( void )
{
	return MemData.AllocMemoryNum ;
}

// 確保しているメモリを列挙する
extern void NS_DxDumpAlloc( void )
{
	DX_ALLOC_ENTER_CRITICAL_SECTION

	DXST_LOGFILE_ADDW( L"\n" ) ;
	DXST_LOGFILE_ADDW( L"Alloc memory dump\n" ) ;

	// 小さいメモリ確保の情報を出力
	HeapInfoDump( &MemData.SmallHeap ) ;

	// 大きいメモリ確保の情報を出力
	HeapInfoDump( &MemData.BigHeap ) ;

	DxPrintAllocSize() ;

	// 環境依存のメモリ確保の情報を出力
	DxDumpAlloc_PF() ;

	DXST_LOGFILE_ADDW( L"\n" ) ;

	DX_ALLOC_LEAVE_CRITICAL_SECTION
}

// DxAlloc や DxCalloc で確保しているメモリの状況を描画する
extern void NS_DxDrawAlloc( int x, int y, int Width, int Height )
{
	MemoryDrawAllocInfo( x, y, Width, Height ) ;
}

// 確保したメモリ情報が破壊されていないか調べる( -1:破壊あり  0:なし )
extern int NS_DxErrorCheckAlloc( void )
{
	DX_ALLOC_ENTER_CRITICAL_SECTION

	// 小さいメモリ確保用のヒープのエラーチェック
	if( HeapErrorCheck( &MemData.SmallHeap ) < 0 )
	{
		return -1 ;
	}

	// 大きいメモリ確保用のヒープのエラーチェック
	if( HeapErrorCheck( &MemData.BigHeap ) < 0 )
	{
		return -1 ;
	}

	DX_ALLOC_LEAVE_CRITICAL_SECTION

	// 何事も無く終了
	return 0 ;
}

// メモリが確保、解放されるたびに確保しているメモリの容量を出力するかどうかのフラグをセットする
extern int NS_DxSetAllocSizeOutFlag( int Flag )
{
	int OutFlag ;

	OutFlag = MemData.AllocMemorySizeOutFlag ;
	MemData.AllocMemorySizeOutFlag = Flag ;

	return OutFlag ;
}

// メモリの確保、解放が行われる度に確保しているメモリ確保情報が破損していないか調べるかどうかのフラグをセットする
extern int NS_DxSetAllocMemoryErrorCheckFlag( int Flag )
{
	int CheckFlag ;

	CheckFlag = MemData.AllocMemoryErrorCheckFlag ;
	MemData.AllocMemoryErrorCheckFlag = Flag ;

	return CheckFlag ;
}

// メモリダンプ
extern void	MemoryDump( void *buffer, int size )
{
#ifndef DX_NON_LITERAL_STRING
	BYTE *dp ;
	int i, j, linenum ;
	wchar_t string[128], string2[128], *sp = NULL, *sp2 = NULL ;

	DXST_LOGFILE_ADDW( L"\n" ) ;
	linenum = size / 16 ;
	dp = (BYTE *)buffer ;
	for( i = 0 ; i < linenum ; i ++, dp += 16 )
	{
		DXST_LOGFILEFMT_ADDW(( L"%08x : %02x %02x %02x %02x  %02x %02x %02x %02x  %02x %02x %02x %02x  %02x %02x %02x %02x : %C%C%C%C %C%C%C%C %C%C%C%C %C%C%C%C\n",
			dp, dp[0], dp[1], dp[2], dp[3],  dp[4], dp[5], dp[6], dp[7],  dp[8], dp[9], dp[10], dp[11],  dp[12], dp[13], dp[14], dp[15],
			dp[0], dp[1], dp[2], dp[3],  dp[4], dp[5], dp[6], dp[7],  dp[8], dp[9], dp[10], dp[11],  dp[12], dp[13], dp[14], dp[15] )) ;
	}

	size -= linenum * 16 ;
	j     = 0 ;
	for( i = 0 ; i < size ; i ++, dp ++ )
	{
		if( j == 0 )
		{
			sp  = string  ;
			sp2 = string2 ;
			_SWPRINTF( sp, L"%08x : ", dp ) ;
			sp += 11 ;
		}

		_SWPRINTF( sp, L"%02x ", *dp ) ;
		sp += 3 ;
		sp2[0] = ( wchar_t )*dp ;
		sp2[1] = '\0' ;
		sp2 ++ ;

		j ++ ;
		if( j == 16 )
		{
			_SWPRINTF( sp, L": %s\n", string2 ) ;
			DXST_LOGFILE_ADDW( string ) ;
			j = 0 ;
		}
	}
	if( j != 0 )
	{
		_SWPRINTF( sp, L": %s\n", string2 ) ;
		DXST_LOGFILE_ADDW( string ) ;
	}
#endif
}









#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE
