// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		Ｘファイル読み込みプログラム
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#define __DX_MAKE

#include "DxModelRead.h"

#ifndef DX_NON_MODEL

// インクルード ---------------------------------
#include "DxLog.h"
#include "DxChar.h"
#include "DxMemory.h"
#include "DxSystem.h"
#include "DxBaseFunc.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 -----------------------------------

#define TOKEN_NAME					(1)
#define TOKEN_STRING				(2)
#define TOKEN_INTEGER				(3)
#define TOKEN_GUID					(5)
#define TOKEN_INTEGER_LIST			(6)
#define TOKEN_FLOAT_LIST			(7)

#define TOKEN_OBRACE				(10)
#define TOKEN_CBRACE				(11)
#define TOKEN_OPAREN				(12)
#define TOKEN_CPAREN				(13)
#define TOKEN_OBRACKET				(14)
#define TOKEN_CBRACKET				(15)
#define TOKEN_OANGLE				(16)
#define TOKEN_CANGLE				(17)
#define TOKEN_DOT					(18)
#define TOKEN_COMMA					(19)
#define TOKEN_SEMICOLON				(20)
#define TOKEN_TEMPLATE				(31)
#define TOKEN_WORD					(40)
#define TOKEN_DWORD					(41)
#define TOKEN_FLOAT					(42)
#define TOKEN_DOUBLE				(43)
#define TOKEN_CHAR					(44)
#define TOKEN_UCHAR					(45)
#define TOKEN_SWORD					(46)
#define TOKEN_SDWORD				(47)
#define TOKEN_VOID					(48)
#define TOKEN_LPSTR					(49)
#define TOKEN_UNICODE				(50)
#define TOKEN_CSTRING				(51)
#define TOKEN_ARRAY					(52)

#define MAX_STACKNUM				(64)			// 階層構造の最大数
#define MAX_MODELNUM				(1000)			// モデルデータの最大数
#define MAX_INDEXNUM				(32)			// 最大頂点インデックス数

// テンプレートタイプ定義
#define TEMP_STRING					(0)				// 文字列
#define TEMP_VECTOR					(1)				// ベクトルテンプレート
#define TEMP_COORDS2D				(2)				// テクスチャ座標２次元
#define TEMP_MATRIX4X4				(3)				// 4 x 4 行列
#define TEMP_COLORRGBA				(4)				// アルファ付きカラー
#define TEMP_COLORRGB				(5)				// カラー
#define TEMP_MATERIAL				(6)				// マテリアル
#define TEMP_TEXTUREFILENAME		(7)				// テクスチャファイルネーム
#define TEMP_MESHFACE				(8)				// メッシュフェイス
#define TEMP_MESHTEXTURECOORDS		(9)				// メッシュテクスチャ座標
#define TEMP_MESHNORMALS			(10)			// メッシュの法線
#define TEMP_MESHMATERIALLIST		(11)			// メッシュマテリアルリスト
#define TEMP_INDEXEDCOLOR			(12)			// インデックスカラー
#define TEMP_MESHVERTEXCOLORS		(13)			// メッシュ頂点カラー
#define TEMP_MESH					(14)			// メッシュ
#define TEMP_FRAMETRANSFORMMATRIX	(15)			// フレームのローカルトランスフォーム
#define TEMP_FRAME					(16)			// フレーム
#define TEMP_FLOATKEYS				(17)			// 浮動小数点数の配列及び配列内の float の数
#define TEMP_TIMEDFLOATKEYS			(18)			// アニメーションで使用される浮動小数点数のセット及び正の時間
#define TEMP_ANIMATIONKEY			(19)			// アニメーションキーのセット
#define TEMP_ANIMATIONOPTIONS		(20)			// アニメーションオプション
#define TEMP_ANIMATION				(21)			// 前のフレームを参照するアニメーションを格納する
#define TEMP_ANIMATIONSET			(22)			// Animation オブジェクトを格納する
#define TEMP_XSKINMESHHEADER		(23)			// スキニングメッシュの情報
#define TEMP_SKINWEIGHTS			(24)			// スキンメッシュのウエイト
#define TEMP_XFILE					(25)			// Ｘファイル
#define TEMP_DWORD					(26)			// ＤＷＯＲＤ
#define TEMP_WORD					(27)			// ＷＯＲＤ
#define TEMP_FLOAT					(28)			// ＦＬＯＡＴ
#define TEMP_NUM					(29)			// 対応しているテンプレートの数


// テンプレートの文字列テーブル
const char *TTable[29] = 
{
	"",
	"",
	"Coords2d",
	"Matrix4x4",
	"ColorRGBA",
	"ColorRGB",
	"Material",
	"TextureFilename",
	"MeshFace",
	"MeshTextureCoords",
	"MeshNormals",
	"MeshMaterialList",
	"IndexedColor",
	"MeshVertexColors",
	"Mesh",
	"FrameTransformMatrix",
	"Frame",
	"FloatKeys",
	"TimedFloatKeys",
	"AnimationKey",
	"AnimationOption",
	"Animation",
	"AnimationSet",
	"XSkinMeshHeader",
	"SkinWeights",
	"",
	"",
	"",
	""
} ;

// オブジェクトデータアドレス参照用マクロ
#define DOFF( x ) ( ( BYTE * )(x) + sizeof( X_OBJECT ) )

// オブジェクトのアドレス参照用マクロ
#define OOFF( x ) ( ( X_OBJECT * )( ( BYTE * )(x) - sizeof( X_OBJECT ) ) )

// デフォルトのスキップ文字列
const char *DefSkipStr = " \r\n\t,;" ;

// データ型宣言 ---------------------------------

// 文字列解析用文字列データ型
struct X_PSTRING
{
	char					*StrBuf ;							// 文字列データ
	char					SkipStr[40] ;						// スキップ文字列
	int						SkipStrLen ;						// スキップ文字列の長さ
	int						StrOffset ;							// 文字列座標
	int						StrSize ;							// 文字列のサイズ
	bool					binf ;								// バイナリデータか( true:バイナリデータ  false:テキストデータ )
	bool					f64f ;								// 浮動小数点データは６４ビット型か( true:６４ビット  false:３２ビット )
} ;

// ストリングデータ型
struct X_STRING
{
	char					*String ;
} ;

// ＤＷＯＲＤ
struct X_DWORD
{
	DWORD					d ;
} ;

// ＷＯＲＤ
struct X_WORD
{
	WORD					w ;
} ;

// ＦＬＯＡＴ
struct X_FLOAT
{
	float					f ;
} ;

// ベクトル
struct X_VECTOR
{
	union
	{
		struct 
		{
			float			x, y, z ;
		} dat ;
		VECTOR				v ;
	} ;
} ;

// コードス２Ｄ
struct X_COORDS2D
{
	float					u ;
	float					v ;
} ;

// ４ｘ４行列
struct X_MATRIX4X4
{
	union
	{
		float				matrix[4][4] ;
		MATRIX				ms ;
	} ;
} ;

// アルファ付きカラーデータ
struct X_COLORRGBA
{
	float					red ;
	float					green ;
	float					blue ;
	float					alpha ;
} ;

// カラーデータ
struct X_COLORRGB
{
	float					red ;
	float					green ;
	float					blue ;
} ;

// マテリアル
struct X_MATERIAL
{
	X_COLORRGBA				faceColor ;
	float					power ;
	X_COLORRGB				specularColor ;
	X_COLORRGB				emissiveColor ;

	struct X_TEXTUREFILENAME *TextureFileName ;					// このマテリアルに関連付けられているテクスチャファイルネーム

	MV1_MATERIAL_R			*MV1RMaterial ;						// このマテリアルに関連付けられた MV1_MATERIAL_R へのポインタ
	int						Index ;								// インデックス
} ;

// テクスチャファイルネーム
struct X_TEXTUREFILENAME
{
	X_STRING				filename ;							// テクスチャファイル名
	int						Index ;								// インデックス
} ;

// メッシュ面
struct X_MESHFACE
{
	DWORD					nFaceVertexIndices ;				// 面定義インデックスの数
	DWORD					faceVertexIndicesFixedLength[ MAX_INDEXNUM ] ;	// 面定義インデックス( 固定長バッファ )
	DWORD					*faceVertexIndices ;				// 面定義インデックス( 固定長バッファに収まらないときに使用 )

} ;

// メッシュテクスチャー座標
struct X_MESHTEXTURECOORDS
{
	DWORD					nTextureCoords ;					// テクスチャ座標の数
	X_COORDS2D				*textureCoords ;					// テクスチャ座標
} ;

// メッシュ法線
struct X_MESHNORMALS
{
	DWORD					nNormals ;							// 法線の数
	X_VECTOR				*normals ;							// 法線
	DWORD					nFaceNormals ;						// 法線面の数
	X_MESHFACE				*faceNormals ;						// 法線面
} ;

// メッシュマテリアルリスト
struct X_MESHMATERIALLIST
{
	DWORD					nMaterials ;						// マテリアルの種類の数
	DWORD					nFaceIndexes ;						// マテリアルインデックスの数
	DWORD					*FaceIndexes ;						// マテリアルインデックス

	X_MATERIAL				**MaterialList ;					// 関連付けらているマテリアルのリスト
} ;

// インデックスカラー
struct X_INDEXEDCOLOR
{
	DWORD					index ;								// インデックス
	X_COLORRGBA				indexColor ;						// カラー
} ;

// メッシュ頂点カラー
struct X_MESHVERTEXCOLORS
{
	DWORD					nVertexColors ;						// 頂点カラーの数
	X_INDEXEDCOLOR			*vertexColors ;						// 頂点カラー
} ;

// メッシュ
struct X_MESH
{
	DWORD					nVertices ;							// 座標の数
	X_VECTOR				*vertices ;							// 座標
	DWORD					nFaces ;							// 面の数
	X_MESHFACE				*faces ;							// 面
	DWORD					nTriangles ;						// 三角形ポリゴン換算のポリゴンの数
	DWORD					nTriangleListTotal ;				// 三角形ポリゴンリストの総数

	X_MESHTEXTURECOORDS		*TextureCoord ;						// テクスチャー座標
	X_MESHNORMALS			*Normals ;							// 法線データ
	X_MESHMATERIALLIST		*MaterialList ;						// マテリアルリスト
	X_MESHVERTEXCOLORS		*VertexColor ;						// 頂点カラー

	int						SkinWeightsNum ;					// スキンメッシュのボーンの数
	struct X_SKINWEIGHTS	**SkinWeights ;						// スキンメッシュのデータ位置
	struct X_FRAME			*Frame ;							// このメッシュのフレーム

	MV1_FRAME_R				*MeshFrame ;
} ;

// フレーム(及びそのすべての子オブジェクト)のローカルトランスフォーム
struct X_FRAMETRANSFORMMATRIX
{
	X_MATRIX4X4				frameMatrix ;
} ;

// フレーム
struct X_FRAME
{
	BYTE					Dummy ;

	X_FRAMETRANSFORMMATRIX	*TransformMatrix ;					// このフレームに関連付けられているトランスフォームマトリックス

	int                     MeshNum ;							// このフレームに関連付けられているメッシュの数
	X_MESH                  **Mesh ;							// このフレームに関連付けられているメッシュのデータ位置

	MV1_FRAME_R				*MV1Frame ;							// このフレームに関連付けられた MV1_FRAME_R 構造体へのポインタ
	int						Index ;								// このフレームのインデックス
} ;

// 浮動小数点数の配列及び配列内の float の数
struct X_FLOATKEYS
{
	DWORD					nValues ;							// 浮動小数点数の数
	float					*values ;							// 浮動小数点数
} ;

// アニメーションで使用される浮動小数点数のセット及び正の時間
struct X_TIMEDFLOATKEYS
{
	DWORD					time ;								// 時間
	X_FLOATKEYS				tfkeys ;							// アニメーションキー
} ;

// アニメーションキーのセット
struct X_ANIMATIONKEY
{
	DWORD					keyType ;							// アニメーションタイプ( 0:回転(クォータニオン)  1:拡大  2:平行移動  4:行列 )
	DWORD					nKeys ;								// キーの数
	X_TIMEDFLOATKEYS		*keys ;								// キー
	int						TotalCount ;						// キーの総時間
} ;

// アニメーションオプション
struct X_ANIMATIONOPTIONS
{
	DWORD					openclosed ;
	DWORD					positionquality ;
} ;

// アニメーション
struct X_ANIMATION
{
	BYTE					Dummy ;

	DWORD					AnimationKeysNum ;					// アニメーションキーの数
	X_ANIMATIONKEY			**AnimationKeys ;					// アニメーションキーのデータポインタ
	X_FRAME					*Frame ;							// 対応するフレームのデータポインタ
	X_ANIMATION				*ParentsAnimation ;					// 対応するフレームの親フレームに対応しているアニメーションデータ
	int						MaxCount ;							// 最大アニメーションカウント値
} ;

// アニメーションセット
struct X_ANIMATIONSET
{
	BYTE					Dummy ;

	int						KeyDataSize ;						// アニメーションキーデータサイズ
	int						MaxCount ;							// 最大カウント値
	int						AnimationNum ;						// アニメーション数
	X_ANIMATION				**Animation ;						// アニメーションデータ位置
} ;

// スキニングメッシュの情報
struct X_XSKINMESHHEADER
{
	WORD					nMaxSkinWeightsPerVertex ;
	WORD					nMaxSkinWeightsPerFace ;
	WORD					nBones ;
} ;

// スキンメッシュのウエイト
struct X_SKINWEIGHTS
{
	X_STRING				transformNodeName ;					// 変換に使用するボーンの名前
	DWORD					nWeights ;							// ウエイト値の数
	DWORD					*vertexIndices ;					// ウエイトを影響させる対象となるメッシュ上の頂点のインデックス
	float					*weights ;							// 頂点に対するウエイト
	X_MATRIX4X4				matrixOffset ;						// オフセット行列

	X_FRAME					*Frame ;							// 関わっているフレームのデータ位置
} ;

// 関連付けされているオブジェクトの情報
struct X_RELATION_OBJECT
{
	char					*ObjectName ;						// 関連付けされているオブジェクトの名前
	int						ObjectNum ;							// 関連付けされているオブジェクトの数
	int						ObjectMaxNum ;						// 関連付けできるオブジェクトの最大数
	struct X_OBJECT			**Object ;							// 関連付けされているオブジェクトへのポインタ
} ;

// オブジェクトデータ型
struct X_OBJECT
{
	int						Type ;								// テンプレートタイプ
	char					*Name ;								// オブジェクト名
	int						ObjectDataSize ;					// X_OBJECT 構造体のデータサイズを含めたオブジェクトのデータサイズ
	struct X_OBJECT			*ParentsData ;						// 親オブジェクトのデータポインタ
	int						ChildNum ;							// 子の数
	struct X_OBJECT			*ChildFirstData ;					// 子の最初のデータのポインタ
	struct X_OBJECT			*ChildLastData ;					// 子の最後のデータのポインタ
	struct X_OBJECT			*ChildNextData ;					// 自分と同じ親に所属している次の子のデータのポインタ

	int						RelationObjectNum ;					// 関連付けがされているオブジェクトの数
	int						RelationObjectMaxNum ;				// 関連付けができるオブジェクトの数
	X_RELATION_OBJECT		*RelationObject ;					// 関連付けがされているオブジェクトの情報の配列
//	struct X_OBJECT			**RelationObject ;					// 関連付けがされているオブジェクトへのポインタ
//	char					**RelationObjectName ;				// 関連付けがされているオブジェクトの名前

	struct X_OBJECT			*NextData ;							// 次のオブジェクトデータへのポインタ
} ;

// スタック処理用データ構造体型
struct X_STACK
{
	int						StackNum ;							// スタックに詰まれているオブジェクトの数
	X_OBJECT				*Stack[ MAX_STACKNUM ] ;			// スタックに詰まれているオブジェクトの情報
} ;

// Ｘファイルデータ型
struct X_XFILE
{
	BYTE					Dummy ;
} ;

// テクスチャファイルネームリスト
struct X_TEXTUREFILENAME_LIST
{
	X_TEXTUREFILENAME		*FileName ;
	X_TEXTUREFILENAME_LIST	*Next ;
} ;

// Ｘファイルモデルデータ型
struct X_MODEL
{
	X_OBJECT				*FirstObject ;						// 先端のオブジェクトデータへのポインタ
	X_OBJECT				*LastObject ;						// 末端のオブジェクトデータへのポインタ

	MEMINFO					*XModelMem ;						// X_MODEL 用のメモリアドレス保存データへのポインタ

	X_TEXTUREFILENAME_LIST	*FirstTextureFileName ;				// テクスチャファイルネームリスト

	int						FrameNum ;							// フレームの数
	int						MaterialNum ;						// マテリアルの数
	int						TextureNum ;						// テクスチャの数
	int						MeshNum ;							// メッシュの数
	int						SkinWeightNum ;						// スキンメッシュのウエイトに関する情報の数
	int						MeshMaterialNum ;					// メッシュで使用されているマテリアルの数
	int						TriangleListNum ;					// トライアングルリストの数
	int						TriangleNum ;						// トライアングルの数
	int						AnimKeyDataSize ;					// アニメーションキーに使用するデータのサイズ
	int						AnimKeySetNum ;						// アニメーションキーセットの数
	int						AnimNum ;							// アニメーションの数
	int						AnimSetNum ;						// アニメーションセットの数
	int						VertexDataSize ;					// 頂点データのサイズ
	int						IndexDataSize ;						// 頂点インデックスデータのサイズ
	int						StringSize ;						// 文字列保存用に必要なメモリのサイズ
} ;

// データ宣言 -----------------------------------

// 関数宣言 -------------------------------------

// クォータニオン関係
extern	int		QTCreate( FLOAT4 *Out, float Angle, VECTOR *Vect ) ;										// 指定軸・指定角回転するためのクォータニオンを作成する
extern	int		MulQT( FLOAT4 *Out, FLOAT4 *In1, FLOAT4 *In2 ) ;											// クォータニオン同士の掛け算
extern	int		MulQTVect( VECTOR *Out, FLOAT4 *In1, VECTOR *InV, FLOAT4 *In2 ) ;							// 任意のクォータニオンを使用してベクトルを回転させる

// ベクトル演算関係
extern	int		CrossVect( VECTOR *Out, VECTOR *In1, VECTOR *In2 ) ;										// ２ベクトルの外積を求める
extern	float	DotVect( VECTOR *In1, VECTOR *In2 ) ;														// ２ベクトルの内積を求める
extern	int		MultiplyMatrix( MATRIX *Out, MATRIX *In1, MATRIX *In2 ) ;									// 行列同士の掛け算

// 構文解析用補助関数
extern	int		StrCmp2( char *String, int StrLength, char *StrBuffer ) ;									// 二つ前の文字列を調べる関数
extern	char	*KakkoSkip( char *StrPoint ) ;																// 次に出てくる中括弧'{}'を読み飛ばす
extern	char	*StrTorkn( char *SertchStr, const char *SkipString ) ;										// strtokのパチもん

// 文字列解析系関数
extern	int		SetPStr(		X_PSTRING *PStrBuf, char *String, int StrOffset, const char *SkipStr, int StrSize = -1, bool binf = false, bool f64f = false ) ;	// 文字列解析用データをセットする
extern	int		PStrGet(		X_PSTRING *PStrBuf, char *StrBuf, size_t BufferBytes ) ;							// 次に出てくる文字列を取得する
extern	int		PStrGet(		X_PSTRING *PStrBuf, char *StrBuf, size_t BufferBytes, const char *AddSkipStr ) ;	// 次に出てくる文字列を取得する
extern	int		PStrGetNMP(		X_PSTRING *PStrBuf, char *StrBuf, size_t BufferBytes, const char *AddSkipStr = "" ) ;			// 検索オフセットを移動せずに次の文字列を得る
extern	int		PStrKakkoSkip(	X_PSTRING *PString ) ;														// 次に出てくる『{...}』を読み飛ばす
extern	int		PStrMove(		X_PSTRING *PStrBuf, const char *CmpStr ) ;									// 指定の文字列中のどれかの文字が来るまで読み飛ばす
extern	int		PStrMoveP(		X_PSTRING *PStrBuf, const char *CmpStr ) ;									// 指定の文字列中のどれかの文字の次の文字まで読み飛ばす
extern	int		PStrMoveNum(	X_PSTRING *PStrBuf ) ;														// 数字が出るまで読み飛ばす
extern	int		PStrGetNameBP(	X_PSTRING *PStrBuf, char *StrBuf, size_t BufferBytes ) ;					// バイナリデータの TOKEN_NAME を得た後トークンを進める
extern	int		PStrGetNameB(	X_PSTRING *PStrBuf, char *StrBuf, size_t BufferBytes ) ;					// バイナリデータの TOKEN_NAME を得る
extern	int		PStrMoveOneB(	X_PSTRING *PStrBuf ) ;														// 次のトークン位置へ移動する
extern	int		PStrMoveB(		X_PSTRING *PStrBuf, int Code ) ;											// 指定のトークンが来るまでトークンを読み飛ばす
extern	int		PStrMovePB(		X_PSTRING *PStrBuf, int Code ) ;											// 指定のトークンの次のトークンまで読み飛ばす
extern	int		PStrKakkoSkipB(	X_PSTRING *PString ) ;														// 次に出てくる『{...}』を読み飛ばす(バイナリバージョン)

#define PSTRC(x)			( ( unsigned char )( (x)->StrBuf[ (x)->StrOffset ] ) )							// ポイントしている文字を得る
#define PSTRP(x,y)			( &( (x)->StrBuf[ (x)->StrOffset + (y) ] ) )									// ポイントしているアドレスを得る
#define PSTRWORD(x,y)		( GET_MEM_WORD(  PSTRP( (x), (y) ) ) )											// ポイントしているアドレスのワード値を得る
#define PSTRDWORD(x,y)		( GET_MEM_DWORD( PSTRP( (x), (y) ) ) )											// ポイントしているアドレスのダブルワード値を得る
#define PSTRFLOAT(x,y)		( *( ( float          * )PSTRP( (x), (y) ) ) )									// ポイントしているアドレスのフロート値を得る
#define PSTRDOUBLE(x,y)		( ( float )( *( ( double * )PSTRP( (x), (y) ) ) ) )								// ポイントしているアドレスのダブル値を得る
#define PSTRMOV(x,y)		( (x)->StrOffset += (y) )														// ポイントしているアドレスを移動する

// Ｘファイル解析データ処理関連
static	X_OBJECT	*AddObject( int TempType, const char *Name, X_OBJECT *Parents, X_MODEL *Model ) ;		// オブジェクトデータを追加する
extern	int			TerminateXModel( X_MODEL *Model ) ;														// Ｘファイルの解析で得たデータを解放する
static	void		*GetRelationObject( int Type, X_OBJECT *Object ) ;										// 指定のオブジェクトに関連のある指定のタイプのオブジェクトを得る
static	int			EnumRelationObject( int Type, int *NumBuf, void **AddresBuf, X_OBJECT *Object ) ;		// 指定のオブジェクトに関連のある指定のタイプのオブジェクトを列挙する
static	void		*GetMatchNameObject( int Type, char *Name, X_MODEL *Model ) ;							// 指定の名前をもつオブジェクトを取得する
//static	X_OBJECT	*GetPrevObject( X_MODEL *Model, X_OBJECT *Object ) ;									// 指定のオブジェクトと同じ親を持つひとつ前のデータを取得する

extern	int			AnalysXFile( char *StringBuffer, int StrSize, X_MODEL *Model ) ;						// Ｘファイルのデータを解析し、読み込む
extern	int			AnalysXData( X_MODEL *Model, MV1_MODEL_R *RModel ) ;									// Ｘファイルのモデルデータを解析し、実行用データの準備を行う

static	int			IncStackObject( X_STACK *Stack, X_OBJECT *Object ) ;									// オブジェクトをスタックに加える
static	int			DecStackObject( X_STACK *Stack ) ;														// オブジェクトのスタックを一つ減らす

