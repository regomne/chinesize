// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		モデルデータ制御プログラム
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXMODEL_H__
#define __DXMODEL_H__

#include "DxCompileConfig.h"

#ifndef DX_NON_GRAPHICS

#ifndef DX_NON_MODEL

// インクルード ---------------------------------
#include "DxLib.h"
#include "DxStatic.h"
#include "DxBaseFunc.h"
#include "DxGraphics.h"
#include "DxUseCLib.h"
#include "DxMemory.h"
//#include "DxVertexShader.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 -----------------------------------

#define MV1_ANIMSET_NUM_UNIT				(4)					// アニメーションセットアドレス追加単位
#define MV1_ANIM_NUM_UNIT					(4)					// アニメーションアドレス追加単位
#define MV1_TRIANGLE_MAX_INDEX				(65500)				// １トライアングルリスト辺りの最大インデックス数

#define MV1_VERTEXBUFFER_MAX_VERTNUM		(65536 * 6)			// 一つの頂点バッファに格納する最大頂点数
#define MV1_INDEXBUFFER_MAX_INDEXNUM		(65536 * 12)		// 一つの頂点バッファに格納する最大インデックス数

#define MV1_ADD_LOAD_FUNC_MAXNUM			(128)				// 追加できるロード関数の最大数

#define MV1_LOADCALC_PHYSICS_GRAVITY_NUM	(256)				// 読み込み時に適用できる重力の種類数

#define MV1_DISABLEPHYSICS_WORD_MAXNUM		(256)				// 物理演算無効ワードの最大数
#define MV1_DISABLEPHYSICS_WORD_MAXLENGTH	(63)				// 物理演算無効ワードで使用できる文字の最大長

#define MV1_TRIANGLE_LIST_USE_BONE_MAX_NUM	(54)				// 一つのトライアングルリストが使えるボーンの最大数

#define MV1_PHYSICS_DEFAULT_GRAVITY			(-9.8f * 12.5f)		// 物理演算のデフォルトの重力

// フレームのフラグ
#define MV1_FRAMEFLAG_VISIBLE				(0x00000001)		// 表示フラグ( 1:表示  0:非表示 )
#define MV1_FRAMEFLAG_IGNOREPARENTTRANS		(0x00000002)		// 親の座標変換を無視するかどうか( 1:無視する 0:無視しない )
#define MV1_FRAMEFLAG_PREROTATE				(0x00000004)		// 前回転が有効かどうか( 1:有効  0:無効 )
#define MV1_FRAMEFLAG_POSTROTATE			(0x00000008)		// 後回転が有効かどうか( 1:有効  0:無効 )
#define MV1_FRAMEFLAG_TANGENT_BINORMAL		(0x00000010)		// MV1_MESH_NORMAL に含まれる Tangent と Binormal が有効な値かどうか( 1:有効な値  0:無効な値 )

// ライトのタイプ
#define MV1_LIGHT_TYPE_POINT				(0)					// ポイントライト
#define MV1_LIGHT_TYPE_SPOT					(1)					// スポットライト
#define MV1_LIGHT_TYPE_DIRECTIONAL			(2)					// ディレクショナルライト

// 頂点タイプ
#define MV1_VERTEX_TYPE_NORMAL				(0)					// 剛体メッシュ用
#define MV1_VERTEX_TYPE_SKIN_4BONE			(1)					// １〜４ボーンのスキニングメッシュ用
#define MV1_VERTEX_TYPE_SKIN_8BONE			(2)					// ５〜８ボーンのスキニングメッシュ用
#define MV1_VERTEX_TYPE_SKIN_FREEBONE		(3)					// ボーン数無制限のスキニングメッシュ用
#define MV1_VERTEX_TYPE_NUM					(4)					// 頂点タイプの数

// 回転タイプ
#define MV1_ROTATE_TYPE_XYZROT				(0)					// X,Y,Z軸回転
#define MV1_ROTATE_TYPE_QUATERNION			(1)					// クォータニオン
#define MV1_ROTATE_TYPE_MATRIX				(2)					// 行列
#define MV1_ROTATE_TYPE_ZAXIS				(3)					// Ｚ軸方向+上方向ベクトル+捻り回転指定

// ＸＹＺ回転オーダー
#define MV1_ROTATE_ORDER_XYZ				(0)					// XYZ
#define MV1_ROTATE_ORDER_XZY				(1)					// XZY
#define MV1_ROTATE_ORDER_YZX				(2)					// YZX
#define MV1_ROTATE_ORDER_YXZ				(3)					// YXZ
#define MV1_ROTATE_ORDER_ZXY				(4)					// ZXY
#define MV1_ROTATE_ORDER_ZYX				(5)					// ZYX

// アニメーション要素のフラグ
#define MV1_ANIMVALUE_TRANSLATE				(0x01)				// 平行移動
#define MV1_ANIMVALUE_SCALE					(0x02)				// 拡大
#define MV1_ANIMVALUE_ROTATE				(0x04)				// 回転
#define MV1_ANIMVALUE_QUATERNION_X			(0x08)				// 回転( Xファイルタイプクォータニオン )
#define MV1_ANIMVALUE_MATRIX				(0x10)				// 行列
#define MV1_ANIMVALUE_QUATERNION_VMD		(0x20)				// 回転( VMDタイプクォータニオン )

// キータイプ
//#define MV1_KEY_TYPE_STEP					(0)					// 定数
//#define MV1_KEY_TYPE_LINEAR				(1)					// 線形補間
//#define MV1_KEY_TYPE_SPLINE				(2)					// スプライン
//#define MV1_KEY_TYPE_FLAT					(3)					// スプライン(傾き０)

// レイヤーブレンドパラメータ
#define MV1_LAYERBLEND_TYPE_TRANSLUCENT		(0)					// アルファ合成
#define MV1_LAYERBLEND_TYPE_ADDITIVE		(1)					// 加算
#define MV1_LAYERBLEND_TYPE_MODULATE		(2)					// 乗算
#define MV1_LAYERBLEND_TYPE_MODULATE2		(3)					// 乗算×２

// データサイズ指定識別子
#define MV1_DATASIZE_VERTEX					(1)					// 頂点データ
#define MV1_DATASIZE_STRING					(2)					// 文字列データ
#define MV1_DATASIZE_ANIM					(3)					// アニメーションデータ
//#define MV1_DATASIZE_OTHER				(4)					// その他

/*
// テクスチャのアドレスモード
#define MV1_TEXTURE_ADDRESS_MODE_WRAP		(0)					// 単純繰り返し
#define MV1_TEXTURE_ADDRESS_MODE_MIRROR		(1)					// ミラーリング
#define MV1_TEXTURE_ADDRESS_MODE_CLAMP 		(2)					// 端のピクセルが続く

// テクスチャのフィルタリングタイプ
#define MV1_TEXTURE_FILTER_MODE_POINT		(0)					// 最近点サンプリング
#define MV1_TEXTURE_FILTER_MODE_LINEAR		(1)					// バイリニアフィルタリング
#define MV1_TEXTURE_FILTER_MODE_ANISOTROPIC	(2)					// 異方性フィルタリング
*/

// チェック用ＩＤ
#define MV1_FRAME_CHECK_ID					(0x5F4D5246)		// フレームのチェック用ＩＤ

#if !defined( __BCC ) || defined( _DEBUG )
	#define ADDMEMAREA( size, mem )				AddMemArea( (size), (mem), __FILE__, __LINE__ )
	#define SUBMEMAREA( mem, buffer )			SubMemArea( (mem), (buffer) )
	#define MDALLOCMEM( size )					NS_DxAlloc( (size),        __FILE__, __LINE__ )
#else
	#define ADDMEMAREA( size, mem )				AddMemArea( (size), (mem) )
	#define SUBMEMAREA( mem, buffer )			SubMemArea( (mem), (buffer) )
	#define MDALLOCMEM( size )					NS_DxAlloc( (size) )
#endif
//#define MDFREEMEM( mem )						MDFreeMem( (mem) )
#define MDFREEMEM( mem )						NS_DxFree( (mem) )


// １６バイト境界にあわせるマクロ
#define ADDR16( addr )		( ( BYTE * )( ( ( DWORD_PTR )( addr ) + 15 ) / 16 * 16 ) )

// モデル基本データハンドル有効性チェック
#define MV1BMDLCHK( HAND, MPOINT )			HANDLECHK(       DX_HANDLETYPE_MODEL_BASE, HAND, *( ( HANDLEINFO ** )&MPOINT ) )
#define MV1BMDLCHK_ASYNC( HAND, MPOINT )	HANDLECHK_ASYNC( DX_HANDLETYPE_MODEL_BASE, HAND, *( ( HANDLEINFO ** )&MPOINT ) )

// モデルハンドル有効性チェック
#define MV1MDLCHK( HAND, MPOINT )			HANDLECHK(       DX_HANDLETYPE_MODEL, HAND, *( ( HANDLEINFO ** )&MPOINT ) )
#define MV1MDLCHK_ASYNC( HAND, MPOINT )		HANDLECHK_ASYNC( DX_HANDLETYPE_MODEL, HAND, *( ( HANDLEINFO ** )&MPOINT ) )

// トライアングルリスト基データの環境依存用情報用バッファのサイズ
#define MV1_TRIANGLE_LIST_BASE_PF_BUFFER_SIZE		(16)

// 構造体定義 -----------------------------------

// トライアングルリスト構築時頂点チェック用構造体
struct MV1_MAKEVERTINDEXINFO
{
	union
	{
		int						VertexIndex ;					// トライアングルリストでの頂点インデックス
		void					*Face ;							// 頂点を参照している面
	} ;
	int							TriangleListIndex ;				// 属するトライアングルリストのインデックス
	void						*Mesh ;							// 面を持つメッシュ
	struct MV1_MAKEVERTINDEXINFO *Next ;						// 次のデータへのポインタ
} ;


// データ一致検査用構造体
struct DATACMPINFO
{
	DWORD					Size ;								// データのサイズ
	DWORD					Data[ 8 ] ;							// チェック用データ
} ;

// メモリアドレス保存データ型
struct MEMINFO
{
	void					*Data ;								// データアドレス
	struct MEMINFO			*PrevInfo ;							// 前のメモリアドレス保存データへのポインタ
	struct MEMINFO			*NextInfo ;							// 次のメモリアドレス保存データへのポインタ
	int						TotalSize ;
} ;

// ビットデータリスト
struct BITLIST
{
	int						DataNum ;							// 有効なデータの数
	int						MaxDataNum ;						// 確保されているバッファの数
	int						BitDepth ;							// ビット深度
	int						UnitSize ;							// データ一つ辺りのバイトサイズ
	void					*Data ;								// データ本体( 各データの最終バイトは Press されたデータかによって変化( 非Press( 使用中かどうか( 1:使用中 0:未使用 ) ) Press( 立っているビットの数 ) )
	int						*PressData ;						// 非Press:要素を抽出した際の抽出後の種類番号リスト  Press:対応するデータの出現回数
} ;

// 4x4構造体の4列目が(0,0,0,1)決め打ちの行列
// 更に普通の MATRIX とは配列の要素の意味が一次元目と二次元目が逆
// MATRIX.m[ 行 ][ 列 ],  MATRIX_4X4CT.m[ 列 ][ 行 ]
struct MATRIX_4X4CT_F
{
	float					m[ 3 ][ 4 ] ;
} ;

struct MATRIX_4X4CT_D
{
	double					m[ 3 ][ 4 ] ;
} ;

union MATRIX_4X4CT
{
	MATRIX_4X4CT_F			mf ;
	MATRIX_4X4CT_D			md ;
} ;


// コリジョン用ポリゴン情報構造体
struct MV1_COLL_POLYGON
{
	MV1_REF_POLYGON			*Polygon ;							// 参照用ポリゴンへのポインタ
	struct MV1_COLL_POLYGON	*Next ;								// 次のコリジョン用ポリゴン情報へのポインタ
} ;

// コリジョン用ポリゴン情報を格納するバッファ構造体
struct MV1_COLL_POLY_BUFFER
{
	int						BufferSize ;						// バッファのサイズ
	int						UseSize ;							// 使用している数
	MV1_COLL_POLYGON		*Buffer ;							// バッファ
	struct MV1_COLL_POLY_BUFFER	*Next ;								// 次のバッファ構造体へのポインタ
} ;

// コリジョン情報構造体
struct MV1_COLLISION
{
	int						XDivNum, YDivNum, ZDivNum ;			// 各軸の分割数
	VECTOR					MinPosition ;						// 賄う範囲の最小値
	VECTOR					MaxPosition ;						// 賄う範囲の最大値
	VECTOR					Size ;								// 賄う範囲のサイズ
	VECTOR					UnitSize ;							// 格子一つあたりの大きさ
	VECTOR					UnitSizeRev ;						// UnitSize の逆数
	MV1_COLL_POLYGON		**Polygon ;							// コリジョン用ポリゴン情報へのポインタ配列、スペースに一つもポリゴンがない場合は NULL、インデックス算出法は [ X + Y * XDivNum + Z * ( XDivNum * YDivNum ) ]
	MV1_COLL_POLY_BUFFER	*FirstBuffer ;						// コリジョン用ポリゴン情報格納用バッファの先端
	MV1_COLL_POLY_BUFFER	*LastBuffer ;						// コリジョン用ポリゴン情報格納用バッファの末端
} ;

// 回転情報構造体
struct MV1_ROTATE
{
	int						Type ;								// 回転タイプ( MV1_ROTATE_TYPE_XYZROT 等 )
	union
	{
		struct
		{
			VECTOR			Z ;									// Ｚ軸の向き
			VECTOR			Up ;								// 上方向の向き
			float			Twist ;								// 捻り回転角度
		} ZAxis ;												// Ｚ軸方向＋捻り角度用
		VECTOR				XYZRot ;							// X軸回転→Y軸回転→Z軸回転用
		FLOAT4				Qt ;								// クォータニオン回転用
		MATRIX_4X4CT_F		Mat ;								// 行列回転用
	} ;
} ;

// テクスチャ基データ構造体
struct MV1_TEXTURE_BASE
{
	int						NameAllocMem ;						// テクスチャの名前を保存するメモリを確保しているかどうか( TRUE:確保している  FALSE:していない )
#ifndef UNICODE
	char					*NameA ;							// テクスチャの名前
#endif
	wchar_t					*NameW ;							// テクスチャの名前

	int						AlphaImageFilePathAllocMem ;		// アルファチャンネル用画像のファイルパス用にメモリを確保しているかどうか( TRUE:確保している  FALSE:していない )
#ifndef UNICODE
	char					*AlphaFilePathA ;					// アルファチャンネル用画像のファイルパス
#endif
	wchar_t					*AlphaFilePathW ;
	void					*AlphaImage ;						// アルファチャンネル画像のファイルイメージへのポインタ
	int						AlphaImageSize ;					// アルファチャンネル画像のファイルイメージのサイズ

	int						ColorImageFilePathAllocMem ;		// アルファチャンネル用画像のファイルパス用にメモリを確保しているかどうか( TRUE:確保している  FALSE:していない )
#ifndef UNICODE
	char					*ColorFilePathA ;					// カラーチャンネル用画像のファイルパス
#endif
	wchar_t					*ColorFilePathW ;
	void					*ColorImage ;						// カラーチャンネル用画像のファイルイメージへのポインタ
	int						ColorImageSize ;					// カラーチャンネル用画像のファイルイメージのサイズ

	int						Width, Height ;						// 幅、高さ
	int						SemiTransFlag ;						// 半透明要素があるかどうか( TRUE:ある  FALSE:ない )
	int						BumpImageFlag ;						// 元がバンプマップ画像か( TRUE:バンプマップ  FALSE:違う )
	float					BumpImageNextPixelLength ;			// バンプマップ画像の場合の隣のピクセルとの距離

	int						GraphHandle ;						// グラフィックハンドル
	int						IsDefaultTexture ;					// デフォルトの画像かどうか( TRUE:デフォルトの画像  FALSE:ファイルから読み込んだ画像 )

	int						UseUserGraphHandle ;				// UserGraphHandle を使用しているかどうか	int						UseGraphHandle ;					// GraphHandle を使用しているかどうか( TRUE:使用している  FALSE:していない )
	int						UserGraphHandle ;					// ユーザーグラフィックハンドル
	int						UserGraphWidth, UserGraphHeight ;	// 幅、高さ
	int						UserGraphHandleSemiTransFlag ;		// ユーザーグラフィックハンドルに半透明要素があるかどうか

	int						AddressModeU ;						// アドレスモード( DX_TEXADDRESS_WRAP 等 )
	int						AddressModeV ;						// アドレスモード( DX_TEXADDRESS_WRAP 等 )
	float					ScaleU ;							// Ｕ座標のスケーリング値
	float					ScaleV ;							// Ｖ座標のスケーリング値
	int						FilterMode ;						// フィルタリングモード( DX_DRAWMODE_BILINEAR 等 )

	int						ReverseFlag ;						// 画像を反転するかどうか( 1:反転する  0:反転しない )
	int						Bmp32AllZeroAlphaToXRGB8Flag ;		// ３２ビットＢＭＰのアルファ値が全部０だったら XRGB8 として扱うかどうかのフラグ

	DWORD					UserData[ 2 ] ;						// 外部定義の情報
} ;

// マテリアルレイヤー構造体
struct MV1_MATERIAL_LAYER
{
	int						IsGraphHandleAlpha ;				// GraphHandle に1.0f以外のα値が含まれているかどうか( TRUE:含まれている  FALSE:含まれていない )
	int						GraphHandle ;						// Texture の代わりに使用するグラフィックハンドル、0の場合は無効
	int						Texture ;							// モデルテクスチャインデックス( MV1_MODEL_BASE.Textue 配列のインデックス )
	int						BlendType ;							// ブレンドタイプ( MV1_LAYERBLEND_TYPE_ADDITIVE 等 )
} ;

// マテリアル基データ構造体
struct MV1_MATERIAL_BASE
{
#ifndef UNICODE
	char					*NameA ;							// 名前
#endif
	wchar_t					*NameW ;

	int						Type ;								// マテリアルタイプ( DX_MATERIAL_TYPE_NORMAL など )

	COLOR_F					Diffuse ;							// ディフューズ色
	COLOR_F					Ambient ;							// アンビエント色
	COLOR_F					Specular ;							// スペキュラー色
	COLOR_F					Emissive ;							// エミッシブ色
	float					Power ;								// スペキュラハイライトのパワー
	float					TypeParam[ MATERIAL_TYPEPARAM_MAX_NUM ] ;	// マテリアルタイプのパラメータ( DX_MATERIAL_TYPE_MAT_SPEC_LUMINANCE_TWO_COLOR などで使用 )

	int						DiffuseLayerNum ;					// ディフューズカラーのレイヤー数
	MV1_MATERIAL_LAYER		DiffuseLayer[ 8 ] ;					// ディフューズカラーのレイヤー情報( ０レイヤーの BlendType は無視される )
	int						SpecularLayerNum ;					// スペキュラマップのレイヤー数
	MV1_MATERIAL_LAYER		SpecularLayer[ 8 ] ;				// スペキュラマップのレイヤー情報( ０レイヤーの BlendType は無視される )
	int						NormalLayerNum ;					// 法線マップのレイヤー数
	MV1_MATERIAL_LAYER		NormalLayer[ 8 ] ;					// 法線マップのレイヤー情報( ０レイヤーの BlendType は無視される )

	int						DiffuseGradTexture ;				// ( トゥーンレンダリングでのみ使用 )ディフューズカラーグラデーションテクスチャインデックス、当たっているライトのディフューズカラーでＵ値が決まるもの、デフォルトテクスチャの場合は -1 or -2、0以上の場合は MV1_MODEL_BASE.Textue 配列のインデックス
	int						SpecularGradTexture ;				// ( トゥーンレンダリングでのみ使用 )スペキュラカラーグラデーションテクスチャインデックス、当たっているライトのスペキュラカラーでＵ値が決まるもの、デフォルトテクスチャの場合は -1 or -2、0以上の場合は MV1_MODEL_BASE.Textue 配列のインデックス
	int						SphereMapTexture ;					// ( トゥーンレンダリングでのみ使用 )スフィアマップテクスチャインデックス
	int						DiffuseGradBlendType ;				// ( トゥーンレンダリングでのみ使用 )ディフューズグラデーションテクスチャのブレンドタイプ( DX_MATERIAL_BLENDTYPE_TRANSLUCENT など )
	int						SpecularGradBlendType ;				// ( トゥーンレンダリングでのみ使用 )スペキュラグラデーションテクスチャのブレンドタイプ( DX_MATERIAL_BLENDTYPE_ADDITIVE など )
	int						SphereMapBlendType ;				// ( トゥーンレンダリングでのみ使用 )スフィアマップテクスチャのブレンドタイプ( DX_MATERIAL_BLENDTYPE_ADDITIVE など )
	float					OutLineWidth ;						// ( トゥーンレンダリングでのみ使用 )輪郭線の幅
	float					OutLineDotWidth ;					// ( トゥーンレンダリングでのみ使用 )輪郭線のドット単位での幅
	COLOR_F					OutLineColor ;						// ( トゥーンレンダリングでのみ使用 )輪郭線の色

	int						UseAlphaTest ;						// アルファテストを使用するかどうか
	int						AlphaFunc ;							// アルファテストモード( 今のところ未使用 )
	int						AlphaRef ;							// アルファテストの閾値

	int						DrawBlendMode ;						// 出力時のブレンドモード( DX_BLENDMODE_NOBLEND 等 )
	int						DrawBlendParam ;					// 出力時のブレンドパラメータ

	DWORD					UserData[ 4 ] ;						// 外部定義の情報
} ;

// ライト構造体
struct MV1_LIGHT
{
#ifndef UNICODE
	char					*NameA ;							// 名前
#endif
	wchar_t					*NameW ;							// 名前
	int						Index ;								// インデックス

	int						FrameIndex ;						// ライトを持っているフレームのインデックス
	int						Type ;								// ライトタイプ( MV1_LIGHT_TYPE_POINT 等 )
	COLOR_F					Diffuse ;							// ディフューズカラー
	COLOR_F					Specular ;							// スペキュラカラー
	COLOR_F					Ambient ;							// アンビエントカラー
	float					Range ;								// スポットライトの有効距離
	float					Falloff ;							// フォールオフ
	float					Attenuation0 ;						// ライトパラメータ０
	float					Attenuation1 ;						// ライトパラメータ１
	float					Attenuation2 ;						// ライトパラメータ２
	float					Theta ;								// スポットライトの内部コーンの角度範囲
	float					Phi ;								// スポットライトの外部コーンの角度範囲

	DWORD					UserData[ 2 ] ;						// 外部定義の情報
} ;



// 16bitデータタイプ用補助情報構造体
struct MV1_ANIM_KEY_16BIT
{
	BYTE					Min ;								// 最小値( bit7:０かどうか( 0:0以外 1:0 )  bit6:符号(0:+ 1:-)  bit5:乗数方向(0:+ 1:-) bit4〜0:乗数(最大10の15乗) ) 
	BYTE					Unit ;								// 16bit値１辺りの値( bit7:乗数方向(0:+ 1:-) bit6〜4:乗数(最大10の7乗) bit3〜0:乗算される値( 0〜15 ) )
} ;

// 16bitデータタイプ用補助情報構造体( float版 )
struct MV1_ANIM_KEY_16BIT_F
{
	float					Min ;								// 最小値
	float					Unit ;								// 16bit値１辺りの値
} ;

// アニメーションキー(４×４行列の４列目が(0,0,0,1)固定の行列タイプ)データ構造体
struct MV1_ANIM_KEY_MATRIX4X4C
{
	float					Matrix[ 4 ][ 3 ] ;					// 行列
} ;

// アニメーションキー(３×３行列タイプ)データ構造体
struct MV1_ANIM_KEY_MATRIX3X3
{
	float					Matrix[ 3 ][ 3 ] ;					// 行列
} ;

// アニメーションキーリスト基データ構造体
struct MV1_ANIM_KEYSET_BASE
{
	char								Type ;					// キータイプ( MV1_ANIMKEY_TYPE_QUATERNION等 )
	char								DataType ;				// データタイプ( MV1_ANIMKEY_DATATYPE_ROTATE等 )
	char								TimeType ;				// 時間タイプ( MV1_ANIMKEY_TIME_TYPE_KEY 等 )
	int									TargetShapeIndex ;		// 対象のシェイプインデックス( DataType が MV1_ANIMKEY_DATATYPE_SHAPE の場合のみ有効 )

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

	DWORD								UserData[ 1 ] ;			// 外部定義の情報

} ;

// アニメーション基データ構造体
struct MV1_ANIM_BASE
{
	struct MV1_ANIMSET_BASE	*Container ;						// このアニメーションを持っているアニメーションセットへのポインタ

	struct MV1_FRAME_BASE	*TargetFrame ;						// 対象となるフレーム
	int						TargetFrameIndex ;					// 対象となるフレームのインデックス
	float					MaxTime ;							// 各キーセットの中で一番長いキーセットの時間値
	int						RotateOrder ;						// 回転オーダー( MV1_ROTATE_ORDER_XYZ 等 )

	int						KeySetNum ;							// キーセットの数
	MV1_ANIM_KEYSET_BASE	*KeySet ;							// キーセット配列へのポインタ

	DWORD					UserData[ 2 ] ;						// 外部定義の情報
} ;

// アニメーションセット基データ構造体
struct MV1_ANIMSET_BASE
{
	int						NameAllocMem ;						// アニメーションの名前を保存するメモリを確保しているかどうか( TRUE:確保している  FALSE:していない )
#ifndef UNICODE
	char					*NameA ;							// アニメーションセット名
#endif
	wchar_t					*NameW ;

	DWORD					KeyDataSize ;						// アニメーションキーデータサイズ
	int						Index ;								// インデックス
	float					MaxTime ;							// 各アニメーションの中で一番長いアニメーションの時間値
	int						AnimNum ;							// アニメーションの数
	MV1_ANIM_BASE			*Anim ;								// アニメーションリスト
	int						IsAddAnim ;							// 加算アニメーションかどうか( 1:加算アニメーション  0:絶対値アニメーション )
	int						IsMatrixLinearBlend ;				// 各キーの補間を行列単位で線形補間を刷るかどうかのフラグ( 1:行列で線形補間  0:要素単位で補間 )
	int						IsLoopAnim ;						// ループ用アニメーションかどうか( 1:ループアニメーション  0:通常アニメーション )

	DWORD					UserData[ 4 ] ;						// 外部定義の情報
} ;

// ボーン数無制限スキンメッシュ用ブレンド情報
struct MV1_SKINBONE_BLEND
{
	int						Index ;								// 使用するボーンのインデックス、-1で終端( Model->SkinBone 配列のインデックス )
	float					W ;									// そのボーンのブレンド率
} ;

// ボーン情報無し座標データ( 16バイト )
struct MV1_TLIST_NORMAL_POS
{
	FLOAT4					Position ;							// 位置
	FLOAT4					Normal ;							// 法線
} ;

// ４ボーンスキニング用座標データ( 48バイト )
struct MV1_TLIST_SKIN_POS_4B
{
	float					MatrixWeight[ 4 ] ;					// 0  行列ウエイト
	FLOAT4					Position ;							// 16 位置
	VECTOR					Normal ;							// 32 法線
	BYTE					MatrixIndex[ 4 ] ;					// 44 行列インデックス
} ;

// ８ボーンスキニング用座標データ( 64バイト )
struct MV1_TLIST_SKIN_POS_8B
{
	float					MatrixWeight[ 8 ] ;					// 0  行列ウエイト
	VECTOR					Normal ;							// 32 法線
	BYTE					MatrixIndex1[ 4 ] ;					// 44 行列インデックス
	VECTOR					Position ;							// 48 位置
	BYTE					MatrixIndex2[ 4 ] ;					// 60 行列インデックス
} ;

// ９ボーン以上スキニング用座標データ
struct MV1_TLIST_SKIN_POS_FREEB
{
	FLOAT4					Position ;							// 座標
	FLOAT4					Normal ;							// 法線
	MV1_SKINBONE_BLEND		MatrixWeight[ 4 ] ;					// 行列ウエイト( ４つ以上の場合はこの後ろにも続く )
} ;

// トライアングルリスト基データ構造体
struct MV1_TRIANGLE_LIST_BASE
{
	struct MV1_MESH_BASE	*Container ;						// このトライアングルリストを持っているメッシュへのポインタ
	unsigned short			VertexType ;						// 頂点タイプ( MV1_VERTEX_TYPE_NORMAL など )
	unsigned short			PosUnitSize ;						// 頂点座標情報一つ辺りのデータサイズ
	unsigned short			TempUnitSize ;						// テンポラリ頂点バッファの１頂点辺りのサイズ
	int						VertexNum ;							// 頂点の数
	int						MaxBoneNum ;						// 関わるボーンの最大数( MV1_VERTEX_TYPE_SKIN_FREEBONE の際に使用 )
	int						UseBoneNum ;						// 使用しているボーンの数( MV1_VERTEX_TYPE_SKIN_2_4BONE の際に使用 )
	int						UseBone[ MV1_TRIANGLE_LIST_USE_BONE_MAX_NUM ] ;	// 使用するボーンの配列、-1の場合は使用しないということ( MV1_FRAME.UseSkinBone 配列のインデックス )( MV1_VERTEX_TYPE_SKIN_4BONE, MV1_VERTEX_TYPE_SKIN_8BONE, MV1_VERTEX_TYPE_SKIN_FREE の際に使用 )

	DWORD					*MeshVertexIndex ;					// 座標頂点データの素となっているメッシュの頂点データインデックスの配列( MV1_MESH_BASE.Vertex のインデックス、VertexNum の数だけ存在する )
	int						MeshVertexIndexAllocMem ;			// MeshVertexIndex 用にメモリを確保しているかどうか( TRUE:確保している  FALSE:していない )

	BYTE					PFBuffer[ MV1_TRIANGLE_LIST_BASE_PF_BUFFER_SIZE ] ;	// 環境依存データ格納用バッファ( MV1_TRIANGLE_LIST_BASE_PF を格納する )
	union
	{
		MV1_TLIST_NORMAL_POS     *NormalPosition ;				// ボーン情報無し頂点座標データ
		MV1_TLIST_SKIN_POS_4B    *SkinPosition4B ;				// ４ボーンスキニング処理用頂点座標データ
		MV1_TLIST_SKIN_POS_8B    *SkinPosition8B ;				// ８ボーンスキニング処理用頂点座標データ
		MV1_TLIST_SKIN_POS_FREEB *SkinPositionFREEB ;			// ９ボーン以上のスキニング処理用頂点座標データ
	} ;
	int						PositionAllocMem ;					// 座標データ用にメモリを確保しているかどうか( TRUE:確保している  FALSE:していない )

	unsigned short			IndexNum ;							// 頂点インデックスの数
	unsigned short			*Index ;							// 頂点インデックス

	unsigned short			ToonOutLineIndexNum ;				// トゥーン輪郭線用頂点インデックスの数
	unsigned short			*ToonOutLineIndex ;					// トゥーン輪郭線用頂点インデックス

	int						PackDrawMaxNum ;					// 同時複数描画の最大数

	struct MV1_VERTEXBUFFER	*VertexBuffer ;						// 使用する頂点バッファ
	int						VBStartVertex ;						// バーテックスバッファ上で使用している頂点の開始番号
	int						VBStartIndex ;						// バーテックスバッファ上で使用しているインデックスの開始番号
	int						ToonOutLineVBStartIndex ;			// バーテックスバッファ上で使用しているトゥーン輪郭線用頂点インデックスの開始番号
	int						ObjectDuplicateNum ;				// バーテックスバッファに格納されているコピーの数
} ;

// スキンメッシュ用ボーンを使用するフレームの情報
struct MV1_SKIN_BONE_USE_FRAME
{
	int						Index ;								// ボーンを使用しているフレームのインデックス
	int						MatrixIndex ;						// このボーンがセットされているフレーム内インデックス( MV1_FRAME_BASE.UseSkinBone のインデックス )
} ;

// スキンメッシュ用ボーン情報
struct MV1_SKIN_BONE
{
	MATRIX_4X4CT_F			ModelLocalMatrix ;					// モデル座標からボーンのローカル座標に変換するための行列
	int						ModelLocalMatrixIsTranslateOnly ;	// モデル座標からボーンのローカル座標に変換するための行列が平行移動のみかどうか( 1:平行移動のみ  0:回転も含む )
	int						BoneFrame ;							// ボーンとして使用するフレーム
	int						UseFrameNum ;						// このボーンを使用するフレームの数
	MV1_SKIN_BONE_USE_FRAME	*UseFrame ;							// このボーンを使用するフレームの情報
} ;

// 状態変更管理用基本情報構造体
struct MV1_CHANGE_BASE
{
	DWORD					Target ;							// 状態変化が発生した際に論理和するべき対象の開始メモリアドレスへのオフセット( ４バイト単位 )
	DWORD					*Fill ;								// 状態変化が発生した際に論理和するフラグデータ、NULLの場合は CheckBit が Fill の代わり
	DWORD					Size ;								// 状態変化が発生した際に論理和するフラグデータのサイズ( ４バイト単位 )、Fill が NULL の場合は０
	DWORD					CheckBit ;							// 自分のフラグビットが立ったビット情報
} ;

// メッシュ構造体用頂点データ
struct MV1_MESH_POSITION
{
	VECTOR					Position ;							// 座標
	MV1_SKINBONE_BLEND		BoneWeight[ 4 ] ;					// 頂点ブレンドの情報( スキニングメッシュのときのみ有効、「４」としているのは仮で、実際は MV1_MESH_BASE.MaxBoneBlendNum の数だけ存在する )
} ;

// メッシュ構造体用法線データ
struct MV1_MESH_NORMAL
{
	VECTOR					Normal ;							// 法線
	VECTOR					Tangent ;							// 接線
	VECTOR					Binormal ;							// 従法線
} ;

