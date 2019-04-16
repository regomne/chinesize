// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		ヒープ関連プログラム
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

// インクルード ------------------------------------------------------------------
#include "DxHeap.h"
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


#if defined( DX_THREAD_SAFE ) || defined( DX_THREAD_SAFE_NETWORK_ONLY )

#define DX_HEAP_ENTER_CRITICAL_SECTION( Heap )\
	CRITICALSECTION_LOCK( &(Heap)->MemoryAllocCriticalSection ) ;

#define DX_HEAP_LEAVE_CRITICAL_SECTION( Heap )\
	CriticalSection_Unlock( &(Heap)->MemoryAllocCriticalSection ) ;

#else

#define DX_HEAP_ENTER_CRITICAL_SECTION( Heap )
#define DX_HEAP_LEAVE_CRITICAL_SECTION( Heap )

#endif

// 構造体定義 --------------------------------------------------------------------

// テーブル-----------------------------------------------------------------------

// 内部大域変数宣言 --------------------------------------------------------------

// 関数プロトタイプ宣言-----------------------------------------------------------

// 新しいメモリタグに基本情報をセットする関数
static	void		AllocMemTag_SetBaseInfo( HEAPINFO *Heap, ALLOCMEMTAG *MemTag, int Use, int NameLineWrite, const char *Name = NULL, int Line = 0, int NoMemoryDump = FALSE ) ;

static	void		AllocMemTag_SetDebugData( ALLOCMEMTAG *MemTag ) ;							// デバッグ領域に情報をセットする
static	int			AllocMemTag_CheckDebugData( ALLOCMEMTAG *MemTag ) ;							// デバッグ領域の情報をチェックする( 0:正常  -1:エラー )

// メモリが足りなかった場合に自動的に新しいヒープを確保してそこからメモリを確保する処理を行う
static	void *		AllocMemory_AutoAllocHeap( HEAPINFO *Heap, ALLOCMEM_SIZE_TYPE Size, ALLOCMEM_SIZE_TYPE Aligned, int Reverse, const char *Name, int Line, int NoMemoryDump ) ;

static	void		Heap_AddNotUseSepMemTag( HEAPINFO *Heap, ALLOCMEMTAG *AddNotUseSepMemTag ) ;								// ヒープの未使用メモリタグリストに新しいメモリタグを追加する
static	void		Heap_SubNotUseSepMemTag( HEAPINFO *Heap, ALLOCMEMTAG *SubNotUseSepMemTag ) ;								// ヒープの未使用メモリタグリストから指定のメモリタグを外す

static	void		Heap_AddNotUseSepHeapMem( HEAPINFO *Heap, HEAPMEMINFO *AddNotUseSepHeapMem ) ;								// ヒープの未使用ヒープメモリ情報リストに新しいヒープメモリ情報を追加する
static	void		Heap_SubNotUseSepHeapMem( HEAPINFO *Heap, HEAPMEMINFO *SubNotUseSepHeapMem ) ;								// ヒープの未使用ヒープメモリ情報リストから指定のヒープメモリ情報を外す

static	void		Heap_AddUseMemTag(  HEAPINFO *Heap, ALLOCMEMTAG *AddUseMemTag, ALLOCMEMTAG *PrevUseMemTag ) ;				// ヒープの使用メモリリストに新しいメモリタグを追加する
static	void		Heap_SubUseMemTag(  HEAPINFO *Heap, ALLOCMEMTAG *SubUseMemTag ) ;											// ヒープの使用メモリリストから指定のメモリタグを外す

// ALLOCMEMTYPE_SIMPLE用関数
static	void		Heap_AddFreeMemTag_Simple( HEAPINFO *Heap, ALLOCMEMTAG *AddFreeMemTag, ALLOCMEMTAG *PrevFreeMemTag ) ;		// ヒープの空きメモリリストに新しいメモリタグを追加する( ALLOCMEMTYPE_SIMPLE用 )
static	void		Heap_SubFreeMemTag_Simple( HEAPINFO *Heap, ALLOCMEMTAG *SubFreeMemTag ) ;									// ヒープの空きメモリリストから指定のメモリタグを外す( ALLOCMEMTYPE_SIMPLE用 )

// ALLOCMEMTYPE_TLSF用関数
static	void		Heap_AddFreeMemTag_TLSF( HEAPINFO *Heap, ALLOCMEMTAG *AddFreeMemTag ) ;										// ヒープの空きメモリリストに新しいメモリタグを追加する( ALLOCMEMTYPE_TLSF用 )
static	void		Heap_SubFreeMemTag_TLSF( HEAPINFO *Heap, ALLOCMEMTAG *SubFreeMemTag ) ;										// ヒープの空きメモリリストから指定のメモリタグを外す( ALLOCMEMTYPE_TLSF用 )

// ヘルパー関数
static	void *		Heap_AllocMemory( HEAPINFO *Heap, ALLOCMEMTAG *MemTag, DWORD_PTR UseAddress, ALLOCMEM_SIZE_TYPE UserSize, ALLOCMEM_SIZE_TYPE Size, ALLOCMEM_SIZE_TYPE Aligned, int Reverse, const char *Name, int Line, int NoMemoryDump ) ;		// 指定の空きメモリタグに対して指定サイズのメモリを確保した際の処理を行う
static	void		Heap_FreeMemory(  HEAPINFO *Heap, ALLOCMEMTAG *MemTag, int ReallocProcess = FALSE ) ;						// 指定の使用メモリタグを解放する処理を行う


// ビット関係

// ビットの数を数える
static __inline DWORD GetBitCount( ALLOCMEM_SIZE_TYPE Num )
{
#ifdef __64BIT__

	Num = (   Num        & ULL_NUM( 0x5555555555555555 ) ) + ( ( Num >>  1 ) & ULL_NUM( 0x5555555555555555 ) ) ;
	Num = (   Num        & ULL_NUM( 0x3333333333333333 ) ) + ( ( Num >>  2 ) & ULL_NUM( 0x3333333333333333 ) ) ;
	Num = (   Num        & ULL_NUM( 0x0f0f0f0f0f0f0f0f ) ) + ( ( Num >>  4 ) & ULL_NUM( 0x0f0f0f0f0f0f0f0f ) ) ;
	Num = (   Num        & ULL_NUM( 0x00ff00ff00ff00ff ) ) + ( ( Num >>  8 ) & ULL_NUM( 0x00ff00ff00ff00ff ) ) ;
	Num = (   Num        & ULL_NUM( 0x0000ffff0000ffff ) ) + ( ( Num >> 16 ) & ULL_NUM( 0x0000ffff0000ffff ) ) ;
	Num = (   Num        & ULL_NUM( 0x00000000ffffffff ) ) + ( ( Num >> 32 ) & ULL_NUM( 0x00000000ffffffff ) ) ;

#else // __64BIT__
	Num = (   Num        & 0x55555555 ) + ( ( Num >>  1 ) & 0x55555555 ) ;
	Num = (   Num        & 0x33333333 ) + ( ( Num >>  2 ) & 0x33333333 ) ;
	Num = (   Num        & 0x0f0f0f0f ) + ( ( Num >>  4 ) & 0x0f0f0f0f ) ;
	Num = (   Num        & 0x00ff00ff ) + ( ( Num >>  8 ) & 0x00ff00ff ) ;
	Num = (   Num        & 0x0000ffff ) + ( ( Num >> 16 ) & 0x0000ffff ) ;
#endif // __64BIT__

	return ( DWORD )Num ;
}

// 最上位ビットのビット番号を取得する
static __inline int GetMSB( ALLOCMEM_SIZE_TYPE Num )
{
	if( Num == 0 )
	{
		return -1 ;
	}

#ifdef __64BIT__
	Num |= Num >>  1 ;
	Num |= Num >>  2 ;
	Num |= Num >>  4 ;
	Num |= Num >>  8 ;
	Num |= Num >> 16 ;
	Num |= Num >> 32 ;
#else // __64BIT__
	Num |= Num >>  1 ;
	Num |= Num >>  2 ;
	Num |= Num >>  4 ;
	Num |= Num >>  8 ;
	Num |= Num >> 16 ;
#endif // __64BIT__
	return ( int )GetBitCount( Num ) - 1 ;
}

// 最下位ビットのビット番号を取得する
static __inline int GetLSB( ALLOCMEM_SIZE_TYPE Num )
{
	if( Num == 0 )
	{
		return -1 ;
	}

#ifdef __64BIT__
	Num |= Num <<  1 ;
	Num |= Num <<  2 ;
	Num |= Num <<  4 ;
	Num |= Num <<  8 ;
	Num |= Num << 16 ;
	Num |= Num << 32 ;
	return 64 - ( int )GetBitCount( Num ) ;
#else // __64BIT__
	Num |= Num <<  1 ;
	Num |= Num <<  2 ;
	Num |= Num <<  4 ;
	Num |= Num <<  8 ;
	Num |= Num << 16 ;
	return 32 - ( int )GetBitCount( Num ) ;
#endif // __64BIT__
}

// メモリ確保のカテゴリ２の値を取得する
static __inline DWORD GetSLI( ALLOCMEM_SIZE_TYPE Num, DWORD MSB, DWORD N )
{
#ifdef __64BIT__

	return ( DWORD )( ( Num & ( ( ULL_NUM( 1 ) << MSB ) - ULL_NUM( 1 ) ) ) >> ( MSB - N ) ) ;

#else // __64BIT__

	return ( Num & ( ( 1 << MSB ) - 1 ) ) >> ( MSB - N ) ;

#endif // __64BIT__
}

// プログラム --------------------------------------------------------------------

// 新しいメモリタグに基本情報をセットする関数
static	void		AllocMemTag_SetBaseInfo( HEAPINFO *Heap, ALLOCMEMTAG *MemTag, int Use, int NameLineWrite, const char *Name, int Line, int NoMemoryDump )
{
	// マジックＩＤをセット
	MemTag->MagicID = ALLOCMEMTAG_MAGIC_ID ;

	if( Use )
	{
		// 使用中フラグを立てる
		MemTag->Flag |= ALLOCMEMTAG_FLAG_USE ;
	}
	else
	{
		// 使用中フラグを倒す
		MemTag->Flag &= ~ALLOCMEMTAG_FLAG_USE ;
	}

	if( NoMemoryDump )
	{
		// メモリダンプを行わないフラグを立てる
		MemTag->Flag |= ALLOCMEMTAG_FLAG_NO_DUMP ;
	}
	else
	{
		// メモリダンプを行わないフラグを倒す
		MemTag->Flag &= ~ALLOCMEMTAG_FLAG_NO_DUMP ;
	}

	// 指定がある場合のみ名前と行番号を更新する
	if( NameLineWrite )
	{
		// 名前を保存
		if( Name == NULL )
		{
			CL_strcpy( DX_CHARCODEFORMAT_ASCII, MemTag->Name, Use ? "NoName" : "Free" ) ;
		}
		else
		{
//			CL_strncpy( DX_CHARCODEFORMAT_ASCII, MemTag->Name, Name, ALLOCMEMTAG_NAMELENGTH - 1 ) ;
			int Length = ( int )CL_strlen( DX_CHARCODEFORMAT_ASCII, Name ) ;
			CL_strcpy( DX_CHARCODEFORMAT_ASCII, MemTag->Name, &Name[ Length < ( ALLOCMEMTAG_NAMELENGTH - 1 ) ? 0 : Length - ( ALLOCMEMTAG_NAMELENGTH - 1 ) ] ) ;
		}

		// 行番号保存
		if( Line < 0 )
		{
			Line = 0 ;
		}
		MemTag->Flag &= ~( ALLOCMEMTAG_FLAG_LINE_BIT_16 | ALLOCMEMTAG_FLAG_LINE_BIT_17 ) ;
		MemTag->Line = ( WORD )Line ;
		MemTag->Flag |= ( WORD )( ( DWORD )Line >> 16 ) ;
	}

	// 所属しているヒープをセット
	MemTag->Owner	= Heap ;

	// IDをセット
	MemTag->IDHigh	= ( BYTE )( Heap->AllocMemoryID >> 16 ) ;
	MemTag->IDLow	= ( WORD )Heap->AllocMemoryID ;
	Heap->AllocMemoryID ++ ;
	if( Heap->AllocMemoryID >= 0x1000000 )
	{
		Heap->AllocMemoryID = 0 ;
	}

	// 時間をセット
	MemTag->Time	= ( WORD )Heap->AllocMemoryTime ;
}

// デバッグ領域に情報をセットする関数
static	void		AllocMemTag_SetDebugData( ALLOCMEMTAG *MemTag )
{
	DWORD *CheckAddress ;

	CheckAddress = ( DWORD * )( ( BYTE * )MemTag + ALLOCMEMTAG_SIZE ) ;
	CheckAddress[ 0 ] = 0xaaaaaaaa ;
	CheckAddress[ 1 ] = 0xaaaaaaaa ;
	CheckAddress[ 2 ] = 0xaaaaaaaa ;
	CheckAddress[ 3 ] = 0xaaaaaaaa ;

	CheckAddress = ( DWORD * )( ( BYTE * )MemTag + MemTag->Size - ALLOCMEMTAG_DEBUGAREA_SIZE ) ;
	CheckAddress[ 0 ] = 0xaaaaaaaa ;
	CheckAddress[ 1 ] = 0xaaaaaaaa ;
	CheckAddress[ 2 ] = 0xaaaaaaaa ;
	CheckAddress[ 3 ] = 0xaaaaaaaa ;
}

// デバッグ領域の情報をチェックする( 0:正常  -1:エラー )
static	int			AllocMemTag_CheckDebugData( ALLOCMEMTAG *MemTag )
{
	DWORD *CheckAddress ;

	CheckAddress = ( DWORD * )( ( BYTE * )MemTag + ALLOCMEMTAG_SIZE ) ;
	if( CheckAddress[ 0 ] != 0xaaaaaaaa || CheckAddress[ 1 ] != 0xaaaaaaaa || CheckAddress[ 2 ] != 0xaaaaaaaa || CheckAddress[ 3 ] != 0xaaaaaaaa )
	{
		return -1 ;
	}

	CheckAddress = ( DWORD * )( ( BYTE * )MemTag + MemTag->Size - ALLOCMEMTAG_DEBUGAREA_SIZE ) ;
	if( CheckAddress[ 0 ] != 0xaaaaaaaa || CheckAddress[ 1 ] != 0xaaaaaaaa || CheckAddress[ 2 ] != 0xaaaaaaaa || CheckAddress[ 3 ] != 0xaaaaaaaa )
	{
		return -1 ;
	}

	return 0 ;
}

// メモリが足りなかった場合に自動的に新しいヒープを確保してそこからメモリを確保する処理を行う
static	void *		AllocMemory_AutoAllocHeap( HEAPINFO *Heap, ALLOCMEM_SIZE_TYPE Size, ALLOCMEM_SIZE_TYPE Aligned, int Reverse, const char *Name, int Line, int NoMemoryDump )
{
	ALLOCMEM_SIZE_TYPE RequestHeapSize ;
	ALLOCMEM_SIZE_TYPE AutoAllocUnitSize ;

	// 自動メモリ確保をしない場合は NULL を返す
	if( Heap->AutoAlloc == FALSE )
	{
		return NULL ;
	}

	// 新しいヒープ用メモリ領域のサイズを決定
	RequestHeapSize = Size ;

	// TLSF の場合は要求メモリサイズよりも大幅に大きなサイズのメモリを
	// 必要とする場合があるので決定処理をもう少し詳しく行う
	if( Heap->AllocType == ALLOCMEMTYPE_TLSF )
	{
		ALLOCMEM_SIZE_TYPE AddSize ;
		DWORD FLI ;
		DWORD SLI ;
		DWORD TempFLI ;
		DWORD TempSLI ;

		// 必要サイズが必ず収まる必要があるので、カテゴリ２のインデックスはひとつ大きくなる
		FLI = GetMSB( Size ) ;
		SLI = GetSLI( Size, FLI, ALLOCMEM_CATEGORY_2_SHIFT_NUM ) ;
		SLI += 1 ;
		if( SLI >= ( 1 << ALLOCMEM_CATEGORY_2_SHIFT_NUM ) )
		{
			SLI -= ( 1 << ALLOCMEM_CATEGORY_2_SHIFT_NUM ) ;
			FLI ++ ;
		}

		// 追加するヒープメモリに希望するメモリ容量を決定する
		do
		{
			AddSize = RequestHeapSize / 100 ;
			if( AddSize <= 1024 * 1024 )
			{
				AddSize = 1024 * 1024 ;
			}
			RequestHeapSize += AddSize ;

			TempFLI = GetMSB( RequestHeapSize ) ;
			TempSLI = GetSLI( RequestHeapSize, TempFLI, ALLOCMEM_CATEGORY_2_SHIFT_NUM ) ;
		}while( TempFLI <= FLI && TempSLI < SLI ) ;
	}

	// 新しいヒープ用メモリ領域を追加
	AutoAllocUnitSize = Heap->AutoAllocUnitSize ;
	for(;;)
	{
		if( AddHeapMemory( Heap, NULL, RequestHeapSize + AutoAllocUnitSize ) >= 0 )
		{
			break ;
		}

		// 確保に失敗したら自動確保サイズを半分にして再度試す
		AutoAllocUnitSize /= 2 ;

		// 自動確保確保サイズが 1MB 以下だったらエラー終了
		if( AutoAllocUnitSize < 1 * 1024 * 1024 )
		{
			return NULL ;
		}
	}

	// メモリを確保
	return AllocMemory( Heap, Size, Aligned, Reverse, Name, Line, NoMemoryDump ) ;
}

// ヒープの未使用メモリタグリストに新しいメモリタグを追加する
static	void		Heap_AddNotUseSepMemTag( HEAPINFO *Heap, ALLOCMEMTAG *AddNotUseSepMemTag )
{
	Heap->SepTagArrayUseNum -- ;

	AddNotUseSepMemTag->ListNext = Heap->SepTagArrayNotUseFirst ;
	AddNotUseSepMemTag->ListPrev = NULL ;
	Heap->SepTagArrayNotUseFirst = AddNotUseSepMemTag ;
	if( AddNotUseSepMemTag->ListNext != NULL )
	{
		AddNotUseSepMemTag->ListNext->ListPrev = AddNotUseSepMemTag ;
	}
}

// ヒープの未使用メモリタグリストから指定のメモリタグを外す
static	void		Heap_SubNotUseSepMemTag( HEAPINFO *Heap, ALLOCMEMTAG *SubNotUseSepMemTag )
{
	Heap->SepTagArrayUseNum ++ ;

	if( SubNotUseSepMemTag->ListPrev == NULL )
	{
		Heap->SepTagArrayNotUseFirst = SubNotUseSepMemTag->ListNext ;
	}
	else
	{
		SubNotUseSepMemTag->ListPrev->ListNext = SubNotUseSepMemTag->ListNext ;
	}

	if( SubNotUseSepMemTag->ListNext != NULL )
	{
		SubNotUseSepMemTag->ListNext->ListPrev = SubNotUseSepMemTag->ListPrev ;
	}

	SubNotUseSepMemTag->ListPrev = NULL ;
	SubNotUseSepMemTag->ListNext = NULL ;
}

// ヒープの未使用ヒープメモリ情報リストに新しいヒープメモリ情報を追加する
static	void		Heap_AddNotUseSepHeapMem( HEAPINFO *Heap, HEAPMEMINFO *AddNotUseSepHeapMem )
{
	Heap->SepHeapMemArrayUseNum -- ;

	AddNotUseSepHeapMem->Next = Heap->SepHeapMemArrayNotUseFirst ;
	AddNotUseSepHeapMem->Prev = NULL ;
	Heap->SepHeapMemArrayNotUseFirst = AddNotUseSepHeapMem ;
	if( AddNotUseSepHeapMem->Next != NULL )
	{
		AddNotUseSepHeapMem->Next->Prev = AddNotUseSepHeapMem ;
	}
}

// ヒープの未使用ヒープメモリ情報リストから指定のヒープメモリ情報を外す
static	void		Heap_SubNotUseSepHeapMem( HEAPINFO *Heap, HEAPMEMINFO *SubNotUseSepHeapMem )
{
	Heap->SepHeapMemArrayUseNum ++ ;

	if( SubNotUseSepHeapMem->Prev == NULL )
	{
		Heap->SepHeapMemArrayNotUseFirst = SubNotUseSepHeapMem->Next ;
	}
	else
	{
		SubNotUseSepHeapMem->Prev->Next = SubNotUseSepHeapMem->Next ;
	}

	if( SubNotUseSepHeapMem->Next != NULL )
	{
		SubNotUseSepHeapMem->Next->Prev = SubNotUseSepHeapMem->Prev ;
	}

	SubNotUseSepHeapMem->Prev = NULL ;
	SubNotUseSepHeapMem->Next = NULL ;
}

// ヒープの使用メモリリストに新しいメモリタグを追加する
static	void		Heap_AddUseMemTag( HEAPINFO *Heap, ALLOCMEMTAG *AddUseMemTag, ALLOCMEMTAG *PrevUseMemTag )
{
	// 一番近い使用メモリタグを探す
	while( PrevUseMemTag != NULL && ( PrevUseMemTag->Flag & ALLOCMEMTAG_FLAG_USE ) == 0 )
	{
		PrevUseMemTag = PrevUseMemTag->Prev ;
	}

	AddUseMemTag->ListPrev = PrevUseMemTag ;
	if( AddUseMemTag->ListPrev == NULL )
	{
		AddUseMemTag->ListNext = Heap->UseFirstTag ;
		Heap->UseFirstTag = AddUseMemTag ;
	}
	else
	{
		AddUseMemTag->ListNext = AddUseMemTag->ListPrev->ListNext ;
		AddUseMemTag->ListPrev->ListNext = AddUseMemTag ;
	}

	if( AddUseMemTag->ListNext == NULL )
	{
		Heap->UseLastTag = AddUseMemTag ;
	}
	else
	{
		AddUseMemTag->ListNext->ListPrev = AddUseMemTag ;
	}
}

// ヒープの使用メモリリストから指定のメモリタグを外す
static	void		Heap_SubUseMemTag( HEAPINFO *Heap, ALLOCMEMTAG *SubUseMemTag )
{
	if( SubUseMemTag->ListPrev == NULL )
	{
		Heap->UseFirstTag = SubUseMemTag->ListNext ;
	}
	else
	{
		SubUseMemTag->ListPrev->ListNext = SubUseMemTag->ListNext ;
	}

	if( SubUseMemTag->ListNext == NULL )
	{
		Heap->UseLastTag = SubUseMemTag->ListPrev ;
	}
	else
	{
		SubUseMemTag->ListNext->ListPrev = SubUseMemTag->ListPrev ;
	}

	SubUseMemTag->ListPrev = NULL ;
	SubUseMemTag->ListNext = NULL ;
}