// テンプレート解析用関数
static	int			TempString(					X_PSTRING *PStr, X_MODEL *Model, X_STRING				*String					) ;	// 文字列テンプレートの解析
static	int			TempWord(					X_PSTRING *PStr, X_MODEL *Model, X_WORD					*word					) ;	// ダブルワードテンプレートの解析
static	int			TempDword(					X_PSTRING *PStr, X_MODEL *Model, X_DWORD				*Dword					) ;	// ダブルワードテンプレートの解析
static	int			TempFloat(					X_PSTRING *PStr, X_MODEL *Model, X_FLOAT				*Float					) ;	// フロートテンプレートの解析
static	int			TempVector(					X_PSTRING *PStr, X_MODEL *Model, X_VECTOR				*Vector					) ;	// ベクトルテンプレートの解析
static	int			TempCoords2d(				X_PSTRING *PStr, X_MODEL *Model, X_COORDS2D				*Coords2d				) ;	// テクスチャ座標２次元テンプレートの解析
static	int			TempMatrix4x4(				X_PSTRING *PStr, X_MODEL *Model, X_MATRIX4X4			*Matrix					) ;	// ４ｘ４行列テンプレートの解析
static	int			TempColorRGBA(				X_PSTRING *PStr, X_MODEL *Model, X_COLORRGBA			*ColorRGBA				) ;	// アルファつきカラーテンプレートの解析
static	int			TempColorRGB(				X_PSTRING *PStr, X_MODEL *Model, X_COLORRGB				*ColorRGB				) ;	// カラーテンプレートの解析
static	int			TempMaterial(				X_PSTRING *PStr, X_MODEL *Model, X_MATERIAL				*Material				) ;	// マテリアルテンプレートの解析
static	int			TempTextureFilename(		X_PSTRING *PStr, X_MODEL *Model, X_TEXTUREFILENAME		*TexFileName			) ;	// テクスチャファイルネームテンプレートの解析
static	int			TempMeshFace(				X_PSTRING *PStr, X_MODEL *Model, X_MESHFACE				*MeshFace				) ;	// メッシュフェイステンプレートの解析
static	int			TempMeshTextureCoords(		X_PSTRING *PStr, X_MODEL *Model, X_MESHTEXTURECOORDS	*MeshTextureCoords		) ;	// メッシュテクスチャーコードステンプレートの解析
static	int			TempMeshNormals(			X_PSTRING *PStr, X_MODEL *Model, X_MESHNORMALS			*MeshNormals			) ;	// メッシュノーマルステンプレートの解析
static	int			TempMeshMaterialList(		X_PSTRING *PStr, X_MODEL *Model, X_MESHMATERIALLIST		*MeshMaterialList		) ;	// メッシュマテリアルリストテンプレートの解析
static	int			TempIndexedColor(			X_PSTRING *PStr, X_MODEL *Model, X_INDEXEDCOLOR			*IndexedColor			) ;	// インデックスカラーの解析
static	int			TempMeshVertexColors(		X_PSTRING *PStr, X_MODEL *Model, X_MESHVERTEXCOLORS		*MeshVertexColors		) ;	// メッシュ頂点カラーの解析
static	int			TempMesh(					X_PSTRING *PStr, X_MODEL *Model, X_MESH					*Mesh					) ;	// メッシュテンプレートの解析
static	int			TempFrameTransformMatrix(	X_PSTRING *PStr, X_MODEL *Model, X_FRAMETRANSFORMMATRIX	*FrameTransformMatrix	) ;	// フレーム(及びそのすべてのこオブジェクト)のローカルトランスフォームテンプレートの解析
static	int			TempFrame(					X_PSTRING *PStr, X_MODEL *Model, X_FRAME				*Frame					) ;	// フレームテンプレートの解析
static	int			TempFloatKeys(				X_PSTRING *PStr, X_MODEL *Model, X_FLOATKEYS			*FloatKeys				) ;	// 浮動小数点の配列及び配列内の float の数テンプレートの解析
static	int			TempTimedFloatKeys(			X_PSTRING *PStr, X_MODEL *Model, X_TIMEDFLOATKEYS		*TimedFloatKeys			) ;	// アニメーションで使用される浮動小数点数のセット及び時間を定義するテンプレートの解析
static	int			TempAnimationKey(			X_PSTRING *PStr, X_MODEL *Model, X_ANIMATIONKEY			*AnimationKey			) ;	// アニメーションキーのセットテンプレートの解析
static	int			TempAnimationOptions(		X_PSTRING *PStr, X_MODEL *Model, X_ANIMATIONOPTIONS		*AnimationOptions		) ;	// アニメーションオプションテンプレートの解析
static	int			TempAnimation(				X_PSTRING *PStr, X_MODEL *Model, X_ANIMATION			*Animation				) ;	// アニメーションテンプレートの解析
static	int			TempAnimationSet(			X_PSTRING *PStr, X_MODEL *Model, X_ANIMATIONSET			*AnimationSet			) ;	// アニメーションセットテンプレートの解析
static	int			TempXSkinMeshHeader(		X_PSTRING *PStr, X_MODEL *Model, X_XSKINMESHHEADER		*XSkinMeshHeader		) ;	// スキニングメッシュのヘッダ情報テンプレートの解析
static	int			TempSkinWeights(			X_PSTRING *PStr, X_MODEL *Model, X_SKINWEIGHTS			*SkinWeights			) ;	// スキンメッシュのウエイト情報テンプレートの解析

// プログラム -----------------------------------

// クォータニオン関係

// 回転用クォータニオンの作成
int QTCreate( FLOAT4 *Out, float Angle, VECTOR *Vect )
{
	float r ;
	float SinA ;

	r = ( float )_SQRT( Vect->x * Vect->x + Vect->y * Vect->y + Vect->z * Vect->z ) ;  
	Vect->x /= r ;
	Vect->y /= r ;
	Vect->z /= r ;

	_SINCOS_PLATFORM( Angle / 20.0f, &SinA, &Out->w ) ;
//	SinA = ( float )sin( Angle / 20.0f ) ;
//	Out->w = ( float )cos( Angle / 20.0f ) ;
	Out->x = SinA * Vect->x ;
	Out->y = SinA * Vect->y ;
	Out->z = SinA * Vect->z ;

	return 0 ;
}

// クォータニオンの掛け算
int MulQT( FLOAT4 *Out, FLOAT4 *In1, FLOAT4 *In2 )
{
	VECTOR VectBuf ;

	Out->w = In1->w * In2->w - DotVect( ( VECTOR * )&In1->x, ( VECTOR * )&In2->x ) ; 
	CrossVect( &VectBuf, ( VECTOR * )&In1->x, ( VECTOR * )&In2->x ) ;
	Out->x = In1->w * In2->x + In2->w * In1->x + VectBuf.x ;
	Out->y = In1->w * In2->y + In2->w * In1->y + VectBuf.y ;
	Out->z = In1->w * In2->z + In2->w * In1->z + VectBuf.z ;

	return 0 ;
}

// ベクトルの回転
int MulQTVect( VECTOR *Out, FLOAT4 *In1, VECTOR *InV, FLOAT4 *In2 )
{
	FLOAT4 Buf, OutBuf1, OutBuf2 ;

	*( ( VECTOR * )&Buf ) = *InV ;
	Buf.w = 0.0f  ;

	MulQT( &OutBuf1, In1, &Buf ) ;
	MulQT( &OutBuf2, &OutBuf1, In2 ) ;

	*Out = *( ( VECTOR * )&OutBuf2 ) ;

	return 0;
}










// ベクトル演算関係

// ベクトルの外積
int CrossVect( VECTOR *Out, VECTOR *In1, VECTOR *In2 )
{
	Out->x = ( In1->y * In2->z - In1->z * In2->y ) ;
	Out->y = -( In1->x * In2->z - In1->z * In2->x ) ;
	Out->z = ( In1->x * In2->y - In1->y * In2->x ) ;

	return 0 ;
}

// ベクトルの内積
float DotVect( VECTOR *In1, VECTOR *In2 )
{
	return ( In1->x * In2->x + In1->y * In2->y + In1->z * In2->z ) ;
}

// 行列の掛け算
int MultiplyMatrix( MATRIX *Out, MATRIX *In1, MATRIX *In2 )
{
	MATRIX Op1 = *In1 , Op2 = *In2 ;

	Out->m[0][0] = Op1.m[0][0] * Op2.m[0][0] + Op1.m[0][1] * Op2.m[1][0] + Op1.m[0][2] * Op2.m[2][0] + Op1.m[0][3] * Op2.m[3][0] ;
	Out->m[0][1] = Op1.m[0][0] * Op2.m[0][1] + Op1.m[0][1] * Op2.m[1][1] + Op1.m[0][2] * Op2.m[2][1] + Op1.m[0][3] * Op2.m[3][1] ;
	Out->m[0][2] = Op1.m[0][0] * Op2.m[0][2] + Op1.m[0][1] * Op2.m[1][2] + Op1.m[0][2] * Op2.m[2][2] + Op1.m[0][3] * Op2.m[3][2] ;
	Out->m[0][3] = Op1.m[0][0] * Op2.m[0][3] + Op1.m[0][1] * Op2.m[1][3] + Op1.m[0][2] * Op2.m[2][3] + Op1.m[0][3] * Op2.m[3][3] ;

	Out->m[1][0] = Op1.m[1][0] * Op2.m[0][0] + Op1.m[1][1] * Op2.m[1][0] + Op1.m[1][2] * Op2.m[2][0] + Op1.m[1][3] * Op2.m[3][0] ;
	Out->m[1][1] = Op1.m[1][0] * Op2.m[0][1] + Op1.m[1][1] * Op2.m[1][1] + Op1.m[1][2] * Op2.m[2][1] + Op1.m[1][3] * Op2.m[3][1] ;
	Out->m[1][2] = Op1.m[1][0] * Op2.m[0][2] + Op1.m[1][1] * Op2.m[1][2] + Op1.m[1][2] * Op2.m[2][2] + Op1.m[1][3] * Op2.m[3][2] ;
	Out->m[1][3] = Op1.m[1][0] * Op2.m[0][3] + Op1.m[1][1] * Op2.m[1][3] + Op1.m[1][2] * Op2.m[2][3] + Op1.m[1][3] * Op2.m[3][3] ;

	Out->m[2][0] = Op1.m[2][0] * Op2.m[0][0] + Op1.m[2][1] * Op2.m[1][0] + Op1.m[2][2] * Op2.m[2][0] + Op1.m[2][3] * Op2.m[3][0] ;
	Out->m[2][1] = Op1.m[2][0] * Op2.m[0][1] + Op1.m[2][1] * Op2.m[1][1] + Op1.m[2][2] * Op2.m[2][1] + Op1.m[2][3] * Op2.m[3][1] ;
	Out->m[2][2] = Op1.m[2][0] * Op2.m[0][2] + Op1.m[2][1] * Op2.m[1][2] + Op1.m[2][2] * Op2.m[2][2] + Op1.m[2][3] * Op2.m[3][2] ;
	Out->m[2][3] = Op1.m[2][0] * Op2.m[0][3] + Op1.m[2][1] * Op2.m[1][3] + Op1.m[2][2] * Op2.m[2][3] + Op1.m[2][3] * Op2.m[3][3] ;

	Out->m[3][0] = Op1.m[3][0] * Op2.m[0][0] + Op1.m[3][1] * Op2.m[1][0] + Op1.m[3][2] * Op2.m[2][0] + Op1.m[3][3] * Op2.m[3][0] ;
	Out->m[3][1] = Op1.m[3][0] * Op2.m[0][1] + Op1.m[3][1] * Op2.m[1][1] + Op1.m[3][2] * Op2.m[2][1] + Op1.m[3][3] * Op2.m[3][1] ;
	Out->m[3][2] = Op1.m[3][0] * Op2.m[0][2] + Op1.m[3][1] * Op2.m[1][2] + Op1.m[3][2] * Op2.m[2][2] + Op1.m[3][3] * Op2.m[3][2] ;
	Out->m[3][3] = Op1.m[3][0] * Op2.m[0][3] + Op1.m[3][1] * Op2.m[1][3] + Op1.m[3][2] * Op2.m[2][3] + Op1.m[3][3] * Op2.m[3][3] ;

	// 終了
	return 0 ;
}










// 文字列比較関数

// 二つ前の文字列を調べる関数
int StrCmp2( char *String, int StrLength, char *StrBuffer )
{
	int i , j ;

	i = 0 ;
	while( i < StrLength && *( String - i ) == ' ' ) i ++ ;
	if( i == StrLength || *( String - i ) == '\n' ) return -1 ;

	while( i < StrLength && *( String - i ) != ' ' && *( String - i ) != '\n' ) i ++ ;
	if( i == StrLength || *( String - i ) == '\n' ) return -1 ;

	while( i < StrLength && *( String - i ) == ' ' ) i ++ ;
	if( i == StrLength || *( String - i ) == '\n' ) return -1 ;

	j = 0 ;
	while( j + i < StrLength && *( String - i - j ) != ' ' && *( String - i - j ) != '\n' ) j ++ ;
	_MEMCPY( StrBuffer, String - i - j + 1, ( size_t )j ) ;
	StrBuffer[j] = '\0' ;

	return 0 ;
}

// strtokのパチもん
char *StrTorkn( char *SertchStr, const char *SkipString )
{
	static char *SertchString = NULL ;
	int i , Length ;
	char *RetPoint ;

	if( SertchStr != NULL ) SertchString = SertchStr ;
	if( SertchString == NULL ) return NULL ;

	Length = ( int )_STRLEN( SkipString ) ;

	for( i = -1 ; i != Length ; SertchString ++ )
	{
		for( i = 0 ; i < Length && SkipString[i] != *SertchString && '\0' != *SertchString ; i ++ ){}
		if( '\0' == *SertchString )
		{
			SertchString = NULL ;
			return NULL ;
		}
		if( i == Length ) break ;
	}

	RetPoint = SertchString ;

	for( i = Length ; i == Length ; SertchString ++ )
	{
		for( i = 0 ; i < Length && SkipString[i] != *SertchString && '\0' != *SertchString ; i ++ ){}
		if( '\0' == *SertchString )
		{
			SertchString = NULL ;
			return RetPoint ;
		}
		if( i != Length ) break ;
	}

	*SertchString = '\0' ;
	SertchString ++ ;

	return RetPoint ;
}

// 次に出てくる中括弧'{}'を読み飛ばす
char *KakkoSkip( char *StrPoint )
{
	int KakkoNum ;

	// '{'があるまで読み飛ばす
	while( *StrPoint != '{' )
	{
		if( ( unsigned char )( *StrPoint ) == 0xff ) return NULL ;
		StrPoint ++ ;
	}
	StrPoint ++ ;
	KakkoNum = 1 ;

	while( KakkoNum != 0 )
	{
		switch( ( BYTE )*StrPoint )
		{
		case '{' : KakkoNum ++ ; break ;
		case '}' : KakkoNum -- ; break ;
		case 0xff : KakkoNum = 0 ; break ;
		}

		StrPoint ++ ;
	}

	// 終了
	return StrPoint ;
}

















// 文字列解析系関数

// 文字列解析用データをセットする
extern int SetPStr( X_PSTRING *PStrBuf, char *String, int StrOffset, const char *SkipStr, int StrSize, bool binf, bool f64f )
{
	PStrBuf->binf = binf ;
	PStrBuf->f64f = f64f ;

	if( String )
	{
		PStrBuf->StrBuf = String ;
		PStrBuf->StrOffset = StrOffset ;
		if( StrSize == -1 && binf == false ) PStrBuf->StrSize = ( int )_STRLEN( String ) ;
		else PStrBuf->StrSize = StrSize ;
	}

	if( SkipStr )
	{
		_STRCPY_S( PStrBuf->SkipStr, sizeof( PStrBuf->SkipStr ), SkipStr ) ;
		PStrBuf->SkipStrLen = ( int )_STRLEN( SkipStr ) ;
	}

	// 終了
	return 0 ;
}

// 次に出てくる文字列を取得する
extern int PStrGet( X_PSTRING *PStrBuf, char *StrBuf, size_t BufferBytes )
{
	int i ;
	size_t DestSize ;
	int Len ;

	Len = ( int )_STRLEN( PStrBuf->SkipStr ) ;

	// スキップ文字でない文字を検索する
	while( PStrBuf->StrOffset != PStrBuf->StrSize )
	{
		if( ( PStrBuf->StrBuf[PStrBuf->StrOffset] == '/' && PStrBuf->StrBuf[PStrBuf->StrOffset+1] == '/' ) ||
			PStrBuf->StrBuf[PStrBuf->StrOffset] == '#' )
		{
			PStrBuf->StrOffset ++ ;
			while( PStrBuf->StrOffset != PStrBuf->StrSize )
			{
				if( CHECK_SHIFTJIS_2BYTE( PStrBuf->StrBuf[PStrBuf->StrOffset] ) )
				{
					PStrBuf->StrOffset += 2 ;
				}
				else
				{
					if( PStrBuf->StrBuf[PStrBuf->StrOffset] == '\n' ) break ;
					PStrBuf->StrOffset ++ ;
				}
			}
				
			if( PStrBuf->StrOffset == PStrBuf->StrSize ) return -1 ;
		}

		if( CHECK_SHIFTJIS_2BYTE( PStrBuf->StrBuf[PStrBuf->StrOffset] ) == FALSE )
		{
			for( i = 0 ; i < Len ; i ++ )
			{
				if( PStrBuf->StrBuf[PStrBuf->StrOffset] == PStrBuf->SkipStr[i] ) goto R1 ;
			}
		}
		break ;
R1 :
		if( CHECK_SHIFTJIS_2BYTE( PStrBuf->StrBuf[PStrBuf->StrOffset] ) )
		{
			PStrBuf->StrOffset += 2 ;
		}
		else
		{
			PStrBuf->StrOffset ++ ;
		}
	}
	if( PStrBuf->StrOffset == PStrBuf->StrSize ) return -1 ;

	// スキップ文字列が来るまで文字列を取得する
	DestSize = 0 ;
	while( PStrBuf->StrOffset < PStrBuf->StrSize )
	{
		if( CHECK_SHIFTJIS_2BYTE( PStrBuf->StrBuf[PStrBuf->StrOffset] ) )
		{
			if( BufferBytes - DestSize >= 3 )
			{
				StrBuf[ DestSize     ] = PStrBuf->StrBuf[PStrBuf->StrOffset] ;
				StrBuf[ DestSize + 1 ] = PStrBuf->StrBuf[PStrBuf->StrOffset + 1] ;
				DestSize += 2 ;
			}
			PStrBuf->StrOffset += 2 ;
		}
		else
		{
			for( i = 0 ; i < Len ; i ++ )
			{
				if( PStrBuf->StrBuf[PStrBuf->StrOffset] == PStrBuf->SkipStr[i] ) goto R2 ;
			}

			if( BufferBytes - DestSize >= 2 )
			{
				StrBuf[ DestSize ] = PStrBuf->StrBuf[PStrBuf->StrOffset] ;
				DestSize ++ ;
			}
			PStrBuf->StrOffset ++ ;
		}
	}
R2 :
	StrBuf[ DestSize ] = '\0' ;

	// 終了
	return 0 ;
}

// 次に出てくる文字列を取得する
extern int PStrGet( X_PSTRING *PStrBuf, char *StrBuf, size_t BufferBytes, const char *AddSkipStr )
{
	char SkipStr[64] ;
	int r ;

	_STRCPY_S( SkipStr, sizeof( SkipStr ), PStrBuf->SkipStr ) ;
	_STRCAT_S( PStrBuf->SkipStr, sizeof( PStrBuf->SkipStr ), AddSkipStr ) ;
	r = PStrGet( PStrBuf, StrBuf, BufferBytes ) ;
	_STRCPY_S( PStrBuf->SkipStr, sizeof( PStrBuf->SkipStr ), SkipStr ) ;

	// 終了
	return r ;
}

// 検索オフセットを移動せずに次の文字列を得る
extern int PStrGetNMP( X_PSTRING *PStrBuf, char *StrBuf, size_t BufferBytes, const char *AddSkipStr )
{
	char SkipStr[42] ;
	int r ;
	int Offset ;

	Offset = PStrBuf->StrOffset ;
	_STRCPY_S( SkipStr, sizeof( SkipStr ), PStrBuf->SkipStr ) ;
	_STRCAT_S( PStrBuf->SkipStr, sizeof( PStrBuf->SkipStr ), AddSkipStr ) ;
	r = PStrGet( PStrBuf, StrBuf, BufferBytes ) ;
	_STRCPY_S( PStrBuf->SkipStr, sizeof( PStrBuf->SkipStr ), SkipStr ) ;
	PStrBuf->StrOffset = Offset ;

	// 終了
	return r ;
}

// 次に出てくる『{...}』を読み飛ばす
extern int PStrKakkoSkip( X_PSTRING *PString )
{
	int KakkoNum ;

	// '{'があるまで読み飛ばす
	while( PString->StrOffset != PString->StrSize )
	{
		if( CHECK_SHIFTJIS_2BYTE( PString->StrBuf[PString->StrOffset] ) )
		{
			PString->StrOffset += 2 ;
		}
		else
		{
			if( PString->StrBuf[PString->StrOffset] == '{' ) break ;
			PString->StrOffset ++ ;
		}
	}
	if( PString->StrOffset == PString->StrSize ) return -1 ;
	PString->StrOffset ++ ;
	KakkoNum = 1 ;

	while( PString->StrOffset != PString->StrSize && KakkoNum != 0 )
	{
		if( PString->StrBuf[PString->StrOffset] == '/' && PString->StrBuf[PString->StrOffset+1] == '/' )
		{
			PString->StrOffset += 2 ;
			while( PString->StrOffset != PString->StrSize )
			{
				if( CHECK_SHIFTJIS_2BYTE( PString->StrBuf[PString->StrOffset] ) )
				{
					PString->StrOffset += 2 ;
				}
				else
				{
					if( PString->StrBuf[PString->StrOffset] == '\n' ) break ;
					PString->StrOffset ++ ;
				}
			}
			if( PString->StrOffset == PString->StrSize ) return -1 ;
		}

		switch( PString->StrBuf[PString->StrOffset] )
		{
		case '{' :
			KakkoNum ++ ;
			PString->StrOffset ++ ;
			break ;

		case '}' :
			KakkoNum -- ;
			PString->StrOffset ++ ;
			break ;

		default :
			if( CHECK_SHIFTJIS_2BYTE( PString->StrBuf[PString->StrOffset] ) )
			{
				PString->StrOffset += 2 ;
			}
			else
			{
				PString->StrOffset ++ ;
			}
			break ;
		}
	}
	if( PString->StrOffset == PString->StrSize ) return -1 ;

	// 終了
	return 0 ;
}




// 指定の文字列中のどれかの文字が来るまで読み飛ばす
extern int PStrMove( X_PSTRING *PStrBuf, const char *CmpStr )
{
	int i ;
	int Len ;

	Len = ( int )_STRLEN( CmpStr ) ;

	// スキップ文字でない文字を検索する
	while( PStrBuf->StrOffset != PStrBuf->StrSize )
	{
		if( PStrBuf->StrBuf[PStrBuf->StrOffset] == '/' && PStrBuf->StrBuf[PStrBuf->StrOffset+1] == '/' )
		{
			PStrBuf->StrOffset += 2 ;
			while( PStrBuf->StrOffset != PStrBuf->StrSize )
			{
				if( CHECK_SHIFTJIS_2BYTE( PStrBuf->StrBuf[PStrBuf->StrOffset] ) )
				{
					PStrBuf->StrOffset += 2 ;
				}
				else
				{
					if( PStrBuf->StrBuf[PStrBuf->StrOffset] == '\n' ) break ;
					PStrBuf->StrOffset ++ ;
				}
			}
		}

		if( CHECK_SHIFTJIS_2BYTE( PStrBuf->StrBuf[PStrBuf->StrOffset] ) )
		{
			PStrBuf->StrOffset += 2 ;
		}
		else
		{
			for( i = 0 ; i < Len ; i ++ )
				if( PStrBuf->StrBuf[PStrBuf->StrOffset] == CmpStr[i] ) goto R1 ;
			PStrBuf->StrOffset ++ ;
		}
	}

R1 :
	// 終了
	return PStrBuf->StrOffset == PStrBuf->StrSize ? -1 : 0 ;
}

// 指定の文字列中のどれかの文字の次の文字まで読み飛ばす
extern int PStrMoveP( X_PSTRING *PStrBuf, const char *CmpStr )
{
	int r ;

	r = PStrMove( PStrBuf, CmpStr ) ;
	if( r != -1 ) PStrBuf->StrOffset ++ ;

	// 終了
	return r ;
}

// 数字が出るまで読み飛ばす
extern int PStrMoveNum( X_PSTRING *PStrBuf )
{
	return PStrMove( PStrBuf, "-0123456789." ) ;
}

// 次のトークン位置へ移動する
extern int PStrMoveOneB( X_PSTRING *PStrBuf )
{
	if( PStrBuf->StrSize < PStrBuf->StrOffset ) return -1 ;

	// トークンによって進める値を算出
	switch( PSTRWORD(PStrBuf,0) )
	{
	case TOKEN_NAME :         PSTRMOV(PStrBuf, 2 + 4 + PSTRDWORD(PStrBuf,2) ) ; break ;
	case TOKEN_STRING :       PSTRMOV(PStrBuf, 2 + 4 + PSTRDWORD(PStrBuf,2) + 2 ) ; break ;
	case TOKEN_INTEGER :      PSTRMOV(PStrBuf, 2 + 4 ) ; break ;
	case TOKEN_GUID :         PSTRMOV(PStrBuf, 2 + 4 + 2 + 2 + 8 ) ; break ;
	case TOKEN_INTEGER_LIST : PSTRMOV(PStrBuf, 2 + 4 + 4 * PSTRDWORD(PStrBuf,2) ) ; break ;
	case TOKEN_FLOAT_LIST :   PSTRMOV(PStrBuf, 2 + 4 + (PStrBuf->f64f ? 8 : 4) * PSTRDWORD(PStrBuf,2) ) ; break ;
	default :                 PSTRMOV(PStrBuf, 2) ; break ;
	}

	// 終了
	return 0 ;
}

