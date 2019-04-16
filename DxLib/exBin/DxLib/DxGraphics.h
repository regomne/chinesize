// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		描画プログラムヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXGRAPHICS_H__
#define __DXGRAPHICS_H__

#include "DxCompileConfig.h"

#ifndef DX_NON_GRAPHICS

// インクルード ------------------------------------------------------------------
#include "DxLib.h"
#include "DxStatic.h"
#include "DxHandle.h"
#include "DxMemImg.h"
#include "DxMovie.h"
#include "DxArchive_.h"
#include "DxBaseImage.h"
#include <stdarg.h>

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

#define GSYS						GraphicsSysData
#define MASKD						MaskManageData

// 記録しておく有効になっているライトのインデックスの数
#define MAX_HARDWAREENABLELIGHTINDEX_NUM	(256)

// 同時に描画できるレンダリングターゲットの最大数
#define DX_RENDERTARGET_COUNT				(8)

// 使用するテクスチャステージの最大数
#define USE_TEXTURESTAGE_NUM				(16)

// 同時に適用できるシャドウマップの最大数
#define MAX_USE_SHADOWMAP_NUM				(3)

// 共有バッファの数
#define COMMON_BUFFER_NUM					(3)

// 頂点タイプ
#define VERTEXTYPE_NOTEX					(0)			// テクスチャを使用しない
#define VERTEXTYPE_TEX						(1)			// テクスチャを一つ使用する
#define VERTEXTYPE_BLENDTEX					(2)			// ブレンドテクスチャを使用する
#define VERTEXTYPE_NUM						(3)

// グラフィックハンドルの有効性チェック
#define GRAPHCHKFULL( HAND, GPOINT )			HANDLECHKFULL(       DX_HANDLETYPE_GRAPH, HAND, *( ( HANDLEINFO ** )&GPOINT ) )
#define GRAPHCHKFULL_ASYNC( HAND, GPOINT )		HANDLECHKFULL_ASYNC( DX_HANDLETYPE_GRAPH, HAND, *( ( HANDLEINFO ** )&GPOINT ) )
#define GRAPHCHK( HAND, GPOINT )				HANDLECHK(           DX_HANDLETYPE_GRAPH, HAND, *( ( HANDLEINFO ** )&GPOINT ) )
#define GRAPHCHK_ASYNC( HAND, GPOINT )			HANDLECHK_ASYNC(     DX_HANDLETYPE_GRAPH, HAND, *( ( HANDLEINFO ** )&GPOINT ) )

// シャドウマップハンドルの有効性チェック
#define SHADOWMAPCHKFULL( HAND, SPOINT )		HANDLECHKFULL(       DX_HANDLETYPE_SHADOWMAP, HAND, *( ( HANDLEINFO ** )&SPOINT ) )
#define SHADOWMAPCHKFULL_ASYNC( HAND, SPOINT )	HANDLECHKFULL_ASYNC( DX_HANDLETYPE_SHADOWMAP, HAND, *( ( HANDLEINFO ** )&SPOINT ) )
#define SHADOWMAPCHK( HAND, SPOINT )			HANDLECHK(           DX_HANDLETYPE_SHADOWMAP, HAND, *( ( HANDLEINFO ** )&SPOINT ) )
#define SHADOWMAPCHK_ASYNC( HAND, SPOINT )		HANDLECHK_ASYNC(     DX_HANDLETYPE_SHADOWMAP, HAND, *( ( HANDLEINFO ** )&SPOINT ) )

// シェーダーハンドルの有効性チェック
#define SHADERCHK( HAND, SPOINT )				HANDLECHK(       DX_HANDLETYPE_SHADER, HAND, *( ( HANDLEINFO ** )&SPOINT ) )
#define SHADERCHK_ASYNC( HAND, SPOINT )			HANDLECHK_ASYNC( DX_HANDLETYPE_SHADER, HAND, *( ( HANDLEINFO ** )&SPOINT ) )

// シェーダー用定数バッファハンドルの有効性チェック
#define SHADERCONSTANTBUFFERCHK( HAND, SPOINT )				HANDLECHK(       DX_HANDLETYPE_SHADER_CONSTANT_BUFFER, HAND, *( ( HANDLEINFO ** )&SPOINT ) )
#define SHADERCONSTANTBUFFERCHK_ASYNC( HAND, SPOINT )		HANDLECHK_ASYNC( DX_HANDLETYPE_SHADER_CONSTANT_BUFFER, HAND, *( ( HANDLEINFO ** )&SPOINT ) )

// 頂点バッファハンドルの有効性チェック
#define VERTEXBUFFERCHK( HAND, SPOINT )			HANDLECHK(       DX_HANDLETYPE_VERTEX_BUFFER, HAND, *( ( HANDLEINFO ** )&SPOINT ) )
#define VERTEXBUFFERCHK_ASYNC( HAND, SPOINT )	HANDLECHK_ASYNC( DX_HANDLETYPE_VERTEX_BUFFER, HAND, *( ( HANDLEINFO ** )&SPOINT ) )

// インデックスバッファハンドルの有効性チェック
#define INDEXBUFFERCHK( HAND, SPOINT )			HANDLECHK(       DX_HANDLETYPE_INDEX_BUFFER, HAND, *( ( HANDLEINFO ** )&SPOINT ) )
#define INDEXBUFFERCHK_ASYNC( HAND, SPOINT )	HANDLECHK_ASYNC( DX_HANDLETYPE_INDEX_BUFFER, HAND, *( ( HANDLEINFO ** )&SPOINT ) )

// カメラの設定タイプ
#define CAMERA_SETUPTYPE_POS_TARG		(0)
#define CAMERA_SETUPTYPE_POS_TARG_UP	(1)
#define CAMERA_SETUPTYPE_POS_ANGLE		(2)
#define CAMERA_SETUPTYPE_MATRIX			(3)

// Ｚバッファフォーマット
#define ZBUFFER_FORMAT_16BIT		(0)
#define ZBUFFER_FORMAT_24BIT		(1)
#define ZBUFFER_FORMAT_32BIT		(2)
#define ZBUFFER_FORMAT_NUM			(3)

// マテリアル別パラメータの最大数
#define MATERIAL_TYPEPARAM_MAX_NUM	(4)

// 描画設定タイプ
#define DX_DRAWSETTING_2D			(0)			// 通常の２Ｄ描画用の設定
#define DX_DRAWSETTING_SHADER		(1)			// シェーダーを使った描画用の設定
#define DX_DRAWSETTING_MASK			(2)			// マスク処理用の設定
#define DX_DRAWSETTING_TOONOUTLINE	(3)			// トゥーンレンダリングモデルの輪郭線描画用の設定
#define DX_DRAWSETTING_MODEL		(4)			// モデル描画用の設定
#define DX_DRAWSETTING_NUM			(5)			// 描画設定タイプの数

// シェーダー定数セット
#define DX_SHADERCONSTANTSET_LIB			0
#define DX_SHADERCONSTANTSET_LIB_SUB		1
#define DX_SHADERCONSTANTSET_MV1			2
#define DX_SHADERCONSTANTSET_USER			3
#define DX_SHADERCONSTANTSET_NUM			4

// シェーダー定数タイプ
#define DX_SHADERCONSTANTTYPE_VS_FLOAT		0
#define DX_SHADERCONSTANTTYPE_VS_INT		1
#define DX_SHADERCONSTANTTYPE_VS_BOOL		2
#define DX_SHADERCONSTANTTYPE_PS_FLOAT		3
#define DX_SHADERCONSTANTTYPE_PS_INT		4
#define DX_SHADERCONSTANTTYPE_PS_BOOL		5
#define DX_SHADERCONSTANTTYPE_NUM			6

// マスク定型処理
#ifdef DX_NON_MASK
#define MASK_FULLSCREEN_MASKUPDATE
#else
#define MASK_FULLSCREEN_MASKUPDATE					\
	if( MASKD.FullScreenMaskUpdateFlag )			\
	{												\
		RECT MaskRect ;								\
		MaskRect.left   = GSYS.DrawSetting.DrawArea.left ;		\
		MaskRect.right  = GSYS.DrawSetting.DrawArea.right ;	\
		MaskRect.top    = GSYS.DrawSetting.DrawArea.top ;		\
		MaskRect.bottom = GSYS.DrawSetting.DrawArea.bottom ;	\
		MASK_END( MaskRect )						\
		MASKD.FullScreenMaskUpdateFlag = FALSE ;	\
	}
#endif

// 描画範囲に入っているかどうかの判定( 0 だったら描画領域に入っていない部分がある )
#define IN_DRAWAREA_CHECK( x1, y1, x2, y2 )	\
		( (DWORD)( ( (x2) - ( GSYS.DrawSetting.DrawArea.right  + 1 ) ) & ( GSYS.DrawSetting.DrawArea.left - ( (x1) + 1 ) ) &					\
		           ( (y2) - ( GSYS.DrawSetting.DrawArea.bottom + 1 ) ) & ( GSYS.DrawSetting.DrawArea.top  - ( (y1) + 1 ) ) ) & 0x80000000 )

// 描画範囲から完全に外れているかどうかの判定( 0 じゃなかったら完全にはずれている )
#define OUT_DRAWAREA_CHECK( x1, y1, x2, y2 ) \
		( (DWORD)( ( GSYS.DrawSetting.DrawArea.right  - ( (x1) + 1 ) ) & ( (x2) - ( GSYS.DrawSetting.DrawArea.left + 1 ) ) &		\
		           ( GSYS.DrawSetting.DrawArea.bottom - ( (y1) + 1 ) ) & ( (y2) - ( GSYS.DrawSetting.DrawArea.top  + 1 ) ) ) & 0x80000000 )

#define DRAWRECT_DRAWAREA_CLIP\
		if( IN_DRAWAREA_CHECK( DrawRect.left, DrawRect.top, DrawRect.right, DrawRect.bottom ) == 0 )\
		{\
			if( DrawRect.left   < GSYS.DrawSetting.DrawArea.left   ) DrawRect.left   = GSYS.DrawSetting.DrawArea.left   ;\
			if( DrawRect.right  < GSYS.DrawSetting.DrawArea.left   ) DrawRect.right  = GSYS.DrawSetting.DrawArea.left   ;\
			if( DrawRect.left   > GSYS.DrawSetting.DrawArea.right  ) DrawRect.left   = GSYS.DrawSetting.DrawArea.right  ;\
			if( DrawRect.right  > GSYS.DrawSetting.DrawArea.right  ) DrawRect.right  = GSYS.DrawSetting.DrawArea.right  ;\
			if( DrawRect.top    < GSYS.DrawSetting.DrawArea.top    ) DrawRect.top    = GSYS.DrawSetting.DrawArea.top    ;\
			if( DrawRect.bottom < GSYS.DrawSetting.DrawArea.top    ) DrawRect.bottom = GSYS.DrawSetting.DrawArea.top    ;\
			if( DrawRect.top    > GSYS.DrawSetting.DrawArea.bottom ) DrawRect.top    = GSYS.DrawSetting.DrawArea.bottom ;\
			if( DrawRect.bottom > GSYS.DrawSetting.DrawArea.bottom ) DrawRect.bottom = GSYS.DrawSetting.DrawArea.bottom ;\
		}

#ifdef DX_NON_MASK
#define MASK_BEGIN( RECT )
#else
#define MASK_BEGIN( RECT )		if( MASKD.MaskValidFlag ) Mask_DrawBeginFunction( (RECT) ) ;
#endif

#ifdef DX_NON_MASK
#define MASK_END( RECT )
#else
#define MASK_END( RECT )		if( MASKD.MaskValidFlag ) Mask_DrawAfterFunction( (RECT) ) ;
#endif

// 描画定型処理
#ifdef DX_NON_2DDRAW
	#ifdef DX_NON_MASK
		#define DRAW_DEF( FUNC3D, FUNC2D, SETRECT, RET, USE3DFLAG )\
			{\
				if( GSYS.DrawSetting.BlendMode == DX_BLENDMODE_SUB && GSYS.HardInfo.ValidSubBlend == FALSE )\
				{\
					RECT DrawRect ;\
					SETRECT\
					Graphics_DrawSetting_BlendModeSub_Pre( &DrawRect ) ;\
					RET = (FUNC3D) ;\
					Graphics_DrawSetting_BlendModeSub_Post( &DrawRect ) ;\
				}\
				else\
				{\
					RET = (FUNC3D) ;\
				}\
			}
	#else // DX_NON_MASK
		#define DRAW_DEF( FUNC3D, FUNC2D, SETRECT, RET, USE3DFLAG )\
			{\
				bool UseSubMode = GSYS.DrawSetting.BlendMode == DX_BLENDMODE_SUB && GSYS.HardInfo.ValidSubBlend == FALSE ;\
				if( UseSubMode || MASKD.MaskValidFlag )\
				{\
					RECT DrawRect ;\
					SETRECT\
					MASK_BEGIN( DrawRect ) ;\
					if( UseSubMode )\
					{\
						Graphics_DrawSetting_BlendModeSub_Pre( &DrawRect ) ;\
						RET = (FUNC3D) ;\
						Graphics_DrawSetting_BlendModeSub_Post( &DrawRect ) ;\
					}\
					else\
					{\
						RET = (FUNC3D) ;\
					}\
					\
					MASK_END( DrawRect ) ;\
				}\
				else\
				{\
					RET = (FUNC3D) ;\
				}\
			}
	#endif // DX_NON_MASK
#else // DX_NON_2DDRAW
	#ifdef DX_NON_MASK
		#define DRAW_DEF( FUNC3D, FUNC2D, SETRECT, RET, USE3DFLAG )\
			{\
				if( GSYS.DrawSetting.BlendMode == DX_BLENDMODE_SUB && GSYS.HardInfo.ValidSubBlend == FALSE && (USE3DFLAG) )\
				{\
					RECT DrawRect ;\
					SETRECT\
					Graphics_DrawSetting_BlendModeSub_Pre( &DrawRect ) ;\
					RET = (FUNC3D) ;\
					Graphics_DrawSetting_BlendModeSub_Post( &DrawRect ) ;\
				}\
				else\
				{\
					RET = (USE3DFLAG) ? (FUNC3D) : (FUNC2D) ;\
				}\
			}
	#else // DX_NON_MASK
		#define DRAW_DEF( FUNC3D, FUNC2D, SETRECT, RET, USE3DFLAG )\
			{\
				bool UseSubMode = GSYS.DrawSetting.BlendMode == DX_BLENDMODE_SUB && GSYS.HardInfo.ValidSubBlend == FALSE && (USE3DFLAG) ;\
				if( UseSubMode || MASKD.MaskValidFlag )\
				{\
					RECT DrawRect ;\
					SETRECT\
					MASK_BEGIN( DrawRect ) ;\
					if( UseSubMode )\
					{\
						Graphics_DrawSetting_BlendModeSub_Pre( &DrawRect ) ;\
						RET = (FUNC3D) ;\
						Graphics_DrawSetting_BlendModeSub_Post( &DrawRect ) ;\
					}\
					else\
					{\
						RET = (USE3DFLAG) ? (FUNC3D) : (FUNC2D) ;\
					}\
					\
					MASK_END( DrawRect ) ;\
				}\
				else\
				{\
					RET = (USE3DFLAG) ? (FUNC3D) : (FUNC2D) ;\
				}\
			}
	#endif // DX_NON_MASK
#endif // DX_NON_2DDRAW

// 描画待機している頂点やモデルを描画する
#ifndef DX_NON_MODEL
	#define MV1DRAWPACKDRAWMODEL		if( MV1Man.PackDrawModel != NULL ) MV1DrawPackDrawModel() ;
#else	// DX_NON_MODEL
	#define MV1DRAWPACKDRAWMODEL
#endif	// DX_NON_MODEL

#define DRAWSTOCKINFO_ASYNC( ASYNCFLAG )	\
	Graphics_Hardware_RenderVertex( ASYNCFLAG ) ;	\
	MV1DRAWPACKDRAWMODEL

#define DRAWSTOCKINFO			\
	Graphics_Hardware_RenderVertex() ;	\
	MV1DRAWPACKDRAWMODEL


// 構造体定義 --------------------------------------------------------------------

// ライブラリ内部で描画対象を変更する際のカメラなどの設定を保存しておくための構造体
typedef struct tagSCREENDRAWSETTINGINFO
{
	int						Use3DFlag ;
	int						UseSysMemFlag ;
	int						BackgroundRed ;
	int						BackgroundGreen ;
	int						BackgroundBlue ;
	int						DrawBlendMode ;
	int						DrawBlendParam ;
	int						DrawMode ;
	int						WaitVSync ;
	int						DrawScreen ;
	MATRIX_D				ViewMatrix ;
	MATRIX_D				ProjectionMatrix ;
	MATRIX_D				ViewportMatrix ;
	MATRIX_D				WorldMatrix ;
	int						ProjectionMatrixMode ;
	double					ProjFov ;
	double					ProjSize ;
	double					ProjNear ;
	double					ProjFar ;
	double					ProjDotAspect ;
	MATRIX_D				ProjMatrix ;
	RECT					DrawRect ;
} SCREENDRAWSETTINGINFO ;

// Graphics_Draw_DrawSimpleTriangleGraphF, Graphics_Draw_DrawSimpleQuadrangleGraphF 用構造体
typedef struct tagGRAPHICS_DRAW_DRAWSIMPLEANGLEGRAPHF_VERTEX
{
	float					x, y, u, v ;
} GRAPHICS_DRAW_DRAWSIMPLEANGLEGRAPHF_VERTEX ;
typedef struct tagGRAPHICS_DRAW_DRAWSIMPLETRIANGLEGRAPHF_PARAM
{
	int						TriangleNum ;
	GRAPHICS_DRAW_DRAWSIMPLEANGLEGRAPHF_VERTEX *Vertex ;
	int						GraphHandle ;
	int						TransFlag ;
} GRAPHICS_DRAW_DRAWSIMPLETRIANGLEGRAPHF_PARAM ;
typedef struct tagGRAPHICS_DRAW_DRAWSIMPLEQUADRANGLEGRAPHF_PARAM
{
	int						QuadrangleNum ;
	GRAPHICS_DRAW_DRAWSIMPLEANGLEGRAPHF_VERTEX *Vertex ;
	int						GraphHandle ;
	int						TransFlag ;
} GRAPHICS_DRAW_DRAWSIMPLEQUADRANGLEGRAPHF_PARAM ;

// テクスチャ座標8個頂点構造体( バイキュービック補間描画用 )
typedef struct tagVERTEX_TEX8_2D
{
	VECTOR					pos ;
	float					rhw ;
	unsigned int			color ;
	float					u0, v0 ;
	float					u1, v1 ;
	float					u2, v2 ;
	float					u3, v3 ;
	float					u4, v4 ;
	float					u5, v5 ;
	float					u6, v6 ;
	float					u7, v7 ;
} VERTEX_TEX8_2D, *LPVERTEX_TEX8_2D ; 

