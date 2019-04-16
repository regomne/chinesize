// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		描画処理プログラム( Direct3D9 )ヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXGRAPHICSD3D9_H__
#define __DXGRAPHICSD3D9_H__

#include "../DxCompileConfig.h"

#ifndef DX_NON_GRAPHICS

#ifndef DX_NON_DIRECT3D9

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

#define GD3D9								GraphicsHardDataDirect3D9

// デバイス列挙の最大数
#define DX_D3D9_MAX_DEVICE_LISTUP			(32)

// Graphics_D3D9_DrawPreparation 関数に渡すフラグ
#define DX_D3D9_DRAWPREP_TRANS				(0x00001)
#define DX_D3D9_DRAWPREP_VECTORINT			(0x00002)
#define DX_D3D9_DRAWPREP_GOURAUDSHADE		(0x00008)
#define DX_D3D9_DRAWPREP_PERSPECTIVE		(0x00010)
#define DX_D3D9_DRAWPREP_DIFFUSERGB			(0x00020)
#define DX_D3D9_DRAWPREP_DIFFUSEALPHA		(0x00040)
#define DX_D3D9_DRAWPREP_FOG				(0x00080)
#define DX_D3D9_DRAWPREP_NOBLENDSETTING		(0x00100)
#define DX_D3D9_DRAWPREP_LIGHTING			(0x00200)
#define DX_D3D9_DRAWPREP_SPECULAR			(0x00400)
#define DX_D3D9_DRAWPREP_3D					(0x00800)
#define DX_D3D9_DRAWPREP_TEXADDRESS			(0x01000)
#define DX_D3D9_DRAWPREP_NOTSHADERRESET		(0x02000)
#define DX_D3D9_DRAWPREP_CULLING			(0x04000)
#define DX_D3D9_DRAWPREP_TEXTURE			(0x08000)
#define DX_D3D9_DRAWPREP_TEXALPHACH			(0x10000)
#define DX_D3D9_DRAWPREP_TEXALPHATEST		(0x20000)
//#define DX_D3D9_DRAWPREP_EDGEFONT			(0x40000)

// シェーダー定数セットマスク
#define DX_SHADERCONSTANTSET_MASK_LIB		0x0001
#define DX_SHADERCONSTANTSET_MASK_LIB_SUB	0x0002
#define DX_SHADERCONSTANTSET_MASK_MV1		0x0004
#define DX_SHADERCONSTANTSET_MASK_USER		0x0008

// ピクセル単位ライティングタイプの頂点シェーダー識別コード作成用マクロ
#define D3D9_PIXELLIGHTING_VERTEXSHADER_SHADOWMAP( use )		( ( use )  * 3 * 2 * 4 )
#define D3D9_PIXELLIGHTING_VERTEXSHADER_SKINMESH( use )			( ( use )  * 2 * 4 )
#define D3D9_PIXELLIGHTING_VERTEXSHADER_BUMPMAP( use )			( ( use )  * 4 )
#define D3D9_PIXELLIGHTING_VERTEXSHADER_FOGMODE( mode )			( ( mode ) )

// ピクセル単位ライティングタイプの頂点シェーダー識別コードから各要素を取得するためのマクロ
#define D3D9_PIXELLIGHTING_VERTEXSHADER_GET_SHADOWMAP( index )	( ( index ) / ( 3 * 2 * 4 ) % 2 )
#define D3D9_PIXELLIGHTING_VERTEXSHADER_GET_SKINMESH( index )	( ( index ) / ( 2 * 4 ) % 3 )
#define D3D9_PIXELLIGHTING_VERTEXSHADER_GET_BUMPMAP( index )	( ( index ) / ( 4 ) % 2 )
#define D3D9_PIXELLIGHTING_VERTEXSHADER_GET_FOGMODE( index )	( ( index ) % 4 )

// ピクセル単位ライティングタイプのピクセルシェーダータイプ
#define D3D9_PIXELLIGHTING_PIXELSHADER_TYPE_NORMAL				0	
#define D3D9_PIXELLIGHTING_PIXELSHADER_TYPE_TOON				1

// ピクセル単位ライティングタイプのピクセルシェーダー識別コード作成用マクロ
#define D3D9_PIXELLIGHTING_PIXELSHADER_TYPE( type )				( ( type  ) * 2 * 5 * 2 * 3 * 2 * 2 * 2 * 2 * 84 * 2 )
#define D3D9_PIXELLIGHTING_PIXELSHADER_SHADOWMAP( use )			( ( use   ) * 5 * 2 * 3 * 2 * 2 * 2 * 2 * 84 * 2 )
#define D3D9_PIXELLIGHTING_PIXELSHADER_MULTITEX( type )			( ( type  ) * 2 * 3 * 2 * 2 * 2 * 2 * 84 * 2 )
#define D3D9_PIXELLIGHTING_PIXELSHADER_TOONTYPE( type )			( ( type  ) * 3 * 2 * 2 * 2 * 2 * 84 * 2 )
#define D3D9_PIXELLIGHTING_PIXELSHADER_TOONSPHEREOP( type )		( ( type  ) * 2 * 2 * 2 * 2 * 84 * 2 )
#define D3D9_PIXELLIGHTING_PIXELSHADER_TOONDIFBLDOP( type )		( ( type  ) * 2 * 2 * 2 * 84 * 2 )
#define D3D9_PIXELLIGHTING_PIXELSHADER_TOONSPCBLDOP( type )		( ( type  ) * 2 * 2 * 84 * 2 )
#define D3D9_PIXELLIGHTING_PIXELSHADER_SPECULARMAP( use )		( ( use   ) * 2 * 84 * 2 )
#define D3D9_PIXELLIGHTING_PIXELSHADER_BUMPMAP( use )			( ( use   ) * 84 * 2 )
#define D3D9_PIXELLIGHTING_PIXELSHADER_LIGHTINDEX( index )		( ( index ) * 2 )
#define D3D9_PIXELLIGHTING_PIXELSHADER_SPECULAR( use )			( ( use   ) )

// ピクセル単位ライティングタイプのピクセルシェーダー識別コードから各要素を取得するためのマクロ
#define D3D9_PIXELLIGHTING_PIXELSHADER_GET_TYPE( index )			( ( index ) / ( 2 * 5 * 2 * 3 * 2 * 2 * 2 * 2 * 84 * 2 ) % 2 )
#define D3D9_PIXELLIGHTING_PIXELSHADER_GET_SHADOWMAP( index )		( ( index ) / ( 5 * 2 * 3 * 2 * 2 * 2 * 2 * 84 * 2 ) % 2 )
#define D3D9_PIXELLIGHTING_PIXELSHADER_GET_MULTITEX( index )		( ( index ) / ( 2 * 3 * 2 * 2 * 2 * 2 * 84 * 2 ) % 5 )
#define D3D9_PIXELLIGHTING_PIXELSHADER_GET_TOONTYPE( index )		( ( index ) / ( 3 * 2 * 2 * 2 * 2 * 84 * 2 ) % 2 )
#define D3D9_PIXELLIGHTING_PIXELSHADER_GET_TOONSPHEREOP( index )	( ( index ) / ( 2 * 2 * 2 * 2 * 84 * 2 ) % 3 )
#define D3D9_PIXELLIGHTING_PIXELSHADER_GET_TOONDIFBLDOP( index )	( ( index ) / ( 2 * 2 * 2 * 84 * 2 ) % 2 )
#define D3D9_PIXELLIGHTING_PIXELSHADER_GET_TOONSPCBLDOP( index )	( ( index ) / ( 2 * 2 * 84 * 2 ) % 2 )
#define D3D9_PIXELLIGHTING_PIXELSHADER_GET_SPECULARMAP( index )		( ( index ) / ( 2 * 84 * 2 ) % 2 )
#define D3D9_PIXELLIGHTING_PIXELSHADER_GET_BUMPMAP( index )			( ( index ) / ( 84 * 2 ) % 2 )
#define D3D9_PIXELLIGHTING_PIXELSHADER_GET_LIGHTINDEX( index )		( ( index ) / ( 2 ) % 84 )
#define D3D9_PIXELLIGHTING_PIXELSHADER_GET_SPECULAR( index )		( ( index ) % 2 )

	// 頂点シェーダータイプ
#define D3D9_VERTEXSHADER_TYPE_NORMAL					0
#define D3D9_VERTEXSHADER_TYPE_DRAW_SHADOWMAP			1
#define D3D9_VERTEXSHADER_TYPE_TOON_OUTLINE				2

// 頂点シェーダー識別コード作成用マクロ
#define D3D9_VERTEXSHADER_TYPE( type )					( ( type  ) * 2 * 2 * 3 * 2 * 4 * 20 * 2 )
#define D3D9_VERTEXSHADER_SHADERMODEL( ver )			( ( ver   ) * 2 * 3 * 2 * 4 * 20 * 2 )
#define D3D9_VERTEXSHADER_SHADOWMAP( use )				( ( use   ) * 3 * 2 * 4 * 20 * 2 )
#define D3D9_VERTEXSHADER_MESHTYPE( type )				( ( type  ) * 2 * 4 * 20 * 2 )
#define D3D9_VERTEXSHADER_BUMPMAP( use )				( ( use   ) * 4 * 20 * 2 )
#define D3D9_VERTEXSHADER_FOGMODE( mode )				( ( mode  ) * 20 * 2 )
#define D3D9_VERTEXSHADER_LIGHTINDEX( index )			( ( index ) * 2 )
#define D3D9_VERTEXSHADER_SPECULAR( use )				( ( use   ) )

// 頂点シェーダー識別コードから各要素を取得するためのマクロ
#define D3D9_VERTEXSHADER_GET_TYPE( index )				( ( index ) / ( 2 * 2 * 3 * 2 * 4 * 20 * 2 ) % 3 )
#define D3D9_VERTEXSHADER_GET_SHADERMODEL( index )		( ( index ) / ( 2 * 3 * 2 * 4 * 20 * 2 ) % 2 )
#define D3D9_VERTEXSHADER_GET_SHADOWMAP( index )		( ( index ) / ( 3 * 2 * 4 * 20 * 2 ) % 2 )
#define D3D9_VERTEXSHADER_GET_MESHTYPE( index )			( ( index ) / ( 2 * 4 * 20 * 2 ) % 3 )
#define D3D9_VERTEXSHADER_GET_BUMPMAP( index )			( ( index ) / ( 4 * 20 * 2 ) % 2 )
#define D3D9_VERTEXSHADER_GET_FOGMODE( index )			( ( index ) / ( 20 * 2 ) % 4 )
#define D3D9_VERTEXSHADER_GET_LIGHTINDEX( index )		( ( index ) / ( 2 ) % 20 )
#define D3D9_VERTEXSHADER_GET_SPECULAR( index )			( ( index ) % 2 )

// ピクセルシェーダータイプ
#define D3D9_PIXELSHADER_TYPE_NORMAL					0
#define D3D9_PIXELSHADER_TYPE_DRAW_SHADOWMAP			1
#define D3D9_PIXELSHADER_TYPE_TOON_OUTLINE				2

// ピクセルシェーダー識別コード作成用マクロ
#define D3D9_PIXELSHADER_MATERIALTYPE( type )			( ( type  ) * 3 * 2 * 2 * 5 * 2 * 2 * 3 * 2 * 2 * 2 * 2 * 10 * 2 )
#define D3D9_PIXELSHADER_TYPE( type )					( ( type  ) * 2 * 2 * 5 * 2 * 2 * 3 * 2 * 2 * 2 * 2 * 10 * 2 )
#define D3D9_PIXELSHADER_SHADERMODEL( ver )				( ( ver   ) * 2 * 5 * 2 * 2 * 3 * 2 * 2 * 2 * 2 * 10 * 2 )
#define D3D9_PIXELSHADER_SHADOWMAP( use )				( ( use   ) * 5 * 2 * 2 * 3 * 2 * 2 * 2 * 2 * 10 * 2 )
#define D3D9_PIXELSHADER_MULTITEX( type )				( ( type  ) * 2 * 2 * 3 * 2 * 2 * 2 * 2 * 10 * 2 )
#define D3D9_PIXELSHADER_TOON( use )					( ( use   ) * 2 * 3 * 2 * 2 * 2 * 2 * 10 * 2 )
#define D3D9_PIXELSHADER_TOONTYPE( type )				( ( type  ) * 3 * 2 * 2 * 2 * 2 * 10 * 2 )
#define D3D9_PIXELSHADER_TOONSPHEREOP( type )			( ( type  ) * 2 * 2 * 2 * 2 * 10 * 2 )
#define D3D9_PIXELSHADER_TOONDIFBLDOP( type )			( ( type  ) * 2 * 2 * 2 * 10 * 2 )
#define D3D9_PIXELSHADER_TOONSPCBLDOP( type )			( ( type  ) * 2 * 2 * 10 * 2 )
#define D3D9_PIXELSHADER_SPECULARMAP( use )				( ( use   ) * 2 * 10 * 2 )
#define D3D9_PIXELSHADER_BUMPMAP( use )					( ( use   ) * 10 * 2 )
#define D3D9_PIXELSHADER_LIGHTINDEX( index )			( ( index ) * 2 )
#define D3D9_PIXELSHADER_SPECULAR( use )				( ( use ) )

// ピクセルシェーダー識別コードから各要素を取得するためのマクロ
#define D3D9_PIXELSHADER_GET_MATERIALTYPE( index )		( ( index ) / ( 3 * 2 * 2 * 5 * 2 * 2 * 3 * 2 * 2 * 2 * 2 * 10 * 2 ) % DX_MATERIAL_TYPE_NUM )
#define D3D9_PIXELSHADER_GET_TYPE( index )				( ( index ) / ( 2 * 2 * 5 * 2 * 2 * 3 * 2 * 2 * 2 * 2 * 10 * 2 ) % 3 )
#define D3D9_PIXELSHADER_GET_SHADERMODEL( index )		( ( index ) / ( 2 * 5 * 2 * 2 * 3 * 2 * 2 * 2 * 2 * 10 * 2 ) % 2 )
#define D3D9_PIXELSHADER_GET_SHADOWMAP( index )			( ( index ) / ( 5 * 2 * 2 * 3 * 2 * 2 * 2 * 2 * 10 * 2 ) % 2 )
#define D3D9_PIXELSHADER_GET_MULTITEX( index )			( ( index ) / ( 2 * 2 * 3 * 2 * 2 * 2 * 2 * 10 * 2 ) % 5 )
#define D3D9_PIXELSHADER_GET_TOON( index )				( ( index ) / ( 2 * 3 * 2 * 2 * 2 * 2 * 10 * 2 ) % 2 )
#define D3D9_PIXELSHADER_GET_TOONTYPE( index )			( ( index ) / ( 3 * 2 * 2 * 2 * 2 * 10 * 2 ) % 2 )
#define D3D9_PIXELSHADER_GET_TOONSPHEREOP( index )		( ( index ) / ( 2 * 2 * 2 * 2 * 10 * 2 ) % 3 )
#define D3D9_PIXELSHADER_GET_TOONDIFBLDOP( index )		( ( index ) / ( 2 * 2 * 2 * 10 * 2 ) % 2 )
#define D3D9_PIXELSHADER_GET_TOONSPCBLDOP( index )		( ( index ) / ( 2 * 2 * 10 * 2 ) % 2 )
#define D3D9_PIXELSHADER_GET_SPECULARMAP( index )		( ( index ) / ( 2 * 10 * 2 ) % 2 )
#define D3D9_PIXELSHADER_GET_BUMPMAP( index )			( ( index ) / ( 10 * 2 ) % 2 )
#define D3D9_PIXELSHADER_GET_LIGHTINDEX( index )		( ( index ) / ( 2 ) % 10 )
#define D3D9_PIXELSHADER_GET_SPECULAR( index )			( ( index ) % 2 )

// 頂点フォーマット
#define VERTEXFVF_2D_USER					( D_D3DFVF_XYZRHW | D_D3DFVF_DIFFUSE | D_D3DFVF_TEX1 )										// トランスフォーム済み頂点フォーマット
#define VERTEXFVF_NOTEX_2D					( D_D3DFVF_XYZRHW | D_D3DFVF_DIFFUSE )														// ライン、ボックス描画用頂点フォーマット
#define VERTEXFVF_2D						( D_D3DFVF_XYZRHW | D_D3DFVF_DIFFUSE | D_D3DFVF_TEX1 )										// トランスフォーム済み頂点フォーマット
#define VERTEXFVF_BLENDTEX_2D				( D_D3DFVF_XYZRHW | D_D3DFVF_DIFFUSE | D_D3DFVF_SPECULAR | D_D3DFVF_TEX2 )					// αブレンドテクスチャ付きトランスフォーム済み頂点フォーマット
#define VERTEXFVF_SHADER_2D					( D_D3DFVF_XYZRHW | D_D3DFVF_DIFFUSE | D_D3DFVF_SPECULAR | D_D3DFVF_TEX2 )					// シェーダー描画用頂点フォーマット
#define VERTEXFVF_NOTEX_3D					( D_D3DFVF_XYZ | D_D3DFVF_DIFFUSE /*| D_D3DFVF_SPECULAR*/ )									// ライン、ボックス描画用頂点フォーマット、３Ｄ用( 旧バージョン用 )
#define VERTEXFVF_3D						( D_D3DFVF_XYZ | D_D3DFVF_DIFFUSE /*| D_D3DFVF_SPECULAR*/ | D_D3DFVF_TEX1 )					// グラフィックス描画用頂点フォーマット、３Ｄ用( 旧バージョン用 )
#define VERTEXFVF_3D_LIGHT					( D_D3DFVF_XYZ | D_D3DFVF_NORMAL | D_D3DFVF_DIFFUSE | D_D3DFVF_SPECULAR | D_D3DFVF_TEX2 )	// グラフィックス描画用頂点フォーマット
#define VERTEXFVF_SHADER_3D					( D_D3DFVF_XYZ | D_D3DFVF_NORMAL | D_D3DFVF_DIFFUSE | D_D3DFVF_SPECULAR | D_D3DFVF_TEX2 )	// シェーダー描画用頂点フォーマット
#define VERTEXFVF_TEX8_2D					( D_D3DFVF_XYZRHW | D_D3DFVF_DIFFUSE | D_D3DFVF_TEX8 )										// トランスフォーム済み頂点フォーマット