// バイナリデータの TOKEN_NAME を得る
extern int PStrGetNameB( X_PSTRING *PStrBuf, char *StrBuf, size_t BufferBytes )
{
	if( PSTRWORD(PStrBuf,0) != TOKEN_NAME ) return -1 ;

	_STRNCPY_S( StrBuf, BufferBytes, PSTRP(PStrBuf,6), ( int )( PSTRDWORD(PStrBuf,2) + 1 ) ) ;
	if( PSTRDWORD(PStrBuf,2) * sizeof( char ) < BufferBytes )
	{
		StrBuf[PSTRDWORD(PStrBuf,2)] = '\0' ;
	}

	// 終了
	return 0 ;
}

// バイナリデータの TOKEN_NAME を得た後トークンを進める
extern int PStrGetNameBP( X_PSTRING *PStrBuf, char *StrBuf, size_t BufferBytes )
{
	if( PSTRWORD(PStrBuf,0) != TOKEN_NAME ) return -1 ;

	PStrGetNameB( PStrBuf, StrBuf, BufferBytes ) ;
	PStrMoveOneB( PStrBuf ) ;

	// 終了
	return 0 ;
}

// 指定のトークンが来るまでトークンを読み飛ばす
extern int PStrMoveB( X_PSTRING *PStrBuf, int Code )
{
	// 指定のトークンが来るまで読み飛ばす
	while( PStrBuf->StrSize > PStrBuf->StrOffset && PSTRWORD(PStrBuf,0) != Code )
		PStrMoveOneB(PStrBuf) ;
	if( PStrBuf->StrSize <= PStrBuf->StrOffset ) return -1 ;

	// 終了
	return 0 ;
}

// 指定のコードの次のトークンまで読み飛ばす
extern int PStrMovePB( X_PSTRING *PStrBuf, int Code )
{
	// 指定のトークンが来るまで読み飛ばす
	PStrMoveB( PStrBuf, Code ) ;

	// 指定のトークンからさらに一つ先のトークンまで読む
	PStrMoveOneB( PStrBuf ) ;

	return 0 ;
}

// 次に出てくる『{...}』を読み飛ばす(バイナリバージョン)
extern int PStrKakkoSkipB( X_PSTRING *PString )
{
	int KakkoNum ;

	// '{'があるまで読み飛ばす
	PStrMovePB( PString, TOKEN_OBRACE ) ;
	if( PString->StrOffset >= PString->StrSize ) return -1 ;
	KakkoNum = 1 ;

	while( PString->StrOffset < PString->StrSize && KakkoNum != 0 )
	{
		switch( PSTRWORD(PString,0)  )
		{
		case TOKEN_OBRACE : KakkoNum ++ ; break ;
		case TOKEN_CBRACE : KakkoNum -- ; break ;
		}

		PStrMoveOneB( PString ) ;
	}
	if( PString->StrOffset >= PString->StrSize ) return -1 ;

	// 終了
	return 0 ;
}


















// Ｘファイルの解析で得たデータを解放する
extern int TerminateXModel( X_MODEL *Model )
{
	X_MODEL *M = Model ;
	X_OBJECT *O, *OB ;

	// データ確認
	if( M == NULL ) return -1 ;

	// モデルデータ中のオブジェクトが確保しているメモリをすべて解放
	ClearMemArea( &Model->XModelMem ) ;

	// オブジェクト用に確保していたメモリをすべて解放する
	O = M->FirstObject ;
	while( O != NULL )
	{
		OB = O->NextData ;
		DXFREE( O ) ;
		O = OB ;
	}
	M->FirstObject = NULL ;

	// 終了
	return 0 ;
}

// オブジェクトデータを追加する
static X_OBJECT *AddObject( int TempType, const char *Name, X_OBJECT *Parents, X_MODEL *Model )
{
	X_OBJECT *O ;
	X_MODEL *M = Model ;
	int DataSize, NameLen ;

	if( M == NULL ) return NULL ;

	if( Name == NULL ) Name = "" ;

	// 確保するメモリのサイズをデータのタイプによって変更する
	DataSize = sizeof( X_OBJECT ) ;
	switch( TempType )
	{
	case TEMP_STRING :					DataSize += sizeof( X_STRING ) ; break ;				// 文字列
	case TEMP_VECTOR :					DataSize += sizeof( X_VECTOR ) ; break ;				// ベクトルテンプレート
	case TEMP_COORDS2D :				DataSize += sizeof( X_COORDS2D ) ; break ;				// テクスチャ座標２次元
	case TEMP_MATRIX4X4 :				DataSize += sizeof( X_MATRIX4X4 ) ; break ;				// 4 x 4 行列
	case TEMP_COLORRGBA :				DataSize += sizeof( X_COLORRGBA ) ; break ;				// アルファ付きカラー
	case TEMP_COLORRGB :				DataSize += sizeof( X_COLORRGB ) ; break ;				// カラー
	case TEMP_MATERIAL :				DataSize += sizeof( X_MATERIAL ) ; break ;				// マテリアル
	case TEMP_TEXTUREFILENAME :			DataSize += sizeof( X_TEXTUREFILENAME ) ; break ;		// テクスチャファイルネーム
	case TEMP_MESHFACE :				DataSize += sizeof( X_MESHFACE ) ; break ;				// メッシュフェイス
	case TEMP_MESHTEXTURECOORDS :		DataSize += sizeof( X_MESHTEXTURECOORDS ) ; break ;		// メッシュテクスチャ座標
	case TEMP_MESHNORMALS :				DataSize += sizeof( X_MESHNORMALS ) ; break ;			// メッシュの法線
	case TEMP_MESHMATERIALLIST :		DataSize += sizeof( X_MESHMATERIALLIST ) ; break ;		// メッシュマテリアルリスト
	case TEMP_INDEXEDCOLOR :			DataSize += sizeof( X_INDEXEDCOLOR ) ; break ;			// インデックスカラー
	case TEMP_MESHVERTEXCOLORS :		DataSize += sizeof( X_MESHVERTEXCOLORS ) ; break ;		// メッシュ頂点カラー
	case TEMP_MESH :					DataSize += sizeof( X_MESH ) ; break ;					// メッシュ
	case TEMP_FRAMETRANSFORMMATRIX :	DataSize += sizeof( X_FRAMETRANSFORMMATRIX ) ; break ;	// フレームのローカルトランスフォーム
	case TEMP_FRAME :					DataSize += sizeof( X_FRAME ) ; break ;					// フレーム
	case TEMP_FLOATKEYS :				DataSize += sizeof( X_FLOATKEYS ) ; break ;				// 浮動小数点数の配列及び配列内の float の数
	case TEMP_TIMEDFLOATKEYS :			DataSize += sizeof( X_TIMEDFLOATKEYS ) ; break ;		// アニメーションで使用される浮動小数点数のセット及び正の時間
	case TEMP_ANIMATIONKEY :			DataSize += sizeof( X_ANIMATIONKEY ) ; break ;			// アニメーションキーのセット
	case TEMP_ANIMATIONOPTIONS :		DataSize += sizeof( X_ANIMATIONOPTIONS ) ; break ;		// アニメーションオプション
	case TEMP_ANIMATION :				DataSize += sizeof( X_ANIMATION ) ; break ;				// 前のフレームを参照するアニメーションを格納する
	case TEMP_ANIMATIONSET :			DataSize += sizeof( X_ANIMATIONSET ) ; break ;			// Animation オブジェクトを格納する
	case TEMP_XSKINMESHHEADER :			DataSize += sizeof( X_XSKINMESHHEADER ) ; break ;		// スキニングメッシュの情報
	case TEMP_SKINWEIGHTS :				DataSize += sizeof( X_SKINWEIGHTS ) ; break ;			// スキンメッシュのウエイト
	case TEMP_XFILE :					DataSize += sizeof( X_XFILE ) ; break ;					// Ｘファイル
	case TEMP_DWORD :					DataSize += sizeof( X_DWORD ) ; break ;					// ＤＷＯＲＤ
	case TEMP_WORD :					DataSize += sizeof( X_WORD ) ; break ;					// ＷＯＲＤ
	case TEMP_FLOAT :					DataSize += sizeof( X_FLOAT ) ; break ;					// ＦＬＯＡＴ
	}

	DWORD jbm ;

	// メモリ領域の確保
	NameLen = ( int )_STRLEN( Name ) + 1 ;
	jbm = ( DWORD )( DataSize + NameLen ) ;
	if( ( O = ( X_OBJECT * )DXALLOC( jbm ) ) == NULL )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"Load XFile : オブジェクトのメモリ領域の確保に失敗しました" @*/ ) ;
		return NULL ;
	}

	// 文字列の保存に必要なメモリの総サイズに加算
	Model->StringSize += ( NameLen + 3 ) / 4 * 4 ;

	// データの初期化
	_MEMSET( O, 0, ( size_t )( DataSize + ( int )_STRLEN( Name ) + 1 ) ) ;
	O->Type = TempType ;
	O->ObjectDataSize = DataSize ;
	O->Name = ( char * )( ( BYTE * )O + O->ObjectDataSize ) ;
	_STRCPY( O->Name, Name ) ;
	O->ParentsData = Parents ;
	O->ChildLastData = O->ChildFirstData = NULL ;
	O->ChildNextData = NULL ;
	O->NextData = NULL ;

	// データの関連づけを施す
	if( Parents != NULL )
	{
		if( Parents->ChildFirstData == NULL )
		{
			Parents->ChildFirstData = Parents->ChildLastData = O ;
		}
		else
		{
			Parents->ChildLastData = ( Parents->ChildLastData->ChildNextData = O ) ;
		}
	}

	// オブジェクトデータを追加する
	if( M->FirstObject == NULL )
	{
		M->FirstObject = M->LastObject = O ;
	}
	else
	{
		M->LastObject = ( M->LastObject->NextData = O ) ;
	}

	// アドレスを返す
	return O ;
}

// 指定のオブジェクトに関連のある指定のタイプのオブジェクトを得る
static void *GetRelationObject( int Type, X_OBJECT *Object )
{
	int i ;
	int j ;
	X_OBJECT *SO ;

	// 指定タイプのオブジェクトが子にいるか調べる
	SO = Object->ChildFirstData ;
	while( SO != NULL )
	{
		if( Type == SO->Type ) break ;
		SO = SO->ChildNextData ;
	}

	// もし子オブジェクトに指定タイプのオブジェクトがいなかった場合は関連付けされているオブジェクトから探す
	if( SO == NULL )
	{
		for( i = 0 ; i < Object->RelationObjectNum ; i ++ )
		{
			for( j = 0 ; j < Object->RelationObject[ i ].ObjectNum ; j ++ )
			{
				if( Object->RelationObject[ i ].Object[ j ]->Type == Type )
				{
					SO = Object->RelationObject[ i ].Object[ j ] ;
					break ;
				}
			}
			if( j != Object->RelationObject[ i ].ObjectNum )
			{
				break ;
			}
		}
	}

	// 見つけたオブジェクトのアドレスを返す
	return SO != NULL ? ( void * )DOFF( SO ) : NULL ;
}

// 指定のオブジェクトに関連のある指定のタイプのオブジェクトを列挙する
static int EnumRelationObject( int Type, int *NumBuf, void **AddresBuf, X_OBJECT *Object )
{
	int i ;
	int j ;
	int Num ;
	X_OBJECT *SO ;

	// 指定タイプのオブジェクトが子にいるか調べる
	SO = Object->ChildFirstData ;
	Num = 0 ;
	while( SO != NULL )
	{
		if( Type == SO->Type )
		{
			Num ++ ;
			if( AddresBuf != NULL )
			{
				*AddresBuf = ( void * )DOFF( SO ) ;
				AddresBuf ++ ;
			}
		}

		SO = SO->ChildNextData ;
	}

	// 関連付けされているオブジェクトからも探す
	for( i = 0 ; i < Object->RelationObjectNum ; i ++ )
	{
		for( j = 0 ; j < Object->RelationObject[ i ].ObjectNum ; j ++ )
		{
			if( Object->RelationObject[ i ].Object[ j ]->Type == Type )
			{
				Num ++ ;
				if( AddresBuf != NULL )
				{
					*AddresBuf = ( void * )DOFF( Object->RelationObject[ i ].Object[ j ] ) ;
					AddresBuf ++ ;
				}
			}
		}
	}

	// 数バッファに格納
	if( NumBuf != NULL ) *NumBuf = Num ;

	// 終了
	return 0 ;
}

// 指定の名前をもつオブジェクトを取得する
static void *GetMatchNameObject( int Type, char *Name, X_MODEL *Model )
{
	X_MODEL *M ;
	X_OBJECT *O ;

	M = Model ;
	if( M == NULL ) return NULL ;

	O = M->FirstObject ;
	while( O != NULL )
	{
		if( O->Type == Type && _STRCMP( O->Name, Name ) == 0 ) break ;
		O = O->NextData ;
	}

	// 終了
	return O != NULL ? DOFF( O ) : NULL ;
}

/*
// 指定のオブジェクトと同じ親を持つひとつ前のデータを取得する
static	X_OBJECT	*GetPrevObject( X_MODEL *Model, X_OBJECT *Object )
{
	X_OBJECT *BO, *O ;

	BO = NULL ;
	O = Object->ParentsData == NULL ? Model->FirstObject : Object->ParentsData->ChildFirstData ;
	while( O != Object )
	{
		if( O->ParentsData == Object->ParentsData )
			BO = O ;
		O = O->NextData ;
	}

	return BO ;
}
*/


// Ｘファイルのデータを解析し、読み込む
extern int AnalysXFile( char *StringBuffer, int StrSize, X_MODEL *Model )
{
	X_PSTRING PS ;
	X_MODEL *M ;
	X_STACK Stack ;
	char StrB[256], Name[128] ;
	int Temp, Ret = 0, Len, i ;
	X_OBJECT *O, *Ob ;
	bool binf, f64f ;

	// テキストＸファイルかチェック
	if( _STRNCMP( StringBuffer   , "xof ", 4 ) != 0 ) goto ERR ;
//	if( _STRNCMP( StringBuffer+ 4, "0302", 4 ) != 0 ) goto ERR ;
	if( _STRNCMP( StringBuffer+ 8, "txt ", 4 ) != 0 && _STRNCMP( StringBuffer+ 8, "bin ", 4 ) != 0 )
	{
		DXST_LOGFILE_ADDUTF16LE( "\xfe\x5b\xdc\x5f\x57\x30\x66\x30\x44\x30\x8b\x30\x20\x00\x78\x00\xd5\x30\xa1\x30\xa4\x30\xeb\x30\x62\x5f\x0f\x5f\x6f\x30\xc6\x30\xad\x30\xb9\x30\xc8\x30\x62\x5f\x0f\x5f\x68\x30\x5e\x97\x27\x57\x2e\x7e\xd0\x30\xa4\x30\xca\x30\xea\x30\x62\x5f\x0f\x5f\x6e\x30\x7f\x30\x67\x30\x59\x30\x0a\x00\x00"/*@ L"対応している xファイル形式はテキスト形式と非圧縮バイナリ形式のみです\n" @*/ ) ;
		goto ERR ;
	}
//	if( _STRNCMP( StringBuffer+12, "0064", 4 ) != 0 ) goto ERR ;
	binf = _STRNCMP( StringBuffer+ 8, "txt ", 4 ) == 0 ? false : true ;
	f64f = _STRNCMP( StringBuffer+12, "0064", 4 ) == 0 ? true : false ;
	goto R1 ;

ERR :
//	return DXST_LOGFILE_ADDW( L"テキストのＸファイルではありません\n" ) ;
//	return DXST_LOGFILE_ADDW( L"Ｘファイルではありません\n" ) ;
	return -1 ;

R1 :
	// 文字列解析用のデータをセット
	SetPStr( &PS, StringBuffer + 16, 0, DefSkipStr, StrSize - 16, binf, f64f ) ;  

	// モデルデータの取得
	M = Model ;
	if( M == NULL ) return -1 ;

	// スタック情報を初期化する
	Stack.StackNum = 0 ;
	IncStackObject( &Stack, M->FirstObject ) ;
	O = Stack.Stack[Stack.StackNum-1] ;

	// データの解析開始
	for(;;)
	{
		// 最初の文字列を得る
		if( !binf )
		{
			if( PStrGetNMP( &PS, StrB, sizeof( StrB ) ) == -1 )
			{
				// ファイルの終端に来ていたらループから抜ける
				if( Stack.StackNum != 1 )
				{
					DXST_LOGFILE_ADDUTF16LE( "\x88\x4e\x1f\x67\x57\x30\x6a\x30\x44\x30\x20\x00\x45\x00\x4f\x00\x46\x00\x20\x00\x67\x30\x59\x30\x0a\x00\x00"/*@ L"予期しない EOF です\n" @*/ ) ;
					return -1 ;
				}
				break ;
			}
		}
		else
		{
			if( PS.StrOffset >= PS.StrSize )
			{
				// ファイルの終端に来ていたらループから抜ける
				if( Stack.StackNum != 1 )
				{
					DXST_LOGFILE_ADDUTF16LE( "\x88\x4e\x1f\x67\x57\x30\x6a\x30\x44\x30\x20\x00\x45\x00\x4f\x00\x46\x00\x20\x00\x67\x30\x59\x30\x0a\x00\x00"/*@ L"予期しない EOF です\n" @*/ ) ;
					return -1 ;
				}
				break ;
			}
		}

		// いきなり開き括弧『{』の場合は間接参照とみなす
		if( ( !binf && StrB[0] == '{' ) || 
			( binf && PSTRWORD(&PS,0) == TOKEN_OBRACE ) )
		{
			// オブジェクト名を取得する
			if( binf )
			{
				PStrMovePB( &PS, TOKEN_OBRACE ) ;
				PStrGetNameBP( &PS, Name, sizeof( Name ) ) ;
				PStrMovePB( &PS, TOKEN_CBRACE ) ;
			}
			else
			{
				PStrMoveP( &PS, "{" ) ;
				PStrGetNMP( &PS, Name, sizeof( Name ) ) ;
				if( _STRCHR( Name, '}' ) != NULL )
				{
					PStrGetNMP( &PS, Name, sizeof( Name ), "}" ) ;
				}
				PStrMoveP( &PS, "}" ) ;
			}

			// 指定のオブジェクトの名前を保存する
			if( O->RelationObjectMaxNum == O->RelationObjectNum )
			{
				void *OldRelationObject ;
				int NewMaxNum ;

				OldRelationObject     = O->RelationObject ;
				NewMaxNum             = O->RelationObjectMaxNum + 32 ;
				O->RelationObject     = ( X_RELATION_OBJECT * )ADDMEMAREA( sizeof( X_RELATION_OBJECT ) * NewMaxNum, &M->XModelMem ) ;
				if( O->RelationObject == NULL )
				{
					return DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\x93\x95\xa5\x63\xc2\x53\x67\x71\x28\x75\xdd\x30\xa4\x30\xf3\x30\xbf\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : 間接参照用ポインタを格納するメモリ領域の確保に失敗しました\n" @*/ )) ;
				}
				if( O->RelationObjectMaxNum != 0 )
				{
					_MEMCPY( O->RelationObject, OldRelationObject, sizeof( X_RELATION_OBJECT ) * O->RelationObjectMaxNum ) ;
				}
				O->RelationObjectMaxNum = NewMaxNum ;
/*
				void *OldRelationObject, *OldRelationObjectName ;
				int NewMaxNum ;

				OldRelationObject     = O->RelationObject ;
				OldRelationObjectName = O->RelationObjectName ;
				NewMaxNum             = O->RelationObjectMaxNum + 32 ;
				O->RelationObject     = ( X_OBJECT ** )ADDMEMAREA( ( sizeof( X_OBJECT * ) + sizeof( char * ) ) * NewMaxNum, &M->XModelMem ) ;
				if( O->RelationObject == NULL )
					return DXST_LOGFILEFMT_ADDW(( L"Load XFile : 間接参照用ポインタを格納するメモリ領域の確保に失敗しました\n" )) ;
				O->RelationObjectName = ( char ** )( O->RelationObject + NewMaxNum ) ;
				if( O->RelationObjectMaxNum != 0 )
				{
					_MEMCPY( O->RelationObject,     OldRelationObject,     sizeof( X_OBJECT * ) * O->RelationObjectMaxNum ) ;
					_MEMCPY( O->RelationObjectName, OldRelationObjectName, sizeof( char * )     * O->RelationObjectMaxNum ) ;
				}
				O->RelationObjectMaxNum = NewMaxNum ;
*/
			}

			Len = ( int )_STRLEN( Name ) ;

			O->RelationObject[ O->RelationObjectNum ].ObjectNum    = 0 ;
			O->RelationObject[ O->RelationObjectNum ].ObjectMaxNum = 0 ;
			O->RelationObject[ O->RelationObjectNum ].Object       = NULL ;
			O->RelationObject[ O->RelationObjectNum ].ObjectName   = ( char * )ADDMEMAREA( ( size_t )( Len + 1 ), &M->XModelMem ) ;
			if( O->RelationObject[ O->RelationObjectNum ].ObjectName == NULL )
				return DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\x93\x95\xa5\x63\xc2\x53\x67\x71\x6e\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x0d\x54\x92\x30\xdd\x4f\x58\x5b\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"Load XFile : 間接参照のオブジェクト名を保存するメモリ領域の確保に失敗しました" @*/ )) ;
			_STRCPY( O->RelationObject[ O->RelationObjectNum ].ObjectName, Name ) ;
/*
			O->RelationObjectName[ O->RelationObjectNum ] = ( char * )ADDMEMAREA( Len + 1, &M->XModelMem ) ;
			if( O->RelationObjectName[ O->RelationObjectNum ] == NULL )
				return DXST_LOGFILEFMT_ADDW(( L"Load XFile : 間接参照のオブジェクト名を保存するメモリ領域の確保に失敗しました" )) ;
			_STRCPY( O->RelationObjectName[ O->RelationObjectNum ], Name ) ;
*/
			O->RelationObjectNum ++ ;
		}
		else
		{
			// 閉じ括弧の場合の処理
			if( ( !binf && _STRCHR( StrB, '}' ) != NULL ) ||
				( binf && PSTRWORD(&PS,0) == TOKEN_CBRACE ) )
			{
				// スタックを一つ減らす
				DecStackObject( &Stack ) ;

				// 親オブジェクトのアドレスを変更する
				O = Stack.Stack[Stack.StackNum-1] ;

				// オフセットを進ませる
				if( !binf ) PStrMoveP( &PS, "}" ) ;
				else PStrMovePB( &PS, TOKEN_CBRACE ) ;
			}
			else
			{
				// テンプレート名の取得
				if( !binf )
				{
					if( _STRCHR( StrB, '{' ) != NULL  )
					{
						PStrGetNMP( &PS, StrB, sizeof( StrB ), "{" ) ;

						// しっかりと文字列を取得する
						PStrGet( &PS, StrB, sizeof( StrB ), "{" ) ;
					}
					else
					{
						// しっかりと文字列を取得する
						PStrGet( &PS, StrB, sizeof( StrB ) ) ;
					}
				}
				else
				{
					PStrGetNameBP( &PS, StrB, sizeof( StrB ) ) ;
				}

				// 各種テンプレート文字列と比較する
				for( Temp = 0 ; Temp < TEMP_NUM ; Temp ++ )
				{
					if( TTable[Temp][0] != '\0' && _STRICMP( StrB, TTable[Temp] ) == 0 ) break ;
				}

				// もし対応していないテンプレートだった場合は次の『{...}』は飛ばす
				if( Temp == TEMP_NUM )
				{
					if( !binf ) PStrKakkoSkip( &PS ) ;
					else PStrKakkoSkipB( &PS ) ;
				}
				else
				{
					// 次の文字列の取得
					if( !binf )
					{
						PStrGetNMP( &PS, Name, sizeof( Name ) ) ;
					}
					else
					{
						if( PSTRWORD(&PS,0) == TOKEN_NAME )
						{
							PStrGetNameB( &PS, Name, sizeof( Name ) ) ;
						}
						else
						{
							if( PSTRWORD(&PS,0) == TOKEN_OBRACE ) _STRCPY_S( Name, sizeof( Name ), "{" ) ;
							else Name[0] = '\0' ;
						}
					}

					// もし名前ではなかったらＮＵＬＬ文字をセット
					if( Name[0] == '{' )
					{
						Name[0] = '\0' ;
					}
					else
					{
						if( !binf )
						{
							if( _STRCHR( Name, '{' ) != NULL )
							{
								PStrGetNMP( &PS, Name, sizeof( Name ), "{" ) ;
							}
						}
					}

					// 新しいオブジェクトの作成
					O = AddObject( Temp, Name, Stack.Stack[Stack.StackNum-1], Model ) ;
					if( O == NULL )
					{
						DXST_LOGFILEFMT_ADDA( ( "Load XFile : \x83\x49\x83\x75\x83\x57\x83\x46\x83\x4e\x83\x67 %s ( %s ) \x82\xcc\x92\xc7\x89\xc1\x82\xc9\x8e\xb8\x94\x73\x82\xb5\x82\xdc\x82\xb5\x82\xbd"/*@ "Load XFile : オブジェクト %s ( %s ) の追加に失敗しました" @*/, Name, TTable[ Temp ] ) ) ;
						return -1 ;
					}

					// スタックに積む
					if( IncStackObject( &Stack, O ) == -1 )
					{
						DXST_LOGFILEFMT_ADDA( ( "Load XFile : \x83\x49\x83\x75\x83\x57\x83\x46\x83\x4e\x83\x67 %s ( %s ) \x82\xcc\x83\x6c\x83\x58\x83\x67\x82\xaa\x90\x5b\x82\xb7\x82\xac\x82\xdc\x82\xb7"/*@ "Load XFile : オブジェクト %s ( %s ) のネストが深すぎます" @*/, Name, TTable[ Temp ] ) ) ;
						return -1 ;
					}

					// 中括弧の中に入る
					if( !binf ) PStrMoveP( &PS, "{" ) ;
					else PStrMovePB( &PS, TOKEN_OBRACE ) ;

					// Temp の値によって処理を分岐
					switch( Temp )
					{
					case TEMP_MATERIAL :				Ret = TempMaterial(             &PS, Model, ( X_MATERIAL             * )DOFF( O ) ) ; break ;	// マテリアル
					case TEMP_TEXTUREFILENAME :			Ret = TempTextureFilename(      &PS, Model, ( X_TEXTUREFILENAME      * )DOFF( O ) ) ; break ;	// テクスチャファイルネーム
					case TEMP_MESHFACE :				Ret = TempMeshFace(             &PS, Model, ( X_MESHFACE             * )DOFF( O ) ) ; break ;	// メッシュフェイス
					case TEMP_MESHTEXTURECOORDS :		Ret = TempMeshTextureCoords(    &PS, Model, ( X_MESHTEXTURECOORDS    * )DOFF( O ) ) ; break ;	// メッシュテクスチャ座標
					case TEMP_MESHNORMALS :				Ret = TempMeshNormals(          &PS, Model, ( X_MESHNORMALS          * )DOFF( O ) ) ; break ;	// メッシュの法線
					case TEMP_MESHMATERIALLIST :		Ret = TempMeshMaterialList(     &PS, Model, ( X_MESHMATERIALLIST     * )DOFF( O ) ) ; break ;	// メッシュマテリアルリスト
					case TEMP_INDEXEDCOLOR :			Ret = TempIndexedColor(			&PS, Model, ( X_INDEXEDCOLOR         * )DOFF( O ) ) ; break ;	// インデックスカラー
					case TEMP_MESHVERTEXCOLORS :		Ret = TempMeshVertexColors(		&PS, Model, ( X_MESHVERTEXCOLORS     * )DOFF( O ) ) ; break ;	// メッシュ頂点カラー
					case TEMP_MESH :					Ret = TempMesh(                 &PS, Model, ( X_MESH                 * )DOFF( O ) ) ; break ;	// メッシュ
					case TEMP_FRAMETRANSFORMMATRIX :	Ret = TempFrameTransformMatrix( &PS, Model, ( X_FRAMETRANSFORMMATRIX * )DOFF( O ) ) ; break ;	// フレームのローカルトランスフォーム
					case TEMP_FRAME :					Ret = TempFrame(                &PS, Model, ( X_FRAME                * )DOFF( O ) ) ; break ;	// フレーム
					case TEMP_FLOATKEYS :				Ret = TempFloatKeys(            &PS, Model, ( X_FLOATKEYS            * )DOFF( O ) ) ; break ;	// 浮動小数点数の配列及び配列内の float の数
					case TEMP_TIMEDFLOATKEYS :			Ret = TempTimedFloatKeys(       &PS, Model, ( X_TIMEDFLOATKEYS       * )DOFF( O ) ) ; break ;	// アニメーションで使用される浮動小数点数のセット及び正の時間
					case TEMP_ANIMATIONKEY :			Ret = TempAnimationKey(         &PS, Model, ( X_ANIMATIONKEY         * )DOFF( O ) ) ; break ;	// アニメーションキーのセット
					case TEMP_ANIMATIONOPTIONS :		Ret = TempAnimationOptions(     &PS, Model, ( X_ANIMATIONOPTIONS     * )DOFF( O ) ) ; break ;	// アニメーションオプション
					case TEMP_ANIMATION :				Ret = TempAnimation(            &PS, Model, ( X_ANIMATION            * )DOFF( O ) ) ; break ;	// 前のフレームを参照するアニメーションを格納する
					case TEMP_ANIMATIONSET :			Ret = TempAnimationSet(         &PS, Model, ( X_ANIMATIONSET         * )DOFF( O ) ) ; break ;	// Animation オブジェクトを格納する
					case TEMP_XSKINMESHHEADER :			Ret = TempXSkinMeshHeader(      &PS, Model, ( X_XSKINMESHHEADER      * )DOFF( O ) ) ; break ;	// スキニングメッシュの情報
					case TEMP_SKINWEIGHTS :				Ret = TempSkinWeights(          &PS, Model, ( X_SKINWEIGHTS          * )DOFF( O ) ) ; break ;	// スキンメッシュのウエイト
					}
					if( Ret == -1 )
					{
						DXST_LOGFILEFMT_ADDA( ( "Load XFile : \x83\x49\x83\x75\x83\x57\x83\x46\x83\x4e\x83\x67 %s ( %s ) \x89\xf0\x90\xcd\x92\x86\x82\xc9\x83\x47\x83\x89\x81\x5b\x82\xaa\x94\xad\x90\xb6\x82\xb5\x82\xdc\x82\xb5\x82\xbd"/*@ "Load XFile : オブジェクト %s ( %s ) 解析中にエラーが発生しました" @*/, Name, TTable[ Temp ] ) ) ;
						return -1 ;
					}
				}
			}
		}
	}

	// 間接参照のオブジェクト同士の関連付けを行う
	for( O = M->FirstObject ; O ; O = O->NextData )
	{
		// 間接参照の数だけ繰り返し
		for( i = 0 ; i < O->RelationObjectNum ; i ++ )
		{
//			// 既に参照先の情報がある場合は何もしない
//			if( O->RelationObject[ i ] ) continue ;

			// 名前が無い場合は何もしない
			if( O->RelationObject[ i ].ObjectName == NULL || O->RelationObject[ i ].ObjectName[ 0 ] == '\0' )
			{
				continue ;
			}

			// 指定のオブジェクトと同じ名前のオブジェクトを探す
			for( Ob = M->FirstObject ; Ob ; Ob = Ob->NextData )
			{
				void *OldRelationObject ;
				int NewMaxNum ;

				if( _STRCMP( Ob->Name, O->RelationObject[ i ].ObjectName ) != 0 )
				{
					continue ;
				}

				// 見つけたオブジェクトのアドレスを保存
				if( O->RelationObject[ i ].ObjectMaxNum == O->RelationObject[ i ].ObjectNum )
				{
					OldRelationObject = O->RelationObject[ i ].Object ;
					NewMaxNum = O->RelationObject[ i ].ObjectMaxNum + 32 ;
					O->RelationObject[ i ].Object = ( X_OBJECT ** )ADDMEMAREA( sizeof( X_OBJECT * ) * NewMaxNum, &M->XModelMem ) ;
					if( O->RelationObject[ i ].Object == NULL )
						return DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\x93\x95\xa5\x63\xc2\x53\x67\x71\x28\x75\xdd\x30\xa4\x30\xf3\x30\xbf\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : 間接参照用ポインタを格納するメモリ領域の確保に失敗しました\n" @*/ )) ;
					if( O->RelationObject[ i ].ObjectMaxNum != 0 )
					{
						_MEMCPY( O->RelationObject[ i ].Object, OldRelationObject, sizeof( X_OBJECT * ) * O->RelationObject[ i ].ObjectMaxNum ) ;
					}
					O->RelationObject[ i ].ObjectMaxNum = NewMaxNum ;
				}
				O->RelationObject[ i ].Object[ O->RelationObject[ i ].ObjectNum ] = Ob ;
				O->RelationObject[ i ].ObjectNum ++ ;

				// 相手先のオブジェクトと関連付けをする
				if( Ob->RelationObjectMaxNum == Ob->RelationObjectNum )
				{
					OldRelationObject = Ob->RelationObject ;
					NewMaxNum = Ob->RelationObjectMaxNum + 32 ;
					Ob->RelationObject = ( X_RELATION_OBJECT * )ADDMEMAREA( sizeof( X_RELATION_OBJECT ) * NewMaxNum, &M->XModelMem ) ;
					if( Ob->RelationObject == NULL )
						return DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\x93\x95\xa5\x63\xc2\x53\x67\x71\x28\x75\xdd\x30\xa4\x30\xf3\x30\xbf\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : 間接参照用ポインタを格納するメモリ領域の確保に失敗しました\n" @*/ )) ;
					if( Ob->RelationObjectMaxNum != 0 )
					{
						_MEMCPY( Ob->RelationObject, OldRelationObject, sizeof( X_RELATION_OBJECT ) * Ob->RelationObjectMaxNum ) ;
					}
					Ob->RelationObjectMaxNum = NewMaxNum ;
/*
					void *OldRelationObject, *OldRelationObjectName ;
					int NewMaxNum ;

					OldRelationObject = Ob->RelationObject ;
					OldRelationObjectName = Ob->RelationObjectName ;
					NewMaxNum = Ob->RelationObjectMaxNum + 32 ;
					Ob->RelationObject = ( X_OBJECT ** )ADDMEMAREA( ( sizeof( X_OBJECT * ) + sizeof( char * ) ) * NewMaxNum, &M->XModelMem ) ;
					if( Ob->RelationObject == NULL )
						return DXST_LOGFILEFMT_ADDW(( L"Load XFile : 間接参照用ポインタを格納するメモリ領域の確保に失敗しました\n" )) ;
					Ob->RelationObjectName = ( char ** )( Ob->RelationObject + NewMaxNum ) ;
					if( Ob->RelationObjectMaxNum != 0 )
					{
						_MEMCPY( Ob->RelationObject,     OldRelationObject,     sizeof( X_OBJECT * ) * Ob->RelationObjectMaxNum ) ;
						_MEMCPY( Ob->RelationObjectName, OldRelationObjectName, sizeof( char * )     * Ob->RelationObjectMaxNum ) ;
					}
					Ob->RelationObjectMaxNum = NewMaxNum ;
*/
				}

				NewMaxNum = 1 ;
				Ob->RelationObject[ Ob->RelationObjectNum ].Object = ( X_OBJECT ** )ADDMEMAREA( sizeof( X_OBJECT * ) * NewMaxNum, &M->XModelMem ) ;
				if( Ob->RelationObject[ Ob->RelationObjectNum ].Object == NULL )
					return DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\x93\x95\xa5\x63\xc2\x53\x67\x71\x28\x75\xdd\x30\xa4\x30\xf3\x30\xbf\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : 間接参照用ポインタを格納するメモリ領域の確保に失敗しました\n" @*/ )) ;
				Ob->RelationObject[ Ob->RelationObjectNum ].ObjectMaxNum = NewMaxNum ;
				Ob->RelationObject[ Ob->RelationObjectNum ].ObjectName = ( char * )"" ;
				Ob->RelationObject[ Ob->RelationObjectNum ].Object[ 0 ] = O ;
				Ob->RelationObject[ Ob->RelationObjectNum ].ObjectNum ++ ;
//				Ob->RelationObject[ Ob->RelationObjectNum ] = O ;
				Ob->RelationObjectNum ++ ;
			}
/*
			// 指定のオブジェクトと同じ名前のオブジェクトを探す
			for( Ob = M->FirstObject ; Ob && _STRCMP( Ob->Name, O->RelationObjectName[ i ] ) != 0 ; Ob = Ob->NextData ){}

			// 指定のオブジェクトがなかったら何もしない
			if( Ob == NULL ) 
			{
				DXST_LOGFILE_ADDW( L"Load XFile : 間接参照が失敗しました\n" ) ;
			}	
			else
			{
				// 見つけたオブジェクトのアドレスを保存
				O->RelationObject[ i ] = Ob ;

				// 相手先のオブジェクトと関連付けをする
				if( Ob->RelationObjectMaxNum == Ob->RelationObjectNum )
				{
					void *OldRelationObject, *OldRelationObjectName ;
					int NewMaxNum ;

					OldRelationObject = Ob->RelationObject ;
					OldRelationObjectName = Ob->RelationObjectName ;
					NewMaxNum = Ob->RelationObjectMaxNum + 32 ;
					Ob->RelationObject = ( X_OBJECT ** )ADDMEMAREA( ( sizeof( X_OBJECT * ) + sizeof( char * ) ) * NewMaxNum, &M->XModelMem ) ;
					if( Ob->RelationObject == NULL )
						return DXST_LOGFILEFMT_ADDW(( L"Load XFile : 間接参照用ポインタを格納するメモリ領域の確保に失敗しました\n" )) ;
					Ob->RelationObjectName = ( char ** )( Ob->RelationObject + NewMaxNum ) ;
					if( Ob->RelationObjectMaxNum != 0 )
					{
						_MEMCPY( Ob->RelationObject,     OldRelationObject,     sizeof( X_OBJECT * ) * Ob->RelationObjectMaxNum ) ;
						_MEMCPY( Ob->RelationObjectName, OldRelationObjectName, sizeof( char * )     * Ob->RelationObjectMaxNum ) ;
					}
					Ob->RelationObjectMaxNum = NewMaxNum ;
				}
				Ob->RelationObject[ Ob->RelationObjectNum ] = O ;
				Ob->RelationObjectNum ++ ;
			}
*/
		}
	}

	// 終了
	return 0 ;
}

