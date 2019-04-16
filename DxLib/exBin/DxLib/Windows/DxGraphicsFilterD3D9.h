// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		GraphFilter系プログラム( Direct3D9 )ヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXGRAPHICSFILTERD3D9_H__
#define __DXGRAPHICSFILTERD3D9_H__

#include "../DxCompileConfig.h"

#ifndef DX_NON_GRAPHICS

#ifndef DX_NON_DIRECT3D9

// インクルード ------------------------------------------------------------------
#include "../DxLib.h"
#include "../DxGraphicsFilter.h"
#include "../DxArchive_.h"
#include "DxGraphicsD3D9.h"

#ifndef DX_NON_FILTER

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------


// 構造体定義 --------------------------------------------------------------------

// フィルター関数用のライブラリの設定情報を一時的に退避するために使う構造体
struct GRAPHFILTER_DRAWPARAMTEMP_DIRECT3D9
{
	D_IDirect3DSurface9 *	TargetSurface[ DX_RENDERTARGET_COUNT ] ;
	D_IDirect3DSurface9 *	DepthStencilSurfaece ;
	int						BlendMode ;
	int						BlendParam ;
	int						AlphaTestMode ;
	int						AlphaTestParam ;
	int						UseZBufferFlag3D ;
	int						DrawMode ;
	int						FogEnable ;
	int						LightEnable ;
} ;

// フィルター処理用の情報構造体
struct GRAPHFILTER_SYSTEMIFNO_DIRET3D9
{
	int									WorkDrawValidGrHandle[ 2 ][ 2 ][ 18 ][ 18 ][ 2 ] ;	// フィルター処理作業用描画可能テクスチャ[ 0:整数テクスチャ 1:浮動小数点テクスチャ ][ チャンネルビット深度  0:16bit  1:32bit ][ ２のｎ乗 ][ 作業用２枚 ]
	GRAPHFILTER_DRAWPARAMTEMP_DIRECT3D9	DrawParamTemp ;
} ;


// 内部大域変数宣言 --------------------------------------------------------------

extern GRAPHFILTER_SYSTEMIFNO_DIRET3D9 GraphFilterSystemInfoD3D9 ;

// 関数プロトタイプ宣言-----------------------------------------------------------

// 環境依存関係
extern int	GraphFilter_D3D9_Mono_PF(          GRAPHFILTER_INFO *Info, float Cb, float Cr ) ;
extern int	GraphFilter_D3D9_Gauss_PF(         GRAPHFILTER_INFO *Info, int PixelWidth, float Param, float *Table ) ;
extern int	GraphFilter_D3D9_Down_Scale_PF(    GRAPHFILTER_INFO *Info, int DivNum ) ;
extern int	GraphFilter_D3D9_Bright_Clip_PF(   GRAPHFILTER_INFO *Info, int CmpType, float CmpParam, int ClipFillFlag, COLOR_F *ClipFillColor, int IsPMA ) ;
extern int	GraphFilter_D3D9_Bright_Scale_PF(  GRAPHFILTER_INFO *Info, int BrightMin, int BrightMax, int IsPMA ) ;
extern int	GraphFilter_D3D9_HSB_PF(           GRAPHFILTER_INFO *Info, int HueType, float Hue, float Saturation, float Bright, int IsPMA ) ;
extern int	GraphFilter_D3D9_Invert_PF(        GRAPHFILTER_INFO *Info, int IsPMA ) ;
extern int	GraphFilter_D3D9_Level_PF(         GRAPHFILTER_INFO *Info, float Min, float Max, float Gamma, float AfterMin, float AfterMax, int IsPMA ) ;
extern int	GraphFilter_D3D9_TwoColor_PF(      GRAPHFILTER_INFO *Info, float Threshold, COLOR_F *LowColor, COLOR_F *HighColor, int IsPMA ) ;
extern int	GraphFilter_D3D9_GradientMap_PF(   GRAPHFILTER_INFO *Info, int MapGrHandle, int Reverse, int IsPMA ) ;
extern int	GraphFilter_D3D9_PremulAlpha_PF(   GRAPHFILTER_INFO *Info ) ;
extern int	GraphFilter_D3D9_InterpAlpha_PF(   GRAPHFILTER_INFO *Info ) ;
extern int	GraphFilter_D3D9_YUVtoRGB_PF(      GRAPHFILTER_INFO *Info, int UVGrHandle ) ;
extern int	GraphFilter_D3D9_BicubicScale_PF(  GRAPHFILTER_INFO *Info, int DestSizeX, int DestSizeY ) ;
extern int	GraphFilter_D3D9_Lanczos3Scale_PF( GRAPHFILTER_INFO *Info, int DestSizeX, int DestSizeY ) ;

//extern int	GraphFilter_D3D9_RectBltBase_PF( int IsBlend, int SrcGrHandle, int BlendGrHandle, int DestGrHandle, int BlendRatio, int FilterOrBlendType, int SrcX1, int SrcY1, int SrcX2, int SrcY2, int BlendX, int BlendY, int BlendPosEnable, int DestX, int DestY, va_list ParamList ) ;			// 画像のフィルター付き転送を行う( 可変引数情報付き )
//extern int	GraphFilter_D3D9_RectBltBase_Timing0_PF( int SrcGrHandle, int DestGrHandle, int BlendGrHandle, int IsBlend, IMAGEDATA *TargetScreenImage ) ;
//extern int	GraphFilter_D3D9_RectBltBase_PF(     GRAPHFILTER_INFO *Info, GRAPHFILTER_PARAM *Param ) ;			// 画像のフィルター付き転送を行う( 可変引数情報付き )

extern int	GraphFilter_D3D9_RectBltBase_Timing0_PF( GRAPHFILTER_INFO *Info, GRAPHFILTER_PARAM *Param ) ;
extern int	GraphFilter_D3D9_RectBltBase_Timing1_PF( void ) ;
extern int	GraphFilter_D3D9_DestGraphSetup_PF(      GRAPHFILTER_INFO *Info, int *UseSrcGrHandle, int *UseDestGrHandle ) ;
extern int	GraphFilter_D3D9_DestGraphUpdate_PF(     GRAPHFILTER_INFO *Info, int UseDestGrHandle, int FilterResult ) ;

extern int	GraphBlend_D3D9_Basic_PF(           GRAPHFILTER_INFO *Info, int IsPMA ) ;
extern int	GraphBlend_D3D9_RGBA_Select_Mix_PF( GRAPHFILTER_INFO *Info, int SelectR, int SelectG, int SelectB, int SelectA, int IsPMA ) ;

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_NON_FILTER

#endif // DX_NON_DIRECT3D9

#endif // DX_NON_GRAPHICS

#endif // __DXGRAPHICSFILTERD3D9_H__

