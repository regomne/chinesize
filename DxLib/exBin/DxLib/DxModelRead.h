// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		モデルデータ読み込みヘッダ
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXMODELREAD_H__
#define __DXMODELREAD_H__

#include "DxCompileConfig.h"

#ifndef DX_NON_MODEL

// インクルード ---------------------------------
#include "DxLib.h"
#include "DxStatic.h"
#include "DxModel.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 -----------------------------------

#define MV1_READ_MAX_UV_NUM						(8)				// 一つの頂点で使用できる最大ＵＶ数
#define MV1_READ_MAX_MESH_MATERIAL_NUM			(1024)			// 一つのメッシュで使用できるマテリアルの最大数
#define MV1_READ_MAX_MESH_SKINWEIGHT_NUM		(4096)			// 一つのメッシュで使用できるスキンウエイトの最大数

// データ型定義 ---------------------------------

// ライト構造体
struct MV1_LIGHT_R
{
	MV1_LIGHT_R				*DataPrev ;						// (読み取り専用)データ的に前のライト
	MV1_LIGHT_R				*DataNext ;						// (読み取り専用)データ的に次のライト
	int						Index ;							// (読み取り専用)インデックス
#ifndef UNICODE
	char					*NameA ;						// (読み取り専用)名前
#endif
	wchar_t					*NameW ;						// (読み取り専用)名前

	int						FrameIndex ;					// ライトを持っているフレームのインデックス
	int						Type ;							// ライトタイプ( MV1_LIGHT_TYPE_POINT 等 )
	COLOR_F					Diffuse ;						// ディフューズカラー
	COLOR_F					Specular ;						// スペキュラカラー
	COLOR_F					Ambient ;						// アンビエントカラー
	float					Range ;							// スポットライトの有効距離
	float					Falloff ;						// フォールオフ
	float					Attenuation0 ;					// ライトパラメータ０
	float					Attenuation1 ;					// ライトパラメータ１
	float					Attenuation2 ;					// ライトパラメータ２
	float					Theta ;							// スポットライトの内部コーンの角度範囲
	float					Phi ;							// スポットライトの外部コーンの角度範囲
} ;

// アニメーションキーセット情報構造体
struct MV1_ANIMKEYSET_R
{
	MV1_ANIMKEYSET_R					*DataPrev ;				// (読み取り専用)データ的に一つ前のアニメーションキーセット
	MV1_ANIMKEYSET_R					*DataNext ;				// (読み取り専用)データ的に一つ先のアニメーションキーセット
	int									Index ;					// (読み取り専用)インデックス

	char								Type ;					// キータイプ( MV1_ANIMKEY_TYPE_QUATERNION 等 )
	char								DataType ;				// データタイプ( MV1_ANIMKEY_DATATYPE_ROTATE 等 )
	char								TimeType ;				// 時間タイプ( MV1_ANIMKEY_TIME_TYPE_KEY 等 )
	int									TargetShapeIndex ;		// 対象のシェイプインデックス( DataType が MV1_ANIMKEY_DATATYPE_SHAPE の場合のみ有効 )

	float								TotalTime ;				// 総時間
	int									Num ;					// キーの数

	float								UnitTime ;				// 単位時間( TimeType が MV1_ANIMKEY_TIME_TYPE_ONE の場合のみ有効 )
	union
	{
		float							*KeyTime ;				// キー時間配列へのポインタ( TimeType が MV1_ANIMKEY_TIME_TYPE_KEY の場合のみ有効 )
		float							StartTime ;				// 開始時間( TimeType が MV1_ANIMKEY_TIME_TYPE_ONE の場合のみ有効 )
	} ;

	union
	{
		FLOAT4							*KeyFloat4 ;			// float4個キー配列へのポインタ
		VECTOR							*KeyVector ;			// ベクターキー配列へのポインタ
		MV1_ANIM_KEY_MATRIX4X4C			*KeyMatrix4x4C ;		// ４列目省略４×４行列キー配列へのポインタ
		MV1_ANIM_KEY_MATRIX3X3			*KeyMatrix3x3 ;			// ３ｘ３行列キー配列へのポインタ
		float							*KeyFlat ;				// フラットキー配列へのポインタ
		float							*KeyLinear ;			// 線形補間キー配列へのポインタ
	} ;

	MV1_ANIMKEYSET_R					*Prev ;					// (読み取り専用)前のアニメーションキーセットへのポインタ
	MV1_ANIMKEYSET_R					*Next ;					// (読み取り専用)次のアニメーションキーセットへのポインタ
} ;

// アニメーション情報構造体
struct MV1_ANIM_R
{
	struct MV1_ANIMSET_R	*Container ;					// (読み取り専用)このデータを含んでいるアニメーションセットへのポインタ

	MV1_ANIM_R				*DataPrev ;						// (読み取り専用)データ的に一つ前のアニメーション
	MV1_ANIM_R				*DataNext ;						// (読み取り専用)データ的に一つ先のアニメーション
	int						Index ;							// (読み取り専用)インデックス

	int						TargetFrameIndex ;				// 対象となるノードのインデックス
	float					MaxTime ;						// 各キーセットの中で一番長いキーセットの時間値
	int						RotateOrder ;					// 回転オーダー( MV1_ROTATE_ORDER_XYZ 等 )

	int						AnimKeySetNum ;					// (読み取り専用)キーセットの数
	MV1_ANIMKEYSET_R		*AnimKeySetFirst ;				// (読み取り専用)先頭のキーセット
	MV1_ANIMKEYSET_R		*AnimKeySetLast ;				// (読み取り専用)末端のキーセット