// ヒープの空きメモリリストに新しいメモリタグを追加する( ALLOCMEMTYPE_SIMPLE用 )
static	void		Heap_AddFreeMemTag_Simple( HEAPINFO *Heap, ALLOCMEMTAG *AddFreeMemTag, ALLOCMEMTAG *PrevFreeMemTag )
{
	AddFreeMemTag->ListPrev = PrevFreeMemTag ;
	if( AddFreeMemTag->ListPrev == NULL )
	{
		AddFreeMemTag->ListNext = Heap->FreeFirstTag ;
		Heap->FreeFirstTag      = AddFreeMemTag ;
	}
	else
	{
		AddFreeMemTag->ListNext = AddFreeMemTag->ListPrev->ListNext ;
		AddFreeMemTag->ListPrev->ListNext = AddFreeMemTag ;
	}

	if( AddFreeMemTag->ListNext == NULL )
	{
		Heap->FreeLastTag = AddFreeMemTag ;
	}
	else
	{
		AddFreeMemTag->ListNext->ListPrev = AddFreeMemTag ;
	}
}

// ヒープの空きメモリリストから指定のメモリタグを外す( ALLOCMEMTYPE_SIMPLE用 )
static	void		Heap_SubFreeMemTag_Simple( HEAPINFO *Heap, ALLOCMEMTAG *SubFreeMemTag )
{
	if( SubFreeMemTag->ListPrev == NULL )
	{
		Heap->FreeFirstTag = SubFreeMemTag->ListNext ;
	}
	else
	{
		SubFreeMemTag->ListPrev->ListNext = SubFreeMemTag->ListNext ;
	}

	if( SubFreeMemTag->ListNext == NULL )
	{
		Heap->FreeLastTag = SubFreeMemTag->ListPrev ;
	}
	else
	{
		SubFreeMemTag->ListNext->ListPrev = SubFreeMemTag->ListPrev ;
	}

	SubFreeMemTag->ListPrev = NULL ;
	SubFreeMemTag->ListNext = NULL ;
}

// ヒープの空きメモリリストに新しいメモリタグを追加する( ALLOCMEMTYPE_TLSF用 )
static	void		Heap_AddFreeMemTag_TLSF( HEAPINFO *Heap, ALLOCMEMTAG *AddFreeMemTag )
{
	// カテゴリ１を取得する
	AddFreeMemTag->FLI = ( BYTE )GetMSB( AddFreeMemTag->Size ) ;

	// カテゴリ２を取得する
	AddFreeMemTag->SLI = ( BYTE )GetSLI( AddFreeMemTag->Size, AddFreeMemTag->FLI, ALLOCMEM_CATEGORY_2_SHIFT_NUM ) ;

	// カテゴリ１のフリービットを立てる
#ifdef __64BIT__
	Heap->FreeTagBitList1 |= ULL_NUM( 1 ) << AddFreeMemTag->FLI ;
#else // __64BIT__
	Heap->FreeTagBitList1 |= 1            << AddFreeMemTag->FLI ;
#endif // __64BIT__

	// カテゴリ２のフリービットを立てる
	Heap->FreeTagBitList2[ AddFreeMemTag->FLI ] |= 1 << AddFreeMemTag->SLI ;

	// 空きメモリリストの先頭に追加する
	AddFreeMemTag->ListPrev = NULL ;
	AddFreeMemTag->ListNext = Heap->FreeFirstTagArray[ AddFreeMemTag->FLI ][ AddFreeMemTag->SLI ] ;
	Heap->FreeFirstTagArray[ AddFreeMemTag->FLI ][ AddFreeMemTag->SLI ] = AddFreeMemTag ;

	if( AddFreeMemTag->ListNext == NULL )
	{
		Heap->FreeFirstTagArray[ AddFreeMemTag->FLI ][ AddFreeMemTag->SLI ] = AddFreeMemTag ;
	}
	else
	{
		AddFreeMemTag->ListNext->ListPrev = AddFreeMemTag ;
	}
}

// ヒープの空きメモリリストから指定のメモリタグを外す( ALLOCMEMTYPE_TLSF用 )
static	void		Heap_SubFreeMemTag_TLSF( HEAPINFO *Heap, ALLOCMEMTAG *SubFreeMemTag )
{
	// 空きメモリリストから外す
	if( SubFreeMemTag->ListPrev == NULL )
	{
		Heap->FreeFirstTagArray[ SubFreeMemTag->FLI ][ SubFreeMemTag->SLI ] = SubFreeMemTag->ListNext ;
	}
	else
	{
		SubFreeMemTag->ListPrev->ListNext = SubFreeMemTag->ListNext ;
	}

	if( SubFreeMemTag->ListNext == NULL )
	{
		Heap->FreeLastTagArray[ SubFreeMemTag->FLI ][ SubFreeMemTag->SLI ] = SubFreeMemTag->ListPrev ;
	}
	else
	{
		SubFreeMemTag->ListNext->ListPrev = SubFreeMemTag->ListPrev ;
	}
	SubFreeMemTag->ListPrev = NULL ;
	SubFreeMemTag->ListNext = NULL ;

	// もし使用していたカテゴリ２からひとつも空きメモリタグが無くなったらフリービットを倒す
	if( Heap->FreeFirstTagArray[ SubFreeMemTag->FLI ][ SubFreeMemTag->SLI ] == NULL )
	{
		Heap->FreeTagBitList2[ SubFreeMemTag->FLI ] &= ~( 1 << SubFreeMemTag->SLI ) ;

		// もし使用していたカテゴリ１管轄のカテゴリ２からひとつも空きメモリタグが無くなったらフリービットを倒す
		if( Heap->FreeTagBitList2[ SubFreeMemTag->FLI ] == 0 )
		{
			#ifdef __64BIT__
				Heap->FreeTagBitList1 &= ~( ULL_NUM( 1 ) << SubFreeMemTag->FLI ) ;
			#else // __64BIT__
				Heap->FreeTagBitList1 &= ~( 1            << SubFreeMemTag->FLI ) ;
			#endif // __64BIT__
		}
	}
}

// ヘルパー関数

// 指定の空きメモリタグに対して指定のメモリを確保した際の処理を行う
static void * Heap_AllocMemory( HEAPINFO *Heap, ALLOCMEMTAG *MemTag, DWORD_PTR UseAddress, ALLOCMEM_SIZE_TYPE UserSize, ALLOCMEM_SIZE_TYPE Size, ALLOCMEM_SIZE_TYPE Aligned, int Reverse, const char *Name, int Line, int NoMemoryDump )
{
	void *ReturnAddress = NULL ;

	// タグ配列を分離している場合は空きが一つは無いとエラー
	if( Heap->UseSeparateInfo )
	{
		if( Heap->SepTagArrayNotUseFirst == NULL )
		{
			return NULL ;
		}
	}

	// 使用しているメモリ領域の数を増やす
	Heap->UseTagNum ++ ;

	// メモリ領域を使用している数を増やす
	MemTag->UseHeapMem->UseMemTagCount ++ ;

	// リバース且つメモリ確保タイプが ALLOCMEMTYPE_SIMPLE の場合と、それ以外で処理を分岐
	if( Heap->AllocType == ALLOCMEMTYPE_SIMPLE && Reverse )
	{
		// 空きメモリから必要なメモリサイズを引いても、残った空きメモリにメモリタグが入れられる場合は
		// 既存の空きメモリタグはそのままに、使用メモリタグを新規に作成する
		{
			ALLOCMEM_SIZE_TYPE	NewMemTagAddress = 0 ;
			ALLOCMEM_SIZE_TYPE	AfterFreeSize ;

			// 残るフリーメモリのサイズを算出
			if( Heap->UseSeparateInfo )
			{
				AfterFreeSize = UseAddress - ( DWORD_PTR )MemTag->AllocAddress ;
			}
			else
			{
				// 新たなメモリタグのアドレスを算出
				{
					NewMemTagAddress = UseAddress - ( ALLOCMEMTAG_TOTAL_SIZE - ALLOCMEMTAG_DEBUGAREA_SIZE ) ;

					// アラインを適用
					NewMemTagAddress = NewMemTagAddress / Aligned * Aligned ;
				}

				// 残るフリーメモリのサイズを算出
				AfterFreeSize = NewMemTagAddress - ( DWORD_PTR )MemTag ;
			}

			// 一つタグを作ることが出来るかチェック
			if( ( Heap->UseSeparateInfo          && AfterFreeSize >= ALLOCMEM_MIN_ALIGNED ) ||
				( Heap->UseSeparateInfo == FALSE && AfterFreeSize >= ALLOCMEMTAG_MIN_SIZE ) )
			{
				ALLOCMEMTAG * NewUseMemTag ;

				// 作れる場合は新しい使用メモリタグを作成
				if( Heap->UseSeparateInfo )
				{
					NewUseMemTag = Heap->SepTagArrayNotUseFirst ;
					Heap_SubNotUseSepMemTag( Heap, NewUseMemTag ) ;

					// 使用するメモリ領域のアドレスをセット
					NewUseMemTag->AllocAddress = ( void * )UseAddress ;
				}
				else
				{
					NewUseMemTag = ( ALLOCMEMTAG * )NewMemTagAddress ;
				}

				// 基本情報をセット
				NewUseMemTag->Flag = 0 ;
				AllocMemTag_SetBaseInfo( Heap, NewUseMemTag, TRUE, TRUE, Name, Line, NoMemoryDump ) ;

				// 使用しているメモリ領域をセット
				NewUseMemTag->UseHeapMem = MemTag->UseHeapMem ;

				// 前後のメモリ領域と連続しているかどうかをセット
				NewUseMemTag->Flag |= ALLOCMEMTAG_FLAG_PREV_CONNECT ;
				if( MemTag->Flag & ALLOCMEMTAG_FLAG_NEXT_CONNECT )
				{
					NewUseMemTag->Flag |= ALLOCMEMTAG_FLAG_NEXT_CONNECT ;
				}
				else
				{
					NewUseMemTag->Flag &= ~ALLOCMEMTAG_FLAG_NEXT_CONNECT ;
				}

				// 既存のメモリ確保タグの次は必ずメモリ領域が連続している
				MemTag->Flag |= ALLOCMEMTAG_FLAG_NEXT_CONNECT ;

				// サイズをセット
				if( Heap->UseSeparateInfo )
				{
					NewUseMemTag->Size = ( ( DWORD_PTR )MemTag->AllocAddress + MemTag->Size ) - ( DWORD_PTR )UseAddress ;
				}
				else
				{
					NewUseMemTag->Size = ( ( DWORD_PTR )MemTag + MemTag->Size ) - ( DWORD_PTR )NewUseMemTag ;

					// デバッグ領域に情報をセット
					AllocMemTag_SetDebugData( NewUseMemTag ) ;
				}

				// 前後のメモリとリストを連結
				{
					NewUseMemTag->Prev = MemTag ;
					NewUseMemTag->Next = MemTag->Next ;
					MemTag->Next       = NewUseMemTag ;
					if( NewUseMemTag->Next == NULL )
					{
						Heap->LastTag = NewUseMemTag ;
					}
					else
					{
						NewUseMemTag->Next->Prev = NewUseMemTag ;
					}
				}

				// 使用メモリリストに追加する
				Heap_AddUseMemTag( Heap, NewUseMemTag, MemTag->Prev ) ;

				// 今までのメモリタグのサイズを変更
				if( Heap->UseSeparateInfo )
				{
					MemTag->Size = AfterFreeSize ;
				}
				else
				{
					MemTag->Size = NewMemTagAddress - ( DWORD_PTR )MemTag ;

					// 今までのメモリタグのサイズが変更されたので、デバッグ領域を再度セット
					AllocMemTag_SetDebugData( MemTag ) ;
				}

				// 空きメモリのサイズを減らす
				Heap->TotalFreeSize -= NewUseMemTag->Size ;

				// 使用メモリのサイズを増やす
				Heap->TotalUseSize += NewUseMemTag->Size ;

				// 指定されたサイズをセット
				NewUseMemTag->UserSize = UserSize ;

				// 返すアドレスをセット
				NewUseMemTag->UserAddress = ( void * )UseAddress ;
				if( Heap->UseSeparateInfo )
				{
					ReturnAddress = NewUseMemTag ;
				}
				else
				{
					ReturnAddress = NewUseMemTag->UserAddress ;

					// 返すアドレスの４バイト後ろにタグまでのバイト数をセット
					*( ( DWORD * )( ( BYTE * )NewUseMemTag->UserAddress - sizeof( DWORD ) ) ) = ( DWORD )( UseAddress - ( DWORD_PTR )NewUseMemTag ) ;
				}
			}
			else
			{
				// 入らない場合は、既存のメモリタグ全部を使用メモリタグにしてしまう

				// 指定されたサイズをセット
				MemTag->UserSize = UserSize ;

				// 返すアドレスをセット
				MemTag->UserAddress = ( void * )UseAddress ;
				if( Heap->UseSeparateInfo )
				{
					ReturnAddress = MemTag ;
				}
				else
				{
					ReturnAddress = MemTag->UserAddress ;

					// 返すアドレスの４バイト後ろにタグまでのバイト数をセット
					*( ( DWORD * )( ( BYTE * )MemTag->UserAddress - sizeof( DWORD ) ) ) = ( DWORD )( UseAddress - ( DWORD_PTR )MemTag ) ;
				}

				// メモリタグを空きメモリリストから外す
				Heap_SubFreeMemTag_Simple( Heap, MemTag ) ;

				// 基本情報をセット
				AllocMemTag_SetBaseInfo( Heap, MemTag, TRUE, TRUE, Name, Line, NoMemoryDump ) ;

				// 使用メモリリストに追加する
				Heap_AddUseMemTag( Heap, MemTag, MemTag->Prev ) ;

				// 空きメモリの数を減らす
				Heap->FreeTagNum -- ;

				// 空きメモリのサイズを減らす
				Heap->TotalFreeSize -= MemTag->Size ;

				// 使用メモリのサイズを増やす
				Heap->TotalUseSize += MemTag->Size ;
			}
		}
	}
	else
	{
		ALLOCMEMTAG *	FreePrevMemTag ;

		// 指定されたサイズをセット
		MemTag->UserSize = UserSize ;

		// 返すアドレスをセット
		MemTag->UserAddress = ( void * )UseAddress ;
		if( Heap->UseSeparateInfo )
		{
			ReturnAddress = MemTag ;
		}
		else
		{
			ReturnAddress = MemTag->UserAddress ;

			// 返すアドレスの４バイト後ろにタグまでのバイト数をセット
			*( ( DWORD * )( ( BYTE * )MemTag->UserAddress - sizeof( DWORD ) ) ) = ( DWORD )( UseAddress - ( DWORD_PTR )MemTag ) ;
		}

		// 空きメモリタグの一つ前のタグを記憶しておく
		FreePrevMemTag = MemTag->ListPrev ;

		// メモリタグを空きメモリリストから外す
		if( Heap->AllocType == ALLOCMEMTYPE_SIMPLE )
		{
			Heap_SubFreeMemTag_Simple( Heap, MemTag ) ;
		}
		else
		if( Heap->AllocType == ALLOCMEMTYPE_TLSF )
		{
			Heap_SubFreeMemTag_TLSF( Heap, MemTag ) ;
		}

		// 基本情報をセットする
		AllocMemTag_SetBaseInfo( Heap, MemTag, TRUE, TRUE, Name, Line, NoMemoryDump ) ;

		// 使用メモリリストに追加する
		Heap_AddUseMemTag( Heap, MemTag, MemTag->Prev ) ;

		// 空きメモリから必要なメモリサイズを引いても、残った空きメモリにメモリタグが入れられる場合は空きメモリタグを増やす
		{
			ALLOCMEM_SIZE_TYPE	NextAddress = 0 ;
			ALLOCMEM_SIZE_TYPE	NextFreeSize ;

			// 残る空きメモリのサイズを算出
			if( Heap->UseSeparateInfo )
			{
				// 残る空きメモリのサイズを算出
				NextFreeSize = ( ( DWORD_PTR )MemTag->AllocAddress + MemTag->Size ) - ( UseAddress + Size ) ;
			}
			else
			{
				// 空きメモリを確保した場合に次のタグを配置できるアドレスを算出
				NextAddress = UseAddress + Size + ALLOCMEMTAG_DEBUGAREA_SIZE ;

				// アラインを適用
				NextAddress = ( NextAddress + ALLOCMEM_MIN_ALIGNED - 1 ) / ALLOCMEM_MIN_ALIGNED * ALLOCMEM_MIN_ALIGNED ;

				// 残る空きメモリのサイズを算出
				NextFreeSize = ( ( DWORD_PTR )MemTag + MemTag->Size ) - NextAddress ;
			}

			// 一つタグを作ることが出来るかチェック
			if( ( Heap->UseSeparateInfo          && NextFreeSize >= ALLOCMEM_MIN_ALIGNED ) ||
				( Heap->UseSeparateInfo == FALSE && NextFreeSize >= ALLOCMEMTAG_MIN_SIZE ) )
			{
				ALLOCMEMTAG * NewFreeMemTag ;

				// 作れる場合は新しい空きメモリタグを作成
				if( Heap->UseSeparateInfo )
				{
					NewFreeMemTag = Heap->SepTagArrayNotUseFirst ;
					Heap_SubNotUseSepMemTag( Heap, NewFreeMemTag ) ;

					// 使用するメモリ領域のアドレスをセット
					NewFreeMemTag->AllocAddress = ( void * )( UseAddress + Size ) ;
				}
				else
				{
					NewFreeMemTag = ( ALLOCMEMTAG * )NextAddress ;
				}

				// 基本情報をセット
				NewFreeMemTag->Flag = 0 ;
				AllocMemTag_SetBaseInfo( Heap, NewFreeMemTag, FALSE, TRUE, "Free", 0 ) ;

				// 使用しているメモリ領域をセット
				NewFreeMemTag->UseHeapMem = MemTag->UseHeapMem ;

				// 前後のメモリ領域と連続しているかどうかをセット
				NewFreeMemTag->Flag = ALLOCMEMTAG_FLAG_PREV_CONNECT ;
				if( MemTag->Flag & ALLOCMEMTAG_FLAG_NEXT_CONNECT )
				{
					NewFreeMemTag->Flag |= ALLOCMEMTAG_FLAG_NEXT_CONNECT ;
				}
				else
				{
					NewFreeMemTag->Flag &= ~ALLOCMEMTAG_FLAG_NEXT_CONNECT ;
				}

				// 新しい空きメモリタグのサイズをセット
				if( Heap->UseSeparateInfo )
				{
					NewFreeMemTag->Size		= NextFreeSize ;
				}
				else
				{
					NewFreeMemTag->Size		= ( ( DWORD_PTR )MemTag + MemTag->Size ) - ( DWORD_PTR )NewFreeMemTag ;

					// デバッグ領域に情報をセット
					AllocMemTag_SetDebugData( NewFreeMemTag ) ;
				}

				// 前後のメモリとリストを連結
				{
					NewFreeMemTag->Prev = MemTag ;
					NewFreeMemTag->Next = MemTag->Next ;
					MemTag->Next        = NewFreeMemTag ;
					if( NewFreeMemTag->Next == NULL )
					{
						Heap->LastTag = NewFreeMemTag ;
					}
					else
					{
						NewFreeMemTag->Next->Prev = NewFreeMemTag ;
					}
				}

				// 既存のメモリ確保タグの次は必ずメモリ領域が連続している
				MemTag->Flag |= ALLOCMEMTAG_FLAG_NEXT_CONNECT ;

				// 空きメモリリストに追加
				if( Heap->AllocType == ALLOCMEMTYPE_SIMPLE )
				{
					Heap_AddFreeMemTag_Simple( Heap, NewFreeMemTag, FreePrevMemTag ) ;
				}
				else
				if( Heap->AllocType == ALLOCMEMTYPE_TLSF )
				{
					Heap_AddFreeMemTag_TLSF( Heap, NewFreeMemTag ) ;
				}

				// 今までのメモリタグのサイズを変更
				if( Heap->UseSeparateInfo )
				{
					MemTag->Size = ( DWORD_PTR )NewFreeMemTag->AllocAddress - ( DWORD_PTR )MemTag->AllocAddress ;
				}
				else
				{
					MemTag->Size = NextAddress - ( DWORD_PTR )MemTag ;

					// 今までのメモリタグのサイズが変更されたので、デバッグ領域を再度セット
					AllocMemTag_SetDebugData( MemTag ) ;
				}
			}
			else
			{
				// 作ることが出来ない場合は空きメモリタグを増やさない

				// 空きメモリの数を減らす
				Heap->FreeTagNum -- ;
			}

			// 空きメモリのサイズを減らす
			Heap->TotalFreeSize -= MemTag->Size ;

			// 使用メモリのサイズを増やす
			Heap->TotalUseSize += MemTag->Size ;
		}
	}

	return ReturnAddress ;
}