// ２Ｄ描画用頂点構造体(テクスチャ無し)
typedef struct tagVERTEX_NOTEX_2D
{
	VECTOR					pos ;
	float					rhw ;
	unsigned int			color ;
} VERTEX_NOTEX_2D, *LPVERTEX_NOTEX_2D ; 

// 主に２Ｄ描画に使用する頂点データ型
typedef struct tagVERTEX_2D
{
	VECTOR					pos ;
	float					rhw ;
	unsigned int			color ;
	float					u, v ;
} VERTEX_2D, *LPVERTEX_2D ; 

// ブレンド画像付き２Ｄ画像描画用頂点構造体
struct VERTEX_BLENDTEX_2D
{
	VECTOR					pos ;
	float					rhw ;
	unsigned int			color ;
	unsigned int			specular ;
	float					u1, v1 ;
	float					u2, v2 ;
} ;

// ３Ｄ描画に使用する頂点データ型( テクスチャなし )( 旧バージョンのもの )
typedef struct tagVERTEX_NOTEX_3D
{
	VECTOR					pos ;
	unsigned char			b, g, r, a ;
} VERTEX_NOTEX_3D, *LPVERTEX_NOTEX_3D ;

// ソフトウエアレンダリング用オリジナル画像情報
struct IMAGEDATA_ORIG_SOFT
{
	MEMIMG					MemImg ;						// MEMIMG
	MEMIMG					*ZBuffer ;						// ZBuffer
} ;

// ハードウエアレンダリング用オリジナル画像テクスチャ情報
struct IMAGEDATA_ORIG_HARD_TEX
{
	struct IMAGEDATA_ORIG_HARD_TEX_PF	*PF ;				// 環境依存データ
	int						OrigPosX, OrigPosY ;			// 元画像で使用している領域の左上座標
	int						UseWidth, UseHeight ;			// 元画像で使用している領域のサイズ
	int						TexWidth, TexHeight ;			// テクスチャ自体の幅と高さ
} ;

// ハードウエアレンダリング用オリジナル画像情報
struct IMAGEDATA_ORIG_HARD
{
	int						MipMapCount ;					// ミップマップの数
	int						TexNum ;						// テクスチャの数
	IMAGEDATA_ORIG_HARD_TEX Tex[ 4 ] ;						// テクスチャリストへのポインタ
} ;

// オリジナル画像情報構造体
struct IMAGEDATA_ORIG
{
	int						RefCount ;						// 参照されている数
	int						ColorFormat ;					// カラーフォーマット( DX_GRAPHICSIMAGE_FORMAT_3D_RGB16 等 )
	IMAGEFORMATDESC			FormatDesc ;					// フォーマット
	unsigned int			TransCode ;						// 透過色コード
	int						Width, Height ;					// 画像のサイズ
	int						ZBufferFlag ;					// Ｚバッファがあるかどうか( TRUE:ある  FALSE:ない )
	int						ZBufferBitDepthIndex ;			// Ｚバッファビット深度インデックス
	int						RestoreFlag ;					// 画像復帰時に使用するフラグ( TRUE:復帰が完了している  FALSE:復帰が完了していない )

	void *					UserPlatformTexture ;			// ユーザー指定の環境依存テクスチャオブジェクトのアドレス

	union
	{
		IMAGEDATA_ORIG_SOFT	Soft ;						// ソフトウエアレンダリング用情報
		IMAGEDATA_ORIG_HARD	Hard ;						// ハードウエアレンダリング用情報
	} ;
} ;

// ソフトウエアレンダリング版イメージデータ構造体
struct IMAGEDATA_SOFT
{
	MEMIMG					MemImg ;						// 描画用 MEMIMG
} ;

// ハードウエアレンダリング版座標情報構造体
struct IMAGEDATA_HARD_VERT
{
	float					x, y ;							// 位置
	float					u, v ;							// ＵＶ位置
} ;

// ハードウエアレンダリング版描画用構造体
struct IMAGEDATA_HARD_DRAW
{
	int						DrawPosXI, DrawPosYI ;			// 描画時に指定される座標からの相対座標
	float					DrawPosXF, DrawPosYF ;			// 描画時に指定される座標からの相対座標
	int						UsePosXI, UsePosYI ;			// テクスチャ内で使用している矩形の左上座標
	float					UsePosXF, UsePosYF ;			// テクスチャ内で使用している矩形の左上座標
	int						WidthI, HeightI ;				// テクスチャ内で使用しているサイズ
	float					WidthF, HeightF ;				// テクスチャ内で使用しているサイズ

	IMAGEDATA_HARD_VERT		Vertex[ 4 ] ;					// テクスチャの描画座標情報
	unsigned char			VertType[ 4 ] ;					// 頂点タイプ( TRUE=三角の半分より上　FALSE=下 )

	IMAGEDATA_ORIG_HARD_TEX *Tex ;							// 使用するテクスチャへのポインタ
} ;

// ハードウエアレンダリング版イメージデータ構造体
struct IMAGEDATA_HARD
{
	int						DrawNum ;						// 描画用情報の数
	IMAGEDATA_HARD_DRAW		Draw[ 4 ] ;						// 描画用情報
} ;

// イメージ元データ構造体
struct IMAGEDATA_READBASE
{
	int						Type ;							// タイプ( 0:ファイル 1:メモリイメージ 2:BaseImage )

	wchar_t					*FileName ;						// ファイル名
	BASEIMAGE				*BaseImage ;					// 基本イメージデータ
	BASEIMAGE				*AlphaBaseImage ;				// αチャンネル用基本イメージデータ
	void					*MemImage ;						// メモリイメージ
	int						MemImageSize ;					// メモリイメージサイズ
	void					*AlphaMemImage ;				// αチャンネル用メモリイメージ
	int						AlphaMemImageSize ;				// αチャンネル用メモリイメージサイズ
	int						ReverseFlag ;					// 反転読み込みしたかどうか( TRUE:した  FALSE:してない )、再読み込みの際に読み込み元を反転するかどうかで使用される、BASEIMAGE に関しては読み込み元保存時に反転するためこのフラグは無視される
	int						ConvertPremultipliedAlpha ;		// 乗算済みαチャンネル付き画像に変換したかどうか( TRUE:した  FALSE:してない )、再読み込みの際に乗算済みα変換を行うかどうかで使用される、BASEIMAGE に関しては読み込み元保存時に乗算済みα変換されるためこのフラグは無視される
	int						NotUseTransColor ;				// 透過色を使用しなかったかどうか( TRUE:使用しなかった  FALSE:使用した )

	int						RefCount ;						// 参照数
} ;

// イメージデータ構造体
struct IMAGEDATA
{
	HANDLEINFO				HandleInfo ;					// ハンドル共通データ

	int						*LostFlag ;						// 解放時に立てるフラグのポインタ

#ifndef DX_NON_MOVIE
	int						MovieHandle ;					// 動画ハンドル
#endif

	IMAGEDATA_READBASE		*ReadBase ;						// 元データ情報
	int						UseBaseXI, UseBaseYI ;			// 元データ中で使用している範囲の左上座標
	float					UseBaseXF, UseBaseYF ;			// 元データ中で使用している範囲の左上座標( float型 )

	IMAGEDATA_ORIG			*Orig ;							// オリジナル画像情報構造体へのポインタ( オリジナル画像ではない場合は NULL )

	int						UseOrigXI, UseOrigYI ;			// オリジナル画像中の使用している矩形の左上座標
	float					UseOrigXF, UseOrigYF ;			// オリジナル画像中の使用している矩形の左上座標( float型 )
	int						WidthI, HeightI ;				// オリジナル画像中の使用している矩形の幅と高さ
	float					WidthF, HeightF ;				// オリジナル画像中の使用している矩形の幅と高さ( float型 )

	DWORD					*FullColorImage ;				// フルカラーイメージ

	int						LockFlag ;						// ロックしているかどうかフラグ
	BYTE					*LockImage ;					// ロック時に作成するテンポラリイメージ
	DWORD					LockImagePitch ;				// ロックイメージのピッチ

	int						NotInitGraphDelete ;			// InitGraph で削除しないかどうかのフラグ( TRUE:InitGraphでは削除しない  FALSE:InitGraphで削除する )

	int						DeviceLostDeleteFlag ;			// デバイスロスト時に削除するかどうかのフラグ( TRUE:デバイスロスト時に削除する  FALSE:デバイスロスト時に削除しない )

	union
	{
		IMAGEDATA_SOFT		Soft ;							// ソフトウエアレンダリング用構造体へのポインタ
		IMAGEDATA_HARD		Hard ;							// ハードウエアレンダリング用構造体へのポインタ
	} ;
} ;

// シェーダーハンドルで使用する情報の構造体
struct SHADERHANDLEDATA
{
	HANDLEINFO				HandleInfo ;						// ハンドル共通データ

	int						IsVertexShader ;					// 頂点シェーダーかどうか( TRUE:頂点シェーダー  FALSE:ピクセルシェーダー )
	void					*FunctionCode ;						// シェーダーコードを格納したメモリ領域へのポインタ
	int						FunctionCodeSize ;					// シェーダーコードのサイズ

	struct SHADERHANDLEDATA_PF 	*PF ;							// 環境依存データ
} ;

// シェーダー用定数バッファハンドルで使用する情報の構造体
struct SHADERCONSTANTBUFFERHANDLEDATA
{
	HANDLEINFO				HandleInfo ;						// ハンドル共通データ

	struct SHADERCONSTANTBUFFERHANDLEDATA_PF 	*PF ;					// 環境依存データ
} ;

// 頂点バッファハンドルで使用する情報の構造体
struct VERTEXBUFFERHANDLEDATA
{
	HANDLEINFO				HandleInfo ;						// ハンドル共通データ

	int						Type ;								// 頂点データのタイプ( DX_VERTEX_TYPE_NORMAL_3D 等 )
	int						UnitSize ;							// 頂点データ一つ辺りのバイト数
	int						Num ;								// 頂点の数
	void					*Buffer ;							// システムメモリに確保されたバッファ

	struct VERTEXBUFFERHANDLEDATA_PF *PF ;						// 環境依存データ
} ;

// インデックスバッファハンドルで使用する情報の構造体
struct INDEXBUFFERHANDLEDATA
{
	HANDLEINFO				HandleInfo ;						// ハンドル共通データ

	int						Type ;								// インデックスデータのタイプ( DX_INDEX_TYPE_NORMAL_3D 等 )
	int						UnitSize ;							// インデックスデータ一つ辺りのバイト数
	int						Num ;								// インデックスの数
	void					*Buffer ;							// システムメモリに確保されたバッファ

	struct INDEXBUFFERHANDLEDATA_PF *PF ;						// 環境依存データ
} ;

// シャドウマップデータ構造体
struct SHADOWMAPDATA
{
	HANDLEINFO				HandleInfo ;					// ハンドル共通データ

	int						*LostFlag ;						// 解放時に立てるフラグのポインタ

	int						TexFormat_Float ;				// 浮動小数点型フォーマットかどうか( TRUE:浮動小数点型  FALSE:固定小数点型 )
	int						TexFormat_BitDepth ;			// ビット深度
	int						ColorFormat ;					// カラーフォーマット
	int						ZBufferFormat ;					// Ｚバッファフォーマット

	int						EnableDrawArea ;				// DrawAreaMinPosition と DrawAreaMaxPosition が有効かどうか
	VECTOR					DrawAreaMinPosition ;			// シャドウマップの描画の際に入れる範囲の小さい座標
	VECTOR					DrawAreaMaxPosition ;			// シャドウマップの描画の際に入れる範囲の大きい座標
	VECTOR					DrawAreaViewClipPos[ 8 ] ;		// シャドウマップに描画する範囲を囲む座標

	VECTOR					DefaultViewClipPos[ 8 ] ;		// EnableDrawArea が FALSE の際に使用するシャドウマップに描画する範囲を囲む座標

	int						BaseSizeX ;						// シャドウマップのサイズ
	int						BaseSizeY ;

	int						DrawSetupFlag ;					// 描画準備をしているかどうかのフラグ

	MATRIX					ShadowMapViewMatrix ;			// シャドウマップを描画した際のビュー行列
	MATRIX					ShadowMapProjectionMatrix ;		// シャドウマップを描画した際の射影行列
	MATRIX					ShadowMapViewProjectionMatrix ; // シャドウマップを描画した際のビュー行列と射影行列を乗算したもの

	float					AdjustDepth ;					// シャドウマップを使用した描画時の深度補正値
	int						BlurParam ;						// シャドウマップに適用するぼかしフィルターの値
	float					GradationParam ;				// シャドウマップ値のグラデーションの範囲

	VECTOR					LightDirection ;				// シャドウマップが想定するライトの方向
	MATRIX					LightMatrix ;					// シャドウマップが想定するライトの行列

	int						RenderTargetScreen[ DX_RENDERTARGET_COUNT ] ;			// 元々のレンダリングターゲット
	int						RenderTargetScreenSurface[ DX_RENDERTARGET_COUNT ] ;	// 元々のレンダリングターゲットサーフェス
	int						RenderTargetScreenMipLevel[ DX_RENDERTARGET_COUNT ] ;	// 元々のレンダリングターゲットMipレベル

	int						RenderMaskUseFlag ;				// マスク画面を使用するかどうかのフラグ

	int						RenderFogEnable ;				// フォグを使用するかどうかのフラグ

	VECTOR_D				RenderCameraPosition ;			// レンダリングを行う際のカメラの位置
	VECTOR_D				RenderCameraTarget ;			// レンダリングを行う際のカメラの注視点
	VECTOR_D				RenderCameraUp ;				// レンダリングを行う際のカメラのアップベクトル
	double					RenderCameraHRotate ;			// レンダリングを行う際のカメラの水平角度
	double					RenderCameraVRotate ;			// レンダリングを行う際のカメラの垂直角度
	double					RenderCameraTRotate ;			// レンダリングを行う際のカメラの捻り角度
	MATRIX_D				RenderCameraMatrix ;			// レンダリングを行う際のビュー行列
	double					RenderCameraScreenCenterX ;		// レンダリングを行う際のカメラの消失点
	double					RenderCameraScreenCenterY ;

	int						RenderProjectionMatrixMode ;	// レンダリングを行う際の射影行列作成モード
	double					RenderProjNear ;				// レンダリングを行う際のカメラの Nearクリップ面
	double					RenderProjFar ;					// レンダリングを行う際のカメラの Farクリップ面
	double					RenderProjDotAspect ;			// レンダリングを行う際のドットアスペクト比( 縦 / 横 )
	double					RenderProjFov ;					// レンダリングを行う際の遠近法時の視野角
	double					RenderProjSize ;				// レンダリングを行う際の正射影時のサイズ
	MATRIX_D				RenderProjMatrix ;				// レンダリングを行う際の射影行列

	struct SHADOWMAPDATA_PF	*PF ;							// 環境依存データ
} ;


// 浮動小数点型の RECT 構造体
struct RECTF
{
	float left, top ;
	float right, bottom ;
} ;

// グラフィックハンドルのセットアップに必要なグローバルデータを纏めた構造体
struct SETUP_GRAPHHANDLE_GPARAM
{
	DWORD					TransColor ;							// 透過色

	int						CreateImageColorBitDepth ;				// 作成する画像の色深度
	int						FloatTypeGraphCreateFlag ;				// Float型画像作成指定フラグ( テクスチャサーフェスのみ )
	int						CreateImageChannelNum ;					// 作成する画像のチャンネル数( テクスチャサーフェスのみ )
	int						CreateImageChannelBitDepth ;			// 作成する画像の１チャンネル辺りのビット深度( こちらが設定されている場合は CreateImageColorBitDepth より優先される )
	int						AlphaTestImageCreateFlag ;				// αテスト付き画像作成指定フラグ( AlphaGraphCreateFlag の方が優先度が高い )( テクスチャサーフェスのみ )
	int						AlphaChannelImageCreateFlag ;			// αチャンネル付き画像作成指定フラグ( DrawValidGraphCreateFlag の方が優先度が高い )
	int						CubeMapTextureCreateFlag ;				// キューブマップテクスチャ作成指定フラグ( 1:キューブマップテクスチャを作成する  0:通常テクスチャを作成する )
	int						BlendImageCreateFlag ;					// ブレンド処理用画像作成指定フラグ
	int						UseManagedTextureFlag ;					// マネージドテクスチャを使用するか、フラグ( 1:使用する  0:使用しない )
	int						UseLinearMapTextureFlag ;				// テクスチャのメモリデータ配置にリニアが選択できる場合はデータ配置方式をリニアにするかどうか( TRUE:リニアが可能な場合はリニアにする  FALSE:リニアが可能な場合も特にリニアを指定しない )
	int						PlatformTextureFormat ;					// 環境依存のテクスチャフォーマットを直接指定するために使用するための変数( DX_TEXTUREFORMAT_DIRECT3D9_R8G8B8 など )

	int						DrawValidImageCreateFlag ;				// 描画可能画像作成指定フラグ( テクスチャサーフェスのみ )
	int						DrawValidAlphaImageCreateFlag ;			// 描画可能なαチャンネル付き画像作成指定フラグ( テクスチャサーフェスのみ )
	int						DrawValidFloatTypeGraphCreateFlag ;		// 描画可能なFloat型画像作成指定フラグ( テクスチャサーフェスのみ )
	int						DrawValidGraphCreateZBufferFlag ;		// 描画可能画像を作成する際に専用のＺバッファも作成するかどうか
	int						CreateDrawValidGraphMipLevels ;			// 描画可能画像のMipLevels( テクスチャサーフェスのみ )
	int						CreateDrawValidGraphChannelNum ;		// 描画可能画像のチャンネル数( テクスチャサーフェスのみ )
	int						CreateDrawValidGraphZBufferBitDepth ;	// 描画可能画像のＺバッファのビット深度( テクスチャサーフェスのみ )
	int						DrawValidMSSamples ;					// 描画可能な画像のマルチサンプリング数
	int						DrawValidMSQuality ;					// 描画可能な画像のマルチサンプリングクオリティ

	int						MipMapCount ;							// 自動で作成するミップマップの数( -1:最大レベルまで作成する )
	int						UserMaxTextureSize ;					// ユーザー指定のテクスチャ最大サイズ
	int						NotUseDivFlag ;							// 画像分割を行わないかどうか( TRUE:行わない  FALSE:行う )
	int						NotUseAlphaImageLoadFlag ;				// _a が付いたアルファチャンネル用の画像ファイルを追加で読み込む処理を行わないかどうか( TRUE:行わない  FALSE:行う )
	int						NotUsePaletteGraphFlag ;				// パレット画像が使用できる場合もパレット画像を使用しないかどうか( TRUE:使用しない  FALSE:使用する )
	int						NotInitGraphDelete ;					// InitGraph で削除しないかどうかのフラグ( TRUE:InitGraphでは削除しない  FALSE:InitGraphで削除する )