	MV1_ANIM_R				*Prev ;							// (読み取り専用)前のアニメーション
	MV1_ANIM_R				*Next ;							// (読み取り専用)次のアニメーション
} ;

// アニメーションセット情報構造体
struct MV1_ANIMSET_R
{
	MV1_ANIMSET_R			*DataPrev ;						// (読み取り専用)データ的に一つ前のアニメーションセット
	MV1_ANIMSET_R			*DataNext ;						// (読み取り専用)データ的に一つ先のアニメーションセット
	int						Index ;							// (読み取り専用)インデックス
#ifndef UNICODE
	const char				*NameA ;						// (読み取り専用)名前
#endif
	const wchar_t			*NameW ;						// (読み取り専用)名前

	float					StartTime ;						// 開始時間
	float					EndTime ;						// 終了時間
	int						IsAddAnim ;						// 加算アニメーションかどうか( 1:加算アニメーション  0:絶対値アニメーション )
	int						IsMatrixLinearBlend ;			// 各キーの補間を行列単位で線形補間を刷るかどうかのフラグ( 1:行列で線形補間  0:要素単位で補間 )
	int						IsLoopAnim ;					// ループ用アニメーションかどうか( 1:ループアニメーション  0:通常アニメーション )

	int						AnimNum ;						// (読み取り専用)アニメーションデータの数
	MV1_ANIM_R				*AnimFirst ;					// (読み取り専用)先頭のアニメーション
	MV1_ANIM_R				*AnimLast ;						// (読み取り専用)末端のアニメーション
} ;

// テクスチャ情報構造体
struct MV1_TEXTURE_R
{
	MV1_TEXTURE_R			*DataPrev ;						// (読み取り専用)データ的に前のテクスチャ
	MV1_TEXTURE_R			*DataNext ;						// (読み取り専用)データ的に次のテクスチャ
	int						Index ;							// (読み取り専用)インデックス
#ifndef UNICODE
	const char				*NameA ;						// (読み取り専用)オブジェクトネーム
#endif
	const wchar_t			*NameW ;						// (読み取り専用)オブジェクトネーム

	void					*UserData ;						// ユーザーデータ

	int						AddressModeU ;					// アドレスモード( MV1_TEXTURE_ADDRESS_MODE_WRAP 等 )
	int						AddressModeV ;					// アドレスモード( MV1_TEXTURE_ADDRESS_MODE_WRAP 等 )
	float					ScaleU ;						// Ｕ座標のスケーリング値
	float					ScaleV ;						// Ｖ座標のスケーリング値
	int						FilterMode ;					// フィルタリングモード( MV1_TEXTURE_FILTER_MODE_POINT 等 )

#ifndef UNICODE
	char					*ColorFileNameA ;				// (読み取り専用)ディフューズテクスチャファイル名
#endif
	wchar_t					*ColorFileNameW ;				// (読み取り専用)ディフューズテクスチャファイル名
#ifndef UNICODE
	char					*AlphaFileNameA ;				// (読み取り専用)アルファチャンネル用テクスチャファイル名
#endif
	wchar_t					*AlphaFileNameW ;				// (読み取り専用)アルファチャンネル用テクスチャファイル名
	int						BumpMapFlag ;					// (読み取り専用)カラーチャンネルがバンプマップかどうか( TRUE:バンプマップ  FALSE:違う )
	float					BumpMapNextPixelLength ;		// (読み取り専用)カラーチャンネルがバンプマップだった場合の、隣のピクセルとの距離
	int						BlendType ;						// ブレンドタイプ( MV1_LAYERBLEND_TYPE_TRANSLUCENT 等 )

	int						ReverseFlag ;					// 画像を反転するかどうか( 1:反転する  0:反転しない )
	int						Bmp32AllZeroAlphaToXRGB8Flag ;	// ３２ビットＢＭＰのアルファ値が全部０だったらXRGB8とするかどうか( 1:そうする  0:そうしない )
} ;

// マテリアル情報構造体
struct MV1_MATERIAL_R
{
	MV1_MATERIAL_R			*DataPrev ;						// (読み取り専用)データ的に前のマテリアル
	MV1_MATERIAL_R			*DataNext ;						// (読み取り専用)データ的に次のマテリアル
	int						Index ;							// (読み取り専用)インデックス
#ifndef UNICODE
	const char				*NameA ;						// (読み取り専用)名前
#endif
	const wchar_t			*NameW ;						// (読み取り専用)名前
	const char				*SubName ;						// 読み込み処理時の名前検索処理等に使用する名前

	void					*UserData ;						// ユーザーデータ

	int						Type ;							// マテリアルタイプ( DX_MATERIAL_TYPE_NORMAL など )

	int						DisableBackCulling ;			// バックカリングを行わないかどうか( TRUE:バックかリングを行わない  FALSE:バックかリングを行う )
	int						TwoSide ;						// 両面ポリゴンにするか( バックカリングOFFではなくポリゴンを二つにする )( TRUE:両面ポリゴン  FALSE:片面ポリゴン )

	int						DiffuseTexNum ;					// ディフューズテクスチャの数
	MV1_TEXTURE_R			*DiffuseTexs[ 256 ] ;			// ディフューズテクスチャへのポインタの配列
	int						SpecularTexNum ;				// スペキュラテクスチャの数
	MV1_TEXTURE_R			*SpecularTexs[ 256 ] ;			// スペキュラテクスチャへのポインタの配列
	int						NormalTexNum ;					// 法線マップテクスチャの数
	MV1_TEXTURE_R			*NormalTexs[ 256 ] ;			// 法線マップテクスチャへのポインタの配列

