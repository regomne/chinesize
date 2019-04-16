// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		マスク処理プログラム( Direct3D11 )ヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXMASKD3D11_H__
#define __DXMASKD3D11_H__

#include "../DxCompileConfig.h"

#ifndef DX_NON_GRAPHICS

#ifndef DX_NON_DIRECT3D11

#ifndef DX_NON_MASK

// インクルード ------------------------------------------------------------------
#include "../DxLib.h"
#include "../DxGraphics.h"
#include "../DxArchive_.h"
#include "DxGraphicsAPIWin.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

#define MASKD3D11					MaskManageData_D3D11


// 構造体定義 --------------------------------------------------------------------


// Direct3D11 を使ったグラフィックス処理情報の構造体
struct MASKMANAGEDATA_DIRECT3D11
{
	int								MaskTextureSizeX ;				// マスクテクスチャのサイズ
	int								MaskTextureSizeY ;

	D_ID3D11Texture2D *				MaskScreenTexture ;				// マスク処理用仮画面テクスチャ
	D_ID3D11ShaderResourceView *	MaskScreenTextureSRV ;			// マスク処理用仮画面テクスチャシェーダーリソースビュー
	D_ID3D11RenderTargetView *		MaskScreenTextureRTV ;			// マスク処理用仮画面テクスチャレンダリングターゲットビュー

	D_ID3D11Texture2D *				MaskScreenTextureOld ;			// 古いマスク処理用仮画面テクスチャ
	D_ID3D11ShaderResourceView *	MaskScreenTextureOldSRV ;		// 古いマスク処理用仮画面テクスチャシェーダーリソースビュー
	D_ID3D11RenderTargetView *		MaskScreenTextureOldRTV ;		// 古いマスク処理用仮画面テクスチャレンダリングターゲットビュー

	D_ID3D11Texture2D *				MaskImageTexture ;				// マスクイメージテクスチャ
	D_ID3D11ShaderResourceView *	MaskImageTextureSRV ;			// マスクイメージテクスチャシェーダーリソースビュー

	D_ID3D11Texture2D *				DestTargetTexture2D ;			// 描画対象のテクスチャ
	D_ID3D11ShaderResourceView *	DestTargetSRV ;					// 描画対象のシェーダーリソースビュー
	D_ID3D11RenderTargetView *		DestTargetRTV ;					// 描画対象のレンダリングターゲットビュー

	D_ID3D11Texture2D *				DestTargetDepthTexture2D ;		// 描画対象の深度テクスチャ
	D_ID3D11DepthStencilView *		DestTargetDepthDSV ;			// 描画対象の深度ステンシルビュー
} ;


// 内部大域変数宣言 --------------------------------------------------------------

extern MASKMANAGEDATA_DIRECT3D11 MaskManageData_D3D11 ;

// 関数プロトタイプ宣言-----------------------------------------------------------


// 環境依存関数
extern	int			Mask_D3D11_CreateScreenFunction_Timing0_PF( void ) ;														// マスクスクリーンを作成する関数
extern	int			Mask_D3D11_CreateScreenFunction_Timing1_PF( int Width, int Height ) ;									// マスクスクリーンを作成する関数
extern	int			Mask_D3D11_CreateScreenFunction_Timing2_PF( int MaskBufferSizeXOld, int MaskBufferSizeYOld ) ;			// マスクスクリーンを作成する関数
extern	int			Mask_D3D11_ReleaseSurface_PF( void ) ;																	// マスクスクリーンを一時削除する
extern	int			Mask_D3D11_SetUseMaskScreenFlag_PF( void ) ;																// マスク使用モードを変更
extern	int			Mask_D3D11_DrawBeginFunction_PF( RECT *Rect ) ;															// マスクを使用した描画の前に呼ぶ関数( ついでにサブバッファを使用した描画エリア機能を使用している場合の処理もいれてしまっているよ )
extern	int			Mask_D3D11_DrawAfterFunction_PF( RECT *Rect ) ;															// マスクを使用した描画の後に呼ぶ関数( ついでにサブバッファを使用した描画エリア機能を使用している場合の処理もいれてしまっているよ )
extern	int			Mask_D3D11_FillMaskScreen_PF( int Flag ) ;																// マスクスクリーンを指定の色で塗りつぶす
extern	int			Mask_D3D11_UpdateMaskImageTexture_PF( RECT *Rect ) ;															// 指定領域のマスクイメージテクスチャを更新する


#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_NON_MASK

#endif // DX_NON_DIRECT3D11

#endif // DX_NON_GRAPHICS

#endif // __DXMASKD3D11_H__