// 指定の使用メモリタグを解放する処理を行う
static void Heap_FreeMemory( HEAPINFO *Heap, ALLOCMEMTAG *MemTag, int ReallocProcess )
{
	// 使用しているメモリの数を減らす
	Heap->UseTagNum -- ;

	// メモリ領域を使用している数を減らす
	MemTag->UseHeapMem->UseMemTagCount -- ;

	// 空きメモリの数を増やす
	Heap->FreeTagNum ++ ;

	// 使用メモリのサイズを減らす
	Heap->TotalUseSize -= MemTag->Size ;

	// 空きメモリのサイズを増やす
	Heap->TotalFreeSize += MemTag->Size ;

	// 使用メモリのフラグを倒す
	MemTag->Flag &= ~ALLOCMEMTAG_FLAG_USE ;

	// 使用メモリのリストから外す
	Heap_SubUseMemTag( Heap, MemTag ) ;
	
	// 空きメモリリストに追加する
	{
		ALLOCMEMTAG *	SubMemTag ;
		int				AddFreeListFlag = FALSE ;

		// 一つ低いアドレスの隣のメモリが空きメモリだったら連結する
		// ( ReallocMemory の処理の一環の場合は低いアドレスの空きメモリとの連結は行わない )
		if( ReallocProcess == FALSE &&
			( MemTag->Flag & ALLOCMEMTAG_FLAG_PREV_CONNECT ) != 0 &&
			MemTag->Prev != NULL &&
			( MemTag->Prev->Flag & ALLOCMEMTAG_FLAG_USE ) == 0 )
		{
			SubMemTag = MemTag->Prev ;

			// 確保タイプが ALLOCMEMTYPE_TLSF の場合は、一旦空きメモリリストから外す
			if( Heap->AllocType == ALLOCMEMTYPE_TLSF )
			{
				Heap_SubFreeMemTag_TLSF( Heap, SubMemTag ) ;
			}

			// サイズを足す
			SubMemTag->Size += MemTag->Size ;

			// リストの連結を変更する
			SubMemTag->Next = MemTag->Next ;
			if( SubMemTag->Next == NULL )
			{
				Heap->LastTag = SubMemTag ;
			}
			else
			{
				SubMemTag->Next->Prev = SubMemTag ;
			}

			// 次のタグとメモリ領域が連続しているかをセット
			if( MemTag->Flag & ALLOCMEMTAG_FLAG_NEXT_CONNECT )
			{
				SubMemTag->Flag |= ALLOCMEMTAG_FLAG_NEXT_CONNECT ;
			}
			else
			{
				SubMemTag->Flag &= ~ALLOCMEMTAG_FLAG_NEXT_CONNECT ;
			}

			// 不要になったメモリタグを未使用リストに追加する
			if( Heap->UseSeparateInfo )
			{
				Heap_AddNotUseSepMemTag( Heap, MemTag ) ;
			}

			// 空きメモリの数を減らす
			Heap->FreeTagNum -- ;

			// 処理対象のタグを変更
			MemTag = SubMemTag ;

			// MemTag がフリーリストに追加されているかどうかのフラグを立てる
			AddFreeListFlag = TRUE ;
		}

		// 一つ高いアドレスの隣のメモリが空きメモリだったら連結する
		if( ( MemTag->Flag & ALLOCMEMTAG_FLAG_NEXT_CONNECT ) != 0 &&
			MemTag->Next != NULL &&
			( MemTag->Next->Flag & ALLOCMEMTAG_FLAG_USE ) == 0 )
		{
			SubMemTag = MemTag->Next ;

			// 確保タイプが ALLOCMEMTYPE_TLSF の場合は、一旦空きメモリリストから外す
			if( Heap->AllocType == ALLOCMEMTYPE_TLSF )
			{
				Heap_SubFreeMemTag_TLSF( Heap, SubMemTag ) ;
			}

			// サイズを足す
			MemTag->Size += SubMemTag->Size ;

			// リストの連結を変更する
			MemTag->Next = SubMemTag->Next ;
			if( MemTag->Next == NULL )
			{
				Heap->LastTag = MemTag ;
			}
			else
			{
				MemTag->Next->Prev = MemTag ;
			}

			// 確保タイプが ALLOCMEMTYPE_SIMPLE の場合は、ここで空きメモリリストの連結を変更する
			if( Heap->AllocType == ALLOCMEMTYPE_SIMPLE )
			{
				MemTag->ListNext = SubMemTag->ListNext ;
				if( MemTag->ListNext == NULL )
				{
					Heap->FreeLastTag = MemTag ;
				}
				else
				{
					MemTag->ListNext->ListPrev = MemTag ;
				}

				if( AddFreeListFlag == FALSE )
				{
					MemTag->ListPrev = SubMemTag->ListPrev ;
					if( MemTag->ListPrev == NULL )
					{
						Heap->FreeFirstTag = MemTag ;
					}
					else
					{
						MemTag->ListPrev->ListNext = MemTag ;
					}
				}
			}

			// 次のタグとメモリ領域が連続しているかをセット
			if( SubMemTag->Flag & ALLOCMEMTAG_FLAG_NEXT_CONNECT )
			{
				MemTag->Flag |= ALLOCMEMTAG_FLAG_NEXT_CONNECT ;
			}
			else
			{
				MemTag->Flag &= ~ALLOCMEMTAG_FLAG_NEXT_CONNECT ;
			}

			// 不要になったメモリタグを未使用リストに追加する
			if( Heap->UseSeparateInfo )
			{
				Heap_AddNotUseSepMemTag( Heap, SubMemTag ) ;
			}

			// 空きメモリの数を減らす
			Heap->FreeTagNum -- ;

			// MemTag がフリーリストに追加されているかどうかのフラグを立てる
			AddFreeListFlag = TRUE ;
		}

		// 確保タイプによって処理を分岐
		if( Heap->AllocType == ALLOCMEMTYPE_SIMPLE )
		{
			// 前後に空きメモリが無かったらそのまま空きメモリリストに追加する
			if( AddFreeListFlag == FALSE )
			{
				// メモリアドレスが低く、一番近い空きメモリを探す
				for( SubMemTag = MemTag->Prev ; SubMemTag != NULL && ( SubMemTag->Flag & ALLOCMEMTAG_FLAG_USE ) != 0 ; SubMemTag = SubMemTag->Prev ){}
				
				// 空きメモリリストに追加
				Heap_AddFreeMemTag_Simple( Heap, MemTag, SubMemTag ) ;
			}
		}
		else
		if( Heap->AllocType == ALLOCMEMTYPE_TLSF )
		{
			// 空きメモリリストに追加する
			Heap_AddFreeMemTag_TLSF( Heap, MemTag ) ;
		}
	}

	// もし使用しているメモリヒープから一つも使用メモリが無くなり、
	// 且つ ReallocMemory の処理の一環でもなく加えて自動確保メモリだった場合は解放する
	if( MemTag->UseHeapMem->UseMemTagCount == 0 &&
		ReallocProcess == 0 && ( MemTag->UseHeapMem->Flag & HEAPMEM_FLAG_AUTOALLOC ) != 0 )
	{
		SubHeapMemory( Heap, MemTag->UseHeapMem->Address ) ;
	}
}















// ヒープ関数

// ヒープを初期化する
extern int InitializeHeap(
	HEAPINFO *				Heap,
	int						AllocType,
	ALLOCMEM_SIZE_TYPE		Aligned,
	int						AutoAlloc,
	ALLOCMEM_SIZE_TYPE		AutoAllocUnitSize,
	int						AutoAlloc_CallbackParam,
	int					 ( *AutoAlloc_CreateHeapCallback     )( int Param, void *Buffer, ALLOCMEM_SIZE_TYPE Size ),
	void *				 ( *AutoAlloc_GetHeapAddressCallback )( int Param, void *Buffer ),
	ALLOCMEM_SIZE_TYPE	 ( *AutoAlloc_GetHeapSizeCallback    )( int Param, void *Buffer ),
	int					 ( *AutoAlloc_DeleteHeapCallback     )( int Param, void *Buffer ),
	void *					HeapAddress,
	ALLOCMEM_SIZE_TYPE		HeapSize,
	int						UseSeparateInfo,
	int						SepTagArrayLength,
	ALLOCMEMTAG *			SepTagArray,
	int						SepHeapMemArrayLength,
	HEAPMEMINFO *			SepHeapMemArray
)
{
	// 構造体サイズエラーチェック
	if( sizeof( ALLOCMEMTAG ) != ALLOCMEMTAG_SIZE )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\xcb\x69\x20\x90\x53\x4f\x20\x00\x41\x00\x4c\x00\x4c\x00\x4f\x00\x43\x00\x4d\x00\x45\x00\x4d\x00\x54\x00\x41\x00\x47\x00\x20\x00\x6e\x30\xb5\x30\xa4\x30\xba\x30\x4c\x30\x20\x00\x25\x00\x64\x00\x20\x00\x62\x00\x79\x00\x74\x00\x65\x00\x20\x00\x67\x30\x6f\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x00"/*@ L"構造体 ALLOCMEMTAG のサイズが %d byte ではありません" @*/, ALLOCMEMTAG_SIZE )) ;
		*( ( DWORD * )0x00000000 ) = 0xffffffff ;
	}

	// ゼロ初期化
	_MEMSET( Heap, 0, sizeof( *Heap ) ) ;

#if defined( DX_THREAD_SAFE ) || defined( DX_THREAD_SAFE_NETWORK_ONLY )

	// ヒープ用のクリティカルセクションの初期化
	CriticalSection_Initialize( &Heap->MemoryAllocCriticalSection ) ;

#endif // defined( DX_THREAD_SAFE ) || defined( DX_THREAD_SAFE_NETWORK_ONLY )

	DX_HEAP_ENTER_CRITICAL_SECTION( Heap )

	// アラインは ALLOCMEM_MIN_ALIGNED バイトの倍数にする
	if( Aligned == 0 )
	{
		Aligned = ALLOCMEM_MIN_ALIGNED ;
	}
	else
	{
		Aligned = ( Aligned + ALLOCMEM_MIN_ALIGNED - 1 ) / ALLOCMEM_MIN_ALIGNED * ALLOCMEM_MIN_ALIGNED ;
	}

	// パラメータを保存
	Heap->AllocType							= AllocType ;
	Heap->Aligned							= Aligned ;
	Heap->AutoAlloc							= AutoAlloc ;
	Heap->AutoAllocUnitSize					= AutoAllocUnitSize ;
	Heap->AutoAlloc_CallbackParam			= AutoAlloc_CallbackParam ;
	Heap->AutoAlloc_CreateHeapCallback		= AutoAlloc_CreateHeapCallback ;
	Heap->AutoAlloc_GetHeapAddressCallback	= AutoAlloc_GetHeapAddressCallback ;
	Heap->AutoAlloc_GetHeapSizeCallback		= AutoAlloc_GetHeapSizeCallback ;
	Heap->AutoAlloc_DeleteHeapCallback		= AutoAlloc_DeleteHeapCallback ;
	Heap->UseSeparateInfo					= UseSeparateInfo ;

	// メモリタグやヒープの情報を別メモリ領域で確保するかどうかの情報をセット
	if( Heap->UseSeparateInfo )
	{
		// 配列が有効な値ではない場合はエラー
		if( SepTagArrayLength     <= 0 || SepTagArray     == NULL ||
			SepHeapMemArrayLength <= 0 || SepHeapMemArray == NULL )
		{
			DX_HEAP_LEAVE_CRITICAL_SECTION( Heap )
			return -1 ;
		}

		// 配列のアドレスや配列の長さなどをセット
		Heap->SepTagArrayLength		= SepTagArrayLength ;
		Heap->SepTagArray			= SepTagArray ;
		Heap->SepHeapMemArrayLength	= SepHeapMemArrayLength ;
		Heap->SepHeapMemArray		= SepHeapMemArray ;

		// ゼロ初期化
		_MEMSET( SepTagArray,     0, sizeof( *SepTagArray     ) * SepTagArrayLength     ) ;
		_MEMSET( SepHeapMemArray, 0, sizeof( *SepHeapMemArray ) * SepHeapMemArrayLength ) ;

		// 未使用メモリタグリストの作成
		{
			Heap->SepTagArrayNotUseFirst     = SepTagArray ;

			if( SepTagArrayLength > 1 )
			{
				ALLOCMEMTAG *MemTag ;
				int i ;

				MemTag = SepTagArray ;
				MemTag->ListNext = MemTag + 1 ;
				MemTag ++ ;
				for( i = 1 ; i < SepTagArrayLength - 1 ; i ++, MemTag ++ )
				{
					MemTag->ListPrev = MemTag - 1 ;
					MemTag->ListNext = MemTag + 1 ;
				}
				MemTag->ListPrev = MemTag - 1 ;
			}
		}

		// 未使用ヒープメモリ情報リストの作成
		{
			Heap->SepHeapMemArrayNotUseFirst = SepHeapMemArray ;

			if( SepHeapMemArrayLength > 1 )
			{
				HEAPMEMINFO *HeapMem ;
				int i ;

				HeapMem = SepHeapMemArray ;
				HeapMem->Next = HeapMem + 1 ;
				HeapMem ++ ;
				for( i = 1 ; i < SepHeapMemArrayLength - 1 ; i ++, HeapMem ++ )
				{
					HeapMem->Prev = HeapMem - 1 ;
					HeapMem->Next = HeapMem + 1 ;
				}
				HeapMem->Prev = HeapMem - 1 ;
			}
		}
	}

	// ヒープメモリのアドレスが有効な場合はヒープ用メモリを追加
	if( HeapAddress != NULL && HeapSize != 0 )
	{
		if( AddHeapMemory( Heap, HeapAddress, HeapSize ) < 0 )
		{
			DX_HEAP_LEAVE_CRITICAL_SECTION( Heap )
			return -1 ;
		}
	}

	DX_HEAP_LEAVE_CRITICAL_SECTION( Heap )

	// 正常終了
	return 0 ;
}

// ヒープの後始末を行う
extern int TerminateHeap( HEAPINFO *Heap )
{
	HEAPMEMINFO *HeapMemInfo ;
	HEAPMEMINFO *NextHeapMemInfo ;

	DX_HEAP_ENTER_CRITICAL_SECTION( Heap )

	// 自動確保されたヒープメモリを解放する
	for( HeapMemInfo = Heap->FirstHeapMem ; HeapMemInfo != NULL ; HeapMemInfo = NextHeapMemInfo )
	{
		NextHeapMemInfo = HeapMemInfo->Next ;

		if( HeapMemInfo->Flag & HEAPMEM_FLAG_AUTOALLOC )
		{
			BYTE CallbackBuffer[ ALLOCHEAP_CALLBACK_INFO_SIZE ] ;
			_MEMCPY( CallbackBuffer, HeapMemInfo->CallbackBuffer, ALLOCHEAP_CALLBACK_INFO_SIZE ) ;

			Heap->AutoAlloc_DeleteHeapCallback( Heap->AutoAlloc_CallbackParam, CallbackBuffer ) ;
		}
	}

	DX_HEAP_LEAVE_CRITICAL_SECTION( Heap )

	// ゼロ初期化
	_MEMSET( Heap, 0, sizeof( *Heap ) ) ;

	return 0 ;
}


// ヒープとして扱うメモリを追加する
extern int AddHeapMemory( HEAPINFO *Heap, void *HeapAddress, ALLOCMEM_SIZE_TYPE HeapSize )
{
	BYTE				CallbackBuffer[ ALLOCHEAP_CALLBACK_INFO_SIZE ] ;
	HEAPMEMINFO *		HeapMemInfo ;
	DWORD_PTR			UseAddress ;
	ALLOCMEM_SIZE_TYPE	UseSize ;
	int					IsAutoAlloc = FALSE ;

	DX_HEAP_ENTER_CRITICAL_SECTION( Heap )

	if( HeapAddress == NULL && Heap->AutoAlloc )
	{
//		if( HeapSize < Heap->AutoAllocUnitSize )
//		{
//			HeapSize = Heap->AutoAllocUnitSize ;
//		}
		// 1MB 以下のヒープの作成はできない
		if( HeapSize < 1 * 1024 * 1024 )
		{
			HeapSize = 1 * 1024 * 1024 ;
		}

		// アライン分足す
		HeapSize += ALLOCMEM_MIN_ALIGNED ;

		// 更にアラインを適用する
		HeapSize = ( HeapSize + ALLOCMEM_MIN_ALIGNED - 1 ) / ALLOCMEM_MIN_ALIGNED * ALLOCMEM_MIN_ALIGNED ;

		// 自動確保メモリのサイズをチェック
		if( Heap->UseSeparateInfo == FALSE && HeapSize < ( sizeof( HEAPMEMINFO ) + ALLOCMEM_MIN_ALIGNED - 1 ) / ALLOCMEM_MIN_ALIGNED * ALLOCMEM_MIN_ALIGNED + ALLOCMEMTAG_MIN_SIZE )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xd2\x30\xfc\x30\xd7\x30\x6e\x30\xea\x81\xd5\x52\xba\x78\xdd\x4f\xb5\x30\xa4\x30\xba\x30\x28\x00\x20\x00\x25\x00\x64\x00\x62\x00\x79\x00\x74\x00\x65\x00\x20\x00\x29\x00\x4c\x30\x0f\x5c\x55\x30\x59\x30\x4e\x30\x7e\x30\x59\x30\x01\x30\x00\x67\x4e\x4f\x67\x30\x82\x30\x20\x00\x25\x00\x64\x00\x62\x00\x79\x00\x74\x00\x65\x00\x20\x00\xc5\x5f\x81\x89\x67\x30\x59\x30\x00"/*@ L"ヒープの自動確保サイズ( %dbyte )が小さすぎます、最低でも %dbyte 必要です" @*/, HeapSize, ( sizeof( HEAPMEMINFO ) + ALLOCMEM_MIN_ALIGNED - 1 ) / ALLOCMEM_MIN_ALIGNED * ALLOCMEM_MIN_ALIGNED + ALLOCMEMTAG_MIN_SIZE )) ;
			DX_HEAP_LEAVE_CRITICAL_SECTION( Heap )
			return -1 ;
		}

		// 環境依存ヒープの作成
		_MEMSET( CallbackBuffer, 0, ALLOCHEAP_CALLBACK_INFO_SIZE ) ;
		if( Heap->AutoAlloc_CreateHeapCallback( Heap->AutoAlloc_CallbackParam, CallbackBuffer, HeapSize ) < 0 )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xd2\x30\xfc\x30\xd7\x30\x28\x75\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"ヒープ用メモリの確保に失敗しました" @*/ )) ;
			DX_HEAP_LEAVE_CRITICAL_SECTION( Heap )
			return -1 ;
		}

		// 実際に確保したメモリのサイズを取得
		HeapSize = Heap->AutoAlloc_GetHeapSizeCallback( Heap->AutoAlloc_CallbackParam, CallbackBuffer ) ;

		// 環境依存ヒープのメモリアドレスを取得
		HeapAddress = Heap->AutoAlloc_GetHeapAddressCallback( Heap->AutoAlloc_CallbackParam, CallbackBuffer ) ;

		// アラインを考慮して使用できるアドレスを算出
		UseAddress = ( ( DWORD_PTR )HeapAddress + ALLOCMEM_MIN_ALIGNED - 1 ) / ALLOCMEM_MIN_ALIGNED * ALLOCMEM_MIN_ALIGNED ;

		// アラインを考慮して使用できるサイズを算出
		UseSize    = ( ( ( DWORD_PTR )HeapAddress + HeapSize ) / ALLOCMEM_MIN_ALIGNED * ALLOCMEM_MIN_ALIGNED ) - UseAddress ;

		// 自動確保されたメモリのフラグを立てる
		IsAutoAlloc = TRUE ;
	}
	else
	{
		if( HeapAddress == NULL || HeapSize == 0 )
		{
			DX_HEAP_LEAVE_CRITICAL_SECTION( Heap )
			return -1 ;
		}

		// アラインを考慮して使用できるアドレスを算出
		UseAddress = ( ( DWORD_PTR )HeapAddress + ALLOCMEM_MIN_ALIGNED - 1 ) / ALLOCMEM_MIN_ALIGNED * ALLOCMEM_MIN_ALIGNED ;

		// アラインを考慮して使用できるサイズを算出
		UseSize    = ( ( ( DWORD_PTR )HeapAddress + HeapSize ) / ALLOCMEM_MIN_ALIGNED * ALLOCMEM_MIN_ALIGNED ) - UseAddress ;

		// ヒープ用メモリのサイズをチェック
		if( Heap->UseSeparateInfo == FALSE && UseSize < ( sizeof( HEAPMEMINFO ) + ALLOCMEM_MIN_ALIGNED - 1 ) / ALLOCMEM_MIN_ALIGNED * ALLOCMEM_MIN_ALIGNED + ALLOCMEMTAG_MIN_SIZE )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xd2\x30\xfc\x30\xd7\x30\x6e\x30\xb5\x30\xa4\x30\xba\x30\x28\x00\x20\x00\x25\x00\x64\x00\x62\x00\x79\x00\x74\x00\x65\x00\x20\x00\x29\x00\x4c\x30\x0f\x5c\x55\x30\x59\x30\x4e\x30\x7e\x30\x59\x30\x01\x30\x00\x67\x4e\x4f\x67\x30\x82\x30\x20\x00\x25\x00\x64\x00\x62\x00\x79\x00\x74\x00\x65\x00\x20\x00\xc5\x5f\x81\x89\x67\x30\x59\x30\x00"/*@ L"ヒープのサイズ( %dbyte )が小さすぎます、最低でも %dbyte 必要です" @*/, UseSize, ( sizeof( HEAPMEMINFO ) + ALLOCMEM_MIN_ALIGNED - 1 ) / ALLOCMEM_MIN_ALIGNED * ALLOCMEM_MIN_ALIGNED + ALLOCMEMTAG_MIN_SIZE )) ;
			DX_HEAP_LEAVE_CRITICAL_SECTION( Heap )
			return -1 ;
		}
	}

	if( Heap->UseSeparateInfo )
	{
		if( Heap->SepHeapMemArrayNotUseFirst == NULL )
		{
			DX_HEAP_LEAVE_CRITICAL_SECTION( Heap )
			return -1 ;
		}

		HeapMemInfo = Heap->SepHeapMemArrayNotUseFirst ;
		Heap_SubNotUseSepHeapMem( Heap, HeapMemInfo ) ;
	}
	else
	{
		HeapMemInfo = ( HEAPMEMINFO * )UseAddress ;
	}

	// ゼロ初期化
	_MEMSET( HeapMemInfo, 0, sizeof( *HeapMemInfo ) ) ;

	if( IsAutoAlloc )
	{
		// 自動確保されたメモリかどうかを保存
		HeapMemInfo->Flag		|= HEAPMEM_FLAG_AUTOALLOC ;

		// 自動確保の場合はコールバック関数に渡した情報を保存
		_MEMCPY( HeapMemInfo->CallbackBuffer, CallbackBuffer, ALLOCHEAP_CALLBACK_INFO_SIZE ) ;
	}

	// 渡された or 確保したメモリ領域のアドレスとサイズを保存
	HeapMemInfo->Address		= HeapAddress ;
	HeapMemInfo->Size			= HeapSize ;

	// アラインを考慮して実際に使用できるメモリ領域のアドレスとサイズを保存
	HeapMemInfo->UseAddress		= ( void * )UseAddress ;
	HeapMemInfo->UseSize		= UseSize ;

	// ヒープとして実際に使用できるアドレスとサイズを算出
	{
		DWORD_PTR HeapUseAddress ;
		DWORD_PTR HeapUseSize ;

		if( Heap->UseSeparateInfo )
		{
			HeapUseAddress = UseAddress ;
		}
		else
		{
			HeapUseAddress = ( DWORD_PTR )( HeapMemInfo + 1 ) ;
		}

		// アラインを適用する
		HeapUseAddress = ( HeapUseAddress + ALLOCMEM_MIN_ALIGNED - 1 ) / ALLOCMEM_MIN_ALIGNED * ALLOCMEM_MIN_ALIGNED ;

		HeapUseSize = ( ( DWORD_PTR )HeapMemInfo->Address + HeapMemInfo->Size ) - HeapUseAddress ;

		// アラインを適用する
		HeapUseSize = HeapUseSize / ALLOCMEM_MIN_ALIGNED * ALLOCMEM_MIN_ALIGNED ;

		HeapMemInfo->HeapAddress	= ( void * )HeapUseAddress ;
		HeapMemInfo->HeapSize		= HeapUseSize ;
	}

	// ヒープで扱うメモリのリストに追加
	HeapMemInfo->Next  = Heap->FirstHeapMem ;
	Heap->FirstHeapMem = HeapMemInfo ;
	if( HeapMemInfo->Next != NULL )
	{
		HeapMemInfo->Next->Prev = HeapMemInfo ;
	}

	// 空きメモリタグを追加
	{
		ALLOCMEMTAG *FreeMemTag ;

		if( Heap->UseSeparateInfo )
		{
			if( Heap->SepTagArrayNotUseFirst == NULL )
			{
				DX_HEAP_LEAVE_CRITICAL_SECTION( Heap )
				return -1 ;
			}

			FreeMemTag = Heap->SepTagArrayNotUseFirst ;
			Heap_SubNotUseSepMemTag( Heap, FreeMemTag ) ;
		}
		else
		{
			FreeMemTag = ( ALLOCMEMTAG * )HeapMemInfo->HeapAddress ;
		}

		// 基本情報をセット
		FreeMemTag->Flag		= 0 ;
		AllocMemTag_SetBaseInfo( Heap, FreeMemTag, FALSE, TRUE, "Free", 0 ) ;

		// UseSeparateInfo = TRUE の場合は確保メモリのアドレスをセット
		if( Heap->UseSeparateInfo )
		{
			FreeMemTag->AllocAddress = HeapMemInfo->HeapAddress ;
		}

		// サイズをセット
		FreeMemTag->Size		= HeapMemInfo->HeapSize ;

		// 使用しているヒープをセット
		FreeMemTag->UseHeapMem	= HeapMemInfo ;

		// ヒープメモリ情報の先頭メモリタグに登録
		HeapMemInfo->FirstMemTag = FreeMemTag ;

		// デバッグ領域に情報をセット
		if( Heap->UseSeparateInfo == FALSE )
		{
			AllocMemTag_SetDebugData( FreeMemTag ) ;
		}

		// メモリタグのリストに追加
		FreeMemTag->Next = Heap->FirstTag ;
		FreeMemTag->Prev = NULL ;
		Heap->FirstTag = FreeMemTag ;
		if( FreeMemTag->Next != NULL )
		{
			FreeMemTag->Next->Prev = FreeMemTag ;
		}
		else
		{
			Heap->LastTag          = FreeMemTag ;
		}

		// 空きメモリタグの数を追加
		Heap->FreeTagNum		+= 1 ;

		// 空きメモリの総サイズを追加
		Heap->TotalFreeSize		+= FreeMemTag->Size ;

		// 空きメモリタグリストに追加
		if( Heap->AllocType == ALLOCMEMTYPE_SIMPLE )
		{
			Heap_AddFreeMemTag_Simple( Heap, FreeMemTag, NULL ) ;
		}
		else
		if( Heap->AllocType == ALLOCMEMTYPE_TLSF )
		{
			Heap_AddFreeMemTag_TLSF( Heap, FreeMemTag ) ;
		}
	}

	DX_HEAP_LEAVE_CRITICAL_SECTION( Heap )

	return 0 ;
}