	COLOR_F					Ambient ;						// アンビエント
	COLOR_F					Diffuse ;						// ディフューズ
	COLOR_F					Specular ;						// スペキュラ
	COLOR_F					Emissive ;						// エミッシブ
//	float					Shininess ;						// 光沢
//	float					Reflection ;					// 反射
	float					Power ;							// 反射の強さ
//	float					Alpha ;							// アルファ値

	int						DiffuseGradTextureDefault ;		// ( トゥーンレンダリングでのみ使用 )ディフューズカラーグラデーションテクスチャが見つからなかったときに使用するデフォルトグラデーションテクスチャタイプ
	int						SpecularGradTextureDefault ;	// ( トゥーンレンダリングでのみ使用 )スペキュラカラーグラデーションテクスチャが見つからなかったときに使用するデフォルトグラデーションテクスチャタイプ
	MV1_TEXTURE_R			*DiffuseGradTexture ;			// ( トゥーンレンダリングでのみ使用 )ディフューズカラーグラデーションテクスチャ、当たっているライトのディフューズカラーでＵ値が決まるもの
	MV1_TEXTURE_R			*SpecularGradTexture ;			// ( トゥーンレンダリングでのみ使用 )スペキュラカラーグラデーションテクスチャ、当たっているライトのスペキュラカラーでＵ値が決まるもの
	MV1_TEXTURE_R			*SphereMapTexture ;				// ( トゥーンレンダリングでのみ使用 )スフィアマップテクスチャインデックス
	int						DiffuseGradBlendType ;			// ( トゥーンレンダリングでのみ使用 )ディフューズグラデーションテクスチャのブレンドタイプ( MV1_LAYERBLEND_TYPE_TRANSLUCENT など )
	int						SpecularGradBlendType ;			// ( トゥーンレンダリングでのみ使用 )スペキュラグラデーションテクスチャのブレンドタイプ( MV1_LAYERBLEND_TYPE_ADDITIVE など )
	int						SphereMapBlendType ;			// ( トゥーンレンダリングでのみ使用 )スフィアマップテクスチャのブレンドタイプ( DX_MATERIAL_BLENDTYPE_ADDITIVE など )
	float					OutLineWidth ;					// ( トゥーンレンダリングでのみ使用 )輪郭線の幅
	float					OutLineDotWidth ;				// ( トゥーンレンダリングでのみ使用 )輪郭線のドット単位での幅
	COLOR_F					OutLineColor ;					// ( トゥーンレンダリングでのみ使用 )輪郭線の色

	int						DrawBlendMode ;					// 出力時のブレンドモード( DX_BLENDMODE_NOBLEND 等 )
	int						DrawBlendParam ;				// 出力時のブレンドパラメータ
} ;

// 面情報構造体
struct MV1_MESHFACE_R
{
	DWORD					IndexNum ;							// 頂点インデックスの数
	DWORD					PolygonNum ;						// (読み取り専用)３角形ポリゴンの数
	DWORD					*VertexIndex ;						// 頂点インデックス
	DWORD					*NormalIndex ;						// 法線インデックス
	DWORD					*VertexColorIndex ;					// 頂点カラーインデックス
	DWORD					*UVIndex[ MV1_READ_MAX_UV_NUM ] ;	// ＵＶインデックス
	DWORD					MaterialIndex ;						// マテリアルインデックス
	VECTOR					Normal ;							// 面の法線( 法線自動計算時に使用 )
} ;

// トライアングルリスト用の情報(コンバートプログラムが使用)
struct MV1_TRIANGLELIST_R
{
	int						UseBoneNum ;						// 使用しているボーンの数
	unsigned short			UseBone[ MV1_TRIANGLE_LIST_USE_BONE_MAX_NUM ] ;	// ボーンのリスト( Mesh.SkinWeights 配列のインデックス )
	int						PolyNum ;							// ポリゴンの数
} ;

// マテリアル毎のポリゴン情報(コンバートプログラムが使用)
struct MV1_MATERIAL_POLY_R
{
	DWORD					TypeNum[ MV1_VERTEX_TYPE_NUM ] ;	// タイプ別のトライアングルの数( 三角形ポリゴン換算 )
	int						MaxBoneCount ;						// 一つのポリゴンに関わっている最大ボーン数
	int						SimpleTriangleListNum ;				// 剛体メッシュのトライアングルリストの数
	MV1_TRIANGLELIST_R		SimpleTriangleList[ 128 ] ;			// 剛体メッシュのトライアングルリストの情報
	int						SkinB4TriangleListNum ;				// ボーン４つ以内のスキニングメッシュのトライアングルリストの数
	MV1_TRIANGLELIST_R		SkinB4TriangleList[ 128 ] ;			// ボーン４つ以内のスキニングメッシュのトライアングルリストの情報
	int						SkinB8TriangleListNum ;				// ボーン４つ以内のスキニングメッシュのトライアングルリストの数
	MV1_TRIANGLELIST_R		SkinB8TriangleList[ 128 ] ;			// ボーン４つ以内のスキニングメッシュのトライアングルリストの情報
} ;

// スキン情報の１頂点あたりの情報
struct MV1_SKIN_WEIGHT_ONE_R
{
	DWORD					TargetVertex ;						// 対象となる頂点番号
	float					Weight ;							// ウエイト
} ;

// スキン情報
struct MV1_SKIN_WEIGHT_R
{
	void					*UserData ;							// ユーザーデータ

	MATRIX					ModelLocalMatrix ;					// ボーンローカルの座標にするための行列
	DWORD					TargetFrameUserID ;					// 対象フレームのユーザーＩＤ
	DWORD					TargetFrame ;						// 対象フレームのインデックス
	DWORD					DataNum ;							// データの数
	MV1_SKIN_WEIGHT_ONE_R	*Data ;								// データ配列
} ;

