// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		モデルデータ制御プログラム( Direct3D9 )ヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXMODELD3D9_H__
#define __DXMODELD3D9_H__

#include "../DxCompileConfig.h"

#ifndef DX_NON_DIRECT3D9

// インクルード ------------------------------------------------------------------

#ifndef DX_NON_MODEL

#include "DxDirectX.h"
#include "../DxLib.h"
#include "../DxModel.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

// 構造体定義 --------------------------------------------------------------------

// 剛体メッシュタイプ頂点構造体
struct MV1_VERTEX_SIMPLE_D3D9
{
	VECTOR					Position ;							// 座標
	VECTOR					Normal ;							// 法線
	COLOR_U8				DiffuseColor ;						// ディフューズ色
	COLOR_U8				SpecularColor ;						// スペキュラー色
	float					UVs[ 3 ][ 4 ] ;						// テクスチャ座標
} ;

// 剛体メッシュタイプ輪郭線用頂点構造体
struct MV1_VERTEX_SIMPLE_TOL_D3D9
{
	VECTOR					Position ;							// 座標
	COLOR_U8				DiffuseColor ;						// ディフューズ色
} ;

// 剛体メッシュタイプバンプマップ用頂点構造体
struct MV1_VERTEX_SIMPLE_BUMP_D3D9
{
	VECTOR					Position ;							// 座標
	VECTOR					Tangent ;							// 頂点の接線
	VECTOR					Binormal ;							// 頂点の従法線
	VECTOR					Normal ;							// 法線
	COLOR_U8				DiffuseColor ;						// ディフューズ色
	COLOR_U8				SpecularColor ;						// スペキュラー色
	float					UVs[ 1 ][ 4 ] ;						// テクスチャ座標
} ;

// スキンメッシュタイプ頂点構造体(関連ボーン４つ以内)
struct MV1_VERTEX_SKIN_B4_D3D9
{
	VECTOR					Position ;							// 座標
	unsigned char			MatrixIndex[ 4 ] ;					// 行列インデックス( 行列番号×４ )
	float					MatrixWeight[ 4 ] ;					// 行列ウエイト
	VECTOR					Normal ;							// 法線
	COLOR_U8				DiffuseColor ;						// ディフューズ色
	COLOR_U8				SpecularColor ;						// スペキュラー色
	float					UVs[ 1 ][ 4 ] ;						// テクスチャ座標
} ;

// スキンメッシュタイプバンプマップ用頂点構造体(関連ボーン４つ以内)
struct MV1_VERTEX_SKIN_B4_BUMP_D3D9
{
	VECTOR					Position ;							// 座標
	unsigned char			MatrixIndex[ 4 ] ;					// 行列インデックス( 行列番号×４ )
	float					MatrixWeight[ 4 ] ;					// 行列ウエイト
	VECTOR					Tangent ;							// 頂点の接線
	VECTOR					Binormal ;							// 頂点の従法線
	VECTOR					Normal ;							// 法線
	COLOR_U8				DiffuseColor ;						// ディフューズ色
	COLOR_U8				SpecularColor ;						// スペキュラー色
	float					UVs[ 1 ][ 4 ] ;						// テクスチャ座標
} ;

// スキンメッシュタイプ頂点構造体(関連ボーン８つ以内)
struct MV1_VERTEX_SKIN_B8_D3D9
{
	VECTOR					Position ;							// 座標
	unsigned char			MatrixIndex[ 8 ] ;					// 行列インデックス( 行列番号×４ )
	float					MatrixWeight[ 8 ] ;					// 行列ウエイト
	VECTOR					Normal ;							// 法線
	COLOR_U8				DiffuseColor ;						// ディフューズ色
	COLOR_U8				SpecularColor ;						// スペキュラー色
	float					UVs[ 1 ][ 4 ] ;						// テクスチャ座標
} ;

