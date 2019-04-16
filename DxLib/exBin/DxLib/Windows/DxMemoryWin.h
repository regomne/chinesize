// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		Windows用メモリ関係プログラムヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXMEMORYWIN_H__
#define __DXMEMORYWIN_H__

// インクルード ------------------------------------------------------------------
#include "../DxCompileConfig.h"

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

// マクロ定義 --------------------------------------------------------------------

#define DXMEMORY_SMALLHEAP_UNITSIZE		( 4 * 1024 * 1024)			// 小さいメモリ確保用のヒープの単位確保サイズ
#define DXMEMORY_BIGHEAP_UNITSIZE		(32 * 1024 * 1024)			// 大きいメモリ確保用のヒープの単位確保サイズ
#define DXMEMORY_SMALL_MAX_SIZE			(64 * 1024)					// 小さいメモリ確保用のヒープを使用する最大サイズ

// 構造体定義 --------------------------------------------------------------------

// Windows用ヒープの情報
struct PLATFORMHEAP_WIN
{
	void *					AllocAddress ;						// 確保したメモリ領域のアドレス
	size_t					AllocSize ;							// 確保したメモリ領域のサイズ
} ;

// テーブル-----------------------------------------------------------------------

// 内部大域変数宣言 --------------------------------------------------------------

// 関数プロトタイプ宣言-----------------------------------------------------------

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // __DXMEMORYWIN_H__