#define VERTEXFVF_DECL_2D_USER				(0)			// トランスフォーム済み頂点フォーマット
#define VERTEXFVF_DECL_NOTEX_2D				(1)			// ライン、ボックス描画用頂点フォーマット
#define VERTEXFVF_DECL_2D					(2)			// トランスフォーム済み頂点フォーマット
#define VERTEXFVF_DECL_BLENDTEX_2D			(3)			// αブレンドテクスチャ付きトランスフォーム済み頂点フォーマット
#define VERTEXFVF_DECL_SHADER_2D			(4)			// シェーダー描画用頂点フォーマット
#define VERTEXFVF_DECL_NOTEX_3D				(5)			// ライン、ボックス描画用頂点フォーマット、３Ｄ用( 旧バージョン用 )
#define VERTEXFVF_DECL_3D					(6)			// グラフィックス描画用頂点フォーマット、３Ｄ用( 旧バージョン用 )
#define VERTEXFVF_DECL_3D_LIGHT				(7)			// グラフィックス描画用頂点フォーマット
#define VERTEXFVF_DECL_SHADER_3D			(8)			// シェーダー描画用頂点フォーマット
#define VERTEXFVF_DECL_TEX8_2D				(9)			// バイキュービック補間用頂点フォーマット
#define VERTEXFVF_DECL_NUM					(10)

// 頂点バッファのサイズ
#define D3DDEV_NOTEX_VERTBUFFERSIZE			(63 * 1024)		// 63KB
#define D3DDEV_TEX_VERTBUFFERSIZE			(64 * 1024)		// 64KB
#define D3DDEV_BLENDTEX_VERTBUFFERSIZE		(64 * 1024)		// 64KB
#define D3DDEV_NOTEX_3D_VERTBUFFERSIZE		(64 * 1024)		// 64KB
#define D3DDEV_TEX_3D_VERTBUFFERSIZE		(64 * 1024)		// 64KB
#define D3DDEV_NOTEX_VERTMAXNUM				(D3DDEV_NOTEX_VERTBUFFERSIZE     / sizeof( VERTEX_NOTEX_2D    ))
#define D3DDEV_TEX_VERTMAXNUM				(D3DDEV_TEX_VERTBUFFERSIZE       / sizeof( VERTEX_2D          ))
#define D3DDEV_BLENDTEX_VERTMAXNUM			(D3DDEV_BLENDTEX_VERTBUFFERSIZE  / sizeof( VERTEX_BLENDTEX_2D ))
#define D3DDEV_NOTEX_3D_VERTMAXNUM			(D3DDEV_NOTEX_3D_VERTBUFFERSIZE  / sizeof( VERTEX_NOTEX_3D    ))
#define D3DDEV_TEX_3D_VERTMAXNUM			(D3DDEV_TEX_3D_VERTBUFFERSIZE    / sizeof( VERTEX_3D          ))

// プリミティブの数を算出
#define GETPRIMNUM( primtype, vertnum )	\
	( DWORD )( \
	( ( (primtype) == D_D3DPT_TRIANGLELIST  ) ? (vertnum) / 3 :\
    ( ( (primtype) == D_D3DPT_LINELIST      ) ? (vertnum) / 2 :\
	( ( (primtype) == D_D3DPT_TRIANGLEFAN   ) ? (vertnum) - 2 :\
	( ( (primtype) == D_D3DPT_TRIANGLESTRIP ) ? (vertnum) - 2 :\
	( ( (primtype) == D_D3DPT_LINESTRIP     ) ? (vertnum) - 1 :\
	( ( (primtype) == D_D3DPT_POINTLIST     ) ? (vertnum)     : 0 ) ) ) ) ) ) )


// 非管理テクスチャへのデータ転送用システムメモリ配置テクスチャの数
#define DIRECT3D9_SYSMEMTEXTURE_NUM					(2048)

// 非管理テクスチャへのデータ転送用システムメモリ配置サーフェスの数
#define DIRECT3D9_SYSMEMSURFACE_NUM					(2048)


// 構造体定義 --------------------------------------------------------------------

// Direct3D9用 ハードウエアレンダリング用オリジナル画像テクスチャ情報
struct IMAGEDATA_ORIG_HARD_TEX_DIRECT3D9
{
	union
	{
		D_IDirect3DCubeTexture9	*	CubeTexture ;							// キューブテクスチャ
		D_IDirect3DTexture9		*	Texture ;								// テクスチャ
	} ;
//	D_IDirect3DTexture9	*			MemTexture ;							// D3DPOOL_MANAGED が使用できない画像用メモリテクスチャ
	D_IDirect3DSurface9	*			Surface[ CUBEMAP_SURFACE_NUM ] ;		// サーフェス
	D_IDirect3DSurface9	*			RenderTargetSurface ;					// レンダリングターゲットサーフェス
	D_IDirect3DSurface9	*			ZBuffer ;								// Ｚバッファ
} ;

// Direct3D9用 シャドウマップ情報
struct SHADOWMAPDATA_DIRECT3D9
{
	D_IDirect3DTexture9		*Texture ;										// テクスチャ

	D_IDirect3DSurface9		*Surface ;										// サーフェス
	D_IDirect3DSurface9		*ZBufferSurface ;								// Ｚバッファサーフェス
} ;

// Direct3D9用 頂点バッファハンドル情報
struct VERTEXBUFFERHANDLEDATA_DIRECT3D9
{
	D_IDirect3DVertexBuffer9	*VertexBuffer ;								// IDirect3DVertexBuffer9 のポインタ
} ;

// Direct3D9用 インデックスバッファハンドル情報
struct INDEXBUFFERHANDLEDATA_DIRECT3D9
{
	D_IDirect3DIndexBuffer9		*IndexBuffer ;								// IDirect3DIndexBuffer9 のポインタ
} ;

// Direct3D9用 シェーダーハンドル情報
struct SHADERHANDLEDATA_DIRECT3D9
{
	union
	{
		D_IDirect3DPixelShader9			*PixelShader ;						// IDirect3DPixelShader9 のポインタ
		D_IDirect3DVertexShader9		*VertexShader ;						// IDirect3DVertexShader9 のポインタ
	} ;

	int									ConstantNum ;						// 定数の数
	D_D3DXSHADER_CONSTANTINFO			*ConstantInfo ;						// 定数情報配列の先頭アドレス
} ;

// Direct3D9用 シェーダー用定数バッファハンドル情報
struct SHADERCONSTANTBUFFERHANDLEDATA_DIRECT3D9
{
	int			Dummy ;
} ;

// シェーダー定数の一つの使用領域情報構造体
struct DIRECT3D9_SHADERCONST_ONE_USEAREA
{
	WORD								Start ;								// 使用領域の開始番号
	WORD								EndPlusOne ;						// 使用領域の終了番号＋１
} ;

// シェーダー定数使用領域情報構造体
struct DIRECT3D9_SHADERCONSTANT_USEAREA
{
	DIRECT3D9_SHADERCONST_ONE_USEAREA	AreaInfo[ 256 ] ;					// 使用領域情報
	int									AreaInfoNum ;						// 使用領域情報の数
	int									TotalSize ;							// 管理する領域のサイズ
} ;

// シェーダー定数情報構造体
struct DIRECT3D9_SHADERCONSTANTINFO
{
	DIRECT3D9_SHADERCONSTANT_USEAREA	UseArea ;							// ユーザー用頂点シェーダー定数の使用領域情報
	int									UnitSize ;							// データ一つあたりのサイズ

	union
	{
		BYTE							Data[ 256 * 16 ] ;					// サイズ保証用データ

		FLOAT4							Float4[ 256 ] ;						// ユーザー用頂点シェーダー float 型定数
		INT4							Int4[ 16 ] ;						// ユーザー用頂点シェーダー int 型定数
		BOOL							Bool[ 16 ] ;						// ユーザー用頂点シェーダー BOOL 型定数
	} ;
} ;

// シェーダー定数情報セット構造体
struct DIRECT3D9_SHADERCONSTANTINFOSET
{
	int									IsApply[ DX_SHADERCONSTANTSET_NUM ] ;			// 定数情報を適用するかどうか( TRUE:適用する  FALSE:適用しない )
	int									ApplyMask ;										// 適用マスク( DX_SHADERCONSTANTSET_MASK_LIB | DX_SHADERCONSTANTSET_MASK_LIB_SUB 等 )
	DIRECT3D9_SHADERCONSTANTINFO		Info[ DX_SHADERCONSTANTTYPE_NUM ][ DX_SHADERCONSTANTSET_NUM ] ;				// 定数情報実体
	BYTE								UseMap[ DX_SHADERCONSTANTTYPE_NUM ][ DX_SHADERCONSTANTSET_NUM ][ 256 ] ;	// 定数使用マップ

	DIRECT3D9_SHADERCONSTANTINFO		FixInfo[ DX_SHADERCONSTANTTYPE_NUM ] ;			// 実際にシェーダーに適用されている定数情報

	BYTE								SetMap[ DX_SHADERCONSTANTTYPE_NUM ][ 256 ] ;	// シェーダー定数の使用中セットマップ( DX_SHADERCONSTANTSET_LIB 等、0xff 未使用 )
} ;


// Direct3D9 の標準描画用のシェーダーを纏めた構造体
struct GRAPHICS_HARDWARE_DIRECT3D9_SHADER_BASE
{
#ifndef DX_NON_NORMAL_DRAW_SHADER
	// 固定機能パイプライン互換用の頂点シェーダー
	D_IDirect3DVertexShader9				*Base2DVertexShader ;
//	D_IDirect3DVertexShader9				*Base3DVertexShader ;

	// 固定機能パイプライン互換のシェーダー( テクスチャなし )
	// [ 特殊効果( 0:通常描画  1:乗算描画  2:RGB反転  3:描画輝度４倍  4:乗算済みαブレンドモードの通常描画  5:乗算済みαブレンドモードのRGB反転  6:乗算済みαブレンドモードの描画輝度4倍 ) ]
	// [ αチャンネル考慮版かどうか( 0:考慮しない  1:考慮する ) ]
	// [ 乗算済みαブレンド用かどうか( 0:乗算済みαブレンド用ではない  1:乗算済みαブレンド用 ) ]
	D_IDirect3DPixelShader9					*BaseNoneTexPixelShader[ 7 ][ 2 ] ;

	// 固定機能パイプライン互換のシェーダー( テクスチャあり )
	// [ ブレンド画像とのブレンドタイプ( 0:なし  1:DX_BLENDGRAPHTYPE_NORMAL  2:DX_BLENDGRAPHTYPE_WIPE  3:DX_BLENDGRAPHTYPE_ALPHA ) ]
	// [ 特殊効果( 0:通常描画  1:乗算描画  2:RGB反転  3:描画輝度４倍  4:乗算済みαブレンドモードの通常描画  5:乗算済みαブレンドモードのRGB反転  6:乗算済みαブレンドモードの描画輝度4倍 ) ]
	// [ テクスチャRGB無視( 0:無視しない  1:無視する ) ]
	// [ テクスチャAlpha無視( 0:無視しない  1:無視する ) ]
	// [ αチャンネル考慮版かどうか( 0:考慮しない  1:考慮する ) ]
	D_IDirect3DPixelShader9					*BaseUseTexPixelShader[ 4 ][ 7 ][ 2 ][ 2 ][ 2 ] ;

	// マスク処理用のシェーダー
	D_IDirect3DPixelShader9					*MaskEffectPixelShader ;
	D_IDirect3DPixelShader9					*MaskEffect_UseGraphHandle_PixelShader[ 4 ] ;
	D_IDirect3DPixelShader9					*MaskEffect_UseGraphHandle_ReverseEffect_PixelShader[ 4 ] ;
#endif // DX_NON_NORMAL_DRAW_SHADER
} ;

// Direct3D9 の標準３Ｄ描画用のシェーダーを纏めた構造体
struct GRAPHICS_HARDWARE_DIRECT3D9_SHADER_BASE3D
{
	// モデル描画用ピクセル単位ライティングタイプの頂点シェーダー( リストは配列の左から )
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// /* [ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ 2:8ボーン内スキニングメッシュ ) ] */
	// /* [ バンプマップの有無( 0:無し 1:有り ) ] */
	// [ フォグタイプ ]
	D_IDirect3DVertexShader9				*Base3D_PixelLighting_VS[ 2 ]/*[ 3 ]*//*[ 2 ]*/[ 4 ] ;

	// モデル描画用ピクセル単位ライティングタイプのピクセルシェーダー( リストは配列の左から )
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// /*[ マルチテクスチャのブレンドモード、MV1_LAYERBLEND_TYPE_TRANSLUCENT などに +1 したもの ( 0:マルチテクスチャ無し  1:αブレンド  2:加算ブレンド  3:乗算ブレンド  4:乗算ブレンド×2 ) ]*/
	// /*[ スペキュラマップ ( 0:無い 1:ある ) ]*/
	// /*[ バンプマップ ( 0:なし 1:あり ) ]*/
	// [ ライトインデックス ]
	// [ スペキュラの有無( 0:無し 1:有り ) ]
	// [ 特殊効果( 0:通常描画  1:乗算描画  2:RGB反転  3:描画輝度４倍  4:乗算済みαブレンドモードの通常描画  5:乗算済みαブレンドモードのRGB反転  6:乗算済みαブレンドモードの描画輝度4倍 ) ]
	D_IDirect3DPixelShader9					*Base3D_PixelLighting_Normal_PS[ 2 ]/*[ 5 ]*//*[ 2 ]*//*[ 2 ]*/[ 84 ][ 2 ][ 7 ] ;






	// モデル描画用のシャドウマップへのレンダリング用頂点シェーダー
	// /*[ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ  2:8ボーン内スキニングメッシュ ) ]*/
	// [ バンプマップの有無( 0:無し 1:有り ) ]
	D_IDirect3DVertexShader9				*Base3D_ShadowMap_VS/*[ 3 ]*/[ 2 ] ;

	// モデル描画用のライティングなし頂点シェーダー
	// /*[ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ  2:8ボーン内スキニングメッシュ ) ]*/
	// [ フォグタイプ ]
	D_IDirect3DVertexShader9				*Base3D_NoLighting_VS/*[ 3 ]*/[ 4 ] ;

	// モデル描画用のライティングあり頂点シェーダー
	// [ シェーダーモデル( 0:SM2  1:SM3 )
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// /*[ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ  2:8ボーン内スキニングメッシュ ) ]*/
	// /*[ バンプマップの有無( 0:無し 1:有り ) ]*/
	// [ フォグタイプ ]
	// [ ライトインデックス ]
	// [ スペキュラの有無( 0:無し 1:有り ) ]
	D_IDirect3DVertexShader9				*Base3D_VertexLighting_VS[ 2 ][ 2 ]/*[ 3 ]*//*[ 2 ]*/[ 4 ][ 20 ][ 2 ] ;


	// モデル描画用のシャドウマップへのレンダリング用ピクセルシェーダー
	D_IDirect3DPixelShader9					*Base3D_ShadowMap_Normal_PS ;

	// モデル描画用のライティングなしピクセルシェーダー
	// /*[ マルチテクスチャのブレンドモード、MV1_LAYERBLEND_TYPE_TRANSLUCENT などに +1 したもの ( 0:マルチテクスチャ無し  1:αブレンド  2:加算ブレンド  3:乗算ブレンド  4:乗算ブレンド×2 ) ]*/
	// [ 特殊効果( 0:通常描画  1:乗算描画  2:RGB反転  3:描画輝度４倍  4:乗算済みαブレンドモードの通常描画  5:乗算済みαブレンドモードのRGB反転  6:乗算済みαブレンドモードの描画輝度4倍 ) ]
	D_IDirect3DPixelShader9					*Base3D_NoLighting_Normal_PS/*[ 5 ]*/[ 7 ] ;

	// モデル描画用のライティングありピクセルシェーダー
	// [ シェーダーモデル( 0:SM2  1:SM3 )
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// /* [ マルチテクスチャのブレンドモード、MV1_LAYERBLEND_TYPE_TRANSLUCENT などに +1 したもの ( 0:マルチテクスチャ無し  1:αブレンド  2:加算ブレンド  3:乗算ブレンド  4:乗算ブレンド×2 ) ] */
	// /* [ スペキュラマップ ( 0:無い 1:ある ) ] */
	// /* [ バンプマップ ( 0:なし 1:あり ) ] */
	// [ ライトインデックス ]
	// [ スペキュラの有無( 0:無し 1:有り ) ]
	// [ 特殊効果( 0:通常描画  1:乗算描画  2:RGB反転  3:描画輝度４倍  4:乗算済みαブレンドモードの通常描画  5:乗算済みαブレンドモードのRGB反転  6:乗算済みαブレンドモードの描画輝度4倍 ) ]
	D_IDirect3DPixelShader9					*Base3D_VertexLighting_Normal_PS[ 2 ][ 2 ]/*[ 5 ]*//*[ 2 ]*//*[ 2 ]*/[ 10 ][ 2 ][ 7 ] ;
} ;

#ifndef DX_NON_MODEL

// Direct3D9 のモデル描画用のシェーダーを纏めた構造体
struct GRAPHICS_HARDWARE_DIRECT3D9_SHADER_MODEL
{
	// モデル描画用ピクセル単位ライティングタイプの頂点シェーダー( リストは配列の左から )
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// [ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ 2:8ボーン内スキニングメッシュ ) ]
	// [ バンプマップの有無( 0:無し 1:有り ) ]
	// [ フォグタイプ ]
	D_IDirect3DVertexShader9				*MV1_PixelLighting_VS[ 2 ][ 3 ][ 2 ][ 4 ] ;

	// モデル描画用ピクセル単位ライティングタイプのトゥーン用ピクセルシェーダー( リストは配列の左から )
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// [ トゥーンレンダリングタイプ( 0:トゥーンレンダリング type 1   1:トゥーンレンダリング Type 2 ) ]
	// [ トゥーンレンダリングのスフィアマップの有無とブレンドタイプ( 0:スフィアマップは無い   1:MV1_LAYERBLEND_TYPE_MODULATE  2:MV1_LAYERBLEND_TYPE_ADDITIVE ) ]
	// [ トゥーンレンダリングのディフューズグラデーションのブレンドタイプ( 0:MV1_LAYERBLEND_TYPE_TRANSLUCENT  1:MV1_LAYERBLEND_TYPE_MODULATE ) ]
	// [ トゥーンレンダリングのスペキュラグラデーションのブレンドタイプ( 0:MV1_LAYERBLEND_TYPE_TRANSLUCENT  1:MV1_LAYERBLEND_TYPE_ADDITIVE ) ]
	// [ スペキュラマップ ( 0:無い 1:ある ) ]
	// [ バンプマップ ( 0:なし 1:あり ) ]
	// [ ライトインデックス ]
	// [ スペキュラの有無( 0:無し 1:有り ) ]
	D_IDirect3DPixelShader9					*MV1_PixelLighting_Toon_PS[ 2 ][ 2 ][ 3 ][ 2 ][ 2 ][ 2 ][ 2 ][ 84 ][ 2 ] ;