	void *					UserPlatformTexture ;					// ユーザー指定の環境依存テクスチャオブジェクトのアドレス
} ;

// シャドウマップハンドルのセットアップに必要なグローバルデータを纏めた構造体
struct SETUP_SHADOWMAPHANDLE_GPARAM
{
	int						Dummy ;
} ;

// 画像データからグラフィックハンドルの作成・画像データの転送に必要なグローバルデータを纏めたもの
struct CREATE_GRAPHHANDLE_AND_BLTGRAPHIMAGE_GPARAM
{
	SETUP_GRAPHHANDLE_GPARAM InitHandleGParam ;						// グラフィックハンドルのセットアップに必要なグローバルデータ

	int						NotUseTransColor;						// 透過色を使用しないかどうかフラグ(TRUE:使用しない  FALSE:使用する)
	int						UseTransColorGraphCreateFlag ;			// 透過色とそうでない部分の境界部分を滑らかにするか、フラグ
	int						LeftUpColorIsTransColorFlag ;			// 画像左上のピクセル色を透過色にするかどうかのフラグ
} ;

// 画像の元データの情報に必要なグローバルデータを纏めたもの
struct SETGRAPHBASEINFO_GPARAM
{
	int						NotGraphBaseDataBackupFlag ;			// グラフィックハンドルを作成した際に使用した画像データをバックアップしないかどうかのフラグ( TRUE:バックアップしない  FALSE:バックアップする )
} ;

// ファイルからグラフィックハンドルを作成する処理に必要なグローバルデータを纏めたもの
struct LOADGRAPH_GPARAM
{
	LOADBASEIMAGE_GPARAM	LoadBaseImageGParam ;					// 画像データの読み込みに必要なグローバルデータ
#ifndef DX_NON_MOVIE
	OPENMOVIE_GPARAM		OpenMovieGParam ;						// ムービーファイルのオープンに必要なグローバルデータ
#endif
	CREATE_GRAPHHANDLE_AND_BLTGRAPHIMAGE_GPARAM CreateGraphGParam ;	// グラフィックハンドルの作成・初期化に必要なグローバルデータ
	SETGRAPHBASEINFO_GPARAM	SetGraphBaseInfoGParam ;				// 画像の元データの情報の保存に必要なグローバルデータ
} ;

// グラフィックデータ読み込み引数を纏めたもの
struct LOADGRAPH_PARAM
{
	LOADGRAPH_GPARAM		GParam ;
	int						ReCreateFlag ;
	int						GrHandle ;
	int						BaseHandle ;

	const wchar_t *			FileName ;
	const void *			RgbMemImage ;
	int						RgbMemImageSize ;
	const void *			AlphaMemImage ;
	int						AlphaMemImageSize ;
	const BITMAPINFO *		RgbBmpInfo ;
	void *					RgbBmpImage ;
	const BITMAPINFO *		AlphaBmpInfo ;
	void *					AlphaBmpImage ;
	BASEIMAGE *				RgbBaseImage ;
	BASEIMAGE *				AlphaBaseImage ;

	int						AllNum ;
	int						XNum ;
	int						YNum ;
	int						IsFloat ;
	int						SizeXI ;
	float					SizeXF ;
	int						SizeYI ;
	float					SizeYF ;
	int *					HandleArray ;
	int						TextureFlag ;
	int						ReverseFlag ;
	int						SurfaceMode ;
} ;

// ライトハンドル構造体
struct LIGHT_HANDLE
{
	unsigned int			ID ;												// エラーチェック用のＩＤ
	int						Handle ;											// 自身のハンドル値

	LIGHTPARAM				Param ;												// ライトのパラメータ
	int						SetHardwareIndex ;									// ハードウェアアクセラレータ上のセットされているインデックス( -1:セットされていない )
	int						EnableFlag ;										// 有効フラグ
	int						HardwareChangeFlag ;								// ハードウェアアクセラレータに反映していない設定があるかどうかのフラグ( TRUE:ある  FALSE:ない )
	int						ShadowMapSlotDisableFlag[ MAX_USE_SHADOWMAP_NUM ] ;	// シャドウマップを使用しないかどうかのフラグ( TRUE:使用しない  FALSE:使用する )
} ;

// ライト関係情報の構造体
struct GRAPHICSSYS_LIGHTATA
{
	int						ProcessDisable ;						// ライト処理を無効にするかどうか
	int						ChangeMaterial ;						// ライト計算用マテリアルが変更したかどうかのフラグ
	MATERIALPARAM			Material ;								// ライト計算用マテリアル
	float					MaterialTypeParam[ MATERIAL_TYPEPARAM_MAX_NUM ] ;	// ライト計算用マテリアルのタイプ別パラメータ( DX_MATERIAL_TYPE_MAT_SPEC_LUMINANCE_TWO_COLOR などで使用 )
	int						MaterialNotUseVertexDiffuseColor ;		// ライト計算に頂点のディフューズカラーを使用しないかどうか
	int						MaterialNotUseVertexSpecularColor;		// ライト計算に頂点のスペキュラカラーを使用しないかどうか
	LIGHT_HANDLE			*Data[ MAX_LIGHT_NUM ] ;				// ライト情報へのポインタ
	int						Num ;									// ライトの数
	int						Area ;									// 有効なライトがある範囲
	int						HandleID ;								// ライトに割り当てるＩＤ
	int						EnableNum ;								// 有効になっているライトの数
	int						MaxHardwareEnableIndex ;				// ハードウェアアクセラレータに対して有効になっているライトで一番大きな値のインデックス
	int						EnableHardwareIndex[ MAX_HARDWAREENABLELIGHTINDEX_NUM ] ;	// ハードウェアアクセラレータに対して有効にしているライトのリスト
	int						HardwareChange ;						// ハードウェアアクセラレータ側への反映が必要な変更があったかどうかのフラグ
	int						HardwareRefresh ;						// ハードウェアアクセラレータ側への全項目の反映が必要かどうかのフラグ
	int						DefaultHandle ;							// デフォルトライト用ハンドル
} ;

// カメラ関係情報の構造体
struct GRAPHICSSYS_CAMERA
{
	VECTOR_D				Position ;								// カメラの位置
	VECTOR_D				Target ;								// カメラの注視点
	VECTOR_D				Up ;									// カメラのアップベクトル
	double					HRotate ;								// カメラの水平角度
	double					VRotate ;								// カメラの垂直角度
	double					TRotate ;								// カメラの捻り角度
	MATRIX_D				Matrix ;								// ビュー行列
	int						SetupType ;								// セットアップタイプ( CAMERA_SETUPTYPE_POS_TARG_UP など )
	double					ScreenCenterX ;							// カメラの消失点
	double					ScreenCenterY ;
} ;

// ユーザーの描画設定情報構造体
struct GRAPHICSSYS_USERRENDERINFO
{
	int						SetTextureGraphHandle[ 20 ] ;			// ユーザー設定の各ステージのテクスチャ
	int						SetRenderTargetGraphHandle[ 4 ] ;		// ユーザー設定の各レンダリングターゲット

	int						SetVertexShaderHandle ;					// ユーザー設定で使用する頂点シェーダー
	int						SetPixelShaderHandle ;					// ユーザー設定で使用するピクセルシェーダー
} ;

// 描画設定関係情報の構造体
struct GRAPHICSSYS_DRAWSETTINGDATA
{
	int						EnableZBufferFlag2D ;					// Ｚバッファの有効フラグ
	int						WriteZBufferFlag2D ;					// Ｚバッファの更新を行うか、フラグ
	int						ZBufferCmpType2D ;						// Ｚ値の比較モード
	int						ZBias2D ;								// Ｚバイアス

	int						EnableZBufferFlag3D ;					// Ｚバッファの有効フラグ
	int						WriteZBufferFlag3D ;					// Ｚバッファの更新を行うか、フラグ
	int						ZBufferCmpType3D ;						// Ｚ値の比較モード
	int						ZBias3D ;								// Ｚバイアス

	int						NotDrawFlagInSetDrawArea ;				// 描画不可能フラグ（SetDrawArea用）
	int						UseNoBlendModeParam ;					// DX_BLENDMODE_NOBLEND 時でも Param の値を使用するかどうかのフラグ( TRUE:使用する  FALSE:使用しない )

//	int						NotUseBasicGraphDraw3DDeviceMethodFlag ;// 単純図形の描画に３Ｄデバイスの機能を使用しないかどうかのフラグ

	int						FillMode ;								// フィルモード( DX_FILL_SOLID など )
	int						CullMode ;								// カリングモード( DX_CULLING_LEFT など )

	int						TexAddressModeU[ USE_TEXTURESTAGE_NUM ] ;	// テクスチャアドレスモードＵ
	int						TexAddressModeV[ USE_TEXTURESTAGE_NUM ] ;	// テクスチャアドレスモードＶ
	int						TexAddressModeW[ USE_TEXTURESTAGE_NUM ] ;	// テクスチャアドレスモードＷ

	int						FogEnable ;								// フォグが有効かどうか( TRUE:有効  FALSE:無効 )
	int						FogMode ;								// フォグモード
	DWORD					FogColor ;								// フォグカラー
	float					FogStart, FogEnd ;						// フォグ開始アドレスと終了アドレス
	float					FogDensity ;							// フォグ密度

	float					DrawZ;									// ２Ｄ描画時にＺバッファに書き込むＺ値
	int						DrawMode ;								// 描画モード

	int						MaxAnisotropy ;							// 最大異方性
	int						Large3DPositionSupport ;				// ３Ｄ描画で使用する座標値が 10000000.0f などの大きな値になっても描画の崩れを小さく抑える処理を使用するかどうかを設定する( TRUE:描画の崩れを抑える処理を使用する( CPU負荷が上がります )　　FALSE:描画の崩れを抑える処理は使用しない( デフォルト ) )

	int						AlphaChDrawMode ;						// 描画先に正しいα値を書き込むかどうか( TRUE:正しい値を書き込む  FALSE:通常モード )

	int						BlendMode ;								// ブレンドモード
	int						BlendParam ;							// ブレンドパラメータ

	int						AADrawInfoValid ;						// BeginAADraw が実行されているかどうか( TRUE:実行されている  FALSE:実行されていない )
	int						AADrawOldBlendMode ;					// BeginAADraw で保存する元のブレンドモード
	int						AADrawOldBlendParam ;					// BeginAADraw で保存する元のブレンドパラメータ
	int						AADrawOldDrawMode ;						// BeginAADraw で保存する元の描画モード
	DWORD					AADrawOldDrawColor ;					// BeginAADraw で保存する元の描画輝度

	int						BlendGraph ;							// ブレンドグラフィックハンドル
//	int						BlendGraphType ;						// ブレンドグラフィックタイプ
//	int						BlendGraphFadeRatio ;					// ブレンドグラフィックのフェードパラメータ
	int						BlendGraphBorderParam ;					// ブレンドグラフィックハンドルのブレンド境界値(0〜255)
	int						BlendGraphBorderRange ;					// ブレンドグラフィックハンドルの境界部分の幅(0〜255)
	int						BlendGraphX, BlendGraphY ;				// ブレンドグラフィックの起点座標
	int						BlendGraphPosMode ;						// ブレンドグラフィックの座標モード( DX_BLENDGRAPH_POSMODE_DRAWGRAPH など )

	int						AlphaTestMode ;							// アルファテストモード
	int						AlphaTestParam ;						// アルファテストパラメータ

	int						NotUseSpecular ;						// スペキュラを使用しないかどうか
	int						ShadowMap[ MAX_USE_SHADOWMAP_NUM ] ;	// 使用するシャドウマップグラフィックハンドル
	int						UseShadowMapNum ;						// 有効なシャドウマップの設定数

	int						ShadowMapDrawSetupRequest ;				// シャドウマップに対する描画準備リクエスト
	int						ShadowMapDraw ;							// シャドウマップに対する描画かどうかのフラグ
	int						ShadowMapDrawHandle ;					// シャドウマップに対する描画の際の、シャドウマップハンドル

	union
	{
		RGBCOLOR			DrawBright ;							// 描画輝度
		DWORD				bDrawBright ;
	} ;
	int						AlwaysDiffuseColorFlag ;				// レンダリングデバイスがディフューズカラーを使用するかどうかでステートが変化するタイプだった場合、必ずディフューズカラーを使用するようにするかどうかのフラグ

	int						IgnoreGraphColorFlag ;					// 描画する画像の色成分を無視するかどうかのフラグ

	int						TargetScreen[ DX_RENDERTARGET_COUNT ] ;	// 描画先グラフィック識別値
	int						TargetScreenSurface[ DX_RENDERTARGET_COUNT ] ; // 描画先グラフィック内サーフェスインデックス
	int						TargetScreenMipLevel[ DX_RENDERTARGET_COUNT ] ;	// 描画先グラフィック内サーフェスのMipレベル
//	int						TargetScreenVramFlag ;					// 描画先グラフィックがＶＲＡＭに存在するか否か
	int						TargetZBuffer ;							// 描画先Ｚバッファ識別値
//	RECT					WindowDrawRect ;						// デスクトップのあるサーフェスに描画処理を行う
																	// 場合ウインドウのクライアント領域の矩形データが入っている

	RECT					OriginalDrawRect ;						// ウインドウの位置によって改変される前の使用者が意図する正しい描画矩形
	RECT					DrawArea ;								// 描画可能矩形
	RECTF					DrawAreaF ;								// 描画可能矩形浮動小数点型
	int						DrawSizeX, DrawSizeY ;					// 描画対象のサイズ
	float					Draw3DScale ;							// ３Ｄ描画処理のスケール

	int						Valid2DMatrix ;							// _2DMatrix に有効な値が設定されているか( TRUE:設定されている  FALSE:設定されていない )
	int						MatchHardware_2DMatrix ;				// ３Ｄデバイスに設定されている２Ｄ変換行列と本構造体中の２Ｄ変換行列が一致しているかどうか( TRUE:一致している  FALSE:一致していない )
	MATRIX_D				_2DMatrix ;								// ２Ｄ変換行列
	MATRIX					_2DMatrixF ;							// ２Ｄ変換行列

	int						MatchHardware3DMatrix ;					// ３Ｄデバイスに設定されている行列と本構造体中の３Ｄ描画用行列が一致しているかどうか( TRUE:一致している  FALSE:一致していない )

	int						MatchHardwareWorldMatrix ;				// ３Ｄデバイスに設定されているワールド変換行列と本構造体中のワールド変換行列が一致しているかどうか( TRUE:一致している  FALSE:一致していない )
	MATRIX_D				WorldMatrix ;							// ワールド変換行列
	MATRIX					WorldMatrixF ;							// ワールド変換行列

	int						MatchHardwareViewMatrix ;				// ３Ｄデバイスに設定されているビュー変換行列と本構造体中のビュー変換行列が一致しているかどうか( TRUE:一致している  FALSE:一致していない )
	MATRIX_D				ViewMatrix ;							// ビュー変換行列
	MATRIX					ViewMatrixF ;							// ビュー変換行列

	int						MatchHardwareProjectionMatrix ;			// ３Ｄデバイスに設定されている射影変換行列と本構造体中の射影変換行列が一致しているかどうか( TRUE:一致している  FALSE:一致していない )
	MATRIX_D				ProjectionMatrix ;						// 射影変換行列
	MATRIX					ProjectionMatrixF ;						// 射影変換行列

	int						ProjectionMatrixMode ;					// 射影行列モード( 0:遠近法  1:正射影  2:行列指定 )
	double					ProjNear, ProjFar ;						// Ｚクリッピングの Near面と Far面
	double					ProjDotAspect ;							// ドットアスペクト比( 縦 / 横 )
	double					ProjFov ;								// 遠近法時の視野角
	double					ProjSize ;								// 正射影時のサイズ
	MATRIX_D				ProjMatrix ;							// 射影行列

	int						SetDrawScreenNoSettingResetFlag ;		// SetDrawScreen を実行した際にカメラなどの設定のリセットを行わないかどうか

	MATRIX_D				Direct3DViewportMatrix ;				// Ｄｉｒｅｃｔ３Ｄ 的なビューポート行列
	MATRIX					Direct3DViewportMatrixF ;				// Ｄｉｒｅｃｔ３Ｄ 的なビューポート行列
	MATRIX_D				Direct3DViewportMatrixAnti ;			// Ｄｉｒｅｃｔ３Ｄ 的なビューポート行列を無効にする行列
	MATRIX					Direct3DViewportMatrixAntiF ;			// Ｄｉｒｅｃｔ３Ｄ 的なビューポート行列を無効にする行列
	MATRIX_D				ViewportMatrix ;						// ビューポート行列
	MATRIX					ViewportMatrixF ;						// ビューポート行列
//	MATRIX					ViewportMatrix2D ;						// ２Ｄ描画用ビューポート行列
//	MATRIX					ProjectionMatrix2D ;					// ２Ｄ描画用射影行列
//	int						UseProjectionMatrix2D ;					// ２Ｄ描画用射影行列を使用する設定になっているかどうか( TRUE:使用する  FALSE:使用しない )

	int						ValidBlend3DMatrix ;					// BlendMatrix, ViewProjectionMatrix, BillboardMatrix が有効かどうか( TRUE:有効  FALSE:無効 )
	MATRIX_D				Blend3DMatrix ;							// ワールド変換、ビュー変換、射影変換、ビューポート変換行列を掛け合わせたもの
	MATRIX					Blend3DMatrixF ;						// float型の Blend3DMatrix
	int						ValidInverseBlend3DMatrix ;				// Blend3DMatrix の逆行列が有効かどうか( TRUE:有効  FALSE:無効 )
	MATRIX_D				InverseBlend3DMatrix ;					// Blend3DMatrix の逆行列
	MATRIX					InverseBlend3DMatrixF ;					// float型の InverseBlend3DMatrix
	MATRIX_D				WorldViewMatrix ;						// ワールド行列とビュー行列を掛け合わせたもの
	MATRIX_D				ViewProjectionViewportMatrix ;			// ビュー行列と射影行列とビューポート行列を掛け合わせたもの
	MATRIX_D				BillboardMatrix ;						// ビルボード用の行列
	MATRIX					BillboardMatrixF ;						// ビルボード用の行列
	VECTOR_D				ViewClipPos[ 2 ][ 2 ][ 2 ] ;			// 視錐台頂点( [ 0:z+ 1:z- ][ 0:top 1:bottom ][ 0:left 1:right ] )
	DOUBLE4					ClippingPlane[ 6 ] ;					// クリッピング平面( 0:-x 1:+x 2:-y 3:+y 4:-z 5:+z )
	int						ValidConvScreenPosToWorldPosMatrix ;	// 画面座標からスクリーン座標に変換する際に使用する行列が有効かどうか( TRUE:有効  FALSE:無効 )
	MATRIX_D				ConvScreenPosToWorldPosMatrix ;			// 画面座標からスクリーン座標に変換する際に使用する行列

	int						TextureAddressTransformUse ;			// テクスチャ座標変換処理を行うかどうか( TRUE:行う  FALSE:行わない )
//	float					TextureTransX ;							// テクスチャ平行移動
//	float					TextureTransY ;
//	float					TextureScaleX ;							// テクスチャ拡大率
//	float					TextureScaleY ;
//	float					TextureRotateCenterX ;					// テクスチャ回転の中心座標
//	float					TextureRotateCenterY ;
//	float					TextureRotate ;							// テクスチャ回転値
//	int						TextureMatrixValid ;					// TextureMatrix が有効かどうか( TRUE:有効  FALSE:無効 )
//	MATRIX					TextureMatrix ;							// TextureTransX や TextureScaleY などを使用しないで設定する行列
	int						MatchHardwareTextureAddressTransformMatrix ;	// ３Ｄデバイスに設定されているテクスチャ座標変換行列と本構造体中のテクスチャ座標変換行列が一致しているかどうか( TRUE:一致している  FALSE:一致していない )
	MATRIX					TextureAddressTransformMatrix ;			// テクスチャ座標変換行列

	GRAPHICSSYS_USERRENDERINFO	UserShaderRenderInfo ;				// SetUseTextureToShader で設定された各ステージのテクスチャ情報や、ユーザー設定のシェーダー定数情報など
} ;

// 画像作成に関係する情報の構造体
struct GRAPHICSSYS_CREATEIMAGEDATA
{
	int						ColorBitDepth ;							// 作成する画像の色深度
	int						ChannelBitDepth ;						// 作成する画像の１チャンネル辺りのビット深度( こちらが設定されている場合は CreateImageColorBitDepth より優先される )
//	int						TextureImageCreateFlag ;				// テクスチャ画像作成フラグ
	int						AlphaChannelFlag ;						// αチャンネル付き画像作成指定フラグ( DrawValidGraphCreateFlag の方が優先度が高い )
	int						AlphaTestFlag ;							// αテスト付き画像作成指定フラグ( AlphaGraphCreateFlag の方が優先度が高い )( テクスチャサーフェスのみ )
	int						CubeMapFlag ;							// キューブマップテクスチャ作成指定フラグ( 1:キューブマップテクスチャを作成する  0:通常テクスチャを作成する )
//	int						SystemMemImageCreateFlag ;				// システムメモリを使用する画像作成指定フラグ( 標準サーフェスのみ )
	int						BlendImageFlag ;						// ブレンド処理用画像作成指定フラグ
	int						NotUseManagedTextureFlag ;				// マネージドテクスチャを使用しないか、フラグ( 1:使用しない  0:使用する )
	int						PlatformTextureFormat ;					// 環境依存のテクスチャフォーマットを直接指定するために使用するための変数( DX_TEXTUREFORMAT_DIRECT3D9_R8G8B8 など )

	int						DrawValidFlag ;							// 描画可能画像作成指定フラグ( テクスチャサーフェスのみ )
	int						DrawValidAlphaFlag ;					// 描画可能なαチャンネル付き画像作成指定フラグ( テクスチャサーフェスのみ )
	int						DrawValidFloatTypeFlag ;				// 描画可能なFloat型画像作成指定フラグ( テクスチャサーフェスのみ )
	int						NotDrawValidCreateZBufferFlag ;			// 描画可能画像を作成する際に専用のＺバッファは作成しないかどうか
	int						DrawValidMipLevels ;					// 描画可能画像のMipLevel( テクスチャサーフェスのみ )
	int						DrawValidChannelNum ;					// 描画可能画像のチャンネル数( テクスチャサーフェスのみ )
	int						DrawValidZBufferBitDepth ;				// 描画可能画像のＺバッファのビット深度( テクスチャサーフェスのみ )
	int						DrawValidMSSamples ;					// 描画可能な画像のマルチサンプリング数
	int						DrawValidMSQuality ;					// 描画可能な画像のマルチサンプリングクオリティ

	int						NotGraphBaseDataBackupFlag ;			// グラフィックハンドルを作成した際に使用した画像データをバックアップしないかどうかのフラグ( TRUE:バックアップしない  FALSE:バックアップする )
	int						LeftUpColorIsTransColorFlag ;			// 画像左上のピクセル色を透過色にするかどうかのフラグ
	int						UseTransColorFlag ;						// 透過色とそうでない部分の境界部分を滑らかにするか、フラグ
	DWORD					TransColor ;							// 透過色
	int						NotUseTransColor;						// 透過色を使用しないかどうかフラグ(TRUE:使用しない  FALSE:使用する)
	int						NotUseDivFlag ;							// 画像分割を行わないかどうか( TRUE:行わない  FALSE:行う )
	int						MipMapCount ;							// 自動で作成するミップマップの数( -1:最大レベルまで作成する )
	int						UserMaxTextureSize ;					// ユーザー指定のテクスチャ最大サイズ
	int						NotUseAlphaImageLoadFlag ;				// _a が付いたアルファチャンネル用の画像ファイルを追加で読み込む処理を行わないかどうか( TRUE:行わない  FALSE:行う )
	int						NotUsePaletteGraphFlag ;				// パレット画像が使用できる場合もパレット画像を使用しないかどうか( TRUE:使用しない  FALSE:使用する )
} ;

// ディスプレイ一つあたりの情報
struct GRAPHICSSYS_DISPLAYINFO
{
	int						IsPrimary ;								// プライマリモニタかどうか( TRUE:プライマリモニタ  FALSE:サブモニタ )
	RECT					DesktopRect ;							// デスクトップ上でのモニタ領域
	int						DesktopSizeX ;							// デスクトップ上での幅
	int						DesktopSizeY ;							// デスクトップ上での高さ
	int						DesktopPixelSizeX ;						// デスクトップ上でのピクセル幅( 拡大率無視幅 )
	int						DesktopPixelSizeY ;						// デスクトップ上でのピクセル高さ( 拡大率無視高さ )
	int						DesktopColorBitDepth ;					// デスクトップ上でのカラービット深度
	int						DesktopRefreshRate ;					// デスクトップ上でのリフレッシュレート
	wchar_t					Name[ 128 ] ;							// 名前

	int						ModeNum ;								// 変更可能なディスプレイモードの数
	DISPLAYMODEDATA			*ModeData ;								// ディスプレイモードリスト
} ;

// 画面関係の情報の構造体
struct GRAPHICSSYS_SCREENDATA
{
	int						MainScreenSizeX ;						// メイン画面のサイズ
	int						MainScreenSizeY ;
	int						MainScreenSizeX_Result ;				// GetDrawScreenSize の返り値になるサイズ
	int						MainScreenSizeY_Result ;
	int						MainScreenColorBitDepth ;				// メイン画面のカラービット深度
	int						MainScreenRefreshRate ;					// メイン画面のリフレッシュレート

	int						Emulation320x240Flag ;					// 640x480 の画面に 320x240 の画面を出力するかどうかのフラグ

	int						FullScreenResolutionMode ;				// フルスクリーン解像度モード( DX_FSRESOLUTIONMODE_NATIVE 等 )
	int						FullScreenResolutionModeAct ;			// 実際に使用されているフルスクリーン解像度モード( 例えば FullScreenMode が DX_FSRESOLUTIONMODE_NATIVE でも指定の解像度にモニタが対応していない場合はこの変数は DX_FSRESOLUTIONMODE_MAXIMUM になる )
	int						FullScreenScalingMode ;					// フルスクリーンスケーリングモード( DX_FSSCALINGMODE_NEAREST 等 )
	int						FullScreenFitScalingFlag ;				// ゲーム画面の解像度とフルスクリーン画面の解像度が異なった場合にフルスクリーン画面一杯に拡大して表示するかどうかのフラグ( TRUE:画面一杯に拡大  FALSE:アスペクト比を保ったまま )
	DISPLAYMODEDATA			FullScreenUseDispModeData ;				// フルスクリーンモードで使用しているディスプレイモードの情報
	RECT					FullScreenScalingDestRect ;				// DX_FSRESOLUTIONMODE_NATIVE 以外でフルスクリーンモード時に使用する転送先矩形
	RECT					FullScreenDesktopRect ;					// フルスクリーンモードでの使用しているデスクトップ上の矩形

//	int						PreSetWaitVSyncFlag ;					// DxLib_Init が呼ばれる前に SetWaitVSyncFlag( TRUE ) ; が実行されたかどうかのフラグ( TRUE:実行された  FALSE:実行されていない )
	int						NotWaitVSyncFlag ;						// ＶＳＹＮＣ待ちをしないかどうかのフラグ（TRUE：しない FALSE：する）

	int						ValidGraphDisplayArea ;					// GraphDisplayArea に有効な値が入っているかどうか
	RECT					GraphDisplayArea ;						// 表画面に転送する裏画面の領域

	int						EnableBackgroundColor ;					// バックグラウンド塗り潰し用カラーが有効かどうかのフラグ
	int						BackgroundRed ;							// バックグラウンド塗り潰し用カラー
	int						BackgroundGreen ;
	int						BackgroundBlue ;

	int						ScreenFlipFlag ;						// ScreenFlip関数を呼びだし中フラグ
	int						Graphics_Screen_ChangeModeFlag ;		// Graphics_Screen_ChangeMode を実行中かどうかのフラグ

//	int						DisplayModeNum ;						// 変更可能なディスプレイモードの数
//	DISPLAYMODEDATA			*DisplayMode ;							// ディスプレイモードリスト
	int						ValidUseDisplayIndex ;					// UseDisplayIndex が有効かどうか( TRUE:有効  FALSE:無効 )
	int						UseDisplayIndex ;						// 使用するディスプレイ番号

	int						DisplayNum ;							// ディスプレイの数
	GRAPHICSSYS_DISPLAYINFO	*DisplayInfo ;							// ディスプレイの情報

	int						DrawScreenBufferLockFlag ;				// バックバッファをロックしているかどうかフラグ
} ;

// 設定関係情報の構造体
struct GRAPHICSSYS_SETTINGDATA
{
	void					( *GraphRestoreShred )( void ) ;		// グラフィック復元関数のポインタ 

	int						ValidHardware ;							// ハードウエア描画が可能かどうか( TRUE:可能  FALSE:不可能 )
	int						NotUseHardware ;						// ハードウエア描画の機能を使用しないかどうか( TRUE:使用しない  FALSE:使用する )
	int						ChangeScreenModeNotGraphicsSystemFlag ;	// 画面モードの変更時に画像ハンドルを削除しないかどうか( TRUE:しない  FALSE:する )
	int						FSAAMultiSampleCount ;					// FSAA用マルチサンプリングピクセル数
	int						FSAAMultiSampleQuality ;				// FSAA用マルチサンプリングクオリティ
} ;

// 処理実行用リソース関係の構造体
struct GRAPHICSSYS_RESOURCE
{
	int						TempVertexBufferSize ;					// 一時的に頂点データを格納するためのバッファのサイズ
	void					*TempVertexBuffer ;						// 一時的に頂点データを格納するためのバッファ

	void					*CommonBuffer[ COMMON_BUFFER_NUM ] ;	// 汎用バッファ
	DWORD					CommonBufferSize[ COMMON_BUFFER_NUM ] ;	// 汎用バッファのメモリ確保サイズ

	int						WhiteTexHandle ;						// 8x8の白いテクスチャのハンドル
	int						LineTexHandle ;							// アンチエイリアス付きの線を描画するためのテクスチャハンドル
	int						LineTexHandle_PMA ;						// アンチエイリアス付きの線を描画するためのテクスチャハンドル( 乗算済みアルファ画像 )
	int						LineOneThicknessTexHandle ;				// アンチエイリアス付きの線を描画するためのテクスチャハンドル( 太さ1ピクセル用 )
	int						LineOneThicknessTexHandle_PMA ;			// アンチエイリアス付きの線を描画するためのテクスチャハンドル( 太さ1ピクセル用 )( 乗算済みアルファ画像 )
	int						LineBoxTexHandle ;						// アンチエイリアス付きの線矩形を描画するためのテクスチャハンドル
	int						LineBoxTexHandle_PMA ;					// アンチエイリアス付きの線矩形を描画するためのテクスチャハンドル( 乗算済みアルファ画像 )
	int						LineBoxOneThicknessTexHandle ;			// アンチエイリアス付きの線矩形を描画するためのテクスチャハンドル( 太さ1ピクセル用 )
	int						LineBoxOneThicknessTexHandle_PMA ;		// アンチエイリアス付きの線矩形を描画するためのテクスチャハンドル( 太さ1ピクセル用 )( 乗算済みアルファ画像 )
} ;

// ハードウェアアクセラレータ情報関係の構造体
struct GRAPHICSSYS_HARDWAREINFO
{
	int						ValidSubBlend ;							// 減算ブレンドが可能かどうか( TRUE:可能  FALSE:不可能 )
	int						ScreenAxisYReverse ;					// 画面のＹ軸が上下反対( 座標０が画面下端、上端ほど座標値が高くなる )かどうか( TRUE:上下反対  FALSE:座標０が画面上端 )
	int						ScreenAxisYReverse_OnlyDrawValidGraph ;	// 描画可能画像に対する描画の場合のみ画面のＹ軸が上下反対( 座標０が画面下端、上端ほど座標値が高くなる )かどうか( TRUE:上下反対  FALSE:座標０が画面上端 )
	int						MainScreenImageYReverse ;				// 表画面、裏画面のビットマップイメージが上下反転しているかどうか( TRUE:上下反対  FALSE:座標０が画面上端 )
	int						Support4bitPaletteTexture ;				// 16色パレットテクスチャをサポートするかどうか( TRUE:サポートする  FALSE:サポートしない )
	int						ChangeGraphModeOnlyChangeSubBackbuffer ;// SetGraphMode の際にサブバックバッファの変更のみを行うかどうか( TRUE:サブバックバッファの変更のみ行う  FALSE:グラフィックスシステムの再セットアップを行う )
	int						TextureSquareOnly ;						// 正方形テクスチャのみ作成可能かどうか( TRUE:正方形テクスチャのみ  FALSE:正方形テクスチャ以外も可能 )
	int						RenderTargetNum ;						// 同時にレンダリングできるレンダーターゲットの数
	int						TextureSizeNonPow2Conditional ;			// 条件付でテクスチャのサイズが２のｎ乗でなくても大丈夫かどうか
	int						TextureSizePow2 ;						// テクスチャのサイズが２のｎ乗である必要があるかどうか
	int						MaxTextureSize ;						// 最大テクスチャサイズ
	int						MaxTextureWidth ;						// 最大テクスチャ幅
	int						MaxTextureHeight ;						// 最大テクスチャ高さ
	int						MaxPrimitiveCount ;						// 一度に描画できるプリミティブの最大数
	int						MaxVertexIndex ;						// 一度の使用することの出来る最大頂点数
	int						UseShader ;								// プログラマブルシェーダーを使用するかどうか( TRUE:使用する  FALSE:使用しない )
	int						UseVertexColorBGRAFormat ;				// BGRAカラータイプの頂点カラーを使用するかどうか( TRUE:使用する  FALSE:使用しない )
	int						DrawFloatCoordType ;					// DrawGraphF 等の浮動小数点値で座標を指定する関数における座標タイプ( DX_DRAWFLOATCOORDTYPE_DIRECT3D9 など )
	int						OggTheoraUseDrawValidGraph ;			// OggTehora再生用の画像は描画可能画像にするかどうか( TRUE:描画可能画像にする  FALSE:描画可能画像にしない )
	int						UseOfSetWaitVSyncFlagIsSupportedEvenAfterDxLib_Init ;	// DxLib_Init の実行後でも SetWaitVSyncFlag が有効かどうか( TRUE:有効  FALSE:無効 )
} ;

// パフォーマンスに関する情報関係の構造体
struct GRAPHICSSYS_PERFORMANCEINFO
{
	int						PrevFrameDrawCallCount ;				// １回の ScreenFlip → ScreenFlip 間に行われた描画コール回数( 前回のフレーム )
	int						NowFrameDrawCallCount ;					// １回の ScreenFlip → ScreenFlip 間に行われた描画コール回数( 現在のフレーム )
	LONGLONG				ScreenFlipTime[ 2 ] ;					// 前回、前々回 ScreenFlip が呼ばれた時間( ナノ秒 )
} ;

// ソフトウエアレンダリングで使用する情報の構造体
struct GRAPHICSSYS_SOFTRENDERDATA
{
	MEMIMG					MainBufferMemImg ;					// メイン画面用 MEMIMG
	MEMIMG					SubBufferMemImg ;					// 補助画面用 MEMIMG
	MEMIMG					FontScreenMemImgNormal ;			// 半透明描画などの時に使うフォント用 MEMIMG ( アルファなし )
	MEMIMG					FontScreenMemImgAlpha ;				// 半透明描画などの時に使うフォント用 MEMIMG ( アルファつき )
	MEMIMG					*TargetMemImg ;						// 描画対象の MEMIMG
	MEMIMG					*BlendMemImg ;						// ブレンド描画用 MEMIMG
} ;

// グラフィクスシステム用データ構造体
struct GRAPHICSSYSTEMDATA
{
	int								InitializeFlag ;						// 初期化フラグ

//	int								NotDrawFlag ;							// 描画不可能フラグ

//	int								Screen3DWidth ;							// ３Ｄ描画を行う際の想定するスクリーンのサイズ
//	int								Screen3DHeight ;
//	int								Screen3DCenterX ;						// ３Ｄ描画を行う際の想定するスクリーンの中心座標
//	int								Screen3DCenterY ;
//	LONGLONG						FrameCounter ;							// フレームカウンター

	GRAPHICSSYS_SETTINGDATA			Setting ;								// 設定関係の情報

	GRAPHICSSYS_HARDWAREINFO		HardInfo ;								// ハードウェアアクセラレータ関係の情報

	GRAPHICSSYS_PERFORMANCEINFO		PerformanceInfo ;						// パフォーマンス関係の情報

	GRAPHICSSYS_SOFTRENDERDATA		SoftRender ;							// ソフトウエアレンダリングで使用する情報

	GRAPHICSSYS_SCREENDATA			Screen ;								// 画面関係の情報

	GRAPHICSSYS_CREATEIMAGEDATA		CreateImage ;							// 画像作成関係の情報

	GRAPHICSSYS_RESOURCE			Resource ;								// 処理実行用リソース関係の構造体

	GRAPHICSSYS_DRAWSETTINGDATA		DrawSetting ;							// 描画設定関係の情報

	GRAPHICSSYS_LIGHTATA			Light ;									// ライト関係の情報

	GRAPHICSSYS_CAMERA				Camera ;								// カメラ関係の情報

	int								ChangeSettingFlag ;						// 設定が変更されたかどうか
} ;

// 内部大域変数宣言---------------------------------------------------------------

// 描画周りの基本的な情報
extern GRAPHICSSYSTEMDATA GraphicsSysData ;

// 関数プロトタイプ宣言-----------------------------------------------------------

// グラフィック関連の初期化と後始末
extern	int		Graphics_Initialize( void ) ;					// グラフィックスシステムの初期化
extern	int		Graphics_Terminate( void ) ;					// グラフィックシステムの後始末
extern	int		Graphics_RestoreOrChangeSetupGraphSystem( int Change, int ScreenSizeX = -1, int ScreenSizeY = -1, int ColorBitDepth = -1, int RefreshRate = -1 ) ;		// グラフィックスシステムの復帰、又は変更付きの再セットアップを行う








// 画面関係関数
extern	int		Graphics_Screen_SetupFullScreenModeInfo( void ) ;											// フルスクリーンモードのモードのチェックや使用する解像度をの決定を行う
extern	int		Graphics_Screen_SetupFullScreenScalingDestRect( void ) ;									// GSYS.Screen.FullScreenScalingDestRect の値をセットアップする
extern	int		Graphics_Screen_ScreenPosConvSubBackbufferPos( int ScreenPosX, int ScreenPosY, int *BackBufferPosX, int *BackBufferPosY ) ;	// スクリーン座標をサブバックバッファー座標に変換する
extern	int		Graphics_Screen_SubBackbufferPosConvScreenPos( int BackBufferPosX, int BackBufferPosY, int *ScreenPosX, int *ScreenPosY ) ;	// サブバックバッファー座標をスクリーン座標に変換する
extern	int		Graphics_Screen_SetZBufferMode( int ZBufferSizeX, int ZBufferSizeY, int ZBufferBitDepth ) ;	// メイン画面のＺバッファの設定を変更する
extern	int		Graphics_Screen_SetupUseZBuffer( void ) ;													// 設定に基づいて使用するＺバッファをセットする
extern	void	Graphics_Screen_SetMainScreenSize( int SizeX, int SizeY ) ;									// メイン画面のサイズ値を変更する
extern	int		Graphics_Screen_ChangeMode( int ScreenSizeX, int ScreenSizeY, int ColorBitDepth, int ChangeWindowFlag, int RefreshRate ) ;				// 画面モードの変更２
extern	int		Graphics_Screen_LockDrawScreen( RECT *LockRect, BASEIMAGE *BaseImage, int TargetScreen/* = -1*/, int TargetScreenSurface/* = -1*/, int TargetScreenMipLevel, int ReadOnly/* = TRUE*/, int TargetScreenTextureNo/* = 0*/ ) ;	// 描画先バッファをロックする
extern	int		Graphics_Screen_UnlockDrawScreen( void ) ;													// 描画先バッファをアンロックする
extern	int		Graphics_Screen_FlipBase( void ) ;															// ScreenFlip のベース関数
extern	int		Graphics_Screen_ScreenCopyBase( int DrawTargetFrontScreenMode_Copy ) ;						// ScreenCopy のベース関数
extern	const GRAPHICSSYS_DISPLAYINFO *Graphics_GetRectMatchDisplayInfo( const RECT *Rect ) ;				// 指定の矩形領域に最も近いディスプレイを返す








// 画像関係関数
extern	int		Graphics_Image_SetupFormatDesc( IMAGEFORMATDESC *Format, SETUP_GRAPHHANDLE_GPARAM *GParam, int Width, int Height, int AlphaValidFlag, int UsePaletteFlag, int PaletteBitDepth, int BaseFormat, int MipMapCount ) ; // グラフィックハンドルに画像データを転送するための関数
extern	int		Graphics_Image_DeleteDeviceLostDelete( void ) ;						// デバイスロスト発生時に削除するフラグが立っているグラフィックを削除する
extern	int		Graphics_Image_CheckMultiSampleDrawValid( int GrHandle ) ;			// ＭＳＡＡを使用する描画可能画像かどうかを調べる( TRUE:MSAA画像  FALSE:MSAA画像ではない )
extern	int		Graphics_Image_AddHandle( int ASyncThread ) ;																			// 新しいグラフィックハンドルを確保する
extern	int		Graphics_Image_SetupHandle_UseGParam( SETUP_GRAPHHANDLE_GPARAM *GParam, int GrHandle, int Width, int Height, int TextureFlag, int AlphaValidFlag, int UsePaletteFlag, int PaletteBitDepth, int BaseFormat/* = DX_BASEIMAGE_FORMAT_NORMAL*/, int MipMapCount, int ASyncThread ) ;							// SetupGraphHandle のグローバル変数にアクセスしないバージョン
extern	int		Graphics_Image_ListUpTexSize( int Size, short *SizeList, int NotDivFlag, int Pow2Flag, int MaxTextureSize, int IsDXT ) ;	// 指定のテクスチャーサイズを上手く分割する
extern	int		Graphics_Image_InitializeHandle( HANDLEINFO *HandleInfo ) ;																// グラフィックハンドルの初期化
extern	int		Graphics_Image_TerminateHandle( HANDLEINFO *HandleInfo ) ;																// グラフィックハンドルの後始末
extern	int		Graphics_Image_InitializeDerivationHandle( int GrHandle, int IsFloat, int SrcXI, float SrcXF, int SrcYI, float SrcYF, int WidthI, float WidthF, int HeightI, float HeightF, int SrcGrHandle, int ASyncThread = FALSE ) ;			// 指定部分だけを抜き出したグラフィックハンドルを初期化する
extern	int		Graphics_Image_InitializeDrawInfo( int GrHandle, int IsFloat, int ASyncThread = FALSE ) ;			// グラフィックハンドルの描画情報を初期化する
extern	int		Graphics_Image_IsValidHandle( int GrHandle ) ;															// グラフィックハンドルが有効かどうかを調べる( TRUE:有効  FALSE:無効 )
extern	int		Graphics_Image_CreateDXGraph_UseGParam( SETUP_GRAPHHANDLE_GPARAM *GParam, int GrHandle, const BASEIMAGE *RgbBaseImage, const BASEIMAGE *AlphaBaseImage, int TextureFlag, int ASyncThread = FALSE ) ;																										// CreateDXGraph のグローバル変数にアクセスしないバージョン
extern	int		Graphics_Image_DerivationGraph_UseGParam( int IsFloat, int SrcXI, float SrcXF, int SrcYI, float SrcYF, int WidthI, float WidthF, int HeightI, float HeightF, int SrcGraphHandle, int ASyncThread = FALSE ) ;																																								// DerivationGraph のグローバル変数にアクセスしないバージョン
#ifndef DX_NON_MOVIE
extern	int		Graphics_Image_OpenMovie_UseGParam( LOADGRAPH_GPARAM *GParam, int GrHandle, const wchar_t *GraphName, const void *FileImage, size_t FileImageSize, int TextureFlag, int SurfaceMode = DX_MOVIESURFACE_NORMAL, int ImageSizeGetOnly = FALSE, int ASyncThread = FALSE ) ;
#endif
extern	int		Graphics_Image_DerivationGraphBase( int GrHandle, int IsFloat, int SrcXI, float SrcXF, int SrcYI, float SrcYF, int WidthI, float WidthF, int HeightI, float HeightF, int SrcGraphHandle, int ASyncThread = FALSE ) ;																												// グラフィックハンドルを作成しない DerivationGraph
extern	int		Graphics_Image_SetBaseInfo_UseGParam( SETGRAPHBASEINFO_GPARAM *GParam, int GrHandle, const wchar_t *FileName, const COLORDATA *BmpColorData, HBITMAP RgbBmp, HBITMAP AlphaBmp, const void *MemImage, int MemImageSize,
										   const void *AlphaMemImage, int AlphaMemImageSize, const BASEIMAGE *BaseImage, const BASEIMAGE *AlphaBaseImage, int ReverseFlag, int ConvertPremultipliedAlpha, int NotUseTransColor, int UnionGrHandle, int ASyncThread ) ;																														// SetGraphBaseInfo のグローバル変数にアクセスしないバージョン
extern	int		Graphics_Image_SetGraphBaseInfo( int GrHandle, const wchar_t *FileName, const COLORDATA *BmpColorData, HBITMAP RgbBmp, HBITMAP AlphaBmp, const void *MemImage, int MemImageSize, const void *AlphaMemImage, int AlphaMemImageSize, const BASEIMAGE *BaseImage, const BASEIMAGE *AlphaBaseImage, int ReverseFlag, int ConvertPremultipliedAlpha, int NotUseTransColor, int UnionGrHandle, int ASyncThread ) ;		// 画像の元データの情報を保存する
extern	int		Graphics_Image_SetName( int Handle, const wchar_t *GraphName, int ReverseFlag, int ConvertPremultipliedAlpha, int NotUseTransColor, int UnionGrHandle, int ASyncThread ) ;	// 特定のファイルから画像を読み込んだ場合のファイルパスをセットする
extern	int		Graphics_Image_FillGraph_UseGParam( int GrHandle, int Red, int Green, int Blue, int Alpha, int ASyncThread ) ;																																																// FillGraph のグローバル変数にアクセスしないバージョン
#ifndef DX_NON_MOVIE
extern	void	Graphics_Image_UpdateGraphMovie( MOVIEGRAPH *Movie, DWORD_PTR GrHandle ) ;				// ムービー画像を更新する
#endif
extern	int		Graphics_Image_CalcMipSize( int OrigWidth, int OrigHeight, int MipLevel, int *CalcWidth, int *CalcHeight ) ;		// 指定の MipLevel のサイズを計算する

// BltBmpOrGraphImageToGraph の内部関数
extern	int		Graphics_Image_BltBmpOrGraphImageToGraphBase(
	const BASEIMAGE	*RgbBaseImage,
	const BASEIMAGE	*AlphaBaseImage,
	      int		CopyPointX,
	      int		CopyPointY,
	      int		GrHandle,
	      int		UseTransColorConvAlpha = TRUE,
	      int		ASyncThread = FALSE
) ;

// BltBmpOrGraphImageToGraph2 の内部関数
extern	int		Graphics_Image_BltBmpOrGraphImageToGraph2Base(
	const BASEIMAGE	*RgbBaseImage,
	const BASEIMAGE	*AlphaBaseImage,
	const RECT		*SrcRect,
	      int		DestX,
	      int		DestY,
	      int		GrHandle,
	      int		UseTransColorConvAlpha = TRUE,
	      int		ASyncThread = FALSE
) ;

// BltBmpOrGraphImageToDivGraph の内部関数
extern	int		Graphics_Image_BltBmpOrGraphImageToDivGraphBase(
	const BASEIMAGE	*RgbBaseImage,
	const BASEIMAGE	*AlphaBaseImage,
	      int		AllNum,
	      int		XNum,
	      int		YNum,
		  int		IsFloat,
	      int		WidthI,
	      float		WidthF,
	      int		HeightI,
	      float		HeightF,
	const int		*GrHandle,
	      int		ReverseFlag,
	      int		UseTransColorConvAlpha = TRUE,
	      int		ASyncThread = FALSE
) ;

// グラフィックハンドルに画像データを転送するための関数
extern	int		Graphics_Image_BltBmpOrBaseImageToGraph3(
	const RECT		*SrcRect,
	      int		DestX,
	      int		DestY,
	      int		GrHandle,
	const BASEIMAGE	*RgbBaseImage,
	const BASEIMAGE	*AlphaBaseImage,
	      int		RedIsAlphaFlag = FALSE,
	      int		UseTransColorConvAlpha = TRUE,
	      int		TargetOrig = FALSE,
	      int		ASyncThread = FALSE
) ;

// 指定のオリジナル画像情報に転送する矩形情報を作成する
// 戻り値  -1:範囲外   0:正常終了
extern	int		Graphics_Image_BltBmpOrBaseImageToGraph3_Make_OrigTex_MoveRect(
	const IMAGEDATA_ORIG_HARD_TEX	*OrigTex,
	const RECT						*SrcRect,
		  int						SrcWidth,
		  int						SrcHeight,
	      int						DestX,
	      int						DestY,
	      RECT						*DestRect,
	      RECT						*MoveRect,
		  int						IsDXT
) ;

// 指定の描画用画像情報に転送する矩形情報を作成する
// 戻り値  -1:範囲外   0:正常終了
extern	int		Graphics_Image_BltBmpOrBaseImageToGraph3_Make_DrawTex_MoveRect(
	const IMAGEDATA_HARD_DRAW		*DrawTex,
	const RECT						*SrcRect,
		  int						SrcWidth,
		  int						SrcHeight,
	      int						DestX,
	      int						DestY,
	      RECT						*DestRect,
	      RECT						*MoveRect,
		  int						IsDXT
) ;

// 描画可能画像やバックバッファから指定領域のグラフィックを取得する
extern	int			Graphics_Image_GetDrawScreenGraphBase( int TargetScreen, int TargetScreenSurface, int TargetScreenMipLevel, int x1, int y1, int x2, int y2, int destX, int destY, int GrHandle ) ;
extern	IMAGEDATA *	Graphics_Image_GetData( int GrHandle, int ASyncThread = FALSE ) ;				// グラフィックのデータをインデックス値から取り出す
extern	int			Graphics_Image_GetWhiteTexHandle( void ) ;										// 真っ白のテクスチャのハンドルを取得する
extern	int			Graphics_Image_GetLineTexHandle( int IsPMA ) ;									// アンチエイリアス付きの線を描画するためのテクスチャハンドルを取得する
extern	int			Graphics_Image_GetLineOneThicknessTexHandle( int IsPMA ) ;						// アンチエイリアス付きの線を描画するためのテクスチャハンドルを取得する( 太さ1ピクセル用 )
extern	int			Graphics_Image_GetLineBoxTexHandle( int IsPMA ) ;								// アンチエイリアス付きの線矩形を描画するためのテクスチャハンドルを取得する
extern	int			Graphics_Image_GetLineBoxOneThicknessTexHandle( int IsPMA ) ;					// アンチエイリアス付きの線矩形を描画するためのテクスチャハンドルを取得する( 太さ1ピクセル用 )

extern	int		Graphics_Image_MakeGraph_UseGParam( SETUP_GRAPHHANDLE_GPARAM *GParam, int SizeX, int SizeY, int NotUse3DFlag, int UsePaletteFlag, int PaletteBitDepth, int ASyncLoadFlag = FALSE, int ASyncThread = FALSE ) ;																																											// 空のグラフィックハンドルを作成する関数
extern	int		Graphics_Image_CreateGraph_UseGParam(                  LOADGRAPH_PARAM *Param, int ASyncLoadFlag = FALSE, int ASyncThread = FALSE ) ;																																																			// 画像データからグラフィックハンドルを作成する関数
extern	int		Graphics_Image_CreateDivGraph_UseGParam(               LOADGRAPH_PARAM *Param, int ASyncLoadFlag = FALSE, int ASyncThread = FALSE ) ;																																																			// 画像データを分割してグラフィックハンドルを作成する関数
extern	int		Graphics_Image_LoadBmpToGraph_UseGParam(               LOADGRAPH_GPARAM *GParam, int ReCreateFlag, int GrHandle, const wchar_t *GraphName, int TextureFlag, int ReverseFlag, int SurfaceMode = DX_MOVIESURFACE_NORMAL, int ASyncLoadFlag = FALSE ) ;																								// LoadBmpToGraph のグローバル変数にアクセスしないバージョン
extern	int		Graphics_Image_LoadDivBmpToGraph_UseGParam(            LOADGRAPH_GPARAM *GParam, int ReCreateFlag, const wchar_t *FileName, int AllNum, int XNum, int YNum, int IsFloat, int SizeXI, float SizeXF, int SizeYI, float SizeYF, int *HandleArray, int TextureFlag, int ReverseFlag, int ASyncLoadFlag = FALSE, int ASyncThread = FALSE ) ;																	// LoadDivBmpToGraph のグローバル変数にアクセスしないバージョン
extern	int		Graphics_Image_CreateGraphFromMem_UseGParam(           LOADGRAPH_GPARAM *GParam, int ReCreateFlag, int GrHandle, const void *MemImage, int MemImageSize, const void *AlphaImage = NULL, int AlphaImageSize = 0, int TextureFlag = TRUE, int ReverseFlag = FALSE, int ASyncLoadFlag = FALSE ) ;																// CreateGraphFromMem のグローバル変数にアクセスしないバージョン
extern	int		Graphics_Image_CreateDivGraphFromMem_UseGParam(        LOADGRAPH_GPARAM *GParam, int ReCreateFlag, const void *MemImage, int MemImageSize, int AllNum, int XNum, int YNum, int IsFloat, int SizeXI, float SizeXF, int SizeYI, float SizeYF, int *HandleArray,int TextureFlag, int ReverseFlag, const void *AlphaImage, int AlphaImageSize, int ASyncLoadFlag = FALSE, int ASyncThread = FALSE ) ;		// CreateDivGraphFromMem のグローバル変数にアクセスしないバージョン
extern	int		Graphics_Image_CreateGraphFromBmp_UseGParam(           LOADGRAPH_GPARAM *GParam, int ReCreateFlag, int GrHandle, const BITMAPINFO *BmpInfo, const void *GraphData, const BITMAPINFO *AlphaInfo = NULL, const void *AlphaData = NULL, int TextureFlag = TRUE, int ReverseFlag = FALSE, int ASyncLoadFlag = FALSE ) ;											// CreateGraphFromBmp のグローバル変数にアクセスしないバージョン
extern	int		Graphics_Image_CreateDivGraphFromBmp_UseGParam(        LOADGRAPH_GPARAM *GParam, int ReCreateFlag, const BITMAPINFO *BmpInfo, const void *GraphData, int AllNum, int XNum, int YNum, int IsFloat, int SizeXI, float SizeXF, int SizeYI, float SizeYF, int *HandleArray,int TextureFlag, int ReverseFlag, const BITMAPINFO *AlphaInfo, const void *AlphaData, int ASyncLoadFlag = FALSE ) ;	// CreateDivGraphFromBmp のグローバル変数にアクセスしないバージョン
extern	int		Graphics_Image_CreateGraphFromGraphImage_UseGParam(    LOADGRAPH_GPARAM *GParam, int ReCreateFlag, int GrHandle, const BASEIMAGE *Image, const BASEIMAGE *AlphaImage, int TextureFlag = TRUE , int ReverseFlag = FALSE, int ASyncLoadFlag = FALSE, int ASyncThread = FALSE ) ;																	// CreateGraphFromGraphImage のグローバル変数にアクセスしないバージョン
extern	int		Graphics_Image_CreateDivGraphFromGraphImage_UseGParam( LOADGRAPH_GPARAM *GParam, int ReCreateFlag, const BASEIMAGE *Image, const BASEIMAGE *AlphaImage, int AllNum, int XNum, int YNum, int IsFloat, int SizeXI, float SizeXF, int SizeYI, float SizeYF, int *HandleArray, int TextureFlag = TRUE , int ReverseFlag = FALSE, int ASyncLoadFlag = FALSE ) ;									// CreateDivGraphFromGraphImage のグローバル変数にアクセスしないバージョン
extern	int		Graphics_Image_CreateGraphFromGraphImageBase_UseGParam(    CREATE_GRAPHHANDLE_AND_BLTGRAPHIMAGE_GPARAM *GParam, int ReCreateFlag, int GrHandle,   BASEIMAGE *Image, const BASEIMAGE *AlphaImage, int TextureFlag, int ASyncThread = FALSE ) ;																							// Graphics_Image_CreateGraphFromGraphImageBase のグローバル変数にアクセスしないバージョン
extern	int		Graphics_Image_CreateDivGraphFromGraphImageBase_UseGParam( CREATE_GRAPHHANDLE_AND_BLTGRAPHIMAGE_GPARAM *GParam, int ReCreateFlag, int BaseHandle, BASEIMAGE *Image, const BASEIMAGE *AlphaImage, int AllNum, int XNum, int YNum, int IsFloat, int SizeXI, float SizeXF, int SizeYI, float SizeYF, int *HandleArray, int TextureFlag, int ReverseFlag, int ASyncThread = FALSE ) ;		// Graphics_Image_CreateDivGraphFromGraphImageBase のグローバル変数にアクセスしないバージョン
extern	int		Graphics_Image_CreateGraphFromGraphImageBase(      BASEIMAGE *Image, const BASEIMAGE *AlphaImage, int TextureFlag, int ASyncThread ) ;																								// CreateGraphFromGraphImage の内部関数
extern	int		Graphics_Image_CreateDivGraphFromGraphImageBase(   BASEIMAGE *Image, const BASEIMAGE *AlphaImage, int AllNum, int XNum, int YNum, int IsFloat, int SizeXI, float SizeXF, int SizeYI, float SizeYF, int *HandleArray, int TextureFlag, int ReverseFlag ) ;						// CreateDivGraphFromGraphImage の内部関数
extern	int		Graphics_Image_ReCreateGraphFromGraphImageBase(    BASEIMAGE *Image, const BASEIMAGE *AlphaImage, int GrHandle, int TextureFlag ) ;																									// ReCreateGraphFromGraphImage の内部関数
extern	int		Graphics_Image_ReCreateDivGraphFromGraphImageBase( BASEIMAGE *Image, const BASEIMAGE *AlphaImage, int AllNum, int XNum, int YNum, int IsFloat, int SizeXI, float SizeXF, int SizeYI, float SizeYF, int *HandleArray, int TextureFlag , int ReverseFlag ) ;						// ReCreateDivGraphFromGraphImage の内部関数
extern	void	Graphics_Image_InitCreateGraphHandleAndBltGraphImageGParam( CREATE_GRAPHHANDLE_AND_BLTGRAPHIMAGE_GPARAM *GParam ) ;			// CREATE_GRAPHHANDLE_AND_BLTGRAPHIMAGE_GPARAM のデータをセットする
extern	void	Graphics_Image_InitSetupGraphHandleGParam( SETUP_GRAPHHANDLE_GPARAM *GParam ) ;												// SETUP_GRAPHHANDLE_GPARAM のデータをセットする
extern	void	Graphics_Image_InitSetupGraphHandleGParam_Normal_NonDrawValid( SETUP_GRAPHHANDLE_GPARAM *GParam, int BitDepth = 32, int AlphaChannel = TRUE, int AlphaTest = FALSE ) ;
extern	void	Graphics_Image_InitSetupGraphHandleGParam_Normal_DrawValid_NoneZBuffer( SETUP_GRAPHHANDLE_GPARAM *GParam, int BitDepth = 32, int AlphaChannel = TRUE ) ;
extern	void	Graphics_Image_InitSetGraphBaseInfoGParam( SETGRAPHBASEINFO_GPARAM *GParam ) ;													// SETGRAPHBASEINFO_GPARAM のデータをセットする
extern	void	Graphics_Image_InitLoadGraphGParam( LOADGRAPH_GPARAM *GParam ) ;																// LOADGRAPH_GPARAM のデータをセットする
#ifndef DX_NON_MOVIE
extern	void	Graphics_Image_InitOpenMovieGParam( OPENMOVIE_GPARAM *GParam ) ;																// OPENMOVIE_GPARAM のデータをセットする
#endif







// 描画設定関係関数
extern	int				Graphics_DrawSetting_Initialize( void ) ;															// グラフィック描画設定関係の情報を初期化
extern	void FASTCALL	Graphics_DrawSetting_SetDrawBrightToOneParam( DWORD Bright ) ;										// SetDrawBright の引数が一つ版
extern	void FASTCALL	Graphics_DrawSetting_BlendModeSub_Pre( RECT *DrawRect ) ;
extern	void FASTCALL	Graphics_DrawSetting_BlendModeSub_Post( RECT *DrawRect ) ;
extern	int				Graphics_DrawSetting_SetBlendGraphParamBase( int BlendGraph, int BlendType, va_list ParamList ) ;	// SetBlendGraphParam の可変長引数パラメータ付き
extern	int				Graphics_DrawSetting_RefreshAlphaChDrawMode( void ) ;												// 描画先に正しいα値を書き込むかどうかのフラグを更新する
extern	void			Graphics_DrawSetting_ApplyLib2DMatrixToHardware( void ) ;											// 基本データに設定されている２Ｄ行列をハードウエアに反映する
extern	void			Graphics_DrawSetting_ApplyLib3DMatrixToHardware( int IgnoreWorldMatrix = FALSE ) ;					// 基本データに設定されている３Ｄ行列をハードウエアに反映する
extern	void			Graphics_DrawSetting_ApplyLibFogToHardware( void ) ;												// 基本データに設定されているフォグ情報をハードウエアに反映する
extern	int				Graphics_DrawSetting_SetTextureAddressTransformMatrix_Direct( int Use, MATRIX *Matrix, int Stage = -1 ) ;		// テクスチャ座標変換行列をセットする
extern	int				Graphics_DrawSetting_SetTransformToWorld_Direct( MATRIX *Matrix ) ;									// ローカル→ワールド行列を変更する
extern	int				Graphics_DrawSetting_SetTransformToProjection_Direct( const MATRIX_D *Matrix ) ;					// 投影変換用行列をセットする
extern	int				Graphics_DrawSetting_SetupDefaultDrawAreaAndCamera( int OldScreen ) ;								// SetDrawScreen 実行時に行う描画範囲設定、カメラ設定を行う
extern	void			Graphics_DrawSetting_RefreshProjectionMatrix( void ) ;												// 射影行列パラメータに基づいて射影行列を構築する
extern	void			Graphics_DrawSetting_RefreshBlend2DTransformMatrix( void ) ;										// ２Ｄ用の頂点変換行列を掛け合わせた行列を更新する
extern	void			Graphics_DrawSetting_RefreshBlend3DTransformMatrix( void ) ;										// ３Ｄ用の頂点変換行列を掛け合わせた行列を更新する
//extern	void		Graphics_DrawSetting_SetUse2DProjectionMatrix( int Use2DProjectionMatrix ) ;						// 使用する射影行列を３Ｄ用にするか２Ｄ用にするかを設定する
extern	int				Graphics_DrawSetting_GetScreenDrawSettingInfo(       SCREENDRAWSETTINGINFO *ScreenDrawSettingInfo ) ;	// ＤＸライブラリ内部で SetDrawScreen を使用して描画先を変更する際の、元のカメラ設定や描画領域を復元する為の情報を取得する処理を行う
extern	int				Graphics_DrawSetting_SetScreenDrawSettingInfo( const SCREENDRAWSETTINGINFO *ScreenDrawSettingInfo ) ;	// ＤＸライブラリ内部で SetDrawScreen を使用して描画先を変更する際の、元のカメラ設定や描画領域を復元する処理を行う








// 描画関係関数
extern	int		Graphics_Draw_GetCircle_ThicknessDrawPosition( int x, int y, int r, int Thickness, short ( *CirclePos )[ 5 ] ) ;		// 線の幅付き円の描画用頂点を取得する関数
extern	int		Graphics_Draw_GetOval_ThicknessDrawPosition( int x, int y, int rx, int ry, int Thickness, short ( *CirclePos )[ 5 ] ) ;	// 線の幅付き楕円の描画用頂点を取得する関数
extern	int		Graphics_Draw_DrawSimpleQuadrangleGraphF( const GRAPHICS_DRAW_DRAWSIMPLEQUADRANGLEGRAPHF_PARAM *Param ) ;				// 座標補正を行わない２ポリゴン描画を行う( １テクスチャ画像のみ有効 )
extern	int		Graphics_Draw_DrawSimpleTriangleGraphF(   const GRAPHICS_DRAW_DRAWSIMPLETRIANGLEGRAPHF_PARAM   *Param ) ;				// 座標補正を行わない１ポリゴン描画を行う( １テクスチャ画像のみ有効 )








// カメラ関係関数
extern	int		Graphics_Camera_CheckCameraViewClip_Box_PosDim(  VECTOR   *CheckBoxPos ) ;					// ８座標で形作るボックスがカメラの視界に入っているかどうかを判定する( 戻り値 TRUE:視界に入っていない  FALSE:視界に入っている )( CheckPosは VECTOR 8個分の配列の先頭アドレス、配列の各要素番号の内容 0:+x +y +z   1:-x +y +z   2:-x -y +z   3:+x -y +z   4:+x -y -z   5:+x +y -z   6:-x +y -z   7:-x -y -z )
extern	int		Graphics_Camera_CheckCameraViewClip_Box_PosDimD( VECTOR_D *CheckBoxPos ) ;					// ８座標で形作るボックスがカメラの視界に入っているかどうかを判定する( 戻り値 TRUE:視界に入っていない  FALSE:視界に入っている )( CheckPosは VECTOR 8個分の配列の先頭アドレス、配列の各要素番号の内容 0:+x +y +z   1:-x +y +z   2:-x -y +z   3:+x -y +z   4:+x -y -z   5:+x +y -z   6:-x +y -z   7:-x -y -z )
extern	void	Graphics_Camera_CalcCameraRollViewMatrix( void ) ;											// ビュー行列から水平、垂直、捻り角度を算出する








// ライト関係関数
extern	int		Graphics_Light_AddHandle( void ) ;														// ライトハンドルの追加
extern	int		Graphics_Light_RefreshState( void ) ;													// ライトの変更を反映する








// シェーダー関係関数
extern	int		Graphics_Shader_CreateHandle( int ASyncThread ) ;																	// シェーダーハンドルを作成する
extern	int		Graphics_Shader_CreateHandle_UseGParam( int IsVertexShader, void *Image, int ImageSize, int ImageAfterFree, int ASyncLoadFlag = FALSE, int ASyncThread = FALSE ) ;		// シェーダーハンドルを作成する
extern	int		Graphics_Shader_LoadShader_UseGParam( int IsVertexShader, const wchar_t *FileName, int ASyncLoadFlag = FALSE ) ;	// シェーダーバイナリをファイルから読み込む
extern	int		Graphics_Shader_InitializeHandle( HANDLEINFO *HandleInfo ) ;														// シェーダーハンドルの初期化
extern	int		Graphics_Shader_TerminateHandle( HANDLEINFO *HandleInfo ) ;															// シェーダーハンドルの後始末
extern	SHADERHANDLEDATA *Graphics_Shader_GetData( int ShaderHandle, int ASyncThread = FALSE ) ;									// シェーダーデータをハンドル値から取り出す







// 定数バッファ関係関数
extern	int		Graphics_ShaderConstantBuffer_CreateHandle( int ASyncThread ) ;																// シェーダー用定数バッファハンドルを作成する
extern	int		Graphics_ShaderConstantBuffer_Create( int BufferSize, int ASyncLoadFlag = FALSE,  int ASyncThread = FALSE ) ;				// シェーダー用定数バッファハンドルを作成する
extern	int		Graphics_ShaderConstantBuffer_InitializeHandle( HANDLEINFO *HandleInfo ) ;													// シェーダー用定数バッファハンドルの初期化
extern	int		Graphics_ShaderConstantBuffer_TerminateHandle( HANDLEINFO *HandleInfo ) ;													// シェーダー用定数バッファハンドルの後始末








// 頂点バッファ・インデックスバッファ関係関数
extern	int		Graphics_VertexBuffer_Create( int VertexNum, int VertexType /* DX_VERTEX_TYPE_NORMAL_3D 等 */, int ASyncThread ) ;			// 頂点バッファを作成する
extern	int		Graphics_VertexBuffer_SetupHandle( int VertexBufHandle, int VertexNum, int VertexType /* DX_VERTEX_TYPE_NORMAL_3D 等 */ ) ;	// 頂点バッファハンドルのセットアップを行う
extern	int		Graphics_VertexBuffer_InitializeHandle( HANDLEINFO *HandleInfo ) ;															// 頂点バッファハンドルの初期化
extern	int		Graphics_VertexBuffer_TerminateHandle( HANDLEINFO *HandleInfo ) ;															// 頂点バッファハンドルの後始末

extern	int		Graphics_IndexBuffer_Create( int IndexNum, int IndexType /* DX_INDEX_TYPE_16BIT 等 */, int ASyncThread ) ;					// インデックスバッファを作成する
extern	int		Graphics_IndexBuffer_SetupHandle( int IndexBufHandle, int IndexNum, int IndexType /* DX_INDEX_TYPE_16BIT 等 */ ) ;			// インデックスバッファハンドルのセットアップを行う
extern	int		Graphics_IndexBuffer_InitializeHandle( HANDLEINFO *HandleInfo ) ;															// インデックスバッファハンドルの初期化
extern	int		Graphics_IndexBuffer_TerminateHandle( HANDLEINFO *HandleInfo ) ;															// インデックスバッファハンドルの後始末








// シャドウマップ関係関数
extern	int		Graphics_ShadowMap_MakeShadowMap_UseGParam( SETUP_SHADOWMAPHANDLE_GPARAM *GParam, int SizeX, int SizeY, int ASyncLoadFlag = FALSE, int ASyncThread = FALSE ) ;																																														// シャドウマップハンドルを作成する
extern	int		Graphics_ShadowMap_AddHandle( int ASyncThread ) ;																			// 新しいシャドウマップハンドルを確保する
extern	int		Graphics_ShadowMap_SetupHandle_UseGParam( SETUP_SHADOWMAPHANDLE_GPARAM *GParam, int SmHandle, int SizeX, int SizeY, int TexFormat_Float, int TexFormat_BitDepth, int ASyncThread ) ;	// シャドウマップハンドルのセットアップを行う
extern	int		Graphics_ShadowMap_CreateTexture( SHADOWMAPDATA *ShadowMap, int ASyncThread = FALSE ) ;										// シャドウマップデータに必要なテクスチャを作成する
extern	int		Graphics_ShadowMap_ReleaseTexture( SHADOWMAPDATA *ShadowMap ) ;																// シャドウマップデータに必要なテクスチャを解放する
extern	SHADOWMAPDATA	*Graphics_ShadowMap_GetData( int SmHandle, int ASyncThread = FALSE ) ;												// シャドウマップデータをハンドル値から取り出す
extern	void	Graphics_ShadowMap_RefreshVSParam( void ) ;																					// 頂点シェーダーに設定するシャドウマップの情報を更新する
extern	void	Graphics_ShadowMap_RefreshPSParam( void ) ;																					// ピクセルシェーダーに設定するシャドウマップの情報を更新する
extern	int		Graphics_ShadowMap_RefreshMatrix( SHADOWMAPDATA *ShadowMap ) ;							// シャドウマップへのレンダリングで使用する行列の情報を更新する

extern	int		Graphics_ShadowMap_InitializeHandle( HANDLEINFO *HandleInfo ) ;																// シャドウマップハンドルの初期化
extern	int		Graphics_ShadowMap_TerminateHandle( HANDLEINFO *HandleInfo ) ;																// シャドウマップハンドルの後始末








// 補助関係関数
extern	int		Graphics_Other_AllocCommonBuffer( int Index, DWORD Size ) ;						// 共有メモリの確保
extern	int		Graphics_Other_TerminateCommonBuffer( void ) ;									// 共有メモリの解放








// 環境依存初期化関係
extern	int		Graphics_Initialize_Timing0_PF( void ) ;										// 描画処理の環境依存部分の初期化を行う関数( 実行箇所区別０ )
extern	int		Graphics_Initialize_Timing1_PF( void ) ;										// 描画処理の環境依存部分の初期化を行う関数( 実行箇所区別２ )
extern	int		Graphics_Hardware_Initialize_PF( void ) ;										// ハードウエアアクセラレータを使用する場合の環境依存の初期化処理を行う
extern	int		Graphics_Terminate_PF( void ) ;													// 描画処理の環境依存部分の後始末を行う関数
extern	int		Graphics_RestoreOrChangeSetupGraphSystem_PF( int Change, int ScreenSizeX = -1, int ScreenSizeY = -1, int ColorBitDepth = -1, int RefreshRate = -1 ) ;		// グラフィックスシステムの復帰、又は変更付きの再セットアップを行う
extern	int		Graphics_Hardware_CheckValid_PF( void ) ;										// 描画用デバイスが有効かどうかを取得する( 戻り値  TRUE:有効  FALSE:無効 )







// 環境依存描画設定関係
extern	int		Graphics_Hardware_SetRenderTargetToShader_PF( int TargetIndex, int DrawScreen, int SurfaceIndex , int MipLevel ) ;		// シェーダー描画での描画先を設定する
extern	int		Graphics_Hardware_SetBackgroundColor_PF( int Red, int Green, int Blue ) ;								// メインウインドウの背景色を設定する( Red,Green,Blue:それぞれ ０〜２５５ )
extern	int		Graphics_Hardware_SetDrawBrightToOneParam_PF( DWORD Bright ) ;											// SetDrawBright の引数が一つ版
extern	int		Graphics_Hardware_SetDrawBlendMode_PF( int BlendMode, int BlendParam ) ;								// 描画ブレンドモードをセットする
extern	int		Graphics_Hardware_SetDrawAlphaTest_PF( int TestMode, int TestParam ) ;									// 描画時のアルファテストの設定を行う( TestMode:DX_CMP_GREATER等( -1:デフォルト動作に戻す )  TestParam:描画アルファ値との比較に使用する値 )
extern	int		Graphics_Hardware_SetDrawMode_PF( int DrawMode ) ;														// 描画モードをセットする
extern	int		Graphics_Hardware_SetDrawBright_PF( int RedBright, int GreenBright, int BlueBright ) ;					// 描画輝度をセット
extern	int		Graphics_Hardware_SetBlendGraphParamBase_PF( IMAGEDATA *BlendImage, int BlendType, int *Param ) ;		// SetBlendGraphParam の可変長引数パラメータ付き
extern	int		Graphics_Hardware_SetMaxAnisotropy_PF( int MaxAnisotropy ) ;											// 最大異方性の値をセットする
extern	int		Graphics_Hardware_SetTransformToWorld_PF( const MATRIX *Matrix ) ;										// ワールド変換用行列をセットする
extern	int		Graphics_Hardware_SetTransformToView_PF( const MATRIX *Matrix ) ;										// ビュー変換用行列をセットする
extern	int		Graphics_Hardware_SetTransformToProjection_PF( const MATRIX *Matrix ) ;									// 投影変換用行列をセットする
extern	int		Graphics_Hardware_SetTransformToViewport_PF( const MATRIX *Matrix ) ;									// ビューポート行列をセットする
extern	int		Graphics_Hardware_SetTextureAddressMode_PF( int Mode /* DX_TEXADDRESS_WRAP 等 */, int Stage ) ;			// テクスチャアドレスモードを設定する
extern	int		Graphics_Hardware_SetTextureAddressModeUV_PF( int ModeU, int ModeV, int Stage ) ;						// テクスチャアドレスモードを設定する
extern	int		Graphics_Hardware_SetTextureAddressTransformMatrix_PF( int UseFlag, MATRIX *Matrix, int Sampler = -1 ) ;// テクスチャ座標変換行列をセットする
extern	int		Graphics_Hardware_SetFogEnable_PF( int Flag ) ;															// フォグを有効にするかどうかを設定する( TRUE:有効  FALSE:無効 )
extern	int		Graphics_Hardware_SetFogMode_PF( int Mode /* DX_FOGMODE_NONE 等 */ ) ;									// フォグモードを設定する
extern	int		Graphics_Hardware_SetFogColor_PF( DWORD FogColor ) ;													// フォグカラーを変更する
extern	int		Graphics_Hardware_SetFogStartEnd_PF( float start, float end ) ;											// フォグが始まる距離と終了する距離を設定する( 0.0f 〜 1.0f )
extern	int		Graphics_Hardware_SetFogDensity_PF( float density ) ;													// フォグの密度を設定する( 0.0f 〜 1.0f )
extern	int		Graphics_Hardware_DeviceDirect_SetWorldMatrix_PF( const MATRIX *Matrix ) ;								// ワールド変換用行列をセットする
extern	int		Graphics_Hardware_DeviceDirect_SetViewMatrix_PF( const MATRIX *Matrix ) ;								// ビュー変換用行列をセットする
extern	int		Graphics_Hardware_DeviceDirect_SetProjectionMatrix_PF( const MATRIX *Matrix ) ;							// 投影変換用行列をセットする
extern	int		Graphics_Hardware_ApplyLigFogToHardware_PF( void ) ;													// 基本データに設定されているフォグ情報をハードウェアに反映する
extern	int		Graphics_Hardware_SetUseOldDrawModiGraphCodeFlag_PF( int Flag ) ;										// 以前の DrawModiGraph 関数のコードを使用するかどうかのフラグをセットする
extern	int		Graphics_Hardware_RefreshAlphaChDrawMode_PF( void ) ;													// 描画先に正しいα値を書き込むかどうかのフラグを更新する
//extern	void	Graphics_Hardware_SetUse2DProjectionMatrix_PF( int Use2DProjectionMatrix ) ;							// 使用する射影行列を３Ｄ用にするか２Ｄ用にするかを設定する









// 環境依存設定関係
extern	int		Graphics_Hardware_SetUseHardwareVertexProcessing_PF( int Flag ) ;						// ハードウエアの頂点演算処理機能を使用するかどうかを設定する
extern	int		Graphics_Hardware_SetUsePixelLighting_PF( int Flag ) ;									// ピクセル単位でライティングを行うかどうかを設定する、要 ShaderModel 3.0( TRUE:ピクセル単位のライティングを行う  FALSE:頂点単位のライティングを行う( デフォルト ) )
extern	int		Graphics_Hardware_SetGraphicsDeviceRestoreCallbackFunction_PF( void (* Callback )( void *Data ), void *CallbackData ) ;			// グラフィックスデバイスがロストから復帰した際に呼ばれるコールバック関数を設定する
extern	int		Graphics_Hardware_SetGraphicsDeviceLostCallbackFunction_PF( void (* Callback )( void *Data ), void *CallbackData ) ;			// グラフィックスデバイスがロストから復帰する前に呼ばれるコールバック関数を設定する
extern	int		Graphics_Hardware_SetUseNormalDrawShader_PF( int Flag ) ;								// 通常描画にプログラマブルシェーダーを使用するかどうかを設定する( TRUE:使用する( デフォルト )  FALSE:使用しない )
extern	int		Graphics_Hardware_GetVideoMemorySize_PF( int *AllSize, int *FreeSize ) ;				// ビデオメモリの容量を得る
extern	int		Graphics_SetAeroDisableFlag_PF( int Flag ) ;											// Vista以降の Windows Aero を無効にするかどうかをセットする、TRUE:無効にする  FALSE:有効にする( DxLib_Init の前に呼ぶ必要があります )












// 環境依存画面関係
extern	int		Graphics_Hardware_SetupUseZBuffer_PF( void ) ;															// 設定に基づいて使用するＺバッファをセットする
extern	int		Graphics_Hardware_ClearDrawScreenZBuffer_PF( const RECT *ClearRect ) ;									// 画面のＺバッファの状態を初期化する
extern	int		Graphics_Hardware_ClearDrawScreen_PF( const RECT *ClearRect ) ;											// 画面の状態を初期化する
extern	int		Graphics_Hardware_SetDrawScreen_PF( int DrawScreen, int OldScreenSurface, int OldScreenMipLevel, IMAGEDATA *NewTargetImage, IMAGEDATA *OldTargetImage, SHADOWMAPDATA *NewTargetShadowMap, SHADOWMAPDATA *OldTargetShadowMap ) ;					// 描画先画面のセット
extern	int		Graphics_Hardware_SetDrawScreen_Post_PF( int DrawScreen ) ;												// SetDrawScreen の最後で呼ばれる関数
extern	int		Graphics_Hardware_SetDrawArea_PF( int x1, int y1, int x2, int y2 ) ;									// 描画可能領域のセット
extern	int		Graphics_Hardware_LockDrawScreenBuffer_PF( RECT *LockRect, BASEIMAGE *BaseImage, int TargetScreen, IMAGEDATA *TargetImage, int TargetScreenSurface, int TargetScreenMipLevel, int ReadOnly, int TargetScreenTextureNo ) ;	// 描画先バッファをロックする
extern	int		Graphics_Hardware_UnlockDrawScreenBuffer_PF( void ) ;													// 描画先バッファをアンロックする
extern	int		Graphics_Hardware_ScreenCopy_PF( int DrawTargetFrontScreenMode_Copy ) ;									// 裏画面の内容を表画面に描画する
extern	int		Graphics_SetupDisplayInfo_PF( void ) ;																	// ディスプレイの情報をセットアップする
extern	int		Graphics_Hardware_WaitVSync_PF( int SyncNum ) ;															// 垂直同期信号を待つ
extern	int		Graphics_SetWaitVSyncFlag_PF( int Flag ) ;																// ScreenFlip 実行時にＶＳＹＮＣ待ちをするかどうかを設定する
extern	int		Graphics_ScreenFlipBase_PF( void ) ;																	// 裏画面と表画面を交換する
#ifdef __WINDOWS__
extern	int		Graphics_BltRectBackScreenToWindow_PF( HWND Window, RECT BackScreenRect, RECT WindowClientRect ) ;		// 裏画面の指定の領域をウインドウのクライアント領域の指定の領域に転送する
extern	int		Graphics_SetScreenFlipTargetWindow_PF( HWND TargetWindow, double ScaleX, double ScaleY ) ;				// ScreenFlip で画像を転送する先のウインドウを設定する( NULL を指定すると設定解除 )
#endif // __WINDOWS__
extern	int		Graphics_Hardware_SetZBufferMode_PF( int ZBufferSizeX, int ZBufferSizeY, int ZBufferBitDepth ) ;		// メイン画面のＺバッファの設定を変更する
extern	int		Graphics_Hardware_SetDrawZBuffer_PF( int DrawScreen, IMAGEDATA *Image ) ;								// 描画先Ｚバッファのセット
extern	int		Graphics_GetRefreshRate_PF( void ) ;																	// 現在の画面のリフレッシュレートを取得する













// 環境依存情報取得関係
extern	const COLORDATA *	Graphics_Hardware_GetMainColorData_PF( void ) ;				// GetColor や GetColor2 で使用するカラーデータを取得する
extern	const COLORDATA *	Graphics_Hardware_GetDispColorData_PF( void ) ;				// ディスプレーのカラーデータポインタを得る
extern	DWORD				Graphics_Hardware_GetPixel_PF( int x, int y ) ;				// 指定座標の色を取得する
extern	COLOR_F				Graphics_Hardware_GetPixelF_PF( int x, int y ) ;			// 指定座標の色を取得する( float型 )











// 環境依存画像関係
extern	int		Graphics_Hardware_UpdateGraphMovie_TheoraYUV_PF( struct MOVIEGRAPH *Movie, IMAGEDATA *Image ) ;							// YUVサーフェスを使った Theora 動画の内容をグラフィックスハンドルのテクスチャに転送する
extern	int		Graphics_Hardware_GraphLock_PF( IMAGEDATA *Image, COLORDATA **ColorDataP, int WriteOnly ) ;								// グラフィックメモリ領域のロック
extern	int		Graphics_Hardware_GraphUnlock_PF( IMAGEDATA *Image ) ;																	// グラフィックメモリ領域のロック解除
extern	int		Graphics_Hardware_CopyGraphZBufferImage_PF( IMAGEDATA *DestImage, IMAGEDATA *SrcImage ) ;								// グラフィックのＺバッファの状態を別のグラフィックのＺバッファにコピーする( DestGrHandle も SrcGrHandle もＺバッファを持っている描画可能画像で、且つアンチエイリアス画像ではないことが条件 )
extern	int		Graphics_Hardware_InitGraph_PF( void ) ;																				// 画像データの初期化
extern	int		Graphics_Hardware_FillGraph_PF( IMAGEDATA *Image, int Red, int Green, int Blue, int Alpha, int ASyncThread ) ;			// グラフィックを特定の色で塗りつぶす
extern	int		Graphics_Hardware_GetDrawScreenGraphBase_PF( IMAGEDATA *Image, IMAGEDATA *TargetImage, int TargetScreen, int TargetScreenSurface, int TargetScreenMipLevel, int TargetScreenWidth, int TargetScreenHeight, int x1, int y1, int x2, int y2, int destX, int destY ) ;		// 描画可能画像やバックバッファから指定領域のグラフィックを取得する

// Graphics_Image_BltBmpOrBaseImageToGraph3 の機種依存部分用関数
extern	int		Graphics_Hardware_BltBmpOrBaseImageToGraph3_PF(
	const RECT		*SrcRect,
	      int		DestX,
	      int		DestY,
	      int		GrHandle,
	const BASEIMAGE	*RgbBaseImage,
	const BASEIMAGE	*AlphaBaseImage,
	      int		RedIsAlphaFlag,
	      int		UseTransColorConvAlpha,
	      int		TargetOrig,
	      int		ASyncThread
) ;

// 基本イメージのフォーマットを DX_BASEIMAGE_FORMAT_NORMAL に変換する必要があるかどうかをチェックする
// ( RequiredRgbBaseImageConvFlag と RequiredAlphaBaseImageConvFlag に入る値  TRUE:変換する必要がある  FALSE:変換する必要は無い )
extern	int		Graphics_CheckRequiredNormalImageConv_BaseImageFormat_PF(
	IMAGEDATA_ORIG *Orig,
	int             RgbBaseImageFormat,
	int            *RequiredRgbBaseImageConvFlag,
	int             AlphaBaseImageFormat = -1,
	int            *RequiredAlphaBaseImageConvFlag = NULL
) ;

extern	int		Graphics_Hardware_CreateOrigTexture_PF(  IMAGEDATA_ORIG *Orig, int ASyncThread = FALSE ) ;			// オリジナル画像情報中のテクスチャを作成する( 0:成功  -1:失敗 )
extern	int		Graphics_Hardware_ReleaseOrigTexture_PF( IMAGEDATA_ORIG *Orig ) ;									// オリジナル画像情報中のテクスチャを解放する
extern	int		Graphics_Hardware_GetMultiSampleQuality_PF( int Samples ) ;											// 指定のマルチサンプル数で使用できる最大クオリティ値を取得する
extern	int		Graphics_Hardware_SetUsePlatformTextureFormat_PF( int PlatformTextureFormat ) ;						// 作成するグラフィックハンドルで使用する環境依存のテクスチャフォーマットを指定する













// 環境依存頂点バッファ・インデックスバッファ関係
extern	int		Graphics_Hardware_VertexBuffer_Create_PF(    VERTEXBUFFERHANDLEDATA *VertexBuffer ) ;															// 頂点バッファハンドルの頂点バッファを作成する
extern	int		Graphics_Hardware_VertexBuffer_Terminate_PF( VERTEXBUFFERHANDLEDATA *VertexBuffer ) ;															// 頂点バッファハンドルの後始末
extern	int		Graphics_Hardware_VertexBuffer_SetData_PF(   VERTEXBUFFERHANDLEDATA *VertexBuffer, int SetIndex, const void *VertexData, int VertexNum ) ;		// 頂点バッファに頂点データを転送する
extern	int		Graphics_Hardware_IndexBuffer_Create_PF(     INDEXBUFFERHANDLEDATA *IndexBuffer ) ;																// インデックスバッファハンドルのセットアップを行う
extern	int		Graphics_Hardware_IndexBuffer_Terminate_PF(  INDEXBUFFERHANDLEDATA *IndexBuffer ) ;																// インデックスバッファハンドルの後始末
extern	int		Graphics_Hardware_IndexBuffer_SetData_PF(    INDEXBUFFERHANDLEDATA *IndexBuffer, int SetIndex, const void *IndexData, int IndexNum ) ;			// インデックスバッファにインデックスデータを転送する











// 環境依存ライト関係
extern	int		Graphics_Hardware_Light_SetUse_PF( int Flag ) ;															// ライティングを行うかどうかを設定する
extern	int		Graphics_Hardware_Light_GlobalAmbient_PF( COLOR_F *Color ) ;											// グローバルアンビエントライトカラーを設定する
extern	int		Graphics_Hardware_Light_SetState_PF( int LightNumber, LIGHTPARAM *LightParam ) ;						// ライトパラメータをセット
extern	int		Graphics_Hardware_Light_SetEnable_PF( int LightNumber, int EnableState ) ;								// ライトの有効、無効を変更












// 環境依存シャドウマップ関係
extern	int		Graphics_Hardware_ShadowMap_CreateTexture_PF( SHADOWMAPDATA *ShadowMap, int ASyncThread = FALSE ) ;		// シャドウマップデータに必要なテクスチャを作成する
extern	int		Graphics_Hardware_ShadowMap_ReleaseTexture_PF( SHADOWMAPDATA *ShadowMap ) ;								// シャドウマップデータに必要なテクスチャを解放する
extern	void	Graphics_Hardware_ShadowMap_RefreshVSParam_PF( void ) ;													// 頂点シェーダーに設定するシャドウマップの情報を更新する
extern	void	Graphics_Hardware_ShadowMap_RefreshPSParam_PF( void ) ;													// ピクセルシェーダーに設定するシャドウマップの情報を更新する
extern	int		Graphics_Hardware_ShadowMap_DrawSetup_PF( SHADOWMAPDATA *ShadowMap ) ;									// シャドウマップへの描画の準備を行う
extern	int		Graphics_Hardware_ShadowMap_DrawEnd_PF( SHADOWMAPDATA *ShadowMap ) ;									// シャドウマップへの描画を終了する
extern	int		Graphics_Hardware_ShadowMap_SetUse_PF( int SlotIndex, SHADOWMAPDATA *ShadowMap ) ;						// 描画で使用するシャドウマップを指定する、スロットは０か１かを指定可能　










// 環境依存シェーダー関係
extern	int		Graphics_Hardware_Shader_Create_PF( int ShaderHandle, int IsVertexShader, void *Image, int ImageSize, int ImageAfterFree, int ASyncThread ) ;		// シェーダーハンドルを初期化する
extern	int		Graphics_Hardware_Shader_TerminateHandle_PF( SHADERHANDLEDATA *Shader ) ;																			// シェーダーハンドルの後始末
extern	int		Graphics_Hardware_Shader_GetValidShaderVersion_PF( void ) ;																							// 使用できるシェーダーのバージョンを取得する( 0=使えない  200=シェーダーモデル２．０が使用可能  300=シェーダーモデル３．０が使用可能 )
extern	int		Graphics_Hardware_Shader_GetConstIndex_PF( const wchar_t *ConstantName, SHADERHANDLEDATA *Shader ) ;													// 指定の名前を持つ定数が使用するシェーダー定数の番号を取得する
extern	int		Graphics_Hardware_Shader_GetConstCount_PF( const wchar_t *ConstantName, SHADERHANDLEDATA *Shader ) ;													// 指定の名前を持つ定数が使用するシェーダー定数の数を取得する
extern	const FLOAT4 *Graphics_Hardware_Shader_GetConstDefaultParamF_PF( const wchar_t *ConstantName, SHADERHANDLEDATA *Shader ) ;									// 指定の名前を持つ浮動小数点定数のデフォルトパラメータが格納されているメモリアドレスを取得する
extern	int		Graphics_Hardware_Shader_SetConst_PF(   int TypeIndex, int SetIndex, int ConstantIndex, const void *Param, int ParamNum, int UpdateUseArea ) ;		// シェーダー定数情報を設定する
extern	int		Graphics_Hardware_Shader_ResetConst_PF( int TypeIndex, int SetIndex, int ConstantIndex, int ParamNum ) ;											// 指定領域のシェーダー定数情報をリセットする
#ifndef DX_NON_MODEL
extern	int		Graphics_Hardware_Shader_ModelCode_Init_PF( void ) ;																								// ３Ｄモデル用のシェーダーコードの初期化を行う
#endif // DX_NON_MODEL