// データメッシュ情報構造体
struct MV1_MESH_R
{
	MV1_MESH_R				*DataPrev ;											// (読み取り専用)データ的に前のメッシュ
	MV1_MESH_R				*DataNext ;											// (読み取り専用)データ的に次のメッシュ
	int						Index ;												// (読み取り専用)インデックス

	void					*UserData ;											// ユーザーデータ

	DWORD					FaceNum ;											// (読み取り専用)面の数
	MV1_MESHFACE_R			*Faces ;											// (読み取り専用)面情報配列
	DWORD					*FaceIndexBuffer ;									// (読み取り専用)面のインデックスバッファ
	DWORD					FaceUnitMaxIndexNum ;								// (読み取り専用)一つあたりの面の最大インデックス値

	DWORD					PositionNum ;										// 頂点位置情報の数
	VECTOR					*Positions ;										// 頂点位置情報配列
	float					*PositionToonOutLineScale ;							// トゥーンレンダリング時の輪郭線のスケール

	DWORD					NormalNum ;											// 法線情報の数
	VECTOR					*Normals ;											// 法線情報配列
	BYTE					*NormalSetFlag ;									// 法線情報セットフラグ( 法線自動算出処理用 )
	VECTOR					*Binormals ;										// 従法線情報配列
	VECTOR					*Tangents ;											// 接線情報配列

	DWORD					VertexColorNum ;									// 頂点カラーの数
	COLOR_F					*VertexColors ;										// 頂点カラー配列

	DWORD					UVNum[ MV1_READ_MAX_UV_NUM ] ;						// ＵＶ情報の数
	FLOAT4					*UVs[ MV1_READ_MAX_UV_NUM ] ;						// ＵＶ情報配列( x=u y=v )

	DWORD					MaterialNum ;										// マテリアルの数
	MV1_MATERIAL_R			*Materials[ MV1_READ_MAX_MESH_MATERIAL_NUM ] ;		// マテリアルの情報へのポインタの配列

	DWORD					SkinWeightsNum ;									// スキンメッシュのボーンの数
	MV1_SKIN_WEIGHT_R		*SkinWeights[ MV1_READ_MAX_MESH_SKINWEIGHT_NUM ] ;	// スキンメッシュ情報へのポインタ配列

	// コンバートプログラムが使用
	DWORD					MaxBoneUseNum ;										// (読み取り専用)一つの頂点データがブレンドしているボーンの最大数
	DWORD					TriangleNum ;										// (読み取り専用)三角形ポリゴンの数
	DWORD					TriangleListNum ;									// (読み取り専用)三角形ポリゴンリストの数
	DWORD					VertexDataTotalSize ;								// (読み取り専用)頂点データの総サイズ
	DWORD					IndexDataTotalSize ;								// (読み取り専用)頂点インデックスデータの総サイズ

	DWORD					TypeNumVertexDataSize[ MV1_VERTEX_TYPE_NUM ] ;		// (読み取り専用)各タイプの頂点データの総サイズ
	DWORD					TypeNumIndexDataSize[ MV1_VERTEX_TYPE_NUM ] ;		// (読み取り専用)各タイプの頂点インデックスデータの総サイズ

	BITLIST					SkinVerticesBlend ;									// (読み取り専用)各頂点がどのボーンの影響を受けるのか情報、nVertices の数だけ存在
	BITLIST					SkinFacesBlend ;									// (読み取り専用)各面がどのボーンの影響を受けるのか情報、 nFaces の数だけ存在 
	BYTE					*SkinFaceType ;										// (読み取り専用)トライアングルタイプ配列、nFaces の数だけ存在
	float					*SkinVerticeWeightInfo ;							// (読み取り専用)各頂点のボーンウエイト値
	WORD					*SkinFaceBoneNum ;									// (読み取り専用)各メッシュに関わっているボーンの数
	WORD					*FaceUseTriangleList ;								// (読み取り専用)各フェイスが使用するトライアングルリスト番号
	MV1_MATERIAL_POLY_R		*MaterialPolyList ;									// (読み取り専用)マテリアル毎のポリゴン情報

//	VECTOR					*Binormals ;										// (読み取り専用)従法線配列
//	VECTOR					*Tangents ;											// (読み取り専用)接線配列

	DWORD					IsShapeMesh ;										// (読み取り専用)シェイプメッシュかどうか

	struct MV1_FRAME_R		*Container ;										// (読み取り専用)このメッシュを持っているフレーム
	MV1_MESH_R				*Next ;												// (読み取り専用)次のメッシュ
	MV1_MESH_R				*Prev ;												// (読み取り専用)前のメッシュ
} ;

// シェイプ頂点データ構造体
struct MV1_SHAPE_VERTEX_R
{
	int						TargetPositionIndex ;			// 対象の座標データインデックス( MV1_MESH_R.Positions 配列の要素番号 )
	VECTOR					Position ;						// 座標( 元の座標に対する差分 )
	VECTOR					Normal ;						// 法線( 元の法線に対する差分 )
} ;

// シェイプ情報構造体
struct MV1_SHAPE_R
{
	MV1_SHAPE_R				*DataPrev ;						// (読み取り専用)データ的に前のフレーム
	MV1_SHAPE_R				*DataNext ;						// (読み取り専用)データ的に次のフレーム
	int						Index ;							// (読み取り専用)インデックス
#ifndef UNICODE
	const char				*NameA ;						// (読み取り専用)名前
#endif
	const wchar_t			*NameW ;						// (読み取り専用)名前

