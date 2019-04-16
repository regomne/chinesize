// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		GraphFilter系プログラムヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXGRAPHICSFILTER_H__
#define __DXGRAPHICSFILTER_H__

#include "DxCompileConfig.h"

#ifndef DX_NON_FILTER

// インクルード ------------------------------------------------------------------
#include "DxLib.h"
#include "DxGraphics.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------


// 構造体定義 --------------------------------------------------------------------

// フィルター情報構造体
struct GRAPHFILTER_INFO
{
	int						IsBlend ;
	int						FilterOrBlendType ;
	int						SrcGrHandle ;
	int						BlendGrHandle ;
	int						DestGrHandle ;
	int						SrcEqualDestClearFlag ;
	float					BlendRatio ;
	int						Pass ;
	int						SrcX1 ;
	int						SrcY1 ;
	int						SrcX2 ;
	int						SrcY2 ;
	int						SrcDivNum ;
	int						BlendX ;
	int						BlendY ;
	int						BlendPosEnable ;
	int						DestX ;
	int						DestY ;

	int						PassNum ;
	int						UseWorkScreen ;

	int						DestWidth ;
	int						DestHeight ;
	BASEIMAGE				SrcBaseImage ;
	BASEIMAGE				BlendBaseImage ;
	BASEIMAGE				DestBaseImage ;

	IMAGEDATA				*SrcImage ;
	IMAGEDATA				*DestImage ;
	IMAGEDATA				*BlendImage ;
	IMAGEDATA				*TargetScreenImage ;
} ;

// フィルター毎情報
struct GRAPHFILTER_PARAM
{
	int						Gauss_PixelWidth ;
	float					Gauss_Param ;
	int						Bright_Clip_CmpType ;
	float					Bright_Clip_CmpParam ;
	int						Bright_Clip_ClipFillFlag ;
	COLOR_F					Bright_Clip_ClipFillColor ;
	int						Bright_Scale_Min ;
	int						Bright_Scale_Max ;
	int						Hsb_HueType ;
	float					Hsb_Hue ;
	float					Hsb_Saturation ;
	float					Hsb_Bright ;
	float					Mono_Cb ;
	float					Mono_Cr ;
	float					Lv_Min ;
	float					Lv_Max ;
	float					Lv_Gamma ;
	float					Lv_AfterMin ;
	float					Lv_AfterMax ;
	float					TC_Threshold ;
	COLOR_F					TC_LowColor ;
	COLOR_F					TC_HighColor ;
	int						GM_MapGrHandle ;
	int						GM_Reverse ;
	int						UVGrHandle ;
	int						DestSizeX ;
	int						DestSizeY ;

	int						RGBA_R ;
	int						RGBA_G ;
	int						RGBA_B ;
	int						RGBA_A ;
} ;

// フィルターのシェーダーハンドル
struct GRAPHFILTER_SHADER_HANDLE
{
	int						Gauss_PS[ 3 ] ;							// ガウスフィルタのピクセルシェーダー
	int						BrightClipPS[ 2 ][ 2 ][ 2 ] ;			// 明るさでクリップするフィルタのピクセルシェーダー[ 1:一定以上クリップ  0:一定以下クリップ ][ 1:クリップ塗りつぶしあり 0:塗りつぶしなし ][ 0:通常用  1:乗算済みアルファ用 ]
	int						BrightScalePS[ 2 ] ;					// 指定の明るさの領域を拡大するフィルタのピクセルシェーダー[ 0:通常用  1:乗算済みアルファ用 ]
	int						DownScalePS[ 3 ] ;						// 縮小フィルター( 0:X2 1:X4 2:X8 )
	int						HsbPS[ 3 ][ 2 ] ;						// ＨＳＢフィルター[ 0:RGB to HSI  1:HSI to RGB  2:HSB の HLock ][ 0:通常用  1:乗算済みアルファ用 ]
	int						MonoPS ;								// モノトーンフィルター
	int						InvertPS[ 2 ] ;							// 階調反転フィルター[ 0:通常用  1:乗算済みアルファ用 ]
	int						LevelPS[ 2 ] ;							// レベル補正フィルター[ 0:通常用  1:乗算済みアルファ用 ]
	int						GammaTex ;								// ガンマ補正に使用する画像
	float					PrevGamma ;								// 前回のガンマフィルタの際のガンマ値
	int						TwoColorPS[ 2 ] ;						// ２値化フィルター[ 0:通常用  1:乗算済みアルファ用 ]
	int						GradientMapPS[ 2 ][ 2 ] ;				// グラデーションマップフィルター[ 0:通常用  1:乗算済みアルファ用 ]
	int						PreMulAlphaPS ;							// 通常画像から乗算済みアルファ画像を作成する為のフィルター
	int						InterpAlphaPS ;							// 乗算済みアルファ画像から通常画像を作成する為のフィルター
	int						YUVtoRGBPS[ 4 ] ;						// YUVカラーをRGBカラーに変換するフィルター
	int						BicubicPS ;								// バイキュービック補間フィルター
	int						Lanczos3PS ;							// Lanczos-3補間フィルター