// ヒープとして扱うメモリを減らす
extern int SubHeapMemory( HEAPINFO *Heap, void *HeapAddress )
{
	HEAPMEMINFO *	HeapMemInfo ;
	ALLOCMEMTAG *	MemTag ;

	DX_HEAP_ENTER_CRITICAL_SECTION( Heap )

	if( Heap->UseSeparateInfo )
	{
		for( HeapMemInfo = Heap->FirstHeapMem ; HeapMemInfo != NULL && HeapMemInfo->Address != HeapAddress ; HeapMemInfo = HeapMemInfo->Next ){}
		if( HeapMemInfo == NULL )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x53\x00\x75\x00\x62\x00\x48\x00\x65\x00\x61\x00\x70\x00\x4d\x00\x65\x00\x6d\x00\x6f\x00\x72\x00\x79\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x07\x63\x9a\x5b\x55\x30\x8c\x30\x5f\x30\xe1\x30\xe2\x30\xea\x30\xa2\x30\xc9\x30\xec\x30\xb9\x30\x92\x30\x01\x63\x64\x30\xd2\x30\xfc\x30\xd7\x30\xe1\x30\xe2\x30\xea\x30\x4c\x30\x8b\x89\x64\x30\x4b\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x20\x00\x20\x00\x48\x00\x65\x00\x61\x00\x70\x00\x41\x00\x64\x00\x64\x00\x72\x00\x65\x00\x73\x00\x73\x00\x3a\x00\x30\x00\x78\x00\x25\x00\x6c\x00\x6c\x00\x30\x00\x31\x00\x36\x00\x78\x00\x00"/*@ L"SubHeapMemory Error : 指定されたメモリアドレスを持つヒープメモリが見つかりません  HeapAddress:0x%ll016x" @*/, ( ULONGLONG )HeapAddress )) ;
			*( ( DWORD * )0x00000000 ) = 0xffffffff ;
		}
	}
	else
	{
		HeapMemInfo = ( HEAPMEMINFO * )( ( ( DWORD_PTR )HeapAddress + ALLOCMEM_MIN_ALIGNED - 1 ) / ALLOCMEM_MIN_ALIGNED * ALLOCMEM_MIN_ALIGNED ) ;
	}

	// 指定されたヒープを使用しているメモリタグがある場合は解放できない
	if( HeapMemInfo->UseMemTagCount != 0 )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x53\x00\x75\x00\x62\x00\x48\x00\x65\x00\x61\x00\x70\x00\x4d\x00\x65\x00\x6d\x00\x6f\x00\x72\x00\x79\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x7f\x4f\x28\x75\x57\x30\x66\x30\x44\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\xbf\x30\xb0\x30\x4c\x30\x42\x30\x8b\x30\xb6\x72\x4b\x61\x67\x30\xd2\x30\xfc\x30\xd7\x30\xe1\x30\xe2\x30\xea\x30\x92\x30\x16\x59\x5d\x30\x46\x30\x68\x30\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x20\x00\x48\x00\x65\x00\x61\x00\x70\x00\x4d\x00\x65\x00\x6d\x00\x49\x00\x6e\x00\x66\x00\x6f\x00\x2d\x00\x3e\x00\x55\x00\x73\x00\x65\x00\x4d\x00\x65\x00\x6d\x00\x54\x00\x61\x00\x67\x00\x43\x00\x6f\x00\x75\x00\x6e\x00\x74\x00\x3a\x00\x25\x00\x64\x00\x00"/*@ L"SubHeapMemory Error : 使用しているメモリタグがある状態でヒープメモリを外そうとしました  HeapMemInfo->UseMemTagCount:%d" @*/, HeapMemInfo->UseMemTagCount )) ;
		*( ( DWORD * )0x00000000 ) = 0xffffffff ;
	}

	// ヒープメモリを使用しているタグが空きメモリタグ一つではなかったらエラー
	MemTag = ( ALLOCMEMTAG * )HeapMemInfo->FirstMemTag ;
	if( MemTag->Flag & ALLOCMEMTAG_FLAG_USE )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x53\x00\x75\x00\x62\x00\x48\x00\x65\x00\x61\x00\x70\x00\x4d\x00\x65\x00\x6d\x00\x6f\x00\x72\x00\x79\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xd2\x30\xfc\x30\xd7\x30\xe1\x30\xe2\x30\xea\x30\x92\x30\x7f\x4f\x28\x75\x57\x30\x66\x30\x44\x30\x8b\x30\x48\x51\x2d\x98\xbf\x30\xb0\x30\x4c\x30\x7f\x4f\x28\x75\xe1\x30\xe2\x30\xea\x30\xbf\x30\xb0\x30\x67\x30\x57\x30\x5f\x30\x00"/*@ L"SubHeapMemory Error : ヒープメモリを使用している先頭タグが使用メモリタグでした" @*/ )) ;
		PrintInfoMemory( Heap->UseSeparateInfo ? MemTag : MemTag->UserAddress, Heap->UseSeparateInfo ) ;
		*( ( DWORD * )0x00000000 ) = 0xffffffff ;
	}
	if( MemTag->UseHeapMem != HeapMemInfo )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x53\x00\x75\x00\x62\x00\x48\x00\x65\x00\x61\x00\x70\x00\x4d\x00\x65\x00\x6d\x00\x6f\x00\x72\x00\x79\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xd2\x30\xfc\x30\xd7\x30\xe1\x30\xe2\x30\xea\x30\x92\x30\x7f\x4f\x28\x75\x57\x30\x66\x30\x44\x30\x8b\x30\x48\x51\x2d\x98\xbf\x30\xb0\x30\x4c\x30\x7f\x4f\x28\x75\x57\x30\x66\x30\x44\x30\x8b\x30\xd2\x30\xfc\x30\xd7\x30\xe1\x30\xe2\x30\xea\x30\xc5\x60\x31\x58\x68\x30\x00\x4e\xf4\x81\x57\x30\x7e\x30\x5b\x30\x93\x30\x20\x00\x4d\x00\x65\x00\x6d\x00\x54\x00\x61\x00\x67\x00\x2d\x00\x3e\x00\x55\x00\x73\x00\x65\x00\x48\x00\x65\x00\x61\x00\x70\x00\x4d\x00\x65\x00\x6d\x00\x3a\x00\x30\x00\x78\x00\x25\x00\x6c\x00\x6c\x00\x30\x00\x31\x00\x36\x00\x78\x00\x20\x00\x20\x00\x48\x00\x65\x00\x61\x00\x70\x00\x4d\x00\x65\x00\x6d\x00\x49\x00\x6e\x00\x66\x00\x6f\x00\x3a\x00\x30\x00\x78\x00\x25\x00\x6c\x00\x6c\x00\x30\x00\x31\x00\x36\x00\x78\x00\x00"/*@ L"SubHeapMemory Error : ヒープメモリを使用している先頭タグが使用しているヒープメモリ情報と一致しません MemTag->UseHeapMem:0x%ll016x  HeapMemInfo:0x%ll016x" @*/, ( ULONGLONG )MemTag->UseHeapMem, ( ULONGLONG )HeapMemInfo )) ;
		PrintInfoMemory( Heap->UseSeparateInfo ? MemTag : MemTag->UserAddress, Heap->UseSeparateInfo ) ;
		*( ( DWORD * )0x00000000 ) = 0xffffffff ;
	}
	if( MemTag->Next != NULL && MemTag->Next->UseHeapMem == HeapMemInfo )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x53\x00\x75\x00\x62\x00\x48\x00\x65\x00\x61\x00\x70\x00\x4d\x00\x65\x00\x6d\x00\x6f\x00\x72\x00\x79\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xd2\x30\xfc\x30\xd7\x30\xe1\x30\xe2\x30\xea\x30\x92\x30\x7f\x4f\x28\x75\x57\x30\x66\x30\x44\x30\x8b\x30\x7f\x4f\x28\x75\xe1\x30\xe2\x30\xea\x30\xbf\x30\xb0\x30\x4c\x30\x8c\x4e\x64\x30\xe5\x4e\x0a\x4e\x42\x30\x8a\x30\x7e\x30\x59\x30\x00"/*@ L"SubHeapMemory Error : ヒープメモリを使用している使用メモリタグが二つ以上あります" @*/ )) ;
		PrintInfoMemory( Heap->UseSeparateInfo ? MemTag : MemTag->UserAddress, Heap->UseSeparateInfo ) ;
		*( ( DWORD * )0x00000000 ) = 0xffffffff ;
	}

	// 空きメモリの数を減らす
	Heap->TotalFreeSize -= MemTag->Size ;

	// 空きメモリタグの数を減らす
	Heap->FreeTagNum -- ;

	// メモリタグの連結を変更する
	{
		if( MemTag->Prev != NULL )
		{
			MemTag->Prev->Next = MemTag->Next ;
		}
		else
		{
			Heap->FirstTag = MemTag->Next ;
		}

		if( MemTag->Next != NULL )
		{
			MemTag->Next->Prev = MemTag->Prev ;
		}
		else
		{
			Heap->LastTag = MemTag->Prev ;
		}
	}

	// 空きメモリリストから外す
	if( Heap->AllocType == ALLOCMEMTYPE_SIMPLE )
	{
		Heap_SubFreeMemTag_Simple( Heap, MemTag ) ;
	}
	else
	if( Heap->AllocType == ALLOCMEMTYPE_TLSF )
	{
		Heap_SubFreeMemTag_TLSF( Heap, MemTag ) ;
	}

	// リストの連結から外す
	{
		if( HeapMemInfo->Prev == NULL )
		{
			Heap->FirstHeapMem = HeapMemInfo->Next ;
		}
		else
		{
			HeapMemInfo->Prev->Next = HeapMemInfo->Next ;
		}

		if( HeapMemInfo->Next != NULL )
		{
			HeapMemInfo->Next->Prev = HeapMemInfo->Prev ;
		}
	}

	// 未使用リストに追加する
	if( Heap->UseSeparateInfo )
	{
		Heap_AddNotUseSepHeapMem( Heap, HeapMemInfo ) ;
	}

	// 自動確保されたメモリだった場合は解放する
	if( HeapMemInfo->Flag & HEAPMEM_FLAG_AUTOALLOC )
	{
		BYTE CallbackBuffer[ ALLOCHEAP_CALLBACK_INFO_SIZE ] ;
		_MEMCPY( CallbackBuffer, HeapMemInfo->CallbackBuffer, ALLOCHEAP_CALLBACK_INFO_SIZE ) ;

		Heap->AutoAlloc_DeleteHeapCallback( Heap->AutoAlloc_CallbackParam, CallbackBuffer ) ;
	}

	DX_HEAP_LEAVE_CRITICAL_SECTION( Heap )

	// 正常終了
	return 0 ;
}

// ヒープが使用しているメモリの総サイズを取得する
extern	ALLOCMEM_SIZE_TYPE	GetHeapTotalMemorySize( HEAPINFO *Heap )
{
	return Heap->TotalUseSize + Heap->TotalFreeSize ;
}

// ヒープからメモリを確保する( UseSeparateInfo=TRUE の場合は、戻ってくるのは ALLOCMEMTAG 構造体のアドレス )
extern void * AllocMemory(
	HEAPINFO *			Heap,
	ALLOCMEM_SIZE_TYPE	Size,
	ALLOCMEM_SIZE_TYPE	Aligned,
	int					Reverse,
	const char *		Name,
	int					Line,
	int					NoMemoryDump
)
{
	void *	ReturnAddress = NULL ;
	ALLOCMEM_SIZE_TYPE	OrigSize = Size ;

	DX_HEAP_ENTER_CRITICAL_SECTION( Heap )

	// サイズが０の場合は NULL を返す
	if( Size == 0 )
	{
		DX_HEAP_LEAVE_CRITICAL_SECTION( Heap )
		return NULL ;
	}

	// 確保サイズをアラインの倍数にする
	if( Aligned == 0 )
	{
		Aligned = Heap->Aligned ;
	}
	else
	{
		if( Aligned < ALLOCMEM_MIN_ALIGNED )
		{
			Aligned = ALLOCMEM_MIN_ALIGNED ;
		}
	}
	Size = ( Size + Aligned - 1 ) / Aligned * Aligned ;

	// メモリ確保タイプによって処理を分岐
	if( Heap->AllocType == ALLOCMEMTYPE_SIMPLE )
	{
		ALLOCMEMTAG *		MemTag ;
		DWORD_PTR			UseAddress = 0 ;

		if( Reverse )
		{
			for( MemTag = Heap->FreeLastTag ; MemTag != NULL ; MemTag = MemTag->ListPrev )
			{
				// アラインを考慮しても指定のメモリサイズが確保できるかチェック
				if( Heap->UseSeparateInfo )
				{
					// サイズが間違いなく足りなければ次へ
					if( MemTag->Size < Size )
					{
						continue ;
					}

					// 返すアドレスの算出
					{
						UseAddress = ( ( DWORD_PTR )MemTag->AllocAddress + MemTag->Size ) - Size ;

						// アラインを適用
						UseAddress = UseAddress / Aligned * Aligned ;
					}

					// 有効なアドレスになっていたらループを抜ける
					if( UseAddress >= ( DWORD_PTR )MemTag->AllocAddress )
					{
						break ;
					}
				}
				else
				{
					// サイズが間違いなく足りなければ次へ
					if( MemTag->Size - ALLOCMEMTAG_TOTAL_SIZE < Size )
					{
						continue ;
					}

					// 返すアドレスの算出
					{
						UseAddress = ( ( DWORD_PTR )MemTag + MemTag->Size - ALLOCMEMTAG_DEBUGAREA_SIZE ) - Size ;

						// アラインを適用
						UseAddress = UseAddress / Aligned * Aligned ;
					}

					// 有効なアドレスになっていたらループを抜ける
					if( UseAddress >= ( DWORD_PTR )MemTag + ALLOCMEMTAG_TOTAL_SIZE - ALLOCMEMTAG_DEBUGAREA_SIZE )
					{
						break ;
					}
				}
			}

			// 空きメモリが無かった場合は処理を分岐
			if( MemTag == NULL )
			{
				// ヒープメモリを追加してメモリの確保を試みる
				ReturnAddress = AllocMemory_AutoAllocHeap( Heap, Size, Aligned, Reverse, Name, Line, NoMemoryDump ) ;
			}
			else
			{
				// 空きメモリタグに対してメモリの確保処理を行う
				ReturnAddress = Heap_AllocMemory( Heap, MemTag, UseAddress, OrigSize, Size, Aligned, Reverse, Name, Line, NoMemoryDump ) ;
			}
		}
		else
		{
			for( MemTag = Heap->FreeFirstTag ; MemTag != NULL ; MemTag = MemTag->ListNext )
			{
				ALLOCMEM_SIZE_TYPE	FreeSize ;

				// 指定のメモリサイズが確保できるかチェック
				if( Heap->UseSeparateInfo )
				{
					// 返すアドレスの算出
					{
						UseAddress = ( DWORD_PTR )MemTag->AllocAddress ;

						// アラインを適用
						UseAddress = ( UseAddress + Aligned - 1 ) / Aligned * Aligned ;
					}

					// 有効な空きメモリサイズを算出
					{
						FreeSize = ( ( DWORD_PTR )MemTag->AllocAddress + MemTag->Size ) - UseAddress ;

						// アラインを適用
						FreeSize = FreeSize / Aligned * Aligned ;
					}
				}
				else
				{
					// 返すアドレスの算出
					{
						UseAddress = ( DWORD_PTR )MemTag + ALLOCMEMTAG_TOTAL_SIZE - ALLOCMEMTAG_DEBUGAREA_SIZE ;

						// アラインを適用
						UseAddress = ( UseAddress + Aligned - 1 ) / Aligned * Aligned ;
					}

					// 有効な空きメモリサイズを算出
					{
						FreeSize = ( ( DWORD_PTR )MemTag + MemTag->Size ) - UseAddress - ALLOCMEMTAG_DEBUGAREA_SIZE ;

						// アラインを適用
						FreeSize = FreeSize / Aligned * Aligned ;
					}
				}

				// 空きメモリが足りる場合はループを抜ける
				if( FreeSize >= Size )
				{
					break ;
				}
			}

			// 空きメモリが無かった場合は処理を分岐
			if( MemTag == NULL )
			{
				// ヒープメモリを追加してメモリの確保を試みる
				ReturnAddress = AllocMemory_AutoAllocHeap( Heap, Size, Aligned, Reverse, Name, Line, NoMemoryDump ) ;
			}
			else
			{
				// 空きメモリタグに対してメモリの確保処理を行う
				ReturnAddress = Heap_AllocMemory( Heap, MemTag, UseAddress, OrigSize, Size, Aligned, Reverse, Name, Line, NoMemoryDump ) ;
			}
		}
	}
	else
	if( Heap->AllocType == ALLOCMEMTYPE_TLSF )
	{
		DWORD				FLI ;
		DWORD				SLI ;
		ALLOCMEM_SIZE_TYPE	UseSize ;
		DWORD_PTR			UseAddress ;
		ALLOCMEMTAG *		MemTag = NULL ;
//		int					Root = 0 ;
		DWORD				EnableBitList2 ;
		ALLOCMEM_SIZE_TYPE	EnableBitList1 ;

		// 必要なメモリ領域を算出
		if( Heap->UseSeparateInfo )
		{
			UseSize = Size ;
		}
		else
		{
			// タグを含めて必要なメモリ領域を算出
			UseSize = Size + ALLOCMEMTAG_MIN_SIZE ;
		}

		// アライン適用
		UseSize = ( UseSize + Aligned * 2 - 1 ) / Aligned * Aligned ;

		// 必要サイズのカテゴリ１とカテゴリ２を取得
		FLI = GetMSB( UseSize ) ;
		SLI = GetSLI( UseSize, FLI, ALLOCMEM_CATEGORY_2_SHIFT_NUM ) ;

		// 必要サイズが必ず収まる必要があるので、カテゴリ２のインデックスはひとつ大きくなる
		SLI += 1 ;
		if( SLI >= ( 1 << ALLOCMEM_CATEGORY_2_SHIFT_NUM ) )
		{
			SLI -= ( 1 << ALLOCMEM_CATEGORY_2_SHIFT_NUM ) ;
			FLI ++ ;
		}

		// 一致するカテゴリに空きメモリタグがあるかチェック
		if( Heap->FreeFirstTagArray[ FLI ][ SLI ] != NULL )
		{
			// あったらそのメモリタグを使用する
			MemTag = Heap->FreeFirstTagArray[ FLI ][ SLI ] ;

//			Root = 1 ;
		}
		else
		{
			// 無かった場合は必要なサイズが収まる最小のメモリタグを探す

			// まず同じカテゴリ１内により大きいサイズのカテゴリ２の空きメモリタグがあるか調べる
			EnableBitList2 = Heap->FreeTagBitList2[ FLI ] & ( 0xffffffff << SLI ) ;

			// あったらあった中で最小のカテゴリ２のメモリタグを使用する
			if( EnableBitList2 != 0 )
			{
				SLI = GetLSB( EnableBitList2 ) ;
				MemTag = Heap->FreeFirstTagArray[ FLI ][ SLI ] ;

//				Root = 2 ;
			}
			else
			{
				// 無かったらより大きいサイズのカテゴリ１に空きメモリタグがあるか調べる

				FLI ++ ;

				// カテゴリ１に収まらなくなってしまった場合はエラー
				if( FLI >= ALLOCMEM_CATEGORY_1_NUM )
				{
					return NULL ;
				}

			#ifdef __64BIT__
				EnableBitList1 = Heap->FreeTagBitList1 & ( ULL_NUM( 0xffffffffffffffff ) << FLI ) ;
			#else // __64BIT__
				EnableBitList1 = Heap->FreeTagBitList1 & (                  0xffffffff   << FLI ) ;
			#endif // __64BIT__

				// 無かったら指定のサイズが収まる空きメモリが無いということ
				if( EnableBitList1 == 0 )
				{
					// ヒープメモリを追加してメモリの確保を試みる
					ReturnAddress = AllocMemory_AutoAllocHeap( Heap, Size, Aligned, Reverse, Name, Line, NoMemoryDump ) ;

					DX_HEAP_LEAVE_CRITICAL_SECTION( Heap )
					return ReturnAddress ;
				}

				// あったらあった中で最小のカテゴリ１を取得
				FLI = GetLSB( EnableBitList1 ) ;

				// 更に最小のカテゴリ１の中の最小のカテゴリ２のメモリタグを使用
				SLI = GetLSB( Heap->FreeTagBitList2[ FLI ] ) ;
				MemTag = Heap->FreeFirstTagArray[ FLI ][ SLI ] ;

//				Root = 3 ;
			}
		}

		// 返すアドレスの算出
		if( Heap->UseSeparateInfo )
		{
			UseAddress = ( DWORD_PTR )MemTag->AllocAddress ;

			// アラインを適用
			UseAddress = ( UseAddress + Aligned - 1 ) / Aligned * Aligned ;

			// サイズのチェック
			{
				ALLOCMEM_SIZE_TYPE FreeSize = ( ( DWORD_PTR )MemTag->AllocAddress + MemTag->Size ) - UseAddress ;
				if( FreeSize < Size )
				{
					DXST_LOGFILEFMT_ADDUTF16LE(( "\x54\x00\x4c\x00\x53\x00\x46\x00\x20\x00\xab\x30\xc6\x30\xb4\x30\xea\x30\xcb\x69\x20\x90\xa8\x30\xe9\x30\xfc\x30\x20\x00\x31\x00\x20\x00\x20\x00\x4d\x00\x65\x00\x6d\x00\x54\x00\x61\x00\x67\x00\x2d\x00\x3e\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x20\x00\x25\x00\x64\x00\x00"/*@ L"TLSF カテゴリ構造エラー 1  MemTag->Size %d" @*/, MemTag->Size )) ;
					*( ( DWORD * )0x00000000 ) = 0xffffffff ;
				}
			}
		}
		else
		{
			UseAddress = ( DWORD_PTR )MemTag + ALLOCMEMTAG_TOTAL_SIZE - ALLOCMEMTAG_DEBUGAREA_SIZE ;

			// アラインを適用
			UseAddress = ( UseAddress + Aligned - 1 ) / Aligned * Aligned ;

			// サイズのチェック
			{
				ALLOCMEM_SIZE_TYPE FreeSize = ( ( DWORD_PTR )MemTag + MemTag->Size ) - UseAddress ;
				if( FreeSize < Size + ALLOCMEMTAG_DEBUGAREA_SIZE )
				{
					DXST_LOGFILEFMT_ADDUTF16LE(( "\x54\x00\x4c\x00\x53\x00\x46\x00\x20\x00\xab\x30\xc6\x30\xb4\x30\xea\x30\xcb\x69\x20\x90\xa8\x30\xe9\x30\xfc\x30\x20\x00\x32\x00\x20\x00\x20\x00\x4d\x00\x65\x00\x6d\x00\x54\x00\x61\x00\x67\x00\x2d\x00\x3e\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x20\x00\x25\x00\x64\x00\x00"/*@ L"TLSF カテゴリ構造エラー 2  MemTag->Size %d" @*/, MemTag->Size )) ;
					*( ( DWORD * )0x00000000 ) = 0xffffffff ;
				}
			}
		}

		// 空きメモリタグに対してメモリの確保処理を行う
		ReturnAddress = Heap_AllocMemory( Heap, MemTag, UseAddress, OrigSize, Size, Aligned, Reverse, Name, Line, NoMemoryDump ) ;
	}

	DX_HEAP_LEAVE_CRITICAL_SECTION( Heap )

	return ReturnAddress ;
}

