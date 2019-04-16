// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		モデルデータファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXMODELFILE_H__
#define __DXMODELFILE_H__

// インクルード ------------------------------

#include "DxCompileConfig.h"
#include "DxLib.h"
#include "DxStatic.h"

#ifndef DX_NON_MODEL

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------

#define MV1_TRIANGLE_LIST_INDEX_TYPE_U8						(0x0000)				// インデックス値は unsigned char 型
#define MV1_TRIANGLE_LIST_INDEX_TYPE_U16					(0x0001)				// インデックス値は unsigned short 型
#define MV1_TRIANGLE_LIST_INDEX_TYPE_U32					(0x0002)				// インデックス値は unsigned dword 型

#define MV1_TRIANGLE_LIST_FLAG_MVERT_INDEX_MASK				(0x0003)				// メッシュ頂点インデックスタイプのマスク( MV1_TRIANGLE_LIST_INDEX_TYPE_U32 等 )
#define MV1_TRIANGLE_LIST_FLAG_INDEX_MASK					(0x000c)				// 頂点インデックスタイプのマスク( MV1_TRIANGLE_LIST_INDEX_TYPE_U32 等 )

#define MV1_FRAME_NORMAL_TYPE_NONE							(0x0000)				// 法線は無い( 自動計算する )
#define MV1_FRAME_NORMAL_TYPE_S8							(0x0001)				// 法線は signed char 型
#define MV1_FRAME_NORMAL_TYPE_S16							(0x0002)				// 法線は signed short 型
#define MV1_FRAME_NORMAL_TYPE_F32							(0x0003)				// 法線は float 型

#define MV1_FRAME_MATRIX_INDEX_TYPE_U8						(0x0000)				// 行列インデックスは unsigned char 型
#define MV1_FRAME_MATRIX_INDEX_TYPE_U16						(0x0001)				// 行列インデックスは unsigned short 型

#define MV1_FRAME_MATRIX_WEIGHT_TYPE_U8						(0x0000)				// 行列ウエイト値は unsigned char 型
#define MV1_FRAME_MATRIX_WEIGHT_TYPE_U16					(0x0001)				// 行列ウエイト値は unsigned short 型

#define MV1_FRAME_VERT_FLAG_NORMAL_TYPE_MASK				(0x0003)				// 法線タイプマスク( MV1_FRAME_NORMAL_TYPE_NONE 等 )
#define MV1_FRAME_VERT_FLAG_POSITION_B16					(0x0004)				// このフラグが立っていたら座標値は16ビット
#define MV1_FRAME_VERT_FLAG_MATRIX_WEIGHT_NONE				(0x0008)				// このフラグが立っていたらウエイト値は無い
#define MV1_FRAME_VERT_FLAG_MATRIX_INDEX_MASK				(0x0010)				// 行列インデックスタイプマスク( MV1_FRAME_MATRIX_INDEX_TYPE_U8 等 )
#define MV1_FRAME_VERT_FLAG_MATRIX_WEIGHT_MASK				(0x0020)				// 行列ウエイトタイプマスク( MV1_FRAME_MATRIX_WEIGHT_TYPE_U8 等 )
#define MV1_FRAME_VERT_FLAG_NOMRAL_TANGENT_BINORMAL			(0x0040)				// このフラグが立っていたら法線情報は、法線、接線、従法線がある


#define MV1_MESH_VERT_INDEX_TYPE_NONE						(0x0000)				// インデックス値は無い
#define MV1_MESH_VERT_INDEX_TYPE_U8							(0x0001)				// インデックス値は unsigned char 型
#define MV1_MESH_VERT_INDEX_TYPE_U16						(0x0002)				// インデックス値は unsigned short 型
#define MV1_MESH_VERT_INDEX_TYPE_U32						(0x0003)				// インデックス値は unsigned int 型

#define MV1_MESH_VERT_FLAG_POS_IND_TYPE_MASK				(0x0003)				// 座標インデックスタイプマスク( MV1_MESH_VERT_INDEX_TYPE_U8 等 )
#define MV1_MESH_VERT_FLAG_NRM_IND_TYPE_MASK				(0x000c)				// 法線インデックスタイプマスク( MV1_MESH_VERT_INDEX_TYPE_U8 等 )
#define MV1_MESH_VERT_FLAG_UV_U16							(0x0010)				// このフラグが立っていたらＵＶ値は16ビット
#define MV1_MESH_VERT_FLAG_COMMON_COLOR						(0x0020)				// このフラグが立っていたら頂点カラーは共通で一つ
#define MV1_MESH_VERT_FLAG_NON_TOON_OUTLINE					(0x0040)				// このフラグが立っていたら各頂点にトゥーンの輪郭線を描画するかどうかの情報が格納されている


#define MV1_TEXTURE_FLAG_REVERSE							(0x0001)				// このフラグが立っていたら画像を反転する
#define MV1_TEXTURE_FLAG_BMP32_ALL_ZERO_ALPHA_TO_XRGB8		(0x0002)				// このフラグが立っていたら３２ビットＢＭＰのアルファ値がすべて０のときは XRGB8 として扱う
#define MV1_TEXTURE_FLAG_VALID_SCALE_UV						(0x0004)				// このフラグが立っていたらＵＶ座標のスケールが有効


#define MV1_ANIM_KEYSET_FLAG_KEY_ONE						(0x0001)				// このフラグが立っていたらキーの数は一つ
#define MV1_ANIM_KEYSET_FLAG_KEYNUM_B						(0x0002)				// このフラグが立っていたらキーの数は１バイト
#define MV1_ANIM_KEYSET_FLAG_KEYNUM_W						(0x0004)				// このフラグが立っていたらキーの数は２バイト
#define MV1_ANIM_KEYSET_FLAG_TIME_UNIT						(0x0008)				// このフラグが立っていたらタイム配列の代わりに等間隔タイム情報が入っている
#define MV1_ANIM_KEYSET_FLAG_TIME_UNIT_ST_W					(0x0010)				// このフラグが立っていたらタイム開始値はWORD値
#define MV1_ANIM_KEYSET_FLAG_TIME_UNIT_ST_Z					(0x0020)				// このフラグが立っていたらタイム開始値は０
#define MV1_ANIM_KEYSET_FLAG_TIME_UNIT_UN_W					(0x0040)				// このフラグが立っていたらタイム間隔値はWORD値
#define MV1_ANIM_KEYSET_FLAG_TIME_BIT16						(0x0080)				// このフラグが立っていたらタイム配列の値が１６ビット値
#define MV1_ANIM_KEYSET_FLAG_KEY_BIT16						(0x0100)				// このフラグが立っていたらキーは１６ビット値
#define MV1_ANIM_KEYSET_FLAG_KEY_MP_PP						(0x0200)				// このフラグが立っていたらキーは -PI 〜 PI を 0〜65535 で表したもの
#define MV1_ANIM_KEYSET_FLAG_KEY_Z_TP						(0x0400)				// このフラグが立っていたらキーは 0.0f 〜 2 * PI を 0〜65535 で表したもの