// メッシュ構造体用頂点データ
struct MV1_MESH_VERTEX
{
	DWORD					PositionIndex ;						// 使用する頂点
	DWORD					NormalIndex ;						// 使用する法線
	COLOR_U8				DiffuseColor ;						// 頂点ディフューズカラー
	COLOR_U8				SpecularColor ;						// 頂点スペキュラカラー
	float					ToonOutLineScale ;					// トゥーンの輪郭線の表示倍率
	float					UVs[ 1 ][ 2 ] ;						// テクスチャ座標( ＵＶセットが一つではなかったり、ＵＶセット一つ辺りの要素数が２つではない場合は添字の値が変化する、ここに書かれているのはあくまでも仮 )
} ;

// メッシュ構造体用面データ
struct MV1_MESH_FACE
{
	WORD					VertexType ;						// 頂点タイプ( MV1_VERTEX_TYPE_NORMAL など )
	WORD					TriangleListIndex ;					// 所属するトライアングルリスト番号
	DWORD					VertexIndex[ 3 ] ;					// 面を構成する頂点インデックス
} ;

// メッシュ基データ構造体
struct MV1_MESH_BASE
{
	struct MV1_FRAME_BASE	*Container ;						// このメッシュを持っているフレームへのポインタ
	MV1_MATERIAL_BASE		*Material ;							// 使用するマテリアルへのポインタ

	MV1_CHANGE_BASE			ChangeInfo ;						// 状態変化管理用基本情報

	int						UseVertexDiffuseColor ;				// 頂点のディフューズカラーをマテリアルのディフューズカラーの代わりに使用するかどうか( TRUE:使用する  FALSE:使用しない )
	int						UseVertexSpecularColor ;			// 頂点のスペキュラカラーをマテリアルのスペキュラカラーの代わりに使用するかどうか( TRUE:使用する  FALSE:使用しない )
	int						RenderRef ;							// 描画用参照カウント
	int						NotOneDiffuseAlpha ;				// ディフューズカラーのアルファ値で 100% 以外のものがあるかどうか( 1:ある  0:ない )

	VECTOR					MaxPosition ;						// このメッシュに含まれる頂点座標の最大値( メッシュローカル座標 )
	VECTOR					MinPosition ;						// このメッシュに含まれる頂点座標の最小値( メッシュローカル座標 )

	int						TriangleListNum ;					// トライアングルリストの数
	MV1_TRIANGLE_LIST_BASE	*TriangleList ;						// トライアングルリストのリストへのポインタ

	BYTE					Visible ;							// 表示フラグ( 1:表示する  0:表示しない )
	BYTE					BackCulling ;						// バックカリングをするかどうか( 1:する  0:しない )
	BYTE					Shape ;								// シェイプメッシュかどうか( 1:シェイプメッシュ  0:通常メッシュ )
	BYTE					SemiTransState ;					// 半透明要素があるかどうか( 1:半透明要素がある  0:不透明 )

	int						UVSetUnitNum ;						// 一つの座標データに含まれるテクスチャ座標セットの数
	int						UVUnitNum ;							// 一つの座標データに含まれるテクスチャ座標の数

	int						VertexNum ;							// 頂点の数
	int						VertUnitSize ;						// 頂点一つ辺りのデータサイズ
	MV1_MESH_VERTEX			*Vertex ;							// 頂点
	int						VertexAllocMem ;					// 頂点データ用にメモリを確保しているかどうか( TRUE:している  FALSE:していない )

	int						FaceNum ;							// 面の数
	MV1_MESH_FACE			*Face ;								// 面

	DWORD					UserData[ 4 ] ;						// 外部定義の情報
} ;

// シェイプ頂点基データ構造体
struct MV1_SHAPE_VERTEX_BASE
{
	int						TargetMeshVertex ;					// 対象となる頂点番号( MV1_MESH_BASE.Vertex に対するインデックス )
	VECTOR					Position ;							// 座標( 元の座標に対する差分 )
	VECTOR					Normal ;							// 法線
} ;

// シェイプメッシュ基データ構造体
struct MV1_SHAPE_MESH_BASE
{
	MV1_MESH_BASE			*TargetMesh ;						// 対象となるメッシュ

	DWORD					VertexNum ;							// 頂点データの数
	MV1_SHAPE_VERTEX_BASE	*Vertex ;							// 頂点データ配列へのポインタ
	int						VertexAllocMem ;					// 頂点データ用にメモリを確保しているかどうか( TRUE:している  FALSE:していない )
} ;

// シェイプ基データ構造体
struct MV1_SHAPE_BASE
{
	struct MV1_FRAME_BASE	*Container ;						// このシェイプを持っているフレームのポインタ

#ifndef UNICODE
	char					*NameA ;							// 名前
#endif
	wchar_t					*NameW ;

	int						MeshNum ;							// シェイプメッシュの数
	MV1_SHAPE_MESH_BASE		*Mesh ;								// シェイプメッシュ配列へのポインタ
} ;

// 物理演算用剛体データ構造体
struct MV1_PHYSICS_RIGIDBODY_BASE
{
	int						Index ;								// インデックス
#ifndef UNICODE
	char					*NameA ;							// 名前
#endif
	wchar_t					*NameW ;							// 名前

	struct MV1_FRAME_BASE	*TargetFrame ;						// 対象となるフレーム

	int						RigidBodyGroupIndex ;				// 剛体グループ番号
	DWORD					RigidBodyGroupTarget ;				// 剛体グループ対象
	int						ShapeType ;							// 形状( 0:球  1:箱  2:カプセル )
	float					ShapeW ;							// 幅
	float					ShapeH ;							// 高さ
	float					ShapeD ;							// 奥行
	VECTOR					Position ;							// 位置
	VECTOR					Rotation ;							// 回転( ラジアン )
	float					RigidBodyWeight ;					// 質量
	float					RigidBodyPosDim ;					// 移動減
	float					RigidBodyRotDim ;					// 回転減
	float					RigidBodyRecoil ;					// 反発力
	float					RigidBodyFriction ;					// 摩擦力
	int						RigidBodyType ;						// 剛体タイプ( 0:Bone追従  1:物理演算  2:物理演算(Bone位置合わせ) )
	int						NoCopyToBone ;						// ボーンの行列を物理に適用しないかどうか
} ;

// 物理演算用剛体ジョイントデータ構造体
struct MV1_PHYSICS_JOINT_BASE
{
	int						Index ;								// インデックス
#ifndef UNICODE
	char					*NameA ;							// 名前
#endif
	wchar_t					*NameW ;							// 名前

	MV1_PHYSICS_RIGIDBODY_BASE	*RigidBodyA ;					// 接続先剛体Ａ
	MV1_PHYSICS_RIGIDBODY_BASE	*RigidBodyB ;					// 接続先剛体Ｂ
	VECTOR					Position ;							// 位置
	VECTOR					Rotation ;							// 回転( ラジアン )
	VECTOR					ConstrainPosition1 ;				// 移動制限値１
	VECTOR					ConstrainPosition2 ;				// 移動制限値２
	VECTOR					ConstrainRotation1 ;				// 回転制限値１
	VECTOR					ConstrainRotation2 ;				// 回転制限値２
	VECTOR					SpringPosition ;					// ばね移動値
	VECTOR					SpringRotation ;					// ばね回転値
} ;

// モデル階層基データ構造体
struct MV1_FRAME_BASE
{
	struct MV1_MODEL_BASE	*Container ;						// このフレームを持っているモデルのポインタ

	int						CheckID ;							// チェックＩＤ
	int						CheckNo ;							// チェック番号

#ifndef UNICODE
	char					*NameA ;							// 名前
#endif
	wchar_t					*NameW ;
	int						Index ;								// インデックス

	MV1_CHANGE_BASE			ChangeDrawMaterialInfo ;			// 描画用マテリアル状態変化管理用基本情報
	MV1_CHANGE_BASE			ChangeMatrixInfo ;					// 行列状態変化管理用基本情報

	int						TotalMeshNum ;						// 自分の下層にあるメッシュの総数
	int						TotalChildNum ;						// 自分の下層にあるフレームの数
	MV1_FRAME_BASE			*Parent ;							// 親階層へのポインタ
	MV1_FRAME_BASE			*FirstChild ;						// 子階層へのポインタ(先端)
	MV1_FRAME_BASE			*LastChild ;						// 子階層へのポインタ(末端)
	MV1_FRAME_BASE			*Prev ;								// 兄階層へのポインタ
	MV1_FRAME_BASE			*Next ;								// 弟階層へのポインタ

	int						TransformMatrixType ;				// 変換行列のタイプ( 0:単位行列  1:平行移動のみ 2:それ以外 )
	MATRIX_4X4CT_F			TransformMatrix ;					// 初期状態のローカル→ワールド行列
	MATRIX_4X4CT_F			InverseTransformMatrix ;			// 初期状態のローカル→ワールド行列の逆行列

	int						LocalTransformMatrixType ;			// 変換行列のタイプ( 0:単位行列  1:平行移動のみ 2:それ以外 )
	MATRIX_4X4CT_F			LocalTransformMatrix ;				// 変換行列
	bool					LocalTransformMatrixUseScaling ;	// 変換行列でスケーリングを使用しているかどうかのフラグ( true:使用している  false:使用していない )

	VECTOR					Translate ;							// 平行移動
	VECTOR					Scale ;								// スケール
	int						RotateOrder ;						// 回転オーダー( MV1_ROTATE_ORDER_XYZ 等 )
	VECTOR					Rotate ;							// 回転(３軸)
	FLOAT4					Quaternion ;						// 回転( クォータニオン )
	int						IgnoreParentTransform ;				// 親の座標変換を無視するかどうか( 1:無視する  0:無視しない )

	VECTOR					PreRotate ;							// 前回転
	VECTOR					PostRotate ;						// 後回転

	VECTOR					MaxPosition ;						// このノードに含まれるメッシュの頂点座標の最大値( メッシュローカル座標 )
	VECTOR					MinPosition ;						// このノードに含まれるメッシュの頂点座標の最小値( メッシュローカル座標 )
	int						ValidMaxMinPosition ;				// MaxPosition, MinPosition が有効かどうか( 1:有効  0:無効 )

	DWORD					Flag ;								// 各種フラグ( MV1_FRAMEFLAG_VISIBLE など )

	int						IsSkinMesh ;						// このフレームに含まれるメッシュの中にスキンメッシュが含まれるかどうか( TRUE:含まれる  FALSE:含まれない )
	int						TriangleNum ;						// このフレームに含まれるポリゴンの数
	int						VertexNum ;							// このフレームに含まれる頂点データの数
	int						MeshNum ;							// メッシュの数
	MV1_MESH_BASE			*Mesh ;								// メッシュリストへのポインタ

	int						IsCombineMesh ;						// 自分を含め子のフレームに含まれるメッシュも纏めて一つに合成したメッシュが存在するかどうか( TRUE:存在する  FALSE:存在しない )
	MV1_MESH_BASE			*CombineMesh ;						// 自分を含め子のフレームに含まれるメッシュも纏めて一つに合成したメッシュへのポインタ

	int						SkinBoneNum ;						// このフレームをターゲットとするボーンの数
	MV1_SKIN_BONE			*SkinBone ;							// このフレームをターゲットとする最初のボーンへのポインタ

	int						UseSkinBoneNum ;					// このフレームが使用しているボーンの数
	MV1_SKIN_BONE			**UseSkinBone ;						// このフレームが使用しているボーンへのポインタの配列

	MV1_LIGHT				*Light ;							// ライト情報へのポインタ

	int						MaxBoneBlendNum ;					// 一つの座標データで使用するボーンウエイト情報の最大数
	float					SmoothingAngle ;					// 自動法線計算の場合のスムージングを行うかどうかの閾値( 単位はラジアン )
	int						AutoCreateNormal ;					// 法線の自動生成を使用するかどうか( TRUE:使用する  FALSE:使用しない )

	int						PositionNum ;						// 座標の数
	int						PosUnitSize ;						// 座標データ一つ辺りのサイズ
	MV1_MESH_POSITION		*Position ;							// 座標

	int						NormalNum ;							// 法線の数
	MV1_MESH_NORMAL			*Normal ;							// 法線
	int						NormalAllocMem ;					// 法線データ用にメモリを確保しているかどうか( TRUE:している  FALSE:していない )

	int						ShapeNum ;							// シェイプデータの数
	MV1_SHAPE_BASE			*Shape ;							// シェイプ配列へのポインタ

	MV1_PHYSICS_RIGIDBODY_BASE *PhysicsRigidBody ;				// 剛体情報

	DWORD					UserData[ 4 ] ;						// 外部定義の情報
} ;


// 頂点バッファ情報
struct MV1_VERTEXBUFFER
{
	int						VertexCount ;						// 頂点数
	int						IndexCount ;						// インデックス数
	void					*VertexBufferL ;					// 頂点バッファロック時にアドレスを格納するポインタ
	void					*IndexBufferL ;						// インデックスバッファロック時にアドレスを格納するポインタ
	DWORD					UnitSize ;							// 頂点データ一つ辺りのサイズ

	struct MV1_VERTEXBUFFER_PF	*PF ;							// 環境依存データ

	struct MV1_VERTEXBUFFER	*DataNext ;							// 次の頂点データへのポインタ
	struct MV1_VERTEXBUFFER	*DataPrev ;							// 前の頂点データへのポインタ
} ;

// モデル基データ構造体
struct MV1_MODEL_BASE
{
	HANDLEINFO				HandleInfo ;						// ハンドル共通データ

	void					*DataBuffer ;						// フレームやマテリアルやアニメーションなどを格納するバッファへのポインタ

	DWORD_PTR				AllocMemorySize ;					// メモリ確保に使用したサイズ

	int						UseNum ;							// この基データを使用しているモデルデータの数
	struct MV1_MODEL		*UseFirst, *UseLast ;				// この基データを使用しているモデルデータの先端と終端へのポインタ

	int						UsePackDraw ;						// 同時複数描画に対応するかどうか( TRUE:対応する  FLASE:対応しない )
	int						PackDrawMaxNum ;					// 同時に行える描画の最大数
	int						PackDrawMatrixUnitNum ;				// 同時複数描画の一描画分で使用する行列の数

	int						RightHandType ;						// 右手座標系かどうか( TRUE:右手座標系  FALSE:左手座標系 )
	int						Ignore32bitBmpAlpha ;				// 32bitＢＭＰのアルファ成分を無視するかどうか( TRUE:無視する FALSE:アルファ成分として扱う )
	int						MaterialNumberOrderDraw ;			// 割り当てられているマテリアルの番号が低いメッシュから描画するかどうか( 1:する  0:しない )

	wchar_t					*Name ;								// モデルの名前
	wchar_t					*FilePath ;							// モデルのファイルパス
	wchar_t					*DirectoryPath ;					// モデルのファイルが存在するディレクトリパス( 末端に \ 付き )
	int						RefCount ;							// 参照カウント

	int						ChangeDrawMaterialTableSize ;		// 描画マテリアルの変更情報管理用に必要なデータの総容量
	int						ChangeMatrixTableSize ;				// 行列の変更情報管理用に必要なデータの総容量
	DWORD					*ChangeDrawMaterialTable ;			// 描画用マテリアル変更確認用ビットデータ
	DWORD					*ChangeMatrixTable ;				// 行列変更確認用ビットデータ

	int						FrameNum ;							// 階層データの数
	MV1_FRAME_BASE			*Frame ;							// 階層データ配列へのポインタ
	int						TopFrameNum ;						// 最上位階層のフレームの数
	MV1_FRAME_BASE			*FirstTopFrame ;					// 最上位階層の最初のフレームへのポインタ
	MV1_FRAME_BASE			*LastTopFrame ;						// 最上位階層の最後のフレームへのポインタ
	int						FrameUseSkinBoneNum ;				// フレームが使用しているボーンの総数
	MV1_SKIN_BONE			**FrameUseSkinBone ;				// フレームが使用しているボーンへのポインタ

	int						MaterialNum ;						// マテリアルデータの数
	MV1_MATERIAL_BASE		*Material ;							// マテリアルデータ配列へのポインタ

	int						TextureNum ;						// テクスチャの数
	MV1_TEXTURE_BASE		*Texture ;							// テクスチャデータ配列へのポインタ
	int						TextureAllocMem ;					// テクスチャデータ用にメモリを確保しているかどうか( TRUE:している  FALSE:していない )

	int						MeshNum ;							// メッシュデータの数
	MV1_MESH_BASE			*Mesh ;								// メッシュデータ配列へのポインタ

	int						LightNum ;							// ライトの数
	MV1_LIGHT				*Light ;							// ライトデータ配列へのポインタ

	VECTOR					MaxPosition ;						// デフォルトポーズの頂点座標の最大値( モデルローカル座標 )
	VECTOR					MinPosition ;						// デフォルトポーズの頂点座標の最小値( モデルローカル座標 )
	int						ValidMaxMinPosition ;				// MaxPosition, MinPosition が有効かどうか( 1:有効  0:無効 )

	int						SkinBoneNum ;						// スキニングメッシュのボーンとして使用するフレームの情報の数
	MV1_SKIN_BONE			*SkinBone ;							// スキニングメッシュのボーンとして使用するフレームの情報
	int						SkinBoneUseFrameNum ;				// スキニングメッシュのボーンを使用するフレームの数
	MV1_SKIN_BONE_USE_FRAME	*SkinBoneUseFrame ;					// スキニングメッシュのボーンを使用するフレーム

	int						ShapeNum ;							// シェイプデータの数
	MV1_SHAPE_BASE			*Shape ;							// シェイプデータ配列へのポインタ
	int						ShapeMeshNum ;						// シェイプメッシュデータの数
	MV1_SHAPE_MESH_BASE		*ShapeMesh ;						// シェイプメッシュデータ配列へのポインタ
	int						ShapeVertexNum ;					// シェイプ頂点データの数
	MV1_SHAPE_VERTEX_BASE	*ShapeVertex ;						// シェイプ頂点データ配列へのポインタ
	int						ShapeTargetMeshVertexNum ;			// シェイプ対象のメッシュの頂点の総数

	int						ShapeNormalPositionNum ;			// シェイプ用トライアングルリストのボーン情報無し座標データの数
	int						ShapeSkinPosition4BNum ;			// シェイプ用トライアングルリストの４ボーンスキニングメッシュ座標データの数
	int						ShapeSkinPosition8BNum ;			// シェイプ用トライアングルリストの８ボーンスキニングメッシュ座標データの数
	int						ShapeSkinPositionFREEBSize ;		// シェイプ用トライアングルリストの９ボーン以上スキニングメッシュ座標データのサイズ

	int						PhysicsRigidBodyNum ;				// 物理演算用剛体データの数
	MV1_PHYSICS_RIGIDBODY_BASE *PhysicsRigidBody ;				// 物理演算用剛体データ配列へのポインタ
	int						PhysicsJointNum ;					// 物理演算用ジョイントデータの数
	MV1_PHYSICS_JOINT_BASE	*PhysicsJoint ;						// 物理演算用ジョイントデータ配列へのポインタ

	int						TriangleListNum ;					// トライアングルリストの数
	MV1_TRIANGLE_LIST_BASE	*TriangleList ;						// トライアングルリストデータ配列へのポインタ

	int						TriangleListMaxVertexNum ;			// 一番頂点数が多いトライアングルリストの頂点数
	int						TriangleListMinVertexNum ;			// 一番頂点数が少ないトライアングルリストの頂点数
	int						TriangleListMaxIndexNum ;			// 一番インデックス数が多いトライアングルリストのインデックス数
	int						TriangleListMinIndexNum ;			// 一番インデックス数が少ないトライアングルリストのインデックス数
	int						TriangleListMaxMatrixNum ;			// 一番使用している行列の数が多いトライアングルリストの行列数
	int						TriangleListMinMatrixNum ;			// 一番使用している行列の数が少ないトライアングルリストの行列数

	int						TriangleListNormalPositionNum ;		// トライアングルリストのボーン情報無し座標データの数
	int						TriangleListSkinPosition4BNum ;		// トライアングルリストの４ボーンスキニングメッシュ座標データの数
	int						TriangleListSkinPosition8BNum ;		// トライアングルリストの８ボーンスキニングメッシュ座標データの数
	int						TriangleListSkinPositionFREEBSize ;	// トライアングルリストの９ボーン以上スキニングメッシュ座標データのサイズ
	int						MeshPositionSize ;					// メッシュ座標のデータサイズ
	int						MeshNormalNum ;						// メッシュ法線の数
	int						MeshVertexSize ;					// メッシュ頂点情報のデータサイズ
	int						MeshFaceNum ;						// メッシュ面情報の数
	int						MeshVertexIndexNum ;				// 頂点インデックスデータの総数
	void					*VertexData ;						// 頂点データを格納するメモリ領域
	DWORD					VertexDataSize ;					// 頂点データのサイズ
	MV1_TLIST_NORMAL_POS	*TriangleListNormalPosition ;		// トライアングルリストのボーン情報無し座標データ
	MV1_TLIST_SKIN_POS_4B	*TriangleListSkinPosition4B ;		// トライアングルリストの４ボーンスキニングメッシュ座標データ
	MV1_TLIST_SKIN_POS_8B	*TriangleListSkinPosition8B ;		// トライアングルリストの８ボーンスキニングメッシュ座標データ
	MV1_TLIST_SKIN_POS_FREEB *TriangleListSkinPositionFREEB ;	// トライアングルリストの９ボーン以上スキニングメッシュ座標データ
	MV1_MESH_POSITION		*MeshPosition ;						// メッシュ座標データ
	MV1_MESH_NORMAL			*MeshNormal ;						// メッシュ法線データ
	MV1_MESH_VERTEX			*MeshVertex ;						// メッシュ頂点データ
	MV1_MESH_FACE			*MeshFace ;							// メッシュ面データ
	DWORD					*MeshVertexIndex ;					// メッシュ用頂点インデックスデータ

	float					PhysicsGravity ;					// 物理演算で使用する重力パラメータ

	int						TriangleListIndexNum ;				// トライアングルリストの頂点インデックスデータの総数
	WORD					*TriangleListIndex ;				// トライアングルリストの頂点インデックスへのポインタ

	int						TriangleNum ;						// 三角形の数
	int						TriangleListVertexNum ;				// トライアングルリストの頂点データの数

#ifndef UNICODE
	int						StringSizeA ;						// 文字列を保存するバッファのサイズ
	char					*StringBufferA ;					// 文字列を保存するバッファへのポインタ
#endif
	int						StringSizeW ;						// 文字列を保存するバッファのサイズ
	wchar_t					*StringBufferW ;					// 文字列を保存するバッファへのポインタ

	int						AnimKeyDataSize ;					// アニメーションキーデータのサイズ
	void					*AnimKeyData ;						// アニメーションキーデータのバッファ

	int						AnimKeySetNum ;						// アニメーションキーセットの数
	MV1_ANIM_KEYSET_BASE	*AnimKeySet ;						// アニメーションキーセットデータ配列へのポインタ

	int						AnimNum ;							// アニメーションの数
	MV1_ANIM_BASE			*Anim ;								// アニメーションデータ配列へのポインタ

	int						AnimSetNum ;						// アニメーションセットの数
	MV1_ANIMSET_BASE		*AnimSet ;							// アニメーションセットデータ配列へのポインタ

	MV1_ANIM_BASE			**AnimTargetFrameTable ;			// フレームの数×アニメーションセットの数だけ存在するフレームとアニメーションの対応テーブル

	int						TotalVertexBufferNum ;				// 頂点バッファの総数
	int						SetupVertexBuffer ;					// 頂点バッファのセットアップが完了しているかどうか( TRUE:している  FALSE:していない )
	int						VertexBufferNum[ 3 ][ 2 ][ 9 ] ;	// 各頂点バッファの数
	MV1_VERTEXBUFFER		*VertexBufferFirst[ 3 ][ 2 ][ 9 ] ;	// 最初の頂点バッファへのポインタ[ 0:１ボーン用  1:４ボーン用  2:８ボーン用 ][ 0:バンプマップ無し  1:バンプマップ有り ][ UV の数 ]
	MV1_VERTEXBUFFER		*VertexBufferLast[ 3 ][ 2 ][ 9 ] ;	// 末端の頂点バッファのポインタ[   0:１ボーン用  1:４ボーン用  2:８ボーン用 ][ 0:バンプマップ無し  1:バンプマップ有り ][ UV の数 ]
	int						ObjectDuplicateNum ;				// 希望するオブジェクトコピーの数

	MEMINFO					*AddFirstMem ;						// 内容変更を加える場合に新たに確保したメモリを記憶しておくためのモノ

	DWORD					UserData[ 4 ] ;						// 外部定義の情報
} ;





// アニメーションキーセット実行用構造体
struct MV1_ANIM_KEYSET
{
	MV1_ANIM_KEYSET_BASE	*BaseData ;							// キーセット基本データへのポインタ

	int						ShapeTargetIndex ;					// 対象のシェイプインデックス
	float					ShapeKeyFactor ;					// シェイプのブレンド率
	int						NowKey ;							// 現在処理しているキーのインデックス
} ;

// アニメーション実行用構造体
struct MV1_ANIM
{
	MV1_ANIM_BASE			*BaseData ;							// アニメーション基本データへのポインタ
	struct MV1_FRAME		*Frame ;							// アニメーションが担当しているフレーム

	int						ValidFlag ;							// 有効な要素のビットが立っているフラグ変数( MV1_ANIMVALUE_TRANSLATE 等 )
	VECTOR					Scale ;								// 拡大値
	VECTOR					Translate ;							// 平行移動値
	int						RotateOrder ;						// 回転オーダー( MV1_ROTATE_ORDER_XYZ 等 )
	VECTOR					Rotate ;							// 回転値
	FLOAT4					Quaternion ;						// 回転値( クォータニオン )
	MATRIX_4X4CT_F			Matrix ;							// 行列
	float					*ShapeRate ;						// シェイプのブレンド率

	bool					ValidBlendMatrix ;					// BlendMatrix が有効かどうか
	MATRIX_4X4CT_F			BlendMatrix ;						// キーセットの状態を反映した行列
	bool					BlendMatrixUseScaling ;				// キーセットの状態を反映した行列でスケーリングが使用されているかどうか( true:使用されている  false:使用されていない )
	MV1_ANIM_KEYSET			*KeySet ;							// キーセット配列へのポインタ
} ;

// アニメーションセット実行用構造体
struct MV1_ANIMSET
{
	MV1_ANIMSET_BASE		*BaseData ;							// アニメーションセット基本データへのポインタ

	bool					ParamSetup ;						// パラメータのセットアップが済んでいるかどうか( true:済んでいる  false:済んでいない )
	float					NowTime ;							// アニメーションの現在の再生時間
	MV1_ANIM				*Anim ;								// アニメーション配列へのポインタ
} ;

// 状態変更管理情報構造体
struct MV1_CHANGE
{
	MV1_CHANGE_BASE			*BaseData ;							// 状態変更管理情報の基本データへのポインタ
	DWORD					*Target ;							// 状態変化が発生した際に論理和するべき対象の開始メモリアドレス
} ;

// 描画用マテリアル情報構造体
struct MV1_DRAW_MATERIAL
{
	BYTE					Visible ;							// 表示フラグ( 1:表示する  0:表示しない )

	bool					UseColorScale ;						// Scale系パラメータで使用している項目があるかどうか( true:ある false:ない )
	COLOR_F					DiffuseScale ;						// ディフューズ色に乗算する値( 初期値は全要素 1.0f )
	COLOR_F					AmbientScale ;						// アンビエント色に乗算する値( 初期値は全要素 1.0f )
	COLOR_F					SpecularScale ;						// スペキュラ色に乗算する値( 初期値は全要素 1.0f )
	COLOR_F					EmissiveScale ;						// エミッシブ色に乗算する値( 初期値は全要素 1.0f )

	float					OpacityRate ;						// 不透明率( 初期値は 1.0f )
} ;

// テクスチャ構造体
struct MV1_TEXTURE
{
	MV1_TEXTURE_BASE		*BaseData ;							// テクスチャの基本データへのポインタ

#ifndef UNICODE
	char					*AlphaFilePathA_ ;					// アルファチャンネル用画像のファイルパス
#endif
	wchar_t					*AlphaFilePathW_ ;
	void					*AlphaImage ;						// アルファチャンネル画像のファイルイメージへのポインタ
	int						AlphaImageSize ;					// アルファチャンネル画像のファイルイメージのサイズ

#ifndef UNICODE
	char					*ColorFilePathA_ ;					// カラーチャンネル用画像のファイルパス
#endif
	wchar_t					*ColorFilePathW_ ;
	void					*ColorImage ;						// カラーチャンネル用画像のファイルイメージへのポインタ
	int						ColorImageSize ;					// カラーチャンネル用画像のファイルイメージのサイズ

	int						Width, Height ;						// 幅、高さ
	int						SemiTransFlag ;						// 半透明要素があるかどうか( TRUE:ある  FALSE:ない )
	int						BumpImageFlag ;						// 元がバンプマップ画像か( TRUE:バンプマップ  FALSE:違う )
	float					BumpImageNextPixelLength ;			// バンプマップ画像の場合の隣のピクセルとの距離

	int						UseGraphHandle ;					// GraphHandle を使用しているかどうか( TRUE:使用している  FALSE:していない )
	int						GraphHandle ;						// グラフィックハンドル
	int						IsDefaultTexture ;					// デフォルトの画像かどうか( TRUE:デフォルトの画像  FALSE:ファイルから読み込んだ画像 )

	int						UseUserGraphHandle ;				// UserGraphHandle を使用しているかどうか( TRUE:使用している  FALSE:していない )
	int						UserGraphHandle ;					// ユーザーグラフィックハンドル
	int						UserGraphWidth, UserGraphHeight ;	// 幅、高さ
	int						UserGraphHandleSemiTransFlag ;		// ユーザーグラフィックハンドルに半透明要素があるかどうか

	int						AddressModeU ;						// アドレスモード( DX_TEXADDRESS_WRAP 等 )
	int						AddressModeV ;						// アドレスモード( DX_TEXADDRESS_WRAP 等 )
	float					ScaleU ;							// Ｕ座標のスケーリング値
	float					ScaleV ;							// Ｖ座標のスケーリング値
	int						FilterMode ;						// フィルタリングモード( DX_DRAWMODE_BILINEAR 等 )

	int						ReverseFlag ;						// 画像を反転するかどうか( 1:反転する  0:反転しない )
	int						Bmp32AllZeroAlphaToXRGB8Flag ;		// ３２ビットＢＭＰのアルファ値が全部０だったら XRGB8 として扱うかどうかのフラグ
} ;

// マテリアルデータ構造体
struct MV1_MATERIAL
{
	MV1_MATERIAL_BASE		*BaseData ;							// マテリアルの基データへのポインタ

	COLOR_F					Diffuse ;							// ディフューズ色
	COLOR_F					Ambient ;							// アンビエント色
	COLOR_F					Specular ;							// スペキュラー色
	COLOR_F					Emissive ;							// エミッシブ色
	float					Power ;								// スペキュラハイライトのパワー
	float					TypeParam[ MATERIAL_TYPEPARAM_MAX_NUM ] ;	// マテリアルタイプのパラメータ( DX_MATERIAL_TYPE_MAT_SPEC_LUMINANCE_TWO_COLOR などで使用 )

	int						DiffuseLayerNum ;					// ディフューズカラーのレイヤー数
	MV1_MATERIAL_LAYER		DiffuseLayer[ 8 ] ;					// ディフューズカラーのレイヤー情報( ０レイヤーの BlendType は無視される )
	int						SpecularLayerNum ;					// スペキュラマップのレイヤー数
	MV1_MATERIAL_LAYER		SpecularLayer[ 8 ] ;				// スペキュラマップのレイヤー情報( ０レイヤーの BlendType は無視される )

	int						DiffuseGradTexture ;				// ( トゥーンレンダリングでのみ使用 )ディフューズカラーグラデーションテクスチャインデックス、当たっているライトのディフューズカラーでＵ値が決まるもの( MV1_MODEL_BASE.Textue 配列のインデックス )
	int						SpecularGradTexture ;				// ( トゥーンレンダリングでのみ使用 )スペキュラカラーグラデーションテクスチャインデックス、当たっているライトのスペキュラカラーでＵ値が決まるもの、無効の場合は−１( MV1_MODEL_BASE.Textue 配列のインデックス )
	int						SphereMapTexture ;					// ( トゥーンレンダリングでのみ使用 )スフィアマップテクスチャインデックス
	int						DiffuseGradBlendType ;				// ( トゥーンレンダリングでのみ使用 )ディフューズグラデーションテクスチャのブレンドタイプ( DX_MATERIAL_BLENDTYPE_TRANSLUCENT など )
	int						SpecularGradBlendType ;				// ( トゥーンレンダリングでのみ使用 )スペキュラグラデーションテクスチャのブレンドタイプ( DX_MATERIAL_BLENDTYPE_ADDITIVE など )
	int						SphereMapBlendType ;				// ( トゥーンレンダリングでのみ使用 )スフィアマップテクスチャのブレンドタイプ( DX_MATERIAL_BLENDTYPE_ADDITIVE など )
	float					OutLineWidth ;						// ( トゥーンレンダリングでのみ使用 )輪郭線の幅( 0.0f 〜 1.0f )
	float					OutLineDotWidth ;					// ( トゥーンレンダリングでのみ使用 )輪郭線のドット単位での幅
	COLOR_F					OutLineColor ;						// ( トゥーンレンダリングでのみ使用 )輪郭線の色

	int						UseAlphaTest ;						// アルファテストを使用するかどうか
	int						AlphaFunc ;							// アルファテストモード( 今のところ未使用 )
	int						AlphaRef ;							// アルファテストの閾値

	int						DrawBlendMode ;						// 出力時のブレンドモード( DX_BLENDMODE_NOBLEND 等 )
	int						DrawBlendParam ;					// 出力時のブレンドパラメータ
} ;