// 確保したメモリのサイズを変更する( UseSeparateInfo=TRUE の場合は、AllocAddress に ALLOCMEMTAG 構造体のアドレスを渡す )
extern void * ReallocMemory( void *AllocAddress, int UseSeparateInfo, ALLOCMEM_SIZE_TYPE Size, ALLOCMEM_SIZE_TYPE Aligned, const char *Name, int Line )
{
	ALLOCMEMTAG *		MemTag ;
	HEAPINFO *			Heap ;
	ALLOCMEM_SIZE_TYPE	OrigSize = Size ;
	void *				ReturnAddress = NULL ;

	if( AllocAddress == NULL )
	{
		return 0 ;
	}

	// メモリタグのアドレスを算出
	if( UseSeparateInfo )
	{
		MemTag = ( ALLOCMEMTAG * )AllocAddress ;
	}
	else
	{
		MemTag = ( ALLOCMEMTAG * )( ( BYTE * )AllocAddress - *( ( DWORD * )( ( BYTE * )AllocAddress - sizeof( DWORD ) ) ) ) ;
	}

	// マジックＩＤの確認
	if( MemTag->MagicID != ALLOCMEMTAG_MAGIC_ID )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x52\x00\x65\x00\x61\x00\x6c\x00\x6c\x00\x6f\x00\x63\x00\x4d\x00\x65\x00\x6d\x00\x6f\x00\x72\x00\x79\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xe1\x30\xe2\x30\xea\x30\xbf\x30\xb0\x30\x6e\x30\x20\x00\x4d\x00\x61\x00\x67\x00\x69\x00\x63\x00\x49\x00\x44\x00\x20\x00\x4c\x30\x0d\x4e\x63\x6b\x67\x30\x59\x30\x01\x30\xa4\x8a\x63\x30\x5f\x30\xe1\x30\xe2\x30\xea\x30\xa2\x30\xc9\x30\xec\x30\xb9\x30\x4c\x30\x21\x6e\x55\x30\x8c\x30\x5f\x30\x4b\x30\x01\x30\xe1\x30\xe2\x30\xea\x30\x4c\x30\x34\x78\xca\x58\x55\x30\x8c\x30\x66\x30\x44\x30\x7e\x30\x59\x30\x00"/*@ L"ReallocMemory Error : メモリタグの MagicID が不正です、誤ったメモリアドレスが渡されたか、メモリが破壊されています" @*/ )) ;
		*( ( DWORD * )0x00000000 ) = 0xffffffff ;
	}

	// ヒープのアドレスをセット
	Heap = MemTag->Owner ;

	DX_HEAP_ENTER_CRITICAL_SECTION( Heap )

	// サイズが０の場合は NULL を返す
	if( Size == 0 )
	{
		DX_HEAP_LEAVE_CRITICAL_SECTION( Heap )
		return NULL ;
	}

	// 確保サイズをアラインの倍数にする
	if( Aligned == 0 )
	{
		Aligned = Heap->Aligned ;
	}
	else
	{
		if( Aligned < ALLOCMEM_MIN_ALIGNED )
		{
			Aligned = ALLOCMEM_MIN_ALIGNED ;
		}
	}
	Size = ( Size + Aligned - 1 ) / Aligned * Aligned ;

	// 確保時と指定されたサイズが同じ場合はファイル名と行番号だけ更新する
	if( MemTag->UserSize == OrigSize )
	{
		AllocMemTag_SetBaseInfo( Heap, MemTag, TRUE, TRUE, Name, Line ) ;

		DX_HEAP_LEAVE_CRITICAL_SECTION( Heap )
		return AllocAddress ;
	}

	// デバッグ領域のチェック
	if( Heap->UseSeparateInfo == FALSE )
	{
		if( AllocMemTag_CheckDebugData( MemTag ) < 0 )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x52\x00\x65\x00\x61\x00\x6c\x00\x6c\x00\x6f\x00\x63\x00\x4d\x00\x65\x00\x6d\x00\x6f\x00\x72\x00\x79\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xc7\x30\xd0\x30\xc3\x30\xb0\x30\x18\x98\xdf\x57\x6e\x30\x34\x78\xca\x58\x92\x30\xba\x78\x8d\x8a\x57\x30\x7e\x30\x57\x30\x5f\x30\x01\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\x0d\x4e\x63\x6b\x6a\x30\xa2\x30\xaf\x30\xbb\x30\xb9\x30\x4c\x30\x7a\x76\x1f\x75\x57\x30\x5f\x30\x53\x30\x68\x30\x4c\x30\x03\x80\x48\x30\x89\x30\x8c\x30\x7e\x30\x59\x30\x00"/*@ L"ReallocMemory Error : デバッグ領域の破壊を確認しました、メモリの不正なアクセスが発生したことが考えられます" @*/ )) ;
			PrintInfoMemory( AllocAddress, UseSeparateInfo ) ;
			*( ( DWORD * )0x00000000 ) = 0xffffffff ;
		}
	}

	// 使用中チェック
	if( ( MemTag->Flag & ALLOCMEMTAG_FLAG_USE ) == 0 )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x52\x00\x65\x00\x61\x00\x6c\x00\x6c\x00\x6f\x00\x63\x00\x4d\x00\x65\x00\x6d\x00\x6f\x00\x72\x00\x79\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xe3\x89\x3e\x65\x08\x6e\x7f\x30\x6e\x30\xe1\x30\xe2\x30\xea\x30\x6b\x30\xfe\x5b\x57\x30\x66\x30\x8d\x51\xba\x78\xdd\x4f\x92\x30\x57\x30\x88\x30\x46\x30\x68\x30\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"ReallocMemory Error : 解放済みのメモリに対して再確保をしようとしました" @*/ )) ;
		PrintInfoMemory( AllocAddress, UseSeparateInfo ) ;
		*( ( DWORD * )0x00000000 ) = 0xffffffff ;
	}

	// 確保メモリが増えるのか減るのかで処理を分岐
	{
		ALLOCMEM_SIZE_TYPE	FreeSize ;
		DWORD_PTR			UseAddress ;
		int					IsRealloc = FALSE ;

		// 使用しているメモリアドレスをセット
		UseAddress = ( DWORD_PTR )MemTag->UserAddress ;

		// 有効な空きメモリサイズを算出
		if( Heap->UseSeparateInfo )
		{
			FreeSize = ( ( DWORD_PTR )MemTag->AllocAddress + MemTag->Size ) - UseAddress ;
		}
		else
		{
			FreeSize = ( ( DWORD_PTR )MemTag + MemTag->Size ) - UseAddress - ALLOCMEMTAG_DEBUGAREA_SIZE ;
		}

		// アラインを適用
		FreeSize = FreeSize / Aligned * Aligned ;

		// 現在の確保メモリに収まらない場合はさらに再確保が必要かチェック
		if( Size > FreeSize )
		{
			// 収まらない場合メモリアドレスの高い方の隣のメモリタグが空きメモリで、且つ増やした分のメモリを格納できるかをチェック

			// 次のメモリタグとメモリアドレスが連続していないか、次のメモリタグが無いか、次のメモリタグが空きメモリではない場合は拡張不可能　
			if( MemTag->Next == NULL || 
				( MemTag->Flag & ALLOCMEMTAG_FLAG_NEXT_CONNECT ) == 0 ||
				( MemTag->Next->Flag & ALLOCMEMTAG_FLAG_USE ) != 0 )
			{
				// 再確保のフラグを立てる
				IsRealloc = TRUE ;
			}
			else
			{
				// 空きメモリのサイズを足した場合、拡張したサイズに収まるかをチェック

				// 有効な空きメモリサイズを算出
				if( Heap->UseSeparateInfo )
				{
					FreeSize = ( ( DWORD_PTR )MemTag->AllocAddress + MemTag->Size + MemTag->Next->Size ) - UseAddress ;
				}
				else
				{
					FreeSize = ( ( DWORD_PTR )MemTag + MemTag->Size + MemTag->Next->Size ) - UseAddress - ALLOCMEMTAG_DEBUGAREA_SIZE ;
				}

				// アラインを適用
				FreeSize = FreeSize / Aligned * Aligned ;

				// 空きメモリが足りない場合は再確保のフラグを立てる
				if( Size > FreeSize )
				{
					IsRealloc = TRUE ;
				}
			}
		}

		// 再確保をする場合と拡張する場合で処理を分岐
		if( IsRealloc )
		{
			// 再確保する場合

			// 新しいメモリ領域を確保
			ReturnAddress = AllocMemory(
				Heap,
				OrigSize,
				Aligned,
				FALSE,
				Name,
				Line
			) ;

			// メモリ確保に失敗した場合は NULL を返す
			if( ReturnAddress == NULL )
			{
				DX_HEAP_LEAVE_CRITICAL_SECTION( Heap )
				return NULL ;
			}

			// 成功した場合は今までの確保メモリの内容をコピーする
			if( Heap->UseSeparateInfo )
			{
				_MEMCPY( ( ( ALLOCMEMTAG * )ReturnAddress )->AllocAddress, MemTag->UserAddress, ( ( DWORD_PTR )MemTag->AllocAddress + MemTag->Size ) - UseAddress ) ;
			}
			else
			{
				_MEMCPY( ReturnAddress, MemTag->UserAddress, ( ( DWORD_PTR )MemTag + MemTag->Size ) - UseAddress - ALLOCMEMTAG_DEBUGAREA_SIZE ) ;
			}

			// 今までのメモリは解放する
			FreeMemory( AllocAddress, Heap->UseSeparateInfo ) ;
		}
		else
		{
			// 拡張する場合は、一旦拡張するメモリタグを解放する( 空きメモリと連結する場合はメモリアドレスが高い場合のみにするフラグを TRUE にして )
			Heap_FreeMemory( Heap, MemTag, TRUE ) ;

			// メモリの再確保
			Heap_AllocMemory(
				Heap,
				MemTag,
				UseAddress,
				OrigSize,
				Size,
				Aligned,
				FALSE,
				Name,
				Line,
				FALSE
			) ;

			// メモリアドレスはそのまま
			ReturnAddress = AllocAddress ;
		}
	}

	DX_HEAP_LEAVE_CRITICAL_SECTION( Heap )

	return ReturnAddress ;
}

// ヒープから確保したメモリを開放する( UseSeparateInfo = TRUE の場合は、AllocAddress に ALLOCMEMTAG 構造体のアドレスを渡す )
extern int FreeMemory( void *AllocAddress, int UseSeparateInfo )
{
	ALLOCMEMTAG *	MemTag ;
	HEAPINFO *		Heap ;

	if( AllocAddress == NULL )
	{
		return 0 ;
	}

	// メモリタグのアドレスを算出
	if( UseSeparateInfo )
	{
		MemTag = ( ALLOCMEMTAG * )AllocAddress ;
	}
	else
	{
		MemTag = ( ALLOCMEMTAG * )( ( BYTE * )AllocAddress - *( ( DWORD * )( ( BYTE * )AllocAddress - sizeof( DWORD ) ) ) ) ;
	}

	// マジックＩＤの確認
	if( MemTag->MagicID != ALLOCMEMTAG_MAGIC_ID )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x72\x00\x65\x00\x65\x00\x4d\x00\x65\x00\x6d\x00\x6f\x00\x72\x00\x79\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xe1\x30\xe2\x30\xea\x30\xbf\x30\xb0\x30\x6e\x30\x20\x00\x4d\x00\x61\x00\x67\x00\x69\x00\x63\x00\x49\x00\x44\x00\x20\x00\x4c\x30\x0d\x4e\x63\x6b\x67\x30\x59\x30\x01\x30\xa4\x8a\x63\x30\x5f\x30\xe1\x30\xe2\x30\xea\x30\xa2\x30\xc9\x30\xec\x30\xb9\x30\x4c\x30\x21\x6e\x55\x30\x8c\x30\x5f\x30\x4b\x30\x01\x30\xe1\x30\xe2\x30\xea\x30\x4c\x30\x34\x78\xca\x58\x55\x30\x8c\x30\x66\x30\x44\x30\x7e\x30\x59\x30\x00"/*@ L"FreeMemory Error : メモリタグの MagicID が不正です、誤ったメモリアドレスが渡されたか、メモリが破壊されています" @*/ )) ;
		*( ( DWORD * )0x00000000 ) = 0xffffffff ;
	}

	// ヒープのアドレスをセット
	Heap = MemTag->Owner ;

	DX_HEAP_ENTER_CRITICAL_SECTION( Heap )

	// デバッグ領域のチェック
	if( Heap->UseSeparateInfo == FALSE )
	{
		if( AllocMemTag_CheckDebugData( MemTag ) < 0 )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x72\x00\x65\x00\x65\x00\x4d\x00\x65\x00\x6d\x00\x6f\x00\x72\x00\x79\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xc7\x30\xd0\x30\xc3\x30\xb0\x30\x18\x98\xdf\x57\x6e\x30\x34\x78\xca\x58\x92\x30\xba\x78\x8d\x8a\x57\x30\x7e\x30\x57\x30\x5f\x30\x01\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\x0d\x4e\x63\x6b\x6a\x30\xa2\x30\xaf\x30\xbb\x30\xb9\x30\x4c\x30\x7a\x76\x1f\x75\x57\x30\x5f\x30\x53\x30\x68\x30\x4c\x30\x03\x80\x48\x30\x89\x30\x8c\x30\x7e\x30\x59\x30\x00"/*@ L"FreeMemory Error : デバッグ領域の破壊を確認しました、メモリの不正なアクセスが発生したことが考えられます" @*/ )) ;
			PrintInfoMemory( AllocAddress, UseSeparateInfo ) ;
			*( ( DWORD * )0x00000000 ) = 0xffffffff ;
		}
	}

	// 使用中チェック
	if( ( MemTag->Flag & ALLOCMEMTAG_FLAG_USE ) == 0 )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x72\x00\x65\x00\x65\x00\x4d\x00\x65\x00\x6d\x00\x6f\x00\x72\x00\x79\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xe3\x89\x3e\x65\x08\x6e\x7f\x30\x6e\x30\xe1\x30\xe2\x30\xea\x30\x6b\x30\xfe\x5b\x57\x30\x66\x30\x8d\x51\xa6\x5e\xe3\x89\x3e\x65\x57\x30\x88\x30\x46\x30\x68\x30\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"FreeMemory Error : 解放済みのメモリに対して再度解放しようとしました" @*/ )) ;
		PrintInfoMemory( AllocAddress, UseSeparateInfo ) ;
		*( ( DWORD * )0x00000000 ) = 0xffffffff ;
	}

	// 確保メモリの初期化
//	_MEMSET( ( BYTE * )MemTag + ALLOCMEMTAG_SIZE + ALLOCMEMTAG_DEBUGAREA_SIZE, 0xff,
//		     ( MemTag->Size - ALLOCMEMTAG_DEBUGAREA_SIZE ) - ( ALLOCMEMTAG_SIZE + ALLOCMEMTAG_DEBUGAREA_SIZE ) ) ;

	// メモリタグを解放する処理を行う
	Heap_FreeMemory( Heap, MemTag, FALSE ) ;

	DX_HEAP_LEAVE_CRITICAL_SECTION( Heap )

	return 0 ;
}

// ヒープから確保したメモリ領域のアドレスを取得する( UseSeparateInfo = TRUE の場合は、AllocAddress に ALLOCMEMTAG 構造体のアドレスを渡す )
extern void * GetAllocMemoryAddress( void *AllocAddress, int UseSeparateInfo )
{
	ALLOCMEMTAG *	MemTag ;

	// メモリタグのアドレスを算出
	if( UseSeparateInfo )
	{
		MemTag = ( ALLOCMEMTAG * )AllocAddress ;
	}
	else
	{
		MemTag = ( ALLOCMEMTAG * )( ( BYTE * )AllocAddress - *( ( DWORD * )( ( BYTE * )AllocAddress - sizeof( DWORD ) ) ) ) ;
	}

	// マジックＩＤの確認
	if( MemTag->MagicID != ALLOCMEMTAG_MAGIC_ID )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x47\x00\x65\x00\x74\x00\x41\x00\x6c\x00\x6c\x00\x6f\x00\x63\x00\x4d\x00\x65\x00\x6d\x00\x6f\x00\x72\x00\x79\x00\x41\x00\x64\x00\x64\x00\x72\x00\x65\x00\x73\x00\x73\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xe1\x30\xe2\x30\xea\x30\xbf\x30\xb0\x30\x6e\x30\x20\x00\x4d\x00\x61\x00\x67\x00\x69\x00\x63\x00\x49\x00\x44\x00\x20\x00\x4c\x30\x0d\x4e\x63\x6b\x67\x30\x59\x30\x01\x30\xa4\x8a\x63\x30\x5f\x30\xe1\x30\xe2\x30\xea\x30\xa2\x30\xc9\x30\xec\x30\xb9\x30\x4c\x30\x21\x6e\x55\x30\x8c\x30\x5f\x30\x4b\x30\x01\x30\xe1\x30\xe2\x30\xea\x30\x4c\x30\x34\x78\xca\x58\x55\x30\x8c\x30\x66\x30\x44\x30\x7e\x30\x59\x30\x00"/*@ L"GetAllocMemoryAddress Error : メモリタグの MagicID が不正です、誤ったメモリアドレスが渡されたか、メモリが破壊されています" @*/ )) ;
		*( ( DWORD * )0x00000000 ) = 0xffffffff ;
	}

	// 使用中チェック
	if( ( MemTag->Flag & ALLOCMEMTAG_FLAG_USE ) == 0 )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x47\x00\x65\x00\x74\x00\x41\x00\x6c\x00\x6c\x00\x6f\x00\x63\x00\x4d\x00\x65\x00\x6d\x00\x6f\x00\x72\x00\x79\x00\x41\x00\x64\x00\x64\x00\x72\x00\x65\x00\x73\x00\x73\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xe3\x89\x3e\x65\x08\x6e\x7f\x30\x6e\x30\xe1\x30\xe2\x30\xea\x30\x6b\x30\xfe\x5b\x57\x30\x66\x30\xe1\x30\xe2\x30\xea\x30\xa2\x30\xc9\x30\xec\x30\xb9\x30\x6e\x30\xd6\x53\x97\x5f\x92\x30\x4c\x88\x4a\x30\x46\x30\x68\x30\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"GetAllocMemoryAddress Error : 解放済みのメモリに対してメモリアドレスの取得を行おうとしました" @*/ )) ;
		PrintInfoMemory( AllocAddress, UseSeparateInfo ) ;
		*( ( DWORD * )0x00000000 ) = 0xffffffff ;
	}

	// アドレスを返す
	return MemTag->UserAddress ;
}