	// モデル描画用ピクセル単位ライティングタイプのピクセルシェーダー( リストは配列の左から )
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// [ マルチテクスチャのブレンドモード、MV1_LAYERBLEND_TYPE_TRANSLUCENT などに +1 したもの ( 0:マルチテクスチャ無し  1:αブレンド  2:加算ブレンド  3:乗算ブレンド  4:乗算ブレンド×2 ) ]
	// [ スペキュラマップ ( 0:無い 1:ある ) ]
	// [ バンプマップ ( 0:なし 1:あり ) ]
	// [ ライトインデックス ]
	// [ スペキュラの有無( 0:無し 1:有り ) ]
	D_IDirect3DPixelShader9					*MV1_PixelLighting_Normal_PS[ 2 ][ 5 ][ 2 ][ 2 ][ 84 ][ 2 ] ;





	// モデル描画用のマテリアル要素描画用ピクセルシェーダー
	// [ マテリアルタイプ( DX_MATERIAL_TYPE_MAT_SPEC_LUMINANCE_UNORM など ) ]
	D_IDirect3DPixelShader9					*MV1_MaterialType_PS[ DX_MATERIAL_TYPE_NUM ] ;





	// モデル描画用のトゥーンレンダリングの輪郭線描画用頂点シェーダー
	// [ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ  2:8ボーン内スキニングメッシュ ) ]
	// [ フォグタイプ ]
	D_IDirect3DVertexShader9				*MV1_ToonOutLine_VS[ 3 ][ 4 ] ;

	// モデル描画用のシャドウマップへのレンダリング用頂点シェーダー
	// [ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ  2:8ボーン内スキニングメッシュ ) ]
	D_IDirect3DVertexShader9				*MV1_ShadowMap_VS[ 3 ] ;

	// モデル描画用のライティングなし頂点シェーダー
	// [ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ  2:8ボーン内スキニングメッシュ ) ]
	// [ フォグタイプ ]
	D_IDirect3DVertexShader9				*MV1_NoLighting_VS[ 3 ][ 4 ] ;

	// モデル描画用のライティングあり頂点シェーダー
	// [ シェーダーモデル( 0:SM2  1:SM3 )
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// [ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ  2:8ボーン内スキニングメッシュ ) ]
	// [ バンプマップの有無( 0:無し 1:有り ) ]
	// [ フォグタイプ ]
	// [ ライトインデックス ]
	// [ スペキュラの有無( 0:無し 1:有り ) ]
	D_IDirect3DVertexShader9				*MV1_VertexLighting_VS[ 2 ][ 2 ][ 3 ][ 2 ][ 4 ][ 20 ][ 2 ] ;


	// モデル描画用のトゥーンレンダリングの輪郭線描画用ピクセルシェーダー
	D_IDirect3DPixelShader9					*MV1_ToonOutLine_PS ;

	// モデル描画用のシャドウマップへのトゥーンレンダリング用ピクセルシェーダー
	// [ トゥーンレンダリングタイプ( 0:トゥーンレンダリング type 1   1:トゥーンレンダリング type 2 ) ]
	// [ トゥーンレンダリングのスフィアマップの有無とブレンドタイプ( 0:スフィアマップは無い   1:MV1_LAYERBLEND_TYPE_MODULATE  2:MV1_LAYERBLEND_TYPE_ADDITIVE ) ]
	// [ トゥーンレンダリングのディフューズグラデーションのブレンドタイプ( 0:MV1_LAYERBLEND_TYPE_TRANSLUCENT  1:MV1_LAYERBLEND_TYPE_MODULATE ) ]
	D_IDirect3DPixelShader9					*MV1_ShadowMap_Toon_PS[ 2 ][ 3 ][ 2 ] ;

	// モデル描画用のシャドウマップへのレンダリング用ピクセルシェーダー
	D_IDirect3DPixelShader9					*MV1_ShadowMap_Normal_PS ;

	// モデル描画用のライティングなしトゥーン用ピクセルシェーダー( リストは配列の左から )
	// [ トゥーンレンダリングタイプ( 0:トゥーンレンダリング type 1   1:トゥーンレンダリング type 2 ) ]
	// [ トゥーンレンダリングのディフューズグラデーションのブレンドタイプ( 0:MV1_LAYERBLEND_TYPE_TRANSLUCENT  1:MV1_LAYERBLEND_TYPE_MODULATE ) ]
	D_IDirect3DPixelShader9					*MV1_NoLighting_Toon_PS[ 2 ][ 2 ] ;

	// モデル描画用のライティングなしピクセルシェーダー
	// [ マルチテクスチャのブレンドモード、MV1_LAYERBLEND_TYPE_TRANSLUCENT などに +1 したもの ( 0:マルチテクスチャ無し  1:αブレンド  2:加算ブレンド  3:乗算ブレンド  4:乗算ブレンド×2 ) ]
	D_IDirect3DPixelShader9					*MV1_NoLighting_Normal_PS[ 5 ] ;

	// モデル描画用のライティングありトゥーン用ピクセルシェーダー
	// [ シェーダーモデル( 0:SM2  1:SM3 )
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// [ トゥーンレンダリングタイプ( 0:トゥーンレンダリング type 1   1:トゥーンレンダリング type 2 ) ]
	// [ トゥーンレンダリングのスフィアマップの有無とブレンドタイプ( 0:スフィアマップは無い   1:MV1_LAYERBLEND_TYPE_MODULATE  2:MV1_LAYERBLEND_TYPE_ADDITIVE ) ]
	// [ トゥーンレンダリングのディフューズグラデーションのブレンドタイプ( 0:MV1_LAYERBLEND_TYPE_TRANSLUCENT  1:MV1_LAYERBLEND_TYPE_MODULATE ) ]
	// [ トゥーンレンダリングのスペキュラグラデーションのブレンドタイプ( 0:MV1_LAYERBLEND_TYPE_TRANSLUCENT  1:MV1_LAYERBLEND_TYPE_ADDITIVE ) ]
	// [ スペキュラマップ ( 0:無い 1:ある ) ]
	// [ バンプマップ ( 0:なし 1:あり ) ]
	// [ ライトインデックス ]
	// [ スペキュラの有無( 0:無し 1:有り ) ]
	D_IDirect3DPixelShader9					*MV1_VertexLighting_Toon_PS[ 2 ][ 2 ][ 2 ][ 3 ][ 2 ][ 2 ][ 2 ][ 2 ][ 10 ][ 2 ] ;

	// モデル描画用のライティングありピクセルシェーダー
	// [ シェーダーモデル( 0:SM2  1:SM3 )
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// [ マルチテクスチャのブレンドモード、MV1_LAYERBLEND_TYPE_TRANSLUCENT などに +1 したもの ( 0:マルチテクスチャ無し  1:αブレンド  2:加算ブレンド  3:乗算ブレンド  4:乗算ブレンド×2 ) ]
	// [ スペキュラマップ ( 0:無い 1:ある ) ]
	// [ バンプマップ ( 0:なし 1:あり ) ]
	// [ ライトインデックス ]
	// [ スペキュラの有無( 0:無し 1:有り ) ]
	D_IDirect3DPixelShader9					*MV1_VertexLighting_Normal_PS[ 2 ][ 2 ][ 5 ][ 2 ][ 2 ][ 10 ][ 2 ] ;
} ;
#endif // DX_NON_MODEL


// Direct3D9 のシェーダー情報を纏めた構造体
struct GRAPHICS_HARDWARE_DIRECT3D9_SHADER
{
//	int											UseShader ;						// プログラマブルシェーダーを使用するかどうか( TRUE:使用する  FALSE:使用しない )

	DWORD										NativeVertexShaderVersion ;		// エミュレーション無しの頂点シェーダーのバージョン

	int											ValidVertexShader ;				// 頂点シェーダーが使用可能かどうか( TRUE:使用可能  FALSE:不可能 )
	int											ValidPixelShader ;				// ピクセルシェーダーが使用可能かどうか( TRUE:使用可能  FALSE:不可能 )
	int											ValidVertexShader_SM3 ;			// Shader Model 3.0 の頂点シェーダーが使用可能かどうか( TRUE:使用可能  FALSE:不可能 )
	int											ValidPixelShader_SM3 ;			// Shader Model 3.0 のピクセルシェーダーが使用可能かどうか( TRUE:使用可能  FALSE:不可能 )

	DIRECT3D9_SHADERCONSTANTINFOSET				ShaderConstantInfo ;			// シェーダーの定数情報

	GRAPHICS_HARDWARE_DIRECT3D9_SHADER_BASE		Base ;							// 標準描画処理用のシェーダーを纏めた構造体
	GRAPHICS_HARDWARE_DIRECT3D9_SHADER_BASE3D	Base3D ;						// 標準３Ｄ描画処理用のシェーダーを纏めた構造体

#ifndef DX_NON_MODEL
	int											UseBaseVertexShaderIndex ;		// 使用する頂点シェーダーのライト・フォグ・フォンシェーディングの有無のみ設定した値
	int											UseBasePixelShaderIndex ;		// 使用するピクセルシェーダーのライト・フォンシェーディングの有無のみ設定した値
	int											UseBaseVertexShaderIndex_PL ;	// ピクセル単位ライティングタイプで使用する頂点シェーダーのライト・フォグ・フォンシェーディングの有無のみ設定した値
	int											UseBasePixelShaderIndex_PL ;	// ピクセル単位ライティングタイプで使用するピクセルシェーダーのライト・フォンシェーディングの有無のみ設定した値
	int											UseOnlyPixelLightingTypeCode ;	// ピクセル単位ライティングタイプのシェーダーコードを使用する指定になっているかどうか

	GRAPHICS_HARDWARE_DIRECT3D9_SHADER_MODEL	Model ;							// モデル描画処理用のシェーダーを纏めた構造体
#endif // DX_NON_MODEL
} ;



// Direct3D9 の頂点シェーダ宣言を纏めた構造体
struct GRAPHICS_HARDWARE_DIRECT3D9_VERTEXDECLARATION
{
	// 固定パイプライン互換シェーダーで使用する頂点データフォーマット
	D_IDirect3DVertexDeclaration9 *				Base2DDeclaration[ VERTEXFVF_DECL_NUM ] ;

	// ユーザーシェーダモードで使用する頂点データフォーマット
	D_IDirect3DVertexDeclaration9 *				UserShaderDeclaration[ DX_VERTEX_TYPE_NUM ] ;

#ifndef DX_NON_MODEL
	// 頂点シェーダ宣言( リストは配列の左から )
	// [ バンプマップ情報付きかどうか( 1:バンプマップ付き 0:付いてない ) ]
	// [ スキニングメッシュかどうか( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ 2:8ボーン内スキニングメッシュ ) ]
	// [ UVの数 ]
	D_IDirect3DVertexDeclaration9 *				MV1_VertexDeclaration[ 2 ][ 3 ][ 9 ] ;
#endif // DX_NON_MODEL
} ;



// テクスチャステージの情報
struct GRAPHICS_HARDDATA_DIRECT3D9_TEXTURESTAGEINFO
{
	void *						Texture ;							// ステージにセットされているテクスチャサーフェス
	int							TextureCoordIndex ;					// 使用するテクスチャ座標インデックス
	int							ResultTempARG ;						// 出力先レジスタをテンポラリにするかどうか
	short						AlphaARG1, AlphaARG2, AlphaOP ;		// Direct3DDevice の D3DTSS_ALPHAOP と D3DTSS_ALPHAARG1 と D3DTSS_ALPHAARG2 の設定値
	short						ColorARG1, ColorARG2, ColorOP ;		// Direct3DDevice の D3DTSS_COLOROP と D3DTSS_COLORARG1 と D3DTSS_COLORARG2 の設定値
} ;

// Ｄｉｒｅｃｔ３Ｄ のブレンド設定に関る設定の情報
struct GRAPHICS_HARDDATA_DIRECT3D9_BLENDINFO
{
	int							AlphaTestEnable ;							// Direct3DDevice9 の D3DRS_ALPHATESTENABLE の設定値
	int							AlphaRef ;									// Direct3DDevice9 の D3DRS_ALPHAREF の設定値
	int							AlphaFunc ;									// Direct3DDevice9 の D3DRS_ALPHAFUNC の設定値
	int							AlphaBlendEnable ;							// Direct3DDevice9 の D3DRS_ALPHABLENDENABLE の設定値
	int							SeparateAlphaBlendEnable ;					// Direct3DDevice9 の D3DRS_SEPARATEALPHABLENDENABLE の設定値

	DWORD						FactorColor ;								// Direct3DDevice9 の D3DRS_TEXTUREFACTOR の設定値
	int							SrcBlend, DestBlend ;						// Direct3DDevice9 の D3DRS_DESTBLEND と D3DRS_SRCBLEND の設定値
	int							BlendOp ;									// Direct3DDevice9 の D3DRS_BLENDOP の設定値
	int							SrcBlendAlpha, DestBlendAlpha ;				// Direct3DDevice9 の D3DRS_DESTBLENDALPHA と D3DRS_SRCBLENDALPHA の設定値
	int							BlendOpAlpha ;								// Direct3DDevice9 の D3DRS_BLENDOPALPHA の設定値
	int							UseTextureStageNum ;						// 使用しているテクスチャステージの数
	GRAPHICS_HARDDATA_DIRECT3D9_TEXTURESTAGEINFO TextureStageInfo[ USE_TEXTURESTAGE_NUM] ;	// Direct3DDevice のテクスチャステージ設定値( Direct3DDevice にこれから適応する予定の設定値 )
} ;

// Direct3D9Device に設定している値を纏めた構造体
struct GRAPHICS_HARDDATA_DIRECT3D9_DEVICE_STATE
{
	GRAPHICS_HARDDATA_DIRECT3D9_BLENDINFO	BlendInfo ;										// ブレンド関係の設定情報
	int										CullMode ;										// カリングモード
	int										DepthBias ;										// 深度値バイアス
	int										DrawMode ;										// 描画モード
	float									FactorColorPSConstantF[ 4 ] ;					// ピクセルシェーダーを使ったＤＸライブラリ標準処理用の FactorColor の値
	float									IgnoreColorPSConstantF[ 4 ] ;					// テクスチャカラー無視処理用カラーの値
	int										FillMode ;										// フィルモード
	int										FogEnable ;										// フォグが有効かどうか( TRUE:有効  FALSE:無効 )
	int										FogMode ;										// フォグモード
	DWORD									FogColor ;										// フォグカラー
	float									FogStart ;										// フォグ開始距離
	float									FogEnd ;										// フォグ終了距離
	float									FogDensity ;									// フォグ密度
	COLOR_F									GlobalAmbientLightColor ;						// グローバルアンビエントライトカラー
	int										LightEnableMaxIndex ;							// 有効なライトの最大インデックス
	int										LightEnableFlag[ 256 ] ;						// ライトが有効かどうかフラグ( TRUE:有効  FALSE:無効 )
	D_D3DLIGHT9								LightParam[ 256 ] ;								// ライトのパラメータ
	int										Lighting ;										// ライトを使用するかフラグ
	D_D3DMATERIAL9							Material ;										// マテリアル
	float									MaterialTypeParam[ MATERIAL_TYPEPARAM_MAX_NUM ] ;	// マテリアル別タイプパラメータ
	int										MaterialUseVertexDiffuseColor ;					// 頂点のディフューズカラーをマテリアルのディフューズカラーとして使用するかどうか
	int										MaterialUseVertexSpecularColor ;				// 頂点のスペキュラカラーをマテリアルのスペキュラカラーとして使用するかどうか
	int										MaxAnisotropy ;									// 最大異方性
	int										MaxAnisotropyDim[ USE_TEXTURESTAGE_NUM ] ;		// 各サンプラの最大異方性
	D_D3DTEXTUREFILTERTYPE					MagFilter[ USE_TEXTURESTAGE_NUM ] ;				// 各サンプラの拡大フィルタ
	D_D3DTEXTUREFILTERTYPE					MinFilter[ USE_TEXTURESTAGE_NUM ] ;				// 各サンプラの縮小フィルタ
	D_D3DTEXTUREFILTERTYPE					MipFilter[ USE_TEXTURESTAGE_NUM ] ;				// 各サンプラのミップマップフィルタ
	RECT									ScissorRect ;									// シザー矩形
	DWORD									SetFVF ;										// 現在 Direct3DDevice9 にセットしてある FVF コード
	D_IDirect3DIndexBuffer9 *				SetIndexBuffer ;								// 現在 Direct3DDevice9 にセットしてあるインデックスバッファ
	int										SetNormalVertexShader ;							// 通常描画用頂点シェーダーがセットされているかどうか
	int										SetNormalPixelShader ;							// 通常描画用ピクセルシェーダーがセットされているかどうか
	D_IDirect3DPixelShader9 *				SetPixelShader ;								// 現在 Direct3DDevice9 にセットしてあるピクセルシェーダー
	D_IDirect3DVertexDeclaration9 *			SetVertexDeclaration ;							// 現在 Direct3DDevice9 にセットしてある頂点シェーダ宣言
	D_IDirect3DVertexShader9 *				SetVertexShader ;								// 現在 Direct3DDevice9 にセットしてある頂点シェーダー
	D_IDirect3DVertexBuffer9 *				SetVertexBuffer ;								// 現在 Direct3DDevice9 にセットしてある頂点バッファ
	int										SetVertexBufferStride ;							// 現在 Direct3DDevice9 にセットしてある頂点バッファのストライド
	int										ShadeMode ;										// シェーディングモード
	D_IDirect3DSurface9	*					TargetSurface[DX_RENDERTARGET_COUNT] ;			// 描画対象のサーフェス
	int										TexAddressModeU[ USE_TEXTURESTAGE_NUM ] ;		// テクスチャアドレスモードＵ
	int										TexAddressModeV[ USE_TEXTURESTAGE_NUM ] ;		// テクスチャアドレスモードＶ
	int										TexAddressModeW[ USE_TEXTURESTAGE_NUM ] ;		// テクスチャアドレスモードＷ
	int										TextureTransformUse[ USE_TEXTURESTAGE_NUM ] ;	// テクスチャ座標変換行列を使用するかどうか( TRUE:使用する  FALSE:使用しない )
	MATRIX									TextureTransformMatrix[ USE_TEXTURESTAGE_NUM ] ;// テクスチャ座標変換行列
	int										UseSpecular ;									// スペキュラを使用するかどうか
	D_D3DVIEWPORT9							Viewport ;										// ビューポート情報
	int										ZEnable ;										// Ｚバッファの有効フラグ
	int										ZFunc ;											// Ｚ値の比較モード
	int										ZWriteEnable ;									// Ｚバッファへの書き込みを行うか、フラグ
} ;


// Direct3DDevice9 の描画処理に関わる情報の構造体
struct GRAPHICS_HARDDATA_DIRECT3D9_DRAWINFO
{
	BYTE *					VertexBufferNextAddr ;					// 次のデータを格納すべき頂点バッファ内のアドレス
	BYTE *					VertexBufferAddr[ 2 ][ 3 ] ;			// 各頂点バッファへのポインタ( [ ３Ｄ頂点かどうか ][ 頂点タイプ ] )
	BYTE					VertexBuffer_NoTex[     D3DDEV_NOTEX_VERTBUFFERSIZE     ] ;	// テクスチャを使用しない頂点バッファ
	BYTE					VertexBuffer_Tex[       D3DDEV_TEX_VERTBUFFERSIZE       ] ;	// テクスチャを使用する頂点バッファ
	BYTE					VertexBuffer_BlendTex[  D3DDEV_BLENDTEX_VERTBUFFERSIZE  ] ;	// ブレンドテクスチャを使用する頂点バッファ
	BYTE					VertexBuffer_3D[        D3DDEV_NOTEX_3D_VERTBUFFERSIZE  ] ;	// ３Ｄ用頂点バッファ
	int						Use3DVertex ;							// ３Ｄ頂点を使用しているかどうか( 1:使用している  0:使用していない )
	int						VertexNum ; 							// 頂点バッファに格納されている頂点の数
	int						VertexType ;							// 頂点バッファに格納されている頂点データ( 0:テクスチャなし  1:テクスチャあり  2:ブレンドテクスチャあり )
	D_D3DPRIMITIVETYPE		PrimitiveType ;							// 頂点バッファに格納されている頂点データのプリミティブタイプ

	DWORD					DiffuseColor ;							// ディフューズカラー

//	RECT					DrawRect ;								// 描画範囲
	int						BeginSceneFlag ;						// Graphics_D3D9_BeginScene を実行してあるかどうか
	int						BlendMaxNotDrawFlag ;					// ブレンド値が最大であることにより描画を行わないかどうか、フラグ
} ;


// Direct3DDevice9 の画面関係の情報の構造体
struct GRAPHICS_HARDDATA_DIRECT3D9_SCREENINFO
{
	D_IDirect3DSurface9 *	BackBufferSurface ;						// デバイスが持つバックバッファサーフェス
	int						SubBackBufferTextureSizeX ;				// サブバックバッファテクスチャの横幅
	int						SubBackBufferTextureSizeY ;				// サブバックバッファテクスチャの縦幅
	D_IDirect3DTexture9 *	SubBackBufferTexture ;					// ScreenCopy や GetDrawScreen を実現するために使用する描画可能テクスチャ
	D_IDirect3DSurface9 *	SubBackBufferSurface ;					// ScreenCopy や GetDrawScreen を実現するために使用する描画可能サーフェス
	D_IDirect3DSurface9 *	ZBufferSurface ;						// メインで使用するＺバッファ
	int						ZBufferSizeX ;							// Ｚバッファサーフェスの幅
	int						ZBufferSizeY ;							// Ｚバッファサーフェスの高さ
	int						ZBufferBitDepth ;						// Ｚバッファサーフェスのビット深度

	D_IDirect3DSurface9 *	DrawScreenBufferLockSMSurface ;			// バックバッファをロックした際に直接ロックできない場合に使用するサーフェスのポインタ
	int						DrawScreenBufferLockSMSurfaceIndex ;	// バックバッファをロックした際に直接ロックできない場合に使用するサーフェスキャッシュのインデックス
	D_IDirect3DSurface9 *	DrawScreenBufferLockSurface ;			// ロックしたバックバッファ

	D_IDirect3DSurface9 *   SaveDrawScreen_SMSurface ;				// SaveDrawScreen で使用するシステムメモリサーフェス

	int						WM_PAINTDrawCounter ;					// WM_PAINT が来た後何回か Direct3D 9 管轄外のクライアント領域を処理するためのカウンタ
} ;


// Direct3DDevice9 の描画設定に関わる情報の構造体
struct GRAPHICS_HARDDATA_DIRECT3D9_DRAWSETTING
{
	int						DrawPrepParamFlag ;						// 前回 Graphics_D3D9_DrawPreparation に入ったときの ParamFlag パラメータ
	int						DrawPrepAlwaysFlag ;					// 必ず Graphics_D3D9_DrawPreparation を行うかどうかのフラグ

	int						AlphaChannelValidFlag ;					// αチャンネル有効フラグ
	int						AlphaTestValidFlag ;					// αテスト有効フラグ( Func は必ず D_D3DCMP_GREATEREQUAL )
	int						AlphaTestMode ;							// アルファテストモード
	int						AlphaTestParam ;						// アルファテストパラメータ
	int						BlendMode ;								// ブレンドモード
	int						ChangeBlendParamFlag ;					// ブレンド設定に関わる部分の変更があったか、フラグ
	int						ChangeTextureFlag ;						// テクスチャが変更されたか、フラグ
	int						IgnoreGraphColorFlag ;					// 描画する画像のＲＧＢ成分を無視するかどうかのフラグ
	int						UseDiffuseRGBColor ;					// ディフューズカラーのＲＧＢ値を使用するか、フラグ
	int						UseDiffuseAlphaColor ;					// ディフューズカラーのα値を使用するか、フラグ
	int						BlendGraphType ;						// ブレンド画像タイプ
	int						BlendGraphFadeRatio ;					// ブレンド画像のフェードパラメータ
	int						BlendGraphBorderParam ;					// ブレンド画像の境界パラメータ(０(ブレンド画像の影響０)　←　(ブレンド画像の影響少ない)　←　１２８(ブレンド画像の影響１００％)　→　(ブレンド画像の影響を超えて非描画部分が増える)　→２５５(全く描画されない) )
	int						BlendGraphBorderRange ;					// ブレンド画像の境界幅(０〜２５５　狭い〜広い　しかし４段階)
	float					BlendTextureWidth ;						// ブレンドテクスチャの幅
	float					BlendTextureHeight ;					// ブレンドテクスチャの高さ
	float					InvBlendTextureWidth ;					// ブレンドテクスチャの幅の逆数
	float					InvBlendTextureHeight ;					// ブレンドテクスチャの高さの逆数

	D_IDirect3DTexture9		*RenderTexture ;						// 描画時に使用するテクスチャー
	D_IDirect3DTexture9		*BlendTexture ;							// 描画時に描画テクスチャーとブレンドするαチャンネルテクスチャー

	int						CancelSettingEqualCheck ;				// このフラグが立っている場合は現在の設定と変更後の設定が同じかどうかに関わらず、必ず変更処理を実行する
} ;


// Direct3DDevice9 関係の設定情報の構造体
struct GRAPHICS_HARDDATA_DIRECT3D9_DEVICE_SETTING
{
	void					( *DeviceRestoreCallbackFunction)( void *Data ) ;	// デバイスロストから復帰したときに呼ぶ関数
	void					  *DeviceRestoreCallbackData ;						// デバイスロストから復帰したときに呼ぶ関数に渡すポインタ

	void					( *DeviceLostCallbackFunction)( void *Data ) ;		// デバイスロストから復帰する前に呼ぶ関数
	void					  *DeviceLostCallbackData ;							// デバイスロストから復帰する前に呼ぶ関数に渡すポインタ
} ;


// Direct3DDevice9 の能力情報構造体
struct GRAPHICS_HARDDATA_DIRECT3D9_DEVICE_CAPS
{
	D_D3DCAPS9				DeviceCaps ;										// デバイス情報

	int						VertexHardwareProcess ;								// 頂点演算をハードウエアで処理するかどうか( TRUE:処理する  FALSE:処理しない )
//	int						TextureSizePow2 ;									// テクスチャのサイズが２のｎ乗である必要があるかどうか
//	int						TextureSizeNonPow2Conditional ;						// 条件付でテクスチャのサイズが２のｎ乗でなくても大丈夫かどうか
//	int						MaxTextureSize ;									// 最大テクスチャサイズ
	int						MaxTextureBlendStages ;								// 固定機能パイプラインで使用できる最大テクスチャステージ数
	int						ValidTexTempRegFlag ;								// テクスチャステージのテンポラリレジスタが使用できるかどうかのフラグ
	int						ValidDestBlendOp ;									// D3DBLENDOP_ADD 以外が使用可能かどうか( TRUE:使用可能  FALSE:使用不可能 )
//	int						RenderTargetNum ;									// 同時に設定できるレンダリングターゲットの数
//	int						MaxPrimitiveCount ;									// 一度に描画できるプリミティブの最大数
	int						UseRenderTargetLock ;								// 描画先サーフェスのロックを行うかどうか( TRUE:行う  FALSE:行わない )
	float					DrawFillCircleLeftVertAddX ;						// 中を塗りつぶす円の描画の際に左端座標の x 座標に足す値
	float					DrawFillCircleRightVertAddX ;						// 中を塗りつぶす円の描画の際に右端座標の x 座標に足す値

#ifndef DX_NON_FILTER
	int						ValidRenderTargetInputTexture ;						// 描画先を入力テクスチャとして使用できるかどうか( TRUE:使用できる  FALSE:使用できない )
#endif // DX_NON_FILTER

	D_D3DFORMAT				ScreenFormat ;										// 画面カラーフォーマット
	D_D3DFORMAT				TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_NUM ] ;	// テクスチャフォーマット
	D_D3DFORMAT				MaskColorFormat ;									// マスクカラーバッファ用フォーマット
	D_D3DFORMAT				MaskAlphaFormat ;									// マスクアルファチャンネル用フォーマット
	D_D3DFORMAT				ZBufferFormat[ ZBUFFER_FORMAT_NUM ] ;				// Ｚバッファフォーマット
} ;


// Direct3D9 を使った描画処理で使用するデータの内の Direct3D9Device のリセットで
// 情報を初期化する必要がある、Direct3D9Device に密接に関わった情報を纏めた構造体
struct GRAPHICS_HARDDATA_DIRECT3D9_DEVICE
{
	GRAPHICS_HARDDATA_DIRECT3D9_DEVICE_CAPS			Caps ;					// デバイス能力情報構造体		

	GRAPHICS_HARDDATA_DIRECT3D9_DEVICE_SETTING		Setting ;				// デバイス関係の設定情報を纏めた構造体

	GRAPHICS_HARDDATA_DIRECT3D9_SCREENINFO			Screen ;				// 画面関係の情報構造体

	GRAPHICS_HARDWARE_DIRECT3D9_SHADER				Shader ;				// シェーダーを纏めた構造体

	GRAPHICS_HARDWARE_DIRECT3D9_VERTEXDECLARATION	VertexDeclaration ;		// 頂点シェーダ宣言を纏めた構造体

	GRAPHICS_HARDDATA_DIRECT3D9_DRAWSETTING			DrawSetting ;			// 描画設定に関わる情報を纏めた構造体

	GRAPHICS_HARDDATA_DIRECT3D9_DRAWINFO			DrawInfo ;				// 描画処理に関わる情報を纏めた構造体

	GRAPHICS_HARDDATA_DIRECT3D9_DEVICE_STATE		State ;					// Direct3D9Device に設定している値を纏めた構造体
} ;














// Direct3D9 の 標準描画用のシェーダーコードの元データを纏めた構造体
struct GRAPHICS_HARDWARE_DIRECT3D9_SHADERCODE_BASE
{
	int						BaseShaderInitializeFlag ;				// シェーダーバイナリのセットアップが完了しているかどうかのフラグ( TRUE:完了している  FALSE:完了していない )

	// ライトインデックスリスト
	short					LightIndexList84[ 4 ][ 4 ][ 4 ][ 4 ][ 4 ][ 4 ] ;
	short					LightIndexList20[ 4 ][ 4 ][ 4 ] ;
	short					LightIndexList10[ 3 ][ 3 ][ 3 ] ;

#ifndef DX_NON_NORMAL_DRAW_SHADER
	DXARC					BaseShaderBinDxa ;						// 基本シェーダーオブジェクトファイルＤＸＡ構造体
	void					*BaseShaderBinDxaImage ;				// 基本シェーダーオブジェクトファイルＤＸＡのバイナリイメージ
#endif // DX_NON_NORMAL_DRAW_SHADER

#ifndef DX_NON_FILTER
	DXARC					FilterShaderBinDxa ;					// フィルターシェーダーオブジェクトファイルＤＸＡ構造体
	void					*FilterShaderBinDxaImage ;				// フィルターシェーダーオブジェクトファイルＤＸＡのバイナリイメージ

	void					*RGBAMixS_ShaderPackImage ;				// RGBAMix の S だけの組み合わせ２５６個のシェーダーパッケージバイナリイメージ

	// RGBAMix の S だけの組み合わせ２５６個×２のシェーダー[ R ][ G ][ B ][ A ][ 0:通常用  1:乗算済みアルファ用 ]
	void					*RGBAMixS_ShaderAddress[ 4 ][ 4 ][ 4 ][ 4 ][ 2 ] ;
	short					RGBAMixS_ShaderSize[ 4 ][ 4 ][ 4 ][ 4 ][ 2 ] ;
#endif // DX_NON_FILTER
} ;

// Direct3D9 の 標準３Ｄ描画用のシェーダーコードの元データを纏めた構造体
struct GRAPHICS_HARDWARE_DIRECT3D9_SHADERCODE_BASE3D
{
	volatile int			Base3DShaderInitializeFlag ;			// シェーダーバイナリのセットアップが完了しているかどうかのフラグ( TRUE:完了している  FALSE:完了していない )

	void					*Base3DShaderPackageImage ;				// 標準３Ｄ描画用シェーダーパッケージバイナリイメージ

	// モデル描画用ピクセル単位ライティングタイプの頂点シェーダー( リストは配列の左から )
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// /* [ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ 2:8ボーン内スキニングメッシュ ) ] */
	// /* [ バンプマップの有無( 0:無し 1:有り ) ] */
	// [ フォグタイプ ]
	void					*Base3D_PixelLighting_VSAddress[ 2 ]/*[ 3 ]*//*[ 2 ]*/[ 4 ] ;

	// モデル描画用ピクセル単位ライティングタイプのピクセルシェーダー( リストは配列の左から )
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// /*[ マルチテクスチャのブレンドモード、MV1_LAYERBLEND_TYPE_TRANSLUCENT などに +1 したもの ( 0:マルチテクスチャ無し  1:αブレンド  2:加算ブレンド  3:乗算ブレンド  4:乗算ブレンド×2 ) ]*/
	// /*[ スペキュラマップ ( 0:無い 1:ある ) ]*/
	// /*[ バンプマップ ( 0:なし 1:あり ) ]*/
	// [ ライトインデックス ]
	// [ スペキュラの有無( 0:無し 1:有り ) ]
	// [ 特殊効果( 0:通常描画  1:乗算描画  2:RGB反転  3:描画輝度４倍  4:乗算済みαブレンドモードの通常描画  5:乗算済みαブレンドモードのRGB反転  6:乗算済みαブレンドモードの描画輝度4倍 ) ]
	void					*Base3D_PixelLighting_Normal_PSAddress[ 2 ]/*[ 5 ]*//*[ 2 ]*//*[ 2 ]*/[ 84 ][ 2 ][ 7 ] ;






	// モデル描画用のシャドウマップへのレンダリング用頂点シェーダー
	// /*[ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ  2:8ボーン内スキニングメッシュ ) ]*/
	// [ バンプマップの有無( 0:無し 1:有り ) ]
	void					*Base3D_ShadowMap_VSAddress/*[ 3 ]*/[ 2 ] ;

	// モデル描画用のライティングなし頂点シェーダー
	// /*[ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ  2:8ボーン内スキニングメッシュ ) ]*/
	// [ フォグタイプ ]
	void					*Base3D_NoLighting_VSAddress/*[ 3 ]*/[ 4 ] ;

	// モデル描画用のライティングあり頂点シェーダー
	// [ シェーダーモデル( 0:SM2  1:SM3 )
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// /*[ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ  2:8ボーン内スキニングメッシュ ) ]*/
	// /*[ バンプマップの有無( 0:無し 1:有り ) ]*/
	// [ フォグタイプ ]
	// [ ライトインデックス ]
	// [ スペキュラの有無( 0:無し 1:有り ) ]
	void					*Base3D_VertexLighting_VSAddress[ 2 ][ 2 ]/*[ 3 ]*//*[ 2 ]*/[ 4 ][ 20 ][ 2 ] ;


	// モデル描画用のシャドウマップへのレンダリング用ピクセルシェーダー
	void					*Base3D_ShadowMap_Normal_PSAddress ;

	// モデル描画用のライティングなしピクセルシェーダー
	// /*[ マルチテクスチャのブレンドモード、MV1_LAYERBLEND_TYPE_TRANSLUCENT などに +1 したもの ( 0:マルチテクスチャ無し  1:αブレンド  2:加算ブレンド  3:乗算ブレンド  4:乗算ブレンド×2 ) ]*/
	// [ 特殊効果( 0:通常描画  1:乗算描画  2:RGB反転  3:描画輝度４倍  4:乗算済みαブレンドモードの通常描画  5:乗算済みαブレンドモードのRGB反転  6:乗算済みαブレンドモードの描画輝度4倍 ) ]
	void					*Base3D_NoLighting_Normal_PSAddress/*[ 5 ]*/[ 7 ] ;

	// モデル描画用のライティングありピクセルシェーダー
	// [ シェーダーモデル( 0:SM2  1:SM3 )
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// /* [ マルチテクスチャのブレンドモード、MV1_LAYERBLEND_TYPE_TRANSLUCENT などに +1 したもの ( 0:マルチテクスチャ無し  1:αブレンド  2:加算ブレンド  3:乗算ブレンド  4:乗算ブレンド×2 ) ] */
	// /* [ スペキュラマップ ( 0:無い 1:ある ) ] */
	// /* [ バンプマップ ( 0:なし 1:あり ) ] */
	// [ ライトインデックス ]
	// [ スペキュラの有無( 0:無し 1:有り ) ]
	// [ 特殊効果( 0:通常描画  1:乗算描画  2:RGB反転  3:描画輝度４倍  4:乗算済みαブレンドモードの通常描画  5:乗算済みαブレンドモードのRGB反転  6:乗算済みαブレンドモードの描画輝度4倍 ) ]
	void					*Base3D_VertexLighting_Normal_PSAddress[ 2 ][ 2 ]/*[ 5 ]*//*[ 2 ]*//*[ 2 ]*/[ 10 ][ 2 ][ 7 ] ;
} ;

#ifndef DX_NON_MODEL

// Direct3D9 の モデル描画用のシェーダーコードの元データを纏めた構造体
struct GRAPHICS_HARDWARE_DIRECT3D9_SHADERCODE_MODEL
{
	volatile int			ModelShaderInitializeFlag ;				// モデル用のシェーダーバイナリのセットアップが完了しているかどうかのフラグ( TRUE:完了している  FALSE:完了していない )

	void					*ModelShaderPackImage ;					// ３Ｄモデル用シェーダーパッケージバイナリイメージ

	// モデル描画用ピクセル単位ライティングタイプの頂点シェーダー( リストは配列の左から )
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// [ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ 2:8ボーン内スキニングメッシュ ) ]
	// [ バンプマップの有無( 0:無し 1:有り ) ]
	// [ フォグタイプ ]
	void					*MV1_PixelLighting_VSAddress[ 2 ][ 3 ][ 2 ][ 4 ] ;

	// モデル描画用ピクセル単位ライティングタイプのトゥーン用ピクセルシェーダー( リストは配列の左から )
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// [ トゥーンレンダリングかどうか( 0:トゥーンレンダリング type 1   1:トゥーンレンダリング Type 2 ) ]
	// [ トゥーンレンダリングのスフィアマップの有無とブレンドタイプ( 0:スフィアマップは無い   1:MV1_LAYERBLEND_TYPE_MODULATE  2:MV1_LAYERBLEND_TYPE_ADDITIVE ) ]
	// [ トゥーンレンダリングのディフューズグラデーションのブレンドタイプ( 0:MV1_LAYERBLEND_TYPE_TRANSLUCENT  1:MV1_LAYERBLEND_TYPE_MODULATE ) ]
	// [ トゥーンレンダリングのスペキュラグラデーションのブレンドタイプ( 0:MV1_LAYERBLEND_TYPE_TRANSLUCENT  1:MV1_LAYERBLEND_TYPE_ADDITIVE ) ]
	// [ スペキュラマップ ( 0:無い 1:ある ) ]
	// [ バンプマップ ( 0:なし 1:あり ) ]
	// [ ライトインデックス ]
	// [ スペキュラの有無( 0:無し 1:有り ) ]
	void					*MV1_PixelLighting_Toon_PSAddress[ 2 ][ 2 ][ 3 ][ 2 ][ 2 ][ 2 ][ 2 ][ 84 ][ 2 ] ;

	// モデル描画用ピクセル単位ライティングタイプのピクセルシェーダー( リストは配列の左から )
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// [ マルチテクスチャのブレンドモード、MV1_LAYERBLEND_TYPE_TRANSLUCENT などに +1 したもの ( 0:マルチテクスチャ無し  1:αブレンド  2:加算ブレンド  3:乗算ブレンド  4:乗算ブレンド×2 ) ]
	// [ スペキュラマップ ( 0:無い 1:ある ) ]
	// [ バンプマップ ( 0:なし 1:あり ) ]
	// [ ライトインデックス ]
	// [ スペキュラの有無( 0:無し 1:有り ) ]
	void					*MV1_PixelLighting_Normal_PSAddress[ 2 ][ 5 ][ 2 ][ 2 ][ 84 ][ 2 ] ;





	// モデル描画用のマテリアル要素描画用ピクセルシェーダー
	// [ マテリアルタイプ( DX_MATERIAL_TYPE_MAT_SPEC_LUMINANCE_UNORM など ) ]
	void					*MV1_MaterialType_PSAddress[ DX_MATERIAL_TYPE_NUM ] ;






	// モデル描画用のトゥーンレンダリングの輪郭線描画用頂点シェーダー
	// [ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ  2:8ボーン内スキニングメッシュ ) ]
	// [ フォグタイプ ]
	void					*MV1_ToonOutLine_VSAddress[ 3 ][ 4 ] ;

	// モデル描画用のシャドウマップへのレンダリング用頂点シェーダー
	// [ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ  2:8ボーン内スキニングメッシュ ) ]
	void					*MV1_ShadowMap_VSAddress[ 3 ] ;

	// モデル描画用のライティングなし頂点シェーダー
	// [ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ  2:8ボーン内スキニングメッシュ ) ]
	// [ フォグタイプ ]
	void					*MV1_NoLighting_VSAddress[ 3 ][ 4 ] ;

	// モデル描画用のライティングあり頂点シェーダー
	// [ シェーダーモデル( 0:SM2  1:SM3 )
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// [ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ  2:8ボーン内スキニングメッシュ ) ]
	// [ バンプマップの有無( 0:無し 1:有り ) ]
	// [ フォグタイプ ]
	// [ ライトインデックス ]
	// [ スペキュラの有無( 0:無し 1:有り ) ]
	void					*MV1_VertexLighting_VSAddress[ 2 ][ 2 ][ 3 ][ 2 ][ 4 ][ 20 ][ 2 ] ;


	// モデル描画用のトゥーンレンダリングの輪郭線描画用ピクセルシェーダー
	void					*MV1_ToonOutLine_PSAddress ;

	// モデル描画用のシャドウマップへのトゥーンレンダリング用ピクセルシェーダー
	// [ トゥーンレンダリングタイプ( 0:トゥーンレンダリング type 1   1:トゥーンレンダリング type 2 ) ]
	// [ トゥーンレンダリングのスフィアマップの有無とブレンドタイプ( 0:スフィアマップは無い   1:MV1_LAYERBLEND_TYPE_MODULATE  2:MV1_LAYERBLEND_TYPE_ADDITIVE ) ]
	// [ トゥーンレンダリングのディフューズグラデーションのブレンドタイプ( 0:MV1_LAYERBLEND_TYPE_TRANSLUCENT  1:MV1_LAYERBLEND_TYPE_MODULATE ) ]
	void					*MV1_ShadowMap_Toon_PSAddress[ 2 ][ 3 ][ 2 ] ;

	// モデル描画用のシャドウマップへのレンダリング用ピクセルシェーダー
	void					*MV1_ShadowMap_Normal_PSAddress ;

	// モデル描画用のライティングなしトゥーン用ピクセルシェーダー( リストは配列の左から )
	// [ トゥーンレンダリングタイプ( 0:トゥーンレンダリング type 1   1:トゥーンレンダリング type 2 ) ]
	// [ トゥーンレンダリングのディフューズグラデーションのブレンドタイプ( 0:MV1_LAYERBLEND_TYPE_TRANSLUCENT or トゥーンレンダリングではない  1:MV1_LAYERBLEND_TYPE_MODULATE ) ]
	void					*MV1_NoLighting_Toon_PSAddress[ 2 ][ 2 ] ;

	// モデル描画用のライティングなしピクセルシェーダー
	// [ マルチテクスチャのブレンドモード、MV1_LAYERBLEND_TYPE_TRANSLUCENT などに +1 したもの ( 0:マルチテクスチャ無し  1:αブレンド  2:加算ブレンド  3:乗算ブレンド  4:乗算ブレンド×2 ) ]
	void					*MV1_NoLighting_Normal_PSAddress[ 5 ] ;

	// モデル描画用のライティングありトゥーン用ピクセルシェーダー
	// [ シェーダーモデル( 0:SM2  1:SM3 )
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// [ トゥーンレンダリングタイプ( 0:トゥーンレンダリング type 1   1:トゥーンレンダリング type 2 ) ]
	// [ トゥーンレンダリングのスフィアマップの有無とブレンドタイプ( 0:スフィアマップは無い   1:MV1_LAYERBLEND_TYPE_MODULATE  2:MV1_LAYERBLEND_TYPE_ADDITIVE ) ]
	// [ トゥーンレンダリングのディフューズグラデーションのブレンドタイプ( 0:MV1_LAYERBLEND_TYPE_TRANSLUCENT  1:MV1_LAYERBLEND_TYPE_MODULATE ) ]
	// [ トゥーンレンダリングのスペキュラグラデーションのブレンドタイプ( 0:MV1_LAYERBLEND_TYPE_TRANSLUCENT  1:MV1_LAYERBLEND_TYPE_ADDITIVE ) ]
	// [ スペキュラマップ ( 0:無い 1:ある ) ]
	// [ バンプマップ ( 0:なし 1:あり ) ]
	// [ ライトインデックス ]
	// [ スペキュラの有無( 0:無し 1:有り ) ]
	void					*MV1_VertexLighting_Toon_PSAddress[ 2 ][ 2 ][ 2 ][ 3 ][ 2 ][ 2 ][ 2 ][ 2 ][ 10 ][ 2 ] ;

	// モデル描画用のライティングありピクセルシェーダー
	// [ シェーダーモデル( 0:SM2  1:SM3 )
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// [ マルチテクスチャのブレンドモード、MV1_LAYERBLEND_TYPE_TRANSLUCENT などに +1 したもの ( 0:マルチテクスチャ無し  1:αブレンド  2:加算ブレンド  3:乗算ブレンド  4:乗算ブレンド×2 ) ]
	// [ スペキュラマップ ( 0:無い 1:ある ) ]
	// [ バンプマップ ( 0:なし 1:あり ) ]
	// [ ライトインデックス ]
	// [ スペキュラの有無( 0:無し 1:有り ) ]
	void					*MV1_VertexLighting_Normal_PSAddress[ 2 ][ 2 ][ 5 ][ 2 ][ 2 ][ 10 ][ 2 ] ;
} ;

#endif // DX_NON_MODEL


// シェーダーコードの元データを纏めた構造体
struct GRAPHICS_HARDWARE_DIRECT3D9_SHADERCODE
{
	GRAPHICS_HARDWARE_DIRECT3D9_SHADERCODE_BASE		Base ;				// 標準描画処理用のシェーダーコードを纏めた構造体
	GRAPHICS_HARDWARE_DIRECT3D9_SHADERCODE_BASE3D	Base3D ;			// 標準３Ｄ描画処理用のシェーダーコードを纏めた構造体

#ifndef DX_NON_MODEL
	GRAPHICS_HARDWARE_DIRECT3D9_SHADERCODE_MODEL	Model ;				// モデル描画処理用のシェーダーコードを纏めた構造体
#endif // DX_NON_MODEL
} ;


// アダプター関係情報構造体
struct GRAPHICS_HARDWARE_DIRECT3D9_ADAPTER
{
	int												ValidInfo ;					// アダプタの情報が有効かどうか( TRUE:有効  FALSE:無効 )
	int												InfoNum ;					// アダプタ情報の数
	D_D3DADAPTER_IDENTIFIER9						Info[ DX_D3D9_MAX_DEVICE_LISTUP ] ;	// アダプタの情報

//	int												ValidUseAdapterNumber ;		// UseAdapterNumber が有効かどうか( TRUE:有効  FALSE:無効 )
//	int												UseAdapterNumber ;			// 使用するデバイスアダプタ番号
} ;


// Direct3D9 関係の設定情報の構造体
struct GRAPHICS_HARDDATA_DIRECT3D9_SETTING
{
	int						NotUseDirect3D9Ex ;						// Direct3D9Ex を使用しないかどうか
	int						NonUseVertexHardwareProcess ;			// 強制的に頂点演算をハードウエアで処理しないかどうか( TRUE:処理しない  FALSE:処理する )
//	int						DisableAeroFlag ;						// Aero を無効にするかどうかのフラグ( TRUE:無効にする  FALSE:無効にしない )
//	D_D3DMULTISAMPLE_TYPE	FSAAMultiSampleType ;					// FSAA用マルチサンプリングタイプ
//	int						FSAAMultiSampleQuality ;				// FSAA用マルチサンプリングクオリティ
	int						UseOldDrawModiGraphCodeFlag ;			// 以前の DrawModiGraph 関数コードを使用するかどうかのフラグ
	int						UserZBufferSizeSet ;					// 外部からＺバッファサイズの指定があったかどうか
	int						UserZBufferBitDepthSet ;				// 外部からＺバッファのビット深度指定があったかどうか
} ;


// 非管理テクスチャへのデータ転送用システムメモリ配置テクスチャの情報
struct GRAPHICS_HARDDATA_DIRECT3D9_SYSMEMTEXTURE
{
	unsigned char					InitializeFlag ;						// 使用可能な状態かどうかのフラグ
	unsigned char					UseFlag ;								// 使用中かどうかのフラグ
	unsigned char					CubeMap ;								// キューブマップかどうか( 1:キューブマップテクスチャ  0:通常テクスチャ )
	unsigned char					MipMapCount ;							// ミップマップの数
	short							Width ;									// 幅
	short							Height ;								// 高さ
	D_D3DFORMAT						Format ;								// フォーマット
	unsigned int					UseCount ;								// 使用回数
	int								UseTime ;								// 最後に使用した時間
	D_IDirect3DBaseTexture9	*		MemTexture ;							// システムメモリテクスチャ
} ;


// 非管理テクスチャへのデータ転送用システムメモリ配置サーフェスの情報
struct GRAPHICS_HARDDATA_DIRECT3D9_SYSMEMSURFACE
{
	unsigned char					InitializeFlag ;						// 使用可能な状態かどうかのフラグ
	unsigned char					UseFlag ;								// 使用中かどうかのフラグ
	short							Width ;									// 幅
	short							Height ;								// 高さ
	D_D3DFORMAT						Format ;								// フォーマット
	unsigned int					UseCount ;								// 使用回数
	int								UseTime ;								// 最後に使用した時間
	D_IDirect3DSurface9	*			MemSurface ;							// システムメモリサーフェス
} ;


// Direct3D9 のシステムメモリテクスチャ・サーフェス情報の構造体
struct GRAPHICS_HARDDATA_DIRECT3D9_SYSMEM_TEX_SURF
{
	GRAPHICS_HARDDATA_DIRECT3D9_SYSMEMTEXTURE		Texture[ DIRECT3D9_SYSMEMTEXTURE_NUM ] ;	// 非管理テクスチャへのデータ転送用システムメモリ配置テクスチャ
	int												TextureInitNum ;				// 初期化済みのシステムメモリテクスチャの数

	GRAPHICS_HARDDATA_DIRECT3D9_SYSMEMSURFACE		Surface[ DIRECT3D9_SYSMEMSURFACE_NUM ] ;	// 非管理テクスチャへのデータ転送用システムメモリ配置サーフェス
	int												SurfaceInitNum ;				// 初期化済みのシステムメモリサーフェスの数
} ;


// Direct3D9 を使ったグラフィックス処理情報の構造体
struct GRAPHICS_HARDDATA_DIRECT3D9
{
//	int												NormalDraw_UseVertexShader ;	// 通常描画に頂点シェーダーを使用するかどうか( TRUE:使用する  FALSE:使用しない )
	int												NormalDraw_NotUsePixelShader ;	// 通常描画にピクセルシェーダーを使用しないかどうか( TRUE:使用しない  FALSE:使用する )
	int												UsePixelLightingShader ;	// ピクセル単位でライティングを行うタイプのシェーダーを使用するかどうか( TRUE:使用する  FALSE:使用しない )

#ifndef DX_NON_MODEL
	D_IDirect3DVolumeTexture9 *						RGBtoVMaxRGBVolumeTexture ;	// RGBカラーを輝度を最大にしたRGB値に変換するためのボリュームテクスチャ
#endif // DX_NON_MODEL

	GRAPHICS_HARDDATA_DIRECT3D9_SETTING				Setting ;					// Direct3D9 関係の設定情報

	GRAPHICS_HARDDATA_DIRECT3D9_DEVICE				Device ;					// Direct3D9Device に密接に関連した情報

	GRAPHICS_HARDWARE_DIRECT3D9_ADAPTER				Adapter ;					// アダプター関係情報

	GRAPHICS_HARDWARE_DIRECT3D9_SHADERCODE			ShaderCode ;				// D_IDirect3DPixelShader9 や D_IDirect3DVertexShader9 の元となるシェーダーコードデータを纏めた構造体

	GRAPHICS_HARDDATA_DIRECT3D9_SYSMEM_TEX_SURF		SysMemTexSurf ;				// システムメモリテクスチャ・サーフェス情報
} ;


// 内部大域変数宣言 --------------------------------------------------------------

extern GRAPHICS_HARDDATA_DIRECT3D9 GraphicsHardDataDirect3D9 ;

// 関数プロトタイプ宣言-----------------------------------------------------------


// Direct3D9 の初期化関係
extern	int		Graphics_D3D9_Initialize( void ) ;								// Direct3D9 を使用したグラフィックス処理の初期化を行う( 0:成功 -1:失敗 )
extern	int		Graphics_D3D9_Terminate( void ) ;								// Direct3D9 を使用したグラフィックス処理の後始末を行う

extern	int		Graphics_D3D9_ReleaseObjectAll( void ) ;						// すべての Direct3D9 系オブジェクトを解放する
extern	int		Graphics_D3D9_CreateObjectAll( void ) ;							// すべての Direct3D9 系オブジェクトを作成する

extern	int		Graphics_D3D9_SetupRenderTargetInputTextureFlag( void ) ;		// ピクセルシェーダーを使用する場合は出力先をテクスチャとして使用できるかどうかのチェックを行う








// シェーダーコード関係
extern	int		Graphics_D3D9_ShaderCode_Base_Initialize( void ) ;				// Direct3D9 の標準描画用のシェーダーコードの初期化を行う
extern	int		Graphics_D3D9_ShaderCode_Base_Terminate( void ) ;				// Direct3D9 の標準描画用のシェーダーコードの後始末を行う

extern	int		Graphics_D3D9_ShaderCode_Base3D_Initialize( void ) ;			// Direct3D9 の標準３Ｄ描画用のシェーダーコードの初期化を行う
extern	int		Graphics_D3D9_ShaderCode_Base3D_Terminate( void ) ;				// Direct3D9 の標準３Ｄ描画用のシェーダーコードの後始末を行う

#ifndef DX_NON_MODEL

extern	int		Graphics_D3D9_ShaderCode_Model_Initialize( void ) ;				// Direct3D9 のモデル描画用のシェーダーコードの初期化を行う
extern	int		Graphics_D3D9_ShaderCode_Model_Terminate( void ) ;				// Direct3D9 のモデル描画用のシェーダーコードの後始末を行う

extern	int		Graphics_D3D9_Shader_Normal3DDraw_Setup( void ) ;				// ３Ｄ標準描画の指定の頂点用の描画用シェーダーをセットアップする
extern	int		Graphics_D3D9_Shader_Model_Setup( int VertexShaderIndex, int VertexShaderIndex_PL, int PixelShaderIndex, int PixelShaderIndex_PL ) ;		// 指定のモデル描画用シェーダーをセットアップする( TRUE:成功  FALSE:失敗 )

#endif // DX_NON_MODEL

extern	int		Graphics_D3D9_Shader_Initialize( void ) ;						// Direct3D9 のシェーダーの初期化を行う
extern	int		Graphics_D3D9_Shader_Terminate( void ) ;						// Direct3D9 のシェーダーの後始末をする

extern	void	Graphics_D3D9_Shader_RefreshUseIndex( void ) ;					// 使用するシェーダーのインデックスの値を更新する

extern	D_D3DXSHADER_CONSTANTINFO *Graphics_D3D9_GetShaderConstInfo( SHADERHANDLEDATA *Shader, const wchar_t *ConstantName ) ;				// シェーダーの定数情報を得る








// 頂点バッファ・インデックスバッファ関係
extern	int		Graphics_D3D9_VertexBuffer_CreateObject(  VERTEXBUFFERHANDLEDATA *VertexBuffer, int Restore ) ;	// 頂点バッファハンドル用の Direct3DVertexBuffer9 オブジェクトを作成する
extern	int		Graphics_D3D9_VertexBuffer_ReleaseObject( VERTEXBUFFERHANDLEDATA *VertexBuffer ) ;				// 頂点バッファハンドル用の Direct3DVertexBuffer9 オブジェクトを解放する
extern	int		Graphics_D3D9_IndexBuffer_CreateObject(   INDEXBUFFERHANDLEDATA *IndexBuffer,   int Restore ) ;	// インデックスバッファハンドル用の Direct3DIndexBuffer9 オブジェクトを作成する
extern	int		Graphics_D3D9_IndexBuffer_ReleaseObject(  INDEXBUFFERHANDLEDATA *IndexBuffer ) ;				// インデックスバッファハンドル用の Direct3DIndexBuffer9 オブジェクトを解放する








// Direct3D9 の画面関係
extern	int		Graphics_D3D9_SetupSubBackBuffer( void ) ;						// サブバックバッファを使用する準備を行う( 0:成功  -1:失敗 )
extern	int		Graphics_D3D9_CreateZBuffer( void ) ;							// Ｚバッファオブジェクトの作成( 0:成功  -1:失敗 )








// Direct3D9 の画像関係
extern COLORDATA *Graphics_D3D9_GetD3DFormatColorData( D_D3DFORMAT Format ) ;									// D3DFORMAT のフォーマットに合わせたカラーフォーマット情報を取得する
extern	int		Graphics_D3D9_UpdateDrawTexture( IMAGEDATA_ORIG_HARD_TEX *OrigTex, int TargetSurface ) ;		// 描画先用テクスチャと描画用テクスチャが分かれている場合に、描画用テクスチャに描画先用テクスチャの内容を反映する

// 基本イメージのフォーマットを DX_BASEIMAGE_FORMAT_NORMAL に変換する必要があるかどうかをチェックする
// ( RequiredRgbBaseImageConvFlag と RequiredAlphaBaseImageConvFlag に入る値  TRUE:変換する必要がある  FALSE:変換する必要は無い )
extern	int		Graphics_D3D9_CheckRequiredNormalImageConv_BaseImageFormat_PF(
	IMAGEDATA_ORIG *Orig,
	int             RgbBaseImageFormat,
	int            *RequiredRgbBaseImageConvFlag,
	int             AlphaBaseImageFormat = -1,
	int            *RequiredAlphaBaseImageConvFlag = NULL
) ;








// Direct3DDevice の初期化関係
extern	int		Graphics_D3D9_Device_Create( void ) ;																// Direct3DDevice9 オブジェクトの作成( 0:成功  -1:失敗 )

extern	int		Graphics_D3D9_Device_Initialize( void ) ;															// Direct3DDevice9 関係の初期化
extern	int		Graphics_D3D9_Device_ReInitialize( void ) ;															// 一時的に Direct3DDevice9 を破棄した際に、破棄前の状態を作り出すための関数








// Direct3DDevice9 ステータス関係
extern	void	Graphics_D3D9_DeviceState_RefreshRenderState( void ) ;												// 描画設定をリフレッシュ
extern	int		Graphics_D3D9_DeviceState_SetBaseState( void ) ;													// Direct3DDevice9 の基本設定を行う
extern	int		Graphics_D3D9_DeviceState_SetSampleFilterMode( int Sampler, int SetTarget, D_D3DTEXTUREFILTERTYPE FilterType ) ;		// サンプラーのテクスチャフィルタリングモードを設定する
extern	int		Graphics_D3D9_DeviceState_SetZEnable( int Flag )  ;													// Ｚバッファの有効無効のセット
extern	int		Graphics_D3D9_DeviceState_SetZWriteEnable( int Flag ) ;												// Ｚバッファの書き込みの有無をセット
extern	int		Graphics_D3D9_DeviceState_SetZFunc( int CmpType ) ;													// 深度値の比較タイプをセット
extern	int		Graphics_D3D9_DeviceState_SetDepthBias( int Bias ) ;												// 深度値のバイアスをセット
extern	int		Graphics_D3D9_DeviceState_SetFillMode( int FillMode ) ;												// フィルモードをセット
extern	int		Graphics_D3D9_DeviceState_SetWorldMatrix( const MATRIX *Matrix ) ;									// ワールド変換用行列をセットする
extern	int		Graphics_D3D9_DeviceState_SetViewMatrix( const MATRIX *Matrix ) ;									// ビュー変換用行列をセットする
extern	int		Graphics_D3D9_DeviceState_SetProjectionMatrix( const MATRIX *Matrix ) ;								// 投影変換用行列をセットする
extern	int		Graphics_D3D9_DeviceState_SetLightState( int LightNumber, D_D3DLIGHT9 *Light ) ;					// ライトのセット
extern	int		Graphics_D3D9_DeviceState_SetLightEnable( int LightNumber, int EnableState ) ;						// ライトの有効、無効を変更
extern	int		Graphics_D3D9_DeviceState_SetMaterial( D_D3DMATERIAL9 *Material ) ;									// マテリアルのセット
extern	int		Graphics_D3D9_DeviceState_SetSpecularEnable( int UseFlag ) ;										// スペキュラを使用するかどうかをセット
extern	int		Graphics_D3D9_DeviceState_SetDiffuseMaterialSource( int UseFlag ) ;									// 頂点のディフューズカラーをマテリアルのディフューズカラーとして使用するかどうかを設定する
extern	int		Graphics_D3D9_DeviceState_SetSpecularMaterialSource( int UseFlag ) ;								// 頂点のスペキュラカラーをマテリアルのスペキュラカラーとして使用するかどうかを設定する
extern	int		Graphics_D3D9_DeviceState_SetCullMode( int State ) ;												// カリングの有無をセット 
extern	int		Graphics_D3D9_DeviceState_SetAmbient( unsigned int Color ) ;										// アンビエントカラーのセット
extern	int		Graphics_D3D9_DeviceState_SetShadeMode( int ShadeMode ) ;											// シェーディングモードのセット
extern	int		Graphics_D3D9_DeviceState_SetTextureAddress( int AddressMode, int Sampler = -1 ) ;					// テクスチャーのアドレッシングモードをセットする
extern	int		Graphics_D3D9_DeviceState_SetTextureAddressU( int AddressMode, int Sampler = -1 ) ;					// テクスチャーのアドレッシングモードをセットする
extern	int		Graphics_D3D9_DeviceState_SetTextureAddressV( int AddressMode, int Sampler = -1 ) ;					// テクスチャーのアドレッシングモードをセットする
extern	int		Graphics_D3D9_DeviceState_SetTextureAddressW( int AddressMode, int Sampler = -1 ) ;					// テクスチャーのアドレッシングモードをセットする
extern	int		Graphics_D3D9_DeviceState_SetTextureAddressUVW( int AddressModeU, int AddressModeV, int AddressModeW, int Sampler = -1 ) ;		// テクスチャーのアドレッシングモードをセットする
extern	int		Graphics_D3D9_DeviceState_SetTextureAddressTransformMatrix( int Use, MATRIX *Matrix, int Samplar = -1 ) ;	// テクスチャ座標変換行列をセットする
extern	int		Graphics_D3D9_DeviceState_SetFogEnable( int Flag ) ;												// フォグを有効にするかどうかを設定する( TRUE:有効  FALSE:無効 )
extern	int		Graphics_D3D9_DeviceState_SetFogVertexMode( int Mode /* DX_FOGMODE_NONE 等 */ ) ;					// フォグモードを設定する
extern	int		Graphics_D3D9_DeviceState_SetFogColor( unsigned int Color ) ;										// フォグカラーを変更する
extern	int		Graphics_D3D9_DeviceState_SetFogStartEnd( float Start, float End ) ;								// フォグが始まる距離と終了する距離を設定する( 0.0f 〜 1.0f )
extern	int		Graphics_D3D9_DeviceState_SetFogDensity( float Density ) ;											// フォグの密度を設定する( 0.0f 〜 1.0f )
extern	int		Graphics_D3D9_DeviceState_SetLighting( int Flag ) ;													// ライトの有無フラグをセットする
extern	int		Graphics_D3D9_DeviceState_SetMaxAnisotropy( int MaxAnisotropy, int Sampler = -1 ) ;					// 最大異方性をセットする
extern	int		Graphics_D3D9_DeviceState_SetViewport( D_D3DVIEWPORT9 *Viewport ) ;									// ビューポートをセットする
extern	int		Graphics_D3D9_DeviceState_SetViewportEasy( int x1, int y1, int x2, int y2 ) ;						// ビューポートをセットする( 簡易版 )
extern	int		Graphics_D3D9_DeviceState_SetScissorRect( RECT *Rect ) ;											// シザー矩形を設定する
extern	int		Graphics_D3D9_DeviceState_SetDrawMode( int DrawMode ) ;												// 描画モードのセット
extern	int		Graphics_D3D9_DeviceState_SetRenderTarget( D_IDirect3DSurface9 *TargetSurface, int TargetIndex = 0 ) ;	// 描画対象の変更
extern	int		Graphics_D3D9_DeviceState_SetFVF( int FVF ) ;														// 使用するFVFを変更する
extern	int		Graphics_D3D9_DeviceState_SetVertexDeclaration( D_IDirect3DVertexDeclaration9 *VertexDeclaration ) ;	// 使用する頂点シェーダ宣言を変更する
#ifndef DX_NON_MODEL
extern	int		Graphics_D3D9_DeviceState_SetMV1VertexDeclaration( int BumpMap, int SkinMesh, int UVNum ) ;			// 指定のモデル用の頂点シェーダ宣言をセットする
#endif // DX_NON_MODEL
extern	int		Graphics_D3D9_DeviceState_SetVertexShaderToHandle( int ShaderHandle ) ;								// 指定の頂点シェーダーをデバイスにセットする
extern	int		Graphics_D3D9_DeviceState_SetVertexShader( D_IDirect3DVertexShader9 *VertexShader, int NormalVertexShader = FALSE ) ;	// 使用する頂点シェーダーを変更する
extern	int		Graphics_D3D9_DeviceState_ResetVertexShader( int SetNormalVertexShaderCancel = FALSE ) ;			// 頂点シェーダーの使用を止める( 固定機能パイプラインを使用する状態にする )
extern	int		Graphics_D3D9_DeviceState_SetPixelShaderToHandle( int ShaderHandle ) ;								// 指定のピクセルシェーダーをデバイスにセットする
extern	int		Graphics_D3D9_DeviceState_SetPixelShader( D_IDirect3DPixelShader9 *PixelShader, int NormalPixelShader = FALSE ) ;	// 使用するピクセルシェーダーを変更する
extern	int		Graphics_D3D9_DeviceState_ResetPixelShader( int SetNormalPixelShaderCancel = FALSE ) ;				// ピクセルシェーダーの使用を止める( 固定機能パイプラインを使用する状態にする )
extern	int		Graphics_D3D9_DeviceState_SetVertexBuffer( D_IDirect3DVertexBuffer9 *VertexBuffer, int VertexStride = 0 ) ;	// 使用する頂点バッファを変更する
extern	int		Graphics_D3D9_DeviceState_SetIndexBuffer( D_IDirect3DIndexBuffer9 *IndexBuffer ) ;					// 使用するインデックスバッファを変更する
extern	int		Graphics_D3D9_DeviceState_ResetTextureCoord( void ) ;												// 各ステージが使用するテクスチャアドレスをステージ番号と同じにする
extern	int		Graphics_D3D9_DeviceState_SetUserBlendInfo(  GRAPHICS_HARDDATA_DIRECT3D9_BLENDINFO *BlendInfo, int TextureStageIsTextureAndTextureCoordOnlyFlag, int TextureIsGraphHandleFlag = TRUE, int UseShader = TRUE ) ;					// ユーザーのブレンドインフォを適応する
extern	int		Graphics_D3D9_DeviceState_RefreshBlendState( GRAPHICS_HARDDATA_DIRECT3D9_BLENDINFO *pUserInfo = NULL, int TextureStageIsTextureAndTextureCoordOnlyFlag = FALSE, int TextureIsGraphHandleFlag = TRUE, int UseShader = TRUE ) ;	// ブレンド設定を更新(Direct3D に反映)する
extern	void	Graphics_D3D9_DeviceState_RefreshAmbientAndEmissiveParam( void ) ;									// アンビエントライトとエミッシブカラーを掛け合わせたパラメータを更新する








// 描画設定関係関数
extern	int		Graphics_D3D9_DrawSetting_SetDrawBlendMode( int BlendMode, int AlphaTestValidFlag, int AlphaChannelValidFlag ) ;	// 描画ブレンドモードの設定
extern	int		Graphics_D3D9_DrawSetting_SetIgnoreDrawGraphColor( int EnableFlag ) ;							// 描画時の画像のＲＧＢを無視するかどうかを設定する
extern	int		Graphics_D3D9_DrawSetting_SetDrawAlphaTest( int TestMode, int TestParam ) ;					// アルファテストの設定
extern	int		Graphics_D3D9_DrawSetting_SetUseDiffuseAlphaColorFlag( int UseFlag ) ;						// ディフューズカラーのα値を使用するかどうかフラグをセットする
extern	int		Graphics_D3D9_DrawSetting_SetUseDiffuseRGBColorFlag( int UseFlag ) ;							// ディフューズカラーのＲＧＢ値を使用するかどうかフラグをセットする
extern	int		Graphics_D3D9_DrawSetting_SetBlendTextureParam( int BlendType, int *Param ) ;					// ブレンドするテクスチャのパラメータをセットする
extern	int		Graphics_D3D9_DrawSetting_SetTexture( D_IDirect3DTexture9 *RenderTexture ) ;					// テクスチャをセットする 
extern	int		Graphics_D3D9_DrawSetting_SetBlendTexture( D_IDirect3DTexture9 *BlendTexture, int TexWidth = -1, int TexHeight = -1 ) ;	// ブレンドするテクスチャをセットする 








// シェーダー定数管理構造体処理関数
extern	int		Graphics_D3D9_ShaderConstant_UseArea_Initialize(  DIRECT3D9_SHADERCONSTANT_USEAREA *UseArea, int TotalSize ) ;															// シェーダー定数使用領域情報を初期化する
extern	int		Graphics_D3D9_ShaderConstant_UseArea_Set(         DIRECT3D9_SHADERCONSTANT_USEAREA *UseArea, int IsUse, int Index, int Num ) ;											// シェーダー定数使用領域情報を変更する
extern	int		Graphics_D3D9_ShaderConstant_UseArea_Create(      DIRECT3D9_SHADERCONSTANT_USEAREA *UseArea, BYTE *Map, BYTE SetNumber ) ;												// シェーダー定数使用領域情報から使用マップを作成する

extern	int		Graphics_D3D9_ShaderConstant_InfoSet_Initialize(  DIRECT3D9_SHADERCONSTANTINFOSET *ConstInfoSet ) ;																		// シェーダー定数情報の初期化
extern	int		Graphics_D3D9_ShaderConstant_InfoSet_SetUseState( DIRECT3D9_SHADERCONSTANTINFOSET *ConstInfoSet, int ApplyMask ) ;														// 指定のシェーダー定数セットを適用するかどうかを設定する DX_SHADERCONSTANTSET_MASK_LIB | DX_SHADERCONSTANTSET_MASK_LIB_SUB 等
extern	int		Graphics_D3D9_ShaderConstant_InfoSet_SetParam(    DIRECT3D9_SHADERCONSTANTINFOSET *ConstInfoSet, int TypeIndex, int SetIndex, int ConstantIndex, const void *Param, int ParamNum, int UpdateUseArea ) ;	// シェーダー定数情報を設定する
extern	int		Graphics_D3D9_ShaderConstant_InfoSet_ResetParam(  DIRECT3D9_SHADERCONSTANTINFOSET *ConstInfoSet, int TypeIndex, int SetIndex, int ConstantIndex, int ParamNum ) ;										// 指定領域のシェーダー定数情報をリセットする








// 頂点シェーダ宣言関係関数
extern	int		Graphics_D3D9_VertexDeclaration_Create( void ) ;		// Direct3DVertexDeclaration9 オブジェクトの作成( 0:成功  -1:失敗 )
extern	int		Graphics_D3D9_VertexDeclaration_Terminate( void ) ;		// Direct3DVertexDeclaration9 オブジェクトの削除( 0:成功  -1:失敗 )








// システムテクスチャ・サーフェス関係関数
extern	int		Graphics_D3D9_GetSysMemTextureFromOrig( struct IMAGEDATA_ORIG *Orig, struct IMAGEDATA_ORIG_HARD_TEX *OrigTex, int ASyncThread = FALSE ) ;		// IMAGEDATA_ORIG と IMAGEDATA_ORIG_HARD_TEX の情報を元に Graphics_D3D9_GetSysMemTexture を使用する
extern	int		Graphics_D3D9_GetSysMemTexture( int CubeMap, int Width, int Height, int MipMapCount, D_D3DFORMAT Format, int ASyncThread ) ;		// 管理テクスチャへの転送用のシステムメモリテクスチャを取得する
extern	int		Graphics_D3D9_GetSysMemSurface( int Width, int Height, D_D3DFORMAT Format, int ASyncThread ) ;										// 管理テクスチャへの転送用のシステムメモリサーフェスを取得する
extern	int		Graphics_D3D9_CreateSysMemTextureDirect3D9Texture( GRAPHICS_HARDDATA_DIRECT3D9_SYSMEMTEXTURE *MemTex ) ;							// 構造体のデータを元に転送用のシステムメモリテクスチャを作成する
extern	int		Graphics_D3D9_CreateSysMemSurfaceDirect3D9Surface( GRAPHICS_HARDDATA_DIRECT3D9_SYSMEMSURFACE *MemSurface ) ;						// 構造体のデータを元に転送用のシステムメモリサーフェスを作成する
extern	int		Graphics_D3D9_ReleaseSysMemTexture( int Index, int ASyncThread = FALSE ) ;															// 管理テクスチャへの転送用のシステムメモリテクスチャを解放する
extern	int		Graphics_D3D9_ReleaseSysMemSurface( int Index, int ASyncThread = FALSE ) ;															// 管理テクスチャへの転送用のシステムメモリサーフェスを解放する
extern	int		Graphics_D3D9_ReleaseSysMemTextureAll( int IsNotUseOnly = FALSE ) ;																	// 管理テクスチャへの転送用のシステムメモリテクスチャをすべて解放する
extern	int		Graphics_D3D9_ReleaseSysMemSurfaceAll( int IsNotUseOnly = FALSE ) ;																	// 管理テクスチャへの転送用のシステムメモリサーフェスをすべて解放する

extern	int		Graphics_D3D9_SysMemTextureProcess( void ) ;																						// 管理テクスチャへの転送用のシステムメモリテクスチャの定期処理を行う
extern	int		Graphics_D3D9_SysMemSurfaceProcess( void ) ;																						// 管理テクスチャへの転送用のシステムメモリサーフェスの定期処理を行う








// Direct3D9 の描画処理準備関係
extern	int		Graphics_D3D9_RenderVertex( int ASyncThread = FALSE ) ;							// 頂点バッファに溜まった頂点データをレンダリングする
extern	void	FASTCALL Graphics_D3D9_DrawPreparation( int ParamFlag = 0 ) ;					// 描画準備を行う
extern	void	Graphics_D3D9_BeginScene( void ) ;												// ビギンシーンを行う
extern	void	Graphics_D3D9_EndScene( void ) ;												// エンドシーンを行う	








// Direct3D9 を使った描画関係
extern	int		Graphics_D3D9_DrawBillboard3D( VECTOR Pos, float cx, float cy, float Size, float Angle, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, int ReverseXFlag, int ReverseYFlag, int DrawFlag = TRUE, RECT *DrawArea = NULL ) ;	// ハードウエアアクセラレータ使用版 DrawBillboard3D
extern	int		Graphics_D3D9_DrawModiBillboard3D( VECTOR Pos, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, int DrawFlag = TRUE, RECT *DrawArea = NULL ) ;	// ハードウエアアクセラレータ使用版 DrawModiBillboard3D
extern	int		Graphics_D3D9_DrawGraph( int x, int y, float xf, float yf, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, int IntFlag ) ;																// ハードウエアアクセラレータ使用版 DrawGraph
extern	int		Graphics_D3D9_DrawExtendGraph( int x1, int y1, int x2, int y2, float x1f, float y1f, float x2f, float y2f, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, int IntFlag ) ;				// ハードウエアアクセラレータ使用版 DrawExtendGraph
extern	int		Graphics_D3D9_DrawRotaGraph( int x, int y, float xf, float yf, double ExRate, double Angle, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, int ReverseXFlag, int ReverseYFlag, int IntFlag ) ;				// ハードウエアアクセラレータ使用版 DrawRotaGraph
extern	int		Graphics_D3D9_DrawRotaGraphFast( int x, int y, float xf, float yf, float ExRate, float Angle, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, int ReverseXFlag, int ReverseYFlag, int IntFlag ) ;				// ハードウエアアクセラレータ使用版 DrawRotaGraphFast
extern	int		Graphics_D3D9_DrawModiGraph( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, bool SimpleDrawFlag ) ;					// ハードウエアアクセラレータ使用版 DrawModiGraph
extern	int		Graphics_D3D9_DrawModiGraphF( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, bool SimpleDrawFlag ) ;	// ハードウエアアクセラレータ使用版 DrawModiGraphF
extern	void	Graphics_D3D9_DrawModiTex( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, IMAGEDATA_HARD_DRAW *DrawTex, bool SimpleDrawFlag ) ;								// テクスチャを変形して描画する
extern	int		Graphics_D3D9_DrawSimpleQuadrangleGraphF( const GRAPHICS_DRAW_DRAWSIMPLEQUADRANGLEGRAPHF_PARAM *Param, IMAGEDATA *Image, IMAGEDATA *BlendImage ) ;												// ハードウエアアクセラレータ使用版 DrawSimpleQuadrangleGraphF
extern	int		Graphics_D3D9_DrawSimpleTriangleGraphF(   const GRAPHICS_DRAW_DRAWSIMPLETRIANGLEGRAPHF_PARAM   *Param, IMAGEDATA *Image, IMAGEDATA *BlendImage ) ;												// ハードウエアアクセラレータ使用版 DrawSimpleTriangleGraphF

extern	int		Graphics_D3D9_DrawFillBox( int x1, int y1, int x2, int y2, unsigned int Color ) ;																		// ハードウエアアクセラレータ使用版 DrawFillBox
extern	int		Graphics_D3D9_DrawLineBox( int x1, int y1, int x2, int y2, unsigned int Color ) ;																		// ハードウエアアクセラレータ使用版 DrawLineBox
extern	int		Graphics_D3D9_DrawLine( int x1, int y1, int x2, int y2, unsigned int Color ) ;																			// ハードウエアアクセラレータ使用版 DrawLine
extern	int		Graphics_D3D9_DrawLine3D( VECTOR Pos1, VECTOR Pos2, unsigned int Color, int DrawFlag = TRUE, RECT *DrawArea = NULL ) ;									// ハードウエアアクセラレータ使用版 DrawLine3D
extern	int		Graphics_D3D9_DrawCircle_Thickness( int x, int y, int r, unsigned int Color, int Thickness ) ;															// ハードウエアアクセラレータ使用版 DrawCircle( 太さ指定あり )
extern	int		Graphics_D3D9_DrawOval_Thickness( int x, int y, int rx, int ry, unsigned int Color, int Thickness ) ;													// ハードウエアアクセラレータ使用版 DrawOval( 太さ指定あり )
extern	int		Graphics_D3D9_DrawCircle( int x, int y, int r, unsigned int Color, int FillFlag, int Rx_One_Minus, int Ry_One_Minus ) ;									// ハードウエアアクセラレータ使用版 DrawCircle
extern	int		Graphics_D3D9_DrawOval( int x, int y, int rx, int ry, unsigned int Color, int FillFlag, int Rx_One_Minus, int Ry_One_Minus ) ;							// ハードウエアアクセラレータ使用版 DrawOval
extern	int		Graphics_D3D9_DrawTriangle( int x1, int y1, int x2, int y2, int x3, int y3, unsigned int Color, int FillFlag ) ;										// ハードウエアアクセラレータ使用版 DrawTriangle
extern	int		Graphics_D3D9_DrawTriangle3D( VECTOR Pos1, VECTOR Pos2, VECTOR Pos3, unsigned int Color, int FillFlag, int DrawFlag = TRUE, RECT *DrawArea = NULL ) ;	// ハードウエアアクセラレータ使用版 DrawTriangle3D
extern	int		Graphics_D3D9_DrawQuadrangle( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int Color, int FillFlag ) ;						// ハードウエアアクセラレータ使用版 DrawQuadrangle
extern	int		Graphics_D3D9_DrawQuadrangleF( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int Color, int FillFlag ) ;						// ハードウエアアクセラレータ使用版 DrawQuadrangle
extern	int		Graphics_D3D9_DrawPixel( int x, int y, unsigned int Color ) ;																							// ハードウエアアクセラレータ使用版 DrawPixel
extern	int		Graphics_D3D9_DrawPixel3D( VECTOR Pos, unsigned int Color, int DrawFlag = TRUE, RECT *DrawArea = NULL ) ;												// ハードウエアアクセラレータ使用版 DrawPixel3D
extern	int		Graphics_D3D9_DrawPixelSet( const POINTDATA *PointData, int Num ) ;																						// ハードウエアアクセラレータ使用版 DrawPixelSet
extern	int		Graphics_D3D9_DrawLineSet( const LINEDATA *LineData, int Num ) ;																						// ハードウエアアクセラレータ使用版 DrawLineSet

extern	int		Graphics_D3D9_DrawPrimitive(                             const VERTEX_3D *Vertex, int VertexNum,                                    int PrimitiveType, IMAGEDATA *Image, int TransFlag ) ;
extern	int		Graphics_D3D9_DrawIndexedPrimitive(                      const VERTEX_3D *Vertex, int VertexNum, const WORD *Indices, int IndexNum, int PrimitiveType, IMAGEDATA *Image, int TransFlag ) ;
extern	int		Graphics_D3D9_DrawPrimitiveLight(                        const VERTEX3D  *Vertex, int VertexNum,                                    int PrimitiveType, IMAGEDATA *Image, int TransFlag ) ;
extern	int		Graphics_D3D9_DrawIndexedPrimitiveLight(                 const VERTEX3D  *Vertex, int VertexNum, const WORD *Indices, int IndexNum, int PrimitiveType, IMAGEDATA *Image, int TransFlag ) ;
extern	int		Graphics_D3D9_DrawPrimitiveLight_UseVertexBuffer(        VERTEXBUFFERHANDLEDATA *VertexBuffer,                                      int PrimitiveType,                 int StartVertex, int UseVertexNum, IMAGEDATA *Image, int TransFlag ) ;
extern	int		Graphics_D3D9_DrawIndexedPrimitiveLight_UseVertexBuffer( VERTEXBUFFERHANDLEDATA *VertexBuffer, INDEXBUFFERHANDLEDATA *IndexBuffer,  int PrimitiveType, int BaseVertex, int StartVertex, int UseVertexNum, int StartIndex, int UseIndexNum, IMAGEDATA *Image, int TransFlag ) ;
extern	int		Graphics_D3D9_DrawPrimitive2D(                                 VERTEX_2D *Vertex, int VertexNum,                                    int PrimitiveType, IMAGEDATA *Image, int TransFlag, int BillboardFlag, int Is3D, int ReverseXFlag, int ReverseYFlag, int TextureNo, int IsShadowMap ) ;
extern	int		Graphics_D3D9_DrawPrimitive2DUser(                       const VERTEX2D  *Vertex, int VertexNum,                                    int PrimitiveType, IMAGEDATA *Image, int TransFlag, int Is3D, int ReverseXFlag, int ReverseYFlag, int TextureNo ) ;
extern	int		Graphics_D3D9_DrawIndexedPrimitive2DUser(                const VERTEX2D  *Vertex, int VertexNum, const WORD *Indices, int IndexNum, int PrimitiveType, IMAGEDATA *Image, int TransFlag ) ;

extern	void	Graphics_D3D9_DrawPreparationToShader( int ParamFlag, int UseVertexShaderNo, int Is2D, int UseLibSubShaderConst ) ;							// シェーダー描画用描画前セットアップ関数

extern	int		Graphics_D3D9_DrawPrimitive2DToShader(        const VERTEX2DSHADER *Vertex, int VertexNum,                                              int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ ) ;		// シェーダーを使って２Ｄプリミティブを描画する
extern	int		Graphics_D3D9_DrawPrimitive3DToShader(        const VERTEX3DSHADER *Vertex, int VertexNum,                                              int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ ) ;		// シェーダーを使って３Ｄプリミティブを描画する
extern	int		Graphics_D3D9_DrawPrimitiveIndexed2DToShader( const VERTEX2DSHADER *Vertex, int VertexNum, const unsigned short *Indices, int IndexNum, int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ ) ;		// シェーダーを使って２Ｄプリミティブを描画する( 頂点インデックスを使用する )
extern	int		Graphics_D3D9_DrawPrimitiveIndexed3DToShader( const VERTEX3DSHADER *Vertex, int VertexNum, const unsigned short *Indices, int IndexNum, int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ ) ;		// シェーダーを使って３Ｄプリミティブを描画する( 頂点インデックスを使用する )

extern	int		Graphics_D3D9_DrawPrimitive3DToShader_UseVertexBuffer2(        int VertexBufHandle,                     int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */, int StartVertex, int UseVertexNum ) ;	// シェーダーを使って３Ｄプリミティブを描画する( 頂点バッファ使用版 )
extern	int		Graphics_D3D9_DrawPrimitiveIndexed3DToShader_UseVertexBuffer2( int VertexBufHandle, int IndexBufHandle, int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */, int BaseVertex, int StartVertex, int UseVertexNum, int StartIndex, int UseIndexNum ) ;	// シェーダーを使って３Ｄプリミティブを描画する( 頂点バッファとインデックスバッファ使用版 )








// 環境依存初期化関係
extern	int		Graphics_D3D9_Initialize_Timing0_PF( void ) ;											// 描画処理の環境依存部分の初期化を行う関数( 実行箇所区別０ )
extern	int		Graphics_D3D9_Initialize_Timing1_PF( void ) ;											// 描画処理の環境依存部分の初期化を行う関数( 実行箇所区別２ )
extern	int		Graphics_D3D9_Hardware_Initialize_PF( void ) ;											// ハードウエアアクセラレータを使用する場合の環境依存の初期化処理を行う
extern	int		Graphics_D3D9_Terminate_PF( void ) ;													// 描画処理の環境依存部分の後始末を行う関数
extern	int		Graphics_D3D9_RestoreOrChangeSetupGraphSystem_PF( int Change, int ScreenSizeX = -1, int ScreenSizeY = -1, int ColorBitDepth = -1, int RefreshRate = -1 ) ;		// グラフィックスシステムの復帰、又は変更付きの再セットアップを行う
extern	int		Graphics_D3D9_Hardware_CheckValid_PF( void ) ;											// 描画用デバイスが有効かどうかを取得する( 戻り値  TRUE:有効  FALSE:無効 )
extern	void	Graphics_D3D9_ReleaseDirectXObject_PF( void ) ;											// DirectX のオブジェクトを解放する








// 環境依存システム関係
extern	void	Graphics_D3D9_WM_ACTIVATE_ActiveProcess_PF( void ) ;									// WM_ACTIVATE メッセージの処理で、ウインドウモード時にアクティブになった際に呼ばれる関数









// 環境依存描画設定関係
extern	int		Graphics_Hardware_D3D9_SetRenderTargetToShader_PF( int TargetIndex, int DrawScreen, int SurfaceIndex, int MipLevel ) ;	// シェーダー描画での描画先を設定する
extern	int		Graphics_Hardware_D3D9_SetDrawBrightToOneParam_PF( DWORD Bright ) ;											// SetDrawBright の引数が一つ版
extern	int		Graphics_Hardware_D3D9_SetDrawBlendMode_PF( int BlendMode, int BlendParam ) ;								// 描画ブレンドモードをセットする
extern	int		Graphics_Hardware_D3D9_SetDrawAlphaTest_PF( int TestMode, int TestParam ) ;									// 描画時のアルファテストの設定を行う( TestMode:DX_CMP_GREATER等( -1:デフォルト動作に戻す )  TestParam:描画アルファ値との比較に使用する値 )
extern	int		Graphics_Hardware_D3D9_SetDrawMode_PF( int DrawMode ) ;														// 描画モードをセットする
extern	int		Graphics_Hardware_D3D9_SetDrawBright_PF( int RedBright, int GreenBright, int BlueBright ) ;					// 描画輝度をセット
extern	int		Graphics_Hardware_D3D9_SetBlendGraphParamBase_PF( IMAGEDATA *BlendImage, int BlendType, int *Param ) ;		// SetBlendGraphParam の可変長引数パラメータ付き
extern	int		Graphics_Hardware_D3D9_SetMaxAnisotropy_PF( int MaxAnisotropy ) ;											// 最大異方性の値をセットする
extern	int		Graphics_Hardware_D3D9_SetTransformToWorld_PF( const MATRIX *Matrix ) ;										// ワールド変換用行列をセットする
extern	int		Graphics_Hardware_D3D9_SetTransformToView_PF( const MATRIX *Matrix ) ;										// ビュー変換用行列をセットする
extern	int		Graphics_Hardware_D3D9_SetTransformToProjection_PF( const MATRIX *Matrix ) ;								// 投影変換用行列をセットする
extern	int		Graphics_Hardware_D3D9_SetTransformToViewport_PF( const MATRIX *Matrix ) ;									// ビューポート行列をセットする
extern	int		Graphics_Hardware_D3D9_SetTextureAddressMode_PF( int Mode /* DX_TEXADDRESS_WRAP 等 */, int Stage ) ;		// テクスチャアドレスモードを設定する
extern	int		Graphics_Hardware_D3D9_SetTextureAddressModeUV_PF( int ModeU, int ModeV, int Stage ) ;						// テクスチャアドレスモードを設定する
extern	int		Graphics_Hardware_D3D9_SetTextureAddressTransformMatrix_PF( int UseFlag, MATRIX *Matrix, int Sampler = -1 ) ;// テクスチャ座標変換行列をセットする
extern	int		Graphics_Hardware_D3D9_SetFogEnable_PF( int Flag ) ;														// フォグを有効にするかどうかを設定する( TRUE:有効  FALSE:無効 )
extern	int		Graphics_Hardware_D3D9_SetFogMode_PF( int Mode /* DX_FOGMODE_NONE 等 */ ) ;									// フォグモードを設定する
extern	int		Graphics_Hardware_D3D9_SetFogColor_PF( DWORD FogColor ) ;													// フォグカラーを変更する
extern	int		Graphics_Hardware_D3D9_SetFogStartEnd_PF( float start, float end ) ;										// フォグが始まる距離と終了する距離を設定する( 0.0f 〜 1.0f )
extern	int		Graphics_Hardware_D3D9_SetFogDensity_PF( float density ) ;													// フォグの密度を設定する( 0.0f 〜 1.0f )
extern	int		Graphics_Hardware_D3D9_ApplyLigFogToHardware_PF( void ) ;													// 基本データに設定されているフォグ情報をハードウェアに反映する
extern	int		Graphics_Hardware_D3D9_SetUseOldDrawModiGraphCodeFlag_PF( int Flag ) ;										// 以前の DrawModiGraph 関数のコードを使用するかどうかのフラグをセットする
extern	int		Graphics_Hardware_D3D9_RefreshAlphaChDrawMode_PF( void ) ;													// 描画先に正しいα値を書き込むかどうかのフラグを更新する








// 環境依存設定関係
extern	int		Graphics_Hardware_D3D9_SetUseHardwareVertexProcessing_PF( int Flag ) ;						// ハードウエアの頂点演算処理機能を使用するかどうかを設定する
extern	int		Graphics_Hardware_D3D9_SetUsePixelLighting_PF( int Flag ) ;									// ピクセル単位でライティングを行うかどうかを設定する、要 ShaderModel 3.0( TRUE:ピクセル単位のライティングを行う  FALSE:頂点単位のライティングを行う( デフォルト ) )
extern	int		Graphics_Hardware_D3D9_SetGraphicsDeviceRestoreCallbackFunction_PF( void (* Callback )( void *Data ), void *CallbackData ) ;			// グラフィックスデバイスがロストから復帰した際に呼ばれるコールバック関数を設定する
extern	int		Graphics_Hardware_D3D9_SetGraphicsDeviceLostCallbackFunction_PF( void (* Callback )( void *Data ), void *CallbackData ) ;			// グラフィックスデバイスがロストから復帰する前に呼ばれるコールバック関数を設定する
extern	int		Graphics_Hardware_D3D9_SetUseNormalDrawShader_PF( int Flag ) ;								// 通常描画にプログラマブルシェーダーを使用するかどうかを設定する( TRUE:使用する( デフォルト )  FALSE:使用しない )
extern	int		Graphics_Hardware_D3D9_GetVideoMemorySize_PF( int *AllSize, int *FreeSize ) ;				// ビデオメモリの容量を得る
extern	const D_IDirect3DDevice9 * Graphics_Hardware_D3D9_GetUseDirect3DDevice9_PF( void ) ;				// 使用中のＤｉｒｅｃｔ３ＤＤｅｖｉｃｅ９オブジェクトを得る
extern	const D_IDirect3DSurface9* Graphics_Hardware_D3D9_GetUseDirect3D9BackBufferSurface_PF( void ) ;		// 使用中のバックバッファのDirect3DSurface9オブジェクトを取得する
extern	int		Graphics_Hardware_D3D9_RefreshDxLibDirect3DSetting_PF( void ) ;								// ＤＸライブラリのＤｉｒｅｃｔ３Ｄ設定をしなおす
extern	int		Graphics_D3D9_SetDDrawUseGuid_PF( const GUID FAR *Guid ) ;									// ＤｉｒｅｃｔＤｒａｗが使用するＧＵＩＤを設定する
extern	const GUID *Graphics_D3D9_GetDirectDrawDeviceGUID_PF( int Number ) ;								// 有効な DirectDraw デバイスの GUID を取得する
extern	int		Graphics_D3D9_GetDirectDrawDeviceDescription_PF( int Number, char *StringBuffer ) ;			// 有効な DirectDraw デバイスの名前を得る
extern	int		Graphics_D3D9_GetDirectDrawDeviceNum_PF( void ) ;											// 有効な DirectDraw デバイスの数を取得する
//extern	int		Graphics_D3D9_SetUseDirectDrawDeviceIndex_PF( int Index ) ;									// 使用する DirectDraw デバイスのインデックスを設定する
extern	int		Graphics_D3D9_SetUseDirect3D9Ex_PF( int Flag ) ;											// Vista以降の環境で Direct3D9Ex を使用するかどうかを設定する( TRUE:使用する( デフォルト )  FALSE:使用しない )
extern	int		Graphics_D3D9_SetDialogBoxMode_PF( int Flag ) ;												// ダイアログボックスモードを変更する








// 環境依存画面関係
extern	int		Graphics_Hardware_D3D9_SetupUseZBuffer_PF( void ) ;															// 設定に基づいて使用するＺバッファをセットする
extern	int		Graphics_Hardware_D3D9_ClearDrawScreenZBuffer_PF( const RECT *ClearRect ) ;									// 画面のＺバッファの状態を初期化する
extern	int		Graphics_Hardware_D3D9_ClearDrawScreen_PF( const RECT *ClearRect ) ;										// 画面の状態を初期化する
extern	int		Graphics_Hardware_D3D9_SetDrawScreen_PF( int DrawScreen, int OldScreenSurface, int OldScreenMipLevel, IMAGEDATA *NewTargetImage, IMAGEDATA *OldTargetImage, SHADOWMAPDATA *NewTargetShadowMap, SHADOWMAPDATA *OldTargetShadowMap ) ;						// 描画先画面のセット
extern	int		Graphics_Hardware_D3D9_SetDrawScreen_Post_PF( int DrawScreen ) ;											// SetDrawScreen の最後で呼ばれる関数
extern	int		Graphics_Hardware_D3D9_SetDrawArea_PF( int x1, int y1, int x2, int y2 ) ;									// 描画可能領域のセット
extern	int		Graphics_Hardware_D3D9_LockDrawScreenBuffer_PF( RECT *LockRect, BASEIMAGE *BaseImage, int TargetScreen, IMAGEDATA *TargetImage, int TargetScreenSurface, int TargetScreenMipLevel, int ReadOnly, int TargetScreenTextureNo ) ;	// 描画先バッファをロックする
extern	int		Graphics_Hardware_D3D9_UnlockDrawScreenBuffer_PF( void ) ;													// 描画先バッファをアンロックする
extern	int		Graphics_Hardware_D3D9_ScreenCopy_PF( void ) ;																// 裏画面の内容を表画面に描画する
extern	int		Graphics_Hardware_D3D9_SaveDrawScreen_Begin_PF( int x1, int y1, int x2, int y2, BASEIMAGE *BaseImage ) ;	// 現在描画対象になっている画面を保存する
extern	int		Graphics_Hardware_D3D9_SaveDrawScreen_Post_PF( void ) ;														// 現在描画対象になっている画面を保存する
extern	int		Graphics_Hardware_D3D9_WaitVSync_PF( int SyncNum ) ;														// 垂直同期信号を待つ
extern	int		Graphics_Hardware_D3D9_ScreenFlipBase_PF( void ) ;															// 裏画面と表画面を交換する
extern	int		Graphics_Hardware_D3D9_BltRectBackScreenToWindow_PF( HWND Window, RECT BackScreenRect, RECT WindowClientRect ) ;		// 裏画面の指定の領域をウインドウのクライアント領域の指定の領域に転送する
extern	int		Graphics_Hardware_D3D9_SetScreenFlipTargetWindow_PF( HWND TargetWindow, double ScaleX, double ScaleY ) ;	// ScreenFlip で画像を転送する先のウインドウを設定する( NULL を指定すると設定解除 )
extern	int		Graphics_Hardware_D3D9_SetZBufferMode_PF( int ZBufferSizeX, int ZBufferSizeY, int ZBufferBitDepth ) ;		// メイン画面のＺバッファの設定を変更する
extern	int		Graphics_Hardware_D3D9_SetDrawZBuffer_PF( int DrawScreen, IMAGEDATA *Image ) ;								// 描画先Ｚバッファのセット








// 環境依存情報取得関係
extern	const COLORDATA *	Graphics_Hardware_D3D9_GetMainColorData_PF( void ) ;				// GetColor や GetColor2 で使用するカラーデータを取得する
extern	const COLORDATA *	Graphics_Hardware_D3D9_GetDispColorData_PF( void ) ;				// ディスプレーのカラーデータポインタを得る
extern	DWORD				Graphics_Hardware_D3D9_GetPixel_PF( int x, int y ) ;				// 指定座標の色を取得する
extern	COLOR_F				Graphics_Hardware_D3D9_GetPixelF_PF( int x, int y ) ;				// 指定座標の色を取得する( float型 )








// 環境依存画像関係
extern	int		Graphics_Hardware_D3D9_UpdateGraphMovie_TheoraYUV_PF( MOVIEGRAPH *Movie, IMAGEDATA *Image ) ;			// YUVサーフェスを使った Theora 動画の内容をグラフィックスハンドルのテクスチャに転送する
extern	int		Graphics_Hardware_D3D9_GraphLock_PF( IMAGEDATA *Image, COLORDATA **ColorDataP, int WriteOnly ) ;		// グラフィックメモリ領域のロック
extern	int		Graphics_Hardware_D3D9_GraphUnlock_PF( IMAGEDATA *Image ) ;												// グラフィックメモリ領域のロック解除
extern	int		Graphics_Hardware_D3D9_CopyGraphZBufferImage_PF( IMAGEDATA *DestImage, IMAGEDATA *SrcImage ) ;			// グラフィックのＺバッファの状態を別のグラフィックのＺバッファにコピーする( DestGrHandle も SrcGrHandle もＺバッファを持っている描画可能画像で、且つアンチエイリアス画像ではないことが条件 )
extern	int		Graphics_Hardware_D3D9_InitGraph_PF( void ) ;															// 画像データの初期化
extern	int		Graphics_Hardware_D3D9_FillGraph_PF( IMAGEDATA *Image, int Red, int Green, int Blue, int Alpha, int ASyncThread ) ;			// グラフィックを特定の色で塗りつぶす
extern	int		Graphics_Hardware_D3D9_GetDrawScreenGraphBase_PF( IMAGEDATA *Image, IMAGEDATA *TargetImage, int TargetScreen, int TargetScreenSurface, int TargetScreenMipLevel, int TargetScreenWidth, int TargetScreenHeight, int x1, int y1, int x2, int y2, int destX, int destY ) ;		// 描画可能画像やバックバッファから指定領域のグラフィックを取得する

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
) ;

extern	int		Graphics_Hardware_D3D9_CreateOrigTexture_PF(  IMAGEDATA_ORIG *Orig, int ASyncThread = FALSE ) ;		// オリジナル画像情報中のテクスチャを作成する( 0:成功  -1:失敗 )
extern	int		Graphics_Hardware_D3D9_ReleaseOrigTexture_PF( IMAGEDATA_ORIG *Orig ) ;								// オリジナル画像情報中のテクスチャを解放する
extern	int		Graphics_Hardware_D3D9_GetMultiSampleQuality_PF( int Samples ) ;									// 指定のマルチサンプル数で使用できる最大クオリティ値を取得する
extern	int		Graphics_Hardware_D3D9_SetUsePlatformTextureFormat_PF( int PlatformTextureFormat ) ;				// 作成するグラフィックハンドルで使用する環境依存のテクスチャフォーマットを指定する








// 環境依存頂点バッファ・インデックスバッファ関係
extern	int		Graphics_Hardware_D3D9_VertexBuffer_Create_PF(    VERTEXBUFFERHANDLEDATA *VertexBuffer ) ;															// 頂点バッファハンドルの頂点バッファを作成する
extern	int		Graphics_Hardware_D3D9_VertexBuffer_Terminate_PF( VERTEXBUFFERHANDLEDATA *VertexBuffer ) ;															// 頂点バッファハンドルの後始末
extern	int		Graphics_Hardware_D3D9_VertexBuffer_SetData_PF(   VERTEXBUFFERHANDLEDATA *VertexBuffer, int SetIndex, const void *VertexData, int VertexNum ) ;		// 頂点バッファに頂点データを転送する
extern	int		Graphics_Hardware_D3D9_IndexBuffer_Create_PF(     INDEXBUFFERHANDLEDATA *IndexBuffer ) ;															// インデックスバッファハンドルのセットアップを行う
extern	int		Graphics_Hardware_D3D9_IndexBuffer_Terminate_PF(  INDEXBUFFERHANDLEDATA *IndexBuffer ) ;															// インデックスバッファハンドルの後始末
extern	int		Graphics_Hardware_D3D9_IndexBuffer_SetData_PF(    INDEXBUFFERHANDLEDATA *IndexBuffer, int SetIndex, const void *IndexData, int IndexNum ) ;			// インデックスバッファにインデックスデータを転送する









// 環境依存ライト関係
extern	int		Graphics_Hardware_D3D9_Light_SetUse_PF( int Flag ) ;															// ライティングを行うかどうかを設定する
extern	int		Graphics_Hardware_D3D9_Light_GlobalAmbient_PF( COLOR_F *Color ) ;												// グローバルアンビエントライトカラーを設定する
extern	int		Graphics_Hardware_D3D9_Light_SetState_PF( int LightNumber, LIGHTPARAM *LightParam ) ;							// ライトパラメータをセット
extern	int		Graphics_Hardware_D3D9_Light_SetEnable_PF( int LightNumber, int EnableState ) ;									// ライトの有効、無効を変更








// 環境依存シャドウマップ関係
extern	int		Graphics_Hardware_D3D9_ShadowMap_CreateTexture_PF( SHADOWMAPDATA *ShadowMap, int ASyncThread = FALSE ) ;		// シャドウマップデータに必要なテクスチャを作成する
extern	int		Graphics_Hardware_D3D9_ShadowMap_ReleaseTexture_PF( SHADOWMAPDATA *ShadowMap ) ;								// シャドウマップデータに必要なテクスチャを解放する
extern	void	Graphics_Hardware_D3D9_ShadowMap_RefreshVSParam_PF( void ) ;													// 頂点シェーダーに設定するシャドウマップの情報を更新する
extern	void	Graphics_Hardware_D3D9_ShadowMap_RefreshPSParam_PF( void ) ;													// ピクセルシェーダーに設定するシャドウマップの情報を更新する
extern	int		Graphics_Hardware_D3D9_ShadowMap_DrawSetup_PF( SHADOWMAPDATA *ShadowMap ) ;										// シャドウマップへの描画の準備を行う
extern	int		Graphics_Hardware_D3D9_ShadowMap_DrawEnd_PF( SHADOWMAPDATA *ShadowMap ) ;										// シャドウマップへの描画を終了する
extern	int		Graphics_Hardware_D3D9_ShadowMap_SetUse_PF( int SlotIndex, SHADOWMAPDATA *ShadowMap ) ;							// 描画で使用するシャドウマップを指定する、スロットは０か１かを指定可能








// 環境依存シェーダー関係
extern	int		Graphics_Hardware_D3D9_Shader_Create_PF( int ShaderHandle, int IsVertexShader, void *Image, int ImageSize, int ImageAfterFree, int ASyncThread ) ;		// シェーダーハンドルを初期化する
extern	int		Graphics_Hardware_D3D9_Shader_TerminateHandle_PF( SHADERHANDLEDATA *Shader ) ;																			// シェーダーハンドルの後始末
extern	int		Graphics_Hardware_D3D9_Shader_GetValidShaderVersion_PF( void ) ;																							// 使用できるシェーダーのバージョンを取得する( 0=使えない  200=シェーダーモデル２．０が使用可能  300=シェーダーモデル３．０が使用可能 )
extern	int		Graphics_Hardware_D3D9_Shader_GetConstIndex_PF( const wchar_t *ConstantName, SHADERHANDLEDATA *Shader ) ;													// 指定の名前を持つ定数が使用するシェーダー定数の番号を取得する
extern	int		Graphics_Hardware_D3D9_Shader_GetConstCount_PF( const wchar_t *ConstantName, SHADERHANDLEDATA *Shader ) ;													// 指定の名前を持つ定数が使用するシェーダー定数の数を取得する
extern	const FLOAT4 *Graphics_Hardware_D3D9_Shader_GetConstDefaultParamF_PF( const wchar_t *ConstantName, SHADERHANDLEDATA *Shader ) ;									// 指定の名前を持つ浮動小数点定数のデフォルトパラメータが格納されているメモリアドレスを取得する
extern	int		Graphics_Hardware_D3D9_Shader_SetConst_PF(   int TypeIndex, int SetIndex, int ConstantIndex, const void *Param, int ParamNum, int UpdateUseArea ) ;	// シェーダー定数情報を設定する
extern	int		Graphics_Hardware_D3D9_Shader_ResetConst_PF( int TypeIndex, int SetIndex, int ConstantIndex, int ParamNum ) ;											// 指定領域のシェーダー定数情報をリセットする
extern	int		Graphics_Hardware_D3D9_Shader_ModelCode_Init_PF( void ) ;																								// ３Ｄモデル用のシェーダーコードの初期化を行う








// 環境依存シェーダー用定数バッファ関係
extern	int		Graphics_Hardware_D3D9_ShaderConstantBuffer_Create_PF( int ShaderConstantBufferHandle, int BufferSize, int ASyncThread ) ;												// シェーダー用定数バッファハンドルを初期化する
extern	int		Graphics_Hardware_D3D9_ShaderConstantBuffer_TerminateHandle_PF( SHADERCONSTANTBUFFERHANDLEDATA *ShaderConstantBuffer ) ;													// シェーダー用定数バッファハンドルの後始末
extern	void *	Graphics_Hardware_D3D9_ShaderConstantBuffer_GetBuffer_PF( SHADERCONSTANTBUFFERHANDLEDATA *ShaderConstantBuffer ) ;														// シェーダー用定数バッファハンドルの定数バッファのアドレスを取得する
extern	int		Graphics_Hardware_D3D9_ShaderConstantBuffer_Update_PF( SHADERCONSTANTBUFFERHANDLEDATA *ShaderConstantBuffer ) ;															// シェーダー用定数バッファハンドルの定数バッファへの変更を適用する
extern	int		Graphics_Hardware_D3D9_ShaderConstantBuffer_Set_PF( SHADERCONSTANTBUFFERHANDLEDATA *ShaderConstantBuffer, int TargetShader /* DX_SHADERTYPE_VERTEX など */, int Slot ) ;	// シェーダー用定数バッファハンドルの定数バッファを指定のシェーダーの指定のスロットにセットする








// 環境依存描画関係
extern	int		Graphics_Hardware_D3D9_Paint_PF( int x, int y, unsigned int FillColor, ULONGLONG BoundaryColor ) ;			// 指定点から境界色があるところまで塗りつぶす






#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_NON_DIRECT3D9

#endif // DX_NON_GRAPHICS

#endif // __DXGRAPHICSD3D9_H__