// Ｘファイルのモデルデータを解析し、実行用データの準備を行う
extern int AnalysXData( X_MODEL *Model, MV1_MODEL_R *RModel )
{
	X_OBJECT *O/*, *BO*/ ;
	X_MODEL *M ;
	int i, j, k, l ;

	M = Model ;
	if( M == NULL ) return -1 ;

	// すべてのオブジェクトを走査する
	O = M->FirstObject ;
	while( O != NULL )
	{
		// オブジェクトのタイプによって処理を分岐
		switch( O->Type )
		{
		case TEMP_MATERIAL :			// マテリアル
			{
				X_MATERIAL *MT = ( X_MATERIAL * )DOFF( O ) ;

				// このマテリアルに関連付けられているテクスチャーファイルネームを取得する
				MT->TextureFileName = ( X_TEXTUREFILENAME * )GetRelationObject( TEMP_TEXTUREFILENAME, O ) ;

				// テクスチャが無かった場合で、且つどこかのオブジェクトに所属する場合は直前のオブジェクトのマテリアルのテクスチャファイルを使用する
/*				if( MT->TextureFileName == NULL && O->ParentsData->ParentsData != NULL )
				{
					BO = GetPrevObject( M, O ) ;
					if( BO && BO->Type == TEMP_MATERIAL && ( ( X_MATERIAL * )DOFF( BO ) )->TextureFileName )
					{
						MT->TextureFileName = ( ( X_MATERIAL * )DOFF( BO ) )->TextureFileName ;
					}
				}
*/
				// マテリアルのインデックスをセット
				MT->Index = M->MaterialNum ;

				// マテリアルの数をインクリメント
				M->MaterialNum ++ ;
			}
			break ;

		case TEMP_MESHMATERIALLIST :	// メッシュマテリアルリスト
			{
				X_MESHMATERIALLIST *ML = ( X_MESHMATERIALLIST * )DOFF( O ) ;
				DWORD Num ;

				// 関連付けられているマテリアルのアドレスを格納するためのメモリ領域を確保
				if( ( ML->MaterialList = ( X_MATERIAL ** )ADDMEMAREA( sizeof( X_MATERIAL *) * ML->nMaterials, &M->XModelMem ) ) == NULL )
				{
					DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\xe1\x30\xc3\x30\xb7\x30\xe5\x30\x6b\x30\xa2\x95\x23\x90\xd8\x4e\x51\x30\x89\x30\x8c\x30\x66\x30\x44\x30\x8b\x30\xde\x30\xc6\x30\xea\x30\xa2\x30\xeb\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : メッシュに関連付けられているマテリアルを格納するメモリ領域の確保に失敗しました\n" @*/ )) ;
					return -1 ;
				}

				// 関連付けられているマテリアルを取得する
				EnumRelationObject( TEMP_MATERIAL, ( int * )&Num, ( void ** )ML->MaterialList, O ) ;

				// 数が違ったらエラー
				if( Num != ML->nMaterials )
				{
					DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\xde\x30\xc6\x30\xea\x30\xa2\x30\xeb\x30\xea\x30\xb9\x30\xc8\x30\x6e\x30\xde\x30\xc6\x30\xea\x30\xa2\x30\xeb\x30\x70\x65\x28\x00\x20\x00\x25\x00\x64\x00\x20\x00\x29\x00\x68\x30\x9f\x5b\x9b\x96\x6e\x30\xde\x30\xc6\x30\xea\x30\xa2\x30\xeb\x30\x6e\x30\x70\x65\x28\x00\x20\x00\x25\x00\x64\x00\x20\x00\x29\x00\x4c\x30\x55\x90\x44\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : マテリアルリストのマテリアル数( %d )と実際のマテリアルの数( %d )が違いました\n" @*/, ML->nMaterials, Num ) ) ;
					return -1 ;
				}
			}
			break ;

		case TEMP_MESH :				// メッシュ
			{
				X_MESH *MS = ( X_MESH * )DOFF( O ) ;

				// メッシュに関連付けられている頂点カラーを得る
				MS->VertexColor = ( X_MESHVERTEXCOLORS * )GetRelationObject( TEMP_MESHVERTEXCOLORS, O ) ;

				// メッシュに関連付けられているテクスチャー座標を得る
				MS->TextureCoord = ( X_MESHTEXTURECOORDS * )GetRelationObject( TEMP_MESHTEXTURECOORDS, O ) ;

				// テクスチャ座標と頂点座標の数が違う場合はエラー
				if( MS->TextureCoord && MS->TextureCoord->nTextureCoords != MS->nVertices )
				{
					DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\xa7\x5e\x19\x6a\x6e\x30\x70\x65\x28\x00\x20\x00\x25\x00\x64\x00\x20\x00\x29\x00\x68\x30\x02\x98\xb9\x70\xa7\x5e\x19\x6a\x6e\x30\x70\x65\x28\x00\x20\x00\x25\x00\x64\x00\x20\x00\x29\x00\x4c\x30\x55\x90\x44\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : テクスチャ座標の数( %d )と頂点座標の数( %d )が違いました\n" @*/, MS->TextureCoord->nTextureCoords, MS->nVertices ) ) ;
					return -1 ;
				}

				// メッシュに関連付けられている法線データを得る
				MS->Normals = ( X_MESHNORMALS * )GetRelationObject( TEMP_MESHNORMALS, O ) ;

				// メッシュに関連付けられているマテリアルリストを得る
				MS->MaterialList = ( X_MESHMATERIALLIST * )GetRelationObject( TEMP_MESHMATERIALLIST, O ) ;

				// 面の数が違ったらエラー
				if( MS->Normals && MS->nFaces != MS->Normals->nFaceNormals )
				{
					DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\xe1\x30\xc3\x30\xb7\x30\xe5\x30\xea\x30\xb9\x30\xc8\x30\x6e\x30\x62\x97\x6e\x30\x70\x65\x28\x00\x20\x00\x25\x00\x64\x00\x20\x00\x29\x00\x68\x30\xd5\x6c\xda\x7d\x6e\x30\x62\x97\x6e\x30\x70\x65\x28\x00\x20\x00\x25\x00\x64\x00\x20\x00\x29\x00\x4c\x30\x55\x90\x44\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : メッシュリストの面の数( %d )と法線の面の数( %d )が違いました\n" @*/, MS->nFaces, MS->Normals->nFaceNormals ) ) ;
					return -1 ;
				}

				// メッシュに関連付けられているスキンメッシュのウエイト値の数を得る
				EnumRelationObject( TEMP_SKINWEIGHTS, &MS->SkinWeightsNum, NULL, O ) ;

				// ウエイト値データのリストを格納するメモリ領域を確保
				if( MS->SkinWeightsNum )
				{
					if( ( MS->SkinWeights = ( X_SKINWEIGHTS ** )ADDMEMAREA( sizeof( X_SKINWEIGHTS * ) * MS->SkinWeightsNum, &M->XModelMem ) ) == NULL )
					{
						DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\xb9\x30\xad\x30\xf3\x30\xa6\x30\xa8\x30\xa4\x30\xc8\x30\xc7\x30\xfc\x30\xbf\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : スキンウエイトデータを格納するメモリの確保に失敗しました\n" @*/ )) ;
						return -1 ;
					}

					// 再度メッシュに関連付けられているスキンメッシュのウエイト値のデータのアドレスを得る
					EnumRelationObject( TEMP_SKINWEIGHTS, &MS->SkinWeightsNum, ( void ** )MS->SkinWeights, O ) ;
				}
				else
				{
					MS->SkinWeights = NULL ;
				}

				// トライアングルの数を加算
				M->TriangleNum += MS->nTriangles ;

				// スキンメッシュのウエイトに関する情報の数を加算
				M->SkinWeightNum += MS->SkinWeightsNum ;

				// メッシュの数をインクリメント
				M->MeshNum ++ ;
			}
			break ;

		case TEMP_FRAME :				// フレーム
			{
				X_FRAME *Frame = ( X_FRAME * )DOFF( O ) ;
				X_MESH *MS ;
				X_OBJECT *Ob ;

				// このフレームに関連付けられているトランスフォーム行列の取得
				Frame->TransformMatrix = ( X_FRAMETRANSFORMMATRIX * )GetRelationObject( TEMP_FRAMETRANSFORMMATRIX, O ) ;

				// このフレームに関連付けられているメッシュにメッシュの座標を登録する
				Ob = O->ChildFirstData ;
				Frame->MeshNum = 0 ;
				while( Ob != NULL )
				{
					if( Ob->Type == TEMP_MESH )
					{
						MS = ( X_MESH * )DOFF( Ob ) ;
						MS->Frame = Frame ;
						Frame->MeshNum ++ ;
					}
					Ob = Ob->ChildNextData ;
				}

				// 関連付けられているメッシュのリストを作成する
				if( ( Frame->Mesh = ( X_MESH ** )ADDMEMAREA( sizeof( X_MESH * ) * Frame->MeshNum, &M->XModelMem ) ) == NULL )
				{
					DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\xe1\x30\xc3\x30\xb7\x30\xe5\x30\xa2\x30\xc9\x30\xec\x30\xb9\x30\x4d\x91\x17\x52\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : メッシュアドレス配列を格納するメモリの確保に失敗しました\n" @*/ )) ;
					return -1 ;
				}
				EnumRelationObject( TEMP_MESH, ( int * )&Frame->MeshNum, ( void ** )Frame->Mesh, O ) ;

				// フレームのインデックスをセット
				Frame->Index = M->FrameNum ;

				// フレームの数をインクリメント
				M->FrameNum ++ ;
			}
			break ;

		case TEMP_ANIMATION :			// アニメーション
			{
				DWORD Max ;
				X_ANIMATION *AN = ( X_ANIMATION * )DOFF( O ) ;

				// アニメーションキーの数を数える
				EnumRelationObject( TEMP_ANIMATIONKEY, ( int * )&AN->AnimationKeysNum, NULL, O ) ;
				
				// アニメーションキーのアドレスを保存するメモリ領域の確保
				if( ( AN->AnimationKeys = ( X_ANIMATIONKEY ** )ADDMEMAREA( sizeof( X_ANIMATIONKEY * ) * AN->AnimationKeysNum, &M->XModelMem ) ) == NULL )
				{
					DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\xa2\x30\xcb\x30\xe1\x30\xfc\x30\xb7\x30\xe7\x30\xf3\x30\xad\x30\xfc\x30\xa2\x30\xc9\x30\xec\x30\xb9\x30\x4d\x91\x17\x52\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : アニメーションキーアドレス配列を格納するメモリの確保に失敗しました\n" @*/ )) ;
					return -1 ;
				}

				// アニメーションキーのアドレスを取得する
				EnumRelationObject( TEMP_ANIMATIONKEY, ( int * )&AN->AnimationKeysNum, ( void ** )AN->AnimationKeys, O ) ;

				// 最大カウントを得る
				Max = 0 ;
				for( i = 0 ; ( DWORD )i < AN->AnimationKeysNum ; i ++ )
				{
					AN->AnimationKeys[ i ]->TotalCount = ( int )( AN->AnimationKeys[ i ]->keys[ AN->AnimationKeys[ i ]->nKeys - 1 ].time ) ;
					if( Max < ( DWORD )AN->AnimationKeys[ i ]->TotalCount ) Max = ( DWORD )AN->AnimationKeys[ i ]->TotalCount ;
				}
				AN->MaxCount = ( int )Max ;

				// アニメーションデータに関連しているフレームのデータアドレスを得る
				AN->Frame = ( X_FRAME * )GetRelationObject( TEMP_FRAME, O ) ;
			}
			break ;

		case TEMP_SKINWEIGHTS :			// スキンメッシュのウエイト
			{
				X_SKINWEIGHTS *SW = ( X_SKINWEIGHTS * )DOFF( O ) ;

				// 指定のフレーム名に合致するオブジェクトを取得する
				SW->Frame = ( X_FRAME * )GetMatchNameObject( TEMP_FRAME, SW->transformNodeName.String, Model ) ;
			}
			break ;
		}

		O = O->NextData ;
	}

	// もう一度すべてのオブジェクトを走査する
	O = M->FirstObject ;
	while( O != NULL )
	{
		// オブジェクトのタイプによって処理を分岐
		switch( O->Type )
		{
		case TEMP_ANIMATIONSET :		// アニメーションセット
			{
				int Max, Num ;

				X_ANIMATIONSET *AS = ( X_ANIMATIONSET * )DOFF( O ) ;
				X_ANIMATION *AN ;
				X_ANIMATIONKEY *AK ;

				// アニメーションの数を数える
				EnumRelationObject( TEMP_ANIMATION, &AS->AnimationNum, NULL, O ) ;
				
				// アニメーションのアドレスを保存するメモリ領域の確保
				if( ( AS->Animation = ( X_ANIMATION ** )ADDMEMAREA( sizeof( X_ANIMATION * ) * AS->AnimationNum, &M->XModelMem ) ) == NULL )
				{
					DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\xa2\x30\xcb\x30\xe1\x30\xfc\x30\xb7\x30\xe7\x30\xf3\x30\xa2\x30\xc9\x30\xec\x30\xb9\x30\x4d\x91\x17\x52\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : アニメーションアドレス配列を格納するメモリの確保に失敗しました\n" @*/ )) ;
					return -1 ;
				}

				// アニメーションのアドレスを取得する
				EnumRelationObject( TEMP_ANIMATION, &AS->AnimationNum, ( void ** )AS->Animation, O ) ;

				// アニメーションの最大アニメーションカウント値を得る
				// ついでに有効なアニメーションの数を数える
				Max = 0 ;
				Num = 0 ;
				for( i = 0 ; i < AS->AnimationNum ; i ++ )
				{
					if( AS->Animation[i]->Frame == NULL ) continue ;
					Num ++ ;
					if( Max < AS->Animation[i]->MaxCount ) Max = AS->Animation[i]->MaxCount ;
				}
				AS->MaxCount = Max ;

				// 各データの数をカウントする
				for( i = 0 ; i < AS->AnimationNum ; i ++ )
				{
					AN = AS->Animation[ i ] ;

					if( AS->Animation[i]->Frame == NULL ) continue ;

					// 関連するフレームの名前を保存するためのメモリサイズを加算
					Model->StringSize += ( int )_STRLEN( OOFF( AN->Frame )->Name ) + 1 ;
					Model->StringSize = ( Model->StringSize + 3 ) / 4 * 4 ;

					// アニメーションキーセットの数を加算する
					Model->AnimKeySetNum += AN->AnimationKeysNum ;

					// 各キーの要素の数を加算する
					for( j = 0 ; ( DWORD )j < AN->AnimationKeysNum ; j ++ )
					{
						DWORD DataSize = 0 ;
						AK = AN->AnimationKeys[ j ] ;

						// アニメーションキーの数を加算する
						switch( AK->keyType )
						{
						case 0 : DataSize = AK->nKeys * ( sizeof( FLOAT4                  ) + sizeof( float ) ) ; break ;
						case 1 : DataSize = AK->nKeys * ( sizeof( VECTOR                  ) + sizeof( float ) ) ; break ;
						case 2 : DataSize = AK->nKeys * ( sizeof( VECTOR                  ) + sizeof( float ) ) ; break ;
						case 4 : DataSize = AK->nKeys * ( sizeof( MV1_ANIM_KEY_MATRIX4X4C ) + sizeof( float ) ) ; break ;
						}
						Model->AnimKeyDataSize += DataSize ;
						AS->KeyDataSize += DataSize ;
					}
				}

				// アニメーションの数を加算
				Model->AnimNum += Num ;

				// アニメーションセットの数をインクリメント
				Model->AnimSetNum ++ ;
			}
			break ;
		}

		O = O->NextData ;
	}

	// 読み込みモデルの構築
	{
		MV1_MATERIAL_R *Material ;
		MV1_TEXTURE_R *Texture ;
		MV1_ANIMSET_R *AnimSet ;
		MV1_ANIM_R *Anim ;
		MV1_ANIMKEYSET_R *AnimKeySet ;
		MV1_FRAME_R *Frame ;
		MV1_MESH_R *Mesh ;
		MV1_SKIN_WEIGHT_R *SkinWeight ;
		MV1_SKIN_WEIGHT_ONE_R *SkinWeightOne ;

		// マテリアルとテクスチャの追加
		for( O = M->FirstObject ; O ; O = O->NextData )
		{
			// マテリアルではない場合はスキップ
			if( O->Type != TEMP_MATERIAL ) continue ;

			X_MATERIAL *MT = ( X_MATERIAL * )DOFF( O ) ;

			// マテリアルの追加
			Material = MV1RAddMaterial( RModel, O->Name ) ;
			if( Material == NULL )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\xde\x30\xc6\x30\xea\x30\xa2\x30\xeb\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\xfd\x8f\xa0\x52\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : マテリアルオブジェクトの追加に失敗しました\n" @*/ )) ;
				return -1 ;
			}
			
			// テクスチャを使用している場合はテクスチャも追加
			if( MT->TextureFileName )
			{
				Texture = MV1RAddTexture( RModel, MT->TextureFileName->filename.String, MT->TextureFileName->filename.String ) ;
				if( Texture == NULL )
				{
					DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\xfd\x8f\xa0\x52\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : テクスチャオブジェクトの追加に失敗しました\n" @*/ )) ;
					return -1 ;
				}

				// パラメータのセット
				Material->DiffuseTexNum = 1 ;
				Material->DiffuseTexs[ 0 ] = Texture ;
			}

			// パラメータのセット
			Material->Ambient.r = 0.0f ;
			Material->Ambient.g = 0.0f ;
			Material->Ambient.b = 0.0f ;
			Material->Ambient.a = 0.0f ;
			Material->Diffuse.r = MT->faceColor.red ;
			Material->Diffuse.g = MT->faceColor.green ;
			Material->Diffuse.b = MT->faceColor.blue ;
			Material->Diffuse.a = 1.0f ;
			Material->Specular.r = MT->specularColor.red ;
			Material->Specular.g = MT->specularColor.green ;
			Material->Specular.b = MT->specularColor.blue ;
			Material->Specular.a = 0.0f ;
			Material->Emissive.r = MT->emissiveColor.red ;
			Material->Emissive.g = MT->emissiveColor.green ;
			Material->Emissive.b = MT->emissiveColor.blue ;
			Material->Emissive.a = 0.0f ;
			Material->Power = MT->power ;

			// faceColor の alpha が 1.0f 意外だった場合は描画モードのパラメータを変更する
			if( MT->faceColor.alpha < 0.9999999f )
			{
				Material->DrawBlendParam = _FTOL( MT->faceColor.alpha * 255.0f ) ;
				if( Material->DrawBlendParam < 0 )
				{
					Material->DrawBlendParam = 0 ;
				}
				if( Material->DrawBlendParam > 255 )
				{
					Material->DrawBlendParam = 255 ;
				}
			}

			MT->MV1RMaterial = Material ;
		}

		// フレームの追加
		for( O = M->FirstObject ; O ; O = O->NextData )
		{
			// 親が居ない場合はスキップ
			if( O->ParentsData == NULL ) continue ;

			// フレームオブジェクトではない場合はスキップ
			if( O->Type != TEMP_FRAME ) continue ;

			X_FRAME *FR = ( X_FRAME * )DOFF( O ) ;

			// フレームの追加
			Frame = MV1RAddFrame( RModel, O->Name, O->ParentsData && O->ParentsData->ParentsData && O->ParentsData->Type == TEMP_FRAME ? ( ( X_FRAME * )DOFF( O->ParentsData ) )->MV1Frame : NULL ) ;
			if( Frame == NULL )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\xd5\x30\xec\x30\xfc\x30\xe0\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\xfd\x8f\xa0\x52\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : フレームオブジェクトの追加に失敗しました\n" @*/ )) ;
				return -1 ;
			}

			// パラメータのセット
			FR->MV1Frame = Frame ; 

			// 行列のセット
			if( FR->TransformMatrix != NULL )
			{
				Frame->Matrix = FR->TransformMatrix->frameMatrix.ms ;
			}
			else
			{
				CreateIdentityMatrix( &Frame->Matrix ) ;
			}

			// メッシュがある場合はメッシュの追加
			for( i = 0 ; i < FR->MeshNum ; i ++ )
			{
				X_MESH *MS ;

				MS = FR->Mesh[ i ] ;

				// メッシュに名前があるか、親の居ないメッシュだった場合はメッシュ名でフレームを作成する
				if( OOFF( MS )->Name[ 0 ] != '\0' || OOFF( MS )->ParentsData == NULL || OOFF( MS )->ParentsData->ParentsData == NULL )
				{
					MS->MeshFrame = MV1RAddFrame( RModel, OOFF( MS )->Name, Frame ) ;
				}
			}
		}

		// メッシュとアニメーションの追加
		O = M->FirstObject ;
		while( O != NULL )
		{
			// オブジェクトのタイプによって処理を分岐
			switch( O->Type )
			{
			case TEMP_FRAME :		// フレーム
				X_FRAME *FR ;
				X_MESH *MS ;
				X_MESHFACE *MF ;
				X_SKINWEIGHTS *SW ;
				X_INDEXEDCOLOR *IC ;

				FR = ( X_FRAME * )DOFF( O ) ;

				// フレームのアドレスセット
				Frame = FR->MV1Frame ;

				// メッシュがある場合はメッシュの追加
				for( i = 0 ; i < FR->MeshNum ; i ++ )
				{
					MS = FR->Mesh[ i ] ;

					// フレームのセット
					Frame = MS->MeshFrame == NULL ? FR->MV1Frame : MS->MeshFrame ;
					if( Frame == NULL && MS->MeshFrame == NULL )
					{
						MS->MeshFrame = MV1RAddFrame( RModel, OOFF( MS )->Name[ 0 ] == '\0' ? "Mesh" : OOFF( MS )->Name, NULL ) ;
						Frame = MS->MeshFrame ;
					}

					// メッシュの追加
					Mesh = MV1RAddMesh( RModel, Frame ) ;
					if( Mesh == NULL )
					{
						DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\xe1\x30\xc3\x30\xb7\x30\xe5\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\xfd\x8f\xa0\x52\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : メッシュオブジェクトの追加に失敗しました\n" @*/ )) ;
						return -1 ;
					}

					// マテリアル情報のコピー
					if( MS->MaterialList == NULL )
					{
//						DXST_LOGFILEFMT_ADDW(( L"Load XFile : メッシュにマテリアルの情報がありません\n" )) ;
//						return -1 ;
						// マテリアルが無かったら適当なマテリアルを追加する
						Mesh->MaterialNum = 1 ;
						Mesh->Materials[ 0 ] = MV1RAddMaterial( RModel, "Normal" ) ;
					}
					else
					{
						// 使用しているマテリアルの情報をコピー
						Mesh->MaterialNum = MS->MaterialList->nMaterials ;
						for( j = 0 ; j < ( int )Mesh->MaterialNum ; j ++ )
							Mesh->Materials[ j ] = MS->MaterialList->MaterialList[ j ]->MV1RMaterial ;
					}

					// スキニング情報のコピー
					Mesh->SkinWeightsNum = ( DWORD )MS->SkinWeightsNum ;
					for( j = 0 ; j < ( int )Mesh->SkinWeightsNum ; j ++ )
					{
						SW = MS->SkinWeights[ j ] ;
						
						// スキニングメッシュの追加
						SkinWeight = MV1RAddSkinWeight( RModel ) ;
						if( SkinWeight == NULL )
						{
							DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\xb9\x30\xad\x30\xcb\x30\xf3\x30\xb0\x30\xe1\x30\xc3\x30\xb7\x30\xe5\x30\xa6\x30\xa8\x30\xa4\x30\xc8\x30\xc5\x60\x31\x58\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : スキニングメッシュウエイト情報を格納するメモリ領域の確保に失敗しました\n" @*/ )) ;
							return -1 ;
						}
						Mesh->SkinWeights[ j ] = SkinWeight ;

						// パラメータのセット
						SkinWeight->ModelLocalMatrix = SW->matrixOffset.ms ;
						SkinWeight->TargetFrame = ( DWORD )SW->Frame->MV1Frame->Index ;

						// 頂点との対応表を格納するためのメモリの確保
						SkinWeight->DataNum = SW->nWeights ;
						SkinWeight->Data = ( MV1_SKIN_WEIGHT_ONE_R * )ADDMEMAREA( sizeof( MV1_SKIN_WEIGHT_ONE_R ) * SkinWeight->DataNum, &RModel->Mem ) ;
						if( SkinWeight->Data == NULL )
						{
							DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\xb9\x30\xad\x30\xcb\x30\xf3\x30\xb0\x30\xe1\x30\xc3\x30\xb7\x30\xe5\x30\xa6\x30\xa8\x30\xa4\x30\xc8\x30\x24\x50\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : スキニングメッシュウエイト値を格納するメモリ領域の確保に失敗しました\n" @*/ )) ;
							return -1 ;
						}

						// 頂点との対応表をコピーする
						SkinWeightOne = SkinWeight->Data ;
						for( k = 0 ; k < ( int )SW->nWeights ; k ++, SkinWeightOne ++ )
						{
							SkinWeightOne->TargetVertex = SW->vertexIndices[ k ] ;
							SkinWeightOne->Weight = SW->weights[ k ] ;
						}
					}

					// 頂点情報をコピー
					Mesh->PositionNum = MS->nVertices ;
					Mesh->Positions = ( VECTOR * )ADDMEMAREA( sizeof( VECTOR ) * Mesh->PositionNum, &RModel->Mem ) ;
					if( Mesh->Positions == NULL )
					{
						DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\x02\x98\xb9\x70\xa7\x5e\x19\x6a\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : 頂点座標を格納するメモリの確保に失敗しました\n" @*/ )) ;
						return -1 ;
					}
					_MEMCPY( Mesh->Positions, MS->vertices, sizeof( VECTOR ) * Mesh->PositionNum ) ;

					// 法線情報のコピー
					if( MS->Normals )
					{
						Mesh->NormalNum = MS->Normals->nNormals ;
						Mesh->Normals = ( VECTOR * )ADDMEMAREA( sizeof( VECTOR ) * Mesh->NormalNum, &RModel->Mem ) ;
						if( Mesh->Normals == NULL )
						{
							DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\xd5\x6c\xda\x7d\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : 法線を格納するメモリの確保に失敗しました\n" @*/ )) ;
							return -1 ;
						}
						_MEMCPY( Mesh->Normals, MS->Normals->normals, sizeof( VECTOR ) * Mesh->NormalNum ) ;
					}

					// UV座標のコピー
					if( MS->TextureCoord )
					{
						Mesh->UVNum[ 0 ] = MS->TextureCoord->nTextureCoords ;
						Mesh->UVs[ 0 ] = ( FLOAT4 * )ADDMEMAREA( sizeof( FLOAT4 ) * Mesh->UVNum[ 0 ], &RModel->Mem ) ;
						if( Mesh->UVs[ 0 ] == NULL )
						{
							DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\x35\xff\x36\xff\xa7\x5e\x19\x6a\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : ＵＶ座標を格納するメモリの確保に失敗しました\n" @*/ )) ;
							return -1 ;
						}
						for( j = 0 ; j < ( int )Mesh->UVNum[ 0 ] ; j ++ )
						{
							Mesh->UVs[ 0 ][ j ].x = MS->TextureCoord->textureCoords[ j ].u ;
							Mesh->UVs[ 0 ][ j ].y = MS->TextureCoord->textureCoords[ j ].v ;
						}
					}

					// 面情報の取得
					{
						// 面情報を格納するメモリ領域の確保
						if( MV1RSetupMeshFaceBuffer( RModel, Mesh, ( int )MS->nFaces, 4 ) < 0 )
						{
							DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\x62\x97\xc5\x60\x31\x58\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : 面情報を格納するメモリ領域の確保に失敗しました\n" @*/ )) ;
							return -1 ;
						}

						// 頂点インデックスとマテリアルインデックスとＵＶインデックスをコピー
						MF = MS->faces ;
						for( j = 0 ; j < ( int )Mesh->FaceNum ; j ++, MF ++ )
						{
							Mesh->Faces[ j ].IndexNum = MF->nFaceVertexIndices ;
							if( Mesh->FaceUnitMaxIndexNum < Mesh->Faces[ j ].IndexNum )
							{
								if( MV1RSetupMeshFaceBuffer( RModel, Mesh, ( int )Mesh->FaceNum, ( int )Mesh->Faces[ j ].IndexNum ) < 0 )
								{
									DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\x62\x97\xc5\x60\x31\x58\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\x8d\x51\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : 面情報を格納するメモリ領域の再確保に失敗しました\n" @*/ )) ;
									return -1 ;
								}
							}

							Mesh->Faces[ j ].MaterialIndex = MS->MaterialList == NULL || MS->nFaces != MS->MaterialList->nFaceIndexes ? 0 : MS->MaterialList->FaceIndexes[ j ] ;
							for( k = 0 ; k < ( int )Mesh->Faces[ j ].IndexNum ; k ++ )
							{
								if( MF->faceVertexIndices != NULL )
								{
									Mesh->Faces[ j ].VertexIndex[ k ]  = MF->faceVertexIndices[ k ] ;
									Mesh->Faces[ j ].UVIndex[ 0 ][ k ] = MF->faceVertexIndices[ k ] ;
								}
								else
								{
									Mesh->Faces[ j ].VertexIndex[ k ]  = MF->faceVertexIndicesFixedLength[ k ] ;
									Mesh->Faces[ j ].UVIndex[ 0 ][ k ] = MF->faceVertexIndicesFixedLength[ k ] ;
								}

								if( Mesh->Normals )
								{
									if( MS->Normals->faceNormals[ j ].faceVertexIndices != NULL )
									{
										Mesh->Faces[ j ].NormalIndex[ k ] = MS->Normals->faceNormals[ j ].faceVertexIndices[ k ] ;
									}
									else
									{
										Mesh->Faces[ j ].NormalIndex[ k ] = MS->Normals->faceNormals[ j ].faceVertexIndicesFixedLength[ k ] ;
									}
								}
							}
						}
					}

					// 頂点カラーのコピー
					if( MS->VertexColor )
					{
						int TotalIndexNum ;
						int *FaceUseVertexIndexBuffer ;
						int *FaceUseVertexIndexList ;
						int *pFUVIB ;
						int *pFUVIL ;
						int FaceUseVertexIndexBufferSize ;

						// 頂点カラーの数をセット
						Mesh->VertexColorNum = MS->VertexColor->nVertexColors ;

						// 頂点カラーを格納するメモリ領域の確保
						Mesh->VertexColors   = ( COLOR_F * )ADDMEMAREA( sizeof( COLOR_F ) * ( Mesh->VertexColorNum + 1 ), &RModel->Mem ) ;
						if( Mesh->VertexColors == NULL )
						{
							DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\x02\x98\xb9\x70\xab\x30\xe9\x30\xfc\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : 頂点カラーを格納するメモリの確保に失敗しました\n" @*/ )) ;
							return -1 ;
						}

						// デフォルトの頂点カラー用
						Mesh->VertexColors[ Mesh->VertexColorNum ].r = 1.0f ;
						Mesh->VertexColors[ Mesh->VertexColorNum ].g = 1.0f ;
						Mesh->VertexColors[ Mesh->VertexColorNum ].b = 1.0f ;
						Mesh->VertexColors[ Mesh->VertexColorNum ].a = 1.0f ;

						// 面のカラー情報はデフォルトでは真っ白にする
						// ついでに全インデックスの数も取得する
						TotalIndexNum = 0 ;
						for( j = 0 ; j < ( int )Mesh->FaceNum ; j ++ )
						{
							TotalIndexNum += ( int )Mesh->Faces[ j ].IndexNum ;
							for( k = 0 ; k < ( int )Mesh->Faces[ j ].IndexNum ; k ++ )
							{
								Mesh->Faces[ j ].VertexColorIndex[ k ] = Mesh->VertexColorNum ;
							}
						}

						// メッシュが使用している頂点のマップを作成する
						{
							FaceUseVertexIndexBufferSize = ( int )( sizeof( int ) * ( Mesh->PositionNum * 2 + TotalIndexNum * 2 ) ) ;
							FaceUseVertexIndexBuffer     = ( int * )DXALLOC( ( size_t )FaceUseVertexIndexBufferSize ) ;
							if( FaceUseVertexIndexBuffer == NULL )
							{
								DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\xe1\x30\xc3\x30\xb7\x30\xe5\x30\x4c\x30\x7f\x4f\x28\x75\x57\x30\x66\x30\x44\x30\x8b\x30\x02\x98\xb9\x70\x6e\x30\xde\x30\xc3\x30\xd7\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : メッシュが使用している頂点のマップを格納するメモリ領域の確保に失敗しました\n" @*/ )) ;
								return -1 ;
							}
							_MEMSET( FaceUseVertexIndexBuffer, 0, ( size_t )FaceUseVertexIndexBufferSize ) ;
							FaceUseVertexIndexList = FaceUseVertexIndexBuffer + Mesh->PositionNum * 2 ;

							// まずは各頂点が使用されている数を取得
							for( j = 0 ; j < ( int )Mesh->FaceNum ; j ++ )
							{
								for( l = 0 ; l < ( int )Mesh->Faces[ j ].IndexNum ; l ++ )
								{
									pFUVIB = &FaceUseVertexIndexBuffer[ Mesh->Faces[ j ].VertexIndex[ l ] * 2 ] ;
									pFUVIB[ 0 ] ++ ;
								}
							}

							// 各頂点が使用している面の情報が格納されているバッファ位置をセット
							TotalIndexNum = 0 ;
							pFUVIB = FaceUseVertexIndexBuffer ;
							for( j = 0 ; j < ( int )Mesh->PositionNum ; j ++, pFUVIB += 2 )
							{
								pFUVIB[ 1 ] = TotalIndexNum ;
								TotalIndexNum += pFUVIB[ 0 ] ;

								// 次にカウンタとして使用するので面の数はリセットする
								pFUVIB[ 0 ] = 0 ;
							}

							// 各頂点が使用している面の情報をセット
							for( j = 0 ; j < ( int )Mesh->FaceNum ; j ++ )
							{
								for( l = 0 ; l < ( int )Mesh->Faces[ j ].IndexNum ; l ++ )
								{
									pFUVIB = &FaceUseVertexIndexBuffer[ Mesh->Faces[ j ].VertexIndex[ l ] * 2 ] ;
									pFUVIL = &FaceUseVertexIndexList[ pFUVIB[ 1 ] * 2 ] ;

									pFUVIL[ pFUVIB[ 0 ] * 2 + 0 ] = j ;
									pFUVIL[ pFUVIB[ 0 ] * 2 + 1 ] = l ;
									pFUVIB[ 0 ] ++ ;
								}
							}
						}

						// 頂点カラーデータのセット
						IC = MS->VertexColor->vertexColors ;
						for( j = 0 ; j < ( int )MS->VertexColor->nVertexColors ; j ++, IC ++ )
						{
							Mesh->VertexColors[ j ].r = IC->indexColor.red   / 255.0f ;
							Mesh->VertexColors[ j ].g = IC->indexColor.green / 255.0f ;
							Mesh->VertexColors[ j ].b = IC->indexColor.blue  / 255.0f ;
							Mesh->VertexColors[ j ].a = IC->indexColor.alpha / 255.0f ;

							// 指定番号の頂点座標を使用している面の頂点カラーを j 番目の頂点カラーにする
							pFUVIB = &FaceUseVertexIndexBuffer[ IC->index * 2 ] ;
							pFUVIL = &FaceUseVertexIndexList[ pFUVIB[ 1 ] * 2 ] ;
							for( k = 0 ; k < pFUVIB[ 0 ] ; k ++ )
							{
								Mesh->Faces[ pFUVIL[ 0 ] ].VertexColorIndex[ pFUVIL[ 1 ] ] = ( DWORD )j ;
								pFUVIL += 2 ;
							}
						}

						// メッシュが使用している頂点のマップ用に確保していたメモリを解放
						DXFREE( FaceUseVertexIndexBuffer ) ;
						FaceUseVertexIndexBuffer = NULL ;
					}
				}
				break ;

			case TEMP_ANIMATIONSET :	// アニメーションセット
				X_ANIMATIONSET *AS ;
				AS = ( X_ANIMATIONSET * )DOFF( O ) ;

				// アニメーションセットの追加
				AnimSet = MV1RAddAnimSet( RModel, O->Name ) ;
				if( AnimSet == NULL )
				{
					DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\xa2\x30\xcb\x30\xe1\x30\xfc\x30\xb7\x30\xe7\x30\xf3\x30\xbb\x30\xc3\x30\xc8\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\xfd\x8f\xa0\x52\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : アニメーションセットオブジェクトの追加に失敗しました\n" @*/ )) ;
					return -1 ;
				}

				// パラメータのセット
				AnimSet->StartTime = 0.0f ;
				AnimSet->EndTime = ( float )AS->MaxCount ;

				// アニメーション情報のセット
				for( i = 0 ; i < AS->AnimationNum ; i ++ )
				{
					X_ANIMATION *AN = AS->Animation[ i ] ;

					// 参照に失敗していたら何もしない
					if( AN->Frame == NULL ) continue ;

					// アニメーションの追加
					Anim = MV1RAddAnim( RModel, AnimSet ) ;
					if( Anim == NULL )
					{
						DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\xa2\x30\xcb\x30\xe1\x30\xfc\x30\xb7\x30\xe7\x30\xf3\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\xfd\x8f\xa0\x52\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : アニメーションオブジェクトの追加に失敗しました\n" @*/ )) ;
						return -1 ;
					}

					// パラメータセット
					Anim->TargetFrameIndex = AN->Frame->MV1Frame->Index ;
					Anim->MaxTime = ( float )AN->MaxCount ;

					// アニメーションキーの数だけ繰り返し
					for( j = 0 ; j < ( int )AN->AnimationKeysNum ; j ++ )
					{
						X_ANIMATIONKEY *AK = AN->AnimationKeys[ j ] ;
						X_TIMEDFLOATKEYS *TimeKeys ;
						int AllocSize = 0 ;

						// アニメーションキーの追加
						AnimKeySet = MV1RAddAnimKeySet( RModel, Anim ) ;
						if( AnimKeySet == NULL )
						{
							DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\xa2\x30\xcb\x30\xe1\x30\xfc\x30\xb7\x30\xe7\x30\xf3\x30\xad\x30\xfc\x30\xbb\x30\xc3\x30\xc8\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\xfd\x8f\xa0\x52\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : アニメーションキーセットオブジェクトの追加に失敗しました\n" @*/ )) ;
							return -1 ;
						}

						// パラメータのセット
						AnimKeySet->TotalTime = ( float )AK->TotalCount ;
						AnimKeySet->Num       = ( int )AK->nKeys ;

						// キータイプのセット
						switch( AK->keyType )
						{
						case 0 :
							AnimKeySet->DataType = MV1_ANIMKEY_DATATYPE_ROTATE ;
							AnimKeySet->Type = MV1_ANIMKEY_TYPE_QUATERNION_X ;
							break ;

						case 1 :
							AnimKeySet->DataType = MV1_ANIMKEY_DATATYPE_SCALE ;
							AnimKeySet->Type = MV1_ANIMKEY_TYPE_VECTOR ;
							break ;

						case 2 :
							AnimKeySet->DataType = MV1_ANIMKEY_DATATYPE_TRANSLATE ;
							AnimKeySet->Type = MV1_ANIMKEY_TYPE_VECTOR ;
							break ;

						case 3 :
						case 4 :
							AnimKeySet->DataType = MV1_ANIMKEY_DATATYPE_MATRIX4X4C ;
							AnimKeySet->Type = MV1_ANIMKEY_TYPE_MATRIX4X4C ;
							break ;
						}

						// キーを格納するメモリ領域の確保
						switch( AnimKeySet->Type )
						{
						case MV1_ANIMKEY_TYPE_QUATERNION_X : AllocSize = sizeof( FLOAT4                  ) ;	break ;
						case MV1_ANIMKEY_TYPE_VECTOR :       AllocSize = sizeof( VECTOR                  ) ;	break ;
						case MV1_ANIMKEY_TYPE_MATRIX4X4C :   AllocSize = sizeof( MV1_ANIM_KEY_MATRIX4X4C ) ;	break ;
						}
						AnimKeySet->KeyFloat4 = ( FLOAT4 * )ADDMEMAREA( ( AllocSize + sizeof( float ) ) * AnimKeySet->Num, &RModel->Mem ) ;
						if( AnimKeySet->KeyFloat4 == NULL )
						{
							DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\xa2\x30\xcb\x30\xe1\x30\xfc\x30\xb7\x30\xe7\x30\xf3\x30\xad\x30\xfc\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : アニメーションキーを格納するメモリ領域の確保に失敗しました\n" @*/ )) ;
							return -1 ;
						}
						AnimKeySet->KeyTime = ( float * )( ( BYTE * )AnimKeySet->KeyFloat4 + AllocSize * AnimKeySet->Num ) ;

						// キーデータのサイズを加算する
						RModel->AnimKeyDataSize += ( AllocSize + sizeof( float ) ) * AnimKeySet->Num ;

						// キーのコピー
						TimeKeys = AK->keys ;
						switch( AnimKeySet->Type )
						{
						case MV1_ANIMKEY_TYPE_QUATERNION_X :		// 回転タイプの場合
							for( k = 0 ; k < AnimKeySet->Num ; k ++, TimeKeys ++ )
							{
								AnimKeySet->KeyTime[ k ]       = ( float )TimeKeys->time ;
								AnimKeySet->KeyFloat4[ k ].x = TimeKeys->tfkeys.values[ 1 ] ;
								AnimKeySet->KeyFloat4[ k ].y = TimeKeys->tfkeys.values[ 2 ] ;
								AnimKeySet->KeyFloat4[ k ].z = TimeKeys->tfkeys.values[ 3 ] ;
								AnimKeySet->KeyFloat4[ k ].w = TimeKeys->tfkeys.values[ 0 ] ;
							}
							break ;

						case MV1_ANIMKEY_TYPE_VECTOR :		// ベクタータイプの場合
							for( k = 0 ; k < AnimKeySet->Num ; k ++, TimeKeys ++ )
							{
								AnimKeySet->KeyTime[ k ]    = ( float )TimeKeys->time ;
								AnimKeySet->KeyVector[ k ].x = TimeKeys->tfkeys.values[ 0 ] ;
								AnimKeySet->KeyVector[ k ].y = TimeKeys->tfkeys.values[ 1 ] ;
								AnimKeySet->KeyVector[ k ].z = TimeKeys->tfkeys.values[ 2 ] ;
							}
							break ;

						case MV1_ANIMKEY_TYPE_MATRIX4X4C :		// 行列タイプの場合
//							AnimSet->IsMatrixLinearBlend = 1 ;
							for( k = 0 ; k < AnimKeySet->Num ; k ++, TimeKeys ++ )
							{
								AnimKeySet->KeyTime[ k ]                        = ( float )TimeKeys->time ;
								AnimKeySet->KeyMatrix4x4C[ k ].Matrix[ 0 ][ 0 ] = TimeKeys->tfkeys.values[ 0 ] ;
								AnimKeySet->KeyMatrix4x4C[ k ].Matrix[ 0 ][ 1 ] = TimeKeys->tfkeys.values[ 1 ] ;
								AnimKeySet->KeyMatrix4x4C[ k ].Matrix[ 0 ][ 2 ] = TimeKeys->tfkeys.values[ 2 ] ;

								AnimKeySet->KeyMatrix4x4C[ k ].Matrix[ 1 ][ 0 ] = TimeKeys->tfkeys.values[ 4 ] ;
								AnimKeySet->KeyMatrix4x4C[ k ].Matrix[ 1 ][ 1 ] = TimeKeys->tfkeys.values[ 5 ] ;
								AnimKeySet->KeyMatrix4x4C[ k ].Matrix[ 1 ][ 2 ] = TimeKeys->tfkeys.values[ 6 ] ;

								AnimKeySet->KeyMatrix4x4C[ k ].Matrix[ 2 ][ 0 ] = TimeKeys->tfkeys.values[ 8 ] ;
								AnimKeySet->KeyMatrix4x4C[ k ].Matrix[ 2 ][ 1 ] = TimeKeys->tfkeys.values[ 9 ] ;
								AnimKeySet->KeyMatrix4x4C[ k ].Matrix[ 2 ][ 2 ] = TimeKeys->tfkeys.values[ 10 ] ;

								AnimKeySet->KeyMatrix4x4C[ k ].Matrix[ 3 ][ 0 ] = TimeKeys->tfkeys.values[ 12 ] ;
								AnimKeySet->KeyMatrix4x4C[ k ].Matrix[ 3 ][ 1 ] = TimeKeys->tfkeys.values[ 13 ] ;
								AnimKeySet->KeyMatrix4x4C[ k ].Matrix[ 3 ][ 2 ] = TimeKeys->tfkeys.values[ 14 ] ;
							}
							break ;
						}
					}
				}
				break ;
			}

			O = O->NextData ;
		}
	}

	// 終了
	return 0 ;
}