// 構造体定義 --------------------------------

// プロトタイプ宣言
struct MV1_MESH_SET_F1 ;
struct MV1_FRAME_F1 ;
struct MV1_MESH_F1 ;
struct MV1_ANIMSET_F1 ;

// 回転情報構造体
struct MV1_ROTATE_F1
{
	int										Type ;								// 回転タイプ( MV1_ROTATE_TYPE_XYZROT 等 )
	union
	{
		VECTOR								XYZRot ;							// XYZ回転用
		FLOAT4								Qt ;								// クォータニオン回転用
		MATRIX_4X4CT_F						Mat ;								// 行列回転用
	} ;
} ;

// 16bitデータタイプ用補助情報構造体
struct MV1_ANIM_KEY_16BIT_F1
{
	BYTE									Min ;								// 最小値( bit7:０かどうか( 0:0以外 1:0 )  bit6:符号(0:+ 1:-)  bit5:乗数方向(0:+ 1:-) bit4〜0:乗数(最大10の15乗) ) 
	BYTE									Unit ;								// 16bit値１辺りの値( bit7:乗数方向(0:+ 1:-) bit6〜4:乗数(最大10の7乗) bit3〜0:乗算される値( 0〜15 ) )
} ;

// アニメーションキー(４×４行列の４列目が(0,0,0,1)固定の行列タイプ)データ構造体
struct MV1_ANIM_KEY_MATRIX4X4C_F1
{
	float									Matrix[ 4 ][ 3 ] ;					// 行列
} ;

// アニメーションキー(３×３行列タイプ)データ構造体
struct MV1_ANIM_KEY_MATRIX3X3_F1
{
	float									Matrix[ 3 ][ 3 ] ;					// 行列
} ;

// アニメーションキー(３×３行列１６ビットタイプ)データ構造体
struct MV1_ANIM_KEY_MATRIX3X3_B16_F1
{
	WORD									Matrix[ 3 ][ 3 ] ;					// 行列
} ;

// アニメーションキーリスト基データ構造体
/*
	KeyData の情報格納順序

	基本情報

		Type が MV1_ANIMKEY_DATATYPE_SHAPE の場合
			WORD								TargetShapeIndex ;		// 対象のシェイプインデックス
	
		Flag に MV1_ANIM_KEYSET_FLAG_KEY_ONE が付いていない場合
			Flag に MV1_ANIM_KEYSET_FLAG_KEYNUM_B が付いている場合
				BYTE							Num ;					// キーの数

			Flag に MV1_ANIM_KEYSET_FLAG_KEYNUM_W が付いている場合
				WORD							Num ;					// キーの数

			Flag に MV1_ANIM_KEYSET_FLAG_KEYNUM_W も MV1_ANIM_KEYSET_FLAG_KEYNUM_B も付いていない場合
				DWORD							Num ;					// キーの数


			Flag に MV1_ANIM_KEYSET_FLAG_TIME_BIT16 が付いている場合
				MV1_ANIM_KEY_16BIT_F1			Time16BSub ;			// 16bitタイム値用補助情報
				WORD タイム値列

			Flag に MV1_ANIM_KEYSET_FLAG_TIME_BIT16 も MV1_ANIM_KEYSET_FLAG_TIME_UNIT もついていない場合
				float タイム値列

			Flag に MV1_ANIM_KEYSET_FLAG_TIME_UNIT が付いている場合
				Flag に MV1_ANIM_KEYSET_FLAG_TIME_UNIT_ST_W が付いている場合
					WORD						StartTime ;				// 開始時間

				Flag に MV1_ANIM_KEYSET_FLAG_TIME_UNIT_ST_Z も MV1_ANIM_KEYSET_FLAG_TIME_UNIT_ST_W もついていない場合
					float						StartTime ;				// 開始時間

				Flag に MV1_ANIM_KEYSET_FLAG_TIME_UNIT_UN_W が付いている場合
					WORD						UnitTime ;				// 単位時間

				Flag に MV1_ANIM_KEYSET_FLAG_TIME_UNIT_UN_W が付いていない場合
					float						UnitTime ;				// 単位時間

		Flag に MV1_ANIM_KEYSET_FLAG_VAL_MP_PP も MV1_ANIM_KEYSET_FLAG_VAL_Z_TP も付いてなくて、且つ MV1_ANIM_KEYSET_FLAG_KEY_BIT16 が付いている場合
			MV1_ANIM_KEY_16BIT_F1				Key16BSub ;				// 16bitキー値用補助情報


	キー情報
		各フォーマットに沿ったデータ

*/
struct MV1_ANIM_KEYSET_F1
{
	char									Type ;								// キータイプ( MV1_ANIMKEY_TYPE_QUATERNION 等 )
	char									DataType ;							// データタイプ( MV1_ANIMKEY_DATATYPE_ROTATE 等 )
	WORD									Flag ;								// フラグ
	DWORD/*void* */							KeyData ;							// キーデータ

	DWORD									UserData[ 1 ] ;						// 外部定義の情報

	DWORD									Padding[ 2 ] ;
} ;

// アニメーションデータ構造体
struct MV1_ANIM_F1
{
	int										Index ;								// インデックス

	DWORD/*MV1_ANIMSET_F1* */				Container ;							// このアニメーションを持っているアニメーションセットへのポインタ

