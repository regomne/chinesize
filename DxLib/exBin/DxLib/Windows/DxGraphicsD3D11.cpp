//-----------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		描画処理プログラム( Direct3D11 )
// 
//  	Ver 3.20c
// 
//-----------------------------------------------------------------------------

#define VERTEXBUFFERMODE		2

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

#include "../DxCompileConfig.h"

#ifndef DX_NON_GRAPHICS

#ifndef DX_NON_DIRECT3D11

// インクルード ---------------------------------------------------------------
#include "DxGraphicsD3D11.h"
#include "DxGraphicsWin.h"
#include "DxMaskD3D11.h"
#include "DxWindow.h"
#include "DxWinAPI.h"
#include "DxGUID.h"
#include "../DxSystem.h"
#include "../DxLog.h"
#include "../DxModel.h"
#include "../DxMovie.h"
#include "../DxMask.h"
#include "../DxMath.h"
#include "../DxBaseFunc.h"
#include "../DxGraphics.h"
#include "../DxGraphicsFilter.h"
#include "../DxInput.h"
#include "../DxASyncLoad.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 -----------------------------------------------------------------

// Graphics_D3D11_DrawPreparation を呼ぶ定型文
#define DX_D3D11_DRAWPREP_NOTEX( FLAG )													\
	if( GD3D11.Device.DrawSetting.RenderTexture != NULL )								\
		Graphics_D3D11_DrawSetting_SetTexture( NULL, NULL ) ;							\
	if( GSYS.ChangeSettingFlag ||														\
		GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag != FALSE ||						\
		GD3D11.Device.DrawSetting.DrawPrepParamFlag  != FLAG )							\
		Graphics_D3D11_DrawPreparation( FLAG ) ;												


#define DX_D3D11_DRAWPREP_TEX( ORIG, TEX, TEXSRV, FLAG )								\
	FLAG |= DX_D3D11_DRAWPREP_TEXTURE ;													\
	if( (ORIG)->FormatDesc.AlphaChFlag   ) FLAG |= DX_D3D11_DRAWPREP_TEXALPHACH ;		\
	if( (ORIG)->FormatDesc.AlphaTestFlag ) FLAG |= DX_D3D11_DRAWPREP_TEXALPHATEST ;		\
	if( GD3D11.Device.DrawSetting.RenderTexture != (TEX) )								\
		Graphics_D3D11_DrawSetting_SetTexture( (TEX), (TEXSRV) ) ;						\
	if( GSYS.ChangeSettingFlag ||														\
		GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag != FALSE ||						\
		GD3D11.Device.DrawSetting.DrawPrepParamFlag  != FLAG )							\
		Graphics_D3D11_DrawPreparation( FLAG ) ;


#define NORMALVERTEXBUFFER_MAP	\
	if( GD3D11.Device.DrawInfo.VertexBuffer     != NULL &&\
	    GD3D11.Device.DrawInfo.VertexBufferAddr == NULL )\
	{\
		D_D3D11_MAPPED_SUBRESOURCE MappedSubResource ;\
		HRESULT                    hr ;\
\
		hr = D3D11DeviceContext_Map_ASync( \
			GD3D11.Device.DrawInfo.VertexBuffer,\
			0,\
			D_D3D11_MAP_WRITE_DISCARD,\
			0,\
			&MappedSubResource\
		) ;\
\
		GD3D11.Device.DrawInfo.VertexBufferAddr = ( BYTE * )MappedSubResource.pData ;\
	}

#define NORMALVERTEXBUFFER_UNMAP	\
	if( GD3D11.Device.DrawInfo.VertexBuffer     != NULL &&\
	    GD3D11.Device.DrawInfo.VertexBufferAddr != NULL )\
	{\
		D3D11DeviceContext_Unmap_ASync(\
			GD3D11.Device.DrawInfo.VertexBuffer,\
			0\
		) ;\
		GD3D11.Device.DrawInfo.VertexBufferAddr = NULL ;\
	}

#define GETVERTEX_QUAD( p )		\
	MV1DRAWPACKDRAWMODEL		\
	if(\
		  GD3D11.Device.DrawInfo.Use3DVertex != 0 ||\
		  GD3D11.Device.DrawInfo.PrimitiveType != D_D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ||\
		  GD3D11.Device.DrawInfo.VertexNum + 6 >= D3D11_VertexBuffer_MaxVertexNum[ D3D11_VertexTypeToInputLayout[ GD3D11.Device.DrawInfo.Use3DVertex ][ GD3D11.Device.DrawInfo.VertexType ] ]\
	  )\
	{\
		Graphics_D3D11_RenderVertex( 0 ) ;\
		GD3D11.Device.DrawInfo.PrimitiveType = D_D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ;\
	}\
	*( ( void ** )&p ) = ( void * )GD3D11.Device.DrawInfo.VertexBufferNextAddr ;

#define GETVERTEX_BILLBOARD( p )		\
	MV1DRAWPACKDRAWMODEL		\
	if(\
		  GD3D11.Device.DrawInfo.Use3DVertex != 1 ||\
		  GD3D11.Device.DrawInfo.PrimitiveType != D_D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ||\
		  GD3D11.Device.DrawInfo.VertexNum + 6 >= D3D11_VertexBuffer_MaxVertexNum[ D3D11_VertexTypeToInputLayout[ GD3D11.Device.DrawInfo.Use3DVertex ][ GD3D11.Device.DrawInfo.VertexType ] ]\
	  )\
	{\
		Graphics_D3D11_RenderVertex( 1 ) ;\
		GD3D11.Device.DrawInfo.PrimitiveType = D_D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ;\
	}\
	*( ( void ** )&p ) = ( void * )GD3D11.Device.DrawInfo.VertexBufferNextAddr ;

#define GETVERTEX_TRIANGLE( p )		\
	MV1DRAWPACKDRAWMODEL		\
	if(\
		  GD3D11.Device.DrawInfo.Use3DVertex != 0 ||\
		  GD3D11.Device.DrawInfo.PrimitiveType != D_D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ||\
		  GD3D11.Device.DrawInfo.VertexNum + 3 >= D3D11_VertexBuffer_MaxVertexNum[ D3D11_VertexTypeToInputLayout[ GD3D11.Device.DrawInfo.Use3DVertex ][ GD3D11.Device.DrawInfo.VertexType ] ]\
	  )\
	{\
		Graphics_D3D11_RenderVertex( 0 ) ;\
		GD3D11.Device.DrawInfo.PrimitiveType = D_D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ;\
	}\
	*( ( void ** )&p ) = ( void * )GD3D11.Device.DrawInfo.VertexBufferNextAddr ;

#define GETVERTEX_TRIANGLE3D( p )		\
	MV1DRAWPACKDRAWMODEL		\
	if(\
		  GD3D11.Device.DrawInfo.Use3DVertex != 1 ||\
		  GD3D11.Device.DrawInfo.PrimitiveType != D_D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ||\
		  GD3D11.Device.DrawInfo.VertexNum + 3 >= D3D11_VertexBuffer_MaxVertexNum[ D3D11_VertexTypeToInputLayout[ GD3D11.Device.DrawInfo.Use3DVertex ][ GD3D11.Device.DrawInfo.VertexType ] ]\
	  )\
	{\
		Graphics_D3D11_RenderVertex( 1 ) ;\
		GD3D11.Device.DrawInfo.PrimitiveType = D_D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ;\
	}\
	*( ( void ** )&p ) = ( void * )GD3D11.Device.DrawInfo.VertexBufferNextAddr ;

#define GETVERTEX_LINEBOX( p )		\
	MV1DRAWPACKDRAWMODEL		\
	if(\
		  GD3D11.Device.DrawInfo.Use3DVertex != 0 ||\
		  GD3D11.Device.DrawInfo.PrimitiveType != D_D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ||\
		  GD3D11.Device.DrawInfo.VertexNum + 24 >= D3D11_VertexBuffer_MaxVertexNum[ D3D11_VertexTypeToInputLayout[ GD3D11.Device.DrawInfo.Use3DVertex ][ GD3D11.Device.DrawInfo.VertexType ] ]\
	  )\
	{\
		Graphics_D3D11_RenderVertex( 0 ) ;\
		GD3D11.Device.DrawInfo.PrimitiveType = D_D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ;\
	}\
	*( ( void ** )&p ) = ( void * )GD3D11.Device.DrawInfo.VertexBufferNextAddr ;

#define GETVERTEX_LINETRIANGLE( p )		\
	MV1DRAWPACKDRAWMODEL		\
	if(\
		  GD3D11.Device.DrawInfo.Use3DVertex != 0 ||\
		  GD3D11.Device.DrawInfo.PrimitiveType != D_D3D11_PRIMITIVE_TOPOLOGY_LINELIST ||\
		  GD3D11.Device.DrawInfo.VertexNum + 6 >= D3D11_VertexBuffer_MaxVertexNum[ D3D11_VertexTypeToInputLayout[ GD3D11.Device.DrawInfo.Use3DVertex ][ GD3D11.Device.DrawInfo.VertexType ] ]\
	  )\
	{\
		Graphics_D3D11_RenderVertex( 0 ) ;\
		GD3D11.Device.DrawInfo.PrimitiveType = D_D3D11_PRIMITIVE_TOPOLOGY_LINELIST ;\
	}\
	*( ( void ** )&p ) = ( void * )GD3D11.Device.DrawInfo.VertexBufferNextAddr ;

#define GETVERTEX_LINETRIANGLE3D( p )		\
	MV1DRAWPACKDRAWMODEL		\
	if(\
		  GD3D11.Device.DrawInfo.Use3DVertex != 1 ||\
		  GD3D11.Device.DrawInfo.PrimitiveType != D_D3D11_PRIMITIVE_TOPOLOGY_LINELIST ||\
		  GD3D11.Device.DrawInfo.VertexNum + 6 >= D3D11_VertexBuffer_MaxVertexNum[ D3D11_VertexTypeToInputLayout[ GD3D11.Device.DrawInfo.Use3DVertex ][ GD3D11.Device.DrawInfo.VertexType ] ]\
	  )\
	{\
		Graphics_D3D11_RenderVertex( 1 ) ;\
		GD3D11.Device.DrawInfo.PrimitiveType = D_D3D11_PRIMITIVE_TOPOLOGY_LINELIST ;\
	}\
	*( ( void ** )&p ) = ( void * )GD3D11.Device.DrawInfo.VertexBufferNextAddr ;

#define GETVERTEX_LINE( p )		\
	MV1DRAWPACKDRAWMODEL		\
	if(\
		  GD3D11.Device.DrawInfo.Use3DVertex == 1 ||\
		  GD3D11.Device.DrawInfo.PrimitiveType != D_D3D11_PRIMITIVE_TOPOLOGY_LINELIST ||\
		  GD3D11.Device.DrawInfo.VertexNum + 2 >= D3D11_VertexBuffer_MaxVertexNum[ D3D11_VertexTypeToInputLayout[ GD3D11.Device.DrawInfo.Use3DVertex ][ GD3D11.Device.DrawInfo.VertexType ] ]\
	  )\
	{\
		Graphics_D3D11_RenderVertex( 0 ) ;\
		GD3D11.Device.DrawInfo.PrimitiveType = D_D3D11_PRIMITIVE_TOPOLOGY_LINELIST ;\
	}\
	*( ( void ** )&p ) = ( void * )GD3D11.Device.DrawInfo.VertexBufferNextAddr ;


#define GETVERTEX_LINE3D( p )		\
	MV1DRAWPACKDRAWMODEL		\
	if(\
		  GD3D11.Device.DrawInfo.Use3DVertex != 1 ||\
		  GD3D11.Device.DrawInfo.PrimitiveType != D_D3D11_PRIMITIVE_TOPOLOGY_LINELIST ||\
		  GD3D11.Device.DrawInfo.VertexNum + 2 >= D3D11_VertexBuffer_MaxVertexNum[ D3D11_VertexTypeToInputLayout[ GD3D11.Device.DrawInfo.Use3DVertex ][ GD3D11.Device.DrawInfo.VertexType ] ]\
	  )\
	{\
		Graphics_D3D11_RenderVertex( 1 ) ;\
		GD3D11.Device.DrawInfo.PrimitiveType = D_D3D11_PRIMITIVE_TOPOLOGY_LINELIST ;\
	}\
	*( ( void ** )&p ) = ( void * )GD3D11.Device.DrawInfo.VertexBufferNextAddr ;


#define GETVERTEX_POINT( p )		\
	MV1DRAWPACKDRAWMODEL		\
	if(\
		  GD3D11.Device.DrawInfo.Use3DVertex != 0 ||\
		  GD3D11.Device.DrawInfo.PrimitiveType != D_D3D11_PRIMITIVE_TOPOLOGY_POINTLIST ||\
		  GD3D11.Device.DrawInfo.VertexNum + 1 >= D3D11_VertexBuffer_MaxVertexNum[ D3D11_VertexTypeToInputLayout[ GD3D11.Device.DrawInfo.Use3DVertex ][ GD3D11.Device.DrawInfo.VertexType ] ]\
	  )\
	{\
		Graphics_D3D11_RenderVertex( 0 ) ;\
		GD3D11.Device.DrawInfo.PrimitiveType = D_D3D11_PRIMITIVE_TOPOLOGY_POINTLIST ;\
	}\
	*( ( void ** )&p ) = ( void * )GD3D11.Device.DrawInfo.VertexBufferNextAddr ;

#define GETVERTEX_POINT3D( p )		\
	MV1DRAWPACKDRAWMODEL		\
	if(\
		  GD3D11.Device.DrawInfo.Use3DVertex != 1 ||\
		  GD3D11.Device.DrawInfo.PrimitiveType != D_D3D11_PRIMITIVE_TOPOLOGY_POINTLIST ||\
		  GD3D11.Device.DrawInfo.VertexNum + 1 >= D3D11_VertexBuffer_MaxVertexNum[ D3D11_VertexTypeToInputLayout[ GD3D11.Device.DrawInfo.Use3DVertex ][ GD3D11.Device.DrawInfo.VertexType ] ]\
	  )\
	{\
		Graphics_D3D11_RenderVertex( 1 ) ;\
		GD3D11.Device.DrawInfo.PrimitiveType = D_D3D11_PRIMITIVE_TOPOLOGY_POINTLIST ;\
	}\
	*( ( void ** )&p ) = ( void * )GD3D11.Device.DrawInfo.VertexBufferNextAddr ;



#define ADD4VERTEX_POINT		\
		GD3D11.Device.DrawInfo.VertexBufferNextAddr += sizeof( VERTEX_NOTEX_2D ) ;	\
		GD3D11.Device.DrawInfo.VertexNum ++ ;

#define ADD4VERTEX_POINT3D		\
		GD3D11.Device.DrawInfo.VertexBufferNextAddr += sizeof( VERTEX_NOTEX_3D ) ;	\
		GD3D11.Device.DrawInfo.VertexNum ++ ;
/*
#define ADD4VERTEX_LINEBOX			\
		GD3D11.Device.DrawInfo.VertexBufferNextAddr += sizeof( VERTEX_NOTEX_2D ) * 8 ;	\
		GD3D11.Device.DrawInfo.VertexNum += 8 ;
*/
#define ADD4VERTEX_LINEBOX			\
		GD3D11.Device.DrawInfo.VertexBufferNextAddr += sizeof( VERTEX_NOTEX_2D ) * 24 ;	\
		GD3D11.Device.DrawInfo.VertexNum += 24 ;

#define ADD4VERTEX_LINETRIANGLE		\
		GD3D11.Device.DrawInfo.VertexBufferNextAddr += sizeof( VERTEX_NOTEX_2D ) * 6 ;	\
		GD3D11.Device.DrawInfo.VertexNum += 6 ;

#define ADD4VERTEX_LINETRIANGLE3D		\
		GD3D11.Device.DrawInfo.VertexBufferNextAddr += sizeof( VERTEX_NOTEX_3D ) * 6 ;	\
		GD3D11.Device.DrawInfo.VertexNum += 6 ;

#define ADD4VERTEX_LINE			\
		GD3D11.Device.DrawInfo.VertexBufferNextAddr += sizeof( VERTEX_NOTEX_2D ) * 2 ;	\
		GD3D11.Device.DrawInfo.VertexNum += 2 ;

#define ADD4VERTEX_LINE3D		\
		GD3D11.Device.DrawInfo.VertexBufferNextAddr += sizeof( VERTEX_NOTEX_3D ) * 2 ;	\
		GD3D11.Device.DrawInfo.VertexNum += 2 ;

#define ADD4VERTEX_NOTEX		\
		GD3D11.Device.DrawInfo.VertexBufferNextAddr += sizeof( VERTEX_NOTEX_2D ) * 6 ;	\
		GD3D11.Device.DrawInfo.VertexNum += 6 ;

#define ADD4VERTEX_TEX			\
		GD3D11.Device.DrawInfo.VertexBufferNextAddr += sizeof( VERTEX_2D ) * 6 ;	\
		GD3D11.Device.DrawInfo.VertexNum += 6 ;

#define ADD4VERTEX_BLENDTEX		\
		GD3D11.Device.DrawInfo.VertexBufferNextAddr += sizeof( VERTEX_BLENDTEX_2D ) * 6 ;	\
		GD3D11.Device.DrawInfo.VertexNum += 6 ;

#define ADD3VERTEX_NOTEX		\
		GD3D11.Device.DrawInfo.VertexBufferNextAddr += sizeof( VERTEX_NOTEX_2D ) * 3 ;	\
		GD3D11.Device.DrawInfo.VertexNum += 3 ;

#define ADD3VERTEX_TEX			\
		GD3D11.Device.DrawInfo.VertexBufferNextAddr += sizeof( VERTEX_2D ) * 3 ;	\
		GD3D11.Device.DrawInfo.VertexNum += 3 ;

#define ADD3VERTEX_BLENDTEX		\
		GD3D11.Device.DrawInfo.VertexBufferNextAddr += sizeof( VERTEX_BLENDTEX_2D ) * 3 ;	\
		GD3D11.Device.DrawInfo.VertexNum += 3 ;

#define ADD3VERTEX_NOTEX3D		\
		GD3D11.Device.DrawInfo.VertexBufferNextAddr += sizeof( VERTEX_NOTEX_3D ) * 3 ;	\
		GD3D11.Device.DrawInfo.VertexNum += 3 ;

#define ADD4VERTEX_BILLBOARD		\
		GD3D11.Device.DrawInfo.VertexBufferNextAddr += sizeof( VERTEX_3D ) * 6 ;	\
		GD3D11.Device.DrawInfo.VertexNum += 6 ;

#define SETUPCOLOR( DEST )		\
	if( GSYS.HardInfo.UseVertexColorBGRAFormat )\
	{\
		DEST  = ( ( ( ( ( 	GD3D11.Device.DrawInfo.DiffuseColor & 0x00ff0000 ) >> 16 ) * Red   ) / 255 ) << 16 ) |\
				( ( ( ( ( 	GD3D11.Device.DrawInfo.DiffuseColor & 0x0000ff00 ) >> 8  ) * Green ) / 255 ) << 8  ) |\
				( ( ( ( (	GD3D11.Device.DrawInfo.DiffuseColor & 0x000000ff )       ) * Blue  ) / 255 )       ) | \
				( 			GD3D11.Device.DrawInfo.DiffuseColor & 0xff000000 ) ;\
	}\
	else\
	{\
		DEST  = ( ( ( ( ( 	GD3D11.Device.DrawInfo.DiffuseColor & 0x000000ff )       ) * Red   ) / 255 )       ) |\
				( ( ( ( ( 	GD3D11.Device.DrawInfo.DiffuseColor & 0x0000ff00 ) >> 8  ) * Green ) / 255 ) << 8  ) |\
				( ( ( ( (	GD3D11.Device.DrawInfo.DiffuseColor & 0x00ff0000 ) >> 16 ) * Blue  ) / 255 ) << 16 ) | \
				( 			GD3D11.Device.DrawInfo.DiffuseColor & 0xff000000 ) ;\
	}


#define D3D11_COMMONBUFFER_ADDSIZE			(16 * 1024)

// RGB値を輝度最大のRGB値に変換するためのボリュームテクスチャのサイズ
#define RGBTOVMAXRGB_TEX_SIZE		(128)

// 構造体宣言 -----------------------------------------------------------------

// 標準描画用の複雑な処理を行わない頂点シェーダーの情報
typedef struct tagGRAPHICS_HARDWARE_DIRECT3D11_BASE_SIMPLE_VERTEXSHADER_INFO
{
	const char *				ShaderFileName ;						// 使用する頂点シェーダーファイル名
	int							VertexDataSize ;						// 頂点データ一つあたりのバイト数
	D_D3D11_INPUT_ELEMENT_DESC	InputElementDesc[ 11 ] ;				// 入力頂点データ形式
} GRAPHICS_HARDWARE_DIRECT3D11_BASE_SIMPLE_VERTEXSHADER_INFO ;

// データ定義 -----------------------------------------------------------------

#define RENDER_BLEND_TYPE_NORMAL			(0)			// 0:通常描画
#define RENDER_BLEND_TYPE_MUL				(1)			// 1:乗算描画
#define RENDER_BLEND_TYPE_INVERSE			(2)			// 2:RGB反転
#define RENDER_BLEND_TYPE_X4				(3)			// 3:描画輝度4倍
#define RENDER_BLEND_TYPE_PMA_NORMAL		(4)			// 4:乗算済みαブレンドモードの通常描画
#define RENDER_BLEND_TYPE_PMA_INVERSE		(5)			// 5:乗算済みαブレンドモードのRGB反転
#define RENDER_BLEND_TYPE_PMA_X4			(6)			// 6:乗算済みαブレンドモードの描画輝度4倍
#define RENDER_BLEND_TYPE_NUM				(7)
typedef struct tagDX_DIRECT3D11_RENDER_BLEND_INFO
{
	int									RenderBlendType ;
	D_D3D11_RENDER_TARGET_BLEND_DESC	RenderTargetBlendDesc ;
	int									AlphaZeroNotDrawFlag ;
} DX_DIRECT3D11_RENDER_BLEND_INFO ;
static DX_DIRECT3D11_RENDER_BLEND_INFO g_DefaultBlendDescArray[ DX_BLENDMODE_NUM ] =
{
	{ RENDER_BLEND_TYPE_NORMAL,			{ FALSE, D_D3D11_BLEND_ONE,				D_D3D11_BLEND_ZERO,				D_D3D11_BLEND_OP_ADD,			D_D3D11_BLEND_ONE,				D_D3D11_BLEND_ZERO,				D_D3D11_BLEND_OP_ADD,			D_D3D11_COLOR_WRITE_ENABLE_ALL },  TRUE },	// DX_BLENDMODE_NOBLEND			ノーブレンド
	{ RENDER_BLEND_TYPE_NORMAL,			{ TRUE,  D_D3D11_BLEND_SRC_ALPHA,		D_D3D11_BLEND_INV_SRC_ALPHA,	D_D3D11_BLEND_OP_ADD,			D_D3D11_BLEND_SRC_ALPHA,		D_D3D11_BLEND_INV_SRC_ALPHA,	D_D3D11_BLEND_OP_ADD,			D_D3D11_COLOR_WRITE_ENABLE_ALL },  TRUE },	// DX_BLENDMODE_ALPHA			αブレンド
	{ RENDER_BLEND_TYPE_NORMAL,			{ TRUE,  D_D3D11_BLEND_SRC_ALPHA,		D_D3D11_BLEND_ONE,				D_D3D11_BLEND_OP_ADD,			D_D3D11_BLEND_SRC_ALPHA,		D_D3D11_BLEND_ONE,				D_D3D11_BLEND_OP_ADD,			D_D3D11_COLOR_WRITE_ENABLE_ALL },  TRUE },	// DX_BLENDMODE_ADD				加算ブレンド
	{ RENDER_BLEND_TYPE_NORMAL,			{ TRUE,  D_D3D11_BLEND_INV_DEST_COLOR,	D_D3D11_BLEND_ZERO,				D_D3D11_BLEND_OP_ADD,			D_D3D11_BLEND_INV_DEST_ALPHA,	D_D3D11_BLEND_ZERO,				D_D3D11_BLEND_OP_ADD,			D_D3D11_COLOR_WRITE_ENABLE_ALL },  TRUE },	// DX_BLENDMODE_SUB				減算ブレンド
	{ RENDER_BLEND_TYPE_NORMAL,			{ TRUE,  D_D3D11_BLEND_ZERO,			D_D3D11_BLEND_SRC_COLOR,		D_D3D11_BLEND_OP_ADD,			D_D3D11_BLEND_ZERO,				D_D3D11_BLEND_SRC_ALPHA,		D_D3D11_BLEND_OP_ADD,			D_D3D11_COLOR_WRITE_ENABLE_ALL }, FALSE },	// DX_BLENDMODE_MUL				乗算ブレンド
	{ RENDER_BLEND_TYPE_NORMAL,			{ TRUE,  D_D3D11_BLEND_SRC_ALPHA,		D_D3D11_BLEND_ONE,				D_D3D11_BLEND_OP_ADD,			D_D3D11_BLEND_SRC_ALPHA,		D_D3D11_BLEND_ONE,				D_D3D11_BLEND_OP_ADD,			D_D3D11_COLOR_WRITE_ENABLE_ALL },  TRUE },	// DX_BLENDMODE_SUB2			内部処理用減算ブレンド１
	{ RENDER_BLEND_TYPE_NORMAL,			{ FALSE, D_D3D11_BLEND_ONE,				D_D3D11_BLEND_ZERO,				D_D3D11_BLEND_OP_ADD,			D_D3D11_BLEND_ONE,				D_D3D11_BLEND_ZERO,				D_D3D11_BLEND_OP_ADD,			D_D3D11_COLOR_WRITE_ENABLE_ALL },  TRUE },	// DX_BLENDMODE_XOR				XORブレンド(非対応)
	{ RENDER_BLEND_TYPE_NORMAL,			{ FALSE, D_D3D11_BLEND_ONE,				D_D3D11_BLEND_ZERO,				D_D3D11_BLEND_OP_ADD,			D_D3D11_BLEND_ONE,				D_D3D11_BLEND_ZERO,				D_D3D11_BLEND_OP_ADD,			D_D3D11_COLOR_WRITE_ENABLE_ALL },  TRUE },	// 欠番
	{ RENDER_BLEND_TYPE_NORMAL,			{ TRUE,  D_D3D11_BLEND_ZERO,			D_D3D11_BLEND_ONE,				D_D3D11_BLEND_OP_ADD,			D_D3D11_BLEND_ZERO,				D_D3D11_BLEND_ONE,				D_D3D11_BLEND_OP_ADD,			D_D3D11_COLOR_WRITE_ENABLE_ALL }, FALSE },	// DX_BLENDMODE_DESTCOLOR		カラーは更新されない
	{ RENDER_BLEND_TYPE_NORMAL,			{ TRUE,  D_D3D11_BLEND_INV_DEST_COLOR,	D_D3D11_BLEND_ZERO,				D_D3D11_BLEND_OP_ADD,			D_D3D11_BLEND_INV_DEST_ALPHA,	D_D3D11_BLEND_ZERO,				D_D3D11_BLEND_OP_ADD,			D_D3D11_COLOR_WRITE_ENABLE_ALL }, FALSE },	// DX_BLENDMODE_INVDESTCOLOR	描画先の色の反転値を掛ける
	{ RENDER_BLEND_TYPE_INVERSE,		{ TRUE,  D_D3D11_BLEND_SRC_ALPHA,		D_D3D11_BLEND_INV_SRC_ALPHA,	D_D3D11_BLEND_OP_ADD,			D_D3D11_BLEND_SRC_ALPHA,		D_D3D11_BLEND_INV_SRC_ALPHA,	D_D3D11_BLEND_OP_ADD,			D_D3D11_COLOR_WRITE_ENABLE_ALL },  TRUE },	// DX_BLENDMODE_INVSRC			描画元の色を反転する
	{ RENDER_BLEND_TYPE_MUL,			{ TRUE,  D_D3D11_BLEND_ZERO,			D_D3D11_BLEND_SRC_COLOR,		D_D3D11_BLEND_OP_ADD,			D_D3D11_BLEND_ZERO,				D_D3D11_BLEND_SRC_ALPHA,		D_D3D11_BLEND_OP_ADD,			D_D3D11_COLOR_WRITE_ENABLE_ALL },  TRUE },	// DX_BLENDMODE_MULA			アルファチャンネル考慮付き乗算ブレンド
	{ RENDER_BLEND_TYPE_X4,				{ TRUE,  D_D3D11_BLEND_SRC_ALPHA,		D_D3D11_BLEND_INV_SRC_ALPHA,	D_D3D11_BLEND_OP_ADD,			D_D3D11_BLEND_SRC_ALPHA,		D_D3D11_BLEND_INV_SRC_ALPHA,	D_D3D11_BLEND_OP_ADD,			D_D3D11_COLOR_WRITE_ENABLE_ALL },  TRUE },	// DX_BLENDMODE_ALPHA_X4		αブレンドの描画側の輝度を最大４倍にできるモード
	{ RENDER_BLEND_TYPE_X4,				{ TRUE,  D_D3D11_BLEND_SRC_ALPHA,		D_D3D11_BLEND_ONE,				D_D3D11_BLEND_OP_ADD,			D_D3D11_BLEND_SRC_ALPHA,		D_D3D11_BLEND_ONE,				D_D3D11_BLEND_OP_ADD,			D_D3D11_COLOR_WRITE_ENABLE_ALL },  TRUE },	// DX_BLENDMODE_ADD_X4			加算ブレンドの描画側の輝度を最大４倍にできるモード
	{ RENDER_BLEND_TYPE_NORMAL,			{ TRUE,  D_D3D11_BLEND_ONE,				D_D3D11_BLEND_ZERO,				D_D3D11_BLEND_OP_ADD,			D_D3D11_BLEND_ONE,				D_D3D11_BLEND_ZERO,				D_D3D11_BLEND_OP_ADD,			D_D3D11_COLOR_WRITE_ENABLE_ALL }, FALSE },	// DX_BLENDMODE_SRCCOLOR		描画元のカラーでそのまま描画される
	{ RENDER_BLEND_TYPE_NORMAL,			{ TRUE,  D_D3D11_BLEND_ONE,				D_D3D11_BLEND_INV_SRC_ALPHA,	D_D3D11_BLEND_OP_ADD,			D_D3D11_BLEND_ONE,				D_D3D11_BLEND_INV_SRC_ALPHA,	D_D3D11_BLEND_OP_ADD,			D_D3D11_COLOR_WRITE_ENABLE_ALL }, FALSE },	// DX_BLENDMODE_HALF_ADD		半加算ブレンド
	{ RENDER_BLEND_TYPE_NORMAL,			{ TRUE,  D_D3D11_BLEND_SRC_ALPHA,		D_D3D11_BLEND_ONE,				D_D3D11_BLEND_OP_REV_SUBTRACT,	D_D3D11_BLEND_SRC_ALPHA,		D_D3D11_BLEND_ONE,				D_D3D11_BLEND_OP_REV_SUBTRACT,	D_D3D11_COLOR_WRITE_ENABLE_ALL },  TRUE },	// DX_BLENDMODE_SUB1			出力ブレンドが使用可能な場合の減算ブレンド

	{ RENDER_BLEND_TYPE_PMA_NORMAL,		{ TRUE,  D_D3D11_BLEND_ONE,				D_D3D11_BLEND_INV_SRC_ALPHA,	D_D3D11_BLEND_OP_ADD,			D_D3D11_BLEND_ONE,				D_D3D11_BLEND_INV_SRC_ALPHA,	D_D3D11_BLEND_OP_ADD,			D_D3D11_COLOR_WRITE_ENABLE_ALL },  TRUE },	// DX_BLENDMODE_PMA_ALPHA		乗算済みαブレンドモードのαブレンド
	{ RENDER_BLEND_TYPE_PMA_NORMAL,		{ TRUE,  D_D3D11_BLEND_ONE,				D_D3D11_BLEND_ONE,				D_D3D11_BLEND_OP_ADD,			D_D3D11_BLEND_ONE,				D_D3D11_BLEND_ONE,				D_D3D11_BLEND_OP_ADD,			D_D3D11_COLOR_WRITE_ENABLE_ALL },  TRUE },	// DX_BLENDMODE_PMA_ADD			乗算済みαブレンドモードの加算ブレンド
	{ RENDER_BLEND_TYPE_PMA_NORMAL,		{ TRUE,  D_D3D11_BLEND_ONE,				D_D3D11_BLEND_ONE,				D_D3D11_BLEND_OP_REV_SUBTRACT,	D_D3D11_BLEND_ONE,				D_D3D11_BLEND_ONE,				D_D3D11_BLEND_OP_REV_SUBTRACT,	D_D3D11_COLOR_WRITE_ENABLE_ALL },  TRUE },	// DX_BLENDMODE_PMA_SUB			乗算済みαブレンドモードの減算ブレンド
	{ RENDER_BLEND_TYPE_PMA_INVERSE,	{ TRUE,  D_D3D11_BLEND_ONE,				D_D3D11_BLEND_INV_SRC_ALPHA,	D_D3D11_BLEND_OP_ADD,			D_D3D11_BLEND_ONE,				D_D3D11_BLEND_INV_SRC_ALPHA,	D_D3D11_BLEND_OP_ADD,			D_D3D11_COLOR_WRITE_ENABLE_ALL },  TRUE },	// DX_BLENDMODE_PMA_INVSRC		乗算済みαブレンドモードの描画元の色を反転する
	{ RENDER_BLEND_TYPE_PMA_X4,			{ TRUE,  D_D3D11_BLEND_ONE,				D_D3D11_BLEND_INV_SRC_ALPHA,	D_D3D11_BLEND_OP_ADD,			D_D3D11_BLEND_ONE,				D_D3D11_BLEND_INV_SRC_ALPHA,	D_D3D11_BLEND_OP_ADD,			D_D3D11_COLOR_WRITE_ENABLE_ALL },  TRUE },	// DX_BLENDMODE_PMA_ALPHA_X4	乗算済みαブレンドモードのαブレンドの描画側の輝度を最大４倍にできるモード
	{ RENDER_BLEND_TYPE_PMA_X4,			{ TRUE,  D_D3D11_BLEND_ONE,				D_D3D11_BLEND_ONE,				D_D3D11_BLEND_OP_ADD,			D_D3D11_BLEND_ONE,				D_D3D11_BLEND_ONE,				D_D3D11_BLEND_OP_ADD,			D_D3D11_COLOR_WRITE_ENABLE_ALL },  TRUE },	// DX_BLENDMODE_PMA_ADD_X4		乗算済みαブレンドモードの加算ブレンドの描画側の輝度を最大４倍にできるモード
} ;

// 頂点バッファに格納できる頂点の最大数のテーブル
static const int D3D11_VertexBuffer_MaxVertexNum[ D3D11_VERTEX_INPUTLAYOUT_NUM ] =
{
	D3D11_VERTEXBUFFER_SIZE / sizeof( VERTEX_NOTEX_2D    ),
	D3D11_VERTEXBUFFER_SIZE / sizeof( VERTEX_2D          ),
	D3D11_VERTEXBUFFER_SIZE / sizeof( VERTEX_BLENDTEX_2D ),
	D3D11_VERTEXBUFFER_SIZE / sizeof( VERTEX_NOTEX_3D    ),
	D3D11_VERTEXBUFFER_SIZE / sizeof( VERTEX_3D          ),
	D3D11_VERTEXBUFFER_SIZE / sizeof( VERTEX3D           ),
	D3D11_VERTEXBUFFER_SIZE / sizeof( VERTEX3DSHADER     ),
} ;

// 各頂点データのサイズのテーブル
static const int D3D11_VertexSize[ D3D11_VERTEX_INPUTLAYOUT_NUM ] =
{
	sizeof( VERTEX_NOTEX_2D    ),
	sizeof( VERTEX_2D          ),
	sizeof( VERTEX_BLENDTEX_2D ),
	sizeof( VERTEX_NOTEX_3D    ),
	sizeof( VERTEX_3D          ),
	sizeof( VERTEX3D           ),
	sizeof( VERTEX3DSHADER     ),
} ;

// ３Ｄ頂点かどうかと頂点タイプの組み合わせに対応する頂点データタイプ[ 0:２Ｄ頂点  1:３Ｄ頂点 ][ 頂点タイプ ]
static const DWORD D3D11_VertexTypeToInputLayout[ 2 ][ VERTEXTYPE_NUM ] =
{
	{
		D3D11_VERTEX_INPUTLAYOUT_NOTEX_2D,
		D3D11_VERTEX_INPUTLAYOUT_2D,
		D3D11_VERTEX_INPUTLAYOUT_BLENDTEX_2D,
	},

	{
		D3D11_VERTEX_INPUTLAYOUT_NOTEX_3D,
		D3D11_VERTEX_INPUTLAYOUT_3D
	},
} ;

// 単位行列
static MATRIX D3D11_GlobalIdentMatrix =
{
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
} ;

// 標準描画用の複雑な処理を行わない頂点シェーダーの情報
static GRAPHICS_HARDWARE_DIRECT3D11_BASE_SIMPLE_VERTEXSHADER_INFO g_BaseSimpleVertexShaderInfo[ D3D11_VERTEX_INPUTLAYOUT_NUM ] =
{
	// D3D11_VERTEX_INPUTLAYOUT_NOTEX_2D( VERTEX_NOTEX_2D )
	{
		"Base_VERTEX_NOTEX_2D.vso",
		sizeof( VERTEX_NOTEX_2D ),
		{
			{ "POSITION",     0, D_DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,  D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",        0, D_DXGI_FORMAT_R8G8B8A8_UNORM,     0, 16, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ NULL },
		},
	},

	// D3D11_VERTEX_INPUTLAYOUT_2D( VERTEX2D or VERTEX_2D )
	{
		"Base_VERTEX2D.vso",
		sizeof( VERTEX2D ),
		{
			{ "POSITION",     0, D_DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,  D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",        0, D_DXGI_FORMAT_R8G8B8A8_UNORM,     0, 16, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",     0, D_DXGI_FORMAT_R32G32_FLOAT,       0, 20, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ NULL },
		},
	},

	// D3D11_VERTEX_INPUTLAYOUT_BLENDTEX_2D( VERTEX_BLENDTEX_2D or VERTEX2DSHADER )
	{
		"Base_VERTEX_BLENDTEX_2D.vso",		
		sizeof( VERTEX_BLENDTEX_2D ),
		{
			{ "POSITION",     0, D_DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,  D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",        0, D_DXGI_FORMAT_R8G8B8A8_UNORM,     0, 16, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",        1, D_DXGI_FORMAT_R8G8B8A8_UNORM,     0, 20, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",     0, D_DXGI_FORMAT_R32G32_FLOAT,       0, 24, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",     1, D_DXGI_FORMAT_R32G32_FLOAT,       0, 32, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ NULL },
		},
	},

	// D3D11_VERTEX_INPUTLAYOUT_NOTEX_3D( VERTEX_NOTEX_3D )
	{
		"Base_VERTEX_NOTEX_3D.vso",
		sizeof( VERTEX_NOTEX_3D ),
		{
			{ "POSITION",     0, D_DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",        0, D_DXGI_FORMAT_R8G8B8A8_UNORM,     0, 12, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ NULL },
		},
	},

	// D3D11_VERTEX_INPUTLAYOUT_3D( VERTEX_3D )
	{
		"Base_VERTEX_3D.vso",
		sizeof( VERTEX_3D ),
		{
			{ "POSITION",     0, D_DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",        0, D_DXGI_FORMAT_R8G8B8A8_UNORM,     0, 12, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",     0, D_DXGI_FORMAT_R32G32_FLOAT,       0, 16, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ NULL },
		},
	},

	// D3D11_VERTEX_INPUTLAYOUT_3D_LIGHT( VERTEX3D )
	{
		"Base_VERTEX3D.vso",
		sizeof( VERTEX3D ),
		{
			{ "POSITION",     0, D_DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",       0, D_DXGI_FORMAT_R32G32B32_FLOAT,    0, 12, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",        0, D_DXGI_FORMAT_R8G8B8A8_UNORM,     0, 24, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",        1, D_DXGI_FORMAT_R8G8B8A8_UNORM,     0, 28, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",     0, D_DXGI_FORMAT_R32G32_FLOAT,       0, 32, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",     1, D_DXGI_FORMAT_R32G32_FLOAT,       0, 40, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ NULL },
		},
	},

	// D3D11_VERTEX_INPUTLAYOUT_SHADER_3D( VERTEX3DSHADER )
	{
		"Base_VERTEX3DSHADER.vso",
		sizeof( VERTEX3DSHADER ),
		{
			{ "POSITION",     0, D_DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "POSITION",     1, D_DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",       0, D_DXGI_FORMAT_R32G32B32_FLOAT,    0, 28, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT",      0, D_DXGI_FORMAT_R32G32B32_FLOAT,    0, 40, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BINORMAL",     0, D_DXGI_FORMAT_R32G32B32_FLOAT,    0, 52, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",        0, D_DXGI_FORMAT_R8G8B8A8_UNORM,     0, 64, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",        1, D_DXGI_FORMAT_R8G8B8A8_UNORM,     0, 68, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",     0, D_DXGI_FORMAT_R32G32_FLOAT,       0, 72, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",     1, D_DXGI_FORMAT_R32G32_FLOAT,       0, 80, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ NULL },
		},
	},
} ;

// Direct3D11 を使ったグラフィックス処理情報の構造体
GRAPHICS_HARDDATA_DIRECT3D11 GraphicsHardDataDirect3D11 ;

// シェーダーバイナリ配列
extern int  DxShaderCodeBin_Model_D3D11Convert ;
extern BYTE DxShaderCodeBin_Model_D3D11[] ;

extern int  DxShaderCodeBin_Filter_D3D11Convert ;
extern BYTE DxShaderCodeBin_Filter_D3D11[] ;

extern int  DxShaderCodeBin_Base_D3D11Convert ;
extern BYTE DxShaderCodeBin_Base_D3D11[] ;

extern int  DxShaderCodeBin_Base3D_D3D11Convert ;
extern BYTE DxShaderCodeBin_Base3D_D3D11[] ;

extern int  DxShaderCodeBin_RgbaMix_D3D11Convert ;
extern BYTE DxShaderCodeBin_RgbaMix_D3D11[] ;

// 関数宣言 -------------------------------------------------------------------

static void Graphics_D3D11_VertexShaderArray_Release(      D_ID3D11VertexShader      **pObject, int Num ) ;		// 頂点シェーダー配列を解放する
static void Graphics_D3D11_PixelShaderArray_Release(       D_ID3D11PixelShader       **pObject, int Num ) ;		// ピクセルシェーダー配列を解放する
static void Graphics_D3D11_DepthStencilStateArray_Release( D_ID3D11DepthStencilState **pObject, int Num ) ;		// デプスステンシルステート配列を解放する
static void Graphics_D3D11_BlendStateArray_Release(        D_ID3D11BlendState        **pObject, int Num ) ;		// ブレンドステート配列を解放する
static void Graphics_D3D11_RasterizerStateArray_Release(   D_ID3D11RasterizerState   **pObject, int Num ) ;		// ラスタライザーステート配列を解放する
static void Graphics_D3D11_SamplerStateArray_Release(      D_ID3D11SamplerState      **pObject, int Num ) ;		// サンプラーステート配列を解放する

// D3D11Device ステータス関係
static int Graphics_D3D11_DeviceState_SetupSamplerState( void ) ;									// 現在の設定に基づいて ID3D11SamplerState をセットアップする
static int Graphics_D3D11_DeviceState_TerminateSamplerState( void ) ;								// ID3D11SamplerState を解放する

static int Graphics_D3D11_DeviceState_SetupRasterizerState( void ) ;								// 現在の設定に基づいて ID3D11RasterizerState をセットアップする
static int Graphics_D3D11_DeviceState_TerminateRasterizerState( void ) ;							// ID3D11RasterizerState を解放する

static int Graphics_D3D11_DeviceState_SetupDepthStencilState( void ) ;								// 現在の設定に基づいて ID3D11DepthStencilState をセットアップする
static int Graphics_D3D11_DeviceState_TerminateDepthStencilState( void ) ;							// ID3D11DepthStencilState を解放する

static int Graphics_D3D11_DeviceState_SetupBlendState( void ) ;										// 現在の設定に基づいて ID3D11BlendState をセットアップする
static int Graphics_D3D11_DeviceState_TerminateBlendState( void ) ;									// ID3D11BlendState を解放する

static int Graphics_D3D11_DeviceState_SetNormalTextureAddressTransformMatrix_ConstBuffer( void ) ;	// 標準描画用のテクスチャ座標変換行列を定数バッファにセットする
static int Graphics_D3D11_DeviceState_UpdateConstantFogParam( void ) ;								// フォグの色以外の定数情報を更新する
static void Graphics_D3D11_DeviceState_RefreshAmbientAndEmissiveParam( void ) ;						// アンビエントライトとエミッシブカラーを掛け合わせたパラメータを更新する
static void Graphics_D3D11_DeviceState_SetRenderTarget_SizeCheck( void ) ;							// 全ての描画対象のサイズが一致していたらレンダリングターゲットにセット


// Direct3D11 に設定する射影行列を更新する
static void Graphics_Hardware_D3D11_RefreshProjectionMatrix( void ) ;

static int Graphics_D3D11_BltBmpOrBaseImageToGraph3_MipMapBlt(
	      IMAGEDATA_ORIG	*Orig,
	const RECT				*SrcRect,
	const RECT				*DestRect,
	      D_ID3D11Texture2D	*UseTex,
	      int				TexWidth,
	      int				TexHeight,
	const BASEIMAGE			*RgbBaseImage,
	const BASEIMAGE			*AlphaBaseImage,
	const COLORDATA			*DestColor,
	      int				RedIsAlphaFlag,
	      int				UseTransColorConvAlpha,
	      int				ASyncThread
) ;

static int Graphics_D3D11_BltBmpOrBaseImageToGraph3_NoMipMapBlt(
	      IMAGEDATA_ORIG	*Orig,
	const RECT				*SrcRect,
	const RECT				*DestRect,
	      D_ID3D11Texture2D	*UseTex,
	      int				TexWidth,
	      int				TexHeight,
	      int				TexUseWidth,
	      int				TexUseHeight,
	const BASEIMAGE			*RgbBaseImage,
	const BASEIMAGE			*AlphaBaseImage,
	const COLORDATA			*DestColor,
	      int				RedIsAlphaFlag,
	      int				UseTransColorConvAlpha,
	      int				ASyncThread
) ;

__inline	static DWORD GetDiffuseColor( void )															// 現在のディフューズカラーを得る
			{
				if( GSYS.HardInfo.UseVertexColorBGRAFormat )
				{
					return  ( DWORD )( ( GSYS.DrawSetting.DrawBright.Red   << 16 ) |
									   ( GSYS.DrawSetting.DrawBright.Green << 8  ) |
									   ( GSYS.DrawSetting.DrawBright.Blue        ) |
									   ( ( ( GSYS.DrawSetting.BlendMode != DX_BLENDMODE_NOBLEND || GSYS.DrawSetting.UseNoBlendModeParam ) ? GSYS.DrawSetting.BlendParam : 255 ) << 24 ) ) ;
				}
				else
				{
					return  ( DWORD )( ( GSYS.DrawSetting.DrawBright.Red         ) |
									   ( GSYS.DrawSetting.DrawBright.Green << 8  ) |
									   ( GSYS.DrawSetting.DrawBright.Blue  << 16 ) |
									   ( ( ( GSYS.DrawSetting.BlendMode != DX_BLENDMODE_NOBLEND || GSYS.DrawSetting.UseNoBlendModeParam ) ? GSYS.DrawSetting.BlendParam : 255 ) << 24 ) ) ;
				}
			}

#ifndef DX_NON_MODEL
static	int		CreateRGBtoVMaxRGBVolumeTexture( void ) ;							// RGBカラーを輝度を最大にしたRGB値に変換するためのボリュームテクスチャを作成する( 0:成功  -1:失敗 )
#endif // DX_NON_MODEL

// プログラム -----------------------------------------------------------------

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
static int CreateRGBtoVMaxRGBVolumeTexture( void )
{
	D_D3D11_TEXTURE3D_DESC				Texture3DDesc ;
	D_D3D11_SHADER_RESOURCE_VIEW_DESC	SRVDesc ;
	D_D3D11_BOX							DestBox ;
	void								*TempBuffer ;
	int									r ;
	int									g ;
	int									b ;
	BYTE								*pDest ;
	HRESULT								hr ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	GD3D11.RGBtoVMaxRGBVolumeTexture	= NULL ;
	GD3D11.RGBtoVMaxRGBVolumeTextureSRV = NULL ;
	TempBuffer							= NULL ;

	// ボリュームテクスチャの作成
	_MEMSET( &Texture3DDesc, 0, sizeof( Texture3DDesc ) ) ;
	Texture3DDesc.Usage              = D_D3D11_USAGE_DEFAULT ;
	Texture3DDesc.Format             = D_DXGI_FORMAT_R8G8B8A8_UNORM ;
	Texture3DDesc.BindFlags          = D_D3D11_BIND_SHADER_RESOURCE ;
	Texture3DDesc.Width              = RGBTOVMAXRGB_TEX_SIZE ;
	Texture3DDesc.Height             = RGBTOVMAXRGB_TEX_SIZE ;
	Texture3DDesc.Depth              = RGBTOVMAXRGB_TEX_SIZE ;
	Texture3DDesc.CPUAccessFlags     = 0 ;
	Texture3DDesc.MipLevels          = 1 ;
	hr = D3D11Device_CreateTexture3D_ASync( &Texture3DDesc, NULL, &GD3D11.RGBtoVMaxRGBVolumeTexture, FALSE ) ;
	if( FAILED( hr ) )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x52\x00\x47\x00\x42\x00\xab\x30\xe9\x30\xfc\x30\x92\x30\x1d\x8f\xa6\x5e\x92\x30\x00\x67\x27\x59\x6b\x30\x57\x30\x5f\x30\x52\x00\x47\x00\x42\x00\x24\x50\x6b\x30\x09\x59\xdb\x63\x59\x30\x8b\x30\x5f\x30\x81\x30\x6e\x30\xdc\x30\xea\x30\xe5\x30\xfc\x30\xe0\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"RGBカラーを輝度を最大にしたRGB値に変換するためのボリュームテクスチャの作成に失敗しました\n" @*/ ) ;
		goto ERR ;
	}

	// シェーダーリソースビューの作成
	_MEMSET( &SRVDesc, 0, sizeof( SRVDesc ) );
	SRVDesc.Format						= D_DXGI_FORMAT_R8G8B8A8_UNORM ;
	SRVDesc.ViewDimension				= D_D3D11_SRV_DIMENSION_TEXTURE3D ;
	SRVDesc.Texture3D.MipLevels			= 1 ;
	SRVDesc.Texture3D.MostDetailedMip	= 0 ;
	hr = D3D11Device_CreateShaderResourceView_ASync( GD3D11.RGBtoVMaxRGBVolumeTexture, &SRVDesc, &GD3D11.RGBtoVMaxRGBVolumeTextureSRV, FALSE ) ;
	if( FAILED( hr ) )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x52\x00\x47\x00\x42\x00\xab\x30\xe9\x30\xfc\x30\x92\x30\x1d\x8f\xa6\x5e\x92\x30\x00\x67\x27\x59\x6b\x30\x57\x30\x5f\x30\x52\x00\x47\x00\x42\x00\x24\x50\x6b\x30\x09\x59\xdb\x63\x59\x30\x8b\x30\xba\x70\x6e\x30\xdc\x30\xea\x30\xe5\x30\xfc\x30\xe0\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x28\x75\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\xea\x30\xbd\x30\xfc\x30\xb9\x30\xd3\x30\xe5\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"RGBカラーを輝度を最大にしたRGB値に変換する為のボリュームテクスチャ用シェーダーリソースビューの作成に失敗しました" @*/ ) ;
		goto ERR ;
	}

	// データ転送用メモリの確保
	TempBuffer = DXALLOC( RGBTOVMAXRGB_TEX_SIZE * RGBTOVMAXRGB_TEX_SIZE * RGBTOVMAXRGB_TEX_SIZE * 4 ) ;
	if( TempBuffer == NULL )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x52\x00\x47\x00\x42\x00\xab\x30\xe9\x30\xfc\x30\x92\x30\x1d\x8f\xa6\x5e\x92\x30\x00\x67\x27\x59\x6b\x30\x57\x30\x5f\x30\x52\x00\x47\x00\x42\x00\x24\x50\x6b\x30\x09\x59\xdb\x63\x59\x30\x8b\x30\x5f\x30\x81\x30\x6e\x30\xe2\x8e\x01\x90\x43\x51\x28\x75\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"RGBカラーを輝度を最大にしたRGB値に変換するための転送元用メモリの確保に失敗しました\n" @*/ ) ;
		goto ERR ;
	}

	// システムメモリボリュームテクスチャに変換情報を書き込む
	for( b = 0 ; b < RGBTOVMAXRGB_TEX_SIZE ; b ++ )
	{
		for( g = 0 ; g < RGBTOVMAXRGB_TEX_SIZE ; g ++ )
		{
			pDest = ( BYTE * )TempBuffer + ( RGBTOVMAXRGB_TEX_SIZE * RGBTOVMAXRGB_TEX_SIZE * 4 ) * b + ( RGBTOVMAXRGB_TEX_SIZE * 4 ) * g ;
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

	// テクスチャに転送
	DestBox.left   = 0 ;
	DestBox.top    = 0 ;
	DestBox.front  = 0 ;
	DestBox.right  = RGBTOVMAXRGB_TEX_SIZE ;
	DestBox.bottom = RGBTOVMAXRGB_TEX_SIZE ;
	DestBox.back   = RGBTOVMAXRGB_TEX_SIZE ;

	D3D11DeviceContext_UpdateSubresource_ASync(
		GD3D11.RGBtoVMaxRGBVolumeTexture,
		D_D3D11CalcSubresource( 0, 0, 1 ),
		&DestBox,
		TempBuffer,
		( DWORD )( RGBTOVMAXRGB_TEX_SIZE * 4 ),
		( DWORD )( RGBTOVMAXRGB_TEX_SIZE * RGBTOVMAXRGB_TEX_SIZE * 4 ),
		0
	) ;

	// データ転送用のメモリを解放
	DXFREE( TempBuffer ) ;
	TempBuffer = NULL ;

	// 終了
	return 0 ;

ERR :
	if( GD3D11.RGBtoVMaxRGBVolumeTextureSRV )
	{
		Direct3D11_Release_ShaderResourceView( GD3D11.RGBtoVMaxRGBVolumeTextureSRV ) ;
		GD3D11.RGBtoVMaxRGBVolumeTextureSRV = NULL ;
	}

	if( GD3D11.RGBtoVMaxRGBVolumeTexture )
	{
		Direct3D11_Release_Texture3D_ASync( GD3D11.RGBtoVMaxRGBVolumeTexture ) ;
		GD3D11.RGBtoVMaxRGBVolumeTexture = NULL ;
	}

	if( TempBuffer )
	{
		DXFREE( TempBuffer ) ;
		TempBuffer = NULL ;
	}
	
	return -1 ;
}

#endif // DX_NON_MODEL

// Direct3D11 を使用したグラフィックス処理の初期化を行う( 0:成功 -1:失敗 )
extern	int		Graphics_D3D11_Initialize( void )
{
	// Direct3D11.DLL の読み込み
	if( Direct3D11_LoadDLL() < 0 )
	{
		goto ERR ;
	}

	// DXGI.DLL の読み込み
	if( DXGI_LoadDLL() < 0 )
	{
		goto ERR ;
	}

	// D3D11Device を作成する
	if( Graphics_D3D11_Device_Create() != 0 )
	{
		goto ERR ;
	}

	// 標準描画用頂点バッファの作成
	if( Graphics_D3D11_NormalDrawVertexBuffer_Create() != 0 )
	{
		goto ERR ;
	}

	// 深度バッファの作成を試みる
	if( Graphics_D3D11_CreateDepthBuffer() != 0 )
	{
		goto ERR ;
	}

	// シェーダーの作成
	if( Graphics_D3D11_Shader_Initialize() != 0 )
	{
		goto ERR ;
	}

	// 定数情報の初期化
	if( Graphics_D3D11_Constant_Initialize() != 0 )
	{
		goto ERR ;
	}

	// 入力レイアウトの作成
	if( Graphics_D3D11_InputLayout_Base_Create() != 0 )
	{
		goto ERR ;
	}

#ifndef DX_NON_MODEL
	// ボリュームテクスチャの作成
	CreateRGBtoVMaxRGBVolumeTexture() ;
#endif // DX_NON_MODEL

	// フルスクリーンモードの場合はサブバックバッファのセットアップを行う
	if( GetWindowModeFlag() == FALSE )
	{
		if( Graphics_D3D11_SetupSubBackBuffer() != 0 )
		{
			goto ERR ;
		}
	}

	// 単純転送処理の初期化
	if( Graphics_D3D11_StretchRect_Initialize() != 0 )
	{
		goto ERR ;
	}

	GSYS.Setting.ValidHardware = TRUE ;

	// 終了
	return 0 ;

	// エラー終了
ERR :

	// 後始末を行う
	Graphics_D3D11_Terminate() ;

	return -1 ;
}

// Direct3D11 を使用したグラフィックス処理の後始末を行う
extern int Graphics_D3D11_Terminate( void )
{
	int i ;

#ifndef DX_NON_MODEL
	// モデルの頂点バッファを解放
	MV1TerminateVertexBufferAll() ;
#endif // DX_NON_MODEL

	// 単純転送処理の後始末
	Graphics_D3D11_StretchRect_Terminate() ;

	// テクスチャへ画像を転送するためのメモリの後始末を行う
	Graphics_D3D11_Texture_TerminateCommonBuffer() ;

	// サンプラーステートの解放
	Graphics_D3D11_DeviceState_TerminateSamplerState() ;

	// ラスタライザーステートの解放
	Graphics_D3D11_DeviceState_TerminateRasterizerState() ;

	// 深度ステンシルステートの解放
	Graphics_D3D11_DeviceState_TerminateDepthStencilState() ;

	// ブレンドステートの解放
	Graphics_D3D11_DeviceState_TerminateBlendState() ;

	// 汎用描画用頂点バッファとインデックスバッファの解放
	Graphics_D3D11_CommonVertexBuffer_Terminate() ;
	Graphics_D3D11_CommonIndexBuffer_Terminate() ;

	// 標準描画用の頂点バッファの解放
	Graphics_D3D11_NormalDrawVertexBuffer_Terminate() ;

	// 入力レイアウト情報の後始末
	Graphics_D3D11_InputLayout_Base_Terminate() ;
#ifndef DX_NON_MODEL
	Graphics_D3D11_InputLayout_Model_Terminate() ;
#endif // DX_NON_MODEL

	// シェーダーの解放
	Graphics_D3D11_Shader_Terminate() ;

	// 定数情報の後始末
	Graphics_D3D11_Constant_Terminate() ;

	// フルスクリーンモードにしていた場合はここでウインドウモードに戻す
	if( GD3D11.Device.Screen.FullscreenSatte )
	{
		GD3D11.Device.Screen.FullscreenSatte = FALSE ;

		if( GD3D11.Device.Screen.OutputWindowInfo[ 0 ].DXGISwapChain != NULL )
		{
			DXGISwapChain_SetFullscreenState( GD3D11.Device.Screen.OutputWindowInfo[ 0 ].DXGISwapChain, FALSE, NULL ) ;
		}
	}

	// Zバッファ の解放
	if( GD3D11.Device.Screen.DepthBufferDSV )
	{
		Direct3D11_Release_DepthStencilView( GD3D11.Device.Screen.DepthBufferDSV ) ;
		GD3D11.Device.Screen.DepthBufferDSV = NULL ;
	}
	if( GD3D11.Device.Screen.DepthBufferTexture2D )
	{
		Direct3D11_Release_Texture2D( GD3D11.Device.Screen.DepthBufferTexture2D ) ;
		GD3D11.Device.Screen.DepthBufferTexture2D = NULL ;
	}

	// サブバックバッファの解放
	if( GD3D11.Device.Screen.SubBackBufferSRV )
	{
		Direct3D11_Release_ShaderResourceView( GD3D11.Device.Screen.SubBackBufferSRV ) ;
		GD3D11.Device.Screen.SubBackBufferSRV = NULL ;
	}
	if( GD3D11.Device.Screen.SubBackBufferRTV )
	{
		Direct3D11_Release_RenderTargetView( GD3D11.Device.Screen.SubBackBufferRTV ) ;
		GD3D11.Device.Screen.SubBackBufferRTV = NULL ;
	}
	if( GD3D11.Device.Screen.SubBackBufferTexture2D )
	{
		Direct3D11_Release_Texture2D( GD3D11.Device.Screen.SubBackBufferTexture2D ) ;
		GD3D11.Device.Screen.SubBackBufferTexture2D = NULL ;
	}

#ifndef DX_NON_MODEL
	// ボリュームテクスチャの解放
	if( GD3D11.RGBtoVMaxRGBVolumeTextureSRV )
	{
		Direct3D11_Release_ShaderResourceView( GD3D11.RGBtoVMaxRGBVolumeTextureSRV ) ;
		GD3D11.RGBtoVMaxRGBVolumeTextureSRV = NULL ;
	}

	if( GD3D11.RGBtoVMaxRGBVolumeTexture )
	{
		Direct3D11_Release_Texture3D( GD3D11.RGBtoVMaxRGBVolumeTexture ) ;
		GD3D11.RGBtoVMaxRGBVolumeTexture = NULL ;
	}
#endif // DX_NON_MODEL

	// スワップチェインを解放
	for( i = 0 ; i < DX_D3D11_MAX_OUTPUTWINDOW ; i ++ )
	{
		Graphics_D3D11_OutputWindow_Sub( NULL, i ) ;
	}

	// D3D11Device の解放
	D3D11Device_Release() ;
	GD3D11.Device.DrawInfo.BeginSceneFlag = FALSE ;

	// DXGIFactory の解放
	DXGIFactory_Release() ;

	// d3d11.dll の解放
	Direct3D11_FreeDLL() ;

	// dxgi.dll の解放
	DXGI_FreeDLL() ;

	// Direct3D11 の残りオブジェクト数を出力
	Direct3D11_DumpObject() ;

	// デバイスセット情報を初期化
	_MEMSET( GD3D11.Device.State.SamplerState, 0, sizeof( GD3D11.Device.State.SamplerState ) ) ;
	GD3D11.Device.DrawInfo.VertexBufferNextAddr	= NULL ;
	GD3D11.Device.DrawInfo.VertexBufferAddr		= NULL ;
	GD3D11.Device.State.RasterizerState      = NULL ;
	GD3D11.Device.State.DepthStencilState    = NULL ;
	GD3D11.Device.State.BlendState           = NULL ;
	_MEMSET( GD3D11.Device.State.PSSetShaderResourceView, 0, sizeof( GD3D11.Device.State.PSSetShaderResourceView ) ) ;
	GD3D11.Device.State.SetVertexInputLayout = NULL ;
	GD3D11.Device.State.SetVertexShader      = NULL ;
	GD3D11.Device.State.SetPixelShader       = NULL ;
	GD3D11.Device.State.SetIndexBuffer       = NULL ;
	GD3D11.Device.State.SetVertexBuffer      = NULL ;
	_MEMSET( GD3D11.Device.State.TargetTexture2D, 0, sizeof( GD3D11.Device.State.TargetTexture2D ) ) ;
	_MEMSET( GD3D11.Device.State.TargetRTV,       0, sizeof( GD3D11.Device.State.TargetRTV ) ) ;
	GD3D11.Device.State.TargetDepthTexture2D = NULL ;
	GD3D11.Device.State.TargetDepthDSV       = NULL ;
	GD3D11.Device.State.LightEnableMaxIndex  = 0 ;
	_MEMSET( GD3D11.Device.State.LightEnableFlag, 0, sizeof( GD3D11.Device.State.LightEnableFlag ) ) ;
	_MEMSET( GD3D11.Device.State.LightParam,      0, sizeof( GD3D11.Device.State.LightParam ) ) ;
	GD3D11.Device.State.Lighting             = 0 ;
	_MEMSET( &GD3D11.Device.State.GlobalAmbientLightColor, 0, sizeof( GD3D11.Device.State.GlobalAmbientLightColor ) ) ;

	// 終了
	return 0 ;
}

// すべての Direct3D11 系オブジェクトを解放する
extern int Graphics_D3D11_ReleaseObjectAll( void )
{
	int i ;
	SHADERHANDLEDATA                          *Shader ;
	VERTEXBUFFERHANDLEDATA                    *VertexBuffer ;
	INDEXBUFFERHANDLEDATA                     *IndexBuffer ;
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

			Graphics_Hardware_D3D11_ShadowMap_ReleaseTexture_PF( ShadowMap  ) ;
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

			Graphics_D3D11_VertexBuffer_ReleaseObject( VertexBuffer ) ;
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

			Graphics_D3D11_IndexBuffer_ReleaseObject( IndexBuffer ) ;
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
				if( Shader->PF->D3D11.VertexShader )
				{
					Direct3D11_Release_VertexShader( Shader->PF->D3D11.VertexShader ) ;
					Shader->PF->D3D11.VertexShader = NULL ;
				}
			}
			else
			{
				if( Shader->PF->D3D11.PixelShader )
				{
					Direct3D11_Release_PixelShader( Shader->PF->D3D11.PixelShader ) ;
					Shader->PF->D3D11.PixelShader = NULL ;
				}
			}
		}
	}

	// デバイスセット情報を初期化
//	State 系は作成してセットしているものもあるので、初期化処理はしない
//	_MEMSET( GD3D11.Device.State.SamplerState, 0, sizeof( GD3D11.Device.State.SamplerState ) ) ;
//	GD3D11.Device.State.RasterizerState      = NULL ;
//	GD3D11.Device.State.DepthStencilState    = NULL ;
//	GD3D11.Device.State.BlendState           = NULL ;
	_MEMSET( GD3D11.Device.State.PSSetShaderResourceView, 0, sizeof( GD3D11.Device.State.PSSetShaderResourceView ) ) ;
	GD3D11.Device.State.SetVertexInputLayout = NULL ;
	GD3D11.Device.State.SetVertexShader      = NULL ;
	GD3D11.Device.State.SetPixelShader       = NULL ;
	GD3D11.Device.State.SetIndexBuffer       = NULL ;
	GD3D11.Device.State.SetVertexBuffer      = NULL ;
	_MEMSET( GD3D11.Device.State.TargetTexture2D, 0, sizeof( GD3D11.Device.State.TargetTexture2D ) ) ;
	_MEMSET( GD3D11.Device.State.TargetRTV,       0, sizeof( GD3D11.Device.State.TargetRTV ) ) ;
	GD3D11.Device.State.TargetDepthTexture2D = NULL ;
	GD3D11.Device.State.TargetDepthDSV       = NULL ;

	// 終了
	return 0 ;
}

// すべての Direct3D11 系オブジェクトを作成する
extern int Graphics_D3D11_CreateObjectAll( void )
{
	int                                       i ;
	SHADERHANDLEDATA                          *Shader ;
	VERTEXBUFFERHANDLEDATA                    *VertexBuffer ;
	INDEXBUFFERHANDLEDATA                     *IndexBuffer ;
	IMAGEDATA                                 *Image ;
	SHADOWMAPDATA                             *ShadowMap ;

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
			Graphics_Hardware_D3D11_ShadowMap_CreateTexture_PF( ShadowMap ) ;
		}
	}

	if( HandleManageArray[ DX_HANDLETYPE_VERTEX_BUFFER ].InitializeFlag )
	{
		for( i = HandleManageArray[ DX_HANDLETYPE_VERTEX_BUFFER ].AreaMin ; i <= HandleManageArray[ DX_HANDLETYPE_VERTEX_BUFFER ].AreaMax ; i ++ )
		{
			VertexBuffer = ( VERTEXBUFFERHANDLEDATA * )HandleManageArray[ DX_HANDLETYPE_VERTEX_BUFFER ].Handle[ i ] ;
			if( VertexBuffer == NULL ) continue ;
			Graphics_D3D11_VertexBuffer_CreateObject( VertexBuffer, TRUE ) ;
		}
	}

	if( HandleManageArray[ DX_HANDLETYPE_INDEX_BUFFER ].InitializeFlag )
	{
		for( i = HandleManageArray[ DX_HANDLETYPE_INDEX_BUFFER ].AreaMin ; i <= HandleManageArray[ DX_HANDLETYPE_INDEX_BUFFER ].AreaMax ; i ++ )
		{
			IndexBuffer = ( INDEXBUFFERHANDLEDATA * )HandleManageArray[ DX_HANDLETYPE_INDEX_BUFFER ].Handle[ i ] ;
			if( IndexBuffer == NULL ) continue ;
			Graphics_D3D11_IndexBuffer_CreateObject( IndexBuffer, TRUE ) ;
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
				D3D11Device_CreateVertexShader( ( BYTE * )Shader->FunctionCode, ( SIZE_T )Shader->FunctionCodeSize, NULL, &Shader->PF->D3D11.VertexShader ) ;
			}
			else
			{
				D3D11Device_CreatePixelShader(  ( BYTE * )Shader->FunctionCode, ( SIZE_T )Shader->FunctionCodeSize, NULL, &Shader->PF->D3D11.PixelShader ) ;
			}
		}
	}

	// 終了
	return 0 ;
}
















// シェーダーコード関係

// シェーダーコードパッケージからシェーダーコードバイナリのアドレスとサイズを取得する
static void Graphics_D3D11_ShaderCodePackage_GetInfo( WORD **pSize, BYTE **pCode, GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO *DestInfo, int Num )
{
	WORD *Size ;
	BYTE *Code ;
	int i ;

	Size = *pSize ;
	Code = *pCode ;
	for( i = 0 ; i < Num ; i ++, Size ++, DestInfo ++ )
	{
		if( *Size != 0 )
		{
			DestInfo->Binary = ( void * )Code ;
			DestInfo->Size   = *Size ;
			Code            += *Size ;
		}
	}

	*pSize = Size ;
	*pCode = Code ;
}

// Direct3D11 の標準描画用のシェーダーコードの初期化を行う
extern int Graphics_D3D11_ShaderCode_Base_Initialize( void )
{
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_BASE *SCBASE = &GraphicsHardDataDirect3D11.ShaderCode.Base ;
	int   Size = 0 ;
	BYTE  *ShaderAddr = NULL ;
	WORD  *SizeBuf    = NULL ;

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

	SCBASE->Base2DShaderPackageImage = NULL ;

	// 標準描画用シェーダーオブジェクトファイルＤＸＡを圧縮したデータを解凍する
	{
		if( DxShaderCodeBin_Base_D3D11Convert == 0 )
		{
			DxShaderCodeBin_Base_D3D11Convert = 1 ;
			Char128ToBin( DxShaderCodeBin_Base_D3D11, DxShaderCodeBin_Base_D3D11 ) ;
		}
		Size = DXA_Decode( DxShaderCodeBin_Base_D3D11, NULL ) ;
		SCBASE->Base2DShaderPackageImage = DXALLOC( ( size_t )Size ) ;
		if( SCBASE->Base2DShaderPackageImage == NULL )
		{
			goto ERR ;
		}

		DXA_Decode( DxShaderCodeBin_Base_D3D11, SCBASE->Base2DShaderPackageImage ) ;
	}

	// アドレスリストのセット
	SizeBuf    = ( WORD * )SCBASE->Base2DShaderPackageImage ;
	ShaderAddr = ( BYTE * )SCBASE->Base2DShaderPackageImage +
		sizeof( WORD ) * (
			sizeof( SCBASE->BaseSimple_VS_Code               ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) +
			sizeof( SCBASE->BaseNoneTex_PS_Code              ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) +
			sizeof( SCBASE->BaseUseTex_PS_Code               ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) +
			sizeof( SCBASE->MaskEffect_PS_Code               ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) +
			sizeof( SCBASE->MaskEffect_ReverseEffect_PS_Code ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) +
			sizeof( SCBASE->MaskEffect_UseGraphHandle_PS_Code               ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) +
			sizeof( SCBASE->MaskEffect_UseGraphHandle_ReverseEffect_PS_Code ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) +
			sizeof( SCBASE->StretchRect_VS_Code              ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) +
			sizeof( SCBASE->StretchRectTex8_VS_Code          ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) +
			sizeof( SCBASE->StretchRect_PS_Code              ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )SCBASE->BaseSimple_VS_Code,                sizeof( SCBASE->BaseSimple_VS_Code               ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )SCBASE->BaseNoneTex_PS_Code,               sizeof( SCBASE->BaseNoneTex_PS_Code              ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )SCBASE->BaseUseTex_PS_Code,                sizeof( SCBASE->BaseUseTex_PS_Code               ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )&SCBASE->MaskEffect_PS_Code,               sizeof( SCBASE->MaskEffect_PS_Code               ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )&SCBASE->MaskEffect_ReverseEffect_PS_Code, sizeof( SCBASE->MaskEffect_ReverseEffect_PS_Code ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )SCBASE->MaskEffect_UseGraphHandle_PS_Code,               sizeof( SCBASE->MaskEffect_UseGraphHandle_PS_Code               ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )SCBASE->MaskEffect_UseGraphHandle_ReverseEffect_PS_Code, sizeof( SCBASE->MaskEffect_UseGraphHandle_ReverseEffect_PS_Code ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )&SCBASE->StretchRect_VS_Code,              sizeof( SCBASE->StretchRect_VS_Code              ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )&SCBASE->StretchRectTex8_VS_Code,          sizeof( SCBASE->StretchRectTex8_VS_Code          ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )&SCBASE->StretchRect_PS_Code,              sizeof( SCBASE->StretchRect_PS_Code              ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;

#ifndef DX_NON_FILTER

	// RGBAMixシェーダーパック圧縮ファイルを展開する
	{
		if( DxShaderCodeBin_RgbaMix_D3D11Convert == 0 )
		{
			DxShaderCodeBin_RgbaMix_D3D11Convert = 1 ;
			Char128ToBin( DxShaderCodeBin_RgbaMix_D3D11, DxShaderCodeBin_RgbaMix_D3D11 ) ;
		}
		Size = DXA_Decode( DxShaderCodeBin_RgbaMix_D3D11, NULL ) ;
		SCBASE->RGBAMixS_ShaderPackImage = DXALLOC( ( size_t )Size ) ;
		if( SCBASE->RGBAMixS_ShaderPackImage == NULL )
		{
			goto ERR ;
		}

		DXA_Decode( DxShaderCodeBin_RgbaMix_D3D11, SCBASE->RGBAMixS_ShaderPackImage ) ;

		// アドレスリストのセット
		SizeBuf    = ( WORD  * )SCBASE->RGBAMixS_ShaderPackImage ;
		ShaderAddr = ( BYTE  * )SCBASE->RGBAMixS_ShaderPackImage +
			sizeof( WORD ) * (
				sizeof( SCBASE->RGBAMixS_PS_Code )  / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
		Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )SCBASE->RGBAMixS_PS_Code,  sizeof( SCBASE->RGBAMixS_PS_Code )  / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
	}

	// フィルターシェーダーオブジェクトファイルＤＸＡを圧縮したデータを解凍する
	{
		if( DxShaderCodeBin_Filter_D3D11Convert == 0 )
		{
			DxShaderCodeBin_Filter_D3D11Convert = 1 ;
			Char128ToBin( DxShaderCodeBin_Filter_D3D11, DxShaderCodeBin_Filter_D3D11 ) ;
		}
		Size = DXA_Decode( DxShaderCodeBin_Filter_D3D11, NULL ) ;
		SCBASE->FilterShaderBinDxaImage = DXALLOC( ( size_t )Size ) ;
		if( SCBASE->FilterShaderBinDxaImage == NULL )
		{
			goto ERR ;
		}

		DXA_Decode( DxShaderCodeBin_Filter_D3D11, SCBASE->FilterShaderBinDxaImage ) ;

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

ERR :
	if( SCBASE->Base2DShaderPackageImage != NULL )
	{
		DXFREE( SCBASE->Base2DShaderPackageImage ) ;
		SCBASE->Base2DShaderPackageImage = NULL ;
	}

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

// Direct3D11 の標準描画用のシェーダーコードの後始末を行う
extern int Graphics_D3D11_ShaderCode_Base_Terminate( void )
{
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_BASE *SCBASE = &GraphicsHardDataDirect3D11.ShaderCode.Base ;

	// すでに後始末されていたら何もしない
	if( SCBASE->BaseShaderInitializeFlag == FALSE )
	{
		return TRUE ;
	}

	// 解凍したシェーダーを格納していたメモリの解放
	if( SCBASE->Base2DShaderPackageImage != NULL )
	{
		DXFREE( SCBASE->Base2DShaderPackageImage ) ;
		SCBASE->Base2DShaderPackageImage = NULL ;
	}

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

// Direct3D11 の標準３Ｄ描画用のシェーダーコードの初期化を行う
extern int Graphics_D3D11_ShaderCode_Base3D_Initialize( void )
{
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_BASE3D *SCBASE3D = &GraphicsHardDataDirect3D11.ShaderCode.Base3D ;
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
		if( DxShaderCodeBin_Base3D_D3D11Convert == 0 )
		{
			DxShaderCodeBin_Base3D_D3D11Convert = 1 ;
			Char128ToBin( DxShaderCodeBin_Base3D_D3D11, DxShaderCodeBin_Base3D_D3D11 ) ;
		}
		Size = DXA_Decode( DxShaderCodeBin_Base3D_D3D11, NULL ) ;
		SCBASE3D->Base3DShaderPackageImage = DXCALLOC( ( size_t )Size ) ;
		if( SCBASE3D->Base3DShaderPackageImage == NULL )
		{
			goto ERR ;
		}

		DXA_Decode( DxShaderCodeBin_Base3D_D3D11, SCBASE3D->Base3DShaderPackageImage ) ;
	}

	// アドレスリストのセット
	SizeBuf    = ( WORD * )SCBASE3D->Base3DShaderPackageImage ;
	ShaderAddr = ( BYTE * )SCBASE3D->Base3DShaderPackageImage +
		sizeof( WORD ) * (
			sizeof( SCBASE3D->Base3D_PixelLighting_VS_Code         ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) +
			sizeof( SCBASE3D->Base3D_PixelLighting_Normal_PS_Code  ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) +
			sizeof( SCBASE3D->Base3D_ShadowMap_VS_Code             ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) +
			sizeof( SCBASE3D->Base3D_NoLighting_VS_Code            ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) +
			sizeof( SCBASE3D->Base3D_VertexLighting_VS_Code        ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) +
			sizeof( SCBASE3D->Base3D_ShadowMap_Normal_PS_Code      ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) +
			sizeof( SCBASE3D->Base3D_NoLighting_Normal_PS_Code     ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) +
			sizeof( SCBASE3D->Base3D_VertexLighting_Normal_PS_Code ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )SCBASE3D->Base3D_PixelLighting_VS_Code,         sizeof( SCBASE3D->Base3D_PixelLighting_VS_Code         ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )SCBASE3D->Base3D_PixelLighting_Normal_PS_Code,  sizeof( SCBASE3D->Base3D_PixelLighting_Normal_PS_Code  ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )SCBASE3D->Base3D_ShadowMap_VS_Code,             sizeof( SCBASE3D->Base3D_ShadowMap_VS_Code             ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )SCBASE3D->Base3D_NoLighting_VS_Code,            sizeof( SCBASE3D->Base3D_NoLighting_VS_Code            ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )SCBASE3D->Base3D_VertexLighting_VS_Code,        sizeof( SCBASE3D->Base3D_VertexLighting_VS_Code        ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )SCBASE3D->Base3D_ShadowMap_Normal_PS_Code,      sizeof( SCBASE3D->Base3D_ShadowMap_Normal_PS_Code      ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )SCBASE3D->Base3D_NoLighting_Normal_PS_Code,     sizeof( SCBASE3D->Base3D_NoLighting_Normal_PS_Code     ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )SCBASE3D->Base3D_VertexLighting_Normal_PS_Code, sizeof( SCBASE3D->Base3D_VertexLighting_Normal_PS_Code ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;

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


// Direct3D11 の標準３Ｄ描画用のシェーダーコードの後始末を行う
extern int Graphics_D3D11_ShaderCode_Base3D_Terminate( void )
{
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_BASE3D *SCBASE3D = &GraphicsHardDataDirect3D11.ShaderCode.Base3D ;

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

// Direct3D11 のモデル描画用のシェーダーコードの初期化を行う
extern	int		Graphics_D3D11_ShaderCode_Model_Initialize( void )
{
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_MODEL *SCMODEL = &GraphicsHardDataDirect3D11.ShaderCode.Model ;
	HANDLEMANAGE *HandleManage = &HandleManageArray[ DX_HANDLETYPE_GRAPH ] ;
	int   Size = 0 ;
	BYTE  *ShaderAddr = NULL ;
	WORD  *SizeBuf    = NULL ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManage->CriticalSection ) ;

	// すでに初期化されていたら何もしない
	if( SCMODEL->ModelShaderInitializeFlag == TRUE )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManage->CriticalSection ) ;

		return TRUE ;
	}

	SCMODEL->ModelShaderPackImage = NULL ;

	// 圧縮データの解凍
	{
		if( DxShaderCodeBin_Model_D3D11Convert == 0 )
		{
			DxShaderCodeBin_Model_D3D11Convert = 1 ;
			Char128ToBin( DxShaderCodeBin_Model_D3D11, DxShaderCodeBin_Model_D3D11 ) ;
		}
		Size = DXA_Decode( DxShaderCodeBin_Model_D3D11, NULL ) ;
		SCMODEL->ModelShaderPackImage = DXCALLOC( ( size_t )Size ) ;
		if( SCMODEL->ModelShaderPackImage == NULL )
		{
			goto ERR ;
		}

		DXA_Decode( DxShaderCodeBin_Model_D3D11, SCMODEL->ModelShaderPackImage ) ;
	}

	// アドレスリストのセット
	SizeBuf    = ( WORD * )SCMODEL->ModelShaderPackImage ;
	ShaderAddr = ( BYTE * )SCMODEL->ModelShaderPackImage +
		sizeof( WORD ) * (
			sizeof( SCMODEL->MV1_PixelLighting_VS_Code            ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) +
			sizeof( SCMODEL->MV1_PixelLighting_ToonType1_PS_Code  ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) +
			sizeof( SCMODEL->MV1_PixelLighting_ToonType2_PS_Code  ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) +
			sizeof( SCMODEL->MV1_PixelLighting_Normal_PS_Code     ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) +
			sizeof( SCMODEL->MV1_MaterialType_PS_Code             ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) +
			sizeof( SCMODEL->MV1_ToonOutLine_ShadowMap_VS_Code    ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) +
			sizeof( SCMODEL->MV1_ToonOutLine_VS_Code              ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) +
			sizeof( SCMODEL->MV1_ShadowMap_VS_Code                ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) +
			sizeof( SCMODEL->MV1_NoLighting_VS_Code               ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) +
			sizeof( SCMODEL->MV1_VertexLighting_VS_Code           ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) +
			sizeof( SCMODEL->MV1_ShadowMap_Toon_PS_Code           ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) +
			sizeof( SCMODEL->MV1_ShadowMap_Normal_PS_Code         ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) +
			sizeof( SCMODEL->MV1_NoLighting_Toon_PS_Code          ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) +
			sizeof( SCMODEL->MV1_NoLighting_Normal_PS_Code        ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) +
			sizeof( SCMODEL->MV1_VertexLighting_ToonType1_PS_Code ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) +
			sizeof( SCMODEL->MV1_VertexLighting_ToonType2_PS_Code ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) +
			sizeof( SCMODEL->MV1_VertexLighting_Normal_PS_Code    ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )SCMODEL->MV1_PixelLighting_VS_Code,            sizeof( SCMODEL->MV1_PixelLighting_VS_Code            ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )SCMODEL->MV1_PixelLighting_ToonType1_PS_Code,  sizeof( SCMODEL->MV1_PixelLighting_ToonType1_PS_Code  ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )SCMODEL->MV1_PixelLighting_ToonType2_PS_Code,  sizeof( SCMODEL->MV1_PixelLighting_ToonType2_PS_Code  ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )SCMODEL->MV1_PixelLighting_Normal_PS_Code,     sizeof( SCMODEL->MV1_PixelLighting_Normal_PS_Code     ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )SCMODEL->MV1_MaterialType_PS_Code,             sizeof( SCMODEL->MV1_MaterialType_PS_Code             ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )SCMODEL->MV1_ToonOutLine_ShadowMap_VS_Code,    sizeof( SCMODEL->MV1_ToonOutLine_ShadowMap_VS_Code    ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )SCMODEL->MV1_ToonOutLine_VS_Code,              sizeof( SCMODEL->MV1_ToonOutLine_VS_Code              ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )SCMODEL->MV1_ShadowMap_VS_Code,                sizeof( SCMODEL->MV1_ShadowMap_VS_Code                ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )SCMODEL->MV1_NoLighting_VS_Code,               sizeof( SCMODEL->MV1_NoLighting_VS_Code               ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )SCMODEL->MV1_VertexLighting_VS_Code,           sizeof( SCMODEL->MV1_VertexLighting_VS_Code           ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )SCMODEL->MV1_ShadowMap_Toon_PS_Code,           sizeof( SCMODEL->MV1_ShadowMap_Toon_PS_Code           ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )SCMODEL->MV1_ShadowMap_Normal_PS_Code,         sizeof( SCMODEL->MV1_ShadowMap_Normal_PS_Code         ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )SCMODEL->MV1_NoLighting_Toon_PS_Code,          sizeof( SCMODEL->MV1_NoLighting_Toon_PS_Code          ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )SCMODEL->MV1_NoLighting_Normal_PS_Code,        sizeof( SCMODEL->MV1_NoLighting_Normal_PS_Code        ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )SCMODEL->MV1_VertexLighting_ToonType1_PS_Code, sizeof( SCMODEL->MV1_VertexLighting_ToonType1_PS_Code ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )SCMODEL->MV1_VertexLighting_ToonType2_PS_Code, sizeof( SCMODEL->MV1_VertexLighting_ToonType2_PS_Code ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
	Graphics_D3D11_ShaderCodePackage_GetInfo( &SizeBuf, &ShaderAddr, ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )SCMODEL->MV1_VertexLighting_Normal_PS_Code,    sizeof( SCMODEL->MV1_VertexLighting_Normal_PS_Code    ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;

	SCMODEL->ModelShaderInitializeFlag = TRUE ;

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManage->CriticalSection ) ;

	// 正常終了
	return TRUE ;

ERR :
	if( SCMODEL->ModelShaderPackImage != NULL )
	{
		DXFREE( SCMODEL->ModelShaderPackImage ) ;
		SCMODEL->ModelShaderPackImage = NULL ;
	}

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManage->CriticalSection ) ;

	return FALSE ;
}

// Direct3D11 のモデル描画用のシェーダーコードの後始末を行う
extern	int		Graphics_D3D11_ShaderCode_Model_Terminate( void )
{
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_MODEL *SCMODEL = &GraphicsHardDataDirect3D11.ShaderCode.Model ;

	// すでに後始末されていたら何もしない
	if( SCMODEL->ModelShaderInitializeFlag == FALSE )
	{
		return TRUE ;
	}

	if( SCMODEL->ModelShaderPackImage )
	{
		DXFREE( SCMODEL->ModelShaderPackImage ) ;
		SCMODEL->ModelShaderPackImage = NULL ;
	}

	// 初期化フラグを倒す
	SCMODEL->ModelShaderInitializeFlag = FALSE ;

	// 正常終了
	return TRUE ;
}

#endif // DX_NON_MODEL



// シェーダーコードから頂点シェーダーを作成する
extern int Graphics_D3D11_VertexShader_Create( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO *Code, D_ID3D11VertexShader **pDestShader, int Num )
{
	int i ;

	for( i = 0; i < Num; i++, Code++, pDestShader++ )
	{
		if( D3D11Device_CreateVertexShader( ( BYTE * )Code->Binary, ( SIZE_T )Code->Size, NULL, pDestShader ) != S_OK )
		{
			return -1 + i ;
		}
	}

	return 0 ;
}

// シェーダーコードからピクセルシェーダーを作成する
extern int Graphics_D3D11_PixelShader_Create( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO *Code, D_ID3D11PixelShader **pDestShader, int Num )
{
	int i ;

	for( i = 0; i < Num; i++, Code++, pDestShader++ )
	{
		if( D3D11Device_CreatePixelShader( ( BYTE * )Code->Binary, ( SIZE_T )Code->Size, NULL, pDestShader ) != S_OK )
		{
			return -1 + i ;
		}
	}

	return 0 ;
}

// 頂点シェーダー配列を解放する
static void Graphics_D3D11_VertexShaderArray_Release( D_ID3D11VertexShader **pObject, int Num )
{
	int i ;

	for( i = 0 ; i < Num ; i++, pObject++ )
	{
		if( *pObject )
		{
			Direct3D11_Release_VertexShader( *pObject ) ;
			*pObject = NULL ;
		}
	}
}

// ピクセルシェーダー配列を解放する
static void Graphics_D3D11_PixelShaderArray_Release( D_ID3D11PixelShader **pObject, int Num )
{
	int i ;

	for( i = 0 ; i < Num ; i++, pObject++ )
	{
		if( *pObject )
		{
			Direct3D11_Release_PixelShader( *pObject ) ;
			*pObject = NULL ;
		}
	}
}

// ブレンドステート配列を解放する
static void Graphics_D3D11_BlendStateArray_Release( D_ID3D11BlendState **pObject, int Num )
{
	int i ;

	for( i = 0 ; i < Num ; i++, pObject++ )
	{
		if( *pObject )
		{
			Direct3D11_Release_BlendState( *pObject ) ;
			*pObject = NULL ;
		}
	}
}

// デプスステンシルステート配列を解放する
static void Graphics_D3D11_DepthStencilStateArray_Release( D_ID3D11DepthStencilState **pObject, int Num )
{
	int i ;

	for( i = 0 ; i < Num ; i++, pObject++ )
	{
		if( *pObject )
		{
			Direct3D11_Release_DepthStencilState( *pObject ) ;
			*pObject = NULL ;
		}
	}
}

// ラスタライザーステート配列を解放する
static void Graphics_D3D11_RasterizerStateArray_Release( D_ID3D11RasterizerState **pObject, int Num )
{
	int i ;

	for( i = 0 ; i < Num ; i++, pObject++ )
	{
		if( *pObject )
		{
			Direct3D11_Release_RasterizerState( *pObject ) ;
			*pObject = NULL ;
		}
	}
}

// サンプラーステート配列を解放する
static void Graphics_D3D11_SamplerStateArray_Release( D_ID3D11SamplerState **pObject, int Num )
{
	int i ;

	for( i = 0 ; i < Num ; i++, pObject++ )
	{
		if( *pObject )
		{
			Direct3D11_Release_SamplerState( *pObject ) ;
			*pObject = NULL ;
		}
	}
}

// Direct3D11 のシェーダーの初期化を行う
extern int Graphics_D3D11_Shader_Initialize( void )
{
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE      *ShaderCode = &GraphicsHardDataDirect3D11.ShaderCode ;
	GRAPHICS_HARDWARE_DIRECT3D11_SHADER          *Shader     = &GraphicsHardDataDirect3D11.Device.Shader ;
	int Result ;

	// 既に作成されていたときのために削除処理を行う
	//TerminateDirect3D11Shader() ;
	Graphics_D3D11_Shader_Terminate() ;

//	return 0 ;

	DXST_LOGFILE_ADDUTF16LE( "\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\xb3\x30\xfc\x30\xc9\x30\xa2\x95\xc2\x4f\x6e\x30\x1d\x52\x1f\x67\x16\x53\x2e\x00\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"シェーダーコード関係の初期化.... " @*/ ) ;

	// Direct3D11 の場合はシェーダーは必ず使用できる
	GSYS.HardInfo.UseShader = TRUE ;

	// 標準描画用シェーダーコードが展開されていない場合は展開
	if( Graphics_D3D11_ShaderCode_Base_Initialize() == FALSE )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\xb3\x30\xfc\x30\xc9\x30\x92\x30\x55\x5c\x8b\x95\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"シェーダーコードを展開するメモリ領域の確保に失敗しました\n" @*/ )) ;
		GSYS.HardInfo.UseShader = FALSE ;
		goto ERR ;
	}

	// 基本的な描画処理に使用するシェーダーを作成する
	{
		// 標準描画用の複雑な処理を行わない頂点シェーダーの作成
		Result = Graphics_D3D11_VertexShader_Create( ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )ShaderCode->Base.BaseSimple_VS_Code, ( D_ID3D11VertexShader ** )Shader->Base.BaseSimple_VS, sizeof( ShaderCode->Base.BaseSimple_VS_Code ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
		if( Result < 0 )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x19\x6a\x96\x6e\xcf\x63\x3b\x75\x28\x75\x6e\x30\x07\x89\xd1\x96\x6a\x30\xe6\x51\x06\x74\x92\x30\x4c\x88\x8f\x30\x6a\x30\x44\x30\x02\x98\xb9\x70\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x20\x00\x4e\x00\x6f\x00\x2e\x00\x25\x00\x64\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"標準描画用の複雑な処理を行わない頂点シェーダーオブジェクト No.%d の作成に失敗しました\n" @*/, -( Result + 1 ) ) ) ;
			GSYS.HardInfo.UseShader = FALSE ;
			goto ERR ;
		}

		// 固定機能パイプライン互換のピクセルシェーダー( テクスチャなし )の作成
		Result = Graphics_D3D11_PixelShader_Create( ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )ShaderCode->Base.BaseNoneTex_PS_Code, ( D_ID3D11PixelShader ** )Shader->Base.BaseNoneTex_PS, sizeof( ShaderCode->Base.BaseNoneTex_PS_Code ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
		if( Result < 0 )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xfa\x56\x9a\x5b\x5f\x6a\xfd\x80\xd1\x30\xa4\x30\xd7\x30\xe9\x30\xa4\x30\xf3\x30\x92\x4e\xdb\x63\x6e\x30\xd4\x30\xaf\x30\xbb\x30\xeb\x30\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x28\x00\x20\x00\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6a\x30\x57\x30\x20\x00\x29\x00\x20\x00\x4e\x00\x6f\x00\x2e\x00\x25\x00\x64\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"固定機能パイプライン互換のピクセルシェーダーオブジェクト( テクスチャなし ) No.%d の作成に失敗しました\n" @*/, -( Result + 1 ) ) ) ;
			GSYS.HardInfo.UseShader = FALSE ;
			goto ERR ;
		}

		// 固定機能パイプライン互換のピクセルシェーダー( テクスチャあり )の作成
		Result = Graphics_D3D11_PixelShader_Create( ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )ShaderCode->Base.BaseUseTex_PS_Code, ( D_ID3D11PixelShader ** )Shader->Base.BaseUseTex_PS, sizeof( ShaderCode->Base.BaseUseTex_PS_Code ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
		if( Result < 0 )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xfa\x56\x9a\x5b\x5f\x6a\xfd\x80\xd1\x30\xa4\x30\xd7\x30\xe9\x30\xa4\x30\xf3\x30\x92\x4e\xdb\x63\x6e\x30\xd4\x30\xaf\x30\xbb\x30\xeb\x30\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x28\x00\x20\x00\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x42\x30\x8a\x30\x20\x00\x29\x00\x20\x00\x4e\x00\x6f\x00\x2e\x00\x25\x00\x64\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"固定機能パイプライン互換のピクセルシェーダーオブジェクト( テクスチャあり ) No.%d の作成に失敗しました\n" @*/, -( Result + 1 ) ) ) ;
			GSYS.HardInfo.UseShader = FALSE ;
			goto ERR ;
		}

		// マスク処理用ピクセルシェーダーの作成
		Result = Graphics_D3D11_PixelShader_Create( ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )&ShaderCode->Base.MaskEffect_PS_Code, ( D_ID3D11PixelShader ** )&Shader->Base.MaskEffect_PS, sizeof( ShaderCode->Base.MaskEffect_PS_Code ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
		if( Result < 0 )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xde\x30\xb9\x30\xaf\x30\xe6\x51\x06\x74\x28\x75\x6e\x30\xd4\x30\xaf\x30\xbb\x30\xeb\x30\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x20\x00\x4e\x00\x6f\x00\x2e\x00\x25\x00\x64\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"マスク処理用のピクセルシェーダーオブジェクト No.%d の作成に失敗しました\n" @*/, -( Result + 1 ) ) ) ;
			GSYS.HardInfo.UseShader = FALSE ;
			goto ERR ;
		}
		Result = Graphics_D3D11_PixelShader_Create( ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )&ShaderCode->Base.MaskEffect_ReverseEffect_PS_Code, ( D_ID3D11PixelShader ** )&Shader->Base.MaskEffect_ReverseEffect_PS, sizeof( ShaderCode->Base.MaskEffect_ReverseEffect_PS_Code ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
		if( Result < 0 )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xcd\x53\xe2\x8e\xb9\x52\x9c\x67\xde\x30\xb9\x30\xaf\x30\xe6\x51\x06\x74\x28\x75\x6e\x30\xd4\x30\xaf\x30\xbb\x30\xeb\x30\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x20\x00\x4e\x00\x6f\x00\x2e\x00\x25\x00\x64\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"反転効果マスク処理用のピクセルシェーダーオブジェクト No.%d の作成に失敗しました\n" @*/, -( Result + 1 ) ) ) ;
			GSYS.HardInfo.UseShader = FALSE ;
			goto ERR ;
		}
		Result = Graphics_D3D11_PixelShader_Create( ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )ShaderCode->Base.MaskEffect_UseGraphHandle_PS_Code, ( D_ID3D11PixelShader ** )Shader->Base.MaskEffect_UseGraphHandle_PS, sizeof( ShaderCode->Base.MaskEffect_UseGraphHandle_PS_Code ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
		if( Result < 0 )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xde\x30\xb9\x30\xaf\x30\xe6\x51\x06\x74\x28\x75\x6e\x30\xd4\x30\xaf\x30\xbb\x30\xeb\x30\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x20\x00\x4e\x00\x6f\x00\x2e\x00\x25\x00\x64\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"マスク処理用のピクセルシェーダーオブジェクト No.%d の作成に失敗しました\n" @*/, -( Result + 1 ) ) ) ;
			GSYS.HardInfo.UseShader = FALSE ;
			goto ERR ;
		}
		Result = Graphics_D3D11_PixelShader_Create( ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )ShaderCode->Base.MaskEffect_UseGraphHandle_ReverseEffect_PS_Code, ( D_ID3D11PixelShader ** )Shader->Base.MaskEffect_UseGraphHandle_ReverseEffect_PS, sizeof( ShaderCode->Base.MaskEffect_UseGraphHandle_ReverseEffect_PS_Code ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
		if( Result < 0 )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xcd\x53\xe2\x8e\xb9\x52\x9c\x67\xde\x30\xb9\x30\xaf\x30\xe6\x51\x06\x74\x28\x75\x6e\x30\xd4\x30\xaf\x30\xbb\x30\xeb\x30\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x20\x00\x4e\x00\x6f\x00\x2e\x00\x25\x00\x64\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"反転効果マスク処理用のピクセルシェーダーオブジェクト No.%d の作成に失敗しました\n" @*/, -( Result + 1 ) ) ) ;
			GSYS.HardInfo.UseShader = FALSE ;
			goto ERR ;
		}

		// 単純転送用シェーダーの作成
		Result = Graphics_D3D11_VertexShader_Create( ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )&ShaderCode->Base.StretchRect_VS_Code, ( D_ID3D11VertexShader ** )&Shader->Base.StretchRect_VS, sizeof( ShaderCode->Base.StretchRect_VS_Code ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
		if( Result < 0 )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x58\x53\x14\x7d\xe2\x8e\x01\x90\x28\x75\x6e\x30\x02\x98\xb9\x70\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x20\x00\x4e\x00\x6f\x00\x2e\x00\x25\x00\x64\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"単純転送用の頂点シェーダーオブジェクト No.%d の作成に失敗しました\n" @*/, -( Result + 1 ) ) ) ;
			GSYS.HardInfo.UseShader = FALSE ;
			goto ERR ;
		}
		Result = Graphics_D3D11_VertexShader_Create( ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )&ShaderCode->Base.StretchRectTex8_VS_Code, ( D_ID3D11VertexShader ** )&Shader->Base.StretchRectTex8_VS, sizeof( ShaderCode->Base.StretchRectTex8_VS_Code ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
		if( Result < 0 )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\xa7\x5e\x19\x6a\x38\x00\x0b\x50\x6e\x30\x58\x53\x14\x7d\xe2\x8e\x01\x90\x28\x75\x6e\x30\x02\x98\xb9\x70\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x20\x00\x4e\x00\x6f\x00\x2e\x00\x25\x00\x64\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"テクスチャ座標8個の単純転送用の頂点シェーダーオブジェクト No.%d の作成に失敗しました\n" @*/, -( Result + 1 ) ) ) ;
			GSYS.HardInfo.UseShader = FALSE ;
			goto ERR ;
		}
		Result = Graphics_D3D11_PixelShader_Create( ( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO * )&ShaderCode->Base.StretchRect_PS_Code, ( D_ID3D11PixelShader ** )&Shader->Base.StretchRect_PS, sizeof( ShaderCode->Base.StretchRect_PS_Code ) / sizeof( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO ) ) ;
		if( Result < 0 )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x58\x53\x14\x7d\xe2\x8e\x01\x90\x28\x75\x6e\x30\xd4\x30\xaf\x30\xbb\x30\xeb\x30\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x20\x00\x4e\x00\x6f\x00\x2e\x00\x25\x00\x64\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"単純転送用のピクセルシェーダーオブジェクト No.%d の作成に失敗しました\n" @*/, -( Result + 1 ) ) ) ;
			GSYS.HardInfo.UseShader = FALSE ;
			goto ERR ;
		}
	}

	DXST_LOGFILE_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"成功\n" @*/ ) ;

	// 正常終了
	return 0 ;

ERR :

	// エラー終了
	return -1 ;
}


// Direct3D11 のシェーダーの後始末をする
extern int Graphics_D3D11_Shader_Terminate( void )
{
	GRAPHICS_HARDDATA_DIRECT3D11_DEVICE *Device = &GraphicsHardDataDirect3D11.Device ;
	GRAPHICS_HARDWARE_DIRECT3D11_SHADER *Shader = &Device->Shader ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	Graphics_D3D11_DeviceState_ResetVertexShader() ;
	Graphics_D3D11_DeviceState_ResetPixelShader() ;

	Graphics_D3D11_VertexShaderArray_Release( ( D_ID3D11VertexShader ** )Shader->Base.BaseSimple_VS,               sizeof( Shader->Base.BaseSimple_VS               ) / sizeof( D_ID3D11VertexShader * ) ) ;
	Graphics_D3D11_PixelShaderArray_Release(  ( D_ID3D11PixelShader ** )Shader->Base.BaseNoneTex_PS,               sizeof( Shader->Base.BaseNoneTex_PS              ) / sizeof( D_ID3D11PixelShader  * ) ) ;
	Graphics_D3D11_PixelShaderArray_Release(  ( D_ID3D11PixelShader ** )Shader->Base.BaseUseTex_PS,                sizeof( Shader->Base.BaseUseTex_PS               ) / sizeof( D_ID3D11PixelShader  * ) ) ;
	Graphics_D3D11_PixelShaderArray_Release(  ( D_ID3D11PixelShader ** )&Shader->Base.MaskEffect_PS,               sizeof( Shader->Base.MaskEffect_PS               ) / sizeof( D_ID3D11PixelShader  * ) ) ;
	Graphics_D3D11_PixelShaderArray_Release(  ( D_ID3D11PixelShader ** )&Shader->Base.MaskEffect_ReverseEffect_PS, sizeof( Shader->Base.MaskEffect_ReverseEffect_PS ) / sizeof( D_ID3D11PixelShader  * ) ) ;
	Graphics_D3D11_PixelShaderArray_Release(  ( D_ID3D11PixelShader ** )Shader->Base.MaskEffect_UseGraphHandle_PS,               sizeof( Shader->Base.MaskEffect_UseGraphHandle_PS               ) / sizeof( D_ID3D11PixelShader  * ) ) ;
	Graphics_D3D11_PixelShaderArray_Release(  ( D_ID3D11PixelShader ** )Shader->Base.MaskEffect_UseGraphHandle_ReverseEffect_PS, sizeof( Shader->Base.MaskEffect_UseGraphHandle_ReverseEffect_PS ) / sizeof( D_ID3D11PixelShader  * ) ) ;
	Graphics_D3D11_VertexShaderArray_Release( ( D_ID3D11VertexShader ** )&Shader->Base.StretchRect_VS,             sizeof( Shader->Base.StretchRect_VS              ) / sizeof( D_ID3D11VertexShader * ) ) ;
	Graphics_D3D11_VertexShaderArray_Release( ( D_ID3D11VertexShader ** )&Shader->Base.StretchRectTex8_VS,         sizeof( Shader->Base.StretchRectTex8_VS          ) / sizeof( D_ID3D11VertexShader * ) ) ;
	Graphics_D3D11_PixelShaderArray_Release(  ( D_ID3D11PixelShader ** )&Shader->Base.StretchRect_PS,              sizeof( Shader->Base.StretchRect_PS              ) / sizeof( D_ID3D11PixelShader  * ) ) ;

	Graphics_D3D11_VertexShaderArray_Release( ( D_ID3D11VertexShader ** )&Shader->Base3D.Base3D_PixelLighting_VS,        sizeof( Shader->Base3D.Base3D_PixelLighting_VS         ) / sizeof( D_ID3D11VertexShader * ) ) ;
	Graphics_D3D11_PixelShaderArray_Release(  ( D_ID3D11PixelShader ** )&Shader->Base3D.Base3D_PixelLighting_Normal_PS,  sizeof( Shader->Base3D.Base3D_PixelLighting_Normal_PS  ) / sizeof( D_ID3D11PixelShader  * ) ) ;
	Graphics_D3D11_VertexShaderArray_Release( ( D_ID3D11VertexShader ** )&Shader->Base3D.Base3D_ShadowMap_VS,            sizeof( Shader->Base3D.Base3D_ShadowMap_VS             ) / sizeof( D_ID3D11VertexShader * ) ) ;
	Graphics_D3D11_VertexShaderArray_Release( ( D_ID3D11VertexShader ** )&Shader->Base3D.Base3D_NoLighting_VS,           sizeof( Shader->Base3D.Base3D_NoLighting_VS            ) / sizeof( D_ID3D11VertexShader * ) ) ;
	Graphics_D3D11_VertexShaderArray_Release( ( D_ID3D11VertexShader ** )&Shader->Base3D.Base3D_VertexLighting_VS,       sizeof( Shader->Base3D.Base3D_VertexLighting_VS        ) / sizeof( D_ID3D11VertexShader * ) ) ;
	Graphics_D3D11_PixelShaderArray_Release(  ( D_ID3D11PixelShader ** )&Shader->Base3D.Base3D_ShadowMap_Normal_PS,      sizeof( Shader->Base3D.Base3D_ShadowMap_Normal_PS      ) / sizeof( D_ID3D11PixelShader  * ) ) ;
	Graphics_D3D11_PixelShaderArray_Release(  ( D_ID3D11PixelShader ** )&Shader->Base3D.Base3D_NoLighting_Normal_PS,     sizeof( Shader->Base3D.Base3D_NoLighting_Normal_PS     ) / sizeof( D_ID3D11PixelShader  * ) ) ;
	Graphics_D3D11_PixelShaderArray_Release(  ( D_ID3D11PixelShader ** )&Shader->Base3D.Base3D_VertexLighting_Normal_PS, sizeof( Shader->Base3D.Base3D_VertexLighting_Normal_PS ) / sizeof( D_ID3D11PixelShader  * ) ) ;

#ifndef DX_NON_MODEL

	Graphics_D3D11_VertexShaderArray_Release( ( D_ID3D11VertexShader ** )&Shader->Model.MV1_PixelLighting_VS,           sizeof( Shader->Model.MV1_PixelLighting_VS            ) / sizeof( D_ID3D11VertexShader * ) ) ;
	Graphics_D3D11_PixelShaderArray_Release(  ( D_ID3D11PixelShader ** )&Shader->Model.MV1_PixelLighting_ToonType1_PS,  sizeof( Shader->Model.MV1_PixelLighting_ToonType1_PS  ) / sizeof( D_ID3D11PixelShader  * ) ) ;
	Graphics_D3D11_PixelShaderArray_Release(  ( D_ID3D11PixelShader ** )&Shader->Model.MV1_PixelLighting_ToonType2_PS,  sizeof( Shader->Model.MV1_PixelLighting_ToonType2_PS  ) / sizeof( D_ID3D11PixelShader  * ) ) ;
	Graphics_D3D11_PixelShaderArray_Release(  ( D_ID3D11PixelShader ** )&Shader->Model.MV1_PixelLighting_Normal_PS,     sizeof( Shader->Model.MV1_PixelLighting_Normal_PS     ) / sizeof( D_ID3D11PixelShader  * ) ) ;
	Graphics_D3D11_PixelShaderArray_Release(  ( D_ID3D11PixelShader ** )&Shader->Model.MV1_MaterialType_PS,             sizeof( Shader->Model.MV1_MaterialType_PS             ) / sizeof( D_ID3D11PixelShader  * ) ) ;
	Graphics_D3D11_VertexShaderArray_Release( ( D_ID3D11VertexShader ** )&Shader->Model.MV1_ToonOutLine_ShadowMap_VS,   sizeof( Shader->Model.MV1_ToonOutLine_ShadowMap_VS    ) / sizeof( D_ID3D11VertexShader * ) ) ;
	Graphics_D3D11_VertexShaderArray_Release( ( D_ID3D11VertexShader ** )&Shader->Model.MV1_ToonOutLine_VS,             sizeof( Shader->Model.MV1_ToonOutLine_VS              ) / sizeof( D_ID3D11VertexShader * ) ) ;
	Graphics_D3D11_VertexShaderArray_Release( ( D_ID3D11VertexShader ** )&Shader->Model.MV1_ShadowMap_VS,               sizeof( Shader->Model.MV1_ShadowMap_VS                ) / sizeof( D_ID3D11VertexShader * ) ) ;
	Graphics_D3D11_VertexShaderArray_Release( ( D_ID3D11VertexShader ** )&Shader->Model.MV1_NoLighting_VS,              sizeof( Shader->Model.MV1_NoLighting_VS               ) / sizeof( D_ID3D11VertexShader * ) ) ;
	Graphics_D3D11_VertexShaderArray_Release( ( D_ID3D11VertexShader ** )&Shader->Model.MV1_VertexLighting_VS,          sizeof( Shader->Model.MV1_VertexLighting_VS           ) / sizeof( D_ID3D11VertexShader * ) ) ;
	Graphics_D3D11_PixelShaderArray_Release(  ( D_ID3D11PixelShader ** )&Shader->Model.MV1_ShadowMap_Toon_PS,           sizeof( Shader->Model.MV1_ShadowMap_Toon_PS           ) / sizeof( D_ID3D11PixelShader  * ) ) ;
	Graphics_D3D11_PixelShaderArray_Release(  ( D_ID3D11PixelShader ** )&Shader->Model.MV1_ShadowMap_Normal_PS,         sizeof( Shader->Model.MV1_ShadowMap_Normal_PS         ) / sizeof( D_ID3D11PixelShader  * ) ) ;
	Graphics_D3D11_PixelShaderArray_Release(  ( D_ID3D11PixelShader ** )&Shader->Model.MV1_NoLighting_Toon_PS,          sizeof( Shader->Model.MV1_NoLighting_Toon_PS          ) / sizeof( D_ID3D11PixelShader  * ) ) ;
	Graphics_D3D11_PixelShaderArray_Release(  ( D_ID3D11PixelShader ** )&Shader->Model.MV1_NoLighting_Normal_PS,        sizeof( Shader->Model.MV1_NoLighting_Normal_PS        ) / sizeof( D_ID3D11PixelShader  * ) ) ;
	Graphics_D3D11_PixelShaderArray_Release(  ( D_ID3D11PixelShader ** )&Shader->Model.MV1_VertexLighting_ToonType1_PS, sizeof( Shader->Model.MV1_VertexLighting_ToonType1_PS ) / sizeof( D_ID3D11PixelShader  * ) ) ;
	Graphics_D3D11_PixelShaderArray_Release(  ( D_ID3D11PixelShader ** )&Shader->Model.MV1_VertexLighting_ToonType2_PS, sizeof( Shader->Model.MV1_VertexLighting_ToonType2_PS ) / sizeof( D_ID3D11PixelShader  * ) ) ;
	Graphics_D3D11_PixelShaderArray_Release(  ( D_ID3D11PixelShader ** )&Shader->Model.MV1_VertexLighting_Normal_PS,    sizeof( Shader->Model.MV1_VertexLighting_Normal_PS    ) / sizeof( D_ID3D11PixelShader  * ) ) ;

#endif // DX_NON_MODEL

	// 正常終了
	return 0 ;
}

// ３Ｄ標準描画の指定の頂点用の描画用シェーダーをセットアップする
extern int Graphics_D3D11_Shader_Normal3DDraw_Setup( void )
{
	GRAPHICS_HARDWARE_DIRECT3D11_SHADER_BASE3D		*SB3D  = &GD3D11.Device.Shader.Base3D ;
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_BASE3D	*SCB3D = &GD3D11.ShaderCode.Base3D ;
	int												ValidPL ;
	D_ID3D11VertexShader							**VS_PL       = NULL ;
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO	*VSAddress_PL = NULL ;
	D_ID3D11PixelShader								**PS_PL       = NULL ;
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO	*PSAddress_PL = NULL ;
	D_ID3D11VertexShader							**VS          = NULL ;
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO	*VSAddress    = NULL ;
	D_ID3D11PixelShader								**PS          = NULL ;
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO	*PSAddress    = NULL ;
	int												ShadowMap ;
	int												IgnoreTextureAlpha ;
	int												FogType ;
	int												LightIndex84 ;
	int												LightIndex20 ;
	int												LightIndex10 ;
	int												AlphaTestMode ;
	int												BumpMap ;
	int												RenderBlendType ;
	int												NextBlendMode ;

	if( GD3D11.ShaderCode.Base3D.Base3DShaderInitializeFlag == FALSE &&
		Graphics_D3D11_ShaderCode_Base3D_Initialize() == FALSE )
	{
		return FALSE ;
	}

	// テクスチャアルファを無視するかどうかをセット
	IgnoreTextureAlpha = 1 ;
	if( GD3D11.Device.DrawSetting.AlphaChannelValidFlag ||
		GD3D11.Device.DrawSetting.AlphaTestValidFlag    ||
		GD3D11.Device.State.DepthStencilDesc.DepthEnable )
	{
		IgnoreTextureAlpha = 0 ;
	}
	if( IgnoreTextureAlpha != GD3D11.Device.DrawSetting.IgnoreGraphAlphaFlag )
	{
		Graphics_D3D11_DrawSetting_SetIgnoreDrawGraphAlpha( IgnoreTextureAlpha ) ;
	}

	// ブレンドモードの決定
	{
		NextBlendMode = GD3D11.Device.DrawSetting.BlendMode ;
		switch( GD3D11.Device.DrawSetting.BlendMode )
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
			if( GD3D11.Device.DrawSetting.RenderTexture != NULL )
			{
				if( GD3D11.Device.DrawSetting.AlphaChannelValidFlag == TRUE )
				{
					NextBlendMode = DX_BLENDMODE_ALPHA ;
				}
			}
			break ;
		}
	}

	ShadowMap       = GSYS.DrawSetting.UseShadowMapNum != 0 ? 1 : 0 ;
	FogType         = GD3D11.Device.State.FogEnable ? GD3D11.Device.State.FogMode : DX_FOGMODE_NONE ;
	RenderBlendType = g_DefaultBlendDescArray[ NextBlendMode ].RenderBlendType ;
	AlphaTestMode   = GD3D11.Device.State.AlphaTestModeShaderIndex ;
	BumpMap         = 0 ;

	if( GSYS.DrawSetting.ShadowMapDraw )
	{
		// シャドウマップへの描画

		VS           =  &SB3D->Base3D_ShadowMap_VS     [ BumpMap ] ;
		VSAddress    = &SCB3D->Base3D_ShadowMap_VS_Code[ BumpMap ] ;

		PS           =  &SB3D->Base3D_ShadowMap_Normal_PS     [ AlphaTestMode ] ;
		PSAddress    = &SCB3D->Base3D_ShadowMap_Normal_PS_Code[ AlphaTestMode ] ;
	}
	else
	if( GD3D11.Device.State.Lighting )
	{
		// ライティングあり描画

		int LightMode[ DX_PIXELLIGHTING_LIGHT_NUM ] ;
		int i ;

		for( i = 0 ; i < DX_PIXELLIGHTING_LIGHT_NUM ; i ++ )
		{
			LightMode[ i ] = GD3D11.Device.State.LightEnableFlag[ i ] ? GD3D11.Device.State.LightParam[ i ].LightType : 0 ;
		}

		LightIndex84 = GD3D11.ShaderCode.Base.LightIndexList84[ LightMode[ 0 ] ][ LightMode[ 1 ] ][ LightMode[ 2 ] ][ LightMode[ 3 ] ][ LightMode[ 4 ] ][ LightMode[ 5 ] ] ;
		LightIndex20 = GD3D11.ShaderCode.Base.LightIndexList20[ LightMode[ 0 ] ][ LightMode[ 1 ] ][ LightMode[ 2 ] ] ;
		LightIndex10 = GD3D11.ShaderCode.Base.LightIndexList10
				[ LightMode[ 0 ] == DX_LIGHTTYPE_D3DLIGHT_DIRECTIONAL ? 2 : ( LightMode[ 0 ] ? 1 : 0 ) ]
				[ LightMode[ 1 ] == DX_LIGHTTYPE_D3DLIGHT_DIRECTIONAL ? 2 : ( LightMode[ 1 ] ? 1 : 0 ) ]
				[ LightMode[ 2 ] == DX_LIGHTTYPE_D3DLIGHT_DIRECTIONAL ? 2 : ( LightMode[ 2 ] ? 1 : 0 ) ] ;

		VS_PL        =  &SB3D->Base3D_PixelLighting_VS     [ ShadowMap ][ FogType ] ;
		VSAddress_PL = &SCB3D->Base3D_PixelLighting_VS_Code[ ShadowMap ][ FogType ] ;

		PS_PL        =  &SB3D->Base3D_PixelLighting_Normal_PS     [ ShadowMap ][ LightIndex84 ][ RenderBlendType ][ AlphaTestMode ] ;
		PSAddress_PL = &SCB3D->Base3D_PixelLighting_Normal_PS_Code[ ShadowMap ][ LightIndex84 ][ RenderBlendType ][ AlphaTestMode ] ;

		VS           =  &SB3D->Base3D_VertexLighting_VS     [ ShadowMap ][ FogType ][ LightIndex20 ] ;
		VSAddress    = &SCB3D->Base3D_VertexLighting_VS_Code[ ShadowMap ][ FogType ][ LightIndex20 ] ;

		PS           =  &SB3D->Base3D_VertexLighting_Normal_PS     [ ShadowMap ][ LightIndex10 ][ RenderBlendType ][ AlphaTestMode ] ;
		PSAddress    = &SCB3D->Base3D_VertexLighting_Normal_PS_Code[ ShadowMap ][ LightIndex10 ][ RenderBlendType ][ AlphaTestMode ] ;
	}
	else
	{
		// ライティングなし描画

		VS           =  &SB3D->Base3D_NoLighting_VS     [ FogType ] ;
		VSAddress    = &SCB3D->Base3D_NoLighting_VS_Code[ FogType ] ;

		PS           =  &SB3D->Base3D_NoLighting_Normal_PS     [ RenderBlendType ][ AlphaTestMode ] ;
		PSAddress    = &SCB3D->Base3D_NoLighting_Normal_PS_Code[ RenderBlendType ][ AlphaTestMode ] ;
	}

	ValidPL = 
		VSAddress_PL != NULL && VSAddress_PL->Binary != NULL &&
		PSAddress_PL != NULL && PSAddress_PL->Binary != NULL ;

	if( ( GSYS.Light.EnableNum > DX_VERTEXLIGHTING_LIGHT_NUM || GD3D11.UsePixelLightingShader ) && ValidPL )
	{
		// シェーダーがあるかどうかを調べる
		if( *VS_PL == NULL )
		{
			// シェーダーの作成を試みる
			if( Graphics_D3D11_VertexShader_Create( VSAddress_PL, VS_PL, 1 ) != 0 )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x47\x00\x72\x00\x61\x00\x70\x00\x68\x00\x69\x00\x63\x00\x73\x00\x5f\x00\x44\x00\x33\x00\x44\x00\x31\x00\x31\x00\x5f\x00\x53\x00\x68\x00\x61\x00\x64\x00\x65\x00\x72\x00\x5f\x00\x4e\x00\x6f\x00\x72\x00\x6d\x00\x61\x00\x6c\x00\x33\x00\x44\x00\x44\x00\x72\x00\x61\x00\x77\x00\x5f\x00\x53\x00\x65\x00\x74\x00\x75\x00\x70\x00\x20\x00\x67\x30\x02\x98\xb9\x70\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Graphics_D3D11_Shader_Normal3DDraw_Setup で頂点シェーダーの作成に失敗しました\n" @*/ )) ;
				return FALSE ;
			}
		}

		// バーテックスシェーダーのセットアップ
		Graphics_D3D11_DeviceState_SetVertexShader( *VS_PL, FALSE ) ;


		// シェーダーがあるかどうかを調べる
		if( *PS_PL == NULL )
		{
			// シェーダーの作成を試みる
			if( Graphics_D3D11_PixelShader_Create( PSAddress_PL, PS_PL, 1 ) != 0 )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x47\x00\x72\x00\x61\x00\x70\x00\x68\x00\x69\x00\x63\x00\x73\x00\x5f\x00\x44\x00\x33\x00\x44\x00\x31\x00\x31\x00\x5f\x00\x53\x00\x68\x00\x61\x00\x64\x00\x65\x00\x72\x00\x5f\x00\x4e\x00\x6f\x00\x72\x00\x6d\x00\x61\x00\x6c\x00\x33\x00\x44\x00\x44\x00\x72\x00\x61\x00\x77\x00\x5f\x00\x53\x00\x65\x00\x74\x00\x75\x00\x70\x00\x20\x00\x67\x30\xd4\x30\xaf\x30\xbb\x30\xeb\x30\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Graphics_D3D11_Shader_Normal3DDraw_Setup でピクセルシェーダーの作成に失敗しました\n" @*/ )) ;
				return FALSE ;
			}
		}

		// ピクセルシェーダーのセットアップ
		Graphics_D3D11_DeviceState_SetPixelShader( *PS_PL, FALSE ) ;
	}
	else
	{
		// シェーダーがあるかどうかを調べる
		if( *VS == NULL )
		{
			// シェーダーの作成を試みる
			if( Graphics_D3D11_VertexShader_Create( VSAddress, VS, 1 ) != 0 )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x47\x00\x72\x00\x61\x00\x70\x00\x68\x00\x69\x00\x63\x00\x73\x00\x5f\x00\x44\x00\x33\x00\x44\x00\x31\x00\x31\x00\x5f\x00\x53\x00\x68\x00\x61\x00\x64\x00\x65\x00\x72\x00\x5f\x00\x4e\x00\x6f\x00\x72\x00\x6d\x00\x61\x00\x6c\x00\x33\x00\x44\x00\x44\x00\x72\x00\x61\x00\x77\x00\x5f\x00\x53\x00\x65\x00\x74\x00\x75\x00\x70\x00\x20\x00\x67\x30\x02\x98\xb9\x70\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Graphics_D3D11_Shader_Normal3DDraw_Setup で頂点シェーダーの作成に失敗しました\n" @*/ )) ;
				return FALSE ;
			}
		}

		// バーテックスシェーダーのセットアップ
		Graphics_D3D11_DeviceState_SetVertexShader( *VS, FALSE ) ;


		// シェーダーがあるかどうかを調べる
		if( *PS == NULL )
		{
			// シェーダーの作成を試みる
			if( Graphics_D3D11_PixelShader_Create( PSAddress, PS, 1 ) != 0 )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x47\x00\x72\x00\x61\x00\x70\x00\x68\x00\x69\x00\x63\x00\x73\x00\x5f\x00\x44\x00\x33\x00\x44\x00\x31\x00\x31\x00\x5f\x00\x53\x00\x68\x00\x61\x00\x64\x00\x65\x00\x72\x00\x5f\x00\x4e\x00\x6f\x00\x72\x00\x6d\x00\x61\x00\x6c\x00\x33\x00\x44\x00\x44\x00\x72\x00\x61\x00\x77\x00\x5f\x00\x53\x00\x65\x00\x74\x00\x75\x00\x70\x00\x20\x00\x67\x30\xd4\x30\xaf\x30\xbb\x30\xeb\x30\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Graphics_D3D11_Shader_Normal3DDraw_Setup でピクセルシェーダーの作成に失敗しました\n" @*/ )) ;
				return FALSE ;
			}
		}

		// ピクセルシェーダーのセットアップ
		Graphics_D3D11_DeviceState_SetPixelShader( *PS ) ;
	}


	// 終了
	return TRUE ;
}

// アルファテストの比較モードからピクセルシェーダーのアルファテストモード( DIRECT3D11_PS_ALPHATEST_CMP_GREATER 等 )を取得する
extern int Graphics_D3D11_Shader_GetAlphaTestModeIndex( int AlphaTestEnable, int AlphaTestMode /* DX_CMP_NEVER 等 */ )
{
	if( AlphaTestEnable == FALSE )
	{
		return DIRECT3D11_PS_ALPHATEST_CMP_GREATER ;
	}

	switch( AlphaTestMode )
	{
	default:
		return DIRECT3D11_PS_ALPHATEST_CMP_OTHER ;

	case DX_CMP_GREATER :		// DrawAlpha >  TestParam
		return DIRECT3D11_PS_ALPHATEST_CMP_GREATER ;
	}
}

























// 頂点バッファ・インデックスバッファ関係

// 頂点バッファハンドル用の ID3D11Buffer オブジェクトを作成する
extern	int Graphics_D3D11_VertexBuffer_CreateObject( VERTEXBUFFERHANDLEDATA *VertexBuffer, int Restore )
{
	D_D3D11_BUFFER_DESC BufferDesc ;
	HRESULT             hr ;

	_MEMSET( &BufferDesc, 0, sizeof( BufferDesc ) ) ;
	BufferDesc.ByteWidth      = ( UINT )( VertexBuffer->UnitSize * VertexBuffer->Num ) ;
	BufferDesc.Usage          = D_D3D11_USAGE_DEFAULT ;
	BufferDesc.BindFlags      = D_D3D11_BIND_VERTEX_BUFFER ;
	BufferDesc.CPUAccessFlags = 0 ;
	hr = D3D11Device_CreateBuffer( &BufferDesc, NULL, &VertexBuffer->PF->D3D11.VertexBuffer ) ;
	if( FAILED( hr ) )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x02\x98\xb9\x70\xd0\x30\xc3\x30\xd5\x30\xa1\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x28\x75\x6e\x30\x02\x98\xb9\x70\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"頂点バッファハンドル用の頂点バッファの作成に失敗しました\n" @*/ )) ;
		return -1 ;
	}

	// データを復帰する
	if( Restore )
	{
		NS_SetVertexBufferData( 0, VertexBuffer->Buffer, VertexBuffer->Num, VertexBuffer->HandleInfo.Handle ) ; 
	}

	// 正常終了
	return 0 ;
}

// 頂点バッファハンドル用の ID3D11Buffer オブジェクトを解放する
extern	int	Graphics_D3D11_VertexBuffer_ReleaseObject( VERTEXBUFFERHANDLEDATA *VertexBuffer )
{
	// 頂点バッファの解放
	if( VertexBuffer->PF->D3D11.VertexBuffer )
	{
		Direct3D11_Release_Buffer( VertexBuffer->PF->D3D11.VertexBuffer ) ;
		VertexBuffer->PF->D3D11.VertexBuffer = NULL ;
	}

	// 正常終了
	return 0 ;
}

// インデックスバッファハンドル用の ID3D11Buffer オブジェクトを作成する
extern	int	Graphics_D3D11_IndexBuffer_CreateObject( INDEXBUFFERHANDLEDATA *IndexBuffer, int Restore )
{
	D_D3D11_BUFFER_DESC BufferDesc ;
	HRESULT             hr ;

	_MEMSET( &BufferDesc, 0, sizeof( BufferDesc ) ) ;
	BufferDesc.ByteWidth      = ( UINT )( IndexBuffer->UnitSize * IndexBuffer->Num ) ;
	BufferDesc.Usage          = D_D3D11_USAGE_DEFAULT ;
	BufferDesc.BindFlags      = D_D3D11_BIND_INDEX_BUFFER ;
	BufferDesc.CPUAccessFlags = 0 ;
	hr = D3D11Device_CreateBuffer( &BufferDesc, NULL, &IndexBuffer->PF->D3D11.IndexBuffer ) ;
	if( FAILED( hr ) )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\xa4\x30\xf3\x30\xc7\x30\xc3\x30\xaf\x30\xb9\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x28\x75\x6e\x30\xa4\x30\xf3\x30\xc7\x30\xc3\x30\xaf\x30\xb9\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"インデックスバッファハンドル用のインデックスバッファの作成に失敗しました\n" @*/ )) ;
		return -1 ;
	}

	// データを復帰する
	if( Restore )
	{
		NS_SetIndexBufferData( 0, IndexBuffer->Buffer, IndexBuffer->Num, IndexBuffer->HandleInfo.Handle ) ; 
	}

	// 正常終了
	return 0 ;
}

// インデックスバッファハンドル用の ID3D11Buffer オブジェクトを解放する
extern	int	Graphics_D3D11_IndexBuffer_ReleaseObject( INDEXBUFFERHANDLEDATA *IndexBuffer )
{
	// インデックスバッファの解放
	if( IndexBuffer->PF->D3D11.IndexBuffer )
	{
		Direct3D11_Release_Buffer( IndexBuffer->PF->D3D11.IndexBuffer ) ;
		IndexBuffer->PF->D3D11.IndexBuffer = NULL ;
	}

	// 正常終了
	return 0 ;
}





























// Direct3D11 の画面関係

// ScreenCopy や GetDrawScreen を実現するために使用するテンポラリバッファの作成( 0:成功  -1:失敗 )
extern	int		Graphics_D3D11_SetupSubBackBuffer( void )
{
	HRESULT                           hr ;
	D_D3D11_TEXTURE2D_DESC            Texture2DDesc ;
	D_D3D11_RENDER_TARGET_VIEW_DESC   RTVDesc ;
	D_D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc ;
	float                             ColorRGBA[ 4 ] ;
	RECT                              BltRect ;

	// 既にサブバックバッファが作成されている場合は何もしない
	if( GD3D11.Device.Screen.SubBackBufferTexture2D != NULL ) return 0 ;

	// デバイスが無効の場合は何もせず終了
	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// 描画情報を吐き出しておく
	DRAWSTOCKINFO

	// サブバックバッファのサイズをセット
	GD3D11.Device.Screen.SubBackBufferTextureSizeX = GSYS.Screen.MainScreenSizeX ;
	GD3D11.Device.Screen.SubBackBufferTextureSizeY = GSYS.Screen.MainScreenSizeY ;

	// サブバックバッファ用テクスチャの作成
	_MEMSET( &Texture2DDesc, 0, sizeof( Texture2DDesc ) );
	Texture2DDesc.Usage              = D_D3D11_USAGE_DEFAULT ;
	Texture2DDesc.Format             = DIRECT3D11_BACKBUFFER_FORMAT ;
	Texture2DDesc.BindFlags          = D_D3D11_BIND_RENDER_TARGET | D_D3D11_BIND_SHADER_RESOURCE ;
	Texture2DDesc.Width              = ( UINT )GD3D11.Device.Screen.SubBackBufferTextureSizeX ;
	Texture2DDesc.Height             = ( UINT )GD3D11.Device.Screen.SubBackBufferTextureSizeY ;
	Texture2DDesc.CPUAccessFlags     = 0 ;
	Texture2DDesc.MipLevels          = 1 ;
	Texture2DDesc.ArraySize          = 1 ;
	if( GSYS.Setting.FSAAMultiSampleCount > 1 )
	{
		// D_D3D_FEATURE_LEVEL_10_0 未満の場合はマルチサンプルは使用できない
		if( GD3D11.Setting.FeatureLevel < D_D3D_FEATURE_LEVEL_10_0 )
		{
			Texture2DDesc.SampleDesc.Count		= 1 ;
			Texture2DDesc.SampleDesc.Quality	= 0 ;
		}
		else
		{
			Texture2DDesc.SampleDesc.Count		= ( UINT )GSYS.Setting.FSAAMultiSampleCount ;
			Texture2DDesc.SampleDesc.Quality	= ( UINT )GSYS.Setting.FSAAMultiSampleQuality ;
			D3D11Device_CheckMultiSampleParam( Texture2DDesc.Format, &Texture2DDesc.SampleDesc.Count, &Texture2DDesc.SampleDesc.Quality, FALSE ) ;
		}
	}
	else
	{
		Texture2DDesc.SampleDesc.Count		= 1 ;
		Texture2DDesc.SampleDesc.Quality	= 0 ;
	}
	hr = D3D11Device_CreateTexture2D( &Texture2DDesc, NULL, &GD3D11.Device.Screen.SubBackBufferTexture2D ) ;
	if( FAILED( hr ) )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\xb5\x30\xd6\x30\xd0\x30\xc3\x30\xaf\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x28\x75\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x20\x00\x46\x00\x6f\x00\x72\x00\x6d\x00\x61\x00\x74\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x20\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x58\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x20\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x59\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x0a\x00\x00"/*@ L"サブバックバッファ用テクスチャの作成に失敗しました  Format:%d  SizeX:%d  SizeY:%d \n" @*/,
			Texture2DDesc.Format, GD3D11.Device.Screen.SubBackBufferTextureSizeX, GD3D11.Device.Screen.SubBackBufferTextureSizeY )) ;
		return -1 ;
	}

	// サブバックバッファ用レンダーターゲットビューを作成
	_MEMSET( &RTVDesc, 0, sizeof( RTVDesc ) );
	RTVDesc.Format            = Texture2DDesc.Format ;
	if( Texture2DDesc.SampleDesc.Count > 1 )
	{
		RTVDesc.ViewDimension = D_D3D11_RTV_DIMENSION_TEXTURE2DMS ;
	}
	else
	{
		RTVDesc.ViewDimension = D_D3D11_RTV_DIMENSION_TEXTURE2D ;
	}
	hr = D3D11Device_CreateRenderTargetView( GD3D11.Device.Screen.SubBackBufferTexture2D, &RTVDesc, &GD3D11.Device.Screen.SubBackBufferRTV ) ;
	if( FAILED( hr ) )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\xb5\x30\xd6\x30\xd0\x30\xc3\x30\xaf\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x28\x75\xec\x30\xf3\x30\xc0\x30\xfc\x30\xbf\x30\xfc\x30\xb2\x30\xc3\x30\xc8\x30\xd3\x30\xe5\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x20\x00\x46\x00\x6f\x00\x72\x00\x6d\x00\x61\x00\x74\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x20\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x58\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x20\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x59\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x0a\x00\x00"/*@ L"サブバックバッファ用レンダーターゲットビューの作成に失敗しました  Format:%d  SizeX:%d  SizeY:%d \n" @*/,
			Texture2DDesc.Format, GD3D11.Device.Screen.SubBackBufferTextureSizeX, GD3D11.Device.Screen.SubBackBufferTextureSizeY )) ;
		return -1 ;
	}

	// サブバックバッファ用のシェーダリソースビューを作成
	_MEMSET( &SRVDesc, 0, sizeof( SRVDesc ) );
	SRVDesc.Format            = Texture2DDesc.Format ;
	if( Texture2DDesc.SampleDesc.Count > 1 )
	{
		SRVDesc.ViewDimension = D_D3D11_SRV_DIMENSION_TEXTURE2DMS ;
	}
	else
	{
		SRVDesc.ViewDimension = D_D3D11_SRV_DIMENSION_TEXTURE2D ;
	}
	SRVDesc.Texture2D.MipLevels = 1 ;
	hr = D3D11Device_CreateShaderResourceView( GD3D11.Device.Screen.SubBackBufferTexture2D, &SRVDesc, &GD3D11.Device.Screen.SubBackBufferSRV ) ;
	if( FAILED( hr ) )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\xb5\x30\xd6\x30\xd0\x30\xc3\x30\xaf\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x28\x75\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\xea\x30\xbd\x30\xfc\x30\xb9\x30\xd3\x30\xe5\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x20\x00\x46\x00\x6f\x00\x72\x00\x6d\x00\x61\x00\x74\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x20\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x58\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x20\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x59\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x0a\x00\x00"/*@ L"サブバックバッファ用シェーダーリソースビューの作成に失敗しました  Format:%d  SizeX:%d  SizeY:%d \n" @*/,
			Texture2DDesc.Format, GD3D11.Device.Screen.SubBackBufferTextureSizeX, GD3D11.Device.Screen.SubBackBufferTextureSizeY )) ;
		return -1 ;
	}

	// 本バックバッファの内容を転送
	BltRect.left   = 0 ;
	BltRect.top    = 0 ;
	BltRect.right  = GD3D11.Device.Screen.SubBackBufferTextureSizeX ;
	BltRect.bottom = GD3D11.Device.Screen.SubBackBufferTextureSizeY ;
	if( BltRect.right > ( int )GD3D11.Device.Screen.OutputWindowInfo[ 0 ].BufferTexture2DDesc.Width )
	{
		BltRect.right = ( LONG )GD3D11.Device.Screen.OutputWindowInfo[ 0 ].BufferTexture2DDesc.Width ;
	}
	if( BltRect.bottom > ( int )GD3D11.Device.Screen.OutputWindowInfo[ 0 ].BufferTexture2DDesc.Height )
	{
		BltRect.bottom = ( LONG )GD3D11.Device.Screen.OutputWindowInfo[ 0 ].BufferTexture2DDesc.Height ;
	}
	Graphics_D3D11_StretchRect(
		GD3D11.Device.Screen.OutputWindowInfo[ 0 ].BufferTexture2D,
		GD3D11.Device.Screen.OutputWindowInfo[ 0 ].BufferSRV,
		&BltRect,
		GD3D11.Device.Screen.SubBackBufferTexture2D,
		GD3D11.Device.Screen.SubBackBufferRTV,
		&BltRect
	) ;

	// 本バックバッファの内容を初期化
	ColorRGBA[ 0 ] = 0.0f ;
	ColorRGBA[ 1 ] = 0.0f ;
	ColorRGBA[ 2 ] = 0.0f ;
	ColorRGBA[ 3 ] = 0.0f ;
	D3D11DeviceContext_ClearRenderTargetView( GD3D11.Device.Screen.OutputWindowInfo[ 0 ].BufferRTV, ColorRGBA ) ;

	// レンダリングターゲットの変更
	GD3D11.Device.DrawSetting.CancelSettingEqualCheck = TRUE ;
	Graphics_D3D11_DeviceState_SetRenderTarget( GD3D11.Device.Screen.SubBackBufferTexture2D, GD3D11.Device.Screen.SubBackBufferRTV ) ;
	GD3D11.Device.DrawSetting.CancelSettingEqualCheck = FALSE ;

	// 終了
	return 0 ;
}

// 出力先ウインドウの追加を行う( -1:失敗  -1以外:対象ウインドウ用情報の配列インデックス )
extern	int		Graphics_D3D11_OutputWindow_Add( HWND TargetWindow, int IsMainTarget )
{
	int                    i ;
	int                    j ;
	int                    NewIndex ;
	HRESULT                Result ;
	D_DXGI_SWAP_CHAIN_DESC SwapChainDesc ;
	GRAPHICS_HARDDATA_DIRECT3D11_OUTPUTWINDOWINFO *OWI ;

	SETUP_WIN_API

	// 既に指定のウインドウが対象に入っているか調べる
	OWI = GD3D11.Device.Screen.OutputWindowInfo ;
	j = 0 ;
	NewIndex = -1 ;
	for( i = 0 ; i < DX_D3D11_MAX_OUTPUTWINDOW && j < GD3D11.Device.Screen.OutputWindowNum ; i ++, OWI ++ )
	{
		if( OWI->UseFlag == FALSE )
		{
			NewIndex = i ;
			continue ;
		}

		j ++ ;

		if( OWI->DXGISwapChainDesc.OutputWindow == TargetWindow )
		{
			// 既に入っていたら配列のインデックスを返す
			return i ;
		}
	}

	// 既に出力先ウインドウの数が最大に達していたらエラー
	if( GD3D11.Device.Screen.OutputWindowNum >= DX_D3D11_MAX_OUTPUTWINDOW )
	{
		return -1 ;
	}

	// 入っていない場合は新規に作成

	// 使用されていない配列要素を検索する
	if( NewIndex == -1 )
	{
		for( NewIndex = 0; GD3D11.Device.Screen.OutputWindowInfo[ NewIndex ].UseFlag; NewIndex++ ){}
	}
	OWI = &GD3D11.Device.Screen.OutputWindowInfo[ NewIndex ] ;

	// スワップチェインの作成
	{
/*		UINT FormatSupport ;
		UINT SupportRenderTarget ;
		UINT DepthStencil ;
		Result = GAPIWin.D3D11DeviceObject->CheckFormatSupport( 
//			D_DXGI_FORMAT_D16_UNORM,
//			D_DXGI_FORMAT_B8G8R8X8_UNORM,
			D_DXGI_FORMAT_R24G8_TYPELESS,
			&FormatSupport ) ;
		SupportRenderTarget = FormatSupport & D_D3D11_FORMAT_SUPPORT_RENDER_TARGET ;
		DepthStencil        = FormatSupport & D_D3D11_FORMAT_SUPPORT_DEPTH_STENCIL ;
*/

		_MEMSET( &SwapChainDesc, 0, sizeof( SwapChainDesc ) ) ;
		SwapChainDesc.BufferDesc.RefreshRate.Numerator		= ( UINT )( GSYS.Screen.MainScreenRefreshRate == 0 ? 60 : GSYS.Screen.MainScreenRefreshRate ) ;
		SwapChainDesc.BufferDesc.RefreshRate.Denominator	= 1 ;
		SwapChainDesc.BufferDesc.Format						= DIRECT3D11_BACKBUFFER_FORMAT /* GSYS.Screen.MainScreenColorBitDepth == 16 ? D_DXGI_FORMAT_B5G5R5A1_UNORM : D_DXGI_FORMAT_B8G8R8A8_UNORM */ ;
		SwapChainDesc.BufferDesc.ScanlineOrdering			= D_DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED ;
		SwapChainDesc.BufferDesc.Scaling					= D_DXGI_MODE_SCALING_UNSPECIFIED ;
		SwapChainDesc.BufferCount							= 2 ;
		SwapChainDesc.BufferUsage							= D_DXGI_USAGE_RENDER_TARGET_OUTPUT | D_DXGI_USAGE_SHADER_INPUT ;
		SwapChainDesc.OutputWindow							= TargetWindow ;
		SwapChainDesc.SwapEffect							= D_DXGI_SWAP_EFFECT_DISCARD ;
		SwapChainDesc.Flags									= D_DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH ;
		if( IsMainTarget )
		{
			if( NS_GetWindowModeFlag() == TRUE )
			{
				SwapChainDesc.BufferDesc.Width				= ( UINT )GSYS.Screen.MainScreenSizeX ;
				SwapChainDesc.BufferDesc.Height				= ( UINT )GSYS.Screen.MainScreenSizeY ;
				SwapChainDesc.Windowed						= TRUE ;

				DXST_LOGFILEFMT_ADDUTF16LE(( "\x5b\x00\xa6\x30\xa4\x30\xf3\x30\xc9\x30\xa6\x30\xe2\x30\xfc\x30\xc9\x30\x20\x00\x25\x00\x64\x00\x78\x00\x25\x00\x64\x00\x5d\x00\x00"/*@ L"[ウインドウモード %dx%d]" @*/, SwapChainDesc.BufferDesc.Width, SwapChainDesc.BufferDesc.Height )) ;
			}
			else
			{
				// フルスクリーンモードの情報をセットアップ
				Graphics_Screen_SetupFullScreenModeInfo() ;

				SwapChainDesc.BufferDesc.Width				= ( UINT )GSYS.Screen.FullScreenUseDispModeData.Width ;
				SwapChainDesc.BufferDesc.Height				= ( UINT )GSYS.Screen.FullScreenUseDispModeData.Height ;
				SwapChainDesc.Windowed						= FALSE ;

				// フルスクリーンモードフラグを立てる
				GD3D11.Device.Screen.FullscreenSatte        = TRUE ;

				DXST_LOGFILEFMT_ADDUTF16LE(( "\x5b\x00\xd5\x30\xeb\x30\xb9\x30\xaf\x30\xea\x30\xfc\x30\xf3\x30\xe2\x30\xfc\x30\xc9\x30\x20\x00\x25\x00\x64\x00\x78\x00\x25\x00\x64\x00\x5d\x00\x00"/*@ L"[フルスクリーンモード %dx%d]" @*/, SwapChainDesc.BufferDesc.Width, SwapChainDesc.BufferDesc.Height )) ;
			}
		}
		else
		{
			RECT ClientRect ;

			WinAPIData.Win32Func.GetClientRectFunc( TargetWindow, &ClientRect ) ;

			SwapChainDesc.BufferDesc.Width					= ( UINT )( ClientRect.right  - ClientRect.left ) ;
			SwapChainDesc.BufferDesc.Height					= ( UINT )( ClientRect.bottom - ClientRect.top  ) ;
			SwapChainDesc.Windowed							= TRUE ;

			DXST_LOGFILE_ADDUTF16LE( "\x5b\x00\xa6\x30\xa4\x30\xf3\x30\xc9\x30\xa6\x30\xe2\x30\xfc\x30\xc9\x30\x5d\x00\x20\x00\x00"/*@ L"[ウインドウモード] " @*/ ) ;
		}

		// FSAAの設定値を調べる
		if( GSYS.Setting.FSAAMultiSampleCount > 1 )
		{
			// D_D3D_FEATURE_LEVEL_10_0 未満の場合はマルチサンプルは使用できない
			if( GD3D11.Setting.FeatureLevel < D_D3D_FEATURE_LEVEL_10_0 )
			{
				SwapChainDesc.SampleDesc.Count		= 1 ;
				SwapChainDesc.SampleDesc.Quality	= 0 ;
			}
			else
			{
				SwapChainDesc.SampleDesc.Count		  = ( UINT )GSYS.Setting.FSAAMultiSampleCount ;
				SwapChainDesc.SampleDesc.Quality	  = ( UINT )GSYS.Setting.FSAAMultiSampleQuality ;
				D3D11Device_CheckMultiSampleParam( SwapChainDesc.BufferDesc.Format, &SwapChainDesc.SampleDesc.Count, &SwapChainDesc.SampleDesc.Quality, FALSE ) ;
			}
		}
		else
		{
			SwapChainDesc.SampleDesc.Count		= 1 ;
			SwapChainDesc.SampleDesc.Quality	= 0 ;
		}
		GSYS.Setting.FSAAMultiSampleCount   = ( int )SwapChainDesc.SampleDesc.Count ;
		GSYS.Setting.FSAAMultiSampleQuality = ( int )SwapChainDesc.SampleDesc.Quality ;

		DXST_LOGFILE_ADDUTF16LE( "\x49\x00\x44\x00\x58\x00\x47\x00\x49\x00\x53\x00\x77\x00\x61\x00\x70\x00\x43\x00\x68\x00\x61\x00\x69\x00\x6e\x00\x20\x00\x92\x30\x5c\x4f\x10\x62\x57\x30\x7e\x30\x59\x30\x2e\x00\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"IDXGISwapChain を作成します.... " @*/ ) ;

		// スワップチェインの作成
		Result = DXGIFactory_CreateSwapChain( &SwapChainDesc, &OWI->DXGISwapChain ) ;
		if( OWI->DXGISwapChain == NULL )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x31\x59\x57\x65\x20\x00\xa8\x30\xe9\x30\xfc\x30\xb3\x30\xfc\x30\xc9\x30\x1a\xff\x30\x00\x78\x00\x25\x00\x30\x00\x38\x00\x78\x00\x00"/*@ L"失敗 エラーコード：0x%08x" @*/, Result )) ;
			goto ERR ;
		}

		// ALT+Enterを無効化する
		DXGIFactory_MakeWindowAssociation( TargetWindow, D_DXGI_MWA_NO_WINDOW_CHANGES | D_DXGI_MWA_NO_ALT_ENTER ) ;

		DXST_LOGFILE_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"成功\n" @*/ ) ;

		DXST_LOGFILEFMT_ADDUTF16LE(( "\x49\x00\x44\x00\x58\x00\x47\x00\x49\x00\x46\x00\x61\x00\x63\x00\x74\x00\x6f\x00\x72\x00\x79\x00\x2d\x00\x3e\x00\x43\x00\x72\x00\x65\x00\x61\x00\x74\x00\x65\x00\x53\x00\x77\x00\x61\x00\x70\x00\x43\x00\x68\x00\x61\x00\x69\x00\x6e\x00\x20\x00\x6e\x30\x3b\x62\x8a\x30\x24\x50\x1a\xff\x30\x00\x78\x00\x25\x00\x30\x00\x38\x00\x78\x00\x00"/*@ L"IDXGIFactory->CreateSwapChain の戻り値：0x%08x" @*/, Result )) ;

		// スワップチェインの情報を取得する
		DXGISwapChain_GetDesc( OWI->DXGISwapChain, &OWI->DXGISwapChainDesc ) ;
	}


	DXST_LOGFILE_ADDUTF16LE( "\x49\x00\x44\x00\x58\x00\x47\x00\x49\x00\x4f\x00\x75\x00\x74\x00\x70\x00\x75\x00\x74\x00\x20\x00\x92\x30\xd6\x53\x97\x5f\x57\x30\x7e\x30\x59\x30\x2e\x00\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"IDXGIOutput を取得します.... " @*/ ) ;

	// IDXGIOutput の取得
	Result = DXGISwapChain_GetContainingOutput( OWI->DXGISwapChain, &OWI->DXGIOutput ) ;
	if( Result != S_OK )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x31\x59\x57\x65\x20\x00\xa8\x30\xe9\x30\xfc\x30\xb3\x30\xfc\x30\xc9\x30\x1a\xff\x30\x00\x78\x00\x25\x00\x30\x00\x38\x00\x78\x00\x00"/*@ L"失敗 エラーコード：0x%08x" @*/, Result )) ;
		goto ERR ;
	}

	DXST_LOGFILE_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"成功\n" @*/ ) ;


	// バックバッファのセットアップ
	if( Graphics_D3D11_OutputWindow_SetupBuffer( NewIndex ) < 0 )
	{
		goto ERR ;
	}

	// 使用しているかどうかのフラグを立てる
	OWI->UseFlag = TRUE ;

	// 出力先ウインドウの数を増やす
	GD3D11.Device.Screen.OutputWindowNum ++ ;

	// 正常終了
	return NewIndex ;

	// エラー処理
ERR :
	if( OWI->BufferSRV != NULL )
	{
		Direct3D11_Release_ShaderResourceView( OWI->BufferSRV ) ;
		OWI->BufferSRV = NULL ;
	}

	if( OWI->BufferRTV != NULL )
	{
		Direct3D11_Release_RenderTargetView( OWI->BufferRTV ) ;
		OWI->BufferRTV = NULL ;
	}

	if( OWI->BufferTexture2D != NULL )
	{
		Direct3D11_Release_Texture2D( OWI->BufferTexture2D ) ;
		OWI->BufferTexture2D = NULL ;
	}

	if( OWI->DXGIOutput != NULL )
	{
		Direct3D11_ObjectRelease( OWI->DXGIOutput ) ;
		OWI->DXGIOutput = NULL ;
	}

	if( OWI->DXGISwapChain != NULL )
	{
		Direct3D11_ObjectRelease( OWI->DXGISwapChain ) ;
		OWI->DXGISwapChain = NULL ;
	}

	return -1 ;
}

// 指定のウインドウを出力先ウインドウから外す
extern int Graphics_D3D11_OutputWindow_Sub( HWND TargetWindow, int Index )
{
	GRAPHICS_HARDDATA_DIRECT3D11_OUTPUTWINDOWINFO *OWI ;

	// TargetWindow が NULL の場合は Index を使用する
	if( TargetWindow == NULL )
	{
		if( Index < 0 || Index >= DX_D3D11_MAX_OUTPUTWINDOW )
		{
			return -1 ;
		}
		OWI = &GD3D11.Device.Screen.OutputWindowInfo[ Index ] ;

		if( OWI->UseFlag == FALSE )
		{
			return -1 ;
		}
	}
	else
	{
		int i ;

		OWI = GD3D11.Device.Screen.OutputWindowInfo ;
		for( i = 0 ; i < DX_D3D11_MAX_OUTPUTWINDOW ; i ++, OWI ++ )
		{
			if( OWI->UseFlag && OWI->DXGISwapChainDesc.OutputWindow == TargetWindow )
			{
				break ;
			}
		}
		if( i == DX_D3D11_MAX_OUTPUTWINDOW )
		{
			return -1 ;
		}
		Index = i ;
	}

	Graphics_D3D11_OutputWindow_ReleaseBuffer( Index ) ;

	if( OWI->DXGIOutput != NULL )
	{
		Direct3D11_ObjectRelease( OWI->DXGIOutput ) ;
		OWI->DXGIOutput = NULL ;
	}

	if( OWI->DXGISwapChain != NULL )
	{
		Direct3D11_ObjectRelease( OWI->DXGISwapChain ) ;
		OWI->DXGISwapChain = NULL ;
	}

	// 使用しているかどうかのフラグを倒す
	if( OWI->UseFlag )
	{
		OWI->UseFlag = FALSE ;

		// 出力先ウインドウの数を減らす
		GD3D11.Device.Screen.OutputWindowNum -- ;
	}

	// 終了
	return 0 ;
}

// 指定の出力先ウインドウ用のバックバッファーをセットアップする
extern int Graphics_D3D11_OutputWindow_SetupBuffer( int Index )
{
	GRAPHICS_HARDDATA_DIRECT3D11_OUTPUTWINDOWINFO *OWI ;
	HRESULT Result ;

	if( Index < 0 || Index >= DX_D3D11_MAX_OUTPUTWINDOW )
	{
		return -1 ;
	}

	OWI = &GD3D11.Device.Screen.OutputWindowInfo[ Index ] ;


//	DXST_LOGFILE_ADDUTF16LE( "\xb9\x30\xef\x30\xc3\x30\xd7\x30\xc1\x30\xa7\x30\xa4\x30\xf3\x30\x6e\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\x20\x00\x49\x00\x44\x00\x33\x00\x44\x00\x31\x00\x31\x00\x54\x00\x65\x00\x78\x00\x74\x00\x75\x00\x72\x00\x65\x00\x32\x00\x44\x00\x20\x00\x92\x30\xd6\x53\x97\x5f\x57\x30\x7e\x30\x59\x30\x2e\x00\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"スワップチェインのバッファの ID3D11Texture2D を取得します.... " @*/ ) ;

	// バックバッファを取得
	Result = DXGISwapChain_GetBuffer( OWI->DXGISwapChain, 0, IID_ID3D11TEXTURE2D, ( void ** )&OWI->BufferTexture2D ) ;
	if( Result != S_OK )
	{
//		DXST_LOGFILE_ADDUTF16LE( "\x31\x59\x57\x65\x0a\x00\x00"/*@ L"失敗\n" @*/ ) ;
		DXST_LOGFILE_ADDUTF16LE( "\xb9\x30\xef\x30\xc3\x30\xd7\x30\xc1\x30\xa7\x30\xa4\x30\xf3\x30\x6e\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\x20\x00\x49\x00\x44\x00\x33\x00\x44\x00\x31\x00\x31\x00\x54\x00\x65\x00\x78\x00\x74\x00\x75\x00\x72\x00\x65\x00\x32\x00\x44\x00\x20\x00\x6e\x30\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"スワップチェインのバッファの ID3D11Texture2D の取得に失敗しました\n" @*/ ) ;
		return -1 ;
	}

//	DXST_LOGFILE_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"成功\n" @*/ ) ;


	// バックバッファの情報を取得
	D3D11Texture2D_GetDesc( OWI->BufferTexture2D, &OWI->BufferTexture2DDesc ) ;


//	DXST_LOGFILE_ADDUTF16LE( "\xb9\x30\xef\x30\xc3\x30\xd7\x30\xc1\x30\xa7\x30\xa4\x30\xf3\x30\x6e\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\x20\x00\x49\x00\x44\x00\x33\x00\x44\x00\x31\x00\x31\x00\x53\x00\x68\x00\x61\x00\x64\x00\x65\x00\x72\x00\x52\x00\x65\x00\x73\x00\x6f\x00\x75\x00\x72\x00\x63\x00\x65\x00\x56\x00\x69\x00\x65\x00\x77\x00\x20\x00\x92\x30\x5c\x4f\x10\x62\x57\x30\x7e\x30\x59\x30\x2e\x00\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"スワップチェインのバッファの ID3D11ShaderResourceView を作成します.... " @*/ ) ;

	// バックバッファ用のシェーダーリソースビューを作成
	Result = D3D11Device_CreateShaderResourceView( OWI->BufferTexture2D, NULL, &OWI->BufferSRV ) ;
	if( Result != S_OK )
	{
//		DXST_LOGFILE_ADDUTF16LE( "\x31\x59\x57\x65\x0a\x00\x00"/*@ L"失敗\n" @*/ ) ;
		DXST_LOGFILE_ADDUTF16LE( "\xb9\x30\xef\x30\xc3\x30\xd7\x30\xc1\x30\xa7\x30\xa4\x30\xf3\x30\x6e\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\x20\x00\x49\x00\x44\x00\x33\x00\x44\x00\x31\x00\x31\x00\x53\x00\x68\x00\x61\x00\x64\x00\x65\x00\x72\x00\x52\x00\x65\x00\x73\x00\x6f\x00\x75\x00\x72\x00\x63\x00\x65\x00\x56\x00\x69\x00\x65\x00\x77\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"スワップチェインのバッファの ID3D11ShaderResourceView の作成に失敗しました\n" @*/ ) ;
		return -1 ;
	}

//	DXST_LOGFILE_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"成功\n" @*/ ) ;


//	DXST_LOGFILE_ADDUTF16LE( "\xb9\x30\xef\x30\xc3\x30\xd7\x30\xc1\x30\xa7\x30\xa4\x30\xf3\x30\x6e\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\x20\x00\x49\x00\x44\x00\x33\x00\x44\x00\x31\x00\x31\x00\x52\x00\x65\x00\x6e\x00\x64\x00\x65\x00\x72\x00\x54\x00\x61\x00\x72\x00\x67\x00\x65\x00\x74\x00\x56\x00\x69\x00\x65\x00\x77\x00\x20\x00\x92\x30\x5c\x4f\x10\x62\x57\x30\x7e\x30\x59\x30\x2e\x00\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"スワップチェインのバッファの ID3D11RenderTargetView を作成します.... " @*/ ) ;

	// バックバッファ用のレンダーターゲットを作成
	Result = D3D11Device_CreateRenderTargetView( OWI->BufferTexture2D, NULL, &OWI->BufferRTV ) ;
	if( Result != S_OK )
	{
//		DXST_LOGFILE_ADDUTF16LE( "\x31\x59\x57\x65\x0a\x00\x00"/*@ L"失敗\n" @*/ ) ;
		DXST_LOGFILE_ADDUTF16LE( "\xb9\x30\xef\x30\xc3\x30\xd7\x30\xc1\x30\xa7\x30\xa4\x30\xf3\x30\x6e\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\x20\x00\x49\x00\x44\x00\x33\x00\x44\x00\x31\x00\x31\x00\x52\x00\x65\x00\x6e\x00\x64\x00\x65\x00\x72\x00\x54\x00\x61\x00\x72\x00\x67\x00\x65\x00\x74\x00\x56\x00\x69\x00\x65\x00\x77\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"スワップチェインのバッファの ID3D11RenderTargetView の作成に失敗しました\n" @*/ ) ;
		return -1 ;
	}

//	DXST_LOGFILE_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"成功\n" @*/ ) ;


	// 正常終了
	return 0 ;
}

// 指定の出力先ウインドウ用のバックバッファーの後始末をする
extern int Graphics_D3D11_OutputWindow_ReleaseBuffer( int Index )
{
	GRAPHICS_HARDDATA_DIRECT3D11_OUTPUTWINDOWINFO *OWI ;

	if( Index < 0 || Index >= DX_D3D11_MAX_OUTPUTWINDOW )
	{
		return -1 ;
	}

	OWI = &GD3D11.Device.Screen.OutputWindowInfo[ Index ] ;

	if( OWI->BufferSRV != NULL )
	{
		Direct3D11_Release_ShaderResourceView( OWI->BufferSRV ) ;
		OWI->BufferSRV = NULL ;
	}

	if( OWI->BufferRTV != NULL )
	{
		Direct3D11_Release_RenderTargetView( OWI->BufferRTV ) ;
		OWI->BufferRTV = NULL ;
	}

	if( OWI->BufferTexture2D != NULL )
	{
		Direct3D11_Release_Texture2D( OWI->BufferTexture2D ) ;
		OWI->BufferTexture2D = NULL ;
	}

	// 終了
	return 0 ;
}

// 指定の出力先ウインドウのバックバッファのサイズを変更する
extern int Graphics_D3D11_OutputWindow_ResizeBuffer( int Index, int NewSizeX, int NewSizeY )
{
	int     RenderTargetIndex ;
	int     i ;
	HRESULT hr ;
	GRAPHICS_HARDDATA_DIRECT3D11_OUTPUTWINDOWINFO *OWI ;

	if( NewSizeX == 0 || NewSizeY == 0 )
	{
		return -1 ;
	}

	if( Index < 0 || Index >= DX_D3D11_MAX_OUTPUTWINDOW )
	{
		return -1 ;
	}

	OWI = &GD3D11.Device.Screen.OutputWindowInfo[ Index ] ;

	// 指定のバックバッファが描画対象になっている場合は一度外す
	RenderTargetIndex = -1 ;
	for( i = 0 ; i < DX_RENDERTARGET_COUNT ; i ++ )
	{
		if( GD3D11.Device.State.TargetTexture2D[ i ] == OWI->BufferTexture2D )
		{
			RenderTargetIndex = i ;
			Graphics_D3D11_DeviceState_SetRenderTarget( NULL, NULL, RenderTargetIndex ) ;
			break ;
		}
	}

	// バックバッファを解放
	Graphics_D3D11_OutputWindow_ReleaseBuffer( Index ) ;

	// バックバッファのサイズを変更
	hr = DXGISwapChain_ResizeBuffers( OWI->DXGISwapChain, 2, ( UINT )NewSizeX, ( UINT )NewSizeY, DIRECT3D11_BACKBUFFER_FORMAT, 0 ) ;
	if( hr != S_OK )
	{
		DXST_LOGFILE_ADDUTF16LE( "\xd0\x30\xc3\x30\xaf\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\xb5\x30\xa4\x30\xba\x30\x09\x59\xf4\x66\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"バックバッファのサイズ変更に失敗しました\n" @*/ ) ;
		return -1 ;
	}

	// バックバッファのセットアップ
	if( Graphics_D3D11_OutputWindow_SetupBuffer( Index ) < 0 )
	{
		return -1 ;
	}

	// バックバッファが描画対象になっていたら再度描画対象に設定する
	if( RenderTargetIndex != -1 )
	{
		Graphics_D3D11_DeviceState_SetRenderTarget( OWI->BufferTexture2D, OWI->BufferRTV, RenderTargetIndex ) ;
	}

	// 終了
	return 0 ;
}

// 深度バッファオブジェクトの作成( 0:成功  -1:失敗 )
extern	int		Graphics_D3D11_CreateDepthBuffer( void )
{
	HRESULT hr ;
	int depth ;
	int index ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// 既に作成済みの場合は一度削除する
	if( GD3D11.Device.Screen.DepthBufferTexture2D )
	{
		Direct3D11_Release_Texture2D( GD3D11.Device.Screen.DepthBufferTexture2D ) ;
		GD3D11.Device.Screen.DepthBufferTexture2D = NULL ;
	}
	if( GD3D11.Device.Screen.DepthBufferDSV )
	{
		Direct3D11_Release_DepthStencilView( GD3D11.Device.Screen.DepthBufferDSV ) ;
		GD3D11.Device.Screen.DepthBufferDSV = NULL ;
	}

	DXST_LOGFILE_ADDUTF16LE( "\xf1\x6d\xa6\x5e\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x92\x30\x5c\x4f\x10\x62\x57\x30\x7e\x30\x59\x30\x2e\x00\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"深度バッファを作成します.... " @*/ ) ;

	// 深度バッファのサイズを決定
	GD3D11.Device.Screen.DepthBufferSizeX = GSYS.Screen.MainScreenSizeX ;
	GD3D11.Device.Screen.DepthBufferSizeY = GSYS.Screen.MainScreenSizeY ;

	// 深度バッファのビット深度を決定
	if( GD3D11.Setting.UserDepthBufferBitDepthSet == FALSE ||
		GD3D11.Device.Screen.DepthBufferBitDepth == 0  )
	{
		depth = DEFAULT_ZBUFFER_BITDEPTH ;
		GD3D11.Device.Screen.DepthBufferBitDepth = DEFAULT_ZBUFFER_BITDEPTH ;
	}
	else
	{
		depth = GD3D11.Device.Screen.DepthBufferBitDepth ;
	}
	switch( depth )
	{
	default :
	case 16 : index = ZBUFFER_FORMAT_16BIT ; break ;
	case 24 : index = ZBUFFER_FORMAT_24BIT ; break ;
	case 32 : index = ZBUFFER_FORMAT_32BIT ; break ;
	}

	// 深度バッファテクスチャの作成
	{
		D_D3D11_TEXTURE2D_DESC Texture2DDesc ;

		_MEMSET( &Texture2DDesc, 0, sizeof( Texture2DDesc ) ) ;
		Texture2DDesc.Width              = ( UINT )GSYS.Screen.MainScreenSizeX ;
		Texture2DDesc.Height             = ( UINT )GSYS.Screen.MainScreenSizeY ;
		Texture2DDesc.MipLevels          = 1 ;
		Texture2DDesc.ArraySize          = 1 ;
		Texture2DDesc.Format             = GD3D11.Device.Caps.DepthBufferTexture2DFormat[ index ] ;
//		Texture2DDesc.Format             = GD3D11.Device.Caps.DepthBufferDepthStencilFormat[ index ] ;
		Texture2DDesc.SampleDesc.Count   = ( UINT )GSYS.Setting.FSAAMultiSampleCount ;
		Texture2DDesc.SampleDesc.Quality = ( UINT )GSYS.Setting.FSAAMultiSampleQuality ;
		Texture2DDesc.Usage              = D_D3D11_USAGE_DEFAULT ;
		Texture2DDesc.BindFlags          = D_D3D11_BIND_DEPTH_STENCIL /* | D_D3D11_BIND_SHADER_RESOURCE */ ;
		Texture2DDesc.CPUAccessFlags     = 0 ;
		Texture2DDesc.MiscFlags          = 0 ;

		// 作成
		hr = D3D11Device_CreateTexture2D( &Texture2DDesc, NULL, &GD3D11.Device.Screen.DepthBufferTexture2D ) ;
		if( FAILED( hr ) )
		{
			DXST_LOGFILE_ADDUTF16LE( "\xf1\x6d\xa6\x5e\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x28\x75\x20\x00\x54\x00\x65\x00\x78\x00\x74\x00\x75\x00\x72\x00\x65\x00\x32\x00\x44\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"深度バッファ用 Texture2D の作成に失敗しました\n" @*/ ) ;
			return -1 ;
		}
	}

	// 深度ステンシルビューの作成
	{
		D_D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc ;

		_MEMSET( &DepthStencilViewDesc, 0, sizeof( DepthStencilViewDesc ) ) ;
		DepthStencilViewDesc.Format = GD3D11.Device.Caps.DepthBufferDepthStencilFormat[ index ] ;
		if ( GSYS.Setting.FSAAMultiSampleCount <= 1 )
		{
			DepthStencilViewDesc.ViewDimension      = D_D3D11_DSV_DIMENSION_TEXTURE2D;
			DepthStencilViewDesc.Texture2D.MipSlice = 0;
		}
		else
		{
			DepthStencilViewDesc.ViewDimension		= D_D3D11_DSV_DIMENSION_TEXTURE2DMS;
		}

		hr = D3D11Device_CreateDepthStencilView( GD3D11.Device.Screen.DepthBufferTexture2D, &DepthStencilViewDesc, &GD3D11.Device.Screen.DepthBufferDSV ) ;
		if( FAILED( hr ) )
		{
			DXST_LOGFILE_ADDUTF16LE( "\xf1\x6d\xa6\x5e\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x28\x75\x20\x00\x44\x00\x65\x00\x70\x00\x74\x00\x68\x00\x53\x00\x74\x00\x65\x00\x6e\x00\x63\x00\x69\x00\x6c\x00\x56\x00\x69\x00\x65\x00\x77\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"深度バッファ用 DepthStencilView の作成に失敗しました\n" @*/ ) ;
			return -1 ;
		}
	}

	// デバイスにセット
	Graphics_D3D11_DeviceState_SetDepthStencil( GD3D11.Device.Screen.DepthBufferTexture2D, GD3D11.Device.Screen.DepthBufferDSV ) ;

	// クリア
	D3D11DeviceContext_ClearDepthStencilView( GD3D11.Device.Screen.DepthBufferDSV, D_D3D11_CLEAR_DEPTH, 1.0f, 0 ) ;

	DXST_LOGFILE_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"成功\n" @*/ ) ;

	// 終了
	return 0 ;
}























// Direct3D11 の画像関係

// D_DXGI_FORMAT のフォーマットに合わせたカラーフォーマット情報を取得する
extern COLORDATA *Graphics_D3D11_GetD3DFormatColorData( D_DXGI_FORMAT Format )
{
	volatile static int InitializeFlag = FALSE ;
	static COLORDATA B5G6R5_ColorData ;
	static COLORDATA R8G8B8A8_ColorData ;
	static COLORDATA B8G8R8X8_ColorData ;
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
		NS_CreateColorData( &B5G6R5_ColorData,         16, 0x0000f800, 0x000007e0, 0x0000001f, 0x00000000 ) ;
		NS_CreateColorData( &R8G8B8A8_ColorData,       32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000 ) ;
		NS_CreateColorData( &B8G8R8X8_ColorData,       32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000 ) ;
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
		InitializeFlag = TRUE ;
	}

	switch( Format )
	{
	case D_DXGI_FORMAT_B5G6R5_UNORM :       return &B5G6R5_ColorData ;
	case D_DXGI_FORMAT_R8G8B8A8_UNORM :     return &R8G8B8A8_ColorData ;
	case D_DXGI_FORMAT_B8G8R8X8_UNORM :     return &B8G8R8X8_ColorData ;
	case D_DXGI_FORMAT_R16G16B16A16_UNORM : return &A16B16G16R16I_ColorData ;
	case D_DXGI_FORMAT_R16G16B16A16_FLOAT : return &A16B16G16R16F_ColorData ;
	case D_DXGI_FORMAT_R32G32B32A32_FLOAT : return &A32B32G32R32F_ColorData ;
	case D_DXGI_FORMAT_R8_UNORM :           return &ONE_I8_ColorData ;
	case D_DXGI_FORMAT_R16_UNORM :          return &ONE_I16_ColorData ;
	case D_DXGI_FORMAT_R16_FLOAT :          return &ONE_F16_ColorData ;
	case D_DXGI_FORMAT_R32_FLOAT :          return &ONE_F32_ColorData ;
	case D_DXGI_FORMAT_R8G8_UNORM :         return &TWO_I8_ColorData ;
	case D_DXGI_FORMAT_R16G16_UNORM :       return &TWO_I16_ColorData ;
	case D_DXGI_FORMAT_R16G16_FLOAT :       return &TWO_F16_ColorData ;
	case D_DXGI_FORMAT_R32G32_FLOAT :       return &TWO_F32_ColorData ;
	}
	return NULL ;
}

// 描画先用テクスチャと描画用テクスチャが分かれている場合に、描画用テクスチャに描画先用テクスチャの内容を反映する
extern	int		Graphics_D3D11_UpdateDrawTexture( IMAGEDATA_ORIG_HARD_TEX *OrigTex, int TargetSurface, D_DXGI_FORMAT Format )
{
	D3D11DeviceContext_ResolveSubresource(
		OrigTex->PF->D3D11.Texture,
		D_D3D11CalcSubresource( 0, TargetSurface, 1 ),
		OrigTex->PF->D3D11.MSTexture,
		D_D3D11CalcSubresource( 0, TargetSurface, 1 ),
		Format
	) ;

	// 終了
	return 0 ;
}

// 基本イメージのフォーマットを DX_BASEIMAGE_FORMAT_NORMAL に変換する必要があるかどうかをチェックする
// ( RequiredRgbBaseImageConvFlag と RequiredAlphaBaseImageConvFlag に入る値  TRUE:変換する必要がある  FALSE:変換する必要は無い )
extern	int		Graphics_D3D11_CheckRequiredNormalImageConv_BaseImageFormat_PF(
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
		if( ( RgbBaseImageFormat == DX_BASEIMAGE_FORMAT_DXT1 && GD3D11.Device.Caps.TextureFormat[ Orig->ColorFormat ] != D_DXGI_FORMAT_BC1_UNORM ) ||
			( RgbBaseImageFormat == DX_BASEIMAGE_FORMAT_DXT2                                                                                     ) ||
			( RgbBaseImageFormat == DX_BASEIMAGE_FORMAT_DXT3 && GD3D11.Device.Caps.TextureFormat[ Orig->ColorFormat ] != D_DXGI_FORMAT_BC2_UNORM ) ||
			( RgbBaseImageFormat == DX_BASEIMAGE_FORMAT_DXT4                                                                                      ) ||
			( RgbBaseImageFormat == DX_BASEIMAGE_FORMAT_DXT5 && GD3D11.Device.Caps.TextureFormat[ Orig->ColorFormat ] != D_DXGI_FORMAT_BC3_UNORM ) )
		{
			RgbConvFlag = TRUE ;
		}

		if( AlphaBaseImageFormat >= 0 )
		{
			if( ( AlphaBaseImageFormat == DX_BASEIMAGE_FORMAT_DXT1 && GD3D11.Device.Caps.TextureFormat[ Orig->ColorFormat ] != D_DXGI_FORMAT_BC1_UNORM ) ||
				( AlphaBaseImageFormat == DX_BASEIMAGE_FORMAT_DXT2                                                                                     ) ||
				( AlphaBaseImageFormat == DX_BASEIMAGE_FORMAT_DXT3 && GD3D11.Device.Caps.TextureFormat[ Orig->ColorFormat ] != D_DXGI_FORMAT_BC2_UNORM ) ||
				( AlphaBaseImageFormat == DX_BASEIMAGE_FORMAT_DXT4                                                                                     ) ||
				( AlphaBaseImageFormat == DX_BASEIMAGE_FORMAT_DXT5 && GD3D11.Device.Caps.TextureFormat[ Orig->ColorFormat ] != D_DXGI_FORMAT_BC3_UNORM ) )
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

// テクスチャ画像転送用の共有メモリのセットアップを行う
extern int Graphics_D3D11_Texture_SetupCommonBuffer( unsigned int Size )
{
	// 既に指定のサイズを確保してあったら何もせずに終了
	if( GD3D11.Texture.CommonBufferSize >= Size )
	{
		return 0 ;
	}

	// 既存のメモリを解放
	DXFREE( GD3D11.Texture.CommonBuffer ) ;
	GD3D11.Texture.CommonBuffer     = NULL ;
	GD3D11.Texture.CommonBufferSize = 0 ;

	// メモリの確保
	GD3D11.Texture.CommonBuffer = DXALLOC( Size ) ;
	if( GD3D11.Texture.CommonBuffer == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6b\x30\x3b\x75\xcf\x50\x92\x30\xe2\x8e\x01\x90\x59\x30\x8b\x30\x5f\x30\x81\x30\x6e\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x5b\x00\x47\x00\x72\x00\x61\x00\x70\x00\x68\x00\x69\x00\x63\x00\x73\x00\x5f\x00\x44\x00\x33\x00\x44\x00\x31\x00\x31\x00\x5f\x00\x54\x00\x65\x00\x78\x00\x74\x00\x75\x00\x72\x00\x65\x00\x5f\x00\x53\x00\x65\x00\x74\x00\x75\x00\x70\x00\x43\x00\x6f\x00\x6d\x00\x6d\x00\x6f\x00\x6e\x00\x42\x00\x75\x00\x66\x00\x66\x00\x65\x00\x72\x00\x5d\x00\x0a\x00\x00"/*@ L"テクスチャに画像を転送するためのメモリの確保に失敗しました[Graphics_D3D11_Texture_SetupCommonBuffer]\n" @*/ )) ;
		return -1 ;
	}

	// 確保したメモリのサイズを保存
	GD3D11.Texture.CommonBufferSize = Size ;

	// 終了
	return 0 ;
}

// テクスチャ画像転送用の共有メモリの後始末を行う
extern int Graphics_D3D11_Texture_TerminateCommonBuffer( void )
{
	// メモリを確保していたら解放
	if( GD3D11.Texture.CommonBuffer != NULL )
	{
		DXFREE( GD3D11.Texture.CommonBuffer ) ;
		GD3D11.Texture.CommonBuffer = NULL ;
	}
	GD3D11.Texture.CommonBufferSize = 0 ;

	// 終了
	return 0 ;
}

// 単純転送処理の初期化を行う
extern int Graphics_D3D11_StretchRect_Initialize( void )
{
	HRESULT hr ;
	int i ;

	DXST_LOGFILE_ADDUTF16LE( "\x3b\x75\xcf\x50\x6e\x30\x58\x53\x14\x7d\xe2\x8e\x01\x90\xe6\x51\x06\x74\x6e\x30\x1d\x52\x1f\x67\x16\x53\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"画像の単純転送処理の初期化... " @*/ ) ;

	// ブレンドステートの作成
	if( GD3D11.Device.Shader.Base.StretchRect_BlendState[ 0 ] == NULL )
	{
		D_D3D11_BLEND_DESC BlendDesc ;
		static D_D3D11_RENDER_TARGET_BLEND_DESC RenderTargetBlendDesc_NoBlend = 
		{
			FALSE,
			D_D3D11_BLEND_ONE,
			D_D3D11_BLEND_ZERO,
			D_D3D11_BLEND_OP_ADD,
			D_D3D11_BLEND_ONE,
			D_D3D11_BLEND_ZERO,
			D_D3D11_BLEND_OP_ADD,
			D_D3D11_COLOR_WRITE_ENABLE_ALL
		} ;

		static D_D3D11_RENDER_TARGET_BLEND_DESC RenderTargetBlendDesc_AlphaBlend = 
		{
			TRUE,
			D_D3D11_BLEND_SRC_ALPHA,
			D_D3D11_BLEND_INV_SRC_ALPHA,
			D_D3D11_BLEND_OP_ADD,
			D_D3D11_BLEND_SRC_ALPHA,
			D_D3D11_BLEND_INV_SRC_ALPHA,
			D_D3D11_BLEND_OP_ADD,
			D_D3D11_COLOR_WRITE_ENABLE_ALL
		} ;

		BlendDesc.AlphaToCoverageEnable  = FALSE ;
		BlendDesc.IndependentBlendEnable = FALSE ;
		for( i = 0; i < 8; i++ )
		{
			BlendDesc.RenderTarget[ i ] = RenderTargetBlendDesc_NoBlend ;
		}

		hr = D3D11Device_CreateBlendState( &BlendDesc, &GD3D11.Device.Shader.Base.StretchRect_BlendState[ 0 ] ) ;
		if( hr != S_OK )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x58\x53\x14\x7d\xe2\x8e\x01\x90\x28\x75\x6e\x30\x20\x00\x49\x00\x44\x00\x33\x00\x44\x00\x31\x00\x31\x00\x42\x00\x6c\x00\x65\x00\x6e\x00\x64\x00\x53\x00\x74\x00\x61\x00\x74\x00\x65\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"単純転送用の ID3D11BlendState の作成に失敗しました\n" @*/ )) ;
			return -1 ;
		}

		BlendDesc.RenderTarget[ 0 ] = RenderTargetBlendDesc_AlphaBlend ;
		hr = D3D11Device_CreateBlendState( &BlendDesc, &GD3D11.Device.Shader.Base.StretchRect_BlendState[ 1 ] ) ;
		if( hr != S_OK )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x58\x53\x14\x7d\xa2\x30\xeb\x30\xd5\x30\xa1\x30\xd6\x30\xec\x30\xf3\x30\xc9\x30\xe2\x8e\x01\x90\x28\x75\x6e\x30\x20\x00\x49\x00\x44\x00\x33\x00\x44\x00\x31\x00\x31\x00\x42\x00\x6c\x00\x65\x00\x6e\x00\x64\x00\x53\x00\x74\x00\x61\x00\x74\x00\x65\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"単純アルファブレンド転送用の ID3D11BlendState の作成に失敗しました\n" @*/ )) ;
			return -1 ;
		}
	}

	// サンプラーステートの作成
	{
		static D_D3D11_SAMPLER_DESC SamplerDesc =
		{
			D_D3D11_FILTER_MIN_MAG_MIP_POINT,
			D_D3D11_TEXTURE_ADDRESS_CLAMP,
			D_D3D11_TEXTURE_ADDRESS_CLAMP,
			D_D3D11_TEXTURE_ADDRESS_CLAMP,
			0.0f,
			16,
			D_D3D11_COMPARISON_ALWAYS,
			{ 0.0f, 0.0f, 0.0f, 0.0f },
			-D_D3D11_FLOAT32_MAX,
			 D_D3D11_FLOAT32_MAX
		} ;

		if( GD3D11.Device.Shader.Base.StretchRect_SamplerState[ 0 ] == NULL )
		{
			SamplerDesc.Filter = D_D3D11_FILTER_MIN_MAG_MIP_POINT ;
			hr = D3D11Device_CreateSamplerState( &SamplerDesc, &GD3D11.Device.Shader.Base.StretchRect_SamplerState[ 0 ] ) ;
			if( hr != S_OK )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x00\x67\xd1\x8f\xb9\x70\xb5\x30\xf3\x30\xd7\x30\xea\x30\xf3\x30\xb0\x30\x58\x53\x14\x7d\xe2\x8e\x01\x90\x28\x75\x6e\x30\x20\x00\x49\x00\x44\x00\x33\x00\x44\x00\x31\x00\x31\x00\x53\x00\x61\x00\x6d\x00\x70\x00\x6c\x00\x65\x00\x72\x00\x53\x00\x74\x00\x61\x00\x74\x00\x65\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"最近点サンプリング単純転送用の ID3D11SamplerState の作成に失敗しました\n" @*/ )) ;
				return -1 ;
			}
		}

		if( GD3D11.Device.Shader.Base.StretchRect_SamplerState[ 1 ] == NULL )
		{
			SamplerDesc.Filter = D_D3D11_FILTER_MIN_MAG_MIP_LINEAR ;
			hr = D3D11Device_CreateSamplerState( &SamplerDesc, &GD3D11.Device.Shader.Base.StretchRect_SamplerState[ 1 ] ) ;
			if( hr != S_OK )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\xd0\x30\xa4\x30\xea\x30\xcb\x30\xa2\x30\xdc\x88\x93\x95\x58\x53\x14\x7d\xe2\x8e\x01\x90\x28\x75\x6e\x30\x20\x00\x49\x00\x44\x00\x33\x00\x44\x00\x31\x00\x31\x00\x53\x00\x61\x00\x6d\x00\x70\x00\x6c\x00\x65\x00\x72\x00\x53\x00\x74\x00\x61\x00\x74\x00\x65\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"バイリニア補間単純転送用の ID3D11SamplerState の作成に失敗しました\n" @*/ )) ;
				return -1 ;
			}
		}
	}

	// ラスタライザーステートの作成
	if( GD3D11.Device.Shader.Base.StretchRect_RasterizerState == NULL )
	{
		static D_D3D11_RASTERIZER_DESC RasterizerDesc =
		{
			D_D3D11_FILL_SOLID,
			D_D3D11_CULL_BACK,
			0,
			0,
			0.0f,
			0.0f,
			TRUE,
			FALSE,
			FALSE,
			FALSE
		} ;

		hr = D3D11Device_CreateRasterizerState( &RasterizerDesc, &GD3D11.Device.Shader.Base.StretchRect_RasterizerState ) ;
		if( hr != S_OK )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x58\x53\x14\x7d\xe2\x8e\x01\x90\x28\x75\x6e\x30\x20\x00\x49\x00\x44\x00\x33\x00\x44\x00\x31\x00\x31\x00\x52\x00\x61\x00\x73\x00\x74\x00\x65\x00\x72\x00\x69\x00\x7a\x00\x65\x00\x72\x00\x53\x00\x74\x00\x61\x00\x74\x00\x65\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"単純転送用の ID3D11RasterizerState の作成に失敗しました\n" @*/ )) ;
			return -1 ;
		}
	}

	// 深度ステンシルステートの作成
	if( GD3D11.Device.Shader.Base.StretchRect_DepthStencilState == NULL )
	{
		static D_D3D11_DEPTH_STENCIL_DESC DepthStencilDesc =
		{
			FALSE,
			D_D3D11_DEPTH_WRITE_MASK_ZERO,
			D_D3D11_COMPARISON_ALWAYS,
			FALSE,
			D_D3D11_DEFAULT_STENCIL_READ_MASK,
			D_D3D11_DEFAULT_STENCIL_WRITE_MASK,
			{
				D_D3D11_STENCIL_OP_KEEP,
				D_D3D11_STENCIL_OP_KEEP,
				D_D3D11_STENCIL_OP_KEEP,
				D_D3D11_COMPARISON_ALWAYS
			},
			{
				D_D3D11_STENCIL_OP_KEEP,
				D_D3D11_STENCIL_OP_KEEP,
				D_D3D11_STENCIL_OP_KEEP,
				D_D3D11_COMPARISON_ALWAYS
			}
		} ;

		// よく使う設定の深度ステンシルステートではない場合はここで深度ステンシルステートを作成する
		hr = D3D11Device_CreateDepthStencilState( &DepthStencilDesc, &GD3D11.Device.Shader.Base.StretchRect_DepthStencilState ) ;
		if( hr != S_OK )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x58\x53\x14\x7d\xe2\x8e\x01\x90\x28\x75\x6e\x30\x20\x00\x49\x00\x44\x00\x33\x00\x44\x00\x31\x00\x31\x00\x44\x00\x65\x00\x70\x00\x74\x00\x68\x00\x53\x00\x74\x00\x65\x00\x6e\x00\x63\x00\x69\x00\x6c\x00\x53\x00\x74\x00\x61\x00\x74\x00\x65\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"単純転送用の ID3D11DepthStencilState の作成に失敗しました\n" @*/ )) ;
			return -1 ;
		}
	}

	DXST_LOGFILE_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"成功\n" @*/ ) ;

	// 正常終了
	return 0 ;
}

// 単純転送処理の後始末を行う
extern int Graphics_D3D11_StretchRect_Terminate( void )
{
	int i ;

	for( i = 0 ; i < 2 ; i ++ )
	{
		if( GD3D11.Device.Shader.Base.StretchRect_SamplerState[ i ] != NULL )
		{
			Direct3D11_Release_SamplerState( GD3D11.Device.Shader.Base.StretchRect_SamplerState[ i ] ) ;
			GD3D11.Device.Shader.Base.StretchRect_SamplerState[ i ] = NULL ;
		}
	}

	if( GD3D11.Device.Shader.Base.StretchRect_RasterizerState != NULL )
	{
		Direct3D11_Release_RasterizerState( GD3D11.Device.Shader.Base.StretchRect_RasterizerState ) ;
		GD3D11.Device.Shader.Base.StretchRect_RasterizerState = NULL ;
	}

	if( GD3D11.Device.Shader.Base.StretchRect_DepthStencilState != NULL )
	{
		Direct3D11_Release_DepthStencilState( GD3D11.Device.Shader.Base.StretchRect_DepthStencilState ) ;
		GD3D11.Device.Shader.Base.StretchRect_DepthStencilState = NULL ;
	}

	for( i = 0 ; i < 2 ; i ++ )
	{
		if( GD3D11.Device.Shader.Base.StretchRect_BlendState[ i ] != NULL )
		{
			Direct3D11_Release_BlendState( GD3D11.Device.Shader.Base.StretchRect_BlendState[ i ] ) ;
			GD3D11.Device.Shader.Base.StretchRect_BlendState[ i ] = NULL ;
		}
	}

	// 正常終了
	return 0 ;
}

// レンダーターゲットテクスチャにテクスチャの単純転送を行う
extern int		Graphics_D3D11_StretchRect(
	D_ID3D11Texture2D *SrcTexture,  D_ID3D11ShaderResourceView *SrcTextureSRV,  const RECT *SrcRect,
	D_ID3D11Texture2D *DestTexture, D_ID3D11RenderTargetView   *DestTextureRTV, const RECT *DestRect,
	D_D3D11_FILTER_TYPE FilterType,
	int AlphaBlend,
	D_ID3D11VertexShader *VertexShader,
	D_ID3D11PixelShader  *PixelShader,
	D_ID3D11Texture2D *BlendTexture, D_ID3D11ShaderResourceView  *BlendTextureSRV, const RECT *BlendRect,
	VERTEX_TEX8_2D *Texcoord8Vertex
)
{
	float                  VertexData[ 4 ][ 6 ] ;
	float                  VertexDataTex8[ 4 ][ 18 ] ;
	D_D3D11_TEXTURE2D_DESC SrcDesc ;
	D_D3D11_TEXTURE2D_DESC DestDesc ;
	D_D3D11_TEXTURE2D_DESC BlendDesc = { 0 } ;
	D_D3D11_VIEWPORT       Viewport ;
	RECTF                  SrcRectF ;
	RECTF                  DestRectF ;
	RECTF                  BlendRectF = { 0.0f } ;
//	D_D3D11_BOX            DestBox ;
	UINT                   Offsets = 0 ;
	UINT                   VertexStride ;
	RECT                   SrcRectTemp ;
	RECT                   DestRectTemp ;
	RECT                   BlendRectTemp ;
	static FLOAT BlendFactor[ 4 ] = { 0.0f } ;

	DRAWSTOCKINFO

	// テクスチャ情報を取得
	SrcTexture->GetDesc(  &SrcDesc  ) ;
	DestTexture->GetDesc( &DestDesc ) ;
	if( BlendTexture != NULL )
	{
		BlendTexture->GetDesc( &BlendDesc ) ;
	}

	if( SrcRect == NULL )
	{
		SrcRectTemp.left   = 0 ;
		SrcRectTemp.top    = 0 ;
		SrcRectTemp.right  = ( LONG )SrcDesc.Width ;
		SrcRectTemp.bottom = ( LONG )SrcDesc.Height ;

		SrcRect = &SrcRectTemp ;
	}

	if( DestRect == NULL )
	{
		DestRectTemp.left   = 0 ;
		DestRectTemp.top    = 0 ;
		DestRectTemp.right  = ( LONG )DestDesc.Width ;
		DestRectTemp.bottom = ( LONG )DestDesc.Height ;

		DestRect = &DestRectTemp ;
	}

	// 頂点データを準備
	if( Texcoord8Vertex == NULL )
	{
		DestRectF.left   =     ( float )DestRect->left   / DestDesc.Width  * 2.0f - 1.0f   ;
		DestRectF.top    = - ( ( float )DestRect->top    / DestDesc.Height * 2.0f - 1.0f ) ;
		DestRectF.right  =     ( float )DestRect->right  / DestDesc.Width  * 2.0f - 1.0f   ;
		DestRectF.bottom = - ( ( float )DestRect->bottom / DestDesc.Height * 2.0f - 1.0f ) ;

		SrcRectF.left    = ( float )SrcRect->left    / SrcDesc.Width ;
		SrcRectF.top     = ( float )SrcRect->top     / SrcDesc.Height ;
		SrcRectF.right   = ( float )SrcRect->right   / SrcDesc.Width ;
		SrcRectF.bottom  = ( float )SrcRect->bottom  / SrcDesc.Height ;

		if( BlendTexture != NULL )
		{
			if( BlendRect == NULL )
			{
				BlendRectTemp.left   = 0 ;
				BlendRectTemp.top    = 0 ;
				BlendRectTemp.right  = ( LONG )BlendDesc.Width ;
				BlendRectTemp.bottom = ( LONG )BlendDesc.Height ;

				BlendRect = &BlendRectTemp ;
			}

			BlendRectF.left    = ( float )BlendRect->left    / BlendDesc.Width ;
			BlendRectF.top     = ( float )BlendRect->top     / BlendDesc.Height ;
			BlendRectF.right   = ( float )BlendRect->right   / BlendDesc.Width ;
			BlendRectF.bottom  = ( float )BlendRect->bottom  / BlendDesc.Height ;
		}

		VertexData[ 0 ][ 0 ] = DestRectF.left ;
		VertexData[ 0 ][ 1 ] = DestRectF.top ;
		VertexData[ 0 ][ 2 ] = SrcRectF.left ;
		VertexData[ 0 ][ 3 ] = SrcRectF.top ;

		VertexData[ 1 ][ 0 ] = DestRectF.right ;
		VertexData[ 1 ][ 1 ] = DestRectF.top ;
		VertexData[ 1 ][ 2 ] = SrcRectF.right ;
		VertexData[ 1 ][ 3 ] = SrcRectF.top ;

		VertexData[ 2 ][ 0 ] = DestRectF.left ;
		VertexData[ 2 ][ 1 ] = DestRectF.bottom ;
		VertexData[ 2 ][ 2 ] = SrcRectF.left ;
		VertexData[ 2 ][ 3 ] = SrcRectF.bottom ;

		VertexData[ 3 ][ 0 ] = DestRectF.right ;
		VertexData[ 3 ][ 1 ] = DestRectF.bottom ;
		VertexData[ 3 ][ 2 ] = SrcRectF.right ;
		VertexData[ 3 ][ 3 ] = SrcRectF.bottom ;

		if( BlendTexture != NULL )
		{
			VertexData[ 0 ][ 4 ] = BlendRectF.left ;
			VertexData[ 0 ][ 5 ] = BlendRectF.top ;

			VertexData[ 1 ][ 4 ] = BlendRectF.right ;
			VertexData[ 1 ][ 5 ] = BlendRectF.top ;

			VertexData[ 2 ][ 4 ] = BlendRectF.left ;
			VertexData[ 2 ][ 5 ] = BlendRectF.bottom ;

			VertexData[ 3 ][ 4 ] = BlendRectF.right ;
			VertexData[ 3 ][ 5 ] = BlendRectF.bottom ;
		}
		else
		{
			VertexData[ 0 ][ 4 ] = 0.0f ;
			VertexData[ 0 ][ 5 ] = 0.0f ;

			VertexData[ 1 ][ 4 ] = 0.0f ;
			VertexData[ 1 ][ 5 ] = 0.0f ;

			VertexData[ 2 ][ 4 ] = 0.0f ;
			VertexData[ 2 ][ 5 ] = 0.0f ;

			VertexData[ 3 ][ 4 ] = 0.0f ;
			VertexData[ 3 ][ 5 ] = 0.0f ;
		}
	}
	else
	{
		int i ;

		for( i = 0 ; i < 4 ; i ++ )
		{
			VertexDataTex8[ i ][  0 ] =     Texcoord8Vertex[ i ].pos.x / DestDesc.Width  * 2.0f - 1.0f   ;
			VertexDataTex8[ i ][  1 ] = - ( Texcoord8Vertex[ i ].pos.y / DestDesc.Height * 2.0f - 1.0f ) ;

			VertexDataTex8[ i ][  2 ] = Texcoord8Vertex[ i ].u0 ;
			VertexDataTex8[ i ][  3 ] = Texcoord8Vertex[ i ].v0 ;
			VertexDataTex8[ i ][  4 ] = Texcoord8Vertex[ i ].u1 ;
			VertexDataTex8[ i ][  5 ] = Texcoord8Vertex[ i ].v1 ;
			VertexDataTex8[ i ][  6 ] = Texcoord8Vertex[ i ].u2 ;
			VertexDataTex8[ i ][  7 ] = Texcoord8Vertex[ i ].v2 ;
			VertexDataTex8[ i ][  8 ] = Texcoord8Vertex[ i ].u3 ;
			VertexDataTex8[ i ][  9 ] = Texcoord8Vertex[ i ].v3 ;
			VertexDataTex8[ i ][ 10 ] = Texcoord8Vertex[ i ].u4 ;
			VertexDataTex8[ i ][ 11 ] = Texcoord8Vertex[ i ].v4 ;
			VertexDataTex8[ i ][ 12 ] = Texcoord8Vertex[ i ].u5 ;
			VertexDataTex8[ i ][ 13 ] = Texcoord8Vertex[ i ].v5 ;
			VertexDataTex8[ i ][ 14 ] = Texcoord8Vertex[ i ].u6 ;
			VertexDataTex8[ i ][ 15 ] = Texcoord8Vertex[ i ].v6 ;
			VertexDataTex8[ i ][ 16 ] = Texcoord8Vertex[ i ].u7 ;
			VertexDataTex8[ i ][ 17 ] = Texcoord8Vertex[ i ].v7 ;
		}
	}

	Graphics_D3D11_CommonVertexBuffer_Setup( Texcoord8Vertex != NULL ? sizeof( VertexDataTex8 ) : sizeof( VertexData ) ) ;

//	DestBox.left   = 0 ;
//	DestBox.top    = 0 ;
//	DestBox.front  = 0 ;
//	DestBox.right  = sizeof( VertexData ) ;
//	DestBox.bottom = 1 ;
//	DestBox.back   = 1 ;
//	D3D11DeviceContext_UpdateSubresource_ASync( GD3D11.Device.DrawInfo.CommonVertexBuffer, 0, &DestBox, VertexData, 0, 0 ) ;
	D_D3D11_MAPPED_SUBRESOURCE MappedSubResource ;
	HRESULT                    hr ;
	hr = D3D11DeviceContext_Map_ASync( 
		GD3D11.Device.DrawInfo.CommonVertexBuffer,
		0,
		D_D3D11_MAP_WRITE_DISCARD,
		0,
		&MappedSubResource
	) ;
	if( Texcoord8Vertex != NULL )
	{
		_MEMCPY( MappedSubResource.pData, VertexDataTex8, sizeof( VertexDataTex8 ) ) ;
	}
	else
	{
		_MEMCPY( MappedSubResource.pData, VertexData, sizeof( VertexData ) ) ;
	}
	D3D11DeviceContext_Unmap_ASync(
		GD3D11.Device.DrawInfo.CommonVertexBuffer,
		0
	) ;

	// レンダリングターゲットのセット
	D3D11DeviceContext_OMSetRenderTargets( 1, &DestTextureRTV, NULL ) ;

	// シェーダーリソースビューをセット
	D3D11DeviceContext_PSSetShaderResources( 0, 1, &SrcTextureSRV ) ;
	if( BlendTextureSRV != NULL )
	{
		D3D11DeviceContext_PSSetShaderResources( 1, 1, &BlendTextureSRV ) ;
	}

	// ビューポートのセット
	Viewport.TopLeftX = 0.0f ;
	Viewport.TopLeftY = 0.0f ;
	Viewport.Width    = ( float )DestDesc.Width ;
	Viewport.Height   = ( float )DestDesc.Height ;
	Viewport.MinDepth = 0.0f ;
	Viewport.MaxDepth = 1.0f ;
	D3D11DeviceContext_RSSetViewports( 1, &Viewport ) ;

	// サンプラーステートをセット
	D3D11DeviceContext_PSSetSamplers( 0, 1, &GD3D11.Device.Shader.Base.StretchRect_SamplerState[ FilterType == D_D3D11_FILTER_TYPE_POINT ? 0 : 1 ] ) ;
	if( BlendTexture != NULL )
	{
		D3D11DeviceContext_PSSetSamplers( 1, 1, &GD3D11.Device.Shader.Base.StretchRect_SamplerState[ FilterType == D_D3D11_FILTER_TYPE_POINT ? 0 : 1 ] ) ;
	}

	// ラスタライザーステートをセット
	D3D11DeviceContext_RSSetState( GD3D11.Device.Shader.Base.StretchRect_RasterizerState ) ;

	// 深度ステンシルステートをセット
	D3D11DeviceContext_OMSetDepthStencilState( GD3D11.Device.Shader.Base.StretchRect_DepthStencilState, 0 ) ;

	// ブレンドステートをセット
	D3D11DeviceContext_OMSetBlendState( GD3D11.Device.Shader.Base.StretchRect_BlendState[ AlphaBlend ? 1 : 0 ], BlendFactor, 0xffffffff ) ;

	// プリミティブタイプをセット
	D3D11DeviceContext_IASetPrimitiveTopology( D_D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP ) ;

	// 入力レイアウトをセット
	D3D11DeviceContext_IASetInputLayout( Texcoord8Vertex != NULL ? GD3D11.Device.InputLayout.BaseStretchRectTex8InputLayout : GD3D11.Device.InputLayout.BaseStretchRectInputLayout ) ;

	// ピクセルシェーダーをセット
	D3D11DeviceContext_PSSetShader( PixelShader  != NULL ? PixelShader  : GD3D11.Device.Shader.Base.StretchRect_PS, NULL, 0 ) ;

	// 頂点シェーダーをセット
	D3D11DeviceContext_VSSetShader( VertexShader != NULL ? VertexShader : ( Texcoord8Vertex != NULL ? GD3D11.Device.Shader.Base.StretchRectTex8_VS : GD3D11.Device.Shader.Base.StretchRect_VS ), NULL, 0 ) ;

	// 頂点バッファをセット
	VertexStride = Texcoord8Vertex != NULL ? sizeof( VertexDataTex8[ 0 ] ) : sizeof( VertexData[ 0 ] ) ;
	D3D11DeviceContext_IASetVertexBuffers( 0, 1, &GD3D11.Device.DrawInfo.CommonVertexBuffer, &VertexStride, &Offsets ) ;


	// 描画
	D3D11DeviceContext_Draw( 4, 0 ) ;
	GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;


	// 頂点バッファ設定を復帰
	D3D11DeviceContext_IASetVertexBuffers( 0, 1, &GD3D11.Device.State.SetVertexBuffer, ( UINT * )&GD3D11.Device.State.SetVertexBufferStride, &Offsets ) ;

	// セットする頂点シェーダーを復帰
	D3D11DeviceContext_VSSetShader( GD3D11.Device.State.SetVertexShader, NULL, 0 ) ;

	// セットするピクセルシェーダーを復帰
	D3D11DeviceContext_PSSetShader( GD3D11.Device.State.SetPixelShader, NULL, 0 ) ;

	// 入力レイアウト設定を復帰
	D3D11DeviceContext_IASetInputLayout( GD3D11.Device.State.SetVertexInputLayout ) ;

	// プリミティブタイプ設定の復帰
	D3D11DeviceContext_IASetPrimitiveTopology( GD3D11.Device.State.PrimitiveTopology ) ;

	// セットするブレンドステート復帰
	D3D11DeviceContext_OMSetBlendState( GD3D11.Device.State.BlendState, BlendFactor, 0xffffffff ) ;

	// セットする深度ステンシルステートを復帰
	D3D11DeviceContext_OMSetDepthStencilState( GD3D11.Device.State.DepthStencilState, 0 ) ;

	// セットするラスタライザーステートを復帰
	D3D11DeviceContext_RSSetState( GD3D11.Device.State.RasterizerState ) ;

	// セットするサンプラーステートを復帰
	D3D11DeviceContext_PSSetSamplers( 0, 1, &GD3D11.Device.State.SamplerState[ 0 ] ) ;
	if( BlendTexture != NULL )
	{
		D3D11DeviceContext_PSSetSamplers( 1, 1, &GD3D11.Device.State.SamplerState[ 1 ] ) ;
	}

	// セットするレンダリングターゲットの復帰
	Graphics_D3D11_DeviceState_SetRenderTarget_SizeCheck() ;

	// ビューポート設定の復帰
	D3D11DeviceContext_RSSetViewports( 1, &GD3D11.Device.State.Viewport ) ;

	// セットするシェーダーリソースビューを復帰
	D3D11DeviceContext_PSSetShaderResources( 0, 1, &GD3D11.Device.State.PSSetShaderResourceView[ 0 ] ) ;
	if( BlendTextureSRV != NULL )
	{
		D3D11DeviceContext_PSSetShaderResources( 1, 1, &GD3D11.Device.State.PSSetShaderResourceView[ 1 ] ) ;
	}

	return 0 ;
}

// 指定のテクスチャの内容に読み込みアクセスできるようにする
extern	int		Graphics_D3D11_Texture2D_Map(
	D_ID3D11Texture2D *				TargetTexture,
	D_ID3D11ShaderResourceView *	/*TargetTextureSRV*/,
	int								TargetTextureArraySlice,
	int								TargetTextureMipSlice,
	const RECT *					MapRect /* NULL で全体 */,
	BASEIMAGE *						MapBaseImage,
	D_ID3D11Texture2D **			MapTempTexture
)
{
	D_D3D11_TEXTURE2D_DESC			TargetTexture2DDesc ;
	D_D3D11_TEXTURE2D_DESC			Texture2DDesc ;
//	D_D3D11_RENDER_TARGET_VIEW_DESC	RTVDesc ;
	D_D3D11_MAPPED_SUBRESOURCE		MappedSubresource ;
	D_ID3D11Texture2D				*DownSampleTexture   = NULL ;
//	D_ID3D11RenderTargetView		*DrawValidTextureRTV = NULL ;
	COLORDATA						*TargetColorData ;
	RECT							TempRect ;
	int								TextureWidth ;
	int								TextureHeight ;
	int								Width ;
	int								Height ;
	HRESULT							hr ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 情報を取得
	TargetTexture->GetDesc( &TargetTexture2DDesc ) ;
	TargetColorData = Graphics_D3D11_GetD3DFormatColorData( TargetTexture2DDesc.Format ) ;

	TextureWidth  = TargetTexture2DDesc.Width ;
	TextureHeight = TargetTexture2DDesc.Height ;
	if( TargetTextureMipSlice > 0 )
	{
		Graphics_Image_CalcMipSize( TextureWidth, TextureHeight, TargetTextureMipSlice, &TextureWidth, &TextureHeight ) ;
	}

	if( MapRect == NULL )
	{
		TempRect.left   = 0 ;
		TempRect.top    = 0 ;
		TempRect.right  = ( LONG )TextureWidth ;
		TempRect.bottom = ( LONG )TextureHeight ;
	}
	else
	{
		TempRect = *MapRect ;
	}

	Width  = TempRect.right  - TempRect.left ;
	Height = TempRect.bottom - TempRect.top ;

	*MapTempTexture = NULL ;

	// ロック対象がマルチサンプル対応か、配列テクスチャ(キューブマップなど)か、
	// ミップマップ付きの場合は同サイズのミップマップなし非マルチサンプルテクスチャに転送
	if( TargetTexture2DDesc.SampleDesc.Count != 1 ||
		TargetTexture2DDesc.MipLevels        != 1 ||
		TargetTexture2DDesc.ArraySize        != 1 )
	{
		_MEMSET( &Texture2DDesc, 0, sizeof( Texture2DDesc ) ) ;
		Texture2DDesc.Usage              = D_D3D11_USAGE_DEFAULT ;
		Texture2DDesc.Format             = TargetTexture2DDesc.Format ;
		Texture2DDesc.BindFlags          = 0 ;
		Texture2DDesc.Width              = TextureWidth ;
		Texture2DDesc.Height             = TextureHeight ;
		Texture2DDesc.CPUAccessFlags     = 0 ;
		Texture2DDesc.MipLevels          = 1 ;
		Texture2DDesc.ArraySize          = 1 ;
		Texture2DDesc.SampleDesc.Count   = 1 ;
		Texture2DDesc.SampleDesc.Quality = 0 ;
		hr = D3D11Device_CreateTexture2D_ASync( &Texture2DDesc, NULL, &DownSampleTexture ) ;
		if( FAILED( hr ) )
		{
			DXST_LOGFILE_ADDUTF16LE( "\xc0\x30\xa6\x30\xf3\x30\xb5\x30\xf3\x30\xd7\x30\xea\x30\xf3\x30\xb0\x30\x28\x75\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ダウンサンプリング用テクスチャの作成に失敗しました\n" @*/ ) ;
			goto ERR ;
		}

		// 転送
		D3D11DeviceContext_ResolveSubresource(
			DownSampleTexture,
			D_D3D11CalcSubresource( 0, 0, 1 ),
			TargetTexture,
			D_D3D11CalcSubresource( TargetTextureMipSlice, TargetTextureArraySlice, TargetTexture2DDesc.MipLevels ),
			TargetTexture2DDesc.Format
		) ;

//		_MEMSET( &RTVDesc, 0, sizeof( RTVDesc ) );
//		RTVDesc.Format        = TargetTexture2DDesc.Format ;
//		RTVDesc.ViewDimension = D_D3D11_RTV_DIMENSION_TEXTURE2D ;
//		hr = D3D11Device_CreateRenderTargetView( DrawValidTexture, &RTVDesc, &DrawValidTextureRTV ) ;
//		if( FAILED( hr ) )
//		{
//			DXST_LOGFILEFMT_ADDUTF16LE(( "\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\xfc\x30\xde\x30\xc3\x30\xd7\x30\x28\x75\xcf\x63\x3b\x75\xef\x53\xfd\x80\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x28\x75\xec\x30\xf3\x30\xc0\x30\xfc\x30\xbf\x30\xfc\x30\xb2\x30\xc3\x30\xc8\x30\xd3\x30\xe5\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"テクスチャーマップ用描画可能テクスチャ用レンダーターゲットビューの作成に失敗しました" @*/ )) ;
//			goto ERR ;
//		}
//
//		// 転送
//		Graphics_D3D11_StretchRect(
//			TargetTexture,    TargetTextureSRV,    NULL,
//			DrawValidTexture, DrawValidTextureRTV, NULL,
//			D_D3D11_FILTER_TYPE_LINEAR
//		) ;
	}

	// 転送用テクスチャに描画対象画像を転送
	{
		_MEMSET( &Texture2DDesc, 0, sizeof( Texture2DDesc ) ) ;
		Texture2DDesc.Usage              = D_D3D11_USAGE_STAGING ;
		Texture2DDesc.Format             = TargetTexture2DDesc.Format ;
		Texture2DDesc.BindFlags          = 0 ;
		Texture2DDesc.Width              = TextureWidth ;
		Texture2DDesc.Height             = TextureHeight ;
		Texture2DDesc.CPUAccessFlags     = D_D3D11_CPU_ACCESS_READ ;
		Texture2DDesc.MipLevels          = 1 ;
		Texture2DDesc.ArraySize          = 1 ;
		Texture2DDesc.SampleDesc.Count   = 1 ;
		Texture2DDesc.SampleDesc.Quality = 0 ;
		hr = D3D11Device_CreateTexture2D_ASync( &Texture2DDesc, NULL, MapTempTexture ) ;
		if( FAILED( hr ) )
		{
			DXST_LOGFILE_ADDUTF16LE( "\xcf\x63\x3b\x75\xfe\x5b\x61\x8c\xd0\x30\xc3\x30\xd5\x30\xa1\x30\xad\x8a\x7f\x30\xd6\x53\x8a\x30\x28\x75\x6e\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"描画対象バッファ読み取り用のテクスチャの作成に失敗しました\n" @*/ ) ;
			goto ERR ;
		}

		D3D11DeviceContext_CopyResource( *MapTempTexture, DownSampleTexture ? DownSampleTexture : TargetTexture ) ;

		// マップ
		hr = D3D11DeviceContext_Map( *MapTempTexture, D_D3D11CalcSubresource( 0, 0, 1 ), D_D3D11_MAP_READ, 0, &MappedSubresource ) ;
		if( FAILED( hr ) )
		{
			DXST_LOGFILE_ADDUTF16LE( "\xcf\x63\x3b\x75\xfe\x5b\x61\x8c\xd0\x30\xc3\x30\xd5\x30\xa1\x30\xad\x8a\x7f\x30\xd6\x53\x8a\x30\x28\x75\x6e\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\xde\x30\xc3\x30\xd4\x30\xf3\x30\xb0\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"描画対象バッファ読み取り用のテクスチャのマッピングに失敗しました\n" @*/ ) ;
			goto ERR ;
		}
	}

//	// 描画用テクスチャを使用した場合はここで解放
//	if( DrawValidTextureRTV != NULL )
//	{
//		Direct3D11_Release_RenderTargetView( DrawValidTextureRTV ) ;
//		DrawValidTextureRTV = NULL ;
//	}

	// ダウンサンプリング用テクスチャを使用した場合はここで解放
	if( DownSampleTexture != NULL )
	{
		Direct3D11_Release_Texture2D( DownSampleTexture ) ;
		DownSampleTexture = NULL ;
	}

	// 基本イメージ構造体のデータにセット
	MapBaseImage->ColorData      = *TargetColorData ;
	MapBaseImage->Width          = Width ;
	MapBaseImage->Height         = Height ;
	MapBaseImage->Pitch          = ( int )MappedSubresource.RowPitch ;
	MapBaseImage->GraphData      = ( BYTE * )MappedSubresource.pData + TempRect.left * TargetColorData->PixelByte + TempRect.top * MappedSubresource.RowPitch ;
	MapBaseImage->MipMapCount    = 0 ;
	MapBaseImage->GraphDataCount = 0 ;

	// 正常終了
	return 0 ;

	// エラー処理
ERR :

	if( *MapTempTexture != NULL )
	{
		Direct3D11_Release_Texture2D( *MapTempTexture ) ;
		*MapTempTexture = NULL ;
	}

//	if( DrawValidTextureRTV != NULL )
//	{
//		Direct3D11_Release_RenderTargetView( DrawValidTextureRTV ) ;
//		DrawValidTextureRTV = NULL ;
//	}

	if( DownSampleTexture != NULL )
	{
		Direct3D11_Release_Texture2D( DownSampleTexture ) ;
		DownSampleTexture = NULL ;
	}

	// エラー終了
	return -1 ;
}

// 指定のテクスチャの内容への読み込みアクセスを終了する
extern	int		Graphics_D3D11_Texture2D_Unmap( D_ID3D11Texture2D *MapTempTexture )
{
	if( MapTempTexture != NULL )
	{
		Direct3D11_Release_Texture2D( MapTempTexture ) ;
		MapTempTexture = NULL ;
	}

	// 終了
	return 0 ;
}













































// D3D11Device の初期化関係

// D3D11Device オブジェクトの作成( 0:成功  -1:失敗 )
extern	int		Graphics_D3D11_Device_Create( void )
{
	const char *String = NULL ;

	// 既に作成済みの場合は一度削除する
	D3D11Device_Release() ;
	GD3D11.Device.DrawInfo.BeginSceneFlag = FALSE ;

	// DXGIFactory の作成
	if( DXGIFactory_Create() < 0 )
	{
		return -1 ;
	}

	// アダプター情報の列挙
	if( EnumDXGIAdapterInfo() < 0 )
	{
		return -1 ;
	}

	// デバイスの作成
	if( D3D11_CreateDevice() < 0 )
	{
		return -1 ;
	}

	// メインスワップチェインの作成
	if( Graphics_D3D11_OutputWindow_Add( GetDisplayWindowHandle(), TRUE ) < 0 )
	{
		return -1 ;
	}
	GD3D11.Device.Screen.TargetOutputWindow = 0 ;

	// フルスクリーンモードの場合はデスクトップ上の位置を取得しておく
	if( NS_GetWindowModeFlag() == FALSE )
	{
		GSYS.Screen.FullScreenDesktopRect = GSYS.Screen.DisplayInfo[ GSYS.Screen.ValidUseDisplayIndex ? GSYS.Screen.UseDisplayIndex : 0 ].DesktopRect ;
	}

	// 減算ブレンドが可能かどうかをセットする
	GSYS.HardInfo.ValidSubBlend = TRUE ;

	// テクスチャが正方形である必要があるかどうかを取得する
	GSYS.HardInfo.TextureSquareOnly = FALSE ;

	// 一度に使用することの出来る最大頂点数を取得する
	GSYS.HardInfo.MaxVertexIndex = 0xffff ;

	// 頂点座標タイプは DX_DRAWFLOATCOORDTYPE_DIRECT3D10
	GSYS.HardInfo.DrawFloatCoordType = DX_DRAWFLOATCOORDTYPE_DIRECT3D10 ;

	switch( GD3D11.Setting.FeatureLevel )
	{
	case D_D3D_FEATURE_LEVEL_11_0 :
	case D_D3D_FEATURE_LEVEL_11_1 :
		// 同時にレンダリングできるターゲットの数を保存
		GSYS.HardInfo.RenderTargetNum = 8 ;

		// テクスチャの最大幅と最大高さを保存する
		GSYS.HardInfo.MaxTextureWidth  = 16384 ;
		GSYS.HardInfo.MaxTextureHeight = GSYS.HardInfo.MaxTextureWidth ;

		// テクスチャのサイズが２のｎ乗である必要は無い
		GSYS.HardInfo.TextureSizePow2 = FALSE ;

		// 条件付でテクスチャのサイズが２のｎ乗ではなくても大丈夫
		GSYS.HardInfo.TextureSizeNonPow2Conditional = TRUE ;

		// 頂点カラーはBGRAカラータイプを使用する
		GSYS.HardInfo.UseVertexColorBGRAFormat = TRUE ;

		// 一度に描画できる最大プリミティブ数を取得する
		GSYS.HardInfo.MaxPrimitiveCount = 0x7fffffff ;
		break ;

	case D_D3D_FEATURE_LEVEL_10_0 :
	case D_D3D_FEATURE_LEVEL_10_1 :
		// 同時にレンダリングできるターゲットの数を保存
		GSYS.HardInfo.RenderTargetNum = 8 ;

		// テクスチャの最大幅と最大高さを保存する
		GSYS.HardInfo.MaxTextureWidth  = 8192 ;
		GSYS.HardInfo.MaxTextureHeight = GSYS.HardInfo.MaxTextureWidth ;

		// テクスチャのサイズが２のｎ乗である必要は無い
		GSYS.HardInfo.TextureSizePow2 = FALSE ;

		// 条件付でテクスチャのサイズが２のｎ乗ではなくても大丈夫
		GSYS.HardInfo.TextureSizeNonPow2Conditional = TRUE ;

		// 頂点カラーはBGRAカラータイプを使用する
		GSYS.HardInfo.UseVertexColorBGRAFormat = TRUE ;

		// 一度に描画できる最大プリミティブ数を取得する
		GSYS.HardInfo.MaxPrimitiveCount = 0x7fffffff ;
		break ;

	case D_D3D_FEATURE_LEVEL_9_1 :
	case D_D3D_FEATURE_LEVEL_9_2 :
	case D_D3D_FEATURE_LEVEL_9_3 :
		// 同時にレンダリングできるターゲットの数を保存
		GSYS.HardInfo.RenderTargetNum = GD3D11.Setting.FeatureLevel == D_D3D_FEATURE_LEVEL_9_3 ? 4 : 1 ;

		// テクスチャの最大幅と最大高さを保存する
		GSYS.HardInfo.MaxTextureWidth  = GD3D11.Setting.FeatureLevel == D_D3D_FEATURE_LEVEL_9_3 ? 4096 : 2048 ;
		GSYS.HardInfo.MaxTextureHeight = GSYS.HardInfo.MaxTextureWidth ;

		// 条件付でテクスチャのサイズが２のｎ乗ではなくても大丈夫
		GSYS.HardInfo.TextureSizeNonPow2Conditional = TRUE ;

		// 頂点カラーはBGRAカラータイプを使用する
		GSYS.HardInfo.UseVertexColorBGRAFormat = TRUE ;

		// テクスチャのサイズが２のｎ乗である必要がある
//		GSYS.HardInfo.TextureSizePow2 = TRUE ;

		// 条件付でテクスチャのサイズが２のｎ乗ではなくても大丈夫ではない
//		GSYS.HardInfo.TextureSizeNonPow2Conditional = FALSE ;

		// ピクセル単位ライティングは無効
		GD3D11.UsePixelLightingShader = FALSE ;

		// 頂点カラーはBGRAカラータイプを使用しない
		GSYS.HardInfo.UseVertexColorBGRAFormat = FALSE ;

		// 一度に描画できる最大プリミティブ数を取得する
		GSYS.HardInfo.MaxPrimitiveCount = GD3D11.Setting.FeatureLevel == D_D3D_FEATURE_LEVEL_9_1 ? 65535 : 1048575 ;
		break ;
	}

	// サイズの小さいほうを最大サイズとする
	GSYS.HardInfo.MaxTextureSize = GSYS.HardInfo.MaxTextureWidth < GSYS.HardInfo.MaxTextureHeight ? GSYS.HardInfo.MaxTextureWidth : GSYS.HardInfo.MaxTextureHeight ;

	// レンダーターゲットの数を調整
	if( GSYS.HardInfo.RenderTargetNum > DX_RENDERTARGET_COUNT )
	{
		GSYS.HardInfo.RenderTargetNum = DX_RENDERTARGET_COUNT ;
	}

	// Graphics_D3D11_DrawPreparation を行うべきフラグを立てる
	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// ドライバ情報出力
	const WCHAR *Description = GD3D11.Adapter.Info[ 0 ].Desc.Description ;
	DXST_LOGFILEFMT_ADDW(( L"Graphics Device:%s", Description )) ;

	// 各種フォーマットを調べる
	{
		UINT FormatSupport ;

		// 画面のフォーマットの取得
		GD3D11.Device.Caps.ScreenFormat = DIRECT3D11_BACKBUFFER_FORMAT ;
		String = "DXGI_FORMAT_R8G8B8A8_UNORM" ;
		DXST_LOGFILEFMT_ADDA(( "\x89\xe6\x96\xca\x82\xcc\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "画面のフォーマットは %s です" @*/, String )) ;

		// 16ビット深度バッファフォーマットの設定
		GD3D11.Device.Caps.DepthBufferTexture2DFormat[ ZBUFFER_FORMAT_16BIT ]    = DIRECT3D11_DEPTHBUFFER_TEXTURE2D_FORMAT ;
		GD3D11.Device.Caps.DepthBufferDepthStencilFormat[ ZBUFFER_FORMAT_16BIT ] = DIRECT3D11_DEPTHBUFFER_DEPTHSTENCIL_FORMAT ;
		String = "DXGI_FORMAT_D16_UNORM" ;
		DXST_LOGFILEFMT_ADDA(( "16bit Z\x83\x6f\x83\x62\x83\x74\x83\x40\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "16bit Zバッファフォーマットは %s です" @*/, String )) ;

		// 24ビット深度バッファフォーマットの選定
		if( /*D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R24G8_TYPELESS, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_RENDER_TARGET ) != 0 &&*/
			D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_D24_UNORM_S8_UINT, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_DEPTH_STENCIL ) != 0 )
		{
			GD3D11.Device.Caps.DepthBufferTexture2DFormat[ ZBUFFER_FORMAT_24BIT ]    = D_DXGI_FORMAT_R24G8_TYPELESS ;
			GD3D11.Device.Caps.DepthBufferDepthStencilFormat[ ZBUFFER_FORMAT_24BIT ] = D_DXGI_FORMAT_D24_UNORM_S8_UINT ;
			String = "DXGI_FORMAT_D24_UNORM_S8_UINT" ;
		}
		else
		{
			GD3D11.Device.Caps.DepthBufferTexture2DFormat[ ZBUFFER_FORMAT_24BIT ]    = GD3D11.Device.Caps.DepthBufferTexture2DFormat[ ZBUFFER_FORMAT_16BIT ] ;
			GD3D11.Device.Caps.DepthBufferDepthStencilFormat[ ZBUFFER_FORMAT_24BIT ] = GD3D11.Device.Caps.DepthBufferDepthStencilFormat[ ZBUFFER_FORMAT_16BIT ] ;
		}
		DXST_LOGFILEFMT_ADDA(( "24bit Z\x83\x6f\x83\x62\x83\x74\x83\x40\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "24bit Zバッファフォーマットは %s です" @*/, String )) ;

		// 32ビット深度バッファフォーマットの設定
		if( /*D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R32_TYPELESS, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_RENDER_TARGET ) != 0 &&*/
			D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_D32_FLOAT, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_DEPTH_STENCIL ) != 0 )
		{
			GD3D11.Device.Caps.DepthBufferTexture2DFormat[ ZBUFFER_FORMAT_32BIT ]    = D_DXGI_FORMAT_R32_TYPELESS ;
			GD3D11.Device.Caps.DepthBufferDepthStencilFormat[ ZBUFFER_FORMAT_32BIT ] = D_DXGI_FORMAT_D32_FLOAT ;
			String = "DXGI_FORMAT_D32_FLOAT" ;
		}
		else
		{
			GD3D11.Device.Caps.DepthBufferTexture2DFormat[ ZBUFFER_FORMAT_32BIT ]    = GD3D11.Device.Caps.DepthBufferTexture2DFormat[ ZBUFFER_FORMAT_24BIT ] ;
			GD3D11.Device.Caps.DepthBufferDepthStencilFormat[ ZBUFFER_FORMAT_32BIT ] = GD3D11.Device.Caps.DepthBufferDepthStencilFormat[ ZBUFFER_FORMAT_24BIT ] ;
		}
		DXST_LOGFILEFMT_ADDA(( "32bit Z\x83\x6f\x83\x62\x83\x74\x83\x40\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "32bit Zバッファフォーマットは %s です" @*/, String )) ;


		// カラーフォーマットのチェック
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R8G8B8A8_UNORM, &FormatSupport ) != S_OK ||
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D ) == 0 )
		{
			DXST_LOGFILE_ADDA( "\x8e\x67\x97\x70\x82\xc5\x82\xab\x82\xe9\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xaa\x8c\xa9\x82\xc2\x82\xa9\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x82\xc5\x82\xb5\x82\xbd\n"/*@ "使用できるカラーフォーマットが見つかりませんでした\n" @*/ ) ;
			return -1 ;
		}
		GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_RGB16           ] = D_DXGI_FORMAT_R8G8B8A8_UNORM ;
		GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_RGB32           ] = D_DXGI_FORMAT_R8G8B8A8_UNORM ;
		GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHA_RGB16     ] = D_DXGI_FORMAT_R8G8B8A8_UNORM ;
		GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHA_RGB32     ] = D_DXGI_FORMAT_R8G8B8A8_UNORM ;
		GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHATEST_RGB16 ] = D_DXGI_FORMAT_R8G8B8A8_UNORM ;
		GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHATEST_RGB32 ] = D_DXGI_FORMAT_R8G8B8A8_UNORM ;
		String = "DXGI_FORMAT_R8G8B8A8_UNORM" ;
		DXST_LOGFILEFMT_ADDA(( "\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "カラーフォーマットは %s です" @*/, String )) ;

		// パレットカラーフォーマットのセット
		GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_PAL4           ] = D_DXGI_FORMAT_R8G8B8A8_UNORM ;
		GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_PAL8           ] = D_DXGI_FORMAT_R8G8B8A8_UNORM ;
		GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHA_PAL4     ] = D_DXGI_FORMAT_R8G8B8A8_UNORM ;
		GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHA_PAL8     ] = D_DXGI_FORMAT_R8G8B8A8_UNORM ;
		GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHATEST_PAL4 ] = D_DXGI_FORMAT_R8G8B8A8_UNORM ;
		GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHATEST_PAL8 ] = D_DXGI_FORMAT_R8G8B8A8_UNORM ;

		// DXT1フォーマットのチェック
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_BC1_UNORM, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DXT1 ] = D_DXGI_FORMAT_BC1_UNORM ;
			String = " DXGI_FORMAT_BC1_UNORM \x82\xc5\x82\xb7"/*@ " DXGI_FORMAT_BC1_UNORM です" @*/ ;
		}
		else
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DXT1 ] = GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHATEST_RGB16 ] ;
			String = "\x8e\x67\x82\xa6\x82\xdc\x82\xb9\x82\xf1"/*@ "使えません" @*/ ;
		}
		DXST_LOGFILEFMT_ADDA(( "\x82\x63\x82\x77\x82\x73\x82\x50\x83\x65\x83\x4e\x83\x58\x83\x60\x83\x83\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd%s"/*@ "ＤＸＴ１テクスチャフォーマットは%s" @*/, String )) ;

		// DXT2フォーマットは使用できない
		GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DXT2 ] = GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHA_RGB32 ] ;
		DXST_LOGFILEFMT_ADDA(( "\x82\x63\x82\x77\x82\x73\x82\x51\x83\x65\x83\x4e\x83\x58\x83\x60\x83\x83\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd\x8e\x67\x82\xa6\x82\xdc\x82\xb9\x82\xf1"/*@ "ＤＸＴ２テクスチャフォーマットは使えません" @*/ )) ;

		// DXT3フォーマットのチェック
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_BC2_UNORM, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DXT3 ] = D_DXGI_FORMAT_BC2_UNORM ;
			String = " DXGI_FORMAT_BC2_UNORM \x82\xc5\x82\xb7"/*@ " DXGI_FORMAT_BC2_UNORM です" @*/ ;
		}
		else
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DXT3 ] = GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHA_RGB32 ] ;
			String = "\x8e\x67\x82\xa6\x82\xdc\x82\xb9\x82\xf1"/*@ "使えません" @*/ ;
		}
		DXST_LOGFILEFMT_ADDA(( "\x82\x63\x82\x77\x82\x73\x82\x52\x83\x65\x83\x4e\x83\x58\x83\x60\x83\x83\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd%s"/*@ "ＤＸＴ３テクスチャフォーマットは%s" @*/, String )) ;

		// DXT4フォーマットは使用できない
		GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DXT4 ] = GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHA_RGB32 ] ;
		DXST_LOGFILEFMT_ADDA(( "\x82\x63\x82\x77\x82\x73\x82\x53\x83\x65\x83\x4e\x83\x58\x83\x60\x83\x83\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd\x8e\x67\x82\xa6\x82\xdc\x82\xb9\x82\xf1"/*@ "ＤＸＴ４テクスチャフォーマットは使えません" @*/ )) ;

		// DXT5フォーマットのチェック
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_BC3_UNORM, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DXT5 ] = D_DXGI_FORMAT_BC3_UNORM ;
			String = " DXGI_FORMAT_BC3_UNORM \x82\xc5\x82\xb7"/*@ " DXGI_FORMAT_BC3_UNORM です" @*/ ;
		}
		else
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DXT5 ] = GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ALPHA_RGB32 ] ;
			String = "\x8e\x67\x82\xa6\x82\xdc\x82\xb9\x82\xf1"/*@ "使えません" @*/ ;
		}
		DXST_LOGFILEFMT_ADDA(( "\x82\x63\x82\x77\x82\x73\x82\x54\x83\x65\x83\x4e\x83\x58\x83\x60\x83\x83\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd%s"/*@ "ＤＸＴ５テクスチャフォーマットは%s" @*/, String )) ;





		// ARGB整数16ビット型カラーフォーマットの選定
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R16G16B16A16_UNORM, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ABGR_I16 ] = D_DXGI_FORMAT_R16G16B16A16_UNORM ;
			String = "DXGI_FORMAT_R16G16B16A16_UNORM" ;
		}
		else
		{
			DXST_LOGFILE_ADDA( "\x8e\x67\x97\x70\x82\xc5\x82\xab\x82\xe9\x20\x41\x42\x47\x52\x20\x90\xae\x90\x94\x20\x31\x36\x20\x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xaa\x82\xa0\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x82\xc5\x82\xb5\x82\xbd\x5c\x6e"/*@ "使用できる ABGR 整数 16 ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ABGR_I16 ] = D_DXGI_FORMAT_UNKNOWN ;
		}

		if( GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ABGR_I16 ] != D_DXGI_FORMAT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x41\x42\x47\x52\x20\x90\xae\x90\x94\x20\x31\x36\x20\x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd\x20\x25\x73\x20\x82\xc5\x82\xb7"/*@ "ABGR 整数 16 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// ARGB浮動小数点16ビット型カラーフォーマットの選定
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R16G16B16A16_FLOAT, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ABGR_F16 ] = D_DXGI_FORMAT_R16G16B16A16_FLOAT ;
			String = "DXGI_FORMAT_R16G16B16A16_FLOAT" ;
		}
		else
		{
			DXST_LOGFILE_ADDA( "\x8e\x67\x97\x70\x82\xc5\x82\xab\x82\xe9\x41\x42\x47\x52\x20\x95\x82\x93\xae\x8f\xac\x90\x94\x93\x5f\x20\x31\x36\x20\x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xaa\x82\xa0\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x82\xc5\x82\xb5\x82\xbd\x5c\x6e"/*@ "使用できるABGR 浮動小数点 16 ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ABGR_F16 ] = D_DXGI_FORMAT_UNKNOWN ;
		}

		if( GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ABGR_F16 ] != D_DXGI_FORMAT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x41\x42\x47\x52\x20\x95\x82\x93\xae\x8f\xac\x90\x94\x93\x5f\x20\x31\x36\x20\x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd\x20\x25\x73\x20\x82\xc5\x82\xb7"/*@ "ABGR 浮動小数点 16 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// ARGB浮動小数点32ビット型カラーフォーマットの選定
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R32G32B32A32_FLOAT, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ABGR_F32 ] = D_DXGI_FORMAT_R32G32B32A32_FLOAT ;
			String = "DXGI_FORMAT_R32G32B32A32_FLOAT" ;
		}
		else
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R16G16B16A16_FLOAT, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ABGR_F32 ] = D_DXGI_FORMAT_R16G16B16A16_FLOAT ;
			String = "DXGI_FORMAT_R16G16B16A16_FLOAT" ;
		}
		else
		{
			DXST_LOGFILE_ADDA( "\x8e\x67\x97\x70\x82\xc5\x82\xab\x82\xe9\x41\x42\x47\x52\x20\x95\x82\x93\xae\x8f\xac\x90\x94\x93\x5f\x20\x33\x32\x20\x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xaa\x82\xa0\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x82\xc5\x82\xb5\x82\xbd\x5c\x6e"/*@ "使用できるABGR 浮動小数点 32 ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ABGR_F32 ] = D_DXGI_FORMAT_UNKNOWN ;
		}

		if( GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ABGR_F32 ] != D_DXGI_FORMAT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x41\x42\x47\x52\x20\x95\x82\x93\xae\x8f\xac\x90\x94\x93\x5f\x20\x33\x32\x20\x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd\x20\x25\x73\x20\x82\xc5\x82\xb7"/*@ "ABGR 浮動小数点 32 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// １チャンネル整数8ビット型カラーフォーマットの選定
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R8_UNORM, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_I8 ] = D_DXGI_FORMAT_R8_UNORM ;
			String = "DXGI_FORMAT_R8_UNORM" ;
		}
		else
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R8G8_UNORM, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_I8 ] = D_DXGI_FORMAT_R8G8_UNORM ;
			String = "DXGI_FORMAT_R8G8_UNORM" ;
		}
		else
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R8G8B8A8_UNORM, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_I8 ] = D_DXGI_FORMAT_R8G8B8A8_UNORM ;
			String = "DXGI_FORMAT_R8G8B8A8_UNORM" ;
		}
		else
		{
			DXST_LOGFILE_ADDA( "\x8e\x67\x97\x70\x82\xc5\x82\xab\x82\xe9\x82\x50\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x90\xae\x90\x94\x20\x38\x20\x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xaa\x82\xa0\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x82\xc5\x82\xb5\x82\xbd\x5c\x6e"/*@ "使用できる１チャンネル整数 8 ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_I8 ] = D_DXGI_FORMAT_UNKNOWN ;
		}

		if( GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_I8 ] != D_DXGI_FORMAT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x82\x50\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x90\xae\x90\x94\x20\x38\x20\x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd\x20\x25\x73\x20\x82\xc5\x82\xb7"/*@ "１チャンネル整数 8 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// １チャンネル整数16ビット型カラーフォーマットの選定
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R16_UNORM, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_I16 ] = D_DXGI_FORMAT_R16_UNORM ;
			String = "DXGI_FORMAT_R16_UNORM" ;
		}
		else
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R16G16_UNORM, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_I16 ] = D_DXGI_FORMAT_R16G16_UNORM ;
			String = "DXGI_FORMAT_R16G16_UNORM" ;
		}
		else
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R16G16B16A16_UNORM, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_I16 ] = D_DXGI_FORMAT_R16G16B16A16_UNORM ;
			String = "DXGI_FORMAT_R16G16B16A16_UNORM" ;
		}
		else
		{
			DXST_LOGFILE_ADDA( "\x8e\x67\x97\x70\x82\xc5\x82\xab\x82\xe9\x82\x50\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x90\xae\x90\x94\x20\x31\x36\x20\x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xaa\x82\xa0\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x82\xc5\x82\xb5\x82\xbd\x5c\x6e"/*@ "使用できる１チャンネル整数 16 ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_I16 ] = D_DXGI_FORMAT_UNKNOWN ;
		}

		if( GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_I16 ] != D_DXGI_FORMAT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x82\x50\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x90\xae\x90\x94\x20\x31\x36\x20\x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd\x20\x25\x73\x20\x82\xc5\x82\xb7"/*@ "１チャンネル整数 16 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// １チャンネル浮動小数点16ビット型カラーフォーマットの選定
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R16_FLOAT, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_F16 ] = D_DXGI_FORMAT_R16_FLOAT ;
			String = "DXGI_FORMAT_R16_FLOAT" ;
		}
		else
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R16G16_FLOAT, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_F16 ] = D_DXGI_FORMAT_R16G16_FLOAT ;
			String = "DXGI_FORMAT_R16G16_FLOAT" ;
		}
		else
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R16G16B16A16_FLOAT, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_F16 ] = D_DXGI_FORMAT_R16G16B16A16_FLOAT ;
			String = "DXGI_FORMAT_R16G16B16A16_FLOAT" ;
		}
		else
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R32G32B32A32_FLOAT, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_F16 ] = D_DXGI_FORMAT_R32G32B32A32_FLOAT ;
			String = "DXGI_FORMAT_R32G32B32A32_FLOAT" ;
		}
		else
		{
			DXST_LOGFILE_ADDA( "\x8e\x67\x97\x70\x82\xc5\x82\xab\x82\xe9\x82\x50\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x95\x82\x93\xae\x8f\xac\x90\x94\x93\x5f\x20\x31\x36\x20\x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xaa\x82\xa0\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x82\xc5\x82\xb5\x82\xbd\x5c\x6e"/*@ "使用できる１チャンネル浮動小数点 16 ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_F16 ] = D_DXGI_FORMAT_UNKNOWN ;
		}

		if( GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_F16 ] != D_DXGI_FORMAT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x82\x50\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x95\x82\x93\xae\x8f\xac\x90\x94\x93\x5f\x20\x31\x36\x20\x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd\x20\x25\x73\x20\x82\xc5\x82\xb7"/*@ "１チャンネル浮動小数点 16 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// １チャンネル浮動小数点32ビット型カラーフォーマットの選定
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R32_FLOAT, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_F32 ] = D_DXGI_FORMAT_R32_FLOAT ;
			String = "DXGI_FORMAT_R32_FLOAT" ;
		}
		else
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R32G32_FLOAT, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_F32 ] = D_DXGI_FORMAT_R32G32_FLOAT ;
			String = "DXGI_FORMAT_R32G32_FLOAT" ;
		}
		else
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R32G32B32A32_FLOAT, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_F32 ] = D_DXGI_FORMAT_R32G32B32A32_FLOAT ;
			String = "DXGI_FORMAT_R32G32B32A32_FLOAT" ;
		}
		else
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R16G16B16A16_FLOAT, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_F32 ] = D_DXGI_FORMAT_R16G16B16A16_FLOAT ;
			String = "DXGI_FORMAT_R16G16B16A16_FLOAT" ;
		}
		else
		{
			DXST_LOGFILE_ADDA( "\x8e\x67\x97\x70\x82\xc5\x82\xab\x82\xe9\x82\x50\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x95\x82\x93\xae\x8f\xac\x90\x94\x93\x5f\x20\x33\x32\x20\x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xaa\x82\xa0\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x82\xc5\x82\xb5\x82\xbd\x5c\x6e"/*@ "使用できる１チャンネル浮動小数点 32 ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_F32 ] = D_DXGI_FORMAT_UNKNOWN ;
		}

		if( GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_ONE_F32 ] != D_DXGI_FORMAT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x82\x50\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x95\x82\x93\xae\x8f\xac\x90\x94\x93\x5f\x20\x33\x32\x20\x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd\x20\x25\x73\x20\x82\xc5\x82\xb7"/*@ "１チャンネル浮動小数点 32 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// ２チャンネル整数8ビット型カラーフォーマットの選定
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R8G8_UNORM, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_TWO_I8 ] = D_DXGI_FORMAT_R8G8_UNORM ;
			String = "DXGI_FORMAT_R8G8_UNORM" ;
		}
		else
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R8G8B8A8_UNORM, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_TWO_I8 ] = D_DXGI_FORMAT_R8G8B8A8_UNORM ;
			String = "DXGI_FORMAT_R8G8B8A8_UNORM" ;
		}
		else
		{
			DXST_LOGFILE_ADDA( "\x8e\x67\x97\x70\x82\xc5\x82\xab\x82\xe9\x82\x51\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x90\xae\x90\x94\x20\x38\x20\x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xaa\x82\xa0\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x82\xc5\x82\xb5\x82\xbd\x5c\x6e"/*@ "使用できる２チャンネル整数 8 ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_TWO_I8 ] = D_DXGI_FORMAT_UNKNOWN ;
		}

		if( GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_TWO_I8 ] != D_DXGI_FORMAT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x82\x51\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x90\xae\x90\x94\x20\x38\x20\x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd\x20\x25\x73\x20\x82\xc5\x82\xb7"/*@ "２チャンネル整数 8 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// ２チャンネル整数16ビット型カラーフォーマットの選定
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R16G16_UNORM, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_TWO_I16 ] = D_DXGI_FORMAT_R16G16_UNORM ;
			String = "DXGI_FORMAT_R16G16_UNORM" ;
		}
		else
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R16G16B16A16_UNORM, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_TWO_I16 ] = D_DXGI_FORMAT_R16G16B16A16_UNORM ;
			String = "DXGI_FORMAT_R16G16B16A16_UNORM" ;
		}
		else
		{
			DXST_LOGFILE_ADDA( "\x8e\x67\x97\x70\x82\xc5\x82\xab\x82\xe9\x82\x51\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x90\xae\x90\x94\x20\x31\x36\x20\x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xaa\x82\xa0\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x82\xc5\x82\xb5\x82\xbd\x5c\x6e"/*@ "使用できる２チャンネル整数 16 ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_TWO_I16 ] = D_DXGI_FORMAT_UNKNOWN ;
		}

		if( GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_TWO_I16 ] != D_DXGI_FORMAT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x82\x51\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x90\xae\x90\x94\x20\x31\x36\x20\x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd\x20\x25\x73\x20\x82\xc5\x82\xb7"/*@ "２チャンネル整数 16 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// ２チャンネル浮動小数点16ビット型カラーフォーマットの選定
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R16G16_FLOAT, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_TWO_F16 ] = D_DXGI_FORMAT_R16G16_FLOAT ;
			String = "DXGI_FORMAT_R16G16_FLOAT" ;
		}
		else
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R16G16B16A16_FLOAT, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_TWO_F16 ] = D_DXGI_FORMAT_R16G16B16A16_FLOAT ;
			String = "DXGI_FORMAT_R16G16B16A16_FLOAT" ;
		}
		else
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R32G32B32A32_FLOAT, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_TWO_F16 ] = D_DXGI_FORMAT_R32G32B32A32_FLOAT ;
			String = "DXGI_FORMAT_R32G32B32A32_FLOAT" ;
		}
		else
		{
			DXST_LOGFILE_ADDA( "\x8e\x67\x97\x70\x82\xc5\x82\xab\x82\xe9\x82\x51\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x95\x82\x93\xae\x8f\xac\x90\x94\x93\x5f\x20\x31\x36\x20\x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xaa\x82\xa0\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x82\xc5\x82\xb5\x82\xbd\x5c\x6e"/*@ "使用できる２チャンネル浮動小数点 16 ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_TWO_F16 ] = D_DXGI_FORMAT_UNKNOWN ;
		}

		if( GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_TWO_F16 ] != D_DXGI_FORMAT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x82\x51\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x95\x82\x93\xae\x8f\xac\x90\x94\x93\x5f\x20\x31\x36\x20\x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd\x20\x25\x73\x20\x82\xc5\x82\xb7"/*@ "２チャンネル浮動小数点 16 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// ２チャンネル浮動小数点32ビット型カラーフォーマットの選定
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R32G32_FLOAT, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_TWO_F32 ] = D_DXGI_FORMAT_R32G32_FLOAT ;
			String = "DXGI_FORMAT_R32G32_FLOAT" ;
		}
		else
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R32G32B32A32_FLOAT, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_TWO_F32 ] = D_DXGI_FORMAT_R32G32B32A32_FLOAT ;
			String = "DXGI_FORMAT_R32G32B32A32_FLOAT" ;
		}
		else
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R16G16B16A16_FLOAT, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_TWO_F32 ] = D_DXGI_FORMAT_R16G16B16A16_FLOAT ;
			String = "DXGI_FORMAT_R16G16B16A16_FLOAT" ;
		}
		else
		{
			DXST_LOGFILE_ADDA( "\x8e\x67\x97\x70\x82\xc5\x82\xab\x82\xe9\x82\x51\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x95\x82\x93\xae\x8f\xac\x90\x94\x93\x5f\x20\x33\x32\x20\x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xaa\x82\xa0\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x82\xc5\x82\xb5\x82\xbd\x5c\x6e"/*@ "使用できる２チャンネル浮動小数点 32 ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_TWO_F32 ] = D_DXGI_FORMAT_UNKNOWN ;
		}

		if( GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_TWO_F32 ] != D_DXGI_FORMAT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x82\x51\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x95\x82\x93\xae\x8f\xac\x90\x94\x93\x5f\x20\x33\x32\x20\x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd\x20\x25\x73\x20\x82\xc5\x82\xb7"/*@ "２チャンネル浮動小数点 32 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}





		// 16bit描画可能カラーフォーマットの選定
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_B5G6R5_UNORM, &FormatSupport ) == S_OK ||
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 ||
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_RENDER_TARGET ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_RGB16 ] = D_DXGI_FORMAT_B5G6R5_UNORM ;
			String = "DXGI_FORMAT_B5G6R5_UNORM" ;
		}
		else
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_B8G8R8X8_UNORM, &FormatSupport ) == S_OK ||
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 ||
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_RENDER_TARGET ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_RGB16 ] = D_DXGI_FORMAT_B8G8R8X8_UNORM ;
			String = "DXGI_FORMAT_B8G8R8X8_UNORM" ;
		}
		else
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R8G8B8A8_UNORM, &FormatSupport ) == S_OK ||
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 ||
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_RENDER_TARGET ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_RGB16 ] = D_DXGI_FORMAT_R8G8B8A8_UNORM ;
			String = "DXGI_FORMAT_R8G8B8A8_UNORM" ;
		}
		else
		{
			DXST_LOGFILE_ADDA( "\x8e\x67\x97\x70\x82\xc5\x82\xab\x82\xe9 16bit \x95\x60\x89\xe6\x89\xc2\x94\x5c\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xaa\x8c\xa9\x82\xc2\x82\xa9\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x82\xc5\x82\xb5\x82\xbd\n"/*@ "使用できる 16bit 描画可能カラーフォーマットが見つかりませんでした\n" @*/ ) ;
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_RGB16 ] = D_DXGI_FORMAT_UNKNOWN ;
		}

		if( GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_RGB16 ] != D_DXGI_FORMAT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x95\x60\x89\xe6\x97\x70 16bit \x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "描画用 16bit カラーフォーマットは %s です" @*/, String )) ;
		}

		// 32bit描画可能カラーフォーマットの選定
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_B8G8R8X8_UNORM, &FormatSupport ) == S_OK ||
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 ||
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_RENDER_TARGET ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_RGB32 ] = D_DXGI_FORMAT_B8G8R8X8_UNORM ;
			String = "DXGI_FORMAT_B8G8R8X8_UNORM" ;
		}
		else
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R8G8B8A8_UNORM, &FormatSupport ) == S_OK ||
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 ||
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_RENDER_TARGET ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_RGB32 ] = D_DXGI_FORMAT_R8G8B8A8_UNORM ;
			String = "DXGI_FORMAT_R8G8B8A8_UNORM" ;
		}
		else
		{
			DXST_LOGFILE_ADDA( "\x8e\x67\x97\x70\x82\xc5\x82\xab\x82\xe9 32bit \x95\x60\x89\xe6\x89\xc2\x94\x5c\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xaa\x8c\xa9\x82\xc2\x82\xa9\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x82\xc5\x82\xb5\x82\xbd\n"/*@ "使用できる 32bit 描画可能カラーフォーマットが見つかりませんでした\n" @*/ ) ;
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_RGB32 ] = D_DXGI_FORMAT_UNKNOWN ;
		}

		if( GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_RGB32 ] != D_DXGI_FORMAT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x95\x60\x89\xe6\x97\x70 32bit \x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "描画用 32bit カラーフォーマットは %s です" @*/, String )) ;
		}

		// アルファチャンネル付き 32bit描画可能カラーフォーマットの選定
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R8G8B8A8_UNORM, &FormatSupport ) == S_OK ||
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 ||
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_RENDER_TARGET ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ALPHA_RGB32 ] = D_DXGI_FORMAT_R8G8B8A8_UNORM ;
			String = "DXGI_FORMAT_R8G8B8A8_UNORM" ;
		}
		else
		{
			DXST_LOGFILE_ADDA( "\x8e\x67\x97\x70\x82\xc5\x82\xab\x82\xe9\x83\x41\x83\x8b\x83\x74\x83\x40\x95\x74\x82\xab 32bit \x95\x60\x89\xe6\x89\xc2\x94\x5c\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xaa\x8c\xa9\x82\xc2\x82\xa9\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x82\xc5\x82\xb5\x82\xbd\n"/*@ "使用できるアルファ付き 32bit 描画可能カラーフォーマットが見つかりませんでした\n" @*/ ) ;
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ALPHA_RGB32 ] = D_DXGI_FORMAT_UNKNOWN ;
		}

		if( GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ALPHA_RGB32 ] != D_DXGI_FORMAT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x95\x60\x89\xe6\x97\x70\x83\x41\x83\x8b\x83\x74\x83\x40\x95\x74\x82\xab 32bit \x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "描画用アルファ付き 32bit カラーフォーマットは %s です" @*/, String )) ;
		}

		// 描画可能 ARGB整数16ビット型カラーフォーマットの選定
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R16G16B16A16_UNORM, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_RENDER_TARGET ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ABGR_I16 ] = D_DXGI_FORMAT_R16G16B16A16_UNORM ;
			String = "DXGI_FORMAT_R16G16B16A16_UNORM" ;
		}
		else
		{
			DXST_LOGFILE_ADDA( "\x8e\x67\x97\x70\x82\xc5\x82\xab\x82\xe9\x95\x60\x89\xe6\x97\x70 ABGR \x90\xae\x90\x94 16 \x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xaa\x82\xa0\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x82\xc5\x82\xb5\x82\xbd\n"/*@ "使用できる描画用 ABGR 整数 16 ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ABGR_I16 ] = D_DXGI_FORMAT_UNKNOWN ;
		}

		if( GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ABGR_I16 ] != D_DXGI_FORMAT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x95\x60\x89\xe6\x97\x70 ABGR \x90\xae\x90\x94 16 \x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "描画用 ABGR 整数 16 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// 描画可能 ARGB浮動小数点16ビット型カラーフォーマットの選定
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R16G16B16A16_FLOAT, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_RENDER_TARGET ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ABGR_F16 ] = D_DXGI_FORMAT_R16G16B16A16_FLOAT ;
			String = "DXGI_FORMAT_R16G16B16A16_FLOAT" ;
		}
		else
		{
			DXST_LOGFILE_ADDA( "\x8e\x67\x97\x70\x82\xc5\x82\xab\x82\xe9\x95\x60\x89\xe6\x97\x70 ABGR \x95\x82\x93\xae\x8f\xac\x90\x94\x93\x5f 16 \x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xaa\x82\xa0\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x82\xc5\x82\xb5\x82\xbd\n"/*@ "使用できる描画用 ABGR 浮動小数点 16 ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ABGR_F16 ] = D_DXGI_FORMAT_UNKNOWN ;
		}

		if( GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ABGR_F16 ] != D_DXGI_FORMAT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x95\x60\x89\xe6\x97\x70 ABGR \x95\x82\x93\xae\x8f\xac\x90\x94\x93\x5f 16 \x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "描画用 ABGR 浮動小数点 16 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// 描画可能 ARGB浮動小数点32ビット型カラーフォーマットの選定
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R32G32B32A32_FLOAT, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_RENDER_TARGET ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ABGR_F32 ] = D_DXGI_FORMAT_R32G32B32A32_FLOAT ;
			String = "DXGI_FORMAT_R32G32B32A32_FLOAT" ;
		}
		else
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R16G16B16A16_FLOAT, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_RENDER_TARGET ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ABGR_F32 ] = D_DXGI_FORMAT_R16G16B16A16_FLOAT ;
			String = "DXGI_FORMAT_R16G16B16A16_FLOAT" ;
		}
		else
		{
			DXST_LOGFILE_ADDA( "\x8e\x67\x97\x70\x82\xc5\x82\xab\x82\xe9\x95\x60\x89\xe6\x97\x70 ABGR \x95\x82\x93\xae\x8f\xac\x90\x94\x93\x5f 32 \x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xaa\x82\xa0\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x82\xc5\x82\xb5\x82\xbd\n"/*@ "使用できる描画用 ABGR 浮動小数点 32 ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ABGR_F32 ] = D_DXGI_FORMAT_UNKNOWN ;
		}

		if( GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ABGR_F32 ] != D_DXGI_FORMAT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x95\x60\x89\xe6\x97\x70 ABGR \x95\x82\x93\xae\x8f\xac\x90\x94\x93\x5f 32 \x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "描画用 ABGR 浮動小数点 32 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// 描画可能 １チャンネル整数8ビット型カラーフォーマットの選定
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R8_UNORM, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_RENDER_TARGET ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_I8 ] = D_DXGI_FORMAT_R8_UNORM ;
			String = "DXGI_FORMAT_R8_UNORM" ;
		}
		else
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R8G8_UNORM, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_RENDER_TARGET ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_I8 ] = D_DXGI_FORMAT_R8G8_UNORM ;
			String = "DXGI_FORMAT_R8G8_UNORM" ;
		}
		else
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R8G8B8A8_UNORM, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_RENDER_TARGET ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_I8 ] = D_DXGI_FORMAT_R8G8B8A8_UNORM ;
			String = "DXGI_FORMAT_R8G8B8A8_UNORM" ;
		}
		else
		{
			DXST_LOGFILE_ADDA( "\x8e\x67\x97\x70\x82\xc5\x82\xab\x82\xe9\x95\x60\x89\xe6\x97\x70\x82\x50\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x90\xae\x90\x94 8 \x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xaa\x82\xa0\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x82\xc5\x82\xb5\x82\xbd\n"/*@ "使用できる描画用１チャンネル整数 8 ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_I8 ] = D_DXGI_FORMAT_UNKNOWN ;
		}

		if( GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_I8 ] != D_DXGI_FORMAT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x95\x60\x89\xe6\x97\x70\x82\x50\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x90\xae\x90\x94 8 \x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "描画用１チャンネル整数 8 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// 描画可能 １チャンネル整数16ビット型カラーフォーマットの選定
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R16_UNORM, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_RENDER_TARGET ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_I16 ] = D_DXGI_FORMAT_R16_UNORM ;
			String = "DXGI_FORMAT_R16_UNORM" ;
		}
		else
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R16G16_UNORM, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_RENDER_TARGET ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_I16 ] = D_DXGI_FORMAT_R16G16_UNORM ;
			String = "DXGI_FORMAT_R16G16_UNORM" ;
		}
		else
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R16G16B16A16_UNORM, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_RENDER_TARGET ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_I16 ] = D_DXGI_FORMAT_R16G16B16A16_UNORM ;
			String = "DXGI_FORMAT_R16G16B16A16_UNORM" ;
		}
		else
		{
			DXST_LOGFILE_ADDA( "\x8e\x67\x97\x70\x82\xc5\x82\xab\x82\xe9\x95\x60\x89\xe6\x97\x70\x82\x50\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x90\xae\x90\x94 16 \x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xaa\x82\xa0\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x82\xc5\x82\xb5\x82\xbd\n"/*@ "使用できる描画用１チャンネル整数 16 ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_I16 ] = D_DXGI_FORMAT_UNKNOWN ;
		}

		if( GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_I16 ] != D_DXGI_FORMAT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x95\x60\x89\xe6\x97\x70\x82\x50\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x90\xae\x90\x94 16 \x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "描画用１チャンネル整数 16 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// 描画可能 １チャンネル浮動小数点16ビット型カラーフォーマットの選定
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R16_FLOAT, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_RENDER_TARGET ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_F16 ] = D_DXGI_FORMAT_R16_FLOAT ;
			String = "DXGI_FORMAT_R16_FLOAT" ;
		}
		else
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R16G16_FLOAT, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_RENDER_TARGET ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_F16 ] = D_DXGI_FORMAT_R16G16_FLOAT ;
			String = "DXGI_FORMAT_R16G16_FLOAT" ;
		}
		else
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R16G16B16A16_FLOAT, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_RENDER_TARGET ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_F16 ] = D_DXGI_FORMAT_R16G16B16A16_FLOAT ;
			String = "DXGI_FORMAT_R16G16B16A16_FLOAT" ;
		}
		else
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R32G32B32A32_FLOAT, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_RENDER_TARGET ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_F16 ] = D_DXGI_FORMAT_R32G32B32A32_FLOAT ;
			String = "DXGI_FORMAT_R32G32B32A32_FLOAT" ;
		}
		else
		{
			DXST_LOGFILE_ADDA( "\x8e\x67\x97\x70\x82\xc5\x82\xab\x82\xe9\x95\x60\x89\xe6\x97\x70\x82\x50\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x95\x82\x93\xae\x8f\xac\x90\x94\x93\x5f 16 \x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xaa\x82\xa0\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x82\xc5\x82\xb5\x82\xbd\n"/*@ "使用できる描画用１チャンネル浮動小数点 16 ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_F16 ] = D_DXGI_FORMAT_UNKNOWN ;
		}

		if( GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_F16 ] != D_DXGI_FORMAT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x95\x60\x89\xe6\x97\x70\x82\x50\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x95\x82\x93\xae\x8f\xac\x90\x94\x93\x5f 16 \x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "描画用１チャンネル浮動小数点 16 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// 描画可能 １チャンネル浮動小数点32ビット型カラーフォーマットの選定
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R32_FLOAT, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_RENDER_TARGET ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_F32 ] = D_DXGI_FORMAT_R32_FLOAT ;
			String = "DXGI_FORMAT_R32_FLOAT" ;
		}
		else
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R32G32_FLOAT, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_RENDER_TARGET ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_F32 ] = D_DXGI_FORMAT_R32G32_FLOAT ;
			String = "DXGI_FORMAT_R32G32_FLOAT" ;
		}
		else
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R32G32B32A32_FLOAT, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_RENDER_TARGET ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_F32 ] = D_DXGI_FORMAT_R32G32B32A32_FLOAT ;
			String = "DXGI_FORMAT_R32G32B32A32_FLOAT" ;
		}
		else
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R16G16B16A16_FLOAT, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_RENDER_TARGET ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_F32 ] = D_DXGI_FORMAT_R16G16B16A16_FLOAT ;
			String = "DXGI_FORMAT_R16G16B16A16_FLOAT" ;
		}
		else
		{
			DXST_LOGFILE_ADDA( "\x8e\x67\x97\x70\x82\xc5\x82\xab\x82\xe9\x95\x60\x89\xe6\x97\x70\x82\x50\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x95\x82\x93\xae\x8f\xac\x90\x94\x93\x5f 32 \x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xaa\x82\xa0\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x82\xc5\x82\xb5\x82\xbd\n"/*@ "使用できる描画用１チャンネル浮動小数点 32 ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_F32 ] = D_DXGI_FORMAT_UNKNOWN ;
		}

		if( GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_F32 ] != D_DXGI_FORMAT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x95\x60\x89\xe6\x97\x70\x82\x50\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x95\x82\x93\xae\x8f\xac\x90\x94\x93\x5f 32 \x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "描画用１チャンネル浮動小数点 32 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// 描画可能 ２チャンネル整数8ビット型カラーフォーマットの選定
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R8G8_UNORM, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_RENDER_TARGET ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_I8 ] = D_DXGI_FORMAT_R8G8_UNORM ;
			String = "DXGI_FORMAT_R8G8_UNORM" ;
		}
		else
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R8G8B8A8_UNORM, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_RENDER_TARGET ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_I8 ] = D_DXGI_FORMAT_R8G8B8A8_UNORM ;
			String = "DXGI_FORMAT_R8G8B8A8_UNORM" ;
		}
		else
		{
			DXST_LOGFILE_ADDA( "\x8e\x67\x97\x70\x82\xc5\x82\xab\x82\xe9\x95\x60\x89\xe6\x97\x70\x82\x51\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x90\xae\x90\x94 8 \x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xaa\x82\xa0\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x82\xc5\x82\xb5\x82\xbd\n"/*@ "使用できる描画用２チャンネル整数 8 ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_I8 ] = D_DXGI_FORMAT_UNKNOWN ;
		}

		if( GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_I8 ] != D_DXGI_FORMAT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x95\x60\x89\xe6\x97\x70\x82\x51\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x90\xae\x90\x94 8 \x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "描画用２チャンネル整数 8 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// 描画可能 ２チャンネル整数16ビット型カラーフォーマットの選定
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R16G16_UNORM, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_RENDER_TARGET ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_I16 ] = D_DXGI_FORMAT_R16G16_UNORM ;
			String = "DXGI_FORMAT_R16G16_UNORM" ;
		}
		else
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R16G16B16A16_UNORM, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_RENDER_TARGET ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_I16 ] = D_DXGI_FORMAT_R16G16B16A16_UNORM ;
			String = "DXGI_FORMAT_R16G16B16A16_UNORM" ;
		}
		else
		{
			DXST_LOGFILE_ADDA( "\x8e\x67\x97\x70\x82\xc5\x82\xab\x82\xe9\x95\x60\x89\xe6\x97\x70\x82\x51\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x90\xae\x90\x94 16 \x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xaa\x82\xa0\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x82\xc5\x82\xb5\x82\xbd\n"/*@ "使用できる描画用２チャンネル整数 16 ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_I16 ] = D_DXGI_FORMAT_UNKNOWN ;
		}

		if( GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_I16 ] != D_DXGI_FORMAT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x95\x60\x89\xe6\x97\x70\x82\x51\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x90\xae\x90\x94 16 \x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "描画用２チャンネル整数 16 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// 描画可能 ２チャンネル浮動小数点16ビット型カラーフォーマットの選定
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R16G16_FLOAT, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_RENDER_TARGET ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_F16 ] = D_DXGI_FORMAT_R16G16_FLOAT ;
			String = "DXGI_FORMAT_R16G16_FLOAT" ;
		}
		else
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R16G16B16A16_FLOAT, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_RENDER_TARGET ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_F16 ] = D_DXGI_FORMAT_R16G16B16A16_FLOAT ;
			String = "DXGI_FORMAT_R16G16B16A16_FLOAT" ;
		}
		else
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R32G32B32A32_FLOAT, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_RENDER_TARGET ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_F16 ] = D_DXGI_FORMAT_R32G32B32A32_FLOAT ;
			String = "DXGI_FORMAT_R32G32B32A32_FLOAT" ;
		}
		else
		{
			DXST_LOGFILE_ADDA( "\x8e\x67\x97\x70\x82\xc5\x82\xab\x82\xe9\x95\x60\x89\xe6\x97\x70\x82\x51\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x95\x82\x93\xae\x8f\xac\x90\x94\x93\x5f 16 \x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xaa\x82\xa0\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x82\xc5\x82\xb5\x82\xbd\n"/*@ "使用できる描画用２チャンネル浮動小数点 16 ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_F16 ] = D_DXGI_FORMAT_UNKNOWN ;
		}

		if( GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_F16 ] != D_DXGI_FORMAT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x95\x60\x89\xe6\x97\x70\x82\x51\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x95\x82\x93\xae\x8f\xac\x90\x94\x93\x5f 16 \x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "描画用２チャンネル浮動小数点 16 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// 描画可能 ２チャンネル浮動小数点32ビット型カラーフォーマットの選定
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R32G32_FLOAT, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_RENDER_TARGET ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_F32 ] = D_DXGI_FORMAT_R32G32_FLOAT ;
			String = "DXGI_FORMAT_R32G32_FLOAT" ;
		}
		else
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R32G32B32A32_FLOAT, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_RENDER_TARGET ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_F32 ] = D_DXGI_FORMAT_R32G32B32A32_FLOAT ;
			String = "DXGI_FORMAT_R32G32B32A32_FLOAT" ;
		}
		else
		if( D3D11Device_CheckFormatSupport( D_DXGI_FORMAT_R16G16B16A16_FLOAT, &FormatSupport ) == S_OK &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_TEXTURE2D     ) != 0 &&
			( FormatSupport & D_D3D11_FORMAT_SUPPORT_RENDER_TARGET ) != 0 )
		{
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_F32 ] = D_DXGI_FORMAT_R16G16B16A16_FLOAT ;
			String = "DXGI_FORMAT_R16G16B16A16_FLOAT" ;
		}
		else
		{
			DXST_LOGFILE_ADDA( "\x8e\x67\x97\x70\x82\xc5\x82\xab\x82\xe9\x95\x60\x89\xe6\x97\x70\x82\x51\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x95\x82\x93\xae\x8f\xac\x90\x94\x93\x5f 32 \x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xaa\x82\xa0\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x82\xc5\x82\xb5\x82\xbd\n"/*@ "使用できる描画用２チャンネル浮動小数点 32 ビット型カラーフォーマットがありませんでした\n" @*/ ) ;
			GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_F32 ] = D_DXGI_FORMAT_UNKNOWN ;
		}

		if( GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_F32 ] != D_DXGI_FORMAT_UNKNOWN )
		{
			DXST_LOGFILEFMT_ADDA(( "\x95\x60\x89\xe6\x97\x70\x82\x51\x83\x60\x83\x83\x83\x93\x83\x6c\x83\x8b\x95\x82\x93\xae\x8f\xac\x90\x94\x93\x5f 32 \x83\x72\x83\x62\x83\x67\x8c\x5e\x83\x4a\x83\x89\x81\x5b\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcd %s \x82\xc5\x82\xb7"/*@ "描画用２チャンネル浮動小数点 32 ビット型カラーフォーマットは %s です" @*/, String )) ;
		}

		// マスクカラーバッファ用フォーマットの選定
		GD3D11.Device.Caps.MaskColorFormat = GSYS.Screen.MainScreenColorBitDepth == 32 ? GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_RGB32 ] : GD3D11.Device.Caps.TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_RGB16 ] ;

		// マスクアルファフォーマットの選定
		GD3D11.Device.Caps.MaskAlphaFormat = D_DXGI_FORMAT_R8_UNORM ;
	}

	// 情報出力
	{
		switch( GD3D11.Setting.FeatureLevel )
		{
		case D_D3D_FEATURE_LEVEL_9_1 :
			String = "D3D_FEATURE_LEVEL_9_1" ;
			break ;

		case D_D3D_FEATURE_LEVEL_9_2 :
			String = "D3D_FEATURE_LEVEL_9_2" ;
			break ;

		case D_D3D_FEATURE_LEVEL_9_3 :
			String = "D3D_FEATURE_LEVEL_9_3" ;
			break ;

		case D_D3D_FEATURE_LEVEL_10_0 :
			String = "D3D_FEATURE_LEVEL_10_0" ;
			break ;

		case D_D3D_FEATURE_LEVEL_10_1 :
			String = "D3D_FEATURE_LEVEL_10_1" ;
			break ;

		case D_D3D_FEATURE_LEVEL_11_0 :
			String = "D3D_FEATURE_LEVEL_11_0" ;
			break ;

		case D_D3D_FEATURE_LEVEL_11_1 :
			String = "D3D_FEATURE_LEVEL_11_1" ;
			break ;
		}
		DXST_LOGFILEFMT_ADDA(( "\x8e\x67\x97\x70\x82\xb7\x82\xe9\x8b\x40\x94\x5c\x83\x8c\x83\x78\x83\x8b:%s"/*@ "使用する機能レベル:%s" @*/, String )) ;
//		DXST_LOGFILEFMT_ADDA(( "対応している最大頂点インデックス:%d",     GSYS.HardInfo.MaxVertexIndex )) ;
		DXST_LOGFILEFMT_ADDA(( "\x93\xaf\x8e\x9e\x82\xc9\x83\x8c\x83\x93\x83\x5f\x83\x8a\x83\x93\x83\x4f\x82\xc5\x82\xab\x82\xe9\x83\x6f\x83\x62\x83\x74\x83\x40\x82\xcc\x90\x94:%d"/*@ "同時にレンダリングできるバッファの数:%d" @*/, GSYS.HardInfo.RenderTargetNum )) ;
		DXST_LOGFILEFMT_ADDA(( "\x8d\xc5\x91\xe5\x83\x65\x83\x4e\x83\x58\x83\x60\x83\x83\x83\x54\x83\x43\x83\x59\x81\x40\x95\x9d:%d \x8d\x82\x82\xb3:%d"/*@ "最大テクスチャサイズ　幅:%d 高さ:%d" @*/, GSYS.HardInfo.MaxTextureWidth, GSYS.HardInfo.MaxTextureHeight )) ;
	}

	// カラーフォーマット情報の初期化を行う
	Graphics_D3D11_GetD3DFormatColorData( D_DXGI_FORMAT_R8G8B8A8_UNORM ) ;

	// 終了
	return 0 ;
}

// Direct3DDevice11 関係の初期化
extern	int		Graphics_D3D11_Device_Initialize( void )
{
	int i ;

#ifndef DX_NON_FILTER
	GraphFilter_Initialize() ;
#endif // DX_NON_FILTER

	// 描画ステータスのセット
	{
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck = TRUE ;

		// ラスタライズ関係の初期値をセット
		GD3D11.Device.State.RasterizerDesc.FillMode              = D_D3D11_FILL_SOLID ;
		GD3D11.Device.State.RasterizerDesc.CullMode              = D_D3D11_CULL_NONE ;
		GD3D11.Device.State.RasterizerDesc.FrontCounterClockwise = 0 ;
		GD3D11.Device.State.RasterizerDesc.DepthBias             = 0 ;
		GD3D11.Device.State.RasterizerDesc.DepthBiasClamp        = 0.0f ;
		GD3D11.Device.State.RasterizerDesc.SlopeScaledDepthBias  = 0.0f ;
		GD3D11.Device.State.RasterizerDesc.DepthClipEnable       = TRUE ;
		GD3D11.Device.State.RasterizerDesc.ScissorEnable         = FALSE ;
		GD3D11.Device.State.RasterizerDesc.MultisampleEnable     = TRUE ;
		GD3D11.Device.State.RasterizerDesc.AntialiasedLineEnable = FALSE ;

		// 深度ステンシル関係の初期値をセット
		GD3D11.Device.State.DepthStencilDesc.DepthEnable					= FALSE ;
		GD3D11.Device.State.DepthStencilDesc.DepthWriteMask					= D_D3D11_DEPTH_WRITE_MASK_ZERO ;
		GD3D11.Device.State.DepthStencilDesc.DepthFunc						= D_D3D11_COMPARISON_LESS_EQUAL ;
		GD3D11.Device.State.DepthStencilDesc.StencilEnable					= FALSE ;
		GD3D11.Device.State.DepthStencilDesc.StencilReadMask				= D_D3D11_DEFAULT_STENCIL_READ_MASK ;
		GD3D11.Device.State.DepthStencilDesc.StencilWriteMask				= D_D3D11_DEFAULT_STENCIL_WRITE_MASK ;
		GD3D11.Device.State.DepthStencilDesc.FrontFace.StencilFailOp		= D_D3D11_STENCIL_OP_KEEP ;
		GD3D11.Device.State.DepthStencilDesc.FrontFace.StencilDepthFailOp	= D_D3D11_STENCIL_OP_KEEP ;
		GD3D11.Device.State.DepthStencilDesc.FrontFace.StencilPassOp		= D_D3D11_STENCIL_OP_KEEP ;
		GD3D11.Device.State.DepthStencilDesc.FrontFace.StencilFunc			= D_D3D11_COMPARISON_ALWAYS ;
		GD3D11.Device.State.DepthStencilDesc.BackFace.StencilFailOp			= D_D3D11_STENCIL_OP_KEEP ;
		GD3D11.Device.State.DepthStencilDesc.BackFace.StencilDepthFailOp	= D_D3D11_STENCIL_OP_KEEP ;
		GD3D11.Device.State.DepthStencilDesc.BackFace.StencilPassOp			= D_D3D11_STENCIL_OP_KEEP ;
		GD3D11.Device.State.DepthStencilDesc.BackFace.StencilFunc			= D_D3D11_COMPARISON_ALWAYS ;

		// ブレンドステート関係の初期値をセット
		GD3D11.Device.State.BlendDesc.AlphaToCoverageEnable		= FALSE ;
		GD3D11.Device.State.BlendDesc.IndependentBlendEnable	= FALSE ;
		for( i = 0 ; i < 8 ; i ++ )
		{
			GD3D11.Device.State.BlendDesc.RenderTarget[ i ].BlendEnable				= FALSE ;
			GD3D11.Device.State.BlendDesc.RenderTarget[ i ].SrcBlend				= D_D3D11_BLEND_ONE ;
			GD3D11.Device.State.BlendDesc.RenderTarget[ i ].DestBlend				= D_D3D11_BLEND_ZERO ;
			GD3D11.Device.State.BlendDesc.RenderTarget[ i ].BlendOp					= D_D3D11_BLEND_OP_ADD ;
			GD3D11.Device.State.BlendDesc.RenderTarget[ i ].SrcBlendAlpha			= D_D3D11_BLEND_ONE ;
			GD3D11.Device.State.BlendDesc.RenderTarget[ i ].DestBlendAlpha			= D_D3D11_BLEND_ZERO ;
			GD3D11.Device.State.BlendDesc.RenderTarget[ i ].BlendOpAlpha			= D_D3D11_BLEND_OP_ADD ;
			GD3D11.Device.State.BlendDesc.RenderTarget[ i ].RenderTargetWriteMask	= D_D3D11_COLOR_WRITE_ENABLE_ALL ;
		}

		// サンプラーステート関係の初期値をセット
		for( i = 0 ; i < USE_TEXTURESTAGE_NUM ; i ++ )
		{
			GD3D11.Device.State.SamplerDesc[ i ].Filter				= D_D3D11_FILTER_MIN_MAG_MIP_POINT ;
			GD3D11.Device.State.SamplerDesc[ i ].AddressU			= D_D3D11_TEXTURE_ADDRESS_CLAMP ;
			GD3D11.Device.State.SamplerDesc[ i ].AddressV			= D_D3D11_TEXTURE_ADDRESS_CLAMP ;
			GD3D11.Device.State.SamplerDesc[ i ].AddressW			= D_D3D11_TEXTURE_ADDRESS_CLAMP ;
			GD3D11.Device.State.SamplerDesc[ i ].MipLODBias			= 0.0f ;
			GD3D11.Device.State.SamplerDesc[ i ].MaxAnisotropy		= 16 ;
			GD3D11.Device.State.SamplerDesc[ i ].ComparisonFunc		= D_D3D11_COMPARISON_ALWAYS ;
			GD3D11.Device.State.SamplerDesc[ i ].BorderColor[ 0 ]	= 0.0f ;
			GD3D11.Device.State.SamplerDesc[ i ].BorderColor[ 1 ]	= 0.0f ;
			GD3D11.Device.State.SamplerDesc[ i ].BorderColor[ 2 ]	= 0.0f ;
			GD3D11.Device.State.SamplerDesc[ i ].BorderColor[ 3 ]	= 0.0f ;
			GD3D11.Device.State.SamplerDesc[ i ].MinLOD				= -D_D3D11_FLOAT32_MAX ;
			GD3D11.Device.State.SamplerDesc[ i ].MaxLOD				=  D_D3D11_FLOAT32_MAX ;
		}

		// テクスチャアドレスモード設定
		for( i = 0 ; i < USE_TEXTURESTAGE_NUM ; i ++ )
		{
			if( GSYS.DrawSetting.TexAddressModeU[ i ] == 0 )
			{
				GSYS.DrawSetting.TexAddressModeU[ i ] = D_D3D11_TEXTURE_ADDRESS_CLAMP ;
			}

			if( GSYS.DrawSetting.TexAddressModeV[ i ] == 0 )
			{
				GSYS.DrawSetting.TexAddressModeV[ i ] = D_D3D11_TEXTURE_ADDRESS_CLAMP ;
			}

			if( GSYS.DrawSetting.TexAddressModeW[ i ] == 0 )
			{
				GSYS.DrawSetting.TexAddressModeW[ i ] = D_D3D11_TEXTURE_ADDRESS_CLAMP ;
			}
		}

		// 描画モードをセット
		Graphics_D3D11_DeviceState_SetDrawMode( DX_DRAWMODE_NEAREST ) ;

		// 最大異方性をセット
		Graphics_D3D11_DeviceState_SetMaxAnisotropy( 16 ) ;

		// テクスチャ座標変換行列を使用するかどうかの設定をセット
		Graphics_D3D11_DeviceState_SetTextureAddressTransformMatrix(
			FALSE,
			&D3D11_GlobalIdentMatrix ) ;

		// ライティングＯＮ
		Graphics_D3D11_DeviceState_SetLighting( TRUE ) ;

		// マテリアルのパラメータを設定
		{
			MATERIALPARAM Material ;
			Material.Diffuse.r  = 0.8f ;
			Material.Diffuse.g  = 0.8f ;
			Material.Diffuse.b  = 0.8f ;
			Material.Diffuse.a  = 1.0f ;
			Material.Specular.r = 0.8f ;
			Material.Specular.g = 0.8f ;
			Material.Specular.b = 0.8f ;
			Material.Specular.a = 0.0f ;
			Material.Emissive.r = 0.0f ;
			Material.Emissive.g = 0.0f ;
			Material.Emissive.b = 0.0f ;
			Material.Emissive.a = 0.0f ;
			Material.Ambient.r  = 0.0f ;
			Material.Ambient.g  = 0.0f ;
			Material.Ambient.b  = 0.0f ;
			Material.Ambient.a  = 0.0f ;
			Material.Power      = 20.0f ;
			Graphics_D3D11_DeviceState_SetMaterial( &Material ) ;
		}

		// 頂点のカラー値をマテリアルのディフューズカラーやスペキュラカラーとして使用するかどうかの設定を初期化
		Graphics_D3D11_DeviceState_SetUseVertexDiffuseColor( TRUE ) ;
		Graphics_D3D11_DeviceState_SetUseVertexSpecularColor( TRUE ) ;

		// スペキュラ処理を使用するかどうかを初期化
		Graphics_D3D11_DeviceState_SetSpecularEnable( TRUE ) ;

		// フォグ関係の設定
		Graphics_D3D11_DeviceState_SetFogEnable( FALSE ) ;
		Graphics_D3D11_DeviceState_SetFogColor( 0 ) ;
		Graphics_D3D11_DeviceState_SetFogVertexMode( DX_FOGMODE_NONE ) ;
		Graphics_D3D11_DeviceState_SetFogStartEnd( 0.0f, 1.0f ) ;
		Graphics_D3D11_DeviceState_SetFogDensity( 1.0f ) ;

		// グローバルアンビエントカラーセット
		{
			COLOR_F GAmbColor = { 0.0f } ;

			Graphics_D3D11_DeviceState_SetAmbient( &GAmbColor ) ;
		}

		// ビューポートを設定
		{
			D_D3D11_VIEWPORT Viewport ;

			Viewport.TopLeftX	= 0 ;
			Viewport.TopLeftY	= 0 ;
			Viewport.Width		= ( FLOAT )GSYS.Screen.MainScreenSizeX ;
			Viewport.Height		= ( FLOAT )GSYS.Screen.MainScreenSizeY ;
			Viewport.MinDepth	= 0.0f ;
			Viewport.MaxDepth	= 1.0f ;
			Graphics_D3D11_DeviceState_SetViewport( &Viewport ) ;
		}

		// 描画画像のＲＧＢを無視するかどうかをセット
		Graphics_D3D11_DrawSetting_SetIgnoreDrawGraphColor( FALSE ) ;

		// 描画画像のＡを無視するかどうかをセット
		Graphics_D3D11_DrawSetting_SetIgnoreDrawGraphAlpha( FALSE ) ;

		// 描画先設定
		Graphics_D3D11_DeviceState_SetRenderTarget(
			GD3D11.Device.Screen.OutputWindowInfo[ 0 ].BufferTexture2D,
			GD3D11.Device.Screen.OutputWindowInfo[ 0 ].BufferRTV
		) ;

		// ブレンドモード設定
		Graphics_D3D11_DeviceState_SetBlendMode( DX_BLENDMODE_NOBLEND ) ;

		// プリミティブタイプをセット
		Graphics_D3D11_DeviceState_SetPrimitiveTopology( D_D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) ;

		// アルファテスト設定を初期化
		Graphics_D3D11_DrawSetting_SetDrawAlphaTest( -1, 0 ) ;

		// ブレンドテクスチャ関係を初期化
		Graphics_D3D11_DrawSetting_SetBlendTexture( NULL, NULL ) ;
		GD3D11.Device.DrawSetting.BlendGraphBorderParam	= -1 ;
		GD3D11.Device.DrawSetting.BlendGraphType		= 0 ;

		// 描画テクスチャ関係を初期化
		Graphics_D3D11_DrawSetting_SetTexture( NULL, NULL ) ;
		GD3D11.Device.DrawSetting.AlphaChannelValidFlag	= FALSE ;
		GD3D11.Device.DrawSetting.AlphaTestValidFlag	= FALSE ;

		GD3D11.Device.DrawSetting.CancelSettingEqualCheck = FALSE ;
	}

	// デバイス設定をリフレッシュ
	Graphics_D3D11_DeviceState_RefreshRenderState() ;

	// フラグを初期化
	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag	= TRUE ;
	GD3D11.Device.DrawSetting.ChangeBlendParamFlag	= TRUE ;
	GD3D11.Device.DrawSetting.ChangeTextureFlag		= FALSE ;

	// 描画情報を初期化
	GD3D11.Device.DrawInfo.DiffuseColor				= 0xffffffff ;
	GD3D11.Device.DrawInfo.BlendMaxNotDrawFlag		= FALSE ;
	GD3D11.Device.DrawInfo.BeginSceneFlag			= FALSE ;
	GD3D11.Device.DrawInfo.VertexNum				= 0 ;
	GD3D11.Device.DrawInfo.Use3DVertex				= 0 ;
	GD3D11.Device.DrawInfo.VertexType				= VERTEXTYPE_TEX ;

#if VERTEXBUFFERMODE == 2
	NORMALVERTEXBUFFER_MAP
	GD3D11.Device.DrawInfo.VertexBufferNextAddr		= GD3D11.Device.DrawInfo.VertexBufferAddr ;
#else
	GD3D11.Device.DrawInfo.VertexBufferNextAddr		= GD3D11.Device.DrawInfo.VertexBufferTemp ;
#endif

	// 終了
	return 0 ;
}

// 一時的に Direct3DDevice11 を破棄した際に、破棄前の状態を作り出すための関数
extern	int		Graphics_D3D11_Device_ReInitialize( void )
{
	int i ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// 初期化フラグを立てる
	GD3D11.Device.DrawSetting.CancelSettingEqualCheck = TRUE ;

	// 必ず Graphics_D3D9_DrawPreparation を実行するフラグを立てる
	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// 削除前と同じ環境を作り出す
	{
		// 描画テクスチャ関係を初期化
		Graphics_D3D11_DrawSetting_SetTexture( NULL, NULL ) ;
		GD3D11.Device.DrawSetting.AlphaChannelValidFlag	= FALSE ;
		GD3D11.Device.DrawSetting.AlphaTestValidFlag	= FALSE ;

		// 描画頂点情報もリセット
		GD3D11.Device.DrawInfo.VertexNum = 0 ;

		// 描画ブレンドモードの設定
		Graphics_D3D11_DrawSetting_SetDrawBlendMode( GD3D11.Device.DrawSetting.BlendMode, GD3D11.Device.DrawSetting.AlphaTestValidFlag, GD3D11.Device.DrawSetting.AlphaChannelValidFlag ) ;

		// 描画画像のＲＧＢを無視するかどうかをセットする
		Graphics_D3D11_DrawSetting_SetIgnoreDrawGraphColor( GD3D11.Device.DrawSetting.IgnoreGraphColorFlag ) ;

		// 描画画像のＡを無視するかどうかをセット
		Graphics_D3D11_DrawSetting_SetIgnoreDrawGraphAlpha( GD3D11.Device.DrawSetting.IgnoreGraphAlphaFlag ) ;

		// 異方性フィルタリングの設定をセット
		Graphics_D3D11_DeviceState_SetMaxAnisotropy( GD3D11.Device.State.MaxAnisotropy ) ;

		// UVアドレッシングモードの設定をセット
		for( i = 0 ; i < USE_TEXTURESTAGE_NUM ; i ++ )
		{
			Graphics_D3D11_DeviceState_SetTextureAddressUVW(
				GD3D11.Device.State.SamplerDesc[ i ].AddressU,
				GD3D11.Device.State.SamplerDesc[ i ].AddressV,
				GD3D11.Device.State.SamplerDesc[ i ].AddressW,
				i ) ;
		}

		// テクスチャ座標変換行列を使用するかどうかの設定をセット
		Graphics_D3D11_DeviceState_SetTextureAddressTransformMatrix(
			GD3D11.Device.State.TextureAddressTransformMatrixUse,
			&GD3D11.Device.State.TextureAddressTransformMatrix ) ;

		// プリミティブタイプをセット
		Graphics_D3D11_DeviceState_SetPrimitiveTopology( GD3D11.Device.State.PrimitiveTopology ) ;

		// フォグの設定をセット
		Graphics_D3D11_DeviceState_SetFogEnable(     GD3D11.Device.State.FogEnable ) ;
		Graphics_D3D11_DeviceState_SetFogVertexMode( GD3D11.Device.State.FogMode ) ;
		Graphics_D3D11_DeviceState_SetFogColor(      GD3D11.Device.State.FogColor ) ;
		Graphics_D3D11_DeviceState_SetFogStartEnd(   GD3D11.Device.State.FogStart, GD3D11.Device.State.FogEnd ) ;
		Graphics_D3D11_DeviceState_SetFogDensity(    GD3D11.Device.State.FogDensity ) ;

		// 行列の再設定
		NS_SetTransformToWorldD(                              &GSYS.DrawSetting.WorldMatrix ) ;
		NS_SetTransformToViewD(                               &GSYS.DrawSetting.ViewMatrix ) ;
		Graphics_DrawSetting_SetTransformToProjection_Direct( &GSYS.DrawSetting.ProjectionMatrix ) ;
		NS_SetTransformToViewportD(                           &GSYS.DrawSetting.ViewportMatrix ) ;

		// ライトの再設定
		Graphics_D3D11_DeviceState_SetAmbient( &GD3D11.Device.State.GlobalAmbientLightColor ) ;
		Graphics_D3D11_DeviceState_SetLighting( GD3D11.Device.State.Lighting ) ;
		for( i = 0 ; i < DX_D3D11_COMMON_CONST_LIGHT_NUM ; i ++ )
		{
			if( GD3D11.Device.State.LightParam[ i ].LightType == 0 )
			{
				continue ;
			}

			Graphics_D3D11_DeviceState_SetLightState( i, &GD3D11.Device.State.LightParam[ i ] ) ;
			Graphics_D3D11_DeviceState_SetLightEnable( i, GD3D11.Device.State.LightEnableFlag[ i ] ) ;
		}
	}

	// 初期化フラグを倒す
	GD3D11.Device.DrawSetting.CancelSettingEqualCheck = FALSE ;

	// 上記以外の設定をリフレッシュ
	Graphics_D3D11_DeviceState_RefreshRenderState() ;

	// 終了
	return 0 ;
}



























// DXGIAdapter 関係

// DXGIオブジェクト情報の列挙
extern int EnumDXGIAdapterInfo( void )
{
	D_IDXGIAdapter *DXGIAdapter ;
	D_IDXGIOutput  *DXGIOutput ;
	int            AfterRelease = FALSE ;
	GRAPHICS_HARDWARE_DIRECT3D11_ADAPTER_UNIT *AdapterInfo ;

	if( GD3D11.Adapter.ValidInfo )
	{
		return 0 ;
	}

	if( DXGIFactory_IsValid() == FALSE )
	{
		AfterRelease = TRUE ;

		// dxgi.DLL の読み込み
		if( DXGI_LoadDLL() < 0 )
		{
			return -1 ;
		}

		// IDXGIFactory の作成
		if( DXGIFactory_Create() < 0 )
		{
			return -1 ;
		}
	}

	AdapterInfo = GD3D11.Adapter.Info ;
	for( GD3D11.Adapter.InfoNum = 0 ; GD3D11.Adapter.InfoNum < DX_D3D11_MAX_ADAPTER_LISTUP ; GD3D11.Adapter.InfoNum ++, AdapterInfo ++ )
	{
		if( DXGIFactory_EnumAdapters( ( UINT )GD3D11.Adapter.InfoNum, &DXGIAdapter ) != S_OK )
		{
			break ;
		}

		DXGIAdapter_GetDesc( DXGIAdapter, &AdapterInfo->Desc ) ;

		for( AdapterInfo->OutputNum = 0 ; AdapterInfo->OutputNum < DX_D3D11_MAX_OUTPUT_LISTUP ; AdapterInfo->OutputNum ++ )
		{
			if( DXGIAdapter_EnumOutputs( DXGIAdapter, ( UINT )AdapterInfo->OutputNum, &DXGIOutput ) != S_OK )
			{
				break ;
			}

			DXGIOutput_GetDesc( DXGIOutput, &AdapterInfo->OutputDesc[ AdapterInfo->OutputNum ] ) ;

			Direct3D11_ObjectRelease( DXGIOutput ) ;
		}

		Direct3D11_ObjectRelease( DXGIAdapter ) ;
	}

	if( AfterRelease )
	{
		// IDXGIFactory の解放
		DXGIFactory_Release() ;

		// dxgi.dll の解放
		DXGI_FreeDLL() ;
	}

	GD3D11.Adapter.ValidInfo = TRUE ;

	// 正常終了
	return 0 ;
}


































// D3D11Device ステータス関係

// 描画設定をリフレッシュ
extern	void	Graphics_D3D11_DeviceState_RefreshRenderState( void )
{
	if( GAPIWin.D3D11DeviceObject == NULL ) return ;

	// 初期化フラグを立てる
	GD3D11.Device.DrawSetting.CancelSettingEqualCheck = TRUE ;

	// Graphics_D3D11_DrawPreparation を行うべきフラグを立てる
	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// 設定のリフレッシュ
	Graphics_D3D11_DeviceState_SetFogEnable             ( GD3D11.Device.State.FogEnable           ) ;
	Graphics_D3D11_DeviceState_SetViewport              ( &GD3D11.Device.State.Viewport           ) ;
//	Graphics_D3D11_DeviceState_SetScissorRect           ( &GD3D11.Device.State.ScissorRect        ) ;
	Graphics_D3D11_DrawSetting_SetTexture               ( GD3D11.Device.DrawSetting.RenderTexture, GD3D11.Device.DrawSetting.RenderTextureSRV ) ;
	Graphics_D3D11_DeviceState_SetBaseState             (                                        ) ;
	Graphics_D3D11_DeviceState_SetDrawMode              ( GD3D11.Device.State.DrawMode            ) ;
//	Graphics_D3D11_DrawSetting_SetDrawBlendMode         ( GD3D11.Device.DrawSetting.BlendMode, GD3D11.Device.DrawSetting.AlphaChannelValidFlag  ) ;
	Graphics_D3D11_DeviceState_NormalDrawSetup          (                                        ) ;
	Graphics_D3D11_DeviceState_SetDepthEnable           ( GD3D11.Device.State.DepthStencilDesc.DepthEnable ) ;
	Graphics_D3D11_DeviceState_SetDepthWriteEnable      ( GD3D11.Device.State.DepthStencilDesc.DepthWriteMask == D_D3D11_DEPTH_WRITE_MASK_ALL ? TRUE : FALSE ) ;
	Graphics_D3D11_DeviceState_SetDepthFunc             ( GD3D11.Device.State.DepthStencilDesc.DepthFunc ) ;
	Graphics_D3D11_DeviceState_SetDepthBias             ( GD3D11.Device.State.RasterizerDesc.DepthBias ) ;
	Graphics_D3D11_DeviceState_SetFillMode              ( GD3D11.Device.State.RasterizerDesc.FillMode  ) ;
	Graphics_D3D11_DeviceState_SetCullMode              ( GD3D11.Device.State.RasterizerDesc.CullMode == D_D3D11_CULL_FRONT ? 2 : ( GD3D11.Device.State.RasterizerDesc.CullMode == D_D3D11_CULL_BACK ? 1 : 0 ) ) ;
//	Graphics_D3D11_DeviceState_SetShadeMode             ( GD3D11.Device.State.ShadeMode           ) ;
	Graphics_D3D11_DeviceState_SetMaterial              ( &GD3D11.Device.State.Material           ) ;
	Graphics_D3D11_DeviceState_SetSpecularEnable        ( GD3D11.Device.State.UseSpecular         ) ;
	Graphics_D3D11_DeviceState_SetUseVertexDiffuseColor ( GD3D11.Device.State.MaterialUseVertexDiffuseColor ) ;
	Graphics_D3D11_DeviceState_SetUseVertexSpecularColor( GD3D11.Device.State.MaterialUseVertexSpecularColor ) ;
	Graphics_D3D11_DeviceState_SetPrimitiveTopology     ( GD3D11.Device.State.PrimitiveTopology   ) ;
	Graphics_D3D11_DeviceState_SetInputLayout			( GD3D11.Device.State.SetVertexInputLayout ) ;
	Graphics_D3D11_DeviceState_SetVertexShader			( GD3D11.Device.State.SetVertexShader, GD3D11.Device.State.SetNormalVertexShader ) ;
	Graphics_D3D11_DeviceState_SetIndexBuffer			( GD3D11.Device.State.SetIndexBuffer, GD3D11.Device.State.SetIndexBufferFormat ) ;
	Graphics_D3D11_DeviceState_SetPixelShader			( GD3D11.Device.State.SetPixelShader, GD3D11.Device.State.SetNormalPixelShader ) ;
	Graphics_D3D11_DeviceState_SetVertexBuffer			( GD3D11.Device.State.SetVertexBuffer, GD3D11.Device.State.SetVertexBufferStride ) ;

	// 定数バッファをリフレッシュ
	GD3D11.Device.Shader.Constant.ConstBuffer_Common->ChangeFlag = TRUE ;
	Graphics_D3D11_ConstantBuffer_Update( GD3D11.Device.Shader.Constant.ConstBuffer_Common ) ;

	GD3D11.Device.Shader.Constant.ConstBuffer_VS_Base->ChangeFlag = TRUE ;
	Graphics_D3D11_ConstantBuffer_Update( GD3D11.Device.Shader.Constant.ConstBuffer_VS_Base ) ;

	GD3D11.Device.Shader.Constant.ConstBuffer_VS_OtherMatrix->ChangeFlag = TRUE ;
	Graphics_D3D11_ConstantBuffer_Update( GD3D11.Device.Shader.Constant.ConstBuffer_VS_OtherMatrix ) ;

	GD3D11.Device.Shader.Constant.ConstBuffer_VS_LocalWorldMatrix->ChangeFlag = TRUE ;
	Graphics_D3D11_ConstantBuffer_Update( GD3D11.Device.Shader.Constant.ConstBuffer_VS_LocalWorldMatrix ) ;

	GD3D11.Device.Shader.Constant.ConstBuffer_PS_Base->ChangeFlag = TRUE ;
	Graphics_D3D11_ConstantBuffer_Update( GD3D11.Device.Shader.Constant.ConstBuffer_PS_Base ) ;

	GD3D11.Device.Shader.Constant.ConstBuffer_PS_ShadowMap->ChangeFlag = TRUE ;
	Graphics_D3D11_ConstantBuffer_Update( GD3D11.Device.Shader.Constant.ConstBuffer_PS_ShadowMap ) ;

	GD3D11.Device.Shader.Constant.ConstBuffer_PS_Filter->ChangeFlag = TRUE ;
	Graphics_D3D11_ConstantBuffer_Update( GD3D11.Device.Shader.Constant.ConstBuffer_PS_Filter ) ;

	// 定数バッファをセット
	Graphics_D3D11_ConstantBuffer_VSSet( DX_D3D11_VS_CONSTANTBUFFER_COMMON,      1, &GD3D11.Device.Shader.Constant.ConstBuffer_Common ) ;
	Graphics_D3D11_ConstantBuffer_VSSet( DX_D3D11_VS_CONSTANTBUFFER_BASE,        1, &GD3D11.Device.Shader.Constant.ConstBuffer_VS_Base ) ;
	Graphics_D3D11_ConstantBuffer_VSSet( DX_D3D11_VS_CONSTANTBUFFER_OTHERMATRIX, 1, &GD3D11.Device.Shader.Constant.ConstBuffer_VS_OtherMatrix ) ;
	Graphics_D3D11_ConstantBuffer_VSSet( DX_D3D11_VS_CONSTANTBUFFER_MATRIX,      1, &GD3D11.Device.Shader.Constant.ConstBuffer_VS_LocalWorldMatrix ) ;

	Graphics_D3D11_ConstantBuffer_PSSet( DX_D3D11_PS_CONSTANTBUFFER_COMMON,      1, &GD3D11.Device.Shader.Constant.ConstBuffer_Common ) ;
	Graphics_D3D11_ConstantBuffer_PSSet( DX_D3D11_PS_CONSTANTBUFFER_BASE,        1, &GD3D11.Device.Shader.Constant.ConstBuffer_PS_Base ) ;
	Graphics_D3D11_ConstantBuffer_PSSet( DX_D3D11_PS_CONSTANTBUFFER_SHADOWMAP,   1, &GD3D11.Device.Shader.Constant.ConstBuffer_PS_ShadowMap ) ;
	Graphics_D3D11_ConstantBuffer_PSSet( DX_D3D11_PS_CONSTANTBUFFER_FILTER,      1, &GD3D11.Device.Shader.Constant.ConstBuffer_PS_Filter ) ;

	// 初期化フラグを倒す
	GD3D11.Device.DrawSetting.CancelSettingEqualCheck = FALSE ;
}

// Direct3DDevice11 の基本設定を行う
extern int Graphics_D3D11_DeviceState_SetBaseState( void )
{
	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// 書き出し
	DRAWSTOCKINFO

	// 使用しないシェーダーを無効化
	D3D11DeviceContext_GSSetShader( NULL, NULL, 0 ) ;
	D3D11DeviceContext_HSSetShader( NULL, NULL, 0 ) ;
	D3D11DeviceContext_DSSetShader( NULL, NULL, 0 ) ;
	D3D11DeviceContext_CSSetShader( NULL, NULL, 0 ) ;

	// サンプラー設定再セットアップ
	Graphics_D3D11_DeviceState_SetupSamplerState() ;

	// ラスタライザステート再セットアップ
	Graphics_D3D11_DeviceState_SetupRasterizerState() ;

	// 深度ステンシルステート再セットアップ
	Graphics_D3D11_DeviceState_SetupDepthStencilState() ;

	// ブレンドステート再セットアップ
	Graphics_D3D11_DeviceState_SetupBlendState() ;

	// 終了
	return 0 ;
}

// サンプラーのテクスチャフィルタリングモードを設定する
extern int Graphics_D3D11_DeviceState_SetSampleFilterMode( D_D3D11_FILTER Filter, int Sampler )
{
	int						i ;
	D_D3D11_SAMPLER_DESC	*SamplerDesc ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	if( Sampler < 0 )
	{
		if( Filter == D_D3D11_FILTER_MIN_MAG_MIP_LINEAR ||
			Filter == D_D3D11_FILTER_MIN_MAG_MIP_POINT ||
			Filter == D_D3D11_FILTER_ANISOTROPIC )
		{
			GD3D11.Device.State.DrawMode = -1 ;
		}

		if( GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
		{
			SamplerDesc = GD3D11.Device.State.SamplerDesc ;
			for( i = 0 ; i < USE_TEXTURESTAGE_NUM ; i ++, SamplerDesc ++ )
			{
				if( SamplerDesc->Filter != Filter )
				{
					break ;
				}
			}
			if( i == USE_TEXTURESTAGE_NUM )
			{
				return 0 ;
			}
		}

		DRAWSTOCKINFO

		SamplerDesc = GD3D11.Device.State.SamplerDesc ;
		for( i = 0 ; i < USE_TEXTURESTAGE_NUM ; i ++, SamplerDesc ++ )
		{
			if( SamplerDesc->Filter == Filter &&
				GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
			{
				continue ;
			}

			GD3D11.Device.State.ChangeSamplerDescSlot[ i ] = TRUE ;
			GD3D11.Device.State.ChangeSamplerDesc = TRUE ;

			SamplerDesc->Filter = Filter ;
		}
	}
	else
	{
		if( Sampler < 0 || Sampler >= USE_TEXTURESTAGE_NUM )
		{
			return 0 ;
		}

		GD3D11.Device.State.DrawMode = -1 ;

		SamplerDesc = &GD3D11.Device.State.SamplerDesc[ Sampler ] ;
		if( SamplerDesc->Filter == Filter &&
			GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
		{
			return 0 ;
		}

		DRAWSTOCKINFO

		GD3D11.Device.State.ChangeSamplerDescSlot[ Sampler ] = TRUE ;
		GD3D11.Device.State.ChangeSamplerDesc = TRUE ;

		SamplerDesc->Filter = Filter ;
	}

	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

//	return Graphics_D3D11_DeviceState_SetupSamplerState() ;
	return 0 ;
}

// 深度バッファの有効無効のセット
extern int Graphics_D3D11_DeviceState_SetDepthEnable( BOOL DepthEnable )
{
	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// モードが同じだったら何もせず終了
	if( GD3D11.Device.State.DepthStencilDesc.DepthEnable == DepthEnable &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}
	
	DRAWSTOCKINFO

	GD3D11.Device.State.ChangeDepthStencilDesc = TRUE ;

	GD3D11.Device.State.DepthStencilDesc.DepthEnable = DepthEnable ;

	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

//	return Graphics_D3D11_DeviceState_SetupDepthStencilState() ;
	return 0 ;
}

// 深度バッファの書き込みの有無をセット
extern int Graphics_D3D11_DeviceState_SetDepthWriteEnable( int Flag )
{
	D_D3D11_DEPTH_WRITE_MASK DepthWriteMask ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	DepthWriteMask = Flag == FALSE ? D_D3D11_DEPTH_WRITE_MASK_ZERO : D_D3D11_DEPTH_WRITE_MASK_ALL ;

	// モードが同じだったら何もせず終了
	if( GD3D11.Device.State.DepthStencilDesc.DepthWriteMask == DepthWriteMask &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}
	
	DRAWSTOCKINFO

	GD3D11.Device.State.ChangeDepthStencilDesc = TRUE ;

	GD3D11.Device.State.DepthStencilDesc.DepthWriteMask = DepthWriteMask ;

	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

//	return Graphics_D3D11_DeviceState_SetupDepthStencilState() ;
	return 0 ;
}

// 深度値の比較タイプをセット
extern int Graphics_D3D11_DeviceState_SetDepthFunc( D_D3D11_COMPARISON_FUNC DepthFunc )
{
	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// モードが同じだったら何もせず終了
	if( GD3D11.Device.State.DepthStencilDesc.DepthFunc == DepthFunc &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}
	
	DRAWSTOCKINFO

	GD3D11.Device.State.ChangeDepthStencilDesc = TRUE ;

	GD3D11.Device.State.DepthStencilDesc.DepthFunc = DepthFunc ;

	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

//	return Graphics_D3D11_DeviceState_SetupDepthStencilState() ;
	return 0 ;
}

// 深度値のバイアスをセット
extern int Graphics_D3D11_DeviceState_SetDepthBias( int DepthBias )
{
	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// モードが同じだったら何もせず終了
	if( GD3D11.Device.State.RasterizerDesc.DepthBias == DepthBias &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}
	
	DRAWSTOCKINFO

	GD3D11.Device.State.ChangeRasterizerDesc = TRUE ;

	GD3D11.Device.State.RasterizerDesc.DepthBias = DepthBias ;

	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

//	return Graphics_D3D11_DeviceState_SetupRasterizerState() ;
	return 0 ;
}

// フィルモードをセット
extern int Graphics_D3D11_DeviceState_SetFillMode( D_D3D11_FILL_MODE FillMode )
{
	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// モードが同じだったら何もせず終了
	if( GD3D11.Device.State.RasterizerDesc.FillMode == FillMode &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}
	
	DRAWSTOCKINFO

	GD3D11.Device.State.ChangeRasterizerDesc = TRUE ;

	GD3D11.Device.State.RasterizerDesc.FillMode = FillMode ;

	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

//	return Graphics_D3D11_DeviceState_SetupRasterizerState() ;
	return 0 ;
}

// ワールド変換用行列をセットする
extern int Graphics_D3D11_DeviceState_SetWorldMatrix( const MATRIX *Matrix )
{
	DX_D3D11_VS_CONST_BUFFER_BASE *ConstantVSBase ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	ConstantVSBase = ( DX_D3D11_VS_CONST_BUFFER_BASE * )GD3D11.Device.Shader.Constant.ConstBuffer_VS_Base->SysmemBuffer ;

	// 定数データに反映
	ConstantVSBase->LocalWorldMatrix[ 0 ][ 0 ] = Matrix->m[ 0 ][ 0 ] ;
	ConstantVSBase->LocalWorldMatrix[ 0 ][ 1 ] = Matrix->m[ 1 ][ 0 ] ;
	ConstantVSBase->LocalWorldMatrix[ 0 ][ 2 ] = Matrix->m[ 2 ][ 0 ] ;
	ConstantVSBase->LocalWorldMatrix[ 0 ][ 3 ] = Matrix->m[ 3 ][ 0 ] ;
	ConstantVSBase->LocalWorldMatrix[ 1 ][ 0 ] = Matrix->m[ 0 ][ 1 ] ;
	ConstantVSBase->LocalWorldMatrix[ 1 ][ 1 ] = Matrix->m[ 1 ][ 1 ] ;
	ConstantVSBase->LocalWorldMatrix[ 1 ][ 2 ] = Matrix->m[ 2 ][ 1 ] ;
	ConstantVSBase->LocalWorldMatrix[ 1 ][ 3 ] = Matrix->m[ 3 ][ 1 ] ;
	ConstantVSBase->LocalWorldMatrix[ 2 ][ 0 ] = Matrix->m[ 0 ][ 2 ] ;
	ConstantVSBase->LocalWorldMatrix[ 2 ][ 1 ] = Matrix->m[ 1 ][ 2 ] ;
	ConstantVSBase->LocalWorldMatrix[ 2 ][ 2 ] = Matrix->m[ 2 ][ 2 ] ;
	ConstantVSBase->LocalWorldMatrix[ 2 ][ 3 ] = Matrix->m[ 3 ][ 2 ] ;

	GD3D11.Device.Shader.Constant.ConstBuffer_VS_Base->ChangeFlag = TRUE ;

	// 定数バッファを更新
//	Graphics_D3D11_ConstantBuffer_Update( GD3D11.Device.Shader.Constant.ConstBuffer_VS_Base ) ;

	// 終了
	return 0 ;
}

// ビュー変換用行列をセットする
extern int Graphics_D3D11_DeviceState_SetViewMatrix( const MATRIX *Matrix )
{
	DX_D3D11_VS_CONST_BUFFER_BASE *ConstantVSBase ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	ConstantVSBase = ( DX_D3D11_VS_CONST_BUFFER_BASE * )GD3D11.Device.Shader.Constant.ConstBuffer_VS_Base->SysmemBuffer ;

	// ライトの位置・方向の再計算
	{
		DX_D3D11_CONST_BUFFER_COMMON  *ConstantCommon = ( DX_D3D11_CONST_BUFFER_COMMON  * )GD3D11.Device.Shader.Constant.ConstBuffer_Common->SysmemBuffer ;
		DX_D3D11_CONST_LIGHT          *ConstantLight ;
		int                           i ;
		VECTOR                        Direction ;
		VECTOR                        Position ;

		ConstantLight = ConstantCommon->Light ;
		for( i = 0 ; i < DX_D3D11_COMMON_CONST_LIGHT_NUM ; i ++, ConstantLight ++ )
		{
			VectorTransformSR( &Direction, &GD3D11.Device.State.LightParam[ i ].Direction, Matrix ) ;
			VectorTransform(   &Position,  &GD3D11.Device.State.LightParam[ i ].Position,  Matrix ) ;
			ConstantLight->Position[ 0 ]  = Position.x ;
			ConstantLight->Position[ 1 ]  = Position.y ;
			ConstantLight->Position[ 2 ]  = Position.z ;
			ConstantLight->Direction[ 0 ] = Direction.x ;
			ConstantLight->Direction[ 1 ] = Direction.y ;
			ConstantLight->Direction[ 2 ] = Direction.z ;
		}

		GD3D11.Device.Shader.Constant.ConstBuffer_Common->ChangeFlag = TRUE ;

		// 定数バッファを更新
//		Graphics_D3D11_ConstantBuffer_Update( GD3D11.Device.Shader.Constant.ConstBuffer_Common ) ;
	}

	// 定数データに反映
	ConstantVSBase->ViewMatrix[ 0 ][ 0 ] = Matrix->m[ 0 ][ 0 ] ;
	ConstantVSBase->ViewMatrix[ 0 ][ 1 ] = Matrix->m[ 1 ][ 0 ] ;
	ConstantVSBase->ViewMatrix[ 0 ][ 2 ] = Matrix->m[ 2 ][ 0 ] ;
	ConstantVSBase->ViewMatrix[ 0 ][ 3 ] = Matrix->m[ 3 ][ 0 ] ;
	ConstantVSBase->ViewMatrix[ 1 ][ 0 ] = Matrix->m[ 0 ][ 1 ] ;
	ConstantVSBase->ViewMatrix[ 1 ][ 1 ] = Matrix->m[ 1 ][ 1 ] ;
	ConstantVSBase->ViewMatrix[ 1 ][ 2 ] = Matrix->m[ 2 ][ 1 ] ;
	ConstantVSBase->ViewMatrix[ 1 ][ 3 ] = Matrix->m[ 3 ][ 1 ] ;
	ConstantVSBase->ViewMatrix[ 2 ][ 0 ] = Matrix->m[ 0 ][ 2 ] ;
	ConstantVSBase->ViewMatrix[ 2 ][ 1 ] = Matrix->m[ 1 ][ 2 ] ;
	ConstantVSBase->ViewMatrix[ 2 ][ 2 ] = Matrix->m[ 2 ][ 2 ] ;
	ConstantVSBase->ViewMatrix[ 2 ][ 3 ] = Matrix->m[ 3 ][ 2 ] ;

	GD3D11.Device.Shader.Constant.ConstBuffer_VS_Base->ChangeFlag = TRUE ;

	// 定数バッファを更新
//	Graphics_D3D11_ConstantBuffer_Update( GD3D11.Device.Shader.Constant.ConstBuffer_VS_Base ) ;

	// 終了
	return 0 ;
}

// 投影変換用行列をセットする
extern int Graphics_D3D11_DeviceState_SetProjectionMatrix( const MATRIX *Matrix )
{
	DX_D3D11_VS_CONST_BUFFER_BASE *ConstantVSBase ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	ConstantVSBase = ( DX_D3D11_VS_CONST_BUFFER_BASE * )GD3D11.Device.Shader.Constant.ConstBuffer_VS_Base->SysmemBuffer ;

	// 定数データに反映
	ConstantVSBase->ProjectionMatrix[ 0 ][ 0 ] = Matrix->m[ 0 ][ 0 ] ;
	ConstantVSBase->ProjectionMatrix[ 0 ][ 1 ] = Matrix->m[ 1 ][ 0 ] ;
	ConstantVSBase->ProjectionMatrix[ 0 ][ 2 ] = Matrix->m[ 2 ][ 0 ] ;
	ConstantVSBase->ProjectionMatrix[ 0 ][ 3 ] = Matrix->m[ 3 ][ 0 ] ;
	ConstantVSBase->ProjectionMatrix[ 1 ][ 0 ] = Matrix->m[ 0 ][ 1 ] ;
	ConstantVSBase->ProjectionMatrix[ 1 ][ 1 ] = Matrix->m[ 1 ][ 1 ] ;
	ConstantVSBase->ProjectionMatrix[ 1 ][ 2 ] = Matrix->m[ 2 ][ 1 ] ;
	ConstantVSBase->ProjectionMatrix[ 1 ][ 3 ] = Matrix->m[ 3 ][ 1 ] ;
	ConstantVSBase->ProjectionMatrix[ 2 ][ 0 ] = Matrix->m[ 0 ][ 2 ] ;
	ConstantVSBase->ProjectionMatrix[ 2 ][ 1 ] = Matrix->m[ 1 ][ 2 ] ;
	ConstantVSBase->ProjectionMatrix[ 2 ][ 2 ] = Matrix->m[ 2 ][ 2 ] ;
	ConstantVSBase->ProjectionMatrix[ 2 ][ 3 ] = Matrix->m[ 3 ][ 2 ] ;
	ConstantVSBase->ProjectionMatrix[ 3 ][ 0 ] = Matrix->m[ 0 ][ 3 ] ;
	ConstantVSBase->ProjectionMatrix[ 3 ][ 1 ] = Matrix->m[ 1 ][ 3 ] ;
	ConstantVSBase->ProjectionMatrix[ 3 ][ 2 ] = Matrix->m[ 2 ][ 3 ] ;
	ConstantVSBase->ProjectionMatrix[ 3 ][ 3 ] = Matrix->m[ 3 ][ 3 ] ;

	GD3D11.Device.Shader.Constant.ConstBuffer_VS_Base->ChangeFlag = TRUE ;

	// 定数バッファを更新
//	Graphics_D3D11_ConstantBuffer_Update( GD3D11.Device.Shader.Constant.ConstBuffer_VS_Base ) ;

	// 終了
	return 0 ;
}

// アンチビューポート行列をセットする
extern int Graphics_D3D11_DeviceState_SetAntiViewportMatrix( const MATRIX *Matrix )
{
	DX_D3D11_VS_CONST_BUFFER_BASE *ConstantVSBase ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	ConstantVSBase = ( DX_D3D11_VS_CONST_BUFFER_BASE * )GD3D11.Device.Shader.Constant.ConstBuffer_VS_Base->SysmemBuffer ;

	// 定数データに反映
	ConstantVSBase->AntiViewportMatrix[ 0 ][ 0 ] = Matrix->m[ 0 ][ 0 ] ;
	ConstantVSBase->AntiViewportMatrix[ 0 ][ 1 ] = Matrix->m[ 1 ][ 0 ] ;
	ConstantVSBase->AntiViewportMatrix[ 0 ][ 2 ] = Matrix->m[ 2 ][ 0 ] ;
	ConstantVSBase->AntiViewportMatrix[ 0 ][ 3 ] = Matrix->m[ 3 ][ 0 ] ;
	ConstantVSBase->AntiViewportMatrix[ 1 ][ 0 ] = Matrix->m[ 0 ][ 1 ] ;
	ConstantVSBase->AntiViewportMatrix[ 1 ][ 1 ] = Matrix->m[ 1 ][ 1 ] ;
	ConstantVSBase->AntiViewportMatrix[ 1 ][ 2 ] = Matrix->m[ 2 ][ 1 ] ;
	ConstantVSBase->AntiViewportMatrix[ 1 ][ 3 ] = Matrix->m[ 3 ][ 1 ] ;
	ConstantVSBase->AntiViewportMatrix[ 2 ][ 0 ] = Matrix->m[ 0 ][ 2 ] ;
	ConstantVSBase->AntiViewportMatrix[ 2 ][ 1 ] = Matrix->m[ 1 ][ 2 ] ;
	ConstantVSBase->AntiViewportMatrix[ 2 ][ 2 ] = Matrix->m[ 2 ][ 2 ] ;
	ConstantVSBase->AntiViewportMatrix[ 2 ][ 3 ] = Matrix->m[ 3 ][ 2 ] ;
	ConstantVSBase->AntiViewportMatrix[ 3 ][ 0 ] = Matrix->m[ 0 ][ 3 ] ;
	ConstantVSBase->AntiViewportMatrix[ 3 ][ 1 ] = Matrix->m[ 1 ][ 3 ] ;
	ConstantVSBase->AntiViewportMatrix[ 3 ][ 2 ] = Matrix->m[ 2 ][ 3 ] ;
	ConstantVSBase->AntiViewportMatrix[ 3 ][ 3 ] = Matrix->m[ 3 ][ 3 ] ;

	GD3D11.Device.Shader.Constant.ConstBuffer_VS_Base->ChangeFlag = TRUE ;

	// 定数バッファを更新
//	Graphics_D3D11_ConstantBuffer_Update( GD3D11.Device.Shader.Constant.ConstBuffer_VS_Base ) ;

	// 終了
	return 0 ;
}

// アンビエントライトとエミッシブカラーを掛け合わせたパラメータを更新する
extern void Graphics_D3D11_DeviceState_RefreshAmbientAndEmissiveParam( void )
{
	DX_D3D11_CONST_BUFFER_COMMON  *ConstantCommon ;
	DX_D3D11_CONST_LIGHT          *ConstantLight ;
	int                           i ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return ;
	}

	ConstantCommon = ( DX_D3D11_CONST_BUFFER_COMMON  * )GD3D11.Device.Shader.Constant.ConstBuffer_Common->SysmemBuffer ;

	// 有効なライトのアンビエントカラーとマテリアルのアンビエントカラーを乗算したものをシェーダーにセットする
	ConstantLight = ConstantCommon->Light ;
	for( i = 0 ; i <= GD3D11.Device.State.LightEnableMaxIndex ; i ++, ConstantLight ++ )
	{
		if( GD3D11.Device.State.LightEnableFlag[ i ] == FALSE )
		{
			continue ;
		}

		// 定数バッファに反映する
		ConstantLight->Ambient[ 0 ] = GD3D11.Device.State.LightParam[ i ].Ambient.r * GD3D11.Device.State.Material.Ambient.r ;
		ConstantLight->Ambient[ 1 ] = GD3D11.Device.State.LightParam[ i ].Ambient.g * GD3D11.Device.State.Material.Ambient.g ;
		ConstantLight->Ambient[ 2 ] = GD3D11.Device.State.LightParam[ i ].Ambient.b * GD3D11.Device.State.Material.Ambient.b ;
		ConstantLight->Ambient[ 3 ] = GD3D11.Device.State.LightParam[ i ].Ambient.a * GD3D11.Device.State.Material.Ambient.a ;
	}

	// グローバルアンビエントライトカラーとマテリアルアンビエントカラーを乗算したものとエミッシブカラーを加算したものを作成する
	ConstantCommon->Material.Ambient_Emissive[ 0 ] = GD3D11.Device.State.GlobalAmbientLightColor.r * GD3D11.Device.State.Material.Ambient.r + GD3D11.Device.State.Material.Emissive.r ;
	ConstantCommon->Material.Ambient_Emissive[ 1 ] = GD3D11.Device.State.GlobalAmbientLightColor.g * GD3D11.Device.State.Material.Ambient.g + GD3D11.Device.State.Material.Emissive.g ;
	ConstantCommon->Material.Ambient_Emissive[ 2 ] = GD3D11.Device.State.GlobalAmbientLightColor.b * GD3D11.Device.State.Material.Ambient.b + GD3D11.Device.State.Material.Emissive.b ;
	ConstantCommon->Material.Ambient_Emissive[ 3 ] = GD3D11.Device.State.GlobalAmbientLightColor.a * GD3D11.Device.State.Material.Ambient.a + GD3D11.Device.State.Material.Emissive.a ;

	GD3D11.Device.Shader.Constant.ConstBuffer_Common->ChangeFlag = TRUE ;		
}



// ライトのセット
extern int Graphics_D3D11_DeviceState_SetLightState( int LightNumber, LIGHTPARAM *Light )
{
	DX_D3D11_CONST_BUFFER_COMMON  *ConstantCommon ;
	DX_D3D11_CONST_LIGHT          *ConstantLight ;
	VECTOR                        Direction ;
	VECTOR                        Position ;
	float                         ThetaSin ;
	float                         ThetaCos ;
	float                         PhiSin ;
	float                         PhiCos ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	if( LightNumber < 0 || LightNumber >= DX_D3D11_COMMON_CONST_LIGHT_NUM )
	{
		return -1 ;
	}

	ConstantCommon = ( DX_D3D11_CONST_BUFFER_COMMON  * )GD3D11.Device.Shader.Constant.ConstBuffer_Common->SysmemBuffer ;

	if( _MEMCMP( &GD3D11.Device.State.LightParam[ LightNumber ], Light, sizeof( LIGHTPARAM ) ) == 0 &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	DRAWSTOCKINFO

	// パラメータの保存
	GD3D11.Device.State.LightParam[ LightNumber ] = *Light ;

	// 定数データに反映
	ConstantLight = &ConstantCommon->Light[ LightNumber ] ;

	_SINCOS( Light->Theta / 2.0f, &ThetaSin, &ThetaCos ) ;
	_SINCOS( Light->Phi   / 2.0f, &PhiSin,   &PhiCos ) ;

	ConstantLight->Type           = Light->LightType ;
	VectorTransformSR( &Direction, &Light->Direction, &GSYS.DrawSetting.ViewMatrixF ) ;
	VectorTransform(   &Position,  &Light->Position,  &GSYS.DrawSetting.ViewMatrixF ) ;
	ConstantLight->Position[ 0 ]  = Position.x ;
	ConstantLight->Position[ 1 ]  = Position.y ;
	ConstantLight->Position[ 2 ]  = Position.z ;
	ConstantLight->RangePow2      = Light->Range * Light->Range ;
	ConstantLight->Direction[ 0 ] = Direction.x ;
	ConstantLight->Direction[ 1 ] = Direction.y ;
	ConstantLight->Direction[ 2 ] = Direction.z ;
	ConstantLight->FallOff        = Light->Falloff ;
	ConstantLight->Diffuse[ 0 ]   = Light->Diffuse.r ;
	ConstantLight->Diffuse[ 1 ]   = Light->Diffuse.g ;
	ConstantLight->Diffuse[ 2 ]   = Light->Diffuse.b ;
	ConstantLight->SpotParam0     = PhiCos ;
	ConstantLight->Specular[ 0 ]  = Light->Specular.r ;
	ConstantLight->Specular[ 1 ]  = Light->Specular.g ;
	ConstantLight->Specular[ 2 ]  = Light->Specular.b ;
	if( _FABS( ThetaCos - PhiCos ) < 0.0000001f )
	{
		ConstantLight->SpotParam1     = 100000.0f ;
	}
	else
	{
		ConstantLight->SpotParam1     = 1.0f / ( ThetaCos - PhiCos ) ;
	}
	ConstantLight->Ambient[ 0 ]   = Light->Ambient.r ;
	ConstantLight->Ambient[ 1 ]   = Light->Ambient.g ;
	ConstantLight->Ambient[ 2 ]   = Light->Ambient.b ;
	ConstantLight->Ambient[ 3 ]   = Light->Ambient.a ;
	ConstantLight->Attenuation0   = Light->Attenuation0 ;
	ConstantLight->Attenuation1   = Light->Attenuation1 ;
	ConstantLight->Attenuation2   = Light->Attenuation2 ;
	GD3D11.Device.Shader.Constant.ConstBuffer_Common->ChangeFlag = TRUE ;

	// アンビエントカラーとエミッシブカラーを掛け合わせた情報を更新する
	Graphics_D3D11_DeviceState_RefreshAmbientAndEmissiveParam() ;

	// 定数バッファを更新
//	Graphics_D3D11_ConstantBuffer_Update( GD3D11.Device.Shader.Constant.ConstBuffer_Common ) ;

	// 終了
	return 0 ;
}

// ライトの有効、無効を変更
extern int Graphics_D3D11_DeviceState_SetLightEnable( int LightNumber, int EnableState )
{
	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	if( LightNumber < 0 || LightNumber >= DX_D3D11_COMMON_CONST_LIGHT_NUM )
	{
		return -1 ;
	}

	if( GD3D11.Device.State.LightEnableFlag[ LightNumber ] == EnableState &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	DRAWSTOCKINFO

	GD3D11.Device.State.LightEnableFlag[ LightNumber ] = EnableState ;

	// 有効なライトの最大インデックスを更新
	if( EnableState == TRUE )
	{
		if( GD3D11.Device.State.LightEnableMaxIndex < LightNumber )
			GD3D11.Device.State.LightEnableMaxIndex = LightNumber ;
	}
	else
	{
		if( GD3D11.Device.State.LightEnableMaxIndex == LightNumber )
		{
			int i ;

			for( i = LightNumber ; i > 0 && GD3D11.Device.State.LightEnableFlag[ i ] == FALSE ; i -- ){}
			GD3D11.Device.State.LightEnableMaxIndex = i ;
		}
	}

	// アンビエントカラーとエミッシブカラーを掛け合わせた情報を更新する
	Graphics_D3D11_DeviceState_RefreshAmbientAndEmissiveParam() ;

	// 定数バッファを更新
//	Graphics_D3D11_ConstantBuffer_Update( GD3D11.Device.Shader.Constant.ConstBuffer_Common ) ;

	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// 終了
	return 0 ;
}

// マテリアルのセット
extern int Graphics_D3D11_DeviceState_SetMaterial( MATERIALPARAM *Material )
{
	DX_D3D11_CONST_BUFFER_COMMON *ConstantCommon ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	if( _MEMCMP( GSYS.Light.MaterialTypeParam, GD3D11.Device.State.MaterialTypeParam, sizeof( GSYS.Light.MaterialTypeParam) ) == 0 &&
		_MEMCMP( Material, &GD3D11.Device.State.Material, sizeof( MATERIALPARAM ) ) == 0 &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	ConstantCommon = ( DX_D3D11_CONST_BUFFER_COMMON * )GD3D11.Device.Shader.Constant.ConstBuffer_Common->SysmemBuffer ;

	DRAWSTOCKINFO

	GD3D11.Device.State.Material = *Material ;
	_MEMCPY( GD3D11.Device.State.MaterialTypeParam, GSYS.Light.MaterialTypeParam, sizeof( GSYS.Light.MaterialTypeParam ) ) ;

	// 定数データに反映
	ConstantCommon->Material.Diffuse[ 0 ]  = Material->Diffuse.r ;
	ConstantCommon->Material.Diffuse[ 1 ]  = Material->Diffuse.g ;
	ConstantCommon->Material.Diffuse[ 2 ]  = Material->Diffuse.b ;
	ConstantCommon->Material.Diffuse[ 3 ]  = Material->Diffuse.a ;
	ConstantCommon->Material.Specular[ 0 ] = Material->Specular.r ;
	ConstantCommon->Material.Specular[ 1 ] = Material->Specular.g ;
	ConstantCommon->Material.Specular[ 2 ] = Material->Specular.b ;
	ConstantCommon->Material.Specular[ 3 ] = Material->Specular.a ;
	ConstantCommon->Material.Power         = Material->Power < 0.00000000001f ? 0.00000000001f : Material->Power ;
	ConstantCommon->Material.TypeParam0    = GD3D11.Device.State.MaterialTypeParam[ 0 ] ;
	ConstantCommon->Material.TypeParam1    = GD3D11.Device.State.MaterialTypeParam[ 1 ] ;
	ConstantCommon->Material.TypeParam2    = GD3D11.Device.State.MaterialTypeParam[ 2 ] ;
	GD3D11.Device.Shader.Constant.ConstBuffer_Common->ChangeFlag = TRUE ;

	// アンビエントカラーとエミッシブカラーを掛け合わせた情報を更新する
	Graphics_D3D11_DeviceState_RefreshAmbientAndEmissiveParam() ;

	// 定数バッファを更新
//	Graphics_D3D11_ConstantBuffer_Update( GD3D11.Device.Shader.Constant.ConstBuffer_Common ) ;

	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// 終了
	return 0 ;
}

// スペキュラを使用するかどうかをセット
extern int Graphics_D3D11_DeviceState_SetSpecularEnable( int UseFlag )
{
	DX_D3D11_VS_CONST_BUFFER_BASE *ConstantVSBase ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// 既存のパラメータと同じ場合は何もしない
	if( GD3D11.Device.State.UseSpecular == UseFlag &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	ConstantVSBase = ( DX_D3D11_VS_CONST_BUFFER_BASE * )GD3D11.Device.Shader.Constant.ConstBuffer_VS_Base->SysmemBuffer ;

	DRAWSTOCKINFO

	GD3D11.Device.State.UseSpecular = UseFlag ;

	// 定数データに反映
	ConstantVSBase->MulSpecularColor = UseFlag ? 1.0f : 0.0f ;
	GD3D11.Device.Shader.Constant.ConstBuffer_VS_Base->ChangeFlag = TRUE ;

	// 定数バッファを更新
//	Graphics_D3D11_ConstantBuffer_Update( GD3D11.Device.Shader.Constant.ConstBuffer_VS_Base ) ;

	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// 終了
	return 0 ;
}

// 頂点のディフューズカラーをマテリアルのディフューズカラーとして使用するかどうかを設定する
extern int Graphics_D3D11_DeviceState_SetUseVertexDiffuseColor( int UseFlag )
{
	DX_D3D11_VS_CONST_BUFFER_BASE *ConstantVSBase ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	if( UseFlag == GD3D11.Device.State.MaterialUseVertexDiffuseColor &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	ConstantVSBase = ( DX_D3D11_VS_CONST_BUFFER_BASE * )GD3D11.Device.Shader.Constant.ConstBuffer_VS_Base->SysmemBuffer ;

	DRAWSTOCKINFO

	// 定数データに反映
	ConstantVSBase->DiffuseSource = UseFlag ? 1.0f : 0.0f ;
	GD3D11.Device.Shader.Constant.ConstBuffer_VS_Base->ChangeFlag = TRUE ;

	// 定数バッファを更新
//	Graphics_D3D11_ConstantBuffer_Update( GD3D11.Device.Shader.Constant.ConstBuffer_VS_Base ) ;

	GD3D11.Device.State.MaterialUseVertexDiffuseColor = UseFlag ;

	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// 終了
	return 0 ;
}

// 頂点のスペキュラカラーをマテリアルのスペキュラカラーとして使用するかどうかを設定する
extern int Graphics_D3D11_DeviceState_SetUseVertexSpecularColor( int UseFlag )
{
	DX_D3D11_VS_CONST_BUFFER_BASE *ConstantVSBase ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	if( UseFlag == GD3D11.Device.State.MaterialUseVertexSpecularColor &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	ConstantVSBase = ( DX_D3D11_VS_CONST_BUFFER_BASE * )GD3D11.Device.Shader.Constant.ConstBuffer_VS_Base->SysmemBuffer ;

	DRAWSTOCKINFO

	// 定数データに反映
	ConstantVSBase->SpecularSource = UseFlag ? 1.0f : 0.0f ;
	GD3D11.Device.Shader.Constant.ConstBuffer_VS_Base->ChangeFlag = TRUE ;

	// 定数バッファを更新
//	Graphics_D3D11_ConstantBuffer_Update( GD3D11.Device.Shader.Constant.ConstBuffer_VS_Base ) ;

	GD3D11.Device.State.MaterialUseVertexSpecularColor = UseFlag ;

	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// 終了
	return 0 ;
}

// カリングの有無をセット 
extern int Graphics_D3D11_DeviceState_SetCullMode( int State )
{
	D_D3D11_CULL_MODE D3DCullMode ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	switch( State )
	{
	case 0 :
	default :
		D3DCullMode = D_D3D11_CULL_NONE ;
		break ;

	case 1 :
		D3DCullMode = D_D3D11_CULL_BACK ;
		break ;

	case 2 :
		D3DCullMode = D_D3D11_CULL_FRONT ;
		break ;
	}

	// モードが同じだったら何もせず終了
	if( GD3D11.Device.State.RasterizerDesc.CullMode == D3DCullMode &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}
	
	DRAWSTOCKINFO

	GD3D11.Device.State.ChangeRasterizerDesc = TRUE ;

	GD3D11.Device.State.RasterizerDesc.CullMode = D3DCullMode ;

	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

//	return Graphics_D3D11_DeviceState_SetupRasterizerState() ;
	return 0 ;
}

// アンビエントカラーのセット
extern int Graphics_D3D11_DeviceState_SetAmbient( COLOR_F *Color )
{
	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	if( _MEMCMP( Color, &GD3D11.Device.State.GlobalAmbientLightColor, sizeof( COLOR_F ) ) == 0 &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	DRAWSTOCKINFO

	GD3D11.Device.State.GlobalAmbientLightColor = *Color ;

	// アンビエントカラーとエミッシブカラーを掛け合わせた情報を更新する
	Graphics_D3D11_DeviceState_RefreshAmbientAndEmissiveParam() ;

	// 定数バッファを更新
//	Graphics_D3D11_ConstantBuffer_Update( GD3D11.Device.Shader.Constant.ConstBuffer_Common ) ;

	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// 終了
	return 0 ;
}

// 三つのフィルタータイプから D_D3D11_FILTER を取得する
static D_D3D11_FILTER Graphics_D3D11_DeviceState_GetFilter( int MinFilter /* DX_DRAWMODE_NEAREST 等 */, int MagFilter, int MipFilter )
{
	if( MagFilter == DX_DRAWMODE_ANISOTROPIC ||
		MinFilter == DX_DRAWMODE_ANISOTROPIC ||
		MipFilter == DX_DRAWMODE_ANISOTROPIC )
	{
		return D_D3D11_FILTER_ANISOTROPIC ;
	}
	else
	{
		// 0:Point  1:Linear
		static D_D3D11_FILTER FilterTable[ 2 /* Min */ ][ 2 /* Mag */ ][ 2 /* Mip */ ] =
		{
			{
				{
					D_D3D11_FILTER_MIN_MAG_MIP_POINT,
					D_D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR
				},

				{
					D_D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT,
					D_D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR,
				},
			},

			{
				{
					D_D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT,
					D_D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR
				},

				{
					D_D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT,
					D_D3D11_FILTER_MIN_MAG_MIP_LINEAR,
				},
			},
		} ;

		return FilterTable[ MinFilter != DX_DRAWMODE_NEAREST ? 1 : 0 ][ MagFilter != DX_DRAWMODE_NEAREST ? 1 : 0 ][ MipFilter != DX_DRAWMODE_NEAREST ? 1 : 0 ] ;
	}
}

// 現在の設定に基づいて ID3D11SamplerState をセットアップする
static int Graphics_D3D11_DeviceState_SetupSamplerState( void )
{
	int                     i ;
	int                     SetLength ;
	D_D3D11_SAMPLER_DESC	*SamplerDesc ;
	D_ID3D11SamplerState	*NewSamplerState ;
	D_ID3D11SamplerState	**DefaultSamplerState ;
	HRESULT                 hr ;
	static int				DefaultFilterIndexTableInitialize = FALSE ;
	static int				DefaultFilterIndexTable[ 0xff ] ;

	// D_D3D11_FILTER からインデックスを取得するためのテーブルが初期化されていない場合は初期化する
	if( DefaultFilterIndexTableInitialize == FALSE )
	{
		DefaultFilterIndexTableInitialize = TRUE ;

		DefaultFilterIndexTable[ D_D3D11_FILTER_MIN_MAG_MIP_POINT                          ] = 0 ;
		DefaultFilterIndexTable[ D_D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR                   ] = 1 ;
		DefaultFilterIndexTable[ D_D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT             ] = 2 ;
		DefaultFilterIndexTable[ D_D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR                   ] = 3 ;
		DefaultFilterIndexTable[ D_D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT                   ] = 4 ;
		DefaultFilterIndexTable[ D_D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR            ] = 5 ;
		DefaultFilterIndexTable[ D_D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT                   ] = 6 ;
		DefaultFilterIndexTable[ D_D3D11_FILTER_MIN_MAG_MIP_LINEAR                         ] = 7 ;
		DefaultFilterIndexTable[ D_D3D11_FILTER_ANISOTROPIC                                ] = 8 ;
		DefaultFilterIndexTable[ D_D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT               ] = 9 ;
		DefaultFilterIndexTable[ D_D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR        ] = 10 ;
		DefaultFilterIndexTable[ D_D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT  ] = 11 ;
		DefaultFilterIndexTable[ D_D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR        ] = 12 ;
		DefaultFilterIndexTable[ D_D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT        ] = 13 ;
		DefaultFilterIndexTable[ D_D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR ] = 14 ;
		DefaultFilterIndexTable[ D_D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT        ] = 15 ;
		DefaultFilterIndexTable[ D_D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR              ] = 16 ;
		DefaultFilterIndexTable[ D_D3D11_FILTER_COMPARISON_ANISOTROPIC                     ] = 17 ;
	}

	// 設定に変更が無い場合は何もせずに終了
	if( GD3D11.Device.State.ChangeSamplerDesc == FALSE &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	// 変更があったかどうかのフラグを倒す
	GD3D11.Device.State.ChangeSamplerDesc = FALSE ;

	// サンプラースロットの数だけ繰り返し
	SamplerDesc = GD3D11.Device.State.SamplerDesc ;
	SetLength     = 0 ;
	for( i = 0 ; i < USE_TEXTURESTAGE_NUM ; i ++, SamplerDesc ++ )
	{
		// 設定値に変更が無く、必ず再設定を行うフラグも倒れていたら次のループへ
		if( GD3D11.Device.State.ChangeSamplerDescSlot[ i ] == FALSE &&
			GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
		{
			// 変更する設定の数が０ではない場合はサンプラーステートをデバイスコンテキストに設定する
			if( SetLength > 0 )
			{
				D3D11DeviceContext_PSSetSamplers( ( UINT )( i - SetLength ), ( UINT )SetLength, &GD3D11.Device.State.SamplerState[ i - SetLength ] ) ;
				SetLength = 0 ;
			}
			continue ;
		}

		// 変更があったかどうかのフラグを倒す
		GD3D11.Device.State.ChangeSamplerDescSlot[ i ] = FALSE ;

		// 現在設定されているサンプラーがある場合は解除する
		if( GD3D11.Device.State.SamplerState[ i ] != NULL )
		{
			// 現在の設定を変更する際にサンプラーステートをリリースする指定になっていたらリリースする
			if( GD3D11.Device.State.ChangeSamplerStateReleaseFlag[ i ] )
			{
				Direct3D11_Release_SamplerState( GD3D11.Device.State.SamplerState[ i ] ) ;
			}

			GD3D11.Device.State.SamplerState[ i ] = NULL ;
		}

		// 設定を変更する際にサンプラーステートをリリースするかどうかのフラグを倒す
		GD3D11.Device.State.ChangeSamplerStateReleaseFlag[ i ] = FALSE ;

		// よく使う設定のサンプラーステートかどうかを判定する
		if(	SamplerDesc->MipLODBias       != 0.0f ||
			( SamplerDesc->MaxAnisotropy  != 16 && SamplerDesc->MaxAnisotropy != 2 && SamplerDesc->MaxAnisotropy != 1 ) ||
			SamplerDesc->ComparisonFunc   != D_D3D11_COMPARISON_ALWAYS ||
			SamplerDesc->BorderColor[ 0 ] != 0.0f ||
			SamplerDesc->BorderColor[ 1 ] != 0.0f ||
			SamplerDesc->BorderColor[ 2 ] != 0.0f ||
			SamplerDesc->BorderColor[ 3 ] != 0.0f ||
			SamplerDesc->MinLOD           != -D_D3D11_FLOAT32_MAX ||
			SamplerDesc->MaxLOD           !=  D_D3D11_FLOAT32_MAX )
		{
			// よく使う設定のサンプラーステートではない場合はここでサンプラーステートを作成する
			hr = D3D11Device_CreateSamplerState( SamplerDesc, &NewSamplerState ) ;
			if( hr != S_OK )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x49\x00\x44\x00\x33\x00\x44\x00\x31\x00\x31\x00\x53\x00\x61\x00\x6d\x00\x70\x00\x6c\x00\x65\x00\x72\x00\x53\x00\x74\x00\x61\x00\x74\x00\x65\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00\x30\x87\x7b\x40\x62\x11\xff\x0a\x00\x00"/*@ L"ID3D11SamplerState の作成に失敗しました　箇所１\n" @*/ )) ;
				return -1 ;
			}

			// 作成したサンプラーステートを設定
			GD3D11.Device.State.SamplerState[ i ] = NewSamplerState ;

			// このサンプラーステートは一時的なものなので、終了時にリリースするようにする
			GD3D11.Device.State.ChangeSamplerStateReleaseFlag[ i ] = TRUE ;
		}
		else
		{
			// よく使う設定のサンプラーステートの場合

			// 指定のよく使う設定のサンプラーステートがまだ作成されていなかったら作成する
			DefaultSamplerState = &GD3D11.Device.State.DefaultSamplerStateArray[ DefaultFilterIndexTable[ SamplerDesc->Filter ] ][ SamplerDesc->AddressU ][ SamplerDesc->AddressV ][ SamplerDesc->AddressW ][ SamplerDesc->MaxAnisotropy == 1 ? 0 : ( SamplerDesc->MaxAnisotropy == 2 ? 1 : 2 ) ] ;
			if( *DefaultSamplerState == NULL )
			{
				hr = D3D11Device_CreateSamplerState( SamplerDesc, &NewSamplerState ) ;
				if( hr != S_OK )
				{
					DXST_LOGFILEFMT_ADDUTF16LE(( "\x49\x00\x44\x00\x33\x00\x44\x00\x31\x00\x31\x00\x53\x00\x61\x00\x6d\x00\x70\x00\x6c\x00\x65\x00\x72\x00\x53\x00\x74\x00\x61\x00\x74\x00\x65\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00\x30\x87\x7b\x40\x62\x12\xff\x0a\x00\x00"/*@ L"ID3D11SamplerState の作成に失敗しました　箇所２\n" @*/ )) ;
					return -1 ;
				}
				*DefaultSamplerState = NewSamplerState ;
			}

			// よく使う設定のサンプラーステートを設定
			GD3D11.Device.State.SamplerState[ i ] = *DefaultSamplerState ;
		}

		// 変更する設定の数をインクリメント
		SetLength ++ ;
	}

	// 変更する設定の数が０ではない場合はサンプラーステートをデバイスコンテキストに設定する
	if( SetLength > 0 )
	{
		D3D11DeviceContext_PSSetSamplers( ( UINT )( i - SetLength ), ( UINT )SetLength, &GD3D11.Device.State.SamplerState[ i - SetLength ] ) ;
		SetLength = 0 ;
	}

	// 正常終了
	return 0 ;
}

// ID3D11SamplerState を解放する
static int Graphics_D3D11_DeviceState_TerminateSamplerState( void )
{
	int i ;

	GD3D11.Device.State.ChangeSamplerDesc = FALSE ;

	_MEMSET( GD3D11.Device.State.ChangeSamplerDescSlot, 0, sizeof( GD3D11.Device.State.ChangeSamplerDescSlot ) ) ;
//	_MEMSET( GD3D11.Device.State.SamplerDesc, 0, sizeof( GD3D11.Device.State.SamplerDesc ) ) ;

	for( i = 0 ; i < USE_TEXTURESTAGE_NUM ; i ++ )
	{
		if( GD3D11.Device.State.SamplerState[ i ] == NULL )
		{
			continue ;
		}

		if( GD3D11.Device.State.ChangeSamplerStateReleaseFlag[ i ] )
		{
			Direct3D11_Release_SamplerState( GD3D11.Device.State.SamplerState[ i ] ) ;
			GD3D11.Device.State.ChangeSamplerStateReleaseFlag[ i ] = FALSE ;
		}
		GD3D11.Device.State.SamplerState[ i ] = NULL ;
	}

	Graphics_D3D11_SamplerStateArray_Release( ( D_ID3D11SamplerState ** )GD3D11.Device.State.DefaultSamplerStateArray, sizeof( GD3D11.Device.State.DefaultSamplerStateArray ) / sizeof( D_ID3D11SamplerState * ) ) ;

	// 正常終了
	return 0 ;
}

// 現在の設定に基づいて ID3D11RasterizerState をセットアップする
static int Graphics_D3D11_DeviceState_SetupRasterizerState( void )
{
	D_D3D11_RASTERIZER_DESC *RasterizerDesc ;
	D_ID3D11RasterizerState *NewRasterizerState ;
	D_ID3D11RasterizerState **DefaultRasterizerState ;
	HRESULT                  hr ;

	// 設定に変更が無く、必ず再設定を行うフラグも倒れていたら何もせず終了
	if( GD3D11.Device.State.ChangeRasterizerDesc == FALSE &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	// 設定に変更があったかどうかのフラグを倒す
	GD3D11.Device.State.ChangeRasterizerDesc = FALSE ;

	// 現在設定されているラスタライザーステートがある場合は解除する
	if( GD3D11.Device.State.RasterizerState != NULL )
	{
		// 現在の設定を変更する際にラスタライザーステートをリリースする指定になっていたらリリースする
		if( GD3D11.Device.State.ChangeRasterizerStateReleaseFlag )
		{
			Direct3D11_Release_RasterizerState( GD3D11.Device.State.RasterizerState ) ;
		}

		GD3D11.Device.State.RasterizerState = NULL ;
	}

	// 設定を変更する際にラスタライザーステートをリリースするかどうかのフラグを倒す
	GD3D11.Device.State.ChangeRasterizerStateReleaseFlag = FALSE ;

	// よく使う設定のラスタライザーステートかどうかを判定する
	RasterizerDesc = &GD3D11.Device.State.RasterizerDesc ;
	if( RasterizerDesc->FrontCounterClockwise != FALSE ||
		RasterizerDesc->DepthBias             != 0 ||
		RasterizerDesc->DepthBiasClamp        != 0.0f ||
		RasterizerDesc->SlopeScaledDepthBias  != 0.0f ||
		RasterizerDesc->ScissorEnable         != FALSE ||
		RasterizerDesc->AntialiasedLineEnable != FALSE )
	{
		// よく使う設定のラスタライザーステートではない場合はここでラスタライザーステートを作成する
		hr = D3D11Device_CreateRasterizerState( RasterizerDesc, &NewRasterizerState ) ;
		if( hr != S_OK )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x49\x00\x44\x00\x33\x00\x44\x00\x31\x00\x31\x00\x52\x00\x61\x00\x73\x00\x74\x00\x65\x00\x72\x00\x69\x00\x7a\x00\x65\x00\x72\x00\x53\x00\x74\x00\x61\x00\x74\x00\x65\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00\x30\x87\x7b\x40\x62\x11\xff\x0a\x00\x00"/*@ L"ID3D11RasterizerState の作成に失敗しました　箇所１\n" @*/ )) ;
			return -1 ;
		}

		// 作成したラスタライザーステートを設定
		GD3D11.Device.State.RasterizerState = NewRasterizerState ;

		// このラスタライザーステートは一時的なものなので、終了時にリリースするようにする
		GD3D11.Device.State.ChangeRasterizerStateReleaseFlag = TRUE ;
	}
	else
	{
		// よく使う設定のラスタライザーステートの場合

		// 指定のよく使う設定のラスタライザーステートがまだ作成されていなかったら作成する
		DefaultRasterizerState = &GD3D11.Device.State.DefaultRasterizerStateArray[ RasterizerDesc->CullMode ][ RasterizerDesc->FillMode ][ RasterizerDesc->DepthClipEnable ][ RasterizerDesc->MultisampleEnable ] ;
		if( *DefaultRasterizerState == NULL )
		{
			hr = D3D11Device_CreateRasterizerState( RasterizerDesc, &NewRasterizerState ) ;
			if( hr != S_OK )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x49\x00\x44\x00\x33\x00\x44\x00\x31\x00\x31\x00\x52\x00\x61\x00\x73\x00\x74\x00\x65\x00\x72\x00\x69\x00\x7a\x00\x65\x00\x72\x00\x53\x00\x74\x00\x61\x00\x74\x00\x65\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00\x30\x87\x7b\x40\x62\x12\xff\x0a\x00\x00"/*@ L"ID3D11RasterizerState の作成に失敗しました　箇所２\n" @*/ )) ;
				return -1 ;
			}
			*DefaultRasterizerState = NewRasterizerState ;
		}

		// よく使う設定のラスタライザーステートを設定
		GD3D11.Device.State.RasterizerState = *DefaultRasterizerState ;
	}

	// ラスタライザーステートをデバイスコンテキストに設定する
	D3D11DeviceContext_RSSetState( GD3D11.Device.State.RasterizerState ) ;

	// 正常終了
	return 0 ;
}

// ID3D11RasterizerState を解放する
static int Graphics_D3D11_DeviceState_TerminateRasterizerState( void )
{
	GD3D11.Device.State.ChangeRasterizerDesc = FALSE ;

//	_MEMSET( &GD3D11.Device.State.RasterizerDesc, 0, sizeof( GD3D11.Device.State.RasterizerDesc ) ) ;

	if( GD3D11.Device.State.ChangeRasterizerStateReleaseFlag )
	{
		Direct3D11_Release_RasterizerState( GD3D11.Device.State.RasterizerState ) ;
		GD3D11.Device.State.ChangeRasterizerStateReleaseFlag = FALSE ;
	}
	GD3D11.Device.State.RasterizerState = NULL ;

	Graphics_D3D11_RasterizerStateArray_Release( ( D_ID3D11RasterizerState ** )GD3D11.Device.State.DefaultRasterizerStateArray, sizeof( GD3D11.Device.State.DefaultRasterizerStateArray ) / sizeof( D_ID3D11RasterizerState * ) ) ;

	// 正常終了
	return 0 ;
}

// 現在の設定に基づいて ID3D11DepthStencilState をセットアップする
static int Graphics_D3D11_DeviceState_SetupDepthStencilState( void )
{
	D_D3D11_DEPTH_STENCIL_DESC *DepthStencilDesc ;
	D_ID3D11DepthStencilState  *NewDepthStencilState ;
	D_ID3D11DepthStencilState  **DefaultDepthStencilState ;
	HRESULT                     hr ;

	// 設定に変更が無く、必ず再設定を行うフラグも倒れていたら何もせず終了
	if( GD3D11.Device.State.ChangeDepthStencilDesc == FALSE &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	// 設定に変更があったかどうかのフラグを倒す
	GD3D11.Device.State.ChangeDepthStencilDesc = FALSE ;

	// 現在設定されている深度ステンシルステートがある場合は解除する
	if( GD3D11.Device.State.DepthStencilState != NULL )
	{
		// 現在の設定を変更する際に深度ステンシルステートをリリースする指定になっていたらリリースする
		if( GD3D11.Device.State.ChangeDepthStencilStateReleaseFlag )
		{
			Direct3D11_Release_DepthStencilState( GD3D11.Device.State.DepthStencilState ) ;
		}

		GD3D11.Device.State.DepthStencilState = NULL ;
	}

	// 設定を変更する際に深度ステンシルステートをリリースするかどうかのフラグを倒す
	GD3D11.Device.State.ChangeDepthStencilStateReleaseFlag = FALSE ;

	// よく使う設定の深度ステンシルステートかどうかを判定する
	DepthStencilDesc = &GD3D11.Device.State.DepthStencilDesc ;
	if( DepthStencilDesc->StencilEnable					!= FALSE ||
		DepthStencilDesc->StencilReadMask				!= D_D3D11_DEFAULT_STENCIL_READ_MASK ||
		DepthStencilDesc->StencilWriteMask				!= D_D3D11_DEFAULT_STENCIL_WRITE_MASK ||
		DepthStencilDesc->FrontFace.StencilFailOp		!= D_D3D11_STENCIL_OP_KEEP ||
		DepthStencilDesc->FrontFace.StencilDepthFailOp	!= D_D3D11_STENCIL_OP_KEEP ||
		DepthStencilDesc->FrontFace.StencilPassOp		!= D_D3D11_STENCIL_OP_KEEP ||
		DepthStencilDesc->FrontFace.StencilFunc			!= D_D3D11_COMPARISON_ALWAYS ||
		DepthStencilDesc->BackFace.StencilFailOp		!= D_D3D11_STENCIL_OP_KEEP ||
		DepthStencilDesc->BackFace.StencilDepthFailOp	!= D_D3D11_STENCIL_OP_KEEP ||
		DepthStencilDesc->BackFace.StencilPassOp		!= D_D3D11_STENCIL_OP_KEEP ||
		DepthStencilDesc->BackFace.StencilFunc			!= D_D3D11_COMPARISON_ALWAYS )
	{
		// よく使う設定の深度ステンシルステートではない場合はここで深度ステンシルステートを作成する
		hr = D3D11Device_CreateDepthStencilState( DepthStencilDesc, &NewDepthStencilState ) ;
		if( hr != S_OK )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x49\x00\x44\x00\x33\x00\x44\x00\x31\x00\x31\x00\x44\x00\x65\x00\x70\x00\x74\x00\x68\x00\x53\x00\x74\x00\x65\x00\x6e\x00\x63\x00\x69\x00\x6c\x00\x53\x00\x74\x00\x61\x00\x74\x00\x65\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00\x30\x87\x7b\x40\x62\x11\xff\x0a\x00\x00"/*@ L"ID3D11DepthStencilState の作成に失敗しました　箇所１\n" @*/ )) ;
			return -1 ;
		}

		// 作成した深度ステンシルステートを設定
		GD3D11.Device.State.DepthStencilState = NewDepthStencilState ;

		// この深度ステンシルステートは一時的なものなので、終了時にリリースするようにする
		GD3D11.Device.State.ChangeDepthStencilStateReleaseFlag = TRUE ;
	}
	else
	{
		// よく使う設定の深度ステンシルステートの場合

		// 指定のよく使う設定の深度ステンシルステートがまだ作成されていなかったら作成する
		DefaultDepthStencilState = &GD3D11.Device.State.DefaultDepthStencilStateArray[ DepthStencilDesc->DepthEnable ][ DepthStencilDesc->DepthWriteMask == D_D3D11_DEPTH_WRITE_MASK_ALL ? 1 : 0 ][ DepthStencilDesc->DepthFunc ] ;
		if( *DefaultDepthStencilState == NULL )
		{
			hr = D3D11Device_CreateDepthStencilState( DepthStencilDesc, &NewDepthStencilState ) ;
			if( hr != S_OK )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x49\x00\x44\x00\x33\x00\x44\x00\x31\x00\x31\x00\x44\x00\x65\x00\x70\x00\x74\x00\x68\x00\x53\x00\x74\x00\x65\x00\x6e\x00\x63\x00\x69\x00\x6c\x00\x53\x00\x74\x00\x61\x00\x74\x00\x65\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00\x30\x87\x7b\x40\x62\x12\xff\x0a\x00\x00"/*@ L"ID3D11DepthStencilState の作成に失敗しました　箇所２\n" @*/ )) ;
				return -1 ;
			}
			*DefaultDepthStencilState = NewDepthStencilState ;
		}

		// よく使う設定の深度ステンシルステートを設定
		GD3D11.Device.State.DepthStencilState = *DefaultDepthStencilState ;
	}

	// 深度ステンシルステートをデバイスコンテキストに設定する
	D3D11DeviceContext_OMSetDepthStencilState( GD3D11.Device.State.DepthStencilState, 0 ) ;

	// 正常終了
	return 0 ;
}

// ID3D11DepthStencilState を解放する
static int Graphics_D3D11_DeviceState_TerminateDepthStencilState( void )
{
	GD3D11.Device.State.ChangeDepthStencilDesc = FALSE ;

//	_MEMSET( &GD3D11.Device.State.DepthStencilDesc, 0, sizeof( GD3D11.Device.State.DepthStencilDesc ) ) ;

	if( GD3D11.Device.State.ChangeDepthStencilStateReleaseFlag )
	{
		Direct3D11_Release_DepthStencilState( GD3D11.Device.State.DepthStencilState ) ;
		GD3D11.Device.State.ChangeDepthStencilStateReleaseFlag = FALSE ;
	}
	GD3D11.Device.State.DepthStencilState = NULL ;

	Graphics_D3D11_DepthStencilStateArray_Release( ( D_ID3D11DepthStencilState ** )GD3D11.Device.State.DefaultDepthStencilStateArray, sizeof( GD3D11.Device.State.DefaultDepthStencilStateArray ) / sizeof( D_ID3D11DepthStencilState * ) ) ;

	// 正常終了
	return 0 ;
}

// 現在の設定に基づいて ID3D11BlendState をセットアップする
static int Graphics_D3D11_DeviceState_SetupBlendState( void )
{
	D_ID3D11BlendState *NewBlendState ;
	D_ID3D11BlendState **DefaultBlendState ;
	HRESULT            hr ;

	// 設定に変更が無く、必ず再設定を行うフラグも倒れていたら何もせず終了
	if( GD3D11.Device.State.ChangeBlendDesc == FALSE &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	// 設定に変更があったかどうかのフラグを倒す
	GD3D11.Device.State.ChangeBlendDesc = FALSE ;

	// 現在設定されているブレンドステートがある場合は解除する
	if( GD3D11.Device.State.BlendState != NULL )
	{
		// 現在の設定を変更する際にブレンドステートをリリースする指定になっていたらリリースする
		if( GD3D11.Device.State.ChangeBlendStateReleaseFlag )
		{
			Direct3D11_Release_BlendState( GD3D11.Device.State.BlendState ) ;
		}

		GD3D11.Device.State.BlendState = NULL ;
	}

	// 設定を変更する際にブレンドステートをリリースするかどうかのフラグを倒す
	GD3D11.Device.State.ChangeBlendStateReleaseFlag = FALSE ;

	// よく使う設定のブレンドステートかどうかを判定する
	if( GD3D11.Device.State.BlendMode == -1 )
	{
		// よく使う設定のブレンドステートではない場合はここでブレンドステートを作成する
		hr = D3D11Device_CreateBlendState( &GD3D11.Device.State.BlendDesc, &NewBlendState ) ;
		if( hr != S_OK )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x49\x00\x44\x00\x33\x00\x44\x00\x31\x00\x31\x00\x42\x00\x6c\x00\x65\x00\x6e\x00\x64\x00\x53\x00\x74\x00\x61\x00\x74\x00\x65\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00\x30\x87\x7b\x40\x62\x11\xff\x0a\x00\x00"/*@ L"ID3D11BlendState の作成に失敗しました　箇所１\n" @*/ )) ;
			return -1 ;
		}

		// 作成したブレンドステートを設定
		GD3D11.Device.State.BlendState = NewBlendState ;

		// このブレンドステートは一時的なものなので、終了時にリリースするようにする
		GD3D11.Device.State.ChangeBlendStateReleaseFlag = TRUE ;
	}
	else
	{
		// よく使う設定のブレンドステートの場合

		// 指定のよく使う設定のブレンドステートがまだ作成されていなかったら作成する
		DefaultBlendState = &GD3D11.Device.State.DefaultBlendStateArray[ GD3D11.Device.State.BlendMode ] ;
		if( *DefaultBlendState == NULL )
		{
			D_D3D11_BLEND_DESC TempBlendDesc ;
			int i ;

			TempBlendDesc.AlphaToCoverageEnable  = FALSE ;
			TempBlendDesc.IndependentBlendEnable = FALSE ;
			for( i = 0; i < DX_RENDERTARGET_COUNT; i++ )
			{
				TempBlendDesc.RenderTarget[ i ] = g_DefaultBlendDescArray[ GD3D11.Device.State.BlendMode ].RenderTargetBlendDesc ;
			}

			hr = D3D11Device_CreateBlendState( &TempBlendDesc, &NewBlendState ) ;
			if( hr != S_OK )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x49\x00\x44\x00\x33\x00\x44\x00\x31\x00\x31\x00\x42\x00\x6c\x00\x65\x00\x6e\x00\x64\x00\x53\x00\x74\x00\x61\x00\x74\x00\x65\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00\x30\x87\x7b\x40\x62\x12\xff\x0a\x00\x00"/*@ L"ID3D11BlendState の作成に失敗しました　箇所２\n" @*/ )) ;
				return -1 ;
			}
			*DefaultBlendState = NewBlendState ;
		}

		// よく使う設定のブレンドステートを設定
		GD3D11.Device.State.BlendState = *DefaultBlendState ;
	}

	// ブレンドステートをデバイスコンテキストに設定する
	static FLOAT BlendFactor[ 4 ] = { 0.0f } ;
	D3D11DeviceContext_OMSetBlendState( GD3D11.Device.State.BlendState, BlendFactor, 0xffffffff ) ;

	// 正常終了
	return 0 ;
}

// ID3D11BlendState を解放する
static int Graphics_D3D11_DeviceState_TerminateBlendState( void )
{
	GD3D11.Device.State.ChangeBlendDesc = FALSE ;

//	_MEMSET( &GD3D11.Device.State.BlendDesc, 0, sizeof( GD3D11.Device.State.BlendDesc ) ) ;

	if( GD3D11.Device.State.ChangeBlendStateReleaseFlag )
	{
		Direct3D11_Release_BlendState( GD3D11.Device.State.BlendState ) ;
		GD3D11.Device.State.ChangeBlendStateReleaseFlag = FALSE ;
	}
	GD3D11.Device.State.BlendState = NULL ;

	Graphics_D3D11_BlendStateArray_Release( ( D_ID3D11BlendState ** )GD3D11.Device.State.DefaultBlendStateArray, sizeof( GD3D11.Device.State.DefaultBlendStateArray ) / sizeof( D_ID3D11BlendState * ) ) ;

	// 正常終了
	return 0 ;
}

// テクスチャーのアドレッシングモードをセットする
extern int  Graphics_D3D11_DeviceState_SetTextureAddress( int AddressMode, int Sampler )
{
	int								i ;
	D_D3D11_SAMPLER_DESC			*SamplerDesc ;
	D_D3D11_TEXTURE_ADDRESS_MODE	D3D11AddressMode ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	D3D11AddressMode = ( D_D3D11_TEXTURE_ADDRESS_MODE )AddressMode ;

	if( Sampler < 0 )
	{
		if( GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
		{
			SamplerDesc = GD3D11.Device.State.SamplerDesc ;
			for( i = 0 ; i < USE_TEXTURESTAGE_NUM ; i ++, SamplerDesc ++ )
			{
				if( D3D11AddressMode != SamplerDesc->AddressU ||
					D3D11AddressMode != SamplerDesc->AddressV ||
					D3D11AddressMode != SamplerDesc->AddressW )
				{
					break ;
				}
			}
			if( i == USE_TEXTURESTAGE_NUM )
			{
				return 0 ;
			}
		}

		DRAWSTOCKINFO

		SamplerDesc = GD3D11.Device.State.SamplerDesc ;
		for( i = 0 ; i < USE_TEXTURESTAGE_NUM ; i ++, SamplerDesc ++ )
		{
			if( D3D11AddressMode == SamplerDesc->AddressU &&
				D3D11AddressMode == SamplerDesc->AddressV &&
				D3D11AddressMode == SamplerDesc->AddressW &&
				GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
			{
				continue ;
			}

			GD3D11.Device.State.ChangeSamplerDescSlot[ i ] = TRUE ;
			GD3D11.Device.State.ChangeSamplerDesc = TRUE ;

			SamplerDesc->AddressU = D3D11AddressMode ;
			SamplerDesc->AddressV = D3D11AddressMode ;
			SamplerDesc->AddressW = D3D11AddressMode ;
		}
	}
	else
	{
		if( Sampler < 0 || Sampler >= USE_TEXTURESTAGE_NUM )
		{
			return -1 ;
		}

		SamplerDesc = &GD3D11.Device.State.SamplerDesc[ Sampler ] ;
		if( D3D11AddressMode == SamplerDesc->AddressU &&
			D3D11AddressMode == SamplerDesc->AddressV &&
			D3D11AddressMode == SamplerDesc->AddressW &&
			GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
		{
			return 0 ;
		}

		DRAWSTOCKINFO

		GD3D11.Device.State.ChangeSamplerDescSlot[ Sampler ] = TRUE ;
		GD3D11.Device.State.ChangeSamplerDesc = TRUE ;

		SamplerDesc->AddressU = D3D11AddressMode ;
		SamplerDesc->AddressV = D3D11AddressMode ;
		SamplerDesc->AddressW = D3D11AddressMode ;
	}

	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

//	return Graphics_D3D11_DeviceState_SetupSamplerState() ;
	return 0 ;
}

// テクスチャーのアドレッシングモードをセットする
extern int Graphics_D3D11_DeviceState_SetTextureAddressU( int AddressMode, int Sampler )
{
	int								i ;
	D_D3D11_SAMPLER_DESC			*SamplerDesc ;
	D_D3D11_TEXTURE_ADDRESS_MODE	D3D11AddressMode ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	D3D11AddressMode = ( D_D3D11_TEXTURE_ADDRESS_MODE )AddressMode ;

	if( Sampler == -1 )
	{
		if( GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
		{
			SamplerDesc = GD3D11.Device.State.SamplerDesc ;
			for( i = 0 ; i < USE_TEXTURESTAGE_NUM ; i ++, SamplerDesc ++ )
			{
				if( D3D11AddressMode != SamplerDesc->AddressU )
				{
					break ;
				}
			}
			if( i == USE_TEXTURESTAGE_NUM )
			{
				return 0 ;
			}
		}

		DRAWSTOCKINFO

		SamplerDesc = GD3D11.Device.State.SamplerDesc ;
		for( i = 0 ; i < USE_TEXTURESTAGE_NUM ; i ++, SamplerDesc ++ )
		{
			if( D3D11AddressMode == SamplerDesc->AddressU &&
				GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
			{
				continue ;
			}

			GD3D11.Device.State.ChangeSamplerDescSlot[ i ] = TRUE ;
			GD3D11.Device.State.ChangeSamplerDesc = TRUE ;

			SamplerDesc->AddressU = D3D11AddressMode ;
		}
	}
	else
	{
		if( Sampler < 0 || Sampler >= USE_TEXTURESTAGE_NUM )
		{
			return -1 ;
		}

		SamplerDesc = &GD3D11.Device.State.SamplerDesc[ Sampler ] ;
		if( D3D11AddressMode == SamplerDesc->AddressU &&
			GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
		{
			return 0 ;
		}

		DRAWSTOCKINFO

		GD3D11.Device.State.ChangeSamplerDescSlot[ Sampler ] = TRUE ;
		GD3D11.Device.State.ChangeSamplerDesc = TRUE ;

		SamplerDesc->AddressU = D3D11AddressMode ;
	}

	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

//	return Graphics_D3D11_DeviceState_SetupSamplerState() ;
	return 0 ;
}

// テクスチャーのアドレッシングモードをセットする
extern int Graphics_D3D11_DeviceState_SetTextureAddressV( int AddressMode, int Sampler )
{
	int								i ;
	D_D3D11_SAMPLER_DESC			*SamplerDesc ;
	D_D3D11_TEXTURE_ADDRESS_MODE	D3D11AddressMode ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	D3D11AddressMode = ( D_D3D11_TEXTURE_ADDRESS_MODE )AddressMode ;

	if( Sampler == -1 )
	{
		if( GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
		{
			SamplerDesc = GD3D11.Device.State.SamplerDesc ;
			for( i = 0 ; i < USE_TEXTURESTAGE_NUM ; i ++, SamplerDesc ++ )
			{
				if( D3D11AddressMode != SamplerDesc->AddressV )
				{
					break ;
				}
			}
			if( i == USE_TEXTURESTAGE_NUM )
			{
				return 0 ;
			}
		}

		DRAWSTOCKINFO

		SamplerDesc = GD3D11.Device.State.SamplerDesc ;
		for( i = 0 ; i < USE_TEXTURESTAGE_NUM ; i ++, SamplerDesc ++ )
		{
			if( D3D11AddressMode == SamplerDesc->AddressV &&
				GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
			{
				continue ;
			}

			GD3D11.Device.State.ChangeSamplerDescSlot[ i ] = TRUE ;
			GD3D11.Device.State.ChangeSamplerDesc = TRUE ;

			SamplerDesc->AddressV = D3D11AddressMode ;
		}
	}
	else
	{
		if( Sampler < 0 || Sampler >= USE_TEXTURESTAGE_NUM )
		{
			return -1 ;
		}

		SamplerDesc = &GD3D11.Device.State.SamplerDesc[ Sampler ] ;
		if( D3D11AddressMode == SamplerDesc->AddressV &&
			GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
		{
			return 0 ;
		}

		DRAWSTOCKINFO

		GD3D11.Device.State.ChangeSamplerDescSlot[ Sampler ] = TRUE ;
		GD3D11.Device.State.ChangeSamplerDesc = TRUE ;

		SamplerDesc->AddressV = D3D11AddressMode ;
	}

	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

//	return Graphics_D3D11_DeviceState_SetupSamplerState() ;
	return 0 ;
}

// テクスチャーのアドレッシングモードをセットする
extern int Graphics_D3D11_DeviceState_SetTextureAddressW( int AddressMode, int Sampler )
{
	int								i ;
	D_D3D11_SAMPLER_DESC			*SamplerDesc ;
	D_D3D11_TEXTURE_ADDRESS_MODE	D3D11AddressMode ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	D3D11AddressMode = ( D_D3D11_TEXTURE_ADDRESS_MODE )AddressMode ;

	if( Sampler == -1 )
	{
		if( GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
		{
			SamplerDesc = GD3D11.Device.State.SamplerDesc ;
			for( i = 0 ; i < USE_TEXTURESTAGE_NUM ; i ++, SamplerDesc ++ )
			{
				if( D3D11AddressMode != SamplerDesc->AddressW )
				{
					break ;
				}
			}
			if( i == USE_TEXTURESTAGE_NUM )
			{
				return 0 ;
			}
		}

		DRAWSTOCKINFO

		SamplerDesc = GD3D11.Device.State.SamplerDesc ;
		for( i = 0 ; i < USE_TEXTURESTAGE_NUM ; i ++, SamplerDesc ++ )
		{
			if( D3D11AddressMode == SamplerDesc->AddressW &&
				GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
			{
				continue ;
			}

			GD3D11.Device.State.ChangeSamplerDescSlot[ i ] = TRUE ;
			GD3D11.Device.State.ChangeSamplerDesc = TRUE ;

			SamplerDesc->AddressW = D3D11AddressMode ;
		}
	}
	else
	{
		if( Sampler < 0 || Sampler >= USE_TEXTURESTAGE_NUM )
		{
			return -1 ;
		}

		SamplerDesc = &GD3D11.Device.State.SamplerDesc[ Sampler ] ;
		if( D3D11AddressMode == SamplerDesc->AddressW &&
			GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
		{
			return 0 ;
		}

		DRAWSTOCKINFO

		GD3D11.Device.State.ChangeSamplerDescSlot[ Sampler ] = TRUE ;
		GD3D11.Device.State.ChangeSamplerDesc = TRUE ;

		SamplerDesc->AddressW = D3D11AddressMode ;

	}

	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

//	return Graphics_D3D11_DeviceState_SetupSamplerState() ;
	return 0 ;
}

// テクスチャーのアドレッシングモードをセットする
extern int Graphics_D3D11_DeviceState_SetTextureAddressUVW( int AddressModeU, int AddressModeV, int AddressModeW, int Sampler )
{
	if( Graphics_D3D11_DeviceState_SetTextureAddressU( AddressModeU, Sampler ) < 0 )
	{
		return -1 ;
	}

	if( Graphics_D3D11_DeviceState_SetTextureAddressV( AddressModeV, Sampler ) < 0 )
	{
		return -1 ;
	}

	if( Graphics_D3D11_DeviceState_SetTextureAddressW( AddressModeW, Sampler ) < 0 )
	{
		return -1 ;
	}

	return 0 ;
}

// 標準描画用のテクスチャ座標変換行列を定数バッファにセットする
static int Graphics_D3D11_DeviceState_SetNormalTextureAddressTransformMatrix_ConstBuffer( void )
{
	DX_D3D11_VS_CONST_BUFFER_OTHERMATRIX *ConstOtherMatrix ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	ConstOtherMatrix = ( DX_D3D11_VS_CONST_BUFFER_OTHERMATRIX * )GD3D11.Device.Shader.Constant.ConstBuffer_VS_OtherMatrix->SysmemBuffer ;

	// 定数データに反映
	ConstOtherMatrix->TextureMatrix[ 0 ][ 0 ][ 0 ] = GD3D11.Device.State.TextureAddressTransformMatrix.m[ 0 ][ 0 ] ;
	ConstOtherMatrix->TextureMatrix[ 0 ][ 0 ][ 1 ] = GD3D11.Device.State.TextureAddressTransformMatrix.m[ 1 ][ 0 ] ;
	ConstOtherMatrix->TextureMatrix[ 0 ][ 0 ][ 2 ] = GD3D11.Device.State.TextureAddressTransformMatrix.m[ 2 ][ 0 ] ;
	ConstOtherMatrix->TextureMatrix[ 0 ][ 0 ][ 3 ] = GD3D11.Device.State.TextureAddressTransformMatrix.m[ 3 ][ 0 ] ;
	ConstOtherMatrix->TextureMatrix[ 0 ][ 1 ][ 0 ] = GD3D11.Device.State.TextureAddressTransformMatrix.m[ 0 ][ 1 ] ;
	ConstOtherMatrix->TextureMatrix[ 0 ][ 1 ][ 1 ] = GD3D11.Device.State.TextureAddressTransformMatrix.m[ 1 ][ 1 ] ;
	ConstOtherMatrix->TextureMatrix[ 0 ][ 1 ][ 2 ] = GD3D11.Device.State.TextureAddressTransformMatrix.m[ 2 ][ 1 ] ;
	ConstOtherMatrix->TextureMatrix[ 0 ][ 1 ][ 3 ] = GD3D11.Device.State.TextureAddressTransformMatrix.m[ 3 ][ 1 ] ;
	GD3D11.Device.Shader.Constant.ConstBuffer_VS_OtherMatrix->ChangeFlag = TRUE ;

	// 定数バッファを更新
//	Graphics_D3D11_ConstantBuffer_Update( GD3D11.Device.Shader.Constant.ConstBuffer_VS_OtherMatrix ) ;

	// 標準描画用のテクスチャパラメータが設定されているかどうかフラグを立てる
	GD3D11.Device.Shader.Constant.SetNormalTextureAddressTransformMatrix = TRUE ;

	// 正常終了
	return 0 ;
}

// テクスチャ座標変換行列をセットする
extern int  Graphics_D3D11_DeviceState_SetTextureAddressTransformMatrix( int Use, MATRIX *Matrix )
{
	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	if( GD3D11.Device.State.TextureAddressTransformMatrixUse == FALSE &&
		Use == FALSE && 
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	DRAWSTOCKINFO

	// テクスチャ座標変換行列を使用するかどうかを保存
	GD3D11.Device.State.TextureAddressTransformMatrixUse = Use ;

	// テクスチャ座標変換行列をセット
	if( Use == TRUE )
	{
		GD3D11.Device.State.TextureAddressTransformMatrix = *Matrix ;
	}
	else
	{
		CreateIdentityMatrix( &GD3D11.Device.State.TextureAddressTransformMatrix ) ;
	}

	// 定数バッファに反映する
	Graphics_D3D11_DeviceState_SetNormalTextureAddressTransformMatrix_ConstBuffer() ;

	return 0 ;
}

// フォグを有効にするかどうかを設定する( TRUE:有効  FALSE:無効 )
extern int  Graphics_D3D11_DeviceState_SetFogEnable( int Flag )
{
	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	if( Flag == GD3D11.Device.State.FogEnable &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	DRAWSTOCKINFO

	GD3D11.Device.State.FogEnable = Flag ;

	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// 終了
	return 0 ;
}

// フォグモードを設定する
extern int  Graphics_D3D11_DeviceState_SetFogVertexMode( int Mode /* DX_FOGMODE_NONE 等 */ )
{
	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	if( Mode == GD3D11.Device.State.FogMode &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	DRAWSTOCKINFO

	GD3D11.Device.State.FogMode = Mode ;

	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// 終了
	return 0 ;
}

// フォグカラーを変更する
extern int  Graphics_D3D11_DeviceState_SetFogColor( unsigned int Color )
{
	DX_D3D11_CONST_BUFFER_COMMON *ConstantCommon ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	if( Color == GD3D11.Device.State.FogColor &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	ConstantCommon = ( DX_D3D11_CONST_BUFFER_COMMON * )GD3D11.Device.Shader.Constant.ConstBuffer_Common->SysmemBuffer ;

	DRAWSTOCKINFO

	// 定数データに反映
	ConstantCommon->Fog.Color[ 0 ] = ( ( Color >> 16 ) & 0xff ) / 255.0f ;
	ConstantCommon->Fog.Color[ 1 ] = ( ( Color >>  8 ) & 0xff ) / 255.0f ;
	ConstantCommon->Fog.Color[ 2 ] = ( ( Color >>  0 ) & 0xff ) / 255.0f ;
	ConstantCommon->Fog.Color[ 3 ] = 1.0f ;
	GD3D11.Device.Shader.Constant.ConstBuffer_Common->ChangeFlag = TRUE ;

	// 定数バッファを更新
//	Graphics_D3D11_ConstantBuffer_Update( GD3D11.Device.Shader.Constant.ConstBuffer_Common ) ;

	GD3D11.Device.State.FogColor = Color ;

	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// 終了
	return 0 ;
}

// フォグの色以外の定数情報を更新する
static int Graphics_D3D11_DeviceState_UpdateConstantFogParam( void )
{
	DX_D3D11_CONST_BUFFER_COMMON *ConstantCommon ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	ConstantCommon = ( DX_D3D11_CONST_BUFFER_COMMON * )GD3D11.Device.Shader.Constant.ConstBuffer_Common->SysmemBuffer ;

	// 定数データに反映
	ConstantCommon->Fog.LinearAdd = GD3D11.Device.State.FogEnd / ( GD3D11.Device.State.FogEnd - GD3D11.Device.State.FogStart ) ;
	ConstantCommon->Fog.LinearDiv = -1.0f / ( GD3D11.Device.State.FogEnd - GD3D11.Device.State.FogStart ) ;
	ConstantCommon->Fog.Density   = GD3D11.Device.State.FogDensity ;
	ConstantCommon->Fog.E         = 2.71828183f ;
	GD3D11.Device.Shader.Constant.ConstBuffer_Common->ChangeFlag = TRUE ;

	// 定数バッファを更新
//	Graphics_D3D11_ConstantBuffer_Update( GD3D11.Device.Shader.Constant.ConstBuffer_Common ) ;

	// 終了
	return 0 ;
}

// フォグが始まる距離と終了する距離を設定する( 0.0f 〜 1.0f )
extern int  Graphics_D3D11_DeviceState_SetFogStartEnd( float Start, float End )
{
	int UpdateFlag ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	DRAWSTOCKINFO

	UpdateFlag = FALSE ;

	if( Start != GD3D11.Device.State.FogStart ||
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == TRUE )
	{
		GD3D11.Device.State.FogStart = Start ;
		UpdateFlag = TRUE ;
	}

	if( End != GD3D11.Device.State.FogEnd ||
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == TRUE )
	{
		GD3D11.Device.State.FogEnd = End ;
		UpdateFlag = TRUE ;
	}

	if( UpdateFlag )
	{
		// 定数情報を更新
		Graphics_D3D11_DeviceState_UpdateConstantFogParam() ;
	}

	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// 終了
	return 0 ;
}

// フォグの密度を設定する( 0.0f 〜 1.0f )
extern int  Graphics_D3D11_DeviceState_SetFogDensity( float Density )
{
	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	if( Density == GD3D11.Device.State.FogDensity &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	DRAWSTOCKINFO

	GD3D11.Device.State.FogDensity = Density ;

	// 定数情報を更新
	Graphics_D3D11_DeviceState_UpdateConstantFogParam() ;

	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// 終了
	return 0 ;
}

// ライティングの有無フラグをセットする
extern int  Graphics_D3D11_DeviceState_SetLighting( int UseFlag )
{
	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	if( UseFlag == GD3D11.Device.State.Lighting &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	DRAWSTOCKINFO

	GD3D11.Device.State.Lighting = UseFlag ;

	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// 終了
	return 0 ;
}

// 最大異方性をセットする
extern	int		Graphics_D3D11_DeviceState_SetMaxAnisotropy( int MaxAnisotropy, int Sampler )
{
	int						i ;
	D_D3D11_SAMPLER_DESC	*SamplerDesc ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// 値の補正
	if( MaxAnisotropy < 1 )
	{
		MaxAnisotropy = 1 ;
	}
	else
	if( MaxAnisotropy > 16 )
	{
		MaxAnisotropy = 16 ;
	}

	if( Sampler < 0 )
	{
		if( GD3D11.Device.State.MaxAnisotropy == MaxAnisotropy &&
			GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
		{
			return 0 ;
		}

		GD3D11.Device.State.MaxAnisotropy = MaxAnisotropy ;

		if( GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
		{
			SamplerDesc = GD3D11.Device.State.SamplerDesc ;
			for( i = 0 ; i < USE_TEXTURESTAGE_NUM ; i ++, SamplerDesc ++ )
			{
				if( SamplerDesc->MaxAnisotropy != ( UINT )MaxAnisotropy )
				{
					break ;
				}
			}
			if( i == USE_TEXTURESTAGE_NUM )
			{
				return 0 ;
			}
		}

		DRAWSTOCKINFO

		SamplerDesc = GD3D11.Device.State.SamplerDesc ;
		for( i = 0 ; i < USE_TEXTURESTAGE_NUM ; i ++, SamplerDesc ++ )
		{
			if( SamplerDesc->MaxAnisotropy == ( UINT )MaxAnisotropy &&
				GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
			{
				continue ;
			}

			GD3D11.Device.State.ChangeSamplerDescSlot[ i ] = TRUE ;
			GD3D11.Device.State.ChangeSamplerDesc = TRUE ;

			SamplerDesc->MaxAnisotropy = ( UINT )MaxAnisotropy ;
		}
	}
	else
	{
		if( Sampler < 0 || Sampler >= USE_TEXTURESTAGE_NUM )
		{
			return 0 ;
		}

		GD3D11.Device.State.MaxAnisotropy = -1 ;

		SamplerDesc = &GD3D11.Device.State.SamplerDesc[ Sampler ] ;
		if( SamplerDesc->MaxAnisotropy == ( UINT )MaxAnisotropy &&
			GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
		{
			return 0 ;
		}

		DRAWSTOCKINFO

		GD3D11.Device.State.ChangeSamplerDescSlot[ Sampler ] = TRUE ;
		GD3D11.Device.State.ChangeSamplerDesc = TRUE ;

		SamplerDesc->MaxAnisotropy = ( UINT )MaxAnisotropy ;
	}

	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

//	return Graphics_D3D11_DeviceState_SetupSamplerState() ;
	return 0 ;
}

// ビューポートをセットする
extern	int		Graphics_D3D11_DeviceState_SetViewport( D_D3D11_VIEWPORT *Viewport )
{
	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	if( Viewport->TopLeftX == GD3D11.Device.State.Viewport.TopLeftX &&
		Viewport->TopLeftY == GD3D11.Device.State.Viewport.TopLeftY &&
		Viewport->Width    == GD3D11.Device.State.Viewport.Width    &&
		Viewport->Height   == GD3D11.Device.State.Viewport.Height   &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	// 書き出す
	if( GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		DRAWSTOCKINFO
	}

	// 描画を終了させておく
	Graphics_D3D11_RenderEnd() ;

	// ビューポートのセッティング
	D3D11DeviceContext_RSSetViewports( 1, Viewport ) ;

	// ビューポートの情報の保存
	GD3D11.Device.State.Viewport = *Viewport ;

	// 終了
	return 0 ;
}

// ビューポートをセットする( 簡易版 )
extern	int		Graphics_D3D11_DeviceState_SetViewportEasy( float x1, float y1, float x2, float y2 )
{
	D_D3D11_VIEWPORT Viewport ;

	Viewport.TopLeftX = x1 ;
	Viewport.TopLeftY = y1 ;
	Viewport.Width    = x2 - x1 ;
	Viewport.Height   = y2 - y1 ;
	Viewport.MinDepth = 0.0f ;
	Viewport.MaxDepth = 1.0f ;

	return Graphics_D3D11_DeviceState_SetViewport( &Viewport ) ;
}

// 描画モードのセット
extern	int		Graphics_D3D11_DeviceState_SetDrawMode( int DrawMode )
{
	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	if( GD3D11.Device.State.DrawMode == DrawMode &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	if( GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		DRAWSTOCKINFO
	}

	// レンダリング設定を変更する
	switch( DrawMode )
	{
	case DX_DRAWMODE_BILINEAR :
		// バイリニア描画
		Graphics_D3D11_DeviceState_SetSampleFilterMode( D_D3D11_FILTER_MIN_MAG_MIP_LINEAR, -1 ) ;
		break ;

	case DX_DRAWMODE_NEAREST :
		// 二アレストネイバー
		Graphics_D3D11_DeviceState_SetSampleFilterMode( D_D3D11_FILTER_MIN_MAG_MIP_POINT, -1 ) ;
		break ;

	case DX_DRAWMODE_ANISOTROPIC :
		// 異方性フィルタリング
		Graphics_D3D11_DeviceState_SetSampleFilterMode( D_D3D11_FILTER_ANISOTROPIC, -1 ) ;
		break ;

	default : return -1 ;
	}

	GD3D11.Device.State.DrawMode                 = DrawMode ;
	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// 終了
	return 0 ;
}

// アルファテストで使用する比較モードを設定する
extern	int		Graphics_D3D11_DeviceState_SetAlphaTestCmpMode( int AlphaTestCmpMode /* DX_CMP_NEVER など */ )
{
	DX_D3D11_PS_CONST_BUFFER_BASE *ConstantPSBase ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	if( GD3D11.Device.State.AlphaTestCmpMode == AlphaTestCmpMode &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	ConstantPSBase = ( DX_D3D11_PS_CONST_BUFFER_BASE * )GD3D11.Device.Shader.Constant.ConstBuffer_PS_Base->SysmemBuffer ;

	DRAWSTOCKINFO

	// 値を保存
	GD3D11.Device.State.AlphaTestCmpMode = AlphaTestCmpMode ;

	// 定数データに反映
	ConstantPSBase->AlphaTestCmpMode = AlphaTestCmpMode ;

	GD3D11.Device.Shader.Constant.ConstBuffer_PS_Base->ChangeFlag = TRUE ;
	
	// 定数バッファを更新
//	Graphics_D3D11_ConstantBuffer_Update( GD3D11.Device.Shader.Constant.ConstBuffer_PS_Base ) ;

	// 終了
	return 0 ;
}

// アルファテストで使用する参照値を設定する
extern	int		Graphics_D3D11_DeviceState_SetAlphaTestRef( int AlphaTestRef )
{
	DX_D3D11_PS_CONST_BUFFER_BASE *ConstantPSBase ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	if( GD3D11.Device.State.AlphaTestRef == AlphaTestRef &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	ConstantPSBase = ( DX_D3D11_PS_CONST_BUFFER_BASE * )GD3D11.Device.Shader.Constant.ConstBuffer_PS_Base->SysmemBuffer ;

	DRAWSTOCKINFO

	// 値を保存
	GD3D11.Device.State.AlphaTestRef = AlphaTestRef ;

	// 定数データに反映
	ConstantPSBase->AlphaTestRef = AlphaTestRef / 255.0f ;

	GD3D11.Device.Shader.Constant.ConstBuffer_PS_Base->ChangeFlag = TRUE ;
	
	// 定数バッファを更新
//	Graphics_D3D11_ConstantBuffer_Update( GD3D11.Device.Shader.Constant.ConstBuffer_PS_Base ) ;

	// 終了
	return 0 ;
}

// カラーにアルファ値を乗算するかどうかを設定する
extern	int		Graphics_D3D11_DeviceState_SetMulAlphaColor( int UseMulAlphaColor )
{
	DX_D3D11_PS_CONST_BUFFER_BASE *ConstantPSBase ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	ConstantPSBase = ( DX_D3D11_PS_CONST_BUFFER_BASE * )GD3D11.Device.Shader.Constant.ConstBuffer_PS_Base->SysmemBuffer ;

	DRAWSTOCKINFO

	// 定数データに反映
	ConstantPSBase->MulAlphaColor = UseMulAlphaColor ? 1.0f : 0.0f ;

	GD3D11.Device.Shader.Constant.ConstBuffer_PS_Base->ChangeFlag = TRUE ;
	
	// 定数バッファを更新
//	Graphics_D3D11_ConstantBuffer_Update( GD3D11.Device.Shader.Constant.ConstBuffer_PS_Base ) ;

	// 終了
	return 0 ;
}

// デバイスで使用するプリミティブタイプを設定する
extern	int		Graphics_D3D11_DeviceState_SetPrimitiveTopology( D_D3D11_PRIMITIVE_TOPOLOGY PrimitiveTopology )
{
	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	if( GD3D11.Device.State.PrimitiveTopology == PrimitiveTopology &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	// 値を保存
	GD3D11.Device.State.PrimitiveTopology = PrimitiveTopology ;

	// プリミティブタイプをセット
	D3D11DeviceContext_IASetPrimitiveTopology( GD3D11.Device.State.PrimitiveTopology ) ;

	// 終了
	return 0 ;
}

// Factor Color を設定する
extern	int		Graphics_D3D11_DeviceState_SetFactorColor( const DX_D3D11_SHADER_FLOAT4 *FactorColor )
{
	DX_D3D11_PS_CONST_BUFFER_BASE *ConstantPSBase ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	ConstantPSBase = ( DX_D3D11_PS_CONST_BUFFER_BASE * )GD3D11.Device.Shader.Constant.ConstBuffer_PS_Base->SysmemBuffer ;

	if( ConstantPSBase->FactorColor[ 0 ] == (*FactorColor)[ 0 ] &&
		ConstantPSBase->FactorColor[ 1 ] == (*FactorColor)[ 1 ] &&
		ConstantPSBase->FactorColor[ 2 ] == (*FactorColor)[ 2 ] &&
		ConstantPSBase->FactorColor[ 3 ] == (*FactorColor)[ 3 ] &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	DRAWSTOCKINFO

	// 定数データに反映
	ConstantPSBase->FactorColor[ 0 ] = (*FactorColor)[ 0 ] ;
	ConstantPSBase->FactorColor[ 1 ] = (*FactorColor)[ 1 ] ;
	ConstantPSBase->FactorColor[ 2 ] = (*FactorColor)[ 2 ] ;
	ConstantPSBase->FactorColor[ 3 ] = (*FactorColor)[ 3 ] ;

	GD3D11.Device.Shader.Constant.ConstBuffer_PS_Base->ChangeFlag = TRUE ;
	
	// 定数バッファを更新
//	Graphics_D3D11_ConstantBuffer_Update( GD3D11.Device.Shader.Constant.ConstBuffer_PS_Base ) ;

	// 終了
	return 0 ;
}

// トゥーンレンダリングの輪郭線の太さを設定する
extern int Graphics_D3D11_DeviceState_SetToonOutLineSize( float Size )
{
	DX_D3D11_VS_CONST_BUFFER_BASE *ConstantVSBase ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	ConstantVSBase = ( DX_D3D11_VS_CONST_BUFFER_BASE * )GD3D11.Device.Shader.Constant.ConstBuffer_VS_Base->SysmemBuffer ;

	if( ConstantVSBase->ToonOutLineSize[ 0 ] == Size &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	DRAWSTOCKINFO

	// 定数データに反映
	ConstantVSBase->ToonOutLineSize[ 0 ] = Size ;
	ConstantVSBase->ToonOutLineSize[ 1 ] = Size ;
	ConstantVSBase->ToonOutLineSize[ 2 ] = Size ;
	ConstantVSBase->ToonOutLineSize[ 3 ] = Size ;

	GD3D11.Device.Shader.Constant.ConstBuffer_VS_Base->ChangeFlag = TRUE ;
	
	// 定数バッファを更新
//	Graphics_D3D11_ConstantBuffer_Update( GD3D11.Device.Shader.Constant.ConstBuffer_VS_Base ) ;

	// 終了
	return 0 ;
}

// 描画ブレンドモードのセット
extern	int		Graphics_D3D11_DeviceState_SetBlendMode( int BlendMode )
{
	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	if( GD3D11.Device.State.BlendMode == BlendMode &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	DRAWSTOCKINFO

	GD3D11.Device.State.BlendMode = BlendMode ;
	GD3D11.Device.State.ChangeBlendDesc = TRUE ;

	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

//	return Graphics_D3D11_DeviceState_SetupBlendState() ;
	return 0 ;
}

// 全ての描画対象のサイズが一致していたらレンダリングターゲットにセット
static	void	Graphics_D3D11_DeviceState_SetRenderTarget_SizeCheck( void )
{
	int i, Base ;

	for( Base = 0; Base < DX_RENDERTARGET_COUNT ; Base ++ )
	{
		if( GD3D11.Device.State.TargetTexture2D[ Base ] != NULL )
		{
			break ;
		}
	}

	for( i = Base + 1 ; i < DX_RENDERTARGET_COUNT ; i ++ )
	{
		if( GD3D11.Device.State.TargetTexture2D[ i ] != NULL &&
			( GD3D11.Device.State.TargetTexture2DDesc[ i ].Width  != GD3D11.Device.State.TargetTexture2DDesc[ Base ].Width  ||
			  GD3D11.Device.State.TargetTexture2DDesc[ i ].Height != GD3D11.Device.State.TargetTexture2DDesc[ Base ].Height ) )
		{
			return ;
		}
	}

	if( GD3D11.Device.State.TargetDepthTexture2D != NULL &&
		( GD3D11.Device.State.TargetDepthTexture2DDesc.Width  != GD3D11.Device.State.TargetTexture2DDesc[ Base ].Width  ||
		  GD3D11.Device.State.TargetDepthTexture2DDesc.Height != GD3D11.Device.State.TargetTexture2DDesc[ Base ].Height ) )
	{
		return ;
	}

	D3D11DeviceContext_OMSetRenderTargets( DX_RENDERTARGET_COUNT, GD3D11.Device.State.TargetRTV, GD3D11.Device.State.TargetDepthDSV ) ;
}

// 描画対象の変更
extern	int		Graphics_D3D11_DeviceState_SetRenderTarget( D_ID3D11Texture2D *RenderTargetTexture, D_ID3D11RenderTargetView *RenderTargetView, int TargetIndex )
{
	D_D3D11_TEXTURE2D_DESC      Texture2DDesc ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	if( TargetIndex < 0 || TargetIndex >= DX_RENDERTARGET_COUNT )
	{
		return -1 ;
	}

	if( GD3D11.Device.State.TargetTexture2D[ TargetIndex ] == RenderTargetTexture &&
		GD3D11.Device.State.TargetRTV[ TargetIndex ]       == RenderTargetView &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck  == FALSE )
	{
		return 0 ;
	}

	// 頂点書き出し
	DRAWSTOCKINFO

	// レンダリングターゲットの保存
	GD3D11.Device.State.TargetTexture2D[ TargetIndex ] = RenderTargetTexture ;
	GD3D11.Device.State.TargetRTV[ TargetIndex ]       = RenderTargetView ;
	if( RenderTargetTexture != NULL )
	{
		D3D11Texture2D_GetDesc( RenderTargetTexture, &GD3D11.Device.State.TargetTexture2DDesc[ TargetIndex ] ) ;
	}
	else
	{
		_MEMSET( &GD3D11.Device.State.TargetTexture2DDesc[ TargetIndex ], 0, sizeof( D_D3D11_TEXTURE2D_DESC ) ) ;
	}

	// レンダリングターゲットの変更
	Graphics_D3D11_DeviceState_SetRenderTarget_SizeCheck() ;

	// ビューポートの設定値を更新
	if( TargetIndex == 0 && RenderTargetTexture != NULL )
	{
		int Width, Height ;
		int MipSlice = 0 ;
		D_D3D11_RENDER_TARGET_VIEW_DESC RTVDesc ;

		D3D11Texture2D_GetDesc( RenderTargetTexture, &Texture2DDesc ) ; 
		D3D11RenderTargetView_GetDesc( RenderTargetView, &RTVDesc ) ;

		switch( RTVDesc.ViewDimension )
		{
		case D_D3D11_RTV_DIMENSION_TEXTURE2D :
			MipSlice = RTVDesc.Texture2D.MipSlice ;
			break ;

		case D_D3D11_RTV_DIMENSION_TEXTURE2DARRAY :
			MipSlice = RTVDesc.Texture2DArray.MipSlice ;
			break ;

		case D_D3D11_RTV_DIMENSION_TEXTURE2DMS :
		case D_D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY :
			MipSlice = 0 ;
			break ;
		}
		Graphics_Image_CalcMipSize( Texture2DDesc.Width, Texture2DDesc.Height, MipSlice, &Width, &Height ) ;
		Graphics_D3D11_DeviceState_SetViewportEasy( 0.0f, 0.0f, ( float )Width, ( float )Height ) ;
	}

	// 終了
	return  0 ;
}

// 描画対象の深度バッファを変更
extern	int		Graphics_D3D11_DeviceState_SetDepthStencil( D_ID3D11Texture2D *DepthStencilTexture, D_ID3D11DepthStencilView *DepthStencilView )
{
	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	if( GD3D11.Device.State.TargetDepthTexture2D == DepthStencilTexture &&
		GD3D11.Device.State.TargetDepthDSV       == DepthStencilView &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck  == FALSE )
	{
		return 0 ;
	}

	// 頂点書き出し
	DRAWSTOCKINFO

	// 使用する深度バッファの保存
	GD3D11.Device.State.TargetDepthTexture2D = DepthStencilTexture ;
	GD3D11.Device.State.TargetDepthDSV       = DepthStencilView ;
	if( DepthStencilTexture != NULL )
	{
		DepthStencilTexture->GetDesc( &GD3D11.Device.State.TargetDepthTexture2DDesc ) ;
	}
	else
	{
		_MEMSET( &GD3D11.Device.State.TargetDepthTexture2DDesc, 0, sizeof( D_D3D11_TEXTURE2D_DESC ) ) ;
	}

	// レンダリングターゲットの変更
	Graphics_D3D11_DeviceState_SetRenderTarget_SizeCheck() ;

	// 終了
	return  0 ;
}

// 使用する入力レイアウトを変更する
extern	int		Graphics_D3D11_DeviceState_SetInputLayout( D_ID3D11InputLayout *InputLayout )
{
	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	if( GD3D11.Device.State.SetVertexInputLayout == InputLayout &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	if( InputLayout != NULL )
	{
		D3D11DeviceContext_IASetInputLayout( InputLayout ) ;
	}
	GD3D11.Device.State.SetVertexInputLayout = InputLayout ;

	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// 終了
	return 0 ;
}

// 指定の頂点シェーダーをデバイスにセットする
extern int Graphics_D3D11_DeviceState_SetVertexShaderToHandle( int ShaderHandle )
{
	SHADERHANDLEDATA *VertexShader ;

	// ハンドルチェック
	if( SHADERCHK( ShaderHandle, VertexShader ) )
	{
		return -1 ;
	}
	if( VertexShader->IsVertexShader == FALSE )
	{
		return -1 ;
	}

	// シェーダーのセット
	Graphics_D3D11_DeviceState_SetVertexShader( VertexShader->PF->D3D11.VertexShader ) ;

	// 正常終了
	return 0 ;
}

// 使用する頂点シェーダーを変更する
extern	int		Graphics_D3D11_DeviceState_SetVertexShader( D_ID3D11VertexShader *VertexShader, int NormalVertexShader )
{
	if( GD3D11.Device.State.SetVertexShader == VertexShader &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	D3D11DeviceContext_VSSetShader( VertexShader, NULL, 0 ) ;
	GD3D11.Device.State.SetVertexShader          = VertexShader ;
	GD3D11.Device.State.SetNormalVertexShader    = NormalVertexShader ;
	if( NormalVertexShader == FALSE )
	{
		GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;
	}

	// 終了
	return 0 ;
}

// 頂点シェーダーの使用を止める
extern	int		Graphics_D3D11_DeviceState_ResetVertexShader( int SetNormalVertexShaderCancel )
{
	if( GD3D11.Device.State.SetNormalVertexShader &&
		SetNormalVertexShaderCancel &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	if( GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE &&
		GD3D11.Device.State.SetVertexShader == NULL &&
		GD3D11.Device.State.SetVertexInputLayout == NULL )
	{
		return 0 ;
	}

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	if( GD3D11.Device.DrawSetting.CancelSettingEqualCheck ||
		GD3D11.Device.State.SetVertexShader != NULL )
	{
		D3D11DeviceContext_VSSetShader( NULL, NULL, 0 ) ;
		GD3D11.Device.State.SetVertexShader = NULL ;
	}

	if( GD3D11.Device.DrawSetting.CancelSettingEqualCheck ||
		GD3D11.Device.State.SetVertexInputLayout != NULL )
	{
		D3D11DeviceContext_IASetInputLayout( NULL ) ;
		GD3D11.Device.State.SetVertexInputLayout = NULL ;
	}

	GD3D11.Device.State.SetNormalVertexShader    = FALSE ;
	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// 終了
	return 0 ;
}

// 頂点シェーダーで使用する定数バッファーを設定する
extern	int		Graphics_D3D11_DeviceState_SetVertexShaderConstantBuffers( UINT StartSlot, UINT NumBuffers, D_ID3D11Buffer * const *pConstantBuffers )
{
	UINT i ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	if( StartSlot < 0 || StartSlot + NumBuffers > D_D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT )
	{
		return -1 ;
	}

	if( GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		for( i = 0 ; i < NumBuffers ; i ++ )
		{
			if( GD3D11.Device.State.SetVertexShaderConstantBuffer[ i + StartSlot ] != pConstantBuffers[ i ] )
			{
				break ;
			}
		}
		if( i == NumBuffers )
		{
			return 0 ;
		}
	}

	D3D11DeviceContext_VSSetConstantBuffers( StartSlot, NumBuffers, pConstantBuffers ) ;

	for( i = 0 ; i < NumBuffers ; i ++ )
	{
		GD3D11.Device.State.SetVertexShaderConstantBuffer[ i + StartSlot ] = pConstantBuffers[ i ] ;
	}

	// 終了
	return 0 ;
}


// 指定のピクセルシェーダーをデバイスにセットする
extern int Graphics_D3D11_DeviceState_SetPixelShaderToHandle( int ShaderHandle )
{
	SHADERHANDLEDATA *PixelShader ;

	// ハンドルチェック
	if( SHADERCHK( ShaderHandle, PixelShader ) )
	{
		return -1 ;
	}
	if( PixelShader->IsVertexShader != FALSE )
	{
		return -1 ;
	}

	// シェーダーのセット
	Graphics_D3D11_DeviceState_SetPixelShader( PixelShader->PF->D3D11.PixelShader ) ;

	// 正常終了
	return 0 ;
}

// 使用するピクセルシェーダーを変更する
extern	int		Graphics_D3D11_DeviceState_SetPixelShader( D_ID3D11PixelShader *PixelShader, int NormalPixelShader )
{
	if( GD3D11.Device.State.SetPixelShader == PixelShader &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	D3D11DeviceContext_PSSetShader( PixelShader, NULL, 0 ) ;
	GD3D11.Device.State.SetPixelShader       = PixelShader ;
	GD3D11.Device.State.SetNormalPixelShader = NormalPixelShader ;
	if( NormalPixelShader == FALSE )
	{
		GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;
	}

	// 終了
	return 0 ;
}

// ピクセルシェーダーの使用を止める
extern	int		Graphics_D3D11_DeviceState_ResetPixelShader( int SetNormalPixelShaderCancel )
{
	if( GD3D11.Device.State.SetNormalPixelShader &&
		SetNormalPixelShaderCancel &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	if( GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE &&
		GD3D11.Device.State.SetPixelShader == NULL )
	{
		return 0 ;
	}

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	if( GD3D11.Device.DrawSetting.CancelSettingEqualCheck ||
		GD3D11.Device.State.SetPixelShader != NULL )
	{
		D3D11DeviceContext_PSSetShader( NULL, NULL, 0 ) ;
		GD3D11.Device.State.SetPixelShader = NULL ;
	}

	GD3D11.Device.State.SetNormalPixelShader     = FALSE ;
	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// 終了
	return 0 ;
}

// ピクセルシェーダーで使用する定数バッファーを設定する
extern	int		Graphics_D3D11_DeviceState_SetPixelShaderConstantBuffers( UINT StartSlot, UINT NumBuffers, D_ID3D11Buffer * const *pConstantBuffers )
{
	UINT i ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	if( StartSlot < 0 || StartSlot + NumBuffers > D_D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT )
	{
		return -1 ;
	}

	if( GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		for( i = 0 ; i < NumBuffers ; i ++ )
		{
			if( GD3D11.Device.State.SetPixelShaderConstantBuffer[ i + StartSlot ] != pConstantBuffers[ i ] )
			{
				break ;
			}
		}
		if( i == NumBuffers )
		{
			return 0 ;
		}
	}

	D3D11DeviceContext_PSSetConstantBuffers( StartSlot, NumBuffers, pConstantBuffers ) ;

	for( i = 0 ; i < NumBuffers ; i ++ )
	{
		GD3D11.Device.State.SetPixelShaderConstantBuffer[ i + StartSlot ] = pConstantBuffers[ i ] ;
	}

	// 終了
	return 0 ;
}

// 使用する頂点バッファを変更する
extern	int		Graphics_D3D11_DeviceState_SetVertexBuffer( D_ID3D11Buffer *VertexBuffer, int VertexStride )
{
	UINT Offsets ;

	if( GD3D11.Device.State.SetVertexBuffer       == VertexBuffer &&
		GD3D11.Device.State.SetVertexBufferStride == VertexStride &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	if( VertexBuffer != NULL )
	{
		Offsets = 0 ;
		D3D11DeviceContext_IASetVertexBuffers( 0, 1, &VertexBuffer, ( UINT* )&VertexStride, &Offsets ) ;
	}
	GD3D11.Device.State.SetVertexBuffer       = VertexBuffer ;
	GD3D11.Device.State.SetVertexBufferStride = VertexStride ;

	// 終了
	return 0 ;
}

// 使用するインデックスバッファを変更する
extern	int		Graphics_D3D11_DeviceState_SetIndexBuffer( D_ID3D11Buffer *IndexBuffer, D_DXGI_FORMAT Format )
{
	if( GD3D11.Device.State.SetIndexBuffer == IndexBuffer &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	if( IndexBuffer != NULL )
	{
		D3D11DeviceContext_IASetIndexBuffer( IndexBuffer, Format, 0 ) ;
	}
	GD3D11.Device.State.SetIndexBuffer       = IndexBuffer ;
	GD3D11.Device.State.SetIndexBufferFormat = Format ;

	// 終了
	return 0 ;
}

// ピクセルシェーダーで使用するシェーダーリソースビューを変更する
extern int Graphics_D3D11_DeviceState_SetPSShaderResouceView( int StartSlot, int Num, D_ID3D11ShaderResourceView * const *ppShaderResourceViews )
{
	int i ;
	int Change ;
	int FixStartI = 0 ;
	int FixStartSlot ;
	int FixNum ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	FixStartSlot = -1 ;
	FixNum = 0 ;
	Change = FALSE ;
	for( i = 0; i < Num; i++ )
	{
		if( GD3D11.Device.State.PSSetShaderResourceView[ StartSlot + i ] != ppShaderResourceViews[ i ] ||
			GD3D11.Device.DrawSetting.CancelSettingEqualCheck )
		{
			if( Change == FALSE )
			{
				Change = TRUE ;
				FixStartI = i ;
				FixStartSlot = StartSlot + i ;
			}
			FixNum = i - FixStartI + 1 ;
			GD3D11.Device.State.PSSetShaderResourceView[ StartSlot + i ] = ppShaderResourceViews[ i ] ;
		}
	}
	if( Change == FALSE )
	{
		return 0 ;
	}
	D3D11DeviceContext_PSSetShaderResources( ( UINT )FixStartSlot, ( UINT )FixNum, &ppShaderResourceViews[ FixStartI ] ) ;

	// 終了
	return 0 ;
}

// 標準描画の準備を行う
extern	int		Graphics_D3D11_DeviceState_NormalDrawSetup( void )
{
	int							IgnoreTextureAlpha ;
	int							NextBlendMode ;
	int							UseFloatFactorColor = FALSE ;
	DX_D3D11_SHADER_FLOAT4		FloatFactorColor ;
	int							AlphaTestRef = 0 ;
	int							AlphaTestCmpMode = 0 ;
	D_ID3D11ShaderResourceView	*ShaderResourceView[ USE_TEXTURESTAGE_NUM ] = { NULL } ;
	int							ShaderResourceViewNum = 0 ;
	D_ID3D11PixelShader			*UsePixelShader = NULL ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// テクスチャが変更された、フラグを倒す
	GD3D11.Device.DrawSetting.ChangeTextureFlag = FALSE ;
	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// パラメータが変更された、フラグを倒す
	GD3D11.Device.DrawSetting.ChangeBlendParamFlag = FALSE ;

	// ブレンドモードの決定
	{
		NextBlendMode = GD3D11.Device.DrawSetting.BlendMode ;
		switch( GD3D11.Device.DrawSetting.BlendMode )
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
			if( GD3D11.Device.DrawSetting.RenderTexture != NULL )
			{
				if( GD3D11.Device.DrawSetting.BlendTexture != NULL )
				{
					switch( GD3D11.Device.DrawSetting.BlendGraphType )
					{
					case DX_BLENDGRAPHTYPE_NORMAL :
					case DX_BLENDGRAPHTYPE_ALPHA :
						if( GD3D11.Device.DrawSetting.AlphaTestValidFlag    == TRUE ||
							GD3D11.Device.DrawSetting.AlphaChannelValidFlag == TRUE )
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
					if( GD3D11.Device.DrawSetting.AlphaChannelValidFlag == TRUE )
					{
						NextBlendMode = DX_BLENDMODE_ALPHA ;
					}
				}
			}
			break ;
		}
	}
	
	// カレントテクスチャが無い場合とある場合で分岐
	if( GD3D11.Device.DrawSetting.RenderTexture == NULL )
	{
		// カレントテクスチャが無い場合は線や箱の描画と判断、テクスチャステージは無効にする

		// とりあえず描画処理を行う
		GD3D11.Device.DrawInfo.BlendMaxNotDrawFlag = FALSE ;

		// アルファテストパラメータが有効な場合はそれを優先する
		if( GD3D11.Device.DrawSetting.AlphaTestMode != -1 )
		{
			GD3D11.Device.State.AlphaTestEnable = TRUE ;
			AlphaTestCmpMode = GD3D11.Device.DrawSetting.AlphaTestMode ;
			AlphaTestRef     = GD3D11.Device.DrawSetting.AlphaTestParam ;
		}
		else
		{
			GD3D11.Device.State.AlphaTestEnable = FALSE ;
		}

		// 使用するピクセルシェーダーの取得
		GD3D11.Device.State.AlphaTestModeShaderIndex = Graphics_D3D11_Shader_GetAlphaTestModeIndex( GD3D11.Device.State.AlphaTestEnable, AlphaTestCmpMode ) ;
		UsePixelShader = GD3D11.Device.Shader.Base.BaseNoneTex_PS[ g_DefaultBlendDescArray[ NextBlendMode ].RenderBlendType ][ GD3D11.Device.State.AlphaTestModeShaderIndex ] ;

		// 頂点タイプのセット
		GD3D11.Device.DrawInfo.VertexType = VERTEXTYPE_NOTEX ;

#if VERTEXBUFFERMODE == 2
		NORMALVERTEXBUFFER_MAP
		GD3D11.Device.DrawInfo.VertexBufferNextAddr = GD3D11.Device.DrawInfo.VertexBufferAddr ;
#else
		GD3D11.Device.DrawInfo.VertexBufferNextAddr = GD3D11.Device.DrawInfo.VertexBufferTemp ;
#endif
	}
	else
	{
		// カレントテクスチャがある場合はテクスチャステージブレンドテーブルから値を取得する

		// 使用するテクスチャブレンドステージステータステーブルのインデックスをセット
		IgnoreTextureAlpha = 1 ;
		if( GD3D11.Device.DrawSetting.AlphaChannelValidFlag ||
			GD3D11.Device.DrawSetting.AlphaTestValidFlag    ||
			( GD3D11.Device.State.DepthStencilDesc.DepthEnable &&
			  GD3D11.Device.DrawSetting.BlendTexture == NULL ) )
		{
			IgnoreTextureAlpha = 0 ;
		}

		// ブレンドテクスチャーを使用するかどうかで分岐
		if( GD3D11.Device.DrawSetting.BlendTexture != NULL )
		{
			int RenderBlendType ;

			// ブレンドテクスチャーがある場合は AlphaTestValidFlag は無視

			// 使用するテーブルのインデックスを割り出す
			RenderBlendType = g_DefaultBlendDescArray[ NextBlendMode ].RenderBlendType ;

			// 使用するテクスチャーアドレスをセット
			ShaderResourceView[ 0 ] = GD3D11.Device.DrawSetting.RenderTextureSRV ;
			ShaderResourceView[ 1 ] = GD3D11.Device.DrawSetting.BlendTextureSRV ;
			ShaderResourceViewNum = 2 ;

			// 頂点タイプのセット
			GD3D11.Device.DrawInfo.VertexType = VERTEXTYPE_BLENDTEX ;

#if VERTEXBUFFERMODE == 2
			NORMALVERTEXBUFFER_MAP
			GD3D11.Device.DrawInfo.VertexBufferNextAddr = GD3D11.Device.DrawInfo.VertexBufferAddr ;
#else
			GD3D11.Device.DrawInfo.VertexBufferNextAddr = GD3D11.Device.DrawInfo.VertexBufferTemp ;
#endif

			// ブレンドタイプによって処理を分岐
			switch( GD3D11.Device.DrawSetting.BlendGraphType )
			{
			case DX_BLENDGRAPHTYPE_NORMAL :
				// とりあえず描画処理を行う
				GD3D11.Device.DrawInfo.BlendMaxNotDrawFlag = FALSE ;

				// アルファテストパラメータが有効な場合はそれを優先する
				if( GD3D11.Device.DrawSetting.AlphaTestMode != -1 )
				{
					GD3D11.Device.State.AlphaTestEnable = TRUE ;
					AlphaTestCmpMode = GD3D11.Device.DrawSetting.AlphaTestMode ;
					AlphaTestRef     = GD3D11.Device.DrawSetting.AlphaTestParam ;
				}
				else
				{
					// α処理が有効な場合はα値が０の場合のみ表示されないようにする
					if( GD3D11.Device.DrawSetting.AlphaTestValidFlag == TRUE ||
						g_DefaultBlendDescArray[ NextBlendMode ].RenderTargetBlendDesc.BlendEnable == TRUE ||
						GD3D11.Device.DrawSetting.AlphaChannelValidFlag == TRUE )
					{
						GD3D11.Device.State.AlphaTestEnable = TRUE ;
						AlphaTestRef     = 0 ;
						AlphaTestCmpMode = DX_CMP_GREATER ;
					}
					else
					{
						GD3D11.Device.State.AlphaTestEnable = FALSE ;
					}
				}

				// クロスフェード率を設定する
				UseFloatFactorColor = TRUE ;
				FloatFactorColor[ 0 ] = 1.0f ;
				FloatFactorColor[ 1 ] = 1.0f ;
				FloatFactorColor[ 2 ] = 1.0f ;
				FloatFactorColor[ 3 ] = GD3D11.Device.DrawSetting.BlendGraphFadeRatio / 255.0f ;

				// 使用するシェーダーをセット
				GD3D11.Device.State.AlphaTestModeShaderIndex = Graphics_D3D11_Shader_GetAlphaTestModeIndex( GD3D11.Device.State.AlphaTestEnable, AlphaTestCmpMode ) ;
				UsePixelShader = GD3D11.Device.Shader.Base.BaseUseTex_PS[ 1 ][ RenderBlendType ][ GD3D11.Device.DrawSetting.IgnoreGraphColorFlag ? 1 : 0 ][ IgnoreTextureAlpha ][ GD3D11.Device.State.AlphaTestModeShaderIndex ] ;
				break ;

			case DX_BLENDGRAPHTYPE_WIPE :
				// ブレンド境界値が最大の場合は何も描画しない
				GD3D11.Device.DrawInfo.BlendMaxNotDrawFlag = GD3D11.Device.DrawSetting.BlendGraphBorderParam == 255 ? TRUE : FALSE ;

				// float型の FactorColor を使用する
				UseFloatFactorColor = TRUE ;
				FloatFactorColor[ 0 ] = 0.0f ;
				FloatFactorColor[ 1 ] = 0.0f ;
				FloatFactorColor[ 2 ] = 0.0f ;
				FloatFactorColor[ 3 ] = 0.0f ;

				// 境界範囲が１以下かどうかで処理を分岐
				if( GD3D11.Device.DrawSetting.BlendGraphBorderRange <= 1 )
				{
					// ここはもう殆ど例外処理 -------------------------------------------

					// αテストを使用する
					GD3D11.Device.State.AlphaTestEnable = TRUE ;
					AlphaTestRef     = GD3D11.Device.DrawSetting.BlendGraphBorderParam ;
					AlphaTestCmpMode = DX_CMP_GREATEREQUAL ;

					// αブレンド機能ＯＦＦ
					NextBlendMode = DX_BLENDMODE_NOBLEND ;

					// 増分するα値は無し
					FloatFactorColor[ 3 ] = 0.0f ;
					FloatFactorColor[ 2 ] = 1.0f ;

					// もし画像がαチャンネルを使用していて、且つ BlendGraphBorderParam が 0 だったら AlphaRef を０にしない
					if( ( GD3D11.Device.DrawSetting.AlphaChannelValidFlag ||
						  GD3D11.Device.DrawSetting.AlphaTestValidFlag ) &&
						GD3D11.Device.DrawSetting.BlendGraphBorderParam == 0 )
					{
						AlphaTestRef = 1 ;
					}
				}
				else
				{
					int p ;
				
					// αブレンドによるテクスチャブレンド

					// αテストは行わない
					GD3D11.Device.State.AlphaTestEnable = FALSE ;

					// 境界範囲によって４段階
					if( GD3D11.Device.DrawSetting.BlendGraphBorderRange <= 64 )
					{
						// ×４

						p = 1280 - GD3D11.Device.DrawSetting.BlendGraphBorderParam * 1280 / 255 ; 

						// ブレンドテクスチャのブレンド値によって合成パラメータを変更
						if( p < 1024 )
						{
							FloatFactorColor[ 3 ] = - ( float )( 255 - p * 255 / 1024 ) / 255.0f ;
						}
						else
						{
							FloatFactorColor[ 3 ] =   ( float )( 64 * ( p - 1024 ) / 256 ) / 255.0f ;
						}

						FloatFactorColor[ 2 ] = 4.0f ;
					}
					else
					if( GD3D11.Device.DrawSetting.BlendGraphBorderRange <= 128 )
					{
						// ×２

						p = 768 - GD3D11.Device.DrawSetting.BlendGraphBorderParam * 768 / 255 ; 

						// ブレンドテクスチャのブレンド値によって合成パラメータを変更
						if( p < 512 )
						{
							FloatFactorColor[ 3 ] = - ( float )( 255 - p * 255 / 512 ) / 255.0f ;
						}
						else
						{
							FloatFactorColor[ 3 ] =   ( float )( 128 * ( p - 512 ) / 256 ) / 255.0f ;
						}

						FloatFactorColor[ 2 ] = 2.0f ;
					}
					else
					{
						// ×１

						p = 512 - GD3D11.Device.DrawSetting.BlendGraphBorderParam * 512 / 255 ; 

						// ブレンドテクスチャのブレンド値によって合成パラメータを変更
						if( p < 256 )
						{
							FloatFactorColor[ 3 ] = - ( float )( 255 - p * 255 / 256 ) / 255.0f ;
						}
						else
						{
							FloatFactorColor[ 3 ] =   ( float )( 255 * ( p - 256 ) / 256 ) / 255.0f ;
						}

						FloatFactorColor[ 2 ] = 1.0f ;
					}
				}

				// 使用するシェーダーをセット
				GD3D11.Device.State.AlphaTestModeShaderIndex = Graphics_D3D11_Shader_GetAlphaTestModeIndex( GD3D11.Device.State.AlphaTestEnable, AlphaTestCmpMode ) ;
				UsePixelShader = GD3D11.Device.Shader.Base.BaseUseTex_PS[ 2 ][ RenderBlendType ][ GD3D11.Device.DrawSetting.IgnoreGraphColorFlag ? 1 : 0 ][ IgnoreTextureAlpha ][ GD3D11.Device.State.AlphaTestModeShaderIndex ] ;
				break ;

			case DX_BLENDGRAPHTYPE_ALPHA :
				// とりあえず描画処理を行う
				GD3D11.Device.DrawInfo.BlendMaxNotDrawFlag = FALSE ;

				// アルファテストパラメータが有効な場合はそれを優先する
				if( GD3D11.Device.DrawSetting.AlphaTestMode != -1 )
				{
					GD3D11.Device.State.AlphaTestEnable = TRUE ;
					AlphaTestCmpMode = GD3D11.Device.DrawSetting.AlphaTestMode ;
					AlphaTestRef     = GD3D11.Device.DrawSetting.AlphaTestParam ;
				}
				else
				{
					// α処理が有効な場合はα値が０の場合のみ表示されないようにする
					if( g_DefaultBlendDescArray[ NextBlendMode ].RenderTargetBlendDesc.BlendEnable == TRUE )
					{
						GD3D11.Device.State.AlphaTestEnable = TRUE ;
						AlphaTestRef     = 0 ;
						AlphaTestCmpMode = DX_CMP_GREATER ;
					}
					else
					{
						GD3D11.Device.State.AlphaTestEnable = FALSE ;
					}
				}

				// 使用するシェーダーをセット
				GD3D11.Device.State.AlphaTestModeShaderIndex = Graphics_D3D11_Shader_GetAlphaTestModeIndex( GD3D11.Device.State.AlphaTestEnable, AlphaTestCmpMode ) ;
				UsePixelShader = GD3D11.Device.Shader.Base.BaseUseTex_PS[ 3 ][ RenderBlendType ][ GD3D11.Device.DrawSetting.IgnoreGraphColorFlag ? 1 : 0 ][ IgnoreTextureAlpha ][ GD3D11.Device.State.AlphaTestModeShaderIndex ] ;
				break ;
			}
		}
		else
		{
			// ブレンドテクスチャーが無い場合

			// とりあえず描画処理を行う
			GD3D11.Device.DrawInfo.BlendMaxNotDrawFlag = FALSE ;

			// 頂点タイプのセット
			GD3D11.Device.DrawInfo.VertexType = VERTEXTYPE_TEX ;

#if VERTEXBUFFERMODE == 2
			NORMALVERTEXBUFFER_MAP
			GD3D11.Device.DrawInfo.VertexBufferNextAddr = GD3D11.Device.DrawInfo.VertexBufferAddr ;
#else
			GD3D11.Device.DrawInfo.VertexBufferNextAddr = GD3D11.Device.DrawInfo.VertexBufferTemp ;
#endif

			// アルファテストパラメータが有効な場合はそれを優先する
			if( GD3D11.Device.DrawSetting.AlphaTestMode != -1 )
			{
				GD3D11.Device.State.AlphaTestEnable = TRUE ;
				AlphaTestCmpMode = GD3D11.Device.DrawSetting.AlphaTestMode ;
				AlphaTestRef     = GD3D11.Device.DrawSetting.AlphaTestParam ;
			}
			else
			{
				// αテストを使用するかどうかで処理を分岐
				if( GD3D11.Device.DrawSetting.AlphaTestValidFlag == TRUE )
				{
					// αテストを行う(透過色処理以外にαテストはブレンドテクスチャでも使用している)
					GD3D11.Device.State.AlphaTestEnable = TRUE ;
					AlphaTestRef     = 16 ;
					AlphaTestCmpMode = DX_CMP_GREATER ;
				}
				else
				{
					// α処理が有効な場合はブレンドモードによってはα値が０の場合のみ表示されないようにする
					if( g_DefaultBlendDescArray[ NextBlendMode ].RenderTargetBlendDesc.BlendEnable == TRUE &&
						g_DefaultBlendDescArray[ NextBlendMode ].AlphaZeroNotDrawFlag == TRUE )
					{
						GD3D11.Device.State.AlphaTestEnable = TRUE ;
						AlphaTestRef     = 0 ;
						AlphaTestCmpMode = DX_CMP_GREATER ;
					}
					else
					{
						GD3D11.Device.State.AlphaTestEnable = FALSE ;
					}
				}
			}

			// 使用するピクセルシェーダーをセット
			GD3D11.Device.State.AlphaTestModeShaderIndex = Graphics_D3D11_Shader_GetAlphaTestModeIndex( GD3D11.Device.State.AlphaTestEnable, AlphaTestCmpMode ) ;
			UsePixelShader = GD3D11.Device.Shader.Base.BaseUseTex_PS[ 0 ][ g_DefaultBlendDescArray[ NextBlendMode ].RenderBlendType ][ GD3D11.Device.DrawSetting.IgnoreGraphColorFlag ? 1 : 0 ][ IgnoreTextureAlpha ][ GD3D11.Device.State.AlphaTestModeShaderIndex ] ;

			// 使用するテクスチャーアドレスをセット
			ShaderResourceView[ 0 ] = GD3D11.Device.DrawSetting.RenderTextureSRV ;

			// 使用するテクスチャの数は一つ
			ShaderResourceViewNum = 1 ;
		}
	}

	// シャドウマップの設定
	if( GSYS.DrawSetting.UseShadowMapNum > 0 )
	{
		int i ;

		for( i = 0 ; i < MAX_USE_SHADOWMAP_NUM ; i ++ )
		{
			SHADOWMAPDATA *ShadowMap ;
			if( GSYS.DrawSetting.ShadowMap[ i ] > 0 &&
				( ShadowMap = Graphics_ShadowMap_GetData( GSYS.DrawSetting.ShadowMap[ i ] ) ) != NULL )
			{
				ShaderResourceView[ 8 + i ] = ShadowMap->PF->D3D11.DepthTextureSRV ;
				Graphics_D3D11_DeviceState_SetTextureAddressUVW( D_D3D11_TEXTURE_ADDRESS_CLAMP, D_D3D11_TEXTURE_ADDRESS_CLAMP, D_D3D11_TEXTURE_ADDRESS_CLAMP, 8 + i ) ;
				Graphics_D3D11_DeviceState_SetSampleFilterMode( D_D3D11_ENCODE_BASIC_FILTER( D_D3D11_FILTER_TYPE_LINEAR, D_D3D11_FILTER_TYPE_LINEAR, D_D3D11_FILTER_TYPE_LINEAR, 0 ), 8 + i ) ;
				ShaderResourceViewNum = 9 + i ;
			}
		}
	}

	if( ShaderResourceViewNum > 0 )
	{
		Graphics_D3D11_DeviceState_SetPSShaderResouceView( 0, ShaderResourceViewNum, ShaderResourceView ) ;
	}

	if( GD3D11.Device.State.AlphaTestEnable )
	{
		if( GD3D11.Device.State.AlphaTestRef != AlphaTestRef )
		{
			Graphics_D3D11_DeviceState_SetAlphaTestRef( AlphaTestRef ) ;
		}
		if( GD3D11.Device.State.AlphaTestCmpMode != AlphaTestCmpMode )
		{
			Graphics_D3D11_DeviceState_SetAlphaTestCmpMode( AlphaTestCmpMode ) ;
		}
	}
	else
	{
		if( GD3D11.Device.State.AlphaTestRef != -1 )
		{
			Graphics_D3D11_DeviceState_SetAlphaTestRef( -1 ) ;
		}
		if( GD3D11.Device.State.AlphaTestCmpMode != DX_CMP_GREATER )
		{
			Graphics_D3D11_DeviceState_SetAlphaTestCmpMode( DX_CMP_GREATER ) ;
		}
	}

	if( UseFloatFactorColor )
	{
		Graphics_D3D11_DeviceState_SetFactorColor( &FloatFactorColor ) ;
	}

	if( GD3D11.Device.State.BlendMode != NextBlendMode )
	{
		Graphics_D3D11_DeviceState_SetBlendMode( NextBlendMode ) ;
	}

	if( GD3D11.Device.State.SetPixelShader != UsePixelShader )
	{
		Graphics_D3D11_DeviceState_SetPixelShader( UsePixelShader, TRUE ) ;
	}
	
	// 終了
	return 0 ;
}

// 現在の設定に基づいて ID3D11SamplerState、ID3D11RasterizerState、ID3D11DepthStencilState、ID3D11BlendState のセットアップと、定数バッファのアップデートを行う
__inline void Graphics_D3D11_DeviceState_SetupStateAndConstantBuffer_Inline( void )
{
	// 現在の設定に基づいて ID3D11SamplerState をセットアップする
	if( GD3D11.Device.State.ChangeSamplerDesc )
	{
		Graphics_D3D11_DeviceState_SetupSamplerState() ;
	}

	// 現在の設定に基づいて ID3D11RasterizerState をセットアップする
	if( GD3D11.Device.State.ChangeRasterizerDesc )
	{
		Graphics_D3D11_DeviceState_SetupRasterizerState() ;
	}

	// 現在の設定に基づいて ID3D11DepthStencilState をセットアップする
	if( GD3D11.Device.State.ChangeDepthStencilDesc )
	{
		Graphics_D3D11_DeviceState_SetupDepthStencilState() ;
	}

	// 現在の設定に基づいて ID3D11BlendState をセットアップする
	if( GD3D11.Device.State.ChangeBlendDesc )
	{
		Graphics_D3D11_DeviceState_SetupBlendState() ;
	}

	// 定数バッファをアップデート
	if( GD3D11.Device.Shader.Constant.ConstBuffer_Common->ChangeFlag )
	{
		Graphics_D3D11_ConstantBuffer_Update( GD3D11.Device.Shader.Constant.ConstBuffer_Common ) ;
	}
	if( GD3D11.Device.Shader.Constant.ConstBuffer_VS_Base->ChangeFlag )
	{
		Graphics_D3D11_ConstantBuffer_Update( GD3D11.Device.Shader.Constant.ConstBuffer_VS_Base ) ;
	}
	if( GD3D11.Device.Shader.Constant.ConstBuffer_VS_OtherMatrix->ChangeFlag )
	{
		Graphics_D3D11_ConstantBuffer_Update( GD3D11.Device.Shader.Constant.ConstBuffer_VS_OtherMatrix ) ;
	}
	if( GD3D11.Device.Shader.Constant.ConstBuffer_VS_LocalWorldMatrix->ChangeFlag )
	{
		Graphics_D3D11_ConstantBuffer_Update( GD3D11.Device.Shader.Constant.ConstBuffer_VS_LocalWorldMatrix ) ;
	}
	if( GD3D11.Device.Shader.Constant.ConstBuffer_PS_Base->ChangeFlag )
	{
		Graphics_D3D11_ConstantBuffer_Update( GD3D11.Device.Shader.Constant.ConstBuffer_PS_Base ) ;
	}
	if( GD3D11.Device.Shader.Constant.ConstBuffer_PS_ShadowMap->ChangeFlag )
	{
		Graphics_D3D11_ConstantBuffer_Update( GD3D11.Device.Shader.Constant.ConstBuffer_PS_ShadowMap ) ;
	}
	if( GD3D11.Device.Shader.Constant.ConstBuffer_PS_Filter->ChangeFlag )
	{
		Graphics_D3D11_ConstantBuffer_Update( GD3D11.Device.Shader.Constant.ConstBuffer_PS_Filter ) ;
	}
}

extern int Graphics_D3D11_DeviceState_SetupStateAndConstantBuffer( void )
{
	Graphics_D3D11_DeviceState_SetupStateAndConstantBuffer_Inline() ;
	return 0 ;
}






















// 描画設定関係関数

// 描画ブレンドモードの設定
extern int Graphics_D3D11_DrawSetting_SetDrawBlendMode( int BlendMode, int AlphaTestValidFlag, int AlphaChannelValidFlag )
{
	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	if( GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE &&
		GD3D11.Device.DrawSetting.BlendMode               == BlendMode &&
		GD3D11.Device.DrawSetting.AlphaTestValidFlag      == AlphaTestValidFlag &&
		GD3D11.Device.DrawSetting.AlphaChannelValidFlag   == AlphaChannelValidFlag )
	{
		return 0 ;
	}

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	GD3D11.Device.DrawSetting.BlendMode             = BlendMode ;
	GD3D11.Device.DrawSetting.AlphaChannelValidFlag = AlphaChannelValidFlag ;
	GD3D11.Device.DrawSetting.AlphaTestValidFlag    = AlphaTestValidFlag ;
	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag    = TRUE ;

	// パラメータが変更された、フラグを立てる
	GD3D11.Device.DrawSetting.ChangeBlendParamFlag = TRUE ;

	return 0 ;
}

// 描画時の画像のＲＧＢを無視するかどうかを設定する
extern int Graphics_D3D11_DrawSetting_SetIgnoreDrawGraphColor( int EnableFlag )
{
	DX_D3D11_PS_CONST_BUFFER_BASE *ConstantPSBase ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	if( GD3D11.Device.DrawSetting.IgnoreGraphColorFlag == EnableFlag &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	ConstantPSBase = ( DX_D3D11_PS_CONST_BUFFER_BASE * )GD3D11.Device.Shader.Constant.ConstBuffer_PS_Base->SysmemBuffer ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 値を保存
	GD3D11.Device.DrawSetting.IgnoreGraphColorFlag = EnableFlag ;

	// 定数データに反映
	if( GD3D11.Device.DrawSetting.IgnoreGraphColorFlag )
	{
		ConstantPSBase->IgnoreTextureColor[ 0 ] = 1.0f ;
		ConstantPSBase->IgnoreTextureColor[ 1 ] = 1.0f ;
		ConstantPSBase->IgnoreTextureColor[ 2 ] = 1.0f ;
	}
	else
	{
		ConstantPSBase->IgnoreTextureColor[ 0 ] = 0.0f ;
		ConstantPSBase->IgnoreTextureColor[ 1 ] = 0.0f ;
		ConstantPSBase->IgnoreTextureColor[ 2 ] = 0.0f ;
	}

	GD3D11.Device.Shader.Constant.ConstBuffer_PS_Base->ChangeFlag = TRUE ;

//	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// パラメータが変更された、フラグを立てる
	GD3D11.Device.DrawSetting.ChangeBlendParamFlag = TRUE ;

	return 0 ;
}

// 描画時の画像のＡを無視するかどうかを設定する
extern int Graphics_D3D11_DrawSetting_SetIgnoreDrawGraphAlpha( int EnableFlag )
{
	DX_D3D11_PS_CONST_BUFFER_BASE *ConstantPSBase ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	if( GD3D11.Device.DrawSetting.IgnoreGraphAlphaFlag == EnableFlag &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	ConstantPSBase = ( DX_D3D11_PS_CONST_BUFFER_BASE * )GD3D11.Device.Shader.Constant.ConstBuffer_PS_Base->SysmemBuffer ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 値を保存
	GD3D11.Device.DrawSetting.IgnoreGraphAlphaFlag = EnableFlag ;

	// 定数データに反映
	if( GD3D11.Device.DrawSetting.IgnoreGraphAlphaFlag )
	{
		ConstantPSBase->IgnoreTextureColor[ 3 ] = 1.0f ;
	}
	else
	{
		ConstantPSBase->IgnoreTextureColor[ 3 ] = 0.0f ;
	}

	GD3D11.Device.Shader.Constant.ConstBuffer_PS_Base->ChangeFlag = TRUE ;

	// パラメータが変更された、フラグを立てる
	GD3D11.Device.DrawSetting.ChangeBlendParamFlag = TRUE ;

	return 0 ;
}

// アルファテストの設定
extern int Graphics_D3D11_DrawSetting_SetDrawAlphaTest( int TestMode, int TestParam )
{
	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	if( GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE &&
		GD3D11.Device.DrawSetting.AlphaTestMode  == TestMode &&
		GD3D11.Device.DrawSetting.AlphaTestParam == TestParam )
	{
		return 0 ;
	}

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	GD3D11.Device.DrawSetting.AlphaTestMode      = TestMode ;
	GD3D11.Device.DrawSetting.AlphaTestParam     = TestParam ;
	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// パラメータが変更された、フラグを立てる
	GD3D11.Device.DrawSetting.ChangeBlendParamFlag = TRUE ;

	return 0 ;
}

// ブレンドするテクスチャのパラメータをセットする
extern int Graphics_D3D11_DrawSetting_SetBlendTextureParam( int BlendType, int *Param )
{
	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// パラメータを保存
	switch( BlendType )
	{
	case DX_BLENDGRAPHTYPE_NORMAL :
		if( GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE &&
			BlendType == GD3D11.Device.DrawSetting.BlendGraphType &&
			GD3D11.Device.DrawSetting.BlendGraphFadeRatio == Param[ 0 ] )
		{
			return 0 ;
		}

		if( GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
		{
			// 描画待機している描画物を描画
			DRAWSTOCKINFO
		}

		GD3D11.Device.DrawSetting.BlendGraphFadeRatio = Param[ 0 ] ;
		break;

	case DX_BLENDGRAPHTYPE_WIPE:
		if( GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE &&
			BlendType == GD3D11.Device.DrawSetting.BlendGraphType &&
			GD3D11.Device.DrawSetting.BlendGraphBorderParam == Param[ 0 ] &&
			GD3D11.Device.DrawSetting.BlendGraphBorderRange == Param[ 1 ] )
		{
			return 0 ;
		}

		if( GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
		{
			// 描画待機している描画物を描画
			DRAWSTOCKINFO
		}

		GD3D11.Device.DrawSetting.BlendGraphBorderParam = Param[ 0 ] ;
		GD3D11.Device.DrawSetting.BlendGraphBorderRange = Param[ 1 ] ;
		break;

	case DX_BLENDGRAPHTYPE_ALPHA :
		if( GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE &&
			BlendType == GD3D11.Device.DrawSetting.BlendGraphType )
		{
			return 0 ;
		}

		if( GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
		{
			// 描画待機している描画物を描画
			DRAWSTOCKINFO
		}
		break;
	}

	// ブレンドタイプを保存する
	GD3D11.Device.DrawSetting.BlendGraphType = BlendType ;

	// パラメータが変更された、フラグを立てる
	GD3D11.Device.DrawSetting.ChangeBlendParamFlag = TRUE ;
	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// 終了
	return 0 ;
}

// テクスチャをセットする 
extern int Graphics_D3D11_DrawSetting_SetTexture( D_ID3D11Texture2D *RenderTexture, D_ID3D11ShaderResourceView *RenderTextureSRV )
{
	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	if( RenderTexture == GD3D11.Device.DrawSetting.RenderTexture &&
		GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		return 0 ;
	}

	if( GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		// 描画待機している描画物を描画
		DRAWSTOCKINFO
	}

	// テクスチャが変更された、フラグを立てる
	GD3D11.Device.DrawSetting.ChangeTextureFlag = TRUE ;

	// テクスチャの有り無しが変更された場合はブレンドパラメータの変更も行うフラグもセットする
	if( ( GD3D11.Device.DrawSetting.RenderTexture == NULL && RenderTexture != NULL ) ||
		( GD3D11.Device.DrawSetting.RenderTexture != NULL && RenderTexture == NULL ) )
	{
		GD3D11.Device.DrawSetting.ChangeBlendParamFlag = TRUE ;
	}

	// テクスチャセット
	GD3D11.Device.DrawSetting.RenderTexture      = RenderTexture ;
	GD3D11.Device.DrawSetting.RenderTextureSRV   = RenderTextureSRV ;
	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// 終了
	return 0 ;
}

// ブレンドするテクスチャをセットする 
extern int Graphics_D3D11_DrawSetting_SetBlendTexture( D_ID3D11Texture2D *BlendTexture,  D_ID3D11ShaderResourceView *BlendTextureSRV, int TexWidth, int TexHeight )
{
	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	if( GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE &&
		BlendTexture == GD3D11.Device.DrawSetting.BlendTexture )
	{
		return 0 ;
	}

	if( GD3D11.Device.DrawSetting.CancelSettingEqualCheck == FALSE )
	{
		// 描画待機している描画物を描画
		DRAWSTOCKINFO
	}

	// テクスチャが変更された、フラグを立てる
	GD3D11.Device.DrawSetting.ChangeTextureFlag = TRUE ;
	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// ブレンドテクスチャの有り無しが変更された場合はブレンドパラメータの変更も行うフラグもセットする
	if( ( GD3D11.Device.DrawSetting.BlendTexture == NULL && BlendTexture != NULL ) ||
		( GD3D11.Device.DrawSetting.BlendTexture != NULL && BlendTexture == NULL ) )
	{
		GD3D11.Device.DrawSetting.ChangeBlendParamFlag = TRUE ;
	}

	// テクスチャセット
	GD3D11.Device.DrawSetting.BlendTexture    = BlendTexture ;
	GD3D11.Device.DrawSetting.BlendTextureSRV = BlendTextureSRV ;

	// テクスチャの幅と高さを保存
	if( BlendTexture != NULL )
	{
		GD3D11.Device.DrawSetting.BlendTextureWidth  = ( float )TexWidth  ;
		GD3D11.Device.DrawSetting.BlendTextureHeight = ( float )TexHeight ;
		GD3D11.Device.DrawSetting.InvBlendTextureWidth  = 1.0F / GD3D11.Device.DrawSetting.BlendTextureWidth ;
		GD3D11.Device.DrawSetting.InvBlendTextureHeight = 1.0F / GD3D11.Device.DrawSetting.BlendTextureHeight ;
	}

	// 終了
	return 0 ;
}






























// 定数情報関係関数

// Direct3D11 の定数情報の初期化を行う
extern int Graphics_D3D11_Constant_Initialize( void )
{
	DXST_LOGFILE_ADDUTF16LE( "\x04\x54\x2e\x7a\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\x28\x75\x9a\x5b\x70\x65\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\x5c\x4f\x10\x62\x2e\x00\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"各種シェーダー用定数バッファの作成.... " @*/ ) ;

	// 各種定数バッファを作成
	GD3D11.Device.Shader.Constant.ConstBuffer_Common = Graphics_D3D11_ConstantBuffer_Create( sizeof( DX_D3D11_CONST_BUFFER_COMMON ) ) ;
	if( GD3D11.Device.Shader.Constant.ConstBuffer_Common == NULL )
	{
		goto ERR ;
	}

	GD3D11.Device.Shader.Constant.ConstBuffer_VS_Base = Graphics_D3D11_ConstantBuffer_Create( sizeof( DX_D3D11_VS_CONST_BUFFER_BASE ) ) ;
	if( GD3D11.Device.Shader.Constant.ConstBuffer_VS_Base == NULL )
	{
		goto ERR ;
	}

	GD3D11.Device.Shader.Constant.ConstBuffer_VS_OtherMatrix = Graphics_D3D11_ConstantBuffer_Create( sizeof( DX_D3D11_VS_CONST_BUFFER_OTHERMATRIX ) ) ;
	if( GD3D11.Device.Shader.Constant.ConstBuffer_VS_OtherMatrix == NULL )
	{
		goto ERR ;
	}

	GD3D11.Device.Shader.Constant.ConstBuffer_VS_LocalWorldMatrix = Graphics_D3D11_ConstantBuffer_Create( sizeof( DX_D3D11_VS_CONST_BUFFER_LOCALWORLDMATRIX ) ) ;
	if( GD3D11.Device.Shader.Constant.ConstBuffer_VS_LocalWorldMatrix == NULL )
	{
		goto ERR ;
	}

	GD3D11.Device.Shader.Constant.ConstBuffer_PS_Base = Graphics_D3D11_ConstantBuffer_Create( sizeof( DX_D3D11_PS_CONST_BUFFER_BASE ) ) ;
	if( GD3D11.Device.Shader.Constant.ConstBuffer_PS_Base == NULL )
	{
		goto ERR ;
	}

	GD3D11.Device.Shader.Constant.ConstBuffer_PS_ShadowMap = Graphics_D3D11_ConstantBuffer_Create( sizeof( DX_D3D11_PS_CONST_BUFFER_SHADOWMAP ) ) ;
	if( GD3D11.Device.Shader.Constant.ConstBuffer_PS_ShadowMap == NULL )
	{
		goto ERR ;
	}

	GD3D11.Device.Shader.Constant.ConstBuffer_PS_Filter = Graphics_D3D11_ConstantBuffer_Create( DX_D3D11_PS_CONST_FILTER_SIZE ) ;
	if( GD3D11.Device.Shader.Constant.ConstBuffer_PS_Filter == NULL )
	{
		goto ERR ;
	}

	DXST_LOGFILE_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"成功\n" @*/ ) ;

	// 正常終了
	return 0 ;

ERR :
	DXST_LOGFILE_ADDUTF16LE( "\x31\x59\x57\x65\x0a\x00\x00"/*@ L"失敗\n" @*/ ) ;

	if( GD3D11.Device.Shader.Constant.ConstBuffer_Common != NULL )
	{
		Graphics_D3D11_ConstantBuffer_Delete( GD3D11.Device.Shader.Constant.ConstBuffer_Common ) ;
		GD3D11.Device.Shader.Constant.ConstBuffer_Common = NULL ;
	}

	if( GD3D11.Device.Shader.Constant.ConstBuffer_VS_Base != NULL )
	{
		Graphics_D3D11_ConstantBuffer_Delete( GD3D11.Device.Shader.Constant.ConstBuffer_VS_Base ) ;
		GD3D11.Device.Shader.Constant.ConstBuffer_VS_Base = NULL ;
	}

	if( GD3D11.Device.Shader.Constant.ConstBuffer_VS_OtherMatrix != NULL )
	{
		Graphics_D3D11_ConstantBuffer_Delete( GD3D11.Device.Shader.Constant.ConstBuffer_VS_OtherMatrix ) ;
		GD3D11.Device.Shader.Constant.ConstBuffer_VS_OtherMatrix = NULL ;
	}

	if( GD3D11.Device.Shader.Constant.ConstBuffer_VS_LocalWorldMatrix != NULL )
	{
		Graphics_D3D11_ConstantBuffer_Delete( GD3D11.Device.Shader.Constant.ConstBuffer_VS_LocalWorldMatrix ) ;
		GD3D11.Device.Shader.Constant.ConstBuffer_VS_LocalWorldMatrix = NULL ;
	}

	if( GD3D11.Device.Shader.Constant.ConstBuffer_PS_Base != NULL )
	{
		Graphics_D3D11_ConstantBuffer_Delete( GD3D11.Device.Shader.Constant.ConstBuffer_PS_Base ) ;
		GD3D11.Device.Shader.Constant.ConstBuffer_PS_Base = NULL ;
	}

	if( GD3D11.Device.Shader.Constant.ConstBuffer_PS_ShadowMap != NULL )
	{
		Graphics_D3D11_ConstantBuffer_Delete( GD3D11.Device.Shader.Constant.ConstBuffer_PS_ShadowMap ) ;
		GD3D11.Device.Shader.Constant.ConstBuffer_PS_ShadowMap = NULL ;
	}

	if( GD3D11.Device.Shader.Constant.ConstBuffer_PS_Filter != NULL )
	{
		Graphics_D3D11_ConstantBuffer_Delete( GD3D11.Device.Shader.Constant.ConstBuffer_PS_Filter ) ;
		GD3D11.Device.Shader.Constant.ConstBuffer_PS_Filter = NULL ;
	}

	// エラー終了
	return -1 ;
}

// Direct3D11 の定数情報の後始末を行う
extern int Graphics_D3D11_Constant_Terminate( void )
{
	if( GD3D11.Device.Shader.Constant.ConstBuffer_Common != NULL )
	{
		Graphics_D3D11_ConstantBuffer_Delete( GD3D11.Device.Shader.Constant.ConstBuffer_Common ) ;
		GD3D11.Device.Shader.Constant.ConstBuffer_Common = NULL ;
	}

	if( GD3D11.Device.Shader.Constant.ConstBuffer_VS_Base != NULL )
	{
		Graphics_D3D11_ConstantBuffer_Delete( GD3D11.Device.Shader.Constant.ConstBuffer_VS_Base ) ;
		GD3D11.Device.Shader.Constant.ConstBuffer_VS_Base = NULL ;
	}

	if( GD3D11.Device.Shader.Constant.ConstBuffer_VS_OtherMatrix != NULL )
	{
		Graphics_D3D11_ConstantBuffer_Delete( GD3D11.Device.Shader.Constant.ConstBuffer_VS_OtherMatrix ) ;
		GD3D11.Device.Shader.Constant.ConstBuffer_VS_OtherMatrix = NULL ;
	}

	if( GD3D11.Device.Shader.Constant.ConstBuffer_VS_LocalWorldMatrix != NULL )
	{
		Graphics_D3D11_ConstantBuffer_Delete( GD3D11.Device.Shader.Constant.ConstBuffer_VS_LocalWorldMatrix ) ;
		GD3D11.Device.Shader.Constant.ConstBuffer_VS_LocalWorldMatrix = NULL ;
	}

	if( GD3D11.Device.Shader.Constant.ConstBuffer_PS_Base != NULL )
	{
		Graphics_D3D11_ConstantBuffer_Delete( GD3D11.Device.Shader.Constant.ConstBuffer_PS_Base ) ;
		GD3D11.Device.Shader.Constant.ConstBuffer_PS_Base = NULL ;
	}

	if( GD3D11.Device.Shader.Constant.ConstBuffer_PS_ShadowMap != NULL )
	{
		Graphics_D3D11_ConstantBuffer_Delete( GD3D11.Device.Shader.Constant.ConstBuffer_PS_ShadowMap ) ;
		GD3D11.Device.Shader.Constant.ConstBuffer_PS_ShadowMap = NULL ;
	}

	if( GD3D11.Device.Shader.Constant.ConstBuffer_PS_Filter != NULL )
	{
		Graphics_D3D11_ConstantBuffer_Delete( GD3D11.Device.Shader.Constant.ConstBuffer_PS_Filter ) ;
		GD3D11.Device.Shader.Constant.ConstBuffer_PS_Filter = NULL ;
	}

	// 正常終了
	return 0 ;
}

































// 定数バッファ関係関数


// 定数バッファを作成する
extern	CONSTANTBUFFER_DIRECT3D11 *Graphics_D3D11_ConstantBuffer_Create( DWORD Size, int ASyncThread )
{
	CONSTANTBUFFER_DIRECT3D11	*NewBuffer = NULL ;
	DWORD						AllocSize ;
	D_D3D11_BUFFER_DESC			BufferDesc ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SHADER_CONSTANT_BUFFER ].CriticalSection ) ;

	// サイズチェック
	if( Size % 16 != 0 )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x9a\x5b\x70\x65\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\xb5\x30\xa4\x30\xba\x30\x6f\x30\x11\xff\x16\xff\x6e\x30\x0d\x50\x70\x65\x67\x30\x42\x30\x8b\x30\xc5\x5f\x81\x89\x4c\x30\x42\x30\x8a\x30\x7e\x30\x59\x30\x0a\x00\x00"/*@ L"定数バッファのサイズは１６の倍数である必要があります\n" @*/ )) ;
		goto ERR ;
	}

	// メモリの確保
	AllocSize = Size + sizeof( CONSTANTBUFFER_DIRECT3D11 ) ;
	NewBuffer = ( CONSTANTBUFFER_DIRECT3D11 * )DXALLOC( AllocSize ) ;
	if( NewBuffer == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x9a\x5b\x70\x65\xd0\x30\xc3\x30\xd5\x30\xa1\x30\xc5\x60\x31\x58\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"定数バッファ情報を格納するメモリ領域の確保に失敗しました\n" @*/ )) ;
		goto ERR ;
	}

	// 零初期化
	_MEMSET( NewBuffer, 0, AllocSize ) ;

	// ID3D11Buffer の作成
	_MEMSET( &BufferDesc, 0, sizeof( BufferDesc ) ) ;
	BufferDesc.ByteWidth      = Size ;
	BufferDesc.Usage          = D_D3D11_USAGE_DEFAULT ;
	BufferDesc.BindFlags      = D_D3D11_BIND_CONSTANT_BUFFER ;
	BufferDesc.CPUAccessFlags = 0 ;
	if( D3D11Device_CreateBuffer_ASync( &BufferDesc, NULL, &NewBuffer->ConstantBuffer, ASyncThread ) != S_OK )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x9a\x5b\x70\x65\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x28\x75\x6e\x30\x20\x00\x49\x00\x44\x00\x33\x00\x44\x00\x31\x00\x31\x00\x42\x00\x75\x00\x66\x00\x66\x00\x65\x00\x72\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"定数バッファ用の ID3D11Buffer の作成に失敗しました\n" @*/ )) ;
		goto ERR ;
	}

	// 情報をセット
	NewBuffer->Size         = Size ;
	NewBuffer->SysmemBuffer = NewBuffer + 1 ;
	NewBuffer->ChangeFlag   = FALSE ;

	// リストに追加
	if( GD3D11.Device.Shader.Constant.ConstantBufferManage.FirstBuffer != NULL )
	{
		NewBuffer->NextData = GD3D11.Device.Shader.Constant.ConstantBufferManage.FirstBuffer ;
		GD3D11.Device.Shader.Constant.ConstantBufferManage.FirstBuffer->PrevData = NewBuffer ;
	}
	GD3D11.Device.Shader.Constant.ConstantBufferManage.FirstBuffer = NewBuffer ;

	// バッファの数を増やす
	GD3D11.Device.Shader.Constant.ConstantBufferManage.BufferNum ++ ;

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SHADER_CONSTANT_BUFFER ].CriticalSection ) ;

	// 正常終了
	return NewBuffer ;


	// エラー処理
ERR :
	if( NewBuffer != NULL )
	{
		if( NewBuffer->ConstantBuffer != NULL )
		{
			Direct3D11_Release_Buffer( NewBuffer->ConstantBuffer ) ;
			NewBuffer->ConstantBuffer = NULL ;
		}

		DXFREE( NewBuffer ) ;
	}

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SHADER_CONSTANT_BUFFER ].CriticalSection ) ;

	// エラー終了
	return NULL ;
}

// 定数バッファを削除する
extern int Graphics_D3D11_ConstantBuffer_Delete( CONSTANTBUFFER_DIRECT3D11 *ConstantBuffer )
{
	int i ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SHADER_CONSTANT_BUFFER ].CriticalSection ) ;

	// デバイスで使用されていたら外す
	for( i = 0 ; i < D_D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT ; i ++ )
	{
		if( GD3D11.Device.State.SetPixelShaderConstantBuffer[ i ] == ConstantBuffer->ConstantBuffer )
		{
			GD3D11.Device.State.SetPixelShaderConstantBuffer[ i ] = NULL ;
		}

		if( GD3D11.Device.State.SetVertexShaderConstantBuffer[ i ] == ConstantBuffer->ConstantBuffer )
		{
			GD3D11.Device.State.SetVertexShaderConstantBuffer[ i ] = NULL ;
		}
	}

	// ID3D11Buffer を解放
	if( ConstantBuffer->ConstantBuffer != NULL )
	{
		Direct3D11_Release_Buffer( ConstantBuffer->ConstantBuffer ) ;
		ConstantBuffer->ConstantBuffer = NULL ;
	}

	// リストから外す
	if( GD3D11.Device.Shader.Constant.ConstantBufferManage.FirstBuffer == ConstantBuffer )
	{
		GD3D11.Device.Shader.Constant.ConstantBufferManage.FirstBuffer = ConstantBuffer->NextData ;
		if( ConstantBuffer->NextData != NULL )
		{
			ConstantBuffer->NextData->PrevData = NULL ;
		}
	}
	else
	{
		ConstantBuffer->PrevData->NextData = ConstantBuffer->NextData ;
		if( ConstantBuffer->NextData != NULL )
		{
			ConstantBuffer->NextData->PrevData = ConstantBuffer->PrevData ;
		}
	}

	// メモリの解放
	DXFREE( ConstantBuffer ) ;

	// バッファの数を減らす
	GD3D11.Device.Shader.Constant.ConstantBufferManage.BufferNum -- ;

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SHADER_CONSTANT_BUFFER ].CriticalSection ) ;

	// 終了
	return 0 ;
}

// システムメモリ上のバッファに行った変更を適用する
extern int Graphics_D3D11_ConstantBuffer_Update( CONSTANTBUFFER_DIRECT3D11 *ConstantBuffer )
{
	// 変更が無い場合は何もせずに終了
	if( ConstantBuffer->ChangeFlag == FALSE )
	{
		return 0 ;
	}

	// 更新
	D3D11DeviceContext_UpdateSubresource( ConstantBuffer->ConstantBuffer, 0, NULL, ConstantBuffer->SysmemBuffer, 0, 0 ) ;

	// 変更があるフラグを倒す
	ConstantBuffer->ChangeFlag = FALSE ;

	// 終了
	return 0 ;
}

// 指定の定数バッファを頂点シェーダーに設定する
extern int Graphics_D3D11_ConstantBuffer_VSSet( UINT StartSlot, UINT NumBuffers, CONSTANTBUFFER_DIRECT3D11 * const *pConstantBuffers )
{
	D_ID3D11Buffer *D3D11ConstantBuffer[ D_D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT ] ;
	UINT                 i ;

	if( StartSlot < 0 || StartSlot + NumBuffers > D_D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT )
	{
		return -1 ;
	}

	if( NumBuffers == 1 )
	{
		return Graphics_D3D11_DeviceState_SetVertexShaderConstantBuffers( StartSlot, NumBuffers, &pConstantBuffers[ 0 ]->ConstantBuffer ) ;
	}
	
	for( i = 0 ; i < NumBuffers ; i ++ )
	{
		D3D11ConstantBuffer[ i ] = pConstantBuffers[ i ]->ConstantBuffer ;
	}

	return Graphics_D3D11_DeviceState_SetVertexShaderConstantBuffers( StartSlot, NumBuffers, D3D11ConstantBuffer ) ;
}

// 指定の定数バッファをピクセルシェーダーに設定する
extern int Graphics_D3D11_ConstantBuffer_PSSet( UINT StartSlot, UINT NumBuffers, CONSTANTBUFFER_DIRECT3D11 * const *pConstantBuffers )
{
	D_ID3D11Buffer *D3D11ConstantBuffer[ D_D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT ] ;
	UINT                 i ;

	if( StartSlot < 0 || StartSlot + NumBuffers > D_D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT )
	{
		return -1 ;
	}

	if( NumBuffers == 1 )
	{
		return Graphics_D3D11_DeviceState_SetPixelShaderConstantBuffers( StartSlot, NumBuffers, &pConstantBuffers[ 0 ]->ConstantBuffer ) ;
	}
	
	for( i = 0 ; i < NumBuffers ; i ++ )
	{
		D3D11ConstantBuffer[ i ] = pConstantBuffers[ i ]->ConstantBuffer ;
	}

	return Graphics_D3D11_DeviceState_SetPixelShaderConstantBuffers( StartSlot, NumBuffers, D3D11ConstantBuffer ) ;
}
































// 入力レイアウト関係関数

static D_D3D11_INPUT_ELEMENT_DESC MakeD3D11InputElement( LPCSTR SemanticName, UINT SemanticIndex, D_DXGI_FORMAT Format, UINT InputSlot, UINT AlignedByteOffset )
{
	D_D3D11_INPUT_ELEMENT_DESC Result = 
	{
		SemanticName,
		SemanticIndex,
		Format,
		InputSlot,
		AlignedByteOffset,
		D_D3D11_INPUT_PER_VERTEX_DATA,
		0
	} ;
	return Result ;
}

// 標準描画用の ID3D11InputLayout オブジェクトの作成( 0:成功  -1:失敗 )
extern int Graphics_D3D11_InputLayout_Base_Create( void )
{
	int							i ;
	int							j ;
	int							ElementNum ;
	HRESULT						hr ;
	GRAPHICS_HARDWARE_DIRECT3D11_BASE_SIMPLE_VERTEXSHADER_INFO *BSVI ;
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO                *Code ;

	// 既に作成されていたときのために削除処理を行う
	Graphics_D3D11_InputLayout_Base_Terminate() ;

	DXST_LOGFILE_ADDUTF16LE( "\x04\x54\x2e\x7a\x20\x00\x49\x00\x44\x00\x33\x00\x44\x00\x31\x00\x31\x00\x49\x00\x6e\x00\x70\x00\x75\x00\x74\x00\x4c\x00\x61\x00\x79\x00\x6f\x00\x75\x00\x74\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x2e\x00\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"各種 ID3D11InputLayout の作成.... " @*/ ) ;

	// 標準描画用のシェーダーで使用する頂点データ形式を作成
	BSVI = g_BaseSimpleVertexShaderInfo ;
	Code = GraphicsHardDataDirect3D11.ShaderCode.Base.BaseSimple_VS_Code ;
	for( i = 0 ; i < D3D11_VERTEX_INPUTLAYOUT_NUM ; i++, BSVI++, Code++ )
	{
		// 頂点に含まれる情報の数を数える
		for( ElementNum = 0 ; BSVI->InputElementDesc[ ElementNum ].SemanticName != NULL ; ElementNum ++ ){}

		// BGRAカラーを使用する場合は頂点カラーフォーマットを変更する
		if( GSYS.HardInfo.UseVertexColorBGRAFormat )
		{
			for( j = 0 ; j < ElementNum ; j ++ )
			{
				if( BSVI->InputElementDesc[ j ].Format == D_DXGI_FORMAT_R8G8B8A8_UNORM )
				{
					BSVI->InputElementDesc[ j ].Format = D_DXGI_FORMAT_B8G8R8A8_UNORM ;
				}
			}
		}

		// 頂点データフォーマットを作成
		hr = D3D11Device_CreateInputLayout( BSVI->InputElementDesc, ( UINT )ElementNum, Code->Binary, ( SIZE_T )Code->Size, &GD3D11.Device.InputLayout.BaseInputLayout[ i ] ) ;
		if( hr != S_OK )
		{
			DXST_LOGFILE_ADDUTF16LE( "\x19\x6a\x96\x6e\xcf\x63\x3b\x75\x28\x75\x6e\x30\x20\x00\x49\x00\x44\x00\x33\x00\x44\x00\x31\x00\x31\x00\x49\x00\x6e\x00\x70\x00\x75\x00\x74\x00\x4c\x00\x61\x00\x79\x00\x6f\x00\x75\x00\x74\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x0a\x00\x00"/*@ L"標準描画用の ID3D11InputLayout の作成に失敗\n" @*/ ) ;
			return -1 ;
		}
	}

	// 単純転送用の頂点データフォーマットを作成
	{
		static D_D3D11_INPUT_ELEMENT_DESC StretchRectInputElementDesc[] =
		{
			{ "POSITION",     0, D_DXGI_FORMAT_R32G32_FLOAT, 0,  0, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",     0, D_DXGI_FORMAT_R32G32_FLOAT, 0,  8, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",     1, D_DXGI_FORMAT_R32G32_FLOAT, 0, 16, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
		} ;

		static D_D3D11_INPUT_ELEMENT_DESC StretchRectTex8InputElementDesc[] =
		{
			{ "POSITION",     0, D_DXGI_FORMAT_R32G32_FLOAT, 0,  0, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",     0, D_DXGI_FORMAT_R32G32_FLOAT, 0,  8, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",     1, D_DXGI_FORMAT_R32G32_FLOAT, 0, 16, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",     2, D_DXGI_FORMAT_R32G32_FLOAT, 0, 24, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",     3, D_DXGI_FORMAT_R32G32_FLOAT, 0, 32, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",     4, D_DXGI_FORMAT_R32G32_FLOAT, 0, 40, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",     5, D_DXGI_FORMAT_R32G32_FLOAT, 0, 48, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",     6, D_DXGI_FORMAT_R32G32_FLOAT, 0, 56, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",     7, D_DXGI_FORMAT_R32G32_FLOAT, 0, 64, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
		} ;

		hr = D3D11Device_CreateInputLayout(
			StretchRectInputElementDesc,
			sizeof( StretchRectInputElementDesc ) / sizeof( D_D3D11_INPUT_ELEMENT_DESC ),
			GD3D11.ShaderCode.Base.StretchRect_VS_Code.Binary,
			( SIZE_T )GD3D11.ShaderCode.Base.StretchRect_VS_Code.Size,
			&GD3D11.Device.InputLayout.BaseStretchRectInputLayout ) ;
		if( hr != S_OK )
		{
			DXST_LOGFILE_ADDUTF16LE( "\x58\x53\x14\x7d\xe2\x8e\x01\x90\x28\x75\x6e\x30\x20\x00\x49\x00\x44\x00\x33\x00\x44\x00\x31\x00\x31\x00\x49\x00\x6e\x00\x70\x00\x75\x00\x74\x00\x4c\x00\x61\x00\x79\x00\x6f\x00\x75\x00\x74\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x0a\x00\x00"/*@ L"単純転送用の ID3D11InputLayout の作成に失敗\n" @*/ ) ;
			return -1 ;
		}

		hr = D3D11Device_CreateInputLayout(
			StretchRectTex8InputElementDesc,
			sizeof( StretchRectTex8InputElementDesc ) / sizeof( D_D3D11_INPUT_ELEMENT_DESC ),
			GD3D11.ShaderCode.Base.StretchRectTex8_VS_Code.Binary,
			( SIZE_T )GD3D11.ShaderCode.Base.StretchRectTex8_VS_Code.Size,
			&GD3D11.Device.InputLayout.BaseStretchRectTex8InputLayout ) ;
		if( hr != S_OK )
		{
			DXST_LOGFILE_ADDUTF16LE( "\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\xa7\x5e\x19\x6a\x38\x00\x0b\x50\x6e\x30\x58\x53\x14\x7d\xe2\x8e\x01\x90\x28\x75\x6e\x30\x20\x00\x49\x00\x44\x00\x33\x00\x44\x00\x31\x00\x31\x00\x49\x00\x6e\x00\x70\x00\x75\x00\x74\x00\x4c\x00\x61\x00\x79\x00\x6f\x00\x75\x00\x74\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x0a\x00\x00"/*@ L"テクスチャ座標8個の単純転送用の ID3D11InputLayout の作成に失敗\n" @*/ ) ;
			return -1 ;
		}
	}

	DXST_LOGFILE_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"成功\n" @*/ ) ;

	// 正常終了
	return 0 ;
}

// 標準描画用の ID3D11InputLayout オブジェクトの削除( 0:成功  -1:失敗 )
extern int Graphics_D3D11_InputLayout_Base_Terminate( void )
{
	int							i ;
	GRAPHICS_HARDWARE_DIRECT3D11_BASE_SIMPLE_VERTEXSHADER_INFO *BSVI ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// 標準描画用のシェーダーで使用する頂点データ形式を解放
	BSVI = g_BaseSimpleVertexShaderInfo ;
	for( i = 0 ; i < D3D11_VERTEX_INPUTLAYOUT_NUM ; i++, BSVI++ )
	{
		if( GD3D11.Device.InputLayout.BaseInputLayout[ i ] != NULL )
		{
			Direct3D11_Release_InputLayout( GD3D11.Device.InputLayout.BaseInputLayout[ i ] ) ;
			GD3D11.Device.InputLayout.BaseInputLayout[ i ] = NULL ;
		}
	}

	// 単純転送用の頂点データフォーマットを解放
	if( GD3D11.Device.InputLayout.BaseStretchRectInputLayout != NULL )
	{
		Direct3D11_Release_InputLayout( GD3D11.Device.InputLayout.BaseStretchRectInputLayout ) ;
		GD3D11.Device.InputLayout.BaseStretchRectInputLayout = NULL ;
	}
	if( GD3D11.Device.InputLayout.BaseStretchRectTex8InputLayout != NULL )
	{
		Direct3D11_Release_InputLayout( GD3D11.Device.InputLayout.BaseStretchRectTex8InputLayout ) ;
		GD3D11.Device.InputLayout.BaseStretchRectTex8InputLayout = NULL ;
	}

	// 正常終了
	return 0 ;
}

#ifndef DX_NON_MODEL

// モデル描画用の ID3D11InputLayout オブジェクトの作成( 0:成功  -1:失敗 )
extern int Graphics_D3D11_InputLayout_Model_Create( int BumpMap, int MeshType )
{
	DWORD offset, index ;
	HRESULT hr ;
	D_ID3D11VertexShader **VS = NULL ;
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO *VSAddress = NULL ;
	D_D3D11_INPUT_ELEMENT_DESC InputElement[ 64 ] = 
	{
		{ "POSITION",     0, D_DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",       0, D_DXGI_FORMAT_R32G32B32_FLOAT,    0, 12, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",        0, D_DXGI_FORMAT_R8G8B8A8_UNORM,     0, 24, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",        1, D_DXGI_FORMAT_R8G8B8A8_UNORM,     0, 28, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",     0, D_DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",     1, D_DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D_D3D11_INPUT_PER_VERTEX_DATA, 0 },
	} ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// 既にあったら何もせず終了
	if( GD3D11.Device.InputLayout.MV1_VertexInputLayout[ BumpMap ][ MeshType ] != NULL )
	{
		return 0 ;
	}

	index  = 6 ;
	offset = 64 ;
	if( BumpMap )
	{
		InputElement[ index ] = MakeD3D11InputElement( "TANGENT",      0, D_DXGI_FORMAT_R32G32B32_FLOAT,    0, offset ) ;
		index++ ;
		offset += sizeof( VECTOR ) ;

		InputElement[ index ] = MakeD3D11InputElement( "BINORMAL",     0, D_DXGI_FORMAT_R32G32B32_FLOAT,    0, offset ) ;
		index++ ;
		offset += sizeof( VECTOR ) ;
	}

	switch( MeshType )
	{
	case 0 :	// 剛体メッシュ
		break ;

	case 1 :	// ４ボーン以内スキニングメッシュ
	case 2 :	// ８ボーン以内スキニングメッシュ
		InputElement[ index ] = MakeD3D11InputElement( "BLENDINDICES", 0, D_DXGI_FORMAT_R8G8B8A8_UINT,      0, offset ) ;
		index++ ;
		offset += sizeof( BYTE ) * 4 ;

		InputElement[ index ] = MakeD3D11InputElement( "BLENDWEIGHT",  0, D_DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offset ) ;
		index++ ;
		offset += sizeof( FLOAT4 ) ;

		if( MeshType == 2 )
		{
			InputElement[ index ] = MakeD3D11InputElement( "BLENDINDICES", 1, D_DXGI_FORMAT_R8G8B8A8_UINT,      0, offset ) ;
			index++ ;
			offset += sizeof( BYTE ) * 4 ;

			InputElement[ index ] = MakeD3D11InputElement( "BLENDWEIGHT",  1, D_DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offset ) ;
			index++ ;
			offset += sizeof( FLOAT4 ) ;
		}
		break ;
	}

	VS        = &GD3D11.Device.Shader.Model.MV1_VertexLighting_VS     [ 0 ][ MeshType ][ BumpMap ][ 0 ][ 0 ] ;
	VSAddress =    &GD3D11.ShaderCode.Model.MV1_VertexLighting_VS_Code[ 0 ][ MeshType ][ BumpMap ][ 0 ][ 0 ] ;

	// シェーダーがあるかを調べる
	if( *VS == NULL )
	{
		// シェーダーの作成を試みる
		if( Graphics_D3D11_VertexShader_Create( VSAddress, VS, 1 ) != 0 )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x47\x00\x72\x00\x61\x00\x70\x00\x68\x00\x69\x00\x63\x00\x73\x00\x5f\x00\x44\x00\x33\x00\x44\x00\x31\x00\x31\x00\x5f\x00\x49\x00\x6e\x00\x70\x00\x75\x00\x74\x00\x4c\x00\x61\x00\x79\x00\x6f\x00\x75\x00\x74\x00\x5f\x00\x4d\x00\x6f\x00\x64\x00\x65\x00\x6c\x00\x5f\x00\x43\x00\x72\x00\x65\x00\x61\x00\x74\x00\x65\x00\x20\x00\x67\x30\x02\x98\xb9\x70\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Graphics_D3D11_InputLayout_Model_Create で頂点シェーダーの作成に失敗しました\n" @*/ )) ;
			return -1 ;
		}
	}

	hr = D3D11Device_CreateInputLayout( InputElement, index, VSAddress->Binary, ( SIZE_T )VSAddress->Size, &GD3D11.Device.InputLayout.MV1_VertexInputLayout[ BumpMap ][ MeshType ] ) ;
	if( hr != S_OK )
	{
		return -1 ;
	}

	// 正常終了
	return 0 ;
}

// モデル描画用の ID3D11InputLayout オブジェクトの削除( 0:成功  -1:失敗 )
extern int Graphics_D3D11_InputLayout_Model_Terminate( void )
{
	int i ;
	int j ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	for( i = 0; i < 2; i ++ )
	{
		for( j = 0; j < 3; j ++ )
		{
			if( GD3D11.Device.InputLayout.MV1_VertexInputLayout[ i ][ j ] != NULL )
			{
				Direct3D11_Release_InputLayout( GD3D11.Device.InputLayout.MV1_VertexInputLayout[ i ][ j ] ) ;
				GD3D11.Device.InputLayout.MV1_VertexInputLayout[ i ][ j ] = NULL ;
			}
		}
	}

	return 0 ;
}

#endif // DX_NON_MODEL














// 標準描画関係

// 標準描画用の頂点バッファの作成( 0:成功  -1:失敗 )
extern	int		Graphics_D3D11_NormalDrawVertexBuffer_Create( void )
{
	D_D3D11_BUFFER_DESC BufferDesc ;
	HRESULT             hr ;

	DXST_LOGFILE_ADDUTF16LE( "\x19\x6a\x96\x6e\xcf\x63\x3b\x75\x28\x75\x6e\x30\x02\x98\xb9\x70\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\x5c\x4f\x10\x62\x2e\x00\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"標準描画用の頂点バッファの作成.... " @*/ ) ;

	_MEMSET( &BufferDesc, 0, sizeof( BufferDesc ) ) ;
	BufferDesc.ByteWidth      = D3D11_VERTEXBUFFER_SIZE ;
	BufferDesc.BindFlags      = D_D3D11_BIND_VERTEX_BUFFER ;
#if VERTEXBUFFERMODE != 0
	BufferDesc.Usage          = D_D3D11_USAGE_DYNAMIC ;
	BufferDesc.CPUAccessFlags = D_D3D11_CPU_ACCESS_WRITE ;
#else
	BufferDesc.Usage          = D_D3D11_USAGE_DEFAULT ;
	BufferDesc.CPUAccessFlags = 0 ;
#endif
	hr = D3D11Device_CreateBuffer( &BufferDesc, NULL, &GD3D11.Device.DrawInfo.VertexBuffer ) ;
	if( FAILED( hr ) )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x31\x59\x57\x65\x0a\x00\x00"/*@ L"失敗\n" @*/ ) ;
		return -1 ;
	}

	DXST_LOGFILE_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"成功\n" @*/ ) ;

	// 正常終了
	return 0 ;
}

// 標準描画用の頂点バッファの削除( 0:成功  -1:失敗 )
extern	int		Graphics_D3D11_NormalDrawVertexBuffer_Terminate( void )
{
	if( GD3D11.Device.DrawInfo.VertexBuffer )
	{
//		NORMALVERTEXBUFFER_UNMAP
		Direct3D11_Release_Buffer( GD3D11.Device.DrawInfo.VertexBuffer ) ;
		GD3D11.Device.DrawInfo.VertexBuffer = NULL ;
		GD3D11.Device.DrawInfo.VertexBufferAddr = NULL ;
		GD3D11.Device.DrawInfo.VertexBufferNextAddr = NULL ;
	}

	// 正常終了
	return 0 ;
}

// 汎用描画用頂点バッファを使用してシステムメモリ上の頂点データの描画を行う
extern int Graphics_D3D11_CommonBuffer_DrawPrimitive(
	int                        InputLayout,
	D_D3D11_PRIMITIVE_TOPOLOGY PrimitiveType,
	const void *               VertexData,
	int                        VertexNum,
	int                        UseDefaultVertexShader
)
{
	int         VertexSize ;
//	D_D3D11_BOX DestBox ;

	VertexSize = D3D11_VertexSize[ InputLayout ] ;

	Graphics_D3D11_CommonVertexBuffer_Setup( VertexSize * VertexNum ) ;

//	DestBox.left   = 0 ;
//	DestBox.top    = 0 ;
//	DestBox.front  = 0 ;
//	DestBox.bottom = 1 ;
//	DestBox.back   = 1 ;
//	DestBox.right  = ( UINT )( VertexSize * VertexNum ) ;
//	D3D11DeviceContext_UpdateSubresource_ASync( GD3D11.Device.DrawInfo.CommonVertexBuffer, 0, &DestBox, VertexData, 0, 0 ) ;
	D_D3D11_MAPPED_SUBRESOURCE MappedSubResource ;
	HRESULT                    hr ;
	hr = D3D11DeviceContext_Map_ASync( 
		GD3D11.Device.DrawInfo.CommonVertexBuffer,
		0,
		D_D3D11_MAP_WRITE_DISCARD,
		0,
		&MappedSubResource
	) ;
	_MEMCPY( MappedSubResource.pData, VertexData, VertexSize * VertexNum ) ;
	D3D11DeviceContext_Unmap_ASync(
		GD3D11.Device.DrawInfo.CommonVertexBuffer,
		0
	) ;

	// プリミティブタイプをセット
	Graphics_D3D11_DeviceState_SetPrimitiveTopology( PrimitiveType ) ;

	// 入力レイアウトをセット
	Graphics_D3D11_DeviceState_SetInputLayout( GD3D11.Device.InputLayout.BaseInputLayout[ InputLayout ] ) ;

	// 頂点シェーダーをセット
	if( UseDefaultVertexShader )
	{
		if( InputLayout == D3D11_VERTEX_INPUTLAYOUT_3D_LIGHT )
		{
			if( Graphics_D3D11_Shader_Normal3DDraw_Setup() == FALSE )
			{
				return -1 ;
			}
		}
		else
		{
			Graphics_D3D11_DeviceState_SetVertexShader( GD3D11.Device.Shader.Base.BaseSimple_VS[ InputLayout ], TRUE ) ;
		}
	}

	// 頂点バッファをセット
	Graphics_D3D11_DeviceState_SetVertexBuffer( GD3D11.Device.DrawInfo.CommonVertexBuffer, D3D11_VertexSize[ InputLayout ] ) ;

	// ステートと定数バッファの更新
	Graphics_D3D11_DeviceState_SetupStateAndConstantBuffer() ;

	// 描画
	D3D11DeviceContext_Draw( ( UINT )VertexNum, 0 ) ;
	GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;

	// 終了
	return 0 ;
}

// 汎用描画用頂点バッファ・インデックスバッファを使用してシステムメモリ上の頂点データ・インデックスデータの描画を行う
extern int Graphics_D3D11_CommonBuffer_DrawIndexedPrimitive(
	int							InputLayout,
	D_D3D11_PRIMITIVE_TOPOLOGY	PrimitiveType,
	const void *				VertexData,
	int							VertexNum,
	const void *				IndexData,
	int							IndexNum,
	D_DXGI_FORMAT				IndexFormat,
	int                         UseDefaultVertexShader
)
{
	int         VertexSize ;
	int         IndexSize ;
//	D_D3D11_BOX DestBox ;

	VertexSize = D3D11_VertexSize[ InputLayout ] ;
	IndexSize = 0 ;
	switch( IndexFormat )
	{
	case D_DXGI_FORMAT_R16_UINT :
		IndexSize = 2 ;
		break ;

	case D_DXGI_FORMAT_R32_UINT :
		IndexSize = 4 ;
		break ;
	}

	Graphics_D3D11_CommonVertexBuffer_Setup( VertexSize * VertexNum ) ;
	Graphics_D3D11_CommonIndexBuffer_Setup(  IndexSize  * IndexNum ) ;

//	DestBox.left   = 0 ;
//	DestBox.top    = 0 ;
//	DestBox.front  = 0 ;
//	DestBox.bottom = 1 ;
//	DestBox.back   = 1 ;
//	DestBox.right  = ( UINT )( VertexSize * VertexNum ) ;
//	D3D11DeviceContext_UpdateSubresource_ASync( GD3D11.Device.DrawInfo.CommonVertexBuffer, 0, &DestBox, VertexData, 0, 0 ) ;
	D_D3D11_MAPPED_SUBRESOURCE MappedSubResource ;
	HRESULT                    hr ;
	hr = D3D11DeviceContext_Map_ASync( 
		GD3D11.Device.DrawInfo.CommonVertexBuffer,
		0,
		D_D3D11_MAP_WRITE_DISCARD,
		0,
		&MappedSubResource
	) ;
	_MEMCPY( MappedSubResource.pData, VertexData, VertexSize * VertexNum ) ;
	D3D11DeviceContext_Unmap_ASync(
		GD3D11.Device.DrawInfo.CommonVertexBuffer,
		0
	) ;

//	DestBox.right  = ( UINT )( IndexSize  * IndexNum ) ;
//	D3D11DeviceContext_UpdateSubresource_ASync( GD3D11.Device.DrawInfo.CommonIndexBuffer,  0, &DestBox, IndexData,  0, 0 ) ;
	hr = D3D11DeviceContext_Map_ASync( 
		GD3D11.Device.DrawInfo.CommonIndexBuffer,
		0,
		D_D3D11_MAP_WRITE_DISCARD,
		0,
		&MappedSubResource
	) ;
	_MEMCPY( MappedSubResource.pData, IndexData, IndexSize * IndexNum ) ;
	D3D11DeviceContext_Unmap_ASync(
		GD3D11.Device.DrawInfo.CommonIndexBuffer,
		0
	) ;

	// プリミティブタイプをセット
	Graphics_D3D11_DeviceState_SetPrimitiveTopology( PrimitiveType ) ;

	// 入力レイアウトをセット
	Graphics_D3D11_DeviceState_SetInputLayout( GD3D11.Device.InputLayout.BaseInputLayout[ InputLayout ] ) ;

	// 頂点シェーダーをセット
	if( UseDefaultVertexShader )
	{
		if( InputLayout == D3D11_VERTEX_INPUTLAYOUT_3D_LIGHT )
		{
			if( Graphics_D3D11_Shader_Normal3DDraw_Setup() == FALSE )
			{
				return -1 ;
			}
		}
		else
		{
			Graphics_D3D11_DeviceState_SetVertexShader( GD3D11.Device.Shader.Base.BaseSimple_VS[ InputLayout ], TRUE ) ;
		}
	}

	// 頂点バッファをセット
	Graphics_D3D11_DeviceState_SetVertexBuffer( GD3D11.Device.DrawInfo.CommonVertexBuffer, D3D11_VertexSize[ InputLayout ] ) ;

	// インデックスバッファをセット
	Graphics_D3D11_DeviceState_SetIndexBuffer( GD3D11.Device.DrawInfo.CommonIndexBuffer, IndexFormat ) ;

	// ステートと定数バッファの更新
	Graphics_D3D11_DeviceState_SetupStateAndConstantBuffer() ;

	// 描画
	D3D11DeviceContext_DrawIndexed( ( UINT )IndexNum, 0, 0 ) ;
	GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;

	// 終了
	return 0 ;
}

// 指定サイズの汎用描画用頂点バッファの準備を行う
extern int Graphics_D3D11_CommonVertexBuffer_Setup( int Size )
{
	D_D3D11_BUFFER_DESC BufferDesc ;
	HRESULT             hr ;

	if( Size <= GD3D11.Device.DrawInfo.CommonVertexBufferSize )
	{
		return 0 ;
	}

	Graphics_D3D11_CommonVertexBuffer_Terminate() ;

	Size += D3D11_COMMONBUFFER_ADDSIZE ;
	Size = ( Size + 15 ) / 16 * 16 ;

	_MEMSET( &BufferDesc, 0, sizeof( BufferDesc ) ) ;
	BufferDesc.ByteWidth      = ( UINT )Size ;
	BufferDesc.Usage          = D_D3D11_USAGE_DYNAMIC ;
//	BufferDesc.Usage          = D_D3D11_USAGE_DEFAULT ;
	BufferDesc.BindFlags      = D_D3D11_BIND_VERTEX_BUFFER ;
	BufferDesc.CPUAccessFlags = D_D3D11_CPU_ACCESS_WRITE ;
//	BufferDesc.CPUAccessFlags = 0 ;
	hr = D3D11Device_CreateBuffer( &BufferDesc, NULL, &GD3D11.Device.DrawInfo.CommonVertexBuffer ) ;
	if( FAILED( hr ) )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x4e\x6c\x28\x75\xcf\x63\x3b\x75\x28\x75\x6e\x30\x02\x98\xb9\x70\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x28\x00\x20\x00\xb5\x30\xa4\x30\xba\x30\x20\x00\x25\x00\x64\x00\x20\x00\x62\x00\x79\x00\x74\x00\x65\x00\x20\x00\x29\x00\x0a\x00\x00"/*@ L"汎用描画用の頂点バッファの作成に失敗しました( サイズ %d byte )\n" @*/, Size ) ) ;
		return -1 ;
	}

	GD3D11.Device.DrawInfo.CommonVertexBufferSize = Size ;

	// 正常終了
	return 0 ;
}

// 指定サイズの汎用描画用インデックスバッファの準備を行う
extern int Graphics_D3D11_CommonIndexBuffer_Setup( int Size )
{
	D_D3D11_BUFFER_DESC BufferDesc ;
	HRESULT             hr ;

	if( Size <= GD3D11.Device.DrawInfo.CommonIndexBufferSize )
	{
		return 0 ;
	}

	Graphics_D3D11_CommonIndexBuffer_Terminate() ;

	Size += D3D11_COMMONBUFFER_ADDSIZE ;
	Size = ( Size + 15 ) / 16 * 16 ;

	_MEMSET( &BufferDesc, 0, sizeof( BufferDesc ) ) ;
	BufferDesc.ByteWidth      = ( UINT )Size ;
	BufferDesc.Usage          = D_D3D11_USAGE_DYNAMIC ;
//	BufferDesc.Usage          = D_D3D11_USAGE_DEFAULT ;
	BufferDesc.BindFlags      = D_D3D11_BIND_INDEX_BUFFER ;
	BufferDesc.CPUAccessFlags = D_D3D11_CPU_ACCESS_WRITE ;
//	BufferDesc.CPUAccessFlags = 0 ;
	hr = D3D11Device_CreateBuffer( &BufferDesc, NULL, &GD3D11.Device.DrawInfo.CommonIndexBuffer ) ;
	if( FAILED( hr ) )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x4e\x6c\x28\x75\xcf\x63\x3b\x75\x28\x75\x6e\x30\xa4\x30\xf3\x30\xc7\x30\xc3\x30\xaf\x30\xb9\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x28\x00\x20\x00\xb5\x30\xa4\x30\xba\x30\x20\x00\x25\x00\x64\x00\x20\x00\x62\x00\x79\x00\x74\x00\x65\x00\x20\x00\x29\x00\x0a\x00\x00"/*@ L"汎用描画用のインデックスバッファの作成に失敗しました( サイズ %d byte )\n" @*/, Size ) ) ;
		return -1 ;
	}

	GD3D11.Device.DrawInfo.CommonIndexBufferSize = Size ;

	// 正常終了
	return 0 ;
}

// 汎用描画用頂点バッファの後始末を行う
extern int Graphics_D3D11_CommonVertexBuffer_Terminate( void )
{
	if( GD3D11.Device.DrawInfo.CommonVertexBuffer == NULL )
	{
		return 0 ;
	}

	if( GD3D11.Device.State.SetVertexBuffer == GD3D11.Device.DrawInfo.CommonVertexBuffer )
	{
		Graphics_D3D11_DeviceState_SetVertexBuffer( NULL, 0 ) ;
	}

	Direct3D11_Release_Buffer( GD3D11.Device.DrawInfo.CommonVertexBuffer ) ;
	GD3D11.Device.DrawInfo.CommonVertexBuffer = NULL ;

	GD3D11.Device.DrawInfo.CommonVertexBufferSize = 0 ;

	return 0 ;
}

// 汎用描画用インデックスバッファの後始末を行う
extern int Graphics_D3D11_CommonIndexBuffer_Terminate( void )
{
	if( GD3D11.Device.DrawInfo.CommonIndexBuffer == NULL )
	{
		return 0 ;
	}

	if( GD3D11.Device.State.SetIndexBuffer == GD3D11.Device.DrawInfo.CommonIndexBuffer )
	{
		Graphics_D3D11_DeviceState_SetIndexBuffer( NULL, D_DXGI_FORMAT_R16_UINT ) ;
	}

	Direct3D11_Release_Buffer( GD3D11.Device.DrawInfo.CommonIndexBuffer ) ;
	GD3D11.Device.DrawInfo.CommonIndexBuffer = NULL ;

	GD3D11.Device.DrawInfo.CommonIndexBufferSize = 0 ;

	return 0 ;
}






















// Direct3D11 の描画処理準備関係

#ifndef DX_NON_ASYNCLOAD
static int Graphics_D3D11_RenderVertexASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO * Info )
{
	return Graphics_D3D11_RenderVertex(
		( int )Info->Data[ 0 ],
		FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

// 頂点バッファに溜まった頂点データをレンダリングする
extern	int		Graphics_D3D11_RenderVertex( int NextUse3DVertex, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Graphics_D3D11_RenderVertexASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )NextUse3DVertex ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// 頂点が一つも無かったら描画は行わない
	if( GD3D11.Device.DrawInfo.VertexNum != 0 && DxLib_GetEndRequest() == FALSE )
	{
		// Graphics_D3D11_BeginScene をしていなかったらする
//		if( GD3D11.Device.DrawInfo.BeginSceneFlag == FALSE ) Graphics_D3D11_BeginScene() ;

		// 非描画フラグが立っていなければレンダリングする
		if( GD3D11.Device.DrawInfo.BlendMaxNotDrawFlag == FALSE )
		{
			DWORD                      InputLayout ;

			InputLayout = D3D11_VertexTypeToInputLayout[ GD3D11.Device.DrawInfo.Use3DVertex ][ GD3D11.Device.DrawInfo.VertexType ] ;

			// 頂点データを頂点バッファに反映
#if VERTEXBUFFERMODE == 2
			NORMALVERTEXBUFFER_UNMAP
#elif VERTEXBUFFERMODE == 1
			D_D3D11_MAPPED_SUBRESOURCE MappedSubResource ;
			HRESULT                    hr ;
			hr = D3D11DeviceContext_Map_ASync( 
				GD3D11.Device.DrawInfo.VertexBuffer,
				0,
				D_D3D11_MAP_WRITE_DISCARD,
				0,
				&MappedSubResource
			) ;
			_MEMCPY( MappedSubResource.pData, GD3D11.Device.DrawInfo.VertexBufferTemp, D3D11_VertexSize[ InputLayout ] * GD3D11.Device.DrawInfo.VertexNum ) ;
			D3D11DeviceContext_Unmap_ASync(
				GD3D11.Device.DrawInfo.VertexBuffer,
				0
			) ;
#else
			D_D3D11_BOX DestBox ;
			DestBox.left   = 0 ;
			DestBox.top    = 0 ;
			DestBox.front  = 0 ;
			DestBox.right  = ( UINT )( D3D11_VertexSize[ InputLayout ] * GD3D11.Device.DrawInfo.VertexNum ) ;
			DestBox.bottom = 1 ;
			DestBox.back   = 1 ;
			D3D11DeviceContext_UpdateSubresource_ASync(
				GD3D11.Device.DrawInfo.VertexBuffer,
				0,
				&DestBox,
				GD3D11.Device.DrawInfo.VertexBufferTemp,
				0,
				0,
				ASyncThread
			) ;
#endif

			// プリミティブタイプをセット
			if( GD3D11.Device.State.PrimitiveTopology != GD3D11.Device.DrawInfo.PrimitiveType )
			{
				Graphics_D3D11_DeviceState_SetPrimitiveTopology( GD3D11.Device.DrawInfo.PrimitiveType ) ;
			}

			// 入力レイアウトをセット
			if( GD3D11.Device.State.SetVertexInputLayout != GD3D11.Device.InputLayout.BaseInputLayout[ InputLayout ] )
			{
				Graphics_D3D11_DeviceState_SetInputLayout( GD3D11.Device.InputLayout.BaseInputLayout[ InputLayout ] ) ;
			}

			// 頂点シェーダーをセット
			if( InputLayout == D3D11_VERTEX_INPUTLAYOUT_3D_LIGHT )
			{
				if( Graphics_D3D11_Shader_Normal3DDraw_Setup() == FALSE )
				{
					return -1 ;
				}
			}
			else
			{
				if( GD3D11.Device.State.SetVertexShader != GD3D11.Device.Shader.Base.BaseSimple_VS[ InputLayout ] )
				{
					Graphics_D3D11_DeviceState_SetVertexShader( GD3D11.Device.Shader.Base.BaseSimple_VS[ InputLayout ], TRUE ) ;
				}
			}

			// 頂点バッファをセット
			if( GD3D11.Device.State.SetVertexBuffer       != GD3D11.Device.DrawInfo.VertexBuffer ||
				GD3D11.Device.State.SetVertexBufferStride != D3D11_VertexSize[ InputLayout ] )
			{
				Graphics_D3D11_DeviceState_SetVertexBuffer( GD3D11.Device.DrawInfo.VertexBuffer, D3D11_VertexSize[ InputLayout ] ) ;
			}

			// ステートと定数バッファの更新
			Graphics_D3D11_DeviceState_SetupStateAndConstantBuffer_Inline() ;

			// 描画
			D3D11DeviceContext_Draw( ( UINT )GD3D11.Device.DrawInfo.VertexNum, 0 ) ;
			GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;
		}
	}

	if( NextUse3DVertex >= 0 )
	{
		GD3D11.Device.DrawInfo.Use3DVertex = NextUse3DVertex ;
	}

	GD3D11.Device.DrawInfo.VertexNum = 0 ;

#if VERTEXBUFFERMODE == 2
	NORMALVERTEXBUFFER_MAP
	GD3D11.Device.DrawInfo.VertexBufferNextAddr = GD3D11.Device.DrawInfo.VertexBufferAddr ;
#else
	GD3D11.Device.DrawInfo.VertexBufferNextAddr = GD3D11.Device.DrawInfo.VertexBufferTemp ;
#endif

	// 終了
	return 0 ;
}

// 描画準備を行う
extern	void	FASTCALL Graphics_D3D11_DrawPreparation( int ParamFlag )
{
	int AlphaTest ;
	int AlphaChannel ;
	int Specular ;
	int i ;

	// 必ず Graphics_D3D11_DrawPreparation を行うべきというフラグを倒す
	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = FALSE ;

	// 設定が変更されたかどうかのフラグを倒す
	GSYS.ChangeSettingFlag = FALSE ;

	// ライトを使うかどうかで処理を分岐
	if( ( ParamFlag & DX_D3D11_DRAWPREP_LIGHTING ) != 0 && GSYS.Light.ProcessDisable == FALSE )
	{
		// ライトの設定が変更されていたら変更を適応する
		if( GSYS.Light.HardwareChange )
		{
			Graphics_Light_RefreshState() ;
		}
		else
		{
			// ライトが有効なのに一時的に無効になっていたら有効にする
			if( GD3D11.Device.State.Lighting == 0 )
			{
				Graphics_D3D11_DeviceState_SetLighting( TRUE ) ;
			}
		}
	}
	else
	{
		// ライトが有効になっていたら無効にする
		if( GD3D11.Device.State.Lighting == 1 )
		{
			Graphics_D3D11_DeviceState_SetLighting( FALSE ) ;
		}
	}

	// マテリアルの設定が外部から変更されていたら元に戻す
	if( GSYS.Light.ChangeMaterial )
	{
		GSYS.Light.ChangeMaterial = 0 ;
		Graphics_D3D11_DeviceState_SetMaterial( &GSYS.Light.Material ) ;
	}

	// 頂点カラーをマテリアルのカラーとして使用するかどうかをセットする
	if( ( GSYS.Light.MaterialNotUseVertexDiffuseColor ? FALSE : TRUE ) != GD3D11.Device.State.MaterialUseVertexDiffuseColor )
	{
		Graphics_D3D11_DeviceState_SetUseVertexDiffuseColor( GSYS.Light.MaterialNotUseVertexDiffuseColor ? FALSE : TRUE ) ;
	}

	if( ( GSYS.Light.MaterialNotUseVertexSpecularColor ? FALSE : TRUE ) != GD3D11.Device.State.MaterialUseVertexSpecularColor )
	{
		Graphics_D3D11_DeviceState_SetUseVertexSpecularColor( GSYS.Light.MaterialNotUseVertexSpecularColor ? FALSE : TRUE ) ;
	}

	// スペキュラを使用するかどうかの設定を行う
	Specular = ( GSYS.DrawSetting.NotUseSpecular == FALSE && ( ParamFlag & DX_D3D11_DRAWPREP_SPECULAR ) != 0 ) ? TRUE : FALSE ;
	if( Specular != GD3D11.Device.State.UseSpecular )
	{
		Graphics_D3D11_DeviceState_SetSpecularEnable( Specular ) ;
	}

	// 射影行列の設定を行う
//	Graphics_DrawSetting_SetUse2DProjectionMatrix( ( ParamFlag & DX_D3D11_DRAWPREP_3D ) == 0 ? TRUE : FALSE ) ;

	// Ｚバッファの設定を行う
	{
		BOOL                    DepthEnable ;
		int                     WriteDepthBuffer ;
		D_D3D11_COMPARISON_FUNC DepthFunc ;
		int						DepthBias ;

		if( ParamFlag & DX_D3D11_DRAWPREP_3D )
		{
			DepthEnable      = GSYS.DrawSetting.EnableZBufferFlag3D ;
			WriteDepthBuffer = GSYS.DrawSetting.WriteZBufferFlag3D ;
			DepthFunc        = ( D_D3D11_COMPARISON_FUNC )GSYS.DrawSetting.ZBufferCmpType3D ;
			DepthBias        = GSYS.DrawSetting.ZBias3D ;
		}
		else
		{
			DepthEnable      = GSYS.DrawSetting.EnableZBufferFlag2D ;
			WriteDepthBuffer = GSYS.DrawSetting.WriteZBufferFlag2D ;
			DepthFunc        = ( D_D3D11_COMPARISON_FUNC )GSYS.DrawSetting.ZBufferCmpType2D ;
			DepthBias        = GSYS.DrawSetting.ZBias2D ;
		}

		if( DepthEnable )
		{
			if( GD3D11.Device.State.DepthStencilDesc.DepthEnable != DepthEnable )
			{
				Graphics_D3D11_DeviceState_SetDepthEnable( DepthEnable ) ;
			}

			if( ( GD3D11.Device.State.DepthStencilDesc.DepthWriteMask == D_D3D11_DEPTH_WRITE_MASK_ALL ? TRUE : FALSE ) != WriteDepthBuffer  )
			{
				Graphics_D3D11_DeviceState_SetDepthWriteEnable( WriteDepthBuffer  ) ;
			}

			if( GD3D11.Device.State.DepthStencilDesc.DepthFunc != DepthFunc )
			{
				Graphics_D3D11_DeviceState_SetDepthFunc( DepthFunc ) ;
			}

			if( GD3D11.Device.State.RasterizerDesc.DepthBias != DepthBias )
			{
				Graphics_D3D11_DeviceState_SetDepthBias( DepthBias ) ;
			}
		}
		else
		{
			if( GD3D11.Device.State.DepthStencilDesc.DepthEnable )
			{
				Graphics_D3D11_DeviceState_SetDepthEnable( FALSE ) ;
			}

			if( GD3D11.Device.State.DepthStencilDesc.DepthWriteMask != D_D3D11_DEPTH_WRITE_MASK_ZERO  )
			{
				Graphics_D3D11_DeviceState_SetDepthWriteEnable( FALSE ) ;
			}
		}
	}

	// フィルモードのセット
	if( GD3D11.Device.State.RasterizerDesc.FillMode != ( D_D3D11_FILL_MODE )GSYS.DrawSetting.FillMode )
	{
		Graphics_D3D11_DeviceState_SetFillMode( ( D_D3D11_FILL_MODE )GSYS.DrawSetting.FillMode ) ;
	}

	// フォグの設定を行う
	if( ParamFlag & DX_D3D11_DRAWPREP_FOG )
	{
		if( GD3D11.Device.State.FogEnable != GSYS.DrawSetting.FogEnable )
		{
			Graphics_D3D11_DeviceState_SetFogEnable( GSYS.DrawSetting.FogEnable ) ;
		}
	}
	else
	{
		if( GD3D11.Device.State.FogEnable )
		{
			Graphics_D3D11_DeviceState_SetFogEnable( FALSE ) ;
		}
	}

	// ブレンディング関係のセッティングを行う場合のみ実行する
	if( ( ParamFlag & DX_D3D11_DRAWPREP_NOBLENDSETTING ) == 0 )
	{
		// フラグの初期化
		AlphaTest    = FALSE ;
		AlphaChannel = FALSE ;

		// テクスチャーを使用するかどうかで処理を分岐
		// 使用しない場合は初期値のまま
		if( ParamFlag & DX_D3D11_DRAWPREP_TEXTURE )
		{
			// 透過色処理を行わない場合はカラーキーもαテストもαチャンネルも使用しないので初期値のまま
			// 透過色処理を行う場合のみ処理をする
			if( ParamFlag & DX_D3D11_DRAWPREP_TRANS )
			{
				int TexAlphaTestFlag = ( ParamFlag & DX_D3D11_DRAWPREP_TEXALPHATEST ) != 0 ? 1 : 0 ;
				int TexAlphaChFlag   = ( ParamFlag & DX_D3D11_DRAWPREP_TEXALPHACH   ) != 0 ? 1 : 0 ;

				// テクスチャにαビットがあるかどうかで処理を分岐
				if( TexAlphaTestFlag || TexAlphaChFlag )
				{
					// αテストでも頂点座標のデータ型が浮動小数点型で、且つテクスチャフィルタリングモードが
					// 線形補間であるか、ブレンドモードが DX_BLENDMODE_NOBLEND 以外だったらαチャンネルとして扱う
					if( ( GSYS.DrawSetting.BlendMode != DX_BLENDMODE_NOBLEND && GSYS.DrawSetting.BlendMode != DX_BLENDMODE_DESTCOLOR ) ||
						( ( ParamFlag & DX_D3D11_DRAWPREP_VECTORINT ) == 0   && GSYS.DrawSetting.DrawMode  == DX_DRAWMODE_BILINEAR   ) )
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

		// 調整されたパラメータをセットする
		if( GD3D11.Device.DrawSetting.IgnoreGraphColorFlag  != GSYS.DrawSetting.IgnoreGraphColorFlag )
		{
			Graphics_D3D11_DrawSetting_SetIgnoreDrawGraphColor( GSYS.DrawSetting.IgnoreGraphColorFlag ) ;
		}

		if( GD3D11.Device.DrawSetting.BlendMode             != GSYS.DrawSetting.BlendMode ||
			GD3D11.Device.DrawSetting.AlphaTestValidFlag    != AlphaTest ||
			GD3D11.Device.DrawSetting.AlphaChannelValidFlag != AlphaChannel               )
		{
			Graphics_D3D11_DrawSetting_SetDrawBlendMode( GSYS.DrawSetting.BlendMode, AlphaTest, AlphaChannel ) ;
		}

		if( GD3D11.Device.DrawSetting.AlphaTestMode         != GSYS.DrawSetting.AlphaTestMode ||
			GD3D11.Device.DrawSetting.AlphaTestParam        != GSYS.DrawSetting.AlphaTestParam )
		{
			Graphics_D3D11_DrawSetting_SetDrawAlphaTest( GSYS.DrawSetting.AlphaTestMode, GSYS.DrawSetting.AlphaTestParam ) ;
		}

		if( GD3D11.Device.DrawSetting.ChangeBlendParamFlag ||
			GD3D11.Device.DrawSetting.ChangeTextureFlag ||
			( GSYS.HardInfo.UseShader &&
			  GD3D11.Device.State.SetNormalPixelShader == FALSE ) )
		{
			Graphics_D3D11_DeviceState_NormalDrawSetup() ;
		}
	}

	if( GD3D11.Device.State.DrawMode != GSYS.DrawSetting.DrawMode )
	{
		Graphics_D3D11_DeviceState_SetDrawMode( GSYS.DrawSetting.DrawMode ) ;
	}

	if( GD3D11.Device.State.MaxAnisotropy != GSYS.DrawSetting.MaxAnisotropy )
	{
		Graphics_D3D11_DeviceState_SetMaxAnisotropy( GSYS.DrawSetting.MaxAnisotropy ) ;
	}

	if( ParamFlag & DX_D3D11_DRAWPREP_CULLING )
	{
		int SetCullMode ;

		switch( GD3D11.Device.State.RasterizerDesc.CullMode )
		{
		case D_D3D11_CULL_NONE :
		default :
			SetCullMode = 0 ;
			break ;

		case D_D3D11_CULL_BACK :
			SetCullMode = 1 ;
			break ;

		case D_D3D11_CULL_FRONT :
			SetCullMode = 2 ;
			break ;
		}

		if( SetCullMode != GSYS.DrawSetting.CullMode )
		{
			Graphics_D3D11_DeviceState_SetCullMode( GSYS.DrawSetting.CullMode ) ;
		}
	}
	else
	{
		if( GD3D11.Device.State.RasterizerDesc.CullMode != D_D3D11_CULL_NONE )
		{
			Graphics_D3D11_DeviceState_SetCullMode( DX_CULLING_NONE ) ;
		}
	}

	if( GSYS.DrawSetting.MatchHardwareTextureAddressTransformMatrix == FALSE )
	{
		Graphics_D3D11_DeviceState_SetTextureAddressTransformMatrix( GSYS.DrawSetting.TextureAddressTransformUse, &GSYS.DrawSetting.TextureAddressTransformMatrix ) ;
		GSYS.DrawSetting.MatchHardwareTextureAddressTransformMatrix = TRUE ;
		GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;
	}

	if( ParamFlag & DX_D3D11_DRAWPREP_TEXADDRESS )
	{
		for( i = 0; i < USE_TEXTURESTAGE_NUM; i++ )
		{
			if( GSYS.DrawSetting.TexAddressModeU[ i ] == GSYS.DrawSetting.TexAddressModeV[ i ] )
			{
				if( GD3D11.Device.State.SamplerDesc[ i ].AddressU != ( D_D3D11_TEXTURE_ADDRESS_MODE )GSYS.DrawSetting.TexAddressModeU[ i ] ||
					GD3D11.Device.State.SamplerDesc[ i ].AddressV != ( D_D3D11_TEXTURE_ADDRESS_MODE )GSYS.DrawSetting.TexAddressModeV[ i ] )
				{
					Graphics_D3D11_DeviceState_SetTextureAddress( GSYS.DrawSetting.TexAddressModeU[ i ], i ) ;
				}
			}
			else
			{
				if( GD3D11.Device.State.SamplerDesc[ i ].AddressU != ( D_D3D11_TEXTURE_ADDRESS_MODE )GSYS.DrawSetting.TexAddressModeU[ i ] )
				{
					Graphics_D3D11_DeviceState_SetTextureAddressU( GSYS.DrawSetting.TexAddressModeU[ i ], i ) ;
				}

				if( GD3D11.Device.State.SamplerDesc[ i ].AddressV != ( D_D3D11_TEXTURE_ADDRESS_MODE )GSYS.DrawSetting.TexAddressModeV[ i ] )
				{
					Graphics_D3D11_DeviceState_SetTextureAddressV( GSYS.DrawSetting.TexAddressModeV[ i ], i ) ;
				}
			}
		}
	}
	else
	{
		for( i = 0; i < 4; i++ )
		{
			if( GD3D11.Device.State.SamplerDesc[ i ].AddressU != D_D3D11_TEXTURE_ADDRESS_CLAMP ||
				GD3D11.Device.State.SamplerDesc[ i ].AddressV != D_D3D11_TEXTURE_ADDRESS_CLAMP )
			{
				Graphics_D3D11_DeviceState_SetTextureAddress( D_D3D11_TEXTURE_ADDRESS_CLAMP, i ) ;
			}
		}
	}

	// パラメータを保存
	GD3D11.Device.DrawSetting.DrawPrepParamFlag = ParamFlag ;
}


// 描画コマンドを開始する
extern	void	Graphics_D3D11_RenderBegin( void )
{
	if( D3D11Device_IsValid() == 0 || GD3D11.Device.DrawInfo.BeginSceneFlag == TRUE ) return ;

//	D3D11DeviceContext_Begin() ;
	GD3D11.Device.DrawInfo.BeginSceneFlag = TRUE ;
	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;
}

// 描画コマンドを終了する
extern	void	Graphics_D3D11_RenderEnd( void )
{
	if( D3D11Device_IsValid() == 0 || GD3D11.Device.DrawInfo.BeginSceneFlag == FALSE ) return ;

//	D3D11DeviceContext_End() ;
	GD3D11.Device.DrawInfo.BeginSceneFlag = FALSE ;
}




























// Direct3D11 を使った描画関係

// ハードウエアアクセラレータ使用版 DrawBillboard3D
extern	int		Graphics_D3D11_DrawBillboard3D( VECTOR Pos, float cx, float cy, float Size, float Angle, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, int ReverseXFlag, int ReverseYFlag, int DrawFlag, RECT *DrawArea )
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

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// 描画準備
	if( DrawFlag )
	{
		Flag = TransFlag | DX_D3D11_DRAWPREP_3D | DX_D3D11_DRAWPREP_FOG | DX_D3D11_DRAWPREP_TEXADDRESS ;
		DX_D3D11_DRAWPREP_TEX( Orig, Image->Hard.Draw[ 0 ].Tex->PF->D3D11.Texture, Image->Hard.Draw[ 0 ].Tex->PF->D3D11.TextureSRV, Flag )
	}
	else
	{
		dleft   = -100000000.0f ;
		dright  =  100000000.0f ;
		dtop    = -100000000.0f ;
		dbottom =  100000000.0f ;
	}

	// 頂点データを取得
	DiffuseColor = GD3D11.Device.DrawInfo.DiffuseColor ;

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
			Graphics_D3D11_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D11.Texture, DrawTex->Tex->PF->D3D11.TextureSRV ) ;

			if( BlendDrawTex != NULL )
			{
				Graphics_D3D11_DrawSetting_SetBlendTexture( BlendDrawTex->Tex->PF->D3D11.Texture, BlendDrawTex->Tex->PF->D3D11.TextureSRV, BlendDrawTex->Tex->TexWidth, BlendDrawTex->Tex->TexHeight );
			}

			if( GD3D11.Device.DrawSetting.ChangeTextureFlag )
			{
				Graphics_D3D11_DeviceState_NormalDrawSetup() ;
			}
		}

		TexVert = DrawTex->Vertex ;
		if( BlendDrawTex != NULL )
		{
			BlendTexVert = BlendDrawTex->Vertex ;
		}

		// ブレンドグラフィックを使用していなくて、且つ描画する場合は高速な処理を使用する
		if( GD3D11.Device.DrawInfo.VertexType != VERTEXTYPE_BLENDTEX && DrawFlag == TRUE )
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

				DrawVert3D[ 4 ].pos.x = SrcVec[ 2 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][0] + SrcVec[ 2 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][0] + Pos.x ;
				DrawVert3D[ 4 ].pos.y = SrcVec[ 2 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][1] + SrcVec[ 2 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][1] + Pos.y ;
				DrawVert3D[ 4 ].pos.z = SrcVec[ 2 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][2] + SrcVec[ 2 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][2] + Pos.z ;

				DrawVert3D[ 5 ].pos.x = SrcVec[ 1 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][0] + SrcVec[ 1 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][0] + Pos.x ;
				DrawVert3D[ 5 ].pos.y = SrcVec[ 1 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][1] + SrcVec[ 1 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][1] + Pos.y ;
				DrawVert3D[ 5 ].pos.z = SrcVec[ 1 ].x * GSYS.DrawSetting.BillboardMatrixF.m[0][2] + SrcVec[ 1 ].y * GSYS.DrawSetting.BillboardMatrixF.m[1][2] + Pos.z ;
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

				DrawVert3D[ 4 ].pos.x = TempVecX[ 0 ].x + TempVecY[ 1 ].x ;
				DrawVert3D[ 4 ].pos.y = TempVecX[ 0 ].y + TempVecY[ 1 ].y ;
				DrawVert3D[ 4 ].pos.z = TempVecX[ 0 ].z + TempVecY[ 1 ].z ;

				DrawVert3D[ 5 ].pos.x = TempVecX[ 1 ].x + TempVecY[ 0 ].x ;
				DrawVert3D[ 5 ].pos.y = TempVecX[ 1 ].y + TempVecY[ 0 ].y ;
				DrawVert3D[ 5 ].pos.z = TempVecX[ 1 ].z + TempVecY[ 0 ].z ;
			}

			*( ( DWORD * )&DrawVert3D[0].b ) = DiffuseColor ;
			*( ( DWORD * )&DrawVert3D[1].b ) = DiffuseColor ;
			*( ( DWORD * )&DrawVert3D[2].b ) = DiffuseColor ;
			*( ( DWORD * )&DrawVert3D[3].b ) = DiffuseColor ;
			*( ( DWORD * )&DrawVert3D[4].b ) = DiffuseColor ; 
			*( ( DWORD * )&DrawVert3D[5].b ) = DiffuseColor ;

			if( ReverseXFlag )
			{
				DrawVert3D[0].u = TexVert[1].u ;
				DrawVert3D[1].u = TexVert[0].u ;
				DrawVert3D[2].u = TexVert[1].u ;
				DrawVert3D[3].u = TexVert[0].u ;
				DrawVert3D[4].u = TexVert[1].u ;
				DrawVert3D[5].u = TexVert[0].u ;
			}
			else
			{
				DrawVert3D[0].u = TexVert[0].u ;
				DrawVert3D[1].u = TexVert[1].u ;
				DrawVert3D[2].u = TexVert[0].u ;
				DrawVert3D[3].u = TexVert[1].u ;
				DrawVert3D[4].u = TexVert[0].u ;
				DrawVert3D[5].u = TexVert[1].u ;
			}
			if( ReverseYFlag )
			{
				DrawVert3D[0].v = TexVert[2].v ;
				DrawVert3D[1].v = TexVert[2].v ;
				DrawVert3D[2].v = TexVert[0].v ;
				DrawVert3D[3].v = TexVert[0].v ;
				DrawVert3D[4].v = TexVert[0].v ;
				DrawVert3D[5].v = TexVert[2].v ;
			}
			else
			{
				DrawVert3D[0].v = TexVert[0].v ;
				DrawVert3D[1].v = TexVert[0].v ;
				DrawVert3D[2].v = TexVert[2].v ;
				DrawVert3D[3].v = TexVert[2].v ;
				DrawVert3D[4].v = TexVert[2].v ;
				DrawVert3D[5].v = TexVert[0].v ;
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
			switch( GD3D11.Device.DrawInfo.VertexType )
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
					u[ 0 ] = ( TexVert[0].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					u[ 1 ] = ( TexVert[1].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					v[ 0 ] = ( TexVert[0].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					v[ 1 ] = ( TexVert[2].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
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
					DrawVertB[0].u1 = ( DrawPos[ 0 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[0].v1 = ( DrawPos[ 0 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[1].u1 = ( DrawPos[ 1 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[1].v1 = ( DrawPos[ 1 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[2].u1 = ( DrawPos[ 2 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[2].v1 = ( DrawPos[ 2 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[3].u1 = ( DrawPos[ 3 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[3].v1 = ( DrawPos[ 3 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[4].u1 = ( DrawPos[ 2 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[4].v1 = ( DrawPos[ 2 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[5].u1 = ( DrawPos[ 1 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[5].v1 = ( DrawPos[ 1 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
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
extern	int		Graphics_D3D11_DrawModiBillboard3D( VECTOR Pos, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, int DrawFlag, RECT *DrawArea )
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
	float dleft = 0.0f, dright = 0.0f, dtop = 0.0f, dbottom = 0.0f;
	float z ;
	float rhw ;
	float u[ 2 ] ;
	float v[ 2 ] ;

	Orig = Image->Orig ;

	if( GAPIWin.D3D11DeviceObject == NULL ) return -1 ;

	// 描画準備
	if( DrawFlag )
	{
		Flag = TransFlag | DX_D3D11_DRAWPREP_3D | DX_D3D11_DRAWPREP_FOG | DX_D3D11_DRAWPREP_TEXADDRESS ;
		DX_D3D11_DRAWPREP_TEX( Orig, Image->Hard.Draw[ 0 ].Tex->PF->D3D11.Texture, Image->Hard.Draw[ 0 ].Tex->PF->D3D11.TextureSRV, Flag )
	}
	else
	{
		dleft   = -100000000.0f ;
		dright  =  100000000.0f ;
		dtop    = -100000000.0f ;
		dbottom =  100000000.0f ;
	}

	// 頂点データを取得
	DiffuseColor = GD3D11.Device.DrawInfo.DiffuseColor ;

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
			Graphics_D3D11_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D11.Texture, DrawTex->Tex->PF->D3D11.TextureSRV ) ;

			if( BlendDrawTex != NULL )
			{
				Graphics_D3D11_DrawSetting_SetBlendTexture( BlendDrawTex->Tex->PF->D3D11.Texture, BlendDrawTex->Tex->PF->D3D11.TextureSRV, BlendDrawTex->Tex->TexWidth, BlendDrawTex->Tex->TexHeight );
			}

			if( GD3D11.Device.DrawSetting.ChangeTextureFlag )
			{
				Graphics_D3D11_DeviceState_NormalDrawSetup() ;
			}
		}

		TexVert = DrawTex->Vertex ;
		if( BlendDrawTex != NULL )
		{
			BlendTexVert = BlendDrawTex->Vertex ;
		}

		// ブレンドグラフィックを使用していなくて、且つ描画する場合は高速な処理を使用する
		if( GD3D11.Device.DrawInfo.VertexType != VERTEXTYPE_BLENDTEX && DrawFlag == TRUE )
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

			DrawVert3D[ 4 ].pos.x = x4 * GSYS.DrawSetting.BillboardMatrixF.m[0][0] + y4 * GSYS.DrawSetting.BillboardMatrixF.m[1][0] + Pos.x ;
			DrawVert3D[ 4 ].pos.y = x4 * GSYS.DrawSetting.BillboardMatrixF.m[0][1] + y4 * GSYS.DrawSetting.BillboardMatrixF.m[1][1] + Pos.y ;
			DrawVert3D[ 4 ].pos.z = x4 * GSYS.DrawSetting.BillboardMatrixF.m[0][2] + y4 * GSYS.DrawSetting.BillboardMatrixF.m[1][2] + Pos.z ;

			DrawVert3D[ 5 ].pos.x = x2 * GSYS.DrawSetting.BillboardMatrixF.m[0][0] + y2 * GSYS.DrawSetting.BillboardMatrixF.m[1][0] + Pos.x ;
			DrawVert3D[ 5 ].pos.y = x2 * GSYS.DrawSetting.BillboardMatrixF.m[0][1] + y2 * GSYS.DrawSetting.BillboardMatrixF.m[1][1] + Pos.y ;
			DrawVert3D[ 5 ].pos.z = x2 * GSYS.DrawSetting.BillboardMatrixF.m[0][2] + y2 * GSYS.DrawSetting.BillboardMatrixF.m[1][2] + Pos.z ;

			*( ( DWORD * )&DrawVert3D[0].b ) = DiffuseColor ;
			*( ( DWORD * )&DrawVert3D[1].b ) = DiffuseColor ;
			*( ( DWORD * )&DrawVert3D[2].b ) = DiffuseColor ;
			*( ( DWORD * )&DrawVert3D[3].b ) = DiffuseColor ;
			*( ( DWORD * )&DrawVert3D[4].b ) = DiffuseColor ;
			*( ( DWORD * )&DrawVert3D[5].b ) = DiffuseColor ;

			DrawVert3D[0].u = TexVert[0].u ;
			DrawVert3D[0].v = TexVert[0].v ;
			DrawVert3D[1].u = TexVert[1].u ;
			DrawVert3D[1].v = TexVert[0].v ;
			DrawVert3D[2].u = TexVert[0].u ;
			DrawVert3D[2].v = TexVert[2].v ;
			DrawVert3D[3].u = TexVert[1].u ;
			DrawVert3D[3].v = TexVert[2].v ;
			DrawVert3D[4].u = TexVert[0].u ;
			DrawVert3D[4].v = TexVert[2].v ;
			DrawVert3D[5].u = TexVert[1].u ;
			DrawVert3D[5].v = TexVert[0].v ;

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
			switch( GD3D11.Device.DrawInfo.VertexType )
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
					u[ 0 ] = ( TexVert[0].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					u[ 1 ] = ( TexVert[1].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					v[ 0 ] = ( TexVert[0].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					v[ 1 ] = ( TexVert[2].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
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
					DrawVertB[0].u1 = ( DrawPos[ 0 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[0].v1 = ( DrawPos[ 0 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[1].u1 = ( DrawPos[ 1 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[1].v1 = ( DrawPos[ 1 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[2].u1 = ( DrawPos[ 2 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[2].v1 = ( DrawPos[ 2 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[3].u1 = ( DrawPos[ 3 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[3].v1 = ( DrawPos[ 3 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[4].u1 = ( DrawPos[ 2 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[4].v1 = ( DrawPos[ 2 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[5].u1 = ( DrawPos[ 1 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[5].v1 = ( DrawPos[ 1 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
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
extern	int		Graphics_D3D11_DrawGraph( int x, int y, float xf, float yf, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, int IntFlag )
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

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// float座標補正
//	xf -= 0.5F ;
//	yf -= 0.5F ;

	// 描画準備
	Flag = TransFlag | ( IntFlag << 1 ) ;
	DX_D3D11_DRAWPREP_TEX( Orig, Image->Hard.Draw[ 0 ].Tex->PF->D3D11.Texture, Image->Hard.Draw[ 0 ].Tex->PF->D3D11.TextureSRV, Flag )

	// 頂点データを取得
	DiffuseColor = GD3D11.Device.DrawInfo.DiffuseColor ;

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
//	if( x + Image->Width  < GSYS.DrawSetting.DrawArea.right  && x > GSYS.DrawSetting.DrawArea.left &&
//		y + Image->Height < GSYS.DrawSetting.DrawArea.bottom && y > GSYS.DrawSetting.DrawArea.right )
	{
		// はみ出ていない場合

		// 頂点タイプによって処理を分岐
		if( GD3D11.Device.DrawInfo.VertexType == VERTEXTYPE_BLENDTEX )
		{
			// ブレンドテクスチャを使用する

			// テクスチャーの数だけ繰り返す
			for( i = 0 ; i < DrawTexNum ; i ++, DrawTex ++ )
			{
				// 描画するテクスチャーのセット
				Graphics_D3D11_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D11.Texture, DrawTex->Tex->PF->D3D11.TextureSRV ) ;
				Graphics_D3D11_DrawSetting_SetBlendTexture( BlendDrawTex->Tex->PF->D3D11.Texture, BlendDrawTex->Tex->PF->D3D11.TextureSRV, BlendDrawTex->Tex->TexWidth, BlendDrawTex->Tex->TexHeight );

				if( GD3D11.Device.DrawSetting.ChangeTextureFlag )
				{
					Graphics_D3D11_DeviceState_NormalDrawSetup() ;
				}

				// 頂点バッファの取得
				GETVERTEX_QUAD( DrawVertB )
				
				TexVert = DrawTex->Vertex ;
				BlendTexVert = BlendDrawTex->Vertex ;

				DrawVertB[0].color = ( DWORD )DiffuseColor ;
				DrawVertB[1].color = ( DWORD )DiffuseColor ;
				DrawVertB[2].color = ( DWORD )DiffuseColor ;
				DrawVertB[3].color = ( DWORD )DiffuseColor ;
				DrawVertB[4].color = ( DWORD )DiffuseColor ;
				DrawVertB[5].color = ( DWORD )DiffuseColor ;

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

				u[ 0 ] *= GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
				u[ 1 ] *= GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
				v[ 0 ] *= GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
				v[ 1 ] *= GD3D11.Device.DrawSetting.InvBlendTextureHeight ;

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

				DrawVertB[0].rhw = 1.0f ;
				DrawVertB[1].rhw = 1.0f ;
				DrawVertB[2].rhw = 1.0f ;
				DrawVertB[3].rhw = 1.0f ;
				DrawVertB[4].rhw = 1.0f ;
				DrawVertB[5].rhw = 1.0f ;

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
				DrawVert[0].color	= DiffuseColor ;
				DrawVert[1].color	= DiffuseColor ;
				DrawVert[2].color	= DiffuseColor ;
				DrawVert[3].color	= DiffuseColor ;
				DrawVert[4].color	= DiffuseColor ;
				DrawVert[5].color	= DiffuseColor ;

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

				DrawVert[0].rhw = 1.0f ;
				DrawVert[1].rhw = 1.0f ;
				DrawVert[2].rhw = 1.0f ;
				DrawVert[3].rhw = 1.0f ;
				DrawVert[4].rhw = 1.0f ;
				DrawVert[5].rhw = 1.0f ;

				// テクスチャーを描画する
				ADD4VERTEX_TEX

				i ++ ;
				if( i >= DrawTexNum ) break ;

				// 描画するテクスチャーのセット
				DrawTex ++ ;
				Graphics_D3D11_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D11.Texture, DrawTex->Tex->PF->D3D11.TextureSRV ) ;
				Graphics_D3D11_DeviceState_NormalDrawSetup() ;
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
		switch( GD3D11.Device.DrawInfo.VertexType )
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
				Graphics_D3D11_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D11.Texture, DrawTex->Tex->PF->D3D11.TextureSRV ) ;
				Graphics_D3D11_DrawSetting_SetBlendTexture( BlendDrawTex->Tex->PF->D3D11.Texture, BlendDrawTex->Tex->PF->D3D11.TextureSRV, BlendDrawTex->Tex->TexWidth, BlendDrawTex->Tex->TexHeight );

				if( GD3D11.Device.DrawSetting.ChangeTextureFlag )
				{
					Graphics_D3D11_DeviceState_NormalDrawSetup() ;
				}

				// 頂点バッファの取得
				GETVERTEX_QUAD( DrawVertB )
				
				// 頂点データを初期化する
				TexVert = DrawTex->Vertex ;
				BlendTexVert = BlendDrawTex->Vertex ;

				Vx  = 1.0F / ( float )DrawTex->Tex->TexWidth  ;
				Vy  = 1.0F / ( float )DrawTex->Tex->TexHeight ;
				VBx = GD3D11.Device.DrawSetting.InvBlendTextureWidth  ;
				VBy = GD3D11.Device.DrawSetting.InvBlendTextureHeight ;

				DrawVertB[0].color = ( DWORD )DiffuseColor ;
				DrawVertB[1].color = ( DWORD )DiffuseColor ;
				DrawVertB[2].color = ( DWORD )DiffuseColor ;
				DrawVertB[3].color = ( DWORD )DiffuseColor ;
				DrawVertB[4].color = ( DWORD )DiffuseColor ;
				DrawVertB[5].color = ( DWORD )DiffuseColor ;

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
					u1[2] = u1[0] = ( TexVert[0].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					u1[3] = u1[1] = ( TexVert[1].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					v1[1] = v1[0] = ( TexVert[0].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					v1[3] = v1[2] = ( TexVert[2].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					break ;

				case DX_BLENDGRAPH_POSMODE_SCREEN :
					u1[2] = u1[0] = ( ( xf + TexVert[0].x ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					u1[3] = u1[1] = ( ( xf + TexVert[1].x ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					v1[1] = v1[0] = ( ( yf + TexVert[0].y ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					v1[3] = v1[2] = ( ( yf + TexVert[2].y ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
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

				DrawVertB[0].rhw = 1.0f ;
				DrawVertB[1].rhw = 1.0f ;
				DrawVertB[2].rhw = 1.0f ;
				DrawVertB[3].rhw = 1.0f ;
				DrawVertB[4].rhw = 1.0f ;
				DrawVertB[5].rhw = 1.0f ;

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
				DrawVert[1].color	= DiffuseColor ;
				DrawVert[2].color	= DiffuseColor ;
				DrawVert[3].color	= DiffuseColor ;
				DrawVert[4].color	= DiffuseColor ;
				DrawVert[5].color	= DiffuseColor ;

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

				DrawVert[0].rhw = 1.0f ;
				DrawVert[1].rhw = 1.0f ;
				DrawVert[2].rhw = 1.0f ;
				DrawVert[3].rhw = 1.0f ;
				DrawVert[4].rhw = 1.0f ;
				DrawVert[5].rhw = 1.0f ;

				// テクスチャーを描画する
				ADD4VERTEX_TEX

				i ++ ;
				if( i >= DrawTexNum ) break ;

				// 描画するテクスチャーのセット
				DrawTex ++ ;
				Graphics_D3D11_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D11.Texture, DrawTex->Tex->PF->D3D11.TextureSRV ) ;
				Graphics_D3D11_DeviceState_NormalDrawSetup() ;
			}
			break ;
		}
	}

	// 終了
	return 0 ;
}

// ハードウエアアクセラレータ使用版 DrawExtendGraph
extern	int		Graphics_D3D11_DrawExtendGraph( int x1, int y1, int x2, int y2, float x1f, float y1f, float x2f, float y2f, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, int IntFlag )
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
	DWORD drawz;
	int Flag ;
	float XBuf[ 2 ] ;
	float YBuf[ 2 ] ;
	float u[ 2 ] ;
	float v[ 2 ] ;

	Orig = Image->Orig ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// 描画判定
//	if( x2 - x1 == Image->Width && y2 - y1 == Image->Height ) return G_D3D_DrawGraph( x1, y1, GraphData, TransFlag ) ; 	

	// 幅がなかった場合はエラー
	if( x1 == x2 || y1 == y2 ) return -1 ;

	// 描画の準備
	Flag = TransFlag | ( IntFlag ? ( GSYS.DrawSetting.DrawMode == DX_DRAWMODE_BILINEAR ? 0 : DX_D3D11_DRAWPREP_VECTORINT ) : 0 ) ;
	DX_D3D11_DRAWPREP_TEX( Orig, Image->Hard.Draw[ 0 ].Tex->PF->D3D11.Texture, Image->Hard.Draw[ 0 ].Tex->PF->D3D11.TextureSRV, Flag )

	// ディフューズカラーの取得
	DiffuseColor = GD3D11.Device.DrawInfo.DiffuseColor ;

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

//	x1f -= 0.5F ;
//	y1f -= 0.5F ;

	if( x1 > x2 ){ i = x2; x2 = x1; x1 = i; }
	if( y1 > y2 ){ i = y2; y2 = y1; y1 = i; }

	// 描画範囲からはみ出ているかどうかで処理を分岐
	if( GSYS.DrawSetting.Valid2DMatrix || IN_DRAWAREA_CHECK( x1, y1, x2, y2 ) )
	{
		// 頂点タイプによって処理を分岐
		switch( GD3D11.Device.DrawInfo.VertexType )
		{
		case VERTEXTYPE_BLENDTEX :	// ブレンドテクスチャを使用する
			// テクスチャーの数だけ繰り返す
			for( i = 0 ; i < DrawTexNum ; i ++, DrawTex ++ )
			{
				// 描画するテクスチャーのセット
				Graphics_D3D11_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D11.Texture, DrawTex->Tex->PF->D3D11.TextureSRV ) ;
				Graphics_D3D11_DrawSetting_SetBlendTexture( BlendDrawTex->Tex->PF->D3D11.Texture, BlendDrawTex->Tex->PF->D3D11.TextureSRV, BlendDrawTex->Tex->TexWidth, BlendDrawTex->Tex->TexHeight );

				if( GD3D11.Device.DrawSetting.ChangeTextureFlag )
				{
					Graphics_D3D11_DeviceState_NormalDrawSetup() ;
				}

				// 頂点データを初期化する
				GETVERTEX_QUAD( DrawVertB )
				
				TexVert = DrawTex->Vertex ;
				BlendTexVert = BlendDrawTex->Vertex ;

				DrawVertB[0].color = ( DWORD )DiffuseColor ;
				DrawVertB[1].color = ( DWORD )DiffuseColor ;
				DrawVertB[2].color = ( DWORD )DiffuseColor ;
				DrawVertB[3].color = ( DWORD )DiffuseColor ;
				DrawVertB[4].color = ( DWORD )DiffuseColor ;
				DrawVertB[5].color = ( DWORD )DiffuseColor ;

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

				u[ 0 ] *= GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
				u[ 1 ] *= GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
				v[ 0 ] *= GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
				v[ 1 ] *= GD3D11.Device.DrawSetting.InvBlendTextureHeight ;

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

				DrawVertB[0].rhw = 1.0f ;
				DrawVertB[1].rhw = 1.0f ;
				DrawVertB[2].rhw = 1.0f ;
				DrawVertB[3].rhw = 1.0f ;
				DrawVertB[4].rhw = 1.0f ;
				DrawVertB[5].rhw = 1.0f ;

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
				DrawVert[1].color = ( DWORD )DiffuseColor ;
				DrawVert[2].color = ( DWORD )DiffuseColor ;
				DrawVert[3].color = ( DWORD )DiffuseColor ;
				DrawVert[4].color = ( DWORD )DiffuseColor ;
				DrawVert[5].color = ( DWORD )DiffuseColor ;

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

				DrawVert[0].rhw = 1.0f ;
				DrawVert[1].rhw = 1.0f ;
				DrawVert[2].rhw = 1.0f ;
				DrawVert[3].rhw = 1.0f ;
				DrawVert[4].rhw = 1.0f ;
				DrawVert[5].rhw = 1.0f ;

				// テクスチャーを描画する
				ADD4VERTEX_TEX

				i ++ ;
				if( i >= DrawTexNum ) break ;

				// 描画するテクスチャーのセット
				DrawTex ++ ;
				Graphics_D3D11_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D11.Texture, DrawTex->Tex->PF->D3D11.TextureSRV ) ;
				Graphics_D3D11_DeviceState_NormalDrawSetup() ;
			}
			break ;
		}
	}
	else
	{
		// 完全にはみ出ていたら何もせず終了
		if( OUT_DRAWAREA_CHECK( x1, y1, x2, y2 ) ) return 0 ;

		// 描画矩形の保存
		left	= GSYS.DrawSetting.DrawAreaF.left   ;
		top		= GSYS.DrawSetting.DrawAreaF.top    ;
		right	= GSYS.DrawSetting.DrawAreaF.right  ;
		bottom	= GSYS.DrawSetting.DrawAreaF.bottom ;

		// 頂点タイプによって処理を分岐
		switch( GD3D11.Device.DrawInfo.VertexType )
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
				Graphics_D3D11_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D11.Texture, DrawTex->Tex->PF->D3D11.TextureSRV ) ;
				Graphics_D3D11_DrawSetting_SetBlendTexture( BlendDrawTex->Tex->PF->D3D11.Texture, BlendDrawTex->Tex->PF->D3D11.TextureSRV, BlendDrawTex->Tex->TexWidth, BlendDrawTex->Tex->TexHeight );

				if( GD3D11.Device.DrawSetting.ChangeTextureFlag )
				{
					Graphics_D3D11_DeviceState_NormalDrawSetup() ;
				}

				GETVERTEX_QUAD( DrawVertB )
				
				Vx = 1.0F / ( DrawTex->Tex->TexWidth * GraphExRateX ) ;
				Vy = 1.0F / ( DrawTex->Tex->TexHeight * GraphExRateY ) ;
				VBx = GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
				VBy = GD3D11.Device.DrawSetting.InvBlendTextureHeight ;

				// 頂点データを初期化する
				TexVert = DrawTex->Vertex ;
				BlendTexVert = BlendDrawTex->Vertex ;

				DrawVertB[0].color = ( DWORD )DiffuseColor ;
				DrawVertB[1].color = ( DWORD )DiffuseColor ;
				DrawVertB[2].color = ( DWORD )DiffuseColor ;
				DrawVertB[3].color = ( DWORD )DiffuseColor ;
				DrawVertB[4].color = ( DWORD )DiffuseColor ;
				DrawVertB[5].color = ( DWORD )DiffuseColor ;

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
					u1[2] = u1[0] = ( TexVert[0].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					u1[3] = u1[1] = ( TexVert[1].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					v1[1] = v1[0] = ( TexVert[0].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					v1[3] = v1[2] = ( TexVert[2].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					break ;

				case DX_BLENDGRAPH_POSMODE_SCREEN :
					u1[2] = u1[0] = ( XBuf[ 0 ] - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					u1[3] = u1[1] = ( XBuf[ 1 ] - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					v1[1] = v1[0] = ( YBuf[ 0 ] - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					v1[3] = v1[2] = ( YBuf[ 1 ] - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
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

				DrawVertB[0].rhw = 1.0f ;
				DrawVertB[1].rhw = 1.0f ;
				DrawVertB[2].rhw = 1.0f ;
				DrawVertB[3].rhw = 1.0f ;
				DrawVertB[4].rhw = 1.0f ;
				DrawVertB[5].rhw = 1.0f ;

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
				DrawVert[1].color = ( DWORD )DiffuseColor ;
				DrawVert[2].color = ( DWORD )DiffuseColor ;
				DrawVert[3].color = ( DWORD )DiffuseColor ;
				DrawVert[4].color = ( DWORD )DiffuseColor ;
				DrawVert[5].color = ( DWORD )DiffuseColor ;

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

				DrawVert[0].rhw = 1.0f ;
				DrawVert[1].rhw = 1.0f ;
				DrawVert[2].rhw = 1.0f ;
				DrawVert[3].rhw = 1.0f ;
				DrawVert[4].rhw = 1.0f ;
				DrawVert[5].rhw = 1.0f ;

				// テクスチャーを描画する
				ADD4VERTEX_TEX

				i ++ ;
				if( i >= DrawTexNum ) break ;

				// 描画するテクスチャーのセット
				DrawTex ++ ;
				Graphics_D3D11_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D11.Texture, DrawTex->Tex->PF->D3D11.TextureSRV ) ;
				Graphics_D3D11_DeviceState_NormalDrawSetup() ;
			}
			break ;
		}
	}

	// 終了
	return 0 ;
}

// ハードウエアアクセラレータ使用版 DrawRotaGraph
extern int Graphics_D3D11_DrawRotaGraph( int x, int y, float xf, float yf, double ExRate, double Angle, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, int ReverseXFlag, int ReverseYFlag, int IntFlag )
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
	DWORD drawz;
//	BOOL Adj = FALSE ;
	float CenX, CenY ;
	float fx, fy ;
	int hx = 0, hy = 0 ;
	int FastCode, SinCosNone, AngleCheck, AlwaysCheck ;
	int Flag ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// 幅がなかった場合はエラー
	if( ExRate == 0.0 ) return -1 ;

	// 描画の準備
	Flag = TransFlag ;
	DX_D3D11_DRAWPREP_TEX( Image->Orig, Image->Hard.Draw[ 0 ].Tex->PF->D3D11.Texture, Image->Hard.Draw[ 0 ].Tex->PF->D3D11.TextureSRV, Flag )

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
						xf   = ( float )x ;
						yf   = ( float )y ;
					}
					else
					{
						CenX = ( float )Image->WidthI * 0.5F ;
						CenY = ( float )Image->HeightI * 0.5F ;
						xf   = ( float )x - 0.5F ;
						yf   = ( float )y - 0.5F ;
						FastCode = 1;
					}
				}
				else
				{
					CenX = ( float )Image->WidthI * 0.5F ;
					CenY = ( float )Image->HeightI * 0.5F ;
					xf   = ( float )x ;
					yf   = ( float )y ;
						
					if( hx != 0 ) CenX -= 0.5F ;
					if( hy != 0 ) CenY -= 0.5F ;
				}
			}
			else
			{
				CenX = ( float )Image->WidthI * 0.5F ;
				CenY = ( float )Image->HeightI * 0.5F ;
				xf   = ( float )x ;
				yf   = ( float )y ;
				FastCode = 1 ;
			}
		}
		else
		{
			CenX = ( float )Image->WidthI * 0.5F ;
			CenY = ( float )Image->HeightI * 0.5F ;
			xf   = ( float )x + 0.5f ;
			yf   = ( float )y + 0.5f ;
			AlwaysCheck = 1 ;
		}
	}
	else
	{
		CenX = ( float )Image->WidthF * 0.5F ;
		CenY = ( float )Image->HeightF * 0.5F ;
//		xf -= 0.5F ;
//		yf -= 0.5F ;
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

//	AlwaysCheck = 1 ;
//	FastCode = 1 ;

	// 頂点データを取得（今のところ色情報のセットのみ）
	DiffuseColor = GD3D11.Device.DrawInfo.DiffuseColor ;

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
	switch( GD3D11.Device.DrawInfo.VertexType )
	{
	case VERTEXTYPE_BLENDTEX :	// ブレンドテクスチャを使用する
		// テクスチャーの数だけ繰り返す
		if( DrawTexNum == 1 && FastCode == 1 )
		{
			// 描画するテクスチャーのセット
			Graphics_D3D11_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D11.Texture, DrawTex->Tex->PF->D3D11.TextureSRV ) ;
			Graphics_D3D11_DrawSetting_SetBlendTexture( BlendDrawTex->Tex->PF->D3D11.Texture, BlendDrawTex->Tex->PF->D3D11.TextureSRV, BlendDrawTex->Tex->TexWidth, BlendDrawTex->Tex->TexHeight );

			if( GD3D11.Device.DrawSetting.ChangeTextureFlag )
			{
				Graphics_D3D11_DeviceState_NormalDrawSetup() ;
			}

			// 頂点データを初期化する
			GETVERTEX_QUAD( DrawVertB )

			TexVert = DrawTex->Vertex ;
			BlendTexVert = BlendDrawTex->Vertex ;

			DrawVertB[0].color = ( DWORD )DiffuseColor ;
			DrawVertB[1].color = ( DWORD )DiffuseColor ;
			DrawVertB[2].color = ( DWORD )DiffuseColor ;
			DrawVertB[3].color = ( DWORD )DiffuseColor ;
			DrawVertB[4].color = ( DWORD )DiffuseColor ;
			DrawVertB[5].color = ( DWORD )DiffuseColor ;

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
				u[ 0 ] = TexVert[0].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
				u[ 1 ] = TexVert[1].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
				v[ 0 ] = TexVert[0].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
				v[ 1 ] = TexVert[2].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
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
				DrawVertB[0].u1 = ( (  XBuf[1] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
				DrawVertB[0].v1 = ( (  YBuf[1] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
				DrawVertB[1].u1 = ( (  XBuf[3] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
				DrawVertB[1].v1 = ( (  YBuf[3] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
				DrawVertB[2].u1 = ( ( -XBuf[3] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
				DrawVertB[2].v1 = ( ( -YBuf[3] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
				DrawVertB[3].u1 = ( ( -XBuf[1] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
				DrawVertB[3].v1 = ( ( -YBuf[1] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
				DrawVertB[4].u1 = ( ( -XBuf[3] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
				DrawVertB[4].v1 = ( ( -YBuf[3] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
				DrawVertB[5].u1 = ( (  XBuf[3] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
				DrawVertB[5].v1 = ( (  YBuf[3] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
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

			DrawVertB[0].rhw = 1.0f ;
			DrawVertB[1].rhw = 1.0f ;
			DrawVertB[2].rhw = 1.0f ;
			DrawVertB[3].rhw = 1.0f ;
			DrawVertB[4].rhw = 1.0f ;
			DrawVertB[5].rhw = 1.0f ;

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
				Graphics_D3D11_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D11.Texture, DrawTex->Tex->PF->D3D11.TextureSRV ) ;
				Graphics_D3D11_DrawSetting_SetBlendTexture( BlendDrawTex->Tex->PF->D3D11.Texture, BlendDrawTex->Tex->PF->D3D11.TextureSRV, BlendDrawTex->Tex->TexWidth, BlendDrawTex->Tex->TexHeight );

				if( GD3D11.Device.DrawSetting.ChangeTextureFlag )
				{
					Graphics_D3D11_DeviceState_NormalDrawSetup() ;
				}

				// 頂点データを初期化する
				GETVERTEX_QUAD( DrawVertB )

				TexVert = DrawTex->Vertex ;
				BlendTexVert = BlendDrawTex->Vertex ;

				DrawVertB[0].color = ( DWORD )DiffuseColor ;
				DrawVertB[1].color = ( DWORD )DiffuseColor ;
				DrawVertB[2].color = ( DWORD )DiffuseColor ;
				DrawVertB[3].color = ( DWORD )DiffuseColor ;
				DrawVertB[4].color = ( DWORD )DiffuseColor ;
				DrawVertB[5].color = ( DWORD )DiffuseColor ;

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
					u[ 0 ] = ( TexVert[0].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					u[ 1 ] = ( TexVert[1].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					u[ 2 ] = ( TexVert[2].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					u[ 3 ] = ( TexVert[3].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					
					v[ 0 ] = ( TexVert[0].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					v[ 1 ] = ( TexVert[1].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					v[ 2 ] = ( TexVert[2].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					v[ 3 ] = ( TexVert[3].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;

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
					DrawVertB[0].u1 = ( ( XBuf[1] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[0].v1 = ( ( YBuf[1] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[1].u1 = ( ( XBuf[3] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[1].v1 = ( ( YBuf[3] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[2].u1 = ( ( XBuf[5] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[2].v1 = ( ( YBuf[5] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[3].u1 = ( ( XBuf[7] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[3].v1 = ( ( YBuf[7] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[4].u1 = ( ( XBuf[5] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[4].v1 = ( ( YBuf[5] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[5].u1 = ( ( XBuf[3] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[5].v1 = ( ( YBuf[3] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
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

				DrawVertB[0].rhw = 1.0f ;
				DrawVertB[1].rhw = 1.0f ;
				DrawVertB[2].rhw = 1.0f ;
				DrawVertB[3].rhw = 1.0f ;
				DrawVertB[4].rhw = 1.0f ;
				DrawVertB[5].rhw = 1.0f ;

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
				DrawVert[1].color = ( DWORD )DiffuseColor ;
				DrawVert[2].color = ( DWORD )DiffuseColor ;
				DrawVert[3].color = ( DWORD )DiffuseColor ;
				DrawVert[4].color = ( DWORD )DiffuseColor ;
				DrawVert[5].color = ( DWORD )DiffuseColor ;

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
				YBuf[0] = ( TexVert[0].y - CenY ) * ExtendRateY  ;
				XBuf[1] = XBuf[0] * Cos - YBuf[0] * Sin ;
				YBuf[1] = XBuf[0] * Sin + YBuf[0] * Cos ;
				
				XBuf[2] = ( TexVert[1].x - CenX ) * ExtendRateX ;
				YBuf[2] = ( TexVert[1].y - CenY ) * ExtendRateY  ;
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

				DrawVert[0].rhw = 1.0f ;
				DrawVert[1].rhw = 1.0f ;
				DrawVert[2].rhw = 1.0f ;
				DrawVert[3].rhw = 1.0f ;
				DrawVert[4].rhw = 1.0f ;
				DrawVert[5].rhw = 1.0f ;

				// テクスチャーを描画する
				ADD4VERTEX_TEX
			}
			else
			{
				// 頂点データを初期化する
				GETVERTEX_QUAD( DrawVert )
				
				TexVert = DrawTex->Vertex ;

				DrawVert[0].color = ( DWORD )DiffuseColor ;
				DrawVert[1].color = ( DWORD )DiffuseColor ;
				DrawVert[2].color = ( DWORD )DiffuseColor ;
				DrawVert[3].color = ( DWORD )DiffuseColor ;
				DrawVert[4].color = ( DWORD )DiffuseColor ;
				DrawVert[5].color = ( DWORD )DiffuseColor ;

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

				DrawVert[0].rhw = 1.0f ;
				DrawVert[1].rhw = 1.0f ;
				DrawVert[2].rhw = 1.0f ;
				DrawVert[3].rhw = 1.0f ;
				DrawVert[4].rhw = 1.0f ;
				DrawVert[5].rhw = 1.0f ;

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
					DrawVert[1].color = ( DWORD )DiffuseColor ;
					DrawVert[2].color = ( DWORD )DiffuseColor ;
					DrawVert[3].color = ( DWORD )DiffuseColor ;
					DrawVert[4].color = ( DWORD )DiffuseColor ;
					DrawVert[5].color = ( DWORD )DiffuseColor ;

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
					YBuf[0] = ( TexVert[0].y - CenY ) * ExtendRateY  ;
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

					DrawVert[0].rhw = 1.0f ;
					DrawVert[1].rhw = 1.0f ;
					DrawVert[2].rhw = 1.0f ;
					DrawVert[3].rhw = 1.0f ;
					DrawVert[4].rhw = 1.0f ;
					DrawVert[5].rhw = 1.0f ;

					// テクスチャーを描画する
					ADD4VERTEX_TEX

					i ++ ;
					if( i >= DrawTexNum ) break ;

					// 描画するテクスチャーのセット
					DrawTex ++ ;
					Graphics_D3D11_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D11.Texture, DrawTex->Tex->PF->D3D11.TextureSRV ) ;
					Graphics_D3D11_DeviceState_NormalDrawSetup() ;
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
					DrawVert[1].color = ( DWORD )DiffuseColor ;
					DrawVert[2].color = ( DWORD )DiffuseColor ;
					DrawVert[3].color = ( DWORD )DiffuseColor ;
					DrawVert[4].color = ( DWORD )DiffuseColor ;
					DrawVert[5].color = ( DWORD )DiffuseColor ;

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

					DrawVert[0].rhw = 1.0f ;
					DrawVert[1].rhw = 1.0f ;
					DrawVert[2].rhw = 1.0f ;
					DrawVert[3].rhw = 1.0f ;
					DrawVert[4].rhw = 1.0f ;
					DrawVert[5].rhw = 1.0f ;

					// テクスチャーを描画する
					ADD4VERTEX_TEX

					i ++ ;
					if( i >= DrawTexNum ) break ;

					// 描画するテクスチャーのセット
					DrawTex ++ ;
					Graphics_D3D11_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D11.Texture, DrawTex->Tex->PF->D3D11.TextureSRV ) ;
					Graphics_D3D11_DeviceState_NormalDrawSetup() ;
				}
			}
		}
		break ;
	}

	// 終了
	return 0 ;
}

// ハードウエアアクセラレータ使用版 DrawRotaGraphFast
extern	int		Graphics_D3D11_DrawRotaGraphFast( int /*x*/, int /*y*/, float xf, float yf, float ExtendRate, float Angle, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, int ReverseXFlag, int ReverseYFlag, int /*IntFlag*/ )
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
	float XBuf[ 8 ] ;
	float YBuf[ 8 ] ;
	float u[ 4 ] ;
	float v[ 4 ] ;
	float ExtendRateX ;
	float ExtendRateY ;
	DWORD drawz;
//	BOOL Adj = FALSE ;
	float CenX, CenY ;
	int SinCosNone ;
	int Flag ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// 幅がなかった場合はエラー
	if( ExtendRate == 0.0f ) return -1 ;

	// 描画の準備
	Flag = TransFlag ;
	DX_D3D11_DRAWPREP_TEX( Image->Orig, Image->Hard.Draw[ 0 ].Tex->PF->D3D11.Texture, Image->Hard.Draw[ 0 ].Tex->PF->D3D11.TextureSRV, Flag )

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

	// 中心座標のセット
	CenX = ( float )Image->WidthF  * 0.5f ;
	CenY = ( float )Image->HeightF * 0.5f ;

	// 頂点データを取得（今のところ色情報のセットのみ）
	DiffuseColor = GD3D11.Device.DrawInfo.DiffuseColor ;

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
	switch( GD3D11.Device.DrawInfo.VertexType )
	{
	case VERTEXTYPE_BLENDTEX :	// ブレンドテクスチャを使用する
		// テクスチャーの数だけ繰り返す
		if( DrawTexNum == 1 )
		{
			// 描画するテクスチャーのセット
			Graphics_D3D11_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D11.Texture, DrawTex->Tex->PF->D3D11.TextureSRV ) ;
			Graphics_D3D11_DrawSetting_SetBlendTexture( BlendDrawTex->Tex->PF->D3D11.Texture, BlendDrawTex->Tex->PF->D3D11.TextureSRV, BlendDrawTex->Tex->TexWidth, BlendDrawTex->Tex->TexHeight );

			if( GD3D11.Device.DrawSetting.ChangeTextureFlag )
			{
				Graphics_D3D11_DeviceState_NormalDrawSetup() ;
			}

			// 頂点データを初期化する
			GETVERTEX_QUAD( DrawVertB )

			TexVert = DrawTex->Vertex ;
			BlendTexVert = BlendDrawTex->Vertex ;

			DrawVertB[0].color = ( DWORD )DiffuseColor ;
			DrawVertB[1].color = ( DWORD )DiffuseColor ;
			DrawVertB[2].color = ( DWORD )DiffuseColor ;
			DrawVertB[3].color = ( DWORD )DiffuseColor ;
			DrawVertB[4].color = ( DWORD )DiffuseColor ;
			DrawVertB[5].color = ( DWORD )DiffuseColor ;

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
				u[ 0 ] = TexVert[0].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
				u[ 1 ] = TexVert[1].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
				v[ 0 ] = TexVert[0].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
				v[ 1 ] = TexVert[2].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
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
				DrawVertB[0].u1 = ( (  XBuf[1] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
				DrawVertB[0].v1 = ( (  YBuf[1] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
				DrawVertB[1].u1 = ( (  XBuf[3] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
				DrawVertB[1].v1 = ( (  YBuf[3] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
				DrawVertB[2].u1 = ( ( -XBuf[3] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
				DrawVertB[2].v1 = ( ( -YBuf[3] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
				DrawVertB[3].u1 = ( ( -XBuf[1] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
				DrawVertB[3].v1 = ( ( -YBuf[1] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
				DrawVertB[4].u1 = ( ( -XBuf[3] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
				DrawVertB[4].v1 = ( ( -YBuf[3] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
				DrawVertB[5].u1 = ( (  XBuf[3] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
				DrawVertB[5].v1 = ( (  YBuf[3] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
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

			DrawVertB[0].rhw = 1.0f ;
			DrawVertB[1].rhw = 1.0f ;
			DrawVertB[2].rhw = 1.0f ;
			DrawVertB[3].rhw = 1.0f ;
			DrawVertB[4].rhw = 1.0f ;
			DrawVertB[5].rhw = 1.0f ;

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
				Graphics_D3D11_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D11.Texture, DrawTex->Tex->PF->D3D11.TextureSRV ) ;
				Graphics_D3D11_DrawSetting_SetBlendTexture( BlendDrawTex->Tex->PF->D3D11.Texture, BlendDrawTex->Tex->PF->D3D11.TextureSRV, BlendDrawTex->Tex->TexWidth, BlendDrawTex->Tex->TexHeight );

				if( GD3D11.Device.DrawSetting.ChangeTextureFlag )
				{
					Graphics_D3D11_DeviceState_NormalDrawSetup() ;
				}

				// 頂点データを初期化する
				GETVERTEX_QUAD( DrawVertB )

				TexVert = DrawTex->Vertex ;
				BlendTexVert = BlendDrawTex->Vertex ;

				DrawVertB[0].color = ( DWORD )DiffuseColor ;
				DrawVertB[1].color = ( DWORD )DiffuseColor ;
				DrawVertB[2].color = ( DWORD )DiffuseColor ;
				DrawVertB[3].color = ( DWORD )DiffuseColor ;
				DrawVertB[4].color = ( DWORD )DiffuseColor ;
				DrawVertB[5].color = ( DWORD )DiffuseColor ;

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
					u[ 0 ] = ( TexVert[0].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					u[ 1 ] = ( TexVert[1].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					u[ 2 ] = ( TexVert[2].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					u[ 3 ] = ( TexVert[3].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					
					v[ 0 ] = ( TexVert[0].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					v[ 1 ] = ( TexVert[1].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					v[ 2 ] = ( TexVert[2].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					v[ 3 ] = ( TexVert[3].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;

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
					DrawVertB[0].u1 = ( ( XBuf[1] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[0].v1 = ( ( YBuf[1] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[1].u1 = ( ( XBuf[3] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[1].v1 = ( ( YBuf[3] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[2].u1 = ( ( XBuf[5] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[2].v1 = ( ( YBuf[5] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[3].u1 = ( ( XBuf[7] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[3].v1 = ( ( YBuf[7] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[4].u1 = ( ( XBuf[5] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[4].v1 = ( ( YBuf[5] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[5].u1 = ( ( XBuf[3] + xf ) - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[5].v1 = ( ( YBuf[3] + yf ) - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
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

				DrawVertB[0].rhw = 1.0f ;
				DrawVertB[1].rhw = 1.0f ;
				DrawVertB[2].rhw = 1.0f ;
				DrawVertB[3].rhw = 1.0f ;
				DrawVertB[4].rhw = 1.0f ;
				DrawVertB[5].rhw = 1.0f ;

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
				DrawVert[1].color = ( DWORD )DiffuseColor ;
				DrawVert[2].color = ( DWORD )DiffuseColor ;
				DrawVert[3].color = ( DWORD )DiffuseColor ;
				DrawVert[4].color = ( DWORD )DiffuseColor ;
				DrawVert[5].color = ( DWORD )DiffuseColor ;

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

				DrawVert[0].rhw = 1.0f ;
				DrawVert[1].rhw = 1.0f ;
				DrawVert[2].rhw = 1.0f ;
				DrawVert[3].rhw = 1.0f ;
				DrawVert[4].rhw = 1.0f ;
				DrawVert[5].rhw = 1.0f ;

				// テクスチャーを描画する
				ADD4VERTEX_TEX
			}
			else
			{
				// 頂点データを初期化する
				GETVERTEX_QUAD( DrawVert )
				
				TexVert = DrawTex->Vertex ;

				DrawVert[0].color = ( DWORD )DiffuseColor ;
				DrawVert[1].color = ( DWORD )DiffuseColor ;
				DrawVert[2].color = ( DWORD )DiffuseColor ;
				DrawVert[3].color = ( DWORD )DiffuseColor ;
				DrawVert[4].color = ( DWORD )DiffuseColor ;
				DrawVert[5].color = ( DWORD )DiffuseColor ;

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

				DrawVert[0].rhw = 1.0f ;
				DrawVert[1].rhw = 1.0f ;
				DrawVert[2].rhw = 1.0f ;
				DrawVert[3].rhw = 1.0f ;
				DrawVert[4].rhw = 1.0f ;
				DrawVert[5].rhw = 1.0f ;

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
					DrawVert[1].color = ( DWORD )DiffuseColor ;
					DrawVert[2].color = ( DWORD )DiffuseColor ;
					DrawVert[3].color = ( DWORD )DiffuseColor ;
					DrawVert[4].color = ( DWORD )DiffuseColor ;
					DrawVert[5].color = ( DWORD )DiffuseColor ;

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

					DrawVert[0].rhw = 1.0f ;
					DrawVert[1].rhw = 1.0f ;
					DrawVert[2].rhw = 1.0f ;
					DrawVert[3].rhw = 1.0f ;
					DrawVert[4].rhw = 1.0f ;
					DrawVert[5].rhw = 1.0f ;

					// テクスチャーを描画する
					ADD4VERTEX_TEX

					i ++ ;
					if( i >= DrawTexNum ) break ;

					// 描画するテクスチャーのセット
					DrawTex ++ ;
					Graphics_D3D11_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D11.Texture, DrawTex->Tex->PF->D3D11.TextureSRV ) ;
					Graphics_D3D11_DeviceState_NormalDrawSetup() ;
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
					DrawVert[1].color = ( DWORD )DiffuseColor ;
					DrawVert[2].color = ( DWORD )DiffuseColor ;
					DrawVert[3].color = ( DWORD )DiffuseColor ;
					DrawVert[4].color = ( DWORD )DiffuseColor ;
					DrawVert[5].color = ( DWORD )DiffuseColor ;

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

					DrawVert[0].rhw = 1.0f ;
					DrawVert[1].rhw = 1.0f ;
					DrawVert[2].rhw = 1.0f ;
					DrawVert[3].rhw = 1.0f ;
					DrawVert[4].rhw = 1.0f ;
					DrawVert[5].rhw = 1.0f ;

					// テクスチャーを描画する
					ADD4VERTEX_TEX

					i ++ ;
					if( i >= DrawTexNum ) break ;

					// 描画するテクスチャーのセット
					DrawTex ++ ;
					Graphics_D3D11_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D11.Texture, DrawTex->Tex->PF->D3D11.TextureSRV ) ;
					Graphics_D3D11_DeviceState_NormalDrawSetup() ;
				}
			}
		}
		break ;
	}

	// 終了
	return 0 ;
}

// ハードウエアアクセラレータ使用版 DrawModiGraph
extern	int		Graphics_D3D11_DrawModiGraph( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, bool SimpleDrawFlag )
{
	if( x1 == x3 && x2 == x4 && y1 == y2 && y3 == y4 ) return Graphics_D3D11_DrawExtendGraph( x1, y1, x4, y4, ( float )x1, ( float )y1, ( float )x4, ( float )y4, Image, BlendImage, TransFlag, TRUE ) ; 

	// もしブレンドテクスチャを使用せず、旧型関数を使用するフラグも立っていなければ
	// 内部分割を行う描画関数を使用する
	if( GD3D11.Setting.UseOldDrawModiGraphCodeFlag == FALSE && GSYS.DrawSetting.BlendGraph <= 0 )
		return Graphics_D3D11_DrawModiGraphF( (float)x1, (float)y1, (float)x2, (float)y2, (float)x3, (float)y3, (float)x4, (float)y4, Image, BlendImage, TransFlag, SimpleDrawFlag ) ;

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
	float Reverse0 ;
//	float Reverse1 ;
	float ExRate1X1, ExRate1Y1, ExRate2X1, ExRate2Y1 ;
	float ExRate1X2, ExRate1Y2, ExRate2X2, ExRate2Y2 ;
	DWORD DiffuseColor ;
	VECTOR DrawPos[ 4 ] ;
	float u[ 4 ] ;
	float v[ 4 ] ;
	float  xf1, yf1, xf4, yf4;
	DWORD drawz ;
	int Flag ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// 描画の準備
	Flag = TransFlag | DX_D3D11_DRAWPREP_VECTORINT ;
	DX_D3D11_DRAWPREP_TEX( Image->Orig, Image->Hard.Draw[ 0 ].Tex->PF->D3D11.Texture, Image->Hard.Draw[ 0 ].Tex->PF->D3D11.TextureSRV, Flag )

	GraphSizeX = ( float )Image->WidthF ;  GraphSizeY = ( float )Image->HeightF ;
	ExRate1X1 = ( x2 - x1 ) /  GraphSizeX ;  ExRate1X2 = ( x3 - x1 ) /  GraphSizeY ;
	ExRate1Y1 = ( y3 - y1 ) /  GraphSizeY ;  ExRate1Y2 = ( y2 - y1 ) /  GraphSizeX ;
	ExRate2X1 = ( x3 - x4 ) / -GraphSizeX ;  ExRate2X2 = ( x2 - x4 ) / -GraphSizeY ;
	ExRate2Y1 = ( y2 - y4 ) / -GraphSizeY ;  ExRate2Y2 = ( y3 - y4 ) / -GraphSizeX ;
//	xf1 = (float)x1 - 0.5F ; yf1 = (float)y1 - 0.5F ;
//	xf4 = (float)x4 - 0.5F ; yf4 = (float)y4 - 0.5F ; 
	xf1 = (float)x1 ; yf1 = (float)y1 ;
	xf4 = (float)x4 ; yf4 = (float)y4 ; 

	// 頂点データを取得（今のところ色情報のセットのみ）
	DiffuseColor = GD3D11.Device.DrawInfo.DiffuseColor ;

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
	switch( GD3D11.Device.DrawInfo.VertexType )
	{
	case VERTEXTYPE_BLENDTEX :	// ブレンドテクスチャを使用する
		// テクスチャーの数だけ繰り返す
		for( i = 0 ; i < DrawTexNum ; i ++, DrawTex ++ )
		{
			// 描画するテクスチャーのセット
			Graphics_D3D11_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D11.Texture, DrawTex->Tex->PF->D3D11.TextureSRV ) ;
			Graphics_D3D11_DrawSetting_SetBlendTexture( BlendDrawTex->Tex->PF->D3D11.Texture, BlendDrawTex->Tex->PF->D3D11.TextureSRV, BlendDrawTex->Tex->TexWidth, BlendDrawTex->Tex->TexHeight );

			if( GD3D11.Device.DrawSetting.ChangeTextureFlag )
			{
				Graphics_D3D11_DeviceState_NormalDrawSetup() ;
			}

			// 頂点バッファの取得
			GETVERTEX_QUAD( DrawVertB )

			DrawVertB[0].color = ( DWORD )DiffuseColor ;
			DrawVertB[1].color = ( DWORD )DiffuseColor ;
			DrawVertB[2].color = ( DWORD )DiffuseColor ;
			DrawVertB[3].color = ( DWORD )DiffuseColor ;
			DrawVertB[4].color = ( DWORD )DiffuseColor ;
			DrawVertB[5].color = ( DWORD )DiffuseColor ;

			VertType     = DrawTex->VertType ;
			TexVert      = DrawTex->Vertex ;
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

				u[ j ] = ( TexVert->x - BlendTexVert->x + GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
				v[ j ] = ( TexVert->y - BlendTexVert->y + GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
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
					DrawVertB[ 0 ].u1 = ( DrawPos[ 0 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[ 0 ].v1 = ( DrawPos[ 0 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[ 1 ].u1 = ( DrawPos[ 2 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[ 1 ].v1 = ( DrawPos[ 2 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[ 2 ].u1 = ( DrawPos[ 1 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[ 2 ].v1 = ( DrawPos[ 1 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[ 3 ].u1 = ( DrawPos[ 3 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[ 3 ].v1 = ( DrawPos[ 3 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[ 4 ].u1 = ( DrawPos[ 1 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[ 4 ].v1 = ( DrawPos[ 1 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[ 5 ].u1 = ( DrawPos[ 2 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[ 5 ].v1 = ( DrawPos[ 2 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
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
					DrawVertB[ 0 ].u1 = ( DrawPos[ 0 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[ 0 ].v1 = ( DrawPos[ 0 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[ 1 ].u1 = ( DrawPos[ 1 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[ 1 ].v1 = ( DrawPos[ 1 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[ 2 ].u1 = ( DrawPos[ 2 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[ 2 ].v1 = ( DrawPos[ 2 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[ 3 ].u1 = ( DrawPos[ 3 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[ 3 ].v1 = ( DrawPos[ 3 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[ 4 ].u1 = ( DrawPos[ 2 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[ 4 ].v1 = ( DrawPos[ 2 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
					DrawVertB[ 5 ].u1 = ( DrawPos[ 1 ].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
					DrawVertB[ 5 ].v1 = ( DrawPos[ 1 ].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
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

			DrawVertB[0].rhw = 1.0f ;
			DrawVertB[1].rhw = 1.0f ;
			DrawVertB[2].rhw = 1.0f ;
			DrawVertB[3].rhw = 1.0f ;
			DrawVertB[4].rhw = 1.0f ;
			DrawVertB[5].rhw = 1.0f ;

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
			// 頂点バッファの取得
			GETVERTEX_QUAD( DrawVert )

			// 頂点データをセットする
			DrawVert[0].color = ( DWORD )DiffuseColor ;
			DrawVert[1].color = ( DWORD )DiffuseColor ;
			DrawVert[2].color = ( DWORD )DiffuseColor ;
			DrawVert[3].color = ( DWORD )DiffuseColor ;
			DrawVert[4].color = ( DWORD )DiffuseColor ;
			DrawVert[5].color = ( DWORD )DiffuseColor ;

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

			DrawVert[0].rhw = 1.0f ;
			DrawVert[1].rhw = 1.0f ;
			DrawVert[2].rhw = 1.0f ;
			DrawVert[3].rhw = 1.0f ;
			DrawVert[4].rhw = 1.0f ;
			DrawVert[5].rhw = 1.0f ;

			ADD4VERTEX_TEX

			i ++ ;
			if( i >= DrawTexNum ) break ;

			// 描画するテクスチャーのセット
			DrawTex ++ ;
			Graphics_D3D11_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D11.Texture, DrawTex->Tex->PF->D3D11.TextureSRV ) ;
			Graphics_D3D11_DeviceState_NormalDrawSetup() ;
		}
		break ;
	}

	// 終了
	return 0 ;
}

// ハードウエアアクセラレータ使用版 DrawModiGraphF
extern	int		Graphics_D3D11_DrawModiGraphF( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, bool SimpleDrawFlag )
{
	int Flag ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// もしブレンドテクスチャ付の場合は内部分割を行わない描画関数を使用する
	if( GSYS.DrawSetting.BlendGraph > 0 ) return Graphics_D3D11_DrawModiGraph( _FTOL( x1 ), _FTOL( y1 ), _FTOL( x2 ), _FTOL( y2 ), _FTOL( x3 ), _FTOL( y3 ), _FTOL( x4 ), _FTOL( y4 ), Image, BlendImage, TransFlag, SimpleDrawFlag ) ;

	// 描画の準備
	Flag = TransFlag ;
	DX_D3D11_DRAWPREP_TEX( Image->Orig, Image->Hard.Draw[ 0 ].Tex->PF->D3D11.Texture, Image->Hard.Draw[ 0 ].Tex->PF->D3D11.TextureSRV, Flag )

	// テクスチャーが１枚のみの場合は処理を分岐
	if( Image->Hard.DrawNum == 1 )
	{
		// そのまま描画
		Graphics_D3D11_DrawModiTex( x1, y1, x2, y2, x3, y3, x4, y4, &Image->Hard.Draw[ 0 ], false ) ;
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
		
//		w = 1.0F / (float)Image->WidthF ;
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
			Graphics_D3D11_DrawModiTex( x[0], y[0], x[1], y[1], x[2], y[2], x[3], y[3], DrawTex, false ) ;
		}
	}

	// 終了
	return 0 ;
}

// ２次元配列的に配置された頂点データを頂点バッファに追加する
static int D3D11_Graphics_SetPlaneVertexHardware( VERTEX_2D *GraphVert, int xnum, int ynum )
{
	int i, j, k, l ;

	if( GAPIWin.D3D11DeviceObject == NULL ) return 0 ;

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
				Graphics_D3D11_CommonBuffer_DrawIndexedPrimitive(
					D3D11_VERTEX_INPUTLAYOUT_2D,
					D_D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
					GraphVert,
					xnum * ynum,
					list,
					k,
					D_DXGI_FORMAT_R16_UINT
				) ;
					
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
			Graphics_D3D11_CommonBuffer_DrawIndexedPrimitive(
				D3D11_VERTEX_INPUTLAYOUT_2D,
				D_D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
				GraphVert,
				xnum * ynum,
				list,
				k,
				D_DXGI_FORMAT_R16_UINT
			) ;
		}
#undef	MAX_POSNUM
	}

	// 終了
	return 0 ;
}

// テクスチャを変形して描画する
extern	void	Graphics_D3D11_DrawModiTex( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, IMAGEDATA_HARD_DRAW *DrawTex, bool SimpleDrawFlag )
{
	IMAGEDATA_HARD_VERT *TexVert ;
	VERTEX_2D *vec ; 
	float xx1, yy1, xx2, yy2, xx3, yy3, xx4, yy4/*, r1, r2, r3, r4*/ ;
//	float xtmp1, ytmp1, xtmp2, ytmp2 ;
	int c, f1, f2 ;
	DWORD DiffuseColor ;
	float Reverse0, Reverse1;
	DWORD drawz ;
	float u[ 2 ] ;
	float v[ 2 ] ;

	// 描画するテクスチャーのセット
	Graphics_D3D11_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D11.Texture, DrawTex->Tex->PF->D3D11.TextureSRV ) ;
	if( GD3D11.Device.DrawSetting.ChangeTextureFlag )
	{
		Graphics_D3D11_DeviceState_NormalDrawSetup() ;
	}

	// 頂点データを取得（今のところ色情報のセットのみ）
	DiffuseColor = GD3D11.Device.DrawInfo.DiffuseColor ;

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
		VERTEX_2D *Vert2D ;
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
		Vert2D = VertData2 ;
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
										 xxx3 += adx3, yyy3 += ady3, Vert2D ++ )
			{
				Vert2D->pos.x = (float)xxx3 ;
				Vert2D->pos.y = (float)yyy3 ;
				*((DWORD *)&Vert2D->pos.z) = drawz ;
				Vert2D->rhw   = 1.0F ;
				Vert2D->u     = (float)tu ;
				Vert2D->v     = (float)tv ;
				Vert2D->color = DiffuseColor ;
			}
		}

		// 頂点追加処理
		D3D11_Graphics_SetPlaneVertexHardware( VertData2, tdn, tdn ) ;

#undef TDIVNUM
#undef TDN
	}

	return ;
		
R1 :
	// 普通に描画
	GETVERTEX_QUAD( vec )

	vec[0].color = ( DWORD )DiffuseColor ;
	vec[1].color = ( DWORD )DiffuseColor ;
	vec[2].color = ( DWORD )DiffuseColor ;
	vec[3].color = ( DWORD )DiffuseColor ;
	vec[4].color = ( DWORD )DiffuseColor ;
	vec[5].color = ( DWORD )DiffuseColor ;

	u[ 0 ] = TexVert[0].u + 0.0625f / DrawTex->Tex->TexWidth ;
	u[ 1 ] = TexVert[1].u - 0.0625f / DrawTex->Tex->TexWidth ;
	v[ 0 ] = TexVert[0].v + 0.0625f / DrawTex->Tex->TexHeight ;
	v[ 1 ] = TexVert[2].v - 0.0625f / DrawTex->Tex->TexHeight ;

	if( Reverse0 <= 0  &&  Reverse1 <= 0 )
	{
		vec[0].pos.x = x1 ;	
		vec[0].pos.y = y1 ;
		vec[1].pos.x = x3 ;
		vec[1].pos.y = y3 ;
		vec[2].pos.x = x2 ;	
		vec[2].pos.y = y2 ;
		vec[3].pos.x = x4 ;
		vec[3].pos.y = y4 ;
		vec[4].pos.x = x2 ;	
		vec[4].pos.y = y2 ;
		vec[5].pos.x = x3 ;
		vec[5].pos.y = y3 ;

		vec[0].u = u[ 0 ] ;
		vec[0].v = v[ 0 ] ;
		vec[1].u = u[ 0 ] ;
		vec[1].v = v[ 1 ] ;
		vec[2].u = u[ 1 ] ;
		vec[2].v = v[ 0 ] ;
		vec[3].u = u[ 1 ] ;
		vec[3].v = v[ 1 ] ;
		vec[4].u = u[ 1 ] ;
		vec[4].v = v[ 0 ] ;
		vec[5].u = u[ 0 ] ;
		vec[5].v = v[ 1 ] ;
	}
	else
	{
		vec[0].pos.x = x1 ;	
		vec[0].pos.y = y1 ;
		vec[1].pos.x = x2 ;	
		vec[1].pos.y = y2 ;
		vec[2].pos.x = x3 ;
		vec[2].pos.y = y3 ;
		vec[3].pos.x = x4 ;
		vec[3].pos.y = y4 ;
		vec[4].pos.x = x3 ;
		vec[4].pos.y = y3 ;
		vec[5].pos.x = x2 ;	
		vec[5].pos.y = y2 ;

		vec[0].u = u[ 0 ] ;
		vec[0].v = v[ 0 ] ;
		vec[1].u = u[ 1 ] ;
		vec[1].v = v[ 0 ] ;
		vec[2].u = u[ 0 ] ;
		vec[2].v = v[ 1 ] ;
		vec[3].u = u[ 1 ] ;
		vec[3].v = v[ 1 ] ;
		vec[4].u = u[ 0 ] ;
		vec[4].v = v[ 1 ] ;
		vec[5].u = u[ 1 ] ;
		vec[5].v = v[ 0 ] ;
	}

	*((DWORD *)&vec[0].pos.z) = drawz ;
	*((DWORD *)&vec[1].pos.z) = drawz ;
	*((DWORD *)&vec[2].pos.z) = drawz ;
	*((DWORD *)&vec[3].pos.z) = drawz ;
	*((DWORD *)&vec[4].pos.z) = drawz ;
	*((DWORD *)&vec[5].pos.z) = drawz ;

	vec[0].rhw = 1.0f ;
	vec[1].rhw = 1.0f ;
	vec[2].rhw = 1.0f ;
	vec[3].rhw = 1.0f ;
	vec[4].rhw = 1.0f ;
	vec[5].rhw = 1.0f ;

	ADD4VERTEX_TEX
}

// ハードウエアアクセラレータ使用版 DrawSimpleQuadrangleGraphF
extern	int		Graphics_D3D11_DrawSimpleQuadrangleGraphF( const GRAPHICS_DRAW_DRAWSIMPLEQUADRANGLEGRAPHF_PARAM *Param, IMAGEDATA *Image, IMAGEDATA *BlendImage )
{
	VERTEX_2D *DrawVert ;
	VERTEX_BLENDTEX_2D *DrawVertB ;
	DWORD DiffuseColor ;
	IMAGEDATA_HARD_DRAW *DrawTex ;
	IMAGEDATA_HARD_DRAW *BlendDrawTex = NULL ;
	IMAGEDATA_HARD_VERT *TexVert ;
	IMAGEDATA_HARD_VERT *BlendTexVert ;
	DWORD drawz;
	int Flag ;
	float u[ 4 ] ;
	float v[ 4 ] ;
	int i ;
	const GRAPHICS_DRAW_DRAWSIMPLEANGLEGRAPHF_VERTEX *ParamV ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// テクスチャの数が１以外の場合はエラー
	if( Image->Hard.DrawNum != 1 )
	{
		return -1 ;
	}

	// 描画の準備
	Flag = Param->TransFlag ;
	DX_D3D11_DRAWPREP_TEX( Image->Orig, Image->Hard.Draw[ 0 ].Tex->PF->D3D11.Texture, Image->Hard.Draw[ 0 ].Tex->PF->D3D11.TextureSRV, Flag )

	// 頂点データを取得（今のところ色情報のセットのみ）
	DiffuseColor = GD3D11.Device.DrawInfo.DiffuseColor ;

	// 描画情報配列のアドレスをセットしておく
	DrawTex = Image->Hard.Draw ;
	if( BlendImage != NULL )
	{
		BlendDrawTex = BlendImage->Hard.Draw ;
	}

	// Ｚバッファに書き込むＺ値をセットする
	drawz = *((DWORD *)&GSYS.DrawSetting.DrawZ);

	// 頂点タイプによって処理を分岐
	switch( GD3D11.Device.DrawInfo.VertexType )
	{
	case VERTEXTYPE_BLENDTEX :	// ブレンドテクスチャを使用する
		// 描画するテクスチャーのセット
		Graphics_D3D11_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D11.Texture, DrawTex->Tex->PF->D3D11.TextureSRV ) ;
		Graphics_D3D11_DrawSetting_SetBlendTexture( BlendDrawTex->Tex->PF->D3D11.Texture, BlendDrawTex->Tex->PF->D3D11.TextureSRV, BlendDrawTex->Tex->TexWidth, BlendDrawTex->Tex->TexHeight );

		if( GD3D11.Device.DrawSetting.ChangeTextureFlag )
		{
			Graphics_D3D11_DeviceState_NormalDrawSetup() ;
		}

		TexVert = DrawTex->Vertex ;
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
				u[ 0 ] = ( TexVert[0].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
				v[ 0 ] = ( TexVert[0].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
				u[ 1 ] = ( TexVert[1].x - BlendTexVert[1].x + GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
				v[ 1 ] = ( TexVert[1].y - BlendTexVert[1].y + GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
				u[ 2 ] = ( TexVert[2].x - BlendTexVert[2].x + GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
				v[ 2 ] = ( TexVert[2].y - BlendTexVert[2].y + GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
				u[ 3 ] = ( TexVert[3].x - BlendTexVert[3].x + GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
				v[ 3 ] = ( TexVert[3].y - BlendTexVert[3].y + GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
				break ;

			case DX_BLENDGRAPH_POSMODE_SCREEN :
				u[ 0 ] = ( ParamV[0].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
				v[ 0 ] = ( ParamV[0].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
				u[ 1 ] = ( ParamV[1].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
				v[ 1 ] = ( ParamV[1].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
				u[ 2 ] = ( ParamV[2].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
				v[ 2 ] = ( ParamV[2].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
				u[ 3 ] = ( ParamV[3].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
				v[ 3 ] = ( ParamV[3].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
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

			DrawVert[0].rhw = 1.0f ;
			DrawVert[1].rhw = 1.0f ;
			DrawVert[2].rhw = 1.0f ;
			DrawVert[3].rhw = 1.0f ;
			DrawVert[4].rhw = 1.0f ;
			DrawVert[5].rhw = 1.0f ;

			// テクスチャーを描画する
			ADD4VERTEX_TEX
		}
		break ;
	}

	// 終了
	return 0 ;
}

// ハードウエアアクセラレータ使用版 DrawSimpleTriangleGraphF
extern	int		Graphics_D3D11_DrawSimpleTriangleGraphF( const GRAPHICS_DRAW_DRAWSIMPLETRIANGLEGRAPHF_PARAM *Param, IMAGEDATA *Image, IMAGEDATA *BlendImage )
{
	VERTEX_2D *DrawVert ;
	VERTEX_BLENDTEX_2D *DrawVertB ;
	DWORD DiffuseColor ;
	IMAGEDATA_HARD_DRAW *DrawTex ;
	IMAGEDATA_HARD_DRAW *BlendDrawTex = NULL ;
	IMAGEDATA_HARD_VERT *TexVert ;
	IMAGEDATA_HARD_VERT *BlendTexVert ;
	DWORD drawz;
	int Flag ;
	float u[ 3 ] ;
	float v[ 3 ] ;
	int i ;
	const GRAPHICS_DRAW_DRAWSIMPLEANGLEGRAPHF_VERTEX *ParamV ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// テクスチャの数が１以外の場合はエラー
	if( Image->Hard.DrawNum != 1 )
	{
		return -1 ;
	}

	// 描画の準備
	Flag = Param->TransFlag ;
	DX_D3D11_DRAWPREP_TEX( Image->Orig, Image->Hard.Draw[ 0 ].Tex->PF->D3D11.Texture, Image->Hard.Draw[ 0 ].Tex->PF->D3D11.TextureSRV, Flag )

	// 頂点データを取得（今のところ色情報のセットのみ）
	DiffuseColor = GD3D11.Device.DrawInfo.DiffuseColor ;

	// 描画情報配列のアドレスをセットしておく
	DrawTex = Image->Hard.Draw ;
	if( BlendImage != NULL )
	{
		BlendDrawTex = BlendImage->Hard.Draw ;
	}

	// Ｚバッファに書き込むＺ値をセットする
	drawz = *((DWORD *)&GSYS.DrawSetting.DrawZ);

	// 頂点タイプによって処理を分岐
	switch( GD3D11.Device.DrawInfo.VertexType )
	{
	case VERTEXTYPE_BLENDTEX :	// ブレンドテクスチャを使用する
		// 描画するテクスチャーのセット
		Graphics_D3D11_DrawSetting_SetTexture( DrawTex->Tex->PF->D3D11.Texture, DrawTex->Tex->PF->D3D11.TextureSRV ) ;
		Graphics_D3D11_DrawSetting_SetBlendTexture( BlendDrawTex->Tex->PF->D3D11.Texture, BlendDrawTex->Tex->PF->D3D11.TextureSRV, BlendDrawTex->Tex->TexWidth, BlendDrawTex->Tex->TexHeight );

		if( GD3D11.Device.DrawSetting.ChangeTextureFlag )
		{
			Graphics_D3D11_DeviceState_NormalDrawSetup() ;
		}

		TexVert = DrawTex->Vertex ;
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
				u[ 0 ] = ( TexVert[0].x - BlendTexVert[0].x + GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
				v[ 0 ] = ( TexVert[0].y - BlendTexVert[0].y + GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
				u[ 1 ] = ( TexVert[1].x - BlendTexVert[1].x + GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
				v[ 1 ] = ( TexVert[1].y - BlendTexVert[1].y + GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
				u[ 2 ] = ( TexVert[2].x - BlendTexVert[2].x + GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
				v[ 2 ] = ( TexVert[2].y - BlendTexVert[2].y + GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
				break ;

			case DX_BLENDGRAPH_POSMODE_SCREEN :
				u[ 0 ] = ( ParamV[0].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
				v[ 0 ] = ( ParamV[0].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
				u[ 1 ] = ( ParamV[1].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
				v[ 1 ] = ( ParamV[1].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
				u[ 2 ] = ( ParamV[2].x - BlendTexVert[0].x - GSYS.DrawSetting.BlendGraphX ) * GD3D11.Device.DrawSetting.InvBlendTextureWidth ;
				v[ 2 ] = ( ParamV[2].y - BlendTexVert[0].y - GSYS.DrawSetting.BlendGraphY ) * GD3D11.Device.DrawSetting.InvBlendTextureHeight ;
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

			DrawVert[0].rhw = 1.0f ;
			DrawVert[1].rhw = 1.0f ;
			DrawVert[2].rhw = 1.0f ;

			// テクスチャーを描画する
			ADD3VERTEX_TEX
		}
		break ;
	}

	// 終了
	return 0 ;
}

// ハードウエアアクセラレータ使用版 DrawFillBox
extern	int		Graphics_D3D11_DrawFillBox( int x1, int y1, int x2, int y2, unsigned int Color )
{
	VERTEX_NOTEX_2D *vec ;
	RECT drect ;
	int Red, Green, Blue, Flag ;
	DWORD drawz;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

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
	Flag = DX_D3D11_DRAWPREP_DIFFUSERGB ;
	DX_D3D11_DRAWPREP_NOTEX( Flag ) ;

	// 色その他ステータスのセット
	NS_GetColor2( Color, &Red, &Green, &Blue ) ;
	SETUPCOLOR( Color )

	// Ｚバッファに書き込むＺ値をセットする
	drawz = *((DWORD *)&GSYS.DrawSetting.DrawZ);

	// 頂点データのセット
	GETVERTEX_QUAD( vec )

	vec[0].color = Color ;
	vec[1].color = Color ;
	vec[2].color = Color ;
	vec[3].color = Color ;
	vec[4].color = Color ;
	vec[5].color = Color ;
	
	vec[0].pos.x = ( float )drect.left   ;
	vec[0].pos.y = ( float )drect.top    ;
	vec[1].pos.x = ( float )drect.right  ;
	vec[1].pos.y = ( float )drect.top    ;
	vec[2].pos.x = ( float )drect.left   ;
	vec[2].pos.y = ( float )drect.bottom ;
	vec[3].pos.x = ( float )drect.right  ;
	vec[3].pos.y = ( float )drect.bottom ;
	vec[4].pos.x = ( float )drect.left   ;
	vec[4].pos.y = ( float )drect.bottom ;
	vec[5].pos.x = ( float )drect.right  ;
	vec[5].pos.y = ( float )drect.top    ;

	*((DWORD *)&vec[0].pos.z) = drawz ;
	*((DWORD *)&vec[1].pos.z) = drawz ;
	*((DWORD *)&vec[2].pos.z) = drawz ;
	*((DWORD *)&vec[3].pos.z) = drawz ;
	*((DWORD *)&vec[4].pos.z) = drawz ;
	*((DWORD *)&vec[5].pos.z) = drawz ;

	vec[0].rhw = 1.0f ;
	vec[1].rhw = 1.0f ;
	vec[2].rhw = 1.0f ;
	vec[3].rhw = 1.0f ;
	vec[4].rhw = 1.0f ;
	vec[5].rhw = 1.0f ;

	// 頂点データの出力
	ADD4VERTEX_NOTEX

	// 終了
	return 0 ;
}

// ハードウエアアクセラレータ使用版 DrawLineBox
extern	int		Graphics_D3D11_DrawLineBox( int x1, int y1, int x2, int y2, unsigned int Color )
{
	VERTEX_NOTEX_2D *VertData ;
	int Red, Green, Blue ;
	float fx1, fx2, fy1, fy2 ;
	int Flag;
	DWORD drawz ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// 描画の準備
	Flag = DX_D3D11_DRAWPREP_DIFFUSERGB ;
	DX_D3D11_DRAWPREP_NOTEX( Flag ) ;

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

	VertData[ 0].rhw = 1.0f ;
	VertData[ 1].rhw = 1.0f ;
	VertData[ 2].rhw = 1.0f ;
	VertData[ 3].rhw = 1.0f ;
	VertData[ 4].rhw = 1.0f ;
	VertData[ 5].rhw = 1.0f ;
	VertData[ 6].rhw = 1.0f ;
	VertData[ 7].rhw = 1.0f ;
	VertData[ 8].rhw = 1.0f ;
	VertData[ 9].rhw = 1.0f ;
	VertData[10].rhw = 1.0f ;
	VertData[11].rhw = 1.0f ;
	VertData[12].rhw = 1.0f ;
	VertData[13].rhw = 1.0f ;
	VertData[14].rhw = 1.0f ;
	VertData[15].rhw = 1.0f ;
	VertData[16].rhw = 1.0f ;
	VertData[17].rhw = 1.0f ;
	VertData[18].rhw = 1.0f ;
	VertData[19].rhw = 1.0f ;
	VertData[20].rhw = 1.0f ;
	VertData[21].rhw = 1.0f ;
	VertData[22].rhw = 1.0f ;
	VertData[23].rhw = 1.0f ;

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
extern	int		Graphics_D3D11_DrawLine( int x1, int y1, int x2, int y2, unsigned int Color )
{
	VERTEX_NOTEX_2D *VertData ;
	int Red, Green, Blue, Flag ;
	DWORD drawz ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	} 

	if( x2 - x1 == 0 && y2 - y1 == 0 ) return 0 ; 

	// 描画の準備
	Flag = DX_D3D11_DRAWPREP_DIFFUSERGB ;
	DX_D3D11_DRAWPREP_NOTEX( Flag )

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
	VertData[ 0 ].rhw = 1.0f ;
	VertData[ 1 ].rhw = 1.0f ;

	// 座標をセット
	VertData[ 0 ].pos.x = ( float )( x1 + 0.5f ) ;
	VertData[ 0 ].pos.y = ( float )( y1 + 0.5f ) ;

	VertData[ 1 ].pos.x = ( float )( x2 + 0.5f ) ;
	VertData[ 1 ].pos.y = ( float )( y2 + 0.5f ) ;

	// 座標の補正
	if( x1 == x2 )
	{
		if( y1 < y2 )
		{
			VertData[ 0 ].pos.y -= 0.1f ;
			VertData[ 1 ].pos.y -= 0.1f ;
		}
		else
		{
			VertData[ 0 ].pos.y += 0.1f ;
			VertData[ 1 ].pos.y += 0.1f ;
		}
	}

	if( y1 == y2 )
	{
		if( x1 < x2 )
		{
			VertData[ 0 ].pos.x -= 0.1f ;
			VertData[ 1 ].pos.x -= 0.1f ;
		}
		else
		{
			VertData[ 0 ].pos.x += 0.1f ;
			VertData[ 1 ].pos.x += 0.1f ;
		}
	}

	// 頂点の追加
	ADD4VERTEX_LINE

	// 終了
	return 0 ;
}

// ハードウエアアクセラレータ使用版 DrawLine3D
extern	int		Graphics_D3D11_DrawLine3D( VECTOR Pos1, VECTOR Pos2, unsigned int Color, int DrawFlag, RECT *DrawArea )
{
	VERTEX_NOTEX_3D *VertData ;
	int Red, Green, Blue, Flag ;

	if( DrawFlag )
	{
		if( GAPIWin.D3D11DeviceObject == NULL )
		{
			return -1 ;
		}

		// 描画の準備
		Flag = DX_D3D11_DRAWPREP_DIFFUSERGB | DX_D3D11_DRAWPREP_3D | DX_D3D11_DRAWPREP_FOG ;
		DX_D3D11_DRAWPREP_NOTEX( Flag )

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



#define D3D11_CIRCLE_VERTEX_NUM			(2000)

#define DX_D3D11_CIRCLE_THICKNESS											\
	if( x1 < DrawRect.left      ) x1 = DrawRect.left ;						\
	if( x2 > DrawRect.right - 1 ) x2 = DrawRect.right - 1 ;					\
																			\
	VertBuf[ VertNum ].pos.x  = ( float )x1 + 0.1f ;						\
	VertBuf[ VertNum ].pos.y  = y1f + 0.1f ;								\
	VertBuf[ VertNum ].color = Color ;										\
	VertBuf[ VertNum ].rhw   = 1.0f ;										\
	*((DWORD *)&VertBuf[ VertNum ].pos.z) = drawz ;							\
	VertNum ++ ;															\
																			\
	VertBuf[ VertNum ].pos.x =  ( float )( x2 + 1 ) + 0.1f ;				\
	VertBuf[ VertNum ].pos.y =  y1f + 0.1f ;								\
	VertBuf[ VertNum ].color = Color ;										\
	VertBuf[ VertNum ].rhw   = 1.0f ;										\
	*((DWORD *)&VertBuf[ VertNum ].pos.z) = drawz ;							\
	VertNum  ++ ;															\
																			\
	if( VertNum >= D3D11_CIRCLE_VERTEX_NUM )								\
	{																		\
		Graphics_D3D11_CommonBuffer_DrawPrimitive( D3D11_VERTEX_INPUTLAYOUT_NOTEX_2D, D_D3D11_PRIMITIVE_TOPOLOGY_LINELIST, VertBuf, VertNum ) ;\
		VertNum = 0 ;														\
	}																		

// ハードウエアアクセラレータ使用版 DrawCircle( 太さ指定あり )
extern	int		Graphics_D3D11_DrawCircle_Thickness( int x, int y, int r, unsigned int Color, int Thickness )
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

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// 描画用メモリの確保
	if( Graphics_Other_AllocCommonBuffer( 0, sizeof( VERTEX_NOTEX_2D ) * D3D11_CIRCLE_VERTEX_NUM ) < 0 )
	{
		return -1 ;
	}
	VertBuf = ( VERTEX_NOTEX_2D * )GSYS.Resource.CommonBuffer[ 0 ] ;

	// 描画の準備
	Flag = DX_D3D11_DRAWPREP_DIFFUSERGB ;
	DX_D3D11_DRAWPREP_NOTEX( Flag )

	// Ｚバッファに書き込むＺ値をセットする
	drawz = *((DWORD *)&GSYS.DrawSetting.DrawZ);

	// 補正
	DrawRect = GSYS.DrawSetting.DrawArea ;

	NS_GetColor2( Color, &Red, &Green, &Blue ) ;
	SETUPCOLOR( Color )

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
			DX_D3D11_CIRCLE_THICKNESS
		}
		else
		{
			x1 = CirclePos[ i ][ 0 ] ;
			x2 = CirclePos[ i ][ 2 ] ;
			DX_D3D11_CIRCLE_THICKNESS

			x1 = CirclePos[ i ][ 3 ] ;
			x2 = CirclePos[ i ][ 1 ] ;
			DX_D3D11_CIRCLE_THICKNESS
		}
	}
	if( VertNum )
	{
		Graphics_D3D11_CommonBuffer_DrawPrimitive( D3D11_VERTEX_INPUTLAYOUT_NOTEX_2D, D_D3D11_PRIMITIVE_TOPOLOGY_LINELIST, VertBuf, VertNum ) ;
	}

	// 終了
	return 0 ;
}

// ハードウエアアクセラレータ使用版 DrawOval( 太さ指定あり )
extern	int		Graphics_D3D11_DrawOval_Thickness( int x, int y, int rx, int ry, unsigned int Color, int Thickness )
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

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// 描画用メモリの確保
	if( Graphics_Other_AllocCommonBuffer( 0, sizeof( VERTEX_NOTEX_2D ) * D3D11_CIRCLE_VERTEX_NUM ) < 0 )
	{
		return -1 ;
	}
	VertBuf = ( VERTEX_NOTEX_2D * )GSYS.Resource.CommonBuffer[ 0 ] ;

	// 描画の準備
	Flag = DX_D3D11_DRAWPREP_DIFFUSERGB ;
	DX_D3D11_DRAWPREP_NOTEX( Flag )

	// Ｚバッファに書き込むＺ値をセットする
	drawz = *((DWORD *)&GSYS.DrawSetting.DrawZ);

	// 補正
	DrawRect = GSYS.DrawSetting.DrawArea ;

	NS_GetColor2( Color, &Red, &Green, &Blue ) ;
	SETUPCOLOR( Color )

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
			DX_D3D11_CIRCLE_THICKNESS
		}
		else
		{
			x1 = CirclePos[ i ][ 0 ] ;
			x2 = CirclePos[ i ][ 2 ] ;
			DX_D3D11_CIRCLE_THICKNESS

			x1 = CirclePos[ i ][ 3 ] ;
			x2 = CirclePos[ i ][ 1 ] ;
			DX_D3D11_CIRCLE_THICKNESS
		}
	}
	if( VertNum )
	{
		Graphics_D3D11_CommonBuffer_DrawPrimitive( D3D11_VERTEX_INPUTLAYOUT_NOTEX_2D, D_D3D11_PRIMITIVE_TOPOLOGY_LINELIST, VertBuf, VertNum ) ;
	}

	// 終了
	return 0 ;
}




#define DX_D3D11_CIRCLE																	\
			if( !( ( x2 < DrawRect.left ) || ( x1 >= DrawRect.right  ) ||				\
			       ( y1 < DrawRect.top  ) || ( y1 >= DrawRect.bottom ) || LineDrawBuf[ y1 ] ) )		\
			{																			\
				if( x1 < DrawRect.left ) x1 = DrawRect.left ;							\
				if( x2 > DrawRect.right - 1 ) x2 = DrawRect.right - 1 ;					\
																						\
				VertBuf[ VertNum ].pos.x  = ( float )x1 + 0.1f ;						\
				VertBuf[ VertNum ].pos.y  = ( float )y1 + 0.2f ;						\
				VertBuf[ VertNum ].color = Color ;										\
				VertBuf[ VertNum ].rhw   = 1.0f ;										\
				*((DWORD *)&VertBuf[ VertNum ].pos.z) = drawz ;							\
				VertNum ++ ;															\
				LineDrawBuf[y1] = TRUE ;												\
																						\
				VertBuf[ VertNum ].pos.x =  ( float )( x2 + 1 ) + 0.1f ;				\
				VertBuf[ VertNum ].pos.y =  ( float )y1 + 0.2f ;						\
				VertBuf[ VertNum ].color = Color ;										\
				VertBuf[ VertNum ].rhw   = 1.0f ;										\
				*((DWORD *)&VertBuf[ VertNum ].pos.z) = drawz ;							\
				VertNum  ++ ;															\
																						\
				if( VertNum >= D3D11_CIRCLE_VERTEX_NUM )								\
				{																		\
					Graphics_D3D11_CommonBuffer_DrawPrimitive( D3D11_VERTEX_INPUTLAYOUT_NOTEX_2D, D_D3D11_PRIMITIVE_TOPOLOGY_LINELIST, VertBuf, VertNum ) ;\
					VertNum = 0 ;														\
				}																		\
			}

#define DX_D3D11_CIRCLE_PSET															\
			if( !( ( x1 < DrawRect.left ) || ( x1 >= DrawRect.right  ) ||				\
				   ( y1 < DrawRect.top  ) || ( y1 >= DrawRect.bottom ) ) )				\
			{																			\
				VertBuf[ VertNum ].pos.x  = ( float )x1 + 0.1f ;						\
				VertBuf[ VertNum ].pos.y  = ( float )y1 + 0.1f ;						\
				VertBuf[ VertNum ].color = Color ;										\
				VertBuf[ VertNum ].rhw   = 1.0f ;										\
				*((DWORD *)&VertBuf[ VertNum ].pos.z) = drawz ;							\
				VertNum ++ ;															\
																						\
				if( VertNum >= D3D11_CIRCLE_VERTEX_NUM )								\
				{																		\
					Graphics_D3D11_CommonBuffer_DrawPrimitive( D3D11_VERTEX_INPUTLAYOUT_NOTEX_2D, D_D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, VertBuf, VertNum ) ;\
					VertNum = 0 ;														\
				}																		\
			}

// ハードウエアアクセラレータ使用版 DrawCircle
extern	int		Graphics_D3D11_DrawCircle( int x, int y, int r, unsigned int Color, int FillFlag, int Rx_One_Minus, int Ry_One_Minus )
{
	VERTEX_NOTEX_2D *VertBuf ;
	int             VertNum ;
	BYTE            *LineDrawBuf ;
	int             Red ;
	int             Green ;
	int             Blue ;
	int             Flag ;
	RECT            DrawRect ;
	DWORD           drawz ;

	Rx_One_Minus = Rx_One_Minus ? 1 : 0 ;
	Ry_One_Minus = Ry_One_Minus ? 1 : 0 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// 描画用メモリの確保
	if( Graphics_Other_AllocCommonBuffer( 0, sizeof( VERTEX_NOTEX_2D ) * D3D11_CIRCLE_VERTEX_NUM ) < 0 )
	{
		return -1 ;
	}
	VertBuf = ( VERTEX_NOTEX_2D * )GSYS.Resource.CommonBuffer[ 0 ] ;

	// 描画の準備
	Flag = DX_D3D11_DRAWPREP_DIFFUSERGB ;
	DX_D3D11_DRAWPREP_NOTEX( Flag )

	// Ｚバッファに書き込むＺ値をセットする
	drawz = *((DWORD *)&GSYS.DrawSetting.DrawZ);

	// 補正
	DrawRect = GSYS.DrawSetting.DrawArea ;

	NS_GetColor2( Color, &Red, &Green, &Blue ) ;
	SETUPCOLOR( Color )

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
					DX_D3D11_CIRCLE ;
				}

				// 座標データを進める
				if( F >= 0 )
				{
					x2 = Dy + x - Rx_One_Minus ; x1 = -Dy + x ; y1 = Dx + y - Ry_One_Minus ;
					DX_D3D11_CIRCLE ;
					x2 = Dy + x - Rx_One_Minus ; x1 = -Dy + x ; y1 = -Dx + y;
					DX_D3D11_CIRCLE ;

					Dx -- ;
					F -= 4 * Dx ;
				}	

				Dy ++ ;
				F += 4 * Dy + 2 ;
			}

			while( Dx >= Dy )
			{
				x2 = Dx + x - Rx_One_Minus ; x1 = -Dx + x ; y1 = Dy + y - Ry_One_Minus ;
				DX_D3D11_CIRCLE ;
				x2 = Dx + x - Rx_One_Minus ; x1 = -Dx + x ; y1 = -Dy + y ;
				DX_D3D11_CIRCLE ;

				// 座標データを進める
				if( F >= 0 )
				{
					x2 = Dy + x - Rx_One_Minus ; x1 = -Dy + x ; y1 = Dx + y - Ry_One_Minus ;
					DX_D3D11_CIRCLE ;
					x2 = Dy + x - Rx_One_Minus ; x1 = -Dy + x ; y1 = -Dx + y ;
					DX_D3D11_CIRCLE ;

					Dx -- ;
					F -= 4 * Dx ;
				}

				Dy ++ ;
				F += 4 * Dy + 2 ;
			}
			if( VertNum )
			{
				Graphics_D3D11_CommonBuffer_DrawPrimitive( D3D11_VERTEX_INPUTLAYOUT_NOTEX_2D, D_D3D11_PRIMITIVE_TOPOLOGY_LINELIST, VertBuf, VertNum ) ;
			}
		}
		else
		{
			// 最初の点を描く
			{
				if( Ry_One_Minus == FALSE )
				{
					x1 = -Dx + x ; y1 = Dy + y ;
					DX_D3D11_CIRCLE_PSET ;
					x1 = Dx + x - Rx_One_Minus ;  
					DX_D3D11_CIRCLE_PSET ;
				}

				if( Rx_One_Minus == FALSE )
				{
					x1 = Dy + x ; y1 = Dx + y - Ry_One_Minus ;
					DX_D3D11_CIRCLE_PSET ;

					x1 = Dy + x ; y1 = -Dx + y;
					DX_D3D11_CIRCLE_PSET ;
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
				DX_D3D11_CIRCLE_PSET ;
				x1 = Dx + x - Rx_One_Minus ;  
				DX_D3D11_CIRCLE_PSET ;

				x1 = Dy + x - Rx_One_Minus ; y1 = Dx + y - Ry_One_Minus ;
				DX_D3D11_CIRCLE_PSET ;
				x1 = -Dy + x ; 
				DX_D3D11_CIRCLE_PSET ;
			
				x1 = Dx + x - Rx_One_Minus ; y1 = -Dy + y ;
				DX_D3D11_CIRCLE_PSET ;
				x1 = -Dx + x ; 
				DX_D3D11_CIRCLE_PSET ;

				x1 = Dy + x - Rx_One_Minus ; y1 = -Dx + y ;
				DX_D3D11_CIRCLE_PSET ;
				x1 = -Dy + x ; 
				DX_D3D11_CIRCLE_PSET ;

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
				Graphics_D3D11_CommonBuffer_DrawPrimitive( D3D11_VERTEX_INPUTLAYOUT_NOTEX_2D, D_D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, VertBuf, VertNum ) ;
			}
		}
	}

	// 終了
	return 0 ;
}

// ハードウエアアクセラレータ使用版 DrawOval
extern	int		Graphics_D3D11_DrawOval( int x, int y, int rx, int ry, unsigned int Color, int FillFlag, int Rx_One_Minus, int Ry_One_Minus )
{
	VERTEX_NOTEX_2D *VertBuf ;
	int VertNum ;
	BYTE *LineDrawBuf ;
//	VERTEX_NOTEX_2D *VertData ;
//	int i, j, r2 ;
//	float xd, x2, y2 ;
//	float d, yd ;
	int Red, Green, Blue, Flag ;
	RECT DrawRect ;
	DWORD drawz;

	Rx_One_Minus = Rx_One_Minus ? 1 : 0 ;
	Ry_One_Minus = Ry_One_Minus ? 1 : 0 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}
	if( !rx || !ry ) return -1 ;

	// 描画用メモリの確保
	if( Graphics_Other_AllocCommonBuffer( 0, sizeof( VERTEX_NOTEX_2D ) * D3D11_CIRCLE_VERTEX_NUM ) < 0 )
	{
		return -1 ;
	}
	VertBuf = ( VERTEX_NOTEX_2D * )GSYS.Resource.CommonBuffer[ 0 ] ;

	// 描画の準備
	Flag = DX_D3D11_DRAWPREP_DIFFUSERGB ;
	DX_D3D11_DRAWPREP_NOTEX( Flag )

	// 補正
	if( rx < 0 ) rx *= -1 ;
	if( ry < 0 ) ry *= -1 ;
	DrawRect = GSYS.DrawSetting.DrawArea ;

	NS_GetColor2( Color, &Red, &Green, &Blue ) ;
	SETUPCOLOR( Color )

	// Ｚバッファに書き込むＺ値をセットする
	drawz = *((DWORD *)&GSYS.DrawSetting.DrawZ);

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
						DX_D3D11_CIRCLE ;
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
						DX_D3D11_CIRCLE ;

						y1 = -Dy + y ;
						DX_D3D11_CIRCLE ;
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
				DX_D3D11_CIRCLE ;
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
						DX_D3D11_CIRCLE ;

						y1 = Dy + y - Ry_One_Minus ;
						DX_D3D11_CIRCLE ;

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
				Graphics_D3D11_CommonBuffer_DrawPrimitive( D3D11_VERTEX_INPUTLAYOUT_NOTEX_2D, D_D3D11_PRIMITIVE_TOPOLOGY_LINELIST, VertBuf, VertNum ) ;
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
				Df = 1 ;
		
				Dx = r ;
				Dy = 0;
				F = -2 * r + 1 + 2 * 1;
				H = -4 * r + 2 + 1;

				// 最初の点を描く
				{
					if( Ry_One_Minus == FALSE )
					{
						x1 = -Dx + x ; y1 = Dy + y ;
						DX_D3D11_CIRCLE_PSET ;
						x1 = Dx + x - Rx_One_Minus ;  
						DX_D3D11_CIRCLE_PSET ;
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
						DX_D3D11_CIRCLE_PSET ;
						x1 = Dx + x - Rx_One_Minus ;
						DX_D3D11_CIRCLE_PSET ;

						x1 = -Dx + x ; y1 = -Dy + y ;
						DX_D3D11_CIRCLE_PSET ;
						x1 = Dx + x - Rx_One_Minus ;
						DX_D3D11_CIRCLE_PSET ;
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
					DX_D3D11_CIRCLE_PSET ;

					x1 = -Dx + x ; y1 = Dy + y - Ry_One_Minus ;
					DX_D3D11_CIRCLE_PSET ;
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
						DX_D3D11_CIRCLE_PSET ;
						y1 = Dy + y - Ry_One_Minus ;  
						DX_D3D11_CIRCLE_PSET ;
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
						DX_D3D11_CIRCLE_PSET ;
						y1 = Dy + y - Ry_One_Minus ;
						DX_D3D11_CIRCLE_PSET ;

						y1 = -Dy + y ; x1 = -Dx + x ;
						DX_D3D11_CIRCLE_PSET ;
						y1 = Dy + y - Ry_One_Minus ;
						DX_D3D11_CIRCLE_PSET ;
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
					DX_D3D11_CIRCLE_PSET ;

					y1 = -Dy + y ; x1 = Dx + x - Rx_One_Minus ;
					DX_D3D11_CIRCLE_PSET ;
				}
			}
			if( VertNum )
			{
				Graphics_D3D11_CommonBuffer_DrawPrimitive( D3D11_VERTEX_INPUTLAYOUT_NOTEX_2D, D_D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, VertBuf, VertNum ) ;
			}
		}
	}

	// 終了
	return 0 ;
}

// ハードウエアアクセラレータ使用版 DrawTriangle
extern	int		Graphics_D3D11_DrawTriangle( int x1, int y1, int x2, int y2, int x3, int y3, unsigned int Color, int FillFlag )
{
	VERTEX_NOTEX_2D *vec ; 
	int Red, Green, Blue ;
	int Flag ;
	int sx1, sx2, sy1, sy2 ;
	DWORD drawz;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// 描画の準備
	Flag = DX_D3D11_DRAWPREP_DIFFUSERGB ;
	DX_D3D11_DRAWPREP_NOTEX( Flag )

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
		vec[ 1 ].color = Color ;
		vec[ 2 ].color = Color ;

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

		vec[ 0 ].rhw = 1.0f ;
		vec[ 1 ].rhw = 1.0f ;
		vec[ 2 ].rhw = 1.0f ;

		ADD3VERTEX_NOTEX
	}
	else
	{
		GETVERTEX_LINETRIANGLE( vec )

		vec[ 0 ].color = Color ;
		vec[ 1 ].color = Color ;
		vec[ 2 ].color = Color ;
		vec[ 3 ].color = Color ;
		vec[ 4 ].color = Color ;
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

		vec[ 0 ].rhw = 1.0f ;
		vec[ 1 ].rhw = 1.0f ;
		vec[ 2 ].rhw = 1.0f ;
		vec[ 3 ].rhw = 1.0f ;
		vec[ 4 ].rhw = 1.0f ;
		vec[ 5 ].rhw = 1.0f ;

		ADD4VERTEX_LINETRIANGLE
	}

	// 終了
	return 0 ;
}

// ハードウエアアクセラレータ使用版 DrawTriangle3D
extern	int		Graphics_D3D11_DrawTriangle3D( VECTOR Pos1, VECTOR Pos2, VECTOR Pos3, unsigned int Color, int FillFlag, int DrawFlag, RECT *DrawArea )
{
	VERTEX_NOTEX_3D *vec ; 
	int Red, Green, Blue ;
	int Flag ;

	if( DrawFlag )
	{
		if( GAPIWin.D3D11DeviceObject == NULL )
		{
			return -1 ;
		}

		// 描画の準備
		Flag = DX_D3D11_DRAWPREP_DIFFUSERGB | DX_D3D11_DRAWPREP_3D | DX_D3D11_DRAWPREP_FOG | DX_D3D11_DRAWPREP_CULLING ;
		DX_D3D11_DRAWPREP_NOTEX( Flag )

		// 色その他ステータスのセット
		NS_GetColor2( Color, &Red, &Green, &Blue ) ;
		SETUPCOLOR( Color )

		// ラインかどうかで処理を分岐
		if( FillFlag )
		{
			GETVERTEX_TRIANGLE3D( vec ) ;

			*( ( DWORD * )&vec[ 0 ].b ) = ( DWORD )Color ;
			*( ( DWORD * )&vec[ 1 ].b ) = ( DWORD )Color ;
			*( ( DWORD * )&vec[ 2 ].b ) = ( DWORD )Color ;
			vec[ 0 ].pos = Pos1 ;
			vec[ 1 ].pos = Pos2 ;
			vec[ 2 ].pos = Pos3 ;

			ADD3VERTEX_NOTEX3D
		}
		else
		{
			GETVERTEX_LINETRIANGLE3D( vec )

			*( ( DWORD * )&vec[ 0 ].b ) = ( DWORD )Color ;
			*( ( DWORD * )&vec[ 1 ].b ) = ( DWORD )Color ;
			*( ( DWORD * )&vec[ 2 ].b ) = ( DWORD )Color ;
			*( ( DWORD * )&vec[ 3 ].b ) = ( DWORD )Color ;
			*( ( DWORD * )&vec[ 4 ].b ) = ( DWORD )Color ;
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
extern	int		Graphics_D3D11_DrawQuadrangle( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int Color, int FillFlag )
{
	VERTEX_NOTEX_2D *vec ;
	int Red, Green, Blue ;
	int Flag ;
	DWORD drawz;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// 描画の準備
	Flag = DX_D3D11_DRAWPREP_DIFFUSERGB ;
	DX_D3D11_DRAWPREP_NOTEX( Flag )

	// Ｚバッファに書き込むＺ値をセットする
	drawz = *((DWORD *)&GSYS.DrawSetting.DrawZ);

	// 色その他ステータスのセット
	NS_GetColor2( Color, &Red, &Green, &Blue ) ;
	SETUPCOLOR( Color )

	// 塗りつぶしかどうかで処理を分岐
	if( FillFlag )
	{
		// 塗りつぶしの場合

		// 頂点データのセット
		GETVERTEX_QUAD( vec )

		vec[ 0 ].color = Color ;
		vec[ 1 ].color = Color ;
		vec[ 2 ].color = Color ;
		vec[ 3 ].color = Color ;
		vec[ 4 ].color = Color ;
		vec[ 5 ].color = Color ;
		
		vec[ 0 ].pos.x = ( float )x1 ; vec[ 0 ].pos.y = ( float )y1 ;
		vec[ 1 ].pos.x = ( float )x2 ; vec[ 1 ].pos.y = ( float )y2 ;
		vec[ 2 ].pos.x = ( float )x4 ; vec[ 2 ].pos.y = ( float )y4 ;
		vec[ 3 ].pos.x = ( float )x3 ; vec[ 3 ].pos.y = ( float )y3 ;
		vec[ 4 ].pos.x = ( float )x4 ; vec[ 4 ].pos.y = ( float )y4 ;
		vec[ 5 ].pos.x = ( float )x2 ; vec[ 5 ].pos.y = ( float )y2 ;

		*((DWORD *)&vec[ 0 ].pos.z) = drawz ;
		*((DWORD *)&vec[ 1 ].pos.z) = drawz ;
		*((DWORD *)&vec[ 2 ].pos.z) = drawz ;
		*((DWORD *)&vec[ 3 ].pos.z) = drawz ;
		*((DWORD *)&vec[ 4 ].pos.z) = drawz ;
		*((DWORD *)&vec[ 5 ].pos.z) = drawz ;

		vec[0].rhw = 1.0f ;
		vec[1].rhw = 1.0f ;
		vec[2].rhw = 1.0f ;
		vec[3].rhw = 1.0f ;
		vec[4].rhw = 1.0f ;
		vec[5].rhw = 1.0f ;

		// 頂点データの出力
		ADD4VERTEX_NOTEX
	}
	else
	{
		// 塗りつぶしでは無い場合

		GETVERTEX_LINE( vec ) ;

		vec[ 0 ].color = Color ;
		vec[ 1 ].color = Color ;

		*((DWORD *)&vec[ 0 ].pos.z) = drawz ;
		*((DWORD *)&vec[ 1 ].pos.z) = drawz ;

		vec[ 0 ].rhw = 1.0f ;
		vec[ 1 ].rhw = 1.0f ;

		vec[ 0 ].pos.x = ( float )( x1 ) ;
		vec[ 0 ].pos.y = ( float )( y1 ) ;

		vec[ 1 ].pos.x = ( float )( x2 ) ;
		vec[ 1 ].pos.y = ( float )( y2 ) ;

		// 頂点の追加
		ADD4VERTEX_LINE


		GETVERTEX_LINE( vec ) ;

		vec[ 0 ].color = Color ;
		vec[ 1 ].color = Color ;

		*((DWORD *)&vec[ 0 ].pos.z) = drawz ;
		*((DWORD *)&vec[ 1 ].pos.z) = drawz ;

		vec[ 0 ].rhw = 1.0f ;
		vec[ 1 ].rhw = 1.0f ;

		vec[ 0 ].pos.x = ( float )( x2 ) ;
		vec[ 0 ].pos.y = ( float )( y2 ) ;

		vec[ 1 ].pos.x = ( float )( x3 ) ;
		vec[ 1 ].pos.y = ( float )( y3 ) ;

		// 頂点の追加
		ADD4VERTEX_LINE


		GETVERTEX_LINE( vec ) ;

		vec[ 0 ].color = Color ;
		vec[ 1 ].color = Color ;

		*((DWORD *)&vec[ 0 ].pos.z) = drawz ;
		*((DWORD *)&vec[ 1 ].pos.z) = drawz ;

		vec[ 0 ].rhw = 1.0f ;
		vec[ 1 ].rhw = 1.0f ;

		vec[ 0 ].pos.x = ( float )( x3 ) ;
		vec[ 0 ].pos.y = ( float )( y3 ) ;

		vec[ 1 ].pos.x = ( float )( x4 ) ;
		vec[ 1 ].pos.y = ( float )( y4 ) ;

		// 頂点の追加
		ADD4VERTEX_LINE


		GETVERTEX_LINE( vec ) ;

		vec[ 0 ].color = Color ;
		vec[ 1 ].color = Color ;

		*((DWORD *)&vec[ 0 ].pos.z) = drawz ;
		*((DWORD *)&vec[ 1 ].pos.z) = drawz ;

		vec[ 0 ].rhw = 1.0f ;
		vec[ 1 ].rhw = 1.0f ;

		vec[ 0 ].pos.x = ( float )( x4 ) ;
		vec[ 0 ].pos.y = ( float )( y4 ) ;

		vec[ 1 ].pos.x = ( float )( x1 ) ;
		vec[ 1 ].pos.y = ( float )( y1 ) ;

		// 頂点の追加
		ADD4VERTEX_LINE
	}

	// 終了
	return 0 ;
}

// ハードウエアアクセラレータ使用版 DrawQuadrangle
extern	int		Graphics_D3D11_DrawQuadrangleF( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int Color, int FillFlag )
{
	VERTEX_NOTEX_2D *vec ;
	int Red, Green, Blue ;
	int Flag ;
	DWORD drawz;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// 描画の準備
	Flag = DX_D3D11_DRAWPREP_DIFFUSERGB ;
	DX_D3D11_DRAWPREP_NOTEX( Flag )

	// Ｚバッファに書き込むＺ値をセットする
	drawz = *((DWORD *)&GSYS.DrawSetting.DrawZ);

	// 色その他ステータスのセット
	NS_GetColor2( Color, &Red, &Green, &Blue ) ;
	SETUPCOLOR( Color )

	// 塗りつぶしかどうかで処理を分岐
	if( FillFlag )
	{
		// 塗りつぶしの場合

		// 頂点データのセット
		GETVERTEX_QUAD( vec )

		vec[ 0 ].color = Color ;
		vec[ 1 ].color = Color ;
		vec[ 2 ].color = Color ;
		vec[ 3 ].color = Color ;
		vec[ 4 ].color = Color ;
		vec[ 5 ].color = Color ;
		
		vec[ 0 ].pos.x = x1 ; vec[ 0 ].pos.y = y1 ;
		vec[ 1 ].pos.x = x2 ; vec[ 1 ].pos.y = y2 ;
		vec[ 2 ].pos.x = x4 ; vec[ 2 ].pos.y = y4 ;
		vec[ 3 ].pos.x = x3 ; vec[ 3 ].pos.y = y3 ;
		vec[ 4 ].pos.x = x4 ; vec[ 4 ].pos.y = y4 ;
		vec[ 5 ].pos.x = x2 ; vec[ 5 ].pos.y = y2 ;

		*((DWORD *)&vec[ 0 ].pos.z) = drawz ;
		*((DWORD *)&vec[ 1 ].pos.z) = drawz ;
		*((DWORD *)&vec[ 2 ].pos.z) = drawz ;
		*((DWORD *)&vec[ 3 ].pos.z) = drawz ;
		*((DWORD *)&vec[ 4 ].pos.z) = drawz ;
		*((DWORD *)&vec[ 5 ].pos.z) = drawz ;

		vec[0].rhw = 1.0f ;
		vec[1].rhw = 1.0f ;
		vec[2].rhw = 1.0f ;
		vec[3].rhw = 1.0f ;
		vec[4].rhw = 1.0f ;
		vec[5].rhw = 1.0f ;

		// 頂点データの出力
		ADD4VERTEX_NOTEX
	}
	else
	{
		// 塗りつぶしでは無い場合

		GETVERTEX_LINE( vec ) ;

		vec[ 0 ].color = Color ;
		vec[ 1 ].color = Color ;

		*((DWORD *)&vec[ 0 ].pos.z) = drawz ;
		*((DWORD *)&vec[ 1 ].pos.z) = drawz ;

		vec[ 0 ].rhw = 1.0f ;
		vec[ 1 ].rhw = 1.0f ;

		vec[ 0 ].pos.x = ( x1 ) ;
		vec[ 0 ].pos.y = ( y1 ) ;

		vec[ 1 ].pos.x = ( x2 ) ;
		vec[ 1 ].pos.y = ( y2 ) ;

		// 頂点の追加
		ADD4VERTEX_LINE


		GETVERTEX_LINE( vec ) ;

		vec[ 0 ].color = Color ;
		vec[ 1 ].color = Color ;

		*((DWORD *)&vec[ 0 ].pos.z) = drawz ;
		*((DWORD *)&vec[ 1 ].pos.z) = drawz ;

		vec[ 0 ].rhw = 1.0f ;
		vec[ 1 ].rhw = 1.0f ;

		vec[ 0 ].pos.x = ( x2 ) ;
		vec[ 0 ].pos.y = ( y2 ) ;

		vec[ 1 ].pos.x = ( x3 ) ;
		vec[ 1 ].pos.y = ( y3 ) ;

		// 頂点の追加
		ADD4VERTEX_LINE


		GETVERTEX_LINE( vec ) ;

		vec[ 0 ].color = Color ;
		vec[ 1 ].color = Color ;

		*((DWORD *)&vec[ 0 ].pos.z) = drawz ;
		*((DWORD *)&vec[ 1 ].pos.z) = drawz ;

		vec[ 0 ].rhw = 1.0f ;
		vec[ 1 ].rhw = 1.0f ;

		vec[ 0 ].pos.x = ( x3 ) ;
		vec[ 0 ].pos.y = ( y3 ) ;

		vec[ 1 ].pos.x = ( x4 ) ;
		vec[ 1 ].pos.y = ( y4 ) ;

		// 頂点の追加
		ADD4VERTEX_LINE


		GETVERTEX_LINE( vec ) ;

		vec[ 0 ].color = Color ;
		vec[ 1 ].color = Color ;

		*((DWORD *)&vec[ 0 ].pos.z) = drawz ;
		*((DWORD *)&vec[ 1 ].pos.z) = drawz ;

		vec[ 0 ].rhw = 1.0f ;
		vec[ 1 ].rhw = 1.0f ;

		vec[ 0 ].pos.x = ( x4 ) ;
		vec[ 0 ].pos.y = ( y4 ) ;

		vec[ 1 ].pos.x = ( x1 ) ;
		vec[ 1 ].pos.y = ( y1 ) ;

		// 頂点の追加
		ADD4VERTEX_LINE
	}

	// 終了
	return 0 ;
}

// ハードウエアアクセラレータ使用版 DrawPixel
extern	int		Graphics_D3D11_DrawPixel( int x, int y, unsigned int Color )
{
	VERTEX_NOTEX_2D *VertData ;
	int Red, Green, Blue, Flag ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ; 
	}

	// 描画の準備
	Flag = DX_D3D11_DRAWPREP_DIFFUSERGB ;
	DX_D3D11_DRAWPREP_NOTEX( Flag )

	// 色その他ステータスのセット
	GETVERTEX_POINT( VertData ) ;

	if( GD3D11.Device.DrawInfo.DiffuseColor == 0xffffffff )
	{
		NS_GetColor2( Color, &Red, &Green, &Blue ) ;
		if( GSYS.HardInfo.UseVertexColorBGRAFormat )
		{
			VertData->color = 0xff000000 | ( Red << 16 ) | ( Green << 8 ) | ( Blue ) ;
		}
		else
		{
			VertData->color = 0xff000000 | ( Red ) | ( Green << 8 ) | ( Blue << 16 ) ;
		}
	}
	else
	{
		NS_GetColor2( Color, &Red, &Green, &Blue ) ;
		SETUPCOLOR( VertData->color )
	}

	// 座標のセット
	VertData->pos.x = ( float )x + 0.1f ;
	VertData->pos.y = ( float )y + 0.1f ;
	VertData->pos.z = GSYS.DrawSetting.DrawZ ;
	VertData->rhw   = 1.0f ;

	// 頂点の追加
	ADD4VERTEX_POINT

	// 終了
	return 0 ;
}

// ハードウエアアクセラレータ使用版 DrawPixel3D
extern	int		Graphics_D3D11_DrawPixel3D( VECTOR Pos, unsigned int Color, int DrawFlag, RECT *DrawArea )
{
	VERTEX_NOTEX_3D *VertData ; 
	int Red, Green, Blue ;
	int Flag ;

	if( DrawFlag )
	{
		if( GAPIWin.D3D11DeviceObject == NULL )
		{
			return -1 ;
		}

		// 描画の準備
		Flag = DX_D3D11_DRAWPREP_DIFFUSERGB | DX_D3D11_DRAWPREP_3D | DX_D3D11_DRAWPREP_FOG ;
		DX_D3D11_DRAWPREP_NOTEX( Flag )

		// 色その他ステータスのセット
		GETVERTEX_POINT3D( VertData ) ;

		NS_GetColor2( Color, &Red, &Green, &Blue ) ;
		if( GD3D11.Device.DrawInfo.DiffuseColor == 0xffffffff )
		{
			if( GSYS.HardInfo.UseVertexColorBGRAFormat )
			{
				*( ( DWORD * )&VertData->b ) = 0xff000000 | ( Red << 16 ) | ( Green << 8 ) | ( Blue ) ;
			}
			else
			{
				*( ( DWORD * )&VertData->b ) = 0xff000000 | ( Red ) | ( Green << 8 ) | ( Blue << 16 ) ;
			}
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
extern	int		Graphics_D3D11_DrawPixelSet( const POINTDATA *PointData, int Num )
{
	VERTEX_NOTEX_2D *VertData ;
	int i ;
	COLORDATA ColorData ;
	int MaxRed, MaxGreen, MaxBlue ; 
	unsigned int Color ;
	int Flag ;
	DWORD drawz;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画の準備
	Flag = DX_D3D11_DRAWPREP_DIFFUSERGB ;
	DX_D3D11_DRAWPREP_NOTEX( Flag )

	// Ｚバッファに書き込むＺ値をセットする
	drawz = *((DWORD *)&GSYS.DrawSetting.DrawZ);

	// ベクトル基本ステータスを取得
	ColorData = *( ( COLORDATA * )NS_GetDispColorData() ) ;
	
	MaxRed		= ( 1 << ColorData.RedWidth	  ) - 1 ; 
	MaxGreen	= ( 1 << ColorData.GreenWidth ) - 1 ;
	MaxBlue		= ( 1 << ColorData.BlueWidth  ) - 1 ; 

	for( i = 0 ; i < Num ; i ++, PointData ++ )
	{
		GETVERTEX_POINT( VertData ) ;

		Color = PointData->color ;
		VertData->color = ( PointData->pal << 24 ) |
						( ( ( ( ( ( Color & ColorData.RedMask	) >> ColorData.RedLoc	) << 8 ) - 1 ) / MaxRed		)       ) |
						( ( ( ( ( ( Color & ColorData.GreenMask	) >> ColorData.GreenLoc	) << 8 ) - 1 ) / MaxGreen	) << 8  ) |
						( ( ( ( ( ( Color & ColorData.BlueMask	) >> ColorData.BlueLoc	) << 8 ) - 1 ) / MaxBlue	) << 16 ) ; 

		VertData->pos.x = ( float )PointData->x + 0.1f ;
		VertData->pos.y = ( float )PointData->y + 0.1f ;
		*((DWORD *)&VertData->pos.z) = drawz ;
		VertData->rhw    = 1.0f ;

		// 頂点の追加
		ADD4VERTEX_POINT
	}

	// 終了
	return 0;
}

// ハードウエアアクセラレータ使用版 DrawLineSet
extern	int		Graphics_D3D11_DrawLineSet( const LINEDATA *LineData, int Num )
{
	int i ;
	VERTEX_NOTEX_2D *VertData ;
	COLORDATA ColorData ;
	int MaxRed, MaxGreen, MaxBlue ; 
	unsigned int Color ;
	int Flag ;
	DWORD drawz;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画の準備
	Flag = DX_D3D11_DRAWPREP_DIFFUSERGB ;
	DX_D3D11_DRAWPREP_NOTEX( Flag )

	// Ｚバッファに書き込むＺ値をセットする
	drawz = *((DWORD *)&GSYS.DrawSetting.DrawZ);

	// ベクトル基本ステータスを取得
	ColorData = *( ( COLORDATA * )NS_GetDispColorData() ) ;
	
	MaxRed		= ( 1 << ColorData.RedWidth	) - 1 ; 
	MaxGreen	= ( 1 << ColorData.GreenWidth ) - 1 ;
	MaxBlue		= ( 1 << ColorData.BlueWidth ) - 1 ; 

	for( i = 0 ; i < Num ; i ++, LineData ++ )
	{
		GETVERTEX_LINE( VertData ) ;

		Color = LineData->color ;
		VertData->color = ( LineData->pal << 24 ) |
						( ( ( ( ( ( Color & ColorData.RedMask	) >> ColorData.RedLoc	) << 8 ) - 1 ) / MaxRed		)       ) |
						( ( ( ( ( ( Color & ColorData.GreenMask	) >> ColorData.GreenLoc	) << 8 ) - 1 ) / MaxGreen	) << 8  ) |
						( ( ( ( ( ( Color & ColorData.BlueMask	) >> ColorData.BlueLoc	) << 8 ) - 1 ) / MaxBlue	) << 16 ) ; 
		VertData[1].color = VertData[0].color ;
		*((DWORD *)&VertData[0].pos.z) = drawz ;
		*((DWORD *)&VertData[1].pos.z) = drawz ;
		VertData[0].rhw   = 1.0f ;
		VertData[1].rhw   = 1.0f ;
 
		// 座標のセット＆ウインドウ直接描画時補正
		VertData[0].pos.x = ( float )LineData->x1 ;
		VertData[0].pos.y = ( float )LineData->y1 ;
		VertData[1].pos.x = ( float )LineData->x2 ;
		VertData[1].pos.y = ( float )LineData->y2 ;

		// 座標の補正
		if( LineData->x1 == LineData->x2 ) VertData[ 1 ].pos.y += LineData->y2 > LineData->y1 ? -0.1F : 0.1F ;
		if( LineData->y1 == LineData->y2 ) VertData[ 1 ].pos.x += LineData->x2 > LineData->x1 ? -0.1F : 0.1F ;

		// 頂点の追加
		ADD4VERTEX_LINE
	}

	// 終了
	return 0 ;
}














// DrawPrimitive3D 系の共通の準備処理を行う関数
__inline int Graphics_D3D11_DrawPrimitive3DPreparation( int AddFlag, IMAGEDATA *Image, int TransFlag, int TextureNo = 0 )
{
	int Flag ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画の準備
	Flag = TransFlag | DX_D3D11_DRAWPREP_3D | DX_D3D11_DRAWPREP_FOG | DX_D3D11_DRAWPREP_TEXADDRESS | DX_D3D11_DRAWPREP_CULLING | AddFlag ;
	if( Image )
	{
		DX_D3D11_DRAWPREP_TEX( Image->Orig, Image->Hard.Draw[ TextureNo ].Tex->PF->D3D11.Texture, Image->Hard.Draw[ TextureNo ].Tex->PF->D3D11.TextureSRV, Flag )
	}
	else
	{
		int WhiteHandle ;

		WhiteHandle = Graphics_Image_GetWhiteTexHandle() ;
		if( GRAPHCHK( WhiteHandle, Image ) )
		{
			return -1 ;
		}
//		DX_D3D11_DRAWPREP_NOTEX( Flag )
		DX_D3D11_DRAWPREP_TEX( Image->Orig, Image->Hard.Draw[ 0 ].Tex->PF->D3D11.Texture, Image->Hard.Draw[ 0 ].Tex->PF->D3D11.TextureSRV, Flag )
	}

	return 0 ;
}

extern	int		Graphics_D3D11_DrawPrimitive( const VERTEX_3D *Vertex, int VertexNum, int PrimitiveType, IMAGEDATA *Image, int TransFlag )
{
	int i ;
	VERTEX_3D *VertP ;
	unsigned char tmp ;

	if( Graphics_D3D11_DrawPrimitive3DPreparation( 0, Image, TransFlag ) < 0 )
	{
		return -1 ;
	}

	if( GSYS.HardInfo.UseVertexColorBGRAFormat == FALSE )
	{
		VertP = ( VERTEX_3D * )Vertex ;
		for( i = 0 ; i < VertexNum ; i ++, VertP ++ )
		{
			tmp = VertP->b ;
			VertP->b = VertP->r ;
			VertP->r = tmp ;
		}
	}

	// 描画
	Graphics_D3D11_CommonBuffer_DrawPrimitive(
		D3D11_VERTEX_INPUTLAYOUT_3D,
		( D_D3D11_PRIMITIVE_TOPOLOGY )PrimitiveType,
		Vertex,
		VertexNum
	) ;

	if( GSYS.HardInfo.UseVertexColorBGRAFormat == FALSE )
	{
		VertP = ( VERTEX_3D * )Vertex ;
		for( i = 0 ; i < VertexNum ; i ++, VertP ++ )
		{
			tmp = VertP->b ;
			VertP->b = VertP->r ;
			VertP->r = tmp ;
		}
	}

	// 終了
	return 0 ;
}

extern	int		Graphics_D3D11_DrawIndexedPrimitive( const VERTEX_3D *Vertex, int VertexNum, const WORD *Indices, int IndexNum, int PrimitiveType, IMAGEDATA *Image, int TransFlag )
{
	int i ;
	VERTEX_3D *VertP ;
	unsigned char tmp ;

	if( Graphics_D3D11_DrawPrimitive3DPreparation( 0, Image, TransFlag ) < 0 )
	{
		return -1 ;
	}

	if( GSYS.HardInfo.UseVertexColorBGRAFormat == FALSE )
	{
		VertP = ( VERTEX_3D * )Vertex ;
		for( i = 0 ; i < VertexNum ; i ++, VertP ++ )
		{
			tmp = VertP->b ;
			VertP->b = VertP->r ;
			VertP->r = tmp ;
		}
	}

	// 描画
	Graphics_D3D11_CommonBuffer_DrawIndexedPrimitive(
		D3D11_VERTEX_INPUTLAYOUT_3D,
		( D_D3D11_PRIMITIVE_TOPOLOGY )PrimitiveType,
		Vertex,
		VertexNum,
		Indices,
		IndexNum,
		D_DXGI_FORMAT_R16_UINT
	) ;

	if( GSYS.HardInfo.UseVertexColorBGRAFormat == FALSE )
	{
		VertP = ( VERTEX_3D * )Vertex ;
		for( i = 0 ; i < VertexNum ; i ++, VertP ++ )
		{
			tmp = VertP->b ;
			VertP->b = VertP->r ;
			VertP->r = tmp ;
		}
	}

	// 終了
	return 0 ;
}

extern	int		Graphics_D3D11_DrawPrimitiveLight( const VERTEX3D *Vertex, int VertexNum, int PrimitiveType, IMAGEDATA *Image, int TransFlag )
{
	int i ;
	VERTEX3D *VertP ;
	unsigned char tmp ;

	if( Graphics_D3D11_DrawPrimitive3DPreparation( DX_D3D11_DRAWPREP_LIGHTING | DX_D3D11_DRAWPREP_SPECULAR, Image, TransFlag ) < 0 )
	{
		return -1 ;
	}

	if( GSYS.HardInfo.UseVertexColorBGRAFormat == FALSE )
	{
		VertP = ( VERTEX3D * )Vertex ;
		for( i = 0 ; i < VertexNum ; i ++, VertP ++ )
		{
			tmp = VertP->dif.b ;
			VertP->dif.b = VertP->dif.r ;
			VertP->dif.r = tmp ;

			tmp = VertP->spc.b ;
			VertP->spc.b = VertP->spc.r ;
			VertP->spc.r = tmp ;
		}
	}

	// 描画
	Graphics_D3D11_CommonBuffer_DrawPrimitive(
		D3D11_VERTEX_INPUTLAYOUT_3D_LIGHT,
		( D_D3D11_PRIMITIVE_TOPOLOGY )PrimitiveType,
		Vertex,
		VertexNum
	) ;

	if( GSYS.HardInfo.UseVertexColorBGRAFormat == FALSE )
	{
		VertP = ( VERTEX3D * )Vertex ;
		for( i = 0 ; i < VertexNum ; i ++, VertP ++ )
		{
			tmp = VertP->dif.b ;
			VertP->dif.b = VertP->dif.r ;
			VertP->dif.r = tmp ;

			tmp = VertP->spc.b ;
			VertP->spc.b = VertP->spc.r ;
			VertP->spc.r = tmp ;
		}
	}

	// 終了
	return 0 ;
}

extern	int		Graphics_D3D11_DrawIndexedPrimitiveLight( const VERTEX3D *Vertex, int VertexNum, const WORD *Indices, int IndexNum, int PrimitiveType, IMAGEDATA *Image, int TransFlag )
{
	int i ;
	VERTEX3D *VertP ;
	unsigned char tmp ;

	if( Graphics_D3D11_DrawPrimitive3DPreparation( DX_D3D11_DRAWPREP_LIGHTING | DX_D3D11_DRAWPREP_SPECULAR, Image, TransFlag ) < 0 )
	{
		return -1 ;
	}

	if( GSYS.HardInfo.UseVertexColorBGRAFormat == FALSE )
	{
		VertP = ( VERTEX3D * )Vertex ;
		for( i = 0 ; i < VertexNum ; i ++, VertP ++ )
		{
			tmp = VertP->dif.b ;
			VertP->dif.b = VertP->dif.r ;
			VertP->dif.r = tmp ;

			tmp = VertP->spc.b ;
			VertP->spc.b = VertP->spc.r ;
			VertP->spc.r = tmp ;
		}
	}

	// 描画
	Graphics_D3D11_CommonBuffer_DrawIndexedPrimitive(
		D3D11_VERTEX_INPUTLAYOUT_3D_LIGHT,
		( D_D3D11_PRIMITIVE_TOPOLOGY )PrimitiveType,
		Vertex,
		VertexNum,
		Indices,
		IndexNum,
		D_DXGI_FORMAT_R16_UINT
	) ;

	if( GSYS.HardInfo.UseVertexColorBGRAFormat == FALSE )
	{
		VertP = ( VERTEX3D * )Vertex ;
		for( i = 0 ; i < VertexNum ; i ++, VertP ++ )
		{
			tmp = VertP->dif.b ;
			VertP->dif.b = VertP->dif.r ;
			VertP->dif.r = tmp ;

			tmp = VertP->spc.b ;
			VertP->spc.b = VertP->spc.r ;
			VertP->spc.r = tmp ;
		}
	}

	// 終了
	return 0 ;
}

extern	int		Graphics_D3D11_DrawPrimitiveLight_UseVertexBuffer( 
	VERTEXBUFFERHANDLEDATA *VertexBuffer,
	int						PrimitiveType,
	int						StartVertex,
	int						UseVertexNum,
	IMAGEDATA *				Image,
	int						TransFlag 
)
{
	// VERTEX3D構造体のみ対応
	if( VertexBuffer->Type != DX_VERTEX_TYPE_NORMAL_3D )
	{
		return -1 ;
	}

	if( Graphics_D3D11_DrawPrimitive3DPreparation( DX_D3D11_DRAWPREP_LIGHTING | DX_D3D11_DRAWPREP_SPECULAR, Image, TransFlag ) < 0 )
	{
		return -1 ;
	}

	// プリミティブタイプをセット
	Graphics_D3D11_DeviceState_SetPrimitiveTopology( ( D_D3D11_PRIMITIVE_TOPOLOGY )PrimitiveType ) ;

	// 入力レイアウトをセット
	Graphics_D3D11_DeviceState_SetInputLayout( GD3D11.Device.InputLayout.BaseInputLayout[ D3D11_VERTEX_INPUTLAYOUT_3D_LIGHT ] ) ;

	// 頂点シェーダーをセット
	if( Graphics_D3D11_Shader_Normal3DDraw_Setup() == FALSE )
	{
		return -1 ;
	}

	// 頂点バッファをセット
	Graphics_D3D11_DeviceState_SetVertexBuffer( VertexBuffer->PF->D3D11.VertexBuffer, D3D11_VertexSize[ D3D11_VERTEX_INPUTLAYOUT_3D_LIGHT ] ) ;

	// ステートと定数バッファの更新
	Graphics_D3D11_DeviceState_SetupStateAndConstantBuffer() ;

	// 描画
	D3D11DeviceContext_Draw( ( UINT )UseVertexNum, ( UINT )StartVertex ) ;
	GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;

	// 終了
	return 0 ;
}

extern	int		Graphics_D3D11_DrawIndexedPrimitiveLight_UseVertexBuffer(
	VERTEXBUFFERHANDLEDATA *	VertexBuffer,
	INDEXBUFFERHANDLEDATA *		IndexBuffer,
	int							PrimitiveType,
	int							BaseVertex,
	int							/*StartVertex*/,
	int							/*UseVertexNum*/,
	int							StartIndex,
	int							UseIndexNum,
	IMAGEDATA *					Image,
	int							TransFlag
)
{
	D_DXGI_FORMAT IndexFormat ;

	// VERTEX3D構造体のみ対応
	if( VertexBuffer->Type != DX_VERTEX_TYPE_NORMAL_3D )
	{
		return -1 ;
	}

	switch( IndexBuffer->Type )
	{
	default :
	case DX_INDEX_TYPE_16BIT :
		IndexFormat = D_DXGI_FORMAT_R16_UINT ;
		break ;

	case DX_INDEX_TYPE_32BIT :
		IndexFormat = D_DXGI_FORMAT_R32_UINT ;
		break ;
	}

	if( Graphics_D3D11_DrawPrimitive3DPreparation( DX_D3D11_DRAWPREP_LIGHTING | DX_D3D11_DRAWPREP_SPECULAR, Image, TransFlag ) < 0 )
	{
		return -1 ;
	}

	// プリミティブタイプをセット
	Graphics_D3D11_DeviceState_SetPrimitiveTopology( ( D_D3D11_PRIMITIVE_TOPOLOGY )PrimitiveType ) ;

	// 入力レイアウトをセット
	Graphics_D3D11_DeviceState_SetInputLayout( GD3D11.Device.InputLayout.BaseInputLayout[ D3D11_VERTEX_INPUTLAYOUT_3D_LIGHT ] ) ;

	// 頂点シェーダーをセット
	if( Graphics_D3D11_Shader_Normal3DDraw_Setup() == FALSE )
	{
		return -1 ;
	}

	// 頂点バッファをセット
	Graphics_D3D11_DeviceState_SetVertexBuffer( VertexBuffer->PF->D3D11.VertexBuffer, D3D11_VertexSize[ D3D11_VERTEX_INPUTLAYOUT_3D_LIGHT ] ) ;

	// インデックスバッファをセット
	Graphics_D3D11_DeviceState_SetIndexBuffer( IndexBuffer->PF->D3D11.IndexBuffer, IndexFormat ) ;

	// ステートと定数バッファの更新
	Graphics_D3D11_DeviceState_SetupStateAndConstantBuffer() ;

	// 描画
	D3D11DeviceContext_DrawIndexed( ( UINT )UseIndexNum, ( UINT )StartIndex, BaseVertex ) ;
	GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;

	// 終了
	return 0 ;
}

extern	int		Graphics_D3D11_DrawPrimitive2D( VERTEX_2D *Vertex, int VertexNum, int PrimitiveType, IMAGEDATA *Image, int TransFlag, int BillboardFlag, int Is3D, int ReverseXFlag, int ReverseYFlag, int TextureNo, int IsShadowMap )
{
	int Flag ;
	int i ;
	VERTEX_2D *Vert ;
	int SmHandle ;
	SHADOWMAPDATA *ShadowMap ;
	VERTEX_2D *VertP ;

	if( GAPIWin.D3D11DeviceObject == NULL ) return -1 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画の準備
	if( BillboardFlag == TRUE )
	{
		Flag = TransFlag | ( Is3D ? DX_D3D11_DRAWPREP_3D | DX_D3D11_DRAWPREP_FOG : 0 ) | DX_D3D11_DRAWPREP_TEXADDRESS ;
	}
	else
	{
		Flag = TransFlag | ( Is3D ? DX_D3D11_DRAWPREP_3D | DX_D3D11_DRAWPREP_FOG : 0 ) | DX_D3D11_DRAWPREP_TEXADDRESS | DX_D3D11_DRAWPREP_CULLING ;
	}
	if( IsShadowMap )
	{
		SmHandle = ( int )( DWORD_PTR )Image ;
		if( !SHADOWMAPCHKFULL( SmHandle, ShadowMap ) )
		{
			Flag |= DX_D3D11_DRAWPREP_TEXTURE ;
			Graphics_D3D11_DrawSetting_SetTexture( ShadowMap->PF->D3D11.DepthTexture, ShadowMap->PF->D3D11.DepthTextureSRV ) ;

			if( GSYS.ChangeSettingFlag ||
				GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag != FALSE ||
				GD3D11.Device.DrawSetting.DrawPrepParamFlag != Flag )
			{
				Graphics_D3D11_DrawPreparation( Flag ) ;
			}
		}
	}
	else
	if( Image )
	{
		DX_D3D11_DRAWPREP_TEX( Image->Orig, Image->Hard.Draw[ TextureNo ].Tex->PF->D3D11.Texture, Image->Hard.Draw[ TextureNo ].Tex->PF->D3D11.TextureSRV, Flag )
	}
	else
	{
		DX_D3D11_DRAWPREP_NOTEX( Flag )
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

	if( GSYS.HardInfo.UseVertexColorBGRAFormat == FALSE )
	{
		VertP = ( VERTEX_2D * )Vertex ;
		for( i = 0 ; i < VertexNum ; i ++, VertP ++ )
		{
			VertP->color =	(   VertP->color & 0xff00ff00 ) |
							( ( VertP->color & 0x00ff0000 ) >> 16 ) |
							( ( VertP->color & 0x000000ff ) << 16 ) ;
		}
	}

	// 描画
	Graphics_D3D11_CommonBuffer_DrawPrimitive(
		D3D11_VERTEX_INPUTLAYOUT_2D,
		( D_D3D11_PRIMITIVE_TOPOLOGY )PrimitiveType,
		Vertex,
		VertexNum
	) ;

	if( GSYS.HardInfo.UseVertexColorBGRAFormat == FALSE )
	{
		VertP = ( VERTEX_2D * )Vertex ;
		for( i = 0 ; i < VertexNum ; i ++, VertP ++ )
		{
			VertP->color =	(   VertP->color & 0xff00ff00 ) |
							( ( VertP->color & 0x00ff0000 ) >> 16 ) |
							( ( VertP->color & 0x000000ff ) << 16 ) ;
		}
	}

	// 終了
	return 0 ;
}

extern	int		Graphics_D3D11_DrawPrimitive2DUser( const VERTEX2D *Vertex, int VertexNum, int PrimitiveType, IMAGEDATA *Image, int TransFlag, int Is3D, int /*ReverseXFlag*/, int /*ReverseYFlag*/, int TextureNo )
{
	int Flag ;
	int i ;
	VERTEX2D *VertP ;
	unsigned char tmp ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画の準備
	Flag = TransFlag | ( Is3D ? DX_D3D11_DRAWPREP_3D | DX_D3D11_DRAWPREP_FOG : 0 ) | DX_D3D11_DRAWPREP_TEXADDRESS | DX_D3D11_DRAWPREP_CULLING ;
	if( Image )
	{
		DX_D3D11_DRAWPREP_TEX( Image->Orig, Image->Hard.Draw[ TextureNo ].Tex->PF->D3D11.Texture, Image->Hard.Draw[ TextureNo ].Tex->PF->D3D11.TextureSRV, Flag )
	}
	else
	{
		DX_D3D11_DRAWPREP_NOTEX( Flag )
	}

	if( GSYS.HardInfo.UseVertexColorBGRAFormat == FALSE )
	{
		VertP = ( VERTEX2D * )Vertex ;
		for( i = 0 ; i < VertexNum ; i ++, VertP ++ )
		{
			tmp = VertP->dif.b ;
			VertP->dif.b = VertP->dif.r ;
			VertP->dif.r = tmp ;
		}
	}

	// 描画
	Graphics_D3D11_CommonBuffer_DrawPrimitive(
		D3D11_VERTEX_INPUTLAYOUT_2D,
		( D_D3D11_PRIMITIVE_TOPOLOGY )PrimitiveType,
		Vertex,
		VertexNum
	) ;

	if( GSYS.HardInfo.UseVertexColorBGRAFormat == FALSE )
	{
		VertP = ( VERTEX2D * )Vertex ;
		for( i = 0 ; i < VertexNum ; i ++, VertP ++ )
		{
			tmp = VertP->dif.b ;
			VertP->dif.b = VertP->dif.r ;
			VertP->dif.r = tmp ;
		}
	}

	// 終了
	return 0 ;
}

extern	int		Graphics_D3D11_DrawIndexedPrimitive2DUser( const VERTEX2D *Vertex, int VertexNum, const WORD *Indices, int IndexNum, int PrimitiveType, IMAGEDATA *Image, int TransFlag )
{
	int Flag ;
	int i ;
	VERTEX2D *VertP ;
	unsigned char tmp ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画の準備
	Flag = TransFlag | DX_D3D11_DRAWPREP_TEXADDRESS | DX_D3D11_DRAWPREP_CULLING ;
	if( Image )
	{
		DX_D3D11_DRAWPREP_TEX( Image->Orig, Image->Hard.Draw[ 0 ].Tex->PF->D3D11.Texture, Image->Hard.Draw[ 0 ].Tex->PF->D3D11.TextureSRV, Flag )
	}
	else
	{
		DX_D3D11_DRAWPREP_NOTEX( Flag )
	}

	if( GSYS.HardInfo.UseVertexColorBGRAFormat == FALSE )
	{
		VertP = ( VERTEX2D * )Vertex ;
		for( i = 0 ; i < VertexNum ; i ++, VertP ++ )
		{
			tmp = VertP->dif.b ;
			VertP->dif.b = VertP->dif.r ;
			VertP->dif.r = tmp ;
		}
	}

	// 描画
	Graphics_D3D11_CommonBuffer_DrawIndexedPrimitive(
		D3D11_VERTEX_INPUTLAYOUT_2D,
		( D_D3D11_PRIMITIVE_TOPOLOGY )PrimitiveType,
		Vertex,
		VertexNum,
		Indices,
		IndexNum, 
		D_DXGI_FORMAT_R16_UINT
	) ;

	if( GSYS.HardInfo.UseVertexColorBGRAFormat == FALSE )
	{
		VertP = ( VERTEX2D * )Vertex ;
		for( i = 0 ; i < VertexNum ; i ++, VertP ++ )
		{
			tmp = VertP->dif.b ;
			VertP->dif.b = VertP->dif.r ;
			VertP->dif.r = tmp ;
		}
	}

	// 終了
	return 0 ;
}












// シェーダー描画用描画前セットアップ関数
extern void Graphics_D3D11_DrawPreparationToShader( int ParamFlag, int Is2D )
{
	int							i ;
	int							Flag ;
	D_ID3D11ShaderResourceView	*ShaderResourceView[ USE_TEXTURESTAGE_NUM ] ;
	int							ShaderResourceViewNum ;

	// 基本的なセットアップ処理
	Flag = ParamFlag | DX_D3D11_DRAWPREP_SPECULAR | DX_D3D11_DRAWPREP_TEXADDRESS | DX_D3D11_DRAWPREP_NOBLENDSETTING | DX_D3D11_DRAWPREP_CULLING | ( Is2D ? 0 : DX_D3D11_DRAWPREP_3D ) ;
	DX_D3D11_DRAWPREP_NOTEX( Flag )

	// アルファテストパラメータが有効な場合はそれを優先する
	if( GSYS.DrawSetting.AlphaTestMode != -1 )
	{
		Graphics_D3D11_DeviceState_SetAlphaTestRef( GSYS.DrawSetting.AlphaTestParam ) ;
		Graphics_D3D11_DeviceState_SetAlphaTestCmpMode( GSYS.DrawSetting.AlphaTestMode ) ;
	}
	else
	{
		// デフォルトのアルファテストの設定をセット
		Graphics_D3D11_DeviceState_SetAlphaTestRef( -1 ) ;
		Graphics_D3D11_DeviceState_SetAlphaTestCmpMode( DX_CMP_GREATER ) ;
	}

	// 出力先とのブレンド方式をセット
	Graphics_D3D11_DeviceState_SetBlendMode( GSYS.DrawSetting.BlendMode == DX_BLENDMODE_SUB ? DX_BLENDMODE_SUB1 : GSYS.DrawSetting.BlendMode ) ;

	// 使用するテクスチャを列挙
	ShaderResourceViewNum = 0 ;
	for( i = 0 ; i < USE_TEXTURESTAGE_NUM ; i ++ )
	{
		IMAGEDATA     *TempImage ;
		SHADOWMAPDATA *TempShadowMap ;

		if( !GRAPHCHKFULL( GSYS.DrawSetting.UserShaderRenderInfo.SetTextureGraphHandle[ i ], TempImage ) )
		{
			ShaderResourceView[ i ] = TempImage->Hard.Draw[ 0 ].Tex->PF->D3D11.TextureSRV ;
		}
		else
		if( !SHADOWMAPCHKFULL( GSYS.DrawSetting.UserShaderRenderInfo.SetTextureGraphHandle[ i ], TempShadowMap ) )
		{
			ShaderResourceView[ i ] = TempShadowMap->PF->D3D11.DepthTextureSRV ;
		}
		else
		{
			continue ;
		}

		ShaderResourceViewNum = i + 1 ;
	}

	// 使用するテクスチャをセット
	if( ShaderResourceViewNum > 0 )
	{
		Graphics_D3D11_DeviceState_SetPSShaderResouceView( 0, ShaderResourceViewNum, ShaderResourceView ) ;
	}

	// 使用する頂点シェーダーを更新
	if( Graphics_D3D11_DeviceState_SetVertexShaderToHandle( GSYS.DrawSetting.UserShaderRenderInfo.SetVertexShaderHandle ) < 0 )
	{
		// オリジナルの頂点シェーダーの指定が無い場合はデフォルトの頂点シェーダーを使用する
		Graphics_D3D11_DeviceState_SetVertexShader(
			Is2D ?
				GD3D11.Device.Shader.Base.BaseSimple_VS[ D3D11_VERTEX_INPUTLAYOUT_BLENDTEX_2D ] :
				GD3D11.Device.Shader.Base.BaseSimple_VS[ D3D11_VERTEX_INPUTLAYOUT_SHADER_3D   ], FALSE ) ;
	}

	// 使用するピクセルシェーダーを更新
	Graphics_D3D11_DeviceState_SetPixelShaderToHandle( GSYS.DrawSetting.UserShaderRenderInfo.SetPixelShaderHandle ) ;
}

// シェーダーを使って２Ｄプリミティブを描画する
extern	int		Graphics_D3D11_DrawPrimitive2DToShader(        const VERTEX2DSHADER *Vertex, int VertexNum,                                              int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ )
{
	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画の準備
	Graphics_D3D11_DrawPreparationToShader( 0, TRUE ) ;

	// 描画
	Graphics_D3D11_CommonBuffer_DrawPrimitive(
		D3D11_VERTEX_INPUTLAYOUT_BLENDTEX_2D,
		( D_D3D11_PRIMITIVE_TOPOLOGY )PrimitiveType,
		Vertex,
		VertexNum
	) ;

	// 終了
	return 0 ;
}

// シェーダーを使って３Ｄプリミティブを描画する
extern	int		Graphics_D3D11_DrawPrimitive3DToShader(        const VERTEX3DSHADER *Vertex, int VertexNum,                                              int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ )
{
	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画の準備
	Graphics_D3D11_DrawPreparationToShader( DX_D3D11_DRAWPREP_LIGHTING | DX_D3D11_DRAWPREP_FOG, FALSE ) ;

	// ３Ｄ行列をハードウエアに反映する
	if( GSYS.DrawSetting.MatchHardware3DMatrix == FALSE )
		Graphics_DrawSetting_ApplyLib3DMatrixToHardware() ;

	// 描画
	Graphics_D3D11_CommonBuffer_DrawPrimitive(
		D3D11_VERTEX_INPUTLAYOUT_SHADER_3D,
		( D_D3D11_PRIMITIVE_TOPOLOGY )PrimitiveType,
		Vertex,
		VertexNum,
		FALSE
	) ;

	// 終了
	return 0 ;
}

// シェーダーを使って２Ｄプリミティブを描画する( 頂点インデックスを使用する )
extern	int		Graphics_D3D11_DrawPrimitiveIndexed2DToShader( const VERTEX2DSHADER *Vertex, int VertexNum, const unsigned short *Indices, int IndexNum, int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ )
{
	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画の準備
	Graphics_D3D11_DrawPreparationToShader( 0, TRUE ) ;

	// 描画
	Graphics_D3D11_CommonBuffer_DrawIndexedPrimitive(
		D3D11_VERTEX_INPUTLAYOUT_BLENDTEX_2D,
		( D_D3D11_PRIMITIVE_TOPOLOGY )PrimitiveType,
		Vertex,
		VertexNum,
		Indices,
		IndexNum,
		D_DXGI_FORMAT_R16_UINT
	) ;

	// 終了
	return 0 ;
}

// シェーダーを使って３Ｄプリミティブを描画する( 頂点インデックスを使用する )
extern	int		Graphics_D3D11_DrawPrimitiveIndexed3DToShader( const VERTEX3DSHADER *Vertex, int VertexNum, const unsigned short *Indices, int IndexNum, int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ )
{
	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画の準備
	Graphics_D3D11_DrawPreparationToShader( DX_D3D11_DRAWPREP_LIGHTING | DX_D3D11_DRAWPREP_FOG, FALSE ) ;

	// ３Ｄ行列をハードウエアに反映する
	if( GSYS.DrawSetting.MatchHardware3DMatrix == FALSE )
		Graphics_DrawSetting_ApplyLib3DMatrixToHardware() ;

	// 描画
	Graphics_D3D11_CommonBuffer_DrawIndexedPrimitive(
		D3D11_VERTEX_INPUTLAYOUT_SHADER_3D,
		( D_D3D11_PRIMITIVE_TOPOLOGY )PrimitiveType,
		Vertex,
		VertexNum,
		Indices,
		IndexNum,
		D_DXGI_FORMAT_R16_UINT,
		FALSE
	) ;

	// 終了
	return 0 ;
}

// シェーダーを使って３Ｄプリミティブを描画する( 頂点バッファ使用版 )
extern	int		Graphics_D3D11_DrawPrimitive3DToShader_UseVertexBuffer2(        int VertexBufHandle,                     int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */, int StartVertex, int UseVertexNum )
{
	VERTEXBUFFERHANDLEDATA *VertexBuffer ;

	// エラー判定
	if( VERTEXBUFFERCHK( VertexBufHandle, VertexBuffer ) )
	{
		return -1 ;
	}

	// シェーダー用頂点データではない場合はエラー
	if( VertexBuffer->Type != DX_VERTEX_TYPE_SHADER_3D )
	{
		return -1 ;
	}

	// ３Ｄ行列をハードウエアに反映する
	if( GSYS.DrawSetting.MatchHardware3DMatrix == FALSE )
		Graphics_DrawSetting_ApplyLib3DMatrixToHardware() ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画の準備
	Graphics_D3D11_DrawPreparationToShader( DX_D3D11_DRAWPREP_LIGHTING | DX_D3D11_DRAWPREP_FOG, FALSE ) ;

	// プリミティブタイプをセット
	Graphics_D3D11_DeviceState_SetPrimitiveTopology( ( D_D3D11_PRIMITIVE_TOPOLOGY )PrimitiveType ) ;

	// 入力レイアウトをセット
	Graphics_D3D11_DeviceState_SetInputLayout( GD3D11.Device.InputLayout.BaseInputLayout[ D3D11_VERTEX_INPUTLAYOUT_SHADER_3D ] ) ;

	// 頂点バッファをセット
	Graphics_D3D11_DeviceState_SetVertexBuffer( VertexBuffer->PF->D3D11.VertexBuffer, D3D11_VertexSize[ D3D11_VERTEX_INPUTLAYOUT_SHADER_3D ] ) ;

	// ステートと定数バッファの更新
	Graphics_D3D11_DeviceState_SetupStateAndConstantBuffer() ;

	// 描画
	D3D11DeviceContext_Draw( ( UINT )UseVertexNum, ( UINT )StartVertex ) ;
	GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;

	// 終了
	return 0 ;
}

// シェーダーを使って３Ｄプリミティブを描画する( 頂点バッファとインデックスバッファ使用版 )
extern	int		Graphics_D3D11_DrawPrimitiveIndexed3DToShader_UseVertexBuffer2( int VertexBufHandle, int IndexBufHandle, int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */, int BaseVertex, int /*StartVertex*/, int /*UseVertexNum*/, int StartIndex, int UseIndexNum )
{
	VERTEXBUFFERHANDLEDATA *VertexBuffer ;
	INDEXBUFFERHANDLEDATA  *IndexBuffer ;
	D_DXGI_FORMAT IndexFormat ;

	// エラー判定
	if( VERTEXBUFFERCHK( VertexBufHandle, VertexBuffer ) )
	{
		return -1 ;
	}
	if( INDEXBUFFERCHK( IndexBufHandle, IndexBuffer ) )
	{
		return -1 ;
	}

	// シェーダー用頂点データではない場合はエラー
	if( VertexBuffer->Type != DX_VERTEX_TYPE_SHADER_3D )
	{
		return -1 ;
	}

	switch( IndexBuffer->Type )
	{
	default :
	case DX_INDEX_TYPE_16BIT :
		IndexFormat = D_DXGI_FORMAT_R16_UINT ;
		break ;

	case DX_INDEX_TYPE_32BIT :
		IndexFormat = D_DXGI_FORMAT_R32_UINT ;
		break ;
	}

	// ３Ｄ行列をハードウエアに反映する
	if( GSYS.DrawSetting.MatchHardware3DMatrix == FALSE )
		Graphics_DrawSetting_ApplyLib3DMatrixToHardware() ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画の準備
	Graphics_D3D11_DrawPreparationToShader( DX_D3D11_DRAWPREP_LIGHTING | DX_D3D11_DRAWPREP_FOG, FALSE ) ;

	// プリミティブタイプをセット
	Graphics_D3D11_DeviceState_SetPrimitiveTopology( ( D_D3D11_PRIMITIVE_TOPOLOGY )PrimitiveType ) ;

	// 入力レイアウトをセット
	Graphics_D3D11_DeviceState_SetInputLayout( GD3D11.Device.InputLayout.BaseInputLayout[ D3D11_VERTEX_INPUTLAYOUT_SHADER_3D ] ) ;

	// 頂点バッファをセット
	Graphics_D3D11_DeviceState_SetVertexBuffer( VertexBuffer->PF->D3D11.VertexBuffer, D3D11_VertexSize[ D3D11_VERTEX_INPUTLAYOUT_SHADER_3D ] ) ;

	// インデックスバッファをセット
	Graphics_D3D11_DeviceState_SetIndexBuffer( IndexBuffer->PF->D3D11.IndexBuffer, IndexFormat ) ;

	// ステートと定数バッファの更新
	Graphics_D3D11_DeviceState_SetupStateAndConstantBuffer() ;

	// 描画
	D3D11DeviceContext_DrawIndexed( ( UINT )UseIndexNum, ( UINT )StartIndex, BaseVertex ) ;
	GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;

	// 終了
	return 0 ;
}




























// 環境依存初期化関係

// 描画処理の環境依存部分の初期化を行う関数( 実行箇所区別０ )
extern	int		Graphics_D3D11_Initialize_Timing0_PF( void )
{
	// Direct3D11 を使用したグラフィックス処理の初期化
	if( Graphics_D3D11_Initialize() == -1 )
	{
		return -1 ;
	}

	// 正常終了
	return 0 ;
}

// 描画処理の環境依存部分の初期化を行う関数( 実行箇所区別２ )
extern	int		Graphics_D3D11_Initialize_Timing1_PF( void )
{
	// デバイスの設定をリフレッシュ
	Graphics_D3D11_DeviceState_RefreshRenderState() ;

	// 画面の初期化
	Graphics_Hardware_D3D11_ClearDrawScreen_PF( NULL ) ;

	// 正常終了
	return 0 ;
}

// ハードウエアアクセラレータを使用する場合の環境依存の初期化処理を行う
extern	int		Graphics_D3D11_Hardware_Initialize_PF( void )
{
	return Graphics_D3D11_Device_Initialize() ;
}

// 描画処理の環境依存部分の後始末を行う関数
extern	int		Graphics_D3D11_Terminate_PF( void )
{
	// Direct3D11 の描画処理の後始末
	Graphics_D3D11_Terminate() ;

	// シェーダーコードの後始末
	Graphics_D3D11_ShaderCode_Base_Terminate() ;
	Graphics_D3D11_ShaderCode_Base3D_Terminate() ;

#ifndef DX_NON_MODEL
	Graphics_D3D11_ShaderCode_Model_Terminate() ;
#endif // DX_NON_MODEL

	// 終了
	return 0 ;
}

// グラフィックスシステムの復帰、又は変更付きの再セットアップを行う
extern	int		Graphics_D3D11_RestoreOrChangeSetupGraphSystem_PF( int Change, int ScreenSizeX, int ScreenSizeY, int ColorBitDepth, int RefreshRate )
{
	int i ;

	// グラフィックスデバイスのロストから復帰する前に呼ぶ関数が登録されていたら実行する
	if( GD3D11.Device.Setting.DeviceLostCallbackFunction )
	{
		GD3D11.Device.Setting.DeviceLostCallbackFunction( GD3D11.Device.Setting.DeviceLostCallbackData ) ;
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
//		SetMemImgDefaultColorType( ColorBitDepth == 32 ? 1 : 0 ) ;
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

		// Direct3D11 による描画処理の初期化
		GSYS.Screen.FullScreenResolutionModeAct = GSYS.Screen.FullScreenResolutionMode ;
		Graphics_D3D11_Initialize() ;

		// グラフィックハンドルが持つ Direct3D11 オブジェクトの再作成
		Graphics_D3D11_CreateObjectAll() ;

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
		Graphics_D3D11_Device_ReInitialize() ;

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
		if( GD3D11.Device.Setting.DeviceRestoreCallbackFunction )
		{
			GD3D11.Device.Setting.DeviceRestoreCallbackFunction( GD3D11.Device.Setting.DeviceRestoreCallbackData ) ;
		}

		// ハードウエアの設定をリフレッシュ
		Graphics_D3D11_DeviceState_RefreshRenderState() ;

#ifndef DX_NON_MODEL
		// 頂点バッファのセットアップ
		MV1SetupVertexBufferAll() ;
#endif

		// ライトの再設定
		GSYS.Light.HardwareRefresh = TRUE ;
		NS_SetUseLighting( GSYS.Light.ProcessDisable ? FALSE : TRUE );

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
extern	int		Graphics_D3D11_Hardware_CheckValid_PF( void )
{
	return D3D11Device_IsValid() ;
}

// DirectX のオブジェクトを解放する
extern	void	Graphics_D3D11_ReleaseDirectXObject_PF( void )
{
#ifndef DX_NON_MASK
	// マスクサーフェスを一時削除
	Mask_ReleaseSurface() ;
#endif
	// グラフィックハンドルが持つ DirectX オブジェクトの解放
	Graphics_D3D11_ReleaseObjectAll() ;

	// システムが持つ Direct3D11 オブジェクトの解放
	Graphics_D3D11_Terminate() ;
}





















// 環境依存システム関係

// WM_ACTIVATE メッセージの処理で、ウインドウモード時にアクティブになった際に呼ばれる関数
extern	void	Graphics_D3D11_WM_ACTIVATE_ActiveProcess_PF( void )
{
}
























// 環境依存描画設定関係

// シェーダー描画での描画先を設定する
extern	int		Graphics_Hardware_D3D11_SetRenderTargetToShader_PF( int TargetIndex, int DrawScreen, int SurfaceIndex, int MipLevel )
{
	IMAGEDATA *Image ;
	IMAGEDATA *OldImage ;

	// ターゲットが０以外の場合はシェーダーが使用できない場合は何もしない
	if( GSYS.HardInfo.UseShader == FALSE )
		return -1 ;

	// デバイスが無効になっていたら何もせず終了
	if( GAPIWin.D3D11DeviceObject == NULL )
		return -1 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// セットしていたテクスチャーを外す
	Graphics_D3D11_DrawSetting_SetTexture( NULL, NULL ) ;

	// 今までの描画先の画像情報の取得
	if( GRAPHCHKFULL( GSYS.DrawSetting.TargetScreen[ TargetIndex ], OldImage ) )
	{
		OldImage = NULL ;
	}
	else
	{
		// 今までの描画先がマルチサンプルタイプのレンダリングターゲットだった場合はダウンサンプリングする
		if( OldImage->Hard.Draw[ 0 ].Tex->PF->D3D11.MSTexture != NULL )
		{
			Graphics_D3D11_UpdateDrawTexture(
				OldImage->Hard.Draw[ 0 ].Tex,
				GSYS.DrawSetting.TargetScreenSurface[ TargetIndex ],
				GD3D11.Device.Caps.TextureFormat[ OldImage->Orig->ColorFormat ]
			) ;
		}
	}

	// 画像かどうかを判定
	if( GRAPHCHKFULL( DrawScreen, Image ) )
	{
		// 画像ではない場合は描画対象を無効にする
		GSYS.DrawSetting.TargetScreen[ TargetIndex ] = 0 ;
		GSYS.DrawSetting.TargetScreenSurface[ TargetIndex ] = 0 ;
		GSYS.DrawSetting.TargetScreenMipLevel[ TargetIndex ] = 0 ;
		Graphics_D3D11_DeviceState_SetRenderTarget( NULL, NULL, TargetIndex ) ;
		return 0 ;
	}

	// 画像だった場合は描画可能では無い場合はエラー
	if( Image->Orig->FormatDesc.DrawValidFlag == FALSE )
	{
		return -1 ;
	}

	// 描画先をセット
	Graphics_D3D11_DeviceState_SetRenderTarget( Image->Hard.Draw[ 0 ].Tex->PF->D3D11.Texture, Image->Hard.Draw[ 0 ].Tex->PF->D3D11.TextureRTV[ SurfaceIndex * Image->Orig->Hard.MipMapCount + MipLevel ], TargetIndex ) ;

	// ターゲットが 0 の場合は使用するＺバッファのセットアップ
	if( TargetIndex == 0 )
	{
		Graphics_Screen_SetupUseZBuffer() ;
	}

	// 正常終了
	return 0 ;
}

// SetDrawBright の引数が一つ版
extern	int		Graphics_Hardware_D3D11_SetDrawBrightToOneParam_PF( DWORD /*Bright*/ )
{
	// ディフーズカラーの更新
	GD3D11.Device.DrawInfo.DiffuseColor = GetDiffuseColor() ;

	// 正常終了
	return 0 ;
}

// 描画ブレンドモードをセットする
extern	int		Graphics_Hardware_D3D11_SetDrawBlendMode_PF( int /*BlendMode*/, int /*BlendParam*/ )
{
	// ディフーズカラーの更新
	GD3D11.Device.DrawInfo.DiffuseColor = GetDiffuseColor() ;

	// 正常終了
	return 0 ;
}

// 描画時のアルファテストの設定を行う( TestMode:DX_CMP_GREATER等( -1:デフォルト動作に戻す )  TestParam:描画アルファ値との比較に使用する値 )
extern	int		Graphics_Hardware_D3D11_SetDrawAlphaTest_PF( int /*TestMode*/, int /*TestParam*/ )
{
	// ディフーズカラーの更新
	GD3D11.Device.DrawInfo.DiffuseColor = GetDiffuseColor() ;

	// 正常終了
	return 0 ;
}

// 描画モードをセットする
extern	int		Graphics_Hardware_D3D11_SetDrawMode_PF( int DrawMode )
{
	Graphics_D3D11_DeviceState_SetDrawMode( DrawMode ) ;

	// 正常終了
	return 0 ;
}

// 描画輝度をセット
extern	int		Graphics_Hardware_D3D11_SetDrawBright_PF( int /*RedBright*/, int /*GreenBright*/, int /*BlueBright*/ )
{
	// ディフーズカラーの更新
	GD3D11.Device.DrawInfo.DiffuseColor = GetDiffuseColor() ;

	// 正常終了
	return 0 ;
}

// SetBlendGraphParam の可変長引数パラメータ付き
extern	int		Graphics_Hardware_D3D11_SetBlendGraphParamBase_PF( IMAGEDATA *BlendImage, int BlendType, int *Param )
{
	if( GAPIWin.D3D11DeviceObject == NULL ) return -1 ;
	
	// ブレンド画像が NULL もしくはテクスチャではなかったらブレンド画像処理の解除
	if( BlendImage == NULL || BlendImage->Orig->FormatDesc.TextureFlag == FALSE )
	{
		GSYS.DrawSetting.BlendGraph = -1 ;
		Graphics_D3D11_DrawSetting_SetBlendTexture( NULL, NULL ) ;
	}
	else
	{
		// ブレンドパラメータをセットする
		Graphics_D3D11_DrawSetting_SetBlendTexture(
			BlendImage->Hard.Draw[ 0 ].Tex[ 0 ].PF->D3D11.Texture,
			BlendImage->Hard.Draw[ 0 ].Tex[ 0 ].PF->D3D11.TextureSRV,
			BlendImage->Hard.Draw[ 0 ].Tex[ 0 ].TexWidth,
			BlendImage->Hard.Draw[ 0 ].Tex[ 0 ].TexHeight
		) ;
		Graphics_D3D11_DrawSetting_SetBlendTextureParam( BlendType, Param ) ;
	}

	// 正常終了
	return 0 ;
}

// 最大異方性の値をセットする
extern	int		Graphics_Hardware_D3D11_SetMaxAnisotropy_PF( int /*MaxAnisotropy*/ )
{
	// 正常終了
	return 0 ;
}

// ワールド変換用行列をセットする
extern	int		Graphics_Hardware_D3D11_SetTransformToWorld_PF( const MATRIX *Matrix )
{
	Graphics_D3D11_DeviceState_SetWorldMatrix( Matrix ) ;

	// 正常終了
	return 0 ;
}

// ビュー変換用行列をセットする
extern	int		Graphics_Hardware_D3D11_SetTransformToView_PF( const MATRIX *Matrix )
{
	Graphics_D3D11_DeviceState_SetViewMatrix( Matrix ) ;

	// 正常終了
	return 0 ;
}

// Direct3D11 に設定する射影行列を更新する
static void Graphics_Hardware_D3D11_RefreshProjectionMatrix( /* int Update3DProjection, int Update2DProjection, int AlwaysUpdate */ )
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
//	{
//		if( AlwaysUpdate == FALSE && Update3DProjection == FALSE )
//		{
//			return ;
//		}

		UseProjectionMatrix = &GSYS.DrawSetting.ProjectionMatrixF ;
		UseViewportMatrix   = &GSYS.DrawSetting.ViewportMatrixF ;
//	}
	if( GSYS.DrawSetting.MatchHardware_2DMatrix )
	{
		CreateMultiplyMatrix( &TempMatrix, &GSYS.DrawSetting._2DMatrixF, &GSYS.DrawSetting.Direct3DViewportMatrixAntiF ) ;
	}
	else
	{
		CreateMultiplyMatrix( &TempMatrix, UseProjectionMatrix, UseViewportMatrix                             ) ;
		CreateMultiplyMatrix( &TempMatrix, &TempMatrix,         &GSYS.DrawSetting.Direct3DViewportMatrixAntiF ) ;
	}

	Graphics_D3D11_DeviceState_SetProjectionMatrix( &TempMatrix ) ;
}

// 投影変換用行列をセットする
extern	int		Graphics_Hardware_D3D11_SetTransformToProjection_PF( const MATRIX * /*Matrix*/ )
{
	Graphics_Hardware_D3D11_RefreshProjectionMatrix() ;

	// 正常終了
	return 0 ;
}

// ビューポート行列をセットする
extern	int		Graphics_Hardware_D3D11_SetTransformToViewport_PF( const MATRIX * /* Matrix */ )
{
	Graphics_Hardware_D3D11_RefreshProjectionMatrix() ;

	// 正常終了
	return 0 ;
}

// テクスチャアドレスモードを設定する
extern	int		Graphics_Hardware_D3D11_SetTextureAddressMode_PF( int Mode /* DX_TEXADDRESS_WRAP 等 */, int Stage )
{
	Graphics_D3D11_DeviceState_SetTextureAddress( Mode, Stage ) ;

	// 正常終了
	return 0 ;
}

// テクスチャアドレスモードを設定する
extern	int		Graphics_Hardware_D3D11_SetTextureAddressModeUV_PF( int ModeU, int ModeV, int Stage )
{
	Graphics_D3D11_DeviceState_SetTextureAddressU( ModeU, Stage ) ;
	Graphics_D3D11_DeviceState_SetTextureAddressV( ModeV, Stage ) ;

	// 正常終了
	return 0 ;
}

// テクスチャ座標変換行列をセットする
extern	int		Graphics_Hardware_D3D11_SetTextureAddressTransformMatrix_PF( int UseFlag, MATRIX *Matrix, int /*Sampler*/ )
{
	Graphics_D3D11_DeviceState_SetTextureAddressTransformMatrix( UseFlag, Matrix ) ;

	// 正常終了
	return 0 ;
}

// フォグを有効にするかどうかを設定する( TRUE:有効  FALSE:無効 )
extern	int		Graphics_Hardware_D3D11_SetFogEnable_PF( int Flag )
{
	Graphics_D3D11_DeviceState_SetFogEnable( Flag ) ;

	// 正常終了
	return 0 ;
}

// フォグモードを設定する
extern	int		Graphics_Hardware_D3D11_SetFogMode_PF( int Mode /* DX_FOGMODE_NONE 等 */ )
{
	Graphics_D3D11_DeviceState_SetFogVertexMode( Mode ) ;

	// 正常終了
	return 0 ;
}

// フォグカラーを変更する
extern	int		Graphics_Hardware_D3D11_SetFogColor_PF( DWORD FogColor )
{
	Graphics_D3D11_DeviceState_SetFogColor( FogColor ) ;

	// 正常終了
	return 0 ;
}

// フォグが始まる距離と終了する距離を設定する( 0.0f 〜 1.0f )
extern	int		Graphics_Hardware_D3D11_SetFogStartEnd_PF( float start, float end )
{
	Graphics_D3D11_DeviceState_SetFogStartEnd( start, end ) ;

	// 正常終了
	return 0 ;
}

// フォグの密度を設定する( 0.0f 〜 1.0f )
extern	int		Graphics_Hardware_D3D11_SetFogDensity_PF( float density )
{
	Graphics_D3D11_DeviceState_SetFogDensity( density ) ;

	// 正常終了
	return 0 ;
}

// 基本データに設定されているフォグ情報をハードウェアに反映する
extern	int		Graphics_Hardware_D3D11_ApplyLigFogToHardware_PF( void )
{
	if( GD3D11.Device.State.FogEnable != GSYS.DrawSetting.FogEnable )
	{
		Graphics_D3D11_DeviceState_SetFogEnable( GSYS.DrawSetting.FogEnable ) ;
	}

	// 正常終了
	return 0 ;
}

// 以前の DrawModiGraph 関数のコードを使用するかどうかのフラグをセットする
extern	int		Graphics_Hardware_D3D11_SetUseOldDrawModiGraphCodeFlag_PF( int Flag )
{
	GD3D11.Setting.UseOldDrawModiGraphCodeFlag = Flag ;
	
	// 終了
	return 0 ;
}

// 描画先に正しいα値を書き込むかどうかのフラグを更新する
extern	int		Graphics_Hardware_D3D11_RefreshAlphaChDrawMode_PF( void )
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

		WorkTextureHandle = GetWorkTexture( Image->Orig->FormatDesc.FloatTypeFlag, Image->WidthF, Image->Height, 0 ) ;
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
		GD3D11.Device.DrawSetting.ChangeBlendParamFlag = TRUE ;
		GSYS.ChangeSettingFlag = TRUE ;
	}

	// 終了
	return 0 ;
}
























// 環境依存設定関係

// ピクセル単位でライティングを行うかどうかを設定する、要 ShaderModel 3.0( TRUE:ピクセル単位のライティングを行う  FALSE:頂点単位のライティングを行う( デフォルト ) )
extern	int		Graphics_Hardware_D3D11_SetUsePixelLighting_PF( int Flag )
{
	// 初期化前の場合は設定値だけ保存する
	if( DxSysData.DxLib_InitializeFlag == FALSE )
	{
		GD3D11.UsePixelLightingShader = Flag ;
	}
	else
	{
		// 初期化後の場合は D_D3D_FEATURE_LEVEL_10_0 未満の場合はピクセル単位ライティングは使用できない
		if( GD3D11.Setting.FeatureLevel < D_D3D_FEATURE_LEVEL_10_0 )
		{
			GD3D11.UsePixelLightingShader = FALSE ;
		}
		else
		{
			GD3D11.UsePixelLightingShader = Flag ;
		}
	}

	// 終了
	return 0 ;
}

// グラフィックスデバイスがロストから復帰した際に呼ばれるコールバック関数を設定する
extern	int		Graphics_Hardware_D3D11_SetGraphicsDeviceRestoreCallbackFunction_PF( void (* Callback )( void *Data ), void *CallbackData )
{
	GD3D11.Device.Setting.DeviceRestoreCallbackFunction = Callback ;
	GD3D11.Device.Setting.DeviceRestoreCallbackData = CallbackData ;

	// 終了
	return 0 ;
}

// グラフィックスデバイスがロストから復帰する前に呼ばれるコールバック関数を設定する
extern	int		Graphics_Hardware_D3D11_SetGraphicsDeviceLostCallbackFunction_PF( void (* Callback )( void *Data ), void *CallbackData )
{
	GD3D11.Device.Setting.DeviceLostCallbackFunction = Callback ;
	GD3D11.Device.Setting.DeviceLostCallbackData = CallbackData ;

	// 終了
	return 0 ;
}

// ＤＸライブラリのＤｉｒｅｃｔ３Ｄ設定をしなおす
extern	int		Graphics_Hardware_D3D11_RefreshDxLibDirect3DSetting_PF( void )
{
	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	Graphics_D3D11_DeviceState_RefreshRenderState() ;

	// 初期化フラグを立てる
	GD3D11.Device.DrawSetting.CancelSettingEqualCheck = TRUE ;

	// ブレンドモードの再設定
	Graphics_D3D11_DrawSetting_SetDrawBlendMode( GD3D11.Device.DrawSetting.BlendMode, GD3D11.Device.DrawSetting.AlphaTestValidFlag, GD3D11.Device.DrawSetting.AlphaChannelValidFlag ) ;
	Graphics_D3D11_DrawPreparation() ;

	// 初期化フラグを倒す
	GD3D11.Device.DrawSetting.CancelSettingEqualCheck = FALSE ;

	// 終了
	return 0 ;
}

// Direct3D11 で使用する最低機能レベルを指定する関数です、尚、DX_DIRECT3D_11_FEATURE_LEVEL_11_0 より低い機能レベルでの正常な動作は保証しません( デフォルトは DX_DIRECT3D_11_FEATURE_LEVEL_11_0 )
extern int Graphics_D3D11_SetUseDirect3D11MinFeatureLevel_PF( int Level /* DX_DIRECT3D_11_FEATURE_LEVEL_10_0 など */ )
{
	// 初期化前のみ有効
	if( DxSysData.DxLib_InitializeFlag == TRUE ) return -1 ;

	// フラグを保存
	GD3D11.Setting.UseMinFeatureLevelDirect3D11 = Level ;

	// 終了
	return 0 ;
}

// D3D_DRIVER_TYPE_WARP タイプの Direct3D 11 ドライバを使用するかどうかを設定する( TRUE:使用する  FALSE:使用しない( デフォルト ) )
extern int NS_SetUseDirect3D11WARPDriver( int Flag )
{
	// 初期化前のみ有効
	if( DxSysData.DxLib_InitializeFlag == TRUE ) return -1 ;

	// フラグを保存
	GD3D11.Setting.UseDirect3D11WARPTypeDriver = Flag ;

	// 終了
	return 0 ;
}






























// 環境依存画面関係

// 設定に基づいて使用するＺバッファをセットする
extern	int		Graphics_Hardware_D3D11_SetupUseZBuffer_PF_Base( D_ID3D11DepthStencilView *pID3D11DepthStencilView )
{
	IMAGEDATA *TargetScreenImage = NULL ;
	IMAGEDATA *TargetZBufferImage = NULL ;
	SHADOWMAPDATA *TargetShadowMap = NULL ;

	// シャドウマップかどうかをチェック
	if( !SHADOWMAPCHKFULL( GSYS.DrawSetting.TargetScreen[ 0 ], TargetShadowMap ) )
	{
		// シャドウマップの場合は専用のＺバッファを使用する
		Graphics_D3D11_DeviceState_SetDepthStencil( TargetShadowMap->PF->D3D11.DepthBuffer, TargetShadowMap->PF->D3D11.DepthBufferDSV ) ;
	}
	else
	// ID3D11DepthStencilView が直接指定された場合はそれを描画対称にする
	if( pID3D11DepthStencilView != NULL )
	{
		D_ID3D11Texture2D *pID3D11DepthBuffer = NULL ;
		D_D3D11_TEXTURE2D_DESC DepthBufferDesc ;

		D3D11View_GetResource( pID3D11DepthStencilView, ( D_ID3D11Resource ** )&pID3D11DepthBuffer ) ;
		D3D11Texture2D_GetDesc( pID3D11DepthBuffer, &DepthBufferDesc ) ;

		Graphics_D3D11_DeviceState_SetDepthStencil(
			pID3D11DepthBuffer,
			pID3D11DepthStencilView
		) ;
	}
	else
	{
		// 描画先の画像情報の取得
		GRAPHCHKFULL( GSYS.DrawSetting.TargetScreen[ 0 ], TargetScreenImage ) ;

		// 使用するＺバッファの画像情報の取得
		GRAPHCHKFULL( GSYS.DrawSetting.TargetZBuffer, TargetZBufferImage ) ;

		// マスクサーフェスが存在していて且つ有効な場合はマスクサーフェスのＺバッファを使用する
//#ifndef DX_NON_MASK
//		if( MASKD.MaskValidFlag && MASKD3D9.MaskScreenSurface )
//		{
//			Direct3DDevice9_SetDepthStencilSurface( GD3D9.Device.Screen.ZBufferSurface ) ;
//		}
//		else
//#endif
		// 描画可能画像が描画先かどうかで処理を分岐
		if( TargetScreenImage )
		{
			// 描画先Ｚバッファがあって、且つ描画先以上のサイズを持つ場合は描画先Ｚバッファを使用する
			if( TargetZBufferImage != NULL &&
				TargetZBufferImage->WidthI  >= TargetScreenImage->WidthI &&
				TargetZBufferImage->HeightI >= TargetScreenImage->HeightI )
			{
				Graphics_D3D11_DeviceState_SetDepthStencil(
					TargetZBufferImage->Hard.Draw[ 0 ].Tex->PF->D3D11.DepthBuffer,
					TargetZBufferImage->Hard.Draw[ 0 ].Tex->PF->D3D11.DepthBufferDSV[ GSYS.DrawSetting.TargetScreenMipLevel[ 0 ] ]
				) ;
			}
			else
			// 描画可能画像のＺバッファがある場合はそのＺバッファを使用する
			if( TargetScreenImage->Hard.Draw[ 0 ].Tex->PF->D3D11.DepthBuffer )
			{
				Graphics_D3D11_DeviceState_SetDepthStencil(
					TargetScreenImage->Hard.Draw[ 0 ].Tex->PF->D3D11.DepthBuffer,
					TargetScreenImage->Hard.Draw[ 0 ].Tex->PF->D3D11.DepthBufferDSV[ GSYS.DrawSetting.TargetScreenMipLevel[ 0 ] ]
				) ;
			}
			else
			// 描画先がデフォルトのＺバッファと大きさが異なったらＺバッファを外す
			if( TargetScreenImage->WidthI  != GD3D11.Device.Screen.DepthBufferSizeX || 
				TargetScreenImage->HeightI != GD3D11.Device.Screen.DepthBufferSizeY )
			{
				Graphics_D3D11_DeviceState_SetDepthStencil( NULL, NULL ) ;
			}
			else
			{
				// デフォルトのＺバッファを使用する
				Graphics_D3D11_DeviceState_SetDepthStencil( GD3D11.Device.Screen.DepthBufferTexture2D, GD3D11.Device.Screen.DepthBufferDSV ) ;
			}
		}
		else
		{
			// 描画先Ｚバッファがあって、且つ描画先と同じサイズを持つ場合は描画先Ｚバッファを使用する
			if( TargetZBufferImage != NULL &&
				TargetZBufferImage->WidthI  == GSYS.DrawSetting.DrawSizeX &&
				TargetZBufferImage->HeightI == GSYS.DrawSetting.DrawSizeY )
			{
				Graphics_D3D11_DeviceState_SetDepthStencil(
					TargetZBufferImage->Hard.Draw[ 0 ].Tex->PF->D3D11.DepthBuffer,
					TargetZBufferImage->Hard.Draw[ 0 ].Tex->PF->D3D11.DepthBufferDSV[ GSYS.DrawSetting.TargetScreenMipLevel[ 0 ] ]
				) ;
			}
			else
			{
				// デフォルトのＺバッファを使用する
				Graphics_D3D11_DeviceState_SetDepthStencil( GD3D11.Device.Screen.DepthBufferTexture2D, GD3D11.Device.Screen.DepthBufferDSV ) ;
			}
		}
	}

	// 終了
	return 0 ;
}

extern	int		Graphics_Hardware_D3D11_SetupUseZBuffer_PF( void )
{
	return Graphics_Hardware_D3D11_SetupUseZBuffer_PF_Base( NULL ) ;
}

// 画面のＺバッファの状態を初期化する
extern	int		Graphics_Hardware_D3D11_ClearDrawScreenZBuffer_PF( const RECT * /*ClearRect*/ )
{
	// 描画対象の深度バッファを初期化
	D3D11DeviceContext_ClearDepthStencilView_ASync( GD3D11.Device.State.TargetDepthDSV, D_D3D11_CLEAR_DEPTH, 1.0f, 0 ) ;

	// 終了
	return 0 ;
}

// 画面の状態を初期化する
extern	int		Graphics_Hardware_D3D11_ClearDrawScreen_PF( const RECT * /*ClearRect*/ )
{
	D_ID3D11Texture2D        *TargetTexture2D ;
	D_ID3D11RenderTargetView *TargetRTV ;
	FLOAT                    ClearColor[ 4 ] ;
	IMAGEDATA                *Image = NULL ;
	SHADOWMAPDATA            *ShadowMap = NULL ;
	int                      i ;
//	HRESULT hr ;

	if( GAPIWin.D3D11DeviceObject == NULL ) return 0 ;

	for( i = 0 ; i < GSYS.HardInfo.RenderTargetNum ; i ++ )
	{
		// 無効な描画先だった場合は何もしない
		if( GSYS.DrawSetting.TargetScreen[ i ] == 0 )
		{
			continue ;
		}

		// 描画先画像の画像情報を取得
		Image     = NULL ;
		ShadowMap = NULL ;
		if( GRAPHCHKFULL( GSYS.DrawSetting.TargetScreen[ i ], Image ) && i != 0 )
		{
			continue ;
		}
		SHADOWMAPCHKFULL( GSYS.DrawSetting.TargetScreen[ i ], ShadowMap ) ;

		// 描画待機している描画物を描画
		DRAWSTOCKINFO

		// 描画の終了
	//	Graphics_D3D9_EndScene() ;

		// シャドウマップが使用されている場合はシャドウマップを初期化
		if( ShadowMap )
		{
			TargetTexture2D = ShadowMap->PF->D3D11.DepthTexture ;
			TargetRTV       = ShadowMap->PF->D3D11.DepthTextureRTV ;
		}
		else
		// 描画可能画像が使用されている場合は描画可能画像を初期化
		if( Image )
		{
			TargetTexture2D = Image->Orig->Hard.Tex[ 0 ].PF->D3D11.Texture ;
			TargetRTV       = Image->Orig->Hard.Tex[ 0 ].PF->D3D11.TextureRTV[ GSYS.DrawSetting.TargetScreenSurface[ i ] * Image->Orig->Hard.MipMapCount + GSYS.DrawSetting.TargetScreenMipLevel[ i ] ] ;
		}
		else
		// その他の描画先の場合は ID3D11RenderTargetView が直接指定されたと判断する
		if( GSYS.DrawSetting.TargetScreen[ i ] == DX_SCREEN_OTHER )
		{
			TargetTexture2D = GD3D11.Device.State.TargetTexture2D[ i ] ;
			TargetRTV       = GD3D11.Device.State.TargetRTV[ i ] ;
		}
		else
		// サブバックバッファが有効になっている場合はサブバックバッファを初期化
		if( GD3D11.Device.Screen.SubBackBufferTexture2D != NULL )
		{
			TargetTexture2D = GD3D11.Device.Screen.SubBackBufferTexture2D ;
			TargetRTV       = GD3D11.Device.Screen.SubBackBufferRTV ;
		}
		else
		{
			TargetTexture2D = GD3D11.Device.Screen.OutputWindowInfo[ 0 ].BufferTexture2D ;
			TargetRTV       = GD3D11.Device.Screen.OutputWindowInfo[ 0 ].BufferRTV ;
		}

		// 描画対象のカラーバッファを初期化
		ClearColor[ 0 ] = GSYS.Screen.BackgroundRed   / 255.0f ;
		ClearColor[ 1 ] = GSYS.Screen.BackgroundGreen / 255.0f ;
		ClearColor[ 2 ] = GSYS.Screen.BackgroundBlue  / 255.0f ;
		ClearColor[ 3 ] = 0.0f ;
		D3D11DeviceContext_ClearRenderTargetView_ASync( TargetRTV, ClearColor ) ;
	}

	// 描画対象の深度バッファを初期化
	D3D11DeviceContext_ClearDepthStencilView_ASync( GD3D11.Device.State.TargetDepthDSV, D_D3D11_CLEAR_DEPTH, 1.0f, 0 ) ;

	// 正常終了
	return 0 ;
}

// 描画先画面のセット
static	int		Graphics_Hardware_D3D11_SetDrawScreen_PF_Base( int /*DrawScreen*/, int /*OldScreenSurface*/, int /*OldScreenMipLevel*/, IMAGEDATA *NewTargetImage, IMAGEDATA *OldTargetImage, SHADOWMAPDATA *NewTargetShadowMap, SHADOWMAPDATA * /*OldTargetShadowMap*/, D_ID3D11RenderTargetView *pID3D11RenderTargetView, D_ID3D11DepthStencilView *pID3D11DepthStencilView )
{
	D_ID3D11ShaderResourceView *pShaderResourceViews[ D_D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT ] = { NULL } ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// セットしていたテクスチャーを外す
	Graphics_D3D11_DrawSetting_SetTexture( NULL, NULL ) ;
	Graphics_D3D11_DeviceState_SetPSShaderResouceView( 0, D_D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pShaderResourceViews ) ;

	// 今までの描画先がマルチサンプルタイプのレンダリングターゲットだった場合はダウンサンプリングする
	if( OldTargetImage != NULL && OldTargetImage->Hard.Draw[ 0 ].Tex->PF->D3D11.MSTexture != NULL )
	{
		Graphics_D3D11_UpdateDrawTexture(
			OldTargetImage->Hard.Draw[ 0 ].Tex,
			GSYS.DrawSetting.TargetScreenSurface[ 0 ],
			GD3D11.Device.Caps.TextureFormat[ OldTargetImage->Orig->ColorFormat ]
		) ;
	}

	// 描画先の変更

	// マスクサーフェスが存在していて且つ有効な場合はマスクサーフェスを描画対象にする
#ifndef DX_NON_MASK
	if( MASKD.MaskValidFlag && MASKD3D11.MaskScreenTexture )
	{
		Graphics_D3D11_DeviceState_SetRenderTarget( MASKD3D11.MaskScreenTexture, MASKD3D11.MaskScreenTextureRTV ) ;
	}
	else
#endif
	// シャドウマップが有効な場合はシャドウマップを描画対象にする
	if( NewTargetShadowMap )
	{
		Graphics_D3D11_DeviceState_SetRenderTarget( NewTargetShadowMap->PF->D3D11.DepthTexture, NewTargetShadowMap->PF->D3D11.DepthTextureRTV, 0 ) ;
	}
	else
	// 描画可能画像が有効な場合は描画可能画像を描画対象にする
	if( NewTargetImage )
	{
		Graphics_D3D11_DeviceState_SetRenderTarget(
			NewTargetImage->Hard.Draw[ 0 ].Tex->PF->D3D11.Texture,
			NewTargetImage->Hard.Draw[ 0 ].Tex->PF->D3D11.TextureRTV[ GSYS.DrawSetting.TargetScreenSurface[ 0 ] * NewTargetImage->Orig->Hard.MipMapCount + GSYS.DrawSetting.TargetScreenMipLevel[ 0 ] ],
			0
		) ;
	}
	else
	// ID3D11RenderTargetView が直接指定された場合はそれを描画対象にする
	if( pID3D11RenderTargetView )
	{
		D_ID3D11Texture2D *pID3D11Texture2D = NULL ;

		D3D11View_GetResource( pID3D11RenderTargetView, ( D_ID3D11Resource ** )&pID3D11Texture2D ) ;
		Graphics_D3D11_DeviceState_SetRenderTarget( pID3D11Texture2D, pID3D11RenderTargetView ) ;
	}
	else
	// サブバックバッファが有効な場合はサブバックバッファを描画対象にする
	if( GD3D11.Device.Screen.SubBackBufferTexture2D != NULL )
	{
		Graphics_D3D11_DeviceState_SetRenderTarget( GD3D11.Device.Screen.SubBackBufferTexture2D, GD3D11.Device.Screen.SubBackBufferRTV ) ;
	}
	else
	// それ以外の場合はバックバッファを描画対象にする
	{
		Graphics_D3D11_DeviceState_SetRenderTarget( GD3D11.Device.Screen.OutputWindowInfo[ 0 ].BufferTexture2D, GD3D11.Device.Screen.OutputWindowInfo[ 0 ].BufferRTV ) ;
	}

	// 使用するＺバッファのセットアップ
	Graphics_Hardware_D3D11_SetupUseZBuffer_PF_Base( pID3D11DepthStencilView ) ;

	// 正常終了
	return 0 ;
}
extern	int		Graphics_Hardware_D3D11_SetDrawScreen_PF( int DrawScreen, int OldScreenSurface, int OldScreenMipLevel, IMAGEDATA *NewTargetImage, IMAGEDATA *OldTargetImage, SHADOWMAPDATA *NewTargetShadowMap, SHADOWMAPDATA *OldTargetShadowMap )
{
	return Graphics_Hardware_D3D11_SetDrawScreen_PF_Base( DrawScreen, OldScreenSurface, OldScreenMipLevel, NewTargetImage, OldTargetImage, NewTargetShadowMap, OldTargetShadowMap, NULL, NULL ) ;
}

// 定期的に ScreenCopy を行うコールバック関数
#define GRAPHICS_HARDWARE_D3D11_SCREENFLIPTIMER_ID		(32767)
#if _MSC_VER > 1200 || defined( DX_GCC_COMPILE_4_9_2 )
static VOID CALLBACK Graphics_Hardware_D3D11_ScreenFlipTimerProc( HWND /*hwnd*/, UINT /*uMsg*/, UINT_PTR /*idEvent*/, DWORD /*dwTime*/ )
#else
static VOID CALLBACK Graphics_Hardware_D3D11_ScreenFlipTimerProc( HWND /*hwnd*/, UINT /*uMsg*/, UINT     /*idEvent*/, DWORD /*dwTime*/ )
#endif
{
	SETUP_WIN_API

	WinAPIData.Win32Func.KillTimerFunc( NS_GetMainWindowHandle(), GRAPHICS_HARDWARE_D3D11_SCREENFLIPTIMER_ID ) ;

	if( GSYS.DrawSetting.TargetScreen[ 0 ]        == DX_SCREEN_FRONT &&
		GSYS.DrawSetting.TargetScreenSurface[ 0 ] == 0               &&
		WinData.BackBufferTransColorFlag          == FALSE           &&
		WinData.UseUpdateLayerdWindowFlag         == FALSE )
	{
		if( NS_GetActiveFlag() == TRUE )
		{
			NS_ScreenCopy() ;
		}
		WinAPIData.Win32Func.SetTimerFunc( NS_GetMainWindowHandle(), GRAPHICS_HARDWARE_D3D11_SCREENFLIPTIMER_ID, 32, Graphics_Hardware_D3D11_ScreenFlipTimerProc ) ;
	}
}

// SetDrawScreen の最後で呼ばれる関数
extern	int		Graphics_Hardware_D3D11_SetDrawScreen_Post_PF( int DrawScreen )
{
	SETUP_WIN_API

	if( DrawScreen == DX_SCREEN_FRONT )
	{
		WinAPIData.Win32Func.SetTimerFunc( NS_GetMainWindowHandle(), GRAPHICS_HARDWARE_D3D11_SCREENFLIPTIMER_ID, 32, Graphics_Hardware_D3D11_ScreenFlipTimerProc ) ;
	}

	// 正常終了
	return 0 ;
}

// 描画可能領域のセット
extern	int		Graphics_Hardware_D3D11_SetDrawArea_PF( int /*x1*/, int /*y1*/, int /*x2*/, int /*y2*/ )
{
	D_D3D11_VIEWPORT Viewport ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	if( GSYS.DrawSetting.DrawArea.right  - GSYS.DrawSetting.DrawArea.left == 0 ||
		GSYS.DrawSetting.DrawArea.bottom - GSYS.DrawSetting.DrawArea.top  == 0 )
	{
		return -1 ;
	}

	// ビューポートのセット
	Viewport.TopLeftX	= ( float )GSYS.DrawSetting.DrawArea.left ;
	Viewport.TopLeftY	= ( float )GSYS.DrawSetting.DrawArea.top ;
	Viewport.Width		= ( float )( GSYS.DrawSetting.DrawArea.right  - GSYS.DrawSetting.DrawArea.left ) ;
	Viewport.Height		= ( float )( GSYS.DrawSetting.DrawArea.bottom - GSYS.DrawSetting.DrawArea.top  ) ;
	Viewport.MinDepth	= 0.0f ;
	Viewport.MaxDepth	= 1.0f ;
	Graphics_D3D11_DeviceState_SetViewport( &Viewport ) ;

	// 射影行列とビューポート行列の逆行列を乗算したものをセット
	Graphics_Hardware_D3D11_RefreshProjectionMatrix() ;
	GSYS.DrawSetting.MatchHardwareProjectionMatrix = TRUE ;

	// アンチビューポート行列を更新
	Graphics_D3D11_DeviceState_SetAntiViewportMatrix( &GSYS.DrawSetting.Direct3DViewportMatrixAntiF ) ;

	// 正常終了
	return 0 ;
}

// 描画先バッファをロックする
extern	int		Graphics_Hardware_D3D11_LockDrawScreenBuffer_PF( RECT *LockRect, BASEIMAGE *BaseImage, int /*TargetScreen*/, IMAGEDATA *TargetImage, int TargetScreenSurface, int TargetScreenMipLevel, int /*ReadOnly*/, int TargetScreenTextureNo )
{
	D_ID3D11Texture2D                 *TargetTexture ;
	D_ID3D11ShaderResourceView        *TargetTextureSRV ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 取り込み元となるサーフェスの決定

	// 描画可能画像が対象の場合
	if( TargetImage )
	{
		TargetTexture    = TargetImage->Orig->Hard.Tex[ TargetScreenTextureNo ].PF->D3D11.Texture ;
		TargetTextureSRV = TargetImage->Orig->Hard.Tex[ TargetScreenTextureNo ].PF->D3D11.TextureSRV ;
	}
	else
	// サブバックバッファが有効になっている場合はサブバックバッファを初期化
	if( GD3D11.Device.Screen.SubBackBufferTexture2D != NULL )
	{
		TargetTexture    = GD3D11.Device.Screen.SubBackBufferTexture2D ;
		TargetTextureSRV = GD3D11.Device.Screen.SubBackBufferSRV ;
	}
	else
	{
		TargetTexture    = GD3D11.Device.Screen.OutputWindowInfo[ 0 ].BufferTexture2D ;
		TargetTextureSRV = GD3D11.Device.Screen.OutputWindowInfo[ 0 ].BufferSRV ;
	}

	// マップ
	if( Graphics_D3D11_Texture2D_Map(
			TargetTexture,
			TargetTextureSRV,
			TargetScreenSurface,
			TargetScreenMipLevel,
			LockRect,
			BaseImage,
			&GD3D11.Device.Screen.DrawScreenBufferLockSMTexture ) < 0 )
	{
		DXST_LOGFILE_ADDUTF16LE( "\xcf\x63\x3b\x75\xfe\x5b\x61\x8c\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\xed\x30\xc3\x30\xaf\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"描画対象バッファのロックに失敗しました\n" @*/ ) ;
		return -1 ;
	}

	// 正常終了
	return 0 ;
}

// 描画先バッファをアンロックする
extern	int		Graphics_Hardware_D3D11_UnlockDrawScreenBuffer_PF( void )
{
	// マップを解除
	Graphics_D3D11_Texture2D_Unmap( GD3D11.Device.Screen.DrawScreenBufferLockSMTexture ) ;

	// 正常終了
	return 0 ;
}

// 裏画面の内容を表画面に描画する
extern	int		Graphics_Hardware_D3D11_ScreenCopy_PF( void )
{
	// サブバックバッファのセットアップ
	if( NS_GetWindowModeFlag() == FALSE )
	{
		Graphics_D3D11_SetupSubBackBuffer() ;
	}

	// 正常終了
	return 0 ;
}

// 垂直同期信号を待つ
extern	int		Graphics_Hardware_D3D11_WaitVSync_PF( int SyncNum )
{
	int i ;

	for( i = 0 ; i < SyncNum ; i ++ )
	{
		DXGIOutput_WaitForVBlank( GD3D11.Device.Screen.OutputWindowInfo[ 0 ].DXGIOutput ) ;
	}

	return 0 ;
}

// 裏画面と表画面を交換する
extern	int		Graphics_Hardware_D3D11_ScreenFlipBase_PF( void )
{
	GRAPHICS_HARDDATA_DIRECT3D11_OUTPUTWINDOWINFO *OWI ;

	SETUP_WIN_API

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

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
		Graphics_D3D11_SetupSubBackBuffer() ;
	}

	// 裏画面の内容を表画面に反映
//	return DXGISwapChain_Present() ;

	// 出力先ウインドウの情報を取得しておく
	OWI = &GD3D11.Device.Screen.OutputWindowInfo[ GD3D11.Device.Screen.TargetOutputWindow ] ;

	// ウインドウのクライアント領域のサイズがバックバッファーのサイズと異なる場合はバックバッファのサイズを変更する
	{
		RECT TargetWindowClientRect ;
		SIZE TargetWindowClientSize ;

		if( GD3D11.Device.Screen.TargetOutputWindow == 0 )
		{
			TargetWindowClientRect = WinData.WindowRect ;
		}
		else
		{
			WinAPIData.Win32Func.GetClientRectFunc( GD3D11.Device.Screen.OutputWindowInfo[ GD3D11.Device.Screen.TargetOutputWindow ].DXGISwapChainDesc.OutputWindow, &TargetWindowClientRect ) ;
		}

		TargetWindowClientSize.cx = TargetWindowClientRect.right  - TargetWindowClientRect.left ;
		TargetWindowClientSize.cy = TargetWindowClientRect.bottom - TargetWindowClientRect.top ;

		if( ( TargetWindowClientSize.cx != 0 && TargetWindowClientSize.cy != 0 ) &&
			( ( UINT )TargetWindowClientSize.cx != OWI->BufferTexture2DDesc.Width ||
			  ( UINT )TargetWindowClientSize.cy != OWI->BufferTexture2DDesc.Height ) )
		{
			// サブバックバッファを先にセットアップする
			Graphics_D3D11_SetupSubBackBuffer() ;

			Graphics_D3D11_OutputWindow_ResizeBuffer(
				GD3D11.Device.Screen.TargetOutputWindow,
				TargetWindowClientSize.cx,
				TargetWindowClientSize.cy
			) ;
		}
	}

	// サブバックバッファを使用している場合は内容を出力先ウインドウのバックバッファに転送する
	if( GD3D11.Device.Screen.SubBackBufferTexture2D != NULL )
	{
		RECT SrcRect ;
		RECT DestRect = { 0 } ;

		// サブバックバッファに転送する領域を設定する
		if( GSYS.Screen.ValidGraphDisplayArea )
		{
			RECT ClipRect ;

			ClipRect.left   = 0 ;
			ClipRect.top    = 0 ;
			ClipRect.right  = GSYS.Screen.MainScreenSizeX ;
			ClipRect.bottom = GSYS.Screen.MainScreenSizeY ;

			SrcRect = GSYS.Screen.GraphDisplayArea ;
			RectClipping_Inline( &SrcRect, &ClipRect ) ;
			if( SrcRect.right  - SrcRect.left <= 0 ||
				SrcRect.bottom - SrcRect.top  <= 0 )
			{
				return -1 ;
			}
		}
		else
		{
			SrcRect.left   = 0 ;
			SrcRect.top    = 0 ;
			SrcRect.right  = GSYS.Screen.MainScreenSizeX ;
			SrcRect.bottom = GSYS.Screen.MainScreenSizeY ;
		}

		if( NS_GetWindowModeFlag() == TRUE )
		{
			// メインウインドウ以外への転送の場合は最大化関係なし
			if( GD3D11.Device.Screen.TargetOutputWindow != 0 )
			{
				double WindowSizeExRateX ;
				double WindowSizeExRateY ;

				NS_GetWindowSizeExtendRate( &WindowSizeExRateX, &WindowSizeExRateY ) ;

				DestRect.left   = 0 ;
				DestRect.top    = 0 ;
				DestRect.right  = _DTOL( GSYS.Screen.MainScreenSizeX * WindowSizeExRateX * GRAWIN.Setting.ScreenFlipTargetWindowScaleX ) ;
				DestRect.bottom = _DTOL( GSYS.Screen.MainScreenSizeY * WindowSizeExRateY * GRAWIN.Setting.ScreenFlipTargetWindowScaleY ) ;
			}
			else
			{
				// 320x240画面エミュレーションモードの場合は転送先の矩形は固定
				if( GSYS.Screen.Emulation320x240Flag )
				{
					SrcRect.left    = 0 ;
					SrcRect.top     = 0 ;
					SrcRect.right   = 320 ;
					SrcRect.bottom  = 240 ;

					DestRect.left   = 0 ;
					DestRect.top    = 0 ;
					DestRect.right  = 640 ;
					DestRect.bottom = 480 ;
				}
				else
				{
					double ExRateX ;
					double ExRateY ;

					NS_GetWindowSizeExtendRate( &ExRateX, &ExRateY ) ;

					if( WinData.WindowMaximizeFlag &&
						WinData.ScreenNotFitWindowSize == FALSE &&
						WinData.WindowSizeValid == FALSE )
					{
						DestRect.left   = ( ( WinData.WindowMaximizedClientRect.right  - WinData.WindowMaximizedClientRect.left ) - _DTOL( GSYS.Screen.MainScreenSizeX * ExRateX ) ) / 2 ;
						DestRect.top    = ( ( WinData.WindowMaximizedClientRect.bottom - WinData.WindowMaximizedClientRect.top  ) - _DTOL( GSYS.Screen.MainScreenSizeY * ExRateY ) ) / 2 ;
					}
					else
					{
						DestRect.left   = 0 ;
						DestRect.top    = 0 ;
					}

					DestRect.right  = DestRect.left + _DTOL( GSYS.Screen.MainScreenSizeX * ExRateX ) ;
					DestRect.bottom = DestRect.top  + _DTOL( GSYS.Screen.MainScreenSizeY * ExRateY ) ;

					if( WinData.ToolBarUseFlag )
					{
						int ClientHeight ;

						ClientHeight   = WinData.WindowRect.bottom - WinData.WindowRect.top ;
						DestRect.top    = ClientHeight - GSYS.Screen.MainScreenSizeY ;
						DestRect.bottom = ClientHeight ;
					}
				}
			}
		}
		else
		{
			// フルスクリーンモードの場合はフルスクリーン解像度モードによって処理を分岐
			switch( GSYS.Screen.FullScreenResolutionModeAct )
			{
			case DX_FSRESOLUTIONMODE_NATIVE :
				DestRect.left   = 0 ;
				DestRect.top    = 0 ;
				DestRect.right  = GSYS.Screen.MainScreenSizeX ;
				DestRect.bottom = GSYS.Screen.MainScreenSizeY ;
				break ;

			case DX_FSRESOLUTIONMODE_MAXIMUM :
			case DX_FSRESOLUTIONMODE_DESKTOP :
				// 転送先矩形のセットアップ
				Graphics_Screen_SetupFullScreenScalingDestRect() ;

				DestRect = GSYS.Screen.FullScreenScalingDestRect ;
				break ;
			}
		}

		if( DestRect.right  > ( int )OWI->BufferTexture2DDesc.Width )
		{
			SrcRect.right  -= ( DestRect.right - OWI->BufferTexture2DDesc.Width ) * SrcRect.right / DestRect.right ;
			DestRect.right  = ( LONG )OWI->BufferTexture2DDesc.Width ;
		}
		if( DestRect.bottom > ( int )OWI->BufferTexture2DDesc.Height )
		{
			SrcRect.bottom -= ( DestRect.bottom - OWI->BufferTexture2DDesc.Height ) * SrcRect.bottom / DestRect.bottom ;
			DestRect.bottom = ( LONG )OWI->BufferTexture2DDesc.Height ;
		}

		// サブバックバッファと本バックバッファのサイズが異なる場合は本バックバッファを先にクリアする
		if( OWI->BufferTexture2DDesc.Width  != ( DWORD )GD3D11.Device.Screen.SubBackBufferTextureSizeX ||
			OWI->BufferTexture2DDesc.Height != ( DWORD )GD3D11.Device.Screen.SubBackBufferTextureSizeY )
		{
			FLOAT ClearColor[ 4 ] ;

			ClearColor[ 0 ] = GSYS.Screen.BackgroundRed   / 255.0f ;
			ClearColor[ 1 ] = GSYS.Screen.BackgroundGreen / 255.0f ;
			ClearColor[ 2 ] = GSYS.Screen.BackgroundBlue  / 255.0f ;
			ClearColor[ 3 ] = 0.0f ;
			D3D11DeviceContext_ClearRenderTargetView_ASync( OWI->BufferRTV, ClearColor ) ;
		}

		// サブバックバッファの内容を本バックバッファに転送
		Graphics_D3D11_StretchRect(
			GD3D11.Device.Screen.SubBackBufferTexture2D,
			GD3D11.Device.Screen.SubBackBufferSRV,
			&SrcRect,
			OWI->BufferTexture2D,
			OWI->BufferRTV,
			&DestRect,
			GSYS.Screen.FullScreenScalingMode == DX_FSSCALINGMODE_BILINEAR ? D_D3D11_FILTER_TYPE_LINEAR : D_D3D11_FILTER_TYPE_POINT
		) ;
	}

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// バックバッファの透過色の部分を透過するフラグか、UpdateLayerdWindow を使用するフラグが立っている場合は処理を分岐
	if( WinData.BackBufferTransColorFlag || WinData.UseUpdateLayerdWindowFlag )
	{
		BASEIMAGE BackBufferImage ;
		RECT      LockRect ;
		int       OldTargetScreen ;
		int       OldTargetScreenSurfaceIndex ;

		// ＶＳＹＮＣを待つ	
		if( GSYS.Screen.NotWaitVSyncFlag == FALSE )
		{
			DXGIOutput_WaitForVBlank( GD3D11.Device.Screen.OutputWindowInfo[ 0 ].DXGIOutput ) ;
		}

		// Graphics_Screen_LockDrawScreen を使う方法
		OldTargetScreen                           = GSYS.DrawSetting.TargetScreen[ 0 ] ;
		OldTargetScreenSurfaceIndex               = GSYS.DrawSetting.TargetScreenSurface [ 0 ] ;
		GSYS.DrawSetting.TargetScreen[ 0 ]        = DX_SCREEN_BACK ;
		GSYS.DrawSetting.TargetScreenSurface[ 0 ] = 0 ;
		LockRect.left                             = 0 ;
		LockRect.top                              = 0 ;
		LockRect.right                            = GSYS.Screen.MainScreenSizeX ;
		LockRect.bottom                           = GSYS.Screen.MainScreenSizeY ;
		if( Graphics_Screen_LockDrawScreen( &LockRect, &BackBufferImage, -1, -1, -1, TRUE, 0 ) < 0 )
		{
			goto ERR ;
		}
		UpdateBackBufferTransColorWindow( &BackBufferImage ) ;
		Graphics_Screen_UnlockDrawScreen() ;
		GSYS.DrawSetting.TargetScreen[ 0 ]        = OldTargetScreen ;
		GSYS.DrawSetting.TargetScreenSurface[ 0 ] = OldTargetScreenSurfaceIndex ;
	}
	else
	{
		// フリップ
		if( OWI->DXGISwapChain )
		{
//			// 最小化していないときだけ Present を実行
//			if( WinData.WindowMinSizeFlag == FALSE )
			{
				// ( フリップしないと使用メモリが増える現象が発生するので、最小化されていてもフリップを行う )
				if( FAILED( DXGISwapChain_Present( OWI->DXGISwapChain, ( UINT )( GSYS.Screen.NotWaitVSyncFlag ? 0 : 1 ), 0 ) ) )
				{
					goto ERR ;
				}
			}
//			else
//			{
//				// 最小化している場合でVSYNC待ちをする設定になっている場合は VSYNC 待ちを行う
//				if( GSYS.Screen.NotWaitVSyncFlag == FALSE )
//				{
//					DXGIOutput_WaitForVBlank( OWI->DXGIOutput ) ;
//				}
//			}
		}
	}

	// Direct3D 11 に対する設定を再度行う
	NS_RefreshDxLibDirect3DSetting() ;

	// 終了
	return 0 ;

ERR:
	// エラー終了
	return -1;
}

// 裏画面の指定の領域をウインドウのクライアント領域の指定の領域に転送する
extern	int		Graphics_Hardware_D3D11_BltRectBackScreenToWindow_PF( HWND /*Window*/, RECT /*BackScreenRect*/, RECT /*WindowClientRect*/ )
{
	// 未実装
	return 0 ;
}

// ScreenFlip で画像を転送する先のウインドウを設定する( NULL を指定すると設定解除 )
extern	int		Graphics_Hardware_D3D11_SetScreenFlipTargetWindow_PF( HWND TargetWindow, double /*ScaleX*/, double /*ScaleY*/ )
{
	int Result ;

	if( TargetWindow == NULL )
	{
		Result = Graphics_D3D11_OutputWindow_Add( GetDisplayWindowHandle(), TRUE ) ;
	}
	else
	{
		Result = Graphics_D3D11_OutputWindow_Add( TargetWindow ) ;
	}
	if( Result == -1 )
	{
		return -1 ;
	}

	// メインウインドウ以外がターゲットになった場合はサブバックバッファをセットアップする
	if( Result > 0 )
	{
		if( Graphics_D3D11_SetupSubBackBuffer() < 0 )
		{
			return -1 ;
		}
	}

	// ターゲットウインドウの配列インデックスを保存
	GD3D11.Device.Screen.TargetOutputWindow = Result ;

	// 正常終了
	return 0 ;
}

// メイン画面のＺバッファの設定を変更する
extern	int		Graphics_Hardware_D3D11_SetZBufferMode_PF( int /*ZBufferSizeX*/, int /*ZBufferSizeY*/, int ZBufferBitDepth )
{
	// 初期化前の場合のみ有効
	if( GAPIWin.D3D11DeviceObject != NULL )
		return 0 ;

	if( ZBufferBitDepth > 0 )
	{
		if( ZBufferBitDepth != 16 && ZBufferBitDepth != 24 && ZBufferBitDepth != 32 )
		{
			return -1 ;
		}
		GD3D11.Device.Screen.DepthBufferBitDepth = ZBufferBitDepth ;
		GD3D11.Setting.UserDepthBufferBitDepthSet = TRUE ;
	}

	return 0 ;
}

// 描画先Ｚバッファのセット
extern	int		Graphics_Hardware_D3D11_SetDrawZBuffer_PF( int /*DrawScreen*/, IMAGEDATA * /*Image*/ )
{
	// 未実装
	return 0 ;
}

// 使用中のID3D11Deviceオブジェクトを取得する( 戻り値を ID3D11Device * にキャストしてください )
extern const void* Graphics_Hardware_D3D11_GetUseDirect3D11Device_PF( void )
{
	return D3D11Device_Get() ;
}

// 使用中のID3D11DeviceContextオブジェクトを取得する( 戻り値を ID3D11DeviceContext * にキャストしてください )
extern const void* Graphics_Hardware_D3D11_GetUseDirect3D11DeviceContext_PF( void )
{
	return D3D11DeviceContext_Get() ;
}

// 使用中のバックバッファのID3D11Texture2Dオブジェクトを取得する( 戻り値を ID3D11Texture2D * にキャストしてください )
extern const void* Graphics_Hardware_D3D11_GetUseDirect3D11BackBufferTexture2D_PF( void )
{
	// サブバックバッファが有効な場合はサブバックバッファの情報を返す
	if( GD3D11.Device.Screen.SubBackBufferTexture2D != NULL )
	{
		return GD3D11.Device.Screen.SubBackBufferTexture2D ;
	}

	// それ以外の場合はバックバッファの情報を返す
	return GD3D11.Device.Screen.OutputWindowInfo[ 0 ].BufferTexture2D ;
}

// 使用中のバックバッファのID3D11RenderTargetViewオブジェクトを取得する( 戻り値を ID3D11RenderTargetView * にキャストしてください )
extern const void* Graphics_Hardware_D3D11_GetUseDirect3D11BackBufferRenderTargetView_PF( void )
{
	// サブバックバッファが有効な場合はサブバックバッファの情報を返す
	if( GD3D11.Device.Screen.SubBackBufferRTV != NULL )
	{
		return GD3D11.Device.Screen.SubBackBufferRTV ;
	}

	// それ以外の場合はバックバッファの情報を返す
	return GD3D11.Device.Screen.OutputWindowInfo[ 0 ].BufferRTV ;
}

// 使用中の深度ステンシルバッファのID3D11Texture2Dオブジェクトを取得する( 戻り値を ID3D11Texture2D * にキャストしてください )
extern const void* Graphics_Hardware_D3D11_GetUseDirect3D11DepthStencilTexture2D_PF( void )
{
	return GD3D11.Device.Screen.DepthBufferTexture2D ;
}

// グラフィックハンドルが持つ ID3D11Texture2D を取得する( Direct3D11 を使用している場合のみ有効 )( 戻り値を ID3D11Texture2D * にキャストしてください )
extern const void* Graphics_Hardware_D3D11_GetGraphID3D11Texture2D_PF( IMAGEDATA *Image )
{
	return Image->Hard.Draw[ 0 ].Tex->PF->D3D11.Texture ;
}

// グラフィックハンドルが持つ ID3D11RenderTargetView を取得する( Direct3D11 を使用していて、且つ MakeScreen で作成したグラフィックハンドルでのみ有効 )( 戻り値を ID3D11RenderTargetView * にキャストしてください )
extern const void* Graphics_Hardware_D3D11_GetGraphID3D11RenderTargetView_PF( IMAGEDATA *Image )
{
	return Image->Hard.Draw[ 0 ].Tex->PF->D3D11.TextureRTV[ 0 ] ;
}

// グラフィックハンドルが持つ ID3D11DepthStencilView を取得する( Direct3D11 を使用していて、且つ MakeScreen で作成したグラフィックハンドルでのみ有効 )( 戻り値を ID3D11DepthStencilView * にキャストしてください )
extern const void* Graphics_Hardware_D3D11_GetGraphID3D11DepthStencilView_PF( IMAGEDATA *Image )
{
	return Image->Hard.Draw[ 0 ].Tex->PF->D3D11.DepthBufferDSV[ 0 ] ;
}

// 指定の ID3D11RenderTargetView を描画対象にする
extern int Graphics_Hardware_D3D11_SetDrawScreen_ID3D11RenderTargetView_PF( const void *pID3D11RenderTargetView, const void *pID3D11DepthStencilView )
{
	// 処理内容は NS_SetDrawScreen をコピー＆ペーストして改造したもの

	int OldScreen ;
	int OldScreenSurface ;
	int OldScreenMipLevel ;
#ifndef DX_NON_MASK
	int MaskUseFlag ;
#endif
//	int Result ;
	IMAGEDATA     *OldImage     = NULL ;
	SHADOWMAPDATA *OldShadowMap = NULL ;
	D_ID3D11Texture2D *pID3D11Texture2D = NULL ;
	D_D3D11_TEXTURE2D_DESC TexDesc ;
	D_ID3D11Texture2D *pID3D11DepthBuffer = NULL ;
	D_D3D11_TEXTURE2D_DESC DepthBufferDesc ;
	
	if( GSYS.InitializeFlag == FALSE ) return -1 ;

	// ソフトが非アクティブの場合はアクティブになるまで待つ
	CheckActiveState() ;

	// 今までの描画先の画像情報の取得
	if( GRAPHCHKFULL( GSYS.DrawSetting.TargetScreen[ 0 ], OldImage ) )
	{
		OldImage = NULL ;
		if( SHADOWMAPCHKFULL( GSYS.DrawSetting.TargetScreen[ 0 ], OldShadowMap ) )
		{
			OldShadowMap = NULL ;
		}
	}

	// 描画先のサイズを取得
	D3D11View_GetResource( ( D_ID3D11RenderTargetView * )pID3D11RenderTargetView, ( D_ID3D11Resource ** )&pID3D11Texture2D ) ;
	D3D11Texture2D_GetDesc( pID3D11Texture2D, &TexDesc ) ;
	if( pID3D11DepthStencilView != NULL )
	{
		D3D11View_GetResource( ( D_ID3D11DepthStencilView * )pID3D11DepthStencilView, ( D_ID3D11Resource ** )&pID3D11DepthBuffer ) ;
		D3D11Texture2D_GetDesc( pID3D11DepthBuffer, &DepthBufferDesc ) ;

		// 深度バッファと描画先のバッファのサイズが異なる場合はエラー
		if( DepthBufferDesc.Width != TexDesc.Width ||
			DepthBufferDesc.Height != TexDesc.Height )
		{
			return -1 ;
		}
	}

	// 今までの画面が表画面だった場合は一度 ScreenCopy を行う
	if( GSYS.DrawSetting.TargetScreen[ 0 ] == DX_SCREEN_FRONT )
	{
		Graphics_Screen_ScreenCopyBase( TRUE ) ;
	}

	// シャドウマップへの描画設定がされていたら解除
	if( GSYS.DrawSetting.ShadowMapDraw )
	{
		NS_ShadowMap_DrawEnd() ;
	}

#ifndef DX_NON_MASK
	// マスクを使用終了する手続きを取る
	MaskUseFlag = MASKD.MaskUseFlag ;
	NS_SetUseMaskScreenFlag( FALSE ) ;

	// 描画先のサイズでマスク画面を作成しなおす
	if( MASKD.CreateMaskFlag )
	{
		Mask_CreateScreenFunction( TRUE, ( int )TexDesc.Width, ( int )TexDesc.Height ) ;
	}
#endif

	// 描画先を保存
	OldScreen                                 = GSYS.DrawSetting.TargetScreen[ 0 ] ;
	OldScreenSurface                          = GSYS.DrawSetting.TargetScreenSurface[ 0 ] ;
	OldScreenMipLevel                         = GSYS.DrawSetting.TargetScreenMipLevel[ 0 ] ;
	GSYS.DrawSetting.TargetScreen[ 0 ]        = DX_SCREEN_OTHER ;
	GSYS.DrawSetting.TargetScreenSurface[ 0 ] = 0 ;

	// 描画可能サイズの取得
	GSYS.DrawSetting.DrawSizeX = ( int )TexDesc.Width ;
	GSYS.DrawSetting.DrawSizeY = ( int )TexDesc.Height ;

	// 描画先に正しいα値を書き込むかどうかのフラグを更新する
	Graphics_DrawSetting_RefreshAlphaChDrawMode() ;

	// ３Ｄ描画関係の描画先をセット
	if( GSYS.Setting.ValidHardware )
	{
		// ハードウエアアクセラレータを使用する場合の処理
		Graphics_Hardware_D3D11_SetDrawScreen_PF_Base( DX_SCREEN_OTHER, OldScreenSurface, OldScreenMipLevel, NULL, OldImage, NULL, OldShadowMap, ( D_ID3D11RenderTargetView * )pID3D11RenderTargetView, ( D_ID3D11DepthStencilView * )pID3D11DepthStencilView ) ;
	}

	// 描画領域を更新
	{
		RECT SrcRect ;
		int NewWidth, NewHeight, OldWidth = 10, OldHeight = 10 ;

		NewWidth  = ( int )TexDesc.Width ;
		NewHeight = ( int )TexDesc.Height ;
		NS_GetGraphSize( OldScreen,  &OldWidth, &OldHeight ) ;

		SrcRect = GSYS.DrawSetting.DrawArea ;

		if( GSYS.DrawSetting.SetDrawScreenNoSettingResetFlag )
		{
			if( GSYS.DrawSetting.ShadowMapDrawSetupRequest )
			{
				RECT OriginalDrawRect = GSYS.DrawSetting.OriginalDrawRect ;

				NS_SetDrawArea( 0, 0, NewWidth, NewHeight ) ;

				GSYS.DrawSetting.OriginalDrawRect = OriginalDrawRect ;
			}
			else
			{
				NS_SetDrawArea(
					GSYS.DrawSetting.OriginalDrawRect.left,
					GSYS.DrawSetting.OriginalDrawRect.top,
					GSYS.DrawSetting.OriginalDrawRect.right,
					GSYS.DrawSetting.OriginalDrawRect.bottom
				) ;
			}
		}
		else
		{
//			if( GSYS.DrawSetting.ShadowMapDrawSetupRequest || ( NewWidth != OldWidth ) || ( NewHeight != OldHeight ) )
//			{
				NS_SetDrawArea( 0, 0, NewWidth, NewHeight ) ;
//			}
//			else
//			{
//				NS_SetDrawArea( SrcRect.left, SrcRect.top, SrcRect.right, SrcRect.bottom ) ;
//			}
		}
	}
		
	// ３Ｄ描画処理用の行列設定の初期化を行う
	if( GSYS.DrawSetting.SetDrawScreenNoSettingResetFlag == FALSE )
	{
		MATRIX mat ;
		float D ;

		// ドットアスペクトを１．０に戻す
		NS_SetCameraDotAspect( 1.0f ) ;

		// ワールド変換行列は単位行列
		CreateIdentityMatrix( &mat ) ;
		NS_SetTransformToWorld( &mat ) ;

		// ライブラリ管理のビューポート行列を更新
		GSYS.Camera.ScreenCenterX = ( double )GSYS.DrawSetting.DrawSizeX / 2.0 ;
		GSYS.Camera.ScreenCenterY = ( double )GSYS.DrawSetting.DrawSizeY / 2.0 ;
		NS_SetCameraScreenCenterD( GSYS.Camera.ScreenCenterX, GSYS.Camera.ScreenCenterY ) ;

		// ビュー行列は z = 0.0 の時に丁度スクリーン全体が写る位置と方向を持つカメラを
		{
			VECTOR up, at, eye ;

			D = (float)( ( GSYS.DrawSetting.DrawSizeY / 2 ) / DEFAULT_TAN_FOV_HALF ) ;

			eye.x = (float)GSYS.DrawSetting.DrawSizeX / 2 ;
			eye.y = (float)GSYS.DrawSetting.DrawSizeY / 2 ;
			eye.z = -D;
			
			at.x = eye.x ;
			at.y = eye.y ;
			at.z = eye.z + 1.0F ;
			
			up.x = 0.0f ;
			up.y = 1.0F ;
			up.z = 0.0f ;
			
			NS_SetCameraPositionAndTargetAndUpVec( eye, at, up ) ;
		}

		// プロジェクション行列は普通に
		NS_SetupCamera_Perspective( DEFAULT_FOV ) ;
		NS_SetCameraNearFar( D * 0.1f + DEFAULT_NEAR, D + DEFAULT_FAR ) ;
	}

#ifndef DX_NON_MASK
	// マスクを使用する手続きを取る
	NS_SetUseMaskScreenFlag( MaskUseFlag ) ;
#endif

	// 終了
	return 0 ;
}






















// 環境依存情報取得関係

// GetColor や GetColor2 で使用するカラーデータを取得する
extern const COLORDATA *Graphics_Hardware_D3D11_GetMainColorData_PF( void )
{
	return Graphics_D3D11_GetD3DFormatColorData( DIRECT3D11_BACKBUFFER_FORMAT ) ;
}

// ディスプレーのカラーデータポインタを得る
extern	const COLORDATA *Graphics_Hardware_D3D11_GetDispColorData_PF( void )
{
	return Graphics_D3D11_GetD3DFormatColorData( GD3D11.Device.Caps.ScreenFormat ) ;
}

// 指定座標の色を取得する
extern	DWORD Graphics_Hardware_D3D11_GetPixel_PF( int x, int y )
{
	RECT            SrcRect ;
//	const COLORDATA *DestColorData ;
	BASEIMAGE       BufferImage ;
	DWORD           Ret = 0xffffffff ;
	int	Red = 0, Green = 0, Blue = 0, Alpha = 0 ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

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
extern COLOR_F Graphics_Hardware_D3D11_GetPixelF_PF( int x, int y )
{
	RECT            SrcRect ;
//	const COLORDATA *DestColorData ;
	BASEIMAGE       BufferImage ;
	COLOR_F			Result = { 0.0f } ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

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
extern	int		Graphics_Hardware_D3D11_UpdateGraphMovie_TheoraYUV_PF( MOVIEGRAPH * /*Movie*/, IMAGEDATA * /*Image*/ )
{
	// 未実装
	return 0 ;
}

// グラフィックメモリ領域のロック
extern	int		Graphics_Hardware_D3D11_GraphLock_PF( IMAGEDATA *Image, COLORDATA **ColorDataP, int WriteOnly )
{
	COLORDATA           *ColorData ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// カラーフォーマットを取得する
	ColorData = Graphics_D3D11_GetD3DFormatColorData( GD3D11.Device.Caps.TextureFormat[ Image->Orig->ColorFormat ] ) ;

	// 標準フォーマット以外ではロックできない
	if( ColorData->Format != DX_BASEIMAGE_FORMAT_NORMAL )
	{
		return -1 ;
	}

	// テンポラリバッファを確保する
	Image->LockImagePitch	= ( DWORD )( ColorData->PixelByte * Image->WidthI ) ;
	Image->LockImage		= ( BYTE * )DXALLOC( Image->LockImagePitch * Image->HeightI ) ;
	if( Image->LockImage == NULL )
	{
		DXST_LOGFILE_ADDUTF16LE( "\xed\x30\xc3\x30\xaf\x30\x28\x75\xc6\x30\xf3\x30\xdd\x30\xe9\x30\xea\x30\xa4\x30\xe1\x30\xfc\x30\xb8\x30\x3c\x68\x0d\x7d\x28\x75\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ロック用テンポラリイメージ格納用メモリの確保に失敗しました\n" @*/ ) ;
		goto ERR ;
	}

	// 書き込み専用ではない場合はテンポラリバッファにデータを転送する
	if( WriteOnly == FALSE )
	{
		int                  i ;
		BYTE                *Dest ;
		BYTE                *Src ;
		DWORD                WidthByte ;
		BASEIMAGE            MapBaseImage ;
		D_ID3D11Texture2D   *MapTempTexture ;
		IMAGEDATA_HARD_DRAW *DrawTex ;

		// 描画情報の数だけ繰り返し
		DrawTex = Image->Hard.Draw ;
		for( i = 0 ; i < Image->Hard.DrawNum ; i ++, DrawTex ++ )
		{
			// マップ
			if( Graphics_D3D11_Texture2D_Map(
				DrawTex->Tex->PF->D3D11.Texture,
				DrawTex->Tex->PF->D3D11.TextureSRV,
				0,
				0,
				NULL,
				&MapBaseImage,
				&MapTempTexture ) < 0 )
			{
				goto ERR ;
			}

			// 転送
			Src  = ( BYTE * )MapBaseImage.GraphData + DrawTex->UsePosXI  * ColorData->PixelByte + DrawTex->UsePosYI  * MapBaseImage.Pitch ;
			Dest = Image->LockImage                 + DrawTex->DrawPosXI * ColorData->PixelByte + DrawTex->DrawPosYI * Image->LockImagePitch ;
			WidthByte = ( DWORD )( DrawTex->WidthI * ColorData->PixelByte ) ;
			for( i = 0 ; i < DrawTex->HeightI ; i ++, Dest += Image->LockImagePitch, Src += MapBaseImage.Pitch )
			{
				_MEMCPY( Dest, Src, WidthByte ) ;
			}

			// マップの解除
			Graphics_D3D11_Texture2D_Unmap( MapTempTexture ) ;
		}
	}

	// カラーデータのアドレスを保存
	*ColorDataP = ColorData ;

	// 正常終了
	return 0 ;

ERR :

	if( Image->LockImage != NULL )
	{
		DXFREE( Image->LockImage ) ;
		Image->LockImage = NULL ;
	}

	// エラー終了
	return -1 ;
}

// グラフィックメモリ領域のロック解除
extern	int		Graphics_Hardware_D3D11_GraphUnlock_PF( IMAGEDATA *Image )
{
	COLORDATA           *ColorData ;
	BASEIMAGE            BaseImage ;
	BASEIMAGE            AlphaBaseImage = { 0 } ;
	RECT                 Rect ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// カラーフォーマットを取得する
	ColorData = Graphics_D3D11_GetD3DFormatColorData( GD3D11.Device.Caps.TextureFormat[ Image->Orig->ColorFormat ] ) ;

	// 基本イメージを構築する
	BaseImage.ColorData      = *ColorData ;
	BaseImage.Width          = Image->WidthI ;
	BaseImage.Height         = Image->HeightI ;
	BaseImage.Pitch          = ColorData->PixelByte * Image->WidthI ;
	BaseImage.GraphData      = Image->LockImage ;
	BaseImage.MipMapCount    = 0 ;
	BaseImage.GraphDataCount = 0 ;

	// 転送
	Rect.left   = 0 ;
	Rect.top    = 0 ;
	Rect.right  = Image->WidthI ;
	Rect.bottom = Image->HeightI ;
	Graphics_Hardware_D3D11_BltBmpOrBaseImageToGraph3_PF( &Rect, 0, 0, Image->HandleInfo.Handle, &BaseImage, &AlphaBaseImage, FALSE, FALSE, TRUE, FALSE ) ;

	// ロックイメージの解放
	if( Image->LockImage )
	{
		DXFREE( Image->LockImage ) ;
		Image->LockImage = NULL ;
	}

	// 正常終了
	return 0 ;
}

// グラフィックのＺバッファの状態を別のグラフィックのＺバッファにコピーする( DestGrHandle も SrcGrHandle もＺバッファを持っている描画可能画像で、且つアンチエイリアス画像ではないことが条件 )
extern	int		Graphics_Hardware_D3D11_CopyGraphZBufferImage_PF( IMAGEDATA * /*DestImage*/, IMAGEDATA * /*SrcImage*/ )
{
	// 未実装
	return 0 ;
}

// 画像データの初期化
extern	int		Graphics_Hardware_D3D11_InitGraph_PF( void )
{
	// 正常終了
	return 0 ;
}

// グラフィックを特定の色で塗りつぶす
extern	int		Graphics_Hardware_D3D11_FillGraph_PF( IMAGEDATA *Image, int Red, int Green, int Blue, int Alpha, int /*ASyncThread*/ )
{
	// デバイスが無かったらエラー
	if( GAPIWin.D3D11DeviceObject == NULL ) return -1 ;

	// 描画情報の数だけ繰り返し
	if( Image->Orig->FormatDesc.DrawValidFlag )
	{
		IMAGEDATA_HARD_DRAW *DrawTex ;
		FLOAT               ColorRGBA[ 4 ] ;
		int                 i ;

		// 描画待機している描画物を描画
		DRAWSTOCKINFO

		// 描画の終了
//		Graphics_D3D9_EndScene() ;

		ColorRGBA[ 0 ] = Red   / 255.0f ;
		ColorRGBA[ 1 ] = Green / 255.0f ;
		ColorRGBA[ 2 ] = Blue  / 255.0f ;
		ColorRGBA[ 3 ] = Alpha / 255.0f ;

		DrawTex = Image->Hard.Draw ;
		for( i = 0 ; i < Image->Hard.DrawNum ; i ++, DrawTex ++ )
		{
			// 使用域を指定色で塗りつぶし
			D3D11DeviceContext_ClearRenderTargetView( DrawTex->Tex->PF->D3D11.TextureRTV[ 0 ], ColorRGBA ) ;
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
extern	int		Graphics_Hardware_D3D11_GetDrawScreenGraphBase_PF( IMAGEDATA *Image, IMAGEDATA *TargetImage, int TargetScreen, int TargetScreenSurface, int TargetScreenMipLevel, int /*TargetScreenWidth*/, int /*TargetScreenHeight*/, int x1, int y1, int x2, int y2, int destX, int destY )
{
	RECT SrcRect ;
	RECT DestRect ;
	RECT Rect ;
	int Width, Height ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 幅と高さを計算
	Width  = x2 - x1 ;
	Height = y2 - y1 ;

	// 転送先となるサーフェスが描画可能テクスチャだった場合は単純に転送
	if( Image->Orig->FormatDesc.DrawValidFlag )
	{
		D_ID3D11Texture2D          *TargetTexture ;
		D_ID3D11ShaderResourceView *TargetTextureSRV ;

		// 取り込み元となるテクスチャの決定

		// 描画可能画像が対象の場合
		if( TargetImage )
		{
			TargetTexture    = TargetImage->Orig->Hard.Tex[ 0 ].PF->D3D11.Texture ;
			TargetTextureSRV = TargetImage->Orig->Hard.Tex[ 0 ].PF->D3D11.TextureSRV ;
		}
		else
		// サブバックバッファが有効になっている場合はサブバックバッファを初期化
		if( GD3D11.Device.Screen.SubBackBufferTexture2D != NULL )
		{
			TargetTexture    = GD3D11.Device.Screen.SubBackBufferTexture2D ;
			TargetTextureSRV = GD3D11.Device.Screen.SubBackBufferSRV ;
		}
		else
		{
			TargetTexture    = GD3D11.Device.Screen.OutputWindowInfo[ 0 ].BufferTexture2D ;
			TargetTextureSRV = GD3D11.Device.Screen.OutputWindowInfo[ 0 ].BufferSRV ;
		}

		SrcRect.left    = x1 ;
		SrcRect.top     = y1 ;
		SrcRect.right   = x2 ;
		SrcRect.bottom  = y2 ;

		DestRect.left   = destX ;
		DestRect.top    = destY ;
		DestRect.right  = destX + Width ;
		DestRect.bottom = destY + Height ;

		if( TargetTexture    != NULL &&
			TargetTextureSRV != NULL &&
			Image->Orig->Hard.Tex[ 0 ].PF->D3D11.Texture         != NULL &&
			Image->Orig->Hard.Tex[ 0 ].PF->D3D11.TextureRTV[ 0 ] != NULL )
		{
			Graphics_D3D11_StretchRect(
				TargetTexture,                                TargetTextureSRV,                                     &SrcRect,
				Image->Orig->Hard.Tex[ 0 ].PF->D3D11.Texture, Image->Orig->Hard.Tex[ 0 ].PF->D3D11.TextureRTV[ 0 ], &DestRect
			) ; 
		}
	}
	else
	// 通常のテクスチャだった場合は最初にロックを試す
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

static int Graphics_D3D11_BltBmpOrBaseImageToGraph3_MipMapBlt(
	      IMAGEDATA_ORIG	*Orig,
	const RECT				*SrcRect,
	const RECT				*DestRect,
	      D_ID3D11Texture2D	*UseTex,
	      int				TexWidth,
	      int				TexHeight,
	const BASEIMAGE			*RgbBaseImage,
	const BASEIMAGE			*AlphaBaseImage,
	const COLORDATA			*DestColor,
	      int				RedIsAlphaFlag,
	      int				UseTransColorConvAlpha,
	      int				ASyncThread
)
{
	int			ImageSize ;
	int			ImageNum ;
	void		*ImageBuffer ;
	int			TempTexWidth ;
	int			TempTexHeight ;
	int			i ;
	int			j ;
	D_D3D11_BOX	DestBox ;
	int			IsDXTFormat ;
	DWORD		SrcScale ;

	IsDXTFormat =
		( RgbBaseImage->ColorData.Format == DX_BASEIMAGE_FORMAT_DXT1 ) ||
		( RgbBaseImage->ColorData.Format == DX_BASEIMAGE_FORMAT_DXT3 ) ||
		( RgbBaseImage->ColorData.Format == DX_BASEIMAGE_FORMAT_DXT5 ) ;

	ImageNum = Orig->FormatDesc.CubeMapTextureFlag ? CUBEMAP_SURFACE_NUM : 1 ;

	// 転送先がテクスチャ全体で、且つ転送元にミップマップ情報がある場合は分岐
	if( DestRect->left == 0 && DestRect->right  == TexWidth  &&
		DestRect->top  == 0 && DestRect->bottom == TexHeight &&
		RgbBaseImage->MipMapCount >= Orig->Hard.MipMapCount )
	{
		ImageBuffer = RgbBaseImage->GraphData ;
		if( IsDXTFormat )
		{
			SrcScale = ( DWORD )( RgbBaseImage->ColorData.ColorBitDepth * 4 / 8 ) ;
		}
		else
		{
			SrcScale = ( DWORD )( RgbBaseImage->ColorData.PixelByte ) ;
		}
		for( i = 0 ; i < ImageNum ; i ++ )
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

				// テクスチャに転送
				DestBox.left   = 0 ;
				DestBox.top    = 0 ;
				DestBox.front  = 0 ;
				if( IsDXTFormat )
				{
					DestBox.right  = ( UINT )( ( TempTexWidth + 3 ) / 4 * 4 ) ;
					DestBox.bottom = ( UINT )( ( TempTexHeight + 3 ) / 4 * 4 ) ;
				}
				else
				{
					DestBox.right  = ( UINT )TempTexWidth ;
					DestBox.bottom = ( UINT )TempTexHeight ;
				}
				DestBox.back   = 1 ;
				D3D11DeviceContext_UpdateSubresource_ASync(
					UseTex,
					D_D3D11CalcSubresource( ( UINT )j, ( UINT )i, Orig->Hard.MipMapCount ),
					&DestBox,
					ImageBuffer,
					IsDXTFormat ? ( TempTexWidth + 3 ) / 4 * 4 * SrcScale : TempTexWidth * SrcScale,
					0,
					ASyncThread
				) ;

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
		if( IsDXTFormat  )
		{
			DXST_LOGFILE_ADDUTF16LE( "\x44\x00\x58\x00\x54\x00\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x6e\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6b\x30\x19\x6a\x96\x6e\x62\x5f\x0f\x5f\x6e\x30\xa4\x30\xe1\x30\xfc\x30\xb8\x30\x92\x30\xe2\x8e\x01\x90\x59\x30\x8b\x30\x53\x30\x68\x30\x6f\x30\x67\x30\x4d\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"DXTフォーマットのテクスチャに標準形式のイメージを転送することはできません\n" @*/ ) ;
		}
		else
		{
			int		ImageW ;
			int		ImageH ;
			int		ImageDW ;
			int		ImageDH ;
			int		ImageSW ;
			int		ImageSH ;
			void	*Image1 ;
			void	*Image2 ;
			void	*ImageD ;
			void	*ImageS ;
			int		ImagePitch ;
			int		ts ;
			POINT	DestPoint ;
			int		RgbImageSize ;
			int		AlphaImageSize ;

			DestPoint.x = 0 ;
			DestPoint.y = 0 ;

			// 一時バッファに格納
			ts = DestRect->right  - DestRect->left ;
			for( ImageW = 1 ; ImageW < ts ; ImageW <<= 1 ){}
			ts = DestRect->bottom - DestRect->top  ;
			for( ImageH = 1 ; ImageH < ts ; ImageH <<= 1 ){}
			ImagePitch  = DestColor->PixelByte * ImageW ;
			ImageSize   = ImagePitch * ImageH ;

			if( ASyncThread )
			{
				ImageBuffer = DXALLOC( ( size_t )( ImageSize * 2 ) ) ;
				if( ImageBuffer == NULL )
				{
					DXST_LOGFILE_ADDUTF16LE( "\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x78\x30\xe2\x8e\x01\x90\x59\x30\x8b\x30\x3b\x75\xcf\x50\x92\x30\x00\x4e\x42\x66\x84\x76\x6b\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"テクスチャへ転送する画像を一時的に格納するメモリ領域の確保に失敗しました\n" @*/ ) ;
					return -1 ;
				}
			}
			else
			{
				if( Graphics_D3D11_Texture_SetupCommonBuffer( ( unsigned int )( ImageSize * 2 ) ) < 0 )
				{
					return -1 ;
				}
				ImageBuffer = GD3D11.Texture.CommonBuffer ;
			}
			Image1      = ImageBuffer ;
			Image2      = ( BYTE * )Image1 + ImageSize ;

			RgbImageSize   = RgbBaseImage->Pitch   * RgbBaseImage->Height ;
			AlphaImageSize = AlphaBaseImage->Pitch * AlphaBaseImage->Height ;
			for( i = 0 ; i < ImageNum ; i ++ )
			{
				// ミップマップの深さだけ繰り返し
				ImageSW = ImageW ;
				ImageSH = ImageH ;
				ImageDW = ImageW ;
				ImageDH = ImageH ;
				for( j = 0 ; j < Orig->Hard.MipMapCount ; j ++ )
				{
					// 縮小画像の作成
					if( j == 0 )
					{
						// level0 の時は等倍の出力フォーマット画像を作成する
						NS_GraphColorMatchBltVer2(
							Image1,                                                   ImagePitch,            DestColor,
							( BYTE * )RgbBaseImage->GraphData   + RgbImageSize   * i, RgbBaseImage->Pitch,   &RgbBaseImage->ColorData,
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

					// テクスチャに転送
					DestBox.left   = ( DWORD )DestRect->left   >> j ;
					DestBox.top    = ( DWORD )DestRect->top    >> j ;
					DestBox.front  = 0 ;
					DestBox.right  = ( DWORD )DestRect->right  >> j ;
					DestBox.bottom = ( DWORD )DestRect->bottom >> j ;
					DestBox.back   = 1 ;
					if( DestBox.right == DestBox.left )
					{
						DestBox.right = DestBox.left + 1 ;
					}
					if( DestBox.bottom == DestBox.top )
					{
						DestBox.bottom = DestBox.top + 1 ;
					}

					D3D11DeviceContext_UpdateSubresource_ASync( UseTex, D_D3D11CalcSubresource( ( UINT )j, ( UINT )i, 1 ), &DestBox, ImageD, ( DWORD )ImagePitch, 0, ASyncThread ) ;

					// 次のイメージのサイズをセット
					ImageSW = ImageDW ;
					ImageSH = ImageDH ;
					ImageDW >>= 1 ;
					ImageDH >>= 1 ;
					if( ImageDW == 0 ) ImageDW = 1 ;
					if( ImageDH == 0 ) ImageDH = 1 ;
				}
			}

			// メモリの解放
			if( ASyncThread )
			{
				DXFREE( ImageBuffer ) ;
			}
		}
	}

	// 正常終了
	return 0 ;
}

static int Graphics_D3D11_BltBmpOrBaseImageToGraph3_NoMipMapBlt(
	      IMAGEDATA_ORIG	*Orig,
	const RECT				*SrcRect,
	const RECT				*DestRect,
	      D_ID3D11Texture2D	*UseTex,
	      int				TexWidth,
	      int				TexHeight,
	      int				TexUseWidth,
	      int				TexUseHeight,
	const BASEIMAGE			*RgbBaseImage,
	const BASEIMAGE			*AlphaBaseImage,
	const COLORDATA			*DestColor,
	      int				RedIsAlphaFlag,
	      int				UseTransColorConvAlpha,
	      int				ASyncThread
)
{
	int				i ;
	int				IsDXTFormat ;
	int				k ;
	POINT			DestPoint ;
	int				H ;
	BYTE			*DestAddr ;
	BYTE			*SrcRgbAddr ;
	BYTE			*SrcAlphaAddr ;
	int				RightLineFillFlag ;
	int				BottomLineFillFlag ;
	int				ImageNum ;
	DWORD			ImageSize ;
	DWORD			AlphaImageSize ;
	void			*TempBuffer = NULL ;
	unsigned int	TempBufferSize ;
	int				TempBufferPitch = 0 ;
	D_D3D11_BOX		DestBox ;

	IsDXTFormat =
		( RgbBaseImage->ColorData.Format == DX_BASEIMAGE_FORMAT_DXT1 ) ||
		( RgbBaseImage->ColorData.Format == DX_BASEIMAGE_FORMAT_DXT3 ) ||
		( RgbBaseImage->ColorData.Format == DX_BASEIMAGE_FORMAT_DXT5 ) ;

	DestPoint.x = 0 ;
	DestPoint.y = 0 ;

	RightLineFillFlag  = FALSE ;
	BottomLineFillFlag = FALSE ;

	if( IsDXTFormat == FALSE )
	{
		if( TexUseWidth  < TexWidth  && TexUseWidth  == DestRect->right )
		{
			RightLineFillFlag = TRUE ;
		}
		if( TexUseHeight < TexHeight && TexUseHeight == DestRect->bottom )
		{
			BottomLineFillFlag = TRUE ;
		}

		TempBufferPitch =                   ( TexUseWidth  + ( RightLineFillFlag  ? 1 : 0 ) ) * DestColor->PixelByte ;
		TempBufferSize  = ( unsigned int )( ( TexUseHeight + ( BottomLineFillFlag ? 1 : 0 ) ) * TempBufferPitch ) ;

		if( ASyncThread )
		{
			TempBuffer      = DXALLOC( TempBufferSize ) ;
			if( TempBuffer == NULL )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6b\x30\x3b\x75\xcf\x50\x92\x30\xe2\x8e\x01\x90\x59\x30\x8b\x30\x5f\x30\x81\x30\x6e\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x5b\x00\x47\x00\x72\x00\x61\x00\x70\x00\x68\x00\x69\x00\x63\x00\x73\x00\x5f\x00\x44\x00\x33\x00\x44\x00\x31\x00\x31\x00\x5f\x00\x42\x00\x6c\x00\x74\x00\x42\x00\x6d\x00\x70\x00\x4f\x00\x72\x00\x42\x00\x61\x00\x73\x00\x65\x00\x49\x00\x6d\x00\x61\x00\x67\x00\x65\x00\x54\x00\x6f\x00\x47\x00\x72\x00\x61\x00\x70\x00\x68\x00\x33\x00\x5f\x00\x4e\x00\x6f\x00\x4d\x00\x69\x00\x70\x00\x4d\x00\x61\x00\x70\x00\x42\x00\x6c\x00\x74\x00\x5d\x00\x0a\x00\x00"/*@ L"テクスチャに画像を転送するためのメモリの確保に失敗しました[Graphics_D3D11_BltBmpOrBaseImageToGraph3_NoMipMapBlt]\n" @*/ )) ;
				return -1 ;
			}
		}
		else
		{
			if( Graphics_D3D11_Texture_SetupCommonBuffer( TempBufferSize ) < 0 )
			{
				return -1 ;
			}
			TempBuffer      = GD3D11.Texture.CommonBuffer ;
		}
	}

	ImageNum = Orig->FormatDesc.CubeMapTextureFlag ? CUBEMAP_SURFACE_NUM : 1 ;

	for( i = 0 ; i < ImageNum ; i ++ )
	{
		// 転送処理

		// 転送先がテクスチャ全体で、且つDXTフォーマットの場合は単純なメモリ転送を行う
		if( DestRect->left == 0 && DestRect->right  == TexWidth  &&
			DestRect->top  == 0 && DestRect->bottom == TexHeight && IsDXTFormat )
		{
			ImageSize = ( unsigned int )( RgbBaseImage->Width * RgbBaseImage->Height * RgbBaseImage->ColorData.ColorBitDepth / 8 ) ;

			DestBox.left   = 0 ;
			DestBox.top    = 0 ;
			DestBox.front  = 0 ;
			DestBox.right  = ( UINT )TexWidth ;
			DestBox.bottom = ( UINT )TexHeight ;
			DestBox.back   = 1 ;

			D3D11DeviceContext_UpdateSubresource_ASync(
				UseTex,
				D_D3D11CalcSubresource( 0, ( UINT )i, 1 ),
				&DestBox,
				( BYTE * )RgbBaseImage->GraphData + ImageSize * i,
				( UINT )( TexWidth * ( RgbBaseImage->ColorData.ColorBitDepth * 4 / 8 ) ),
				0,
				ASyncThread
			) ;
		}
		else
		{
			// 転送先が標準フォーマットではない場合は転送できない
			if( IsDXTFormat == FALSE )
			{
				ImageSize      = ( DWORD )( RgbBaseImage->Pitch   * RgbBaseImage->Height   ) ;
				AlphaImageSize = ( DWORD )( AlphaBaseImage->Pitch * AlphaBaseImage->Height ) ;

				SrcRgbAddr   = ( BYTE * )RgbBaseImage->GraphData   + i * ImageSize ;
				SrcAlphaAddr = ( BYTE * )AlphaBaseImage->GraphData + i * AlphaImageSize ;
				NS_GraphColorMatchBltVer2(
					TempBuffer,     TempBufferPitch,       DestColor,
					SrcRgbAddr,     RgbBaseImage->Pitch,   &RgbBaseImage->ColorData,
					SrcAlphaAddr,   AlphaBaseImage->Pitch, &AlphaBaseImage->ColorData,
					DestPoint, SrcRect, FALSE,
					UseTransColorConvAlpha && ( DestColor->AlphaWidth != 0 && AlphaBaseImage->GraphData == NULL ), Orig->TransCode,
					BASEIM.ImageShavedMode, FALSE,
					RedIsAlphaFlag, FALSE,
					FALSE ) ;

				if( BottomLineFillFlag )
				{
					_MEMCPY( ( BYTE * )TempBuffer + ( TexUseHeight - DestRect->top     ) * TempBufferPitch,
							 ( BYTE * )TempBuffer + ( TexUseHeight - DestRect->top - 1 ) * TempBufferPitch,
							 ( size_t )( DestColor->PixelByte * ( SrcRect->right - SrcRect->left ) ) ) ;
				}
				if( RightLineFillFlag )
				{
					H = SrcRect->bottom - SrcRect->top ;
					DestAddr = ( BYTE * )TempBuffer + ( TexUseWidth - DestRect->left ) * DestColor->PixelByte ;
					switch( DestColor->PixelByte )
					{
					case 2 :
						for( k = 0 ; k < H ; k ++, DestAddr += TempBufferPitch )
						{
							*( ( WORD * )DestAddr ) = *( ( WORD * )DestAddr - 1 ) ;
						}
						break ;

					case 4 :
						for( k = 0 ; k < H ; k ++, DestAddr += TempBufferPitch )
						{
							*( ( DWORD * )DestAddr ) = *( ( DWORD * )DestAddr - 1 ) ;
						}
						break ;
					}
				}
				if( BottomLineFillFlag && RightLineFillFlag )
				{
					DestAddr = ( BYTE * )TempBuffer + ( TexUseHeight - DestRect->top ) * TempBufferPitch + ( TexUseWidth - DestRect->left ) * DestColor->PixelByte ;
					switch( DestColor->PixelByte )
					{
					case 2 : *( ( WORD  * )DestAddr ) = *( ( WORD  * )( DestAddr - TempBufferPitch ) - 1 ) ; break ;
					case 4 : *( ( DWORD * )DestAddr ) = *( ( DWORD * )( DestAddr - TempBufferPitch ) - 1 ) ;	break ;
					}
				}

				// テクスチャへ転送
				DestBox.left   = ( UINT )DestRect->left ;
				DestBox.top    = ( UINT )DestRect->top ;
				DestBox.front  = 0 ;
				DestBox.right  = ( UINT )DestRect->right ;
				DestBox.bottom = ( UINT )DestRect->bottom ;
				DestBox.back   = 1 ;

				D3D11DeviceContext_UpdateSubresource_ASync( UseTex, D_D3D11CalcSubresource( 0, ( UINT )i, 1 ), &DestBox, TempBuffer, ( UINT )TempBufferPitch, 0, ASyncThread ) ;
			}
			else
			{
				DXST_LOGFILE_ADDUTF16LE( "\x44\x00\x58\x00\x54\x00\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x6e\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6b\x30\x19\x6a\x96\x6e\x62\x5f\x0f\x5f\x6e\x30\xa4\x30\xe1\x30\xfc\x30\xb8\x30\x92\x30\xe2\x8e\x01\x90\x59\x30\x8b\x30\x53\x30\x68\x30\x6f\x30\x67\x30\x4d\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"DXTフォーマットのテクスチャに標準形式のイメージを転送することはできません\n" @*/ ) ;
			}
		}
	}

	if( IsDXTFormat == FALSE )
	{
		if( ASyncThread )
		{
			if( TempBuffer != NULL )
			{
				DXFREE( TempBuffer ) ;
				TempBuffer = NULL ;
			}
		}
	}

	// 正常終了
	return 0 ;
}

// Direct3D11 のグラフィックハンドルに画像データを転送するための関数
extern	int		Graphics_Hardware_D3D11_BltBmpOrBaseImageToGraph3_PF(
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
	int							i ;
	POINT						DestPoint ;
	RECT						DestRect ;
	RECT						MoveRect ;
	int							IsDXT ;
	COLORDATA					*DestColor ;
	D_ID3D11Texture2D			*UseTex ;
	int							SrcWidth ;
	int							SrcHeight ;

	if( GAPIWin.D3D11DeviceObject == NULL )
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
	DestColor = Graphics_D3D11_GetD3DFormatColorData( GD3D11.Device.Caps.TextureFormat[ Orig->ColorFormat ] ) ;

	// 転送位置のセット
	DestPoint.x = 0 ;
	DestPoint.y = 0 ;

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

			UseTex = OrigTex->PF->D3D11.Texture ;

			// ミップマップかどうかで処理を分岐
			if( Orig->Hard.MipMapCount > 1 )
			{
				if( Graphics_D3D11_BltBmpOrBaseImageToGraph3_MipMapBlt(
						Orig,
						&MoveRect,
						&DestRect,
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
				if( Graphics_D3D11_BltBmpOrBaseImageToGraph3_NoMipMapBlt(
						Orig,
						&MoveRect,
						&DestRect,
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

			UseTex = DrawTex->Tex->PF->D3D11.Texture ;

			// ミップマップかどうかで処理を分岐
			if( Orig->Hard.MipMapCount > 1 )
			{
				if( Graphics_D3D11_BltBmpOrBaseImageToGraph3_MipMapBlt(
						Orig,
						&MoveRect,
						&DestRect,
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
				if( Graphics_D3D11_BltBmpOrBaseImageToGraph3_NoMipMapBlt(
						Orig,
						&MoveRect,
						&DestRect,
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
		}
	}

	// 正常終了
	return 0 ;


	// エラー終了
ERR :
	return -1 ;
}

// オリジナル画像情報中のテクスチャを作成する( 0:成功  -1:失敗 )
extern	int		Graphics_Hardware_D3D11_CreateOrigTexture_PF(  IMAGEDATA_ORIG *Orig, int ASyncThread )
{
	int									i ;
	int                                 j ;
	int									k ;
	IMAGEDATA_ORIG_HARD_TEX				*OrigTex ;
	D_D3D11_TEXTURE2D_DESC				Texture2DDesc ;
	D_D3D11_RENDER_TARGET_VIEW_DESC		RTVDesc ;
	D_D3D11_SHADER_RESOURCE_VIEW_DESC	SRVDesc ;
	D_D3D11_DEPTH_STENCIL_VIEW_DESC		DepthStencilViewDesc ;
	HRESULT								hr ;
	UINT								Samples = 1 ;
	UINT								Quality = 0 ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return -1 ;
	}

	// ハードウエアを使用する画像ではない場合は何もせずに終了
	if( Orig->FormatDesc.TextureFlag == FALSE )
	{
		return 0 ;
	}

	// 指定のフォーマットが作成できない場合は失敗
	if( Orig->UserPlatformTexture == NULL && GD3D11.Device.Caps.TextureFormat[ Orig->ColorFormat ] == D_DXGI_FORMAT_UNKNOWN )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x07\x63\x9a\x5b\x6e\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x67\x30\x5c\x4f\x10\x62\x67\x30\x4d\x30\x8b\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"指定のフォーマットで作成できるテクスチャフォーマットがありませんでした\n" @*/ ) ;
		return -1 ;
	}

	// マルチサンプルフォーマットの場合は使用できるサンプル数とクオリティを取得しておく
	if( Orig->UserPlatformTexture == NULL && Orig->FormatDesc.MSSamples != 0 )
	{
		// キューブマップ又は D_D3D_FEATURE_LEVEL_10_0 未満の場合はマルチサンプルは使用できない
		if( Orig->FormatDesc.CubeMapTextureFlag || GD3D11.Setting.FeatureLevel < D_D3D_FEATURE_LEVEL_10_0 )
		{
			Samples = 1 ;
			Quality = 0 ;
		}
		else
		{
			Samples = ( UINT )Orig->FormatDesc.MSSamples ;
			Quality = ( UINT )Orig->FormatDesc.MSQuality ;
			D3D11Device_CheckMultiSampleParam_ASync( GD3D11.Device.Caps.TextureFormat[ Orig->ColorFormat ], &Samples, &Quality, FALSE, ASyncThread ) ;
		}
	}

	// テクスチャの場合はテクスチャの数だけテクスチャを作成
	OrigTex = Orig->Hard.Tex ;
	for( i = 0 ; i < Orig->Hard.TexNum ; i ++, OrigTex ++ )
	{
		// Direct3D11 に管理属性は無い
		Orig->FormatDesc.UseManagedTextureFlag = FALSE ;
		
		// ユーザー指定テクスチャがある場合とない場合で処理を分岐
		if( Orig->UserPlatformTexture != NULL )
		{
			OrigTex->PF->D3D11.Texture = ( D_ID3D11Texture2D * )Orig->UserPlatformTexture ;
		}
		else
		{
			// マルチサンプルフォーマットの場合はマルチサンプルテクスチャを作成
			if( OrigTex->PF->D3D11.MSTexture == NULL && Samples > 1 )
			{
				_MEMSET( &Texture2DDesc, 0, sizeof( Texture2DDesc ) ) ;
				Texture2DDesc.Usage              = D_D3D11_USAGE_DEFAULT ;
	//			Texture2DDesc.Usage              = D_D3D11_USAGE_DYNAMIC ;
				Texture2DDesc.Format             = GD3D11.Device.Caps.TextureFormat[ Orig->ColorFormat ] ;
				Texture2DDesc.BindFlags          = ( UINT )( D_D3D11_BIND_SHADER_RESOURCE | ( Orig->FormatDesc.DrawValidFlag ? D_D3D11_BIND_RENDER_TARGET : 0 ) ) ;
				Texture2DDesc.Width              = ( UINT )OrigTex->TexWidth ;
				Texture2DDesc.Height             = ( UINT )OrigTex->TexHeight ;
				Texture2DDesc.CPUAccessFlags     = 0 ;
	//			Texture2DDesc.CPUAccessFlags     = D_D3D11_CPU_ACCESS_WRITE ;
				Texture2DDesc.MipLevels          = ( UINT )Orig->Hard.MipMapCount ;
				Texture2DDesc.ArraySize          = ( UINT )( Orig->FormatDesc.CubeMapTextureFlag ? CUBEMAP_SURFACE_NUM : 1 ) ;
				Texture2DDesc.SampleDesc.Count   = Samples ;
				Texture2DDesc.SampleDesc.Quality = Quality ;
				Texture2DDesc.MiscFlags          = ( UINT )( Orig->FormatDesc.CubeMapTextureFlag ? D_D3D11_RESOURCE_MISC_TEXTURECUBE : 0 ) ;
				hr = D3D11Device_CreateTexture2D_ASync( &Texture2DDesc, NULL, &OrigTex->PF->D3D11.MSTexture, ASyncThread ) ;
				if( FAILED( hr ) )
				{
					DXST_LOGFILE_ADDUTF16LE( "\xde\x30\xeb\x30\xc1\x30\xb5\x30\xf3\x30\xd7\x30\xeb\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"マルチサンプルテクスチャの作成に失敗しました\n" @*/ ) ;
					return -1 ;
				}
			}

			// テクスチャの作成
			if( OrigTex->PF->D3D11.Texture == NULL )
			{
				_MEMSET( &Texture2DDesc, 0, sizeof( Texture2DDesc ) ) ;
				Texture2DDesc.Usage              = D_D3D11_USAGE_DEFAULT ;
	//			Texture2DDesc.Usage              = D_D3D11_USAGE_DYNAMIC ;
				Texture2DDesc.Format             = GD3D11.Device.Caps.TextureFormat[ Orig->ColorFormat ] ;
				Texture2DDesc.BindFlags          = ( UINT )( D_D3D11_BIND_SHADER_RESOURCE | ( Orig->FormatDesc.DrawValidFlag ? D_D3D11_BIND_RENDER_TARGET : 0 ) ) ;
				Texture2DDesc.Width              = ( UINT )OrigTex->TexWidth ;
				Texture2DDesc.Height             = ( UINT )OrigTex->TexHeight ;
				Texture2DDesc.CPUAccessFlags     = 0 ;
	//			Texture2DDesc.CPUAccessFlags     = D_D3D11_CPU_ACCESS_WRITE ;
				Texture2DDesc.MipLevels          = ( UINT )Orig->Hard.MipMapCount ;
				Texture2DDesc.ArraySize          = ( UINT )( Orig->FormatDesc.CubeMapTextureFlag ? CUBEMAP_SURFACE_NUM : 1 ) ;
				Texture2DDesc.SampleDesc.Count   = 1 ;
				Texture2DDesc.SampleDesc.Quality = 0 ;
				Texture2DDesc.MiscFlags          = ( UINT )( Orig->FormatDesc.CubeMapTextureFlag ? D_D3D11_RESOURCE_MISC_TEXTURECUBE : 0 ) ;
				hr = D3D11Device_CreateTexture2D_ASync( &Texture2DDesc, NULL, &OrigTex->PF->D3D11.Texture, ASyncThread ) ;
				if( FAILED( hr ) )
				{
					DXST_LOGFILE_ADDUTF16LE( "\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"テクスチャの作成に失敗しました\n" @*/ ) ;
					return -1 ;
				}
			}

			// 描画対象にすることができる場合は処理を分岐
			if( Orig->FormatDesc.DrawValidFlag )
			{
				int ArraySize ;

				ArraySize = Orig->FormatDesc.CubeMapTextureFlag ? CUBEMAP_SURFACE_NUM : 1 ;

				// レンダーターゲットビューのポインタを保存するためのメモリを確保
				if( OrigTex->PF->D3D11.TextureRTV == NULL )
				{
					OrigTex->PF->D3D11.TextureRTV = ( D_ID3D11RenderTargetView ** )DXALLOC( sizeof( D_ID3D11RenderTargetView * ) * ArraySize * Orig->Hard.MipMapCount ) ;
					if( OrigTex->PF->D3D11.TextureRTV == NULL )
					{
						DXST_LOGFILE_ADDUTF16LE( "\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x28\x75\xec\x30\xf3\x30\xc0\x30\xfc\x30\xbf\x30\xfc\x30\xb2\x30\xc3\x30\xc8\x30\xd3\x30\xe5\x30\xfc\x30\x6e\x30\xa2\x30\xc9\x30\xec\x30\xb9\x30\x92\x30\xdd\x4f\x58\x5b\x59\x30\x8b\x30\x5f\x30\x81\x30\x6e\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"テクスチャ用レンダーターゲットビューのアドレスを保存するためのメモリの確保に失敗しました\n" @*/ ) ;
						return -1 ;
					}
					_MEMSET( OrigTex->PF->D3D11.TextureRTV, 0, sizeof( D_ID3D11RenderTargetView * ) * ArraySize * Orig->Hard.MipMapCount ) ;
				}

				for( j = 0 ; j < ArraySize ; j ++ )
				{
					for( k = 0 ; k < Orig->Hard.MipMapCount ; k ++ )
					{
						// レンダーターゲットビューを作成
						if( OrigTex->PF->D3D11.TextureRTV[ j * Orig->Hard.MipMapCount + k ] == NULL )
						{
							_MEMSET( &RTVDesc, 0, sizeof( RTVDesc ) );
							RTVDesc.Format = GD3D11.Device.Caps.TextureFormat[ Orig->ColorFormat ] ;
							if( Orig->FormatDesc.CubeMapTextureFlag )
							{
								RTVDesc.ViewDimension					= D_D3D11_RTV_DIMENSION_TEXTURE2DARRAY ;
								RTVDesc.Texture2DArray.FirstArraySlice	= ( UINT )j ;
								RTVDesc.Texture2DArray.ArraySize		= 1 ;
								RTVDesc.Texture2DArray.MipSlice			= k ;
							}
							else
							{
								RTVDesc.Texture2D.MipSlice = k ;
								if( Samples <= 1 )
								{
									RTVDesc.ViewDimension = D_D3D11_RTV_DIMENSION_TEXTURE2D ;
								}
								else
								{
									RTVDesc.ViewDimension = D_D3D11_RTV_DIMENSION_TEXTURE2DMS ;
								}
							}
							hr = D3D11Device_CreateRenderTargetView_ASync(
								OrigTex->PF->D3D11.MSTexture != NULL ? OrigTex->PF->D3D11.MSTexture : OrigTex->PF->D3D11.Texture,
								&RTVDesc,
								&OrigTex->PF->D3D11.TextureRTV[ j * Orig->Hard.MipMapCount + k ],
								ASyncThread
							) ;
							if( FAILED( hr ) )
							{
								DXST_LOGFILEFMT_ADDUTF16LE(( "\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x28\x75\xec\x30\xf3\x30\xc0\x30\xfc\x30\xbf\x30\xfc\x30\xb2\x30\xc3\x30\xc8\x30\xd3\x30\xe5\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x20\x00\x46\x00\x6f\x00\x72\x00\x6d\x00\x61\x00\x74\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x20\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x58\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x20\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x59\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x0a\x00\x00"/*@ L"テクスチャ用レンダーターゲットビューの作成に失敗しました  Format:%d  SizeX:%d  SizeY:%d \n" @*/,
									GD3D11.Device.Caps.TextureFormat[ Orig->ColorFormat ], OrigTex->TexWidth, OrigTex->TexHeight )) ;
								return -1 ;
							}
						}
					}
				}

				// マルチサンプルか、深度バッファを作成する指定がある場合は深度バッファを作成
				if( ( ( GSYS.Setting.FSAAMultiSampleCount == 1 && Samples >  1 ) ||
					  ( GSYS.Setting.FSAAMultiSampleCount >  1 && Samples == 1 ) ||
					  Orig->ZBufferFlag ) )
				{
					if( OrigTex->PF->D3D11.DepthBuffer != NULL )
					{
						Direct3D11_Release_Texture2D_ASync( OrigTex->PF->D3D11.DepthBuffer ) ;
						OrigTex->PF->D3D11.DepthBuffer = NULL ;
					}

					if( OrigTex->PF->D3D11.DepthBufferDSV != NULL )
					{
						for( j = 0 ; j < Orig->Hard.MipMapCount ; j ++ )
						{
							if( OrigTex->PF->D3D11.DepthBufferDSV[ j ] != NULL )
							{
								Direct3D11_Release_DepthStencilView_ASync( OrigTex->PF->D3D11.DepthBufferDSV[ j ] ) ;
								OrigTex->PF->D3D11.DepthBufferDSV[ j ] = NULL ;
							}
						}
						DXFREE( OrigTex->PF->D3D11.DepthBufferDSV ) ;
						OrigTex->PF->D3D11.DepthBufferDSV = NULL ;
					}

					// テクスチャの作成
					_MEMSET( &Texture2DDesc, 0, sizeof( Texture2DDesc ) ) ;
					Texture2DDesc.Width              = ( UINT )OrigTex->TexWidth ;
					Texture2DDesc.Height             = ( UINT )OrigTex->TexHeight ;
					Texture2DDesc.MipLevels          = Orig->Hard.MipMapCount ;
					Texture2DDesc.ArraySize          = 1 ;
					Texture2DDesc.Format             = GD3D11.Device.Caps.DepthBufferTexture2DFormat[ Orig->ZBufferBitDepthIndex ] ;
					Texture2DDesc.SampleDesc.Count   = Samples ;
					Texture2DDesc.SampleDesc.Quality = Quality ;
					Texture2DDesc.Usage              = D_D3D11_USAGE_DEFAULT ;
					Texture2DDesc.BindFlags          = D_D3D11_BIND_DEPTH_STENCIL ;
					Texture2DDesc.CPUAccessFlags     = 0 ;
					Texture2DDesc.MiscFlags          = 0 ;
					hr = D3D11Device_CreateTexture2D_ASync( &Texture2DDesc, NULL, &OrigTex->PF->D3D11.DepthBuffer, ASyncThread ) ;
					if( FAILED( hr ) )
					{
						DXST_LOGFILE_ADDUTF16LE( "\xf8\x66\x4d\x30\xbc\x8f\x7f\x30\x02\x5c\x28\x75\xde\x30\xeb\x30\xc1\x30\xb5\x30\xf3\x30\xd7\x30\xeb\x30\xf1\x6d\xa6\x5e\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"書き込み専用マルチサンプル深度テクスチャの作成に失敗しました\n" @*/ ) ;
						return -1 ;
					}

					// 深度ステンシルビューのポインタを保存するためのメモリを確保
					if( OrigTex->PF->D3D11.DepthBufferDSV == NULL )
					{
						OrigTex->PF->D3D11.DepthBufferDSV = ( D_ID3D11DepthStencilView ** )DXALLOC( sizeof( D_ID3D11DepthStencilView * ) * Orig->Hard.MipMapCount ) ;
						if( OrigTex->PF->D3D11.DepthBufferDSV == NULL )
						{
							DXST_LOGFILE_ADDUTF16LE( "\xf1\x6d\xa6\x5e\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x28\x75\xf1\x6d\xa6\x5e\xb9\x30\xc6\x30\xf3\x30\xb7\x30\xeb\x30\xd3\x30\xe5\x30\xfc\x30\x6e\x30\xa2\x30\xc9\x30\xec\x30\xb9\x30\x92\x30\xdd\x4f\x58\x5b\x59\x30\x8b\x30\x5f\x30\x81\x30\x6e\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"深度バッファ用深度ステンシルビューのアドレスを保存するためのメモリの確保に失敗しました\n" @*/ ) ;
							return -1 ;
						}
						_MEMSET( OrigTex->PF->D3D11.DepthBufferDSV, 0, sizeof( D_ID3D11DepthStencilView * ) * Orig->Hard.MipMapCount ) ;
					}

					// 深度ステンシルビューの作成
					for( j = 0 ; j < Orig->Hard.MipMapCount ; j ++ )
					{
						_MEMSET( &DepthStencilViewDesc, 0, sizeof( DepthStencilViewDesc ) ) ;
						DepthStencilViewDesc.Format = GD3D11.Device.Caps.DepthBufferDepthStencilFormat[ Orig->ZBufferBitDepthIndex ] ;
						DepthStencilViewDesc.Texture2D.MipSlice = j ;
						if( Samples <= 1 )
						{
							DepthStencilViewDesc.ViewDimension = D_D3D11_DSV_DIMENSION_TEXTURE2D;
						}
						else
						{
							DepthStencilViewDesc.ViewDimension = D_D3D11_DSV_DIMENSION_TEXTURE2DMS;
						}

						hr = D3D11Device_CreateDepthStencilView_ASync( OrigTex->PF->D3D11.DepthBuffer, &DepthStencilViewDesc, &OrigTex->PF->D3D11.DepthBufferDSV[ j ], ASyncThread ) ;
						if( FAILED( hr ) )
						{
							DXST_LOGFILE_ADDUTF16LE( "\xf1\x6d\xa6\x5e\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x28\x75\xf1\x6d\xa6\x5e\xb9\x30\xc6\x30\xf3\x30\xb7\x30\xeb\x30\xd3\x30\xe5\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"深度バッファ用深度ステンシルビューの作成に失敗しました\n" @*/ ) ;
							return -1 ;
						}
					}
				}
				else
				{
					if( OrigTex->PF->D3D11.DepthBuffer != NULL )
					{
						Direct3D11_Release_Texture2D_ASync( OrigTex->PF->D3D11.DepthBuffer ) ;
						OrigTex->PF->D3D11.DepthBuffer = NULL ;
					}

					if( OrigTex->PF->D3D11.DepthBufferDSV != NULL )
					{
						for( j = 0 ; j < Orig->Hard.MipMapCount ; j ++ )
						{
							if( OrigTex->PF->D3D11.DepthBufferDSV[ j ] != NULL )
							{
								Direct3D11_Release_DepthStencilView_ASync( OrigTex->PF->D3D11.DepthBufferDSV[ j ] ) ;
								OrigTex->PF->D3D11.DepthBufferDSV[ j ] = NULL ;
							}
						}
						DXFREE( OrigTex->PF->D3D11.DepthBufferDSV ) ;
						OrigTex->PF->D3D11.DepthBufferDSV = NULL ;
					}
				}
			}
		}

		// シェーダーリソースビューを作成
		if( OrigTex->PF->D3D11.TextureSRV == NULL )
		{
			_MEMSET( &SRVDesc, 0, sizeof( SRVDesc ) );
			SRVDesc.Format              = GD3D11.Device.Caps.TextureFormat[ Orig->ColorFormat ] ;
			if( Orig->FormatDesc.CubeMapTextureFlag )
			{
				SRVDesc.ViewDimension               = D_D3D11_SRV_DIMENSION_TEXTURECUBE ;
				SRVDesc.TextureCube.MipLevels       = ( UINT )Orig->Hard.MipMapCount ;
				SRVDesc.TextureCube.MostDetailedMip = 0 ;
			}
			else
			{
//				if( Samples <= 1 )
//				{
					SRVDesc.ViewDimension       = D_D3D11_SRV_DIMENSION_TEXTURE2D ;
					SRVDesc.Texture2D.MipLevels = ( UINT )Orig->Hard.MipMapCount ;
//				}
//				else
//				{
//					SRVDesc.ViewDimension       = D_D3D11_SRV_DIMENSION_TEXTURE2DMS ;
//				}
			}
			hr = D3D11Device_CreateShaderResourceView_ASync( OrigTex->PF->D3D11.Texture, &SRVDesc, &OrigTex->PF->D3D11.TextureSRV, ASyncThread ) ;
			if( FAILED( hr ) )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x28\x75\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\xea\x30\xbd\x30\xfc\x30\xb9\x30\xd3\x30\xe5\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x20\x00\x46\x00\x6f\x00\x72\x00\x6d\x00\x61\x00\x74\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x20\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x58\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x20\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x59\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x0a\x00\x00"/*@ L"テクスチャ用シェーダーリソースビューの作成に失敗しました  Format:%d  SizeX:%d  SizeY:%d \n" @*/,
					GD3D11.Device.Caps.TextureFormat[ Orig->ColorFormat ], OrigTex->TexWidth, OrigTex->TexHeight )) ;
				return -1 ;
			}
		}

		// 描画対象に出来る画像の場合は真っ黒に塗りつぶす
		if( Orig->FormatDesc.DrawValidFlag )
		{
			int RTVNum ;

			RTVNum = ( Orig->FormatDesc.CubeMapTextureFlag ? CUBEMAP_SURFACE_NUM : 1 ) * Orig->Hard.MipMapCount ;
			for( j = 0 ; j < RTVNum ; j ++ )
			{
				if( OrigTex->PF->D3D11.TextureRTV[ j ] != NULL )
				{
					FLOAT ClearColor[ 4 ] = { 0.0f } ;

					D3D11DeviceContext_ClearRenderTargetView_ASync( OrigTex->PF->D3D11.TextureRTV[ j ], ClearColor, ASyncThread ) ;
				}
			}

			if( OrigTex->PF->D3D11.DepthBufferDSV != NULL )
			{
				for( j = 0 ; j < Orig->Hard.MipMapCount ; j ++ )
				{
					D3D11DeviceContext_ClearDepthStencilView_ASync( OrigTex->PF->D3D11.DepthBufferDSV[ j ], D_D3D11_CLEAR_DEPTH, 1.0f, 0, ASyncThread ) ;
				}
			}
		}
	}

	// 終了
	return 0 ;
}

// テクスチャシェーダーリソースビューを解放する
static	int		Graphics_Hardware_D3D11_ReleaseTextureSRV( D_ID3D11ShaderResourceView *TextureSRV )
{
	int i ;
	int Flag ;
	D_ID3D11ShaderResourceView *pShaderResourceView = NULL ;

	// ピクセルシェーダーで使用するテクスチャとして設定されていたら解除する
	Flag = FALSE ;
	for( i = 0 ; i < D_D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT ; i ++ )
	{
		if( GD3D11.Device.State.PSSetShaderResourceView[ i ] == TextureSRV )
		{
			if( Flag == FALSE )
			{
				// 描画待機している描画物を描画
				DRAWSTOCKINFO
				Flag = TRUE ;
			}

			Graphics_D3D11_DeviceState_SetPSShaderResouceView( i, 1, &pShaderResourceView ) ;
		}
	}

	// どこかのスロットにセットされていたら描画用テクスチャも解除
	if( Flag )
	{
		Graphics_D3D11_DrawSetting_SetTexture( NULL, NULL ) ;
	}

	// シェーダーリソースビューを解放する
	Direct3D11_Release_ShaderResourceView( TextureSRV ) ;

	return 0 ;
}

// オリジナル画像情報中のテクスチャを解放する
extern	int		Graphics_Hardware_D3D11_ReleaseOrigTexture_PF( IMAGEDATA_ORIG *Orig )
{
	int i ;
	int j ;
	IMAGEDATA_ORIG_HARD_TEX *OrigTex ;

	// ハードウエアを使用する画像ではない場合は何もせずに終了
	if( Orig->FormatDesc.TextureFlag == FALSE )
	{
		return 0 ;
	}

	OrigTex = Orig->Hard.Tex ;
	for( i = 0 ; i < Orig->Hard.TexNum ; i ++, OrigTex ++ )
	{
		if( OrigTex->PF->D3D11.TextureRTV != NULL )
		{
			int RTVNum ;

			RTVNum = ( Orig->FormatDesc.CubeMapTextureFlag ? CUBEMAP_SURFACE_NUM : 1 ) * Orig->Hard.MipMapCount ;
			for( j = 0 ; j < RTVNum ; j ++ )
			{
				if( OrigTex->PF->D3D11.TextureRTV[ j ] )
				{
					Direct3D11_Release_RenderTargetView( OrigTex->PF->D3D11.TextureRTV[ j ] ) ;
					OrigTex->PF->D3D11.TextureRTV[ j ] = NULL ;
				}
			}
			DXFREE( OrigTex->PF->D3D11.TextureRTV ) ;
			OrigTex->PF->D3D11.TextureRTV = NULL ;
		}
		if( OrigTex->PF->D3D11.TextureSRV )
		{
			Graphics_Hardware_D3D11_ReleaseTextureSRV( OrigTex->PF->D3D11.TextureSRV ) ;
			OrigTex->PF->D3D11.TextureSRV = NULL ;
		}
		if( OrigTex->PF->D3D11.MSTexture )
		{
			Direct3D11_Release_Texture2D( OrigTex->PF->D3D11.MSTexture ) ;
			OrigTex->PF->D3D11.MSTexture = NULL ;
		}
		if( OrigTex->PF->D3D11.Texture )
		{
			// ユーザー指定のテクスチャではなかった場合のみ解放
			if( Orig->UserPlatformTexture == NULL )
			{
				Direct3D11_Release_Texture2D( OrigTex->PF->D3D11.Texture ) ;
			}
			OrigTex->PF->D3D11.Texture = NULL ;
		}

		if( OrigTex->PF->D3D11.DepthBufferDSV != NULL )
		{
			for( j = 0 ; j < Orig->Hard.MipMapCount ; j ++ )
			{
				if( OrigTex->PF->D3D11.DepthBufferDSV[ j ] != NULL )
				{
					Direct3D11_Release_DepthStencilView( OrigTex->PF->D3D11.DepthBufferDSV[ j ] ) ;
					OrigTex->PF->D3D11.DepthBufferDSV[ j ] = NULL ;
				}
			}
			DXFREE( OrigTex->PF->D3D11.DepthBufferDSV ) ;
			OrigTex->PF->D3D11.DepthBufferDSV = NULL ;
		}
		if( OrigTex->PF->D3D11.DepthBuffer )
		{
			Direct3D11_Release_Texture2D( OrigTex->PF->D3D11.DepthBuffer ) ;
			OrigTex->PF->D3D11.DepthBuffer = NULL ;
		}
	}

	// 終了
	return 0 ;
}

// 指定のマルチサンプル数で使用できる最大クオリティ値を取得する
extern	int		Graphics_Hardware_D3D11_GetMultiSampleQuality_PF( int Samples )
{
	IMAGEFORMATDESC				Format ;
	SETUP_GRAPHHANDLE_GPARAM	GParam ;
	int							FormatIndex ;
	D_DXGI_FORMAT				D3DFormat ;
	UINT						Quality ;

	if( Samples > 16 )
	{
		Samples = 16 ;
	}

	if( GAPIWin.D3D11DeviceObject == NULL )
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
	D3DFormat = GD3D11.Device.Caps.TextureFormat[ FormatIndex ] ;
	if( D3DFormat == D_DXGI_FORMAT_UNKNOWN )
	{
		return -1 ;
	}

	// 設定できるクオリティを取得
	Quality = 10000 ;
	if( D3D11Device_CheckMultiSampleParam_ASync( D3DFormat, ( UINT * )&Samples, &Quality, TRUE ) < 0 )
	{
		return -1 ;
	}

	// クオリティを返す
	return ( int )Quality ;
}

// 作成するグラフィックハンドルで使用する環境依存のテクスチャフォーマットを指定する
extern int Graphics_Hardware_D3D11_SetUsePlatformTextureFormat_PF( int /*PlatformTextureFormat*/ )
{
	return 0 ;
}




























// 環境依存頂点バッファ・インデックスバッファ関係

// 頂点バッファハンドルの頂点バッファを作成する
extern	int		Graphics_Hardware_D3D11_VertexBuffer_Create_PF( VERTEXBUFFERHANDLEDATA *VertexBuffer )
{
	return Graphics_D3D11_VertexBuffer_CreateObject( VertexBuffer, FALSE ) ;
}

// 頂点バッファハンドルの後始末
extern	int		Graphics_Hardware_D3D11_VertexBuffer_Terminate_PF( VERTEXBUFFERHANDLEDATA *VertexBuffer )
{
	return Graphics_D3D11_VertexBuffer_ReleaseObject( VertexBuffer ) ;
}

// 頂点バッファに頂点データを転送する
extern	int		Graphics_Hardware_D3D11_VertexBuffer_SetData_PF( VERTEXBUFFERHANDLEDATA *VertexBuffer, int SetIndex, const void *VertexData, int VertexNum )
{
	D_D3D11_BOX DestBox ;

	DestBox.left   = ( UINT )(   SetIndex               * VertexBuffer->UnitSize ) ;
	DestBox.top    = 0 ;
	DestBox.front  = 0 ;
	DestBox.right  = ( UINT )( ( SetIndex + VertexNum ) * VertexBuffer->UnitSize ) ;
	DestBox.bottom = 1 ;
	DestBox.back   = 1 ;

	D3D11DeviceContext_UpdateSubresource_ASync( VertexBuffer->PF->D3D11.VertexBuffer, 0, &DestBox, VertexData, 0, 0 ) ;

	// 正常終了
	return 0 ;
}

// インデックスバッファハンドルのセットアップを行う
extern	int		Graphics_Hardware_D3D11_IndexBuffer_Create_PF( INDEXBUFFERHANDLEDATA *IndexBuffer )
{
	return Graphics_D3D11_IndexBuffer_CreateObject( IndexBuffer, FALSE ) ;
}

// インデックスバッファハンドルの後始末
extern	int		Graphics_Hardware_D3D11_IndexBuffer_Terminate_PF( INDEXBUFFERHANDLEDATA *IndexBuffer )
{
	return Graphics_D3D11_IndexBuffer_ReleaseObject( IndexBuffer ) ;
}

// インデックスバッファにインデックスデータを転送する
extern	int		Graphics_Hardware_D3D11_IndexBuffer_SetData_PF( INDEXBUFFERHANDLEDATA *IndexBuffer, int SetIndex, const void *IndexData, int IndexNum )
{
	D_D3D11_BOX DestBox ;

	DestBox.left   = ( UINT )(   SetIndex              * IndexBuffer->UnitSize ) ;
	DestBox.top    = 0 ;
	DestBox.front  = 0 ;
	DestBox.right  = ( UINT )( ( SetIndex + IndexNum ) * IndexBuffer->UnitSize ) ;
	DestBox.bottom = 1 ;
	DestBox.back   = 1 ;

	D3D11DeviceContext_UpdateSubresource_ASync( IndexBuffer->PF->D3D11.IndexBuffer, 0, &DestBox, IndexData, 0, 0 ) ;

	// 正常終了
	return 0 ;
}























// 環境依存ライト関係

// ライティングを行うかどうかを設定する
extern	int		Graphics_Hardware_D3D11_Light_SetUse_PF( int Flag )
{
	return Graphics_D3D11_DeviceState_SetLighting( Flag ) ;
}

// グローバルアンビエントライトカラーを設定する
extern	int		Graphics_Hardware_D3D11_Light_GlobalAmbient_PF( COLOR_F *Color )
{
	return Graphics_D3D11_DeviceState_SetAmbient( Color ) ;
}

// ライトパラメータをセット
extern	int		Graphics_Hardware_D3D11_Light_SetState_PF( int LightNumber, LIGHTPARAM *LightParam )
{
	return Graphics_D3D11_DeviceState_SetLightState( LightNumber, LightParam ) ;
}

// ライトの有効、無効を変更
extern	int		Graphics_Hardware_D3D11_Light_SetEnable_PF( int LightNumber, int EnableState )
{
	return Graphics_D3D11_DeviceState_SetLightEnable( LightNumber, EnableState ) ;
}




























// 環境依存シャドウマップ関係

// シャドウマップデータに必要なテクスチャを作成する
extern	int		Graphics_Hardware_D3D11_ShadowMap_CreateTexture_PF( SHADOWMAPDATA *ShadowMap, int ASyncThread )
{
	D_D3D11_TEXTURE2D_DESC				Texture2DDesc ;
	D_D3D11_RENDER_TARGET_VIEW_DESC		RTVDesc ;
	D_D3D11_SHADER_RESOURCE_VIEW_DESC	SRVDesc ;
	D_D3D11_DEPTH_STENCIL_VIEW_DESC		DepthStencilViewDesc ;
	HRESULT								hr ;

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
	if( GD3D11.Device.Caps.TextureFormat[ ShadowMap->ColorFormat ] == D_DXGI_FORMAT_UNKNOWN )
	{
		DXST_LOGFILE_ADDUTF16LE( "\xb7\x30\xe3\x30\xc9\x30\xa6\x30\xde\x30\xc3\x30\xd7\x30\xc7\x30\xfc\x30\xbf\x30\x28\x75\x6e\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x67\x30\x5c\x4f\x10\x62\x67\x30\x4d\x30\x8b\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"シャドウマップデータ用のフォーマットで作成できるテクスチャフォーマットがありませんでした\n" @*/ ) ;
		return -1 ;
	}

	// レンダリングターゲット深度バッファの作成
	if( ShadowMap->PF->D3D11.DepthBuffer == NULL )
	{
		// テクスチャの作成
		_MEMSET( &Texture2DDesc, 0, sizeof( Texture2DDesc ) ) ;
		Texture2DDesc.Width              = ( UINT )ShadowMap->BaseSizeX ;
		Texture2DDesc.Height             = ( UINT )ShadowMap->BaseSizeY ;
		Texture2DDesc.MipLevels          = 1 ;
		Texture2DDesc.ArraySize          = 1 ;
		Texture2DDesc.Format             = GD3D11.Device.Caps.DepthBufferTexture2DFormat[ ShadowMap->ZBufferFormat ] ;
		Texture2DDesc.SampleDesc.Count   = 1 ;
		Texture2DDesc.SampleDesc.Quality = 0 ;
		Texture2DDesc.Usage              = D_D3D11_USAGE_DEFAULT ;
		Texture2DDesc.BindFlags          = D_D3D11_BIND_DEPTH_STENCIL ;
		Texture2DDesc.CPUAccessFlags     = 0 ;
		Texture2DDesc.MiscFlags          = 0 ;
		hr = D3D11Device_CreateTexture2D_ASync( &Texture2DDesc, NULL, &ShadowMap->PF->D3D11.DepthBuffer, ASyncThread ) ;
		if( FAILED( hr ) )
		{
			DXST_LOGFILE_ADDUTF16LE( "\xb7\x30\xe3\x30\xc9\x30\xa6\x30\xde\x30\xc3\x30\xd7\x30\x28\x75\xf1\x6d\xa6\x5e\xd0\x30\xc3\x30\xd5\x30\xa1\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"シャドウマップ用深度バッファテクスチャの作成に失敗しました\n" @*/ ) ;
			return -1 ;
		}

		// 深度ステンシルビューの作成
		_MEMSET( &DepthStencilViewDesc, 0, sizeof( DepthStencilViewDesc ) ) ;
		DepthStencilViewDesc.Format             = GD3D11.Device.Caps.DepthBufferDepthStencilFormat[ ShadowMap->ZBufferFormat ] ;
		DepthStencilViewDesc.ViewDimension      = D_D3D11_DSV_DIMENSION_TEXTURE2D;
		DepthStencilViewDesc.Texture2D.MipSlice = 0;

		hr = D3D11Device_CreateDepthStencilView_ASync( ShadowMap->PF->D3D11.DepthBuffer, &DepthStencilViewDesc, &ShadowMap->PF->D3D11.DepthBufferDSV, ASyncThread ) ;
		if( FAILED( hr ) )
		{
			DXST_LOGFILE_ADDUTF16LE( "\xb7\x30\xe3\x30\xc9\x30\xa6\x30\xde\x30\xc3\x30\xd7\x30\x28\x75\xf1\x6d\xa6\x5e\xb9\x30\xc6\x30\xf3\x30\xb7\x30\xeb\x30\xd3\x30\xe5\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"シャドウマップ用深度ステンシルビューの作成に失敗しました\n" @*/ ) ;
			return -1 ;
		}
	}

	// レンダーターゲットにできるテクスチャの作成
	if( ShadowMap->PF->D3D11.DepthTexture == NULL )
	{
		_MEMSET( &Texture2DDesc, 0, sizeof( Texture2DDesc ) ) ;
		Texture2DDesc.Usage              = D_D3D11_USAGE_DEFAULT ;
		Texture2DDesc.Format             = GD3D11.Device.Caps.TextureFormat[ ShadowMap->ColorFormat ] ;
		Texture2DDesc.BindFlags          = D_D3D11_BIND_SHADER_RESOURCE | D_D3D11_BIND_RENDER_TARGET ;
		Texture2DDesc.Width              = ( UINT )ShadowMap->BaseSizeX ;
		Texture2DDesc.Height             = ( UINT )ShadowMap->BaseSizeY ;
		Texture2DDesc.CPUAccessFlags     = 0 ;
		Texture2DDesc.MipLevels          = 1 ;
		Texture2DDesc.ArraySize          = 1 ;
		Texture2DDesc.SampleDesc.Count   = 1 ;
		Texture2DDesc.SampleDesc.Quality = 0 ;
		hr = D3D11Device_CreateTexture2D_ASync( &Texture2DDesc, NULL, &ShadowMap->PF->D3D11.DepthTexture, ASyncThread ) ;
		if( FAILED( hr ) )
		{
			DXST_LOGFILE_ADDUTF16LE( "\xb7\x30\xe3\x30\xc9\x30\xa6\x30\xde\x30\xc3\x30\xd7\x30\x28\x75\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"シャドウマップ用テクスチャの作成に失敗しました\n" @*/ ) ;
			return -1 ;
		}

		_MEMSET( &RTVDesc, 0, sizeof( RTVDesc ) );
		RTVDesc.Format        = GD3D11.Device.Caps.TextureFormat[ ShadowMap->ColorFormat ] ;
		RTVDesc.ViewDimension = D_D3D11_RTV_DIMENSION_TEXTURE2D ;
		hr = D3D11Device_CreateRenderTargetView_ASync( ShadowMap->PF->D3D11.DepthTexture, &RTVDesc, &ShadowMap->PF->D3D11.DepthTextureRTV, ASyncThread ) ;
		if( FAILED( hr ) )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xb7\x30\xe3\x30\xc9\x30\xa6\x30\xde\x30\xc3\x30\xd7\x30\x28\x75\xec\x30\xf3\x30\xc0\x30\xfc\x30\xbf\x30\xfc\x30\xb2\x30\xc3\x30\xc8\x30\xd3\x30\xe5\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x20\x00\x46\x00\x6f\x00\x72\x00\x6d\x00\x61\x00\x74\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x20\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x58\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x20\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x59\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x0a\x00\x00"/*@ L"シャドウマップ用レンダーターゲットビューの作成に失敗しました  Format:%d  SizeX:%d  SizeY:%d \n" @*/,
				GD3D11.Device.Caps.TextureFormat[ ShadowMap->ColorFormat ], ShadowMap->BaseSizeX, ShadowMap->BaseSizeY )) ;
			return -1 ;
		}

		_MEMSET( &SRVDesc, 0, sizeof( SRVDesc ) );
		SRVDesc.Format              = GD3D11.Device.Caps.TextureFormat[ ShadowMap->ColorFormat ] ;
		SRVDesc.ViewDimension       = D_D3D11_SRV_DIMENSION_TEXTURE2D ;
		SRVDesc.Texture2D.MipLevels = 1 ;
		hr = D3D11Device_CreateShaderResourceView_ASync( ShadowMap->PF->D3D11.DepthTexture, &SRVDesc, &ShadowMap->PF->D3D11.DepthTextureSRV, ASyncThread ) ;
		if( FAILED( hr ) )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xb7\x30\xe3\x30\xc9\x30\xa6\x30\xde\x30\xc3\x30\xd7\x30\x28\x75\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\xea\x30\xbd\x30\xfc\x30\xb9\x30\xd3\x30\xe5\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x20\x00\x46\x00\x6f\x00\x72\x00\x6d\x00\x61\x00\x74\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x20\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x58\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x20\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x59\x00\x3a\x00\x25\x00\x64\x00\x20\x00\x0a\x00\x00"/*@ L"シャドウマップ用シェーダーリソースビューの作成に失敗しました  Format:%d  SizeX:%d  SizeY:%d \n" @*/,
				GD3D11.Device.Caps.TextureFormat[ ShadowMap->ColorFormat ], ShadowMap->BaseSizeX, ShadowMap->BaseSizeY )) ;
			return -1 ;
		}
	}

	// 終了
	return 0 ;
}

// シャドウマップデータに必要なテクスチャを解放する
extern	int		Graphics_Hardware_D3D11_ShadowMap_ReleaseTexture_PF( SHADOWMAPDATA *ShadowMap )
{
	if( ShadowMap->PF->D3D11.DepthTexture != NULL )
	{
		Direct3D11_Release_Texture2D( ShadowMap->PF->D3D11.DepthTexture ) ;
		ShadowMap->PF->D3D11.DepthTexture = NULL ;
	}

	if( ShadowMap->PF->D3D11.DepthTextureSRV != NULL )
	{
		Direct3D11_Release_ShaderResourceView( ShadowMap->PF->D3D11.DepthTextureSRV ) ;
		ShadowMap->PF->D3D11.DepthTextureSRV = NULL ;
	}

	if( ShadowMap->PF->D3D11.DepthTextureRTV != NULL )
	{
		Direct3D11_Release_RenderTargetView( ShadowMap->PF->D3D11.DepthTextureRTV ) ;
		ShadowMap->PF->D3D11.DepthTextureRTV = NULL ;
	}

	if( ShadowMap->PF->D3D11.DepthBuffer != NULL )
	{
		Direct3D11_Release_Texture2D( ShadowMap->PF->D3D11.DepthBuffer ) ;
		ShadowMap->PF->D3D11.DepthBuffer = NULL ;
	}

	if( ShadowMap->PF->D3D11.DepthBufferDSV != NULL )
	{
		Direct3D11_Release_DepthStencilView( ShadowMap->PF->D3D11.DepthBufferDSV ) ;
		ShadowMap->PF->D3D11.DepthBufferDSV = NULL ;
	}

	// 終了
	return 0 ;
}

// 頂点シェーダーに設定するシャドウマップの情報を更新する
extern	void	Graphics_Hardware_D3D11_ShadowMap_RefreshVSParam_PF( void )
{
	DX_D3D11_VS_CONST_BUFFER_OTHERMATRIX *ConstOtherMatrix ;
	SHADOWMAPDATA *ShadowMap ;
	static float TranspMatDef[ 4 ][ 4 ] =
	{
		{ 0.0f, 0.0f, 0.0f, -1.0f },
		{ 0.0f, 0.0f, 0.0f, -1.0f },
		{ 0.0f, 0.0f, 0.0f, -1.0f },
		{ 0.0f, 0.0f, 0.0f, -1.0f },
	} ;
	int i ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return ;
	}

	ConstOtherMatrix = ( DX_D3D11_VS_CONST_BUFFER_OTHERMATRIX * )GD3D11.Device.Shader.Constant.ConstBuffer_VS_OtherMatrix->SysmemBuffer ;

	for( i = 0 ; i < MAX_USE_SHADOWMAP_NUM ; i ++ )
	{
		// アドレスの取得
		if( SHADOWMAPCHKFULL( GSYS.DrawSetting.ShadowMap[ i ], ShadowMap ) )
		{
			_MEMCPY( ConstOtherMatrix->ShadowMapLightViewProjectionMatrix[ i ], TranspMatDef, sizeof( TranspMatDef ) ) ;
		}
		else
		{
			// 行列のセット
			ConstOtherMatrix->ShadowMapLightViewProjectionMatrix[ i ][ 0 ][ 0 ] = ShadowMap->ShadowMapViewProjectionMatrix.m[ 0 ][ 0 ] ;
			ConstOtherMatrix->ShadowMapLightViewProjectionMatrix[ i ][ 0 ][ 1 ] = ShadowMap->ShadowMapViewProjectionMatrix.m[ 1 ][ 0 ] ;
			ConstOtherMatrix->ShadowMapLightViewProjectionMatrix[ i ][ 0 ][ 2 ] = ShadowMap->ShadowMapViewProjectionMatrix.m[ 2 ][ 0 ] ;
			ConstOtherMatrix->ShadowMapLightViewProjectionMatrix[ i ][ 0 ][ 3 ] = ShadowMap->ShadowMapViewProjectionMatrix.m[ 3 ][ 0 ] ;

			ConstOtherMatrix->ShadowMapLightViewProjectionMatrix[ i ][ 1 ][ 0 ] = ShadowMap->ShadowMapViewProjectionMatrix.m[ 0 ][ 1 ] ;
			ConstOtherMatrix->ShadowMapLightViewProjectionMatrix[ i ][ 1 ][ 1 ] = ShadowMap->ShadowMapViewProjectionMatrix.m[ 1 ][ 1 ] ;
			ConstOtherMatrix->ShadowMapLightViewProjectionMatrix[ i ][ 1 ][ 2 ] = ShadowMap->ShadowMapViewProjectionMatrix.m[ 2 ][ 1 ] ;
			ConstOtherMatrix->ShadowMapLightViewProjectionMatrix[ i ][ 1 ][ 3 ] = ShadowMap->ShadowMapViewProjectionMatrix.m[ 3 ][ 1 ] ;

			ConstOtherMatrix->ShadowMapLightViewProjectionMatrix[ i ][ 2 ][ 0 ] = ShadowMap->ShadowMapViewProjectionMatrix.m[ 0 ][ 2 ] ;
			ConstOtherMatrix->ShadowMapLightViewProjectionMatrix[ i ][ 2 ][ 1 ] = ShadowMap->ShadowMapViewProjectionMatrix.m[ 1 ][ 2 ] ;
			ConstOtherMatrix->ShadowMapLightViewProjectionMatrix[ i ][ 2 ][ 2 ] = ShadowMap->ShadowMapViewProjectionMatrix.m[ 2 ][ 2 ] ;
			ConstOtherMatrix->ShadowMapLightViewProjectionMatrix[ i ][ 2 ][ 3 ] = ShadowMap->ShadowMapViewProjectionMatrix.m[ 3 ][ 2 ] ;

			ConstOtherMatrix->ShadowMapLightViewProjectionMatrix[ i ][ 3 ][ 0 ] = ShadowMap->ShadowMapViewProjectionMatrix.m[ 0 ][ 3 ] ;
			ConstOtherMatrix->ShadowMapLightViewProjectionMatrix[ i ][ 3 ][ 1 ] = ShadowMap->ShadowMapViewProjectionMatrix.m[ 1 ][ 3 ] ;
			ConstOtherMatrix->ShadowMapLightViewProjectionMatrix[ i ][ 3 ][ 2 ] = ShadowMap->ShadowMapViewProjectionMatrix.m[ 2 ][ 3 ] ;
			ConstOtherMatrix->ShadowMapLightViewProjectionMatrix[ i ][ 3 ][ 3 ] = ShadowMap->ShadowMapViewProjectionMatrix.m[ 3 ][ 3 ] ;
		}
	}

	GD3D11.Device.Shader.Constant.ConstBuffer_VS_OtherMatrix->ChangeFlag = TRUE ;

	// 定数バッファを更新
//	Graphics_D3D11_ConstantBuffer_Update( GD3D11.Device.Shader.Constant.ConstBuffer_VS_OtherMatrix ) ;
}

// ピクセルシェーダーに設定するシャドウマップの情報を更新する
extern	void	Graphics_Hardware_D3D11_ShadowMap_RefreshPSParam_PF( void )
{
	DX_D3D11_PS_CONST_BUFFER_SHADOWMAP *ConstantPSShadowMap ;
	SHADOWMAPDATA *ShadowMap ;
	LIGHT_HANDLE  *pLH ;
	int           CheckNum ;
	int           i ;
	int           j ;

	if( GAPIWin.D3D11DeviceObject == NULL )
	{
		return ;
	}

	ConstantPSShadowMap = ( DX_D3D11_PS_CONST_BUFFER_SHADOWMAP * )GD3D11.Device.Shader.Constant.ConstBuffer_PS_ShadowMap->SysmemBuffer ;

	CheckNum = GSYS.Light.EnableNum > 3 ? 3 : GSYS.Light.EnableNum ;

	for( i = 0 ; i < MAX_USE_SHADOWMAP_NUM ; i ++ )
	{
		ConstantPSShadowMap->Data[ i ].Enable_Light0 = 1.0f ;
		ConstantPSShadowMap->Data[ i ].Enable_Light1 = 1.0f ;
		ConstantPSShadowMap->Data[ i ].Enable_Light2 = 1.0f ;

		// アドレスの取得
		if( SHADOWMAPCHKFULL( GSYS.DrawSetting.ShadowMap[ i ], ShadowMap ) )
		{
			ConstantPSShadowMap->Data[ i ].AdjustDepth    = 1.0f ;
			ConstantPSShadowMap->Data[ i ].GradationParam = 0.0f ;
		}
		else
		{
			ConstantPSShadowMap->Data[ i ].AdjustDepth    = ShadowMap->AdjustDepth ;
			ConstantPSShadowMap->Data[ i ].GradationParam = ShadowMap->GradationParam ;

			for( j = 0 ; j < CheckNum ; j ++ )
			{
				pLH = GSYS.Light.Data[ GSYS.Light.EnableHardwareIndex[ j ] ] ;
				if( pLH->ShadowMapSlotDisableFlag[ i ] == FALSE )
				{
					switch( j )
					{
					case 0 :
						ConstantPSShadowMap->Data[ i ].Enable_Light0 = 0.0f ;
						break ;

					case 1 :
						ConstantPSShadowMap->Data[ i ].Enable_Light1 = 0.0f ;
						break ;

					case 2 :
						ConstantPSShadowMap->Data[ i ].Enable_Light2 = 0.0f ;
						break ;
					}
				}
			}
		}
	}
	GD3D11.Device.Shader.Constant.ConstBuffer_PS_ShadowMap->ChangeFlag = TRUE ;

	// 定数バッファを更新
//	Graphics_D3D11_ConstantBuffer_Update( GD3D11.Device.Shader.Constant.ConstBuffer_PS_ShadowMap ) ;
}

// シャドウマップへの描画の準備を行う
extern	int		Graphics_Hardware_D3D11_ShadowMap_DrawSetup_PF( SHADOWMAPDATA * /*ShadowMap*/ )
{
	// 正常終了
	return 0 ;
}

// シャドウマップへの描画を終了する
extern	int		Graphics_Hardware_D3D11_ShadowMap_DrawEnd_PF( SHADOWMAPDATA * /*ShadowMap*/ )
{
	// 正常終了
	return 0 ;
}

// 描画で使用するシャドウマップを指定する( スロットは０か１かを指定可能 )
extern	int		Graphics_Hardware_D3D11_ShadowMap_SetUse_PF( int /*SlotIndex*/, SHADOWMAPDATA * /*ShadowMap*/ )
{
	// パラメータが変更された、フラグを立てる
	GD3D11.Device.DrawSetting.ChangeBlendParamFlag = TRUE ;

	// Graphics_D3D11_DrawPreparation を行うべきフラグを立てる
	GD3D11.Device.DrawSetting.DrawPrepAlwaysFlag = TRUE ;

	// 正常終了
	return 0 ;
}































// 環境依存シェーダー関係

// シェーダーハンドルを初期化する
extern	int		Graphics_Hardware_D3D11_Shader_Create_PF( int ShaderHandle, int IsVertexShader, void *Image, int ImageSize, int ImageAfterFree, int ASyncThread )
{
	SHADERHANDLEDATA     *pShader ;
	D_ID3D11PixelShader  *PixelShader  = NULL ;
	D_ID3D11VertexShader *VertexShader = NULL ;

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
		if( D3D11Device_CreateVertexShader_ASync( Image, ( SIZE_T )ImageSize, NULL, &VertexShader, ASyncThread ) != S_OK )
		{
			DXST_LOGFILE_ADDUTF16LE( "\x02\x98\xb9\x70\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"頂点シェーダーの作成に失敗しました\n" @*/ ) ;
			goto ERR ;
		}
	}
	else
	{
		if( D3D11Device_CreatePixelShader_ASync( Image, ( SIZE_T )ImageSize, NULL, &PixelShader, ASyncThread ) != S_OK )
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

	// バイナリイメージのサイズを保存する
	pShader->FunctionCodeSize = ImageSize ;

	// 頂点シェーダーかどうかを保存する
	pShader->IsVertexShader = IsVertexShader ? TRUE : FALSE ;

	// シェーダーのアドレスを保存
	if( IsVertexShader )
	{
		pShader->PF->D3D11.VertexShader = VertexShader ;
	}
	else
	{
		pShader->PF->D3D11.PixelShader = PixelShader ;
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
		Direct3D11_Release_VertexShader_ASync( VertexShader, ASyncThread ) ;
	}

	if( PixelShader )
	{
		Direct3D11_Release_PixelShader_ASync( PixelShader, ASyncThread ) ;
	}

	// 処理後に解放すべしのフラグが立っていたら解放する
	if( ImageAfterFree )
	{
		DXFREE( Image ) ;
	}

	return -1 ;
}

// シェーダーハンドルの後始末
extern	int		Graphics_Hardware_D3D11_Shader_TerminateHandle_PF( SHADERHANDLEDATA *Shader )
{
	// シェーダーの解放
	if( Shader->IsVertexShader )
	{
		if( Shader->PF->D3D11.VertexShader )
		{
			Direct3D11_Release_VertexShader( Shader->PF->D3D11.VertexShader ) ;
			Shader->PF->D3D11.VertexShader = NULL ;
		}
	}
	else
	{
		if( Shader->PF->D3D11.PixelShader )
		{
			Direct3D11_Release_PixelShader( Shader->PF->D3D11.PixelShader ) ;
			Shader->PF->D3D11.PixelShader = NULL ;
		}
	}

	// 正常終了
	return 0 ;
}

// 使用できるシェーダーのバージョンを取得する( 0=使えない  200=シェーダーモデル２．０が使用可能  300=シェーダーモデル３．０が使用可能 )
extern	int		Graphics_Hardware_D3D11_Shader_GetValidShaderVersion_PF( void )
{
	switch( GD3D11.Setting.FeatureLevel )
	{
	case D_D3D_FEATURE_LEVEL_9_1 :
	case D_D3D_FEATURE_LEVEL_9_2 :
	case D_D3D_FEATURE_LEVEL_9_3 :
		return 200 ;

	case D_D3D_FEATURE_LEVEL_10_0 :
	case D_D3D_FEATURE_LEVEL_10_1 :
		return 400 ;

	case D_D3D_FEATURE_LEVEL_11_0 :
	case D_D3D_FEATURE_LEVEL_11_1 :
		return 500 ;
	}

	return 0 ;
}

// 指定の名前を持つ定数が使用するシェーダー定数の番号を取得する
extern	int		Graphics_Hardware_D3D11_Shader_GetConstIndex_PF( const wchar_t * /*ConstantName*/, SHADERHANDLEDATA * /*Shader*/ )
{
	return 0 ;
}

// 指定の名前を持つ定数が使用するシェーダー定数の数を取得する
extern	int		Graphics_Hardware_D3D11_Shader_GetConstCount_PF( const wchar_t * /*ConstantName*/, SHADERHANDLEDATA * /*Shader*/ )
{
	return 0 ;
}

// 指定の名前を持つ浮動小数点定数のデフォルトパラメータが格納されているメモリアドレスを取得する
extern	const FLOAT4 *Graphics_Hardware_D3D11_Shader_GetConstDefaultParamF_PF( const wchar_t * /*ConstantName*/, SHADERHANDLEDATA * /*Shader*/ )
{
	return 0 ;
}

// シェーダー定数情報を設定する
extern	int		Graphics_Hardware_D3D11_Shader_SetConst_PF( int /*TypeIndex*/, int /*SetIndex*/, int /*ConstantIndex*/, const void * /*Param*/, int /*ParamNum*/, int /*UpdateUseArea*/ )
{
	// 正常終了
	return 0 ;
}

// 指定領域のシェーダー定数情報をリセットする
extern	int		Graphics_Hardware_D3D11_Shader_ResetConst_PF( int /*TypeIndex*/, int /*SetIndex*/, int /*ConstantIndex*/, int /*ParamNum*/ )
{
	// 正常終了
	return 0 ;
}

#ifndef DX_NON_MODEL
// ３Ｄモデル用のシェーダーコードの初期化を行う
extern	int		Graphics_Hardware_D3D11_Shader_ModelCode_Init_PF( void )
{
	if( Graphics_D3D11_ShaderCode_Model_Initialize() == FALSE )
	{
		return -1 ;
	}

	// 正常終了
	return 0 ;
}
#endif // DX_NON_MODEL
































// 環境依存シェーダー用定数バッファ関係

// シェーダー用定数バッファハンドルを初期化する
extern	int		Graphics_Hardware_D3D11_ShaderConstantBuffer_Create_PF( int ShaderConstantBufferHandle, int BufferSize, int ASyncThread ) 
{
	SHADERCONSTANTBUFFERHANDLEDATA *ShaderConstantBuffer ;

	// エラー判定
	if( ASyncThread )
	{
		if( SHADERCONSTANTBUFFERCHK_ASYNC( ShaderConstantBufferHandle, ShaderConstantBuffer ) )
			return -1 ;
	}
	else
	{
		if( SHADERCONSTANTBUFFERCHK( ShaderConstantBufferHandle, ShaderConstantBuffer ) )
			return -1 ;
	}

	// シェーダー用定数バッファを作成する
	ShaderConstantBuffer->PF->D3D11.ConstBuffer = Graphics_D3D11_ConstantBuffer_Create( ( DWORD )BufferSize, ASyncThread ) ;
	if( ShaderConstantBuffer->PF->D3D11.ConstBuffer == NULL )
	{
		return -1 ;
	}

	// 正常終了
	return 0 ;
}

// シェーダー用定数バッファハンドルの後始末
extern	int		Graphics_Hardware_D3D11_ShaderConstantBuffer_TerminateHandle_PF( SHADERCONSTANTBUFFERHANDLEDATA *ShaderConstantBuffer )
{
	// シェーダー用定数バッファを削除する
	if( ShaderConstantBuffer->PF->D3D11.ConstBuffer != NULL )
	{
		Graphics_D3D11_ConstantBuffer_Delete( ShaderConstantBuffer->PF->D3D11.ConstBuffer ) ;
		ShaderConstantBuffer->PF->D3D11.ConstBuffer = NULL ;
	}

	// 正常終了
	return 0 ;
}

// シェーダー用定数バッファハンドルの定数バッファのアドレスを取得する
extern	void *	Graphics_Hardware_D3D11_ShaderConstantBuffer_GetBuffer_PF( SHADERCONSTANTBUFFERHANDLEDATA *ShaderConstantBuffer )
{
	return ShaderConstantBuffer->PF->D3D11.ConstBuffer->SysmemBuffer ;
}

// シェーダー用定数バッファハンドルの定数バッファへの変更を適用する
extern	int		Graphics_Hardware_D3D11_ShaderConstantBuffer_Update_PF( SHADERCONSTANTBUFFERHANDLEDATA *ShaderConstantBuffer )
{
	ShaderConstantBuffer->PF->D3D11.ConstBuffer->ChangeFlag = TRUE ;
	Graphics_D3D11_ConstantBuffer_Update( ShaderConstantBuffer->PF->D3D11.ConstBuffer ) ;

	// 終了
	return 0 ;
}

// シェーダー用定数バッファハンドルの定数バッファを指定のシェーダーの指定のスロットにセットする
extern	int		Graphics_Hardware_D3D11_ShaderConstantBuffer_Set_PF( SHADERCONSTANTBUFFERHANDLEDATA *ShaderConstantBuffer, int TargetShader /* DX_SHADERTYPE_VERTEX など */, int Slot )
{
	switch( TargetShader )
	{
	case DX_SHADERTYPE_VERTEX :			// 頂点シェーダー
		Graphics_D3D11_ConstantBuffer_VSSet( ( UINT )Slot, 1, &ShaderConstantBuffer->PF->D3D11.ConstBuffer ) ;
		break ;

	case DX_SHADERTYPE_PIXEL :			// ピクセルシェーダー
		Graphics_D3D11_ConstantBuffer_PSSet( ( UINT )Slot, 1, &ShaderConstantBuffer->PF->D3D11.ConstBuffer ) ;
		break ;

	case DX_SHADERTYPE_GEOMETRY :		// ジオメトリシェーダー
		break ;

	case DX_SHADERTYPE_COMPUTE :		// コンピュートシェーダー
		break ;

	case DX_SHADERTYPE_DOMAIN :			// ドメインシェーダー
		break ;

	case DX_SHADERTYPE_HULL :			// ハルシェーダー
		break ;
	}

	// 終了
	return 0 ;
}





































// 環境依存描画関係

// 指定点から境界色があるところまで塗りつぶす
extern	int		Graphics_Hardware_D3D11_Paint_PF( int /*x*/, int /*y*/, unsigned int /*FillColor*/, ULONGLONG /*BoundaryColor*/ )
{
	// デフォルトの処理を行う
	return 2 ;
}


























#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_NON_DIRECT3D11

#endif // DX_NON_GRAPHICS