// オブジェクトをスタックに加える
static int IncStackObject( X_STACK *Stack, X_OBJECT *Object )
{
	// 既に限界だった場合はエラー
	if( Stack->StackNum == MAX_STACKNUM )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x8e\x96\x64\x5c\xcb\x69\x20\x90\x6e\x30\x50\x96\x4c\x75\x70\x65\x20\x00\x25\x00\x64\x00\x20\x00\x92\x30\x85\x8d\x48\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"階層構造の限界数 %d を超えました" @*/, MAX_STACKNUM )) ;
		return -1 ;
	}

	// スタックにオブジェクトを加える
	Stack->Stack[Stack->StackNum] = Object ;
	Stack->StackNum ++ ;

	// 終了
	return 0 ;
}

// オブジェクトのスタックを一つ減らす
static int DecStackObject( X_STACK *Stack )
{
	// スタックの数をへらす
	Stack->StackNum -- ;

	// 終了
	return 0 ;
}






















// テンプレート解析用関数

// 文字列テンプレートの解析
static int TempString( X_PSTRING *PStr, X_MODEL *Model, X_STRING *String )
{
	X_STRING *S = String ;
	char StrB[256] ;

	if( !PStr->binf )
	{
		// ダブルコーテーションがあるところまでスキップ
		PStrMoveP( PStr, "\"" ) ;

		// 次のダブルコーテーションがあるところまでを取得
		SetPStr( PStr, NULL, 0, "\"" ) ;
		PStrGet( PStr, StrB, sizeof( StrB ) ) ;
		SetPStr( PStr, NULL, 0, DefSkipStr ) ;
		PStr->StrOffset ++ ;
	}
	else
	{
		PStrMoveB( PStr, TOKEN_STRING ) ;
		if( PStr->StrOffset >= PStr->StrSize ) return -1 ;
		_STRNCPY_S( StrB, sizeof( StrB ), PSTRP(PStr,6), ( int )( PSTRDWORD(PStr,2) + 1 ) ) ;
		StrB[PSTRDWORD(PStr,2)] = '\0' ;

		PStrMoveOneB(PStr) ;
	}

	// 文字列を格納するメモリ領域を確保
	if( ( S->String = ( char * )ADDMEMAREA( _STRLEN( StrB ) + 1, &Model->XModelMem ) ) == NULL )
	{
		DXST_LOGFILEFMT_ADDA( ( "Load XFile : \x95\xb6\x8e\x9a\x97\xf1 %s \x82\xf0\x8a\x69\x94\x5b\x82\xb7\x82\xe9\x83\x81\x83\x82\x83\x8a\x97\xcc\x88\xe6\x82\xcc\x8a\x6d\x95\xdb\x82\xc9\x8e\xb8\x94\x73\x82\xb5\x82\xdc\x82\xb5\x82\xbd\n"/*@ "Load XFile : 文字列 %s を格納するメモリ領域の確保に失敗しました\n" @*/, StrB ) ) ;
		return -1 ;
	}

	// 文字列のコピー
	_STRCPY( S->String, StrB ) ;

	// 終了
	return 0 ;
}

// ワードテンプレートの解析
static int TempWord( X_PSTRING *PStr, X_MODEL * /*Model*/, X_WORD *Word )
{
	char StrB[80] ;

	// 次の文字列を読む
	PStrGet( PStr, StrB, sizeof( StrB ) ) ;

	// 数値に換える
	Word->w = ( WORD )_ATOI( StrB ) ;

	// 終了
	return 0 ;
}

// ダブルワードテンプレートの解析
static int TempDword( X_PSTRING *PStr, X_MODEL * /*Model*/, X_DWORD *Dword )
{
	char StrB[80] ;

	if( !PStr->binf )
	{
		// 次の文字列を読む
		PStrGet( PStr, StrB, sizeof( StrB ) ) ;

		// 数値に換える
		Dword->d = ( DWORD )_ATOI( StrB ) ;
	}
	else
	{
		switch( PSTRWORD(PStr,0) )
		{
		case TOKEN_INTEGER :
			Dword->d = PSTRDWORD(PStr,2) ;
			break ;

		case TOKEN_INTEGER_LIST :
			if( PSTRDWORD(PStr,2) != 1 ) return -1 ;
			Dword->d = PSTRDWORD(PStr,6) ;
			break ;
		}
		PStrMoveOneB( PStr ) ;
	}

	// 終了
	return 0 ;
}

// フロートテンプレートの解析
static int TempFloat( X_PSTRING *PStr, X_MODEL * /*Model*/, X_FLOAT *Float )
{
	char StrB[80] ;

	// 次の文字列を読む
	PStrGet( PStr, StrB, sizeof( StrB ) ) ;

	// 数値に換える
	Float->f = ( float )_ATOF( StrB ) ;

	// 終了
	return 0 ;
}

// ベクトルテンプレートの解析
static int TempVector( X_PSTRING *PStr, X_MODEL *Model, X_VECTOR *Vector )
{
	// フロート型を３つ読み込む
	TempFloat( PStr, Model, ( X_FLOAT * )&Vector->dat.x ) ;	if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;
	TempFloat( PStr, Model, ( X_FLOAT * )&Vector->dat.y ) ;	if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;
	TempFloat( PStr, Model, ( X_FLOAT * )&Vector->dat.z ) ;	if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;

	// 終了
	return 0 ;
}

// テクスチャ座標２次元テンプレートの解析
static int TempCoords2d( X_PSTRING *PStr, X_MODEL *Model, X_COORDS2D *Coords2d )
{
	// フロート型を２つ読み込む
	TempFloat( PStr, Model, ( X_FLOAT * )&Coords2d->u ) ;	if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;
	TempFloat( PStr, Model, ( X_FLOAT * )&Coords2d->v ) ;	if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;

	// 終了
	return 0 ;
}

// ４ｘ４行列テンプレートの解析
static int TempMatrix4x4( X_PSTRING *PStr, X_MODEL *Model, X_MATRIX4X4 *Matrix )
{
	// フロート型を１６こ読み込む
	TempFloat( PStr, Model, ( X_FLOAT * )&Matrix->matrix[0][0] ) ;	if( !PStr->binf ) PStrMoveP( PStr, "," ) ;
	TempFloat( PStr, Model, ( X_FLOAT * )&Matrix->matrix[0][1] ) ;	if( !PStr->binf ) PStrMoveP( PStr, "," ) ;
	TempFloat( PStr, Model, ( X_FLOAT * )&Matrix->matrix[0][2] ) ;	if( !PStr->binf ) PStrMoveP( PStr, "," ) ;
	TempFloat( PStr, Model, ( X_FLOAT * )&Matrix->matrix[0][3] ) ;	if( !PStr->binf ) PStrMoveP( PStr, "," ) ;

	TempFloat( PStr, Model, ( X_FLOAT * )&Matrix->matrix[1][0] ) ;	if( !PStr->binf ) PStrMoveP( PStr, "," ) ;
	TempFloat( PStr, Model, ( X_FLOAT * )&Matrix->matrix[1][1] ) ;	if( !PStr->binf ) PStrMoveP( PStr, "," ) ;
	TempFloat( PStr, Model, ( X_FLOAT * )&Matrix->matrix[1][2] ) ;	if( !PStr->binf ) PStrMoveP( PStr, "," ) ;
	TempFloat( PStr, Model, ( X_FLOAT * )&Matrix->matrix[1][3] ) ;	if( !PStr->binf ) PStrMoveP( PStr, "," ) ;

	TempFloat( PStr, Model, ( X_FLOAT * )&Matrix->matrix[2][0] ) ;	if( !PStr->binf ) PStrMoveP( PStr, "," ) ;
	TempFloat( PStr, Model, ( X_FLOAT * )&Matrix->matrix[2][1] ) ;	if( !PStr->binf ) PStrMoveP( PStr, "," ) ;
	TempFloat( PStr, Model, ( X_FLOAT * )&Matrix->matrix[2][2] ) ;	if( !PStr->binf ) PStrMoveP( PStr, "," ) ;
	TempFloat( PStr, Model, ( X_FLOAT * )&Matrix->matrix[2][3] ) ;	if( !PStr->binf ) PStrMoveP( PStr, "," ) ;

	TempFloat( PStr, Model, ( X_FLOAT * )&Matrix->matrix[3][0] ) ;	if( !PStr->binf ) PStrMoveP( PStr, "," ) ;
	TempFloat( PStr, Model, ( X_FLOAT * )&Matrix->matrix[3][1] ) ;	if( !PStr->binf ) PStrMoveP( PStr, "," ) ;
	TempFloat( PStr, Model, ( X_FLOAT * )&Matrix->matrix[3][2] ) ;	if( !PStr->binf ) PStrMoveP( PStr, "," ) ;
	TempFloat( PStr, Model, ( X_FLOAT * )&Matrix->matrix[3][3] ) ;	if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;

	// 終了
	return 0 ;
}

// アルファつきカラーテンプレートの解析
static int TempColorRGBA( X_PSTRING *PStr, X_MODEL *Model, X_COLORRGBA *ColorRGBA )
{
	// フロート型を４つ読み込む
	TempFloat( PStr, Model, ( X_FLOAT * )&ColorRGBA->red ) ;	if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;
	TempFloat( PStr, Model, ( X_FLOAT * )&ColorRGBA->green ) ;	if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;
	TempFloat( PStr, Model, ( X_FLOAT * )&ColorRGBA->blue ) ;	if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;
	TempFloat( PStr, Model, ( X_FLOAT * )&ColorRGBA->alpha ) ;	if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;

	// 終了
	return 0 ;
}

// カラーテンプレートの解析
static int TempColorRGB( X_PSTRING *PStr, X_MODEL *Model, X_COLORRGB *ColorRGB )
{
	// フロート型を４つ読み込む
	TempFloat( PStr, Model, ( X_FLOAT * )&ColorRGB->red ) ;		if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;
	TempFloat( PStr, Model, ( X_FLOAT * )&ColorRGB->green ) ;	if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;
	TempFloat( PStr, Model, ( X_FLOAT * )&ColorRGB->blue ) ;	if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;

	// 終了
	return 0 ;
}

// マテリアルテンプレートの解析
static int TempMaterial( X_PSTRING *PStr, X_MODEL *Model, X_MATERIAL *Material )
{
	if( !PStr->binf )
	{
		// フェイスカラーを読み込む
		TempColorRGBA( PStr, Model, &Material->faceColor ) ;		if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;

		// パワーを読み込む
		TempFloat( PStr, Model, ( X_FLOAT * )&Material->power ) ;	if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;

		// スペキュラカラーとエミッシブカラーを読み込む
		TempColorRGB( PStr, Model, &Material->specularColor ) ;		if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;
		TempColorRGB( PStr, Model, &Material->emissiveColor ) ;		if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;
	}
	else
	{
		if( PSTRWORD(PStr,0) != TOKEN_FLOAT_LIST )
		{
			DXST_LOGFILEFMT_ADDW(( L"Load XFile : Material FloadList\n" )) ;
			return -1 ;
		}
		if( PSTRDWORD(PStr,2) != 11 )
		{
			DXST_LOGFILEFMT_ADDW(( L"Load XFile : Material Not 11\n" )) ;
			return -1 ;
		}

		if( PStr->f64f )
		{
			// フェイス・パワー・スケキュラ・エミッシブを読み込む
			Material->faceColor.red   = PSTRDOUBLE(PStr,6) ;
			Material->faceColor.green = PSTRDOUBLE(PStr,14) ;
			Material->faceColor.blue  = PSTRDOUBLE(PStr,22) ;
			Material->faceColor.alpha = PSTRDOUBLE(PStr,30) ;
			Material->power = PSTRDOUBLE(PStr,38) ;
			Material->specularColor.red   = PSTRDOUBLE(PStr,46) ;
			Material->specularColor.green = PSTRDOUBLE(PStr,54) ;
			Material->specularColor.blue  = PSTRDOUBLE(PStr,62) ;
			Material->emissiveColor.red   = PSTRDOUBLE(PStr,70) ;
			Material->emissiveColor.green = PSTRDOUBLE(PStr,78) ;
			Material->emissiveColor.blue  = PSTRDOUBLE(PStr,86) ;
		}
		else
		{
			// フェイス・パワー・スケキュラ・エミッシブを読み込む
			Material->faceColor.red = PSTRFLOAT(PStr,6) ;	Material->faceColor.green = PSTRFLOAT(PStr,10) ;	Material->faceColor.blue = PSTRFLOAT(PStr,14) ;	Material->faceColor.alpha = PSTRFLOAT(PStr,18) ;
			Material->power = PSTRFLOAT(PStr,22) ;
			Material->specularColor.red = PSTRFLOAT(PStr,26) ;	Material->specularColor.green = PSTRFLOAT(PStr,30) ;	Material->specularColor.blue = PSTRFLOAT(PStr,34) ;
			Material->emissiveColor.red = PSTRFLOAT(PStr,38) ;	Material->emissiveColor.green = PSTRFLOAT(PStr,42) ;	Material->emissiveColor.blue = PSTRFLOAT(PStr,46) ;
		}

		PStrMoveOneB( PStr ) ;
	}

	// 終了
	return 0 ;
}

// テクスチャファイルネームテンプレートの解析
static int TempTextureFilename( X_PSTRING *PStr, X_MODEL *Model, X_TEXTUREFILENAME *TexFileName )
{
	int i ;
	X_TEXTUREFILENAME_LIST *Name, *BackName ;

	// テクスチャファイルネームを読み込む
	TempString( PStr, Model, &TexFileName->filename ) ;	if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;

	// 今まで同じ名前のテクスチャファイル名があったか調べる
	Name = Model->FirstTextureFileName ;
	BackName = Name ;
	for( i = 0 ; i < Model->TextureNum && _STRCMP( Name->FileName->filename.String, TexFileName->filename.String ) != 0 ; i ++, BackName = Name, Name = Name->Next ){}

	// 無かったら追加
	if( i == Model->TextureNum )
	{
		// 新たなオブジェクトのメモリを確保
		Name = ( X_TEXTUREFILENAME_LIST * )ADDMEMAREA( sizeof( X_TEXTUREFILENAME_LIST ), &Model->XModelMem ) ; 
		if( Name == NULL )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\xd5\x30\xa1\x30\xa4\x30\xeb\x30\xcd\x30\xfc\x30\xe0\x30\xea\x30\xb9\x30\xc8\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : テクスチャファイルネームリストオブジェクトを格納するメモリ領域の確保に失敗しました\n" @*/ )) ;
			return -1 ;
		}

		// パラメータのセット
		Name->FileName = TexFileName ;
		Name->Next = NULL ;

		// リストへの追加
		if( Model->TextureNum == 0 )
		{
			Model->FirstTextureFileName = Name ;
		}
		else
		{
			BackName->Next = Name ;
		}

		// 数をインクリメント
		Model->TextureNum ++ ;
	}

	// インデックスをセット
	TexFileName->Index = i ;

	// 終了
	return 0 ;
}

