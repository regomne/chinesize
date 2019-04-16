// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		Direct3D11 シェーダー共通定義コード
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXSHADER_COMMON_D3D11_H__
#define __DXSHADER_COMMON_D3D11_H__

// マクロ定義 -----------------------------------

#define DX_D3D11_COMMON_CONST_LIGHT_NUM			(6)		// 共通パラメータのライトの最大数

// 構造体定義 --------------------------------------------------------------------

// マテリアルパラメータ
struct DX_D3D11_CONST_MATERIAL
{
	DX_D3D11_SHADER_FLOAT4		Diffuse ;				// ディフューズカラー
	DX_D3D11_SHADER_FLOAT4		Specular ;				// スペキュラカラー
	DX_D3D11_SHADER_FLOAT4		Ambient_Emissive ;		// マテリアルエミッシブカラー + マテリアルアンビエントカラー * グローバルアンビエントカラー

	DX_D3D11_SHADER_FLOAT		Power ;					// スペキュラの強さ
	DX_D3D11_SHADER_FLOAT		TypeParam0 ;			// マテリアルタイプパラメータ0
	DX_D3D11_SHADER_FLOAT		TypeParam1 ;			// マテリアルタイプパラメータ1
	DX_D3D11_SHADER_FLOAT		TypeParam2 ;			// マテリアルタイプパラメータ2
} ;

// フォグパラメータ
struct DX_D3D11_VS_CONST_FOG
{
	DX_D3D11_SHADER_FLOAT		LinearAdd ;				// フォグ用パラメータ end / ( end - start )
	DX_D3D11_SHADER_FLOAT		LinearDiv ;				// フォグ用パラメータ -1  / ( end - start )
	DX_D3D11_SHADER_FLOAT		Density ;				// フォグ用パラメータ density
	DX_D3D11_SHADER_FLOAT		E ;						// フォグ用パラメータ 自然対数の低

	DX_D3D11_SHADER_FLOAT4		Color ;					// カラー
} ;

// ライトパラメータ
struct DX_D3D11_CONST_LIGHT
{
	DX_D3D11_SHADER_INT			Type ;					// ライトタイプ( DX_LIGHTTYPE_POINT など )
	DX_D3D11_SHADER_INT3		Padding1 ;				// パディング１

	DX_D3D11_SHADER_FLOAT3		Position ;				// 座標( ビュー空間 )
	DX_D3D11_SHADER_FLOAT		RangePow2 ;				// 有効距離の２乗

	DX_D3D11_SHADER_FLOAT3		Direction ;				// 方向( ビュー空間 )
	DX_D3D11_SHADER_FLOAT		FallOff ;				// スポットライト用FallOff

	DX_D3D11_SHADER_FLOAT3		Diffuse ;				// ディフューズカラー
	DX_D3D11_SHADER_FLOAT		SpotParam0 ;			// スポットライト用パラメータ０( cos( Phi / 2.0f ) )

	DX_D3D11_SHADER_FLOAT3		Specular ;				// スペキュラカラー
	DX_D3D11_SHADER_FLOAT		SpotParam1 ;			// スポットライト用パラメータ１( 1.0f / ( cos( Theta / 2.0f ) - cos( Phi / 2.0f ) ) )

	DX_D3D11_SHADER_FLOAT4		Ambient ;				// アンビエントカラーとマテリアルのアンビエントカラーを乗算したもの

	DX_D3D11_SHADER_FLOAT		Attenuation0 ;			// 距離による減衰処理用パラメータ０
	DX_D3D11_SHADER_FLOAT		Attenuation1 ;			// 距離による減衰処理用パラメータ１
	DX_D3D11_SHADER_FLOAT		Attenuation2 ;			// 距離による減衰処理用パラメータ２
	DX_D3D11_SHADER_FLOAT		Padding2 ;				// パディング２
} ;

// ピクセルシェーダー・頂点シェーダー共通パラメータ
struct DX_D3D11_CONST_BUFFER_COMMON
{
	DX_D3D11_CONST_LIGHT		Light[ DX_D3D11_COMMON_CONST_LIGHT_NUM ] ;			// ライトパラメータ
	DX_D3D11_CONST_MATERIAL		Material ;				// マテリアルパラメータ
	DX_D3D11_VS_CONST_FOG		Fog ;					// フォグパラメータ
} ;

#endif // DXSHADER_COMMON_D3D11