// トライアングルリスト構造体
struct MV1_TRIANGLE_LIST
{
	struct MV1_MESH				*Container ;					// このトライアングルリストを持っているメッシュへのポインタ
	MV1_TRIANGLE_LIST_BASE		*BaseData ;						// 基データへのポインタ

	union
	{
		MV1_TLIST_NORMAL_POS     *NormalPosition ;				// ボーン情報無し頂点座標データ
		MV1_TLIST_SKIN_POS_4B    *SkinPosition4B ;				// ４ボーンスキニング処理用頂点座標データ
		MV1_TLIST_SKIN_POS_8B    *SkinPosition8B ;				// ８ボーンスキニング処理用頂点座標データ
		MV1_TLIST_SKIN_POS_FREEB *SkinPositionFREEB ;			// ９ボーン以上のスキニング処理用頂点座標データ
	} ;

	struct MV1_TRIANGLE_LIST_PF	*PF ;							// 環境依存データ
} ;

// シェイプ頂点データ構造体
struct MV1_SHAPE_VERTEX
{
	VECTOR					Position ;							// 座標
	VECTOR					Normal ;							// 法線
} ;

// 物理演算用剛体データ構造体
struct MV1_PHYSICS_RIGIDBODY
{
	MV1_PHYSICS_RIGIDBODY_BASE *BaseData ;						// 基データへのポインタ
	struct MV1_FRAME		*TargetFrame ;						// 対象となるフレーム

	DWORD					BulletInfo[ 160 ] ;					// BULLET_RIGIDBODY_INFO を格納するバッファ
} ;

// 物理演算用剛体ジョイントデータ構造体
struct MV1_PHYSICS_JOINT
{
	MV1_PHYSICS_JOINT_BASE	*BaseData ;							// 基データへのポインタ

	DWORD					BulletInfo[ 16 ] ;					// BULLET_JOINT_INFO を格納するバッファ
} ;

// メッシュデータ構造体
struct MV1_MESH
{
	struct MV1_FRAME		*Container ;						// このメッシュを持っているフレームへのポインタ
	MV1_MESH_BASE			*BaseData ;							// 基データへのポインタ

	MV1_MATERIAL			*Material ;							// マテリアルへのポインタ
	MV1_CHANGE				DrawMaterialChange ;				// 状態変化チェック用情報
	MV1_DRAW_MATERIAL		DrawMaterial ;						// 描画用マテリアル情報
	MV1_DRAW_MATERIAL		SetupDrawMaterial ;					// 上位フレームの情報も考慮した描画用マテリアル情報
	bool					SemiTransStateSetupFlag ;			// SemiTransState のセットアップが済んでいるか( true:済んでいる  false:済んでいない )
	bool					SemiTransState ;					// このメッシュに半透明要素があるかどうか( true:ある  false:ない )

	MV1_SHAPE_VERTEX		*ShapeVertex ;						// シェイプ処理した頂点データ( BaseData->Shape が 1 の場合のみ使用 )
	MV1_TRIANGLE_LIST		*TriangleList ;						// トライアングルリストへのポインタ

	int						DrawBlendMode ;						// 描画ブレンドモード( -1:の場合はマテリアルのデフォルト値 )
	int						DrawBlendParam ;					// 描画ブレンドパラメータ
} ;

// シェイプデータ構造体
struct MV1_SHAPE
{
	struct MV1_FRAME		*Container ;						// このシェイプを持っているフレームへのポインタ
	MV1_SHAPE_BASE			*BaseData ;							// 基データへのポインタ

	int						ShapeRateApplyType ;				// 影響率適用タイプ( DX_MV1_SHAPERATE_ADD など )
	float					ShapeRate ;							// 影響率
} ;

// モデル階層構造体
struct MV1_FRAME
{
	int						CheckID ;							// チェックＩＤ( "MFRM" )

	int						ModelHandle ;						// このフレームを持っているモデルのハンドル
	struct MV1_MODEL		*Container ;						// このフレームを持っているモデルへのポインタ

	int						ChildIndex ;						// 親の何番目の子か
	MV1_FRAME_BASE			*BaseData ;							// 階層の基本データへのポインタ

	struct MV1_FRAME		*Parent ;							// 親階層へのポインタ
	struct MV1_FRAME		*Child ;							// 子階層へのポインタ
	struct MV1_FRAME		*Prev ;								// 兄階層へのポインタ
	struct MV1_FRAME		*Next ;								// 弟階層へのポインタ
	int						ChildNum ;							// 子の数
	struct MV1_FRAME		**ChildList ;						// 子リストへのポインタ

	bool					ValidUserLocalTransformMatrix ;		// 外部指定のローカル行列が有効かどうか( true:有効  false:無効 )
	MATRIX_4X4CT			UserLocalTransformMatrix ;			// 外部指定のローカル行列
	bool					ValidUserLocalWorldTransformMatrix ;// 外部指定のローカル→ワールド行列が有効かどうか( true:有効  false:無効 )
	MATRIX_4X4CT			UserLocalWorldTransformMatrix ;		// 外部指定のローカル→ワールド行列
	MV1_CHANGE				LocalWorldMatrixChange ;			// 行列の状態変化チェック用情報
	MATRIX_4X4CT			LocalWorldMatrix ;					// ローカル→ワールド行列
	bool					LocalWorldMatrixUseScaling ;		// ローカル→ワールド行列でスケーリングが使用されているかどうか( true:使用されている  false:使用されていない )
	bool					ValidLocalWorldMatrixNM ;			// LocalWorldMatrixNM が有効かどうか( true:有効  false:無効 )
	MATRIX					LocalWorldMatrixNM ;				// ローカル→ワールド行列( 固定機能シェーダー用 )

	int						TextureAddressTransformUse ;		// テクスチャ座標変換行列を使用するかどうか( TRUE:使用する  FALSE:使用しない )
	MATRIX					TextureAddressTransformMatrix ;		// テクスチャ座標変換行列

	MATRIX_4X4CT			**UseSkinBoneMatrix ;				// 使用しているボーンの行列へのポインタの配列

//	bool					SetupTransformPolygon ;				// 頂点変換ありポリゴンリストのセットアップが完了しているかどうか( true:完了している  false:完了していない )
//	bool					SetupNonTransformPolygon ;			// 頂点変換なしポリゴンリストのセットアップが完了しているかどうか( true:完了している  false:完了していない )
//	MV1_REF_POLYGONLIST		*TransformPolygon ;					// 頂点変換ありポリゴンリスト
//	MV1_REF_POLYGONLIST		*NonTransformPolygon ;				// 頂点変換なしポリゴンリスト

	bool					SetupRefPolygon[ 2 ][ 2 ] ;			// 参照用ポリゴンリストのセットアップが完了しているかどうか( true:完了している  false:完了していない )[ 座標変換ありかどうか( 1:あり 0:なし ) ][ 座標のみかどうか( 1:座標のみ 0:全要素あり ) ]
	MV1_REF_POLYGONLIST		*RefPolygon[ 2 ][ 2 ] ;				// 参照用ポリゴンリスト( [ 座標変換ありかどうか( 1:あり 0:なし ) ][ 座標のみかどうか( 1:座標のみ 0:全要素あり ) ] )

	bool					SetupCollision ;					// コリジョン情報のセットアップが完了しているかどうか( true:完了している  false:完了していない )
	MV1_COLLISION			*Collision ;						// コリジョン情報

	MV1_MESH				*Mesh ;								// このフレームに属するメッシュへのポインタ( メッシュの数は BaseData->MeshNum )

	MV1_SHAPE				*Shape ;							// このフレームに属するシェイプへのポインタ( シェイプの数は BaseData->ShapeNum )

	bool					ShapeChangeFlag ;					// シェイプの情報が変化したか、フラグ( true:変化した  false:変化していない )

	MV1_PHYSICS_RIGIDBODY	*PhysicsRigidBody ;					// 剛体情報

	MV1_CHANGE				DrawMaterialChange ;				// 描画用マテリアルの状態変化チェック用情報
	MV1_DRAW_MATERIAL		DrawMaterial ;						// 描画用マテリアル情報
	MV1_DRAW_MATERIAL		SetupDrawMaterial ;					// 上位フレームの情報も考慮した描画用マテリアル情報
	bool					SemiTransStateSetupFlag ;			// SemiTransState のセットアップが済んでいるか( true:済んでいる  false:済んでいない )
	bool					SemiTransState ;					// このフレームに半透明要素があるかどうか( true:ある  false:ない )
} ;

// モデルアニメーション情報構造体
struct MV1_MODEL_ANIM
{
	bool					Use ;								// この構造体が有効かどうか( true :有効  false:無効 )
	float					BlendRate ;							// ブレンド率
	MV1_ANIM				*Anim ;								// アニメーション情報へのポインタ
} ;

// モデルアニメーションセット情報構造体
struct MV1_MODEL_ANIMSET
{
	bool					Use ;								// この構造体が有効かどうか( true :有効  false:無効 )
	bool					DisableShapeFlag ;					// シェイプアニメーションを無効にするかどうか
	int						BaseDataHandle ;					// モデル基本データハンドル
	int						BaseDataAnimIndex ;					// モデル基本データ中のアニメーションデータのインデックス
	int						UseAnimNum ;						// 適合しているアニメーションの数
	MV1_ANIMSET				*AnimSet ;							// アニメーションセット情報へのポインタ
} ;

// モデル実行用構造体
struct MV1_MODEL
{
	HANDLEINFO				HandleInfo ;						// ハンドル共通データ

	void					*DataBuffer ;						// フレームやマテリアルやアニメーションなどを格納するバッファへのポインタ

	MV1_MODEL				*UseBaseDataNext ;					// 同じモデル基本データを使用している次のモデルデータへのポインタ
	MV1_MODEL				*UseBaseDataPrev ;					// 同じモデル基本データを使用している前のモデルデータへのポインタ

	int						BaseDataHandle ;					// モデルの基本データのハンドル
	MV1_MODEL_BASE			*BaseData ;							// モデル基本データへのポインタ

	DWORD					*ChangeDrawMaterialFlag ;			// 描画用マテリアル変更確認用ビットデータ
	DWORD					*ChangeMatrixFlag ;					// 行列変更確認用ビットデータ
	DWORD					ChangeDrawMaterialFlagSize ;		// 描画用マテリアル変更確認用ビットデータのサイズ( バイト単位 )
	DWORD					ChangeMatrixFlagSize ;				// 行列変更確認用ビットデータのサイズ( バイト単位 )

	VECTOR_D				Translation ;						// 平行移動値
	MV1_ROTATE				Rotation ;							// 回転値
	VECTOR					Scale ;								// 拡大値
	MATRIX_4X4CT			Matrix ;							// 行列
	bool					ValidMatrix ;						// 行列に有効な値が入っているか( true:入っている  false:入っていない )
	bool					NotUseFrameRotation ;				// フレームの回転要素を使用しないかどうか( PreRotate と PostRotate は使用する )( true:使用しない  false:使用する )

	bool					LocalWorldMatrixSetupFlag ;			// モデルとモデル中のフレームの行列のセットアップが完了しているかどうか( true:完了している  false:していない )
	MATRIX_4X4CT			LocalWorldMatrix ;					// ローカル→ワールド行列
	bool					LocalWorldMatrixUseScaling ;		// ローカル→ワールド行列でスケーリングを使用しているかどうか( true:使用している  false:使用していない )

	MV1_FRAME				*Frame ;							// フレーム情報へのポインタ
	int						TopFrameNum ;						// 最上位フレームの数
	MV1_FRAME				**TopFrameList ;					// 最上位フレームリストへのポインタ
	MATRIX_4X4CT			**SkinBoneUseFrameMatrix ;			// スキニングメッシュが使用しているボーンの行列へのポインタの配列

	MATRIX_4X4CT			*SkinBoneMatrix ;					// BaseData->SkinBone->ModelLocalMatrix と BaseData->SkinBone->Frame が示すフレームの LocalWorldMatrix を乗算したものの配列

	int						PackDrawStockNum ;					// 同時複数描画用に溜まっている描画情報の数
	MATRIX_4X4CT_F			*PackDrawMatrix ;					// 同時複数描画で使用する行列の配列

	MV1_SHAPE_VERTEX		*ShapeVertex ;						// シェイプ頂点情報へのポインタ
	bool					ShapeChangeFlag ;					// シェイプの情報が変化したか、フラグ( true:変化した  false:変化していない )
	bool					ShapeDisableFlag ;					// シェイプを使用しないかどうかのフラグ( true:使用しない  false:使用する )

	MV1_PHYSICS_RIGIDBODY	*PhysicsRigidBody ;					// 剛体情報へのポインタ
	MV1_PHYSICS_JOINT		*PhysicsJoint ;						// ジョイント情報へのポインタ
	int						PhysicsResetRequestFlag ;			// リセットリクエストフラグ

	MV1_TRIANGLE_LIST		*TriangleList ;						// トライアングルリスト情報へのポインタ
	MV1_MESH				*Mesh ;								// メッシュ情報へのポインタ
	MV1_SHAPE				*Shape ;							// シェイプ情報へのポインタ
	MV1_MATERIAL			*Material ;							// マテリアル情報へのポインタ
	MV1_TEXTURE				*Texture ;							// テクスチャ情報へのポインタ
	int						TextureAllocMem ;					// テクスチャデータ用にメモリを確保しているかどうか( TRUE:している  FALSE:していない )

	MV1_TLIST_NORMAL_POS	*ShapeNormalPosition ;				// シェイプ用トライアングルリストのボーン情報無し座標データ
	MV1_TLIST_SKIN_POS_4B	*ShapeSkinPosition4B ;				// シェイプ用トライアングルリストの４ボーンスキニングメッシュ座標データ
	MV1_TLIST_SKIN_POS_8B	*ShapeSkinPosition8B ;				// シェイプ用トライアングルリストの８ボーンスキニングメッシュ座標データ
	MV1_TLIST_SKIN_POS_FREEB *ShapeSkinPositionFREEB ;			// シェイプ用トライアングルリストの９ボーン以上スキニングメッシュ座標データ

	bool					SetupRefPolygon[ 2 ][ 2 ] ;			// 参照用ポリゴンリストのセットアップが完了しているかどうか( true:完了している  false:完了していない )[ 座標変換ありかどうか( 1:あり 0:なし ) ][ 座標のみかどうか( 1:座標のみ 0:全要素あり ) ]
	MV1_REF_POLYGONLIST		*RefPolygon[ 2 ][ 2 ] ;				// 参照用ポリゴンリスト( [ 座標変換ありかどうか( 1:あり 0:なし ) ][ 座標のみかどうか( 1:座標のみ 0:全要素あり ) ] )

//	bool					SetupTransformPolygon ;				// 頂点変換ありポリゴンリストのセットアップが完了しているかどうか( true:完了している  false:完了していない )
//	bool					SetupNonTransformPolygon ;			// 頂点変換なしポリゴンリストのセットアップが完了しているかどうか( true:完了している  false:完了していない )
//	MV1_REF_POLYGONLIST		*TransformPolygon ;					// 頂点変換ありポリゴンリスト
//	MV1_REF_POLYGONLIST		*NonTransformPolygon ;				// 頂点変換なしポリゴンリスト

	bool					SetupCollision ;					// コリジョン情報のセットアップが完了しているかどうか( true:完了している  false:完了していない )
	MV1_COLLISION			*Collision ;						// コリジョン情報

	int						AnimSetNum ;						// セットされているアニメーションセットの数
	int						AnimSetMaxNum ;						// 現在セットできるアニメーションセットの最大数
	MV1_MODEL_ANIMSET		*AnimSet ;							// アニメーションセット情報へのポインタ配列
	MV1_MODEL_ANIM			*Anim ;								// アニメーション情報へのポインタ配列
	bool					AnimSetupFlag ;						// アニメーションのパラメータを元にした行列のセットアップが完了しているかどうか( true:完了している  false:完了していない )

	MV1_DRAW_MATERIAL		DrawMaterial ;						// 描画用マテリアル情報
	bool					SemiTransStateSetupFlag ;			// SemiTransState のセットアップが済んでいるか( true:済んでいる  false:済んでいない )
	bool					SemiTransState ;					// このモデルに半透明要素があるかどうか( true:ある  false:ない )
	bool					UseDrawMulAlphaColor ;				// アルファ値と乗算したカラーで描画するかどうか( true:乗算する  false:乗算しない )

	int						MeshCategoryHide[ DX_MV1_MESHCATEGORY_NUM ] ;	// カテゴリ別のメッシュ非表示フラグ
	int						EnableZBufferFlag ;					// Ｚバッファの有効フラグ
	int						WriteZBufferFlag ;					// Ｚバッファの更新を行うか、フラグ
	int						ZBufferCmpType ;					// Ｚ値の比較モード
	int						ZBias ;								// Ｚバイアス

	bool					WireFrame ;							// ワイヤーフレーム描画をするかどうかのフラグ

	int						MaxAnisotropy ;						// 異方性フィルタリングの最大次数

	float					PhysicsGravity ;					// 物理演算で使用する重力パラメータ
	DWORD					BulletPhysicsDataBuffer[ 32 ] ;		// BULLET_PHYSICS を格納するバッファ
} ;







// モデルデータ管理用構造体
struct MV1_MODEL_MANAGE
{
	bool					Initialize ;							// 初期化済みか、フラグ( true:初期化済み  false:未初期化 )

	int						UseOrigShaderFlag ;						// モデルの描画に SetUseVertexShader, SetUsePixelShader で指定したシェーダーを使用するかどうか( TRUE:使用する  FALSE:使用しない( デフォルト ) )
	int						SemiTransDrawMode ;						// モデルの半透明要素がある部分についての描画モード( DX_SEMITRANSDRAWMODE_ALWAYS 等 )

	int						LoadModelToReMakeNormal ;				// モデルの読み込み処理で法線の再計算を行うかどうか( TRUE:行う  FALSE:行わない )
	float					LoadModelToReMakeNormalSmoothingAngle ;	// モデルの読み込み処理時に行う法泉の再計算で使用するスムージング角度( 単位はラジアン )
	int						LoadModelToIgnoreScaling ;				// モデルを読み込む際にスケーリングデータを無視するかどうか( TRUE:無視する  FALSE:無視しない )
	int						LoadModelToPositionOptimize ;			// モデルの読み込み処理で座標の最適化処理を行うかどうか( TRUE:行う  FALSE:行わない )
	int						LoadModelToNotEqNormalSideAddZeroAreaPolygon ;	// モデルを読み込む際にポリゴンの辺が接していて、且つ法線の方向が異なる辺に面積０のポリゴンを埋め込むかどうか( TRUE:埋め込む　FALSE:埋め込まない( デフォルト ) )
	int						LoadModelToUsePhysicsMode ;				// 読み込むモデルの物理演算モードを設定する( DX_LOADMODEL_PHYSICS_LOADCALC 等 )
	int						LoadModelToWorldGravityInitialize ;		// 読み込むモデルの物理演算に適用する重力パラメータが初期化されたかどうか
	float					LoadModelToWorldGravity ;				// 読み込むモデルの物理演算に適用する重力パラメータ
	int						LoadModelToPhysicsCalcPrecision ;		// 読み込むモデルの物理演算モードが事前計算( DX_LOADMODEL_PHYSICS_LOADCALC )だった場合に適用される物理演算の時間進行の精度( 0:60FPS  1:120FPS  2:240FPS  3:480FPS  4:960FPS  5:1920FPS )
	int						LoadModelToPMD_PMX_AnimationFPSMode ;	// PMD, PMX を読み込む際のアニメーションの FPS モード( DX_LOADMODEL_PMD_PMX_ANIMATION_FPSMODE_30 等 )
	int						LoadModelToDisablePhysicsNameWordNum ;	// 読み込むモデルの物理演算を無効にするワードの数
	wchar_t					LoadModelToDisablePhysicsNameWord[ MV1_DISABLEPHYSICS_WORD_MAXNUM ][ MV1_DISABLEPHYSICS_WORD_MAXLENGTH + 1 ] ;	// 読み込むモデルの物理演算を特定のワードが含まれる剛体に対してのみ無効にする
	char					LoadModelToDisablePhysicsNameWord_ShiftJIS[ MV1_DISABLEPHYSICS_WORD_MAXNUM ][ MV1_DISABLEPHYSICS_WORD_MAXLENGTH + 1 ] ;	// 読み込むモデルの物理演算を特定のワードが含まれる剛体に対してのみ無効にする( シフトJISコード )
	int						LoadModelToDisablePhysicsNameWordMode ;	// LoadModelToDisablePhysicsNameWord の適用ルール( DX_LOADMODEL_PHYSICS_DISABLENAMEWORD_ALWAYS 等 )
	int						LoadModelToUsePackDraw ;				// 読み込むモデルを一度に複数の描画に対応させるかどうか( TRUE:対応させる  FALSE:対応させない )
	int						LoadModelToTriangleListUseMaxBoneNum ;	// 読み込むモデルのひとつのトライアングルリストで使用できる最大ボーン数
	VECTOR					LoadCalcPhysicsWorldGravity[ MV1_LOADCALC_PHYSICS_GRAVITY_NUM ] ;	// 読み込むモデルの事前計算に使用する重力

	int						AnimFilePathValid ;					// AnimFilePath が有効かどうか( TRUE:有効  FALSE:無効 )
	wchar_t					AnimFileName[ 512 ] ;				// アニメーションファイルの名前
	wchar_t					AnimFileDirPath[ 512 ] ;			// アニメーションファイルのディレクトリパス

	int						ModelBaseNum ;						// モデル基本データ数
	int						ModelBaseMaxNum ;					// モデル基本データの現在の最大数
	int						ModelBaseNextIndex ;				// 次のモデル基本データの格納を試みるインデックス
	MV1_MODEL_BASE			**ModelBase ;						// モデル基本データポインタの配列へのポインタ

	int						ModelNum ;							// モデルのデー多数
	int						ModelMaxNum ;						// モデルの現在の最大数
	int						ModelNextIndex ;					// 次のモデルデータの格納を試みるインデックス
	int						ModelNextCheckNumber ;				// 次のモデルデータに振る番号
	MV1_MODEL				**Model ;							// モデルポインタの配列へのポインタ

	int						( *AddLoadFunc[ MV1_ADD_LOAD_FUNC_MAXNUM ] )( const struct MV1_MODEL_LOAD_PARAM *LoadParam ) ;		// 追加モデルロード関数
	int						AddLoadFuncNum ;					// 追加モデルロード関数の数

	MV1_MESH				**DrawMeshList ;					// 描画するメッシュのアドレス配列を格納するメモリ領域
	int						DrawMeshListSize ;					// 描画するメッシュのアドレス配列を格納するメモリ領域のサイズ( 個数単位 )

	int						TexNoneHandle ;						// テクスチャが無いモデルに貼るテクスチャ
	int						TexNoneBlackHandle ;				// テクスチャが無いモデルに貼るテクスチャ( 黒 )
	int						ToonDefaultGradTexHandle[ 2 ] ;		// トゥーン用グラデーションテクスチャが無いモデルに張るテクスチャ

	int						WorldMatrixIsIdentity ;				// ワールド行列に単位行列がセットされているかどうか( TRUE:されている  FALSE:されていない )
//	MATRIX					OrigLocalWorldMatrix ;				// もともと設定されていたＬＷ行列

	MV1_MODEL				*PackDrawModel ;					// 同時複数描画の為に描画待機をしているモデルのアドレス

	// テスト用シェーダー
//	D_IDirect3DPixelShader9  *PS_Test ;
//	D_IDirect3DVertexShader9 *VS_Test ;

//	int						UseBaseVertexShaderIndex ;			// 使用する頂点シェーダーのライト・フォグ・フォンシェーディングの有無のみ設定した値
//	int						UseBasePixelShaderIndex ;			// 使用するピクセルシェーダーのライト・フォンシェーディングの有無のみ設定した値

//	MV1_MATERIAL			*SetMaterial ;						// セットされているマテリアル
} ;

// 独自ファイル読み込み関数の変数をまとめた構造体
struct MV1_FILE_READ_FUNC
{
	int						( *Read )( const TCHAR *FilePath, void **FileImageAddr, int *FileSize, void *FileReadFuncData ) ;	// 独自ファイル読み込み関数
	int						( *Release )( void *FileImageAddr, void *FileReadFuncData ) ;										// 独自ファイル解放関数
	void					*Data ;								// 独自ファイル読み込み関数に渡すデータ
} ;

// ファイルからモデルハンドルを作成する処理に必要なグローバルデータを纏めたもの
struct MV1LOADMODEL_GPARAM
{
	LOADGRAPH_GPARAM		LoadGraphGParam ;

	int						LoadModelToReMakeNormal ;				// モデルの読み込み処理で法線の再計算を行うかどうか( TRUE:行う  FALSE:行わない )
	float					LoadModelToReMakeNormalSmoothingAngle ;	// モデルの読み込み処理時に行う法泉の再計算で使用するスムージング角度( 単位はラジアン )
	int						LoadModelToIgnoreScaling ;				// モデルを読み込む際にスケーリングデータを無視するかどうか( TRUE:無視する  FALSE:無視しない )
	int						LoadModelToPositionOptimize ;			// モデルの読み込み処理で座標の最適化処理を行うかどうか( TRUE:行う  FALSE:行わない )
	int						LoadModelToNotEqNormalSideAddZeroAreaPolygon ;	// モデルを読み込む際にポリゴンの辺が接していて、且つ法線の方向が異なる辺に面積０のポリゴンを埋め込むかどうか( TRUE:埋め込む　FALSE:埋め込まない( デフォルト ) )
	int						LoadModelToUsePhysicsMode ;				// 読み込むモデルの物理演算モードを設定する( DX_LOADMODEL_PHYSICS_LOADCALC 等 )
	int						LoadModelToWorldGravityInitialize ;		// 読み込むモデルの物理演算に適用する重力パラメータが初期化されたかどうか
	float					LoadModelToWorldGravity ;				// 読み込むモデルの物理演算に適用する重力パラメータ
	int						LoadModelToPhysicsCalcPrecision ;		// 読み込むモデルの物理演算モードが事前計算( DX_LOADMODEL_PHYSICS_LOADCALC )だった場合に適用される物理演算の時間進行の精度( 0:60FPS  1:120FPS  2:240FPS  3:480FPS  4:960FPS  5:1920FPS )
	int						LoadModelToPMD_PMX_AnimationFPSMode ;	// PMD, PMX を読み込む際のアニメーションの FPS モード( DX_LOADMODEL_PMD_PMX_ANIMATION_FPSMODE_30 等 )
	int						LoadModelToUsePackDraw ;				// 読み込むモデルを一度に複数の描画に対応させるかどうか( TRUE:対応させる  FALSE:対応させない )
	int						LoadModelToTriangleListUseMaxBoneNum ;	// 読み込むモデルのひとつのトライアングルリストで使用できる最大ボーン数
	VECTOR					LoadCalcPhysicsWorldGravity[ MV1_LOADCALC_PHYSICS_GRAVITY_NUM ] ;	// 読み込むモデルの事前計算に使用する重力

	int						AnimFilePathValid ;					// AnimFilePath が有効かどうか( TRUE:有効  FALSE:無効 )
	wchar_t					AnimFileName[ 512 ] ;				// アニメーションファイルの名前
	wchar_t					AnimFileDirPath[ 512 ] ;			// アニメーションファイルのディレクトリパス
} ;

// モデルデータ読み込み処理関数の引数をまとめた構造体
struct MV1_MODEL_LOAD_PARAM
{
	MV1LOADMODEL_GPARAM		GParam ;							// グローバルパラメータ

	void					*DataBuffer ;						// モデルファイルイメージ
	int						DataSize ;							// モデルファイルサイズ
	const wchar_t			*FilePath ;							// ファイルパス
	const wchar_t			*Name ;								// モデル名
	const wchar_t			*CurrentDir ;						// カレントディレクトリ

	MV1_FILE_READ_FUNC		*FileReadFunc ;						// 独自ファイル読み込み関数の変数をまとめた構造体へのポインタ
	int						ASyncThread ;						// 非同期読み込みスレッドでの読み込みかどうか( TRUE:非同期読み込みスレッドでの読み込み処理  FALSE:同期読み込み )
} ;

// データ宣言 -----------------------------------

extern MV1_MODEL_MANAGE MV1Man ;

// 関数プロトタイプ宣言 -------------------------

// メモリ管理系
#if !defined( __BCC ) || defined( _DEBUG )
	extern	void		*AddMemArea( size_t Size, MEMINFO **FirstMem, const char *FileName = NULL, int Line = 0 ) ;// メモリ領域の確保
#else
	extern	void		*AddMemArea( size_t Size, MEMINFO **FirstMem ) ;											// メモリ領域の確保
#endif
extern	int				SubMemArea( MEMINFO **FirstMem, void *Buffer ) ;										// メモリ領域の解放
extern	int				ClearMemArea( MEMINFO **FirstMem ) ;													// 全てのメモリ領域の解放


// ビットデータ関係
extern	int				InitBitList( BITLIST *BitList, int BitDepth, int DataNum, MEMINFO **FirstMem ) ;			// ビットデータリストを初期化する( -1:失敗  0:成功 )
extern	int				SetBitList( BITLIST *BitList, int Index, int SetBitIndex ) ;								// ビットデータリスト中の指定の要素のビットを立てる
extern	int				PressBitList( BITLIST *BitListDest, BITLIST *BitListSrc, MEMINFO **FirstMem ) ;				// ビットデータリスト中の値の種類を抽出する( 出力側は初期化されている必要は無い )( -1:失敗  0:成功 )
extern	int				SearchBitList( BITLIST *BitListTarget, void *Buffer ) ;										// 指定のビットデータに一致するターゲット側のデータインデックスを得る( -1:エラー、又は無かった  -1以外:データインデックス )
extern	int				SearchBitList( BITLIST *BitListTarget, BITLIST *BitListSrc, int SrcIndex ) ;				// ソース側の指定のビットデータに一致するターゲット側のデータインデックスを得る( -1:エラー、又は無かった  -1以外:データインデックス )
extern	int				CmpBitList( BITLIST *BitList1, int Index1, BITLIST *BitList2, int Index2 ) ;				// 二つの要素が一致しているかどうかを取得する( 0:一致している  0:一致していない )
extern	int				GetBitList( BITLIST *BitList, int Index, void *Buffer ) ;									// ビットデータリストから指定番号のデータを取得する
extern	int				CopyBitList( BITLIST *BitListDest, int Index, void *Buffer ) ;								// 指定のビットデータを指定のインデックスにコピーする
extern	int				OrBitList( BITLIST *BitList, int Index, void *Buffer ) ;									// 任意のビットデータとビットデータリスト中の指定のデータを Or 演算する
extern	int				GetBitCount( void *Buffer, int UnitSize ) ;													// 指定のビットデータ中、何ビット立っているか調べる
extern	int				AddBitList( BITLIST *BitListDest, void *Buffer, int RefCount = 1 ) ;						// 指定のビットデータを、ターゲット側のビットデータに加える
extern	int				AddBitList( BITLIST *BitListDest, BITLIST *BitListSrc, int SrcIndex, int RefCount=1 ) ;		// ソース側の指定のビットデータを、ターゲット側のビットデータに加える
extern	int				GetBitListNumber( BITLIST *BitList, int Index, WORD *Buffer ) ;								// 指定のビットデータ中で立っているビットのリストを取得する( 戻り値  -1:エラー  0以上:立っているビットの数 )

// 16bit補助情報関係
extern	BYTE			MV1AnimKey16BitMinFtoB( float Min ) ;													// float型の値から MV1_ANIM_KEY_16BIT構造体の Min 変数用の値を作成する
extern	BYTE			MV1AnimKey16BitUnitFtoB( float Unit ) ;													// float型の値から MV1_ANIM_KEY_16BIT構造体の Unit 変数用の値を作成する
extern	float			MV1AnimKey16BitMinBtoF( BYTE Min ) ;													// MV1_ANIM_KEY_16BIT構造体の Min 変数の値から float型の値を作成する
extern	float			MV1AnimKey16BitUnitBtoF( BYTE Unit ) ;													// MV1_ANIM_KEY_16BIT構造体の Unit 変数の値から float型の値を作成する