	MV1_MESH_R				*TargetMesh ;					// 対象メッシュ
	int						ValidVertexNormal ;				// 頂点データの法線が有効かどうか( TRUE:有効  FALSE:無効 )
	int						VertexNum ;						// 頂点の数
	MV1_SHAPE_VERTEX_R		*Vertex ;						// シェイプ頂点で配列へのポインタ

	DWORD					*NextTable ;					// (読み取り専用)シェイプ対象の座標要素に対応する Vertex 配列の要素番号が格納されたテーブル
	struct MV1_FRAME_R		*Container ;					// (読み取り専用)このシェイプを持っているフレーム
	MV1_SHAPE_R				*Next ;							// (読み取り専用)次のメッシュ
	MV1_SHAPE_R				*Prev ;							// (読み取り専用)前のメッシュ
} ;

// 物理演算用剛体データ構造体
struct MV1_PHYSICS_RIGIDBODY_R
{
	MV1_PHYSICS_RIGIDBODY_R	*DataPrev ;						// (読み取り専用)データ的に前の剛体データ
	MV1_PHYSICS_RIGIDBODY_R	*DataNext ;						// (読み取り専用)データ的に次の剛体データ
	int						Index ;							// (読み取り専用)インデックス
#ifndef UNICODE
	const char				*NameA ;						// (読み取り専用)名前
#endif
	const wchar_t			*NameW ;						// (読み取り専用)名前

	struct MV1_FRAME_R		*TargetFrame ;					// (読み取り専用)対象となるフレーム

	int						RigidBodyGroupIndex ;			// 剛体グループ番号
	DWORD					RigidBodyGroupTarget ;			// 剛体グループ対象
	int						ShapeType ;						// 形状( 0:球  1:箱  2:カプセル )
	float					ShapeW ;						// 幅
	float					ShapeH ;						// 高さ
	float					ShapeD ;						// 奥行
	VECTOR					Position ;						// 位置
	VECTOR					Rotation ;						// 回転( ラジアン )
	float					RigidBodyWeight ;				// 質量
	float					RigidBodyPosDim ;				// 移動減
	float					RigidBodyRotDim ;				// 回転減
	float					RigidBodyRecoil ;				// 反発力
	float					RigidBodyFriction ;				// 摩擦力
	int						RigidBodyType ;					// 剛体タイプ( 0:Bone追従  1:物理演算  2:物理演算(Bone位置合わせ) )
	int						NoCopyToBone ;					// ボーンの行列を物理に適用しないかどうか
} ;

// 物理演算用剛体ジョイントデータ構造体
struct MV1_PHYSICS_JOINT_R
{
	MV1_PHYSICS_JOINT_R		*DataPrev ;						// (読み取り専用)データ的に前の剛体ジョイントデータ
	MV1_PHYSICS_JOINT_R		*DataNext ;						// (読み取り専用)データ的に次の剛体ジョイントデータ
	int						Index ;							// (読み取り専用)インデックス
#ifndef UNICODE
	const char				*NameA ;						// (読み取り専用)名前
#endif
	const wchar_t			*NameW ;						// (読み取り専用)名前

	DWORD					RigidBodyA ;					// 接続先剛体Ａのデータインデックス
	DWORD					RigidBodyB ;					// 接続先剛体Ｂのデータインデックス
	VECTOR					Position ;						// 位置
	VECTOR					Rotation ;						// 回転( ラジアン )
	VECTOR					ConstrainPosition1 ;			// 移動制限値１
	VECTOR					ConstrainPosition2 ;			// 移動制限値２
	VECTOR					ConstrainRotation1 ;			// 回転制限値１
	VECTOR					ConstrainRotation2 ;			// 回転制限値２
	VECTOR					SpringPosition ;				// ばね移動値
	VECTOR					SpringRotation ;				// ばね回転値
} ;

// データフレーム情報構造体
struct MV1_FRAME_R
{
	MV1_FRAME_R				*DataPrev ;						// (読み取り専用)データ的に前のフレーム
	MV1_FRAME_R				*DataNext ;						// (読み取り専用)データ的に次のフレーム
	int						Index ;							// (読み取り専用)インデックス(ノードを追加すると値が変化する可能性有り)
#ifndef UNICODE
	const char				*NameA ;						// (読み取り専用)名前
#endif
	const wchar_t			*NameW ;						// (読み取り専用)名前

	int						TotalMeshNum ;					// (読み取り専用)自分の下層にあるメッシュの総数
	int						TotalChildNum ;					// (読み取り専用)自分の下層にあるフレームの数

	int						Visible ;						// 表示、非表示設定( 0:非表示  1:表示 )

	DWORD					UserID ;						// ユーザーが設定できるＩＤ
	void					*UserData ;						// ユーザーデータ

	VECTOR					TempVector ;					// セットアップ作業で使っていい汎用ベクトル
	MATRIX					LocalWorldMatrix ;				// セットアップ作業で使っていいローカル→ワールド変換行列
	MATRIX					Matrix ;						// 変換行列
	VECTOR					Translate ;						// 平行移動
	VECTOR					Scale ;							// スケール
	VECTOR					Rotate ;						// 回転
	int						RotateOrder ;					// 回転オーダー( MV1_ROTATE_ORDER_XYZ等 )
	FLOAT4					Quaternion ;					// 回転( クォータニオン )
	int						IgnoreParentTransform ;			// 親の座標変換を無視するかどうか( 1:無視する 0:無視しない )