// ヒープから確保したメモリのサイズを取得する( UseSeparateInfo = TRUE の場合は、AllocAddress に ALLOCMEMTAG 構造体のアドレスを渡す )
extern ALLOCMEM_SIZE_TYPE GetAllocSize( void *AllocAddress, int UseSeparateInfo )
{
	ALLOCMEMTAG *	MemTag ;

	// メモリタグのアドレスを算出
	if( UseSeparateInfo )
	{
		MemTag = ( ALLOCMEMTAG * )AllocAddress ;
	}
	else
	{
		MemTag = ( ALLOCMEMTAG * )( ( BYTE * )AllocAddress - *( ( DWORD * )( ( BYTE * )AllocAddress - sizeof( DWORD ) ) ) ) ;
	}

	// マジックＩＤの確認
	if( MemTag->MagicID != ALLOCMEMTAG_MAGIC_ID )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x47\x00\x65\x00\x74\x00\x41\x00\x6c\x00\x6c\x00\x6f\x00\x63\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xe1\x30\xe2\x30\xea\x30\xbf\x30\xb0\x30\x6e\x30\x20\x00\x4d\x00\x61\x00\x67\x00\x69\x00\x63\x00\x49\x00\x44\x00\x20\x00\x4c\x30\x0d\x4e\x63\x6b\x67\x30\x59\x30\x01\x30\xa4\x8a\x63\x30\x5f\x30\xe1\x30\xe2\x30\xea\x30\xa2\x30\xc9\x30\xec\x30\xb9\x30\x4c\x30\x21\x6e\x55\x30\x8c\x30\x5f\x30\x4b\x30\x01\x30\xe1\x30\xe2\x30\xea\x30\x4c\x30\x34\x78\xca\x58\x55\x30\x8c\x30\x66\x30\x44\x30\x7e\x30\x59\x30\x00"/*@ L"GetAllocSize Error : メモリタグの MagicID が不正です、誤ったメモリアドレスが渡されたか、メモリが破壊されています" @*/ )) ;
		*( ( DWORD * )0x00000000 ) = 0xffffffff ;
	}

	// 使用中チェック
	if( ( MemTag->Flag & ALLOCMEMTAG_FLAG_USE ) == 0 )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x47\x00\x65\x00\x74\x00\x41\x00\x6c\x00\x6c\x00\x6f\x00\x63\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xe3\x89\x3e\x65\x08\x6e\x7f\x30\x6e\x30\xe1\x30\xe2\x30\xea\x30\x6b\x30\xfe\x5b\x57\x30\x66\x30\xba\x78\xdd\x4f\xe1\x30\xe2\x30\xea\x30\xb5\x30\xa4\x30\xba\x30\x6e\x30\xd6\x53\x97\x5f\x92\x30\x4c\x88\x4a\x30\x46\x30\x68\x30\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"GetAllocSize Error : 解放済みのメモリに対して確保メモリサイズの取得を行おうとしました" @*/ )) ;
		PrintInfoMemory( AllocAddress, UseSeparateInfo ) ;
		*( ( DWORD * )0x00000000 ) = 0xffffffff ;
	}

	// サイズを返す
	return MemTag->UserSize ;
}

// ヒープから確保したメモリの ALLOCMEMTAG構造体を取得する( UseSeparateInfo = TRUE の場合は、AllocAddress に ALLOCMEMTAG 構造体のアドレスを渡す )
extern ALLOCMEMTAG * GetAllocMemTag( void *AllocAddress, int UseSeparateInfo )
{
	ALLOCMEMTAG *	MemTag ;

	// メモリタグのアドレスを算出
	if( UseSeparateInfo )
	{
		MemTag = ( ALLOCMEMTAG * )AllocAddress ;
	}
	else
	{
		MemTag = ( ALLOCMEMTAG * )( ( BYTE * )AllocAddress - *( ( DWORD * )( ( BYTE * )AllocAddress - sizeof( DWORD ) ) ) ) ;
	}

	// マジックＩＤの確認
	if( MemTag->MagicID != ALLOCMEMTAG_MAGIC_ID )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x47\x00\x65\x00\x74\x00\x41\x00\x6c\x00\x6c\x00\x6f\x00\x63\x00\x4d\x00\x65\x00\x6d\x00\x54\x00\x61\x00\x67\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xe1\x30\xe2\x30\xea\x30\xbf\x30\xb0\x30\x6e\x30\x20\x00\x4d\x00\x61\x00\x67\x00\x69\x00\x63\x00\x49\x00\x44\x00\x20\x00\x4c\x30\x0d\x4e\x63\x6b\x67\x30\x59\x30\x01\x30\xa4\x8a\x63\x30\x5f\x30\xe1\x30\xe2\x30\xea\x30\xa2\x30\xc9\x30\xec\x30\xb9\x30\x4c\x30\x21\x6e\x55\x30\x8c\x30\x5f\x30\x4b\x30\x01\x30\xe1\x30\xe2\x30\xea\x30\x4c\x30\x34\x78\xca\x58\x55\x30\x8c\x30\x66\x30\x44\x30\x7e\x30\x59\x30\x00"/*@ L"GetAllocMemTag Error : メモリタグの MagicID が不正です、誤ったメモリアドレスが渡されたか、メモリが破壊されています" @*/ )) ;
		*( ( DWORD * )0x00000000 ) = 0xffffffff ;
	}

	// 使用中チェック
	if( ( MemTag->Flag & ALLOCMEMTAG_FLAG_USE ) == 0 )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x47\x00\x65\x00\x74\x00\x41\x00\x6c\x00\x6c\x00\x6f\x00\x63\x00\x4d\x00\x65\x00\x6d\x00\x54\x00\x61\x00\x67\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xe3\x89\x3e\x65\x08\x6e\x7f\x30\x6e\x30\xe1\x30\xe2\x30\xea\x30\x6b\x30\xfe\x5b\x57\x30\x66\x30\xba\x78\xdd\x4f\xe1\x30\xe2\x30\xea\x30\xb5\x30\xa4\x30\xba\x30\x6e\x30\xd6\x53\x97\x5f\x92\x30\x4c\x88\x4a\x30\x46\x30\x68\x30\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"GetAllocMemTag Error : 解放済みのメモリに対して確保メモリサイズの取得を行おうとしました" @*/ )) ;
		PrintInfoMemory( AllocAddress, UseSeparateInfo ) ;
		*( ( DWORD * )0x00000000 ) = 0xffffffff ;
	}

	// タグのアドレスを返す
	return MemTag ;
}

// ヒープから確保したメモリの情報をログに出力する( UseSeparateInfo = TRUE の場合は、AllocAddress に ALLOCMEMTAG 構造体のアドレスを渡す )
extern void PrintInfoMemory( void *AllocAddress, int UseSeparateInfo )
{
#ifndef DX_NON_LITERAL_STRING
	ALLOCMEMTAG *	MemTag ;
	char			str[ 512 ] ;

	// メモリタグのアドレスを算出
	if( UseSeparateInfo )
	{
		MemTag = ( ALLOCMEMTAG * )AllocAddress ;
	}
	else
	{
		MemTag = ( ALLOCMEMTAG * )( ( BYTE * )AllocAddress - *( ( DWORD * )( ( BYTE * )AllocAddress - sizeof( DWORD ) ) ) ) ;
	}

	// ダンプに出力しないフラグが足っていたら何もせず終了
	if( MemTag->Flag & ALLOCMEMTAG_FLAG_NO_DUMP )
	{
		return ;
	}

	CL_snprintf( DX_CHARCODEFORMAT_ASCII, FALSE, DX_CHARCODEFORMAT_ASCII, WCHAR_T_CHARCODEFORMAT, str, sizeof( str ),
#ifdef __64BIT__
		"\tsize:%10d(%10.3fkb)  user size:%10d(%10.3fkb)  time:%05d  file:%-26s  line:%-6d  ID:%-5d  addr:%016llx",
#else // __64BIT__
		"\tsize:%10d(%10.3fkb)  user size:%10d(%10.3fkb)  time:%05d  file:%-26s  line:%-6d  ID:%-5d  addr:%08x",
#endif // __64BIT__
		MemTag->Size,
		MemTag->Size / 1024.0f,
		MemTag->UserSize,
		MemTag->UserSize / 1024.0f,
		( int )MemTag->Time,
		MemTag->Name,
		MemTag->Line + ( ( MemTag->Flag & ( ALLOCMEMTAG_FLAG_LINE_BIT_16 | ALLOCMEMTAG_FLAG_LINE_BIT_17 ) ) << 16 ),
		( int )( ( MemTag->IDHigh << 16 ) | MemTag->IDLow ),
		( DWORD_PTR )MemTag->UserAddress
	) ;

	{
		char *	p ;
		BYTE *	d ;
		size_t	len ;
		size_t	i ;

//		CL_strcpy( DX_CHARCODEFORMAT_ASCII, str + CL_strlen( DX_CHARCODEFORMAT_ASCII, str ), "  data:<" ) ;
		len = 16 < MemTag->UserSize ? 16 : MemTag->UserSize ;
		p = str + CL_strlen( DX_CHARCODEFORMAT_ASCII, str ) ;
		d = ( BYTE * )MemTag->UserAddress ;
//		for( i = 0 ; i < len ; i ++, p ++ )
//		{
//			*p = d[ i ] < 0x20 ? '.' : d[ i ] ;
//		}
//		CL_strcpy( DX_CHARCODEFORMAT_ASCII, p, "> [" ) ;
//		p += 3 ;

		CL_strcpy( DX_CHARCODEFORMAT_ASCII, p, "  data:[" ) ;
		p += 8 ;
		for( i = 0 ; i < len ; i ++, p += 3, d ++ )
		{
			CL_sprintf( DX_CHARCODEFORMAT_ASCII, FALSE, DX_CHARCODEFORMAT_ASCII, WCHAR_T_CHARCODEFORMAT, p, "%02x ", *( ( unsigned char * ) d ) ) ;
		}
		CL_strcpy( DX_CHARCODEFORMAT_ASCII, &p[ -1 ], "]\n" ) ;
	}

	DXST_LOGFILE_ADDA( str ) ;

#endif // DX_NON_LITERAL_STRING
}

// ヒープの情報をログに出力する
extern int HeapInfoDump( HEAPINFO *Heap )
{
	ALLOCMEMTAG *MemTag ;

	DX_HEAP_ENTER_CRITICAL_SECTION( Heap )

	// 全ての確保メモリタグの情報を出力
	for( MemTag = Heap->UseFirstTag ; MemTag != NULL ; MemTag = MemTag->ListNext )
	{
		PrintInfoMemory( Heap->UseSeparateInfo ? MemTag : MemTag->UserAddress, Heap->UseSeparateInfo ) ;
	}

	DX_HEAP_LEAVE_CRITICAL_SECTION( Heap )

	return 0 ;
}

// ヒープから確保したメモリに設定する時間を変更する
extern int SetHeapTime( HEAPINFO *Heap, WORD Time )
{
	Heap->AllocMemoryTime = Time ;

	return 0 ;
}

