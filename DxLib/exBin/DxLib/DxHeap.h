// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		ヒープ関係プログラムヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXHEAP_H__
#define __DXHEAP_H__

// インクルード ------------------------------------------------------------------
#include "DxCompileConfig.h"
#include "DxThread.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------


#define HEAPMEM_FLAG_AUTOALLOC					(0x0001)			// 自動確保したメモリ領域かどうか

#define ALLOCMEMTAG_MAGIC_ID					(0x4D454D44)		// "DMEM"( ビッグエンディアンだと逆になっちゃうけど・・・ )

#define ALLOCMEM_MIN_ALIGNED					(16)				// アラインの最低値
#define ALLOCMEM_CATEGORY_2_SHIFT_NUM			(5)					// カテゴリー２の数( 32 )の2の乗数

#ifdef __64BIT__
#define ALLOCMEMTAG_NAMELENGTH					(18)				// 確保メモリの名前
#define ALLOCMEM_CATEGORY_1_NUM					(64)				// カテゴリー１の数( ただし 0 〜 ALLOCMEM_CATEGORY_2_SHIFT_NUM のカテゴリは使用しない( できない ) )
#define ALLOCMEM_SIZE_TYPE						ULONGLONG			// 確保メモリのサイズの型
#define ALLOCMEMTAG_SIZE						(112)				// ALLOCMEMTAG構造体のサイズ
#else // __64BIT__
#define ALLOCMEMTAG_NAMELENGTH					(18)				// 確保メモリの名前
#define ALLOCMEM_CATEGORY_1_NUM					(32)				// カテゴリー１の数( ただし 0 〜 ALLOCMEM_CATEGORY_2_SHIFT_NUM のカテゴリは使用しない( できない ) )
#define ALLOCMEM_SIZE_TYPE						DWORD				// 確保メモリのサイズの型
#define ALLOCMEMTAG_SIZE						(72)				// ALLOCMEMTAG構造体のサイズ
#endif // __64BIT__

#define ALLOCMEMTAG_DEBUGAREA_SIZE				(16)				// デバッグ用の値で埋める領域のサイズ

// 一つのメモリ確保で必要なタグ関係のデータサイズ
#define ALLOCMEMTAG_TOTAL_SIZE					( ALLOCMEMTAG_SIZE + ALLOCMEMTAG_DEBUGAREA_SIZE * 2 + sizeof( DWORD ) )

	// メモリタグ一つ入れるために最低限必要な空きメモリサイズ( アラインを考慮したもの )
#define ALLOCMEMTAG_MIN_SIZE					( ( ALLOCMEMTAG_TOTAL_SIZE + ALLOCMEM_MIN_ALIGNED * 2 + ALLOCMEM_MIN_ALIGNED - 1 ) / ALLOCMEM_MIN_ALIGNED * ALLOCMEM_MIN_ALIGNED )

#define ALLOCMEMTAG_FLAG_LINE_BIT_16			(0x01)				// 行番号の16bit目( WORD では 65535行までしか表現できないので、262143行まで表現できるようにするための対応 )
#define ALLOCMEMTAG_FLAG_LINE_BIT_17			(0x02)				// 行番号の17bit目( 同上 )
#define ALLOCMEMTAG_FLAG_USE					(0x04)				// 確保・使用されているメモリかどうか
#define ALLOCMEMTAG_FLAG_NEXT_CONNECT			(0x08)				// Next のメモリ領域とアドレスが連続しているかどうか
#define ALLOCMEMTAG_FLAG_PREV_CONNECT			(0x10)				// Prev のメモリ領域とアドレスが連続しているかどうか
#define ALLOCMEMTAG_FLAG_NO_DUMP				(0x20)				// メモリダンプに出力しないかどうか

// メモリ確保処理タイプ
#define ALLOCMEMTYPE_SIMPLE						(0)					// シンプルで低速なアルゴリズム
#define ALLOCMEMTYPE_TLSF						(1)					// TLSF

// ヒープメモリ確保コールバック関数で使用できる情報のサイズ
#define ALLOCHEAP_CALLBACK_INFO_SIZE					(32)

// 構造体定義 --------------------------------------------------------------------

