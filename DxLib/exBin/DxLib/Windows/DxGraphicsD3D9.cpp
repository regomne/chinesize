//-----------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		描画処理プログラム( Direct3D9 )
// 
//  	Ver 3.20c
// 
//-----------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

#include "../DxCompileConfig.h"

#ifndef DX_NON_GRAPHICS

#ifndef DX_NON_DIRECT3D9

// インクルード ---------------------------------------------------------------
#include "DxGraphicsD3D9.h"
#include "DxGraphicsWin.h"
#include "DxMaskD3D9.h"
#include "DxWindow.h"
#include "DxWinAPI.h"
#include "DxShader_PS_D3D9.h"
#include "DxShader_VS_D3D9.h"
#include "../DxSystem.h"
#include "../DxLog.h"
#include "../DxModel.h"
#include "../DxMovie.h"
#include "../DxMask.h"
#include "../DxMath.h"
#include "../DxBaseFunc.h"
#include "../DxGraphics.h"
#include "../DxGraphicsFilter.h"
#include "../DxUseCLib.h"
#include "../DxInput.h"
#include "../DxThread.h"
#include "../DxASyncLoad.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 -----------------------------------------------------------------

// 非管理テクスチャへのデータ転送用システムメモリ配置テクスチャの同じフォーマットの最大数
#define DIRECT3D9_SYSMEMTEXTURE_ONE_FORMAT_NUM	(16)

// 非管理テクスチャへのデータ転送用システムメモリ配置サーフェスの同じフォーマットの最大数
#define DIRECT3D9_SYSMEMSURFACE_ONE_FORMAT_NUM	(16)




#define DX_D3D9_DRAWPREP_NOTEX( FLAG )													\
	if( GD3D9.Device.DrawSetting.RenderTexture != NULL )								\
		Graphics_D3D9_DrawSetting_SetTexture( NULL ) ;									\
	if( GSYS.ChangeSettingFlag ||														\
		GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag != FALSE ||							\
		GD3D9.Device.DrawSetting.DrawPrepParamFlag  != FLAG )							\
		Graphics_D3D9_DrawPreparation( FLAG ) ;											\
	/*if( GRH.UseAlphaChDrawShader )*/													\
	/*{*/																				\
	/*	AlphaChDraw_Pre( &GD3D9.Device.DrawInfo.DrawRect ) ;*/							\
	/*}*/


// Graphics_D3D9_DrawPreparation を呼ぶ定型文
#define DX_D3D9_DRAWPREP_TEX( ORIG, TEX, FLAG )											\
	FLAG |= DX_D3D9_DRAWPREP_TEXTURE ;													\
	if( (ORIG)->FormatDesc.AlphaChFlag   ) FLAG |= DX_D3D9_DRAWPREP_TEXALPHACH ;		\
	if( (ORIG)->FormatDesc.AlphaTestFlag ) FLAG |= DX_D3D9_DRAWPREP_TEXALPHATEST ;		\
	if( GD3D9.Device.DrawSetting.RenderTexture != (TEX) )								\
		Graphics_D3D9_DrawSetting_SetTexture( (TEX) ) ;									\
	if( GSYS.ChangeSettingFlag ||														\
		GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag != FALSE ||							\
		GD3D9.Device.DrawSetting.DrawPrepParamFlag  != FLAG )							\
		Graphics_D3D9_DrawPreparation( FLAG ) ;											\
	/*if( GRH.UseAlphaChDrawShader )*/													\
	/*{*/																				\
	/*	AlphaChDraw_Pre( &GD3D9.Device.DrawInfo.DrawRect ) ;*/							\
	/*}*/



#define RENDERVERTEX( NextUse3DVertex )	\
	{\
		if( GD3D9.Device.DrawInfo.VertexNum != 0 )\
		{\
			/*SETFVF(                     D3DDev_VertexType[ GD3D9.Device.DrawInfo.Use3DVertex ][ GD3D9.Device.DrawInfo.VertexType ] )*/\
			/*Graphics_D3D9_DeviceState_SetFVF( D3DDev_VertexType[ GD3D9.Device.DrawInfo.Use3DVertex ][ GD3D9.Device.DrawInfo.VertexType ] ) ;*/\
			Graphics_Hardware_D3D9_SetFVF_UseTable( D3DDev_VertexDeclaration[ GD3D9.Device.DrawInfo.Use3DVertex ][ GD3D9.Device.DrawInfo.VertexType ] ) ;\
			/*if( GD3D9.Device.DrawInfo.Use3DVertex )*/\
			/*{*/\
				/*Graphics_D3D9_DeviceState_SetWorldMatrix( &GlobalIdentMatrix ) ;*/\
			/*}*/\
			Direct3DDevice9_DrawPrimitiveUP(\
				GD3D9.Device.DrawInfo.PrimitiveType,\
				( UINT )GETPRIMNUM( GD3D9.Device.DrawInfo.PrimitiveType, GD3D9.Device.DrawInfo.VertexNum ),\
				GD3D9.Device.DrawInfo.VertexBufferAddr[ GD3D9.Device.DrawInfo.Use3DVertex ][ GD3D9.Device.DrawInfo.VertexType ],\
				D3DDev_VertexSize[ GD3D9.Device.DrawInfo.Use3DVertex ][ GD3D9.Device.DrawInfo.VertexType ] ) ;\
			/*if( GD3D9.Device.DrawInfo.Use3DVertex )*/\
			/*{*/\
				/*Graphics_D3D9_DeviceState_SetWorldMatrix( &GSYS.DrawSetting.WorldMatrixF ) ;*/\
			/*}*/\
			/*RenderVertexDrawPrimitive_PF() ;*/\
			GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;\
		}\
		GD3D9.Device.DrawInfo.VertexNum = 0 ;\
		GD3D9.Device.DrawInfo.VertexBufferNextAddr = GD3D9.Device.DrawInfo.VertexBufferAddr[ NextUse3DVertex ][ GD3D9.Device.DrawInfo.VertexType ] ;\
	}

#define GETVERTEX_QUAD( p )		\
	MV1DRAWPACKDRAWMODEL		\
	if(\
		  GD3D9.Device.DrawInfo.Use3DVertex != 0 ||\
		  GD3D9.Device.DrawInfo.PrimitiveType != D_D3DPT_TRIANGLELIST ||\
		  GD3D9.Device.DrawInfo.VertexNum + 6 >= D3DDev_VertexBuffer_MaxVertexNum[ GD3D9.Device.DrawInfo.Use3DVertex ][ GD3D9.Device.DrawInfo.VertexType ] /* ←本来は > だけど、GeForce 9800 GT の不具合対応のため >= にした */\
	  )\
	{\
		RENDERVERTEX( 0 )\
		GD3D9.Device.DrawInfo.Use3DVertex = 0 ;\
		GD3D9.Device.DrawInfo.PrimitiveType = D_D3DPT_TRIANGLELIST ;\
	}\
	*((DWORD_PTR *)&p) = (DWORD_PTR)GD3D9.Device.DrawInfo.VertexBufferNextAddr ;

#define GETVERTEX_BILLBOARD( p )		\
	MV1DRAWPACKDRAWMODEL		\
	if(\
		  GD3D9.Device.DrawInfo.Use3DVertex != 1 ||\
		  GD3D9.Device.DrawInfo.PrimitiveType != D_D3DPT_TRIANGLELIST ||\
		  GD3D9.Device.DrawInfo.VertexNum + 6 >= D3DDev_VertexBuffer_MaxVertexNum[ GD3D9.Device.DrawInfo.Use3DVertex ][ GD3D9.Device.DrawInfo.VertexType ] /* ←本来は > だけど、GeForce 9800 GT の不具合対応のため >= にした */\
	  )\
	{\
		RENDERVERTEX( 1 )\
		GD3D9.Device.DrawInfo.Use3DVertex = 1 ;\
		GD3D9.Device.DrawInfo.PrimitiveType = D_D3DPT_TRIANGLELIST ;\
	}\
	*((DWORD_PTR *)&p) = (DWORD_PTR)GD3D9.Device.DrawInfo.VertexBufferNextAddr ;

#define GETVERTEX_TRIANGLE( p )		\
	MV1DRAWPACKDRAWMODEL		\
	if(\
		  GD3D9.Device.DrawInfo.Use3DVertex != 0 ||\
		  GD3D9.Device.DrawInfo.PrimitiveType != D_D3DPT_TRIANGLELIST ||\
		  GD3D9.Device.DrawInfo.VertexNum + 3 >= D3DDev_VertexBuffer_MaxVertexNum[ GD3D9.Device.DrawInfo.Use3DVertex ][ GD3D9.Device.DrawInfo.VertexType ] /* ←本来は > だけど、GeForce 9800 GT の不具合対応のため >= にした */\
	  )\
	{\
		RENDERVERTEX( 0 )\
		GD3D9.Device.DrawInfo.Use3DVertex = 0 ;\
		GD3D9.Device.DrawInfo.PrimitiveType = D_D3DPT_TRIANGLELIST ;\
	}\
	*((DWORD_PTR *)&p) = (DWORD_PTR)GD3D9.Device.DrawInfo.VertexBufferNextAddr ;

#define GETVERTEX_TRIANGLE3D( p )		\
	MV1DRAWPACKDRAWMODEL		\
	if(\
		  GD3D9.Device.DrawInfo.Use3DVertex != 1 ||\
		  GD3D9.Device.DrawInfo.PrimitiveType != D_D3DPT_TRIANGLELIST ||\
		  GD3D9.Device.DrawInfo.VertexNum + 3 >= D3DDev_VertexBuffer_MaxVertexNum[ GD3D9.Device.DrawInfo.Use3DVertex ][ GD3D9.Device.DrawInfo.VertexType ] /* ←本来は > だけど、GeForce 9800 GT の不具合対応のため >= にした */\
	  )\
	{\
		RENDERVERTEX( 1 )\
		GD3D9.Device.DrawInfo.Use3DVertex = 1 ;\
		GD3D9.Device.DrawInfo.PrimitiveType = D_D3DPT_TRIANGLELIST ;\
	}\
	*((DWORD_PTR *)&p) = (DWORD_PTR)GD3D9.Device.DrawInfo.VertexBufferNextAddr ;

#define GETVERTEX_LINEBOX( p )		\
	MV1DRAWPACKDRAWMODEL		\
	if(\
		  GD3D9.Device.DrawInfo.Use3DVertex != 0 ||\
		  GD3D9.Device.DrawInfo.PrimitiveType != D_D3DPT_TRIANGLELIST ||\
		  GD3D9.Device.DrawInfo.VertexNum + 24 >= D3DDev_VertexBuffer_MaxVertexNum[ GD3D9.Device.DrawInfo.Use3DVertex ][ GD3D9.Device.DrawInfo.VertexType ] /* ←本来は > だけど、GeForce 9800 GT の不具合対応のため >= にした */\
	  )\
	{\
		RENDERVERTEX( 0 )\
		GD3D9.Device.DrawInfo.Use3DVertex = 0 ;\
		GD3D9.Device.DrawInfo.PrimitiveType = D_D3DPT_TRIANGLELIST ;\
	}\
	*((DWORD_PTR *)&p) = (DWORD_PTR)GD3D9.Device.DrawInfo.VertexBufferNextAddr ;

#define GETVERTEX_LINETRIANGLE( p )		\
	MV1DRAWPACKDRAWMODEL		\
	if(\
		  GD3D9.Device.DrawInfo.Use3DVertex != 0 ||\
		  GD3D9.Device.DrawInfo.PrimitiveType != D_D3DPT_LINELIST ||\
		  GD3D9.Device.DrawInfo.VertexNum + 6 >= D3DDev_VertexBuffer_MaxVertexNum[ GD3D9.Device.DrawInfo.Use3DVertex ][ GD3D9.Device.DrawInfo.VertexType ] /* ←本来は > だけど、GeForce 9800 GT の不具合対応のため >= にした */\
	  )\
	{\
		RENDERVERTEX( 0 )\
		GD3D9.Device.DrawInfo.Use3DVertex = 0 ;\
		GD3D9.Device.DrawInfo.PrimitiveType = D_D3DPT_LINELIST ;\
	}\
	*((DWORD_PTR *)&p) = (DWORD_PTR)GD3D9.Device.DrawInfo.VertexBufferNextAddr ;

#define GETVERTEX_LINETRIANGLE3D( p )		\
	MV1DRAWPACKDRAWMODEL		\
	if(\
		  GD3D9.Device.DrawInfo.Use3DVertex != 1 ||\
		  GD3D9.Device.DrawInfo.PrimitiveType != D_D3DPT_LINELIST ||\
		  GD3D9.Device.DrawInfo.VertexNum + 6 >= D3DDev_VertexBuffer_MaxVertexNum[ GD3D9.Device.DrawInfo.Use3DVertex ][ GD3D9.Device.DrawInfo.VertexType ] /* ←本来は > だけど、GeForce 9800 GT の不具合対応のため >= にした */\
	  )\
	{\
		RENDERVERTEX( 1 )\
		GD3D9.Device.DrawInfo.Use3DVertex = 1 ;\
		GD3D9.Device.DrawInfo.PrimitiveType = D_D3DPT_LINELIST ;\
	}\
	*((DWORD_PTR *)&p) = (DWORD_PTR)GD3D9.Device.DrawInfo.VertexBufferNextAddr ;

#define GETVERTEX_LINE( p )		\
	MV1DRAWPACKDRAWMODEL		\
	if(\
		  GD3D9.Device.DrawInfo.Use3DVertex == 1 ||\
		  GD3D9.Device.DrawInfo.PrimitiveType != D_D3DPT_LINELIST ||\
		  GD3D9.Device.DrawInfo.VertexNum + 2 >= D3DDev_VertexBuffer_MaxVertexNum[ GD3D9.Device.DrawInfo.Use3DVertex ][ GD3D9.Device.DrawInfo.VertexType ] /* ←本来は > だけど、GeForce 9800 GT の不具合対応のため >= にした */\
	  )\
	{\
		RENDERVERTEX( 0 )\
		GD3D9.Device.DrawInfo.Use3DVertex = 0 ;\
		GD3D9.Device.DrawInfo.PrimitiveType = D_D3DPT_LINELIST ;\
	}\
	*((DWORD_PTR *)&p) = (DWORD_PTR)GD3D9.Device.DrawInfo.VertexBufferNextAddr ;


#define GETVERTEX_LINE3D( p )		\
	MV1DRAWPACKDRAWMODEL		\
	if(\
		  GD3D9.Device.DrawInfo.Use3DVertex != 1 ||\
		  GD3D9.Device.DrawInfo.PrimitiveType != D_D3DPT_LINELIST ||\
		  GD3D9.Device.DrawInfo.VertexNum + 2 >= D3DDev_VertexBuffer_MaxVertexNum[ GD3D9.Device.DrawInfo.Use3DVertex ][ GD3D9.Device.DrawInfo.VertexType ] /* ←本来は > だけど、GeForce 9800 GT の不具合対応のため >= にした */\
	  )\
	{\
		RENDERVERTEX( 1 )\
		GD3D9.Device.DrawInfo.Use3DVertex = 1 ;\
		GD3D9.Device.DrawInfo.PrimitiveType = D_D3DPT_LINELIST ;\
	}\
	*((DWORD_PTR *)&p) = (DWORD_PTR)GD3D9.Device.DrawInfo.VertexBufferNextAddr ;


#define GETVERTEX_POINT( p )		\
	MV1DRAWPACKDRAWMODEL		\
	if(\
		  GD3D9.Device.DrawInfo.Use3DVertex != 0 ||\
		  GD3D9.Device.DrawInfo.PrimitiveType != D_D3DPT_POINTLIST ||\
		  GD3D9.Device.DrawInfo.VertexNum + 1 >= D3DDev_VertexBuffer_MaxVertexNum[ GD3D9.Device.DrawInfo.Use3DVertex ][ GD3D9.Device.DrawInfo.VertexType ] /* ←本来は > だけど、GeForce 9800 GT の不具合対応のため >= にした */\
	  )\
	{\
		RENDERVERTEX( 0 )\
		GD3D9.Device.DrawInfo.Use3DVertex = 0 ;\
		GD3D9.Device.DrawInfo.PrimitiveType = D_D3DPT_POINTLIST ;\
	}\
	*((DWORD_PTR *)&p) = (DWORD_PTR)GD3D9.Device.DrawInfo.VertexBufferNextAddr ;

#define GETVERTEX_POINT3D( p )		\
	MV1DRAWPACKDRAWMODEL		\
	if(\
		  GD3D9.Device.DrawInfo.Use3DVertex != 1 ||\
		  GD3D9.Device.DrawInfo.PrimitiveType != D_D3DPT_POINTLIST ||\
		  GD3D9.Device.DrawInfo.VertexNum + 1 >= D3DDev_VertexBuffer_MaxVertexNum[ GD3D9.Device.DrawInfo.Use3DVertex ][ GD3D9.Device.DrawInfo.VertexType ] /* ←本来は > だけど、GeForce 9800 GT の不具合対応のため >= にした */\
	  )\
	{\
		RENDERVERTEX( 1 )\
		GD3D9.Device.DrawInfo.Use3DVertex = 1 ;\
		GD3D9.Device.DrawInfo.PrimitiveType = D_D3DPT_POINTLIST ;\
	}\
	*((DWORD_PTR *)&p) = (DWORD_PTR)GD3D9.Device.DrawInfo.VertexBufferNextAddr ;

#define ADD4VERTEX_POINT		\
		GD3D9.Device.DrawInfo.VertexBufferNextAddr += sizeof( VERTEX_NOTEX_2D ) ;	\
		GD3D9.Device.DrawInfo.VertexNum ++ ;

#define ADD4VERTEX_POINT3D		\
		GD3D9.Device.DrawInfo.VertexBufferNextAddr += sizeof( VERTEX_NOTEX_3D ) ;	\
		GD3D9.Device.DrawInfo.VertexNum ++ ;
/*
#define ADD4VERTEX_LINEBOX			\
		GD3D9.Device.DrawInfo.VertexBufferNextAddr += sizeof( VERTEX_NOTEX_2D ) * 8 ;	\
		GD3D9.Device.DrawInfo.VertexNum += 8 ;
*/
#define ADD4VERTEX_LINEBOX			\
		GD3D9.Device.DrawInfo.VertexBufferNextAddr += sizeof( VERTEX_NOTEX_2D ) * 24 ;	\
		GD3D9.Device.DrawInfo.VertexNum += 24 ;

#define ADD4VERTEX_LINETRIANGLE		\
		GD3D9.Device.DrawInfo.VertexBufferNextAddr += sizeof( VERTEX_NOTEX_2D ) * 6 ;	\
		GD3D9.Device.DrawInfo.VertexNum += 6 ;

#define ADD4VERTEX_LINETRIANGLE3D		\
		GD3D9.Device.DrawInfo.VertexBufferNextAddr += sizeof( VERTEX_NOTEX_3D ) * 6 ;	\
		GD3D9.Device.DrawInfo.VertexNum += 6 ;

#define ADD4VERTEX_LINE			\
		GD3D9.Device.DrawInfo.VertexBufferNextAddr += sizeof( VERTEX_NOTEX_2D ) * 2 ;	\
		GD3D9.Device.DrawInfo.VertexNum += 2 ;

#define ADD4VERTEX_LINE3D		\
		GD3D9.Device.DrawInfo.VertexBufferNextAddr += sizeof( VERTEX_NOTEX_3D ) * 2 ;	\
		GD3D9.Device.DrawInfo.VertexNum += 2 ;

#define ADD4VERTEX_NOTEX		\
		GD3D9.Device.DrawInfo.VertexBufferNextAddr += sizeof( VERTEX_NOTEX_2D ) * 6 ;	\
		GD3D9.Device.DrawInfo.VertexNum += 6 ;

#define ADD4VERTEX_TEX			\
		GD3D9.Device.DrawInfo.VertexBufferNextAddr += sizeof( VERTEX_2D ) * 6 ;	\
		GD3D9.Device.DrawInfo.VertexNum += 6 ;

#define ADD4VERTEX_BLENDTEX		\
		GD3D9.Device.DrawInfo.VertexBufferNextAddr += sizeof( VERTEX_BLENDTEX_2D ) * 6 ;	\
		GD3D9.Device.DrawInfo.VertexNum += 6 ;

#define ADD3VERTEX_NOTEX		\
		GD3D9.Device.DrawInfo.VertexBufferNextAddr += sizeof( VERTEX_NOTEX_2D ) * 3 ;	\
		GD3D9.Device.DrawInfo.VertexNum += 3 ;

#define ADD3VERTEX_TEX			\
		GD3D9.Device.DrawInfo.VertexBufferNextAddr += sizeof( VERTEX_2D ) * 3 ;	\
		GD3D9.Device.DrawInfo.VertexNum += 3 ;

#define ADD3VERTEX_BLENDTEX		\
		GD3D9.Device.DrawInfo.VertexBufferNextAddr += sizeof( VERTEX_BLENDTEX_2D ) * 3 ;	\
		GD3D9.Device.DrawInfo.VertexNum += 3 ;

#define ADD3VERTEX_NOTEX3D		\
		GD3D9.Device.DrawInfo.VertexBufferNextAddr += sizeof( VERTEX_NOTEX_3D ) * 3 ;	\
		GD3D9.Device.DrawInfo.VertexNum += 3 ;

#define ADD4VERTEX_BILLBOARD		\
		GD3D9.Device.DrawInfo.VertexBufferNextAddr += sizeof( VERTEX_3D ) * 6 ;	\
		GD3D9.Device.DrawInfo.VertexNum += 6 ;

#define SETUPCOLOR( DEST )		\
	DEST  = ( ( ( ( ( 	GD3D9.Device.DrawInfo.DiffuseColor & 0x00ff0000 ) >> 16 ) * Red   ) / 255 ) << 16 ) |\
			( ( ( ( ( 	GD3D9.Device.DrawInfo.DiffuseColor & 0x0000ff00 ) >> 8  ) * Green ) / 255 ) << 8  ) |\
			( ( ( ( (	GD3D9.Device.DrawInfo.DiffuseColor & 0x000000ff )       ) * Blue  ) / 255 )       ) |\
			( 			GD3D9.Device.DrawInfo.DiffuseColor & 0xff000000 ) ;


// デバイスからの最大・最小のテクスチャサイズを取得できなかった時に設定するサイズ
#define DEFAULT_MAXTEXTURE_SIZE		(256)
#define DEFAULT_MINTEXTURE_SIZE		(8)

// RGB値を輝度最大のRGB値に変換するためのボリュームテクスチャのサイズ
#define RGBTOVMAXRGB_TEX_SIZE		(128)

// 構造体宣言 -----------------------------------------------------------------

// データ定義 -----------------------------------------------------------------

// 環境依存テクスチャフォーマットを D3DFORMAT に変換するためのテーブル
D_D3DFORMAT g_DXD3D9FMT_to_D3DFORMAT[] =
{
	D_D3DFMT_UNKNOWN,
	D_D3DFMT_R8G8B8,			// DX_TEXTUREFORMAT_DIRECT3D9_R8G8B8				(1)
	D_D3DFMT_A8R8G8B8,			// DX_TEXTUREFORMAT_DIRECT3D9_A8R8G8B8				(2)
	D_D3DFMT_X8R8G8B8,			// DX_TEXTUREFORMAT_DIRECT3D9_X8R8G8B8				(3)
	D_D3DFMT_R5G6B5,			// DX_TEXTUREFORMAT_DIRECT3D9_R5G6B5				(4)
	D_D3DFMT_X1R5G5B5,			// DX_TEXTUREFORMAT_DIRECT3D9_X1R5G5B5				(5)
	D_D3DFMT_A1R5G5B5,			// DX_TEXTUREFORMAT_DIRECT3D9_A1R5G5B5				(6)
	D_D3DFMT_A4R4G4B4,			// DX_TEXTUREFORMAT_DIRECT3D9_A4R4G4B4				(7)
	D_D3DFMT_R3G3B2,			// DX_TEXTUREFORMAT_DIRECT3D9_R3G3B2				(8)
	D_D3DFMT_A8R3G3B2,			// DX_TEXTUREFORMAT_DIRECT3D9_A8R3G3B2				(9)
	D_D3DFMT_X4R4G4B4,			// DX_TEXTUREFORMAT_DIRECT3D9_X4R4G4B4				(10)
	D_D3DFMT_A2B10G10R10,		// DX_TEXTUREFORMAT_DIRECT3D9_A2B10G10R10			(11)
	D_D3DFMT_G16R16,			// DX_TEXTUREFORMAT_DIRECT3D9_G16R16				(12)
	D_D3DFMT_A8B8G8R8,			// DX_TEXTUREFORMAT_DIRECT3D9_A8B8G8R8				(13)
	D_D3DFMT_X8B8G8R8,			// DX_TEXTUREFORMAT_DIRECT3D9_X8B8G8R8				(14)
	D_D3DFMT_A2R10G10B10,		// DX_TEXTUREFORMAT_DIRECT3D9_A2R10G10B10			(15)
	D_D3DFMT_A16B16G16R16,		// DX_TEXTUREFORMAT_DIRECT3D9_A16B16G16R16			(16)
	D_D3DFMT_R16F,				// DX_TEXTUREFORMAT_DIRECT3D9_R16F					(17)
	D_D3DFMT_G16R16F,			// DX_TEXTUREFORMAT_DIRECT3D9_G16R16F				(18)
	D_D3DFMT_A16B16G16R16F,		// DX_TEXTUREFORMAT_DIRECT3D9_A16B16G16R16F			(19)
	D_D3DFMT_R32F,				// DX_TEXTUREFORMAT_DIRECT3D9_R32F					(20)
	D_D3DFMT_G32R32F,			// DX_TEXTUREFORMAT_DIRECT3D9_G32R32F				(21)
	D_D3DFMT_A32B32G32R32F,		// DX_TEXTUREFORMAT_DIRECT3D9_A32B32G32R32F			(22)
} ;

// 頂点バッファに格納できる頂点の最大数のテーブル[ 0:２Ｄ頂点  1:３Ｄ頂点 ]
static const int D3DDev_VertexBuffer_MaxVertexNum[ 2 ][ VERTEXTYPE_NUM ] =
{
	D3DDEV_NOTEX_VERTMAXNUM,
	D3DDEV_TEX_VERTMAXNUM, 
	D3DDEV_BLENDTEX_VERTMAXNUM,

	D3DDEV_NOTEX_3D_VERTMAXNUM,
	D3DDEV_TEX_3D_VERTMAXNUM, 
} ;

// 単位行列
static MATRIX GlobalIdentMatrix =
{
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
} ;

// 各頂点タイプのFVFテーブル
static const DWORD D3DDev_VertexFVF[ VERTEXFVF_DECL_NUM ] =
{
	VERTEXFVF_2D_USER,
	VERTEXFVF_NOTEX_2D,
	VERTEXFVF_2D,
	VERTEXFVF_BLENDTEX_2D,
	VERTEXFVF_SHADER_2D,
	VERTEXFVF_NOTEX_3D,
	VERTEXFVF_3D,
	VERTEXFVF_3D_LIGHT,
	VERTEXFVF_SHADER_3D,
	VERTEXFVF_TEX8_2D,
} ;

// 各頂点宣言が３Ｄかどうかのテーブル
static const int D3DDev_VertexDeclaration_Is3D[ VERTEXFVF_DECL_NUM ] =
{
	0,		// VERTEXFVF_DECL_2D_USER
	0,		// VERTEXFVF_DECL_NOTEX_2D
	0,		// VERTEXFVF_DECL_2D
	0,		// VERTEXFVF_DECL_BLENDTEX_2D
	0,		// VERTEXFVF_DECL_SHADER_2D
	1,		// VERTEXFVF_DECL_NOTEX_3D
	1,		// VERTEXFVF_DECL_3D
	1,		// VERTEXFVF_DECL_3D_LIGHT
	1,		// VERTEXFVF_DECL_SHADER_3D
	0,		// VERTEXFVF_DECL_TEX8_2D
} ;

// 頂点構造の定義のテーブル[ 0:２Ｄ頂点  1:３Ｄ頂点 ]
static const DWORD D3DDev_VertexType[ 2 ][ VERTEXTYPE_NUM ] =
{
	VERTEXFVF_NOTEX_2D,
	VERTEXFVF_2D,
	VERTEXFVF_BLENDTEX_2D,

	VERTEXFVF_NOTEX_3D,
	VERTEXFVF_3D
} ;

// 頂点宣言の定義のテーブル[ 0:２Ｄ頂点  1:３Ｄ頂点 ]
static const int D3DDev_VertexDeclaration[ 2 ][ VERTEXTYPE_NUM ] =
{
	VERTEXFVF_DECL_NOTEX_2D,
	VERTEXFVF_DECL_2D,
	VERTEXFVF_DECL_BLENDTEX_2D,

	VERTEXFVF_DECL_NOTEX_3D,
	VERTEXFVF_DECL_3D
} ;

// 頂点のサイズのテーブル[ 0:２Ｄ頂点  1:３Ｄ頂点 ]
static const UINT D3DDev_VertexSize[ 2 ][ VERTEXTYPE_NUM ] =
{
	sizeof( VERTEX_NOTEX_2D ),
	sizeof( VERTEX_2D ),
	sizeof( VERTEX_BLENDTEX_2D ),

	sizeof( VERTEX_NOTEX_3D ),
	sizeof( VERTEX_3D ),
} ;

// ブレンドテーブル(デバッグ用)
static const char *__BufferBlendName[] =
{
	NULL,
	"D3DBLEND_ZERO",
	"D3DBLEND_ONE",
	"D3DBLEND_SRCCOLOR",
	"D3DBLEND_INVSRCCOLOR",
	"D3DBLEND_SRCALPHA",
	"D3DBLEND_INVSRCALPHA",
	"D3DBLEND_DESTALPHA",
	"D3DBLEND_INVDESTALPHA",
	"D3DBLEND_DESTCOLOR",
	"D3DBLEND_INVDESTCOLOR",
	"D3DBLEND_SRCALPHASAT",
	"D3DBLEND_BOTHSRCALPHA",
	"D3DBLEND_BOTHINVSRCALPHA",
} ;

// 要素テーブル(デバッグ用)
static const char *__TextureStage_ARGName[] =
{
	"D3DTA_DIFFUSE",
	"D3DTA_CURRENT",
	"D3DTA_TEXTURE",
	"D3DTA_TFACTOR",
} ;

// ブレンドテーブル(デバッグ用)
static const char *__TextureStage_OPName[] =
{
	NULL, 
	"D3DTOP_DISABLE",
	"D3DTOP_SELECTARG1",
	"D3DTOP_SELECTARG2",

	"D3DTOP_MODULATE",
	"D3DTOP_MODULATE2X",
	"D3DTOP_MODULATE4X",

	"D3DTOP_ADD",
	"D3DTOP_ADDSIGNED",
	"D3DTOP_ADDSIGNED2X",
	"D3DTOP_SUBTRACT",
	"D3DTOP_ADDSMOOTH",

	"D3DTOP_BLENDDIFFUSEALPHA",
	"D3DTOP_BLENDTEXTUREALPHA",
	"D3DTOP_BLENDFACTORALPHA",

	"D3DTOP_BLENDTEXTUREALPHAPM",
	"D3DTOP_BLENDCURRENTALPHA",

	"D3DTOP_PREMODULATE",
	"D3DTOP_MODULATEALPHA_ADDCOLOR",
	"D3DTOP_MODULATECOLOR_ADDALPHA",
	"D3DTOP_MODULATEINVALPHA_ADDCOLOR",
	"D3DTOP_MODULATEINVCOLOR_ADDALPHA",

	"D3DTOP_BUMPENVMAP",
	"D3DTOP_BUMPENVMAPLUMINANCE",
	"D3DTOP_DOTPRODUCT3",
} ;


// ＲＧＢ値ブレンドステートテーブル
// [ブレンドモード値]
// [
//   0:DESTBLEND
//   1:SRCBLEND
//   2:使用テクスチャブレンドテーブル( TEXTURE_BLEND_TABLE_INDEX_NORMAL 等 )
//   3:使用BLENDOP
//   4:αブレンド機能を使用するかどうか( 1:使用する 0:使用しない )
//   5:αチャンネルの存在する描画先の場合にピクセルシェーダーを使用したαブレンドを行うかどうか( 1:行う  0:行わない )
//   6:アルファ値が０のときに描画を行わないかどうか( 1:行わない  0:行う )
// ]
#define RGB_BLENDSTATE_TABLE_DESTBLEND				(0)
#define RGB_BLENDSTATE_TABLE_SRCBLEND				(1)
#define RGB_BLENDSTATE_TABLE_TEXBLENDTABLEINDEX		(2)
#define RGB_BLENDSTATE_TABLE_BLENDOP				(3)
#define RGB_BLENDSTATE_TABLE_ALPHABLENDENABLE		(4)
#define RGB_BLENDSTATE_TABLE_SHADERALPHABLEND		(5)

#define TEXTURE_BLEND_TABLE_INDEX_NORMAL			(0)			// 0:通常描画
#define TEXTURE_BLEND_TABLE_INDEX_MUL				(1)			// 1:乗算描画
#define TEXTURE_BLEND_TABLE_INDEX_INVERSE			(2)			// 2:RGB反転
#define TEXTURE_BLEND_TABLE_INDEX_X4				(3)			// 3:描画輝度4倍
#define TEXTURE_BLEND_TABLE_INDEX_PMA_NORMAL		(4)			// 4:乗算済みαブレンドモードの通常描画
#define TEXTURE_BLEND_TABLE_INDEX_PMA_INVERSE		(5)			// 5:乗算済みαブレンドモードのRGB反転
#define TEXTURE_BLEND_TABLE_INDEX_PMA_X4			(6)			// 6:乗算済みαブレンドモードの描画輝度4倍
#define TEXTURE_BLEND_TABLE_INDEX_NUM				(7)
static const int __RGBBlendStateTable[ DX_BLENDMODE_NUM ][ 7 ] =
{
	{ -1,						-1,							TEXTURE_BLEND_TABLE_INDEX_NORMAL,		D_D3DBLENDOP_ADD,			0, 0, 1 },	// DX_BLENDMODE_NOBLEND			ノーブレンド
	{ D_D3DBLEND_INVSRCALPHA,	D_D3DBLEND_SRCALPHA,		TEXTURE_BLEND_TABLE_INDEX_NORMAL,		D_D3DBLENDOP_ADD,			1, 1, 1 },	// DX_BLENDMODE_ALPHA			αブレンド
	{ D_D3DBLEND_ONE,			D_D3DBLEND_SRCALPHA,		TEXTURE_BLEND_TABLE_INDEX_NORMAL,		D_D3DBLENDOP_ADD,			1, 0, 1 },	// DX_BLENDMODE_ADD				加算ブレンド
	{ D_D3DBLEND_ZERO,			D_D3DBLEND_INVDESTCOLOR,	TEXTURE_BLEND_TABLE_INDEX_NORMAL,		D_D3DBLENDOP_ADD,			1, 0, 1 },	// DX_BLENDMODE_SUB				減算ブレンド
	{ D_D3DBLEND_SRCCOLOR,		D_D3DBLEND_ZERO,			TEXTURE_BLEND_TABLE_INDEX_NORMAL,		D_D3DBLENDOP_ADD,			1, 0, 0 },	// DX_BLENDMODE_MUL				乗算ブレンド
	{ D_D3DBLEND_ONE,			D_D3DBLEND_SRCALPHA,		TEXTURE_BLEND_TABLE_INDEX_NORMAL,		D_D3DBLENDOP_ADD,			1, 0, 1 },	// DX_BLENDMODE_SUB2			内部処理用減算ブレンド１
	{ -1,						-1,							TEXTURE_BLEND_TABLE_INDEX_NORMAL,		D_D3DBLENDOP_ADD,			0, 0, 1 },	// DX_BLENDMODE_XOR				XORブレンド(非対応)
	{ -1,						-1,							TEXTURE_BLEND_TABLE_INDEX_NORMAL,		D_D3DBLENDOP_ADD,			0, 0, 1 },	// 欠番
	{ D_D3DBLEND_ONE,			D_D3DBLEND_ZERO,			TEXTURE_BLEND_TABLE_INDEX_NORMAL,		D_D3DBLENDOP_ADD,			1, 0, 0 },	// DX_BLENDMODE_DESTCOLOR		カラーは更新されない
	{ D_D3DBLEND_ZERO,			D_D3DBLEND_INVDESTCOLOR,	TEXTURE_BLEND_TABLE_INDEX_NORMAL,		D_D3DBLENDOP_ADD,			1, 0, 0 },	// DX_BLENDMODE_INVDESTCOLOR	描画先の色の反転値を掛ける
	{ D_D3DBLEND_INVSRCALPHA,	D_D3DBLEND_SRCALPHA,		TEXTURE_BLEND_TABLE_INDEX_INVERSE,		D_D3DBLENDOP_ADD,			1, 1, 1 },	// DX_BLENDMODE_INVSRC			描画元の色を反転する
	{ D_D3DBLEND_SRCCOLOR,		D_D3DBLEND_ZERO,			TEXTURE_BLEND_TABLE_INDEX_MUL,			D_D3DBLENDOP_ADD,			1, 0, 1 },	// DX_BLENDMODE_MULA			アルファチャンネル考慮付き乗算ブレンド
	{ D_D3DBLEND_INVSRCALPHA,	D_D3DBLEND_SRCALPHA,		TEXTURE_BLEND_TABLE_INDEX_X4,			D_D3DBLENDOP_ADD,			1, 1, 1 },	// DX_BLENDMODE_ALPHA_X4		αブレンドの描画側の輝度を最大４倍にできるモード
	{ D_D3DBLEND_ONE,			D_D3DBLEND_SRCALPHA,		TEXTURE_BLEND_TABLE_INDEX_X4,			D_D3DBLENDOP_ADD,			1, 0, 1 },	// DX_BLENDMODE_ADD_X4			加算ブレンドの描画側の輝度を最大４倍にできるモード
	{ D_D3DBLEND_ZERO,			D_D3DBLEND_ONE,				TEXTURE_BLEND_TABLE_INDEX_NORMAL,		D_D3DBLENDOP_ADD,			1, 0, 0 },	// DX_BLENDMODE_SRCCOLOR		描画元のカラーでそのまま描画される
	{ D_D3DBLEND_INVSRCALPHA,	D_D3DBLEND_ONE,				TEXTURE_BLEND_TABLE_INDEX_NORMAL,		D_D3DBLENDOP_ADD,			1, 0, 0 },	// DX_BLENDMODE_HALF_ADD		半加算ブレンド
	{ D_D3DBLEND_ONE,			D_D3DBLEND_SRCALPHA,		TEXTURE_BLEND_TABLE_INDEX_NORMAL,		D_D3DBLENDOP_REVSUBTRACT,	1, 0, 1 },	// DX_BLENDMODE_SUB1			出力ブレンドが使用可能な場合の減算ブレンド

	{ D_D3DBLEND_INVSRCALPHA,	D_D3DBLEND_ONE,				TEXTURE_BLEND_TABLE_INDEX_PMA_NORMAL,	D_D3DBLENDOP_ADD,			1, 0, 1 },	// DX_BLENDMODE_PMA_ALPHA		乗算済みαブレンドモードのαブレンド
	{ D_D3DBLEND_ONE,			D_D3DBLEND_ONE,				TEXTURE_BLEND_TABLE_INDEX_PMA_NORMAL,	D_D3DBLENDOP_ADD,			1, 0, 1 },	// DX_BLENDMODE_PMA_ADD			乗算済みαブレンドモードの加算ブレンド
	{ D_D3DBLEND_ONE,			D_D3DBLEND_ONE,				TEXTURE_BLEND_TABLE_INDEX_PMA_NORMAL,	D_D3DBLENDOP_REVSUBTRACT,	1, 0, 1 },	// DX_BLENDMODE_PMA_SUB			乗算済みαブレンドモードの減算ブレンド
	{ D_D3DBLEND_INVSRCALPHA,	D_D3DBLEND_ONE,				TEXTURE_BLEND_TABLE_INDEX_PMA_INVERSE,	D_D3DBLENDOP_ADD,			1, 0, 1 },	// DX_BLENDMODE_PMA_INVSRC		乗算済みαブレンドモードの描画元の色を反転する
	{ D_D3DBLEND_INVSRCALPHA,	D_D3DBLEND_ONE,				TEXTURE_BLEND_TABLE_INDEX_PMA_X4,		D_D3DBLENDOP_ADD,			1, 0, 1 },	// DX_BLENDMODE_PMA_ALPHA_X4	乗算済みαブレンドモードのαブレンドの描画側の輝度を最大４倍にできるモード
	{ D_D3DBLEND_ONE,			D_D3DBLEND_ONE,				TEXTURE_BLEND_TABLE_INDEX_PMA_X4,		D_D3DBLENDOP_ADD,			1, 0, 1 },	// DX_BLENDMODE_PMA_ADD_X4		乗算済みαブレンドモードの加算ブレンドの描画側の輝度を最大４倍にできるモード
} ;

// Ａ値ブレンドステートテーブル[ブレンドモード値][ 0:DESTBLEND  1:SRCBLEND  2:シェーダーαブレンドを使用するか( 0:使用しない  1:使用する )  3:使用BLENDOP ]
#define ALPHA_BLENDSTATE_TABLE_DESTBLEND			(0)
#define ALPHA_BLENDSTATE_TABLE_SRCBLEND				(1)
#define ALPHA_BLENDSTATE_TABLE_USESHADERBLEND		(2)
#define ALPHA_BLENDSTATE_TABLE_BLENDOP				(3)
static const int __ABlendStateTable[ DX_BLENDMODE_NUM ][ 4 ] =
{
	{ -1,						-1,							0, D_D3DBLENDOP_ADD			},  // DX_BLENDMODE_NOBLEND			ノーブレンド
	{ -1,						-1,							1, D_D3DBLENDOP_ADD			},	// DX_BLENDMODE_ALPHA			αブレンド
	{ D_D3DBLEND_ONE,			D_D3DBLEND_ZERO,			0, D_D3DBLENDOP_ADD			},	// DX_BLENDMODE_ADD				加算ブレンド
	{ D_D3DBLEND_ONE,			D_D3DBLEND_ZERO,			0, D_D3DBLENDOP_ADD			},	// DX_BLENDMODE_SUB				減算ブレンド
	{ D_D3DBLEND_ONE,			D_D3DBLEND_ZERO,			0, D_D3DBLENDOP_ADD			},	// DX_BLENDMODE_MUL				乗算ブレンド
	{ D_D3DBLEND_ONE,			D_D3DBLEND_ZERO,			0, D_D3DBLENDOP_ADD			},	// DX_BLENDMODE_SUB2			内部処理用減算ブレンド１
	{ -1,						-1,							0, D_D3DBLENDOP_ADD			},	// DX_BLENDMODE_XOR				XORブレンド(非対応)
	{ -1,						-1,							0, D_D3DBLENDOP_ADD			},	// 欠番
	{ D_D3DBLEND_ONE,			D_D3DBLEND_ZERO,			0, D_D3DBLENDOP_ADD			},	// DX_BLENDMODE_DESTCOLOR		カラーは更新されない
	{ D_D3DBLEND_ONE,			D_D3DBLEND_ZERO,			0, D_D3DBLENDOP_ADD			},	// DX_BLENDMODE_INVDESTCOLOR	描画先の色の反転値を掛ける
	{ -1,						-1,							1, D_D3DBLENDOP_ADD			},	// DX_BLENDMODE_INVSRC			描画元の色を反転する
	{ D_D3DBLEND_ONE,			D_D3DBLEND_ZERO,			0, D_D3DBLENDOP_ADD			},	// DX_BLENDMODE_MULA			アルファチャンネル考慮付き乗算ブレンド
	{ -1,						-1,							1, D_D3DBLENDOP_ADD			},	// DX_BLENDMODE_ALPHA_X4		αブレンドの描画側の輝度を最大４倍にできるモード
	{ D_D3DBLEND_ONE,			D_D3DBLEND_ZERO,			0, D_D3DBLENDOP_ADD			},	// DX_BLENDMODE_ADD_X4			加算ブレンドの描画側の輝度を最大４倍にできるモード
	{ D_D3DBLEND_ZERO,			D_D3DBLEND_ONE,				0, D_D3DBLENDOP_ADD			},	// DX_BLENDMODE_SRCCOLOR		描画元のカラーでそのまま描画される
	{ D_D3DBLEND_INVSRCALPHA,	D_D3DBLEND_ONE,				0, D_D3DBLENDOP_ADD			},	// DX_BLENDMODE_HALF_ADD		半加算ブレンド
	{ D_D3DBLEND_ONE,			D_D3DBLEND_ZERO,			0, D_D3DBLENDOP_ADD			},	// DX_BLENDMODE_SUB1			出力ブレンドが使用可能な場合の減算ブレンド

	{ D_D3DBLEND_INVSRCALPHA,	D_D3DBLEND_ONE,				0, D_D3DBLENDOP_ADD			},	// DX_BLENDMODE_PMA_ALPHA		乗算済みαブレンドモードのαブレンド
	{ D_D3DBLEND_ONE,			D_D3DBLEND_ONE,				0, D_D3DBLENDOP_ADD			},	// DX_BLENDMODE_PMA_ADD			乗算済みαブレンドモードの加算ブレンド
	{ D_D3DBLEND_ONE,			D_D3DBLEND_ONE,				0, D_D3DBLENDOP_REVSUBTRACT },	// DX_BLENDMODE_PMA_SUB			乗算済みαブレンドモードの減算ブレンド
	{ D_D3DBLEND_INVSRCALPHA,	D_D3DBLEND_ONE,				0, D_D3DBLENDOP_ADD			},	// DX_BLENDMODE_PMA_INVSRC		乗算済みαブレンドモードの描画元の色を反転する
	{ D_D3DBLEND_INVSRCALPHA,	D_D3DBLEND_ONE,				0, D_D3DBLENDOP_ADD			},	// DX_BLENDMODE_PMA_ALPHA_X4	乗算済みαブレンドモードのαブレンドの描画側の輝度を最大４倍にできるモード
	{ D_D3DBLEND_ONE,			D_D3DBLEND_ONE,				0, D_D3DBLENDOP_ADD			},	// DX_BLENDMODE_PMA_ADD_X4		乗算済みαブレンドモードの加算ブレンドの描画側の輝度を最大４倍にできるモード
} ;

// シェーダーバイナリ配列
extern int  DxShaderCodeBin_ModelConvert ;
extern BYTE DxShaderCodeBin_Model[] ;

extern int  DxShaderCodeBin_FilterConvert ;
extern BYTE DxShaderCodeBin_Filter[] ;

extern int  DxShaderCodeBin_Base_D3D9Convert ;
extern BYTE DxShaderCodeBin_Base_D3D9[] ;

extern int  DxShaderCodeBin_Base3D_D3D9Convert ;
extern BYTE DxShaderCodeBin_Base3D_D3D9[] ;

extern int  DxShaderCodeBin_RgbaMixConvert ;
extern BYTE DxShaderCodeBin_RgbaMix[] ;

// Direct3D9 を使ったグラフィックス処理情報の構造体
GRAPHICS_HARDDATA_DIRECT3D9 GraphicsHardDataDirect3D9 ;

// 関数宣言 -------------------------------------------------------------------

static void Graphics_D3D9_VertexShaderArray_Release(      D_IDirect3DVertexShader9      **pObject, int Num ) ;		// 頂点シェーダー配列を解放する
static void Graphics_D3D9_PixelShaderArray_Release(       D_IDirect3DPixelShader9       **pObject, int Num ) ;		// ピクセルシェーダー配列を解放する

static int EnumDirect3DAdapter() ;												// Direct3D のアダプタ情報を列挙する

static  D_D3DVERTEXELEMENT9	MakeVertexElement9( BYTE Stream, BYTE Offset, BYTE Type, BYTE Method, BYTE Usage, BYTE UsageIndex ) ;

static	int		SetPlaneVertexHardware( VERTEX_2D *GraphVert, int xnum, int ynum ) ;		// ２次元配列的に配置された頂点データを頂点バッファに追加する

// Direct3D9 に設定する射影行列を更新する
static void Graphics_Hardware_D3D9_RefreshProjectionMatrix( int Update3DProjection, int Update2DProjection, int AlwaysUpdate ) ;

// 固定機能パイプライン互換処理用の頂点宣言と頂点シェーダーの設定を行う
static void Graphics_Hardware_D3D9_SetFVF_UseTable( int FVFDecl /* VERTEXFVF_DECL_2D_USER等 */ ) ;

#ifndef DX_NON_MODEL
static	int		CreateRGBtoVMaxRGBVolumeTexture() ;								// RGBカラーを輝度を最大にしたRGB値に変換するためのボリュームテクスチャを作成する( 0:成功  -1:失敗 )
#endif // DX_NON_MODEL

static int Graphics_D3D9_BltBmpOrBaseImageToGraph3_MipMapBlt(
	      IMAGEDATA_ORIG			*Orig,
	const RECT						*SrcRect,
	const RECT						*DestRect,
	      int						UseSysMemSurface,
	      D_IDirect3DBaseTexture9	*UseTex,
	      int						TexWidth,
	      int						TexHeight,
	const BASEIMAGE					*RgbBaseImage,
	const BASEIMAGE					*AlphaBaseImage,
	const COLORDATA					*DestColor,
	      int						RedIsAlphaFlag,
	      int						UseTransColorConvAlpha,
	      int						ASyncThread = FALSE
) ;

static int Graphics_D3D9_BltBmpOrBaseImageToGraph3_NoMipMapBlt(
	      IMAGEDATA_ORIG			*Orig,
	const RECT						*SrcRect,
	const RECT						*DestRect,
	      int						UseSysMemSurface,
	      D_IDirect3DBaseTexture9	*UseTex,
	      int						TexWidth,
	      int						TexHeight,
	      int						TexUseWidth,
	      int						TexUseHeight,
	const BASEIMAGE					*RgbBaseImage,
	const BASEIMAGE					*AlphaBaseImage,
	const COLORDATA					*DestColor,
	      int						RedIsAlphaFlag,
	      int						UseTransColorConvAlpha,
	      int						ASyncThread = FALSE
) ;

__inline	static DWORD GetDiffuseColor( void )															// 現在のディフューズカラーを得る
			{
				return  ( ( DWORD )GSYS.DrawSetting.DrawBright.Red   << 16 ) |
						( ( DWORD )GSYS.DrawSetting.DrawBright.Green << 8  ) |
						( ( DWORD )GSYS.DrawSetting.DrawBright.Blue        ) |
						( ( ( GSYS.DrawSetting.BlendMode != DX_BLENDMODE_NOBLEND || GSYS.DrawSetting.UseNoBlendModeParam ) ? ( DWORD )GSYS.DrawSetting.BlendParam : 255 ) << 24 ) ;
			}

// プログラム -----------------------------------------------------------------

// Direct3D9 の初期化関係

#ifndef DX_NON_MODEL

__inline void RGBtoVMaxRGBI( int R, int G, int B, BYTE &RD, BYTE &GD, BYTE &BD )
{
	int h, s ;

	if( R + G + B == 0 )
	{
		RD = 255 ;
		GD = 255 ;
		BD = 255 ;
		return ;
	}

	if( R > 255 ) R = 255 ;
	if( G > 255 ) G = 255 ;
	if( B > 255 ) B = 255 ;

	if( R > G )
	{
		if( R > B )
		{
			if( G > B )
			{
				// R > G > B
				if( R == B )
				{
					RD = 255 ;
					GD = 255 ;
					BD = 255 ;
				}
				else
				{
					h = ( G - B ) * 255 / ( R - B ) ;
					s = ( R - B ) * 255 / R ;

					RD = 255 ;
					GD = ( BYTE )( 255 - ( 255 - h ) * s / 255 ) ;
					BD = ( BYTE )( 255 - s ) ;
				}
			}
			else
			{
				// R > B > G
				if( R == G )
				{
					RD = 255 ;
					GD = 255 ;
					BD = 255 ;
				}
				else
				{
					h = ( B - G ) * 255 / ( R - G ) ;
					s = ( R - G ) * 255 / R ;

					RD = 255 ;
					GD = ( BYTE )( 255 - s ) ;
					BD = ( BYTE )( 255 - ( 255 - h ) * s / 255 ) ;
				}
			}
		}
		else
		{
			// B > R > G
			if( B == G )
			{
				RD = 255 ;
				GD = 255 ;
				BD = 255 ;
			}
			else
			{
				h = ( R - G ) * 255 / ( B - G ) ;
				s = ( B - G ) * 255 / B ;

				RD = ( BYTE )( 255 - ( 255 - h ) * s / 255 ) ;
				GD = ( BYTE )( 255 - s ) ;
				BD = 255 ;
			}
		}
	}
	else
	{
		if( G > B )
		{
			if( R > B )
			{
				// G > R > B
				if( G == B )
				{
					RD = 255 ;
					GD = 255 ;
					BD = 255 ;
				}
				else
				{
					h = ( R - B ) * 255 / ( G - B ) ;
					s = ( G - B ) * 255 / G ;

					RD = ( BYTE )( 255 - ( 255 - h ) * s / 255 ) ;
					GD = 255 ;
					BD = ( BYTE )( 255 - s ) ;
				}
			}
			else
			{
				// G > B > R
				if( G == R )
				{
					RD = 255 ;
					GD = 255 ;
					BD = 255 ;
				}
				else
				{
					h = ( B - R ) * 255 / ( G - R ) ;
					s = ( G - R ) * 255 / G ;

					RD = ( BYTE )( 255 - s ) ;
					GD = 255 ;
					BD = ( BYTE )( 255 - ( 255 - h ) * s / 255 ) ;
				}
			}
		}
		else
		{
			// B > G > R
			if( B == R )
			{
				RD = 255 ;
				GD = 255 ;
				BD = 255 ;
			}
			else
			{
				h = ( G - R ) * 255 / ( B - R ) ;
				s = ( B - R ) * 255 / B ;

				RD = ( BYTE )( 255 - s ) ;
				GD = ( BYTE )( 255 - ( 255 - h ) * s / 255 ) ;
				BD = 255 ;
			}
		}
	}
}

// RGBカラーを輝度を最大にしたRGB値に変換するためのボリュームテクスチャを作成する( 0:成功  -1:失敗 )
static int CreateRGBtoVMaxRGBVolumeTexture()
{
	D_IDirect3DVolumeTexture9 *TempMemTexture9 ;
	D_D3DLOCKED_BOX LockInfo ;
	D_D3DBOX LockBox ;
	int r, g, b ;
	BYTE *pDest ;
	HRESULT hr ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	GD3D9.RGBtoVMaxRGBVolumeTexture = NULL ;
	TempMemTexture9 = NULL ;

	// ボリュームテクスチャの作成
	hr = Direct3DDevice9_CreateVolumeTexture(
		RGBTOVMAXRGB_TEX_SIZE,
		RGBTOVMAXRGB_TEX_SIZE,
		RGBTOVMAXRGB_TEX_SIZE,
		1,
		0,
		D_D3DFMT_X8R8G8B8,
		D_D3DPOOL_DEFAULT,
		&GD3D9.RGBtoVMaxRGBVolumeTexture,
		NULL ) ;
	if( hr != D_D3D_OK )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x52\x00\x47\x00\x42\x00\xab\x30\xe9\x30\xfc\x30\x92\x30\x1d\x8f\xa6\x5e\x92\x30\x00\x67\x27\x59\x6b\x30\x57\x30\x5f\x30\x52\x00\x47\x00\x42\x00\x24\x50\x6b\x30\x09\x59\xdb\x63\x59\x30\x8b\x30\x5f\x30\x81\x30\x6e\x30\xdc\x30\xea\x30\xe5\x30\xfc\x30\xe0\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"RGBカラーを輝度を最大にしたRGB値に変換するためのボリュームテクスチャの作成に失敗しました\n" @*/ ) ;
		goto ERR ;
	}

	// 転送用システムメモリボリュームテクスチャの作成
	if( Direct3DDevice9_CreateVolumeTexture(
		RGBTOVMAXRGB_TEX_SIZE,
		RGBTOVMAXRGB_TEX_SIZE,
		RGBTOVMAXRGB_TEX_SIZE,
		1,
		0,
		D_D3DFMT_X8R8G8B8,
		D_D3DPOOL_SYSTEMMEM,
		&TempMemTexture9,
		NULL ) != D_D3D_OK )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x52\x00\x47\x00\x42\x00\xab\x30\xe9\x30\xfc\x30\x92\x30\x1d\x8f\xa6\x5e\x92\x30\x00\x67\x27\x59\x6b\x30\x57\x30\x5f\x30\x52\x00\x47\x00\x42\x00\x24\x50\x6b\x30\x09\x59\xdb\x63\x59\x30\x8b\x30\x5f\x30\x81\x30\x6e\x30\xe2\x8e\x01\x90\x43\x51\x28\x75\xdc\x30\xea\x30\xe5\x30\xfc\x30\xe0\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"RGBカラーを輝度を最大にしたRGB値に変換するための転送元用ボリュームテクスチャの作成に失敗しました\n" @*/ ) ;
		goto ERR ;
	}

	// システムメモリボリュームテクスチャのロック
	_MEMSET( &LockInfo, 0, sizeof( LockInfo ) ) ;
	LockBox.Left   = 0 ;
	LockBox.Top    = 0 ;
	LockBox.Front  = 0 ;
	LockBox.Right  = RGBTOVMAXRGB_TEX_SIZE ;
	LockBox.Bottom = RGBTOVMAXRGB_TEX_SIZE ;
	LockBox.Back   = RGBTOVMAXRGB_TEX_SIZE ;
	hr = TempMemTexture9->LockBox( 0, &LockInfo, &LockBox, 0 ) ;
	if( hr == D_D3D_OK )
	{
		// システムメモリボリュームテクスチャに変換情報を書き込む
		for( b = 0 ; b < RGBTOVMAXRGB_TEX_SIZE ; b ++ )
		{
			for( g = 0 ; g < RGBTOVMAXRGB_TEX_SIZE ; g ++ )
			{
				pDest = ( BYTE * )LockInfo.pBits + LockInfo.SlicePitch * b + LockInfo.RowPitch * g ;
				for( r = 0 ; r < RGBTOVMAXRGB_TEX_SIZE ; r ++, pDest += 4 )
				{
					RGBtoVMaxRGBI( r * 2, g * 2, b * 2, pDest[ 2 ], pDest[ 1 ], pDest[ 0 ] ) ;
					if( r < 8 && g < 8 && b < 8 )
					{
						pDest[ 0 ] = 255 ;
						pDest[ 1 ] = 255 ;
						pDest[ 2 ] = 255 ;
					}
					pDest[ 3 ] = 255 ;
				}
			}
		}
	}
	else
	{
		DXST_LOGFILE_ADDUTF16LE( "\x52\x00\x47\x00\x42\x00\xab\x30\xe9\x30\xfc\x30\x92\x30\x1d\x8f\xa6\x5e\x92\x30\x00\x67\x27\x59\x6b\x30\x57\x30\x5f\x30\x52\x00\x47\x00\x42\x00\x24\x50\x6b\x30\x09\x59\xdb\x63\x59\x30\x8b\x30\x5f\x30\x81\x30\x6e\x30\xe2\x8e\x01\x90\x43\x51\x28\x75\xdc\x30\xea\x30\xe5\x30\xfc\x30\xe0\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\xed\x30\xc3\x30\xaf\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"RGBカラーを輝度を最大にしたRGB値に変換するための転送元用ボリュームテクスチャのロックに失敗しました\n" @*/ ) ;
		goto ERR ;
	}

	// ロックの解除
	TempMemTexture9->UnlockBox( 0 ) ;

	// テクスチャの転送
	hr = Direct3DDevice9_UpdateTexture( TempMemTexture9, GD3D9.RGBtoVMaxRGBVolumeTexture ) ;

	// システムメモリボリュームテクスチャの解放
	Direct3D9_ObjectRelease( TempMemTexture9 ) ;
	TempMemTexture9 = NULL ;

	// 終了
	return 0 ;

ERR :
	if( GD3D9.RGBtoVMaxRGBVolumeTexture )
	{
		Direct3D9_ObjectRelease( GD3D9.RGBtoVMaxRGBVolumeTexture ) ;
		GD3D9.RGBtoVMaxRGBVolumeTexture = NULL ;
	}

	if( TempMemTexture9 )
	{
		Direct3D9_ObjectRelease( TempMemTexture9 ) ;
		TempMemTexture9 = NULL ;
	}
	
	return -1 ;
}

#endif // DX_NON_MODEL

// Direct3D9 を使用したグラフィックス処理の初期化を行う( 0:成功 -1:失敗 )
extern	int		Graphics_D3D9_Initialize( void )
{
	int Ret ;

	GSYS.Setting.ValidHardware = FALSE ;

	// Direct3D9.DLL の読み込み
	if( Direct3D9_LoadDLL() < 0 )
	{
		goto ERR ;
	}

	// DirectDraw7 を作成する
	if( DirectDraw7_Create() < 0 )
	{
		goto ERR ;
	}

CREATEDIRECT3D9OBJECT :
	// Direct3DDevice9 を作成する
	if( Direct3D9_Create() != 0 )
	{
		goto ERR ;
	}

	// Direct3DDevice9 の取得を試みる
	Ret = Graphics_D3D9_Device_Create() ;
	if( Ret == -2 )
	{
		goto CREATEDIRECT3D9OBJECT ;
	}
	else
	if( Ret != 0 )
	{
		goto ERR ;
	}

	// Zバッファ の作成を試みる
	if( Graphics_D3D9_CreateZBuffer() != 0 )
	{
		goto ERR ;
	}

	// シェーダーの作成
	Graphics_D3D9_Shader_Initialize() ;

	// シェーダーが使用できる場合とできない場合で処理を分岐
	if( GSYS.HardInfo.UseShader )
	{
		// 頂点宣言の作成
		Graphics_D3D9_VertexDeclaration_Create() ;

#ifndef DX_NON_MODEL
		// ボリュームテクスチャの作成
		CreateRGBtoVMaxRGBVolumeTexture() ;
#endif // DX_NON_MODEL

		// フルスクリーンモードの場合はサブバックバッファのセットアップを行う
		if( GetWindowModeFlag() == FALSE )
		{
			Graphics_D3D9_SetupSubBackBuffer() ;
		}
	}

	GSYS.Setting.ValidHardware = TRUE ;

	// 終了
	return 0 ;

ERR :
	// Zバッファ の解放
	if( GD3D9.Device.Screen.ZBufferSurface )
	{
		Direct3D9_ObjectRelease( GD3D9.Device.Screen.ZBufferSurface ) ;
		GD3D9.Device.Screen.ZBufferSurface = NULL ;
	}

	// Direct3DDevice9 の解放
	Direct3DDevice9_Release() ;
	GD3D9.Device.DrawInfo.BeginSceneFlag = FALSE ;

	// Direct3D9 の解放
	Direct3D9_Release() ;

	// Direct3D9.DLL の解放
	Direct3D9_FreeDLL() ;

	// DirectDraw7 の解放
	DirectDraw7_Release() ;

	return -1 ;
}

// Direct3D9 を使用したグラフィックス処理の後始末を行う
extern	int		Graphics_D3D9_Terminate( void )
{
	SETUP_WIN_API

#ifndef DX_NON_MODEL
	// モデルの頂点バッファを解放
	MV1TerminateVertexBufferAll() ;
#endif

	// 頂点宣言の解放
	Graphics_D3D9_VertexDeclaration_Terminate() ;

	// シェーダーの解放
	Graphics_D3D9_Shader_Terminate() ;

//	// ハードウエアアクセラレータを使用しない場合はここで画面モードを元に戻す
//	if( /*NS_GetWindowModeFlag() == FALSE &&*/
//		( GSYS.Setting.ValidHardware == FALSE ||
//		 ( WinData.WindowsVersion >= DX_WINDOWSVERSION_VISTA && ( GSYS.Screen.ValidGraphDisplayArea || GSYS.Screen.Emulation320x240Flag || GSYS.Screen.FullScreenEmulation320x240 ) ) ) )
	// ChangeDisplaySettings を使用して画面モードを変更した場合はここで画面モードを元に戻す
	if( GRAWIN.UseChangeDisplaySettings )
	{
		GRAWIN.UseChangeDisplaySettings = FALSE ;
		WinAPIData.Win32Func.ChangeDisplaySettingsAFunc( NULL, 0 ) ;
	}

	// Zバッファ の解放
	if( GD3D9.Device.Screen.ZBufferSurface )
	{
		Direct3D9_ObjectRelease( GD3D9.Device.Screen.ZBufferSurface ) ;
		GD3D9.Device.Screen.ZBufferSurface = NULL ;
	}

	// サブバックバッファの解放
	if( GD3D9.Device.Screen.SubBackBufferSurface )
	{
		Direct3D9_ObjectRelease( GD3D9.Device.Screen.SubBackBufferSurface ) ;
		GD3D9.Device.Screen.SubBackBufferSurface = NULL ;
	}
	if( GD3D9.Device.Screen.SubBackBufferTexture )
	{
		Direct3D9_ObjectRelease( GD3D9.Device.Screen.SubBackBufferTexture ) ;
		GD3D9.Device.Screen.SubBackBufferTexture = NULL ;
	}

	// バックバッファの解放
	if( GD3D9.Device.Screen.BackBufferSurface )
	{
		Direct3D9_ObjectRelease( GD3D9.Device.Screen.BackBufferSurface ) ;
		GD3D9.Device.Screen.BackBufferSurface = NULL ;
	}

#ifndef DX_NON_MODEL
	// ボリュームテクスチャの解放
	if( GD3D9.RGBtoVMaxRGBVolumeTexture )
	{
		Direct3D9_ObjectRelease( GD3D9.RGBtoVMaxRGBVolumeTexture ) ;
		GD3D9.RGBtoVMaxRGBVolumeTexture = NULL ;
	}
#endif // DX_NON_MODEL

	// Direct3DDevice9 の解放
	Direct3DDevice9_Release() ;
	GD3D9.Device.DrawInfo.BeginSceneFlag = FALSE ;

	// Direct3D9 の解放
	Direct3D9_Release() ;

	// DirectDraw の解放
	DirectDraw7_Release() ;

	// d3d9.dll の解放
	Direct3D9_FreeDLL() ;

	// ディスプレイ情報用に確保したメモリの解放
	if( GSYS.Screen.DisplayInfo )
	{
		WinAPIData.Win32Func.HeapFreeFunc( WinAPIData.Win32Func.GetProcessHeapFunc(), 0, GSYS.Screen.DisplayInfo ) ;
		GSYS.Screen.DisplayInfo = NULL ;
	}

	// 終了
	return 0 ;
}


// すべての Direct3D9 系オブジェクトを解放する
extern	int		Graphics_D3D9_ReleaseObjectAll( void )
{
	int i ;
	SHADERHANDLEDATA                          *Shader ;
	VERTEXBUFFERHANDLEDATA                    *VertexBuffer ;
	INDEXBUFFERHANDLEDATA                     *IndexBuffer ;
	GRAPHICS_HARDDATA_DIRECT3D9_SYSMEMTEXTURE *MemTex ;
	GRAPHICS_HARDDATA_DIRECT3D9_SYSMEMSURFACE *MemSurface ;
	IMAGEDATA                                 *Image ;
	SHADOWMAPDATA                             *ShadowMap ;

	if( HandleManageArray[ DX_HANDLETYPE_GRAPH ].InitializeFlag )
	{
		for( i = HandleManageArray[ DX_HANDLETYPE_GRAPH ].AreaMin ; i <= HandleManageArray[ DX_HANDLETYPE_GRAPH ].AreaMax ; i ++ )
		{
			Image = ( IMAGEDATA * )HandleManageArray[ DX_HANDLETYPE_GRAPH ].Handle[ i ] ;
			if( Image == NULL ) continue ;

#ifndef DX_NON_ASYNCLOAD
			WaitASyncLoad( Image->HandleInfo.Handle ) ;
#endif

			if( Image->Orig == NULL ) continue ;
			Graphics_Hardware_ReleaseOrigTexture_PF( Image->Orig ) ;

#ifndef DX_NON_MOVIE
			if( Image->MovieHandle != -1 )
			{
				ReleaseMovieSurface( Image->MovieHandle ) ;
			}
#endif
		}
	}

	if( HandleManageArray[ DX_HANDLETYPE_SHADOWMAP ].InitializeFlag )
	{
		for( i = HandleManageArray[ DX_HANDLETYPE_SHADOWMAP ].AreaMin ; i <= HandleManageArray[ DX_HANDLETYPE_SHADOWMAP ].AreaMax ; i ++ )
		{
			ShadowMap = ( SHADOWMAPDATA * )HandleManageArray[ DX_HANDLETYPE_SHADOWMAP ].Handle[ i ] ;
			if( ShadowMap == NULL ) continue ;

#ifndef DX_NON_ASYNCLOAD
			WaitASyncLoad( ShadowMap->HandleInfo.Handle ) ;
#endif

			Graphics_Hardware_D3D9_ShadowMap_ReleaseTexture_PF( ShadowMap  ) ;
		}
	}

	if( HandleManageArray[ DX_HANDLETYPE_VERTEX_BUFFER ].InitializeFlag )
	{
		for( i = HandleManageArray[ DX_HANDLETYPE_VERTEX_BUFFER ].AreaMin ; i <= HandleManageArray[ DX_HANDLETYPE_VERTEX_BUFFER ].AreaMax ; i ++ )
		{
			VertexBuffer = ( VERTEXBUFFERHANDLEDATA * )HandleManageArray[ DX_HANDLETYPE_VERTEX_BUFFER ].Handle[ i ] ;
			if( VertexBuffer == NULL ) continue ;

#ifndef DX_NON_ASYNCLOAD
			WaitASyncLoad( VertexBuffer->HandleInfo.Handle ) ;
#endif

			Graphics_D3D9_VertexBuffer_ReleaseObject( VertexBuffer ) ;
		}
	}

	if( HandleManageArray[ DX_HANDLETYPE_INDEX_BUFFER ].InitializeFlag )
	{
		for( i = HandleManageArray[ DX_HANDLETYPE_INDEX_BUFFER ].AreaMin ; i <= HandleManageArray[ DX_HANDLETYPE_INDEX_BUFFER ].AreaMax ; i ++ )
		{
			IndexBuffer = ( INDEXBUFFERHANDLEDATA * )HandleManageArray[ DX_HANDLETYPE_INDEX_BUFFER ].Handle[ i ] ;
			if( IndexBuffer == NULL ) continue ;

#ifndef DX_NON_ASYNCLOAD
			WaitASyncLoad( IndexBuffer->HandleInfo.Handle ) ;
#endif

			Graphics_D3D9_IndexBuffer_ReleaseObject( IndexBuffer ) ;
		}
	}

	if( HandleManageArray[ DX_HANDLETYPE_SHADER ].InitializeFlag )
	{
		for( i = HandleManageArray[ DX_HANDLETYPE_SHADER ].AreaMin ; i <= HandleManageArray[ DX_HANDLETYPE_SHADER ].AreaMax ; i ++ )
		{
			Shader = ( SHADERHANDLEDATA * )HandleManageArray[ DX_HANDLETYPE_SHADER ].Handle[ i ] ;
			if( Shader == NULL ) continue ;

#ifndef DX_NON_ASYNCLOAD
			WaitASyncLoad( Shader->HandleInfo.Handle ) ;
#endif

			if( Shader->IsVertexShader )
			{
				if( Shader->PF->D3D9.VertexShader )
				{
					Direct3D9_ObjectRelease( Shader->PF->D3D9.VertexShader ) ;
					Shader->PF->D3D9.VertexShader = NULL ;
				}
			}
			else
			{
				if( Shader->PF->D3D9.PixelShader )
				{
					Direct3D9_ObjectRelease( Shader->PF->D3D9.PixelShader ) ;
					Shader->PF->D3D9.PixelShader = NULL ;
				}
			}
		}
	}

	MemTex = GD3D9.SysMemTexSurf.Texture ;
	for( i = 0 ; i < GD3D9.SysMemTexSurf.TextureInitNum ; MemTex ++ )
	{
		if( MemTex->InitializeFlag == FALSE )
			continue ;

		i ++ ;

		if( MemTex->MemTexture != NULL )
		{
			Direct3D9_ObjectRelease( MemTex->MemTexture ) ;
			MemTex->MemTexture = NULL ;
		}
	}

	MemSurface = GD3D9.SysMemTexSurf.Surface ;
	for( i = 0 ; i < GD3D9.SysMemTexSurf.SurfaceInitNum ; MemSurface ++ )
	{
		if( MemSurface->InitializeFlag == FALSE )
			continue ;

		i ++ ;

		if( MemSurface->MemSurface != NULL )
		{
			Direct3D9_ObjectRelease( MemSurface->MemSurface ) ;
			MemSurface->MemSurface = NULL ;
		}
	}

	// 終了
	return 0 ;
}


// すべての Direct3D9 系オブジェクトを作成する
extern	int		Graphics_D3D9_CreateObjectAll( void )
{
	int                                       i ;
	SHADERHANDLEDATA                          *Shader ;
	VERTEXBUFFERHANDLEDATA                    *VertexBuffer ;
	INDEXBUFFERHANDLEDATA                     *IndexBuffer ;
	GRAPHICS_HARDDATA_DIRECT3D9_SYSMEMTEXTURE *MemTex ;
	GRAPHICS_HARDDATA_DIRECT3D9_SYSMEMSURFACE *MemSurface ;
	IMAGEDATA                                 *Image ;
	SHADOWMAPDATA                             *ShadowMap ;

	MemTex = GD3D9.SysMemTexSurf.Texture ;
	for( i = 0 ; i < GD3D9.SysMemTexSurf.TextureInitNum ; MemTex ++ )
	{
		if( MemTex->InitializeFlag == FALSE )
			continue ;

		i ++ ;

		Graphics_D3D9_CreateSysMemTextureDirect3D9Texture( MemTex ) ;
	}

	MemSurface = GD3D9.SysMemTexSurf.Surface ;
	for( i = 0 ; i < GD3D9.SysMemTexSurf.SurfaceInitNum ; MemSurface ++ )
	{
		if( MemSurface->InitializeFlag == FALSE )
			continue ;

		i ++ ;

		Graphics_D3D9_CreateSysMemSurfaceDirect3D9Surface( MemSurface ) ;
	}

	if( HandleManageArray[ DX_HANDLETYPE_GRAPH ].InitializeFlag )
	{
		for( i = HandleManageArray[ DX_HANDLETYPE_GRAPH ].AreaMin ; i <= HandleManageArray[ DX_HANDLETYPE_GRAPH ].AreaMax ; i ++ )
		{
			Image = ( IMAGEDATA * )HandleManageArray[ DX_HANDLETYPE_GRAPH ].Handle[ i ] ;

			if( Image == NULL ) continue ;
			if( Image->Orig == NULL ) continue ;
			Graphics_Hardware_CreateOrigTexture_PF( Image->Orig ) ;
		}
	}

	if( HandleManageArray[ DX_HANDLETYPE_SHADOWMAP ].InitializeFlag )
	{
		for( i = HandleManageArray[ DX_HANDLETYPE_SHADOWMAP ].AreaMin ; i <= HandleManageArray[ DX_HANDLETYPE_SHADOWMAP ].AreaMax ; i ++ )
		{
			ShadowMap = ( SHADOWMAPDATA * )HandleManageArray[ DX_HANDLETYPE_SHADOWMAP ].Handle[ i ] ;

			if( ShadowMap == NULL ) continue ;
			Graphics_Hardware_D3D9_ShadowMap_CreateTexture_PF( ShadowMap ) ;
		}
	}

	if( HandleManageArray[ DX_HANDLETYPE_VERTEX_BUFFER ].InitializeFlag )
	{
		for( i = HandleManageArray[ DX_HANDLETYPE_VERTEX_BUFFER ].AreaMin ; i <= HandleManageArray[ DX_HANDLETYPE_VERTEX_BUFFER ].AreaMax ; i ++ )
		{
			VertexBuffer = ( VERTEXBUFFERHANDLEDATA * )HandleManageArray[ DX_HANDLETYPE_VERTEX_BUFFER ].Handle[ i ] ;
			if( VertexBuffer == NULL ) continue ;
			Graphics_D3D9_VertexBuffer_CreateObject( VertexBuffer, TRUE ) ;
		}
	}

	if( HandleManageArray[ DX_HANDLETYPE_INDEX_BUFFER ].InitializeFlag )
	{
		for( i = HandleManageArray[ DX_HANDLETYPE_INDEX_BUFFER ].AreaMin ; i <= HandleManageArray[ DX_HANDLETYPE_INDEX_BUFFER ].AreaMax ; i ++ )
		{
			IndexBuffer = ( INDEXBUFFERHANDLEDATA * )HandleManageArray[ DX_HANDLETYPE_INDEX_BUFFER ].Handle[ i ] ;
			if( IndexBuffer == NULL ) continue ;
			Graphics_D3D9_IndexBuffer_CreateObject( IndexBuffer, TRUE ) ;
		}
	}

	if( HandleManageArray[ DX_HANDLETYPE_SHADER ].InitializeFlag )
	{
		for( i = HandleManageArray[ DX_HANDLETYPE_SHADER ].AreaMin ; i <= HandleManageArray[ DX_HANDLETYPE_SHADER ].AreaMax ; i ++ )
		{
			Shader = ( SHADERHANDLEDATA * )HandleManageArray[ DX_HANDLETYPE_SHADER ].Handle[ i ] ;
			if( Shader == NULL ) continue ;
			if( Shader->IsVertexShader )
			{
				Direct3DDevice9_CreateVertexShader( ( DWORD * )Shader->FunctionCode, &Shader->PF->D3D9.VertexShader ) ;
			}
			else
			{
				Direct3DDevice9_CreatePixelShader( ( DWORD * )Shader->FunctionCode, &Shader->PF->D3D9.PixelShader ) ;
			}
		}
	}

	// 終了
	return 0 ;
}

// ピクセルシェーダーを使用する場合は出力先をテクスチャとして使用できるかどうかのチェックを行う
extern	int		Graphics_D3D9_SetupRenderTargetInputTextureFlag( void )
{
#ifndef DX_NON_FILTER
	// ピクセルシェーダーを使用する場合は出力先をテクスチャとして使用できるかどうかのチェックを行う
	GD3D9.Device.Caps.ValidRenderTargetInputTexture = FALSE ;
/*	if( GSYS.Setting.ValidHardware == TRUE && GSYS.HardInfo.UseShader == TRUE )
	{
		int TempScreen ;
		BASEIMAGE TempInImage ;
		BASEIMAGE TempOutImage ;
		BASEIMAGE TempScreenImage ;
		RECT LockRect ;
		int i, j, s, count ;
		int r, g, b, a ;
		const int rgb[ 4 * 3 ] =
		{
			204, 168,  80,
			128, 100, 245,
			 66, 222, 143,
			 34, 255,  78,
		} ;
		SETUP_GRAPHHANDLE_GPARAM GParam ;

		Graphics_Image_InitSetupGraphHandleGParam( &GParam ) ;
		GParam.DrawValidImageCreateFlag      = TRUE ;
		GParam.DrawValidAlphaImageCreateFlag = FALSE ;
		GParam.CubeMapTextureCreateFlag      = FALSE ;
		TempScreen = Graphics_Image_MakeGraph_UseGParam( &GParam, 8, 8, FALSE, FALSE ) ;
		NS_CreateXRGB8ColorBaseImage( 8, 8, &TempInImage ) ;
		NS_CreateXRGB8ColorBaseImage( 8, 8, &TempOutImage ) ;

		j = 0 ;
		for( i = 0 ; i < 4 ; i ++, j += 3 )
		{
			NS_SetPixelBaseImage( &TempInImage, i, i, rgb[ j + 0 ], rgb[ j + 1 ], rgb[ j + 2 ], 255 ) ;
		}

		NS_BltBmpOrGraphImageToGraph( NULL, NULL, NULL, FALSE, &TempInImage, NULL, 0, 0, TempScreen ) ;
		GD3D9.Device.Caps.ValidRenderTargetInputTexture = TRUE ;
		NS_GraphFilter( TempScreen, DX_GRAPH_FILTER_INVERT ) ;
		GD3D9.Device.Caps.ValidRenderTargetInputTexture = FALSE ;
		LockRect.left   = 0 ;
		LockRect.top    = 0 ;
		LockRect.right  = 8 ;
		LockRect.bottom = 8 ;
		if( Graphics_Screen_LockDrawScreen( &LockRect, &TempScreenImage, TempScreen, 0, TRUE, 0 ) != -1 )
		{
			NS_BltBaseImage( 0, 0, &TempScreenImage, &TempOutImage ) ;
			Graphics_Screen_UnlockDrawScreen() ;

			count = 0 ;
			j = 0 ;
			for( i = 0 ; i < 4 ; i ++, j += 3 )
			{
				NS_GetPixelBaseImage( &TempOutImage, i, i, &r, &g, &b, &a ) ;
				s = r - ( 255 - rgb[ j + 0 ] ) ; if( s >= -1 && s <= 1 ) count ++ ;
				s = g - ( 255 - rgb[ j + 1 ] ) ; if( s >= -1 && s <= 1 ) count ++ ;
				s = b - ( 255 - rgb[ j + 2 ] ) ; if( s >= -1 && s <= 1 ) count ++ ;
			}
			if( count == 4 * 3 )
			{
				GD3D9.Device.Caps.ValidRenderTargetInputTexture = TRUE ;
			}
		}
		NS_DeleteGraph( TempScreen ) ;
		NS_ReleaseBaseImage( &TempInImage ) ;
		NS_ReleaseBaseImage( &TempOutImage ) ;
	}*/
#endif // DX_NON_FILTER

	// 終了
	return 0 ;
}


























// シェーダーコード関係

// シェーダーコードパッケージからシェーダーコードバイナリのアドレスとサイズを取得する
static void Graphics_D3D11_ShaderCodePackage_GetInfo( WORD **SizeBuf, BYTE **ShaderAddr, void **AddrBuf, int Num )
{
	BYTE *ShaderAddrTemp = *ShaderAddr ;
	WORD *SizeBufTemp    = *SizeBuf ;
	int i ;

	for( i = 0 ; i < Num ; i ++, SizeBufTemp ++, AddrBuf ++ )
	{
		if( *SizeBufTemp != 0 )
		{
			*AddrBuf = ( void * )ShaderAddrTemp ;
			ShaderAddrTemp += *SizeBufTemp ;
		}
	}

	*ShaderAddr = ShaderAddrTemp ;
	*SizeBuf    = SizeBufTemp ;
}

// Direct3D9 の標準描画用のシェーダーコードの初期化を行う
extern int Graphics_D3D9_ShaderCode_Base_Initialize( void )
{
	GRAPHICS_HARDWARE_DIRECT3D9_SHADERCODE_BASE *SCBASE = &GraphicsHardDataDirect3D9.ShaderCode.Base ;
	int   Size = 0 ;
	int   Addr = 0 ;
	int   i    = 0 ;
	void  **AddrBuf   = NULL ;
	BYTE  *ShaderAddr = NULL ;
	WORD  *SizeBuf    = NULL ;
	short *SizeDest   = NULL ;

	// すでに初期化されていたら何もしない
	if( SCBASE->BaseShaderInitializeFlag == TRUE )
	{
		return TRUE ;
	}

	// ライトインデックスリストの構築
	{
		int l0, l1, l2, l3, l4, l5, ind ;

		ind = 0 ;
		for( l0 = 0 ; l0 < 4 ; l0 ++ )
		{
			for( l1 = 0 ; l1 <= l0 ; l1 ++ )
			{
				for( l2 = 0 ; l2 <= l1 ; l2 ++ )
				{
					for( l3 = 0 ; l3 <= l2 ; l3 ++ )
					{
						for( l4 = 0 ; l4 <= l3 ; l4 ++ )
						{
							for( l5 = 0 ; l5 <= l4 ; l5 ++ )
							{
								SCBASE->LightIndexList84[ l0 ][ l1 ][ l2 ][ l3 ][ l4 ][ l5 ] = ( short )ind ;
								ind ++ ;
							}
						}
					}
				}
			}
		}

		ind = 0 ;
		for( l0 = 0 ; l0 < 4 ; l0 ++ )
		{
			for( l1 = 0 ; l1 <= l0 ; l1 ++ )
			{
				for( l2 = 0 ; l2 <= l1 ; l2 ++ )
				{
					SCBASE->LightIndexList20[ l0 ][ l1 ][ l2 ] = ( short )ind ;
					ind ++ ;
				}
			}
		}

		ind = 0 ;
		for( l0 = 0 ; l0 < 3 ; l0 ++ )
		{
			for( l1 = 0 ; l1 <= l0 ; l1 ++ )
			{
				for( l2 = 0 ; l2 <= l1 ; l2 ++ )
				{
					SCBASE->LightIndexList10[ l0 ][ l1 ][ l2 ] = ( short )ind ;
					ind ++ ;
				}
			}
		}
	}

#ifndef DX_NON_FILTER
	SCBASE->FilterShaderBinDxaImage = NULL ;
	SCBASE->RGBAMixS_ShaderPackImage = NULL ;
#endif // DX_NON_FILTER

#ifndef DX_NON_NORMAL_DRAW_SHADER
	SCBASE->BaseShaderBinDxaImage = NULL ;

	// 基本シェーダーオブジェクトファイルＤＸＡを圧縮したデータを解凍する
	{
		if( DxShaderCodeBin_Base_D3D9Convert == 0 )
		{
			DxShaderCodeBin_Base_D3D9Convert = 1 ;
			Char128ToBin( DxShaderCodeBin_Base_D3D9, DxShaderCodeBin_Base_D3D9 ) ;
		}
		Size = DXA_Decode( DxShaderCodeBin_Base_D3D9, NULL ) ;
		SCBASE->BaseShaderBinDxaImage = DXALLOC( ( size_t )Size ) ;
		if( SCBASE->BaseShaderBinDxaImage == NULL )
		{
			goto ERR ;
		}

		DXA_Decode( DxShaderCodeBin_Base_D3D9, SCBASE->BaseShaderBinDxaImage ) ;

		// ＤＸＡファイルをオープンする
		DXA_Initialize( &SCBASE->BaseShaderBinDxa ) ;
		if( DXA_OpenArchiveFromMem( &SCBASE->BaseShaderBinDxa, SCBASE->BaseShaderBinDxaImage, Size, FALSE, FALSE ) != 0 )
		{
			goto ERR ;
		}
	}
#else // DX_NON_NORMAL_DRAW_SHADER
	Graphics_Hardware_D3D9_SetUseNormalDrawShader_PF( FALSE ) ;
#endif // DX_NON_NORMAL_DRAW_SHADER

#ifndef DX_NON_FILTER

	// RGBAMixシェーダーパック圧縮ファイルを展開する
	{
		if( DxShaderCodeBin_RgbaMixConvert == 0 )
		{
			DxShaderCodeBin_RgbaMixConvert = 1 ;
			Char128ToBin( DxShaderCodeBin_RgbaMix, DxShaderCodeBin_RgbaMix ) ;
		}
		Size = DXA_Decode( DxShaderCodeBin_RgbaMix, NULL ) ;
		SCBASE->RGBAMixS_ShaderPackImage = DXALLOC( ( size_t )Size ) ;
		if( SCBASE->RGBAMixS_ShaderPackImage == NULL )
		{
			goto ERR ;
		}

		DXA_Decode( DxShaderCodeBin_RgbaMix, SCBASE->RGBAMixS_ShaderPackImage ) ;

		// アドレスリストのセット
		Addr       = 0 ;
		SizeBuf    = ( WORD  * )SCBASE->RGBAMixS_ShaderPackImage ;
		ShaderAddr = ( BYTE  * )SCBASE->RGBAMixS_ShaderPackImage + sizeof( WORD ) * 256 ;
		AddrBuf    = ( void ** )SCBASE->RGBAMixS_ShaderAddress ;
		SizeDest   = ( short * )SCBASE->RGBAMixS_ShaderSize ;
		for( i = 0 ; i < 256 * 2 ; i ++, SizeBuf ++, AddrBuf ++, SizeDest ++ )
		{
			if( *SizeBuf != 0 )
			{
				*AddrBuf    = ( void * )ShaderAddr ;
				ShaderAddr += *SizeBuf ;
			}
			*SizeDest = ( short )*SizeBuf ;
		}
	}

	// フィルターシェーダーオブジェクトファイルＤＸＡを圧縮したデータを解凍する
	{
		if( DxShaderCodeBin_FilterConvert == 0 )
		{
			DxShaderCodeBin_FilterConvert = 1 ;
			Char128ToBin( DxShaderCodeBin_Filter, DxShaderCodeBin_Filter ) ;
		}
		Size = DXA_Decode( DxShaderCodeBin_Filter, NULL ) ;
		SCBASE->FilterShaderBinDxaImage = DXALLOC( ( size_t )Size ) ;
		if( SCBASE->FilterShaderBinDxaImage == NULL )
		{
			goto ERR ;
		}

		DXA_Decode( DxShaderCodeBin_Filter, SCBASE->FilterShaderBinDxaImage ) ;

		// ＤＸＡファイルをオープンする
		DXA_Initialize( &SCBASE->FilterShaderBinDxa ) ;
		if( DXA_OpenArchiveFromMem( &SCBASE->FilterShaderBinDxa, SCBASE->FilterShaderBinDxaImage, Size, FALSE, FALSE ) != 0 )
		{
			goto ERR ;
		}
	}

#endif // DX_NON_FILTER

	SCBASE->BaseShaderInitializeFlag = TRUE ;

	// 正常終了
	return TRUE ;

#ifndef DX_NON_NORMAL_DRAW_SHADER
ERR :
	if( SCBASE->BaseShaderBinDxaImage != NULL )
	{
		DXFREE( SCBASE->BaseShaderBinDxaImage ) ;
		SCBASE->BaseShaderBinDxaImage = NULL ;
	}
#endif // DX_NON_NORMAL_DRAW_SHADER

#ifndef DX_NON_FILTER
	if( SCBASE->FilterShaderBinDxaImage != NULL )
	{
		DXFREE( SCBASE->FilterShaderBinDxaImage ) ;
		SCBASE->FilterShaderBinDxaImage = NULL ;
	}

	if( SCBASE->RGBAMixS_ShaderPackImage != NULL )
	{
		DXFREE( SCBASE->RGBAMixS_ShaderPackImage ) ;
		SCBASE->RGBAMixS_ShaderPackImage = NULL ;
	}
#endif // DX_NON_FILTER

	return FALSE ;
}

// Direct3D9 の標準描画用のシェーダーコードの後始末を行う
extern int Graphics_D3D9_ShaderCode_Base_Terminate( void )
{
	GRAPHICS_HARDWARE_DIRECT3D9_SHADERCODE_BASE *SCBASE = &GraphicsHardDataDirect3D9.ShaderCode.Base ;

	// すでに後始末されていたら何もしない
	if( SCBASE->BaseShaderInitializeFlag == FALSE )
	{
		return TRUE ;
	}

#ifndef DX_NON_NORMAL_DRAW_SHADER
	// 基本シェーダー用ＤＸＡの後始末
	DXA_Terminate( &SCBASE->BaseShaderBinDxa ) ;


	// 解凍したシェーダーを格納していたメモリの解放
	if( SCBASE->BaseShaderBinDxaImage != NULL )
	{
		DXFREE( SCBASE->BaseShaderBinDxaImage ) ;
		SCBASE->BaseShaderBinDxaImage = NULL ;
	}
#endif // DX_NON_NORMAL_DRAW_SHADER

#ifndef DX_NON_FILTER

	// フィルターシェーダー用ＤＸＡの後始末
	DXA_Terminate( &SCBASE->FilterShaderBinDxa ) ;

	if( SCBASE->RGBAMixS_ShaderPackImage )
	{
		DXFREE( SCBASE->RGBAMixS_ShaderPackImage ) ;
		SCBASE->RGBAMixS_ShaderPackImage = NULL ;
	}
	if( SCBASE->FilterShaderBinDxaImage )
	{
		DXFREE( SCBASE->FilterShaderBinDxaImage ) ;
		SCBASE->FilterShaderBinDxaImage = NULL ;
	}

#endif // DX_NON_FILTER

	// 初期化フラグを倒す
	SCBASE->BaseShaderInitializeFlag = FALSE ;

	// 正常終了
	return TRUE ;
}

// Direct3D9 の標準３Ｄ描画用のシェーダーコードの初期化を行う
extern int Graphics_D3D9_ShaderCode_Base3D_Initialize( void )
{
	GRAPHICS_HARDWARE_DIRECT3D9_SHADERCODE_BASE3D *SCBASE3D = &GraphicsHardDataDirect3D9.ShaderCode.Base3D ;
	HANDLEMANAGE *HandleManage = &HandleManageArray[ DX_HANDLETYPE_GRAPH ] ;
	int   Size = 0 ;
	BYTE  *ShaderAddr = NULL ;
	WORD  *SizeBuf    = NULL ;

	// すでに初期化されていたら何もしない
	if( SCBASE3D->Base3DShaderInitializeFlag == TRUE )
	{
		return TRUE ;
	}

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManage->CriticalSection ) ;

	// クリティカルセクションを取得した上で改めて初期化確認
	if( SCBASE3D->Base3DShaderInitializeFlag == TRUE )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManage->CriticalSection ) ;

		return TRUE ;
	}

	SCBASE3D->Base3DShaderPackageImage = NULL ;

	// 圧縮データの解凍
	{
		if( DxShaderCodeBin_Base3D_D3D9Convert == 0 )
		{
			DxShaderCodeBin_Base3D_D3D9Convert = 1 ;
			Char128ToBin( DxShaderCodeBin_Base3D_D3D9, DxShaderCodeBin_Base3D_D3D9 ) ;
		}
		Size = DXA_Decode( DxShaderCodeBin_Base3D_D3D9, NULL ) ;
		SCBASE3D->Base3DShaderPackageImage = DXCALLOC( ( size_t )Size ) ;
		if( SCBASE3D->Base3DShaderPackageImage == NULL )
		{
			goto ERR ;
		}

		DXA_Decode( DxShaderCodeBin_Base3D_D3D9, SCBASE3D->Base3DShaderPackageImage ) ;
	}

	// アドレスリストのセット
	SizeBuf    = ( WORD * )SCBASE3D->Base3DShaderPackageImage ;
	ShaderAddr = ( BYTE * )SCBASE3D->Base3DShaderPackageImage +
		sizeof( WORD ) * (
			sizeof( SCBASE3D->Base3D_PixelLighting_VSAddress         ) / sizeof( void * ) +
			sizeof( SCBASE3D->Base3D_PixelLighting_Normal_PSAddress  ) / sizeof( void * ) +
			sizeof( SCBASE3D->Base3D_ShadowMap_VSAddress             ) / sizeof( void * ) +
			sizeof( SCBASE3D->Base3D_NoLighting_VSAddress            ) / sizeof( void * ) +
			sizeof( SCBASE3D->Base3D_VertexLighting_VSAddress        ) / sizeof( void * ) +
			sizeof( SCBASE3D->Base3D_ShadowMap_Normal_PSAddress      ) / sizeof( void * ) +
			sizeof( SCBASE3D->Base3D_NoLighting_Normal_PSAddress     ) / sizeof( void * ) +
			sizeof( SCBASE3D->Base3D_VertexLighting_Normal_PSAddress ) / sizeof( void * ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( void ** )SCBASE3D->Base3D_PixelLighting_VSAddress,         sizeof( SCBASE3D->Base3D_PixelLighting_VSAddress         ) / sizeof( void * ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( void ** )SCBASE3D->Base3D_PixelLighting_Normal_PSAddress,  sizeof( SCBASE3D->Base3D_PixelLighting_Normal_PSAddress  ) / sizeof( void * ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( void ** )SCBASE3D->Base3D_ShadowMap_VSAddress,             sizeof( SCBASE3D->Base3D_ShadowMap_VSAddress             ) / sizeof( void * ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( void ** )SCBASE3D->Base3D_NoLighting_VSAddress,            sizeof( SCBASE3D->Base3D_NoLighting_VSAddress            ) / sizeof( void * ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( void ** )SCBASE3D->Base3D_VertexLighting_VSAddress,        sizeof( SCBASE3D->Base3D_VertexLighting_VSAddress        ) / sizeof( void * ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( void ** )&SCBASE3D->Base3D_ShadowMap_Normal_PSAddress,     sizeof( SCBASE3D->Base3D_ShadowMap_Normal_PSAddress      ) / sizeof( void * ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( void ** )SCBASE3D->Base3D_NoLighting_Normal_PSAddress,     sizeof( SCBASE3D->Base3D_NoLighting_Normal_PSAddress     ) / sizeof( void * ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( void ** )SCBASE3D->Base3D_VertexLighting_Normal_PSAddress, sizeof( SCBASE3D->Base3D_VertexLighting_Normal_PSAddress ) / sizeof( void * ) ) ;

	SCBASE3D->Base3DShaderInitializeFlag = TRUE ;

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManage->CriticalSection ) ;

	// 正常終了
	return TRUE ;

ERR :
	if( SCBASE3D->Base3DShaderPackageImage != NULL )
	{
		DXFREE( SCBASE3D->Base3DShaderPackageImage ) ;
		SCBASE3D->Base3DShaderPackageImage = NULL ;
	}

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManage->CriticalSection ) ;

	return FALSE ;
}


// Direct3D9 の標準３Ｄ描画用のシェーダーコードの後始末を行う
extern int Graphics_D3D9_ShaderCode_Base3D_Terminate( void )
{
	GRAPHICS_HARDWARE_DIRECT3D9_SHADERCODE_BASE3D *SCBASE3D = &GraphicsHardDataDirect3D9.ShaderCode.Base3D ;

	// すでに後始末されていたら何もしない
	if( SCBASE3D->Base3DShaderInitializeFlag == FALSE )
	{
		return TRUE ;
	}

	if( SCBASE3D->Base3DShaderPackageImage )
	{
		DXFREE( SCBASE3D->Base3DShaderPackageImage ) ;
		SCBASE3D->Base3DShaderPackageImage = NULL ;
	}

	// 初期化フラグを倒す
	SCBASE3D->Base3DShaderInitializeFlag = FALSE ;

	// 正常終了
	return TRUE ;
}







#ifndef DX_NON_MODEL

// Direct3D9 のモデル描画用のシェーダーコードの初期化を行う
extern int Graphics_D3D9_ShaderCode_Model_Initialize( void )
{
	GRAPHICS_HARDWARE_DIRECT3D9_SHADERCODE_MODEL *SCMD = &GraphicsHardDataDirect3D9.ShaderCode.Model ;
	HANDLEMANAGE *HandleManage = &HandleManageArray[ DX_HANDLETYPE_MODEL_BASE ] ;
	int  Size = 0 ;
	int  Addr = 0 ;
	BYTE *ShaderAddr = NULL ;
	WORD *SizeBuf    = NULL ;

	// すでに初期化されていたら何もしない
	if( SCMD->ModelShaderInitializeFlag == TRUE )
	{
		return TRUE ;
	}

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManage->CriticalSection ) ;

	// クリティカルセクションを取得した上で改めて初期化確認
	if( SCMD->ModelShaderInitializeFlag == TRUE )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManage->CriticalSection ) ;

		return TRUE ;
	}

	SCMD->ModelShaderPackImage = NULL ;

	if( DxShaderCodeBin_ModelConvert == 0 )
	{
		DxShaderCodeBin_ModelConvert = 1 ;
		Char128ToBin( DxShaderCodeBin_Model, DxShaderCodeBin_Model ) ;
	}
	Size = DXA_Decode( DxShaderCodeBin_Model, NULL ) ;
	SCMD->ModelShaderPackImage = DXCALLOC( ( size_t )Size ) ;
	if( SCMD->ModelShaderPackImage == NULL )
	{
		goto ERR ;
	}

	DXA_Decode( DxShaderCodeBin_Model, SCMD->ModelShaderPackImage ) ;

	// アドレスリストのセット
	Addr = 0 ;
	SizeBuf = ( WORD * )SCMD->ModelShaderPackImage ;
	ShaderAddr = ( BYTE * )SCMD->ModelShaderPackImage +
		sizeof( WORD ) * (
			sizeof( SCMD->MV1_PixelLighting_VSAddress          ) / sizeof( void * ) +
			sizeof( SCMD->MV1_PixelLighting_Toon_PSAddress     ) / sizeof( void * ) +
			sizeof( SCMD->MV1_PixelLighting_Normal_PSAddress   ) / sizeof( void * ) +

			sizeof( SCMD->MV1_MaterialType_PSAddress           ) / sizeof( void * ) +

			sizeof( SCMD->MV1_ToonOutLine_VSAddress            ) / sizeof( void * ) +
			sizeof( SCMD->MV1_ShadowMap_VSAddress              ) / sizeof( void * ) +
			sizeof( SCMD->MV1_NoLighting_VSAddress             ) / sizeof( void * ) +
			sizeof( SCMD->MV1_VertexLighting_VSAddress         ) / sizeof( void * ) +
			sizeof( SCMD->MV1_ToonOutLine_PSAddress            ) / sizeof( void * ) +
			sizeof( SCMD->MV1_ShadowMap_Toon_PSAddress         ) / sizeof( void * ) +
			sizeof( SCMD->MV1_ShadowMap_Normal_PSAddress       ) / sizeof( void * ) +
			sizeof( SCMD->MV1_NoLighting_Toon_PSAddress        ) / sizeof( void * ) +
			sizeof( SCMD->MV1_NoLighting_Normal_PSAddress      ) / sizeof( void * ) +
			sizeof( SCMD->MV1_VertexLighting_Toon_PSAddress    ) / sizeof( void * ) +
			sizeof( SCMD->MV1_VertexLighting_Normal_PSAddress  ) / sizeof( void * ) ) ;

	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( void ** )SCMD->MV1_PixelLighting_VSAddress,			sizeof( SCMD->MV1_PixelLighting_VSAddress			) / sizeof( void * ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( void ** )SCMD->MV1_PixelLighting_Toon_PSAddress,		sizeof( SCMD->MV1_PixelLighting_Toon_PSAddress		) / sizeof( void * ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( void ** )SCMD->MV1_PixelLighting_Normal_PSAddress,	sizeof( SCMD->MV1_PixelLighting_Normal_PSAddress	) / sizeof( void * ) ) ;

	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( void ** )SCMD->MV1_MaterialType_PSAddress,			sizeof( SCMD->MV1_MaterialType_PSAddress			) / sizeof( void * ) ) ;

	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( void ** )SCMD->MV1_ToonOutLine_VSAddress,			sizeof( SCMD->MV1_ToonOutLine_VSAddress				) / sizeof( void * ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( void ** )SCMD->MV1_ShadowMap_VSAddress,				sizeof( SCMD->MV1_ShadowMap_VSAddress				) / sizeof( void * ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( void ** )SCMD->MV1_NoLighting_VSAddress,				sizeof( SCMD->MV1_NoLighting_VSAddress				) / sizeof( void * ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( void ** )SCMD->MV1_VertexLighting_VSAddress,			sizeof( SCMD->MV1_VertexLighting_VSAddress			) / sizeof( void * ) ) ;

	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( void ** )&SCMD->MV1_ToonOutLine_PSAddress,			sizeof( SCMD->MV1_ToonOutLine_PSAddress				) / sizeof( void * ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( void ** )SCMD->MV1_ShadowMap_Toon_PSAddress,			sizeof( SCMD->MV1_ShadowMap_Toon_PSAddress			) / sizeof( void * ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( void ** )&SCMD->MV1_ShadowMap_Normal_PSAddress,		sizeof( SCMD->MV1_ShadowMap_Normal_PSAddress		) / sizeof( void * ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( void ** )SCMD->MV1_NoLighting_Toon_PSAddress,		sizeof( SCMD->MV1_NoLighting_Toon_PSAddress			) / sizeof( void * ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( void ** )SCMD->MV1_NoLighting_Normal_PSAddress,		sizeof( SCMD->MV1_NoLighting_Normal_PSAddress		) / sizeof( void * ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( void ** )SCMD->MV1_VertexLighting_Toon_PSAddress,	sizeof( SCMD->MV1_VertexLighting_Toon_PSAddress		) / sizeof( void * ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( void ** )SCMD->MV1_VertexLighting_Normal_PSAddress,	sizeof( SCMD->MV1_VertexLighting_Normal_PSAddress	) / sizeof( void * ) ) ;

	SCMD->ModelShaderInitializeFlag = TRUE ;

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManage->CriticalSection ) ;

	// 正常終了
	return TRUE ;

ERR :
	if( SCMD->ModelShaderPackImage != NULL )
	{
		DXFREE( SCMD->ModelShaderPackImage ) ;
		SCMD->ModelShaderPackImage = NULL ;
	}

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManage->CriticalSection ) ;

	return FALSE ;
}

// Direct3D9 のモデル描画用のシェーダーコードの後始末を行う
extern int Graphics_D3D9_ShaderCode_Model_Terminate( void )
{
	GRAPHICS_HARDWARE_DIRECT3D9_SHADERCODE_MODEL *SCMD = &GraphicsHardDataDirect3D9.ShaderCode.Model ;

	// すでに後始末されていたら何もしない
	if( SCMD->ModelShaderInitializeFlag == FALSE )
	{
		return TRUE ;
	}

	if( SCMD->ModelShaderPackImage )
	{
		DXFREE( SCMD->ModelShaderPackImage ) ;
		SCMD->ModelShaderPackImage = NULL ;
	}

	// 初期化フラグを倒す
	SCMD->ModelShaderInitializeFlag = FALSE ;

	// 正常終了
	return TRUE ;
}

#endif // DX_NON_MODEL








// Direct3D9 のシェーダーの初期化を行う
extern int Graphics_D3D9_Shader_Initialize( void )
{
	GRAPHICS_HARDWARE_DIRECT3D9_SHADERCODE *ShaderCode = &GraphicsHardDataDirect3D9.ShaderCode ;
	GRAPHICS_HARDWARE_DIRECT3D9_SHADER     *Shader     = &GraphicsHardDataDirect3D9.Device.Shader ;

	// 既に作成されていたときのために削除処理を行う
	//TerminateDirect3D9Shader() ;
	Graphics_D3D9_Shader_Terminate() ;

//	return 0 ;

	// シェーダーが使用できるか調べる
	GSYS.HardInfo.UseShader = FALSE ;
	if( ( Shader->ValidVertexShader_SM3 && Shader->ValidPixelShader_SM3 ) ||
		( Shader->ValidVertexShader && Shader->ValidPixelShader ) )
	{
		GSYS.HardInfo.UseShader = TRUE ;

		// シェーダーコードが展開されていない場合は展開
		if( Graphics_D3D9_ShaderCode_Base_Initialize() == FALSE )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\xb3\x30\xfc\x30\xc9\x30\x92\x30\x55\x5c\x8b\x95\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"シェーダーコードを展開するメモリ領域の確保に失敗しました\n" @*/ )) ;
			GSYS.HardInfo.UseShader = FALSE ;
			goto ENDLABEL ;
		}

#ifndef DX_NON_NORMAL_DRAW_SHADER
		// 基本的な描画処理に使用するピクセルシェーダーを作成する
		{
			int  i ;
			int  j ;
			int  k ;
			int  l ;
			int  m ;
			char FileName[ 64 ] ;
			int  Addr ;
			int  Size ;
			void *DataImage ;

			DataImage = DXA_GetFileImage( &ShaderCode->Base.BaseShaderBinDxa ) ;

			// ２Ｄ処理用頂点シェーダーの作成
			if( DXA_GetFileInfo( &ShaderCode->Base.BaseShaderBinDxa, DX_CHARCODEFORMAT_ASCII, "Base2DVertexShader.vso", &Addr, &Size ) == 0 )
			{
				if( Direct3DDevice9_CreateVertexShader( 
					( DWORD * )( ( BYTE * )DataImage + Addr ),
					&Shader->Base.Base2DVertexShader ) != D_D3D_OK )
				{
					DXST_LOGFILEFMT_ADDUTF16LE(( "\x12\xff\x24\xff\xcf\x63\x3b\x75\x28\x75\xfa\x57\x2c\x67\x02\x98\xb9\x70\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"２Ｄ描画用基本頂点シェーダーオブジェクトの作成に失敗しました\n" @*/ )) ;
					GSYS.HardInfo.UseShader = FALSE ;
					goto ENDLABEL ;
				}
			}

			// ３Ｄ処理用頂点シェーダーの作成
//			if( DXA_GetFileInfo( &ShaderCode->Base.BaseShaderBinDxa, "Base3DVertexShader.vso", &Addr, &Size ) == 0 )
//			{
//				if( Direct3DDevice9_CreateVertexShader( 
//					( DWORD * )( ( BYTE * )DataImage + Addr ),
//					&Shader->Base.Base3DVertexShader ) != D_D3D_OK )
//				{
//					DXST_LOGFILEFMT_ADDUTF16LE(( L"３Ｄ描画用基本頂点シェーダーオブジェクトの作成に失敗しました\n" )) ;
//					GSYS.HardInfo.UseShader = FALSE ;
//					goto ENDLABEL ;
//				}
//			}

			// テクスチャなしシェーダーの作成
			_STRCPY_S( FileName, sizeof( FileName ), "ps_nonetex_type0_ach0.pso" ) ;
			for( i = 0 ; i < 7 ; i ++ )
			{
				FileName[ 15 ] = ( char )( '0' + i ) ;
				for( j = 0 ; j < 2 ; j ++ )
				{
					FileName[ 20 ] = ( char )( '0' + j ) ;

					if( DXA_GetFileInfo( &ShaderCode->Base.BaseShaderBinDxa, DX_CHARCODEFORMAT_ASCII, FileName, &Addr, &Size ) == 0 )
					{
						if( Direct3DDevice9_CreatePixelShader( 
							( DWORD * )( ( BYTE * )DataImage + Addr ),
							&Shader->Base.BaseNoneTexPixelShader[ i ][ j ] ) != D_D3D_OK )
						{
							DXST_LOGFILEFMT_ADDUTF16LE(( "\xfa\x57\x2c\x67\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"基本シェーダーオブジェクトの作成に失敗しました\n" @*/ )) ;
							GSYS.HardInfo.UseShader = FALSE ;
							goto ENDLABEL ;
						}
					}
				}
			}

			// テクスチャありシェーダーの作成
			_STRCPY_S( FileName, sizeof( FileName ), "ps_blend0_type0_igcolor0_igtalpha0_ach0.pso" ) ;
			for( i = 0 ; i < 4 ; i ++ )
			{
				FileName[ 8 ] = ( char )( '0' + i ) ;
				for( j = 0 ; j < 7 ; j ++ )
				{
					FileName[ 14 ] = ( char )( '0' + j ) ;
					for( k = 0 ; k < 2 ; k ++ )
					{
						FileName[ 23 ] =( char )(  '0' + k ) ;
						for( l = 0 ; l < 2 ; l ++ )
						{
							FileName[ 33 ] = ( char )( '0' + l ) ;
							for( m = 0 ; m < 2 ; m ++ )
							{
								FileName[ 38 ] = ( char )( '0' + m ) ;

								if( DXA_GetFileInfo( &ShaderCode->Base.BaseShaderBinDxa, DX_CHARCODEFORMAT_ASCII, FileName, &Addr, &Size ) == 0 )
								{
									if( Direct3DDevice9_CreatePixelShader(
										( DWORD * )( ( BYTE * )DataImage + Addr ),
										&Shader->Base.BaseUseTexPixelShader[ i ][ j ][ k ][ l ][ m ] ) != D_D3D_OK )
									{
										DXST_LOGFILEFMT_ADDUTF16LE(( "\xfa\x57\x2c\x67\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"基本シェーダーオブジェクトの作成に失敗しました\n" @*/ )) ;
										GSYS.HardInfo.UseShader = FALSE ;
										goto ENDLABEL ;
									}
								}
							}
						}
					}
				}
			}

			// マスク処理用シェーダーの作成
			DXA_GetFileInfo( &ShaderCode->Base.BaseShaderBinDxa, DX_CHARCODEFORMAT_ASCII, "ps_mask_blend.pso", &Addr, &Size ) ;
			if( Direct3DDevice9_CreatePixelShader(
				( DWORD * )( ( BYTE * )DataImage + Addr ),
				&Shader->Base.MaskEffectPixelShader ) != D_D3D_OK )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\xde\x30\xb9\x30\xaf\x30\xe6\x51\x06\x74\x28\x75\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"マスク処理用シェーダーオブジェクトの作成に失敗しました\n" @*/ )) ;
				GSYS.HardInfo.UseShader = FALSE ;
				goto ENDLABEL ;
			}
			{
				static const char *mask_use_grhandle_filename[ 4 ] =
				{
					"ps_mask_blend_use_grhandle_cha.pso",
					"ps_mask_blend_use_grhandle_chr.pso",
					"ps_mask_blend_use_grhandle_chg.pso",
					"ps_mask_blend_use_grhandle_chb.pso",
				};
				static const char *mask_use_grhandle_reverse_filename[ 4 ] =
				{
					"ps_mask_blend_use_grhandle_cha_reverse.pso",
					"ps_mask_blend_use_grhandle_chr_reverse.pso",
					"ps_mask_blend_use_grhandle_chg_reverse.pso",
					"ps_mask_blend_use_grhandle_chb_reverse.pso",
				};

				for( i = 0 ; i < 4 ; i ++ )
				{
					DXA_GetFileInfo( &ShaderCode->Base.BaseShaderBinDxa, DX_CHARCODEFORMAT_ASCII, mask_use_grhandle_filename[ i ], &Addr, &Size ) ;
					if( Direct3DDevice9_CreatePixelShader(
						( DWORD * )( ( BYTE * )DataImage + Addr ),
						&Shader->Base.MaskEffect_UseGraphHandle_PixelShader[ i ] ) != D_D3D_OK )
					{
						DXST_LOGFILEFMT_ADDUTF16LE(( "\xde\x30\xb9\x30\xaf\x30\xe6\x51\x06\x74\x28\x75\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"マスク処理用シェーダーオブジェクトの作成に失敗しました\n" @*/ )) ;
						GSYS.HardInfo.UseShader = FALSE ;
						goto ENDLABEL ;
					}

					DXA_GetFileInfo( &ShaderCode->Base.BaseShaderBinDxa, DX_CHARCODEFORMAT_ASCII, mask_use_grhandle_reverse_filename[ i ], &Addr, &Size ) ;
					if( Direct3DDevice9_CreatePixelShader(
						( DWORD * )( ( BYTE * )DataImage + Addr ),
						&Shader->Base.MaskEffect_UseGraphHandle_ReverseEffect_PixelShader[ i ] ) != D_D3D_OK )
					{
						DXST_LOGFILEFMT_ADDUTF16LE(( "\xde\x30\xb9\x30\xaf\x30\xe6\x51\x06\x74\x28\x75\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"マスク処理用シェーダーオブジェクトの作成に失敗しました\n" @*/ )) ;
						GSYS.HardInfo.UseShader = FALSE ;
						goto ENDLABEL ;
					}
				}
			}
		}
#endif // DX_NON_NORMAL_DRAW_SHADER
	}

ENDLABEL:

	if( GSYS.HardInfo.UseShader == FALSE )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\xd7\x30\xed\x30\xb0\x30\xe9\x30\xde\x30\xd6\x30\xeb\x30\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\x92\x30\x7f\x4f\x28\x75\x57\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"プログラマブルシェーダーを使用しません\n" @*/ )) ;
	}
	else
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\xd7\x30\xed\x30\xb0\x30\xe9\x30\xde\x30\xd6\x30\xeb\x30\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\x92\x30\x7f\x4f\x28\x75\x57\x30\x7e\x30\x59\x30\x0a\x00\x00"/*@ L"プログラマブルシェーダーを使用します\n" @*/ )) ;
	}

	// シェーダーの定数をセット
	static float ZeroOne[ 4 ]        = { 0.0f, 1.0f, 0.0f, 0.0f } ;
	static float ZeroHalfOneTwo[ 4 ] = { 0.0f, 0.5f, 1.0f, 2.0f } ;
	Graphics_D3D9_ShaderConstant_InfoSet_SetParam( &Shader->ShaderConstantInfo, DX_SHADERCONSTANTTYPE_VS_FLOAT, DX_SHADERCONSTANTSET_LIB, DX_VS_CONSTF_ZERO_ONE,          ZeroOne,        1, TRUE ) ;
	Graphics_D3D9_ShaderConstant_InfoSet_SetParam( &Shader->ShaderConstantInfo, DX_SHADERCONSTANTTYPE_PS_FLOAT, DX_SHADERCONSTANTSET_LIB, DX_PS_CONSTF_ZERO_HALF_ONE_TWO, ZeroHalfOneTwo, 1, TRUE ) ;

	// 正常終了
	return TRUE ;
}

// 頂点シェーダー配列を解放する
static void Graphics_D3D9_VertexShaderArray_Release( D_IDirect3DVertexShader9 **pObject, int Num )
{
	int i ;

	for( i = 0 ; i < Num ; i++, pObject++ )
	{
		if( *pObject )
		{
			Direct3D9_ObjectRelease( *pObject ) ;
			*pObject = NULL ;
		}
	}
}

// ピクセルシェーダー配列を解放する
static void Graphics_D3D9_PixelShaderArray_Release( D_IDirect3DPixelShader9 **pObject, int Num )
{
	int i ;

	for( i = 0 ; i < Num ; i++, pObject++ )
	{
		if( *pObject )
		{
			Direct3D9_ObjectRelease( *pObject ) ;
			*pObject = NULL ;
		}
	}
}

// Direct3D9 のシェーダーの後始末をする
extern int Graphics_D3D9_Shader_Terminate( void )
{
	GRAPHICS_HARDDATA_DIRECT3D9_DEVICE *Device = &GraphicsHardDataDirect3D9.Device ;
	GRAPHICS_HARDWARE_DIRECT3D9_SHADER *Shader = &Device->Shader ;
	int                                i = 0 ;

	if( GAPIWin.Direct3DDevice9Object == NULL )
	{
		return 0 ;
	}

	Graphics_D3D9_DeviceState_ResetVertexShader() ;
	Graphics_D3D9_DeviceState_ResetPixelShader() ;

#ifndef DX_NON_NORMAL_DRAW_SHADER

	if( Shader->Base.Base2DVertexShader )
	{
		Direct3D9_ObjectRelease( Shader->Base.Base2DVertexShader ) ;
		Shader->Base.Base2DVertexShader = NULL ;
	}

//	if( Shader->Base.Base3DVertexShader )
//	{
//		Direct3D9_ObjectRelease( Shader->Base.Base3DVertexShader ) ;
//		Shader->Base.Base3DVertexShader = NULL ;
//	}

	Graphics_D3D9_PixelShaderArray_Release( ( D_IDirect3DPixelShader9 ** )Shader->Base.BaseNoneTexPixelShader, sizeof( Shader->Base.BaseNoneTexPixelShader ) / sizeof( D_IDirect3DPixelShader9 * ) ) ;
	Graphics_D3D9_PixelShaderArray_Release( ( D_IDirect3DPixelShader9 ** )Shader->Base.BaseUseTexPixelShader,  sizeof( Shader->Base.BaseUseTexPixelShader  ) / sizeof( D_IDirect3DPixelShader9 * ) ) ;

	if( Shader->Base.MaskEffectPixelShader )
	{
		Direct3D9_ObjectRelease( Shader->Base.MaskEffectPixelShader ) ;
		Shader->Base.MaskEffectPixelShader = NULL ;
	}

	for( i = 0 ; i < 4 ; i ++ )
	{
		if( Shader->Base.MaskEffect_UseGraphHandle_PixelShader[ i ] )
		{
			Direct3D9_ObjectRelease( Shader->Base.MaskEffect_UseGraphHandle_PixelShader[ i ] ) ;
			Shader->Base.MaskEffect_UseGraphHandle_PixelShader[ i ] = NULL ;
		}

		if( Shader->Base.MaskEffect_UseGraphHandle_ReverseEffect_PixelShader[ i ] )
		{
			Direct3D9_ObjectRelease( Shader->Base.MaskEffect_UseGraphHandle_ReverseEffect_PixelShader[ i ] ) ;
			Shader->Base.MaskEffect_UseGraphHandle_ReverseEffect_PixelShader[ i ] = NULL ;
		}
	}

	Graphics_D3D9_VertexShaderArray_Release( ( D_IDirect3DVertexShader9 ** )Shader->Base3D.Base3D_PixelLighting_VS,         sizeof( Shader->Base3D.Base3D_PixelLighting_VS         ) / sizeof( D_IDirect3DVertexShader9 *	) ) ;
	Graphics_D3D9_PixelShaderArray_Release(  ( D_IDirect3DPixelShader9  ** )Shader->Base3D.Base3D_PixelLighting_Normal_PS,  sizeof( Shader->Base3D.Base3D_PixelLighting_Normal_PS  ) / sizeof( D_IDirect3DPixelShader9 *	) ) ;
	Graphics_D3D9_VertexShaderArray_Release( ( D_IDirect3DVertexShader9 ** )Shader->Base3D.Base3D_ShadowMap_VS,             sizeof( Shader->Base3D.Base3D_ShadowMap_VS             ) / sizeof( D_IDirect3DVertexShader9 *	) ) ;
	Graphics_D3D9_VertexShaderArray_Release( ( D_IDirect3DVertexShader9 ** )Shader->Base3D.Base3D_NoLighting_VS,            sizeof( Shader->Base3D.Base3D_NoLighting_VS            ) / sizeof( D_IDirect3DVertexShader9 *	) ) ;
	Graphics_D3D9_VertexShaderArray_Release( ( D_IDirect3DVertexShader9 ** )Shader->Base3D.Base3D_VertexLighting_VS,        sizeof( Shader->Base3D.Base3D_VertexLighting_VS        ) / sizeof( D_IDirect3DVertexShader9 *	) ) ;
	Graphics_D3D9_PixelShaderArray_Release(  ( D_IDirect3DPixelShader9  ** )&Shader->Base3D.Base3D_ShadowMap_Normal_PS,     sizeof( Shader->Base3D.Base3D_ShadowMap_Normal_PS      ) / sizeof( D_IDirect3DPixelShader9 *	) ) ;
	Graphics_D3D9_PixelShaderArray_Release(  ( D_IDirect3DPixelShader9  ** )Shader->Base3D.Base3D_NoLighting_Normal_PS,     sizeof( Shader->Base3D.Base3D_NoLighting_Normal_PS     ) / sizeof( D_IDirect3DPixelShader9 *	) ) ;
	Graphics_D3D9_PixelShaderArray_Release(  ( D_IDirect3DPixelShader9  ** )Shader->Base3D.Base3D_VertexLighting_Normal_PS, sizeof( Shader->Base3D.Base3D_VertexLighting_Normal_PS ) / sizeof( D_IDirect3DPixelShader9 *	) ) ;

#endif // DX_NON_NORMAL_DRAW_SHADER

#ifndef DX_NON_MODEL

	Graphics_D3D9_VertexShaderArray_Release( ( D_IDirect3DVertexShader9 ** )Shader->Model.MV1_PixelLighting_VS,			sizeof( Shader->Model.MV1_PixelLighting_VS			) / sizeof( D_IDirect3DVertexShader9 *	) ) ;
	Graphics_D3D9_PixelShaderArray_Release(  ( D_IDirect3DPixelShader9  ** )Shader->Model.MV1_PixelLighting_Toon_PS,	sizeof( Shader->Model.MV1_PixelLighting_Toon_PS		) / sizeof( D_IDirect3DPixelShader9 *	) ) ;
	Graphics_D3D9_PixelShaderArray_Release(  ( D_IDirect3DPixelShader9  ** )Shader->Model.MV1_PixelLighting_Normal_PS,	sizeof( Shader->Model.MV1_PixelLighting_Normal_PS	) / sizeof( D_IDirect3DPixelShader9 *	) ) ;
	Graphics_D3D9_PixelShaderArray_Release(  ( D_IDirect3DPixelShader9  ** )Shader->Model.MV1_MaterialType_PS,			sizeof( Shader->Model.MV1_MaterialType_PS			) / sizeof( D_IDirect3DPixelShader9 *	) ) ;
	Graphics_D3D9_VertexShaderArray_Release( ( D_IDirect3DVertexShader9 ** )Shader->Model.MV1_ToonOutLine_VS,			sizeof( Shader->Model.MV1_ToonOutLine_VS			) / sizeof( D_IDirect3DVertexShader9 *	) ) ;
	Graphics_D3D9_VertexShaderArray_Release( ( D_IDirect3DVertexShader9 ** )Shader->Model.MV1_ShadowMap_VS,				sizeof( Shader->Model.MV1_ShadowMap_VS				) / sizeof( D_IDirect3DVertexShader9 *	) ) ;
	Graphics_D3D9_VertexShaderArray_Release( ( D_IDirect3DVertexShader9 ** )Shader->Model.MV1_NoLighting_VS,			sizeof( Shader->Model.MV1_NoLighting_VS				) / sizeof( D_IDirect3DVertexShader9 *	) ) ;
	Graphics_D3D9_VertexShaderArray_Release( ( D_IDirect3DVertexShader9 ** )Shader->Model.MV1_VertexLighting_VS,		sizeof( Shader->Model.MV1_VertexLighting_VS			) / sizeof( D_IDirect3DVertexShader9 *	) ) ;
	Graphics_D3D9_PixelShaderArray_Release(  ( D_IDirect3DPixelShader9  ** )&Shader->Model.MV1_ToonOutLine_PS,			sizeof( Shader->Model.MV1_ToonOutLine_PS			) / sizeof( D_IDirect3DPixelShader9 *	) ) ;
	Graphics_D3D9_PixelShaderArray_Release(  ( D_IDirect3DPixelShader9  ** )Shader->Model.MV1_ShadowMap_Toon_PS,		sizeof( Shader->Model.MV1_ShadowMap_Toon_PS			) / sizeof( D_IDirect3DPixelShader9 *	) ) ;
	Graphics_D3D9_PixelShaderArray_Release(  ( D_IDirect3DPixelShader9  ** )&Shader->Model.MV1_ShadowMap_Normal_PS,		sizeof( Shader->Model.MV1_ShadowMap_Normal_PS		) / sizeof( D_IDirect3DPixelShader9 *	) ) ;
	Graphics_D3D9_PixelShaderArray_Release(  ( D_IDirect3DPixelShader9  ** )Shader->Model.MV1_NoLighting_Toon_PS,		sizeof( Shader->Model.MV1_NoLighting_Toon_PS		) / sizeof( D_IDirect3DPixelShader9 *	) ) ;
	Graphics_D3D9_PixelShaderArray_Release(  ( D_IDirect3DPixelShader9  ** )Shader->Model.MV1_NoLighting_Normal_PS,		sizeof( Shader->Model.MV1_NoLighting_Normal_PS		) / sizeof( D_IDirect3DPixelShader9 *	) ) ;
	Graphics_D3D9_PixelShaderArray_Release(  ( D_IDirect3DPixelShader9  ** )Shader->Model.MV1_VertexLighting_Toon_PS,	sizeof( Shader->Model.MV1_VertexLighting_Toon_PS	) / sizeof( D_IDirect3DPixelShader9 *	) ) ;
	Graphics_D3D9_PixelShaderArray_Release(  ( D_IDirect3DPixelShader9  ** )Shader->Model.MV1_VertexLighting_Normal_PS,	sizeof( Shader->Model.MV1_VertexLighting_Normal_PS	) / sizeof( D_IDirect3DPixelShader9 *	) ) ;

#endif // DX_NON_MODEL

	// 正常終了
	return 0 ;
}

// ３Ｄ標準描画の指定の頂点用の描画用シェーダーをセットアップする
extern int Graphics_D3D9_Shader_Normal3DDraw_Setup( void )
{
	GRAPHICS_HARDWARE_DIRECT3D9_SHADER				*Shader = &GD3D9.Device.Shader ;
	GRAPHICS_HARDWARE_DIRECT3D9_SHADER_BASE3D		*SB3D  = &GD3D9.Device.Shader.Base3D ;
	GRAPHICS_HARDWARE_DIRECT3D9_SHADERCODE_BASE3D	*SCB3D = &GD3D9.ShaderCode.Base3D ;
	int												ValidPL ;
	D_IDirect3DVertexShader9						**VS_PL			= NULL ;
	void											**VSAddress_PL	= NULL ;
	D_IDirect3DPixelShader9							**PS_PL			= NULL ;
	void											**PSAddress_PL	= NULL ;
	D_IDirect3DVertexShader9						**VS			= NULL ;
	void											**VSAddress		= NULL ;
	D_IDirect3DPixelShader9							**PS			= NULL ;
	void											**PSAddress		= NULL ;
	int												IgnoreTextureAlpha ;
	int												ShaderModel ;
	int												ShadowMap ;
	int												FogType ;
	int												LightIndex84 = 0 ;
	int												LightIndex20 = 0 ;
	int												LightIndex10 = 0 ;
	int												BumpMap ;
	int												Specular ;
	int												RenderBlendType ;
	int												NextBlendMode ;
	float											IgnoreColor[ 4 ] ;

	if( GD3D9.ShaderCode.Base3D.Base3DShaderInitializeFlag == FALSE &&
		Graphics_D3D9_ShaderCode_Base3D_Initialize() == FALSE )
	{
		return FALSE ;
	}

	// テクスチャアルファを無視するかどうかをセット
	IgnoreTextureAlpha = 1 ;
	if( GD3D9.Device.DrawSetting.AlphaChannelValidFlag ||
		GD3D9.Device.DrawSetting.AlphaTestValidFlag ||
		GD3D9.Device.State.ZEnable )
	{
		IgnoreTextureAlpha = 0 ;
	}

	// DX_PS_CONSTF_IGNORE_TEX_COLOR の更新
	{
		if( GD3D9.Device.DrawSetting.IgnoreGraphColorFlag )
		{
			IgnoreColor[ 0 ] = 1.0f ;
			IgnoreColor[ 1 ] = 1.0f ;
			IgnoreColor[ 2 ] = 1.0f ;
		}
		else
		{
			IgnoreColor[ 0 ] = 0.0f ;
			IgnoreColor[ 1 ] = 0.0f ;
			IgnoreColor[ 2 ] = 0.0f ;
		}

		if( IgnoreTextureAlpha == 1 )
		{
			IgnoreColor[ 3 ] = 1.0f ;
		}
		else
		{
			IgnoreColor[ 3 ] = 0.0f ;
		}

		if( _MEMCMP( GD3D9.Device.State.IgnoreColorPSConstantF, IgnoreColor, sizeof( float ) * 4 ) != 0 )
		{
			_MEMCPY( GD3D9.Device.State.IgnoreColorPSConstantF, IgnoreColor, sizeof( float ) * 4 ) ;
			Graphics_D3D9_ShaderConstant_InfoSet_SetParam(
				&GD3D9.Device.Shader.ShaderConstantInfo,
				DX_SHADERCONSTANTTYPE_PS_FLOAT,
				DX_SHADERCONSTANTSET_LIB,
				DX_PS_CONSTF_IGNORE_TEX_COLOR,
				IgnoreColor,
				1,
				TRUE
			) ;
		}
	}

	// ブレンドモードの決定
	{
		NextBlendMode = GD3D9.Device.DrawSetting.BlendMode ;
		switch( GD3D9.Device.DrawSetting.BlendMode )
		{
		case DX_BLENDMODE_SUB :
			// 減算ブレンドの場合は14番目のブレンドモードを使用する
			NextBlendMode = DX_BLENDMODE_SUB1 ;
			break ;

		case DX_BLENDMODE_NOBLEND :
			// 描画先にα値がある場合は DX_BLENDMODE_NOBLEND でもブレンドモードは DX_BLENDMODE_SRCCOLOR にする
			if( GSYS.DrawSetting.AlphaChDrawMode )
			{
				NextBlendMode = DX_BLENDMODE_SRCCOLOR ;
			}

			// 描画元画像にαチャンネルがある場合やブレンド画像の有無など条件次第で DX_BLENDMODE_ALPHA を使用する
			if( GD3D9.Device.DrawSetting.RenderTexture != NULL )
			{
				if( GD3D9.Device.DrawSetting.AlphaChannelValidFlag == TRUE )
				{
					NextBlendMode = DX_BLENDMODE_ALPHA ;
				}
			}
			break ;
		}
	}

	ShaderModel		= ( Shader->ValidPixelShader_SM3 && Shader->ValidVertexShader_SM3 ) ? 1 : 0 ;
	Specular		= GD3D9.Device.State.Material.Specular.r > 0.00001f ||
					  GD3D9.Device.State.Material.Specular.g > 0.00001f ||
					  GD3D9.Device.State.Material.Specular.b > 0.00001f ? 1 : 0 ;
	ShadowMap       = GSYS.DrawSetting.UseShadowMapNum != 0 ? 1 : 0 ;
	FogType         = GD3D9.Device.State.FogEnable ? GD3D9.Device.State.FogMode : DX_FOGMODE_NONE ;
	RenderBlendType = __RGBBlendStateTable[ NextBlendMode ][ RGB_BLENDSTATE_TABLE_TEXBLENDTABLEINDEX ] ;
	BumpMap         = 0 ;

	if( GSYS.DrawSetting.ShadowMapDraw )
	{
		// シャドウマップへの描画

		VS           =  &SB3D->Base3D_ShadowMap_VS       [ BumpMap ] ;
		VSAddress    = &SCB3D->Base3D_ShadowMap_VSAddress[ BumpMap ] ;

		PS           =  &SB3D->Base3D_ShadowMap_Normal_PS        ;
		PSAddress    = &SCB3D->Base3D_ShadowMap_Normal_PSAddress ;
	}
	else
	if( GD3D9.Device.State.Lighting )
	{
		// ライティングあり描画

		int LightMode[ DX_PS_CONSTB_LIGHT2_NUM ] ;
		int i ;

		for( i = 0 ; i < DX_PS_CONSTB_LIGHT2_NUM ; i ++ )
		{
			LightMode[ i ] = GD3D9.Device.State.LightEnableFlag[ i ] ? GD3D9.Device.State.LightParam[ i ].Type : 0 ;
		}

		LightIndex84 = GD3D9.ShaderCode.Base.LightIndexList84[ LightMode[ 0 ] ][ LightMode[ 1 ] ][ LightMode[ 2 ] ][ LightMode[ 3 ] ][ LightMode[ 4 ] ][ LightMode[ 5 ] ] ;
		LightIndex20 = GD3D9.ShaderCode.Base.LightIndexList20[ LightMode[ 0 ] ][ LightMode[ 1 ] ][ LightMode[ 2 ] ] ;
		LightIndex10 = GD3D9.ShaderCode.Base.LightIndexList10
				[ LightMode[ 0 ] == DX_LIGHTTYPE_D3DLIGHT_DIRECTIONAL ? 2 : ( LightMode[ 0 ] ? 1 : 0 ) ]
				[ LightMode[ 1 ] == DX_LIGHTTYPE_D3DLIGHT_DIRECTIONAL ? 2 : ( LightMode[ 1 ] ? 1 : 0 ) ]
				[ LightMode[ 2 ] == DX_LIGHTTYPE_D3DLIGHT_DIRECTIONAL ? 2 : ( LightMode[ 2 ] ? 1 : 0 ) ] ;

		VS_PL        =  &SB3D->Base3D_PixelLighting_VS       [ ShadowMap ][ FogType ] ;
		VSAddress_PL = &SCB3D->Base3D_PixelLighting_VSAddress[ ShadowMap ][ FogType ] ;

		PS_PL        =  &SB3D->Base3D_PixelLighting_Normal_PS       [ ShadowMap ][ LightIndex84 ][ Specular ][ RenderBlendType ] ;
		PSAddress_PL = &SCB3D->Base3D_PixelLighting_Normal_PSAddress[ ShadowMap ][ LightIndex84 ][ Specular ][ RenderBlendType ] ;

		VS           =  &SB3D->Base3D_VertexLighting_VS       [ ShaderModel ][ ShadowMap ][ FogType ][ LightIndex20 ][ Specular ] ;
		VSAddress    = &SCB3D->Base3D_VertexLighting_VSAddress[ ShaderModel ][ ShadowMap ][ FogType ][ LightIndex20 ][ Specular ] ;

		PS           =  &SB3D->Base3D_VertexLighting_Normal_PS       [ ShaderModel ][ ShadowMap ][ LightIndex10 ][ Specular ][ RenderBlendType ] ;
		PSAddress    = &SCB3D->Base3D_VertexLighting_Normal_PSAddress[ ShaderModel ][ ShadowMap ][ LightIndex10 ][ Specular ][ RenderBlendType ] ;
	}
	else
	{
		// ライティングなし描画

		VS           =  &SB3D->Base3D_NoLighting_VS       [ FogType ] ;
		VSAddress    = &SCB3D->Base3D_NoLighting_VSAddress[ FogType ] ;

		PS           =  &SB3D->Base3D_NoLighting_Normal_PS       [ RenderBlendType ] ;
		PSAddress    = &SCB3D->Base3D_NoLighting_Normal_PSAddress[ RenderBlendType ] ;
	}

	ValidPL =
		LightIndex84 != 0 &&

		Shader->ValidVertexShader_SM3 &&
		VSAddress_PL != NULL &&

		Shader->ValidPixelShader_SM3 &&
		PSAddress_PL != NULL ;

	if( ( GSYS.Light.EnableNum > 3 /* 頂点単位ライティングではライトは３つまでしか対応していない */ 
		  || GD3D9.UsePixelLightingShader ) && ValidPL )
	{
SHADER_MODEL_3:
		// シェーダーがあるかどうかを調べる
		if( *VS_PL == NULL )
		{
			// シェーダーの作成を試みる
			if( Direct3DDevice9_CreateVertexShader( ( DWORD * )*VSAddress_PL, VS_PL ) != 0 )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x47\x00\x72\x00\x61\x00\x70\x00\x68\x00\x69\x00\x63\x00\x73\x00\x5f\x00\x44\x00\x33\x00\x44\x00\x39\x00\x5f\x00\x53\x00\x68\x00\x61\x00\x64\x00\x65\x00\x72\x00\x5f\x00\x4e\x00\x6f\x00\x72\x00\x6d\x00\x61\x00\x6c\x00\x33\x00\x44\x00\x44\x00\x72\x00\x61\x00\x77\x00\x5f\x00\x53\x00\x65\x00\x74\x00\x75\x00\x70\x00\x20\x00\x67\x30\x02\x98\xb9\x70\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Graphics_D3D9_Shader_Normal3DDraw_Setup で頂点シェーダーの作成に失敗しました\n" @*/ )) ;
				return FALSE ;
			}
		}

		// バーテックスシェーダーのセットアップ
		Graphics_D3D9_DeviceState_SetVertexShader( *VS_PL ) ;


		// シェーダーがあるかどうかを調べる
		if( *PS_PL == NULL )
		{
			// シェーダーの作成を試みる
			if( Direct3DDevice9_CreatePixelShader( ( DWORD * )*PSAddress_PL, PS_PL ) != 0 )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x47\x00\x72\x00\x61\x00\x70\x00\x68\x00\x69\x00\x63\x00\x73\x00\x5f\x00\x44\x00\x33\x00\x44\x00\x39\x00\x5f\x00\x53\x00\x68\x00\x61\x00\x64\x00\x65\x00\x72\x00\x5f\x00\x4e\x00\x6f\x00\x72\x00\x6d\x00\x61\x00\x6c\x00\x33\x00\x44\x00\x44\x00\x72\x00\x61\x00\x77\x00\x5f\x00\x53\x00\x65\x00\x74\x00\x75\x00\x70\x00\x20\x00\x67\x30\xd4\x30\xaf\x30\xbb\x30\xeb\x30\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Graphics_D3D9_Shader_Normal3DDraw_Setup でピクセルシェーダーの作成に失敗しました\n" @*/ )) ;
				return FALSE ;
			}
		}

		// ピクセルシェーダーのセットアップ
		Graphics_D3D9_DeviceState_SetPixelShader( *PS_PL ) ;
	}
	else
	{
		// シェーダーがあるかどうかを調べる
		if( *VS == NULL )
		{
			// シェーダーの作成を試みる
			if( *VSAddress == NULL ||
				Direct3DDevice9_CreateVertexShader( ( DWORD * )*VSAddress, VS ) != D_D3D_OK )
			{
				if( ValidPL )
				{
					goto SHADER_MODEL_3 ;
				}

				DXST_LOGFILEFMT_ADDUTF16LE(( "\x47\x00\x72\x00\x61\x00\x70\x00\x68\x00\x69\x00\x63\x00\x73\x00\x5f\x00\x44\x00\x33\x00\x44\x00\x39\x00\x5f\x00\x53\x00\x68\x00\x61\x00\x64\x00\x65\x00\x72\x00\x5f\x00\x4e\x00\x6f\x00\x72\x00\x6d\x00\x61\x00\x6c\x00\x33\x00\x44\x00\x44\x00\x72\x00\x61\x00\x77\x00\x5f\x00\x53\x00\x65\x00\x74\x00\x75\x00\x70\x00\x20\x00\x67\x30\x02\x98\xb9\x70\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Graphics_D3D9_Shader_Normal3DDraw_Setup で頂点シェーダーの作成に失敗しました\n" @*/ )) ;
				return FALSE ;
			}
		}

		// バーテックスシェーダーのセットアップ
		Graphics_D3D9_DeviceState_SetVertexShader( *VS ) ;


		// シェーダーがあるかどうかを調べる
		if( *PS == NULL )
		{
			// シェーダーの作成を試みる
			if( *PSAddress == NULL ||
				Direct3DDevice9_CreatePixelShader( ( DWORD * )*PSAddress, PS ) != D_D3D_OK )
			{
				if( ValidPL )
				{
					goto SHADER_MODEL_3 ;
				}

				DXST_LOGFILEFMT_ADDUTF16LE(( "\x47\x00\x72\x00\x61\x00\x70\x00\x68\x00\x69\x00\x63\x00\x73\x00\x5f\x00\x44\x00\x33\x00\x44\x00\x39\x00\x5f\x00\x53\x00\x68\x00\x61\x00\x64\x00\x65\x00\x72\x00\x5f\x00\x4e\x00\x6f\x00\x72\x00\x6d\x00\x61\x00\x6c\x00\x33\x00\x44\x00\x44\x00\x72\x00\x61\x00\x77\x00\x5f\x00\x53\x00\x65\x00\x74\x00\x75\x00\x70\x00\x20\x00\x67\x30\xd4\x30\xaf\x30\xbb\x30\xeb\x30\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Graphics_D3D9_Shader_Normal3DDraw_Setup でピクセルシェーダーの作成に失敗しました\n" @*/ )) ;
				return FALSE ;
			}
		}

		// ピクセルシェーダーのセットアップ
		Graphics_D3D9_DeviceState_SetPixelShader( *PS ) ;
	}


	// 終了
	return TRUE ;
}


#ifndef DX_NON_MODEL

// 指定のモデル描画用シェーダーをセットアップする( TRUE:成功  FALSE:失敗 )
extern int Graphics_D3D9_Shader_Model_Setup(
	int VertexShaderIndex,
	int VertexShaderIndex_PL,
	int PixelShaderIndex,
	int PixelShaderIndex_PL )
{
	GRAPHICS_HARDDATA_DIRECT3D9_DEVICE           *Device = &GraphicsHardDataDirect3D9.Device ;
	GRAPHICS_HARDWARE_DIRECT3D9_SHADER           *Shader = &Device->Shader ;
	GRAPHICS_HARDWARE_DIRECT3D9_SHADER_MODEL     *SM     = &Shader->Model ;
	GRAPHICS_HARDWARE_DIRECT3D9_SHADERCODE_MODEL *SCM    = &GraphicsHardDataDirect3D9.ShaderCode.Model ;
	int                        ValidPL ;
	D_IDirect3DVertexShader9 **VS_PL        = NULL ;
	void                     **VSAddress_PL = NULL ;
	D_IDirect3DPixelShader9  **PS_PL        = NULL ;
	void                     **PSAddress_PL = NULL ;
	D_IDirect3DVertexShader9 **VS           = NULL ;
	void                     **VSAddress    = NULL ;
	D_IDirect3DPixelShader9  **PS           = NULL ;
	void                     **PSAddress    = NULL ;
	int                        PS_Type_PL = 0 ;
	int                        PS_ShadowMap_PL = 0 ;
	int                        PS_MultiTexBlendMode_PL = 0 ;
	int                        PS_ToonType_PL = 0 ;
	int                        PS_ToonSphereOP_PL = 0 ;
	int                        PS_ToonDiffuseBlendOP_PL = 0 ;
	int                        PS_ToonSpecularBlendOP_PL = 0 ;
	int                        PS_SpecularMap_PL = 0 ;
	int                        PS_BumpMap_PL = 0 ;
	int                        PS_LightIndex_PL = 0 ;
	int                        PS_Specular_PL = 0 ;
	int                        VS_Type = 0 ;
	int                        VS_ShaderModel = 0 ;
	int                        VS_ShadowMap = 0 ;
	int                        VS_MeshType = 0 ;
	int                        VS_BumpMap = 0 ;
	int                        VS_FogMode = 0 ;
	int                        VS_LightIndex = 0 ;
	int                        VS_Specular = 0 ;
	int                        PS_MaterialType = 0 ;
	int                        PS_Type = 0 ;
	int                        PS_ShaderModel = 0 ;
	int                        PS_ShadowMap = 0 ;
	int                        PS_MultiTexBlendMode = 0 ;
	int                        PS_Toon = 0 ;
	int                        PS_ToonType = 0 ;
	int                        PS_ToonSphereOP = 0 ;
	int                        PS_ToonDiffuseBlendOP = 0 ;
	int                        PS_ToonSpecularBlendOP = 0 ;
	int                        PS_SpecularMap = 0 ;
	int                        PS_BumpMap = 0 ;
	int                        PS_LightIndex = 0 ;
	int                        PS_Specular = 0 ;
//	int                        Time = NS_GetNowCount() ;

	if( 0 )
//	if( Time % 1000 < 500 )
	{
		GSYS.HardInfo.UseShader = FALSE ;
		GD3D9.Device.Caps.ValidTexTempRegFlag = FALSE ;
		return FALSE ;
	}
/*	else
	{
		GSYS.HardInfo.UseShader = TRUE ;
	}
*/

	// シェーダが使えない場合は失敗
	if( GSYS.HardInfo.UseShader == FALSE )
	{
		return FALSE ;
	}

	if( VertexShaderIndex_PL >= 0 )
	{
		VS_PL        = &( ( D_IDirect3DVertexShader9 ** )SM->MV1_PixelLighting_VS         )[ VertexShaderIndex_PL ] ;
		VSAddress_PL = &( ( void **                     )SCM->MV1_PixelLighting_VSAddress )[ VertexShaderIndex_PL ] ;
	}

	if( PixelShaderIndex_PL >= 0 )
	{
		PS_Type_PL        = D3D9_PIXELLIGHTING_PIXELSHADER_GET_TYPE(        PixelShaderIndex_PL ) ;
		PS_ShadowMap_PL   = D3D9_PIXELLIGHTING_PIXELSHADER_GET_SHADOWMAP(   PixelShaderIndex_PL ) ;
		PS_SpecularMap_PL = D3D9_PIXELLIGHTING_PIXELSHADER_GET_SPECULARMAP( PixelShaderIndex_PL ) ;
		PS_BumpMap_PL     = D3D9_PIXELLIGHTING_PIXELSHADER_GET_BUMPMAP(     PixelShaderIndex_PL ) ;
		PS_LightIndex_PL  = D3D9_PIXELLIGHTING_PIXELSHADER_GET_LIGHTINDEX(  PixelShaderIndex_PL ) ;
		PS_Specular_PL    = D3D9_PIXELLIGHTING_PIXELSHADER_GET_SPECULAR(    PixelShaderIndex_PL ) ;
		switch( PS_Type_PL )
		{
		case D3D9_PIXELLIGHTING_PIXELSHADER_TYPE_TOON :
			PS_ToonType_PL            = D3D9_PIXELLIGHTING_PIXELSHADER_GET_TOONTYPE(     PixelShaderIndex_PL ) ;
			PS_ToonSphereOP_PL        = D3D9_PIXELLIGHTING_PIXELSHADER_GET_TOONSPHEREOP( PixelShaderIndex_PL ) ;
			PS_ToonDiffuseBlendOP_PL  = D3D9_PIXELLIGHTING_PIXELSHADER_GET_TOONDIFBLDOP( PixelShaderIndex_PL ) ;
			PS_ToonSpecularBlendOP_PL = D3D9_PIXELLIGHTING_PIXELSHADER_GET_TOONSPCBLDOP( PixelShaderIndex_PL ) ;

			PS_PL        =  &SM->MV1_PixelLighting_Toon_PS       [ PS_ShadowMap_PL ][ PS_ToonType_PL ][ PS_ToonSphereOP_PL ][ PS_ToonDiffuseBlendOP_PL ][ PS_ToonSpecularBlendOP_PL ][ PS_SpecularMap_PL ][ PS_BumpMap_PL ][ PS_LightIndex_PL ][ PS_Specular_PL ] ;
			PSAddress_PL = &SCM->MV1_PixelLighting_Toon_PSAddress[ PS_ShadowMap_PL ][ PS_ToonType_PL ][ PS_ToonSphereOP_PL ][ PS_ToonDiffuseBlendOP_PL ][ PS_ToonSpecularBlendOP_PL ][ PS_SpecularMap_PL ][ PS_BumpMap_PL ][ PS_LightIndex_PL ][ PS_Specular_PL ] ;
			break ;

		case D3D9_PIXELLIGHTING_PIXELSHADER_TYPE_NORMAL :
			PS_MultiTexBlendMode_PL = D3D9_PIXELLIGHTING_PIXELSHADER_GET_MULTITEX( PixelShaderIndex_PL ) ;

			PS_PL        = & SM->MV1_PixelLighting_Normal_PS       [ PS_ShadowMap_PL ][ PS_MultiTexBlendMode_PL ][ PS_SpecularMap_PL ][ PS_BumpMap_PL ][ PS_LightIndex_PL ][ PS_Specular_PL ] ;
			PSAddress_PL = &SCM->MV1_PixelLighting_Normal_PSAddress[ PS_ShadowMap_PL ][ PS_MultiTexBlendMode_PL ][ PS_SpecularMap_PL ][ PS_BumpMap_PL ][ PS_LightIndex_PL ][ PS_Specular_PL ] ;
			break ;
		}
	}

	if( VertexShaderIndex >= 0 )
	{
		VS_Type     = D3D9_VERTEXSHADER_GET_TYPE(        VertexShaderIndex ) ;
		VS_MeshType = D3D9_VERTEXSHADER_GET_MESHTYPE(    VertexShaderIndex ) ;
		VS_FogMode  = D3D9_VERTEXSHADER_GET_FOGMODE(     VertexShaderIndex ) ;
		switch( VS_Type )
		{
		case D3D9_VERTEXSHADER_TYPE_NORMAL :
			VS_LightIndex  = D3D9_VERTEXSHADER_GET_LIGHTINDEX(  VertexShaderIndex ) ;
			if( VS_LightIndex == 0 )
			{
				VS        =  &SM->MV1_NoLighting_VS       [ VS_MeshType ][ VS_FogMode ] ;
				VSAddress = &SCM->MV1_NoLighting_VSAddress[ VS_MeshType ][ VS_FogMode ] ;
			}
			else
			{
				VS_ShaderModel = D3D9_VERTEXSHADER_GET_SHADERMODEL( VertexShaderIndex ) ;
				VS_ShadowMap   = D3D9_VERTEXSHADER_GET_SHADOWMAP(   VertexShaderIndex ) ;
				VS_BumpMap     = D3D9_VERTEXSHADER_GET_BUMPMAP(     VertexShaderIndex ) ;
				VS_Specular    = D3D9_VERTEXSHADER_GET_SPECULAR(    VertexShaderIndex ) ;

				if( Shader->ValidVertexShader_SM3 )
				{
					VS_ShaderModel = 1 ;
				}

				VS        = & SM->MV1_VertexLighting_VS       [ VS_ShaderModel ][ VS_ShadowMap ][ VS_MeshType ][ VS_BumpMap ][ VS_FogMode ][ VS_LightIndex ][ VS_Specular ] ;
				VSAddress = &SCM->MV1_VertexLighting_VSAddress[ VS_ShaderModel ][ VS_ShadowMap ][ VS_MeshType ][ VS_BumpMap ][ VS_FogMode ][ VS_LightIndex ][ VS_Specular ] ;
			}
			break ;

		case D3D9_VERTEXSHADER_TYPE_DRAW_SHADOWMAP :
			VS        =  &SM->MV1_ShadowMap_VS       [ VS_MeshType ] ;
			VSAddress = &SCM->MV1_ShadowMap_VSAddress[ VS_MeshType ] ;
			break ;

		case D3D9_VERTEXSHADER_TYPE_TOON_OUTLINE :
			VS        =  &SM->MV1_ToonOutLine_VS       [ VS_MeshType ][ VS_FogMode ] ;
			VSAddress = &SCM->MV1_ToonOutLine_VSAddress[ VS_MeshType ][ VS_FogMode ] ;
			break ;
		}
	}

	if( PixelShaderIndex >= 0 )
	{
		PS_MaterialType = D3D9_PIXELSHADER_GET_MATERIALTYPE( PixelShaderIndex ) ;
		PS_Type         = D3D9_PIXELSHADER_GET_TYPE(         PixelShaderIndex ) ;
		PS_ShaderModel  = D3D9_PIXELSHADER_GET_SHADERMODEL(  PixelShaderIndex ) ;
		PS_ShadowMap    = D3D9_PIXELSHADER_GET_SHADOWMAP(    PixelShaderIndex ) ;
		PS_SpecularMap  = D3D9_PIXELSHADER_GET_SPECULARMAP(  PixelShaderIndex ) ;
		PS_BumpMap      = D3D9_PIXELSHADER_GET_BUMPMAP(      PixelShaderIndex ) ;
		PS_LightIndex   = D3D9_PIXELSHADER_GET_LIGHTINDEX(   PixelShaderIndex ) ;
		PS_Specular     = D3D9_PIXELSHADER_GET_SPECULAR(     PixelShaderIndex ) ;

		if( Shader->ValidPixelShader_SM3 )
		{
			PS_ShaderModel = 1 ;
		}

		if( PS_MaterialType >= DX_MATERIAL_TYPE_MAT_SPEC_LUMINANCE_UNORM &&
			PS_MaterialType <= DX_MATERIAL_TYPE_MAT_SPEC_POWER_CMP_GREATEREQUAL )
		{
			PS        =  &SM->MV1_MaterialType_PS       [ PS_MaterialType ] ;
			PSAddress = &SCM->MV1_MaterialType_PSAddress[ PS_MaterialType ] ;
		}
		else
		{
			switch( PS_Type )
			{
			case D3D9_PIXELSHADER_TYPE_NORMAL :
				PS_Toon = D3D9_PIXELSHADER_GET_TOON( PixelShaderIndex ) ;
				if( PS_Toon )
				{
					PS_ToonType            = D3D9_PIXELSHADER_GET_TOONTYPE(     PixelShaderIndex ) ;
					PS_ToonDiffuseBlendOP  = D3D9_PIXELSHADER_GET_TOONDIFBLDOP( PixelShaderIndex ) ;
					if( PS_LightIndex == 0 )
					{
						PS        =  &SM->MV1_NoLighting_Toon_PS       [ PS_ToonType ][ PS_ToonDiffuseBlendOP ] ;
						PSAddress = &SCM->MV1_NoLighting_Toon_PSAddress[ PS_ToonType ][ PS_ToonDiffuseBlendOP ] ;
					}
					else
					{
						PS_ToonSphereOP        = D3D9_PIXELSHADER_GET_TOONSPHEREOP( PixelShaderIndex ) ;
						PS_ToonSpecularBlendOP = D3D9_PIXELSHADER_GET_TOONSPCBLDOP( PixelShaderIndex ) ;

						PS        =  &SM->MV1_VertexLighting_Toon_PS       [ PS_ShaderModel ][ PS_ShadowMap ][ PS_ToonType ][ PS_ToonSphereOP ][ PS_ToonDiffuseBlendOP ][ PS_ToonSpecularBlendOP ][ PS_SpecularMap ][ PS_BumpMap ][ PS_LightIndex ][ PS_Specular ] ;
						PSAddress = &SCM->MV1_VertexLighting_Toon_PSAddress[ PS_ShaderModel ][ PS_ShadowMap ][ PS_ToonType ][ PS_ToonSphereOP ][ PS_ToonDiffuseBlendOP ][ PS_ToonSpecularBlendOP ][ PS_SpecularMap ][ PS_BumpMap ][ PS_LightIndex ][ PS_Specular ] ;
					}
				}
				else
				{
					PS_MultiTexBlendMode = D3D9_PIXELSHADER_GET_MULTITEX( PixelShaderIndex ) ;
					if( PS_LightIndex == 0 )
					{
						PS        =  &SM->MV1_NoLighting_Normal_PS       [ PS_MultiTexBlendMode ] ;
						PSAddress = &SCM->MV1_NoLighting_Normal_PSAddress[ PS_MultiTexBlendMode ] ;
					}
					else
					{
						PS        =  &SM->MV1_VertexLighting_Normal_PS       [ PS_ShaderModel ][ PS_ShadowMap ][ PS_MultiTexBlendMode ][ PS_SpecularMap ][ PS_BumpMap ][ PS_LightIndex ][ PS_Specular ] ;
						PSAddress = &SCM->MV1_VertexLighting_Normal_PSAddress[ PS_ShaderModel ][ PS_ShadowMap ][ PS_MultiTexBlendMode ][ PS_SpecularMap ][ PS_BumpMap ][ PS_LightIndex ][ PS_Specular ] ;
					}
				}
				break ;

			case D3D9_PIXELSHADER_TYPE_DRAW_SHADOWMAP :
				PS_Toon = D3D9_PIXELSHADER_GET_TOON( PixelShaderIndex ) ;
				if( PS_Toon )
				{
					PS_ToonType            = D3D9_PIXELSHADER_GET_TOONTYPE(     PixelShaderIndex ) ;
					PS_ToonSphereOP        = D3D9_PIXELSHADER_GET_TOONSPHEREOP( PixelShaderIndex ) ;
					PS_ToonDiffuseBlendOP  = D3D9_PIXELSHADER_GET_TOONDIFBLDOP( PixelShaderIndex ) ;

					PS        =  &SM->MV1_ShadowMap_Toon_PS       [ PS_ToonType ][ PS_ToonSphereOP ][ PS_ToonDiffuseBlendOP ] ;
					PSAddress = &SCM->MV1_ShadowMap_Toon_PSAddress[ PS_ToonType ][ PS_ToonSphereOP ][ PS_ToonDiffuseBlendOP ] ;
				}
				else
				{
					PS        =  &SM->MV1_ShadowMap_Normal_PS ;
					PSAddress = &SCM->MV1_ShadowMap_Normal_PSAddress ;
				}
				break ;

			case D3D9_PIXELSHADER_TYPE_TOON_OUTLINE :
				PS        =  &SM->MV1_ToonOutLine_PS ;
				PSAddress = &SCM->MV1_ToonOutLine_PSAddress ;
				break ;
			}
		}
	}

	ValidPL = 
		VertexShaderIndex_PL >= 0 &&
		Shader->ValidVertexShader_SM3 &&
		VS_Type == D3D9_VERTEXSHADER_TYPE_NORMAL &&
		VS_LightIndex != 0 &&
		*VSAddress_PL != NULL &&

		PixelShaderIndex_PL >= 0 &&
		Shader->ValidPixelShader_SM3 &&
		PS_Type == D3D9_PIXELSHADER_TYPE_NORMAL &&
		PS_LightIndex != 0 &&
		*PSAddress_PL != NULL
	;

//	void *Addr1 = *VSAddress_PL ;
//	void *Addr2 = *PSAddress_PL ;

	if( ( Shader->UseOnlyPixelLightingTypeCode || GD3D9.UsePixelLightingShader ) && ValidPL )
	{
SHADER_MODEL_3:
		if( VertexShaderIndex >= 0 )
		{
			// シェーダーがあるかどうかを調べる
			if( *VS_PL == NULL )
			{
				// シェーダーの作成を試みる
				if( Direct3DDevice9_CreateVertexShader( ( DWORD * )*VSAddress_PL, VS_PL ) != D_D3D_OK )
					return FALSE ;
			}

			// バーテックスシェーダーのセットアップ
			Graphics_D3D9_DeviceState_SetVertexShader( *VS_PL ) ;
		}

		if( PixelShaderIndex >= 0 )
		{
			// シェーダーがあるかどうかを調べる
			if( *PS_PL == NULL )
			{
				// シェーダーの作成を試みる
				if( Direct3DDevice9_CreatePixelShader( ( DWORD * )*PSAddress_PL, PS_PL ) != D_D3D_OK )
					return FALSE ;
			}

			// ピクセルシェーダーのセットアップ
			Graphics_D3D9_DeviceState_SetPixelShader( *PS_PL ) ;
		}
	}
	else
	{
		if( VertexShaderIndex >= 0 )
		{
			// シェーダーがあるかどうかを調べる
			if( *VS == NULL )
			{
				// シェーダーの作成を試みる
				if( *VSAddress == NULL ||
					Direct3DDevice9_CreateVertexShader( ( DWORD * )*VSAddress, VS ) != D_D3D_OK )
				{
					if( ValidPL )
					{
						goto SHADER_MODEL_3 ;
					}
					return FALSE ;
				}
			}

			// バーテックスシェーダーのセットアップ
		#ifdef DXLIB_DEBUG
			Graphics_D3D9_DeviceState_SetVertexShader( MV1Man.VS_Test ) ;
			Graphics_D3D9_DeviceState_SetVertexDeclaration( MV1Man.VertexDeclaration[ 1 ][ 0 ][ 1 ] ) ;
		#else
			Graphics_D3D9_DeviceState_SetVertexShader( *VS ) ;
		#endif
		}

		if( PixelShaderIndex >= 0 )
		{
			// シェーダーがあるかどうかを調べる
			if( *PS == NULL )
			{
				// シェーダーの作成を試みる
				if( *PSAddress == NULL ||
					Direct3DDevice9_CreatePixelShader( ( DWORD * )*PSAddress, PS ) != D_D3D_OK )
				{
					if( ValidPL )
					{
						goto SHADER_MODEL_3 ;
					}

					return FALSE ;
				}
			}

			// ピクセルシェーダーのセットアップ
		#ifdef DXLIB_DEBUG
			Graphics_D3D9_DeviceState_SetPixelShader( MV1Man.PS_Test ) ;
		#else
			Graphics_D3D9_DeviceState_SetPixelShader( *PS ) ;
		#endif
		}
	}


	// 終了
	return TRUE ;
}

#endif // DX_NON_MODEL











// 使用するシェーダーのインデックスの値を更新する
extern void Graphics_D3D9_Shader_RefreshUseIndex( void )
{
#ifndef DX_NON_MODEL
	GRAPHICS_HARDWARE_DIRECT3D9_SHADER *Shader = &GraphicsHardDataDirect3D9.Device.Shader ;
	int LightMode[ DX_PS_CONSTB_LIGHT2_NUM ] ;
	int PhongValid ;
	int i ;
	int UseShadowMap ;

	if( GSYS.HardInfo.UseShader == FALSE )
	{
		return ;
	}

	PhongValid     = 0 ;
	LightMode[ 0 ] = 0 ;
	LightMode[ 1 ] = 0 ;
	LightMode[ 2 ] = 0 ;
	LightMode[ 3 ] = 0 ;
	LightMode[ 4 ] = 0 ;
	LightMode[ 5 ] = 0 ;
	Shader->UseOnlyPixelLightingTypeCode = FALSE ;
	if( GD3D9.Device.State.Lighting )
	{
		for( i = 0 ; i < DX_PS_CONSTB_LIGHT2_NUM ; i ++ )
		{
			if( GD3D9.Device.State.LightEnableFlag[ i ] )
			{
				LightMode[ i ] = GD3D9.Device.State.LightParam[ i ].Type ;
				PhongValid = 1 ;

				// 頂点単位ライティングではライトは３つまでしか対応していない
				if( i >= 3 )
				{
					Shader->UseOnlyPixelLightingTypeCode = TRUE ;
				}
			}
		}
	}

	UseShadowMap = FALSE ;
	for( i = 0 ; i < MAX_USE_SHADOWMAP_NUM ; i ++ )
	{
		if( GSYS.DrawSetting.ShadowMap[ i ] > 0 )
		{
			UseShadowMap = TRUE ;
			break ;
		}
	}

	Shader->UseBaseVertexShaderIndex =
		D3D9_VERTEXSHADER_TYPE( GSYS.DrawSetting.ShadowMapDraw ? D3D9_VERTEXSHADER_TYPE_DRAW_SHADOWMAP : D3D9_VERTEXSHADER_TYPE_NORMAL ) +
		D3D9_VERTEXSHADER_SHADOWMAP( UseShadowMap ? 1 : 0 ) +
		D3D9_VERTEXSHADER_FOGMODE( GD3D9.Device.State.FogEnable ? GD3D9.Device.State.FogMode : DX_FOGMODE_NONE ) +
		D3D9_VERTEXSHADER_LIGHTINDEX( GD3D9.ShaderCode.Base.LightIndexList20[ LightMode[ 0 ] ][ LightMode[ 1 ] ][ LightMode[ 2 ] ] )
	;

	Shader->UseBasePixelShaderIndex =
		D3D9_PIXELSHADER_TYPE( GSYS.DrawSetting.ShadowMapDraw ? D3D9_PIXELSHADER_TYPE_DRAW_SHADOWMAP : D3D9_PIXELSHADER_TYPE_NORMAL ) +
		D3D9_PIXELSHADER_SHADOWMAP( UseShadowMap ? 1 : 0 ) +
		D3D9_PIXELSHADER_LIGHTINDEX( 
			GD3D9.ShaderCode.Base.LightIndexList10
				[ LightMode[ 0 ] == DX_LIGHTTYPE_D3DLIGHT_DIRECTIONAL ? 2 : ( LightMode[ 0 ] ? 1 : 0 ) ]
				[ LightMode[ 1 ] == DX_LIGHTTYPE_D3DLIGHT_DIRECTIONAL ? 2 : ( LightMode[ 1 ] ? 1 : 0 ) ]
				[ LightMode[ 2 ] == DX_LIGHTTYPE_D3DLIGHT_DIRECTIONAL ? 2 : ( LightMode[ 2 ] ? 1 : 0 ) ] 
		)
	;

	Shader->UseBaseVertexShaderIndex_PL =
		D3D9_PIXELLIGHTING_VERTEXSHADER_FOGMODE( GD3D9.Device.State.FogEnable ? GD3D9.Device.State.FogMode : DX_FOGMODE_NONE ) +
		D3D9_PIXELLIGHTING_VERTEXSHADER_SHADOWMAP( UseShadowMap ? 1 : 0 )
	;

	Shader->UseBasePixelShaderIndex_PL =
		D3D9_PIXELLIGHTING_PIXELSHADER_LIGHTINDEX( GD3D9.ShaderCode.Base.LightIndexList84[ LightMode[ 0 ] ][ LightMode[ 1 ] ][ LightMode[ 2 ] ][ LightMode[ 3 ] ][ LightMode[ 4 ] ][ LightMode[ 5 ] ] ) +
		D3D9_PIXELLIGHTING_PIXELSHADER_SHADOWMAP( UseShadowMap ? 1 : 0 )
	;
#endif // DX_NON_MODEL
}

// シェーダーの定数情報を得る
extern	D_D3DXSHADER_CONSTANTINFO *Graphics_D3D9_GetShaderConstInfo( SHADERHANDLEDATA *Shader, const wchar_t *ConstantName )
{
	D_D3DXSHADER_CONSTANTINFO *Info ;
	int i ;

	// 指定の名前を持つ定数を捜す
	Info = Shader->PF->D3D9.ConstantInfo ;

	char ConstantNameA[ 512 ] ;

	ConvString( ( const char * )ConstantName, -1, WCHAR_T_CHARCODEFORMAT, ConstantNameA, sizeof( ConstantNameA ), DX_CHARCODEFORMAT_ASCII ) ;
	for( i = 0 ; i < Shader->PF->D3D9.ConstantNum ; i ++, Info ++ )
	{
		if( _STRCMP( ConstantNameA, ( char * )Shader->FunctionCode + Info->Name + 12 ) == 0 )
			break ;
	}

	if( i == Shader->PF->D3D9.ConstantNum )
		return NULL ;

	// 情報を返す
	return Info ;
}





















// 頂点バッファ・インデックスバッファ関係

// 頂点バッファハンドル用の Direct3DVertexBuffer9 オブジェクトを作成する
extern	int		Graphics_D3D9_VertexBuffer_CreateObject( VERTEXBUFFERHANDLEDATA *VertexBuffer, int Restore )
{
	DWORD FVF ;
	int Result ;

	// 頂点処理をハードウエアで処理する場合のみ Direct3D9VertexBuffer を作成する
	if( GD3D9.Device.Caps.VertexHardwareProcess == FALSE && GD3D9.Device.Shader.ValidVertexShader == FALSE )
	{
		return 0 ;
	}

	// 対応していないタイプの場合はエラー
	FVF = VERTEXFVF_3D_LIGHT ;
	switch( VertexBuffer->Type )
	{
	case DX_VERTEX_TYPE_NORMAL_3D :
		FVF = VERTEXFVF_3D_LIGHT ;
		break ;

	case DX_VERTEX_TYPE_SHADER_3D :
		FVF = VERTEXFVF_3D_LIGHT ;
		break ;

	default :
		return -1 ;
	}

	// 頂点バッファを作成する
	Result = Direct3DDevice9_CreateVertexBuffer_ASync( 
				( DWORD )( VertexBuffer->UnitSize * VertexBuffer->Num ),
				D_D3DUSAGE_WRITEONLY,
				FVF,
				D_D3DPOOL_DEFAULT,
				&VertexBuffer->PF->D3D9.VertexBuffer ) ;
	if( Result < 0 )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x02\x98\xb9\x70\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"頂点バッファの作成に失敗しました\n" @*/ ) ;
		return -1 ;
	}

	// データを復帰する
	if( Restore )
	{
		NS_SetVertexBufferData( 0, VertexBuffer->Buffer, VertexBuffer->Num, VertexBuffer->HandleInfo.Handle ) ; 
	}

	// 終了
	return 0 ;
}

// 頂点バッファハンドル用の Direct3DVertexBuffer9 オブジェクトを解放する
extern	int		Graphics_D3D9_VertexBuffer_ReleaseObject( VERTEXBUFFERHANDLEDATA *VertexBuffer )
{
	if( VertexBuffer->PF->D3D9.VertexBuffer != NULL )
	{
		Direct3D9_ObjectRelease( VertexBuffer->PF->D3D9.VertexBuffer ) ;
		VertexBuffer->PF->D3D9.VertexBuffer = NULL ;
	}

	// 終了
	return 0 ;
}

// インデックスバッファハンドル用の Direct3DIndexBuffer9 オブジェクトを作成する
extern	int		Graphics_D3D9_IndexBuffer_CreateObject( INDEXBUFFERHANDLEDATA *IndexBuffer, int Restore )
{
	D_D3DFORMAT Format ;

	// 頂点処理をハードウエアで処理する場合のみ Direct3D9IndexBuffer を作成する
	if( GD3D9.Device.Caps.VertexHardwareProcess == FALSE && GD3D9.Device.Shader.ValidVertexShader == FALSE )
	{
		return 0 ;
	}

	// 対応していないタイプの場合はエラー
	Format = D_D3DFMT_INDEX16 ;
	switch( IndexBuffer->Type )
	{
	case DX_INDEX_TYPE_16BIT :
		Format = D_D3DFMT_INDEX16 ;
		break ;

	case DX_INDEX_TYPE_32BIT :
		Format = D_D3DFMT_INDEX32 ;
		break ;
	}

	// インデックスバッファを作成する
	if( Direct3DDevice9_CreateIndexBuffer_ASync( 
			( DWORD )( IndexBuffer->UnitSize * IndexBuffer->Num ),
			D_D3DUSAGE_WRITEONLY,
			Format,
			D_D3DPOOL_DEFAULT,
			&IndexBuffer->PF->D3D9.IndexBuffer
		) < 0 )
	{
		DXST_LOGFILE_ADDUTF16LE( "\xa4\x30\xf3\x30\xc7\x30\xc3\x30\xaf\x30\xb9\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"インデックスバッファの作成に失敗しました\n" @*/ ) ;
		return -1 ;
	}

	// データを復帰する
	if( Restore )
	{
		NS_SetIndexBufferData( 0, IndexBuffer->Buffer, IndexBuffer->Num, IndexBuffer->HandleInfo.Handle ) ; 
	}

	// 終了
	return 0 ;
}

// インデックスバッファハンドル用の Direct3DIndexBuffer9 オブジェクトを解放する
extern	int		Graphics_D3D9_IndexBuffer_ReleaseObject( INDEXBUFFERHANDLEDATA *IndexBuffer )
{
	if( IndexBuffer->PF->D3D9.IndexBuffer != NULL )
	{
		Direct3D9_ObjectRelease( IndexBuffer->PF->D3D9.IndexBuffer ) ;
		IndexBuffer->PF->D3D9.IndexBuffer = NULL ;
	}

	// 終了
	return 0 ;
}



























// Direct3D の画面関係

// ScreenCopy や GetDrawScreen を実現するために使用するテンポラリバッファの作成( 0:成功  -1:失敗 )
extern	int		Graphics_D3D9_SetupSubBackBuffer( void )
{
	HRESULT hr ;

	// 既にサブバックバッファが作成されている場合は何もしない
	if( GD3D9.Device.Screen.SubBackBufferSurface != NULL ) return 0 ;

	// デバイスが無効の場合は何もせず終了
	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画を終わらせる
	Graphics_D3D9_EndScene() ;

	// フルシーンアンチエイリアスを使用しない場合はレンダリングターゲットにできるテクスチャを作成する
	GD3D9.Device.Screen.SubBackBufferTexture = NULL ;
	if( GSYS.Setting.FSAAMultiSampleCount == 0 && GSYS.Setting.FSAAMultiSampleQuality == 0 )
	{
		// レンダリングターゲットにできるテクスチャのサイズを決定
		if( GSYS.HardInfo.TextureSizePow2 )
		{
			for( GD3D9.Device.Screen.SubBackBufferTextureSizeX = 1 ;
				GD3D9.Device.Screen.SubBackBufferTextureSizeX < GSYS.Screen.MainScreenSizeX ;
				GD3D9.Device.Screen.SubBackBufferTextureSizeX <<= 1 ){}

			for( GD3D9.Device.Screen.SubBackBufferTextureSizeY = 1 ;
				GD3D9.Device.Screen.SubBackBufferTextureSizeY < GSYS.Screen.MainScreenSizeY ;
				GD3D9.Device.Screen.SubBackBufferTextureSizeY <<= 1 ){}
		}
		else
		{
			GD3D9.Device.Screen.SubBackBufferTextureSizeX = GSYS.Screen.MainScreenSizeX ;
			GD3D9.Device.Screen.SubBackBufferTextureSizeY = GSYS.Screen.MainScreenSizeY ;
		}

		// レンダリングターゲットにできるテクスチャの作成
		if( Direct3DDevice9_CreateTexture_ASync(
			( UINT )GD3D9.Device.Screen.SubBackBufferTextureSizeX,	// 幅
			( UINT )GD3D9.Device.Screen.SubBackBufferTextureSizeY,	// 高さ
			1,														// MipMap レベル
			D_D3DUSAGE_RENDERTARGET,								// フラグ
			GD3D9.Device.Caps.ScreenFormat,							// フォーマット
			D_D3DPOOL_DEFAULT,										// ビデオメモリに配置する
			&GD3D9.Device.Screen.SubBackBufferTexture,				// D_IDirect3DTexture9 のポインタを受け取るアドレス
			NULL,																																// 絶対 NULL
			FALSE ) == D_D3D_OK )
		{
			// レンダリングターゲットにできるテクスチャのサーフェスを取得する
			if( Direct3DTexture9_GetSurfaceLevel_ASync(
				GD3D9.Device.Screen.SubBackBufferTexture,
				0,
				&GD3D9.Device.Screen.SubBackBufferSurface,
				FALSE ) != D_D3D_OK )
			{
				DXST_LOGFILE_ADDUTF16LE( "\xb5\x30\xd6\x30\xd0\x30\xc3\x30\xaf\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x28\x75\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\xb5\x30\xfc\x30\xd5\x30\xa7\x30\xb9\x30\xa2\x30\xc9\x30\xec\x30\xb9\x30\x6e\x30\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"サブバックバッファ用テクスチャのサーフェスアドレスの取得に失敗しました\n" @*/ ) ;
				return -1 ;
			}
		}
		else
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xb5\x30\xd6\x30\xd0\x30\xc3\x30\xaf\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x28\x75\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x20\x00\x46\x00\x6f\x00\x72\x00\x6d\x00\x61\x00\x74\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x20\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x58\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x20\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x59\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x0a\x00\x00"/*@ L"サブバックバッファ用テクスチャの作成に失敗しました  Format:%d  SizeX:%d  SizeY:%d \n" @*/,
				GD3D9.Device.Caps.ScreenFormat, GD3D9.Device.Screen.SubBackBufferTextureSizeX, GD3D9.Device.Screen.SubBackBufferTextureSizeY )) ;
		}
	}

	// ここに来た時点でレンダリングターゲットが作成されていなかったらレンダリングターゲットを作成する
	if( GD3D9.Device.Screen.SubBackBufferSurface == NULL )
	{
		// レンダリングターゲットの作成
		hr = Direct3DDevice9_CreateRenderTarget(
			( UINT )GSYS.Screen.MainScreenSizeX,
			( UINT )GSYS.Screen.MainScreenSizeY,
			GD3D9.Device.Caps.ScreenFormat,
			( D_D3DMULTISAMPLE_TYPE )GSYS.Setting.FSAAMultiSampleCount,
			( DWORD )GSYS.Setting.FSAAMultiSampleQuality,
			FALSE,
			&GD3D9.Device.Screen.SubBackBufferSurface,
			NULL
		) ;

		if( FAILED( hr ) )
		{
			DXST_LOGFILE_ADDUTF16LE( "\xb5\x30\xd6\x30\xd0\x30\xc3\x30\xaf\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"サブバックバッファの作成に失敗しました\n" @*/ ) ;
			return -1 ;
		}
	}

	// 本バックバッファの内容を転送
	Direct3DDevice9_StretchRect( GD3D9.Device.Screen.BackBufferSurface, NULL, GD3D9.Device.Screen.SubBackBufferSurface, NULL, D_D3DTEXF_NONE ) ;

	// 本バックバッファの内容を初期化
	Direct3DDevice9_ColorFill( GD3D9.Device.Screen.BackBufferSurface, NULL, 0 ) ;

	// レンダリングターゲットの変更
	GD3D9.Device.DrawSetting.CancelSettingEqualCheck = TRUE ;
	Graphics_D3D9_DeviceState_SetRenderTarget( GD3D9.Device.Screen.SubBackBufferSurface ) ;
	GD3D9.Device.DrawSetting.CancelSettingEqualCheck = FALSE ;

	// 終了
	return 0 ;
}

// Ｚバッファオブジェクトの作成( 0:成功  -1:失敗 )
extern	int		Graphics_D3D9_CreateZBuffer( void )
{
	HRESULT hr ;
	DWORD w, h ;
	int depth ;
	int index ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	// 既に作成済みの場合は一度削除する
	if( GD3D9.Device.Screen.ZBufferSurface )
	{
		Direct3D9_ObjectRelease( GD3D9.Device.Screen.ZBufferSurface ) ;
		GD3D9.Device.Screen.ZBufferSurface = NULL ;
	}

	DXST_LOGFILE_ADDUTF16LE( "\x3a\xff\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x92\x30\x5c\x4f\x10\x62\x57\x30\x7e\x30\x59\x30\x2e\x00\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"Ｚバッファを作成します.... " @*/ ) ;

	// Ｚバッファのサイズを決定
	if( GD3D9.Setting.UserZBufferSizeSet == FALSE ||
		GD3D9.Device.Screen.ZBufferSizeX == 0 ||
		GD3D9.Device.Screen.ZBufferSizeY == 0 )
	{
		for( w = 1 ; w < ( DWORD )GSYS.Screen.MainScreenSizeX ; w <<= 1 ){}
		for( h = 1 ; h < ( DWORD )GSYS.Screen.MainScreenSizeY ; h <<= 1 ){}
		GD3D9.Device.Screen.ZBufferSizeX = ( int )w ;
		GD3D9.Device.Screen.ZBufferSizeY = ( int )h ;
	}
	else
	{
		w = ( DWORD )GD3D9.Device.Screen.ZBufferSizeX ;
		h = ( DWORD )GD3D9.Device.Screen.ZBufferSizeY ;
	}

	// Ｚバッファのビット深度を決定
	if( GD3D9.Setting.UserZBufferBitDepthSet == FALSE ||
		GD3D9.Device.Screen.ZBufferBitDepth == 0  )
	{
		depth = DEFAULT_ZBUFFER_BITDEPTH ;
		GD3D9.Device.Screen.ZBufferBitDepth = DEFAULT_ZBUFFER_BITDEPTH ;
	}
	else
	{
		depth = GD3D9.Device.Screen.ZBufferBitDepth ;
	}
	switch( depth )
	{
	default :
	case 16 : index = ZBUFFER_FORMAT_16BIT ; break ;
	case 24 : index = ZBUFFER_FORMAT_24BIT ; break ;
	case 32 : index = ZBUFFER_FORMAT_32BIT ; break ;
	}

	// 作成
	hr = Direct3DDevice9_CreateDepthStencilSurface(
		w,
		h,
		GD3D9.Device.Caps.ZBufferFormat[ index ],
		( D_D3DMULTISAMPLE_TYPE )GSYS.Setting.FSAAMultiSampleCount,
		( DWORD )GSYS.Setting.FSAAMultiSampleQuality,
		TRUE,
		&GD3D9.Device.Screen.ZBufferSurface,
		NULL
	) ;
	if( FAILED( hr ) )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x3a\xff\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Ｚバッファの作成に失敗しました\n" @*/ ) ;
		return -1 ;
	}

	// デバイスにセット
	Direct3DDevice9_SetDepthStencilSurface( GD3D9.Device.Screen.ZBufferSurface ) ;

	// クリア
	Direct3DDevice9_Clear( 0, NULL, D_D3DCLEAR_ZBUFFER, 0, 1.0f, 0 ) ;

	DXST_LOGFILE_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"成功\n" @*/ ) ;

	// 終了
	return 0 ;
}



















// Direct3D9 の画像関係


// D3DFORMAT のフォーマットに合わせたカラーフォーマット情報を取得する
extern COLORDATA *Graphics_D3D9_GetD3DFormatColorData( D_D3DFORMAT Format )
{
	static int InitializeFlag = FALSE ;
	static COLORDATA R8G8B8_ColorData,   A8R8G8B8_ColorData,    X8R8G8B8_ColorData ;
	static COLORDATA R5G6B5_ColorData,   X1R5G5B5_ColorData,    A1R5G5B5_ColorData ;
	static COLORDATA A4R4G4B4_ColorData, R3G3B2_ColorData,      A8R3G3B2_ColorData ;
	static COLORDATA X4R4G4B4_ColorData, A2B10G10R10_ColorData, A8B8G8R8_ColorData ;
	static COLORDATA X8B8G8R8_ColorData, A2R10G10B10_ColorData ;
	static COLORDATA A16B16G16R16I_ColorData ;
	static COLORDATA A16B16G16R16F_ColorData ;
	static COLORDATA A32B32G32R32F_ColorData ;
	static COLORDATA ONE_I8_ColorData ;
	static COLORDATA ONE_I16_ColorData ;
	static COLORDATA ONE_F16_ColorData ;
	static COLORDATA ONE_F32_ColorData ;
	static COLORDATA TWO_I8_ColorData ;
	static COLORDATA TWO_I16_ColorData ;
	static COLORDATA TWO_F16_ColorData ;
	static COLORDATA TWO_F32_ColorData ;

	if( InitializeFlag == FALSE )
	{
		InitializeFlag = TRUE ;
		NS_CreateColorData( &R8G8B8_ColorData,         24, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000 ) ;
		NS_CreateColorData( &A8R8G8B8_ColorData,       32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000 ) ;
		NS_CreateColorData( &X8R8G8B8_ColorData,       32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000 ) ;
		NS_CreateColorData( &R5G6B5_ColorData,         16, 0x0000f800, 0x000007e0, 0x0000001f, 0x00000000 ) ;
		NS_CreateColorData( &X1R5G5B5_ColorData,       16, 0x00007c00, 0x000003e0, 0x0000001f, 0x00000000 ) ;
		NS_CreateColorData( &A1R5G5B5_ColorData,       16, 0x00007c00, 0x000003e0, 0x0000001f, 0x00008000 ) ;
		NS_CreateColorData( &A4R4G4B4_ColorData,       16, 0x00000f00, 0x000000f0, 0x0000000f, 0x0000f000 ) ;
		NS_CreateColorData( &R3G3B2_ColorData,          8, 0x000000e0, 0x0000001c, 0x00000003, 0x00000000 ) ;
		NS_CreateColorData( &A8R3G3B2_ColorData,       16, 0x000000e0, 0x0000001c, 0x00000003, 0x0000ff00 ) ;
		NS_CreateColorData( &X4R4G4B4_ColorData,       16, 0x00000f00, 0x000000f0, 0x0000000f, 0x00000000 ) ;
		NS_CreateColorData( &A2B10G10R10_ColorData,    32, 0x000003ff, 0x000ffc00, 0x3ff00000, 0xc0000000 ) ;
		NS_CreateColorData( &A8B8G8R8_ColorData,       32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000 ) ;
		NS_CreateColorData( &X8B8G8R8_ColorData,       32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0x00000000 ) ;
		NS_CreateColorData( &A2R10G10B10_ColorData,    32, 0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000 ) ;
		NS_CreateColorData( &A16B16G16R16I_ColorData,   0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 4, 16, FALSE ) ;
		NS_CreateColorData( &A16B16G16R16F_ColorData,   0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 4, 16,  TRUE ) ;
		NS_CreateColorData( &A32B32G32R32F_ColorData,   0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 4, 32,  TRUE ) ;
		NS_CreateColorData( &ONE_I8_ColorData,	        0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 1,  8, FALSE ) ;
		NS_CreateColorData( &ONE_I16_ColorData,	        0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 1, 16, FALSE ) ;
		NS_CreateColorData( &ONE_F16_ColorData,	        0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 1, 16,  TRUE ) ;
		NS_CreateColorData( &ONE_F32_ColorData,	        0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 1, 32,  TRUE ) ;
		NS_CreateColorData( &TWO_I8_ColorData,	        0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 2,  8, FALSE ) ;
		NS_CreateColorData( &TWO_I16_ColorData,	        0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 2, 16, FALSE ) ;
		NS_CreateColorData( &TWO_F16_ColorData,	        0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 2, 16,  TRUE ) ;
		NS_CreateColorData( &TWO_F32_ColorData,	        0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 2, 32,  TRUE ) ;
	}

	switch( Format )
	{
	case D_D3DFMT_R8G8B8 :        return &R8G8B8_ColorData ;
	case D_D3DFMT_A8R8G8B8 :      return &A8R8G8B8_ColorData ;
	case D_D3DFMT_X8R8G8B8 :      return &X8R8G8B8_ColorData ;
	case D_D3DFMT_R5G6B5 :        return &R5G6B5_ColorData ;
	case D_D3DFMT_X1R5G5B5 :      return &X1R5G5B5_ColorData ;
	case D_D3DFMT_A1R5G5B5 :      return &A1R5G5B5_ColorData ;
	case D_D3DFMT_A4R4G4B4 :      return &A4R4G4B4_ColorData ;
	case D_D3DFMT_R3G3B2 :        return &R3G3B2_ColorData ;
	case D_D3DFMT_A8R3G3B2 :      return &A8R3G3B2_ColorData ;
	case D_D3DFMT_X4R4G4B4 :      return &X4R4G4B4_ColorData ;
	case D_D3DFMT_A2B10G10R10 :   return &A2B10G10R10_ColorData ;
	case D_D3DFMT_A8B8G8R8 :      return &A8B8G8R8_ColorData ;
	case D_D3DFMT_X8B8G8R8 :      return &X8B8G8R8_ColorData ;
	case D_D3DFMT_A2R10G10B10 :   return &A2R10G10B10_ColorData ;
	case D_D3DFMT_A16B16G16R16 :  return &A16B16G16R16I_ColorData ;
	case D_D3DFMT_A16B16G16R16F : return &A16B16G16R16F_ColorData ;
	case D_D3DFMT_A32B32G32R32F : return &A32B32G32R32F_ColorData ;
	case D_D3DFMT_L8 :            return &ONE_I8_ColorData ;
	case D_D3DFMT_L16 :           return &ONE_I16_ColorData ;
	case D_D3DFMT_R16F :          return &ONE_F16_ColorData ;
	case D_D3DFMT_R32F :          return &ONE_F32_ColorData ;
	case D_D3DFMT_A8L8 :          return &TWO_I8_ColorData ;
	case D_D3DFMT_V8U8 :          return &TWO_I8_ColorData ;
	case D_D3DFMT_G16R16 :        return &TWO_I16_ColorData ;
	case D_D3DFMT_G16R16F :       return &TWO_F16_ColorData ;
	case D_D3DFMT_G32R32F :       return &TWO_F32_ColorData ;
	}
	return NULL ;
}


// 描画先用テクスチャと描画用テクスチャが分かれている場合に、描画用テクスチャに描画先用テクスチャの内容を反映する
extern	int		Graphics_D3D9_UpdateDrawTexture( IMAGEDATA_ORIG_HARD_TEX *OrigTex, int TargetSurface )
{
	Direct3DDevice9_StretchRect(
		OrigTex->PF->D3D9.RenderTargetSurface,
		NULL,
		OrigTex->PF->D3D9.Surface[ TargetSurface ],
		NULL,
		D_D3DTEXF_NONE ) ;

	// 終了
	return 0 ;
}

// 基本イメージのフォーマットを DX_BASEIMAGE_FORMAT_NORMAL に変換する必要があるかどうかをチェックする
// ( TRUE:変換する必要がある  FALSE:変換する必要は無い )
extern	int		Graphics_D3D9_CheckRequiredNormalImageConv_BaseImageFormat_PF(
	IMAGEDATA_ORIG *Orig,
	int             RgbBaseImageFormat,
	int            *RequiredRgbBaseImageConvFlag,
	int             AlphaBaseImageFormat,
	int            *RequiredAlphaBaseImageConvFlag
)
{
	int RgbConvFlag   = FALSE ;
	int AlphaConvFlag = FALSE ;

	if( Orig->FormatDesc.TextureFlag )
	{
		if( ( RgbBaseImageFormat == DX_BASEIMAGE_FORMAT_DXT1 && GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ] != D_D3DFMT_DXT1 ) ||
			( RgbBaseImageFormat == DX_BASEIMAGE_FORMAT_DXT2 && GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ] != D_D3DFMT_DXT2 ) ||
			( RgbBaseImageFormat == DX_BASEIMAGE_FORMAT_DXT3 && GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ] != D_D3DFMT_DXT3 ) ||
			( RgbBaseImageFormat == DX_BASEIMAGE_FORMAT_DXT4 && GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ] != D_D3DFMT_DXT4 ) ||
			( RgbBaseImageFormat == DX_BASEIMAGE_FORMAT_DXT5 && GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ] != D_D3DFMT_DXT5 ) )
		{
			RgbConvFlag = TRUE ;
		}

		if( AlphaBaseImageFormat >= 0 )
		{
			if( ( AlphaBaseImageFormat == DX_BASEIMAGE_FORMAT_DXT1 && GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ] != D_D3DFMT_DXT1 ) ||
				( AlphaBaseImageFormat == DX_BASEIMAGE_FORMAT_DXT2 && GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ] != D_D3DFMT_DXT2 ) ||
				( AlphaBaseImageFormat == DX_BASEIMAGE_FORMAT_DXT3 && GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ] != D_D3DFMT_DXT3 ) ||
				( AlphaBaseImageFormat == DX_BASEIMAGE_FORMAT_DXT4 && GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ] != D_D3DFMT_DXT4 ) ||
				( AlphaBaseImageFormat == DX_BASEIMAGE_FORMAT_DXT5 && GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ] != D_D3DFMT_DXT5 ) )
			{
				AlphaConvFlag = TRUE ;
			}
		}
	}
	else
	{
		if( RgbBaseImageFormat == DX_BASEIMAGE_FORMAT_DXT1 ||
			RgbBaseImageFormat == DX_BASEIMAGE_FORMAT_DXT2 ||
			RgbBaseImageFormat == DX_BASEIMAGE_FORMAT_DXT3 ||
			RgbBaseImageFormat == DX_BASEIMAGE_FORMAT_DXT4 ||
			RgbBaseImageFormat == DX_BASEIMAGE_FORMAT_DXT5 )
		{
			RgbConvFlag = TRUE ;
		}

		if( AlphaBaseImageFormat >= 0 )
		{
			if( AlphaBaseImageFormat == DX_BASEIMAGE_FORMAT_DXT1 ||
				AlphaBaseImageFormat == DX_BASEIMAGE_FORMAT_DXT2 ||
				AlphaBaseImageFormat == DX_BASEIMAGE_FORMAT_DXT3 ||
				AlphaBaseImageFormat == DX_BASEIMAGE_FORMAT_DXT4 ||
				AlphaBaseImageFormat == DX_BASEIMAGE_FORMAT_DXT5 )
			{
				AlphaConvFlag = TRUE ;
			}
		}
	}

	if( RequiredRgbBaseImageConvFlag != NULL )
	{
		*RequiredRgbBaseImageConvFlag = RgbConvFlag ;
	}

	if( RequiredAlphaBaseImageConvFlag != NULL )
	{
		*RequiredAlphaBaseImageConvFlag = AlphaConvFlag ;
	}

	// 正常終了
	return 0 ;
}



























// Direct3DDevice の初期化関係

// 指定の深度ステンシルフォーマットが使用可能かどうかを調べる
// 戻り値   -1:使用できない    0:使用できる
static int CheckDeatpStencilMatchRapper( DWORD Adapter, D_D3DDEVTYPE DeviceType, D_D3DFORMAT AdapterFormat, D_D3DFORMAT RenderTargetFormat, D_D3DFORMAT DepthStencilFormat )
{
	if( Direct3D9_CheckDeviceFormat( Adapter, DeviceType, AdapterFormat, D_D3DUSAGE_DEPTHSTENCIL, D_D3DRTYPE_SURFACE, DepthStencilFormat ) != D_D3D_OK )
	{
		return -1 ;
	}

	if( Direct3D9_CheckDepthStencilMatch( Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat ) != D_D3D_OK )
	{
		return -1 ;
	}

	return 0 ;
}

// Direct3DDevice9 オブジェクトの作成
extern int Graphics_D3D9_Device_Create()
{
	HRESULT hr ;
	const char *String = NULL ;
	int ErrorRet = -1 ;

	SETUP_WIN_API

	if( Direct3D9_IsValid() == 0 ) return -1 ;

	// 既に作成済みの場合は一度削除する
	Direct3DDevice9_Release() ;
	GD3D9.Device.DrawInfo.BeginSceneFlag = FALSE ;

	// シェーダー２．０を使えない環境ではフルスクリーンアンチエイリアスを正常に使える可能性も低いので
	// シェーダー２．０を使えない環境ではフルスクリーンアンチエイリアスを無効にする
	if( NS_GetValidShaderVersion() < 200 )
	{
		GSYS.Setting.FSAAMultiSampleCount = D_D3DMULTISAMPLE_NONE ;
		GSYS.Setting.FSAAMultiSampleQuality = 0 ;
	}

	// デバイスの作成
	if( Direct3D9_CreateDevice() < 0 )
	{
		// 失敗したら画面モードを変更して試す
		GSYS.Screen.MainScreenColorBitDepth = 32 ;
		SetMemImgDefaultColorType( GSYS.Screen.MainScreenColorBitDepth == 32 ? 1 : 0 ) ;
		if( Direct3D9_CreateDevice() < 0 )
		{
			Graphics_Screen_SetMainScreenSize( 800, 600 ) ;
			if( Direct3D9_CreateDevice() < 0 )
			{
				Graphics_Screen_SetMainScreenSize( 1024, 768 ) ;
				if( Direct3D9_CreateDevice() < 0 )
				{
					goto ERR ;
				}
			}
		}
	}

	// VSyncの前にSleepする時間を取得する
	DirectDraw7_WaitVSyncInitialize() ;

	// 固定機能パイプラインに戻す
	GD3D9.Device.State.SetVertexShader       = NULL ;
	GD3D9.Device.State.SetVertexDeclaration  = NULL ;
	GD3D9.Device.State.SetPixelShader        = NULL ;
	GD3D9.Device.State.SetFVF                = 0 ;
	GD3D9.Device.State.SetIndexBuffer        = NULL ;
	GD3D9.Device.State.SetVertexBuffer       = NULL ;
	GD3D9.Device.State.SetVertexBufferStride = 0 ;

	// バックバッファのアドレスを取得しておく
	hr = Direct3DDevice9_GetBackBuffer( 0, 0, D_D3DBACKBUFFER_TYPE_MONO, &GD3D9.Device.Screen.BackBufferSurface ) ;

	DXST_LOGFILE_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"成功\n" @*/ ) ;

	// フルスクリーンモードの場合はデスクトップ上の位置を取得しておく
	if( NS_GetWindowModeFlag() == FALSE )
	{
		HMONITOR      hMonitor ;
		MONITORINFOEX MonitorInfoEx ;

		hMonitor = ( HMONITOR )Direct3D9_GetAdapterMonitor( GSYS.Screen.ValidUseDisplayIndex ? ( DWORD )GSYS.Screen.UseDisplayIndex : D_D3DADAPTER_DEFAULT ) ;

		_MEMSET( &MonitorInfoEx, 0, sizeof( MonitorInfoEx ) ) ;
		MonitorInfoEx.cbSize = sizeof( MonitorInfoEx ) ;
		WinAPIData.Win32Func.GetMonitorInfoWFunc( hMonitor, &MonitorInfoEx ) ;

		GSYS.Screen.FullScreenDesktopRect = MonitorInfoEx.rcMonitor ;
	}

	// 成功した場合はデバイス情報を取得する
	Direct3DDevice9_GetDeviceCaps( &GD3D9.Device.Caps.DeviceCaps ) ;

//	GD3D9.Device.Caps.DeviceCaps.VertexShaderVersion = 0x200 ;
//	GD3D9.Device.Caps.DeviceCaps.PixelShaderVersion  = 0x200 ;
//	GD3D9.Device.Caps.DeviceCaps.VertexShaderVersion = 0x000 ;
//	GD3D9.Device.Caps.DeviceCaps.PixelShaderVersion  = 0x000 ;

	// 使用できるテクスチャステージの数を保存
	GD3D9.Device.Caps.MaxTextureBlendStages = ( int )GD3D9.Device.Caps.DeviceCaps.MaxTextureBlendStages ;

	// 頂点カラーはBGRAカラータイプを使用する
	GSYS.HardInfo.UseVertexColorBGRAFormat = TRUE ;

	// 頂点座標タイプは DX_DRAWFLOATCOORDTYPE_DIRECT3D9
	GSYS.HardInfo.DrawFloatCoordType = DX_DRAWFLOATCOORDTYPE_DIRECT3D9 ;

	// OggTheora再生用の画像に描画可能画像を使用する
	GSYS.HardInfo.OggTheoraUseDrawValidGraph = TRUE ;

	// 同時にレンダリングできるターゲットの数を保存
	GSYS.HardInfo.RenderTargetNum = ( int )GD3D9.Device.Caps.DeviceCaps.NumSimultaneousRTs ;
	if( GSYS.HardInfo.RenderTargetNum > DX_RENDERTARGET_COUNT )
	{
		GSYS.HardInfo.RenderTargetNum = DX_RENDERTARGET_COUNT ;
	}

	// テクスチャステージのテンポラリレジスタが使用できるかどうかを保存
	GD3D9.Device.Caps.ValidTexTempRegFlag = ( GD3D9.Device.Caps.DeviceCaps.PrimitiveMiscCaps & D_D3DPMISCCAPS_TSSARGTEMP ) != 0 ? TRUE : FALSE ;

	// シェーダーを使わない設定の場合は必ず無効
	if( GD3D9.Setting.NonUseVertexHardwareProcess )
	{
		GD3D9.Device.Shader.ValidVertexShader = FALSE ;
		GD3D9.Device.Shader.ValidPixelShader = FALSE ;
		GD3D9.Device.Shader.ValidVertexShader_SM3 = FALSE ;
		GD3D9.Device.Shader.ValidPixelShader_SM3 = FALSE ;
	}
	else
	{
		// 頂点シェーダーが使用可能かどうかを保存
		GD3D9.Device.Shader.ValidVertexShader = /* GD3D9.Device.Caps.VertexHardwareProcess && */ ( GD3D9.Device.Caps.DeviceCaps.VertexShaderVersion & 0xffff ) >= 0x0200 ;

		// ピクセルシェーダーが使用可能かどうかを保存
		GD3D9.Device.Shader.ValidPixelShader = /* GD3D9.Device.Caps.VertexHardwareProcess && */ ( GD3D9.Device.Caps.DeviceCaps.PixelShaderVersion & 0xffff ) >= 0x0200 ;

		// SM3.0の頂点シェーダーが使用可能かどうかを保存
		GD3D9.Device.Shader.ValidVertexShader_SM3 = /* GD3D9.Device.Caps.VertexHardwareProcess && */ ( GD3D9.Device.Caps.DeviceCaps.VertexShaderVersion & 0xffff ) >= 0x0300 ;

		// SM3.0のピクセルシェーダーが使用可能かどうかを保存
		GD3D9.Device.Shader.ValidPixelShader_SM3 = /* GD3D9.Device.Caps.VertexHardwareProcess && */ ( GD3D9.Device.Caps.DeviceCaps.PixelShaderVersion & 0xffff ) >= 0x0300 ;

		// SM3.0 はピクセルシェーダーも頂点シェーダーも対応していない場合は使用できない
		if( GD3D9.Device.Shader.ValidVertexShader_SM3 == FALSE ||
			GD3D9.Device.Shader.ValidPixelShader_SM3 == FALSE )
		{
			GD3D9.Device.Shader.ValidVertexShader_SM3 = FALSE ;
			GD3D9.Device.Shader.ValidPixelShader_SM3  = FALSE ;
		}
	}

	// D3DBLENDOP_ADD 以外が使用できるかどうかを取得する
	GD3D9.Device.Caps.ValidDestBlendOp = ( GD3D9.Device.Caps.DeviceCaps.PrimitiveMiscCaps & D_D3DPMISCCAPS_BLENDOP ) == 0 ? FALSE : TRUE ;

	// 減算ブレンドが可能かどうかをセットする
	GSYS.HardInfo.ValidSubBlend = GD3D9.Device.Caps.ValidDestBlendOp ;

	// テクスチャのサイズが２のｎ乗である必要があるかどうかを取得する
	GSYS.HardInfo.TextureSizePow2 = ( GD3D9.Device.Caps.DeviceCaps.TextureCaps & D_D3DPTEXTURECAPS_POW2 ) != 0 ? TRUE : FALSE ;

	// テクスチャが正方形である必要があるかどうかを取得する
	GSYS.HardInfo.TextureSquareOnly = ( GD3D9.Device.Caps.DeviceCaps.TextureCaps & D_D3DPTEXTURECAPS_SQUAREONLY ) != 0 ? TRUE : FALSE ;

	// 条件付でテクスチャのサイズが２のｎ乗でなくても大丈夫かどうかを取得する
	GSYS.HardInfo.TextureSizeNonPow2Conditional = ( GD3D9.Device.Caps.DeviceCaps.TextureCaps & D_D3DPTEXTURECAPS_NONPOW2CONDITIONAL ) != 0 ? TRUE : FALSE ;

	// テクスチャサイズが設定されていない場合は設定する
	if( GD3D9.Device.Caps.DeviceCaps.MaxTextureWidth  == 0 ) GD3D9.Device.Caps.DeviceCaps.MaxTextureWidth  = DEFAULT_MAXTEXTURE_SIZE ;
	if( GD3D9.Device.Caps.DeviceCaps.MaxTextureHeight == 0 ) GD3D9.Device.Caps.DeviceCaps.MaxTextureHeight = DEFAULT_MAXTEXTURE_SIZE ;

	// テクスチャの最大幅と最大高さを保存する
	GSYS.HardInfo.MaxTextureWidth  = ( int )GD3D9.Device.Caps.DeviceCaps.MaxTextureWidth ;
	GSYS.HardInfo.MaxTextureHeight = ( int )GD3D9.Device.Caps.DeviceCaps.MaxTextureHeight ;

	// サイズの小さいほうを最大サイズとする
	GSYS.HardInfo.MaxTextureSize = ( int )( GD3D9.Device.Caps.DeviceCaps.MaxTextureWidth < GD3D9.Device.Caps.DeviceCaps.MaxTextureHeight ? GD3D9.Device.Caps.DeviceCaps.MaxTextureWidth : GD3D9.Device.Caps.DeviceCaps.MaxTextureHeight ) ;

	// 一度に描画できる最大プリミティブ数を取得する
	GSYS.HardInfo.MaxPrimitiveCount = ( int )GD3D9.Device.Caps.DeviceCaps.MaxPrimitiveCount ;

	// 一度に使用することの出来る最大頂点数を取得する
	GSYS.HardInfo.MaxVertexIndex = ( int )GD3D9.Device.Caps.DeviceCaps.MaxVertexIndex ;

	// 頂点カラーを使用するかどうかの値を初期化
	GD3D9.Device.State.MaterialUseVertexDiffuseColor = 0xff ;
	GD3D9.Device.State.MaterialUseVertexSpecularColor = 0xff ;

	// Graphics_D3D9_DrawPreparation を行うべきフラグを立てる
	GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	{
		D_D3DADAPTER_IDENTIFIER9 Iden ;

		Direct3D9_GetAdapterIdentifier( D_D3DADAPTER_DEFAULT, 0, &Iden ) ;
		DXST_LOGFILEFMT_ADDA(( "Driver:%s  Description:%s", Iden.Driver, Iden.Description )) ;
	}

	// 各種フォーマットを調べる
	{
		D_D3DSURFACE_DESC SurfaceDesc ;
		DWORD UseAdapterNumber ;

		// 使用するディスプレイアダプターを決定
		UseAdapterNumber = GSYS.Screen.ValidUseDisplayIndex ? ( DWORD )GSYS.Screen.UseDisplayIndex : D_D3DADAPTER_DEFAULT ;

		// 画面のフォーマットの取得
		Direct3DSurface9_GetDesc( GD3D9.Device.Screen.BackBufferSurface, &SurfaceDesc ) ;
		GD3D9.Device.Caps.ScreenFormat = SurfaceDesc.Format ;
		switch( GD3D9.Device.Caps.ScreenFormat )
		{
		case D_D3DFMT_X8R8G8B8 : String = "D3DFMT_X8R8G8B8" ; break ;
		case D_D3DFMT_R5G6B5 :   String = "D3DFMT_R5G6B5" ;   break ;
		case D_D3DFMT_X1R5G5B5 : String = "D3DFMT_X1R5G5B5" ; break ;
		default:
			DXST_LOGFILEFMT_ADDA(( "\x83\x6f\x83\x62\x83\x4e\x83\x6f\x83\x62\x83\x74\x83\x40\x82\xaa\x91\xce\x89\x9e\x82\xb5\x82\xc4\x82\xa2\x82\xc8\x82\xa2\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67 %d \x82\xc5\x82\xb7"/*@ "バックバッファが対応していないフォーマット %d です" @*/, GD3D9.Device.Caps.ScreenFormat )) ;
			break ;
		}
		DXST_LOGFILEFMT_ADDA(( "\x89\xe6\x96\xca\x82\xcc\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "画面のフォーマットは %s です" @*/, String )) ;

		// 16ビットＺバッファフォーマットの設定
		GD3D9.Device.Caps.ZBufferFormat[ ZBUFFER_FORMAT_16BIT ] = D_D3DFMT_D16 ;
		String = "D3DFMT_D16" ;
		DXST_LOGFILEFMT_ADDA(( "16bit Z\x83\x6f\x83\x62\x83\x74\x83\x40\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "16bit Zバッファフォーマットは %s です" @*/, String )) ;

		// 24ビットＺバッファフォーマットの選定
		if( CheckDeatpStencilMatchRapper( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, GD3D9.Device.Caps.ScreenFormat, D_D3DFMT_D24X8 ) == 0 )
		{
			GD3D9.Device.Caps.ZBufferFormat[ ZBUFFER_FORMAT_24BIT ] = D_D3DFMT_D24X8 ;
			String = "D3DFMT_D24X8" ;
		}
		else
		if( CheckDeatpStencilMatchRapper( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, GD3D9.Device.Caps.ScreenFormat, D_D3DFMT_D32 ) == 0 )
		{
			GD3D9.Device.Caps.ZBufferFormat[ ZBUFFER_FORMAT_24BIT ] = D_D3DFMT_D32 ;
			String = "D3DFMT_D32" ;
		}
		else
		{
			GD3D9.Device.Caps.ZBufferFormat[ ZBUFFER_FORMAT_24BIT ] = D_D3DFMT_D16 ;
			String = "D3DFMT_D16" ;
		}
		DXST_LOGFILEFMT_ADDA(( "24bit Z\x83\x6f\x83\x62\x83\x74\x83\x40\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "24bit Zバッファフォーマットは %s です" @*/, String )) ;

		// 32ビットＺバッファフォーマットの選定
		if( CheckDeatpStencilMatchRapper( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, GD3D9.Device.Caps.ScreenFormat, D_D3DFMT_D32 ) == 0 )
		{
			GD3D9.Device.Caps.ZBufferFormat[ ZBUFFER_FORMAT_32BIT ] = D_D3DFMT_D32 ;
			String = "D3DFMT_D32" ;
		}
		else
		if( CheckDeatpStencilMatchRapper( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, GD3D9.Device.Caps.ScreenFormat, D_D3DFMT_D24X8 ) == 0 )
		{
			GD3D9.Device.Caps.ZBufferFormat[ ZBUFFER_FORMAT_32BIT ] = D_D3DFMT_D24X8 ;
			String = "D3DFMT_D24X8" ;
		}
		else
		if( CheckDeatpStencilMatchRapper( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, GD3D9.Device.Caps.ScreenFormat, D_D3DFMT_D16 ) == 0 )
		{
			GD3D9.Device.Caps.ZBufferFormat[ ZBUFFER_FORMAT_32BIT ] = D_D3DFMT_D16 ;
			String = "D3DFMT_D16" ;
		}
		DXST_LOGFILEFMT_ADDA(( "32bit Z\x83\x6f\x83\x62\x83\x74\x83\x40\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "32bit Zバッファフォーマットは %s です" @*/, String )) ;

		// 16ビットカラーフォーマットの選定
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_R5G6B5 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_RGB16 ] = D_D3DFMT_R5G6B5 ;
			String = "D3DFMT_R5G6B5" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_X1R5G5B5 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_RGB16 ] = D_D3DFMT_X1R5G5B5 ;
			String = "D3DFMT_X1R5G5B5" ;
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x7f\x4f\x28\x75\x67\x30\x4d\x30\x8b\x30\x20\x00\x31\x00\x36\x00\x62\x00\x69\x00\x74\x00\x20\x00\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x4c\x30\x8b\x89\x64\x30\x4b\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"使用できる 16bit カラーフォーマットが見つかりませんでした\n" @*/ ) ;
			goto ERR ;
		}
		DXST_LOGFILEFMT_ADDA(( "16bit \x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "16bit カラーフォーマットは %s です" @*/, String )) ;

		// 32ビットカラーフォーマットの選定
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_X8R8G8B8 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_RGB32 ] = D_D3DFMT_X8R8G8B8 ;
			String = "D3DFMT_X8R8G8B8" ;
		}
		else
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_RGB32 ] = GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_RGB16 ] ;
		}
		DXST_LOGFILEFMT_ADDA(( "32bit \x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "32bit カラーフォーマットは %s です" @*/, String )) ;

		// アルファ付き 16ビットカラーフォーマットの選定
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_A4R4G4B4 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHA_RGB16 ] = D_D3DFMT_A4R4G4B4 ;
			String = "D3DFMT_A4R4G4B4" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_A8R8G8B8 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHA_RGB16 ] = D_D3DFMT_A8R8G8B8 ;
			String = "D3DFMT_A8R8G8B8" ;
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x7f\x4f\x28\x75\x67\x30\x4d\x30\x8b\x30\xa2\x30\xeb\x30\xd5\x30\xa1\x30\xd8\x4e\x4d\x30\x20\x00\x31\x00\x36\x00\x62\x00\x69\x00\x74\x00\x20\x00\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x4c\x30\x8b\x89\x64\x30\x4b\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"使用できるアルファ付き 16bit カラーフォーマットが見つかりませんでした\n" @*/ ) ;
			goto ERR ;
		}
		DXST_LOGFILEFMT_ADDA(( "\x83\x41\x83\x8b\x83\x74\x83\x40\x95\x74\x82\xab 16bit \x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "アルファ付き 16bit カラーフォーマットは %s です" @*/, String )) ;

		// アルファ付き 32ビットカラーフォーマットの選定
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_A8R8G8B8 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHA_RGB32 ] = D_D3DFMT_A8R8G8B8 ;
			String = "D3DFMT_A8R8G8B8" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_A4R4G4B4 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHA_RGB32 ] = D_D3DFMT_A4R4G4B4 ;
			String = "D3DFMT_A4R4G4B4" ;
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x7f\x4f\x28\x75\x67\x30\x4d\x30\x8b\x30\xa2\x30\xeb\x30\xd5\x30\xa1\x30\xd8\x4e\x4d\x30\x20\x00\x33\x00\x32\x00\x62\x00\x69\x00\x74\x00\x20\x00\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x4c\x30\x8b\x89\x64\x30\x4b\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"使用できるアルファ付き 32bit カラーフォーマットが見つかりませんでした\n" @*/ ) ;
			goto ERR ;
		}
		DXST_LOGFILEFMT_ADDA(( "\x83\x41\x83\x8b\x83\x74\x83\x40\x95\x74\x82\xab 32bit \x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "アルファ付き 32bit カラーフォーマットは %s です" @*/, String )) ;

		// アルファテスト用 16ビットカラーフォーマットの選定
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_A1R5G5B5 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHATEST_RGB16 ] = D_D3DFMT_A1R5G5B5 ;
			String = "D3DFMT_A1R5G5B5" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_A8R8G8B8 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHATEST_RGB16 ] = D_D3DFMT_A8R8G8B8 ;
			String = "D3DFMT_A8R8G8B8" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_A4R4G4B4 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHATEST_RGB16 ] = D_D3DFMT_A4R4G4B4 ;
			String = "D3DFMT_A4R4G4B4" ;
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x7f\x4f\x28\x75\x67\x30\x4d\x30\x8b\x30\xa2\x30\xeb\x30\xd5\x30\xa1\x30\xc6\x30\xb9\x30\xc8\x30\x28\x75\x20\x00\x31\x00\x36\x00\x62\x00\x69\x00\x74\x00\x20\x00\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x4c\x30\x8b\x89\x64\x30\x4b\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"使用できるアルファテスト用 16bit カラーフォーマットが見つかりませんでした\n" @*/ ) ;
			goto ERR ;
		}
		DXST_LOGFILEFMT_ADDA(( "\x83\x41\x83\x8b\x83\x74\x83\x40\x83\x65\x83\x58\x83\x67\x97\x70 16bit \x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "アルファテスト用 16bit カラーフォーマットは %s です" @*/, String )) ;

		// アルファテスト用 32ビットカラーフォーマットの選定
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_A8R8G8B8 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHATEST_RGB32 ] = D_D3DFMT_A8R8G8B8 ;
			String = "D3DFMT_A8R8G8B8" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_A1R5G5B5 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHATEST_RGB32 ] = D_D3DFMT_A1R5G5B5 ;
			String = "D3DFMT_A1R5G5B5" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_A4R4G4B4 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHATEST_RGB32 ] = D_D3DFMT_A4R4G4B4 ;
			String = "D3DFMT_A4R4G4B4" ;
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x7f\x4f\x28\x75\x67\x30\x4d\x30\x8b\x30\xa2\x30\xeb\x30\xd5\x30\xa1\x30\xc6\x30\xb9\x30\xc8\x30\x28\x75\x20\x00\x33\x00\x32\x00\x62\x00\x69\x00\x74\x00\x20\x00\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x4c\x30\x8b\x89\x64\x30\x4b\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"使用できるアルファテスト用 32bit カラーフォーマットが見つかりませんでした\n" @*/ ) ;
			goto ERR ;
		}
		DXST_LOGFILEFMT_ADDA(( "\x83\x41\x83\x8b\x83\x74\x83\x40\x83\x65\x83\x58\x83\x67\x97\x70 32bit \x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "アルファテスト用 32bit カラーフォーマットは %s です" @*/, String )) ;

		// パレットカラーフォーマットのセット
		if( GSYS.Screen.MainScreenColorBitDepth == 16 )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_PAL4           ] = GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_RGB16 ] ;
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_PAL8           ] = GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_RGB16 ] ;
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHA_PAL4     ] = GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHA_RGB16 ] ;
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHA_PAL8     ] = GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHA_RGB16 ] ;
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHATEST_PAL4 ] = GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHATEST_RGB16 ] ;
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHATEST_PAL8 ] = GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHATEST_RGB16 ] ;
		}
		else
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_PAL4           ] = GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_RGB32 ] ;
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_PAL8           ] = GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_RGB32 ] ;
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHA_PAL4     ] = GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHA_RGB32 ] ;
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHA_PAL8     ] = GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHA_RGB32 ] ;
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHATEST_PAL4 ] = GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHATEST_RGB32 ] ;
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHATEST_PAL8 ] = GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHATEST_RGB32 ] ;
		}

		// DXT1フォーマットのチェック
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_DXT1 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DXT1 ] = D_D3DFMT_DXT1 ;
			String = "D3DFMT_DXT1 \x82\xc5\x82\xb7"/*@ "D3DFMT_DXT1 です" @*/ ;
		}
		else
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DXT1 ] = GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHATEST_RGB16 ] ;
			String = "\x8e\x67\x82\xa6\x82\xdc\x82\xb9\x82\xf1"/*@ "使えません" @*/ ;
		}
		DXST_LOGFILEFMT_ADDA(( "\x82\x63\x82\x77\x82\x73\x82\x50\x83\x65\x83\x4e\x83\x58\x83\x60\x83\x83\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd%s"/*@ "ＤＸＴ１テクスチャフォーマットは%s" @*/, String )) ;

		// DXT2フォーマットのチェック
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_DXT2 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DXT2 ] = D_D3DFMT_DXT2 ;
			String = "D3DFMT_DXT2 \x82\xc5\x82\xb7"/*@ "D3DFMT_DXT2 です" @*/ ;
		}
		else
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DXT2 ] = GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHA_RGB32 ] ;
			String = "\x8e\x67\x82\xa6\x82\xdc\x82\xb9\x82\xf1"/*@ "使えません" @*/ ;
		}
		DXST_LOGFILEFMT_ADDA(( "\x82\x63\x82\x77\x82\x73\x82\x51\x83\x65\x83\x4e\x83\x58\x83\x60\x83\x83\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd%s"/*@ "ＤＸＴ２テクスチャフォーマットは%s" @*/, String )) ;

		// DXT3フォーマットのチェック
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_DXT3 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DXT3 ] = D_D3DFMT_DXT3 ;
			String = "D3DFMT_DXT3 \x82\xc5\x82\xb7"/*@ "D3DFMT_DXT3 です" @*/ ;
		}
		else
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DXT3 ] = GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHA_RGB32 ] ;
			String = "\x8e\x67\x82\xa6\x82\xdc\x82\xb9\x82\xf1"/*@ "使えません" @*/ ;
		}
		DXST_LOGFILEFMT_ADDA(( "\x82\x63\x82\x77\x82\x73\x82\x52\x83\x65\x83\x4e\x83\x58\x83\x60\x83\x83\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd%s"/*@ "ＤＸＴ３テクスチャフォーマットは%s" @*/, String )) ;

		// DXT4フォーマットのチェック
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_DXT4 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DXT4 ] = D_D3DFMT_DXT4 ;
			String = "D3DFMT_DXT4 \x82\xc5\x82\xb7"/*@ "D3DFMT_DXT4 です" @*/ ;
		}
		else
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DXT4 ] = GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHA_RGB32 ] ;
			String = "\x8e\x67\x82\xa6\x82\xdc\x82\xb9\x82\xf1"/*@ "使えません" @*/ ;
		}
		DXST_LOGFILEFMT_ADDA(( "\x82\x63\x82\x77\x82\x73\x82\x53\x83\x65\x83\x4e\x83\x58\x83\x60\x83\x83\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd%s"/*@ "ＤＸＴ４テクスチャフォーマットは%s" @*/, String )) ;

		// DXT5フォーマットのチェック
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_DXT5 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DXT5 ] = D_D3DFMT_DXT5 ;
			String = "D3DFMT_DXT5 \x82\xc5\x82\xb7"/*@ "D3DFMT_DXT5 です" @*/ ;
		}
		else
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DXT5 ] = GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHA_RGB32 ] ;
			String = "\x8e\x67\x82\xa6\x82\xdc\x82\xb9\x82\xf1"/*@ "使えません" @*/ ;
		}
		DXST_LOGFILEFMT_ADDA(( "\x82\x63\x82\x77\x82\x73\x82\x54\x83\x65\x83\x4e\x83\x58\x83\x60\x83\x83\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd%s"/*@ "ＤＸＴ５テクスチャフォーマットは%s" @*/, String )) ;







		// ARGB整数16ビット型カラーフォーマットの選定
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_A16B16G16R16 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ABGR_I16 ] = D_D3DFMT_A16B16G16R16 ;
			String = "D3DFMT_A16B16G16R16" ;
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x7f\x4f\x28\x75\x67\x30\x4d\x30\x8b\x30\x41\x00\x42\x00\x47\x00\x52\x00\x74\x65\x70\x65\x31\x00\x36\x00\xd3\x30\xc3\x30\xc8\x30\x8b\x57\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"使用できるABGR整数16ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ABGR_I16 ] = D_D3DFMT_UNKNOWN ;
		}

		if( GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ABGR_I16 ] != D_D3DFMT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x41\x42\x47\x52\x20\x90\xae\x90\x94\x20\x31\x36\x20\x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd\x20\x25\x73\x20\x82\xc5\x82\xb7"/*@ "ABGR 整数 16 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// ARGB浮動小数点16ビット型カラーフォーマットの選定
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_A16B16G16R16F ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ABGR_F16 ] = D_D3DFMT_A16B16G16R16F ;
			String = "D3DFMT_A16B16G16R16F" ;
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x7f\x4f\x28\x75\x67\x30\x4d\x30\x8b\x30\x41\x00\x42\x00\x47\x00\x52\x00\x6e\x6d\xd5\x52\x0f\x5c\x70\x65\xb9\x70\x31\x00\x36\x00\xd3\x30\xc3\x30\xc8\x30\x8b\x57\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"使用できるABGR浮動小数点16ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ABGR_F16 ] = D_D3DFMT_UNKNOWN ;
		}

		if( GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ABGR_F16 ] != D_D3DFMT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x41\x42\x47\x52\x20\x95\x82\x93\xae\x8f\xac\x90\x94\x93\x5f\x20\x31\x36\x20\x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd\x20\x25\x73\x20\x82\xc5\x82\xb7"/*@ "ABGR 浮動小数点 16 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// ARGB浮動小数点32ビット型カラーフォーマットの選定
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_A32B32G32R32F ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ABGR_F32 ] = D_D3DFMT_A32B32G32R32F ;
			String = "D3DFMT_A32B32G32R32F" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_A16B16G16R16F ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ABGR_F32 ] = D_D3DFMT_A16B16G16R16F ;
			String = "D3DFMT_A16B16G16R16F" ;
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x7f\x4f\x28\x75\x67\x30\x4d\x30\x8b\x30\x41\x00\x42\x00\x47\x00\x52\x00\x6e\x6d\xd5\x52\x0f\x5c\x70\x65\xb9\x70\x33\x00\x32\x00\xd3\x30\xc3\x30\xc8\x30\x8b\x57\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"使用できるABGR浮動小数点32ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ABGR_F32 ] = D_D3DFMT_UNKNOWN ;
		}

		if( GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ABGR_F32 ] != D_D3DFMT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x41\x42\x47\x52\x20\x95\x82\x93\xae\x8f\xac\x90\x94\x93\x5f\x20\x33\x32\x20\x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd\x20\x25\x73\x20\x82\xc5\x82\xb7"/*@ "ABGR 浮動小数点 32 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// １チャンネル整数8ビット型カラーフォーマットの選定
/*		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_L8 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_I8 ] = D_D3DFMT_L8 ;
			String = "D3DFMT_L8" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_A8L8 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_I8 ] = D_D3DFMT_A8L8 ;
			String = "D3DFMT_A8L8" ;
		}
		else*/
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_A8R8G8B8 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_I8 ] = D_D3DFMT_A8R8G8B8 ;
			String = "D3DFMT_A8R8G8B8" ;
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x7f\x4f\x28\x75\x67\x30\x4d\x30\x8b\x30\x11\xff\xc1\x30\xe3\x30\xf3\x30\xcd\x30\xeb\x30\x74\x65\x70\x65\x38\x00\xd3\x30\xc3\x30\xc8\x30\x8b\x57\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"使用できる１チャンネル整数8ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_I8 ] = D_D3DFMT_UNKNOWN ;
		}

		if( GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_I8 ] != D_D3DFMT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x82\x50\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x90\xae\x90\x94\x20\x38\x20\x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd\x20\x25\x73\x20\x82\xc5\x82\xb7"/*@ "１チャンネル整数 8 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// １チャンネル整数16ビット型カラーフォーマットの選定
/*		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_L16 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_I16 ] = D_D3DFMT_L16 ;
			String = "D3DFMT_L16" ;
		}
		else*/
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_G16R16 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_I16 ] = D_D3DFMT_G16R16 ;
			String = "D3DFMT_G16R16" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_A16B16G16R16 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_I16 ] = D_D3DFMT_A16B16G16R16 ;
			String = "D3DFMT_A16B16G16R16" ;
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x7f\x4f\x28\x75\x67\x30\x4d\x30\x8b\x30\x11\xff\xc1\x30\xe3\x30\xf3\x30\xcd\x30\xeb\x30\x74\x65\x70\x65\x31\x00\x36\x00\xd3\x30\xc3\x30\xc8\x30\x8b\x57\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"使用できる１チャンネル整数16ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_I16 ] = D_D3DFMT_UNKNOWN ;
		}

		if( GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_I16 ] != D_D3DFMT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x82\x50\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x90\xae\x90\x94\x20\x31\x36\x20\x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd\x20\x25\x73\x20\x82\xc5\x82\xb7"/*@ "１チャンネル整数 16 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// １チャンネル浮動小数点16ビット型カラーフォーマットの選定
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_R16F ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_F16 ] = D_D3DFMT_R16F ;
			String = "D3DFMT_R16F" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_G16R16F ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_F16 ] = D_D3DFMT_G16R16F ;
			String = "D3DFMT_G16R16F" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_A16B16G16R16F ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_F16 ] = D_D3DFMT_A16B16G16R16F ;
			String = "D3DFMT_A16B16G16R16F" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_A32B32G32R32F ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_F16 ] = D_D3DFMT_A32B32G32R32F ;
			String = "D3DFMT_A32B32G32R32F" ;
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x7f\x4f\x28\x75\x67\x30\x4d\x30\x8b\x30\x11\xff\xc1\x30\xe3\x30\xf3\x30\xcd\x30\xeb\x30\x6e\x6d\xd5\x52\x0f\x5c\x70\x65\xb9\x70\x31\x00\x36\x00\xd3\x30\xc3\x30\xc8\x30\x8b\x57\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"使用できる１チャンネル浮動小数点16ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_F16 ] = D_D3DFMT_UNKNOWN ;
		}

		if( GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_F16 ] != D_D3DFMT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x82\x50\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x95\x82\x93\xae\x8f\xac\x90\x94\x93\x5f\x20\x31\x36\x20\x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd\x20\x25\x73\x20\x82\xc5\x82\xb7"/*@ "１チャンネル浮動小数点 16 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// １チャンネル浮動小数点32ビット型カラーフォーマットの選定
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_R32F ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_F32 ] = D_D3DFMT_R32F ;
			String = "D3DFMT_R32F" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_G32R32F ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_F32 ] = D_D3DFMT_G32R32F ;
			String = "D3DFMT_G32R32F" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_A32B32G32R32F ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_F32 ] = D_D3DFMT_A32B32G32R32F ;
			String = "D3DFMT_A32B32G32R32F" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_A16B16G16R16F ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_F32 ] = D_D3DFMT_A16B16G16R16F ;
			String = "D3DFMT_A16B16G16R16F" ;
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x7f\x4f\x28\x75\x67\x30\x4d\x30\x8b\x30\x11\xff\xc1\x30\xe3\x30\xf3\x30\xcd\x30\xeb\x30\x6e\x6d\xd5\x52\x0f\x5c\x70\x65\xb9\x70\x33\x00\x32\x00\xd3\x30\xc3\x30\xc8\x30\x8b\x57\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"使用できる１チャンネル浮動小数点32ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_F32 ] = D_D3DFMT_UNKNOWN ;
		}

		if( GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_F32 ] != D_D3DFMT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x82\x50\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x95\x82\x93\xae\x8f\xac\x90\x94\x93\x5f\x20\x33\x32\x20\x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd\x20\x25\x73\x20\x82\xc5\x82\xb7"/*@ "１チャンネル浮動小数点 32 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// ２チャンネル整数8ビット型カラーフォーマットの選定
/*		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_A8L8 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_TWO_I8 ] = D_D3DFMT_A8L8 ;
			String = "D3DFMT_A8L8" ;
		}
		else*/
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_A8R8G8B8 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_TWO_I8 ] = D_D3DFMT_A8R8G8B8 ;
			String = "D3DFMT_A8B8G8R8" ;
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x7f\x4f\x28\x75\x67\x30\x4d\x30\x8b\x30\x12\xff\xc1\x30\xe3\x30\xf3\x30\xcd\x30\xeb\x30\x74\x65\x70\x65\x38\x00\xd3\x30\xc3\x30\xc8\x30\x8b\x57\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"使用できる２チャンネル整数8ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_TWO_I8 ] = D_D3DFMT_UNKNOWN ;
		}

		if( GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_TWO_I8 ] != D_D3DFMT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x82\x51\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x90\xae\x90\x94\x20\x38\x20\x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd\x20\x25\x73\x20\x82\xc5\x82\xb7"/*@ "２チャンネル整数 8 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// ２チャンネル整数16ビット型カラーフォーマットの選定
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_G16R16 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_TWO_I16 ] = D_D3DFMT_G16R16 ;
			String = "D3DFMT_G16R16" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_A16B16G16R16 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_TWO_I16 ] = D_D3DFMT_A16B16G16R16 ;
			String = "D3DFMT_A16B16G16R16" ;
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x7f\x4f\x28\x75\x67\x30\x4d\x30\x8b\x30\x12\xff\xc1\x30\xe3\x30\xf3\x30\xcd\x30\xeb\x30\x74\x65\x70\x65\x31\x00\x36\x00\xd3\x30\xc3\x30\xc8\x30\x8b\x57\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"使用できる２チャンネル整数16ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_TWO_I16 ] = D_D3DFMT_UNKNOWN ;
		}

		if( GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_TWO_I16 ] != D_D3DFMT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x82\x51\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x90\xae\x90\x94\x20\x31\x36\x20\x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd\x20\x25\x73\x20\x82\xc5\x82\xb7"/*@ "２チャンネル整数 16 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// ２チャンネル浮動小数点16ビット型カラーフォーマットの選定
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_G16R16F ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_TWO_F16 ] = D_D3DFMT_G16R16F ;
			String = "D3DFMT_G16R16F" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_A16B16G16R16F ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_TWO_F16 ] = D_D3DFMT_A16B16G16R16F ;
			String = "D3DFMT_A16B16G16R16F" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_A32B32G32R32F ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_TWO_F16 ] = D_D3DFMT_A32B32G32R32F ;
			String = "D3DFMT_A32B32G32R32F" ;
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x7f\x4f\x28\x75\x67\x30\x4d\x30\x8b\x30\x12\xff\xc1\x30\xe3\x30\xf3\x30\xcd\x30\xeb\x30\x6e\x6d\xd5\x52\x0f\x5c\x70\x65\xb9\x70\x31\x00\x36\x00\xd3\x30\xc3\x30\xc8\x30\x8b\x57\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"使用できる２チャンネル浮動小数点16ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_TWO_F16 ] = D_D3DFMT_UNKNOWN ;
		}

		if( GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_TWO_F16 ] != D_D3DFMT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x82\x51\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x95\x82\x93\xae\x8f\xac\x90\x94\x93\x5f\x20\x31\x36\x20\x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd\x20\x25\x73\x20\x82\xc5\x82\xb7"/*@ "２チャンネル浮動小数点 16 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// ２チャンネル浮動小数点32ビット型カラーフォーマットの選定
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_G32R32F ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_TWO_F32 ] = D_D3DFMT_G32R32F ;
			String = "D3DFMT_G32R32F" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_A32B32G32R32F ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_TWO_F32 ] = D_D3DFMT_A32B32G32R32F ;
			String = "D3DFMT_A32B32G32R32F" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, 0, D_D3DRTYPE_TEXTURE, D_D3DFMT_A16B16G16R16F ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_TWO_F32 ] = D_D3DFMT_A16B16G16R16F ;
			String = "D3DFMT_A16B16G16R16F" ;
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x7f\x4f\x28\x75\x67\x30\x4d\x30\x8b\x30\x12\xff\xc1\x30\xe3\x30\xf3\x30\xcd\x30\xeb\x30\x6e\x6d\xd5\x52\x0f\x5c\x70\x65\xb9\x70\x33\x00\x32\x00\xd3\x30\xc3\x30\xc8\x30\x8b\x57\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"使用できる２チャンネル浮動小数点32ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_TWO_F32 ] = D_D3DFMT_UNKNOWN ;
		}

		if( GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_TWO_F32 ] != D_D3DFMT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x82\x51\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x95\x82\x93\xae\x8f\xac\x90\x94\x93\x5f\x20\x33\x32\x20\x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd\x20\x25\x73\x20\x82\xc5\x82\xb7"/*@ "２チャンネル浮動小数点 32 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}







		// 描画可能 16ビットカラーフォーマットの選定
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_RENDERTARGET, D_D3DRTYPE_TEXTURE, GD3D9.Device.Caps.ScreenFormat == D_D3DFMT_R5G6B5 ? D_D3DFMT_R5G6B5 : D_D3DFMT_X1R5G5B5 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_RGB16 ] = GD3D9.Device.Caps.ScreenFormat == D_D3DFMT_R5G6B5 ? D_D3DFMT_R5G6B5 : D_D3DFMT_X1R5G5B5 ;
			String = GD3D9.Device.Caps.ScreenFormat == D_D3DFMT_R5G6B5 ? "D3DFMT_R5G6B5" : "D3DFMT_R5G5B5" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_RENDERTARGET, D_D3DRTYPE_TEXTURE, GD3D9.Device.Caps.ScreenFormat == D_D3DFMT_R5G6B5 ? D_D3DFMT_X1R5G5B5 : D_D3DFMT_R5G6B5 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_RGB16 ] = GD3D9.Device.Caps.ScreenFormat == D_D3DFMT_R5G6B5 ? D_D3DFMT_X1R5G5B5 : D_D3DFMT_R5G6B5 ;
			String = GD3D9.Device.Caps.ScreenFormat == D_D3DFMT_R5G6B5 ? "D3DFMT_R5G5B5" : "D3DFMT_R5G6B5" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_RENDERTARGET, D_D3DRTYPE_TEXTURE, D_D3DFMT_X8R8G8B8 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_RGB16 ] = D_D3DFMT_X8R8G8B8 ;
			String = "D3DFMT_X8R8G8B8" ;
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x7f\x4f\x28\x75\x67\x30\x4d\x30\x8b\x30\xcf\x63\x3b\x75\x28\x75\x20\x00\x31\x00\x36\x00\x62\x00\x69\x00\x74\x00\x20\x00\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"使用できる描画用 16bit カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_RGB16 ] = D_D3DFMT_UNKNOWN ;
		}

		if( GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_RGB16 ] != D_D3DFMT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x95\x60\x89\xe6\x97\x70 16bit \x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "描画用 16bit カラーフォーマットは %s です" @*/, String )) ;
		}

		// 描画可能 32ビットカラーフォーマットの選定
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_RENDERTARGET, D_D3DRTYPE_TEXTURE, D_D3DFMT_X8R8G8B8 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_RGB32 ] = D_D3DFMT_X8R8G8B8 ;
			String = "D3DFMT_X8R8G8B8" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_RENDERTARGET, D_D3DRTYPE_TEXTURE, D_D3DFMT_R5G6B5 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_RGB32 ] = D_D3DFMT_R5G6B5 ;
			String = "D3DFMT_R5G6B5" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_RENDERTARGET, D_D3DRTYPE_TEXTURE, D_D3DFMT_X1R5G5B5 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_RGB32 ] = D_D3DFMT_X1R5G5B5 ;
			String = "D3DFMT_R5G5B5" ;
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x7f\x4f\x28\x75\x67\x30\x4d\x30\x8b\x30\xcf\x63\x3b\x75\x28\x75\x20\x00\x33\x00\x32\x00\x62\x00\x69\x00\x74\x00\x20\x00\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"使用できる描画用 32bit カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_RGB32 ] = D_D3DFMT_UNKNOWN ;
		}

		if( GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_RGB32 ] != D_D3DFMT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x95\x60\x89\xe6\x97\x70 32bit \x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "描画用 32bit カラーフォーマットは %s です" @*/, String )) ;
		}

		// 描画可能 アルファチャンネル付き 32 ビットカラーフォーマットの選定
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_RENDERTARGET, D_D3DRTYPE_TEXTURE, D_D3DFMT_A8R8G8B8 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ALPHA_RGB32 ] = D_D3DFMT_A8R8G8B8 ;
			String = "D3DFMT_A8R8G8B8" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_RENDERTARGET, D_D3DRTYPE_TEXTURE, D_D3DFMT_A4R4G4B4 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ALPHA_RGB32 ] = D_D3DFMT_A4R4G4B4 ;
			String = "D3DFMT_A4R4G4B4" ;
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x7f\x4f\x28\x75\x67\x30\x4d\x30\x8b\x30\xcf\x63\x3b\x75\x28\x75\xa2\x30\xeb\x30\xd5\x30\xa1\x30\xd8\x4e\x4d\x30\x20\x00\x33\x00\x32\x00\x62\x00\x69\x00\x74\x00\x20\x00\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"使用できる描画用アルファ付き 32bit カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ALPHA_RGB32 ] = D_D3DFMT_UNKNOWN ;
		}

		if( GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ALPHA_RGB32 ] != D_D3DFMT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x95\x60\x89\xe6\x97\x70\x83\x41\x83\x8b\x83\x74\x83\x40\x95\x74\x82\xab 32bit \x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "描画用アルファ付き 32bit カラーフォーマットは %s です" @*/, String )) ;
		}

		// 描画可能 ARGB整数16ビット型カラーフォーマットの選定
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_RENDERTARGET, D_D3DRTYPE_TEXTURE, D_D3DFMT_A16B16G16R16 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ABGR_I16 ] = D_D3DFMT_A16B16G16R16 ;
			String = "D3DFMT_A16B16G16R16" ;
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x7f\x4f\x28\x75\x67\x30\x4d\x30\x8b\x30\xcf\x63\x3b\x75\x28\x75\x41\x00\x42\x00\x47\x00\x52\x00\x74\x65\x70\x65\x31\x00\x36\x00\xd3\x30\xc3\x30\xc8\x30\x8b\x57\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"使用できる描画用ABGR整数16ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ABGR_I16 ] = D_D3DFMT_UNKNOWN ;
		}

		if( GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ABGR_I16 ] != D_D3DFMT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x95\x60\x89\xe6\x97\x70 ABGR \x90\xae\x90\x94 16 \x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "描画用 ABGR 整数 16 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// 描画可能 ARGB浮動小数点16ビット型カラーフォーマットの選定
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_RENDERTARGET, D_D3DRTYPE_TEXTURE, D_D3DFMT_A16B16G16R16F ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ABGR_F16 ] = D_D3DFMT_A16B16G16R16F ;
			String = "D3DFMT_A16B16G16R16F" ;
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x7f\x4f\x28\x75\x67\x30\x4d\x30\x8b\x30\xcf\x63\x3b\x75\x28\x75\x41\x00\x42\x00\x47\x00\x52\x00\x6e\x6d\xd5\x52\x0f\x5c\x70\x65\xb9\x70\x31\x00\x36\x00\xd3\x30\xc3\x30\xc8\x30\x8b\x57\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"使用できる描画用ABGR浮動小数点16ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ABGR_F16 ] = D_D3DFMT_UNKNOWN ;
		}

		if( GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ABGR_F16 ] != D_D3DFMT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x95\x60\x89\xe6\x97\x70 ABGR \x95\x82\x93\xae\x8f\xac\x90\x94\x93\x5f 16 \x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "描画用 ABGR 浮動小数点 16 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// 描画可能 ARGB浮動小数点32ビット型カラーフォーマットの選定
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_RENDERTARGET, D_D3DRTYPE_TEXTURE, D_D3DFMT_A32B32G32R32F ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ABGR_F32 ] = D_D3DFMT_A32B32G32R32F ;
			String = "D3DFMT_A32B32G32R32F" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_RENDERTARGET, D_D3DRTYPE_TEXTURE, D_D3DFMT_A16B16G16R16F ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ABGR_F32 ] = D_D3DFMT_A16B16G16R16F ;
			String = "D3DFMT_A16B16G16R16F" ;
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x7f\x4f\x28\x75\x67\x30\x4d\x30\x8b\x30\xcf\x63\x3b\x75\x28\x75\x41\x00\x42\x00\x47\x00\x52\x00\x6e\x6d\xd5\x52\x0f\x5c\x70\x65\xb9\x70\x33\x00\x32\x00\xd3\x30\xc3\x30\xc8\x30\x8b\x57\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"使用できる描画用ABGR浮動小数点32ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ABGR_F32 ] = D_D3DFMT_UNKNOWN ;
		}

		if( GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ABGR_F32 ] != D_D3DFMT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x95\x60\x89\xe6\x97\x70 ABGR \x95\x82\x93\xae\x8f\xac\x90\x94\x93\x5f 32 \x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "描画用 ABGR 浮動小数点 32 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// 描画可能 １チャンネル整数8ビット型カラーフォーマットの選定
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_RENDERTARGET, D_D3DRTYPE_TEXTURE, D_D3DFMT_L8 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_I8 ] = D_D3DFMT_L8 ;
			String = "D3DFMT_L8" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_RENDERTARGET, D_D3DRTYPE_TEXTURE, D_D3DFMT_A8L8 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_I8 ] = D_D3DFMT_A8L8 ;
			String = "D3DFMT_A8L8" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_RENDERTARGET, D_D3DRTYPE_TEXTURE, D_D3DFMT_A8R8G8B8 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_I8 ] = D_D3DFMT_A8R8G8B8 ;
			String = "D3DFMT_A8R8G8B8" ;
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x7f\x4f\x28\x75\x67\x30\x4d\x30\x8b\x30\xcf\x63\x3b\x75\x28\x75\x11\xff\xc1\x30\xe3\x30\xf3\x30\xcd\x30\xeb\x30\x74\x65\x70\x65\x38\x00\xd3\x30\xc3\x30\xc8\x30\x8b\x57\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"使用できる描画用１チャンネル整数8ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_I8 ] = D_D3DFMT_UNKNOWN ;
		}

		if( GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_I8 ] != D_D3DFMT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x95\x60\x89\xe6\x97\x70\x82\x50\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x90\xae\x90\x94 8 \x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "描画用１チャンネル整数 8 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// 描画可能 １チャンネル整数16ビット型カラーフォーマットの選定
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_RENDERTARGET, D_D3DRTYPE_TEXTURE, D_D3DFMT_L16 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_I16 ] = D_D3DFMT_L16 ;
			String = "D3DFMT_L16" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_RENDERTARGET, D_D3DRTYPE_TEXTURE, D_D3DFMT_G16R16 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_I16 ] = D_D3DFMT_G16R16 ;
			String = "D3DFMT_G16R16" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_RENDERTARGET, D_D3DRTYPE_TEXTURE, D_D3DFMT_A16B16G16R16 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_I16 ] = D_D3DFMT_A16B16G16R16 ;
			String = "D3DFMT_A16B16G16R16" ;
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x7f\x4f\x28\x75\x67\x30\x4d\x30\x8b\x30\xcf\x63\x3b\x75\x28\x75\x11\xff\xc1\x30\xe3\x30\xf3\x30\xcd\x30\xeb\x30\x74\x65\x70\x65\x31\x00\x36\x00\xd3\x30\xc3\x30\xc8\x30\x8b\x57\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"使用できる描画用１チャンネル整数16ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_I16 ] = D_D3DFMT_UNKNOWN ;
		}

		if( GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_I16 ] != D_D3DFMT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x95\x60\x89\xe6\x97\x70\x82\x50\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x90\xae\x90\x94 16 \x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "描画用１チャンネル整数 16 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// 描画可能 １チャンネル浮動小数点16ビット型カラーフォーマットの選定
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_RENDERTARGET, D_D3DRTYPE_TEXTURE, D_D3DFMT_R16F ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_F16 ] = D_D3DFMT_R16F ;
			String = "D3DFMT_R16F" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_RENDERTARGET, D_D3DRTYPE_TEXTURE, D_D3DFMT_G16R16F ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_F16 ] = D_D3DFMT_G16R16F ;
			String = "D3DFMT_G16R16F" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_RENDERTARGET, D_D3DRTYPE_TEXTURE, D_D3DFMT_A16B16G16R16F ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_F16 ] = D_D3DFMT_A16B16G16R16F ;
			String = "D3DFMT_A16B16G16R16F" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_RENDERTARGET, D_D3DRTYPE_TEXTURE, D_D3DFMT_A32B32G32R32F ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_F16 ] = D_D3DFMT_A32B32G32R32F ;
			String = "D3DFMT_A32B32G32R32F" ;
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x7f\x4f\x28\x75\x67\x30\x4d\x30\x8b\x30\xcf\x63\x3b\x75\x28\x75\x11\xff\xc1\x30\xe3\x30\xf3\x30\xcd\x30\xeb\x30\x6e\x6d\xd5\x52\x0f\x5c\x70\x65\xb9\x70\x31\x00\x36\x00\xd3\x30\xc3\x30\xc8\x30\x8b\x57\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"使用できる描画用１チャンネル浮動小数点16ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_F16 ] = D_D3DFMT_UNKNOWN ;
		}

		if( GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_F16 ] != D_D3DFMT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x95\x60\x89\xe6\x97\x70\x82\x50\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x95\x82\x93\xae\x8f\xac\x90\x94\x93\x5f 16 \x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "描画用１チャンネル浮動小数点 16 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// 描画可能 １チャンネル浮動小数点32ビット型カラーフォーマットの選定
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_RENDERTARGET, D_D3DRTYPE_TEXTURE, D_D3DFMT_R32F ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_F32 ] = D_D3DFMT_R32F ;
			String = "D3DFMT_R32F" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_RENDERTARGET, D_D3DRTYPE_TEXTURE, D_D3DFMT_G32R32F ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_F32 ] = D_D3DFMT_G32R32F ;
			String = "D3DFMT_G32R32F" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_RENDERTARGET, D_D3DRTYPE_TEXTURE, D_D3DFMT_A32B32G32R32F ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_F32 ] = D_D3DFMT_A32B32G32R32F ;
			String = "D3DFMT_A32B32G32R32F" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_RENDERTARGET, D_D3DRTYPE_TEXTURE, D_D3DFMT_A16B16G16R16F ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_F32 ] = D_D3DFMT_A16B16G16R16F ;
			String = "D3DFMT_A16B16G16R16F" ;
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x7f\x4f\x28\x75\x67\x30\x4d\x30\x8b\x30\xcf\x63\x3b\x75\x28\x75\x11\xff\xc1\x30\xe3\x30\xf3\x30\xcd\x30\xeb\x30\x6e\x6d\xd5\x52\x0f\x5c\x70\x65\xb9\x70\x33\x00\x32\x00\xd3\x30\xc3\x30\xc8\x30\x8b\x57\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"使用できる描画用１チャンネル浮動小数点32ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_F32 ] = D_D3DFMT_UNKNOWN ;
		}

		if( GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_F32 ] != D_D3DFMT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x95\x60\x89\xe6\x97\x70\x82\x50\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x95\x82\x93\xae\x8f\xac\x90\x94\x93\x5f 32 \x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "描画用１チャンネル浮動小数点 32 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// 描画可能 ２チャンネル整数8ビット型カラーフォーマットの選定
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_RENDERTARGET, D_D3DRTYPE_TEXTURE, D_D3DFMT_A8L8 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_I8 ] = D_D3DFMT_A8L8 ;
			String = "D3DFMT_A8L8" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_RENDERTARGET, D_D3DRTYPE_TEXTURE, D_D3DFMT_A8R8G8B8 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_I8 ] = D_D3DFMT_A8R8G8B8 ;
			String = "D3DFMT_A8B8G8R8" ;
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x7f\x4f\x28\x75\x67\x30\x4d\x30\x8b\x30\xcf\x63\x3b\x75\x28\x75\x12\xff\xc1\x30\xe3\x30\xf3\x30\xcd\x30\xeb\x30\x74\x65\x70\x65\x38\x00\xd3\x30\xc3\x30\xc8\x30\x8b\x57\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"使用できる描画用２チャンネル整数8ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_I8 ] = D_D3DFMT_UNKNOWN ;
		}

		if( GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_I8 ] != D_D3DFMT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x95\x60\x89\xe6\x97\x70\x82\x51\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x90\xae\x90\x94 8 \x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "描画用２チャンネル整数 8 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// 描画可能 ２チャンネル整数16ビット型カラーフォーマットの選定
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_RENDERTARGET, D_D3DRTYPE_TEXTURE, D_D3DFMT_G16R16 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_I16 ] = D_D3DFMT_G16R16 ;
			String = "D3DFMT_G16R16" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_RENDERTARGET, D_D3DRTYPE_TEXTURE, D_D3DFMT_A16B16G16R16 ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_I16 ] = D_D3DFMT_A16B16G16R16 ;
			String = "D3DFMT_A16B16G16R16" ;
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x7f\x4f\x28\x75\x67\x30\x4d\x30\x8b\x30\xcf\x63\x3b\x75\x28\x75\x12\xff\xc1\x30\xe3\x30\xf3\x30\xcd\x30\xeb\x30\x74\x65\x70\x65\x31\x00\x36\x00\xd3\x30\xc3\x30\xc8\x30\x8b\x57\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"使用できる描画用２チャンネル整数16ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_I16 ] = D_D3DFMT_UNKNOWN ;
		}

		if( GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_I16 ] != D_D3DFMT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x95\x60\x89\xe6\x97\x70\x82\x51\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x90\xae\x90\x94 16 \x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "描画用２チャンネル整数 16 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// 描画可能 ２チャンネル浮動小数点16ビット型カラーフォーマットの選定
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_RENDERTARGET, D_D3DRTYPE_TEXTURE, D_D3DFMT_G16R16F ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_F16 ] = D_D3DFMT_G16R16F ;
			String = "D3DFMT_G16R16F" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_RENDERTARGET, D_D3DRTYPE_TEXTURE, D_D3DFMT_A16B16G16R16F ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_F16 ] = D_D3DFMT_A16B16G16R16F ;
			String = "D3DFMT_A16B16G16R16F" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_RENDERTARGET, D_D3DRTYPE_TEXTURE, D_D3DFMT_A32B32G32R32F ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_F16 ] = D_D3DFMT_A32B32G32R32F ;
			String = "D3DFMT_A32B32G32R32F" ;
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x7f\x4f\x28\x75\x67\x30\x4d\x30\x8b\x30\xcf\x63\x3b\x75\x28\x75\x12\xff\xc1\x30\xe3\x30\xf3\x30\xcd\x30\xeb\x30\x6e\x6d\xd5\x52\x0f\x5c\x70\x65\xb9\x70\x31\x00\x36\x00\xd3\x30\xc3\x30\xc8\x30\x8b\x57\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"使用できる描画用２チャンネル浮動小数点16ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_F16 ] = D_D3DFMT_UNKNOWN ;
		}

		if( GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_F16 ] != D_D3DFMT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x95\x60\x89\xe6\x97\x70\x82\x51\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x95\x82\x93\xae\x8f\xac\x90\x94\x93\x5f 16 \x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "描画用２チャンネル浮動小数点 16 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// 描画可能 ２チャンネル浮動小数点32ビット型カラーフォーマットの選定
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_RENDERTARGET, D_D3DRTYPE_TEXTURE, D_D3DFMT_G32R32F ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_F32 ] = D_D3DFMT_G32R32F ;
			String = "D3DFMT_G32R32F" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_RENDERTARGET, D_D3DRTYPE_TEXTURE, D_D3DFMT_A32B32G32R32F ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_F32 ] = D_D3DFMT_A32B32G32R32F ;
			String = "D3DFMT_A32B32G32R32F" ;
		}
		else
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_RENDERTARGET, D_D3DRTYPE_TEXTURE, D_D3DFMT_A16B16G16R16F ) == D_D3D_OK )
		{
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_F32 ] = D_D3DFMT_A16B16G16R16F ;
			String = "D3DFMT_A16B16G16R16F" ;
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x7f\x4f\x28\x75\x67\x30\x4d\x30\x8b\x30\xcf\x63\x3b\x75\x28\x75\x12\xff\xc1\x30\xe3\x30\xf3\x30\xcd\x30\xeb\x30\x6e\x6d\xd5\x52\x0f\x5c\x70\x65\xb9\x70\x33\x00\x32\x00\xd3\x30\xc3\x30\xc8\x30\x8b\x57\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"使用できる描画用２チャンネル浮動小数点32ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_F32 ] = D_D3DFMT_UNKNOWN ;
		}

		if( GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_F32 ] != D_D3DFMT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x95\x60\x89\xe6\x97\x70\x82\x51\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x95\x82\x93\xae\x8f\xac\x90\x94\x93\x5f 32 \x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "描画用２チャンネル浮動小数点 32 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// マスクカラーバッファ用フォーマットの選定
		GD3D9.Device.Caps.MaskColorFormat = GSYS.Screen.MainScreenColorBitDepth == 32 ? GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_RGB32 ] : GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_RGB16 ] ;

		// マスクアルファフォーマットの選定
		GD3D9.Device.Caps.MaskAlphaFormat = GSYS.Screen.MainScreenColorBitDepth == 32 ? GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHATEST_RGB32 ] : GD3D9.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHATEST_RGB16 ] ;


		// 頂点テクスチャフェッチチェック
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_QUERY_VERTEXTEXTURE, D_D3DRTYPE_TEXTURE, D_D3DFMT_A8R8G8B8 ) == D_D3D_OK )
		{
			DXST_LOGFILE_ADDUTF16LE( "\x41\x00\x38\x00\x52\x00\x38\x00\x47\x00\x38\x00\x42\x00\x38\x00\x20\x00\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x6f\x30\x02\x98\xb9\x70\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\xd5\x30\xa7\x30\xc3\x30\xc1\x30\x6b\x30\x7f\x4f\x28\x75\x67\x30\x4d\x30\x7e\x30\x59\x30\x0a\x00\x00"/*@ L"A8R8G8B8 カラーフォーマットは頂点テクスチャフェッチに使用できます\n" @*/ ) ;
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x41\x00\x38\x00\x52\x00\x38\x00\x47\x00\x38\x00\x42\x00\x38\x00\x20\x00\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x6f\x30\x02\x98\xb9\x70\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\xd5\x30\xa7\x30\xc3\x30\xc1\x30\x6b\x30\x7f\x4f\x28\x75\x67\x30\x4d\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"A8R8G8B8 カラーフォーマットは頂点テクスチャフェッチに使用できません\n" @*/ ) ;
		}
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_QUERY_VERTEXTEXTURE, D_D3DRTYPE_TEXTURE, D_D3DFMT_X8R8G8B8 ) == D_D3D_OK )
		{
			DXST_LOGFILE_ADDUTF16LE( "\x58\x00\x38\x00\x52\x00\x38\x00\x47\x00\x38\x00\x42\x00\x38\x00\x20\x00\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x6f\x30\x02\x98\xb9\x70\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\xd5\x30\xa7\x30\xc3\x30\xc1\x30\x6b\x30\x7f\x4f\x28\x75\x67\x30\x4d\x30\x7e\x30\x59\x30\x0a\x00\x00"/*@ L"X8R8G8B8 カラーフォーマットは頂点テクスチャフェッチに使用できます\n" @*/ ) ;
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x58\x00\x38\x00\x52\x00\x38\x00\x47\x00\x38\x00\x42\x00\x38\x00\x20\x00\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x6f\x30\x02\x98\xb9\x70\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\xd5\x30\xa7\x30\xc3\x30\xc1\x30\x6b\x30\x7f\x4f\x28\x75\x67\x30\x4d\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"X8R8G8B8 カラーフォーマットは頂点テクスチャフェッチに使用できません\n" @*/ ) ;
		}
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_QUERY_VERTEXTEXTURE, D_D3DRTYPE_TEXTURE, D_D3DFMT_R5G6B5 ) == D_D3D_OK )
		{
			DXST_LOGFILE_ADDUTF16LE( "\x52\x00\x35\x00\x47\x00\x36\x00\x42\x00\x35\x00\x20\x00\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x6f\x30\x02\x98\xb9\x70\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\xd5\x30\xa7\x30\xc3\x30\xc1\x30\x6b\x30\x7f\x4f\x28\x75\x67\x30\x4d\x30\x7e\x30\x59\x30\x0a\x00\x00"/*@ L"R5G6B5 カラーフォーマットは頂点テクスチャフェッチに使用できます\n" @*/ ) ;
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x52\x00\x35\x00\x47\x00\x36\x00\x42\x00\x35\x00\x20\x00\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x6f\x30\x02\x98\xb9\x70\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\xd5\x30\xa7\x30\xc3\x30\xc1\x30\x6b\x30\x7f\x4f\x28\x75\x67\x30\x4d\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"R5G6B5 カラーフォーマットは頂点テクスチャフェッチに使用できません\n" @*/ ) ;
		}
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_QUERY_VERTEXTEXTURE, D_D3DRTYPE_TEXTURE, D_D3DFMT_A1R5G5B5 ) == D_D3D_OK )
		{
			DXST_LOGFILE_ADDUTF16LE( "\x41\x00\x31\x00\x52\x00\x35\x00\x47\x00\x35\x00\x42\x00\x35\x00\x20\x00\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x6f\x30\x02\x98\xb9\x70\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\xd5\x30\xa7\x30\xc3\x30\xc1\x30\x6b\x30\x7f\x4f\x28\x75\x67\x30\x4d\x30\x7e\x30\x59\x30\x0a\x00\x00"/*@ L"A1R5G5B5 カラーフォーマットは頂点テクスチャフェッチに使用できます\n" @*/ ) ;
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x41\x00\x31\x00\x52\x00\x35\x00\x47\x00\x35\x00\x42\x00\x35\x00\x20\x00\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x6f\x30\x02\x98\xb9\x70\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\xd5\x30\xa7\x30\xc3\x30\xc1\x30\x6b\x30\x7f\x4f\x28\x75\x67\x30\x4d\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"A1R5G5B5 カラーフォーマットは頂点テクスチャフェッチに使用できません\n" @*/ ) ;
		}
		if( Direct3D9_CheckDeviceFormat( UseAdapterNumber, D_D3DDEVTYPE_HAL, GD3D9.Device.Caps.ScreenFormat, D_D3DUSAGE_QUERY_VERTEXTEXTURE, D_D3DRTYPE_TEXTURE, D_D3DFMT_X1R5G5B5 ) == D_D3D_OK )
		{
			DXST_LOGFILE_ADDUTF16LE( "\x58\x00\x31\x00\x52\x00\x35\x00\x47\x00\x35\x00\x42\x00\x35\x00\x20\x00\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x6f\x30\x02\x98\xb9\x70\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\xd5\x30\xa7\x30\xc3\x30\xc1\x30\x6b\x30\x7f\x4f\x28\x75\x67\x30\x4d\x30\x7e\x30\x59\x30\x0a\x00\x00"/*@ L"X1R5G5B5 カラーフォーマットは頂点テクスチャフェッチに使用できます\n" @*/ ) ;
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x58\x00\x31\x00\x52\x00\x35\x00\x47\x00\x35\x00\x42\x00\x35\x00\x20\x00\xab\x30\xe9\x30\xfc\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x6f\x30\x02\x98\xb9\x70\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\xd5\x30\xa7\x30\xc3\x30\xc1\x30\x6b\x30\x7f\x4f\x28\x75\x67\x30\x4d\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"X1R5G5B5 カラーフォーマットは頂点テクスチャフェッチに使用できません\n" @*/ ) ;
		}
	}

	// 情報出力
	{
		DXST_LOGFILEFMT_ADDA(( "\x88\xea\x93\x78\x82\xc9\x95\x60\x89\xe6\x82\xc5\x82\xab\x82\xe9\x83\x76\x83\x8a\x83\x7e\x83\x65\x83\x42\x83\x75\x82\xcc\x8d\xc5\x91\xe5\x90\x94:%d"/*@ "一度に描画できるプリミティブの最大数:%d" @*/, GSYS.HardInfo.MaxPrimitiveCount )) ;
		DXST_LOGFILEFMT_ADDA(( "\x91\xce\x89\x9e\x82\xb5\x82\xc4\x82\xa2\x82\xe9\x8d\xc5\x91\xe5\x92\xb8\x93\x5f\x83\x43\x83\x93\x83\x66\x83\x62\x83\x4e\x83\x58:%d"/*@ "対応している最大頂点インデックス:%d" @*/,     GSYS.HardInfo.MaxVertexIndex )) ;
		DXST_LOGFILEFMT_ADDA(( "\x93\xaf\x8e\x9e\x82\xc9\x83\x8c\x83\x93\x83\x5f\x83\x8a\x83\x93\x83\x4f\x82\xc5\x82\xab\x82\xe9\x83\x6f\x83\x62\x83\x74\x83\x40\x82\xcc\x90\x94:%d"/*@ "同時にレンダリングできるバッファの数:%d" @*/, GSYS.HardInfo.RenderTargetNum )) ;
		DXST_LOGFILEFMT_ADDA(( "\x8d\xc5\x91\xe5\x83\x65\x83\x4e\x83\x58\x83\x60\x83\x83\x83\x54\x83\x43\x83\x59\x81\x40\x95\x9d:%d \x8d\x82\x82\xb3:%d"/*@ "最大テクスチャサイズ　幅:%d 高さ:%d" @*/, GD3D9.Device.Caps.DeviceCaps.MaxTextureWidth, GD3D9.Device.Caps.DeviceCaps.MaxTextureHeight )) ;
		DXST_LOGFILEFMT_ADDA(( "\x83\x65\x83\x4e\x83\x58\x83\x60\x83\x83\x83\x58\x83\x65\x81\x5b\x83\x57\x83\x65\x83\x93\x83\x7c\x83\x89\x83\x8a\x83\x8c\x83\x57\x83\x58\x83\x5e\x81\x46%s"/*@ "テクスチャステージテンポラリレジスタ：%s" @*/, GD3D9.Device.Caps.ValidTexTempRegFlag ? "\x8e\x67\x97\x70\x89\xc2"/*@ "使用可" @*/ : "\x8e\x67\x97\x70\x95\x73\x89\xc2"/*@ "使用不可" @*/ )) ;
		DXST_LOGFILEFMT_ADDA(( "\x8c\xb8\x8e\x5a\x8d\x87\x90\xac\x82\xcc\x83\x6e\x81\x5b\x83\x68\x83\x45\x83\x46\x83\x41\x91\xce\x89\x9e\x81\x46%s"/*@ "減算合成のハードウェア対応：%s" @*/, GD3D9.Device.Caps.ValidDestBlendOp ? "\x83\x6c\x83\x43\x83\x65\x83\x42\x83\x75"/*@ "ネイティブ" @*/ : "\x83\x47\x83\x7e\x83\x85\x83\x8c\x81\x5b\x83\x67"/*@ "エミュレート" @*/ )) ;
		if( GD3D9.Device.Caps.VertexHardwareProcess )
		{
			DXST_LOGFILEFMT_ADDA(( "\x83\x6e\x81\x5b\x83\x68\x83\x45\x83\x46\x83\x41\x92\xb8\x93\x5f\x83\x56\x83\x46\x81\x5b\x83\x5f\x81\x5b\x83\x6f\x81\x5b\x83\x57\x83\x87\x83\x93\x83\x52\x81\x5b\x83\x68\x81\x46%x"/*@ "ハードウェア頂点シェーダーバージョンコード：%x" @*/, GD3D9.Device.Caps.DeviceCaps.VertexShaderVersion & 0xffff )) ;
			DXST_LOGFILEFMT_ADDA(( "\x83\x47\x83\x7e\x83\x85\x83\x8c\x81\x5b\x83\x56\x83\x87\x83\x93\x92\xb8\x93\x5f\x83\x56\x83\x46\x81\x5b\x83\x5f\x81\x5b\x83\x6f\x81\x5b\x83\x57\x83\x87\x83\x93\x83\x52\x81\x5b\x83\x68\x81\x46\x83\x47\x83\x7e\x83\x85\x83\x8c\x81\x5b\x83\x56\x83\x87\x83\x93\x96\xb3\x82\xb5"/*@ "エミュレーション頂点シェーダーバージョンコード：エミュレーション無し" @*/ )) ;
		}
		else
		{
			DXST_LOGFILEFMT_ADDA(( "\x83\x6e\x81\x5b\x83\x68\x83\x45\x83\x46\x83\x41\x92\xb8\x93\x5f\x83\x56\x83\x46\x81\x5b\x83\x5f\x81\x5b\x83\x6f\x81\x5b\x83\x57\x83\x87\x83\x93\x83\x52\x81\x5b\x83\x68\x81\x46%x"/*@ "ハードウェア頂点シェーダーバージョンコード：%x" @*/, GD3D9.Device.Shader.NativeVertexShaderVersion & 0xffff )) ;
			DXST_LOGFILEFMT_ADDA(( "\x83\x47\x83\x7e\x83\x85\x83\x8c\x81\x5b\x83\x56\x83\x87\x83\x93\x92\xb8\x93\x5f\x83\x56\x83\x46\x81\x5b\x83\x5f\x81\x5b\x83\x6f\x81\x5b\x83\x57\x83\x87\x83\x93\x83\x52\x81\x5b\x83\x68\x81\x46%x"/*@ "エミュレーション頂点シェーダーバージョンコード：%x" @*/, GD3D9.Device.Caps.DeviceCaps.VertexShaderVersion & 0xffff )) ;
		}
		DXST_LOGFILEFMT_ADDA(( "\x83\x73\x83\x4e\x83\x5a\x83\x8b\x83\x56\x83\x46\x81\x5b\x83\x5f\x81\x5b\x83\x6f\x81\x5b\x83\x57\x83\x87\x83\x93\x83\x52\x81\x5b\x83\x68\x81\x46%x"/*@ "ピクセルシェーダーバージョンコード：%x" @*/, GD3D9.Device.Caps.DeviceCaps.PixelShaderVersion & 0xffff )) ;
	}

	// 描画先の情報を描画先をロックして読み取る場合と一時的な描画サーフェスを介して読み取る場合のどちらが高速かを調べる
	{
		D_IDirect3DSurface9 *RTSurface = NULL ;
		D_IDirect3DSurface9 *SMSurface = NULL ;
		D_IDirect3DSurface9 *DestSurface = NULL ;
		D_D3DLOCKED_RECT SrcLockedRect ;
		D_D3DLOCKED_RECT DestLockedRect ;
		RECT LockRect ;
		LONGLONG Time1 = 0, Time2 = 0;
		UINT Size, i, j ;
		COLORDATA *ColorData ;

		// 初期状態ではロックを使用しない、にしておく
		GD3D9.Device.Caps.UseRenderTargetLock = FALSE ;

		// フルシーンアンチエイリアスを使用する場合はバックバッファのロックはできない
		if( ( D_D3DMULTISAMPLE_TYPE )GSYS.Setting.FSAAMultiSampleCount != D_D3DMULTISAMPLE_NONE )
		{
			DXST_LOGFILE_ADDUTF16LE( "\xd5\x30\xeb\x30\xb7\x30\xfc\x30\xf3\x30\xa2\x30\xf3\x30\xc1\x30\xa8\x30\xa4\x30\xea\x30\xa2\x30\xb9\x30\x92\x30\x7f\x4f\x28\x75\x59\x30\x8b\x30\x34\x58\x08\x54\x6f\x30\xd0\x30\xc3\x30\xaf\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6f\x30\xed\x30\xc3\x30\xaf\x30\x67\x30\x4d\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"フルシーンアンチエイリアスを使用する場合はバックバッファはロックできません\n" @*/ ) ;
		}
		else
		{
			// キャッシュなどの関係で先行する方が不利になる可能性があるため、３回同じことを繰り返す
			for( j = 0 ; j < 3 ; j ++ )
			{
				// チェック開始
				Time1 = -1 ;
				Time2 = -1 ;
				for(;;)
				{
					// テストに使用するサーフェスのサイズを決定
					Size = 256 ;
					while( Size > ( UINT )GSYS.Screen.MainScreenSizeX || Size > ( UINT )GSYS.Screen.MainScreenSizeY ) Size >>= 1 ;

					// 仮の出力先のサーフェスを作成する
					DestSurface = NULL ;
					if( Direct3DDevice9_CreateOffscreenPlainSurface(
							Size, Size, GD3D9.Device.Caps.ScreenFormat, D_D3DPOOL_SYSTEMMEM, &DestSurface, NULL ) != D_D3D_OK )
						break ;

					// ロックする矩形の作成
					LockRect.left   = 0 ;
					LockRect.top    = 0 ;
					LockRect.right  = ( LONG )Size ;
					LockRect.bottom = ( LONG )Size ;

					// カラーデータの取得
					ColorData = Graphics_D3D9_GetD3DFormatColorData( GD3D9.Device.Caps.ScreenFormat ) ;

					// 仮の出力先のサーフェスをロック
					if( Direct3DSurface9_LockRect( DestSurface, &DestLockedRect, &LockRect, D_D3DLOCK_READONLY ) != D_D3D_OK )
						break ;

					// 最初に直接ロックする場合を試みる
					{
						Time1 = NS_GetNowHiPerformanceCount() ;

						// 直接ロック
						if( Direct3DSurface9_LockRect( GD3D9.Device.Screen.BackBufferSurface, &SrcLockedRect, &LockRect, D_D3DLOCK_READONLY ) != D_D3D_OK )
							break ;

						// データを転送
						for( i = 0 ; i < Size ; i ++ )
							_MEMCPY(
								( BYTE * )DestLockedRect.pBits + DestLockedRect.Pitch * i,
								( BYTE * )SrcLockedRect.pBits + SrcLockedRect.Pitch * i,
								Size * ColorData->PixelByte ) ;

						// ロック解除
						Direct3DSurface9_UnlockRect( GD3D9.Device.Screen.BackBufferSurface ) ;

						Time1 = NS_GetNowHiPerformanceCount() - Time1 ;

					}

					// 一時的な描画先サーフェスを作成して読み取る場合を試みる
					{
						Time2 = GetNowHiPerformanceCount() ;

						// ロックサイズと同じ大きさの描画可能サーフェスの作成
						if( Direct3DDevice9_CreateRenderTarget(
								Size, Size, GD3D9.Device.Caps.ScreenFormat, D_D3DMULTISAMPLE_NONE, 0, FALSE, &RTSurface, NULL ) != D_D3D_OK )
							break ;

						// ロックサイズと同じ大きさのシステムメモリ上のオフスクリーンサーフェスの作成
						if( Direct3DDevice9_CreateOffscreenPlainSurface(
								Size, Size, GD3D9.Device.Caps.ScreenFormat, D_D3DPOOL_SYSTEMMEM, &SMSurface, NULL ) != D_D3D_OK )
							break ;

						// まず描画可能サーフェスに内容を転送する
						Direct3DDevice9_StretchRect(
							GD3D9.Device.Screen.BackBufferSurface, &LockRect,
							RTSurface,             &LockRect, D_D3DTEXF_NONE ) ;

						// その後システムメモリサーフェスに内容を転送する
						Direct3DDevice9_GetRenderTargetData(
							RTSurface, SMSurface ) ;

						// システムメモリサーフェスをロック
						if( Direct3DSurface9_LockRect( SMSurface, &SrcLockedRect, NULL, D_D3DLOCK_READONLY ) != D_D3D_OK )
							break ;

						// データを転送
						for( i = 0 ; i < Size ; i ++ )
							_MEMCPY(
								( BYTE * )DestLockedRect.pBits + DestLockedRect.Pitch * i,
								( BYTE * )SrcLockedRect.pBits + SrcLockedRect.Pitch * i,
								Size * ColorData->PixelByte ) ;

						// ロック解除
						Direct3DSurface9_UnlockRect( SMSurface ) ;

						// 一時的なサーフェスを解放
						Direct3D9_ObjectRelease( RTSurface ) ;
						Direct3D9_ObjectRelease( SMSurface ) ;
						RTSurface = NULL ;
						SMSurface = NULL ;

						Time2 = GetNowHiPerformanceCount() - Time2 ;
					}

					break ;
				}
				if( SMSurface != NULL )
				{
					Direct3D9_ObjectRelease( SMSurface ) ;
					SMSurface = NULL ;
				}
				if( RTSurface != NULL )
				{
					Direct3D9_ObjectRelease( RTSurface ) ;
					RTSurface = NULL ;
				}
				if( DestSurface != NULL )
				{
					Direct3D9_ObjectRelease( DestSurface ) ;
					DestSurface = NULL ;
				}
			}

			if( Time2 == -1 || Time1 == -1 )
			{
				DXST_LOGFILE_ADDUTF16LE( "\xd0\x30\xc3\x30\xaf\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6f\x30\xed\x30\xc3\x30\xaf\x30\x67\x30\x4d\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"バックバッファはロックできません\n" @*/ ) ;
			}
			else
			{
				// 高速な方を使用する
				DXST_LOGFILEFMT_ADDUTF16LE(( "\xd0\x30\xc3\x30\xaf\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\xed\x30\xc3\x30\xaf\x30\xe2\x8e\x01\x90\x6e\x30\x42\x66\x93\x95\x3a\x00\x25\x00\x64\x00\x6e\x00\x73\x00\x65\x00\x63\x00\x20\x00\x20\x00\x00\x4e\x42\x66\x84\x76\x6a\x30\xcf\x63\x3b\x75\xef\x53\xfd\x80\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x92\x30\x7f\x4f\x28\x75\x57\x30\x5f\x30\xe2\x8e\x01\x90\x6e\x30\x42\x66\x93\x95\x3a\x00\x25\x00\x64\x00\x6e\x00\x73\x00\x65\x00\x63\x00\x00"/*@ L"バックバッファロック転送の時間:%dnsec  一時的な描画可能バッファを使用した転送の時間:%dnsec" @*/, ( int )Time1, ( int )Time2 ) ) ;
				if( Time1 < Time2 )
				{
					GD3D9.Device.Caps.UseRenderTargetLock = TRUE ;
					DXST_LOGFILE_ADDUTF16LE( "\xd0\x30\xc3\x30\xaf\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\xed\x30\xc3\x30\xaf\x30\x92\x30\x7f\x4f\x28\x75\x57\x30\x7e\x30\x59\x30\x0a\x00\x00"/*@ L"バックバッファロックを使用します\n" @*/ ) ;
				}
				else
				{
					DXST_LOGFILE_ADDUTF16LE( "\xd0\x30\xc3\x30\xaf\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\xed\x30\xc3\x30\xaf\x30\x92\x30\x7f\x4f\x28\x75\x57\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"バックバッファロックを使用しません\n" @*/ ) ;
				}
			}
		}
	}

	// 終了
	return 0 ;

	// エラー処理
ERR:
	return ErrorRet ;
}

// Direct3DDevice9 関係の初期化
extern	int		Graphics_D3D9_Device_Initialize( void )
{
	DWORD i ;

#ifndef DX_NON_FILTER
	GraphFilter_Initialize() ;
#endif // DX_NON_FILTER

	// 頂点バッファ内の固定値をセットする
	{
		VERTEX_NOTEX_2D    *vertNoTex ;
		VERTEX_2D          *vertTex ;
		VERTEX_BLENDTEX_2D *vertBlend ;

		vertNoTex = (VERTEX_NOTEX_2D    *)GD3D9.Device.DrawInfo.VertexBuffer_NoTex ;
		vertTex   = (VERTEX_2D          *)GD3D9.Device.DrawInfo.VertexBuffer_Tex ;
		vertBlend = (VERTEX_BLENDTEX_2D *)GD3D9.Device.DrawInfo.VertexBuffer_BlendTex ;
		
		for( i = 0 ; i < D3DDEV_NOTEX_VERTMAXNUM ; i ++, vertNoTex ++ )
		{
			vertNoTex->pos.z = 0.0F ;
			vertNoTex->rhw   = 1.0F ;
		}

		for( i = 0 ; i < D3DDEV_TEX_VERTMAXNUM ; i ++, vertTex ++ )
		{
			vertTex->pos.z = 0.0F ;
			vertTex->rhw   = 1.0F ;
		}

		for( i = 0 ; i < D3DDEV_BLENDTEX_VERTMAXNUM ; i ++, vertBlend ++ )
		{
			vertBlend->pos.z    = 0.0F ;
			vertBlend->rhw      = 1.0F ;
			vertBlend->specular = 0xffffffff ;
		}
		
		GD3D9.Device.DrawInfo.VertexBufferAddr[ 0 ][ 0 ] = ( BYTE * )GD3D9.Device.DrawInfo.VertexBuffer_NoTex ;
		GD3D9.Device.DrawInfo.VertexBufferAddr[ 0 ][ 1 ] = ( BYTE * )GD3D9.Device.DrawInfo.VertexBuffer_Tex ;
		GD3D9.Device.DrawInfo.VertexBufferAddr[ 0 ][ 2 ] = ( BYTE * )GD3D9.Device.DrawInfo.VertexBuffer_BlendTex ;
		GD3D9.Device.DrawInfo.VertexBufferAddr[ 1 ][ 0 ] = ( BYTE * )GD3D9.Device.DrawInfo.VertexBuffer_3D ;
		GD3D9.Device.DrawInfo.VertexBufferAddr[ 1 ][ 1 ] = ( BYTE * )GD3D9.Device.DrawInfo.VertexBuffer_3D ;
	}

	// 描画ステータスのセット
	{
		float FloatParam ;

		// テクスチャ描画用設定
		for( i = 0 ; i < USE_TEXTURESTAGE_NUM ; i ++ )
		{
			if( GSYS.DrawSetting.TexAddressModeU[ i ] == 0 )
				GSYS.DrawSetting.TexAddressModeU[ i ] = D_D3DTADDRESS_CLAMP ;
			if( GSYS.DrawSetting.TexAddressModeV[ i ] == 0 )
				GSYS.DrawSetting.TexAddressModeV[ i ] = D_D3DTADDRESS_CLAMP ;
			if( GSYS.DrawSetting.TexAddressModeW[ i ] == 0 )
				GSYS.DrawSetting.TexAddressModeW[ i ] = D_D3DTADDRESS_CLAMP ;
		}
		for( i = 0 ; i < USE_TEXTURESTAGE_NUM ; i ++ )
		{
			// テクスチャアドレスモード設定
			Direct3DDevice9_SetSamplerState( i, D_D3DSAMP_ADDRESSU, ( DWORD )GSYS.DrawSetting.TexAddressModeU[ i ] ) ;
			Direct3DDevice9_SetSamplerState( i, D_D3DSAMP_ADDRESSV, ( DWORD )GSYS.DrawSetting.TexAddressModeV[ i ] ) ;
			Direct3DDevice9_SetSamplerState( i, D_D3DSAMP_ADDRESSW, ( DWORD )GSYS.DrawSetting.TexAddressModeW[ i ] ) ;

			// フィリタリングモードを設定
			switch( GD3D9.Device.State.DrawMode )
			{
			case DX_DRAWMODE_NEAREST :
				GD3D9.Device.State.MagFilter[ i ] = D_D3DTEXF_POINT ;
				GD3D9.Device.State.MinFilter[ i ] = D_D3DTEXF_POINT ;
				GD3D9.Device.State.MipFilter[ i ] = D_D3DTEXF_POINT ;
				break ;

			case DX_DRAWMODE_BILINEAR :
				GD3D9.Device.State.MagFilter[ i ] = D_D3DTEXF_LINEAR ;
				GD3D9.Device.State.MinFilter[ i ] = D_D3DTEXF_LINEAR ;
				GD3D9.Device.State.MipFilter[ i ] = D_D3DTEXF_LINEAR ;
				break ;

			case DX_DRAWMODE_ANISOTROPIC :
				GD3D9.Device.State.MagFilter[ i ] = D_D3DTEXF_ANISOTROPIC ;
				GD3D9.Device.State.MinFilter[ i ] = D_D3DTEXF_ANISOTROPIC ;
				GD3D9.Device.State.MipFilter[ i ] = D_D3DTEXF_LINEAR ;
				break ;
			}
			Direct3DDevice9_SetSamplerState( i, D_D3DSAMP_MAGFILTER, GD3D9.Device.State.MagFilter[ i ] ) ;
			Direct3DDevice9_SetSamplerState( i, D_D3DSAMP_MINFILTER, GD3D9.Device.State.MinFilter[ i ] ) ;
			Direct3DDevice9_SetSamplerState( i, D_D3DSAMP_MIPFILTER, GD3D9.Device.State.MipFilter[ i ] ) ;

			// 初期最大異方性をセット
			GD3D9.Device.State.MaxAnisotropyDim[ i ] = 1 ;
		}

		for( i = 0 ; i < 4 ; i ++ )
		{
			// テクスチャアドレスモード設定
			Direct3DDevice9_SetSamplerState( D_D3DDMAPSAMPLER + 1 + i, D_D3DSAMP_ADDRESSU, D_D3DTADDRESS_CLAMP ) ;
			Direct3DDevice9_SetSamplerState( D_D3DDMAPSAMPLER + 1 + i, D_D3DSAMP_ADDRESSV, D_D3DTADDRESS_CLAMP ) ;
			Direct3DDevice9_SetSamplerState( D_D3DDMAPSAMPLER + 1 + i, D_D3DSAMP_ADDRESSW, D_D3DTADDRESS_CLAMP ) ;

			// フィリタリングモードを設定
			Direct3DDevice9_SetSamplerState( D_D3DDMAPSAMPLER + 1 + i, D_D3DSAMP_MAGFILTER, D_D3DTEXF_POINT ) ;
			Direct3DDevice9_SetSamplerState( D_D3DDMAPSAMPLER + 1 + i, D_D3DSAMP_MINFILTER, D_D3DTEXF_POINT ) ;
			Direct3DDevice9_SetSamplerState( D_D3DDMAPSAMPLER + 1 + i, D_D3DSAMP_MIPFILTER, D_D3DTEXF_NONE ) ;
		}

		// 最後のドット描画しない
//		Direct3DDevice9_SetRenderState( D_D3DRS_LASTPIXEL, TRUE ) ;

		// カリングオフ
		Direct3DDevice9_SetRenderState( D_D3DRS_CULLMODE, D_D3DCULL_NONE ) ;

		// ライティングＯＮ
		Direct3DDevice9_SetRenderState( D_D3DRS_LIGHTING, TRUE ) ;

		// フラットシェーディング
		Direct3DDevice9_SetRenderState( D_D3DRS_SHADEMODE, D_D3DSHADE_FLAT ) ;

		// 描画ブレンドなし
//		Direct3DDevice9_SetRenderState( D_D3DRS_ALPHABLENDENABLE, FALSE ) ;

		// ディザリングＯＦＦ
		Direct3DDevice9_SetRenderState( D_D3DRS_DITHERENABLE, FALSE ) ;

		// スペキュラライトＯＦＦ
		Direct3DDevice9_SetRenderState( D_D3DRS_SPECULARENABLE, TRUE ) ;

		// アンビエントライトソースはマテリアル
		Direct3DDevice9_SetRenderState( D_D3DRS_AMBIENTMATERIALSOURCE, D_D3DMCS_MATERIAL  ) ;

		// 法線の正規化を行う
		Direct3DDevice9_SetRenderState( D_D3DRS_NORMALIZENORMALS, TRUE ) ;

		// フォグは無効
		Direct3DDevice9_SetRenderState( D_D3DRS_FOGENABLE, FALSE ) ;
		Direct3DDevice9_SetRenderState( D_D3DRS_FOGVERTEXMODE, D_D3DFOG_NONE ) ;
		Direct3DDevice9_SetRenderState( D_D3DRS_FOGCOLOR, 0 ) ;
		FloatParam = 0.0f ;
		Direct3DDevice9_SetRenderState( D_D3DRS_FOGSTART, *( ( DWORD * )&FloatParam ) ) ;
		FloatParam = 1.0f ;
		Direct3DDevice9_SetRenderState( D_D3DRS_FOGEND, *( ( DWORD * )&FloatParam ) ) ;

		// マテリアルのパラメータを取得
		{
			D_D3DMATERIAL9 Material ;

			Direct3DDevice9_GetMaterial( &Material ) ;
			GD3D9.Device.State.Material.Diffuse.r = Material.Diffuse.r ;
			GD3D9.Device.State.Material.Diffuse.g = Material.Diffuse.g ;
			GD3D9.Device.State.Material.Diffuse.b = Material.Diffuse.b ;
			GD3D9.Device.State.Material.Diffuse.a = Material.Diffuse.a ;
			GD3D9.Device.State.Material.Specular.r = Material.Specular.r ;
			GD3D9.Device.State.Material.Specular.g = Material.Specular.g ;
			GD3D9.Device.State.Material.Specular.b = Material.Specular.b ;
			GD3D9.Device.State.Material.Specular.a = Material.Specular.a ;
			GD3D9.Device.State.Material.Emissive.r = Material.Emissive.r ;
			GD3D9.Device.State.Material.Emissive.g = Material.Emissive.g ;
			GD3D9.Device.State.Material.Emissive.b = Material.Emissive.b ;
			GD3D9.Device.State.Material.Emissive.a = Material.Emissive.a ;
			GD3D9.Device.State.Material.Ambient.r = Material.Ambient.r ;
			GD3D9.Device.State.Material.Ambient.g = Material.Ambient.g ;
			GD3D9.Device.State.Material.Ambient.b = Material.Ambient.b ;
			GD3D9.Device.State.Material.Ambient.a = Material.Ambient.a ;
			GD3D9.Device.State.Material.Power = Material.Power ;
		}
	}

	// ブレンディングパラメータを初期化する
	_MEMSET( &GD3D9.Device.State.BlendInfo, 0, sizeof( GRAPHICS_HARDDATA_DIRECT3D9_BLENDINFO ) ) ;
	GD3D9.Device.State.BlendInfo.UseTextureStageNum = 0 ;
	for( i = 0 ; i < USE_TEXTURESTAGE_NUM ; i ++ )
	{
		GD3D9.Device.State.BlendInfo.TextureStageInfo[i].Texture       = NULL ;
		GD3D9.Device.State.BlendInfo.TextureStageInfo[i].ResultTempARG = FALSE ;
		GD3D9.Device.State.BlendInfo.TextureStageInfo[i].AlphaARG1     = D_D3DTA_TEXTURE ;
		GD3D9.Device.State.BlendInfo.TextureStageInfo[i].AlphaARG2     = D_D3DTA_DIFFUSE ;
		GD3D9.Device.State.BlendInfo.TextureStageInfo[i].AlphaOP       = D_D3DTOP_DISABLE ;
		GD3D9.Device.State.BlendInfo.TextureStageInfo[i].ColorARG1     = D_D3DTA_TEXTURE ;
		GD3D9.Device.State.BlendInfo.TextureStageInfo[i].ColorARG2     = D_D3DTA_DIFFUSE ;
		GD3D9.Device.State.BlendInfo.TextureStageInfo[i].ColorOP       = D_D3DTOP_DISABLE ;
	}

	GD3D9.Device.State.Lighting				= TRUE ;
	GD3D9.Device.State.MaxAnisotropy		= 1 ;
	GD3D9.Device.State.ShadeMode			= DX_SHADEMODE_FLAT ;
	GD3D9.Device.State.ZEnable				= TRUE ;
	GD3D9.Device.State.ZWriteEnable			= FALSE ; 
	GD3D9.Device.State.ZFunc				= DX_CMP_LESSEQUAL ;
	GD3D9.Device.State.DepthBias			= 0 ;
	GD3D9.Device.State.FillMode				= D_D3DFILL_SOLID ;
	GD3D9.Device.State.CullMode				= FALSE ;
	GD3D9.Device.State.UseSpecular			= TRUE ;
	GD3D9.Device.State.FogEnable			= FALSE ;
	GD3D9.Device.State.FogColor				= 0 ;
	GD3D9.Device.State.FogMode				= DX_FOGMODE_NONE ;
	GD3D9.Device.State.FogStart				= 0.0f ;
	GD3D9.Device.State.FogEnd				= 1.0f ;
	for( i = 0 ; i < USE_TEXTURESTAGE_NUM ; i ++ )
	{
		GD3D9.Device.State.TexAddressModeU[ i ] = GSYS.DrawSetting.TexAddressModeU[ i ] ;
		GD3D9.Device.State.TexAddressModeV[ i ] = GSYS.DrawSetting.TexAddressModeV[ i ] ;
		GD3D9.Device.State.TexAddressModeW[ i ] = GSYS.DrawSetting.TexAddressModeW[ i ] ;
	}

	GD3D9.Device.State.Viewport.X					= 0 ;
	GD3D9.Device.State.Viewport.Y					= 0 ;
	GD3D9.Device.State.Viewport.Width				= ( DWORD )GSYS.Screen.MainScreenSizeX ;
	GD3D9.Device.State.Viewport.Height				= ( DWORD )GSYS.Screen.MainScreenSizeY ;
	GD3D9.Device.State.Viewport.MinZ				= 0.0f ;
	GD3D9.Device.State.Viewport.MaxZ				= 1.0f ;
	GD3D9.Device.State.DrawMode						= DX_DRAWMODE_OTHER ;
	GD3D9.Device.State.TargetSurface[ 0 ]			= GD3D9.Device.Screen.BackBufferSurface ;

	GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag		= TRUE ;
	GD3D9.Device.DrawSetting.UseDiffuseRGBColor		= FALSE ;
	GD3D9.Device.DrawSetting.UseDiffuseAlphaColor	= FALSE ;
	GD3D9.Device.DrawSetting.BlendMode				= DX_BLENDMODE_NOBLEND ;
	GD3D9.Device.DrawSetting.BlendGraphBorderParam	= -1 ;
	GD3D9.Device.DrawSetting.AlphaTestMode			= -1 ;
	GD3D9.Device.DrawSetting.AlphaTestParam			= 0 ;
	GD3D9.Device.DrawSetting.AlphaChannelValidFlag	= FALSE ;
	GD3D9.Device.DrawSetting.AlphaTestValidFlag		= FALSE ;
	GD3D9.Device.DrawSetting.ChangeBlendParamFlag	= TRUE ;
	GD3D9.Device.DrawSetting.ChangeTextureFlag		= FALSE ;
	GD3D9.Device.DrawSetting.BlendTexture			= NULL ;
	GD3D9.Device.DrawSetting.BlendGraphType			= 0 ;
	GD3D9.Device.DrawSetting.RenderTexture			= NULL ;

	GD3D9.Device.DrawInfo.DiffuseColor				= 0xffffffff ;
	GD3D9.Device.DrawInfo.BlendMaxNotDrawFlag		= FALSE ;
	GD3D9.Device.DrawInfo.BeginSceneFlag			= FALSE ;
	GD3D9.Device.DrawInfo.VertexNum					= 0 ;
	GD3D9.Device.DrawInfo.VertexBufferNextAddr		= GD3D9.Device.DrawInfo.VertexBuffer_Tex ;
	GD3D9.Device.DrawInfo.VertexType				= VERTEXTYPE_TEX ;

//	GRH.PerspectiveFlag			= FALSE ;
//	GRH.AlphaValidFlag			= FALSE ;
//	GRH.AlphaBlendEnable		= FALSE ;
//	GRH.BlendParam				= 0 ;
//	GRH.VertexBufferUseFlag		= FALSE ;
//	GRH.ColorKeyFlag			= FALSE ;

	// 設定をリフレッシュ
	Graphics_D3D9_DeviceState_RefreshRenderState() ;

	// おまじない
	{
		VERTEX_NOTEX_2D *vec ;

		Graphics_D3D9_BeginScene() ;
		Graphics_D3D9_DrawPreparation( 0 ) ;

//		GETVERTEX_POINT( vec ) ;
		GD3D9.Device.DrawInfo.VertexNum            = 0 ;
		GD3D9.Device.DrawInfo.VertexBufferNextAddr = GD3D9.Device.DrawInfo.VertexBufferAddr[ 0 ][ GD3D9.Device.DrawInfo.VertexType ] ;
		GD3D9.Device.DrawInfo.Use3DVertex          = 0 ;
		GD3D9.Device.DrawInfo.PrimitiveType        = D_D3DPT_POINTLIST ;
		*( (DWORD_PTR *)&vec) = (DWORD_PTR)GD3D9.Device.DrawInfo.VertexBufferNextAddr ;


		vec->pos.x = 0.0f ;
		vec->pos.y = 0.0f ;
		vec->pos.z = 0.0f ;


//		ADD4VERTEX_POINT
		GD3D9.Device.DrawInfo.VertexBufferNextAddr += sizeof( VERTEX_NOTEX_2D ) ;
		GD3D9.Device.DrawInfo.VertexNum ++ ;
	}

	// 終了
	return 0 ;
}

// 一時的に Direct3DDevice9 を破棄した際に、破棄前の状態を作り出すための関数
extern	int		Graphics_D3D9_Device_ReInitialize( void )
{
	int i ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	// 描画ステータスのセット
	{
		// テクスチャ描画用設定
//		Direct3DDevice9_SetTextureStageState( 0, D_D3DTSS_ADDRESS, D_D3DTADDRESS_CLAMP );

		// ディザリングＯＦＦ
		Direct3DDevice9_SetRenderState( D_D3DRS_DITHERENABLE, FALSE ) ;
	}

	// 初期化フラグを立てる
	GD3D9.Device.DrawSetting.CancelSettingEqualCheck = TRUE ;

	// 必ず Graphics_D3D9_DrawPreparation を実行するフラグを立てる
	GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// 削除前と同じ環境を作り出す
	{
		// 描画先の設定
//		if( GD3D9.Device.State.TargetSurface != NULL )
//			SetRenderTargetHardware( GD3D9.Device.State.TargetSurface ) ;

		// テクスチャのリセット
		GD3D9.Device.DrawSetting.RenderTexture = NULL ;

		// 描画頂点もリセット
		GD3D9.Device.DrawInfo.VertexNum = 0 ;

		// 描画ブレンドモードの設定
		Graphics_D3D9_DrawSetting_SetDrawBlendMode( GD3D9.Device.DrawSetting.BlendMode, GD3D9.Device.DrawSetting.AlphaTestValidFlag, GD3D9.Device.DrawSetting.AlphaChannelValidFlag ) ;
//		Graphics_D3D9_DeviceState_RefreshBlendState( DevHandle ) ;

		// 描画画像の色を無視するかどうかをセットする
		Graphics_D3D9_DrawSetting_SetIgnoreDrawGraphColor( GD3D9.Device.DrawSetting.IgnoreGraphColorFlag ) ;

		// カリングモードのセット
		Graphics_D3D9_DeviceState_SetCullMode( GD3D9.Device.State.CullMode ) ;

		// 描画モードの設定
		Graphics_D3D9_DeviceState_SetDrawMode( GD3D9.Device.State.DrawMode ) ;

		// シェーディングモードのセット
		Graphics_D3D9_DeviceState_SetShadeMode( GD3D9.Device.State.ShadeMode ) ;

		// 異方性フィルタリングの設定をセット
		Graphics_D3D9_DeviceState_SetMaxAnisotropy( GD3D9.Device.State.MaxAnisotropy ) ;

		// カリングモードの設定をセット
		Graphics_D3D9_DeviceState_SetCullMode( GD3D9.Device.State.CullMode ) ;

		// UVアドレッシングモードの設定をセット
		for( i = 0 ; i < 8 ; i ++ )
		{
			Graphics_D3D9_DeviceState_SetTextureAddressU( GD3D9.Device.State.TexAddressModeU[ i ], i ) ;
			Graphics_D3D9_DeviceState_SetTextureAddressV( GD3D9.Device.State.TexAddressModeV[ i ], i ) ;
			Graphics_D3D9_DeviceState_SetTextureAddressW( GD3D9.Device.State.TexAddressModeW[ i ], i ) ;
		}

		// テクスチャ座標変換行列を使用するかどうかの設定をセット
		for( i = 0 ; i < 8 ; i ++ )
		{
			Graphics_D3D9_DeviceState_SetTextureAddressTransformMatrix( GD3D9.Device.State.TextureTransformUse[ i ], &GD3D9.Device.State.TextureTransformMatrix[ i ], i ) ;
		}

		// フォグの設定をセット
		Graphics_D3D9_DeviceState_SetFogEnable(     GD3D9.Device.State.FogEnable ) ;
		Graphics_D3D9_DeviceState_SetFogVertexMode( GD3D9.Device.State.FogMode ) ;
		Graphics_D3D9_DeviceState_SetFogColor(      GD3D9.Device.State.FogColor ) ;
		Graphics_D3D9_DeviceState_SetFogStartEnd(   GD3D9.Device.State.FogStart, GD3D9.Device.State.FogEnd ) ;
		Graphics_D3D9_DeviceState_SetFogDensity(    GD3D9.Device.State.FogDensity ) ;

		// Ｚバッファの設定をセット
		Graphics_D3D9_DeviceState_SetZEnable(       GD3D9.Device.State.ZEnable ) ;
		Graphics_D3D9_DeviceState_SetZWriteEnable(  GD3D9.Device.State.ZWriteEnable ) ;
		Graphics_D3D9_DeviceState_SetZFunc(         GD3D9.Device.State.ZFunc ) ;
		Graphics_D3D9_DeviceState_SetDepthBias(     GD3D9.Device.State.DepthBias ) ;
		Graphics_D3D9_DeviceState_SetFillMode(      GD3D9.Device.State.FillMode ) ;

		// 行列の再設定
		NS_SetTransformToWorldD(                              &GSYS.DrawSetting.WorldMatrix ) ;
		NS_SetTransformToViewD(                               &GSYS.DrawSetting.ViewMatrix ) ;
		Graphics_DrawSetting_SetTransformToProjection_Direct( &GSYS.DrawSetting.ProjectionMatrix ) ;
		NS_SetTransformToViewportD(                           &GSYS.DrawSetting.ViewportMatrix ) ;

		// マテリアルの再設定
		Graphics_D3D9_DeviceState_SetMaterial(               &GD3D9.Device.State.Material ) ;
		Graphics_D3D9_DeviceState_SetSpecularEnable(          GD3D9.Device.State.UseSpecular ) ;
		Graphics_D3D9_DeviceState_SetDiffuseMaterialSource(   GD3D9.Device.State.MaterialUseVertexDiffuseColor ) ;
		Graphics_D3D9_DeviceState_SetSpecularMaterialSource(  GD3D9.Device.State.MaterialUseVertexSpecularColor ) ;

		// ライトの再設定
		Graphics_D3D9_DeviceState_SetAmbient(
			( ( ( DWORD )( _FTOL( GD3D9.Device.State.GlobalAmbientLightColor.a ) * 255 ) ) << 24 ) | 
			( ( ( DWORD )( _FTOL( GD3D9.Device.State.GlobalAmbientLightColor.r ) * 255 ) ) << 16 ) | 
			( ( ( DWORD )( _FTOL( GD3D9.Device.State.GlobalAmbientLightColor.g ) * 255 ) ) << 8  ) | 
			( ( ( DWORD )( _FTOL( GD3D9.Device.State.GlobalAmbientLightColor.b ) * 255 ) ) << 0  ) ) ;
		Graphics_D3D9_DeviceState_SetLighting( GD3D9.Device.State.Lighting ) ;
		for( i = 0 ; i < 256 ; i ++ )
		{
			if( GD3D9.Device.State.LightParam[ i ].Type == 0 ) continue ;
			Graphics_D3D9_DeviceState_SetLightState( i, &GD3D9.Device.State.LightParam[ i ] ) ;
			Graphics_D3D9_DeviceState_SetLightEnable( i, GD3D9.Device.State.LightEnableFlag[ i ] ) ;
		}

		// 描画領域の設定
		{
			D_D3DVIEWPORT9 Viewport ;

			// ビューポートのセッティング
			_MEMSET( &Viewport, 0, sizeof( Viewport ) ) ;
			Viewport.X		= ( DWORD )GSYS.DrawSetting.DrawArea.left ;
			Viewport.Y		= ( DWORD )GSYS.DrawSetting.DrawArea.top ;
			Viewport.Width	= ( DWORD )( GSYS.DrawSetting.DrawArea.right  - GSYS.DrawSetting.DrawArea.left ) ;
			Viewport.Height	= ( DWORD )( GSYS.DrawSetting.DrawArea.bottom - GSYS.DrawSetting.DrawArea.top  ) ;
			Viewport.MinZ	= 0.0f ;
			Viewport.MaxZ	= 1.0f ;

			// セット
			Graphics_D3D9_DeviceState_SetViewport( &Viewport ) ;
			Graphics_D3D9_DeviceState_SetScissorRect( &GSYS.DrawSetting.DrawArea ) ;
		}

		// 各ステージで使用するテクスチャ座標のリセット
		Graphics_D3D9_DeviceState_ResetTextureCoord() ;
	
		// 設定を反映
		Graphics_D3D9_DeviceState_RefreshBlendState( NULL, 0, 1, FALSE ) ;
		Graphics_D3D9_DeviceState_RefreshBlendState( NULL, 0, 1, TRUE ) ;
		Graphics_D3D9_DeviceState_RefreshRenderState() ;
	}

	// 初期化フラグを倒す
	GD3D9.Device.DrawSetting.CancelSettingEqualCheck = FALSE ;

	// 終了
	return 0 ;
}
























// Direct3DDevice9 ステータス関係

// 描画設定をリフレッシュ
extern	void	Graphics_D3D9_DeviceState_RefreshRenderState( void )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return ;

	// 初期化フラグを立てる
	GD3D9.Device.DrawSetting.CancelSettingEqualCheck = TRUE ;

	// Graphics_D3D9_DrawPreparation を行うべきフラグを立てる
	GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// 設定のリフレッシュ
	Graphics_D3D9_DeviceState_SetFogEnable             ( GD3D9.Device.State.FogEnable           ) ;
	Graphics_D3D9_DeviceState_SetViewport              ( &GD3D9.Device.State.Viewport           ) ;
	Graphics_D3D9_DeviceState_SetScissorRect           ( &GD3D9.Device.State.ScissorRect        ) ;
	Graphics_D3D9_DrawSetting_SetTexture               ( GD3D9.Device.DrawSetting.RenderTexture ) ;
	Graphics_D3D9_DeviceState_SetBaseState             (                                        ) ;
	Graphics_D3D9_DeviceState_SetDrawMode              ( GD3D9.Device.State.DrawMode            ) ;
//	Graphics_D3D9_DrawSetting_SetDrawBlendMode         ( GD3D9.Device.DrawSetting.BlendMode, GD3D9.Device.DrawSetting.AlphaTestValidFlag, GD3D9.Device.DrawSetting.AlphaChannelValidFlag  ) ;
	Graphics_D3D9_DeviceState_RefreshBlendState        ( NULL, 0, 1, FALSE                      ) ;
	Graphics_D3D9_DeviceState_RefreshBlendState        ( NULL, 0, 1, TRUE                       ) ;
	Graphics_D3D9_DeviceState_SetZEnable               ( GD3D9.Device.State.ZEnable             ) ;
	Graphics_D3D9_DeviceState_SetZWriteEnable          ( GD3D9.Device.State.ZWriteEnable        ) ;
	Graphics_D3D9_DeviceState_SetZFunc                 ( GD3D9.Device.State.ZFunc               ) ;
	Graphics_D3D9_DeviceState_SetDepthBias             ( GD3D9.Device.State.DepthBias           ) ;
	Graphics_D3D9_DeviceState_SetFillMode              ( GD3D9.Device.State.FillMode            ) ;
	Graphics_D3D9_DeviceState_SetCullMode              ( GD3D9.Device.State.CullMode            ) ;
	Graphics_D3D9_DeviceState_SetShadeMode             ( GD3D9.Device.State.ShadeMode           ) ;
	Graphics_D3D9_DeviceState_SetMaterial              ( &GD3D9.Device.State.Material           ) ;
	Graphics_D3D9_DeviceState_SetSpecularEnable        ( GD3D9.Device.State.UseSpecular         ) ;
	Graphics_D3D9_DeviceState_SetDiffuseMaterialSource ( GD3D9.Device.State.MaterialUseVertexDiffuseColor ) ;
	Graphics_D3D9_DeviceState_SetSpecularMaterialSource( GD3D9.Device.State.MaterialUseVertexSpecularColor ) ;

	if( GAPIWin.Direct3DDevice9Object != NULL )
	{
		Direct3DDevice9_SetVertexShaderConstantF( 0, ( float * )GD3D9.Device.Shader.ShaderConstantInfo.FixInfo[ DX_SHADERCONSTANTTYPE_VS_FLOAT ].Float4, 256 ) ;
		Direct3DDevice9_SetVertexShaderConstantI( 0, ( int   * )GD3D9.Device.Shader.ShaderConstantInfo.FixInfo[ DX_SHADERCONSTANTTYPE_VS_INT   ].Int4,   16  ) ;
		Direct3DDevice9_SetVertexShaderConstantB( 0, ( BOOL  * )GD3D9.Device.Shader.ShaderConstantInfo.FixInfo[ DX_SHADERCONSTANTTYPE_VS_BOOL  ].Bool,   16  ) ;

		Direct3DDevice9_SetPixelShaderConstantF( 0, ( float * )GD3D9.Device.Shader.ShaderConstantInfo.FixInfo[ DX_SHADERCONSTANTTYPE_PS_FLOAT ].Float4, 224 ) ;
		Direct3DDevice9_SetPixelShaderConstantI( 0, ( int   * )GD3D9.Device.Shader.ShaderConstantInfo.FixInfo[ DX_SHADERCONSTANTTYPE_PS_INT   ].Int4,   16  ) ;
		Direct3DDevice9_SetPixelShaderConstantB( 0, ( BOOL  * )GD3D9.Device.Shader.ShaderConstantInfo.FixInfo[ DX_SHADERCONSTANTTYPE_PS_BOOL  ].Bool,   16  ) ;
	}

	// 初期化フラグを倒す
	GD3D9.Device.DrawSetting.CancelSettingEqualCheck = FALSE ;
}

// Direct3DDevice9 の基本設定を行う
extern	int		Graphics_D3D9_DeviceState_SetBaseState( void )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		// 描画待機している描画物を描画
		DRAWSTOCKINFO
	}

	// アルファブレンディングカラーキー無効
//	Direct3DDevice9_SetRenderState( D_D3DRS_COLORKEYBLENDENABLE, FALSE ) ;

	// 最後のドット描画しない
//	Direct3DDevice9_SetRenderState( D_D3DRS_LASTPIXEL, TRUE ) ;

	// テクスチャ描画用設定
	Direct3DDevice9_SetSamplerState( 0, D_D3DSAMP_ADDRESSU, ( DWORD )GD3D9.Device.State.TexAddressModeU[ 0 ] ) ;
	Direct3DDevice9_SetSamplerState( 0, D_D3DSAMP_ADDRESSV, ( DWORD )GD3D9.Device.State.TexAddressModeV[ 0 ] ) ;
	Direct3DDevice9_SetSamplerState( 1, D_D3DSAMP_ADDRESSU, ( DWORD )GD3D9.Device.State.TexAddressModeU[ 1 ] ) ;
	Direct3DDevice9_SetSamplerState( 1, D_D3DSAMP_ADDRESSV, ( DWORD )GD3D9.Device.State.TexAddressModeV[ 1 ] ) ;
	Direct3DDevice9_SetSamplerState( 2, D_D3DSAMP_ADDRESSU, ( DWORD )GD3D9.Device.State.TexAddressModeU[ 2 ] ) ;
	Direct3DDevice9_SetSamplerState( 2, D_D3DSAMP_ADDRESSV, ( DWORD )GD3D9.Device.State.TexAddressModeV[ 2 ] ) ;
	Direct3DDevice9_SetSamplerState( 3, D_D3DSAMP_ADDRESSU, ( DWORD )GD3D9.Device.State.TexAddressModeU[ 3 ] ) ;
	Direct3DDevice9_SetSamplerState( 3, D_D3DSAMP_ADDRESSV, ( DWORD )GD3D9.Device.State.TexAddressModeV[ 3 ] ) ;
	Direct3DDevice9_SetTextureStageState( 0, D_D3DTSS_TEXCOORDINDEX, 0 ) ;
	Direct3DDevice9_SetTextureStageState( 1, D_D3DTSS_TEXCOORDINDEX, 1 ) ;
	Direct3DDevice9_SetTextureStageState( 2, D_D3DTSS_TEXCOORDINDEX, 1 ) ;
	Direct3DDevice9_SetTextureStageState( 3, D_D3DTSS_TEXCOORDINDEX, 0 ) ;
	GD3D9.Device.State.BlendInfo.TextureStageInfo[ 0 ].TextureCoordIndex = 0 ;
	GD3D9.Device.State.BlendInfo.TextureStageInfo[ 1 ].TextureCoordIndex = 1 ;
	GD3D9.Device.State.BlendInfo.TextureStageInfo[ 2 ].TextureCoordIndex = 1 ;
	GD3D9.Device.State.BlendInfo.TextureStageInfo[ 3 ].TextureCoordIndex = 0 ;
	if( GD3D9.Device.Caps.ValidTexTempRegFlag )
	{
		Direct3DDevice9_SetTextureStageState( 0, D_D3DTSS_RESULTARG, D_D3DTA_CURRENT ) ;
		Direct3DDevice9_SetTextureStageState( 1, D_D3DTSS_RESULTARG, D_D3DTA_CURRENT ) ;
		Direct3DDevice9_SetTextureStageState( 2, D_D3DTSS_RESULTARG, D_D3DTA_CURRENT ) ;
		Direct3DDevice9_SetTextureStageState( 3, D_D3DTSS_RESULTARG, D_D3DTA_CURRENT ) ;
		GD3D9.Device.State.BlendInfo.TextureStageInfo[ 0 ].ResultTempARG = FALSE ;
		GD3D9.Device.State.BlendInfo.TextureStageInfo[ 1 ].ResultTempARG = FALSE ;
		GD3D9.Device.State.BlendInfo.TextureStageInfo[ 2 ].ResultTempARG = FALSE ;
		GD3D9.Device.State.BlendInfo.TextureStageInfo[ 3 ].ResultTempARG = FALSE ;
	}

	// パースペクティブコレクト設定
//	Direct3DDevice9_SetRenderState( D_D3DRS_TEXTUREPERSPECTIVE, GRH.PerspectiveFlag ) ;

	// シェーディングモード設定
	Direct3DDevice9_SetRenderState( D_D3DRS_SHADEMODE, ( DWORD )GD3D9.Device.State.ShadeMode ) ;

	// ライティング設定
	Direct3DDevice9_SetRenderState( D_D3DRS_LIGHTING, ( DWORD )GD3D9.Device.State.Lighting ) ;

	// カリング設定
	Direct3DDevice9_SetRenderState( D_D3DRS_CULLMODE, D_D3DCULL_NONE ) ;

	// ディザリングＯＦＦ
	Direct3DDevice9_SetRenderState( D_D3DRS_DITHERENABLE, FALSE ) ;
	
	// シェーダーの定数をセット
	static float ZeroOne[ 4 ]        = { 0.0f, 1.0f, 0.0f, 0.0f } ;
	static float ZeroHalfOneTwo[ 4 ] = { 0.0f, 0.5f, 1.0f, 2.0f } ;
	Graphics_D3D9_ShaderConstant_InfoSet_SetParam( &GD3D9.Device.Shader.ShaderConstantInfo, DX_SHADERCONSTANTTYPE_VS_FLOAT, DX_SHADERCONSTANTSET_LIB, DX_VS_CONSTF_ZERO_ONE,          ZeroOne,        1, TRUE ) ;
	Graphics_D3D9_ShaderConstant_InfoSet_SetParam( &GD3D9.Device.Shader.ShaderConstantInfo, DX_SHADERCONSTANTTYPE_PS_FLOAT, DX_SHADERCONSTANTSET_LIB, DX_PS_CONSTF_ZERO_HALF_ONE_TWO, ZeroHalfOneTwo, 1, TRUE ) ;

	// 固定機能パイプラインに戻す
	Graphics_D3D9_DeviceState_ResetVertexShader() ;
	Graphics_D3D9_DeviceState_ResetPixelShader() ;
	Graphics_D3D9_DeviceState_SetVertexBuffer( NULL ) ;
	Graphics_D3D9_DeviceState_SetIndexBuffer( NULL ) ;

	// 終了
	return 0 ;
}

// サンプラーのテクスチャフィルタリングモードを設定する
extern int	Graphics_D3D9_DeviceState_SetSampleFilterMode( int Sampler, int SetTarget, D_D3DTEXTUREFILTERTYPE FilterType )
{
	GRAPHICS_HARDDATA_DIRECT3D9_DEVICE_STATE *State = &GraphicsHardDataDirect3D9.Device.State ;

	if( GAPIWin.Direct3DDevice9Object == NULL )
	{
		return 0 ;
	}

	if( Sampler < 0 || Sampler >= USE_TEXTURESTAGE_NUM )
	{
		return 0 ;
	}

	GD3D9.Device.State.DrawMode           = DX_DRAWMODE_OTHER ;
	GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	switch( SetTarget )
	{
	case D_D3DTSS_MAGFILTER :
		if( State->MagFilter[ Sampler ] != FilterType )
		{
			Direct3DDevice9_SetSamplerState( ( DWORD )Sampler, D_D3DSAMP_MAGFILTER, FilterType ) ;
			State->MagFilter[ Sampler ] = FilterType ;
		}
		break ;

	case D_D3DTSS_MINFILTER :
		if( State->MinFilter[ Sampler ] != FilterType )
		{
			Direct3DDevice9_SetSamplerState( ( DWORD )Sampler, D_D3DSAMP_MINFILTER, FilterType ) ;
			State->MinFilter[ Sampler ] = FilterType ;
		}
		break ;

	case D_D3DTSS_MIPFILTER :
		if( State->MipFilter[ Sampler ] != FilterType )
		{
			Direct3DDevice9_SetSamplerState( ( DWORD )Sampler, D_D3DSAMP_MIPFILTER, FilterType ) ;
			State->MipFilter[ Sampler ] = FilterType ;
		}
		break ;
	}

	// 終了
	return 0 ;
}


// Ｚバッファの有効無効のセット
extern int  Graphics_D3D9_DeviceState_SetZEnable( int Flag )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	// フラグが同じだったら何もせず終了
	if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE && GD3D9.Device.State.ZEnable == Flag ) return 0 ;

	if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		// 描画待機している描画物を描画
		DRAWSTOCKINFO
	}

	// Ｚバッファの有効状態を変更
	Direct3DDevice9_SetRenderState( D_D3DRS_ZENABLE, Flag == TRUE ? D_D3DZB_TRUE : D_D3DZB_FALSE ) ;
	GD3D9.Device.State.ZEnable = Flag ;
	GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// 終了
	return 0 ;
}

// Ｚバッファの書き込みの有無をセット
extern int  Graphics_D3D9_DeviceState_SetZWriteEnable( int Flag )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	// フラグが同じだったら何もせず終了
	if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE && GD3D9.Device.State.ZWriteEnable == Flag ) return 0 ;

	if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		// 描画待機している描画物を描画
		DRAWSTOCKINFO
	}

	// Ｚバッファの書き込み状態を変更
	Direct3DDevice9_SetRenderState( D_D3DRS_ZWRITEENABLE, ( DWORD )Flag ) ;
	GD3D9.Device.State.ZWriteEnable = Flag ;
	GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// 終了
	return 0 ;
}

// 深度値の比較タイプをセット
extern int  Graphics_D3D9_DeviceState_SetZFunc( int CmpType )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	// タイプが同じだったら何もせず終了
	if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE && GD3D9.Device.State.ZFunc == CmpType ) return 0 ;

	if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		// 描画待機している描画物を描画
		DRAWSTOCKINFO
	}

	// Ｚ値の比較タイプを変更
	Direct3DDevice9_SetRenderState( D_D3DRS_ZFUNC, ( DWORD )CmpType ) ;
	GD3D9.Device.State.ZFunc = CmpType ;
	GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// 終了
	return 0 ;
}

// 深度値のバイアスをセット
extern int  Graphics_D3D9_DeviceState_SetDepthBias( int Bias )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	// バイアス値が同じだったら何もせず終了
	if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE && GD3D9.Device.State.DepthBias == Bias ) return 0 ;

	if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		// 描画待機している描画物を描画
		DRAWSTOCKINFO
	}

	// Ｚバイアスを変更
	Direct3DDevice9_SetRenderState( D_D3DRS_DEPTHBIAS, ( DWORD )Bias ) ;
	GD3D9.Device.State.DepthBias = Bias ;
	GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// 終了
	return 0 ;
}

// フィルモードをセット
extern int  Graphics_D3D9_DeviceState_SetFillMode( int FillMode )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	// フィルモードが同じだったら何もせず終了
	if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE && GD3D9.Device.State.FillMode == FillMode ) return 0 ;

	if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		// 描画待機している描画物を描画
		DRAWSTOCKINFO
	}

	// フィルモードを変更
	Direct3DDevice9_SetRenderState( D_D3DRS_FILLMODE, ( DWORD )FillMode ) ;
	GD3D9.Device.State.FillMode = FillMode ;
	GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// 終了
	return 0 ;
}


// ワールド変換用行列をセットする
extern int Graphics_D3D9_DeviceState_SetWorldMatrix( const MATRIX *Matrix )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return 0 ;

	Direct3DDevice9_SetTransform( D_D3DTS_WORLDMATRIX( 0 ), ( D_D3DMATRIX * )Matrix ) ;

//	if( GSYS.HardInfo.UseShader )
	{
		// 転置行列を頂点シェーダーにセットする
		MATRIX TransMat ;
		CreateTransposeMatrix( &TransMat, Matrix ) ;
		Graphics_D3D9_ShaderConstant_InfoSet_SetParam(
			&GD3D9.Device.Shader.ShaderConstantInfo,
			DX_SHADERCONSTANTTYPE_VS_FLOAT,
			DX_SHADERCONSTANTSET_LIB,
			DX_VS_CONSTF_WORLD_MAT_START,
			&TransMat,
			4,
			TRUE
		) ;
	}

	// 終了
	return 0 ;
}

// ビュー変換用行列をセットする
extern int Graphics_D3D9_DeviceState_SetViewMatrix( const MATRIX *Matrix )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return 0 ;

	Direct3DDevice9_SetTransform( D_D3DTS_VIEW, ( D_D3DMATRIX * )Matrix ) ;

//	if( GSYS.HardInfo.UseShader )
	{
		int i ;
		VECTOR Dir, Pos ;
		float Data[ 2 ][ 4 ] ;
		MATRIX RotateTransMat ;

		// 転置行列を頂点シェーダーにセットする
		MATRIX TransMat ;
		CreateTransposeMatrix( &TransMat, Matrix ) ;
		Graphics_D3D9_ShaderConstant_InfoSet_SetParam(
			&GD3D9.Device.Shader.ShaderConstantInfo,
			DX_SHADERCONSTANTTYPE_VS_FLOAT,
			DX_SHADERCONSTANTSET_LIB,
			DX_VS_CONSTF_VIEW_MAT,
			&TransMat,
			4,
			TRUE
		) ;

		// ライトの位置・方向の再計算
		RotateTransMat = *Matrix ;
		RotateTransMat.m[ 3 ][ 0 ] = 0.0f ;
		RotateTransMat.m[ 3 ][ 1 ] = 0.0f ;
		RotateTransMat.m[ 3 ][ 2 ] = 0.0f ;
		for( i = 0 ; i < DX_PS_CONSTB_LIGHT2_NUM ; i ++ )
		{
			VectorTransform( &Dir, ( VECTOR * )&GD3D9.Device.State.LightParam[ i ].Direction, &RotateTransMat ) ;
			VectorTransform( &Pos, ( VECTOR * )&GD3D9.Device.State.LightParam[ i ].Position,  &GSYS.DrawSetting.ViewMatrixF ) ;
			Data[ 0 ][ 0 ] = Pos.x ;
			Data[ 0 ][ 1 ] = Pos.y ;
			Data[ 0 ][ 2 ] = Pos.z ;
			Data[ 0 ][ 3 ] = 1.0f ;
			Data[ 1 ][ 0 ] = Dir.x ;
			Data[ 1 ][ 1 ] = Dir.y ;
			Data[ 1 ][ 2 ] = Dir.z ;
			Data[ 1 ][ 3 ] = 0.0f ;
			if( i < DX_VS_CONSTF_LIGHT_NUM )
			{
				Graphics_D3D9_ShaderConstant_InfoSet_SetParam(
					&GD3D9.Device.Shader.ShaderConstantInfo,
					DX_SHADERCONSTANTTYPE_VS_FLOAT,
					DX_SHADERCONSTANTSET_LIB,
					DX_VS_CONSTF_LIGHT_START + i * DX_VS_CONSTF_LIGHT_UNITSIZE,
					Data[ 0 ],
					2,
					TRUE
				) ;
			}
			Graphics_D3D9_ShaderConstant_InfoSet_SetParam(
				&GD3D9.Device.Shader.ShaderConstantInfo,
				DX_SHADERCONSTANTTYPE_PS_FLOAT,
				DX_SHADERCONSTANTSET_LIB,
				DX_PS_CONSTF_LIGHT2_START + i * DX_PS_CONSTF_LIGHT2_UNITSIZE,
				Data[ 0 ],
				2,
				TRUE
			) ;
		}
	}

	// 終了
	return 0 ;
}

// 投影変換用行列をセットする
extern int Graphics_D3D9_DeviceState_SetProjectionMatrix( const MATRIX *Matrix )
{
	MATRIX TransMat ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return 0 ;

	Direct3DDevice9_SetTransform( D_D3DTS_PROJECTION, ( D_D3DMATRIX * )Matrix ) ;

	// 転置行列を頂点シェーダーにセットする
//	if( GSYS.HardInfo.UseShader )
	{
		CreateTransposeMatrix( &TransMat, Matrix ) ;
		Graphics_D3D9_ShaderConstant_InfoSet_SetParam(
			&GD3D9.Device.Shader.ShaderConstantInfo,
			DX_SHADERCONSTANTTYPE_VS_FLOAT,
			DX_SHADERCONSTANTSET_LIB,
			DX_VS_CONSTF_PROJECTION_MAT,
			&TransMat,
			4,
			TRUE
		) ;
	}

	// 終了
	return 0 ;
}

// ライトのセット
extern int Graphics_D3D9_DeviceState_SetLightState( int LightNumber, D_D3DLIGHT9 *Light )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return 0 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// パラメータの保存
	if( LightNumber < 256 && LightNumber >= 0 )
	{
		GD3D9.Device.State.LightParam[ LightNumber ] = *Light ;
	}

	// バーテックスシェーダーとピクセルシェーダーにも反映する
//	if( GSYS.HardInfo.UseShader )
	{
		if( LightNumber < DX_PS_CONSTB_LIGHT2_NUM )
		{
			float Data[ 7 ][ 4 ] ;
			float ThetaSin, ThetaCos ;
			float PhiSin, PhiCos ;
			VECTOR Dir, Pos ;

			Dir = VTransformSR( *( ( VECTOR * )&Light->Direction ), GSYS.DrawSetting.ViewMatrixF ) ;
			Pos = VTransform(   *( ( VECTOR * )&Light->Position ),  GSYS.DrawSetting.ViewMatrixF ) ;
			Data[ 0 ][ 0 ] = Pos.x ;
			Data[ 0 ][ 1 ] = Pos.y ;
			Data[ 0 ][ 2 ] = Pos.z ;
			Data[ 0 ][ 3 ] = 1.0f ;
			Data[ 1 ][ 0 ] = Dir.x ;
			Data[ 1 ][ 1 ] = Dir.y ;
			Data[ 1 ][ 2 ] = Dir.z ;
			Data[ 1 ][ 3 ] = 0.0f ;
			Data[ 2 ][ 0 ] = Light->Diffuse.r ;
			Data[ 2 ][ 1 ] = Light->Diffuse.g ;
			Data[ 2 ][ 2 ] = Light->Diffuse.b ;
			Data[ 2 ][ 3 ] = Light->Diffuse.a ;
			Data[ 3 ][ 0 ] = Light->Specular.r ;
			Data[ 3 ][ 1 ] = Light->Specular.g ;
			Data[ 3 ][ 2 ] = Light->Specular.b ;
			Data[ 3 ][ 3 ] = Light->Specular.a ;
			Data[ 4 ][ 0 ] = Light->Ambient.r ;
			Data[ 4 ][ 1 ] = Light->Ambient.g ;
			Data[ 4 ][ 2 ] = Light->Ambient.b ;
			Data[ 4 ][ 3 ] = Light->Ambient.a ;
			Data[ 5 ][ 0 ] = Light->Range * Light->Range ;
			Data[ 5 ][ 1 ] = Light->Falloff ;
			Data[ 5 ][ 2 ] = Light->Attenuation0 ;
			Data[ 5 ][ 3 ] = Light->Attenuation1 ;
			Data[ 6 ][ 0 ] = Light->Attenuation2 ;
			_SINCOS( Light->Theta / 2.0f, &ThetaSin, &ThetaCos ) ;
			_SINCOS( Light->Phi / 2.0f, &PhiSin, &PhiCos ) ;
			Data[ 6 ][ 1 ] = PhiCos ;
			if( _FABS( ThetaCos - PhiCos ) < 0.0000001f )
			{
				Data[ 6 ][ 2 ] = 100000.0f ;
			}
			else
			{
				Data[ 6 ][ 2 ] = 1.0f / ( ThetaCos - PhiCos ) ;
			}
			Data[ 6 ][ 3 ] = 0.0f ;
			if( LightNumber < DX_VS_CONSTF_LIGHT_NUM )
			{
				Graphics_D3D9_ShaderConstant_InfoSet_SetParam(
					&GD3D9.Device.Shader.ShaderConstantInfo,
					DX_SHADERCONSTANTTYPE_VS_FLOAT,
					DX_SHADERCONSTANTSET_LIB,
					DX_VS_CONSTF_LIGHT_START + LightNumber * DX_VS_CONSTF_LIGHT_UNITSIZE,
					Data[ 0 ],
					DX_VS_CONSTF_LIGHT_UNITSIZE,
					TRUE
				) ;
				Graphics_D3D9_ShaderConstant_InfoSet_SetParam(
					&GD3D9.Device.Shader.ShaderConstantInfo,
					DX_SHADERCONSTANTTYPE_PS_FLOAT,
					DX_SHADERCONSTANTSET_LIB,
					DX_PS_CONSTF_LIGHT_START  + LightNumber * DX_PS_CONSTF_LIGHT_UNITSIZE,
					Data[ 2 ],
					DX_PS_CONSTF_LIGHT_UNITSIZE,
					TRUE
				) ;
			}
			Graphics_D3D9_ShaderConstant_InfoSet_SetParam(
				&GD3D9.Device.Shader.ShaderConstantInfo,
				DX_SHADERCONSTANTTYPE_PS_FLOAT,
				DX_SHADERCONSTANTSET_LIB,
				DX_PS_CONSTF_LIGHT2_START + LightNumber * DX_PS_CONSTF_LIGHT2_UNITSIZE,
				Data[ 0 ],
				DX_PS_CONSTF_LIGHT2_UNITSIZE,
				TRUE
			) ;

			// アンビエントカラーとエミッシブカラーを掛け合わせた情報を更新する
			Graphics_D3D9_DeviceState_RefreshAmbientAndEmissiveParam() ;
		}

		if( LightNumber < DX_VS_CONSTB_LIGHT_NUM )
		{
			BOOL BData[ 2 ] ;
			BData[ 0 ] = Light->Type == D_D3DLIGHT_SPOT || Light->Type == D_D3DLIGHT_POINT ;
			BData[ 1 ] = Light->Type == D_D3DLIGHT_SPOT ;
			Graphics_D3D9_ShaderConstant_InfoSet_SetParam(
				&GD3D9.Device.Shader.ShaderConstantInfo,
				DX_SHADERCONSTANTTYPE_VS_BOOL,
				DX_SHADERCONSTANTSET_LIB,
				DX_VS_CONSTB_LIGHT_START + LightNumber * DX_VS_CONSTB_LIGHT_UNITSIZE + DX_VS_CONSTB_LGT_POINT_OR_SPOT,
				BData,
				2,
				TRUE
			) ;
		}

		// 使用するシェーダーのインデックスを更新する
		Graphics_D3D9_Shader_RefreshUseIndex() ;
	}

	return Direct3DDevice9_SetLight( ( DWORD )LightNumber, ( D_D3DLIGHT9 * )Light ) == D_D3D_OK ? 0 : -1 ;
}

// ライトの有効、無効を変更
extern int Graphics_D3D9_DeviceState_SetLightEnable( int LightNumber, int EnableState )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return 0 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	if( LightNumber < 256 )
	{
		GD3D9.Device.State.LightEnableFlag[ LightNumber ] = EnableState ;

		// 有効なライトの最大インデックスを更新
		if( EnableState == TRUE )
		{
			if( GD3D9.Device.State.LightEnableMaxIndex < LightNumber )
				GD3D9.Device.State.LightEnableMaxIndex = LightNumber ;
		}
		else
		{
			if( GD3D9.Device.State.LightEnableMaxIndex == LightNumber )
			{
				int i ;

				for( i = LightNumber ; i > 0 && GD3D9.Device.State.LightEnableFlag[ i ] == FALSE ; i -- ){}
				GD3D9.Device.State.LightEnableMaxIndex = i ;
			}
		}

		// アンビエントカラーとエミッシブカラーを掛け合わせた情報を更新する
		Graphics_D3D9_DeviceState_RefreshAmbientAndEmissiveParam() ;
	}

	if( GSYS.HardInfo.UseShader == TRUE && LightNumber < DX_VS_CONSTB_LIGHT_NUM )
	{
		BOOL BData ;
		BData = EnableState ;
		Graphics_D3D9_ShaderConstant_InfoSet_SetParam(
			&GD3D9.Device.Shader.ShaderConstantInfo,
			DX_SHADERCONSTANTTYPE_VS_BOOL,
			DX_SHADERCONSTANTSET_LIB,
			DX_VS_CONSTB_LIGHT_START + LightNumber * DX_VS_CONSTB_LIGHT_UNITSIZE + DX_VS_CONSTB_LGT_USE,
			&BData,
			1,
			TRUE
		) ;
	}

	// 使用するシェーダーのインデックスを更新する
	Graphics_D3D9_Shader_RefreshUseIndex() ;

	return Direct3DDevice9_LightEnable( ( DWORD )LightNumber, EnableState ) == D_D3D_OK ? 0 : -1 ;
}

// マテリアルのセット
extern int Graphics_D3D9_DeviceState_SetMaterial( D_D3DMATERIAL9 *Material )
{
	int i ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return 0 ;

	// 既存のパラメータと同じ場合は何もしない
	for( i = 0 ; i < MATERIAL_TYPEPARAM_MAX_NUM ; i ++ )
	{
		if( *( ( DWORD * )&GD3D9.Device.State.MaterialTypeParam[ i ] ) != *( ( DWORD * )&GSYS.Light.MaterialTypeParam[ i ] ) )
		{
			break ;
		}
	}
	if( i == MATERIAL_TYPEPARAM_MAX_NUM )
	{
		if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE &&
			*( ( DWORD * )&GD3D9.Device.State.Material.Ambient.a )  == *( ( DWORD * )&Material->Ambient.a  ) &&
			*( ( DWORD * )&GD3D9.Device.State.Material.Ambient.r )  == *( ( DWORD * )&Material->Ambient.r  ) &&
			*( ( DWORD * )&GD3D9.Device.State.Material.Ambient.g )  == *( ( DWORD * )&Material->Ambient.g  ) &&
			*( ( DWORD * )&GD3D9.Device.State.Material.Ambient.b )  == *( ( DWORD * )&Material->Ambient.b  ) &&
			*( ( DWORD * )&GD3D9.Device.State.Material.Diffuse.a )  == *( ( DWORD * )&Material->Diffuse.a  ) &&
			*( ( DWORD * )&GD3D9.Device.State.Material.Diffuse.r )  == *( ( DWORD * )&Material->Diffuse.r  ) &&
			*( ( DWORD * )&GD3D9.Device.State.Material.Diffuse.g )  == *( ( DWORD * )&Material->Diffuse.g  ) &&
			*( ( DWORD * )&GD3D9.Device.State.Material.Diffuse.b )  == *( ( DWORD * )&Material->Diffuse.b  ) &&
			*( ( DWORD * )&GD3D9.Device.State.Material.Emissive.a ) == *( ( DWORD * )&Material->Emissive.a ) &&
			*( ( DWORD * )&GD3D9.Device.State.Material.Emissive.r ) == *( ( DWORD * )&Material->Emissive.r ) &&
			*( ( DWORD * )&GD3D9.Device.State.Material.Emissive.g ) == *( ( DWORD * )&Material->Emissive.g ) &&
			*( ( DWORD * )&GD3D9.Device.State.Material.Emissive.b ) == *( ( DWORD * )&Material->Emissive.b ) &&
			*( ( DWORD * )&GD3D9.Device.State.Material.Specular.a ) == *( ( DWORD * )&Material->Specular.a ) &&
			*( ( DWORD * )&GD3D9.Device.State.Material.Specular.r ) == *( ( DWORD * )&Material->Specular.r ) &&
			*( ( DWORD * )&GD3D9.Device.State.Material.Specular.g ) == *( ( DWORD * )&Material->Specular.g ) &&
			*( ( DWORD * )&GD3D9.Device.State.Material.Specular.b ) == *( ( DWORD * )&Material->Specular.b ) &&
			*( ( DWORD * )&GD3D9.Device.State.Material.Power )      == *( ( DWORD * )&Material->Power      ) )
		{
			return 0 ;
		}
	}

	if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		// 描画待機している描画物を描画
		DRAWSTOCKINFO
	}

	// パラメータを保存
	GD3D9.Device.State.Material = *Material ;
	_MEMCPY( GD3D9.Device.State.MaterialTypeParam, GSYS.Light.MaterialTypeParam, sizeof( GSYS.Light.MaterialTypeParam ) ) ;

	// バーテックスシェーダーとピクセルシェーダーにも反映する
//	if( GSYS.HardInfo.UseShader )
	{
		float Data[ 3 ][ 4 ] ;

		Data[ 0 ][ 0 ] = Material->Diffuse.r ;
		Data[ 0 ][ 1 ] = Material->Diffuse.g ;
		Data[ 0 ][ 2 ] = Material->Diffuse.b ;
		Data[ 0 ][ 3 ] = Material->Diffuse.a ;
		Data[ 1 ][ 0 ] = Material->Specular.r ;
		Data[ 1 ][ 1 ] = Material->Specular.g ;
		Data[ 1 ][ 2 ] = Material->Specular.b ;
		Data[ 1 ][ 3 ] = Material->Specular.a ;
		Data[ 2 ][ 0 ] = Material->Power ;
		Data[ 2 ][ 1 ] = GD3D9.Device.State.MaterialTypeParam[ 0 ] ;
		Data[ 2 ][ 2 ] = GD3D9.Device.State.MaterialTypeParam[ 1 ] ;
		Data[ 2 ][ 3 ] = GD3D9.Device.State.MaterialTypeParam[ 2 ] ;
		Graphics_D3D9_ShaderConstant_InfoSet_SetParam(
			&GD3D9.Device.Shader.ShaderConstantInfo,
			DX_SHADERCONSTANTTYPE_VS_FLOAT,
			DX_SHADERCONSTANTSET_LIB,
			DX_VS_CONSTF_MATERIAL_START,
			Data[ 0 ],
			DX_VS_CONSTF_MATERIAL_SIZE,
			TRUE
		) ;
		Graphics_D3D9_ShaderConstant_InfoSet_SetParam(
			&GD3D9.Device.Shader.ShaderConstantInfo,
			DX_SHADERCONSTANTTYPE_PS_FLOAT,
			DX_SHADERCONSTANTSET_LIB,
			DX_PS_CONSTF_MATERIAL_START,
			Data[ 0 ],
			DX_PS_CONSTF_MATERIAL_SIZE,
			TRUE
		) ;

		// アンビエントカラーとエミッシブカラーを掛け合わせたものを更新
		Graphics_D3D9_DeviceState_RefreshAmbientAndEmissiveParam() ;
	}

	return Direct3DDevice9_SetMaterial( Material ) == D_D3D_OK ? 0 : -1 ;
}


// スペキュラを使用するかどうかをセット
extern int  Graphics_D3D9_DeviceState_SetSpecularEnable( int UseFlag )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return 0 ;

	// 既存のパラメータと同じ場合は何もしない
	if( GD3D9.Device.State.UseSpecular == UseFlag && GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE ) return 0 ;

	if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		// 描画待機している描画物を描画
		DRAWSTOCKINFO
	}

	// パラメータを保存
	GD3D9.Device.State.UseSpecular = UseFlag ;
	GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	return Direct3DDevice9_SetRenderState( D_D3DRS_SPECULARENABLE, ( DWORD )UseFlag ) == D_D3D_OK ? 0 : -1 ;
}

// 頂点のディフューズカラーをマテリアルのディフューズカラーとして使用するかどうかを設定する
extern int  Graphics_D3D9_DeviceState_SetDiffuseMaterialSource( int UseFlag )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return 0 ;

	// 既存のパラメータと同じ場合は何もしない
	if( GD3D9.Device.State.MaterialUseVertexDiffuseColor == UseFlag && GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE ) return 0 ;

	if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		// 描画待機している描画物を描画
		DRAWSTOCKINFO
	}

	// パラメータを保存
	GD3D9.Device.State.MaterialUseVertexDiffuseColor = UseFlag ;
	GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// バーテックスシェーダーに反映する
//	if( GSYS.HardInfo.UseShader )
	{
		float Data[ 4 ] ;

		Data[ 0 ] = GD3D9.Device.State.MaterialUseVertexDiffuseColor  ? 1.0f : 0.0f ;
		Data[ 1 ] = GD3D9.Device.State.MaterialUseVertexSpecularColor ? 1.0f : 0.0f ;
		Data[ 2 ] = 0.0f ;
		Data[ 3 ] = 0.0f ;
		Graphics_D3D9_ShaderConstant_InfoSet_SetParam(
			&GD3D9.Device.Shader.ShaderConstantInfo,
			DX_SHADERCONSTANTTYPE_VS_FLOAT,
			DX_SHADERCONSTANTSET_LIB,
			DX_VS_CONSTF_DIF_SPE_SOURCE,
			Data,
			1,
			TRUE
		) ;
	}

	return Direct3DDevice9_SetRenderState( D_D3DRS_DIFFUSEMATERIALSOURCE, UseFlag ? D_D3DMCS_COLOR1 : D_D3DMCS_MATERIAL ) == D_D3D_OK ? 0 : -1 ;
}

// 頂点のスペキュラカラーをマテリアルのスペキュラカラーとして使用するかどうかを設定する
extern int  Graphics_D3D9_DeviceState_SetSpecularMaterialSource( int UseFlag )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return 0 ;

	// 既存のパラメータと同じ場合は何もしない
	if( GD3D9.Device.State.MaterialUseVertexSpecularColor == UseFlag && GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE ) return 0 ;

	if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		// 描画待機している描画物を描画
		DRAWSTOCKINFO
	}

	// パラメータを保存
	GD3D9.Device.State.MaterialUseVertexSpecularColor = UseFlag ;
	GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// バーテックスシェーダーに反映する
//	if( GSYS.HardInfo.UseShader )
	{
		float Data[ 4 ] ;

		Data[ 0 ] = GD3D9.Device.State.MaterialUseVertexDiffuseColor  ? 1.0f : 0.0f ;
		Data[ 1 ] = GD3D9.Device.State.MaterialUseVertexSpecularColor ? 1.0f : 0.0f ;
		Data[ 2 ] = 0.0f ;
		Data[ 3 ] = 0.0f ;
		Graphics_D3D9_ShaderConstant_InfoSet_SetParam(
			&GD3D9.Device.Shader.ShaderConstantInfo,
			DX_SHADERCONSTANTTYPE_VS_FLOAT,
			DX_SHADERCONSTANTSET_LIB,
			DX_VS_CONSTF_DIF_SPE_SOURCE,
			Data,
			1,
			TRUE
		) ;
	}

	return Direct3DDevice9_SetRenderState( D_D3DRS_SPECULARMATERIALSOURCE, UseFlag ? D_D3DMCS_COLOR2 : D_D3DMCS_MATERIAL ) == D_D3D_OK ? 0 : -1 ;
}

// カリングの有無をセット 
extern int  Graphics_D3D9_DeviceState_SetCullMode( int State )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return 0 ;

	// モードが同じだったら何もせず終了
	if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE && GD3D9.Device.State.CullMode == State ) return 0 ;
	
	if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		// 描画待機している描画物を描画
		DRAWSTOCKINFO
	}

	// カリングの設定を変更
	Direct3DDevice9_SetRenderState( D_D3DRS_CULLMODE, State == 2 ? D_D3DCULL_CW : ( State ? D_D3DCULL_CCW : D_D3DCULL_NONE ) ) == D_D3D_OK ? 0 : -1 ;
	GD3D9.Device.State.CullMode = State;
	GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// 終了
	return 0;
}

// アンビエントカラーのセット
extern int  Graphics_D3D9_DeviceState_SetAmbient( unsigned int Color )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return 0 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// アンビエントカラーを保存する
	GD3D9.Device.State.GlobalAmbientLightColor.r = ( float )( ( Color >> 16 ) & 0xff ) / 255.0f ;
	GD3D9.Device.State.GlobalAmbientLightColor.g = ( float )( ( Color >>  8 ) & 0xff ) / 255.0f ;
	GD3D9.Device.State.GlobalAmbientLightColor.b = ( float )( ( Color >>  0 ) & 0xff ) / 255.0f ;
	GD3D9.Device.State.GlobalAmbientLightColor.a = ( float )( ( Color >> 24 ) & 0xff ) / 255.0f ;

	// アンビエントライトとエミッシブカラーを掛け合わせたパラメータを更新する
	Graphics_D3D9_DeviceState_RefreshAmbientAndEmissiveParam() ;

	return Direct3DDevice9_SetRenderState( D_D3DRS_AMBIENT, Color ) == D_D3D_OK ? 0 : -1 ;
}

// シェーディングモードのセット
extern int  Graphics_D3D9_DeviceState_SetShadeMode( int ShadeMode )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return 0 ;

	if( GD3D9.Device.State.ShadeMode == ShadeMode && GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE ) return 0 ;

	GD3D9.Device.State.ShadeMode = ShadeMode ;
	GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		// 描画待機している描画物を描画
		DRAWSTOCKINFO
	}

	return Direct3DDevice9_SetRenderState( D_D3DRS_SHADEMODE, ( DWORD )ShadeMode ) == D_D3D_OK ? 0 : -1 ;
}

// テクスチャーのアドレッシングモードをセットする
extern int  Graphics_D3D9_DeviceState_SetTextureAddress( int AddressMode, int Sampler )
{
	DWORD i ;
	HRESULT hr ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return 0 ;

	if( Sampler == -1 )
	{
		for( i = 0 ; i < USE_TEXTURESTAGE_NUM ; i ++ )
		{
			if( AddressMode != GD3D9.Device.State.TexAddressModeU[ i ] ||
				AddressMode != GD3D9.Device.State.TexAddressModeV[ i ] ||
				AddressMode != GD3D9.Device.State.TexAddressModeW[ i ] )
			{
				break ;
			}
		}

		if( i == USE_TEXTURESTAGE_NUM &&
			GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
		{
			return 0 ;
		}

		// 描画待機している描画物を描画
		DRAWSTOCKINFO

		for( i = 0 ; i < USE_TEXTURESTAGE_NUM ; i ++ )
		{
			GD3D9.Device.State.TexAddressModeU[ i ] = AddressMode ;
			GD3D9.Device.State.TexAddressModeV[ i ] = AddressMode ;
			GD3D9.Device.State.TexAddressModeW[ i ] = AddressMode ;
		}

		GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

		hr = Direct3DDevice9_SetSamplerState( 0, D_D3DSAMP_ADDRESSU, ( DWORD )AddressMode ) ;
		Direct3DDevice9_SetSamplerState( 0, D_D3DSAMP_ADDRESSV, ( DWORD )AddressMode ) ;
		Direct3DDevice9_SetSamplerState( 0, D_D3DSAMP_ADDRESSW, ( DWORD )AddressMode ) ;
		for( i = 1 ; i < USE_TEXTURESTAGE_NUM ; i ++ )
		{
			Direct3DDevice9_SetSamplerState( i, D_D3DSAMP_ADDRESSU, ( DWORD )AddressMode ) ;
			Direct3DDevice9_SetSamplerState( i, D_D3DSAMP_ADDRESSV, ( DWORD )AddressMode ) ;
			Direct3DDevice9_SetSamplerState( i, D_D3DSAMP_ADDRESSW, ( DWORD )AddressMode ) ;
		}
	}
	else
	{
		if( Sampler < 0 || Sampler >= USE_TEXTURESTAGE_NUM )
		{
			return -1 ;
		}

		if( AddressMode == GD3D9.Device.State.TexAddressModeU[ Sampler ] &&
			AddressMode == GD3D9.Device.State.TexAddressModeV[ Sampler ] &&
			AddressMode == GD3D9.Device.State.TexAddressModeW[ Sampler ] &&
			GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
		{
			return 0 ;
		}

		// 描画待機している描画物を描画
		DRAWSTOCKINFO

		GD3D9.Device.State.TexAddressModeU[ Sampler ] = AddressMode ;
		GD3D9.Device.State.TexAddressModeV[ Sampler ] = AddressMode ;
		GD3D9.Device.State.TexAddressModeW[ Sampler ] = AddressMode ;

		GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

		hr = Direct3DDevice9_SetSamplerState( ( DWORD )Sampler, D_D3DSAMP_ADDRESSU, ( DWORD )AddressMode ) ;
		Direct3DDevice9_SetSamplerState( ( DWORD )Sampler, D_D3DSAMP_ADDRESSV, ( DWORD )AddressMode ) ;
		Direct3DDevice9_SetSamplerState( ( DWORD )Sampler, D_D3DSAMP_ADDRESSW, ( DWORD )AddressMode ) ;
	}

	return hr == D_D3D_OK ? 0 : -1 ;
}

// テクスチャーのアドレッシングモードをセットする
extern int  Graphics_D3D9_DeviceState_SetTextureAddressU( int AddressMode, int Sampler )
{
	DWORD i ;
	HRESULT hr ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return 0 ;

	if( Sampler == -1 )
	{
		for( i = 0 ; i < USE_TEXTURESTAGE_NUM ; i ++ )
		{
			if( AddressMode != GD3D9.Device.State.TexAddressModeU[ i ] )
			{
				break ;
			}
		}

		if( i == USE_TEXTURESTAGE_NUM &&
			GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
		{
			return 0 ;
		}

		// 描画待機している描画物を描画
		DRAWSTOCKINFO

		for( i = 0 ; i < USE_TEXTURESTAGE_NUM ; i ++ )
		{
			GD3D9.Device.State.TexAddressModeU[ i ] = AddressMode ;
		}

		GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

		hr = Direct3DDevice9_SetSamplerState( 0, D_D3DSAMP_ADDRESSU, ( DWORD )AddressMode ) ;
		for( i = 1 ; i < USE_TEXTURESTAGE_NUM ; i ++ )
		{
			Direct3DDevice9_SetSamplerState( i, D_D3DSAMP_ADDRESSU, ( DWORD )AddressMode ) ;
		}
	}
	else
	{
		if( Sampler < 0 || Sampler >= USE_TEXTURESTAGE_NUM )
		{
			return -1 ;
		}

		if( AddressMode == GD3D9.Device.State.TexAddressModeU[ Sampler ] &&
			GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
		{
			return 0 ;
		}

		// 描画待機している描画物を描画
		DRAWSTOCKINFO

		GD3D9.Device.State.TexAddressModeU[ Sampler ] = AddressMode ;

		GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

		hr = Direct3DDevice9_SetSamplerState( ( DWORD )Sampler, D_D3DSAMP_ADDRESSU, ( DWORD )AddressMode ) ;
	}

	return hr == D_D3D_OK ? 0 : -1 ;
}

// テクスチャーのアドレッシングモードをセットする
extern int  Graphics_D3D9_DeviceState_SetTextureAddressV( int AddressMode, int Sampler )
{
	DWORD i ;
	HRESULT hr ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return 0 ;

	if( Sampler == -1 )
	{
		for( i = 0 ; i < USE_TEXTURESTAGE_NUM ; i ++ )
		{
			if( AddressMode != GD3D9.Device.State.TexAddressModeV[ i ] )
			{
				break ;
			}
		}

		if( i == USE_TEXTURESTAGE_NUM &&
			GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
		{
			return 0 ;
		}

		// 描画待機している描画物を描画
		DRAWSTOCKINFO

		for( i = 0 ; i < USE_TEXTURESTAGE_NUM ; i ++ )
		{
			GD3D9.Device.State.TexAddressModeV[ i ] = AddressMode ;
		}

		GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

		hr = Direct3DDevice9_SetSamplerState( 0, D_D3DSAMP_ADDRESSV, ( DWORD )AddressMode ) ;
		for( i = 1 ; i < USE_TEXTURESTAGE_NUM ; i ++ )
		{
			Direct3DDevice9_SetSamplerState( i, D_D3DSAMP_ADDRESSV, ( DWORD )AddressMode ) ;
		}
	}
	else
	{
		if( Sampler < 0 || Sampler >= USE_TEXTURESTAGE_NUM )
		{
			return -1 ;
		}

		if( AddressMode == GD3D9.Device.State.TexAddressModeV[ Sampler ] &&
			GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
		{
			return 0 ;
		}

		// 描画待機している描画物を描画
		DRAWSTOCKINFO

		GD3D9.Device.State.TexAddressModeV[ Sampler ] = AddressMode ;

		GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

		hr = Direct3DDevice9_SetSamplerState( ( DWORD )Sampler, D_D3DSAMP_ADDRESSV, ( DWORD )AddressMode ) ;
	}

	return hr == D_D3D_OK ? 0 : -1 ;
}

// テクスチャーのアドレッシングモードをセットする
extern int  Graphics_D3D9_DeviceState_SetTextureAddressW( int AddressMode, int Sampler )
{
	DWORD i ;
	HRESULT hr ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return 0 ;

	if( Sampler == -1 )
	{
		for( i = 0 ; i < USE_TEXTURESTAGE_NUM ; i ++ )
		{
			if( AddressMode != GD3D9.Device.State.TexAddressModeW[ i ] )
			{
				break ;
			}
		}

		if( i == USE_TEXTURESTAGE_NUM &&
			GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
		{
			return 0 ;
		}

		// 描画待機している描画物を描画
		DRAWSTOCKINFO

		for( i = 0 ; i < USE_TEXTURESTAGE_NUM ; i ++ )
		{
			GD3D9.Device.State.TexAddressModeW[ i ] = AddressMode ;
		}

		GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

		hr = Direct3DDevice9_SetSamplerState( 0, D_D3DSAMP_ADDRESSW, ( DWORD )AddressMode ) ;
		for( i = 1 ; i < USE_TEXTURESTAGE_NUM ; i ++ )
		{
			Direct3DDevice9_SetSamplerState( i, D_D3DSAMP_ADDRESSW, ( DWORD )AddressMode ) ;
		}
	}
	else
	{
		if( Sampler < 0 || Sampler >= USE_TEXTURESTAGE_NUM )
		{
			return -1 ;
		}

		if( AddressMode == GD3D9.Device.State.TexAddressModeW[ Sampler ] &&
			GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
		{
			return 0 ;
		}

		// 描画待機している描画物を描画
		DRAWSTOCKINFO

		GD3D9.Device.State.TexAddressModeW[ Sampler ] = AddressMode ;

		GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

		hr = Direct3DDevice9_SetSamplerState( ( DWORD )Sampler, D_D3DSAMP_ADDRESSW, ( DWORD )AddressMode ) ;
	}

	return hr == D_D3D_OK ? 0 : -1 ;
}

// テクスチャーのアドレッシングモードをセットする
extern int Graphics_D3D9_DeviceState_SetTextureAddressUVW( int AddressModeU, int AddressModeV, int AddressModeW, int Sampler )
{
	Graphics_D3D9_DeviceState_SetTextureAddressU( AddressModeU, Sampler ) ;
	Graphics_D3D9_DeviceState_SetTextureAddressV( AddressModeV, Sampler ) ;
	return Graphics_D3D9_DeviceState_SetTextureAddressW( AddressModeW, Sampler ) ;
}

// テクスチャ座標変換行列をセットする
extern int  Graphics_D3D9_DeviceState_SetTextureAddressTransformMatrix( int Use, MATRIX *Matrix, int Samplar )
{
	if( Samplar == -1 )
	{
		// 描画待機している描画物を描画
		DRAWSTOCKINFO

		if( Use == TRUE )
		{
			Direct3DDevice9_SetTextureStageState( 0, D_D3DTSS_TEXTURETRANSFORMFLAGS, D_D3DTTFF_COUNT3 ) ;
			Direct3DDevice9_SetTextureStageState( 1, D_D3DTSS_TEXTURETRANSFORMFLAGS, D_D3DTTFF_COUNT3 ) ;
			Direct3DDevice9_SetTextureStageState( 2, D_D3DTSS_TEXTURETRANSFORMFLAGS, D_D3DTTFF_COUNT3 ) ;
			Direct3DDevice9_SetTextureStageState( 3, D_D3DTSS_TEXTURETRANSFORMFLAGS, D_D3DTTFF_COUNT3 ) ;
			Direct3DDevice9_SetTransform( D_D3DTS_TEXTURE0, ( D_D3DMATRIX * )Matrix ) ;
			Direct3DDevice9_SetTransform( D_D3DTS_TEXTURE1, ( D_D3DMATRIX * )Matrix ) ;
			Direct3DDevice9_SetTransform( D_D3DTS_TEXTURE2, ( D_D3DMATRIX * )Matrix ) ;
			Direct3DDevice9_SetTransform( D_D3DTS_TEXTURE3, ( D_D3DMATRIX * )Matrix ) ;
		}
		else
		{
			Direct3DDevice9_SetTextureStageState( 0, D_D3DTSS_TEXTURETRANSFORMFLAGS, D_D3DTTFF_DISABLE ) ;
			Direct3DDevice9_SetTextureStageState( 1, D_D3DTSS_TEXTURETRANSFORMFLAGS, D_D3DTTFF_DISABLE ) ;
			Direct3DDevice9_SetTextureStageState( 2, D_D3DTSS_TEXTURETRANSFORMFLAGS, D_D3DTTFF_DISABLE ) ;
			Direct3DDevice9_SetTextureStageState( 3, D_D3DTSS_TEXTURETRANSFORMFLAGS, D_D3DTTFF_DISABLE ) ;
		}
		GD3D9.Device.State.TextureTransformUse[ 0 ] = Use ;
		GD3D9.Device.State.TextureTransformUse[ 1 ] = Use ;
		GD3D9.Device.State.TextureTransformUse[ 2 ] = Use ;
		GD3D9.Device.State.TextureTransformUse[ 3 ] = Use ;
		GD3D9.Device.State.TextureTransformMatrix[ 0 ] = *Matrix ;
		GD3D9.Device.State.TextureTransformMatrix[ 1 ] = *Matrix ;
		GD3D9.Device.State.TextureTransformMatrix[ 2 ] = *Matrix ;
		GD3D9.Device.State.TextureTransformMatrix[ 3 ] = *Matrix ;
	}
	else
	{
		if( Samplar >= GD3D9.Device.Caps.MaxTextureBlendStages || GD3D9.Device.State.TextureTransformUse[ Samplar ] == FALSE && Use == FALSE )
			return 0 ;

		// 描画待機している描画物を描画
		DRAWSTOCKINFO

		if( Use == TRUE )
		{
			if( GD3D9.Device.State.TextureTransformUse[ Samplar ] == FALSE )
			{
				Direct3DDevice9_SetTextureStageState( ( DWORD )Samplar, D_D3DTSS_TEXTURETRANSFORMFLAGS, D_D3DTTFF_COUNT3 ) ;
			}
			Direct3DDevice9_SetTransform( ( D_D3DTRANSFORMSTATETYPE )( D_D3DTS_TEXTURE0 + Samplar ), ( D_D3DMATRIX * )Matrix ) ;
		}
		else
		{
			Direct3DDevice9_SetTextureStageState( ( DWORD )Samplar, D_D3DTSS_TEXTURETRANSFORMFLAGS, D_D3DTTFF_DISABLE ) ;
		}
		GD3D9.Device.State.TextureTransformUse[ Samplar ] = Use ;
		GD3D9.Device.State.TextureTransformMatrix[ Samplar ] = *Matrix ;
	}
	return 0 ;
}

// フォグを有効にするかどうかを設定する( TRUE:有効  FALSE:無効 )
extern int  Graphics_D3D9_DeviceState_SetFogEnable( int Flag )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return 0 ;

	if( Flag == GD3D9.Device.State.FogEnable && GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE ) return 0 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	GD3D9.Device.State.FogEnable = Flag ;
	GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// 使用するシェーダーのインデックスを更新する
	Graphics_D3D9_Shader_RefreshUseIndex() ;

	{
		BOOL BData ;
		BData = Flag == 0 ? 0 : 1 ;
		Graphics_D3D9_ShaderConstant_InfoSet_SetParam(
			&GD3D9.Device.Shader.ShaderConstantInfo,
			DX_SHADERCONSTANTTYPE_VS_BOOL,
			DX_SHADERCONSTANTSET_LIB,
			DX_VS_CONSTB_FOG_USE,
			&BData,
			1,
			TRUE
		) ;
	}

	return Direct3DDevice9_SetRenderState( D_D3DRS_FOGENABLE, ( DWORD )Flag ) == D_D3D_OK ? 0 : -1 ;
}

// フォグモードを設定する
extern int  Graphics_D3D9_DeviceState_SetFogVertexMode( int Mode /* DX_FOGMODE_NONE 等 */ )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return 0 ;

	if( Mode == GD3D9.Device.State.FogMode && GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE ) return 0 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// バーテックスシェーダー用のパラメータセット
	if( GSYS.HardInfo.UseShader )
	{
		BOOL BData[ 4 ] ;

		BData[ 0 ] = Mode == DX_FOGMODE_LINEAR ;
		BData[ 1 ] = Mode == DX_FOGMODE_EXP ;
		BData[ 2 ] = Mode == DX_FOGMODE_EXP2 ;
		BData[ 3 ] = Mode != DX_FOGMODE_NONE ;
		Graphics_D3D9_ShaderConstant_InfoSet_SetParam(
			&GD3D9.Device.Shader.ShaderConstantInfo,
			DX_SHADERCONSTANTTYPE_VS_BOOL,
			DX_SHADERCONSTANTSET_LIB,
			DX_VS_CONSTB_FOG_LINEAR,
			BData,
			4,
			TRUE
		) ;
	}

	GD3D9.Device.State.FogMode = Mode ;

	// 使用するシェーダーのインデックスを更新する
	Graphics_D3D9_Shader_RefreshUseIndex() ;

	return Direct3DDevice9_SetRenderState( D_D3DRS_FOGVERTEXMODE, ( DWORD )Mode ) == D_D3D_OK ? 0 : -1 ;
}

// フォグカラーを変更する
extern int  Graphics_D3D9_DeviceState_SetFogColor( unsigned int Color )
{
	float Data[ 4 ] ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return 0 ;

	if( Color == GD3D9.Device.State.FogColor && GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE ) return 0 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

//	if( GSYS.HardInfo.UseShader )
	{
		Data[ 0 ] = ( ( Color >> 16 ) & 0xff ) / 255.0f ;
		Data[ 1 ] = ( ( Color >>  8 ) & 0xff ) / 255.0f ;
		Data[ 2 ] = ( ( Color >>  0 ) & 0xff ) / 255.0f ;
		Data[ 3 ] = 1.0f ;
		Graphics_D3D9_ShaderConstant_InfoSet_SetParam(
			&GD3D9.Device.Shader.ShaderConstantInfo,
			DX_SHADERCONSTANTTYPE_PS_FLOAT,
			DX_SHADERCONSTANTSET_LIB,
			DX_PS_CONSTF_FOG_COLOR,
			Data,
			1,
			TRUE
		) ;
	}

	GD3D9.Device.State.FogColor = Color ;
	return Direct3DDevice9_SetRenderState( D_D3DRS_FOGCOLOR, Color ) == D_D3D_OK ? 0 : -1 ;
}

// フォグが始まる距離と終了する距離を設定する( 0.0f 〜 1.0f )
extern int  Graphics_D3D9_DeviceState_SetFogStartEnd( float Start, float End )
{
	float Data[ 4 ] ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return 0 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	if( Start != GD3D9.Device.State.FogStart || GD3D9.Device.DrawSetting.CancelSettingEqualCheck == TRUE )
	{
		GD3D9.Device.State.FogStart = Start ;
		Direct3DDevice9_SetRenderState( D_D3DRS_FOGSTART, *(( DWORD *)&Start) ) ;

		// バーテックスシェーダー用のパラメータセット
//		if( GSYS.HardInfo.UseShader )
		{
			Data[ 0 ] = GD3D9.Device.State.FogEnd / ( GD3D9.Device.State.FogEnd - GD3D9.Device.State.FogStart ) ;
			Data[ 1 ] = -1.0f / ( GD3D9.Device.State.FogEnd - GD3D9.Device.State.FogStart ) ;
			Data[ 2 ] = GD3D9.Device.State.FogDensity ;
			Data[ 3 ] = 2.71828183f ;
			Graphics_D3D9_ShaderConstant_InfoSet_SetParam(
				&GD3D9.Device.Shader.ShaderConstantInfo,
				DX_SHADERCONSTANTTYPE_VS_FLOAT,
				DX_SHADERCONSTANTSET_LIB,
				DX_VS_CONSTF_FOG,
				Data,
				1,
				TRUE
			) ;
		}
	}

	if( End != GD3D9.Device.State.FogEnd || GD3D9.Device.DrawSetting.CancelSettingEqualCheck == TRUE )
	{
		GD3D9.Device.State.FogEnd = End ;
		Direct3DDevice9_SetRenderState( D_D3DRS_FOGEND, *(( DWORD *)&End) ) ;

		// バーテックスシェーダー用のパラメータセット
//		if( GSYS.HardInfo.UseShader )
		{
			Data[ 0 ] = GD3D9.Device.State.FogEnd / ( GD3D9.Device.State.FogEnd - GD3D9.Device.State.FogStart ) ;
			Data[ 1 ] = -1.0f / ( GD3D9.Device.State.FogEnd - GD3D9.Device.State.FogStart ) ;
			Data[ 2 ] = GD3D9.Device.State.FogDensity ;
			Data[ 3 ] = 2.71828183f ;
			Graphics_D3D9_ShaderConstant_InfoSet_SetParam(
				&GD3D9.Device.Shader.ShaderConstantInfo,
				DX_SHADERCONSTANTTYPE_VS_FLOAT,
				DX_SHADERCONSTANTSET_LIB,
				DX_VS_CONSTF_FOG,
				Data,
				1,
				TRUE
			) ;
		}
	}

	// 終了
	return 0 ;
}

// フォグの密度を設定する( 0.0f 〜 1.0f )
extern int  Graphics_D3D9_DeviceState_SetFogDensity( float Density )
{
	float Data[ 4 ] ;
	if( GAPIWin.Direct3DDevice9Object == NULL ) return 0 ;

	if( Density == GD3D9.Device.State.FogDensity && GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE ) return 0 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	GD3D9.Device.State.FogDensity = Density ;

	// バーテックスシェーダー用のパラメータセット
//	if( GSYS.HardInfo.UseShader )
	{
		Data[ 0 ] = GD3D9.Device.State.FogEnd / ( GD3D9.Device.State.FogEnd - GD3D9.Device.State.FogStart ) ;
		Data[ 1 ] = -1.0f / ( GD3D9.Device.State.FogEnd - GD3D9.Device.State.FogStart ) ;
		Data[ 2 ] = GD3D9.Device.State.FogDensity ;
		Data[ 3 ] = 2.71828183f ;
		Graphics_D3D9_ShaderConstant_InfoSet_SetParam(
			&GD3D9.Device.Shader.ShaderConstantInfo,
			DX_SHADERCONSTANTTYPE_VS_FLOAT,
			DX_SHADERCONSTANTSET_LIB,
			DX_VS_CONSTF_FOG,
			Data,
			1,
			TRUE
		) ;
	}

	return Direct3DDevice9_SetRenderState( D_D3DRS_FOGDENSITY, *(( DWORD *)&Density) ) == D_D3D_OK ? 0 : -1 ;
}

// ライトの有無フラグをセットする
extern int  Graphics_D3D9_DeviceState_SetLighting( int Flag )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return 0 ;

	if( Flag == GD3D9.Device.State.Lighting && GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE ) return 0 ;

	GD3D9.Device.State.Lighting = Flag ;
	GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	Graphics_D3D9_Shader_RefreshUseIndex() ;

	return Direct3DDevice9_SetRenderState( D_D3DRS_LIGHTING, ( DWORD )Flag ) == D_D3D_OK ? 0 : -1 ;
}

// 最大異方性をセットする
extern	int		Graphics_D3D9_DeviceState_SetMaxAnisotropy( int MaxAnisotropy, int Sampler )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return 0 ;

	if( MaxAnisotropy < 1 )
	{
		MaxAnisotropy = 1 ;
	}
	else
	if( MaxAnisotropy > ( int )GD3D9.Device.Caps.DeviceCaps.MaxAnisotropy )
	{
		MaxAnisotropy = ( int )GD3D9.Device.Caps.DeviceCaps.MaxAnisotropy ;
	}

	if( Sampler >= 0 )
	{
		if( Sampler < 0 || Sampler >= USE_TEXTURESTAGE_NUM ) return 0 ;

		if( GD3D9.Device.State.MaxAnisotropyDim[ Sampler ] == MaxAnisotropy && GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
		{
			return 0 ;
		}

		Direct3DDevice9_SetSamplerState( ( DWORD )Sampler, D_D3DSAMP_MAXANISOTROPY, ( DWORD )MaxAnisotropy ) ;
		GD3D9.Device.State.MaxAnisotropyDim[ Sampler ] = MaxAnisotropy ;

		GD3D9.Device.State.MaxAnisotropy = -1 ;
	}
	else
	{
		int i, num ;

		if( GD3D9.Device.State.MaxAnisotropy == MaxAnisotropy && GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
		{
			return 0 ;
		}

		// 値の補正
		if( MaxAnisotropy < 1 )
		{
			MaxAnisotropy = 1 ;
		}

		// 描画待機している描画物を描画
		DRAWSTOCKINFO

		// レンダリング設定を変更する
		num = GD3D9.Device.State.BlendInfo.UseTextureStageNum ;
		if( num < 4 ) num = 4 ;
		for( i = 0 ; i < num ; i ++ )
		{
			if( GD3D9.Device.State.MaxAnisotropyDim[ i ] != MaxAnisotropy )
			{
				Direct3DDevice9_SetSamplerState( ( DWORD )i, D_D3DSAMP_MAXANISOTROPY, ( DWORD )MaxAnisotropy ) ;
				GD3D9.Device.State.MaxAnisotropyDim[ i ] = MaxAnisotropy ;
			}
		}

		GD3D9.Device.State.MaxAnisotropy = MaxAnisotropy ;
	}

	GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// 終了
	return 0 ;

}

// ビューポートをセットする
extern int Graphics_D3D9_DeviceState_SetViewport( D_D3DVIEWPORT9 *Viewport )
{
	HRESULT hr ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;
	if( Viewport->X      == GD3D9.Device.State.Viewport.X      &&
		Viewport->Y      == GD3D9.Device.State.Viewport.Y      &&
		Viewport->Width  == GD3D9.Device.State.Viewport.Width  &&
		Viewport->Height == GD3D9.Device.State.Viewport.Height &&
		GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE ) return 0 ;

	// 書き出す
	if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		// 描画待機している描画物を描画
		DRAWSTOCKINFO
	}

	// 描画を終了させておく
	Graphics_D3D9_EndScene() ;

	// ビューポートのセッティング
	hr = Direct3DDevice9_SetViewport( Viewport ) ;
	if( hr != D_D3D_OK )
		return DXST_LOGFILEFMT_ADDUTF16LE(( "\xd3\x30\xe5\x30\xfc\x30\xdd\x30\xfc\x30\xc8\x30\x6e\x30\xbb\x30\xc3\x30\xc8\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x43\x00\x6f\x00\x64\x00\x65\x00\x20\x00\x25\x00\x78\x00\x0a\x00\x00"/*@ L"ビューポートのセットに失敗しました ErrorCode %x\n" @*/, hr ));

	// ビューポートの情報の保存
	GD3D9.Device.State.Viewport = *Viewport ;

	// 終了
	return 0 ;
}

// ビューポートをセットする( 簡易版 )
extern int Graphics_D3D9_DeviceState_SetViewportEasy( int x1, int y1, int x2, int y2 )
{
	D_D3DVIEWPORT9 Viewport ;

	Viewport.X      = ( DWORD )x1 ;
	Viewport.Y      = ( DWORD )y1 ;
	Viewport.Width  = ( DWORD )( x2 - x1 ) ;
	Viewport.Height = ( DWORD )( y2 - y1 ) ;
	Viewport.MinZ   = 0.0f ;
	Viewport.MaxZ   = 1.0f ;

	return Graphics_D3D9_DeviceState_SetViewport( &Viewport ) ;
}

// シザー矩形を設定する
extern int Graphics_D3D9_DeviceState_SetScissorRect( RECT *Rect )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;
	if( Rect->left   == GD3D9.Device.State.ScissorRect.left   &&
		Rect->right  == GD3D9.Device.State.ScissorRect.right  &&
		Rect->top    == GD3D9.Device.State.ScissorRect.top    &&
		Rect->bottom == GD3D9.Device.State.ScissorRect.bottom &&
		GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE ) return 0 ;

	if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		// 描画待機している描画物を描画
		DRAWSTOCKINFO
	}

	// 描画を終了させておく
	Graphics_D3D9_EndScene() ;

	// シザー矩形のセッティング
	Direct3DDevice9_SetScissorRect( Rect ) ;

	// シザー矩形の保存
	GD3D9.Device.State.ScissorRect = *Rect ;

	// 終了
	return 0 ;
}

// 描画モードのセット
extern int Graphics_D3D9_DeviceState_SetDrawMode( int DrawMode )
{
	DWORD i, num ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;
	if( GD3D9.Device.State.DrawMode == DrawMode && GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE ) return 0 ;

	if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		// 描画待機している描画物を描画
		DRAWSTOCKINFO
	}

	// レンダリング設定を変更する
	num = ( DWORD )GD3D9.Device.State.BlendInfo.UseTextureStageNum ;
	if( num < USE_TEXTURESTAGE_NUM ) num = USE_TEXTURESTAGE_NUM ;
	switch( DrawMode )
	{
	case DX_DRAWMODE_BILINEAR :
		// バイリニア描画
		for( i = 0 ; i < num ; i ++ )
		{
			if( GD3D9.Device.State.MagFilter[ i ] != D_D3DTEXF_LINEAR || GD3D9.Device.DrawSetting.CancelSettingEqualCheck )
			{
				Direct3DDevice9_SetSamplerState( i, D_D3DSAMP_MAGFILTER, D_D3DTEXF_LINEAR ) ;
				GD3D9.Device.State.MagFilter[ i ] = D_D3DTEXF_LINEAR ;
			}
			if( GD3D9.Device.State.MinFilter[ i ] != D_D3DTEXF_LINEAR || GD3D9.Device.DrawSetting.CancelSettingEqualCheck )
			{
				Direct3DDevice9_SetSamplerState( i, D_D3DSAMP_MINFILTER, D_D3DTEXF_LINEAR ) ;
				GD3D9.Device.State.MinFilter[ i ] = D_D3DTEXF_LINEAR ;
			}
			if( GD3D9.Device.State.MipFilter[ i ] != D_D3DTEXF_LINEAR || GD3D9.Device.DrawSetting.CancelSettingEqualCheck )
			{
				Direct3DDevice9_SetSamplerState( i, D_D3DSAMP_MIPFILTER, D_D3DTEXF_NONE ) ;
				GD3D9.Device.State.MipFilter[ i ] = D_D3DTEXF_NONE ;
			}
		}
		break ;

	case DX_DRAWMODE_NEAREST :
		// 二アレストネイバー
		for( i = 0 ; i < num ; i ++ )
		{
			if( GD3D9.Device.State.MagFilter[ i ] != D_D3DTEXF_POINT || GD3D9.Device.DrawSetting.CancelSettingEqualCheck )
			{
				Direct3DDevice9_SetSamplerState( i, D_D3DSAMP_MAGFILTER, D_D3DTEXF_POINT ) ;
				GD3D9.Device.State.MagFilter[ i ] = D_D3DTEXF_POINT ;
			}
			if( GD3D9.Device.State.MinFilter[ i ] != D_D3DTEXF_POINT || GD3D9.Device.DrawSetting.CancelSettingEqualCheck )
			{
				Direct3DDevice9_SetSamplerState( i, D_D3DSAMP_MINFILTER, D_D3DTEXF_POINT ) ;
				GD3D9.Device.State.MinFilter[ i ] = D_D3DTEXF_POINT ;
			}
			if( GD3D9.Device.State.MipFilter[ i ] != D_D3DTEXF_POINT || GD3D9.Device.DrawSetting.CancelSettingEqualCheck )
			{
				Direct3DDevice9_SetSamplerState( i, D_D3DSAMP_MIPFILTER, D_D3DTEXF_NONE ) ;
				GD3D9.Device.State.MipFilter[ i ] = D_D3DTEXF_NONE ;
			}
		}
		break ;

	case DX_DRAWMODE_ANISOTROPIC :
		// 異方性フィルタリング
		for( i = 0 ; i < num ; i ++ )
		{
			if( GD3D9.Device.State.MagFilter[ i ] != D_D3DTEXF_LINEAR || GD3D9.Device.DrawSetting.CancelSettingEqualCheck )
			{
				Direct3DDevice9_SetSamplerState( i, D_D3DSAMP_MAGFILTER, D_D3DTEXF_LINEAR ) ;
				GD3D9.Device.State.MagFilter[ i ] = D_D3DTEXF_LINEAR ;
			}
			if( GD3D9.Device.State.MinFilter[ i ] != D_D3DTEXF_ANISOTROPIC || GD3D9.Device.DrawSetting.CancelSettingEqualCheck )
			{
				Direct3DDevice9_SetSamplerState( i, D_D3DSAMP_MINFILTER, D_D3DTEXF_ANISOTROPIC ) ;
				GD3D9.Device.State.MinFilter[ i ] = D_D3DTEXF_ANISOTROPIC ;
			}
			if( GD3D9.Device.State.MipFilter[ i ] != D_D3DTEXF_LINEAR || GD3D9.Device.DrawSetting.CancelSettingEqualCheck )
			{
				Direct3DDevice9_SetSamplerState( i, D_D3DSAMP_MIPFILTER, D_D3DTEXF_LINEAR ) ;
				GD3D9.Device.State.MipFilter[ i ] = D_D3DTEXF_LINEAR ;
			}
		}
		break ;

	default : return -1 ;
	}
	GD3D9.Device.State.DrawMode                 = DrawMode ;
	GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// 終了
	return 0 ;
}

// 描画対象の変更
extern	int		Graphics_D3D9_DeviceState_SetRenderTarget( D_IDirect3DSurface9 *TargetSurface, int TargetIndex )
{
	D_D3DSURFACE_DESC SurfaceDesc ;
	HRESULT hr ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	if( TargetIndex < 0 || TargetIndex >= DX_RENDERTARGET_COUNT ) return -1 ;

	if( GD3D9.Device.State.TargetSurface[ TargetIndex ] == TargetSurface && GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE ) return 0 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// レンダリングターゲットの変更
	hr = Direct3DDevice9_SetRenderTarget( ( DWORD )TargetIndex, TargetSurface ) ;

	// レンダリングターゲットの保存
	GD3D9.Device.State.TargetSurface[ TargetIndex ] = TargetSurface ;

	// ビューポートの設定値を更新
	if( TargetIndex == 0 )
	{
		Direct3DSurface9_GetDesc( TargetSurface, &SurfaceDesc ) ; 
		Graphics_D3D9_DeviceState_SetViewportEasy( 0, 0, ( int )SurfaceDesc.Width, ( int )SurfaceDesc.Height ) ;
	}

	// 終了
	return  0 ;
}

// 固定機能パイプライン互換処理用の頂点宣言と頂点シェーダーの設定を行う
static void Graphics_Hardware_D3D9_SetFVF_UseTable( int FVFDecl /* VERTEXFVF_DECL_2D_USER等 */ )
{
#ifndef DX_NON_NORMAL_DRAW_SHADER
//	if( GSYS.HardInfo.UseShader == FALSE || GD3D9.NormalDraw_UseVertexShader == FALSE )
//	{
//		Graphics_D3D9_DeviceState_SetFVF( ( int )D3DDev_VertexFVF[ FVFDecl ] ) ;
//	}
//	else
//	{
//		if( D3DDev_VertexDeclaration_Is3D[ FVFDecl ] )
//		{
//			Graphics_D3D9_DeviceState_SetVertexShader( GD3D9.Device.Shader.Base.Base3DVertexShader, TRUE ) ;
//		}
//		else
//		{
//			Graphics_D3D9_DeviceState_SetVertexShader( GD3D9.Device.Shader.Base.Base2DVertexShader, TRUE ) ;
//		}
//
//		Graphics_D3D9_DeviceState_SetVertexDeclaration( GD3D9.Device.VertexDeclaration.Base2DDeclaration[ FVFDecl ] ) ;
//	}

	// 2D行列を使用する場合のみシェーダーを使用する
	if( GSYS.HardInfo.UseShader == FALSE ||
		GSYS.DrawSetting.Valid2DMatrix == FALSE ||
		GSYS.DrawSetting.MatchHardware_2DMatrix == FALSE ||
		D3DDev_VertexDeclaration_Is3D[ FVFDecl ] )
	{
		Graphics_D3D9_DeviceState_SetFVF( ( int )D3DDev_VertexFVF[ FVFDecl ] ) ;
	}
	else
	{
		Graphics_D3D9_DeviceState_SetVertexShader( GD3D9.Device.Shader.Base.Base2DVertexShader, TRUE ) ;
		Graphics_D3D9_DeviceState_SetVertexDeclaration( GD3D9.Device.VertexDeclaration.Base2DDeclaration[ FVFDecl ] ) ;
	}
#else  // DX_NON_NORMAL_DRAW_SHADER

	Graphics_D3D9_DeviceState_SetFVF( D3DDev_VertexFVF[ FVFDecl ] ) ;

#endif // DX_NON_NORMAL_DRAW_SHADER
}

// 使用するFVFを変更する
extern	int		Graphics_D3D9_DeviceState_SetFVF( int FVF )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	Graphics_D3D9_DeviceState_SetVertexDeclaration( NULL ) ;

	if( GD3D9.Device.State.SetFVF != ( DWORD )FVF || GD3D9.Device.DrawSetting.CancelSettingEqualCheck )
	{
		Direct3DDevice9_SetFVF( ( DWORD )FVF ) ;
		GD3D9.Device.State.SetFVF = ( DWORD )FVF ;
	}

	// 終了
	return 0 ;
}

// 使用する頂点シェーダ宣言を変更する
extern	int		Graphics_D3D9_DeviceState_SetVertexDeclaration( D_IDirect3DVertexDeclaration9 *VertexDeclaration )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	if( GD3D9.Device.State.SetVertexDeclaration != VertexDeclaration || GD3D9.Device.DrawSetting.CancelSettingEqualCheck )
	{
		Direct3DDevice9_SetVertexDeclaration( VertexDeclaration ) ;
		GD3D9.Device.State.SetVertexDeclaration = VertexDeclaration ;
	}

	GD3D9.Device.State.SetFVF = 0 ;
	GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag    = TRUE ;

	// 終了
	return 0 ;
}

#ifndef DX_NON_MODEL

// 指定のモデル用の頂点シェーダ宣言をセットする
extern	int		Graphics_D3D9_DeviceState_SetMV1VertexDeclaration( int BumpMap, int SkinMesh, int UVNum )
{
	return Graphics_D3D9_DeviceState_SetVertexDeclaration( GD3D9.Device.VertexDeclaration.MV1_VertexDeclaration[ BumpMap ][ SkinMesh ][ UVNum ] ) ;
}

#endif // DX_NON_MODEL

// 指定の頂点シェーダーをデバイスにセットする
extern int Graphics_D3D9_DeviceState_SetVertexShaderToHandle( int ShaderHandle )
{
	SHADERHANDLEDATA *VertexShader ;

	// ハンドルチェック
	if( SHADERCHK( ShaderHandle, VertexShader ) )
		return -1 ;
	if( VertexShader->IsVertexShader == FALSE )
		return -1 ;

	// シェーダーのセット
	Graphics_D3D9_DeviceState_SetVertexShader( VertexShader->PF->D3D9.VertexShader ) ;

	// 正常終了
	return 0 ;
}

// 使用する頂点シェーダーを変更する
extern	int		Graphics_D3D9_DeviceState_SetVertexShader( D_IDirect3DVertexShader9 *VertexShader, int NormalVertexShader )
{
	if( GD3D9.Device.State.SetVertexShader == VertexShader && GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	Direct3DDevice9_SetVertexShader( VertexShader ) ;
	GD3D9.Device.State.SetVertexShader          = VertexShader ;
	GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;
	GD3D9.Device.State.SetNormalVertexShader    = NormalVertexShader ;
	if( NormalVertexShader == FALSE )
	{
		GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;
	}

	// 終了
	return 0 ;
}


// 頂点シェーダーの使用を止める( 固定機能パイプラインを使用する状態にする )
extern	int		Graphics_D3D9_DeviceState_ResetVertexShader( int SetNormalVertexShaderCancel )
{
	if( GD3D9.Device.State.SetNormalVertexShader &&
		SetNormalVertexShaderCancel &&
		GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE &&
		GD3D9.Device.State.SetVertexShader == NULL &&
		GD3D9.Device.State.SetVertexDeclaration == NULL )
	{
		return 0 ;
	}

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck || GD3D9.Device.State.SetVertexShader != NULL )
	{
		Direct3DDevice9_SetVertexShader( NULL ) ;
		GD3D9.Device.State.SetVertexShader = NULL ;
	}

	if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck || GD3D9.Device.State.SetVertexDeclaration != NULL )
	{
		Direct3DDevice9_SetVertexDeclaration( NULL ) ;
		GD3D9.Device.State.SetVertexDeclaration = NULL ;
	}

	GD3D9.Device.State.SetNormalVertexShader    = FALSE ;
	GD3D9.Device.State.SetFVF                   = 0 ;
	GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// 終了
	return 0 ;
}

// 指定のピクセルシェーダーをデバイスにセットする
extern int Graphics_D3D9_DeviceState_SetPixelShaderToHandle( int ShaderHandle )
{
	SHADERHANDLEDATA *PixelShader ;

	// ハンドルチェック
	if( SHADERCHK( ShaderHandle, PixelShader ) )
		return -1 ;
	if( PixelShader->IsVertexShader != FALSE )
		return -1 ;

	// シェーダーのセット
	Graphics_D3D9_DeviceState_SetPixelShader( PixelShader->PF->D3D9.PixelShader ) ;

	// 正常終了
	return 0 ;
}

// 使用するピクセルシェーダーを変更する
extern	int		Graphics_D3D9_DeviceState_SetPixelShader( D_IDirect3DPixelShader9 *PixelShader, int NormalPixelShader )
{
	if( GD3D9.Device.State.SetPixelShader == PixelShader && GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	Direct3DDevice9_SetPixelShader( PixelShader ) ;
	GD3D9.Device.State.SetPixelShader       = PixelShader ;
	GD3D9.Device.State.SetNormalPixelShader = NormalPixelShader ;
	if( NormalPixelShader == FALSE )
	{
		GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;
	}

	// 終了
	return 0 ;
}

// ピクセルシェーダーの使用を止める( 固定機能パイプラインを使用する状態にする )
extern	int		Graphics_D3D9_DeviceState_ResetPixelShader( int SetNormalPixelShaderCancel )
{
	if( GD3D9.Device.State.SetNormalPixelShader &&
		SetNormalPixelShaderCancel &&
		GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE &&
		GD3D9.Device.State.SetPixelShader == NULL )
	{
		return 0 ;
	}

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck || GD3D9.Device.State.SetPixelShader != NULL )
	{
		Direct3DDevice9_SetPixelShader( NULL ) ;
		GD3D9.Device.State.SetPixelShader = NULL ;
	}

	GD3D9.Device.State.SetNormalPixelShader     = FALSE ;
	GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// 終了
	return 0 ;
}

// 使用する頂点バッファを変更する
extern	int		Graphics_D3D9_DeviceState_SetVertexBuffer( D_IDirect3DVertexBuffer9 *VertexBuffer, int VertexStride )
{
	if( GD3D9.Device.State.SetVertexBuffer       == VertexBuffer &&
		GD3D9.Device.State.SetVertexBufferStride == VertexStride &&
		GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	if( VertexBuffer != NULL )
	{
		Direct3DDevice9_SetStreamSource( 0, VertexBuffer, 0, ( UINT )VertexStride ) ;
	}
	GD3D9.Device.State.SetVertexBuffer       = VertexBuffer ;
	GD3D9.Device.State.SetVertexBufferStride = VertexStride ;

	// 終了
	return 0 ;
}

// 使用するインデックスバッファを変更する
extern	int		Graphics_D3D9_DeviceState_SetIndexBuffer( D_IDirect3DIndexBuffer9 *IndexBuffer )
{
	if( GD3D9.Device.State.SetIndexBuffer == IndexBuffer && GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	if( IndexBuffer != NULL )
	{
		Direct3DDevice9_SetIndices( IndexBuffer ) ;
	}
	GD3D9.Device.State.SetIndexBuffer = IndexBuffer ;

	// 終了
	return 0 ;
}

// 各ステージが使用するテクスチャアドレスをステージ番号と同じにする
extern	int		Graphics_D3D9_DeviceState_ResetTextureCoord( void )
{
	DWORD i ;
	GRAPHICS_HARDDATA_DIRECT3D9_TEXTURESTAGEINFO *Stage ;

	Stage = GD3D9.Device.State.BlendInfo.TextureStageInfo ;
	for( i = 0 ; i < ( DWORD )GD3D9.Device.Caps.MaxTextureBlendStages ; i ++, Stage ++ )
	{
		if( ( DWORD )Stage->TextureCoordIndex != i || GD3D9.Device.DrawSetting.CancelSettingEqualCheck )
		{
			Direct3DDevice9_SetTextureStageState( i, D_D3DTSS_TEXCOORDINDEX, i ) ;
			Stage->TextureCoordIndex = ( int )i ;
		}
	}

	// 終了
	return 0 ;
}










// ユーザーのブレンドインフォを適応する
extern	int		Graphics_D3D9_DeviceState_SetUserBlendInfo( GRAPHICS_HARDDATA_DIRECT3D9_BLENDINFO *BlendInfo, int TextureStageIsTextureAndTextureCoordOnlyFlag, int TextureIsGraphHandleFlag, int UseShader )
{
	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	Graphics_D3D9_DeviceState_RefreshBlendState( BlendInfo, TextureStageIsTextureAndTextureCoordOnlyFlag, TextureIsGraphHandleFlag, UseShader ) ;
	GD3D9.Device.DrawSetting.ChangeBlendParamFlag = TRUE ;
	GD3D9.Device.DrawSetting.ChangeTextureFlag    = TRUE ;
	GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag   = TRUE ;

	// 終了
	return 0 ;
}













#define __NONE		(-1)

#define __DIFF		D_D3DTA_DIFFUSE
#define __CURR		D_D3DTA_CURRENT
#define __TEX_		D_D3DTA_TEXTURE
#define __FACT		D_D3DTA_TFACTOR
#define __SPEC		D_D3DTA_SPECULAR
#define __COMP		D_D3DTA_COMPLEMENT
#define __ALRP		D_D3DTA_ALPHAREPLICATE
#define __TEMP		D_D3DTA_TEMP

#define __DISB		D_D3DTOP_DISABLE
#define __SEL1		D_D3DTOP_SELECTARG1
#define __SEL2		D_D3DTOP_SELECTARG2
#define __MUL1		D_D3DTOP_MODULATE
#define __MUL2		D_D3DTOP_MODULATE2X
#define __MUL4		D_D3DTOP_MODULATE4X
#define __ADD1		D_D3DTOP_ADD
#define __SUB1		D_D3DTOP_SUBTRACT
#define __BLDC		D_D3DTOP_BLENDCURRENTALPHA 
#define __BLDF		D_D3DTOP_BLENDFACTORALPHA 

#define __NUM(x)	((void *)(x))
#define DISB_ST		{NULL, -1, -1, __NONE,__NONE,__DISB,	__NONE,__NONE,__DISB},

// 減算ブレンド用ステート
static const GRAPHICS_HARDDATA_DIRECT3D9_TEXTURESTAGEINFO __TexSubBlendStageStateTable[USE_TEXTURESTAGE_NUM] =
{
	/*Stage 0*/{__NUM(1),0,0,__NONE,__DIFF,__SEL2,	__DIFF,__NONE,__SEL1},
	/*Stage 1*/{    NULL,0,0,__NONE,__NONE,__DISB,	__NONE,__NONE,__DISB},
	/*Stage 2*/{    NULL,0,0,__NONE,__NONE,__DISB,	__NONE,__NONE,__DISB},
	/*Stage 3*/{    NULL,0,0,__NONE,__NONE,__DISB,	__NONE,__NONE,__DISB},
	/*Stage 4*/{    NULL,0,0,__NONE,__NONE,__DISB,	__NONE,__NONE,__DISB},
	/*Stage 5*/{    NULL,0,0,__NONE,__NONE,__DISB,	__NONE,__NONE,__DISB},
	/*Stage 6*/{    NULL,0,0,__NONE,__NONE,__DISB,	__NONE,__NONE,__DISB},
	/*Stage 7*/{    NULL,0,0,__NONE,__NONE,__DISB,	__NONE,__NONE,__DISB},
	/*Stage 8*/{    NULL,0,0,__NONE,__NONE,__DISB,	__NONE,__NONE,__DISB},
	/*Stage 9*/{    NULL,0,0,__NONE,__NONE,__DISB,	__NONE,__NONE,__DISB},
	/*Stage10*/{    NULL,0,0,__NONE,__NONE,__DISB,	__NONE,__NONE,__DISB},
	/*Stage11*/{    NULL,0,0,__NONE,__NONE,__DISB,	__NONE,__NONE,__DISB},
} ;

// テクスチャーステージステート
//0:[0:通常描画 1:MUL 2:RGB反転 3:描画輝度4倍 4:乗算済みαブレンドモードの通常描画 5:乗算済みαブレンドモードのRGB反転 6:乗算済みαブレンドモードの描画輝度4倍]
//1:[0:ブレンドテクスチャ無し  1:DX_BLENDGRAPHTYPE_NORMAL  2:DX_BLENDGRAPHTYPE_WIPE  3:DX_BLENDGRAPHTYPE_ALPHA ]
//2:[0:ディフューズRGBカラー無し  1:ディフューズRGBカラーあり]
//3:[0:ディフューズAlphaカラー無し  1:ディフューズAlphaカラーあり]
//4:[0:αチャンネル無し　1:αチャンネルあり  2:αテストあり]
//5:[ テクスチャーステージナンバー ]
static const GRAPHICS_HARDDATA_DIRECT3D9_TEXTURESTAGEINFO __TexStageStateTable[ TEXTURE_BLEND_TABLE_INDEX_NUM ][4][2][2][3][USE_TEXTURESTAGE_NUM] =
{
	// 通常描画
	{
		// ブレンドテクスチャ無し
		{
			//ディフューズRGBカラー無し
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(1), 0, 0,__NONE,__DIFF,__SEL2,	__TEX_,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(1), 0, 0,__TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αテストあり
					{
						/*Stage0*/{__NUM(1), 0, 0,__TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(1), 0, 0,__NONE,__DIFF,__SEL2,	__TEX_,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(1), 0, 0,__TEX_,__DIFF,__MUL1,	__TEX_,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(1), 0, 0,__TEX_,__DIFF,__MUL1,	__TEX_,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
			//ディフューズRGBカラーあり
			{
				//ディフューズAlphaカラーなし
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(1), 0, 0,__NONE,__DIFF,__SEL2,	__TEX_,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(1), 0, 0,__TEX_,__NONE,__SEL1,	__TEX_,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(1), 0, 0,__TEX_,__NONE,__SEL1,	__TEX_,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(1), 0, 0,__NONE,__DIFF,__SEL2,	__TEX_,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(1), 0, 0,__TEX_,__DIFF,__MUL1,	__TEX_,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(1), 0, 0,__TEX_,__DIFF,__MUL1,	__TEX_,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
		},
		//ブレンドテクスチャあり DX_BLENDGRAPHTYPE_NORMAL
		{
			//ディフューズRGBカラー無し
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR,__TEX_,__BLDF}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR,__TEX_,__BLDF}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR,__TEX_,__BLDF}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR,__TEX_,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR,__TEX_,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR,__TEX_,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
			//ディフューズRGBカラーあり
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR,__TEX_,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR,__TEX_,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR,__TEX_,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR,__TEX_,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR,__TEX_,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR,__TEX_,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
		},
		//ブレンドテクスチャあり DX_BLENDGRAPHTYPE_WIPE
		{
			//ディフューズRGBカラー無し
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__FACT,__MUL1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __CURR,__DIFF,__MUL1,	__TEX_,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __SPEC,__CURR,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __DIFF,__CURR,__MUL1,	__TEX_,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(4), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __SPEC,__CURR,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_,__NONE,__SEL1},
						/*Stage3*/{    NULL, 0, 0, __DIFF,__CURR,__MUL1,	__CURR,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_,__NONE,__SEL1},
						/*Stage2*/{    NULL, 1, 0, __DIFF,__CURR,__MUL1,	__CURR,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
			//ディフューズRGBカラーあり
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __CURR,__DIFF,__MUL1,	__TEX_,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __SPEC,__CURR,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __DIFF,__CURR,__MUL1,	__TEX_,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(4), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __SPEC,__CURR,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_,__DIFF,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __DIFF,__CURR,__MUL1,	__CURR,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_,__DIFF,__MUL1},
						/*Stage2*/{    NULL, 1, 0, __DIFF,__CURR,__MUL1,	__CURR,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
		},
		//ブレンドテクスチャあり DX_BLENDGRAPHTYPE_ALPHA
		{
			//ディフューズRGBカラー無し
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(2), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(2), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(2), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(3), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
			//ディフューズRGBカラーあり
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(3), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(3), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
		},
	},

	// MUL描画
	{
		// ブレンドテクスチャ無し
		{
			//ディフューズRGBカラー無し
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(2), 0, 0, __NONE,__DIFF,__SEL2,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__NONE,__SEL1,	__CURR,__FACT,__BLDC}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__NONE,__SEL1,	__CURR,__FACT,__BLDC}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αテストあり
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__NONE,__SEL1,	__CURR,__FACT,__BLDC}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(2), 0, 0, __NONE,__DIFF,__SEL2,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__NONE,__SEL1,	__CURR,__FACT,__BLDC}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__DIFF,__MUL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__NONE,__SEL1,	__CURR,__FACT,__BLDC}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__DIFF,__MUL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__NONE,__SEL1,	__CURR,__FACT,__BLDC}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
			//ディフューズRGBカラーあり
			{
				//ディフューズAlphaカラーなし
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(2), 0, 0, __NONE,__DIFF,__SEL2,	__TEX_,__DIFF,__MUL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__NONE,__SEL1,	__CURR,__FACT,__BLDC}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__DIFF,__MUL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__NONE,__SEL1,	__CURR,__FACT,__BLDC}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__DIFF,__MUL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__NONE,__SEL1,	__CURR,__FACT,__BLDC}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(2), 0, 0, __NONE,__DIFF,__SEL2,	__TEX_,__DIFF,__MUL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__NONE,__SEL1,	__CURR,__FACT,__BLDC}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__DIFF,__MUL1,	__TEX_,__DIFF,__MUL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__NONE,__SEL1,	__CURR,__FACT,__BLDC}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__DIFF,__MUL1,	__TEX_,__DIFF,__MUL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__NONE,__SEL1,	__CURR,__FACT,__BLDC}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
		},
		//ブレンドテクスチャあり DX_BLENDGRAPHTYPE_NORMAL
		{
			//ディフューズRGBカラー無し
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						 DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						 DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						 DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						 DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						 DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						 DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
			//ディフューズRGBカラーあり
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						 DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						 DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						 DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						 DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						 DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						 DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
		},
		//ブレンドテクスチャあり DX_BLENDGRAPHTYPE_WIPE
		{
			//ディフューズRGBカラー無し
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						 DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						 DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						 DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						 DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						 DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						 DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
			//ディフューズRGBカラーあり
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						 DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						 DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						 DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						 DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						 DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						 DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
		},
		//ブレンドテクスチャあり DX_BLENDGRAPHTYPE_ALPHA
		{
			//ディフューズRGBカラー無し
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						 DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						 DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						 DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						 DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						 DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						 DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
			//ディフューズRGBカラーあり
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						 DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						 DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						 DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						 DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						 DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						 DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
		},
	},

	// RGB反転
	{
		// ブレンドテクスチャ無し
		{
			//ディフューズRGBカラー無し
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(1), 0, 0,__NONE,__DIFF,__SEL2,	__TEX_ | __COMP,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(1), 0, 0,__TEX_,__NONE,__SEL1,	__TEX_ | __COMP,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αテストあり
					{
						/*Stage0*/{__NUM(1), 0, 0,__TEX_,__NONE,__SEL1,	__TEX_ | __COMP,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(1), 0, 0,__NONE,__DIFF,__SEL2,	__TEX_ | __COMP,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(1), 0, 0,__TEX_,__DIFF,__MUL1,	__TEX_ | __COMP,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(1), 0, 0,__TEX_,__DIFF,__MUL1,	__TEX_ | __COMP,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
			//ディフューズRGBカラーあり
			{
				//ディフューズAlphaカラーなし
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(1), 0, 0, __NONE,__DIFF,__SEL2,	__TEX_ | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(1), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_ | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(1), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_ | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(1), 0, 0, __NONE,__DIFF,__SEL2,	__TEX_ | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(1), 0, 0, __TEX_,__DIFF,__MUL1,	__TEX_ | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(1), 0, 0, __TEX_,__DIFF,__MUL1,	__TEX_ | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
		},
		//ブレンドテクスチャあり DX_BLENDGRAPHTYPE_NORMAL
		{
			//ディフューズRGBカラー無し
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR,__TEX_,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR,__TEX_,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR,__TEX_,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR,__TEX_,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR,__TEX_,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR,__TEX_,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
			//ディフューズRGBカラーあり
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR,__TEX_,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR,__TEX_,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR,__TEX_,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR,__TEX_,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR,__TEX_,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR,__TEX_,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
		},
		//ブレンドテクスチャあり DX_BLENDGRAPHTYPE_WIPE
		{
			//ディフューズRGBカラー無し
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __DIFF,__CURR,__MUL1,	__TEX_ | __COMP,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __SPEC,__CURR,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_ | __COMP,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_ | __COMP,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __DIFF,__CURR,__MUL1,	__TEX_ | __COMP,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(4), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __SPEC,__CURR,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_ | __COMP,__NONE,__SEL1},
						/*Stage3*/{    NULL, 0, 0, __DIFF,__CURR,__MUL1,	__CURR,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_ | __COMP,__NONE,__SEL1},
						/*Stage2*/{    NULL, 1, 0, __DIFF,__CURR,__MUL1,	__CURR,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
			//ディフューズRGBカラーあり
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __CURR,__DIFF,__MUL1,	__TEX_ | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __SPEC,__CURR,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_ | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_ | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __DIFF,__CURR,__MUL1,	__TEX_ | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(4), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __SPEC,__CURR,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_ | __COMP,__DIFF,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __DIFF,__CURR,__MUL1,	__CURR,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_ | __COMP,__DIFF,__MUL1},
						/*Stage2*/{    NULL, 1, 0, __DIFF,__CURR,__MUL1,	__CURR,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
		},
		//ブレンドテクスチャあり DX_BLENDGRAPHTYPE_ALPHA
		{
			//ディフューズRGBカラー無し
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
			//ディフューズRGBカラーあり
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
		},
	},

	// 描画輝度４倍
	{
		// ブレンドテクスチャ無し
		{
			//ディフューズRGBカラー無し
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(1), 0, 0,__NONE,__DIFF,__SEL2,	__TEX_,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(1), 0, 0,__TEX_,__NONE,__SEL1,	__TEX_,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αテストあり
					{
						/*Stage0*/{__NUM(1), 0, 0,__TEX_,__NONE,__SEL1,	__TEX_,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(1), 0, 0,__NONE,__DIFF,__SEL2,	__TEX_,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(1), 0, 0,__TEX_,__DIFF,__MUL1,	__TEX_,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(1), 0, 0,__TEX_,__DIFF,__MUL1,	__TEX_,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
			//ディフューズRGBカラーあり
			{
				//ディフューズAlphaカラーなし
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(1), 0, 0,__NONE,__DIFF,__SEL2,	__TEX_,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(1), 0, 0,__TEX_,__NONE,__SEL1,	__TEX_,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(1), 0, 0,__TEX_,__NONE,__SEL1,	__TEX_,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(1), 0, 0,__NONE,__DIFF,__SEL2,	__TEX_,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(1), 0, 0,__TEX_,__DIFF,__MUL1,	__TEX_,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(1), 0, 0,__TEX_,__DIFF,__MUL1,	__TEX_,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
		},
		//ブレンドテクスチャあり DX_BLENDGRAPHTYPE_NORMAL
		{
			//ディフューズRGBカラー無し
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(5), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__SUB1,	__CURR,__TEX_,__SUB1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__FACT,__MUL1,	__CURR,__FACT,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __CURR,__TEX_,__ADD1,	__CURR,__TEX_,__ADD1},
						/*Stage4*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(5), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__SUB1,	__CURR,__TEX_,__SUB1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__FACT,__MUL1,	__CURR,__FACT,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __CURR,__TEX_,__ADD1,	__CURR,__TEX_,__ADD1},
						/*Stage4*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(5), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__SUB1,	__CURR,__TEX_,__SUB1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__FACT,__MUL1,	__CURR,__FACT,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __CURR,__TEX_,__ADD1,	__CURR,__TEX_,__ADD1},
						/*Stage4*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(5), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__SUB1,	__CURR,__TEX_,__SUB1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__FACT,__MUL1,	__CURR,__FACT,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __CURR,__TEX_,__ADD1,	__CURR,__TEX_,__ADD1},
						/*Stage4*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(5), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__SUB1,	__CURR,__TEX_,__SUB1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__FACT,__MUL1,	__CURR,__FACT,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __CURR,__TEX_,__ADD1,	__CURR,__TEX_,__ADD1},
						/*Stage4*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(5), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__SUB1,	__CURR,__TEX_,__SUB1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__FACT,__MUL1,	__CURR,__FACT,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __CURR,__TEX_,__ADD1,	__CURR,__TEX_,__ADD1},
						/*Stage4*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
			//ディフューズRGBカラーあり
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(5), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__SUB1,	__CURR,__TEX_,__SUB1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__FACT,__MUL1,	__CURR,__FACT,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __CURR,__TEX_,__ADD1,	__CURR,__TEX_,__ADD1},
						/*Stage4*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(5), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__SUB1,	__CURR,__TEX_,__SUB1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__FACT,__MUL1,	__CURR,__FACT,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __CURR,__TEX_,__ADD1,	__CURR,__TEX_,__ADD1},
						/*Stage4*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(5), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__SUB1,	__CURR,__TEX_,__SUB1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__FACT,__MUL1,	__CURR,__FACT,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __CURR,__TEX_,__ADD1,	__CURR,__TEX_,__ADD1},
						/*Stage4*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(5), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__SUB1,	__CURR,__TEX_,__SUB1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__FACT,__MUL1,	__CURR,__FACT,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __CURR,__TEX_,__ADD1,	__CURR,__TEX_,__ADD1},
						/*Stage4*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(5), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__SUB1,	__CURR,__TEX_,__SUB1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__FACT,__MUL1,	__CURR,__FACT,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __CURR,__TEX_,__ADD1,	__CURR,__TEX_,__ADD1},
						/*Stage4*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(5), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__SUB1,	__CURR,__TEX_,__SUB1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__FACT,__MUL1,	__CURR,__FACT,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __CURR,__TEX_,__ADD1,	__CURR,__TEX_,__ADD1},
						/*Stage4*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
		},
		//ブレンドテクスチャあり DX_BLENDGRAPHTYPE_WIPE
		{
			//ディフューズRGBカラー無し
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __CURR,__DIFF,__MUL1,	__TEX_,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __SPEC,__CURR,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __DIFF,__CURR,__MUL1,	__TEX_,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(4), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __SPEC,__CURR,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_,__DIFF,__MUL4},
						/*Stage3*/{    NULL, 0, 0, __DIFF,__CURR,__MUL1,	__CURR,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_,__DIFF,__MUL4},
						/*Stage2*/{    NULL, 1, 0, __DIFF,__CURR,__MUL1,	__CURR,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
			//ディフューズRGBカラーあり
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __CURR,__DIFF,__MUL1,	__TEX_,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __SPEC,__CURR,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __DIFF,__CURR,__MUL1,	__TEX_,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(4), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __SPEC,__CURR,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_,__DIFF,__MUL4},
						/*Stage3*/{    NULL, 0, 0, __DIFF,__CURR,__MUL1,	__CURR,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_,__DIFF,__MUL4},
						/*Stage2*/{    NULL, 1, 0, __DIFF,__CURR,__MUL1,	__CURR,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
		},
		//ブレンドテクスチャあり DX_BLENDGRAPHTYPE_ALPHA
		{
			//ディフューズRGBカラー無し
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
			//ディフューズRGBカラーあり
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
		},
	},

	// 乗算済みαブレンドモードの通常描画
	{
		// ブレンドテクスチャ無し
		{
			//ディフューズRGBカラー無し
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(1), 0, 0,__NONE,__DIFF,__SEL2,	__TEX_,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(1), 0, 0,__TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αテストあり
					{
						/*Stage0*/{__NUM(1), 0, 0,__TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(1), 0, 0,__NONE,__DIFF,__SEL2,	__TEX_,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(1), 0, 0,__TEX_,__DIFF,__MUL1,	__TEX_,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(1), 0, 0,__TEX_,__DIFF,__MUL1,	__TEX_,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
			//ディフューズRGBカラーあり
			{
				//ディフューズAlphaカラーなし
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(1), 0, 0,__NONE,__DIFF,__SEL2,	__TEX_,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(1), 0, 0,__TEX_,__NONE,__SEL1,	__TEX_,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(1), 0, 0,__TEX_,__NONE,__SEL1,	__TEX_,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(2), 0, 0,__NONE,__DIFF,__SEL2,	__TEX_,__DIFF         ,__MUL1},
						/*Stage1*/{       0, 0, 0,__CURR,__NONE,__SEL1,	__CURR,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(2), 0, 0,__TEX_,__DIFF,__MUL1,	__TEX_,__DIFF         ,__MUL1},
						/*Stage1*/{       0, 0, 0,__CURR,__NONE,__SEL1,	__CURR,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(2), 0, 0,__TEX_,__DIFF,__MUL1,	__TEX_,__DIFF         ,__MUL1},
						/*Stage1*/{       0, 0, 0,__CURR,__NONE,__SEL1,	__CURR,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
		},
		//ブレンドテクスチャあり DX_BLENDGRAPHTYPE_NORMAL
		{
			//ディフューズRGBカラー無し
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR,__TEX_,__BLDF}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR,__TEX_,__BLDF}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR,__TEX_,__BLDF}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE         ,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR,__TEX_         ,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE         ,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR,__TEX_         ,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE         ,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR,__TEX_         ,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
			//ディフューズRGBカラーあり
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR,__TEX_,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR,__TEX_,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR,__TEX_,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(4), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE         ,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR,__TEX_         ,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF         ,__MUL1}, 
						/*Stage3*/{    NULL, 0, 0, __CURR,__NONE,__SEL1,	__CURR,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(4), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE         ,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR,__TEX_         ,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF         ,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __CURR,__NONE,__SEL1,	__CURR,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(4), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE         ,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR,__TEX_         ,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF         ,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __CURR,__NONE,__SEL1,	__CURR,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
		},
		//ブレンドテクスチャあり DX_BLENDGRAPHTYPE_WIPE
		{
			//ディフューズRGBカラー無し
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__FACT,__MUL1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __CURR,__DIFF,__MUL1,	__TEX_,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __SPEC,__CURR,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF         ,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __DIFF,__CURR,__MUL1,	__TEX_,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(4), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF         ,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __SPEC,__CURR,__MUL1,	__CURR,__NONE         ,__SEL1},
						/*Stage2*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_,__DIFF | __ALRP,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __DIFF,__CURR,__MUL1,	__CURR,__NONE         ,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF         ,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_,__DIFF | __ALRP,__MUL1},
						/*Stage2*/{    NULL, 1, 0, __DIFF,__CURR,__MUL1,	__CURR,__NONE         ,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
			//ディフューズRGBカラーあり
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __CURR,__DIFF,__MUL1,	__TEX_,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __SPEC,__CURR,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF         ,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __DIFF,__CURR,__MUL1,	__TEX_,__DIFF         ,__MUL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__NONE,__SEL1,	__CURR,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(4), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF         ,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __SPEC,__CURR,__MUL1,	__CURR,__NONE         ,__SEL1},
						/*Stage2*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_,__DIFF         ,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __DIFF,__CURR,__MUL1,	__CURR,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF         ,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_,__DIFF         ,__MUL1},
						/*Stage2*/{    NULL, 1, 0, __DIFF,__CURR,__MUL1,	__CURR,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
		},
		//ブレンドテクスチャあり DX_BLENDGRAPHTYPE_ALPHA
		{
			//ディフューズRGBカラー無し
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(2), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(2), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(2), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(3), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__DIFF | __ALRP,__MUL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE         ,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF         ,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__DIFF | __ALRP,__MUL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE         ,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF         ,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__DIFF | __ALRP,__MUL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE         ,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF         ,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
			//ディフューズRGBカラーあり
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(3), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(3), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__DIFF | __ALRP,__MUL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE         ,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF         ,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__DIFF | __ALRP,__MUL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE         ,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF         ,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__DIFF | __ALRP,__MUL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE         ,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF         ,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
		},
	},

	// 乗算済みαブレンドモードのRGB反転
	{
		// ブレンドテクスチャ無し
		{
			//ディフューズRGBカラー無し
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(1), 0, 0,__NONE,__DIFF,__SEL2,	__TEX_ | __COMP,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(1), 0, 0,__TEX_,__NONE,__SEL1,	__TEX_ | __COMP,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αテストあり
					{
						/*Stage0*/{__NUM(1), 0, 0,__TEX_,__NONE,__SEL1,	__TEX_ | __COMP,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(1), 0, 0,__NONE,__DIFF,__SEL2,	__TEX_ | __COMP,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(1), 0, 0,__TEX_,__DIFF,__MUL1,	__TEX_ | __COMP,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(1), 0, 0,__TEX_,__DIFF,__MUL1,	__TEX_ | __COMP,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
			//ディフューズRGBカラーあり
			{
				//ディフューズAlphaカラーなし
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(1), 0, 0, __NONE,__DIFF,__SEL2,	__TEX_ | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(1), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_ | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(1), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_ | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(2), 0, 0, __NONE,__DIFF,__SEL2,	__TEX_ | __COMP,__DIFF         ,__MUL1}, 
						/*Stage1*/{       0, 0, 0, __CURR,__NONE,__SEL1,	__CURR         ,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__DIFF,__MUL1,	__TEX_ | __COMP,__DIFF         ,__MUL1},
						/*Stage1*/{       0, 0, 0, __CURR,__NONE,__SEL1,	__CURR         ,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__DIFF,__MUL1,	__TEX_ | __COMP,__DIFF         ,__MUL1},
						/*Stage1*/{       0, 0, 0, __CURR,__NONE,__SEL1,	__CURR         ,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
		},
		//ブレンドテクスチャあり DX_BLENDGRAPHTYPE_NORMAL
		{
			//ディフューズRGBカラー無し
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_         ,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR         ,__TEX_,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_         ,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR         ,__TEX_,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_         ,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR         ,__TEX_,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_         ,__NONE         ,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR         ,__TEX_         ,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_         ,__NONE         ,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR         ,__TEX_         ,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_         ,__NONE         ,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR         ,__TEX_         ,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
			//ディフューズRGBカラーあり
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_         ,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR         ,__TEX_,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_         ,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR         ,__TEX_,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_         ,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR         ,__TEX_,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(4), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_         ,__NONE         ,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR         ,__TEX_         ,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF         ,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __CURR,__NONE,__SEL1,	__CURR         ,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(4), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_         ,__NONE         ,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR         ,__TEX_         ,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF         ,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __CURR,__NONE,__SEL1,	__CURR         ,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(4), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_         ,__NONE         ,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__BLDF,	__CURR         ,__TEX_         ,__BLDF},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF         ,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __CURR,__NONE,__SEL1,	__CURR         ,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
		},
		//ブレンドテクスチャあり DX_BLENDGRAPHTYPE_WIPE
		{
			//ディフューズRGBカラー無し
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__FACT,__ADD1,	__NONE         ,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __DIFF,__CURR,__MUL1,	__TEX_ | __COMP,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__FACT,__ADD1,	__NONE         ,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __SPEC,__CURR,__MUL1,	__CURR         ,__NONE,__SEL1},
						/*Stage2*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_ | __COMP,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__FACT,__ADD1,	__NONE         ,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_ | __COMP,__NONE,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__FACT,__ADD1,	__NONE         ,__DIFF         ,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __DIFF,__CURR,__MUL1,	__TEX_ | __COMP,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(4), 0, 0, __TEX_,__FACT,__ADD1,	__NONE         ,__DIFF         ,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __SPEC,__CURR,__MUL1,	__CURR         ,__NONE         ,__SEL1},
						/*Stage2*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_ | __COMP,__DIFF | __ALRP,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __DIFF,__CURR,__MUL1,	__CURR         ,__NONE         ,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__FACT,__ADD1,	__NONE         ,__DIFF         ,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_ | __COMP,__DIFF | __ALRP,__MUL1},
						/*Stage2*/{    NULL, 1, 0, __DIFF,__CURR,__MUL1,	__CURR         ,__NONE         ,__SEL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
			//ディフューズRGBカラーあり
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__FACT,__ADD1,	__NONE         ,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __CURR,__DIFF,__MUL1,	__TEX_ | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__FACT,__ADD1,	__NONE         ,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __SPEC,__CURR,__MUL1,	__CURR         ,__NONE,__SEL1},
						/*Stage2*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_ | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__FACT,__ADD1,	__NONE         ,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_ | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__FACT,__ADD1,	__NONE         ,__DIFF         ,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __DIFF,__CURR,__MUL1,	__TEX_ | __COMP,__DIFF         ,__MUL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__CURR,__SEL1,	__CURR         ,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(4), 0, 0, __TEX_,__FACT,__ADD1,	__NONE         ,__DIFF         ,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __SPEC,__CURR,__MUL1,	__CURR         ,__NONE         ,__SEL1},
						/*Stage2*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_ | __COMP,__DIFF         ,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __DIFF,__CURR,__MUL1,	__CURR         ,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__FACT,__ADD1,	__NONE         ,__DIFF         ,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_ | __COMP,__DIFF         ,__MUL1},
						/*Stage2*/{    NULL, 1, 0, __DIFF,__CURR,__MUL1,	__CURR         ,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
		},
		//ブレンドテクスチャあり DX_BLENDGRAPHTYPE_ALPHA
		{
			//ディフューズRGBカラー無し
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_         ,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR         ,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_         ,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR         ,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_         ,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR         ,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(4), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_         ,__NONE         ,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR         ,__NONE         ,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF         ,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __CURR,__NONE,__SEL1,	__CURR         ,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(4), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_         ,__NONE         ,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR         ,__NONE         ,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF         ,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __CURR,__NONE,__SEL1,	__CURR         ,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(4), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_         ,__NONE         ,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR         ,__NONE         ,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF         ,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __CURR,__NONE,__SEL1,	__CURR         ,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
			//ディフューズRGBカラーあり
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_         ,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR         ,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_         ,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR         ,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_         ,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR         ,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(4), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_         ,__NONE         ,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR         ,__NONE         ,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF         ,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __CURR,__NONE,__SEL1,	__CURR         ,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(4), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_         ,__NONE         ,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR         ,__NONE         ,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF         ,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __CURR,__NONE,__SEL1,	__CURR         ,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(4), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_         ,__NONE         ,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR         ,__NONE         ,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR | __COMP,__DIFF         ,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __CURR,__NONE,__SEL1,	__CURR         ,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
		},
	},

	// 乗算済みαブレンドモードの描画輝度４倍
	{
		// ブレンドテクスチャ無し
		{
			//ディフューズRGBカラー無し
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(1), 0, 0,__NONE,__DIFF,__SEL2,	__TEX_,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(1), 0, 0,__TEX_,__NONE,__SEL1,	__TEX_,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αテストあり
					{
						/*Stage0*/{__NUM(1), 0, 0,__TEX_,__NONE,__SEL1,	__TEX_,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(2), 0, 0,__NONE,__DIFF,__SEL2,	__TEX_,__DIFF         ,__MUL4},
						/*Stage1*/{    NULL, 0, 0,__CURR,__NONE,__SEL1,	__CURR,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(2), 0, 0,__TEX_,__DIFF,__MUL1,	__TEX_,__DIFF         ,__MUL4},
						/*Stage1*/{    NULL, 0, 0,__CURR,__NONE,__SEL1,	__CURR,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(2), 0, 0,__TEX_,__DIFF,__MUL1,	__TEX_,__DIFF         ,__MUL4},
						/*Stage1*/{    NULL, 0, 0,__CURR,__NONE,__SEL1,	__CURR,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
			//ディフューズRGBカラーあり
			{
				//ディフューズAlphaカラーなし
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(1), 0, 0,__NONE,__DIFF,__SEL2,	__TEX_,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(1), 0, 0,__TEX_,__NONE,__SEL1,	__TEX_,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(1), 0, 0,__TEX_,__NONE,__SEL1,	__TEX_,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(2), 0, 0,__NONE,__DIFF,__SEL2,	__TEX_,__DIFF         ,__MUL4},
						/*Stage1*/{    NULL, 0, 0,__CURR,__NONE,__SEL1,	__CURR,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(2), 0, 0,__TEX_,__DIFF,__MUL1,	__TEX_,__DIFF         ,__MUL4},
						/*Stage1*/{    NULL, 0, 0,__CURR,__NONE,__SEL1,	__CURR,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(2), 0, 0,__TEX_,__DIFF,__MUL1,	__TEX_,__DIFF         ,__MUL4},
						/*Stage1*/{    NULL, 0, 0,__CURR,__NONE,__SEL1,	__CURR,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
		},
		//ブレンドテクスチャあり DX_BLENDGRAPHTYPE_NORMAL
		{
			//ディフューズRGBカラー無し
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(5), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__SUB1,	__CURR,__TEX_,__SUB1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__FACT,__MUL1,	__CURR,__FACT,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __CURR,__TEX_,__ADD1,	__CURR,__TEX_,__ADD1},
						/*Stage4*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(5), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__SUB1,	__CURR,__TEX_,__SUB1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__FACT,__MUL1,	__CURR,__FACT,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __CURR,__TEX_,__ADD1,	__CURR,__TEX_,__ADD1},
						/*Stage4*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(5), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__SUB1,	__CURR,__TEX_,__SUB1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__FACT,__MUL1,	__CURR,__FACT,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __CURR,__TEX_,__ADD1,	__CURR,__TEX_,__ADD1},
						/*Stage4*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(6), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE         ,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__SUB1,	__CURR,__TEX_         ,__SUB1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__FACT,__MUL1,	__CURR,__FACT         ,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __CURR,__TEX_,__ADD1,	__CURR,__TEX_         ,__ADD1},
						/*Stage4*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF         ,__MUL4},
						/*Stage5*/{    NULL, 0, 0, __CURR,__NONE,__SEL1,	__CURR,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(6), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE         ,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__SUB1,	__CURR,__TEX_         ,__SUB1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__FACT,__MUL1,	__CURR,__FACT         ,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __CURR,__TEX_,__ADD1,	__CURR,__TEX_         ,__ADD1},
						/*Stage4*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF         ,__MUL4},
						/*Stage5*/{    NULL, 0, 0, __CURR,__NONE,__SEL1,	__CURR,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(6), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE         ,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__SUB1,	__CURR,__TEX_         ,__SUB1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__FACT,__MUL1,	__CURR,__FACT         ,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __CURR,__TEX_,__ADD1,	__CURR,__TEX_         ,__ADD1},
						/*Stage4*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF         ,__MUL4},
						/*Stage5*/{    NULL, 0, 0, __CURR,__NONE,__SEL1,	__CURR,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
			//ディフューズRGBカラーあり
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(5), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__SUB1,	__CURR,__TEX_,__SUB1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__FACT,__MUL1,	__CURR,__FACT,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __CURR,__TEX_,__ADD1,	__CURR,__TEX_,__ADD1},
						/*Stage4*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(5), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__SUB1,	__CURR,__TEX_,__SUB1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__FACT,__MUL1,	__CURR,__FACT,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __CURR,__TEX_,__ADD1,	__CURR,__TEX_,__ADD1},
						/*Stage4*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(5), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__SUB1,	__CURR,__TEX_,__SUB1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__FACT,__MUL1,	__CURR,__FACT,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __CURR,__TEX_,__ADD1,	__CURR,__TEX_,__ADD1},
						/*Stage4*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(6), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE         ,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__SUB1,	__CURR,__TEX_         ,__SUB1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__FACT,__MUL1,	__CURR,__FACT         ,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __CURR,__TEX_,__ADD1,	__CURR,__TEX_         ,__ADD1},
						/*Stage4*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF         ,__MUL4},
						/*Stage5*/{    NULL, 0, 0, __CURR,__NONE,__SEL1,	__CURR,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(6), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE         ,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__SUB1,	__CURR,__TEX_         ,__SUB1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__FACT,__MUL1,	__CURR,__FACT         ,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __CURR,__TEX_,__ADD1,	__CURR,__TEX_         ,__ADD1},
						/*Stage4*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF         ,__MUL4},
						/*Stage5*/{    NULL, 0, 0, __CURR,__NONE,__SEL1,	__CURR,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(6), 1, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE         ,__SEL1},
						/*Stage1*/{    NULL, 0, 0, __CURR,__TEX_,__SUB1,	__CURR,__TEX_         ,__SUB1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__FACT,__MUL1,	__CURR,__FACT         ,__MUL1},
						/*Stage3*/{    NULL, 0, 0, __CURR,__TEX_,__ADD1,	__CURR,__TEX_         ,__ADD1},
						/*Stage4*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF         ,__MUL4},
						/*Stage5*/{    NULL, 0, 0, __CURR,__NONE,__SEL1,	__CURR,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
		},
		//ブレンドテクスチャあり DX_BLENDGRAPHTYPE_WIPE
		{
			//ディフューズRGBカラー無し
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __CURR,__DIFF,__MUL1,	__TEX_,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __SPEC,__CURR,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF         ,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __DIFF,__CURR,__MUL1,	__TEX_,__DIFF         ,__MUL4},
						/*Stage2*/{    NULL, 0, 0, __CURR,__NONE,__SEL1,	__CURR,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(4), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF         ,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __SPEC,__CURR,__MUL1,	__CURR,__NONE         ,__SEL1},
						/*Stage2*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_,__DIFF         ,__MUL4},
						/*Stage3*/{    NULL, 0, 0, __DIFF,__CURR,__MUL1,	__CURR,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF         ,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_,__DIFF         ,__MUL4},
						/*Stage2*/{    NULL, 1, 0, __DIFF,__CURR,__MUL1,	__CURR,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
			//ディフューズRGBカラーあり
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __CURR,__DIFF,__MUL1,	__TEX_,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __SPEC,__CURR,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF         ,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __DIFF,__CURR,__MUL1,	__TEX_,__DIFF         ,__MUL4},
						/*Stage2*/{    NULL, 0, 0, __CURR,__NONE,__SEL1,	__CURR,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(4), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF         ,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __SPEC,__CURR,__MUL1,	__CURR,__NONE         ,__SEL1},
						/*Stage2*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_,__DIFF         ,__MUL4},
						/*Stage3*/{    NULL, 0, 0, __DIFF,__CURR,__MUL1,	__CURR,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__FACT,__ADD1,	__NONE,__DIFF         ,__SEL2},
						/*Stage1*/{    NULL, 1, 0, __TEX_,__CURR,__MUL1,	__TEX_,__DIFF         ,__MUL4},
						/*Stage2*/{    NULL, 1, 0, __DIFF,__CURR,__MUL1,	__CURR,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
		},
		//ブレンドテクスチャあり DX_BLENDGRAPHTYPE_ALPHA
		{
			//ディフューズRGBカラー無し
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(2), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(4), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE         ,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE         ,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF         ,__MUL4},
						/*Stage3*/{    NULL, 0, 0, __CURR,__NONE,__SEL1,	__CURR,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(4), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE         ,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE         ,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF         ,__MUL4},
						/*Stage3*/{    NULL, 0, 0, __CURR,__NONE,__SEL1,	__CURR,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(4), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE         ,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE         ,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF         ,__MUL4},
						/*Stage3*/{    NULL, 0, 0, __CURR,__NONE,__SEL1,	__CURR,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
			//ディフューズRGBカラーあり
			{
				//ディフューズAlphaカラー無し
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(3), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF,__MUL4}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
				//ディフューズAlphaカラーあり
				{
					//αチャンネル無し
					{
						/*Stage0*/{__NUM(4), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE         ,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE         ,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF         ,__MUL4},
						/*Stage3*/{    NULL, 0, 0, __CURR,__NONE,__SEL1,	__CURR,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},

					//αチャンネルあり
					{
						/*Stage0*/{__NUM(4), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE         ,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE         ,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF         ,__MUL4},
						/*Stage3*/{    NULL, 0, 0, __CURR,__NONE,__SEL1,	__CURR,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
					
					//αテストあり
					{
						/*Stage0*/{__NUM(4), 0, 0, __TEX_,__NONE,__SEL1,	__TEX_,__NONE         ,__SEL1},
						/*Stage1*/{    NULL, 1, 0, __CURR,__TEX_,__MUL1,	__CURR,__NONE         ,__SEL1},
						/*Stage2*/{    NULL, 0, 0, __CURR,__DIFF,__MUL1,	__CURR,__DIFF         ,__MUL4},
						/*Stage3*/{    NULL, 0, 0, __CURR,__NONE,__SEL1,	__CURR,__DIFF | __ALRP,__MUL1}, DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST DISB_ST
					},
				},
			},
		},
	},
} ;

#undef __DIFF
#undef __CURR
#undef __TEX_
#undef __FACT
#undef __SPEC
#undef __TEMP

#undef __DISB
#undef __SEL1
#undef __SEL2
#undef __MUL1
#undef __MUL2
#undef __MUL4
#undef __ADD1

#undef __NUM

// ブレンド設定を更新(Direct3D に反映)する
extern int  Graphics_D3D9_DeviceState_RefreshBlendState(
	GRAPHICS_HARDDATA_DIRECT3D9_BLENDINFO *pUserInfo,
	int TextureStageIsTextureAndTextureCoordOnlyFlag,
	int TextureIsGraphHandleFlag,
	int UseShader
)
{
	GRAPHICS_HARDDATA_DIRECT3D9_BLENDINFO BlendInfo ;
	int AlphaTableIndex ;
	int BlendTextureIndex ;
	int NextBlendMode ;
	DWORD i ;
	DWORD StageNum ;
	int TextureOnly ;
	GRAPHICS_HARDDATA_DIRECT3D9_TEXTURESTAGEINFO *Dest ;
	const GRAPHICS_HARDDATA_DIRECT3D9_TEXTURESTAGEINFO *Src, *TextureStageInfo = NULL ;
	D_IDirect3DTexture9 *StageTexture[USE_TEXTURESTAGE_NUM] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL } ;
	D_IDirect3DPixelShader9 *UsePixelShader = NULL ;
	IMAGEDATA *Image ;
	SHADOWMAPDATA *ShadowMap ;
	int UseFloatFactorColor = FALSE ;
	FLOAT4 FloatFactorColor = { 0.0f } ;
	GRAPHICS_HARDDATA_DIRECT3D9_TEXTURESTAGEINFO TempTextureStageInfo[ USE_TEXTURESTAGE_NUM ] ;
	int UseShaderFlag ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

#ifndef DX_NON_NORMAL_DRAW_SHADER
	UseShaderFlag = GSYS.HardInfo.UseShader && ( GD3D9.NormalDraw_NotUsePixelShader == FALSE ? TRUE : FALSE ) && UseShader ;
#else
	UseShaderFlag = FALSE ;
#endif // DX_NON_NORMAL_DRAW_SHADER

	// ブレンドテクスチャがある場合は、基本的にシェーダーを使用しないように処理を変更
	if( GD3D9.Device.DrawSetting.BlendTexture != NULL )
	{
		UseShaderFlag = FALSE ;
	}

	for( i = 0 ; i < USE_TEXTURESTAGE_NUM ; i ++ )
	{
		BlendInfo.TextureStageInfo[i].ResultTempARG = FALSE ;
	}

	BlendInfo.SeparateAlphaBlendEnable = FALSE ;
	BlendInfo.SrcBlendAlpha = -1 ;
	BlendInfo.DestBlendAlpha = -1 ;
	BlendInfo.BlendOpAlpha = -1 ;

	// ユーザー指定のブレンド情報があるかどうかで処理を分岐
	TextureOnly = FALSE ;
	if( pUserInfo )
	{
		StageNum = ( DWORD )pUserInfo->UseTextureStageNum ;

		_MEMCPY( &BlendInfo, pUserInfo, sizeof( GRAPHICS_HARDDATA_DIRECT3D9_BLENDINFO ) - sizeof( pUserInfo->TextureStageInfo ) ) ;
		TextureStageInfo = pUserInfo->TextureStageInfo ;
		TextureOnly = TextureStageIsTextureAndTextureCoordOnlyFlag ;

		// グラフィックハンドルからテクスチャを取得する
		if( TextureIsGraphHandleFlag )
		{
			for( i = 0 ; i < StageNum ; i ++ )
			{
				if( !GRAPHCHKFULL( ( LONG_PTR )pUserInfo->TextureStageInfo[ i ].Texture, Image ) )
				{
					StageTexture[ i ] = Image->Hard.Draw[ 0 ].Tex->PF->D3D9.Texture ;
				}
				else
				if( !SHADOWMAPCHKFULL( ( LONG_PTR )pUserInfo->TextureStageInfo[ i ].Texture, ShadowMap ) )
				{
					StageTexture[ i ] = ShadowMap->PF->D3D9.Texture ;
				}
				else
				{
					StageTexture[ i ] = NULL ;
				}
			}
		}
		else
		{
			for( i = 0 ; i < StageNum ; i ++ )
			{
				StageTexture[ i ] = ( D_IDirect3DTexture9 * )pUserInfo->TextureStageInfo[ i ].Texture ;
			}
		}

//		GRH.UseAlphaChDrawShader = FALSE ;
	}
	else
	{
		// 初期化フラグが立っておらず、且つパラメータが一切変更されて無かったら何もしない
		if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE && GD3D9.Device.DrawSetting.ChangeBlendParamFlag == FALSE && GD3D9.Device.DrawSetting.ChangeTextureFlag == FALSE ) return -1 ; 

		// 描画テクスチャのみ変更された場合は描画テクスチャの変更のみ行う
		if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE && GD3D9.Device.DrawSetting.ChangeBlendParamFlag == FALSE && GD3D9.Device.DrawSetting.ChangeTextureFlag == TRUE )
		{
			// ブレンドテクスチャがある場合と無い場合で処理を分岐
			if( GD3D9.Device.DrawSetting.BlendTexture != NULL )
			{
				// ある場合

				if( UseShaderFlag )
				{
					// 使用するテクスチャーアドレスをセット
					Direct3DDevice9_SetTexture( 0, GD3D9.Device.DrawSetting.RenderTexture ) ;
					Direct3DDevice9_SetTexture( D_D3DDMAPSAMPLER + 1 + 0, GD3D9.Device.DrawSetting.RenderTexture ) ;
					GD3D9.Device.State.BlendInfo.TextureStageInfo[0].Texture = GD3D9.Device.DrawSetting.RenderTexture ;

					Direct3DDevice9_SetTexture( 1, GD3D9.Device.DrawSetting.BlendTexture ) ;
					Direct3DDevice9_SetTexture( D_D3DDMAPSAMPLER + 1 + 1, GD3D9.Device.DrawSetting.BlendTexture ) ;
					GD3D9.Device.State.BlendInfo.TextureStageInfo[1].Texture = GD3D9.Device.DrawSetting.BlendTexture ;
				}
				else
				{
					// モードによって処理を変更
					switch( GD3D9.Device.DrawSetting.BlendGraphType )
					{
					case DX_BLENDGRAPHTYPE_NORMAL :
						Direct3DDevice9_SetTexture( 0, GD3D9.Device.DrawSetting.BlendTexture ) ;
						Direct3DDevice9_SetTexture( D_D3DDMAPSAMPLER + 1 + 0, GD3D9.Device.DrawSetting.BlendTexture ) ;
						GD3D9.Device.State.BlendInfo.TextureStageInfo[0].Texture = GD3D9.Device.DrawSetting.BlendTexture ;

						Direct3DDevice9_SetTexture( 1, GD3D9.Device.DrawSetting.RenderTexture ) ;
						Direct3DDevice9_SetTexture( D_D3DDMAPSAMPLER + 1 + 1, GD3D9.Device.DrawSetting.RenderTexture ) ;
						GD3D9.Device.State.BlendInfo.TextureStageInfo[1].Texture = GD3D9.Device.DrawSetting.RenderTexture ;

						Direct3DDevice9_SetTexture( 3, GD3D9.Device.DrawSetting.RenderTexture ) ;
						Direct3DDevice9_SetTexture( D_D3DDMAPSAMPLER + 1 + 3, GD3D9.Device.DrawSetting.RenderTexture ) ;
						GD3D9.Device.State.BlendInfo.TextureStageInfo[3].Texture = GD3D9.Device.DrawSetting.RenderTexture ;
						break;

					case DX_BLENDGRAPHTYPE_WIPE :
						Direct3DDevice9_SetTexture( 0, GD3D9.Device.DrawSetting.BlendTexture ) ;
						Direct3DDevice9_SetTexture( D_D3DDMAPSAMPLER + 1 + 0, GD3D9.Device.DrawSetting.BlendTexture ) ;
						GD3D9.Device.State.BlendInfo.TextureStageInfo[0].Texture = GD3D9.Device.DrawSetting.BlendTexture ;

						// αチャンネルが有効な場合とそうでない場合で処理を分岐
						if( GD3D9.Device.DrawSetting.AlphaChannelValidFlag == TRUE )
						{
							Direct3DDevice9_SetTexture( 2, GD3D9.Device.DrawSetting.RenderTexture ) ;
							Direct3DDevice9_SetTexture( D_D3DDMAPSAMPLER + 1 + 2, GD3D9.Device.DrawSetting.RenderTexture ) ;
							GD3D9.Device.State.BlendInfo.TextureStageInfo[2].Texture = GD3D9.Device.DrawSetting.RenderTexture ;
	//						StageTexture[2] = GD3D9.Device.DrawSetting.RenderTexture ;
						}
						else
						{
							Direct3DDevice9_SetTexture( 1, GD3D9.Device.DrawSetting.RenderTexture ) ;
							Direct3DDevice9_SetTexture( D_D3DDMAPSAMPLER + 1 + 1, GD3D9.Device.DrawSetting.RenderTexture ) ;
							GD3D9.Device.State.BlendInfo.TextureStageInfo[1].Texture = GD3D9.Device.DrawSetting.RenderTexture ;
	//						StageTexture[1] = GD3D9.Device.DrawSetting.RenderTexture ;
						}
						break ;

					case DX_BLENDGRAPHTYPE_ALPHA :
						Direct3DDevice9_SetTexture( 0, GD3D9.Device.DrawSetting.RenderTexture ) ;
						Direct3DDevice9_SetTexture( D_D3DDMAPSAMPLER + 1 + 0, GD3D9.Device.DrawSetting.RenderTexture ) ;
						GD3D9.Device.State.BlendInfo.TextureStageInfo[0].Texture = GD3D9.Device.DrawSetting.RenderTexture ;

						Direct3DDevice9_SetTexture( 1, GD3D9.Device.DrawSetting.BlendTexture ) ;
						Direct3DDevice9_SetTexture( D_D3DDMAPSAMPLER + 1 + 1, GD3D9.Device.DrawSetting.BlendTexture ) ;
						GD3D9.Device.State.BlendInfo.TextureStageInfo[1].Texture = GD3D9.Device.DrawSetting.BlendTexture ;
						break;
					}
				}
			}
			else
			{
				// 無い場合
				Direct3DDevice9_SetTexture( 0, GD3D9.Device.DrawSetting.RenderTexture ) ;
				Direct3DDevice9_SetTexture( D_D3DDMAPSAMPLER + 1 + 0, GD3D9.Device.DrawSetting.RenderTexture ) ;
				GD3D9.Device.State.BlendInfo.TextureStageInfo[0].Texture = GD3D9.Device.DrawSetting.RenderTexture ;
	//			StageTexture[0] = GD3D9.Device.DrawSetting.RenderTexture ;
			}

			// テクスチャが変更された、フラグを倒す
			GD3D9.Device.DrawSetting.ChangeTextureFlag = FALSE ;
			GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

			// 終了
			return 0 ;
		}

		// テクスチャが変更された、フラグを倒す
		GD3D9.Device.DrawSetting.ChangeTextureFlag = FALSE ;
		GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

		// パラメータが変更された、フラグを倒す
		GD3D9.Device.DrawSetting.ChangeBlendParamFlag = FALSE ;

		// 描画待機している描画物を描画
//		DRAWSTOCKINFO

		// ブレンドモードの決定
		{
			NextBlendMode = GD3D9.Device.DrawSetting.BlendMode ;
			switch( GD3D9.Device.DrawSetting.BlendMode )
			{
			case DX_BLENDMODE_SUB :
				// 減算ブレンド指定で D3DRS_BLENDOP が使用可能な場合は 14番目のブレンドモードを使用する
				if( GD3D9.Device.Caps.ValidDestBlendOp )
				{
					NextBlendMode = DX_BLENDMODE_SUB1 ;
				}
				break ;

			case DX_BLENDMODE_NOBLEND :
				// 描画先にα値がある場合は DX_BLENDMODE_NOBLEND でもブレンドモードは DX_BLENDMODE_SRCCOLOR にする
				if( GSYS.DrawSetting.AlphaChDrawMode )
				{
					NextBlendMode = DX_BLENDMODE_SRCCOLOR ;
				}

				// 描画元画像にαチャンネルがある場合やブレンド画像の有無など条件次第で DX_BLENDMODE_ALPHA を使用する
				if( GD3D9.Device.DrawSetting.RenderTexture != NULL )
				{
					if( GD3D9.Device.DrawSetting.BlendTexture != NULL )
					{
						switch( GD3D9.Device.DrawSetting.BlendGraphType )
						{
						case DX_BLENDGRAPHTYPE_NORMAL :
						case DX_BLENDGRAPHTYPE_ALPHA :
							if( GD3D9.Device.DrawSetting.AlphaTestValidFlag == TRUE || GD3D9.Device.DrawSetting.AlphaChannelValidFlag == TRUE )
							{
								NextBlendMode = DX_BLENDMODE_ALPHA ;
							}
							break ;

						case DX_BLENDGRAPHTYPE_WIPE :
							NextBlendMode = DX_BLENDMODE_ALPHA ;
							break ;
						}
					}
					else
					{
						if( GD3D9.Device.DrawSetting.AlphaChannelValidFlag == TRUE )
						{
							NextBlendMode = DX_BLENDMODE_ALPHA ;
						}
					}
				}
				break ;
			}
		}
		
		// ソースブレンドとデストブレンドの値をセット
		BlendInfo.DestBlend = __RGBBlendStateTable[ NextBlendMode ][ RGB_BLENDSTATE_TABLE_DESTBLEND ] ;
		BlendInfo.SrcBlend  = __RGBBlendStateTable[ NextBlendMode ][ RGB_BLENDSTATE_TABLE_SRCBLEND ] ;
		BlendInfo.BlendOp   = __RGBBlendStateTable[ NextBlendMode ][ RGB_BLENDSTATE_TABLE_BLENDOP ] ;
		BlendInfo.FactorColor = 0xffffffff ;

		// Direct3D のαブレンド機能を使用するブレンドモードかどうかを取得する
		BlendInfo.AlphaBlendEnable = __RGBBlendStateTable[ NextBlendMode ][ RGB_BLENDSTATE_TABLE_ALPHABLENDENABLE ] ;
/*
		// 書き込みのα値を正しく計算するブレンドモードを使用するかどうかで処理を分岐
		GRH.UseAlphaChDrawShader = FALSE ;
		if( 0 ) // 重いのでとりあえず無効化
		{
			if( UseShaderFlag && GSYS.DrawSetting.AlphaChDrawMode )
			{
				// ブレンド処理にシェーダーを使用する場合とそうでない場合で分岐
				if( __RGBBlendStateTable[ NextBlendMode ][ RGB_BLENDSTATE_TABLE_SHADERALPHABLEND ] == 1 )
				{
					GRH.UseAlphaChDrawShader = TRUE ;

					BlendInfo.SeparateAlphaBlendEnable = FALSE ;
					BlendInfo.SrcBlend       = D_D3DBLEND_ONE ;
					BlendInfo.DestBlend      = D_D3DBLEND_ZERO ;
					BlendInfo.BlendOp        = D_D3DBLENDOP_ADD ;
					BlendInfo.SrcBlendAlpha  = -1 ;
					BlendInfo.DestBlendAlpha = -1 ;
					BlendInfo.BlendOpAlpha   = -1 ;
				}
			}
		}
*/
		// カレントテクスチャが無い場合とある場合で分岐
		if( GD3D9.Device.DrawSetting.RenderTexture == NULL )
		{
			// カレントテクスチャが無い場合は線や箱の描画と判断、テクスチャステージは無効にする

			// とりあえず描画処理を行う
			GD3D9.Device.DrawInfo.BlendMaxNotDrawFlag = FALSE ;
			
			// シェーダーを使用するかどうかで処理を分岐
#ifndef DX_NON_NORMAL_DRAW_SHADER
			if( UseShaderFlag )
			{
				UsePixelShader = GD3D9.Device.Shader.Base.BaseNoneTexPixelShader[ __RGBBlendStateTable[ NextBlendMode ][ RGB_BLENDSTATE_TABLE_TEXBLENDTABLEINDEX ] ][ 0 /*GRH.UseAlphaChDrawShader*/ ] ;
				StageNum = 0 ;
			}
			else
#endif // DX_NON_NORMAL_DRAW_SHADER
			{
				// 減算ブレンドの反転処理の時のみ専用のテーブルを使用する
				if( NextBlendMode == DX_BLENDMODE_SUB )
				{
					TextureStageInfo = __TexSubBlendStageStateTable ;
					StageNum         = ( DWORD )(DWORD_PTR)TextureStageInfo[0].Texture ;
				}
				else
				{
					IMAGEDATA *WhiteTex = NULL ;

					BlendInfo.TextureStageInfo[0].TextureCoordIndex = 0 ;
					BlendInfo.TextureStageInfo[1].TextureCoordIndex = 0 ;
					BlendInfo.TextureStageInfo[1].AlphaARG1 = D_D3DTA_DIFFUSE ;
					BlendInfo.TextureStageInfo[1].AlphaARG2 = D_D3DTA_DIFFUSE ;
					BlendInfo.TextureStageInfo[1].AlphaOP   = D_D3DTOP_DISABLE ;
					BlendInfo.TextureStageInfo[1].ColorARG1 = D_D3DTA_DIFFUSE ;
					BlendInfo.TextureStageInfo[1].ColorARG2 = D_D3DTA_DIFFUSE ;
					BlendInfo.TextureStageInfo[1].ColorOP   = D_D3DTOP_DISABLE ;
					StageNum = 1 ;
					switch( __RGBBlendStateTable[ NextBlendMode ][ RGB_BLENDSTATE_TABLE_TEXBLENDTABLEINDEX ] )
					{
					case TEXTURE_BLEND_TABLE_INDEX_NORMAL :	// 通常描画
						BlendInfo.TextureStageInfo[0].AlphaARG1 = D_D3DTA_DIFFUSE ;
						BlendInfo.TextureStageInfo[0].AlphaARG2 = D_D3DTA_DIFFUSE ;
						BlendInfo.TextureStageInfo[0].AlphaOP   = D_D3DTOP_SELECTARG2 ;
						BlendInfo.TextureStageInfo[0].ColorARG1 = D_D3DTA_DIFFUSE ;
						BlendInfo.TextureStageInfo[0].ColorARG2 = D_D3DTA_DIFFUSE ;
						BlendInfo.TextureStageInfo[0].ColorOP   = D_D3DTOP_SELECTARG2 ;
						break ;

					case TEXTURE_BLEND_TABLE_INDEX_MUL :	// 乗算描画
						BlendInfo.TextureStageInfo[0].AlphaARG1 = D_D3DTA_DIFFUSE ;
						BlendInfo.TextureStageInfo[0].AlphaARG2 = D_D3DTA_DIFFUSE ;
						BlendInfo.TextureStageInfo[0].AlphaOP   = D_D3DTOP_SELECTARG2 ;
						BlendInfo.TextureStageInfo[0].ColorARG1 = D_D3DTA_DIFFUSE ;
						BlendInfo.TextureStageInfo[0].ColorARG2 = D_D3DTA_TFACTOR ;
						BlendInfo.TextureStageInfo[0].ColorOP   = D_D3DTOP_BLENDDIFFUSEALPHA ;
						break ;

					case TEXTURE_BLEND_TABLE_INDEX_INVERSE :	// ＲＧＢ反転
						BlendInfo.TextureStageInfo[0].AlphaARG1 = D_D3DTA_DIFFUSE ;
						BlendInfo.TextureStageInfo[0].AlphaARG2 = D_D3DTA_DIFFUSE ;
						BlendInfo.TextureStageInfo[0].AlphaOP   = D_D3DTOP_SELECTARG2 ;
						BlendInfo.TextureStageInfo[0].ColorARG1 = D_D3DTA_DIFFUSE | D_D3DTA_COMPLEMENT;
						BlendInfo.TextureStageInfo[0].ColorARG2 = D_D3DTA_DIFFUSE ;
						BlendInfo.TextureStageInfo[0].ColorOP   = D_D3DTOP_SELECTARG1 ;
						break ;

					case TEXTURE_BLEND_TABLE_INDEX_X4 :	// 描画輝度4倍
						GRAPHCHK( Graphics_Image_GetWhiteTexHandle(), WhiteTex ) ;
						Graphics_D3D9_BeginScene() ;
						StageTexture[0] = WhiteTex->Orig->Hard.Tex[ 0 ].PF->D3D9.Texture ;
						BlendInfo.TextureStageInfo[0].AlphaARG1 = D_D3DTA_DIFFUSE ;
						BlendInfo.TextureStageInfo[0].AlphaARG2 = D_D3DTA_DIFFUSE ;
						BlendInfo.TextureStageInfo[0].AlphaOP   = D_D3DTOP_SELECTARG2 ;
						BlendInfo.TextureStageInfo[0].ColorARG1 = D_D3DTA_DIFFUSE ;
						BlendInfo.TextureStageInfo[0].ColorARG2 = D_D3DTA_TEXTURE ;
						BlendInfo.TextureStageInfo[0].ColorOP   = D_D3DTOP_MODULATE4X ;
						break ;

					case TEXTURE_BLEND_TABLE_INDEX_PMA_NORMAL :		// 乗算済みαブレンドモードの通常描画
						BlendInfo.TextureStageInfo[0].AlphaARG1 = D_D3DTA_DIFFUSE ;
						BlendInfo.TextureStageInfo[0].AlphaARG2 = D_D3DTA_DIFFUSE ;
						BlendInfo.TextureStageInfo[0].AlphaOP   = D_D3DTOP_SELECTARG2 ;
						BlendInfo.TextureStageInfo[0].ColorARG1 = D_D3DTA_DIFFUSE ;
						BlendInfo.TextureStageInfo[0].ColorARG2 = D_D3DTA_DIFFUSE | D_D3DTA_ALPHAREPLICATE ;
						BlendInfo.TextureStageInfo[0].ColorOP   = D_D3DTOP_MODULATE ;
						break ;

					case TEXTURE_BLEND_TABLE_INDEX_PMA_INVERSE :	// 乗算済みαブレンドモードのRGB反転
						BlendInfo.TextureStageInfo[0].AlphaARG1 = D_D3DTA_DIFFUSE ;
						BlendInfo.TextureStageInfo[0].AlphaARG2 = D_D3DTA_DIFFUSE ;
						BlendInfo.TextureStageInfo[0].AlphaOP   = D_D3DTOP_SELECTARG2 ;
						BlendInfo.TextureStageInfo[0].ColorARG1 = D_D3DTA_DIFFUSE | D_D3DTA_COMPLEMENT;
						BlendInfo.TextureStageInfo[0].ColorARG2 = D_D3DTA_DIFFUSE | D_D3DTA_ALPHAREPLICATE ;
						BlendInfo.TextureStageInfo[0].ColorOP   = D_D3DTOP_MODULATE ;
						break ;

					case TEXTURE_BLEND_TABLE_INDEX_PMA_X4 :			// 乗算済みαブレンドモードの描画輝度4倍
						GRAPHCHK( Graphics_Image_GetWhiteTexHandle(), WhiteTex ) ;
						Graphics_D3D9_BeginScene() ;
						StageTexture[0] = WhiteTex->Orig->Hard.Tex[ 0 ].PF->D3D9.Texture ;
						BlendInfo.TextureStageInfo[0].AlphaARG1 = D_D3DTA_DIFFUSE ;
						BlendInfo.TextureStageInfo[0].AlphaARG2 = D_D3DTA_DIFFUSE ;
						BlendInfo.TextureStageInfo[0].AlphaOP   = D_D3DTOP_SELECTARG2 ;
						BlendInfo.TextureStageInfo[0].ColorARG1 = D_D3DTA_DIFFUSE ;
						BlendInfo.TextureStageInfo[0].ColorARG2 = D_D3DTA_TEXTURE ;
						BlendInfo.TextureStageInfo[0].ColorOP   = D_D3DTOP_MODULATE4X ;

						BlendInfo.TextureStageInfo[1].AlphaARG1 = D_D3DTA_CURRENT ;
						BlendInfo.TextureStageInfo[1].AlphaARG2 = D_D3DTA_DIFFUSE ;
						BlendInfo.TextureStageInfo[1].AlphaOP   = D_D3DTOP_SELECTARG1 ;
						BlendInfo.TextureStageInfo[1].ColorARG1 = D_D3DTA_CURRENT ;
						BlendInfo.TextureStageInfo[1].ColorARG2 = D_D3DTA_DIFFUSE | D_D3DTA_ALPHAREPLICATE ;
						BlendInfo.TextureStageInfo[1].ColorOP   = D_D3DTOP_MODULATE ;

						BlendInfo.TextureStageInfo[2].AlphaARG1 = D_D3DTA_DIFFUSE ;
						BlendInfo.TextureStageInfo[2].AlphaARG2 = D_D3DTA_DIFFUSE ;
						BlendInfo.TextureStageInfo[2].AlphaOP   = D_D3DTOP_DISABLE ;
						BlendInfo.TextureStageInfo[2].ColorARG1 = D_D3DTA_DIFFUSE ;
						BlendInfo.TextureStageInfo[2].ColorARG2 = D_D3DTA_DIFFUSE ;
						BlendInfo.TextureStageInfo[2].ColorOP   = D_D3DTOP_DISABLE ;
						StageNum = 2 ;
						break ;
					}

					TextureStageInfo = BlendInfo.TextureStageInfo ;
				}
			}

			// αテストは行わない
			BlendInfo.AlphaTestEnable = FALSE ;
			BlendInfo.AlphaRef        = GD3D9.Device.State.BlendInfo.AlphaRef ;
			BlendInfo.AlphaFunc       = GD3D9.Device.State.BlendInfo.AlphaFunc ;

			// 頂点タイプのセット
			GD3D9.Device.DrawInfo.VertexType = VERTEXTYPE_NOTEX ;
			GD3D9.Device.DrawInfo.VertexBufferNextAddr = GD3D9.Device.DrawInfo.VertexBufferAddr[ GD3D9.Device.DrawInfo.Use3DVertex ][ GD3D9.Device.DrawInfo.VertexType ] ;
		}
		else
		{
			// カレントテクスチャがある場合はテクスチャステージブレンドテーブルから値を取得する

			// ブレンドテクスチャーがあるかどうかを取得する
			BlendTextureIndex = GD3D9.Device.DrawSetting.BlendTexture != NULL ? GD3D9.Device.DrawSetting.BlendGraphType + 1 : 0 ;

			// 使用するテクスチャブレンドステージステータステーブルのインデックスをセット
			AlphaTableIndex = 0 ;
			if( GD3D9.Device.DrawSetting.AlphaChannelValidFlag || GD3D9.Device.DrawSetting.AlphaTestValidFlag || ( GD3D9.Device.State.ZEnable && BlendTextureIndex == 0 ) )
			{
				AlphaTableIndex = GD3D9.Device.DrawSetting.AlphaChannelValidFlag ? 1 : 2 ;
			}

			// ブレンドテクスチャーを使用するかどうかで分岐
			if( BlendTextureIndex != 0 )
			{
				int TableIndex ;

				// ブレンドテクスチャーがある場合は AlphaTestValidFlag は無視

				// 使用するテーブルのインデックスを割り出す
				TableIndex = __RGBBlendStateTable[ NextBlendMode ][ RGB_BLENDSTATE_TABLE_TEXBLENDTABLEINDEX ] ;
/*
				if( NextBlendMode == DX_BLENDMODE_ADD_X4 || NextBlendMode == DX_BLENDMODE_ALPHA_X4 )
				{
					TableIndex = TEXTURE_BLEND_TABLE_INDEX_X4 ;
				}
				else
				if( NextBlendMode == DX_BLENDMODE_INVSRC )
				{
					TableIndex = TEXTURE_BLEND_TABLE_INDEX_INVERSE ;
				}
				else
				{
					TableIndex = TEXTURE_BLEND_TABLE_INDEX_NORMAL ;
				}
*/

				// シェーダーを使用するかどうかで処理を分岐
				if( UseShaderFlag )
				{
					// 使用するテクスチャーアドレスをセット
					StageTexture[0] = GD3D9.Device.DrawSetting.RenderTexture ;
					StageTexture[1] = GD3D9.Device.DrawSetting.BlendTexture ;
					StageTexture[2] = NULL ;
					StageNum = 2 ;
				}
				else
				{
					// テクスチャーステージステートの設定値を取得する
					TextureStageInfo = BlendInfo.TextureStageInfo ;
					memcpy( BlendInfo.TextureStageInfo,
						__TexStageStateTable[ TableIndex ][ BlendTextureIndex ][ GD3D9.Device.DrawSetting.UseDiffuseRGBColor ][ GD3D9.Device.DrawSetting.UseDiffuseAlphaColor ][ AlphaTableIndex ],
							 sizeof( GRAPHICS_HARDDATA_DIRECT3D9_TEXTURESTAGEINFO ) * USE_TEXTURESTAGE_NUM ) ;

/*
					int p[5] ;

					p[0] = TableIndex ;
					p[1] = BlendTextureIndex ;
					p[2] = GD3D9.Device.DrawSetting.UseDiffuseRGBColor ;
					p[3] = GD3D9.Device.DrawSetting.UseDiffuseAlphaColor ;
					p[4] = AlphaTableIndex ;
*/
					StageNum = ( DWORD )(DWORD_PTR)TextureStageInfo[0].Texture ;
				}

				// 頂点タイプのセット
				GD3D9.Device.DrawInfo.VertexType = VERTEXTYPE_BLENDTEX ;
				GD3D9.Device.DrawInfo.VertexBufferNextAddr = GD3D9.Device.DrawInfo.VertexBufferAddr[ GD3D9.Device.DrawInfo.Use3DVertex ][ GD3D9.Device.DrawInfo.VertexType ] ;

				// ブレンドタイプによって処理を分岐
				switch( GD3D9.Device.DrawSetting.BlendGraphType )
				{
				case DX_BLENDGRAPHTYPE_NORMAL :
					// とりあえず描画処理を行う
					GD3D9.Device.DrawInfo.BlendMaxNotDrawFlag = FALSE ;

					// α処理が有効な場合はα値が０の場合のみ表示されないようにする
					if( GD3D9.Device.DrawSetting.AlphaTestValidFlag == TRUE || BlendInfo.AlphaBlendEnable == TRUE || GD3D9.Device.DrawSetting.AlphaChannelValidFlag == TRUE )
					{
						BlendInfo.AlphaTestEnable = TRUE ;
						BlendInfo.AlphaRef        = 0 ;
						BlendInfo.AlphaFunc       = D_D3DCMP_GREATER ;
					}
					else
					{
						BlendInfo.AlphaTestEnable = FALSE ;
						BlendInfo.AlphaRef        = GD3D9.Device.State.BlendInfo.AlphaRef ;
						BlendInfo.AlphaFunc       = GD3D9.Device.State.BlendInfo.AlphaFunc ;
					}

					// クロスフェード率を設定する
					BlendInfo.FactorColor = ( ( DWORD )GD3D9.Device.DrawSetting.BlendGraphFadeRatio << 24 ) | 0x00ffffff ;

					// シェーダーを使用するかどうかで処理を分岐
					if( UseShaderFlag == FALSE )
					{
						// 使用するテクスチャーアドレスをセットと頂点タイプ、テクスチャステージが使用するテクスチャ座標のインデックスをセットする
						StageTexture[0] = GD3D9.Device.DrawSetting.BlendTexture ;
						StageTexture[1] = GD3D9.Device.DrawSetting.RenderTexture ;
						StageTexture[2] = NULL ;

						// テクスチャカラー無視フラグが立っている場合はテクスチャ要素を無効にする
						if( GD3D9.Device.DrawSetting.IgnoreGraphColorFlag )
						{
							BlendInfo.TextureStageInfo[ 0 ].ColorARG1 = D_D3DTA_DIFFUSE ;
							BlendInfo.TextureStageInfo[ 0 ].ColorARG2 = -1 ;
							BlendInfo.TextureStageInfo[ 0 ].ColorOP = D_D3DTOP_SELECTARG1 ;
							BlendInfo.TextureStageInfo[ 1 ].ColorARG1 = D_D3DTA_CURRENT ;
							BlendInfo.TextureStageInfo[ 1 ].ColorARG2 = -1 ;
							BlendInfo.TextureStageInfo[ 1 ].ColorOP = D_D3DTOP_SELECTARG1 ;
							if( StageNum == 3 )
							{
								BlendInfo.TextureStageInfo[ 2 ].ColorARG1 = D_D3DTA_CURRENT ;
								BlendInfo.TextureStageInfo[ 2 ].ColorARG2 = -1 ;
								BlendInfo.TextureStageInfo[ 2 ].ColorOP = D_D3DTOP_SELECTARG1 ;
							}
						}
					}
#ifndef DX_NON_NORMAL_DRAW_SHADER
					else
					{
						// 使用するシェーダーをセット
						UsePixelShader = GD3D9.Device.Shader.Base.BaseUseTexPixelShader[ 1 ][ TableIndex ][ GD3D9.Device.DrawSetting.IgnoreGraphColorFlag ? 1 : 0 ][ AlphaTableIndex == 0 ? 1 : 0 ][ 0/*GRH.UseAlphaChDrawShader*/ ] ;
					}
#endif // DX_NON_NORMAL_DRAW_SHADER
					break ;

				case DX_BLENDGRAPHTYPE_WIPE :
					// ブレンド境界値が最大の場合は何も描画しない
					GD3D9.Device.DrawInfo.BlendMaxNotDrawFlag = GD3D9.Device.DrawSetting.BlendGraphBorderParam == 255 ? TRUE : FALSE ;

					// シェーダーを使用するかどうかで処理を分岐
					if( UseShaderFlag == FALSE )
					{
						// 使用するテクスチャーアドレスをセットと頂点タイプ、テクスチャステージが使用するテクスチャ座標のインデックスをセットする
						StageTexture[0] = GD3D9.Device.DrawSetting.BlendTexture ;
						if( GD3D9.Device.DrawSetting.AlphaChannelValidFlag == TRUE )
						{
							StageTexture[1] = NULL ;
							StageTexture[2] = GD3D9.Device.DrawSetting.RenderTexture ;
						}
						else
						{
							StageTexture[1] = GD3D9.Device.DrawSetting.RenderTexture ;
						}
					}
#ifndef DX_NON_NORMAL_DRAW_SHADER
					else
					{
						// 使用するシェーダーをセット
						UsePixelShader = GD3D9.Device.Shader.Base.BaseUseTexPixelShader[ 2 ][ TableIndex ][ GD3D9.Device.DrawSetting.IgnoreGraphColorFlag ? 1 : 0 ][ AlphaTableIndex == 0 ? 1 : 0 ][ 0/*GRH.UseAlphaChDrawShader*/ ] ;

						// float型の FactorColor を使用する
						UseFloatFactorColor = TRUE ;
						FloatFactorColor.x = 0.0f ;
						FloatFactorColor.y = 0.0f ;
						FloatFactorColor.z = 0.0f ;
						FloatFactorColor.w = 0.0f ;
					}
#endif // DX_NON_NORMAL_DRAW_SHADER

					// 境界範囲が１以下かどうかで処理を分岐
					if( GD3D9.Device.DrawSetting.BlendGraphBorderRange <= 1 )
					{
						// ここはもう殆ど例外処理 -------------------------------------------

						// αテストを使用する
						BlendInfo.AlphaTestEnable = TRUE ;
						BlendInfo.AlphaRef = GD3D9.Device.DrawSetting.BlendGraphBorderParam ;
						BlendInfo.AlphaFunc = D_D3DCMP_GREATEREQUAL ;

						// αブレンド機能ＯＦＦ
						BlendInfo.AlphaBlendEnable = FALSE ;

						// 増分するα値は無し
						if( UseShaderFlag )
						{
							FloatFactorColor.w = 0.0f ;
							FloatFactorColor.z = 1.0f ;
						}
						else
						{
							BlendInfo.FactorColor = 0x00FFFFFF | ( 0 << 24 ) ;
						}

						// もし画像がαチャンネルを使用していて、且つ BlendGraphBorderParam が 0 だったら
						// AlphaRef を０にしない
						if( ( GD3D9.Device.DrawSetting.AlphaChannelValidFlag || GD3D9.Device.DrawSetting.AlphaTestValidFlag ) && GD3D9.Device.DrawSetting.BlendGraphBorderParam == 0 )
						{
							BlendInfo.AlphaRef = 1 ;
						}

						// -------------------------------------------------------------------
					}
					else
					{
						int p ;
					
						// αブレンドによるテクスチャブレンド

						// αテストは行わない
						BlendInfo.AlphaTestEnable = FALSE ;
						BlendInfo.AlphaRef        = GD3D9.Device.State.BlendInfo.AlphaRef ;
						BlendInfo.AlphaFunc       = GD3D9.Device.State.BlendInfo.AlphaFunc ;

						// αブレンド機能ＯＮ
						BlendInfo.AlphaBlendEnable = TRUE ;

						// 境界範囲によって４段階
						if( GD3D9.Device.DrawSetting.BlendGraphBorderRange <= 64 )
						{
							// ×４

							p = 1280 - GD3D9.Device.DrawSetting.BlendGraphBorderParam * 1280 / 255 ; 
							BlendInfo.TextureStageInfo[1].AlphaOP = D_D3DTOP_MODULATE4X ;

							// ブレンドテクスチャのブレンド値によって合成パラメータを変更
							if( p < 1024 )
							{
								BlendInfo.FactorColor = ( DWORD )( 0x00FFFFFF | ( ( 255 - p * 255 / 1024 ) << 24 ) ) ;
								BlendInfo.TextureStageInfo[0].AlphaOP = D_D3DTOP_SUBTRACT ;

								if( UseShaderFlag )
								{
									FloatFactorColor.w = - ( float )( BlendInfo.FactorColor >> 24 ) / 255.0f ;
								}
							}
							else
							{
								BlendInfo.FactorColor = ( DWORD )( 0x00FFFFFF | ( ( 64 * ( p - 1024 ) / 256 ) << 24 ) ) ;
								BlendInfo.TextureStageInfo[0].AlphaOP = D_D3DTOP_ADD ;

								if( UseShaderFlag )
								{
									FloatFactorColor.w = ( float )( BlendInfo.FactorColor >> 24 ) / 255.0f ;
								}
							}

							if( UseShaderFlag )
							{
								FloatFactorColor.z = 4.0f ;
							}
						}
						else
						if( GD3D9.Device.DrawSetting.BlendGraphBorderRange <= 128 )
						{
							// ×２

							p = 768 - GD3D9.Device.DrawSetting.BlendGraphBorderParam * 768 / 255 ; 
							BlendInfo.TextureStageInfo[1].AlphaOP = D_D3DTOP_MODULATE2X ;

							// ブレンドテクスチャのブレンド値によって合成パラメータを変更
							if( p < 512 )
							{
								BlendInfo.FactorColor = ( DWORD )( 0x00FFFFFF | ( ( 255 - p * 255 / 512 ) << 24 ) ) ;
								BlendInfo.TextureStageInfo[0].AlphaOP = D_D3DTOP_SUBTRACT ;

								if( UseShaderFlag )
								{
									FloatFactorColor.w = - ( float )( BlendInfo.FactorColor >> 24 ) / 255.0f ;
								}
							}
							else
							{
								BlendInfo.FactorColor = ( DWORD )( 0x00FFFFFF | ( ( 128 * ( p - 512 ) / 256 ) << 24 ) ) ;
								BlendInfo.TextureStageInfo[0].AlphaOP = D_D3DTOP_ADD ;

								if( UseShaderFlag )
								{
									FloatFactorColor.w = ( float )( BlendInfo.FactorColor >> 24 ) / 255.0f ;
								}
							}

							if( UseShaderFlag )
							{
								FloatFactorColor.z = 2.0f ;
							}
						}
						else
						{
							// ×１

							p = 512 - GD3D9.Device.DrawSetting.BlendGraphBorderParam * 512 / 255 ; 
							BlendInfo.TextureStageInfo[1].AlphaOP = D_D3DTOP_MODULATE ;

							// ブレンドテクスチャのブレンド値によって合成パラメータを変更
							if( p < 256 )
							{
								BlendInfo.FactorColor = ( DWORD )( 0x00FFFFFF | ( ( 255 - p * 255 / 256 ) << 24 ) ) ;
								BlendInfo.TextureStageInfo[0].AlphaOP = D_D3DTOP_SUBTRACT ;

								if( UseShaderFlag )
								{
									FloatFactorColor.w = - ( float )( BlendInfo.FactorColor >> 24 ) / 255.0f ;
								}
							}
							else
							{
								BlendInfo.FactorColor = ( DWORD )( 0x00FFFFFF | ( ( 255 * ( p - 256 ) / 256 ) << 24 ) ) ;
								BlendInfo.TextureStageInfo[0].AlphaOP = D_D3DTOP_ADD ;

								if( UseShaderFlag )
								{
									FloatFactorColor.w = ( float )( BlendInfo.FactorColor >> 24 ) / 255.0f ;
								}
							}

							if( UseShaderFlag )
							{
								FloatFactorColor.z = 1.0f ;
							}
						}
					}

					if( UseShaderFlag == FALSE )
					{
						// テクスチャカラー無視フラグが立っている場合はテクスチャ要素を無効にする
						if( GD3D9.Device.DrawSetting.IgnoreGraphColorFlag )
						{
							int TargetNumber ;

							if( GD3D9.Device.DrawSetting.AlphaChannelValidFlag == TRUE )
							{
								TargetNumber = 2 ;
							}
							else
							{
								TargetNumber = 1 ;
							}

							if( BlendInfo.TextureStageInfo[ TargetNumber ].ColorARG1 == D_D3DTA_TEXTURE &&
								BlendInfo.TextureStageInfo[ TargetNumber ].ColorOP == D_D3DTOP_SELECTARG1 )
							{
								BlendInfo.TextureStageInfo[ TargetNumber ].ColorARG1 = D_D3DTA_CURRENT ;
							}
							else
							if( BlendInfo.TextureStageInfo[ TargetNumber ].ColorARG1 == ( D_D3DTA_TEXTURE | D_D3DTA_COMPLEMENT ) &&
								BlendInfo.TextureStageInfo[ TargetNumber ].ColorOP == D_D3DTOP_SELECTARG1 )
							{
								BlendInfo.TextureStageInfo[ TargetNumber ].ColorARG1 = D_D3DTA_CURRENT | D_D3DTA_COMPLEMENT ;
							}
							else
							if( BlendInfo.TextureStageInfo[ TargetNumber ].ColorARG1 == D_D3DTA_TEXTURE &&
								( BlendInfo.TextureStageInfo[ TargetNumber ].ColorOP == D_D3DTOP_MODULATE ||
								  BlendInfo.TextureStageInfo[ TargetNumber ].ColorOP == D_D3DTOP_MODULATE2X ||
								  BlendInfo.TextureStageInfo[ TargetNumber ].ColorOP == D_D3DTOP_MODULATE4X ) )
							{
								BlendInfo.TextureStageInfo[ TargetNumber ].ColorOP = D_D3DTOP_SELECTARG2 ;
							}
						}
					}
					break ;

				case DX_BLENDGRAPHTYPE_ALPHA :
					// とりあえず描画処理を行う
					GD3D9.Device.DrawInfo.BlendMaxNotDrawFlag = FALSE ;

					// α処理が有効な場合はα値が０の場合のみ表示されないようにする
					if( BlendInfo.AlphaBlendEnable == TRUE )
					{
						BlendInfo.AlphaTestEnable = TRUE ;
						BlendInfo.AlphaRef        = 0 ;
						BlendInfo.AlphaFunc       = D_D3DCMP_GREATER ;
					}
					else
					{
						BlendInfo.AlphaTestEnable = FALSE ;
						BlendInfo.AlphaRef        = GD3D9.Device.State.BlendInfo.AlphaRef ;
						BlendInfo.AlphaFunc       = GD3D9.Device.State.BlendInfo.AlphaFunc ;
					}

					// シェーダーを使用するかどうかで処理を分岐
					if( UseShaderFlag == FALSE )
					{
						// 使用するテクスチャーアドレスをセットと頂点タイプ、テクスチャステージが使用するテクスチャ座標のインデックスをセットする
						StageTexture[0] = GD3D9.Device.DrawSetting.RenderTexture ;
						StageTexture[1] = GD3D9.Device.DrawSetting.BlendTexture ;
						StageTexture[2] = NULL ;

						// テクスチャカラー無視フラグが立っている場合はテクスチャカラー要素を無効にする
						if( GD3D9.Device.DrawSetting.IgnoreGraphColorFlag )
						{
							BlendInfo.TextureStageInfo[ 0 ].ColorARG1 = D_D3DTA_DIFFUSE ;
						}
					}
#ifndef DX_NON_NORMAL_DRAW_SHADER
					else
					{
						// 使用するシェーダーをセット
						UsePixelShader = GD3D9.Device.Shader.Base.BaseUseTexPixelShader[ 3 ][ TableIndex ][ GD3D9.Device.DrawSetting.IgnoreGraphColorFlag ? 1 : 0 ][ AlphaTableIndex == 0 ? 1 : 0 ][ 0/*GRH.UseAlphaChDrawShader*/ ] ;
					}
#endif // DX_NON_NORMAL_DRAW_SHADER
					break ;
				}
			}
			else
			{
				// ブレンドテクスチャーが無い場合

				// とりあえず描画処理を行う
				GD3D9.Device.DrawInfo.BlendMaxNotDrawFlag = FALSE ;

				// 頂点タイプのセット
				GD3D9.Device.DrawInfo.VertexType = VERTEXTYPE_TEX ;
				GD3D9.Device.DrawInfo.VertexBufferNextAddr = GD3D9.Device.DrawInfo.VertexBufferAddr[ GD3D9.Device.DrawInfo.Use3DVertex ][ GD3D9.Device.DrawInfo.VertexType ] ;

#ifndef DX_NON_NORMAL_DRAW_SHADER
				// シェーダーを使用するかどうかで処理を分岐
				if( UseShaderFlag )
				{
					// 使用するピクセルシェーダーをセット
					UsePixelShader = GD3D9.Device.Shader.Base.BaseUseTexPixelShader[ 0 ][ __RGBBlendStateTable[NextBlendMode][RGB_BLENDSTATE_TABLE_TEXBLENDTABLEINDEX] ][ GD3D9.Device.DrawSetting.IgnoreGraphColorFlag ? 1 : 0 ][ AlphaTableIndex == 0 ? 1 : 0 ][ 0/*GRH.UseAlphaChDrawShader*/ ] ;

					// 使用するテクスチャの数は一つ
					StageNum = 1 ;
				}
				else
#endif // DX_NON_NORMAL_DRAW_SHADER
				{
					// テクスチャーステージステートの設定値を取得する
					TextureStageInfo = __TexStageStateTable[ __RGBBlendStateTable[ NextBlendMode ][ RGB_BLENDSTATE_TABLE_TEXBLENDTABLEINDEX ] ][ FALSE ][ GD3D9.Device.DrawSetting.UseDiffuseRGBColor ][ GD3D9.Device.DrawSetting.UseDiffuseAlphaColor ][ AlphaTableIndex ] ;
					StageNum = ( DWORD )(DWORD_PTR)TextureStageInfo[0].Texture ;

					// テクスチャカラー無視フラグが立っている場合はテクスチャ要素を無効にする
					if( GD3D9.Device.DrawSetting.IgnoreGraphColorFlag )
					{
						_MEMCPY( TempTextureStageInfo, TextureStageInfo, sizeof( GRAPHICS_HARDDATA_DIRECT3D9_TEXTURESTAGEINFO ) * ( StageNum + 1 ) ) ;
						if( TempTextureStageInfo[ 0 ].ColorARG1 == D_D3DTA_TEXTURE )
						{
							TempTextureStageInfo[ 0 ].ColorARG1 = D_D3DTA_TFACTOR ;
						}
						else
						if( TempTextureStageInfo[ 0 ].ColorARG1 == ( D_D3DTA_TEXTURE | D_D3DTA_COMPLEMENT ) )
						{
							TempTextureStageInfo[ 0 ].ColorARG1 = D_D3DTA_TFACTOR | D_D3DTA_COMPLEMENT ;
						}
						TextureStageInfo = TempTextureStageInfo ;
					}
				}

				// αテストを使用するかどうかで処理を分岐
				if( GD3D9.Device.DrawSetting.AlphaTestValidFlag == TRUE )
				{
					// αテストを行う(透過色処理以外にαテストはブレンドテクスチャでも使用している)
					BlendInfo.AlphaTestEnable = TRUE ;
					BlendInfo.AlphaRef = 16 ;
					BlendInfo.AlphaFunc = D_D3DCMP_GREATER ;
				}
				else
				{
					// α処理が有効な場合はブレンドモードによってはα値が０の場合のみ表示されないようにする
					if( BlendInfo.AlphaBlendEnable == TRUE &&
						__RGBBlendStateTable[ NextBlendMode ][ 6 ] == 1 )
					{
						BlendInfo.AlphaTestEnable = TRUE ;
						BlendInfo.AlphaRef        = 0 ;
						BlendInfo.AlphaFunc       = D_D3DCMP_GREATER ;
					}
					else
					{
						BlendInfo.AlphaTestEnable = FALSE ;
						BlendInfo.AlphaRef        = GD3D9.Device.State.BlendInfo.AlphaRef ;
						BlendInfo.AlphaFunc       = GD3D9.Device.State.BlendInfo.AlphaFunc ;
					}
				}

				// 使用するテクスチャーアドレスをセット
				StageTexture[0] = GD3D9.Device.DrawSetting.RenderTexture ;
			}
		}

		// αチャンネルを正しく計算するシェーダーを使用する場合はレンダリングターゲットのテクスチャをセットする
/*		if( GRH.UseAlphaChDrawShader )
		{
			StageTexture[ 2 ] = GRH.RenderTargetTexture ;
			if( StageNum < 3 )
			{
				for( i = StageNum ; i < 2 ; i ++ )
				{
					StageTexture[ i ] = NULL ;
				}
				StageNum = 3 ;
			}
		}*/
		
		// 初期化フラグが立っているかどうかで処理を分岐	
		if( StageNum != ( DWORD )GD3D9.Device.State.BlendInfo.UseTextureStageNum )
		{
			int InitFlag ;
			
			GD3D9.Device.State.BlendInfo.UseTextureStageNum = ( int )StageNum ;
			
			InitFlag = GD3D9.Device.DrawSetting.CancelSettingEqualCheck ;

			GD3D9.Device.DrawSetting.CancelSettingEqualCheck = TRUE ;
			Graphics_D3D9_DeviceState_SetDrawMode( GD3D9.Device.State.DrawMode ) ;
			Graphics_D3D9_DeviceState_SetMaxAnisotropy( GD3D9.Device.State.MaxAnisotropy ) ;
			GD3D9.Device.DrawSetting.CancelSettingEqualCheck = InitFlag ;
		}
		if( StageNum < USE_TEXTURESTAGE_NUM ) StageNum ++ ;

		// アルファテストパラメータが有効な場合はそれを優先する
		if( GD3D9.Device.DrawSetting.AlphaTestMode != -1 )
		{
			BlendInfo.AlphaTestEnable = TRUE ;
			BlendInfo.AlphaFunc = GD3D9.Device.DrawSetting.AlphaTestMode ;
			BlendInfo.AlphaRef = GD3D9.Device.DrawSetting.AlphaTestParam ;
		}

		// シャドウマップの設定
		if( GSYS.DrawSetting.UseShadowMapNum > 0 )
		{
			int j ;

			for( j = 0 ; j < MAX_USE_SHADOWMAP_NUM ; j ++ )
			{
				BlendInfo.TextureStageInfo[ 8 + j ].TextureCoordIndex = 0 ;
				BlendInfo.TextureStageInfo[ 8 + j ].ResultTempARG = FALSE ;

				if( !SHADOWMAPCHKFULL( GSYS.DrawSetting.ShadowMap[ j ], ShadowMap ) )
				{
					StageTexture[ 8 + j ] = ShadowMap->PF->D3D9.Texture ;
					Graphics_D3D9_DeviceState_SetTextureAddressUVW( DX_TEXADDRESS_CLAMP, DX_TEXADDRESS_CLAMP, D_D3DTADDRESS_CLAMP, 8 + j ) ;

					Graphics_D3D9_DeviceState_SetSampleFilterMode( 8 + j, D_D3DTSS_MINFILTER, D_D3DTEXF_LINEAR ) ;
					Graphics_D3D9_DeviceState_SetSampleFilterMode( 8 + j, D_D3DTSS_MAGFILTER, D_D3DTEXF_LINEAR ) ;
					Graphics_D3D9_DeviceState_SetSampleFilterMode( 8 + j, D_D3DTSS_MIPFILTER, D_D3DTEXF_LINEAR ) ;

					if( StageNum < ( DWORD )( 9 + j ) )
					{
						StageNum = ( DWORD )( 9 + j ) ;
					}
				}
				else
				{
					StageTexture[ 8 + j ] = NULL ;
				}
			}
		}
	}

	if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == TRUE )
	{
		// 立っている場合は状態を全て更新する

		Direct3DDevice9_SetRenderState( D_D3DRS_ALPHAFUNC, ( DWORD )BlendInfo.AlphaFunc ) ;
		GD3D9.Device.State.BlendInfo.AlphaFunc = BlendInfo.AlphaFunc ;

		Direct3DDevice9_SetRenderState( D_D3DRS_ALPHAREF, ( DWORD )BlendInfo.AlphaRef ) ;
		GD3D9.Device.State.BlendInfo.AlphaRef = BlendInfo.AlphaRef ;
		
		Direct3DDevice9_SetRenderState( D_D3DRS_ALPHATESTENABLE, ( DWORD )BlendInfo.AlphaTestEnable ) ;
		GD3D9.Device.State.BlendInfo.AlphaTestEnable = BlendInfo.AlphaTestEnable ;

		Direct3DDevice9_SetRenderState( D_D3DRS_ALPHABLENDENABLE, ( DWORD )BlendInfo.AlphaBlendEnable ) ;
		GD3D9.Device.State.BlendInfo.AlphaBlendEnable = BlendInfo.AlphaBlendEnable ;
			
		if( UseShaderFlag == FALSE )
		{
			Direct3DDevice9_SetRenderState( D_D3DRS_TEXTUREFACTOR, BlendInfo.FactorColor ) ;
		}
		GD3D9.Device.State.BlendInfo.FactorColor = BlendInfo.FactorColor ;

		{
			if( UseFloatFactorColor )
			{
				*( ( FLOAT4 * )GD3D9.Device.State.FactorColorPSConstantF ) = FloatFactorColor ;
			}
			else
			{
				GD3D9.Device.State.FactorColorPSConstantF[ 0 ] = (   BlendInfo.FactorColor         & 0xff ) / 255.0f ;
				GD3D9.Device.State.FactorColorPSConstantF[ 1 ] = ( ( BlendInfo.FactorColor >> 8  ) & 0xff ) / 255.0f ;
				GD3D9.Device.State.FactorColorPSConstantF[ 2 ] = ( ( BlendInfo.FactorColor >> 16 ) & 0xff ) / 255.0f ;
				GD3D9.Device.State.FactorColorPSConstantF[ 3 ] = ( ( BlendInfo.FactorColor >> 24 ) & 0xff ) / 255.0f ;
			}
			Graphics_D3D9_ShaderConstant_InfoSet_SetParam(
				&GD3D9.Device.Shader.ShaderConstantInfo,
				DX_SHADERCONSTANTTYPE_PS_FLOAT,
				DX_SHADERCONSTANTSET_LIB,
				DX_PS_CONSTF_FACTORCOLOR,
				GD3D9.Device.State.FactorColorPSConstantF,
				1,
				TRUE
			) ;
		}

		if( BlendInfo.SrcBlend != -1 )
		{
			Direct3DDevice9_SetRenderState( D_D3DRS_SRCBLEND, ( DWORD )BlendInfo.SrcBlend ) ;
			GD3D9.Device.State.BlendInfo.SrcBlend = BlendInfo.SrcBlend ;
		}
		else
		if( GD3D9.Device.State.BlendInfo.SrcBlend > 1 )
		{
			Direct3DDevice9_SetRenderState( D_D3DRS_SRCBLEND, ( DWORD )GD3D9.Device.State.BlendInfo.SrcBlend ) ;
		}

		if( BlendInfo.DestBlend != -1 )
		{
			Direct3DDevice9_SetRenderState( D_D3DRS_DESTBLEND, ( DWORD )BlendInfo.DestBlend ) ;
			GD3D9.Device.State.BlendInfo.DestBlend = BlendInfo.DestBlend ;
		}
		else
		if( GD3D9.Device.State.BlendInfo.DestBlend > 1 )
		{
			Direct3DDevice9_SetRenderState( D_D3DRS_DESTBLEND, ( DWORD )GD3D9.Device.State.BlendInfo.DestBlend ) ;
		}

		if( BlendInfo.BlendOp != -1 )
		{
			Direct3DDevice9_SetRenderState( D_D3DRS_BLENDOP, ( DWORD )BlendInfo.BlendOp ) ;
			GD3D9.Device.State.BlendInfo.BlendOp = BlendInfo.BlendOp ;
		}
		else
		if( GD3D9.Device.State.BlendInfo.BlendOp > 1 )
		{
			Direct3DDevice9_SetRenderState( D_D3DRS_BLENDOP, ( DWORD )GD3D9.Device.State.BlendInfo.BlendOp ) ;
		}

		Direct3DDevice9_SetRenderState( D_D3DRS_SEPARATEALPHABLENDENABLE, ( DWORD )BlendInfo.SeparateAlphaBlendEnable ) ;
		GD3D9.Device.State.BlendInfo.SeparateAlphaBlendEnable = BlendInfo.SeparateAlphaBlendEnable ;

		if( BlendInfo.SrcBlendAlpha != -1 )
		{
			Direct3DDevice9_SetRenderState( D_D3DRS_SRCBLENDALPHA, ( DWORD )BlendInfo.SrcBlendAlpha ) ;
			GD3D9.Device.State.BlendInfo.SrcBlendAlpha = BlendInfo.SrcBlendAlpha ;
		}
		else
		if( GD3D9.Device.State.BlendInfo.SrcBlendAlpha > 1 )
		{
			Direct3DDevice9_SetRenderState( D_D3DRS_SRCBLENDALPHA, ( DWORD )GD3D9.Device.State.BlendInfo.SrcBlendAlpha ) ;
		}

		if( BlendInfo.DestBlendAlpha != -1 )
		{
			Direct3DDevice9_SetRenderState( D_D3DRS_DESTBLENDALPHA, ( DWORD )BlendInfo.DestBlendAlpha ) ;
			GD3D9.Device.State.BlendInfo.DestBlendAlpha = BlendInfo.DestBlendAlpha ;
		}
		else
		if( GD3D9.Device.State.BlendInfo.DestBlendAlpha > 1 )
		{
			Direct3DDevice9_SetRenderState( D_D3DRS_DESTBLENDALPHA, ( DWORD )GD3D9.Device.State.BlendInfo.DestBlendAlpha ) ;
		}

		if( BlendInfo.BlendOpAlpha != -1 )
		{
			Direct3DDevice9_SetRenderState( D_D3DRS_BLENDOPALPHA, ( DWORD )BlendInfo.BlendOpAlpha ) ;
			GD3D9.Device.State.BlendInfo.BlendOpAlpha = BlendInfo.BlendOpAlpha ;
		}
		else
		if( GD3D9.Device.State.BlendInfo.BlendOpAlpha > 1 )
		{
			Direct3DDevice9_SetRenderState( D_D3DRS_BLENDOPALPHA, ( DWORD )GD3D9.Device.State.BlendInfo.BlendOpAlpha ) ;
		}

		if( UseShaderFlag )
		{
			Src = NULL ;
		}
		else
		{
			Src  = TextureStageInfo ;
		}
		Dest = GD3D9.Device.State.BlendInfo.TextureStageInfo ;
		for( i = 0 ; i < USE_TEXTURESTAGE_NUM ; i ++, Src ++, Dest ++ )
		{
//			if( i >= 2 && Src[-1].AlphaOP == D_D3DTOP_DISABLE && TextureOnly == FALSE ) break ;

			if( i < StageNum )
			{
				Direct3DDevice9_SetTexture( i, StageTexture[i] ) ;
				Dest->Texture = StageTexture[i] ;

				if( i < 4 )
				{
					Direct3DDevice9_SetTexture( D_D3DDMAPSAMPLER + 1 + i, StageTexture[i] ) ;
				}

				if( UseShaderFlag == FALSE )
				{
					if( Src->TextureCoordIndex != -1 && i < ( DWORD )GD3D9.Device.Caps.MaxTextureBlendStages )
					{
						Direct3DDevice9_SetTextureStageState( i, D_D3DTSS_TEXCOORDINDEX, ( DWORD )Src->TextureCoordIndex ) ;
						Dest->TextureCoordIndex = Src->TextureCoordIndex ;
					}
				}
			}
			else
			{
				Direct3DDevice9_SetTexture( i, NULL ) ;
				Dest->Texture = NULL ;

				if( i < 4 )
				{
					Direct3DDevice9_SetTexture( D_D3DDMAPSAMPLER + 1 + i, NULL ) ;
				}

				if( UseShaderFlag == FALSE && i < ( DWORD )GD3D9.Device.Caps.MaxTextureBlendStages )
				{
					Direct3DDevice9_SetTextureStageState( i, D_D3DTSS_TEXCOORDINDEX, 0 ) ;
					Dest->TextureCoordIndex = 0 ;
				}
			}

			if( TextureOnly || ( UseShaderFlag && i != 0 ) ) continue ;

			if( UseShaderFlag )
			{
				Graphics_D3D9_DeviceState_SetPixelShader( UsePixelShader, TRUE ) ;
			}
			else
			{
				Graphics_D3D9_DeviceState_ResetPixelShader( FALSE ) ;

				if( i < StageNum && i < ( DWORD )GD3D9.Device.Caps.MaxTextureBlendStages )
				{
					if( GD3D9.Device.Caps.ValidTexTempRegFlag )
					{
						if( Src->ResultTempARG != -1 )
						{
							Direct3DDevice9_SetTextureStageState( i, D_D3DTSS_RESULTARG, ( DWORD )( Src->ResultTempARG ? D_D3DTA_TEMP : D_D3DTA_CURRENT ) ) ;
							Dest->ResultTempARG = Src->ResultTempARG ;
						}
					}

					if( Src->AlphaOP != -1 )
					{
						Direct3DDevice9_SetTextureStageState( i, D_D3DTSS_ALPHAOP, ( DWORD )Src->AlphaOP ) ;
						Dest->AlphaOP = Src->AlphaOP ;
					}

					if( Src->AlphaARG1 != -1 )
					{
						Direct3DDevice9_SetTextureStageState( i, D_D3DTSS_ALPHAARG1, ( DWORD )Src->AlphaARG1 ) ;
						Dest->AlphaARG1 = Src->AlphaARG1 ;
					}

					if( Src->AlphaARG2 != -1 )
					{
						Direct3DDevice9_SetTextureStageState( i, D_D3DTSS_ALPHAARG2, ( DWORD )Src->AlphaARG2 ) ;
						Dest->AlphaARG2 = Src->AlphaARG2 ;
					}

					if( Src->ColorOP != -1 )
					{
						Direct3DDevice9_SetTextureStageState( i, D_D3DTSS_COLOROP, ( DWORD )Src->ColorOP ) ;
						Dest->ColorOP = Src->ColorOP ;
					}

					if( Src->ColorARG1 != -1 )
					{
						Direct3DDevice9_SetTextureStageState( i, D_D3DTSS_COLORARG1, ( DWORD )Src->ColorARG1 ) ;
						Dest->ColorARG1 = Src->ColorARG1 ;
					}

					if( Src->ColorARG2 != -1 )
					{
						Direct3DDevice9_SetTextureStageState( i, D_D3DTSS_COLORARG2, ( DWORD )Src->ColorARG2 ) ;
						Dest->ColorARG2 = Src->ColorARG2 ;
					}
				}
				else
				{
					if( GD3D9.Device.Caps.ValidTexTempRegFlag )
					{
						Direct3DDevice9_SetTextureStageState( i, D_D3DTSS_RESULTARG, D_D3DTA_CURRENT ) ;
						Dest->ResultTempARG = FALSE ;
					}

					Direct3DDevice9_SetTextureStageState( i, D_D3DTSS_ALPHAOP, D_D3DTOP_DISABLE ) ;
					Dest->AlphaOP = D_D3DTOP_DISABLE ;

					Direct3DDevice9_SetTextureStageState( i, D_D3DTSS_ALPHAARG1, D_D3DTA_DIFFUSE ) ;
					Dest->AlphaARG1 = D_D3DTA_DIFFUSE ;

					Direct3DDevice9_SetTextureStageState( i, D_D3DTSS_ALPHAARG2, D_D3DTA_DIFFUSE ) ;
					Dest->AlphaARG2 = D_D3DTA_DIFFUSE ;

					Direct3DDevice9_SetTextureStageState( i, D_D3DTSS_COLOROP, D_D3DTOP_DISABLE ) ;
					Dest->ColorOP = D_D3DTOP_DISABLE ;

					Direct3DDevice9_SetTextureStageState( i, D_D3DTSS_COLORARG1, D_D3DTA_DIFFUSE ) ;
					Dest->ColorARG1 = D_D3DTA_DIFFUSE ;

					Direct3DDevice9_SetTextureStageState( i, D_D3DTSS_COLORARG2, D_D3DTA_DIFFUSE ) ;
					Dest->ColorARG2 = D_D3DTA_DIFFUSE ;
				}
			}
		}
	}
	else
	{
		// 立っていない場合は変更点のみ再設定する

		if( BlendInfo.AlphaFunc != GD3D9.Device.State.BlendInfo.AlphaFunc )
		{
			Direct3DDevice9_SetRenderState( D_D3DRS_ALPHAFUNC, ( DWORD )BlendInfo.AlphaFunc ) ;
			GD3D9.Device.State.BlendInfo.AlphaFunc = BlendInfo.AlphaFunc ;
		}

		if( BlendInfo.AlphaRef != GD3D9.Device.State.BlendInfo.AlphaRef )
		{
			Direct3DDevice9_SetRenderState( D_D3DRS_ALPHAREF, ( DWORD )BlendInfo.AlphaRef ) ;
			GD3D9.Device.State.BlendInfo.AlphaRef = BlendInfo.AlphaRef ;
		}
		
		if( BlendInfo.AlphaTestEnable != GD3D9.Device.State.BlendInfo.AlphaTestEnable )
		{
			Direct3DDevice9_SetRenderState( D_D3DRS_ALPHATESTENABLE, ( DWORD )BlendInfo.AlphaTestEnable ) ;
			GD3D9.Device.State.BlendInfo.AlphaTestEnable = BlendInfo.AlphaTestEnable ;
		}
		
		if( BlendInfo.AlphaBlendEnable != GD3D9.Device.State.BlendInfo.AlphaBlendEnable )
		{
			Direct3DDevice9_SetRenderState( D_D3DRS_ALPHABLENDENABLE, ( DWORD )BlendInfo.AlphaBlendEnable ) ;
			GD3D9.Device.State.BlendInfo.AlphaBlendEnable = BlendInfo.AlphaBlendEnable ;
		}
			
		if( BlendInfo.FactorColor != GD3D9.Device.State.BlendInfo.FactorColor )
		{
			if( UseShaderFlag == FALSE )
			{
				Direct3DDevice9_SetRenderState( D_D3DRS_TEXTUREFACTOR, BlendInfo.FactorColor ) ;
			}
			GD3D9.Device.State.BlendInfo.FactorColor = BlendInfo.FactorColor ;
		}

		{
			float FactorData[ 4 ] ;

			if( UseFloatFactorColor )
			{
				*( ( FLOAT4 * )FactorData ) = FloatFactorColor ;
			}
			else
			{
				FactorData[ 0 ] = (   BlendInfo.FactorColor         & 0xff ) / 255.0f ;
				FactorData[ 1 ] = ( ( BlendInfo.FactorColor >> 8  ) & 0xff ) / 255.0f ;
				FactorData[ 2 ] = ( ( BlendInfo.FactorColor >> 16 ) & 0xff ) / 255.0f ;
				FactorData[ 3 ] = ( ( BlendInfo.FactorColor >> 24 ) & 0xff ) / 255.0f ;
			}
			if( _MEMCMP( GD3D9.Device.State.FactorColorPSConstantF, FactorData, sizeof( float ) * 4 ) != 0 )
			{
				_MEMCPY( GD3D9.Device.State.FactorColorPSConstantF, FactorData, sizeof( float ) * 4 ) ;
				Graphics_D3D9_ShaderConstant_InfoSet_SetParam(
					&GD3D9.Device.Shader.ShaderConstantInfo,
					DX_SHADERCONSTANTTYPE_PS_FLOAT,
					DX_SHADERCONSTANTSET_LIB,
					DX_PS_CONSTF_FACTORCOLOR,
					FactorData,
					1,
					TRUE
				) ;
			}
		}

		if( BlendInfo.SrcBlend != -1 && BlendInfo.SrcBlend != GD3D9.Device.State.BlendInfo.SrcBlend )
		{
			Direct3DDevice9_SetRenderState( D_D3DRS_SRCBLEND, ( DWORD )BlendInfo.SrcBlend ) ;
			GD3D9.Device.State.BlendInfo.SrcBlend = BlendInfo.SrcBlend ;
		}

		if( BlendInfo.DestBlend != -1 && BlendInfo.DestBlend != GD3D9.Device.State.BlendInfo.DestBlend )
		{
			Direct3DDevice9_SetRenderState( D_D3DRS_DESTBLEND, ( DWORD )BlendInfo.DestBlend ) ;
			GD3D9.Device.State.BlendInfo.DestBlend = BlendInfo.DestBlend ;
		}

		if( BlendInfo.BlendOp != -1 && BlendInfo.BlendOp != GD3D9.Device.State.BlendInfo.BlendOp )
		{
			Direct3DDevice9_SetRenderState( D_D3DRS_BLENDOP, ( DWORD )BlendInfo.BlendOp ) ;
			GD3D9.Device.State.BlendInfo.BlendOp = BlendInfo.BlendOp ;
		}

		if( BlendInfo.SeparateAlphaBlendEnable != GD3D9.Device.State.BlendInfo.SeparateAlphaBlendEnable )
		{
			Direct3DDevice9_SetRenderState( D_D3DRS_SEPARATEALPHABLENDENABLE, ( DWORD )BlendInfo.SeparateAlphaBlendEnable ) ;
			GD3D9.Device.State.BlendInfo.SeparateAlphaBlendEnable = BlendInfo.SeparateAlphaBlendEnable ;
		}

		if( BlendInfo.SrcBlendAlpha != -1 && BlendInfo.SrcBlendAlpha != GD3D9.Device.State.BlendInfo.SrcBlendAlpha )
		{
			Direct3DDevice9_SetRenderState( D_D3DRS_SRCBLENDALPHA, ( DWORD )BlendInfo.SrcBlendAlpha ) ;
			GD3D9.Device.State.BlendInfo.SrcBlendAlpha = BlendInfo.SrcBlendAlpha ;
		}

		if( BlendInfo.DestBlendAlpha != -1 && BlendInfo.DestBlendAlpha != GD3D9.Device.State.BlendInfo.DestBlendAlpha )
		{
			Direct3DDevice9_SetRenderState( D_D3DRS_DESTBLENDALPHA, ( DWORD )BlendInfo.DestBlendAlpha ) ;
			GD3D9.Device.State.BlendInfo.DestBlendAlpha = BlendInfo.DestBlendAlpha ;
		}

		if( BlendInfo.BlendOpAlpha != -1 && BlendInfo.BlendOpAlpha != GD3D9.Device.State.BlendInfo.BlendOpAlpha )
		{
			Direct3DDevice9_SetRenderState( D_D3DRS_BLENDOPALPHA, ( DWORD )BlendInfo.BlendOpAlpha ) ;
			GD3D9.Device.State.BlendInfo.BlendOpAlpha = BlendInfo.BlendOpAlpha ;
		}


		if( UseShaderFlag )
		{
			Src = NULL ;
		}
		else
		{
			Src  = TextureStageInfo ;
		}
		DWORD UseShaderStageNum ;
		if( GSYS.DrawSetting.UseShadowMapNum )
		{
			UseShaderStageNum = StageNum ;
		}
		else
		{
			UseShaderStageNum = /* GRH.UseAlphaChDrawShader ? 3 : */ 2 ;
		}
		Dest = GD3D9.Device.State.BlendInfo.TextureStageInfo ;
		for( i = 0 ; i < StageNum ; i ++, Src ++, Dest ++ )
		{
			if( i >= UseShaderStageNum && ( UseShaderFlag || ( i > 0 && Src[-1].AlphaOP == D_D3DTOP_DISABLE ) ) && TextureOnly == FALSE ) break ;

			if( StageTexture[i] != Dest->Texture )
			{
				HRESULT hr ;

				hr = Direct3DDevice9_SetTexture( i, StageTexture[i] ) ;
				Dest->Texture = StageTexture[i] ;

				if( i < 4 )
				{
					Direct3DDevice9_SetTexture( D_D3DDMAPSAMPLER + 1 + i, StageTexture[i] ) ;
				}
			}

			if( UseShaderFlag == FALSE )
			{
				if( Src->TextureCoordIndex != -1 &&
					Src->TextureCoordIndex != Dest->TextureCoordIndex &&
					i < ( DWORD )GD3D9.Device.Caps.MaxTextureBlendStages )
				{
					Direct3DDevice9_SetTextureStageState( i, D_D3DTSS_TEXCOORDINDEX, ( DWORD )Src->TextureCoordIndex ) ;
					Dest->TextureCoordIndex = Src->TextureCoordIndex ;
				}
			}

			if( TextureOnly || ( UseShaderFlag && i != 0 ) ) continue ;

			if( UseShaderFlag )
			{
				Graphics_D3D9_DeviceState_SetPixelShader( UsePixelShader, TRUE ) ;
			}
			else
			{
				Graphics_D3D9_DeviceState_ResetPixelShader( FALSE ) ;

				if( i < ( DWORD )GD3D9.Device.Caps.MaxTextureBlendStages )
				{
					if( GD3D9.Device.Caps.ValidTexTempRegFlag )
					{
						if( Src->ResultTempARG != -1 && Src->ResultTempARG != Dest->ResultTempARG )
						{
							Direct3DDevice9_SetTextureStageState( i, D_D3DTSS_RESULTARG, ( DWORD )( Src->ResultTempARG ? D_D3DTA_TEMP : D_D3DTA_CURRENT ) ) ;
							Dest->ResultTempARG = Src->ResultTempARG ;
						}
					}

					if( Src->AlphaOP != -1 && Src->AlphaOP != Dest->AlphaOP )
					{
						Direct3DDevice9_SetTextureStageState( i, D_D3DTSS_ALPHAOP, ( DWORD )Src->AlphaOP ) ;
						Dest->AlphaOP = Src->AlphaOP ;
					}

					if( Src->AlphaARG1 != -1 && Src->AlphaARG1 != Dest->AlphaARG1 )
					{
						Direct3DDevice9_SetTextureStageState( i, D_D3DTSS_ALPHAARG1, ( DWORD )Src->AlphaARG1 ) ;
						Dest->AlphaARG1 = Src->AlphaARG1 ;
					}

					if( Src->AlphaARG2 != -1 && Src->AlphaARG2 != Dest->AlphaARG2 )
					{
						Direct3DDevice9_SetTextureStageState( i, D_D3DTSS_ALPHAARG2, ( DWORD )Src->AlphaARG2 ) ;
						Dest->AlphaARG2 = Src->AlphaARG2 ;
					}

					if( Src->ColorOP != -1 && Src->ColorOP != Dest->ColorOP )
					{
						Direct3DDevice9_SetTextureStageState( i, D_D3DTSS_COLOROP, ( DWORD )Src->ColorOP ) ;
						Dest->ColorOP = Src->ColorOP ;
					}

					if( Src->ColorARG1 != -1 && Src->ColorARG1 != Dest->ColorARG1 )
					{
						Direct3DDevice9_SetTextureStageState( i, D_D3DTSS_COLORARG1, ( DWORD )Src->ColorARG1 ) ;
						Dest->ColorARG1 = Src->ColorARG1 ;
					}

					if( Src->ColorARG2 != -1 && Src->ColorARG2 != Dest->ColorARG2 )
					{
						Direct3DDevice9_SetTextureStageState( i, D_D3DTSS_COLORARG2, ( DWORD )Src->ColorARG2 ) ;
						Dest->ColorARG2 = Src->ColorARG2 ;
					}
				}
			}
		}
		for( ; i < USE_TEXTURESTAGE_NUM ; i ++, Dest ++ )
		{
			if( Dest->Texture != NULL )
			{
				Direct3DDevice9_SetTexture( i, NULL ) ;
				Dest->Texture = NULL ;
			}

			if( i < 4 )
			{
				Direct3DDevice9_SetTexture( D_D3DDMAPSAMPLER + 1 + i, NULL ) ;
			}
		}
	}
	
	// 終了
	return 0 ;
}

// アンビエントライトとエミッシブカラーを掛け合わせたパラメータを更新する
extern	void	Graphics_D3D9_DeviceState_RefreshAmbientAndEmissiveParam( void )
{
	int i ;
	COLOR_F Result ;

	// 結果値を初期化
	Result.r = 0.0f ;
	Result.g = 0.0f ;
	Result.b = 0.0f ;
	Result.a = 0.0f ;

	// 有効なライトのアンビエントカラーとマテリアルのアンビエントカラーを乗算したものをシェーダーにセットする
	for( i = 0 ; i <= GD3D9.Device.State.LightEnableMaxIndex ; i ++ )
	{
		if( GD3D9.Device.State.LightEnableFlag[ i ] == FALSE ) continue ;
		Result.r = GD3D9.Device.State.LightParam[ i ].Ambient.r * GD3D9.Device.State.Material.Ambient.r ;
		Result.g = GD3D9.Device.State.LightParam[ i ].Ambient.g * GD3D9.Device.State.Material.Ambient.g ;
		Result.b = GD3D9.Device.State.LightParam[ i ].Ambient.b * GD3D9.Device.State.Material.Ambient.b ;
		Result.a = GD3D9.Device.State.LightParam[ i ].Ambient.a * GD3D9.Device.State.Material.Ambient.a ;

		// バーテックスシェーダーとピクセルシェーダーに反映する
		if( i < DX_VS_CONSTB_LIGHT_NUM )
		{
			Graphics_D3D9_ShaderConstant_InfoSet_SetParam(
				&GD3D9.Device.Shader.ShaderConstantInfo,
				DX_SHADERCONSTANTTYPE_VS_FLOAT,
				DX_SHADERCONSTANTSET_LIB,
				DX_VS_CONSTF_LIGHT_START + DX_VS_CONSTF_LIGHT_UNITSIZE * i + DX_VS_CONSTF_LGT_AMBIENT,
				&Result,
				1,
				TRUE
			) ;
			Graphics_D3D9_ShaderConstant_InfoSet_SetParam(
				&GD3D9.Device.Shader.ShaderConstantInfo,
				DX_SHADERCONSTANTTYPE_PS_FLOAT,
				DX_SHADERCONSTANTSET_LIB,
				DX_PS_CONSTF_LIGHT_START  + DX_PS_CONSTF_LIGHT_UNITSIZE  * i + DX_PS_CONSTF_LGT_AMBIENT,
				&Result,
				1,
				TRUE
			) ;
		}
		if( i < DX_PS_CONSTB_LIGHT2_NUM )
		{
			Graphics_D3D9_ShaderConstant_InfoSet_SetParam(
				&GD3D9.Device.Shader.ShaderConstantInfo,
				DX_SHADERCONSTANTTYPE_PS_FLOAT,
				DX_SHADERCONSTANTSET_LIB,
				DX_PS_CONSTF_LIGHT2_START  + DX_PS_CONSTF_LIGHT2_UNITSIZE  * i + DX_PS_CONSTF_LGT2_AMBIENT,
				&Result,
				1,
				TRUE
			) ;
		}
	}

	// グローバルアンビエントライトカラーとマテリアルアンビエントカラーを乗算したものとエミッシブカラーを加算したものを作成する
	Result.r = GD3D9.Device.State.GlobalAmbientLightColor.r * GD3D9.Device.State.Material.Ambient.r + GD3D9.Device.State.Material.Emissive.r ;
	Result.g = GD3D9.Device.State.GlobalAmbientLightColor.g * GD3D9.Device.State.Material.Ambient.g + GD3D9.Device.State.Material.Emissive.g ;
	Result.b = GD3D9.Device.State.GlobalAmbientLightColor.b * GD3D9.Device.State.Material.Ambient.b + GD3D9.Device.State.Material.Emissive.b ;
	Result.a = GD3D9.Device.State.GlobalAmbientLightColor.a * GD3D9.Device.State.Material.Ambient.a + GD3D9.Device.State.Material.Emissive.a ;

	// バーテックスシェーダーとピクセルシェーダーに反映する
	Graphics_D3D9_ShaderConstant_InfoSet_SetParam(
		&GD3D9.Device.Shader.ShaderConstantInfo,
		DX_SHADERCONSTANTTYPE_VS_FLOAT,
		DX_SHADERCONSTANTSET_LIB,
		DX_VS_CONSTF_AMBIENT_EMISSIVE,
		&Result,
		1,
		TRUE
	) ;
	Graphics_D3D9_ShaderConstant_InfoSet_SetParam(
		&GD3D9.Device.Shader.ShaderConstantInfo,
		DX_SHADERCONSTANTTYPE_PS_FLOAT,
		DX_SHADERCONSTANTSET_LIB,
		DX_PS_CONSTF_AMBIENT_EMISSIVE,
		&Result,
		1,
		TRUE
	) ;
}
























// 描画設定関係関数

// 描画ブレンドモードの設定
extern	int		Graphics_D3D9_DrawSetting_SetDrawBlendMode( int BlendMode, int AlphaTestValidFlag, int AlphaChannelValidFlag )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE &&
		GD3D9.Device.DrawSetting.BlendMode == BlendMode &&
		GD3D9.Device.DrawSetting.AlphaTestValidFlag == AlphaTestValidFlag &&
		GD3D9.Device.DrawSetting.AlphaChannelValidFlag == AlphaChannelValidFlag ) return 0 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	GD3D9.Device.DrawSetting.BlendMode             = BlendMode ;
	GD3D9.Device.DrawSetting.AlphaChannelValidFlag = AlphaChannelValidFlag ;
	GD3D9.Device.DrawSetting.AlphaTestValidFlag    = AlphaTestValidFlag ;
	GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag    = TRUE ;

	// パラメータが変更された、フラグを立てる
	GD3D9.Device.DrawSetting.ChangeBlendParamFlag = TRUE ;

	return 0 ;
}

// 描画時の画像のＲＧＢを無視するかどうかを設定する
extern	int		Graphics_D3D9_DrawSetting_SetIgnoreDrawGraphColor( int EnableFlag )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE &&
		GD3D9.Device.DrawSetting.IgnoreGraphColorFlag == EnableFlag )
		return 0 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	GD3D9.Device.DrawSetting.IgnoreGraphColorFlag = EnableFlag ;
	GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// パラメータが変更された、フラグを立てる
	GD3D9.Device.DrawSetting.ChangeBlendParamFlag = TRUE ;

	return 0 ;
}

// アルファテストの設定
extern	int		Graphics_D3D9_DrawSetting_SetDrawAlphaTest( int TestMode, int TestParam )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE && GD3D9.Device.DrawSetting.AlphaTestMode == TestMode && GD3D9.Device.DrawSetting.AlphaTestParam == TestParam ) return 0 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	GD3D9.Device.DrawSetting.AlphaTestMode  = TestMode ;
	GD3D9.Device.DrawSetting.AlphaTestParam = TestParam ;
	GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// パラメータが変更された、フラグを立てる
	GD3D9.Device.DrawSetting.ChangeBlendParamFlag = TRUE ;

	return 0 ;
}

// ディフューズカラーのα値を使用するかどうかフラグをセットする
extern	int		Graphics_D3D9_DrawSetting_SetUseDiffuseAlphaColorFlag( int UseFlag )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	if( UseFlag == GD3D9.Device.DrawSetting.UseDiffuseAlphaColor && GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE ) return 0 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// パラメータの保存
	GD3D9.Device.DrawSetting.UseDiffuseAlphaColor = UseFlag ;
	GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// パラメータが変更された、フラグを立てる
	GD3D9.Device.DrawSetting.ChangeBlendParamFlag = TRUE ;

	// 終了
	return 0 ;
}

// ディフューズカラーのＲＧＢ値を使用するかどうかフラグをセットする
extern	int		Graphics_D3D9_DrawSetting_SetUseDiffuseRGBColorFlag( int UseFlag )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	if( UseFlag == GD3D9.Device.DrawSetting.UseDiffuseRGBColor && GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE ) return 0 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// パラメータの保存
	GD3D9.Device.DrawSetting.UseDiffuseRGBColor = UseFlag ;
	GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// パラメータが変更された、フラグを立てる
	GD3D9.Device.DrawSetting.ChangeBlendParamFlag = TRUE ;

	// 終了
	return 0 ;
}

// ブレンドするテクスチャのパラメータをセットする
extern	int		Graphics_D3D9_DrawSetting_SetBlendTextureParam( int BlendType, int *Param )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	// パラメータを保存
	switch( BlendType )
	{
	case DX_BLENDGRAPHTYPE_NORMAL :
		if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE &&
			BlendType == GD3D9.Device.DrawSetting.BlendGraphType &&
			GD3D9.Device.DrawSetting.BlendGraphFadeRatio == Param[ 0 ] )
			return 0 ;

		if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
		{
			// 描画待機している描画物を描画
			DRAWSTOCKINFO
		}

		GD3D9.Device.DrawSetting.BlendGraphFadeRatio = Param[ 0 ] ;
		break;

	case DX_BLENDGRAPHTYPE_WIPE:
		if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE &&
			BlendType == GD3D9.Device.DrawSetting.BlendGraphType &&
			GD3D9.Device.DrawSetting.BlendGraphBorderParam == Param[ 0 ] &&
			GD3D9.Device.DrawSetting.BlendGraphBorderRange == Param[ 1 ] )
			return 0 ;

		if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
		{
			// 描画待機している描画物を描画
			DRAWSTOCKINFO
		}

		GD3D9.Device.DrawSetting.BlendGraphBorderParam = Param[ 0 ] ;
		GD3D9.Device.DrawSetting.BlendGraphBorderRange = Param[ 1 ] ;
		break;

	case DX_BLENDGRAPHTYPE_ALPHA :
		if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE &&
			BlendType == GD3D9.Device.DrawSetting.BlendGraphType )
			return 0 ;

		if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
		{
			// 描画待機している描画物を描画
			DRAWSTOCKINFO
		}
		break;
	}

	// ブレンドタイプを保存する
	GD3D9.Device.DrawSetting.BlendGraphType = BlendType ;

	// パラメータが変更された、フラグを立てる
	GD3D9.Device.DrawSetting.ChangeBlendParamFlag = TRUE ;
	GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// 終了
	return 0 ;
}

// テクスチャをセットする 
extern	int		Graphics_D3D9_DrawSetting_SetTexture( D_IDirect3DTexture9 *RenderTexture )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	if( RenderTexture == GD3D9.Device.DrawSetting.RenderTexture && GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE ) return 0 ;

	if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		// 描画待機している描画物を描画
		DRAWSTOCKINFO
	}

	// テクスチャが変更された、フラグを立てる
	GD3D9.Device.DrawSetting.ChangeTextureFlag = TRUE ;

	// テクスチャの有り無しが変更された場合はブレンドパラメータの変更も行うフラグもセットする
	if( ( GD3D9.Device.DrawSetting.RenderTexture == NULL && RenderTexture != NULL ) ||
		( GD3D9.Device.DrawSetting.RenderTexture != NULL && RenderTexture == NULL ) )
	{
		GD3D9.Device.DrawSetting.ChangeBlendParamFlag = TRUE ;
	}

	// テクスチャセット
	GD3D9.Device.DrawSetting.RenderTexture = RenderTexture ;
	GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// 終了
	return 0 ;
}

// ブレンドするテクスチャをセットする 
extern	int		Graphics_D3D9_DrawSetting_SetBlendTexture( D_IDirect3DTexture9 *BlendTexture, int TexWidth, int TexHeight )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE && BlendTexture == GD3D9.Device.DrawSetting.BlendTexture ) return 0 ;

	if( GD3D9.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		// 描画待機している描画物を描画
		DRAWSTOCKINFO
	}

	// テクスチャが変更された、フラグを立てる
	GD3D9.Device.DrawSetting.ChangeTextureFlag = TRUE ;
	GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// ブレンドテクスチャの有り無しが変更された場合はブレンドパラメータの変更も行うフラグもセットする
	if( ( GD3D9.Device.DrawSetting.BlendTexture == NULL && BlendTexture != NULL ) ||
		( GD3D9.Device.DrawSetting.BlendTexture != NULL && BlendTexture == NULL ) )
		GD3D9.Device.DrawSetting.ChangeBlendParamFlag = TRUE ;

	// テクスチャセット
	GD3D9.Device.DrawSetting.BlendTexture = BlendTexture ;

	// テクスチャの幅と高さを保存
	if( BlendTexture != NULL )
	{
		GD3D9.Device.DrawSetting.BlendTextureWidth  = ( float )TexWidth  ;
		GD3D9.Device.DrawSetting.BlendTextureHeight = ( float )TexHeight ;
		GD3D9.Device.DrawSetting.InvBlendTextureWidth  = 1.0F / GD3D9.Device.DrawSetting.BlendTextureWidth ;
		GD3D9.Device.DrawSetting.InvBlendTextureHeight = 1.0F / GD3D9.Device.DrawSetting.BlendTextureHeight ;
	}

	// 終了
	return 0 ;
}

























// シェーダー定数管理構造体処理関数

// シェーダー定数使用領域情報を初期化する
extern int Graphics_D3D9_ShaderConstant_UseArea_Initialize( DIRECT3D9_SHADERCONSTANT_USEAREA *UseArea, int TotalSize )
{
	_MEMSET( UseArea, 0, sizeof( DIRECT3D9_SHADERCONSTANT_USEAREA ) ) ;

	// 管理する領域のサイズを設定
	UseArea->TotalSize = TotalSize ;

	// 終了
	return 0 ;
}

// シェーダー定数使用領域情報を変更する
extern int Graphics_D3D9_ShaderConstant_UseArea_Set( DIRECT3D9_SHADERCONSTANT_USEAREA *UseArea, int IsUse, int Index, int Num )
{
	DIRECT3D9_SHADERCONST_ONE_USEAREA *OneArea ;
	int i ;
	int j ;
	int StartIndex ;
	int EndPlusOneIndex ;
	int Bottom ;
	int NoHit ;

	// 変更個数が０だったら何もしない
	if( Num == 0 )
		return 0 ;

	StartIndex = Index ;
	EndPlusOneIndex = Index + Num ;

	// 全く引っかかってないかを調べる
	Bottom = TRUE ;
	NoHit = FALSE ;
	OneArea = UseArea->AreaInfo ;
	i = 0 ;
	while( i < UseArea->AreaInfoNum && OneArea->Start > EndPlusOneIndex )
	{
		i ++ ;
		OneArea ++ ;
	}
	if( i != UseArea->AreaInfoNum )
	{
		Bottom = FALSE ;

		// 引っかかるところまでアドレスを進める
		OneArea = UseArea->AreaInfo ;
		i = 0 ;
		while( i < UseArea->AreaInfoNum && OneArea->EndPlusOne <= StartIndex )
		{
			i ++ ;
			OneArea ++ ;
		}

		// 既存の範囲に触れているかをチェック
		if( OneArea->Start > EndPlusOneIndex )
		{
			NoHit = TRUE ;
		}
	}

	// 使用領域が増えるのかどうかで処理を分岐
	if( IsUse == FALSE )
	{
		// 減る場合

		// 一つも引っかからなかったら終了
		if( i == UseArea->AreaInfoNum || NoHit )
			return 0 ;

		// 最初に引っかかった範囲内に収まる場合は範囲情報の数が増える
		if( OneArea->Start      < StartIndex &&
			OneArea->EndPlusOne > EndPlusOneIndex )
		{
			_MEMMOVE( OneArea + 1, OneArea, sizeof( DIRECT3D9_SHADERCONST_ONE_USEAREA ) * ( UseArea->AreaInfoNum - i ) ) ;
			OneArea[ 0 ].EndPlusOne = ( WORD )StartIndex ;
			OneArea[ 1 ].Start = ( WORD )EndPlusOneIndex ;
			UseArea->AreaInfoNum ++ ;
			return 0 ;
		}

		// 最初に引っかかった範囲内の開始位置と等しくない場合はサイズを調整
		if( OneArea->Start < StartIndex )
		{
			OneArea->EndPlusOne = ( WORD )StartIndex ;
			OneArea ++ ;
			i ++ ;
		}

		// 丸々含まれてしまう範囲の数を調べる
		j = 0 ;
		while( i < UseArea->AreaInfoNum && OneArea[ j ].EndPlusOne <= EndPlusOneIndex )
		{
			j ++ ;
			i ++ ;
		}

		// 丸々含まれてしまう範囲情報を削除
		if( j > 0 && UseArea->AreaInfoNum - i > 0 )
		{
			_MEMMOVE( OneArea, OneArea + j, sizeof( DIRECT3D9_SHADERCONST_ONE_USEAREA ) * ( UseArea->AreaInfoNum - i ) ) ;
		}
		UseArea->AreaInfoNum -= j ;

		// 残りがある場合で範囲が被っている場合は範囲の開始位置をずらす
		if( UseArea->AreaInfoNum > 0 && OneArea->Start < EndPlusOneIndex )
		{
			OneArea->Start = ( WORD )EndPlusOneIndex ;
		}
	}
	else
	{
		// 増える場合

		// 一つも引っかからなかったら範囲情報を追加
		if( i == UseArea->AreaInfoNum )
		{
			// もし一つ前の範囲と接していたら一つ前の範囲情報を広げる
			if( i > 0 && OneArea[ -1 ].EndPlusOne == StartIndex )
			{
				OneArea[ -1 ].EndPlusOne = ( WORD )EndPlusOneIndex ;
			}
			else
			{
				if( Bottom )
				{
					OneArea = UseArea->AreaInfo ;
					_MEMMOVE( OneArea + 1, OneArea, sizeof( DIRECT3D9_SHADERCONST_ONE_USEAREA ) * UseArea->AreaInfoNum ) ;
				}
				OneArea->Start = ( WORD )StartIndex ;
				OneArea->EndPlusOne = ( WORD )EndPlusOneIndex ;
				UseArea->AreaInfoNum ++ ;
			}

			return 0 ;
		}
		if( NoHit )
		{
			// もし一つ前の範囲と接していたら一つ前の範囲情報を広げる
			if( i > 0 && OneArea[ -1 ].EndPlusOne == StartIndex )
			{
				OneArea[ -1 ].EndPlusOne = ( WORD )EndPlusOneIndex ;
			}
			else
			{
				_MEMMOVE( OneArea + 1, OneArea, sizeof( DIRECT3D9_SHADERCONST_ONE_USEAREA ) * ( UseArea->AreaInfoNum - i ) ) ;
				OneArea->Start = ( WORD )StartIndex ;
				OneArea->EndPlusOne = ( WORD )EndPlusOneIndex ;
				UseArea->AreaInfoNum ++ ;
			}
			return 0 ;
		}

		// 最初の一つ前の範囲情報と接していた場合は対象を一つ前の範囲情報にする
		if( i > 0 && OneArea[ -1 ].EndPlusOne == StartIndex )
		{
			OneArea -- ;
			i -- ;
		}

		// 最初に引っかかった範囲情報の開始位置より開始位置が低かったら広げる
		if( OneArea->Start > StartIndex )
		{
			OneArea->Start = ( WORD )StartIndex ;
		}

		// 最初に引っかかった範囲情報に収まる場合はここで終了
		if( OneArea->EndPlusOne >= EndPlusOneIndex )
			return 0 ;

		// 収まらない場合は範囲を広げる
		OneArea->EndPlusOne = ( WORD )EndPlusOneIndex ;
		StartIndex = OneArea->Start ;

		// 以降の範囲情報で丸々被っている範囲の数を調べる
		OneArea ++ ;
		i ++ ;
		j = 0 ;
		while( i < UseArea->AreaInfoNum && OneArea[ j ].EndPlusOne <= EndPlusOneIndex )
		{
			j ++ ;
			i ++ ;
		}

		// 残りがある場合で範囲が被っていたら範囲を追加して削除
		if( i < UseArea->AreaInfoNum && OneArea[ j ].Start <= EndPlusOneIndex )
		{
			OneArea[ -1 ].EndPlusOne = OneArea[ j ].EndPlusOne ;
			j ++ ;
			i ++ ;
		}

		// 範囲情報を削除
		if( j > 0 && UseArea->AreaInfoNum - i > 0 )
		{
			_MEMMOVE( OneArea, OneArea + j, sizeof( DIRECT3D9_SHADERCONST_ONE_USEAREA ) * ( UseArea->AreaInfoNum - i ) ) ;
		}
		UseArea->AreaInfoNum -= j ;
	}

	// 終了
	return 0 ;
}

// シェーダー定数使用領域情報から使用マップを作成する
extern int Graphics_D3D9_ShaderConstant_UseArea_Create( DIRECT3D9_SHADERCONSTANT_USEAREA *UseArea, BYTE *Map, BYTE SetNumber )
{
	int i, j ;
	DIRECT3D9_SHADERCONST_ONE_USEAREA *AInfo ;

	AInfo = UseArea->AreaInfo ;
	for( i = 0 ; i < UseArea->AreaInfoNum ; i ++, AInfo ++ )
	{
		for( j = AInfo->Start ; j < AInfo->EndPlusOne ; j ++ )
		{
			Map[ j ] = SetNumber ;
		}
	}

	// 終了
	return 0 ;
}




// シェーダー定数情報の初期化
extern int Graphics_D3D9_ShaderConstant_InfoSet_Initialize( DIRECT3D9_SHADERCONSTANTINFOSET *ConstInfoSet )
{
	int i ;

	// とりあえず零初期化
	_MEMSET( ConstInfoSet, 0, sizeof( *ConstInfoSet ) ) ;

	// SetMap は 0xff で埋める
	_MEMSET( ConstInfoSet->SetMap, 0xff, sizeof( ConstInfoSet->SetMap ) ) ;

	// シェーダー定数の使用領域情報を初期化する
	for( i = 0 ; i < DX_SHADERCONSTANTSET_NUM ; i ++ )
	{
		Graphics_D3D9_ShaderConstant_UseArea_Initialize( &ConstInfoSet->Info[ DX_SHADERCONSTANTTYPE_VS_FLOAT ][ i ].UseArea, 256 ) ;
		ConstInfoSet->Info[ DX_SHADERCONSTANTTYPE_VS_FLOAT ][ i ].UnitSize = sizeof( FLOAT4 ) ;

		Graphics_D3D9_ShaderConstant_UseArea_Initialize( &ConstInfoSet->Info[ DX_SHADERCONSTANTTYPE_VS_INT   ][ i ].UseArea,  16 ) ;
		ConstInfoSet->Info[ DX_SHADERCONSTANTTYPE_VS_INT ][ i ].UnitSize = sizeof( INT4 ) ;

		Graphics_D3D9_ShaderConstant_UseArea_Initialize( &ConstInfoSet->Info[ DX_SHADERCONSTANTTYPE_VS_BOOL  ][ i ].UseArea,  16 ) ;
		ConstInfoSet->Info[ DX_SHADERCONSTANTTYPE_VS_BOOL ][ i ].UnitSize = sizeof( BOOL ) ;

		Graphics_D3D9_ShaderConstant_UseArea_Initialize( &ConstInfoSet->Info[ DX_SHADERCONSTANTTYPE_PS_FLOAT ][ i ].UseArea, 224 ) ;
		ConstInfoSet->Info[ DX_SHADERCONSTANTTYPE_PS_FLOAT ][ i ].UnitSize = sizeof( FLOAT4 ) ;

		Graphics_D3D9_ShaderConstant_UseArea_Initialize( &ConstInfoSet->Info[ DX_SHADERCONSTANTTYPE_PS_INT   ][ i ].UseArea,  16 ) ;
		ConstInfoSet->Info[ DX_SHADERCONSTANTTYPE_PS_INT ][ i ].UnitSize = sizeof( INT4 ) ;

		Graphics_D3D9_ShaderConstant_UseArea_Initialize( &ConstInfoSet->Info[ DX_SHADERCONSTANTTYPE_PS_BOOL  ][ i ].UseArea,  16 ) ;
		ConstInfoSet->Info[ DX_SHADERCONSTANTTYPE_PS_BOOL ][ i ].UnitSize = sizeof( BOOL ) ;
	}

	// 終了
	return 0 ;
}

// 指定のシェーダー定数セットを適用するかどうかを設定する
extern int Graphics_D3D9_ShaderConstant_InfoSet_SetUseState( DIRECT3D9_SHADERCONSTANTINFOSET *ConstInfoSet, int ApplyMask )
{
	BYTE *BeforeMap ;
	BYTE AfterMap[ DX_SHADERCONSTANTTYPE_NUM ][ 256 ] ;
	BYTE ChangeInfo[ DX_SHADERCONSTANTTYPE_NUM ][ 256 ][ 2 ] ;
	int ChangeInfoNum[ DX_SHADERCONSTANTTYPE_NUM ] ;
	DIRECT3D9_SHADERCONSTANTINFO *Info ;
	BYTE *Map ;
	FLOAT4 FloatConst[ 256 ] ;
	INT4 IntConst[ 16 ] ;
	BOOL BoolConst[ 16 ] ;
	int i, j, k, Size ;
	BYTE Start, End ;

	if( GAPIWin.Direct3DDevice9Object == NULL )
	{
		return -1 ;
	}

	// 指定の定数セットの状態に変化が無い場合は何もしない
	if( ConstInfoSet->ApplyMask == ApplyMask )
		return 0 ;

	// マスクを保存
	ConstInfoSet->ApplyMask = ApplyMask ;

	// マスクからフラグマップを作成
	for( i = 0 ; i < DX_SHADERCONSTANTSET_NUM ; i ++ )
	{
		ConstInfoSet->IsApply[ i ] = ( ApplyMask & ( 1 << i ) ) != 0 ? TRUE : FALSE ;
	}

	// 変化後の使用マップの作成
	_MEMSET( AfterMap, 0xff, sizeof( AfterMap ) ) ;
	for( i = 0 ; i < DX_SHADERCONSTANTSET_NUM ; i ++ )
	{
		if( ConstInfoSet->IsApply[ i ] )
		{
			for( j = 0 ; j < DX_SHADERCONSTANTTYPE_NUM ; j ++ )
			{
				Graphics_D3D9_ShaderConstant_UseArea_Create( &ConstInfoSet->Info[ j ][ i ].UseArea, AfterMap[ j ], ( BYTE )i ) ;
			}
		}
	}

	// 変化がある箇所の情報を構築
	for( i = 0 ; i < DX_SHADERCONSTANTTYPE_NUM ; i ++ )
	{
		BeforeMap = ConstInfoSet->SetMap[ i ] ;
		ChangeInfoNum[ i ] = 0 ;
		Size = ConstInfoSet->Info[ i ][ 0 ].UseArea.TotalSize ;
		j = 0;
		for(;;)
		{
			for( ; j < Size && ( AfterMap[ i ][ j ] == 0xff || BeforeMap[ j ] == AfterMap[ i ][ j ] ) ; j ++ ){}
			if( j == Size ) break ;

			ChangeInfo[ i ][ ChangeInfoNum[ i ] ][ 0 ] = ( BYTE )j ;

			for( ; j < Size && AfterMap[ i ][ j ] != 0xff && BeforeMap[ j ] != AfterMap[ i ][ j ] ; j ++ ){}

			ChangeInfo[ i ][ ChangeInfoNum[ i ] ][ 1 ] = ( BYTE )j ;

			ChangeInfoNum[ i ] ++ ;
		}
	}

	// 変化後の使用中タイプマップを保存
	_MEMCPY( ConstInfoSet->SetMap, AfterMap, sizeof( AfterMap ) ) ;


	// 変化のある箇所の定数を Direct3DDevice9 に適用

	for( j = 0 ; j < ChangeInfoNum[ DX_SHADERCONSTANTTYPE_VS_FLOAT ] ; j ++ )
	{
		Start = ChangeInfo[ DX_SHADERCONSTANTTYPE_VS_FLOAT ][ j ][ 0 ] ;
		End   = ChangeInfo[ DX_SHADERCONSTANTTYPE_VS_FLOAT ][ j ][ 1 ] ;
		Map = AfterMap[ DX_SHADERCONSTANTTYPE_VS_FLOAT ] ;
		Info = ConstInfoSet->Info[ DX_SHADERCONSTANTTYPE_VS_FLOAT ] ;
		for( k = Start ; k < End ; k ++ )
		{
			FloatConst[ k ] = Info[ Map[ k ] ].Float4[ k ] ;
		}
		Direct3DDevice9_SetVertexShaderConstantF( Start, ( float * )&FloatConst[ Start ], ( UINT )( End - Start ) ) ;
		_MEMCPY( &ConstInfoSet->FixInfo[ DX_SHADERCONSTANTTYPE_VS_FLOAT ].Float4[ Start ], ( float * )&FloatConst[ Start ], sizeof( FLOAT4 ) * ( End - Start ) ) ;
	}

	for( j = 0 ; j < ChangeInfoNum[ DX_SHADERCONSTANTTYPE_VS_INT ] ; j ++ )
	{
		Start = ChangeInfo[ DX_SHADERCONSTANTTYPE_VS_INT ][ j ][ 0 ] ;
		End   = ChangeInfo[ DX_SHADERCONSTANTTYPE_VS_INT ][ j ][ 1 ] ;
		Map = AfterMap[ DX_SHADERCONSTANTTYPE_VS_INT ] ;
		Info = ConstInfoSet->Info[ DX_SHADERCONSTANTTYPE_VS_INT ] ;
		for( k = Start ; k < End ; k ++ )
		{
			IntConst[ k ] = Info[ Map[ k ] ].Int4[ k ] ;
		}
		Direct3DDevice9_SetVertexShaderConstantI( Start, ( int * )&IntConst[ Start ], ( UINT )( End - Start ) ) ;
		_MEMCPY( &ConstInfoSet->FixInfo[ DX_SHADERCONSTANTTYPE_VS_INT ].Int4[ Start ], ( int * )&IntConst[ Start ], sizeof( INT4 ) * ( End - Start ) ) ;
	}

	for( j = 0 ; j < ChangeInfoNum[ DX_SHADERCONSTANTTYPE_VS_BOOL ] ; j ++ )
	{
		Start = ChangeInfo[ DX_SHADERCONSTANTTYPE_VS_BOOL ][ j ][ 0 ] ;
		End   = ChangeInfo[ DX_SHADERCONSTANTTYPE_VS_BOOL ][ j ][ 1 ] ;
		Map = AfterMap[ DX_SHADERCONSTANTTYPE_VS_BOOL ] ;
		Info = ConstInfoSet->Info[ DX_SHADERCONSTANTTYPE_VS_BOOL ] ;
		for( k = Start ; k < End ; k ++ )
		{
			BoolConst[ k ] = Info[ Map[ k ] ].Bool[ k ] ;
		}
		Direct3DDevice9_SetVertexShaderConstantB( Start, ( BOOL * )&BoolConst[ Start ], ( UINT )( End - Start ) ) ;
		_MEMCPY( &ConstInfoSet->FixInfo[ DX_SHADERCONSTANTTYPE_VS_BOOL ].Bool[ Start ], ( BOOL * )&BoolConst[ Start ], sizeof( BOOL ) * ( End - Start ) ) ;
	}



	for( j = 0 ; j < ChangeInfoNum[ DX_SHADERCONSTANTTYPE_PS_FLOAT ] ; j ++ )
	{
		Start = ChangeInfo[ DX_SHADERCONSTANTTYPE_PS_FLOAT ][ j ][ 0 ] ;
		End   = ChangeInfo[ DX_SHADERCONSTANTTYPE_PS_FLOAT ][ j ][ 1 ] ;
		Map = AfterMap[ DX_SHADERCONSTANTTYPE_PS_FLOAT ] ;
		Info = ConstInfoSet->Info[ DX_SHADERCONSTANTTYPE_PS_FLOAT ] ;
		for( k = Start ; k < End ; k ++ )
		{
			FloatConst[ k ] = Info[ Map[ k ] ].Float4[ k ] ;
		}
		Direct3DDevice9_SetPixelShaderConstantF( Start, ( float * )&FloatConst[ Start ], ( UINT )( End - Start ) ) ;
		_MEMCPY( &ConstInfoSet->FixInfo[ DX_SHADERCONSTANTTYPE_PS_FLOAT ].Float4[ Start ], ( float * )&FloatConst[ Start ], sizeof( FLOAT4 ) * ( End - Start ) ) ;
	}

	for( j = 0 ; j < ChangeInfoNum[ DX_SHADERCONSTANTTYPE_PS_INT ] ; j ++ )
	{
		Start = ChangeInfo[ DX_SHADERCONSTANTTYPE_PS_INT ][ j ][ 0 ] ;
		End   = ChangeInfo[ DX_SHADERCONSTANTTYPE_PS_INT ][ j ][ 1 ] ;
		Map = AfterMap[ DX_SHADERCONSTANTTYPE_PS_INT ] ;
		Info = ConstInfoSet->Info[ DX_SHADERCONSTANTTYPE_PS_INT ] ;
		for( k = Start ; k < End ; k ++ )
		{
			IntConst[ k ] = Info[ Map[ k ] ].Int4[ k ] ;
		}
		Direct3DDevice9_SetPixelShaderConstantI( Start, ( int * )&IntConst[ Start ], ( UINT )( End - Start ) ) ;
		_MEMCPY( &ConstInfoSet->FixInfo[ DX_SHADERCONSTANTTYPE_PS_INT ].Int4[ Start ], ( int * )&IntConst[ Start ], sizeof( INT4 ) * ( End - Start ) ) ;
	}

	for( j = 0 ; j < ChangeInfoNum[ DX_SHADERCONSTANTTYPE_PS_BOOL ] ; j ++ )
	{
		Start = ChangeInfo[ DX_SHADERCONSTANTTYPE_PS_BOOL ][ j ][ 0 ] ;
		End   = ChangeInfo[ DX_SHADERCONSTANTTYPE_PS_BOOL ][ j ][ 1 ] ;
		Map = AfterMap[ DX_SHADERCONSTANTTYPE_PS_BOOL ] ;
		Info = ConstInfoSet->Info[ DX_SHADERCONSTANTTYPE_PS_BOOL ] ;
		for( k = Start ; k < End ; k ++ )
		{
			BoolConst[ k ] = Info[ Map[ k ] ].Bool[ k ] ;
		}
		Direct3DDevice9_SetPixelShaderConstantB( Start, ( BOOL * )&BoolConst[ Start ], ( UINT )( End - Start ) ) ;
		_MEMCPY( &ConstInfoSet->FixInfo[ DX_SHADERCONSTANTTYPE_PS_BOOL ].Bool[ Start ], ( BOOL * )&BoolConst[ Start ], sizeof( BOOL ) * ( End - Start ) ) ;
	}

	// 終了
	return 0 ;
}

// シェーダー定数情報をデバイスに設定する
static int Graphics_D3D9_SetShaderConstantDeviceSet( DIRECT3D9_SHADERCONSTANTINFOSET *ConstInfoSet, int TypeIndex, int ConstantIndex, const void *Param, int ParamNum )
{
	union
	{
		FLOAT4 TempArrayF[ 256 ] ;
		INT4 TempArrayI[ 16 ] ;
		BOOL TempArrayB[ 16 ] ;
	} ;

	if( GAPIWin.Direct3DDevice9Object == NULL )
	{
		return -1 ;
	}

	// データが無い場合はマップから参照する
	if( Param == NULL )
	{
		DIRECT3D9_SHADERCONSTANTINFO *Info ;
		BYTE *Map ;
		int Start, End ;
		int i ;

		Start = ConstantIndex ;
		End = Start + ParamNum ;
		Map = ConstInfoSet->SetMap[ TypeIndex ] ;

		Info = ConstInfoSet->Info[ TypeIndex ] ;
		switch( TypeIndex )
		{
		case DX_SHADERCONSTANTTYPE_VS_FLOAT :
		case DX_SHADERCONSTANTTYPE_PS_FLOAT :
			_MEMSET( TempArrayF, 0, sizeof( TempArrayF ) ) ;
			for( i = Start ; i < End ; i ++ )
			{
				if( Map[ i ] != 0xff )
				{
					TempArrayF[ i ] = Info[ Map[ i ] ].Float4[ i ] ;
				}
			}
			Param = &TempArrayF[ Start ] ;
			break ;

		case DX_SHADERCONSTANTTYPE_VS_INT :
		case DX_SHADERCONSTANTTYPE_PS_INT :
			_MEMSET( TempArrayI, 0, sizeof( TempArrayI ) ) ;
			for( i = Start ; i < End ; i ++ )
			{
				if( Map[ i ] != 0xff )
				{
					TempArrayI[ i ] = Info[ Map[ i ] ].Int4[ i ] ;
				}
			}
			Param = &TempArrayI[ Start ] ;
			break ;

		case DX_SHADERCONSTANTTYPE_VS_BOOL :
		case DX_SHADERCONSTANTTYPE_PS_BOOL :
			_MEMSET( TempArrayB, 0, sizeof( TempArrayB ) ) ;
			for( i = Start ; i < End ; i ++ )
			{
				if( Map[ i ] != 0xff )
				{
					TempArrayB[ i ] = Info[ Map[ i ] ].Bool[ i ] ;
				}
			}
			Param = &TempArrayB[ Start ] ;
			break ;
		}
	}

	// デバイスにセットとパラメータを保存
	switch( TypeIndex )
	{
	case DX_SHADERCONSTANTTYPE_VS_FLOAT :
		if( GSYS.HardInfo.UseShader == TRUE ) 
		{
			Direct3DDevice9_SetVertexShaderConstantF( ( UINT )ConstantIndex, ( float * )Param, ( UINT )ParamNum ) ;
		}
		_MEMCPY( &ConstInfoSet->FixInfo[ DX_SHADERCONSTANTTYPE_VS_FLOAT ].Float4[ ConstantIndex ], Param, sizeof( FLOAT4 ) * ParamNum ) ;
		break ;

	case DX_SHADERCONSTANTTYPE_VS_INT :
		if( GSYS.HardInfo.UseShader == TRUE ) 
		{
			Direct3DDevice9_SetVertexShaderConstantI( ( UINT )ConstantIndex, ( int * )Param, ( UINT )ParamNum ) ;
		}
		_MEMCPY( &ConstInfoSet->FixInfo[ DX_SHADERCONSTANTTYPE_VS_INT ].Int4[ ConstantIndex ], Param, sizeof( INT4 ) * ParamNum ) ;
		break ;

	case DX_SHADERCONSTANTTYPE_VS_BOOL :
		if( GSYS.HardInfo.UseShader == TRUE ) 
		{
			Direct3DDevice9_SetVertexShaderConstantB( ( UINT )ConstantIndex, ( BOOL * )Param, ( UINT )ParamNum ) ;
		}
		_MEMCPY( &ConstInfoSet->FixInfo[ DX_SHADERCONSTANTTYPE_VS_BOOL ].Bool[ ConstantIndex ], Param, sizeof( BOOL ) * ParamNum ) ;
		break ;

	case DX_SHADERCONSTANTTYPE_PS_FLOAT :
		if( GSYS.HardInfo.UseShader == TRUE ) 
		{
			Direct3DDevice9_SetPixelShaderConstantF( ( UINT )ConstantIndex, ( float * )Param, ( UINT )ParamNum ) ;
		}
		_MEMCPY( &ConstInfoSet->FixInfo[ DX_SHADERCONSTANTTYPE_PS_FLOAT ].Float4[ ConstantIndex ], Param, sizeof( FLOAT4 ) * ParamNum ) ;
		break ;

	case DX_SHADERCONSTANTTYPE_PS_INT :
		if( GSYS.HardInfo.UseShader == TRUE ) 
		{
			Direct3DDevice9_SetPixelShaderConstantI( ( UINT )ConstantIndex, ( int * )Param, ( UINT )ParamNum ) ;
		}
		_MEMCPY( &ConstInfoSet->FixInfo[ DX_SHADERCONSTANTTYPE_PS_INT ].Int4[ ConstantIndex ], Param, sizeof( INT4 ) * ParamNum ) ;
		break ;

	case DX_SHADERCONSTANTTYPE_PS_BOOL :
		if( GSYS.HardInfo.UseShader == TRUE ) 
		{
			Direct3DDevice9_SetPixelShaderConstantB( ( UINT )ConstantIndex, ( BOOL * )Param, ( UINT )ParamNum ) ;
		}
		_MEMCPY( &ConstInfoSet->FixInfo[ DX_SHADERCONSTANTTYPE_PS_BOOL ].Bool[ ConstantIndex ], Param, sizeof( BOOL ) * ParamNum ) ;
		break ;
	}

	// 終了
	return 0 ;
}

// シェーダー定数情報を設定する
extern int Graphics_D3D9_ShaderConstant_InfoSet_SetParam( DIRECT3D9_SHADERCONSTANTINFOSET *ConstInfoSet, int TypeIndex, int SetIndex, int ConstantIndex, const void *Param, int ParamNum, int UpdateUseArea )
{
	int Start, End ;
	int i ;
	bool bMaskHit ;
	BYTE *Map ;
	BYTE *UseMap ;
	DIRECT3D9_SHADERCONSTANTINFO *Info ;

	Info = &ConstInfoSet->Info[ TypeIndex ][ SetIndex ] ;

	// 単体の使用範囲の更新
	if( UpdateUseArea )
	{
		Graphics_D3D9_ShaderConstant_UseArea_Set( &Info->UseArea, TRUE, ConstantIndex, ParamNum ) ;
	}

	// 配列にデータをセット
	_MEMCPY( &Info->Data[ ConstantIndex * Info->UnitSize ], Param, ( size_t )( ParamNum * Info->UnitSize ) ) ;

	// 範囲のセット
	Start = ConstantIndex ;
	End = Start + ParamNum ;

	// 全体の使用領域の更新
	bMaskHit = false ;
	Map = ConstInfoSet->SetMap[ TypeIndex ] ;
	UseMap = ConstInfoSet->UseMap[ TypeIndex ][ SetIndex ] ;
	for( i = Start; i < End; i++ )
	{
		UseMap[ i ] = TRUE ;

		if( ConstInfoSet->IsApply[ SetIndex ] )
		{
			if( Map[ i ] == 0xff || Map[ i ] <= SetIndex )
			{
				Map[ i ] = ( BYTE )SetIndex ;
			}
			else
			{
				bMaskHit = true ;
			}
		}
	}

	// デバイスにセット
	if( ConstInfoSet->IsApply[ SetIndex ] )
	{
		Graphics_D3D9_SetShaderConstantDeviceSet( ConstInfoSet, TypeIndex, ConstantIndex, bMaskHit == false ? Param : NULL, ParamNum ) ;
	}

	// 終了
	return 0 ;
}

// 指定領域のシェーダー定数情報をリセットする
extern int Graphics_D3D9_ShaderConstant_InfoSet_ResetParam( DIRECT3D9_SHADERCONSTANTINFOSET *ConstInfoSet, int TypeIndex, int SetIndex, int ConstantIndex, int ParamNum )
{
	int Start, End ;
	int i, j ;
	bool bMaskHit ;
	BYTE *Map ;
	BYTE ( *UseMap )[ 256 ] ;
	DIRECT3D9_SHADERCONSTANTINFO *Info ;

	// 使用範囲の更新
	Info = &ConstInfoSet->Info[ TypeIndex ][ SetIndex ] ;
	Graphics_D3D9_ShaderConstant_UseArea_Set( &Info->UseArea, FALSE, ConstantIndex, ParamNum ) ;

	// 範囲のセット
	Start = ConstantIndex ;
	End = Start + ParamNum ;

	// 使用領域の更新
	Map = ConstInfoSet->SetMap[ TypeIndex ] ;
	UseMap = ConstInfoSet->UseMap[ TypeIndex ] ;
	bMaskHit = false ;
	for( i = Start; i < End; i++ )
	{
		UseMap[ SetIndex ][ i ] = FALSE ;

		if( Map[ i ] == SetIndex )
		{
			for( j = SetIndex; j >= 0 && UseMap[ j ][ i ] == FALSE; j-- ){}
			if( j == -1 )
			{
				Map[ i ] = 0xff ;
			}
			else
			{
				Map[ i ] = ( BYTE )j ;
				bMaskHit = true ;
			}
		}
	}

	// 使用領域が変化することで今まで無効だった定数が有効になる場合は定数情報の更新
	if( bMaskHit )
	{
		Graphics_D3D9_SetShaderConstantDeviceSet( ConstInfoSet, TypeIndex, ConstantIndex, NULL, ParamNum ) ;
	}

	// 終了
	return 0 ;
}














// 頂点シェーダ宣言関係関数

static  D_D3DVERTEXELEMENT9	MakeVertexElement9( BYTE Stream, BYTE Offset, BYTE Type, BYTE Method, BYTE Usage, BYTE UsageIndex )
{
	D_D3DVERTEXELEMENT9 Result = { Stream, Offset, Type, Method, Usage, UsageIndex } ;
	return Result ;
}

// Direct3DVertexDeclaration9 オブジェクトの作成( 0:成功  -1:失敗 )
extern	int		Graphics_D3D9_VertexDeclaration_Create( void )
{
	// 既に作成されていたときのために削除処理を行う
	Graphics_D3D9_VertexDeclaration_Terminate() ;

#ifndef DX_NON_MODEL
	{
		D_D3DVERTEXELEMENT9 *VE, VertElem[ 64 ] = 
		{
			{ 0, 0,   D_D3DDECLTYPE_FLOAT3,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_POSITION,     0 },
			{ 0, 12,  D_D3DDECLTYPE_FLOAT4,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_BLENDWEIGHT,  0 },
			{ 0, 28,  D_D3DDECLTYPE_UBYTE4,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_BLENDINDICES, 0 },
			{ 0, 32,  D_D3DDECLTYPE_FLOAT3,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_NORMAL,       0 },
			{ 0, 44,  D_D3DDECLTYPE_D3DCOLOR, D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_COLOR,        0 },
			{ 0, 48,  D_D3DDECLTYPE_D3DCOLOR, D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_COLOR,        1 },
			{ 0, 52,  D_D3DDECLTYPE_FLOAT4,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_TEXCOORD,     0 },
			D_D3DDECL_END()
		} ;
		int offset = 0, index = 0 ;
		int i, j, k, l ;

		// バンプマップ情報つきかどうかで２ループ
		for( i = 0 ; i < 2 ; i ++ )
		{
			// スキニングメッシュタイプによって３ループ
			for( j = 0 ; j < 3 ; j ++ )
			{
				if( i == 0 )
				{
					switch( j )
					{
					case 0 :
						// バンプマップ情報無し剛体メッシュ
						VertElem[ 0 ] = MakeVertexElement9( 0,  0,  D_D3DDECLTYPE_FLOAT3,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_POSITION,     0 ) ;
						VertElem[ 1 ] = MakeVertexElement9( 0, 12,  D_D3DDECLTYPE_FLOAT3,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_NORMAL,       0 ) ;
						VertElem[ 2 ] = MakeVertexElement9( 0, 24,  D_D3DDECLTYPE_D3DCOLOR, D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_COLOR,        0 ) ;
						VertElem[ 3 ] = MakeVertexElement9( 0, 28,  D_D3DDECLTYPE_D3DCOLOR, D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_COLOR,        1 ) ;
						offset = 32 ;
						index = 4 ;
						break ;

					case 1 :
						// バンプマップ情報無し４ボーン内スキニングメッシュ
						VertElem[ 0 ] = MakeVertexElement9( 0,  0,  D_D3DDECLTYPE_FLOAT3,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_POSITION,     0 ) ;
						VertElem[ 1 ] = MakeVertexElement9( 0, 12,  D_D3DDECLTYPE_UBYTE4,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_BLENDINDICES, 0 ) ;
						VertElem[ 2 ] = MakeVertexElement9( 0, 16,  D_D3DDECLTYPE_FLOAT4,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_BLENDWEIGHT,  0 ) ;
						VertElem[ 3 ] = MakeVertexElement9( 0, 32,  D_D3DDECLTYPE_FLOAT3,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_NORMAL,       0 ) ;
						VertElem[ 4 ] = MakeVertexElement9( 0, 44,  D_D3DDECLTYPE_D3DCOLOR, D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_COLOR,        0 ) ;
						VertElem[ 5 ] = MakeVertexElement9( 0, 48,  D_D3DDECLTYPE_D3DCOLOR, D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_COLOR,        1 ) ;
						offset = 52 ;
						index = 6 ;
						break ;

					case 2 :
						// バンプマップ情報無し８ボーン内スキニングメッシュ
						VertElem[ 0 ] = MakeVertexElement9( 0,  0,  D_D3DDECLTYPE_FLOAT3,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_POSITION,     0 ) ;
						VertElem[ 1 ] = MakeVertexElement9( 0, 12,  D_D3DDECLTYPE_UBYTE4,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_BLENDINDICES, 0 ) ;
						VertElem[ 2 ] = MakeVertexElement9( 0, 16,  D_D3DDECLTYPE_UBYTE4,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_BLENDINDICES, 1 ) ;
						VertElem[ 3 ] = MakeVertexElement9( 0, 20,  D_D3DDECLTYPE_FLOAT4,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_BLENDWEIGHT,  0 ) ;
						VertElem[ 4 ] = MakeVertexElement9( 0, 36,  D_D3DDECLTYPE_FLOAT4,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_BLENDWEIGHT,  1 ) ;
						VertElem[ 5 ] = MakeVertexElement9( 0, 52,  D_D3DDECLTYPE_FLOAT3,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_NORMAL,       0 ) ;
						VertElem[ 6 ] = MakeVertexElement9( 0, 64,  D_D3DDECLTYPE_D3DCOLOR, D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_COLOR,        0 ) ;
						VertElem[ 7 ] = MakeVertexElement9( 0, 68,  D_D3DDECLTYPE_D3DCOLOR, D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_COLOR,        1 ) ;
						offset = 72 ;
						index = 8 ;
						break ;
					}
				}
				else
				{
					switch( j )
					{
					case 0 :
						// バンプマップ情報有り剛体メッシュ
						VertElem[ 0 ] = MakeVertexElement9( 0,  0,  D_D3DDECLTYPE_FLOAT3,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_POSITION,     0 ) ;
						VertElem[ 1 ] = MakeVertexElement9( 0, 12,  D_D3DDECLTYPE_FLOAT3,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_TANGENT,      0 ) ;
						VertElem[ 2 ] = MakeVertexElement9( 0, 24,  D_D3DDECLTYPE_FLOAT3,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_BINORMAL,     0 ) ;
						VertElem[ 3 ] = MakeVertexElement9( 0, 36,  D_D3DDECLTYPE_FLOAT3,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_NORMAL,       0 ) ;
						VertElem[ 4 ] = MakeVertexElement9( 0, 48,  D_D3DDECLTYPE_D3DCOLOR, D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_COLOR,        0 ) ;
						VertElem[ 5 ] = MakeVertexElement9( 0, 52,  D_D3DDECLTYPE_D3DCOLOR, D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_COLOR,        1 ) ;
						offset = 56 ;
						index = 6 ;
						break ;

					case 1 :
						// バンプマップ情報有り４ボーン内スキニングメッシュ
						VertElem[ 0 ] = MakeVertexElement9( 0,  0,  D_D3DDECLTYPE_FLOAT3,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_POSITION,     0 ) ;
						VertElem[ 1 ] = MakeVertexElement9( 0, 12,  D_D3DDECLTYPE_UBYTE4,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_BLENDINDICES, 0 ) ;
						VertElem[ 2 ] = MakeVertexElement9( 0, 16,  D_D3DDECLTYPE_FLOAT4,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_BLENDWEIGHT,  0 ) ;
						VertElem[ 3 ] = MakeVertexElement9( 0, 32,  D_D3DDECLTYPE_FLOAT3,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_TANGENT,      0 ) ;
						VertElem[ 4 ] = MakeVertexElement9( 0, 44,  D_D3DDECLTYPE_FLOAT3,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_BINORMAL,     0 ) ;
						VertElem[ 5 ] = MakeVertexElement9( 0, 56,  D_D3DDECLTYPE_FLOAT3,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_NORMAL,       0 ) ;
						VertElem[ 6 ] = MakeVertexElement9( 0, 68,  D_D3DDECLTYPE_D3DCOLOR, D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_COLOR,        0 ) ;
						VertElem[ 7 ] = MakeVertexElement9( 0, 72,  D_D3DDECLTYPE_D3DCOLOR, D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_COLOR,        1 ) ;
						offset = 76 ;
						index = 8 ;
						break ;

					case 2 :
						// バンプマップ情報有り８ボーン内スキニングメッシュ
						VertElem[ 0 ] = MakeVertexElement9( 0,  0,  D_D3DDECLTYPE_FLOAT3,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_POSITION,     0 ) ;
						VertElem[ 1 ] = MakeVertexElement9( 0, 12,  D_D3DDECLTYPE_UBYTE4,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_BLENDINDICES, 0 ) ;
						VertElem[ 2 ] = MakeVertexElement9( 0, 16,  D_D3DDECLTYPE_UBYTE4,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_BLENDINDICES, 1 ) ;
						VertElem[ 3 ] = MakeVertexElement9( 0, 20,  D_D3DDECLTYPE_FLOAT4,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_BLENDWEIGHT,  0 ) ;
						VertElem[ 4 ] = MakeVertexElement9( 0, 36,  D_D3DDECLTYPE_FLOAT4,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_BLENDWEIGHT,  1 ) ;
						VertElem[ 5 ] = MakeVertexElement9( 0, 52,  D_D3DDECLTYPE_FLOAT3,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_TANGENT,      0 ) ;
						VertElem[ 6 ] = MakeVertexElement9( 0, 64,  D_D3DDECLTYPE_FLOAT3,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_BINORMAL,     0 ) ;
						VertElem[ 7 ] = MakeVertexElement9( 0, 76,  D_D3DDECLTYPE_FLOAT3,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_NORMAL,       0 ) ;
						VertElem[ 8 ] = MakeVertexElement9( 0, 88,  D_D3DDECLTYPE_D3DCOLOR, D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_COLOR,        0 ) ;
						VertElem[ 9 ] = MakeVertexElement9( 0, 92,  D_D3DDECLTYPE_D3DCOLOR, D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_COLOR,        1 ) ;
						offset = 96 ;
						index = 10 ;
						break ;
					}
				}

				for( k = 0 ; k < 9 ; k ++ )
				{
					VE = &VertElem[ index ] ;
					for( l = 0 ; l < k ; l ++, VE ++ )
					{
						VE->Stream     = 0 ;
						VE->Offset     = ( WORD )( offset + sizeof( float ) * 4 * l ) ;
						VE->Type       = D_D3DDECLTYPE_FLOAT4 ;
						VE->Method     = D_D3DDECLMETHOD_DEFAULT ;
						VE->Usage      = D_D3DDECLUSAGE_TEXCOORD ;
						VE->UsageIndex = ( BYTE )l ;
					}
					VE->Stream     = 0xFF ;
					VE->Offset     = 0 ;
					VE->Type       = D_D3DDECLTYPE_UNUSED ;
					VE->Method     = 0 ;
					VE->Usage      = 0 ;
					VE->UsageIndex = 0 ;

					Direct3DDevice9_CreateVertexDeclaration( VertElem, &GD3D9.Device.VertexDeclaration.MV1_VertexDeclaration[ i ][ j ][ k ] ) ;
				}
			}
		}
	}
#endif // DX_NON_MODEL

	// 固定パイプライン互換シェーダーで使用する頂点データ形式を作成
	{
		D_D3DVERTEXELEMENT9 VertElem_[ 11 ] ;

		// VERTEXFVF_2D_USER ( VERTEX2D構造体形式 )
		VertElem_[ 0 ] = MakeVertexElement9( 0,     0,  D_D3DDECLTYPE_FLOAT4,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_POSITION,     0 ) ;
		VertElem_[ 1 ] = MakeVertexElement9( 0,    16,  D_D3DDECLTYPE_D3DCOLOR, D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_COLOR,        0 ) ;
		VertElem_[ 2 ] = MakeVertexElement9( 0,    20,  D_D3DDECLTYPE_FLOAT2,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_TEXCOORD,     0 ) ;
		VertElem_[ 3 ] = MakeVertexElement9( 0xff,  0,  D_D3DDECLTYPE_UNUSED,                         0,                       0,     0 ) ;
		Direct3DDevice9_CreateVertexDeclaration( VertElem_, &GD3D9.Device.VertexDeclaration.Base2DDeclaration[ VERTEXFVF_DECL_2D_USER ] ) ;

		// VERTEXFVF_NOTEX_2D ( VERTEX_NOTEX_2D構造体形式 )
		VertElem_[ 0 ] = MakeVertexElement9( 0,     0,  D_D3DDECLTYPE_FLOAT4,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_POSITION,     0 ) ;
		VertElem_[ 1 ] = MakeVertexElement9( 0,    16,  D_D3DDECLTYPE_D3DCOLOR, D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_COLOR,        0 ) ;
		VertElem_[ 2 ] = MakeVertexElement9( 0xff,  0,  D_D3DDECLTYPE_UNUSED,                         0,                       0,     0 ) ;
		Direct3DDevice9_CreateVertexDeclaration( VertElem_, &GD3D9.Device.VertexDeclaration.Base2DDeclaration[ VERTEXFVF_DECL_NOTEX_2D ] ) ;

		// VERTEXFVF_2D ( VERTEX_2D構造体形式 )
		VertElem_[ 0 ] = MakeVertexElement9( 0,     0,  D_D3DDECLTYPE_FLOAT4,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_POSITION,     0 ) ;
		VertElem_[ 1 ] = MakeVertexElement9( 0,    16,  D_D3DDECLTYPE_D3DCOLOR, D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_COLOR,        0 ) ;
		VertElem_[ 2 ] = MakeVertexElement9( 0,    20,  D_D3DDECLTYPE_FLOAT2,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_TEXCOORD,     0 ) ;
		VertElem_[ 3 ] = MakeVertexElement9( 0xff,  0,  D_D3DDECLTYPE_UNUSED,                         0,                       0,     0 ) ;
		Direct3DDevice9_CreateVertexDeclaration( VertElem_, &GD3D9.Device.VertexDeclaration.Base2DDeclaration[ VERTEXFVF_DECL_2D ] ) ;

		// VERTEXFVF_BLENDTEX_2D ( VERTEX_BLENDTEX_2D構造体形式 )
		VertElem_[ 0 ] = MakeVertexElement9( 0,     0,  D_D3DDECLTYPE_FLOAT4,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_POSITION,     0 ) ;
		VertElem_[ 1 ] = MakeVertexElement9( 0,    16,  D_D3DDECLTYPE_D3DCOLOR, D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_COLOR,        0 ) ;
		VertElem_[ 2 ] = MakeVertexElement9( 0,    20,  D_D3DDECLTYPE_D3DCOLOR, D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_COLOR,        1 ) ;
		VertElem_[ 3 ] = MakeVertexElement9( 0,    24,  D_D3DDECLTYPE_FLOAT2,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_TEXCOORD,     0 ) ;
		VertElem_[ 4 ] = MakeVertexElement9( 0,    32,  D_D3DDECLTYPE_FLOAT2,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_TEXCOORD,     1 ) ;
		VertElem_[ 5 ] = MakeVertexElement9( 0xff,  0,  D_D3DDECLTYPE_UNUSED,                         0,                       0,     0 ) ;
		Direct3DDevice9_CreateVertexDeclaration( VertElem_, &GD3D9.Device.VertexDeclaration.Base2DDeclaration[ VERTEXFVF_DECL_BLENDTEX_2D ] ) ;

		// VERTEXFVF_SHADER_2D ( VERTEX2DSHADER構造体形式 )
		VertElem_[ 0 ] = MakeVertexElement9( 0,     0,  D_D3DDECLTYPE_FLOAT4,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_POSITION,     0 ) ;
		VertElem_[ 1 ] = MakeVertexElement9( 0,    16,  D_D3DDECLTYPE_D3DCOLOR, D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_COLOR,        0 ) ;
		VertElem_[ 2 ] = MakeVertexElement9( 0,    20,  D_D3DDECLTYPE_D3DCOLOR, D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_COLOR,        1 ) ;
		VertElem_[ 3 ] = MakeVertexElement9( 0,    24,  D_D3DDECLTYPE_FLOAT2,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_TEXCOORD,     0 ) ;
		VertElem_[ 4 ] = MakeVertexElement9( 0,    32,  D_D3DDECLTYPE_FLOAT2,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_TEXCOORD,     1 ) ;
		VertElem_[ 5 ] = MakeVertexElement9( 0xff,  0,  D_D3DDECLTYPE_UNUSED,                         0,                       0,     0 ) ;
		Direct3DDevice9_CreateVertexDeclaration( VertElem_, &GD3D9.Device.VertexDeclaration.Base2DDeclaration[ VERTEXFVF_DECL_SHADER_2D ] ) ;

		// VERTEXFVF_NOTEX_3D ( VERTEX_NOTEX_3D構造体形式 )
		VertElem_[ 0 ] = MakeVertexElement9( 0,     0,  D_D3DDECLTYPE_FLOAT3,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_POSITION,     0 ) ;
		VertElem_[ 1 ] = MakeVertexElement9( 0,    12,  D_D3DDECLTYPE_D3DCOLOR, D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_COLOR,        0 ) ;
		VertElem_[ 2 ] = MakeVertexElement9( 0xff,  0,  D_D3DDECLTYPE_UNUSED,                         0,                       0,     0 ) ;
		Direct3DDevice9_CreateVertexDeclaration( VertElem_, &GD3D9.Device.VertexDeclaration.Base2DDeclaration[ VERTEXFVF_DECL_NOTEX_3D ] ) ;

		// VERTEXFVF_3D ( VERTEX_3D構造体形式 )
		VertElem_[ 0 ] = MakeVertexElement9( 0,     0,  D_D3DDECLTYPE_FLOAT3,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_POSITION,     0 ) ;
		VertElem_[ 1 ] = MakeVertexElement9( 0,    12,  D_D3DDECLTYPE_D3DCOLOR, D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_COLOR,        0 ) ;
		VertElem_[ 2 ] = MakeVertexElement9( 0,    16,  D_D3DDECLTYPE_FLOAT2,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_TEXCOORD,     0 ) ;
		VertElem_[ 3 ] = MakeVertexElement9( 0xff,  0,  D_D3DDECLTYPE_UNUSED,                         0,                       0,     0 ) ;
		Direct3DDevice9_CreateVertexDeclaration( VertElem_, &GD3D9.Device.VertexDeclaration.Base2DDeclaration[ VERTEXFVF_DECL_3D ] ) ;

		// VERTEXFVF_3D_LIGHT ( VERTEX3D構造体形式 )
		VertElem_[ 0 ] = MakeVertexElement9( 0,     0,  D_D3DDECLTYPE_FLOAT3,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_POSITION,     0 ) ;
		VertElem_[ 1 ] = MakeVertexElement9( 0,    12,  D_D3DDECLTYPE_FLOAT3,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_NORMAL,       0 ) ;
		VertElem_[ 2 ] = MakeVertexElement9( 0,    24,  D_D3DDECLTYPE_D3DCOLOR, D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_COLOR,        0 ) ;
		VertElem_[ 3 ] = MakeVertexElement9( 0,    28,  D_D3DDECLTYPE_D3DCOLOR, D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_COLOR,        1 ) ;
		VertElem_[ 4 ] = MakeVertexElement9( 0,    32,  D_D3DDECLTYPE_FLOAT2,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_TEXCOORD,     0 ) ;
		VertElem_[ 5 ] = MakeVertexElement9( 0,    40,  D_D3DDECLTYPE_FLOAT2,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_TEXCOORD,     1 ) ;
		VertElem_[ 6 ] = MakeVertexElement9( 0xff,  0,  D_D3DDECLTYPE_UNUSED,                         0,                       0,     0 ) ;
		Direct3DDevice9_CreateVertexDeclaration( VertElem_, &GD3D9.Device.VertexDeclaration.Base2DDeclaration[ VERTEXFVF_DECL_3D_LIGHT ] ) ;

		// VERTEXFVF_SHADER_3D ( VERTEX3DSHADER構造体形式 )
		VertElem_[ 0 ] = MakeVertexElement9( 0,     0,  D_D3DDECLTYPE_FLOAT3,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_POSITION,     0 ) ;
		VertElem_[ 1 ] = MakeVertexElement9( 0,    12,  D_D3DDECLTYPE_FLOAT4,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_POSITION,     1 ) ;
		VertElem_[ 2 ] = MakeVertexElement9( 0,    28,  D_D3DDECLTYPE_FLOAT3,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_NORMAL,       0 ) ;
		VertElem_[ 3 ] = MakeVertexElement9( 0,    40,  D_D3DDECLTYPE_FLOAT3,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_TANGENT,      0 ) ;
		VertElem_[ 4 ] = MakeVertexElement9( 0,    52,  D_D3DDECLTYPE_FLOAT3,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_BINORMAL,     0 ) ;
		VertElem_[ 5 ] = MakeVertexElement9( 0,    64,  D_D3DDECLTYPE_D3DCOLOR, D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_COLOR,        0 ) ;
		VertElem_[ 6 ] = MakeVertexElement9( 0,    68,  D_D3DDECLTYPE_D3DCOLOR, D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_COLOR,        1 ) ;
		VertElem_[ 7 ] = MakeVertexElement9( 0,    72,  D_D3DDECLTYPE_FLOAT2,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_TEXCOORD,     0 ) ;
		VertElem_[ 8 ] = MakeVertexElement9( 0,    80,  D_D3DDECLTYPE_FLOAT2,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_TEXCOORD,     1 ) ;
		VertElem_[ 9 ] = MakeVertexElement9( 0xff,  0,  D_D3DDECLTYPE_UNUSED,                         0,                       0,     0 ) ;
		Direct3DDevice9_CreateVertexDeclaration( VertElem_, &GD3D9.Device.VertexDeclaration.Base2DDeclaration[ VERTEXFVF_DECL_SHADER_3D ] ) ;

		// VERTEXFVF_DECL_TEX8_2D ( VERTEX_TEX8_2D構造体形式 )
		VertElem_[  0 ] = MakeVertexElement9( 0,     0,  D_D3DDECLTYPE_FLOAT4,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_POSITION,     0 ) ;
		VertElem_[  1 ] = MakeVertexElement9( 0,    16,  D_D3DDECLTYPE_D3DCOLOR, D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_COLOR,        0 ) ;
		VertElem_[  2 ] = MakeVertexElement9( 0,    20,  D_D3DDECLTYPE_FLOAT2,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_TEXCOORD,     0 ) ;
		VertElem_[  3 ] = MakeVertexElement9( 0,    28,  D_D3DDECLTYPE_FLOAT2,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_TEXCOORD,     1 ) ;
		VertElem_[  4 ] = MakeVertexElement9( 0,    36,  D_D3DDECLTYPE_FLOAT2,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_TEXCOORD,     2 ) ;
		VertElem_[  5 ] = MakeVertexElement9( 0,    44,  D_D3DDECLTYPE_FLOAT2,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_TEXCOORD,     3 ) ;
		VertElem_[  6 ] = MakeVertexElement9( 0,    52,  D_D3DDECLTYPE_FLOAT2,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_TEXCOORD,     4 ) ;
		VertElem_[  7 ] = MakeVertexElement9( 0,    60,  D_D3DDECLTYPE_FLOAT2,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_TEXCOORD,     5 ) ;
		VertElem_[  8 ] = MakeVertexElement9( 0,    68,  D_D3DDECLTYPE_FLOAT2,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_TEXCOORD,     6 ) ;
		VertElem_[  9 ] = MakeVertexElement9( 0,    76,  D_D3DDECLTYPE_FLOAT2,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_TEXCOORD,     7 ) ;
		VertElem_[ 10 ] = MakeVertexElement9( 0xff,  0,  D_D3DDECLTYPE_UNUSED,                         0,                       0,     0 ) ;
		Direct3DDevice9_CreateVertexDeclaration( VertElem_, &GD3D9.Device.VertexDeclaration.Base2DDeclaration[ VERTEXFVF_DECL_TEX8_2D ] ) ;
	}

	// ユーザーシェーダーモードで使用する頂点データ形式を作成
	{
		D_D3DVERTEXELEMENT9 _VertElem[ 10 ] ;

		// DX_VERTEX_TYPE_NORMAL_3D ( VERTEX3D構造体形式 )
		_VertElem[ 0 ] = MakeVertexElement9( 0,     0,  D_D3DDECLTYPE_FLOAT3,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_POSITION,     0 ) ;
		_VertElem[ 1 ] = MakeVertexElement9( 0,    12,  D_D3DDECLTYPE_FLOAT3,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_NORMAL,       0 ) ;
		_VertElem[ 2 ] = MakeVertexElement9( 0,    24,  D_D3DDECLTYPE_D3DCOLOR, D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_COLOR,        0 ) ;
		_VertElem[ 3 ] = MakeVertexElement9( 0,    28,  D_D3DDECLTYPE_D3DCOLOR, D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_COLOR,        1 ) ;
		_VertElem[ 4 ] = MakeVertexElement9( 0,    32,  D_D3DDECLTYPE_FLOAT2,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_TEXCOORD,     0 ) ;
		_VertElem[ 5 ] = MakeVertexElement9( 0,    40,  D_D3DDECLTYPE_FLOAT2,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_TEXCOORD,     1 ) ;
		_VertElem[ 6 ] = MakeVertexElement9( 0xff,  0,  D_D3DDECLTYPE_UNUSED,                         0,                       0,     0 ) ;
		Direct3DDevice9_CreateVertexDeclaration( _VertElem, &GD3D9.Device.VertexDeclaration.UserShaderDeclaration[ DX_VERTEX_TYPE_NORMAL_3D ] ) ;

		// DX_VERTEX_TYPE_SHADER_3D ( VERTEX3DSHADER構造体形式 )
		_VertElem[ 0 ] = MakeVertexElement9( 0,     0,  D_D3DDECLTYPE_FLOAT3,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_POSITION,     0 ) ;
		_VertElem[ 1 ] = MakeVertexElement9( 0,    12,  D_D3DDECLTYPE_FLOAT4,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_POSITION,     1 ) ;
		_VertElem[ 2 ] = MakeVertexElement9( 0,    28,  D_D3DDECLTYPE_FLOAT3,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_NORMAL,       0 ) ;
		_VertElem[ 3 ] = MakeVertexElement9( 0,    40,  D_D3DDECLTYPE_FLOAT3,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_TANGENT,      0 ) ;
		_VertElem[ 4 ] = MakeVertexElement9( 0,    52,  D_D3DDECLTYPE_FLOAT3,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_BINORMAL,     0 ) ;
		_VertElem[ 5 ] = MakeVertexElement9( 0,    64,  D_D3DDECLTYPE_D3DCOLOR, D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_COLOR,        0 ) ;
		_VertElem[ 6 ] = MakeVertexElement9( 0,    68,  D_D3DDECLTYPE_D3DCOLOR, D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_COLOR,        1 ) ;
		_VertElem[ 7 ] = MakeVertexElement9( 0,    72,  D_D3DDECLTYPE_FLOAT2,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_TEXCOORD,     0 ) ;
		_VertElem[ 8 ] = MakeVertexElement9( 0,    80,  D_D3DDECLTYPE_FLOAT2,   D_D3DDECLMETHOD_DEFAULT, D_D3DDECLUSAGE_TEXCOORD,     1 ) ;
		_VertElem[ 9 ] = MakeVertexElement9( 0xff,  0,  D_D3DDECLTYPE_UNUSED,                         0,                       0,     0 ) ;
		Direct3DDevice9_CreateVertexDeclaration( _VertElem, &GD3D9.Device.VertexDeclaration.UserShaderDeclaration[ DX_VERTEX_TYPE_SHADER_3D ] ) ;
	}

	// 正常終了
	return 0 ;
}

// Direct3DVertexDeclaration9 オブジェクトの削除( 0:成功  -1:失敗 )
extern	int		Graphics_D3D9_VertexDeclaration_Terminate( void )
{
	int i ;

	if( GAPIWin.Direct3DDevice9Object == NULL )
		return 0 ;

	Graphics_D3D9_DeviceState_ResetVertexShader() ;
	Graphics_D3D9_DeviceState_ResetPixelShader() ;

#ifndef DX_NON_MODEL
	D_IDirect3DVertexDeclaration9 **VD ;

	VD = ( D_IDirect3DVertexDeclaration9 ** )GD3D9.Device.VertexDeclaration.MV1_VertexDeclaration ;
	for( i = 0 ; i < sizeof( GD3D9.Device.VertexDeclaration.MV1_VertexDeclaration ) / sizeof( D_IDirect3DVertexDeclaration9 * ) ; i ++ )
	{
		if( VD[ i ] )
		{
			Direct3D9_ObjectRelease( VD[ i ] ) ;
			VD[ i ] = NULL ;
		}
	}

#endif // DX_NON_MODEL

	// 固定パイプライン互換シェーダーで使用する頂点宣言の解放
	for( i = 0 ; i < VERTEXFVF_DECL_NUM ; i ++ )
	{
		if( GD3D9.Device.VertexDeclaration.Base2DDeclaration[ i ] )
		{
			Direct3D9_ObjectRelease( GD3D9.Device.VertexDeclaration.Base2DDeclaration[ i ] ) ;
			GD3D9.Device.VertexDeclaration.Base2DDeclaration[ i ] = NULL ;
		}
	}

	// ユーザーシェーダーモード用の頂点宣言の解放
	for( i = 0 ; i < DX_VERTEX_TYPE_NUM ; i ++ )
	{
		if( GD3D9.Device.VertexDeclaration.UserShaderDeclaration[ i ] )
		{
			Direct3D9_ObjectRelease( GD3D9.Device.VertexDeclaration.UserShaderDeclaration[ i ] ) ;
			GD3D9.Device.VertexDeclaration.UserShaderDeclaration[ i ] = NULL ;
		}
	}

	// 正常終了
	return 0 ;
}



















// システムテクスチャ・サーフェス関係関数

// IMAGEDATA_ORIG と IMAGEDATA_ORIG_HARD_TEX の情報を元に Graphics_D3D9_GetSysMemTexture を使用する
extern int Graphics_D3D9_GetSysMemTextureFromOrig( struct IMAGEDATA_ORIG *Orig, struct IMAGEDATA_ORIG_HARD_TEX *OrigTex, int ASyncThread )
{
	return Graphics_D3D9_GetSysMemTexture( Orig->FormatDesc.CubeMapTextureFlag, OrigTex->TexWidth, OrigTex->TexHeight, Orig->Hard.MipMapCount,  GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ], ASyncThread ) ; 
}


#ifndef DX_NON_ASYNCLOAD

static int Graphics_D3D9_GetSysMemTextureASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return Graphics_D3D9_GetSysMemTexture(
				( int )Info->Data[ 0 ],
				( int )Info->Data[ 1 ],
				( int )Info->Data[ 2 ],
				( int )Info->Data[ 3 ],
				( D_D3DFORMAT )Info->Data[ 4 ],
				FALSE ) ;
}

#endif // DX_NON_ASYNCLOAD

// 管理テクスチャへの転送用のシステムメモリテクスチャを取得する
extern int Graphics_D3D9_GetSysMemTexture( int CubeMap, int Width, int Height, int MipMapCount, D_D3DFORMAT Format, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Graphics_D3D9_GetSysMemTextureASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )CubeMap ;
		AInfo.Data[ 1 ] = ( DWORD_PTR )Width ;
		AInfo.Data[ 2 ] = ( DWORD_PTR )Height ;
		AInfo.Data[ 3 ] = ( DWORD_PTR )MipMapCount ;
		AInfo.Data[ 4 ] = ( DWORD_PTR )Format ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	int i, j ;
	GRAPHICS_HARDDATA_DIRECT3D9_SYSMEMTEXTURE *MemTex ;
	GRAPHICS_HARDDATA_DIRECT3D9_SYSMEMTEXTURE *NewMemTex ;

	// 既に適合するテクスチャで未使用のものが無いか調べる
	MemTex = GD3D9.SysMemTexSurf.Texture ;
	NewMemTex = NULL ;
	j = 0 ;
	for( i = 0 ; i < GD3D9.SysMemTexSurf.TextureInitNum ; MemTex ++ )
	{
		if( MemTex->InitializeFlag == FALSE )
		{
			if( NewMemTex == NULL )
			{
				NewMemTex = MemTex ;
			}
			continue ;
		}

		i ++ ;

		if( MemTex->CubeMap     != ( unsigned char )CubeMap ||
			MemTex->Width       != ( short )        Width ||
			MemTex->Height      != ( short )        Height ||
			MemTex->MipMapCount != ( unsigned char )MipMapCount ||
			MemTex->Format      !=                  Format )
			continue ;

		j ++ ;

		if( MemTex->UseFlag == TRUE )
		{
			// 同じフォーマットのテクスチャが規定数使用中だったらエラー
			if( j == DIRECT3D9_SYSMEMTEXTURE_ONE_FORMAT_NUM )
				return -1 ;
			continue ;
		}

		MemTex->UseCount ++ ;
		MemTex->UseFlag = TRUE ;
		return ( int )( MemTex - GD3D9.SysMemTexSurf.Texture ) ; 
	}

	// 適合するテクスチャが無く、すべてのテクスチャが初期化済みの場合は未使用中で使用頻度の低いテクスチャを解放する
	if( GD3D9.SysMemTexSurf.TextureInitNum == DIRECT3D9_SYSMEMTEXTURE_NUM )
	{
		unsigned int MinUseCount ;

		MinUseCount = 0xffffffff ;
		MemTex = GD3D9.SysMemTexSurf.Texture ;
		NewMemTex = NULL ;
		for( i = 0 ; i < DIRECT3D9_SYSMEMTEXTURE_NUM ; i ++, MemTex ++ )
		{
			if( MemTex->UseFlag == FALSE &&
				MemTex->UseCount <= MinUseCount )
			{
				MinUseCount = MemTex->UseCount ;
				NewMemTex = MemTex ;
			}
		}

		// すべてが使用中の場合はエラー
		if( NewMemTex == NULL )
		{
			return -1 ;
		}

		// 既存のデータを解放
		Direct3D9_ObjectRelease( NewMemTex->MemTexture ) ;
		NewMemTex->MemTexture = NULL ;

		NewMemTex->InitializeFlag = FALSE ;
		NewMemTex->UseCount = 0 ;
		NewMemTex->UseFlag = FALSE ;

		// 初期化データを減らす
		GD3D9.SysMemTexSurf.TextureInitNum -- ;
	}
	else
	{
		if( NewMemTex == NULL )
		{
			NewMemTex = MemTex ;
		}
	}

	// データの初期化
	NewMemTex->CubeMap = ( unsigned char )CubeMap ;
	NewMemTex->MipMapCount = ( unsigned char )MipMapCount ;
	NewMemTex->Width = ( short )Width ;
	NewMemTex->Height = ( short )Height ;
	NewMemTex->Format = Format ;
	NewMemTex->UseCount = 1 ;

	// テクスチャの作成
	if( Graphics_D3D9_CreateSysMemTextureDirect3D9Texture( NewMemTex ) < 0 )
	{
		// 使用されていないシステムメモリテクスチャを解放して、再度作成を試みる
		Graphics_D3D9_ReleaseSysMemTextureAll( TRUE ) ;
		if( Graphics_D3D9_CreateSysMemTextureDirect3D9Texture( NewMemTex ) < 0 )
		{
			return -1 ;
		}
	}

	// 初期化フラグと使用中フラグを立てる
	NewMemTex->InitializeFlag = TRUE ;
	NewMemTex->UseFlag = TRUE ;

	// 初期化データの追加
	GD3D9.SysMemTexSurf.TextureInitNum ++ ;

	return ( int )( NewMemTex - GD3D9.SysMemTexSurf.Texture ) ;
}



#ifndef DX_NON_ASYNCLOAD

static int Graphics_D3D9_GetSysMemSurfaceASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return Graphics_D3D9_GetSysMemSurface(
				( int )Info->Data[ 0 ],
				( int )Info->Data[ 1 ],
				( D_D3DFORMAT )Info->Data[ 2 ],
				FALSE ) ;
}

#endif // DX_NON_ASYNCLOAD

// 管理テクスチャへの転送用のシステムメモリテクスチャを取得する
extern int Graphics_D3D9_GetSysMemSurface( int Width, int Height, D_D3DFORMAT Format, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Graphics_D3D9_GetSysMemSurfaceASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )Width ;
		AInfo.Data[ 1 ] = ( DWORD_PTR )Height ;
		AInfo.Data[ 2 ] = ( DWORD_PTR )Format ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	int i, j ;
	GRAPHICS_HARDDATA_DIRECT3D9_SYSMEMSURFACE *MemSurface ;
	GRAPHICS_HARDDATA_DIRECT3D9_SYSMEMSURFACE *NewMemSurface ;

	// 既に適合するテクスチャで未使用のものが無いか調べる
	MemSurface = GD3D9.SysMemTexSurf.Surface ;
	NewMemSurface = NULL ;
	j = 0 ;
	for( i = 0 ; i < GD3D9.SysMemTexSurf.SurfaceInitNum ; MemSurface ++ )
	{
		if( MemSurface->InitializeFlag == FALSE )
		{
			if( NewMemSurface == NULL )
			{
				NewMemSurface = MemSurface ;
			}
			continue ;
		}

		i ++ ;

		if( MemSurface->Width       != ( short )Width ||
			MemSurface->Height      != ( short )Height ||
			MemSurface->Format      !=          Format )
			continue ;

		j ++ ;

		if( MemSurface->UseFlag == TRUE )
		{
			// 同じフォーマットのテクスチャが規定数使用中だったらエラー
			if( j == DIRECT3D9_SYSMEMSURFACE_ONE_FORMAT_NUM )
				return -1 ;
			continue ;
		}

		MemSurface->UseCount ++ ;
		MemSurface->UseFlag = TRUE ;
		return ( int )( MemSurface - GD3D9.SysMemTexSurf.Surface ) ; 
	}

	// 適合するテクスチャが無く、すべてのテクスチャが初期化済みの場合は未使用中で使用頻度の低いテクスチャを解放する
	if( GD3D9.SysMemTexSurf.SurfaceInitNum == DIRECT3D9_SYSMEMSURFACE_NUM )
	{
		unsigned int MinUseCount ;

		MinUseCount = 0xffffffff ;
		MemSurface = GD3D9.SysMemTexSurf.Surface ;
		NewMemSurface = NULL ;
		for( i = 0 ; i < DIRECT3D9_SYSMEMSURFACE_NUM ; i ++, MemSurface ++ )
		{
			if( MemSurface->UseFlag == FALSE &&
				MemSurface->UseCount <= MinUseCount )
			{
				MinUseCount = MemSurface->UseCount ;
				NewMemSurface = MemSurface ;
			}
		}

		// すべてが使用中の場合はエラー
		if( NewMemSurface == NULL )
		{
			return -1 ;
		}

		// 既存のデータを解放
		Direct3D9_ObjectRelease( NewMemSurface->MemSurface ) ;
		NewMemSurface->MemSurface = NULL ;

		NewMemSurface->InitializeFlag = FALSE ;
		NewMemSurface->UseCount = 0 ;
		NewMemSurface->UseFlag = FALSE ;

		// 初期化データを減らす
		GD3D9.SysMemTexSurf.SurfaceInitNum -- ;
	}
	else
	{
		if( NewMemSurface == NULL )
		{
			NewMemSurface = MemSurface ;
		}
	}

	// データの初期化
	NewMemSurface->Width = ( short )Width ;
	NewMemSurface->Height = ( short )Height ;
	NewMemSurface->Format = Format ;
	NewMemSurface->UseCount = 1 ;

	// テクスチャの作成
	if( Graphics_D3D9_CreateSysMemSurfaceDirect3D9Surface( NewMemSurface ) < 0 )
	{
		// 使用されていないシステムメモリテクスチャを解放して、再度作成を試みる
		Graphics_D3D9_ReleaseSysMemSurfaceAll( TRUE ) ;
		if( Graphics_D3D9_CreateSysMemSurfaceDirect3D9Surface( NewMemSurface ) < 0 )
		{
			return -1 ;
		}
	}

	// 初期化フラグと使用中フラグを立てる
	NewMemSurface->InitializeFlag = TRUE ;
	NewMemSurface->UseFlag = TRUE ;

	// 初期化データの追加
	GD3D9.SysMemTexSurf.SurfaceInitNum ++ ;

	return ( int )( NewMemSurface - GD3D9.SysMemTexSurf.Surface ) ;
}


// 構造体のデータを元に転送用のシステムメモリテクスチャを作成する
extern int Graphics_D3D9_CreateSysMemTextureDirect3D9Texture( GRAPHICS_HARDDATA_DIRECT3D9_SYSMEMTEXTURE *MemTex )
{
	long Result ;

	if( MemTex->CubeMap != 0 )
	{
		Result = Direct3DDevice9_CreateCubeTexture(
			( UINT )MemTex->Width,									// 幅
			( UINT )MemTex->MipMapCount,							// MipMap レベル
			D_D3DUSAGE_DYNAMIC,										// フラグ
			MemTex->Format,											// フォーマット
			D_D3DPOOL_SYSTEMMEM,									// テクスチャを DirectX が管理するかどうか
			( D_IDirect3DCubeTexture9 ** )&MemTex->MemTexture,		// D_IDirect3DCubeTexture9 のポインタを受け取るアドレス
			NULL													// 絶対 NULL
		) ;
		if( Result != D_D3D_OK )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xe1\x30\xe2\x30\xea\x30\xad\x30\xe5\x30\xfc\x30\xd6\x30\xde\x30\xc3\x30\xd7\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x20\x00\xa8\x30\xe9\x30\xfc\x30\xb3\x30\xfc\x30\xc9\x30\x3a\x00\x30\x00\x78\x00\x25\x00\x30\x00\x38\x00\x78\x00\x20\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x58\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x59\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x46\x00\x6f\x00\x72\x00\x6d\x00\x61\x00\x74\x00\x3a\x00\x25\x00\x64\x00\x20\x00\xcf\x7d\xe1\x30\xe2\x30\xea\x30\xba\x78\xdd\x4f\x70\x65\x3a\x00\x25\x00\x64\x00\x20\x00\xcf\x7d\xba\x78\xdd\x4f\xe1\x30\xe2\x30\xea\x30\xb5\x30\xa4\x30\xba\x30\x3a\x00\x25\x00\x64\x00\x42\x00\x79\x00\x74\x00\x65\x00\x28\x00\x25\x00\x64\x00\x4b\x00\x42\x00\x79\x00\x74\x00\x65\x00\x29\x00\x00"/*@ L"メモリキューブマップテクスチャの作成に失敗しました  エラーコード:0x%08x SizeX:%d SizeY:%d Format:%d 総メモリ確保数:%d 総確保メモリサイズ:%dByte(%dKByte)" @*/,
				Result, MemTex->Width, MemTex->Height, GD3D9.Device.Caps.TextureFormat[ MemTex->Format ], NS_DxGetAllocNum(), NS_DxGetAllocSize(), NS_DxGetAllocSize() )) ;
			return -1 ;
		}
	}
	else
	{
		Result = Direct3DDevice9_CreateTexture(
			( UINT )MemTex->Width,							// 幅
			( UINT )MemTex->Height,							// 高さ
			MemTex->MipMapCount,							// MipMap レベル
			D_D3DUSAGE_DYNAMIC,								// フラグ
			MemTex->Format,									// フォーマット
			D_D3DPOOL_SYSTEMMEM,							// テクスチャを DirectX が管理するかどうか
			( D_IDirect3DTexture9 ** )&MemTex->MemTexture,	// D_IDirect3DTexture9 のポインタを受け取るアドレス
			NULL											// 絶対 NULL
		) ;
		if( Result != D_D3D_OK )
		{
			static bool OutPutLog = false ;

			DXST_LOGFILEFMT_ADDUTF16LE(( "\xe1\x30\xe2\x30\xea\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x20\x00\xa8\x30\xe9\x30\xfc\x30\xb3\x30\xfc\x30\xc9\x30\x3a\x00\x30\x00\x78\x00\x25\x00\x30\x00\x38\x00\x78\x00\x20\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x58\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x59\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x46\x00\x6f\x00\x72\x00\x6d\x00\x61\x00\x74\x00\x3a\x00\x25\x00\x64\x00\x20\x00\xcf\x7d\xe1\x30\xe2\x30\xea\x30\xba\x78\xdd\x4f\x70\x65\x3a\x00\x25\x00\x64\x00\x20\x00\xcf\x7d\xba\x78\xdd\x4f\xe1\x30\xe2\x30\xea\x30\xb5\x30\xa4\x30\xba\x30\x3a\x00\x25\x00\x64\x00\x42\x00\x79\x00\x74\x00\x65\x00\x28\x00\x25\x00\x64\x00\x4b\x00\x42\x00\x79\x00\x74\x00\x65\x00\x29\x00\x00"/*@ L"メモリテクスチャの作成に失敗しました  エラーコード:0x%08x SizeX:%d SizeY:%d Format:%d 総メモリ確保数:%d 総確保メモリサイズ:%dByte(%dKByte)" @*/,
				Result, MemTex->Width, MemTex->Height, GD3D9.Device.Caps.TextureFormat[ MemTex->Format ], NS_DxGetAllocNum(), NS_DxGetAllocSize(), NS_DxGetAllocSize() )) ;

			if( OutPutLog == false )
			{
				OutPutLog = true ;

				DXST_LOGFILEFMT_ADDUTF16LE(( "\xe1\x30\xe2\x30\xea\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x20\x00\xa8\x30\xe9\x30\xfc\x30\xb3\x30\xfc\x30\xc9\x30\x3a\x00\x30\x00\x78\x00\x25\x00\x30\x00\x38\x00\x78\x00\x20\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x58\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x59\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x46\x00\x6f\x00\x72\x00\x6d\x00\x61\x00\x74\x00\x3a\x00\x25\x00\x64\x00\x20\x00\xcf\x7d\xe1\x30\xe2\x30\xea\x30\xba\x78\xdd\x4f\x70\x65\x3a\x00\x25\x00\x64\x00\x20\x00\xcf\x7d\xba\x78\xdd\x4f\xe1\x30\xe2\x30\xea\x30\xb5\x30\xa4\x30\xba\x30\x3a\x00\x25\x00\x64\x00\x42\x00\x79\x00\x74\x00\x65\x00\x28\x00\x25\x00\x64\x00\x4b\x00\x42\x00\x79\x00\x74\x00\x65\x00\x29\x00\x00"/*@ L"メモリテクスチャの作成に失敗しました  エラーコード:0x%08x SizeX:%d SizeY:%d Format:%d 総メモリ確保数:%d 総確保メモリサイズ:%dByte(%dKByte)" @*/,
					Result, MemTex->Width, MemTex->Height, GD3D9.Device.Caps.TextureFormat[ MemTex->Format ], NS_DxGetAllocNum(), NS_DxGetAllocSize(), NS_DxGetAllocSize() )) ;
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x5e\x97\xa1\x7b\x06\x74\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x78\x30\x6e\x30\xc7\x30\xfc\x30\xbf\x30\xe2\x8e\x01\x90\x28\x75\xb7\x30\xb9\x30\xc6\x30\xe0\x30\xe1\x30\xe2\x30\xea\x30\x4d\x91\x6e\x7f\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\x70\x65\x1a\xff\x25\x00\x64\x00\x00"/*@ L"非管理テクスチャへのデータ転送用システムメモリ配置テクスチャの数：%d" @*/, GD3D9.SysMemTexSurf.TextureInitNum )) ;

				int i, j ;
				MemTex = GD3D9.SysMemTexSurf.Texture ;
				j = 0 ;
				for( i = 0 ; i < GD3D9.SysMemTexSurf.TextureInitNum ; j ++ , MemTex ++ )
				{
					if( MemTex->InitializeFlag == FALSE )
					{
						continue ;
					}
					i++ ;
					DXST_LOGFILEFMT_ADDW(( L"No%d. UseFlag:%d UseCount:%d CubeMap:%d SizeX:%d SizeY:%d MipMapCount:%d Format:%d",
						j, MemTex->UseFlag, MemTex->UseCount, MemTex->CubeMap, MemTex->Width, MemTex->Height, MemTex->MipMapCount, MemTex->Format )) ;
				}
			}
			return -1 ;
		}
	}

	// 正常終了
	return 0 ;
}


// 構造体のデータを元に転送用のシステムメモリサーフェスを作成する
extern int Graphics_D3D9_CreateSysMemSurfaceDirect3D9Surface( GRAPHICS_HARDDATA_DIRECT3D9_SYSMEMSURFACE *MemSurface )
{
	long Result ;

	Result = Direct3DDevice9_CreateOffscreenPlainSurface(
		( UINT )MemSurface->Width,							// 幅
		( UINT )MemSurface->Height,							// 高さ
		MemSurface->Format,									// フォーマット
		D_D3DPOOL_SYSTEMMEM,								// テクスチャを DirectX が管理するかどうか
		( D_IDirect3DSurface9 ** )&MemSurface->MemSurface,	// D_IDirect3DSurface9 のポインタを受け取るアドレス
		NULL												// 絶対 NULL
	) ;
	if( Result != D_D3D_OK )
	{
		static bool OutPutLog = false ;

		DXST_LOGFILEFMT_ADDUTF16LE(( "\xe1\x30\xe2\x30\xea\x30\xb5\x30\xfc\x30\xd5\x30\xa7\x30\xb9\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x50\x00\x6f\x00\x73\x00\x4e\x00\x6f\x00\x2e\x00\x31\x00\x20\x00\x20\x00\xa8\x30\xe9\x30\xfc\x30\xb3\x30\xfc\x30\xc9\x30\x3a\x00\x30\x00\x78\x00\x25\x00\x30\x00\x38\x00\x78\x00\x20\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x58\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x59\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x46\x00\x6f\x00\x72\x00\x6d\x00\x61\x00\x74\x00\x3a\x00\x25\x00\x64\x00\x20\x00\xcf\x7d\xe1\x30\xe2\x30\xea\x30\xba\x78\xdd\x4f\x70\x65\x3a\x00\x25\x00\x64\x00\x20\x00\xcf\x7d\xba\x78\xdd\x4f\xe1\x30\xe2\x30\xea\x30\xb5\x30\xa4\x30\xba\x30\x3a\x00\x25\x00\x64\x00\x42\x00\x79\x00\x74\x00\x65\x00\x28\x00\x25\x00\x64\x00\x4b\x00\x42\x00\x79\x00\x74\x00\x65\x00\x29\x00\x00"/*@ L"メモリサーフェスの作成に失敗しました PosNo.1  エラーコード:0x%08x SizeX:%d SizeY:%d Format:%d 総メモリ確保数:%d 総確保メモリサイズ:%dByte(%dKByte)" @*/,
			Result, MemSurface->Width, MemSurface->Height, MemSurface->Format, NS_DxGetAllocNum(), NS_DxGetAllocSize(), NS_DxGetAllocSize() / 1024 )) ;

		if( OutPutLog == false )
		{
			OutPutLog = true ;

			DXST_LOGFILEFMT_ADDUTF16LE(( "\xe1\x30\xe2\x30\xea\x30\xb5\x30\xfc\x30\xd5\x30\xa7\x30\xb9\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x50\x00\x6f\x00\x73\x00\x4e\x00\x6f\x00\x2e\x00\x32\x00\x20\x00\x20\x00\xa8\x30\xe9\x30\xfc\x30\xb3\x30\xfc\x30\xc9\x30\x3a\x00\x30\x00\x78\x00\x25\x00\x30\x00\x38\x00\x78\x00\x20\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x58\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x59\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x46\x00\x6f\x00\x72\x00\x6d\x00\x61\x00\x74\x00\x3a\x00\x25\x00\x64\x00\x20\x00\xcf\x7d\xe1\x30\xe2\x30\xea\x30\xba\x78\xdd\x4f\x70\x65\x3a\x00\x25\x00\x64\x00\x20\x00\xcf\x7d\xba\x78\xdd\x4f\xe1\x30\xe2\x30\xea\x30\xb5\x30\xa4\x30\xba\x30\x3a\x00\x25\x00\x64\x00\x42\x00\x79\x00\x74\x00\x65\x00\x28\x00\x25\x00\x64\x00\x4b\x00\x42\x00\x79\x00\x74\x00\x65\x00\x29\x00\x00"/*@ L"メモリサーフェスの作成に失敗しました PosNo.2  エラーコード:0x%08x SizeX:%d SizeY:%d Format:%d 総メモリ確保数:%d 総確保メモリサイズ:%dByte(%dKByte)" @*/,
				Result, MemSurface->Width, MemSurface->Height, MemSurface->Format, NS_DxGetAllocNum(), NS_DxGetAllocSize(), NS_DxGetAllocSize() / 1024 )) ;
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x5e\x97\xa1\x7b\x06\x74\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x78\x30\x6e\x30\xc7\x30\xfc\x30\xbf\x30\xe2\x8e\x01\x90\x28\x75\xb7\x30\xb9\x30\xc6\x30\xe0\x30\xe1\x30\xe2\x30\xea\x30\x4d\x91\x6e\x7f\xb5\x30\xfc\x30\xd5\x30\xa7\x30\xb9\x30\x6e\x30\x70\x65\x1a\xff\x25\x00\x64\x00\x00"/*@ L"非管理テクスチャへのデータ転送用システムメモリ配置サーフェスの数：%d" @*/, GD3D9.SysMemTexSurf.TextureInitNum )) ;

			int i, j ;
			MemSurface = GD3D9.SysMemTexSurf.Surface ;
			j = 0 ;
			for( i = 0 ; i < GD3D9.SysMemTexSurf.SurfaceInitNum ; j ++ , MemSurface ++ )
			{
				if( MemSurface->InitializeFlag == FALSE )
				{
					continue ;
				}
				i++ ;
				DXST_LOGFILEFMT_ADDW(( L"No%d. UseFlag:%d UseCount:%d SizeX:%d SizeY:%d Format:%d",
					j, MemSurface->UseFlag, MemSurface->UseCount, MemSurface->Width, MemSurface->Height, MemSurface->Format )) ;
			}
		}
		return -1 ;
	}

	// 正常終了
	return 0 ;
}


#ifndef DX_NON_ASYNCLOAD

static int Graphics_D3D9_ReleaseSysMemTextureASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return Graphics_D3D9_ReleaseSysMemTexture(
				( int )Info->Data[ 0 ],
				FALSE ) ;
}

#endif // DX_NON_ASYNCLOAD

// 管理テクスチャへの転送用のシステムメモリテクスチャの取得を解放する
extern int Graphics_D3D9_ReleaseSysMemTexture( int Index, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Graphics_D3D9_ReleaseSysMemTextureASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )Index ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	GRAPHICS_HARDDATA_DIRECT3D9_SYSMEMTEXTURE *MemTex ;

	MemTex = &GD3D9.SysMemTexSurf.Texture[ Index ] ;

	if( MemTex->InitializeFlag == FALSE ||
		MemTex->UseFlag == FALSE )
		return 0 ;

	MemTex->UseFlag = FALSE ;
	MemTex->UseTime = NS_GetNowCount() ;

	// 終了
	return 0 ;
}


#ifndef DX_NON_ASYNCLOAD

static int Graphics_D3D9_ReleaseSysMemSurfaceASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return Graphics_D3D9_ReleaseSysMemSurface(
				( int )Info->Data[ 0 ],
				FALSE ) ;
}

#endif // DX_NON_ASYNCLOAD

// 管理テクスチャへの転送用のシステムメモリサーフェスの取得を解放する
extern int Graphics_D3D9_ReleaseSysMemSurface( int Index, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Graphics_D3D9_ReleaseSysMemSurfaceASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )Index ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	GRAPHICS_HARDDATA_DIRECT3D9_SYSMEMSURFACE *MemSurface ;

	MemSurface = &GD3D9.SysMemTexSurf.Surface[ Index ] ;

	if( MemSurface->InitializeFlag == FALSE ||
		MemSurface->UseFlag == FALSE )
		return 0 ;

	MemSurface->UseFlag = FALSE ;
	MemSurface->UseTime = NS_GetNowCount() ;

	// 終了
	return 0 ;
}


// 管理テクスチャへの転送用のシステムメモリテクスチャをすべて解放する
extern int Graphics_D3D9_ReleaseSysMemTextureAll( int IsNotUseOnly )
{
	int i ;
	int ReleaseCount ;
	GRAPHICS_HARDDATA_DIRECT3D9_SYSMEMTEXTURE *MemTex ;

	ReleaseCount = 0 ;
	MemTex = GD3D9.SysMemTexSurf.Texture ;
	for( i = 0 ; i < GD3D9.SysMemTexSurf.TextureInitNum ; MemTex ++ )
	{
		if( MemTex->InitializeFlag == FALSE )
		{
			continue ;
		}

		i ++ ;

		if( IsNotUseOnly && MemTex->UseFlag )
		{
			continue ;
		}

		if( MemTex->MemTexture != NULL )
		{
			Direct3D9_ObjectRelease( MemTex->MemTexture ) ;
			MemTex->MemTexture = NULL ;
		}

		MemTex->InitializeFlag = FALSE ;
		MemTex->UseCount = 0 ;
		MemTex->UseFlag = FALSE ;

		ReleaseCount ++ ;
	}
	GD3D9.SysMemTexSurf.TextureInitNum -= ReleaseCount ;

	// 終了
	return 0 ;
}

// 管理テクスチャへの転送用のシステムメモリサーフェスをすべて解放する
extern int Graphics_D3D9_ReleaseSysMemSurfaceAll( int IsNotUseOnly )
{
	int i ;
	int ReleaseCount ;
	GRAPHICS_HARDDATA_DIRECT3D9_SYSMEMSURFACE *MemSurface ;

	ReleaseCount = 0 ;
	MemSurface = GD3D9.SysMemTexSurf.Surface ;
	for( i = 0 ; i < GD3D9.SysMemTexSurf.SurfaceInitNum ; MemSurface ++ )
	{
		if( MemSurface->InitializeFlag == FALSE )
		{
			continue ;
		}

		i ++ ;

		if( IsNotUseOnly && MemSurface->UseFlag )
		{
			continue ;
		}

		if( MemSurface->MemSurface != NULL )
		{
			Direct3D9_ObjectRelease( MemSurface->MemSurface ) ;
			MemSurface->MemSurface = NULL ;
		}

		MemSurface->InitializeFlag = FALSE ;
		MemSurface->UseCount = 0 ;
		MemSurface->UseFlag = FALSE ;

		ReleaseCount ++ ;
	}
	GD3D9.SysMemTexSurf.SurfaceInitNum -= ReleaseCount ;

	// 終了
	return 0 ;
}


// 管理テクスチャへの転送用のシステムメモリテクスチャの定期処理を行う
extern int Graphics_D3D9_SysMemTextureProcess( void )
{
	int i ;
	int ReleaseCount ;
	GRAPHICS_HARDDATA_DIRECT3D9_SYSMEMTEXTURE *MemTex ;
	int NowTime ;

#ifndef DX_NON_ASYNCLOAD
	// メインスレッド以外から呼ばれた場合は何もしない
	if( CheckMainThread() == FALSE )
	{
		return -1 ;
	}
#endif // DX_NON_ASYNCLOAD

	NowTime = NS_GetNowCount() ;

	MemTex = GD3D9.SysMemTexSurf.Texture ;
	ReleaseCount = 0 ;
	for( i = 0 ; i < GD3D9.SysMemTexSurf.TextureInitNum ; MemTex ++ )
	{
		if( MemTex->InitializeFlag == FALSE )
		{
			continue ;
		}

		i ++ ;

		// 最後に使用してから２秒以上経過していたら解放する
		if( MemTex->UseFlag || NowTime - MemTex->UseTime < 2000 )
		{
			continue ;
		}

		if( MemTex->MemTexture != NULL )
		{
			Direct3D9_ObjectRelease( MemTex->MemTexture ) ;
			MemTex->MemTexture = NULL ;
		}

		MemTex->InitializeFlag = FALSE ;
		MemTex->UseCount = 0 ;
		MemTex->UseFlag = FALSE ;

		ReleaseCount ++ ;
	}
	GD3D9.SysMemTexSurf.TextureInitNum -= ReleaseCount ;

	// 終了
	return 0 ;
}


// 管理テクスチャへの転送用のシステムメモリサーフェスの定期処理を行う
extern int Graphics_D3D9_SysMemSurfaceProcess( void )
{
	int i ;
	int ReleaseCount ;
	GRAPHICS_HARDDATA_DIRECT3D9_SYSMEMSURFACE *MemSurface ;
	int NowTime ;

#ifndef DX_NON_ASYNCLOAD
	// メインスレッド以外から呼ばれた場合は何もしない
	if( CheckMainThread() == FALSE )
	{
		return -1 ;
	}
#endif // DX_NON_ASYNCLOAD

	NowTime = NS_GetNowCount() ;

	MemSurface = GD3D9.SysMemTexSurf.Surface ;
	ReleaseCount = 0 ;
	for( i = 0 ; i < GD3D9.SysMemTexSurf.SurfaceInitNum ; MemSurface ++ )
	{
		if( MemSurface->InitializeFlag == FALSE )
		{
			continue ;
		}

		i ++ ;

		// 最後に使用してから２秒以上経過していたら解放する
		if( MemSurface->UseFlag || NowTime - MemSurface->UseTime < 2000 )
		{
			continue ;
		}

		if( MemSurface->MemSurface != NULL )
		{
			Direct3D9_ObjectRelease( MemSurface->MemSurface ) ;
			MemSurface->MemSurface = NULL ;
		}

		MemSurface->InitializeFlag = FALSE ;
		MemSurface->UseCount = 0 ;
		MemSurface->UseFlag = FALSE ;

		ReleaseCount ++ ;
	}
	GD3D9.SysMemTexSurf.SurfaceInitNum -= ReleaseCount ;

	// 終了
	return 0 ;
}






























// Direct3D9 の描画処理準備関係

#ifndef DX_NON_ASYNCLOAD
static int Graphics_D3D9_RenderVertexASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO * /*Info*/ )
{
	return Graphics_D3D9_RenderVertex( FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

// 頂点バッファに溜まった頂点データをレンダリングする
extern	int		Graphics_D3D9_RenderVertex( int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Graphics_D3D9_RenderVertexASyncCallback ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	// 頂点が一つも無かったら描画は行わない
	if( GD3D9.Device.DrawInfo.VertexNum != 0 && DxLib_GetEndRequest() == FALSE )
	{
		// Graphics_D3D9_BeginScene をしていなかったらする
		if( GD3D9.Device.DrawInfo.BeginSceneFlag == FALSE ) Graphics_D3D9_BeginScene() ;

		// 非描画フラグが立っていなければレンダリングする
		if( GD3D9.Device.DrawInfo.BlendMaxNotDrawFlag == FALSE )
		{
			UINT PrimNum ;

			PrimNum = 0 ;
			switch( GD3D9.Device.DrawInfo.PrimitiveType )
			{
			case D_D3DPT_POINTLIST     : PrimNum = ( UINT )( GD3D9.Device.DrawInfo.VertexNum     ) ; break ;
			case D_D3DPT_LINELIST      : PrimNum = ( UINT )( GD3D9.Device.DrawInfo.VertexNum / 2 ) ; break ;
			case D_D3DPT_LINESTRIP     : PrimNum = ( UINT )( GD3D9.Device.DrawInfo.VertexNum - 1 ) ; break ;
			case D_D3DPT_TRIANGLELIST  : PrimNum = ( UINT )( GD3D9.Device.DrawInfo.VertexNum / 3 ) ; break ;
			case D_D3DPT_TRIANGLESTRIP : PrimNum = ( UINT )( GD3D9.Device.DrawInfo.VertexNum - 2 ) ; break ;
			case D_D3DPT_TRIANGLEFAN   : PrimNum = ( UINT )( GD3D9.Device.DrawInfo.VertexNum - 2 ) ; break ;
			}

//			SETFVF( D3DDev_VertexType[ GD3D9.Device.DrawInfo.Use3DVertex ][ GD3D9.Device.DrawInfo.VertexType ] );
//			Graphics_D3D9_DeviceState_SetFVF( D3DDev_VertexType[ GD3D9.Device.DrawInfo.Use3DVertex ][ GD3D9.Device.DrawInfo.VertexType ] ) ;
			Graphics_Hardware_D3D9_SetFVF_UseTable( D3DDev_VertexDeclaration[ GD3D9.Device.DrawInfo.Use3DVertex ][ GD3D9.Device.DrawInfo.VertexType ] ) ;
//			if( GD3D9.Device.DrawInfo.Use3DVertex )
//			{
//				Graphics_D3D9_DeviceState_SetWorldMatrix( &GlobalIdentMatrix ) ;
//			}
			Direct3DDevice9_DrawPrimitiveUP(
				GD3D9.Device.DrawInfo.PrimitiveType,
				PrimNum,
				GD3D9.Device.DrawInfo.VertexBufferAddr[ GD3D9.Device.DrawInfo.Use3DVertex ][ GD3D9.Device.DrawInfo.VertexType ],
				D3DDev_VertexSize[ GD3D9.Device.DrawInfo.Use3DVertex ][ GD3D9.Device.DrawInfo.VertexType ] ) ;

//			if( GD3D9.Device.DrawInfo.Use3DVertex )
//			{
//				Graphics_D3D9_DeviceState_SetWorldMatrix( &GSYS.DrawSetting.WorldMatrixF ) ;
//			}
//			RenderVertexDrawPrimitive_PF() ;
			GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;
		}
	}

	GD3D9.Device.DrawInfo.VertexNum = 0 ;
	GD3D9.Device.DrawInfo.VertexBufferNextAddr = GD3D9.Device.DrawInfo.VertexBufferAddr[ GD3D9.Device.DrawInfo.Use3DVertex ][ GD3D9.Device.DrawInfo.VertexType ] ;

	// 終了
	return 0 ;
}

// 描画準備を行う
extern	void	FASTCALL Graphics_D3D9_DrawPreparation( int ParamFlag )
{
	int AlphaTest ;
	int AlphaChannel ;
	int UseDiffuseRGB ;
	int UseDiffuseAlpha ;
	int ShadeMode ;
	int Specular ;
	int UseShader ;
	int Is3D ;
//	int EdgeFont ;

	// 縁付きフォントの描画かどうかのフラグを取得
//	EdgeFont = ( ParamFlag & DX_D3D9_DRAWPREP_EDGEFONT ) != 0 ? TRUE : FALSE ;

	// 必ず Graphics_D3D9_DrawPreparation を行うべきというフラグを倒す
	GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = FALSE ;

	// 設定が変更されたかどうかのフラグを倒す
	GSYS.ChangeSettingFlag = FALSE ;

	UseShader = GSYS.HardInfo.UseShader && GD3D9.NormalDraw_NotUsePixelShader == FALSE ;
	Is3D      = ( ParamFlag & DX_D3D9_DRAWPREP_3D ) != 0 ;

	// シェーダーがセットされていたら外す
	if( ( ParamFlag & DX_D3D9_DRAWPREP_NOTSHADERRESET ) == 0 )
	{
		Graphics_D3D9_DeviceState_ResetVertexShader( TRUE ) ;
		Graphics_D3D9_DeviceState_ResetPixelShader( TRUE ) ;
	}

	Graphics_D3D9_DeviceState_SetIndexBuffer( NULL ) ;
	Graphics_D3D9_DeviceState_SetVertexBuffer( NULL ) ;

	// ライトを使うかどうかで処理を分岐
	if( ( ParamFlag & DX_D3D9_DRAWPREP_LIGHTING ) != 0 && GSYS.Light.ProcessDisable == FALSE )
	{
		// ライトの設定が変更されていたら変更を適応する
		if( GSYS.Light.HardwareChange )
		{
			Graphics_Light_RefreshState() ;
		}
		else
		{
			// ライトが有効なのに一時的に無効になっていたら有効にする
			if( GD3D9.Device.State.Lighting == 0 )
			{
				Graphics_D3D9_DeviceState_SetLighting( TRUE ) ;
			}
		}

		// マテリアルの設定が外部から変更されていたら元に戻す
		if( GSYS.Light.ChangeMaterial )
		{
			GSYS.Light.ChangeMaterial = 0 ;
			Graphics_D3D9_DeviceState_SetMaterial( ( D_D3DMATERIAL9 * )&GSYS.Light.Material ) ;
		}

		// 頂点カラーをマテリアルのカラーとして使用するかどうかをセットする
		if( ( GSYS.Light.MaterialNotUseVertexDiffuseColor ? FALSE : TRUE ) != GD3D9.Device.State.MaterialUseVertexDiffuseColor )
		{
			Graphics_D3D9_DeviceState_SetDiffuseMaterialSource( GSYS.Light.MaterialNotUseVertexDiffuseColor ? FALSE : TRUE ) ;
		}

		if( ( GSYS.Light.MaterialNotUseVertexSpecularColor ? FALSE : TRUE ) != GD3D9.Device.State.MaterialUseVertexSpecularColor )
		{
			Graphics_D3D9_DeviceState_SetSpecularMaterialSource( GSYS.Light.MaterialNotUseVertexSpecularColor ? FALSE : TRUE ) ;
		}
	}
	else
	{
		// ライトが有効になっていたら無効にする
		if( GD3D9.Device.State.Lighting == 1 )
		{
			Graphics_D3D9_DeviceState_SetLighting( FALSE ) ;
		}
	}

	// スペキュラを使用するかどうかの設定を行う
	Specular = ( GSYS.DrawSetting.NotUseSpecular == FALSE && ( ParamFlag & DX_D3D9_DRAWPREP_SPECULAR ) != 0 ) ? TRUE : FALSE ;
	if( Specular != GD3D9.Device.State.UseSpecular )
	{
		Graphics_D3D9_DeviceState_SetSpecularEnable( Specular ) ;
	}

	// 射影行列の設定を行う
//	Graphics_DrawSetting_SetUse2DProjectionMatrix( ( ParamFlag & DX_D3D9_DRAWPREP_3D ) == 0 ? TRUE : FALSE ) ;

	// Ｚバッファの設定を行う
	if( Is3D )
	{
		if( GSYS.DrawSetting.EnableZBufferFlag3D )
		{
			if( GD3D9.Device.State.ZEnable      != GSYS.DrawSetting.EnableZBufferFlag3D ) Graphics_D3D9_DeviceState_SetZEnable(      GSYS.DrawSetting.EnableZBufferFlag3D ) ;
			if( GD3D9.Device.State.ZWriteEnable != GSYS.DrawSetting.WriteZBufferFlag3D  ) Graphics_D3D9_DeviceState_SetZWriteEnable( GSYS.DrawSetting.WriteZBufferFlag3D  ) ;
			if( GD3D9.Device.State.ZFunc        != GSYS.DrawSetting.ZBufferCmpType3D    ) Graphics_D3D9_DeviceState_SetZFunc(        GSYS.DrawSetting.ZBufferCmpType3D    ) ;
			if( GD3D9.Device.State.DepthBias    != GSYS.DrawSetting.ZBias3D             ) Graphics_D3D9_DeviceState_SetDepthBias(    GSYS.DrawSetting.ZBias3D             ) ;
		}
		else
		{
			if( GD3D9.Device.State.ZEnable ) Graphics_D3D9_DeviceState_SetZEnable( FALSE ) ;
		}
	}
	else
	{
		if( GSYS.DrawSetting.EnableZBufferFlag2D )
		{
			if( GD3D9.Device.State.ZEnable      != GSYS.DrawSetting.EnableZBufferFlag2D ) Graphics_D3D9_DeviceState_SetZEnable(      GSYS.DrawSetting.EnableZBufferFlag2D ) ;
			if( GD3D9.Device.State.ZWriteEnable != GSYS.DrawSetting.WriteZBufferFlag2D  ) Graphics_D3D9_DeviceState_SetZWriteEnable( GSYS.DrawSetting.WriteZBufferFlag2D  ) ;
			if( GD3D9.Device.State.ZFunc        != GSYS.DrawSetting.ZBufferCmpType2D    ) Graphics_D3D9_DeviceState_SetZFunc(        GSYS.DrawSetting.ZBufferCmpType2D    ) ;
			if( GD3D9.Device.State.DepthBias    != GSYS.DrawSetting.ZBias2D             ) Graphics_D3D9_DeviceState_SetDepthBias(    GSYS.DrawSetting.ZBias2D             ) ;
		}
		else
		{
			if( GD3D9.Device.State.ZEnable      ) Graphics_D3D9_DeviceState_SetZEnable( FALSE ) ;
			if( GD3D9.Device.State.ZWriteEnable ) Graphics_D3D9_DeviceState_SetZWriteEnable( FALSE ) ;
		}
	}

	// フィルモードのセット
	if( GD3D9.Device.State.FillMode != GSYS.DrawSetting.FillMode ) Graphics_D3D9_DeviceState_SetFillMode( GSYS.DrawSetting.FillMode ) ;

	// フォグの設定を行う
	if( ParamFlag & DX_D3D9_DRAWPREP_FOG )
	{
		if( GD3D9.Device.State.FogEnable != GSYS.DrawSetting.FogEnable ) Graphics_D3D9_DeviceState_SetFogEnable( GSYS.DrawSetting.FogEnable ) ;
	}
	else
	{
		if( GD3D9.Device.State.FogEnable ) Graphics_D3D9_DeviceState_SetFogEnable( FALSE ) ;
	}

	// ブレンディング関係のセッティングを行う場合のみ実行する
	if( ( ParamFlag & DX_D3D9_DRAWPREP_NOBLENDSETTING ) == 0 )
	{
		// 前回とまったく同じ場合は何もせずに終了
//		if( Format    == GRH.DrawPrepFormat   &&
//			Texture   == GRH.DrawPrepTexture  &&
//			ParamFlag == GD3D9.Device.DrawSetting.DrawPrepParamFlag ) return ;

		// フラグの初期化
		AlphaTest    = FALSE ;
		AlphaChannel = FALSE ;

		// テクスチャーを使用するかどうかで処理を分岐
		// 使用しない場合は初期値のまま
		if( ParamFlag & DX_D3D9_DRAWPREP_TEXTURE /* Texture */ )
		{
			// 透過色処理を行わない場合はカラーキーもαテストもαチャンネルも使用しないので初期値のまま
			// 透過色処理を行う場合のみ処理をする
			if( ParamFlag & DX_D3D9_DRAWPREP_TRANS )
			{
				int TexAlphaTestFlag = ( ParamFlag & DX_D3D9_DRAWPREP_TEXALPHATEST ) != 0 ? 1 : 0 ;
				int TexAlphaChFlag   = ( ParamFlag & DX_D3D9_DRAWPREP_TEXALPHACH   ) != 0 ? 1 : 0 ;

				// テクスチャにαビットがあるかどうかで処理を分岐
				if( TexAlphaTestFlag || TexAlphaChFlag )
				{
					// テクスチャにαビットがある場合カラーキーは使用しない(ので初期値のまま)
/*
					// αテストでも頂点座標のデータ型が浮動小数点型で、且つテクスチャフィルタリングモードが
					// 線形補間だった場合はαチャンネルとして扱う
					if( VectorIntFlag == FALSE && GSYS.DrawSetting.DrawMode == DX_DRAWMODE_BILINEAR )
					{
						AlphaChannel = TRUE ;
					}
					else
					{
						// それ以外の場合はテクスチャーのフラグに委ねる
						AlphaTest = Format->AlphaTestFlag ;
						AlphaChannel = Format->AlphaChFlag ;
					}
*/
					// αテストでも頂点座標のデータ型が浮動小数点型で、且つテクスチャフィルタリングモードが
					// 線形補間であるか、ブレンドモードが DX_BLENDMODE_NOBLEND 以外だったらαチャンネルとして扱う
					if( ( GSYS.DrawSetting.BlendMode != DX_BLENDMODE_NOBLEND && GSYS.DrawSetting.BlendMode != DX_BLENDMODE_DESTCOLOR ) ||
						( ( ParamFlag & DX_D3D9_DRAWPREP_VECTORINT ) == 0 && GSYS.DrawSetting.DrawMode == DX_DRAWMODE_BILINEAR ) )
//					if( VectorIntFlag == FALSE && GSYS.DrawSetting.DrawMode == DX_DRAWMODE_BILINEAR )
					{
						AlphaChannel = TRUE ;
					}
					else
					{
						// それ以外の場合はテクスチャーのフラグに委ねる
						AlphaTest    = TexAlphaTestFlag ;
						AlphaChannel = TexAlphaChFlag ;
					}
				}
			}
		}

		// ディフーズカラーを使用するかどうかをセット
		UseDiffuseRGB   = ( ParamFlag & DX_D3D9_DRAWPREP_DIFFUSERGB )   || ( GSYS.DrawSetting.bDrawBright & 0xffffff ) != 0xffffff || GSYS.DrawSetting.AlwaysDiffuseColorFlag ;
		UseDiffuseAlpha = ( ParamFlag & DX_D3D9_DRAWPREP_DIFFUSEALPHA ) || ( GSYS.DrawSetting.BlendMode != DX_BLENDMODE_NOBLEND || GSYS.DrawSetting.UseNoBlendModeParam /*&& ( GSYS.DrawSetting.BlendMode != DX_BLENDMODE_ALPHA || GSYS.DrawSetting.BlendParam != 255 )*/ ) ;

		// 調整されたパラメータをセットする
		if( UseDiffuseRGB             != GD3D9.Device.DrawSetting.UseDiffuseRGBColor     ) Graphics_D3D9_DrawSetting_SetUseDiffuseRGBColorFlag(   UseDiffuseRGB   ) ;
		if( UseDiffuseAlpha           != GD3D9.Device.DrawSetting.UseDiffuseAlphaColor   ) Graphics_D3D9_DrawSetting_SetUseDiffuseAlphaColorFlag( UseDiffuseAlpha ) ;
//		if( Texture                   != GD3D9.Device.DrawSetting.RenderTexture          ) Graphics_D3D9_DrawSetting_SetTexture(                  Texture         ) ;
		if( GD3D9.Device.DrawSetting.IgnoreGraphColorFlag  != GSYS.DrawSetting.IgnoreGraphColorFlag ) Graphics_D3D9_DrawSetting_SetIgnoreDrawGraphColor( GSYS.DrawSetting.IgnoreGraphColorFlag ) ;
		if( GD3D9.Device.DrawSetting.BlendMode             != GSYS.DrawSetting.BlendMode ||
			GD3D9.Device.DrawSetting.AlphaTestValidFlag    != AlphaTest ||
			GD3D9.Device.DrawSetting.AlphaChannelValidFlag != AlphaChannel               ) Graphics_D3D9_DrawSetting_SetDrawBlendMode( GSYS.DrawSetting.BlendMode, AlphaTest, AlphaChannel ) ;
//		if( GRH.EdgeFontDrawFlag      != EdgeFont                   ) SetEdgeFontDrawFlagHardware( EdgeFont ) ;
		if( GD3D9.Device.DrawSetting.AlphaTestMode         != GSYS.DrawSetting.AlphaTestMode ||
			GD3D9.Device.DrawSetting.AlphaTestParam        != GSYS.DrawSetting.AlphaTestParam       ) Graphics_D3D9_DrawSetting_SetDrawAlphaTest( GSYS.DrawSetting.AlphaTestMode, GSYS.DrawSetting.AlphaTestParam ) ;
		if( GD3D9.Device.DrawSetting.ChangeBlendParamFlag || GD3D9.Device.DrawSetting.ChangeTextureFlag ||
			( UseShader && GD3D9.Device.State.SetNormalPixelShader == FALSE && ( Is3D == FALSE || Specular == FALSE ) ) ||
			( UseShader && GD3D9.Device.State.SetNormalPixelShader == TRUE  && ( Is3D == TRUE  && Specular          ) ) )
		{
			Graphics_D3D9_DeviceState_RefreshBlendState( NULL, 0, 1, Is3D == FALSE || Specular == FALSE ) ;
		}
	}
	else
	{
		if( ( UseShader && GD3D9.Device.State.SetNormalPixelShader == FALSE && ( Is3D == FALSE || Specular == FALSE ) ) ||
			( UseShader && GD3D9.Device.State.SetNormalPixelShader == TRUE  && ( Is3D == TRUE  && Specular          ) ) )
		{
			Graphics_D3D9_DeviceState_RefreshBlendState( NULL, 0, 1, Is3D == FALSE || Specular == FALSE ) ;
		}
	}

	// シェーディングモードのセット
	ShadeMode = ( ParamFlag & DX_D3D9_DRAWPREP_GOURAUDSHADE ) ? D_D3DSHADE_GOURAUD : D_D3DSHADE_FLAT ;

	if( GD3D9.Device.State.ShadeMode      != ShadeMode            ) Graphics_D3D9_DeviceState_SetShadeMode(      ShadeMode           ) ;
	if( GD3D9.Device.State.DrawMode       != GSYS.DrawSetting.DrawMode       ) Graphics_D3D9_DeviceState_SetDrawMode(       GSYS.DrawSetting.DrawMode      ) ;
	if( GD3D9.Device.State.MaxAnisotropy  != GSYS.DrawSetting.MaxAnisotropy  ) Graphics_D3D9_DeviceState_SetMaxAnisotropy(  GSYS.DrawSetting.MaxAnisotropy ) ;
	if( ParamFlag & DX_D3D9_DRAWPREP_CULLING )
	{
		if( GD3D9.Device.State.CullMode   != GSYS.DrawSetting.CullMode       ) Graphics_D3D9_DeviceState_SetCullMode(     GSYS.DrawSetting.CullMode      ) ;
	}
	else
	{
		if( GD3D9.Device.State.CullMode   != DX_CULLING_NONE      ) Graphics_D3D9_DeviceState_SetCullMode(     DX_CULLING_NONE     ) ;
	}
//	if( GRH.TextureTransformMatrixDirectChange     )
	if( GSYS.DrawSetting.MatchHardwareTextureAddressTransformMatrix == FALSE )
	{
		Graphics_D3D9_DeviceState_SetTextureAddressTransformMatrix( GSYS.DrawSetting.TextureAddressTransformUse, &GSYS.DrawSetting.TextureAddressTransformMatrix, -1 ) ;
		GSYS.DrawSetting.MatchHardwareTextureAddressTransformMatrix = TRUE ;
//		GRH.TextureTransformMatrixDirectChange = FALSE ;
		GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;
	}
	if( ParamFlag & DX_D3D9_DRAWPREP_TEXADDRESS )
	{
		if( GD3D9.Device.State.TexAddressModeU[ 0 ] != GSYS.DrawSetting.TexAddressModeU[ 0 ] ) Graphics_D3D9_DeviceState_SetTextureAddressU( GSYS.DrawSetting.TexAddressModeU[ 0 ], 0 ) ;
		if( GD3D9.Device.State.TexAddressModeU[ 1 ] != GSYS.DrawSetting.TexAddressModeU[ 1 ] ) Graphics_D3D9_DeviceState_SetTextureAddressU( GSYS.DrawSetting.TexAddressModeU[ 1 ], 1 ) ;
		if( GD3D9.Device.State.TexAddressModeU[ 2 ] != GSYS.DrawSetting.TexAddressModeU[ 2 ] ) Graphics_D3D9_DeviceState_SetTextureAddressU( GSYS.DrawSetting.TexAddressModeU[ 2 ], 2 ) ;
		if( GD3D9.Device.State.TexAddressModeU[ 3 ] != GSYS.DrawSetting.TexAddressModeU[ 3 ] ) Graphics_D3D9_DeviceState_SetTextureAddressU( GSYS.DrawSetting.TexAddressModeU[ 3 ], 3 ) ;
		if( GD3D9.Device.State.TexAddressModeU[ 4 ] != GSYS.DrawSetting.TexAddressModeU[ 4 ] ) Graphics_D3D9_DeviceState_SetTextureAddressU( GSYS.DrawSetting.TexAddressModeU[ 4 ], 4 ) ;
		if( GD3D9.Device.State.TexAddressModeU[ 5 ] != GSYS.DrawSetting.TexAddressModeU[ 5 ] ) Graphics_D3D9_DeviceState_SetTextureAddressU( GSYS.DrawSetting.TexAddressModeU[ 5 ], 5 ) ;
		if( GD3D9.Device.State.TexAddressModeU[ 6 ] != GSYS.DrawSetting.TexAddressModeU[ 6 ] ) Graphics_D3D9_DeviceState_SetTextureAddressU( GSYS.DrawSetting.TexAddressModeU[ 6 ], 6 ) ;
		if( GD3D9.Device.State.TexAddressModeU[ 7 ] != GSYS.DrawSetting.TexAddressModeU[ 7 ] ) Graphics_D3D9_DeviceState_SetTextureAddressU( GSYS.DrawSetting.TexAddressModeU[ 7 ], 7 ) ;

		if( GD3D9.Device.State.TexAddressModeV[ 0 ] != GSYS.DrawSetting.TexAddressModeV[ 0 ] ) Graphics_D3D9_DeviceState_SetTextureAddressV( GSYS.DrawSetting.TexAddressModeV[ 0 ], 0 ) ;
		if( GD3D9.Device.State.TexAddressModeV[ 1 ] != GSYS.DrawSetting.TexAddressModeV[ 1 ] ) Graphics_D3D9_DeviceState_SetTextureAddressV( GSYS.DrawSetting.TexAddressModeV[ 1 ], 1 ) ;
		if( GD3D9.Device.State.TexAddressModeV[ 2 ] != GSYS.DrawSetting.TexAddressModeV[ 2 ] ) Graphics_D3D9_DeviceState_SetTextureAddressV( GSYS.DrawSetting.TexAddressModeV[ 2 ], 2 ) ;
		if( GD3D9.Device.State.TexAddressModeV[ 3 ] != GSYS.DrawSetting.TexAddressModeV[ 3 ] ) Graphics_D3D9_DeviceState_SetTextureAddressV( GSYS.DrawSetting.TexAddressModeV[ 3 ], 3 ) ;
		if( GD3D9.Device.State.TexAddressModeV[ 4 ] != GSYS.DrawSetting.TexAddressModeV[ 4 ] ) Graphics_D3D9_DeviceState_SetTextureAddressV( GSYS.DrawSetting.TexAddressModeV[ 4 ], 4 ) ;
		if( GD3D9.Device.State.TexAddressModeV[ 5 ] != GSYS.DrawSetting.TexAddressModeV[ 5 ] ) Graphics_D3D9_DeviceState_SetTextureAddressV( GSYS.DrawSetting.TexAddressModeV[ 5 ], 5 ) ;
		if( GD3D9.Device.State.TexAddressModeV[ 6 ] != GSYS.DrawSetting.TexAddressModeV[ 6 ] ) Graphics_D3D9_DeviceState_SetTextureAddressV( GSYS.DrawSetting.TexAddressModeV[ 6 ], 6 ) ;
		if( GD3D9.Device.State.TexAddressModeV[ 7 ] != GSYS.DrawSetting.TexAddressModeV[ 7 ] ) Graphics_D3D9_DeviceState_SetTextureAddressV( GSYS.DrawSetting.TexAddressModeV[ 7 ], 7 ) ;
	}
	else
	{
		if( GD3D9.Device.State.TexAddressModeU[ 0 ] != DX_TEXADDRESS_CLAMP || GD3D9.Device.State.TexAddressModeV[ 0 ] != DX_TEXADDRESS_CLAMP ) Graphics_D3D9_DeviceState_SetTextureAddress( DX_TEXADDRESS_CLAMP, 0 ) ;
		if( GD3D9.Device.State.TexAddressModeU[ 1 ] != DX_TEXADDRESS_CLAMP || GD3D9.Device.State.TexAddressModeV[ 1 ] != DX_TEXADDRESS_CLAMP ) Graphics_D3D9_DeviceState_SetTextureAddress( DX_TEXADDRESS_CLAMP, 1 ) ;
		if( GD3D9.Device.State.TexAddressModeU[ 2 ] != DX_TEXADDRESS_CLAMP || GD3D9.Device.State.TexAddressModeV[ 2 ] != DX_TEXADDRESS_CLAMP ) Graphics_D3D9_DeviceState_SetTextureAddress( DX_TEXADDRESS_CLAMP, 2 ) ;
		if( GD3D9.Device.State.TexAddressModeU[ 3 ] != DX_TEXADDRESS_CLAMP || GD3D9.Device.State.TexAddressModeV[ 3 ] != DX_TEXADDRESS_CLAMP ) Graphics_D3D9_DeviceState_SetTextureAddress( DX_TEXADDRESS_CLAMP, 3 ) ;
	}

/*
	Graphics_D3D9_DrawSetting_SetDrawBlendMode( GSYS.DrawSetting.BlendMode, GSYS.DrawSetting.BlendParam,  ) ;

	if( ( Format->AlphaTestFlag || Format->AlphaChFlag ) && ( TransFlag != GRH.AlphaValidFlag ) )
	{
		Graphics_D3D9_DrawSetting_SetDrawBlendMode( DX_BLENDMODE_BLINEALPHA ,
									Format->AlphaTestFlag ? ( TransFlag  ? TRUE : FALSE ) : ( Format->AlphaChFlag ? TRUE : FALSE ) ) ;
	}
*/
	// パラメータを保存
//	GRH.DrawPrepFormat    = Format ;
//	GRH.DrawPrepTexture   = Texture ;
	GD3D9.Device.DrawSetting.DrawPrepParamFlag = ParamFlag ;
}

// ビギンシーンを行う
extern void Graphics_D3D9_BeginScene( void )
{
	if( Direct3D9_IsValid() == 0 || GD3D9.Device.DrawInfo.BeginSceneFlag == TRUE ) return ;

	Direct3DDevice9_BeginScene() ;
	GD3D9.Device.DrawInfo.BeginSceneFlag = TRUE ;

//	GRH.DrawPrepFormat    = NULL ;
//	GRH.DrawPrepTexture   = NULL ;
	GD3D9.Device.DrawSetting.DrawPrepParamFlag = 0 ;
}

// エンドシーンを行う
extern void Graphics_D3D9_EndScene( void )
{
	if( Direct3D9_IsValid() == 0 || GD3D9.Device.DrawInfo.BeginSceneFlag == FALSE ) return ;

	Direct3DDevice9_EndScene() ;
	GD3D9.Device.DrawInfo.BeginSceneFlag = FALSE ;
}





























// Direct3D9 を使った描画関係

// ハードウエアアクセラレータ使用版 DrawBillboard3D
extern	int		Graphics_D3D9_DrawBillboard3D( VECTOR Pos, float cx, float cy, float Size, float Angle, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, int ReverseXFlag, int ReverseYFlag, int DrawFlag, RECT *DrawArea )
{
	VERTEX_2D *DrawVert ;
	VERTEX_2D TempVert[ 6 ] ;
	VERTEX_3D *DrawVert3D ;
	VERTEX_BLENDTEX_2D *DrawVertB ;
	VERTEX_BLENDTEX_2D TempVertB[ 6 ] ;
	IMAGEDATA_HARD_DRAW *DrawTex ;
	IMAGEDATA_HARD_DRAW *BlendDrawTex ;
	IMAGEDATA_ORIG *Orig ;
	IMAGEDATA_HARD_VERT *TexVert ;
	IMAGEDATA_HARD_VERT *BlendTexVert = NULL ;
	DWORD DiffuseColor ;
	int DrawTexNum ;
	int i ;
	int Flag ;
	int BlendGraphNoIncFlag ;
	float SizeX ;
	float SizeY ;
	float f ;
	VECTOR SrcVec[ 4 ] ;
	VECTOR SrcVec2[ 4 ] ;
	float Sin = 0.0f ;
	float Cos = 1.0f ;
	float ScaleX ;
	float ScaleY ;
	float dleft = 0.0f ;
	float dright = 0.0f ;
	float dtop = 0.0f ;
	float dbottom = 0.0f ;
	float z ;
	float rhw ;
	float u[ 2 ] ;
	float v[ 2 ] ;

	Orig = Image->Orig ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	// 描画準備
	if( DrawFlag )
	{
		if( GD3D9.Device.DrawInfo.BeginSceneFlag == FALSE ) Graphics_D3D9_BeginScene() ;
		Flag = TransFlag | DX_D3D9_DRAWPREP_3D | DX_D3D9_DRAWPREP_FOG | DX_D3D9_DRAWPREP_TEXADDRESS ;
		DX_D3D9_DRAWPREP_TEX( Orig, Image->Hard.Draw[ 0 ].Tex->PF->D3D9.Texture, Flag )
	}
	else
	{
		dleft   = -100000000.0f ;
		dright  =  100000000.0f ;
		dtop    = -100000000.0f ;
		dbottom =  100000000.0f ;
	}

	// 頂点データを取得
	DiffuseColor = GD3D9.Device.DrawInfo.DiffuseColor ;

	// 描画情報の数をセット
	DrawTexNum = Image->Hard.DrawNum ;

	// 描画情報の数がブレンド画像と異なっていたら０番目のテクスチャだけを使用する
	BlendGraphNoIncFlag = FALSE ;
	if( BlendImage != NULL && BlendImage->Hard.DrawNum != Image->Hard.DrawNum )
	{
		BlendGraphNoIncFlag = TRUE ;
	}

	// 描画情報配列のアドレスをセットしておく
	DrawTex = Image->Hard.Draw ;
	BlendDrawTex = NULL ;
	if( BlendImage != NULL )
	{
		BlendDrawTex = BlendImage->Hard.Draw ;
	}

	// サイズと座標関係の事前計算
	SizeX = Size ;
	SizeY = Size * ( float )Image->HeightF / ( float )Image->WidthF ;

	ScaleX = SizeX / Image->WidthF ;
	ScaleY = SizeY / Image->HeightF ;
	cx *= Image->WidthF ;
	cy *= Image->HeightF ;

	// 回転する場合は回転値を求めておく
	if( Angle != 0.0 )
	{
		_SINCOS( (float)Angle, &Sin, &Cos ) ;
	}

	// テクスチャーの数だけ繰り返す
	for( i = 0 ; i < DrawTexNum ; i ++, DrawTex ++ )
	{
		// 描画するテクスチャーのセット
		if( DrawFlag )
		{
			Graphics_D3D9_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D9.Texture ) ;

			if( BlendDrawTex != NULL )
			{
				Graphics_D3D9_DrawSetting_SetBlendTexture( BlendDrawTex->Tex->PF->D3D9.Texture, BlendDrawTex->Tex->TexWidth, BlendDrawTex->Tex->TexHeight );
			}

			if( GD3D9.Device.DrawSetting.ChangeTextureFlag )
			{
				Graphics_D3D9_DeviceState_RefreshBlendState() ;
			}
		}

		TexVert = DrawTex->Vertex ;
		if( BlendDrawTex != NULL )
		{
			BlendTexVert = BlendDrawTex->Vertex ;
		}

		// ブレンドグラフィックを使用していなくて、且つ描画する場合は高速な処理を使用する
		if( GD3D9.Device.DrawInfo.VertexType != VERTEXTYPE_BLENDTEX && DrawFlag == TRUE )
		{
			GETVERTEX_BILLBOARD( DrawVert3D ) ;

			// 回転する場合としない場合で処理を分岐
			if( Angle != 0.0 )
			{
				// ローカル座標準備
				SrcVec[2].x = SrcVec[0].x = ( -cx + TexVert[0].x ) * ScaleX ;
				SrcVec[3].x = SrcVec[1].x = ( -cx + TexVert[1].x ) * ScaleX ;

				SrcVec[1].y = SrcVec[0].y = ( -cy + Image->HeightF - TexVert[0].y ) * ScaleY ;
				SrcVec[3].y = SrcVec[2].y = ( -cy + Image->HeightF - TexVert[2].y ) * ScaleY ;

				// 回転計算
				f             = SrcVec[ 0 ].x * Cos - SrcVec[ 0 ].y * Sin ;	
				SrcVec[ 0 ].y = SrcVec[ 0 ].x * Sin + SrcVec[ 0 ].y * Cos ;
				SrcVec[ 0 ].x = f ;

				f             = SrcVec[ 1 ].x * Cos - SrcVec[ 1 ].y * Sin ;	
				SrcVec[ 1 ].y = SrcVec[ 1 ].x * Sin + SrcVec[ 1 ].y * Cos ;
				SrcVec[ 1 ].x = f ;

				f             = SrcVec[ 2 ].x * Cos - SrcVec[ 2 ].y * Sin ;	
				SrcVec[ 2 ].y = SrcVec[ 2 ].x * Sin + SrcVec[ 2 ].y * Cos ;
				SrcVec[ 2 ].x = f ;

				f             = SrcVec[ 3 ].x * Cos - SrcVec[ 3 ].y * Sin ;	
				SrcVec[ 3 ].y = SrcVec[ 3 ].x * Sin + SrcVec[ 3 ].y * Cos ;
				SrcVec[ 3 ].x = f ;

				// ビルボード座標をワールド座標へ変換
				DrawVert3D[ 0 ].pos.x = SrcVec[ 0 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][0] + SrcVec[ 0 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][0] + Pos.x ;
				DrawVert3D[ 0 ].pos.y = SrcVec[ 0 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][1] + SrcVec[ 0 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][1] + Pos.y ;
				DrawVert3D[ 0 ].pos.z = SrcVec[ 0 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][2] + SrcVec[ 0 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][2] + Pos.z ;

				DrawVert3D[ 1 ].pos.x = SrcVec[ 1 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][0] + SrcVec[ 1 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][0] + Pos.x ;
				DrawVert3D[ 1 ].pos.y = SrcVec[ 1 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][1] + SrcVec[ 1 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][1] + Pos.y ;
				DrawVert3D[ 1 ].pos.z = SrcVec[ 1 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][2] + SrcVec[ 1 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][2] + Pos.z ;

				DrawVert3D[ 2 ].pos.x = SrcVec[ 2 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][0] + SrcVec[ 2 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][0] + Pos.x ;
				DrawVert3D[ 2 ].pos.y = SrcVec[ 2 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][1] + SrcVec[ 2 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][1] + Pos.y ;
				DrawVert3D[ 2 ].pos.z = SrcVec[ 2 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][2] + SrcVec[ 2 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][2] + Pos.z ;

				DrawVert3D[ 3 ].pos.x = SrcVec[ 3 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][0] + SrcVec[ 3 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][0] + Pos.x ;
				DrawVert3D[ 3 ].pos.y = SrcVec[ 3 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][1] + SrcVec[ 3 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][1] + Pos.y ;
				DrawVert3D[ 3 ].pos.z = SrcVec[ 3 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][2] + SrcVec[ 3 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][2] + Pos.z ;
			}
			else
			{
				VECTOR TempVecX[ 2 ], TempVecY[ 2 ] ;

				// ローカル座標準備
				SrcVec[0].x = ( -cx + TexVert[0].x ) * ScaleX ;
				SrcVec[1].x = ( -cx + TexVert[1].x ) * ScaleX ;

				SrcVec[0].y = ( -cy + Image->HeightF - TexVert[0].y ) * ScaleY ;
				SrcVec[2].y = ( -cy + Image->HeightF - TexVert[2].y ) * ScaleY ;

				// ビルボード座標をワールド座標へ変換
				TempVecX[ 0 ].x = SrcVec[ 0 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][0] + Pos.x ;
				TempVecX[ 0 ].y = SrcVec[ 0 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][1] + Pos.y ;
				TempVecX[ 0 ].z = SrcVec[ 0 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][2] + Pos.z ;

				TempVecX[ 1 ].x = SrcVec[ 1 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][0] + Pos.x ;
				TempVecX[ 1 ].y = SrcVec[ 1 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][1] + Pos.y ;
				TempVecX[ 1 ].z = SrcVec[ 1 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][2] + Pos.z ;

				TempVecY[ 0 ].x = SrcVec[ 0 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][0] ;
				TempVecY[ 0 ].y = SrcVec[ 0 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][1] ;
				TempVecY[ 0 ].z = SrcVec[ 0 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][2] ;

				TempVecY[ 1 ].x = SrcVec[ 2 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][0] ;
				TempVecY[ 1 ].y = SrcVec[ 2 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][1] ;
				TempVecY[ 1 ].z = SrcVec[ 2 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][2] ;

				DrawVert3D[ 0 ].pos.x = TempVecX[ 0 ].x + TempVecY[ 0 ].x ;
				DrawVert3D[ 0 ].pos.y = TempVecX[ 0 ].y + TempVecY[ 0 ].y ;
				DrawVert3D[ 0 ].pos.z = TempVecX[ 0 ].z + TempVecY[ 0 ].z ;

				DrawVert3D[ 1 ].pos.x = TempVecX[ 1 ].x + TempVecY[ 0 ].x ;
				DrawVert3D[ 1 ].pos.y = TempVecX[ 1 ].y + TempVecY[ 0 ].y ;
				DrawVert3D[ 1 ].pos.z = TempVecX[ 1 ].z + TempVecY[ 0 ].z ;

				DrawVert3D[ 2 ].pos.x = TempVecX[ 0 ].x + TempVecY[ 1 ].x ;
				DrawVert3D[ 2 ].pos.y = TempVecX[ 0 ].y + TempVecY[ 1 ].y ;
				DrawVert3D[ 2 ].pos.z = TempVecX[ 0 ].z + TempVecY[ 1 ].z ;

				DrawVert3D[ 3 ].pos.x = TempVecX[ 1 ].x + TempVecY[ 1 ].x ;
				DrawVert3D[ 3 ].pos.y = TempVecX[ 1 ].y + TempVecY[ 1 ].y ;
				DrawVert3D[ 3 ].pos.z = TempVecX[ 1 ].z + TempVecY[ 1 ].z ;
			}

			DrawVert3D[4].pos = DrawVert3D[2].pos ;
			DrawVert3D[5].pos = DrawVert3D[1].pos ;

			*( ( DWORD * )&DrawVert3D[0].b ) = 
			*( ( DWORD * )&DrawVert3D[3].b ) = DiffuseColor ;

			if( ReverseXFlag )
			{
				DrawVert3D[5].u = DrawVert3D[3].u = DrawVert3D[1].u = TexVert[0].u ;
				DrawVert3D[4].u = DrawVert3D[2].u = DrawVert3D[0].u = TexVert[1].u ;
			}
			else
			{
				DrawVert3D[4].u = DrawVert3D[2].u = DrawVert3D[0].u = TexVert[0].u ;
				DrawVert3D[5].u = DrawVert3D[3].u = DrawVert3D[1].u = TexVert[1].u ;
			}
			if( ReverseYFlag )
			{
				DrawVert3D[4].v = DrawVert3D[3].v = DrawVert3D[2].v = TexVert[0].v ;
				DrawVert3D[5].v = DrawVert3D[1].v = DrawVert3D[0].v = TexVert[2].v ;
			}
			else
			{
				DrawVert3D[5].v = DrawVert3D[1].v = DrawVert3D[0].v = TexVert[0].v ;
				DrawVert3D[4].v = DrawVert3D[3].v = DrawVert3D[2].v = TexVert[2].v ;
			}

			// テクスチャーを描画する
			ADD4VERTEX_BILLBOARD
		}
		else
		{
			VECTOR DrawPos[ 4 ] ;

			// ビルボードの４頂点を得る
			{
				// 回転する場合としない場合で処理を分岐
				if( Angle != 0.0 )
				{
					// ローカル座標準備
					SrcVec[2].x = SrcVec[0].x = ( -cx + TexVert[0].x ) * ScaleX ;
					SrcVec[3].x = SrcVec[1].x = ( -cx + TexVert[1].x ) * ScaleX ;

					SrcVec[1].y = SrcVec[0].y = ( -cy + Image->HeightF - TexVert[0].y ) * ScaleY ;
					SrcVec[3].y = SrcVec[2].y = ( -cy + Image->HeightF - TexVert[2].y ) * ScaleY ;

					// 回転計算
					f             = SrcVec[ 0 ].x * Cos - SrcVec[ 0 ].y * Sin ;	
					SrcVec[ 0 ].y = SrcVec[ 0 ].x * Sin + SrcVec[ 0 ].y * Cos ;
					SrcVec[ 0 ].x = f ;

					f             = SrcVec[ 1 ].x * Cos - SrcVec[ 1 ].y * Sin ;	
					SrcVec[ 1 ].y = SrcVec[ 1 ].x * Sin + SrcVec[ 1 ].y * Cos ;
					SrcVec[ 1 ].x = f ;

					f             = SrcVec[ 2 ].x * Cos - SrcVec[ 2 ].y * Sin ;	
					SrcVec[ 2 ].y = SrcVec[ 2 ].x * Sin + SrcVec[ 2 ].y * Cos ;
					SrcVec[ 2 ].x = f ;

					f             = SrcVec[ 3 ].x * Cos - SrcVec[ 3 ].y * Sin ;	
					SrcVec[ 3 ].y = SrcVec[ 3 ].x * Sin + SrcVec[ 3 ].y * Cos ;
					SrcVec[ 3 ].x = f ;

					// ビルボード座標をワールド座標へ変換
					SrcVec2[ 0 ].x = SrcVec[ 0 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][0] + SrcVec[ 0 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][0] + Pos.x ;
					SrcVec2[ 0 ].y = SrcVec[ 0 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][1] + SrcVec[ 0 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][1] + Pos.y ;
					SrcVec2[ 0 ].z = SrcVec[ 0 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][2] + SrcVec[ 0 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][2] + Pos.z ;

					SrcVec2[ 1 ].x = SrcVec[ 1 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][0] + SrcVec[ 1 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][0] + Pos.x ;
					SrcVec2[ 1 ].y = SrcVec[ 1 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][1] + SrcVec[ 1 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][1] + Pos.y ;
					SrcVec2[ 1 ].z = SrcVec[ 1 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][2] + SrcVec[ 1 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][2] + Pos.z ;

					SrcVec2[ 2 ].x = SrcVec[ 2 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][0] + SrcVec[ 2 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][0] + Pos.x ;
					SrcVec2[ 2 ].y = SrcVec[ 2 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][1] + SrcVec[ 2 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][1] + Pos.y ;
					SrcVec2[ 2 ].z = SrcVec[ 2 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][2] + SrcVec[ 2 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][2] + Pos.z ;

					SrcVec2[ 3 ].x = SrcVec[ 3 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][0] + SrcVec[ 3 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][0] + Pos.x ;
					SrcVec2[ 3 ].y = SrcVec[ 3 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][1] + SrcVec[ 3 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][1] + Pos.y ;
					SrcVec2[ 3 ].z = SrcVec[ 3 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][2] + SrcVec[ 3 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][2] + Pos.z ;
				}
				else
				{
					VECTOR TempVecX[ 2 ], TempVecY[ 2 ] ;

					// ローカル座標準備
					SrcVec[0].x = ( -cx + TexVert[0].x ) * ScaleX ;
					SrcVec[1].x = ( -cx + TexVert[1].x ) * ScaleX ;

					SrcVec[0].y = ( -cy + Image->HeightF - TexVert[0].y ) * ScaleY ;
					SrcVec[2].y = ( -cy + Image->HeightF - TexVert[2].y ) * ScaleY ;

					// ビルボード座標をワールド座標へ変換
					TempVecX[ 0 ].x = SrcVec[ 0 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][0] + Pos.x ;
					TempVecX[ 0 ].y = SrcVec[ 0 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][1] + Pos.y ;
					TempVecX[ 0 ].z = SrcVec[ 0 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][2] + Pos.z ;

					TempVecX[ 1 ].x = SrcVec[ 1 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][0] + Pos.x ;
					TempVecX[ 1 ].y = SrcVec[ 1 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][1] + Pos.y ;
					TempVecX[ 1 ].z = SrcVec[ 1 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][2] + Pos.z ;

					TempVecY[ 0 ].x = SrcVec[ 0 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][0] ;
					TempVecY[ 0 ].y = SrcVec[ 0 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][1] ;
					TempVecY[ 0 ].z = SrcVec[ 0 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][2] ;

					TempVecY[ 1 ].x = SrcVec[ 2 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][0] ;
					TempVecY[ 1 ].y = SrcVec[ 2 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][1] ;
					TempVecY[ 1 ].z = SrcVec[ 2 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][2] ;

					SrcVec2[ 0 ].x = TempVecX[ 0 ].x + TempVecY[ 0 ].x ;
					SrcVec2[ 0 ].y = TempVecX[ 0 ].y + TempVecY[ 0 ].y ;
					SrcVec2[ 0 ].z = TempVecX[ 0 ].z + TempVecY[ 0 ].z ;

					SrcVec2[ 1 ].x = TempVecX[ 1 ].x + TempVecY[ 0 ].x ;
					SrcVec2[ 1 ].y = TempVecX[ 1 ].y + TempVecY[ 0 ].y ;
					SrcVec2[ 1 ].z = TempVecX[ 1 ].z + TempVecY[ 0 ].z ;

					SrcVec2[ 2 ].x = TempVecX[ 0 ].x + TempVecY[ 1 ].x ;
					SrcVec2[ 2 ].y = TempVecX[ 0 ].y + TempVecY[ 1 ].y ;
					SrcVec2[ 2 ].z = TempVecX[ 0 ].z + TempVecY[ 1 ].z ;

					SrcVec2[ 3 ].x = TempVecX[ 1 ].x + TempVecY[ 1 ].x ;
					SrcVec2[ 3 ].y = TempVecX[ 1 ].y + TempVecY[ 1 ].y ;
					SrcVec2[ 3 ].z = TempVecX[ 1 ].z + TempVecY[ 1 ].z ;
				}
			}

			// 頂点タイプによって処理を分岐
			switch( GD3D9.Device.DrawInfo.VertexType )
			{
			case VERTEXTYPE_BLENDTEX :
				// ブレンドテクスチャを使用する

				// 頂点バッファの取得
				if( DrawFlag )
				{
					GETVERTEX_QUAD( DrawVertB )
				}
				else
				{
					DrawVertB = TempVertB ;
				}

				DrawVertB[0].color	= DiffuseColor ;
				DrawVertB[3].color	= DiffuseColor ;

				// スクリーン座標に変換
				rhw = 1.0f / ( SrcVec2[0].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][3] + SrcVec2[0].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][3] + SrcVec2[0].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][3] + GSYS.DrawSetting.Blend3DMatrixF.m[3][3] ) ;
				z   = rhw  * ( SrcVec2[0].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][2] + SrcVec2[0].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][2] + SrcVec2[0].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][2] + GSYS.DrawSetting.Blend3DMatrixF.m[3][2] ) ;

				DrawPos[ 0 ].x = ( SrcVec2[0].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][0] + SrcVec2[0].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][0] + SrcVec2[0].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][0] + GSYS.DrawSetting.Blend3DMatrixF.m[3][0] ) * rhw ;
				DrawPos[ 0 ].y = ( SrcVec2[0].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][1] + SrcVec2[0].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][1] + SrcVec2[0].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][1] + GSYS.DrawSetting.Blend3DMatrixF.m[3][1] ) * rhw ;
				DrawPos[ 1 ].x = ( SrcVec2[1].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][0] + SrcVec2[1].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][0] + SrcVec2[1].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][0] + GSYS.DrawSetting.Blend3DMatrixF.m[3][0] ) * rhw ;
				DrawPos[ 1 ].y = ( SrcVec2[1].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][1] + SrcVec2[1].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][1] + SrcVec2[1].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][1] + GSYS.DrawSetting.Blend3DMatrixF.m[3][1] ) * rhw ;
				DrawPos[ 2 ].x = ( SrcVec2[2].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][0] + SrcVec2[2].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][0] + SrcVec2[2].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][0] + GSYS.DrawSetting.Blend3DMatrixF.m[3][0] ) * rhw ;
				DrawPos[ 2 ].y = ( SrcVec2[2].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][1] + SrcVec2[2].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][1] + SrcVec2[2].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][1] + GSYS.DrawSetting.Blend3DMatrixF.m[3][1] ) * rhw ;
				DrawPos[ 3 ].x = ( SrcVec2[3].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][0] + SrcVec2[3].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][0] + SrcVec2[3].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][0] + GSYS.DrawSetting.Blend3DMatrixF.m[3][0] ) * rhw ;
				DrawPos[ 3 ].y = ( SrcVec2[3].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][1] + SrcVec2[3].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][1] + SrcVec2[3].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][1] + GSYS.DrawSetting.Blend3DMatrixF.m[3][1] ) * rhw ;

				DrawVertB[0].pos.x = DrawPos[ 0 ].x ;
				DrawVertB[0].pos.y = DrawPos[ 0 ].y ;
				DrawVertB[1].pos.x = DrawPos[ 1 ].x ;
				DrawVertB[1].pos.y = DrawPos[ 1 ].y ;
				DrawVertB[2].pos.x = DrawPos[ 2 ].x ;
				DrawVertB[2].pos.y = DrawPos[ 2 ].y ;
				DrawVertB[3].pos.x = DrawPos[ 3 ].x ;
				DrawVertB[3].pos.y = DrawPos[ 3 ].y ;
				DrawVertB[4].pos.x = DrawPos[ 2 ].x ;
				DrawVertB[4].pos.y = DrawPos[ 2 ].y ;
				DrawVertB[5].pos.x = DrawPos[ 1 ].x ;
				DrawVertB[5].pos.y = DrawPos[ 1 ].y ;

				switch( GSYS.DrawSetting.BlendGraphPosMode )
				{
				default :
				case DX_BLENDGRAPH_POSMODE_DRAWGRAPH:
					u[ 0 ] = ( TexVert[0].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					u[ 1 ] = ( TexVert[1].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					v[ 0 ] = ( TexVert[0].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					v[ 1 ] = ( TexVert[2].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[0].u1 = u[ 0 ] ;
					DrawVertB[0].v1 = v[ 0 ] ;
					DrawVertB[1].u1 = u[ 1 ] ;
					DrawVertB[1].v1 = v[ 0 ] ;
					DrawVertB[2].u1 = u[ 0 ] ;
					DrawVertB[2].v1 = v[ 1 ] ;
					DrawVertB[3].u1 = u[ 1 ] ;
					DrawVertB[3].v1 = v[ 1 ] ;
					DrawVertB[4].u1 = u[ 0 ] ;
					DrawVertB[4].v1 = v[ 1 ] ;
					DrawVertB[5].u1 = u[ 1 ] ;
					DrawVertB[5].v1 = v[ 0 ] ;
					break ;

				case DX_BLENDGRAPH_POSMODE_SCREEN :
					DrawVertB[0].u1 = ( DrawPos[ 0 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[0].v1 = ( DrawPos[ 0 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[1].u1 = ( DrawPos[ 1 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[1].v1 = ( DrawPos[ 1 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[2].u1 = ( DrawPos[ 2 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[2].v1 = ( DrawPos[ 2 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[3].u1 = ( DrawPos[ 3 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[3].v1 = ( DrawPos[ 3 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[4].u1 = ( DrawPos[ 2 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[4].v1 = ( DrawPos[ 2 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[5].u1 = ( DrawPos[ 1 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[5].v1 = ( DrawPos[ 1 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					break ;
				}

				if( ReverseXFlag )
				{
					DrawVertB[0].u2 = TexVert[1].u ;
					DrawVertB[1].u2 = TexVert[0].u ;
					DrawVertB[2].u2 = TexVert[1].u ;
					DrawVertB[3].u2 = TexVert[0].u ;
					DrawVertB[4].u2 = TexVert[1].u ;
					DrawVertB[5].u2 = TexVert[0].u ;
				}
				else
				{
					DrawVertB[0].u2 = TexVert[0].u ;
					DrawVertB[1].u2 = TexVert[1].u ;
					DrawVertB[2].u2 = TexVert[0].u ;
					DrawVertB[3].u2 = TexVert[1].u ;
					DrawVertB[4].u2 = TexVert[0].u ;
					DrawVertB[5].u2 = TexVert[1].u ;
				}
				if( ReverseYFlag )
				{
					DrawVertB[0].v2 = TexVert[2].v ;
					DrawVertB[1].v2 = TexVert[2].v ;
					DrawVertB[2].v2 = TexVert[0].v ;
					DrawVertB[3].v2 = TexVert[0].v ;
					DrawVertB[4].v2 = TexVert[0].v ;
					DrawVertB[5].v2 = TexVert[2].v ;
				}
				else
				{
					DrawVertB[0].v2 = TexVert[0].v ;
					DrawVertB[1].v2 = TexVert[0].v ;
					DrawVertB[2].v2 = TexVert[2].v ;
					DrawVertB[3].v2 = TexVert[2].v ;
					DrawVertB[4].v2 = TexVert[2].v ;
					DrawVertB[5].v2 = TexVert[0].v ;
				}

				DrawVertB[0].pos.z = z ;
				DrawVertB[1].pos.z = z ;
				DrawVertB[2].pos.z = z ;
				DrawVertB[3].pos.z = z ;
				DrawVertB[4].pos.z = z ;
				DrawVertB[5].pos.z = z ;

				DrawVertB[0].rhw = rhw ;
				DrawVertB[1].rhw = rhw ;
				DrawVertB[2].rhw = rhw ;
				DrawVertB[3].rhw = rhw ;
				DrawVertB[4].rhw = rhw ;
				DrawVertB[5].rhw = rhw ;

				// テクスチャーを描画する
				if( DrawFlag )
				{
					ADD4VERTEX_BLENDTEX
				}
				else
				{
					if( dright  > DrawVertB[ 0 ].pos.x ) dright  = DrawVertB[ 0 ].pos.x ;
					if( dright  > DrawVertB[ 1 ].pos.x ) dright  = DrawVertB[ 1 ].pos.x ;
					if( dright  > DrawVertB[ 2 ].pos.x ) dright  = DrawVertB[ 2 ].pos.x ;
					if( dright  > DrawVertB[ 3 ].pos.x ) dright  = DrawVertB[ 3 ].pos.x ;

					if( dleft   < DrawVertB[ 0 ].pos.x ) dleft   = DrawVertB[ 0 ].pos.x ;
					if( dleft   < DrawVertB[ 1 ].pos.x ) dleft   = DrawVertB[ 1 ].pos.x ;
					if( dleft   < DrawVertB[ 2 ].pos.x ) dleft   = DrawVertB[ 2 ].pos.x ;
					if( dleft   < DrawVertB[ 3 ].pos.x ) dleft   = DrawVertB[ 3 ].pos.x ;

					if( dbottom > DrawVertB[ 0 ].pos.y ) dbottom = DrawVertB[ 0 ].pos.y ;
					if( dbottom > DrawVertB[ 1 ].pos.y ) dbottom = DrawVertB[ 1 ].pos.y ;
					if( dbottom > DrawVertB[ 2 ].pos.y ) dbottom = DrawVertB[ 2 ].pos.y ;
					if( dbottom > DrawVertB[ 3 ].pos.y ) dbottom = DrawVertB[ 3 ].pos.y ;

					if( dtop    < DrawVertB[ 0 ].pos.y ) dtop    = DrawVertB[ 0 ].pos.y ;
					if( dtop    < DrawVertB[ 1 ].pos.y ) dtop    = DrawVertB[ 1 ].pos.y ;
					if( dtop    < DrawVertB[ 2 ].pos.y ) dtop    = DrawVertB[ 2 ].pos.y ;
					if( dtop    < DrawVertB[ 3 ].pos.y ) dtop    = DrawVertB[ 3 ].pos.y ;
				}

				if( BlendGraphNoIncFlag == FALSE )
				{
					BlendDrawTex ++ ;
				}
				break ;

			case VERTEXTYPE_TEX :
				// ブレンドテクスチャを使用しない

				// 頂点バッファの取得
				if( DrawFlag )
				{
					GETVERTEX_QUAD( DrawVert )
				}
				else
				{
					DrawVert = TempVert;
				}

				// スクリーン座標に変換
				rhw = 1.0f / ( SrcVec2[0].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][3] + SrcVec2[0].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][3] + SrcVec2[0].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][3] + GSYS.DrawSetting.Blend3DMatrixF.m[3][3] ) ;
				z   = rhw  * ( SrcVec2[0].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][2] + SrcVec2[0].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][2] + SrcVec2[0].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][2] + GSYS.DrawSetting.Blend3DMatrixF.m[3][2] ) ;

				DrawVert[0].pos.x = ( SrcVec2[0].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][0] + SrcVec2[0].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][0] + SrcVec2[0].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][0] + GSYS.DrawSetting.Blend3DMatrixF.m[3][0] ) * rhw ;
				DrawVert[0].pos.y = ( SrcVec2[0].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][1] + SrcVec2[0].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][1] + SrcVec2[0].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][1] + GSYS.DrawSetting.Blend3DMatrixF.m[3][1] ) * rhw ;
				DrawVert[1].pos.x = ( SrcVec2[1].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][0] + SrcVec2[1].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][0] + SrcVec2[1].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][0] + GSYS.DrawSetting.Blend3DMatrixF.m[3][0] ) * rhw ;
				DrawVert[1].pos.y = ( SrcVec2[1].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][1] + SrcVec2[1].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][1] + SrcVec2[1].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][1] + GSYS.DrawSetting.Blend3DMatrixF.m[3][1] ) * rhw ;
				DrawVert[2].pos.x = ( SrcVec2[2].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][0] + SrcVec2[2].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][0] + SrcVec2[2].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][0] + GSYS.DrawSetting.Blend3DMatrixF.m[3][0] ) * rhw ;
				DrawVert[2].pos.y = ( SrcVec2[2].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][1] + SrcVec2[2].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][1] + SrcVec2[2].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][1] + GSYS.DrawSetting.Blend3DMatrixF.m[3][1] ) * rhw ;
				DrawVert[3].pos.x = ( SrcVec2[3].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][0] + SrcVec2[3].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][0] + SrcVec2[3].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][0] + GSYS.DrawSetting.Blend3DMatrixF.m[3][0] ) * rhw ;
				DrawVert[3].pos.y = ( SrcVec2[3].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][1] + SrcVec2[3].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][1] + SrcVec2[3].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][1] + GSYS.DrawSetting.Blend3DMatrixF.m[3][1] ) * rhw ;
				DrawVert[4].pos.x = ( SrcVec2[2].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][0] + SrcVec2[2].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][0] + SrcVec2[2].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][0] + GSYS.DrawSetting.Blend3DMatrixF.m[3][0] ) * rhw ;
				DrawVert[4].pos.y = ( SrcVec2[2].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][1] + SrcVec2[2].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][1] + SrcVec2[2].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][1] + GSYS.DrawSetting.Blend3DMatrixF.m[3][1] ) * rhw ;
				DrawVert[5].pos.x = ( SrcVec2[1].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][0] + SrcVec2[1].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][0] + SrcVec2[1].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][0] + GSYS.DrawSetting.Blend3DMatrixF.m[3][0] ) * rhw ;
				DrawVert[5].pos.y = ( SrcVec2[1].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][1] + SrcVec2[1].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][1] + SrcVec2[1].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][1] + GSYS.DrawSetting.Blend3DMatrixF.m[3][1] ) * rhw ;

				DrawVert[0].pos.z = z ;
				DrawVert[1].pos.z = z ;
				DrawVert[2].pos.z = z ;
				DrawVert[3].pos.z = z ;
				DrawVert[4].pos.z = z ;
				DrawVert[5].pos.z = z ;

				DrawVert[0].rhw = rhw ;
				DrawVert[1].rhw = rhw ;
				DrawVert[2].rhw = rhw ;
				DrawVert[3].rhw = rhw ;
				DrawVert[4].rhw = rhw ;
				DrawVert[5].rhw = rhw ;

				DrawVert[0].color = DiffuseColor ;
				DrawVert[1].color = DiffuseColor ;
				DrawVert[2].color = DiffuseColor ;
				DrawVert[3].color = DiffuseColor ;
				DrawVert[4].color = DiffuseColor ;
				DrawVert[5].color = DiffuseColor ;

				if( ReverseXFlag )
				{
					DrawVert[0].u = TexVert[1].u ;
					DrawVert[1].u = TexVert[0].u ;
					DrawVert[2].u = TexVert[1].u ;
					DrawVert[3].u = TexVert[0].u ;
					DrawVert[4].u = TexVert[1].u ;
					DrawVert[5].u = TexVert[0].u ;
				}
				else
				{
					DrawVert[0].u = TexVert[0].u ;
					DrawVert[1].u = TexVert[1].u ;
					DrawVert[2].u = TexVert[0].u ;
					DrawVert[3].u = TexVert[1].u ;
					DrawVert[4].u = TexVert[0].u ;
					DrawVert[5].u = TexVert[1].u ;
				}
				if( ReverseYFlag )
				{
					DrawVert[0].v = TexVert[2].v ;
					DrawVert[1].v = TexVert[2].v ;
					DrawVert[2].v = TexVert[0].v ;
					DrawVert[3].v = TexVert[0].v ;
					DrawVert[4].v = TexVert[0].v ;
					DrawVert[5].v = TexVert[2].v ;
				}
				else
				{
					DrawVert[0].v = TexVert[0].v ;
					DrawVert[1].v = TexVert[0].v ;
					DrawVert[2].v = TexVert[2].v ;
					DrawVert[3].v = TexVert[2].v ;
					DrawVert[4].v = TexVert[2].v ;
					DrawVert[5].v = TexVert[0].v ;
				}

				// テクスチャーを描画する
				if( DrawFlag )
				{
					ADD4VERTEX_TEX
				}
				else
				{
					if( dright  > DrawVert[ 0 ].pos.x ) dright  = DrawVert[ 0 ].pos.x ;
					if( dright  > DrawVert[ 1 ].pos.x ) dright  = DrawVert[ 1 ].pos.x ;
					if( dright  > DrawVert[ 2 ].pos.x ) dright  = DrawVert[ 2 ].pos.x ;
					if( dright  > DrawVert[ 3 ].pos.x ) dright  = DrawVert[ 3 ].pos.x ;

					if( dleft   < DrawVert[ 0 ].pos.x ) dleft   = DrawVert[ 0 ].pos.x ;
					if( dleft   < DrawVert[ 1 ].pos.x ) dleft   = DrawVert[ 1 ].pos.x ;
					if( dleft   < DrawVert[ 2 ].pos.x ) dleft   = DrawVert[ 2 ].pos.x ;
					if( dleft   < DrawVert[ 3 ].pos.x ) dleft   = DrawVert[ 3 ].pos.x ;

					if( dbottom > DrawVert[ 0 ].pos.y ) dbottom = DrawVert[ 0 ].pos.y ;
					if( dbottom > DrawVert[ 1 ].pos.y ) dbottom = DrawVert[ 1 ].pos.y ;
					if( dbottom > DrawVert[ 2 ].pos.y ) dbottom = DrawVert[ 2 ].pos.y ;
					if( dbottom > DrawVert[ 3 ].pos.y ) dbottom = DrawVert[ 3 ].pos.y ;

					if( dtop    < DrawVert[ 0 ].pos.y ) dtop    = DrawVert[ 0 ].pos.y ;
					if( dtop    < DrawVert[ 1 ].pos.y ) dtop    = DrawVert[ 1 ].pos.y ;
					if( dtop    < DrawVert[ 2 ].pos.y ) dtop    = DrawVert[ 2 ].pos.y ;
					if( dtop    < DrawVert[ 3 ].pos.y ) dtop    = DrawVert[ 3 ].pos.y ;
				}
				break ;
			}
		}
	}

	if( DrawFlag == FALSE )
	{
		DrawArea->left   = _FTOL( dleft   ) - 1 ;
		DrawArea->right  = _FTOL( dright  ) + 1 ;
		DrawArea->top    = _FTOL( dtop    ) - 1 ;
		DrawArea->bottom = _FTOL( dbottom ) + 1 ;
	}

	// 終了
	return 0 ;
}

// ハードウエアアクセラレータ使用版 DrawModiBillboard3D
extern	int		Graphics_D3D9_DrawModiBillboard3D( VECTOR Pos, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, int DrawFlag, RECT *DrawArea )
{
	VERTEX_2D *DrawVert ;
	VERTEX_3D *DrawVert3D ;
	VERTEX_BLENDTEX_2D *DrawVertB ;
	IMAGEDATA_HARD_DRAW *DrawTex ;
	IMAGEDATA_HARD_DRAW *BlendDrawTex ;
	IMAGEDATA_ORIG *Orig ;
	IMAGEDATA_HARD_VERT *TexVert ;
	IMAGEDATA_HARD_VERT *BlendTexVert = NULL ;
	DWORD DiffuseColor ;
	int DrawTexNum ;
	int i, Flag ;
	int BlendGraphNoIncFlag ;
	VECTOR SrcVec2[ 4 ] ;
	float dleft = 0.0f ;
	float dright = 0.0f ;
	float dtop = 0.0f ;
	float dbottom = 0.0f ;
	float z ;
	float rhw ;
	float u[ 2 ] ;
	float v[ 2 ] ;

	Orig = Image->Orig ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	// 描画準備
	if( DrawFlag )
	{
		if( GD3D9.Device.DrawInfo.BeginSceneFlag == FALSE ) Graphics_D3D9_BeginScene() ;
		Flag = TransFlag | DX_D3D9_DRAWPREP_3D | DX_D3D9_DRAWPREP_FOG | DX_D3D9_DRAWPREP_TEXADDRESS ;
		DX_D3D9_DRAWPREP_TEX( Orig, Image->Hard.Draw[ 0 ].Tex->PF->D3D9.Texture, Flag )
	}
	else
	{
		dleft   = -100000000.0f ;
		dright  =  100000000.0f ;
		dtop    = -100000000.0f ;
		dbottom =  100000000.0f ;
	}

	// 頂点データを取得
	DiffuseColor = GD3D9.Device.DrawInfo.DiffuseColor ;

	// 描画情報の数をセット
	DrawTexNum = Image->Hard.DrawNum ;

	// 描画情報の数がブレンド画像と異なっていたら０番目のテクスチャだけを使用する
	BlendGraphNoIncFlag = FALSE ;
	if( BlendImage != NULL && BlendImage->Hard.DrawNum != Image->Hard.DrawNum )
	{
		BlendGraphNoIncFlag = TRUE ;
	}

	// 描画情報配列のアドレスをセットしておく
	DrawTex = Image->Hard.Draw ;
	BlendDrawTex = NULL ;
	if( BlendImage != NULL )
	{
		BlendDrawTex = BlendImage->Hard.Draw ;
	}

	// テクスチャーの数だけ繰り返す
	for( i = 0 ; i < DrawTexNum ; i ++, DrawTex ++ )
	{
		// 描画するテクスチャーのセット
		if( DrawFlag )
		{
			Graphics_D3D9_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D9.Texture ) ;

			if( BlendDrawTex != NULL )
			{
				Graphics_D3D9_DrawSetting_SetBlendTexture( BlendDrawTex->Tex->PF->D3D9.Texture, BlendDrawTex->Tex->TexWidth, BlendDrawTex->Tex->TexHeight );
			}

			if( GD3D9.Device.DrawSetting.ChangeTextureFlag )
			{
				Graphics_D3D9_DeviceState_RefreshBlendState() ;
			}
		}

		TexVert = DrawTex->Vertex ;
		if( BlendDrawTex != NULL )
		{
			BlendTexVert = BlendDrawTex->Vertex ;
		}

		// ブレンドグラフィックを使用していなくて、且つ描画する場合は高速な処理を使用する
		if( GD3D9.Device.DrawInfo.VertexType != VERTEXTYPE_BLENDTEX && DrawFlag == TRUE )
		{
			GETVERTEX_BILLBOARD( DrawVert3D ) ;

			// ビルボード座標をワールド座標へ変換
			DrawVert3D[ 0 ].pos.x = x1 * GSYS.DrawSetting.BillboardMatrixF.m[0][0] + y1 * GSYS.DrawSetting.BillboardMatrixF.m[1][0] + Pos.x ;
			DrawVert3D[ 0 ].pos.y = x1 * GSYS.DrawSetting.BillboardMatrixF.m[0][1] + y1 * GSYS.DrawSetting.BillboardMatrixF.m[1][1] + Pos.y ;
			DrawVert3D[ 0 ].pos.z = x1 * GSYS.DrawSetting.BillboardMatrixF.m[0][2] + y1 * GSYS.DrawSetting.BillboardMatrixF.m[1][2] + Pos.z ;

			DrawVert3D[ 1 ].pos.x = x2 * GSYS.DrawSetting.BillboardMatrixF.m[0][0] + y2 * GSYS.DrawSetting.BillboardMatrixF.m[1][0] + Pos.x ;
			DrawVert3D[ 1 ].pos.y = x2 * GSYS.DrawSetting.BillboardMatrixF.m[0][1] + y2 * GSYS.DrawSetting.BillboardMatrixF.m[1][1] + Pos.y ;
			DrawVert3D[ 1 ].pos.z = x2 * GSYS.DrawSetting.BillboardMatrixF.m[0][2] + y2 * GSYS.DrawSetting.BillboardMatrixF.m[1][2] + Pos.z ;

			DrawVert3D[ 2 ].pos.x = x4 * GSYS.DrawSetting.BillboardMatrixF.m[0][0] + y4 * GSYS.DrawSetting.BillboardMatrixF.m[1][0] + Pos.x ;
			DrawVert3D[ 2 ].pos.y = x4 * GSYS.DrawSetting.BillboardMatrixF.m[0][1] + y4 * GSYS.DrawSetting.BillboardMatrixF.m[1][1] + Pos.y ;
			DrawVert3D[ 2 ].pos.z = x4 * GSYS.DrawSetting.BillboardMatrixF.m[0][2] + y4 * GSYS.DrawSetting.BillboardMatrixF.m[1][2] + Pos.z ;

			DrawVert3D[ 3 ].pos.x = x3 * GSYS.DrawSetting.BillboardMatrixF.m[0][0] + y3 * GSYS.DrawSetting.BillboardMatrixF.m[1][0] + Pos.x ;
			DrawVert3D[ 3 ].pos.y = x3 * GSYS.DrawSetting.BillboardMatrixF.m[0][1] + y3 * GSYS.DrawSetting.BillboardMatrixF.m[1][1] + Pos.y ;
			DrawVert3D[ 3 ].pos.z = x3 * GSYS.DrawSetting.BillboardMatrixF.m[0][2] + y3 * GSYS.DrawSetting.BillboardMatrixF.m[1][2] + Pos.z ;

			DrawVert3D[4].pos = DrawVert3D[2].pos ;
			DrawVert3D[5].pos = DrawVert3D[1].pos ;

			*( ( DWORD * )&DrawVert3D[0].b ) = 
			*( ( DWORD * )&DrawVert3D[3].b )	= DiffuseColor ;

			DrawVert3D[4].u       = DrawVert3D[2].u       = DrawVert3D[0].u       = TexVert[0].u ;
			DrawVert3D[5].u       = DrawVert3D[3].u       = DrawVert3D[1].u       = TexVert[1].u ;
			DrawVert3D[5].v       = DrawVert3D[1].v       = DrawVert3D[0].v       = TexVert[0].v ;
			DrawVert3D[4].v       = DrawVert3D[3].v       = DrawVert3D[2].v       = TexVert[2].v ;

			// テクスチャーを描画する
			ADD4VERTEX_BILLBOARD
		}
		else
		{
			VECTOR DrawPos[ 4 ] ;

			// ビルボードの４頂点を得る
			{
				// ビルボード座標をワールド座標へ変換
				SrcVec2[ 0 ].x = x1 * GSYS.DrawSetting.BillboardMatrixF.m[0][0] + y1 * GSYS.DrawSetting.BillboardMatrixF.m[1][0] + Pos.x ;
				SrcVec2[ 0 ].y = x1 * GSYS.DrawSetting.BillboardMatrixF.m[0][1] + y1 * GSYS.DrawSetting.BillboardMatrixF.m[1][1] + Pos.y ;
				SrcVec2[ 0 ].z = x1 * GSYS.DrawSetting.BillboardMatrixF.m[0][2] + y1 * GSYS.DrawSetting.BillboardMatrixF.m[1][2] + Pos.z ;

				SrcVec2[ 1 ].x = x2 * GSYS.DrawSetting.BillboardMatrixF.m[0][0] + y2 * GSYS.DrawSetting.BillboardMatrixF.m[1][0] + Pos.x ;
				SrcVec2[ 1 ].y = x2 * GSYS.DrawSetting.BillboardMatrixF.m[0][1] + y2 * GSYS.DrawSetting.BillboardMatrixF.m[1][1] + Pos.y ;
				SrcVec2[ 1 ].z = x2 * GSYS.DrawSetting.BillboardMatrixF.m[0][2] + y2 * GSYS.DrawSetting.BillboardMatrixF.m[1][2] + Pos.z ;

				SrcVec2[ 2 ].x = x4 * GSYS.DrawSetting.BillboardMatrixF.m[0][0] + y4 * GSYS.DrawSetting.BillboardMatrixF.m[1][0] + Pos.x ;
				SrcVec2[ 2 ].y = x4 * GSYS.DrawSetting.BillboardMatrixF.m[0][1] + y4 * GSYS.DrawSetting.BillboardMatrixF.m[1][1] + Pos.y ;
				SrcVec2[ 2 ].z = x4 * GSYS.DrawSetting.BillboardMatrixF.m[0][2] + y4 * GSYS.DrawSetting.BillboardMatrixF.m[1][2] + Pos.z ;

				SrcVec2[ 3 ].x = x3 * GSYS.DrawSetting.BillboardMatrixF.m[0][0] + y3 * GSYS.DrawSetting.BillboardMatrixF.m[1][0] + Pos.x ;
				SrcVec2[ 3 ].y = x3 * GSYS.DrawSetting.BillboardMatrixF.m[0][1] + y3 * GSYS.DrawSetting.BillboardMatrixF.m[1][1] + Pos.y ;
				SrcVec2[ 3 ].z = x3 * GSYS.DrawSetting.BillboardMatrixF.m[0][2] + y3 * GSYS.DrawSetting.BillboardMatrixF.m[1][2] + Pos.z ;
			}

			// 頂点タイプによって処理を分岐
			switch( GD3D9.Device.DrawInfo.VertexType )
			{
			case VERTEXTYPE_BLENDTEX :
				// ブレンドテクスチャを使用する

				// 頂点バッファの取得
				GETVERTEX_QUAD( DrawVertB )

				DrawVertB[0].color = DiffuseColor ;
				DrawVertB[1].color = DiffuseColor ;
				DrawVertB[2].color = DiffuseColor ;
				DrawVertB[3].color = DiffuseColor ;
				DrawVertB[4].color = DiffuseColor ;
				DrawVertB[5].color = DiffuseColor ;

				// スクリーン座標に変換
				rhw = 1.0f / ( SrcVec2[0].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][3] + SrcVec2[0].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][3] + SrcVec2[0].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][3] + GSYS.DrawSetting.Blend3DMatrixF.m[3][3] ) ;
				z   = rhw  * ( SrcVec2[0].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][2] + SrcVec2[0].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][2] + SrcVec2[0].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][2] + GSYS.DrawSetting.Blend3DMatrixF.m[3][2] ) ;

				DrawPos[ 0 ].x = ( SrcVec2[0].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][0] + SrcVec2[0].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][0] + SrcVec2[0].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][0] + GSYS.DrawSetting.Blend3DMatrixF.m[3][0] ) * rhw ;
				DrawPos[ 0 ].y = ( SrcVec2[0].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][1] + SrcVec2[0].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][1] + SrcVec2[0].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][1] + GSYS.DrawSetting.Blend3DMatrixF.m[3][1] ) * rhw ;
				DrawPos[ 1 ].x = ( SrcVec2[1].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][0] + SrcVec2[1].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][0] + SrcVec2[1].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][0] + GSYS.DrawSetting.Blend3DMatrixF.m[3][0] ) * rhw ;
				DrawPos[ 1 ].y = ( SrcVec2[1].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][1] + SrcVec2[1].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][1] + SrcVec2[1].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][1] + GSYS.DrawSetting.Blend3DMatrixF.m[3][1] ) * rhw ;
				DrawPos[ 2 ].x = ( SrcVec2[2].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][0] + SrcVec2[2].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][0] + SrcVec2[2].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][0] + GSYS.DrawSetting.Blend3DMatrixF.m[3][0] ) * rhw ;
				DrawPos[ 2 ].y = ( SrcVec2[2].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][1] + SrcVec2[2].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][1] + SrcVec2[2].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][1] + GSYS.DrawSetting.Blend3DMatrixF.m[3][1] ) * rhw ;
				DrawPos[ 3 ].x = ( SrcVec2[3].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][0] + SrcVec2[3].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][0] + SrcVec2[3].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][0] + GSYS.DrawSetting.Blend3DMatrixF.m[3][0] ) * rhw ;
				DrawPos[ 3 ].y = ( SrcVec2[3].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][1] + SrcVec2[3].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][1] + SrcVec2[3].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][1] + GSYS.DrawSetting.Blend3DMatrixF.m[3][1] ) * rhw ;

				DrawVertB[0].pos.x = DrawPos[ 0 ].x ;
				DrawVertB[0].pos.y = DrawPos[ 0 ].y ;
				DrawVertB[1].pos.x = DrawPos[ 1 ].x ;
				DrawVertB[1].pos.y = DrawPos[ 1 ].y ;
				DrawVertB[2].pos.x = DrawPos[ 2 ].x ;
				DrawVertB[2].pos.y = DrawPos[ 2 ].y ;
				DrawVertB[3].pos.x = DrawPos[ 3 ].x ;
				DrawVertB[3].pos.y = DrawPos[ 3 ].y ;
				DrawVertB[4].pos.x = DrawPos[ 2 ].x ;
				DrawVertB[4].pos.y = DrawPos[ 2 ].y ;
				DrawVertB[5].pos.x = DrawPos[ 1 ].x ;
				DrawVertB[5].pos.y = DrawPos[ 1 ].y ;

				switch( GSYS.DrawSetting.BlendGraphPosMode )
				{
				default :
				case DX_BLENDGRAPH_POSMODE_DRAWGRAPH:
					u[ 0 ] = ( TexVert[0].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					u[ 1 ] = ( TexVert[1].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					v[ 0 ] = ( TexVert[0].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					v[ 1 ] = ( TexVert[2].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[0].u1 = u[ 0 ] ;
					DrawVertB[0].v1 = v[ 0 ] ;
					DrawVertB[1].u1 = u[ 1 ] ;
					DrawVertB[1].v1 = v[ 0 ] ;
					DrawVertB[2].u1 = u[ 0 ] ;
					DrawVertB[2].v1 = v[ 1 ] ;
					DrawVertB[3].u1 = u[ 1 ] ;
					DrawVertB[3].v1 = v[ 1 ] ;
					DrawVertB[4].u1 = u[ 0 ] ;
					DrawVertB[4].v1 = v[ 1 ] ;
					DrawVertB[5].u1 = u[ 1 ] ;
					DrawVertB[5].v1 = v[ 0 ] ;
					break ;

				case DX_BLENDGRAPH_POSMODE_SCREEN :
					DrawVertB[0].u1 = ( DrawPos[ 0 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[0].v1 = ( DrawPos[ 0 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[1].u1 = ( DrawPos[ 1 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[1].v1 = ( DrawPos[ 1 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[2].u1 = ( DrawPos[ 2 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[2].v1 = ( DrawPos[ 2 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[3].u1 = ( DrawPos[ 3 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[3].v1 = ( DrawPos[ 3 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[4].u1 = ( DrawPos[ 2 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[4].v1 = ( DrawPos[ 2 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[5].u1 = ( DrawPos[ 1 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[5].v1 = ( DrawPos[ 1 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					break ;
				}

				DrawVertB[0].u2 = TexVert[0].u ;
				DrawVertB[0].v2 = TexVert[0].v ;
				DrawVertB[1].u2 = TexVert[1].u ;
				DrawVertB[1].v2 = TexVert[0].v ;
				DrawVertB[2].u2 = TexVert[0].u ;
				DrawVertB[2].v2 = TexVert[2].v ;
				DrawVertB[3].u2 = TexVert[1].u ;
				DrawVertB[3].v2 = TexVert[2].v ;
				DrawVertB[4].u2 = TexVert[0].u ;
				DrawVertB[4].v2 = TexVert[2].v ;
				DrawVertB[5].u2 = TexVert[1].u ;
				DrawVertB[5].v2 = TexVert[0].v ;

				DrawVertB[0].pos.z = z ;
				DrawVertB[1].pos.z = z ;
				DrawVertB[2].pos.z = z ;
				DrawVertB[3].pos.z = z ;
				DrawVertB[4].pos.z = z ;
				DrawVertB[5].pos.z = z ;

				DrawVertB[0].rhw = rhw ;
				DrawVertB[1].rhw = rhw ;
				DrawVertB[2].rhw = rhw ;
				DrawVertB[3].rhw = rhw ;
				DrawVertB[4].rhw = rhw ;
				DrawVertB[5].rhw = rhw ;

				// テクスチャーを描画する
				if( DrawFlag )
				{
					ADD4VERTEX_BLENDTEX
				}
				else
				{
					if( dright  > DrawVertB[ 0 ].pos.x ) dright  = DrawVertB[ 0 ].pos.x ;
					if( dright  > DrawVertB[ 1 ].pos.x ) dright  = DrawVertB[ 1 ].pos.x ;
					if( dright  > DrawVertB[ 2 ].pos.x ) dright  = DrawVertB[ 2 ].pos.x ;
					if( dright  > DrawVertB[ 3 ].pos.x ) dright  = DrawVertB[ 3 ].pos.x ;

					if( dleft   < DrawVertB[ 0 ].pos.x ) dleft   = DrawVertB[ 0 ].pos.x ;
					if( dleft   < DrawVertB[ 1 ].pos.x ) dleft   = DrawVertB[ 1 ].pos.x ;
					if( dleft   < DrawVertB[ 2 ].pos.x ) dleft   = DrawVertB[ 2 ].pos.x ;
					if( dleft   < DrawVertB[ 3 ].pos.x ) dleft   = DrawVertB[ 3 ].pos.x ;

					if( dbottom > DrawVertB[ 0 ].pos.y ) dbottom = DrawVertB[ 0 ].pos.y ;
					if( dbottom > DrawVertB[ 1 ].pos.y ) dbottom = DrawVertB[ 1 ].pos.y ;
					if( dbottom > DrawVertB[ 2 ].pos.y ) dbottom = DrawVertB[ 2 ].pos.y ;
					if( dbottom > DrawVertB[ 3 ].pos.y ) dbottom = DrawVertB[ 3 ].pos.y ;

					if( dtop    < DrawVertB[ 0 ].pos.y ) dtop    = DrawVertB[ 0 ].pos.y ;
					if( dtop    < DrawVertB[ 1 ].pos.y ) dtop    = DrawVertB[ 1 ].pos.y ;
					if( dtop    < DrawVertB[ 2 ].pos.y ) dtop    = DrawVertB[ 2 ].pos.y ;
					if( dtop    < DrawVertB[ 3 ].pos.y ) dtop    = DrawVertB[ 3 ].pos.y ;
				}

				if( BlendGraphNoIncFlag == FALSE )
				{
					BlendDrawTex ++ ;
				}
				break ;

			case VERTEXTYPE_TEX :
				// ブレンドテクスチャを使用しない

				// 頂点バッファの取得
				GETVERTEX_QUAD( DrawVert )

				// スクリーン座標に変換
				rhw = 1.0f / ( SrcVec2[0].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][3] + SrcVec2[0].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][3] + SrcVec2[0].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][3] + GSYS.DrawSetting.Blend3DMatrixF.m[3][3] ) ;
				z   = rhw  * ( SrcVec2[0].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][2] + SrcVec2[0].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][2] + SrcVec2[0].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][2] + GSYS.DrawSetting.Blend3DMatrixF.m[3][2] ) ;

				DrawVert[0].pos.x = ( SrcVec2[0].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][0] + SrcVec2[0].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][0] + SrcVec2[0].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][0] + GSYS.DrawSetting.Blend3DMatrixF.m[3][0] ) * rhw ;
				DrawVert[0].pos.y = ( SrcVec2[0].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][1] + SrcVec2[0].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][1] + SrcVec2[0].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][1] + GSYS.DrawSetting.Blend3DMatrixF.m[3][1] ) * rhw ;
				DrawVert[1].pos.x = ( SrcVec2[1].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][0] + SrcVec2[1].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][0] + SrcVec2[1].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][0] + GSYS.DrawSetting.Blend3DMatrixF.m[3][0] ) * rhw ;
				DrawVert[1].pos.y = ( SrcVec2[1].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][1] + SrcVec2[1].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][1] + SrcVec2[1].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][1] + GSYS.DrawSetting.Blend3DMatrixF.m[3][1] ) * rhw ;
				DrawVert[2].pos.x = ( SrcVec2[2].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][0] + SrcVec2[2].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][0] + SrcVec2[2].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][0] + GSYS.DrawSetting.Blend3DMatrixF.m[3][0] ) * rhw ;
				DrawVert[2].pos.y = ( SrcVec2[2].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][1] + SrcVec2[2].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][1] + SrcVec2[2].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][1] + GSYS.DrawSetting.Blend3DMatrixF.m[3][1] ) * rhw ;
				DrawVert[3].pos.x = ( SrcVec2[3].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][0] + SrcVec2[3].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][0] + SrcVec2[3].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][0] + GSYS.DrawSetting.Blend3DMatrixF.m[3][0] ) * rhw ;
				DrawVert[3].pos.y = ( SrcVec2[3].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][1] + SrcVec2[3].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][1] + SrcVec2[3].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][1] + GSYS.DrawSetting.Blend3DMatrixF.m[3][1] ) * rhw ;
				DrawVert[4].pos.x = ( SrcVec2[2].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][0] + SrcVec2[2].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][0] + SrcVec2[2].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][0] + GSYS.DrawSetting.Blend3DMatrixF.m[3][0] ) * rhw ;
				DrawVert[4].pos.y = ( SrcVec2[2].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][1] + SrcVec2[2].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][1] + SrcVec2[2].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][1] + GSYS.DrawSetting.Blend3DMatrixF.m[3][1] ) * rhw ;
				DrawVert[5].pos.x = ( SrcVec2[1].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][0] + SrcVec2[1].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][0] + SrcVec2[1].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][0] + GSYS.DrawSetting.Blend3DMatrixF.m[3][0] ) * rhw ;
				DrawVert[5].pos.y = ( SrcVec2[1].x * GSYS.DrawSetting.Blend3DMatrixF.m[0][1] + SrcVec2[1].y * GSYS.DrawSetting.Blend3DMatrixF.m[1][1] + SrcVec2[1].z * GSYS.DrawSetting.Blend3DMatrixF.m[2][1] + GSYS.DrawSetting.Blend3DMatrixF.m[3][1] ) * rhw ;

				DrawVert[0].pos.z = z ;
				DrawVert[1].pos.z = z ;
				DrawVert[2].pos.z = z ;
				DrawVert[3].pos.z = z ;
				DrawVert[4].pos.z = z ;
				DrawVert[5].pos.z = z ;

				DrawVert[0].rhw = rhw ;
				DrawVert[1].rhw = rhw ;
				DrawVert[2].rhw = rhw ;
				DrawVert[3].rhw = rhw ;
				DrawVert[4].rhw = rhw ;
				DrawVert[5].rhw = rhw ;

				DrawVert[0].color = DiffuseColor ;
				DrawVert[1].color = DiffuseColor ;
				DrawVert[2].color = DiffuseColor ;
				DrawVert[3].color = DiffuseColor ;
				DrawVert[4].color = DiffuseColor ;
				DrawVert[5].color = DiffuseColor ;

				DrawVert[0].u = TexVert[0].u ;
				DrawVert[1].u = TexVert[1].u ;
				DrawVert[2].u = TexVert[0].u ;
				DrawVert[3].u = TexVert[1].u ;
				DrawVert[4].u = TexVert[0].u ;
				DrawVert[5].u = TexVert[1].u ;

				DrawVert[0].v = TexVert[0].v ;
				DrawVert[1].v = TexVert[0].v ;
				DrawVert[2].v = TexVert[2].v ;
				DrawVert[3].v = TexVert[2].v ;
				DrawVert[4].v = TexVert[2].v ;
				DrawVert[5].v = TexVert[0].v ;

				// テクスチャーを描画する
				if( DrawFlag )
				{
					ADD4VERTEX_TEX
				}
				else
				{
					if( dright  > DrawVert[ 0 ].pos.x ) dright  = DrawVert[ 0 ].pos.x ;
					if( dright  > DrawVert[ 1 ].pos.x ) dright  = DrawVert[ 1 ].pos.x ;
					if( dright  > DrawVert[ 2 ].pos.x ) dright  = DrawVert[ 2 ].pos.x ;
					if( dright  > DrawVert[ 3 ].pos.x ) dright  = DrawVert[ 3 ].pos.x ;

					if( dleft   < DrawVert[ 0 ].pos.x ) dleft   = DrawVert[ 0 ].pos.x ;
					if( dleft   < DrawVert[ 1 ].pos.x ) dleft   = DrawVert[ 1 ].pos.x ;
					if( dleft   < DrawVert[ 2 ].pos.x ) dleft   = DrawVert[ 2 ].pos.x ;
					if( dleft   < DrawVert[ 3 ].pos.x ) dleft   = DrawVert[ 3 ].pos.x ;

					if( dbottom > DrawVert[ 0 ].pos.y ) dbottom = DrawVert[ 0 ].pos.y ;
					if( dbottom > DrawVert[ 1 ].pos.y ) dbottom = DrawVert[ 1 ].pos.y ;
					if( dbottom > DrawVert[ 2 ].pos.y ) dbottom = DrawVert[ 2 ].pos.y ;
					if( dbottom > DrawVert[ 3 ].pos.y ) dbottom = DrawVert[ 3 ].pos.y ;

					if( dtop    < DrawVert[ 0 ].pos.y ) dtop    = DrawVert[ 0 ].pos.y ;
					if( dtop    < DrawVert[ 1 ].pos.y ) dtop    = DrawVert[ 1 ].pos.y ;
					if( dtop    < DrawVert[ 2 ].pos.y ) dtop    = DrawVert[ 2 ].pos.y ;
					if( dtop    < DrawVert[ 3 ].pos.y ) dtop    = DrawVert[ 3 ].pos.y ;
				}
				break ;
			}
		}
	}

	if( DrawFlag == FALSE )
	{
		DrawArea->left   = _FTOL( dleft   ) - 1 ;
		DrawArea->right  = _FTOL( dright  ) + 1 ;
		DrawArea->top    = _FTOL( dtop    ) - 1 ;
		DrawArea->bottom = _FTOL( dbottom ) + 1 ;
	}

	// 終了
	return 0 ;
}

// ハードウエアアクセラレータ使用版 DrawGraph
extern	int		Graphics_D3D9_DrawGraph( int x, int y, float xf, float yf, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, int IntFlag )
{
	VERTEX_2D			*DrawVert ;
	VERTEX_BLENDTEX_2D	*DrawVertB ;
	IMAGEDATA_HARD_DRAW	*DrawTex ;
	IMAGEDATA_HARD_DRAW	*BlendDrawTex = NULL ;
	IMAGEDATA_ORIG		*Orig ;
	IMAGEDATA_HARD_VERT	*TexVert ;
	IMAGEDATA_HARD_VERT	*BlendTexVert ;
	DWORD				DiffuseColor ;
	int					DrawTexNum ;
	int					i ;
	int					j ;
	int					Flag ;
	int					BlendGraphNoIncFlag ;
	float				left ;
	float				right ;
	float				top ;
	float				bottom ;
	float				Vx ;
	float				Vy ;
	float				VBx ;
	float				VBy ;
	DWORD				drawz ;
	float				u[ 2 ] ;
	float				v[ 2 ] ;

	Orig = Image->Orig ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	// float座標補正
	xf -= 0.5F ;
	yf -= 0.5F ;

	// 描画準備
	if( GD3D9.Device.DrawInfo.BeginSceneFlag == FALSE )
	{
		Graphics_D3D9_BeginScene() ;
	}
	Flag = TransFlag | ( IntFlag << 1 ) ;
	DX_D3D9_DRAWPREP_TEX( Orig, Image->Hard.Draw[ 0 ].Tex->PF->D3D9.Texture, Flag )

	// 頂点データを取得
	DiffuseColor = GD3D9.Device.DrawInfo.DiffuseColor ;

	// 描画情報の数をセット
	DrawTexNum = Image->Hard.DrawNum ;

	// 描画情報の数がブレンド画像と異なっていたら０番目のテクスチャだけを使用する
	BlendGraphNoIncFlag = FALSE ;
	if( BlendImage != NULL && BlendImage->Hard.DrawNum != Image->Hard.DrawNum )
	{
		BlendGraphNoIncFlag = TRUE ;
	}

	// 描画情報配列のアドレスをセットしておく
	DrawTex = Image->Hard.Draw ;
	if( BlendImage != NULL )
	{
		BlendDrawTex = BlendImage->Hard.Draw ;
	}

	// Ｚバッファに書き込むＺ値をセットする
	drawz = *( ( DWORD * )&GSYS.DrawSetting.DrawZ ) ;

	// 描画範囲からはみ出ているかどうかで処理を分岐
	if( GSYS.DrawSetting.Valid2DMatrix || IN_DRAWAREA_CHECK( x, y, x + Image->WidthI, y + Image->HeightI ) )
//	if( x + Image->Width < GSYS.DrawSetting.DrawArea.right && x > GSYS.DrawSetting.DrawArea.left &&
//		y + Image->Height < GSYS.DrawSetting.DrawArea.bottom && y > GSYS.DrawSetting.DrawArea.right )
	{
		// はみ出ていない場合

		// 頂点タイプによって処理を分岐
		if( GD3D9.Device.DrawInfo.VertexType == VERTEXTYPE_BLENDTEX )
		{
			// ブレンドテクスチャを使用する

			// テクスチャーの数だけ繰り返す
			for( i = 0 ; i < DrawTexNum ; i ++, DrawTex ++ )
			{
				// 描画するテクスチャーのセット
				Graphics_D3D9_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D9.Texture ) ;
				Graphics_D3D9_DrawSetting_SetBlendTexture( BlendDrawTex->Tex->PF->D3D9.Texture, BlendDrawTex->Tex->TexWidth, BlendDrawTex->Tex->TexHeight );

				if( GD3D9.Device.DrawSetting.ChangeTextureFlag )
				{
					Graphics_D3D9_DeviceState_RefreshBlendState() ;
				}

				// 頂点バッファの取得
				GETVERTEX_QUAD( DrawVertB )
				
				TexVert = DrawTex->Vertex ;
				BlendTexVert = BlendDrawTex->Vertex ;

				DrawVertB[0].color = ( DWORD )DiffuseColor ; 
				DrawVertB[3].color = ( DWORD )DiffuseColor ;

				DrawVertB[0].pos.x = TexVert[0].x + xf ;
				DrawVertB[0].pos.y = TexVert[0].y + yf ;
				DrawVertB[1].pos.x = TexVert[1].x + xf ;
				DrawVertB[1].pos.y = TexVert[0].y + yf ;
				DrawVertB[2].pos.x = TexVert[0].x + xf ;
				DrawVertB[2].pos.y = TexVert[2].y + yf ;
				DrawVertB[3].pos.x = TexVert[1].x + xf ;
				DrawVertB[3].pos.y = TexVert[2].y + yf ;
				DrawVertB[4].pos.x = TexVert[0].x + xf ;
				DrawVertB[4].pos.y = TexVert[2].y + yf ;
				DrawVertB[5].pos.x = TexVert[1].x + xf ;
				DrawVertB[5].pos.y = TexVert[0].y + yf ;

				switch( GSYS.DrawSetting.BlendGraphPosMode )
				{
				default :
				case DX_BLENDGRAPH_POSMODE_DRAWGRAPH:
					u[ 0 ] = TexVert[0].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ;
					u[ 1 ] = TexVert[1].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ;
					v[ 0 ] = TexVert[0].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ;
					v[ 1 ] = TexVert[2].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ;
					break ;

				case DX_BLENDGRAPH_POSMODE_SCREEN :
					u[ 0 ] = ( xf + TexVert[0].x ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ;
					u[ 1 ] = ( xf + TexVert[1].x ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ;
					v[ 0 ] = ( yf + TexVert[0].y ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ;
					v[ 1 ] = ( yf + TexVert[2].y ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ;
					break ;
				}

				u[ 0 ] *= GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
				u[ 1 ] *= GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
				v[ 0 ] *= GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
				v[ 1 ] *= GD3D9.Device.DrawSetting.InvBlendTextureHeight ;

				DrawVertB[0].u1 = u[ 0 ] ;
				DrawVertB[0].v1 = v[ 0 ] ;
				DrawVertB[1].u1 = u[ 1 ] ;
				DrawVertB[1].v1 = v[ 0 ] ;
				DrawVertB[2].u1 = u[ 0 ] ;
				DrawVertB[2].v1 = v[ 1 ] ;
				DrawVertB[3].u1 = u[ 1 ] ;
				DrawVertB[3].v1 = v[ 1 ] ;
				DrawVertB[4].u1 = u[ 0 ] ;
				DrawVertB[4].v1 = v[ 1 ] ;
				DrawVertB[5].u1 = u[ 1 ] ;
				DrawVertB[5].v1 = v[ 0 ] ;

				DrawVertB[0].u2 = TexVert[0].u ;
				DrawVertB[0].v2 = TexVert[0].v ;
				DrawVertB[1].u2 = TexVert[1].u ;
				DrawVertB[1].v2 = TexVert[0].v ;
				DrawVertB[2].u2 = TexVert[0].u ;
				DrawVertB[2].v2 = TexVert[2].v ;
				DrawVertB[3].u2 = TexVert[1].u ;
				DrawVertB[3].v2 = TexVert[2].v ;
				DrawVertB[4].u2 = TexVert[0].u ;
				DrawVertB[4].v2 = TexVert[2].v ;
				DrawVertB[5].u2 = TexVert[1].u ;
				DrawVertB[5].v2 = TexVert[0].v ;

				*((DWORD *)&DrawVertB[0].pos.z) = drawz ;
				*((DWORD *)&DrawVertB[1].pos.z) = drawz ;
				*((DWORD *)&DrawVertB[2].pos.z) = drawz ;
				*((DWORD *)&DrawVertB[3].pos.z) = drawz ;
				*((DWORD *)&DrawVertB[4].pos.z) = drawz ;
				*((DWORD *)&DrawVertB[5].pos.z) = drawz ;

				// テクスチャーを描画する
				ADD4VERTEX_BLENDTEX

				if( BlendGraphNoIncFlag == FALSE )
				{
					BlendDrawTex ++ ;
				}
			}
		}
		else
		{
			// ブレンドテクスチャを使用しない

			// テクスチャーの数だけ繰り返す
			i = 0 ;
			for(;;)
			{
				// 頂点バッファの取得
				GETVERTEX_QUAD( DrawVert )

				// 頂点データ準備
				TexVert = DrawTex->Vertex ;
				DrawVert[0].color = DiffuseColor ;
				DrawVert[3].color = DiffuseColor ;

				DrawVert[0].pos.x = TexVert[0].x + xf ;
				DrawVert[0].pos.y = TexVert[0].y + yf ;
				DrawVert[1].pos.x = TexVert[1].x + xf ;
				DrawVert[1].pos.y = TexVert[0].y + yf ;
				DrawVert[2].pos.x = TexVert[0].x + xf ;
				DrawVert[2].pos.y = TexVert[2].y + yf ;
				DrawVert[3].pos.x = TexVert[1].x + xf ;
				DrawVert[3].pos.y = TexVert[2].y + yf ;
				DrawVert[4].pos.x = TexVert[0].x + xf ;
				DrawVert[4].pos.y = TexVert[2].y + yf ;
				DrawVert[5].pos.x = TexVert[1].x + xf ;
				DrawVert[5].pos.y = TexVert[0].y + yf ;

				DrawVert[0].u = TexVert[0].u ;
				DrawVert[0].v = TexVert[0].v ;
				DrawVert[1].u = TexVert[1].u ;
				DrawVert[1].v = TexVert[0].v ;
				DrawVert[2].u = TexVert[0].u ;
				DrawVert[2].v = TexVert[2].v ;
				DrawVert[3].u = TexVert[1].u ;
				DrawVert[3].v = TexVert[2].v ;
				DrawVert[4].u = TexVert[0].u ;
				DrawVert[4].v = TexVert[2].v ;
				DrawVert[5].u = TexVert[1].u ;
				DrawVert[5].v = TexVert[0].v ;

				*((DWORD *)&DrawVert[0].pos.z) = drawz ;
				*((DWORD *)&DrawVert[1].pos.z) = drawz ;
				*((DWORD *)&DrawVert[2].pos.z) = drawz ;
				*((DWORD *)&DrawVert[3].pos.z) = drawz ;
				*((DWORD *)&DrawVert[4].pos.z) = drawz ;
				*((DWORD *)&DrawVert[5].pos.z) = drawz ;

				// テクスチャーを描画する
				ADD4VERTEX_TEX

				i ++ ;
				if( i >= DrawTexNum ) break ;

				// 描画するテクスチャーのセット
				DrawTex ++ ;
				Graphics_D3D9_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D9.Texture ) ;
				Graphics_D3D9_DeviceState_RefreshBlendState() ;
			}
		}
	}
	else
	{
		// はみ出ている場合

		// 完全にはみ出ていたら何もせず終了
		if( OUT_DRAWAREA_CHECK( x, y, x + Image->WidthI, y + Image->HeightI ) ) return 0 ;
//		if( x > GSYS.DrawSetting.DrawArea.right || x + Image->Width < GSYS.DrawSetting.DrawArea.left ||
//			y > GSYS.DrawSetting.DrawArea.bottom || y + Image->Height < GSYS.DrawSetting.DrawArea.top ) return 0 ;

		// 描画矩形の保存
		left	= GSYS.DrawSetting.DrawAreaF.left   ;
		top		= GSYS.DrawSetting.DrawAreaF.top    ;
		right	= GSYS.DrawSetting.DrawAreaF.right  ;
		bottom	= GSYS.DrawSetting.DrawAreaF.bottom ;

		// 頂点タイプによって処理を分岐
		switch( GD3D9.Device.DrawInfo.VertexType )
		{
		case VERTEXTYPE_BLENDTEX :
			// ブレンドテクスチャを使用する

			// テクスチャーの数だけ繰り返す
			for( i = 0 ; i < DrawTexNum ; i ++, DrawTex ++ )
			{
				VECTOR DrawPos[ 4 ] ;
				float u1[ 4 ] ;
				float v1[ 4 ] ;
				float u2[ 4 ] ;
				float v2[ 4 ] ;

				// 描画するテクスチャーのセット
				Graphics_D3D9_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D9.Texture ) ;
				Graphics_D3D9_DrawSetting_SetBlendTexture( BlendDrawTex->Tex->PF->D3D9.Texture, BlendDrawTex->Tex->TexWidth, BlendDrawTex->Tex->TexHeight );

				if( GD3D9.Device.DrawSetting.ChangeTextureFlag )
				{
					Graphics_D3D9_DeviceState_RefreshBlendState() ;
				}

				// 頂点バッファの取得
				GETVERTEX_QUAD( DrawVertB )
				
				// 頂点データを初期化する
				TexVert = DrawTex->Vertex ;
				BlendTexVert = BlendDrawTex->Vertex ;

				Vx  = 1.0F / ( float )DrawTex->Tex->TexWidth  ;
				Vy  = 1.0F / ( float )DrawTex->Tex->TexHeight ;
				VBx = GD3D9.Device.DrawSetting.InvBlendTextureWidth  ;
				VBy = GD3D9.Device.DrawSetting.InvBlendTextureHeight ;

				DrawVertB[0].color = ( DWORD )DiffuseColor ;
				DrawVertB[3].color = ( DWORD )DiffuseColor ;

				DrawPos[0].x = TexVert[0].x + xf ;
				DrawPos[0].y = TexVert[0].y + yf ;
				DrawPos[1].x = TexVert[1].x + xf ;
				DrawPos[1].y = TexVert[0].y + yf ;
				DrawPos[2].x = TexVert[0].x + xf ;
				DrawPos[2].y = TexVert[2].y + yf ;
				DrawPos[3].x = TexVert[1].x + xf ;
				DrawPos[3].y = TexVert[2].y + yf ;

				u2[2] = u2[0] = TexVert[0].u ;
				u2[3] = u2[1] = TexVert[1].u ;
				v2[1] = v2[0] = TexVert[0].v ;
				v2[3] = v2[2] = TexVert[2].v ;

				switch( GSYS.DrawSetting.BlendGraphPosMode )
				{
				default :
				case DX_BLENDGRAPH_POSMODE_DRAWGRAPH:
					u1[2] = u1[0] = ( TexVert[0].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					u1[3] = u1[1] = ( TexVert[1].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					v1[1] = v1[0] = ( TexVert[0].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					v1[3] = v1[2] = ( TexVert[2].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					break ;

				case DX_BLENDGRAPH_POSMODE_SCREEN :
					u1[2] = u1[0] = ( ( xf + TexVert[0].x ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					u1[3] = u1[1] = ( ( xf + TexVert[1].x ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					v1[1] = v1[0] = ( ( yf + TexVert[0].y ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					v1[3] = v1[2] = ( ( yf + TexVert[2].y ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					break ;
				}

				for( j = 0 ; j < 4 ; j ++ )
				{
					// 描画矩形から出ていた場合の補正処理
					if( DrawPos[ j ].x < left )
					{
						u2[ j ] += ( left - DrawPos[ j ].x ) * Vx ;
						u1[ j ] += ( left - DrawPos[ j ].x ) * VBx ;
						DrawPos[ j ].x = left ;
					}
					else
					if( DrawPos[ j ].x > right )
					{
						u2[ j ] -= ( DrawPos[ j ].x - right ) * Vx ;
						u1[ j ] -= ( DrawPos[ j ].x - right ) * VBx ;
						DrawPos[ j ].x = right ;
					}	

					if( DrawPos[ j ].y < top )
					{
						v2[ j ] += ( top - DrawPos[ j ].y ) * Vy ;
						v1[ j ] += ( top - DrawPos[ j ].y ) * VBy ;
						DrawPos[ j ].y = top ;
					}
					else
					if( DrawPos[ j ].y > bottom )
					{
						v2[ j ] -= ( DrawPos[ j ].y - bottom ) * Vy ;
						v1[ j ] -= ( DrawPos[ j ].y - bottom ) * VBy ;
						DrawPos[ j ].y = bottom ;
					}
				}

				DrawVertB[0].pos.x = DrawPos[0].x ;
				DrawVertB[0].pos.y = DrawPos[0].y ;
				DrawVertB[1].pos.x = DrawPos[1].x ;
				DrawVertB[1].pos.y = DrawPos[1].y ;
				DrawVertB[2].pos.x = DrawPos[2].x ;
				DrawVertB[2].pos.y = DrawPos[2].y ;
				DrawVertB[3].pos.x = DrawPos[3].x ;
				DrawVertB[3].pos.y = DrawPos[3].y ;
				DrawVertB[4].pos.x = DrawPos[2].x ;
				DrawVertB[4].pos.y = DrawPos[2].y ;
				DrawVertB[5].pos.x = DrawPos[1].x ;
				DrawVertB[5].pos.y = DrawPos[1].y ;

				DrawVertB[0].u1 = u1[0] ;
				DrawVertB[0].v1 = v1[0] ;
				DrawVertB[1].u1 = u1[1] ;
				DrawVertB[1].v1 = v1[1] ;
				DrawVertB[2].u1 = u1[2] ;
				DrawVertB[2].v1 = v1[2] ;
				DrawVertB[3].u1 = u1[3] ;
				DrawVertB[3].v1 = v1[3] ;
				DrawVertB[4].u1 = u1[2] ;
				DrawVertB[4].v1 = v1[2] ;
				DrawVertB[5].u1 = u1[1] ;
				DrawVertB[5].v1 = v1[1] ;

				DrawVertB[0].u2 = u2[0] ;
				DrawVertB[0].v2 = v2[0] ;
				DrawVertB[1].u2 = u2[1] ;
				DrawVertB[1].v2 = v2[1] ;
				DrawVertB[2].u2 = u2[2] ;
				DrawVertB[2].v2 = v2[2] ;
				DrawVertB[3].u2 = u2[3] ;
				DrawVertB[3].v2 = v2[3] ;
				DrawVertB[4].u2 = u2[2] ;
				DrawVertB[4].v2 = v2[2] ;
				DrawVertB[5].u2 = u2[1] ;
				DrawVertB[5].v2 = v2[1] ;

				*((DWORD *)&DrawVertB[0].pos.z) = drawz ;
				*((DWORD *)&DrawVertB[1].pos.z) = drawz ;
				*((DWORD *)&DrawVertB[2].pos.z) = drawz ;
				*((DWORD *)&DrawVertB[3].pos.z) = drawz ;
				*((DWORD *)&DrawVertB[4].pos.z) = drawz ;
				*((DWORD *)&DrawVertB[5].pos.z) = drawz ;

				// テクスチャーを描画する
				ADD4VERTEX_BLENDTEX

				if( BlendGraphNoIncFlag == FALSE )
				{
					BlendDrawTex ++ ;
				}
			}
			break ;
		
		case VERTEXTYPE_TEX :
			// ブレンドテクスチャを使用しない

			// テクスチャーの数だけ繰り返す
			i = 0 ;
			for(;;)
			{
				VECTOR DrawPos[ 4 ] ;
				float u4[ 4 ] ;
				float v4[ 4 ] ;

				// 頂点バッファの取得
				GETVERTEX_QUAD( DrawVert )
				
				// 頂点データ準備
				TexVert = DrawTex->Vertex ;
				DrawVert[0].color	= DiffuseColor ;
				DrawVert[3].color	= DiffuseColor ;

				DrawPos[0].x = TexVert[0].x + xf ;
				DrawPos[0].y = TexVert[0].y + yf ;
				DrawPos[1].x = TexVert[1].x + xf ;
				DrawPos[1].y = TexVert[0].y + yf ;
				DrawPos[2].x = TexVert[0].x + xf ;
				DrawPos[2].y = TexVert[2].y + yf ;
				DrawPos[3].x = TexVert[1].x + xf ;
				DrawPos[3].y = TexVert[2].y + yf ;

				u4[2] = u4[0] = TexVert[0].u ;
				u4[3] = u4[1] = TexVert[1].u ;
				v4[1] = v4[0] = TexVert[0].v ;
				v4[3] = v4[2] = TexVert[2].v ;
				
				// 頂点データを初期化する
				Vx	= 1.0F / ( float )DrawTex->Tex->TexWidth ;
				Vy	= 1.0F / ( float )DrawTex->Tex->TexHeight ;
				for( j = 0 ; j < 4 ; j ++ )
				{
					// 描画矩形から出ていた場合の補正処理
					if( DrawPos[ j ].x < left )
					{
						u4[ j ] += ( left - DrawPos[ j ].x ) * Vx ;
						DrawPos[ j ].x = left ;
					}
					else
					if( DrawPos[ j ].x > right )
					{
						u4[ j ] -= ( DrawPos[ j ].x - right ) * Vx ;
						DrawPos[ j ].x = right ;
					}	

					if( DrawPos[ j ].y < top )
					{
						v4[ j ] += ( top - DrawPos[ j ].y ) * Vy ;
						DrawPos[ j ].y = top ;
					}
					else
					if( DrawPos[ j ].y > bottom )
					{
						v4[ j ] -= ( DrawPos[ j ].y - bottom ) * Vy ;
						DrawPos[ j ].y = bottom ;
					}
				}

				DrawVert[0].pos.x = DrawPos[0].x ;
				DrawVert[0].pos.y = DrawPos[0].y ;
				DrawVert[1].pos.x = DrawPos[1].x ;
				DrawVert[1].pos.y = DrawPos[1].y ;
				DrawVert[2].pos.x = DrawPos[2].x ;
				DrawVert[2].pos.y = DrawPos[2].y ;
				DrawVert[3].pos.x = DrawPos[3].x ;
				DrawVert[3].pos.y = DrawPos[3].y ;
				DrawVert[4].pos.x = DrawPos[2].x ;
				DrawVert[4].pos.y = DrawPos[2].y ;
				DrawVert[5].pos.x = DrawPos[1].x ;
				DrawVert[5].pos.y = DrawPos[1].y ;

				DrawVert[0].u = u4[0] ;
				DrawVert[0].v = v4[0] ;
				DrawVert[1].u = u4[1] ;
				DrawVert[1].v = v4[1] ;
				DrawVert[2].u = u4[2] ;
				DrawVert[2].v = v4[2] ;
				DrawVert[3].u = u4[3] ;
				DrawVert[3].v = v4[3] ;
				DrawVert[4].u = u4[2] ;
				DrawVert[4].v = v4[2] ;
				DrawVert[5].u = u4[1] ;
				DrawVert[5].v = v4[1] ;

				*((DWORD *)&DrawVert[0].pos.z) = drawz ;
				*((DWORD *)&DrawVert[1].pos.z) = drawz ;
				*((DWORD *)&DrawVert[2].pos.z) = drawz ;
				*((DWORD *)&DrawVert[3].pos.z) = drawz ;
				*((DWORD *)&DrawVert[4].pos.z) = drawz ;
				*((DWORD *)&DrawVert[5].pos.z) = drawz ;

				// テクスチャーを描画する
				ADD4VERTEX_TEX

				i ++ ;
				if( i >= DrawTexNum ) break ;

				// 描画するテクスチャーのセット
				DrawTex ++ ;
				Graphics_D3D9_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D9.Texture ) ;
				Graphics_D3D9_DeviceState_RefreshBlendState() ;
			}
			break ;
		}
	}

	// 終了
	return 0 ;
}

// ハードウエアアクセラレータ使用版 DrawExtendGraph
extern	int		Graphics_D3D9_DrawExtendGraph( int x1, int y1, int x2, int y2, float x1f, float y1f, float x2f, float y2f, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, int IntFlag )
{
	VERTEX_2D *DrawVert ;
	VERTEX_BLENDTEX_2D *DrawVertB ; 
	IMAGEDATA_HARD_DRAW *DrawTex ;
	IMAGEDATA_HARD_DRAW *BlendDrawTex = NULL ;
	IMAGEDATA_HARD_VERT *TexVert ;
	IMAGEDATA_HARD_VERT *BlendTexVert ;
	IMAGEDATA_ORIG *Orig ;
	int DrawTexNum ;
	int i, j ;
	int BlendGraphNoIncFlag ;
	float GraphExRateX, GraphExRateY ;
//	int ReverseX = FALSE, ReverseY = FALSE ;
	int Reverse ;
//	int AdjX = FALSE, AdjY = FALSE ;
	float left, right, top, bottom ;
	float Vx, Vy ;
	float VBx, VBy ;
	DWORD DiffuseColor ;
	DWORD drawz ;
	int Flag ;
	float XBuf[ 2 ] ;
	float YBuf[ 2 ] ;
	float u[ 2 ] ;
	float v[ 2 ] ;

	Orig = Image->Orig ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	// 描画判定
//	if( x2 - x1 == Image->Width && y2 - y1 == Image->Height ) return G_D3D_DrawGraph( x1, y1, GraphData, TransFlag ) ; 	

	// 幅がなかった場合はエラー
	if( x1 == x2 || y1 == y2 ) return -1 ;

	// 描画の準備
	if( GD3D9.Device.DrawInfo.BeginSceneFlag == FALSE ) Graphics_D3D9_BeginScene() ;
	Flag = TransFlag | ( IntFlag ? ( GSYS.DrawSetting.DrawMode == DX_DRAWMODE_BILINEAR ? 0 : DX_D3D9_DRAWPREP_VECTORINT ) : 0 ) ;
	DX_D3D9_DRAWPREP_TEX( Orig, Image->Hard.Draw[ 0 ].Tex->PF->D3D9.Texture, Flag )

	// ディフューズカラーの取得
	DiffuseColor = GD3D9.Device.DrawInfo.DiffuseColor ;

	// 描画情報の数をセット
	DrawTexNum = Image->Hard.DrawNum ;

	// 描画情報の数がブレンド画像と異なっていたら０番目のテクスチャだけを使用する
	BlendGraphNoIncFlag = FALSE ;
	if( BlendImage != NULL && BlendImage->Hard.DrawNum != Image->Hard.DrawNum )
	{
		BlendGraphNoIncFlag = TRUE ;
	}

	// 描画情報配列のアドレスをセットしておく
	DrawTex = Image->Hard.Draw ;
	if( BlendImage != NULL )
	{
		BlendDrawTex = BlendImage->Hard.Draw ;
	}

	// Ｚバッファに書き込むＺ値をセットする
	drawz = *((DWORD *)&GSYS.DrawSetting.DrawZ);

	// 拡大率を取得しておく
	GraphExRateX = ( x2f - x1f ) / Image->WidthF ;
	GraphExRateY = ( y2f - y1f ) / Image->HeightF ;
	Reverse = ( GraphExRateX < 0 ) != ( GraphExRateY < 0 ) ;

	x1f -= 0.5F ;
	y1f -= 0.5F ;

	if( x1 > x2 ){ i = x2; x2 = x1; x1 = i; }
	if( y1 > y2 ){ i = y2; y2 = y1; y1 = i; }

	// 描画範囲からはみ出ているかどうかで処理を分岐
	if( GSYS.DrawSetting.Valid2DMatrix || IN_DRAWAREA_CHECK( x1, y1, x2, y2 ) )
	{
		// 頂点タイプによって処理を分岐
		switch( GD3D9.Device.DrawInfo.VertexType )
		{
		case VERTEXTYPE_BLENDTEX :	// ブレンドテクスチャを使用する
			// テクスチャーの数だけ繰り返す
			for( i = 0 ; i < DrawTexNum ; i ++, DrawTex ++ )
			{
				// 描画するテクスチャーのセット
				Graphics_D3D9_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D9.Texture ) ;
				Graphics_D3D9_DrawSetting_SetBlendTexture( BlendDrawTex->Tex->PF->D3D9.Texture, BlendDrawTex->Tex->TexWidth, BlendDrawTex->Tex->TexHeight );

				if( GD3D9.Device.DrawSetting.ChangeTextureFlag )
					Graphics_D3D9_DeviceState_RefreshBlendState() ;

				// 頂点データを初期化する
				GETVERTEX_QUAD( DrawVertB )
				
				TexVert = DrawTex->Vertex ;
				BlendTexVert = BlendDrawTex->Vertex ;

				DrawVertB[0].color = ( DWORD )DiffuseColor ;
				DrawVertB[3].color = ( DWORD )DiffuseColor ;

				XBuf[ 0 ] = TexVert[0].x * GraphExRateX + x1f ;
				XBuf[ 1 ] = TexVert[1].x * GraphExRateX + x1f ;
				YBuf[ 0 ] = TexVert[0].y * GraphExRateY + y1f ;
				YBuf[ 1 ] = TexVert[2].y * GraphExRateY + y1f ;
				DrawVertB[0].pos.x = XBuf[ 0 ] ;
				DrawVertB[0].pos.y = YBuf[ 0 ] ;
				DrawVertB[1].pos.x = XBuf[ 1 ] ;
				DrawVertB[1].pos.y = YBuf[ 0 ] ;
				DrawVertB[2].pos.x = XBuf[ 0 ] ;
				DrawVertB[2].pos.y = YBuf[ 1 ] ;
				DrawVertB[3].pos.x = XBuf[ 1 ] ;
				DrawVertB[3].pos.y = YBuf[ 1 ] ;
				DrawVertB[4].pos.x = XBuf[ 0 ] ;
				DrawVertB[4].pos.y = YBuf[ 1 ] ;
				DrawVertB[5].pos.x = XBuf[ 1 ] ;
				DrawVertB[5].pos.y = YBuf[ 0 ] ;

				switch( GSYS.DrawSetting.BlendGraphPosMode )
				{
				default :
				case DX_BLENDGRAPH_POSMODE_DRAWGRAPH:
					u[ 0 ] = TexVert[0].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ;
					u[ 1 ] = TexVert[1].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ;
					v[ 0 ] = TexVert[0].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ;
					v[ 1 ] = TexVert[2].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ;
					break ;

				case DX_BLENDGRAPH_POSMODE_SCREEN :
					u[ 0 ] = XBuf[ 0 ] - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ;
					u[ 1 ] = XBuf[ 1 ] - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ;
					v[ 0 ] = YBuf[ 0 ] - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ;
					v[ 1 ] = YBuf[ 1 ] - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ;
					break ;
				}

				u[ 0 ] *= GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
				u[ 1 ] *= GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
				v[ 0 ] *= GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
				v[ 1 ] *= GD3D9.Device.DrawSetting.InvBlendTextureHeight ;

				DrawVertB[0].u1 = u[ 0 ] ;
				DrawVertB[0].v1 = v[ 0 ] ;
				DrawVertB[1].u1 = u[ 1 ] ;
				DrawVertB[1].v1 = v[ 0 ] ;
				DrawVertB[2].u1 = u[ 0 ] ;
				DrawVertB[2].v1 = v[ 1 ] ;
				DrawVertB[3].u1 = u[ 1 ] ;
				DrawVertB[3].v1 = v[ 1 ] ;
				DrawVertB[4].u1 = u[ 0 ] ;
				DrawVertB[4].v1 = v[ 1 ] ;
				DrawVertB[5].u1 = u[ 1 ] ;
				DrawVertB[5].v1 = v[ 0 ] ;

				DrawVertB[0].u2 = TexVert[0].u ;
				DrawVertB[0].v2 = TexVert[0].v ;
				DrawVertB[1].u2 = TexVert[1].u ;
				DrawVertB[1].v2 = TexVert[0].v ;
				DrawVertB[2].u2 = TexVert[0].u ;
				DrawVertB[2].v2 = TexVert[2].v ;
				DrawVertB[3].u2 = TexVert[1].u ;
				DrawVertB[3].v2 = TexVert[2].v ;
				DrawVertB[4].u2 = TexVert[0].u ;
				DrawVertB[4].v2 = TexVert[2].v ;
				DrawVertB[5].u2 = TexVert[1].u ;
				DrawVertB[5].v2 = TexVert[0].v ;

				*((DWORD *)&DrawVertB[0].pos.z) = drawz ;
				*((DWORD *)&DrawVertB[1].pos.z) = drawz ;
				*((DWORD *)&DrawVertB[2].pos.z) = drawz ;
				*((DWORD *)&DrawVertB[3].pos.z) = drawz ;
				*((DWORD *)&DrawVertB[4].pos.z) = drawz ;
				*((DWORD *)&DrawVertB[5].pos.z) = drawz ;

				// テクスチャーを描画する
				ADD4VERTEX_BLENDTEX

				if( BlendGraphNoIncFlag == FALSE )
				{
					BlendDrawTex ++ ;
				}
			}
			break ;

		case VERTEXTYPE_TEX :	// ブレンドテクスチャを使用しない
			// テクスチャーの数だけ繰り返す
			i = 0 ;
			for(;;)
			{
				// 頂点データを初期化する
				GETVERTEX_QUAD( DrawVert )
				
				TexVert = DrawTex->Vertex ;
				
				DrawVert[0].color = ( DWORD )DiffuseColor ;
				DrawVert[3].color = ( DWORD )DiffuseColor ;

				XBuf[ 0 ] = TexVert[0].x * GraphExRateX + x1f ;
				XBuf[ 1 ] = TexVert[1].x * GraphExRateX + x1f ;
				YBuf[ 0 ] = TexVert[0].y * GraphExRateY + y1f ;
				YBuf[ 1 ] = TexVert[2].y * GraphExRateY + y1f ;
				DrawVert[0].pos.x = XBuf[ 0 ] ;
				DrawVert[0].pos.y = YBuf[ 0 ] ;
				DrawVert[1].pos.x = XBuf[ 1 ] ;
				DrawVert[1].pos.y = YBuf[ 0 ] ;
				DrawVert[2].pos.x = XBuf[ 0 ] ;
				DrawVert[2].pos.y = YBuf[ 1 ] ;
				DrawVert[3].pos.x = XBuf[ 1 ] ;
				DrawVert[3].pos.y = YBuf[ 1 ] ;
				DrawVert[4].pos.x = XBuf[ 0 ] ;
				DrawVert[4].pos.y = YBuf[ 1 ] ;
				DrawVert[5].pos.x = XBuf[ 1 ] ;
				DrawVert[5].pos.y = YBuf[ 0 ] ;

				DrawVert[0].u = TexVert[0].u ;
				DrawVert[0].v = TexVert[0].v ;
				DrawVert[1].u = TexVert[1].u ;
				DrawVert[1].v = TexVert[0].v ;
				DrawVert[2].u = TexVert[0].u ;
				DrawVert[2].v = TexVert[2].v ;
				DrawVert[3].u = TexVert[1].u ;
				DrawVert[3].v = TexVert[2].v ;
				DrawVert[4].u = TexVert[0].u ;
				DrawVert[4].v = TexVert[2].v ;
				DrawVert[5].u = TexVert[1].u ;
				DrawVert[5].v = TexVert[0].v ;

				*((DWORD *)&DrawVert[0].pos.z) = drawz ;
				*((DWORD *)&DrawVert[1].pos.z) = drawz ;
				*((DWORD *)&DrawVert[2].pos.z) = drawz ;
				*((DWORD *)&DrawVert[3].pos.z) = drawz ;
				*((DWORD *)&DrawVert[4].pos.z) = drawz ;
				*((DWORD *)&DrawVert[5].pos.z) = drawz ;

				// テクスチャーを描画する
				ADD4VERTEX_TEX

				i ++ ;
				if( i >= DrawTexNum ) break ;

				// 描画するテクスチャーのセット
				DrawTex ++ ;
				Graphics_D3D9_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D9.Texture ) ;
				Graphics_D3D9_DeviceState_RefreshBlendState() ;
			}
			break ;
		}
	}
	else
	{
		// 完全にはみ出ていたら何もせず終了
		if( OUT_DRAWAREA_CHECK( x1, y1, x2, y2 ) ) return 0 ;

		// 描画矩形の保存
		left	= GSYS.DrawSetting.DrawAreaF.left   - 0.5f /* + GSYS.WindowDrawRect.left */ ;
		top		= GSYS.DrawSetting.DrawAreaF.top    - 0.5f /* + GSYS.WindowDrawRect.top  */ ;
		right	= GSYS.DrawSetting.DrawAreaF.right  - 0.5f /* + GSYS.WindowDrawRect.left */ ;
		bottom	= GSYS.DrawSetting.DrawAreaF.bottom - 0.5f /* + GSYS.WindowDrawRect.top  */ ;

		// 頂点タイプによって処理を分岐
		switch( GD3D9.Device.DrawInfo.VertexType )
		{
		case VERTEXTYPE_BLENDTEX :	// ブレンドテクスチャを使用する
			// テクスチャーの数だけ繰り返す
			for( i = 0 ; i < DrawTexNum ; i ++, DrawTex ++ )
			{
				VECTOR DrawPos[ 4 ] ;
				float u1[ 4 ] ;
				float v1[ 4 ] ;
				float u2[ 4 ] ;
				float v2[ 4 ] ;

				// 描画するテクスチャーのセット
				Graphics_D3D9_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D9.Texture ) ;
				Graphics_D3D9_DrawSetting_SetBlendTexture( BlendDrawTex->Tex->PF->D3D9.Texture, BlendDrawTex->Tex->TexWidth, BlendDrawTex->Tex->TexHeight );

				if( GD3D9.Device.DrawSetting.ChangeTextureFlag )
					Graphics_D3D9_DeviceState_RefreshBlendState() ;

				GETVERTEX_QUAD( DrawVertB )
				
				Vx = 1.0F / ( DrawTex->Tex->TexWidth * GraphExRateX ) ;
				Vy = 1.0F / ( DrawTex->Tex->TexHeight * GraphExRateY ) ;
				VBx = GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
				VBy = GD3D9.Device.DrawSetting.InvBlendTextureHeight ;

				// 頂点データを初期化する
				TexVert = DrawTex->Vertex ;
				BlendTexVert = BlendDrawTex->Vertex ;

				DrawVertB[0].color = ( DWORD )DiffuseColor ;
				DrawVertB[3].color = ( DWORD )DiffuseColor ;

				XBuf[ 0 ] = TexVert[0].x * GraphExRateX + x1f ;
				XBuf[ 1 ] = TexVert[1].x * GraphExRateX + x1f ;
				YBuf[ 0 ] = TexVert[0].y * GraphExRateY + y1f ;
				YBuf[ 1 ] = TexVert[2].y * GraphExRateY + y1f ;
				DrawPos[0].x = XBuf[ 0 ] ;
				DrawPos[0].y = YBuf[ 0 ] ;
				DrawPos[1].x = XBuf[ 1 ] ;
				DrawPos[1].y = YBuf[ 0 ] ;
				DrawPos[2].x = XBuf[ 0 ] ;
				DrawPos[2].y = YBuf[ 1 ] ;
				DrawPos[3].x = XBuf[ 1 ] ;
				DrawPos[3].y = YBuf[ 1 ] ;

				switch( GSYS.DrawSetting.BlendGraphPosMode )
				{
				default :
				case DX_BLENDGRAPH_POSMODE_DRAWGRAPH:
					u1[2] = u1[0] = ( TexVert[0].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					u1[3] = u1[1] = ( TexVert[1].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					v1[1] = v1[0] = ( TexVert[0].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					v1[3] = v1[2] = ( TexVert[2].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					break ;

				case DX_BLENDGRAPH_POSMODE_SCREEN :
					u1[2] = u1[0] = ( XBuf[ 0 ] - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					u1[3] = u1[1] = ( XBuf[ 1 ] - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					v1[1] = v1[0] = ( YBuf[ 0 ] - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					v1[3] = v1[2] = ( YBuf[ 1 ] - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					break ;
				}

				u2[2] = u2[0] = TexVert[0].u ;
				u2[3] = u2[1] = TexVert[1].u ;
				v2[1] = v2[0] = TexVert[0].v ;
				v2[3] = v2[2] = TexVert[2].v ;

				for( j = 0 ; j < 4 ; j ++ )
				{
					// 描画矩形から出ていた場合の補正処理
					if( DrawPos[ j ].x < left )
					{
						u2[ j ] += ( left - DrawPos[ j ].x ) * Vx ;
						u1[ j ] += ( left - DrawPos[ j ].x ) * VBx ;
						DrawPos[ j ].x = left ;
					}
					else
					if( DrawPos[ j ].x > right )
					{
						u2[ j ] -= ( DrawPos[ j ].x - right ) * Vx ;
						u1[ j ] -= ( DrawPos[ j ].x - right ) * VBx ;
						DrawPos[ j ].x = right ;
					}	

					if( DrawPos[ j ].y < top )
					{
						v2[ j ] += ( top - DrawPos[ j ].y ) * Vy ;
						v1[ j ] += ( top - DrawPos[ j ].y ) * VBy ;
						DrawPos[ j ].y = top ;
					}
					else
					if( DrawPos[ j ].y > bottom )
					{
						v2[ j ] -= ( DrawPos[ j ].y - bottom ) * Vy ;
						v1[ j ] -= ( DrawPos[ j ].y - bottom ) * VBy ;
						DrawPos[ j ].y = bottom ;
					}
				}

				DrawVertB[0].pos.x = DrawPos[0].x ;
				DrawVertB[0].pos.y = DrawPos[0].y ;
				DrawVertB[1].pos.x = DrawPos[1].x ;
				DrawVertB[1].pos.y = DrawPos[1].y ;
				DrawVertB[2].pos.x = DrawPos[2].x ;
				DrawVertB[2].pos.y = DrawPos[2].y ;
				DrawVertB[3].pos.x = DrawPos[3].x ;
				DrawVertB[3].pos.y = DrawPos[3].y ;
				DrawVertB[4].pos.x = DrawPos[2].x ;
				DrawVertB[4].pos.y = DrawPos[2].y ;
				DrawVertB[5].pos.x = DrawPos[1].x ;
				DrawVertB[5].pos.y = DrawPos[1].y ;

				DrawVertB[0].u1 = u1[0] ;
				DrawVertB[0].v1 = v1[0] ;
				DrawVertB[1].u1 = u1[1] ;
				DrawVertB[1].v1 = v1[1] ;
				DrawVertB[2].u1 = u1[2] ;
				DrawVertB[2].v1 = v1[2] ;
				DrawVertB[3].u1 = u1[3] ;
				DrawVertB[3].v1 = v1[3] ;
				DrawVertB[4].u1 = u1[2] ;
				DrawVertB[4].v1 = v1[2] ;
				DrawVertB[5].u1 = u1[1] ;
				DrawVertB[5].v1 = v1[1] ;

				DrawVertB[0].u2 = u2[0] ;
				DrawVertB[0].v2 = v2[0] ;
				DrawVertB[1].u2 = u2[1] ;
				DrawVertB[1].v2 = v2[1] ;
				DrawVertB[2].u2 = u2[2] ;
				DrawVertB[2].v2 = v2[2] ;
				DrawVertB[3].u2 = u2[3] ;
				DrawVertB[3].v2 = v2[3] ;
				DrawVertB[4].u2 = u2[2] ;
				DrawVertB[4].v2 = v2[2] ;
				DrawVertB[5].u2 = u2[1] ;
				DrawVertB[5].v2 = v2[1] ;

				*((DWORD *)&DrawVertB[0].pos.z) = drawz ;
				*((DWORD *)&DrawVertB[1].pos.z) = drawz ;
				*((DWORD *)&DrawVertB[2].pos.z) = drawz ;
				*((DWORD *)&DrawVertB[3].pos.z) = drawz ;
				*((DWORD *)&DrawVertB[4].pos.z) = drawz ;
				*((DWORD *)&DrawVertB[5].pos.z) = drawz ;

				// テクスチャーを描画する
				ADD4VERTEX_BLENDTEX

				if( BlendGraphNoIncFlag == FALSE )
				{
					BlendDrawTex ++ ;
				}
			}
			break ;

		case VERTEXTYPE_TEX :	// ブレンドテクスチャを使用しない
			// テクスチャーの数だけ繰り返す
			i = 0 ;
			for(;;)
			{
				VECTOR DrawPos[ 4 ] ;
				float u4[ 4 ] ;
				float v4[ 4 ] ;

				// 頂点データを初期化する
				GETVERTEX_QUAD( DrawVert )
				
				TexVert = DrawTex->Vertex ;

				DrawVert[0].color = ( DWORD )DiffuseColor ;
				DrawVert[3].color = ( DWORD )DiffuseColor ;

				XBuf[ 0 ] = TexVert[0].x * GraphExRateX + x1f ;
				XBuf[ 1 ] = TexVert[1].x * GraphExRateX + x1f ;
				YBuf[ 0 ] = TexVert[0].y * GraphExRateY + y1f ;
				YBuf[ 1 ] = TexVert[2].y * GraphExRateY + y1f ;
				DrawPos[0].x = XBuf[ 0 ] ;
				DrawPos[0].y = YBuf[ 0 ] ;
				DrawPos[1].x = XBuf[ 1 ] ;
				DrawPos[1].y = YBuf[ 0 ] ;
				DrawPos[2].x = XBuf[ 0 ] ;
				DrawPos[2].y = YBuf[ 1 ] ;
				DrawPos[3].x = XBuf[ 1 ] ;
				DrawPos[3].y = YBuf[ 1 ] ;

				u4[2] = u4[0] = TexVert[0].u ;
				u4[3] = u4[1] = TexVert[1].u ;
				v4[1] = v4[0] = TexVert[0].v ;
				v4[3] = v4[2] = TexVert[2].v ;

				Vx = 1.0F / ( DrawTex->Tex->TexWidth * GraphExRateX ) ;
				Vy = 1.0F / ( DrawTex->Tex->TexHeight * GraphExRateY ) ;
				for( j = 0 ; j < 4 ; j ++ )
				{
					// 描画矩形から出ていた場合の補正処理
					if( DrawPos[ j ].x < left )
					{
						u4[ j ] += ( left - DrawPos[ j ].x ) * Vx ;
						DrawPos[ j ].x = left ;
					}
					else
					if( DrawPos[ j ].x > right )
					{
						u4[ j ] -= ( DrawPos[ j ].x - right ) * Vx ;
						DrawPos[ j ].x = right ;
					}	

					if( DrawPos[ j ].y < top )
					{
						v4[ j ] += ( top - DrawPos[ j ].y ) * Vy ;
						DrawPos[ j ].y = top ;
					}
					else
					if( DrawPos[ j ].y > bottom )
					{
						v4[ j ] -= ( DrawPos[ j ].y - bottom ) * Vy ;
						DrawPos[ j ].y = bottom ;
					}
				}

				DrawVert[0].pos.x = DrawPos[0].x ;
				DrawVert[0].pos.y = DrawPos[0].y ;
				DrawVert[1].pos.x = DrawPos[1].x ;
				DrawVert[1].pos.y = DrawPos[1].y ;
				DrawVert[2].pos.x = DrawPos[2].x ;
				DrawVert[2].pos.y = DrawPos[2].y ;
				DrawVert[3].pos.x = DrawPos[3].x ;
				DrawVert[3].pos.y = DrawPos[3].y ;
				DrawVert[4].pos.x = DrawPos[2].x ;
				DrawVert[4].pos.y = DrawPos[2].y ;
				DrawVert[5].pos.x = DrawPos[1].x ;
				DrawVert[5].pos.y = DrawPos[1].y ;

				DrawVert[0].u = u4[0] ;
				DrawVert[0].v = v4[0] ;
				DrawVert[1].u = u4[1] ;
				DrawVert[1].v = v4[1] ;
				DrawVert[2].u = u4[2] ;
				DrawVert[2].v = v4[2] ;
				DrawVert[3].u = u4[3] ;
				DrawVert[3].v = v4[3] ;
				DrawVert[4].u = u4[2] ;
				DrawVert[4].v = v4[2] ;
				DrawVert[5].u = u4[1] ;
				DrawVert[5].v = v4[1] ;

				*((DWORD *)&DrawVert[0].pos.z) = drawz ;
				*((DWORD *)&DrawVert[1].pos.z) = drawz ;
				*((DWORD *)&DrawVert[2].pos.z) = drawz ;
				*((DWORD *)&DrawVert[3].pos.z) = drawz ;
				*((DWORD *)&DrawVert[4].pos.z) = drawz ;
				*((DWORD *)&DrawVert[5].pos.z) = drawz ;

				// テクスチャーを描画する
				ADD4VERTEX_TEX

				i ++ ;
				if( i >= DrawTexNum ) break ;

				// 描画するテクスチャーのセット
				DrawTex ++ ;
				Graphics_D3D9_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D9.Texture ) ;
				Graphics_D3D9_DeviceState_RefreshBlendState() ;
			}
			break ;
		}
	}

	// 終了
	return 0 ;
}

// ハードウエアアクセラレータ使用版 DrawRotaGraph
extern	int		Graphics_D3D9_DrawRotaGraph( int x, int y, float xf, float yf, double ExRate, double Angle, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, int ReverseXFlag, int ReverseYFlag, int IntFlag )
{
	VERTEX_2D *DrawVert ;
	VERTEX_BLENDTEX_2D *DrawVertB ;
	VECTOR DrawPos[ 4 ] ;
	DWORD DiffuseColor ;
	IMAGEDATA_HARD_DRAW *DrawTex ;
	IMAGEDATA_HARD_DRAW *BlendDrawTex = NULL ;
	IMAGEDATA_HARD_VERT *TexVert ;
	IMAGEDATA_HARD_VERT *BlendTexVert ;
	int DrawTexNum ;
	int i ; 
	float Sin = 0.0f, Cos = 1.0f ;
	int BlendGraphNoIncFlag ;
	float XBuf[ 8 ], YBuf[ 8 ] ;
	float u[ 4 ], v[ 4 ] ;
	float ExtendRate = ( float )ExRate ; 
	float ExtendRateX ;
	float ExtendRateY ;
	DWORD drawz ;
//	BOOL Adj = FALSE ;
	float CenX, CenY ;
	float fx, fy ;
	int hx = 0, hy = 0 ;
	int FastCode, SinCosNone, AngleCheck, AlwaysCheck ;
	int Flag ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	// 幅がなかった場合はエラー
	if( ExRate == 0.0 ) return -1 ;

	// 描画の準備
	if( GD3D9.Device.DrawInfo.BeginSceneFlag == FALSE ) Graphics_D3D9_BeginScene() ;
	Flag = TransFlag ;
	DX_D3D9_DRAWPREP_TEX( Image->Orig, Image->Hard.Draw[ 0 ].Tex->PF->D3D9.Texture, Flag )

	// 回転要素が無いか調べる
	if( Angle == 0.0 )
	{
		SinCosNone = 1;
	}
	else
	{
		SinCosNone = 0;
		_SINCOS( (float)Angle, &Sin, &Cos ) ;
	}
//	if( _FABS( (float)Angle ) < 0.0001 && ExRate < 1.001 && ExRate > 0.999 ) Adj = TRUE ;

	// 座標の補正
	if( IntFlag || GSYS.DrawSetting.DrawMode == DX_DRAWMODE_NEAREST )
	{
		AlwaysCheck = 0 ;
		FastCode    = 0 ;
		hx          = Image->WidthI % 2 ;
		hy          = Image->HeightI % 2 ;
		fx = xf - ( float )x ;
		fy = yf - ( float )y ;
		if( fx < 0.0f ) fx = -fx;
		if( fy < 0.0f ) fy = -fy;
		if( fx < 0.00001f && fy < 0.00001f )
		{
			if( ExRate == 1.0 )
			{
				if( hx == hy )
				{
					if( hx == 0 )
					{
						CenX = ( float )(Image->WidthI >> 1) ;
						CenY = ( float )(Image->HeightI >> 1) ;
						xf   = ( float )x - 0.5F ;
						yf   = ( float )y - 0.5F ;
					}
					else
					{
						CenX = ( float )Image->WidthI * 0.5F ;
						CenY = ( float )Image->HeightI * 0.5F ;
						xf   = ( float )x ;
						yf   = ( float )y ;
						FastCode = 1;
					}
				}
				else
				{
					CenX = ( float )Image->WidthI * 0.5F ;
					CenY = ( float )Image->HeightI * 0.5F ;
					xf   = ( float )x ;
					yf   = ( float )y ;
						
					if( hx == 0 ) CenX -= 0.5F ;
					if( hy == 0 ) CenY -= 0.5F ;
				}
			}
			else
			{
				CenX = ( float )Image->WidthI * 0.5F ;
				CenY = ( float )Image->HeightI * 0.5F ;
				xf   = ( float )x - 0.5F ;
				yf   = ( float )y - 0.5F ;
				FastCode = 1 ;
			}
		}
		else
		{
			CenX = ( float )Image->WidthI * 0.5F ;
			CenY = ( float )Image->HeightI * 0.5F ;
			xf   = ( float )x ;
			yf   = ( float )y ;
			AlwaysCheck = 1 ;
		}
	}
	else
	{
		CenX = ( float )Image->WidthF * 0.5F ;
		CenY = ( float )Image->HeightF * 0.5F ;
		xf -= 0.5F ;
		yf -= 0.5F ;
		AngleCheck = 0 ;
		AlwaysCheck = 0 ;
		FastCode = 1 ;
	}

	// 微妙な回転値のチェックを行うかどうかを調べる
	if( IntFlag == TRUE && AlwaysCheck == 0 && SinCosNone == 0 && ( hx == 1 || hy == 1 ) &&
		( ( Angle < DX_PI / 180.0 *         - 4.0   ) ||
		  ( Angle > DX_PI / 180.0 * ( 360.0 + 4.0 ) ) ||
		  ( Angle > DX_PI / 180.0 *         - 4.0   && Angle < DX_PI / 180.0 *           4.0   ) ||
		  ( Angle > DX_PI / 180.0 * (  90.0 - 4.0 ) && Angle < DX_PI / 180.0 * (  90.0 + 4.0 ) ) ||
		  ( Angle > DX_PI / 180.0 * ( 180.0 - 4.0 ) && Angle < DX_PI / 180.0 * ( 180.0 + 4.0 ) ) ||
		  ( Angle > DX_PI / 180.0 * ( 270.0 - 4.0 ) && Angle < DX_PI / 180.0 * ( 270.0 + 4.0 ) ) ||
		  ( Angle > DX_PI / 180.0 * ( 360.0 - 4.0 ) && Angle < DX_PI / 180.0 * ( 360.0 + 4.0 ) ) ) )
	{
		AngleCheck = 1 ;
	}
	else
	{
		AngleCheck = 0 ;
	}

	// 頂点データを取得（今のところ色情報のセットのみ）
	DiffuseColor = GD3D9.Device.DrawInfo.DiffuseColor ;

	// 描画情報の数をセット
	DrawTexNum = Image->Hard.DrawNum ;

	// 描画情報の数がブレンド画像と異なっていたら０番目のテクスチャだけを使用する
	BlendGraphNoIncFlag = FALSE ;
	if( BlendImage != NULL && BlendImage->Hard.DrawNum != Image->Hard.DrawNum )
	{
		BlendGraphNoIncFlag = TRUE ;
	}

	// 描画情報配列のアドレスをセットしておく
	DrawTex = Image->Hard.Draw ;
	if( BlendImage != NULL )
	{
		BlendDrawTex = BlendImage->Hard.Draw ;
	}

	// Ｚバッファに書き込むＺ値をセットする
	drawz = *((DWORD *)&GSYS.DrawSetting.DrawZ);

	// 反転を考慮した拡大率をセット
	ExtendRateX = ( ( ReverseXFlag == TRUE ) ? -1.0f : 1.0f ) * ExtendRate ;
	ExtendRateY = ( ( ReverseYFlag == TRUE ) ? -1.0f : 1.0f ) * ExtendRate ;

	// 頂点タイプによって処理を分岐
	switch( GD3D9.Device.DrawInfo.VertexType )
	{
	case VERTEXTYPE_BLENDTEX :	// ブレンドテクスチャを使用する
		// テクスチャーの数だけ繰り返す
		if( DrawTexNum == 1 && FastCode == 1 )
		{
			// 描画するテクスチャーのセット
			Graphics_D3D9_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D9.Texture ) ;
			Graphics_D3D9_DrawSetting_SetBlendTexture( BlendDrawTex->Tex->PF->D3D9.Texture, BlendDrawTex->Tex->TexWidth, BlendDrawTex->Tex->TexHeight );

			if( GD3D9.Device.DrawSetting.ChangeTextureFlag )
				Graphics_D3D9_DeviceState_RefreshBlendState() ;

			// 頂点データを初期化する
			GETVERTEX_QUAD( DrawVertB )

			TexVert = DrawTex->Vertex ;
			BlendTexVert = BlendDrawTex->Vertex ;

			DrawVertB[0].color = ( DWORD )DiffuseColor ;
			DrawVertB[3].color = ( DWORD )DiffuseColor ;

			XBuf[0] = ( TexVert[0].x - CenX ) * ExtendRateX ;
			YBuf[0] = ( TexVert[0].y - CenY ) * ExtendRateY ;
			XBuf[1] = XBuf[0] * Cos - YBuf[0] * Sin ;
			YBuf[1] = XBuf[0] * Sin + YBuf[0] * Cos ;

			XBuf[2] = ( TexVert[1].x - CenX ) * ExtendRateX ;
			YBuf[2] = ( TexVert[1].y - CenY ) * ExtendRateY ;
			XBuf[3] = XBuf[2] * Cos - YBuf[2] * Sin ;
			YBuf[3] = XBuf[2] * Sin + YBuf[2] * Cos ;

			DrawVertB[0].pos.x =  XBuf[1] + xf ;
			DrawVertB[0].pos.y =  YBuf[1] + yf ;
			DrawVertB[1].pos.x =  XBuf[3] + xf ;
			DrawVertB[1].pos.y =  YBuf[3] + yf ;
			DrawVertB[2].pos.x = -XBuf[3] + xf ;
			DrawVertB[2].pos.y = -YBuf[3] + yf ;
			DrawVertB[3].pos.x = -XBuf[1] + xf ;
			DrawVertB[3].pos.y = -YBuf[1] + yf ;
			DrawVertB[4].pos.x = -XBuf[3] + xf ;
			DrawVertB[4].pos.y = -YBuf[3] + yf ;
			DrawVertB[5].pos.x =  XBuf[3] + xf ;
			DrawVertB[5].pos.y =  YBuf[3] + yf ;

			switch( GSYS.DrawSetting.BlendGraphPosMode )
			{
			default :
			case DX_BLENDGRAPH_POSMODE_DRAWGRAPH:
				u[ 0 ] = TexVert[0].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
				u[ 1 ] = TexVert[1].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
				v[ 0 ] = TexVert[0].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
				v[ 1 ] = TexVert[2].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
				DrawVertB[0].u1 = u[ 0 ] ;
				DrawVertB[0].v1 = v[ 0 ] ;
				DrawVertB[1].u1 = u[ 1 ] ;
				DrawVertB[1].v1 = v[ 0 ] ;
				DrawVertB[2].u1 = u[ 0 ] ;
				DrawVertB[2].v1 = v[ 1 ] ;
				DrawVertB[3].u1 = u[ 1 ] ;
				DrawVertB[3].v1 = v[ 1 ] ;
				DrawVertB[4].u1 = u[ 0 ] ;
				DrawVertB[4].v1 = v[ 1 ] ;
				DrawVertB[5].u1 = u[ 1 ] ;
				DrawVertB[5].v1 = v[ 0 ] ;
				break ;

			case DX_BLENDGRAPH_POSMODE_SCREEN :
				DrawVertB[0].u1 = ( (  XBuf[1] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
				DrawVertB[0].v1 = ( (  YBuf[1] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
				DrawVertB[1].u1 = ( (  XBuf[3] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
				DrawVertB[1].v1 = ( (  YBuf[3] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
				DrawVertB[2].u1 = ( ( -XBuf[3] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
				DrawVertB[2].v1 = ( ( -YBuf[3] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
				DrawVertB[3].u1 = ( ( -XBuf[1] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
				DrawVertB[3].v1 = ( ( -YBuf[1] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
				DrawVertB[4].u1 = ( ( -XBuf[3] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
				DrawVertB[4].v1 = ( ( -YBuf[3] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
				DrawVertB[5].u1 = ( (  XBuf[3] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
				DrawVertB[5].v1 = ( (  YBuf[3] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
				break ;
			}

			DrawVertB[0].u2 = TexVert[0].u ;
			DrawVertB[0].v2 = TexVert[0].v ;
			DrawVertB[1].u2 = TexVert[1].u ;
			DrawVertB[1].v2 = TexVert[0].v ;
			DrawVertB[2].u2 = TexVert[0].u ;
			DrawVertB[2].v2 = TexVert[2].v ;
			DrawVertB[3].u2 = TexVert[1].u ;
			DrawVertB[3].v2 = TexVert[2].v ;
			DrawVertB[4].u2 = TexVert[0].u ;
			DrawVertB[4].v2 = TexVert[2].v ;
			DrawVertB[5].u2 = TexVert[1].u ;
			DrawVertB[5].v2 = TexVert[0].v ;

			*((DWORD *)&DrawVertB[0].pos.z) = drawz ;
			*((DWORD *)&DrawVertB[1].pos.z) = drawz ;
			*((DWORD *)&DrawVertB[2].pos.z) = drawz ;
			*((DWORD *)&DrawVertB[3].pos.z) = drawz ;
			*((DWORD *)&DrawVertB[4].pos.z) = drawz ;
			*((DWORD *)&DrawVertB[5].pos.z) = drawz ;

			// テクスチャーを描画する
			ADD4VERTEX_BLENDTEX

			if( BlendGraphNoIncFlag == FALSE )
			{
				BlendDrawTex ++ ;
			}
		}
		else
		{
			// テクスチャーの数だけ繰り返す
			for( i = 0 ; i < DrawTexNum ; i ++, DrawTex ++ )
			{
				// 描画するテクスチャーのセット
				Graphics_D3D9_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D9.Texture ) ;
				Graphics_D3D9_DrawSetting_SetBlendTexture( BlendDrawTex->Tex->PF->D3D9.Texture, BlendDrawTex->Tex->TexWidth, BlendDrawTex->Tex->TexHeight );

				if( GD3D9.Device.DrawSetting.ChangeTextureFlag )
					Graphics_D3D9_DeviceState_RefreshBlendState() ;

				// 頂点データを初期化する
				GETVERTEX_QUAD( DrawVertB )

				TexVert = DrawTex->Vertex ;
				BlendTexVert = BlendDrawTex->Vertex ;

				DrawVertB[0].color = ( DWORD )DiffuseColor ;
				DrawVertB[3].color = ( DWORD )DiffuseColor ;

				XBuf[0] = ( TexVert[0].x - CenX ) * ExtendRateX ;
				YBuf[0] = ( TexVert[0].y - CenY ) * ExtendRateY ;
				XBuf[1] = XBuf[0] * Cos - YBuf[0] * Sin ;
				YBuf[1] = XBuf[0] * Sin + YBuf[0] * Cos ;

				XBuf[2] = ( TexVert[1].x - CenX ) * ExtendRateX ;
				YBuf[2] = ( TexVert[1].y - CenY ) * ExtendRateY ;
				XBuf[3] = XBuf[2] * Cos - YBuf[2] * Sin ;
				YBuf[3] = XBuf[2] * Sin + YBuf[2] * Cos ;

				XBuf[4] = ( TexVert[2].x - CenX ) * ExtendRateX ;
				YBuf[4] = ( TexVert[2].y - CenY ) * ExtendRateY ;
				XBuf[5] = XBuf[4] * Cos - YBuf[4] * Sin ;
				YBuf[5] = XBuf[4] * Sin + YBuf[4] * Cos ;

				XBuf[6] = ( TexVert[3].x - CenX ) * ExtendRateX ;
				YBuf[6] = ( TexVert[3].y - CenY ) * ExtendRateY ;
				XBuf[7] = XBuf[6] * Cos - YBuf[6] * Sin ;
				YBuf[7] = XBuf[6] * Sin + YBuf[6] * Cos ;

				DrawVertB[0].pos.x = XBuf[1] + xf ;
				DrawVertB[0].pos.y = YBuf[1] + yf ;
				DrawVertB[1].pos.x = XBuf[3] + xf ;
				DrawVertB[1].pos.y = YBuf[3] + yf ;
				DrawVertB[2].pos.x = XBuf[5] + xf ;
				DrawVertB[2].pos.y = YBuf[5] + yf ;
				DrawVertB[3].pos.x = XBuf[7] + xf ;
				DrawVertB[3].pos.y = YBuf[7] + yf ;
				DrawVertB[4].pos.x = XBuf[5] + xf ;
				DrawVertB[4].pos.y = YBuf[5] + yf ;
				DrawVertB[5].pos.x = XBuf[3] + xf ;
				DrawVertB[5].pos.y = YBuf[3] + yf ;

				switch( GSYS.DrawSetting.BlendGraphPosMode )
				{
				default :
				case DX_BLENDGRAPH_POSMODE_DRAWGRAPH:
					u[ 0 ] = ( TexVert[0].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					u[ 1 ] = ( TexVert[1].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					u[ 2 ] = ( TexVert[2].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					u[ 3 ] = ( TexVert[3].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					
					v[ 0 ] = ( TexVert[0].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					v[ 1 ] = ( TexVert[1].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					v[ 2 ] = ( TexVert[2].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					v[ 3 ] = ( TexVert[3].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;

					DrawVertB[0].u1 = u[ 0 ] ;
					DrawVertB[0].v1 = v[ 0 ] ;
					DrawVertB[1].u1 = u[ 1 ] ;
					DrawVertB[1].v1 = v[ 1 ] ;
					DrawVertB[2].u1 = u[ 2 ] ;
					DrawVertB[2].v1 = v[ 2 ] ;
					DrawVertB[3].u1 = u[ 3 ] ;
					DrawVertB[3].v1 = v[ 3 ] ;
					DrawVertB[4].u1 = u[ 2 ] ;
					DrawVertB[4].v1 = v[ 2 ] ;
					DrawVertB[5].u1 = u[ 1 ] ;
					DrawVertB[5].v1 = v[ 1 ] ;
					break ;

				case DX_BLENDGRAPH_POSMODE_SCREEN :
					DrawVertB[0].u1 = ( ( XBuf[1] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[0].v1 = ( ( YBuf[1] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[1].u1 = ( ( XBuf[3] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[1].v1 = ( ( YBuf[3] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[2].u1 = ( ( XBuf[5] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[2].v1 = ( ( YBuf[5] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[3].u1 = ( ( XBuf[7] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[3].v1 = ( ( YBuf[7] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[4].u1 = ( ( XBuf[5] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[4].v1 = ( ( YBuf[5] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[5].u1 = ( ( XBuf[3] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[5].v1 = ( ( YBuf[3] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					break ;
				}

				DrawVertB[0].u2 = TexVert[0].u ;
				DrawVertB[0].v2 = TexVert[0].v ;
				DrawVertB[1].u2 = TexVert[1].u ;
				DrawVertB[1].v2 = TexVert[0].v ;
				DrawVertB[2].u2 = TexVert[0].u ;
				DrawVertB[2].v2 = TexVert[2].v ;
				DrawVertB[3].u2 = TexVert[1].u ;
				DrawVertB[3].v2 = TexVert[2].v ;
				DrawVertB[4].u2 = TexVert[0].u ;
				DrawVertB[4].v2 = TexVert[2].v ;
				DrawVertB[5].u2 = TexVert[1].u ;
				DrawVertB[5].v2 = TexVert[0].v ;

				*((DWORD *)&DrawVertB[0].pos.z) = drawz ;
				*((DWORD *)&DrawVertB[1].pos.z) = drawz ;
				*((DWORD *)&DrawVertB[2].pos.z) = drawz ;
				*((DWORD *)&DrawVertB[3].pos.z) = drawz ;
				*((DWORD *)&DrawVertB[4].pos.z) = drawz ;
				*((DWORD *)&DrawVertB[5].pos.z) = drawz ;

				// テクスチャーを描画する
				ADD4VERTEX_BLENDTEX

				if( BlendGraphNoIncFlag == FALSE )
				{
					BlendDrawTex ++ ;
				}
			}
		}
		break ;

	case VERTEXTYPE_TEX :	// ブレンドテクスチャを使用しない
		// テクスチャーの数だけ繰り返す
		if( DrawTexNum == 1 && FastCode == 1 )
		{
			if( SinCosNone == 0 )
			{
				// 頂点データを初期化する
				GETVERTEX_QUAD( DrawVert )
				
				TexVert = DrawTex->Vertex ;

				DrawVert[0].color = ( DWORD )DiffuseColor ;
				DrawVert[3].color = ( DWORD )DiffuseColor ;

				DrawVert[0].u = TexVert[0].u ;
				DrawVert[0].v = TexVert[0].v ;
				DrawVert[1].u = TexVert[1].u ;
				DrawVert[1].v = TexVert[0].v ;
				DrawVert[2].v = TexVert[2].v ;
				DrawVert[2].u = TexVert[0].u ;
				DrawVert[3].u = TexVert[1].u ;
				DrawVert[3].v = TexVert[2].v ;
				DrawVert[4].u = TexVert[0].u ;
				DrawVert[4].v = TexVert[2].v ;
				DrawVert[5].u = TexVert[1].u ;
				DrawVert[5].v = TexVert[0].v ;

				XBuf[0] = ( TexVert[0].x - CenX ) * ExtendRateX ;
				YBuf[0] = ( TexVert[0].y - CenY ) * ExtendRateY ;
				XBuf[1] = XBuf[0] * Cos - YBuf[0] * Sin ;
				YBuf[1] = XBuf[0] * Sin + YBuf[0] * Cos ;
				
				XBuf[2] = ( TexVert[1].x - CenX ) * ExtendRateX ;
				YBuf[2] = ( TexVert[1].y - CenY ) * ExtendRateY ;
				XBuf[3] = XBuf[2] * Cos - YBuf[2] * Sin ;
				YBuf[3] = XBuf[2] * Sin + YBuf[2] * Cos ;

				if( AlwaysCheck )
				{
					DrawPos[0].x =  XBuf[1] + xf ;
					DrawPos[0].y =  YBuf[1] + yf ;
					DrawPos[1].x =  XBuf[3] + xf ;
					DrawPos[1].y =  YBuf[3] + yf ;
					DrawPos[2].x = -XBuf[3] + xf ;
					DrawPos[2].y = -YBuf[3] + yf ;
					DrawPos[3].x = -XBuf[1] + xf ;
					DrawPos[3].y = -YBuf[1] + yf ;

					if( ( ( ( *((DWORD *)&DrawPos[0].x) == *((DWORD *)&DrawPos[2].x) ) ||
							( ( DrawPos[0].x > DrawPos[2].x ) && ( DrawPos[0].x < DrawPos[2].x + 0.01f ) ) ||
							( ( DrawPos[2].x > DrawPos[0].x ) && ( DrawPos[2].x < DrawPos[0].x + 0.01f ) ) ) &&
							_FABS( DrawPos[0].x - _FTOL( DrawPos[0].x ) ) < 0.0001f ) ||
						( ( ( *((DWORD *)&DrawPos[0].x) == *((DWORD *)&DrawPos[1].x) ) ||
							( ( DrawPos[0].x > DrawPos[1].x ) && ( DrawPos[0].x < DrawPos[1].x + 0.01f ) ) ||
							( ( DrawPos[1].x > DrawPos[0].x ) && ( DrawPos[1].x < DrawPos[0].x + 0.01f ) ) ) &&
							_FABS( DrawPos[0].x - _FTOL( DrawPos[0].x ) ) < 0.0001f ) )
					{
						DrawPos[0].x -= 0.5f;
						DrawPos[1].x -= 0.5f;
						DrawPos[2].x -= 0.5f;
						DrawPos[3].x -= 0.5f;
					}

					if( ( ( ( *((DWORD *)&DrawPos[0].y) == *((DWORD *)&DrawPos[2].y) ) ||
							( ( DrawPos[0].y > DrawPos[2].y ) && ( DrawPos[0].y < DrawPos[2].y + 0.01f ) ) ||
							( ( DrawPos[2].y > DrawPos[0].y ) && ( DrawPos[2].y < DrawPos[0].y + 0.01f ) ) ) &&
							_FABS( DrawPos[0].y - _FTOL( DrawPos[0].y ) ) < 0.0001f ) ||
						( ( ( *((DWORD *)&DrawPos[0].y) == *((DWORD *)&DrawPos[1].y) ) ||
							( ( DrawPos[0].y > DrawPos[1].y ) && ( DrawPos[0].y < DrawPos[1].y + 0.01f ) ) ||
							( ( DrawPos[1].y > DrawPos[0].y ) && ( DrawPos[1].y < DrawPos[0].y + 0.01f ) ) ) &&
							_FABS( DrawPos[0].y - _FTOL( DrawPos[0].y ) ) < 0.0001f ) )
					{
						DrawPos[0].y -= 0.5f;
						DrawPos[1].y -= 0.5f;
						DrawPos[2].y -= 0.5f;
						DrawPos[3].y -= 0.5f;
					}

					DrawVert[0].pos.x = DrawPos[0].x ;
					DrawVert[0].pos.y = DrawPos[0].y ;
					DrawVert[1].pos.x = DrawPos[1].x ;
					DrawVert[1].pos.y = DrawPos[1].y ;
					DrawVert[2].pos.x = DrawPos[2].x ;
					DrawVert[2].pos.y = DrawPos[2].y ;
					DrawVert[3].pos.x = DrawPos[3].x ;
					DrawVert[3].pos.y = DrawPos[3].y ;
					DrawVert[4].pos.x = DrawPos[2].x ;
					DrawVert[4].pos.y = DrawPos[2].y ;
					DrawVert[5].pos.x = DrawPos[1].x ;
					DrawVert[5].pos.y = DrawPos[1].y ;
				}
				else
				if( AngleCheck )
				{
					DrawPos[0].x =  XBuf[1] + xf ;
					DrawPos[0].y =  YBuf[1] + yf ;
					DrawPos[3].x = -XBuf[1] + xf ;
					DrawPos[3].y = -YBuf[1] + yf ;

					DrawPos[1].x =  XBuf[3] + xf ;
					DrawPos[1].y =  YBuf[3] + yf ;
					DrawPos[2].x = -XBuf[3] + xf ;
					DrawPos[2].y = -YBuf[3] + yf ;

					if( hx &&
						( ( *((DWORD *)&DrawPos[0].x) == *((DWORD *)&DrawPos[2].x) ) ||
						  ( ( DrawPos[0].x > DrawPos[2].x - 0.01f ) && ( DrawPos[0].x < DrawPos[2].x + 0.01f ) ) ||
						  ( ( DrawPos[2].x > DrawPos[0].x - 0.01f ) && ( DrawPos[2].x < DrawPos[0].x + 0.01f ) ) ||
						  ( *((DWORD *)&DrawPos[0].x) == *((DWORD *)&DrawPos[1].x) ) ||
						  ( ( DrawPos[0].x > DrawPos[1].x - 0.01f ) && ( DrawPos[0].x < DrawPos[1].x + 0.01f ) ) ||
						  ( ( DrawPos[1].x > DrawPos[0].x - 0.01f ) && ( DrawPos[1].x < DrawPos[0].x + 0.01f ) ) ) )
					{
						DrawPos[0].x -= 0.5f;
						DrawPos[1].x -= 0.5f;
						DrawPos[2].x -= 0.5f;
						DrawPos[3].x -= 0.5f;
					}

					if( hy &&
						( ( *((DWORD *)&DrawPos[0].y) == *((DWORD *)&DrawPos[1].y) ) ||
						  ( ( DrawPos[0].y > DrawPos[1].y - 0.01f ) && ( DrawPos[0].y < DrawPos[1].y + 0.01f ) ) ||
						  ( ( DrawPos[1].y > DrawPos[0].y - 0.01f ) && ( DrawPos[1].y < DrawPos[0].y + 0.01f ) ) ||
						  ( *((DWORD *)&DrawPos[0].y) == *((DWORD *)&DrawPos[2].y) ) ||
						  ( ( DrawPos[0].y > DrawPos[2].y - 0.01f ) && ( DrawPos[0].y < DrawPos[2].y + 0.01f ) ) ||
						  ( ( DrawPos[2].y > DrawPos[0].y - 0.01f ) && ( DrawPos[2].y < DrawPos[0].y + 0.01f ) ) ) )
					{
						DrawPos[0].y -= 0.5f;
						DrawPos[1].y -= 0.5f;
						DrawPos[2].y -= 0.5f;
						DrawPos[3].y -= 0.5f;
					}

					DrawVert[0].pos.x = DrawPos[0].x ;
					DrawVert[0].pos.y = DrawPos[0].y ;
					DrawVert[1].pos.x = DrawPos[1].x ;
					DrawVert[1].pos.y = DrawPos[1].y ;
					DrawVert[2].pos.x = DrawPos[2].x ;
					DrawVert[2].pos.y = DrawPos[2].y ;
					DrawVert[3].pos.x = DrawPos[3].x ;
					DrawVert[3].pos.y = DrawPos[3].y ;
					DrawVert[4].pos.x = DrawPos[2].x ;
					DrawVert[4].pos.y = DrawPos[2].y ;
					DrawVert[5].pos.x = DrawPos[1].x ;
					DrawVert[5].pos.y = DrawPos[1].y ;
				}
				else
				{
					DrawVert[0].pos.x =  XBuf[1] + xf ;
					DrawVert[0].pos.y =  YBuf[1] + yf ;
					DrawVert[1].pos.x =  XBuf[3] + xf ;
					DrawVert[1].pos.y =  YBuf[3] + yf ;
					DrawVert[2].pos.x = -XBuf[3] + xf ;
					DrawVert[2].pos.y = -YBuf[3] + yf ;
					DrawVert[3].pos.x = -XBuf[1] + xf ;
					DrawVert[3].pos.y = -YBuf[1] + yf ;
					DrawVert[4].pos.x = -XBuf[3] + xf ;
					DrawVert[4].pos.y = -YBuf[3] + yf ;
					DrawVert[5].pos.x =  XBuf[3] + xf ;
					DrawVert[5].pos.y =  YBuf[3] + yf ;
				}

				*((DWORD *)&DrawVert[0].pos.z) = drawz ;
				*((DWORD *)&DrawVert[1].pos.z) = drawz ;
				*((DWORD *)&DrawVert[2].pos.z) = drawz ;
				*((DWORD *)&DrawVert[3].pos.z) = drawz ;
				*((DWORD *)&DrawVert[4].pos.z) = drawz ;
				*((DWORD *)&DrawVert[5].pos.z) = drawz ;

				// テクスチャーを描画する
				ADD4VERTEX_TEX
			}
			else
			{
				// 頂点データを初期化する
				GETVERTEX_QUAD( DrawVert )
				
				TexVert = DrawTex->Vertex ;

				DrawVert[0].color = ( DWORD )DiffuseColor ;
				DrawVert[3].color = ( DWORD )DiffuseColor ;

				DrawVert[0].u = TexVert[0].u ;
				DrawVert[0].v = TexVert[0].v ;
				DrawVert[1].u = TexVert[1].u ;
				DrawVert[1].v = TexVert[0].v ;
				DrawVert[2].u = TexVert[0].u ;
				DrawVert[2].v = TexVert[2].v ;
				DrawVert[3].u = TexVert[1].u ;
				DrawVert[3].v = TexVert[2].v ;
				DrawVert[4].u = TexVert[0].u ;
				DrawVert[4].v = TexVert[2].v ;
				DrawVert[5].u = TexVert[1].u ;
				DrawVert[5].v = TexVert[0].v ;

				XBuf[0] = ( TexVert[0].x - CenX ) * ExtendRateX ;
				YBuf[0] = ( TexVert[0].y - CenY ) * ExtendRateY ;

				XBuf[1] = ( TexVert[1].x - CenX ) * ExtendRateX ;
				YBuf[1] = ( TexVert[1].y - CenY ) * ExtendRateY ;

				DrawVert[0].pos.x =  XBuf[0] + xf ;
				DrawVert[0].pos.y =  YBuf[0] + yf ;
				DrawVert[1].pos.x =  XBuf[1] + xf ;
				DrawVert[1].pos.y =  YBuf[1] + yf ;
				DrawVert[2].pos.x = -XBuf[1] + xf ;
				DrawVert[2].pos.y = -YBuf[1] + yf ;
				DrawVert[3].pos.x = -XBuf[0] + xf ;
				DrawVert[3].pos.y = -YBuf[0] + yf ;
				DrawVert[4].pos.x = -XBuf[1] + xf ;
				DrawVert[4].pos.y = -YBuf[1] + yf ;
				DrawVert[5].pos.x =  XBuf[1] + xf ;
				DrawVert[5].pos.y =  YBuf[1] + yf ;

				*((DWORD *)&DrawVert[0].pos.z) = drawz ;
				*((DWORD *)&DrawVert[1].pos.z) = drawz ;
				*((DWORD *)&DrawVert[2].pos.z) = drawz ;
				*((DWORD *)&DrawVert[3].pos.z) = drawz ;
				*((DWORD *)&DrawVert[4].pos.z) = drawz ;
				*((DWORD *)&DrawVert[5].pos.z) = drawz ;

				// テクスチャーを描画する
				ADD4VERTEX_TEX
			}
		}
		else
		{
			if( SinCosNone == 0 )
			{
				// テクスチャーの数だけ繰り返す
				i = 0 ;
				for(;;)
				{
					// 頂点データを初期化する
					GETVERTEX_QUAD( DrawVert )
					
					TexVert = DrawTex->Vertex ;

					DrawVert[0].color = ( DWORD )DiffuseColor ;
					DrawVert[3].color = ( DWORD )DiffuseColor ;

					DrawVert[0].u = TexVert[0].u ;
					DrawVert[0].v = TexVert[0].v ;
					DrawVert[1].u = TexVert[1].u ;
					DrawVert[1].v = TexVert[0].v ;
					DrawVert[2].u = TexVert[0].u ;
					DrawVert[2].v = TexVert[2].v ;
					DrawVert[3].u = TexVert[1].u ;
					DrawVert[3].v = TexVert[2].v ;
					DrawVert[4].u = TexVert[0].u ;
					DrawVert[4].v = TexVert[2].v ;
					DrawVert[5].u = TexVert[1].u ;
					DrawVert[5].v = TexVert[0].v ;

					XBuf[0] = ( TexVert[0].x - CenX ) * ExtendRateX ;
					YBuf[0] = ( TexVert[0].y - CenY ) * ExtendRateY ;
					XBuf[1] = XBuf[0] * Cos - YBuf[0] * Sin ;
					YBuf[1] = XBuf[0] * Sin + YBuf[0] * Cos ;

					XBuf[2] = ( TexVert[1].x - CenX ) * ExtendRateX ;
					YBuf[2] = ( TexVert[1].y - CenY ) * ExtendRateY ;
					XBuf[3] = XBuf[2] * Cos - YBuf[2] * Sin ;
					YBuf[3] = XBuf[2] * Sin + YBuf[2] * Cos ;

					XBuf[4] = ( TexVert[2].x - CenX ) * ExtendRateX ;
					YBuf[4] = ( TexVert[2].y - CenY ) * ExtendRateY ;
					XBuf[5] = XBuf[4] * Cos - YBuf[4] * Sin ;
					YBuf[5] = XBuf[4] * Sin + YBuf[4] * Cos ;

					XBuf[6] = ( TexVert[3].x - CenX ) * ExtendRateX ;
					YBuf[6] = ( TexVert[3].y - CenY ) * ExtendRateY ;
					XBuf[7] = XBuf[6] * Cos - YBuf[6] * Sin ;
					YBuf[7] = XBuf[6] * Sin + YBuf[6] * Cos ;

					if( AlwaysCheck )
					{
						float f;

						DrawPos[0].x = XBuf[1] + xf ;
						DrawPos[0].y = YBuf[1] + yf ;
						DrawPos[1].x = XBuf[3] + xf ;
						DrawPos[1].y = YBuf[3] + yf ;
						DrawPos[2].x = XBuf[5] + xf ;
						DrawPos[2].y = YBuf[5] + yf ;
						DrawPos[3].x = XBuf[7] + xf ;
						DrawPos[3].y = YBuf[7] + yf ;

						f = _FABS( DrawPos[0].x - _FTOL( DrawPos[0].x ) );
						if( ( ( ( *((DWORD *)&DrawPos[0].x) == *((DWORD *)&DrawPos[2].x) ) ||
							    ( ( DrawPos[0].x >= DrawPos[2].x - 0.01f ) && ( DrawPos[0].x < DrawPos[2].x + 0.01f ) ) ||
							    ( ( DrawPos[2].x >= DrawPos[0].x - 0.01f ) && ( DrawPos[2].x < DrawPos[0].x + 0.01f ) ) ) &&
							    ( f < 0.001f || f > 0.999f ) ) ||
							( ( ( *((DWORD *)&DrawPos[0].x) == *((DWORD *)&DrawPos[1].x) ) ||
							    ( ( DrawPos[0].x >= DrawPos[1].x - 0.01f ) && ( DrawPos[0].x < DrawPos[1].x + 0.01f ) ) ||
							    ( ( DrawPos[1].x >= DrawPos[0].x - 0.01f ) && ( DrawPos[1].x < DrawPos[0].x + 0.01f ) ) ) &&
							    ( f < 0.001f || f > 0.999f ) ) )
						{
							DrawPos[0].x -= 0.5f;
							DrawPos[1].x -= 0.5f;
							DrawPos[2].x -= 0.5f;
							DrawPos[3].x -= 0.5f;
						}

						f = _FABS( DrawPos[0].y - _FTOL( DrawPos[0].y ) );
						if( ( ( ( *((DWORD *)&DrawPos[0].y) == *((DWORD *)&DrawPos[2].y) ) ||
							    ( ( DrawPos[0].y >= DrawPos[2].y - 0.01f ) && ( DrawPos[0].y < DrawPos[2].y + 0.01f ) ) ||
							    ( ( DrawPos[2].y >= DrawPos[0].y - 0.01f ) && ( DrawPos[2].y < DrawPos[0].y + 0.01f ) ) ) &&
							    ( f < 0.001f || f > 0.999f ) ) ||
							( ( ( *((DWORD *)&DrawPos[0].y) == *((DWORD *)&DrawPos[1].y) ) ||
							    ( ( DrawPos[0].y >= DrawPos[1].y - 0.01f ) && ( DrawPos[0].y < DrawPos[1].y + 0.01f ) ) ||
							    ( ( DrawPos[1].y >= DrawPos[0].y - 0.01f ) && ( DrawPos[1].y < DrawPos[0].y + 0.01f ) ) ) &&
							    ( f < 0.001f || f > 0.999f ) ) )
						{
							DrawPos[0].y -= 0.5f;
							DrawPos[1].y -= 0.5f;
							DrawPos[2].y -= 0.5f;
							DrawPos[3].y -= 0.5f;
						}

						DrawVert[0].pos.x = DrawPos[0].x ;
						DrawVert[0].pos.y = DrawPos[0].y ;
						DrawVert[1].pos.x = DrawPos[1].x ;
						DrawVert[1].pos.y = DrawPos[1].y ;
						DrawVert[2].pos.x = DrawPos[2].x ;
						DrawVert[2].pos.y = DrawPos[2].y ;
						DrawVert[3].pos.x = DrawPos[3].x ;
						DrawVert[3].pos.y = DrawPos[3].y ;
						DrawVert[4].pos.x = DrawPos[2].x ;
						DrawVert[4].pos.y = DrawPos[2].y ;
						DrawVert[5].pos.x = DrawPos[1].x ;
						DrawVert[5].pos.y = DrawPos[1].y ;
					}
					else
					if( AngleCheck )
					{
						float f;

						DrawPos[0].x = XBuf[1] + xf ;
						DrawPos[0].y = YBuf[1] + yf ;
						DrawPos[1].x = XBuf[3] + xf ;
						DrawPos[1].y = YBuf[3] + yf ;
						DrawPos[2].x = XBuf[5] + xf ;
						DrawPos[2].y = YBuf[5] + yf ;
						DrawPos[3].x = XBuf[7] + xf ;
						DrawPos[3].y = YBuf[7] + yf ;

						if( hx )
						{
							f = _FABS( DrawPos[0].x - _FTOL( DrawPos[0].x ) );
							if( ( ( ( *((DWORD *)&DrawPos[0].x) == *((DWORD *)&DrawPos[2].x) ) ||
									( ( DrawPos[0].x >= DrawPos[2].x - 0.01f ) && ( DrawPos[0].x < DrawPos[2].x + 0.01f ) ) ||
									( ( DrawPos[2].x >= DrawPos[0].x - 0.01f ) && ( DrawPos[2].x < DrawPos[0].x + 0.01f ) ) ) &&
									( f < 0.001f || f > 0.999f ) ) ||
								( ( ( *((DWORD *)&DrawPos[0].x) == *((DWORD *)&DrawPos[1].x) ) ||
									( ( DrawPos[0].x >= DrawPos[1].x - 0.01f ) && ( DrawPos[0].x < DrawPos[1].x + 0.01f ) ) ||
									( ( DrawPos[1].x >= DrawPos[0].x - 0.01f ) && ( DrawPos[1].x < DrawPos[0].x + 0.01f ) ) ) &&
									( f < 0.001f || f > 0.999f ) ) )
							{
								DrawPos[0].x -= 0.5f;
								DrawPos[1].x -= 0.5f;
								DrawPos[2].x -= 0.5f;
								DrawPos[3].x -= 0.5f;
							}
						}

						if( hy )
						{
							f = _FABS( DrawPos[0].y - _FTOL( DrawPos[0].y ) );
							if( ( ( ( *((DWORD *)&DrawPos[0].y) == *((DWORD *)&DrawPos[2].y) ) ||
									( ( DrawPos[0].y >= DrawPos[2].y - 0.01f ) && ( DrawPos[0].y < DrawPos[2].y + 0.01f ) ) ||
									( ( DrawPos[2].y >= DrawPos[0].y - 0.01f ) && ( DrawPos[2].y < DrawPos[0].y + 0.01f ) ) ) &&
									( f < 0.001f || f > 0.999f ) ) ||
								( ( ( *((DWORD *)&DrawPos[0].y) == *((DWORD *)&DrawPos[1].y) ) ||
									( ( DrawPos[0].y >= DrawPos[1].y - 0.01f ) && ( DrawPos[0].y < DrawPos[1].y + 0.01f ) ) ||
									( ( DrawPos[1].y >= DrawPos[0].y - 0.01f ) && ( DrawPos[1].y < DrawPos[0].y + 0.01f ) ) ) &&
									( f < 0.001f || f > 0.999f ) ) )
							{
								DrawPos[0].y -= 0.5f;
								DrawPos[1].y -= 0.5f;
								DrawPos[2].y -= 0.5f;
								DrawPos[3].y -= 0.5f;
							}
						}

						DrawVert[0].pos.x = DrawPos[0].x ;
						DrawVert[0].pos.y = DrawPos[0].y ;
						DrawVert[1].pos.x = DrawPos[1].x ;
						DrawVert[1].pos.y = DrawPos[1].y ;
						DrawVert[2].pos.x = DrawPos[2].x ;
						DrawVert[2].pos.y = DrawPos[2].y ;
						DrawVert[3].pos.x = DrawPos[3].x ;
						DrawVert[3].pos.y = DrawPos[3].y ;
						DrawVert[4].pos.x = DrawPos[2].x ;
						DrawVert[4].pos.y = DrawPos[2].y ;
						DrawVert[5].pos.x = DrawPos[1].x ;
						DrawVert[5].pos.y = DrawPos[1].y ;
					}
					else
					{
						DrawVert[0].pos.x = XBuf[1] + xf ;
						DrawVert[0].pos.y = YBuf[1] + yf ;
						DrawVert[1].pos.x = XBuf[3] + xf ;
						DrawVert[1].pos.y = YBuf[3] + yf ;
						DrawVert[2].pos.x = XBuf[5] + xf ;
						DrawVert[2].pos.y = YBuf[5] + yf ;
						DrawVert[3].pos.x = XBuf[7] + xf ;
						DrawVert[3].pos.y = YBuf[7] + yf ;
						DrawVert[4].pos.x = XBuf[5] + xf ;
						DrawVert[4].pos.y = YBuf[5] + yf ;
						DrawVert[5].pos.x = XBuf[3] + xf ;
						DrawVert[5].pos.y = YBuf[3] + yf ;
					}

					*((DWORD *)&DrawVert[0].pos.z) = drawz ;
					*((DWORD *)&DrawVert[1].pos.z) = drawz ;
					*((DWORD *)&DrawVert[2].pos.z) = drawz ;
					*((DWORD *)&DrawVert[3].pos.z) = drawz ;
					*((DWORD *)&DrawVert[4].pos.z) = drawz ;
					*((DWORD *)&DrawVert[5].pos.z) = drawz ;

					// テクスチャーを描画する
					ADD4VERTEX_TEX

					i ++ ;
					if( i >= DrawTexNum ) break ;

					// 描画するテクスチャーのセット
					DrawTex ++ ;
					Graphics_D3D9_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D9.Texture ) ;
					Graphics_D3D9_DeviceState_RefreshBlendState() ;
				}
			}
			else
			{
				// テクスチャーの数だけ繰り返す
				i = 0 ;
				for(;;)
				{
					// 頂点データを初期化する
					GETVERTEX_QUAD( DrawVert )
					
					TexVert = DrawTex->Vertex ;

					DrawVert[0].color = ( DWORD )DiffuseColor ;
					DrawVert[3].color = ( DWORD )DiffuseColor ;

					DrawVert[0].u = TexVert[0].u ;
					DrawVert[0].v = TexVert[0].v ;
					DrawVert[1].u = TexVert[1].u ;
					DrawVert[1].v = TexVert[0].v ;
					DrawVert[2].v = TexVert[2].v ;
					DrawVert[2].u = TexVert[0].u ;
					DrawVert[3].u = TexVert[1].u ;
					DrawVert[3].v = TexVert[2].v ;
					DrawVert[4].u = TexVert[0].u ;
					DrawVert[4].v = TexVert[2].v ;
					DrawVert[5].u = TexVert[1].u ;
					DrawVert[5].v = TexVert[0].v ;

					XBuf[0] = ( TexVert[0].x - CenX ) * ExtendRateX + xf ;
					YBuf[0] = ( TexVert[0].y - CenY ) * ExtendRateY + yf ;
					XBuf[1] = ( TexVert[1].x - CenX ) * ExtendRateX + xf ;
					YBuf[1] = ( TexVert[1].y - CenY ) * ExtendRateY + yf ;
					XBuf[2] = ( TexVert[2].x - CenX ) * ExtendRateX + xf ;
					YBuf[2] = ( TexVert[2].y - CenY ) * ExtendRateY + yf ;
					XBuf[3] = ( TexVert[3].x - CenX ) * ExtendRateX + xf ;
					YBuf[3] = ( TexVert[3].y - CenY ) * ExtendRateY + yf ;

					DrawVert[0].pos.x = XBuf[0] ;
					DrawVert[0].pos.y = YBuf[0] ;
					DrawVert[1].pos.x = XBuf[1] ;
					DrawVert[1].pos.y = YBuf[1] ;
					DrawVert[2].pos.x = XBuf[2] ;
					DrawVert[2].pos.y = YBuf[2] ;
					DrawVert[3].pos.x = XBuf[3] ;
					DrawVert[3].pos.y = YBuf[3] ;
					DrawVert[4].pos.x = XBuf[2] ;
					DrawVert[4].pos.y = YBuf[2] ;
					DrawVert[5].pos.x = XBuf[1] ;
					DrawVert[5].pos.y = YBuf[1] ;

					*((DWORD *)&DrawVert[0].pos.z) = drawz ;
					*((DWORD *)&DrawVert[1].pos.z) = drawz ;
					*((DWORD *)&DrawVert[2].pos.z) = drawz ;
					*((DWORD *)&DrawVert[3].pos.z) = drawz ;
					*((DWORD *)&DrawVert[4].pos.z) = drawz ;
					*((DWORD *)&DrawVert[5].pos.z) = drawz ;

					// テクスチャーを描画する
					ADD4VERTEX_TEX

					i ++ ;
					if( i >= DrawTexNum ) break ;

					// 描画するテクスチャーのセット
					DrawTex ++ ;
					Graphics_D3D9_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D9.Texture ) ;
					Graphics_D3D9_DeviceState_RefreshBlendState() ;
				}
			}
		}
		break ;
	}

	// 終了
	return 0 ;
}

// ハードウエアアクセラレータ使用版 DrawRotaGraph
extern	int		Graphics_D3D9_DrawRotaGraphFast( int /*x*/, int /*y*/, float xf, float yf, float ExtendRate, float Angle, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, int ReverseXFlag, int ReverseYFlag, int /*IntFlag*/ )
{
	VERTEX_2D *DrawVert ;
	VERTEX_BLENDTEX_2D *DrawVertB ;
	DWORD DiffuseColor ;
	IMAGEDATA_HARD_DRAW *DrawTex ;
	IMAGEDATA_HARD_DRAW *BlendDrawTex = NULL ;
	IMAGEDATA_HARD_VERT *TexVert ;
	IMAGEDATA_HARD_VERT *BlendTexVert ;
	int DrawTexNum ;
	int i ; 
	float Sin = 0.0f, Cos = 1.0f ;
	int BlendGraphNoIncFlag ;
	float XBuf[ 8 ], YBuf[ 8 ] ;
	float u[ 4 ], v[ 4 ] ;
	float ExtendRateX ;
	float ExtendRateY ;
	DWORD drawz ;
//	BOOL Adj = FALSE ;
	float CenX, CenY ;
	int SinCosNone ;
	int Flag ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	// 幅がなかった場合はエラー
	if( ExtendRate == 0.0f ) return -1 ;

	// 描画の準備
	if( GD3D9.Device.DrawInfo.BeginSceneFlag == FALSE ) Graphics_D3D9_BeginScene() ;
	Flag = TransFlag ;
	DX_D3D9_DRAWPREP_TEX( Image->Orig, Image->Hard.Draw[ 0 ].Tex->PF->D3D9.Texture, Flag )

	// 回転要素が無いか調べる
	if( Angle == 0.0f )
	{
		SinCosNone = 1;
	}
	else
	{
		SinCosNone = 0;
		_TABLE_SINCOS( Angle, &Sin, &Cos ) ;
	}

	// 中心座標の計算
	CenX = ( float )Image->WidthF * 0.5F ;
	CenY = ( float )Image->HeightF * 0.5F ;
	xf -= 0.5F ;
	yf -= 0.5F ;

	// 頂点データを取得（今のところ色情報のセットのみ）
	DiffuseColor = GD3D9.Device.DrawInfo.DiffuseColor ;

	// 描画情報の数をセット
	DrawTexNum = Image->Hard.DrawNum ;

	// 描画情報の数がブレンド画像と異なっていたら０番目のテクスチャだけを使用する
	BlendGraphNoIncFlag = FALSE ;
	if( BlendImage != NULL && BlendImage->Hard.DrawNum != Image->Hard.DrawNum )
	{
		BlendGraphNoIncFlag = TRUE ;
	}

	// 描画情報配列のアドレスをセットしておく
	DrawTex = Image->Hard.Draw ;
	if( BlendImage != NULL )
	{
		BlendDrawTex = BlendImage->Hard.Draw ;
	}

	// Ｚバッファに書き込むＺ値をセットする
	drawz = *((DWORD *)&GSYS.DrawSetting.DrawZ);

	// 反転を考慮した拡大率をセット
	ExtendRateX = ( ( ReverseXFlag == TRUE ) ? -1.0f : 1.0f ) * ExtendRate ;
	ExtendRateY = ( ( ReverseYFlag == TRUE ) ? -1.0f : 1.0f ) * ExtendRate ;

	// 頂点タイプによって処理を分岐
	switch( GD3D9.Device.DrawInfo.VertexType )
	{
	case VERTEXTYPE_BLENDTEX :	// ブレンドテクスチャを使用する
		// テクスチャーの数だけ繰り返す
		if( DrawTexNum == 1 )
		{
			// 描画するテクスチャーのセット
			Graphics_D3D9_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D9.Texture ) ;
			Graphics_D3D9_DrawSetting_SetBlendTexture( BlendDrawTex->Tex->PF->D3D9.Texture, BlendDrawTex->Tex->TexWidth, BlendDrawTex->Tex->TexHeight );

			if( GD3D9.Device.DrawSetting.ChangeTextureFlag )
				Graphics_D3D9_DeviceState_RefreshBlendState() ;

			// 頂点データを初期化する
			GETVERTEX_QUAD( DrawVertB )

			TexVert = DrawTex->Vertex ;
			BlendTexVert = BlendDrawTex->Vertex ;

			DrawVertB[0].color = ( DWORD )DiffuseColor ;
			DrawVertB[3].color = ( DWORD )DiffuseColor ;

			XBuf[0] = ( TexVert[0].x - CenX ) * ExtendRateX ;
			YBuf[0] = ( TexVert[0].y - CenY ) * ExtendRateY ;
			XBuf[1] = XBuf[0] * Cos - YBuf[0] * Sin ;
			YBuf[1] = XBuf[0] * Sin + YBuf[0] * Cos ;

			XBuf[2] = ( TexVert[1].x - CenX ) * ExtendRateX ;
			YBuf[2] = ( TexVert[1].y - CenY ) * ExtendRateY ;
			XBuf[3] = XBuf[2] * Cos - YBuf[2] * Sin ;
			YBuf[3] = XBuf[2] * Sin + YBuf[2] * Cos ;

			DrawVertB[0].pos.x =  XBuf[1] + xf ;
			DrawVertB[0].pos.y =  YBuf[1] + yf ;
			DrawVertB[1].pos.x =  XBuf[3] + xf ;
			DrawVertB[1].pos.y =  YBuf[3] + yf ;
			DrawVertB[2].pos.x = -XBuf[3] + xf ;
			DrawVertB[2].pos.y = -YBuf[3] + yf ;
			DrawVertB[3].pos.x = -XBuf[1] + xf ;
			DrawVertB[3].pos.y = -YBuf[1] + yf ;
			DrawVertB[4].pos.x = -XBuf[3] + xf ;
			DrawVertB[4].pos.y = -YBuf[3] + yf ;
			DrawVertB[5].pos.x =  XBuf[3] + xf ;
			DrawVertB[5].pos.y =  YBuf[3] + yf ;

			switch( GSYS.DrawSetting.BlendGraphPosMode )
			{
			default :
			case DX_BLENDGRAPH_POSMODE_DRAWGRAPH:
				u[ 0 ] = TexVert[0].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
				u[ 1 ] = TexVert[1].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
				v[ 0 ] = TexVert[0].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
				v[ 1 ] = TexVert[2].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
				DrawVertB[0].u1 = u[ 0 ] ;
				DrawVertB[0].v1 = v[ 0 ] ;
				DrawVertB[1].u1 = u[ 1 ] ;
				DrawVertB[1].v1 = v[ 0 ] ;
				DrawVertB[2].u1 = u[ 0 ] ;
				DrawVertB[2].v1 = v[ 1 ] ;
				DrawVertB[3].u1 = u[ 1 ] ;
				DrawVertB[3].v1 = v[ 1 ] ;
				DrawVertB[4].u1 = u[ 0 ] ;
				DrawVertB[4].v1 = v[ 1 ] ;
				DrawVertB[5].u1 = u[ 1 ] ;
				DrawVertB[5].v1 = v[ 0 ] ;
				break ;

			case DX_BLENDGRAPH_POSMODE_SCREEN :
				DrawVertB[0].u1 = ( (  XBuf[1] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
				DrawVertB[0].v1 = ( (  YBuf[1] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
				DrawVertB[1].u1 = ( (  XBuf[3] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
				DrawVertB[1].v1 = ( (  YBuf[3] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
				DrawVertB[2].u1 = ( ( -XBuf[3] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
				DrawVertB[2].v1 = ( ( -YBuf[3] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
				DrawVertB[3].u1 = ( ( -XBuf[1] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
				DrawVertB[3].v1 = ( ( -YBuf[1] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
				DrawVertB[4].u1 = ( ( -XBuf[3] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
				DrawVertB[4].v1 = ( ( -YBuf[3] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
				DrawVertB[5].u1 = ( (  XBuf[3] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
				DrawVertB[5].v1 = ( (  YBuf[3] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
				break ;
			}

			DrawVertB[0].u2 = TexVert[0].u ;
			DrawVertB[0].v2 = TexVert[0].v ;
			DrawVertB[1].u2 = TexVert[1].u ;
			DrawVertB[1].v2 = TexVert[0].v ;
			DrawVertB[2].u2 = TexVert[0].u ;
			DrawVertB[2].v2 = TexVert[2].v ;
			DrawVertB[3].u2 = TexVert[1].u ;
			DrawVertB[3].v2 = TexVert[2].v ;
			DrawVertB[4].u2 = TexVert[0].u ;
			DrawVertB[4].v2 = TexVert[2].v ;
			DrawVertB[5].u2 = TexVert[1].u ;
			DrawVertB[5].v2 = TexVert[0].v ;

			*((DWORD *)&DrawVertB[0].pos.z) = drawz ;
			*((DWORD *)&DrawVertB[1].pos.z) = drawz ;
			*((DWORD *)&DrawVertB[2].pos.z) = drawz ;
			*((DWORD *)&DrawVertB[3].pos.z) = drawz ;
			*((DWORD *)&DrawVertB[4].pos.z) = drawz ;
			*((DWORD *)&DrawVertB[5].pos.z) = drawz ;

			// テクスチャーを描画する
			ADD4VERTEX_BLENDTEX

			if( BlendGraphNoIncFlag == FALSE )
			{
				BlendDrawTex ++ ;
			}
		}
		else
		{
			// テクスチャーの数だけ繰り返す
			for( i = 0 ; i < DrawTexNum ; i ++, DrawTex ++ )
			{
				// 描画するテクスチャーのセット
				Graphics_D3D9_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D9.Texture ) ;
				Graphics_D3D9_DrawSetting_SetBlendTexture( BlendDrawTex->Tex->PF->D3D9.Texture, BlendDrawTex->Tex->TexWidth, BlendDrawTex->Tex->TexHeight );

				if( GD3D9.Device.DrawSetting.ChangeTextureFlag )
					Graphics_D3D9_DeviceState_RefreshBlendState() ;

				// 頂点データを初期化する
				GETVERTEX_QUAD( DrawVertB )

				TexVert = DrawTex->Vertex ;
				BlendTexVert = BlendDrawTex->Vertex ;

				DrawVertB[0].color = ( DWORD )DiffuseColor ;
				DrawVertB[3].color = ( DWORD )DiffuseColor ;

				XBuf[0] = ( TexVert[0].x - CenX ) * ExtendRateX ;
				YBuf[0] = ( TexVert[0].y - CenY ) * ExtendRateY ;
				XBuf[1] = XBuf[0] * Cos - YBuf[0] * Sin ;
				YBuf[1] = XBuf[0] * Sin + YBuf[0] * Cos ;

				XBuf[2] = ( TexVert[1].x - CenX ) * ExtendRateX ;
				YBuf[2] = ( TexVert[1].y - CenY ) * ExtendRateY ;
				XBuf[3] = XBuf[2] * Cos - YBuf[2] * Sin ;
				YBuf[3] = XBuf[2] * Sin + YBuf[2] * Cos ;

				XBuf[4] = ( TexVert[2].x - CenX ) * ExtendRateX ;
				YBuf[4] = ( TexVert[2].y - CenY ) * ExtendRateY ;
				XBuf[5] = XBuf[4] * Cos - YBuf[4] * Sin ;
				YBuf[5] = XBuf[4] * Sin + YBuf[4] * Cos ;

				XBuf[6] = ( TexVert[3].x - CenX ) * ExtendRateX ;
				YBuf[6] = ( TexVert[3].y - CenY ) * ExtendRateY ;
				XBuf[7] = XBuf[6] * Cos - YBuf[6] * Sin ;
				YBuf[7] = XBuf[6] * Sin + YBuf[6] * Cos ;

				DrawVertB[0].pos.x = XBuf[1] + xf ;
				DrawVertB[0].pos.y = YBuf[1] + yf ;
				DrawVertB[1].pos.x = XBuf[3] + xf ;
				DrawVertB[1].pos.y = YBuf[3] + yf ;
				DrawVertB[2].pos.x = XBuf[5] + xf ;
				DrawVertB[2].pos.y = YBuf[5] + yf ;
				DrawVertB[3].pos.x = XBuf[7] + xf ;
				DrawVertB[3].pos.y = YBuf[7] + yf ;
				DrawVertB[4].pos.x = XBuf[5] + xf ;
				DrawVertB[4].pos.y = YBuf[5] + yf ;
				DrawVertB[5].pos.x = XBuf[3] + xf ;
				DrawVertB[5].pos.y = YBuf[3] + yf ;

				switch( GSYS.DrawSetting.BlendGraphPosMode )
				{
				default :
				case DX_BLENDGRAPH_POSMODE_DRAWGRAPH:
					u[ 0 ] = ( TexVert[0].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					u[ 1 ] = ( TexVert[1].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					u[ 2 ] = ( TexVert[2].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					u[ 3 ] = ( TexVert[3].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					
					v[ 0 ] = ( TexVert[0].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					v[ 1 ] = ( TexVert[1].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					v[ 2 ] = ( TexVert[2].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					v[ 3 ] = ( TexVert[3].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;

					DrawVertB[0].u1 = u[ 0 ] ;
					DrawVertB[0].v1 = v[ 0 ] ;
					DrawVertB[1].u1 = u[ 1 ] ;
					DrawVertB[1].v1 = v[ 1 ] ;
					DrawVertB[2].u1 = u[ 2 ] ;
					DrawVertB[2].v1 = v[ 2 ] ;
					DrawVertB[3].u1 = u[ 3 ] ;
					DrawVertB[3].v1 = v[ 3 ] ;
					DrawVertB[4].u1 = u[ 2 ] ;
					DrawVertB[4].v1 = v[ 2 ] ;
					DrawVertB[5].u1 = u[ 1 ] ;
					DrawVertB[5].v1 = v[ 1 ] ;
					break ;

				case DX_BLENDGRAPH_POSMODE_SCREEN :
					DrawVertB[0].u1 = ( ( XBuf[1] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[0].v1 = ( ( YBuf[1] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[1].u1 = ( ( XBuf[3] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[1].v1 = ( ( YBuf[3] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[2].u1 = ( ( XBuf[5] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[2].v1 = ( ( YBuf[5] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[3].u1 = ( ( XBuf[7] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[3].v1 = ( ( YBuf[7] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[4].u1 = ( ( XBuf[5] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[4].v1 = ( ( YBuf[5] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[5].u1 = ( ( XBuf[3] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[5].v1 = ( ( YBuf[3] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					break ;
				}

				DrawVertB[0].u2 = TexVert[0].u ;
				DrawVertB[0].v2 = TexVert[0].v ;
				DrawVertB[1].u2 = TexVert[1].u ;
				DrawVertB[1].v2 = TexVert[0].v ;
				DrawVertB[2].u2 = TexVert[0].u ;
				DrawVertB[2].v2 = TexVert[2].v ;
				DrawVertB[3].u2 = TexVert[1].u ;
				DrawVertB[3].v2 = TexVert[2].v ;
				DrawVertB[4].u2 = TexVert[0].u ;
				DrawVertB[4].v2 = TexVert[2].v ;
				DrawVertB[5].u2 = TexVert[1].u ;
				DrawVertB[5].v2 = TexVert[0].v ;

				*((DWORD *)&DrawVertB[0].pos.z) = drawz ;
				*((DWORD *)&DrawVertB[1].pos.z) = drawz ;
				*((DWORD *)&DrawVertB[2].pos.z) = drawz ;
				*((DWORD *)&DrawVertB[3].pos.z) = drawz ;
				*((DWORD *)&DrawVertB[4].pos.z) = drawz ;
				*((DWORD *)&DrawVertB[5].pos.z) = drawz ;

				// テクスチャーを描画する
				ADD4VERTEX_BLENDTEX

				if( BlendGraphNoIncFlag == FALSE )
				{
					BlendDrawTex ++ ;
				}
			}
		}
		break ;

	case VERTEXTYPE_TEX :	// ブレンドテクスチャを使用しない
		// テクスチャーの数だけ繰り返す
		if( DrawTexNum == 1 )
		{
			if( SinCosNone == 0 )
			{
				// 頂点データを初期化する
				GETVERTEX_QUAD( DrawVert )
				
				TexVert = DrawTex->Vertex ;

				DrawVert[0].color = ( DWORD )DiffuseColor ;
				DrawVert[3].color = ( DWORD )DiffuseColor ;

				DrawVert[0].u = TexVert[0].u ;
				DrawVert[0].v = TexVert[0].v ;
				DrawVert[1].u = TexVert[1].u ;
				DrawVert[1].v = TexVert[0].v ;
				DrawVert[2].v = TexVert[2].v ;
				DrawVert[2].u = TexVert[0].u ;
				DrawVert[3].u = TexVert[1].u ;
				DrawVert[3].v = TexVert[2].v ;
				DrawVert[4].u = TexVert[0].u ;
				DrawVert[4].v = TexVert[2].v ;
				DrawVert[5].u = TexVert[1].u ;
				DrawVert[5].v = TexVert[0].v ;

				XBuf[0] = ( TexVert[0].x - CenX ) * ExtendRateX ;
				YBuf[0] = ( TexVert[0].y - CenY ) * ExtendRateY ;
				XBuf[1] = XBuf[0] * Cos - YBuf[0] * Sin ;
				YBuf[1] = XBuf[0] * Sin + YBuf[0] * Cos ;
				
				XBuf[2] = ( TexVert[1].x - CenX ) * ExtendRateX ;
				YBuf[2] = ( TexVert[1].y - CenY ) * ExtendRateY ;
				XBuf[3] = XBuf[2] * Cos - YBuf[2] * Sin ;
				YBuf[3] = XBuf[2] * Sin + YBuf[2] * Cos ;

				DrawVert[0].pos.x =  XBuf[1] + xf ;
				DrawVert[0].pos.y =  YBuf[1] + yf ;
				DrawVert[1].pos.x =  XBuf[3] + xf ;
				DrawVert[1].pos.y =  YBuf[3] + yf ;
				DrawVert[2].pos.x = -XBuf[3] + xf ;
				DrawVert[2].pos.y = -YBuf[3] + yf ;
				DrawVert[3].pos.x = -XBuf[1] + xf ;
				DrawVert[3].pos.y = -YBuf[1] + yf ;
				DrawVert[4].pos.x = -XBuf[3] + xf ;
				DrawVert[4].pos.y = -YBuf[3] + yf ;
				DrawVert[5].pos.x =  XBuf[3] + xf ;
				DrawVert[5].pos.y =  YBuf[3] + yf ;

				*((DWORD *)&DrawVert[0].pos.z) = drawz ;
				*((DWORD *)&DrawVert[1].pos.z) = drawz ;
				*((DWORD *)&DrawVert[2].pos.z) = drawz ;
				*((DWORD *)&DrawVert[3].pos.z) = drawz ;
				*((DWORD *)&DrawVert[4].pos.z) = drawz ;
				*((DWORD *)&DrawVert[5].pos.z) = drawz ;

				// テクスチャーを描画する
				ADD4VERTEX_TEX
			}
			else
			{
				// 頂点データを初期化する
				GETVERTEX_QUAD( DrawVert )
				
				TexVert = DrawTex->Vertex ;

				DrawVert[0].color = ( DWORD )DiffuseColor ;
				DrawVert[3].color = ( DWORD )DiffuseColor ;

				DrawVert[0].u = TexVert[0].u ;
				DrawVert[0].v = TexVert[0].v ;
				DrawVert[1].u = TexVert[1].u ;
				DrawVert[1].v = TexVert[0].v ;
				DrawVert[2].u = TexVert[0].u ;
				DrawVert[2].v = TexVert[2].v ;
				DrawVert[3].u = TexVert[1].u ;
				DrawVert[3].v = TexVert[2].v ;
				DrawVert[4].u = TexVert[0].u ;
				DrawVert[4].v = TexVert[2].v ;
				DrawVert[5].u = TexVert[1].u ;
				DrawVert[5].v = TexVert[0].v ;

				XBuf[0] = ( TexVert[0].x - CenX ) * ExtendRateX ;
				YBuf[0] = ( TexVert[0].y - CenY ) * ExtendRateY ;

				XBuf[1] = ( TexVert[1].x - CenX ) * ExtendRateX ;
				YBuf[1] = ( TexVert[1].y - CenY ) * ExtendRateY ;

				DrawVert[0].pos.x =  XBuf[0] + xf ;
				DrawVert[0].pos.y =  YBuf[0] + yf ;
				DrawVert[1].pos.x =  XBuf[1] + xf ;
				DrawVert[1].pos.y =  YBuf[1] + yf ;
				DrawVert[2].pos.x = -XBuf[1] + xf ;
				DrawVert[2].pos.y = -YBuf[1] + yf ;
				DrawVert[3].pos.x = -XBuf[0] + xf ;
				DrawVert[3].pos.y = -YBuf[0] + yf ;
				DrawVert[4].pos.x = -XBuf[1] + xf ;
				DrawVert[4].pos.y = -YBuf[1] + yf ;
				DrawVert[5].pos.x =  XBuf[1] + xf ;
				DrawVert[5].pos.y =  YBuf[1] + yf ;

				*((DWORD *)&DrawVert[0].pos.z) = drawz ;
				*((DWORD *)&DrawVert[1].pos.z) = drawz ;
				*((DWORD *)&DrawVert[2].pos.z) = drawz ;
				*((DWORD *)&DrawVert[3].pos.z) = drawz ;
				*((DWORD *)&DrawVert[4].pos.z) = drawz ;
				*((DWORD *)&DrawVert[5].pos.z) = drawz ;

				// テクスチャーを描画する
				ADD4VERTEX_TEX
			}
		}
		else
		{
			if( SinCosNone == 0 )
			{
				// テクスチャーの数だけ繰り返す
				i = 0 ;
				for(;;)
				{
					// 頂点データを初期化する
					GETVERTEX_QUAD( DrawVert )
					
					TexVert = DrawTex->Vertex ;

					DrawVert[0].color = ( DWORD )DiffuseColor ;
					DrawVert[3].color = ( DWORD )DiffuseColor ;

					DrawVert[0].u = TexVert[0].u ;
					DrawVert[0].v = TexVert[0].v ;
					DrawVert[1].u = TexVert[1].u ;
					DrawVert[1].v = TexVert[0].v ;
					DrawVert[2].u = TexVert[0].u ;
					DrawVert[2].v = TexVert[2].v ;
					DrawVert[3].u = TexVert[1].u ;
					DrawVert[3].v = TexVert[2].v ;
					DrawVert[4].u = TexVert[0].u ;
					DrawVert[4].v = TexVert[2].v ;
					DrawVert[5].u = TexVert[1].u ;
					DrawVert[5].v = TexVert[0].v ;

					XBuf[0] = ( TexVert[0].x - CenX ) * ExtendRateX ;
					YBuf[0] = ( TexVert[0].y - CenY ) * ExtendRateY ;
					XBuf[1] = XBuf[0] * Cos - YBuf[0] * Sin ;
					YBuf[1] = XBuf[0] * Sin + YBuf[0] * Cos ;

					XBuf[2] = ( TexVert[1].x - CenX ) * ExtendRateX ;
					YBuf[2] = ( TexVert[1].y - CenY ) * ExtendRateY ;
					XBuf[3] = XBuf[2] * Cos - YBuf[2] * Sin ;
					YBuf[3] = XBuf[2] * Sin + YBuf[2] * Cos ;

					XBuf[4] = ( TexVert[2].x - CenX ) * ExtendRateX ;
					YBuf[4] = ( TexVert[2].y - CenY ) * ExtendRateY ;
					XBuf[5] = XBuf[4] * Cos - YBuf[4] * Sin ;
					YBuf[5] = XBuf[4] * Sin + YBuf[4] * Cos ;

					XBuf[6] = ( TexVert[3].x - CenX ) * ExtendRateX ;
					YBuf[6] = ( TexVert[3].y - CenY ) * ExtendRateY ;
					XBuf[7] = XBuf[6] * Cos - YBuf[6] * Sin ;
					YBuf[7] = XBuf[6] * Sin + YBuf[6] * Cos ;

					DrawVert[0].pos.x = XBuf[1] + xf ;
					DrawVert[0].pos.y = YBuf[1] + yf ;
					DrawVert[1].pos.x = XBuf[3] + xf ;
					DrawVert[1].pos.y = YBuf[3] + yf ;
					DrawVert[2].pos.x = XBuf[5] + xf ;
					DrawVert[2].pos.y = YBuf[5] + yf ;
					DrawVert[3].pos.x = XBuf[7] + xf ;
					DrawVert[3].pos.y = YBuf[7] + yf ;
					DrawVert[4].pos.x = XBuf[5] + xf ;
					DrawVert[4].pos.y = YBuf[5] + yf ;
					DrawVert[5].pos.x = XBuf[3] + xf ;
					DrawVert[5].pos.y = YBuf[3] + yf ;

					*((DWORD *)&DrawVert[0].pos.z) = drawz ;
					*((DWORD *)&DrawVert[1].pos.z) = drawz ;
					*((DWORD *)&DrawVert[2].pos.z) = drawz ;
					*((DWORD *)&DrawVert[3].pos.z) = drawz ;
					*((DWORD *)&DrawVert[4].pos.z) = drawz ;
					*((DWORD *)&DrawVert[5].pos.z) = drawz ;

					// テクスチャーを描画する
					ADD4VERTEX_TEX

					i ++ ;
					if( i >= DrawTexNum ) break ;

					// 描画するテクスチャーのセット
					DrawTex ++ ;
					Graphics_D3D9_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D9.Texture ) ;
					Graphics_D3D9_DeviceState_RefreshBlendState() ;
				}
			}
			else
			{
				// テクスチャーの数だけ繰り返す
				i = 0 ;
				for(;;)
				{
					// 頂点データを初期化する
					GETVERTEX_QUAD( DrawVert )
					
					TexVert = DrawTex->Vertex ;

					DrawVert[0].color = ( DWORD )DiffuseColor ;
					DrawVert[3].color = ( DWORD )DiffuseColor ;

					DrawVert[0].u = TexVert[0].u ;
					DrawVert[0].v = TexVert[0].v ;
					DrawVert[1].u = TexVert[1].u ;
					DrawVert[1].v = TexVert[0].v ;
					DrawVert[2].v = TexVert[2].v ;
					DrawVert[2].u = TexVert[0].u ;
					DrawVert[3].u = TexVert[1].u ;
					DrawVert[3].v = TexVert[2].v ;
					DrawVert[4].u = TexVert[0].u ;
					DrawVert[4].v = TexVert[2].v ;
					DrawVert[5].u = TexVert[1].u ;
					DrawVert[5].v = TexVert[0].v ;

					XBuf[0] = ( TexVert[0].x - CenX ) * ExtendRateX + xf ;
					YBuf[0] = ( TexVert[0].y - CenY ) * ExtendRateY + yf ;
					XBuf[1] = ( TexVert[1].x - CenX ) * ExtendRateX + xf ;
					YBuf[1] = ( TexVert[1].y - CenY ) * ExtendRateY + yf ;
					XBuf[2] = ( TexVert[2].x - CenX ) * ExtendRateX + xf ;
					YBuf[2] = ( TexVert[2].y - CenY ) * ExtendRateY + yf ;
					XBuf[3] = ( TexVert[3].x - CenX ) * ExtendRateX + xf ;
					YBuf[3] = ( TexVert[3].y - CenY ) * ExtendRateY + yf ;

					DrawVert[0].pos.x = XBuf[0] ;
					DrawVert[0].pos.y = YBuf[0] ;
					DrawVert[1].pos.x = XBuf[1] ;
					DrawVert[1].pos.y = YBuf[1] ;
					DrawVert[2].pos.x = XBuf[2] ;
					DrawVert[2].pos.y = YBuf[2] ;
					DrawVert[3].pos.x = XBuf[3] ;
					DrawVert[3].pos.y = YBuf[3] ;
					DrawVert[4].pos.x = XBuf[2] ;
					DrawVert[4].pos.y = YBuf[2] ;
					DrawVert[5].pos.x = XBuf[1] ;
					DrawVert[5].pos.y = YBuf[1] ;

					*((DWORD *)&DrawVert[0].pos.z) = drawz ;
					*((DWORD *)&DrawVert[1].pos.z) = drawz ;
					*((DWORD *)&DrawVert[2].pos.z) = drawz ;
					*((DWORD *)&DrawVert[3].pos.z) = drawz ;
					*((DWORD *)&DrawVert[4].pos.z) = drawz ;
					*((DWORD *)&DrawVert[5].pos.z) = drawz ;

					// テクスチャーを描画する
					ADD4VERTEX_TEX

					i ++ ;
					if( i >= DrawTexNum ) break ;

					// 描画するテクスチャーのセット
					DrawTex ++ ;
					Graphics_D3D9_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D9.Texture ) ;
					Graphics_D3D9_DeviceState_RefreshBlendState() ;
				}
			}
		}
		break ;
	}

	// 終了
	return 0 ;
}

// ハードウエアアクセラレータ使用版 DrawModiGraph
extern	int		Graphics_D3D9_DrawModiGraph( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, bool SimpleDrawFlag )
{
	if( x1 == x3 && x2 == x4 && y1 == y2 && y3 == y4 ) return Graphics_D3D9_DrawExtendGraph( x1, y1, x4, y4, ( float )x1, ( float )y1, ( float )x4, ( float )y4, Image, BlendImage, TransFlag, TRUE ) ; 

	// もしブレンドテクスチャを使用せず、旧型関数を使用するフラグも立っていなければ
	// 内部分割を行う描画関数を使用する
	if( GD3D9.Setting.UseOldDrawModiGraphCodeFlag == FALSE && GSYS.DrawSetting.BlendGraph <= 0 )
		return Graphics_D3D9_DrawModiGraphF( (float)x1 - 0.5f, (float)y1 - 0.5f, (float)x2 - 0.5f, (float)y2 - 0.5f, (float)x3 - 0.5f, (float)y3 - 0.5f, (float)x4 - 0.5f, (float)y4 - 0.5f, Image, BlendImage, TransFlag, SimpleDrawFlag ) ;

	VERTEX_2D           *DrawVert ;
	VERTEX_BLENDTEX_2D	*DrawVertB ;
	IMAGEDATA_HARD_DRAW *DrawTex ;
	IMAGEDATA_HARD_DRAW *BlendDrawTex = NULL ;
	IMAGEDATA_HARD_VERT *TexVert ;
	IMAGEDATA_HARD_VERT *BlendTexVert ;
	float GraphSizeX, GraphSizeY ;
	float XBuf, YBuf ;
	int DrawTexNum ;
	unsigned char *VertType ;
	int i, j ; 
	int BlendGraphNoIncFlag ;
	float Reverse0/*, Reverse1 */ ;
	float ExRate1X1, ExRate1Y1, ExRate2X1, ExRate2Y1 ;
	float ExRate1X2, ExRate1Y2, ExRate2X2, ExRate2Y2 ;
	DWORD DiffuseColor ;
	VECTOR DrawPos[ 4 ] ;
	float u[ 4 ] ;
	float v[ 4 ] ;
	float  xf1, yf1, xf4, yf4;
	DWORD drawz ;
	int Flag ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画の準備
	if( GD3D9.Device.DrawInfo.BeginSceneFlag == FALSE ) Graphics_D3D9_BeginScene() ;
	Flag = TransFlag | DX_D3D9_DRAWPREP_VECTORINT ;
	DX_D3D9_DRAWPREP_TEX( Image->Orig, Image->Hard.Draw[ 0 ].Tex->PF->D3D9.Texture, Flag )

	GraphSizeX = ( float )Image->WidthF ;  GraphSizeY = ( float )Image->HeightF ;
	ExRate1X1 = ( x2 - x1 ) /  GraphSizeX ;  ExRate1X2 = ( x3 - x1 ) /  GraphSizeY ;
	ExRate1Y1 = ( y3 - y1 ) /  GraphSizeY ;  ExRate1Y2 = ( y2 - y1 ) /  GraphSizeX ;
	ExRate2X1 = ( x3 - x4 ) / -GraphSizeX ;  ExRate2X2 = ( x2 - x4 ) / -GraphSizeY ;
	ExRate2Y1 = ( y2 - y4 ) / -GraphSizeY ;  ExRate2Y2 = ( y3 - y4 ) / -GraphSizeX ;
	xf1 = (float)x1 - 0.5F ; yf1 = (float)y1 - 0.5F ;
	xf4 = (float)x4 - 0.5F ; yf4 = (float)y4 - 0.5F ; 

	// 頂点データを取得（今のところ色情報のセットのみ）
	DiffuseColor = GD3D9.Device.DrawInfo.DiffuseColor ;

	// Ｚバッファに書き込むＺ値をセットする
	drawz = *((DWORD *)&GSYS.DrawSetting.DrawZ);

	// 描画情報の数をセット
	DrawTexNum = Image->Hard.DrawNum ;

	// 描画情報の数がブレンド画像と異なっていたら０番目のテクスチャだけを使用する
	BlendGraphNoIncFlag = FALSE ;
	if( BlendImage != NULL && BlendImage->Hard.DrawNum != Image->Hard.DrawNum )
	{
		BlendGraphNoIncFlag = TRUE ;
	}

	// 描画情報配列のアドレスをセットしておく
	DrawTex = Image->Hard.Draw ;
	if( BlendImage != NULL )
	{
		BlendDrawTex = BlendImage->Hard.Draw ;
	}

	// 頂点タイプによって処理を分岐
	switch( GD3D9.Device.DrawInfo.VertexType )
	{
	case VERTEXTYPE_BLENDTEX :	// ブレンドテクスチャを使用する
		// FVF設定
//		Graphics_D3D9_DeviceState_SetFVF( VERTEXFVF_BLENDTEX_2D ) ;
//		Graphics_Hardware_D3D9_SetFVF_UseTable( VERTEXFVF_DECL_BLENDTEX_2D ) ;

		// テクスチャーの数だけ繰り返す
		for( i = 0 ; i < DrawTexNum ; i ++, DrawTex ++ )
		{
			// 描画するテクスチャーのセット
			Graphics_D3D9_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D9.Texture ) ;
			Graphics_D3D9_DrawSetting_SetBlendTexture( BlendDrawTex->Tex->PF->D3D9.Texture, BlendDrawTex->Tex->TexWidth, BlendDrawTex->Tex->TexHeight );

			if( GD3D9.Device.DrawSetting.ChangeTextureFlag )
			{
				Graphics_D3D9_DeviceState_RefreshBlendState() ;
			}

			// 頂点バッファの取得
			GETVERTEX_QUAD( DrawVertB )

			// 頂点データをセットする
			DrawVertB[0].color = (DWORD)DiffuseColor ;
			DrawVertB[1].color = (DWORD)DiffuseColor ;
			DrawVertB[2].color = (DWORD)DiffuseColor ;
			DrawVertB[3].color = (DWORD)DiffuseColor ;
			DrawVertB[4].color = (DWORD)DiffuseColor ;
			DrawVertB[5].color = (DWORD)DiffuseColor ;

			VertType = DrawTex->VertType ;
			TexVert = DrawTex->Vertex ;
			BlendTexVert = BlendDrawTex->Vertex ;

			for( j = 0 ; j < 4 ; j ++, TexVert ++ )
			{
				// 頂点座標の算出
				if( *VertType ++ )
				{
					// 三角の上側
					DrawPos[ j ].x = ( TexVert->x * ExRate1X1 ) + ( TexVert->y * ExRate1X2 ) + xf1 ;
					DrawPos[ j ].y = ( TexVert->y * ExRate1Y1 ) + ( TexVert->x * ExRate1Y2 ) + yf1 ;
				} 
				else
				{
					// 三角の下側
					XBuf = TexVert->x - GraphSizeX ;
					YBuf = TexVert->y - GraphSizeY ;
					DrawPos[ j ].x = ( XBuf * ExRate2X1 ) + ( YBuf * ExRate2X2 ) + xf4 ;
					DrawPos[ j ].y = ( YBuf * ExRate2Y1 ) + ( XBuf * ExRate2Y2 ) + yf4 ;
				}

				u[ j ] = ( TexVert->x - BlendTexVert->x + GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
				v[ j ] = ( TexVert->y - BlendTexVert->y + GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
			}

			// 反転判定処理
			Reverse0 =  ( DrawPos[ 1 ].x - DrawPos[ 0 ].x ) * ( DrawPos[ 2 ].y - DrawPos[ 0 ].y ) -
						( DrawPos[ 2 ].x - DrawPos[ 0 ].x ) * ( DrawPos[ 1 ].y - DrawPos[ 0 ].y ) ;
//			Reverse1 = -( DrawPos[ 1 ].x - DrawPos[ 3 ].x ) * ( DrawPos[ 2 ].y - DrawPos[ 3 ].y ) +
//						( DrawPos[ 1 ].y - DrawPos[ 3 ].y ) * ( DrawPos[ 2 ].x - DrawPos[ 3 ].x ) ;
			TexVert = DrawTex->Vertex ;
			if( Reverse0 <= 0 )
			{
				DrawVertB[ 0 ].pos.x = DrawPos[ 0 ].x ;
				DrawVertB[ 0 ].pos.y = DrawPos[ 0 ].y ;
				DrawVertB[ 1 ].pos.x = DrawPos[ 2 ].x ;
				DrawVertB[ 1 ].pos.y = DrawPos[ 2 ].y ;
				DrawVertB[ 2 ].pos.x = DrawPos[ 1 ].x ;
				DrawVertB[ 2 ].pos.y = DrawPos[ 1 ].y ;
				DrawVertB[ 3 ].pos.x = DrawPos[ 3 ].x ;
				DrawVertB[ 3 ].pos.y = DrawPos[ 3 ].y ;
				DrawVertB[ 4 ].pos.x = DrawPos[ 1 ].x ;
				DrawVertB[ 4 ].pos.y = DrawPos[ 1 ].y ;
				DrawVertB[ 5 ].pos.x = DrawPos[ 2 ].x ;
				DrawVertB[ 5 ].pos.y = DrawPos[ 2 ].y ;

				switch( GSYS.DrawSetting.BlendGraphPosMode )
				{
				default :
				case DX_BLENDGRAPH_POSMODE_DRAWGRAPH:
					DrawVertB[ 0 ].u1 = u[ 0 ] ;
					DrawVertB[ 0 ].v1 = v[ 0 ] ;
					DrawVertB[ 1 ].u1 = u[ 2 ] ;
					DrawVertB[ 1 ].v1 = v[ 2 ] ;
					DrawVertB[ 2 ].u1 = u[ 1 ] ;
					DrawVertB[ 2 ].v1 = v[ 1 ] ;
					DrawVertB[ 3 ].u1 = u[ 3 ] ;
					DrawVertB[ 3 ].v1 = v[ 3 ] ;
					DrawVertB[ 4 ].u1 = u[ 1 ] ;
					DrawVertB[ 4 ].v1 = v[ 1 ] ;
					DrawVertB[ 5 ].u1 = u[ 2 ] ;
					DrawVertB[ 5 ].v1 = v[ 2 ] ;
					break ;

				case DX_BLENDGRAPH_POSMODE_SCREEN :
					DrawVertB[ 0 ].u1 = ( DrawPos[ 0 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[ 0 ].v1 = ( DrawPos[ 0 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[ 1 ].u1 = ( DrawPos[ 2 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[ 1 ].v1 = ( DrawPos[ 2 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[ 2 ].u1 = ( DrawPos[ 1 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[ 2 ].v1 = ( DrawPos[ 1 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[ 3 ].u1 = ( DrawPos[ 3 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[ 3 ].v1 = ( DrawPos[ 3 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[ 4 ].u1 = ( DrawPos[ 1 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[ 4 ].v1 = ( DrawPos[ 1 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[ 5 ].u1 = ( DrawPos[ 2 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[ 5 ].v1 = ( DrawPos[ 2 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					break ;
				}

				DrawVertB[ 0 ].u2 = TexVert[ 0 ].u ;
				DrawVertB[ 0 ].v2 = TexVert[ 0 ].v ;
				DrawVertB[ 1 ].u2 = TexVert[ 2 ].u ;
				DrawVertB[ 1 ].v2 = TexVert[ 2 ].v ;
				DrawVertB[ 2 ].u2 = TexVert[ 1 ].u ;
				DrawVertB[ 2 ].v2 = TexVert[ 1 ].v ;
				DrawVertB[ 3 ].u2 = TexVert[ 3 ].u ;
				DrawVertB[ 3 ].v2 = TexVert[ 3 ].v ;
				DrawVertB[ 4 ].u2 = TexVert[ 1 ].u ;
				DrawVertB[ 4 ].v2 = TexVert[ 1 ].v ;
				DrawVertB[ 5 ].u2 = TexVert[ 2 ].u ;
				DrawVertB[ 5 ].v2 = TexVert[ 2 ].v ;
			}
			else
			{
				DrawVertB[ 0 ].pos.x = DrawPos[ 0 ].x ;
				DrawVertB[ 0 ].pos.y = DrawPos[ 0 ].y ;
				DrawVertB[ 1 ].pos.x = DrawPos[ 1 ].x ;
				DrawVertB[ 1 ].pos.y = DrawPos[ 1 ].y ;
				DrawVertB[ 2 ].pos.x = DrawPos[ 2 ].x ;
				DrawVertB[ 2 ].pos.y = DrawPos[ 2 ].y ;
				DrawVertB[ 3 ].pos.x = DrawPos[ 3 ].x ;
				DrawVertB[ 3 ].pos.y = DrawPos[ 3 ].y ;
				DrawVertB[ 4 ].pos.x = DrawPos[ 2 ].x ;
				DrawVertB[ 4 ].pos.y = DrawPos[ 2 ].y ;
				DrawVertB[ 5 ].pos.x = DrawPos[ 1 ].x ;
				DrawVertB[ 5 ].pos.y = DrawPos[ 1 ].y ;

				switch( GSYS.DrawSetting.BlendGraphPosMode )
				{
				default :
				case DX_BLENDGRAPH_POSMODE_DRAWGRAPH:
					DrawVertB[ 0 ].u1 = u[ 0 ] ;
					DrawVertB[ 0 ].v1 = v[ 0 ] ;
					DrawVertB[ 1 ].u1 = u[ 1 ] ;
					DrawVertB[ 1 ].v1 = v[ 1 ] ;
					DrawVertB[ 2 ].u1 = u[ 2 ] ;
					DrawVertB[ 2 ].v1 = v[ 2 ] ;
					DrawVertB[ 3 ].u1 = u[ 3 ] ;
					DrawVertB[ 3 ].v1 = v[ 3 ] ;
					DrawVertB[ 4 ].u1 = u[ 2 ] ;
					DrawVertB[ 4 ].v1 = v[ 2 ] ;
					DrawVertB[ 5 ].u1 = u[ 1 ] ;
					DrawVertB[ 5 ].v1 = v[ 1 ] ;
					break ;

				case DX_BLENDGRAPH_POSMODE_SCREEN :
					DrawVertB[ 0 ].u1 = ( DrawPos[ 0 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[ 0 ].v1 = ( DrawPos[ 0 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[ 1 ].u1 = ( DrawPos[ 1 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[ 1 ].v1 = ( DrawPos[ 1 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[ 2 ].u1 = ( DrawPos[ 2 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[ 2 ].v1 = ( DrawPos[ 2 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[ 3 ].u1 = ( DrawPos[ 3 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[ 3 ].v1 = ( DrawPos[ 3 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[ 4 ].u1 = ( DrawPos[ 2 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[ 4 ].v1 = ( DrawPos[ 2 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[ 5 ].u1 = ( DrawPos[ 1 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[ 5 ].v1 = ( DrawPos[ 1 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
					break ;
				}

				DrawVertB[ 0 ].u2 = TexVert[ 0 ].u ;
				DrawVertB[ 0 ].v2 = TexVert[ 0 ].v ;
				DrawVertB[ 1 ].u2 = TexVert[ 1 ].u ;
				DrawVertB[ 1 ].v2 = TexVert[ 1 ].v ;
				DrawVertB[ 2 ].u2 = TexVert[ 2 ].u ;
				DrawVertB[ 2 ].v2 = TexVert[ 2 ].v ;
				DrawVertB[ 3 ].u2 = TexVert[ 3 ].u ;
				DrawVertB[ 3 ].v2 = TexVert[ 3 ].v ;
				DrawVertB[ 4 ].u2 = TexVert[ 2 ].u ;
				DrawVertB[ 4 ].v2 = TexVert[ 2 ].v ;
				DrawVertB[ 5 ].u2 = TexVert[ 1 ].u ;
				DrawVertB[ 5 ].v2 = TexVert[ 1 ].v ;
			}

			*((DWORD *)&DrawVertB[0].pos.z) = drawz ;
			*((DWORD *)&DrawVertB[1].pos.z) = drawz ;
			*((DWORD *)&DrawVertB[2].pos.z) = drawz ;
			*((DWORD *)&DrawVertB[3].pos.z) = drawz ;
			*((DWORD *)&DrawVertB[4].pos.z) = drawz ;
			*((DWORD *)&DrawVertB[5].pos.z) = drawz ;

			// テクスチャーを描画する
			ADD4VERTEX_BLENDTEX

			if( BlendGraphNoIncFlag == FALSE )
			{
				BlendDrawTex ++ ;
			}
		}
		break ;

	case VERTEXTYPE_TEX :	// ブレンドテクスチャを使用しない
		// FVF設定
//		Graphics_D3D9_DeviceState_SetFVF( VERTEXFVF_2D ) ;
//		Graphics_Hardware_D3D9_SetFVF_UseTable( VERTEXFVF_DECL_2D ) ;

		// テクスチャーの数だけ繰り返す
		i = 0 ;
		for(;;)
		{
			// 頂点バッファの取得
			GETVERTEX_QUAD( DrawVert )

			// 頂点データをセットする
			DrawVert[ 0 ].color = DiffuseColor ;
			DrawVert[ 1 ].color = DiffuseColor ;
			DrawVert[ 2 ].color = DiffuseColor ;
			DrawVert[ 3 ].color = DiffuseColor ;
			DrawVert[ 4 ].color = DiffuseColor ;
			DrawVert[ 5 ].color = DiffuseColor ;

			VertType = DrawTex->VertType ;
			TexVert  = DrawTex->Vertex ;

			for( j = 0 ; j < 4 ; j ++, TexVert ++ )
			{
				// 頂点座標の算出
				if( *VertType ++ )
				{
					// 三角の上側
					DrawPos[ j ].x = ( TexVert->x * ExRate1X1 ) + ( TexVert->y * ExRate1X2 ) + xf1 ;
					DrawPos[ j ].y = ( TexVert->y * ExRate1Y1 ) + ( TexVert->x * ExRate1Y2 ) + yf1 ;
				} 
				else
				{
					// 三角の下側
					XBuf = TexVert->x - GraphSizeX ;
					YBuf = TexVert->y - GraphSizeY ;
					DrawPos[ j ].x = ( XBuf * ExRate2X1 ) + ( YBuf * ExRate2X2 ) + xf4 ;
					DrawPos[ j ].y = ( YBuf * ExRate2Y1 ) + ( XBuf * ExRate2Y2 ) + yf4 ;
				}
			}

			// 反転判定処理
			Reverse0 =  ( DrawPos[ 1 ].x - DrawPos[ 0 ].x ) * ( DrawPos[ 2 ].y - DrawPos[ 0 ].y ) -
						( DrawPos[ 2 ].x - DrawPos[ 0 ].x ) * ( DrawPos[ 1 ].y - DrawPos[ 0 ].y ) ;
//			Reverse1 = -( DrawPos[ 1 ].x - DrawPos[ 3 ].x ) * ( DrawPos[ 2 ].y - DrawPos[ 3 ].y ) +
//						( DrawPos[ 1 ].y - DrawPos[ 3 ].y ) * ( DrawPos[ 2 ].x - DrawPos[ 3 ].x ) ;
			TexVert = DrawTex->Vertex ;
			if( Reverse0 <= 0 )
			{
				DrawVert[ 0 ].pos.x = DrawPos[ 0 ].x ;
				DrawVert[ 0 ].pos.y = DrawPos[ 0 ].y ;
				DrawVert[ 1 ].pos.x = DrawPos[ 2 ].x ;
				DrawVert[ 1 ].pos.y = DrawPos[ 2 ].y ;
				DrawVert[ 2 ].pos.x = DrawPos[ 1 ].x ;
				DrawVert[ 2 ].pos.y = DrawPos[ 1 ].y ;
				DrawVert[ 3 ].pos.x = DrawPos[ 3 ].x ;
				DrawVert[ 3 ].pos.y = DrawPos[ 3 ].y ;
				DrawVert[ 4 ].pos.x = DrawPos[ 1 ].x ;
				DrawVert[ 4 ].pos.y = DrawPos[ 1 ].y ;
				DrawVert[ 5 ].pos.x = DrawPos[ 2 ].x ;
				DrawVert[ 5 ].pos.y = DrawPos[ 2 ].y ;

				DrawVert[ 0 ].u = TexVert[ 0 ].u ;
				DrawVert[ 0 ].v = TexVert[ 0 ].v ;
				DrawVert[ 1 ].u = TexVert[ 2 ].u ;
				DrawVert[ 1 ].v = TexVert[ 2 ].v ;
				DrawVert[ 2 ].u = TexVert[ 1 ].u ;
				DrawVert[ 2 ].v = TexVert[ 1 ].v ;
				DrawVert[ 3 ].u = TexVert[ 3 ].u ;
				DrawVert[ 3 ].v = TexVert[ 3 ].v ;
				DrawVert[ 4 ].u = TexVert[ 1 ].u ;
				DrawVert[ 4 ].v = TexVert[ 1 ].v ;
				DrawVert[ 5 ].u = TexVert[ 2 ].u ;
				DrawVert[ 5 ].v = TexVert[ 2 ].v ;
			}
			else
			{
				DrawVert[ 0 ].pos.x = DrawPos[ 0 ].x ;
				DrawVert[ 0 ].pos.y = DrawPos[ 0 ].y ;
				DrawVert[ 1 ].pos.x = DrawPos[ 1 ].x ;
				DrawVert[ 1 ].pos.y = DrawPos[ 1 ].y ;
				DrawVert[ 2 ].pos.x = DrawPos[ 2 ].x ;
				DrawVert[ 2 ].pos.y = DrawPos[ 2 ].y ;
				DrawVert[ 3 ].pos.x = DrawPos[ 3 ].x ;
				DrawVert[ 3 ].pos.y = DrawPos[ 3 ].y ;
				DrawVert[ 4 ].pos.x = DrawPos[ 2 ].x ;
				DrawVert[ 4 ].pos.y = DrawPos[ 2 ].y ;
				DrawVert[ 5 ].pos.x = DrawPos[ 1 ].x ;
				DrawVert[ 5 ].pos.y = DrawPos[ 1 ].y ;

				DrawVert[ 0 ].u = TexVert[ 0 ].u ;
				DrawVert[ 0 ].v = TexVert[ 0 ].v ;
				DrawVert[ 1 ].u = TexVert[ 1 ].u ;
				DrawVert[ 1 ].v = TexVert[ 1 ].v ;
				DrawVert[ 2 ].u = TexVert[ 2 ].u ;
				DrawVert[ 2 ].v = TexVert[ 2 ].v ;
				DrawVert[ 3 ].u = TexVert[ 3 ].u ;
				DrawVert[ 3 ].v = TexVert[ 3 ].v ;
				DrawVert[ 4 ].u = TexVert[ 2 ].u ;
				DrawVert[ 4 ].v = TexVert[ 2 ].v ;
				DrawVert[ 5 ].u = TexVert[ 1 ].u ;
				DrawVert[ 5 ].v = TexVert[ 1 ].v ;
			}

			*((DWORD *)&DrawVert[0].pos.z) = drawz ;
			*((DWORD *)&DrawVert[1].pos.z) = drawz ;
			*((DWORD *)&DrawVert[2].pos.z) = drawz ;
			*((DWORD *)&DrawVert[3].pos.z) = drawz ;
			*((DWORD *)&DrawVert[4].pos.z) = drawz ;
			*((DWORD *)&DrawVert[5].pos.z) = drawz ;

			// 描画選択
/*			if( Reverse0 * Reverse1 >= 0 )
			{
				// プリミティブの描画
				Direct3DDevice9_DrawPrimitiveUP( D_D3DPT_TRIANGLESTRIP, 2, VertData, sizeof( VERTEX_2D ) ) ;
			}
			else
			{
				// Ｘ方向反転時の補正
				if( Reverse0 < 0 ) 
				{
					VertBuf = VertData[ 1 ] ; VertData[ 1 ] = VertData[ 2 ] ; VertData[ 2 ] = VertBuf ;
				}

				// プリミティブの描画
				Direct3DDevice9_DrawPrimitiveUP( D_D3DPT_TRIANGLESTRIP, 2, VertData, sizeof( VERTEX_2D ) ) ;
	//			GRH.Direct3DDeviceObject->DrawPrimitive( D_D3DPT_TRIANGLESTRIP, VERTEXFVF_2D, VertData,     3, 0 ) ;
	//			GRH.Direct3DDeviceObject->DrawPrimitive( D_D3DPT_TRIANGLESTRIP, VERTEXFVF_2D, &VertData[1], 3, 0 ) ;
			}
*/
			ADD4VERTEX_TEX

			i ++ ;
			if( i >= DrawTexNum ) break ;

			// 描画するテクスチャーのセット
			DrawTex ++ ;
			Graphics_D3D9_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D9.Texture ) ;
			Graphics_D3D9_DeviceState_RefreshBlendState() ;
		}
		break ;
	}

	// 終了
	return 0 ;
}

// ハードウエアアクセラレータ使用版 DrawModiGraphF
extern	int		Graphics_D3D9_DrawModiGraphF( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, bool SimpleDrawFlag )
{
	int Flag ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	// もしブレンドテクスチャ付の場合は内部分割を行わない描画関数を使用する
	if( GSYS.DrawSetting.BlendGraph > 0 ) return Graphics_D3D9_DrawModiGraph( _FTOL( x1 ), _FTOL( y1 ), _FTOL( x2 ), _FTOL( y2 ), _FTOL( x3 ), _FTOL( y3 ), _FTOL( x4 ), _FTOL( y4 ), Image, BlendImage, TransFlag, SimpleDrawFlag ) ;

	// 描画の準備
	if( GD3D9.Device.DrawInfo.BeginSceneFlag == FALSE ) Graphics_D3D9_BeginScene() ;
	Flag = TransFlag ;
	DX_D3D9_DRAWPREP_TEX( Image->Orig, Image->Hard.Draw[ 0 ].Tex->PF->D3D9.Texture, Flag )

	// テクスチャーが１枚のみの場合は処理を分岐
	if( Image->Hard.DrawNum == 1 )
	{
		// そのまま描画
		Graphics_D3D9_DrawModiTex( x1, y1, x2, y2, x3, y3, x4, y4, &Image->Hard.Draw[ 0 ], false ) ;
	}
	else
	{
		IMAGEDATA_HARD_DRAW *DrawTex ;
		IMAGEDATA_HARD_VERT *TexVert ;
		int DrawTexNum ;
		int i, j ;
		double xx1, yy1, xx2, yy2, xx3, yy3, xx4, yy4, xt1, yt1, xt2, yt2 ;
		double w, h, r ;
		float x[4], y[4], *xp, *yp ;

		xx1 = x2 - x1 ; xx2 = x4 - x3 ;
		yy1 = y2 - y1 ; yy2 = y4 - y3 ;

		xx3 = x3 - x1 ; xx4 = x4 - x2 ;
		yy3 = y3 - y1 ; yy4 = y4 - y2 ;
		
//		w = 1.0F / (float)Image->Width ;
//		h = 1.0F / (float)Image->Height ;
		w = 1.0F / (double)Image->WidthF ;
		h = 1.0F / (double)Image->HeightF ;

		// 描画情報の数をセット
		DrawTexNum = Image->Hard.DrawNum ;

		// 描画情報配列のアドレスをセットしておく
		DrawTex = Image->Hard.Draw ;

		// テクスチャーの数だけ繰り返す
		for( i = 0 ; i < DrawTexNum ; i ++, DrawTex ++ )
		{
			// テクスチャーのデータアドレスを取得
			TexVert = DrawTex->Vertex ;

			// 座標を算出
			xp = x ;
			yp = y ;
			for( j = 0 ; j < 4 ; j ++, TexVert ++ )
			{
				r = TexVert->y * h ;
				xt1 = x1 + xx3 * r ;	yt1 = y1 + yy3 * r ;
				xt2 = x2 + xx4 * r ;	yt2 = y2 + yy4 * r ;

				r = TexVert->x * w ;
				*xp++ = (float)( ( xt2 - xt1 ) * r + xt1 ) ;
				*yp++ = (float)( ( yt2 - yt1 ) * r + yt1 ) ;
			}

			// 描画
			Graphics_D3D9_DrawModiTex( x[0], y[0], x[1], y[1], x[2], y[2], x[3], y[3], DrawTex, false ) ;
		}
	}

	// 終了
	return 0 ;
}

// ２次元配列的に配置された頂点データを頂点バッファに追加する
static int SetPlaneVertexHardware( VERTEX_2D *GraphVert, int xnum, int ynum )
{
	int i, j, k, l ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return 0 ;

	if( xnum < 2 || ynum < 2 ) return -1 ;

	// トライアングルリストモードの場合はバッファは使用しない
	DRAWSTOCKINFO

	// バッファを使用しない設定になっていたらこの場で描画
	{
#define MAX_POSNUM		1000
		WORD list[MAX_POSNUM] ;

		// 頂点インデックスリストを作成する
		k = 0 ;
		l = 0 ;
		for( i = 0 ; i < ynum - 1 ; i ++ )
		{
			// 頂点数が超えそうだったら描画
			if( k + xnum * 2 + 2 > MAX_POSNUM )
			{
//				Graphics_D3D9_DeviceState_SetFVF( VERTEXFVF_2D ) ;
				Graphics_Hardware_D3D9_SetFVF_UseTable( VERTEXFVF_DECL_2D ) ;
				Direct3DDevice9_DrawIndexedPrimitiveUP(
					D_D3DPT_TRIANGLESTRIP,
					0,
					( UINT )( xnum * ynum ),
					( UINT )( k - 2 ),
					list,
					D_D3DFMT_INDEX16,
					GraphVert,
					sizeof( VERTEX_2D ) ) ;
				GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;
				k = 0 ;
			}

			// 前の行からの続きだった場合はストリップの連続用の頂点を出力		
			if( k != 0 )
			{
				list[k] = ( WORD )( l + xnum ) ;
				k ++ ;
			}
			
			for( j = 0 ; j < xnum ; j ++, l ++ )
			{
				list[k] = ( WORD )( l + xnum ) ;
				list[k+1] = ( WORD )l ;
				k += 2 ;
			}
			if( i != ynum - 2 )
			{
				list[k] = list[k-1] ;
				k ++ ;
			}
		}
		if( k != 0 )
		{
//			Graphics_D3D9_DeviceState_SetFVF( VERTEXFVF_2D ) ;
			Graphics_Hardware_D3D9_SetFVF_UseTable( VERTEXFVF_DECL_2D ) ;
			Direct3DDevice9_DrawIndexedPrimitiveUP(
				D_D3DPT_TRIANGLESTRIP,
				0,
				( UINT )( xnum * ynum ),
				( UINT )( k - 2 ),
				list,
				D_D3DFMT_INDEX16,
				GraphVert,
				sizeof( VERTEX_2D ) ) ;
			GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;
		}
#undef	MAX_POSNUM
	}

	// 終了
	return 0 ;
}

// テクスチャを変形して描画する
extern	void	Graphics_D3D9_DrawModiTex( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, IMAGEDATA_HARD_DRAW *DrawTex, bool SimpleDrawFlag )
{
	IMAGEDATA_HARD_VERT *TexVert ;
	VERTEX_2D *vec ; 
	VERTEX_2D VertBuf[2] ;
	float xx1, yy1, xx2, yy2, xx3, yy3, xx4, yy4/*, r1, r2, r3, r4*/ ;
//	float xtmp1, ytmp1, xtmp2, ytmp2 ;
	int c, f1, f2 ;
	DWORD DiffuseColor ;
	float Reverse0, Reverse1;
	DWORD drawz ;

	// 描画するテクスチャーのセット
	Graphics_D3D9_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D9.Texture ) ;
	Graphics_D3D9_DeviceState_RefreshBlendState() ;

	// 頂点データを取得（今のところ色情報のセットのみ）
	DiffuseColor = GD3D9.Device.DrawInfo.DiffuseColor ;

	// Ｚバッファに書き込むＺ値をセットする
	drawz = *((DWORD *)&GSYS.DrawSetting.DrawZ);

	TexVert = DrawTex->Vertex ;

	// 反転判定用処理
	Reverse0 =  ( x2 - x1 ) * ( y3 - y1 ) - ( x3 - x1 ) * ( y2 - y1 ) ;
	Reverse1 = -( x2 - x4 ) * ( y3 - y4 ) + ( y2 - y4 ) * ( x3 - x4 ) ;

	if( SimpleDrawFlag ) goto R1 ;

	// 平行四辺形以上の変形が起きていなかったら普通の処理を実行
	f1 = f2 = c = 0 ;
	xx1 = x2 - x1 ; xx2 = x4 - x3 ;
	yy1 = y2 - y1 ; yy2 = y4 - y3 ;

	xx3 = x3 - x1 ; xx4 = x4 - x2 ;
	yy3 = y3 - y1 ; yy4 = y4 - y2 ;

	if( _FABS( xx1 - xx2 ) < 1.0F && _FABS( yy1 - yy2 ) < 1.0F ) c ++, f1 = 1 ;
	if( _FABS( xx3 - xx4 ) < 1.0F && _FABS( yy3 - yy4 ) < 1.0F ) c ++, f2 = 1 ;
	if( c == 2 ) goto R1 ;

/*
	// 台形か完全な自由四角形かを判定
	f1 = f2 = c = 0 ;
	r1 = _SQRT( xx1 * xx1 + yy1 * yy1 ) ;	xtmp1 = xx1 / r1 ;	ytmp1 = yy1 / r1 ;
	r2 = _SQRT( xx2 * xx2 + yy2 * yy2 ) ;	xtmp2 = xx2 / r2 ;	ytmp2 = yy2 / r2 ;
	if( _FABS( xtmp1 - xtmp2 ) < 0.001F && _FABS( ytmp1 - ytmp2 ) < 0.001F ) c ++, f1 = 1 ;
	
	r3 = _SQRT( xx3 * xx3 + yy3 * yy3 ) ;	xtmp1 = xx3 / r3 ;	ytmp1 = yy3 / r3 ;
	r4 = _SQRT( xx4 * xx4 + yy4 * yy4 ) ;	xtmp2 = xx4 / r4 ;	ytmp2 = yy4 / r4 ;
	if( _FABS( xtmp1 - xtmp2 ) < 0.001F && _FABS( ytmp1 - ytmp2 ) < 0.001F ) c ++, f2 = 1 ;
*/

	{
#define TDIVNUM		5
#define TDN			(1 + ( 1 << TDIVNUM ))
#define ADJUSTUV	(0.001f)
		int DivNum, tdn ;
		static VERTEX_2D VertData2[TDN * TDN] ;
		VERTEX_2D *v ;
		float xxx1, yyy1, xxx2, yyy2, xxx3, yyy3 ;
		float adx1, ady1, adx2, ady2, adx3, ady3, adtu, adtv, tu, tv ; 
		int i, j ;
		float adjust_u_l, adjust_u_r, adjust_v_t, adjust_v_b ;

		// 分割画像の場合隣の画像のピクセルが入らないための補正ＵＶ値を作成する
		{
			if( TexVert[ 0 ].u < 0.000001f )
			{
				adjust_u_l = 0.0f ;
			}
			else
			{
				adjust_u_l =  ( TexVert[ 3 ].u - TexVert[ 0 ].u ) / ( TexVert[ 3 ].x - TexVert[ 0 ].x ) * ADJUSTUV ;
			}

			if( TexVert[ 3 ].u > 0.99999f )
			{
				adjust_u_r = 0.0f ;
			}
			else
			{
				adjust_u_r = -( TexVert[ 3 ].u - TexVert[ 0 ].u ) / ( TexVert[ 3 ].x - TexVert[ 0 ].x ) * ADJUSTUV ;
			}

			if( TexVert[ 0 ].v < 0.000001f )
			{
				adjust_v_t = 0.0f ;
			}
			else
			{
				adjust_v_t =  ( TexVert[ 3 ].v - TexVert[ 0 ].v ) / ( TexVert[ 3 ].y - TexVert[ 0 ].y ) * ADJUSTUV ;
			}

			if( TexVert[ 3 ].v > 0.99999f )
			{
				adjust_v_b = 0.0f ;
			}
			else
			{
				adjust_v_b = -( TexVert[ 3 ].v - TexVert[ 0 ].v ) / ( TexVert[ 3 ].y - TexVert[ 0 ].y ) * ADJUSTUV ;
			}
		}

		// 分割数の算出
		{
			float v1x, v1y, v2x, v2y ;
			float rate ;
			const float Table[] = { 2.0F, 1.0F, 0.6F, 0.1F, 0.05F } ;

			v1x = x4 - x1 ;		v1y = y4 - y1 ;
			v2x = x3 - x2 ;		v2y = y3 - y2 ;
			rate = (float)( 
				( _SQRT( v1x * v1x + v1y * v1y ) + _SQRT( v2x * v2x + v2y * v2y ) ) /
				( ( _SQRT( (float)( GSYS.DrawSetting.DrawSizeX * GSYS.DrawSetting.DrawSizeX + 
						GSYS.DrawSetting.DrawSizeY * GSYS.DrawSetting.DrawSizeY ) ) * 2 ) ) ) ;
			tdn = 8 ;
			if( tdn < 8 ) tdn = 8 ;
			else if( tdn > TDN ) tdn = TDN ;
			for( DivNum = TDIVNUM ; DivNum > 0 && rate < Table[TDIVNUM - DivNum] ; DivNum -- ){}
		}

		// 自由四角形処理
		adx1 = xx3 / ( tdn - 1 ) ;	ady1 = yy3 / ( tdn - 1 ) ;
		adx2 = xx4 / ( tdn - 1 ) ;	ady2 = yy4 / ( tdn - 1 ) ;
		xxx1 = x1 ;		yyy1 = y1 ;
		xxx2 = x2 ;		yyy2 = y2 ;
		adtu = ( ( TexVert[3].u + adjust_u_r ) - ( TexVert[0].u + adjust_u_l ) ) / ( tdn - 1 ) ;
		adtv = ( ( TexVert[3].v + adjust_v_b ) - ( TexVert[0].v + adjust_v_t ) ) / ( tdn - 1 ) ;
		tv = TexVert[0].v + adjust_v_t ;
		v = VertData2 ;
		for( i = 0 ; i < tdn ; i ++, tv += adtv,
									 xxx1 += adx1, yyy1 += ady1,
									 xxx2 += adx2, yyy2 += ady2 )
		{
			adx3 = ( xxx2 - xxx1 ) / ( tdn - 1 ) ;
			ady3 = ( yyy2 - yyy1 ) / ( tdn - 1 ) ;

//			xxx3 = xxx1 - 0.5F ;	yyy3 = yyy1 - 0.5F ;
			xxx3 = xxx1 ;
			yyy3 = yyy1 ;
			tu = TexVert[0].u + adjust_u_l ;
			for( j = 0 ; j < tdn ; j ++, tu += adtu,
										 xxx3 += adx3, yyy3 += ady3, v ++ )
			{
				v->pos.x = (float)xxx3 ;
				v->pos.y = (float)yyy3 ;
				*((DWORD *)&v->pos.z) = drawz ;
				v->rhw   = 1.0F ;
				v->u     = (float)tu ;
				v->v     = (float)tv ;
				v->color = DiffuseColor ;
			}
		}

		// 頂点追加処理
		SetPlaneVertexHardware( VertData2, tdn, tdn ) ;

#undef TDIVNUM
#undef TDN
	}

	return ;
		
R1 :
	// 普通に描画
	GETVERTEX_QUAD( vec )

	vec[0].color =
	vec[3].color = DiffuseColor ;

	vec[0].pos.x = x1 ;	
	vec[5].pos.x = vec[1].pos.x = x2 ;	
	vec[4].pos.x = vec[2].pos.x = x3 ;
	vec[3].pos.x = x4 ;
	
	vec[0].pos.y = y1 ;
	vec[5].pos.y = vec[1].pos.y = y2 ;
	vec[4].pos.y = vec[2].pos.y = y3 ;
	vec[3].pos.y = y4 ;

	vec[4].u = vec[2].u = vec[0].u = TexVert[0].u + 0.0625f / DrawTex->Tex->TexWidth ;
	vec[5].u = vec[3].u = vec[1].u = TexVert[1].u - 0.0625f / DrawTex->Tex->TexWidth ;
	vec[5].v = vec[1].v = vec[0].v = TexVert[0].v + 0.0625f / DrawTex->Tex->TexHeight ;
	vec[4].v = vec[3].v = vec[2].v = TexVert[2].v - 0.0625f / DrawTex->Tex->TexHeight ;

	*((DWORD *)&vec[0].pos.z) = drawz ;
	*((DWORD *)&vec[1].pos.z) = drawz ;
	*((DWORD *)&vec[2].pos.z) = drawz ;
	*((DWORD *)&vec[3].pos.z) = drawz ;
	*((DWORD *)&vec[4].pos.z) = drawz ;
	*((DWORD *)&vec[5].pos.z) = drawz ;

	if( Reverse0 <= 0  &&  Reverse1 <= 0 )
	{
		VertBuf[ 0 ] = vec[ 1 ] ;
		VertBuf[ 1 ] = vec[ 4 ] ;
		vec[ 1 ]     = vec[ 2 ] ;
		vec[ 4 ]     = vec[ 5 ] ;
		vec[ 2 ]     = VertBuf[ 0 ] ;
		vec[ 5 ]     = VertBuf[ 1 ] ;
	}

	ADD4VERTEX_TEX
}

// ハードウエアアクセラレータ使用版 DrawSimpleQuadrangleGraphF
extern	int		Graphics_D3D9_DrawSimpleQuadrangleGraphF( const GRAPHICS_DRAW_DRAWSIMPLEQUADRANGLEGRAPHF_PARAM *Param, IMAGEDATA *Image, IMAGEDATA *BlendImage )
{
	VERTEX_2D *DrawVert ;
	VERTEX_BLENDTEX_2D *DrawVertB ;
	DWORD DiffuseColor ;
	IMAGEDATA_HARD_DRAW *DrawTex ;
	IMAGEDATA_HARD_DRAW *BlendDrawTex = NULL ;
	IMAGEDATA_HARD_VERT *TexVert ;
	IMAGEDATA_HARD_VERT *BlendTexVert ;
	DWORD drawz ;
	int Flag ;
	float u[ 4 ] ;
	float v[ 4 ] ;
	int i ;
	const GRAPHICS_DRAW_DRAWSIMPLEANGLEGRAPHF_VERTEX *ParamV ;

	if( GAPIWin.Direct3DDevice9Object == NULL )
	{
		return -1 ;
	}

	// テクスチャの数が１以外の場合はエラー
	if( Image->Hard.DrawNum != 1 )
	{
		return -1 ;
	}

	// 描画の準備
	if( GD3D9.Device.DrawInfo.BeginSceneFlag == FALSE )
	{
		Graphics_D3D9_BeginScene() ;
	}
	Flag = Param->TransFlag ;
	DX_D3D9_DRAWPREP_TEX( Image->Orig, Image->Hard.Draw[ 0 ].Tex->PF->D3D9.Texture, Flag )

	// 頂点データを取得（今のところ色情報のセットのみ）
	DiffuseColor = GD3D9.Device.DrawInfo.DiffuseColor ;

	// 描画情報配列のアドレスをセットしておく
	DrawTex = Image->Hard.Draw ;
	if( BlendImage != NULL )
	{
		BlendDrawTex = BlendImage->Hard.Draw ;
	}

	// Ｚバッファに書き込むＺ値をセットする
	drawz = *((DWORD *)&GSYS.DrawSetting.DrawZ);

	// 頂点タイプによって処理を分岐
	switch( GD3D9.Device.DrawInfo.VertexType )
	{
	case VERTEXTYPE_BLENDTEX :	// ブレンドテクスチャを使用する
		// 描画するテクスチャーのセット
		Graphics_D3D9_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D9.Texture ) ;
		Graphics_D3D9_DrawSetting_SetBlendTexture( BlendDrawTex->Tex->PF->D3D9.Texture, BlendDrawTex->Tex->TexWidth, BlendDrawTex->Tex->TexHeight );

		if( GD3D9.Device.DrawSetting.ChangeTextureFlag )
		{
			Graphics_D3D9_DeviceState_RefreshBlendState() ;
		}

		TexVert      = DrawTex->Vertex ;
		BlendTexVert = BlendDrawTex->Vertex ;

		// 頂点データを初期化する
		ParamV = Param->Vertex ;
		for( i = 0 ; i < Param->QuadrangleNum ; i ++, ParamV += 4 )
		{
			GETVERTEX_QUAD( DrawVertB )

			DrawVertB[0].color = ( DWORD )DiffuseColor ;
			DrawVertB[1].color = ( DWORD )DiffuseColor ;
			DrawVertB[2].color = ( DWORD )DiffuseColor ;
			DrawVertB[3].color = ( DWORD )DiffuseColor ;
			DrawVertB[4].color = ( DWORD )DiffuseColor ;
			DrawVertB[5].color = ( DWORD )DiffuseColor ;

			DrawVertB[0].pos.x = ParamV[ 0 ].x ;
			DrawVertB[0].pos.y = ParamV[ 0 ].y ;
			DrawVertB[1].pos.x = ParamV[ 1 ].x ;
			DrawVertB[1].pos.y = ParamV[ 1 ].y ;
			DrawVertB[2].pos.x = ParamV[ 2 ].x ;
			DrawVertB[2].pos.y = ParamV[ 2 ].y ;
			DrawVertB[3].pos.x = ParamV[ 3 ].x ;
			DrawVertB[3].pos.y = ParamV[ 3 ].y ;
			DrawVertB[4].pos.x = ParamV[ 2 ].x ;
			DrawVertB[4].pos.y = ParamV[ 2 ].y ;
			DrawVertB[5].pos.x = ParamV[ 1 ].x ;
			DrawVertB[5].pos.y = ParamV[ 1 ].y ;

			switch( GSYS.DrawSetting.BlendGraphPosMode )
			{
			default :
			case DX_BLENDGRAPH_POSMODE_DRAWGRAPH:
				u[ 0 ] = ( TexVert[0].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
				v[ 0 ] = ( TexVert[0].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
				u[ 1 ] = ( TexVert[1].x - BlendTexVert[1].x + GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
				v[ 1 ] = ( TexVert[1].y - BlendTexVert[1].y + GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
				u[ 2 ] = ( TexVert[2].x - BlendTexVert[2].x + GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
				v[ 2 ] = ( TexVert[2].y - BlendTexVert[2].y + GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
				u[ 3 ] = ( TexVert[3].x - BlendTexVert[3].x + GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
				v[ 3 ] = ( TexVert[3].y - BlendTexVert[3].y + GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
				break ;

			case DX_BLENDGRAPH_POSMODE_SCREEN :
				u[ 0 ] = ( ParamV[0].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
				v[ 0 ] = ( ParamV[0].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
				u[ 1 ] = ( ParamV[1].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
				v[ 1 ] = ( ParamV[1].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
				u[ 2 ] = ( ParamV[2].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
				v[ 2 ] = ( ParamV[2].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
				u[ 3 ] = ( ParamV[3].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
				v[ 3 ] = ( ParamV[3].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
				break ;
			}

			DrawVertB[0].u1 = u[ 0 ] ;
			DrawVertB[0].v1 = v[ 0 ] ;
			DrawVertB[1].u1 = u[ 1 ] ;
			DrawVertB[1].v1 = v[ 1 ] ;
			DrawVertB[2].u1 = u[ 2 ] ;
			DrawVertB[2].v1 = v[ 2 ] ;
			DrawVertB[3].u1 = u[ 3 ] ;
			DrawVertB[3].v1 = v[ 3 ] ;
			DrawVertB[4].u1 = u[ 2 ] ;
			DrawVertB[4].v1 = v[ 2 ] ;
			DrawVertB[5].u1 = u[ 1 ] ;
			DrawVertB[5].v1 = v[ 1 ] ;

			DrawVertB[0].u2 = ParamV[ 0 ].u ;
			DrawVertB[0].v2 = ParamV[ 0 ].v ;
			DrawVertB[1].u2 = ParamV[ 1 ].u ;
			DrawVertB[1].v2 = ParamV[ 1 ].v ;
			DrawVertB[2].u2 = ParamV[ 2 ].u ;
			DrawVertB[2].v2 = ParamV[ 2 ].v ;
			DrawVertB[3].u2 = ParamV[ 3 ].u ;
			DrawVertB[3].v2 = ParamV[ 3 ].v ;
			DrawVertB[4].u2 = ParamV[ 2 ].u ;
			DrawVertB[4].v2 = ParamV[ 2 ].v ;
			DrawVertB[5].u2 = ParamV[ 1 ].u ;
			DrawVertB[5].v2 = ParamV[ 1 ].v ;

			*((DWORD *)&DrawVertB[0].pos.z) = drawz ;
			*((DWORD *)&DrawVertB[1].pos.z) = drawz ;
			*((DWORD *)&DrawVertB[2].pos.z) = drawz ;
			*((DWORD *)&DrawVertB[3].pos.z) = drawz ;
			*((DWORD *)&DrawVertB[4].pos.z) = drawz ;
			*((DWORD *)&DrawVertB[5].pos.z) = drawz ;

			DrawVertB[0].rhw = 1.0f ;
			DrawVertB[1].rhw = 1.0f ;
			DrawVertB[2].rhw = 1.0f ;
			DrawVertB[3].rhw = 1.0f ;
			DrawVertB[4].rhw = 1.0f ;
			DrawVertB[5].rhw = 1.0f ;

			// テクスチャーを描画する
			ADD4VERTEX_BLENDTEX
		}
		break ;

	case VERTEXTYPE_TEX :	// ブレンドテクスチャを使用しない
		TexVert = DrawTex->Vertex ;

		// 頂点データを初期化する
		ParamV = Param->Vertex ;
		for( i = 0 ; i < Param->QuadrangleNum ; i ++, ParamV += 4 )
		{
			GETVERTEX_QUAD( DrawVert )

			DrawVert[0].color = ( DWORD )DiffuseColor ;
			DrawVert[1].color = ( DWORD )DiffuseColor ;
			DrawVert[2].color = ( DWORD )DiffuseColor ;
			DrawVert[3].color = ( DWORD )DiffuseColor ;
			DrawVert[4].color = ( DWORD )DiffuseColor ;
			DrawVert[5].color = ( DWORD )DiffuseColor ;

			DrawVert[0].u = ParamV[ 0 ].u ;
			DrawVert[0].v = ParamV[ 0 ].v ;
			DrawVert[1].u = ParamV[ 1 ].u ;
			DrawVert[1].v = ParamV[ 1 ].v ;
			DrawVert[2].u = ParamV[ 2 ].u ;
			DrawVert[2].v = ParamV[ 2 ].v ;
			DrawVert[3].u = ParamV[ 3 ].u ;
			DrawVert[3].v = ParamV[ 3 ].v ;
			DrawVert[4].u = ParamV[ 2 ].u ;
			DrawVert[4].v = ParamV[ 2 ].v ;
			DrawVert[5].u = ParamV[ 1 ].u ;
			DrawVert[5].v = ParamV[ 1 ].v ;

			DrawVert[0].pos.x = ParamV[ 0 ].x ;
			DrawVert[0].pos.y = ParamV[ 0 ].y ;
			DrawVert[1].pos.x = ParamV[ 1 ].x ;
			DrawVert[1].pos.y = ParamV[ 1 ].y ;
			DrawVert[2].pos.x = ParamV[ 2 ].x ;
			DrawVert[2].pos.y = ParamV[ 2 ].y ;
			DrawVert[3].pos.x = ParamV[ 3 ].x ;
			DrawVert[3].pos.y = ParamV[ 3 ].y ;
			DrawVert[4].pos.x = ParamV[ 2 ].x ;
			DrawVert[4].pos.y = ParamV[ 2 ].y ;
			DrawVert[5].pos.x = ParamV[ 1 ].x ;
			DrawVert[5].pos.y = ParamV[ 1 ].y ;

			*((DWORD *)&DrawVert[0].pos.z) = drawz ;
			*((DWORD *)&DrawVert[1].pos.z) = drawz ;
			*((DWORD *)&DrawVert[2].pos.z) = drawz ;
			*((DWORD *)&DrawVert[3].pos.z) = drawz ;
			*((DWORD *)&DrawVert[4].pos.z) = drawz ;
			*((DWORD *)&DrawVert[5].pos.z) = drawz ;

			// テクスチャーを描画する
			ADD4VERTEX_TEX
		}
		break ;
	}

	// 終了
	return 0 ;
}

// ハードウエアアクセラレータ使用版 DrawSimpleTriangleGraphF
extern	int		Graphics_D3D9_DrawSimpleTriangleGraphF( const GRAPHICS_DRAW_DRAWSIMPLETRIANGLEGRAPHF_PARAM *Param, IMAGEDATA *Image, IMAGEDATA *BlendImage )
{
	VERTEX_2D *DrawVert ;
	VERTEX_BLENDTEX_2D *DrawVertB ;
	DWORD DiffuseColor ;
	IMAGEDATA_HARD_DRAW *DrawTex ;
	IMAGEDATA_HARD_DRAW *BlendDrawTex = NULL ;
	IMAGEDATA_HARD_VERT *TexVert ;
	IMAGEDATA_HARD_VERT *BlendTexVert ;
	DWORD drawz ;
	int Flag ;
	float u[ 3 ] ;
	float v[ 3 ] ;
	int i ;
	const GRAPHICS_DRAW_DRAWSIMPLEANGLEGRAPHF_VERTEX *ParamV ;

	if( GAPIWin.Direct3DDevice9Object == NULL )
	{
		return -1 ;
	}

	// テクスチャの数が１以外の場合はエラー
	if( Image->Hard.DrawNum != 1 )
	{
		return -1 ;
	}

	// 描画の準備
	if( GD3D9.Device.DrawInfo.BeginSceneFlag == FALSE )
	{
		Graphics_D3D9_BeginScene() ;
	}
	Flag = Param->TransFlag ;
	DX_D3D9_DRAWPREP_TEX( Image->Orig, Image->Hard.Draw[ 0 ].Tex->PF->D3D9.Texture, Flag )

	// 頂点データを取得（今のところ色情報のセットのみ）
	DiffuseColor = GD3D9.Device.DrawInfo.DiffuseColor ;

	// 描画情報配列のアドレスをセットしておく
	DrawTex = Image->Hard.Draw ;
	if( BlendImage != NULL )
	{
		BlendDrawTex = BlendImage->Hard.Draw ;
	}

	// Ｚバッファに書き込むＺ値をセットする
	drawz = *((DWORD *)&GSYS.DrawSetting.DrawZ);

	// 頂点タイプによって処理を分岐
	switch( GD3D9.Device.DrawInfo.VertexType )
	{
	case VERTEXTYPE_BLENDTEX :	// ブレンドテクスチャを使用する
		// 描画するテクスチャーのセット
		Graphics_D3D9_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D9.Texture ) ;
		Graphics_D3D9_DrawSetting_SetBlendTexture( BlendDrawTex->Tex->PF->D3D9.Texture, BlendDrawTex->Tex->TexWidth, BlendDrawTex->Tex->TexHeight );

		if( GD3D9.Device.DrawSetting.ChangeTextureFlag )
		{
			Graphics_D3D9_DeviceState_RefreshBlendState() ;
		}

		TexVert      = DrawTex->Vertex ;
		BlendTexVert = BlendDrawTex->Vertex ;

		// 頂点データを初期化する
		ParamV = Param->Vertex ;
		for( i = 0 ; i < Param->TriangleNum ; i ++, ParamV += 3 )
		{
			GETVERTEX_TRIANGLE( DrawVertB )

			DrawVertB[0].color = ( DWORD )DiffuseColor ;
			DrawVertB[1].color = ( DWORD )DiffuseColor ;
			DrawVertB[2].color = ( DWORD )DiffuseColor ;

			DrawVertB[0].pos.x = ParamV[ 0 ].x ;
			DrawVertB[0].pos.y = ParamV[ 0 ].y ;
			DrawVertB[1].pos.x = ParamV[ 1 ].x ;
			DrawVertB[1].pos.y = ParamV[ 1 ].y ;
			DrawVertB[2].pos.x = ParamV[ 2 ].x ;
			DrawVertB[2].pos.y = ParamV[ 2 ].y ;

			switch( GSYS.DrawSetting.BlendGraphPosMode )
			{
			default :
			case DX_BLENDGRAPH_POSMODE_DRAWGRAPH:
				u[ 0 ] = ( TexVert[0].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
				v[ 0 ] = ( TexVert[0].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
				u[ 1 ] = ( TexVert[1].x - BlendTexVert[1].x + GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
				v[ 1 ] = ( TexVert[1].y - BlendTexVert[1].y + GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
				u[ 2 ] = ( TexVert[2].x - BlendTexVert[2].x + GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
				v[ 2 ] = ( TexVert[2].y - BlendTexVert[2].y + GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
				break ;

			case DX_BLENDGRAPH_POSMODE_SCREEN :
				u[ 0 ] = ( ParamV[0].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
				v[ 0 ] = ( ParamV[0].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
				u[ 1 ] = ( ParamV[1].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
				v[ 1 ] = ( ParamV[1].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
				u[ 2 ] = ( ParamV[2].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D9.Device.DrawSetting.InvBlendTextureWidth ;
				v[ 2 ] = ( ParamV[2].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D9.Device.DrawSetting.InvBlendTextureHeight ;
				break ;
			}

			DrawVertB[0].u1 = u[ 0 ] ;
			DrawVertB[0].v1 = v[ 0 ] ;
			DrawVertB[1].u1 = u[ 1 ] ;
			DrawVertB[1].v1 = v[ 1 ] ;
			DrawVertB[2].u1 = u[ 2 ] ;
			DrawVertB[2].v1 = v[ 2 ] ;

			DrawVertB[0].u2 = ParamV[ 0 ].u ;
			DrawVertB[0].v2 = ParamV[ 0 ].v ;
			DrawVertB[1].u2 = ParamV[ 1 ].u ;
			DrawVertB[1].v2 = ParamV[ 1 ].v ;
			DrawVertB[2].u2 = ParamV[ 2 ].u ;
			DrawVertB[2].v2 = ParamV[ 2 ].v ;

			*((DWORD *)&DrawVertB[0].pos.z) = drawz ;
			*((DWORD *)&DrawVertB[1].pos.z) = drawz ;
			*((DWORD *)&DrawVertB[2].pos.z) = drawz ;

			DrawVertB[0].rhw = 1.0f ;
			DrawVertB[1].rhw = 1.0f ;
			DrawVertB[2].rhw = 1.0f ;

			// テクスチャーを描画する
			ADD3VERTEX_BLENDTEX
		}
		break ;

	case VERTEXTYPE_TEX :	// ブレンドテクスチャを使用しない
		TexVert = DrawTex->Vertex ;

		// 頂点データを初期化する
		ParamV = Param->Vertex ;
		for( i = 0 ; i < Param->TriangleNum ; i ++, ParamV += 3 )
		{
			GETVERTEX_TRIANGLE( DrawVert )

			DrawVert[0].color = ( DWORD )DiffuseColor ;
			DrawVert[1].color = ( DWORD )DiffuseColor ;
			DrawVert[2].color = ( DWORD )DiffuseColor ;

			DrawVert[0].u = ParamV[ 0 ].u ;
			DrawVert[0].v = ParamV[ 0 ].v ;
			DrawVert[1].u = ParamV[ 1 ].u ;
			DrawVert[1].v = ParamV[ 1 ].v ;
			DrawVert[2].u = ParamV[ 2 ].u ;
			DrawVert[2].v = ParamV[ 2 ].v ;

			DrawVert[0].pos.x = ParamV[ 0 ].x ;
			DrawVert[0].pos.y = ParamV[ 0 ].y ;
			DrawVert[1].pos.x = ParamV[ 1 ].x ;
			DrawVert[1].pos.y = ParamV[ 1 ].y ;
			DrawVert[2].pos.x = ParamV[ 2 ].x ;
			DrawVert[2].pos.y = ParamV[ 2 ].y ;

			*((DWORD *)&DrawVert[0].pos.z) = drawz ;
			*((DWORD *)&DrawVert[1].pos.z) = drawz ;
			*((DWORD *)&DrawVert[2].pos.z) = drawz ;

			// テクスチャーを描画する
			ADD3VERTEX_TEX
		}
		break ;
	}

	// 終了
	return 0 ;
}

// ハードウエアアクセラレータ使用版 DrawFillBox
extern	int		Graphics_D3D9_DrawFillBox( int x1, int y1, int x2, int y2, unsigned int Color )
{
	VERTEX_NOTEX_2D *vec ;
	RECT drect ;
	int Red, Green, Blue, Flag ;
	DWORD drawz ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	// 反転処理
	{
		int b ;

		if( x1 > x2 ){ b = x1 ; x1 = x2 ; x2 = b ; }
		if( y1 > y2 ){ b = y1 ; y1 = y2 ; y2 = b ; }
	}

	// 座標のセット
	drect.left   = x1 ;
	drect.right  = x2 ;
	drect.top    = y1 ;
	drect.bottom = y2 ;
	
	// クリッピング処理
	if( GSYS.DrawSetting.Valid2DMatrix == FALSE )
	{
		RectClipping_Inline( &drect, &GSYS.DrawSetting.DrawArea ) ;
		if( drect.left == drect.right || drect.top == drect.bottom ) return 0 ;
	}

	// 描画の準備
	Graphics_D3D9_BeginScene() ;
	Flag = DX_D3D9_DRAWPREP_DIFFUSERGB ;
	DX_D3D9_DRAWPREP_NOTEX( Flag ) ;

	// 色その他ステータスのセット
	NS_GetColor2( Color, &Red, &Green, &Blue ) ;
	SETUPCOLOR( Color )

	// Ｚバッファに書き込むＺ値をセットする
	drawz = *((DWORD *)&GSYS.DrawSetting.DrawZ);

	// 頂点データのセット
	GETVERTEX_QUAD( vec )
	
	vec[0].color =
	vec[3].color = Color ;
	
	vec[4].pos.x = vec[2].pos.x = vec[0].pos.x = (float)drect.left   ;
	vec[5].pos.x = vec[3].pos.x = vec[1].pos.x = (float)drect.right  ;
	vec[5].pos.y = vec[1].pos.y = vec[0].pos.y = (float)drect.top    ;
	vec[4].pos.y = vec[3].pos.y = vec[2].pos.y = (float)drect.bottom ;

	*((DWORD *)&vec[0].pos.z) = drawz ;
	*((DWORD *)&vec[1].pos.z) = drawz ;
	*((DWORD *)&vec[2].pos.z) = drawz ;
	*((DWORD *)&vec[3].pos.z) = drawz ;
	*((DWORD *)&vec[4].pos.z) = drawz ;
	*((DWORD *)&vec[5].pos.z) = drawz ;

	// 頂点データの出力
	ADD4VERTEX_NOTEX

	// 終了
	return 0 ;
}

// ハードウエアアクセラレータ使用版 DrawLineBox
extern	int		Graphics_D3D9_DrawLineBox( int x1, int y1, int x2, int y2, unsigned int Color )
{
	VERTEX_NOTEX_2D *VertData ;
	int Red, Green, Blue ;
	float fx1, fx2, fy1, fy2 ;
	int Flag;
	DWORD drawz ;

//	Graphics_D3D9_RenderVertex( D3D.Direct3DDeviceHandle ) ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	// 描画の準備
	if( GD3D9.Device.DrawInfo.BeginSceneFlag == FALSE ) Graphics_D3D9_BeginScene() ;
	if( GD3D9.Device.DrawSetting.RenderTexture != NULL ) Graphics_D3D9_DrawSetting_SetTexture( NULL ) ;
	Flag = DX_D3D9_DRAWPREP_DIFFUSERGB ;
	DX_D3D9_DRAWPREP_NOTEX( Flag ) ;

	// Ｚバッファに書き込むＺ値をセットする
	drawz = *((DWORD *)&GSYS.DrawSetting.DrawZ);

	// 色その他ステータスのセット
	NS_GetColor2( Color, &Red, &Green, &Blue ) ;
	SETUPCOLOR( Color )

	// 反転処理
	{
		int bx, by ;

		if( x1 > x2 || y1 > y2 )
		{
			bx = x1 ; by = y1 ;
			x1 = x2 ; y1 = y2 ;
			x2 = bx ; y2 = by ;
		}
	}

	fx1 = ( float )x1 ;
	fy1 = ( float )y1 ;
	fx2 = ( float )x2 ;
	fy2 = ( float )y2 ;

	GETVERTEX_LINEBOX( VertData ) ;

	VertData[ 0].color = Color ;
	VertData[ 1].color = Color ;
	VertData[ 2].color = Color ;
	VertData[ 3].color = Color ;
	VertData[ 4].color = Color ;
	VertData[ 5].color = Color ;
	VertData[ 6].color = Color ;
	VertData[ 7].color = Color ;
	VertData[ 8].color = Color ;
	VertData[ 9].color = Color ;
	VertData[10].color = Color ;
	VertData[11].color = Color ;
	VertData[12].color = Color ;
	VertData[13].color = Color ;
	VertData[14].color = Color ;
	VertData[15].color = Color ;
	VertData[16].color = Color ;
	VertData[17].color = Color ;
	VertData[18].color = Color ;
	VertData[19].color = Color ;
	VertData[20].color = Color ;
	VertData[21].color = Color ;
	VertData[22].color = Color ;
	VertData[23].color = Color ;

	*((DWORD *)&VertData[ 0].pos.z) = drawz ;
	*((DWORD *)&VertData[ 1].pos.z) = drawz ;
	*((DWORD *)&VertData[ 2].pos.z) = drawz ;
	*((DWORD *)&VertData[ 3].pos.z) = drawz ;
	*((DWORD *)&VertData[ 4].pos.z) = drawz ;
	*((DWORD *)&VertData[ 5].pos.z) = drawz ;
	*((DWORD *)&VertData[ 6].pos.z) = drawz ;
	*((DWORD *)&VertData[ 7].pos.z) = drawz ;
	*((DWORD *)&VertData[ 8].pos.z) = drawz ;
	*((DWORD *)&VertData[ 9].pos.z) = drawz ;
	*((DWORD *)&VertData[10].pos.z) = drawz ;
	*((DWORD *)&VertData[11].pos.z) = drawz ;
	*((DWORD *)&VertData[12].pos.z) = drawz ;
	*((DWORD *)&VertData[13].pos.z) = drawz ;
	*((DWORD *)&VertData[14].pos.z) = drawz ;
	*((DWORD *)&VertData[15].pos.z) = drawz ;
	*((DWORD *)&VertData[16].pos.z) = drawz ;
	*((DWORD *)&VertData[17].pos.z) = drawz ;
	*((DWORD *)&VertData[18].pos.z) = drawz ;
	*((DWORD *)&VertData[19].pos.z) = drawz ;
	*((DWORD *)&VertData[20].pos.z) = drawz ;
	*((DWORD *)&VertData[21].pos.z) = drawz ;
	*((DWORD *)&VertData[22].pos.z) = drawz ;
	*((DWORD *)&VertData[23].pos.z) = drawz ;

	VertData[0].pos.x = fx1			; VertData[0].pos.y = fy1		 ;
	VertData[1].pos.x = fx2			; VertData[1].pos.y = fy1		 ;
	VertData[2].pos.x = fx1			; VertData[2].pos.y = fy1 + 1.0f ;

	VertData[3].pos.x = fx2			; VertData[3].pos.y = fy1 + 1.0f ;
	VertData[4].pos.x = fx1			; VertData[4].pos.y = fy1 + 1.0f ;
	VertData[5].pos.x = fx2			; VertData[5].pos.y = fy1		 ;

	VertData += 6 ;


	VertData[0].pos.x = fx2 - 1.0f	; VertData[0].pos.y = fy1 + 1.0f ;
	VertData[1].pos.x = fx2			; VertData[1].pos.y = fy1 + 1.0f ;
	VertData[2].pos.x = fx2	- 1.0f	; VertData[2].pos.y = fy2		 ;

	VertData[3].pos.x = fx2			; VertData[3].pos.y = fy2		 ;
	VertData[4].pos.x = fx2 - 1.0f	; VertData[4].pos.y = fy2		 ;
	VertData[5].pos.x = fx2			; VertData[5].pos.y = fy1 + 1.0f ;

	VertData += 6 ;


	VertData[0].pos.x = fx1			; VertData[0].pos.y = fy1 + 1.0f ;
	VertData[1].pos.x = fx1 + 1.0f	; VertData[1].pos.y = fy1 + 1.0f ;
	VertData[2].pos.x = fx1			; VertData[2].pos.y = fy2		 ;

	VertData[3].pos.x = fx1 + 1.0f	; VertData[3].pos.y = fy2		 ;
	VertData[4].pos.x = fx1			; VertData[4].pos.y = fy2		 ;
	VertData[5].pos.x = fx1 + 1.0f	; VertData[5].pos.y = fy1 + 1.0f ;

	VertData += 6 ;


	VertData[0].pos.x = fx1 + 1.0f	; VertData[0].pos.y = fy2 - 1.0f ;
	VertData[1].pos.x = fx2 - 1.0f	; VertData[1].pos.y = fy2 - 1.0f ;
	VertData[2].pos.x = fx1 + 1.0f	; VertData[2].pos.y = fy2		 ;

	VertData[3].pos.x = fx2 - 1.0f	; VertData[3].pos.y = fy2		 ;
	VertData[4].pos.x = fx1 + 1.0f	; VertData[4].pos.y = fy2		 ;
	VertData[5].pos.x = fx2 - 1.0f	; VertData[5].pos.y = fy2 - 1.0f ;

	VertData += 6 ;

	// 頂点の追加
	ADD4VERTEX_LINEBOX

	// 終了
	return 0 ;
}



// ハードウエアアクセラレータ使用版 DrawLine
extern	int		Graphics_D3D9_DrawLine( int x1, int y1, int x2, int y2, unsigned int Color )
{
	VERTEX_NOTEX_2D *VertData ;
	int Red, Green, Blue, Flag ;
	DWORD drawz ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ; 

	if( x2 - x1 == 0 && y2 - y1 == 0 ) return 0 ; 

	// 描画の準備
	if( GD3D9.Device.DrawInfo.BeginSceneFlag == FALSE ) Graphics_D3D9_BeginScene() ;
	if( GD3D9.Device.DrawSetting.RenderTexture != NULL ) Graphics_D3D9_DrawSetting_SetTexture( NULL ) ;
	Flag = DX_D3D9_DRAWPREP_DIFFUSERGB ;
	DX_D3D9_DRAWPREP_NOTEX( Flag )

	// Ｚバッファに書き込むＺ値をセットする
	drawz = *((DWORD *)&GSYS.DrawSetting.DrawZ);

	// 色をセット
	GETVERTEX_LINE( VertData ) ;
	NS_GetColor2( Color, &Red, &Green, &Blue ) ;
	SETUPCOLOR( Color )
	VertData[ 0 ].color = Color ;
	VertData[ 1 ].color = Color ;
	*((DWORD *)&VertData[ 0 ].pos.z) = drawz ;
	*((DWORD *)&VertData[ 1 ].pos.z) = drawz ;

	// 座標をセット
	VertData[ 0 ].pos.x = ( float )( x1 ) ;
	VertData[ 0 ].pos.y = ( float )( y1 ) ;

	VertData[ 1 ].pos.x = ( float )( x2 ) ;
	VertData[ 1 ].pos.y = ( float )( y2 ) ;

	// 座標の補正
	if( x1 == x2 ) VertData[ 1 ].pos.y += y2 > y1 ? -0.1F : 0.1F ;
	if( y1 == y2 ) VertData[ 1 ].pos.x += x2 > x1 ? -0.1F : 0.1F ;

	// 頂点の追加
	ADD4VERTEX_LINE

	// 終了
	return 0 ;
}

// ハードウエアアクセラレータ使用版 DrawLine3D
extern	int		Graphics_D3D9_DrawLine3D( VECTOR Pos1, VECTOR Pos2, unsigned int Color, int DrawFlag, RECT *DrawArea )
{
	VERTEX_NOTEX_3D *VertData ;
	int Red, Green, Blue, Flag ;

	if( DrawFlag )
	{
		if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ; 

		// 描画の準備
		if( GD3D9.Device.DrawInfo.BeginSceneFlag == FALSE ) Graphics_D3D9_BeginScene() ;
		if( GD3D9.Device.DrawSetting.RenderTexture != NULL ) Graphics_D3D9_DrawSetting_SetTexture( NULL ) ;

		Flag = DX_D3D9_DRAWPREP_DIFFUSERGB | DX_D3D9_DRAWPREP_3D | DX_D3D9_DRAWPREP_FOG ;
		DX_D3D9_DRAWPREP_NOTEX( Flag )

		GETVERTEX_LINE3D( VertData ) ;

		// 色をセット
		NS_GetColor2( Color, &Red, &Green, &Blue ) ;
		SETUPCOLOR( Color )
		*( ( DWORD * )&VertData[ 0 ].b ) = Color ;
		*( ( DWORD * )&VertData[ 1 ].b ) = Color ;

		// 座標をセット
		VertData[ 0 ].pos = Pos1 ;
		VertData[ 1 ].pos = Pos2 ;

		// 頂点の追加
		ADD4VERTEX_LINE3D
	}
	else
	{
		VERTEX_2D TempVert[ 2 ] ;
		float dleft, dright, dtop, dbottom ;

		dleft   = -100000000.0f ;
		dright  =  100000000.0f ;
		dtop    = -100000000.0f ;
		dbottom =  100000000.0f ;

		// スクリーン座標に変換
		TempVert[0].pos.x = Pos1.x * GSYS.DrawSetting.Blend3DMatrixF.m[0][0] + Pos1.y * GSYS.DrawSetting.Blend3DMatrixF.m[1][0] + Pos1.z * GSYS.DrawSetting.Blend3DMatrixF.m[2][0] + GSYS.DrawSetting.Blend3DMatrixF.m[3][0]  ;
		TempVert[0].pos.y = Pos1.x * GSYS.DrawSetting.Blend3DMatrixF.m[0][1] + Pos1.y * GSYS.DrawSetting.Blend3DMatrixF.m[1][1] + Pos1.z * GSYS.DrawSetting.Blend3DMatrixF.m[2][1] + GSYS.DrawSetting.Blend3DMatrixF.m[3][1]  ;
		TempVert[0].pos.z = Pos1.x * GSYS.DrawSetting.Blend3DMatrixF.m[0][2] + Pos1.y * GSYS.DrawSetting.Blend3DMatrixF.m[1][2] + Pos1.z * GSYS.DrawSetting.Blend3DMatrixF.m[2][2] + GSYS.DrawSetting.Blend3DMatrixF.m[3][2]  ;
		TempVert[0].rhw   = Pos1.x * GSYS.DrawSetting.Blend3DMatrixF.m[0][3] + Pos1.y * GSYS.DrawSetting.Blend3DMatrixF.m[1][3] + Pos1.z * GSYS.DrawSetting.Blend3DMatrixF.m[2][3] + GSYS.DrawSetting.Blend3DMatrixF.m[3][3]  ;

		TempVert[1].pos.x = Pos2.x * GSYS.DrawSetting.Blend3DMatrixF.m[0][0] + Pos2.y * GSYS.DrawSetting.Blend3DMatrixF.m[1][0] + Pos2.z * GSYS.DrawSetting.Blend3DMatrixF.m[2][0] + GSYS.DrawSetting.Blend3DMatrixF.m[3][0]  ;
		TempVert[1].pos.y = Pos2.x * GSYS.DrawSetting.Blend3DMatrixF.m[0][1] + Pos2.y * GSYS.DrawSetting.Blend3DMatrixF.m[1][1] + Pos2.z * GSYS.DrawSetting.Blend3DMatrixF.m[2][1] + GSYS.DrawSetting.Blend3DMatrixF.m[3][1]  ;
		TempVert[1].pos.z = Pos2.x * GSYS.DrawSetting.Blend3DMatrixF.m[0][2] + Pos2.y * GSYS.DrawSetting.Blend3DMatrixF.m[1][2] + Pos2.z * GSYS.DrawSetting.Blend3DMatrixF.m[2][2] + GSYS.DrawSetting.Blend3DMatrixF.m[3][2]  ;
		TempVert[1].rhw   = Pos2.x * GSYS.DrawSetting.Blend3DMatrixF.m[0][3] + Pos2.y * GSYS.DrawSetting.Blend3DMatrixF.m[1][3] + Pos2.z * GSYS.DrawSetting.Blend3DMatrixF.m[2][3] + GSYS.DrawSetting.Blend3DMatrixF.m[3][3]  ;

		TempVert[0].rhw = 1.0F / TempVert[0].rhw ;
		TempVert[0].pos.x *= TempVert[0].rhw ;
		TempVert[0].pos.y *= TempVert[0].rhw ;
		TempVert[0].pos.z *= TempVert[0].rhw ;

		TempVert[1].rhw = 1.0F / TempVert[1].rhw ;
		TempVert[1].pos.x *= TempVert[1].rhw ;
		TempVert[1].pos.y *= TempVert[1].rhw ;
		TempVert[1].pos.z *= TempVert[1].rhw ;

		if( dright  > TempVert[ 0 ].pos.x ) dright  = TempVert[ 0 ].pos.x ;
		if( dright  > TempVert[ 1 ].pos.x ) dright  = TempVert[ 1 ].pos.x ;

		if( dleft   < TempVert[ 0 ].pos.x ) dleft   = TempVert[ 0 ].pos.x ;
		if( dleft   < TempVert[ 1 ].pos.x ) dleft   = TempVert[ 1 ].pos.x ;

		if( dbottom > TempVert[ 0 ].pos.y ) dbottom = TempVert[ 0 ].pos.y ;
		if( dbottom > TempVert[ 1 ].pos.y ) dbottom = TempVert[ 1 ].pos.y ;

		if( dtop    < TempVert[ 0 ].pos.y ) dtop    = TempVert[ 0 ].pos.y ;
		if( dtop    < TempVert[ 1 ].pos.y ) dtop    = TempVert[ 1 ].pos.y ;

		DrawArea->left   = _FTOL( dleft   ) - 1 ;
		DrawArea->right  = _FTOL( dright  ) + 1 ;
		DrawArea->top    = _FTOL( dtop    ) - 1 ;
		DrawArea->bottom = _FTOL( dbottom ) + 1 ;
	}

	// 終了
	return 0 ;
}



#define CIRCLE_VERTEX_NUM			(2000)

#define DX_3D_CIRCLE_THICKNESS												\
	if( x1 < DrawRect.left      ) x1 = DrawRect.left ;						\
	if( x2 > DrawRect.right - 1 ) x2 = DrawRect.right - 1 ;					\
																			\
	VertBuf[ VertNum ].pos.x  = ( float )x1 ;								\
	VertBuf[ VertNum ].pos.y  = y1f ;										\
	VertBuf[ VertNum ].color = Color ;										\
	VertBuf[ VertNum ].rhw   = 1.0f ;										\
	*((DWORD *)&VertBuf[ VertNum ].pos.z) = drawz ;							\
	VertNum ++ ;															\
																			\
	VertBuf[ VertNum ].pos.x =  ( float )( x2 + 1 ) ;						\
	VertBuf[ VertNum ].pos.y =  y1f ;										\
	VertBuf[ VertNum ].color = Color ;										\
	VertBuf[ VertNum ].rhw   = 1.0f ;										\
	*((DWORD *)&VertBuf[ VertNum ].pos.z) = drawz ;							\
	VertNum  ++ ;															\
																			\
	if( VertNum >= CIRCLE_VERTEX_NUM )										\
	{																		\
		Direct3DDevice9_DrawPrimitiveUP( D_D3DPT_LINELIST, ( UINT )( VertNum / 2 ), VertBuf, sizeof( VERTEX_NOTEX_2D ) ) ;\
		GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;						\
		VertNum = 0 ;														\
	}																		

// ハードウエアアクセラレータ使用版 DrawCircle( 太さ指定あり )
extern	int		Graphics_D3D9_DrawCircle_Thickness( int x, int y, int r, unsigned int Color, int Thickness )
{
	VERTEX_NOTEX_2D *VertBuf ;
	DWORD VertNum ;
	short ( *CirclePos )[ 5 ] ;
	int Red, Green, Blue, Flag ;
	RECT DrawRect ;
	DWORD drawz ;
	int i ;
//	int j ;
	int x1, x2 ;
	float y1f ;

	// 太さが２以下だったら何もせず終了
	if( Thickness < 2 )
	{
		return -1 ;
	}

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ; 

	// 描画用メモリの確保
	if( Graphics_Other_AllocCommonBuffer( 0, sizeof( VERTEX_NOTEX_2D ) * CIRCLE_VERTEX_NUM ) < 0 )
	{
		return -1 ;
	}
	VertBuf = ( VERTEX_NOTEX_2D * )GSYS.Resource.CommonBuffer[ 0 ] ;

	// 描画の準備
	Graphics_D3D9_BeginScene() ;
	Flag = DX_D3D9_DRAWPREP_DIFFUSERGB ;
	DX_D3D9_DRAWPREP_NOTEX( Flag )

	// Ｚバッファに書き込むＺ値をセットする
	drawz = *((DWORD *)&GSYS.DrawSetting.DrawZ);

	// 補正
	DrawRect = GSYS.DrawSetting.DrawArea ;

	NS_GetColor2( Color, &Red, &Green, &Blue ) ;
	SETUPCOLOR( Color )

	// FVF の変更
//	Graphics_D3D9_DeviceState_SetFVF( VERTEXFVF_NOTEX_2D ) ;
	Graphics_Hardware_D3D9_SetFVF_UseTable( VERTEXFVF_DECL_NOTEX_2D ) ;

	// 円の座標を代入するバッファを取得
	if( Graphics_Other_AllocCommonBuffer( 1, sizeof( short ) * 5 * DrawRect.bottom ) < 0 )
	{
		return -1 ;
	}
	CirclePos = ( short (*)[ 5 ] )GSYS.Resource.CommonBuffer[ 1 ] ;
	_MEMSET( CirclePos, 0, sizeof( short ) * 5 * DrawRect.bottom ) ;

	// 円の座標リスト作成
	Graphics_Draw_GetCircle_ThicknessDrawPosition( x, y, r, Thickness, CirclePos ) ;

	// 描画処理
	VertNum = 0 ;
	for( i = DrawRect.top ; i < DrawRect.bottom ; i ++ )
	{
		if( CirclePos[ i ][ 4 ] == 0 )
		{
			continue ;
		}

		y1f = ( float )i ;

		if( ( CirclePos[ i ][ 4 ] & ( 4 | 8 ) ) == 0 || CirclePos[ i ][ 2 ] >= CirclePos[ i ][ 3 ] )
		{
			x1 = CirclePos[ i ][ 0 ] ;
			x2 = CirclePos[ i ][ 1 ] ;
			DX_3D_CIRCLE_THICKNESS
		}
		else
		{
			x1 = CirclePos[ i ][ 0 ] ;
			x2 = CirclePos[ i ][ 2 ] ;
			DX_3D_CIRCLE_THICKNESS

			x1 = CirclePos[ i ][ 3 ] ;
			x2 = CirclePos[ i ][ 1 ] ;
			DX_3D_CIRCLE_THICKNESS
		}
	}
	if( VertNum )
	{
		Direct3DDevice9_DrawPrimitiveUP( D_D3DPT_LINELIST, VertNum / 2, VertBuf, sizeof( VERTEX_NOTEX_2D ) ) ;
		GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;
	}

	// 終了
	return 0 ;
}

// ハードウエアアクセラレータ使用版 DrawOval( 太さ指定あり )
extern	int		Graphics_D3D9_DrawOval_Thickness( int x, int y, int rx, int ry, unsigned int Color, int Thickness )
{
	VERTEX_NOTEX_2D *VertBuf ;
	int VertNum ;
	short ( *CirclePos )[ 5 ] ;
	int Red, Green, Blue, Flag ;
	RECT DrawRect ;
	DWORD drawz ;
	int i ;
//	int j ;
	int x1, x2 ;
	float y1f ;

	// 太さが２以下だったら何もせず終了
	if( Thickness < 2 )
	{
		return -1 ;
	}

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ; 

	// 描画用メモリの確保
	if( Graphics_Other_AllocCommonBuffer( 0, sizeof( VERTEX_NOTEX_2D ) * CIRCLE_VERTEX_NUM ) < 0 )
	{
		return -1 ;
	}
	VertBuf = ( VERTEX_NOTEX_2D * )GSYS.Resource.CommonBuffer[ 0 ] ;

	// 描画の準備
	Graphics_D3D9_BeginScene() ;
	Flag = DX_D3D9_DRAWPREP_DIFFUSERGB ;
	DX_D3D9_DRAWPREP_NOTEX( Flag )

	// Ｚバッファに書き込むＺ値をセットする
	drawz = *((DWORD *)&GSYS.DrawSetting.DrawZ);

	// 補正
	DrawRect = GSYS.DrawSetting.DrawArea ;

	NS_GetColor2( Color, &Red, &Green, &Blue ) ;
	SETUPCOLOR( Color )

	// FVF の変更
//	Graphics_D3D9_DeviceState_SetFVF( VERTEXFVF_NOTEX_2D ) ;
	Graphics_Hardware_D3D9_SetFVF_UseTable( VERTEXFVF_DECL_NOTEX_2D ) ;

	// 円の座標を代入するバッファを取得
	if( Graphics_Other_AllocCommonBuffer( 1, sizeof( short ) * 5 * DrawRect.bottom ) < 0 )
	{
		return -1 ;
	}
	CirclePos = ( short (*)[ 5 ] )GSYS.Resource.CommonBuffer[ 1 ] ;
	_MEMSET( CirclePos, 0, sizeof( short ) * 5 * DrawRect.bottom ) ;

	// 楕円の座標リスト作成
	if( Graphics_Draw_GetOval_ThicknessDrawPosition( x, y, rx, ry, Thickness, CirclePos ) < 0 )
	{
		return -1 ;
	}

	// 描画処理
	VertNum = 0 ;
	for( i = DrawRect.top ; i < DrawRect.bottom ; i ++ )
	{
		if( CirclePos[ i ][ 4 ] == 0 )
		{
			continue ;
		}

		y1f = ( float )i ;

		if( ( CirclePos[ i ][ 4 ] & ( 4 | 8 ) ) == 0 || CirclePos[ i ][ 2 ] >= CirclePos[ i ][ 3 ] )
		{
			x1 = CirclePos[ i ][ 0 ] ;
			x2 = CirclePos[ i ][ 1 ] ;
			DX_3D_CIRCLE_THICKNESS
		}
		else
		{
			x1 = CirclePos[ i ][ 0 ] ;
			x2 = CirclePos[ i ][ 2 ] ;
			DX_3D_CIRCLE_THICKNESS

			x1 = CirclePos[ i ][ 3 ] ;
			x2 = CirclePos[ i ][ 1 ] ;
			DX_3D_CIRCLE_THICKNESS
		}
	}
	if( VertNum )
	{
		Direct3DDevice9_DrawPrimitiveUP( D_D3DPT_LINELIST, ( UINT )( VertNum / 2 ), VertBuf, sizeof( VERTEX_NOTEX_2D ) ) ;
		GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;
	}

	// 終了
	return 0 ;
}




#define DX_3D_CIRCLE																						\
			if( !( ( x2 < DrawRect.left ) || ( x1 >= DrawRect.right  ) ||									\
			       ( y1 < DrawRect.top  ) || ( y1 >= DrawRect.bottom ) || LineDrawBuf[ y1 ] ) )				\
			{																								\
				if( x1 < DrawRect.left ) x1 = DrawRect.left ;												\
				if( x2 > DrawRect.right - 1 ) x2 = DrawRect.right - 1 ;										\
																											\
				VertBuf[ VertNum ].pos.x  = ( float )x1 + GD3D9.Device.Caps.DrawFillCircleLeftVertAddX ;	\
				VertBuf[ VertNum ].pos.y  = ( float )y1 ;													\
				VertBuf[ VertNum ].color = Color ;															\
				VertBuf[ VertNum ].rhw   = 1.0f ;															\
				*((DWORD *)&VertBuf[ VertNum ].pos.z) = drawz ;												\
				VertNum ++ ;																				\
				LineDrawBuf[y1] = TRUE ;																	\
																											\
				VertBuf[ VertNum ].pos.x =  ( float )( x2 + 1 ) + GD3D9.Device.Caps.DrawFillCircleRightVertAddX ;	\
				VertBuf[ VertNum ].pos.y =  ( float )y1 ;													\
				VertBuf[ VertNum ].color = Color ;															\
				VertBuf[ VertNum ].rhw   = 1.0f ;															\
				*((DWORD *)&VertBuf[ VertNum ].pos.z) = drawz ;												\
				VertNum  ++ ;																				\
																											\
				if( VertNum >= CIRCLE_VERTEX_NUM )															\
				{																							\
					Direct3DDevice9_DrawPrimitiveUP( D_D3DPT_LINELIST, ( UINT )( VertNum / 2 ), VertBuf, sizeof( VERTEX_NOTEX_2D ) ) ;\
					GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;											\
					VertNum = 0 ;																			\
				}																							\
			}

#define DX_3D_CIRCLE_PSET																\
			if( !( ( x1 < DrawRect.left ) || ( x1 >= DrawRect.right  ) ||				\
				   ( y1 < DrawRect.top  ) || ( y1 >= DrawRect.bottom ) ) )				\
			{																			\
				VertBuf[ VertNum ].pos.x  = ( float )x1 ;								\
				VertBuf[ VertNum ].pos.y  = ( float )y1 ;								\
				VertBuf[ VertNum ].color = Color ;										\
				VertBuf[ VertNum ].rhw   = 1.0f ;										\
				*((DWORD *)&VertBuf[ VertNum ].pos.z) = drawz ;							\
				VertNum ++ ;															\
																						\
				if( VertNum >= CIRCLE_VERTEX_NUM )										\
				{																		\
					Direct3DDevice9_DrawPrimitiveUP( D_D3DPT_POINTLIST, VertNum, VertBuf, sizeof( VERTEX_NOTEX_2D ) ) ;\
					GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;						\
					VertNum = 0 ;														\
				}																		\
			}

// ハードウエアアクセラレータ使用版 DrawCircle
extern	int		Graphics_D3D9_DrawCircle( int x, int y, int r, unsigned int Color, int FillFlag, int Rx_One_Minus, int Ry_One_Minus )
{
	VERTEX_NOTEX_2D *VertBuf ;
	DWORD VertNum ;
	BYTE *LineDrawBuf ;
	int Red, Green, Blue, Flag ;
	RECT DrawRect ;
	DWORD drawz ;

	Rx_One_Minus = Rx_One_Minus ? 1 : 0 ;
	Ry_One_Minus = Ry_One_Minus ? 1 : 0 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ; 

	// 描画用メモリの確保
	if( Graphics_Other_AllocCommonBuffer( 0, sizeof( VERTEX_NOTEX_2D ) * CIRCLE_VERTEX_NUM ) < 0 )
	{
		return -1 ;
	}
	VertBuf = ( VERTEX_NOTEX_2D * )GSYS.Resource.CommonBuffer[ 0 ] ;

	// 描画の準備
	Graphics_D3D9_BeginScene() ;
	Flag = DX_D3D9_DRAWPREP_DIFFUSERGB ;
	DX_D3D9_DRAWPREP_NOTEX( Flag )

	// Ｚバッファに書き込むＺ値をセットする
	drawz = *((DWORD *)&GSYS.DrawSetting.DrawZ);

	// 補正
	DrawRect = GSYS.DrawSetting.DrawArea ;

	NS_GetColor2( Color, &Red, &Green, &Blue ) ;
	SETUPCOLOR( Color )

	// FVF の変更
//	Graphics_D3D9_DeviceState_SetFVF( VERTEXFVF_NOTEX_2D ) ;
	Graphics_Hardware_D3D9_SetFVF_UseTable( VERTEXFVF_DECL_NOTEX_2D ) ;

	// 描画処理
	{
		int Dx, Dy, F/*, i*/, j ;
		int x1, x2, y1 ;

		// 初期値セット
		Dx = r ; Dy = 0 ; F = -2 * r + 3 ;
		VertNum = 0 ;

		j = 0 ;
		// 描画開始
		if( FillFlag )
		{
			if( Graphics_Other_AllocCommonBuffer( 1, ( DWORD )DrawRect.bottom ) < 0 )
			{
				return DXST_LOGFILE_ADDUTF16LE( "\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x69\x00\x6e\x00\x20\x00\x47\x00\x5f\x00\x44\x00\x33\x00\x44\x00\x5f\x00\x44\x00\x72\x00\x61\x00\x77\x00\x43\x00\x69\x00\x72\x00\x63\x00\x6c\x00\x65\x00\x00"/*@ L"メモリの確保に失敗しました in G_D3D_DrawCircle" @*/ ) ;
			}
			LineDrawBuf = ( BYTE * )GSYS.Resource.CommonBuffer[ 1 ] ;
			_MEMSET( LineDrawBuf, 0, ( size_t )DrawRect.bottom ) ;

			// 最初のラインを描く
			{
				if( Ry_One_Minus == FALSE )
				{
					x2 = Dx + x - Rx_One_Minus ; x1 = -Dx + x ; y1 = Dy + y ;
					DX_3D_CIRCLE ;
				}

				// 座標データを進める
				if( F >= 0 )
				{
					x2 = Dy + x - Rx_One_Minus ; x1 = -Dy + x ; y1 = Dx + y - Ry_One_Minus ;
					DX_3D_CIRCLE ;
					x2 = Dy + x - Rx_One_Minus ; x1 = -Dy + x ; y1 = -Dx + y;
					DX_3D_CIRCLE ;

					Dx -- ;
					F -= 4 * Dx ;
				}	

				Dy ++ ;
				F += 4 * Dy + 2 ;
			}

			while( Dx >= Dy )
			{
				x2 = Dx + x - Rx_One_Minus ; x1 = -Dx + x ; y1 = Dy + y - Ry_One_Minus ;
				DX_3D_CIRCLE ;
				x2 = Dx + x - Rx_One_Minus ; x1 = -Dx + x ; y1 = -Dy + y ;
				DX_3D_CIRCLE ;

				// 座標データを進める
				if( F >= 0 )
				{
					x2 = Dy + x - Rx_One_Minus ; x1 = -Dy + x ; y1 = Dx + y - Ry_One_Minus ;
					DX_3D_CIRCLE ;
					x2 = Dy + x - Rx_One_Minus ; x1 = -Dy + x ; y1 = -Dx + y ;
					DX_3D_CIRCLE ;

					Dx -- ;
					F -= 4 * Dx ;
				}

				Dy ++ ;
				F += 4 * Dy + 2 ;
			}
			if( VertNum )
			{
				Direct3DDevice9_DrawPrimitiveUP( D_D3DPT_LINELIST, ( UINT )( VertNum / 2 ), VertBuf, sizeof( VERTEX_NOTEX_2D ) ) ;
				GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;
			}
		}
		else
		{
			// 最初の点を描く
			{
				if( Ry_One_Minus == FALSE )
				{
					x1 = -Dx + x ; y1 = Dy + y ;
					DX_3D_CIRCLE_PSET ;
					x1 = Dx + x - Rx_One_Minus ;  
					DX_3D_CIRCLE_PSET ;
				}

				if( Rx_One_Minus == FALSE )
				{
					x1 = Dy + x ; y1 = Dx + y - Ry_One_Minus ;
					DX_3D_CIRCLE_PSET ;

					x1 = Dy + x ; y1 = -Dx + y;
					DX_3D_CIRCLE_PSET ;
				}

				// 座標データを進める
				if( F >= 0 )
				{
					Dx -- ;
					F -= 4 * Dx ;
				}	

				Dy ++ ;
				F += 4 * Dy + 2 ;
			}

			while( Dx >= Dy )
			{
				x1 = -Dx + x ; y1 = Dy + y - Ry_One_Minus ;
				DX_3D_CIRCLE_PSET ;
				x1 = Dx + x - Rx_One_Minus ;  
				DX_3D_CIRCLE_PSET ;

				x1 = Dy + x - Rx_One_Minus ; y1 = Dx + y - Ry_One_Minus ;
				DX_3D_CIRCLE_PSET ;
				x1 = -Dy + x ; 
				DX_3D_CIRCLE_PSET ;
			
				x1 = Dx + x - Rx_One_Minus ; y1 = -Dy + y ;
				DX_3D_CIRCLE_PSET ;
				x1 = -Dx + x ; 
				DX_3D_CIRCLE_PSET ;

				x1 = Dy + x - Rx_One_Minus ; y1 = -Dx + y ;
				DX_3D_CIRCLE_PSET ;
				x1 = -Dy + x ; 
				DX_3D_CIRCLE_PSET ;

				// 座標データを進める
				if( F >= 0 )
				{
					Dx -- ;
					F -= 4 * Dx ;
				}

				Dy ++ ;
				F += 4 * Dy + 2 ;
			}
			if( VertNum )
			{
				Direct3DDevice9_DrawPrimitiveUP( D_D3DPT_POINTLIST, VertNum, VertBuf, sizeof( VERTEX_NOTEX_2D ) ) ;
				GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;
			}
		}
	}

	// 終了
	return 0 ;
}

// ハードウエアアクセラレータ使用版 DrawOval
extern	int		Graphics_D3D9_DrawOval( int x, int y, int rx, int ry, unsigned int Color, int FillFlag, int Rx_One_Minus, int Ry_One_Minus )
{
	VERTEX_NOTEX_2D *VertBuf ;
	DWORD VertNum ;
	BYTE *LineDrawBuf ;
//	VERTEX_NOTEX_2D *VertData ;
//	int i, j, r2 ;
//	float xd, x2, y2 ;
//	float d, yd ;
	int Red, Green, Blue, Flag ;
	RECT DrawRect ;
	DWORD drawz ;

	Rx_One_Minus = Rx_One_Minus ? 1 : 0 ;
	Ry_One_Minus = Ry_One_Minus ? 1 : 0 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ; 
	if( !rx || !ry ) return -1 ;

	// 描画用メモリの確保
	if( Graphics_Other_AllocCommonBuffer( 0, sizeof( VERTEX_NOTEX_2D ) * CIRCLE_VERTEX_NUM ) < 0 )
	{
		return -1 ;
	}
	VertBuf = ( VERTEX_NOTEX_2D * )GSYS.Resource.CommonBuffer[ 0 ] ;

	// 描画の準備
	Graphics_D3D9_BeginScene() ;
	Graphics_D3D9_DrawSetting_SetTexture( NULL ) ;
	Flag = DX_D3D9_DRAWPREP_DIFFUSERGB ;
	DX_D3D9_DRAWPREP_NOTEX( Flag )

	// 補正
	if( rx < 0 ) rx *= -1 ;
	if( ry < 0 ) ry *= -1 ;
	DrawRect = GSYS.DrawSetting.DrawArea ;

	NS_GetColor2( Color, &Red, &Green, &Blue ) ;
	SETUPCOLOR( Color )

	// Ｚバッファに書き込むＺ値をセットする
	drawz = *((DWORD *)&GSYS.DrawSetting.DrawZ);

	// FVF の変更
//	Graphics_D3D9_DeviceState_SetFVF( VERTEXFVF_NOTEX_2D ) ;
	Graphics_Hardware_D3D9_SetFVF_UseTable( VERTEXFVF_DECL_NOTEX_2D ) ;

	VertNum = 0 ;

	// 描画処理
	{
		int Dx, Dy, F, H, j/*, i */ ;
		int x1, x2, y1, Df ;
		int yc, xc, rc, r ;

		j = 0 ;
		// 描画開始
		if( FillFlag )
		{
			if( Graphics_Other_AllocCommonBuffer( 1, ( DWORD )DrawRect.bottom ) < 0 )
			{
				return DXST_LOGFILE_ADDUTF16LE( "\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x69\x00\x6e\x00\x20\x00\x47\x00\x5f\x00\x44\x00\x33\x00\x44\x00\x5f\x00\x44\x00\x72\x00\x61\x00\x77\x00\x43\x00\x69\x00\x72\x00\x63\x00\x6c\x00\x65\x00\x00"/*@ L"メモリの確保に失敗しました in G_D3D_DrawCircle" @*/ ) ;
			}
			LineDrawBuf = ( BYTE * )GSYS.Resource.CommonBuffer[ 1 ] ;
			_MEMSET( LineDrawBuf, 0, ( size_t )DrawRect.bottom ) ;

			if( rx >= ry )
			{
				// 初期値をセット
				rc = ( rx * rx ) / ry ;

				xc = 0 ; yc = 0 ;
				r = rx ;
				Df = 1 ;
		
				Dx = r ;
				Dy = 0;
				F = -2 * r + 1 + 2 * 1;
				H = -4 * r + 2 + 1;

				// 最初の線を描く
				{
					if( Ry_One_Minus == FALSE )
					{
						x1 = -Dx + x ; x2 = Dx + x - Rx_One_Minus ; y1 = Dy + y ;
						DX_3D_CIRCLE ;
					}

					Df = 0 ;

					// 座標データを進める
					if( F < 0 )
					{
						yc += ry ;
						if( yc >= rc )
						{
							Dy ++ ;
							yc -= rc ;
							Df = 1 ;
						}
						F += 4 * Dy + 2 ;
						H += 4 * Dy;
					}
					else
					{
						Df = 1 ;

						if( H>=0 )
						{
							Dx--;
							F -= 4 * Dx;
							H -= 4 * Dx - 2 ;
						}
						else
						{
							Dx--;	
							yc += ry ;
							if( yc >= rc )
							{
								Dy ++ ;
								yc -= rc ;
								Df = 1 ;
							}
							F += 4 * Dy - 4 * Dx + 2 ;
							H += 4 * Dy - 4 * Dx + 2 ;
						}
					}
				}

				while( Dx >= 0 )
				{
					if( Df )
					{
						x1 = -Dx + x ; x2 = Dx + x - Rx_One_Minus ; y1 = Dy + y - Ry_One_Minus ;
						DX_3D_CIRCLE ;

						y1 = -Dy + y ;
						DX_3D_CIRCLE ;
					}

					Df = 0 ;

					// 座標データを進める
					if( F < 0 )
					{
						yc += ry ;
						if( yc >= rc )
						{
							Dy ++ ;
							yc -= rc ;
							Df = 1 ;
						}
						F += 4 * Dy + 2 ;
						H += 4 * Dy;
					}
					else
					{
						if( H>=0 )
						{
							Dx--;
							F -= 4 * Dx;
							H -= 4 * Dx - 2 ;
						}
						else
						{
							Dx--;	
							yc += ry ;
							if( yc >= rc )
							{
								Dy ++ ;
								yc -= rc ;
								Df = 1 ;
							}
							F += 4 * Dy - 4 * Dx + 2 ;
							H += 4 * Dy - 4 * Dx + 2 ;
						}
					}
				}

				// 最後の線を描く
				x1 = -Dx + x ; x2 = Dx + x - Rx_One_Minus ; y1 = -Dy + y;
				DX_3D_CIRCLE ;
			}
			else
			{
				// 初期値をセット
				rc = ( ry * ry ) / rx ;

				yc = 0 ; xc = 0 ;
				r = ry ;
		
				Dy = r ;
				Dx = 0;
				F = -2 * r + 1 + 2 * 1;
				H = -4 * r + 2 + 1;

				// 最初の線を描く
				{
					// 座標データを進める
					if( F < 0 )
					{
						xc += rx ;
						if( xc >= rc )
						{
							Dx ++ ;
							xc -= rc ;
						}
						F += 4 * Dx + 2 ;
						H += 4 * Dx;
					}
					else
					{
						if( H>=0 )
						{
							Dy--;
							F -= 4 * Dy;
							H -= 4 * Dy - 2 ;
						}
						else
						{
							Dy--;	
							xc += rx ;
							if( xc >= rc )
							{
								Dx ++ ;
								xc -= rc ;
							}
							F += 4 * Dx - 4 * Dy + 2 ;
							H += 4 * Dx - 4 * Dy + 2 ;
						}
					}
				}

				while( Dy >= 0 )
				{
					// 座標データを進める
					if( F < 0 )
					{
						xc += rx ;
						if( xc >= rc )
						{
							Dx ++ ;
							xc -= rc ;
						}
						F += 4 * Dx + 2 ;
						H += 4 * Dx;
					}
					else
					{
						y1 = -Dy + y ; x1 = -Dx + x ; x2 = Dx + x - Rx_One_Minus ;
						DX_3D_CIRCLE ;

						y1 = Dy + y - Ry_One_Minus ;
						DX_3D_CIRCLE ;

						if( H>=0 )
						{
							Dy--;
							F -= 4 * Dy;
							H -= 4 * Dy - 2 ;
						}
						else
						{
							Dy--;	
							xc += rx ;
							if( xc >= rc )
							{
								Dx ++ ;
								xc -= rc ;
							}
							F += 4 * Dx - 4 * Dy + 2 ;
							H += 4 * Dx - 4 * Dy + 2 ;
						}
					}
				}
			}
			if( VertNum )
			{
				Direct3DDevice9_DrawPrimitiveUP( D_D3DPT_LINELIST, VertNum / 2, VertBuf, sizeof( VERTEX_NOTEX_2D ) ) ;
				GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;
			}
		}
		else
		{
			if( rx >= ry )
			{
				// 初期値をセット
				rc = ( rx * rx ) / ry ;

				xc = 0 ; yc = 0 ;
				r = rx ;
				Df = 2 ;
		
				Dx = r ;
				Dy = 0;
				F = -2 * r + 1 + 2 * 1;
				H = -4 * r + 2 + 1;

				// 最初の点を描く
				{
					if( Ry_One_Minus == FALSE )
					{
						x1 = -Dx + x ; y1 = Dy + y ;
						DX_3D_CIRCLE_PSET ;
						x1 = Dx + x - Rx_One_Minus ;  
						DX_3D_CIRCLE_PSET ;
					}

					Df = 0 ;

					// 座標データを進める
					if( F < 0 )
					{
						yc += ry ;
						if( yc >= rc )
						{
							Dy ++ ;
							yc -= rc ;
							Df = 1 ;
						}
						F += 4 * Dy + 2 ;
						H += 4 * Dy;
					}
					else
					{
						Df = 1 ;

						if( H>=0 )
						{
							Dx--;
							F -= 4 * Dx;
							H -= 4 * Dx - 2 ;
						}
						else
						{
							Dx--;	
							yc += ry ;
							if( yc >= rc )
							{
								Dy ++ ;
								yc -= rc ;
								Df = 1 ;
							}
							F += 4 * Dy - 4 * Dx + 2 ;
							H += 4 * Dy - 4 * Dx + 2 ;
						}
					}
				}

				while( Dx > 0 )
				{
					if( Df )
					{
						x1 = -Dx + x ; y1 = Dy + y - Ry_One_Minus ;
						DX_3D_CIRCLE_PSET ;
						x1 = Dx + x - Rx_One_Minus ;
						DX_3D_CIRCLE_PSET ;

						x1 = -Dx + x ; y1 = -Dy + y ;
						DX_3D_CIRCLE_PSET ;
						x1 = Dx + x - Rx_One_Minus ;
						DX_3D_CIRCLE_PSET ;
					}

					Df = 0 ;

					// 座標データを進める
					if( F < 0 )
					{
						yc += ry ;
						if( yc >= rc )
						{
							Dy ++ ;
							yc -= rc ;
							Df = 1 ;
						}
						F += 4 * Dy + 2 ;
						H += 4 * Dy;
					}
					else
					{
						Df = 1 ;

						if( H>=0 )
						{
							Dx--;
							F -= 4 * Dx;
							H -= 4 * Dx - 2 ;
						}
						else
						{
							Dx--;	
							yc += ry ;
							if( yc >= rc )
							{
								Dy ++ ;
								yc -= rc ;
								Df = 1 ;
							}
							F += 4 * Dy - 4 * Dx + 2 ;
							H += 4 * Dy - 4 * Dx + 2 ;
						}
					}
				}

				// 最後の点を描く
				if( Rx_One_Minus == FALSE )
				{
					x1 = Dx + x ; y1 = -Dy + y ;
					DX_3D_CIRCLE_PSET ;

					x1 = -Dx + x ; y1 = Dy + y - Ry_One_Minus ;
					DX_3D_CIRCLE_PSET ;
				}
			}
			else
			{
				// 初期値をセット
				rc = ( ry * ry ) / rx ;

				xc = 0 ; yc = 0 ;
				r = ry ;
				Df = 1 ;
		
				Dy = r ;
				Dx = 0;
				F = -2 * r + 1 + 2 * 1;
				H = -4 * r + 2 + 1;

				// 最初の点を描く
				{
					if( Ry_One_Minus == FALSE )
					{
						y1 = -Dy + y ; x1 = Dx + x ;
						DX_3D_CIRCLE_PSET ;
						y1 = Dy + y - Ry_One_Minus ;  
						DX_3D_CIRCLE_PSET ;
					}

					Df = 0 ;

					// 座標データを進める
					if( F < 0 )
					{
						xc += rx ;
						if( xc >= rc )
						{
							Dx ++ ;
							xc -= rc ;
							Df = 1 ;
						}
						F += 4 * Dx + 2 ;
						H += 4 * Dx;
					}
					else
					{
						Df = 1 ;

						if( H>=0 )
						{
							Dy--;
							F -= 4 * Dy;
							H -= 4 * Dy - 2 ;
						}
						else
						{
							Dy--;	
							xc += rx ;
							if( xc >= rc )
							{
								Dx ++ ;
								xc -= rc ;
								Df = 1 ;
							}
							F += 4 * Dx - 4 * Dy + 2 ;
							H += 4 * Dx - 4 * Dy + 2 ;
						}
					}
				}

				while( Dy > 0 )
				{
					if( Df )
					{
						y1 = -Dy + y ; x1 = Dx + x - Rx_One_Minus ;
						DX_3D_CIRCLE_PSET ;
						y1 = Dy + y - Ry_One_Minus ;
						DX_3D_CIRCLE_PSET ;

						y1 = -Dy + y ; x1 = -Dx + x ;
						DX_3D_CIRCLE_PSET ;
						y1 = Dy + y - Ry_One_Minus ;
						DX_3D_CIRCLE_PSET ;
					}

					Df = 0 ;

					// 座標データを進める
					if( F < 0 )
					{
						xc += rx ;
						if( xc >= rc )
						{
							Dx ++ ;
							xc -= rc ;
							Df = 1 ;
						}
						F += 4 * Dx + 2 ;
						H += 4 * Dx;
					}
					else
					{
						Df = 1 ;

						if( H>=0 )
						{
							Dy--;
							F -= 4 * Dy;
							H -= 4 * Dy - 2 ;
						}
						else
						{
							Dy--;	
							xc += rx ;
							if( xc >= rc )
							{
								Dx ++ ;
								xc -= rc ;
								Df = 1 ;
							}
							F += 4 * Dx - 4 * Dy + 2 ;
							H += 4 * Dx - 4 * Dy + 2 ;
						}
					}
				}

				// 最後の点を描く
				if( Rx_One_Minus == FALSE )
				{
					y1 = Dy + y ; x1 = -Dx + x ;
					DX_3D_CIRCLE_PSET ;

					y1 = -Dy + y ; x1 = Dx + x - Rx_One_Minus ;
					DX_3D_CIRCLE_PSET ;
				}
			}
			if( VertNum )
			{
				Direct3DDevice9_DrawPrimitiveUP( D_D3DPT_POINTLIST, VertNum, VertBuf, sizeof( VERTEX_NOTEX_2D ) ) ;
				GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;
			}
		}
	}

	// 終了
	return 0 ;
}

// ハードウエアアクセラレータ使用版 DrawTriangle
extern	int		Graphics_D3D9_DrawTriangle( int x1, int y1, int x2, int y2, int x3, int y3, unsigned int Color, int FillFlag )
{
	VERTEX_NOTEX_2D *vec ; 
	int Red, Green, Blue ;
	int Flag ;
	int sx1, sx2, sy1, sy2 ;
	DWORD drawz ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	// 描画の準備
	Graphics_D3D9_BeginScene() ;
	Graphics_D3D9_DrawSetting_SetTexture( NULL ) ;
	Flag = DX_D3D9_DRAWPREP_DIFFUSERGB ;
	DX_D3D9_DRAWPREP_NOTEX( Flag )

	// Ｚバッファに書き込むＺ値をセットする
	drawz = *((DWORD *)&GSYS.DrawSetting.DrawZ);

	// 色その他ステータスのセット
	NS_GetColor2( Color, &Red, &Green, &Blue ) ;
	SETUPCOLOR( Color )

	// ラインかどうかで処理を分岐
	if( FillFlag )
	{
		GETVERTEX_TRIANGLE( vec ) ;

		vec[ 0 ].color = Color ;

		vec[ 0 ].pos.x = ( float )x1 ; vec[ 0 ].pos.y = ( float )y1 ;

		// カリングが有効になっている場合はカリングされないようにする
		switch( GSYS.DrawSetting.CullMode )
		{
		case 0 :	// カリングなし
			vec[ 1 ].pos.x = ( float )x2 ; vec[ 1 ].pos.y = ( float )y2 ;
			vec[ 2 ].pos.x = ( float )x3 ; vec[ 2 ].pos.y = ( float )y3 ;
			break ;

		case 1 :	// 左回りカリング
			sx1 = x2 - x1 ;		sy1 = y2 - y1 ;
			sx2 = x3 - x1 ;		sy2 = y3 - y1 ;
			if( sx1 * sy2 - sy1 * sx2 > 0 )
			{
				vec[ 1 ].pos.x = ( float )x2 ; vec[ 1 ].pos.y = ( float )y2 ;
				vec[ 2 ].pos.x = ( float )x3 ; vec[ 2 ].pos.y = ( float )y3 ;
			}
			else
			{
				vec[ 1 ].pos.x = ( float )x3 ; vec[ 1 ].pos.y = ( float )y3 ;
				vec[ 2 ].pos.x = ( float )x2 ; vec[ 2 ].pos.y = ( float )y2 ;
			}
			break ;

		case 2 :	// 右回りカリング
			sx1 = x2 - x1 ;		sy1 = y2 - y1 ;
			sx2 = x3 - x1 ;		sy2 = y3 - y1 ;
			if( sx1 * sy2 - sy1 * sx2 < 0 )
			{
				vec[ 1 ].pos.x = ( float )x2 ; vec[ 1 ].pos.y = ( float )y2 ;
				vec[ 2 ].pos.x = ( float )x3 ; vec[ 2 ].pos.y = ( float )y3 ;
			}
			else
			{
				vec[ 1 ].pos.x = ( float )x3 ; vec[ 1 ].pos.y = ( float )y3 ;
				vec[ 2 ].pos.x = ( float )x2 ; vec[ 2 ].pos.y = ( float )y2 ;
			}
			break ;
		}

		*( ( DWORD * )&vec[ 0 ].pos.z ) = drawz ;
		*( ( DWORD * )&vec[ 1 ].pos.z ) = drawz ;
		*( ( DWORD * )&vec[ 2 ].pos.z ) = drawz ;

		ADD3VERTEX_NOTEX
	}
	else
	{
		GETVERTEX_LINETRIANGLE( vec )

		vec[ 0 ].color =
		vec[ 1 ].color =
		vec[ 2 ].color =
		vec[ 3 ].color =
		vec[ 4 ].color =
		vec[ 5 ].color = Color ;

		vec[ 0 ].pos.x = ( float )x1 ; vec[ 0 ].pos.y = ( float )y1 ;
		vec[ 1 ].pos.x = ( float )x2 ; vec[ 1 ].pos.y = ( float )y2 ;
		vec[ 2 ].pos.x = ( float )x2 ; vec[ 2 ].pos.y = ( float )y2 ;
		vec[ 3 ].pos.x = ( float )x3 ; vec[ 3 ].pos.y = ( float )y3 ;
		vec[ 4 ].pos.x = ( float )x3 ; vec[ 4 ].pos.y = ( float )y3 ;
		vec[ 5 ].pos.x = ( float )x1 ; vec[ 5 ].pos.y = ( float )y1 ;
		*( ( DWORD * )&vec[ 0 ].pos.z ) = drawz ;
		*( ( DWORD * )&vec[ 1 ].pos.z ) = drawz ;
		*( ( DWORD * )&vec[ 2 ].pos.z ) = drawz ;
		*( ( DWORD * )&vec[ 3 ].pos.z ) = drawz ;
		*( ( DWORD * )&vec[ 4 ].pos.z ) = drawz ;
		*( ( DWORD * )&vec[ 5 ].pos.z ) = drawz ;

		ADD4VERTEX_LINETRIANGLE
	}

	// 終了
	return 0 ;
}

// ハードウエアアクセラレータ使用版 DrawTriangle3D
extern	int		Graphics_D3D9_DrawTriangle3D( VECTOR Pos1, VECTOR Pos2, VECTOR Pos3, unsigned int Color, int FillFlag, int DrawFlag, RECT *DrawArea )
{
	VERTEX_NOTEX_3D *vec ; 
	int Red, Green, Blue ;
	int Flag ;

	if( DrawFlag )
	{
		if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

		// 描画の準備
		if( GD3D9.Device.DrawInfo.BeginSceneFlag == FALSE ) Graphics_D3D9_BeginScene() ;
		if( GD3D9.Device.DrawSetting.RenderTexture != NULL ) Graphics_D3D9_DrawSetting_SetTexture( NULL ) ;

		Flag = DX_D3D9_DRAWPREP_DIFFUSERGB | DX_D3D9_DRAWPREP_3D | DX_D3D9_DRAWPREP_FOG | DX_D3D9_DRAWPREP_CULLING ;
		DX_D3D9_DRAWPREP_NOTEX( Flag )

		// 色その他ステータスのセット
		NS_GetColor2( Color, &Red, &Green, &Blue ) ;
		SETUPCOLOR( Color )

		// ラインかどうかで処理を分岐
		if( FillFlag )
		{
			GETVERTEX_TRIANGLE3D( vec ) ;

			*( ( DWORD * )&vec[ 0 ].b ) =
			*( ( DWORD * )&vec[ 1 ].b ) =
			*( ( DWORD * )&vec[ 2 ].b ) = ( DWORD )Color ;
			vec[ 0 ].pos = Pos1 ;
			vec[ 1 ].pos = Pos2 ;
			vec[ 2 ].pos = Pos3 ;

			ADD3VERTEX_NOTEX3D
		}
		else
		{
			GETVERTEX_LINETRIANGLE3D( vec )

			*( ( DWORD * )&vec[ 0 ].b ) =
			*( ( DWORD * )&vec[ 1 ].b ) =
			*( ( DWORD * )&vec[ 2 ].b ) =
			*( ( DWORD * )&vec[ 3 ].b ) =
			*( ( DWORD * )&vec[ 4 ].b ) =
			*( ( DWORD * )&vec[ 5 ].b ) = ( DWORD )Color ;

			vec[ 0 ].pos = Pos1 ;
			vec[ 1 ].pos = Pos2 ;
			vec[ 2 ].pos = Pos2 ;
			vec[ 3 ].pos = Pos3 ;
			vec[ 4 ].pos = Pos3 ;
			vec[ 5 ].pos = Pos1 ;

			ADD4VERTEX_LINETRIANGLE3D
		}
	}
	else
	{
		VERTEX_2D TempVert[ 3 ] ;
		float dleft, dright, dtop, dbottom ;

		dleft   =  100000000.0f ;
		dright  = -100000000.0f ;
		dtop    =  100000000.0f ;
		dbottom = -100000000.0f ;

		// スクリーン座標に変換
		TempVert[0].pos.x = Pos1.x * GSYS.DrawSetting.Blend3DMatrixF.m[0][0] + Pos1.y * GSYS.DrawSetting.Blend3DMatrixF.m[1][0] + Pos1.z * GSYS.DrawSetting.Blend3DMatrixF.m[2][0] + GSYS.DrawSetting.Blend3DMatrixF.m[3][0]  ;
		TempVert[0].pos.y = Pos1.x * GSYS.DrawSetting.Blend3DMatrixF.m[0][1] + Pos1.y * GSYS.DrawSetting.Blend3DMatrixF.m[1][1] + Pos1.z * GSYS.DrawSetting.Blend3DMatrixF.m[2][1] + GSYS.DrawSetting.Blend3DMatrixF.m[3][1]  ;
		TempVert[0].pos.z = Pos1.x * GSYS.DrawSetting.Blend3DMatrixF.m[0][2] + Pos1.y * GSYS.DrawSetting.Blend3DMatrixF.m[1][2] + Pos1.z * GSYS.DrawSetting.Blend3DMatrixF.m[2][2] + GSYS.DrawSetting.Blend3DMatrixF.m[3][2]  ;
		TempVert[0].rhw   = Pos1.x * GSYS.DrawSetting.Blend3DMatrixF.m[0][3] + Pos1.y * GSYS.DrawSetting.Blend3DMatrixF.m[1][3] + Pos1.z * GSYS.DrawSetting.Blend3DMatrixF.m[2][3] + GSYS.DrawSetting.Blend3DMatrixF.m[3][3]  ;

		TempVert[1].pos.x = Pos2.x * GSYS.DrawSetting.Blend3DMatrixF.m[0][0] + Pos2.y * GSYS.DrawSetting.Blend3DMatrixF.m[1][0] + Pos2.z * GSYS.DrawSetting.Blend3DMatrixF.m[2][0] + GSYS.DrawSetting.Blend3DMatrixF.m[3][0]  ;
		TempVert[1].pos.y = Pos2.x * GSYS.DrawSetting.Blend3DMatrixF.m[0][1] + Pos2.y * GSYS.DrawSetting.Blend3DMatrixF.m[1][1] + Pos2.z * GSYS.DrawSetting.Blend3DMatrixF.m[2][1] + GSYS.DrawSetting.Blend3DMatrixF.m[3][1]  ;
		TempVert[1].pos.z = Pos2.x * GSYS.DrawSetting.Blend3DMatrixF.m[0][2] + Pos2.y * GSYS.DrawSetting.Blend3DMatrixF.m[1][2] + Pos2.z * GSYS.DrawSetting.Blend3DMatrixF.m[2][2] + GSYS.DrawSetting.Blend3DMatrixF.m[3][2]  ;
		TempVert[1].rhw   = Pos2.x * GSYS.DrawSetting.Blend3DMatrixF.m[0][3] + Pos2.y * GSYS.DrawSetting.Blend3DMatrixF.m[1][3] + Pos2.z * GSYS.DrawSetting.Blend3DMatrixF.m[2][3] + GSYS.DrawSetting.Blend3DMatrixF.m[3][3]  ;

		TempVert[2].pos.x = Pos3.x * GSYS.DrawSetting.Blend3DMatrixF.m[0][0] + Pos3.y * GSYS.DrawSetting.Blend3DMatrixF.m[1][0] + Pos3.z * GSYS.DrawSetting.Blend3DMatrixF.m[2][0] + GSYS.DrawSetting.Blend3DMatrixF.m[3][0]  ;
		TempVert[2].pos.y = Pos3.x * GSYS.DrawSetting.Blend3DMatrixF.m[0][1] + Pos3.y * GSYS.DrawSetting.Blend3DMatrixF.m[1][1] + Pos3.z * GSYS.DrawSetting.Blend3DMatrixF.m[2][1] + GSYS.DrawSetting.Blend3DMatrixF.m[3][1]  ;
		TempVert[2].pos.z = Pos3.x * GSYS.DrawSetting.Blend3DMatrixF.m[0][2] + Pos3.y * GSYS.DrawSetting.Blend3DMatrixF.m[1][2] + Pos3.z * GSYS.DrawSetting.Blend3DMatrixF.m[2][2] + GSYS.DrawSetting.Blend3DMatrixF.m[3][2]  ;
		TempVert[2].rhw   = Pos3.x * GSYS.DrawSetting.Blend3DMatrixF.m[0][3] + Pos3.y * GSYS.DrawSetting.Blend3DMatrixF.m[1][3] + Pos3.z * GSYS.DrawSetting.Blend3DMatrixF.m[2][3] + GSYS.DrawSetting.Blend3DMatrixF.m[3][3]  ;

		TempVert[0].rhw = 1.0F / TempVert[0].rhw ;
		TempVert[0].pos.x *= TempVert[0].rhw ;
		TempVert[0].pos.y *= TempVert[0].rhw ;
		TempVert[0].pos.z *= TempVert[0].rhw ;

		TempVert[1].rhw = 1.0F / TempVert[1].rhw ;
		TempVert[1].pos.x *= TempVert[1].rhw ;
		TempVert[1].pos.y *= TempVert[1].rhw ;
		TempVert[1].pos.z *= TempVert[1].rhw ;

		TempVert[2].rhw = 1.0F / TempVert[2].rhw ;
		TempVert[2].pos.x *= TempVert[2].rhw ;
		TempVert[2].pos.y *= TempVert[2].rhw ;
		TempVert[2].pos.z *= TempVert[2].rhw ;

		if( dright  < TempVert[ 0 ].pos.x ) dright  = TempVert[ 0 ].pos.x ;
		if( dright  < TempVert[ 1 ].pos.x ) dright  = TempVert[ 1 ].pos.x ;
		if( dright  < TempVert[ 2 ].pos.x ) dright  = TempVert[ 2 ].pos.x ;

		if( dleft   > TempVert[ 0 ].pos.x ) dleft   = TempVert[ 0 ].pos.x ;
		if( dleft   > TempVert[ 1 ].pos.x ) dleft   = TempVert[ 1 ].pos.x ;
		if( dleft   > TempVert[ 2 ].pos.x ) dleft   = TempVert[ 2 ].pos.x ;

		if( dbottom < TempVert[ 0 ].pos.y ) dbottom = TempVert[ 0 ].pos.y ;
		if( dbottom < TempVert[ 1 ].pos.y ) dbottom = TempVert[ 1 ].pos.y ;
		if( dbottom < TempVert[ 2 ].pos.y ) dbottom = TempVert[ 2 ].pos.y ;

		if( dtop    > TempVert[ 0 ].pos.y ) dtop    = TempVert[ 0 ].pos.y ;
		if( dtop    > TempVert[ 1 ].pos.y ) dtop    = TempVert[ 1 ].pos.y ;
		if( dtop    > TempVert[ 2 ].pos.y ) dtop    = TempVert[ 2 ].pos.y ;

		DrawArea->left   = _FTOL( dleft   ) - 1 ;
		DrawArea->right  = _FTOL( dright  ) + 1 ;
		DrawArea->top    = _FTOL( dtop    ) - 1 ;
		DrawArea->bottom = _FTOL( dbottom ) + 1 ;
	}

	// 終了
	return 0 ;
}

// ハードウエアアクセラレータ使用版 DrawQuadrangle
extern	int		Graphics_D3D9_DrawQuadrangle( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int Color, int FillFlag )
{
	VERTEX_NOTEX_2D VertData[ 5 ], *Vert ; 
	int Red, Green, Blue ;
	int i, Flag ;
	DWORD drawz ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	// 描画の準備
	Graphics_D3D9_BeginScene() ;
	Flag = DX_D3D9_DRAWPREP_DIFFUSERGB ;
	DX_D3D9_DRAWPREP_NOTEX( Flag )

	// Ｚバッファに書き込むＺ値をセットする
	drawz = *((DWORD *)&GSYS.DrawSetting.DrawZ);

	// 色その他ステータスのセット
	NS_GetColor2( Color, &Red, &Green, &Blue ) ;
	SETUPCOLOR( Color )
	Vert = VertData ;
	for( i = 0 ; i < 5 ; i ++, Vert ++ )
	{
		Vert->color = Color ;
		Vert->rhw   = 1.0f ;
		*((DWORD *)&Vert->pos.z) = drawz ;
	}

	// FVF のセット
//	Graphics_D3D9_DeviceState_SetFVF( VERTEXFVF_NOTEX_2D ) ;
	Graphics_Hardware_D3D9_SetFVF_UseTable( VERTEXFVF_DECL_NOTEX_2D ) ;

	// 塗りつぶしかどうかで処理を分岐
	if( FillFlag )
	{
		// 塗りつぶしの場合
		VertData[ 0 ].pos.x = ( float )x1 ; VertData[ 0 ].pos.y = ( float )y1 ;
		VertData[ 1 ].pos.x = ( float )x2 ; VertData[ 1 ].pos.y = ( float )y2 ;
		VertData[ 2 ].pos.x = ( float )x4 ; VertData[ 2 ].pos.y = ( float )y4 ;
		VertData[ 3 ].pos.x = ( float )x3 ; VertData[ 3 ].pos.y = ( float )y3 ;

		// 反転処理判定
		{
			double Reverse0, Reverse1 ;
			VERTEX_NOTEX_2D VertTemp ;

			Reverse0 =  ( VertData[ 1 ].pos.x - VertData[ 0 ].pos.x ) * ( VertData[ 2 ].pos.y - VertData[ 0 ].pos.y ) -
					    ( VertData[ 2 ].pos.x - VertData[ 0 ].pos.x ) * ( VertData[ 1 ].pos.y - VertData[ 0 ].pos.y ) ;
			Reverse1 = -( VertData[ 1 ].pos.x - VertData[ 3 ].pos.x ) * ( VertData[ 2 ].pos.y - VertData[ 3 ].pos.y ) +
						( VertData[ 1 ].pos.y - VertData[ 3 ].pos.y ) * ( VertData[ 2 ].pos.x - VertData[ 3 ].pos.x ) ;
			if( Reverse0 <= 0 && Reverse1 <= 0 )
			{
				VertTemp = VertData[ 1 ] ; VertData[ 1 ] = VertData[ 2 ] ; VertData[ 2 ] = VertTemp ;
			}

			if( Reverse0 * Reverse1 >= 0 )
			{
				// プリミティブの描画
				Direct3DDevice9_DrawPrimitiveUP( D_D3DPT_TRIANGLESTRIP, 2, VertData, sizeof( VERTEX_NOTEX_2D ) ) ;
				GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;
			}
			else
			{
				// Ｘ方向反転時の補正
				if( Reverse0 < 0 ) 
				{
					VertTemp = VertData[ 1 ] ; VertData[ 1 ] = VertData[ 2 ] ; VertData[ 2 ] = VertTemp ;
				}

				// プリミティブの描画
				Direct3DDevice9_DrawPrimitiveUP( D_D3DPT_TRIANGLESTRIP, 2, VertData, sizeof( VERTEX_NOTEX_2D ) ) ;
				GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;
			}
		}
	}
	else
	{
		// 塗りつぶしでは無い場合
		VertData[ 0 ].pos.x = ( float )x1 ; VertData[ 0 ].pos.y = ( float )y1 ;
		VertData[ 1 ].pos.x = ( float )x2 ; VertData[ 1 ].pos.y = ( float )y2 ;
		VertData[ 2 ].pos.x = ( float )x3 ; VertData[ 2 ].pos.y = ( float )y3 ;
		VertData[ 3 ].pos.x = ( float )x4 ; VertData[ 3 ].pos.y = ( float )y4 ;
		VertData[ 4 ].pos.x = ( float )x1 ; VertData[ 4 ].pos.y = ( float )y1 ;

		// プリミティブの描画
		Direct3DDevice9_DrawPrimitiveUP( D_D3DPT_LINESTRIP, 4, VertData, sizeof( VERTEX_NOTEX_2D ) ) ;
		GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;
	}

	// 終了
	return 0 ;
}

// ハードウエアアクセラレータ使用版 DrawQuadrangle
extern	int		Graphics_D3D9_DrawQuadrangleF( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int Color, int FillFlag )
{
	VERTEX_NOTEX_2D VertData[ 5 ], *Vert ; 
	int Red, Green, Blue ;
	int i, Flag ;
	DWORD drawz ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	// 描画の準備
	Graphics_D3D9_BeginScene() ;
	Flag = DX_D3D9_DRAWPREP_DIFFUSERGB ;
	DX_D3D9_DRAWPREP_NOTEX( Flag )

	// Ｚバッファに書き込むＺ値をセットする
	drawz = *((DWORD *)&GSYS.DrawSetting.DrawZ);

	// 色その他ステータスのセット
	NS_GetColor2( Color, &Red, &Green, &Blue ) ;
	SETUPCOLOR( Color )
	Vert = VertData ;
	for( i = 0 ; i < 5 ; i ++, Vert ++ )
	{
		Vert->color = Color ;
		Vert->rhw   = 1.0f ;
		*((DWORD *)&Vert->pos.z) = drawz ;
	}

	// FVF のセット
//	Graphics_D3D9_DeviceState_SetFVF( VERTEXFVF_NOTEX_2D ) ;
	Graphics_Hardware_D3D9_SetFVF_UseTable( VERTEXFVF_DECL_NOTEX_2D ) ;

	// 塗りつぶしかどうかで処理を分岐
	if( FillFlag )
	{
		// 塗りつぶしの場合
		VertData[ 0 ].pos.x = x1 ; VertData[ 0 ].pos.y = y1 ;
		VertData[ 1 ].pos.x = x2 ; VertData[ 1 ].pos.y = y2 ;
		VertData[ 2 ].pos.x = x4 ; VertData[ 2 ].pos.y = y4 ;
		VertData[ 3 ].pos.x = x3 ; VertData[ 3 ].pos.y = y3 ;

		// 反転処理判定
		{
			double Reverse0, Reverse1 ;
			VERTEX_NOTEX_2D VertTemp ;

			Reverse0 =  ( VertData[ 1 ].pos.x - VertData[ 0 ].pos.x ) * ( VertData[ 2 ].pos.y - VertData[ 0 ].pos.y ) -
					    ( VertData[ 2 ].pos.x - VertData[ 0 ].pos.x ) * ( VertData[ 1 ].pos.y - VertData[ 0 ].pos.y ) ;
			Reverse1 = -( VertData[ 1 ].pos.x - VertData[ 3 ].pos.x ) * ( VertData[ 2 ].pos.y - VertData[ 3 ].pos.y ) +
						( VertData[ 1 ].pos.y - VertData[ 3 ].pos.y ) * ( VertData[ 2 ].pos.x - VertData[ 3 ].pos.x ) ;
			if( Reverse0 <= 0 && Reverse1 <= 0 )
			{
				VertTemp = VertData[ 1 ] ; VertData[ 1 ] = VertData[ 2 ] ; VertData[ 2 ] = VertTemp ;
			}

			if( Reverse0 * Reverse1 >= 0 )
			{
				// プリミティブの描画
				Direct3DDevice9_DrawPrimitiveUP( D_D3DPT_TRIANGLESTRIP, 2, VertData, sizeof( VERTEX_NOTEX_2D ) ) ;
				GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;
			}
			else
			{
				// Ｘ方向反転時の補正
				if( Reverse0 < 0 ) 
				{
					VertTemp = VertData[ 1 ] ; VertData[ 1 ] = VertData[ 2 ] ; VertData[ 2 ] = VertTemp ;
				}

				// プリミティブの描画
				Direct3DDevice9_DrawPrimitiveUP( D_D3DPT_TRIANGLESTRIP, 2, VertData, sizeof( VERTEX_NOTEX_2D ) ) ;
				GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;
			}
		}
	}
	else
	{
		// 塗りつぶしでは無い場合
		VertData[ 0 ].pos.x = x1 ; VertData[ 0 ].pos.y = y1 ;
		VertData[ 1 ].pos.x = x2 ; VertData[ 1 ].pos.y = y2 ;
		VertData[ 2 ].pos.x = x3 ; VertData[ 2 ].pos.y = y3 ;
		VertData[ 3 ].pos.x = x4 ; VertData[ 3 ].pos.y = y4 ;
		VertData[ 4 ].pos.x = x1 ; VertData[ 4 ].pos.y = y1 ;

		// プリミティブの描画
		Direct3DDevice9_DrawPrimitiveUP( D_D3DPT_LINESTRIP, 4, VertData, sizeof( VERTEX_NOTEX_2D ) ) ;
		GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;
	}

	// 終了
	return 0 ;
}

// ハードウエアアクセラレータ使用版 DrawPixel
extern	int		Graphics_D3D9_DrawPixel( int x, int y, unsigned int Color )
{
	VERTEX_NOTEX_2D *VertData ;
	int Red, Green, Blue, Flag ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ; 

	// 描画の準備
	if( GD3D9.Device.DrawInfo.BeginSceneFlag == FALSE ) Graphics_D3D9_BeginScene() ;
	Flag = DX_D3D9_DRAWPREP_DIFFUSERGB ;
	DX_D3D9_DRAWPREP_NOTEX( Flag )

	// 色その他ステータスのセット
	GETVERTEX_POINT( VertData ) ;

	if( GD3D9.Device.DrawInfo.DiffuseColor == 0xffffffff )
	{
		NS_GetColor2( Color, &Red, &Green, &Blue ) ;
		VertData->color = 0xff000000 | ( Red << 16 ) | ( Green << 8 ) | Blue ;
	}
	else
	{
		NS_GetColor2( Color, &Red, &Green, &Blue ) ;
		SETUPCOLOR( VertData->color )
	}

	// 座標のセット
	VertData->pos.x = ( float )x ;
	VertData->pos.y = ( float )y ;
	VertData->pos.z = GSYS.DrawSetting.DrawZ;

	// 頂点の追加
	ADD4VERTEX_POINT

	// 終了
	return 0 ;
}

// ハードウエアアクセラレータ使用版 DrawPixel3D
extern	int		Graphics_D3D9_DrawPixel3D( VECTOR Pos, unsigned int Color, int DrawFlag, RECT *DrawArea )
{
	VERTEX_NOTEX_3D *VertData ; 
	int Red, Green, Blue ;
	int Flag ;

	if( DrawFlag )
	{
		if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

		// 描画の準備
		if( GD3D9.Device.DrawInfo.BeginSceneFlag == FALSE ) Graphics_D3D9_BeginScene() ;
		if( GD3D9.Device.DrawSetting.RenderTexture != NULL ) Graphics_D3D9_DrawSetting_SetTexture( NULL ) ;

		Flag = DX_D3D9_DRAWPREP_DIFFUSERGB | DX_D3D9_DRAWPREP_3D | DX_D3D9_DRAWPREP_FOG ;
		DX_D3D9_DRAWPREP_NOTEX( Flag )

		// 色その他ステータスのセット
		GETVERTEX_POINT3D( VertData ) ;

		NS_GetColor2( Color, &Red, &Green, &Blue ) ;
		if( GD3D9.Device.DrawInfo.DiffuseColor == 0xffffffff )
		{
			*( ( DWORD * )&VertData->b ) = ( DWORD )( 0xff000000 | ( Red << 16 ) | ( Green << 8 ) | Blue ) ;
		}
		else
		{
			SETUPCOLOR( *( ( DWORD * )&VertData->b ) )
		}

		// 座標のセット
		VertData->pos = Pos ;

		// 頂点の追加
		ADD4VERTEX_POINT3D
	}
	else
	{
		VERTEX_2D TempVert ;

		// スクリーン座標に変換
		TempVert.pos.x = Pos.x * GSYS.DrawSetting.Blend3DMatrixF.m[0][0] + Pos.y * GSYS.DrawSetting.Blend3DMatrixF.m[1][0] + Pos.z * GSYS.DrawSetting.Blend3DMatrixF.m[2][0] + GSYS.DrawSetting.Blend3DMatrixF.m[3][0]  ;
		TempVert.pos.y = Pos.x * GSYS.DrawSetting.Blend3DMatrixF.m[0][1] + Pos.y * GSYS.DrawSetting.Blend3DMatrixF.m[1][1] + Pos.z * GSYS.DrawSetting.Blend3DMatrixF.m[2][1] + GSYS.DrawSetting.Blend3DMatrixF.m[3][1]  ;
		TempVert.pos.z = Pos.x * GSYS.DrawSetting.Blend3DMatrixF.m[0][2] + Pos.y * GSYS.DrawSetting.Blend3DMatrixF.m[1][2] + Pos.z * GSYS.DrawSetting.Blend3DMatrixF.m[2][2] + GSYS.DrawSetting.Blend3DMatrixF.m[3][2]  ;
		TempVert.rhw   = Pos.x * GSYS.DrawSetting.Blend3DMatrixF.m[0][3] + Pos.y * GSYS.DrawSetting.Blend3DMatrixF.m[1][3] + Pos.z * GSYS.DrawSetting.Blend3DMatrixF.m[2][3] + GSYS.DrawSetting.Blend3DMatrixF.m[3][3]  ;

		TempVert.rhw = 1.0F / TempVert.rhw ;
		TempVert.pos.x *= TempVert.rhw ;
		TempVert.pos.y *= TempVert.rhw ;
		TempVert.pos.z *= TempVert.rhw ;

		DrawArea->left   = _FTOL( TempVert.pos.x ) - 1 ;
		DrawArea->right  = _FTOL( TempVert.pos.x ) + 1 ;
		DrawArea->top    = _FTOL( TempVert.pos.y ) - 1 ;
		DrawArea->bottom = _FTOL( TempVert.pos.y ) + 1 ;
	}

	// 終了
	return 0 ;
}

// ハードウエアアクセラレータ使用版 DrawPixelSet
#define SetDrawNum (1000)
extern	int		Graphics_D3D9_DrawPixelSet( const POINTDATA *PointData, int Num )
{
	DWORD i, j/*, k*/ ;
	static VERTEX_NOTEX_2D VertData[ SetDrawNum ], *Vert ;
	COLORDATA ColorData ;
	int MaxRed, MaxGreen, MaxBlue ; 
	unsigned int Color ;
	int Flag ;
	DWORD drawz ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画の準備
	Graphics_D3D9_BeginScene() ;
	Flag = DX_D3D9_DRAWPREP_DIFFUSERGB ;
	DX_D3D9_DRAWPREP_NOTEX( Flag )

	// Ｚバッファに書き込むＺ値をセットする
	drawz = *((DWORD *)&GSYS.DrawSetting.DrawZ);

	// ベクトル基本ステータスを取得
	ColorData = *( ( COLORDATA * )NS_GetDispColorData() ) ;
	
	MaxRed		= ( 1 << ColorData.RedWidth	) - 1 ; 
	MaxGreen	= ( 1 << ColorData.GreenWidth ) - 1 ;
	MaxBlue		= ( 1 << ColorData.BlueWidth ) - 1 ; 

	j = 0 ;
	Vert = VertData ;

	// FVF のセット
//	Graphics_D3D9_DeviceState_SetFVF( VERTEXFVF_NOTEX_2D ) ;
	Graphics_Hardware_D3D9_SetFVF_UseTable( VERTEXFVF_DECL_NOTEX_2D ) ;

	for( i = 0 ; i < ( UINT )Num ; i ++, PointData ++ )
	{
		// 色その他ステータスのセット
		Color = PointData->color ;
		Vert->color = ( PointData->pal << 24 ) |
						( ( ( ( ( ( Color & ColorData.RedMask	) >> ColorData.RedLoc	) << 8 ) - 1 ) / MaxRed		) << 16 ) |
						( ( ( ( ( ( Color & ColorData.GreenMask	) >> ColorData.GreenLoc	) << 8 ) - 1 ) / MaxGreen	) << 8 ) |
						( ( ( ( ( ( Color & ColorData.BlueMask	) >> ColorData.BlueLoc	) << 8 ) - 1 ) / MaxBlue	) ) ; 
		Vert->rhw	= 1.0f ;
		*((DWORD *)&Vert->pos.z) = drawz ;

		// 座標のセット＆ウインドウ直接描画時補正
		Vert->pos.x = ( float )PointData->x ;
		Vert->pos.y = ( float )PointData->y ;

		j ++ ;
		Vert ++ ;
		if( j == SetDrawNum )
		{
			// 描画
			Direct3DDevice9_DrawPrimitiveUP( D_D3DPT_POINTLIST, j, VertData, sizeof( VERTEX_NOTEX_2D ) ) ;
			GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;
			j = 0 ;
			Vert = VertData ;
		}
	}

	if( j != 0 )
	{
		// 描画
		Direct3DDevice9_DrawPrimitiveUP( D_D3DPT_POINTLIST, j, VertData, sizeof( VERTEX_NOTEX_2D ) ) ;
		GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;
	}

	// 終了
	return 0;
}

// ハードウエアアクセラレータ使用版 DrawLineSet
extern	int		Graphics_D3D9_DrawLineSet( const LINEDATA *LineData, int Num )
{
	DWORD i, j/*, k, l */ ;
	static VERTEX_NOTEX_2D VertData[ SetDrawNum ], *Vert ;
	COLORDATA ColorData ;
	int MaxRed, MaxGreen, MaxBlue ; 
	unsigned int Color ;
	int Flag ;
	DWORD drawz ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画の準備
	Graphics_D3D9_BeginScene() ;
	Flag = DX_D3D9_DRAWPREP_DIFFUSERGB ;
	DX_D3D9_DRAWPREP_NOTEX( Flag )

	// Ｚバッファに書き込むＺ値をセットする
	drawz = *((DWORD *)&GSYS.DrawSetting.DrawZ);

	// ベクトル基本ステータスを取得
	ColorData = *( ( COLORDATA * )NS_GetDispColorData() ) ;
	
	MaxRed		= ( 1 << ColorData.RedWidth	) - 1 ; 
	MaxGreen	= ( 1 << ColorData.GreenWidth ) - 1 ;
	MaxBlue		= ( 1 << ColorData.BlueWidth ) - 1 ; 

	j = 0 ;
	Vert = VertData ;

	// FVF の設定
//	Graphics_D3D9_DeviceState_SetFVF( VERTEXFVF_NOTEX_2D ) ;
	Graphics_Hardware_D3D9_SetFVF_UseTable( VERTEXFVF_DECL_NOTEX_2D ) ;

	for( i = 0 ; i < ( DWORD )Num ; i ++, LineData ++ )
	{
		// 色その他ステータスのセット
		Color = LineData->color ;
		Vert[0].color = ( LineData->pal << 24 ) |
						( ( ( ( ( ( Color & ColorData.RedMask	) >> ColorData.RedLoc	) << 8 ) - 1 ) / MaxRed		) << 16 ) |
						( ( ( ( ( ( Color & ColorData.GreenMask	) >> ColorData.GreenLoc	) << 8 ) - 1 ) / MaxGreen	) << 8 ) |
						( ( ( ( ( ( Color & ColorData.BlueMask	) >> ColorData.BlueLoc	) << 8 ) - 1 ) / MaxBlue	) ) ; 
		Vert[1].color = Vert[0].color ;
		Vert[0].rhw	  =
		Vert[1].rhw   = 1.0f ;
		*((DWORD *)&Vert[0].pos.z) = drawz ;
		*((DWORD *)&Vert[1].pos.z) = drawz ;
 
		// 座標のセット＆ウインドウ直接描画時補正
		Vert[0].pos.x = ( float )LineData->x1 ;
		Vert[0].pos.y = ( float )LineData->y1 ;
		Vert[1].pos.x = ( float )LineData->x2 ;
		Vert[1].pos.y = ( float )LineData->y2 ;

		j ++ ;
		Vert += 2 ;
		if( j * 2 == SetDrawNum )
		{
			// 描画
			Direct3DDevice9_DrawPrimitiveUP( D_D3DPT_LINELIST, j, VertData, sizeof( VERTEX_NOTEX_2D ) ) ;
			GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;
			j = 0 ;
			Vert = VertData ;
		}
	}

	if( j != 0 )
	{
		// 描画
		Direct3DDevice9_DrawPrimitiveUP( D_D3DPT_LINELIST, j, VertData, sizeof( VERTEX_NOTEX_2D ) ) ;
		GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;
	}

	// 終了
	return 0;
}
















extern	int		Graphics_D3D9_DrawPrimitive( const VERTEX_3D *Vertex, int VertexNum, int PrimitiveType, IMAGEDATA *Image, int TransFlag )
{
	int Flag ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画の準備
	Graphics_D3D9_BeginScene() ;
	Graphics_D3D9_DrawSetting_SetBlendTexture( NULL ) ;

	// 描画フラグのセット
	Flag = TransFlag | DX_D3D9_DRAWPREP_GOURAUDSHADE | DX_D3D9_DRAWPREP_PERSPECTIVE | DX_D3D9_DRAWPREP_DIFFUSERGB | DX_D3D9_DRAWPREP_DIFFUSEALPHA | DX_D3D9_DRAWPREP_FOG | DX_D3D9_DRAWPREP_3D | DX_D3D9_DRAWPREP_TEXADDRESS | DX_D3D9_DRAWPREP_CULLING ;

	// 描画の準備
	if( Image )
	{
		DX_D3D9_DRAWPREP_TEX( Image->Orig, Image->Hard.Draw[ 0 ].Tex->PF->D3D9.Texture, Flag )
	}
	else
	{
		DX_D3D9_DRAWPREP_NOTEX( Flag )
	}

	// 描画
//	Graphics_D3D9_DeviceState_SetFVF( VERTEXFVF_3D ) ;
	Graphics_Hardware_D3D9_SetFVF_UseTable( VERTEXFVF_DECL_3D ) ;
	Direct3DDevice9_DrawPrimitiveUP( ( D_D3DPRIMITIVETYPE )PrimitiveType, GETPRIMNUM( PrimitiveType, VertexNum ), Vertex, sizeof( VERTEX_3D ) ) ;
	GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;

	// 終了
	return 0 ;
}

extern	int		Graphics_D3D9_DrawIndexedPrimitive( const VERTEX_3D *Vertex, int VertexNum, const WORD *Indices, int IndexNum, int PrimitiveType, IMAGEDATA *Image, int TransFlag )
{
	int Flag ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画の準備
	Graphics_D3D9_BeginScene() ;
	Graphics_D3D9_DrawSetting_SetBlendTexture( NULL ) ;

	Flag = TransFlag | DX_D3D9_DRAWPREP_GOURAUDSHADE | DX_D3D9_DRAWPREP_PERSPECTIVE | DX_D3D9_DRAWPREP_DIFFUSERGB | DX_D3D9_DRAWPREP_DIFFUSEALPHA | DX_D3D9_DRAWPREP_FOG | DX_D3D9_DRAWPREP_3D | DX_D3D9_DRAWPREP_TEXADDRESS | DX_D3D9_DRAWPREP_CULLING ;
	if( Image )
	{
		DX_D3D9_DRAWPREP_TEX( Image->Orig, Image->Hard.Draw[ 0 ].Tex->PF->D3D9.Texture, Flag )
	}
	else
	{
		DX_D3D9_DRAWPREP_NOTEX( Flag )
	}

	// 描画
//	Graphics_D3D9_DeviceState_SetFVF( VERTEXFVF_3D ) ;
	Graphics_Hardware_D3D9_SetFVF_UseTable( VERTEXFVF_DECL_3D ) ;
	Direct3DDevice9_DrawIndexedPrimitiveUP( ( D_D3DPRIMITIVETYPE )PrimitiveType, 0, ( UINT )VertexNum, GETPRIMNUM( PrimitiveType, IndexNum ), Indices, D_D3DFMT_INDEX16, Vertex, sizeof( VERTEX_3D ) ) ;
	GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;

	// 終了
	return 0 ;
}

extern	int		Graphics_D3D9_DrawPrimitiveLight( const VERTEX3D *Vertex, int VertexNum, int PrimitiveType, IMAGEDATA *Image, int TransFlag )
{
	int Flag ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画の準備
	Graphics_D3D9_BeginScene() ;
	Graphics_D3D9_DrawSetting_SetBlendTexture( NULL ) ;

	Flag = TransFlag | DX_D3D9_DRAWPREP_GOURAUDSHADE | DX_D3D9_DRAWPREP_PERSPECTIVE | DX_D3D9_DRAWPREP_DIFFUSERGB | DX_D3D9_DRAWPREP_DIFFUSEALPHA | DX_D3D9_DRAWPREP_FOG | DX_D3D9_DRAWPREP_LIGHTING | DX_D3D9_DRAWPREP_SPECULAR | DX_D3D9_DRAWPREP_3D | DX_D3D9_DRAWPREP_TEXADDRESS | DX_D3D9_DRAWPREP_CULLING ;
	if( Image )
	{
		DX_D3D9_DRAWPREP_TEX( Image->Orig, Image->Hard.Draw[ 0 ].Tex->PF->D3D9.Texture, Flag )
	}
	else
	{
		int WhiteHandle ;

		WhiteHandle = Graphics_Image_GetWhiteTexHandle() ;
		if( GRAPHCHK( WhiteHandle, Image ) )
		{
			return -1 ;
		}

//		DX_D3D9_DRAWPREP_NOTEX( Flag )
		DX_D3D9_DRAWPREP_TEX( Image->Orig, Image->Hard.Draw[ 0 ].Tex->PF->D3D9.Texture, Flag )
	}

	// 描画
//	Graphics_D3D9_DeviceState_SetFVF( VERTEXFVF_3D_LIGHT ) ;
	if( GSYS.DrawSetting.UseShadowMapNum > 0 ||
		GSYS.DrawSetting.ShadowMapDraw )
	{
		Graphics_D3D9_DeviceState_SetVertexDeclaration( GD3D9.Device.VertexDeclaration.Base2DDeclaration[ VERTEXFVF_DECL_3D_LIGHT ] ) ;
		Graphics_D3D9_Shader_Normal3DDraw_Setup() ;
	}
	else
	{
		Graphics_Hardware_D3D9_SetFVF_UseTable( VERTEXFVF_DECL_3D_LIGHT ) ;
	}
	Direct3DDevice9_DrawPrimitiveUP( ( D_D3DPRIMITIVETYPE )PrimitiveType, GETPRIMNUM( PrimitiveType, VertexNum ), Vertex, sizeof( VERTEX3D ) ) ;
	GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;

	// 終了
	return 0 ;
}

extern	int		Graphics_D3D9_DrawIndexedPrimitiveLight( const VERTEX3D *Vertex, int VertexNum, const WORD *Indices, int IndexNum, int PrimitiveType, IMAGEDATA *Image, int TransFlag )
{
	int Flag ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画の準備
	Graphics_D3D9_BeginScene() ;
	Graphics_D3D9_DrawSetting_SetBlendTexture( NULL ) ;

	Flag = TransFlag | DX_D3D9_DRAWPREP_GOURAUDSHADE | DX_D3D9_DRAWPREP_PERSPECTIVE | DX_D3D9_DRAWPREP_DIFFUSERGB | DX_D3D9_DRAWPREP_DIFFUSEALPHA | DX_D3D9_DRAWPREP_FOG | DX_D3D9_DRAWPREP_LIGHTING | DX_D3D9_DRAWPREP_SPECULAR | DX_D3D9_DRAWPREP_3D | DX_D3D9_DRAWPREP_TEXADDRESS | DX_D3D9_DRAWPREP_CULLING ;
	if( Image )
	{
		DX_D3D9_DRAWPREP_TEX( Image->Orig, Image->Hard.Draw[ 0 ].Tex->PF->D3D9.Texture, Flag )
	}
	else
	{
		int WhiteHandle ;

		WhiteHandle = Graphics_Image_GetWhiteTexHandle() ;
		if( GRAPHCHK( WhiteHandle, Image ) )
		{
			return -1 ;
		}

//		DX_D3D9_DRAWPREP_NOTEX( Flag )
		DX_D3D9_DRAWPREP_TEX( Image->Orig, Image->Hard.Draw[ 0 ].Tex->PF->D3D9.Texture, Flag )
	}

	// 描画
//	Graphics_D3D9_DeviceState_SetFVF( VERTEXFVF_3D_LIGHT ) ;
	if( GSYS.DrawSetting.UseShadowMapNum > 0 ||
		GSYS.DrawSetting.ShadowMapDraw )
	{
		Graphics_D3D9_DeviceState_SetVertexDeclaration( GD3D9.Device.VertexDeclaration.Base2DDeclaration[ VERTEXFVF_DECL_3D_LIGHT ] ) ;
		Graphics_D3D9_Shader_Normal3DDraw_Setup() ;
	}
	else
	{
		Graphics_Hardware_D3D9_SetFVF_UseTable( VERTEXFVF_DECL_3D_LIGHT ) ;
	}
	Direct3DDevice9_DrawIndexedPrimitiveUP( ( D_D3DPRIMITIVETYPE )PrimitiveType, 0, ( UINT )VertexNum, GETPRIMNUM( PrimitiveType, IndexNum ), Indices, D_D3DFMT_INDEX16, Vertex, sizeof( VERTEX3D ) ) ;
	GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;

	// 終了
	return 0 ;
}

extern	int		Graphics_D3D9_DrawPrimitiveLight_UseVertexBuffer( 
	VERTEXBUFFERHANDLEDATA *VertexBuffer,
	int						PrimitiveType,
	int						StartVertex,
	int						UseVertexNum,
	IMAGEDATA *				Image,
	int						TransFlag 
)
{
	int Flag ;
	DWORD PrimitiveCount ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画の準備
	Graphics_D3D9_BeginScene() ;
	Graphics_D3D9_DrawSetting_SetBlendTexture( NULL ) ;

	Flag = TransFlag | DX_D3D9_DRAWPREP_GOURAUDSHADE | DX_D3D9_DRAWPREP_PERSPECTIVE | DX_D3D9_DRAWPREP_DIFFUSERGB | DX_D3D9_DRAWPREP_DIFFUSEALPHA | DX_D3D9_DRAWPREP_FOG | DX_D3D9_DRAWPREP_LIGHTING | DX_D3D9_DRAWPREP_SPECULAR | DX_D3D9_DRAWPREP_3D | DX_D3D9_DRAWPREP_TEXADDRESS | DX_D3D9_DRAWPREP_CULLING ;
	if( Image )
	{
		DX_D3D9_DRAWPREP_TEX( Image->Orig, Image->Hard.Draw[ 0 ].Tex->PF->D3D9.Texture, Flag )
	}
	else
	{
		int WhiteHandle ;

		WhiteHandle = Graphics_Image_GetWhiteTexHandle() ;
		if( GRAPHCHK( WhiteHandle, Image ) )
		{
			return -1 ;
		}

//		DX_D3D9_DRAWPREP_NOTEX( Flag )
		DX_D3D9_DRAWPREP_TEX( Image->Orig, Image->Hard.Draw[ 0 ].Tex->PF->D3D9.Texture, Flag )
	}

	// FVFのセットアップ
	switch( VertexBuffer->Type )
	{
	case DX_VERTEX_TYPE_NORMAL_3D:
//		Graphics_D3D9_DeviceState_SetFVF( VERTEXFVF_3D_LIGHT ) ;
		if( GSYS.DrawSetting.UseShadowMapNum > 0 ||
			GSYS.DrawSetting.ShadowMapDraw )
		{
			Graphics_D3D9_DeviceState_SetVertexDeclaration( GD3D9.Device.VertexDeclaration.Base2DDeclaration[ VERTEXFVF_DECL_3D_LIGHT ] ) ;
			Graphics_D3D9_Shader_Normal3DDraw_Setup() ;
		}
		else
		{
			Graphics_Hardware_D3D9_SetFVF_UseTable( VERTEXFVF_DECL_3D_LIGHT ) ;
		}
		break;
	}

	// プリミティブの数を取得
	PrimitiveCount = GETPRIMNUM( PrimitiveType, UseVertexNum ) ;

	// 描画
	if( VertexBuffer->PF->D3D9.VertexBuffer != NULL )
	{
		Graphics_D3D9_DeviceState_SetVertexBuffer( VertexBuffer->PF->D3D9.VertexBuffer, VertexBuffer->UnitSize ) ;
		Direct3DDevice9_DrawPrimitive(
			( D_D3DPRIMITIVETYPE )PrimitiveType,
			( UINT )StartVertex,
			PrimitiveCount ) ;
		GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;
	}
	else
	{
		Direct3DDevice9_DrawPrimitiveUP(
			( D_D3DPRIMITIVETYPE )PrimitiveType,
			PrimitiveCount,
			( BYTE * )VertexBuffer->Buffer + StartVertex * VertexBuffer->UnitSize,
			( UINT )VertexBuffer->UnitSize ) ;
		GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;
	}

	// 終了
	return 0 ;
}

extern	int		Graphics_D3D9_DrawIndexedPrimitiveLight_UseVertexBuffer(
	VERTEXBUFFERHANDLEDATA *	VertexBuffer,
	INDEXBUFFERHANDLEDATA *		IndexBuffer,
	int							PrimitiveType,
	int							BaseVertex,
	int							StartVertex,
	int							UseVertexNum,
	int							StartIndex,
	int							UseIndexNum,
	IMAGEDATA *				Image,
	int							TransFlag
)
{
	int Flag ;
	DWORD PrimitiveCount ;
	D_D3DFORMAT IndexFormat ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画の準備
	Graphics_D3D9_BeginScene() ;
	Graphics_D3D9_DrawSetting_SetBlendTexture( NULL ) ;

	Flag = TransFlag | DX_D3D9_DRAWPREP_GOURAUDSHADE | DX_D3D9_DRAWPREP_PERSPECTIVE | DX_D3D9_DRAWPREP_DIFFUSERGB | DX_D3D9_DRAWPREP_DIFFUSEALPHA | DX_D3D9_DRAWPREP_FOG | DX_D3D9_DRAWPREP_LIGHTING | DX_D3D9_DRAWPREP_SPECULAR | DX_D3D9_DRAWPREP_3D | DX_D3D9_DRAWPREP_TEXADDRESS | DX_D3D9_DRAWPREP_CULLING ;
	if( Image )
	{
		DX_D3D9_DRAWPREP_TEX( Image->Orig, Image->Hard.Draw[ 0 ].Tex->PF->D3D9.Texture, Flag )
	}
	else
	{
		int WhiteHandle ;

		WhiteHandle = Graphics_Image_GetWhiteTexHandle() ;
		if( GRAPHCHK( WhiteHandle, Image ) )
		{
			return -1 ;
		}

//		DX_D3D9_DRAWPREP_NOTEX( Flag )
		DX_D3D9_DRAWPREP_TEX( Image->Orig, Image->Hard.Draw[ 0 ].Tex->PF->D3D9.Texture, Flag )
	}

	// FVFのセットアップ
	switch( VertexBuffer->Type )
	{
	case DX_VERTEX_TYPE_NORMAL_3D:
//		Graphics_D3D9_DeviceState_SetFVF( VERTEXFVF_3D_LIGHT ) ;
		if( GSYS.DrawSetting.UseShadowMapNum > 0 ||
			GSYS.DrawSetting.ShadowMapDraw )
		{
			Graphics_D3D9_DeviceState_SetVertexDeclaration( GD3D9.Device.VertexDeclaration.Base2DDeclaration[ VERTEXFVF_DECL_3D_LIGHT ] ) ;
			Graphics_D3D9_Shader_Normal3DDraw_Setup() ;
		}
		else
		{
			Graphics_Hardware_D3D9_SetFVF_UseTable( VERTEXFVF_DECL_3D_LIGHT ) ;
		}
		break ;
	}

	// インデックスタイプをセット
	IndexFormat = D_D3DFMT_INDEX16 ;
	switch( IndexBuffer->Type )
	{
	case DX_INDEX_TYPE_16BIT:
		IndexFormat = D_D3DFMT_INDEX16 ;
		break ;

	case DX_INDEX_TYPE_32BIT:
		IndexFormat = D_D3DFMT_INDEX32 ;
		break ;
	}

	// プリミティブの数を取得
	PrimitiveCount = GETPRIMNUM( PrimitiveType, UseIndexNum ) ;

	// 描画
	if( VertexBuffer->PF->D3D9.VertexBuffer != NULL &&
		IndexBuffer->PF->D3D9.IndexBuffer  != NULL )
	{
		Graphics_D3D9_DeviceState_SetVertexBuffer( VertexBuffer->PF->D3D9.VertexBuffer, VertexBuffer->UnitSize ) ;
		Graphics_D3D9_DeviceState_SetIndexBuffer( IndexBuffer->PF->D3D9.IndexBuffer ) ;

		Direct3DDevice9_DrawIndexedPrimitive(
			( D_D3DPRIMITIVETYPE )PrimitiveType,
			BaseVertex,
			( UINT )StartVertex,
			( UINT )UseVertexNum,
			( UINT )StartIndex,
			PrimitiveCount ) ;
		GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;
	}
	else
	{
		Direct3DDevice9_DrawIndexedPrimitiveUP(
			( D_D3DPRIMITIVETYPE )PrimitiveType,
			( UINT )StartVertex,
			( UINT )UseVertexNum,
			PrimitiveCount,
			( BYTE * )IndexBuffer->Buffer + StartIndex * IndexBuffer->UnitSize,
			IndexFormat, 
			( BYTE * )VertexBuffer->Buffer + BaseVertex * VertexBuffer->UnitSize,
			( UINT )VertexBuffer->UnitSize ) ;
		GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;
	}

	// 終了
	return 0 ;
}

extern	int		Graphics_D3D9_DrawPrimitive2D( VERTEX_2D *Vertex, int VertexNum, int PrimitiveType, IMAGEDATA *Image, int TransFlag, int BillboardFlag, int Is3D, int ReverseXFlag, int ReverseYFlag, int TextureNo, int IsShadowMap )
{
	int Flag ;
	int i ;
	VERTEX_2D *Vert ;
	int SmHandle ;
	SHADOWMAPDATA *ShadowMap ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画の準備
	Graphics_D3D9_BeginScene() ;
	Graphics_D3D9_DrawSetting_SetBlendTexture( NULL ) ;

	if( BillboardFlag == TRUE )
	{
		Flag = TransFlag | ( Is3D ? DX_D3D9_DRAWPREP_3D | DX_D3D9_DRAWPREP_FOG : 0 ) | DX_D3D9_DRAWPREP_TEXADDRESS ;
	}
	else
	{
		Flag = TransFlag | ( Is3D ? DX_D3D9_DRAWPREP_3D | DX_D3D9_DRAWPREP_FOG : 0 ) | DX_D3D9_DRAWPREP_GOURAUDSHADE | DX_D3D9_DRAWPREP_PERSPECTIVE | DX_D3D9_DRAWPREP_DIFFUSERGB | DX_D3D9_DRAWPREP_DIFFUSEALPHA | DX_D3D9_DRAWPREP_TEXADDRESS | DX_D3D9_DRAWPREP_CULLING ;
	}
	if( IsShadowMap )
	{
		SmHandle = ( int )( DWORD_PTR )Image ;
		if( !SHADOWMAPCHKFULL( SmHandle, ShadowMap ) )
		{
			Flag |= DX_D3D9_DRAWPREP_TEXTURE ;
			Graphics_D3D9_DrawSetting_SetTexture( ShadowMap->PF->D3D9.Texture ) ;
			if( GSYS.ChangeSettingFlag ||
				GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag != FALSE ||
				GD3D9.Device.DrawSetting.DrawPrepParamFlag != Flag )
			{
				Graphics_D3D9_DrawPreparation( Flag ) ;
			}
		}
	}
	else
	if( Image )
	{
		DX_D3D9_DRAWPREP_TEX( Image->Orig, Image->Hard.Draw[ TextureNo ].Tex->PF->D3D9.Texture, Flag )
	}
	else
	{
		DX_D3D9_DRAWPREP_NOTEX( Flag )
	}

	// ビルボードの場合はＵＶ値をサーフェスデータから取得する
	if( BillboardFlag == TRUE )
	{
		float tu1, tv1, tu2, tv2 ;
		int Num ;
		
		Vert = Vertex ;
		Num = VertexNum / 6 ;
		tu1 = Image->Hard.Draw[ 0 ].Vertex[ 0 ].u ;
		tv1 = Image->Hard.Draw[ 0 ].Vertex[ 0 ].v ;
		tu2 = Image->Hard.Draw[ 0 ].Vertex[ 3 ].u ;
		tv2 = Image->Hard.Draw[ 0 ].Vertex[ 3 ].v ;
		for( i = 0 ; i < Num ; i ++, Vert += 6 )
		{
			if( ReverseXFlag )
			{
				Vert[ 0 ].u = tu2 ;
				Vert[ 1 ].u = tu1 ;
				Vert[ 2 ].u = tu2 ;
				Vert[ 3 ].u = tu1 ;
			}
			else
			{
				Vert[ 0 ].u = tu1 ;
				Vert[ 1 ].u = tu2 ;
				Vert[ 2 ].u = tu1 ;
				Vert[ 3 ].u = tu2 ;
			}
			if( ReverseYFlag )
			{
				Vert[ 0 ].v = tv2 ;
				Vert[ 1 ].v = tv2 ;
				Vert[ 2 ].v = tv1 ;
				Vert[ 3 ].v = tv1 ;
			}
			else
			{
				Vert[ 0 ].v = tv1 ;
				Vert[ 1 ].v = tv1 ;
				Vert[ 2 ].v = tv2 ;
				Vert[ 3 ].v = tv2 ;
			}

			Vert[ 4 ].u = Vert[ 2 ].u ;
			Vert[ 4 ].v = Vert[ 2 ].v ;
			Vert[ 5 ].u = Vert[ 1 ].u ;
			Vert[ 5 ].v = Vert[ 1 ].v ;
		}
	}

	// 描画
//	Graphics_D3D9_DeviceState_SetFVF( VERTEXFVF_2D ) ;
	Graphics_Hardware_D3D9_SetFVF_UseTable( VERTEXFVF_DECL_2D ) ;
	Direct3DDevice9_DrawPrimitiveUP( ( D_D3DPRIMITIVETYPE )PrimitiveType, GETPRIMNUM( PrimitiveType, VertexNum ), Vertex, sizeof( VERTEX_2D ) ) ;
	GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;

	// 終了
	return 0 ;
}

extern	int		Graphics_D3D9_DrawPrimitive2DUser( const VERTEX2D *Vertex, int VertexNum, int PrimitiveType, IMAGEDATA *Image, int TransFlag, int Is3D, int /* ReverseXFlag */, int /* ReverseYFlag */, int TextureNo )
{
	int Flag ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画の準備
	Graphics_D3D9_BeginScene() ;
	Graphics_D3D9_DrawSetting_SetBlendTexture( NULL ) ;

	Flag = TransFlag | ( Is3D ? DX_D3D9_DRAWPREP_3D | DX_D3D9_DRAWPREP_FOG : 0 ) | DX_D3D9_DRAWPREP_GOURAUDSHADE | DX_D3D9_DRAWPREP_PERSPECTIVE | DX_D3D9_DRAWPREP_DIFFUSERGB | DX_D3D9_DRAWPREP_DIFFUSEALPHA | DX_D3D9_DRAWPREP_TEXADDRESS | DX_D3D9_DRAWPREP_CULLING ;
	if( Image )
	{
		DX_D3D9_DRAWPREP_TEX( Image->Orig, Image->Hard.Draw[ TextureNo ].Tex->PF->D3D9.Texture, Flag )
	}
	else
	{
		DX_D3D9_DRAWPREP_NOTEX( Flag )
	}

	// 描画
//	Graphics_D3D9_DeviceState_SetFVF( VERTEXFVF_2D_USER ) ;
	Graphics_Hardware_D3D9_SetFVF_UseTable( VERTEXFVF_DECL_2D_USER ) ;
	Direct3DDevice9_DrawPrimitiveUP( ( D_D3DPRIMITIVETYPE )PrimitiveType, GETPRIMNUM( PrimitiveType, VertexNum ), Vertex, sizeof( VERTEX2D ) ) ;
	GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;

	// 終了
	return 0 ;
}

extern	int		Graphics_D3D9_DrawIndexedPrimitive2DUser( const VERTEX2D *Vertex, int VertexNum, const WORD *Indices, int IndexNum, int PrimitiveType, IMAGEDATA *Image, int TransFlag )
{
	int Flag ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画の準備
	Graphics_D3D9_BeginScene() ;
	Graphics_D3D9_DrawSetting_SetBlendTexture( NULL ) ;

	Flag = TransFlag | DX_D3D9_DRAWPREP_GOURAUDSHADE | DX_D3D9_DRAWPREP_PERSPECTIVE | DX_D3D9_DRAWPREP_DIFFUSERGB | DX_D3D9_DRAWPREP_DIFFUSEALPHA | DX_D3D9_DRAWPREP_TEXADDRESS | DX_D3D9_DRAWPREP_CULLING ;
	if( Image )
	{
		DX_D3D9_DRAWPREP_TEX( Image->Orig, Image->Hard.Draw[ 0 ].Tex->PF->D3D9.Texture, Flag )
	}
	else
	{
		DX_D3D9_DRAWPREP_NOTEX( Flag )
	}

	// 描画
//	Graphics_D3D9_DeviceState_SetFVF( VERTEXFVF_2D_USER ) ;
	Graphics_Hardware_D3D9_SetFVF_UseTable( VERTEXFVF_DECL_2D_USER ) ;
	Direct3DDevice9_DrawIndexedPrimitiveUP( ( D_D3DPRIMITIVETYPE )PrimitiveType, 0, ( UINT )VertexNum, GETPRIMNUM( PrimitiveType, IndexNum ), Indices, D_D3DFMT_INDEX16, Vertex, sizeof( VERTEX2D ) ) ;
	GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;

	// 終了
	return 0 ;
}












// シェーダー描画用描画前セットアップ関数
extern void Graphics_D3D9_DrawPreparationToShader( int ParamFlag, int UseVertexShaderNo, int Is2D, int UseLibSubShaderConst )
{
	GRAPHICS_HARDDATA_DIRECT3D9_BLENDINFO BlendInfo ;
	int i, Flag ;

	// 基本的なセットアップ処理
	Flag = ParamFlag | DX_D3D9_DRAWPREP_SPECULAR | DX_D3D9_DRAWPREP_NOTSHADERRESET | DX_D3D9_DRAWPREP_TEXADDRESS | DX_D3D9_DRAWPREP_NOBLENDSETTING | DX_D3D9_DRAWPREP_CULLING ;
	DX_D3D9_DRAWPREP_NOTEX( Flag )

	// ユーザー設定のシェーダー定数を有効にする
	if( UseLibSubShaderConst )
	{
		Graphics_D3D9_ShaderConstant_InfoSet_SetUseState( &GD3D9.Device.Shader.ShaderConstantInfo, DX_SHADERCONSTANTSET_MASK_LIB | DX_SHADERCONSTANTSET_MASK_LIB_SUB ) ;
	}
	else
	{
		Graphics_D3D9_ShaderConstant_InfoSet_SetUseState( &GD3D9.Device.Shader.ShaderConstantInfo, DX_SHADERCONSTANTSET_MASK_LIB | DX_SHADERCONSTANTSET_MASK_USER ) ;
	}

	// 頂点フォーマットはリセット
	Graphics_D3D9_DeviceState_SetVertexDeclaration( NULL ) ;

	// ブレンドモードは自前でセットアップ
	{
		BlendInfo.SeparateAlphaBlendEnable = FALSE ;
		BlendInfo.SrcBlendAlpha            = -1 ;
		BlendInfo.DestBlendAlpha           = -1 ;
		BlendInfo.BlendOpAlpha             = -1 ;

		// アルファテストパラメータが有効な場合はそれを優先する
		if( GSYS.DrawSetting.AlphaTestMode != -1 )
		{
			BlendInfo.AlphaTestEnable = TRUE ;
			BlendInfo.AlphaFunc       = GSYS.DrawSetting.AlphaTestMode ;
			BlendInfo.AlphaRef        = GSYS.DrawSetting.AlphaTestParam ;
		}
		else
		{
			// デフォルトのアルファテストの設定をセット
			BlendInfo.AlphaTestEnable = TRUE ;
			BlendInfo.AlphaRef        = 0 ;
			BlendInfo.AlphaFunc       = D_D3DCMP_GREATER ;
		}

		// 出力先とのブレンド方式をセット
		BlendInfo.AlphaBlendEnable = GSYS.DrawSetting.BlendMode != DX_BLENDMODE_NOBLEND && GSYS.DrawSetting.BlendMode != DX_BLENDMODE_XOR ;
		BlendInfo.DestBlend        = __RGBBlendStateTable[ GSYS.DrawSetting.BlendMode ][ RGB_BLENDSTATE_TABLE_DESTBLEND ] ;
		BlendInfo.SrcBlend         = __RGBBlendStateTable[ GSYS.DrawSetting.BlendMode ][ RGB_BLENDSTATE_TABLE_SRCBLEND ] ;
		BlendInfo.BlendOp          = __RGBBlendStateTable[ GSYS.DrawSetting.BlendMode ][ RGB_BLENDSTATE_TABLE_BLENDOP ] ;
		BlendInfo.FactorColor      = 0xffffffff ;

		// 減算ブレンド指定で D3DRS_BLENDOP が使用可能な場合は 14番目のブレンドモードを使用する
		if( GSYS.DrawSetting.BlendMode == DX_BLENDMODE_SUB && GD3D9.Device.Caps.ValidDestBlendOp )
		{
			BlendInfo.DestBlend   = __RGBBlendStateTable[ DX_BLENDMODE_SUB1 ][ RGB_BLENDSTATE_TABLE_DESTBLEND ] ;
			BlendInfo.SrcBlend    = __RGBBlendStateTable[ DX_BLENDMODE_SUB1 ][ RGB_BLENDSTATE_TABLE_SRCBLEND ] ;
			BlendInfo.BlendOp     = __RGBBlendStateTable[ DX_BLENDMODE_SUB1 ][ RGB_BLENDSTATE_TABLE_BLENDOP ] ;
			BlendInfo.FactorColor = 0xffffffff ;
		}

		// 使用するテクスチャを列挙
		BlendInfo.UseTextureStageNum = 0 ;
		for( i = 0 ; i < USE_TEXTURESTAGE_NUM ; i ++ )
		{
			BlendInfo.TextureStageInfo[ i ].Texture = ( void * )( DWORD_PTR )GSYS.DrawSetting.UserShaderRenderInfo.SetTextureGraphHandle[ i ] ;
			BlendInfo.TextureStageInfo[ i ].TextureCoordIndex = i ;
			if( GSYS.DrawSetting.UserShaderRenderInfo.SetTextureGraphHandle[ i ] == 0 ) continue ;
			BlendInfo.UseTextureStageNum = i + 1 ;
		}

		// 設定を反映
		Graphics_D3D9_DeviceState_SetUserBlendInfo( &BlendInfo, TRUE, TRUE ) ;

		// 使用するテクスチャ座標をリセット
		Graphics_D3D9_DeviceState_ResetTextureCoord() ;
	}

	// 頂点シェーダーを使用するかどうかで処理を分岐
	if( UseVertexShaderNo >= 0 )
	{
		// 使用する頂点データフォーマットを更新
		Graphics_D3D9_DeviceState_SetVertexDeclaration( GD3D9.Device.VertexDeclaration.UserShaderDeclaration[ UseVertexShaderNo ] ) ;

		// 使用する頂点シェーダーを更新
		Graphics_D3D9_DeviceState_SetVertexShaderToHandle( GSYS.DrawSetting.UserShaderRenderInfo.SetVertexShaderHandle ) ;
	}
	else
	{
		// 使用しない場合は頂点シェーダーをリセットする
		Graphics_D3D9_DeviceState_ResetVertexShader( TRUE ) ;

		// 頂点データ形式をセット
		if( Is2D )
		{
//			Graphics_D3D9_DeviceState_SetFVF( VERTEXFVF_SHADER_2D ) ;
			Graphics_Hardware_D3D9_SetFVF_UseTable( VERTEXFVF_DECL_SHADER_2D ) ;
		}
		else
		{
//			Graphics_D3D9_DeviceState_SetFVF( VERTEXFVF_SHADER_3D ) ;
			Graphics_Hardware_D3D9_SetFVF_UseTable( VERTEXFVF_DECL_SHADER_3D ) ;
		}
	}

	// 使用するピクセルシェーダーを更新
	Graphics_D3D9_DeviceState_SetPixelShaderToHandle( GSYS.DrawSetting.UserShaderRenderInfo.SetPixelShaderHandle ) ;
}

// シェーダーを使って２Ｄプリミティブを描画する
extern	int		Graphics_D3D9_DrawPrimitive2DToShader(        const VERTEX2DSHADER *Vertex, int VertexNum,                                              int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;
	if( GSYS.HardInfo.UseShader == FALSE ) return -1 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画の準備
	Graphics_D3D9_BeginScene() ;
	Graphics_D3D9_DrawPreparationToShader( DX_D3D9_DRAWPREP_GOURAUDSHADE | DX_D3D9_DRAWPREP_PERSPECTIVE, -1, TRUE, FALSE ) ;

	// 描画
	Direct3DDevice9_DrawPrimitiveUP( ( D_D3DPRIMITIVETYPE )PrimitiveType, GETPRIMNUM( PrimitiveType, VertexNum ), Vertex, sizeof( VERTEX2DSHADER ) ) ;
	GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;

	// 終了
	return 0 ;
}

// シェーダーを使って３Ｄプリミティブを描画する
extern	int		Graphics_D3D9_DrawPrimitive3DToShader(        const VERTEX3DSHADER *Vertex, int VertexNum,                                              int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;
	if( GSYS.HardInfo.UseShader == FALSE ) return -1 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画の準備
	Graphics_D3D9_BeginScene() ;
	Graphics_D3D9_DrawPreparationToShader( DX_D3D9_DRAWPREP_3D | DX_D3D9_DRAWPREP_GOURAUDSHADE | DX_D3D9_DRAWPREP_FOG | DX_D3D9_DRAWPREP_PERSPECTIVE | DX_D3D9_DRAWPREP_LIGHTING, DX_VERTEX_TYPE_SHADER_3D, FALSE, FALSE ) ;

	// ３Ｄ行列をハードウエアに反映する
	if( GSYS.DrawSetting.MatchHardware3DMatrix == FALSE )
		Graphics_DrawSetting_ApplyLib3DMatrixToHardware() ;

	// 描画
	Direct3DDevice9_DrawPrimitiveUP( ( D_D3DPRIMITIVETYPE )PrimitiveType, GETPRIMNUM( PrimitiveType, VertexNum ), Vertex, sizeof( VERTEX3DSHADER ) ) ;
	GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;

	// 終了
	return 0 ;
}

// シェーダーを使って２Ｄプリミティブを描画する( 頂点インデックスを使用する )
extern	int		Graphics_D3D9_DrawPrimitiveIndexed2DToShader( const VERTEX2DSHADER *Vertex, int VertexNum, const unsigned short *Indices, int IndexNum, int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;
	if( GSYS.HardInfo.UseShader == FALSE ) return -1 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画の準備
	Graphics_D3D9_BeginScene() ;
	Graphics_D3D9_DrawPreparationToShader( DX_D3D9_DRAWPREP_GOURAUDSHADE | DX_D3D9_DRAWPREP_PERSPECTIVE, -1, TRUE, FALSE ) ;

	// 描画
	Direct3DDevice9_DrawIndexedPrimitiveUP( ( D_D3DPRIMITIVETYPE )PrimitiveType, 0, ( UINT )VertexNum, GETPRIMNUM( PrimitiveType, IndexNum ), Indices, D_D3DFMT_INDEX16, Vertex, sizeof( VERTEX2DSHADER ) ) ;
	GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;

	// 終了
	return 0 ;
}

// シェーダーを使って３Ｄプリミティブを描画する( 頂点インデックスを使用する )
extern	int		Graphics_D3D9_DrawPrimitiveIndexed3DToShader( const VERTEX3DSHADER *Vertex, int VertexNum, const unsigned short *Indices, int IndexNum, int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;
	if( GSYS.HardInfo.UseShader == FALSE ) return -1 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画の準備
	Graphics_D3D9_BeginScene() ;
	Graphics_D3D9_DrawPreparationToShader( DX_D3D9_DRAWPREP_3D | DX_D3D9_DRAWPREP_GOURAUDSHADE | DX_D3D9_DRAWPREP_FOG | DX_D3D9_DRAWPREP_PERSPECTIVE | DX_D3D9_DRAWPREP_LIGHTING, DX_VERTEX_TYPE_SHADER_3D, FALSE, FALSE ) ;

	// ３Ｄ行列をハードウエアに反映する
	if( GSYS.DrawSetting.MatchHardware3DMatrix == FALSE )
		Graphics_DrawSetting_ApplyLib3DMatrixToHardware() ;

	// 描画
	Direct3DDevice9_DrawIndexedPrimitiveUP( ( D_D3DPRIMITIVETYPE )PrimitiveType, 0, ( UINT )VertexNum, GETPRIMNUM( PrimitiveType, IndexNum ), Indices, D_D3DFMT_INDEX16, Vertex, sizeof( VERTEX3DSHADER ) ) ;
	GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;

	// 終了
	return 0 ;
}

// シェーダーを使って３Ｄプリミティブを描画する( 頂点バッファ使用版 )
extern	int		Graphics_D3D9_DrawPrimitive3DToShader_UseVertexBuffer2(        int VertexBufHandle,                     int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */, int StartVertex, int UseVertexNum )
{
	VERTEXBUFFERHANDLEDATA *VertexBuffer ;
	DWORD PrimitiveCount ;

	// エラー判定
	if( VERTEXBUFFERCHK( VertexBufHandle, VertexBuffer ) )
		return -1 ;

	// ３Ｄ行列をハードウエアに反映する
	if( GSYS.DrawSetting.MatchHardware3DMatrix == FALSE )
		Graphics_DrawSetting_ApplyLib3DMatrixToHardware() ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画の準備
	Graphics_D3D9_BeginScene() ;
	Graphics_D3D9_DrawPreparationToShader( DX_D3D9_DRAWPREP_3D | DX_D3D9_DRAWPREP_GOURAUDSHADE | DX_D3D9_DRAWPREP_FOG | DX_D3D9_DRAWPREP_PERSPECTIVE | DX_D3D9_DRAWPREP_LIGHTING, VertexBuffer->Type, FALSE, FALSE ) ;

	// プリミティブの数を取得
	PrimitiveCount = GETPRIMNUM( PrimitiveType, UseVertexNum ) ;

	// 描画
	if( VertexBuffer->PF->D3D9.VertexBuffer != NULL )
	{
		Graphics_D3D9_DeviceState_SetVertexBuffer( VertexBuffer->PF->D3D9.VertexBuffer, VertexBuffer->UnitSize ) ;
		Direct3DDevice9_DrawPrimitive(
			( D_D3DPRIMITIVETYPE )PrimitiveType,
			( UINT )StartVertex,
			PrimitiveCount ) ;
		GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;
	}
	else
	{
		Direct3DDevice9_DrawPrimitiveUP(
			( D_D3DPRIMITIVETYPE )PrimitiveType,
			PrimitiveCount,
			( BYTE * )VertexBuffer->Buffer + StartVertex * VertexBuffer->UnitSize,
			( UINT )VertexBuffer->UnitSize ) ;
		GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;
	}

	// 終了
	return 0 ;
}

// シェーダーを使って３Ｄプリミティブを描画する( 頂点バッファとインデックスバッファ使用版 )
extern	int		Graphics_D3D9_DrawPrimitiveIndexed3DToShader_UseVertexBuffer2( int VertexBufHandle, int IndexBufHandle, int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */, int BaseVertex, int StartVertex, int UseVertexNum, int StartIndex, int UseIndexNum )
{
	VERTEXBUFFERHANDLEDATA *VertexBuffer ;
	INDEXBUFFERHANDLEDATA *IndexBuffer ;
	DWORD PrimitiveCount ;
	D_D3DFORMAT IndexFormat ;

	// エラー判定
	if( VERTEXBUFFERCHK( VertexBufHandle, VertexBuffer ) )
		return -1 ;
	if( INDEXBUFFERCHK( IndexBufHandle, IndexBuffer ) )
		return -1 ;

	// ３Ｄ行列をハードウエアに反映する
	if( GSYS.DrawSetting.MatchHardware3DMatrix == FALSE )
		Graphics_DrawSetting_ApplyLib3DMatrixToHardware() ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画の準備
	Graphics_D3D9_BeginScene() ;
	Graphics_D3D9_DrawPreparationToShader( DX_D3D9_DRAWPREP_3D | DX_D3D9_DRAWPREP_GOURAUDSHADE | DX_D3D9_DRAWPREP_FOG | DX_D3D9_DRAWPREP_PERSPECTIVE | DX_D3D9_DRAWPREP_LIGHTING, VertexBuffer->Type, FALSE, FALSE ) ;

	// インデックスタイプをセット
	IndexFormat = D_D3DFMT_INDEX16 ;
	switch( IndexBuffer->Type )
	{
	case DX_INDEX_TYPE_16BIT:
		IndexFormat = D_D3DFMT_INDEX16 ;
		break ;

	case DX_INDEX_TYPE_32BIT:
		IndexFormat = D_D3DFMT_INDEX32 ;
		break ;
	}

	// プリミティブの数を取得
	PrimitiveCount = GETPRIMNUM( PrimitiveType, UseIndexNum ) ;

	// 描画
	if( VertexBuffer->PF->D3D9.VertexBuffer != NULL &&
		IndexBuffer->PF->D3D9.IndexBuffer  != NULL )
	{
		Graphics_D3D9_DeviceState_SetVertexBuffer( VertexBuffer->PF->D3D9.VertexBuffer, VertexBuffer->UnitSize ) ;
		Graphics_D3D9_DeviceState_SetIndexBuffer( IndexBuffer->PF->D3D9.IndexBuffer ) ;

		Direct3DDevice9_DrawIndexedPrimitive(
			( D_D3DPRIMITIVETYPE )PrimitiveType,
			BaseVertex,
			( UINT )StartVertex,
			( UINT )UseVertexNum,
			( UINT )StartIndex,
			PrimitiveCount ) ;
		GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;
	}
	else
	{
		Direct3DDevice9_DrawIndexedPrimitiveUP(
			( D_D3DPRIMITIVETYPE )PrimitiveType,
			( UINT )StartVertex,
			( UINT )UseVertexNum,
			PrimitiveCount,
			( BYTE * )IndexBuffer->Buffer + StartIndex * IndexBuffer->UnitSize,
			IndexFormat, 
			( BYTE * )VertexBuffer->Buffer + BaseVertex * VertexBuffer->UnitSize,
			( UINT )VertexBuffer->UnitSize ) ;
		GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;
	}

	// 終了
	return 0 ;
}
















// Direct3D のアダプタ情報を列挙する
static int EnumDirect3DAdapter()
{
	int i ;

	if( GD3D9.Adapter.ValidInfo ) return 0 ;

	// 初期化判定
	if( DxSysData.DxLib_InitializeFlag == FALSE || DxLib_GetEndRequest() )
	{
		// Direct3D9.DLL の読み込み
		if( Direct3D9_LoadDLL() < 0 )
			return -1 ;

		// Direct3D9 の作成
		if( Direct3D9_Create() == 0 )
		{
			GD3D9.Adapter.InfoNum = ( int )Direct3D9_GetAdapterCount() ;
			for( i = 0 ; i < GD3D9.Adapter.InfoNum ; i ++ )
				Direct3D9_GetAdapterIdentifier( ( UINT )i, 0, &GD3D9.Adapter.Info[ i ] ) ;

			// Direct3D9 の解放
			Direct3D9_Release() ;
		}

		// d3d9.dll の解放
		Direct3D9_FreeDLL() ;
	}
	else
	{
		GD3D9.Adapter.InfoNum = ( int )Direct3D9_GetAdapterCount() ;
		for( i = 0 ; i < GD3D9.Adapter.InfoNum ; i ++ )
			Direct3D9_GetAdapterIdentifier( ( UINT )i, 0, &GD3D9.Adapter.Info[ i ] ) ;
	}

	GD3D9.Adapter.ValidInfo = TRUE ;

	// 正常終了
	return 0 ;
}





















// 環境依存初期化関係

// 描画処理の環境依存部分の初期化を行う関数( 実行箇所区別０ )
extern	int		Graphics_D3D9_Initialize_Timing0_PF( void )
{
	// Direct3D9 を使用したグラフィックス処理の初期化
	if( Graphics_D3D9_Initialize() == -1 )
	{
		return -1 ;
	}

	// シェーダー定数情報を初期化
	Graphics_D3D9_ShaderConstant_InfoSet_Initialize( &GD3D9.Device.Shader.ShaderConstantInfo ) ;
	Graphics_D3D9_ShaderConstant_InfoSet_SetUseState( &GD3D9.Device.Shader.ShaderConstantInfo, DX_SHADERCONSTANTSET_MASK_LIB ) ;

	// 正常終了
	return 0 ;
}

// 描画処理の環境依存部分の初期化を行う関数( 実行箇所区別２ )
extern	int		Graphics_D3D9_Initialize_Timing1_PF( void )
{
	// デバイスの設定をリフレッシュ
	Graphics_D3D9_DeviceState_RefreshRenderState() ;

	// ピクセルシェーダーを使用する場合は出力先をテクスチャとして使用できるかどうかのチェックを行う
	Graphics_D3D9_SetupRenderTargetInputTextureFlag() ;

	// 中を塗りつぶす DrawCircle の際に X 座標に足すべき値を調べる
	{
		DWORD RetL ;
		DWORD RetR ;

		GD3D9.Device.Caps.DrawFillCircleLeftVertAddX  = -0.5f ;
		GD3D9.Device.Caps.DrawFillCircleRightVertAddX = -0.5f ;

		NS_DrawCircle( 16, 16, 10, GetColor( 255,255,255 ), TRUE ) ;
		RetL = NS_GetPixel( 5, 16 ) ;
		RetR = NS_GetPixel( 26, 16 ) ;
		NS_DrawBox( 5, 5, 28, 28, GetColor( GSYS.Screen.BackgroundRed, GSYS.Screen.BackgroundGreen, GSYS.Screen.BackgroundBlue ), TRUE ) ;

		if( RetL != 0 )
		{
			GD3D9.Device.Caps.DrawFillCircleLeftVertAddX  = -0.1f ;
		}

		if( RetR == 0 )
		{
			GD3D9.Device.Caps.DrawFillCircleLeftVertAddX  = -0.1f ;
		}

		if( RetL != 0 || RetR == 0 )
		{
			NS_DrawCircle( 16, 16, 10, GetColor( 255,255,255 ), TRUE ) ;
			RetL = NS_GetPixel( 5, 16 ) ;
			RetR = NS_GetPixel( 26, 16 ) ;
			NS_DrawBox( 5, 5, 28, 28, GetColor( GSYS.Screen.BackgroundRed, GSYS.Screen.BackgroundGreen, GSYS.Screen.BackgroundBlue ), TRUE ) ;

			if( RetL != 0 )
			{
				GD3D9.Device.Caps.DrawFillCircleLeftVertAddX  = 0.0f ;
			}

			if( RetR == 0 )
			{
				GD3D9.Device.Caps.DrawFillCircleLeftVertAddX  = 0.0f ;
			}
		}
	}

	// 正常終了
	return 0 ;
}

// ハードウエアアクセラレータを使用する場合の環境依存の初期化処理を行う
extern	int		Graphics_D3D9_Hardware_Initialize_PF( void )
{
	return Graphics_D3D9_Device_Initialize() ;
}

// 描画処理の環境依存部分の後始末を行う関数
extern	int		Graphics_D3D9_Terminate_PF( void )
{
	// Direct3D9 の描画処理の後始末
	Graphics_D3D9_Terminate() ;

	// シェーダーコードの後始末
//	ShaderCodeTerminate() ;
	Graphics_D3D9_ShaderCode_Base_Terminate() ;
	Graphics_D3D9_ShaderCode_Base3D_Terminate() ;

#ifndef DX_NON_MODEL
	// ShaderCodeTerminate_Model() ;
	Graphics_D3D9_ShaderCode_Model_Terminate() ;
#endif // DX_NON_MODEL

	// 終了
	return 0 ;
}

// グラフィックスシステムの復帰、又は変更付きの再セットアップを行う
extern	int		Graphics_D3D9_RestoreOrChangeSetupGraphSystem_PF( int Change, int ScreenSizeX, int ScreenSizeY, int ColorBitDepth, int RefreshRate )
{
	int i ;

	// グラフィックスデバイスのロストから復帰する前に呼ぶ関数が登録されていたら実行する
	if( GD3D9.Device.Setting.DeviceLostCallbackFunction )
	{
		GD3D9.Device.Setting.DeviceLostCallbackFunction( GD3D9.Device.Setting.DeviceLostCallbackData ) ;
	}

	if( DxSysData.NotDrawFlag == FALSE )
	{
		// DirectX のオブジェクトを解放する
		Graphics_ReleaseDirectXObject() ;
	}

	if( Change == TRUE )
	{
		// 画面モードのセット
		Graphics_Screen_SetMainScreenSize( ScreenSizeX, ScreenSizeY ) ;
		GSYS.Screen.MainScreenColorBitDepth = ColorBitDepth ;
		GSYS.Screen.MainScreenRefreshRate   = RefreshRate ;
		SetMemImgDefaultColorType( ColorBitDepth == 32 ? 1 : 0 ) ;
	}
	else
	{
		// DirectInput オブジェクトの再セットアップ
#ifndef DX_NON_INPUT
		TerminateInputSystem() ;
		InitializeInputSystem() ;
#endif // DX_NON_INPUT
	}

//	DXST_LOGFILEFMT_ADDUTF16LE(( L"確保メモリ数:%d  確保メモリ総サイズ:%dByte(%dKByte)", NS_DxGetAllocNum(), NS_DxGetAllocSize(), NS_DxGetAllocSize() / 1024 )) ;

	if( DxSysData.NotDrawFlag == FALSE )
	{
		MATRIX_D ViewMatrix, ProjectionMatrix, ViewportMatrix ;

		// Direct3D9 による描画処理の初期化
		GSYS.Screen.FullScreenResolutionModeAct = GSYS.Screen.FullScreenResolutionMode ;
		Graphics_D3D9_Initialize() ;

		// グラフィックハンドルが持つ Direct3D9 オブジェクトの再作成
		Graphics_D3D9_CreateObjectAll() ;

#ifndef DX_NON_MASK
		// マスクサーフェスの再作成
		Mask_ReCreateSurface() ;
#endif
		// アクティブグラフィックのアドレスを再度設定
		if( Change == FALSE )
		{
			ViewMatrix       = GSYS.DrawSetting.ViewMatrix ;
			ProjectionMatrix = GSYS.DrawSetting.ProjectionMatrix ;
			ViewportMatrix   = GSYS.DrawSetting.ViewportMatrix ;
		}
		NS_SetRenderTargetToShader( 0, GSYS.DrawSetting.TargetScreen[ 0 ], GSYS.DrawSetting.TargetScreenSurface[ 0 ] ) ;

		// グラフィック関係の設定を初期化
		Graphics_D3D9_Device_ReInitialize() ;

		// 画面の初期化
//		NS_ClearDrawScreen() ;
//		NS_ScreenFlip() ;
//		NS_ClearDrawScreen() ;
//		NS_ScreenFlip() ;

		// デバイスロスト発生時に削除するフラグが立っているグラフィックを削除する
		Graphics_Image_DeleteDeviceLostDelete() ;

		// グラフィックの復元
/*
		if( Change == TRUE )
		{
			// グラフィックの復元
			DefaultRestoreGraphFunction() ;

#ifndef DX_NON_MODEL
			// モデルのテクスチャを再読み込み
			MV1ReloadTexture() ;
#endif
			// フォントハンドルが持つキャッシュを初期化
			InitCacheFontToHandle() ;
		}
		else
*/		{
			NS_RunRestoreShred() ;
		}

		// グラフィックスデバイスのロストから復帰したときに呼ぶ関数が登録されていたら実行する
		if( GD3D9.Device.Setting.DeviceRestoreCallbackFunction )
		{
			GD3D9.Device.Setting.DeviceRestoreCallbackFunction( GD3D9.Device.Setting.DeviceRestoreCallbackData ) ;
		}

		// ハードウエアの設定をリフレッシュ
		Graphics_D3D9_DeviceState_RefreshRenderState() ;

#ifndef DX_NON_MODEL
		// 頂点バッファのセットアップ
		MV1SetupVertexBufferAll() ;
#endif

		// ライトの設定を Direct3D に再反映するようにフラグをセット
		GSYS.Light.HardwareRefresh = TRUE ;

		// その他の再設定
		if( Change == FALSE )
		{
			NS_SetDrawArea( GSYS.DrawSetting.OriginalDrawRect.left, GSYS.DrawSetting.OriginalDrawRect.top, GSYS.DrawSetting.OriginalDrawRect.right, GSYS.DrawSetting.OriginalDrawRect.bottom ) ;
			NS_SetTransformToWorldD( &GSYS.DrawSetting.WorldMatrix );
			NS_SetTransformToViewD( &ViewMatrix );
			NS_SetTransformToProjectionD( &ProjectionMatrix );
			NS_SetTransformToViewportD( &ViewportMatrix );
		}

		// ０以外のターゲットのセットアップ
		for( i = 1 ; i < GSYS.HardInfo.RenderTargetNum ; i ++ )
		{
			NS_SetRenderTargetToShader( i, GSYS.DrawSetting.TargetScreen[ i ], GSYS.DrawSetting.TargetScreenSurface[ i ] ) ;
		}
	}

	// 終了
	return 0 ;
}

// 描画用デバイスが有効かどうかを取得する( 戻り値  TRUE:有効  FALSE:無効 )
extern	int		Graphics_D3D9_Hardware_CheckValid_PF( void )
{
	return Direct3DDevice9_IsValid() ;
}

// DirectX のオブジェクトを解放する
extern	void	Graphics_D3D9_ReleaseDirectXObject_PF( void )
{
#ifndef DX_NON_MASK
	// マスクサーフェスを一時削除
	Mask_ReleaseSurface() ;
#endif
	// グラフィックハンドルが持つ DirectX オブジェクトの解放
	Graphics_D3D9_ReleaseObjectAll() ;

	// システムが持つ Direct3D9 オブジェクトの解放
	Graphics_D3D9_Terminate() ;
}























// 環境依存システム関係

// WM_ACTIVATE メッセージの処理で、ウインドウモード時にアクティブになった際に呼ばれる関数
extern	void	Graphics_D3D9_WM_ACTIVATE_ActiveProcess_PF( void )
{
//	if( NS_ScreenFlip() != 0 )
	if( GAPIWin.Direct3DDevice9Object == NULL )
	{
		return ;
	}

	Graphics_D3D9_EndScene() ;
	if( Direct3DDevice9_IsLost() != 0 )
	{
		NS_RestoreGraphSystem() ;
	}
}























// 環境依存描画設定関係

// シェーダー描画での描画先を設定する
extern	int		Graphics_Hardware_D3D9_SetRenderTargetToShader_PF( int TargetIndex, int DrawScreen, int SurfaceIndex, int /*MipLevel*/ )
{
	IMAGEDATA *Image ;
	IMAGEDATA *OldImage ;

	// ターゲットが０以外の場合はシェーダーが使用できない場合は何もしない
	if( GSYS.HardInfo.UseShader == FALSE )
		return -1 ;

	// デバイスが無効になっていたら何もせず終了
	if( GAPIWin.Direct3DDevice9Object == NULL )
		return -1 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画処理を終了しておく
	Graphics_D3D9_EndScene() ;

	// セットしていたテクスチャーを外す
	Graphics_D3D9_DrawSetting_SetTexture( NULL ) ;

	// 今までの描画先の画像情報の取得
	if( GRAPHCHKFULL( GSYS.DrawSetting.TargetScreen[ TargetIndex ], OldImage ) )
	{
		OldImage = NULL ;
	}
	else
	{
		// 今までの描画先がレンダリングターゲットサーフェスだった場合はレンダリングターゲットテクスチャに転送する
		if( OldImage->Hard.Draw[ 0 ].Tex->PF->D3D9.RenderTargetSurface )
		{
			Graphics_D3D9_UpdateDrawTexture( OldImage->Hard.Draw[ 0 ].Tex, GSYS.DrawSetting.TargetScreenSurface[ TargetIndex ] ) ;
		}
	}

	// 画像かどうかを判定
	if( GRAPHCHKFULL( DrawScreen, Image ) )
	{
		// 画像ではない場合は描画対象を無効にする
		GSYS.DrawSetting.TargetScreen[ TargetIndex ] = 0 ;
		GSYS.DrawSetting.TargetScreenSurface[ TargetIndex ] = 0 ;
		Graphics_D3D9_DeviceState_SetRenderTarget( NULL, TargetIndex ) ;
		return 0 ;
	}

	// 画像だった場合は描画可能では無い場合はエラー
	if( Image->Orig->FormatDesc.DrawValidFlag == FALSE )
	{
		return -1 ;
	}

	// 描画先をセット
	if( Image->Hard.Draw[ 0 ].Tex->PF->D3D9.RenderTargetSurface )
	{
		Graphics_D3D9_DeviceState_SetRenderTarget( Image->Hard.Draw[ 0 ].Tex->PF->D3D9.RenderTargetSurface, TargetIndex ) ;
	}
	else
	{
		Graphics_D3D9_DeviceState_SetRenderTarget( Image->Hard.Draw[ 0 ].Tex->PF->D3D9.Surface[ SurfaceIndex ], TargetIndex ) ;
	}

	// 正常終了
	return 0 ;
}

// SetDrawBright の引数が一つ版
extern	int		Graphics_Hardware_D3D9_SetDrawBrightToOneParam_PF( DWORD /* Bright */ )
{
	// ディフューズカラーの更新
	GD3D9.Device.DrawInfo.DiffuseColor = GetDiffuseColor() ;

	// 正常終了
	return 0 ;
}

// 描画ブレンドモードをセットする
extern	int		Graphics_Hardware_D3D9_SetDrawBlendMode_PF( int /*  BlendMode */, int /* BlendParam */ )
{
	// ディフーズカラーの更新
	GD3D9.Device.DrawInfo.DiffuseColor = GetDiffuseColor() ;

	// 正常終了
	return 0 ;
}

// 描画時のアルファテストの設定を行う( TestMode:DX_CMP_GREATER等( -1:デフォルト動作に戻す )  TestParam:描画アルファ値との比較に使用する値 )
extern	int		Graphics_Hardware_D3D9_SetDrawAlphaTest_PF( int /* TestMode */, int /* TestParam */ )
{
	// ディフーズカラーの更新
	GD3D9.Device.DrawInfo.DiffuseColor = GetDiffuseColor() ;

	// 正常終了
	return 0 ;
}

// 描画モードをセットする
extern	int		Graphics_Hardware_D3D9_SetDrawMode_PF( int DrawMode )
{
	Graphics_D3D9_DeviceState_SetDrawMode( DrawMode ) ;

	// 正常終了
	return 0 ;
}

// 描画輝度をセット
extern	int		Graphics_Hardware_D3D9_SetDrawBright_PF( int /* RedBright */, int /* GreenBright */, int /* BlueBright */ )
{
	// ディフューズカラーの更新
	GD3D9.Device.DrawInfo.DiffuseColor = GetDiffuseColor() ;

	// 正常終了
	return 0 ;
}

// SetBlendGraphParam の可変長引数パラメータ付き
extern	int		Graphics_Hardware_D3D9_SetBlendGraphParamBase_PF( IMAGEDATA *BlendImage, int BlendType, int *Param )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;
	
	// ブレンド画像が NULL もしくはテクスチャではなかったらブレンド画像処理の解除
	if( BlendImage == NULL || BlendImage->Orig->FormatDesc.TextureFlag == FALSE )
	{
		GSYS.DrawSetting.BlendGraph = -1 ;
		Graphics_D3D9_DrawSetting_SetBlendTexture( NULL ) ;
	}
	else
	{
		// ブレンドパラメータをセットする
		Graphics_D3D9_DrawSetting_SetBlendTexture(
			BlendImage->Hard.Draw[ 0 ].Tex[ 0 ].PF->D3D9.Texture,
			BlendImage->Hard.Draw[ 0 ].Tex[ 0 ].TexWidth,
			BlendImage->Hard.Draw[ 0 ].Tex[ 0 ].TexHeight
		) ;
		Graphics_D3D9_DrawSetting_SetBlendTextureParam( BlendType, Param ) ;
	}

	// 正常終了
	return 0 ;
}

// 最大異方性の値をセットする
extern	int		Graphics_Hardware_D3D9_SetMaxAnisotropy_PF( int MaxAnisotropy )
{
	Graphics_D3D9_DeviceState_SetMaxAnisotropy( MaxAnisotropy ) ;

	// 正常終了
	return 0 ;
}

// ワールド変換用行列をセットする
extern	int		Graphics_Hardware_D3D9_SetTransformToWorld_PF( const MATRIX *Matrix )
{
	Graphics_D3D9_DeviceState_SetWorldMatrix( Matrix ) ;

	// 正常終了
	return 0 ;
}

// ビュー変換用行列をセットする
extern	int		Graphics_Hardware_D3D9_SetTransformToView_PF( const MATRIX *Matrix )
{
	Graphics_D3D9_DeviceState_SetViewMatrix( Matrix ) ;

	// 正常終了
	return 0 ;
}

// Direct3D9 に設定する射影行列を更新する
static void Graphics_Hardware_D3D9_RefreshProjectionMatrix( int Update3DProjection, int /*Update2DProjection*/, int AlwaysUpdate )
{
	MATRIX *UseProjectionMatrix ;
	MATRIX *UseViewportMatrix ;
	MATRIX TempMatrix ;

//	if( GSYS.DrawSetting.UseProjectionMatrix2D )
//	{
//		if( AlwaysUpdate == FALSE && Update2DProjection == FALSE )
//		{
//			return ;
//		}
//
//		UseProjectionMatrix = &GSYS.DrawSetting.ProjectionMatrix2D ;
//		UseViewportMatrix   = &GSYS.DrawSetting.ViewportMatrix2D ;
//	}
//	else
	{
		if( AlwaysUpdate == FALSE && Update3DProjection == FALSE )
		{
			return ;
		}

		UseProjectionMatrix = &GSYS.DrawSetting.ProjectionMatrixF ;
		UseViewportMatrix   = &GSYS.DrawSetting.ViewportMatrixF ;
	}

	if( GSYS.DrawSetting.MatchHardware_2DMatrix )
	{
		CreateMultiplyMatrix( &TempMatrix, &GSYS.DrawSetting._2DMatrixF, &GSYS.DrawSetting.Direct3DViewportMatrixAntiF ) ;
	}
	else
	{
		CreateMultiplyMatrix( &TempMatrix, UseProjectionMatrix, UseViewportMatrix                             ) ;
		CreateMultiplyMatrix( &TempMatrix, &TempMatrix,         &GSYS.DrawSetting.Direct3DViewportMatrixAntiF ) ;
	}

	Graphics_D3D9_DeviceState_SetProjectionMatrix( &TempMatrix ) ;
}

// 投影変換用行列をセットする
extern	int		Graphics_Hardware_D3D9_SetTransformToProjection_PF( const MATRIX * /* Matrix */ )
{
	Graphics_Hardware_D3D9_RefreshProjectionMatrix( TRUE, FALSE, FALSE ) ;

//	MATRIX TempMatrix ;
//
//	CreateMultiplyMatrix( &TempMatrix, &GSYS.DrawSetting.ProjectionMatrix, &GSYS.DrawSetting.ViewportMatrix             ) ;
//	CreateMultiplyMatrix( &TempMatrix, &TempMatrix,                        &GSYS.DrawSetting.Direct3DViewportMatrixAnti ) ;
//	Graphics_D3D9_DeviceState_SetProjectionMatrix( &TempMatrix ) ;

	// 正常終了
	return 0 ;
}

// ビューポート行列をセットする
extern	int		Graphics_Hardware_D3D9_SetTransformToViewport_PF( const MATRIX * /* Matrix */ )
{
	Graphics_Hardware_D3D9_RefreshProjectionMatrix( TRUE, FALSE, FALSE ) ;

//	MATRIX TempMatrix ;
//
//	CreateMultiplyMatrix( &TempMatrix, &GSYS.DrawSetting.ProjectionMatrix, &GSYS.DrawSetting.ViewportMatrix ) ;
//	CreateMultiplyMatrix( &TempMatrix, &TempMatrix,                        &GSYS.DrawSetting.Direct3DViewportMatrixAnti ) ;
//	Graphics_D3D9_DeviceState_SetProjectionMatrix( &TempMatrix ) ;

	// 正常終了
	return 0 ;
}

// テクスチャアドレスモードを設定する
extern	int		Graphics_Hardware_D3D9_SetTextureAddressMode_PF( int Mode /* DX_TEXADDRESS_WRAP 等 */, int Stage )
{
	Graphics_D3D9_DeviceState_SetTextureAddress( Mode, Stage ) ;

	// 正常終了
	return 0 ;
}

// テクスチャアドレスモードを設定する
extern	int		Graphics_Hardware_D3D9_SetTextureAddressModeUV_PF( int ModeU, int ModeV, int Stage )
{
	Graphics_D3D9_DeviceState_SetTextureAddressU( ModeU, Stage ) ;
	Graphics_D3D9_DeviceState_SetTextureAddressV( ModeV, Stage ) ;

	// 正常終了
	return 0 ;
}

// テクスチャ座標変換行列をセットする
extern	int		Graphics_Hardware_D3D9_SetTextureAddressTransformMatrix_PF( int UseFlag, MATRIX *Matrix, int Sampler )
{
	Graphics_D3D9_DeviceState_SetTextureAddressTransformMatrix( UseFlag, Matrix, Sampler ) ;

	// 正常終了
	return 0 ;
}

// フォグを有効にするかどうかを設定する( TRUE:有効  FALSE:無効 )
extern	int		Graphics_Hardware_D3D9_SetFogEnable_PF( int Flag )
{
	Graphics_D3D9_DeviceState_SetFogEnable( Flag ) ;

	// 正常終了
	return 0 ;
}

// フォグモードを設定する
extern	int		Graphics_Hardware_D3D9_SetFogMode_PF( int Mode /* DX_FOGMODE_NONE 等 */ )
{
	Graphics_D3D9_DeviceState_SetFogVertexMode( Mode ) ;

	// 正常終了
	return 0 ;
}

// フォグカラーを変更する
extern	int		Graphics_Hardware_D3D9_SetFogColor_PF( DWORD FogColor )
{
	Graphics_D3D9_DeviceState_SetFogColor( FogColor ) ;

	// 正常終了
	return 0 ;
}

// フォグが始まる距離と終了する距離を設定する( 0.0f 〜 1.0f )
extern	int		Graphics_Hardware_D3D9_SetFogStartEnd_PF( float start, float end )
{
	Graphics_D3D9_DeviceState_SetFogStartEnd( start, end ) ;

	// 正常終了
	return 0 ;
}

// フォグの密度を設定する( 0.0f 〜 1.0f )
extern	int		Graphics_Hardware_D3D9_SetFogDensity_PF( float density )
{
	Graphics_D3D9_DeviceState_SetFogDensity( density ) ;

	// 正常終了
	return 0 ;
}

// 基本データに設定されているフォグ情報をハードウェアに反映する
extern	int		Graphics_Hardware_D3D9_ApplyLigFogToHardware_PF( void )
{
	if( GD3D9.Device.State.FogEnable != GSYS.DrawSetting.FogEnable )
	{
		Graphics_D3D9_DeviceState_SetFogEnable( GSYS.DrawSetting.FogEnable ) ;
	}

	// 正常終了
	return 0 ;
}

// 以前の DrawModiGraph 関数のコードを使用するかどうかのフラグをセットする
extern	int		Graphics_Hardware_D3D9_SetUseOldDrawModiGraphCodeFlag_PF( int Flag )
{
	GD3D9.Setting.UseOldDrawModiGraphCodeFlag = Flag ;
	
	// 終了
	return 0 ;
}

// 描画先に正しいα値を書き込むかどうかのフラグを更新する
extern	int		Graphics_Hardware_D3D9_RefreshAlphaChDrawMode_PF( void )
{
	IMAGEDATA *Image ;
	int i ;
	int NextFlag ;

	NextFlag = FALSE ;

	// ０番以外にも描画先が設定されていた場合は通常モード
	for( i = 1 ; i < DX_RENDERTARGET_COUNT ; i ++ )
	{
		if( GSYS.DrawSetting.TargetScreen[ i ] != 0 )
			goto END ;
	}

	// 描画先が通常画面の場合は何もしない
	if( GSYS.DrawSetting.TargetScreen[ 0 ] == DX_SCREEN_BACK ||
		GSYS.DrawSetting.TargetScreen[ 0 ] == DX_SCREEN_FRONT ||
		GSYS.DrawSetting.TargetScreen[ 0 ] == DX_SCREEN_WORK ||
		GSYS.DrawSetting.TargetScreen[ 0 ] == DX_SCREEN_TEMPFRONT )
		goto END ;

	// 描画先となっている画像が無効の場合は何もしない
	if( GRAPHCHKFULL( GSYS.DrawSetting.TargetScreen[ 0 ], Image ) )
		goto END ;

	// 描画先の画像にαチャンネルが無い場合も何もしない
	if( Image->Orig->FormatDesc.AlphaChFlag == FALSE )
		goto END ;
	
	// テクスチャではない場合は何もしない
	if( Image->Orig->FormatDesc.TextureFlag == FALSE )
		goto END ;

	// ここに来たということは正確なαチャンネルの計算を行うということ
	NextFlag = TRUE ;

END :
/*
	// テクスチャの保存
	if( NextFlag == TRUE )
	{
		IMAGEDATA *WorkImage ;
		int WorkTextureHandle ;

		WorkTextureHandle = GetWorkTexture( Image->Orig->FormatDesc.FloatTypeFlag, Image->Width, Image->Height, 0 ) ;
		if( GRAPHCHK( WorkTextureHandle, WorkImage ) )
			return -1 ;

		GRH.RenderTargetTexture          = WorkImage->Orig->Hard.Tex[ 0 ].PF->D3D9.Texture ;
		GRH.RenderTargetSurface          =     Image->Orig->Hard.Tex[ 0 ].PF->D3D9.Surface[ 0 ] ;
		GRH.RenderTargetCopySurface      = WorkImage->Orig->Hard.Tex[ 0 ].PF->D3D9.Surface[ 0 ] ;
		GRH.RenderTargetTextureInvWidth  = 1.0f / WorkImage->Orig->Hard.Tex[ 0 ].TexWidth ;
		GRH.RenderTargetTextureInvHeight = 1.0f / WorkImage->Orig->Hard.Tex[ 0 ].TexHeight ;
	}
	else
	{
		GRH.RenderTargetTexture = FALSE ;
	}
*/
	// 以前とフラグの状態が異なる場合のみブレンドモードの更新も行う
	if( NextFlag != GSYS.DrawSetting.AlphaChDrawMode )
	{
		// フラグの保存
		GSYS.DrawSetting.AlphaChDrawMode = NextFlag ;
		GD3D9.Device.DrawSetting.ChangeBlendParamFlag = TRUE ;
		GSYS.ChangeSettingFlag = TRUE ;
	}

	// 終了
	return 0 ;
}
























// 環境依存設定関係

// ハードウエアの頂点演算処理機能を使用するかどうかを設定する
extern	int		Graphics_Hardware_D3D9_SetUseHardwareVertexProcessing_PF( int Flag )
{
	// 初期化前のみ有効
	if( DxSysData.DxLib_InitializeFlag == TRUE ) return -1 ;

	GD3D9.Setting.NonUseVertexHardwareProcess = !Flag ;

	// 正常終了
	return 0 ;
}

// ピクセル単位でライティングを行うかどうかを設定する、要 ShaderModel 3.0( TRUE:ピクセル単位のライティングを行う  FALSE:頂点単位のライティングを行う( デフォルト ) )
extern	int		Graphics_Hardware_D3D9_SetUsePixelLighting_PF( int Flag )
{
	GD3D9.UsePixelLightingShader = Flag ;

	// 終了
	return 0 ;
}

// グラフィックスデバイスがロストから復帰した際に呼ばれるコールバック関数を設定する
extern	int		Graphics_Hardware_D3D9_SetGraphicsDeviceRestoreCallbackFunction_PF( void (* Callback )( void *Data ), void *CallbackData )
{
	GD3D9.Device.Setting.DeviceRestoreCallbackFunction = Callback ;
	GD3D9.Device.Setting.DeviceRestoreCallbackData = CallbackData ;

	// 終了
	return 0 ;
}

// グラフィックスデバイスがロストから復帰する前に呼ばれるコールバック関数を設定する
extern	int		Graphics_Hardware_D3D9_SetGraphicsDeviceLostCallbackFunction_PF( void (* Callback )( void *Data ), void *CallbackData )
{
	GD3D9.Device.Setting.DeviceLostCallbackFunction = Callback ;
	GD3D9.Device.Setting.DeviceLostCallbackData = CallbackData ;

	// 終了
	return 0 ;
}

// 通常描画にプログラマブルシェーダーを使用するかどうかを設定する( TRUE:使用する( デフォルト )  FALSE:使用しない )
extern	int		Graphics_Hardware_D3D9_SetUseNormalDrawShader_PF( int Flag )
{
#ifdef DX_NON_NORMAL_DRAW_SHADER
	return -1 ;
#endif // DX_NON_NORMAL_DRAW_SHADER

	Flag = Flag ? FALSE : TRUE ;

	// 初期化前の場合は、フラグだけ保存する
	if( DxSysData.DxLib_InitializeFlag == FALSE )
	{
		GD3D9.NormalDraw_NotUsePixelShader = Flag ;

		return 0 ;
	}

	// フラグが同じ場合は何もしない
	if( Flag == GD3D9.NormalDraw_NotUsePixelShader )
	{
		return 0 ;
	}

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画情報を吐き出しておく
	Graphics_D3D9_EndScene() ;

	// フラグを保存
	GD3D9.NormalDraw_NotUsePixelShader = Flag ;
	GSYS.ChangeSettingFlag = TRUE ;
	GD3D9.Device.DrawSetting.DrawPrepParamFlag = TRUE ;

	// 設定をリフレッシュ
	Graphics_D3D9_DeviceState_RefreshRenderState() ;

	// 終了
	return 0 ;
}

// ビデオメモリの容量を得る
extern	int		Graphics_Hardware_D3D9_GetVideoMemorySize_PF( int *AllSize, int *FreeSize )
{
	return DirectDraw7_GetVideoMemorySize( AllSize, FreeSize ) ;
}

// 使用中のＤｉｒｅｃｔ３ＤＤｅｖｉｃｅ９オブジェクトを得る
extern	const D_IDirect3DDevice9 * Graphics_Hardware_D3D9_GetUseDirect3DDevice9_PF( void )
{
	return ( D_IDirect3DDevice9* )Direct3DDevice9_GetObject() ;
}

// 使用中のバックバッファのDirect3DSurface9オブジェクトを取得する
extern	const D_IDirect3DSurface9* Graphics_Hardware_D3D9_GetUseDirect3D9BackBufferSurface_PF( void )
{
	return GD3D9.Device.Screen.BackBufferSurface ;
}

// ＤＸライブラリのＤｉｒｅｃｔ３Ｄ設定をしなおす
extern	int		Graphics_Hardware_D3D9_RefreshDxLibDirect3DSetting_PF( void )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	Graphics_D3D9_DeviceState_RefreshRenderState() ;

	// 初期化フラグを立てる
	GD3D9.Device.DrawSetting.CancelSettingEqualCheck = TRUE ;

	// ブレンドモードの再設定
	Graphics_D3D9_DrawSetting_SetDrawBlendMode( GD3D9.Device.DrawSetting.BlendMode, GD3D9.Device.DrawSetting.AlphaTestValidFlag, GD3D9.Device.DrawSetting.AlphaChannelValidFlag ) ;
	Graphics_D3D9_DrawPreparation() ;

	// 初期化フラグを倒す
	GD3D9.Device.DrawSetting.CancelSettingEqualCheck = FALSE ;

	// 終了
	return 0 ;
}

// ＤｉｒｅｃｔＤｒａｗが使用するＧＵＩＤを設定する
extern	int		Graphics_D3D9_SetDDrawUseGuid_PF( const GUID FAR *Guid )
{
	int i ;

	if( EnumDirect3DAdapter() != 0 ) return -1 ;

	// 指定のＧＵＩＤがあるかどうか調べる
	for( i = 0 ; i < GD3D9.Adapter.InfoNum && _MEMCMP( Guid, &GD3D9.Adapter.Info[ i ].DeviceIdentifier, sizeof( GUID ) ) != 0 ; i ++ ){}
	if( i == GD3D9.Adapter.InfoNum ) return -1 ;

	// 値を保存する
	GSYS.Screen.ValidUseDisplayIndex = TRUE ;
	GSYS.Screen.UseDisplayIndex = i ;

	// 終了
	return 0 ;
}

// 有効な DirectDraw デバイスの GUID を取得する
extern	const GUID *Graphics_D3D9_GetDirectDrawDeviceGUID_PF( int Number )
{
	if( EnumDirect3DAdapter() != 0 || Number >= GD3D9.Adapter.InfoNum ) return NULL ;

	return &GD3D9.Adapter.Info[ Number ].DeviceIdentifier ;
}

// 有効な DirectDraw デバイスの名前を得る
extern	int		Graphics_D3D9_GetDirectDrawDeviceDescription_PF( int Number, char *StringBuffer )
{
	if( EnumDirect3DAdapter() != 0 || Number >= GD3D9.Adapter.InfoNum ) return -1 ;

	_STRCPY( StringBuffer, GD3D9.Adapter.Info[ Number ].Description ) ;

	// 終了
	return 0 ;
}

// 有効な DirectDraw デバイスの数を取得する
extern	int		Graphics_D3D9_GetDirectDrawDeviceNum_PF( void )
{
	if( EnumDirect3DAdapter() != 0 ) return -1 ;

	return GD3D9.Adapter.InfoNum ;
}

// 使用する DirectDraw デバイスのインデックスを設定する
//extern	int		Graphics_D3D9_SetUseDirectDrawDeviceIndex_PF( int Index )
//{
//	// 値を保存する
//	GD3D9.Adapter.ValidUseAdapterNumber = TRUE ;
//	GD3D9.Adapter.UseAdapterNumber = Index ;
//
//	// 終了
//	return 0 ;
//}

// Vista以降の環境で Direct3D9Ex を使用するかどうかを設定する( TRUE:使用する( デフォルト )  FALSE:使用しない )
extern	int		Graphics_D3D9_SetUseDirect3D9Ex_PF( int Flag )
{
	// 初期化前のみ有効
	if( DxSysData.DxLib_InitializeFlag == TRUE )
	{
		return -1 ;
	}

	NS_SetUseDirectDrawFlag( TRUE ) ;
	NS_SetUseDirect3D11( FALSE ) ;

	// フラグを保存
	if( Flag )
	{
		GD3D9.Setting.NotUseDirect3D9Ex = FALSE ;
	}
	else
	{
		GD3D9.Setting.NotUseDirect3D9Ex = TRUE ;
	}

	// 終了
	return 0 ;
}

// ダイアログボックスモードを変更する
extern	int		Graphics_D3D9_SetDialogBoxMode_PF( int Flag )
{
	HRESULT hr ;

	if( Direct3DDevice9_IsValid() == FALSE )
	{
		return 0 ;
	}

	Graphics_D3D9_EndScene() ;
	hr = Direct3DDevice9_SetDialogBoxMode( Flag ) ;

	return 0 ;
}































// 環境依存画面関係

// 設定に基づいて使用するＺバッファをセットする
extern	int		Graphics_Hardware_D3D9_SetupUseZBuffer_PF( void )
{
	IMAGEDATA *TargetScreenImage = NULL ;
	IMAGEDATA *TargetZBufferImage = NULL ;
	SHADOWMAPDATA *TargetShadowMap = NULL ;

	// シャドウマップかどうかをチェック
	if( !SHADOWMAPCHKFULL( GSYS.DrawSetting.TargetScreen[ 0 ], TargetShadowMap ) )
	{
		// シャドウマップの場合は専用のＺバッファを使用する
		Direct3DDevice9_SetDepthStencilSurface( TargetShadowMap->PF->D3D9.ZBufferSurface ) ;
	}
	else
	{
		// 描画先の画像情報の取得
		GRAPHCHKFULL( GSYS.DrawSetting.TargetScreen[ 0 ], TargetScreenImage ) ;

		// 使用するＺバッファの画像情報の取得
		GRAPHCHKFULL( GSYS.DrawSetting.TargetZBuffer, TargetZBufferImage ) ;

		// マスクサーフェスが存在していて且つ有効な場合はマスクサーフェスのＺバッファを使用する
#ifndef DX_NON_MASK
		if( MASKD.MaskValidFlag && MASKD3D9.MaskScreenSurface )
		{
			Direct3DDevice9_SetDepthStencilSurface( GD3D9.Device.Screen.ZBufferSurface ) ;
		}
		else
#endif
		// 描画可能画像が描画先かどうかで処理を分岐
		if( TargetScreenImage )
		{
			// 描画先Ｚバッファがあって、且つ描画先以上のサイズを持つ場合は描画先Ｚバッファを使用する
			if( TargetZBufferImage != NULL &&
				TargetZBufferImage->WidthI  >= TargetScreenImage->WidthI &&
				TargetZBufferImage->HeightI >= TargetScreenImage->HeightI )
			{
				Direct3DDevice9_SetDepthStencilSurface( TargetZBufferImage->Hard.Draw[ 0 ].Tex->PF->D3D9.ZBuffer ) ;
			}
			else
			// 描画可能画像のＺバッファがある場合はそのＺバッファを使用する
			if( TargetScreenImage->Hard.Draw[ 0 ].Tex->PF->D3D9.ZBuffer )
			{
				Direct3DDevice9_SetDepthStencilSurface( TargetScreenImage->Hard.Draw[ 0 ].Tex->PF->D3D9.ZBuffer ) ;
			}
			else
			// 描画先がデフォルトのＺバッファよりも大きかったらＺバッファを外す
			if( TargetScreenImage->WidthI  > GD3D9.Device.Screen.ZBufferSizeX || 
				TargetScreenImage->HeightI > GD3D9.Device.Screen.ZBufferSizeY )
			{
				Direct3DDevice9_SetDepthStencilSurface( NULL ) ;
			}
			else
			{
				// デフォルトのＺバッファを使用する
				Direct3DDevice9_SetDepthStencilSurface( GD3D9.Device.Screen.ZBufferSurface ) ;
			}
		}
		else
		{
			// 描画先Ｚバッファがあって、且つ描画先以上のサイズを持つ場合は描画先Ｚバッファを使用する
			if( TargetZBufferImage != NULL &&
				TargetZBufferImage->WidthI  >= GSYS.DrawSetting.DrawSizeX &&
				TargetZBufferImage->HeightI >= GSYS.DrawSetting.DrawSizeY )
			{
				Direct3DDevice9_SetDepthStencilSurface( TargetZBufferImage->Hard.Draw[ 0 ].Tex->PF->D3D9.ZBuffer ) ;
			}
			else
			{
				// それ以外の場合デフォルトのＺバッファを使用する
				Direct3DDevice9_SetDepthStencilSurface( GD3D9.Device.Screen.ZBufferSurface ) ;
			}
		}
	}

	// 終了
	return 0 ;
}

// 画面のＺバッファの状態を初期化する
extern	int		Graphics_Hardware_D3D9_ClearDrawScreenZBuffer_PF( const RECT *ClearRect )
{
	if( GAPIWin.Direct3DDevice9Object == NULL ) return 0 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画の終了
	Graphics_D3D9_EndScene() ;

	// Ｚバッファが存在する場合はＺバッファも初期化する
	if( GD3D9.Device.Screen.ZBufferSurface )
	{
		D_D3DVIEWPORT9 Viewport ;
		D_D3DRECT D3DRect ;

		// ビューポートをサーフェス全体にする
		_MEMSET( &Viewport, 0, sizeof( Viewport ) ) ;
		Viewport.X		= 0 ;
		Viewport.Y		= 0 ;
		Viewport.Width	= ( DWORD )GSYS.DrawSetting.DrawSizeX ;
		Viewport.Height	= ( DWORD )GSYS.DrawSetting.DrawSizeY ;
		Viewport.MinZ	= 0.0f ;
		Viewport.MaxZ	= 1.0f ;
		Graphics_D3D9_DeviceState_SetViewport( &Viewport ) ;

		// クリア
		if( ClearRect )
		{
			D3DRect.x1 = ClearRect->left ;
			D3DRect.y1 = ClearRect->top ;
			D3DRect.x2 = ClearRect->right ;
			D3DRect.y2 = ClearRect->bottom ;
			Direct3DDevice9_Clear( 1, &D3DRect, D_D3DCLEAR_ZBUFFER, 0, 1.0f, 0 ) ;
		}
		else
		{
			Direct3DDevice9_Clear( 0, NULL, D_D3DCLEAR_ZBUFFER, 0, 1.0f, 0 ) ;
		}

		// ビューポートの範囲を元に戻す
		Viewport.X		= ( DWORD )GSYS.DrawSetting.DrawArea.left ;
		Viewport.Y		= ( DWORD )GSYS.DrawSetting.DrawArea.top ;
		Viewport.Width	= ( DWORD )( GSYS.DrawSetting.DrawArea.right  - GSYS.DrawSetting.DrawArea.left ) ;
		Viewport.Height	= ( DWORD )( GSYS.DrawSetting.DrawArea.bottom - GSYS.DrawSetting.DrawArea.top  ) ;
		Graphics_D3D9_DeviceState_SetViewport( &Viewport ) ;
	}

	// 正常終了
	return 0 ;
}

// 画面の状態を初期化する
extern	int		Graphics_Hardware_D3D9_ClearDrawScreen_PF( const RECT *ClearRect )
{
	D_IDirect3DSurface9 *TargetSurface ;
	D_D3DFORMAT          TargetFormat ;
	D_D3DRECT            D3DRect ;
	D_D3DVIEWPORT9       Viewport ;
	DWORD                ClearColor ;
	IMAGEDATA           *Image = NULL ;
	SHADOWMAPDATA       *ShadowMap = NULL ;
//	HRESULT hr ;

	if( GAPIWin.Direct3DDevice9Object == NULL ) return 0 ;

	// 描画先画像の画像情報を取得
	GRAPHCHKFULL( GSYS.DrawSetting.TargetScreen[ 0 ], Image ) ;
	SHADOWMAPCHKFULL( GSYS.DrawSetting.TargetScreen[ 0 ], ShadowMap ) ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画の終了
	Graphics_D3D9_EndScene() ;

	// シャドウマップが使用されている場合はシャドウマップを初期化
	if( ShadowMap )
	{
		TargetSurface = ShadowMap->PF->D3D9.Surface ;
		TargetFormat = GD3D9.Device.Caps.TextureFormat[ ShadowMap->ColorFormat ] ;
	}
	else
	// 描画可能画像が使用されている場合は描画可能画像を初期化
	if( Image )
	{
		TargetSurface = Image->Orig->Hard.Tex[ 0 ].PF->D3D9.Surface[ GSYS.DrawSetting.TargetScreenSurface[ 0 ] ] ;
		TargetFormat = GD3D9.Device.Caps.TextureFormat[ Image->Orig->ColorFormat ] ;
	}
	else
	// サブバックバッファが有効になっている場合はサブバックバッファを初期化
	{
		TargetSurface = GD3D9.Device.Screen.SubBackBufferSurface ? GD3D9.Device.Screen.SubBackBufferSurface : GD3D9.Device.Screen.BackBufferSurface ;
		TargetFormat  = GD3D9.Device.Caps.ScreenFormat ;
	}

	// 描画対象サーフェスを変更する
#ifndef DX_NON_MASK
	if( MASKD.MaskValidFlag && MASKD3D9.MaskScreenSurface )
	{
		Graphics_D3D9_DeviceState_SetRenderTarget( TargetSurface ) ;
	}
#endif

	// ビューポートをサーフェス全体にする
	_MEMSET( &Viewport, 0, sizeof( Viewport ) ) ;
	Viewport.X		= 0 ;
	Viewport.Y		= 0 ;
	Viewport.Width	= ( DWORD )GSYS.DrawSetting.DrawSizeX ;
	Viewport.Height	= ( DWORD )GSYS.DrawSetting.DrawSizeY ;
	Viewport.MinZ	= 0.0f ;
	Viewport.MaxZ	= 1.0f ;
	Graphics_D3D9_DeviceState_SetViewport( &Viewport ) ;

	// パラメータをセット
	if( ClearRect )
	{
		D3DRect.x1 = ClearRect->left ;
		D3DRect.y1 = ClearRect->top ;
		D3DRect.x2 = ClearRect->right ;
		D3DRect.y2 = ClearRect->bottom ;
	}

//	Direct3DDevice9_ColorFill( TargetSurface, NULL, NS_GetColor3( Graphics_D3D9_GetD3DFormatColorData( TargetFormat ), GSYS.Screen.BackgroundRed, GSYS.Screen.BackgroundGreen, GSYS.Screen.BackgroundBlue, 0 ) ) ;
//	hr = Direct3DDevice9_ColorFill( TargetSurface, ClearRect, ( GSYS.Screen.BackgroundRed << 16 ) | ( GSYS.Screen.BackgroundGreen << 8 ) | GSYS.Screen.BackgroundBlue ) ;
	ClearColor = ( ( DWORD )GSYS.Screen.BackgroundRed << 16 ) | ( ( DWORD )GSYS.Screen.BackgroundGreen << 8 ) | ( DWORD )GSYS.Screen.BackgroundBlue ;
	if( ClearRect )
	{
		Direct3DDevice9_Clear( 1, &D3DRect, D_D3DCLEAR_TARGET, ClearColor, 1.0f, 0 ) ;
	}
	else
	{
		Direct3DDevice9_Clear( 0, NULL, D_D3DCLEAR_TARGET, ClearColor, 1.0f, 0 ) ;
	}

	// Ｚバッファが存在する場合はＺバッファも初期化する
	if( GD3D9.Device.Screen.ZBufferSurface )
	{
		// クリア
		if( ClearRect )
		{
			Direct3DDevice9_Clear( 1, &D3DRect, D_D3DCLEAR_ZBUFFER, 0, 1.0f, 0 ) ;
		}
		else
		{
			Direct3DDevice9_Clear( 0, NULL, D_D3DCLEAR_ZBUFFER, 0, 1.0f, 0 ) ;
		}
	}

	// 描画対象サーフェスを元に戻す
#ifndef DX_NON_MASK
	if( MASKD.MaskValidFlag && MASKD3D9.MaskScreenSurface )
	{
		Graphics_D3D9_DeviceState_SetRenderTarget( MASKD3D9.MaskScreenSurface ) ;
	}
#endif

	// ビューポートの範囲を元に戻す
	Viewport.X		= ( DWORD )GSYS.DrawSetting.DrawArea.left ;
	Viewport.Y		= ( DWORD )GSYS.DrawSetting.DrawArea.top ;
	Viewport.Width	= ( DWORD )( GSYS.DrawSetting.DrawArea.right  - GSYS.DrawSetting.DrawArea.left ) ;
	Viewport.Height	= ( DWORD )( GSYS.DrawSetting.DrawArea.bottom - GSYS.DrawSetting.DrawArea.top  ) ;
	Graphics_D3D9_DeviceState_SetViewport( &Viewport ) ;

	// 正常終了
	return 0 ;
}

// 描画先画面のセット
extern	int		Graphics_Hardware_D3D9_SetDrawScreen_PF( int /* DrawScreen */, int OldScreenSurface, int /*OldScreenMipLevel*/, IMAGEDATA *NewTargetImage, IMAGEDATA *OldTargetImage, SHADOWMAPDATA *NewTargetShadowMap, SHADOWMAPDATA * /* OldTargetShadowMap */ )
{
	if( GAPIWin.Direct3DDevice9Object == NULL )
	{
		return -1 ;
	}

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画処理を終了しておく
	Graphics_D3D9_EndScene() ;

	// セットしていたテクスチャーを外す
	Graphics_D3D9_DrawSetting_SetTexture( NULL ) ;

	// 今までの描画先がレンダリングターゲットサーフェスだった場合はレンダリングターゲットテクスチャに転送する
	if( OldTargetImage != NULL && OldTargetImage->Hard.Draw[ 0 ].Tex->PF->D3D9.RenderTargetSurface )
	{
		Graphics_D3D9_UpdateDrawTexture( OldTargetImage->Hard.Draw[ 0 ].Tex, OldScreenSurface ) ;
	}

	// 描画先の変更

	// マスクサーフェスが存在していて且つ有効な場合はマスクサーフェスを描画対象にする
#ifndef DX_NON_MASK
	if( MASKD.MaskValidFlag && MASKD3D9.MaskScreenSurface )
	{
		Graphics_D3D9_DeviceState_SetRenderTarget( MASKD3D9.MaskScreenSurface ) ;
	}
	else
#endif
	// シャドウマップが有効な場合はシャドウマップを描画対象にする
	if( NewTargetShadowMap )
	{
		Graphics_D3D9_DeviceState_SetRenderTarget( NewTargetShadowMap->PF->D3D9.Surface ) ;
	}
	else
	// 描画可能画像が有効な場合は描画可能画像を描画対象にする
	if( NewTargetImage )
	{
		if( NewTargetImage->Hard.Draw[ 0 ].Tex->PF->D3D9.RenderTargetSurface )
		{
			Graphics_D3D9_DeviceState_SetRenderTarget( NewTargetImage->Hard.Draw[ 0 ].Tex->PF->D3D9.RenderTargetSurface ) ;
		}
		else
		{
			Graphics_D3D9_DeviceState_SetRenderTarget( NewTargetImage->Hard.Draw[ 0 ].Tex->PF->D3D9.Surface[ GSYS.DrawSetting.TargetScreenSurface[ 0 ] ] ) ;
		}
	}
	else
	{
		// それ以外の場合はサブバックバッファが有効な場合はサブバックバッファを描画対象にする
		Graphics_D3D9_DeviceState_SetRenderTarget( GD3D9.Device.Screen.SubBackBufferSurface ? GD3D9.Device.Screen.SubBackBufferSurface : GD3D9.Device.Screen.BackBufferSurface ) ;
	}

	// 使用するＺバッファのセットアップ
	Graphics_Screen_SetupUseZBuffer() ;

	// 正常終了
	return 0 ;
}

// SetDrawScreen の最後で呼ばれる関数
extern	int		Graphics_Hardware_D3D9_SetDrawScreen_Post_PF( int DrawScreen )
{
	// 描画先が表画面だったら定期的に画面コピーを行う
	Direct3DDevice9_SetupTimerPresent( DrawScreen == DX_SCREEN_FRONT ) ;

	// 正常終了
	return 0 ;
}

// 描画可能領域のセット
extern	int		Graphics_Hardware_D3D9_SetDrawArea_PF( int /* x1 */, int /* y1 */, int /* x2 */, int /* y2 */ )
{
	D_D3DVIEWPORT9 Viewport ;
//	MATRIX TempMatrix ;

	if( GAPIWin.Direct3DDevice9Object == NULL )	return -1 ;

	if( GSYS.DrawSetting.DrawArea.right  - GSYS.DrawSetting.DrawArea.left == 0 ||
		GSYS.DrawSetting.DrawArea.bottom - GSYS.DrawSetting.DrawArea.top  == 0 )
		return -1 ;

	// ビューポートのセッティング
	_MEMSET( &Viewport, 0, sizeof( Viewport ) ) ;
	Viewport.X		= ( DWORD )GSYS.DrawSetting.DrawArea.left ;
	Viewport.Y		= ( DWORD )GSYS.DrawSetting.DrawArea.top ;
	Viewport.Width	= ( DWORD )( GSYS.DrawSetting.DrawArea.right  - GSYS.DrawSetting.DrawArea.left ) ;
	Viewport.Height	= ( DWORD )( GSYS.DrawSetting.DrawArea.bottom - GSYS.DrawSetting.DrawArea.top  ) ;
	Viewport.MinZ	= 0.0f ;
	Viewport.MaxZ	= 1.0f ;

	// セット
	Graphics_D3D9_DeviceState_SetViewport( &Viewport ) ;
	Graphics_D3D9_DeviceState_SetScissorRect( &GSYS.DrawSetting.DrawArea ) ;

	// 射影行列とビューポート行列の逆行列を乗算したものをセット
//	CreateMultiplyMatrix( &TempMatrix, &GSYS.DrawSetting.ProjectionMatrix, &GSYS.DrawSetting.ViewportMatrix             ) ;
//	CreateMultiplyMatrix( &TempMatrix, &TempMatrix,                        &GSYS.DrawSetting.Direct3DViewportMatrixAnti ) ;
//	Graphics_D3D9_DeviceState_SetProjectionMatrix( &TempMatrix ) ;
	Graphics_Hardware_D3D9_RefreshProjectionMatrix( FALSE, FALSE, TRUE ) ;
	GSYS.DrawSetting.MatchHardwareProjectionMatrix = TRUE ;

	// 正常終了
	return 0 ;
}

// 描画先バッファをロックする
extern	int		Graphics_Hardware_D3D9_LockDrawScreenBuffer_PF( RECT *LockRect, BASEIMAGE *BaseImage, int /* TargetScreen */, IMAGEDATA *TargetImage, int TargetScreenSurface, int /*TargetScreenMipLevel*/, int ReadOnly, int TargetScreenTextureNo )
{
	D_IDirect3DSurface9 *TargetSurface ;
	D_D3DFORMAT          TargetFormat ;
	COLORDATA           *TargetColorData ;
	int                  Width ;
	int                  Height ;
	int                  LockEnable ;
	int                  UseSysMemSurfaceIndex = -1 ;
	D_D3DLOCKED_RECT     SrcLockRect ;
	RECT                 DestRect ;
	HRESULT              hr ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画処理を完了しておく
	Graphics_D3D9_EndScene() ;

	Width  = LockRect->right  - LockRect->left ;
	Height = LockRect->bottom - LockRect->top ;

	// 取り込み元となるサーフェスの決定

	// 描画可能画像が対象の場合
	if( TargetImage )
	{
		TargetSurface = TargetImage->Orig->Hard.Tex[ TargetScreenTextureNo ].PF->D3D9.RenderTargetSurface ?
						TargetImage->Orig->Hard.Tex[ TargetScreenTextureNo ].PF->D3D9.RenderTargetSurface :
						TargetImage->Orig->Hard.Tex[ TargetScreenTextureNo ].PF->D3D9.Surface[ TargetScreenSurface ] ;
		TargetFormat  = GD3D9.Device.Caps.TextureFormat[ TargetImage->Orig->ColorFormat ] ;
		LockEnable    = FALSE ;
	}
	else
	// それ以外の場合はサブバックバッファが有効だったらそれが対象
	{
		TargetSurface = GD3D9.Device.Screen.SubBackBufferSurface ? GD3D9.Device.Screen.SubBackBufferSurface : GD3D9.Device.Screen.BackBufferSurface ;
		TargetFormat  = GD3D9.Device.Caps.ScreenFormat ;
		LockEnable    = GD3D9.Device.Caps.UseRenderTargetLock ;
	}
	TargetColorData = Graphics_D3D9_GetD3DFormatColorData( TargetFormat ) ;

	// ロックできる場合はいきなりロック
	if( LockEnable && 
		Direct3DSurface9_LockRect( TargetSurface, &SrcLockRect, LockRect, ( DWORD )( ReadOnly ? D_D3DLOCK_READONLY : 0 ) ) == D_D3D_OK )
	{
		// ロックしたサーフェスのポインタを保存
		GD3D9.Device.Screen.DrawScreenBufferLockSurface = TargetSurface ;

		// 一時的に使用しているサーフェスのポインタをリセット
		GD3D9.Device.Screen.DrawScreenBufferLockSMSurface = NULL ;
	}
	else
	// 駄目だった場合は一度メモリに取り込んでからロック
	{
		D_IDirect3DSurface9 *RTSurface ;
		D_IDirect3DSurface9 *SMSurface ;

		// 取り込みサイズと同じ大きさの描画可能サーフェスの作成
		RTSurface = NULL ;
		if( Direct3DDevice9_CreateRenderTarget(
				( UINT )Width, ( UINT )Height, TargetFormat, D_D3DMULTISAMPLE_NONE, 0, FALSE, &RTSurface, NULL ) != D_D3D_OK )
		{
			DXST_LOGFILE_ADDUTF16LE( "\xd0\x30\xc3\x30\xaf\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\xed\x30\xc3\x30\xaf\x30\x28\x75\xcf\x63\x3b\x75\xef\x53\xfd\x80\xb5\x30\xfc\x30\xd5\x30\xa7\x30\xb9\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"バックバッファロック用描画可能サーフェスの作成に失敗しました\n" @*/ ) ;
			return -1 ;
		}

		// 取り込みサイズと同じ大きさのシステムメモリ上のオフスクリーンサーフェスの作成
		SMSurface = NULL ;
/*
		if( Direct3DDevice9_CreateOffscreenPlainSurface(
				Width, Height, TargetFormat, D_D3DPOOL_SYSTEMMEM, &SMSurface, NULL ) != D_D3D_OK )
		{
			DXST_LOGFILE_ADDUTF16LE( L"バックバッファロック用システムメモリサーフェスの作成に失敗しました\n" ) ;
			Direct3D9_ObjectRelease( RTSurface ) ;
			return -1 ;
		}
*/
		for(;;)
		{
			UseSysMemSurfaceIndex = Graphics_D3D9_GetSysMemSurface( Width, Height, TargetFormat, FALSE ) ;
			if( UseSysMemSurfaceIndex != -1 ) break ;
			if( NS_ProcessMessage() != 0 ) break ;
			Thread_Sleep( 1 ) ;
		}
		if( UseSysMemSurfaceIndex == -1 )
		{
			DXST_LOGFILE_ADDUTF16LE( "\xd0\x30\xc3\x30\xaf\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\xed\x30\xc3\x30\xaf\x30\x28\x75\xb7\x30\xb9\x30\xc6\x30\xe0\x30\xe1\x30\xe2\x30\xea\x30\xb5\x30\xfc\x30\xd5\x30\xa7\x30\xb9\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"バックバッファロック用システムメモリサーフェスの作成に失敗しました\n" @*/ ) ;
			Direct3D9_ObjectRelease( RTSurface ) ;
			return -1 ;
		}
		SMSurface = GD3D9.SysMemTexSurf.Surface[ UseSysMemSurfaceIndex ].MemSurface ;

		// まず描画可能サーフェスに内容を転送する
		DestRect.left   = 0 ;
		DestRect.top    = 0 ;
		DestRect.right  = Width ;
		DestRect.bottom = Height ;
		hr = Direct3DDevice9_StretchRect(
			TargetSurface, LockRect,
			RTSurface, &DestRect, D_D3DTEXF_NONE ) ;
		if( hr != D_D3D_OK )
		{
			// 一時的に作成したサーフェスの解放
			Direct3D9_ObjectRelease( RTSurface ) ;

			// 一時的に作成したサーフェスの解放
			Graphics_D3D9_ReleaseSysMemSurface( UseSysMemSurfaceIndex, FALSE ) ;

			// エラー終了
			return -1 ;
		}

		// その後システムメモリサーフェスに内容を転送する
		hr = Direct3DDevice9_GetRenderTargetData(
			RTSurface, SMSurface ) ;

		// システムメモリサーフェスをロック
		hr = Direct3DSurface9_LockRect( SMSurface, &SrcLockRect, NULL, ( DWORD )( ReadOnly ? D_D3DLOCK_READONLY : 0 ) ) ;

		// 一時的に使用しているサーフェスのポインタを保存
		GD3D9.Device.Screen.DrawScreenBufferLockSMSurface = SMSurface ;

		// 一時的に使用しているサーフェスキャッシュのインデックスを保存
		GD3D9.Device.Screen.DrawScreenBufferLockSMSurfaceIndex = UseSysMemSurfaceIndex ;

		// 一時的に作成したサーフェスの解放
		Direct3D9_ObjectRelease( RTSurface ) ;
	}

	// 基本イメージ構造体のデータにセット
	BaseImage->ColorData     = *TargetColorData ;
	BaseImage->Width          = Width ;
	BaseImage->Height         = Height ;
	BaseImage->Pitch          = SrcLockRect.Pitch ;
	BaseImage->GraphData      = SrcLockRect.pBits ;
	BaseImage->MipMapCount    = 0 ;
	BaseImage->GraphDataCount = 0 ;

	// 正常終了
	return 0 ;
}

// 描画先バッファをアンロックする
extern	int		Graphics_Hardware_D3D9_UnlockDrawScreenBuffer_PF( void )
{
	// 一時的に使用しているサーフェスをロックしたかどうかで処理を分岐
	if( GD3D9.Device.Screen.DrawScreenBufferLockSMSurface != NULL )
	{
		// ロックの解除
		Direct3DSurface9_UnlockRect( GD3D9.Device.Screen.DrawScreenBufferLockSMSurface ) ;

		// 一時的に作成したサーフェスの解放
		// Direct3D9_ObjectRelease( GD3D9.Device.Screen.DrawScreenBufferLockSMSurface ) ;
		Graphics_D3D9_ReleaseSysMemSurface( GD3D9.Device.Screen.DrawScreenBufferLockSMSurfaceIndex, FALSE ) ;

		// サーフェスキャッシュインデックスを無効化
		GD3D9.Device.Screen.DrawScreenBufferLockSMSurfaceIndex = -1 ;

		// ポインタを無効化
		GD3D9.Device.Screen.DrawScreenBufferLockSMSurface = NULL ;
	}
	else
	{
		// 描画対象のサーフェスのロックを解除
		Direct3DSurface9_UnlockRect( GD3D9.Device.Screen.DrawScreenBufferLockSurface ) ;
		GD3D9.Device.Screen.DrawScreenBufferLockSurface = NULL ;
	}

	// 正常終了
	return 0 ;
}

// 裏画面の内容を表画面に描画する
extern	int		Graphics_Hardware_D3D9_ScreenCopy_PF( void )
{
	// サブバックバッファのセットアップ
	if( NS_GetWindowModeFlag() == FALSE )
	{
		Graphics_D3D9_SetupSubBackBuffer() ;
	}

	// 正常終了
	return 0 ;
}

// 垂直同期信号を待つ
extern	int		Graphics_Hardware_D3D9_WaitVSync_PF( int SyncNum )
{
	return DirectDraw7_WaitVSync( SyncNum ) ;
}

// 裏画面と表画面を交換する
extern	int		Graphics_Hardware_D3D9_ScreenFlipBase_PF( void )
{
	// 特殊な条件ではフリップをキャンセルする
	if( WinData.ActiveFlag == FALSE && 
		WinData.DrawBackGraphFlag == FALSE && 
		WinData.PauseGraph.GraphData != NULL )
	{
		return 0 ;
	}

	// フルスクリーンモードで
	// フルスクリーン解像度モードが DX_FSRESOLUTIONMODE_NATIVE ではない場合や
	// GSYS.Screen/ValidGraphDisplayArea が TRUE の場合はサブバックバッファのセットアップ
	if( NS_GetWindowModeFlag() == FALSE &&
		( GSYS.Screen.FullScreenResolutionModeAct != DX_FSRESOLUTIONMODE_NATIVE ||
		  GSYS.Screen.ValidGraphDisplayArea != FALSE ) )
	{
		Graphics_D3D9_SetupSubBackBuffer() ;
	}

	// 裏画面の内容を表画面に反映
	return Direct3DDevice9_Present() ;
}

// 裏画面の指定の領域をウインドウのクライアント領域の指定の領域に転送する
extern	int		Graphics_Hardware_D3D9_BltRectBackScreenToWindow_PF( HWND Window, RECT BackScreenRect, RECT WindowClientRect )
{
	return Direct3DDevice9_BltRectBackScreenToWindow( Window, BackScreenRect, WindowClientRect ) ;
}

// ScreenFlip で画像を転送する先のウインドウを設定する( NULL を指定すると設定解除 )
extern	int		Graphics_Hardware_D3D9_SetScreenFlipTargetWindow_PF( HWND /*TargetWindow*/, double /*ScaleX*/, double /*ScaleY*/ )
{
	return 0 ;
}

// メイン画面のＺバッファの設定を変更する
extern	int		Graphics_Hardware_D3D9_SetZBufferMode_PF( int ZBufferSizeX, int ZBufferSizeY, int ZBufferBitDepth )
{
	HRESULT hr ;
	D_IDirect3DSurface9 *SetZBuffer ;
	int UseScreenZBuffer = FALSE ;
	int BitIndex ;

	// 値のチェックと保存
	if( ZBufferSizeX > 0 && ZBufferSizeY > 0 )
	{
		for( GD3D9.Device.Screen.ZBufferSizeX = 1 ; GD3D9.Device.Screen.ZBufferSizeX < ZBufferSizeX ; GD3D9.Device.Screen.ZBufferSizeX <<= 1 ){}
		for( GD3D9.Device.Screen.ZBufferSizeY = 1 ; GD3D9.Device.Screen.ZBufferSizeY < ZBufferSizeY ; GD3D9.Device.Screen.ZBufferSizeY <<= 1 ){}
		GD3D9.Setting.UserZBufferSizeSet = TRUE ;
	}
	if( ZBufferBitDepth > 0 )
	{
		if( ZBufferBitDepth != 16 && ZBufferBitDepth != 24 && ZBufferBitDepth != 32 )
		{
			return -1 ;
		}
		GD3D9.Device.Screen.ZBufferBitDepth = ZBufferBitDepth ;
		GD3D9.Setting.UserZBufferBitDepthSet = TRUE ;
	}

	// 初期化前の場合は値だけ設定して終了
	if( GAPIWin.Direct3DDevice9Object == NULL )
		return 0 ;

	// 現在設定されているＺバッファを取得
	Direct3DDevice9_GetDepthStencilSurface( &SetZBuffer ) ;

	// もし画面用Ｚバッファが使用されている場合は一旦外す
	if( SetZBuffer == GD3D9.Device.Screen.ZBufferSurface )
	{
		Direct3DDevice9_SetDepthStencilSurface( NULL ) ;
		UseScreenZBuffer = TRUE ;
	}
	Direct3D9_ObjectRelease( SetZBuffer ) ;
	SetZBuffer = NULL ;

	// Zバッファを開放
	if( GD3D9.Device.Screen.ZBufferSurface )
	{
		Direct3D9_ObjectRelease( GD3D9.Device.Screen.ZBufferSurface ) ;
		GD3D9.Device.Screen.ZBufferSurface = NULL ;
	}

	// 新たなＺバッファの作成
	switch( GD3D9.Device.Screen.ZBufferBitDepth )
	{
	default:
	case 16 : BitIndex = 0 ; break ;
	case 24 : BitIndex = 1 ; break ;
	case 32 : BitIndex = 2 ; break ;
	}
	hr = Direct3DDevice9_CreateDepthStencilSurface(
		( UINT )GD3D9.Device.Screen.ZBufferSizeX,
		( UINT )GD3D9.Device.Screen.ZBufferSizeY,
		GD3D9.Device.Caps.ZBufferFormat[ BitIndex ],
		( D_D3DMULTISAMPLE_TYPE )GSYS.Setting.FSAAMultiSampleCount,
		( DWORD )GSYS.Setting.FSAAMultiSampleQuality,
		TRUE,
		&GD3D9.Device.Screen.ZBufferSurface,
		NULL
	) ;
	if( FAILED( hr ) )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x3a\xff\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Ｚバッファの作成に失敗しました\n" @*/ ) ;
		return -1 ;
	}

	// もし画面用Ｚバッファが使用されている場合は再セット
	if( UseScreenZBuffer )
	{
		Direct3DDevice9_SetDepthStencilSurface( GD3D9.Device.Screen.ZBufferSurface ) ;

		// クリア
		Direct3DDevice9_Clear( 0, NULL, D_D3DCLEAR_ZBUFFER, 0, 1.0f, 0 ) ;
	}

	// 正常終了
	return 0 ;
}

// 描画先Ｚバッファのセット
extern	int		Graphics_Hardware_D3D9_SetDrawZBuffer_PF( int /* DrawScreen */, IMAGEDATA *Image )
{
	// 画像かどうかで処理を分岐
	if( Image != NULL )
	{
		// 画像だった場合は描画可能では無い場合やＺバッファを持っていない画像の場合はエラー
		if( Image->Orig->FormatDesc.DrawValidFlag == FALSE ||
			Image->Hard.Draw[ 0 ].Tex->PF->D3D9.ZBuffer == NULL )
			return -1 ;
	}

	// 正常終了
	return 0 ;
}

























// 環境依存情報取得関係

// GetColor や GetColor2 で使用するカラーデータを取得する
extern const COLORDATA *Graphics_Hardware_D3D9_GetMainColorData_PF( void )
{
	return Graphics_D3D9_GetD3DFormatColorData( GSYS.Screen.MainScreenColorBitDepth == 16 ? D_D3DFMT_R5G6B5 : D_D3DFMT_X8R8G8B8 ) ;
}

// ディスプレーのカラーデータポインタを得る
extern	const COLORDATA *Graphics_Hardware_D3D9_GetDispColorData_PF( void )
{
	return Graphics_D3D9_GetD3DFormatColorData( GD3D9.Device.Caps.ScreenFormat ) ;
}

// 指定座標の色を取得する
extern	DWORD Graphics_Hardware_D3D9_GetPixel_PF( int x, int y )
{
	RECT            SrcRect ;
//	const COLORDATA *DestColorData ;
	BASEIMAGE       BufferImage ;
	DWORD           Ret = 0xffffffff ;
	int	Red = 0, Green = 0, Blue = 0, Alpha = 0 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	Graphics_D3D9_EndScene();

	// 描画先バッファをロック
	SrcRect.left   = x ;
	SrcRect.right  = x + 1 ;
	SrcRect.top    = y ;
	SrcRect.bottom = y + 1 ;
	if( Graphics_Screen_LockDrawScreen( &SrcRect, &BufferImage, -1, -1, -1, TRUE, 0 ) < 0 )
	{
		return Ret ;
	}

	if( BufferImage.ColorData.FloatTypeFlag )
	{
		float RedF = 0.0f, GreenF = 0.0f, BlueF = 0.0f, AlphaF = 0.0f ;

		switch( BufferImage.ColorData.ChannelBitDepth )
		{
		case 16 :
			RedF   =                                        Float16ToFloat32( ( ( WORD * )BufferImage.GraphData )[ 0 ] ) ;
			GreenF = BufferImage.ColorData.ChannelNum > 1 ? Float16ToFloat32( ( ( WORD * )BufferImage.GraphData )[ 1 ] ) : 1.0f ;
			BlueF  = BufferImage.ColorData.ChannelNum > 2 ? Float16ToFloat32( ( ( WORD * )BufferImage.GraphData )[ 2 ] ) : 1.0f ;
			AlphaF = BufferImage.ColorData.ChannelNum > 3 ? Float16ToFloat32( ( ( WORD * )BufferImage.GraphData )[ 3 ] ) : 1.0f ;
			break ;

		case 32 :
			RedF   =                                        ( ( float * )BufferImage.GraphData )[ 0 ] ;
			GreenF = BufferImage.ColorData.ChannelNum > 1 ? ( ( float * )BufferImage.GraphData )[ 1 ] : 1.0f ;
			BlueF  = BufferImage.ColorData.ChannelNum > 2 ? ( ( float * )BufferImage.GraphData )[ 2 ] : 1.0f ;
			AlphaF = BufferImage.ColorData.ChannelNum > 3 ? ( ( float * )BufferImage.GraphData )[ 3 ] : 1.0f ;
			break ;
		}

		Red   = RedF   <= 0.0f ? 0 : ( RedF   >= 1.0f ? 255 : _FTOL( RedF   * 255.0f ) ) ;
		Green = GreenF <= 0.0f ? 0 : ( GreenF >= 1.0f ? 255 : _FTOL( GreenF * 255.0f ) ) ;
		Blue  = BlueF  <= 0.0f ? 0 : ( BlueF  >= 1.0f ? 255 : _FTOL( BlueF  * 255.0f ) ) ;
		Alpha = AlphaF <= 0.0f ? 0 : ( AlphaF >= 1.0f ? 255 : _FTOL( AlphaF * 255.0f ) ) ;
	}
	else
	{
		switch( BufferImage.ColorData.ColorBitDepth )
		{
		case 16 :
			NS_GetColor5( &BufferImage.ColorData, *( ( WORD   * )BufferImage.GraphData ), &Red, &Green, &Blue, &Alpha ) ;
			break ;

		case 32 :
			NS_GetColor5( &BufferImage.ColorData, *( ( DWORD  * )BufferImage.GraphData ), &Red, &Green, &Blue, &Alpha ) ;
			break ;
		}
	}

	Ret = 0xff000000 | ( ( ( unsigned int )Red ) << 16 ) | ( ( ( unsigned int )Green ) << 8 ) | ( ( unsigned int )Blue ) ;

	// 描画先バッファをアンロック
	Graphics_Screen_UnlockDrawScreen() ;

	// 色を返す
	return Ret ;
}

// 指定座標の色を取得する( float型 )
extern COLOR_F Graphics_Hardware_D3D9_GetPixelF_PF( int x, int y )
{
	RECT            SrcRect ;
//	const COLORDATA *DestColorData ;
	BASEIMAGE       BufferImage ;
	COLOR_F			Result = { 0.0f } ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	Graphics_D3D9_EndScene();

	// 描画先バッファをロック
	SrcRect.left   = x ;
	SrcRect.right  = x + 1 ;
	SrcRect.top    = y ;
	SrcRect.bottom = y + 1 ;
	if( Graphics_Screen_LockDrawScreen( &SrcRect, &BufferImage, -1, -1, -1, TRUE, 0 ) < 0 )
	{
		return NS_GetColorF( -1.0f, -1.0f, -1.0f, -1.0f ) ;
	}

	if( BufferImage.ColorData.FloatTypeFlag )
	{
		switch( BufferImage.ColorData.ChannelBitDepth )
		{
		case 16 :
			Result.r =                                        Float16ToFloat32( ( ( WORD * )BufferImage.GraphData )[ 0 ] ) ;
			Result.g = BufferImage.ColorData.ChannelNum > 1 ? Float16ToFloat32( ( ( WORD * )BufferImage.GraphData )[ 1 ] ) : 1.0f ;
			Result.b = BufferImage.ColorData.ChannelNum > 2 ? Float16ToFloat32( ( ( WORD * )BufferImage.GraphData )[ 2 ] ) : 1.0f ;
			Result.a = BufferImage.ColorData.ChannelNum > 3 ? Float16ToFloat32( ( ( WORD * )BufferImage.GraphData )[ 3 ] ) : 1.0f ;
			break ;

		case 32 :
			Result.r =                                        ( ( float * )BufferImage.GraphData )[ 0 ] ;
			Result.g = BufferImage.ColorData.ChannelNum > 1 ? ( ( float * )BufferImage.GraphData )[ 1 ] : 1.0f ;
			Result.b = BufferImage.ColorData.ChannelNum > 2 ? ( ( float * )BufferImage.GraphData )[ 2 ] : 1.0f ;
			Result.a = BufferImage.ColorData.ChannelNum > 3 ? ( ( float * )BufferImage.GraphData )[ 3 ] : 1.0f ;
			break ;
		}
	}
	else
	{
		int Red = 0, Green = 0, Blue = 0, Alpha = 0 ;

		switch( BufferImage.ColorData.ColorBitDepth )
		{
		case 16 :
			NS_GetColor5( &BufferImage.ColorData, *( ( WORD   * )BufferImage.GraphData ), &Red, &Green, &Blue, &Alpha ) ;
			break ;

		case 32 :
			NS_GetColor5( &BufferImage.ColorData, *( ( DWORD  * )BufferImage.GraphData ), &Red, &Green, &Blue, &Alpha ) ;
			break ;
		}

		Result.r = ( float )Red   / 255.0f ;
		Result.g = ( float )Green / 255.0f ;
		Result.b = ( float )Blue  / 255.0f ;
		Result.a = ( float )Alpha / 255.0f ;
	}

	// 描画先バッファをアンロック
	Graphics_Screen_UnlockDrawScreen() ;

	// 色を返す
	return Result ;
}

























// 環境依存画像関係

// YUVサーフェスを使った Theora 動画の内容をグラフィックスハンドルのテクスチャに転送する
extern	int		Graphics_Hardware_D3D9_UpdateGraphMovie_TheoraYUV_PF( MOVIEGRAPH *Movie, IMAGEDATA *Image )
{
#ifndef DX_NON_OGGTHEORA
	RECT                 Rect ;
	D_IDirect3DSurface9 *Surface ;
	D_IDirect3DSurface9 *YUY2Image ;

	if( GAPIWin.Direct3DDevice9Object == NULL )
	{
		return -1 ;
	}

	YUY2Image = ( D_IDirect3DSurface9 * )TheoraDecode_GetYUVImage( Movie->TheoraHandle ) ;

	// ビットマップデータを分割されているテクスチャーに貼り付ける
	SETRECT( Rect, 0, 0, Image->Orig->Hard.Tex[ 0 ].UseWidth, Image->Orig->Hard.Tex[ 0 ].UseHeight ) ;

	// グラフィックの転送
	if( Direct3DTexture9_GetSurfaceLevel( Image->Orig->Hard.Tex[ 0 ].PF->D3D9.Texture, 0, &Surface ) == D_D3D_OK )
	{
		HRESULT hr ;
		hr = Direct3DDevice9_StretchRect( YUY2Image, &Rect, Surface, &Rect, D_D3DTEXF_NONE ) ;
		Direct3D9_ObjectRelease( Surface ) ;
	}
#endif

	// 正常終了
	return 0 ;
}

// グラフィックメモリ領域のロック
extern	int		Graphics_Hardware_D3D9_GraphLock_PF( IMAGEDATA *Image, COLORDATA **ColorDataP, int WriteOnly )
{
	IMAGEDATA_HARD_DRAW *DrawTex ;
	COLORDATA           *ColorData ;
	BYTE                *Dest ;
	BYTE                *Src ;
	DWORD                WidthByte ;
	HRESULT              hr ;
	D_D3DLOCKED_RECT     LockRect ;
	D_IDirect3DSurface9 *SMSurface ;
	int                  i ;
	RECT                 Rect ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画を終了しておく
	Graphics_D3D9_EndScene() ;

	// テクスチャのフォーマットを取得
	ColorData = Graphics_D3D9_GetD3DFormatColorData( GD3D9.Device.Caps.TextureFormat[ Image->Orig->ColorFormat ] ) ;

	// ロック時のテンポラリイメージを格納するためのメモリを確保
	Image->LockImagePitch = ( DWORD )( Image->WidthI * ColorData->PixelByte ) ;
	Image->LockImage = ( BYTE * )DXALLOC( Image->HeightI * Image->LockImagePitch ) ;
	if( Image->LockImage == NULL )
	{
		DXST_LOGFILE_ADDUTF16LE( "\xed\x30\xc3\x30\xaf\x30\x28\x75\xc6\x30\xf3\x30\xdd\x30\xe9\x30\xea\x30\xa4\x30\xe1\x30\xfc\x30\xb8\x30\x3c\x68\x0d\x7d\x28\x75\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ロック用テンポラリイメージ格納用メモリの確保に失敗しました\n" @*/ ) ;
		return -1 ;
	}

	// 書き込み専用ではない場合はテンポラリバッファにデータを転送する
	if( WriteOnly == FALSE )
	{
		// 描画情報の数だけ繰り返し
		DrawTex = Image->Hard.Draw ;
		SMSurface = NULL ;
		for( i = 0 ; i < Image->Hard.DrawNum ; i ++, DrawTex ++ )
		{
			// Direct3D9Ex を使用しているか、描画可能テクスチャの場合は一度システムメモリサーフェスにデータを転送してからロックする
			if( Direct3D9_IsExObject() || Image->Orig->FormatDesc.DrawValidFlag )
			{
				D_IDirect3DSurface9 *RTSurface ;
				RECT DestRect ;

				// 取り込みサイズと同じ大きさの描画可能サーフェスの作成
				RTSurface = NULL ;
				if( Direct3DDevice9_CreateRenderTarget(
						( UINT )DrawTex->WidthI,
						( UINT )DrawTex->HeightI,
						GD3D9.Device.Caps.TextureFormat[ Image->Orig->ColorFormat ],
						D_D3DMULTISAMPLE_NONE,
						0,
						FALSE,
						&RTSurface,
						NULL
					) != D_D3D_OK )
				{
					DXST_LOGFILE_ADDUTF16LE( "\xed\x30\xc3\x30\xaf\x30\x28\x75\xcf\x63\x3b\x75\xef\x53\xfd\x80\xb5\x30\xfc\x30\xd5\x30\xa7\x30\xb9\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ロック用描画可能サーフェスの作成に失敗しました\n" @*/ ) ;
					return -1 ;
				}

				// 取り込みサイズと同じ大きさのシステムメモリ上のオフスクリーンサーフェスの作成
				if( Direct3DDevice9_CreateOffscreenPlainSurface(
						( UINT )DrawTex->WidthI,
						( UINT )DrawTex->HeightI,
						GD3D9.Device.Caps.TextureFormat[ Image->Orig->ColorFormat ],
						D_D3DPOOL_SYSTEMMEM,
						&SMSurface,
						NULL
					) != D_D3D_OK )
				{
					DXST_LOGFILE_ADDUTF16LE( "\xed\x30\xc3\x30\xaf\x30\x28\x75\xb7\x30\xb9\x30\xc6\x30\xe0\x30\xe1\x30\xe2\x30\xea\x30\xb5\x30\xfc\x30\xd5\x30\xa7\x30\xb9\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ロック用システムメモリサーフェスの作成に失敗しました\n" @*/ ) ;
					Direct3D9_ObjectRelease( RTSurface ) ;
					return -1 ;
				}

				// まず描画可能サーフェスに内容を転送する
				Rect.left       = DrawTex->UsePosXI ;
				Rect.top        = DrawTex->UsePosYI ;
				Rect.right      = DrawTex->UsePosXI + DrawTex->WidthI ;
				Rect.bottom     = DrawTex->UsePosYI + DrawTex->HeightI ;
				DestRect.left   = 0 ;
				DestRect.top    = 0 ;
				DestRect.right  = DrawTex->WidthI ;
				DestRect.bottom = DrawTex->HeightI ;
				hr = Direct3DDevice9_StretchRect(
					DrawTex->Tex->PF->D3D9.Surface[ 0 ], &Rect,
					RTSurface, &DestRect, D_D3DTEXF_NONE ) ;

				// その後システムメモリサーフェスに内容を転送する
				hr = Direct3DDevice9_GetRenderTargetData( RTSurface, SMSurface ) ;

				// 一時的に作成したサーフェスの解放
				Direct3D9_ObjectRelease( RTSurface ) ;

				// システムメモリーフェスをロック
				Rect.left   = 0 ;
				Rect.top    = 0 ;
				Rect.right  = DrawTex->WidthI ;
				Rect.bottom = DrawTex->HeightI ;
				hr = Direct3DSurface9_LockRect( SMSurface, &LockRect, &Rect, D_D3DLOCK_READONLY ) ;
			}
			else
			{
				// それ以外の場合はいきなりロック
				Rect.left   = DrawTex->UsePosXI ;
				Rect.top    = DrawTex->UsePosYI ;
				Rect.right  = DrawTex->UsePosXI + DrawTex->WidthI ;
				Rect.bottom = DrawTex->UsePosYI + DrawTex->HeightI ;
				hr = Direct3DSurface9_LockRect( DrawTex->Tex->PF->D3D9.Surface[ 0 ], &LockRect, &Rect, D_D3DLOCK_READONLY ) ;
			}

			// 転送
			Src = ( BYTE * )LockRect.pBits ;
			Dest = Image->LockImage + DrawTex->DrawPosXI * ColorData->PixelByte + DrawTex->DrawPosYI * Image->LockImagePitch ;
			WidthByte = ( DWORD )( DrawTex->WidthI * ColorData->PixelByte ) ;
			for( i = 0 ; i < DrawTex->HeightI ; i ++, Dest += Image->LockImagePitch, Src += LockRect.Pitch )
			{
				_MEMCPY( Dest, Src, WidthByte ) ;
			}

			// ロックの解除
			if( SMSurface )
			{
				Direct3DSurface9_UnlockRect( SMSurface ) ;
				Direct3D9_ObjectRelease( SMSurface ) ;
			}
			else
			{
				Direct3DSurface9_UnlockRect( DrawTex->Tex->PF->D3D9.Surface[ 0 ] ) ;
			}
		}
	}

	// カラーデータのアドレスを保存
	*ColorDataP = ColorData ;

	// 正常終了
	return 0 ;
}

// グラフィックメモリ領域のロック解除
extern	int		Graphics_Hardware_D3D9_GraphUnlock_PF( IMAGEDATA *Image )
{
	IMAGEDATA_HARD_DRAW *DrawTex ;
	int                  i ;
	int                  j ;
	RECT                 Rect ;
	RECT                 SrcRect ;
	COLORDATA           *ColorData ;
	BYTE                *Dest ;
	BYTE                *Src ;
	DWORD                WidthByte ;
	POINT                DestPoint ;
	HRESULT              hr ;
	D_D3DLOCKED_RECT     LockRect ;
	D_IDirect3DSurface9 *SMSurface ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画を終了しておく
	Graphics_D3D9_EndScene() ;

	// テクスチャのフォーマットを取得
	ColorData = Graphics_D3D9_GetD3DFormatColorData( GD3D9.Device.Caps.TextureFormat[ Image->Orig->ColorFormat ] ) ;

	// 描画情報の数だけ繰り返し
	DrawTex = Image->Hard.Draw ;
	SMSurface = NULL ;
	for( i = 0 ; i < Image->Hard.DrawNum ; i ++, DrawTex ++ )
	{
		// Direct3D9Ex を使用しているか、又は描画可能テクスチャかどうかで処理を分岐
		if( Direct3D9_IsExObject() || Image->Orig->FormatDesc.DrawValidFlag )
		{
			// Direct3D9Ex を使用しているか、又は描画可能テクスチャの場合は一度システムメモリサーフェスにデータを転送してから更新する
			if( Direct3DDevice9_CreateOffscreenPlainSurface( ( UINT )DrawTex->WidthI, ( UINT )DrawTex->HeightI, GD3D9.Device.Caps.TextureFormat[ Image->Orig->ColorFormat ], D_D3DPOOL_SYSTEMMEM, &SMSurface, NULL ) != D_D3D_OK )
			{
				DXST_LOGFILE_ADDUTF16LE( "\xed\x30\xc3\x30\xaf\x30\x28\x75\xc6\x30\xf3\x30\xdd\x30\xe9\x30\xea\x30\x6e\x30\xb7\x30\xb9\x30\xc6\x30\xe0\x30\xe1\x30\xe2\x30\xea\x30\x4d\x91\x6e\x7f\xb5\x30\xfc\x30\xd5\x30\xa7\x30\xb9\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ロック用テンポラリのシステムメモリ配置サーフェスの作成に失敗しました\n" @*/ ) ;
				return -1 ;
			}

			// 作成したテクスチャをロック
			hr = Direct3DSurface9_LockRect( SMSurface, &LockRect, NULL, 0 ) ;

			// データを転送
			Src       = Image->LockImage + ColorData->PixelByte * DrawTex->DrawPosXI + Image->LockImagePitch * DrawTex->DrawPosYI ;
			Dest      = ( BYTE * )LockRect.pBits ;
			WidthByte = ( DWORD )( Image->WidthI * ColorData->PixelByte ) ;
			for( j = 0 ; j < DrawTex->HeightI ; j ++, Src += Image->LockImagePitch, Dest += LockRect.Pitch )
			{
				_MEMCPY( Dest, Src, WidthByte ) ;
			}

			// ロック解除
			Direct3DSurface9_UnlockRect( SMSurface ) ;

			// システムメモリサーフェスから描画可能テクスチャに転送
			SrcRect.left = 0 ;
			SrcRect.top = 0 ;
			SrcRect.right = DrawTex->WidthI ;
			SrcRect.bottom = DrawTex->HeightI ;
			DestPoint.x = DrawTex->UsePosXI ;
			DestPoint.y = DrawTex->UsePosYI ;
			hr = Direct3DDevice9_UpdateSurface(
				SMSurface,                  &SrcRect,
				DrawTex->Tex->PF->D3D9.Surface[ 0 ], &DestPoint ) ;

			// システムメモリサーフェス解放
			if( SMSurface )
			{
				Direct3D9_ObjectRelease( SMSurface ) ;
				SMSurface = NULL ;
			}
		}
		else
		{
			// 通常のテクスチャの場合はロックして転送する

			// ロック
			Rect.left   = DrawTex->UsePosXI ;
			Rect.top    = DrawTex->UsePosYI ;
			Rect.right  = DrawTex->UsePosXI + DrawTex->WidthI ;
			Rect.bottom = DrawTex->UsePosYI + DrawTex->HeightI ;
			hr = Direct3DSurface9_LockRect( DrawTex->Tex->PF->D3D9.Surface[ 0 ], &LockRect, &Rect, 0 ) ;

			// 転送
			Src       = Image->LockImage + DrawTex->DrawPosXI * ColorData->PixelByte + DrawTex->DrawPosYI * Image->LockImagePitch ;
			Dest      = ( BYTE * )LockRect.pBits ;
			WidthByte = ( DWORD )( DrawTex->WidthI * ColorData->PixelByte ) ;
			for( i = 0 ; i < DrawTex->HeightI ; i ++, Src += Image->LockImagePitch, Dest += LockRect.Pitch )
			{
				_MEMCPY( Dest, Src, WidthByte ) ;
			}

			// ロックの解除
			Direct3DSurface9_UnlockRect( DrawTex->Tex->PF->D3D9.Surface[ 0 ] ) ;
		}
	}

	// ロックイメージの解放
	DXFREE( Image->LockImage ) ;
	Image->LockImage = NULL ;

	// 正常終了
	return 0 ;
}

// グラフィックのＺバッファの状態を別のグラフィックのＺバッファにコピーする( DestGrHandle も SrcGrHandle もＺバッファを持っている描画可能画像で、且つアンチエイリアス画像ではないことが条件 )
extern	int		Graphics_Hardware_D3D9_CopyGraphZBufferImage_PF( IMAGEDATA *DestImage, IMAGEDATA *SrcImage )
{
	// 描画可能ではない場合も何もしない
	if( SrcImage->Orig->FormatDesc.DrawValidFlag == FALSE ||
		DestImage->Orig->FormatDesc.DrawValidFlag == FALSE )
		return -1 ;

	// アンチエイリアスレンダリングターゲットの場合も何もしない
	if( SrcImage->Orig->FormatDesc.MSSamples != D_D3DMULTISAMPLE_NONE ||
		DestImage->Orig->FormatDesc.MSSamples != D_D3DMULTISAMPLE_NONE )
		return -1 ;

	// Ｚバッファが無い場合も何もしない
	if( SrcImage->Orig->Hard.Tex[ 0 ].PF->D3D9.ZBuffer == NULL ||
		DestImage->Orig->Hard.Tex[ 0 ].PF->D3D9.ZBuffer == NULL )
		return -1 ;

	// サイズが違う場合も何もしない
	if( SrcImage->Orig->Hard.Tex[ 0 ].TexWidth != DestImage->Orig->Hard.Tex[ 0 ].TexWidth ||
		SrcImage->Orig->Hard.Tex[ 0 ].TexHeight != DestImage->Orig->Hard.Tex[ 0 ].TexHeight )
		return -1 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画を終わらせておく
	Graphics_D3D9_EndScene() ;

	// 転送
	if( Direct3DDevice9_StretchRect( SrcImage->Orig->Hard.Tex[ 0 ].PF->D3D9.ZBuffer, NULL, DestImage->Orig->Hard.Tex[ 0 ].PF->D3D9.ZBuffer, NULL, D_D3DTEXF_NONE ) != D_D3D_OK )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x5a\x00\xd0\x30\xc3\x30\xd5\x30\xa1\x30\xfc\x30\xa4\x30\xe1\x30\xfc\x30\xb8\x30\x6e\x30\xe2\x8e\x01\x90\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Zバッファーイメージの転送に失敗しました\n" @*/ ) ;
		return -1 ;
	}

	// 正常終了
	return 0 ;
}

// 画像データの初期化
extern	int		Graphics_Hardware_D3D9_InitGraph_PF( void )
{
	// システムメモリテクスチャの解放
	Graphics_D3D9_ReleaseSysMemTextureAll() ;

	// システムメモリサーフェスの解放
	Graphics_D3D9_ReleaseSysMemSurfaceAll() ;

	// 正常終了
	return 0 ;
}

// グラフィックを特定の色で塗りつぶす
extern	int		Graphics_Hardware_D3D9_FillGraph_PF( IMAGEDATA *Image, int Red, int Green, int Blue, int Alpha, int ASyncThread )
{
	IMAGEDATA_HARD_DRAW *DrawTex ;
	int                  i ;
	DWORD                Code ;
	RECT                 Rect ;

	// デバイスが無かったらエラー
	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;

	// カラーコードの作成
	Code = ( ( DWORD )Alpha << 24 ) | ( ( DWORD )Red << 16 ) | ( ( DWORD )Green << 8 ) | ( ( DWORD )Blue << 0 ) ;

	// 描画情報の数だけ繰り返し
	if( Image->Orig->FormatDesc.DrawValidFlag )
	{
		// 描画待機している描画物を描画
		DRAWSTOCKINFO

		// 描画の終了
		Graphics_D3D9_EndScene() ;

		DrawTex = Image->Hard.Draw ;
		for( i = 0 ; i < Image->Hard.DrawNum ; i ++, DrawTex ++ )
		{
			// 使用域を指定色で塗りつぶし
			Rect.left   = DrawTex->UsePosXI ;
			Rect.top    = DrawTex->UsePosYI ;
			Rect.right  = DrawTex->UsePosXI + DrawTex->WidthI ;
			Rect.bottom = DrawTex->UsePosYI + DrawTex->HeightI ;
			Direct3DDevice9_ColorFill_ASync( DrawTex->Tex->PF->D3D9.Surface[ 0 ], &Rect, Code, ASyncThread ) ;
		}
	}
	else
	{
		return 2 ;
	}

	// 正常終了
	return 0 ;
}

// 描画可能画像やバックバッファから指定領域のグラフィックを取得する
extern	int		Graphics_Hardware_D3D9_GetDrawScreenGraphBase_PF( IMAGEDATA *Image, IMAGEDATA *TargetImage, int TargetScreen, int TargetScreenSurface, int TargetScreenMipLevel, int /* TargetScreenWidth */, int /* TargetScreenHeight */, int x1, int y1, int x2, int y2, int destX, int destY )
{
	RECT SrcRect ;
	RECT DestRect ;
	RECT Rect ;
	int Width, Height ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画処理を完了しておく
	Graphics_D3D9_EndScene() ;

	// 幅と高さを計算
	Width  = x2 - x1 ;
	Height = y2 - y1 ;

	// 転送先となるサーフェスが描画可能テクスチャだった場合は単純に転送
	if( Image->Orig->FormatDesc.DrawValidFlag )
	{
		D_IDirect3DSurface9 *TargetSurface ;

		// 取り込み元となるサーフェスの決定

		// 描画可能画像が対象の場合
		if( TargetImage )
		{
			TargetSurface = TargetImage->Orig->Hard.Tex[ 0 ].PF->D3D9.RenderTargetSurface ?
							TargetImage->Orig->Hard.Tex[ 0 ].PF->D3D9.RenderTargetSurface :
							TargetImage->Orig->Hard.Tex[ 0 ].PF->D3D9.Surface[ TargetScreenSurface ] ;
		}
		else
		// それ以外の場合はサブバックバッファが有効だったらそれが対象
		{
			TargetSurface = GD3D9.Device.Screen.SubBackBufferSurface ? GD3D9.Device.Screen.SubBackBufferSurface : GD3D9.Device.Screen.BackBufferSurface ;
		}

		SrcRect.left   = x1 ;
		SrcRect.top    = y1 ;
		SrcRect.right  = x2 ;
		SrcRect.bottom = y2 ;

		DestRect.left   = destX ;
		DestRect.top    = destY ;
		DestRect.right  = destX + Width ;
		DestRect.bottom = destY + Height ;
		if( TargetSurface != NULL && Image->Orig->Hard.Tex[ 0 ].PF->D3D9.Surface[ 0 ] != NULL )
		{
			Direct3DDevice9_StretchRect(
				TargetSurface,                               &SrcRect,
				Image->Orig->Hard.Tex[ 0 ].PF->D3D9.Surface[ 0 ], &DestRect, D_D3DTEXF_NONE ) ;
		}
	}
	else
	// 通常のテクスチャだった場合は最初にロックを試し、駄目だったら一度メモリに取り込んでから転送
	{
		// 描画先バッファをロック
		BASEIMAGE LockImage ;
		SrcRect.left   = x1 ;
		SrcRect.top    = y1 ;
		SrcRect.right  = x2 ;
		SrcRect.bottom = y2 ;
		if( Graphics_Screen_LockDrawScreen( &SrcRect, &LockImage, TargetScreen, TargetScreenSurface, TargetScreenMipLevel, TRUE, 0 ) < 0 )
		{
			return -1 ;
		}

		// テクスチャに転送
		Rect.left   = 0 ;
		Rect.top    = 0 ;
		Rect.right  = LockImage.Width ;
		Rect.bottom = LockImage.Height ;
		Graphics_Image_BltBmpOrBaseImageToGraph3(
			&Rect,
			destX, destY,
			Image->HandleInfo.Handle,
			&LockImage, NULL,
			FALSE,
			FALSE,
			FALSE
		) ;

		// 描画先バッファのアンロック
		Graphics_Screen_UnlockDrawScreen() ;
	}

	// 正常終了
	return 0 ;
}

static int Graphics_D3D9_BltBmpOrBaseImageToGraph3_MipMapBlt(
	      IMAGEDATA_ORIG			*Orig,
	const RECT						*SrcRect,
	const RECT						*DestRect,
	      int						UseSysMemSurface,
	      D_IDirect3DBaseTexture9	*UseTex,
	      int						TexWidth,
	      int						TexHeight,
	const BASEIMAGE					*RgbBaseImage,
	const BASEIMAGE					*AlphaBaseImage,
	const COLORDATA					*DestColor,
	      int						RedIsAlphaFlag,
	      int						UseTransColorConvAlpha,
	      int						ASyncThread
)
{
	int					AlphaImageSize ;
	int					ImageSize ;
	int					ImageW ;
	int					ImageH ;
	int					ImagePitch ;
	void				*Image1 ;
	void				*Image2 ;
	void				*ImageD ;
	void				*ImageS ;
	void				*ImageBuffer ;
	int					ImageDW ;
	int					ImageDH ;
	int					ImageSW ;
	int					ImageSH ;
	int					ImageNum ;
	RECT				DestRectT ;
	RECT				SrcRectT ;
	D_D3DLOCKED_RECT	LockRect ;
	POINT				DestPoint ;
	int					UseSysMemSurfaceIndex = -1 ;
	D_IDirect3DSurface9	*DestSurface = NULL ;
	D_IDirect3DSurface9	*UseSurface = NULL ;
	int					TempTexWidth ;
	int					TempTexHeight ;
	int					i ;
	int					j ;
	int					ts ;
	int					IsDXTFormat ;
	HRESULT				hr ;

	DestPoint.x = 0 ;
	DestPoint.y = 0 ;

	IsDXTFormat =
		( RgbBaseImage->ColorData.Format == DX_BASEIMAGE_FORMAT_DXT1 ) ||
		( RgbBaseImage->ColorData.Format == DX_BASEIMAGE_FORMAT_DXT2 ) ||
		( RgbBaseImage->ColorData.Format == DX_BASEIMAGE_FORMAT_DXT3 ) ||
		( RgbBaseImage->ColorData.Format == DX_BASEIMAGE_FORMAT_DXT4 ) ||
		( RgbBaseImage->ColorData.Format == DX_BASEIMAGE_FORMAT_DXT5 ) ;

	ImageNum = Orig->FormatDesc.CubeMapTextureFlag ? CUBEMAP_SURFACE_NUM : 1 ;

	// 転送先がテクスチャ全体で、且つDXTフォーマットで転送先もDXTフォーマットの場合は単純なメモリ転送を行う
	if( DestRect->left == 0 && DestRect->right  == TexWidth  &&
		DestRect->top  == 0 && DestRect->bottom == TexHeight &&
		RgbBaseImage->MipMapCount >= Orig->Hard.MipMapCount &&
		( ( RgbBaseImage->ColorData.Format == DX_BASEIMAGE_FORMAT_DXT1 && GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ] == D_D3DFMT_DXT1 ) ||
		  ( RgbBaseImage->ColorData.Format == DX_BASEIMAGE_FORMAT_DXT2 && GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ] == D_D3DFMT_DXT2 ) ||
		  ( RgbBaseImage->ColorData.Format == DX_BASEIMAGE_FORMAT_DXT3 && GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ] == D_D3DFMT_DXT3 ) ||
		  ( RgbBaseImage->ColorData.Format == DX_BASEIMAGE_FORMAT_DXT4 && GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ] == D_D3DFMT_DXT4 ) ||
		  ( RgbBaseImage->ColorData.Format == DX_BASEIMAGE_FORMAT_DXT5 && GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ] == D_D3DFMT_DXT5 ) ) )
	{
		if( Orig->FormatDesc.CubeMapTextureFlag )
		{
			ImageBuffer = RgbBaseImage->GraphData ;
			for( i = 0 ; i < CUBEMAP_SURFACE_NUM ; i ++ )
			{
				TempTexWidth  = TexWidth ;
				TempTexHeight = TexHeight ;
				for( j = 0 ; j < Orig->Hard.MipMapCount ; j ++ )
				{
					if( IsDXTFormat )
					{
						ImageSize = ( ( TempTexWidth + 3 ) / 4 ) * ( ( TempTexHeight + 3 ) / 4 ) * ( RgbBaseImage->ColorData.ColorBitDepth * 2 ) ;
					}
					else
					{
						ImageSize = TempTexWidth * TempTexHeight * RgbBaseImage->ColorData.PixelByte ;
					}

					// テクスチャをロック
					if( UseSysMemSurface == FALSE )
					{
						hr = Direct3DCubeTexture9_LockRect_ASync(
							( D_IDirect3DCubeTexture9 * )UseTex,
							( D_D3DCUBEMAP_FACES )i,
							( UINT )j,
							&LockRect,
							NULL,
							0,
							ASyncThread
						) ;
					}
					else
					{
						for(;;)
						{
							UseSysMemSurfaceIndex = Graphics_D3D9_GetSysMemSurface(
								TempTexWidth,
								TempTexHeight,
								GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ],
								ASyncThread
							) ;

							if( UseSysMemSurfaceIndex != -1 )
							{
								break ;
							}

							if( NS_ProcessMessage() != 0 )
							{
								break ;
							}

							Thread_Sleep( 1 ) ;
						}

						if( UseSysMemSurfaceIndex == -1 )
						{
							return -1 ;
						}

						UseSurface = GD3D9.SysMemTexSurf.Surface[ UseSysMemSurfaceIndex ].MemSurface ;

						hr = Direct3DSurface9_LockRect_ASync(
							UseSurface,
							&LockRect,
							NULL,
							D_D3DLOCK_DISCARD,
							ASyncThread
						) ;
					}
					if( hr != D_D3D_OK )
					{
						DXST_LOGFILE_ADDUTF16LE( "\xad\x30\xe5\x30\xfc\x30\xd6\x30\xde\x30\xc3\x30\xd7\x30\xdf\x30\xc3\x30\xd7\x30\xde\x30\xc3\x30\xd7\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\xed\x30\xc3\x30\xaf\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"キューブマップミップマップテクスチャのロックに失敗しました\n" @*/ ) ;
						return -1 ;
					}

					// テクスチャに転送
					_MEMCPY( LockRect.pBits, ImageBuffer, ( size_t )ImageSize ) ;

					// ロック解除
					if( UseSysMemSurface == FALSE )
					{
						Direct3DCubeTexture9_UnlockRect_ASync(
							( D_IDirect3DCubeTexture9 * )UseTex,
							( D_D3DCUBEMAP_FACES )i,
							( UINT )j,
							ASyncThread
						) ;
					}
					else
					{
						Direct3DSurface9_UnlockRect_ASync( UseSurface, ASyncThread ) ;

						Direct3DCubeTexture9_GetCubeMapSurface_ASync(
							( D_IDirect3DCubeTexture9 * )UseTex,
							( D_D3DCUBEMAP_FACES )i,
							( UINT )j,
							&DestSurface,
							ASyncThread
						) ;

						Direct3DDevice9_UpdateSurface_ASync(
							UseSurface,
							NULL,
							DestSurface,
							NULL,
							ASyncThread
						) ; 

						Direct3D9_ObjectRelease_ASync( DestSurface, ASyncThread ) ;

						Graphics_D3D9_ReleaseSysMemSurface( UseSysMemSurfaceIndex, ASyncThread ) ;
						UseSysMemSurfaceIndex = -1 ;
					}

					// 転送元アドレスを進める
					ImageBuffer  = ( BYTE * )ImageBuffer + ImageSize ;
					ImageSize   /= 4 ;
					if( ImageSize < 8 )
					{
						ImageSize = 8 ;
					}

					TempTexWidth  >>= 1 ;
					TempTexHeight >>= 1 ;
					if( TempTexWidth < 1 )
					{
						TempTexWidth = 1 ;
					}
					if( TempTexHeight < 1 )
					{
						TempTexHeight = 1 ;
					}
				}
			}
		}
		else
		{
			TempTexWidth  = TexWidth ;
			TempTexHeight = TexHeight ;
			ImageBuffer   = RgbBaseImage->GraphData ;
			for( j = 0 ; j < Orig->Hard.MipMapCount ; j ++ )
			{
				if( IsDXTFormat )
				{
					ImageSize = ( ( TempTexWidth + 3 ) / 4 ) * ( ( TempTexHeight + 3 ) / 4 ) * ( RgbBaseImage->ColorData.ColorBitDepth * 2 ) ;
				}
				else
				{
					ImageSize = TempTexWidth * TempTexHeight * RgbBaseImage->ColorData.PixelByte ;
				}

				// テクスチャをロック
				if( UseSysMemSurface == FALSE )
				{
					hr = Direct3DTexture9_LockRect_ASync( ( D_IDirect3DTexture9 * )UseTex, ( UINT )j, &LockRect, NULL, 0, ASyncThread ) ;
				}
				else
				{
					for(;;)
					{
						UseSysMemSurfaceIndex = Graphics_D3D9_GetSysMemSurface(
							TempTexWidth,
							TempTexHeight,
							GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ],
							ASyncThread
						) ;

						if( UseSysMemSurfaceIndex != -1 )
						{
							break ;
						}

						if( NS_ProcessMessage() != 0 )
						{
							break ;
						}

						Thread_Sleep( 1 ) ;
					}

					if( UseSysMemSurfaceIndex == -1 )
					{
						return -1 ;
					}

					UseSurface = GD3D9.SysMemTexSurf.Surface[ UseSysMemSurfaceIndex ].MemSurface ;

					hr = Direct3DSurface9_LockRect_ASync(
						UseSurface,
						&LockRect,
						NULL,
						D_D3DLOCK_DISCARD,
						ASyncThread
					) ;
				}
				if( hr != D_D3D_OK )
				{
					DXST_LOGFILE_ADDUTF16LE( "\xdf\x30\xc3\x30\xd7\x30\xde\x30\xc3\x30\xd7\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\xed\x30\xc3\x30\xaf\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ミップマップテクスチャのロックに失敗しました\n" @*/ ) ;
					return -1 ;
				}

				// テクスチャに転送
				_MEMCPY( LockRect.pBits, ImageBuffer, ( size_t )ImageSize ) ;

				// ロック解除
				if( UseSysMemSurface == FALSE )
				{
					Direct3DTexture9_UnlockRect_ASync( ( D_IDirect3DTexture9 * )UseTex, ( UINT )j, ASyncThread ) ;
				}
				else
				{
					Direct3DSurface9_UnlockRect_ASync( UseSurface, ASyncThread ) ;

					Direct3DTexture9_GetSurfaceLevel_ASync(
						( D_IDirect3DTexture9 * )UseTex,
						( UINT )j,
						&DestSurface,
						ASyncThread
					) ;

					Direct3DDevice9_UpdateSurface_ASync(
						UseSurface,
						NULL,
						DestSurface,
						NULL,
						ASyncThread
					) ; 

					Direct3D9_ObjectRelease_ASync( DestSurface, ASyncThread ) ;

					Graphics_D3D9_ReleaseSysMemSurface( UseSysMemSurfaceIndex, ASyncThread ) ;
					UseSysMemSurfaceIndex = -1 ;
				}

				// 転送元アドレスを進める
				ImageBuffer  = ( BYTE * )ImageBuffer + ImageSize ;
				ImageSize   /= 4 ;
				if( ImageSize < 8 )
				{
					ImageSize = 8 ;
				}

				TempTexWidth  >>= 1 ;
				TempTexHeight >>= 1 ;
				if( TempTexWidth < 1 )
				{
					TempTexWidth = 1 ;
				}
				if( TempTexHeight < 1 )
				{
					TempTexHeight = 1 ;
				}
			}
		}
	}
	else
	{
		// 転送先が標準フォーマットではない場合は転送できない
		if( GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ] != D_D3DFMT_DXT1 &&
			GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ] != D_D3DFMT_DXT2 &&
			GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ] != D_D3DFMT_DXT3 &&
			GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ] != D_D3DFMT_DXT4 &&
			GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ] != D_D3DFMT_DXT5 )
		{
			// 転送元にミップマップ情報がある場合と無い場合で処理を分岐
			if( RgbBaseImage->MipMapCount >= Orig->Hard.MipMapCount )
			{
				// ある場合

				ImageBuffer = RgbBaseImage->GraphData ;
				if( Orig->FormatDesc.CubeMapTextureFlag )
				{
					for( i = 0 ; i < CUBEMAP_SURFACE_NUM ; i ++ )
					{
						TempTexWidth = TexWidth ;
						TempTexHeight = TexHeight ;
//						ImageSize = RgbBaseImage->Width * RgbBaseImage->Height * RgbBaseImage->ColorData.PixelByte ;
						for( j = 0 ; j < Orig->Hard.MipMapCount ; j ++ )
						{
							ImageSize = TempTexWidth * TempTexHeight * RgbBaseImage->ColorData.PixelByte ;

							// テクスチャをロック
							if( UseSysMemSurface == FALSE )
							{
								hr = Direct3DCubeTexture9_LockRect_ASync(
									( D_IDirect3DCubeTexture9 * )UseTex,
									( D_D3DCUBEMAP_FACES )i,
									( UINT )j,
									&LockRect,
									NULL,
									0,
									ASyncThread
								) ;
							}
							else
							{
								for(;;)
								{
									UseSysMemSurfaceIndex = Graphics_D3D9_GetSysMemSurface(
										TempTexWidth,
										TempTexHeight,
										GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ],
										ASyncThread
									) ;

									if( UseSysMemSurfaceIndex != -1 )
									{
										break ;
									}

									if( NS_ProcessMessage() != 0 )
									{
										break ;
									}

									Thread_Sleep( 1 ) ;
								}
								if( UseSysMemSurfaceIndex == -1 )
								{
									return -1 ;
								}
								UseSurface = GD3D9.SysMemTexSurf.Surface[ UseSysMemSurfaceIndex ].MemSurface ;

								hr = Direct3DSurface9_LockRect_ASync( UseSurface, &LockRect, NULL, D_D3DLOCK_DISCARD, ASyncThread ) ;
							}
							if( hr != D_D3D_OK )
							{
								DXST_LOGFILE_ADDUTF16LE( "\xdf\x30\xc3\x30\xd7\x30\xde\x30\xc3\x30\xd7\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\xed\x30\xc3\x30\xaf\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ミップマップテクスチャのロックに失敗しました\n" @*/ ) ;
								return -1 ;
							}

							// テクスチャに転送
							_MEMCPY( LockRect.pBits, ImageBuffer, ( size_t )ImageSize ) ;

							// ロック解除
							if( UseSysMemSurface == FALSE )
							{
								Direct3DCubeTexture9_UnlockRect_ASync( ( D_IDirect3DCubeTexture9 * )UseTex, ( D_D3DCUBEMAP_FACES )i, ( UINT )j, ASyncThread ) ;
							}
							else
							{
								Direct3DSurface9_UnlockRect_ASync( UseSurface, ASyncThread ) ;

								Direct3DCubeTexture9_GetCubeMapSurface_ASync( ( D_IDirect3DCubeTexture9 * )UseTex, ( D_D3DCUBEMAP_FACES )i, ( UINT )j, &DestSurface, ASyncThread ) ;

								Direct3DDevice9_UpdateSurface_ASync( UseSurface, NULL, DestSurface, NULL, ASyncThread ) ; 

								Direct3D9_ObjectRelease_ASync( DestSurface, ASyncThread ) ;

								Graphics_D3D9_ReleaseSysMemSurface( UseSysMemSurfaceIndex, ASyncThread ) ;
								UseSysMemSurfaceIndex = -1 ;
							}

							// 転送元アドレスを進める
							ImageBuffer = ( BYTE * )ImageBuffer + ImageSize ;
							ImageSize /= 4 ;

							TempTexWidth >>= 1 ;
							TempTexHeight >>= 1 ;
							if( TempTexWidth < 1 )
							{
								TempTexWidth = 1 ;
							}
							if( TempTexHeight < 1 )
							{
								TempTexHeight = 1 ;
							}
						}
					}
				}
				else
				{
					TempTexWidth = TexWidth ;
					TempTexHeight = TexHeight ;
//					ImageSize = RgbBaseImage->Width * RgbBaseImage->Height * RgbBaseImage->ColorData.PixelByte ;
					for( j = 0 ; j < Orig->Hard.MipMapCount ; j ++ )
					{
						ImageSize = TempTexWidth * TempTexHeight * RgbBaseImage->ColorData.PixelByte ;

						// テクスチャをロック
						if( UseSysMemSurface == FALSE )
						{
							hr = Direct3DTexture9_LockRect_ASync( ( D_IDirect3DTexture9 * )UseTex, ( UINT )j, &LockRect, NULL, 0, ASyncThread ) ;
						}
						else
						{
							for(;;)
							{
								UseSysMemSurfaceIndex = Graphics_D3D9_GetSysMemSurface( TempTexWidth, TempTexHeight, GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ], ASyncThread ) ;
								if( UseSysMemSurfaceIndex != -1 ) break ;
								if( NS_ProcessMessage() != 0 ) break ;
								Thread_Sleep( 1 ) ;
							}
							if( UseSysMemSurfaceIndex == -1 )
							{
								return -1 ;
							}
							UseSurface = GD3D9.SysMemTexSurf.Surface[ UseSysMemSurfaceIndex ].MemSurface ;

							hr = Direct3DSurface9_LockRect_ASync( UseSurface, &LockRect, NULL, D_D3DLOCK_DISCARD, ASyncThread ) ;
						}
						if( hr != D_D3D_OK )
						{
							DXST_LOGFILE_ADDUTF16LE( "\xdf\x30\xc3\x30\xd7\x30\xde\x30\xc3\x30\xd7\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\xed\x30\xc3\x30\xaf\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ミップマップテクスチャのロックに失敗しました\n" @*/ ) ;
							return -1 ;
						}

						// テクスチャに転送
						_MEMCPY( LockRect.pBits, ImageBuffer, ( size_t )ImageSize ) ;

						// ロック解除
						if( UseSysMemSurface == FALSE )
						{
							Direct3DTexture9_UnlockRect_ASync( ( D_IDirect3DTexture9 * )UseTex, ( UINT )j, ASyncThread ) ;
						}
						else
						{
							Direct3DSurface9_UnlockRect_ASync( UseSurface, ASyncThread ) ;

							Direct3DTexture9_GetSurfaceLevel_ASync( ( D_IDirect3DTexture9 * )UseTex, ( UINT )j, &DestSurface, ASyncThread ) ;

							Direct3DDevice9_UpdateSurface_ASync( UseSurface, NULL, DestSurface, NULL, ASyncThread ) ; 

							Direct3D9_ObjectRelease_ASync( DestSurface, ASyncThread ) ;

							Graphics_D3D9_ReleaseSysMemSurface( UseSysMemSurfaceIndex, ASyncThread ) ;
							UseSysMemSurfaceIndex = -1 ;
						}

						// 転送元アドレスを進める
						ImageBuffer = ( BYTE * )ImageBuffer + ImageSize ;
						ImageSize /= 4 ;

						TempTexWidth >>= 1 ;
						TempTexHeight >>= 1 ;
						if( TempTexWidth < 1 )
						{
							TempTexWidth = 1 ;
						}
						if( TempTexHeight < 1 )
						{
							TempTexHeight = 1 ;
						}
					}
				}
			}
			else
			{
				// 無い場合

				// 一時バッファに格納
				ts = DestRect->right  - DestRect->left ;
				for( ImageW = 1 ; ImageW < ts ; ImageW <<= 1 ){}
				ts = DestRect->bottom - DestRect->top  ;
				for( ImageH = 1 ; ImageH < ts ; ImageH <<= 1 ){}
				ImagePitch  = DestColor->PixelByte * ImageW ;
				ImageSize   = ImagePitch * ImageH ;
				ImageBuffer = DXALLOC( ( size_t )( ImageSize * 2 ) ) ;
				Image1      = ImageBuffer ;
				Image2      = ( BYTE * )Image1 + ImageSize ;
				if( ImageBuffer == NULL )
				{
					DXST_LOGFILE_ADDUTF16LE( "\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x78\x30\xe2\x8e\x01\x90\x59\x30\x8b\x30\x3b\x75\xcf\x50\x92\x30\x00\x4e\x42\x66\x84\x76\x6b\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"テクスチャへ転送する画像を一時的に格納するメモリ領域の確保に失敗しました\n" @*/ ) ;
					return -1 ;
				}

				ImageSize      = RgbBaseImage->Pitch   * RgbBaseImage->Height ;
				AlphaImageSize = AlphaBaseImage->Pitch * AlphaBaseImage->Height ;
				for( i = 0 ; i < ImageNum ; i ++ )
				{
					// ミップマップの深さだけ繰り返し
					ImageSW = ImageW ;
					ImageSH = ImageH ;
					ImageDW = ImageW ;
					ImageDH = ImageH ;
					TempTexWidth = TexWidth ;
					TempTexHeight = TexHeight ;
					for( j = 0 ; j < Orig->Hard.MipMapCount ; j ++ )
					{
						// 縮小画像の作成
						if( j == 0 )
						{
							// level0 の時は等倍の出力フォーマット画像を作成する
							NS_GraphColorMatchBltVer2(
											   Image1,        ImagePitch,              DestColor,
								( BYTE * )RgbBaseImage->GraphData   + ImageSize * i,      RgbBaseImage->Pitch,   &RgbBaseImage->ColorData,
								( BYTE * )AlphaBaseImage->GraphData + AlphaImageSize * i, AlphaBaseImage->Pitch, &AlphaBaseImage->ColorData,
								DestPoint, SrcRect, FALSE,
								UseTransColorConvAlpha && ( DestColor->AlphaWidth != 0 && AlphaBaseImage->GraphData == NULL ), Orig->TransCode,
								BASEIM.ImageShavedMode, FALSE,
								RedIsAlphaFlag, FALSE,
								FALSE ) ;
							ImageD = Image1 ;
						}
						else
						{
							if( j & 1 )
							{
								ImageS = Image1 ;
								ImageD = Image2 ;
							}
							else
							{
								ImageS = Image2 ;
								ImageD = Image1 ;
							}

							GraphHalfScaleBlt( DestColor, ImageD, ImagePitch, ImageS, ImagePitch, 0, 0, 0, 0, ImageSW, ImageSH ) ;
						}

						// 転送先のロック
						DestRectT.left   = DestRect->left   >> j ;
						DestRectT.right  = DestRect->right  >> j ;
						DestRectT.top    = DestRect->top    >> j ;
						DestRectT.bottom = DestRect->bottom >> j ;
						if( DestRectT.right  == DestRectT.left ) DestRectT.right  = DestRectT.left + 1 ;
						if( DestRectT.bottom == DestRectT.top  ) DestRectT.bottom = DestRectT.top  + 1 ;
						if( UseSysMemSurface == FALSE )
						{
							if( Orig->FormatDesc.CubeMapTextureFlag )
							{
								if( Direct3DCubeTexture9_LockRect_ASync( ( D_IDirect3DCubeTexture9 * )UseTex, ( D_D3DCUBEMAP_FACES )i, ( UINT )j, &LockRect, &DestRectT, 0, ASyncThread ) != D_D3D_OK )
								{
									DXFREE( ImageBuffer ) ;
									ImageBuffer = NULL ;
									DXST_LOGFILE_ADDUTF16LE( "\xad\x30\xe5\x30\xfc\x30\xd6\x30\xde\x30\xc3\x30\xd7\x30\xdf\x30\xc3\x30\xd7\x30\xde\x30\xc3\x30\xd7\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\xed\x30\xc3\x30\xaf\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"キューブマップミップマップテクスチャのロックに失敗しました\n" @*/ ) ;
									return -1 ;
								}
							}
							else
							{
								if( Direct3DTexture9_LockRect_ASync( ( D_IDirect3DTexture9 * )UseTex, ( UINT )j, &LockRect, &DestRectT, 0, ASyncThread ) != D_D3D_OK )
								{
									DXFREE( ImageBuffer ) ;
									ImageBuffer = NULL ;
									DXST_LOGFILE_ADDUTF16LE( "\xdf\x30\xc3\x30\xd7\x30\xde\x30\xc3\x30\xd7\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\xed\x30\xc3\x30\xaf\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ミップマップテクスチャのロックに失敗しました\n" @*/ ) ;
									return -1 ;
								}
							}
						}
						else
						{
							for(;;)
							{
								UseSysMemSurfaceIndex = Graphics_D3D9_GetSysMemSurface( TempTexWidth, TempTexHeight, GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ], ASyncThread ) ;
								if( UseSysMemSurfaceIndex != -1 ) break ;
								if( NS_ProcessMessage() != 0 ) break ;
								Thread_Sleep( 1 ) ;
							}
							if( UseSysMemSurfaceIndex == -1 )
							{
								DXFREE( ImageBuffer ) ;
								ImageBuffer = NULL ;
								DXST_LOGFILE_ADDUTF16LE( "\xdf\x30\xc3\x30\xd7\x30\xde\x30\xc3\x30\xd7\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x28\x75\x6e\x30\xe1\x30\xe2\x30\xea\x30\xb5\x30\xfc\x30\xd5\x30\xa7\x30\xb9\x30\x6e\x30\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ミップマップテクスチャ用のメモリサーフェスの取得に失敗しました\n" @*/ ) ;
								return -1 ;
							}
							UseSurface = GD3D9.SysMemTexSurf.Surface[ UseSysMemSurfaceIndex ].MemSurface ;

							if( Direct3DSurface9_LockRect_ASync( UseSurface, &LockRect, &DestRectT, D_D3DLOCK_DISCARD, ASyncThread ) != D_D3D_OK )
							{
								DXFREE( ImageBuffer ) ;
								ImageBuffer = NULL ;
								DXST_LOGFILE_ADDUTF16LE( "\xdf\x30\xc3\x30\xd7\x30\xde\x30\xc3\x30\xd7\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\xed\x30\xc3\x30\xaf\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ミップマップテクスチャのロックに失敗しました\n" @*/ ) ;
								return -1 ;
							}
						}

						// テクスチャに転送
						SrcRectT.left   = 0 ;
						SrcRectT.top    = 0 ;
						SrcRectT.right  = ImageDW ;
						SrcRectT.bottom = ImageDH ;
						NS_GraphColorMatchBltVer2(
							LockRect.pBits, LockRect.Pitch,  DestColor,
									ImageD,     ImagePitch,  DestColor,
									  NULL,              0,       NULL,
							DestPoint, &SrcRectT, FALSE,
							FALSE, 0, 0, FALSE, FALSE, FALSE, FALSE ) ;

						// ロック解除
						if( UseSysMemSurface == FALSE )
						{
							if( Orig->FormatDesc.CubeMapTextureFlag )
							{
								Direct3DCubeTexture9_UnlockRect_ASync( ( D_IDirect3DCubeTexture9 * )UseTex, ( D_D3DCUBEMAP_FACES )i, ( UINT )j, ASyncThread ) ;
							}
							else
							{
								Direct3DTexture9_UnlockRect_ASync( ( D_IDirect3DTexture9 * )UseTex, ( UINT )j, ASyncThread ) ;
							}
						}
						else
						{
							D_IDirect3DSurface9 *DestSurfaceP = NULL ;
							POINT DestSurfacePoint ;

							Direct3DSurface9_UnlockRect_ASync( UseSurface, ASyncThread ) ;

							if( Orig->FormatDesc.CubeMapTextureFlag )
							{
								Direct3DCubeTexture9_GetCubeMapSurface_ASync( ( D_IDirect3DCubeTexture9 * )UseTex, ( D_D3DCUBEMAP_FACES )i, ( UINT )j, &DestSurfaceP, ASyncThread ) ;
							}
							else
							{
								Direct3DTexture9_GetSurfaceLevel_ASync( ( D_IDirect3DTexture9 * )UseTex, ( UINT )j, &DestSurfaceP, ASyncThread ) ;
							}

							DestSurfacePoint.x = DestRectT.left ;
							DestSurfacePoint.y = DestRectT.top ;
							Direct3DDevice9_UpdateSurface_ASync( UseSurface, &DestRectT, DestSurfaceP, &DestSurfacePoint, ASyncThread ) ; 

							Direct3D9_ObjectRelease_ASync( DestSurfaceP, ASyncThread ) ;

							Graphics_D3D9_ReleaseSysMemSurface( UseSysMemSurfaceIndex, ASyncThread ) ;
							UseSysMemSurfaceIndex = -1 ;
						}

						// 次のイメージのサイズをセット
						ImageSW = ImageDW ;
						ImageSH = ImageDH ;
						ImageDW >>= 1 ;
						ImageDH >>= 1 ;
						if( ImageDW == 0 ) ImageDW = 1 ;
						if( ImageDH == 0 ) ImageDH = 1 ;

						TempTexWidth >>= 1 ;
						TempTexHeight >>= 1 ;
						if( TempTexWidth < 1 )
						{
							TempTexWidth = 1 ;
						}
						if( TempTexHeight < 1 )
						{
							TempTexHeight = 1 ;
						}
					}
				}

				// メモリの解放
				DXFREE( ImageBuffer ) ;
			}
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x44\x00\x58\x00\x54\x00\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x6e\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6b\x30\x19\x6a\x96\x6e\x62\x5f\x0f\x5f\x6e\x30\xa4\x30\xe1\x30\xfc\x30\xb8\x30\x92\x30\xe2\x8e\x01\x90\x59\x30\x8b\x30\x53\x30\x68\x30\x6f\x30\x67\x30\x4d\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"DXTフォーマットのテクスチャに標準形式のイメージを転送することはできません\n" @*/ ) ;
		}
	}

	// 正常終了
	return 0 ;
}

static int Graphics_D3D9_BltBmpOrBaseImageToGraph3_NoMipMapBlt(
	      IMAGEDATA_ORIG			*Orig,
	const RECT						*SrcRect,
	const RECT						*DestRect,
	      int						UseSysMemSurface,
	      D_IDirect3DBaseTexture9	*UseTex,
	      int						TexWidth,
	      int						TexHeight,
	      int						TexUseWidth,
	      int						TexUseHeight,
	const BASEIMAGE					*RgbBaseImage,
	const BASEIMAGE					*AlphaBaseImage,
	const COLORDATA					*DestColor,
	      int						RedIsAlphaFlag,
	      int						UseTransColorConvAlpha,
	      int						ASyncThread
)
{
	int					i ;
	int					k ;
	int					hr ;
	POINT				DestPoint ;
	RECT				DestRectT ;
	int					H ;
	BYTE				*DestAddr ;
	BYTE				*SrcRgbAddr ;
	BYTE				*SrcAlphaAddr ;
	D_D3DLOCKED_RECT	LockRect ;
	int					RightLineFillFlag ;
	int					BottomLineFillFlag ;
	int					ImageNum ;
	DWORD				ImageSize ;
	DWORD				AlphaImageSize ;
	int					UseSysMemSurfaceIndex ;
	D_IDirect3DSurface9	*UseSurface = NULL ;

	DestPoint.x = 0 ;
	DestPoint.y = 0 ;

	RightLineFillFlag  = FALSE ;
	BottomLineFillFlag = FALSE ;
	DestRectT          = *DestRect ;
	if( TexUseWidth  < TexWidth  && TexUseWidth  == DestRectT.right )
	{
		RightLineFillFlag = TRUE ;
		DestRectT.right ++ ;
	}
	if( TexUseHeight < TexHeight && TexUseHeight == DestRectT.bottom )
	{
		BottomLineFillFlag = TRUE ;
		DestRectT.bottom ++ ;
	}

	ImageNum = Orig->FormatDesc.CubeMapTextureFlag ? CUBEMAP_SURFACE_NUM : 1 ;

//	DXST_LOGFILEFMT_ADDUTF16LE(( L"SrcRect l:%d t:%d r:%d b:%d  DestRect l:%d t:%d r:%d b:%d",
//	SrcRect->left, SrcRect->top, SrcRect->right, SrcRect->bottom,
//	DestRectT.left, DestRectT.top, DestRectT.right, DestRectT.bottom )) ;

	for( i = 0 ; i < ImageNum ; i ++ )
	{
		// 転送先のロック
		if( UseSysMemSurface == FALSE )
		{
			if( Orig->FormatDesc.CubeMapTextureFlag )
			{
				hr = Direct3DCubeTexture9_LockRect_ASync( ( D_IDirect3DCubeTexture9 * )UseTex, ( D_D3DCUBEMAP_FACES )i, 0, &LockRect, &DestRectT, 0, ASyncThread ) ;
			}
			else
			{
				hr = Direct3DTexture9_LockRect_ASync( ( D_IDirect3DTexture9 * )UseTex, 0, &LockRect, &DestRectT, 0, ASyncThread ) ;
			}
			UseSysMemSurfaceIndex = -1 ;
		}
		else
		{
			for(;;)
			{
				UseSysMemSurfaceIndex = Graphics_D3D9_GetSysMemSurface( TexWidth, TexHeight, GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ], ASyncThread ) ;
				if( UseSysMemSurfaceIndex != -1 ) break ;
				if( NS_ProcessMessage() != 0 ) break ;
				Thread_Sleep( 1 ) ;
			}
			if( UseSysMemSurfaceIndex == -1 )
			{
				return -1 ;
			}
			UseSurface = GD3D9.SysMemTexSurf.Surface[ UseSysMemSurfaceIndex ].MemSurface ;

			hr = Direct3DSurface9_LockRect_ASync( UseSurface, &LockRect, &DestRectT, D_D3DLOCK_DISCARD, ASyncThread ) ;
		}
		if( hr != D_D3D_OK )
		{
			DXST_LOGFILE_ADDUTF16LE( "\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\xed\x30\xc3\x30\xaf\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"テクスチャのロックに失敗しました\n" @*/ ) ;
			return -1  ;
		}

		// 転送処理

		// 転送先がテクスチャ全体で、且つDXTフォーマットで転送先もDXTフォーマットの場合は単純なメモリ転送を行う
		if( DestRect->left == 0 && DestRect->right  == TexWidth  &&
			DestRect->top  == 0 && DestRect->bottom == TexHeight &&
			( ( RgbBaseImage->ColorData.Format == DX_BASEIMAGE_FORMAT_DXT1 && GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ] == D_D3DFMT_DXT1 ) ||
			  ( RgbBaseImage->ColorData.Format == DX_BASEIMAGE_FORMAT_DXT2 && GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ] == D_D3DFMT_DXT2 ) ||
			  ( RgbBaseImage->ColorData.Format == DX_BASEIMAGE_FORMAT_DXT3 && GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ] == D_D3DFMT_DXT3 ) ||
			  ( RgbBaseImage->ColorData.Format == DX_BASEIMAGE_FORMAT_DXT4 && GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ] == D_D3DFMT_DXT4 ) ||
			  ( RgbBaseImage->ColorData.Format == DX_BASEIMAGE_FORMAT_DXT5 && GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ] == D_D3DFMT_DXT5 ) ) )
		{
			ImageSize = ( DWORD )( RgbBaseImage->Width * RgbBaseImage->Height * RgbBaseImage->ColorData.ColorBitDepth / 8 ) ;
			_MEMCPY( LockRect.pBits, ( BYTE * )RgbBaseImage->GraphData + ImageSize * i, ImageSize ) ;
		}
		else
		{
			// 転送先が標準フォーマットではない場合は転送できない
			if( GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ] != D_D3DFMT_DXT1 &&
				GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ] != D_D3DFMT_DXT2 &&
				GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ] != D_D3DFMT_DXT3 &&
				GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ] != D_D3DFMT_DXT4 &&
				GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ] != D_D3DFMT_DXT5 )
			{
				ImageSize      = ( DWORD )( RgbBaseImage->Pitch   * RgbBaseImage->Height   ) ;
				AlphaImageSize = ( DWORD )( AlphaBaseImage->Pitch * AlphaBaseImage->Height ) ;

				SrcRgbAddr   = ( BYTE * )RgbBaseImage->GraphData   + i * ImageSize ;
				SrcAlphaAddr = ( BYTE * )AlphaBaseImage->GraphData + i * AlphaImageSize ;
				NS_GraphColorMatchBltVer2(
					LockRect.pBits, LockRect.Pitch,    DestColor,
					SrcRgbAddr,     RgbBaseImage->Pitch,   &RgbBaseImage->ColorData,
					SrcAlphaAddr,   AlphaBaseImage->Pitch, &AlphaBaseImage->ColorData,
					DestPoint, SrcRect, FALSE,
					UseTransColorConvAlpha && ( DestColor->AlphaWidth != 0 && AlphaBaseImage->GraphData == NULL ), Orig->TransCode,
					BASEIM.ImageShavedMode, FALSE,
					RedIsAlphaFlag, FALSE,
					FALSE ) ;

				if( BottomLineFillFlag )
				{
					_MEMCPY( ( BYTE * )LockRect.pBits + ( TexUseHeight - DestRect->top     ) * LockRect.Pitch,
							 ( BYTE * )LockRect.pBits + ( TexUseHeight - DestRect->top - 1 ) * LockRect.Pitch,
							 ( size_t )( DestColor->PixelByte * ( SrcRect->right - SrcRect->left ) ) ) ;
				}
				if( RightLineFillFlag )
				{
					H = SrcRect->bottom - SrcRect->top ;
					DestAddr = ( BYTE * )LockRect.pBits + ( TexUseWidth - DestRect->left ) * DestColor->PixelByte ;
					switch( DestColor->PixelByte )
					{
					case 2 :
						for( k = 0 ; k < H ; k ++, DestAddr += LockRect.Pitch )
						{
							*( ( WORD * )DestAddr ) = *( ( WORD * )DestAddr - 1 ) ;
						}
						break ;

					case 4 :
						for( k = 0 ; k < H ; k ++, DestAddr += LockRect.Pitch )
						{
							*( ( DWORD * )DestAddr ) = *( ( DWORD * )DestAddr - 1 ) ;
						}
						break ;
					}
				}
				if( BottomLineFillFlag && RightLineFillFlag )
				{
					DestAddr = ( BYTE * )LockRect.pBits + ( TexUseHeight - DestRect->top ) * LockRect.Pitch + ( TexUseWidth - DestRect->left ) * DestColor->PixelByte ;
					switch( DestColor->PixelByte )
					{
					case 2 : *( ( WORD  * )DestAddr ) = *( ( WORD  * )( DestAddr - LockRect.Pitch ) - 1 ) ; break ;
					case 4 : *( ( DWORD * )DestAddr ) = *( ( DWORD * )( DestAddr - LockRect.Pitch ) - 1 ) ;	break ;
					}
				}
			}
			else
			{
				DXST_LOGFILE_ADDUTF16LE( "\x44\x00\x58\x00\x54\x00\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x6e\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6b\x30\x19\x6a\x96\x6e\x62\x5f\x0f\x5f\x6e\x30\xa4\x30\xe1\x30\xfc\x30\xb8\x30\x92\x30\xe2\x8e\x01\x90\x59\x30\x8b\x30\x53\x30\x68\x30\x6f\x30\x67\x30\x4d\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"DXTフォーマットのテクスチャに標準形式のイメージを転送することはできません\n" @*/ ) ;
			}
		}

		// ロック解除
		if( UseSysMemSurface == FALSE )
		{
			if( Orig->FormatDesc.CubeMapTextureFlag )
			{
				Direct3DCubeTexture9_UnlockRect_ASync( ( D_IDirect3DCubeTexture9 * )UseTex, ( D_D3DCUBEMAP_FACES )i, 0, ASyncThread ) ;
			}
			else
			{
				Direct3DTexture9_UnlockRect_ASync( ( D_IDirect3DTexture9 * )UseTex, 0, ASyncThread ) ;
			}
		}
		else
		{
			D_IDirect3DSurface9 *DestSurface = NULL ;
			POINT                DestSurfacePoint ;

			Direct3DSurface9_UnlockRect_ASync( UseSurface, ASyncThread ) ;

			if( Orig->FormatDesc.CubeMapTextureFlag )
			{
				Direct3DCubeTexture9_GetCubeMapSurface_ASync( ( D_IDirect3DCubeTexture9 * )UseTex, ( D_D3DCUBEMAP_FACES )i, 0, &DestSurface, ASyncThread ) ;
			}
			else
			{
				Direct3DTexture9_GetSurfaceLevel_ASync( ( D_IDirect3DTexture9 * )UseTex, 0, &DestSurface, ASyncThread ) ;
			}

			DestSurfacePoint.x = DestRectT.left ;
			DestSurfacePoint.y = DestRectT.top ;
			Direct3DDevice9_UpdateSurface_ASync( UseSurface, &DestRectT, DestSurface, &DestSurfacePoint, ASyncThread ) ; 

			Direct3D9_ObjectRelease_ASync( DestSurface, ASyncThread ) ;

			Graphics_D3D9_ReleaseSysMemSurface( UseSysMemSurfaceIndex, ASyncThread ) ;
			UseSysMemSurfaceIndex = -1 ;
		}
	}

	// 正常終了
	return 0 ;
}

// Direct3D9 のグラフィックハンドルに画像データを転送するための関数
extern	int		Graphics_Hardware_D3D9_BltBmpOrBaseImageToGraph3_PF(
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
)
{
	IMAGEDATA					*Image ;
	IMAGEDATA_ORIG				*Orig ;
	IMAGEDATA_ORIG_HARD_TEX		*OrigTex ;
	IMAGEDATA_HARD_DRAW			*DrawTex ;
	int							ImageNum ;
	int							UseSysMemTex = -1 ;
	int							i ;
	int							j ;
	POINT						DestPoint ;
	RECT						DestRect ;
	RECT						MoveRect ;
	int							IsDXT ;
	COLORDATA					*DestColor ;
	D_D3DLOCKED_RECT			LockRect ;
	D_IDirect3DBaseTexture9		*UseTex ;
	D_IDirect3DSurface9			*SMSurface[ CUBEMAP_SURFACE_NUM ] = { 0 } ;
	D_IDirect3DTexture9			*SMTexture = NULL ;
	int							SrcWidth ;
	int							SrcHeight ;
	HRESULT						hr ;

	if( Direct3DDevice9_IsValid() == 0 )
	{
		return -1 ;
	}

	// 描画待機している描画物を描画
	DRAWSTOCKINFO_ASYNC( ASyncThread ) ;

	// DXT圧縮の画像かを調べておく
	IsDXT =
		( RgbBaseImage->ColorData.Format == DX_BASEIMAGE_FORMAT_DXT1 ||
		  RgbBaseImage->ColorData.Format == DX_BASEIMAGE_FORMAT_DXT2 ||
		  RgbBaseImage->ColorData.Format == DX_BASEIMAGE_FORMAT_DXT3 ||
		  RgbBaseImage->ColorData.Format == DX_BASEIMAGE_FORMAT_DXT4 ||
		  RgbBaseImage->ColorData.Format == DX_BASEIMAGE_FORMAT_DXT5 ) ? TRUE : FALSE ;

	// 転送幅を計算しておく
	SrcWidth  = SrcRect->right  - SrcRect->left ;
	SrcHeight = SrcRect->bottom - SrcRect->top ;
	if( SrcWidth <= 0 || SrcHeight <= 0 )
	{
		return 0 ;
	}

	// エラー判定
	if( ASyncThread )
	{
		if( GRAPHCHK_ASYNC( GrHandle, Image ) )
		{
			return -1 ;
		}
	}
	else
	{
		if( GRAPHCHK( GrHandle, Image ) )
		{
			return -1 ;
		}
	}
	Orig = Image->Orig ;

	// イメージの数をセット
	ImageNum = RgbBaseImage->GraphDataCount == 0 ? 1 : RgbBaseImage->GraphDataCount ;

	// テクスチャのカラー情報を取得する
	DestColor = Graphics_D3D9_GetD3DFormatColorData( GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ] ) ;

	// 転送位置のセット
	DestPoint.x = 0 ;
	DestPoint.y = 0 ;

	// 描画可能画像の場合は転送する画像情報と同じ大きさのシステムメモリサーフェスに内容を転送する
	if( Orig->FormatDesc.DrawValidFlag )
	{
		for( i = 0 ; i < ImageNum ; i ++ )
		{
			if( Direct3DDevice9_CreateOffscreenPlainSurface_ASync(
					( UINT )SrcWidth,
					( UINT )SrcHeight,
					GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ],
					D_D3DPOOL_SYSTEMMEM,
					&SMSurface[ i ],
					NULL,
					ASyncThread
				) != D_D3D_OK )
			{
				break ;
			}

			// システムメモリサーフェスをロック
			Direct3DSurface9_LockRect_ASync( SMSurface[ i ], &LockRect, NULL, D_D3DLOCK_DISCARD, ASyncThread ) ;

			// 内容を転送
			MoveRect.left   = 0 ;
			MoveRect.top    = 0 ;
			MoveRect.right  = SrcWidth ;
			MoveRect.bottom = SrcHeight ;
			NS_GraphColorMatchBltVer2(
				LockRect.pBits,                                                               LockRect.Pitch,    DestColor,
				( BYTE * )RgbBaseImage->GraphData   + RgbBaseImage->Pitch   * RgbBaseImage->Height   * i, RgbBaseImage->Pitch,   &RgbBaseImage->ColorData,
				( BYTE * )AlphaBaseImage->GraphData + AlphaBaseImage->Pitch * AlphaBaseImage->Height * i, AlphaBaseImage->Pitch, &AlphaBaseImage->ColorData,
				DestPoint, &MoveRect, FALSE,
				UseTransColorConvAlpha && ( DestColor->AlphaWidth != 0 && AlphaBaseImage->GraphData == NULL ), Orig->TransCode,
				BASEIM.ImageShavedMode, FALSE,
				RedIsAlphaFlag, FALSE,
				FALSE ) ;

			// システムメモリサーフェスのロックを解除
			Direct3DSurface9_UnlockRect_ASync( SMSurface[ i ], ASyncThread ) ;
		}

		// オフスクリーンサーフェスの作成に失敗したらテクスチャの作成を試みる
		if( ImageNum != i )
		{
			// キューブマップや複数テクスチャには非対応
			if( ImageNum > 1 || Orig->Hard.TexNum > 1 )
			{
				DXST_LOGFILE_ADDUTF16LE( "\x3b\x75\xcf\x50\xe2\x8e\x01\x90\x28\x75\x6e\x30\xb7\x30\xb9\x30\xc6\x30\xe0\x30\xe1\x30\xe2\x30\xea\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"画像転送用のシステムメモリテクスチャの作成に失敗しました\n" @*/ ) ;
				goto ERR ;
			}
			else
			{
				DWORD TexWidth ;
				DWORD TexHeight ;
				D_IDirect3DTexture9 *TargetTexture ;

				if( TargetOrig == TRUE )
				{
					TexWidth  = ( DWORD )Orig->Hard.Tex[ 0 ].TexWidth ;
					TexHeight = ( DWORD )Orig->Hard.Tex[ 0 ].TexHeight ;
					TargetTexture = Orig->Hard.Tex[ 0 ].PF->D3D9.Texture ;
				}
				else
				{
					TexWidth  = ( DWORD )Image->Hard.Draw[ 0 ].Tex->TexWidth ;
					TexHeight = ( DWORD )Image->Hard.Draw[ 0 ].Tex->TexHeight ;
					TargetTexture = Image->Hard.Draw[ 0 ].Tex->PF->D3D9.Texture ;
				}

				if( Direct3DDevice9_CreateTexture_ASync(
						TexWidth,
						TexHeight,
						1,
						0,
						GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ],
						D_D3DPOOL_SYSTEMMEM,
						&SMTexture,
						NULL,
						ASyncThread ) != D_D3D_OK )
				{
					DXST_LOGFILE_ADDUTF16LE( "\x3b\x75\xcf\x50\xe2\x8e\x01\x90\x28\x75\x6e\x30\xb7\x30\xb9\x30\xc6\x30\xe0\x30\xe1\x30\xe2\x30\xea\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"画像転送用のシステムメモリテクスチャの作成に失敗しました\n" @*/ ) ;
					goto ERR ;
				}

				// システムメモリテクスチャをロック
				hr = Direct3DTexture9_LockRect_ASync( SMTexture, 0, &LockRect, NULL, D_D3DLOCK_DISCARD, ASyncThread ) ;

				// 内容を転送
				MoveRect.left   = 0 ;
				MoveRect.top    = 0 ;
				MoveRect.right  = SrcWidth ;
				MoveRect.bottom = SrcHeight ;
				NS_GraphColorMatchBltVer2(
					LockRect.pBits,                                                                                  LockRect.Pitch,                  DestColor,
					( BYTE * )RgbBaseImage->GraphData   + RgbBaseImage->Pitch   * RgbBaseImage->Height   * i,   RgbBaseImage->Pitch,   &RgbBaseImage->ColorData,
					( BYTE * )AlphaBaseImage->GraphData + AlphaBaseImage->Pitch * AlphaBaseImage->Height * i, AlphaBaseImage->Pitch, &AlphaBaseImage->ColorData,
					DestPoint, &MoveRect, FALSE,
					UseTransColorConvAlpha && ( DestColor->AlphaWidth != 0 && AlphaBaseImage->GraphData == NULL ), Orig->TransCode,
					BASEIM.ImageShavedMode, FALSE,
					RedIsAlphaFlag, FALSE,
					FALSE ) ;

				// システムメモリテクスチャのロックを解除
				hr = Direct3DTexture9_UnlockRect_ASync( SMTexture, 0, ASyncThread ) ;

				// テクスチャの更新
				hr = Direct3DDevice9_UpdateTexture_ASync( SMTexture, TargetTexture, ASyncThread ) ;
				goto END ;
			}
		}
	}

	// 転送対象がオリジナル画像に対してかどうかで処理を分岐
	if( TargetOrig == TRUE )
	{
		// オリジナル画像に対しての場合

		// テクスチャの数だけ繰り返し
		OrigTex = Orig->Hard.Tex ;
		for( i = 0 ; i < Orig->Hard.TexNum ; i ++, OrigTex ++ )
		{
			// 転送矩形の作成
			if( Graphics_Image_BltBmpOrBaseImageToGraph3_Make_OrigTex_MoveRect(
					OrigTex,
					SrcRect,
					SrcWidth,
					SrcHeight,
					DestX,
					DestY,
					&DestRect,
					&MoveRect,
					IsDXT ) < 0 )
			{
				continue ;
			}

			// 描画可能画像かどうかで処理を分岐
			if( Orig->FormatDesc.DrawValidFlag == TRUE )
			{
				// 描画可能画像の場合

				// 内容を転送

				DestPoint.x = DestRect.left ;
				DestPoint.y = DestRect.top ;
				for( j = 0 ; j < ImageNum ; j ++ )
				{
					Direct3DDevice9_UpdateSurface_ASync(
						SMSurface[ j ],				&MoveRect,
						OrigTex->PF->D3D9.Surface[ j ],	&DestPoint, ASyncThread ) ;
				}
				if( OrigTex->PF->D3D9.RenderTargetSurface )
				{
					Direct3DDevice9_UpdateSurface_ASync(
						SMSurface[ 0 ],						&MoveRect,
						OrigTex->PF->D3D9.RenderTargetSurface,	&DestPoint, ASyncThread ) ;
				}
			}
			else
			{
				int FullUpdate ;

				// 描画不可能画像の場合

				// 全体転送かどうかを調べる
				if( DestRect.right  - DestRect.left == OrigTex->TexWidth &&
					DestRect.bottom - DestRect.top  == OrigTex->TexHeight )
				{
					FullUpdate = TRUE ;
				}
				else
				{
					FullUpdate = FALSE ;
				}

				// 使用するテクスチャのセット
				//UseTex = Orig->FormatDesc.UseManagedTextureFlag == FALSE ? OrigTex->MemTexture : OrigTex->PF->D3D9.Texture ;
				if( FullUpdate && Orig->FormatDesc.UseManagedTextureFlag == FALSE )
				{
					for(;;)
					{
						UseSysMemTex = Graphics_D3D9_GetSysMemTextureFromOrig( Orig, OrigTex, ASyncThread ) ;
						if( UseSysMemTex != -1 ) break ;
						if( NS_ProcessMessage() != 0 ) break ;
						Thread_Sleep( 1 ) ;
					}
					if( UseSysMemTex == -1 )
					{
						goto ERR ;
					}
					UseTex = GD3D9.SysMemTexSurf.Texture[ UseSysMemTex ].MemTexture ;
				}
				else
				{
					UseSysMemTex = -1 ;
					UseTex = OrigTex->PF->D3D9.Texture ;
				}

				// ミップマップかどうかで処理を分岐
				if( Orig->Hard.MipMapCount > 1 )
				{
					if( Graphics_D3D9_BltBmpOrBaseImageToGraph3_MipMapBlt(
							Orig,
							&MoveRect,
							&DestRect,
							FullUpdate == FALSE && Orig->FormatDesc.UseManagedTextureFlag == FALSE,
							UseTex,
							OrigTex->TexWidth,
							OrigTex->TexHeight,
							RgbBaseImage,
							AlphaBaseImage,
							DestColor,
							RedIsAlphaFlag,
							UseTransColorConvAlpha,
							ASyncThread ) < 0 )
						goto ERR ;
				}
				else
				{
					if( Graphics_D3D9_BltBmpOrBaseImageToGraph3_NoMipMapBlt(
							Orig,
							&MoveRect,
							&DestRect,
							FullUpdate == FALSE && Orig->FormatDesc.UseManagedTextureFlag == FALSE,
							UseTex,
							OrigTex->TexWidth,
							OrigTex->TexHeight,
							OrigTex->UseWidth,
							OrigTex->UseHeight,
							RgbBaseImage,
							AlphaBaseImage,
							DestColor,
							RedIsAlphaFlag,
							UseTransColorConvAlpha,
							ASyncThread ) < 0 )
						goto ERR ;
				}

				// 管理テクスチャではない場合はここで転送用のシステムメモリテクスチャを解放する
				if( FullUpdate && Orig->FormatDesc.UseManagedTextureFlag == FALSE )
				{
					if( Orig->FormatDesc.CubeMapTextureFlag )
					{
						Direct3DDevice9_UpdateTexture_ASync( UseTex, OrigTex->PF->D3D9.CubeTexture, ASyncThread ) ;
					}
					else
					{
						Direct3DDevice9_UpdateTexture_ASync( UseTex, OrigTex->PF->D3D9.Texture, ASyncThread ) ;
					}
					Graphics_D3D9_ReleaseSysMemTexture( UseSysMemTex, ASyncThread ) ;
					UseSysMemTex = -1 ;
				}
			}
		}
	}
	else
	{
		// オリジナル画像に対してではない場合

		// 描画情報の数だけ繰り返し
		DrawTex = Image->Hard.Draw ;
		DestPoint.x = 0 ;
		DestPoint.y = 0 ;
		for( i = 0 ; i < Image->Hard.DrawNum ; i ++, DrawTex ++ )
		{
			// 転送矩形の作成
			if( Graphics_Image_BltBmpOrBaseImageToGraph3_Make_DrawTex_MoveRect(
					DrawTex,
					SrcRect,
					SrcWidth,
					SrcHeight,
					DestX,
					DestY,
					&DestRect,
					&MoveRect,
					IsDXT ) < 0 )
			{
				continue ;
			}

			// 描画可能画像かどうかで処理を分岐
			if( Orig->FormatDesc.DrawValidFlag == TRUE )
			{
				// 描画可能画像の場合

				// 内容を転送
				DestPoint.x = DestRect.left ;
				DestPoint.y = DestRect.top ;
				for( j = 0 ; j < ImageNum ; j ++ )
				{
					if( Direct3DDevice9_UpdateSurface_ASync(
							SMSurface[ j ],             &MoveRect,
							DrawTex->Tex->PF->D3D9.Surface[ j ], &DestPoint, ASyncThread ) != D_D3D_OK )
					{
						DXST_LOGFILE_ADDUTF16LE( "\xcf\x63\x3b\x75\xef\x53\xfd\x80\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x78\x30\x6e\x30\x3b\x75\xcf\x50\x6e\x30\xe2\x8e\x01\x90\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"描画可能テクスチャへの画像の転送に失敗しました\n" @*/ ) ;
						goto ERR ;
					}
				}
			}
			else
			{
				int FullUpdate ;

				// 描画不可能画像の場合

				// 全体転送かどうかを調べる
				if( DestRect.right  - DestRect.left == DrawTex->Tex->TexWidth &&
					DestRect.bottom - DestRect.top  == DrawTex->Tex->TexHeight )
				{
					FullUpdate = TRUE ;
				}
				else
				{
					FullUpdate = FALSE ;
				}

				// 使用するテクスチャのセット
				//UseTex = Orig->FormatDesc.UseManagedTextureFlag == FALSE ? DrawTex->Tex->MemTexture : DrawTex->Tex->PF->D3D9.Texture ;
				if( FullUpdate && Orig->FormatDesc.UseManagedTextureFlag == FALSE )
				{
					for(;;)
					{
						UseSysMemTex = Graphics_D3D9_GetSysMemTextureFromOrig( Orig, DrawTex->Tex, ASyncThread ) ;
						if( UseSysMemTex       != -1 ) break ;
						if( NS_ProcessMessage() != 0 ) break ;
						Thread_Sleep( 1 ) ;
					}
					if( UseSysMemTex == -1 )
					{
						goto ERR ;
					}
					UseTex       = GD3D9.SysMemTexSurf.Texture[ UseSysMemTex ].MemTexture ;
				}
				else
				{
					UseTex       = DrawTex->Tex->PF->D3D9.Texture ;
					UseSysMemTex = -1 ;
				}

				// ミップマップかどうかで処理を分岐
				if( Orig->Hard.MipMapCount > 1 )
				{
					if( Graphics_D3D9_BltBmpOrBaseImageToGraph3_MipMapBlt(
							Orig,
							&MoveRect,
							&DestRect,
							FullUpdate == FALSE && Orig->FormatDesc.UseManagedTextureFlag == FALSE,
							UseTex,
							DrawTex->Tex->TexWidth,
							DrawTex->Tex->TexHeight,
							RgbBaseImage,
							AlphaBaseImage,
							DestColor,
							RedIsAlphaFlag,
							UseTransColorConvAlpha,
							ASyncThread ) < 0 )
						goto ERR ;
				}
				else
				{
					if( Graphics_D3D9_BltBmpOrBaseImageToGraph3_NoMipMapBlt(
							Orig,
							&MoveRect,
							&DestRect,
							FullUpdate == FALSE && Orig->FormatDesc.UseManagedTextureFlag == FALSE,
							UseTex,
							DrawTex->Tex->TexWidth,
							DrawTex->Tex->TexHeight,
							DrawTex->Tex->UseWidth,
							DrawTex->Tex->UseHeight,
							RgbBaseImage,
							AlphaBaseImage,
							DestColor,
							RedIsAlphaFlag,
							UseTransColorConvAlpha,
							ASyncThread ) < 0 )
						goto ERR ;
				}

				// 管理テクスチャではない場合はここで転送用のシステムメモリテクスチャを解放する
				if( FullUpdate && Orig->FormatDesc.UseManagedTextureFlag == FALSE )
				{
					if( Orig->FormatDesc.CubeMapTextureFlag )
					{
						Direct3DDevice9_UpdateTexture_ASync( UseTex, DrawTex->Tex->PF->D3D9.CubeTexture, ASyncThread ) ;
					}
					else
					{
						Direct3DDevice9_UpdateTexture_ASync( UseTex, DrawTex->Tex->PF->D3D9.Texture, ASyncThread ) ;
					}
					Graphics_D3D9_ReleaseSysMemTexture( UseSysMemTex, ASyncThread ) ;
					UseSysMemTex = -1 ;
				}
			}
		}
	}

END :

	for( i = 0 ; i < CUBEMAP_SURFACE_NUM ; i ++ )
	{
		if( SMSurface[ i ] != NULL )
		{
			Direct3D9_ObjectRelease_ASync( SMSurface[ i ], ASyncThread ) ;
			SMSurface[ i ] = NULL ;
		}
	}

	if( SMTexture != NULL )
	{
		Direct3D9_ObjectRelease_ASync( SMTexture, ASyncThread ) ;
		SMTexture = NULL ;
	}

	if( UseSysMemTex != -1 )
	{
		Graphics_D3D9_ReleaseSysMemTexture( UseSysMemTex, ASyncThread ) ;
		UseSysMemTex = -1 ;
	}

	// 正常終了
	return 0 ;


	// エラー終了
ERR :

	for( i = 0 ; i < CUBEMAP_SURFACE_NUM ; i ++ )
	{
		if( SMSurface[ i ] != NULL )
		{
			Direct3D9_ObjectRelease_ASync( SMSurface[ i ], ASyncThread ) ;
			SMSurface[ i ] = NULL ;
		}
	}

	if( SMTexture != NULL )
	{
		Direct3D9_ObjectRelease_ASync( SMTexture, ASyncThread ) ;
		SMTexture = NULL ;
	}

	if( UseSysMemTex != -1 )
	{
		Graphics_D3D9_ReleaseSysMemTexture( UseSysMemTex, ASyncThread ) ;
		UseSysMemTex = -1 ;
	}

	return -1 ;
}

// オリジナル画像情報中のテクスチャを作成する( 0:成功  -1:失敗 )
extern	int		Graphics_Hardware_D3D9_CreateOrigTexture_PF(  IMAGEDATA_ORIG *Orig, int ASyncThread )
{
	int i, j ;
	IMAGEDATA_ORIG_HARD_TEX *OrigTex ;

	// Direct3DDevice が無効だったらエラー
	if( Direct3DDevice9_IsValid() == 0 )
		return -1 ;

	// ハードウエアを使用する画像ではない場合は何もせずに終了
	if( Orig->FormatDesc.TextureFlag == FALSE ) return 0 ;

	// 指定のフォーマットが作成できない場合は失敗
	if( GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ] == D_D3DFMT_UNKNOWN )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x07\x63\x9a\x5b\x6e\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x67\x30\x5c\x4f\x10\x62\x67\x30\x4d\x30\x8b\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"指定のフォーマットで作成できるテクスチャフォーマットがありませんでした\n" @*/ ) ;
		return -1 ;
	}

	// テクスチャの場合はテクスチャの数だけテクスチャを作成
	OrigTex = Orig->Hard.Tex ;
	for( i = 0 ; i < Orig->Hard.TexNum ; i ++, OrigTex ++ )
	{
		// 既にテクスチャが作成されていたらスキップ
//		if( OrigTex->PF->D3D9.Texture != NULL ) continue ;

		// Direct3D9Ex を使用している場合は管理属性を使用しない( できない )
		if( Direct3D9_IsExObject() != 0 )
			Orig->FormatDesc.UseManagedTextureFlag = FALSE ;

		// レンダリングターゲットサーフェスの作成
		if( Orig->FormatDesc.DrawValidFlag )
		{
			// キューブマップでは別途レンダーターゲットは作成しない
			if( Orig->FormatDesc.CubeMapTextureFlag == FALSE )
			{
				D_D3DMULTISAMPLE_TYPE Samples = D_D3DMULTISAMPLE_NONE ;
				DWORD Quality = 0 ;

				if( Orig->FormatDesc.MSSamples != 0 )
				{
					Samples = ( D_D3DMULTISAMPLE_TYPE )Orig->FormatDesc.MSSamples ;
					Quality = ( DWORD )Orig->FormatDesc.MSQuality ;
					Direct3D9_CheckMultiSampleParam_ASync( GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ], &Samples, &Quality, FALSE, ASyncThread ) ;
				}

				if( Samples != D_D3DMULTISAMPLE_NONE && OrigTex->PF->D3D9.RenderTargetSurface == NULL )
				{
					// レンダリングターゲットサーフェスの作成
					if( Direct3DDevice9_CreateRenderTarget_ASync(
						( UINT )OrigTex->TexWidth,								// 幅
						( UINT )OrigTex->TexHeight,								// 高さ
						Orig->FormatDesc.PlatformTextureFormat != 0 ? g_DXD3D9FMT_to_D3DFORMAT[ Orig->FormatDesc.PlatformTextureFormat ] : GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ],	// フォーマット
						Samples,												// マルチサンプリングタイプ
						( UINT )Quality,										// マルチサンプリングクオリティ
						FALSE,													// ロックはできない
						&OrigTex->PF->D3D9.RenderTargetSurface,					// D_IDirect3DSurface9 のポインタを受け取るアドレス
						NULL,													// 絶対 NULL
						ASyncThread
						) != D_D3D_OK )
					{
						DXST_LOGFILE_ADDUTF16LE( "\xf8\x66\x4d\x30\xbc\x8f\x7f\x30\x02\x5c\x28\x75\xb5\x30\xfc\x30\xd5\x30\xa7\x30\xb9\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"書き込み専用サーフェスの作成に失敗しました\n" @*/ ) ;
						return -1 ;
					}
				}

				if( ( ( D_D3DMULTISAMPLE_TYPE )GSYS.Setting.FSAAMultiSampleCount == D_D3DMULTISAMPLE_NONE && Samples != D_D3DMULTISAMPLE_NONE ) ||
					( ( D_D3DMULTISAMPLE_TYPE )GSYS.Setting.FSAAMultiSampleCount != D_D3DMULTISAMPLE_NONE && Samples == D_D3DMULTISAMPLE_NONE ) ||
					Orig->ZBufferFlag  )
				{
					if( OrigTex->PF->D3D9.ZBuffer != NULL )
					{
						Direct3D9_ObjectRelease( OrigTex->PF->D3D9.ZBuffer ) ;
						OrigTex->PF->D3D9.ZBuffer = NULL ;
					}

					// レンダリングターゲット深度バッファの作成
					if( Direct3DDevice9_CreateDepthStencilSurface_ASync(
						( UINT )OrigTex->TexWidth,										// 幅
						( UINT )OrigTex->TexHeight,										// 高さ
						GD3D9.Device.Caps.ZBufferFormat[ Orig->ZBufferBitDepthIndex ],	// フォーマット
						Samples,														// マルチサンプリングタイプ
						( UINT )Quality,												// マルチサンプリングクオリティ
						FALSE,															// ロックはできない
						&OrigTex->PF->D3D9.ZBuffer,										// D_IDirect3DSurface9 のポインタを受け取るアドレス
						NULL,															// 絶対 NULL
						ASyncThread
						) != D_D3D_OK )
					{
						DXST_LOGFILE_ADDUTF16LE( "\xf8\x66\x4d\x30\xbc\x8f\x7f\x30\x02\x5c\x28\x75\xde\x30\xeb\x30\xc1\x30\xb5\x30\xf3\x30\xd7\x30\xeb\x30\xf1\x6d\xa6\x5e\xb5\x30\xfc\x30\xd5\x30\xa7\x30\xb9\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"書き込み専用マルチサンプル深度サーフェスの作成に失敗しました\n" @*/ ) ;
						return -1 ;
					}
				}
				else
				if( OrigTex->PF->D3D9.ZBuffer != NULL )
				{
					Direct3D9_ObjectRelease( OrigTex->PF->D3D9.ZBuffer ) ;
					OrigTex->PF->D3D9.ZBuffer = NULL ;
				}
			}
		}

		// ビデオメモリテクスチャの作成
		if( Orig->FormatDesc.CubeMapTextureFlag )
		{
			if( OrigTex->PF->D3D9.CubeTexture == NULL )
			{
				if( Direct3DDevice9_CreateCubeTexture_ASync(
					( UINT )OrigTex->TexWidth,																											// エッジサイズ
					( UINT )Orig->Hard.MipMapCount,																										// MipMap レベル
					( DWORD )( Orig->FormatDesc.DrawValidFlag ? D_D3DUSAGE_RENDERTARGET : 0 ),															// フラグ
					Orig->FormatDesc.PlatformTextureFormat != 0 ? g_DXD3D9FMT_to_D3DFORMAT[ Orig->FormatDesc.PlatformTextureFormat ] : GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ],		// フォーマット
					Orig->FormatDesc.UseManagedTextureFlag == FALSE || Orig->FormatDesc.DrawValidFlag ? D_D3DPOOL_DEFAULT : D_D3DPOOL_MANAGED,			// テクスチャを DirectX が管理するかどうか
					&OrigTex->PF->D3D9.CubeTexture,																										// D_IDirect3DTexture9 のポインタを受け取るアドレス
					NULL,																																// 絶対 NULL
					ASyncThread
					) != D_D3D_OK )
				{
					DXST_LOGFILE_ADDUTF16LE( "\xad\x30\xe5\x30\xfc\x30\xd6\x30\xde\x30\xc3\x30\xd7\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"キューブマップテクスチャの作成に失敗しました\n" @*/ ) ;
					return -1 ;
				}
			}
		}
		else
		{
			if( OrigTex->PF->D3D9.Texture == NULL )
			{
				if( Direct3DDevice9_CreateTexture_ASync(
					( UINT )OrigTex->TexWidth,																											// 幅
					( UINT )OrigTex->TexHeight,																											// 高さ
					( UINT )Orig->Hard.MipMapCount,																										// MipMap レベル
					( DWORD )( Orig->FormatDesc.DrawValidFlag ? D_D3DUSAGE_RENDERTARGET : 0 ),															// フラグ
					Orig->FormatDesc.PlatformTextureFormat != 0 ? g_DXD3D9FMT_to_D3DFORMAT[ Orig->FormatDesc.PlatformTextureFormat ] : GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ],	// フォーマット
					Orig->FormatDesc.UseManagedTextureFlag == FALSE || Orig->FormatDesc.DrawValidFlag ? D_D3DPOOL_DEFAULT : D_D3DPOOL_MANAGED,			// テクスチャを DirectX が管理するかどうか
					&OrigTex->PF->D3D9.Texture,																											// D_IDirect3DTexture9 のポインタを受け取るアドレス
					NULL,																																// 絶対 NULL
					ASyncThread
					) != D_D3D_OK )
				{
					DXST_LOGFILE_ADDUTF16LE( "\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"テクスチャの作成に失敗しました\n" @*/ ) ;
					return -1 ;
				}
			}
		}

		// 描画可能テクスチャではなく、且つ管理テクスチャではない場合はシステムメモリテクスチャを作成
		/*if( ( Orig->FormatDesc.DrawValidFlag == FALSE && Orig->FormatDesc.UseManagedTextureFlag == FALSE ) && OrigTex->MemTexture == NULL )
		{
			if( Direct3DDevice9_CreateTexture(
				OrigTex->TexWidth,							// 幅
				OrigTex->TexHeight,							// 高さ
				Orig->Hard.MipMapCount,						// MipMap レベル
				D_D3DUSAGE_DYNAMIC,							// フラグ
				GD3D9.Device.Caps.TextureFormat[ Orig->ColorFormat ],		// フォーマット
				D_D3DPOOL_SYSTEMMEM,						// テクスチャを DirectX が管理するかどうか
				&OrigTex->MemTexture,						// D_IDirect3DTexture9 のポインタを受け取るアドレス
				NULL										// 絶対 NULL
				) != D_D3D_OK )
			{
				Direct3D9_ObjectRelease( OrigTex->PF->D3D9.Texture ) ;
				OrigTex->PF->D3D9.Texture = NULL ;

				DXST_LOGFILE_ADDUTF16LE( L"メモリテクスチャの作成に失敗しました\n" ) ;
				return -1 ;
			}
		}*/

		// サーフェスのアドレスも取得する
		if( Orig->FormatDesc.CubeMapTextureFlag )
		{
			for( j = 0 ; j < CUBEMAP_SURFACE_NUM ; j ++ )
			{
				if( OrigTex->PF->D3D9.Surface[ j ] == NULL )
				{
					if( Direct3DCubeTexture9_GetCubeMapSurface_ASync( OrigTex->PF->D3D9.CubeTexture, ( D_D3DCUBEMAP_FACES )j, 0, &OrigTex->PF->D3D9.Surface[ j ], ASyncThread ) != D_D3D_OK )
					{
						DXST_LOGFILE_ADDUTF16LE( "\xad\x30\xe5\x30\xfc\x30\xd6\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\xb5\x30\xfc\x30\xd5\x30\xa7\x30\xb9\x30\xa2\x30\xc9\x30\xec\x30\xb9\x30\x6e\x30\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"キューブテクスチャのサーフェスアドレスの取得に失敗しました\n" @*/ ) ;
						return -1 ;
					}
				}
			}
		}
		else
		{
			if( OrigTex->PF->D3D9.Surface[ 0 ] == NULL )
			{
				if( Direct3DTexture9_GetSurfaceLevel_ASync( OrigTex->PF->D3D9.Texture, 0, &OrigTex->PF->D3D9.Surface[ 0 ], ASyncThread ) != D_D3D_OK )
				{
					DXST_LOGFILE_ADDUTF16LE( "\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\xb5\x30\xfc\x30\xd5\x30\xa7\x30\xb9\x30\xa2\x30\xc9\x30\xec\x30\xb9\x30\x6e\x30\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"テクスチャのサーフェスアドレスの取得に失敗しました\n" @*/ ) ;
					return -1 ;
				}
			}
		}

		// ついでに真っ黒に塗りつぶす
		{
//			int j, MipCount, h ;
//			D_D3DLOCKED_RECT LockRect ;

			if( Orig->FormatDesc.DrawValidFlag )
			{
				for( j = 0 ; j < CUBEMAP_SURFACE_NUM ; j ++ )
				{
					if( OrigTex->PF->D3D9.Surface[ j ] != NULL )
					{
						Direct3DDevice9_ColorFill_ASync( OrigTex->PF->D3D9.Surface[ j ], NULL, 0x00000000, ASyncThread ) ;
					}
				}
			}
/*
			else
			// DXTフォーマットの場合はクリアしない
			if( Orig->ColorFormat != DX_GRAPHICSIMAGE_FORMAT_3D_DXT1 &&
				Orig->ColorFormat != DX_GRAPHICSIMAGE_FORMAT_3D_DXT2 &&
				Orig->ColorFormat != DX_GRAPHICSIMAGE_FORMAT_3D_DXT3 &&
				Orig->ColorFormat != DX_GRAPHICSIMAGE_FORMAT_3D_DXT4 &&
				Orig->ColorFormat != DX_GRAPHICSIMAGE_FORMAT_3D_DXT5 )
			{
				D_IDirect3DTexture9 *UseTex ;

				UseTex = Orig->FormatDesc.UseManagedTextureFlag == FALSE ? OrigTex->MemTexture : OrigTex->PF->D3D9.Texture ;
				h = OrigTex->TexHeight ;
				if( Orig->Hard.MipMapCount > 1 )
				{
					MipCount = Orig->Hard.MipMapCount ;
					for( j = 0 ; j < MipCount ; j ++, h >>= 1 )
					{
						if( UseTex->LockRect( j, &LockRect, NULL, 0 ) == D_D3D_OK )
						{
							_MEMSET( LockRect.pBits, 0, LockRect.Pitch * h ) ;

							UseTex->UnlockRect( j ) ;
						}
					}
				}
				else
				{
					if( UseTex->LockRect( 0, &LockRect, NULL, 0 ) == D_D3D_OK )
					{
						_MEMSET( LockRect.pBits, 0, LockRect.Pitch * h ) ;

						UseTex->UnlockRect( 0 ) ;
					}
				}

				if( Orig->FormatDesc.UseManagedTextureFlag == FALSE )
				{
					Direct3DDevice9_UpdateTexture( OrigTex->MemTexture, OrigTex->PF->D3D9.Texture ) ;
				}
			}
*/
		}
	}

	// 終了
	return 0 ;
}

// オリジナル画像情報中のテクスチャを解放する
extern	int		Graphics_Hardware_D3D9_ReleaseOrigTexture_PF( IMAGEDATA_ORIG *Orig )
{
	int i ;
	int j ;
	IMAGEDATA_ORIG_HARD_TEX *OrigTex ;

	// ハードウエアを使用する画像ではない場合は何もせずに終了
	if( Orig->FormatDesc.TextureFlag == FALSE ) return 0 ;

	// テクスチャの場合はテクスチャの数だけテクスチャを作成
	OrigTex = Orig->Hard.Tex ;
	for( i = 0 ; i < Orig->Hard.TexNum ; i ++, OrigTex ++ )
	{
		// サーフェスの解放
		for( j = 0 ; j < CUBEMAP_SURFACE_NUM ; j ++ )
		{
			if( OrigTex->PF->D3D9.Surface[ j ] )
			{
				Direct3D9_ObjectRelease( OrigTex->PF->D3D9.Surface[ j ] ) ;
				OrigTex->PF->D3D9.Surface[ j ] = NULL ;
			}
		}
		if( OrigTex->PF->D3D9.RenderTargetSurface )
		{
			Direct3D9_ObjectRelease( OrigTex->PF->D3D9.RenderTargetSurface ) ;
			OrigTex->PF->D3D9.RenderTargetSurface = NULL ;
		}
		if( OrigTex->PF->D3D9.ZBuffer )
		{
			Direct3D9_ObjectRelease( OrigTex->PF->D3D9.ZBuffer ) ;
			OrigTex->PF->D3D9.ZBuffer = NULL ;
		}

		// テクスチャの解放
		if( OrigTex->PF->D3D9.Texture )
		{
			Direct3D9_ObjectRelease( OrigTex->PF->D3D9.Texture ) ;
			OrigTex->PF->D3D9.Texture = NULL ;
		}
		
		/*if( OrigTex->MemTexture )
		{
			Direct3D9_ObjectRelease( OrigTex->MemTexture ) ;
			OrigTex->MemTexture = NULL ;
		}*/
	}

	// 終了
	return 0 ;
}

// 指定のマルチサンプル数で使用できる最大クオリティ値を取得する
extern	int		Graphics_Hardware_D3D9_GetMultiSampleQuality_PF( int Samples )
{
	IMAGEFORMATDESC				Format ;
	SETUP_GRAPHHANDLE_GPARAM	GParam ;
	int							FormatIndex ;
	D_D3DFORMAT					D3DFormat ;
	DWORD						Quality ;

	if( Samples > 16 )
	{
		Samples = 16 ;
	}

	if( Direct3D9_IsValid() == 0 )
	{
		return -1 ;
	}

	// 現在の設定のフォーマットをセットアップする
	Graphics_Image_InitSetupGraphHandleGParam( &GParam ) ; 
	GParam.DrawValidImageCreateFlag = TRUE ;
	GParam.CubeMapTextureCreateFlag = FALSE ;
	Graphics_Image_SetupFormatDesc( &Format, &GParam, 256, 256, GParam.AlphaChannelImageCreateFlag, FALSE, 0, DX_BASEIMAGE_FORMAT_NORMAL, -1 ) ;

	// フォーマットインデックスを得る
	FormatIndex = NS_GetTexFormatIndex( &Format ) ;

	// Direct3D でのフォーマットを取得
	D3DFormat = GD3D9.Device.Caps.TextureFormat[ FormatIndex ] ;
	if( D3DFormat == D_D3DFMT_UNKNOWN )
	{
		return -1 ;
	}

	// 設定できるクオリティを取得
	Quality = 10000 ;
	if( Direct3D9_CheckMultiSampleParam_ASync( D3DFormat, ( D_D3DMULTISAMPLE_TYPE * )&Samples, &Quality, TRUE ) < 0 )
	{
		return -1 ;
	}

	// クオリティを返す
	return ( int )Quality ;
}

// 作成するグラフィックハンドルで使用する環境依存のテクスチャフォーマットを指定する
extern int Graphics_Hardware_D3D9_SetUsePlatformTextureFormat_PF( int /*PlatformTextureFormat*/ )
{
	return 0 ;
}




























// 環境依存頂点バッファ・インデックスバッファ関係

// 頂点バッファハンドルの頂点バッファを作成する
extern	int		Graphics_Hardware_D3D9_VertexBuffer_Create_PF( VERTEXBUFFERHANDLEDATA *VertexBuffer )
{
	return Graphics_D3D9_VertexBuffer_CreateObject( VertexBuffer, FALSE ) ;
}

// 頂点バッファハンドルの後始末
extern	int		Graphics_Hardware_D3D9_VertexBuffer_Terminate_PF( VERTEXBUFFERHANDLEDATA *VertexBuffer )
{
	return Graphics_D3D9_VertexBuffer_ReleaseObject( VertexBuffer ) ;
}

// 頂点バッファに頂点データを転送する
extern	int		Graphics_Hardware_D3D9_VertexBuffer_SetData_PF( VERTEXBUFFERHANDLEDATA *VertexBuffer, int SetIndex, const void *VertexData, int VertexNum )
{
	void *pBuffer ;

	// Direct3DVertexBuffer9 がない場合はここで終了
	if( VertexBuffer->PF->D3D9.VertexBuffer == NULL )
	{
		return 0 ;
	}

	// バッファのロック
	if( Direct3DVertexBuffer9_Lock(
			VertexBuffer->PF->D3D9.VertexBuffer, 
			( UINT )( SetIndex  * VertexBuffer->UnitSize ),
			( UINT )( VertexNum * VertexBuffer->UnitSize ),
			&pBuffer,
			0 ) != D_D3D_OK )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x02\x98\xb9\x70\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\xed\x30\xc3\x30\xaf\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"頂点バッファのロックに失敗しました\n" @*/ ) ;
		return -1 ;
	}

	// バッファへデータを転送
	_MEMCPY( pBuffer, VertexData, ( size_t )( VertexNum * VertexBuffer->UnitSize ) ) ;

	// バッファのアンロック
	Direct3DVertexBuffer9_Unlock( VertexBuffer->PF->D3D9.VertexBuffer ) ;

	// 正常終了
	return 0 ;
}

// インデックスバッファハンドルのセットアップを行う
extern	int		Graphics_Hardware_D3D9_IndexBuffer_Create_PF( INDEXBUFFERHANDLEDATA *IndexBuffer )
{
	return Graphics_D3D9_IndexBuffer_CreateObject( IndexBuffer, FALSE ) ;
}

// インデックスバッファハンドルの後始末
extern	int		Graphics_Hardware_D3D9_IndexBuffer_Terminate_PF( INDEXBUFFERHANDLEDATA *IndexBuffer )
{
	return Graphics_D3D9_IndexBuffer_ReleaseObject( IndexBuffer ) ;
}

// インデックスバッファにインデックスデータを転送する
extern	int		Graphics_Hardware_D3D9_IndexBuffer_SetData_PF( INDEXBUFFERHANDLEDATA *IndexBuffer, int SetIndex, const void *IndexData, int IndexNum )
{
	void *pBuffer ;

	// Direct3DIndexBuffer9 が無い場合はここで終了
	if( IndexBuffer->PF->D3D9.IndexBuffer == NULL )
	{
		return 0 ;
	}

	// バッファのロック
	if( Direct3DIndexBuffer9_Lock(
			IndexBuffer->PF->D3D9.IndexBuffer, 
			( UINT )( SetIndex * IndexBuffer->UnitSize ),
			( UINT )( IndexNum * IndexBuffer->UnitSize ),
			&pBuffer,
			0 ) != D_D3D_OK )
	{
		DXST_LOGFILE_ADDUTF16LE( "\xa4\x30\xf3\x30\xc7\x30\xc3\x30\xaf\x30\xb9\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\xed\x30\xc3\x30\xaf\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"インデックスバッファのロックに失敗しました\n" @*/ ) ;
		return -1 ;
	}

	// バッファへデータを転送
	_MEMCPY( pBuffer, IndexData, ( size_t )( IndexNum * IndexBuffer->UnitSize ) ) ;

	// バッファのアンロック
	Direct3DIndexBuffer9_Unlock( IndexBuffer->PF->D3D9.IndexBuffer ) ;

	// 正常終了
	return 0 ;
}























// 環境依存ライト関係

// ライティングを行うかどうかを設定する
extern	int		Graphics_Hardware_D3D9_Light_SetUse_PF( int Flag )
{
	return Graphics_D3D9_DeviceState_SetLighting( Flag ) ;
}

// グローバルアンビエントライトカラーを設定する
extern	int		Graphics_Hardware_D3D9_Light_GlobalAmbient_PF( COLOR_F *Color )
{
	return Graphics_D3D9_DeviceState_SetAmbient( D_RGBA_MAKE( _DTOL( Color->r * 255.0f ), _DTOL( Color->g * 255.0f ), _DTOL( Color->b * 255.0f ), 0 ) ) ;
}

// ライトパラメータをセット
extern	int		Graphics_Hardware_D3D9_Light_SetState_PF( int LightNumber, LIGHTPARAM *LightParam )
{
	return Graphics_D3D9_DeviceState_SetLightState( LightNumber, ( D_D3DLIGHT9 * )LightParam ) ;
}

// ライトの有効、無効を変更
extern	int		Graphics_Hardware_D3D9_Light_SetEnable_PF( int LightNumber, int EnableState )
{
	return Graphics_D3D9_DeviceState_SetLightEnable( LightNumber, EnableState ) ;
}




























// 環境依存シャドウマップ関係

// シャドウマップデータに必要なテクスチャを作成する
extern	int		Graphics_Hardware_D3D9_ShadowMap_CreateTexture_PF( SHADOWMAPDATA *ShadowMap, int ASyncThread )
{
	// フォーマットの決定
	if( ShadowMap->TexFormat_Float )
	{
		if( ShadowMap->TexFormat_BitDepth == 16 )
		{
			ShadowMap->ColorFormat = /*DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_F16*/DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_F16 ;
			ShadowMap->ZBufferFormat = ZBUFFER_FORMAT_16BIT ;
		}
		else
		{
			ShadowMap->ColorFormat = /*DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ABGR_F32*/DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_F32 ;
			ShadowMap->ZBufferFormat = ZBUFFER_FORMAT_32BIT ;
		}
	}
	else
	{
		if( ShadowMap->TexFormat_BitDepth == 16 )
		{
			ShadowMap->ColorFormat = /*DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_I16*/ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_I16 ;
			ShadowMap->ZBufferFormat = ZBUFFER_FORMAT_16BIT ;
		}
		else
		{
			ShadowMap->ColorFormat = DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_F32 ;
			ShadowMap->ZBufferFormat = ZBUFFER_FORMAT_32BIT ;
		}
	}

	// 決定したフォーマットが作成できない場合は失敗
	if( GD3D9.Device.Caps.TextureFormat[ ShadowMap->ColorFormat ] == D_D3DFMT_UNKNOWN )
	{
		DXST_LOGFILE_ADDUTF16LE( "\xb7\x30\xe3\x30\xc9\x30\xa6\x30\xde\x30\xc3\x30\xd7\x30\xc7\x30\xfc\x30\xbf\x30\x28\x75\x6e\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x67\x30\x5c\x4f\x10\x62\x67\x30\x4d\x30\x8b\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"シャドウマップデータ用のフォーマットで作成できるテクスチャフォーマットがありませんでした\n" @*/ ) ;
		return -1 ;
	}

	// レンダリングターゲット深度バッファの作成
	if( ShadowMap->PF->D3D9.ZBufferSurface == NULL )
	{
		if( Direct3DDevice9_CreateDepthStencilSurface_ASync(
			( UINT )ShadowMap->BaseSizeX,					// 幅
			( UINT )ShadowMap->BaseSizeY,					// 高さ
			GD3D9.Device.Caps.ZBufferFormat[ ShadowMap->ZBufferFormat ],		// フォーマット
			D_D3DMULTISAMPLE_NONE,							// マルチサンプリングタイプ
			0,												// マルチサンプリングクオリティ
			FALSE,											// ロックはできない
			&ShadowMap->PF->D3D9.ZBufferSurface,			// D_IDirect3DSurface9 のポインタを受け取るアドレス
			NULL,											// 絶対 NULL
			ASyncThread
			) != D_D3D_OK )
		{
			DXST_LOGFILE_ADDUTF16LE( "\xb7\x30\xe3\x30\xc9\x30\xa6\x30\xde\x30\xc3\x30\xd7\x30\x28\x75\xf1\x6d\xa6\x5e\xb5\x30\xfc\x30\xd5\x30\xa7\x30\xb9\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"シャドウマップ用深度サーフェスの作成に失敗しました\n" @*/ ) ;
			return -1 ;
		}
	}

	// レンダーターゲットにできるテクスチャの作成
	if( ShadowMap->PF->D3D9.Texture == NULL )
	{
		if( Direct3DDevice9_CreateTexture_ASync(
			( UINT )ShadowMap->BaseSizeX,					// 幅
			( UINT )ShadowMap->BaseSizeY,					// 高さ
			1,												// MipMap レベル
			D_D3DUSAGE_RENDERTARGET,						// フラグ
			GD3D9.Device.Caps.TextureFormat[ ShadowMap->ColorFormat ],		// フォーマット
			D_D3DPOOL_DEFAULT,								// テクスチャを DirectX が管理するかどうか
			&ShadowMap->PF->D3D9.Texture,					// D_IDirect3DTexture9 のポインタを受け取るアドレス
			NULL,											// 絶対 NULL
			ASyncThread
			) != D_D3D_OK )
		{
			DXST_LOGFILE_ADDUTF16LE( "\xb7\x30\xe3\x30\xc9\x30\xa6\x30\xde\x30\xc3\x30\xd7\x30\x28\x75\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"シャドウマップ用テクスチャの作成に失敗しました\n" @*/ ) ;
			return -1 ;
		}

		// テクスチャのサーフェスを取得しておく
		if( Direct3DTexture9_GetSurfaceLevel_ASync( ShadowMap->PF->D3D9.Texture, 0, &ShadowMap->PF->D3D9.Surface, ASyncThread ) != D_D3D_OK )
		{
			DXST_LOGFILE_ADDUTF16LE( "\xb7\x30\xe3\x30\xc9\x30\xa6\x30\xde\x30\xc3\x30\xd7\x30\x28\x75\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\xb5\x30\xfc\x30\xd5\x30\xa7\x30\xb9\x30\xa2\x30\xc9\x30\xec\x30\xb9\x30\x6e\x30\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"シャドウマップ用テクスチャのサーフェスアドレスの取得に失敗しました\n" @*/ ) ;
			return -1 ;
		}
	}

	// 終了
	return 0 ;
}

// シャドウマップデータに必要なテクスチャを解放する
extern	int		Graphics_Hardware_D3D9_ShadowMap_ReleaseTexture_PF( SHADOWMAPDATA *ShadowMap )
{
	if( ShadowMap->PF->D3D9.ZBufferSurface != NULL )
	{
		Direct3D9_ObjectRelease( ShadowMap->PF->D3D9.ZBufferSurface ) ;
		ShadowMap->PF->D3D9.ZBufferSurface = NULL ;
	}

	if( ShadowMap->PF->D3D9.Surface != NULL )
	{
		Direct3D9_ObjectRelease( ShadowMap->PF->D3D9.Surface ) ;
		ShadowMap->PF->D3D9.Surface = NULL ;
	}

	if( ShadowMap->PF->D3D9.Texture != NULL )
	{
		Direct3D9_ObjectRelease( ShadowMap->PF->D3D9.Texture ) ;
		ShadowMap->PF->D3D9.Texture = NULL ;
	}

	// 終了
	return 0 ;
}

// 頂点シェーダーに設定するシャドウマップの情報を更新する
extern	void	Graphics_Hardware_D3D9_ShadowMap_RefreshVSParam_PF( void )
{
	SHADOWMAPDATA *ShadowMap ;
	float TranspMat[ 4 ][ 4 ] ;
	static float TranspMatDef[ 4 ][ 4 ] =
	{
		{ 0.0f, 0.0f, 0.0f, -1.0f },
		{ 0.0f, 0.0f, 0.0f, -1.0f },
		{ 0.0f, 0.0f, 0.0f, -1.0f },
		{ 0.0f, 0.0f, 0.0f, -1.0f },
	} ;
	int i ;

	for( i = 0 ; i < MAX_USE_SHADOWMAP_NUM ; i ++ )
	{
		// アドレスの取得
		if( SHADOWMAPCHKFULL( GSYS.DrawSetting.ShadowMap[ i ], ShadowMap ) )
		{
			Graphics_D3D9_ShaderConstant_InfoSet_SetParam(
				&GD3D9.Device.Shader.ShaderConstantInfo,
				DX_SHADERCONSTANTTYPE_VS_FLOAT,
				DX_SHADERCONSTANTSET_LIB,
				DX_VS_CONSTF_SHADOWMAP1_LIGHT_VIEW_PROJECTION_MAT + i * 4,
				TranspMatDef,
				4,
				TRUE
			) ;

		}
		else
		{
			// 行列のセット
			TranspMat[ 0 ][ 0 ] = ShadowMap->ShadowMapViewProjectionMatrix.m[ 0 ][ 0 ] ;
			TranspMat[ 0 ][ 1 ] = ShadowMap->ShadowMapViewProjectionMatrix.m[ 1 ][ 0 ] ;
			TranspMat[ 0 ][ 2 ] = ShadowMap->ShadowMapViewProjectionMatrix.m[ 2 ][ 0 ] ;
			TranspMat[ 0 ][ 3 ] = ShadowMap->ShadowMapViewProjectionMatrix.m[ 3 ][ 0 ] ;

			TranspMat[ 1 ][ 0 ] = ShadowMap->ShadowMapViewProjectionMatrix.m[ 0 ][ 1 ] ;
			TranspMat[ 1 ][ 1 ] = ShadowMap->ShadowMapViewProjectionMatrix.m[ 1 ][ 1 ] ;
			TranspMat[ 1 ][ 2 ] = ShadowMap->ShadowMapViewProjectionMatrix.m[ 2 ][ 1 ] ;
			TranspMat[ 1 ][ 3 ] = ShadowMap->ShadowMapViewProjectionMatrix.m[ 3 ][ 1 ] ;

			TranspMat[ 2 ][ 0 ] = ShadowMap->ShadowMapViewProjectionMatrix.m[ 0 ][ 2 ] ;
			TranspMat[ 2 ][ 1 ] = ShadowMap->ShadowMapViewProjectionMatrix.m[ 1 ][ 2 ] ;
			TranspMat[ 2 ][ 2 ] = ShadowMap->ShadowMapViewProjectionMatrix.m[ 2 ][ 2 ] ;
			TranspMat[ 2 ][ 3 ] = ShadowMap->ShadowMapViewProjectionMatrix.m[ 3 ][ 2 ] ;

			TranspMat[ 3 ][ 0 ] = ShadowMap->ShadowMapViewProjectionMatrix.m[ 0 ][ 3 ] ;
			TranspMat[ 3 ][ 1 ] = ShadowMap->ShadowMapViewProjectionMatrix.m[ 1 ][ 3 ] ;
			TranspMat[ 3 ][ 2 ] = ShadowMap->ShadowMapViewProjectionMatrix.m[ 2 ][ 3 ] ;
			TranspMat[ 3 ][ 3 ] = ShadowMap->ShadowMapViewProjectionMatrix.m[ 3 ][ 3 ] ;

			Graphics_D3D9_ShaderConstant_InfoSet_SetParam(
				&GD3D9.Device.Shader.ShaderConstantInfo,
				DX_SHADERCONSTANTTYPE_VS_FLOAT,
				DX_SHADERCONSTANTSET_LIB,
				DX_VS_CONSTF_SHADOWMAP1_LIGHT_VIEW_PROJECTION_MAT + i * 4,
				TranspMat,
				4,
				TRUE
			) ;
		}
	}
}

// ピクセルシェーダーに設定するシャドウマップの情報を更新する
extern	void	Graphics_Hardware_D3D9_ShadowMap_RefreshPSParam_PF( void )
{
	SHADOWMAPDATA *ShadowMap ;
	float ShadowMapParam[ 4 ][ 4 ] ;
	LIGHT_HANDLE *pLH ;
	static int ConstAddressTable[ MAX_USE_SHADOWMAP_NUM ][ 5 ][ 2 ] =
	{
		// ShadowMap1
		{
			{ 0, 0 },
			{ 0, 1 },
			{ 0, 2 },
			{ 0, 3 },
			{ 1, 0 },
		},

		// ShadowMap2
		{
			{ 1, 1 },
			{ 1, 2 },
			{ 1, 3 },
			{ 2, 0 },
			{ 2, 1 },
		},

		// ShadowMap3
		{
			{ 2, 2 },
			{ 2, 3 },
			{ 3, 0 },
			{ 3, 1 },
			{ 3, 2 },
		},
	} ;
	int CheckNum ;
	int i ;
	int j ;

	CheckNum = GSYS.Light.EnableNum > 3 /* DX_PS_CONSTB_LIGHT2_NUM */ ? 3 /* DX_PS_CONSTB_LIGHT2_NUM */ : GSYS.Light.EnableNum ;

	for( i = 0 ; i < MAX_USE_SHADOWMAP_NUM ; i ++ )
	{
		for( j = 0; j < 3; j++ )
		{
			ShadowMapParam[ ConstAddressTable[ i ][ j + 2 ][ 0 ] ][ ConstAddressTable[ i ][ j + 2 ][ 1 ] ] = 1.0f ;
		}

		// アドレスの取得
		if( SHADOWMAPCHKFULL( GSYS.DrawSetting.ShadowMap[ i ], ShadowMap ) )
		{
			ShadowMapParam[ ConstAddressTable[ i ][ 0 ][ 0 ] ][ ConstAddressTable[ i ][ 0 ][ 1 ] ] = 1.0f ;
			ShadowMapParam[ ConstAddressTable[ i ][ 1 ][ 0 ] ][ ConstAddressTable[ i ][ 1 ][ 1 ] ] = 0.0f ;
		}
		else
		{
			ShadowMapParam[ ConstAddressTable[ i ][ 0 ][ 0 ] ][ ConstAddressTable[ i ][ 0 ][ 1 ] ] = ShadowMap->AdjustDepth ;
			ShadowMapParam[ ConstAddressTable[ i ][ 1 ][ 0 ] ][ ConstAddressTable[ i ][ 1 ][ 1 ] ] = ShadowMap->GradationParam ;

			for( j = 0 ; j < CheckNum ; j ++ )
			{
				pLH = GSYS.Light.Data[ GSYS.Light.EnableHardwareIndex[ j ] ] ;
				if( pLH->ShadowMapSlotDisableFlag[ i ] == FALSE )
				{
					ShadowMapParam[ ConstAddressTable[ i ][ j + 2 ][ 0 ] ][ ConstAddressTable[ i ][ j + 2 ][ 1 ] ] = 0.0f ;
				}
			}
		}
	}

	Graphics_D3D9_ShaderConstant_InfoSet_SetParam(
		&GD3D9.Device.Shader.ShaderConstantInfo,
		DX_SHADERCONSTANTTYPE_PS_FLOAT,
		DX_SHADERCONSTANTSET_LIB,
		DX_PS_CONSTF_SHADOW1_DADJ_GRAD_ENBL0_1,
		ShadowMapParam,
		4,
		TRUE
	) ;
}

// シャドウマップへの描画の準備を行う
extern	int		Graphics_Hardware_D3D9_ShadowMap_DrawSetup_PF( SHADOWMAPDATA * /* ShadowMap */ )
{
	// 使用するシェーダーのインデックスを更新する
	Graphics_D3D9_Shader_RefreshUseIndex() ;

	// 正常終了
	return 0 ;
}

// シャドウマップへの描画を終了する
extern	int		Graphics_Hardware_D3D9_ShadowMap_DrawEnd_PF( SHADOWMAPDATA * /* ShadowMap */ )
{
	// 使用するシェーダーのインデックスを更新する
	Graphics_D3D9_Shader_RefreshUseIndex() ;

	// 正常終了
	return 0 ;
}

// 描画で使用するシャドウマップを指定する( スロットは０か１かを指定可能 )
extern	int		Graphics_Hardware_D3D9_ShadowMap_SetUse_PF( int /* SlotIndex */, SHADOWMAPDATA * /* ShadowMap */ )
{
	// 使用するシェーダーのインデックスを更新する
	Graphics_D3D9_Shader_RefreshUseIndex() ;

	// パラメータが変更された、フラグを立てる
	GD3D9.Device.DrawSetting.ChangeBlendParamFlag = TRUE ;

	// Graphics_D3D11_DrawPreparation を行うべきフラグを立てる
	GD3D9.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// 正常終了
	return 0 ;
}































// 環境依存シェーダー関係

// シェーダーハンドルを初期化する
extern	int		Graphics_Hardware_D3D9_Shader_Create_PF( int ShaderHandle, int IsVertexShader, void *Image, int ImageSize, int ImageAfterFree, int ASyncThread )
{
	SHADERHANDLEDATA *pShader ;
	D_IDirect3DPixelShader9 *PixelShader = NULL ;
	D_IDirect3DVertexShader9 *VertexShader = NULL ;

	// エラー判定
	if( ASyncThread )
	{
		if( SHADERCHK_ASYNC( ShaderHandle, pShader ) )
			return -1 ;
	}
	else
	{
		if( SHADERCHK( ShaderHandle, pShader ) )
			return -1 ;
	}

	// シェーダーを作成する
	if( IsVertexShader )
	{
		if( Direct3DDevice9_CreateVertexShader_ASync( ( DWORD * )Image, &VertexShader, ASyncThread ) != D_D3D_OK )
		{
			DXST_LOGFILE_ADDUTF16LE( "\x02\x98\xb9\x70\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"頂点シェーダーの作成に失敗しました\n" @*/ ) ;
			goto ERR ;
		}
	}
	else
	{
		if( Direct3DDevice9_CreatePixelShader_ASync( ( DWORD * )Image, &PixelShader, ASyncThread ) != D_D3D_OK )
		{
			DXST_LOGFILE_ADDUTF16LE( "\xd4\x30\xaf\x30\xbb\x30\xeb\x30\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ピクセルシェーダーの作成に失敗しました\n" @*/ ) ;
			goto ERR ;
		}
	}

	// バイナリイメージを保存するメモリ領域の確保
	pShader->FunctionCode = DXCALLOC( ( size_t )ImageSize ) ;
	if( pShader->FunctionCode == NULL )
	{
		DXST_LOGFILE_ADDUTF16LE( "\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"メモリの確保に失敗しました" @*/ ) ;
		goto ERR ;
	}

	// バイナリイメージを保存する
	_MEMCPY( pShader->FunctionCode, Image, ( size_t )ImageSize ) ;

	// 頂点シェーダーかどうかを保存する
	pShader->IsVertexShader = IsVertexShader ? TRUE : FALSE ;

	// 補助情報があるかどうかを調べる
	if( ( ( BYTE * )pShader->FunctionCode )[ 4 ] == 0xfe && ( ( BYTE * )pShader->FunctionCode )[ 5 ] == 0xff )
	{
		// ある場合

		// 定数情報配列のアドレスを保存する
		pShader->PF->D3D9.ConstantInfo = ( D_D3DXSHADER_CONSTANTINFO * )( ( BYTE * )pShader->FunctionCode + 0x28 ) ;

		// 定数の数を保存する
		pShader->PF->D3D9.ConstantNum = *( ( int * )( ( BYTE * )pShader->FunctionCode + 0x18 ) ) ;
	}
	else
	{
		// 無い場合

		// 定数情報配列のアドレスを初期化する
		pShader->PF->D3D9.ConstantInfo = NULL ;

		// 情報としてアクセスできる定数は０個
		pShader->PF->D3D9.ConstantNum = 0 ;
	}

	// シェーダーのアドレスを保存
	if( IsVertexShader )
	{
		pShader->PF->D3D9.VertexShader = VertexShader ;
	}
	else
	{
		pShader->PF->D3D9.PixelShader = PixelShader ;
	}

	// 処理後に解放すべしのフラグが立っていたら解放する
	if( ImageAfterFree )
	{
		DXFREE( Image ) ;
	}

	// 正常終了
	return 0 ;

ERR :
	if( VertexShader )
	{
		Direct3D9_ObjectRelease_ASync( VertexShader, ASyncThread ) ;
	}

	if( PixelShader )
	{
		Direct3D9_ObjectRelease_ASync( PixelShader, ASyncThread ) ;
	}

	// 処理後に解放すべしのフラグが立っていたら解放する
	if( ImageAfterFree )
	{
		DXFREE( Image ) ;
	}

	return -1 ;
}

// シェーダーハンドルの後始末
extern	int		Graphics_Hardware_D3D9_Shader_TerminateHandle_PF( SHADERHANDLEDATA *Shader )
{
	// シェーダーの解放
	if( Shader->IsVertexShader )
	{
		if( Shader->PF->D3D9.VertexShader )
		{
			Direct3D9_ObjectRelease( Shader->PF->D3D9.VertexShader ) ;
			Shader->PF->D3D9.VertexShader = NULL ;
		}
	}
	else
	{
		if( Shader->PF->D3D9.PixelShader )
		{
			Direct3D9_ObjectRelease( Shader->PF->D3D9.PixelShader ) ;
			Shader->PF->D3D9.PixelShader = NULL ;
		}
	}

	// 正常終了
	return 0 ;
}

// 使用できるシェーダーのバージョンを取得する( 0=使えない  200=シェーダーモデル２．０が使用可能  300=シェーダーモデル３．０が使用可能 )
extern	int		Graphics_Hardware_D3D9_Shader_GetValidShaderVersion_PF( void )
{
	D_D3DCAPS9 DevCaps ;

	// シェーダが使えない場合は０を返す
	if( GAPIWin.Direct3DDevice9Object != NULL && GSYS.HardInfo.UseShader == FALSE )
		return 0 ;

	// デバイスが無い場合も０を返す
	if( GAPIWin.Direct3DDevice9Object == NULL )
		return 0 ;

	// 能力を取得
	Direct3D9_GetDeviceCaps( ( DWORD )( GSYS.Screen.ValidUseDisplayIndex ? GSYS.Screen.UseDisplayIndex : D_D3DADAPTER_DEFAULT ), D_D3DDEVTYPE_HAL, &DevCaps ) ;

	if( ( DevCaps.VertexShaderVersion & 0xffff ) < 0x200 &&
		( DevCaps.PixelShaderVersion  & 0xffff ) < 0x200 )
	{
		return 0 ;
	}
	else
	if( ( DevCaps.VertexShaderVersion & 0xffff ) >= 0x300 &&
		( DevCaps.PixelShaderVersion  & 0xffff ) >= 0x300 )
	{
		return 300 ;
	}
	else
	{
		return 200 ;
	}
}

// 指定の名前を持つ定数が使用するシェーダー定数の番号を取得する
extern	int		Graphics_Hardware_D3D9_Shader_GetConstIndex_PF( const wchar_t *ConstantName, SHADERHANDLEDATA *Shader )
{
	D_D3DXSHADER_CONSTANTINFO *Info ;

	// 指定の名前を持つ定数を捜す
	Info = Graphics_D3D9_GetShaderConstInfo( Shader, ConstantName ) ;

	// 無かったらエラー
	if( Info == NULL )
		return -1 ;

	// 使用しているレジスタ番号を返す
	return ( int )Info->RegisterIndex ;
}

// 指定の名前を持つ定数が使用するシェーダー定数の数を取得する
extern	int		Graphics_Hardware_D3D9_Shader_GetConstCount_PF( const wchar_t *ConstantName, SHADERHANDLEDATA *Shader )
{
	D_D3DXSHADER_CONSTANTINFO *Info ;

	// 指定の名前を持つ定数を捜す
	Info = Graphics_D3D9_GetShaderConstInfo( Shader, ConstantName ) ;

	// 無かったらエラー
	if( Info == NULL )
		return -1 ;

	// 使用しているレジスタ数を返す
	return ( int )Info->RegisterCount ;
}

// 指定の名前を持つ浮動小数点定数のデフォルトパラメータが格納されているメモリアドレスを取得する
extern	const FLOAT4 *Graphics_Hardware_D3D9_Shader_GetConstDefaultParamF_PF( const wchar_t *ConstantName, SHADERHANDLEDATA *Shader )
{
	D_D3DXSHADER_CONSTANTINFO *Info ;

	// 指定の名前を持つ定数を捜す
	Info = Graphics_D3D9_GetShaderConstInfo( Shader, ConstantName ) ;

	// 無かったらエラー
	if( Info == NULL )
		return NULL ;

	// あってもデフォルトパラメータが無い場合はエラー
	if( Info->DefaultValue == 0 )
		return NULL ;

	// デフォルトパラメータのある位置を返す
	return ( FLOAT4 * )( ( char * )Shader->FunctionCode + Info->DefaultValue + 12 ) ;
}

// シェーダー定数情報を設定する
extern	int		Graphics_Hardware_D3D9_Shader_SetConst_PF( int TypeIndex, int SetIndex, int ConstantIndex, const void *Param, int ParamNum, int UpdateUseArea )
{
	// 定数セット
	Graphics_D3D9_ShaderConstant_InfoSet_SetParam( &GD3D9.Device.Shader.ShaderConstantInfo, TypeIndex, SetIndex, ConstantIndex, Param, ParamNum, UpdateUseArea ) ;

	// 正常終了
	return 0 ;
}

// 指定領域のシェーダー定数情報をリセットする
extern	int		Graphics_Hardware_D3D9_Shader_ResetConst_PF( int TypeIndex, int SetIndex, int ConstantIndex, int ParamNum )
{
	// 定数リセット
	Graphics_D3D9_ShaderConstant_InfoSet_ResetParam( &GD3D9.Device.Shader.ShaderConstantInfo, TypeIndex, SetIndex, ConstantIndex, ParamNum ) ;

	// 正常終了
	return 0 ;
}

#ifndef DX_NON_MODEL
// ３Ｄモデル用のシェーダーコードの初期化を行う
extern	int		Graphics_Hardware_D3D9_Shader_ModelCode_Init_PF( void )
{
	if( Graphics_D3D9_ShaderCode_Model_Initialize() == FALSE )
	{
		return -1 ;
	}

	// 正常終了
	return 0 ;
}
#endif // DX_NON_MODEL






























// 環境依存シェーダー用定数バッファ関係

// シェーダー用定数バッファハンドルを初期化する
extern	int		Graphics_Hardware_D3D9_ShaderConstantBuffer_Create_PF( int /*ShaderConstantBufferHandle*/, int /*BufferSize*/, int /*ASyncThread*/ )
{
	return -1 ;
}

// シェーダー用定数バッファハンドルの後始末
extern	int		Graphics_Hardware_D3D9_ShaderConstantBuffer_TerminateHandle_PF( SHADERCONSTANTBUFFERHANDLEDATA * /*ShaderConstantBuffer*/ )
{
	return -1 ;
}

// シェーダー用定数バッファハンドルの定数バッファのアドレスを取得する
extern	void *	Graphics_Hardware_D3D9_ShaderConstantBuffer_GetBuffer_PF( SHADERCONSTANTBUFFERHANDLEDATA * /*ShaderConstantBuffer*/ )
{
	return NULL ;
}

// シェーダー用定数バッファハンドルの定数バッファへの変更を適用する
extern	int		Graphics_Hardware_D3D9_ShaderConstantBuffer_Update_PF( SHADERCONSTANTBUFFERHANDLEDATA * /*ShaderConstantBuffer*/ )
{
	return -1 ;
}

// シェーダー用定数バッファハンドルの定数バッファを指定のシェーダーの指定のスロットにセットする
extern	int		Graphics_Hardware_D3D9_ShaderConstantBuffer_Set_PF( SHADERCONSTANTBUFFERHANDLEDATA * /*ShaderConstantBuffer*/, int /*TargetShader*/ /* DX_SHADERTYPE_VERTEX など */, int /*Slot*/ )
{
	return -1 ;
}


























// 環境依存描画関係

// 指定点から境界色があるところまで塗りつぶす
extern	int		Graphics_Hardware_D3D9_Paint_PF( int /*x*/, int /*y*/, unsigned int /*FillColor*/, ULONGLONG /*BoundaryColor*/ )
{
	// デフォルトの処理を行う
	return 2 ;

//	MEMIMG MemImg ;
//
//	if( GAPIWin.Direct3DDevice9Object == NULL ) return -1 ;
//
//	// 描画待機している描画物を描画
//	DRAWSTOCKINFO
//
//	// 描画を終了させておく
//	Graphics_D3D9_EndScene() ;
//
//	// フルスクリーンアンチエイリアスを使用しているかどうかで処理を分岐
//	//if( ( D_D3DMULTISAMPLE_TYPE )GSYS.Setting.FSAAMultiSampleCount != D_D3DMULTISAMPLE_NONE )
//	if( true )
//	{
//		BASEIMAGE ScreenImage ;
//		int       DrawMode ;
//		int       DrawBlendMode ;
//		int       DrawBlendParam ;
//		RGBCOLOR  DrawBright ;
//		RECT      DrawArea ;
//
//		// 画面のイメージを取得
//		NS_CreateXRGB8ColorBaseImage( GSYS.DrawSetting.DrawSizeX, GSYS.DrawSetting.DrawSizeY, &ScreenImage ) ;
//		NS_GetDrawScreenBaseImage( 0, 0, GSYS.DrawSetting.DrawSizeX, GSYS.DrawSetting.DrawSizeY, &ScreenImage ) ;
//
//		// Paint 処理用の MEMIMG をセットアップする
//		_MEMSET( &MemImg, 0, sizeof( MEMIMG ) ) ;
//		InitializeMemImg(
//			&MemImg,
//			GSYS.DrawSetting.DrawSizeX,
//			GSYS.DrawSetting.DrawSizeY,
//			ScreenImage.Pitch,
//			0,
//			1,
//			FALSE, FALSE, FALSE, ScreenImage.GraphData ) ;
//
//		// Paint 処理を行う
//		PaintMemImg( &MemImg, x, y, FillColor, BoundaryColor ) ;
//
//		// MEMIMG の後始末
//		TerminateMemImg( &MemImg ) ;
//
//		// 画面にペイント後の画像を描画
//		DrawBlendMode  = GSYS.DrawSetting.BlendMode ;
//		DrawBlendParam = GSYS.DrawSetting.BlendParam ;
//		DrawMode       = GSYS.DrawSetting.DrawMode ;
//		DrawBright     = GSYS.DrawSetting.DrawBright ;
//		DrawArea       = GSYS.DrawSetting.DrawArea ;
//
//		NS_SetDrawBlendMode( DX_BLENDMODE_SRCCOLOR, 255 ) ;
//		NS_SetDrawMode( DX_DRAWMODE_NEAREST ) ;
//		NS_SetDrawBright( 255, 255, 255 ) ;
//		NS_SetDrawArea( 0, 0, GSYS.DrawSetting.DrawSizeX, GSYS.DrawSetting.DrawSizeY ) ;
//
//		NS_DrawBaseImage( 0, 0, &ScreenImage ) ;
//
//		NS_SetDrawBlendMode( DrawBlendMode, DrawBlendParam ) ;
//		NS_SetDrawMode( DrawMode ) ;
//		NS_SetDrawBright( DrawBright.Red, DrawBright.Green, DrawBright.Blue ) ;
//		NS_SetDrawArea( DrawArea.left, DrawArea.top, DrawArea.right, DrawArea.bottom ) ;
//
//		NS_ReleaseBaseImage( &ScreenImage ) ;
//	}
//	else
//	{
//		D_D3DLOCKED_RECT     LockRect ;
//		COLORDATA           *ColorData ;
//		D_D3DFORMAT          TargetFormat ;
//		D_IDirect3DSurface9 *TargetSurface, *SMSurface ;
//		int                  Width ;
//		int                  Height ;
//		IMAGEDATA           *Image ;
//
//		// エラー判定
//		if( GRAPHCHKFULL( GSYS.DrawSetting.TargetScreen[ 0 ], Image ) )
//			Image = NULL ;
//
//		// 描画先によって処理を分岐
//		if( Image )
//		{
//			// 描画先が描画可能サーフェスの場合
//			TargetSurface = Image->Orig->Hard.Tex[ 0 ].PF->D3D9.Surface[ GSYS.DrawSetting.TargetScreenSurface[ 0 ] ] ;
//			TargetFormat  = GD3D9.Device.Caps.TextureFormat[ Image->Orig->ColorFormat ] ;
//			Width         = Image->Orig->Hard.Tex[ 0 ].TexWidth ;
//			Height        = Image->Orig->Hard.Tex[ 0 ].TexHeight ;
//		}
//		else
//		{
//			// 描画先がサブバックバッファ又はバックバッファの場合
//			TargetSurface = GD3D9.Device.Screen.SubBackBufferSurface ? GD3D9.Device.Screen.SubBackBufferSurface : GD3D9.Device.Screen.BackBufferSurface ; 
//			TargetFormat  = GD3D9.Device.Caps.ScreenFormat ;
//			Width         = GSYS.Screen.MainScreenSizeX ;
//			Height        = GSYS.Screen.MainScreenSizeY ;
//		}
//
//		ColorData = Graphics_D3D9_GetD3DFormatColorData( TargetFormat ) ;
//
//		// 描画先のコピーを取得するシステムメモリサーフェスを作成
//		if( Direct3DDevice9_CreateOffscreenPlainSurface(
//				( UINT )Width, ( UINT )Height, TargetFormat, D_D3DPOOL_SYSTEMMEM, &SMSurface, NULL ) != D_D3D_OK )
//		{
//			DXST_LOGFILE_ADDUTF16LE( L"Paint 用システムメモリサーフェスの作成に失敗しました\n" ) ;
//			return -1 ;
//		}
//
//		// システムメモリサーフェスに描画先の内容を転送する
//		Direct3DDevice9_GetRenderTargetData(
//			TargetSurface, SMSurface ) ;
//
//		// システムメモリサーフェスをロック
//		Direct3DSurface9_LockRect( SMSurface, &LockRect, NULL, D_D3DLOCK_READONLY ) ; 
//
//		// Paint 処理用の MEMIMG をセットアップする
//		_MEMSET( &MemImg, 0, sizeof( MEMIMG ) ) ;
//		InitializeMemImg(
//			&MemImg,
//			GSYS.DrawSetting.DrawSizeX,
//			GSYS.DrawSetting.DrawSizeY,
//			LockRect.Pitch,
//			0,
//			ColorData->ColorBitDepth == 16 ? 0 : 1,
//			FALSE, FALSE, FALSE, LockRect.pBits ) ;
//
//		// Paint 処理を行う
//		PaintMemImg( &MemImg, x, y, FillColor, BoundaryColor ) ;
//
//		// MEMIMG の後始末
//		TerminateMemImg( &MemImg ) ;
//
//		// システムメモリサーフェスのロックを解除
//		Direct3DSurface9_UnlockRect( SMSurface ) ;
//
//		// システムメモリサーフェスの内容を描画先に転送する
//		Direct3DDevice9_UpdateSurface(
//			SMSurface,     NULL,
//			TargetSurface, NULL ) ;
//
//		// システムメモリサーフェスの解放
//		Direct3D9_ObjectRelease( SMSurface ) ;
//	}
//
//	// 正常終了
//	return 0 ;
}


























#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_NON_DIRECT3D9

#endif // DX_NON_GRAPHICS