// ヒープの内容にエラーがないかチェックする
extern int HeapErrorCheck( HEAPINFO *Heap )
{
	ALLOCMEMTAG *MemTag ;

	DX_HEAP_ENTER_CRITICAL_SECTION( Heap )

	// メモリ使用領域とマジックIDとデバッグ領域のチェック
	for( MemTag = Heap->FirstTag ; MemTag != NULL ; MemTag = MemTag->Next )
	{
		if( MemTag->MagicID != ALLOCMEMTAG_MAGIC_ID )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x48\x00\x65\x00\x61\x00\x70\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x43\x00\x68\x00\x65\x00\x63\x00\x6b\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xe1\x30\xe2\x30\xea\x30\xbf\x30\xb0\x30\x6e\x30\x20\x00\x4d\x00\x61\x00\x67\x00\x69\x00\x63\x00\x49\x00\x44\x00\x20\x00\x4c\x30\x0d\x4e\x63\x6b\x67\x30\x59\x30\x01\x30\xa4\x8a\x63\x30\x5f\x30\xe1\x30\xe2\x30\xea\x30\xa2\x30\xc9\x30\xec\x30\xb9\x30\x4c\x30\x21\x6e\x55\x30\x8c\x30\x5f\x30\x4b\x30\x01\x30\xe1\x30\xe2\x30\xea\x30\x4c\x30\x34\x78\xca\x58\x55\x30\x8c\x30\x66\x30\x44\x30\x7e\x30\x59\x30\x00"/*@ L"HeapErrorCheck Error : メモリタグの MagicID が不正です、誤ったメモリアドレスが渡されたか、メモリが破壊されています" @*/ )) ;
			*( ( DWORD * )0x00000000 ) = 0xffffffff ;
		}

		if( Heap->UseSeparateInfo )
		{
			if( ( DWORD_PTR )MemTag->AllocAddress <  ( DWORD_PTR )MemTag->UseHeapMem->HeapAddress ||
				( DWORD_PTR )MemTag->AllocAddress >= ( DWORD_PTR )MemTag->UseHeapMem->HeapAddress + MemTag->UseHeapMem->HeapSize )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x48\x00\x65\x00\x61\x00\x70\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x43\x00\x68\x00\x65\x00\x63\x00\x6b\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xe1\x30\xe2\x30\xea\x30\xbf\x30\xb0\x30\x4c\x30\xba\x78\xdd\x4f\x57\x30\x5f\x30\xd2\x30\xfc\x30\xd7\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xc4\x7b\xf2\x56\x6b\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x20\x00\x20\x00\x48\x00\x65\x00\x61\x00\x70\x00\x2d\x00\x3e\x00\x55\x00\x73\x00\x65\x00\x53\x00\x65\x00\x70\x00\x61\x00\x72\x00\x61\x00\x74\x00\x65\x00\x49\x00\x6e\x00\x66\x00\x6f\x00\x20\x00\x3d\x00\x20\x00\x54\x00\x52\x00\x55\x00\x45\x00\x00"/*@ L"HeapErrorCheck Error : メモリタグが確保したヒープメモリの範囲にありません  Heap->UseSeparateInfo = TRUE" @*/ )) ;
				PrintInfoMemory( Heap->UseSeparateInfo ? MemTag : MemTag->UserAddress, Heap->UseSeparateInfo ) ;
				*( ( DWORD * )0x00000000 ) = 0xffffffff ;
			}
		}
		else
		{
			if( ( DWORD_PTR )MemTag <  ( DWORD_PTR )MemTag->UseHeapMem->HeapAddress ||
				( DWORD_PTR )MemTag >= ( DWORD_PTR )MemTag->UseHeapMem->HeapAddress + MemTag->UseHeapMem->HeapSize )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x48\x00\x65\x00\x61\x00\x70\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x43\x00\x68\x00\x65\x00\x63\x00\x6b\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xe1\x30\xe2\x30\xea\x30\xbf\x30\xb0\x30\x4c\x30\xba\x78\xdd\x4f\x57\x30\x5f\x30\xd2\x30\xfc\x30\xd7\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xc4\x7b\xf2\x56\x6b\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x20\x00\x20\x00\x48\x00\x65\x00\x61\x00\x70\x00\x2d\x00\x3e\x00\x55\x00\x73\x00\x65\x00\x53\x00\x65\x00\x70\x00\x61\x00\x72\x00\x61\x00\x74\x00\x65\x00\x49\x00\x6e\x00\x66\x00\x6f\x00\x20\x00\x3d\x00\x20\x00\x46\x00\x41\x00\x4c\x00\x53\x00\x45\x00\x00"/*@ L"HeapErrorCheck Error : メモリタグが確保したヒープメモリの範囲にありません  Heap->UseSeparateInfo = FALSE" @*/ )) ;
				PrintInfoMemory( Heap->UseSeparateInfo ? MemTag : MemTag->UserAddress, Heap->UseSeparateInfo ) ;
				*( ( DWORD * )0x00000000 ) = 0xffffffff ;
			}

			if( AllocMemTag_CheckDebugData( MemTag ) < 0 )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x48\x00\x65\x00\x61\x00\x70\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x43\x00\x68\x00\x65\x00\x63\x00\x6b\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xc7\x30\xd0\x30\xc3\x30\xb0\x30\x18\x98\xdf\x57\x6e\x30\x34\x78\xca\x58\x92\x30\xba\x78\x8d\x8a\x57\x30\x7e\x30\x57\x30\x5f\x30\x01\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\x0d\x4e\x63\x6b\x6a\x30\xa2\x30\xaf\x30\xbb\x30\xb9\x30\x4c\x30\x7a\x76\x1f\x75\x57\x30\x5f\x30\x53\x30\x68\x30\x4c\x30\x03\x80\x48\x30\x89\x30\x8c\x30\x7e\x30\x59\x30\x00"/*@ L"HeapErrorCheck Error : デバッグ領域の破壊を確認しました、メモリの不正なアクセスが発生したことが考えられます" @*/ )) ;
				PrintInfoMemory( Heap->UseSeparateInfo ? MemTag : MemTag->UserAddress, Heap->UseSeparateInfo ) ;
				*( ( DWORD * )0x00000000 ) = 0xffffffff ;
			}
		}
	}

	// アラインチェック
	{
		if( Heap->UseSeparateInfo )
		{
			for( MemTag = Heap->FirstTag ; MemTag != NULL ; MemTag = MemTag->Next )
			{
				if( ( DWORD_PTR )MemTag->AllocAddress % ALLOCMEM_MIN_ALIGNED != 0 )
				{
					DXST_LOGFILEFMT_ADDUTF16LE(( "\x48\x00\x65\x00\x61\x00\x70\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x43\x00\x68\x00\x65\x00\x63\x00\x6b\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xe1\x30\xe2\x30\xea\x30\xbf\x30\xb0\x30\x6e\x30\xa2\x30\xe9\x30\xa4\x30\xf3\x30\x4c\x30\x0d\x4e\x63\x6b\x67\x30\x59\x30\x20\x00\x20\x00\x48\x00\x65\x00\x61\x00\x70\x00\x2d\x00\x3e\x00\x55\x00\x73\x00\x65\x00\x53\x00\x65\x00\x70\x00\x61\x00\x72\x00\x61\x00\x74\x00\x65\x00\x49\x00\x6e\x00\x66\x00\x6f\x00\x20\x00\x3d\x00\x20\x00\x54\x00\x52\x00\x55\x00\x45\x00\x00"/*@ L"HeapErrorCheck Error : メモリタグのアラインが不正です  Heap->UseSeparateInfo = TRUE" @*/ )) ;
					PrintInfoMemory( Heap->UseSeparateInfo ? MemTag : MemTag->UserAddress, Heap->UseSeparateInfo ) ;
					*( ( DWORD * )0x00000000 ) = 0xffffffff ;
				}
			}
		}
		else
		{
			for( MemTag = Heap->FirstTag ; MemTag != NULL ; MemTag = MemTag->Next )
			{
				if( ( DWORD_PTR )MemTag % ALLOCMEM_MIN_ALIGNED != 0 )
				{
					DXST_LOGFILEFMT_ADDUTF16LE(( "\x48\x00\x65\x00\x61\x00\x70\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x43\x00\x68\x00\x65\x00\x63\x00\x6b\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xe1\x30\xe2\x30\xea\x30\xbf\x30\xb0\x30\x6e\x30\xa2\x30\xe9\x30\xa4\x30\xf3\x30\x4c\x30\x0d\x4e\x63\x6b\x67\x30\x59\x30\x20\x00\x20\x00\x48\x00\x65\x00\x61\x00\x70\x00\x2d\x00\x3e\x00\x55\x00\x73\x00\x65\x00\x53\x00\x65\x00\x70\x00\x61\x00\x72\x00\x61\x00\x74\x00\x65\x00\x49\x00\x6e\x00\x66\x00\x6f\x00\x20\x00\x3d\x00\x20\x00\x46\x00\x41\x00\x4c\x00\x53\x00\x45\x00\x00"/*@ L"HeapErrorCheck Error : メモリタグのアラインが不正です  Heap->UseSeparateInfo = FALSE" @*/ )) ;
					PrintInfoMemory( Heap->UseSeparateInfo ? MemTag : MemTag->UserAddress, Heap->UseSeparateInfo ) ;
					*( ( DWORD * )0x00000000 ) = 0xffffffff ;
				}
			}
		}
	}

	// 総メモリサイズのチェック
	{
		ALLOCMEM_SIZE_TYPE TotalSize ;
		ALLOCMEM_SIZE_TYPE TotalSize2 ;
		HEAPMEMINFO *HeapMem ;

		TotalSize = 0 ;
		for( MemTag = Heap->FirstTag ; MemTag != NULL ; MemTag = MemTag->Next )
		{
			TotalSize += MemTag->Size ;
		}

		TotalSize2 = 0 ;
		for( HeapMem = Heap->FirstHeapMem ; HeapMem != NULL ; HeapMem = HeapMem->Next )
		{
			TotalSize2 += HeapMem->HeapSize ;
		}
		if( TotalSize != Heap->TotalUseSize + Heap->TotalFreeSize )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x48\x00\x65\x00\x61\x00\x70\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x43\x00\x68\x00\x65\x00\x63\x00\x6b\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xcf\x7d\xe1\x30\xe2\x30\xea\x30\xb5\x30\xa4\x30\xba\x30\x4c\x30\x00\x4e\xf4\x81\x57\x30\x7e\x30\x5b\x30\x93\x30\x20\x00\x20\x00\x54\x00\x6f\x00\x74\x00\x61\x00\x6c\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x3a\x00\x25\x00\x6c\x00\x6c\x00\x64\x00\x20\x00\x20\x00\x20\x00\x28\x00\x20\x00\x48\x00\x65\x00\x61\x00\x70\x00\x2d\x00\x3e\x00\x54\x00\x6f\x00\x74\x00\x61\x00\x6c\x00\x55\x00\x73\x00\x65\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x20\x00\x2b\x00\x20\x00\x48\x00\x65\x00\x61\x00\x70\x00\x2d\x00\x3e\x00\x54\x00\x6f\x00\x74\x00\x61\x00\x6c\x00\x46\x00\x72\x00\x65\x00\x65\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x20\x00\x29\x00\x3a\x00\x25\x00\x6c\x00\x6c\x00\x64\x00\x00"/*@ L"HeapErrorCheck Error : 総メモリサイズが一致しません  TotalSize:%lld   ( Heap->TotalUseSize + Heap->TotalFreeSize ):%lld" @*/, ( ULONGLONG )TotalSize, ( ULONGLONG )( Heap->TotalUseSize + Heap->TotalFreeSize ) )) ;
			*( ( DWORD * )0x00000000 ) = 0xffffffff ;
		}
		if( TotalSize != TotalSize2 )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x48\x00\x65\x00\x61\x00\x70\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x43\x00\x68\x00\x65\x00\x63\x00\x6b\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xcf\x7d\xe1\x30\xe2\x30\xea\x30\xb5\x30\xa4\x30\xba\x30\x4c\x30\x00\x4e\xf4\x81\x57\x30\x7e\x30\x5b\x30\x93\x30\x20\x00\x20\x00\x54\x00\x6f\x00\x74\x00\x61\x00\x6c\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x3a\x00\x25\x00\x6c\x00\x6c\x00\x64\x00\x20\x00\x20\x00\x20\x00\x54\x00\x6f\x00\x74\x00\x61\x00\x6c\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x32\x00\x3a\x00\x25\x00\x6c\x00\x6c\x00\x64\x00\x00"/*@ L"HeapErrorCheck Error : 総メモリサイズが一致しません  TotalSize:%lld   TotalSize2:%lld" @*/, ( ULONGLONG )TotalSize, ( ULONGLONG )TotalSize2 )) ;
			*( ( DWORD * )0x00000000 ) = 0xffffffff ;
		}
	}

	// メモリ領域の前後関係のチェック
	{
		if( Heap->UseSeparateInfo )
		{
			for( MemTag = Heap->FirstTag ; MemTag != NULL ; MemTag = MemTag->Next )
			{
				if( MemTag->Flag & ALLOCMEMTAG_FLAG_PREV_CONNECT )
				{
					if( ( DWORD_PTR )MemTag->Prev->AllocAddress + MemTag->Prev->Size != ( DWORD_PTR )MemTag->AllocAddress )
					{
						DXST_LOGFILEFMT_ADDUTF16LE(( "\x48\x00\x65\x00\x61\x00\x70\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x43\x00\x68\x00\x65\x00\x63\x00\x6b\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xe1\x30\xe2\x30\xea\x30\xbf\x30\xb0\x30\x6e\x30\x23\x90\x50\x7d\xc5\x60\x31\x58\x4c\x30\x0d\x4e\x63\x6b\x67\x30\x59\x30\x20\x00\x20\x00\x28\x00\x20\x00\x4d\x00\x65\x00\x6d\x00\x54\x00\x61\x00\x67\x00\x2d\x00\x3e\x00\x50\x00\x72\x00\x65\x00\x76\x00\x2d\x00\x3e\x00\x41\x00\x6c\x00\x6c\x00\x6f\x00\x63\x00\x41\x00\x64\x00\x64\x00\x72\x00\x65\x00\x73\x00\x73\x00\x20\x00\x2b\x00\x20\x00\x4d\x00\x65\x00\x6d\x00\x54\x00\x61\x00\x67\x00\x2d\x00\x3e\x00\x50\x00\x72\x00\x65\x00\x76\x00\x2d\x00\x3e\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x20\x00\x29\x00\x3a\x00\x30\x00\x78\x00\x25\x00\x6c\x00\x6c\x00\x30\x00\x31\x00\x36\x00\x78\x00\x20\x00\x20\x00\x4d\x00\x65\x00\x6d\x00\x54\x00\x61\x00\x67\x00\x2d\x00\x3e\x00\x41\x00\x6c\x00\x6c\x00\x6f\x00\x63\x00\x41\x00\x64\x00\x64\x00\x72\x00\x65\x00\x73\x00\x73\x00\x3a\x00\x30\x00\x78\x00\x25\x00\x6c\x00\x6c\x00\x30\x00\x31\x00\x36\x00\x78\x00\x00"/*@ L"HeapErrorCheck Error : メモリタグの連結情報が不正です  ( MemTag->Prev->AllocAddress + MemTag->Prev->Size ):0x%ll016x  MemTag->AllocAddress:0x%ll016x" @*/, ( ULONGLONG )( ( DWORD_PTR )MemTag->Prev->AllocAddress + MemTag->Prev->Size ), ( ULONGLONG )MemTag->AllocAddress )) ;
						PrintInfoMemory( Heap->UseSeparateInfo ? MemTag : MemTag->UserAddress, Heap->UseSeparateInfo ) ;
						*( ( DWORD * )0x00000000 ) = 0xffffffff ;
					}
				}

				if( MemTag->Flag & ALLOCMEMTAG_FLAG_NEXT_CONNECT )
				{
					if( ( DWORD_PTR )MemTag->Next->AllocAddress != ( DWORD_PTR )MemTag->AllocAddress + MemTag->Size )
					{
						DXST_LOGFILEFMT_ADDUTF16LE(( "\x48\x00\x65\x00\x61\x00\x70\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x43\x00\x68\x00\x65\x00\x63\x00\x6b\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xe1\x30\xe2\x30\xea\x30\xbf\x30\xb0\x30\x6e\x30\x23\x90\x50\x7d\xc5\x60\x31\x58\x4c\x30\x0d\x4e\x63\x6b\x67\x30\x59\x30\x20\x00\x20\x00\x4d\x00\x65\x00\x6d\x00\x54\x00\x61\x00\x67\x00\x2d\x00\x3e\x00\x4e\x00\x65\x00\x78\x00\x74\x00\x2d\x00\x3e\x00\x41\x00\x6c\x00\x6c\x00\x6f\x00\x63\x00\x41\x00\x64\x00\x64\x00\x72\x00\x65\x00\x73\x00\x73\x00\x3a\x00\x30\x00\x78\x00\x25\x00\x6c\x00\x6c\x00\x30\x00\x31\x00\x36\x00\x78\x00\x20\x00\x20\x00\x28\x00\x20\x00\x4d\x00\x65\x00\x6d\x00\x54\x00\x61\x00\x67\x00\x2d\x00\x3e\x00\x41\x00\x6c\x00\x6c\x00\x6f\x00\x63\x00\x41\x00\x64\x00\x64\x00\x72\x00\x65\x00\x73\x00\x73\x00\x20\x00\x2b\x00\x20\x00\x4d\x00\x65\x00\x6d\x00\x54\x00\x61\x00\x67\x00\x2d\x00\x3e\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x20\x00\x29\x00\x3a\x00\x30\x00\x78\x00\x25\x00\x6c\x00\x6c\x00\x30\x00\x31\x00\x36\x00\x78\x00\x00"/*@ L"HeapErrorCheck Error : メモリタグの連結情報が不正です  MemTag->Next->AllocAddress:0x%ll016x  ( MemTag->AllocAddress + MemTag->Size ):0x%ll016x" @*/, ( ULONGLONG )MemTag->Next->AllocAddress, ( ULONGLONG )( ( DWORD_PTR )MemTag->AllocAddress + MemTag->Size ) )) ;
						*( ( DWORD * )0x00000000 ) = 0xffffffff ;
					}
				}
			}
		}
		else
		{
			for( MemTag = Heap->FirstTag ; MemTag != NULL ; MemTag = MemTag->Next )
			{
				if( MemTag->Flag & ALLOCMEMTAG_FLAG_PREV_CONNECT )
				{
					if( ( DWORD_PTR )MemTag->Prev + MemTag->Prev->Size != ( DWORD_PTR )MemTag )
					{
						DXST_LOGFILEFMT_ADDUTF16LE(( "\x48\x00\x65\x00\x61\x00\x70\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x43\x00\x68\x00\x65\x00\x63\x00\x6b\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xe1\x30\xe2\x30\xea\x30\xbf\x30\xb0\x30\x6e\x30\x23\x90\x50\x7d\xc5\x60\x31\x58\x4c\x30\x0d\x4e\x63\x6b\x67\x30\x59\x30\x20\x00\x20\x00\x28\x00\x20\x00\x4d\x00\x65\x00\x6d\x00\x54\x00\x61\x00\x67\x00\x2d\x00\x3e\x00\x50\x00\x72\x00\x65\x00\x76\x00\x20\x00\x2b\x00\x20\x00\x4d\x00\x65\x00\x6d\x00\x54\x00\x61\x00\x67\x00\x2d\x00\x3e\x00\x50\x00\x72\x00\x65\x00\x76\x00\x2d\x00\x3e\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x20\x00\x29\x00\x3a\x00\x30\x00\x78\x00\x25\x00\x6c\x00\x6c\x00\x30\x00\x31\x00\x36\x00\x78\x00\x20\x00\x20\x00\x4d\x00\x65\x00\x6d\x00\x54\x00\x61\x00\x67\x00\x3a\x00\x30\x00\x78\x00\x25\x00\x6c\x00\x6c\x00\x30\x00\x31\x00\x36\x00\x78\x00\x00"/*@ L"HeapErrorCheck Error : メモリタグの連結情報が不正です  ( MemTag->Prev + MemTag->Prev->Size ):0x%ll016x  MemTag:0x%ll016x" @*/, ( ULONGLONG )( MemTag->Prev + MemTag->Prev->Size ), ( ULONGLONG )MemTag )) ;
						PrintInfoMemory( Heap->UseSeparateInfo ? MemTag : MemTag->UserAddress, Heap->UseSeparateInfo ) ;
						*( ( DWORD * )0x00000000 ) = 0xffffffff ;
					}
				}

				if( MemTag->Flag & ALLOCMEMTAG_FLAG_NEXT_CONNECT )
				{
					if( ( DWORD_PTR )MemTag->Next != ( DWORD_PTR )MemTag + MemTag->Size )
					{
						DXST_LOGFILEFMT_ADDUTF16LE(( "\x48\x00\x65\x00\x61\x00\x70\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x43\x00\x68\x00\x65\x00\x63\x00\x6b\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xe1\x30\xe2\x30\xea\x30\xbf\x30\xb0\x30\x6e\x30\x23\x90\x50\x7d\xc5\x60\x31\x58\x4c\x30\x0d\x4e\x63\x6b\x67\x30\x59\x30\x20\x00\x20\x00\x4d\x00\x65\x00\x6d\x00\x54\x00\x61\x00\x67\x00\x2d\x00\x3e\x00\x4e\x00\x65\x00\x78\x00\x74\x00\x3a\x00\x30\x00\x78\x00\x25\x00\x6c\x00\x6c\x00\x30\x00\x31\x00\x36\x00\x78\x00\x20\x00\x20\x00\x28\x00\x20\x00\x4d\x00\x65\x00\x6d\x00\x54\x00\x61\x00\x67\x00\x20\x00\x2b\x00\x20\x00\x4d\x00\x65\x00\x6d\x00\x54\x00\x61\x00\x67\x00\x2d\x00\x3e\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x20\x00\x29\x00\x3a\x00\x30\x00\x78\x00\x25\x00\x6c\x00\x6c\x00\x30\x00\x31\x00\x36\x00\x78\x00\x00"/*@ L"HeapErrorCheck Error : メモリタグの連結情報が不正です  MemTag->Next:0x%ll016x  ( MemTag + MemTag->Size ):0x%ll016x" @*/, ( ULONGLONG )MemTag->Next, ( ULONGLONG )( MemTag + MemTag->Size ) )) ;
						PrintInfoMemory( Heap->UseSeparateInfo ? MemTag : MemTag->UserAddress, Heap->UseSeparateInfo ) ;
						*( ( DWORD * )0x00000000 ) = 0xffffffff ;
					}
				}
			}
		}
	}

	// 空きメモリタグが連続してしまっていないかチェック
	{
		for( MemTag = Heap->FirstTag ; MemTag != NULL ; MemTag = MemTag->Next )
		{
			if( MemTag->Flag & ALLOCMEMTAG_FLAG_USE )
			{
				continue ;
			}

			if( MemTag->Flag & ALLOCMEMTAG_FLAG_PREV_CONNECT )
			{
				if( ( MemTag->Prev->Flag & ALLOCMEMTAG_FLAG_USE ) == 0 )
				{
					DXST_LOGFILEFMT_ADDUTF16LE(( "\x48\x00\x65\x00\x61\x00\x70\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x43\x00\x68\x00\x65\x00\x63\x00\x6b\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x7a\x7a\x4d\x30\xe1\x30\xe2\x30\xea\x30\xbf\x30\xb0\x30\x4c\x30\x0b\x4e\x4d\x4f\xe1\x30\xe2\x30\xea\x30\xa2\x30\xc9\x30\xec\x30\xb9\x30\xb9\x65\x11\x54\x6b\x30\x23\x90\x9a\x7d\x57\x30\x66\x30\x44\x30\x7e\x30\x59\x30\x00"/*@ L"HeapErrorCheck Error : 空きメモリタグが下位メモリアドレス方向に連続しています" @*/ )) ;
					PrintInfoMemory( Heap->UseSeparateInfo ? MemTag : MemTag->UserAddress, Heap->UseSeparateInfo ) ;
					*( ( DWORD * )0x00000000 ) = 0xffffffff ;
				}
			}

			if( MemTag->Flag & ALLOCMEMTAG_FLAG_NEXT_CONNECT )
			{
				if( ( MemTag->Next->Flag & ALLOCMEMTAG_FLAG_USE ) == 0 )
				{
					DXST_LOGFILEFMT_ADDUTF16LE(( "\x48\x00\x65\x00\x61\x00\x70\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x43\x00\x68\x00\x65\x00\x63\x00\x6b\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x7a\x7a\x4d\x30\xe1\x30\xe2\x30\xea\x30\xbf\x30\xb0\x30\x4c\x30\x0a\x4e\x4d\x4f\xe1\x30\xe2\x30\xea\x30\xa2\x30\xc9\x30\xec\x30\xb9\x30\xb9\x65\x11\x54\x6b\x30\x23\x90\x9a\x7d\x57\x30\x66\x30\x44\x30\x7e\x30\x59\x30\x00"/*@ L"HeapErrorCheck Error : 空きメモリタグが上位メモリアドレス方向に連続しています" @*/ )) ;
					PrintInfoMemory( Heap->UseSeparateInfo ? MemTag : MemTag->UserAddress, Heap->UseSeparateInfo ) ;
					*( ( DWORD * )0x00000000 ) = 0xffffffff ;
				}
			}
		}
	}

	// 未使用リストのチェック
	if( Heap->UseSeparateInfo )
	{
		// 未使用タグの数をチェック
		{
			int TagNum ;

			for( MemTag = Heap->SepTagArrayNotUseFirst, TagNum = 0 ; MemTag != NULL ; MemTag = MemTag->ListNext, TagNum ++ ){}
			if( TagNum != Heap->SepTagArrayLength - Heap->SepTagArrayUseNum )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x48\x00\x65\x00\x61\x00\x70\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x43\x00\x68\x00\x65\x00\x63\x00\x6b\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xbf\x30\xb0\x30\x4d\x91\x17\x52\x06\x52\xe2\x96\xbf\x30\xa4\x30\xd7\x30\x6e\x30\x2a\x67\x7f\x4f\x28\x75\xbf\x30\xb0\x30\x6e\x30\x70\x65\x4c\x30\x00\x4e\xf4\x81\x57\x30\x7e\x30\x5b\x30\x93\x30\x20\x00\x20\x00\x54\x00\x61\x00\x67\x00\x4e\x00\x75\x00\x6d\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x20\x00\x28\x00\x20\x00\x48\x00\x65\x00\x61\x00\x70\x00\x2d\x00\x3e\x00\x53\x00\x65\x00\x70\x00\x54\x00\x61\x00\x67\x00\x41\x00\x72\x00\x72\x00\x61\x00\x79\x00\x4c\x00\x65\x00\x6e\x00\x67\x00\x74\x00\x68\x00\x20\x00\x2d\x00\x20\x00\x48\x00\x65\x00\x61\x00\x70\x00\x2d\x00\x3e\x00\x53\x00\x65\x00\x70\x00\x54\x00\x61\x00\x67\x00\x41\x00\x72\x00\x72\x00\x61\x00\x79\x00\x55\x00\x73\x00\x65\x00\x4e\x00\x75\x00\x6d\x00\x20\x00\x29\x00\x3a\x00\x25\x00\x64\x00\x00"/*@ L"HeapErrorCheck Error : タグ配列分離タイプの未使用タグの数が一致しません  TagNum:%d  ( Heap->SepTagArrayLength - Heap->SepTagArrayUseNum ):%d" @*/, TagNum, Heap->SepTagArrayLength - Heap->SepTagArrayUseNum )) ;
				*( ( DWORD * )0x00000000 ) = 0xffffffff ;
			}
		}

		// 未使用メモリヒープの数をチェック
		{
			int HeapMemNum ;
			HEAPMEMINFO *HeapMem ;

			for( HeapMem = Heap->SepHeapMemArrayNotUseFirst, HeapMemNum = 0 ; HeapMem != NULL ; HeapMem = HeapMem->Next, HeapMemNum ++ ){}
			if( HeapMemNum != Heap->SepHeapMemArrayLength - Heap->SepHeapMemArrayUseNum )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x48\x00\x65\x00\x61\x00\x70\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x43\x00\x68\x00\x65\x00\x63\x00\x6b\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xd2\x30\xfc\x30\xd7\x30\xe1\x30\xe2\x30\xea\x30\xc5\x60\x31\x58\x06\x52\xe2\x96\xbf\x30\xa4\x30\xd7\x30\x6e\x30\x2a\x67\x7f\x4f\x28\x75\xc5\x60\x31\x58\x6e\x30\x70\x65\x4c\x30\x00\x4e\xf4\x81\x57\x30\x7e\x30\x5b\x30\x93\x30\x20\x00\x20\x00\x48\x00\x65\x00\x61\x00\x70\x00\x4d\x00\x65\x00\x6d\x00\x4e\x00\x75\x00\x6d\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x20\x00\x28\x00\x20\x00\x48\x00\x65\x00\x61\x00\x70\x00\x2d\x00\x3e\x00\x53\x00\x65\x00\x70\x00\x48\x00\x65\x00\x61\x00\x70\x00\x4d\x00\x65\x00\x6d\x00\x41\x00\x72\x00\x72\x00\x61\x00\x79\x00\x4c\x00\x65\x00\x6e\x00\x67\x00\x74\x00\x68\x00\x20\x00\x2d\x00\x20\x00\x48\x00\x65\x00\x61\x00\x70\x00\x2d\x00\x3e\x00\x53\x00\x65\x00\x70\x00\x48\x00\x65\x00\x61\x00\x70\x00\x4d\x00\x65\x00\x6d\x00\x41\x00\x72\x00\x72\x00\x61\x00\x79\x00\x55\x00\x73\x00\x65\x00\x4e\x00\x75\x00\x6d\x00\x20\x00\x29\x00\x3a\x00\x25\x00\x64\x00\x00"/*@ L"HeapErrorCheck Error : ヒープメモリ情報分離タイプの未使用情報の数が一致しません  HeapMemNum:%d  ( Heap->SepHeapMemArrayLength - Heap->SepHeapMemArrayUseNum ):%d" @*/, HeapMemNum, Heap->SepHeapMemArrayLength - Heap->SepHeapMemArrayUseNum )) ;
				*( ( DWORD * )0x00000000 ) = 0xffffffff ;
			}
		}
	}

	// 総使用メモリサイズのチェック
	{
		ALLOCMEM_SIZE_TYPE TotalUseSize ;
		int UseNum ;

		TotalUseSize = 0 ;
		UseNum = 0 ;
		for( MemTag = Heap->UseFirstTag ; MemTag != NULL ; MemTag = MemTag->ListNext, UseNum ++ )
		{
			if( ( MemTag->Flag & ALLOCMEMTAG_FLAG_USE ) == 0 )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x48\x00\x65\x00\x61\x00\x70\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x43\x00\x68\x00\x65\x00\x63\x00\x6b\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x7f\x4f\x28\x75\xe1\x30\xe2\x30\xea\x30\xbf\x30\xb0\x30\x6e\x30\xea\x30\xb9\x30\xc8\x30\x6e\x30\x2d\x4e\x6b\x30\x7a\x7a\x4d\x30\xe1\x30\xe2\x30\xea\x30\xbf\x30\xb0\x30\x4c\x30\x42\x30\x8a\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"HeapErrorCheck Error : 使用メモリタグのリストの中に空きメモリタグがありました" @*/ )) ;
				PrintInfoMemory( Heap->UseSeparateInfo ? MemTag : MemTag->UserAddress, Heap->UseSeparateInfo ) ;
				*( ( DWORD * )0x00000000 ) = 0xffffffff ;
			}
			TotalUseSize += MemTag->Size ;
		}
		if( UseNum != Heap->UseTagNum )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x48\x00\x65\x00\x61\x00\x70\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x43\x00\x68\x00\x65\x00\x63\x00\x6b\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x7f\x4f\x28\x75\xe1\x30\xe2\x30\xea\x30\xbf\x30\xb0\x30\x6e\x30\x70\x65\x4c\x30\x00\x4e\xf4\x81\x57\x30\x7e\x30\x5b\x30\x93\x30\x20\x00\x20\x00\x55\x00\x73\x00\x65\x00\x4e\x00\x75\x00\x6d\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x20\x00\x48\x00\x65\x00\x61\x00\x70\x00\x2d\x00\x3e\x00\x55\x00\x73\x00\x65\x00\x54\x00\x61\x00\x67\x00\x4e\x00\x75\x00\x6d\x00\x3a\x00\x25\x00\x64\x00\x00"/*@ L"HeapErrorCheck Error : 使用メモリタグの数が一致しません  UseNum:%d  Heap->UseTagNum:%d" @*/, UseNum, Heap->UseTagNum )) ;
			*( ( DWORD * )0x00000000 ) = 0xffffffff ;
		}
		if( TotalUseSize != Heap->TotalUseSize )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x48\x00\x65\x00\x61\x00\x70\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x43\x00\x68\x00\x65\x00\x63\x00\x6b\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x7f\x4f\x28\x75\xe1\x30\xe2\x30\xea\x30\x6e\x30\xb5\x30\xa4\x30\xba\x30\x4c\x30\x00\x4e\xf4\x81\x57\x30\x7e\x30\x5b\x30\x93\x30\x20\x00\x20\x00\x54\x00\x6f\x00\x74\x00\x61\x00\x6c\x00\x55\x00\x73\x00\x65\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x3a\x00\x25\x00\x6c\x00\x6c\x00\x64\x00\x20\x00\x20\x00\x48\x00\x65\x00\x61\x00\x70\x00\x2d\x00\x3e\x00\x54\x00\x6f\x00\x74\x00\x61\x00\x6c\x00\x55\x00\x73\x00\x65\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x3a\x00\x25\x00\x6c\x00\x6c\x00\x64\x00\x00"/*@ L"HeapErrorCheck Error : 使用メモリのサイズが一致しません  TotalUseSize:%lld  Heap->TotalUseSize:%lld" @*/, TotalUseSize, Heap->TotalUseSize )) ;
			*( ( DWORD * )0x00000000 ) = 0xffffffff ;
		}
	}

	// 総空きメモリサイズのチェック
	{
		ALLOCMEM_SIZE_TYPE TotalFreeSize ;
		int FreeNum ;

		TotalFreeSize = 0 ;
		FreeNum = 0 ;
		if( Heap->AllocType == ALLOCMEMTYPE_SIMPLE )
		{
			for( MemTag = Heap->FreeFirstTag ; MemTag != NULL ; MemTag = MemTag->ListNext, FreeNum ++ )
			{
				if( ( MemTag->Flag & ALLOCMEMTAG_FLAG_USE ) != 0 )
				{
					DXST_LOGFILEFMT_ADDUTF16LE(( "\x48\x00\x65\x00\x61\x00\x70\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x43\x00\x68\x00\x65\x00\x63\x00\x6b\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x7a\x7a\x4d\x30\xe1\x30\xe2\x30\xea\x30\xbf\x30\xb0\x30\x6e\x30\xea\x30\xb9\x30\xc8\x30\x6e\x30\x2d\x4e\x6b\x30\x7f\x4f\x28\x75\xe1\x30\xe2\x30\xea\x30\xbf\x30\xb0\x30\x4c\x30\x42\x30\x8a\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"HeapErrorCheck Error : 空きメモリタグのリストの中に使用メモリタグがありました" @*/ )) ;
					PrintInfoMemory( Heap->UseSeparateInfo ? MemTag : MemTag->UserAddress, Heap->UseSeparateInfo ) ;
					*( ( DWORD * )0x00000000 ) = 0xffffffff ;
				}
				TotalFreeSize += MemTag->Size ;
			}
		}
		else
		if( Heap->AllocType == ALLOCMEMTYPE_TLSF )
		{
			int i ;
			int j ;
			int ValidCount ;

#ifdef __64BIT__
			for( i = 0 ; i < 64 ; i ++ )
#else // __64BIT__
			for( i = 0 ; i < 32 ; i ++ )
#endif // __64BIT__
			{
#ifdef __64BIT__
				if( ( Heap->FreeTagBitList1 & ( ULL_NUM( 1 ) << i ) ) == 0 )
#else // __64BIT__
				if( ( Heap->FreeTagBitList1 & (          1   << i ) ) == 0 )
#endif // __64BIT__
				{
					continue ;
				}

				ValidCount = 0 ;
				for( j = 0 ; j < ( 1 << ALLOCMEM_CATEGORY_2_SHIFT_NUM ) ; j ++ )
				{
					if( ( Heap->FreeTagBitList2[ i ] & ( 1 << j ) ) == 0 )
					{
						continue ;
					}

					ValidCount ++ ;

					if( Heap->FreeFirstTagArray[ i ][ j ] == NULL )
					{
						DXST_LOGFILEFMT_ADDUTF16LE(( "\x48\x00\x65\x00\x61\x00\x70\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x43\x00\x68\x00\x65\x00\x63\x00\x6b\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x54\x00\x4c\x00\x53\x00\x46\x00\xa8\x30\xe9\x30\xfc\x30\x00\x30\x00\x30\xab\x30\xc6\x30\xb4\x30\xea\x30\x12\xff\xc6\x30\xfc\x30\xd6\x30\xeb\x30\xa8\x30\xe9\x30\xfc\x30\x00"/*@ L"HeapErrorCheck Error : TLSFエラー　　カテゴリ２テーブルエラー" @*/ )) ;
						*( ( DWORD * )0x00000000 ) = 0xffffffff ;
					}

					for( MemTag = Heap->FreeFirstTagArray[ i ][ j ] ; MemTag != NULL ; MemTag = MemTag->ListNext, FreeNum ++ )
					{
						// カテゴリに適合したサイズかをチェック
						int MSB = GetMSB( MemTag->Size ) ; 
						int SLI = GetSLI( MemTag->Size, MSB, ALLOCMEM_CATEGORY_2_SHIFT_NUM ) ; 

						if( MSB != i || SLI != j )
						{
							DXST_LOGFILEFMT_ADDUTF16LE(( "\x48\x00\x65\x00\x61\x00\x70\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x43\x00\x68\x00\x65\x00\x63\x00\x6b\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x54\x00\x4c\x00\x53\x00\x46\x00\xa8\x30\xe9\x30\xfc\x30\x00\x30\x00\x30\x7a\x7a\x4d\x30\xe1\x30\xe2\x30\xea\x30\xbf\x30\xb0\x30\xb5\x30\xa4\x30\xba\x30\xa8\x30\xe9\x30\xfc\x30\x20\x00\x20\x00\x4d\x00\x53\x00\x42\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x20\x00\x69\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x20\x00\x53\x00\x4c\x00\x49\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x20\x00\x6a\x00\x3a\x00\x25\x00\x64\x00\x00"/*@ L"HeapErrorCheck Error : TLSFエラー　　空きメモリタグサイズエラー  MSB:%d  i:%d  SLI:%d  j:%d" @*/, MSB, i, SLI, j )) ;
							*( ( DWORD * )0x00000000 ) = 0xffffffff ;
						}

						if( MemTag->FLI != MSB || MemTag->SLI != SLI )
						{
							DXST_LOGFILEFMT_ADDUTF16LE(( "\x48\x00\x65\x00\x61\x00\x70\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x43\x00\x68\x00\x65\x00\x63\x00\x6b\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x54\x00\x4c\x00\x53\x00\x46\x00\xa8\x30\xe9\x30\xfc\x30\x00\x30\x00\x30\x7a\x7a\x4d\x30\xe1\x30\xe2\x30\xea\x30\xbf\x30\xb0\x30\xab\x30\xc6\x30\xb4\x30\xea\x30\xc5\x60\x31\x58\xa8\x30\xe9\x30\xfc\x30\x20\x00\x20\x00\x4d\x00\x65\x00\x6d\x00\x54\x00\x61\x00\x67\x00\x2d\x00\x3e\x00\x46\x00\x4c\x00\x49\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x20\x00\x4d\x00\x53\x00\x42\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x20\x00\x4d\x00\x65\x00\x6d\x00\x54\x00\x61\x00\x67\x00\x2d\x00\x3e\x00\x53\x00\x4c\x00\x49\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x20\x00\x53\x00\x4c\x00\x49\x00\x3a\x00\x25\x00\x64\x00\x00"/*@ L"HeapErrorCheck Error : TLSFエラー　　空きメモリタグカテゴリ情報エラー  MemTag->FLI:%d  MSB:%d  MemTag->SLI:%d  SLI:%d" @*/, MemTag->FLI, MSB, MemTag->SLI, SLI )) ;
							*( ( DWORD * )0x00000000 ) = 0xffffffff ;
						}

						if( ( MemTag->Flag & ALLOCMEMTAG_FLAG_USE ) != 0 )
						{
							DXST_LOGFILEFMT_ADDUTF16LE(( "\x48\x00\x65\x00\x61\x00\x70\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x43\x00\x68\x00\x65\x00\x63\x00\x6b\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x54\x00\x4c\x00\x53\x00\x46\x00\xa8\x30\xe9\x30\xfc\x30\x00\x30\x00\x30\x7a\x7a\x4d\x30\xe1\x30\xe2\x30\xea\x30\xbf\x30\xb0\x30\xea\x30\xb9\x30\xc8\x30\x6b\x30\x7f\x4f\x28\x75\xe1\x30\xe2\x30\xea\x30\xbf\x30\xb0\x30\x4c\x30\x2b\x54\x7e\x30\x8c\x30\x66\x30\x44\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"HeapErrorCheck Error : TLSFエラー　　空きメモリタグリストに使用メモリタグが含まれていました" @*/ )) ;
							PrintInfoMemory( Heap->UseSeparateInfo ? MemTag : MemTag->UserAddress, Heap->UseSeparateInfo ) ;
							*( ( DWORD * )0x00000000 ) = 0xffffffff ;
						}
						TotalFreeSize += MemTag->Size ;
					}
				}

				if( ValidCount == 0 )
				{
					DXST_LOGFILEFMT_ADDUTF16LE(( "\x48\x00\x65\x00\x61\x00\x70\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x43\x00\x68\x00\x65\x00\x63\x00\x6b\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x54\x00\x4c\x00\x53\x00\x46\x00\xa8\x30\xe9\x30\xfc\x30\x00\x30\x00\x30\xab\x30\xc6\x30\xb4\x30\xea\x30\x11\xff\xc6\x30\xfc\x30\xd6\x30\xeb\x30\xa8\x30\xe9\x30\xfc\x30\x00"/*@ L"HeapErrorCheck Error : TLSFエラー　　カテゴリ１テーブルエラー" @*/ )) ;
					*( ( DWORD * )0x00000000 ) = 0xffffffff ;
				}
			}
		}
		if( FreeNum != Heap->FreeTagNum )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x48\x00\x65\x00\x61\x00\x70\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x43\x00\x68\x00\x65\x00\x63\x00\x6b\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x7a\x7a\x4d\x30\xe1\x30\xe2\x30\xea\x30\xbf\x30\xb0\x30\x6e\x30\x70\x65\x4c\x30\x00\x4e\xf4\x81\x57\x30\x7e\x30\x5b\x30\x93\x30\x20\x00\x20\x00\x46\x00\x72\x00\x65\x00\x65\x00\x4e\x00\x75\x00\x6d\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x20\x00\x48\x00\x65\x00\x61\x00\x70\x00\x2d\x00\x3e\x00\x46\x00\x72\x00\x65\x00\x65\x00\x54\x00\x61\x00\x67\x00\x4e\x00\x75\x00\x6d\x00\x3a\x00\x25\x00\x64\x00\x00"/*@ L"HeapErrorCheck Error : 空きメモリタグの数が一致しません  FreeNum:%d  Heap->FreeTagNum:%d" @*/, FreeNum, Heap->FreeTagNum )) ;
			*( ( DWORD * )0x00000000 ) = 0xffffffff ;
		}
		if( TotalFreeSize != Heap->TotalFreeSize )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x48\x00\x65\x00\x61\x00\x70\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x43\x00\x68\x00\x65\x00\x63\x00\x6b\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x7a\x7a\x4d\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xb5\x30\xa4\x30\xba\x30\x4c\x30\x00\x4e\xf4\x81\x57\x30\x7e\x30\x5b\x30\x93\x30\x20\x00\x20\x00\x54\x00\x6f\x00\x74\x00\x61\x00\x6c\x00\x46\x00\x72\x00\x65\x00\x65\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x3a\x00\x25\x00\x6c\x00\x6c\x00\x64\x00\x20\x00\x20\x00\x48\x00\x65\x00\x61\x00\x70\x00\x2d\x00\x3e\x00\x54\x00\x6f\x00\x74\x00\x61\x00\x6c\x00\x46\x00\x72\x00\x65\x00\x65\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x3a\x00\x25\x00\x6c\x00\x6c\x00\x64\x00\x00"/*@ L"HeapErrorCheck Error : 空きメモリのサイズが一致しません  TotalFreeSize:%lld  Heap->TotalFreeSize:%lld" @*/, TotalFreeSize, Heap->TotalFreeSize )) ;
			*( ( DWORD * )0x00000000 ) = 0xffffffff ;
		}
	}

	// ユーザーメモリアドレスチェック
	{
		for( MemTag = Heap->UseFirstTag ; MemTag != NULL ; MemTag = MemTag->ListNext )
		{
			if( ( DWORD_PTR )MemTag->UserAddress % Heap->Aligned != 0 )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x48\x00\x65\x00\x61\x00\x70\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x43\x00\x68\x00\x65\x00\x63\x00\x6b\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xe6\x30\xfc\x30\xb6\x30\xfc\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xa2\x30\xe9\x30\xa4\x30\xf3\x30\x4c\x30\x07\x63\x9a\x5b\x55\x30\x8c\x30\x5f\x30\xa2\x30\xe9\x30\xa4\x30\xf3\x30\x68\x30\x70\x75\x6a\x30\x8a\x30\x7e\x30\x59\x30\x20\x00\x20\x00\x4d\x00\x65\x00\x6d\x00\x54\x00\x61\x00\x67\x00\x2d\x00\x3e\x00\x55\x00\x73\x00\x65\x00\x72\x00\x41\x00\x64\x00\x64\x00\x72\x00\x65\x00\x73\x00\x73\x00\x3a\x00\x30\x00\x78\x00\x25\x00\x6c\x00\x6c\x00\x30\x00\x31\x00\x36\x00\x78\x00\x20\x00\x20\x00\x48\x00\x65\x00\x61\x00\x70\x00\x2d\x00\x3e\x00\x41\x00\x6c\x00\x69\x00\x67\x00\x6e\x00\x65\x00\x64\x00\x3a\x00\x30\x00\x78\x00\x25\x00\x30\x00\x38\x00\x78\x00\x00"/*@ L"HeapErrorCheck Error : ユーザーメモリのアラインが指定されたアラインと異なります  MemTag->UserAddress:0x%ll016x  Heap->Aligned:0x%08x" @*/, ( ULONGLONG )MemTag->UserAddress, ( DWORD )Heap->Aligned )) ;
				PrintInfoMemory( Heap->UseSeparateInfo ? MemTag : MemTag->UserAddress, Heap->UseSeparateInfo ) ;
				*( ( DWORD * )0x00000000 ) = 0xffffffff ;
			}
			if( Heap->UseSeparateInfo )
			{
				if( ( DWORD_PTR )MemTag->UserAddress <  ( DWORD_PTR )MemTag->UseHeapMem->HeapAddress ||
					( DWORD_PTR )MemTag->UserAddress >= ( DWORD_PTR )MemTag->UseHeapMem->HeapAddress + MemTag->UseHeapMem->HeapSize )
				{
					DXST_LOGFILEFMT_ADDUTF16LE(( "\x48\x00\x65\x00\x61\x00\x70\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x43\x00\x68\x00\x65\x00\x63\x00\x6b\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xe6\x30\xfc\x30\xb6\x30\xfc\x30\xe1\x30\xe2\x30\xea\x30\x4c\x30\xba\x78\xdd\x4f\x57\x30\x5f\x30\xd2\x30\xfc\x30\xd7\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xc4\x7b\xf2\x56\x6b\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x20\x00\x20\x00\x48\x00\x65\x00\x61\x00\x70\x00\x2d\x00\x3e\x00\x55\x00\x73\x00\x65\x00\x53\x00\x65\x00\x70\x00\x61\x00\x72\x00\x61\x00\x74\x00\x65\x00\x49\x00\x6e\x00\x66\x00\x6f\x00\x20\x00\x3d\x00\x20\x00\x54\x00\x52\x00\x55\x00\x45\x00\x00"/*@ L"HeapErrorCheck Error : ユーザーメモリが確保したヒープメモリの範囲にありません  Heap->UseSeparateInfo = TRUE" @*/ )) ;
					PrintInfoMemory( Heap->UseSeparateInfo ? MemTag : MemTag->UserAddress, Heap->UseSeparateInfo ) ;
					*( ( DWORD * )0x00000000 ) = 0xffffffff ;
				}
			}
			else
			{
				if( ( DWORD_PTR )MemTag->UserAddress - *( ( DWORD * )( ( BYTE * )MemTag->UserAddress - sizeof( DWORD ) ) ) != ( DWORD_PTR )MemTag )
				{
					DXST_LOGFILEFMT_ADDUTF16LE(( "\x48\x00\x65\x00\x61\x00\x70\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x43\x00\x68\x00\x65\x00\x63\x00\x6b\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xe6\x30\xfc\x30\xb6\x30\xfc\x30\xe1\x30\xe2\x30\xea\x30\x4c\x30\xba\x78\xdd\x4f\x57\x30\x5f\x30\xd2\x30\xfc\x30\xd7\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xc4\x7b\xf2\x56\x6b\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x20\x00\x20\x00\x48\x00\x65\x00\x61\x00\x70\x00\x2d\x00\x3e\x00\x55\x00\x73\x00\x65\x00\x53\x00\x65\x00\x70\x00\x61\x00\x72\x00\x61\x00\x74\x00\x65\x00\x49\x00\x6e\x00\x66\x00\x6f\x00\x20\x00\x3d\x00\x20\x00\x46\x00\x41\x00\x4c\x00\x53\x00\x45\x00\x00"/*@ L"HeapErrorCheck Error : ユーザーメモリが確保したヒープメモリの範囲にありません  Heap->UseSeparateInfo = FALSE" @*/ )) ;
					PrintInfoMemory( Heap->UseSeparateInfo ? MemTag : MemTag->UserAddress, Heap->UseSeparateInfo ) ;
					*( ( DWORD * )0x00000000 ) = 0xffffffff ;
				}
			}
		}
	}

	DX_HEAP_LEAVE_CRITICAL_SECTION( Heap )

	return 0 ;
}

