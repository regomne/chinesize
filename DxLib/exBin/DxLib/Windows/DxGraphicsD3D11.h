// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		描画処理プログラム( Direct3D11 )ヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXGRAPHICSD3D11_H__
#define __DXGRAPHICSD3D11_H__

#include "../DxCompileConfig.h"

#ifndef DX_NON_GRAPHICS

#ifndef DX_NON_DIRECT3D11

// インクルード ------------------------------------------------------------------
#include "../DxLib.h"
#include "../DxGraphics.h"
#include "../DxArchive_.h"
#include "DxGraphicsAPIWin.h"
#include "DxShader_DataType_D3D11.h"
#include "DxShader_PS_D3D11.h"
#include "DxShader_VS_D3D11.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

#define GD3D11										GraphicsHardDataDirect3D11

// 列挙するアダプターの最大数
#define DX_D3D11_MAX_ADAPTER_LISTUP					(16)

// 列挙するアウトプットの最大数
#define DX_D3D11_MAX_OUTPUT_LISTUP					(32)

// 出力先ウインドウの最大数
#define DX_D3D11_MAX_OUTPUTWINDOW					(256)

// 非管理テクスチャへのデータ転送用システムメモリ配置テクスチャの数
#define DIRECT3D11_SYSMEMTEXTURE_NUM				(2048)

// 非管理テクスチャへのデータ転送用システムメモリ配置サーフェスの数
#define DIRECT3D11_SYSMEMSURFACE_NUM				(2048)

// 試す機能レベルの数
#define DIRECT3D11_FEATURE_LEVEL_NUM				(7)

// バックバッファのフォーマット
#define DIRECT3D11_BACKBUFFER_FORMAT				D_DXGI_FORMAT_R8G8B8A8_UNORM

// 16ビット深度バッファのTexture2Dフォーマット
#define DIRECT3D11_DEPTHBUFFER_TEXTURE2D_FORMAT		D_DXGI_FORMAT_D16_UNORM

// 16ビット深度バッファのDepthStencilフォーマット
#define DIRECT3D11_DEPTHBUFFER_DEPTHSTENCIL_FORMAT	D_DXGI_FORMAT_D16_UNORM

// デフォルトサンプラーステートのフィルターモードの数
#define DIRECT3D11_DEFAULTRS_FILTER_NUM				(18)

// Graphics_D3D11_DrawPreparation 関数に渡すフラグ
#define DX_D3D11_DRAWPREP_TRANS				(0x00001)
#define DX_D3D11_DRAWPREP_VECTORINT			(0x00002)
#define DX_D3D11_DRAWPREP_GOURAUDSHADE		(0x00008)
#define DX_D3D11_DRAWPREP_PERSPECTIVE		(0x00010)
#define DX_D3D11_DRAWPREP_DIFFUSERGB		(0x00020)
#define DX_D3D11_DRAWPREP_DIFFUSEALPHA		(0x00040)
#define DX_D3D11_DRAWPREP_FOG				(0x00080)
#define DX_D3D11_DRAWPREP_NOBLENDSETTING	(0x00100)
#define DX_D3D11_DRAWPREP_LIGHTING			(0x00200)
#define DX_D3D11_DRAWPREP_SPECULAR			(0x00400)
#define DX_D3D11_DRAWPREP_3D				(0x00800)
#define DX_D3D11_DRAWPREP_TEXADDRESS		(0x01000)
#define DX_D3D11_DRAWPREP_NOTSHADERRESET	(0x02000)
#define DX_D3D11_DRAWPREP_CULLING			(0x04000)
#define DX_D3D11_DRAWPREP_TEXTURE			(0x08000)
#define DX_D3D11_DRAWPREP_TEXALPHACH		(0x10000)
#define DX_D3D11_DRAWPREP_TEXALPHATEST		(0x20000)

// デフォルトラスタライザーステートのフィルモード
#define DIRECT3D11_DEFAULTRS_FILLMODE_WIRE					(0)
#define DIRECT3D11_DEFAULTRS_FILLMODE_SOLID					(1)
#define DIRECT3D11_DEFAULTRS_FILLMODE_NUM					(2)

// デフォルトステート用の比較モード定義
#define DIRECT3D11_DEFAULT_CMP_NEVER						(0)				// FALSE
#define DIRECT3D11_DEFAULT_CMP_LESS							(1)				// Src <  Dest		DrawAlpha <  TestParam
#define DIRECT3D11_DEFAULT_CMP_EQUAL						(2)				// Src == Dest		DrawAlpha == TestParam
#define DIRECT3D11_DEFAULT_CMP_LESSEQUAL					(3)				// Src <= Dest		DrawAlpha <= TestParam
#define DIRECT3D11_DEFAULT_CMP_GREATER						(4)				// Src >  Dest		DrawAlpha >  TestParam
#define DIRECT3D11_DEFAULT_CMP_NOTEQUAL						(5)				// Src != Dest		DrawAlpha != TestParam
#define DIRECT3D11_DEFAULT_CMP_GREATEREQUAL					(6)				// Src >= Dest		DrawAlpha >= TestParam
#define DIRECT3D11_DEFAULT_CMP_ALWAYS						(7)				// TRUE
#define DIRECT3D11_DEFAULT_CMP_NUM							(8)

// ピクセルシェーダーのアルファテストモード
#define DIRECT3D11_PS_ALPHATEST_CMP_GREATER					(0)				// 描画アルファ値がテストアルファ値より大きければ描画
#define DIRECT3D11_PS_ALPHATEST_CMP_OTHER					(1)				// GREATER 以外の比較モード
#define DIRECT3D11_PS_ALPHATEST_NUM							(2)

// 標準描画頂点フォーマット
#define D3D11_VERTEX_INPUTLAYOUT_NOTEX_2D		(0)				// ( VERTEX_NOTEX_2D                      構造体形式 )ライン、ボックス描画用頂点フォーマット
#define D3D11_VERTEX_INPUTLAYOUT_2D				(1)				// ( VERTEX2D or VERTEX_2D                構造体形式 )トランスフォーム済み頂点フォーマット
#define D3D11_VERTEX_INPUTLAYOUT_BLENDTEX_2D	(2)				// ( VERTEX_BLENDTEX_2D or VERTEX2DSHADER 構造体形式 )αブレンドテクスチャ付きトランスフォーム済み頂点フォーマット
#define D3D11_VERTEX_INPUTLAYOUT_NOTEX_3D		(3)				// ( VERTEX_NOTEX_3D                      構造体形式 )ライン、ボックス描画用頂点フォーマット、３Ｄ用( 旧バージョン用 )
#define D3D11_VERTEX_INPUTLAYOUT_3D				(4)				// ( VERTEX_3D                            構造体形式 )グラフィックス描画用頂点フォーマット、３Ｄ用( 旧バージョン用 )
#define D3D11_VERTEX_INPUTLAYOUT_3D_LIGHT		(5)				// ( VERTEX3D                             構造体形式 )グラフィックス描画用頂点フォーマット
#define D3D11_VERTEX_INPUTLAYOUT_SHADER_3D		(6)				// ( VERTEX3DSHADER                       構造体形式 )シェーダー描画用頂点フォーマット
#define D3D11_VERTEX_INPUTLAYOUT_NUM			(7)

// 頂点バッファのサイズ
#define D3D11_VERTEXBUFFER_SIZE					(64 * 1024)

// 各定数バッファのセットレジスタ番号
#define DX_D3D11_VS_CONSTANTBUFFER_COMMON		(0)
#define DX_D3D11_VS_CONSTANTBUFFER_BASE			(1)
#define DX_D3D11_VS_CONSTANTBUFFER_OTHERMATRIX	(2)
#define DX_D3D11_VS_CONSTANTBUFFER_MATRIX		(3)

#define DX_D3D11_PS_CONSTANTBUFFER_COMMON		(0)
#define DX_D3D11_PS_CONSTANTBUFFER_BASE			(1)
#define DX_D3D11_PS_CONSTANTBUFFER_SHADOWMAP	(2)
#define DX_D3D11_PS_CONSTANTBUFFER_FILTER		(3)

// 構造体定義 --------------------------------------------------------------------

// Direct3D11用 ハードウエアレンダリング用オリジナル画像テクスチャ情報
struct IMAGEDATA_ORIG_HARD_TEX_DIRECT3D11
{
	D_ID3D11Texture2D *				MSTexture ;								// マルチサンプルテクスチャ
	D_ID3D11Texture2D *				Texture ;								// テクスチャ
	D_ID3D11ShaderResourceView *	TextureSRV ;							// テクスチャ用シェーダーリソースビュー
//	D_ID3D11RenderTargetView *		TextureRTV[ CUBEMAP_SURFACE_NUM ] ;		// テクスチャ用レンダリングターゲットビュー
	D_ID3D11RenderTargetView **		TextureRTV ;							// テクスチャ用レンダリングターゲットビュー

	D_ID3D11Texture2D *				DepthBuffer ;							// 深度バッファ
	D_ID3D11DepthStencilView **		DepthBufferDSV ;						// 深度バッファ用デプスステンシルビュー
} ;

// Direct3D11用 シャドウマップ情報
struct SHADOWMAPDATA_DIRECT3D11
{
	D_ID3D11Texture2D *				DepthTexture ;							// 深度値書き込み用テクスチャ
	D_ID3D11ShaderResourceView *	DepthTextureSRV ;						// 深度値書き込み用テクスチャ用シェーダーリソースビュー
	D_ID3D11RenderTargetView *		DepthTextureRTV ;						// 深度値書き込み用テクスチャ用レンダリングターゲットビュー

	D_ID3D11Texture2D *				DepthBuffer ;							// 深度バッファ
	D_ID3D11DepthStencilView *		DepthBufferDSV ;						// 深度バッファ用デプスステンシルビュー
} ;

// Direct3D11用 頂点バッファハンドル情報
struct VERTEXBUFFERHANDLEDATA_DIRECT3D11
{
	D_ID3D11Buffer					*VertexBuffer ;							// ID3D11Buffer のポインタ
} ;

// Direct3D11用 インデックスバッファハンドル情報
struct INDEXBUFFERHANDLEDATA_DIRECT3D11
{
	D_ID3D11Buffer					*IndexBuffer ;							// ID3D11Buffer のポインタ
} ;

// Direct3D11用 シェーダーハンドル情報
struct SHADERHANDLEDATA_DIRECT3D11
{
	union
	{
		D_ID3D11PixelShader			*PixelShader ;							// ID3D11PixelShader のポインタ
		D_ID3D11VertexShader		*VertexShader ;							// ID3D11VertexShader のポインタ
	} ;
} ;

// 定数バッファ情報構造体
struct CONSTANTBUFFER_DIRECT3D11
{
	DWORD							Size ;									// サイズ( バイト単位 )
	D_ID3D11Buffer *				ConstantBuffer ;						// ID3D11Buffer のポインタ
	void *							SysmemBuffer ;							// 定数バッファと同サイズのシステムメモリ上のバッファ
	int								ChangeFlag ;							// システムメモリ上のバッファが変更され、ID3DBuffer への反映が必要かどうか( TRUE:必要  FALSE:不要 )

	CONSTANTBUFFER_DIRECT3D11 *		NextData ;								// リスト構造の次のデータへのポインタ
	CONSTANTBUFFER_DIRECT3D11 *		PrevData ;								// リスト構造の前のデータへのポインタ
} ;

// 定数バッファ管理情報構造体
struct CONSTANTBUFFER_DIRECT3D11_MANAGE
{
	int								BufferNum ;								// 定数バッファ構造体の数
	CONSTANTBUFFER_DIRECT3D11 *		FirstBuffer ;							// 最初の定数バッファ構造体へのポインタ
} ;

// Direct3D11用 シェーダー用定数バッファハンドル情報
struct SHADERCONSTANTBUFFERHANDLEDATA_DIRECT3D11
{
	int								SetFlag ;								// シェーダーにセットしているかどうか
	CONSTANTBUFFER_DIRECT3D11 *		ConstBuffer ;							// 定数バッファ
} ;

// Direct3D11 の標準描画用のシェーダーを纏めた構造体
struct GRAPHICS_HARDWARE_DIRECT3D11_SHADER_BASE
{
	// 標準描画用の複雑な処理を行わない頂点シェーダー
	D_ID3D11VertexShader					*BaseSimple_VS[ D3D11_VERTEX_INPUTLAYOUT_NUM ] ;

	// 固定機能パイプライン互換のピクセルシェーダー( テクスチャなし )
	// [ 特殊効果( 0:通常描画  1:乗算描画  2:RGB反転  3:描画輝度４倍  4:乗算済みαブレンドモードの通常描画  5:乗算済みαブレンドモードのRGB反転  6:乗算済みαブレンドモードの描画輝度4倍 ) ]
	// [ ピクセルシェーダーのアルファテストモード ]
	D_ID3D11PixelShader						*BaseNoneTex_PS[ 7 ][ DIRECT3D11_PS_ALPHATEST_NUM ] ;

	// 固定機能パイプライン互換のピクセルシェーダー( テクスチャあり )
	// [ ブレンド画像とのブレンドタイプ( 0:なし  1:DX_BLENDGRAPHTYPE_NORMAL  2:DX_BLENDGRAPHTYPE_WIPE  3:DX_BLENDGRAPHTYPE_ALPHA ) ]
	// [ 特殊効果( 0:通常描画  1:乗算描画  2:RGB反転  3:描画輝度４倍  4:乗算済みαブレンドモードの通常描画  5:乗算済みαブレンドモードのRGB反転  6:乗算済みαブレンドモードの描画輝度4倍 ) ]
	// [ テクスチャRGB無視( 0:無視しない  1:無視する ) ]
	// [ テクスチャAlpha無視( 0:無視しない  1:無視する ) ]
	// [ ピクセルシェーダーのアルファテストモード ]
	D_ID3D11PixelShader						*BaseUseTex_PS[ 4 ][ 7 ][ 2 ][ 2 ][ DIRECT3D11_PS_ALPHATEST_NUM ] ;

	// マスク処理用のピクセルシェーダー
	D_ID3D11PixelShader						*MaskEffect_PS ;
	D_ID3D11PixelShader						*MaskEffect_ReverseEffect_PS ;
	D_ID3D11PixelShader						*MaskEffect_UseGraphHandle_PS[ 4 ] ;
	D_ID3D11PixelShader						*MaskEffect_UseGraphHandle_ReverseEffect_PS[ 4 ] ;

	// 単純転送用関係
	D_ID3D11SamplerState					*StretchRect_SamplerState[ 2 /* 0:Point  1:Linear */ ] ;
	D_ID3D11RasterizerState					*StretchRect_RasterizerState ;
	D_ID3D11DepthStencilState				*StretchRect_DepthStencilState ;
	D_ID3D11BlendState						*StretchRect_BlendState[ 2 /* 0:NoBlend  1:AlphaBlend */ ] ;
	D_ID3D11VertexShader					*StretchRect_VS ;
	D_ID3D11VertexShader					*StretchRectTex8_VS ;
	D_ID3D11PixelShader						*StretchRect_PS ;
} ;

// Direct3D11 の標準３Ｄ描画用のシェーダーを纏めた構造体
struct GRAPHICS_HARDWARE_DIRECT3D11_SHADER_BASE3D
{
	// 標準３Ｄ描画用ピクセル単位ライティングタイプの頂点シェーダー( リストは配列の左から )
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// /* [ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ 2:8ボーン内スキニングメッシュ ) ] */
	// /* [ バンプマップの有無( 0:無し 1:有り ) ] */
	// [ フォグタイプ ]
	D_ID3D11VertexShader					*Base3D_PixelLighting_VS[ 2 ]/*[ 3 ]*//*[ 2 ]*/[ 4 ] ;

	// 標準３Ｄ描画用ピクセル単位ライティングタイプのピクセルシェーダー( リストは配列の左から )
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// /*[ マルチテクスチャのブレンドモード、MV1_LAYERBLEND_TYPE_TRANSLUCENT などに +1 したもの ( 0:マルチテクスチャ無し  1:αブレンド  2:加算ブレンド  3:乗算ブレンド  4:乗算ブレンド×2 ) ]*/
	// /*[ スペキュラマップ ( 0:無い 1:ある ) ]*/
	// /*[ バンプマップ ( 0:なし 1:あり ) ]*/
	// [ ライトインデックス ]
	// /*[ スペキュラの有無( 0:無し 1:有り ) ]*/
	// [ 特殊効果( 0:通常描画  1:乗算描画  2:RGB反転  3:描画輝度４倍  4:乗算済みαブレンドモードの通常描画  5:乗算済みαブレンドモードのRGB反転  6:乗算済みαブレンドモードの描画輝度4倍 ) ]
	// [ ピクセルシェーダーのアルファテストモード ]
	D_ID3D11PixelShader						*Base3D_PixelLighting_Normal_PS[ 2 ]/*[ 5 ]*//*[ 2 ]*//*[ 2 ]*/[ 84 ]/*[ 2 ]*/[ 7 ][ DIRECT3D11_PS_ALPHATEST_NUM ] ;



	// 標準３Ｄ描画用のシャドウマップへのレンダリング用頂点シェーダー
	// /*[ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ  2:8ボーン内スキニングメッシュ ) ]*/
	// [ バンプマップの有無( 0:無し 1:有り ) ]
	D_ID3D11VertexShader					*Base3D_ShadowMap_VS/*[ 3 ]*/[ 2 ] ;

	// 標準３Ｄ描画用のライティングなし頂点シェーダー
	// /*[ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ  2:8ボーン内スキニングメッシュ ) ]*/
	// [ フォグタイプ ]
	D_ID3D11VertexShader					*Base3D_NoLighting_VS/*[ 3 ]*/[ 4 ] ;