// メッシュフェイステンプレートの解析
static int TempMeshFace( X_PSTRING *PStr, X_MODEL *Model, X_MESHFACE *MeshFace )
{
	DWORD i ;

	if( !PStr->binf )
	{
		// フェイスの数を読み込む
		TempDword( PStr, Model, ( X_DWORD * )&MeshFace->nFaceVertexIndices ) ;	if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;
		if( MeshFace->nFaceVertexIndices > MAX_INDEXNUM )
		{
			MeshFace->faceVertexIndices = ( DWORD * )ADDMEMAREA( sizeof( DWORD ) * MeshFace->nFaceVertexIndices, &Model->XModelMem ) ;
			if( MeshFace->faceVertexIndices == NULL )
			{
	//			DXST_LOGFILEFMT_ADDW(( L"Load XFile : 一つの面で使用できる頂点数の最大数 %d を超えた面がありました( %d 個の頂点 )", MAX_INDEXNUM, MeshFace->nFaceVertexIndices ) ) ;
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\xd5\x30\xa7\x30\xa4\x30\xb9\x30\x92\x30\xcb\x69\x10\x62\x59\x30\x8b\x30\x02\x98\xb9\x70\x6e\x30\xa4\x30\xf3\x30\xc7\x30\xc3\x30\xaf\x30\xb9\x30\x92\x30\xdd\x4f\x58\x5b\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x28\x00\x20\x00\x25\x00\x64\x00\x20\x00\x0b\x50\x6e\x30\x02\x98\xb9\x70\x20\x00\x29\x00\x00"/*@ L"Load XFile : フェイスを構成する頂点のインデックスを保存するメモリ領域の確保に失敗しました( %d 個の頂点 )" @*/, MeshFace->nFaceVertexIndices ) ) ;
				return -1 ;
			}
		}

		// フェイスの数だけインデックスを読み込む
		if( MeshFace->nFaceVertexIndices != 0 )
		{
			if( MeshFace->faceVertexIndices != NULL )
			{
				for( i = 0 ; i < MeshFace->nFaceVertexIndices - 1 ; i ++ )
				{
					TempDword( PStr, Model, ( X_DWORD * )&MeshFace->faceVertexIndices[i] ) ;
					PStrMoveP( PStr, ";," ) ;
				}
				TempDword( PStr, Model, ( X_DWORD * )&MeshFace->faceVertexIndices[i] ) ;
			}
			else
			{
				for( i = 0 ; i < MeshFace->nFaceVertexIndices - 1 ; i ++ )
				{
					TempDword( PStr, Model, ( X_DWORD * )&MeshFace->faceVertexIndicesFixedLength[i] ) ;
					PStrMoveP( PStr, ";," ) ;
				}
				TempDword( PStr, Model, ( X_DWORD * )&MeshFace->faceVertexIndicesFixedLength[i] ) ;
			}
			PStrMoveP( PStr, ";," ) ;
		}
	}
	else
	{
		DWORD j ;

		// フェイスの数を読み込む
		MeshFace->nFaceVertexIndices = PSTRDWORD(PStr,6) ;
		if( MeshFace->nFaceVertexIndices > MAX_INDEXNUM )
		{
			MeshFace->faceVertexIndices = ( DWORD * )ADDMEMAREA( sizeof( DWORD ) * MeshFace->nFaceVertexIndices, &Model->XModelMem ) ;
			if( MeshFace->faceVertexIndices == NULL )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\xd5\x30\xa7\x30\xa4\x30\xb9\x30\x92\x30\xcb\x69\x10\x62\x59\x30\x8b\x30\x02\x98\xb9\x70\x6e\x30\xa4\x30\xf3\x30\xc7\x30\xc3\x30\xaf\x30\xb9\x30\x92\x30\xdd\x4f\x58\x5b\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x28\x00\x20\x00\x25\x00\x64\x00\x20\x00\x0b\x50\x6e\x30\x02\x98\xb9\x70\x20\x00\x29\x00\x00"/*@ L"Load XFile : フェイスを構成する頂点のインデックスを保存するメモリ領域の確保に失敗しました( %d 個の頂点 )" @*/, MeshFace->nFaceVertexIndices ) ) ;
				return -1 ;
			}
		}

		// フェイスの数だけインデックスを読み込む
		j = 10 ;
		if( MeshFace->faceVertexIndices != NULL )
		{
			for( i = 0 ; i < MeshFace->nFaceVertexIndices ; i ++, j += 4 )
			{
				MeshFace->faceVertexIndices[i] = PSTRDWORD(PStr,j) ;
			}
		}
		else
		{
			for( i = 0 ; i < MeshFace->nFaceVertexIndices ; i ++, j += 4 )
			{
				MeshFace->faceVertexIndicesFixedLength[i] = PSTRDWORD(PStr,j) ;
			}
		}

		PStrMoveOneB(PStr) ;
	}


	// 終了
	return 0 ;
}

// メッシュテクスチャーコードステンプレートの解析
static int TempMeshTextureCoords( X_PSTRING *PStr, X_MODEL *Model, X_MESHTEXTURECOORDS *MeshTextureCoords )
{
	DWORD i, j ;
	X_COORDS2D *C ;

	// テクスチャ座標の数を数える
	TempDword( PStr, Model, ( X_DWORD * )&MeshTextureCoords->nTextureCoords ) ;	if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;

	// テクスチャ座標を格納するメモリの確保
	if( ( MeshTextureCoords->textureCoords = ( X_COORDS2D * )ADDMEMAREA( sizeof( X_COORDS2D ) * MeshTextureCoords->nTextureCoords, &Model->XModelMem ) ) == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\x55\x00\x56\x00\xa7\x5e\x19\x6a\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : UV座標を格納するメモリ領域の確保に失敗しました\n" @*/ )) ;
		return -1 ;
	}

	// テクスチャ座標の数だけインデックスを読み込む
	if( !PStr->binf )
	{
		if( MeshTextureCoords->nTextureCoords != 0 )
		{
			C = MeshTextureCoords->textureCoords ;
			for( i = 0 ; i < MeshTextureCoords->nTextureCoords - 1 ; i ++, C ++ )
			{
				TempCoords2d( PStr, Model, C ) ;

				// 変な記述用のプログラム
				PStrMoveP( PStr, "0123456789.-+," ) ;
				if( ( PStr->StrBuf[ PStr->StrOffset - 1 ] >= '0' && PStr->StrBuf[ PStr->StrOffset - 1 ] <= '9' ) ||
					PStr->StrBuf[ PStr->StrOffset - 1 ] == '.' ||
					PStr->StrBuf[ PStr->StrOffset - 1 ] == '+' ||
					PStr->StrBuf[ PStr->StrOffset - 1 ] == '-' )
				{
					PStr->StrOffset -- ;
				}
			}
			TempCoords2d( PStr, Model, C ) ;

			// 変な記述用に } も含める
			PStrMoveP( PStr, "};," ) ;

			// 変な記述用のプログラム
			if( PStr->StrBuf[ PStr->StrOffset - 1 ] == '}' )
			{
				PStr->StrOffset -- ;
			}
		}
	}
	else
	{
		if( PSTRWORD(PStr, 0) != TOKEN_FLOAT_LIST )
		{
			DXST_LOGFILEFMT_ADDW(( L"Load XFile : MeshTextureCorrds FloadList\n" )) ;
			return -1 ;
		}
		if( PSTRDWORD(PStr, 2) != ( DWORD )( MeshTextureCoords->nTextureCoords * 2 ) )
		{
			DXST_LOGFILEFMT_ADDW(( L"Load XFile : MeshTextureCorrds Not %d ( %d )\n", MeshTextureCoords->nTextureCoords * 2, PSTRDWORD(PStr, 2) ) ) ;
			return -1 ;
		}

		j = 6 ;
		C = MeshTextureCoords->textureCoords ;
		if( PStr->f64f )
		{
			for( i = 0 ; i < MeshTextureCoords->nTextureCoords ; i ++, C ++, j += 16 )
			{
				C->u = PSTRDOUBLE(PStr,j) ;  C->v = PSTRDOUBLE(PStr,j+8) ;
			}
		}
		else
		{
			for( i = 0 ; i < MeshTextureCoords->nTextureCoords ; i ++, C ++, j += 8 )
			{
				C->u = PSTRFLOAT(PStr,j) ;  C->v = PSTRFLOAT(PStr,j+4) ;
			}
		}

		PStrMoveOneB( PStr ) ;
	}

	// 終了
	return 0 ;
}

// メッシュノーマルステンプレートの解析
static int TempMeshNormals( X_PSTRING *PStr, X_MODEL *Model, X_MESHNORMALS *MeshNormals )
{
	DWORD i, j, k ;
	X_VECTOR *V ;
	X_MESHFACE *MF ;

	// 法線の数を取得する
	TempDword( PStr, Model, ( X_DWORD * )&MeshNormals->nNormals ) ;	if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;

	// 法線を格納するメモリの確保
	if( ( MeshNormals->normals = ( X_VECTOR * )ADDMEMAREA( sizeof( X_VECTOR ) * MeshNormals->nNormals, &Model->XModelMem ) ) == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\xd5\x6c\xda\x7d\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : 法線を格納するメモリ領域の確保に失敗しました\n" @*/ )) ;
		return -1 ;
	}

	// 法線の数だけインデックスを読み込む
	if( !PStr->binf )
	{
		if( MeshNormals->nNormals != 0 )
		{
			V = MeshNormals->normals ;
			for( i = 0 ; i < MeshNormals->nNormals - 1 ; i ++, V ++ )
			{
				TempVector( PStr, Model, V ) ;
				if( !PStr->binf ) PStrMoveP( PStr, "," ) ;
			}
			TempVector( PStr, Model, V ) ;
			if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;
		}
	}
	else
	{
		if( PSTRWORD(PStr, 0) != TOKEN_FLOAT_LIST )
		{
			DXST_LOGFILEFMT_ADDW(( L"Load XFile : MeshNormals FloadList\n" )) ;
			return -1 ;
		}
		if( PSTRDWORD(PStr, 2) != ( DWORD )( MeshNormals->nNormals * 3 ) )
		{
			DXST_LOGFILEFMT_ADDW(( L"Load XFile : MeshNormals Not %d ( %d )\n", MeshNormals->nNormals * 3, PSTRDWORD(PStr, 2) ) ) ;
			return -1 ;
		}

		V = MeshNormals->normals ;
		j = 6 ;
		if( PStr->f64f )
		{
			for( i = 0 ; i < MeshNormals->nNormals ; i ++, V ++, j += 24 )
			{
				V->dat.x = (float)PSTRDOUBLE(PStr,j) ; V->dat.y = (float)PSTRDOUBLE(PStr,j+8) ; V->dat.z = (float)PSTRDOUBLE(PStr,j+16) ;
			}
		}
		else
		{
			for( i = 0 ; i < MeshNormals->nNormals  ; i ++, V ++, j += 12 )
			{
				V->dat.x = PSTRFLOAT(PStr,j) ; V->dat.y = PSTRFLOAT(PStr,j+4) ; V->dat.z = PSTRFLOAT(PStr,j+8) ;
			}
		}
		PStrMoveOneB( PStr ) ;
	}

	// 法線フェイスの数を取得する
//	TempDword( PStr, Model, ( X_DWORD * )&MeshNormals->nFaceNormals ) ;	if( !PStr->binf ) PStrMoveP( PStr, ";" ) ;
	if( !PStr->binf )
	{
		TempDword( PStr, Model, ( X_DWORD * )&MeshNormals->nFaceNormals ) ;	if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;
	}
	else
	{
		switch( PSTRWORD(PStr,0) )
		{
		case TOKEN_INTEGER :		MeshNormals->nFaceNormals = PSTRDWORD(PStr,2) ; break ;
		case TOKEN_INTEGER_LIST :	MeshNormals->nFaceNormals = PSTRDWORD(PStr,6) ; break ;
		}
	}

	// 法線フェイスを格納するメモリの確保
	if( ( MeshNormals->faceNormals = ( X_MESHFACE * )ADDMEMAREA( sizeof( X_MESHFACE ) * MeshNormals->nFaceNormals, &Model->XModelMem ) ) == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\xd5\x6c\xda\x7d\x62\x97\xc5\x60\x31\x58\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : 法線面情報を格納するメモリ領域の確保に失敗しました\n" @*/ )) ;
		return -1 ;
	}

	// 法線フェイスの数だけフェイスを読み込む
	if( !PStr->binf )
	{
		if( MeshNormals->nFaceNormals != 0 )
		{
			MF = MeshNormals->faceNormals ;
			for( i = 0 ; i < MeshNormals->nFaceNormals - 1 ; i ++, MF ++ )
			{
				if( !PStr->binf ) PStrMoveNum( PStr );
				if( TempMeshFace( PStr, Model, MF ) < 0 )
				{
					return -1 ;
				}
			}
			if( !PStr->binf ) PStrMoveNum( PStr );
			if( TempMeshFace( PStr, Model, MF ) < 0 )
			{
				return -1 ;
			}
		}
	}
	else
	{
//		if( PSTRWORD(PStr, 0) != TOKEN_INTEGER_LIST ) return DXST_LOGFILE_ADDW( L"Error aadfa;bb" ) ;

		j = 0 ;
		switch( PSTRWORD(PStr,0) )
		{
		case TOKEN_INTEGER : PStrMoveOneB( PStr ) ; j = 6 ; break ;
		case TOKEN_INTEGER_LIST : j = 10 ; break ;
		}

		MF = MeshNormals->faceNormals ;
		for( i = 0 ; i < MeshNormals->nFaceNormals ; i ++, MF ++ )
		{
			MF->nFaceVertexIndices = PSTRDWORD(PStr,j) ; j += 4 ;
			if( MF->faceVertexIndices != NULL )
			{
				for( k = 0 ; k < MF->nFaceVertexIndices ; k ++, j += 4 )
				{
					MF->faceVertexIndices[k] = PSTRDWORD(PStr,j) ;
				}
			}
			else
			{
				for( k = 0 ; k < MF->nFaceVertexIndices ; k ++, j += 4 )
				{
					MF->faceVertexIndicesFixedLength[k] = PSTRDWORD(PStr,j) ;
				}
			}
		}
		PStrMoveOneB( PStr ) ;
	}

	// 終了
	return 0 ;
}


// メッシュマテリアルリストテンプレートの解析
static int TempMeshMaterialList( X_PSTRING *PStr, X_MODEL *Model, X_MESHMATERIALLIST *MeshMaterialList )
{
	DWORD i, MaxIndex ;
	DWORD *D ;

	MaxIndex = 0 ;
	if( !PStr->binf )
	{
		// マテリアルの種類の数を取得する
		TempDword( PStr, Model, ( X_DWORD * )&MeshMaterialList->nMaterials ) ;	if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;

		// 適応面情報の数を取得する
		TempDword( PStr, Model, ( X_DWORD * )&MeshMaterialList->nFaceIndexes ) ;	if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;
	}
	else
	{
		if( PSTRWORD(PStr,0) != TOKEN_INTEGER_LIST )
		{
			DXST_LOGFILEFMT_ADDW(( L"Load XFile : MeshMaterialList IntergerList\n" )) ;
			return -1 ;
		}

		// マテリアルの種類の数を取得する
		MeshMaterialList->nMaterials = PSTRDWORD(PStr,6) ;

		// 適応面情報の数を取得する
		MeshMaterialList->nFaceIndexes = PSTRDWORD(PStr,10) ;
	}

	// 適応面情報を格納するメモリの確保
	if( ( MeshMaterialList->FaceIndexes = ( DWORD * )ADDMEMAREA( sizeof( DWORD ) * MeshMaterialList->nFaceIndexes, &Model->XModelMem ) ) == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\xde\x30\xc6\x30\xea\x30\xa2\x30\xeb\x30\x68\x30\x62\x97\x6e\x30\xa2\x95\xc2\x4f\xc5\x60\x31\x58\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : マテリアルと面の関係情報を格納するメモリ領域の確保に失敗しました\n" @*/ )) ;
		return -1 ;
	}

	// 適応面情報の数だけインデックスを読み込む
	if( !PStr->binf )
	{
		if( MeshMaterialList->nFaceIndexes != 0 )
		{
			D = MeshMaterialList->FaceIndexes ;
			for( i = 0 ; i < MeshMaterialList->nFaceIndexes - 1 ; i ++, D ++ )
			{
				TempDword( PStr, Model, ( X_DWORD * )D ) ;
				if( *D > MaxIndex ) MaxIndex = *D ;
				if( !PStr->binf ) PStrMoveP( PStr, "," ) ;
			}
			TempDword( PStr, Model, ( X_DWORD * )D ) ;
			if( *D > MaxIndex ) MaxIndex = *D ;
			if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;
		}
	}
	else
	{
		DWORD j ;

		j = 14 ;
		D = MeshMaterialList->FaceIndexes ;
		for( i = 0 ; i < MeshMaterialList->nFaceIndexes ; i ++, D ++, j += 4 )
		{
			*D = PSTRDWORD(PStr,j) ;
			if( *D > MaxIndex ) MaxIndex = *D ;
		}

		PStrMoveOneB(PStr) ;
	}

	if( MaxIndex + 1 > MeshMaterialList->nMaterials )
	{
//		DXST_LOGFILE_ADDW( L"Load XFile : nMaterials と実際に使用されているマテリアルの数が違います\n" ) ;
		DXST_LOGFILE_ADDUTF16LE( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\x6e\x00\x4d\x00\x61\x00\x74\x00\x65\x00\x72\x00\x69\x00\x61\x00\x6c\x00\x73\x00\x20\x00\x88\x30\x8a\x30\x27\x59\x4d\x30\x44\x30\xde\x30\xc6\x30\xea\x30\xa2\x30\xeb\x30\xa4\x30\xf3\x30\xc7\x30\xc3\x30\xaf\x30\xb9\x30\x4c\x30\x42\x30\x8a\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : nMaterials より大きいマテリアルインデックスがありました\n" @*/ ) ;
		return -1 ;
	}

	// 終了
	return 0 ;
}

// インデックスカラーの解析
static int TempIndexedColor( X_PSTRING *PStr, X_MODEL *Model, X_INDEXEDCOLOR *IndexedColor )
{
	TempDword( PStr, Model, ( X_DWORD * )&IndexedColor->index ) ; 	if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;
	if( !PStr->binf )
	{
		TempColorRGBA( PStr, Model, &IndexedColor->indexColor ) ;
		PStrMoveP( PStr, ";," ) ;
	}
	else
	{
		if( PSTRWORD(PStr, 0) != TOKEN_FLOAT_LIST )
		{
			DXST_LOGFILEFMT_ADDW(( L"Load XFile : IndexedColor FloadList\n" )) ;
			return -1 ;
		}
		if( PStr->f64f )
		{
			IndexedColor->indexColor.red   = PSTRDOUBLE( PStr, 6 ) ;
			IndexedColor->indexColor.green = PSTRDOUBLE( PStr, 14 ) ;
			IndexedColor->indexColor.blue  = PSTRDOUBLE( PStr, 22 ) ;
			IndexedColor->indexColor.alpha = PSTRDOUBLE( PStr, 30 ) ;
		}
		else
		{
			IndexedColor->indexColor.red   = PSTRFLOAT( PStr, 6 ) ;
			IndexedColor->indexColor.green = PSTRFLOAT( PStr, 10 ) ;
			IndexedColor->indexColor.blue  = PSTRFLOAT( PStr, 14 ) ;
			IndexedColor->indexColor.alpha = PSTRFLOAT( PStr, 18 ) ;
		}

		PStrMoveOneB(PStr) ;
	}

	// 終了
	return 0 ;
}

// メッシュ頂点カラーの解析
static int TempMeshVertexColors( X_PSTRING *PStr, X_MODEL *Model, X_MESHVERTEXCOLORS *MeshVertexColors )
{
	int i ;

	// カラーの数を取得
	TempDword( PStr, Model, ( X_DWORD * )&MeshVertexColors->nVertexColors ) ; 	if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;

	// カラーを格納するためのメモリの確保
	if( ( MeshVertexColors->vertexColors = ( X_INDEXEDCOLOR * )ADDMEMAREA( sizeof( X_INDEXEDCOLOR ) * MeshVertexColors->nVertexColors, &Model->XModelMem ) ) == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\x02\x98\xb9\x70\xab\x30\xe9\x30\xfc\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : 頂点カラーを格納するメモリ領域の確保に失敗しました\n" @*/ )) ;
		return -1 ;
	}

	// 頂点カラーの解析
	for( i = 0 ; i < ( int )MeshVertexColors->nVertexColors ; i ++ )
	{
		TempIndexedColor( PStr, Model, &MeshVertexColors->vertexColors[ i ] ) ;
	 	if( !PStr->binf )
		{
			if( PSTRC( PStr ) != ';' && PSTRC( PStr ) != ',' )
			{
				MeshVertexColors->vertexColors[ i ].indexColor.red   *= 255.0f ;
				MeshVertexColors->vertexColors[ i ].indexColor.green *= 255.0f ;
				MeshVertexColors->vertexColors[ i ].indexColor.blue  *= 255.0f ;
				MeshVertexColors->vertexColors[ i ].indexColor.alpha *= 255.0f ;
			}
			else
			{
				PStrMoveP( PStr, ";," ) ;
			}
		}
	}

	// 終了
	return 0 ;
}

// メッシュテンプレートの解析
static int TempMesh( X_PSTRING *PStr, X_MODEL *Model, X_MESH *Mesh )
{
	DWORD i, j, k ;
	X_VECTOR *V ;
	X_MESHFACE *MF ;

	// 頂点の数を取得する
	TempDword( PStr, Model, ( X_DWORD * )&Mesh->nVertices ) ;	if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;

	// 頂点を格納するメモリの確保
	if( ( Mesh->vertices = ( X_VECTOR * )ADDMEMAREA( sizeof( X_VECTOR ) * Mesh->nVertices, &Model->XModelMem ) ) == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\x02\x98\xb9\x70\xa7\x5e\x19\x6a\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : 頂点座標を格納するメモリ領域の確保に失敗しました\n" @*/ )) ;
		return -1 ;
	}

	// 頂点の数だけインデックスを読み込む
	if( !PStr->binf )
	{
		if( Mesh->nVertices != 0 )
		{
			V = Mesh->vertices ;
			for( i = 0 ; i < Mesh->nVertices - 1 ; i ++, V ++ )
			{
				TempVector( PStr, Model, V ) ;
				if( !PStr->binf ) PStrMoveP( PStr, "," ) ;
			}
			TempVector( PStr, Model, V ) ;
			if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;
		}
	}
	else
	{
		if( PSTRWORD(PStr, 0) != TOKEN_FLOAT_LIST )
		{
			DXST_LOGFILEFMT_ADDW(( L"Load XFile : Mesh FloadList\n" )) ;
			return -1 ;
		}
		if( PSTRDWORD(PStr, 2) != ( DWORD )( Mesh->nVertices * 3 ) )
		{
			DXST_LOGFILEFMT_ADDW(( L"Load XFile : Mesh Not %d ( %d )\n", Mesh->nVertices * 3, PSTRDWORD(PStr, 2) ) ) ;
			return -1 ;
		}

		V = Mesh->vertices ;
		j = 6 ;
		if( PStr->f64f )
		{
			for( i = 0 ; i < Mesh->nVertices  ; i ++, V ++, j += 24 )
			{
				V->dat.x = (float)PSTRDOUBLE(PStr,j) ; V->dat.y = (float)PSTRDOUBLE(PStr,j+8) ; V->dat.z = (float)PSTRDOUBLE(PStr,j+16) ;
			}
		}
		else
		{
			for( i = 0 ; i < Mesh->nVertices  ; i ++, V ++, j += 12 )
			{
				V->dat.x = PSTRFLOAT(PStr,j) ; V->dat.y = PSTRFLOAT(PStr,j+4) ; V->dat.z = PSTRFLOAT(PStr,j+8) ;
			}
		}
		PStrMoveOneB( PStr ) ;
	}

	// 頂点フェイスの数を取得する
	if( !PStr->binf )
	{
		TempDword( PStr, Model, ( X_DWORD * )&Mesh->nFaces ) ;
		PStrMoveP( PStr, ";," ) ;
	}
	else
	{
		switch( PSTRWORD(PStr,0) )
		{
		case TOKEN_INTEGER :		Mesh->nFaces = PSTRDWORD(PStr,2) ; break ;
		case TOKEN_INTEGER_LIST :	Mesh->nFaces = PSTRDWORD(PStr,6) ; break ;
		}
	}

	// 頂点フェイスを格納するメモリの確保
	if( ( Mesh->faces = ( X_MESHFACE * )ADDMEMAREA( sizeof( X_MESHFACE ) * Mesh->nFaces, &Model->XModelMem ) ) == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\x02\x98\xb9\x70\x62\x97\xc5\x60\x31\x58\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : 頂点面情報を格納するメモリ領域の確保に失敗しました\n" @*/ )) ;
		return -1 ;
	}
	Mesh->nTriangles = 0 ;

	// 頂点フェイスの数だけフェイスを読み込む
	if( !PStr->binf )
	{
		if( Mesh->nFaces != 0 )
		{
			MF = Mesh->faces ;
			for( i = 0 ; i < Mesh->nFaces - 1 ; i ++, MF ++ )
			{
				if( !PStr->binf ) PStrMoveNum( PStr );
				if( TempMeshFace( PStr, Model, MF ) < 0 )
				{
					return -1 ;
				}
				Mesh->nTriangles += MF->nFaceVertexIndices == 4 ? 2 : 1 ;
			}
			if( !PStr->binf ) PStrMoveNum( PStr );
			if( TempMeshFace( PStr, Model, MF ) < 0 )
			{
				return -1 ;
			}
			Mesh->nTriangles += MF->nFaceVertexIndices == 4 ? 2 : 1 ;
		}
	}
	else
	{
//		if( PSTRWORD(PStr, 0) != TOKEN_INTEGER_LIST ) return DXST_LOGFILE_ADDW( L"Error aadfa;bb" ) ;

		j = 0 ;
		switch( PSTRWORD(PStr,0) )
		{
		case TOKEN_INTEGER : PStrMoveOneB( PStr ) ; j = 6 ; break ;
		case TOKEN_INTEGER_LIST : j = 10 ; break ;
		}

		MF = Mesh->faces ;
		for( i = 0 ; i < Mesh->nFaces ; i ++, MF ++ )
		{
			MF->nFaceVertexIndices = PSTRDWORD(PStr,j) ; j += 4 ;
			if( MF->faceVertexIndices != NULL )
			{
				for( k = 0 ; k < MF->nFaceVertexIndices ; k ++, j += 4 )
				{
					MF->faceVertexIndices[k] = PSTRDWORD(PStr,j) ; 
				}
			}
			else
			{
				for( k = 0 ; k < MF->nFaceVertexIndices ; k ++, j += 4 )
				{
					MF->faceVertexIndicesFixedLength[k] = PSTRDWORD(PStr,j) ; 
				}
			}
			Mesh->nTriangles += MF->nFaceVertexIndices == 4 ? 2 : 1 ;
		}
		PStrMoveOneB( PStr ) ;
	}

	// 終了
	return 0 ;
}

// フレーム(及びそのすべてのこオブジェクト)のローカルトランスフォームテンプレートの解析
static int TempFrameTransformMatrix( X_PSTRING *PStr, X_MODEL *Model, X_FRAMETRANSFORMMATRIX *FrameTransformMatrix )
{
	// 行列を得る
	if( !PStr->binf )
	{
		TempMatrix4x4( PStr, Model, &FrameTransformMatrix->frameMatrix ) ;
		if( !PStr->binf )
		{
			PStrMoveP( PStr, ";,}" ) ;

			// 変な記述用のプログラム
			if( PStr->StrBuf[ PStr->StrOffset - 1 ] == '}' )
			{
				PStr->StrOffset -- ;
			}
		}
	}
	else
	{
		DWORD i, j, k ;
		X_MATRIX4X4 *mt ;

		if( PSTRWORD(PStr,0) != TOKEN_FLOAT_LIST )
		{
			DXST_LOGFILEFMT_ADDW(( L"Load XFile : FrameTransformMatrix FloadList\n" )) ;
			return -1 ;
		}

		mt = &FrameTransformMatrix->frameMatrix ;
		if( PSTRDWORD(PStr,2) == 16 )
		{
			k = 6 ;
			if( PStr->f64f )
			{
				for( i = 0 ; i < 4 ; i ++ )
					for( j = 0 ; j < 4 ; j ++, k += 8 )
						mt->matrix[i][j] = PSTRDOUBLE(PStr,k) ;
			}
			else
			{
				for( i = 0 ; i < 4 ; i ++ )
					for( j = 0 ; j < 4 ; j ++, k += 4 )
						mt->matrix[i][j] = PSTRFLOAT(PStr,k) ;
			}
			PStrMoveOneB( PStr ) ;
		}
		else
		if( PSTRDWORD(PStr,2) == 4 )
		{
			for( i = 0 ; i < 4 ; i ++ )
			{
				if( PSTRWORD(PStr,0) != TOKEN_FLOAT_LIST )
				{
					DXST_LOGFILEFMT_ADDW(( L"Load XFile : FrameTransformMatrix MatrixLine No.%d FloadList\n", i ) ) ;
					return -1 ;
				}
				if( PSTRDWORD(PStr,2) != 4 )
				{
					DXST_LOGFILEFMT_ADDW(( L"Load XFile : FrameTransformMatrix MatrixLine No.%d Not 4 ( %d )\n", i, PSTRDWORD(PStr, 2) ) ) ;
					return -1 ;
				}

				k = 6 ;
				for( j = 0 ; j < 4 ; j ++, k += 4 )
					mt->matrix[i][j] = PSTRFLOAT(PStr,k) ;
				PStrMoveOneB( PStr ) ;
			}
		}
		else
		{
			DXST_LOGFILEFMT_ADDA(( "Load XFile : FrameTransformMatrix Not 16 ( %d )\n", PSTRDWORD(PStr, 2) ) ) ;
			return -1 ;
		}
	}

	// 終了
	return 0 ;
}

// フレームテンプレートの解析
static int TempFrame( X_PSTRING * /*PStr*/, X_MODEL * /*Model*/, X_FRAME * /*Frame*/ )
{
	// 特にすることなし

	// 終了
	return 0 ;
}

// 浮動小数点の配列及び配列内の float の数テンプレートの解析
static int TempFloatKeys( X_PSTRING *PStr, X_MODEL *Model, X_FLOATKEYS *FloatKeys )
{
	float *f ;
	DWORD i ;

	// float 値の数を取得
	TempDword( PStr, Model, ( X_DWORD * )&FloatKeys->nValues ) ;	if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;

	if( FloatKeys->nValues == 0 )
	{
		i = 8 ;
	}

	// float 値を格納するメモリの確保
	if( ( FloatKeys->values = ( float * )ADDMEMAREA( sizeof( float ) * FloatKeys->nValues, &Model->XModelMem ) ) == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\x6e\x6d\xd5\x52\x0f\x5c\x70\x65\xb9\x70\xea\x30\xb9\x30\xc8\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : 浮動小数点リストを格納するメモリ領域の確保に失敗しました\n" @*/ )) ;
		return -1 ;
	}

	if( !PStr->binf )
	{
		// float 値の数だけ繰り返す
		if( FloatKeys->nValues != 0 )
		{
			f = FloatKeys->values ;
			for( i = 0 ; i < FloatKeys->nValues - 1 ; i ++, f ++ )
			{
				TempFloat( PStr, Model, ( X_FLOAT * )f ) ;
				if( !PStr->binf ) PStrMove( PStr, "," ) ;
			}
			TempFloat( PStr, Model, ( X_FLOAT * )f ) ;
			if( !PStr->binf ) PStrMove( PStr, ";," ) ;
		}
	}
	else
	{
		DWORD j ;

		PStrMoveOneB(PStr) ;

		// float 値の数だけ繰り返す
		j = 6 ;
		if( PStr->f64f )
		{
			f = FloatKeys->values ;
			for( i = 0 ; i < FloatKeys->nValues ; i ++, f ++, j += 8 )
				*f = PSTRDOUBLE(PStr,j) ;
		}
		else
		{
			f = FloatKeys->values ;
			for( i = 0 ; i < FloatKeys->nValues ; i ++, f ++, j += 4 )
				*f = PSTRFLOAT(PStr,j) ;
		}

		PStrMoveOneB(PStr) ;
	}

	// 終了
	return 0 ;
}

// アニメーションで使用される浮動小数点数のセット及び時間を定義するテンプレートの解析
static int TempTimedFloatKeys( X_PSTRING *PStr, X_MODEL *Model, X_TIMEDFLOATKEYS *TimedFloatKeys )
{
	DWORD i ;
	float *f ;

	if( !PStr->binf )
	{
		// 時間値を取得
		TempDword( PStr, Model, ( X_DWORD * )&TimedFloatKeys->time ) ;	if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;

		// FloatKeys を取得
		TempFloatKeys( PStr, Model, &TimedFloatKeys->tfkeys ) ;			if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;
	}
	else
	{
		// 時間値を取得
		TimedFloatKeys->time = PSTRDWORD(PStr,6) ;

		X_FLOATKEYS *FloatKeys = &TimedFloatKeys->tfkeys ;

		// float 値の数を取得
		FloatKeys->nValues = PSTRDWORD(PStr,10) ;

		// float 値を格納するメモリの確保
		if( ( FloatKeys->values = ( float * )ADDMEMAREA( sizeof( float ) * FloatKeys->nValues, &Model->XModelMem ) ) == NULL )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\x6e\x6d\xd5\x52\x0f\x5c\x70\x65\xb9\x70\xad\x30\xfc\x30\xc7\x30\xfc\x30\xbf\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : 浮動小数点キーデータを格納するメモリ領域の確保に失敗しました\n" @*/ )) ;
			return -1 ;
		}

		PStrMoveOneB(PStr) ;

		if( PStr->binf )
		{
			// float 値の数だけ繰り返す
			if( FloatKeys->nValues != 0 )
			{
				f = FloatKeys->values ;
				for( i = 0 ; i < FloatKeys->nValues - 1 ; i ++, f ++ )
				{
					TempFloat( PStr, Model, ( X_FLOAT * )f ) ;
					if( !PStr->binf ) PStrMove( PStr, "," ) ;
				}
				TempFloat( PStr, Model, ( X_FLOAT * )f ) ;
				if( !PStr->binf ) PStrMove( PStr, ";," ) ;
			}
		}
		else
		{
			DWORD j ;

			// float 値の数だけ繰り返す
			j = 6 ;
			if( PStr->f64f )
			{
				f = FloatKeys->values ;
				for( i = 0 ; i < FloatKeys->nValues ; i ++, f ++, j += 8 )
					*f = PSTRDOUBLE(PStr,j) ;
			}
			else
			{
				f = FloatKeys->values ;
				for( i = 0 ; i < FloatKeys->nValues ; i ++, f ++, j += 4 )
					*f = PSTRFLOAT(PStr,j) ;
			}
		}
		PStrMoveOneB(PStr) ;
	}

	// 終了
	return 0 ;
}