// ヒープの確保状況を描画する

#ifndef DX_NON_GRAPHICS

static void HeapDrawAllocInfo_Help( ALLOCMEMTAG *MemTag, int x, int y, int Width, float Scale, int Color )
{
	DWORD_PTR		Address ;
	HEAPMEMINFO *	HeapMemInfo ;
	int				DrawY ;
	int				DrawCheckHeight ;
	int				DrawPixelPosition ;
	int				DrawPixelWidth ;
	int				CheckPixelPosition ;
	int				i ;

	Address = 0 ;
	for( HeapMemInfo = MemTag->Owner->FirstHeapMem ; HeapMemInfo != MemTag->UseHeapMem ; HeapMemInfo = HeapMemInfo->Next )
	{
		Address += HeapMemInfo->HeapSize ;
	}

	if( MemTag->Owner->UseSeparateInfo )
	{
		Address += ( DWORD_PTR )MemTag->AllocAddress - ( DWORD_PTR )MemTag->UseHeapMem->HeapAddress ;
	}
	else
	{
		Address += ( DWORD_PTR )MemTag - ( DWORD_PTR )MemTag->UseHeapMem->HeapAddress ;
	}

	DrawPixelPosition = _FTOL( Address      * Scale ) ;
	DrawPixelWidth    = _FTOL( MemTag->Size * Scale ) ;
	if( DrawPixelWidth <= 0 )
	{
		DrawPixelWidth = 1 ;
	}

	DrawY           = DrawPixelPosition / Width ;
	DrawCheckHeight = ( DrawPixelPosition + DrawPixelWidth + Width - 1 ) / Width - DrawY ;

	CheckPixelPosition = DrawY * Width ;
	for( i = DrawY ; i < DrawY + DrawCheckHeight ; i ++, CheckPixelPosition += Width )
	{
		int DrawPixelTempX1 ;
		int DrawPixelTempX2 ;

		if( DrawPixelPosition <= CheckPixelPosition )
		{
			DrawPixelTempX1 = 0 ;
		}
		else
		{
			DrawPixelTempX1 = DrawPixelPosition - CheckPixelPosition ;
		}

		if( DrawPixelPosition + DrawPixelWidth >= CheckPixelPosition + Width )
		{
			DrawPixelTempX2 = Width ;
		}
		else
		{
			DrawPixelTempX2 = ( DrawPixelPosition + DrawPixelWidth ) - CheckPixelPosition ;
		}

		NS_DrawBox( x + DrawPixelTempX1, y + i, x + DrawPixelTempX2, y + i + 1, Color, TRUE ) ;
	}
}

#endif // DX_NON_GRAPHICS

extern int HeapDrawAllocInfo( HEAPINFO *Heap, int x, int y, int Width, int Height )
{
#ifndef DX_NON_GRAPHICS
	int				Pixels ;
	float			Scale ;
	ALLOCMEMTAG *	MemTag ;
	int				ColorUse ;
	int				ColorFree ;
	int				BlendMode ;
	int				BlendParam ;
	int				RedBright ;
	int				GreenBright ;
	int				BlueBright ;
	RECT			DrawArea ;

	DX_HEAP_ENTER_CRITICAL_SECTION( Heap )

	NS_GetDrawBlendMode( &BlendMode, &BlendParam ) ;
	NS_GetDrawBright( &RedBright, &GreenBright, &BlueBright ) ;
	NS_GetDrawArea( &DrawArea ) ;

	NS_SetDrawArea( x, y, x + Width, y + Height ) ;
	NS_SetDrawBright( 255,255,255 ) ;

	Pixels = Width * Height ;

	Scale = ( float )Pixels / ( Heap->TotalFreeSize + Heap->TotalUseSize ) ;

	NS_SetDrawBlendMode( DX_BLENDMODE_ALPHA, 255 ) ;
	NS_DrawBox( x, y, x + Width, y + Height, GetColor( 0, 0, 0 ), TRUE ) ;

	NS_SetDrawBlendMode( DX_BLENDMODE_ADD, 255 ) ;

	ColorUse  = GetColor( 255,0,0 ) ;
	ColorFree = GetColor( 0,255,0 ) ;
	for( MemTag = Heap->FirstTag ; MemTag != NULL ; MemTag = MemTag->Next )
	{
		HeapDrawAllocInfo_Help( MemTag, x, y, Width, Scale, ( MemTag->Flag & ALLOCMEMTAG_FLAG_USE ) ? ColorUse : ColorFree ) ;
	}

	NS_SetDrawBlendMode( BlendMode, BlendParam ) ;
	NS_SetDrawBright( RedBright, GreenBright, BlueBright ) ;
	NS_SetDrawArea( DrawArea.left, DrawArea.top, DrawArea.right, DrawArea.bottom ) ;

	DX_HEAP_LEAVE_CRITICAL_SECTION( Heap )

	return 0 ;
#else // DX_NON_GRAPHICS
	return -1 ;
#endif // DX_NON_GRAPHICS
}











#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE
