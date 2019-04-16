// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		演算プログラムヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXMATH_H__
#define __DXMATH_H__

// インクルード ------------------------------------------------------------------
#include "DxCompileConfig.h"
#include "DxLib.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

// 構造体定義 --------------------------------------------------------------------

// テーブル-----------------------------------------------------------------------

// 内部大域変数宣言 --------------------------------------------------------------

// 関数プロトタイプ宣言-----------------------------------------------------------

extern	void	HitCheck_Line_Triangle_Base(  HITRESULT_LINE *Result,   VECTOR   LinePos1, VECTOR   LinePos2, VECTOR   TrianglePos1, VECTOR   TrianglePos2, VECTOR   TrianglePos3, float  *TrianglePos1Weight, float  *TrianglePos2Weight, float  *TrianglePos3Weight ) ;		// 三角形と線の当たり判定
extern	void	HitCheck_Line_TriangleD_Base( HITRESULT_LINE_D *Result, VECTOR_D LinePos1, VECTOR_D LinePos2, VECTOR_D TrianglePos1, VECTOR_D TrianglePos2, VECTOR_D TrianglePos3, double *TrianglePos1Weight, double *TrianglePos2Weight, double *TrianglePos3Weight ) ;		// 三角形と線の当たり判定

extern	void	TriangleBarycenter_Base(  const VECTOR   *TrianglePos1, const VECTOR   *TrianglePos2, const VECTOR   *TrianglePos3, const VECTOR   *Position, float  *TrianglePos1Weight, float  *TrianglePos2Weight, float  *TrianglePos3Weight ) ;	// 指定の座標から三角形の重心を求める
extern	void	TriangleBarycenter_BaseD( const VECTOR_D *TrianglePos1, const VECTOR_D *TrianglePos2, const VECTOR_D *TrianglePos3, const VECTOR_D *Position, double *TrianglePos1Weight, double *TrianglePos2Weight, double *TrianglePos3Weight ) ;	// 指定の座標から三角形の重心を求める

extern VECTOR	Get_Triangle_Point_MinPosition(  VECTOR   Point, VECTOR   TrianglePos1, VECTOR   TrianglePos2, VECTOR   TrianglePos3 ) ;			// 点に一番近い三角形上の座標を得る
extern VECTOR_D	Get_Triangle_Point_MinPositionD( VECTOR_D Point, VECTOR_D TrianglePos1, VECTOR_D TrianglePos2, VECTOR_D TrianglePos3 ) ;			// 点に一番近い三角形上の座標を得る
extern VECTOR	Get_Line_Point_MinPosition(  VECTOR   Point, VECTOR   LinePos1, VECTOR   LinePos2 ) ;											// 点に一番近い線上の座標を得る
extern VECTOR_D	Get_Line_Point_MinPositionD( VECTOR_D Point, VECTOR_D LinePos1, VECTOR_D LinePos2 ) ;											// 点に一番近い線上の座標を得る
extern int		Get_TwoPlane_Line(  VECTOR   Normal1, float  Dist1, VECTOR   Normal2, float  Dist2, VECTOR   *OutDir, VECTOR   *OutPos ) ;			// 二つの平面が交差する線を得る( 戻り値が -1 の場合は、二つの面は平行 )
extern int		Get_TwoPlane_LineD( VECTOR_D Normal1, double Dist1, VECTOR_D Normal2, double Dist2, VECTOR_D *OutDir, VECTOR_D *OutPos ) ;			// 二つの平面が交差する線を得る( 戻り値が -1 の場合は、二つの面は平行 )
extern int		Get_TwoLine_MinLength_Rate(  VECTOR   Direction1, VECTOR   Position1, float  *OutRate1, VECTOR   Direction2, VECTOR   Position2, float  *OutRate2 ) ;		// 二つの線の最近点のそれぞれの方向ベクトルの拡大値を得る
extern int		Get_TwoLine_MinLength_RateD( VECTOR_D Direction1, VECTOR_D Position1, double *OutRate1, VECTOR_D Direction2, VECTOR_D Position2, double *OutRate2 ) ;		// 二つの線の最近点のそれぞれの方向ベクトルの拡大値を得る

extern void		CreateNormalizePlane(  FLOAT4  *Plane, VECTOR   *Position, VECTOR   *Normal ) ;												// 平面上の点と平面の法線から正規化された平面パラメータを算出する
extern void		CreateNormalizePlaneD( DOUBLE4 *Plane, VECTOR_D *Position, VECTOR_D *Normal ) ;												// 平面上の点と平面の法線から正規化された平面パラメータを算出する
extern void		SetMathScreenSize( int SizeX, int SizeY ) ;																				// DxMath.cpp で使用する画面のサイズを設定する

// 矩形のクリッピング
__inline int RectClipping_Inline( RECT *Rect, const RECT *ClippuRect )
{
	     if( Rect->left   > Rect->right        ) Rect->right  = Rect->left ;
	else if( Rect->right  < Rect->left         ) Rect->left   = Rect->right	;

		 if( Rect->top    > Rect->bottom	   ) Rect->bottom = Rect->top ;
	else if( Rect->bottom < Rect->top		   ) Rect->top    = Rect->bottom ;

		 if( Rect->bottom > ClippuRect->bottom ) Rect->bottom = ClippuRect->bottom ;
	else if( Rect->bottom < ClippuRect->top	   ) Rect->bottom = ClippuRect->top ;

		 if( Rect->top	  > ClippuRect->bottom ) Rect->top    = ClippuRect->bottom ;
	else if( Rect->top	  < ClippuRect->top    ) Rect->top    = ClippuRect->top ;


		 if( Rect->right  > ClippuRect->right  ) Rect->right  = ClippuRect->right ;
	else if( Rect->right  < ClippuRect->left   ) Rect->right  = ClippuRect->left ;

		 if( Rect->left	  > ClippuRect->right  ) Rect->left   = ClippuRect->right ;
	else if( Rect->left	  < ClippuRect->left   ) Rect->left   = ClippuRect->left ;

	return 0 ;
}

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // __DXMATH_H__