	int										TargetFrameIndex ;					// 対象となるフレームのインデックス
	float									MaxTime ;							// 各キーセットの中で一番長いキーセットの時間値
	int										RotateOrder ;						// 回転オーダー( MV1_ROTATE_ORDER_XYZ 等 )

	int										KeySetNum ;							// キーセットの数
	DWORD/*MV1_ANIM_KEYSET_F1* */			KeySet ;							// キーセット配列へのポインタ

	DWORD									UserData[ 2 ] ;						// 外部定義の情報

	DWORD									Padding[ 2 ] ;
} ;

// アニメーションセット構造体
struct MV1_ANIMSET_F1
{
	DWORD/*MV1_ANIMSET_F1* */				DimPrev ;							// 前のデータへのポインタ
	DWORD/*MV1_ANIMSET_F1* */				DimNext ;							// 次のデータへのポインタ
	DWORD/*char* */							Name ;								// アニメーションセット名

	int										Index ;								// インデックス
	float									MaxTime ;							// 各アニメーションの中で一番長いアニメーションの時間値
	int										AnimNum ;							// アニメーションの数
	DWORD/*MV1_ANIM_F1* */					Anim ;								// アニメーションリスト

	DWORD									UserData[ 4 ] ;						// 外部定義の情報

	DWORD									Flag ;								// フラグ( 0bit:加算アニメーションか  1bit:キーの補間は行列線形補間か  2bit:ループアニメーションか )
	DWORD									Padding[ 3 ] ;
} ;

// テクスチャ構造体
struct MV1_TEXTURE_F1
{
	DWORD/*MV1_TEXTURE_F1* */				DimPrev ;							// 前のデータへのポインタ
	DWORD/*MV1_TEXTURE_F1* */				DimNext ;							// 次のデータへのポインタ
	DWORD/*char* */							Name ;								// テクスチャオブジェクトにつけられた名前
	int										Index ;								// インデックス

	DWORD/*char* */							ColorFilePath ;						// カラーチャンネル用画像ファイルの相対パス
	DWORD/*char* */							AlphaFilePath ;						// アルファチャンネル用画像ファイルの相対パス
	int										BumpImageFlag ;						// カラーチャンネルがバンプマップかどうか( TRUE:バンプマップ  FALSE:違う )
	float									BumpImageNextPixelLength ;			// バンプマップ画像の場合の隣のピクセルとの距離

	int										AddressModeU ;						// アドレスモード( MV1_TEXTURE_ADDRESS_MODE_WRAP 等 )
	int										AddressModeV ;						// アドレスモード( MV1_TEXTURE_ADDRESS_MODE_WRAP 等 )
	int										FilterMode ;						// フィルタリングモード( MV1_TEXTURE_FILTER_MODE_POINT 等 )

	DWORD									UserData[ 2 ] ;						// 外部定義の情報

	BYTE									Flag ;								// フラグ( MV1_TEXTURE_FLAG_REVERSE 等 )
	BYTE									Padding1[ 3 ] ;

	float									ScaleU ;							// Ｕ座標のスケーリング値( フラグ MV1_TEXTURE_FLAG_VALID_SCALE_UV が立っている場合のみ有効 )
	float									ScaleV ;							// Ｖ座標のスケーリング値( フラグ MV1_TEXTURE_FLAG_VALID_SCALE_UV が立っている場合のみ有効 )
	DWORD									Padding[ 1 ] ;
} ;

// マテリアルレイヤー構造体
struct MV1_MATERIAL_LAYER_F1
{
	int										Texture ;							// モデルテクスチャインデックス( MV1_MODEL_F1.Textue 配列のインデックス )
	int										BlendType ;							// ブレンドタイプ( MV1_LAYERBLEND_TYPE_ADDITIVE 等 )

	DWORD									Padding[ 4 ] ;
} ;

// マテリアルトゥーンレンダリング用情報構造体
struct MV1_MATERIAL_TOON_F1
{
	int										Type ;								// マテリアルタイプ( DX_MATERIAL_TYPE_NORMAL など )

	int										DiffuseGradTexture ;				// ( トゥーンレンダリングでのみ使用 )ディフューズカラーグラデーションテクスチャインデックス、当たっているライトのディフューズカラーでＵ値が決まるもの、無効の場合は−１( MV1_MODEL_BASE.Textue 配列のインデックス )
	int										SpecularGradTexture ;				// ( トゥーンレンダリングでのみ使用 )スペキュラカラーグラデーションテクスチャインデックス、当たっているライトのスペキュラカラーでＵ値が決まるもの、無効の場合は−１( MV1_MODEL_BASE.Textue 配列のインデックス )
	int										DiffuseGradBlendType ;				// ( トゥーンレンダリングでのみ使用 )ディフューズグラデーションテクスチャのブレンドタイプ( DX_MATERIAL_BLENDTYPE_TRANSLUCENT など )
	int										SpecularGradBlendType ;				// ( トゥーンレンダリングでのみ使用 )スペキュラグラデーションテクスチャのブレンドタイプ( DX_MATERIAL_BLENDTYPE_ADDITIVE など )
	float									OutLineWidth ;						// ( トゥーンレンダリングでのみ使用 )輪郭線の幅( 0.0f 〜 1.0f )
	COLOR_F									OutLineColor ;						// ( トゥーンレンダリングでのみ使用 )輪郭線の色
	float									OutLineDotWidth ;					// ( トゥーンレンダリングでのみ使用 )輪郭線のドット単位での幅
	BYTE									EnableSphereMap ;					// ( トゥーンレンダリングでのみ使用 )スフィアマップの情報が有効かどうか( 1:有効  0:無効 )
	BYTE									SphereMapBlendType ;				// ( トゥーンレンダリングでのみ使用 )スフィアマップテクスチャのブレンドタイプ( DX_MATERIAL_BLENDTYPE_ADDITIVE など )
	short									SphereMapTexture ;					// ( トゥーンレンダリングでのみ使用 )スフィアマップテクスチャインデックス

	DWORD									Padding[ 2 ] ;
} ;

