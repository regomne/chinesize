// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		WindowsOS用モデルデータ制御プログラムヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXMODELWIN_H__
#define __DXMODELWIN_H__

// インクルード ------------------------------------------------------------------
#include "../DxCompileConfig.h"

#ifndef DX_NON_MODEL

#include "../DxLib.h"
#include "../DxModel.h"
#include "DxModelD3D9.h"
#include "DxModelD3D11.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

// 構造体定義 --------------------------------------------------------------------

// Windows用 頂点バッファ環境依存情報
struct MV1_VERTEXBUFFER_PF
{
	union
	{
		int									Dummy ;
#ifndef DX_NON_DIRECT3D11
		MV1_VERTEXBUFFER_DIRECT3D11			D3D11 ;		// Direct3D11用頂点バッファ情報
#endif // DX_NON_DIRECT3D11
#ifndef DX_NON_DIRECT3D9
		MV1_VERTEXBUFFER_DIRECT3D9			D3D9 ;		// Direct3D9用頂点バッファ情報
#endif // DX_NON_DIRECT3D9
	} ;
} ;

// Windows用 トライアングルリスト環境依存情報
struct MV1_TRIANGLE_LIST_PF
{
	union
	{
		int									Dummy ;
#ifndef DX_NON_DIRECT3D11
		MV1_TRIANGLE_LIST_DIRECT3D11		D3D11 ;		// Direct3D11用トライアングルリスト情報
#endif // DX_NON_DIRECT3D11
#ifndef DX_NON_DIRECT3D9
		MV1_TRIANGLE_LIST_DIRECT3D9			D3D9 ;		// Direct3D9用トライアングルリスト情報
#endif // DX_NON_DIRECT3D9
	} ;
} ;

// Windows用 トライアングルリスト基データ環境依存情報
struct MV1_TRIANGLE_LIST_BASE_PF
{
	union
	{
		int									Dummy ;
#ifndef DX_NON_DIRECT3D11
		MV1_TRIANGLE_LIST_BASE_DIRECT3D11	D3D11 ;		// Direct3D11用トライアングルリスト基データ情報
#endif // DX_NON_DIRECT3D11
#ifndef DX_NON_DIRECT3D9
		MV1_TRIANGLE_LIST_BASE_DIRECT3D9	D3D9 ;		// Direct3D9用トライアングルリスト基データ情報
#endif // DX_NON_DIRECT3D9
	} ;
} ;

// 内部大域変数宣言 --------------------------------------------------------------

// 関数プロトタイプ宣言-----------------------------------------------------------

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_NON_MODEL

#endif // __DXMODELWIN_H__