	int						EnablePreRotate ;				// 前回転が有効か( 1:有効  0:無効 )
	int						EnablePostRotate ;				// 後回転が有効か( 1:有効  0:無効 )
	VECTOR					PreRotate ;						// 前回転
	VECTOR					PostRotate ;					// 後回転

	MV1_LIGHT_R				*Light ;						// ライトの情報( ライトでは無い場合は NULL )

	DWORD					MaxBoneUseNum ;					// (読み取り専用)一つの頂点データがブレンドしているボーンの最大数
	float					SmoothingAngle ;				// 法線をスムージングする面と面の最大角度差( 単位はラジアン )

	int						MeshNum ;						// (読み取り専用)メッシュの数
	MV1_MESH_R				*MeshFirst ;					// (読み取り専用)先頭のメッシュへのポインタ
	MV1_MESH_R				*MeshLast ;						// (読み取り専用)末端のメッシュへのポインタ

	int						ShapeNum ;						// (読み取り専用)シェイプデータの数
	MV1_SHAPE_R				*ShapeFirst ;					// (読み取り専用)先頭のシェイプデータへのポインタ
	MV1_SHAPE_R				*ShapeLast ;					// (読み取り専用)末端のシェイプデータへのポインタ

	MV1_FRAME_R				*Parent ;						// (読み取り専用)親ノード
	MV1_FRAME_R				*ChildFirst ;					// (読み取り専用)長男ノード
	MV1_FRAME_R				*ChildLast ;					// (読み取り専用)末っ子ノード
	MV1_FRAME_R				*Next ;							// (読み取り専用)弟ノード
	MV1_FRAME_R				*Prev ;							// (読み取り専用)兄ノード

	MV1_FRAME_BASE			*MV1Frame ;						// (読み取り専用)このフレームに関連付けられた MV1_FRAME_BASE 構造体へのポインタ
} ;

// データ構造体
struct MV1_MODEL_R
{
	MEMINFO					*Mem ;							// (読み取り専用)メモリアドレス保存データへのポインタ

	int						CharCodeFormat ;				// char版のオブジェクト追加関数が呼ばれた際に想定する文字コード形式
	wchar_t					*FilePath ;						// ファイルパスへのポインタ( この文字列は MV1RAddString を使用してはいけない )
	wchar_t					*Name ;							// 名前へのポインタ( この文字列は MV1RAddString を使用してはいけない )
	int						MeshFaceRightHand ;				// 面情報が右手系の並びになっているかどうか( TRUE:なっている  FALSE:なっていない )
	int						AutoCreateNormal ;				// 法線の自動生成を使用するかどうか( TRUE:使用する  FALSE:使用しない )
	DWORD					AnimKeyDataSize ;				// アニメーションキーデータのサイズ
	int						TranslateIsBackCulling ;		// 半透明部分をバックカリングをするかどうか( 1:する  0:しない )
	int						AnimDataNotDecomposition ;		// アニメデータの分解を行わないかどうか( 1:行わない  0:行う )
	int						MaterialNumberOrderDraw ;		// 割り当てられているマテリアルの番号が低いものから描画するかどうか( 1:する  0:しない )

	int						TotalMV1MeshNum ;				// (読み取り専用)MV1_MESH_BASE 換算のメッシュの数
	int						ChangeDrawMaterialTableSize ;	// (読み取り専用)描画マテリアルの変更情報管理用に必要なデータの総容量
	int						ChangeMatrixTableSize ;			// (読み取り専用)行列の変更情報管理用に必要なデータの総容量

	DWORD					FrameNum ;						// (読み取り専用)ノードの数
	MV1_FRAME_R				*FrameFirst ;					// (読み取り専用)データ的に先頭のノード
	MV1_FRAME_R				*FrameLast ;					// (読み取り専用)データ的に末端のノード

	DWORD					MaterialNum ;					// (読み取り専用)マテリアルの数
	MV1_MATERIAL_R			*MaterialFirst ;				// (読み取り専用)データ的に先頭のマテリアル
	MV1_MATERIAL_R			*MaterialLast ;					// (読み取り専用)データ的に末端のマテリアル

	DWORD					TextureNum ;					// (読み取り専用)テクスチャの数
	MV1_TEXTURE_R			*TextureFirst ;					// (読み取り専用)データ的に先頭のテクスチャ
	MV1_TEXTURE_R			*TextureLast ;					// (読み取り専用)データ的に末端のテクスチャ

	DWORD					MeshNum ;						// (読み取り専用)メッシュの数
	MV1_MESH_R				*MeshFirst ;					// (読み取り専用)データ的に先頭のメッシュ
	MV1_MESH_R				*MeshLast ;						// (読み取り専用)データ的に末端のメッシュ

	DWORD					ShapeNum ;						// (読み取り専用)シェイプの数
	MV1_SHAPE_R				*ShapeFirst ;					// (読み取り専用)データ的に先頭のシェイプ
	MV1_SHAPE_R				*ShapeLast ;					// (読み取り専用)データ的に末端のシェイプ

	DWORD					PhysicsRigidBodyNum ;			// (読み取り専用)物理演算用剛体の数
	MV1_PHYSICS_RIGIDBODY_R	*PhysicsRigidBodyFirst ;		// (読み取り専用)データ的に先頭の物理演算用剛体
	MV1_PHYSICS_RIGIDBODY_R	*PhysicsRigidBodyLast ;			// (読み取り専用)データ的に末端の物理演算用剛体

	DWORD					PhysicsJointNum ;				// (読み取り専用)物理演算用剛体ジョイントの数
	MV1_PHYSICS_JOINT_R		*PhysicsJointFirst ;			// (読み取り専用)データ的に先頭の物理演算用剛体ジョイント
	MV1_PHYSICS_JOINT_R		*PhysicsJointLast ;				// (読み取り専用)データ的に末端の物理演算用剛体ジョイント