	// 標準３Ｄ描画用の頂点単位ライティング頂点シェーダー
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// /*[ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ  2:8ボーン内スキニングメッシュ ) ]*/
	// /*[ バンプマップの有無( 0:無し 1:有り ) ]*/
	// [ フォグタイプ ]
	// [ ライトインデックス ]
	// /*[ スペキュラの有無( 0:無し 1:有り ) ]*/
	D_ID3D11VertexShader					*Base3D_VertexLighting_VS[ 2 ]/*[ 3 ]*//*[ 2 ]*/[ 4 ][ 20 ]/*[ 2 ]*/ ;

	// 標準３Ｄ描画用のシャドウマップへのレンダリング用ピクセルシェーダー
	// [ ピクセルシェーダーのアルファテストモード ]
	D_ID3D11PixelShader						*Base3D_ShadowMap_Normal_PS[ DIRECT3D11_PS_ALPHATEST_NUM ] ;

	// 標準３Ｄ描画用のライティングなしピクセルシェーダー
	// /*[ マルチテクスチャのブレンドモード、MV1_LAYERBLEND_TYPE_TRANSLUCENT などに +1 したもの ( 0:マルチテクスチャ無し  1:αブレンド  2:加算ブレンド  3:乗算ブレンド  4:乗算ブレンド×2 ) ]*/
	// [ 特殊効果( 0:通常描画  1:乗算描画  2:RGB反転  3:描画輝度４倍  4:乗算済みαブレンドモードの通常描画  5:乗算済みαブレンドモードのRGB反転  6:乗算済みαブレンドモードの描画輝度4倍 ) ]
	// [ ピクセルシェーダーのアルファテストモード ]
	D_ID3D11PixelShader						*Base3D_NoLighting_Normal_PS/*[ 5 ]*/[ 7 ][ DIRECT3D11_PS_ALPHATEST_NUM ] ;

	// 標準３Ｄ描画用の頂点単位ライティングピクセルシェーダー
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// /* [ マルチテクスチャのブレンドモード、MV1_LAYERBLEND_TYPE_TRANSLUCENT などに +1 したもの ( 0:マルチテクスチャ無し  1:αブレンド  2:加算ブレンド  3:乗算ブレンド  4:乗算ブレンド×2 ) ] */
	// /* [ スペキュラマップ ( 0:無い 1:ある ) ] */
	// /* [ バンプマップ ( 0:なし 1:あり ) ] */
	// [ ライトインデックス ]
	// /*[ スペキュラの有無( 0:無し 1:有り ) ]*/
	// [ 特殊効果( 0:通常描画  1:乗算描画  2:RGB反転  3:描画輝度４倍  4:乗算済みαブレンドモードの通常描画  5:乗算済みαブレンドモードのRGB反転  6:乗算済みαブレンドモードの描画輝度4倍 ) ]
	// [ ピクセルシェーダーのアルファテストモード ]
	D_ID3D11PixelShader						*Base3D_VertexLighting_Normal_PS[ 2 ]/*[ 5 ]*//*[ 2 ]*//*[ 2 ]*/[ 10 ]/*[ 2 ]*/[ 7 ][ DIRECT3D11_PS_ALPHATEST_NUM ] ;
} ;


#ifndef DX_NON_MODEL

// Direct3D11 のモデル描画用のシェーダーを纏めた構造体
struct GRAPHICS_HARDWARE_DIRECT3D11_SHADER_MODEL
{
	// モデル描画用ピクセル単位ライティングタイプの頂点シェーダー( リストは配列の左から )
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// [ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ 2:8ボーン内スキニングメッシュ ) ]
	// [ バンプマップの有無( 0:無し 1:有り ) ]
	// [ フォグタイプ ]
	D_ID3D11VertexShader					*MV1_PixelLighting_VS[ 2 ][ 3 ][ 2 ][ 4 ] ;

	// モデル描画用ピクセル単位ライティングタイプのトゥーンタイプ１用ピクセルシェーダー( リストは配列の左から )
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// [ トゥーンレンダリングのディフューズグラデーションのブレンドタイプ( 0:MV1_LAYERBLEND_TYPE_TRANSLUCENT  1:MV1_LAYERBLEND_TYPE_MODULATE ) ]
	// [ トゥーンレンダリングのスペキュラグラデーションのブレンドタイプ( 0:MV1_LAYERBLEND_TYPE_TRANSLUCENT  1:MV1_LAYERBLEND_TYPE_ADDITIVE ) ]
	// [ バンプマップ ( 0:なし 1:あり ) ]
	// [ 有効なライトの数 - 1 ]
	// [ ピクセルシェーダーのアルファテストモード ]
	D_ID3D11PixelShader						*MV1_PixelLighting_ToonType1_PS[ 2 ][ 2 ][ 2 ][ 2 ][ DX_D3D11_COMMON_CONST_LIGHT_NUM ][ DIRECT3D11_PS_ALPHATEST_NUM ] ;

	// モデル描画用ピクセル単位ライティングタイプのトゥーンタイプ２用ピクセルシェーダー( リストは配列の左から )
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// [ トゥーンレンダリングのスフィアマップの有無とブレンドタイプ( 0:スフィアマップは無い   1:MV1_LAYERBLEND_TYPE_MODULATE  2:MV1_LAYERBLEND_TYPE_ADDITIVE ) ]
	// [ 有効なライトの数 - 1 ]
	// [ ピクセルシェーダーのアルファテストモード ]
	D_ID3D11PixelShader						*MV1_PixelLighting_ToonType2_PS[ 2 ][ 3 ][ DX_D3D11_COMMON_CONST_LIGHT_NUM ][ DIRECT3D11_PS_ALPHATEST_NUM ] ;

	// モデル描画用ピクセル単位ライティングタイプのピクセルシェーダー( リストは配列の左から )
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// [ マルチテクスチャのブレンドモード、MV1_LAYERBLEND_TYPE_TRANSLUCENT などに +1 したもの ( 0:マルチテクスチャ無し  1:αブレンド  2:加算ブレンド  3:乗算ブレンド  4:乗算ブレンド×2 ) ]
	// [ バンプマップ ( 0:なし 1:あり ) ]
	// [ 有効なライトの数 - 1 ]
	// [ ピクセルシェーダーのアルファテストモード ]
	D_ID3D11PixelShader						*MV1_PixelLighting_Normal_PS[ 2 ][ 5 ][ 2 ][ DX_D3D11_COMMON_CONST_LIGHT_NUM ][ DIRECT3D11_PS_ALPHATEST_NUM ] ;





	// モデル描画用のマテリアル要素描画用ピクセルシェーダー
	// [ マテリアルタイプ( DX_MATERIAL_TYPE_MAT_SPEC_LUMINANCE_UNORM など ) ]
	// [ ピクセルシェーダーのアルファテストモード ]
	D_ID3D11PixelShader						*MV1_MaterialType_PS[ DX_MATERIAL_TYPE_NUM ][ DIRECT3D11_PS_ALPHATEST_NUM ] ;






	// モデル描画用のシャドウマップへのトゥーンレンダリング輪郭線描画用頂点シェーダー
	// [ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ  2:8ボーン内スキニングメッシュ ) ]
	// [ バンプマップの有無( 0:無し 1:有り ) ]
	D_ID3D11VertexShader					*MV1_ToonOutLine_ShadowMap_VS[ 3 ][ 2 ] ;

	// モデル描画用のトゥーンレンダリングの輪郭線描画用頂点シェーダー
	// [ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ  2:8ボーン内スキニングメッシュ ) ]
	// [ バンプマップの有無( 0:無し 1:有り ) ]
	// [ フォグタイプ ]
	D_ID3D11VertexShader					*MV1_ToonOutLine_VS[ 3 ][ 2 ][ 4 ] ;

	// モデル描画用のシャドウマップへのレンダリング用頂点シェーダー
	// [ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ  2:8ボーン内スキニングメッシュ ) ]
	// [ バンプマップの有無( 0:無し 1:有り ) ]
	D_ID3D11VertexShader					*MV1_ShadowMap_VS[ 3 ][ 2 ] ;

	// モデル描画用のライティングなし頂点シェーダー
	// [ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ  2:8ボーン内スキニングメッシュ ) ]
	// [ バンプマップの有無( 0:無し 1:有り ) ]
	// [ フォグタイプ ]
	D_ID3D11VertexShader					*MV1_NoLighting_VS[ 3 ][ 2 ][ 4 ] ;

	// モデル描画用の頂点単位ライティングあり頂点シェーダー
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// [ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ  2:8ボーン内スキニングメッシュ ) ]
	// [ バンプマップの有無( 0:無し 1:有り ) ]
	// [ フォグタイプ ]
	// [ ライトインデックス ]
	D_ID3D11VertexShader					*MV1_VertexLighting_VS[ 2 ][ 3 ][ 2 ][ 4 ][ 20 ] ;

	// モデル描画用のシャドウマップへのトゥーンレンダリング用ピクセルシェーダー
	// [ トゥーンレンダリングタイプ( 0:トゥーンレンダリング type 1   1:トゥーンレンダリング type 2 ) ]
	// [ トゥーンレンダリングのスフィアマップの有無とブレンドタイプ( 0:スフィアマップは無い   1:MV1_LAYERBLEND_TYPE_MODULATE  2:MV1_LAYERBLEND_TYPE_ADDITIVE ) ]
	// [ トゥーンレンダリングのディフューズグラデーションのブレンドタイプ( 0:MV1_LAYERBLEND_TYPE_TRANSLUCENT  1:MV1_LAYERBLEND_TYPE_MODULATE ) ]
	// [ ピクセルシェーダーのアルファテストモード ]
	D_ID3D11PixelShader						*MV1_ShadowMap_Toon_PS[ 2 ][ 3 ][ 2 ][ DIRECT3D11_PS_ALPHATEST_NUM ] ;

	// モデル描画用のシャドウマップへのレンダリング用ピクセルシェーダー
	// [ ピクセルシェーダーのアルファテストモード ]
	D_ID3D11PixelShader						*MV1_ShadowMap_Normal_PS[ DIRECT3D11_PS_ALPHATEST_NUM ] ;

	// モデル描画用のライティングなしトゥーン用ピクセルシェーダー( リストは配列の左から )
	// [ トゥーンレンダリングタイプ( 0:トゥーンレンダリング type 1   1:トゥーンレンダリング type 2 ) ]
	// [ トゥーンレンダリングのディフューズグラデーションのブレンドタイプ( 0:MV1_LAYERBLEND_TYPE_TRANSLUCENT or トゥーンレンダリングではない  1:MV1_LAYERBLEND_TYPE_MODULATE ) ]
	// [ ピクセルシェーダーのアルファテストモード ]
	D_ID3D11PixelShader						*MV1_NoLighting_Toon_PS[ 2 ][ 2 ][ DIRECT3D11_PS_ALPHATEST_NUM ] ;

	// モデル描画用のライティングなしピクセルシェーダー
	// [ マルチテクスチャのブレンドモード、MV1_LAYERBLEND_TYPE_TRANSLUCENT などに +1 したもの ( 0:マルチテクスチャ無し  1:αブレンド  2:加算ブレンド  3:乗算ブレンド  4:乗算ブレンド×2 ) ]
	// [ ピクセルシェーダーのアルファテストモード ]
	D_ID3D11PixelShader						*MV1_NoLighting_Normal_PS[ 5 ][ DIRECT3D11_PS_ALPHATEST_NUM ] ;

	// モデル描画用の頂点単位ライティングありトゥーンタイプ１用ピクセルシェーダー
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// [ トゥーンレンダリングのディフューズグラデーションのブレンドタイプ( 0:MV1_LAYERBLEND_TYPE_TRANSLUCENT  1:MV1_LAYERBLEND_TYPE_MODULATE ) ]
	// [ トゥーンレンダリングのスペキュラグラデーションのブレンドタイプ( 0:MV1_LAYERBLEND_TYPE_TRANSLUCENT  1:MV1_LAYERBLEND_TYPE_ADDITIVE ) ]
	// [ バンプマップ ( 0:なし 1:あり ) ]
	// [ ライトインデックス ]
	// [ ピクセルシェーダーのアルファテストモード ]
	D_ID3D11PixelShader						*MV1_VertexLighting_ToonType1_PS[ 2 ][ 2 ][ 2 ][ 2 ][ 10 ][ DIRECT3D11_PS_ALPHATEST_NUM ] ;

	// モデル描画用の頂点単位ライティングありトゥーンタイプ２用ピクセルシェーダー
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// [ トゥーンレンダリングのスフィアマップの有無とブレンドタイプ( 0:スフィアマップは無い   1:MV1_LAYERBLEND_TYPE_MODULATE  2:MV1_LAYERBLEND_TYPE_ADDITIVE ) ]
	// [ ライトインデックス ]
	// [ ピクセルシェーダーのアルファテストモード ]
	D_ID3D11PixelShader						*MV1_VertexLighting_ToonType2_PS[ 2 ][ 3 ][ 10 ][ DIRECT3D11_PS_ALPHATEST_NUM ] ;

	// モデル描画用の頂点単位ライティングありピクセルシェーダー
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// [ マルチテクスチャのブレンドモード、MV1_LAYERBLEND_TYPE_TRANSLUCENT などに +1 したもの ( 0:マルチテクスチャ無し  1:αブレンド  2:加算ブレンド  3:乗算ブレンド  4:乗算ブレンド×2 ) ]
	// [ バンプマップ ( 0:なし 1:あり ) ]
	// [ ライトインデックス ]
	// [ ピクセルシェーダーのアルファテストモード ]
	D_ID3D11PixelShader						*MV1_VertexLighting_Normal_PS[ 2 ][ 5 ][ 2 ][ 10 ][ DIRECT3D11_PS_ALPHATEST_NUM ] ;
} ;
#endif // DX_NON_MODEL

// Direct3D11 の定数情報を纏めた構造体
struct GRAPHICS_HARDWARE_DIRECT3D11_CONSTANT
{
	CONSTANTBUFFER_DIRECT3D11_MANAGE			ConstantBufferManage ;			// 定数バッファ情報

	CONSTANTBUFFER_DIRECT3D11 *					ConstBuffer_Common ;			// 頂点シェーダー＆ピクセルシェーダー共通のパラメータ用定数バッファ( DX_D3D11_CONST_BUFFER_COMMON 構造体 )
	int											SetNormalMaterial ;				// 定数バッファに標準描画用のマテリアルパラメータが設定されているかどうか( TRUE:設定されている  FALSE:設定されていない )

	CONSTANTBUFFER_DIRECT3D11 *					ConstBuffer_VS_Base ;			// 頂点シェーダー基本パラメータ用定数バッファ( DX_D3D11_VS_CONST_BUFFER_BASE 構造体 )
	int											SetNormalColorSource ;			// 定数バッファに標準描画用の「ディフューズカラー・スペキュラカラーをマテリアルを使用するか、頂点データを使用するか」の設定がされているかどうか( TRUE:設定されている  FALSE:設定されていない )

	CONSTANTBUFFER_DIRECT3D11 *					ConstBuffer_VS_OtherMatrix ;	// 頂点シェーダーその他行列パラメータ用定数バッファ( DX_D3D11_VS_CONST_BUFFER_OTHERMATRIX 構造体 )
	int											SetNormalTextureAddressTransformMatrix ;	// 定数バッファに標準描画用のテクスチャ座標用行列が設定されているかどうか( TRUE:設定されている  FALSE:設定されていない )

	CONSTANTBUFFER_DIRECT3D11 *					ConstBuffer_VS_LocalWorldMatrix ;	// 頂点シェーダースキニングメッシュ用の　ローカル　→　ワールド行列パラメータ用定数バッファ( DX_D3D11_VS_CONST_BUFFER_LOCALWORLDMATRIX 構造体 )

	CONSTANTBUFFER_DIRECT3D11 *					ConstBuffer_PS_Base ;			// ピクセルシェーダー基本パラメータ用定数バッファ( DX_D3D11_PS_CONST_BUFFER_BASE 構造体 )
	CONSTANTBUFFER_DIRECT3D11 *					ConstBuffer_PS_ShadowMap ;		// ピクセルシェーダーシャドウマップ用定数バッファ( DX_D3D11_PS_CONST_BUFFER_SHADOWMAP 構造体 )
	CONSTANTBUFFER_DIRECT3D11 *					ConstBuffer_PS_Filter ;			// ピクセルシェーダーフィルター用定数バッファ
} ;

// Direct3D11 のシェーダー情報を纏めた構造体
struct GRAPHICS_HARDWARE_DIRECT3D11_SHADER
{
	GRAPHICS_HARDWARE_DIRECT3D11_CONSTANT		Constant ;						// 定数情報を纏めた構造体

	GRAPHICS_HARDWARE_DIRECT3D11_SHADER_BASE	Base ;							// 標準描画処理用のシェーダーを纏めた構造体
	GRAPHICS_HARDWARE_DIRECT3D11_SHADER_BASE3D	Base3D ;						// 標準３Ｄ描画処理用のシェーダーを纏めた構造体

#ifndef DX_NON_MODEL
	GRAPHICS_HARDWARE_DIRECT3D11_SHADER_MODEL	Model ;							// モデル描画処理用のシェーダーを纏めた構造体
#endif // DX_NON_MODEL
} ;



// Direct3D11 の入力レイアウトを纏めた構造体
struct GRAPHICS_HARDWARE_DIRECT3D11_INPUTLAYOUT
{
	// 標準描画用の頂点シェーダーで使用する頂点データフォーマット
	D_ID3D11InputLayout *						BaseInputLayout[ D3D11_VERTEX_INPUTLAYOUT_NUM ] ;

	// 単純転送用の頂点シェーダーで使用する頂点データフォーマット
	D_ID3D11InputLayout *						BaseStretchRectInputLayout ;
	D_ID3D11InputLayout *						BaseStretchRectTex8InputLayout ;

#ifndef DX_NON_MODEL
	// 入力レイアウト( リストは配列の左から )
	// [ バンプマップ情報付きかどうか( 1:バンプマップ付き 0:付いてない ) ]
	// [ スキニングメッシュかどうか( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ 2:8ボーン内スキニングメッシュ ) ]
	D_ID3D11InputLayout *						MV1_VertexInputLayout[ 2 ][ 3 ] ;
#endif // DX_NON_MODEL
} ;






// D3D11Device に設定している値を纏めた構造体
struct GRAPHICS_HARDDATA_DIRECT3D11_DEVICE_STATE
{
	int										FogEnable ;										// フォグが有効かどうか( TRUE:有効  FALSE:無効 )
	int										FogMode ;										// フォグモード
	DWORD									FogColor ;										// フォグカラー
	float									FogStart ;										// フォグ開始距離
	float									FogEnd ;										// フォグ終了距離
	float									FogDensity ;									// フォグ密度

	COLOR_F									GlobalAmbientLightColor ;						// グローバルアンビエントライトカラー
	int										LightEnableMaxIndex ;							// 有効なライトの最大インデックス
	int										LightEnableFlag[ DX_D3D11_COMMON_CONST_LIGHT_NUM ] ;// ライトが有効かどうかフラグ( TRUE:有効  FALSE:無効 )
	LIGHTPARAM								LightParam[ DX_D3D11_COMMON_CONST_LIGHT_NUM ] ;	// ライトのパラメータ
	int										Lighting ;										// ライティングを行うかどうかのフラグ( TRUE:ライティングを行う  FALSE:ライティングなし )

	int										UseSpecular ;									// スペキュラを使用するかどうか
	MATERIALPARAM							Material ;										// マテリアル
	float									MaterialTypeParam[ MATERIAL_TYPEPARAM_MAX_NUM ] ;	// マテリアル別タイプパラメータ
	int										MaterialUseVertexDiffuseColor ;					// 頂点のディフューズカラーをマテリアルのディフューズカラーとして使用するかどうか
	int										MaterialUseVertexSpecularColor ;				// 頂点のスペキュラカラーをマテリアルのスペキュラカラーとして使用するかどうか

	int										DrawMode ;										// 描画モード( DX_DRAWMODE_BILINEAR など )、標準のフィルタリングモードではない場合は -1
	int										MaxAnisotropy ;									// 最大異方性、全てのサンプラに対して同じ設定が行われていない場合は -1
	int										ChangeSamplerDesc ;								// デバイスのサンプラーステートの設定に変更があったかどうか( TRUE:変更があった  FALSE:変更はない )
	int										ChangeSamplerDescSlot[ USE_TEXTURESTAGE_NUM ] ;	// デバイスの各スロットのサンプラーステートの設定に変更があったかどうか( TRUE:変更があった  FALSE:変更はない )
	D_D3D11_SAMPLER_DESC					SamplerDesc[ USE_TEXTURESTAGE_NUM ] ;			// デバイスの各スロットのサンプラーステートの設定値
	D_ID3D11SamplerState *					SamplerState[ USE_TEXTURESTAGE_NUM ] ;			// 現在デバイスの各スロットに設定されているサンプラーステート
	int										ChangeSamplerStateReleaseFlag[ USE_TEXTURESTAGE_NUM ] ;					// 現在デバイスの各スロットに設定されているサンプラーステートを変更する際にリリースするかどうかのフラグ( TRUE:リリースする  FALSE:リリースしない )
	D_ID3D11SamplerState *					DefaultSamplerStateArray[ DIRECT3D11_DEFAULTRS_FILTER_NUM ][ D_D3D11_TEXTURE_ADDRESS_MIRROR_ONCE + 1 ][ D_D3D11_TEXTURE_ADDRESS_MIRROR_ONCE + 1 ][ D_D3D11_TEXTURE_ADDRESS_MIRROR_ONCE + 1 ][ 3 /* MaxAnisotropy  要素番号0 = 1  要素番号1 = 2  要素番号2 = 16 */ ] ;	// よく使う設定のサンプラーステートの配列

	D_D3D11_VIEWPORT						Viewport ;										// ビューポート情報

	int										ChangeRasterizerDesc ;							// デバイスのラスタライザーステートの設定に変更があったかどうか( TRUE:変更があった  FALSE:変更はない )
	D_D3D11_RASTERIZER_DESC					RasterizerDesc ;								// 現在デバイスに設定されているラスタライザーステートの設定値
	D_ID3D11RasterizerState *				RasterizerState ;								// 現在デバイスに設定されているラスタライザーステート
	int										ChangeRasterizerStateReleaseFlag ;				// 現在デバイスに設定されているラスタライザーステートを変更する際にリリースするかどうかのフラグ( TRUE:リリースする  FALSE:リリースしない )
	D_ID3D11RasterizerState *				DefaultRasterizerStateArray[ D_D3D11_CULL_BACK + 1 ][ D_D3D11_FILL_SOLID + 1 ][ 2 /* 深度テストの有無 */ ][ 2 /* マルチサンプルの有無 */ ] ;	// よく使う設定のラスタライザーステートの配列

	int										ChangeDepthStencilDesc ;						// デバイスの深度ステンシルステートの設定に変更があったかどうか( TRUE:変更があった  FALSE:変更はない )
	D_D3D11_DEPTH_STENCIL_DESC				DepthStencilDesc ;								// 現在デバイスに設定されている深度ステンシルテストステートの設定値
	D_ID3D11DepthStencilState *				DepthStencilState ;								// 現在デバイスに設定されている深度ステンシルステート
	int										ChangeDepthStencilStateReleaseFlag ;			// 現在デバイスに設定されている深度ステンシルステートを変更する際にリリースするかどうかのフラグ( TRUE:リリースする  FALSE:リリースしない )
	D_ID3D11DepthStencilState *				DefaultDepthStencilStateArray[ 2 /* 深度テストの有無 */ ][ 2 /* 深度バッファへの書き込みの有無 */ ][ D_D3D11_COMPARISON_ALWAYS + 1 ] ;	// よく使う設定の深度ステンシルステートの配列

	int										BlendMode ;										// 現在デバイスに設定されているブレンドモード、プリセットのブレンドモード以外の場合は -1 が入る
	int										ChangeBlendDesc ;								// デバイスのブレンディングステートの設定に変更があったかどうか( TRUE:変更があった  FALSE:変更はない )
	D_D3D11_BLEND_DESC						BlendDesc ;										// 現在デバイスに設定されているブレンディングステートの設定値
	D_ID3D11BlendState *					BlendState ;									// 現在デバイスに設定されているブレンディングステート
	int										ChangeBlendStateReleaseFlag ;					// 現在デバイスに設定されているブレンディングステートを変更する際にリリースするかどうかのフラグ( TRUE:リリースする  FALSE:リリースしない )
	D_ID3D11BlendState *					DefaultBlendStateArray[ DX_BLENDMODE_NUM ] ;	// よく使う設定のブレンディングステートの配列

	D_ID3D11ShaderResourceView *			PSSetShaderResourceView[ D_D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT ] ;		// 現在デバイスに設定されているピクセルシェーダーリソースビュー

	RECT									ScissorRect ;									// シザー矩形
	int										AlphaTestEnable ;								// アルファテストを使用するかどうか
	int										AlphaTestCmpMode ;								// アルファテストで使用する比較モード( DX_CMP_NEVER など )
	int										AlphaTestRef ;									// アルファテストで使用する参照値
	int										AlphaTestModeShaderIndex ;						// アルファテストの設定値に対応するピクセルシェーダーのアルファテストモード( DIRECT3D11_PS_ALPHATEST_CMP_GREATER など )

	D_D3D11_PRIMITIVE_TOPOLOGY				PrimitiveTopology ;								// デバイスに設定されているプリミティブタイプ

	D_ID3D11Buffer *						SetPixelShaderConstantBuffer[  D_D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT ] ;	// 現在 ID3D11DeviceContext にピクセルシェーダー用にセットしてある定数バッファ
	D_ID3D11Buffer *						SetVertexShaderConstantBuffer[ D_D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT ] ;	// 現在 ID3D11DeviceContext に頂点シェーダー用にセットしてある定数バッファ

	int										SetNormalPixelShader ;							// 通常描画用ピクセルシェーダーがセットされているかどうか
	D_ID3D11PixelShader *					SetPixelShader ;								// 現在 ID3D11DeviceContext にセットしてあるピクセルシェーダー

	int										SetNormalVertexShader ;							// 通常描画用頂点シェーダーがセットされているかどうか
	D_ID3D11VertexShader *					SetVertexShader ;								// 現在 ID3D11DeviceContext にセットしてある頂点シェーダー

	D_ID3D11Buffer *						SetIndexBuffer ;								// 現在 ID3D11DeviceContext にセットしてあるインデックスバッファ
	D_DXGI_FORMAT							SetIndexBufferFormat ;							// 現在 ID3D11DeviceContext にセットしてあるインデックスバッファのフォーマット

	D_ID3D11InputLayout *					SetVertexInputLayout ;							// 現在 ID3D11DeviceContext にセットしてある入力レイアウト

	D_ID3D11Buffer *						SetVertexBuffer ;								// 現在 ID3D11DeviceContext にセットしてある頂点バッファ
	int										SetVertexBufferStride ;							// 現在 ID3D11DeviceContext にセットしてある頂点バッファのストライド

	D_ID3D11Texture2D *						TargetTexture2D[ DX_RENDERTARGET_COUNT ] ;		// 描画対象のテクスチャ
	D_D3D11_TEXTURE2D_DESC					TargetTexture2DDesc[ DX_RENDERTARGET_COUNT ] ;	// 描画対象のテクスチャの情報
	D_ID3D11RenderTargetView *				TargetRTV[ DX_RENDERTARGET_COUNT ] ;			// 描画対象のレンダリングターゲットビュー

	D_ID3D11Texture2D *						TargetDepthTexture2D ;							// 描画対象の深度テクスチャ
	D_D3D11_TEXTURE2D_DESC					TargetDepthTexture2DDesc ;						// 描画対象の深度テクスチャの情報
	D_ID3D11DepthStencilView *				TargetDepthDSV ;								// 描画対象の深度ステンシルビュー

	int										TextureAddressTransformMatrixUse ;				// テクスチャ座標変換行列を使用するかどうか( TRUE:使用する  FALSE:使用しない )
	MATRIX									TextureAddressTransformMatrix ;					// テクスチャ座標変換行列
} ;


// D3D11Device の描画処理に関わる情報の構造体
struct GRAPHICS_HARDDATA_DIRECT3D11_DRAWINFO
{
	BYTE						VertexBufferTemp[ D3D11_VERTEXBUFFER_SIZE ] ;	// システムメモリ側の頂点バッファ
	BYTE *						VertexBufferAddr ;								// システムメモリ側の頂点バッファ
	BYTE *						VertexBufferNextAddr ;							// 次のデータを格納すべき頂点バッファ内のアドレス
	D_ID3D11Buffer *			VertexBuffer ;									// 描画用 ID3D11Buffer
	int							VertexNum ; 									// 頂点バッファに格納されている頂点の数
	int							Use3DVertex ;									// ３Ｄ頂点を使用しているかどうか( 1:使用している  0:使用していない )
	int							VertexType ;									// 使用する頂点タイプ( VERTEXTYPE_TEX 等 )
	D_D3D11_PRIMITIVE_TOPOLOGY	PrimitiveType ;									// 頂点バッファに格納されている頂点データのプリミティブタイプ

	DWORD						DiffuseColor ;									// ディフューズカラー

	int							BeginSceneFlag ;								// Graphics_D3D11_BeginScene を実行してあるかどうか
	int							BlendMaxNotDrawFlag ;							// ブレンド値が最大であることにより描画を行わないかどうか、フラグ

	D_ID3D11Buffer *			CommonVertexBuffer ;							// 汎用描画用頂点バッファ
	int							CommonVertexBufferSize ;						// 汎用描画用頂点バッファのサイズ
	D_ID3D11Buffer *			CommonIndexBuffer ;								// 汎用描画用インデックスバッファ
	int							CommonIndexBufferSize ;							// 汎用描画用インデックスバッファのサイズ
} ;

// D3D11Device の出力先ウインドウ一つあたりの情報の構造体
struct GRAPHICS_HARDDATA_DIRECT3D11_OUTPUTWINDOWINFO
{
	int								UseFlag ;								// 配列を使用しているか( TRUE:使用している  FALSE:使用していない )

	D_IDXGISwapChain *				DXGISwapChain ;							// DXGISwapChain
	D_DXGI_SWAP_CHAIN_DESC			DXGISwapChainDesc ;						// DXGISwapChain情報
	D_IDXGIOutput *					DXGIOutput ;							// DXGIOutput

	D_ID3D11Texture2D *				BufferTexture2D ;						// スワップチェインのバックバッファテクスチャ
	D_D3D11_TEXTURE2D_DESC			BufferTexture2DDesc ;					// スワップチェインのバックバッファテクスチャ情報
	D_ID3D11ShaderResourceView *	BufferSRV ;								// スワップチェインのバックバッファシェーダーリソースビュー
	D_ID3D11RenderTargetView *		BufferRTV ;								// スワップチェインのバックバッファレンダーターゲットビュー
} ;


// D3D11Device の画面関係の情報の構造体
struct GRAPHICS_HARDDATA_DIRECT3D11_SCREENINFO
{
	int												OutputWindowNum ;									// 出力先ウインドウの数
	GRAPHICS_HARDDATA_DIRECT3D11_OUTPUTWINDOWINFO	OutputWindowInfo[ DX_D3D11_MAX_OUTPUTWINDOW ] ;		// 出力先ウインドウの情報
	int												TargetOutputWindow ;								// 現在の出力先ウインドウの番号
	int												FullscreenSatte ;									// フルスクリーンモードにしているか( TRUE:している  FALSE:していない )
//	D_ID3D11Texture2D *				BackBufferTexture2D ;
//	D_ID3D11ShaderResourceView *	BackBufferSRV ;
//	D_ID3D11RenderTargetView *		BackBufferRTV ;
//	D_D3D11_TEXTURE2D_DESC			BackBufferTexture2DDesc ;

	int								SubBackBufferTextureSizeX ;				// サブバックバッファテクスチャの横幅
	int								SubBackBufferTextureSizeY ;				// サブバックバッファテクスチャの縦幅
	D_ID3D11Texture2D *				SubBackBufferTexture2D ;				// ScreenCopy や GetDrawScreen を実現するために使用する描画可能テクスチャ
	D_ID3D11RenderTargetView *		SubBackBufferRTV ;						// ScreenCopy や GetDrawScreen を実現するために使用する描画可能テクスチャ用のレンダーターゲットビュー
	D_ID3D11ShaderResourceView *	SubBackBufferSRV ;						// ScreenCopy や GetDrawScreen を実現するために使用する描画可能テクスチャ用のシェーダーリソースビュー

	D_ID3D11Texture2D *				DepthBufferTexture2D ;
	D_ID3D11DepthStencilView *		DepthBufferDSV ;
//	D_ID3D11ShaderResourceView *	DepthBufferSRV ;
	int								DepthBufferSizeX ;						// 深度バッファサーフェスの幅
	int								DepthBufferSizeY ;						// 深度バッファサーフェスの高さ
	int								DepthBufferBitDepth ;					// 深度バッファサーフェスのビット深度

	D_ID3D11Texture2D *				DrawScreenBufferLockSMTexture ;			// 描画対象バッファをロックした際に使用するテクスチャのポインタ
} ;


// D3D11Device の描画設定に関わる情報の構造体
struct GRAPHICS_HARDDATA_DIRECT3D11_DRAWSETTING
{
	int								DrawPrepParamFlag ;						// 前回 Graphics_D3D11_DrawPreparation に入ったときの ParamFlag パラメータ
	int								DrawPrepAlwaysFlag ;					// 必ず Graphics_D3D11_DrawPreparation を行うかどうかのフラグ

	int								AlphaChannelValidFlag ;					// αチャンネル有効フラグ
	int								AlphaTestValidFlag ;					// αテスト有効フラグ( Func は必ず D_D3DCMP_GREATEREQUAL )
	int								AlphaTestMode ;							// アルファテストモード
	int								AlphaTestParam ;						// アルファテストパラメータ
	int								BlendMode ;								// ブレンドモード
	int								ChangeBlendParamFlag ;					// ブレンド設定に関わる部分の変更があったか、フラグ
	int								ChangeTextureFlag ;						// テクスチャが変更されたか、フラグ
	int								IgnoreGraphColorFlag ;					// 描画する画像のＲＧＢ成分を無視するかどうかのフラグ
	int								IgnoreGraphAlphaFlag ;					// 描画する画像のＡ成分を無視するかどうかのフラグ
//	int								UseDiffuseRGBColor ;					// ディフューズカラーのＲＧＢ値を使用するか、フラグ
//	int								UseDiffuseAlphaColor ;					// ディフューズカラーのα値を使用するか、フラグ
	int								BlendGraphType ;						// ブレンド画像タイプ
	int								BlendGraphFadeRatio ;					// ブレンド画像のフェードパラメータ
	int								BlendGraphBorderParam ;					// ブレンド画像の境界パラメータ(０(ブレンド画像の影響０)　←　(ブレンド画像の影響少ない)　←　１２８(ブレンド画像の影響１００％)　→　(ブレンド画像の影響を超えて非描画部分が増える)　→２５５(全く描画されない) )
	int								BlendGraphBorderRange ;					// ブレンド画像の境界幅(０〜２５５　狭い〜広い　しかし４段階)
	float							BlendTextureWidth ;						// ブレンドテクスチャの幅
	float							BlendTextureHeight ;					// ブレンドテクスチャの高さ
	float							InvBlendTextureWidth ;					// ブレンドテクスチャの幅の逆数
	float							InvBlendTextureHeight ;					// ブレンドテクスチャの高さの逆数

	D_ID3D11Texture2D *				RenderTexture ;							// 描画時に使用するテクスチャー
	D_ID3D11ShaderResourceView *	RenderTextureSRV ;						// 描画時に使用するテクスチャーのシェーダーリソースビュー
	D_ID3D11Texture2D *				BlendTexture ;							// 描画時に描画テクスチャーとブレンドするαチャンネルテクスチャー
	D_ID3D11ShaderResourceView *	BlendTextureSRV ;						// 描画時に描画テクスチャーとブレンドするαチャンネルテクスチャーのシェーダーリソースビュー

	int								CancelSettingEqualCheck ;				// このフラグが立っている場合は現在の設定と変更後の設定が同じかどうかに関わらず、必ず変更処理を実行する
} ;


// D3D11Device 関係の設定情報の構造体
struct GRAPHICS_HARDDATA_DIRECT3D11_DEVICE_SETTING
{
	void					( *DeviceRestoreCallbackFunction)( void *Data ) ;	// デバイスロストから復帰したときに呼ぶ関数
	void					  *DeviceRestoreCallbackData ;						// デバイスロストから復帰したときに呼ぶ関数に渡すポインタ

	void					( *DeviceLostCallbackFunction)( void *Data ) ;		// デバイスロストから復帰する前に呼ぶ関数
	void					  *DeviceLostCallbackData ;							// デバイスロストから復帰する前に呼ぶ関数に渡すポインタ
} ;


// D3D11Device の能力情報構造体
struct GRAPHICS_HARDDATA_DIRECT3D11_DEVICE_CAPS
{
	D_DXGI_FORMAT					ScreenFormat ;										// 画面カラーフォーマット
	D_DXGI_FORMAT					TextureFormat[ DX_GRAPHICSIMAGE_FORMAT_3D_NUM ] ;	// テクスチャフォーマット
	D_DXGI_FORMAT					MaskColorFormat ;									// マスクカラーバッファ用フォーマット
	D_DXGI_FORMAT					MaskAlphaFormat ;									// マスクアルファチャンネル用フォーマット
	D_DXGI_FORMAT					DepthBufferTexture2DFormat[ ZBUFFER_FORMAT_NUM ] ;		// 深度バッファ用Texture2Dフォーマット
	D_DXGI_FORMAT					DepthBufferDepthStencilFormat[ ZBUFFER_FORMAT_NUM ] ;	// 深度バッファ用DepthStencilフォーマット
} ;


// Direct3D11 を使った描画処理で使用するデータの内の D3D11Device のリセットで
// 情報を初期化する必要がある、D3D11Device に密接に関わった情報を纏めた構造体
struct GRAPHICS_HARDDATA_DIRECT3D11_DEVICE
{
	GRAPHICS_HARDDATA_DIRECT3D11_DEVICE_CAPS		Caps ;					// デバイス能力情報構造体		

	GRAPHICS_HARDDATA_DIRECT3D11_DEVICE_SETTING		Setting ;				// デバイス関係の設定情報を纏めた構造体

	GRAPHICS_HARDDATA_DIRECT3D11_SCREENINFO			Screen ;				// 画面関係の情報構造体

	GRAPHICS_HARDWARE_DIRECT3D11_SHADER				Shader ;				// シェーダーを纏めた構造体

	GRAPHICS_HARDWARE_DIRECT3D11_INPUTLAYOUT		InputLayout ;			// 入力レイアウトを纏めた構造体

	GRAPHICS_HARDDATA_DIRECT3D11_DRAWSETTING		DrawSetting ;			// 描画設定に関わる情報を纏めた構造体

	GRAPHICS_HARDDATA_DIRECT3D11_DRAWINFO			DrawInfo ;				// 描画処理に関わる情報を纏めた構造体

	GRAPHICS_HARDDATA_DIRECT3D11_DEVICE_STATE		State ;					// D3D11Device に設定している値を纏めた構造体
} ;













// シェーダーコード一つの情報構造体
struct GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO
{
	void					*Binary ;								// シェーダーコードバイナリのアドレス
	int						Size ;									// シェーダーコードバイナリのデータサイズ
} ;


// Direct3D11 の 標準描画用のシェーダーコードの元データを纏めた構造体
struct GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_BASE
{
	int						BaseShaderInitializeFlag ;				// シェーダーバイナリのセットアップが完了しているかどうかのフラグ( TRUE:完了している  FALSE:完了していない )

	// ライトインデックスリスト
	short					LightIndexList84[ 4 ][ 4 ][ 4 ][ 4 ][ 4 ][ 4 ] ;
	short					LightIndexList20[ 4 ][ 4 ][ 4 ] ;
	short					LightIndexList10[ 3 ][ 3 ][ 3 ] ;

	void					*Base2DShaderPackageImage ;				// 標準２Ｄ描画用シェーダーパッケージバイナリイメージ

	// 標準描画用の複雑な処理を行わない頂点シェーダー
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO BaseSimple_VS_Code[ D3D11_VERTEX_INPUTLAYOUT_NUM ] ;

	// 固定機能パイプライン互換のピクセルシェーダー( テクスチャなし )
	// [ 特殊効果( 0:通常描画  1:乗算描画  2:RGB反転  3:描画輝度４倍  4:乗算済みαブレンドモードの通常描画  5:乗算済みαブレンドモードのRGB反転  6:乗算済みαブレンドモードの描画輝度4倍 ) ]
	// [ ピクセルシェーダーのアルファテストモード ]
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO BaseNoneTex_PS_Code[ 7 ][ DIRECT3D11_PS_ALPHATEST_NUM ] ;

	// 固定機能パイプライン互換のピクセルシェーダー( テクスチャあり )
	// [ ブレンド画像とのブレンドタイプ( 0:なし  1:DX_BLENDGRAPHTYPE_NORMAL  2:DX_BLENDGRAPHTYPE_WIPE  3:DX_BLENDGRAPHTYPE_ALPHA ) ]
	// [ 特殊効果( 0:通常描画  1:乗算描画  2:RGB反転  3:描画輝度４倍  4:乗算済みαブレンドモードの通常描画  5:乗算済みαブレンドモードのRGB反転  6:乗算済みαブレンドモードの描画輝度4倍 ) ]
	// [ テクスチャRGB無視( 0:無視しない  1:無視する ) ]
	// [ テクスチャAlpha無視( 0:無視しない  1:無視する ) ]
	// [ ピクセルシェーダーのアルファテストモード ]
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO BaseUseTex_PS_Code[ 4 ][ 7 ][ 2 ][ 2 ][ DIRECT3D11_PS_ALPHATEST_NUM ] ;

	// マスク処理用のピクセルシェーダー
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO MaskEffect_PS_Code ;
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO MaskEffect_ReverseEffect_PS_Code ;
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO MaskEffect_UseGraphHandle_PS_Code[ 4 ] ;
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO MaskEffect_UseGraphHandle_ReverseEffect_PS_Code[ 4 ] ;

	// 単純転送用シェーダー
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO StretchRect_VS_Code ;
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO StretchRectTex8_VS_Code ;
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO StretchRect_PS_Code ;

//	DXARC					BaseShaderBinDxa ;						// 標準描画用シェーダーオブジェクトファイルＤＸＡ構造体
//	void					*BaseShaderBinDxaImage ;				// 標準描画用シェーダーオブジェクトファイルＤＸＡのバイナリイメージ

#ifndef DX_NON_FILTER
	DXARC					FilterShaderBinDxa ;					// フィルターシェーダーオブジェクトファイルＤＸＡ構造体
	void					*FilterShaderBinDxaImage ;				// フィルターシェーダーオブジェクトファイルＤＸＡのバイナリイメージ

	void					*RGBAMixS_ShaderPackImage ;				// RGBAMix の S だけの組み合わせ２５６個のシェーダーパッケージバイナリイメージ

	// RGBAMix の S だけの組み合わせ２５６個のシェーダー[ R ][ G ][ B ][ A ][ 0:通常用  1:乗算済みアルファ用 ]
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO RGBAMixS_PS_Code[ 4 ][ 4 ][ 4 ][ 4 ][ 2 ] ;
#endif // DX_NON_FILTER
} ;

// Direct3D11 の 標準３Ｄ描画用のシェーダーコードの元データを纏めた構造体
struct GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_BASE3D
{
	volatile int			Base3DShaderInitializeFlag ;			// シェーダーバイナリのセットアップが完了しているかどうかのフラグ( TRUE:完了している  FALSE:完了していない )

	void					*Base3DShaderPackageImage ;				// 標準３Ｄ描画用シェーダーパッケージバイナリイメージ

	// 標準３Ｄ描画用ピクセル単位ライティングタイプの頂点シェーダー( リストは配列の左から )
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// /* [ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ 2:8ボーン内スキニングメッシュ ) ] */
	// /* [ バンプマップの有無( 0:無し 1:有り ) ] */
	// [ フォグタイプ ]
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO Base3D_PixelLighting_VS_Code[ 2 ]/*[ 3 ]*//*[ 2 ]*/[ 4 ] ;

	// 標準３Ｄ描画用ピクセル単位ライティングタイプのピクセルシェーダー( リストは配列の左から )
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// /*[ マルチテクスチャのブレンドモード、MV1_LAYERBLEND_TYPE_TRANSLUCENT などに +1 したもの ( 0:マルチテクスチャ無し  1:αブレンド  2:加算ブレンド  3:乗算ブレンド  4:乗算ブレンド×2 ) ]*/
	// /*[ スペキュラマップ ( 0:無い 1:ある ) ]*/
	// /*[ バンプマップ ( 0:なし 1:あり ) ]*/
	// [ ライトインデックス ]
	// /*[ スペキュラの有無( 0:無し 1:有り ) ]*/
	// [ 特殊効果( 0:通常描画  1:乗算描画  2:RGB反転  3:描画輝度４倍  4:乗算済みαブレンドモードの通常描画  5:乗算済みαブレンドモードのRGB反転  6:乗算済みαブレンドモードの描画輝度4倍 ) ]
	// [ ピクセルシェーダーのアルファテストモード ]
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO Base3D_PixelLighting_Normal_PS_Code[ 2 ]/*[ 5 ]*//*[ 2 ]*//*[ 2 ]*/[ 84 ]/*[ 2 ]*/[ 7 ][ DIRECT3D11_PS_ALPHATEST_NUM ] ;



	// 標準３Ｄ描画用のシャドウマップへのレンダリング用頂点シェーダー
	// /*[ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ  2:8ボーン内スキニングメッシュ ) ]*/
	// [ バンプマップの有無( 0:無し 1:有り ) ]
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO Base3D_ShadowMap_VS_Code/*[ 3 ]*/[ 2 ] ;

	// 標準３Ｄ描画用のライティングなし頂点シェーダー
	// /*[ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ  2:8ボーン内スキニングメッシュ ) ]*/
	// [ フォグタイプ ]
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO Base3D_NoLighting_VS_Code/*[ 3 ]*/[ 4 ] ;

	// 標準３Ｄ描画用の頂点単位ライティング頂点シェーダー
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// /*[ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ  2:8ボーン内スキニングメッシュ ) ]*/
	// /*[ バンプマップの有無( 0:無し 1:有り ) ]*/
	// [ フォグタイプ ]
	// [ ライトインデックス ]
	// /*[ スペキュラの有無( 0:無し 1:有り ) ]*/
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO Base3D_VertexLighting_VS_Code[ 2 ]/*[ 3 ]*//*[ 2 ]*/[ 4 ][ 20 ]/*[ 2 ]*/ ;

	// 標準３Ｄ描画用のシャドウマップへのレンダリング用ピクセルシェーダー
	// [ ピクセルシェーダーのアルファテストモード ]
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO Base3D_ShadowMap_Normal_PS_Code[ DIRECT3D11_PS_ALPHATEST_NUM ] ;

	// 標準３Ｄ描画用のライティングなしピクセルシェーダー
	// /*[ マルチテクスチャのブレンドモード、MV1_LAYERBLEND_TYPE_TRANSLUCENT などに +1 したもの ( 0:マルチテクスチャ無し  1:αブレンド  2:加算ブレンド  3:乗算ブレンド  4:乗算ブレンド×2 ) ]*/
	// [ 特殊効果( 0:通常描画  1:乗算描画  2:RGB反転  3:描画輝度４倍  4:乗算済みαブレンドモードの通常描画  5:乗算済みαブレンドモードのRGB反転  6:乗算済みαブレンドモードの描画輝度4倍 ) ]
	// [ ピクセルシェーダーのアルファテストモード ]
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO Base3D_NoLighting_Normal_PS_Code/*[ 5 ]*/[ 7 ][ DIRECT3D11_PS_ALPHATEST_NUM ] ;

	// 標準３Ｄ描画用の頂点単位ライティングピクセルシェーダー
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// /* [ マルチテクスチャのブレンドモード、MV1_LAYERBLEND_TYPE_TRANSLUCENT などに +1 したもの ( 0:マルチテクスチャ無し  1:αブレンド  2:加算ブレンド  3:乗算ブレンド  4:乗算ブレンド×2 ) ] */
	// /* [ スペキュラマップ ( 0:無い 1:ある ) ] */
	// /* [ バンプマップ ( 0:なし 1:あり ) ] */
	// [ ライトインデックス ]
	// /*[ スペキュラの有無( 0:無し 1:有り ) ]*/
	// [ 特殊効果( 0:通常描画  1:乗算描画  2:RGB反転  3:描画輝度４倍  4:乗算済みαブレンドモードの通常描画  5:乗算済みαブレンドモードのRGB反転  6:乗算済みαブレンドモードの描画輝度4倍 ) ]
	// [ ピクセルシェーダーのアルファテストモード ]
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO Base3D_VertexLighting_Normal_PS_Code[ 2 ]/*[ 5 ]*//*[ 2 ]*//*[ 2 ]*/[ 10 ]/*[ 2 ]*/[ 7 ][ DIRECT3D11_PS_ALPHATEST_NUM ] ;
} ;

#ifndef DX_NON_MODEL

// Direct3D11 の モデル描画用のシェーダーコードの元データを纏めた構造体
struct GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_MODEL
{
	volatile int			ModelShaderInitializeFlag ;				// モデル用のシェーダーバイナリのセットアップが完了しているかどうかのフラグ( TRUE:完了している  FALSE:完了していない )

	void					*ModelShaderPackImage ;					// ３Ｄモデル用シェーダーパッケージバイナリイメージ

	// モデル描画用ピクセル単位ライティングタイプの頂点シェーダー( リストは配列の左から )
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// [ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ 2:8ボーン内スキニングメッシュ ) ]
	// [ バンプマップの有無( 0:無し 1:有り ) ]
	// [ フォグタイプ ]
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO MV1_PixelLighting_VS_Code[ 2 ][ 3 ][ 2 ][ 4 ] ;

	// モデル描画用ピクセル単位ライティングタイプのトゥーンタイプ１用ピクセルシェーダー( リストは配列の左から )
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// [ トゥーンレンダリングのディフューズグラデーションのブレンドタイプ( 0:MV1_LAYERBLEND_TYPE_TRANSLUCENT  1:MV1_LAYERBLEND_TYPE_MODULATE ) ]
	// [ トゥーンレンダリングのスペキュラグラデーションのブレンドタイプ( 0:MV1_LAYERBLEND_TYPE_TRANSLUCENT  1:MV1_LAYERBLEND_TYPE_ADDITIVE ) ]
	// [ バンプマップ ( 0:なし 1:あり ) ]
	// [ 有効なライトの数 - 1 ]
	// [ ピクセルシェーダーのアルファテストモード ]
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO MV1_PixelLighting_ToonType1_PS_Code[ 2 ][ 2 ][ 2 ][ 2 ][ DX_D3D11_COMMON_CONST_LIGHT_NUM ][ DIRECT3D11_PS_ALPHATEST_NUM ] ;

	// モデル描画用ピクセル単位ライティングタイプのトゥーンタイプ２用ピクセルシェーダー( リストは配列の左から )
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// [ トゥーンレンダリングのスフィアマップの有無とブレンドタイプ( 0:スフィアマップは無い   1:MV1_LAYERBLEND_TYPE_MODULATE  2:MV1_LAYERBLEND_TYPE_ADDITIVE ) ]
	// [ 有効なライトの数 - 1 ]
	// [ ピクセルシェーダーのアルファテストモード ]
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO MV1_PixelLighting_ToonType2_PS_Code[ 2 ][ 3 ][ DX_D3D11_COMMON_CONST_LIGHT_NUM ][ DIRECT3D11_PS_ALPHATEST_NUM ] ;

	// モデル描画用ピクセル単位ライティングタイプのピクセルシェーダー( リストは配列の左から )
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// [ マルチテクスチャのブレンドモード、MV1_LAYERBLEND_TYPE_TRANSLUCENT などに +1 したもの ( 0:マルチテクスチャ無し  1:αブレンド  2:加算ブレンド  3:乗算ブレンド  4:乗算ブレンド×2 ) ]
	// [ バンプマップ ( 0:なし 1:あり ) ]
	// [ 有効なライトの数 - 1 ]
	// [ ピクセルシェーダーのアルファテストモード ]
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO MV1_PixelLighting_Normal_PS_Code[ 2 ][ 5 ][ 2 ][ DX_D3D11_COMMON_CONST_LIGHT_NUM ][ DIRECT3D11_PS_ALPHATEST_NUM ] ;





	// モデル描画用のマテリアル要素描画用ピクセルシェーダー
	// [ マテリアルタイプ( DX_MATERIAL_TYPE_MAT_SPEC_LUMINANCE_UNORM など ) ]
	// [ ピクセルシェーダーのアルファテストモード ]
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO MV1_MaterialType_PS_Code[ DX_MATERIAL_TYPE_NUM ][ DIRECT3D11_PS_ALPHATEST_NUM ] ;





	// モデル描画用のシャドウマップへのトゥーンレンダリング輪郭線描画用頂点シェーダー
	// [ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ  2:8ボーン内スキニングメッシュ ) ]
	// [ バンプマップの有無( 0:無し 1:有り ) ]
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO MV1_ToonOutLine_ShadowMap_VS_Code[ 3 ][ 2 ] ;

	// モデル描画用のトゥーンレンダリングの輪郭線描画用頂点シェーダー
	// [ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ  2:8ボーン内スキニングメッシュ ) ]
	// [ バンプマップの有無( 0:無し 1:有り ) ]
	// [ フォグタイプ ]
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO MV1_ToonOutLine_VS_Code[ 3 ][ 2 ][ 4 ] ;

	// モデル描画用のシャドウマップへのレンダリング用頂点シェーダー
	// [ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ  2:8ボーン内スキニングメッシュ ) ]
	// [ バンプマップの有無( 0:無し 1:有り ) ]
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO MV1_ShadowMap_VS_Code[ 3 ][ 2 ] ;

	// モデル描画用のライティングなし頂点シェーダー
	// [ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ  2:8ボーン内スキニングメッシュ ) ]
	// [ バンプマップの有無( 0:無し 1:有り ) ]
	// [ フォグタイプ ]
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO MV1_NoLighting_VS_Code[ 3 ][ 2 ][ 4 ] ;

	// モデル描画用の頂点単位ライティングあり頂点シェーダー
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// [ メッシュタイプ( 0:剛体メッシュ 1:4ボーン内スキニングメッシュ  2:8ボーン内スキニングメッシュ ) ]
	// [ バンプマップの有無( 0:無し 1:有り ) ]
	// [ フォグタイプ ]
	// [ ライトインデックス ]
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO MV1_VertexLighting_VS_Code[ 2 ][ 3 ][ 2 ][ 4 ][ 20 ] ;

	// モデル描画用のシャドウマップへのトゥーンレンダリング用ピクセルシェーダー
	// [ トゥーンレンダリングタイプ( 0:トゥーンレンダリング type 1   1:トゥーンレンダリング type 2 ) ]
	// [ トゥーンレンダリングのスフィアマップの有無とブレンドタイプ( 0:スフィアマップは無い   1:MV1_LAYERBLEND_TYPE_MODULATE  2:MV1_LAYERBLEND_TYPE_ADDITIVE ) ]
	// [ トゥーンレンダリングのディフューズグラデーションのブレンドタイプ( 0:MV1_LAYERBLEND_TYPE_TRANSLUCENT  1:MV1_LAYERBLEND_TYPE_MODULATE ) ]
	// [ ピクセルシェーダーのアルファテストモード ]
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO MV1_ShadowMap_Toon_PS_Code[ 2 ][ 3 ][ 2 ][ DIRECT3D11_PS_ALPHATEST_NUM ] ;

	// モデル描画用のシャドウマップへのレンダリング用ピクセルシェーダー
	// [ ピクセルシェーダーのアルファテストモード ]
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO MV1_ShadowMap_Normal_PS_Code[ DIRECT3D11_PS_ALPHATEST_NUM ] ;

	// モデル描画用のライティングなしトゥーン用ピクセルシェーダー( リストは配列の左から )
	// [ トゥーンレンダリングタイプ( 0:トゥーンレンダリング type 1   1:トゥーンレンダリング type 2 ) ]
	// [ トゥーンレンダリングのディフューズグラデーションのブレンドタイプ( 0:MV1_LAYERBLEND_TYPE_TRANSLUCENT or トゥーンレンダリングではない  1:MV1_LAYERBLEND_TYPE_MODULATE ) ]
	// [ ピクセルシェーダーのアルファテストモード ]
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO MV1_NoLighting_Toon_PS_Code[ 2 ][ 2 ][ DIRECT3D11_PS_ALPHATEST_NUM ] ;

	// モデル描画用のライティングなしピクセルシェーダー
	// [ マルチテクスチャのブレンドモード、MV1_LAYERBLEND_TYPE_TRANSLUCENT などに +1 したもの ( 0:マルチテクスチャ無し  1:αブレンド  2:加算ブレンド  3:乗算ブレンド  4:乗算ブレンド×2 ) ]
	// [ ピクセルシェーダーのアルファテストモード ]
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO MV1_NoLighting_Normal_PS_Code[ 5 ][ DIRECT3D11_PS_ALPHATEST_NUM ] ;

	// モデル描画用の頂点単位ライティングありトゥーンタイプ１用ピクセルシェーダー
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// [ トゥーンレンダリングのディフューズグラデーションのブレンドタイプ( 0:MV1_LAYERBLEND_TYPE_TRANSLUCENT  1:MV1_LAYERBLEND_TYPE_MODULATE ) ]
	// [ トゥーンレンダリングのスペキュラグラデーションのブレンドタイプ( 0:MV1_LAYERBLEND_TYPE_TRANSLUCENT  1:MV1_LAYERBLEND_TYPE_ADDITIVE ) ]
	// [ バンプマップ ( 0:なし 1:あり ) ]
	// [ ライトインデックス ]
	// [ ピクセルシェーダーのアルファテストモード ]
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO MV1_VertexLighting_ToonType1_PS_Code[ 2 ][ 2 ][ 2 ][ 2 ][ 10 ][ DIRECT3D11_PS_ALPHATEST_NUM ] ;

	// モデル描画用の頂点単位ライティングありトゥーンタイプ２用ピクセルシェーダー
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// [ トゥーンレンダリングのスフィアマップの有無とブレンドタイプ( 0:スフィアマップは無い   1:MV1_LAYERBLEND_TYPE_MODULATE  2:MV1_LAYERBLEND_TYPE_ADDITIVE ) ]
	// [ ライトインデックス ]
	// [ ピクセルシェーダーのアルファテストモード ]
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO MV1_VertexLighting_ToonType2_PS_Code[ 2 ][ 3 ][ 10 ][ DIRECT3D11_PS_ALPHATEST_NUM ] ;

	// モデル描画用の頂点単位ライティングありピクセルシェーダー
	// [ シャドウマップの有無( 0:無し 1:有り ) ]
	// [ マルチテクスチャのブレンドモード、MV1_LAYERBLEND_TYPE_TRANSLUCENT などに +1 したもの ( 0:マルチテクスチャ無し  1:αブレンド  2:加算ブレンド  3:乗算ブレンド  4:乗算ブレンド×2 ) ]
	// [ バンプマップ ( 0:なし 1:あり ) ]
	// [ ライトインデックス ]
	// [ ピクセルシェーダーのアルファテストモード ]
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO MV1_VertexLighting_Normal_PS_Code[ 2 ][ 5 ][ 2 ][ 10 ][ DIRECT3D11_PS_ALPHATEST_NUM ] ;
} ;

#endif // DX_NON_MODEL


// シェーダーコードの元データを纏めた構造体
struct GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE
{
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_BASE	Base ;				// 標準描画処理用のシェーダーコードを纏めた構造体
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_BASE3D	Base3D ;			// 標準３Ｄ描画処理用のシェーダーコードを纏めた構造体

#ifndef DX_NON_MODEL
	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_MODEL	Model ;				// モデル描画処理用のシェーダーコードを纏めた構造体
#endif // DX_NON_MODEL
} ;

// アダプター一つ辺りの情報構造体
struct GRAPHICS_HARDWARE_DIRECT3D11_ADAPTER_UNIT
{
	D_DXGI_ADAPTER_DESC								Desc ;						// アダプタの情報

	int												OutputNum ;					// 出力先の数
	D_DXGI_OUTPUT_DESC								OutputDesc[ DX_D3D11_MAX_OUTPUT_LISTUP ] ; // 出力先の情報
} ;


// アダプター関係情報構造体
struct GRAPHICS_HARDWARE_DIRECT3D11_ADAPTER
{
	int												ValidInfo ;					// アダプタの情報が有効かどうか( TRUE:有効  FALSE:無効 )
	int												InfoNum ;					// アダプタ情報の数
	GRAPHICS_HARDWARE_DIRECT3D11_ADAPTER_UNIT		Info[ DX_D3D11_MAX_ADAPTER_LISTUP ] ;	// アダプタの情報

//	int												ValidUseOutputNumber ;		// UseOutputNumber が有効かどうか( TRUE:有効  FALSE:無効 )
//	int												UseOutputNumber ;			// 使用する出力番号
} ;


// Direct3D11 関係の設定情報の構造体
struct GRAPHICS_HARDDATA_DIRECT3D11_SETTING
{
	D_D3D_FEATURE_LEVEL				FeatureLevel ;								// 使用している機能レベル
	int								UseMinFeatureLevelDirect3D11 ;				// Direct3D11 で使用する最低機能レベル
	int								UseDirect3D11WARPTypeDriver ;				// D3D_DRIVER_TYPE_WARP ドライバを使用するかどうか
	int								UserDepthBufferSizeSet ;					// 外部から深度バッファサイズの指定があったかどうか
	int								UserDepthBufferBitDepthSet ;				// 外部から深度バッファのビット深度指定があったかどうか

	int								UseOldDrawModiGraphCodeFlag ;				// 以前の DrawModiGraph 関数コードを使用するかどうかのフラグ
} ;



// Direct3D11 のテクスチャ処理用情報の構造体
struct GRAPHICS_HARDDATA_DIRECT3D11_TEXTURE
{
	void *											CommonBuffer ;					// テクスチャ画像転送用の共有メモリ
	unsigned int									CommonBufferSize ;				// テクスチャ画像転送用の共有メモリのサイズ
} ;

// Direct3D11 を使ったグラフィックス処理情報の構造体
struct GRAPHICS_HARDDATA_DIRECT3D11
{
	int												UsePixelLightingShader ;	// ピクセル単位でライティングを行うタイプのシェーダーを使用するかどうか( TRUE:使用する  FALSE:使用しない )

#ifndef DX_NON_MODEL
	D_ID3D11Texture3D *								RGBtoVMaxRGBVolumeTexture ;			// RGBカラーを輝度を最大にしたRGB値に変換するためのボリュームテクスチャ
	D_ID3D11ShaderResourceView *					RGBtoVMaxRGBVolumeTextureSRV ;		// RGBカラーを輝度を最大にしたRGB値に変換するためのボリュームテクスチャのシェーダーリソースビュー
#endif // DX_NON_MODEL

	GRAPHICS_HARDDATA_DIRECT3D11_SETTING			Setting ;					// Direct3D11 関係の設定情報

	GRAPHICS_HARDDATA_DIRECT3D11_DEVICE				Device ;					// D3D11Device に密接に関連した情報

	GRAPHICS_HARDWARE_DIRECT3D11_ADAPTER			Adapter ;					// アダプター関係情報

	GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE			ShaderCode ;				// D_ID3D11PixelShader や D_ID3D11VertexShader の元となるシェーダーコードデータを纏めた構造体

	GRAPHICS_HARDDATA_DIRECT3D11_TEXTURE			Texture ;					// Direct3D11 のテクスチャ処理用情報の構造体
} ;


// 内部大域変数宣言 --------------------------------------------------------------

extern GRAPHICS_HARDDATA_DIRECT3D11 GraphicsHardDataDirect3D11 ;

// 関数プロトタイプ宣言-----------------------------------------------------------

// Direct3D11 の初期化関係
extern	int		Graphics_D3D11_Initialize( void ) ;								// Direct3D11 を使用したグラフィックス処理の初期化を行う( 0:成功 -1:失敗 )
extern	int		Graphics_D3D11_Terminate( void ) ;								// Direct3D11 を使用したグラフィックス処理の後始末を行う

extern	int		Graphics_D3D11_ReleaseObjectAll( void ) ;						// すべての Direct3D11 系オブジェクトを解放する
extern	int		Graphics_D3D11_CreateObjectAll( void ) ;						// すべての Direct3D11 系オブジェクトを作成する










// シェーダーコード関係
extern	int		Graphics_D3D11_ShaderCode_Base_Initialize( void ) ;				// Direct3D11 の標準描画用のシェーダーコードの初期化を行う
extern	int		Graphics_D3D11_ShaderCode_Base_Terminate( void ) ;				// Direct3D11 の標準描画用のシェーダーコードの後始末を行う

extern	int		Graphics_D3D11_ShaderCode_Base3D_Initialize( void ) ;			// Direct3D11 の標準３Ｄ描画用のシェーダーコードの初期化を行う
extern	int		Graphics_D3D11_ShaderCode_Base3D_Terminate( void ) ;			// Direct3D11 の標準３Ｄ描画用のシェーダーコードの後始末を行う

#ifndef DX_NON_MODEL

extern	int		Graphics_D3D11_ShaderCode_Model_Initialize( void ) ;			// Direct3D11 のモデル描画用のシェーダーコードの初期化を行う
extern	int		Graphics_D3D11_ShaderCode_Model_Terminate( void ) ;				// Direct3D11 のモデル描画用のシェーダーコードの後始末を行う

#endif // DX_NON_MODEL

extern	int		Graphics_D3D11_Shader_Initialize( void ) ;						// Direct3D11 のシェーダーの初期化を行う
extern	int		Graphics_D3D11_Shader_Terminate( void ) ;						// Direct3D11 のシェーダーの後始末をする

extern	int		Graphics_D3D11_VertexShader_Create( GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO *Code, D_ID3D11VertexShader **pDestShader, int Num ) ;		// シェーダーコードから頂点シェーダーを作成する
extern	int		Graphics_D3D11_PixelShader_Create(  GRAPHICS_HARDWARE_DIRECT3D11_SHADERCODE_INFO *Code, D_ID3D11PixelShader  **pDestShader, int Num ) ;		// シェーダーコードからピクセルシェーダーを作成する

extern	int		Graphics_D3D11_Shader_Normal3DDraw_Setup( void ) ;				// ３Ｄ標準描画の指定の頂点用の描画用シェーダーをセットアップする
extern	int		Graphics_D3D11_Shader_GetAlphaTestModeIndex( int AlphaTestEnable, int AlphaTestMode /* DX_CMP_NEVER 等 */ ) ;	// アルファテストの比較モードからピクセルシェーダーのアルファテストモード( DIRECT3D11_PS_ALPHATEST_CMP_GREATER 等 )を取得する










// 頂点バッファ・インデックスバッファ関係
extern	int		Graphics_D3D11_VertexBuffer_CreateObject(  VERTEXBUFFERHANDLEDATA *VertexBuffer, int Restore ) ;	// 頂点バッファハンドル用の ID3D11Buffer オブジェクトを作成する
extern	int		Graphics_D3D11_VertexBuffer_ReleaseObject( VERTEXBUFFERHANDLEDATA *VertexBuffer ) ;					// 頂点バッファハンドル用の ID3D11Buffer オブジェクトを解放する
extern	int		Graphics_D3D11_IndexBuffer_CreateObject(   INDEXBUFFERHANDLEDATA *IndexBuffer,   int Restore ) ;	// インデックスバッファハンドル用の ID3D11Buffer オブジェクトを作成する
extern	int		Graphics_D3D11_IndexBuffer_ReleaseObject(  INDEXBUFFERHANDLEDATA *IndexBuffer ) ;					// インデックスバッファハンドル用の ID3D11Buffer オブジェクトを解放する










// Direct3D11 の画面関係
extern	int		Graphics_D3D11_OutputWindow_Add( HWND TargetWindow, int IsMainTarget = FALSE ) ;					// 出力先ウインドウの追加を行う( -1:失敗  -1以外:対象ウインドウ用情報の配列インデックス )
extern	int		Graphics_D3D11_OutputWindow_Sub( HWND TargetWindow, int Index = -1 ) ;								// 指定のウインドウを出力先ウインドウから外す
extern	int		Graphics_D3D11_OutputWindow_SetupBuffer( int Index ) ;												// 指定の出力先ウインドウ用のバックバッファーをセットアップする
extern	int		Graphics_D3D11_OutputWindow_ReleaseBuffer( int Index ) ;											// 指定の出力先ウインドウ用のバックバッファーの後始末をする
extern	int		Graphics_D3D11_OutputWindow_ResizeBuffer( int Index, int NewSizeX, int NewSizeY ) ;					// 指定の出力先ウインドウのバックバッファのサイズを変更する
extern	int		Graphics_D3D11_OutputWindow_Present( int Index, RECT *BackScreenRect, RECT *WindowClientRect ) ;	// 指定の出力先ウインドウの指定の領域に指定のバックバッファの領域を転送する
extern	int		Graphics_D3D11_SetupSubBackBuffer( void ) ;															// サブバックバッファを使用する準備を行う( 0:成功  -1:失敗 )
extern	int		Graphics_D3D11_CreateDepthBuffer( void ) ;															// 深度バッファオブジェクトの作成( 0:成功  -1:失敗 )















// Direct3D11 の画像関係
extern COLORDATA *Graphics_D3D11_GetD3DFormatColorData( D_DXGI_FORMAT Format ) ;									// D_DXGI_FORMAT のフォーマットに合わせたカラーフォーマット情報を取得する

// 描画先用テクスチャと描画用テクスチャが分かれている場合に、描画用テクスチャに描画先用テクスチャの内容を反映する
extern	int		Graphics_D3D11_UpdateDrawTexture( IMAGEDATA_ORIG_HARD_TEX *OrigTex, int TargetSurface, D_DXGI_FORMAT Format ) ;

// 基本イメージのフォーマットを DX_BASEIMAGE_FORMAT_NORMAL に変換する必要があるかどうかをチェックする
// ( RequiredRgbBaseImageConvFlag と RequiredAlphaBaseImageConvFlag に入る値  TRUE:変換する必要がある  FALSE:変換する必要は無い )
extern	int		Graphics_D3D11_CheckRequiredNormalImageConv_BaseImageFormat_PF(
	IMAGEDATA_ORIG *Orig,
	int             RgbBaseImageFormat,
	int            *RequiredRgbBaseImageConvFlag,
	int             AlphaBaseImageFormat = -1,
	int            *RequiredAlphaBaseImageConvFlag = NULL
) ;

extern	int		Graphics_D3D11_Texture_SetupCommonBuffer( unsigned int Size ) ;		// テクスチャ画像転送用の共有メモリのセットアップを行う
extern	int		Graphics_D3D11_Texture_TerminateCommonBuffer( void ) ;				// テクスチャ画像転送用の共有メモリの後始末を行う

extern	int		Graphics_D3D11_StretchRect_Initialize( void ) ;				// 単純転送処理の初期化を行う
extern	int		Graphics_D3D11_StretchRect_Terminate( void ) ;				// 単純転送処理の後始末を行う

// レンダーターゲットテクスチャにテクスチャの単純転送を行う
extern	int		Graphics_D3D11_StretchRect(
	D_ID3D11Texture2D *SrcTexture,          D_ID3D11ShaderResourceView  *SrcTextureSRV,          const RECT *SrcRect  /* NULL で全体 */,
	D_ID3D11Texture2D *DestTexture,         D_ID3D11RenderTargetView    *DestTextureRTV,         const RECT *DestRect /* NULL で全体 */,
	D_D3D11_FILTER_TYPE FilterType = D_D3D11_FILTER_TYPE_POINT,
	int AlphaBlend = FALSE,
	D_ID3D11VertexShader *VertexShader = NULL,
	D_ID3D11PixelShader  *PixelShader  = NULL,
	D_ID3D11Texture2D *BlendTexture = NULL, D_ID3D11ShaderResourceView  *BlendTextureSRV = NULL, const RECT *BlendRect = NULL /* NULL で全体 */,
	VERTEX_TEX8_2D *Texcoord8Vertex = NULL
) ;

// 指定のテクスチャの内容に読み込みアクセスできるようにする
extern	int		Graphics_D3D11_Texture2D_Map(
	D_ID3D11Texture2D *				TargetTexture,
	D_ID3D11ShaderResourceView *	TargetTextureSRV,
	int								TargetTextureArraySlice,
	int								TargetTextureMipSlice,
	const RECT *					MapRect /* NULL で全体 */,
	BASEIMAGE *						MapBaseImage,
	D_ID3D11Texture2D **			MapTempTexture
) ;

// 指定のテクスチャの内容への読み込みアクセスを終了する
extern	int		Graphics_D3D11_Texture2D_Unmap( D_ID3D11Texture2D *MapTempTexture ) ;







// D3D11Device の初期化関係
extern	int		Graphics_D3D11_Device_Create( void ) ;																// Direct3DDevice11 オブジェクトの作成( 0:成功  -1:失敗 )

extern	int		Graphics_D3D11_Device_Initialize( void ) ;															// Direct3DDevice11 関係の初期化
extern	int		Graphics_D3D11_Device_ReInitialize( void ) ;														// 一時的に Direct3DDevice11 を破棄した際に、破棄前の状態を作り出すための関数






// DXGIAdapter 関係
extern	int		EnumDXGIAdapterInfo( void ) ;																		// DXGIオブジェクト情報の列挙








// D3D11Device ステータス関係
extern	void	Graphics_D3D11_DeviceState_RefreshRenderState( void ) ;												// 描画設定をリフレッシュ
extern	int		Graphics_D3D11_DeviceState_SetBaseState( void ) ;													// Direct3DDevice11 の基本設定を行う
extern	int		Graphics_D3D11_DeviceState_SetSampleFilterMode( D_D3D11_FILTER Filter, int Sampler = -1 ) ;			// サンプラーのテクスチャフィルタリングモードを設定する
extern	int		Graphics_D3D11_DeviceState_SetDepthEnable( BOOL DepthEnable )  ;									// 深度バッファの有効無効のセット
extern	int		Graphics_D3D11_DeviceState_SetDepthWriteEnable( int Flag ) ;										// 深度バッファの書き込みの有無をセット
extern	int		Graphics_D3D11_DeviceState_SetDepthFunc( D_D3D11_COMPARISON_FUNC DepthFunc ) ;						// 深度値の比較タイプをセット
extern	int		Graphics_D3D11_DeviceState_SetDepthBias( int DepthBias ) ;											// 深度値のバイアスをセット
extern	int		Graphics_D3D11_DeviceState_SetFillMode( D_D3D11_FILL_MODE FillMode ) ;								// フィルモードをセット
extern	int		Graphics_D3D11_DeviceState_SetWorldMatrix( const MATRIX *Matrix ) ;									// ワールド変換用行列をセットする
extern	int		Graphics_D3D11_DeviceState_SetViewMatrix( const MATRIX *Matrix ) ;									// ビュー変換用行列をセットする
extern	int		Graphics_D3D11_DeviceState_SetProjectionMatrix( const MATRIX *Matrix ) ;							// 投影変換用行列をセットする
extern	int		Graphics_D3D11_DeviceState_SetAntiViewportMatrix( const MATRIX *Matrix ) ;							// アンチビューポート行列をセットする
extern	int		Graphics_D3D11_DeviceState_SetLightState( int LightNumber, LIGHTPARAM *Light ) ;					// ライトのセット
extern	int		Graphics_D3D11_DeviceState_SetLightEnable( int LightNumber, int EnableState ) ;						// ライトの有効、無効を変更
extern	int		Graphics_D3D11_DeviceState_SetMaterial( MATERIALPARAM *Material ) ;									// マテリアルのセット
extern	int		Graphics_D3D11_DeviceState_SetSpecularEnable( int UseFlag ) ;										// スペキュラを使用するかどうかをセット
extern	int		Graphics_D3D11_DeviceState_SetUseVertexDiffuseColor( int UseFlag ) ;								// 頂点のディフューズカラーをマテリアルのディフューズカラーとして使用するかどうかを設定する
extern	int		Graphics_D3D11_DeviceState_SetUseVertexSpecularColor( int UseFlag ) ;								// 頂点のスペキュラカラーをマテリアルのスペキュラカラーとして使用するかどうかを設定する
extern	int		Graphics_D3D11_DeviceState_SetCullMode( int State ) ;												// カリングの有無をセット 
extern	int		Graphics_D3D11_DeviceState_SetAmbient( COLOR_F *Color ) ;												// アンビエントカラーのセット
extern	int		Graphics_D3D11_DeviceState_SetTextureAddress( int AddressMode, int Sampler = -1 ) ;					// テクスチャーのアドレッシングモードをセットする
extern	int		Graphics_D3D11_DeviceState_SetTextureAddressU( int AddressMode, int Sampler = -1 ) ;				// テクスチャーのアドレッシングモードをセットする
extern	int		Graphics_D3D11_DeviceState_SetTextureAddressV( int AddressMode, int Sampler = -1 ) ;				// テクスチャーのアドレッシングモードをセットする
extern	int		Graphics_D3D11_DeviceState_SetTextureAddressW( int AddressMode, int Sampler = -1 ) ;				// テクスチャーのアドレッシングモードをセットする
extern	int		Graphics_D3D11_DeviceState_SetTextureAddressUVW( int AddressModeU, int AddressModeV, int AddressModeW, int Sampler = -1 ) ;		// テクスチャーのアドレッシングモードをセットする
extern	int		Graphics_D3D11_DeviceState_SetTextureAddressTransformMatrix( int Use, MATRIX *Matrix ) ;			// テクスチャ座標変換行列をセットする
extern	int		Graphics_D3D11_DeviceState_SetFogEnable( int Flag ) ;												// フォグを有効にするかどうかを設定する( TRUE:有効  FALSE:無効 )
extern	int		Graphics_D3D11_DeviceState_SetFogVertexMode( int Mode /* DX_FOGMODE_NONE 等 */ ) ;					// フォグモードを設定する
extern	int		Graphics_D3D11_DeviceState_SetFogColor( unsigned int Color ) ;												// フォグカラーを変更する
extern	int		Graphics_D3D11_DeviceState_SetFogStartEnd( float Start, float End ) ;								// フォグが始まる距離と終了する距離を設定する( 0.0f 〜 1.0f )
extern	int		Graphics_D3D11_DeviceState_SetFogDensity( float Density ) ;											// フォグの密度を設定する( 0.0f 〜 1.0f )
extern	int		Graphics_D3D11_DeviceState_SetLighting( int UseFlag ) ;												// ライティングの有無フラグをセットする
extern	int		Graphics_D3D11_DeviceState_SetMaxAnisotropy( int MaxAnisotropy, int Sampler = -1 ) ;				// 最大異方性をセットする
extern	int		Graphics_D3D11_DeviceState_SetViewport( D_D3D11_VIEWPORT *Viewport ) ;								// ビューポートをセットする
extern	int		Graphics_D3D11_DeviceState_SetViewportEasy( float x1, float y1, float x2, float y2 ) ;				// ビューポートをセットする( 簡易版 )
extern	int		Graphics_D3D11_DeviceState_SetDrawMode( int DrawMode ) ;											// 描画モードのセット
extern	int		Graphics_D3D11_DeviceState_SetAlphaTestCmpMode( int AlphaTestCmpMode /* DX_CMP_NEVER など */ ) ;	// アルファテストで使用する比較モードを設定する
extern	int		Graphics_D3D11_DeviceState_SetAlphaTestRef( int AlphaTestRef ) ;									// アルファテストで使用する参照値を設定する
extern	int		Graphics_D3D11_DeviceState_SetMulAlphaColor( int UseMulAlphaColor ) ;								// カラーにアルファ値を乗算するかどうかを設定する
extern	int		Graphics_D3D11_DeviceState_SetPrimitiveTopology( D_D3D11_PRIMITIVE_TOPOLOGY PrimitiveTopology ) ;	// デバイスで使用するプリミティブタイプを設定する
extern	int		Graphics_D3D11_DeviceState_SetFactorColor( const DX_D3D11_SHADER_FLOAT4 *FactorColor ) ;			// Factor Color を設定する
extern	int		Graphics_D3D11_DeviceState_SetToonOutLineSize( float Size ) ;										// トゥーンレンダリングの輪郭線の太さを設定する
extern	int		Graphics_D3D11_DeviceState_SetBlendMode( int BlendMode ) ;											// 描画ブレンドモードのセット
extern	int		Graphics_D3D11_DeviceState_SetRenderTarget( D_ID3D11Texture2D *RenderTargetTexture, D_ID3D11RenderTargetView *RenderTargetView, int TargetIndex = 0 ) ;	// 描画対象の変更
extern	int		Graphics_D3D11_DeviceState_SetDepthStencil( D_ID3D11Texture2D *DepthStencilTexture, D_ID3D11DepthStencilView *DepthStencilView ) ;	// 描画対象の深度バッファを変更
extern	int		Graphics_D3D11_DeviceState_SetInputLayout( D_ID3D11InputLayout *InputLayout ) ;						// 使用する入力レイアウトを変更する
extern	int		Graphics_D3D11_DeviceState_SetVertexShaderToHandle( int ShaderHandle ) ;								// 指定の頂点シェーダーをデバイスにセットする
extern	int		Graphics_D3D11_DeviceState_SetVertexShader( D_ID3D11VertexShader *VertexShader, int NormalVertexShader = FALSE ) ;	// 使用する頂点シェーダーを変更する
extern	int		Graphics_D3D11_DeviceState_ResetVertexShader( int SetNormalVertexShaderCancel = FALSE ) ;						// 頂点シェーダーの使用を止める
extern	int		Graphics_D3D11_DeviceState_SetVertexShaderConstantBuffers( UINT StartSlot, UINT NumBuffers, D_ID3D11Buffer * const *pConstantBuffers ) ;	// 頂点シェーダーで使用する定数バッファーを設定する
extern	int		Graphics_D3D11_DeviceState_SetPixelShaderToHandle( int ShaderHandle ) ;								// 指定のピクセルシェーダーをデバイスにセットする
extern	int		Graphics_D3D11_DeviceState_SetPixelShader( D_ID3D11PixelShader *PixelShader, int NormalPixelShader = FALSE ) ;	// 使用するピクセルシェーダーを変更する
extern	int		Graphics_D3D11_DeviceState_ResetPixelShader( int SetNormalPixelShaderCancel = FALSE ) ;							// ピクセルシェーダーの使用を止める
extern	int		Graphics_D3D11_DeviceState_SetPixelShaderConstantBuffers( UINT StartSlot, UINT NumBuffers, D_ID3D11Buffer * const *pConstantBuffers ) ;	// ピクセルシェーダーで使用する定数バッファーを設定する
extern	int		Graphics_D3D11_DeviceState_SetVertexBuffer( D_ID3D11Buffer *VertexBuffer, int VertexStride = 0 ) ;	// 使用する頂点バッファを変更する
extern	int		Graphics_D3D11_DeviceState_SetIndexBuffer( D_ID3D11Buffer *IndexBuffer, D_DXGI_FORMAT Format ) ;	// 使用するインデックスバッファを変更する
extern	int		Graphics_D3D11_DeviceState_SetPSShaderResouceView( int StartSlot, int Num, D_ID3D11ShaderResourceView * const *ppShaderResourceViews ) ;	// ピクセルシェーダーで使用するシェーダーリソースビューを変更する
extern	int		Graphics_D3D11_DeviceState_NormalDrawSetup( void ) ;												// 標準描画の準備を行う
extern	int		Graphics_D3D11_DeviceState_SetupStateAndConstantBuffer( void ) ;									// 現在の設定に基づいて ID3D11SamplerState、ID3D11RasterizerState、ID3D11DepthStencilState、ID3D11BlendState のセットアップと、定数バッファのアップデートを行う






// 描画設定関係関数
extern	int		Graphics_D3D11_DrawSetting_SetDrawBlendMode( int BlendMode, int AlphaTestValidFlag, int AlphaChannelValidFlag ) ;	// 描画ブレンドモードの設定
extern	int		Graphics_D3D11_DrawSetting_SetIgnoreDrawGraphColor( int EnableFlag ) ;							// 描画時の画像のＲＧＢを無視するかどうかを設定する
extern	int		Graphics_D3D11_DrawSetting_SetIgnoreDrawGraphAlpha( int EnableFlag ) ;							// 描画時の画像のＡを無視するかどうかを設定する
extern	int		Graphics_D3D11_DrawSetting_SetDrawAlphaTest( int TestMode, int TestParam ) ;					// アルファテストの設定
extern	int		Graphics_D3D11_DrawSetting_SetBlendTextureParam( int BlendType, int *Param ) ;					// ブレンドするテクスチャのパラメータをセットする
extern	int		Graphics_D3D11_DrawSetting_SetTexture(      D_ID3D11Texture2D *RenderTexture, D_ID3D11ShaderResourceView *RenderTextureSRV ) ;											// テクスチャをセットする 
extern	int		Graphics_D3D11_DrawSetting_SetBlendTexture( D_ID3D11Texture2D *BlendTexture,  D_ID3D11ShaderResourceView *BlendTextureSRV, int TexWidth = -1, int TexHeight = -1 ) ;	// ブレンドするテクスチャをセットする 









// 定数情報関係関数
extern int Graphics_D3D11_Constant_Initialize( void ) ;												// Direct3D11 の定数情報の初期化を行う
extern int Graphics_D3D11_Constant_Terminate( void ) ;												// Direct3D11 の定数情報の後始末を行う








// 定数バッファ関係関数
extern	CONSTANTBUFFER_DIRECT3D11 *Graphics_D3D11_ConstantBuffer_Create( DWORD Size, int ASyncThread = FALSE ) ;	// 定数バッファを作成する
extern	int		Graphics_D3D11_ConstantBuffer_Delete( CONSTANTBUFFER_DIRECT3D11 *ConstantBuffer ) ;					// 定数バッファを削除する
extern	int		Graphics_D3D11_ConstantBuffer_Update( CONSTANTBUFFER_DIRECT3D11 *ConstantBuffer ) ;					// システムメモリ上のバッファに行った変更を適用する
extern	int		Graphics_D3D11_ConstantBuffer_VSSet( UINT StartSlot, UINT NumBuffers, CONSTANTBUFFER_DIRECT3D11 * const *pConstantBuffers ) ;	// 指定の定数バッファを頂点シェーダーに設定する
extern	int		Graphics_D3D11_ConstantBuffer_PSSet( UINT StartSlot, UINT NumBuffers, CONSTANTBUFFER_DIRECT3D11 * const *pConstantBuffers ) ;	// 指定の定数バッファをピクセルシェーダーに設定する









// 入力レイアウト関係関数
extern	int		Graphics_D3D11_InputLayout_Base_Create( void ) ;		// 標準描画用の ID3D11InputLayout オブジェクトの作成( 0:成功  -1:失敗 )
extern	int		Graphics_D3D11_InputLayout_Base_Terminate( void ) ;		// 標準描画用の ID3D11InputLayout オブジェクトの削除( 0:成功  -1:失敗 )

#ifndef DX_NON_MODEL
extern	int		Graphics_D3D11_InputLayout_Model_Create( int BumpMap, int MeshType ) ;	// モデル描画用の ID3D11InputLayout オブジェクトの作成( 0:成功  -1:失敗 )
extern	int		Graphics_D3D11_InputLayout_Model_Terminate( void ) ;					// モデル描画用の ID3D11InputLayout オブジェクトの削除( 0:成功  -1:失敗 )
#endif // DX_NON_MODEL








// 標準描画関係
extern	int		Graphics_D3D11_NormalDrawVertexBuffer_Create( void ) ;		// 標準描画用の頂点バッファの作成( 0:成功  -1:失敗 )
extern	int		Graphics_D3D11_NormalDrawVertexBuffer_Terminate( void ) ;	// 標準描画用の頂点バッファの削除( 0:成功  -1:失敗 )

extern	int		Graphics_D3D11_CommonBuffer_DrawPrimitive(        int InputLayout, D_D3D11_PRIMITIVE_TOPOLOGY PrimitiveType, const void *VertexData, int VertexNum,                                                                 int UseDefaultVertexShader = TRUE ) ;	// 汎用描画用頂点バッファを使用してシステムメモリ上の頂点データの描画を行う
extern	int		Graphics_D3D11_CommonBuffer_DrawIndexedPrimitive( int InputLayout, D_D3D11_PRIMITIVE_TOPOLOGY PrimitiveType, const void *VertexData, int VertexNum, const void *IndexData, int IndexNum, D_DXGI_FORMAT IndexFormat, int UseDefaultVertexShader = TRUE ) ;	// 汎用描画用頂点バッファ・インデックスバッファを使用してシステムメモリ上の頂点データ・インデックスデータの描画を行う
extern	int		Graphics_D3D11_CommonVertexBuffer_Setup( int Size ) ;		// 指定サイズの汎用描画用頂点バッファの準備を行う
extern	int		Graphics_D3D11_CommonIndexBuffer_Setup( int Size ) ;		// 指定サイズの汎用描画用インデックスバッファの準備を行う
extern	int		Graphics_D3D11_CommonVertexBuffer_Terminate( void ) ;		// 汎用描画用頂点バッファの後始末を行う
extern	int		Graphics_D3D11_CommonIndexBuffer_Terminate( void ) ;		// 汎用描画用インデックスバッファの後始末を行う













// Direct3D11 の描画処理準備関係
extern	int		Graphics_D3D11_RenderVertex( int NextUse3DVertex = -1, int ASyncThread = FALSE ) ;	// 頂点バッファに溜まった頂点データをレンダリングする
extern	void	FASTCALL Graphics_D3D11_DrawPreparation( int ParamFlag = 0 ) ;						// 描画準備を行う
extern	void	Graphics_D3D11_RenderBegin( void ) ;												// 描画コマンドを開始する
extern	void	Graphics_D3D11_RenderEnd( void ) ;													// 描画コマンドを終了する












// Direct3D11 を使った描画関係
extern	int		Graphics_D3D11_DrawBillboard3D( VECTOR Pos, float cx, float cy, float Size, float Angle, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, int ReverseXFlag, int ReverseYFlag, int DrawFlag = TRUE, RECT *DrawArea = NULL ) ;	// ハードウエアアクセラレータ使用版 DrawBillboard3D
extern	int		Graphics_D3D11_DrawModiBillboard3D( VECTOR Pos, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, int DrawFlag = TRUE, RECT *DrawArea = NULL ) ;	// ハードウエアアクセラレータ使用版 DrawModiBillboard3D
extern	int		Graphics_D3D11_DrawGraph( int x, int y, float xf, float yf, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, int IntFlag ) ;																// ハードウエアアクセラレータ使用版 DrawGraph
extern	int		Graphics_D3D11_DrawExtendGraph( int x1, int y1, int x2, int y2, float x1f, float y1f, float x2f, float y2f, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, int IntFlag ) ;				// ハードウエアアクセラレータ使用版 DrawExtendGraph
extern	int		Graphics_D3D11_DrawRotaGraph( int x, int y, float xf, float yf, double ExRate, double Angle, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, int ReverseXFlag, int ReverseYFlag, int IntFlag ) ;				// ハードウエアアクセラレータ使用版 DrawRotaGraph
extern	int		Graphics_D3D11_DrawRotaGraphFast( int x, int y, float xf, float yf, float ExRate, float Angle, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, int ReverseXFlag, int ReverseYFlag, int IntFlag ) ;			// ハードウエアアクセラレータ使用版 DrawRotaGraphFast
extern	int		Graphics_D3D11_DrawModiGraph( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, bool SimpleDrawFlag ) ;					// ハードウエアアクセラレータ使用版 DrawModiGraph
extern	int		Graphics_D3D11_DrawModiGraphF( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, bool SimpleDrawFlag ) ;	// ハードウエアアクセラレータ使用版 DrawModiGraphF
extern	void	Graphics_D3D11_DrawModiTex( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, IMAGEDATA_HARD_DRAW *DrawTex, bool SimpleDrawFlag ) ;								// テクスチャを変形して描画する
extern	int		Graphics_D3D11_DrawSimpleQuadrangleGraphF( const GRAPHICS_DRAW_DRAWSIMPLEQUADRANGLEGRAPHF_PARAM *Param, IMAGEDATA *Image, IMAGEDATA *BlendImage ) ;												// ハードウエアアクセラレータ使用版 DrawSimpleQuadrangleGraphF
extern	int		Graphics_D3D11_DrawSimpleTriangleGraphF(   const GRAPHICS_DRAW_DRAWSIMPLETRIANGLEGRAPHF_PARAM   *Param, IMAGEDATA *Image, IMAGEDATA *BlendImage ) ;												// ハードウエアアクセラレータ使用版 DrawSimpleTriangleGraphF

extern	int		Graphics_D3D11_DrawFillBox( int x1, int y1, int x2, int y2, unsigned int Color ) ;																		// ハードウエアアクセラレータ使用版 DrawFillBox
extern	int		Graphics_D3D11_DrawLineBox( int x1, int y1, int x2, int y2, unsigned int Color ) ;																		// ハードウエアアクセラレータ使用版 DrawLineBox
extern	int		Graphics_D3D11_DrawLine( int x1, int y1, int x2, int y2, unsigned int Color ) ;																			// ハードウエアアクセラレータ使用版 DrawLine
extern	int		Graphics_D3D11_DrawLine3D( VECTOR Pos1, VECTOR Pos2, unsigned int Color, int DrawFlag = TRUE, RECT *DrawArea = NULL ) ;									// ハードウエアアクセラレータ使用版 DrawLine3D
extern	int		Graphics_D3D11_DrawCircle_Thickness( int x, int y, int r, unsigned int Color, int Thickness ) ;															// ハードウエアアクセラレータ使用版 DrawCircle( 太さ指定あり )
extern	int		Graphics_D3D11_DrawOval_Thickness( int x, int y, int rx, int ry, unsigned int Color, int Thickness ) ;													// ハードウエアアクセラレータ使用版 DrawOval( 太さ指定あり )
extern	int		Graphics_D3D11_DrawCircle( int x, int y, int r, unsigned int Color, int FillFlag, int Rx_One_Minus, int Ry_One_Minus ) ;								// ハードウエアアクセラレータ使用版 DrawCircle
extern	int		Graphics_D3D11_DrawOval( int x, int y, int rx, int ry, unsigned int Color, int FillFlag, int Rx_One_Minus, int Ry_One_Minus ) ;							// ハードウエアアクセラレータ使用版 DrawOval
extern	int		Graphics_D3D11_DrawTriangle( int x1, int y1, int x2, int y2, int x3, int y3, unsigned int Color, int FillFlag ) ;										// ハードウエアアクセラレータ使用版 DrawTriangle
extern	int		Graphics_D3D11_DrawTriangle3D( VECTOR Pos1, VECTOR Pos2, VECTOR Pos3, unsigned int Color, int FillFlag, int DrawFlag = TRUE, RECT *DrawArea = NULL ) ;	// ハードウエアアクセラレータ使用版 DrawTriangle3D
extern	int		Graphics_D3D11_DrawQuadrangle( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int Color, int FillFlag ) ;						// ハードウエアアクセラレータ使用版 DrawQuadrangle
extern	int		Graphics_D3D11_DrawQuadrangleF( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int Color, int FillFlag ) ;						// ハードウエアアクセラレータ使用版 DrawQuadrangle
extern	int		Graphics_D3D11_DrawPixel( int x, int y, unsigned int Color ) ;																							// ハードウエアアクセラレータ使用版 DrawPixel
extern	int		Graphics_D3D11_DrawPixel3D( VECTOR Pos, unsigned int Color, int DrawFlag = TRUE, RECT *DrawArea = NULL ) ;												// ハードウエアアクセラレータ使用版 DrawPixel3D
extern	int		Graphics_D3D11_DrawPixelSet( const POINTDATA *PointData, int Num ) ;																				// ハードウエアアクセラレータ使用版 DrawPixelSet
extern	int		Graphics_D3D11_DrawLineSet( const LINEDATA *LineData, int Num ) ;																				// ハードウエアアクセラレータ使用版 DrawLineSet

extern	int		Graphics_D3D11_DrawPrimitive(                             const VERTEX_3D *Vertex, int VertexNum,                                    int PrimitiveType, IMAGEDATA *Image, int TransFlag ) ;
extern	int		Graphics_D3D11_DrawIndexedPrimitive(                      const VERTEX_3D *Vertex, int VertexNum, const WORD *Indices, int IndexNum, int PrimitiveType, IMAGEDATA *Image, int TransFlag ) ;
extern	int		Graphics_D3D11_DrawPrimitiveLight(                        const VERTEX3D  *Vertex, int VertexNum,                                    int PrimitiveType, IMAGEDATA *Image, int TransFlag ) ;
extern	int		Graphics_D3D11_DrawIndexedPrimitiveLight(                 const VERTEX3D  *Vertex, int VertexNum, const WORD *Indices, int IndexNum, int PrimitiveType, IMAGEDATA *Image, int TransFlag ) ;
extern	int		Graphics_D3D11_DrawPrimitiveLight_UseVertexBuffer(        VERTEXBUFFERHANDLEDATA *VertexBuffer,                                      int PrimitiveType,                 int StartVertex, int UseVertexNum, IMAGEDATA *Image, int TransFlag ) ;
extern	int		Graphics_D3D11_DrawIndexedPrimitiveLight_UseVertexBuffer( VERTEXBUFFERHANDLEDATA *VertexBuffer, INDEXBUFFERHANDLEDATA *IndexBuffer,  int PrimitiveType, int BaseVertex, int StartVertex, int UseVertexNum, int StartIndex, int UseIndexNum, IMAGEDATA *Image, int TransFlag ) ;
extern	int		Graphics_D3D11_DrawPrimitive2D(                                 VERTEX_2D *Vertex, int VertexNum,                                    int PrimitiveType, IMAGEDATA *Image, int TransFlag, int BillboardFlag, int Is3D, int ReverseXFlag, int ReverseYFlag, int TextureNo, int IsShadowMap ) ;
extern	int		Graphics_D3D11_DrawPrimitive2DUser(                       const VERTEX2D  *Vertex, int VertexNum,                                    int PrimitiveType, IMAGEDATA *Image, int TransFlag, int Is3D, int ReverseXFlag, int ReverseYFlag, int TextureNo ) ;
extern	int		Graphics_D3D11_DrawIndexedPrimitive2DUser(                const VERTEX2D  *Vertex, int VertexNum, const WORD *Indices, int IndexNum, int PrimitiveType, IMAGEDATA *Image, int TransFlag ) ;

extern	void	Graphics_D3D11_DrawPreparationToShader( int ParamFlag, int Is2D ) ;											// シェーダー描画用描画前セットアップ関数

extern	int		Graphics_D3D11_DrawPrimitive2DToShader(        const VERTEX2DSHADER *Vertex, int VertexNum,                                              int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ ) ;		// シェーダーを使って２Ｄプリミティブを描画する
extern	int		Graphics_D3D11_DrawPrimitive3DToShader(        const VERTEX3DSHADER *Vertex, int VertexNum,                                              int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ ) ;		// シェーダーを使って３Ｄプリミティブを描画する
extern	int		Graphics_D3D11_DrawPrimitiveIndexed2DToShader( const VERTEX2DSHADER *Vertex, int VertexNum, const unsigned short *Indices, int IndexNum, int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ ) ;		// シェーダーを使って２Ｄプリミティブを描画する( 頂点インデックスを使用する )
extern	int		Graphics_D3D11_DrawPrimitiveIndexed3DToShader( const VERTEX3DSHADER *Vertex, int VertexNum, const unsigned short *Indices, int IndexNum, int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ ) ;		// シェーダーを使って３Ｄプリミティブを描画する( 頂点インデックスを使用する )

extern	int		Graphics_D3D11_DrawPrimitive3DToShader_UseVertexBuffer2(        int VertexBufHandle,                     int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */, int StartVertex, int UseVertexNum ) ;	// シェーダーを使って３Ｄプリミティブを描画する( 頂点バッファ使用版 )
extern	int		Graphics_D3D11_DrawPrimitiveIndexed3DToShader_UseVertexBuffer2( int VertexBufHandle, int IndexBufHandle, int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */, int BaseVertex, int StartVertex, int UseVertexNum, int StartIndex, int UseIndexNum ) ;	// シェーダーを使って３Ｄプリミティブを描画する( 頂点バッファとインデックスバッファ使用版 )








// 環境依存初期化関係
extern	int		Graphics_D3D11_Initialize_Timing0_PF( void ) ;											// 描画処理の環境依存部分の初期化を行う関数( 実行箇所区別０ )
extern	int		Graphics_D3D11_Initialize_Timing1_PF( void ) ;											// 描画処理の環境依存部分の初期化を行う関数( 実行箇所区別２ )
extern	int		Graphics_D3D11_Hardware_Initialize_PF( void ) ;											// ハードウエアアクセラレータを使用する場合の環境依存の初期化処理を行う
extern	int		Graphics_D3D11_Terminate_PF( void ) ;													// 描画処理の環境依存部分の後始末を行う関数
extern	int		Graphics_D3D11_RestoreOrChangeSetupGraphSystem_PF( int Change, int ScreenSizeX = -1, int ScreenSizeY = -1, int ColorBitDepth = -1, int RefreshRate = -1 ) ;		// グラフィックスシステムの復帰、又は変更付きの再セットアップを行う
extern	int		Graphics_D3D11_Hardware_CheckValid_PF( void ) ;											// 描画用デバイスが有効かどうかを取得する( 戻り値  TRUE:有効  FALSE:無効 )
extern	void	Graphics_D3D11_ReleaseDirectXObject_PF( void ) ;											// DirectX のオブジェクトを解放する








// 環境依存システム関係
extern	void	Graphics_D3D11_WM_ACTIVATE_ActiveProcess_PF( void ) ;									// WM_ACTIVATE メッセージの処理で、ウインドウモード時にアクティブになった際に呼ばれる関数









// 環境依存描画設定関係
extern	int		Graphics_Hardware_D3D11_SetRenderTargetToShader_PF( int TargetIndex, int DrawScreen, int SurfaceIndex, int MipLevel ) ;	// シェーダー描画での描画先を設定する
extern	int		Graphics_Hardware_D3D11_SetDrawBrightToOneParam_PF( DWORD Bright ) ;											// SetDrawBright の引数が一つ版
extern	int		Graphics_Hardware_D3D11_SetDrawBlendMode_PF( int BlendMode, int BlendParam ) ;								// 描画ブレンドモードをセットする
extern	int		Graphics_Hardware_D3D11_SetDrawAlphaTest_PF( int TestMode, int TestParam ) ;									// 描画時のアルファテストの設定を行う( TestMode:DX_CMP_GREATER等( -1:デフォルト動作に戻す )  TestParam:描画アルファ値との比較に使用する値 )
extern	int		Graphics_Hardware_D3D11_SetDrawMode_PF( int DrawMode ) ;														// 描画モードをセットする
extern	int		Graphics_Hardware_D3D11_SetDrawBright_PF( int RedBright, int GreenBright, int BlueBright ) ;					// 描画輝度をセット
extern	int		Graphics_Hardware_D3D11_SetBlendGraphParamBase_PF( IMAGEDATA *BlendImage, int BlendType, int *Param ) ;		// SetBlendGraphParam の可変長引数パラメータ付き
extern	int		Graphics_Hardware_D3D11_SetMaxAnisotropy_PF( int MaxAnisotropy ) ;											// 最大異方性の値をセットする
extern	int		Graphics_Hardware_D3D11_SetTransformToWorld_PF( const MATRIX *Matrix ) ;										// ワールド変換用行列をセットする
extern	int		Graphics_Hardware_D3D11_SetTransformToView_PF( const MATRIX *Matrix ) ;										// ビュー変換用行列をセットする
extern	int		Graphics_Hardware_D3D11_SetTransformToProjection_PF( const MATRIX *Matrix ) ;								// 投影変換用行列をセットする
extern	int		Graphics_Hardware_D3D11_SetTransformToViewport_PF( const MATRIX *Matrix ) ;									// ビューポート行列をセットする
extern	int		Graphics_Hardware_D3D11_SetTextureAddressMode_PF( int Mode /* DX_TEXADDRESS_WRAP 等 */, int Stage ) ;		// テクスチャアドレスモードを設定する
extern	int		Graphics_Hardware_D3D11_SetTextureAddressModeUV_PF( int ModeU, int ModeV, int Stage ) ;						// テクスチャアドレスモードを設定する
extern	int		Graphics_Hardware_D3D11_SetTextureAddressTransformMatrix_PF( int UseFlag, MATRIX *Matrix, int Sampler = -1 ) ;// テクスチャ座標変換行列をセットする
extern	int		Graphics_Hardware_D3D11_SetFogEnable_PF( int Flag ) ;														// フォグを有効にするかどうかを設定する( TRUE:有効  FALSE:無効 )
extern	int		Graphics_Hardware_D3D11_SetFogMode_PF( int Mode /* DX_FOGMODE_NONE 等 */ ) ;									// フォグモードを設定する
extern	int		Graphics_Hardware_D3D11_SetFogColor_PF( DWORD FogColor ) ;													// フォグカラーを変更する
extern	int		Graphics_Hardware_D3D11_SetFogStartEnd_PF( float start, float end ) ;										// フォグが始まる距離と終了する距離を設定する( 0.0f 〜 1.0f )
extern	int		Graphics_Hardware_D3D11_SetFogDensity_PF( float density ) ;													// フォグの密度を設定する( 0.0f 〜 1.0f )
extern	int		Graphics_Hardware_D3D11_ApplyLigFogToHardware_PF( void ) ;													// 基本データに設定されているフォグ情報をハードウェアに反映する
extern	int		Graphics_Hardware_D3D11_SetUseOldDrawModiGraphCodeFlag_PF( int Flag ) ;										// 以前の DrawModiGraph 関数のコードを使用するかどうかのフラグをセットする
extern	int		Graphics_Hardware_D3D11_RefreshAlphaChDrawMode_PF( void ) ;													// 描画先に正しいα値を書き込むかどうかのフラグを更新する








// 環境依存設定関係
extern	int		Graphics_Hardware_D3D11_SetUsePixelLighting_PF( int Flag ) ;																	// ピクセル単位でライティングを行うかどうかを設定する、要 ShaderModel 3.0( TRUE:ピクセル単位のライティングを行う  FALSE:頂点単位のライティングを行う( デフォルト ) )
extern	int		Graphics_Hardware_D3D11_SetGraphicsDeviceRestoreCallbackFunction_PF( void (* Callback )( void *Data ), void *CallbackData ) ;	// グラフィックスデバイスがロストから復帰した際に呼ばれるコールバック関数を設定する
extern	int		Graphics_Hardware_D3D11_SetGraphicsDeviceLostCallbackFunction_PF( void (* Callback )( void *Data ), void *CallbackData ) ;		// グラフィックスデバイスがロストから復帰する前に呼ばれるコールバック関数を設定する
extern	int		Graphics_Hardware_D3D11_RefreshDxLibDirect3DSetting_PF( void ) ;																// ＤＸライブラリのＤｉｒｅｃｔ３Ｄ設定をしなおす
extern	int		Graphics_D3D11_SetUseDirect3D11MinFeatureLevel_PF( int Level /* DX_DIRECT3D_11_FEATURE_LEVEL_10_0 など */ ) ;					// Direct3D11 で使用する最低機能レベルを指定する関数です、尚、DX_DIRECT3D_11_FEATURE_LEVEL_11_0 より低い機能レベルでの正常な動作は保証しません( デフォルトは DX_DIRECT3D_11_FEATURE_LEVEL_11_0 )








// 環境依存画面関係
extern	int		Graphics_Hardware_D3D11_SetupUseZBuffer_PF( void ) ;															// 設定に基づいて使用する深度バッファをセットする
extern	int		Graphics_Hardware_D3D11_ClearDrawScreenZBuffer_PF( const RECT *ClearRect ) ;									// 画面の深度バッファの状態を初期化する
extern	int		Graphics_Hardware_D3D11_ClearDrawScreen_PF( const RECT *ClearRect ) ;										// 画面の状態を初期化する
extern	int		Graphics_Hardware_D3D11_SetDrawScreen_PF( int DrawScreen, int OldScreenSurface, int OldScreenMipLevel, IMAGEDATA *NewTargetImage, IMAGEDATA *OldTargetImage, SHADOWMAPDATA *NewTargetShadowMap, SHADOWMAPDATA *OldTargetShadowMap ) ;						// 描画先画面のセット
extern	int		Graphics_Hardware_D3D11_SetDrawScreen_Post_PF( int DrawScreen ) ;											// SetDrawScreen の最後で呼ばれる関数
extern	int		Graphics_Hardware_D3D11_SetDrawArea_PF( int x1, int y1, int x2, int y2 ) ;									// 描画可能領域のセット
extern	int		Graphics_Hardware_D3D11_LockDrawScreenBuffer_PF( RECT *LockRect, BASEIMAGE *BaseImage, int TargetScreen, IMAGEDATA *TargetImage, int TargetScreenSurface, int TargetScreenMipLevel, int ReadOnly, int TargetScreenTextureNo ) ;	// 描画先バッファをロックする
extern	int		Graphics_Hardware_D3D11_UnlockDrawScreenBuffer_PF( void ) ;													// 描画先バッファをアンロックする
extern	int		Graphics_Hardware_D3D11_ScreenCopy_PF( void ) ;																// 裏画面の内容を表画面に描画する
extern	int		Graphics_Hardware_D3D11_WaitVSync_PF( int SyncNum ) ;														// 垂直同期信号を待つ
extern	int		Graphics_Hardware_D3D11_ScreenFlipBase_PF( void ) ;															// 裏画面と表画面を交換する
extern	int		Graphics_Hardware_D3D11_BltRectBackScreenToWindow_PF( HWND Window, RECT BackScreenRect, RECT WindowClientRect ) ;		// 裏画面の指定の領域をウインドウのクライアント領域の指定の領域に転送する
extern	int		Graphics_Hardware_D3D11_SetScreenFlipTargetWindow_PF( HWND TargetWindow, double ScaleX, double ScaleY ) ;	// ScreenFlip で画像を転送する先のウインドウを設定する( NULL を指定すると設定解除 )
extern	int		Graphics_Hardware_D3D11_SetZBufferMode_PF( int ZBufferSizeX, int ZBufferSizeY, int ZBufferBitDepth ) ;		// メイン画面のＺバッファの設定を変更する
extern	int		Graphics_Hardware_D3D11_SetDrawZBuffer_PF( int DrawScreen, IMAGEDATA *Image ) ;								// 描画先Ｚバッファのセット
extern	const void*	Graphics_Hardware_D3D11_GetUseDirect3D11Device_PF( void ) ;												// 使用中のID3D11Deviceオブジェクトを取得する( 戻り値を ID3D11Device * にキャストしてください )
extern	const void*	Graphics_Hardware_D3D11_GetUseDirect3D11DeviceContext_PF( void ) ;										// 使用中のID3D11DeviceContextオブジェクトを取得する( 戻り値を ID3D11DeviceContext * にキャストしてください )
extern	const void*	Graphics_Hardware_D3D11_GetUseDirect3D11BackBufferTexture2D_PF( void ) ;								// 使用中のバックバッファのID3D11Texture2Dオブジェクトを取得する( 戻り値を ID3D11Texture2D * にキャストしてください )
extern	const void*	Graphics_Hardware_D3D11_GetUseDirect3D11BackBufferRenderTargetView_PF( void ) ;							// 使用中のバックバッファのID3D11RenderTargetViewオブジェクトを取得する( 戻り値を ID3D11RenderTargetView * にキャストしてください )
extern	const void*	Graphics_Hardware_D3D11_GetUseDirect3D11DepthStencilTexture2D_PF( void ) ;								// 使用中の深度ステンシルバッファのID3D11Texture2Dオブジェクトを取得する( 戻り値を ID3D11Texture2D * にキャストしてください )
extern	const void*	Graphics_Hardware_D3D11_GetGraphID3D11Texture2D_PF( IMAGEDATA *Image ) ;								// グラフィックハンドルが持つ ID3D11Texture2D を取得する( Direct3D11 を使用している場合のみ有効 )( 戻り値を ID3D11Texture2D * にキャストしてください )
extern	const void*	Graphics_Hardware_D3D11_GetGraphID3D11RenderTargetView_PF( IMAGEDATA *Image ) ;							// グラフィックハンドルが持つ ID3D11RenderTargetView を取得する( Direct3D11 を使用していて、且つ MakeScreen で作成したグラフィックハンドルでのみ有効 )( 戻り値を ID3D11RenderTargetView * にキャストしてください )
extern	const void*	Graphics_Hardware_D3D11_GetGraphID3D11DepthStencilView_PF( IMAGEDATA *Image ) ;							// グラフィックハンドルが持つ ID3D11DepthStencilView を取得する( Direct3D11 を使用していて、且つ MakeScreen で作成したグラフィックハンドルでのみ有効 )( 戻り値を ID3D11DepthStencilView * にキャストしてください )
extern	int			Graphics_Hardware_D3D11_SetDrawScreen_ID3D11RenderTargetView_PF( const void *pID3D11RenderTargetView, const void *pID3D11DepthStencilView ) ; // 指定の ID3D11RenderTargetView を描画対象にする








// 環境依存情報取得関係
extern	const COLORDATA *	Graphics_Hardware_D3D11_GetMainColorData_PF( void ) ;				// GetColor や GetColor2 で使用するカラーデータを取得する
extern	const COLORDATA *	Graphics_Hardware_D3D11_GetDispColorData_PF( void ) ;				// ディスプレーのカラーデータポインタを得る
extern	DWORD				Graphics_Hardware_D3D11_GetPixel_PF( int x, int y ) ;				// 指定座標の色を取得する
extern	COLOR_F				Graphics_Hardware_D3D11_GetPixelF_PF( int x, int y ) ;				// 指定座標の色を取得する( float型 )








// 環境依存画像関係
extern	int		Graphics_Hardware_D3D11_UpdateGraphMovie_TheoraYUV_PF( MOVIEGRAPH *Movie, IMAGEDATA *Image ) ;			// YUVサーフェスを使った Theora 動画の内容をグラフィックスハンドルのテクスチャに転送する
extern	int		Graphics_Hardware_D3D11_GraphLock_PF( IMAGEDATA *Image, COLORDATA **ColorDataP, int WriteOnly ) ;		// グラフィックメモリ領域のロック
extern	int		Graphics_Hardware_D3D11_GraphUnlock_PF( IMAGEDATA *Image ) ;												// グラフィックメモリ領域のロック解除
extern	int		Graphics_Hardware_D3D11_CopyGraphZBufferImage_PF( IMAGEDATA *DestImage, IMAGEDATA *SrcImage ) ;			// グラフィックのＺバッファの状態を別のグラフィックのＺバッファにコピーする( DestGrHandle も SrcGrHandle もＺバッファを持っている描画可能画像で、且つアンチエイリアス画像ではないことが条件 )
extern	int		Graphics_Hardware_D3D11_InitGraph_PF( void ) ;															// 画像データの初期化
extern	int		Graphics_Hardware_D3D11_FillGraph_PF( IMAGEDATA *Image, int Red, int Green, int Blue, int Alpha, int ASyncThread ) ;			// グラフィックを特定の色で塗りつぶす
extern	int		Graphics_Hardware_D3D11_GetDrawScreenGraphBase_PF( IMAGEDATA *Image, IMAGEDATA *TargetImage, int TargetScreen, int TargetScreenSurface, int TargetScreenMipLevel, int TargetScreenWidth, int TargetScreenHeight, int x1, int y1, int x2, int y2, int destX, int destY ) ;		// 描画可能画像やバックバッファから指定領域のグラフィックを取得する

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
) ;

extern	int		Graphics_Hardware_D3D11_CreateOrigTexture_PF(  IMAGEDATA_ORIG *Orig, int ASyncThread = FALSE ) ;	// オリジナル画像情報中のテクスチャを作成する( 0:成功  -1:失敗 )
extern	int		Graphics_Hardware_D3D11_ReleaseOrigTexture_PF( IMAGEDATA_ORIG *Orig ) ;								// オリジナル画像情報中のテクスチャを解放する
extern	int		Graphics_Hardware_D3D11_GetMultiSampleQuality_PF( int Samples ) ;									// 指定のマルチサンプル数で使用できる最大クオリティ値を取得する
extern	int		Graphics_Hardware_D3D11_SetUsePlatformTextureFormat_PF( int PlatformTextureFormat ) ;				// 作成するグラフィックハンドルで使用する環境依存のテクスチャフォーマットを指定する








// 環境依存頂点バッファ・インデックスバッファ関係
extern	int		Graphics_Hardware_D3D11_VertexBuffer_Create_PF(    VERTEXBUFFERHANDLEDATA *VertexBuffer ) ;															// 頂点バッファハンドルの頂点バッファを作成する
extern	int		Graphics_Hardware_D3D11_VertexBuffer_Terminate_PF( VERTEXBUFFERHANDLEDATA *VertexBuffer ) ;															// 頂点バッファハンドルの後始末
extern	int		Graphics_Hardware_D3D11_VertexBuffer_SetData_PF(   VERTEXBUFFERHANDLEDATA *VertexBuffer, int SetIndex, const void *VertexData, int VertexNum ) ;	// 頂点バッファに頂点データを転送する
extern	int		Graphics_Hardware_D3D11_IndexBuffer_Create_PF(     INDEXBUFFERHANDLEDATA *IndexBuffer ) ;															// インデックスバッファハンドルのセットアップを行う
extern	int		Graphics_Hardware_D3D11_IndexBuffer_Terminate_PF(  INDEXBUFFERHANDLEDATA *IndexBuffer ) ;															// インデックスバッファハンドルの後始末
extern	int		Graphics_Hardware_D3D11_IndexBuffer_SetData_PF(    INDEXBUFFERHANDLEDATA *IndexBuffer, int SetIndex, const void *IndexData, int IndexNum ) ;		// インデックスバッファにインデックスデータを転送する









// 環境依存ライト関係
extern	int		Graphics_Hardware_D3D11_Light_SetUse_PF( int Flag ) ;															// ライティングを行うかどうかを設定する
extern	int		Graphics_Hardware_D3D11_Light_GlobalAmbient_PF( COLOR_F *Color ) ;												// グローバルアンビエントライトカラーを設定する
extern	int		Graphics_Hardware_D3D11_Light_SetState_PF( int LightNumber, LIGHTPARAM *LightParam ) ;							// ライトパラメータをセット
extern	int		Graphics_Hardware_D3D11_Light_SetEnable_PF( int LightNumber, int EnableState ) ;									// ライトの有効、無効を変更








// 環境依存シャドウマップ関係
extern	int		Graphics_Hardware_D3D11_ShadowMap_CreateTexture_PF( SHADOWMAPDATA *ShadowMap, int ASyncThread = FALSE ) ;		// シャドウマップデータに必要なテクスチャを作成する
extern	int		Graphics_Hardware_D3D11_ShadowMap_ReleaseTexture_PF( SHADOWMAPDATA *ShadowMap ) ;								// シャドウマップデータに必要なテクスチャを解放する
extern	void	Graphics_Hardware_D3D11_ShadowMap_RefreshVSParam_PF( void ) ;													// 頂点シェーダーに設定するシャドウマップの情報を更新する
extern	void	Graphics_Hardware_D3D11_ShadowMap_RefreshPSParam_PF( void ) ;													// ピクセルシェーダーに設定するシャドウマップの情報を更新する
extern	int		Graphics_Hardware_D3D11_ShadowMap_DrawSetup_PF( SHADOWMAPDATA *ShadowMap ) ;										// シャドウマップへの描画の準備を行う
extern	int		Graphics_Hardware_D3D11_ShadowMap_DrawEnd_PF( SHADOWMAPDATA *ShadowMap ) ;										// シャドウマップへの描画を終了する
extern	int		Graphics_Hardware_D3D11_ShadowMap_SetUse_PF( int SlotIndex, SHADOWMAPDATA *ShadowMap ) ;							// 描画で使用するシャドウマップを指定する、スロットは０か１かを指定可能








// 環境依存シェーダー関係
extern	int		Graphics_Hardware_D3D11_Shader_Create_PF( int ShaderHandle, int IsVertexShader, void *Image, int ImageSize, int ImageAfterFree, int ASyncThread ) ;		// シェーダーハンドルを初期化する
extern	int		Graphics_Hardware_D3D11_Shader_TerminateHandle_PF( SHADERHANDLEDATA *Shader ) ;																			// シェーダーハンドルの後始末
extern	int		Graphics_Hardware_D3D11_Shader_GetValidShaderVersion_PF( void ) ;																							// 使用できるシェーダーのバージョンを取得する( 0=使えない  200=シェーダーモデル２．０が使用可能  300=シェーダーモデル３．０が使用可能 )
extern	int		Graphics_Hardware_D3D11_Shader_GetConstIndex_PF( const wchar_t *ConstantName, SHADERHANDLEDATA *Shader ) ;													// 指定の名前を持つ定数が使用するシェーダー定数の番号を取得する
extern	int		Graphics_Hardware_D3D11_Shader_GetConstCount_PF( const wchar_t *ConstantName, SHADERHANDLEDATA *Shader ) ;													// 指定の名前を持つ定数が使用するシェーダー定数の数を取得する
extern	const FLOAT4 *Graphics_Hardware_D3D11_Shader_GetConstDefaultParamF_PF( const wchar_t *ConstantName, SHADERHANDLEDATA *Shader ) ;									// 指定の名前を持つ浮動小数点定数のデフォルトパラメータが格納されているメモリアドレスを取得する
extern	int		Graphics_Hardware_D3D11_Shader_SetConst_PF(   int TypeIndex, int SetIndex, int ConstantIndex, const void *Param, int ParamNum, int UpdateUseArea ) ;	// シェーダー定数情報を設定する
extern	int		Graphics_Hardware_D3D11_Shader_ResetConst_PF( int TypeIndex, int SetIndex, int ConstantIndex, int ParamNum ) ;											// 指定領域のシェーダー定数情報をリセットする
extern	int		Graphics_Hardware_D3D11_Shader_ModelCode_Init_PF( void ) ;																								// ３Ｄモデル用のシェーダーコードの初期化を行う








// 環境依存シェーダー用定数バッファ関係
extern	int		Graphics_Hardware_D3D11_ShaderConstantBuffer_Create_PF( int ShaderConstantBufferHandle, int BufferSize, int ASyncThread ) ;												// シェーダー用定数バッファハンドルを初期化する
extern	int		Graphics_Hardware_D3D11_ShaderConstantBuffer_TerminateHandle_PF( SHADERCONSTANTBUFFERHANDLEDATA *ShaderConstantBuffer ) ;													// シェーダー用定数バッファハンドルの後始末
extern	void *	Graphics_Hardware_D3D11_ShaderConstantBuffer_GetBuffer_PF( SHADERCONSTANTBUFFERHANDLEDATA *ShaderConstantBuffer ) ;														// シェーダー用定数バッファハンドルの定数バッファのアドレスを取得する
extern	int		Graphics_Hardware_D3D11_ShaderConstantBuffer_Update_PF( SHADERCONSTANTBUFFERHANDLEDATA *ShaderConstantBuffer ) ;															// シェーダー用定数バッファハンドルの定数バッファへの変更を適用する
extern	int		Graphics_Hardware_D3D11_ShaderConstantBuffer_Set_PF( SHADERCONSTANTBUFFERHANDLEDATA *ShaderConstantBuffer, int TargetShader /* DX_SHADERTYPE_VERTEX など */, int Slot ) ;	// シェーダー用定数バッファハンドルの定数バッファを指定のシェーダーの指定のスロットにセットする








// 環境依存描画関係
extern	int		Graphics_Hardware_D3D11_Paint_PF( int x, int y, unsigned int FillColor, ULONGLONG BoundaryColor ) ;			// 指定点から境界色があるところまで塗りつぶす






#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_NON_DIRECT3D11

#endif // DX_NON_GRAPHICS

#endif // __DXGRAPHICSD3D11_H__