// マテリアル構造体
struct MV1_MATERIAL_F1
{
	DWORD/*MV1_MATERIAL_F1* */				DimPrev ;							// 前のデータへのポインタ
	DWORD/*MV1_MATERIAL_F1* */				DimNext ;							// 次のデータへのポインタ
	DWORD/*char* */							Name ;								// 名前
	int										Index ;								// インデックス

	COLOR_F									Diffuse ;							// ディフューズ色
	COLOR_F									Ambient ;							// アンビエント色
	COLOR_F									Specular ;							// スペキュラー色
	COLOR_F									Emissive ;							// エミッシブ色
	float									Power ;								// スペキュラハイライトのパワー
	float									Alpha ;								// アルファ値

	int										DiffuseLayerNum ;					// ディフューズカラーのレイヤー数
	MV1_MATERIAL_LAYER_F1					DiffuseLayer[ 8 ] ;					// ディフューズカラーのレイヤー情報( ０レイヤーの BlendType は無視される )
	int										SpecularLayerNum ;					// スペキュラマップのレイヤー数
	MV1_MATERIAL_LAYER_F1					SpecularLayer[ 8 ] ;				// スペキュラマップのレイヤー情報( ０レイヤーの BlendType は無視される )
	int										NormalLayerNum ;					// 法線マップのレイヤー数
	MV1_MATERIAL_LAYER_F1					NormalLayer[ 8 ] ;					// 法線マップのレイヤー情報( ０レイヤーの BlendType は無視される )

	int										UseAlphaTest ;						// アルファテストを使用するかどうか
	int										AlphaFunc ;							// アルファテストモード( 今のところ未使用 )
	int										AlphaRef ;							// アルファテストの閾値

	int										DrawBlendMode ;						// 出力時のブレンドモード( DX_BLENDMODE_NOBLEND 等 )
	int										DrawBlendParam ;					// 出力時のブレンドパラメータ

	DWORD									UserData[ 4 ] ;						// 外部定義の情報

	DWORD/*MV1_MATERIAL_TOON_F1* */			ToonInfo ;						// トゥーンレンダリング用の情報、無い場合は NULL
	DWORD									Padding[ 3 ] ;
} ;

// ライト構造体
struct MV1_LIGHT_F1
{
	DWORD/*MV1_LIGHT_F1* */					DimPrev ;							// 前のデータへのポインタ
	DWORD/*MV1_LIGHT_F1* */					DimNext ;							// 次のデータへのポインタ
	DWORD/*char* */							Name ;								// 名前
	int										Index ;								// インデックス

	int										FrameIndex ;						// ライトを持っているフレームのインデックス
	int										Type ;								// ライトタイプ( MV1_LIGHT_TYPE_POINT 等 )
	COLOR_F									Diffuse ;							// ディフューズカラー
	COLOR_F									Specular ;							// スペキュラカラー
	COLOR_F									Ambient ;							// アンビエントカラー
	float									Range ;								// スポットライトの有効距離
	float									Falloff ;							// フォールオフ
	float									Attenuation0 ;						// ライトパラメータ０
	float									Attenuation1 ;						// ライトパラメータ１
	float									Attenuation2 ;						// ライトパラメータ２
	float									Theta ;								// スポットライトの内部コーンの角度範囲
	float									Phi ;								// スポットライトの外部コーンの角度範囲

	DWORD									UserData[ 2 ] ;						// 外部定義の情報
} ;

// １６ビット座標値用補助情報
struct MV1_POSITION_16BIT_SUBINFO_F1
{
	float									Min ;								// 最小値
	float									Width ;								// 幅
} ;

// トライアングルリスト構造体
/*
	MeshVertexIndexAndIndexData の情報

		使用ボーン情報
			VertexType が
				MV1_VERTEX_TYPE_NORMAL        の場合　　なし
				MV1_VERTEX_TYPE_SKIN_4BONE    の場合　　最初の２バイトに使用ボーンの数、その後に１ボーン辺り２バイトで使用ボーンインデックス値が使用ボーン数分だけ
				MV1_VERTEX_TYPE_SKIN_8BONE    の場合　　最初の２バイトに使用ボーンの数、その後に１ボーン辺り２バイトで使用ボーンインデックス値が使用ボーン数分だけ
				MV1_VERTEX_TYPE_SKIN_FREEBONE の場合    最大使用ボーン数を２バイトで格納

		VertexNum の数だけ

			メッシュ頂点インデックス
				Flag の MV1_TRIANGLE_LIST_FLAG_MVERT_INDEX_MASK に応じた値


		IndexNum の数だけ
		
			頂点インデックス
				Flag の MV1_TRIANGLE_LIST_FLAG_INDEX_MASK に応じた値
*/
struct MV1_TRIANGLE_LIST_F1
{
	DWORD/*MV1_TRIANGLE_LIST_F1* */			DimPrev ;						// 前のデータへのポインタ
	DWORD/*MV1_TRIANGLE_LIST_F1* */			DimNext ;						// 次のデータへのポインタ
	int										Index ;								// インデックス

	DWORD/*MV1_MESH_F1* */					Container ;							// このトライアングルリストを持っているメッシュへのポインタ
	unsigned short							VertexType ;						// 頂点タイプ( MV1_VERTEX_TYPE_NORMAL など )
	unsigned short							Flag ;								// フラグ( MV1_TRIANGLE_LIST_FLAG_NORMAL_S8 等 )
	unsigned short							VertexNum ;							// 頂点データの数
	unsigned short							IndexNum ;							// 頂点インデックスの数
	DWORD/*void* */							MeshVertexIndexAndIndexData ;		// メッシュ頂点インデックスデータと頂点インデックスデータ

	DWORD									Padding[ 2 ] ;
} ;

// スキンメッシュ用ボーンを使用するフレームの情報
struct MV1_SKIN_BONE_USE_FRAME_F1
{
	int										Index ;								// ボーンを使用しているフレームのインデックス
	int										MatrixIndex ;						// このボーンがセットされているフレーム内インデックス( MV1_FRAME_BASE.UseSkinBone のインデックス )
} ;

// スキンメッシュ用ボーン情報
struct MV1_SKIN_BONE_F1
{
	DWORD/*MV1_SKIN_BONE_F1* */				DimPrev ;							// 前のデータへのポインタ
	DWORD/*MV1_SKIN_BONE_F1* */				DimNext ;							// 次のデータへのポインタ
	int										Index ;								// インデックス

	int										BoneFrame ;							// ボーンとして使用するフレーム
	MATRIX_4X4CT_F							ModelLocalMatrix ;					// モデル座標からボーンのローカル座標に変換するための行列
	int										ModelLocalMatrixIsTranslateOnly ;	// モデル座標からボーンのローカル座標に変換するための行列が平行移動のみかどうか( 1:平行移動のみ  0:回転も含む )
	int										UseFrameNum ;						// このボーンを使用するフレームの数
	DWORD/*MV1_SKIN_BONE_USE_FRAME_F1* */	UseFrame ;							// このボーンを使用するフレームの情報

	DWORD									Padding[ 2 ] ;
} ;

// 状態変更管理用基本情報構造体
struct MV1_CHANGE_F1
{
	DWORD									Target ;							// 状態変化が発生した際に論理和するべき対象の開始メモリアドレスへのオフセット( ４バイト単位 )
	DWORD/*DWORD* */						Fill ;								// 状態変化が発生した際に論理和するフラグデータ
	DWORD									CheckBit ;							// 自分のフラグビットが立ったビット情報
	DWORD									Size ;								// 状態変化が発生した際に論理和するフラグデータのサイズ( DWORD 単位 )
} ;

// メッシュ構造体
/*
	VertexData の情報格納順序

		最初に一つだけ

			VertFlag に MV1_MESH_VERT_FLAG_COMMON_COLOR が付いている場合
				COLOR_U8			DiffuseColor ;				// 共通ディフューズ色
				COLOR_U8			SpecularColor ;				// 共通スペキュラ色

		VertexNum の数だけ

			座標インデックス
				VertFlag の MV1_MESH_VERT_FLAG_POS_IND_TYPE_MASK に応じた値

		VertexNum の数だけ

			法線インデックス
				VertFlag の MV1_MESH_VERT_FLAG_NRM_IND_TYPE_MASK に応じた値

		VertexNum の数だけ

			頂点カラー
				VertFlag に MV1_MESH_VERT_FLAG_COMMON_COLOR が付いている場合は無い、ある場合は COLOR_U8 型で、ディフューズ色、スペキュラ色の順

		VertexNum の数だけ

			ＵＶ値
				U,V の順に UVUnitNum * UVSetUnitNum の数だけ
				VertFlag に MV1_MESH_VERT_FLAG_UV_U16 が付いていたら WORD型( 65535 を 1.0 とする固定小数点値 )、それ以外の場合は float型

		VertFlag に MV1_MESH_VERT_FLAG_NON_TOON_OUTLINE が付いていた場合 VertexNum の数だけ

			トゥーンレンダリング時に輪郭線を描画しないかどうかの情報が１頂点１ビットで格納、余りが８ビット未満の場合はそのバイトもパディングで使う
*/
struct MV1_MESH_F1
{
	DWORD/*MV1_MESH_F1* */					DimPrev ;							// 前のデータへのポインタ
	DWORD/*MV1_MESH_F1* */					DimNext ;							// 次のデータへのポインタ

	int										Index ;								// インデックス

	DWORD/*MV1_FRAME_F1* */					Container ;							// このメッシュを持っているフレームへのポインタ
	DWORD/*MV1_MATERIAL_F1* */				Material ;							// 使用するマテリアルへのポインタ

	MV1_CHANGE_F1							ChangeInfo ;						// 状態変化管理用基本情報

	int										UseVertexDiffuseColor ;				// 頂点のディフューズカラーをマテリアルのディフューズカラーの代わりに使用するかどうか( TRUE:使用する  FALSE:使用しない )
	int										UseVertexSpecularColor ;			// 頂点のスペキュラカラーをマテリアルのスペキュラカラーの代わりに使用するかどうか( TRUE:使用する  FALSE:使用しない )
	BYTE									NotOneDiffuseAlpha ;				// ディフューズカラーのアルファ値で 100% 以外のものがあるかどうか( 1:ある  0:ない )
	BYTE									Shape ;								// シェイプメッシュかどうか( 1:シェイプメッシュ  0:シェイプメッシュではない )
	BYTE									Padding2[ 2 ] ;

	int										TriangleListNum ;					// トライアングルリストの数
	DWORD/*MV1_TRIANGLE_LIST_F1* */			TriangleList ;						// トライアングルリストのリストへのポインタ

	BYTE									Visible ;							// 表示フラグ( 1:表示する  0:表示しない )
	BYTE									BackCulling ;						// バックカリングをするかどうか( 2:右回りカリング  1:左回りカリング  0:しない )

	BYTE									UVSetUnitNum ;						// 一つの座標データに含まれるテクスチャ座標セットの数
	BYTE									UVUnitNum ;							// 一つの座標データに含まれるテクスチャ座標の数

	int										VertFlag ;							// 頂点データに関するフラグ( MV1_MESH_VERT_FLAG_POS_IND_TYPE_MASK 等 )

	int										VertexNum ;							// 頂点の数
	int										FaceNum ;							// 面の数
	DWORD/*void* */							VertexData ;						// 頂点データ

	DWORD									UserData[ 4 ] ;						// 外部定義の情報

	DWORD									Padding[ 4 ] ;
} ;

// シェイプ頂点データ構造体
struct MV1_SHAPE_VERTEX_F1
{
	int										TargetMeshVertex ;					// 対象となる頂点番号( MV1_MESH_F1.Vertex に対するインデックス )
	VECTOR									Position ;							// 座標( 元の座標に対する差分 )
	VECTOR									Normal ;							// 法線
} ;

// シェイプメッシュ構造体
struct MV1_SHAPE_MESH_F1
{
	DWORD/*MV1_SHAPE_MESH_F1* */			DimPrev ;							// 前のデータへのポインタ
	DWORD/*MV1_SHAPE_MESH_F1* */			DimNext ;							// 次のデータへのポインタ

	int										Index ;								// インデックス

	DWORD/*MV1_MESH_F1* */					TargetMesh ;						// 対象となるメッシュ