	DWORD					LightNum ;						// (読み取り専用)ライトの数
	MV1_LIGHT_R				*LightFirst ;					// (読み取り専用)データ的に先頭のライト
	MV1_LIGHT_R				*LightLast ;					// (読み取り専用)データ的に末端のライト

	DWORD					AnimSetNum ;					// (読み取り専用)アニメーションセットの数
	MV1_ANIMSET_R			*AnimSetFirst ;					// (読み取り専用)データ的に先頭のアニメーションセット
	MV1_ANIMSET_R			*AnimSetLast ;					// (読み取り専用)データ的に末端のアニメーションセット

	DWORD					AnimNum ;						// (読み取り専用)アニメーションの数
	MV1_ANIM_R				*AnimFirst ;					// (読み取り専用)データ的に先頭のアニメーション
	MV1_ANIM_R				*AnimLast ;						// (読み取り専用)データ的に末端のアニメーション

	DWORD					AnimKeySetNum ;					// (読み取り専用)アニメーションキーセットの数
	MV1_ANIMKEYSET_R		*AnimKeySetFirst ;				// (読み取り専用)データ的に先頭のアニメーションキーセット
	MV1_ANIMKEYSET_R		*AnimKeySetLast ;				// (読み取り専用)データ的に末端のアニメーションキーセット

	int						ShapeNormalPositionNum ;		// (読み取り専用)シェイプ用ボーン情報無し座標データの数
	int						ShapeSkinPosition4BNum ;		// (読み取り専用)シェイプ用４ボーンスキニングメッシュ座標データの数
	int						ShapeSkinPosition8BNum ;		// (読み取り専用)シェイプ用８ボーンスキニングメッシュ座標データの数
	int						ShapeSkinPositionFREEBSize ;	// (読み取り専用)シェイプ用９ボーン以上スキニングメッシュ座標データのサイズ

	DWORD					MeshMaterialNum ;				// (読み取り専用)メッシュで使用されているマテリアルの数
	DWORD					TriangleNum ;					// (読み取り専用)３角形ポリゴンの数
	DWORD					TriangleListNum	;				// (読み取り専用)トライアングルリストの数
	DWORD					IndexNum ;						// (読み取り専用)頂点インデックスの数
	int						NormalPositionNum ;				// (読み取り専用)ボーン情報無し座標データの数
	int						SkinPosition4BNum ;				// (読み取り専用)４ボーンスキニングメッシュ座標データの数
	int						SkinPosition8BNum ;				// (読み取り専用)８ボーンスキニングメッシュ座標データの数
	int						SkinPositionFREEBSize ;			// (読み取り専用)９ボーン以上スキニングメッシュ座標データのサイズ
	int						MeshPositionSize ;				// (読み取り専用)メッシュ座標のデータサイズ
	int						MeshNormalNum ;					// (読み取り専用)メッシュ法線の数
	int						MeshVertexSize ;				// (読み取り専用)メッシュ頂点情報のデータサイズ
	int						MeshFaceNum ;					// (読み取り専用)メッシュ面情報の数
	DWORD					MeshVertexIndexNum ;			// (読み取り専用)メッシュ頂点データインデックスの総数
	DWORD					StringSizeA ;					// (読み取り専用)文字列データのサイズ
	DWORD					StringSizeW ;					// (読み取り専用)文字列データのサイズ
	DWORD					ShapeMeshNum ;					// (読み取り専用)シェイプメッシュの数
	DWORD					ShapeVertexNum ;				// (読み取り専用)シェイプ頂点データの数

	DWORD					SkinWeightNum ;					// (読み取り専用)スキンメッシュのウエイトに関する情報の数
} ;

// 関数宣言 -------------------------------------

extern	int					MV1InitReadModel( MV1_MODEL_R *ReadModel ) ;							// 読み込み処理用モデル構造体の初期化
extern	int					MV1TermReadModel( MV1_MODEL_R *ReadModel ) ;							// 読み込み処理用モデル構造体の後始末