// スキンメッシュタイプバンプマップ用頂点構造体(関連ボーン８つ以内)
struct MV1_VERTEX_SKIN_B8_BUMP_D3D9
{
	VECTOR					Position ;							// 座標
	unsigned char			MatrixIndex[ 8 ] ;					// 行列インデックス( 行列番号×８ )
	float					MatrixWeight[ 8 ] ;					// 行列ウエイト
	VECTOR					Tangent ;							// 頂点の接線
	VECTOR					Binormal ;							// 頂点の従法線
	VECTOR					Normal ;							// 法線
	COLOR_U8				DiffuseColor ;						// ディフューズ色
	COLOR_U8				SpecularColor ;						// スペキュラー色
	float					UVs[ 1 ][ 4 ] ;						// テクスチャ座標
} ;

// Direct3D9用 モデルデータ管理用構造体
struct MV1_MODEL_MANAGE_DIRECT3D9
{
	int Dummy ;
} ;

// Direct3D9用頂点バッファ情報
struct MV1_VERTEXBUFFER_DIRECT3D9
{
	DWORD						FVF ;							// 頂点バッファ作成時に使用したＦＶＦ
	D_IDirect3DVertexBuffer9	*VertexBuffer ;					// 頂点バッファ
	D_IDirect3DIndexBuffer9		*IndexBuffer ;					// インデックスバッファ
} ;

// Direct3D9用トライアングルリスト基データ情報
struct MV1_TRIANGLE_LIST_BASE_DIRECT3D9
{
	MV1_VERTEX_SIMPLE_D3D9		*TempSimpleVertex ;					// シェーダーを使用せずに描画する場合の頂点データ
	MV1_VERTEX_SIMPLE_TOL_D3D9	*TempToonOutLineSimpleVertex ;		// シェーダーを使用せずに描画する場合のトゥーンの輪郭線描画用頂点データ
} ;

// Direct3D9用トライアングルリスト情報
struct MV1_TRIANGLE_LIST_DIRECT3D9
{
	D_IDirect3DVertexBuffer9	*VertexBuffer ;					// 頂点バッファ
} ;

// 内部大域変数宣言 --------------------------------------------------------------

extern MV1_MODEL_MANAGE_DIRECT3D9 MV1Man_D3D9 ;

// 関数プロトタイプ宣言-----------------------------------------------------------

// 環境依存関数
extern	int				MV1_D3D9_Terminate_PF( void ) ;																// モデル機能の後始末
extern	int				MV1_D3D9_TerminateModelBaseHandle_PF( MV1_MODEL_BASE *ModelBase ) ;							// モデルデータハンドルの後始末
extern	int				MV1_D3D9_TerminateTriangleListBaseTempBuffer_PF( MV1_TRIANGLE_LIST_BASE *MBTList ) ;				// トライアングルリストの一時処理用のバッファを開放する
extern	void			MV1_D3D9_SetupPackDrawInfo_PF( MV1_MODEL_BASE *ModelBase ) ;								// 同時複数描画関係の情報をセットアップする
extern	int				MV1_D3D9_SetupVertexBufferBase_PF( int MV1ModelBaseHandle, int DuplicateNum = 1, int ASyncThread = FALSE ) ;	// モデル基データの頂点バッファのセットアップをする( -1:エラー )
extern	int				MV1_D3D9_SetupVertexBuffer_PF( int MHandle, int ASyncThread = FALSE ) ;						// モデルデータの頂点バッファのセットアップをする( -1:エラー )
extern	int				MV1_D3D9_TerminateVertexBufferBase_PF( int MV1ModelBaseHandle ) ;							// 頂点バッファの後始末をする( -1:エラー )
extern	int				MV1_D3D9_TerminateVertexBuffer_PF( int MV1ModelHandle ) ;									// 頂点バッファの後始末をする( -1:エラー )
extern	int				MV1_D3D9_SetupShapeVertex_PF( int MHandle ) ;												// シェイプデータのセットアップをする
extern	int				MV1_D3D9_BeginRender_PF( MV1_MODEL *Model ) ;												// ３Ｄモデルのレンダリングの準備を行う
extern	int				MV1_D3D9_EndRender_PF( void ) ;																// ３Ｄモデルのレンダリングの後始末を行う
extern	void			MV1_D3D9_DrawMesh_PF( MV1_MESH *Mesh, int TriangleListIndex = -1 ) ;						// メッシュ描画部分を抜き出したもの

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_NON_MODEL

#endif // DX_NON_DIRECT3D9

#endif // __DXMODELD3D9_H__