// 補助演算関係
extern	void			ConvertMatrixFToMatrix4x4cF(     MATRIX_4X4CT_F *Out, const MATRIX         *In ) ;		// MATRIX         構造体を MATRIX_4X4CT_F 構造体に変換する
extern	void			ConvertMatrixDToMatrix4x4cD(     MATRIX_4X4CT_D *Out, const MATRIX_D       *In ) ;		// MATRIX_D       構造体を MATRIX_4X4CT_D 構造体に変換する
extern	void			ConvertMatrixDToMatrix4x4cF(     MATRIX_4X4CT_F *Out, const MATRIX_D       *In ) ;		// MATRIX_D       構造体を MATRIX_4X4CT_F 構造体に変換する
extern	void			ConvertMatrixFToMatrix4x4cD(     MATRIX_4X4CT_D *Out, const MATRIX         *In ) ;		// MATRIX         構造体を MATRIX_4X4CT_D 構造体に変換する
extern	void			ConvertMatrix4x4cFToMatrixF(     MATRIX         *Out, const MATRIX_4X4CT_F *In ) ;		// MATRIX_4X4CT_F 構造体を MATRIX         構造体に変換する
extern	void			ConvertMatrix4x4cDToMatrixD(     MATRIX_D       *Out, const MATRIX_4X4CT_D *In ) ;		// MATRIX_4X4CT_D 構造体を MATRIX_D       構造体に変換する
extern	void			ConvertMatrix4x4cDToMatrixF(     MATRIX         *Out, const MATRIX_4X4CT_D *In ) ;		// MATRIX_4X4CT_D 構造体を MATRIX         構造体に変換する
extern	void			ConvertMatrix4x4cFToMatrixD(     MATRIX_D       *Out, const MATRIX_4X4CT_F *In ) ;		// MATRIX_4X4CT_F 構造体を MATRIX_D       構造体に変換する
extern	void			ConvertMatrix4x4cFToMatrix4x4cD( MATRIX_4X4CT_D *Out, const MATRIX_4X4CT_F *In ) ;		// MATRIX_4X4CT_F 構造体を MATRIX_4X4CT_D 構造体に変換する
extern	void			ConvertMatrix4x4cDToMatrix4x4cF( MATRIX_4X4CT_F *Out, const MATRIX_4X4CT_D *In ) ;		// MATRIX_4X4CT_F 構造体を MATRIX_4X4CT_D 構造体に変換する
extern	void			ConvertMatrixFToMatrix4x4c(      MATRIX_4X4CT   *Out, const MATRIX         *In ) ;		// MATRIX         構造体を MATRIX_4X4CT   構造体に変換する
extern	void			ConvertMatrixDToMatrix4x4c(      MATRIX_4X4CT   *Out, const MATRIX_D       *In ) ;		// MATRIX_D       構造体を MATRIX_4X4CT   構造体に変換する
extern	void			ConvertMatrix4x4cToMatrixF(      MATRIX         *Out, const MATRIX_4X4CT   *In ) ;		// MATRIX_4X4CT   構造体を MATRIX         構造体に変換する
extern	void			ConvertMatrix4x4cToMatrixD(      MATRIX_D       *Out, const MATRIX_4X4CT   *In ) ;		// MATRIX_4X4CT   構造体を MATRIX_D       構造体に変換する
extern	void			ConvertMatrix4x4cToMatrix4x4cF(  MATRIX_4X4CT_F *Out, const MATRIX_4X4CT   *In ) ;		// MATRIX_4X4CT   構造体を MATRIX_4X4CT_F 構造体に変換する
extern	void			ConvertMatrix4x4cToMatrix4x4cD(  MATRIX_4X4CT_D *Out, const MATRIX_4X4CT   *In ) ;		// MATRIX_4X4CT   構造体を MATRIX_4X4CT_D 構造体に変換する
extern	void			ConvertMatrix4x4cFToMatrix4x4c(  MATRIX_4X4CT   *Out, const MATRIX_4X4CT_F *In ) ;		// MATRIX_4X4CT_F 構造体を MATRIX_4X4CT   構造体に変換する
extern	void			ConvertMatrix4x4cDToMatrix4x4c(  MATRIX_4X4CT   *Out, const MATRIX_4X4CT_D *In ) ;		// MATRIX_4X4CT_D 構造体を MATRIX_4X4CT   構造体に変換する

// データ一致検査系
extern	void			MakeDataCmpInfo( DATACMPINFO *CmpInfo, void *Image, int Size ) ;						// バイナリイメージからデータ一致検査データを作成する
extern	int				CheckDataCmpInfo( DATACMPINFO *CmpInfo1, DATACMPINFO *CmpInfo2 ) ;						// 二つのチェックデータが一致しているかどうかを取得する( 1:一致している  0:一致していない )

// 更新検出情報構築用関数
extern	void			MV1ChangeInfoSetup( MV1_CHANGE_BASE *ChangeB, void *FillTable, int BitAddress, int FillBitNum ) ;	// 状態変化管理用構造体のセットアップを行う

// モデル読み込み補助系
extern	int				AddVertexInfo( MV1_MAKEVERTINDEXINFO **InfoTable, MV1_MAKEVERTINDEXINFO *InfoBuffer, int *InfoNum, void *DataBuffer, int DataUnitSize, int DataNum, int AddDataIndex, void *AddData ) ;		// MV1_MAKEVERTINDEXINFO を利用した高速同一データ検索を補助する関数
extern	int				MV1MakeMeshBinormalsAndTangents( MV1_MESH_BASE *Mesh ) ;								// メッシュの従法線と接線を計算する
extern	int				MV1SetupTriangleListPositionAndNormal( MV1_TRIANGLE_LIST_BASE *List ) ;					// トライアングルリストの座標と法線情報のセットアップを行う
extern	int				MV1SetupShapeTriangleListPositionAndNormal( MV1_TRIANGLE_LIST *TList ) ;				// シェイプ用トライアングルリストの座標と法線情報のセットアップを行う

// 共通データ系
extern	int				MV1CreateGradationGraph( void ) ;														// グラデーション画像を作成する
extern	int				MV1GetDefaultToonTexture( int Type ) ;													// デフォルトトゥーンテクスチャを取得する
extern	void			MV1SetupTexNoneHandle( void ) ;															// TexNoneHandle のセットアップを行う

// グローバルな初期化と後始末
extern	int				MV1Initialize() ;																		// モデル機能の初期化
extern	int				MV1Terminate() ;																		// モデル機能の後始末

// テクスチャの追加
extern	int				__MV1LoadTexture(
							  void **ColorImage, int *ColorImageSize,
							  void **AlphaImage, int *AlphaImageSize,
							  int *GraphHandle,
							  int *SemiTransFlag,
							  int *DefaultTextureFlag,
#ifndef UNICODE
							  char    **ColorFilePathAMem, char    **AlphaFilePathAMem,
#endif
							  wchar_t **ColorFilePathWMem, wchar_t **AlphaFilePathWMem,
							  const wchar_t *ColorFilePath, const wchar_t *AlphaFilePath, const wchar_t *StartFolderPath,
							  int BumpImageFlag, float BumpImageNextPixelLength,
							  int ReverseFlag,
							  int IgnoreBmp32Alpha,
							  const MV1_FILE_READ_FUNC *FileReadFunc,
							  bool ValidImageAddr,
							  int NotInitGraphDelete,
							  int ASyncThread ) ;
extern	int				MV1CreateTextureColorBaseImage(
										BASEIMAGE *DestColorBaseImage,
										BASEIMAGE *DestAlphaBaseImage,
										void *ColorFileImage, int ColorFileSize,
										void *AlphaFileImage, int AlphaFileSize,
										int BumpImageFlag = FALSE, float BumpImageNextPixelLength = 0.1f,
										int ReverseFlag = FALSE ) ;												// モデル用テクスチャのカラーイメージを作成する
extern	int				MV1ReloadTexture( void ) ;																// テクスチャの再読み込み

// 追加ロード関数関係
extern	int				MV1AddLoadFunc( int ( *AddLoadFunc )( const MV1_MODEL_LOAD_PARAM *LoadParam ) ) ;		// モデル読み込み関数を追加する
extern	int				MV1SubLoadFunc( int ( *AddLoadFunc )( const MV1_MODEL_LOAD_PARAM *LoadParam ) ) ;		// モデル読み込み関数を削除する

// モデル基本データハンドルの追加・削除
extern	int				MV1InitModelBase( void ) ;																// 有効なモデル基本データをすべて削除する
extern	int				MV1AddModelBase( int ASyncThread ) ;													// モデル基本データを追加する( -1:エラー  0以上:モデル基本データハンドル )
extern	int				MV1SubModelBase( int MBHandle ) ;														// モデル基本データを削除する
extern	int				MV1CreateCloneModelBase( int SrcMBHandle ) ;											// モデル基本データを複製する

extern	int				InitializeModelBaseHandle( HANDLEINFO *HandleInfo ) ;									// モデル基本データハンドルの初期化
extern	int				TerminateModelBaseHandle( HANDLEINFO *HandleInfo ) ;									// モデル基本データハンドルの後始末

// 初期セットアップ系
extern	void			MV1SetupInitializeMatrixBase( MV1_MODEL_BASE *ModelBase ) ;								// 初期状態の変換行列をセットアップする
extern	void			MV1SetupPackDrawInfo( MV1_MODEL_BASE *ModelBase ) ;										// 同時複数描画関係の情報をセットアップする
extern	void			MV1SetupToonOutLineTriangleList( MV1_TRIANGLE_LIST_BASE *MBTList ) ;					// トゥーン輪郭線用のメッシュを作成する
extern	void			MV1SetupMeshSemiTransStateBase( MV1_MODEL_BASE *ModelBase ) ;							// モデル中のメッシュの半透明要素があるかどうかを調べる

// 法線再計算・座標最適化
extern	int				MV1ReMakeNormalBase( int MBHandle, float SmoothingAngle = 89.5f * DX_PI_F / 180.0f, int ASyncThread = FALSE ) ;	// モデル全体の法線を再計算する
extern	int				MV1ReMakeNormalFrameBase( int MBHandle, int FrameIndex, float SmoothingAngle = 89.5f * DX_PI_F / 180.0f ) ;	// 指定フレームが持つメッシュの法線を再計算する
extern	int				MV1PositionOptimizeBase( int MBHandle ) ;												// モデル全体の座標情報を最適化する
extern	int				MV1PositionOptimizeFrameBase( int MBHandle, int FrameIndex ) ;							// 指定フレームが持つメッシュの座標情報を最適化する

// 基本データ内マテリアル
extern	int				MV1GetMaterialNumBase( int MBHandle ) ;													// モデルで使用しているマテリアルの数を取得する
#ifndef UNICODE
extern	const char *	MV1GetMaterialNameBase( int MBHandle, int MaterialIndex ) ;								// 指定のマテリアルの名前を取得する
#endif
extern	const wchar_t *	MV1GetMaterialNameBaseW( int MBHandle, int MaterialIndex ) ;							// 指定のマテリアルの名前を取得する
extern	int				MV1SetMaterialTypeBase( int MBHandle, int MaterialIndex, int Type ) ;					// 指定のマテリアルのタイプを変更する( Type : DX_MATERIAL_TYPE_NORMAL など )
extern	int				MV1GetMaterialTypeBase( int MBHandle, int MaterialIndex ) ;								// 指定のマテリアルのタイプを取得する( 戻り値 : DX_MATERIAL_TYPE_NORMAL など )
extern	int				MV1SetMaterialTypeParamBase( int MBHandle, int MaterialIndex, va_list ParamList ) ;		// 指定のマテリアルのタイプ別パラメータを変更する( マテリアルタイプ DX_MATERIAL_TYPE_MAT_SPEC_LUMINANCE_TWO_COLOR などで使用 )
extern	int				MV1SetMaterialTypeParam_Base(    int MHandle, int MaterialIndex, va_list ParamList ) ;	// MV1SetMaterialTypeParam の実処理を行う関数
extern	int				MV1SetMaterialTypeParamAll_Base( int MHandle,                    va_list ParamList ) ;	// MV1SetMaterialTypeParamAll の実処理を行う関数
extern	COLOR_F			MV1GetMaterialDifColorBase( int MBHandle, int MaterialIndex ) ;							// 指定のマテリアルのディフューズカラーを取得する
extern	COLOR_F			MV1GetMaterialSpcColorBase( int MBHandle, int MaterialIndex ) ;							// 指定のマテリアルのスペキュラカラーを取得する
extern	COLOR_F			MV1GetMaterialEmiColorBase( int MBHandle, int MaterialIndex ) ;							// 指定のマテリアルのエミッシブカラーを取得する
extern	COLOR_F			MV1GetMaterialAmbColorBase( int MBHandle, int MaterialIndex ) ;							// 指定のマテリアルのアンビエントカラーを取得する
extern	float			MV1GetMaterialSpcPowerBase( int MBHandle, int MaterialIndex ) ;							// 指定のマテリアルのスペキュラの強さを取得する
extern	int				MV1SetMaterialDifMapTextureBase( int MBHandle, int MaterialIndex, int TexIndex ) ;		// 指定のマテリアルでディフューズマップとして使用するテクスチャを指定する
extern	int				MV1GetMaterialDifMapTextureBase( int MBHandle, int MaterialIndex ) ;					// 指定のマテリアルでディフューズマップとして使用されているテクスチャのインデックスを取得する
extern	int				MV1SetMaterialSpcMapTextureBase( int MBHandle, int MaterialIndex, int TexIndex ) ;		// 指定のマテリアルでスペキュラマップとして使用するテクスチャを指定する
extern	int				MV1GetMaterialSpcMapTextureBase( int MBHandle, int MaterialIndex ) ;					// 指定のマテリアルでスペキュラマップとして使用されているテクスチャのインデックスを取得する
extern	int				MV1SetMaterialNormalMapTextureBase( int MBHandle, int MaterialIndex, int TexIndex ) ;	// 指定のマテリアルで法線マップとして使用するテクスチャを指定する
extern	int				MV1GetMaterialNormalMapTextureBase( int MBHandle, int MaterialIndex ) ;					// 指定のマテリアルで法線マップとして使用されているテクスチャのインデックスを取得する
extern	int				MV1SetMaterialDifColorBase( int MBHandle, int MaterialIndex, COLOR_F Color ) ;			// 指定のマテリアルのディフューズカラーを設定する
extern	int				MV1SetMaterialSpcColorBase( int MBHandle, int MaterialIndex, COLOR_F Color ) ;			// 指定のマテリアルのスペキュラカラーを設定する
extern	int				MV1SetMaterialEmiColorBase( int MBHandle, int MaterialIndex, COLOR_F Color ) ;			// 指定のマテリアルのエミッシブカラーを設定する
extern	int				MV1SetMaterialAmbColorBase( int MBHandle, int MaterialIndex, COLOR_F Color ) ;			// 指定のマテリアルのアンビエントカラーを設定する
extern	int				MV1SetMaterialSpcPowerBase( int MBHandle, int MaterialIndex, float Power ) ;			// 指定のマテリアルのスペキュラの強さを設定する
extern	int				MV1SetMaterialDifGradTextureBase( int MBHandle, int MaterialIndex, int TexIndex ) ;		// 指定のマテリアルでトゥーンレンダリングのディフューズグラデーションマップとして使用するテクスチャを設定する
extern	int				MV1GetMaterialDifGradTextureBase( int MBHandle, int MaterialIndex ) ;					// 指定のマテリアルでトゥーンレンダリングのディフューズグラデーションマップとして使用するテクスチャを取得する
extern	int				MV1SetMaterialSpcGradTextureBase( int MBHandle, int MaterialIndex, int TexIndex ) ;		// 指定のマテリアルでトゥーンレンダリングのスペキュラグラデーションマップとして使用するテクスチャを設定する
extern	int				MV1GetMaterialSpcGradTextureBase( int MBHandle, int MaterialIndex ) ;					// 指定のマテリアルでトゥーンレンダリングのスペキュラグラデーションマップとして使用するテクスチャを取得する
extern	int				MV1SetMaterialSphereMapTextureBase( int MBHandle, int MaterialIndex, int TexIndex ) ;	// 指定のマテリアルでトゥーンレンダリングのスフィアマップとして使用するテクスチャを設定する
extern	int				MV1GetMaterialSphereMapTextureBase( int MBHandle, int MaterialIndex ) ;					// 指定のマテリアルでトゥーンレンダリングのスフィアマップとして使用するテクスチャを取得する
extern	int				MV1SetMaterialDifGradBlendTypeBase( int MBHandle, int MaterialIndex, int BlendType ) ;	// 指定のマテリアルのトゥーンレンダリングで使用するディフューズグラデーションマップとディフューズカラーの合成方法を設定する( DX_MATERIAL_BLENDTYPE_ADDITIVE など )
extern	int				MV1GetMaterialDifGradBlendTypeBase( int MBHandle, int MaterialIndex ) ;					// 指定のマテリアルのトゥーンレンダリングで使用するディフューズグラデーションマップとディフューズカラーの合成方法を取得する( DX_MATERIAL_BLENDTYPE_ADDITIVE など )
extern	int				MV1SetMaterialSpcGradBlendTypeBase( int MBHandle, int MaterialIndex, int BlendType ) ;	// 指定のマテリアルのトゥーンレンダリングで使用するスペキュラグラデーションマップとスペキュラカラーの合成方法を設定する( DX_MATERIAL_BLENDTYPE_ADDITIVE など )
extern	int				MV1GetMaterialSpcGradBlendTypeBase( int MBHandle, int MaterialIndex ) ;					// 指定のマテリアルのトゥーンレンダリングで使用するスペキュラグラデーションマップとスペキュラカラーの合成方法を取得する( DX_MATERIAL_BLENDTYPE_ADDITIVE など )
extern	int				MV1SetMaterialSphereMapBlendTypeBase( int MBHandle, int MaterialIndex, int BlendType ) ;// 指定のマテリアルのトゥーンレンダリングで使用するスフィアマップの合成方法を設定する( DX_MATERIAL_BLENDTYPE_ADDITIVE など )
extern	int				MV1GetMaterialSphereMapBlendTypeBase( int MBHandle, int MaterialIndex ) ;				// 指定のマテリアルのトゥーンレンダリングで使用するスフィアマップの合成方法を取得する( DX_MATERIAL_BLENDTYPE_ADDITIVE など )
extern	int				MV1SetMaterialOutLineWidthBase( int MBHandle, int MaterialIndex, float Width ) ;		// 指定のマテリアルのトゥーンレンダリングで使用する輪郭線の太さを設定する
extern	float			MV1GetMaterialOutLineWidthBase( int MBHandle, int MaterialIndex ) ;						// 指定のマテリアルのトゥーンレンダリングで使用する輪郭線のドット単位の太さを取得する
extern	int				MV1SetMaterialOutLineDotWidthBase( int MBHandle, int MaterialIndex, float Width ) ;		// 指定のマテリアルのトゥーンレンダリングで使用する輪郭線のドット単位の太さを設定する
extern	float			MV1GetMaterialOutLineDotWidthBase( int MBHandle, int MaterialIndex ) ;					// 指定のマテリアルのトゥーンレンダリングで使用する輪郭線の太さを取得する( 0.0f 〜 1.0f )
extern	int				MV1SetMaterialOutLineColorBase( int MBHandle, int MaterialIndex, COLOR_F Color ) ;		// 指定のマテリアルのトゥーンレンダリングで使用する輪郭線の色を設定する
extern	COLOR_F			MV1GetMaterialOutLineColorBase( int MBHandle, int MaterialIndex ) ;						// 指定のマテリアルのトゥーンレンダリングで使用する輪郭線の色を取得する
extern	int				MV1SetMaterialDrawBlendModeBase( int MBHandle, int MaterialIndex, int BlendMode ) ;		// 指定のマテリアルの描画ブレンドモードを設定する( DX_BLENDMODE_ALPHA 等 )
extern	int				MV1SetMaterialDrawBlendParamBase( int MBHandle, int MaterialIndex, int BlendParam ) ;	// 指定のマテリアルの描画ブレンドパラメータを設定する
extern	int				MV1GetMaterialDrawBlendModeBase( int MBHandle, int MaterialIndex ) ;					// 指定のマテリアルの描画ブレンドモードを取得する( DX_BLENDMODE_ALPHA 等 )
extern	int				MV1GetMaterialDrawBlendParamBase( int MBHandle, int MaterialIndex ) ;					// 指定のマテリアルの描画ブレンドパラメータを設定する
extern	int				MV1SetMaterialDrawAlphaTestBase( int MBHandle, int MaterialIndex, int Enable, int Mode, int Param ) ;	// 指定のマテリアルの描画時のアルファテストの設定を行う( Enable:αテストを行うかどうか( TRUE:行う  FALSE:行わない( デフォルト )  Mode:テストモード( DX_CMP_GREATER等 )  Param:描画アルファ値との比較に使用する値( 0〜255 ) )
extern	int				MV1GetMaterialDrawAlphaTestEnableBase( int MBHandle, int MaterialIndex ) ;				// 指定のマテリアルの描画時のアルファテストを行うかどうかを取得する( 戻り値  TRUE:アルファテストを行う  FALSE:アルファテストを行わない )
extern	int				MV1GetMaterialDrawAlphaTestModeBase( int MBHandle, int MaterialIndex ) ;				// 指定のマテリアルの描画時のアルファテストのテストモードを取得する( 戻り値  テストモード( DX_CMP_GREATER等 ) )
extern	int				MV1GetMaterialDrawAlphaTestParamBase( int MBHandle, int MaterialIndex ) ;				// 指定のマテリアルの描画時のアルファテストの描画アルファ地との比較に使用する値( 0〜255 )を取得する

// 基本データ内テクスチャ関係
extern	int				MV1GetTextureNumBase( int MBHandle ) ;													// テクスチャの数を取得
extern	int				MV1AddTextureBase( 
							int MBHandle,
							const wchar_t *Name,
							const wchar_t *ColorFilePathW, const wchar_t *AlphaFilePathW,
							void *ColorFileImage, void *AlphaFileImage,
							int AddressModeU, int AddressModeV, int FilterMode,
							int BumpImageFlag, float BumpImageNextPixelLength,
							bool ReverseFlag,
							bool Bmp32AllZeroAlphaToXRGB8Flag,
							int ASyncThread ) ;																	// テクスチャの追加
extern	int				MV1DeleteTextureBase( int MBHandle, int TexIndex ) ;									// テクスチャの削除
#ifndef UNICODE
extern	const char *	MV1GetTextureNameBase( int MBHandle, int TexIndex ) ;									// テクスチャの名前を取得
#endif
extern	const wchar_t *	MV1GetTextureNameBaseW( int MBHandle, int TexIndex ) ;									// テクスチャの名前を取得
extern	int				MV1SetTextureColorFilePathBaseW( int MBHandle, int TexIndex, const wchar_t *FilePathW ) ;	// カラーテクスチャのファイルパスを変更する
extern	const wchar_t *	MV1GetTextureColorFilePathBaseW( int MBHandle, int TexIndex ) ;							// カラーテクスチャのファイルパスを取得
extern	int				MV1SetTextureAlphaFilePathBaseW( int MBHandle, int TexIndex, const wchar_t *FilePathW ) ;	// アルファテクスチャのファイルパスを変更する
extern	const wchar_t *	MV1GetTextureAlphaFilePathBaseW( int MBHandle, int TexIndex ) ;							// アルファテクスチャのファイルパスを取得
extern	int				MV1SetTextureGraphHandleBase( int MBHandle, int TexIndex, int GrHandle, int SemiTransFlag ) ;	// テクスチャで使用するグラフィックハンドルを変更する( GrHandle を -1 にすると解除 )
extern	int				MV1GetTextureGraphHandleBase( int MBHandle, int TexIndex ) ;							// テクスチャのグラフィックハンドルを取得する
extern	int				MV1SetTextureAddressModeBase( int MBHandle, int TexIndex, int AddrUMode, int AddrVMode ) ;	// テクスチャのアドレスモードを設定する( AddrUMode は DX_TEXADDRESS_WRAP 等 )
extern	int				MV1GetTextureAddressModeUBase( int MBHandle, int TexIndex ) ;							// テクスチャのＵ値のアドレスモードを取得する( 戻り値:DX_TEXADDRESS_WRAP 等 )
extern	int				MV1GetTextureAddressModeVBase( int MBHandle, int TexIndex ) ;							// テクスチャのＶ値のアドレスモードを取得する( 戻り値:DX_TEXADDRESS_WRAP 等 )
extern	int				MV1GetTextureWidthBase( int MBHandle, int TexIndex ) ;									// テクスチャの幅を取得する
extern	int				MV1GetTextureHeightBase( int MBHandle, int TexIndex ) ;									// テクスチャの高さを取得する
extern	int				MV1GetTextureSemiTransStateBase( int MBHandle, int TexIndex ) ;							// テクスチャに半透明要素があるかどうかを取得する( 戻り値  TRUE:ある  FALSE:ない )
extern	int				MV1SetTextureBumpImageFlagBase( int MBHandle, int TexIndex, int Flag ) ;				// テクスチャで使用している画像がバンプマップかどうかを設定する
extern	int				MV1GetTextureBumpImageFlagBase( int MBHandle, int TexIndex ) ;							// テクスチャがバンプマップかどうかを取得する( 戻り値  TRUE:バンプマップ  FALSE:違う )
extern	int				MV1SetTextureBumpImageNextPixelLengthBase( int MBHandle, int TexIndex, float Length ) ;	// バンプマップ画像の場合の隣のピクセルとの距離を設定する
extern	float			MV1GetTextureBumpImageNextPixelLengthBase( int MBHandle, int TexIndex ) ;				// バンプマップ画像の場合の隣のピクセルとの距離を取得する
extern	int				MV1SetTextureSampleFilterModeBase( int MBHandle, int TexIndex, int FilterMode ) ;		// テクスチャのフィルタリングモードを設定する
extern	int				MV1GetTextureSampleFilterModeBase( int MBHandle, int TexIndex ) ;						// テクスチャのフィルタリングモードを取得する( 戻り値  DX_DRAWMODE_BILINEAR等 )

// メッシュ関係
extern	int				MV1SetMeshUseVertDifColorBase( int MBHandle, int MeshIndex, int UseFlag ) ;				// 指定のメッシュの頂点ディフューズカラーをマテリアルのディフューズカラーの代わりに使用するかどうかを設定する( TRUE:マテリアルカラーの代わりに使用する  FALSE:マテリアルカラーを使用する )
extern	int				MV1SetMeshUseVertSpcColorBase( int MBHandle, int MeshIndex, int UseFlag ) ;				// 指定のメッシュの頂点スペキュラカラーをマテリアルのスペキュラカラーの代わりに使用するかどうかを設定する( TRUE:マテリアルカラーの代わりに使用する  FALSE:マテリアルカラーを使用する )
extern	int				MV1GetMeshUseVertDifColorBase( int MBHandle, int MeshIndex ) ;							// 指定のメッシュの頂点ディフューズカラーをマテリアルのディフューズカラーの代わりに使用するかどうかの設定を取得する( 戻り値  TRUE:マテリアルカラーの代わりに使用する  FALSE:マテリアルカラーを使用する )
extern	int				MV1GetMeshUseVertSpcColorBase( int MBHandle, int MeshIndex ) ;							// 指定のメッシュの頂点スペキュラカラーをマテリアルのスペキュラカラーの代わりに使用するかどうかの設定を取得する( 戻り値  TRUE:マテリアルカラーの代わりに使用する  FALSE:マテリアルカラーを使用する )
extern	int				MV1GetMeshShapeFlagBase( int MBHandle, int MeshIndex ) ;								// 指定のメッシュがシェイプメッシュかどうかを取得する( 戻り値 TRUE:シェイプメッシュ  FALSE:通常メッシュ )

// シェイプ関係
extern	int				MV1GetShapeNumBase( int MBHandle ) ;													// モデルに含まれるシェイプの数を取得する
extern	int				MV1SearchShapeBase( int MBHandle, const wchar_t *ShapeName ) ;							// シェイプの名前からモデル中のシェイプのシェイプインデックスを取得する( 無かった場合は戻り値が-1 )
#ifndef UNICODE
extern	const char *	MV1GetShapeNameBaseA( int MBHandle, int ShapeIndex ) ;									// 指定シェイプの名前を取得する
#endif
extern	const wchar_t *	MV1GetShapeNameBaseW( int MBHandle, int ShapeIndex ) ;									// 指定シェイプの名前を取得する
extern	int				MV1GetShapeTargetMeshNumBase( int MBHandle, int ShapeIndex ) ;							// 指定シェイプが対象としているメッシュの数を取得する
extern	int				MV1GetShapeTargetMeshBase( int MBHandle, int ShapeIndex, int Index ) ;					// 指定シェイプが対象としているメッシュのメッシュインデックスを取得する

// モデルデータ構築関係
extern	int				MV1AddModel( int ASyncThread ) ;														// モデルデータを追加する( -1:エラー  0以上:モデルデータハンドル )
extern	int				MV1SubModel( int MV1ModelHandle ) ;														// モデルデータを削除する
extern	int				MV1MakeModel( int MV1ModelHandle, int MV1ModelBaseHandle, int ASyncThread = FALSE ) ;	// モデル基データからモデルデータを構築する( -1:エラー 0:成功 )

extern	int				MV1GetMaxMinPosition( int MHandle, VECTOR *MaxPosition, VECTOR *MinPosition, int ASyncThread = FALSE ) ;	// モデルの最大頂点座標と最小頂点座標を取得する

extern	int				InitializeModelHandle( HANDLEINFO *HandleInfo ) ;										// モデルデータハンドルの初期化
extern	int				TerminateModelHandle( HANDLEINFO *HandleInfo ) ;										// モデルデータハンドルの後始末

// モデルデータ関係
extern	int				MV1GetModelBaseHandle( int MHandle ) ;													// モデルハンドルで使用されているモデル基本データハンドルを取得する
extern	int				MV1GetModelDataSize( int MHandle, int DataType ) ;										// モデルのデータサイズを取得する
extern	int				MV1GetAnimDataSize( int MHandle, const wchar_t *AnimName = NULL, int AnimIndex = -1 ) ;	// アニメーションのデータサイズを取得する
extern	int				MV1SetNotUseFrameRotation( int MHandle, int NotUseFrameRotation ) ;						// フレームの回転を使用しないかどうかを設定する
extern	int				MV1GetNotUseFrameRotation( int MHandle ) ;												// フレームの回転を使用しないかどうかを取得する

// モデル描画関係
extern	int				MV1DrawPackDrawModel( void ) ;															// 同時複数描画の為に描画待機しているモデルを描画する

// モデル物理演算関係
extern	int				MV1PhysicsCalculationBase( int MHandle, float MillisecondTime, int ASyncLoadFlag = FALSE ) ;	// モデルの物理演算を指定時間分経過したと仮定して計算する( MillisecondTime で指定する時間の単位はミリ秒 )



// モデルの読み込み・保存・複製関係
extern	int				MV1LoadModelToPMX( const MV1_MODEL_LOAD_PARAM *LoadParam, int ASyncThread = FALSE ) ;	// ＰＭＸファイルを読み込む( -1:エラー  0以上:モデルハンドル )
extern	int				MV1LoadModelToPMD( const MV1_MODEL_LOAD_PARAM *LoadParam, int ASyncThread = FALSE ) ;	// ＰＭＤファイルを読み込む( -1:エラー  0以上:モデルハンドル )
extern	int				MV1LoadModelToVMD( const MV1_MODEL_LOAD_PARAM *LoadParam, int ASyncThread = FALSE ) ;	// ＶＭＤファイルを読み込む( -1:エラー  0以上:モデルハンドル )
extern	int				MV1LoadModelToX(   const MV1_MODEL_LOAD_PARAM *LoadParam, int ASyncThread = FALSE ) ;	// Ｘファイルを読み込む( -1:エラー  0以上:モデルハンドル )
extern	int				MV1LoadModelToFBX( const MV1_MODEL_LOAD_PARAM *LoadParam, int ASyncThread = FALSE ) ;	// ＦＢＸファイルを読み込む( -1:エラー  0以上:モデルハンドル )
extern	int				MV1LoadModelToMQO( const MV1_MODEL_LOAD_PARAM *LoadParam, int ASyncThread = FALSE ) ;	// ＭＱＯファイルを読み込む( -1:エラー  0以上:モデルハンドル )
extern	int				MV1LoadModelToMV1( const MV1_MODEL_LOAD_PARAM *LoadParam, int ASyncThread = FALSE ) ;	// ＭＶ１ファイルを読み込む( -1:エラー  0以上:モデルハンドル )
extern	int				MV1SetupVertexBufferAll( int ASyncThread = FALSE ) ;									// 頂点バッファのセットアップをする( -1:エラー )
extern	int				MV1TerminateVertexBufferAll( void ) ;													// 全ての頂点バッファの後始末をする( -1:エラー )
extern	const wchar_t *	MV1GetModelFileName( int MHandle ) ;													// ロードしたモデルのファイル名を取得する
extern	const wchar_t *	MV1GetModelDirectoryPath( int MHandle ) ;												// ロードしたモデルが存在するディレクトリパスを取得する( 末端に / か \ が付いています )

extern	void			InitMV1LoadModelGParam( MV1LOADMODEL_GPARAM *GParam ) ;									// MV1LOADMODEL_GPARAM のデータをセットする

extern	int				MV1LoadModel_UseGParam( MV1LOADMODEL_GPARAM *GParam, const wchar_t *FileName, int ASyncLoadFlag = FALSE ) ;																																																									// MV1LoadModel のグローバル変数にアクセスしないバージョン
extern	int				MV1LoadModelFromMem_UseGParam( MV1LOADMODEL_GPARAM *GParam, void *FileImage, int FileSize, int (* FileReadFunc )( const TCHAR *FilePath, void **FileImageAddr, int *FileSize, void *FileReadFuncData ), int (* FileReleaseFunc )( void *MemoryAddr, void *FileReadFuncData ), void *FileReadFuncData = NULL, int ASyncLoadFlag = FALSE ) ;	// MV1LoadModelFromMem のグローバル変数にアクセスしないバージョン

extern	int				MV1LoadModelDisablePhysicsNameCheck_ShiftJIS( const char *Name, int DisablePhysicsFile ) ;	// 物理演算が無効な名前かどうかをチェックする( 戻り値　TRUE:無効  FALSE:有効 )( ShiftJISコード版 )
extern	int				MV1LoadModelDisablePhysicsNameCheck_WCHAR_T(  const WORD *Name, int DisablePhysicsFile ) ;	// 物理演算が無効な名前かどうかをチェックする( 戻り値　TRUE:無効  FALSE:有効 )( wchar_t版 )

// ライト関係
extern	int				MV1GetLightNum( int MHandle ) ;															// ライトの数を取得する
extern	int				MV1LightSetup( int MHandle ) ;															// 持っているライトをライブラリに反映させる

// アニメーション関係
extern	int				MV1GetAttachAnimTargetFrameNum( int MHandle, int AttachIndex ) ;						// アタッチしているアニメーションがターゲットとするフレームの数を取得する
extern	int				MV1GetAttachAnimTargetFrame( int MHandle, int AttachIndex, int Index ) ;				// アタッチしているアニメーションがターゲットとするフレームのインデックスを所得する
extern	int				MV1GetAttachAnimTargetFrameToAnimFrameIndex( int MHandle, int AttachIndex, int FrameIndex ) ;	// 指定のフレームがアタッチしているアニメーションの何番目のターゲットフレームかを取得する( AnimFrameIndex として使用する )

// モデル基本制御関係
extern	VECTOR			MV1GetRotationZAxisZ( int MHandle ) ;													// モデルのＺ軸の方向を得る
extern	VECTOR			MV1GetRotationZAxisUp( int MHandle ) ;													// モデルのＺ軸の方向を指定した際の上方向ベクトルを得る
extern	float			MV1GetRotationZAxisTwist( int MHandle ) ;												// モデルのＺ軸の方向を指定した際の捻り角度を得る