#ifndef UNICODE
extern	char				*MV1RAddString(					MV1_MODEL_R *ReadModel, const char    *String ) ;							// 文字列の追加
#endif
extern	wchar_t				*MV1RAddStringW(				MV1_MODEL_R *ReadModel, const wchar_t *String ) ;							// 文字列の追加
#ifndef UNICODE
extern	char				*MV1RAddStringWToA(				MV1_MODEL_R *ReadModel, const wchar_t *String ) ;							// 文字列の追加
#endif
extern	wchar_t				*MV1RAddStringAToW(				MV1_MODEL_R *ReadModel, const char    *String ) ;							// 文字列の追加
extern	MV1_FRAME_R			*MV1RAddFrame(					MV1_MODEL_R *ReadModel, const char    *Name, MV1_FRAME_R *Parent ) ;		// フレームの追加
extern	MV1_FRAME_R			*MV1RAddFrameW(					MV1_MODEL_R *ReadModel, const wchar_t *Name, MV1_FRAME_R *Parent ) ;		// フレームの追加
extern	MV1_MESH_R			*MV1RAddMesh(					MV1_MODEL_R *ReadModel, MV1_FRAME_R *Frame ) ;								// メッシュの追加
extern	void				MV1RSubMesh(					MV1_MODEL_R *ReadModel, MV1_FRAME_R *Frame, MV1_MESH_R *Mesh ) ;			// メッシュの削除
extern	int					MV1RSetupMeshFaceBuffer(		MV1_MODEL_R *ReadModel, MV1_MESH_R *Mesh, int FaceNum, int MaxIndexNum ) ;	// メッシュの面用のバッファをセットアップする
extern	MV1_SHAPE_R			*MV1RAddShape(					MV1_MODEL_R *ReadModel, const char    *Name, MV1_FRAME_R *Frame ) ;			// シェイプデータの追加
extern	MV1_SHAPE_R			*MV1RAddShapeW(					MV1_MODEL_R *ReadModel, const wchar_t *Name, MV1_FRAME_R *Frame ) ;			// シェイプデータの追加
extern	MV1_PHYSICS_RIGIDBODY_R *MV1RAddPhysicsRididBody(	MV1_MODEL_R *ReadModel, const char    *Name, MV1_FRAME_R *TargetFrame ) ;	// 物理演算用剛体データの追加
extern	MV1_PHYSICS_RIGIDBODY_R *MV1RAddPhysicsRididBodyW(	MV1_MODEL_R *ReadModel, const wchar_t *Name, MV1_FRAME_R *TargetFrame ) ;	// 物理演算用剛体データの追加
extern	MV1_PHYSICS_JOINT_R *MV1RAddPhysicsJoint(			MV1_MODEL_R *ReadModel, const char    *Name ) ;								// 物理演算用剛体ジョイントデータの追加
extern	MV1_PHYSICS_JOINT_R *MV1RAddPhysicsJointW(			MV1_MODEL_R *ReadModel, const wchar_t *Name ) ;								// 物理演算用剛体ジョイントデータの追加
extern	MV1_MATERIAL_R		*MV1RAddMaterial(				MV1_MODEL_R *ReadModel, const char    *Name ) ;								// マテリアルの追加
extern	MV1_MATERIAL_R		*MV1RAddMaterialW(				MV1_MODEL_R *ReadModel, const wchar_t *Name ) ;								// マテリアルの追加
extern	MV1_MATERIAL_R		*MV1RGetMaterial(				MV1_MODEL_R *ReadModel, int Index ) ;										// 指定インデックスのマテリアルを取得する
extern	MV1_TEXTURE_R		*MV1RAddTexture(				MV1_MODEL_R *ReadModel, const char    *Name, const char    *ColorFilePath, const char    *AlphaFilePath = NULL, int BumpMapFlag = FALSE, float BumpMapNextPixelLength = 0.1f, bool FilePathDoubleCancel = true, bool ReverseFlag = false, bool Bmp32AllZeroAlphaToXRGB8Flag = true, bool OpenFailedReturn = false ) ;	// テクスチャの追加
extern	MV1_TEXTURE_R		*MV1RAddTextureW(				MV1_MODEL_R *ReadModel, const wchar_t *Name, const wchar_t *ColorFilePath, const wchar_t *AlphaFilePath = NULL, int BumpMapFlag = FALSE, float BumpMapNextPixelLength = 0.1f, bool FilePathDoubleCancel = true, bool ReverseFlag = false, bool Bmp32AllZeroAlphaToXRGB8Flag = true, bool OpenFailedReturn = false ) ;	// テクスチャの追加
extern	MV1_SKIN_WEIGHT_R	*MV1RAddSkinWeight( 			MV1_MODEL_R *ReadModel ) ;													// スキン情報の追加
extern	MV1_ANIMSET_R		*MV1RAddAnimSet(				MV1_MODEL_R *ReadModel, const char    *Name ) ;								// アニメーションセットの追加
extern	MV1_ANIMSET_R		*MV1RAddAnimSetW(				MV1_MODEL_R *ReadModel, const wchar_t *Name ) ;								// アニメーションセットの追加
extern	MV1_ANIM_R			*MV1RAddAnim(					MV1_MODEL_R *ReadModel, MV1_ANIMSET_R *AnimSet ) ;							// アニメーションの追加
extern	MV1_ANIMKEYSET_R	*MV1RAddAnimKeySet(				MV1_MODEL_R *ReadModel, MV1_ANIM_R *Anim ) ;								// アニメーションキーセットの追加
extern	MV1_LIGHT_R			*MV1RAddLight(					MV1_MODEL_R *ReadModel, const char    *Name ) ;								// ライトの追加
extern	MV1_LIGHT_R			*MV1RAddLightW(					MV1_MODEL_R *ReadModel, const wchar_t *Name ) ;								// ライトの追加
extern	int					MV1GetFrameIndex(				MV1_MODEL_R *ReadModel, DWORD UserID ) ;									// 指定のフレームＩＤを持つノードのインデックスを得る
extern	int					MV1RLoadFile(  const char    *FilePath, void **FileImage, int *FileSize ) ;									// ファイルを丸ごと読み込む( -1:読み込み失敗 )
extern	int					MV1RLoadFileW( const wchar_t *FilePath, void **FileImage, int *FileSize ) ;									// ファイルを丸ごと読み込む( -1:読み込み失敗 )
extern	int					MV1RMakeMatrix( VECTOR *PreRotate, VECTOR *Rotate, VECTOR *PostRotate, VECTOR *Scale, VECTOR *Translate, VECTOR *MatrixRXYZ, MATRIX *DestBuffer, int RotateOrder = MV1_ROTATE_ORDER_XYZ ) ;	// 座標変換行列の作成

extern	int					MV1LoadModelToReadModel( const MV1LOADMODEL_GPARAM *GParam, MV1_MODEL_R *ReadModel, const wchar_t *CurrentDir, const MV1_FILE_READ_FUNC *ReadFunc, int ASyncThread ) ;	// 読み込み処理用モデルから基本モデルデータを作成する

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif	// DX_NON_MODEL

#endif