// 確保メモリのタグ情報
struct ALLOCMEMTAG /* サイズは ALLOCMEMTAG_SIZE byte になるように調整 */
{
	DWORD					MagicID ;							// DMEM
	char					Name[ ALLOCMEMTAG_NAMELENGTH ] ;	// ファイルパス
	BYTE					Flag ;								// フラグ( ALLOCMEMTAG_FLAG_USE など )
	BYTE					FLI ;								// 空きメモリリストのカテゴリ１( タイプが ALLOCMEMTYPE_TLSF の際に使用される )
	BYTE					SLI ;								// 空きメモリリストのカテゴリ２( タイプが ALLOCMEMTYPE_TLSF の際に使用される )
	BYTE					IDHigh ;							// ID上位8ビット
	WORD					IDLow ;								// ID下位16ビット
	WORD					Time ;								// 時間
	WORD					Line ;								// 行番号
	ALLOCMEM_SIZE_TYPE		Size ;								// 占有しているサイズ
	void *					AllocAddress ;						// 確保しているメモリ領域の先頭アドレス( Owner->UseSeparateInfo = TRUE の場合のみ使用 )
	void *					UserAddress ;						// AllocMemory の戻り値として返したアドレス
	ALLOCMEM_SIZE_TYPE		UserSize ;							// AllocMemory で指定されたサイズ
	ALLOCMEMTAG *			Prev ;								// 一つ前のタグ情報へのポインタ( 先端の場合は NULL )
	ALLOCMEMTAG *			Next ;								// 一つ先のタグ情報へのポインタ( 終端の場合は NULL )
	ALLOCMEMTAG *			ListPrev ;							// リストの一つ前のタグ情報へのポインタ( 先端の場合は NULL )
	ALLOCMEMTAG *			ListNext ;							// リストの一つ先のタグ情報へのポインタ( 終端の場合は NULL )
	struct HEAPINFO *		Owner ;								// メモリタグが属しているヒープ情報へのポインタ
	struct HEAPMEMINFO *	UseHeapMem ;						// 使用しているメモリ領域へのポインタ
} ;

// ヒープ用に確保されたメモリの情報
struct HEAPMEMINFO
{
	void *					Address ;							// 使用可能として渡されたメモリ領域の先頭アドレス
	ALLOCMEM_SIZE_TYPE		Size ;								// 使用可能として渡されたメモリのサイズ
	void *					UseAddress ;						// アラインを考慮した場合に使用できる先頭アドレス
	ALLOCMEM_SIZE_TYPE		UseSize ;							// アラインを考慮した場合に使用できるメモリのサイズ
	void *					HeapAddress ;						// ヒープ用に使用できるメモリの先頭アドレス
	ALLOCMEM_SIZE_TYPE		HeapSize ;							// ヒープ用に使用できるメモリのサイズ
	ALLOCMEM_SIZE_TYPE		UseMemTagCount ;					// このメモリを使用している使用メモリタグの数
	ALLOCMEMTAG *			FirstMemTag ;						// このメモリを使用している最初のメモリタグへのポインタ

	HEAPMEMINFO *			Prev ;								// 一つ前のヒープ用に確保されたメモリの情報へのポインタ( 先端の場合は NULL )
	HEAPMEMINFO *			Next ;								// 一つ先のヒープ用に確保されたメモリの情報へのポインタ( 終端の場合は NULL )

	DWORD					Flag ;								// フラグ( HEAPMEM_FLAG_AUTOALLOC など )

	BYTE					CallbackBuffer[ ALLOCHEAP_CALLBACK_INFO_SIZE ] ;	// コールバック関数内で自由に使用できるバッファ
} ;

// ヒープ情報
struct HEAPINFO
{
	int						AutoAlloc ;							// ヒープ用のメモリ領域を自動で確保するかどうか( TRUE:自動で確保する  FALSE:自動では確保しない )
	ALLOCMEM_SIZE_TYPE		AutoAllocUnitSize ;					// ヒープ用のメモリ領域を自動で確保する場合の、一度に確保するヒープ領域のサイズ
	int						AutoAlloc_CallbackParam ;			// InitializeHeap の引数 AutoAlloc_CallbackParam の値
	int					 ( *AutoAlloc_CreateHeapCallback     )( int Param, void *Buffer, ALLOCMEM_SIZE_TYPE Size ) ;	// ヒープ用のメモリ領域を自動で確保するためのコールバック関数
	void *				 ( *AutoAlloc_GetHeapAddressCallback )( int Param, void *Buffer ) ;	// ヒープ用のメモリ領域を自動で確保した場合の、メモリアドレスを取得するための関数
	ALLOCMEM_SIZE_TYPE	 ( *AutoAlloc_GetHeapSizeCallback    )( int Param, void *Buffer ) ;	// ヒープ用のメモリ領域を自動で確保した場合の、メモリサイズを取得するための関数
	int					 ( *AutoAlloc_DeleteHeapCallback     )( int Param, void *Buffer ) ;	// ヒープ用のメモリ領域を自動で確保した場合の、メモリ領域を開放するための関数

	int						AllocType ;							// メモリ確保処理タイプ( ALLOCMEMTYPE_SIMPLE 等 )
	ALLOCMEM_SIZE_TYPE		Aligned ;							// 返すメモリアドレスのアライン

	DWORD					AllocMemoryID ;						// 確保したメモリに割り当てるＩＤ
	DWORD					AllocMemoryTime ;					// 確保したメモリに設定する時間

	int						UseSeparateInfo ;					// メモリタグやヒープメモリの情報をヒープ以外の領域に確保した配列を使用するかどうか( TRUE:使用する  FALSE:使用しない )
	int						SepTagArrayLength ;					// メモリタグをヒープ以外の領域に確保したメモリタグ配列の長さ
	int						SepTagArrayUseNum ;					// メモリタグをヒープ以外の領域に確保したメモリタグ配列の内の使用している数
	ALLOCMEMTAG *			SepTagArray ;						// メモリタグをヒープ以外の領域に確保したメモリタグ配列へのポインタ
	ALLOCMEMTAG *			SepTagArrayNotUseFirst ;			// メモリタグをヒープ以外の領域に確保したメモリタグ配列のまだ未使用のメモリタグのリストの先頭要素へのポインタ
	int						SepHeapMemArrayLength ;				// ヒープ用に確保されたメモリの情報をヒープ以外の領域に確保した情報配列の長さ
	int						SepHeapMemArrayUseNum ;				// ヒープ用に確保されたメモリの情報をヒープ以外の領域に確保した情報配列の内の使用している数
	HEAPMEMINFO *			SepHeapMemArray ;					// ヒープ用に確保されたメモリの情報をヒープ以外の領域に確保した情報配列へのポインタ
	HEAPMEMINFO *			SepHeapMemArrayNotUseFirst ;		// ヒープ用に確保されたメモリの情報をヒープ以外の領域に確保した情報配列のまだ未使用の情報のリストの先頭要素へのポインタ

	HEAPMEMINFO *			FirstHeapMem ;						// ヒープ用に確保されたメモリの情報のリストの最初の情報へのポインタ

	int						FreeTagNum ;						// 空きメモリタグの数
	ALLOCMEM_SIZE_TYPE		TotalFreeSize ;						// 空きメモリの合計サイズ

	ALLOCMEMTAG *			FirstTag ;							// 最初のタグへのポインタ
	ALLOCMEMTAG *			LastTag ;							// 最後のタグへのポインタ

	int						UseTagNum ;							// 使用されているメモリの数
	ALLOCMEM_SIZE_TYPE		TotalUseSize ;						// 使用されているメモリの合計サイズ
	ALLOCMEMTAG *			UseFirstTag ;						// 使用メモリの最初のタグへのポインタ
	ALLOCMEMTAG *			UseLastTag ;						// 使用メモリの最後のタグへのポインタ


#if defined( DX_THREAD_SAFE ) || defined( DX_THREAD_SAFE_NETWORK_ONLY )
	DX_CRITICAL_SECTION		MemoryAllocCriticalSection ;		// メモリ確保処理用クリティカルセクション
#endif

	// ALLOCMEMTYPE_TLSF タイプでのみ使用
#ifdef __64BIT__
	ULONGLONG				FreeTagBitList1 ;					// カテゴリ１の空きメモリがあるカテゴリのビットリスト
#else // __64BIT__
	DWORD					FreeTagBitList1 ;					// カテゴリ１の空きメモリがあるカテゴリのビットリスト
#endif // __64BIT__
	DWORD					FreeTagBitList2[ ALLOCMEM_CATEGORY_1_NUM ] ;												// 各カテゴリ１の空きメモリがあるカテゴリ２のビットリスト
	ALLOCMEMTAG *			FreeFirstTagArray[ ALLOCMEM_CATEGORY_1_NUM ][ 1 << ALLOCMEM_CATEGORY_2_SHIFT_NUM ] ;		// 各カテゴリの空きメモリ最初のタグへのポインタ
	ALLOCMEMTAG *			FreeLastTagArray[  ALLOCMEM_CATEGORY_1_NUM ][ 1 << ALLOCMEM_CATEGORY_2_SHIFT_NUM ] ;		// 各カテゴリの空きメモリ最後のタグへのポインタ


	// ALLOCMEMTYPE_SIMPLE タイプでのみ使用
	ALLOCMEMTAG *			FreeFirstTag ;						// 空きメモリの最初のタグへのポインタ
	ALLOCMEMTAG *			FreeLastTag ;						// 空きメモリの最後のタグへのポインタ
} ;

// テーブル-----------------------------------------------------------------------

// 内部大域変数宣言 --------------------------------------------------------------

// 関数プロトタイプ宣言-----------------------------------------------------------