// トライアングルリスト関係
extern	int				MV1GetTriangleListUseBoneFrameNum( int MHandle, int TListIndex ) ;						// 指定のトライアングルリストが使用しているボーンフレームの数を取得する
extern	int				MV1GetTriangleListUseBoneFrame( int MHandle, int TListIndex, int Index ) ;				// 指定のトライアングルリストが使用しているボーンフレームのフレームインデックスを取得する

// 球面線形補間を行う
__inline static void _MV1SphereLinear( FLOAT4 *Q1, FLOAT4 *Q2, float t, FLOAT4 *Ret )
{
	float Sin1, Cos1, Sin2, Cos2, Sin3, Cos3 ;
	float s1, s2, qr, ss, ph ;

	qr = Q1->w * Q2->w + Q1->x * Q2->x + Q1->y * Q2->y + Q1->z * Q2->z ;
	ss = 1.0f - ( qr * qr ) ;
	if( ss <= 0.0f || qr >= 1.0f )
	{
		Ret->x = Q1->x ;
		Ret->y = Q1->y ;
		Ret->z = Q1->z ;
		Ret->w = Q1->w ;
	}
	else
	{
		ph = _ACOS( qr ) ;
		if( qr < 0.0f && ph > DX_PI_F / 2.0f )
		{
			qr = - Q1->w * Q2->w - Q1->x * Q2->x - Q1->y * Q2->y - Q1->z * Q2->z ;
			ph = _ACOS( qr ) ;
			_SINCOS_PLATFORM( ph,                &Sin1, &Cos1 ) ;
			_SINCOS_PLATFORM( ph * ( 1.0f - t ), &Sin2, &Cos2 ) ;
			_SINCOS_PLATFORM( ph *          t  , &Sin3, &Cos3 ) ;
			s1 = Sin2 / Sin1 ;
			s2 = Sin3 / Sin1 ;
			Ret->x = Q1->x * s1 - Q2->x * s2 ;
			Ret->y = Q1->y * s1 - Q2->y * s2 ;
			Ret->z = Q1->z * s1 - Q2->z * s2 ;
			Ret->w = Q1->w * s1 - Q2->w * s2 ;
		}
		else
		{
			_SINCOS_PLATFORM( ph,               &Sin1, &Cos1 ) ;
			_SINCOS_PLATFORM( ph * (1.0f - t ), &Sin2, &Cos2 ) ;
			_SINCOS_PLATFORM( ph *         t  , &Sin3, &Cos3 ) ;
			s1 = Sin2 / Sin1 ;
			s2 = Sin3 / Sin1 ;
			Ret->x = Q1->x * s1 + Q2->x * s2 ;
			Ret->y = Q1->y * s1 + Q2->y * s2 ;
			Ret->z = Q1->z * s1 + Q2->z * s2 ;
			Ret->w = Q1->w * s1 + Q2->w * s2 ;
		}
	}
}

__inline void UnSafeMultiplyMatrix4X4CTF( MATRIX_4X4CT_F * RST Out, const MATRIX_4X4CT_F * RST In1, const MATRIX_4X4CT_F * RST In2 )
{
	Out->m[0][0] = In1->m[0][0] * In2->m[0][0] + In1->m[1][0] * In2->m[0][1] + In1->m[2][0] * In2->m[0][2] ;
	Out->m[1][0] = In1->m[0][0] * In2->m[1][0] + In1->m[1][0] * In2->m[1][1] + In1->m[2][0] * In2->m[1][2] ;
	Out->m[2][0] = In1->m[0][0] * In2->m[2][0] + In1->m[1][0] * In2->m[2][1] + In1->m[2][0] * In2->m[2][2] ;

	Out->m[0][1] = In1->m[0][1] * In2->m[0][0] + In1->m[1][1] * In2->m[0][1] + In1->m[2][1] * In2->m[0][2] ;
	Out->m[1][1] = In1->m[0][1] * In2->m[1][0] + In1->m[1][1] * In2->m[1][1] + In1->m[2][1] * In2->m[1][2] ;
	Out->m[2][1] = In1->m[0][1] * In2->m[2][0] + In1->m[1][1] * In2->m[2][1] + In1->m[2][1] * In2->m[2][2] ;

	Out->m[0][2] = In1->m[0][2] * In2->m[0][0] + In1->m[1][2] * In2->m[0][1] + In1->m[2][2] * In2->m[0][2] ;
	Out->m[1][2] = In1->m[0][2] * In2->m[1][0] + In1->m[1][2] * In2->m[1][1] + In1->m[2][2] * In2->m[1][2] ;
	Out->m[2][2] = In1->m[0][2] * In2->m[2][0] + In1->m[1][2] * In2->m[2][1] + In1->m[2][2] * In2->m[2][2] ;

	Out->m[0][3] = In1->m[0][3] * In2->m[0][0] + In1->m[1][3] * In2->m[0][1] + In1->m[2][3] * In2->m[0][2] + In2->m[0][3] ;
	Out->m[1][3] = In1->m[0][3] * In2->m[1][0] + In1->m[1][3] * In2->m[1][1] + In1->m[2][3] * In2->m[1][2] + In2->m[1][3] ;
	Out->m[2][3] = In1->m[0][3] * In2->m[2][0] + In1->m[1][3] * In2->m[2][1] + In1->m[2][3] * In2->m[2][2] + In2->m[2][3] ;
}

__inline void UnSafeMultiplyMatrix4X4CTD( MATRIX_4X4CT_D * RST Out, const MATRIX_4X4CT_D * RST In1, const MATRIX_4X4CT_D * RST In2 )
{
	Out->m[0][0] = In1->m[0][0] * In2->m[0][0] + In1->m[1][0] * In2->m[0][1] + In1->m[2][0] * In2->m[0][2] ;
	Out->m[1][0] = In1->m[0][0] * In2->m[1][0] + In1->m[1][0] * In2->m[1][1] + In1->m[2][0] * In2->m[1][2] ;
	Out->m[2][0] = In1->m[0][0] * In2->m[2][0] + In1->m[1][0] * In2->m[2][1] + In1->m[2][0] * In2->m[2][2] ;

	Out->m[0][1] = In1->m[0][1] * In2->m[0][0] + In1->m[1][1] * In2->m[0][1] + In1->m[2][1] * In2->m[0][2] ;
	Out->m[1][1] = In1->m[0][1] * In2->m[1][0] + In1->m[1][1] * In2->m[1][1] + In1->m[2][1] * In2->m[1][2] ;
	Out->m[2][1] = In1->m[0][1] * In2->m[2][0] + In1->m[1][1] * In2->m[2][1] + In1->m[2][1] * In2->m[2][2] ;

	Out->m[0][2] = In1->m[0][2] * In2->m[0][0] + In1->m[1][2] * In2->m[0][1] + In1->m[2][2] * In2->m[0][2] ;
	Out->m[1][2] = In1->m[0][2] * In2->m[1][0] + In1->m[1][2] * In2->m[1][1] + In1->m[2][2] * In2->m[1][2] ;
	Out->m[2][2] = In1->m[0][2] * In2->m[2][0] + In1->m[1][2] * In2->m[2][1] + In1->m[2][2] * In2->m[2][2] ;

	Out->m[0][3] = In1->m[0][3] * In2->m[0][0] + In1->m[1][3] * In2->m[0][1] + In1->m[2][3] * In2->m[0][2] + In2->m[0][3] ;
	Out->m[1][3] = In1->m[0][3] * In2->m[1][0] + In1->m[1][3] * In2->m[1][1] + In1->m[2][3] * In2->m[1][2] + In2->m[1][3] ;
	Out->m[2][3] = In1->m[0][3] * In2->m[2][0] + In1->m[1][3] * In2->m[2][1] + In1->m[2][3] * In2->m[2][2] + In2->m[2][3] ;
}

__inline void UnSafeMultiplyMatrix4X4CT_DF_D( MATRIX_4X4CT_D * RST Out, const MATRIX_4X4CT_D * RST In1, const MATRIX_4X4CT_F * RST In2 )
{
	Out->m[0][0] = In1->m[0][0] * In2->m[0][0] + In1->m[1][0] * In2->m[0][1] + In1->m[2][0] * In2->m[0][2] ;
	Out->m[1][0] = In1->m[0][0] * In2->m[1][0] + In1->m[1][0] * In2->m[1][1] + In1->m[2][0] * In2->m[1][2] ;
	Out->m[2][0] = In1->m[0][0] * In2->m[2][0] + In1->m[1][0] * In2->m[2][1] + In1->m[2][0] * In2->m[2][2] ;

	Out->m[0][1] = In1->m[0][1] * In2->m[0][0] + In1->m[1][1] * In2->m[0][1] + In1->m[2][1] * In2->m[0][2] ;
	Out->m[1][1] = In1->m[0][1] * In2->m[1][0] + In1->m[1][1] * In2->m[1][1] + In1->m[2][1] * In2->m[1][2] ;
	Out->m[2][1] = In1->m[0][1] * In2->m[2][0] + In1->m[1][1] * In2->m[2][1] + In1->m[2][1] * In2->m[2][2] ;

	Out->m[0][2] = In1->m[0][2] * In2->m[0][0] + In1->m[1][2] * In2->m[0][1] + In1->m[2][2] * In2->m[0][2] ;
	Out->m[1][2] = In1->m[0][2] * In2->m[1][0] + In1->m[1][2] * In2->m[1][1] + In1->m[2][2] * In2->m[1][2] ;
	Out->m[2][2] = In1->m[0][2] * In2->m[2][0] + In1->m[1][2] * In2->m[2][1] + In1->m[2][2] * In2->m[2][2] ;

	Out->m[0][3] = In1->m[0][3] * In2->m[0][0] + In1->m[1][3] * In2->m[0][1] + In1->m[2][3] * In2->m[0][2] + In2->m[0][3] ;
	Out->m[1][3] = In1->m[0][3] * In2->m[1][0] + In1->m[1][3] * In2->m[1][1] + In1->m[2][3] * In2->m[1][2] + In2->m[1][3] ;
	Out->m[2][3] = In1->m[0][3] * In2->m[2][0] + In1->m[1][3] * In2->m[2][1] + In1->m[2][3] * In2->m[2][2] + In2->m[2][3] ;
}

__inline void UnSafeMultiplyMatrix4X4CT_FD_D( MATRIX_4X4CT_D * RST Out, const MATRIX_4X4CT_F * RST In1, const MATRIX_4X4CT_D * RST In2 )
{
	Out->m[0][0] = In1->m[0][0] * In2->m[0][0] + In1->m[1][0] * In2->m[0][1] + In1->m[2][0] * In2->m[0][2] ;
	Out->m[1][0] = In1->m[0][0] * In2->m[1][0] + In1->m[1][0] * In2->m[1][1] + In1->m[2][0] * In2->m[1][2] ;
	Out->m[2][0] = In1->m[0][0] * In2->m[2][0] + In1->m[1][0] * In2->m[2][1] + In1->m[2][0] * In2->m[2][2] ;

	Out->m[0][1] = In1->m[0][1] * In2->m[0][0] + In1->m[1][1] * In2->m[0][1] + In1->m[2][1] * In2->m[0][2] ;
	Out->m[1][1] = In1->m[0][1] * In2->m[1][0] + In1->m[1][1] * In2->m[1][1] + In1->m[2][1] * In2->m[1][2] ;
	Out->m[2][1] = In1->m[0][1] * In2->m[2][0] + In1->m[1][1] * In2->m[2][1] + In1->m[2][1] * In2->m[2][2] ;

	Out->m[0][2] = In1->m[0][2] * In2->m[0][0] + In1->m[1][2] * In2->m[0][1] + In1->m[2][2] * In2->m[0][2] ;
	Out->m[1][2] = In1->m[0][2] * In2->m[1][0] + In1->m[1][2] * In2->m[1][1] + In1->m[2][2] * In2->m[1][2] ;
	Out->m[2][2] = In1->m[0][2] * In2->m[2][0] + In1->m[1][2] * In2->m[2][1] + In1->m[2][2] * In2->m[2][2] ;

	Out->m[0][3] = In1->m[0][3] * In2->m[0][0] + In1->m[1][3] * In2->m[0][1] + In1->m[2][3] * In2->m[0][2] + In2->m[0][3] ;
	Out->m[1][3] = In1->m[0][3] * In2->m[1][0] + In1->m[1][3] * In2->m[1][1] + In1->m[2][3] * In2->m[1][2] + In2->m[1][3] ;
	Out->m[2][3] = In1->m[0][3] * In2->m[2][0] + In1->m[1][3] * In2->m[2][1] + In1->m[2][3] * In2->m[2][2] + In2->m[2][3] ;
}

__inline void UnSafeMultiplyMatrix4X4CT( MATRIX_4X4CT * RST Out, MATRIX_4X4CT * RST In1, MATRIX_4X4CT * RST In2 )
{
	if( GSYS.DrawSetting.Large3DPositionSupport )
	{
		UnSafeMultiplyMatrix4X4CTD( &Out->md, &In1->md, &In2->md ) ;
	}
	else
	{
		UnSafeMultiplyMatrix4X4CTF( &Out->mf, &In1->mf, &In2->mf ) ;
	}
}

__inline void UnSafeMultiplyMatrix4X4CT_FC( MATRIX_4X4CT * RST Out, MATRIX_4X4CT_F * RST In1, MATRIX_4X4CT * RST In2 )
{
	if( GSYS.DrawSetting.Large3DPositionSupport )
	{
		UnSafeMultiplyMatrix4X4CT_FD_D( &Out->md, In1, &In2->md ) ;
	}
	else
	{
		UnSafeMultiplyMatrix4X4CTF( &Out->mf, In1, &In2->mf ) ;
	}
}

__inline void UnSafeMultiplyMatrix4X4CT_CF( MATRIX_4X4CT * RST Out, MATRIX_4X4CT * RST In1, MATRIX_4X4CT_F * RST In2 )
{
	if( GSYS.DrawSetting.Large3DPositionSupport )
	{
		UnSafeMultiplyMatrix4X4CT_DF_D( &Out->md, &In1->md, In2 ) ;
	}
	else
	{
		UnSafeMultiplyMatrix4X4CTF( &Out->mf, &In1->mf, In2 ) ;
	}
}

__inline void UnSafeMatrix4X4CT_F_Eq_F_Mul_S( MATRIX_4X4CT_F *RST Out, MATRIX_4X4CT_F * RST In1, float In2 )
{
	Out->m[ 0 ][ 0 ] = In1->m[ 0 ][ 0 ] * In2 ;
	Out->m[ 0 ][ 1 ] = In1->m[ 0 ][ 1 ] * In2 ;
	Out->m[ 0 ][ 2 ] = In1->m[ 0 ][ 2 ] * In2 ;
	Out->m[ 0 ][ 3 ] = In1->m[ 0 ][ 3 ] * In2 ;

	Out->m[ 1 ][ 0 ] = In1->m[ 1 ][ 0 ] * In2 ;
	Out->m[ 1 ][ 1 ] = In1->m[ 1 ][ 1 ] * In2 ;
	Out->m[ 1 ][ 2 ] = In1->m[ 1 ][ 2 ] * In2 ;
	Out->m[ 1 ][ 3 ] = In1->m[ 1 ][ 3 ] * In2 ;

	Out->m[ 2 ][ 0 ] = In1->m[ 2 ][ 0 ] * In2 ;
	Out->m[ 2 ][ 1 ] = In1->m[ 2 ][ 1 ] * In2 ;
	Out->m[ 2 ][ 2 ] = In1->m[ 2 ][ 2 ] * In2 ;
	Out->m[ 2 ][ 3 ] = In1->m[ 2 ][ 3 ] * In2 ;
}

__inline void UnSafeMatrix4X4CT_D_Eq_D_Mul_S( MATRIX_4X4CT_D *RST Out, MATRIX_4X4CT_D * RST In1, float In2 )
{
	Out->m[ 0 ][ 0 ] = In1->m[ 0 ][ 0 ] * In2 ;
	Out->m[ 0 ][ 1 ] = In1->m[ 0 ][ 1 ] * In2 ;
	Out->m[ 0 ][ 2 ] = In1->m[ 0 ][ 2 ] * In2 ;
	Out->m[ 0 ][ 3 ] = In1->m[ 0 ][ 3 ] * In2 ;

	Out->m[ 1 ][ 0 ] = In1->m[ 1 ][ 0 ] * In2 ;
	Out->m[ 1 ][ 1 ] = In1->m[ 1 ][ 1 ] * In2 ;
	Out->m[ 1 ][ 2 ] = In1->m[ 1 ][ 2 ] * In2 ;
	Out->m[ 1 ][ 3 ] = In1->m[ 1 ][ 3 ] * In2 ;

	Out->m[ 2 ][ 0 ] = In1->m[ 2 ][ 0 ] * In2 ;
	Out->m[ 2 ][ 1 ] = In1->m[ 2 ][ 1 ] * In2 ;
	Out->m[ 2 ][ 2 ] = In1->m[ 2 ][ 2 ] * In2 ;
	Out->m[ 2 ][ 3 ] = In1->m[ 2 ][ 3 ] * In2 ;
}

__inline void UnSafeMatrix4X4CT_C_Eq_C_Mul_S( MATRIX_4X4CT *RST Out, MATRIX_4X4CT * RST In1, float In2 )
{
	if( GSYS.DrawSetting.Large3DPositionSupport )
	{
		UnSafeMatrix4X4CT_D_Eq_D_Mul_S( &Out->md, &In1->md, In2 ) ;
	}
	else
	{
		UnSafeMatrix4X4CT_F_Eq_F_Mul_S( &Out->mf, &In1->mf, In2 ) ;
	}
}

__inline void UnSafeMatrix4X4CT_F_EqPlus_F_Mul_S( MATRIX_4X4CT_F *RST Out, MATRIX_4X4CT_F * RST In1, float In2 )
{
	Out->m[ 0 ][ 0 ] += In1->m[ 0 ][ 0 ] * In2 ;
	Out->m[ 0 ][ 1 ] += In1->m[ 0 ][ 1 ] * In2 ;
	Out->m[ 0 ][ 2 ] += In1->m[ 0 ][ 2 ] * In2 ;
	Out->m[ 0 ][ 3 ] += In1->m[ 0 ][ 3 ] * In2 ;

	Out->m[ 1 ][ 0 ] += In1->m[ 1 ][ 0 ] * In2 ;
	Out->m[ 1 ][ 1 ] += In1->m[ 1 ][ 1 ] * In2 ;
	Out->m[ 1 ][ 2 ] += In1->m[ 1 ][ 2 ] * In2 ;
	Out->m[ 1 ][ 3 ] += In1->m[ 1 ][ 3 ] * In2 ;

	Out->m[ 2 ][ 0 ] += In1->m[ 2 ][ 0 ] * In2 ;
	Out->m[ 2 ][ 1 ] += In1->m[ 2 ][ 1 ] * In2 ;
	Out->m[ 2 ][ 2 ] += In1->m[ 2 ][ 2 ] * In2 ;
	Out->m[ 2 ][ 3 ] += In1->m[ 2 ][ 3 ] * In2 ;
}

__inline void UnSafeMatrix4X4CT_D_EqPlus_D_Mul_S( MATRIX_4X4CT_D *RST Out, MATRIX_4X4CT_D * RST In1, float In2 )
{
	Out->m[ 0 ][ 0 ] += In1->m[ 0 ][ 0 ] * In2 ;
	Out->m[ 0 ][ 1 ] += In1->m[ 0 ][ 1 ] * In2 ;
	Out->m[ 0 ][ 2 ] += In1->m[ 0 ][ 2 ] * In2 ;
	Out->m[ 0 ][ 3 ] += In1->m[ 0 ][ 3 ] * In2 ;

	Out->m[ 1 ][ 0 ] += In1->m[ 1 ][ 0 ] * In2 ;
	Out->m[ 1 ][ 1 ] += In1->m[ 1 ][ 1 ] * In2 ;
	Out->m[ 1 ][ 2 ] += In1->m[ 1 ][ 2 ] * In2 ;
	Out->m[ 1 ][ 3 ] += In1->m[ 1 ][ 3 ] * In2 ;

	Out->m[ 2 ][ 0 ] += In1->m[ 2 ][ 0 ] * In2 ;
	Out->m[ 2 ][ 1 ] += In1->m[ 2 ][ 1 ] * In2 ;
	Out->m[ 2 ][ 2 ] += In1->m[ 2 ][ 2 ] * In2 ;
	Out->m[ 2 ][ 3 ] += In1->m[ 2 ][ 3 ] * In2 ;
}

__inline void UnSafeMatrix4X4CT_C_EqPlus_C_Mul_S( MATRIX_4X4CT *RST Out, MATRIX_4X4CT * RST In1, float In2 )
{
	if( GSYS.DrawSetting.Large3DPositionSupport )
	{
		UnSafeMatrix4X4CT_D_EqPlus_D_Mul_S( &Out->md, &In1->md, In2 ) ;
	}
	else
	{
		UnSafeMatrix4X4CT_F_EqPlus_F_Mul_S( &Out->mf, &In1->mf, In2 ) ;
	}
}

// ベクトル行列と4x4正方行列を乗算する( w は 1 と仮定 )
__inline void VectorTransform4X4CTF( VECTOR * RST Out, VECTOR * RST InVec, MATRIX_4X4CT_F * RST InMatrix )
{
	Out->x = InVec->x * InMatrix->m[ 0 ][ 0 ] + 
		     InVec->y * InMatrix->m[ 0 ][ 1 ] +
			 InVec->z * InMatrix->m[ 0 ][ 2 ] +
			            InMatrix->m[ 0 ][ 3 ] ;
	Out->y = InVec->x * InMatrix->m[ 1 ][ 0 ] + 
		     InVec->y * InMatrix->m[ 1 ][ 1 ] +
			 InVec->z * InMatrix->m[ 1 ][ 2 ] +
			            InMatrix->m[ 1 ][ 3 ] ;
	Out->z = InVec->x * InMatrix->m[ 2 ][ 0 ] + 
		     InVec->y * InMatrix->m[ 2 ][ 1 ] +
			 InVec->z * InMatrix->m[ 2 ][ 2 ] +
			            InMatrix->m[ 2 ][ 3 ] ;
}

__inline void VectorTransform4X4CTD( VECTOR * RST Out, VECTOR * RST InVec, MATRIX_4X4CT_D * RST InMatrix )
{
	Out->x = ( float )( 
		     InVec->x * InMatrix->m[ 0 ][ 0 ] + 
		     InVec->y * InMatrix->m[ 0 ][ 1 ] +
			 InVec->z * InMatrix->m[ 0 ][ 2 ] +
			            InMatrix->m[ 0 ][ 3 ] ) ;
	Out->y = ( float )(
		     InVec->x * InMatrix->m[ 1 ][ 0 ] + 
		     InVec->y * InMatrix->m[ 1 ][ 1 ] +
			 InVec->z * InMatrix->m[ 1 ][ 2 ] +
			            InMatrix->m[ 1 ][ 3 ] ) ;
	Out->z = ( float )( 
		     InVec->x * InMatrix->m[ 2 ][ 0 ] + 
		     InVec->y * InMatrix->m[ 2 ][ 1 ] +
			 InVec->z * InMatrix->m[ 2 ][ 2 ] +
			            InMatrix->m[ 2 ][ 3 ] ) ;
}

__inline void VectorTransform4X4CT( VECTOR * RST Out, VECTOR * RST InVec, MATRIX_4X4CT * RST InMatrix )
{
	if( GSYS.DrawSetting.Large3DPositionSupport )
	{
		VectorTransform4X4CTD( Out, InVec, &InMatrix->md ) ;
	}
	else
	{
		VectorTransform4X4CTF( Out, InVec, &InMatrix->mf ) ;
	}
}

// ベクトル行列と4x4正方行列の回転成分のみを乗算する( w は 1 と仮定 )
__inline void VectorTransformSR4X4CTF( VECTOR * RST Out, VECTOR * RST InVec, MATRIX_4X4CT_F * RST InMatrix )
{
	Out->x = InVec->x * InMatrix->m[ 0 ][ 0 ] + 
		     InVec->y * InMatrix->m[ 0 ][ 1 ] +
			 InVec->z * InMatrix->m[ 0 ][ 2 ] ;
	Out->y = InVec->x * InMatrix->m[ 1 ][ 0 ] + 
		     InVec->y * InMatrix->m[ 1 ][ 1 ] +
			 InVec->z * InMatrix->m[ 1 ][ 2 ] ;
	Out->z = InVec->x * InMatrix->m[ 2 ][ 0 ] + 
		     InVec->y * InMatrix->m[ 2 ][ 1 ] +
			 InVec->z * InMatrix->m[ 2 ][ 2 ] ;
}

__inline void VectorTransformSR4X4CTD( VECTOR * RST Out, VECTOR * RST InVec, MATRIX_4X4CT_D * RST InMatrix )
{
	Out->x = ( float )( 
		     InVec->x * InMatrix->m[ 0 ][ 0 ] + 
		     InVec->y * InMatrix->m[ 0 ][ 1 ] +
			 InVec->z * InMatrix->m[ 0 ][ 2 ] ) ;
	Out->y = ( float )(
		     InVec->x * InMatrix->m[ 1 ][ 0 ] + 
		     InVec->y * InMatrix->m[ 1 ][ 1 ] +
			 InVec->z * InMatrix->m[ 1 ][ 2 ] ) ;
	Out->z = ( float )( 
		     InVec->x * InMatrix->m[ 2 ][ 0 ] + 
		     InVec->y * InMatrix->m[ 2 ][ 1 ] +
			 InVec->z * InMatrix->m[ 2 ][ 2 ] ) ;
}

__inline void VectorTransformSR4X4CT( VECTOR * RST Out, VECTOR * RST InVec, MATRIX_4X4CT * RST InMatrix )
{
	if( GSYS.DrawSetting.Large3DPositionSupport )
	{
		VectorTransformSR4X4CTD( Out, InVec, &InMatrix->md ) ;
	}
	else
	{
		VectorTransformSR4X4CTF( Out, InVec, &InMatrix->mf ) ;
	}
}

// 環境依存関数
extern	int				MV1_Terminate_PF( void ) ;																	// モデル機能の後始末
extern	int				MV1_TerminateModelBaseHandle_PF( MV1_MODEL_BASE *ModelBase ) ;								// モデルデータハンドルの後始末
extern	int				MV1_TerminateTriangleListBaseTempBuffer_PF( MV1_TRIANGLE_LIST_BASE *MBTList ) ;				// トライアングルリストの一時処理用のバッファを開放する
extern	void			MV1_SetupPackDrawInfo_PF( MV1_MODEL_BASE *ModelBase ) ;										// 同時複数描画関係の情報をセットアップする
extern	int				MV1_SetupVertexBufferBase_PF( int MV1ModelBaseHandle, int DuplicateNum = 1, int ASyncThread = FALSE ) ;	// モデル基データの頂点バッファのセットアップをする( -1:エラー )
extern	int				MV1_SetupVertexBuffer_PF( int MHandle, int ASyncThread = FALSE ) ;							// モデルデータの頂点バッファのセットアップをする( -1:エラー )
extern	int				MV1_TerminateVertexBufferBase_PF( int MV1ModelBaseHandle ) ;								// 頂点バッファの後始末をする( -1:エラー )
extern	int				MV1_TerminateVertexBuffer_PF( int MV1ModelHandle ) ;										// 頂点バッファの後始末をする( -1:エラー )
extern	int				MV1_SetupShapeVertex_PF( int MHandle ) ;													// シェイプデータのセットアップをする
extern	int				MV1_BeginRender_PF( MV1_MODEL *Model ) ;													// ３Ｄモデルのレンダリングの準備を行う
extern	int				MV1_EndRender_PF() ;																		// ３Ｄモデルのレンダリングの後始末を行う
extern	void			MV1_DrawMesh_PF( MV1_MESH *Mesh, int TriangleListIndex = -1 ) ;								// メッシュ描画部分を抜き出したもの








// wchar_t版関数
extern	int				MV1LoadModel_WCHAR_T(						const wchar_t *FileName ) ;
extern	int				MV1SetLoadModelAnimFilePath_WCHAR_T(		const wchar_t *FileName ) ;
extern	int				MV1AddLoadModelDisablePhysicsNameWord_WCHAR_T( const wchar_t *Word ) ;
extern	int				MV1SaveModelToMV1File_WCHAR_T( int MHandle, const wchar_t *FileName, int SaveType = MV1_SAVETYPE_NORMAL , int AnimMHandle = -1 , int AnimNameCheck = TRUE , int Normal8BitFlag = 1 , int Position16BitFlag = 1 , int Weight8BitFlag = 0 , int Anim16BitFlag = 1 ) ;
#ifndef DX_NON_SAVEFUNCTION
extern	int				MV1SaveModelToXFile_WCHAR_T(   int MHandle, const wchar_t *FileName, int SaveType = MV1_SAVETYPE_NORMAL , int AnimMHandle = -1 , int AnimNameCheck = TRUE ) ;
#endif // DX_NON_SAVEFUNCTION
extern	const wchar_t *	MV1GetAnimName_WCHAR_T(					int MHandle, int AnimIndex ) ;
extern	int				MV1SetAnimName_WCHAR_T(					int MHandle, int AnimIndex, const wchar_t *AnimName ) ;
extern	int				MV1GetAnimIndex_WCHAR_T(				int MHandle, const wchar_t *AnimName ) ;
extern	const wchar_t *	MV1GetAnimTargetFrameName_WCHAR_T(		int MHandle, int AnimIndex, int AnimFrameIndex ) ;
extern	const wchar_t *	MV1GetMaterialName_WCHAR_T(				int MHandle, int MaterialIndex ) ;
extern	const wchar_t *	MV1GetTextureName_WCHAR_T(				int MHandle, int TexIndex ) ;
extern	int				MV1SetTextureColorFilePath_WCHAR_T(		int MHandle, int TexIndex, const wchar_t *FilePath ) ;
extern	const wchar_t *	MV1GetTextureColorFilePath_WCHAR_T(		int MHandle, int TexIndex ) ;
extern	int				MV1SetTextureAlphaFilePath_WCHAR_T(		int MHandle, int TexIndex, const wchar_t *FilePath ) ;
extern	const wchar_t *	MV1GetTextureAlphaFilePath_WCHAR_T(		int MHandle, int TexIndex ) ;
extern	int				MV1LoadTexture_WCHAR_T(					const wchar_t *FilePath ) ;
extern	int				MV1SearchFrame_WCHAR_T(					int MHandle, const wchar_t *FrameName ) ;
extern	int				MV1SearchFrameChild_WCHAR_T(			int MHandle, int FrameIndex = -1 , const wchar_t *ChildName = NULL ) ;
extern	const wchar_t *	MV1GetFrameName_WCHAR_T(				int MHandle, int FrameIndex ) ;
extern	int				MV1GetFrameName2_WCHAR_T(				int MHandle, int FrameIndex, wchar_t *StrBuffer ) ;
extern	int				MV1SearchShape_WCHAR_T(					int MHandle, const wchar_t *ShapeName ) ;
extern	const wchar_t *	MV1GetShapeName_WCHAR_T(				int MHandle, int ShapeIndex ) ;



















#ifdef DX_THREAD_SAFE

