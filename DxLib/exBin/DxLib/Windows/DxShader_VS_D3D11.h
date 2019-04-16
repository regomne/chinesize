// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		バーテックスシェーダー関連定義
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXSHADER_VS_D3D11_H__
#define __DXSHADER_VS_D3D11_H__

// インクルード ---------------------------------

#include "DxShader_Common_D3D11.h"

// マクロ定義 -----------------------------------

#define DX_D3D11_VS_CONST_TEXTURE_MATRIX_NUM			3			// テクスチャ座標変換行列の転置行列の数
#define DX_D3D11_VS_CONST_WORLD_MAT_NUM					54			// トライアングルリスト一つで同時に使用するローカル→ワールド行列の最大数

// データ型定義 ---------------------------------

// 基本パラメータ
struct DX_D3D11_VS_CONST_BUFFER_BASE
{
	DX_D3D11_SHADER_FLOAT4		AntiViewportMatrix[ 4 ] ;										// アンチビューポート行列
	DX_D3D11_SHADER_FLOAT4		ProjectionMatrix[ 4 ] ;											// ビュー　→　プロジェクション行列
	DX_D3D11_SHADER_FLOAT4		ViewMatrix[ 3 ] ;												// ワールド　→　ビュー行列
	DX_D3D11_SHADER_FLOAT4		LocalWorldMatrix[ 3 ] ;											// ローカル　→　ワールド行列

	DX_D3D11_SHADER_FLOAT4		ToonOutLineSize ;												// トゥーンの輪郭線の大きさ
	DX_D3D11_SHADER_FLOAT		DiffuseSource ;													// ディフューズカラー( 0.0f:マテリアル  1.0f:頂点 )
	DX_D3D11_SHADER_FLOAT		SpecularSource ;												// スペキュラカラー(   0.0f:マテリアル  1.0f:頂点 )
	DX_D3D11_SHADER_FLOAT		MulSpecularColor ;												// スペキュラカラー値に乗算する値( スペキュラ無効処理で使用 )
	DX_D3D11_SHADER_FLOAT		Padding ;
} ;

// その他の行列
struct DX_D3D11_VS_CONST_BUFFER_OTHERMATRIX
{
	DX_D3D11_SHADER_FLOAT4		ShadowMapLightViewProjectionMatrix[ 3 ][ 4 ] ;					// シャドウマップ用のライトビュー行列とライト射影行列を乗算したもの
	DX_D3D11_SHADER_FLOAT4		TextureMatrix[ DX_D3D11_VS_CONST_TEXTURE_MATRIX_NUM ][ 2 ] ;	// テクスチャ座標操作用行列
} ;

// スキニングメッシュ用の　ローカル　→　ワールド行列
struct DX_D3D11_VS_CONST_BUFFER_LOCALWORLDMATRIX
{
	DX_D3D11_SHADER_FLOAT4		Matrix[ DX_D3D11_VS_CONST_WORLD_MAT_NUM * 3 ] ;					// ローカル　→　ワールド行列
} ;

#endif // __DXSHADER_VS_D3D11_H__
