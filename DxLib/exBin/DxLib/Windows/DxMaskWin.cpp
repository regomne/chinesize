//-----------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		WindowsOS用マスクデータ管理プログラム
// 
//  	Ver 3.20c
// 
//-----------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

#include "../DxCompileConfig.h"

#ifndef DX_NON_GRAPHICS

#ifndef DX_NON_MASK

// インクルード ---------------------------------------------------------------
#include "DxGraphicsWin.h"
#include "DxMaskWin.h"
#include "DxMaskD3D9.h"
#include "DxMaskD3D11.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 -----------------------------------------------------------------

// 構造体宣言 -----------------------------------------------------------------

// データ定義 -----------------------------------------------------------------

// 関数宣言 -------------------------------------------------------------------

// プログラム -----------------------------------------------------------------

// 環境依存関数

// マスクスクリーンを作成する関数
extern	int			Mask_CreateScreenFunction_Timing0_PF( void )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Mask_D3D11_CreateScreenFunction_Timing0_PF() ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Mask_D3D9_CreateScreenFunction_Timing0_PF() ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// マスクスクリーンを作成する関数
extern	int			Mask_CreateScreenFunction_Timing1_PF( int Width, int Height )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Mask_D3D11_CreateScreenFunction_Timing1_PF(  Width,  Height ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Mask_D3D9_CreateScreenFunction_Timing1_PF(  Width,  Height ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// マスクスクリーンを作成する関数
extern	int			Mask_CreateScreenFunction_Timing2_PF( int MaskBufferSizeXOld, int MaskBufferSizeYOld )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Mask_D3D11_CreateScreenFunction_Timing2_PF(  MaskBufferSizeXOld,  MaskBufferSizeYOld ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Mask_D3D9_CreateScreenFunction_Timing2_PF(  MaskBufferSizeXOld,  MaskBufferSizeYOld ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// マスクスクリーンを一時削除する
extern	int			Mask_ReleaseSurface_PF( void )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Mask_D3D11_ReleaseSurface_PF() ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Mask_D3D9_ReleaseSurface_PF() ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// マスク使用モードを変更
extern	int			Mask_SetUseMaskScreenFlag_PF( void )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Mask_D3D11_SetUseMaskScreenFlag_PF() ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Mask_D3D9_SetUseMaskScreenFlag_PF() ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// マスクを使用した描画の前に呼ぶ関数( ついでにサブバッファを使用した描画エリア機能を使用している場合の処理もいれてしまっているよ )
extern	int			Mask_DrawBeginFunction_PF( RECT *Rect )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Mask_D3D11_DrawBeginFunction_PF( Rect ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Mask_D3D9_DrawBeginFunction_PF( Rect ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// マスクを使用した描画の後に呼ぶ関数( ついでにサブバッファを使用した描画エリア機能を使用している場合の処理もいれてしまっているよ )
extern	int			Mask_DrawAfterFunction_PF( RECT *Rect )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Mask_D3D11_DrawAfterFunction_PF( Rect ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Mask_D3D9_DrawAfterFunction_PF( Rect ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// マスクスクリーンを指定の色で塗りつぶす
extern	int			Mask_FillMaskScreen_PF( int Flag )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Mask_D3D11_FillMaskScreen_PF( Flag ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Mask_D3D9_FillMaskScreen_PF( Flag ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 指定領域のマスクイメージテクスチャを更新する
extern	int			Mask_UpdateMaskImageTexture_PF( RECT *Rect )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Mask_D3D11_UpdateMaskImageTexture_PF( Rect ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Mask_D3D9_UpdateMaskImageTexture_PF( Rect ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}






#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_NON_MASK

#endif // DX_NON_GRAPHICS