// アニメーションキーのセットテンプレートの解析
static int TempAnimationKey( X_PSTRING *PStr, X_MODEL *Model, X_ANIMATIONKEY *AnimationKey )
{
	X_TIMEDFLOATKEYS *T ;
	DWORD i, j, k ;

	if( !PStr->binf )
	{
		// キータイプを取得
		TempDword( PStr, Model, ( X_DWORD * )&AnimationKey->keyType ) ;	if( !PStr->binf ) PStrMove( PStr, ";," ) ;
		
		// キーの数を取得
		TempDword( PStr, Model, ( X_DWORD * )&AnimationKey->nKeys ) ;		if( !PStr->binf ) PStrMove( PStr, ";," ) ;
	}
	else
	{
		// キータイプを取得
		AnimationKey->keyType = PSTRDWORD(PStr,6) ;
		
		// キーの数を取得
		AnimationKey->nKeys = PSTRDWORD(PStr,10) ;
	}
		
	// キーを保存するメモリ領域の確保
	if( ( AnimationKey->keys = ( X_TIMEDFLOATKEYS * )ADDMEMAREA( sizeof( X_TIMEDFLOATKEYS ) * AnimationKey->nKeys, &Model->XModelMem ) ) == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\xa2\x30\xcb\x30\xe1\x30\xfc\x30\xb7\x30\xe7\x30\xf3\x30\xad\x30\xfc\x30\xea\x30\xb9\x30\xc8\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : アニメーションキーリストを格納するメモリ領域の確保に失敗しました\n" @*/ )) ;
		return -1 ;
	}

	if( !PStr->binf )
	{
		// キーの数だけキーを読み込む
		if( AnimationKey->nKeys != 0 )
		{
			T = AnimationKey->keys ;
			for( i = 0 ; i < AnimationKey->nKeys - 1 ; i ++, T ++ )
			{
				TempTimedFloatKeys( PStr, Model, T ) ;
				if( !PStr->binf ) PStrMove( PStr, ";," ) ;
			}
			TempTimedFloatKeys( PStr, Model, T ) ;
			if( !PStr->binf ) PStrMove( PStr, ";," ) ;
		}
	}
	else
	{
		// キーの数だけキーを読み込む
		T = AnimationKey->keys ;
		j = 14 ;
		for( i = 0 ; i < AnimationKey->nKeys ; i ++, T ++ )
		{
			// TimedFloatKeys を取得
			{
				X_TIMEDFLOATKEYS *TimedFloatKeys = T ;

				// 時間値を取得
				TimedFloatKeys->time = PSTRDWORD(PStr,j) ; j += 4 ;

				// FloatKeys を取得
				{
					X_FLOATKEYS *FloatKeys = &TimedFloatKeys->tfkeys ;

					float *f ;

					// float 値の数を取得
					FloatKeys->nValues = PSTRDWORD(PStr,j) ; j += 4 ;

					PStrMoveOneB(PStr) ;
					j = 6 ;

					// float 値を格納するメモリの確保
					if( ( FloatKeys->values = ( float * )ADDMEMAREA( sizeof( float ) * FloatKeys->nValues, &Model->XModelMem ) ) == NULL )
					{
						DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\xa2\x30\xcb\x30\xe1\x30\xfc\x30\xb7\x30\xe7\x30\xf3\x30\xad\x30\xfc\x30\x6e\x30\x6e\x6d\xd5\x52\x0f\x5c\x70\x65\xb9\x70\x24\x50\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : アニメーションキーの浮動小数点値を格納するメモリ領域の確保に失敗しました\n" @*/ )) ;
						return -1 ;
					}

					// float 値の数だけ繰り返す
					if( !PStr->binf )
					{
						f = FloatKeys->values ;
						for( k = 0 ; k < FloatKeys->nValues ; k ++, f ++, j += 8 )
							*f = PSTRDOUBLE(PStr,j) ;
					}
					else
					{
						f = FloatKeys->values ;
						for( k = 0 ; k < FloatKeys->nValues ; k ++, f ++, j += 4 )
							*f = PSTRFLOAT(PStr,j) ;
					}
				}
			}
			PStrMoveOneB(PStr) ;
			j = 6 ;
		}
	}

	// 終了
	return 0 ;
}

// アニメーションオプションテンプレートの解析
static int TempAnimationOptions( X_PSTRING *PStr, X_MODEL *Model, X_ANIMATIONOPTIONS *AnimationOptions )
{
	if( !PStr->binf )
	{
		// アニメーションが閉じているかどうかを取得する
		TempDword( PStr, Model, ( X_DWORD * )&AnimationOptions->openclosed ) ;		if( !PStr->binf ) PStrMove( PStr, ";," ) ;
		
		// 位置キーの位置の品質値を取得する
		TempDword( PStr, Model, ( X_DWORD * )&AnimationOptions->positionquality ) ;	if( !PStr->binf ) PStrMove( PStr, ";," ) ;
	}
	else
	{
		// アニメーションが閉じているかどうかを取得する
		AnimationOptions->openclosed = PSTRDWORD(PStr,6) ;
		
		// 位置キーの位置の品質値を取得する
		AnimationOptions->positionquality = PSTRDWORD(PStr,10) ;

		PStrMoveOneB(PStr) ;
	}

	// 終了
	return 0 ;
}

// アニメーションテンプレートの解析
static int TempAnimation( X_PSTRING * /*PStr*/, X_MODEL * /*Model*/, X_ANIMATION * /*Animation*/ )
{
	// 特にすることなし

	// 終了
	return 0 ;
}

// アニメーションセットテンプレートの解析
static int TempAnimationSet( X_PSTRING * /*PStr*/, X_MODEL * /*Model*/, X_ANIMATIONSET * /*AnimationSet*/ )
{
	// 特にすることなし

	// 終了
	return 0 ;
}

// スキニングメッシュのヘッダ情報テンプレートの解析
static int TempXSkinMeshHeader( X_PSTRING *PStr, X_MODEL *Model, X_XSKINMESHHEADER *XSkinMeshHeader )
{
	if( !PStr->binf )
	{
		// ボーン一つに対して最大いくつの頂点が関わるかの数を取得
		TempWord( PStr, Model, ( X_WORD * )&XSkinMeshHeader->nMaxSkinWeightsPerVertex ) ;	if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;

		// ボーン一つに対して最大いくつの面が関わるかの数を取得
		TempWord( PStr, Model, ( X_WORD * )&XSkinMeshHeader->nMaxSkinWeightsPerFace ) ;	if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;

		// ボーンの数を取得
		TempWord( PStr, Model, ( X_WORD * )&XSkinMeshHeader->nBones ) ;					if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;
	}
	else
	{
		if( PSTRWORD(PStr,0) != TOKEN_INTEGER_LIST )
		{
			DXST_LOGFILEFMT_ADDW(( L"Load XFile : XSkinMeshHeader IntergerList\n" )) ;
			return -1 ;
		}
		if( PSTRDWORD(PStr,2) != 3 )
		{
			DXST_LOGFILEFMT_ADDA(( "Load XFile : XSkinMeshHeader Not 3 ( %d )\n", PSTRDWORD(PStr,2) ) ) ;
			return -1 ;
		}

		// ボーン一つに対して最大いくつの頂点が関わるかの数を取得
		XSkinMeshHeader->nMaxSkinWeightsPerVertex = ( WORD )PSTRDWORD(PStr,6) ;

		// ボーン一つに対して最大いくつの面が関わるかの数を取得
		XSkinMeshHeader->nMaxSkinWeightsPerFace = ( WORD )PSTRDWORD(PStr,10) ;

		// ボーンの数を取得
		XSkinMeshHeader->nBones = ( WORD )PSTRDWORD(PStr,14) ;

		PStrMoveOneB( PStr ) ;
	}

	// 終了
	return 0 ;
}

// スキンメッシュのウエイト情報テンプレートの解析
static int TempSkinWeights( X_PSTRING *PStr, X_MODEL *Model, X_SKINWEIGHTS *SkinWeights )
{
	DWORD i ;
	DWORD *D ;
	float *F ;

	// 影響を受けるボーン名を取得する
	TempString( PStr, Model, &SkinWeights->transformNodeName ) ;
	if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;
	
	// ウエイトを受ける頂点の数を取得
	if( !PStr->binf )
	{
		TempDword( PStr, Model, ( X_DWORD * )&SkinWeights->nWeights ) ;
		if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;
	}
	else
	{
		SkinWeights->nWeights = PSTRDWORD(PStr,6) ;
	}

	// 頂点のインデックスを格納するメモリ領域とウエイト値を格納するメモリ領域を確保
	if( ( SkinWeights->vertexIndices = ( DWORD * )ADDMEMAREA( sizeof( DWORD ) * ( SkinWeights->nWeights + 1 ), &Model->XModelMem ) ) == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\xb9\x30\xad\x30\xf3\x30\xe1\x30\xc3\x30\xb7\x30\xe5\x30\x6e\x30\xa6\x30\xa8\x30\xa4\x30\xc8\x30\x24\x50\xc5\x60\x31\x58\x6e\x30\xbf\x30\xfc\x30\xb2\x30\xc3\x30\xc8\x30\x02\x98\xb9\x70\xa4\x30\xf3\x30\xc7\x30\xc3\x30\xaf\x30\xb9\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\x5f\x30\x81\x30\x6e\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : スキンメッシュのウエイト値情報のターゲット頂点インデックスを格納するためのメモリ領域の確保に失敗しました\n" @*/ )) ;
		return -1 ;
	}

	if( ( SkinWeights->weights = ( float * )ADDMEMAREA( sizeof( float ) * ( SkinWeights->nWeights + 1 ), &Model->XModelMem ) ) == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x58\x00\x46\x00\x69\x00\x6c\x00\x65\x00\x20\x00\x3a\x00\x20\x00\xb9\x30\xad\x30\xf3\x30\xe1\x30\xc3\x30\xb7\x30\xe5\x30\x6e\x30\xa6\x30\xa8\x30\xa4\x30\xc8\x30\x24\x50\xc5\x60\x31\x58\x6e\x30\xa6\x30\xa8\x30\xa4\x30\xc8\x30\x24\x50\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\x5f\x30\x81\x30\x6e\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Load XFile : スキンメッシュのウエイト値情報のウエイト値を格納するためのメモリ領域の確保に失敗しました\n" @*/ )) ;
		return -1 ;
	}

	if( !PStr->binf )
	{
		// ウエイトを受ける頂点の数だけ頂点のインデックス値を得る
		if( SkinWeights->nWeights > 0 )
		{
			D = SkinWeights->vertexIndices ;
			for( i = 0 ; i < SkinWeights->nWeights - 1 ; i ++, D ++ )
			{
				TempDword( PStr, Model, ( X_DWORD * )D ) ;
				if( !PStr->binf ) PStrMoveP( PStr, "," ) ;
			}
			TempDword( PStr, Model, ( X_DWORD * )D ) ;
			if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;
		}

		// ウエイトを受ける頂点の数だけ頂点のウエイト値を得る
		if( SkinWeights->nWeights > 0 )
		{
			F = SkinWeights->weights ;
			for( i = 0 ; i < SkinWeights->nWeights - 1 ; i ++, F ++ )
			{
				TempFloat( PStr, Model, ( X_FLOAT * )F ) ;
				if( !PStr->binf ) PStrMoveP( PStr, "," ) ;
			}
			TempFloat( PStr, Model, ( X_FLOAT * )F ) ;
			if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;
		}

		// オフセット行列を取得する
		TempMatrix4x4( PStr, Model, &SkinWeights->matrixOffset ) ; if( !PStr->binf ) PStrMoveP( PStr, ";," ) ;
	}
	else
	{
		DWORD j, k ;
		X_MATRIX4X4 *mt ;

		if( PSTRWORD(PStr,0) != TOKEN_INTEGER_LIST )
		{
			DXST_LOGFILEFMT_ADDW(( L"Load XFile : SkinWeights TargetVertex IntegerList\n" )) ;
			return -1 ;
		}
		if( PSTRWORD(PStr,2) != ( DWORD )( 1 + SkinWeights->nWeights ) )
		{
			DXST_LOGFILEFMT_ADDW(( L"Load XFile : SkinWeights TargetVertex Not %d ( %d )\n", 1 + SkinWeights->nWeights, PSTRDWORD(PStr,2) ) ) ;
			return -1 ;
		}

		k = 10 ;
		// ウエイトを受ける頂点の数だけ頂点のインデックス値を得る
		D = SkinWeights->vertexIndices ;
		for( i = 0 ; i < SkinWeights->nWeights ; i ++, D ++, k += 4 )
			*D = PSTRDWORD(PStr,k) ;

		PStrMoveOneB( PStr ) ;

		if( PSTRWORD(PStr,0) != TOKEN_FLOAT_LIST )
		{
			DXST_LOGFILEFMT_ADDW(( L"Load XFile : SkinWeights WeightValue FloatList\n" )) ;
			return -1 ;
		}

		if( PSTRWORD(PStr,2) != ( DWORD )( 16 + SkinWeights->nWeights ) )
		{
			DXST_LOGFILEFMT_ADDW(( L"Load XFile : SkinWeights WeightValue Not %d ( %d )\n", 16 + SkinWeights->nWeights, PSTRDWORD(PStr,2) ) ) ;
			return -1 ;
		}

		k = 6 ;
		if( PStr->f64f )
		{
			// ウエイトを受ける頂点の数だけ頂点のウエイト値を得る
			F = SkinWeights->weights ;
			for( i = 0 ; i < SkinWeights->nWeights ; i ++, F ++, k += 8 )
				*F = PSTRDOUBLE(PStr,k) ;

			// オフセット行列を取得する
			mt = &SkinWeights->matrixOffset ;
			for( i = 0 ; i < 4 ; i ++ )
				for( j = 0 ; j < 4 ; j ++, k += 8 )
					mt->matrix[i][j] = PSTRDOUBLE(PStr,k) ;
		}
		else
		{
			// ウエイトを受ける頂点の数だけ頂点のウエイト値を得る
			F = SkinWeights->weights ;
			for( i = 0 ; i < SkinWeights->nWeights ; i ++, F ++, k += 4 )
				*F = PSTRFLOAT(PStr,k) ;

			// オフセット行列を取得する
			mt = &SkinWeights->matrixOffset ;
			for( i = 0 ; i < 4 ; i ++ )
				for( j = 0 ; j < 4 ; j ++, k += 4 )
					mt->matrix[i][j] = PSTRFLOAT(PStr,k) ;
		}

		PStrMoveOneB( PStr ) ;
	}

	// 終了
	return 0 ;
}














// Ｘファイルを読み込む( -1:エラー  0以上:モデル基データハンドル )
extern int MV1LoadModelToX( const MV1_MODEL_LOAD_PARAM *LoadParam, int ASyncThread )
{
	X_MODEL XModel ;
	int NewHandle = -1 ;
	MV1_MODEL_R RModel ;

	// 読み込み用モデルの初期化
	MV1InitReadModel( &RModel ) ;

	// モデル名とファイル名と文字コード形式をセット
	RModel.CharCodeFormat = CHAR_CHARCODEFORMAT ;
	RModel.FilePath = ( wchar_t * )DXALLOC( ( _WCSLEN( LoadParam->FilePath ) + 1 ) * sizeof( wchar_t ) ) ;
	RModel.Name     = ( wchar_t * )DXALLOC( ( _WCSLEN( LoadParam->Name     ) + 1 ) * sizeof( wchar_t ) ) ;
	_WCSCPY( RModel.FilePath, LoadParam->FilePath ) ;
	_WCSCPY( RModel.Name,     LoadParam->Name ) ;

	// Ｘファイルデータの読み込み
	{
		// Ｘファイル解析用データの初期化
		_MEMSET( &XModel, 0, sizeof( XModel ) ) ;

		// データ構造の最上級データを追加する
		AddObject( TEMP_FRAME, "XTopFrame", NULL, &XModel ) ;

		// Ｘファイルの解析
		if( AnalysXFile( ( char * )LoadParam->DataBuffer, LoadParam->DataSize, &XModel ) < 0 ) goto ERRORLABEL ;

		// Ｘファイルの解析、２パス目みたいなもの
		if( AnalysXData( &XModel, &RModel ) < 0 ) goto ERRORLABEL ;
	}

	// Xファイルのデータを解放
	TerminateXModel( &XModel ) ;

	// 読み込みモデルから基本モデルデータを作成する
	NewHandle = MV1LoadModelToReadModel( &LoadParam->GParam, &RModel, LoadParam->CurrentDir, LoadParam->FileReadFunc, ASyncThread ) ;
	if( NewHandle == -1 ) goto ERRORLABEL ;

	// 読み込みモデルの後始末
	MV1TermReadModel( &RModel ) ;

	// ハンドルを返す
	return NewHandle ;

ERRORLABEL :

	// 読み込みモデルの後始末
	MV1TermReadModel( &RModel ) ;

	// 基本モデルデータの解放
	if( NewHandle != -1 )
	{
		MV1SubModelBase( NewHandle ) ;
		NewHandle = -1 ;
	}

	// Xファイルのデータを解放
	TerminateXModel( &XModel ) ;

	// エラー終了
	return -1 ;
}

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif

