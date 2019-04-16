//-----------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		Windows用メモリ関係プログラム
// 
//  	Ver 3.20c
// 
//-----------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

// インクルード ---------------------------------------------------------------
#include "DxWinAPI.h"
#include "../DxMemory.h"

#if !defined(CINTERFACE) && defined(__c2__) &&  __clang_major__ == 3 && __clang_minor__ == 8
//To avoid compile error
//C:\Program Files (x86)\Windows Kits\8.1\Include\um\combaseapi.h(229,21): error : unknown type name 'IUnknown'
//          static_cast<IUnknown*>(*pp);    // make sure everyone derives from IUnknown
#define CINTERFACE
#endif
#include <windows.h>


#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 -----------------------------------------------------------------

// 構造体宣言 -----------------------------------------------------------------

// データ定義 -----------------------------------------------------------------

// 関数宣言 -------------------------------------------------------------------

// プログラム -----------------------------------------------------------------

// メモリ処理の初期化を行う関数の環境依存処理
extern int MemoryInitialize_PF( void )
{
	return 0 ;
}

// メモリ処理の後始末を行う関数の環境依存処理
extern int MemoryTerminate_PF( void )
{
	return 0 ;
}

// メモリ処理の周期的処理の環境依存処理
extern int MemoryProcess_PF( void )
{
	return 0 ;
}

// 環境依存の一般的なヒープ用のメモリ領域を確保するためのコールバック関数群の AutoAlloc_CreateHeapCallback
extern int NormalMemory_AutoAlloc_CreateHeapCallback_PF( int /*Param*/, void *Buffer, ALLOCMEM_SIZE_TYPE Size )
{
	MEMORY_BASIC_INFORMATION	MemoryBasicInfo ;
	PLATFORMHEAP_WIN *			PlatformHeap = ( PLATFORMHEAP_WIN * )Buffer ;
	SIZE_T						Result ;

	SETUP_WIN_API

	// バッファサイズチェック
	if( sizeof( PLATFORMHEAP_WIN ) > ALLOCHEAP_CALLBACK_INFO_SIZE )
	{
		*( ( DWORD * )0x00000000 ) = 0xffffffff ;
	}

	// メモリを確保
	PlatformHeap->AllocAddress = WinAPIData.Win32Func.VirtualAllocFunc( NULL, Size, MEM_COMMIT, PAGE_READWRITE ) ;
	if( PlatformHeap->AllocAddress == NULL )
	{
		return -1 ;
	}

	// 確保したメモリのサイズを取得
	Result = WinAPIData.Win32Func.VirtualQueryFunc( PlatformHeap->AllocAddress, &MemoryBasicInfo, sizeof( MemoryBasicInfo ) ) ;
	if( Result == 0 )
	{
		WinAPIData.Win32Func.VirtualFreeFunc( PlatformHeap->AllocAddress, 0, MEM_RELEASE ) ;
		return -1 ;
	}
	PlatformHeap->AllocSize = MemoryBasicInfo.RegionSize ;

	return 0 ;
}

// 環境依存の一般的なヒープ用のメモリ領域を確保するためのコールバック関数群の AutoAlloc_GetHeapAddressCallback
extern void * NormalMemory_AutoAlloc_GetHeapAddressCallback_PF( int /*Param*/, void *Buffer )
{
	PLATFORMHEAP_WIN *			PlatformHeap = ( PLATFORMHEAP_WIN * )Buffer ;

	return PlatformHeap->AllocAddress ;
}

// 環境依存の一般的なヒープ用のメモリ領域を確保するためのコールバック関数群の AutoAlloc_GetHeapSizeCallback
extern	ALLOCMEM_SIZE_TYPE	 NormalMemory_AutoAlloc_GetHeapSizeCallback_PF( int /*Param*/, void *Buffer )
{
	PLATFORMHEAP_WIN *			PlatformHeap = ( PLATFORMHEAP_WIN * )Buffer ;

	return ( ALLOCMEM_SIZE_TYPE )PlatformHeap->AllocSize ;
}

// 環境依存の一般的なヒープ用のメモリ領域を確保するためのコールバック関数群の AutoAlloc_DeleteHeapCallback
extern int  NormalMemory_AutoAlloc_DeleteHeapCallback_PF( int /*Param*/, void *Buffer )
{
	PLATFORMHEAP_WIN *			PlatformHeap = ( PLATFORMHEAP_WIN * )Buffer ;

	SETUP_WIN_API

	// メモリを解放
	WinAPIData.Win32Func.VirtualFreeFunc( PlatformHeap->AllocAddress, 0, MEM_RELEASE ) ;

	return 0 ;
}

// 確保しているメモリを列挙する関数の環境依存処理
extern int DxDumpAlloc_PF( void )
{
	return 0 ;
}


#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

