// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		マスク処理プログラム( Direct3D9 )ヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXMASKD3D9_H__
#define __DXMASKD3D9_H__

#include "../DxCompileConfig.h"

#ifndef DX_NON_GRAPHICS

#ifndef DX_NON_DIRECT3D9

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

#define MASKD3D9					MaskManageData_D3D9


// 構造体定義 --------------------------------------------------------------------


// Direct3D9 を使ったグラフィックス処理情報の構造体
struct MASKMANAGEDATA_DIRECT3D9
{
	int						MaskTextureSizeX, MaskTextureSizeY ;	// マスクテクスチャのサイズ
	D_IDirect3DTexture9		*MaskImageTexture ;						// マスクイメージテクスチャ
//	D_IDirect3DSurface9		*MaskImageSurface ;						// マスクイメージサーフェス
//	D_IDirect3DSurface9		*MaskMemImageSurface ;					// マスクメモリイメージサーフェス
	D_IDirect3DTexture9		*MaskScreenTexture ;					// マスクスクリーンテクスチャ
	D_IDirect3DSurface9		*MaskScreenSurface ;					// マスクスクリーンサーフェス

	D_IDirect3DTexture9		*MaskImageTextureOld ;
//	D_IDirect3DSurface9		*MaskImageSurfaceOld ;
//	D_IDirect3DSurface9		*MaskMemImageSurfaceOld ;
	D_IDirect3DTexture9		*MaskScreenTextureOld ;
	D_IDirect3DSurface9		*MaskScreenSurfaceOld ;

	D_IDirect3DSurface9		*UseBackupSurface ;
	D_IDirect3DSurface9		*DestTargetSurface ;
} ;


// 内部大域変数宣言 --------------------------------------------------------------

extern MASKMANAGEDATA_DIRECT3D9 MaskManageData_D3D9 ;

// 関数プロトタイプ宣言-----------------------------------------------------------


// 環境依存関数
extern	int			Mask_D3D9_CreateScreenFunction_Timing0_PF( void ) ;														// マスクスクリーンを作成する関数
extern	int			Mask_D3D9_CreateScreenFunction_Timing1_PF( int Width, int Height ) ;									// マスクスクリーンを作成する関数
extern	int			Mask_D3D9_CreateScreenFunction_Timing2_PF( int MaskBufferSizeXOld, int MaskBufferSizeYOld ) ;			// マスクスクリーンを作成する関数
extern	int			Mask_D3D9_ReleaseSurface_PF( void ) ;																	// マスクスクリーンを一時削除する
extern	int			Mask_D3D9_SetUseMaskScreenFlag_PF( void ) ;																// マスク使用モードを変更
extern	int			Mask_D3D9_DrawBeginFunction_PF( RECT *Rect ) ;															// マスクを使用した描画の前に呼ぶ関数( ついでにサブバッファを使用した描画エリア機能を使用している場合の処理もいれてしまっているよ )
extern	int			Mask_D3D9_DrawAfterFunction_PF( RECT *Rect ) ;															// マスクを使用した描画の後に呼ぶ関数( ついでにサブバッファを使用した描画エリア機能を使用している場合の処理もいれてしまっているよ )
extern	int			Mask_D3D9_FillMaskScreen_PF( int Flag ) ;																// マスクスクリーンを指定の色で塗りつぶす
extern	int			Mask_D3D9_UpdateMaskImageTexture_PF( RECT *Rect ) ;															// 指定領域のマスクイメージテクスチャを更新する


#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_NON_MASK

#endif // DX_NON_DIRECT3D9

#endif // DX_NON_GRAPHICS

#endif // __DXMASKD3D9_H__