// モデルの読み込み・保存・複製関係
extern	int			NS_MV1LoadModel( const TCHAR *FileName ) ;												// モデルの読み込み( -1:エラー  0以上:モデルハンドル )
extern	int			NS_MV1LoadModelWithStrLen(				const TCHAR *FileName, size_t FileNameLength ) ;					// モデルの読み込み( -1:エラー  0以上:モデルハンドル )
extern	int			NS_MV1LoadModelFromMem( const void *FileImage, int FileSize, int (* FileReadFunc )( const TCHAR *FilePath, void **FileImageAddr, int *FileSize, void *FileReadFuncData ), int (* FileReleaseFunc )( void *MemoryAddr, void *FileReadFuncData ), void *FileReadFuncData = NULL ) ;	// メモリ上のモデルファイルイメージと独自の読み込みルーチンを使用してモデルを読み込む
extern	int			NS_MV1DeleteModel( int MHandle ) ;														// モデルを削除する
extern	int			NS_MV1InitModel( void ) ;																		// すべてのモデルを削除する
extern	int			NS_MV1CreateCloneModel( int SrcMHandle ) ;														// 指定のモデルと全く同じ情報を持つ別のﾓﾃﾞﾙデータハンドルを作成する( -1:エラー  0以上:モデルハンドル )
extern	int			NS_MV1DuplicateModel( int SrcMHandle ) ;												// 指定のモデルと同じモデル基本データを使用してモデルを作成する( -1:エラー  0以上:モデルハンドル )
extern	int			NS_MV1SetLoadModelReMakeNormal( int Flag ) ;											// モデルを読み込む際に法線の再計算を行うかどうかを設定する( TRUE:行う  FALSE:行わない )
extern	int			NS_MV1SetLoadModelReMakeNormalSmoothingAngle( float SmoothingAngle = 89.5f * DX_PI_F / 180.0f ) ;	// モデルを読み込む際に行う法泉の再計算で使用するスムージング角度を設定する( 単位はラジアン )
extern	int			NS_MV1SetLoadModelIgnoreScaling(       int Flag ) ;														// モデルを読み込む際にスケーリングデータを無視するかどうかを設定する( TRUE:無視する  FALSE:無視しない( デフォルト ) )
extern	int			NS_MV1SetLoadModelPositionOptimize( int Flag ) ;										// モデルを読み込む際に座標データの最適化を行うかどうかを設定する( TRUE:行う  FALSE:行わない )
extern	int			NS_MV1SetLoadModelNotEqNormalSide_AddZeroAreaPolygon( int Flag ) ;											// モデルを読み込む際にポリゴンの辺が接していて、且つ法線の方向が異なる辺に面積０のポリゴンを埋め込むかどうかを設定する( TRUE:埋め込む　FALSE:埋め込まない( デフォルト ) )、( MV1ファイルの読み込みではこの関数の設定は無視され、ポリゴンの埋め込みは実行されません )
extern	int			NS_MV1SetLoadModelUsePhysicsMode( int PhysicsMode /* DX_LOADMODEL_PHYSICS_LOADCALC 等 */ ) ;	// 読み込むモデルの物理演算モードを設定する
extern	int			NS_MV1SetLoadModelPhysicsWorldGravity( float Gravity ) ;										// 読み込むモデルの物理演算に適用する重力パラメータ
extern	float		NS_MV1GetLoadModelPhysicsWorldGravity( void ) ;															// 読み込むモデルの物理演算に適用する重力パラメータを取得する
extern	int			NS_MV1SetLoadCalcPhysicsWorldGravity( int GravityNo, VECTOR Gravity ) ;						// 読み込むモデルの物理演算モードが事前計算( DX_LOADMODEL_PHYSICS_LOADCALC )だった場合に適用される重力の設定をする
extern	VECTOR		NS_MV1GetLoadCalcPhysicsWorldGravity( int GravityNo ) ;													// 読み込むモデルの物理演算モードが事前計算( DX_LOADMODEL_PHYSICS_LOADCALC )だった場合に適用される重力を取得する
extern	int			NS_MV1SetLoadModelPhysicsCalcPrecision( int Precision ) ;												// 読み込むモデルの物理演算モードが事前計算( DX_LOADMODEL_PHYSICS_LOADCALC )だった場合に適用される物理演算の時間進行の精度を設定する( 0:60FPS  1:120FPS  2:240FPS  3:480FPS  4:960FPS  5:1920FPS )
extern	int			NS_MV1SetLoadModel_PMD_PMX_AnimationFPSMode( int FPSMode /* DX_LOADMODEL_PMD_PMX_ANIMATION_FPSMODE_30 等 */ ) ; // PMD, PMX ファイルを読み込んだ際のアニメーションの FPS モードを設定する
extern	int			NS_MV1AddLoadModelDisablePhysicsNameWord( const TCHAR *NameWord ) ;											// 読み込むモデルの物理演算を特定の剛体のみ無効にするための名前のワードを追加する
extern	int			NS_MV1AddLoadModelDisablePhysicsNameWordWithStrLen(	const TCHAR *NameWord, size_t NameWordLength ) ;					// 読み込むモデルの物理演算を特定の剛体のみ無効にするための名前のワードを追加する、追加できるワード文字列の最大長は 63 文字、追加できるワードの数は最大 256 個
extern	int			NS_MV1ResetLoadModelDisablePhysicsNameWord( void ) ;														// MV1AddLoadModelDisablePhysicsNameWord で追加した剛体の無効ワードをリセットして無効ワード無しの初期状態に戻す
extern	int			NS_MV1SetLoadModelDisablePhysicsNameWordMode( int DisableNameWordMode /* DX_LOADMODEL_PHYSICS_DISABLENAMEWORD_ALWAYS 等 */ ) ;	// MV1AddLoadModelDisablePhysicsNameWord で追加した剛体の無効ワードの適用ルールを変更する
extern	int			NS_MV1SetLoadModelAnimFilePath( const TCHAR *FileName ) ;										// 読み込むモデルに適用するアニメーションファイルのパスを設定する( 現在は PMD,PMX のみに効果あり )
extern	int			NS_MV1SetLoadModelAnimFilePathWithStrLen(				const TCHAR *FileName, size_t FileNameLength ) ;					// 読み込むモデルに適用するアニメーションファイルのパスを設定する、NULLを渡すと設定リセット( 現在は PMD,PMX のみに効果あり )
extern	int			NS_MV1SetLoadModelUsePackDraw(			int Flag ) ;														// 読み込むモデルを同時複数描画に対応させるかどうかを設定する( TRUE:対応させる  FALSE:対応させない( デフォルト ) )、( 「対応させる」にすると描画が高速になる可能性がある代わりに消費VRAMが増えます )
extern	int			NS_MV1SetLoadModelTriangleListUseMaxBoneNum( int UseMaxBoneNum ) ;											// 読み込むモデルのひとつのトライアングルリストで使用できる最大ボーン数を設定する( UseMaxBoneNum で指定できる値の範囲は 8 〜 54、 0 を指定するとデフォルト動作に戻る )

// モデル保存関係
extern	int			NS_MV1SaveModelToMV1File( int MHandle, const TCHAR *FileName, int SaveType = MV1_SAVETYPE_NORMAL , int AnimMHandle = -1 , int AnimNameCheck = TRUE , int Normal8BitFlag = 1 , int Position16BitFlag = 1 , int Weight8BitFlag = 0 , int Anim16BitFlag = 1 ) ;		// 指定のパスにモデルを保存する( 戻り値  0:成功  -1:メモリ不足  -2:使われていないアニメーションがあった )
extern	int			NS_MV1SaveModelToMV1FileWithStrLen(	int MHandle, const TCHAR *FileName, size_t FileNameLength, int SaveType = MV1_SAVETYPE_NORMAL , int AnimMHandle = -1 , int AnimNameCheck = TRUE , int Normal8BitFlag = 1 , int Position16BitFlag = 1 , int Weight8BitFlag = 0 , int Anim16BitFlag = 1 ) ;		// 指定のパスにモデルを保存する( 戻り値  0:成功  -1:メモリ不足  -2:使われていないアニメーションがあった )
#ifndef DX_NON_SAVEFUNCTION
extern	int			NS_MV1SaveModelToXFile(   int MHandle, const TCHAR *FileName, int SaveType = MV1_SAVETYPE_NORMAL , int AnimMHandle = -1 , int AnimNameCheck = TRUE ) ;	// 指定のパスにモデルをＸファイル形式で保存する( 戻り値  0:成功  -1:メモリ不足  -2:使われていないアニメーションがあった )
extern	int			NS_MV1SaveModelToXFileWithStrLen(		int MHandle, const TCHAR *FileName, size_t FileNameLength, int SaveType = MV1_SAVETYPE_NORMAL , int AnimMHandle = -1 , int AnimNameCheck = TRUE ) ;	// 指定のパスにモデルをＸファイル形式で保存する( 戻り値  0:成功  -1:メモリ不足  -2:使われていないアニメーションがあった )
#endif // DX_NON_SAVEFUNCTION

// モデル描画関係
extern	int			NS_MV1DrawModel( int MHandle ) ;														// モデルを描画する
extern	int			NS_MV1DrawFrame( int MHandle, int FrameIndex ) ;										// モデルの指定のフレームを描画する
extern	int			NS_MV1DrawMesh( int MHandle, int MeshIndex ) ;											// モデルの指定のメッシュを描画する
extern	int			NS_MV1DrawTriangleList( int MHandle, int TriangleListIndex ) ;									// モデルの指定のトライアングルリストを描画する
extern	int			NS_MV1DrawModelDebug( int MHandle, unsigned int Color, int IsNormalLine, float NormalLineLength, int IsPolyLine, int IsCollisionBox ) ;	// モデルのデバッグ描画
//extern	int			NS_MV1DrawAlphaObject( void ) ;														// アルファオブジェクトの描画

// 描画設定関係
extern	int			NS_MV1SetUseOrigShader( int UseFlag ) ;														// モデルの描画に SetUseVertexShader, SetUsePixelShader で指定したシェーダーを使用するかどうかを設定する( TRUE:使用する  FALSE:使用しない( デフォルト ) )
extern	int			NS_MV1SetSemiTransDrawMode(			int DrawMode /* DX_SEMITRANSDRAWMODE_ALWAYS 等 */ ) ;				// モデルの半透明要素がある部分についての描画モードを設定する

// モデル基本制御関係
//extern	int			   MV1SetupMatrix( int MHandle ) ;														// 描画用の行列を構築する
extern	MATRIX		NS_MV1GetLocalWorldMatrix( int MHandle ) ;												// モデルのローカル座標からワールド座標に変換する行列を得る
extern	MATRIX_D	NS_MV1GetLocalWorldMatrixD( int MHandle ) ;												// モデルのローカル座標からワールド座標に変換する行列を得る
extern	int			NS_MV1SetPosition(  int MHandle, VECTOR   Position ) ;									// モデルの座標をセット
extern	int			NS_MV1SetPositionD( int MHandle, VECTOR_D Position ) ;									// モデルの座標をセット
extern	VECTOR		NS_MV1GetPosition(  int MHandle ) ;														// モデルの座標を取得
extern	VECTOR_D	NS_MV1GetPositionD( int MHandle ) ;														// モデルの座標を取得
extern	int			NS_MV1SetScale( int MHandle, VECTOR Scale ) ;											// モデルの拡大値をセット
extern	VECTOR		NS_MV1GetScale( int MHandle ) ;															// モデルの拡大値を取得
extern	int			NS_MV1SetRotationXYZ( int MHandle, VECTOR Rotate ) ;									// モデルの回転値をセット( X軸回転→Y軸回転→Z軸回転方式 )
extern	VECTOR		NS_MV1GetRotationXYZ( int MHandle ) ;													// モデルの回転値を取得( X軸回転→Y軸回転→Z軸回転方式 )
extern	int			NS_MV1SetRotationZYAxis( int MHandle, VECTOR ZAxisDirection, VECTOR YAxisDirection, float ZAxisTwistRotate ) ;	// モデルのＺ軸とＹ軸の向きをセットする
extern	int			NS_MV1SetRotationYUseDir( int MHandle, VECTOR Direction, float OffsetYAngle ) ;			// モデルのＹ軸の回転値を指定のベクトルの向きを元に設定する、モデルはZ軸のマイナス方向を向いていることを想定するので、そうではない場合は OffsetYAngle で補正する、Ｘ軸回転、Ｚ軸回転は０で固定
extern	int			NS_MV1SetRotationMatrix( int MHandle, MATRIX Matrix ) ;									// モデルの回転用行列をセットする
extern	MATRIX		NS_MV1GetRotationMatrix( int MHandle ) ;												// モデルの回転用行列を取得する
extern	int			NS_MV1SetMatrix(  int MHandle, MATRIX   Matrix ) ;										// モデルの変形用行列をセットする
extern	int			NS_MV1SetMatrixD( int MHandle, MATRIX_D Matrix ) ;										// モデルの変形用行列をセットする
extern	MATRIX		NS_MV1GetMatrix(  int MHandle ) ;														// モデルの変形用行列を取得する
extern	MATRIX_D	NS_MV1GetMatrixD( int MHandle ) ;														// モデルの変形用行列を取得する
extern	int			NS_MV1SetVisible( int MHandle, int VisibleFlag ) ;										// モデルの表示、非表示状態を変更する( TRUE:表示  FALSE:非表示 )
extern	int			NS_MV1GetVisible( int MHandle ) ;														// モデルの表示、非表示状態を取得する( TRUE:表示  FALSE:非表示 )
extern	int			NS_MV1SetMeshCategoryVisible( int MHandle, int MeshCategory, int VisibleFlag ) ;		// モデルのメッシュの種類( DX_MV1_MESHCATEGORY_NORMAL など )毎の表示、非表示を設定する( TRUE:表示  FALSE:非表示 )
extern	int			NS_MV1GetMeshCategoryVisible( int MHandle, int MeshCategory ) ;							// モデルのメッシュの種類( DX_MV1_MESHCATEGORY_NORMAL など )毎の表示、非表示を取得する( TRUE:表示  FALSE:非表示 )
extern	int			NS_MV1SetDifColorScale( int MHandle, COLOR_F Scale ) ;									// モデルのディフューズカラーのスケール値を設定する( デフォルト値は 1.0f )
extern	COLOR_F		NS_MV1GetDifColorScale( int MHandle ) ;													// モデルのディフューズカラーのスケール値を取得する( デフォルト値は 1.0f )
extern	int			NS_MV1SetSpcColorScale( int MHandle, COLOR_F Scale ) ;									// モデルのスペキュラカラーのスケール値を設定する( デフォルト値は 1.0f )
extern	COLOR_F		NS_MV1GetSpcColorScale( int MHandle ) ;													// モデルのスペキュラカラーのスケール値を取得する( デフォルト値は 1.0f )
extern	int			NS_MV1SetEmiColorScale( int MHandle, COLOR_F Scale ) ;									// モデルのエミッシブカラーのスケール値を設定する( デフォルト値は 1.0f )
extern	COLOR_F		NS_MV1GetEmiColorScale( int MHandle ) ;													// モデルのエミッシブカラーのスケール値を取得する( デフォルト値は 1.0f )
extern	int			NS_MV1SetAmbColorScale( int MHandle, COLOR_F Scale ) ;									// モデルのアンビエントカラーのスケール値を設定する( デフォルト値は 1.0f )
extern	COLOR_F		NS_MV1GetAmbColorScale( int MHandle ) ;													// モデルのアンビエントカラーのスケール値を取得する( デフォルト値は 1.0f )
extern	int			NS_MV1GetSemiTransState( int MHandle ) ;												// モデルに半透明要素があるかどうかを取得する( 戻り値 TRUE:ある  FALSE:ない )
extern	int			NS_MV1SetOpacityRate( int MHandle, float Rate ) ;										// モデルの不透明度を設定する( 不透明 1.0f 〜 透明 0.0f )
extern	float		NS_MV1GetOpacityRate( int MHandle ) ;													// モデルの不透明度を取得する( 不透明 1.0f 〜 透明 0.0f )
extern	int			NS_MV1SetUseDrawMulAlphaColor(			int MHandle, int Flag ) ;											// モデルを描画する際にRGB値に対してA値を乗算するかどうかを設定する( 描画結果が乗算済みアルファ画像になります )( Flag   TRUE:RGB値に対してA値を乗算する  FALSE:乗算しない(デフォルト) )
extern	int			NS_MV1GetUseDrawMulAlphaColor(			int MHandle ) ;														// モデルを描画する際にRGB値に対してA値を乗算するかどうかを取得する( 描画結果が乗算済みアルファ画像になります )( 戻り値 TRUE:RGB値に対してA値を乗算する  FALSE:乗算しない(デフォルト) )
extern	int			NS_MV1SetUseZBuffer( int MHandle, int Flag ) ;											// モデルを描画する際にＺバッファを使用するかどうかを設定する
extern	int			NS_MV1SetWriteZBuffer( int MHandle, int Flag ) ;										// モデルを描画する際にＺバッファに書き込みを行うかどうかを設定する
extern	int			NS_MV1SetZBufferCmpType( int MHandle, int CmpType /* DX_CMP_NEVER 等 */ ) ;				// モデルの描画時のＺ値の比較モードを設定する
extern	int			NS_MV1SetZBias( int MHandle, int Bias ) ;												// モデルの描画時の書き込むＺ値のバイアスを設定する
extern	int			NS_MV1SetUseVertDifColor( int MHandle, int UseFlag ) ;									// モデルの含まれるメッシュの頂点ディフューズカラーをマテリアルのディフューズカラーの代わりに使用するかどうかを設定する( TRUE:マテリアルカラーの代わりに使用する  FALSE:マテリアルカラーを使用する )
extern	int			NS_MV1SetUseVertSpcColor( int MHandle, int UseFlag ) ;									// モデルに含まれるメッシュの頂点スペキュラカラーをマテリアルのスペキュラカラーの代わりに使用するかどうかを設定する( TRUE:マテリアルカラーの代わりに使用する  FALSE:マテリアルカラーを使用する )
extern	int			NS_MV1SetSampleFilterMode( int MHandle, int FilterMode ) ;								// モデルのサンプルフィルターモードを変更する
extern	int			NS_MV1SetMaxAnisotropy( int MHandle, int MaxAnisotropy ) ;								// モデルの異方性フィルタリングの最大次数を設定する
extern	int			NS_MV1SetWireFrameDrawFlag( int MHandle, int Flag ) ;									// モデルをワイヤーフレームで描画するかどうかを設定する
extern	int			NS_MV1RefreshVertColorFromMaterial( int MHandle ) ;										// モデルの頂点カラーを現在設定されているマテリアルのカラーにする
extern	int			NS_MV1SetPhysicsWorldGravity(			int MHandle, VECTOR Gravity ) ;										// モデルの物理演算の重力を設定する
extern	int			NS_MV1PhysicsCalculation( int MHandle, float MillisecondTime ) ;						// モデルの物理演算を指定時間分経過したと仮定して計算する( MillisecondTime で指定する時間の単位はミリ秒 )
extern	int			NS_MV1PhysicsResetState( int MHandle ) ;												// モデルの物理演算の状態をリセットする( 位置がワープしたとき用 )
extern	int			NS_MV1SetUseShapeFlag( int MHandle, int Flag ) ;										// モデルのシェイプ機能を使用するかどうかを設定する
extern	int			NS_MV1GetMaterialNumberOrderFlag(		int MHandle ) ;														// モデルのマテリアル番号順にメッシュを描画するかどうかのフラグを取得する( TRUE:マテリアル番号順に描画  FALSE:不透明メッシュの後半透明メッシュ )


// アニメーション関係
extern	int			NS_MV1AttachAnim( int MHandle, int AnimIndex, int AnimSrcMHandle = -1, int NameCheck = TRUE ) ;		// アニメーションをアタッチする( 戻り値  -1:エラー  0以上:アタッチインデックス )
extern	int			NS_MV1DetachAnim( int MHandle, int AttachIndex ) ;										// アニメーションをデタッチする
//extern	int			NS_MV1PlayAnim( int MHandle, int AttachIndex, int Loop ) ;							// アニメーションを再生する
//extern	int			NS_MV1AnimAddTime( int MHandle, int AttachIndex, float AddTime ) ;					// アニメーションを進める
extern	int			NS_MV1SetAttachAnimTime( int MHandle, int AttachIndex, float Time ) ;					// アタッチしているアニメーションの再生時間を設定する
extern	float		NS_MV1GetAttachAnimTime( int MHandle, int AttachIndex ) ;								// アタッチしているアニメーションの再生時間を取得する
extern	float		NS_MV1GetAttachAnimTotalTime( int MHandle, int AttachIndex ) ;							// アタッチしているアニメーションの総時間を得る
extern	int			NS_MV1SetAttachAnimBlendRate( int MHandle, int AttachIndex, float Rate = 1.0f ) ;		// アタッチしているアニメーションのブレンド率を設定する
extern	float		NS_MV1GetAttachAnimBlendRate( int MHandle, int AttachIndex ) ;							// アタッチしているアニメーションのブレンド率を取得する
extern	int			NS_MV1SetAttachAnimBlendRateToFrame( int MHandle, int AttachIndex, int FrameIndex, float Rate, int SetChild ) ;	// アタッチしているアニメーションのブレンド率を設定する( フレーム単位 )
extern	float		NS_MV1GetAttachAnimBlendRateToFrame( int MHandle, int AttachIndex, int FrameIndex ) ;			// アタッチしているアニメーションのブレンド率を設定する( フレーム単位 )
extern	int			NS_MV1GetAttachAnim( int MHandle, int AttachIndex ) ;									// アタッチしているアニメーションのアニメーションインデックスを取得する
extern	int			NS_MV1SetAttachAnimUseShapeFlag( int MHandle, int AttachIndex, int UseFlag ) ;			// アタッチしているアニメーションのシェイプを使用するかどうかを設定する( UseFlag  TRUE:使用する( デフォルト )  FALSE:使用しない )
extern	int			NS_MV1GetAttachAnimUseShapeFlag( int MHandle, int AttachIndex ) ;						// アタッチしているアニメーションのシェイプを使用するかどうかを取得する
extern	VECTOR		NS_MV1GetAttachAnimFrameLocalPosition( int MHandle, int AttachIndex, int FrameIndex ) ;									// アタッチしているアニメーションの指定のフレームの現在のローカル座標を取得する
extern	MATRIX		NS_MV1GetAttachAnimFrameLocalMatrix( int MHandle, int AttachIndex, int FrameIndex ) ;									// アタッチしているアニメーションの指定のフレームの現在のローカル変換行列を取得する

//extern	int			NS_MV1StopAnim( int MHandle, int AttachIndex ) ;									// アニメーションを止める
//extern	int			NS_MV1GetAnimState( int MHandle, int AttachIndex ) ;								// アニメーションが再生中かどうかを取得する( TRUE:再生中  FALSE:停止中 )
extern	int			NS_MV1GetAnimNum( int MHandle ) ;														// アニメーションの数を取得する
extern	const TCHAR *NS_MV1GetAnimName( int MHandle, int AnimIndex ) ;										// 指定番号のアニメーション名を取得する( NULL:エラー )
extern	int			NS_MV1SetAnimName( int MHandle, int AnimIndex, const TCHAR *AnimName ) ;										// 指定番号のアニメーション名を変更する
extern	int			NS_MV1SetAnimNameWithStrLen(			int MHandle, int AnimIndex, const TCHAR *AnimName, size_t AnimNameLength ) ;		// 指定番号のアニメーション名を変更する
extern	int			NS_MV1GetAnimIndex(					int MHandle, const TCHAR *AnimName                        ) ;						// 指定名のアニメーション番号を取得する( -1:エラー )
extern	int			NS_MV1GetAnimIndexWithStrLen(			int MHandle, const TCHAR *AnimName, size_t AnimNameLength ) ;						// 指定名のアニメーション番号を取得する( -1:エラー )
extern	float		NS_MV1GetAnimTotalTime( int MHandle, int AnimIndex ) ;									// 指定番号のアニメーションの総時間を得る
extern	int			NS_MV1GetAnimLoopFlag(					int MHandle, int AnimIndex ) ;														// 指定のアニメーションがループタイプかどうかを取得する( 戻り値  TRUE:ループタイプ  FALSE:通常タイプ )
extern	int			NS_MV1GetAnimTargetFrameNum( int MHandle, int AnimIndex ) ;								// 指定のアニメーションがターゲットとするフレームの数を取得する
extern	const TCHAR *NS_MV1GetAnimTargetFrameName( int MHandle, int AnimIndex, int AnimFrameIndex ) ;		// 指定のアニメーションがターゲットとするフレームの名前を取得する
extern	int			NS_MV1GetAnimTargetFrame( int MHandle, int AnimIndex, int AnimFrameIndex ) ;			// 指定のアニメーションがターゲットとするフレームの番号を取得する
extern	int			NS_MV1GetAnimTargetFrameKeySetNum( int MHandle, int AnimIndex, int AnimFrameIndex ) ;	// 指定のアニメーションがターゲットとするフレーム用のアニメーションキーセットの数を取得する
extern	int			NS_MV1GetAnimTargetFrameKeySet( int MHandle, int AnimIndex, int AnimFrameIndex, int Index ) ;	// 指定のアニメーションがターゲットとするフレーム用のアニメーションキーセットキーセットインデックスを取得する

extern	int			NS_MV1GetAnimKeySetNum( int MHandle ) ;													// モデルに含まれるアニメーションキーセットの総数を得る
extern	int			NS_MV1GetAnimKeySetType( int MHandle, int AnimKeySetIndex ) ;							// 指定のアニメーションキーセットのタイプを取得する( MV1_ANIMKEY_TYPE_QUATERNION 等 )
extern	int			NS_MV1GetAnimKeySetDataType( int MHandle, int AnimKeySetIndex ) ;						// 指定のアニメーションキーセットのデータタイプを取得する( MV1_ANIMKEY_DATATYPE_ROTATE 等 )
//extern	float		NS_MV1GetAnimKeySetTotalTime( int MHandle, int AnimKeySetIndex ) ;					// 指定のアニメーションキーセットの総時間を取得する
extern	int			NS_MV1GetAnimKeySetTimeType( int MHandle, int AnimKeySetIndex ) ;						// 指定のアニメーションキーセットのキーの時間データタイプを取得する( MV1_ANIMKEY_TIME_TYPE_ONE 等 )
extern	int			NS_MV1GetAnimKeySetDataNum( int MHandle, int AnimKeySetIndex ) ;						// 指定のアニメーションキーセットのキーの数を取得する
extern	float		NS_MV1GetAnimKeyDataTime( int MHandle, int AnimKeySetIndex, int Index ) ;				// 指定のアニメーションキーセットのキーの時間を取得する
extern	int			NS_MV1GetAnimKeyDataIndexFromTime( int MHandle, int AnimKeySetIndex, float Time ) ;									// 指定のアニメーションキーセットの指定の時間でのキーの番号を取得する
extern	FLOAT4		NS_MV1GetAnimKeyDataToQuaternion( int MHandle, int AnimKeySetIndex, int Index ) ;		// 指定のアニメーションキーセットのキーを取得する、キータイプが MV1_ANIMKEY_TYPE_QUATERNION では無かった場合は失敗する
extern	FLOAT4		NS_MV1GetAnimKeyDataToQuaternionFromTime( int MHandle, int AnimKeySetIndex, float Time ) ;	// 指定のアニメーションキーセットのキーを取得する、キータイプが MV1_ANIMKEY_TYPE_QUATERNION では無かった場合は失敗する( 時間指定版 )
extern	VECTOR		NS_MV1GetAnimKeyDataToVector( int MHandle, int AnimKeySetIndex, int Index ) ;			// 指定のアニメーションキーセットのキーを取得する、キータイプが MV1_ANIMKEY_TYPE_VECTOR では無かった場合は失敗する
extern	VECTOR		NS_MV1GetAnimKeyDataToVectorFromTime( int MHandle, int AnimKeySetIndex, float Time ) ;	// 指定のアニメーションキーセットのキーを取得する、キータイプが MV1_ANIMKEY_TYPE_VECTOR では無かった場合は失敗する( 時間指定版 )
extern	MATRIX		NS_MV1GetAnimKeyDataToMatrix( int MHandle, int AnimKeySetIndex, int Index ) ;			// 指定のアニメーションキーセットのキーを取得する、キータイプが MV1_ANIMKEY_TYPE_MATRIX4X4C か MV1_ANIMKEY_TYPE_MATRIX3X3 では無かった場合は失敗する
extern	MATRIX		NS_MV1GetAnimKeyDataToMatrixFromTime( int MHandle, int AnimKeySetIndex, float Time ) ;	// 指定のアニメーションキーセットのキーを取得する、キータイプが MV1_ANIMKEY_TYPE_MATRIX4X4C か MV1_ANIMKEY_TYPE_MATRIX3X3 では無かった場合は失敗する( 時間指定版 )
extern	float		NS_MV1GetAnimKeyDataToFlat( int MHandle, int AnimKeySetIndex, int Index ) ;				// 指定のアニメーションキーセットのキーを取得する、キータイプが MV1_ANIMKEY_TYPE_FLAT では無かった場合は失敗する
extern	float		NS_MV1GetAnimKeyDataToFlatFromTime( int MHandle, int AnimKeySetIndex, float Time ) ;	// 指定のアニメーションキーセットのキーを取得する、キータイプが MV1_ANIMKEY_TYPE_FLAT では無かった場合は失敗する( 時間指定版 )
extern	float		NS_MV1GetAnimKeyDataToLinear( int MHandle, int AnimKeySetIndex, int Index ) ;			// 指定のアニメーションキーセットのキーを取得する、キータイプが MV1_ANIMKEY_TYPE_LINEAR では無かった場合は失敗する
extern	float		NS_MV1GetAnimKeyDataToLinearFromTime( int MHandle, int AnimKeySetIndex, float Time ) ;	// 指定のアニメーションキーセットのキーを取得する、キータイプが MV1_ANIMKEY_TYPE_LINEAR では無かった場合は失敗する( 時間指定版 )
//extern	int			NS_MV1GetAnimKeyDataToBlendKeyType( int MHandle, int AnimKeySetIndex, int Index ) ;	// 指定のアニメーションキーセットのキーのキータイプを取得する、キータイプが MV1_ANIMKEY_TYPE_BLEND では無かった場合は失敗する
//extern	float		NS_MV1GetAnimKeyDataToBlendValue( int MHandle, int AnimKeySetIndex, int Index ) ;	// 指定のアニメーションキーセットのキーの値を取得する、キータイプが MV1_ANIMKEY_TYPE_BLEND では無かった場合は失敗する