	WORD									IsVertexPress ;						// 頂点データが圧縮されているかどうか( 1:圧縮されている  0:されていない )
	WORD									VertexPressParam ;					// 頂点データの圧縮パラメータ
	DWORD									VertexNum ;							// 頂点データの数
	DWORD/*MV1_SHAPE_VERTEX_F1* */			Vertex ;							// 頂点データ配列へのポインタ

	DWORD									UserData[ 4 ] ;						// 外部定義の情報

	DWORD									Padding[ 4 ] ;
} ;

// シェイプ構造体
struct MV1_SHAPE_F1
{
	DWORD/*MV1_SHAPE_F1* */					DimPrev ;							// 前のデータへのポインタ
	DWORD/*MV1_SHAPE_F1* */					DimNext ;							// 次のデータへのポインタ

	DWORD/*char* */							Name ;								// 名前
	int										Index ;								// インデックス

	DWORD/*MV1_FRAME_F1* */					Container ;							// このシェイプを持っているフレームのポインタ

	int										MeshNum ;							// シェイプメッシュの数
	DWORD/*MV1_SHAPE_MESH_F1* */			Mesh ;								// シェイプメッシュ配列へのポインタ

	DWORD									UserData[ 4 ] ;						// 外部定義の情報

	DWORD									Padding[ 4 ] ;
} ;

// フレームに付属するシェイプ情報
struct MV1_FRAME_SHAPE_F1
{
	int										ShapeNum ;							// フレームに付属するシェイプ情報の数
	DWORD/*MV1_SHAPE_F1* */					Shape ;								// シェイプ情報配列へのポインタ
} ;

// 階層データ構造体
/*
	PositionAndNormalData の情報格納順序

		最初に一つだけ

			VertFlag に MV1_FRAME_VERT_FLAG_POSITION_B16 が付いている場合
				MV1_POSITION_16BIT_SUBINFO_F1	x, y, z ;				// 座標値 x, y, z の16ビット化用補助情報

		PositionNum の数だけ

			座標データ
				VertFlag に MV1_FRAME_VERT_FLAG_POSITION_B16 が付いている場合 WORD型、付いていなければ float型

		PositionNum の数だけ

			スキニング情報、VertFlag に MV1_FRAME_VERT_FLAG_MATRIX_WEIGHT_NONE が付いている場合は無い
				インデックス値、ウエイト値の順にインデックス値が -1 になるか、MaxBoneBlendNum の数に達するまで、インデックス値が -1 の後ろにウエイト値は無い
				インデックス値は VertFlag の MV1_FRAME_VERT_FLAG_MATRIX_INDEX_MASK  に応じた値
				ウエイト値は     VertFlag の MV1_FRAME_VERT_FLAG_MATRIX_WEIGHT_MASK に応じた値


		法線情報、VertFlag の MV1_FRAME_VERT_FLAG_NORMAL_TYPE_MASK の値が MV1_FRAME_NORMAL_TYPE_NONE の場合は無い
		NormalNum の数だけ

			法線データ
				VertFlag の MV1_FRAME_VERT_FLAG_NORMAL_TYPE_MASK に応じた値

*/
struct MV1_FRAME_F1
{
	DWORD/*MV1_FRAME_F1* */					DimPrev ;							// データ配列上の前の階層データへのポインタ
	DWORD/*MV1_FRAME_F1* */					DimNext ;							// データ配列上の次の階層データへのポインタ

	DWORD/*char* */							Name ;								// 名前
	int										Index ;								// インデックス

	MV1_CHANGE_F1							ChangeDrawMaterialInfo ;			// 描画用マテリアル状態変化管理用基本情報
	MV1_CHANGE_F1							ChangeMatrixInfo ;					// 行列状態変化管理用基本情報

	int										TotalMeshNum ;						// 自分の下層にあるメッシュの総数
	int										TotalChildNum ;						// 自分の下層にあるフレームの数
	DWORD/*MV1_FRAME_F1* */					Parent ;							// 親階層へのポインタ
	DWORD/*MV1_FRAME_F1* */					FirstChild ;						// 子階層へのポインタ(先端)
	DWORD/*MV1_FRAME_F1* */					LastChild ;							// 子階層へのポインタ(末端)
	DWORD/*MV1_FRAME_F1* */					Prev ;								// 兄階層へのポインタ
	DWORD/*MV1_FRAME_F1* */					Next ;								// 弟階層へのポインタ

	VECTOR									Translate ;							// 平行移動
	VECTOR									Scale ;								// スケール
	VECTOR									Rotate ;							// 回転
	int										RotateOrder ;						// 回転オーダー
	FLOAT4									Quaternion ;						// 回転( クォータニオン )

	DWORD									Flag ;								// 各種フラグ( MV1_FRAMEFLAG_VISIBLE など )

	int										IsSkinMesh ;						// このフレームに含まれるメッシュの中にスキンメッシュが含まれるかどうか( TRUE:含まれる  FALSE:含まれない )
	int										TriangleNum ;						// このフレームに含まれるポリゴンの数
	int										VertexNum ;							// このフレームに含まれる頂点データの数
	int										MeshNum ;							// メッシュの数
	DWORD/*MV1_MESH_F1* */					Mesh ;								// メッシュリストへのポインタ

	int										SkinBoneNum ;						// このフレームをターゲットとするボーンの数
	DWORD/*MV1_SKIN_BONE_F1* */				SkinBone ;							// このフレームをターゲットとする最初のボーンへのポインタ

	int										UseSkinBoneNum ;					// このフレームが使用しているボーンの数
	DWORD/*MV1_SKIN_BONE_F1** */			UseSkinBone ;						// このフレームが使用しているボーンへのポインタの配列

	DWORD/*MV1_LIGHT_F1* */					Light ;								// ライト情報へのポインタ

	unsigned short							VertFlag ;							// 頂点データに関するフラグ
	unsigned short							MaxBoneBlendNum ;					// 一つの座標データで使用するボーンウエイト情報の最大数

	float									SmoothingAngle ;					// 自動法線計算の場合のスムージングを行うかどうかの閾値( 単位はラジアン )
	int										AutoCreateNormal ;					// 法線の自動生成を使用するかどうか( TRUE:使用する  FALSE:使用しない )

	int										PositionNum ;						// 座標の数
	int										NormalNum ;							// 法線の数
	DWORD/*void* */							PositionAndNormalData ;				// 座標と法線データ

	DWORD									UserData[ 4 ] ;						// 外部定義の情報

	DWORD/*MV1_FRAME_SHAPE_F1* */			FrameShape ;						// シェイプ情報( 無い場合は NULL )

	VECTOR									PreRotate ;							// 前回転
	VECTOR									PostRotate ;						// 後回転

	DWORD									Padding[ 8 ] ;
} ;

// 物理演算用剛体データ構造体
struct MV1_PHYSICS_RIGIDBODY_F1
{
	DWORD/*MV1_PHYSICS_RIGIDBODY_F1* */		DimPrev ;							// 前のデータへのポインタ
	DWORD/*MV1_PHYSICS_RIGIDBODY_F1* */		DimNext ;							// 次のデータへのポインタ

	DWORD/*char* */							Name ;								// 名前
	int										Index ;								// インデックス

	DWORD/*MV1_FRAME_F1* */					TargetFrame ;						// 対象となるフレーム

	int										RigidBodyGroupIndex ;				// 剛体グループ番号
	DWORD									RigidBodyGroupTarget ;				// 剛体グループ対象
	int										ShapeType ;							// 形状( 0:球  1:箱  2:カプセル )
	float									ShapeW ;							// 幅
	float									ShapeH ;							// 高さ
	float									ShapeD ;							// 奥行
	VECTOR									Position ;							// 位置
	VECTOR									Rotation ;							// 回転( ラジアン )
	float									RigidBodyWeight ;					// 質量
	float									RigidBodyPosDim ;					// 移動減
	float									RigidBodyRotDim ;					// 回転減
	float									RigidBodyRecoil ;					// 反発力
	float									RigidBodyFriction ;					// 摩擦力
	int										RigidBodyType ;						// 剛体タイプ( 0:Bone追従  1:物理演算  2:物理演算(Bone位置合わせ) )
	int										NoCopyToBone ;						// ボーンの行列を物理に適用しないかどうか

	DWORD									UserData[ 4 ] ;						// 外部定義の情報

	DWORD									Padding[ 4 ] ;
} ;

// 物理演算用剛体ジョイントデータ構造体
struct MV1_PHYSICS_JOINT_F1
{
	DWORD/*MV1_PHYSICS_JOINT_F1* */			DimPrev ;							// 前のデータへのポインタ
	DWORD/*MV1_PHYSICS_JOINT_F1* */			DimNext ;							// 次のデータへのポインタ

	DWORD/*char* */							Name ;								// 名前
	int										Index ;								// インデックス

	DWORD/*MV1_PHYSICS_RIGIDBODY_F1* */		RigidBodyA ;						// 接続先剛体Ａ
	DWORD/*MV1_PHYSICS_RIGIDBODY_F1* */		RigidBodyB ;						// 接続先剛体Ｂ
	VECTOR									Position ;							// 位置
	VECTOR									Rotation ;							// 回転( ラジアン )
	VECTOR									ConstrainPosition1 ;				// 移動制限値１
	VECTOR									ConstrainPosition2 ;				// 移動制限値２
	VECTOR									ConstrainRotation1 ;				// 回転制限値１
	VECTOR									ConstrainRotation2 ;				// 回転制限値２
	VECTOR									SpringPosition ;					// ばね移動値
	VECTOR									SpringRotation ;					// ばね回転値

	DWORD									UserData[ 4 ] ;						// 外部定義の情報

	DWORD									Padding[ 4 ] ;
} ;

// ファイルに付属する物理演算関連の情報
struct MV1_FILEHEAD_PHYSICS_F1
{
	float									WorldGravity ;						// 重力パラメータ

	int										RigidBodyNum ;						// 剛体データの数
	DWORD/*MV1_PHYSICS_RIGIDBODY_F1* */		RigidBody ;							// 剛体データ配列へのポインタ

	int										JointNum ;							// ジョイントデータ構造体
	DWORD/*MV1_PHYSICS_JOINT_F1* */			Joint ;								// ジョイントデータ配列へのポインタ

	DWORD									UserData[ 4 ] ;						// 外部定義の情報

	DWORD									Padding[ 4 ] ;
} ;

// ファイルに付属するシェイプ関連の情報
struct MV1_FILEHEAD_SHAPE_F1
{
	int										FrameNum ;							// フレームに付属するシェイプデータの数
	DWORD/*MV1_FRAME_SHAPE_F1* */			Frame ;								// フレームに付属するシェイプデータ配列へのポインタ

	int										DataNum ;							// シェイプデータの数
	DWORD/*MV1_SHAPE_F1* */					Data ;								// シェイプデータ配列へのポインタ
	int										MeshNum ;							// シェイプメッシュデータの数
	DWORD/*MV1_SHAPE_MESH_F1* */			Mesh ;								// シェイプメッシュデータ配列へのポインタ
	int										VertexNum ;							// シェイプ頂点データの数
	DWORD/*MV1_SHAPE_VERTEX_F1* */			Vertex ;							// シェイプ頂点データ配列へのポインタ
	int										PressVertexDataSize ;				// 圧縮されたシェイプ頂点データのサイズ
	DWORD/*void* */							PressVertexData ;					// 圧縮されたシェイプ頂点データへのポインタ
	int										TargetMeshVertexNum ;				// シェイプ対象のメッシュの頂点の総数
	int										ShapeVertexUnitSize ;				// シェイプ用頂点一つ辺りのバイト数

	int										NormalPositionNum ;					// シェイプ用トライアングルリストのボーン情報無し座標データの数
	int										SkinPosition4BNum ;					// シェイプ用トライアングルリストの４ボーンスキニングメッシュ座標データの数
	int										SkinPosition8BNum ;					// シェイプ用トライアングルリストの８ボーンスキニングメッシュ座標データの数
	int										SkinPositionFREEBSize ;				// シェイプ用トライアングルリストの９ボーン以上スキニングメッシュ座標データのサイズ

	DWORD									UserData[ 4 ] ;						// 外部定義の情報

	DWORD									Padding[ 4 ] ;
} ;