	int						BasicBlendPS[ DX_GRAPH_BLEND_NUM ] ;	// 基本的なブレンドフィルター
	int						RgbaMixBasePS[ 2 ] ;					// RGBAMixブレンド基本[ 0:通常用  1:乗算済みアルファ用 ]
	int						RgbaMixSRGBB[ 4 ][ 2 ] ;				// RGBAMixブレンドの A だけ BRGBA の４つ[ 0:通常用  1:乗算済みアルファ用 ]
	int						RgbaMixSRRRB[ 4 ][ 4 ][ 2 ] ;			// RGBAMixブレンドの SYYY BX の１６こ [ Y ][ X ][ 0:通常用  1:乗算済みアルファ用 ]
	int						RgbaMixS[ 4 ][ 4 ][ 4 ][ 4 ][ 2 ] ;		// RGBAMixブレンドの S だけの組み合わせ256個[ R ][ G ][ B ][ A ][ 0:通常用  1:乗算済みアルファ用 ]
} ;

// 内部大域変数宣言 --------------------------------------------------------------

extern GRAPHFILTER_SHADER_HANDLE GraphFilterShaderHandle ;

// 関数プロトタイプ宣言-----------------------------------------------------------

extern int	GraphFilter_Initialize( void ) ;	// フィルター処理の初期化
extern int	GraphFilter_Terminate( void ) ;		// フィルター処理の後始末

extern int	GraphFilter_Mono(          GRAPHFILTER_INFO *Info, float Cb, float Cr ) ;
extern int	GraphFilter_Gauss(         GRAPHFILTER_INFO *Info, int PixelWidth, float Param ) ;
extern int	GraphFilter_Down_Scale(    GRAPHFILTER_INFO *Info, int DivNum ) ;
extern int	GraphFilter_Bright_Clip(   GRAPHFILTER_INFO *Info, int CmpType, float CmpParam, int ClipFillFlag, COLOR_F *ClipFillColor, int IsPMA ) ;
extern int	GraphFilter_Bright_Scale(  GRAPHFILTER_INFO *Info, int BrightMin, int BrightMax, int IsPMA ) ;
extern int	GraphFilter_HSB(           GRAPHFILTER_INFO *Info, int HueType, float Hue, float Saturation, float Bright, int IsPMA ) ;
extern int	GraphFilter_Invert(        GRAPHFILTER_INFO *Info, int IsPMA ) ;
extern int	GraphFilter_Level(         GRAPHFILTER_INFO *Info, float Min, float Max, float Gamma, float AfterMin, float AfterMax, int IsPMA ) ;
extern int	GraphFilter_TwoColor(      GRAPHFILTER_INFO *Info, float Threshold, COLOR_F *LowColor, COLOR_F *HighColor, int IsPMA ) ;
extern int	GraphFilter_GradientMap(   GRAPHFILTER_INFO *Info, int MapGrHandle, int Reverse, int IsPMA ) ;
extern int	GraphFilter_PremulAlpha(   GRAPHFILTER_INFO *Info ) ;
extern int	GraphFilter_InterpAlpha(   GRAPHFILTER_INFO *Info ) ;
extern int	GraphFilter_YUVtoRGB(      GRAPHFILTER_INFO *Info, int UVGrHandle ) ;
extern int	GraphFilter_BicubicScale(  GRAPHFILTER_INFO *Info, int DestSizeX, int DestSizeY ) ;
extern int	GraphFilter_Lanczos3Scale( GRAPHFILTER_INFO *Info, int DestSizeX, int DestSizeY ) ;

extern int	GraphFilter_RectBltBase( int IsBlend, int SrcGrHandle, int BlendGrHandle, int DestGrHandle, int BlendRatio, int FilterOrBlendType, int SrcX1, int SrcY1, int SrcX2, int SrcY2, int BlendX, int BlendY, int BlendPosEnable, int DestX, int DestY, va_list ParamList ) ;

extern int	GraphBlend_Basic(           GRAPHFILTER_INFO *Info, int IsPMA ) ;
extern int	GraphBlend_RGBA_Select_Mix( GRAPHFILTER_INFO *Info, int SelectR, int SelectG, int SelectB, int SelectA, int IsPMA ) ;




// 環境依存関係
extern int	GraphFilter_Initialize_PF( void ) ;
extern int	GraphFilter_Terminate_PF( void ) ;

extern int	GraphFilter_Mono_PF(          GRAPHFILTER_INFO *Info, float Cb, float Cr ) ;
extern int	GraphFilter_Gauss_PF(         GRAPHFILTER_INFO *Info, int PixelWidth, float Param, float *Table ) ;
extern int	GraphFilter_Down_Scale_PF(    GRAPHFILTER_INFO *Info, int DivNum ) ;
extern int	GraphFilter_Bright_Clip_PF(   GRAPHFILTER_INFO *Info, int CmpType, float CmpParam, int ClipFillFlag, COLOR_F *ClipFillColor, int IsPMA ) ;
extern int	GraphFilter_Bright_Scale_PF(  GRAPHFILTER_INFO *Info, int BrightMin, int BrightMax, int IsPMA ) ;
extern int	GraphFilter_HSB_PF(           GRAPHFILTER_INFO *Info, int HueType, float Hue, float Saturation, float Bright, int IsPMA ) ;
extern int	GraphFilter_Invert_PF(        GRAPHFILTER_INFO *Info, int IsPMA ) ;
extern int	GraphFilter_Level_PF(         GRAPHFILTER_INFO *Info, float Min, float Max, float Gamma, float AfterMin, float AfterMax, int IsPMA ) ;
extern int	GraphFilter_TwoColor_PF(      GRAPHFILTER_INFO *Info, float Threshold, COLOR_F *LowColor, COLOR_F *HighColor, int IsPMA ) ;
extern int	GraphFilter_GradientMap_PF(   GRAPHFILTER_INFO *Info, int MapGrHandle, int Reverse, int IsPMA ) ;
extern int	GraphFilter_PremulAlpha_PF(   GRAPHFILTER_INFO *Info ) ;
extern int	GraphFilter_InterpAlpha_PF(   GRAPHFILTER_INFO *Info ) ;
extern int	GraphFilter_YUVtoRGB_PF(      GRAPHFILTER_INFO *Info, int UVGrHandle ) ;
extern int	GraphFilter_BicubicScale_PF(  GRAPHFILTER_INFO *Info, int DestSizeX, int DestSizeY ) ;
extern int	GraphFilter_Lanczos3Scale_PF( GRAPHFILTER_INFO *Info, int DestSizeX, int DestSizeY ) ;
extern int	GraphFilter_RectBltBase_Timing0_PF( GRAPHFILTER_INFO *Info, GRAPHFILTER_PARAM *Param ) ;
extern int	GraphFilter_RectBltBase_Timing1_PF( void ) ;
extern int	GraphFilter_DestGraphSetup_PF(      GRAPHFILTER_INFO *Info, int *UseSrcGrHandle, int *UseDestGrHandle ) ;
extern int	GraphFilter_DestGraphUpdate_PF(     GRAPHFILTER_INFO *Info, int UseDestGrHandle, int FilterResult ) ;

extern int	GraphBlend_Basic_PF(           GRAPHFILTER_INFO *Info, int IsPMA ) ;
extern int	GraphBlend_RGBA_Select_Mix_PF( GRAPHFILTER_INFO *Info, int SelectR, int SelectG, int SelectB, int SelectA, int IsPMA ) ;

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_NON_FILTER

#endif // __DXGRAPHICSFILTER_H__