// マテリアル関係
extern	int			NS_MV1GetMaterialNum( int MHandle ) ;														// モデルで使用しているマテリアルの数を取得する
extern	const TCHAR *NS_MV1GetMaterialName( int MHandle, int MaterialIndex ) ;									// 指定のマテリアルの名前を取得する
extern	int			NS_MV1SetMaterialType( int MHandle, int MaterialIndex, int Type ) ;							// 指定のマテリアルのタイプを変更する( Type : DX_MATERIAL_TYPE_NORMAL など )
extern	int			NS_MV1GetMaterialType( int MHandle, int MaterialIndex ) ;									// 指定のマテリアルのタイプを取得する( 戻り値 : DX_MATERIAL_TYPE_NORMAL など )
extern	int			NS_MV1SetMaterialTypeParam(			int MHandle, int MaterialIndex, ... ) ;									// 指定のマテリアルのタイプ別パラメータを変更する( マテリアルタイプ DX_MATERIAL_TYPE_MAT_SPEC_LUMINANCE_TWO_COLOR などで使用 )
extern	int			NS_MV1SetMaterialDifColor( int MHandle, int MaterialIndex, COLOR_F Color ) ;				// 指定のマテリアルのディフューズカラーを設定する
extern	COLOR_F		NS_MV1GetMaterialDifColor( int MHandle, int MaterialIndex ) ;								// 指定のマテリアルのディフューズカラーを取得する
extern	int			NS_MV1SetMaterialSpcColor( int MHandle, int MaterialIndex, COLOR_F Color ) ;				// 指定のマテリアルのスペキュラカラーを設定する
extern	COLOR_F		NS_MV1GetMaterialSpcColor( int MHandle, int MaterialIndex ) ;								// 指定のマテリアルのスペキュラカラーを取得する
extern	int			NS_MV1SetMaterialEmiColor( int MHandle, int MaterialIndex, COLOR_F Color ) ;				// 指定のマテリアルのエミッシブカラーを設定する
extern	COLOR_F		NS_MV1GetMaterialEmiColor( int MHandle, int MaterialIndex ) ;								// 指定のマテリアルのエミッシブカラーを取得する
extern	int			NS_MV1SetMaterialAmbColor( int MHandle, int MaterialIndex, COLOR_F Color ) ;				// 指定のマテリアルのアンビエントカラーを設定する
extern	COLOR_F		NS_MV1GetMaterialAmbColor( int MHandle, int MaterialIndex ) ;								// 指定のマテリアルのアンビエントカラーを取得する
extern	int			NS_MV1SetMaterialSpcPower( int MHandle, int MaterialIndex, float Power ) ;					// 指定のマテリアルのスペキュラの強さを設定する
extern	float		NS_MV1GetMaterialSpcPower( int MHandle, int MaterialIndex ) ;								// 指定のマテリアルのスペキュラの強さを取得する
extern	int			NS_MV1SetMaterialDifMapTexture( int MHandle, int MaterialIndex, int TexIndex ) ;			// 指定のマテリアルでディフューズマップとして使用するテクスチャを指定する
extern	int			NS_MV1GetMaterialDifMapTexture( int MHandle, int MaterialIndex ) ;							// 指定のマテリアルでディフューズマップとして使用されているテクスチャのインデックスを取得する
extern	int			NS_MV1SetMaterialSpcMapTexture( int MHandle, int MaterialIndex, int TexIndex ) ;			// 指定のマテリアルでスペキュラマップとして使用するテクスチャを指定する
extern	int			NS_MV1GetMaterialSpcMapTexture( int MHandle, int MaterialIndex ) ;							// 指定のマテリアルでスペキュラマップとして使用されているテクスチャのインデックスを取得する
extern	int			NS_MV1GetMaterialNormalMapTexture( int MHandle, int MaterialIndex ) ;					// 指定のマテリアルで法線マップとして使用されているテクスチャのインデックスを取得する
//extern	const char *NS_MV1GetMaterialDifMapTexPath( int MHandle, int MaterialIndex ) ;					// 指定のマテリアルのディフューズマップテクスチャのパスを取得する
//extern	const char *NS_MV1GetMaterialSpcMapTexPath( int MHandle, int MaterialIndex ) ;					// 指定のマテリアルのスペキュラマップテクスチャのパスを取得する
//extern	const char *NS_MV1GetMaterialNormalMapTexPath( int MHandle, int MaterialIndex ) ;					// 指定のマテリアルの法線マップテクスチャのパスを取得する
//extern	const char *NS_MV1GetMaterialBumpMapTexPath( int MHandle, int MaterialIndex ) ;						// 指定のマテリアルの凸凹マップテクスチャのパスを取得する
//extern	float		NS_MV1GetMaterialBumpMapNextPixelLength( int MHandle, int MaterialIndex ) ;				// 指定のマテリアルの凸凹マップテクスチャの１ピクセル辺りの距離を取得する
//extern	int			NS_MV1SetMaterialDifMapTexPath( int MHandle, int MaterialIndex, const char *TexPath ) ;		// 指定のマテリアルのディフューズマップテクスチャのパスを設定する
//extern	int			NS_MV1SetMaterialSpcMapTexPath( int MHandle, int MaterialIndex, const char *TexPath ) ;	// 指定のマテリアルのスペキュラマップテクスチャのパスを設定する
extern	int			NS_MV1SetMaterialDifGradTexture( int MHandle, int MaterialIndex, int TexIndex ) ;		// 指定のマテリアルでトゥーンレンダリングのディフューズグラデーションマップとして使用するテクスチャを設定する
extern	int			NS_MV1GetMaterialDifGradTexture( int MHandle, int MaterialIndex ) ;						// 指定のマテリアルでトゥーンレンダリングのディフューズグラデーションマップとして使用するテクスチャを取得する
extern	int			NS_MV1SetMaterialSpcGradTexture( int MHandle, int MaterialIndex, int TexIndex ) ;		// 指定のマテリアルでトゥーンレンダリングのスペキュラグラデーションマップとして使用するテクスチャを設定する
extern	int			NS_MV1GetMaterialSpcGradTexture( int MHandle, int MaterialIndex ) ;						// 指定のマテリアルでトゥーンレンダリングのスペキュラグラデーションマップとして使用するテクスチャを取得する
extern	int			NS_MV1SetMaterialSphereMapTexture(		int MHandle, int MaterialIndex, int TexIndex ) ;			// 指定のマテリアルでトゥーンレンダリングのスフィアマップとして使用するテクスチャを設定する
extern	int			NS_MV1GetMaterialSphereMapTexture(		int MHandle, int MaterialIndex ) ;							// 指定のマテリアルでトゥーンレンダリングのスフィアマップとして使用するテクスチャを取得する
extern	int			NS_MV1SetMaterialDifGradBlendType( int MHandle, int MaterialIndex, int BlendType ) ;	// 指定のマテリアルのトゥーンレンダリングで使用するディフューズグラデーションマップとディフューズカラーの合成方法を設定する( DX_MATERIAL_BLENDTYPE_ADDITIVE など )
extern	int			NS_MV1GetMaterialDifGradBlendType( int MHandle, int MaterialIndex ) ;					// 指定のマテリアルのトゥーンレンダリングで使用するディフューズグラデーションマップとディフューズカラーの合成方法を取得する( DX_MATERIAL_BLENDTYPE_ADDITIVE など )
extern	int			NS_MV1SetMaterialSpcGradBlendType( int MHandle, int MaterialIndex, int BlendType ) ;	// 指定のマテリアルのトゥーンレンダリングで使用するスペキュラグラデーションマップとスペキュラカラーの合成方法を設定する( DX_MATERIAL_BLENDTYPE_ADDITIVE など )
extern	int			NS_MV1GetMaterialSpcGradBlendType( int MHandle, int MaterialIndex ) ;					// 指定のマテリアルのトゥーンレンダリングで使用するスペキュラグラデーションマップとスペキュラカラーの合成方法を取得する( DX_MATERIAL_BLENDTYPE_ADDITIVE など )
extern	int			NS_MV1SetMaterialSphereMapBlendType(	int MHandle, int MaterialIndex, int BlendType ) ;			// 指定のマテリアルのトゥーンレンダリングで使用するスフィアマップの合成方法を設定する( DX_MATERIAL_BLENDTYPE_ADDITIVE など )
extern	int			NS_MV1GetMaterialSphereMapBlendType(	int MHandle, int MaterialIndex ) ;							// 指定のマテリアルのトゥーンレンダリングで使用するスフィアマップの合成方法を取得する( DX_MATERIAL_BLENDTYPE_ADDITIVE など )
extern	int			NS_MV1SetMaterialOutLineWidth( int MHandle, int MaterialIndex, float Width ) ;			// 指定のマテリアルのトゥーンレンダリングで使用する輪郭線の太さを設定する
extern	float		NS_MV1GetMaterialOutLineWidth( int MHandle, int MaterialIndex ) ;						// 指定のマテリアルのトゥーンレンダリングで使用する輪郭線の太さを取得する
extern	int			NS_MV1SetMaterialOutLineDotWidth( int MHandle, int MaterialIndex, float Width ) ;			// 指定のマテリアルのトゥーンレンダリングで使用する輪郭線のドット単位の太さを設定する
extern	float		NS_MV1GetMaterialOutLineDotWidth( int MHandle, int MaterialIndex ) ;						// 指定のマテリアルのトゥーンレンダリングで使用する輪郭線のドット単位の太さを取得する
extern	int			NS_MV1SetMaterialOutLineColor( int MHandle, int MaterialIndex, COLOR_F Color ) ;		// 指定のマテリアルのトゥーンレンダリングで使用する輪郭線の色を設定する
extern	COLOR_F		NS_MV1GetMaterialOutLineColor( int MHandle, int MaterialIndex ) ;						// 指定のマテリアルのトゥーンレンダリングで使用する輪郭線の色を取得する
extern	int			NS_MV1SetMaterialDrawBlendMode( int MHandle, int MaterialIndex, int BlendMode ) ;		// 指定のマテリアルの描画ブレンドモードを設定する( DX_BLENDMODE_ALPHA 等 )
extern	int			NS_MV1SetMaterialDrawBlendParam( int MHandle, int MaterialIndex, int BlendParam ) ;		// 指定のマテリアルの描画ブレンドパラメータを設定する
extern	int			NS_MV1GetMaterialDrawBlendMode( int MHandle, int MaterialIndex ) ;						// 指定のマテリアルの描画ブレンドモードを取得する( DX_BLENDMODE_ALPHA 等 )
extern	int			NS_MV1GetMaterialDrawBlendParam( int MHandle, int MaterialIndex ) ;						// 指定のマテリアルの描画ブレンドパラメータを設定する
extern	int			NS_MV1SetMaterialDrawAlphaTest(		int MHandle, int MaterialIndex,	int Enable, int Mode, int Param ) ;		// 指定のマテリアルの描画時のアルファテストの設定を行う( Enable:αテストを行うかどうか( TRUE:行う  FALSE:行わない( デフォルト )  Mode:テストモード( DX_CMP_GREATER等 )  Param:描画アルファ値との比較に使用する値( 0〜255 ) )
extern	int			NS_MV1GetMaterialDrawAlphaTestEnable( int MHandle, int MaterialIndex ) ;										// 指定のマテリアルの描画時のアルファテストを行うかどうかを取得する( 戻り値  TRUE:アルファテストを行う  FALSE:アルファテストを行わない )
extern	int			NS_MV1GetMaterialDrawAlphaTestMode(	int MHandle, int MaterialIndex ) ;										// 指定のマテリアルの描画時のアルファテストのテストモードを取得する( 戻り値  テストモード( DX_CMP_GREATER等 ) )
extern	int			NS_MV1GetMaterialDrawAlphaTestParam( int MHandle, int MaterialIndex ) ;										// 指定のマテリアルの描画時のアルファテストの描画アルファ地との比較に使用する値( 0〜255 )を取得する
extern	int			NS_MV1SetMaterialTypeAll(				int MHandle,                    int Type ) ;						// 全てのマテリアルのタイプを変更する( Type : DX_MATERIAL_TYPE_NORMAL など )
extern	int			NS_MV1SetMaterialTypeParamAll(			int MHandle,                    ... ) ;									// 全てのマテリアルのタイプ別パラメータを変更する( マテリアルタイプ DX_MATERIAL_TYPE_MAT_SPEC_LUMINANCE_TWO_COLOR などで使用 )
extern	int			NS_MV1SetMaterialDifGradBlendTypeAll(	int MHandle,                    int BlendType ) ;						// 全てのマテリアルのトゥーンレンダリングで使用するディフューズグラデーションマップとディフューズカラーの合成方法を設定する( DX_MATERIAL_BLENDTYPE_ADDITIVE など )
extern	int			NS_MV1SetMaterialSpcGradBlendTypeAll(	int MHandle,                    int BlendType ) ;						// 全てのマテリアルのトゥーンレンダリングで使用するスペキュラグラデーションマップとスペキュラカラーの合成方法を設定する( DX_MATERIAL_BLENDTYPE_ADDITIVE など )
extern	int			NS_MV1SetMaterialSphereMapBlendTypeAll( int MHandle,                    int BlendType ) ;						// 全てのマテリアルのトゥーンレンダリングで使用するスフィアマップの合成方法を設定する( DX_MATERIAL_BLENDTYPE_ADDITIVE など )
extern	int			NS_MV1SetMaterialOutLineWidthAll(		int MHandle,                    float Width ) ;							// 全てのマテリアルのトゥーンレンダリングで使用する輪郭線の太さを設定する
extern	int			NS_MV1SetMaterialOutLineDotWidthAll(	int MHandle,                    float Width ) ;							// 全てのマテリアルのトゥーンレンダリングで使用する輪郭線のドット単位の太さを設定する
extern	int			NS_MV1SetMaterialOutLineColorAll(		int MHandle,                    COLOR_F Color ) ;						// 全てのマテリアルのトゥーンレンダリングで使用する輪郭線の色を設定する
extern	int			NS_MV1SetMaterialDrawBlendModeAll(		int MHandle,                    int BlendMode ) ;						// 全てのマテリアルの描画ブレンドモードを設定する( DX_BLENDMODE_ALPHA 等 )
extern	int			NS_MV1SetMaterialDrawBlendParamAll(	int MHandle,                    int BlendParam ) ;						// 全てのマテリアルの描画ブレンドパラメータを設定する
extern	int			NS_MV1SetMaterialDrawAlphaTestAll(		int MHandle,                    int Enable, int Mode, int Param ) ;		// 全てのマテリアルの描画時のアルファテストの設定を行う( Enable:αテストを行うかどうか( TRUE:行う  FALSE:行わない( デフォルト ) ) Mode:テストモード( DX_CMP_GREATER等 )  Param:描画アルファ値との比較に使用する値( 0〜255 ) )

// テクスチャ関係
extern	int			NS_MV1GetTextureNum( int MHandle ) ;													// テクスチャの数を取得
extern	const TCHAR *NS_MV1GetTextureName( int MHandle, int TexIndex ) ;										// テクスチャの名前を取得
extern	int			NS_MV1SetTextureColorFilePath( int MHandle, int TexIndex, const TCHAR *FilePath ) ;		// カラーテクスチャのファイルパスを変更する
extern	int			NS_MV1SetTextureColorFilePathWithStrLen(	int MHandle, int TexIndex, const TCHAR *FilePath, size_t FilePathLength ) ;	// カラーテクスチャのファイルパスを変更する
extern	const TCHAR *NS_MV1GetTextureColorFilePath(			int MHandle, int TexIndex ) ;									// カラーテクスチャのファイルパスを取得
extern	int			NS_MV1SetTextureAlphaFilePath(				int MHandle, int TexIndex, const TCHAR *FilePath                        ) ;	// アルファテクスチャのファイルパスを変更する
extern	int			NS_MV1SetTextureAlphaFilePathWithStrLen(	int MHandle, int TexIndex, const TCHAR *FilePath, size_t FilePathLength ) ;	// アルファテクスチャのファイルパスを変更する
extern	const TCHAR *NS_MV1GetTextureAlphaFilePath( int MHandle, int TexIndex ) ;							// アルファテクスチャのファイルパスを取得
extern	int			NS_MV1SetTextureGraphHandle( int MHandle, int TexIndex, int GrHandle, int SemiTransFlag ) ;	// テクスチャで使用するグラフィックハンドルを変更する( GrHandle を -1 にすると解除 )
extern	int			NS_MV1GetTextureGraphHandle( int MHandle, int TexIndex ) ;										// テクスチャのグラフィックハンドルを取得する
extern	int			NS_MV1SetTextureAddressMode( int MHandle, int TexIndex, int AddrUMode, int AddrVMode ) ;	// テクスチャのアドレスモードを設定する( AddrUMode は DX_TEXADDRESS_WRAP 等 )
extern	int			NS_MV1GetTextureAddressModeU( int MHandle, int TexIndex ) ;								// テクスチャのＵ値のアドレスモードを取得する( 戻り値:DX_TEXADDRESS_WRAP 等 )
extern	int			NS_MV1GetTextureAddressModeV( int MHandle, int TexIndex ) ;								// テクスチャのＶ値のアドレスモードを取得する( 戻り値:DX_TEXADDRESS_WRAP 等 )
extern	int			NS_MV1GetTextureWidth( int MHandle, int TexIndex ) ;									// テクスチャの幅を取得する
extern	int			NS_MV1GetTextureHeight( int MHandle, int TexIndex ) ;									// テクスチャの高さを取得する
extern	int			NS_MV1GetTextureSemiTransState( int MHandle, int TexIndex ) ;							// テクスチャに半透明要素があるかどうかを取得する( 戻り値  TRUE:ある  FALSE:ない )
extern	int			NS_MV1SetTextureBumpImageFlag( int MHandle, int TexIndex, int Flag ) ;					// テクスチャで使用している画像がバンプマップかどうかを設定する
extern	int			NS_MV1GetTextureBumpImageFlag( int MHandle, int TexIndex ) ;							// テクスチャがバンプマップかどうかを取得する( 戻り値  TRUE:バンプマップ  FALSE:違う )
extern	int			NS_MV1SetTextureBumpImageNextPixelLength( int MHandle, int TexIndex, float Length ) ;	// バンプマップ画像の場合の隣のピクセルとの距離を設定する
extern	float		NS_MV1GetTextureBumpImageNextPixelLength( int MHandle, int TexIndex ) ;					// バンプマップ画像の場合の隣のピクセルとの距離を取得する
extern	int			NS_MV1SetTextureSampleFilterMode( int MHandle, int TexIndex, int FilterMode ) ;			// テクスチャのフィルタリングモードを設定する
extern	int			NS_MV1GetTextureSampleFilterMode( int MHandle, int TexIndex ) ;							// テクスチャのフィルタリングモードを取得する( 戻り値  DX_DRAWMODE_BILINEAR等 )
extern	int			NS_MV1LoadTexture( const TCHAR *FilePath ) ;													// ３Ｄモデルに貼り付けるのに向いた画像の読み込み方式で画像を読み込む( 戻り値  -1:エラー  0以上:グラフィックハンドル )
extern	int			NS_MV1LoadTextureWithStrLen(				const TCHAR *FilePath, size_t FilePathLength ) ;				// ３Ｄモデルに貼り付けるのに向いた画像の読み込み方式で画像を読み込む( 戻り値  -1:エラー  0以上:グラフィックハンドル )


// フレーム関係
extern	int			NS_MV1GetFrameNum( int MHandle ) ;														// フレームの数を取得する
extern	int			NS_MV1SearchFrame( int MHandle, const TCHAR *FrameName ) ;								// フレームの名前からモデル中のフレームのフレームインデックスを取得する( 無かった場合は戻り値が-1 )
extern	int			NS_MV1SearchFrameWithStrLen(			int MHandle, const TCHAR *FrameName, size_t FrameNameLength ) ;			// フレームの名前からモデル中のフレームのフレームインデックスを取得する( 無かった場合は戻り値が-1 )
extern	int			NS_MV1SearchFrameChild(				int MHandle, int FrameIndex = -1 , const TCHAR *ChildName = NULL                              ) ;	// フレームの名前から指定のフレームの子フレームのフレームインデックスを取得する( 名前指定版 )( FrameIndex を -1 にすると親を持たないフレームを ChildIndex で指定する )( 無かった場合は戻り値が-1 )
extern	int			NS_MV1SearchFrameChildWithStrLen(		int MHandle, int FrameIndex = -1 , const TCHAR *ChildName = NULL , size_t ChildNameLength = 0 ) ;	// フレームの名前から指定のフレームの子フレームのフレームインデックスを取得する( 名前指定版 )( FrameIndex を -1 にすると親を持たないフレームを ChildIndex で指定する )( 無かった場合は戻り値が-1 )
extern	const TCHAR *NS_MV1GetFrameName( int MHandle, int FrameIndex ) ;										// 指定のフレームの名前を取得する( エラーの場合は戻り値が NULL )
extern	int			NS_MV1GetFrameName2( int MHandle, int FrameIndex, TCHAR *StrBuffer ) ;						// 指定のフレームの名前を取得する( 戻り値   -1:エラー  -1以外:文字列のサイズ )
extern	int			NS_MV1GetFrameParent( int MHandle, int FrameIndex ) ;									// 指定のフレームの親フレームのインデックスを得る( 親がいない場合は -2 が返る )
extern	int			NS_MV1GetFrameChildNum( int MHandle, int FrameIndex = -1 ) ;							// 指定のフレームの子フレームの数を取得する( FrameIndex を -1 にすると親を持たないフレームの数が返ってくる )
extern	int			NS_MV1GetFrameChild( int MHandle, int FrameIndex = -1, int ChildIndex = 0 ) ;			// 指定のフレームの子フレームのフレームインデックスを取得する( 番号指定版 )( FrameIndex を -1 にすると親を持たないフレームを ChildIndex で指定する )( エラーの場合は戻り値が-1 )
extern	VECTOR		NS_MV1GetFramePosition(				int MHandle, int FrameIndex ) ;											// 指定のフレームの座標を取得する
extern	VECTOR_D	NS_MV1GetFramePositionD(				int MHandle, int FrameIndex ) ;											// 指定のフレームの座標を取得する
extern	MATRIX		NS_MV1GetFrameBaseLocalMatrix(			int MHandle, int FrameIndex ) ;											// 指定のフレームの初期状態での座標変換行列を取得する
extern	MATRIX_D	NS_MV1GetFrameBaseLocalMatrixD(		int MHandle, int FrameIndex ) ;											// 指定のフレームの初期状態での座標変換行列を取得する
extern	MATRIX		NS_MV1GetFrameLocalMatrix(				int MHandle, int FrameIndex ) ;											// 指定のフレームの座標変換行列を取得する
extern	MATRIX_D	NS_MV1GetFrameLocalMatrixD(			int MHandle, int FrameIndex ) ;											// 指定のフレームの座標変換行列を取得する
extern	MATRIX		NS_MV1GetFrameLocalWorldMatrix(		int MHandle, int FrameIndex ) ;											// 指定のフレームのローカル座標からワールド座標に変換する行列を得る
extern	MATRIX_D	NS_MV1GetFrameLocalWorldMatrixD(		int MHandle, int FrameIndex ) ;											// 指定のフレームのローカル座標からワールド座標に変換する行列を得る
extern	int			NS_MV1SetFrameUserLocalMatrix(			int MHandle, int FrameIndex, MATRIX   Matrix ) ;						// 指定のフレームの座標変換行列を設定する
extern	int			NS_MV1SetFrameUserLocalMatrixD(		int MHandle, int FrameIndex, MATRIX_D Matrix ) ;						// 指定のフレームの座標変換行列を設定する
extern	int			NS_MV1ResetFrameUserLocalMatrix(		int MHandle, int FrameIndex ) ;											// 指定のフレームの座標変換行列をデフォルトに戻す
extern	int			NS_MV1SetFrameUserLocalWorldMatrix(	int MHandle, int FrameIndex, MATRIX   Matrix ) ;						// 指定のフレームの座標変換行列( ローカル座標からワールド座標に変換する行列 )を設定する
extern	int			NS_MV1SetFrameUserLocalWorldMatrixD(	int MHandle, int FrameIndex, MATRIX_D Matrix ) ;						// 指定のフレームの座標変換行列( ローカル座標からワールド座標に変換する行列 )を設定する
extern	int			NS_MV1ResetFrameUserLocalWorldMatrix(	int MHandle, int FrameIndex ) ;											// 指定のフレームの座標変換行列( ローカル座標からワールド座標に変換する行列 )をデフォルトに戻す
extern	VECTOR		NS_MV1GetFrameMaxVertexLocalPosition(	int MHandle, int FrameIndex ) ;											// 指定のフレームが持つメッシュ頂点のローカル座標での最大値を得る
extern	VECTOR_D	NS_MV1GetFrameMaxVertexLocalPositionD(	int MHandle, int FrameIndex ) ;											// 指定のフレームが持つメッシュ頂点のローカル座標での最大値を得る
extern	VECTOR		NS_MV1GetFrameMinVertexLocalPosition(	int MHandle, int FrameIndex ) ;											// 指定のフレームが持つメッシュ頂点のローカル座標での最小値を得る
extern	VECTOR_D	NS_MV1GetFrameMinVertexLocalPositionD(	int MHandle, int FrameIndex ) ;											// 指定のフレームが持つメッシュ頂点のローカル座標での最小値を得る
extern	VECTOR		NS_MV1GetFrameAvgVertexLocalPosition(	int MHandle, int FrameIndex ) ;											// 指定のフレームが持つメッシュ頂点のローカル座標での平均値を得る
extern	VECTOR_D	NS_MV1GetFrameAvgVertexLocalPositionD(	int MHandle, int FrameIndex ) ;											// 指定のフレームが持つメッシュ頂点のローカル座標での平均値を得る
extern	int			NS_MV1GetFrameTriangleNum( int MHandle, int FrameIndex ) ;								// 指定のフレームに含まれるポリゴンの数を取得する
extern	int			NS_MV1GetFrameMeshNum( int MHandle, int FrameIndex ) ;									// 指定のフレームが持つメッシュの数を取得する
extern	int			NS_MV1GetFrameMesh( int MHandle, int FrameIndex, int Index ) ;							// 指定のフレームが持つメッシュのメッシュインデックスを取得する
extern	int			NS_MV1SetFrameVisible( int MHandle, int FrameIndex, int VisibleFlag ) ;					// 指定のフレームの表示、非表示状態を変更する( TRUE:表示  FALSE:非表示 )
extern	int			NS_MV1GetFrameVisible( int MHandle, int FrameIndex ) ;									// 指定のフレームの表示、非表示状態を取得する( TRUE:表示  FALSE:非表示 )
extern	int			NS_MV1SetFrameDifColorScale( int MHandle, int FrameIndex, COLOR_F Scale ) ;			// 指定のフレームのディフューズカラーのスケール値を設定する( デフォルト値は 1.0f )
extern	int			NS_MV1SetFrameSpcColorScale( int MHandle, int FrameIndex, COLOR_F Scale ) ;		// 指定のフレームのスペキュラカラーのスケール値を設定する( デフォルト値は 1.0f )
extern	int			NS_MV1SetFrameEmiColorScale( int MHandle, int FrameIndex, COLOR_F Scale ) ;		// 指定のフレームのエミッシブカラーのスケール値を設定する( デフォルト値は 1.0f )
extern	int			NS_MV1SetFrameAmbColorScale( int MHandle, int FrameIndex, COLOR_F Scale ) ;			// 指定のフレームのアンビエントカラーのスケール値を設定する( デフォルト値は 1.0f )
extern	COLOR_F		NS_MV1GetFrameDifColorScale( int MHandle, int FrameIndex ) ;						// 指定のフレームのディフューズカラーのスケール値を取得する( デフォルト値は 1.0f )
extern	COLOR_F		NS_MV1GetFrameSpcColorScale( int MHandle, int FrameIndex ) ;						// 指定のフレームのスペキュラカラーのスケール値を取得する( デフォルト値は 1.0f )
extern	COLOR_F		NS_MV1GetFrameEmiColorScale( int MHandle, int FrameIndex ) ;						// 指定のフレームのエミッシブカラーのスケール値を取得する( デフォルト値は 1.0f )
extern	COLOR_F		NS_MV1GetFrameAmbColorScale( int MHandle, int FrameIndex ) ;						// 指定のフレームのアンビエントカラーのスケール値を取得する( デフォルト値は 1.0f )
extern	int			NS_MV1GetFrameSemiTransState( int MHandle, int FrameIndex ) ;							// 指定のフレームに半透明要素があるかどうかを取得する( 戻り値 TRUE:ある  FALSE:ない )
extern	int			NS_MV1SetFrameOpacityRate( int MHandle, int FrameIndex, float Rate ) ;					// 指定のフレームの不透明度を設定する( 不透明 1.0f 〜 透明 0.0f )
extern	float		NS_MV1GetFrameOpacityRate( int MHandle, int FrameIndex ) ;								// 指定のフレームの不透明度を取得する( 不透明 1.0f 〜 透明 0.0f )
extern	int			NS_MV1SetFrameBaseVisible( int MHandle, int FrameIndex, int VisibleFlag ) ;				// 指定のフレームの初期表示状態を設定する( TRUE:表示  FALSE:非表示 )
extern	int			NS_MV1GetFrameBaseVisible( int MHandle, int FrameIndex ) ;								// 指定のフレームの初期表示状態を取得する( TRUE:表示  FALSE:非表示 )
extern	int			NS_MV1SetFrameTextureAddressTransform( int MHandle, int FrameIndex, float TransU, float TransV, float ScaleU, float ScaleV, float RotCenterU, float RotCenterV, float Rotate ) ;	// 指定のフレームのテクスチャ座標変換パラメータを設定する
extern	int			NS_MV1SetFrameTextureAddressTransformMatrix( int MHandle, int FrameIndex, MATRIX Matrix ) ;			// 指定のフレームのテクスチャ座標変換行列をセットする
extern	int			NS_MV1ResetFrameTextureAddressTransform( int MHandle, int FrameIndex ) ;							// 指定のフレームのテクスチャ座標変換パラメータをリセットする

// メッシュ関係
extern	int			NS_MV1GetMeshNum( int MHandle ) ;														// モデルに含まれるメッシュの数を取得する
extern	int			NS_MV1GetMeshMaterial( int MHandle, int MeshIndex ) ;									// 指定メッシュが使用しているマテリアルのインデックスを取得する
extern	int			NS_MV1GetMeshTriangleNum( int MHandle, int MeshIndex ) ;								// 指定メッシュに含まれる三角形ポリゴンの数を取得する
extern	int			NS_MV1SetMeshVisible( int MHandle, int MeshIndex, int VisibleFlag ) ;					// 指定メッシュの表示、非表示状態を変更する( TRUE:表示  FALSE:非表示 )
extern	int			NS_MV1GetMeshVisible( int MHandle, int MeshIndex ) ;									// 指定メッシュの表示、非表示状態を取得する( TRUE:表示  FALSE:非表示 )
extern	int			NS_MV1SetMeshDifColorScale( int MHandle, int MeshIndex, COLOR_F Scale ) ;			// 指定のメッシュのディフューズカラーのスケール値を設定する( デフォルト値は 1.0f )
extern	int			NS_MV1SetMeshSpcColorScale( int MHandle, int MeshIndex, COLOR_F Scale ) ;			// 指定のメッシュのスペキュラカラーのスケール値を設定する( デフォルト値は 1.0f )
extern	int			NS_MV1SetMeshEmiColorScale( int MHandle, int MeshIndex, COLOR_F Scale ) ;			// 指定のメッシュのエミッシブカラーのスケール値を設定する( デフォルト値は 1.0f )
extern	int			NS_MV1SetMeshAmbColorScale( int MHandle, int MeshIndex, COLOR_F Scale ) ;			// 指定のメッシュのアンビエントカラーのスケール値を設定する( デフォルト値は 1.0f )
extern	COLOR_F		NS_MV1GetMeshDifColorScale( int MHandle, int MeshIndex ) ;							// 指定のメッシュのディフューズカラーのスケール値を取得する( デフォルト値は 1.0f )
extern	COLOR_F		NS_MV1GetMeshSpcColorScale( int MHandle, int MeshIndex ) ;							// 指定のメッシュのスペキュラカラーのスケール値を取得する( デフォルト値は 1.0f )
extern	COLOR_F		NS_MV1GetMeshEmiColorScale( int MHandle, int MeshIndex ) ;							// 指定のメッシュのエミッシブカラーのスケール値を取得する( デフォルト値は 1.0f )
extern	COLOR_F		NS_MV1GetMeshAmbColorScale( int MHandle, int MeshIndex ) ;							// 指定のメッシュのアンビエントカラーのスケール値を取得する( デフォルト値は 1.0f )
extern	int			NS_MV1SetMeshOpacityRate( int MHandle, int MeshIndex, float Rate ) ;					// 指定のメッシュの不透明度を設定する( 不透明 1.0f 〜 透明 0.0f )
extern	float		NS_MV1GetMeshOpacityRate( int MHandle, int MeshIndex ) ;								// 指定のメッシュの不透明度を取得する( 不透明 1.0f 〜 透明 0.0f )
extern	int			NS_MV1SetMeshDrawBlendMode( int MHandle, int MeshIndex, int BlendMode ) ;				// 指定のメッシュの描画ブレンドモードを設定する( DX_BLENDMODE_ALPHA 等 )
extern	int			NS_MV1SetMeshDrawBlendParam( int MHandle, int MeshIndex, int BlendParam ) ;				// 指定のメッシュの描画ブレンドパラメータを設定する
extern	int			NS_MV1GetMeshDrawBlendMode( int MHandle, int MeshIndex ) ;								// 指定のメッシュの描画ブレンドモードを取得する( DX_BLENDMODE_ALPHA 等 )
extern	int			NS_MV1GetMeshDrawBlendParam( int MHandle, int MeshIndex ) ;								// 指定のメッシュの描画ブレンドパラメータを設定する
extern	int			NS_MV1SetMeshBaseVisible( int MHandle, int MeshIndex, int VisibleFlag ) ;				// 指定のメッシュの初期表示状態を設定する( TRUE:表示  FALSE:非表示 )
extern	int			NS_MV1GetMeshBaseVisible( int MHandle, int MeshIndex ) ;								// 指定のメッシュの初期表示状態を取得する( TRUE:表示  FALSE:非表示 )
extern	int			NS_MV1SetMeshBackCulling( int MHandle, int MeshIndex, int CullingFlag ) ;				// 指定のメッシュのバックカリングを行うかどうかを設定する( DX_CULLING_LEFT 等 )
extern	int			NS_MV1GetMeshBackCulling( int MHandle, int MeshIndex ) ;								// 指定のメッシュのバックカリングを行うかどうかを取得する( DX_CULLING_LEFT 等 )
extern	VECTOR		NS_MV1GetMeshMaxPosition( int MHandle, int MeshIndex ) ;								// 指定のメッシュに含まれるポリゴンの最大ローカル座標を取得する
extern	VECTOR		NS_MV1GetMeshMinPosition( int MHandle, int MeshIndex ) ;								// 指定のメッシュに含まれるポリゴンの最小ローカル座標を取得する
extern	int			NS_MV1GetMeshTListNum( int MHandle, int MeshIndex ) ;									// 指定のメッシュに含まれるトライアングルリストの数を取得する
extern	int			NS_MV1GetMeshTList( int MHandle, int MeshIndex, int Index ) ;							// 指定のメッシュに含まれるトライアングルリストのインデックスを取得する
extern	int			NS_MV1GetMeshSemiTransState( int MHandle, int MeshIndex ) ;								// 指定のメッシュに半透明要素があるかどうかを取得する( 戻り値 TRUE:ある  FALSE:ない )
extern	int			NS_MV1SetMeshUseVertDifColor( int MHandle, int MeshIndex, int UseFlag ) ;				// 指定のメッシュの頂点ディフューズカラーをマテリアルのディフューズカラーの代わりに使用するかどうかを設定する( TRUE:マテリアルカラーの代わりに使用する  FALSE:マテリアルカラーを使用する )
extern	int			NS_MV1SetMeshUseVertSpcColor( int MHandle, int MeshIndex, int UseFlag ) ;			// 指定のメッシュの頂点スペキュラカラーをマテリアルのスペキュラカラーの代わりに使用するかどうかを設定する( TRUE:マテリアルカラーの代わりに使用する  FALSE:マテリアルカラーを使用する )
extern	int			NS_MV1GetMeshUseVertDifColor( int MHandle, int MeshIndex ) ;						// 指定のメッシュの頂点ディフューズカラーをマテリアルのディフューズカラーの代わりに使用するかどうかの設定を取得する( 戻り値  TRUE:マテリアルカラーの代わりに使用する  FALSE:マテリアルカラーを使用する )
extern	int			NS_MV1GetMeshUseVertSpcColor( int MHandle, int MeshIndex ) ;						// 指定のメッシュの頂点スペキュラカラーをマテリアルのスペキュラカラーの代わりに使用するかどうかの設定を取得する( 戻り値  TRUE:マテリアルカラーの代わりに使用する  FALSE:マテリアルカラーを使用する )
extern	int			NS_MV1GetMeshShapeFlag(	int MHandle, int MeshIndex ) ;											// 指定のメッシュがシェイプメッシュかどうかを取得する

// シェイプ関係
extern	int			NS_MV1GetShapeNum( int MHandle ) ;															// モデルに含まれるシェイプの数を取得する
extern	int			NS_MV1SearchShape( int MHandle, const TCHAR *ShapeName ) ;									// シェイプの名前からモデル中のシェイプのシェイプインデックスを取得する( 無かった場合は戻り値が-1 )
extern	int			NS_MV1SearchShapeWithStrLen(			int MHandle, const TCHAR *ShapeName, size_t ShapeNameLength ) ;			// シェイプの名前からモデル中のシェイプのシェイプインデックスを取得する( 無かった場合は戻り値が-1 )
extern	const TCHAR	*NS_MV1GetShapeName( int MHandle, int ShapeIndex ) ;											// 指定シェイプの名前を取得する
extern	int			NS_MV1GetShapeTargetMeshNum( int MHandle, int ShapeIndex ) ;											// 指定シェイプが対象としているメッシュの数を取得する
extern	int			NS_MV1GetShapeTargetMesh( int MHandle, int ShapeIndex, int Index ) ;								// 指定シェイプが対象としているメッシュのメッシュインデックスを取得する
extern	int			NS_MV1SetShapeRate( int MHandle, int ShapeIndex, float Rate, int Type ) ;						// 指定シェイプの有効率を設定する( Rate  0.0f:0% 〜 1.0f:100% )
extern	float		NS_MV1GetShapeRate( int MHandle, int ShapeIndex ) ;											// 指定シェイプの有効率を取得する( 戻り値  0.0f:0% 〜 1.0f:100% )
extern	float		NS_MV1GetShapeApplyRate( int MHandle, int ShapeIndex ) ;											// 指定シェイプの有効率を取得する( 戻り値  0.0f:0% 〜 1.0f:100% )( MV1SetShapeRate で指定した値がそのまま戻り値となる MV1GetShapeRate と異なりアニメーションのシェイプ情報なども加味した値が戻り値となります )

// トライアングルリスト関係
extern	int			NS_MV1GetTriangleListNum( int MHandle ) ;												// トライアングルリストの数を取得する
extern	int			NS_MV1GetTriangleListVertexType( int MHandle, int TListIndex ) ;						// 指定のトライアングルリストの頂点データタイプを取得する( MV1_VERTEX_TYPE_NORMAL 等 )
extern	int			NS_MV1GetTriangleListPolygonNum( int MHandle, int TListIndex ) ;						// 指定のトライアングルリストに含まれるポリゴンの数を取得する
extern	int			NS_MV1GetTriangleListVertexNum( int MHandle, int TListIndex ) ;							// 指定のトライアングルリストに含まれる頂点データの数を取得する
extern	int			NS_MV1GetTriangleListLocalWorldMatrixNum(	int MHandle, int TListIndex ) ;										// 指定のトライアングルリストが使用する座標変換行列の数を取得する
extern	MATRIX		NS_MV1GetTriangleListLocalWorldMatrix(		int MHandle, int TListIndex, int LWMatrixIndex ) ;					// 指定のトライアングルリストが使用する座標変換行列( ローカル→ワールド )を取得する
extern	int			NS_MV1GetTriangleListPolygonVertexPosition(	int MHandle, int TListIndex, int PolygonIndex, VECTOR *VertexPositions = NULL , float *MatrixWeights = NULL ) ;	// 指定のトライアングルリストの指定のポリゴンが使用している頂点の座標を取得する( 戻り値  エラー：-1  0以上：ポリゴンが使用している頂点の数 )
extern	int			NS_MV1GetTriangleListUseMaterial(				int MHandle, int TListIndex ) ;														// 指定のトライアングルリストが使用しているマテリアルのインデックスを取得する