// ファイルヘッダ
struct MV1MODEL_FILEHEADER_F1
{
	BYTE									CheckID[ 4 ] ;						// ファイル形式チェック用ＩＤ( "MV11" )
	DWORD									Version ;							// バージョン

	int										RightHandType ;						// 右手座標系かどうか( TRUE:右手座標系  FALSE:左手座標系 )
	int										AutoCreateNormal ;					// 法線の自動生成を使用するかどうか( TRUE:使用する  FALSE:使用しない )

	int										ChangeDrawMaterialTableSize ;		// 描画マテリアルの変更情報管理用に必要なデータの総容量
	int										ChangeMatrixTableSize ;				// 行列の変更情報管理用に必要なデータの総容量
	DWORD/*DWORD* */						ChangeDrawMaterialTable ;			// 描画用マテリアル変更確認用ビットデータ
	DWORD/*DWORD* */						ChangeMatrixTable ;					// 行列変更確認用ビットデータ

	int										FrameNum ;							// 階層データの数
	DWORD/*MV1_FRAME_F1* */					Frame ;								// 階層データリストへのポインタ
	int										TopFrameNum ;						// 最上位階層のフレームの数
	DWORD/*MV1_FRAME_F1* */					FirstTopFrame ;						// 最上位階層の最初のフレームへのポインタ
	DWORD/*MV1_FRAME_F1* */					LastTopFrame ;						// 最上位階層の最後のフレームへのポインタ
	int										FrameUseSkinBoneNum ;				// フレームが使用しているボーンの総数
	DWORD/*MV1_SKIN_BONE_F1** */			FrameUseSkinBone ;					// フレームが使用しているボーンへのポインタ

	int										MaterialNum ;						// マテリアルの数
	DWORD/*MV1_MATERIAL_F1* */				Material ;							// マテリアルデータリストへのポインタ

	int										TextureNum ;						// テクスチャの数
	DWORD/*MV1_TEXTURE_F1* */				Texture ;							// テクスチャデータリストへのポインタ

	int										MeshNum ;							// メッシュの数
	DWORD/*MV1_MESH_F1* */					Mesh ;								// メッシュデータリストへのポインタ

	int										LightNum ;							// ライトの数
	DWORD/*MV1_LIGHT_F1* */					Light ;								// ライトデータ配列へのポインタ

	int										SkinBoneNum ;						// スキニングメッシュのボーンとして使用するフレームの情報の数
	DWORD/*MV1_SKIN_BONE_F1* */				SkinBone ;							// スキニングメッシュのボーンとして使用するフレームの情報
	int										SkinBoneUseFrameNum ;				// スキニングメッシュのボーンを使用するフレームの数
	DWORD/*MV1_SKIN_BONE_USE_FRAME_F1** */	SkinBoneUseFrame ;					// スキニングメッシュのボーンを使用するフレーム

	int										TriangleListNum ;					// トライアングルリストの数
	DWORD/*MV1_TRIANGLE_LIST_F1* */			TriangleList ;						// トライアングルリストのリストへのポインタ

	DWORD/*void* */							VertexData ;						// 頂点データ
	DWORD									VertexDataSize ;					// 頂点データのサイズ

	int										TriangleListNormalPositionNum ;		// セーブ前のボーン情報無し座標データの数
	int										TriangleListSkinPosition4BNum ;		// セーブ前の４ボーンスキニングメッシュ座標データの数
	int										TriangleListSkinPosition8BNum ;		// セーブ前の８ボーンスキニングメッシュ座標データの数
	int										TriangleListSkinPositionFREEBSize ;	// セーブ前の９ボーン以上スキニングメッシュ座標データのサイズ
	int										MeshPositionSize ;					// メッシュ座標のデータサイズ
	int										MeshNormalNum ;						// メッシュ法線の数
	int										MeshVertexSize ;					// メッシュ頂点情報のデータサイズ
	int										MeshFaceNum ;						// メッシュ面情報の数
	int										MeshVertexIndexNum ;				// 頂点インデックスデータの総数
	int										TriangleListIndexNum ;				// トライアングルリストのインデックスデータの総数

	int										TriangleNum ;						// 三角形の数
	int										TriangleListVertexNum ;				// トライアングルリストの頂点データの数

	int										StringSize ;						// 文字列データのサイズ
	DWORD/*char* */							StringBuffer ;						// 文字列データへのポインタ

	int										OriginalAnimKeyDataSize ;			// 保存前のアニメーションキーデータのサイズ
	int										AnimKeyDataSize ;					// アニメーションキーデータサイズ
	DWORD/*void* */							AnimKeyData ;						// アニメーションキーデータへのポインタ

	int										AnimKeySetNum ;						// アニメーションキーセットの数
	int										AnimKeySetUnitSize ;				// アニメーションキーセット構造体一つ辺りのサイズ
	DWORD/*MV1_ANIM_KEYSET_F1* */			AnimKeySet ;						// アニメーションキーセットリストへのポインタ

	int										AnimNum ;							// アニメーションの数
	int										AnimUnitSize ;						// アニメーション構造体一つ辺りのサイズ
	DWORD/*MV1_ANIM_F1* */					Anim ;								// アニメーションデータリストへのポインタ

	int										AnimSetNum ;						// アニメーションセットの数
	DWORD/*MV1_ANIMSET_F1* */				AnimSet ;							// アニメーションセットリストへのポインタ

	DWORD									UserData[ 4 ] ;						// 外部定義の情報

	DWORD/*MV1_FILEHEAD_SHAPE_F1* */		Shape ;								// シェイプに関する情報、存在しない場合は NULL
	DWORD/*MV1_FILEHEAD_PHYSICS_F1* */		Physics ;							// 物理演算用の情報、存在しない場合は NULL

	BYTE									MaterialNumberOrderDraw ;			// 割り当てられているマテリアルの番号が低いメッシュから描画するかどうか( 1:する  0:しない )
	BYTE									IsStringUTF8 ;						// StringBuffer に格納されている文字列が UTF-8 かどうか( 1:UTF-8  0:Shift-JIS )

	BYTE									Padding1[ 2 ] ;
	DWORD									Padding2[ 13 ] ;
} ;


// データ宣言 --------------------------------

// 関数プロトタイプ宣言 ----------------------

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif	// DX_NON_MODEL

#endif