// 環境依存シェーダー用定数バッファ関係
extern	int		Graphics_Hardware_ShaderConstantBuffer_Create_PF( int ShaderConstantBufferHandle, int BufferSize, int ASyncThread ) ;												// シェーダー用定数バッファハンドルを初期化する
extern	int		Graphics_Hardware_ShaderConstantBuffer_TerminateHandle_PF( SHADERCONSTANTBUFFERHANDLEDATA *ShaderConstantBuffer ) ;													// シェーダー用定数バッファハンドルの後始末
extern	void *	Graphics_Hardware_ShaderConstantBuffer_GetBuffer_PF( SHADERCONSTANTBUFFERHANDLEDATA *ShaderConstantBuffer ) ;														// シェーダー用定数バッファハンドルの定数バッファのアドレスを取得する
extern	int		Graphics_Hardware_ShaderConstantBuffer_Update_PF( SHADERCONSTANTBUFFERHANDLEDATA *ShaderConstantBuffer ) ;															// シェーダー用定数バッファハンドルの定数バッファへの変更を適用する
extern	int		Graphics_Hardware_ShaderConstantBuffer_Set_PF( SHADERCONSTANTBUFFERHANDLEDATA *ShaderConstantBuffer, int TargetShader /* DX_SHADERTYPE_VERTEX など */, int Slot ) ;	// シェーダー用定数バッファハンドルの定数バッファを指定のシェーダーの指定のスロットにセットする











// 環境依存描画関係
extern	int		Graphics_Hardware_RenderVertex( int ASyncThread = FALSE ) ;																							// 頂点バッファに溜まった頂点データをレンダリングする

extern	int		Graphics_Hardware_DrawBillboard3D_PF(     VECTOR Pos, float cx, float cy, float Size, float Angle,                                    IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, int ReverseXFlag, int ReverseYFlag, int DrawFlag = TRUE, RECT *DrawArea = NULL ) ;	// ハードウエアアクセラレータ使用版 DrawBillboard3D
extern	int		Graphics_Hardware_DrawModiBillboard3D_PF( VECTOR Pos, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag,               int DrawFlag = TRUE, RECT *DrawArea = NULL ) ;	// ハードウエアアクセラレータ使用版 DrawModiBillboard3D
extern	int		Graphics_Hardware_DrawGraph_PF(           int x,  int y, float xf, float yf,                                                          IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, int IntFlag ) ;				// ハードウエアアクセラレータ使用版 DrawGraph
extern	int		Graphics_Hardware_DrawExtendGraph_PF(     int x1, int y1, int x2, int y2, float x1f, float y1f, float x2f, float y2f,                 IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, int IntFlag ) ;				// ハードウエアアクセラレータ使用版 DrawExtendGraph
extern	int		Graphics_Hardware_DrawRotaGraph_PF(       int x,  int y, float xf, float yf, double ExRate, double Angle,                             IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, int ReverseXFlag, int ReverseYFlag, int IntFlag ) ;	// ハードウエアアクセラレータ使用版 DrawRotaGraph
extern	int		Graphics_Hardware_DrawRotaGraphFast_PF(   int x,  int y, float xf, float yf, float  ExRate, float  Angle,                             IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, int ReverseXFlag, int ReverseYFlag, int IntFlag ) ;	// ハードウエアアクセラレータ使用版 DrawRotaGraphFast
extern	int		Graphics_Hardware_DrawModiGraph_PF(       int   x1, int   y1, int   x2, int   y2, int   x3, int   y3, int   x4, int   y4,             IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, bool SimpleDrawFlag ) ;		// ハードウエアアクセラレータ使用版 DrawModiGraph
extern	int		Graphics_Hardware_DrawModiGraphF_PF(      float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4,             IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, bool SimpleDrawFlag ) ;		// ハードウエアアクセラレータ使用版 DrawModiGraphF
extern	int		Graphics_Hardware_DrawSimpleQuadrangleGraphF_PF( const GRAPHICS_DRAW_DRAWSIMPLEQUADRANGLEGRAPHF_PARAM *Param,						  IMAGEDATA *Image, IMAGEDATA *BlendImage ) ;											// ハードウエアアクセラレータ使用版 DrawSimpleQuadrangleGraphF
extern	int		Graphics_Hardware_DrawSimpleTriangleGraphF_PF(   const GRAPHICS_DRAW_DRAWSIMPLETRIANGLEGRAPHF_PARAM *Param,							  IMAGEDATA *Image, IMAGEDATA *BlendImage ) ;											// ハードウエアアクセラレータ使用版 DrawSimpleTriangleGraphF

extern	int		Graphics_Hardware_DrawFillBox_PF(          int x1, int y1, int x2, int y2,                                                 unsigned int Color ) ;																// ハードウエアアクセラレータ使用版 DrawFillBox
extern	int		Graphics_Hardware_DrawLineBox_PF(          int x1, int y1, int x2, int y2,                                                 unsigned int Color ) ;																// ハードウエアアクセラレータ使用版 DrawLineBox
extern	int		Graphics_Hardware_DrawLine_PF(             int x1, int y1, int x2, int y2,                                                 unsigned int Color ) ;																// ハードウエアアクセラレータ使用版 DrawLine
extern	int		Graphics_Hardware_DrawLine3D_PF(           VECTOR Pos1, VECTOR Pos2,                                                       unsigned int Color, int DrawFlag = TRUE, RECT *DrawArea = NULL ) ;					// ハードウエアアクセラレータ使用版 DrawLine3D
extern	int		Graphics_Hardware_DrawCircle_Thickness_PF( int x, int y, int r,                                                            unsigned int Color, int Thickness ) ;												// ハードウエアアクセラレータ使用版 DrawCircle( 太さ指定あり )
extern	int		Graphics_Hardware_DrawOval_Thickness_PF(   int x, int y, int rx, int ry,                                                   unsigned int Color, int Thickness ) ;												// ハードウエアアクセラレータ使用版 DrawOval( 太さ指定あり )
extern	int		Graphics_Hardware_DrawCircle_PF(           int x, int y, int r,                                                            unsigned int Color, int FillFlag, int Rx_One_Minus = FALSE, int Ry_One_Minus = FALSE ) ;		// ハードウエアアクセラレータ使用版 DrawCircle
extern	int		Graphics_Hardware_DrawOval_PF(             int x, int y, int rx, int ry,                                                   unsigned int Color, int FillFlag, int Rx_One_Minus = FALSE, int Ry_One_Minus = FALSE ) ;		// ハードウエアアクセラレータ使用版 DrawOval
extern	int		Graphics_Hardware_DrawTriangle_PF(         int x1, int y1, int x2, int y2, int x3, int y3,                                 unsigned int Color, int FillFlag ) ;												// ハードウエアアクセラレータ使用版 DrawTriangle
extern	int		Graphics_Hardware_DrawTriangle3D_PF(       VECTOR Pos1, VECTOR Pos2, VECTOR Pos3,                                          unsigned int Color, int FillFlag, int DrawFlag = TRUE, RECT *DrawArea = NULL ) ;	// ハードウエアアクセラレータ使用版 DrawTriangle3D
extern	int		Graphics_Hardware_DrawQuadrangle_PF(       int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4,                 unsigned int Color, int FillFlag ) ;												// ハードウエアアクセラレータ使用版 DrawQuadrangle
extern	int		Graphics_Hardware_DrawQuadrangleF_PF(      float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int Color, int FillFlag ) ;												// ハードウエアアクセラレータ使用版 DrawQuadrangle
extern	int		Graphics_Hardware_DrawPixel_PF(            int x,  int y,                                                                  unsigned int Color ) ;																// ハードウエアアクセラレータ使用版 DrawPixel
extern	int		Graphics_Hardware_DrawPixel3D_PF(          VECTOR Pos,                                                                     unsigned int Color, int DrawFlag = TRUE, RECT *DrawArea = NULL ) ;					// ハードウエアアクセラレータ使用版 DrawPixel3D
extern	int		Graphics_Hardware_DrawPixelSet_PF(         const POINTDATA *PointData, int Num ) ;																				// ハードウエアアクセラレータ使用版 DrawPixelSet
extern	int		Graphics_Hardware_DrawLineSet_PF(          const LINEDATA  *LineData,  int Num ) ;																				// ハードウエアアクセラレータ使用版 DrawLineSet

extern	int		Graphics_Hardware_DrawPrimitive_PF(                             const VERTEX_3D *Vertex, int VertexNum,                                    int PrimitiveType, IMAGEDATA *Image, int TransFlag ) ;
extern	int		Graphics_Hardware_DrawIndexedPrimitive_PF(                      const VERTEX_3D *Vertex, int VertexNum, const WORD *Indices, int IndexNum, int PrimitiveType, IMAGEDATA *Image, int TransFlag ) ;
extern	int		Graphics_Hardware_DrawPrimitiveLight_PF(                        const VERTEX3D  *Vertex, int VertexNum,                                    int PrimitiveType, IMAGEDATA *Image, int TransFlag ) ;
extern	int		Graphics_Hardware_DrawIndexedPrimitiveLight_PF(                 const VERTEX3D  *Vertex, int VertexNum, const WORD *Indices, int IndexNum, int PrimitiveType, IMAGEDATA *Image, int TransFlag ) ;
extern	int		Graphics_Hardware_DrawPrimitiveLight_UseVertexBuffer_PF(        VERTEXBUFFERHANDLEDATA *VertexBuffer,                                      int PrimitiveType,                 int StartVertex, int UseVertexNum, IMAGEDATA *Image, int TransFlag ) ;
extern	int		Graphics_Hardware_DrawIndexedPrimitiveLight_UseVertexBuffer_PF( VERTEXBUFFERHANDLEDATA *VertexBuffer, INDEXBUFFERHANDLEDATA *IndexBuffer,  int PrimitiveType, int BaseVertex, int StartVertex, int UseVertexNum, int StartIndex, int UseIndexNum, IMAGEDATA *Image, int TransFlag ) ;
extern	int		Graphics_Hardware_DrawPrimitive2D_PF(                                 VERTEX_2D *Vertex, int VertexNum,                                    int PrimitiveType, IMAGEDATA *Image, int TransFlag, int BillboardFlag, int Is3D, int ReverseXFlag, int ReverseYFlag, int TextureNo, int IsShadowMap ) ;
extern	int		Graphics_Hardware_DrawPrimitive2DUser_PF(                       const VERTEX2D  *Vertex, int VertexNum,                                    int PrimitiveType, IMAGEDATA *Image, int TransFlag, int Is3D, int ReverseXFlag, int ReverseYFlag, int TextureNo ) ;
extern	int		Graphics_Hardware_DrawIndexedPrimitive2DUser_PF(                const VERTEX2D  *Vertex, int VertexNum, const WORD *Indices, int IndexNum, int PrimitiveType, IMAGEDATA *Image, int TransFlag ) ;

extern	int		Graphics_Hardware_DrawPolygon3DToShader_PF(          const VERTEX3DSHADER *Vertex, int PolygonNum ) ;																										// シェーダーを使って３Ｄポリゴンを描画する
extern	int		Graphics_Hardware_DrawPolygonIndexed3DToShader_PF(   const VERTEX3DSHADER *Vertex, int VertexNum, const unsigned short *Indices, int PolygonNum ) ;														// シェーダーを使って３Ｄポリゴンを描画する( 頂点インデックスを使用する )
extern	int		Graphics_Hardware_DrawPrimitive2DToShader_PF(        const VERTEX2DSHADER *Vertex, int VertexNum,                                              int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ ) ;		// シェーダーを使って２Ｄプリミティブを描画する
extern	int		Graphics_Hardware_DrawPrimitive3DToShader_PF(        const VERTEX3DSHADER *Vertex, int VertexNum,                                              int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ ) ;		// シェーダーを使って３Ｄプリミティブを描画する
extern	int		Graphics_Hardware_DrawPrimitiveIndexed2DToShader_PF( const VERTEX2DSHADER *Vertex, int VertexNum, const unsigned short *Indices, int IndexNum, int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ ) ;		// シェーダーを使って２Ｄプリミティブを描画する( 頂点インデックスを使用する )
extern	int		Graphics_Hardware_DrawPrimitiveIndexed3DToShader_PF( const VERTEX3DSHADER *Vertex, int VertexNum, const unsigned short *Indices, int IndexNum, int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ ) ;		// シェーダーを使って３Ｄプリミティブを描画する( 頂点インデックスを使用する )

extern	int		Graphics_Hardware_DrawPrimitive3DToShader_UseVertexBuffer2_PF(        int VertexBufHandle,                     int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */, int StartVertex, int UseVertexNum ) ;	// シェーダーを使って３Ｄプリミティブを描画する( 頂点バッファ使用版 )
extern	int		Graphics_Hardware_DrawPrimitiveIndexed3DToShader_UseVertexBuffer2_PF( int VertexBufHandle, int IndexBufHandle, int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */, int BaseVertex, int StartVertex, int UseVertexNum, int StartIndex, int UseIndexNum ) ;	// シェーダーを使って３Ｄプリミティブを描画する( 頂点バッファとインデックスバッファ使用版 )

extern	int		Graphics_Hardware_Paint_PF( int x, int y, unsigned int FillColor, ULONGLONG BoundaryColor ) ;			// 指定点から境界色があるところまで塗りつぶす











// wchar_t版関数
extern	int			LoadBmpToGraph_WCHAR_T(			const wchar_t *FileName, int TextureFlag, int ReverseFlag, int SurfaceMode = DX_MOVIESURFACE_NORMAL ) ;
extern	int			LoadGraph_WCHAR_T(				const wchar_t *FileName, int NotUse3DFlag = FALSE ) ;
extern	int			LoadReverseGraph_WCHAR_T(		const wchar_t *FileName, int NotUse3DFlag = FALSE ) ;
extern	int			LoadDivBmpToGraph_WCHAR_T(		const wchar_t *FileName, int AllNum, int XNum, int YNum, int IsFloat, int SizeXI, float SizeXF, int SizeYI, float SizeYF, int *HandleArray, int TextureFlag, int ReverseFlag ) ;
extern	int			LoadBlendGraph_WCHAR_T(			const wchar_t *FileName ) ;
#ifdef __WINDOWS__
extern	int			LoadGraphToResource_WCHAR_T(	const wchar_t *ResourceName, const wchar_t *ResourceType ) ;
extern	int			LoadDivGraphToResource_WCHAR_T(	const wchar_t *ResourceName, const wchar_t *ResourceType, int AllNum, int XNum, int YNum, int XSize, int YSize, int *HandleArray ) ;
extern	int			LoadDivGraphFToResource_WCHAR_T( const wchar_t *ResourceName, const wchar_t *ResourceType, int AllNum, int XNum, int YNum, float XSize, float YSize, int *HandleArray ) ;
#endif // __WINDOWS__
extern	int			ReloadGraph_WCHAR_T(			const wchar_t *FileName, int GrHandle, int ReverseFlag = FALSE ) ;
extern	int			ReloadDivGraph_WCHAR_T(			const wchar_t *FileName, int AllNum, int XNum, int YNum, int IsFloat, int XSizeI, float XSizeF, int YSizeI, float YSizeF, const int *HandleArray, int ReverseFlag = FALSE ) ;
extern	int			ReloadReverseGraph_WCHAR_T(		const wchar_t *FileName, int GrHandle ) ;

extern	int			GetGraphFilePath_WCHAR_T(		int GrHandle, wchar_t *FilePathBuffer ) ;

extern	int			LoadGraphScreen_WCHAR_T(        int x, int y, const wchar_t *GraphName, int TransFlag ) ;

extern	int			SaveDrawScreen_WCHAR_T(			int x1, int y1, int x2, int y2, const wchar_t *FileName, int SaveType = DX_IMAGESAVETYPE_BMP , int Jpeg_Quality = 80 , int Jpeg_Sample2x1 = TRUE , int Png_CompressionLevel = -1 ) ;
extern	int			SaveDrawScreenToBMP_WCHAR_T(	int x1, int y1, int x2, int y2, const wchar_t *FileName ) ;
extern	int			SaveDrawScreenToDDS_WCHAR_T(	int x1, int y1, int x2, int y2, const wchar_t *FileName ) ;
extern	int			SaveDrawScreenToJPEG_WCHAR_T(	int x1, int y1, int x2, int y2, const wchar_t *FileName, int Quality = 80 , int Sample2x1 = TRUE ) ;
extern	int			SaveDrawScreenToPNG_WCHAR_T(	int x1, int y1, int x2, int y2, const wchar_t *FileName, int CompressionLevel = -1 ) ;

extern	int			SaveDrawValidGraph_WCHAR_T(       int GrHandle, int x1, int y1, int x2, int y2, const wchar_t *FileName, int SaveType = DX_IMAGESAVETYPE_BMP , int Jpeg_Quality = 80 , int Jpeg_Sample2x1 = TRUE , int Png_CompressionLevel = -1 ) ;
extern	int			SaveDrawValidGraphToBMP_WCHAR_T(  int GrHandle, int x1, int y1, int x2, int y2, const wchar_t *FileName ) ;
extern	int			SaveDrawValidGraphToDDS_WCHAR_T(  int GrHandle, int x1, int y1, int x2, int y2, const wchar_t *FileName ) ;
extern	int			SaveDrawValidGraphToJPEG_WCHAR_T( int GrHandle, int x1, int y1, int x2, int y2, const wchar_t *FileName, int Quality = 80 , int Sample2x1 = TRUE ) ;
extern	int			SaveDrawValidGraphToPNG_WCHAR_T(  int GrHandle, int x1, int y1, int x2, int y2, const wchar_t *FileName, int CompressionLevel = -1 ) ;

extern	int			LoadVertexShader_WCHAR_T(		const wchar_t *FileName ) ;
extern	int			LoadPixelShader_WCHAR_T(		const wchar_t *FileName ) ;

extern	int			GetConstIndexToShader_WCHAR_T(           const wchar_t *ConstantName, int ShaderHandle ) ;
extern	int			GetConstCountToShader_WCHAR_T(           const wchar_t *ConstantName, int ShaderHandle ) ;
extern	const FLOAT4 *GetConstDefaultParamFToShader_WCHAR_T( const wchar_t *ConstantName, int ShaderHandle ) ;

extern	int			PlayMovie_WCHAR_T(						const wchar_t *FileName, int ExRate, int PlayType ) ;
extern	int			GetMovieImageSize_File_WCHAR_T(         const wchar_t *FileName, int *SizeX, int *SizeY ) ;
extern	int			OpenMovieToGraph_WCHAR_T(				const wchar_t *FileName, int FullColor = TRUE ) ;




// インライン関数・マクロ---------------------------------------------------------

// SetDrawBright の引数が一つ版
#define Graphics_DrawSetting_SetDrawBrightToOneParam( /* DWORD */ Bright )\
{\
	DWORD lTempBright = ( DWORD )( Bright ) & 0xffffff ;\
	if( GSYS.DrawSetting.bDrawBright != lTempBright )\
	{\
		/* 輝度を保存 */\
		GSYS.DrawSetting.bDrawBright = lTempBright ;\
\
		/* ハードウエアアクセラレーションの設定に反映*/\
		if( GSYS.Setting.ValidHardware )\
		{\
			Graphics_Hardware_SetDrawBrightToOneParam_PF( lTempBright ) ;\
		}\
		else\
		{\
			SetMemImgDrawBright( lTempBright ) ;\
		}\
	}\
}



#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_NON_GRAPHICS

#endif // __DXGRAPHICS_H__