// コリジョン関係
extern	int			NS_MV1SetupCollInfo( int MHandle, int FrameIndex = -1, int XDivNum = 32, int YDivNum = 8, int ZDivNum = 32 ) ;	// コリジョン情報を構築する
extern	int			NS_MV1TerminateCollInfo( int MHandle, int FrameIndex = -1 ) ;							// コリジョン情報の後始末
extern	int			NS_MV1RefreshCollInfo( int MHandle, int FrameIndex = -1 ) ;								// コリジョン情報を更新する
extern	MV1_COLL_RESULT_POLY NS_MV1CollCheck_Line( int MHandle, int FrameIndex, VECTOR PosStart, VECTOR PosEnd ) ;	// 線とモデルの当たり判定
extern	MV1_COLL_RESULT_POLY_DIM NS_MV1CollCheck_LineDim( int MHandle, int FrameIndex, VECTOR PosStart, VECTOR PosEnd ) ; // 線とモデルの当たり判定( 戻り値が MV1_COLL_RESULT_POLY_DIM )
extern	MV1_COLL_RESULT_POLY_DIM NS_MV1CollCheck_Sphere( int MHandle, int FrameIndex, VECTOR CenterPos, float r ) ;	// 球とモデルの当たり判定
extern	MV1_COLL_RESULT_POLY_DIM NS_MV1CollCheck_Capsule( int MHandle, int FrameIndex, VECTOR Pos1, VECTOR Pos2, float r ) ;	// カプセルとモデルの当たり判定
extern	MV1_COLL_RESULT_POLY_DIM NS_MV1CollCheck_Triangle( int MHandle, int FrameIndex, VECTOR Pos1, VECTOR Pos2, VECTOR Pos3 ) ;	// 三角形とモデルの当たり判定
extern	MV1_COLL_RESULT_POLY NS_MV1CollCheck_GetResultPoly( MV1_COLL_RESULT_POLY_DIM ResultPolyDim, int PolyNo ) ;	// コリジョン結果ポリゴン配列から指定番号のポリゴン情報を取得する
extern	int			NS_MV1CollResultPolyDimTerminate( MV1_COLL_RESULT_POLY_DIM ResultPolyDim ) ;			// コリジョン結果ポリゴン配列の後始末をする

// 参照用メッシュ関係
extern	int			NS_MV1SetupReferenceMesh( int MHandle, int FrameIndex, int IsTransform, int IsPositionOnly = FALSE  ) ;				// 参照用メッシュのセットアップ
extern	int			NS_MV1TerminateReferenceMesh( int MHandle, int FrameIndex, int IsTransform, int IsPositionOnly = FALSE  ) ;			// 参照用メッシュの後始末
extern	int			NS_MV1RefreshReferenceMesh( int MHandle, int FrameIndex, int IsTransform, int IsPositionOnly = FALSE  ) ;			// 参照用メッシュの更新
extern	MV1_REF_POLYGONLIST	NS_MV1GetReferenceMesh( int MHandle, int FrameIndex, int IsTransform, int IsPositionOnly = FALSE  ) ;		// 参照用メッシュを取得する

#else // DX_THREAD_SAFE

// モデルの読み込み・保存・複製関係
#define NS_MV1LoadModel											MV1LoadModel
#define NS_MV1LoadModelWithStrLen								MV1LoadModelWithStrLen
#define NS_MV1LoadModelFromMem									MV1LoadModelFromMem
#define NS_MV1LoadModelUseReadFunc								MV1LoadModelUseReadFunc
#define NS_MV1DeleteModel										MV1DeleteModel
#define NS_MV1InitModel											MV1InitModel
#define NS_MV1CreateCloneModel									MV1CreateCloneModel
#define NS_MV1DuplicateModel									MV1DuplicateModel
#define NS_MV1SetLoadModelReMakeNormal							MV1SetLoadModelReMakeNormal
#define NS_MV1SetLoadModelReMakeNormalSmoothingAngle			MV1SetLoadModelReMakeNormalSmoothingAngle
#define NS_MV1SetLoadModelIgnoreScaling							MV1SetLoadModelIgnoreScaling
#define NS_MV1SetLoadModelPositionOptimize						MV1SetLoadModelPositionOptimize
#define NS_MV1SetLoadModelNotEqNormalSide_AddZeroAreaPolygon	MV1SetLoadModelNotEqNormalSide_AddZeroAreaPolygon
#define NS_MV1SetLoadModelUsePhysicsMode						MV1SetLoadModelUsePhysicsMode
#define NS_MV1SetLoadModelPhysicsWorldGravity					MV1SetLoadModelPhysicsWorldGravity
#define NS_MV1GetLoadModelPhysicsWorldGravity					MV1GetLoadModelPhysicsWorldGravity
#define NS_MV1SetLoadCalcPhysicsWorldGravity					MV1SetLoadCalcPhysicsWorldGravity
#define NS_MV1GetLoadCalcPhysicsWorldGravity					MV1GetLoadCalcPhysicsWorldGravity
#define NS_MV1SetLoadModelPhysicsCalcPrecision					MV1SetLoadModelPhysicsCalcPrecision
#define NS_MV1SetLoadModel_PMD_PMX_AnimationFPSMode				MV1SetLoadModel_PMD_PMX_AnimationFPSMode
#define NS_MV1AddLoadModelDisablePhysicsNameWord				MV1AddLoadModelDisablePhysicsNameWord
#define NS_MV1AddLoadModelDisablePhysicsNameWordWithStrLen		MV1AddLoadModelDisablePhysicsNameWordWithStrLen
#define NS_MV1ResetLoadModelDisablePhysicsNameWord				MV1ResetLoadModelDisablePhysicsNameWord
#define NS_MV1SetLoadModelDisablePhysicsNameWordMode			MV1SetLoadModelDisablePhysicsNameWordMode
#define NS_MV1SetLoadModelAnimFilePath							MV1SetLoadModelAnimFilePath
#define NS_MV1SetLoadModelAnimFilePathWithStrLen				MV1SetLoadModelAnimFilePathWithStrLen
#define NS_MV1SetLoadModelUsePackDraw							MV1SetLoadModelUsePackDraw
#define NS_MV1SetLoadModelTriangleListUseMaxBoneNum				MV1SetLoadModelTriangleListUseMaxBoneNum

// モデル保存関係
#define NS_MV1SaveModelToMV1File						MV1SaveModelToMV1File
#define NS_MV1SaveModelToMV1FileWithStrLen				MV1SaveModelToMV1FileWithStrLen
#ifndef DX_NON_SAVEFUNCTION
#define NS_MV1SaveModelToXFile							MV1SaveModelToXFile
#define NS_MV1SaveModelToXFileWithStrLen				MV1SaveModelToXFileWithStrLen
#endif // DX_NON_SAVEFUNCTION

// モデル描画関係
#define NS_MV1DrawModel									MV1DrawModel
#define NS_MV1DrawFrame									MV1DrawFrame
#define NS_MV1DrawMesh									MV1DrawMesh
#define NS_MV1DrawTriangleList							MV1DrawTriangleList
#define NS_MV1DrawModelDebug							MV1DrawModelDebug

// 描画設定関係
#define NS_MV1SetUseOrigShader							MV1SetUseOrigShader
#define NS_MV1SetSemiTransDrawMode						MV1SetSemiTransDrawMode

// モデル基本制御関係
#define NS_MV1GetLocalWorldMatrix						MV1GetLocalWorldMatrix
#define NS_MV1GetLocalWorldMatrixD						MV1GetLocalWorldMatrixD
#define NS_MV1SetPosition								MV1SetPosition
#define NS_MV1SetPositionD								MV1SetPositionD
#define NS_MV1GetPosition								MV1GetPosition
#define NS_MV1GetPositionD								MV1GetPositionD
#define NS_MV1SetScale									MV1SetScale
#define NS_MV1GetScale									MV1GetScale
#define NS_MV1SetRotationXYZ							MV1SetRotationXYZ
#define NS_MV1GetRotationXYZ							MV1GetRotationXYZ
#define NS_MV1SetRotationZYAxis							MV1SetRotationZYAxis
#define NS_MV1SetRotationYUseDir						MV1SetRotationYUseDir
#define NS_MV1SetRotationMatrix							MV1SetRotationMatrix
#define NS_MV1GetRotationMatrix							MV1GetRotationMatrix
#define NS_MV1SetMatrix									MV1SetMatrix
#define NS_MV1SetMatrixD								MV1SetMatrixD
#define NS_MV1GetMatrix									MV1GetMatrix
#define NS_MV1GetMatrixD								MV1GetMatrixD
#define NS_MV1SetVisible								MV1SetVisible
#define NS_MV1GetVisible								MV1GetVisible
#define NS_MV1SetMeshCategoryVisible					MV1SetMeshCategoryVisible
#define NS_MV1GetMeshCategoryVisible					MV1GetMeshCategoryVisible
#define NS_MV1SetDifColorScale							MV1SetDifColorScale
#define NS_MV1GetDifColorScale							MV1GetDifColorScale
#define NS_MV1SetSpcColorScale							MV1SetSpcColorScale
#define NS_MV1GetSpcColorScale							MV1GetSpcColorScale
#define NS_MV1SetEmiColorScale							MV1SetEmiColorScale
#define NS_MV1GetEmiColorScale							MV1GetEmiColorScale
#define NS_MV1SetAmbColorScale							MV1SetAmbColorScale
#define NS_MV1GetAmbColorScale							MV1GetAmbColorScale
#define NS_MV1GetSemiTransState							MV1GetSemiTransState
#define NS_MV1SetOpacityRate							MV1SetOpacityRate
#define NS_MV1GetOpacityRate							MV1GetOpacityRate
#define NS_MV1SetUseDrawMulAlphaColor					MV1SetUseDrawMulAlphaColor
#define NS_MV1GetUseDrawMulAlphaColor					MV1GetUseDrawMulAlphaColor
#define NS_MV1SetUseZBuffer								MV1SetUseZBuffer
#define NS_MV1SetWriteZBuffer							MV1SetWriteZBuffer
#define NS_MV1SetZBufferCmpType							MV1SetZBufferCmpType
#define NS_MV1SetZBias									MV1SetZBias
#define NS_MV1SetUseVertDifColor						MV1SetUseVertDifColor
#define NS_MV1SetUseVertSpcColor						MV1SetUseVertSpcColor
#define NS_MV1SetSampleFilterMode						MV1SetSampleFilterMode
#define NS_MV1SetMaxAnisotropy							MV1SetMaxAnisotropy
#define NS_MV1SetWireFrameDrawFlag						MV1SetWireFrameDrawFlag
#define NS_MV1RefreshVertColorFromMaterial				MV1RefreshVertColorFromMaterial
#define NS_MV1SetPhysicsWorldGravity					MV1SetPhysicsWorldGravity
#define NS_MV1PhysicsCalculation						MV1PhysicsCalculation
#define NS_MV1PhysicsResetState							MV1PhysicsResetState
#define NS_MV1SetUseShapeFlag							MV1SetUseShapeFlag
#define NS_MV1GetMaterialNumberOrderFlag				MV1GetMaterialNumberOrderFlag




// アニメーション関係
#define NS_MV1AttachAnim								MV1AttachAnim
#define NS_MV1DetachAnim								MV1DetachAnim
#define NS_MV1SetAttachAnimTime							MV1SetAttachAnimTime
#define NS_MV1GetAttachAnimTime							MV1GetAttachAnimTime
#define NS_MV1GetAttachAnimTotalTime					MV1GetAttachAnimTotalTime
#define NS_MV1SetAttachAnimBlendRate					MV1SetAttachAnimBlendRate
#define NS_MV1GetAttachAnimBlendRate					MV1GetAttachAnimBlendRate
#define NS_MV1SetAttachAnimBlendRateToFrame				MV1SetAttachAnimBlendRateToFrame
#define NS_MV1GetAttachAnimBlendRateToFrame				MV1GetAttachAnimBlendRateToFrame
#define NS_MV1GetAttachAnim								MV1GetAttachAnim
#define NS_MV1SetAttachAnimUseShapeFlag					MV1SetAttachAnimUseShapeFlag
#define NS_MV1GetAttachAnimUseShapeFlag					MV1GetAttachAnimUseShapeFlag
#define NS_MV1GetAttachAnimFrameLocalPosition			MV1GetAttachAnimFrameLocalPosition
#define NS_MV1GetAttachAnimFrameLocalMatrix				MV1GetAttachAnimFrameLocalMatrix

#define NS_MV1GetAnimNum								MV1GetAnimNum
#define NS_MV1GetAnimName								MV1GetAnimName
#define NS_MV1SetAnimName								MV1SetAnimName
#define NS_MV1SetAnimNameWithStrLen						MV1SetAnimNameWithStrLen
#define NS_MV1GetAnimIndex								MV1GetAnimIndex
#define NS_MV1GetAnimIndexWithStrLen					MV1GetAnimIndexWithStrLen
#define NS_MV1GetAnimTotalTime							MV1GetAnimTotalTime
#define NS_MV1GetAnimLoopFlag							MV1GetAnimLoopFlag
#define NS_MV1GetAnimTargetFrameNum						MV1GetAnimTargetFrameNum
#define NS_MV1GetAnimTargetFrameName					MV1GetAnimTargetFrameName
#define NS_MV1GetAnimTargetFrame						MV1GetAnimTargetFrame
#define NS_MV1GetAnimTargetFrameKeySetNum				MV1GetAnimTargetFrameKeySetNum
#define NS_MV1GetAnimTargetFrameKeySet					MV1GetAnimTargetFrameKeySet

#define NS_MV1GetAnimKeySetNum							MV1GetAnimKeySetNum
#define NS_MV1GetAnimKeySetType							MV1GetAnimKeySetType
#define NS_MV1GetAnimKeySetDataType						MV1GetAnimKeySetDataType
#define NS_MV1GetAnimKeySetTimeType						MV1GetAnimKeySetTimeType
#define NS_MV1GetAnimKeySetDataNum						MV1GetAnimKeySetDataNum
#define NS_MV1GetAnimKeyDataTime						MV1GetAnimKeyDataTime
#define NS_MV1GetAnimKeyDataIndexFromTime				MV1GetAnimKeyDataIndexFromTime
#define NS_MV1GetAnimKeyDataToQuaternion				MV1GetAnimKeyDataToQuaternion
#define NS_MV1GetAnimKeyDataToQuaternionFromTime		MV1GetAnimKeyDataToQuaternionFromTime
#define NS_MV1GetAnimKeyDataToVector					MV1GetAnimKeyDataToVector
#define NS_MV1GetAnimKeyDataToVectorFromTime			MV1GetAnimKeyDataToVectorFromTime
#define NS_MV1GetAnimKeyDataToMatrix					MV1GetAnimKeyDataToMatrix
#define NS_MV1GetAnimKeyDataToMatrixFromTime			MV1GetAnimKeyDataToMatrixFromTime
#define NS_MV1GetAnimKeyDataToFlat						MV1GetAnimKeyDataToFlat
#define NS_MV1GetAnimKeyDataToFlatFromTime				MV1GetAnimKeyDataToFlatFromTime
#define NS_MV1GetAnimKeyDataToLinear					MV1GetAnimKeyDataToLinear
#define NS_MV1GetAnimKeyDataToLinearFromTime			MV1GetAnimKeyDataToLinearFromTime

// マテリアル関係
#define NS_MV1GetMaterialNum							MV1GetMaterialNum
#define NS_MV1GetMaterialName							MV1GetMaterialName
#define NS_MV1SetMaterialType							MV1SetMaterialType
#define NS_MV1GetMaterialType							MV1GetMaterialType
#define NS_MV1SetMaterialTypeParam						MV1SetMaterialTypeParam
#define NS_MV1SetMaterialDifColor						MV1SetMaterialDifColor
#define NS_MV1GetMaterialDifColor						MV1GetMaterialDifColor
#define NS_MV1SetMaterialSpcColor						MV1SetMaterialSpcColor
#define NS_MV1GetMaterialSpcColor						MV1GetMaterialSpcColor
#define NS_MV1SetMaterialEmiColor						MV1SetMaterialEmiColor
#define NS_MV1GetMaterialEmiColor						MV1GetMaterialEmiColor
#define NS_MV1SetMaterialAmbColor						MV1SetMaterialAmbColor
#define NS_MV1GetMaterialAmbColor						MV1GetMaterialAmbColor
#define NS_MV1SetMaterialSpcPower						MV1SetMaterialSpcPower
#define NS_MV1GetMaterialSpcPower						MV1GetMaterialSpcPower
#define NS_MV1SetMaterialDifMapTexture					MV1SetMaterialDifMapTexture
#define NS_MV1GetMaterialDifMapTexture					MV1GetMaterialDifMapTexture
#define NS_MV1SetMaterialSpcMapTexture					MV1SetMaterialSpcMapTexture
#define NS_MV1GetMaterialSpcMapTexture					MV1GetMaterialSpcMapTexture
#define NS_MV1GetMaterialNormalMapTexture				MV1GetMaterialNormalMapTexture
#define NS_MV1SetMaterialDifGradTexture					MV1SetMaterialDifGradTexture
#define NS_MV1GetMaterialDifGradTexture					MV1GetMaterialDifGradTexture
#define NS_MV1SetMaterialSpcGradTexture					MV1SetMaterialSpcGradTexture
#define NS_MV1GetMaterialSpcGradTexture					MV1GetMaterialSpcGradTexture
#define NS_MV1SetMaterialSphereMapTexture				MV1SetMaterialSphereMapTexture
#define NS_MV1GetMaterialSphereMapTexture				MV1GetMaterialSphereMapTexture
#define NS_MV1SetMaterialDifGradBlendType				MV1SetMaterialDifGradBlendType
#define NS_MV1GetMaterialDifGradBlendType				MV1GetMaterialDifGradBlendType
#define NS_MV1SetMaterialSpcGradBlendType				MV1SetMaterialSpcGradBlendType
#define NS_MV1GetMaterialSpcGradBlendType				MV1GetMaterialSpcGradBlendType
#define NS_MV1SetMaterialSphereMapBlendType				MV1SetMaterialSphereMapBlendType
#define NS_MV1GetMaterialSphereMapBlendType				MV1GetMaterialSphereMapBlendType
#define NS_MV1SetMaterialOutLineWidth					MV1SetMaterialOutLineWidth
#define NS_MV1GetMaterialOutLineWidth					MV1GetMaterialOutLineWidth
#define NS_MV1SetMaterialOutLineDotWidth				MV1SetMaterialOutLineDotWidth
#define NS_MV1GetMaterialOutLineDotWidth				MV1GetMaterialOutLineDotWidth
#define NS_MV1SetMaterialOutLineColor					MV1SetMaterialOutLineColor
#define NS_MV1GetMaterialOutLineColor					MV1GetMaterialOutLineColor
#define NS_MV1SetMaterialDrawBlendMode					MV1SetMaterialDrawBlendMode
#define NS_MV1SetMaterialDrawBlendParam					MV1SetMaterialDrawBlendParam
#define NS_MV1GetMaterialDrawBlendMode					MV1GetMaterialDrawBlendMode
#define NS_MV1GetMaterialDrawBlendParam					MV1GetMaterialDrawBlendParam
#define NS_MV1SetMaterialDrawAlphaTest					MV1SetMaterialDrawAlphaTest
#define NS_MV1GetMaterialDrawAlphaTestEnable			MV1GetMaterialDrawAlphaTestEnable
#define NS_MV1GetMaterialDrawAlphaTestMode				MV1GetMaterialDrawAlphaTestMode
#define NS_MV1GetMaterialDrawAlphaTestParam				MV1GetMaterialDrawAlphaTestParam
#define NS_MV1SetMaterialTypeAll						MV1SetMaterialTypeAll
#define NS_MV1SetMaterialTypeParamAll					MV1SetMaterialTypeParamAll
#define NS_MV1SetMaterialDifGradBlendTypeAll			MV1SetMaterialDifGradBlendTypeAll
#define NS_MV1SetMaterialSpcGradBlendTypeAll			MV1SetMaterialSpcGradBlendTypeAll
#define NS_MV1SetMaterialSphereMapBlendTypeAll			MV1SetMaterialSphereMapBlendTypeAll
#define NS_MV1SetMaterialOutLineWidthAll				MV1SetMaterialOutLineWidthAll
#define NS_MV1SetMaterialOutLineDotWidthAll				MV1SetMaterialOutLineDotWidthAll
#define NS_MV1SetMaterialOutLineColorAll				MV1SetMaterialOutLineColorAll
#define NS_MV1SetMaterialDrawBlendModeAll				MV1SetMaterialDrawBlendModeAll
#define NS_MV1SetMaterialDrawBlendParamAll				MV1SetMaterialDrawBlendParamAll
#define NS_MV1SetMaterialDrawAlphaTestAll				MV1SetMaterialDrawAlphaTestAll

// テクスチャ関係
#define NS_MV1GetTextureNum								MV1GetTextureNum
#define NS_MV1GetTextureName							MV1GetTextureName
#define NS_MV1SetTextureColorFilePath					MV1SetTextureColorFilePath
#define NS_MV1SetTextureColorFilePathWithStrLen			MV1SetTextureColorFilePathWithStrLen
#define NS_MV1GetTextureColorFilePath					MV1GetTextureColorFilePath
#define NS_MV1SetTextureAlphaFilePath					MV1SetTextureAlphaFilePath
#define NS_MV1SetTextureAlphaFilePathWithStrLen			MV1SetTextureAlphaFilePathWithStrLen
#define NS_MV1GetTextureAlphaFilePath					MV1GetTextureAlphaFilePath
#define NS_MV1SetTextureGraphHandle						MV1SetTextureGraphHandle
#define NS_MV1GetTextureGraphHandle						MV1GetTextureGraphHandle
#define NS_MV1SetTextureAddressMode						MV1SetTextureAddressMode
#define NS_MV1GetTextureAddressModeU					MV1GetTextureAddressModeU
#define NS_MV1GetTextureAddressModeV					MV1GetTextureAddressModeV
#define NS_MV1GetTextureWidth							MV1GetTextureWidth
#define NS_MV1GetTextureHeight							MV1GetTextureHeight
#define NS_MV1GetTextureSemiTransState					MV1GetTextureSemiTransState
#define NS_MV1SetTextureBumpImageFlag					MV1SetTextureBumpImageFlag
#define NS_MV1GetTextureBumpImageFlag					MV1GetTextureBumpImageFlag
#define NS_MV1SetTextureBumpImageNextPixelLength		MV1SetTextureBumpImageNextPixelLength
#define NS_MV1GetTextureBumpImageNextPixelLength		MV1GetTextureBumpImageNextPixelLength
#define NS_MV1SetTextureSampleFilterMode				MV1SetTextureSampleFilterMode
#define NS_MV1GetTextureSampleFilterMode				MV1GetTextureSampleFilterMode
#define NS_MV1LoadTexture								MV1LoadTexture
#define NS_MV1LoadTextureWithStrLen						MV1LoadTextureWithStrLen


// フレーム関係
#define NS_MV1GetFrameNum								MV1GetFrameNum
#define NS_MV1SearchFrame								MV1SearchFrame
#define NS_MV1SearchFrameWithStrLen						MV1SearchFrameWithStrLen
#define NS_MV1SearchFrameChild							MV1SearchFrameChild
#define NS_MV1SearchFrameChildWithStrLen				MV1SearchFrameChildWithStrLen
#define NS_MV1GetFrameName								MV1GetFrameName
#define NS_MV1GetFrameName2								MV1GetFrameName2
#define NS_MV1GetFrameParent							MV1GetFrameParent
#define NS_MV1GetFrameChildNum							MV1GetFrameChildNum
#define NS_MV1GetFrameChild								MV1GetFrameChild
#define NS_MV1GetFramePosition							MV1GetFramePosition
#define NS_MV1GetFramePositionD							MV1GetFramePositionD
#define NS_MV1GetFrameBaseLocalMatrix					MV1GetFrameBaseLocalMatrix
#define NS_MV1GetFrameBaseLocalMatrixD					MV1GetFrameBaseLocalMatrixD
#define NS_MV1GetFrameLocalMatrix						MV1GetFrameLocalMatrix
#define NS_MV1GetFrameLocalMatrixD						MV1GetFrameLocalMatrixD
#define NS_MV1GetFrameLocalWorldMatrix					MV1GetFrameLocalWorldMatrix
#define NS_MV1GetFrameLocalWorldMatrixD					MV1GetFrameLocalWorldMatrixD
#define NS_MV1SetFrameUserLocalMatrix					MV1SetFrameUserLocalMatrix
#define NS_MV1SetFrameUserLocalMatrixD					MV1SetFrameUserLocalMatrixD
#define NS_MV1ResetFrameUserLocalMatrix					MV1ResetFrameUserLocalMatrix
#define NS_MV1SetFrameUserLocalWorldMatrix				MV1SetFrameUserLocalWorldMatrix
#define NS_MV1SetFrameUserLocalWorldMatrixD				MV1SetFrameUserLocalWorldMatrixD
#define NS_MV1ResetFrameUserLocalWorldMatrix			MV1ResetFrameUserLocalWorldMatrix
#define NS_MV1GetFrameMaxVertexLocalPosition			MV1GetFrameMaxVertexLocalPosition
#define NS_MV1GetFrameMaxVertexLocalPositionD			MV1GetFrameMaxVertexLocalPositionD
#define NS_MV1GetFrameMinVertexLocalPosition			MV1GetFrameMinVertexLocalPosition
#define NS_MV1GetFrameMinVertexLocalPositionD			MV1GetFrameMinVertexLocalPositionD
#define NS_MV1GetFrameAvgVertexLocalPosition			MV1GetFrameAvgVertexLocalPosition
#define NS_MV1GetFrameAvgVertexLocalPositionD			MV1GetFrameAvgVertexLocalPositionD
#define NS_MV1GetFrameTriangleNum						MV1GetFrameTriangleNum
#define NS_MV1GetFrameMeshNum							MV1GetFrameMeshNum
#define NS_MV1GetFrameMesh								MV1GetFrameMesh
#define NS_MV1SetFrameVisible							MV1SetFrameVisible
#define NS_MV1GetFrameVisible							MV1GetFrameVisible
#define NS_MV1SetFrameDifColorScale						MV1SetFrameDifColorScale
#define NS_MV1SetFrameSpcColorScale						MV1SetFrameSpcColorScale
#define NS_MV1SetFrameEmiColorScale						MV1SetFrameEmiColorScale
#define NS_MV1SetFrameAmbColorScale						MV1SetFrameAmbColorScale
#define NS_MV1GetFrameDifColorScale						MV1GetFrameDifColorScale
#define NS_MV1GetFrameSpcColorScale						MV1GetFrameSpcColorScale
#define NS_MV1GetFrameEmiColorScale						MV1GetFrameEmiColorScale
#define NS_MV1GetFrameAmbColorScale						MV1GetFrameAmbColorScale
#define NS_MV1GetFrameSemiTransState					MV1GetFrameSemiTransState
#define NS_MV1SetFrameOpacityRate						MV1SetFrameOpacityRate
#define NS_MV1GetFrameOpacityRate						MV1GetFrameOpacityRate
#define NS_MV1SetFrameBaseVisible						MV1SetFrameBaseVisible
#define NS_MV1GetFrameBaseVisible						MV1GetFrameBaseVisible
#define NS_MV1SetFrameTextureAddressTransform			MV1SetFrameTextureAddressTransform
#define NS_MV1SetFrameTextureAddressTransformMatrix		MV1SetFrameTextureAddressTransformMatrix
#define NS_MV1ResetFrameTextureAddressTransform			MV1ResetFrameTextureAddressTransform

// メッシュ関係
#define NS_MV1GetMeshNum								MV1GetMeshNum
#define NS_MV1GetMeshMaterial							MV1GetMeshMaterial
#define NS_MV1GetMeshTriangleNum						MV1GetMeshTriangleNum
#define NS_MV1SetMeshVisible							MV1SetMeshVisible
#define NS_MV1GetMeshVisible							MV1GetMeshVisible
#define NS_MV1SetMeshDifColorScale						MV1SetMeshDifColorScale
#define NS_MV1SetMeshSpcColorScale						MV1SetMeshSpcColorScale
#define NS_MV1SetMeshEmiColorScale						MV1SetMeshEmiColorScale
#define NS_MV1SetMeshAmbColorScale						MV1SetMeshAmbColorScale
#define NS_MV1GetMeshDifColorScale						MV1GetMeshDifColorScale
#define NS_MV1GetMeshSpcColorScale						MV1GetMeshSpcColorScale
#define NS_MV1GetMeshEmiColorScale						MV1GetMeshEmiColorScale
#define NS_MV1GetMeshAmbColorScale						MV1GetMeshAmbColorScale
#define NS_MV1SetMeshOpacityRate						MV1SetMeshOpacityRate
#define NS_MV1GetMeshOpacityRate						MV1GetMeshOpacityRate
#define NS_MV1SetMeshDrawBlendMode						MV1SetMeshDrawBlendMode
#define NS_MV1SetMeshDrawBlendParam						MV1SetMeshDrawBlendParam
#define NS_MV1GetMeshDrawBlendMode						MV1GetMeshDrawBlendMode
#define NS_MV1GetMeshDrawBlendParam						MV1GetMeshDrawBlendParam
#define NS_MV1SetMeshBaseVisible						MV1SetMeshBaseVisible
#define NS_MV1GetMeshBaseVisible						MV1GetMeshBaseVisible
#define NS_MV1SetMeshBackCulling						MV1SetMeshBackCulling
#define NS_MV1GetMeshBackCulling						MV1GetMeshBackCulling
#define NS_MV1GetMeshMaxPosition						MV1GetMeshMaxPosition
#define NS_MV1GetMeshMinPosition						MV1GetMeshMinPosition
#define NS_MV1GetMeshTListNum							MV1GetMeshTListNum
#define NS_MV1GetMeshTList								MV1GetMeshTList
#define NS_MV1GetMeshSemiTransState						MV1GetMeshSemiTransState
#define NS_MV1SetMeshUseVertDifColor					MV1SetMeshUseVertDifColor
#define NS_MV1SetMeshUseVertSpcColor					MV1SetMeshUseVertSpcColor
#define NS_MV1GetMeshUseVertDifColor					MV1GetMeshUseVertDifColor
#define NS_MV1GetMeshUseVertSpcColor					MV1GetMeshUseVertSpcColor
#define NS_MV1GetMeshShapeFlag							MV1GetMeshShapeFlag

// シェイプ関係
#define NS_MV1GetShapeNum								MV1GetShapeNum
#define NS_MV1SearchShape								MV1SearchShape
#define NS_MV1SearchShapeWithStrLen						MV1SearchShapeWithStrLen
#define NS_MV1GetShapeName								MV1GetShapeName
#define NS_MV1GetShapeTargetMeshNum						MV1GetShapeTargetMeshNum
#define NS_MV1GetShapeTargetMesh						MV1GetShapeTargetMesh
#define NS_MV1SetShapeRate								MV1SetShapeRate
#define NS_MV1GetShapeRate								MV1GetShapeRate
#define NS_MV1GetShapeApplyRate							MV1GetShapeApplyRate

// トライアングルリスト関係
#define NS_MV1GetTriangleListNum						MV1GetTriangleListNum
#define NS_MV1GetTriangleListVertexType					MV1GetTriangleListVertexType
#define NS_MV1GetTriangleListPolygonNum					MV1GetTriangleListPolygonNum
#define NS_MV1GetTriangleListVertexNum					MV1GetTriangleListVertexNum
#define NS_MV1GetTriangleListLocalWorldMatrixNum		MV1GetTriangleListLocalWorldMatrixNum
#define NS_MV1GetTriangleListLocalWorldMatrix			MV1GetTriangleListLocalWorldMatrix
#define NS_MV1GetTriangleListPolygonVertexPosition		MV1GetTriangleListPolygonVertexPosition
#define NS_MV1GetTriangleListUseMaterial				MV1GetTriangleListUseMaterial

// コリジョン関係
#define NS_MV1SetupCollInfo								MV1SetupCollInfo
#define NS_MV1TerminateCollInfo							MV1TerminateCollInfo
#define NS_MV1RefreshCollInfo							MV1RefreshCollInfo
#define NS_MV1CollCheck_Line							MV1CollCheck_Line
#define NS_MV1CollCheck_LineDim							MV1CollCheck_LineDim
#define NS_MV1CollCheck_Sphere							MV1CollCheck_Sphere
#define NS_MV1CollCheck_Capsule							MV1CollCheck_Capsule
#define NS_MV1CollCheck_Triangle						MV1CollCheck_Triangle
#define NS_MV1CollCheck_GetResultPoly					MV1CollCheck_GetResultPoly
#define NS_MV1CollResultPolyDimTerminate				MV1CollResultPolyDimTerminate

// 参照用メッシュ関係
#define NS_MV1SetupReferenceMesh						MV1SetupReferenceMesh
#define NS_MV1TerminateReferenceMesh					MV1TerminateReferenceMesh
#define NS_MV1RefreshReferenceMesh						MV1RefreshReferenceMesh
#define NS_MV1GetReferenceMesh							MV1GetReferenceMesh

#endif  // DX_THREAD_SAFE

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_NON_MODEL

#endif // DX_NON_GRAPHICS

#endif
