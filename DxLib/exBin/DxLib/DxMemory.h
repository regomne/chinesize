// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		メモリ関係プログラムヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXMEMORY_H__
#define __DXMEMORY_H__

// インクルード ------------------------------------------------------------------
#include "DxCompileConfig.h"
#include "DxThread.h"
#include "DxHeap.h"

#ifdef __WINDOWS__
#include "Windows/DxMemoryWin.h"
#endif // __WINDOWS__

#ifdef __ANDROID__
#include "Android/DxMemoryAndroid.h"
#endif // __ANDROID__

#ifdef __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE
		#include "iOS/DxMemoryiOS.h"
    #endif // TARGET_OS_IPHONE
#endif // __APPLE__




#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

// メモリ確保系関数
#if !defined( __BCC ) || defined( _DEBUG )
	#define DXALLOCBASE( size, aligned )	DxAllocBase( (size), (aligned), __FILE__, __LINE__ )
	#define DXALLOC( size )					NS_DxAlloc( (size), __FILE__, __LINE__ )
	#define DXCALLOC( size )				NS_DxCalloc( (size), __FILE__, __LINE__ )
	#define DXREALLOC( mem, size )			NS_DxRealloc( (mem), (size), __FILE__, __LINE__ )
	#define DXFREE( mem )					NS_DxFree( (mem) )
#else
	#define DXALLOCBASE( size, aligned )	DxAllocBase( (size), (aligned), "", 0 )
	#define DXALLOC( size )					NS_DxAlloc( (size), "", 0 )
	#define DXCALLOC( size )				NS_DxCalloc( (size), "", 0 )
	#define DXREALLOC( mem, size )			NS_DxRealloc( (mem), (size), "", 0 )
	#define DXFREE( mem )					NS_DxFree( (mem) )
#endif



// 構造体定義 --------------------------------------------------------------------

// メモリ関係の情報構造体
struct MEMORYDATA
{
	size_t					AllocMemorySize ;					// 確保しているメモリの総量
	int						AllocMemoryNum ;					// ライブラリで確保したメモリの数
	int						AllocMemoryPrintFlag ;				// 確保される時に情報を出力するかどうかのフラグ
	int						AllocMemorySizeOutFlag ;			// 確保や解放が行われる度に確保メモリの総量をログに出力するかどうかのフラグ
	int						AllocMemoryErrorCheckFlag ;			// 確保や解放が行われる度に確保メモリの情報が破損していないかをチェックするかどうかのフラグ
	size_t					AllocTrapSize ;						// 列挙対象にする確保サイズ

	int						InitializeFlag ;					// 初期化フラグ
	int						StartTime ;							// カウント開始時間
	HEAPINFO				SmallHeap ;							// 小さいサイズのメモリ確保用ヒープ
	HEAPINFO				BigHeap ;							// 大きいサイズのメモリ確保用ヒープ

#if defined( DX_THREAD_SAFE ) || defined( DX_THREAD_SAFE_NETWORK_ONLY )
	int						InitMemoryAllocCriticalSection ;	// MemoryAllocCriticalSection の初期化が済んでいるかどうかのフラグ
	DX_CRITICAL_SECTION		MemoryAllocCriticalSection ;		// メモリ確保処理用クリティカルセクション
#endif
} ;

// テーブル-----------------------------------------------------------------------

// 内部大域変数宣言 --------------------------------------------------------------

// メモリ関係の情報
extern MEMORYDATA MemData ;

// 関数プロトタイプ宣言-----------------------------------------------------------

extern	int			MemoryInitialize( void ) ;												// メモリ処理の初期化を行う
extern	int			MemoryTerminate( void ) ;												// メモリ処理の後始末を行う
extern	int			MemoryProcess( void ) ;													// メモリ処理の周期的処理
extern	int			MemoryDrawAllocInfo( int x, int y, int Width, int Height ) ;			// メモリの確保状況を描画する

extern	void		MemoryDump( void *buffer, int size ) ;

extern	void *		DxAllocBase( size_t AllocSize, size_t Aligned, const char *File = NULL, int Line = -1, int NoMemoryDump = FALSE ) ;	// DxAlloc の内部処理用関数
extern	void *		DxReallocBase( void *Memory, size_t AllocSize, size_t Aligned, const char *File = NULL, int Line = -1 ) ;			// DxRealloc の内部処理用関数


// 環境依存関数
extern	int			MemoryInitialize_PF( void ) ;								// メモリ処理の初期化を行う関数の環境依存処理
extern	int			MemoryTerminate_PF( void ) ;								// メモリ処理の後始末を行う関数の環境依存処理
extern	int			MemoryProcess_PF( void ) ;									// メモリ処理の周期的処理の環境依存処理

extern	int					 NormalMemory_AutoAlloc_CreateHeapCallback_PF(     int Param, void *Buffer, ALLOCMEM_SIZE_TYPE Size ) ;		// 環境依存の一般的なヒープ用のメモリ領域を確保するためのコールバック関数群の AutoAlloc_CreateHeapCallback
extern	void *				 NormalMemory_AutoAlloc_GetHeapAddressCallback_PF( int Param, void *Buffer ) ;			// 環境依存の一般的なヒープ用のメモリ領域を確保するためのコールバック関数群の AutoAlloc_GetHeapAddressCallback
extern	ALLOCMEM_SIZE_TYPE	 NormalMemory_AutoAlloc_GetHeapSizeCallback_PF(    int Param, void *Buffer ) ;			// 環境依存の一般的なヒープ用のメモリ領域を確保するためのコールバック関数群の AutoAlloc_GetHeapSizeCallback
extern	int					 NormalMemory_AutoAlloc_DeleteHeapCallback_PF(     int Param, void *Buffer ) ;			// 環境依存の一般的なヒープ用のメモリ領域を確保するためのコールバック関数群の AutoAlloc_DeleteHeapCallback

extern	int			DxDumpAlloc_PF( void ) ;									// 確保しているメモリを列挙する関数の環境依存処理

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // __DXMEMORY_H__