// ヒープを初期化する
extern	int			InitializeHeap(
						HEAPINFO *				Heap,
						int						AllocType				= ALLOCMEMTYPE_SIMPLE,
						ALLOCMEM_SIZE_TYPE		Aligned					= 16,
						int						AutoAlloc				= TRUE,
						ALLOCMEM_SIZE_TYPE		AutoAllocUnitSize		= 16 * 1024 * 1024,
						int						AutoAlloc_CallbackParam	= 0,
						int					 ( *AutoAlloc_CreateHeapCallback     )( int Param, void *Buffer, ALLOCMEM_SIZE_TYPE Size ) = NULL,
						void *				 ( *AutoAlloc_GetHeapAddressCallback )( int Param, void *Buffer ) = NULL,
						ALLOCMEM_SIZE_TYPE	 ( *AutoAlloc_GetHeapSizeCallback    )( int Param, void *Buffer ) = NULL,
						int					 ( *AutoAlloc_DeleteHeapCallback     )( int Param, void *Buffer ) = NULL,
						void *					HeapAddress				= NULL,
						ALLOCMEM_SIZE_TYPE		HeapSize				= 0,
						int						UseSeparateInfo			= FALSE,
						int						SepTagArrayLength		= 0,
						ALLOCMEMTAG *			SepTagArray				= NULL,
						int						SepHeapMemArrayLength	= 0,
						HEAPMEMINFO *			SepHeapMemArray			= NULL
					) ;
extern	int			TerminateHeap( HEAPINFO *Heap ) ;				// ヒープの後始末を行う
extern	int			AddHeapMemory( HEAPINFO *Heap, void *HeapAddress, ALLOCMEM_SIZE_TYPE HeapSize ) ;	// ヒープとして扱うメモリを追加する
extern	int			SubHeapMemory( HEAPINFO *Heap, void *HeapAddress ) ;								// ヒープとして扱うメモリを減らす
extern	ALLOCMEM_SIZE_TYPE	GetHeapTotalMemorySize( HEAPINFO *Heap ) ;	// ヒープが使用しているメモリの総サイズを取得する

// ヒープからメモリを確保する( UseSeparateInfo=TRUE の場合は、戻ってくるのは ALLOCMEMTAG 構造体のアドレス )
extern	void *		AllocMemory(
						HEAPINFO *			Heap,
						ALLOCMEM_SIZE_TYPE	Size,
						ALLOCMEM_SIZE_TYPE	Aligned			= 0,
						int					Reverse			= FALSE,
						const char *		Name			= NULL,
						int					Line			= 0,
						int					NoMemoryDump	= FALSE
					) ;
// 確保したメモリのサイズを変更する( UseSeparateInfo=TRUE の場合は、AllocAddress に ALLOCMEMTAG 構造体のアドレスを渡す )
extern	void *		ReallocMemory(
						void *				AllocAddress,
						int					UseSeparateInfo	= FALSE,
						ALLOCMEM_SIZE_TYPE	Size			= 0,
						ALLOCMEM_SIZE_TYPE	Aligned			= 0,
						const char *		Name			= NULL,
						int					Line			= 0
					) ;
extern	int			FreeMemory(            void *AllocAddress, int UseSeparateInfo = FALSE ) ;	// ヒープから確保したメモリを開放する( UseSeparateInfo = TRUE の場合は、AllocAddress に ALLOCMEMTAG 構造体のアドレスを渡す )
extern	void *		GetAllocMemoryAddress( void *AllocAddress, int UseSeparateInfo = FALSE ) ;	// ヒープから確保したメモリ領域のアドレスを取得する( UseSeparateInfo = TRUE の場合は、AllocAddress に ALLOCMEMTAG 構造体のアドレスを渡す )
extern	ALLOCMEM_SIZE_TYPE	GetAllocSize(  void *AllocAddress, int UseSeparateInfo = FALSE ) ;	// ヒープから確保したメモリのサイズを取得する( UseSeparateInfo = TRUE の場合は、AllocAddress に ALLOCMEMTAG 構造体のアドレスを渡す )
extern	ALLOCMEMTAG * GetAllocMemTag(      void *AllocAddress, int UseSeparateInfo = FALSE ) ;	// ヒープから確保したメモリの ALLOCMEMTAG構造体を取得する( UseSeparateInfo = TRUE の場合は、AllocAddress に ALLOCMEMTAG 構造体のアドレスを渡す )
extern	void		PrintInfoMemory(       void *AllocAddress, int UseSeparateInfo = FALSE ) ;	// ヒープから確保したメモリの情報をログに出力する( UseSeparateInfo = TRUE の場合は、AllocAddress に ALLOCMEMTAG 構造体のアドレスを渡す )
extern	int			HeapInfoDump(      HEAPINFO *Heap ) ;										// ヒープの情報をログに出力する
extern	int			SetHeapTime(       HEAPINFO *Heap, WORD Time ) ;							// ヒープから確保したメモリに設定する時間を変更する
extern	int			HeapErrorCheck(    HEAPINFO *Heap ) ;										// ヒープの内容にエラーがないかチェックする
extern	int			HeapDrawAllocInfo( HEAPINFO *Heap, int x, int y, int Width, int Height ) ;	// ヒープの確保状況を描画する





#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // __DXMEMORY_H__
