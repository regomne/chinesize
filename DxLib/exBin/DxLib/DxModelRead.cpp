// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		モデルデータ読み込みプログラム
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#define __DX_MAKE

#include "DxModelRead.h"

#ifndef DX_NON_MODEL

// インクルード ---------------------------------
#include "DxLib.h"
#include "DxStatic.h"
#include "DxFile.h"
#include "DxModel.h"
#include "DxMemory.h"
#include "DxMath.h"
#include "DxLog.h"
#include "DxSystem.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 -----------------------------------

// モデル内リストに追加
#define MODELLIST_ADD( model, name )\
	if( model->name##First == NULL )\
	{\
		model->name##First = name ;\
		model->name##Last  = name ;\
	}\
	else\
	{\
		name->DataPrev = model->name##Last ;\
		model->name##Last->DataNext = name ;\
		model->name##Last = name;\
	}\
	name->Index = ( int )model->name##Num ;\
	model->name##Num ++ ;

// オブジェクト内リストに追加
#define OBJLIST_ADD( obj, name )\
	if( obj->name##First == NULL )\
	{\
		obj->name##First = name ;\
		obj->name##Last  = name ;\
	}\
	else\
	{\
		name->Prev = obj->name##Last ;\
		obj->name##Last->Next = name ;\
		obj->name##Last = name;\
	}\
	obj->name##Num ++ ;

// アニメーションキーセットのリンクを外す
#define KEYSETLIST_SUB( keyset, motion, readmodel )\
	if( keyset->Next )     keyset->Next->Prev         = keyset->Prev ;\
	else                   motion->AnimKeySetLast     = keyset->Prev ;\
\
	if( keyset->Prev )     keyset->Prev->Next         = keyset->Next ;\
	else                   motion->AnimKeySetFirst    = keyset->Next ;\
\
	if( keyset->DataNext ) keyset->DataNext->DataPrev = keyset->DataPrev ;\
	else                   readmodel->AnimKeySetLast  = keyset->DataPrev ;\
\
	if( keyset->DataPrev ) keyset->DataPrev->DataNext = keyset->DataNext ;\
	else                   readmodel->AnimKeySetFirst = keyset->DataNext ;\
\
	motion->AnimKeySetNum -- ;\
	readmodel->AnimKeySetNum -- ;


// データ型宣言 ---------------------------------

// テスト位置情報のウエイト情報
struct MV1_TESTPOSITIONINFO_SKINWEIGHT
{
	MV1_SKIN_WEIGHT_R *Orig ;
	int BoneNumber ;
	float Weight ;
} ;


// データ定義 -----------------------------------

// 8x8 の tgaテクスチャ
BYTE Tga8x8TextureFileImage[ 84 ] = 
{
	0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x08, 0x00,
	0x20, 0x08, 0x87, 0xFF, 0xFF, 0xFF, 0xFF, 0x87, 0xFF, 0xFF, 0xFF, 0xFF, 0x87, 0xFF, 0xFF, 0xFF,
	0xFF, 0x87, 0xFF, 0xFF, 0xFF, 0xFF, 0x87, 0xFF, 0xFF, 0xFF, 0xFF, 0x87, 0xFF, 0xFF, 0xFF, 0xFF,
	0x87, 0xFF, 0xFF, 0xFF, 0xFF, 0x87, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x54, 0x52, 0x55, 0x45, 0x56, 0x49, 0x53, 0x49, 0x4F, 0x4E, 0x2D, 0x58, 0x46, 0x49,
	0x4C, 0x45, 0x2E, 0x00
} ;

BYTE Tga8x8BlackTextureFileImage[ 76 ] =
{
	0x00,0x00,0x0a,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x08,0x00,
	0x18,0x00,0x87,0x00,0x00,0x00,0x87,0x00,0x00,0x00,0x87,0x00,0x00,0x00,0x87,0x00,
	0x00,0x00,0x87,0x00,0x00,0x00,0x87,0x00,0x00,0x00,0x87,0x00,0x00,0x00,0x87,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x54,0x52,0x55,0x45,0x56,0x49,
	0x53,0x49,0x4f,0x4e,0x2d,0x58,0x46,0x49,0x4c,0x45,0x2e,0x00,
} ;

// 256x8 の仮トゥーンレンダリング用 tgaテクスチャ
BYTE Tga256x8ToonTextureFileImage[ 172 ] =
{
	0x00,0x00,0x0a,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x08,0x00,
	0x18,0x00,0xbf,0x00,0x00,0x00,0xbf,0x55,0x55,0x55,0xbf,0xaa,0xaa,0xaa,0xbf,0xff,
	0xff,0xff,0xbf,0x00,0x00,0x00,0xbf,0x55,0x55,0x55,0xbf,0xaa,0xaa,0xaa,0xbf,0xff,
	0xff,0xff,0xbf,0x00,0x00,0x00,0xbf,0x55,0x55,0x55,0xbf,0xaa,0xaa,0xaa,0xbf,0xff,
	0xff,0xff,0xbf,0x00,0x00,0x00,0xbf,0x55,0x55,0x55,0xbf,0xaa,0xaa,0xaa,0xbf,0xff,
	0xff,0xff,0xbf,0x00,0x00,0x00,0xbf,0x55,0x55,0x55,0xbf,0xaa,0xaa,0xaa,0xbf,0xff,
	0xff,0xff,0xbf,0x00,0x00,0x00,0xbf,0x55,0x55,0x55,0xbf,0xaa,0xaa,0xaa,0xbf,0xff,
	0xff,0xff,0xbf,0x00,0x00,0x00,0xbf,0x55,0x55,0x55,0xbf,0xaa,0xaa,0xaa,0xbf,0xff,
	0xff,0xff,0xbf,0x00,0x00,0x00,0xbf,0x55,0x55,0x55,0xbf,0xaa,0xaa,0xaa,0xbf,0xff,
	0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x54,0x52,0x55,0x45,0x56,0x49,
	0x53,0x49,0x4f,0x4e,0x2d,0x58,0x46,0x49,0x4c,0x45,0x2e,0x00
} ;

// デフォルトのディフューズグラデーション
BYTE TgaDiffuseDefaultGradFileImage[ 172/*156*/ ] =
{
	0x00,0x00,0x0a,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x08,0x00,
	0x18,0x00,0xbf,0x00,0x00,0x00,0xbf,0x55,0x55,0x55,0xbf,0xaa,0xaa,0xaa,0xbf,0xff,
	0xff,0xff,0xbf,0x00,0x00,0x00,0xbf,0x55,0x55,0x55,0xbf,0xaa,0xaa,0xaa,0xbf,0xff,
	0xff,0xff,0xbf,0x00,0x00,0x00,0xbf,0x55,0x55,0x55,0xbf,0xaa,0xaa,0xaa,0xbf,0xff,
	0xff,0xff,0xbf,0x00,0x00,0x00,0xbf,0x55,0x55,0x55,0xbf,0xaa,0xaa,0xaa,0xbf,0xff,
	0xff,0xff,0xbf,0x00,0x00,0x00,0xbf,0x55,0x55,0x55,0xbf,0xaa,0xaa,0xaa,0xbf,0xff,
	0xff,0xff,0xbf,0x00,0x00,0x00,0xbf,0x55,0x55,0x55,0xbf,0xaa,0xaa,0xaa,0xbf,0xff,
	0xff,0xff,0xbf,0x00,0x00,0x00,0xbf,0x55,0x55,0x55,0xbf,0xaa,0xaa,0xaa,0xbf,0xff,
	0xff,0xff,0xbf,0x00,0x00,0x00,0xbf,0x55,0x55,0x55,0xbf,0xaa,0xaa,0xaa,0xbf,0xff,
	0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x54,0x52,0x55,0x45,0x56,0x49,
	0x53,0x49,0x4f,0x4e,0x2d,0x58,0x46,0x49,0x4c,0x45,0x2e,0x00
/*
	0x00,0x01,0x09,0x00,0x00,0x10,0x00,0x18,0x00,0x00,0x00,0x00,0x40,0x00,0x08,0x00,
	0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x80,0x00,0x00,0x80,0x80,0x80,0x00,
	0x00,0x80,0x00,0x80,0x80,0x80,0x00,0x80,0x80,0x80,0xc0,0xc0,0xc0,0x00,0x00,0xff,
	0x00,0xff,0x00,0x00,0xff,0xff,0xff,0x00,0x00,0xff,0x00,0xff,0xff,0xff,0x00,0xff,
	0xff,0xff,0x87,0x00,0x8c,0x07,0x96,0x08,0x93,0x0f,0x87,0x00,0x8c,0x07,0x96,0x08,
	0x93,0x0f,0x87,0x00,0x8c,0x07,0x96,0x08,0x93,0x0f,0x87,0x00,0x8c,0x07,0x96,0x08,
	0x93,0x0f,0x87,0x00,0x8c,0x07,0x96,0x08,0x93,0x0f,0x87,0x00,0x8c,0x07,0x96,0x08,
	0x93,0x0f,0x87,0x00,0x8c,0x07,0x96,0x08,0x93,0x0f,0x87,0x00,0x8c,0x07,0x96,0x08,
	0x93,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x54,0x52,0x55,0x45,0x56,0x49,
	0x53,0x49,0x4f,0x4e,0x2d,0x58,0x46,0x49,0x4c,0x45,0x2e,0x00,
*/
} ;

// デフォルトのスペキュラグラデーション
BYTE TgaSpecularDefaultGradFileImage[ 172/*124*/ ] =
{
	0x00,0x00,0x0a,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x08,0x00,
	0x18,0x00,0xbf,0x00,0x00,0x00,0xbf,0x55,0x55,0x55,0xbf,0xaa,0xaa,0xaa,0xbf,0xff,
	0xff,0xff,0xbf,0x00,0x00,0x00,0xbf,0x55,0x55,0x55,0xbf,0xaa,0xaa,0xaa,0xbf,0xff,
	0xff,0xff,0xbf,0x00,0x00,0x00,0xbf,0x55,0x55,0x55,0xbf,0xaa,0xaa,0xaa,0xbf,0xff,
	0xff,0xff,0xbf,0x00,0x00,0x00,0xbf,0x55,0x55,0x55,0xbf,0xaa,0xaa,0xaa,0xbf,0xff,
	0xff,0xff,0xbf,0x00,0x00,0x00,0xbf,0x55,0x55,0x55,0xbf,0xaa,0xaa,0xaa,0xbf,0xff,
	0xff,0xff,0xbf,0x00,0x00,0x00,0xbf,0x55,0x55,0x55,0xbf,0xaa,0xaa,0xaa,0xbf,0xff,
	0xff,0xff,0xbf,0x00,0x00,0x00,0xbf,0x55,0x55,0x55,0xbf,0xaa,0xaa,0xaa,0xbf,0xff,
	0xff,0xff,0xbf,0x00,0x00,0x00,0xbf,0x55,0x55,0x55,0xbf,0xaa,0xaa,0xaa,0xbf,0xff,
	0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x54,0x52,0x55,0x45,0x56,0x49,
	0x53,0x49,0x4f,0x4e,0x2d,0x58,0x46,0x49,0x4c,0x45,0x2e,0x00
/*
	0x00,0x01,0x09,0x00,0x00,0x10,0x00,0x18,0x00,0x00,0x00,0x00,0x40,0x00,0x08,0x00,
	0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x80,0x00,0x00,0x80,0x80,0x80,0x00,
	0x00,0x80,0x00,0x80,0x80,0x80,0x00,0x80,0x80,0x80,0xc0,0xc0,0xc0,0x00,0x00,0xff,
	0x00,0xff,0x00,0x00,0xff,0xff,0xff,0x00,0x00,0xff,0x00,0xff,0xff,0xff,0x00,0xff,
	0xff,0xff,0x9f,0x00,0x9f,0x0f,0x9f,0x00,0x9f,0x0f,0x9f,0x00,0x9f,0x0f,0x9f,0x00,
	0x9f,0x0f,0x9f,0x00,0x9f,0x0f,0x9f,0x00,0x9f,0x0f,0x9f,0x00,0x9f,0x0f,0x9f,0x00,
	0x9f,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x54,0x52,0x55,0x45,0x56,0x49,
	0x53,0x49,0x4f,0x4e,0x2d,0x58,0x46,0x49,0x4c,0x45,0x2e,0x00,
*/
} ;

// 関数宣言 -------------------------------------

#ifndef UNICODE
static char    *MV1RGetStringSpace(  MV1_MODEL_BASE *MBase, const char    *String ) ;					// モデル基本データ用の文字列スペースを確保する( -1:エラー )
#endif
static wchar_t *MV1RGetStringSpaceW( MV1_MODEL_BASE *MBase, const wchar_t *String ) ;					// モデル基本データ用の文字列スペースを確保する( -1:エラー )
static bool MV1ConvertTrianglePolygon( MV1_MODEL_R *ReadModel, MV1_MESH_R *Mesh ) ;						// 多角形ポリゴンを三角形ポリゴンに変換する
static bool MV1NotEqualNormalSideAddZeroAreaPolygon( MV1_MODEL_R *ReadModel, MV1_MESH_R *Mesh ) ;		// 辺を共有していて、且つ法線の方向が異なる辺に面積０のポリゴンを追加する( ３角形ポリゴンの集合になっている必要あり )
static bool MV1OptimizePosition( MV1_MODEL_R *ReadModel, MV1_MESH_R *Mesh ) ;							// 座標、スキニングウエイト値が全く同一の頂点を統合する
static bool MV1MakeMeshNormals( MV1_MODEL_R *ReadModel, MV1_MESH_R *Mesh ) ;							// メッシュ法線の自動計算( 全ポリゴンが三角形ポリゴンである必要があります )
static bool MV1MakeMeshBinormalsAndTangents( MV1_MODEL_R *ReadModel, MV1_MESH_R *Mesh ) ;				// メッシュの従法線と接線を計算する( 全ポリゴンが三角形ポリゴンである必要があります )
static bool MV1OptimizeSkinBoneInfo( MV1_MODEL_BASE *Model ) ;											// モデルデータのスキニングメッシュ用のボーン情報を最適化する
static bool MV1OptimizeAnim( MV1_MODEL_R *ReadModel ) ;													// アニメーションデータを最適化する
static void MV1OptimizeKeySetTimeType( MV1_MODEL_R *ReadModel, MV1_ANIMKEYSET_R *KeySetR, bool NotKeySub = false ) ;	// キーセットのキーを最適化する
static bool MatrixRotateCmp( MATRIX *In1, MATRIX *In2, float f ) ;
static void MV1MultiplyRotateMatrix( MATRIX *DestBuffer, VECTOR *Rotate, int RotateOrder, bool RotateInverse ) ;			// 回転行列を乗算する

// プログラム -----------------------------------

static bool MatrixRotateCmp( MATRIX *In1, MATRIX *In2, float f )
{
	return In1->m[ 0 ][ 0 ] > In2->m[ 0 ][ 0 ] - f &&
			In1->m[ 0 ][ 0 ] < In2->m[ 0 ][ 0 ] + f &&
			In1->m[ 0 ][ 1 ] > In2->m[ 0 ][ 1 ] - f &&
			In1->m[ 0 ][ 1 ] < In2->m[ 0 ][ 1 ] + f &&
			In1->m[ 0 ][ 2 ] > In2->m[ 0 ][ 2 ] - f &&
			In1->m[ 0 ][ 2 ] < In2->m[ 0 ][ 2 ] + f &&
			In1->m[ 1 ][ 0 ] > In2->m[ 1 ][ 0 ] - f &&
			In1->m[ 1 ][ 0 ] < In2->m[ 1 ][ 0 ] + f &&
			In1->m[ 1 ][ 1 ] > In2->m[ 1 ][ 1 ] - f &&
			In1->m[ 1 ][ 1 ] < In2->m[ 1 ][ 1 ] + f &&
			In1->m[ 1 ][ 2 ] > In2->m[ 1 ][ 2 ] - f &&
			In1->m[ 1 ][ 2 ] < In2->m[ 1 ][ 2 ] + f &&
			In1->m[ 2 ][ 0 ] > In2->m[ 2 ][ 0 ] - f &&
			In1->m[ 2 ][ 0 ] < In2->m[ 2 ][ 0 ] + f &&
			In1->m[ 2 ][ 1 ] > In2->m[ 2 ][ 1 ] - f &&
			In1->m[ 2 ][ 1 ] < In2->m[ 2 ][ 1 ] + f &&
			In1->m[ 2 ][ 2 ] > In2->m[ 2 ][ 2 ] - f &&
			In1->m[ 2 ][ 2 ] < In2->m[ 2 ][ 2 ] + f ;
}

// 多角形ポリゴンを三角形ポリゴンに変換する
static bool MV1ConvertTrianglePolygon( MV1_MODEL_R *ReadModel, MV1_MESH_R *Mesh )
{
	int AddFaceCount, AddNum, i, j, k, p1, p2, pt ;
	MV1_MESHFACE_R *NewFace, *FaceT, *NewFaceT, *NewFaceT2 ;
	DWORD *NewIndex, *NewIndexT ;
	DWORD NewFaceNum ;

	// 増やさなければならない面データの数を調べる
	AddFaceCount = 0 ;
	FaceT = Mesh->Faces ;
	for( i = 0 ; ( DWORD )i < Mesh->FaceNum ; i ++, FaceT ++ )
	{
		AddFaceCount += FaceT->IndexNum - 3 ;

		// 両面ポリゴンの場合はポリゴン数も倍になる
		if( Mesh->Materials[ FaceT->MaterialIndex ]->TwoSide )
		{
			AddFaceCount += FaceT->IndexNum - 2 ;
		}
	}

	// 多角形面が無い場合はここで終了
	if( AddFaceCount == 0 )
		return true ;

	// 増やした面を格納するメモリ領域の確保
	NewFaceNum = Mesh->FaceNum + AddFaceCount ;
	NewFace = ( MV1_MESHFACE_R * )ADDMEMAREA( ( sizeof( MV1_MESHFACE_R ) + ( 3 + MV1_READ_MAX_UV_NUM ) * 3 * sizeof( DWORD ) ) * NewFaceNum, &ReadModel->Mem ) ;
	if( NewFace == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x52\x00\x65\x00\x61\x00\x64\x00\x20\x00\x4d\x00\x6f\x00\x64\x00\x65\x00\x6c\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x13\xff\xdd\x30\xea\x30\x09\x59\xdb\x63\x8c\x5f\x6e\x30\xc7\x30\xfc\x30\xbf\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Read Model Error : ３ポリ変換後のデータを格納するメモリ領域の確保に失敗しました\n" @*/ )) ;
		return false ;
	}
	NewIndex = ( DWORD * )( NewFace + NewFaceNum ) ;

	// インデックスデータのアドレスをセットアップ
	NewFaceT = NewFace ;
	NewIndexT = NewIndex ;
	for( i = 0 ; i < ( int )NewFaceNum ; i ++, NewFaceT ++ )
	{
		NewFaceT->VertexIndex = NewIndexT ; NewIndexT += 3 ;
		NewFaceT->NormalIndex = NewIndexT ; NewIndexT += 3 ;
		NewFaceT->VertexColorIndex = NewIndexT ; NewIndexT += 3 ;
		for( j = 0 ; j < MV1_READ_MAX_UV_NUM ; j ++, NewIndexT += 3 )
			NewFaceT->UVIndex[ j ] = NewIndexT ;
	}

	// 今までの面の情報をコピー
	FaceT = Mesh->Faces ;
	NewFaceT2 = NewFace + Mesh->FaceNum ;
	NewFaceT = NewFace ;
	for( i = 0 ; i < ( int )Mesh->FaceNum ; i ++, FaceT ++, NewFaceT ++ )
	{
		NewFaceT->IndexNum = 3 ;
		NewFaceT->PolygonNum = 1 ;

		for( j = 0 ; j < 3 ; j ++ )
		{
			NewFaceT->VertexIndex[ j ] = FaceT->VertexIndex[ j ] ;
			NewFaceT->NormalIndex[ j ] = FaceT->NormalIndex[ j ] ;
			NewFaceT->VertexColorIndex[ j ] = FaceT->VertexColorIndex[ j ] ;
			for( k = 0 ; k < MV1_READ_MAX_UV_NUM ; k ++ )
			{
				NewFaceT->UVIndex[ k ][ j ] = FaceT->UVIndex[ k ][ j ] ;
			}
		}

		NewFaceT->MaterialIndex = FaceT->MaterialIndex ;
		NewFaceT->Normal = FaceT->Normal ;

		// 両面ポリゴンの場合はポリゴン数も倍になる
		if( Mesh->Materials[ FaceT->MaterialIndex ]->TwoSide )
		{
			NewFaceT2->IndexNum = 3 ;
			NewFaceT2->PolygonNum = 1 ;
			NewFaceT2->VertexIndex[ 0 ] = FaceT->VertexIndex[ 2 ] ;
			NewFaceT2->VertexIndex[ 1 ] = FaceT->VertexIndex[ 1 ] ;
			NewFaceT2->VertexIndex[ 2 ] = FaceT->VertexIndex[ 0 ] ;
			NewFaceT2->NormalIndex[ 0 ] = FaceT->NormalIndex[ 2 ] ;
			NewFaceT2->NormalIndex[ 1 ] = FaceT->NormalIndex[ 1 ] ;
			NewFaceT2->NormalIndex[ 2 ] = FaceT->NormalIndex[ 0 ] ;
			NewFaceT2->VertexColorIndex[ 0 ] = FaceT->VertexColorIndex[ 2 ] ;
			NewFaceT2->VertexColorIndex[ 1 ] = FaceT->VertexColorIndex[ 1 ] ;
			NewFaceT2->VertexColorIndex[ 2 ] = FaceT->VertexColorIndex[ 0 ] ;
			for( k = 0 ; ( DWORD )k < MV1_READ_MAX_UV_NUM ; k ++ )
			{
				NewFaceT2->UVIndex[ k ][ 0 ] = FaceT->UVIndex[ k ][ 2 ] ;
				NewFaceT2->UVIndex[ k ][ 1 ] = FaceT->UVIndex[ k ][ 1 ] ;
				NewFaceT2->UVIndex[ k ][ 2 ] = FaceT->UVIndex[ k ][ 0 ] ;
			}
			NewFaceT2->MaterialIndex = FaceT->MaterialIndex ;
			NewFaceT2 ++ ;
		}
	}

	// 多角形面を三角形面に変換
	FaceT = Mesh->Faces ;
	NewFaceT = NewFace ;
	for( i = 0 ; ( DWORD )i < Mesh->FaceNum ; i ++, NewFaceT ++, FaceT ++ )
	{
		if( FaceT->IndexNum == 4 )
		{
			VECTOR OutVec[ 3 ] ;

			// 三角形( 0, 2, 3 ) の内側に頂点1があるかチェック
			OutVec[ 0 ] = VCross(
				VSub( Mesh->Positions[ FaceT->VertexIndex[ 0 ] ], Mesh->Positions[ FaceT->VertexIndex[ 2 ] ] ),
				VSub( Mesh->Positions[ FaceT->VertexIndex[ 2 ] ], Mesh->Positions[ FaceT->VertexIndex[ 1 ] ] )
			) ;
			OutVec[ 1 ] = VCross(
				VSub( Mesh->Positions[ FaceT->VertexIndex[ 2 ] ], Mesh->Positions[ FaceT->VertexIndex[ 3 ] ] ),
				VSub( Mesh->Positions[ FaceT->VertexIndex[ 3 ] ], Mesh->Positions[ FaceT->VertexIndex[ 1 ] ] )
			) ;
			OutVec[ 2 ] = VCross(
				VSub( Mesh->Positions[ FaceT->VertexIndex[ 3 ] ], Mesh->Positions[ FaceT->VertexIndex[ 0 ] ] ),
				VSub( Mesh->Positions[ FaceT->VertexIndex[ 0 ] ], Mesh->Positions[ FaceT->VertexIndex[ 1 ] ] )
			) ;
			if( VDot( OutVec[ 0 ], OutVec[ 1 ] ) > 0.0f &&
				VDot( OutVec[ 0 ], OutVec[ 2 ] ) > 0.0f )
			{
				// 三角形( 0, 2, 3 ) の内側に頂点1がある場合の処理
				NewFaceT2->IndexNum = 3 ;
				NewFaceT2->PolygonNum = 1 ;
				NewFaceT2->VertexIndex[ 0 ] = FaceT->VertexIndex[ 0 ] ;
				NewFaceT2->VertexIndex[ 1 ] = FaceT->VertexIndex[ 1 ] ;
				NewFaceT2->VertexIndex[ 2 ] = FaceT->VertexIndex[ 3 ] ;
				NewFaceT2->NormalIndex[ 0 ] = FaceT->NormalIndex[ 0 ] ;
				NewFaceT2->NormalIndex[ 1 ] = FaceT->NormalIndex[ 1 ] ;
				NewFaceT2->NormalIndex[ 2 ] = FaceT->NormalIndex[ 3 ] ;
				NewFaceT2->VertexColorIndex[ 0 ] = FaceT->VertexColorIndex[ 0 ] ;
				NewFaceT2->VertexColorIndex[ 1 ] = FaceT->VertexColorIndex[ 1 ] ;
				NewFaceT2->VertexColorIndex[ 2 ] = FaceT->VertexColorIndex[ 3 ] ;
				for( k = 0 ; ( DWORD )k < MV1_READ_MAX_UV_NUM ; k ++ )
				{
					NewFaceT2->UVIndex[ k ][ 0 ] = FaceT->UVIndex[ k ][ 0 ] ;
					NewFaceT2->UVIndex[ k ][ 1 ] = FaceT->UVIndex[ k ][ 1 ] ;
					NewFaceT2->UVIndex[ k ][ 2 ] = FaceT->UVIndex[ k ][ 3 ] ;
				}
				NewFaceT2->MaterialIndex = FaceT->MaterialIndex ;
				NewFaceT2 ++ ;

				NewFaceT->VertexIndex[ 0 ] = FaceT->VertexIndex[ 1 ] ;
				NewFaceT->VertexIndex[ 1 ] = FaceT->VertexIndex[ 2 ] ;
				NewFaceT->VertexIndex[ 2 ] = FaceT->VertexIndex[ 3 ] ;
				NewFaceT->NormalIndex[ 0 ] = FaceT->NormalIndex[ 1 ] ;
				NewFaceT->NormalIndex[ 1 ] = FaceT->NormalIndex[ 2 ] ;
				NewFaceT->NormalIndex[ 2 ] = FaceT->NormalIndex[ 3 ] ;
				NewFaceT->VertexColorIndex[ 0 ] = FaceT->VertexColorIndex[ 1 ] ;
				NewFaceT->VertexColorIndex[ 1 ] = FaceT->VertexColorIndex[ 2 ] ;
				NewFaceT->VertexColorIndex[ 2 ] = FaceT->VertexColorIndex[ 3 ] ;
				for( k = 0 ; ( DWORD )k < MV1_READ_MAX_UV_NUM ; k ++ )
				{
					NewFaceT->UVIndex[ k ][ 0 ] = FaceT->UVIndex[ k ][ 1 ] ;
					NewFaceT->UVIndex[ k ][ 1 ] = FaceT->UVIndex[ k ][ 2 ] ;
					NewFaceT->UVIndex[ k ][ 2 ] = FaceT->UVIndex[ k ][ 3 ] ;
				}

				// 両面ポリゴンの場合はポリゴン数も倍になる
				if( Mesh->Materials[ FaceT->MaterialIndex ]->TwoSide )
				{
					NewFaceT2->IndexNum = 3 ;
					NewFaceT2->PolygonNum = 1 ;
					NewFaceT2->VertexIndex[ 0 ] = FaceT->VertexIndex[ 3 ] ;
					NewFaceT2->VertexIndex[ 1 ] = FaceT->VertexIndex[ 1 ] ;
					NewFaceT2->VertexIndex[ 2 ] = FaceT->VertexIndex[ 0 ] ;
					NewFaceT2->NormalIndex[ 0 ] = FaceT->NormalIndex[ 3 ] ;
					NewFaceT2->NormalIndex[ 1 ] = FaceT->NormalIndex[ 1 ] ;
					NewFaceT2->NormalIndex[ 2 ] = FaceT->NormalIndex[ 0 ] ;
					NewFaceT2->VertexColorIndex[ 0 ] = FaceT->VertexColorIndex[ 3 ] ;
					NewFaceT2->VertexColorIndex[ 1 ] = FaceT->VertexColorIndex[ 1 ] ;
					NewFaceT2->VertexColorIndex[ 2 ] = FaceT->VertexColorIndex[ 0 ] ;
					for( k = 0 ; ( DWORD )k < MV1_READ_MAX_UV_NUM ; k ++ )
					{
						NewFaceT2->UVIndex[ k ][ 0 ] = FaceT->UVIndex[ k ][ 3 ] ;
						NewFaceT2->UVIndex[ k ][ 1 ] = FaceT->UVIndex[ k ][ 1 ] ;
						NewFaceT2->UVIndex[ k ][ 2 ] = FaceT->UVIndex[ k ][ 0 ] ;
					}
					NewFaceT2->MaterialIndex = FaceT->MaterialIndex ;
					NewFaceT2 ++ ;

					NewFaceT2->IndexNum = 3 ;
					NewFaceT2->PolygonNum = 1 ;
					NewFaceT2->VertexIndex[ 0 ] = FaceT->VertexIndex[ 3 ] ;
					NewFaceT2->VertexIndex[ 1 ] = FaceT->VertexIndex[ 2 ] ;
					NewFaceT2->VertexIndex[ 2 ] = FaceT->VertexIndex[ 1 ] ;
					NewFaceT2->NormalIndex[ 0 ] = FaceT->NormalIndex[ 3 ] ;
					NewFaceT2->NormalIndex[ 1 ] = FaceT->NormalIndex[ 2 ] ;
					NewFaceT2->NormalIndex[ 2 ] = FaceT->NormalIndex[ 1 ] ;
					NewFaceT2->VertexColorIndex[ 0 ] = FaceT->VertexColorIndex[ 3 ] ;
					NewFaceT2->VertexColorIndex[ 1 ] = FaceT->VertexColorIndex[ 2 ] ;
					NewFaceT2->VertexColorIndex[ 2 ] = FaceT->VertexColorIndex[ 1 ] ;
					for( k = 0 ; ( DWORD )k < MV1_READ_MAX_UV_NUM ; k ++ )
					{
						NewFaceT2->UVIndex[ k ][ 0 ] = FaceT->UVIndex[ k ][ 3 ] ;
						NewFaceT2->UVIndex[ k ][ 1 ] = FaceT->UVIndex[ k ][ 2 ] ;
						NewFaceT2->UVIndex[ k ][ 2 ] = FaceT->UVIndex[ k ][ 1 ] ;
					}
					NewFaceT2->MaterialIndex = FaceT->MaterialIndex ;
					NewFaceT2 ++ ;
				}

				continue ;
			}

			// 三角形( 0, 1, 2 ) の内側に頂点3があるかチェック
			OutVec[ 0 ] = VCross(
				VSub( Mesh->Positions[ FaceT->VertexIndex[ 0 ] ], Mesh->Positions[ FaceT->VertexIndex[ 1 ] ] ),
				VSub( Mesh->Positions[ FaceT->VertexIndex[ 1 ] ], Mesh->Positions[ FaceT->VertexIndex[ 3 ] ] )
			) ;
			OutVec[ 1 ] = VCross(
				VSub( Mesh->Positions[ FaceT->VertexIndex[ 1 ] ], Mesh->Positions[ FaceT->VertexIndex[ 2 ] ] ),
				VSub( Mesh->Positions[ FaceT->VertexIndex[ 2 ] ], Mesh->Positions[ FaceT->VertexIndex[ 3 ] ] )
			) ;
			OutVec[ 2 ] = VCross(
				VSub( Mesh->Positions[ FaceT->VertexIndex[ 2 ] ], Mesh->Positions[ FaceT->VertexIndex[ 0 ] ] ),
				VSub( Mesh->Positions[ FaceT->VertexIndex[ 0 ] ], Mesh->Positions[ FaceT->VertexIndex[ 3 ] ] )
			) ;
			if( VDot( OutVec[ 0 ], OutVec[ 1 ] ) > 0.0f &&
				VDot( OutVec[ 0 ], OutVec[ 2 ] ) > 0.0f )
			{
				// 三角形( 0, 1, 2 ) の内側に頂点3がある場合の処理
				NewFaceT2->IndexNum = 3 ;
				NewFaceT2->PolygonNum = 1 ;
				NewFaceT2->VertexIndex[ 0 ] = FaceT->VertexIndex[ 0 ] ;
				NewFaceT2->VertexIndex[ 1 ] = FaceT->VertexIndex[ 1 ] ;
				NewFaceT2->VertexIndex[ 2 ] = FaceT->VertexIndex[ 3 ] ;
				NewFaceT2->NormalIndex[ 0 ] = FaceT->NormalIndex[ 0 ] ;
				NewFaceT2->NormalIndex[ 1 ] = FaceT->NormalIndex[ 1 ] ;
				NewFaceT2->NormalIndex[ 2 ] = FaceT->NormalIndex[ 3 ] ;
				NewFaceT2->VertexColorIndex[ 0 ] = FaceT->VertexColorIndex[ 0 ] ;
				NewFaceT2->VertexColorIndex[ 1 ] = FaceT->VertexColorIndex[ 1 ] ;
				NewFaceT2->VertexColorIndex[ 2 ] = FaceT->VertexColorIndex[ 3 ] ;
				for( k = 0 ; ( DWORD )k < MV1_READ_MAX_UV_NUM ; k ++ )
				{
					NewFaceT2->UVIndex[ k ][ 0 ] = FaceT->UVIndex[ k ][ 0 ] ;
					NewFaceT2->UVIndex[ k ][ 1 ] = FaceT->UVIndex[ k ][ 1 ] ;
					NewFaceT2->UVIndex[ k ][ 2 ] = FaceT->UVIndex[ k ][ 3 ] ;
				}
				NewFaceT2->MaterialIndex = FaceT->MaterialIndex ;
				NewFaceT2 ++ ;

				NewFaceT->VertexIndex[ 0 ] = FaceT->VertexIndex[ 1 ] ;
				NewFaceT->VertexIndex[ 1 ] = FaceT->VertexIndex[ 2 ] ;
				NewFaceT->VertexIndex[ 2 ] = FaceT->VertexIndex[ 3 ] ;
				NewFaceT->NormalIndex[ 0 ] = FaceT->NormalIndex[ 1 ] ;
				NewFaceT->NormalIndex[ 1 ] = FaceT->NormalIndex[ 2 ] ;
				NewFaceT->NormalIndex[ 2 ] = FaceT->NormalIndex[ 3 ] ;
				NewFaceT->VertexColorIndex[ 0 ] = FaceT->VertexColorIndex[ 1 ] ;
				NewFaceT->VertexColorIndex[ 1 ] = FaceT->VertexColorIndex[ 2 ] ;
				NewFaceT->VertexColorIndex[ 2 ] = FaceT->VertexColorIndex[ 3 ] ;
				for( k = 0 ; ( DWORD )k < MV1_READ_MAX_UV_NUM ; k ++ )
				{
					NewFaceT->UVIndex[ k ][ 0 ] = FaceT->UVIndex[ k ][ 1 ] ;
					NewFaceT->UVIndex[ k ][ 1 ] = FaceT->UVIndex[ k ][ 2 ] ;
					NewFaceT->UVIndex[ k ][ 2 ] = FaceT->UVIndex[ k ][ 3 ] ;
				}

				// 両面ポリゴンの場合はポリゴン数も倍になる
				if( Mesh->Materials[ FaceT->MaterialIndex ]->TwoSide )
				{
					NewFaceT2->IndexNum = 3 ;
					NewFaceT2->PolygonNum = 1 ;
					NewFaceT2->VertexIndex[ 0 ] = FaceT->VertexIndex[ 3 ] ;
					NewFaceT2->VertexIndex[ 1 ] = FaceT->VertexIndex[ 1 ] ;
					NewFaceT2->VertexIndex[ 2 ] = FaceT->VertexIndex[ 0 ] ;
					NewFaceT2->NormalIndex[ 0 ] = FaceT->NormalIndex[ 3 ] ;
					NewFaceT2->NormalIndex[ 1 ] = FaceT->NormalIndex[ 1 ] ;
					NewFaceT2->NormalIndex[ 2 ] = FaceT->NormalIndex[ 0 ] ;
					NewFaceT2->VertexColorIndex[ 0 ] = FaceT->VertexColorIndex[ 3 ] ;
					NewFaceT2->VertexColorIndex[ 1 ] = FaceT->VertexColorIndex[ 1 ] ;
					NewFaceT2->VertexColorIndex[ 2 ] = FaceT->VertexColorIndex[ 0 ] ;
					for( k = 0 ; ( DWORD )k < MV1_READ_MAX_UV_NUM ; k ++ )
					{
						NewFaceT2->UVIndex[ k ][ 0 ] = FaceT->UVIndex[ k ][ 3 ] ;
						NewFaceT2->UVIndex[ k ][ 1 ] = FaceT->UVIndex[ k ][ 1 ] ;
						NewFaceT2->UVIndex[ k ][ 2 ] = FaceT->UVIndex[ k ][ 0 ] ;
					}
					NewFaceT2->MaterialIndex = FaceT->MaterialIndex ;
					NewFaceT2 ++ ;

					NewFaceT2->IndexNum = 3 ;
					NewFaceT2->PolygonNum = 1 ;
					NewFaceT2->VertexIndex[ 0 ] = FaceT->VertexIndex[ 3 ] ;
					NewFaceT2->VertexIndex[ 1 ] = FaceT->VertexIndex[ 2 ] ;
					NewFaceT2->VertexIndex[ 2 ] = FaceT->VertexIndex[ 1 ] ;
					NewFaceT2->NormalIndex[ 0 ] = FaceT->NormalIndex[ 3 ] ;
					NewFaceT2->NormalIndex[ 1 ] = FaceT->NormalIndex[ 2 ] ;
					NewFaceT2->NormalIndex[ 2 ] = FaceT->NormalIndex[ 1 ] ;
					NewFaceT2->VertexColorIndex[ 0 ] = FaceT->VertexColorIndex[ 3 ] ;
					NewFaceT2->VertexColorIndex[ 1 ] = FaceT->VertexColorIndex[ 2 ] ;
					NewFaceT2->VertexColorIndex[ 2 ] = FaceT->VertexColorIndex[ 1 ] ;
					for( k = 0 ; ( DWORD )k < MV1_READ_MAX_UV_NUM ; k ++ )
					{
						NewFaceT2->UVIndex[ k ][ 0 ] = FaceT->UVIndex[ k ][ 3 ] ;
						NewFaceT2->UVIndex[ k ][ 1 ] = FaceT->UVIndex[ k ][ 2 ] ;
						NewFaceT2->UVIndex[ k ][ 2 ] = FaceT->UVIndex[ k ][ 1 ] ;
					}
					NewFaceT2->MaterialIndex = FaceT->MaterialIndex ;
					NewFaceT2 ++ ;
				}

				continue ;
			}
		}

		AddNum = ( int )( FaceT->IndexNum - 3 ) ;
		if( AddNum == 0 ) continue ;

		p1 = 0 ;
		p2 = 2 ;
		j = 0 ;
		for(;;)
		{
			pt = ( int )( p1 - 1 == -1 ? FaceT->IndexNum - 1 : p1 - 1 ) ;
			NewFaceT2->IndexNum = 3 ;
			NewFaceT2->PolygonNum = 1 ;
			NewFaceT2->VertexIndex[ 0 ] = FaceT->VertexIndex[ p1 ] ;
			NewFaceT2->VertexIndex[ 1 ] = FaceT->VertexIndex[ p2 ] ;
			NewFaceT2->VertexIndex[ 2 ] = FaceT->VertexIndex[ pt ] ;
			NewFaceT2->NormalIndex[ 0 ] = FaceT->NormalIndex[ p1 ] ;
			NewFaceT2->NormalIndex[ 1 ] = FaceT->NormalIndex[ p2 ] ;
			NewFaceT2->NormalIndex[ 2 ] = FaceT->NormalIndex[ pt ] ;
			NewFaceT2->VertexColorIndex[ 0 ] = FaceT->VertexColorIndex[ p1 ] ;
			NewFaceT2->VertexColorIndex[ 1 ] = FaceT->VertexColorIndex[ p2 ] ;
			NewFaceT2->VertexColorIndex[ 2 ] = FaceT->VertexColorIndex[ pt ] ;
			for( k = 0 ; ( DWORD )k < MV1_READ_MAX_UV_NUM ; k ++ )
			{
				NewFaceT2->UVIndex[ k ][ 0 ] = FaceT->UVIndex[ k ][ p1 ] ;
				NewFaceT2->UVIndex[ k ][ 1 ] = FaceT->UVIndex[ k ][ p2 ] ;
				NewFaceT2->UVIndex[ k ][ 2 ] = FaceT->UVIndex[ k ][ pt ] ;
			}
			NewFaceT2->MaterialIndex = FaceT->MaterialIndex ;
			NewFaceT2 ++ ;

			// 両面ポリゴンの場合はポリゴン数も倍になる
			if( Mesh->Materials[ FaceT->MaterialIndex ]->TwoSide )
			{
				NewFaceT2->IndexNum = 3 ;
				NewFaceT2->PolygonNum = 1 ;
				NewFaceT2->VertexIndex[ 0 ] = FaceT->VertexIndex[ pt ] ;
				NewFaceT2->VertexIndex[ 1 ] = FaceT->VertexIndex[ p2 ] ;
				NewFaceT2->VertexIndex[ 2 ] = FaceT->VertexIndex[ p1 ] ;
				NewFaceT2->NormalIndex[ 0 ] = FaceT->NormalIndex[ pt ] ;
				NewFaceT2->NormalIndex[ 1 ] = FaceT->NormalIndex[ p2 ] ;
				NewFaceT2->NormalIndex[ 2 ] = FaceT->NormalIndex[ p1 ] ;
				NewFaceT2->VertexColorIndex[ 0 ] = FaceT->VertexColorIndex[ pt ] ;
				NewFaceT2->VertexColorIndex[ 1 ] = FaceT->VertexColorIndex[ p2 ] ;
				NewFaceT2->VertexColorIndex[ 2 ] = FaceT->VertexColorIndex[ p1 ] ;
				for( k = 0 ; ( DWORD )k < MV1_READ_MAX_UV_NUM ; k ++ )
				{
					NewFaceT2->UVIndex[ k ][ 0 ] = FaceT->UVIndex[ k ][ pt ] ;
					NewFaceT2->UVIndex[ k ][ 1 ] = FaceT->UVIndex[ k ][ p2 ] ;
					NewFaceT2->UVIndex[ k ][ 2 ] = FaceT->UVIndex[ k ][ p1 ] ;
				}
				NewFaceT2->MaterialIndex = FaceT->MaterialIndex ;
				NewFaceT2 ++ ;
			}

			p1 -- ;
			if( p1 == -1 ) p1 = ( int )( FaceT->IndexNum - 1 ) ;
			j ++ ;
			if( AddNum == j ) break ;

			pt = p2 + 1 ;
			NewFaceT2->IndexNum = 3 ;
			NewFaceT2->PolygonNum = 1 ;
			NewFaceT2->VertexIndex[ 0 ] = FaceT->VertexIndex[ p1 ] ;
			NewFaceT2->VertexIndex[ 1 ] = FaceT->VertexIndex[ p2 ] ;
			NewFaceT2->VertexIndex[ 2 ] = FaceT->VertexIndex[ pt ] ;
			NewFaceT2->NormalIndex[ 0 ] = FaceT->NormalIndex[ p1 ] ;
			NewFaceT2->NormalIndex[ 1 ] = FaceT->NormalIndex[ p2 ] ;
			NewFaceT2->NormalIndex[ 2 ] = FaceT->NormalIndex[ pt ] ;
			NewFaceT2->VertexColorIndex[ 0 ] = FaceT->VertexColorIndex[ p1 ] ;
			NewFaceT2->VertexColorIndex[ 1 ] = FaceT->VertexColorIndex[ p2 ] ;
			NewFaceT2->VertexColorIndex[ 2 ] = FaceT->VertexColorIndex[ pt ] ;
			for( k = 0 ; ( DWORD )k < MV1_READ_MAX_UV_NUM ; k ++ )
			{
				NewFaceT2->UVIndex[ k ][ 0 ] = FaceT->UVIndex[ k ][ p1 ] ;
				NewFaceT2->UVIndex[ k ][ 1 ] = FaceT->UVIndex[ k ][ p2 ] ;
				NewFaceT2->UVIndex[ k ][ 2 ] = FaceT->UVIndex[ k ][ pt ] ;
			}
			NewFaceT2->MaterialIndex = FaceT->MaterialIndex ;
			NewFaceT2 ++ ;

			// 両面ポリゴンの場合はポリゴン数も倍になる
			if( Mesh->Materials[ FaceT->MaterialIndex ]->TwoSide )
			{
				NewFaceT2->IndexNum = 3 ;
				NewFaceT2->PolygonNum = 1 ;
				NewFaceT2->VertexIndex[ 0 ] = FaceT->VertexIndex[ pt ] ;
				NewFaceT2->VertexIndex[ 1 ] = FaceT->VertexIndex[ p2 ] ;
				NewFaceT2->VertexIndex[ 2 ] = FaceT->VertexIndex[ p1 ] ;
				NewFaceT2->NormalIndex[ 0 ] = FaceT->NormalIndex[ pt ] ;
				NewFaceT2->NormalIndex[ 1 ] = FaceT->NormalIndex[ p2 ] ;
				NewFaceT2->NormalIndex[ 2 ] = FaceT->NormalIndex[ p1 ] ;
				NewFaceT2->VertexColorIndex[ 0 ] = FaceT->VertexColorIndex[ pt ] ;
				NewFaceT2->VertexColorIndex[ 1 ] = FaceT->VertexColorIndex[ p2 ] ;
				NewFaceT2->VertexColorIndex[ 2 ] = FaceT->VertexColorIndex[ p1 ] ;
				for( k = 0 ; ( DWORD )k < MV1_READ_MAX_UV_NUM ; k ++ )
				{
					NewFaceT2->UVIndex[ k ][ 0 ] = FaceT->UVIndex[ k ][ pt ] ;
					NewFaceT2->UVIndex[ k ][ 1 ] = FaceT->UVIndex[ k ][ p2 ] ;
					NewFaceT2->UVIndex[ k ][ 2 ] = FaceT->UVIndex[ k ][ p1 ] ;
				}
				NewFaceT2->MaterialIndex = FaceT->MaterialIndex ;
				NewFaceT2 ++ ;
			}

			p2 ++ ;
			j ++ ;
			if( AddNum == j ) break ;
		}
	}

	// 面の数を増やす
	Mesh->FaceNum += AddFaceCount ;

	// メモリの解放と差し替え
	SUBMEMAREA( &ReadModel->Mem, Mesh->Faces );
	Mesh->Faces = NewFace ;
	Mesh->FaceIndexBuffer = NewIndex ;

	// 正常終了
	return true ;
}

// 辺を共有していて、且つ法線の方向が異なる辺に面積０のポリゴンを追加する( ３角形ポリゴンの集合になっている必要あり )
static bool MV1NotEqualNormalSideAddZeroAreaPolygon( MV1_MODEL_R *ReadModel, MV1_MESH_R *Mesh )
{
	int AddFaceCount ;
	int i, j, k ;
	MV1_MESHFACE_R *NewFace ;
	MV1_MESHFACE_R *FaceT ;
	MV1_MESHFACE_R *FaceT2 ;
	MV1_MESHFACE_R *NewFaceT ;
	MV1_MESHFACE_R *NewFaceT2 ;
	DWORD *NewIndex, *NewIndexT ;
	DWORD NewFaceNum ;
	DWORD TargetSideNum ;
	DWORD *TargetFace1 ;
	DWORD *TargetFace2 ;
	DWORD (*TargetFace1VIndex)[ 2 ] ;
	DWORD (*TargetFace2VIndex)[ 2 ] ;

	// 作業用のメモリ領域を確保
	TargetFace1 = ( DWORD * )DXALLOC( ( sizeof( DWORD ) * 6 ) * Mesh->FaceNum * 3 ) ;
	if( TargetFace1 == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x52\x00\x65\x00\x61\x00\x64\x00\x20\x00\x4d\x00\x6f\x00\x64\x00\x65\x00\x6c\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x2e\x7e\x00\x90\xdd\x30\xea\x30\xb4\x30\xf3\x30\xfd\x8f\xa0\x52\x5c\x4f\x6d\x69\x28\x75\x6e\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Read Model Error : 縮退ポリゴン追加作業用のメモリの確保に失敗しました\n" @*/ )) ;
		return false ;
	}
	TargetFace2 = TargetFace1 + Mesh->FaceNum * 3 ;
	TargetFace1VIndex = ( DWORD (*)[ 2 ] )( TargetFace2 + Mesh->FaceNum * 3 ) ;
	TargetFace2VIndex = TargetFace1VIndex + Mesh->FaceNum * 3 ;

	// 増やさなければならない面データの数を調べる
	FaceT = Mesh->Faces ;
	TargetSideNum = 0 ;
	for( i = 0 ; ( DWORD )i < Mesh->FaceNum ; i ++, FaceT ++ )
	{
		FaceT2 = FaceT + 1 ;
		for( j = i + 1 ; ( DWORD )j < Mesh->FaceNum ; j++, FaceT2 ++ )
		{
			// 使用している２頂点が等しく、且つ使用している法線が異なる辺があったら面データを増やす
			if( ( FaceT->VertexIndex[ 0 ] == FaceT2->VertexIndex[ 0 ] &&
				  FaceT->VertexIndex[ 1 ] == FaceT2->VertexIndex[ 1 ] &&
				  ( FaceT->NormalIndex[ 0 ] != FaceT2->NormalIndex[ 0 ] ||
				    FaceT->NormalIndex[ 1 ] != FaceT2->NormalIndex[ 1 ] ) ) )
			{
				TargetFace1[ TargetSideNum ] = i ;
				TargetFace1VIndex[ TargetSideNum ][ 0 ] = 0 ;
				TargetFace1VIndex[ TargetSideNum ][ 1 ] = 1 ;

				TargetFace2[ TargetSideNum ] = j ;
				TargetFace2VIndex[ TargetSideNum ][ 0 ] = 0 ;
				TargetFace2VIndex[ TargetSideNum ][ 1 ] = 1 ;

				TargetSideNum++;
			}
			else
			if( ( FaceT->VertexIndex[ 0 ] == FaceT2->VertexIndex[ 1 ] &&
				  FaceT->VertexIndex[ 1 ] == FaceT2->VertexIndex[ 0 ] &&
				  ( FaceT->NormalIndex[ 0 ] != FaceT2->NormalIndex[ 1 ] ||
				    FaceT->NormalIndex[ 1 ] != FaceT2->NormalIndex[ 0 ] ) ) )
			{
				TargetFace1[ TargetSideNum ] = i ;
				TargetFace1VIndex[ TargetSideNum ][ 0 ] = 0 ;
				TargetFace1VIndex[ TargetSideNum ][ 1 ] = 1 ;

				TargetFace2[ TargetSideNum ] = j ;
				TargetFace2VIndex[ TargetSideNum ][ 0 ] = 1 ;
				TargetFace2VIndex[ TargetSideNum ][ 1 ] = 0 ;

				TargetSideNum++;
			}
			else
			if( ( FaceT->VertexIndex[ 0 ] == FaceT2->VertexIndex[ 0 ] &&
				  FaceT->VertexIndex[ 1 ] == FaceT2->VertexIndex[ 2 ] &&
				  ( FaceT->NormalIndex[ 0 ] != FaceT2->NormalIndex[ 0 ] ||
				    FaceT->NormalIndex[ 1 ] != FaceT2->NormalIndex[ 2 ] ) ) )
			{
				TargetFace1[ TargetSideNum ] = i ;
				TargetFace1VIndex[ TargetSideNum ][ 0 ] = 0 ;
				TargetFace1VIndex[ TargetSideNum ][ 1 ] = 1 ;

				TargetFace2[ TargetSideNum ] = j ;
				TargetFace2VIndex[ TargetSideNum ][ 0 ] = 0 ;
				TargetFace2VIndex[ TargetSideNum ][ 1 ] = 2 ;

				TargetSideNum++;
			}
			else
			if( ( FaceT->VertexIndex[ 0 ] == FaceT2->VertexIndex[ 2 ] &&
				  FaceT->VertexIndex[ 1 ] == FaceT2->VertexIndex[ 0 ] &&
				  ( FaceT->NormalIndex[ 0 ] != FaceT2->NormalIndex[ 2 ] ||
				    FaceT->NormalIndex[ 1 ] != FaceT2->NormalIndex[ 0 ] ) ) )
			{
				TargetFace1[ TargetSideNum ] = i ;
				TargetFace1VIndex[ TargetSideNum ][ 0 ] = 0 ;
				TargetFace1VIndex[ TargetSideNum ][ 1 ] = 1 ;

				TargetFace2[ TargetSideNum ] = j ;
				TargetFace2VIndex[ TargetSideNum ][ 0 ] = 2 ;
				TargetFace2VIndex[ TargetSideNum ][ 1 ] = 0 ;

				TargetSideNum++;
			}
			else
			if( ( FaceT->VertexIndex[ 0 ] == FaceT2->VertexIndex[ 1 ] &&
				  FaceT->VertexIndex[ 1 ] == FaceT2->VertexIndex[ 2 ] &&
				  ( FaceT->NormalIndex[ 0 ] != FaceT2->NormalIndex[ 1 ] ||
				    FaceT->NormalIndex[ 1 ] != FaceT2->NormalIndex[ 2 ] ) ) )
			{
				TargetFace1[ TargetSideNum ] = i ;
				TargetFace1VIndex[ TargetSideNum ][ 0 ] = 0 ;
				TargetFace1VIndex[ TargetSideNum ][ 1 ] = 1 ;

				TargetFace2[ TargetSideNum ] = j ;
				TargetFace2VIndex[ TargetSideNum ][ 0 ] = 1 ;
				TargetFace2VIndex[ TargetSideNum ][ 1 ] = 2 ;

				TargetSideNum++;
			}
			else
			if( ( FaceT->VertexIndex[ 0 ] == FaceT2->VertexIndex[ 2 ] &&
				  FaceT->VertexIndex[ 1 ] == FaceT2->VertexIndex[ 1 ] &&
				  ( FaceT->NormalIndex[ 0 ] != FaceT2->NormalIndex[ 2 ] ||
				    FaceT->NormalIndex[ 1 ] != FaceT2->NormalIndex[ 1 ] ) ) )
			{
				TargetFace1[ TargetSideNum ] = i ;
				TargetFace1VIndex[ TargetSideNum ][ 0 ] = 0 ;
				TargetFace1VIndex[ TargetSideNum ][ 1 ] = 1 ;

				TargetFace2[ TargetSideNum ] = j ;
				TargetFace2VIndex[ TargetSideNum ][ 0 ] = 2 ;
				TargetFace2VIndex[ TargetSideNum ][ 1 ] = 1 ;

				TargetSideNum++;
			}

			if( ( FaceT->VertexIndex[ 1 ] == FaceT2->VertexIndex[ 0 ] &&
				  FaceT->VertexIndex[ 2 ] == FaceT2->VertexIndex[ 1 ] &&
				  ( FaceT->NormalIndex[ 1 ] != FaceT2->NormalIndex[ 0 ] ||
				    FaceT->NormalIndex[ 2 ] != FaceT2->NormalIndex[ 1 ] ) ) )
			{
				TargetFace1[ TargetSideNum ] = i ;
				TargetFace1VIndex[ TargetSideNum ][ 0 ] = 1 ;
				TargetFace1VIndex[ TargetSideNum ][ 1 ] = 2 ;

				TargetFace2[ TargetSideNum ] = j ;
				TargetFace2VIndex[ TargetSideNum ][ 0 ] = 0 ;
				TargetFace2VIndex[ TargetSideNum ][ 1 ] = 1 ;

				TargetSideNum++;
			}
			else
			if( ( FaceT->VertexIndex[ 1 ] == FaceT2->VertexIndex[ 1 ] &&
				  FaceT->VertexIndex[ 2 ] == FaceT2->VertexIndex[ 0 ] &&
				  ( FaceT->NormalIndex[ 1 ] != FaceT2->NormalIndex[ 1 ] ||
				    FaceT->NormalIndex[ 2 ] != FaceT2->NormalIndex[ 0 ] ) ) )
			{
				TargetFace1[ TargetSideNum ] = i ;
				TargetFace1VIndex[ TargetSideNum ][ 0 ] = 1 ;
				TargetFace1VIndex[ TargetSideNum ][ 1 ] = 2 ;

				TargetFace2[ TargetSideNum ] = j ;
				TargetFace2VIndex[ TargetSideNum ][ 0 ] = 1 ;
				TargetFace2VIndex[ TargetSideNum ][ 1 ] = 0 ;

				TargetSideNum++;
			}
			else
			if( ( FaceT->VertexIndex[ 1 ] == FaceT2->VertexIndex[ 0 ] &&
				  FaceT->VertexIndex[ 2 ] == FaceT2->VertexIndex[ 2 ] &&
				  ( FaceT->NormalIndex[ 1 ] != FaceT2->NormalIndex[ 0 ] ||
				    FaceT->NormalIndex[ 2 ] != FaceT2->NormalIndex[ 2 ] ) ) )
			{
				TargetFace1[ TargetSideNum ] = i ;
				TargetFace1VIndex[ TargetSideNum ][ 0 ] = 1 ;
				TargetFace1VIndex[ TargetSideNum ][ 1 ] = 2 ;

				TargetFace2[ TargetSideNum ] = j ;
				TargetFace2VIndex[ TargetSideNum ][ 0 ] = 0 ;
				TargetFace2VIndex[ TargetSideNum ][ 1 ] = 2 ;

				TargetSideNum++;
			}
			else
			if( ( FaceT->VertexIndex[ 1 ] == FaceT2->VertexIndex[ 2 ] &&
				  FaceT->VertexIndex[ 2 ] == FaceT2->VertexIndex[ 0 ] &&
				  ( FaceT->NormalIndex[ 1 ] != FaceT2->NormalIndex[ 2 ] ||
				    FaceT->NormalIndex[ 2 ] != FaceT2->NormalIndex[ 0 ] ) ) )
			{
				TargetFace1[ TargetSideNum ] = i ;
				TargetFace1VIndex[ TargetSideNum ][ 0 ] = 1 ;
				TargetFace1VIndex[ TargetSideNum ][ 1 ] = 2 ;

				TargetFace2[ TargetSideNum ] = j ;
				TargetFace2VIndex[ TargetSideNum ][ 0 ] = 2 ;
				TargetFace2VIndex[ TargetSideNum ][ 1 ] = 0 ;

				TargetSideNum++;
			}
			else
			if( ( FaceT->VertexIndex[ 1 ] == FaceT2->VertexIndex[ 1 ] &&
				  FaceT->VertexIndex[ 2 ] == FaceT2->VertexIndex[ 2 ] &&
				  ( FaceT->NormalIndex[ 1 ] != FaceT2->NormalIndex[ 1 ] ||
				    FaceT->NormalIndex[ 2 ] != FaceT2->NormalIndex[ 2 ] ) ) )
			{
				TargetFace1[ TargetSideNum ] = i ;
				TargetFace1VIndex[ TargetSideNum ][ 0 ] = 1 ;
				TargetFace1VIndex[ TargetSideNum ][ 1 ] = 2 ;

				TargetFace2[ TargetSideNum ] = j ;
				TargetFace2VIndex[ TargetSideNum ][ 0 ] = 1 ;
				TargetFace2VIndex[ TargetSideNum ][ 1 ] = 2 ;

				TargetSideNum++;
			}
			else
			if( ( FaceT->VertexIndex[ 1 ] == FaceT2->VertexIndex[ 2 ] &&
				  FaceT->VertexIndex[ 2 ] == FaceT2->VertexIndex[ 1 ] &&
				  ( FaceT->NormalIndex[ 1 ] != FaceT2->NormalIndex[ 2 ] ||
				    FaceT->NormalIndex[ 2 ] != FaceT2->NormalIndex[ 1 ] ) ) )
			{
				TargetFace1[ TargetSideNum ] = i ;
				TargetFace1VIndex[ TargetSideNum ][ 0 ] = 1 ;
				TargetFace1VIndex[ TargetSideNum ][ 1 ] = 2 ;

				TargetFace2[ TargetSideNum ] = j ;
				TargetFace2VIndex[ TargetSideNum ][ 0 ] = 2 ;
				TargetFace2VIndex[ TargetSideNum ][ 1 ] = 1 ;

				TargetSideNum++;
			}

			if( ( FaceT->VertexIndex[ 2 ] == FaceT2->VertexIndex[ 0 ] &&
				  FaceT->VertexIndex[ 0 ] == FaceT2->VertexIndex[ 1 ] &&
				  ( FaceT->NormalIndex[ 2 ] != FaceT2->NormalIndex[ 0 ] ||
				    FaceT->NormalIndex[ 0 ] != FaceT2->NormalIndex[ 1 ] ) ) )
			{
				TargetFace1[ TargetSideNum ] = i ;
				TargetFace1VIndex[ TargetSideNum ][ 0 ] = 2 ;
				TargetFace1VIndex[ TargetSideNum ][ 1 ] = 0 ;

				TargetFace2[ TargetSideNum ] = j ;
				TargetFace2VIndex[ TargetSideNum ][ 0 ] = 0 ;
				TargetFace2VIndex[ TargetSideNum ][ 1 ] = 1 ;

				TargetSideNum++;
			}
			else
			if( ( FaceT->VertexIndex[ 2 ] == FaceT2->VertexIndex[ 1 ] &&
				  FaceT->VertexIndex[ 0 ] == FaceT2->VertexIndex[ 0 ] &&
				  ( FaceT->NormalIndex[ 2 ] != FaceT2->NormalIndex[ 1 ] ||
				    FaceT->NormalIndex[ 0 ] != FaceT2->NormalIndex[ 0 ] ) ) )
			{
				TargetFace1[ TargetSideNum ] = i ;
				TargetFace1VIndex[ TargetSideNum ][ 0 ] = 2 ;
				TargetFace1VIndex[ TargetSideNum ][ 1 ] = 0 ;

				TargetFace2[ TargetSideNum ] = j ;
				TargetFace2VIndex[ TargetSideNum ][ 0 ] = 1 ;
				TargetFace2VIndex[ TargetSideNum ][ 1 ] = 0 ;

				TargetSideNum++;
			}
			else
			if( ( FaceT->VertexIndex[ 2 ] == FaceT2->VertexIndex[ 0 ] &&
				  FaceT->VertexIndex[ 0 ] == FaceT2->VertexIndex[ 2 ] &&
				  ( FaceT->NormalIndex[ 2 ] != FaceT2->NormalIndex[ 0 ] ||
				    FaceT->NormalIndex[ 0 ] != FaceT2->NormalIndex[ 2 ] ) ) )
			{
				TargetFace1[ TargetSideNum ] = i ;
				TargetFace1VIndex[ TargetSideNum ][ 0 ] = 2 ;
				TargetFace1VIndex[ TargetSideNum ][ 1 ] = 0 ;

				TargetFace2[ TargetSideNum ] = j ;
				TargetFace2VIndex[ TargetSideNum ][ 0 ] = 0 ;
				TargetFace2VIndex[ TargetSideNum ][ 1 ] = 2 ;

				TargetSideNum++;
			}
			else
			if( ( FaceT->VertexIndex[ 2 ] == FaceT2->VertexIndex[ 2 ] &&
				  FaceT->VertexIndex[ 0 ] == FaceT2->VertexIndex[ 0 ] &&
				  ( FaceT->NormalIndex[ 2 ] != FaceT2->NormalIndex[ 2 ] ||
				    FaceT->NormalIndex[ 0 ] != FaceT2->NormalIndex[ 0 ] ) ) )
			{
				TargetFace1[ TargetSideNum ] = i ;
				TargetFace1VIndex[ TargetSideNum ][ 0 ] = 2 ;
				TargetFace1VIndex[ TargetSideNum ][ 1 ] = 0 ;

				TargetFace2[ TargetSideNum ] = j ;
				TargetFace2VIndex[ TargetSideNum ][ 0 ] = 2 ;
				TargetFace2VIndex[ TargetSideNum ][ 1 ] = 0 ;

				TargetSideNum++;
			}
			else
			if( ( FaceT->VertexIndex[ 2 ] == FaceT2->VertexIndex[ 1 ] &&
				  FaceT->VertexIndex[ 0 ] == FaceT2->VertexIndex[ 2 ] &&
				  ( FaceT->NormalIndex[ 2 ] != FaceT2->NormalIndex[ 1 ] ||
				    FaceT->NormalIndex[ 0 ] != FaceT2->NormalIndex[ 2 ] ) ) )
			{
				TargetFace1[ TargetSideNum ] = i ;
				TargetFace1VIndex[ TargetSideNum ][ 0 ] = 2 ;
				TargetFace1VIndex[ TargetSideNum ][ 1 ] = 0 ;

				TargetFace2[ TargetSideNum ] = j ;
				TargetFace2VIndex[ TargetSideNum ][ 0 ] = 1 ;
				TargetFace2VIndex[ TargetSideNum ][ 1 ] = 2 ;

				TargetSideNum++;
			}
			else
			if( ( FaceT->VertexIndex[ 2 ] == FaceT2->VertexIndex[ 2 ] &&
				  FaceT->VertexIndex[ 0 ] == FaceT2->VertexIndex[ 1 ] &&
				  ( FaceT->NormalIndex[ 2 ] != FaceT2->NormalIndex[ 2 ] ||
				    FaceT->NormalIndex[ 0 ] != FaceT2->NormalIndex[ 1 ] ) ) )
			{
				TargetFace1[ TargetSideNum ] = i ;
				TargetFace1VIndex[ TargetSideNum ][ 0 ] = 2 ;
				TargetFace1VIndex[ TargetSideNum ][ 1 ] = 0 ;

				TargetFace2[ TargetSideNum ] = j ;
				TargetFace2VIndex[ TargetSideNum ][ 0 ] = 2 ;
				TargetFace2VIndex[ TargetSideNum ][ 1 ] = 1 ;

				TargetSideNum++;
			}
		}
	}
	AddFaceCount = TargetSideNum * 2 ;

	// 追加面が無い場合はここで終了
	if( TargetSideNum == 0 )
	{
		DXFREE( TargetFace1 ) ;
		return true ;
	}

	// 増やした面を格納するメモリ領域の確保
	NewFaceNum = Mesh->FaceNum + AddFaceCount ;
	NewFace = ( MV1_MESHFACE_R * )ADDMEMAREA( ( sizeof( MV1_MESHFACE_R ) + ( 3 + MV1_READ_MAX_UV_NUM ) * 3 * sizeof( DWORD ) ) * NewFaceNum, &ReadModel->Mem ) ;
	if( NewFace == NULL )
	{
		DXFREE( TargetFace1 ) ;
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x52\x00\x65\x00\x61\x00\x64\x00\x20\x00\x4d\x00\x6f\x00\x64\x00\x65\x00\x6c\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x2e\x7e\x00\x90\xdd\x30\xea\x30\xb4\x30\xf3\x30\xfd\x8f\xa0\x52\x8c\x5f\x6e\x30\x62\x97\xc7\x30\xfc\x30\xbf\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Read Model Error : 縮退ポリゴン追加後の面データを格納するメモリ領域の確保に失敗しました\n" @*/ )) ;
		return false ;
	}
	NewIndex = ( DWORD * )( NewFace + NewFaceNum ) ;

	// インデックスデータのアドレスをセットアップ
	NewFaceT = NewFace ;
	NewIndexT = NewIndex ;
	for( i = 0 ; i < ( int )NewFaceNum ; i ++, NewFaceT ++ )
	{
		NewFaceT->VertexIndex = NewIndexT ; NewIndexT += 3 ;
		NewFaceT->NormalIndex = NewIndexT ; NewIndexT += 3 ;
		NewFaceT->VertexColorIndex = NewIndexT ; NewIndexT += 3 ;
		for( j = 0 ; j < MV1_READ_MAX_UV_NUM ; j ++, NewIndexT += 3 )
			NewFaceT->UVIndex[ j ] = NewIndexT ;
	}

	// 既存の面の情報をコピー
	FaceT = Mesh->Faces ;
	NewFaceT = NewFace ;
	for( i = 0 ; i < ( int )Mesh->FaceNum ; i ++, FaceT ++, NewFaceT ++ )
	{
		NewFaceT->IndexNum = 3 ;
		NewFaceT->PolygonNum = 1 ;

		for( j = 0 ; j < 3 ; j ++ )
		{
			NewFaceT->VertexIndex[ j ] = FaceT->VertexIndex[ j ] ;
			NewFaceT->NormalIndex[ j ] = FaceT->NormalIndex[ j ] ;
			NewFaceT->VertexColorIndex[ j ] = FaceT->VertexColorIndex[ j ] ;
			for( k = 0 ; k < MV1_READ_MAX_UV_NUM ; k ++ )
			{
				NewFaceT->UVIndex[ k ][ j ] = FaceT->UVIndex[ k ][ j ] ;
			}
		}

		NewFaceT->MaterialIndex = FaceT->MaterialIndex ;
		NewFaceT->Normal = FaceT->Normal ;
	}

	// 縮退ポリゴンを追加
	NewFaceT2 = NewFace + Mesh->FaceNum ;
	for( i = 0 ; ( DWORD )i < TargetSideNum ; i ++ )
	{
		int i1, i2, i3 ;

		FaceT  = &Mesh->Faces[ TargetFace1[ i ] ] ;
		FaceT2 = &Mesh->Faces[ TargetFace2[ i ] ] ;

		i1 = TargetFace1VIndex[ i ][ 0 ] ;
		i2 = TargetFace2VIndex[ i ][ 0 ] ;
		i3 = TargetFace1VIndex[ i ][ 1 ] ;
		NewFaceT2->IndexNum = 3 ;
		NewFaceT2->PolygonNum = 1 ;
		NewFaceT2->VertexIndex[ 0 ] = FaceT-> VertexIndex[ i1 ] ;
		NewFaceT2->VertexIndex[ 1 ] = FaceT2->VertexIndex[ i2 ] ;
		NewFaceT2->VertexIndex[ 2 ] = FaceT-> VertexIndex[ i3 ] ;
		NewFaceT2->NormalIndex[ 0 ] = FaceT-> NormalIndex[ i1 ] ;
		NewFaceT2->NormalIndex[ 1 ] = FaceT2->NormalIndex[ i2 ] ;
		NewFaceT2->NormalIndex[ 2 ] = FaceT-> NormalIndex[ i3 ] ;
		NewFaceT2->VertexColorIndex[ 0 ] = FaceT-> VertexColorIndex[ i1 ] ;
		NewFaceT2->VertexColorIndex[ 1 ] = FaceT2->VertexColorIndex[ i2 ] ;
		NewFaceT2->VertexColorIndex[ 2 ] = FaceT-> VertexColorIndex[ i3 ] ;
		for( k = 0 ; ( DWORD )k < MV1_READ_MAX_UV_NUM ; k ++ )
		{
			NewFaceT2->UVIndex[ k ][ 0 ] = FaceT ->UVIndex[ k ][ i1 ] ;
			NewFaceT2->UVIndex[ k ][ 1 ] = FaceT2->UVIndex[ k ][ i2 ] ;
			NewFaceT2->UVIndex[ k ][ 2 ] = FaceT ->UVIndex[ k ][ i3 ] ;
		}
		NewFaceT2->MaterialIndex = FaceT->MaterialIndex ;
		NewFaceT2++;

		i1 = TargetFace1VIndex[ i ][ 1 ] ;
		i2 = TargetFace2VIndex[ i ][ 0 ] ;
		i3 = TargetFace2VIndex[ i ][ 1 ] ;
		NewFaceT2->IndexNum = 3 ;
		NewFaceT2->PolygonNum = 1 ;
		NewFaceT2->VertexIndex[ 0 ] = FaceT ->VertexIndex[ i1 ] ;
		NewFaceT2->VertexIndex[ 1 ] = FaceT2->VertexIndex[ i2 ] ;
		NewFaceT2->VertexIndex[ 2 ] = FaceT2->VertexIndex[ i3 ] ;
		NewFaceT2->NormalIndex[ 0 ] = FaceT ->NormalIndex[ i1 ] ;
		NewFaceT2->NormalIndex[ 1 ] = FaceT2->NormalIndex[ i2 ] ;
		NewFaceT2->NormalIndex[ 2 ] = FaceT2->NormalIndex[ i3 ] ;
		NewFaceT2->VertexColorIndex[ 0 ] = FaceT ->VertexColorIndex[ i1 ] ;
		NewFaceT2->VertexColorIndex[ 1 ] = FaceT2->VertexColorIndex[ i2 ] ;
		NewFaceT2->VertexColorIndex[ 2 ] = FaceT2->VertexColorIndex[ i3 ] ;
		for( k = 0 ; ( DWORD )k < MV1_READ_MAX_UV_NUM ; k ++ )
		{
			NewFaceT2->UVIndex[ k ][ 0 ] = FaceT ->UVIndex[ k ][ i1 ] ;
			NewFaceT2->UVIndex[ k ][ 1 ] = FaceT2->UVIndex[ k ][ i2 ] ;
			NewFaceT2->UVIndex[ k ][ 2 ] = FaceT2->UVIndex[ k ][ i3 ] ;
		}
		NewFaceT2->MaterialIndex = FaceT->MaterialIndex ;
		NewFaceT2++;
	}

	// 面の数を増やす
	Mesh->FaceNum += AddFaceCount ;

	// メモリの解放と差し替え
	SUBMEMAREA( &ReadModel->Mem, Mesh->Faces );
	Mesh->Faces = NewFace ;
	Mesh->FaceIndexBuffer = NewIndex ;

	// 作業用の確保したメモリを解放
	DXFREE( TargetFace1 ) ;

	// 正常終了
	return true ;
}

// 座標、スキニングウエイト値が全く同一の頂点を統合する( 法線は無視する )
struct MV1_OPTIMIZEPOSITION_WEIGHT_ONE
{
	MV1_SKIN_WEIGHT_R *SkinWeight ;
	float Weight ;
} ;

struct MV1_OPTIMIZEPOSITION_WEIGHT
{
	DWORD DataNum ;
	MV1_OPTIMIZEPOSITION_WEIGHT_ONE Data[ 1 ] ;
} ;

static bool MV1OptimizePosition( MV1_MODEL_R * /*ReadModel*/, MV1_MESH_R *Mesh )
{
	int i, j, k, l, SubNum, NewPositionNum ;
	MV1_MESHFACE_R *Face ;
	BYTE *DisableFlag ;
	DWORD *NewVertexIndex ;
	VECTOR *OldPositions, Sa ;
	MV1_SKIN_WEIGHT_R *SkinW ;
	DWORD PosSkinWUnitSize ;
	MV1_OPTIMIZEPOSITION_WEIGHT *PositionSkinW, *PosSkinW, *PosSkinW2 ;

	// 無効フラグを格納するメモリ領域の確保
	PosSkinWUnitSize = sizeof( MV1_OPTIMIZEPOSITION_WEIGHT ) + sizeof( MV1_OPTIMIZEPOSITION_WEIGHT_ONE ) * ( Mesh->SkinWeightsNum - 1 ) ;
	OldPositions = ( VECTOR * )DXALLOC( ( sizeof( DWORD ) + sizeof( BYTE ) + sizeof( VECTOR ) + PosSkinWUnitSize ) * Mesh->PositionNum ) ;
	if( OldPositions == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x52\x00\x65\x00\x61\x00\x64\x00\x20\x00\x4d\x00\x6f\x00\x64\x00\x65\x00\x6c\x00\x20\x00\x43\x00\x6f\x00\x6e\x00\x76\x00\x65\x00\x72\x00\x74\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x02\x98\xb9\x70\x00\x67\x69\x90\x16\x53\xe6\x51\x06\x74\x67\x30\x7f\x4f\x28\x75\x59\x30\x8b\x30\xc6\x30\xf3\x30\xdd\x30\xe9\x30\xea\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Read Model Convert Error : 頂点最適化処理で使用するテンポラリバッファの確保に失敗しました\n" @*/ )) ;
		return false ;
	}
	PositionSkinW = ( MV1_OPTIMIZEPOSITION_WEIGHT * )( OldPositions + Mesh->PositionNum ) ;
	NewVertexIndex = ( DWORD * )( ( BYTE * )PositionSkinW + PosSkinWUnitSize * Mesh->PositionNum ) ;
	DisableFlag = ( BYTE * )( NewVertexIndex + Mesh->PositionNum ) ;
	_MEMSET( PositionSkinW, 0, ( sizeof( DWORD ) + sizeof( BYTE ) + PosSkinWUnitSize ) * Mesh->PositionNum ) ;
	_MEMCPY( OldPositions, Mesh->Positions, sizeof( VECTOR ) * Mesh->PositionNum ) ;

	// スキニングメッシュの場合、各頂点のスキニングメッシュリストを作成する
	if( Mesh->SkinWeightsNum )
	{
		for( i = 0 ; ( DWORD )i < Mesh->SkinWeightsNum ; i ++ )
		{
			SkinW = Mesh->SkinWeights[ i ] ;
			for( j = 0 ; ( DWORD )j < SkinW->DataNum ; j ++ )
			{
				PosSkinW = ( MV1_OPTIMIZEPOSITION_WEIGHT * )( ( BYTE * )PositionSkinW + PosSkinWUnitSize * SkinW->Data[ j ].TargetVertex ) ;
				PosSkinW->Data[ PosSkinW->DataNum ].SkinWeight = SkinW ;
				PosSkinW->Data[ PosSkinW->DataNum ].Weight     = SkinW->Data[ j ].Weight ;
				PosSkinW->DataNum ++ ;
			}
		}
	}

	// 一致テスト
	SubNum = 0 ;
	PosSkinW = PositionSkinW ;
	for( i = 0 ; ( DWORD )i < Mesh->PositionNum ; i ++, PosSkinW = ( MV1_OPTIMIZEPOSITION_WEIGHT * )( ( BYTE * )PosSkinW + PosSkinWUnitSize ) )
	{
		// 既に無効だったら何もしない
		if( DisableFlag[ i ] ) continue ;

		PosSkinW2 = ( MV1_OPTIMIZEPOSITION_WEIGHT * )( ( BYTE * )PositionSkinW + PosSkinWUnitSize * ( i + 1 ) ) ;
		for( j = i + 1 ; ( DWORD )j < Mesh->PositionNum ; j ++, PosSkinW2 = ( MV1_OPTIMIZEPOSITION_WEIGHT * )( ( BYTE * )PosSkinW2 + PosSkinWUnitSize ) )
		{
			// 既に無効だったら何もしない
			if( DisableFlag[ j ] ) continue ;

			// 座標が同じか調べる
//			if( *( ( DWORD * )&Mesh->Positions[ i ].x ) != *( ( DWORD * )&Mesh->Positions[ j ].x ) ||
//				*( ( DWORD * )&Mesh->Positions[ i ].y ) != *( ( DWORD * )&Mesh->Positions[ j ].y ) ||
//				*( ( DWORD * )&Mesh->Positions[ i ].z ) != *( ( DWORD * )&Mesh->Positions[ j ].z ) ) continue ;
			Sa = VSub( Mesh->Positions[ i ], Mesh->Positions[ j ] ) ;
			if( ( Sa.x > 0.00001f && Sa.x < -0.00001f ) ||
				( Sa.y > 0.00001f && Sa.y < -0.00001f ) ||
				( Sa.z > 0.00001f && Sa.z < -0.00001f ) ) continue ;

			// スキニングウエイトが同じか調べる
			if( PosSkinW->DataNum != PosSkinW2->DataNum ) continue ;
			for( k = 0 ; ( DWORD )k < PosSkinW->DataNum ; k ++ )
			{
				for( l = 0 ; ( DWORD )l < PosSkinW2->DataNum ; l ++ )
				{
					if( PosSkinW->Data[ k ].SkinWeight              == PosSkinW2->Data[ l ].SkinWeight &&
						*( ( DWORD * )&PosSkinW->Data[ k ].Weight ) == *( ( DWORD * )&PosSkinW2->Data[ l ].Weight ) )
						break ;
				}
				if( ( DWORD )l == PosSkinW2->DataNum ) break ;
			}
			if( ( DWORD )k != PosSkinW->DataNum ) continue ;

			// 同じ頂点を発見したら番号の若い頂点に統合する
			Face = Mesh->Faces ;
			for( k = 0 ; ( DWORD )k < Mesh->FaceNum ; k ++, Face ++ )
			{
				for( l = 0 ; ( DWORD )l < Face->IndexNum ; l ++ )
				{
					if( Face->VertexIndex[ l ] != ( DWORD )j ) continue ;
					Face->VertexIndex[ l ] = ( DWORD )i ;
				}
			}

			// 無効フラグを立てる
			DisableFlag[ j ] = 1 ;

			// 減らした数を加算する
			SubNum ++ ;
		}
	}

	// 何も減らない場合はここで終了
	if( SubNum == 0 )
	{
		DXFREE( OldPositions ) ;
		OldPositions = NULL ;
		return true ;
	}

	// 使用されている頂点のみ列挙する
	NewPositionNum = 0 ;
	for( i = 0 ; ( DWORD )i < Mesh->PositionNum ; i ++ )
	{
		if( DisableFlag[ i ] == 1 ) continue ;

		NewVertexIndex[ i ] = ( DWORD )NewPositionNum ;
		Mesh->Positions[ NewPositionNum ] = OldPositions[ i ] ;
		NewPositionNum ++ ;
	}
	Mesh->PositionNum = ( DWORD )NewPositionNum ;

	// ポリゴンの座標インデックスを付け直す
	Face = Mesh->Faces ;
	for( i = 0 ; ( DWORD )i < Mesh->FaceNum ; i ++, Face ++ )
	{
		for( j = 0 ; ( DWORD )j < Face->IndexNum ; j ++ )
		{
			Face->VertexIndex[ j ] = NewVertexIndex[ Face->VertexIndex[ j ] ] ;
		}
	}

	// ウエイト情報のターゲット座標がなくなった場合はウエイト値を０にする
	for( i = 0 ; ( DWORD )i < Mesh->SkinWeightsNum ; i ++ )
	{
		SkinW = Mesh->SkinWeights[ i ] ;
		for( j = 0 ; ( DWORD )j < SkinW->DataNum ; j ++ )
		{
			if( DisableFlag[ SkinW->Data[ j ].TargetVertex ] == 1 )
			{
				SkinW->Data[ j ].Weight = 0.0f ;
			}
			else
			{
				SkinW->Data[ j ].TargetVertex = NewVertexIndex[ SkinW->Data[ j ].TargetVertex ] ;
			}
		}
	}

	// 確保したメモリを解放する
	if( OldPositions )
	{
		DXFREE( OldPositions ) ;
		OldPositions = NULL ;
	}

	return true ;
}

// メッシュの従法線と接線を計算する( 全ポリゴンが三角形ポリゴンである必要があります )
static bool MV1MakeMeshBinormalsAndTangents( MV1_MODEL_R *ReadModel, MV1_MESH_R *Mesh )
{
	int i ;
	MV1_MESHFACE_R *Face ;
	VECTOR v1, v2, vt, du, dv, vb, vn ;

	// 従法線と接線を格納するメモリ領域を確保する
	if( Mesh->Binormals == NULL )
	{
		Mesh->Binormals = ( VECTOR * )ADDMEMAREA( sizeof( VECTOR ) * Mesh->NormalNum * 2, &ReadModel->Mem ) ;
		Mesh->Tangents = Mesh->Binormals + Mesh->NormalNum ;
	}

	// 全ての面の数だけ繰り返し
	Face = Mesh->Faces ;
	for( i = 0 ; ( DWORD )i < Mesh->FaceNum ; i ++, Face ++ )
	{
		v1.x = Mesh->Positions[ Face->VertexIndex[ 1 ] ].x - Mesh->Positions[ Face->VertexIndex[ 0 ] ].x ;
		v1.y = Mesh->UVs[ 0 ][ Face->UVIndex[ 0 ][ 1 ] ].x - Mesh->UVs[ 0 ][ Face->UVIndex[ 0 ][ 0 ] ].x ;
		v1.z = Mesh->UVs[ 0 ][ Face->UVIndex[ 0 ][ 1 ] ].y - Mesh->UVs[ 0 ][ Face->UVIndex[ 0 ][ 0 ] ].y ;

		v2.x = Mesh->Positions[ Face->VertexIndex[ 2 ] ].x - Mesh->Positions[ Face->VertexIndex[ 0 ] ].x ;
		v2.y = Mesh->UVs[ 0 ][ Face->UVIndex[ 0 ][ 2 ] ].x - Mesh->UVs[ 0 ][ Face->UVIndex[ 0 ][ 0 ] ].x ;
		v2.z = Mesh->UVs[ 0 ][ Face->UVIndex[ 0 ][ 2 ] ].y - Mesh->UVs[ 0 ][ Face->UVIndex[ 0 ][ 0 ] ].y ;

		vt = VCross( v1, v2 ) ;
		du.x = 1.0f ;
		if( VDot( vt, vt ) >= 0.0000001f )
		{
			du.x = -vt.y / vt.x ;
			dv.x = -vt.z / vt.x ;
		}

		v1.x = Mesh->Positions[ Face->VertexIndex[ 1 ] ].y - Mesh->Positions[ Face->VertexIndex[ 0 ] ].y ;
		v2.x = Mesh->Positions[ Face->VertexIndex[ 2 ] ].y - Mesh->Positions[ Face->VertexIndex[ 0 ] ].y ;

		vt = VCross( v1, v2 ) ;
		du.y = 1.0f ;
		if( VDot( vt, vt ) >= 0.0000001f )
		{
			du.y = -vt.y / vt.x ;
			dv.y = -vt.z / vt.x ;
		}

		v1.x = Mesh->Positions[ Face->VertexIndex[ 1 ] ].z - Mesh->Positions[ Face->VertexIndex[ 0 ] ].z ;
		v2.x = Mesh->Positions[ Face->VertexIndex[ 2 ] ].z - Mesh->Positions[ Face->VertexIndex[ 0 ] ].z ;

		vt = VCross( v1, v2 ) ;
		du.z = 1.0f ;
		if( VDot( vt, vt ) >= 0.0000001f )
		{
			du.z = -vt.y / vt.x ;
			dv.z = -vt.z / vt.x ;
		}

		VectorAdd( &Mesh->Tangents[ Face->NormalIndex[ 0 ] ], &Mesh->Tangents[ Face->NormalIndex[ 0 ] ], &du ) ;
		VectorAdd( &Mesh->Tangents[ Face->NormalIndex[ 1 ] ], &Mesh->Tangents[ Face->NormalIndex[ 1 ] ], &du ) ;
		VectorAdd( &Mesh->Tangents[ Face->NormalIndex[ 2 ] ], &Mesh->Tangents[ Face->NormalIndex[ 2 ] ], &du ) ;

		VectorAdd( &Mesh->Binormals[ Face->NormalIndex[ 0 ] ], &Mesh->Binormals[ Face->NormalIndex[ 0 ] ], &dv ) ;
		VectorAdd( &Mesh->Binormals[ Face->NormalIndex[ 1 ] ], &Mesh->Binormals[ Face->NormalIndex[ 1 ] ], &dv ) ;
		VectorAdd( &Mesh->Binormals[ Face->NormalIndex[ 2 ] ], &Mesh->Binormals[ Face->NormalIndex[ 2 ] ], &dv ) ;
	}

	// 法線の算出と正規化
	for( i = 0 ; ( DWORD )i < Mesh->NormalNum ; i ++ )
	{
		vt = VNorm( Mesh->Tangents[ i ] ) ;
		vn = VNorm( VCross( vt, Mesh->Binormals[ i ] ) ) ;
		vb = VNorm( VCross( vn, vt ) ) ;

		// 法線の方向を確認
		if( VDot( vn, Mesh->Normals[ i ] ) < 0.0f )
		{
			vn.x = -vn.x ;
			vn.y = -vn.y ;
			vn.z = -vn.z ;
		}

		// 正規化
		Mesh->Tangents[ i ]  = vt ;
		Mesh->Binormals[ i ] = vb ;
//		Mesh->Normals[ i ]   = vn ;
	}

	// 終了
	return 0 ;
}


// モデルデータのスキニングメッシュ用のボーン情報を最適化する
static bool MV1OptimizeSkinBoneInfo( MV1_MODEL_BASE *Model )
{
	int AfterNum ;
	int AfterUseFrameNum ;
	int *AfterUseFrame ;
	MV1_SKIN_BONE_USE_FRAME *TempUseFrame ;
	MV1_SKIN_BONE *AfterSkin, *TASK, *SK1, *SK2, *ASK, TempSkin ;
	MV1_FRAME_BASE *Frame ;
	int *BeginToAfterIndex, *LostIndex, i, j, k, *BeginIndex, *AfterIndex ;
	bool Change ;

	if( Model->SkinBoneNum == 0 )
		return true ;

	AfterSkin = ( MV1_SKIN_BONE * )DXALLOC( ( sizeof( int ) + sizeof( MV1_SKIN_BONE_USE_FRAME ) ) * Model->SkinBoneUseFrameNum + ( sizeof( MV1_SKIN_BONE ) + sizeof( int ) ) * Model->SkinBoneNum * 4 ) ;
	if( AfterSkin == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x4d\x00\x6f\x00\x64\x00\x65\x00\x6c\x00\x20\x00\x4f\x00\x70\x00\x74\x00\x69\x00\x6d\x00\x69\x00\x7a\x00\x65\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xb9\x30\xad\x30\xcb\x30\xf3\x30\xb0\x30\xe1\x30\xc3\x30\xb7\x30\xe5\x30\xc5\x60\x31\x58\x00\x67\x69\x90\x16\x53\xe6\x51\x06\x74\x67\x30\x7f\x4f\x28\x75\x59\x30\x8b\x30\xc6\x30\xf3\x30\xdd\x30\xe9\x30\xea\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Model Optimize Error : スキニングメッシュ情報最適化処理で使用するテンポラリバッファの確保に失敗しました\n" @*/ )) ;
		return false ;
	}
	BeginToAfterIndex = ( int * )( AfterSkin + Model->SkinBoneNum ) ;
	LostIndex         = BeginToAfterIndex + Model->SkinBoneNum ;
	AfterIndex        = LostIndex         + Model->SkinBoneNum ;
	BeginIndex        = AfterIndex        + Model->SkinBoneNum ;
	AfterUseFrame     = ( int                     * )( BeginIndex    + Model->SkinBoneNum         ) ;
	TempUseFrame      = ( MV1_SKIN_BONE_USE_FRAME * )( AfterUseFrame + Model->SkinBoneUseFrameNum ) ;
	_MEMSET( BeginToAfterIndex,    0, sizeof( int ) * Model->SkinBoneNum         ) ;
	_MEMSET( LostIndex,            0, sizeof( int ) * Model->SkinBoneNum         ) ;
	_MEMSET( AfterUseFrame,     0xff, sizeof( int ) * Model->SkinBoneUseFrameNum ) ;
	_MEMCPY( TempUseFrame, Model->SkinBoneUseFrame, sizeof( MV1_SKIN_BONE_USE_FRAME ) * Model->SkinBoneUseFrameNum ) ;

	// 同一内容のボーン情報を削除した上で、ボーンデータをフレーム順に並べ替える
	{
		// 同一内容の情報が無いか調べながらテーブルを作成
		AfterNum = 0 ;
		SK1 = Model->SkinBone ;
		ASK = AfterSkin ;
		Change = false ;
		for( i = 0 ; i < Model->SkinBoneNum ; i ++, SK1 ++ )
		{
			SK2 = Model->SkinBone ;
			k = 0 ;
			for( j = 0 ; j < i ; j ++, SK2 ++ )
			{
				if( LostIndex[ j ] == 1 ) continue ;

				if( SK1->BoneFrame == SK2->BoneFrame &&
					_MEMCMP( &SK1->ModelLocalMatrix, &SK2->ModelLocalMatrix, sizeof( MATRIX_4X4CT ) ) == 0 )
					break ;
				k ++ ;
			}
			if( j == i )
			{
				*ASK = *SK1 ;
				ASK->UseFrameNum = 0 ;
				BeginIndex[ AfterNum ] = AfterNum ;
				AfterIndex[ AfterNum ] = AfterNum ;
				ASK ++ ;
				AfterNum ++ ;
			}
			else
			{
				LostIndex[ i ] = 1 ;
				Change = true ;
			}
			BeginToAfterIndex[ i ] = k ;

			TASK = &AfterSkin[ k ] ;
			TASK->UseFrameNum += SK1->UseFrameNum ;
			for( j = 0 ; j < SK1->UseFrameNum ; j ++ )
				AfterUseFrame[ SK1->UseFrame - Model->SkinBoneUseFrame + j ] = k ;
		}

		// ボーンデータを対象フレーム順に並べ替える
		for( i = 0 ; i < AfterNum ; i ++ )
		{
			for( j = i + 1 ; j < AfterNum ; j ++ )
			{
				if( AfterSkin[ i ].BoneFrame > AfterSkin[ j ].BoneFrame )
				{
					Change = true ;

					TempSkin = AfterSkin[ i ] ;
					AfterSkin[ i ] = AfterSkin[ j ] ;
					AfterSkin[ j ] = TempSkin ;

					k = BeginIndex[ i ] ;
					BeginIndex[ i ] = BeginIndex[ j ] ;
					BeginIndex[ j ] = k ;

					AfterIndex[ BeginIndex[ i ] ] = i ;
					AfterIndex[ BeginIndex[ j ] ] = j ;
				}
			}
		}

		// 数の変更が無い場合は何もしない
		if( Change )
		{
			// スキニングメッシュの数と情報を更新する
			Model->SkinBoneNum = AfterNum ;
			_MEMCPY( Model->SkinBone, AfterSkin, sizeof( MV1_SKIN_BONE ) * Model->SkinBoneNum ) ;

			// ボーンを使用するフレームの配列のアドレスを割り当てる
			SK1 = Model->SkinBone ;
			AfterUseFrameNum = 0 ;
			for( i = 0 ; i < AfterNum ; i ++, SK1 ++ )
			{
				SK1->UseFrame = Model->SkinBoneUseFrame + AfterUseFrameNum ;
				AfterUseFrameNum += SK1->UseFrameNum ;
				SK1->UseFrameNum = 0 ;
			}

			// ボーンを使用するフレームの配列をセットする
			for( i = 0 ; i < Model->SkinBoneUseFrameNum ; i ++ )
			{
				if( AfterUseFrame[ i ] == -1 ) continue ;
				SK1 = &Model->SkinBone[ AfterIndex[ AfterUseFrame[ i ] ] ] ;
				SK1->UseFrame[ SK1->UseFrameNum ] = TempUseFrame[ i ] ;

				// フレームの方も更新
				Model->Frame[ TempUseFrame[ i ].Index ].UseSkinBone[ TempUseFrame[ i ].MatrixIndex ] = SK1 ;

				SK1->UseFrameNum ++ ;
			}
		}

		// フレームに関連するボーンのアドレスをセットする
		SK1 = Model->SkinBone ;
		for( i = 0 ; i < Model->SkinBoneNum ; i ++, SK1 ++ )
		{
			Frame = &Model->Frame[ SK1->BoneFrame ] ;
			if( Frame->SkinBoneNum == 0 )
				Frame->SkinBone = SK1 ;
			Frame->SkinBoneNum ++ ;
		}
	}

	// 確保していたメモリの解放
	DXFREE( AfterSkin ) ;
	AfterSkin = NULL ;

	return true ;
}

// アニメーションデータを最適化する
static bool MV1OptimizeAnim( MV1_MODEL_R *ReadModel )
{
	MV1_ANIM_R           *AnimR ;
	MV1_ANIMKEYSET_R     *KeySetR ;
//	MV1_ANIM_KEY_16BIT_F  TimeSub ;
//	MV1_ANIM_KEY_16BIT_F  KeySub ;
	int                   i ;
	int                   j ;
	int                   k ;
	int                   l ;
//	float                *Time ;
	float                 f ;
//	WORD                 *NewTimeB16 ;

#if 0 // 行列にXYZで異なるスケールがされていると正常に処理できないので解決の目処が立つまでコメントアウト
	// MV1_ANIMKEY_TYPE_MATRIX4X4C を MV1_ANIMKEY_TYPE_MATRIX3X3 と MV1_ANIMKEY_TYPE_VECTOR に分解する
	if( ReadModel->AnimDataNotDecomposition == FALSE )
	{
		AnimR = ReadModel->AnimFirst ;
		for( i = 0 ; ( DWORD )i < ReadModel->AnimNum ; i ++, AnimR = AnimR->DataNext )
		{
			KeySetR = AnimR->AnimKeySetFirst ;
			for( j = 0 ; j < AnimR->AnimKeySetNum ; j ++, KeySetR = KeySetR->Next )
			{
				switch( KeySetR->DataType )
				{
				case MV1_ANIMKEY_DATATYPE_MATRIX4X4C :
					{
						MV1_ANIM_KEY_MATRIX4X4C *Key ;
						MV1_ANIM_KEY_MATRIX3X3 *NewKeyMat3X3 ;
						MV1_ANIMKEYSET_R *NewKeySetR ;
						VECTOR *NewKeyTrans ;

						NewKeyTrans = ( VECTOR * )ADDMEMAREA( ( sizeof( VECTOR ) + sizeof( float ) ) * KeySetR->Num, &ReadModel->Mem ) ;
						if( NewKeyTrans )
						{
							// 平行移動用のキーセットを追加する
							NewKeySetR = MV1RAddAnimKeySet( ReadModel, AnimR ) ;
							if( NewKeySetR )
							{
								// 増える分のデータサイズを加算する
								ReadModel->AnimKeyDataSize -= ( sizeof( MV1_ANIM_KEY_MATRIX4X4C ) + sizeof( float ) ) * KeySetR->Num ;
								ReadModel->AnimKeyDataSize += ( sizeof( MV1_ANIM_KEY_MATRIX3X3 ) + sizeof( VECTOR ) + sizeof( float ) * 2 ) * KeySetR->Num ;

								// 新しいバッファを３×３行列のデータを格納するのにつかい
								NewKeySetR->Type = MV1_ANIMKEY_TYPE_MATRIX3X3 ;
								NewKeySetR->DataType = MV1_ANIMKEY_DATATYPE_MATRIX3X3 ;
								NewKeySetR->TotalTime = KeySetR->TotalTime ;
								NewKeySetR->Num = KeySetR->Num ;
								NewKeySetR->KeyMatrix3x3 = KeySetR->KeyMatrix3x3 ;
								NewKeySetR->KeyTime = ( float * )( NewKeyTrans + KeySetR->Num ) ;
								_MEMCPY( NewKeySetR->KeyTime, KeySetR->KeyTime, sizeof( float ) * KeySetR->Num ) ;

								// 既存のバッファを平行移動のデータを格納するのに使う
								KeySetR->Type = MV1_ANIMKEY_TYPE_VECTOR ;
								KeySetR->DataType = MV1_ANIMKEY_DATATYPE_TRANSLATE ;
								KeySetR->KeyVector = NewKeyTrans ;

								Key  = NewKeySetR->KeyMatrix4x4C ;
								NewKeyTrans = KeySetR->KeyVector ;
								NewKeyMat3X3 = NewKeySetR->KeyMatrix3x3 ;
								for( k = 0 ; k < KeySetR->Num ; k ++, NewKeyTrans ++, NewKeyMat3X3 ++, Key ++ )
								{
									NewKeyMat3X3->Matrix[ 0 ][ 0 ] = Key->Matrix[ 0 ][ 0 ] ;
									NewKeyMat3X3->Matrix[ 0 ][ 1 ] = Key->Matrix[ 0 ][ 1 ] ;
									NewKeyMat3X3->Matrix[ 0 ][ 2 ] = Key->Matrix[ 0 ][ 2 ] ;
									NewKeyMat3X3->Matrix[ 1 ][ 0 ] = Key->Matrix[ 1 ][ 0 ] ;
									NewKeyMat3X3->Matrix[ 1 ][ 1 ] = Key->Matrix[ 1 ][ 1 ] ;
									NewKeyMat3X3->Matrix[ 1 ][ 2 ] = Key->Matrix[ 1 ][ 2 ] ;
									NewKeyMat3X3->Matrix[ 2 ][ 0 ] = Key->Matrix[ 2 ][ 0 ] ;
									NewKeyMat3X3->Matrix[ 2 ][ 1 ] = Key->Matrix[ 2 ][ 1 ] ;
									NewKeyMat3X3->Matrix[ 2 ][ 2 ] = Key->Matrix[ 2 ][ 2 ] ;
									NewKeyTrans->x = Key->Matrix[ 3 ][ 0 ] ;
									NewKeyTrans->y = Key->Matrix[ 3 ][ 1 ] ;
									NewKeyTrans->z = Key->Matrix[ 3 ][ 2 ] ;
								}
							}
						}
					}
					break ;
				}
			}
		}
	}


	// MV1_ANIMKEY_TYPE_MATRIX3X3 タイプの MV1_ANIMKEY_DATATYPE_MATRIX3X3 を
	// MV1_ANIMKEY_TYPE_QUATERNION_VMD タイプの MV1_ANIMKEY_DATATYPE_ROTATE と
	// MV1_ANIMKEY_TYPE_VECTOR タイプの MV1_ANIMKEY_DATATYPE_SCALE に変換する
	if( ReadModel->AnimDataNotDecomposition == FALSE )
	{
		AnimR = ReadModel->AnimFirst ;
		for( i = 0 ; ( DWORD )i < ReadModel->AnimNum ; i ++, AnimR = AnimR->DataNext )
		{
			KeySetR = AnimR->AnimKeySetFirst ;
			for( j = 0 ; j < AnimR->AnimKeySetNum ; j ++, KeySetR = KeySetR->Next )
			{
				FLOAT4 *NewQKey, *NewBuffer ;
				VECTOR *NewSKey ;
				MV1_ANIM_KEY_MATRIX3X3 *Key ;
				MV1_ANIMKEYSET_R *NewKeySetR ;
				int Error, SizeValid ;

				if( KeySetR->DataType != MV1_ANIMKEY_DATATYPE_MATRIX3X3 ||
					KeySetR->Type     != MV1_ANIMKEY_TYPE_MATRIX3X3 )
					continue ;

				// クォータニオンを使用した回転値に変換する

				// キーサイズを減らして増やす
				ReadModel->AnimKeyDataSize -= ( sizeof( MV1_ANIM_KEY_MATRIX3X3 ) ) * KeySetR->Num ;
				ReadModel->AnimKeyDataSize += ( sizeof( FLOAT4 )                 ) * KeySetR->Num ;

				// 新しいキーを一時的に格納するメモリ領域の確保
				NewBuffer = ( FLOAT4 * )DXALLOC( ( sizeof( VECTOR ) + sizeof( FLOAT4 ) ) * KeySetR->Num ) ;
				if( NewBuffer )
				{
					// タイプを変更
					KeySetR->Type = MV1_ANIMKEY_TYPE_QUATERNION_VMD ;
					KeySetR->DataType = MV1_ANIMKEY_DATATYPE_ROTATE ;

					// 回転オーダーをセット
					AnimR->RotateOrder = MV1_ROTATE_ORDER_XYZ ;

					// 値を変換
					Key    = KeySetR->KeyMatrix3x3 ;
					NewQKey = NewBuffer ;
					NewSKey = ( VECTOR * )( NewBuffer + KeySetR->Num ) ;
					Error = 0 ;
					SizeValid = 0 ;
					for( l = 0 ; Error != -1 && l < KeySetR->Num ; l ++, Key ++, NewQKey ++, NewSKey ++ )
					{
						VECTOR Size ;
						float s, trace ;

						NewSKey->x = _SQRT( Key->Matrix[ 0 ][ 0 ] * Key->Matrix[ 0 ][ 0 ] + Key->Matrix[ 1 ][ 0 ] * Key->Matrix[ 1 ][ 0 ] + Key->Matrix[ 2 ][ 0 ] * Key->Matrix[ 2 ][ 0 ] ) ;
						NewSKey->y = _SQRT( Key->Matrix[ 0 ][ 1 ] * Key->Matrix[ 0 ][ 1 ] + Key->Matrix[ 1 ][ 1 ] * Key->Matrix[ 1 ][ 1 ] + Key->Matrix[ 2 ][ 1 ] * Key->Matrix[ 2 ][ 1 ] ) ;
						NewSKey->z = _SQRT( Key->Matrix[ 0 ][ 2 ] * Key->Matrix[ 0 ][ 2 ] + Key->Matrix[ 1 ][ 2 ] * Key->Matrix[ 1 ][ 2 ] + Key->Matrix[ 2 ][ 2 ] * Key->Matrix[ 2 ][ 2 ] ) ;
						Size.x = 1.0f / NewSKey->x ;
						Size.y = 1.0f / NewSKey->y ;
						Size.z = 1.0f / NewSKey->z ;
						if( NewSKey->x < 0.999f || NewSKey->x > 1.0001f ||
							NewSKey->y < 0.999f || NewSKey->y > 1.0001f ||
							NewSKey->z < 0.999f || NewSKey->z > 1.0001f )
						{
							SizeValid = 1 ;
						}

						// 正規化
						Key->Matrix[ 0 ][ 0 ] *= Size.x ;
						Key->Matrix[ 1 ][ 0 ] *= Size.x ;
						Key->Matrix[ 2 ][ 0 ] *= Size.x ;

						Key->Matrix[ 0 ][ 1 ] *= Size.y ;
						Key->Matrix[ 1 ][ 1 ] *= Size.y ;
						Key->Matrix[ 2 ][ 1 ] *= Size.y ;

						Key->Matrix[ 0 ][ 2 ] *= Size.z ;
						Key->Matrix[ 1 ][ 2 ] *= Size.z ;
						Key->Matrix[ 2 ][ 2 ] *= Size.z ;

						trace = Key->Matrix[ 0 ][ 0 ] + Key->Matrix[ 1 ][ 1 ] + Key->Matrix[ 2 ][ 2 ] + 1.0f ;
						if( trace >= 1.0f )
						{
							s = 0.5f / _SQRT( trace ) ;
							NewQKey->w = 0.25f / s ;
							NewQKey->x = ( Key->Matrix[ 1 ][ 2 ] - Key->Matrix[ 2 ][ 1 ] ) * s ;
							NewQKey->y = ( Key->Matrix[ 2 ][ 0 ] - Key->Matrix[ 0 ][ 2 ] ) * s ;
							NewQKey->z = ( Key->Matrix[ 0 ][ 1 ] - Key->Matrix[ 1 ][ 0 ] ) * s ;
						}
						else
						{
							float Max ;
							Max = Key->Matrix[ 1 ][ 1 ] > Key->Matrix[ 2 ][ 2 ] ? Key->Matrix[ 1 ][ 1 ] : Key->Matrix[ 2 ][ 2 ] ;
							if( Max < Key->Matrix[ 0 ][ 0 ] )
							{
								s = _SQRT( Key->Matrix[ 0 ][ 0 ] - ( Key->Matrix[ 1 ][ 1 ] + Key->Matrix[ 2 ][ 2 ] ) + 1.0f ) ;
								NewQKey->x = s * 0.5f ;
								s = 0.5f / s ;
								NewQKey->y = ( Key->Matrix[ 0 ][ 1 ] + Key->Matrix[ 1 ][ 0 ] ) * s ;
								NewQKey->z = ( Key->Matrix[ 2 ][ 0 ] + Key->Matrix[ 0 ][ 2 ] ) * s ;
								NewQKey->w = ( Key->Matrix[ 1 ][ 2 ] - Key->Matrix[ 2 ][ 1 ] ) * s ;
							}
							else
							if( Max == Key->Matrix[ 1 ][ 1 ] )
							{
								s = _SQRT( Key->Matrix[ 1 ][ 1 ] - ( Key->Matrix[ 2 ][ 2 ] + Key->Matrix[ 0 ][ 0 ] ) + 1.0f ) ;
								NewQKey->y = s * 0.5f ;
								s = 0.5f / s ;
								NewQKey->x = ( Key->Matrix[ 0 ][ 1 ] + Key->Matrix[ 1 ][ 0 ] ) * s ;
								NewQKey->z = ( Key->Matrix[ 1 ][ 2 ] + Key->Matrix[ 2 ][ 1 ] ) * s ;
								NewQKey->w = ( Key->Matrix[ 2 ][ 0 ] - Key->Matrix[ 0 ][ 2 ] ) * s ;
							}
							else
							{
								s = _SQRT( Key->Matrix[ 2 ][ 2 ] - ( Key->Matrix[ 0 ][ 0 ] + Key->Matrix[ 1 ][ 1 ] ) + 1.0f ) ;
								NewQKey->z = s * 0.5f ;
								s = 0.5f / s ;
								NewQKey->x = ( Key->Matrix[ 2 ][ 0 ] + Key->Matrix[ 0 ][ 2 ] ) * s ;
								NewQKey->y = ( Key->Matrix[ 1 ][ 2 ] + Key->Matrix[ 2 ][ 1 ] ) * s ;
								NewQKey->w = ( Key->Matrix[ 0 ][ 1 ] - Key->Matrix[ 1 ][ 0 ] ) * s ;
							}
						}
					}

					// クォータニオンの内容をコピー
					_MEMCPY( KeySetR->KeyFloat4, NewBuffer, sizeof( FLOAT4 ) * KeySetR->Num ) ;

					// サイズキーが有効だった場合はサイズキーセットを追加する
					if( SizeValid )
					{
						NewKeySetR = MV1RAddAnimKeySet( ReadModel, AnimR ) ;
						if( NewKeySetR )
						{
							// 増える分のデータサイズを加算する
							ReadModel->AnimKeyDataSize += ( sizeof( float ) + sizeof( VECTOR ) ) * KeySetR->Num ;

							// キーセット情報をセット
							NewKeySetR->Type = MV1_ANIMKEY_TYPE_VECTOR ;
							NewKeySetR->DataType = MV1_ANIMKEY_DATATYPE_SCALE ;
							NewKeySetR->TotalTime = KeySetR->TotalTime ;
							NewKeySetR->Num = KeySetR->Num ;
							NewKeySetR->KeyVector = ( VECTOR * )( KeySetR->KeyFloat4 + KeySetR->Num ) ;
							NewKeySetR->KeyTime = ( float * )( NewKeySetR->KeyVector + KeySetR->Num ) ;
							_MEMCPY( NewKeySetR->KeyTime, KeySetR->KeyTime, sizeof( float ) * KeySetR->Num ) ;

							// キー情報をバッファからコピー
							_MEMCPY( NewKeySetR->KeyVector, NewBuffer + KeySetR->Num, sizeof( VECTOR ) * KeySetR->Num ) ;
						}
					}

					// 一時的に確保したメモリを解放する
					DXFREE( NewBuffer ) ;
				}

				break ;
			}
		}
	}
#endif 


	// MV1_ANIMKEY_TYPE_MATRIX3X3 タイプの MV1_ANIMKEY_DATATYPE_MATRIX3X3 を
	// MV1_ANIMKEY_TYPE_VECTOR タイプの MV1_ANIMKEY_DATATYPE_ROTATE に変換する
	AnimR = ReadModel->AnimFirst ;
	for( i = 0 ; ( DWORD )i < ReadModel->AnimNum ; i ++, AnimR = AnimR->DataNext )
	{
		KeySetR = AnimR->AnimKeySetFirst ;
		for( j = 0 ; j < AnimR->AnimKeySetNum ; j ++, KeySetR = KeySetR->Next )
		{
			VECTOR *NewKey, *NewBuffer ;
			MV1_ANIM_KEY_MATRIX3X3 *Key ;
			MATRIX TempMat ;
			int Error ;

			if( KeySetR->DataType != MV1_ANIMKEY_DATATYPE_MATRIX3X3 ||
				KeySetR->Type     != MV1_ANIMKEY_TYPE_MATRIX3X3 )
				continue ;

			// 回転値に変換する

			// キーサイズを減らして増やす
			ReadModel->AnimKeyDataSize -= ( sizeof( MV1_ANIM_KEY_MATRIX3X3 ) ) * KeySetR->Num ;
			ReadModel->AnimKeyDataSize += ( sizeof( VECTOR )                   ) * KeySetR->Num ;

			// 新しいキーを一時的に格納するメモリ領域の確保
			NewBuffer = ( VECTOR * )DXALLOC( sizeof( VECTOR ) * KeySetR->Num ) ;
			if( NewBuffer )
			{
				// ジンバルロックが発生しない回転オーダーを検出する
				for( k = 0 ; k < 6 ; k ++ )
				{
					// 値を変換
					Key    = KeySetR->KeyMatrix3x3 ;
					NewKey = NewBuffer ;
					Error = 0 ;
					for( l = 0 ; Error != -1 && l < KeySetR->Num ; l ++, Key ++, NewKey ++ )
					{
						TempMat.m[ 0 ][ 0 ] = Key->Matrix[ 0 ][ 0 ] ;
						TempMat.m[ 0 ][ 1 ] = Key->Matrix[ 0 ][ 1 ] ;
						TempMat.m[ 0 ][ 2 ] = Key->Matrix[ 0 ][ 2 ] ;
						TempMat.m[ 0 ][ 3 ] = 0.0f ;
						TempMat.m[ 1 ][ 0 ] = Key->Matrix[ 1 ][ 0 ] ;
						TempMat.m[ 1 ][ 1 ] = Key->Matrix[ 1 ][ 1 ] ;
						TempMat.m[ 1 ][ 2 ] = Key->Matrix[ 1 ][ 2 ] ;
						TempMat.m[ 1 ][ 3 ] = 0.0f ;
						TempMat.m[ 2 ][ 0 ] = Key->Matrix[ 2 ][ 0 ] ;
						TempMat.m[ 2 ][ 1 ] = Key->Matrix[ 2 ][ 1 ] ;
						TempMat.m[ 2 ][ 2 ] = Key->Matrix[ 2 ][ 2 ] ;
						TempMat.m[ 2 ][ 3 ] = 0.0f ;
						TempMat.m[ 3 ][ 0 ] = 0.0f ;
						TempMat.m[ 3 ][ 1 ] = 0.0f ;
						TempMat.m[ 3 ][ 2 ] = 0.0f ;
						TempMat.m[ 3 ][ 3 ] = 1.0f ;
						switch( k )
						{
						case MV1_ROTATE_ORDER_XYZ : Error = GetMatrixXYZRotation( &TempMat, &NewKey->x, &NewKey->y, &NewKey->z ) ; break ;
						case MV1_ROTATE_ORDER_XZY : Error = GetMatrixXZYRotation( &TempMat, &NewKey->x, &NewKey->y, &NewKey->z ) ; break ;
						case MV1_ROTATE_ORDER_YXZ : Error = GetMatrixYXZRotation( &TempMat, &NewKey->x, &NewKey->y, &NewKey->z ) ; break ;
						case MV1_ROTATE_ORDER_YZX : Error = GetMatrixYZXRotation( &TempMat, &NewKey->x, &NewKey->y, &NewKey->z ) ; break ;
						case MV1_ROTATE_ORDER_ZXY : Error = GetMatrixZXYRotation( &TempMat, &NewKey->x, &NewKey->y, &NewKey->z ) ; break ;
						case MV1_ROTATE_ORDER_ZYX : Error = GetMatrixZYXRotation( &TempMat, &NewKey->x, &NewKey->y, &NewKey->z ) ; break ;
						}
					}
					if( Error == 0 || k == 5 ) break ;
				}

				// タイプを変更
				KeySetR->Type = MV1_ANIMKEY_TYPE_VECTOR ;
				KeySetR->DataType = MV1_ANIMKEY_DATATYPE_ROTATE ;

				// 回転オーダーをセット
				AnimR->RotateOrder = k ;

				// バッファの内容をコピーして一時的に確保したメモリを解放する
				_MEMCPY( KeySetR->KeyVector, NewBuffer, sizeof( VECTOR ) * KeySetR->Num ) ;
				DXFREE( NewBuffer ) ;

				// 回転値の飛びを修正する
				NewKey = KeySetR->KeyVector + 1 ;
				for( l = 1 ; l < KeySetR->Num ; l ++, NewKey ++ )
				{
					f = NewKey->x - NewKey[ -1 ].x ;
					if( f >  ( float )DX_PI / 2.0f )
					{
						NewKey->x -= ( float )DX_PI * 2 ;
					}
					else
					if( f < -( float )DX_PI / 2.0f )
					{
						NewKey->x += ( float )DX_PI * 2 ;
					}

					f = NewKey->y - NewKey[ -1 ].y ;
					if( f >  ( float )DX_PI / 2.0f )
					{
						NewKey->y -= ( float )DX_PI * 2 ;
					}
					else
					if( f < -( float )DX_PI / 2.0f )
					{
						NewKey->y += ( float )DX_PI * 2 ;
					}

					f = NewKey->z - NewKey[ -1 ].z ;
					if( f >  ( float )DX_PI / 2.0f )
					{
						NewKey->z -= ( float )DX_PI * 2 ;
					}
					else
					if( f < -( float )DX_PI / 2.0f )
					{
						NewKey->z += ( float )DX_PI * 2 ;
					}
				}
			}
		}
	}

	// 線形補間で済むデータを削除するか、減らせる数が少なく一定間隔で並ぶキーが多い場合は固定間隔にする
	AnimR = ReadModel->AnimFirst ;
	for( i = 0 ; ( DWORD )i < ReadModel->AnimNum ; i ++, AnimR = AnimR->DataNext )
	{
		KeySetR = AnimR->AnimKeySetFirst ;
		for( j = 0 ; j < AnimR->AnimKeySetNum ; j ++, KeySetR = KeySetR->Next )
		{
			MV1OptimizeKeySetTimeType( ReadModel, KeySetR, true ) ;
		}
	}

	// MV1_ANIMKEY_TYPE_VECTOR タイプのキーを分解できるか( 分解した方がデータサイズが小さくなるか )調べ
	// 分解した方が良い場合は分解する
	if( ReadModel->AnimDataNotDecomposition == FALSE )
	{
		AnimR = ReadModel->AnimFirst ;
		for( i = 0 ; ( DWORD )i < ReadModel->AnimNum ; i ++, AnimR = AnimR->DataNext )
		{
			// 行列から回転ベクトルにした場合は要素単位で分解しない
			if( AnimR->Container->IsMatrixLinearBlend ) continue ;

			KeySetR = AnimR->AnimKeySetFirst ;
			for( j = 0 ; j < AnimR->AnimKeySetNum ; j ++, KeySetR = KeySetR->Next )
			{
				bool ix, iy, iz, comp ;
				int count ;
				VECTOR *Key ;
				int AfterSize, BeginSize ;

				if( KeySetR->Type != MV1_ANIMKEY_TYPE_VECTOR )
					continue ;

				// キーの数が２個よりも少ないときは何もしない
				if( KeySetR->Num < 2 )
					continue ;

				// 値の検証
				ix = true;
				iy = true;
				iz = true;
				Key    = KeySetR->KeyVector + 1 ;
				for( k = 1 ; k < KeySetR->Num ; k ++, Key ++ )
				{
					f = KeySetR->KeyVector[ 0 ].x - Key->x ; if( f < -0.000001f || f > 0.000001f ) ix = false ;
					f = KeySetR->KeyVector[ 0 ].y - Key->y ; if( f < -0.000001f || f > 0.000001f ) iy = false ;
					f = KeySetR->KeyVector[ 0 ].z - Key->z ; if( f < -0.000001f || f > 0.000001f ) iz = false ;
					if( ix == false && iy == false && iz == false ) break ;
				}

				// 一つも効率化できそうもない場合は何もせず終了
				if( ix == false && iy == false && iz == false )
					continue ;

				// 分解した場合の総データサイズと、分解しない場合のサイズを比較して、分解した方が良い場合は分解する
				BeginSize = ( int )( sizeof( VECTOR ) * KeySetR->Num + sizeof( MV1_ANIM_KEYSET_BASE ) ) ;
				if( KeySetR->TimeType == MV1_ANIMKEY_TIME_TYPE_KEY )
					BeginSize += sizeof( float ) * KeySetR->Num ;

				count = 0 ;
				if( ix == false ) count ++ ;
				if( iy == false ) count ++ ;
				if( iz == false ) count ++ ;
				AfterSize = ( int )( sizeof( float ) * KeySetR->Num + sizeof( MV1_ANIM_KEYSET_BASE ) ) ;
				if( KeySetR->TimeType == MV1_ANIMKEY_TIME_TYPE_KEY )
					AfterSize += sizeof( float ) * KeySetR->Num ;
				AfterSize *= count ;

				comp = false ;
	//			if( AfterSize < BeginSize )
				if( 1 )
				{
					MV1_ANIMKEYSET_R *NewKeySetRY, *NewKeySetRZ ;

					NewKeySetRY = NULL ;
					NewKeySetRZ = NULL ;

					NewKeySetRY = MV1RAddAnimKeySet( ReadModel, AnimR ) ;
					if( NewKeySetRY == NULL ) goto BEND ;

					NewKeySetRZ = MV1RAddAnimKeySet( ReadModel, AnimR ) ;
					if( NewKeySetRZ == NULL ) goto BEND ;

					NewKeySetRY->TotalTime = KeySetR->TotalTime ;
					NewKeySetRZ->TotalTime = KeySetR->TotalTime ;
					NewKeySetRY->Type = MV1_ANIMKEY_TYPE_LINEAR ;
					NewKeySetRZ->Type = MV1_ANIMKEY_TYPE_LINEAR ;
					NewKeySetRY->Num = KeySetR->Num ;
					NewKeySetRZ->Num = KeySetR->Num ;
					NewKeySetRY->TimeType = KeySetR->TimeType ;
					NewKeySetRZ->TimeType = KeySetR->TimeType ;
					NewKeySetRY->UnitTime = KeySetR->UnitTime ;
					NewKeySetRZ->UnitTime = KeySetR->UnitTime ;
					NewKeySetRY->StartTime = KeySetR->StartTime ;
					NewKeySetRZ->StartTime = KeySetR->StartTime ;

					if( KeySetR->TimeType == MV1_ANIMKEY_TIME_TYPE_KEY )
					{
						NewKeySetRY->KeyTime = ( float * )ADDMEMAREA( sizeof( float ) * KeySetR->Num * 2, &ReadModel->Mem ) ;
						if( NewKeySetRY->KeyTime == NULL ) goto BEND ;
						NewKeySetRZ->KeyTime = NewKeySetRY->KeyTime + KeySetR->Num ;
						_MEMCPY( NewKeySetRY->KeyTime, KeySetR->KeyTime, sizeof( float ) * KeySetR->Num ) ;
						_MEMCPY( NewKeySetRZ->KeyTime, KeySetR->KeyTime, sizeof( float ) * KeySetR->Num ) ;
					}

					NewKeySetRY->KeyLinear = ( float * )ADDMEMAREA( sizeof( float ) * KeySetR->Num * 4, &ReadModel->Mem ) ;
					if( NewKeySetRY->KeyLinear == NULL ) goto BEND ;
					NewKeySetRZ->KeyLinear = NewKeySetRY->KeyLinear + KeySetR->Num ;

					KeySetR->Type = MV1_ANIMKEY_TYPE_LINEAR ;
					switch( KeySetR->DataType )
					{
					case MV1_ANIMKEY_DATATYPE_ROTATE :
						KeySetR->DataType     = MV1_ANIMKEY_DATATYPE_ROTATE_X ;
						NewKeySetRY->DataType = MV1_ANIMKEY_DATATYPE_ROTATE_Y ;
						NewKeySetRZ->DataType = MV1_ANIMKEY_DATATYPE_ROTATE_Z ;
						break ;

					case MV1_ANIMKEY_DATATYPE_SCALE :
						KeySetR->DataType     = MV1_ANIMKEY_DATATYPE_SCALE_X ;
						NewKeySetRY->DataType = MV1_ANIMKEY_DATATYPE_SCALE_Y ;
						NewKeySetRZ->DataType = MV1_ANIMKEY_DATATYPE_SCALE_Z ;
						break ;

					case MV1_ANIMKEY_DATATYPE_TRANSLATE :
						KeySetR->DataType     = MV1_ANIMKEY_DATATYPE_TRANSLATE_X ;
						NewKeySetRY->DataType = MV1_ANIMKEY_DATATYPE_TRANSLATE_Y ;
						NewKeySetRZ->DataType = MV1_ANIMKEY_DATATYPE_TRANSLATE_Z ;
						break ;
					}

					for( k = 0 ; k < KeySetR->Num ; k ++ )
					{
						KeySetR->KeyLinear[ k ]     = KeySetR->KeyVector[ k ].x ;
						NewKeySetRY->KeyLinear[ k ] = KeySetR->KeyVector[ k ].y ;
						NewKeySetRZ->KeyLinear[ k ] = KeySetR->KeyVector[ k ].z ;
					}

					// キーデータのサイズを減らして増やす
					ReadModel->AnimKeyDataSize -= sizeof( float ) * KeySetR->Num ;
					ReadModel->AnimKeyDataSize += sizeof( float ) * KeySetR->Num * 3 ;

					// 増えたキー共々最適化する
					MV1OptimizeKeySetTimeType( ReadModel, KeySetR ) ;
					MV1OptimizeKeySetTimeType( ReadModel, NewKeySetRY ) ;
					MV1OptimizeKeySetTimeType( ReadModel, NewKeySetRZ ) ;

					comp = true ;

	BEND :
					if( comp == false )
					{
						if( NewKeySetRY )
						{
							KEYSETLIST_SUB( NewKeySetRY, AnimR, ReadModel ) ;
						}

						if( NewKeySetRZ )
						{
							KEYSETLIST_SUB( NewKeySetRZ, AnimR, ReadModel ) ;
						}
					}
				}
			}
		}
	}

	// もう一度線形補間で済むデータを削除するか、減らせる数が少なく一定間隔で並ぶキーが多い場合は固定間隔にする
	AnimR = ReadModel->AnimFirst ;
	for( i = 0 ; ( DWORD )i < ReadModel->AnimNum ; i ++, AnimR = AnimR->DataNext )
	{
		KeySetR = AnimR->AnimKeySetFirst ;
		for( j = 0 ; j < AnimR->AnimKeySetNum ; j ++, KeySetR = KeySetR->Next )
		{
			MV1OptimizeKeySetTimeType( ReadModel, KeySetR, true ) ;
		}
	}
	
	// 同じタイプのキーセットが要素毎に分かれていたら一つにする
	AnimR = ReadModel->AnimFirst ;
	for( i = 0 ; ( DWORD )i < ReadModel->AnimNum ; i ++, AnimR = AnimR->DataNext )
	{
		int fx, fy, fz, conv ;
		MV1_ANIMKEYSET_R *kx = NULL, *ky = NULL, *kz = NULL ;

		// キーの数が同じな平行移動キーセットがＸＹＺと揃っていたら一つにする
		KeySetR = AnimR->AnimKeySetFirst ;
		fx = 0 ;
		fy = 0 ;
		fz = 0 ;
		for( j = 0 ; j < AnimR->AnimKeySetNum ; j ++, KeySetR = KeySetR->Next )
		{
			switch( KeySetR->DataType )
			{
			case MV1_ANIMKEY_DATATYPE_TRANSLATE_X : fx = 1 ; kx = KeySetR ; break ;
			case MV1_ANIMKEY_DATATYPE_TRANSLATE_Y : fy = 1 ; ky = KeySetR ; break ;
			case MV1_ANIMKEY_DATATYPE_TRANSLATE_Z : fz = 1 ; kz = KeySetR ; break ;
			}
		}
		if( fx && fy && fz &&
			kx->Num == ky->Num && kx->Num == kz->Num )
		{
			conv = 0 ;
			if( kx->TimeType == MV1_ANIMKEY_TIME_TYPE_ONE &&
				ky->TimeType == MV1_ANIMKEY_TIME_TYPE_ONE &&
				kz->TimeType == MV1_ANIMKEY_TIME_TYPE_ONE )
			{
				if( kx->UnitTime  == ky->UnitTime  && kx->UnitTime  == kz->UnitTime &&
				    kx->StartTime - ky->StartTime > -0.0000001f && kx->StartTime - ky->StartTime < 0.0000001f &&
					kx->StartTime - kz->StartTime > -0.0000001f && kx->StartTime - kz->StartTime < 0.0000001f )
				{
					conv = 1 ;
				}
			}
			else
			if( kx->TimeType == MV1_ANIMKEY_TIME_TYPE_KEY &&
				ky->TimeType == MV1_ANIMKEY_TIME_TYPE_KEY &&
				kz->TimeType == MV1_ANIMKEY_TIME_TYPE_KEY )
			{
				for( j = 0; j < kx->Num; j++ )
				{
					if( *( ( DWORD * )&kx->KeyTime[ j ] ) != *( ( DWORD * )&ky->KeyTime[ j ] ) ||
						*( ( DWORD * )&kx->KeyTime[ j ] ) != *( ( DWORD * )&kz->KeyTime[ j ] ) )
						break ;
				}
				if( j == kx->Num )
				{
					conv = 1 ;
				}
			}

			if( conv )
			{
				VECTOR *NewBuffer ;
				NewBuffer = ( VECTOR * )ADDMEMAREA( sizeof( VECTOR ) * kx->Num, &ReadModel->Mem ) ;
				if( NewBuffer )
				{
					for( j = 0 ; j < kx->Num ; j ++ )
					{
						NewBuffer[ j ].x = kx->KeyLinear[ j ] ;
						NewBuffer[ j ].y = ky->KeyLinear[ j ] ;
						NewBuffer[ j ].z = kz->KeyLinear[ j ] ;
					}

					kx->DataType = MV1_ANIMKEY_DATATYPE_TRANSLATE ;
					kx->Type = MV1_ANIMKEY_TYPE_VECTOR ;
					kx->KeyVector = NewBuffer ;

					KEYSETLIST_SUB( ky, AnimR, ReadModel ) ;
					KEYSETLIST_SUB( kz, AnimR, ReadModel ) ;
				}
			}
		}

		// キーの数が同じな拡大キーセットがＸＹＺと揃っていたら一つにする
		KeySetR = AnimR->AnimKeySetFirst ;
		fx = 0 ;
		fy = 0 ;
		fz = 0 ;
		for( j = 0 ; j < AnimR->AnimKeySetNum ; j ++, KeySetR = KeySetR->Next )
		{
			switch( KeySetR->DataType )
			{
			case MV1_ANIMKEY_DATATYPE_SCALE_X : fx = 1 ; kx = KeySetR ; break ;
			case MV1_ANIMKEY_DATATYPE_SCALE_Y : fy = 1 ; ky = KeySetR ; break ;
			case MV1_ANIMKEY_DATATYPE_SCALE_Z : fz = 1 ; kz = KeySetR ; break ;
			}
		}
		if( fx && fy && fz &&
			kx->Num == ky->Num && kx->Num == kz->Num )
		{
			conv = 0 ;
			if( kx->TimeType == MV1_ANIMKEY_TIME_TYPE_ONE &&
				ky->TimeType == MV1_ANIMKEY_TIME_TYPE_ONE &&
				kz->TimeType == MV1_ANIMKEY_TIME_TYPE_ONE )
			{
				if( kx->UnitTime  == ky->UnitTime               && kx->UnitTime  == kz->UnitTime &&
				    kx->StartTime - ky->StartTime > -0.0000001f && kx->StartTime - ky->StartTime < 0.0000001f &&
					kx->StartTime - kz->StartTime > -0.0000001f && kx->StartTime - kz->StartTime < 0.0000001f )
				{
					conv = 1 ;
				}
			}
			else
			if( kx->TimeType == MV1_ANIMKEY_TIME_TYPE_KEY &&
				ky->TimeType == MV1_ANIMKEY_TIME_TYPE_KEY &&
				kz->TimeType == MV1_ANIMKEY_TIME_TYPE_KEY )
			{
				for( j = 0; j < kx->Num; j++ )
				{
					if( *( ( DWORD * )&kx->KeyTime[ j ] ) != *( ( DWORD * )&ky->KeyTime[ j ] ) ||
						*( ( DWORD * )&kx->KeyTime[ j ] ) != *( ( DWORD * )&kz->KeyTime[ j ] ) )
						break ;
				}
				if( j == kx->Num )
				{
					conv = 1 ;
				}
			}

			if( conv )
			{
				VECTOR *NewBuffer ;
				NewBuffer = ( VECTOR * )ADDMEMAREA( sizeof( VECTOR ) * kx->Num, &ReadModel->Mem ) ;
				if( NewBuffer )
				{
					for( j = 0 ; j < kx->Num ; j ++ )
					{
						NewBuffer[ j ].x = kx->KeyLinear[ j ] ;
						NewBuffer[ j ].y = ky->KeyLinear[ j ] ;
						NewBuffer[ j ].z = kz->KeyLinear[ j ] ;
					}

					kx->DataType = MV1_ANIMKEY_DATATYPE_SCALE ;
					kx->Type = MV1_ANIMKEY_TYPE_VECTOR ;
					kx->KeyVector = NewBuffer ;

					KEYSETLIST_SUB( ky, AnimR, ReadModel ) ;
					KEYSETLIST_SUB( kz, AnimR, ReadModel ) ;
				}
			}
		}

		// キーの数が同じな回転キーセットがＸＹＺと揃っていたら一つにする
		KeySetR = AnimR->AnimKeySetFirst ;
		fx = 0 ;
		fy = 0 ;
		fz = 0 ;
		for( j = 0 ; j < AnimR->AnimKeySetNum ; j ++, KeySetR = KeySetR->Next )
		{
			switch( KeySetR->DataType )
			{
			case MV1_ANIMKEY_DATATYPE_ROTATE_X : fx = 1 ; kx = KeySetR ; break ;
			case MV1_ANIMKEY_DATATYPE_ROTATE_Y : fy = 1 ; ky = KeySetR ; break ;
			case MV1_ANIMKEY_DATATYPE_ROTATE_Z : fz = 1 ; kz = KeySetR ; break ;
			}
		}
		if( fx && fy && fz &&
			kx->Num == ky->Num && kx->Num == kz->Num )
		{
			conv = 0 ;
			if( kx->TimeType == MV1_ANIMKEY_TIME_TYPE_ONE &&
				ky->TimeType == MV1_ANIMKEY_TIME_TYPE_ONE &&
				kz->TimeType == MV1_ANIMKEY_TIME_TYPE_ONE )
			{
				if( kx->UnitTime  == ky->UnitTime  && kx->UnitTime  == kz->UnitTime &&
				    kx->StartTime - ky->StartTime > -0.0000001f && kx->StartTime - ky->StartTime < 0.0000001f &&
					kx->StartTime - kz->StartTime > -0.0000001f && kx->StartTime - kz->StartTime < 0.0000001f )
				{
					conv = 1 ;
				}
			}
			else
			if( kx->TimeType == MV1_ANIMKEY_TIME_TYPE_KEY &&
				ky->TimeType == MV1_ANIMKEY_TIME_TYPE_KEY &&
				kz->TimeType == MV1_ANIMKEY_TIME_TYPE_KEY )
			{
				for( j = 0; j < kx->Num; j++ )
				{
					if( *( ( DWORD * )&kx->KeyTime[ j ] ) != *( ( DWORD * )&ky->KeyTime[ j ] ) ||
						*( ( DWORD * )&kx->KeyTime[ j ] ) != *( ( DWORD * )&kz->KeyTime[ j ] ) )
						break ;
				}
				if( j == kx->Num )
				{
					conv = 1 ;
				}
			}

			if( conv )
			{
				VECTOR *NewBuffer ;
				NewBuffer = ( VECTOR * )ADDMEMAREA( sizeof( VECTOR ) * kx->Num, &ReadModel->Mem ) ;
				if( NewBuffer )
				{
					for( j = 0 ; j < kx->Num ; j ++ )
					{
						NewBuffer[ j ].x = kx->KeyLinear[ j ] ;
						NewBuffer[ j ].y = ky->KeyLinear[ j ] ;
						NewBuffer[ j ].z = kz->KeyLinear[ j ] ;
					}

					kx->DataType = MV1_ANIMKEY_DATATYPE_ROTATE ;
					kx->Type = MV1_ANIMKEY_TYPE_VECTOR ;
					kx->KeyVector = NewBuffer ;

					KEYSETLIST_SUB( ky, AnimR, ReadModel ) ;
					KEYSETLIST_SUB( kz, AnimR, ReadModel ) ;
				}
			}
		}
	}

	// 削除できるキーがある場合は削除する
	AnimR = ReadModel->AnimFirst ;
	for( i = 0 ; ( DWORD )i < ReadModel->AnimNum ; i ++, AnimR = AnimR->DataNext )
	{
		KeySetR = AnimR->AnimKeySetFirst ;
		for( j = 0 ; j < AnimR->AnimKeySetNum ; j ++, KeySetR = KeySetR->Next )
		{
			MV1OptimizeKeySetTimeType( ReadModel, KeySetR ) ;
		}
	}

	return 0 ;
/*
	// 16bit値に変換する
	AnimR = ReadModel->AnimFirst ;
	for( i = 0 ; ( DWORD )i < ReadModel->AnimNum ; i ++, AnimR = AnimR->DataNext )
	{
		KeySetR = AnimR->AnimKeySetFirst ;
		for( j = 0 ; j < AnimR->AnimKeySetNum ; j ++, KeySetR = KeySetR->Next )
		{
			if( KeySetR->Type == MV1_ANIMKEY_TYPE_QUATERNION ) continue ;

			// 既に１６ビットだったら何もしない
			if( KeySetR->Bit16 ) continue ;

			// キーが１個か無かったら何もしない
			if( KeySetR->Num <= 1 ) continue ;

			// 時間データ部分を固定小数点化
			TimeSub.Unit = 1.0f ;
			TimeSub.Min = 0.0f ;
			if( KeySetR->TimeType == MV1_ANIMKEY_TIME_TYPE_KEY )
			{
				Time = KeySetR->KeyTime ;
				TimeSub.Min  = Time[ 0 ] ;
				TimeSub.Unit = Time[ 0 ] ;
				Time ++ ;
				for( k = 1 ; k < KeySetR->Num ; k ++, Time ++ )
				{
					if( TimeSub.Min  > *Time ) TimeSub.Min  = *Time ;
					if( TimeSub.Unit < *Time ) TimeSub.Unit = *Time ;
				}

				// 16bit補助情報の作成
				KeySetR->Time16BSub.Min  = MV1AnimKey16BitMinFtoB( TimeSub.Min  ) ;
				TimeSub.Min              = MV1AnimKey16BitMinBtoF( KeySetR->Time16BSub.Min ) ;
				TimeSub.Unit            -= TimeSub.Min ;
				if( TimeSub.Unit < 0.00000001f )
					TimeSub.Unit = 1.0f ;
				TimeSub.Unit            /= 60000.0f ;
				KeySetR->Time16BSub.Unit = MV1AnimKey16BitUnitFtoB( TimeSub.Unit ) ;
				TimeSub.Unit             = MV1AnimKey16BitUnitBtoF( KeySetR->Time16BSub.Unit ) ;

				// 値を変換
				Time       = KeySetR->KeyTime ;
				NewTimeB16 = KeySetR->KeyTimeB16 ;
				for( k = 0 ; k < KeySetR->Num ; k ++, Time ++, NewTimeB16 ++ )
				{
					*NewTimeB16 = ( WORD )( _FTOL( ( *Time - TimeSub.Min ) / TimeSub.Unit ) ) ;
				}
			}
			
			switch( KeySetR->Type )
			{
			case MV1_ANIMKEY_TYPE_LINEAR :
				{
					WORD *NewKey ;
					float *Key ;

					// 最小値と最大値を取得
					KeySub.Min  =  1000000000000.0f ;
					KeySub.Unit = -1000000000000.0f ;
					Key = KeySetR->KeyLinear ;
					for( k = 0 ; k < KeySetR->Num ; k ++, Key ++ )
					{
						 if( KeySub.Min  > *Key ) KeySub.Min  = *Key ;
						 if( KeySub.Unit < *Key ) KeySub.Unit = *Key ;
					}

					// 16bit補助情報の作成
					KeySetR->Key16BSub.Min  = MV1AnimKey16BitMinFtoB( KeySub.Min  ) ;
					KeySub.Min              = MV1AnimKey16BitMinBtoF( KeySetR->Key16BSub.Min ) ;
					KeySub.Unit            -= KeySub.Min ;
					if( KeySub.Unit < 0.00000001f )
						KeySub.Unit = 1.0f ;
					KeySub.Unit            /= 60000.0f ;
					KeySetR->Key16BSub.Unit = MV1AnimKey16BitUnitFtoB( KeySub.Unit ) ;
					KeySub.Unit             = MV1AnimKey16BitUnitBtoF( KeySetR->Key16BSub.Unit ) ;

					// 16bit１辺りの値が 0.1f を超えてしまったら 16bit化しない
					if( KeySub.Unit >= 0.1f ) continue ;

					// 値を変換
					Key        = KeySetR->KeyLinear ;
					NewKey     = KeySetR->KeyLinearB16 ;
					for( k = 0 ; k < KeySetR->Num ; k ++, Key ++, NewKey ++ )
					{
						*( ( WORD * )NewKey ) = ( WORD )( _FTOL( ( *Key - KeySub.Min ) / KeySub.Unit ) ) ;
					}

					// キーサイズを減らして増やす
					ReadModel->AnimKeyDataSize -= sizeof( float ) * KeySetR->Num ;
					ReadModel->AnimKeyDataSize += sizeof( WORD  ) * KeySetR->Num ;
				}
				break ;

			case MV1_ANIMKEY_TYPE_VECTOR :
				{
					VECTOR_B16 *NewKey ;
					VECTOR *Key ;

					// 最小値と最大値を取得
					KeySub.Min  =  1000000000000.0f ;
					KeySub.Unit = -1000000000000.0f ;
					Key = KeySetR->KeyVector ;
					for( k = 0 ; k < KeySetR->Num ; k ++, Key ++ )
					{
						 if( KeySub.Min  > Key->x ) KeySub.Min  = Key->x ;
						 if( KeySub.Unit < Key->x ) KeySub.Unit = Key->x ;
						 if( KeySub.Min  > Key->y ) KeySub.Min  = Key->y ;
						 if( KeySub.Unit < Key->y ) KeySub.Unit = Key->y ;
						 if( KeySub.Min  > Key->z ) KeySub.Min  = Key->z ;
						 if( KeySub.Unit < Key->z ) KeySub.Unit = Key->z ;
					}

					// 16bit補助情報の作成
					KeySetR->Key16BSub.Min  = MV1AnimKey16BitMinFtoB( KeySub.Min  ) ;
					KeySub.Min              = MV1AnimKey16BitMinBtoF( KeySetR->Key16BSub.Min ) ;
					KeySub.Unit            -= KeySub.Min ;
					if( KeySub.Unit < 0.00000001f )
						KeySub.Unit = 1.0f ;
					KeySub.Unit            /= 60000.0f ;
					KeySetR->Key16BSub.Unit = MV1AnimKey16BitUnitFtoB( KeySub.Unit ) ;
					KeySub.Unit             = MV1AnimKey16BitUnitBtoF( KeySetR->Key16BSub.Unit ) ;

					// 16bit１辺りの値が 0.1f を超えてしまったら 16bit化しない
					if( KeySub.Unit >= 0.1f ) continue ;

					// 値を変換
					Key        = KeySetR->KeyVector ;
					NewKey     = KeySetR->KeyVectorB16 ;
					for( k = 0 ; k < KeySetR->Num ; k ++, Key ++, NewKey ++ )
					{
						*( ( WORD * )&NewKey->x ) = ( WORD )( _FTOL( ( Key->x - KeySub.Min ) / KeySub.Unit ) ) ;
						*( ( WORD * )&NewKey->y ) = ( WORD )( _FTOL( ( Key->y - KeySub.Min ) / KeySub.Unit ) ) ;
						*( ( WORD * )&NewKey->z ) = ( WORD )( _FTOL( ( Key->z - KeySub.Min ) / KeySub.Unit ) ) ;
					}

					// キーサイズを減らして増やす
					ReadModel->AnimKeyDataSize -= sizeof( VECTOR )     * KeySetR->Num ;
					ReadModel->AnimKeyDataSize += sizeof( VECTOR_B16 ) * KeySetR->Num ;
				}
				break ;

			case MV1_ANIMKEY_TYPE_QUATERNION :
				{
					FLOAT4_B16 *NewKey ;
					FLOAT4 *Key ;

					// 最小値と最大値を取得
					KeySub.Min  =  1000000000000.0f ;
					KeySub.Unit = -1000000000000.0f ;
					Key = KeySetR->KeyFloat4 ;
					for( k = 0 ; k < KeySetR->Num ; k ++, Key ++ )
					{
						if( KeySub.Min  > Key->x ) KeySub.Min  = Key->x ;
						if( KeySub.Unit < Key->x ) KeySub.Unit = Key->x ;
						if( KeySub.Min  > Key->y ) KeySub.Min  = Key->y ;
						if( KeySub.Unit < Key->y ) KeySub.Unit = Key->y ;
						if( KeySub.Min  > Key->z ) KeySub.Min  = Key->z ;
						if( KeySub.Unit < Key->z ) KeySub.Unit = Key->z ;
						if( KeySub.Min  > Key->w ) KeySub.Min  = Key->w ;
						if( KeySub.Unit < Key->w ) KeySub.Unit = Key->w ;
					}

					// 16bit補助情報の作成
					KeySetR->Key16BSub.Min  = MV1AnimKey16BitMinFtoB( KeySub.Min  ) ;
					KeySub.Min              = MV1AnimKey16BitMinBtoF( KeySetR->Key16BSub.Min ) ;
					KeySub.Unit            -= KeySub.Min ;
					if( KeySub.Unit < 0.00000001f )
						KeySub.Unit = 1.0f ;
					KeySub.Unit            /= 60000.0f ;
					KeySetR->Key16BSub.Unit = MV1AnimKey16BitUnitFtoB( KeySub.Unit ) ;
					KeySub.Unit             = MV1AnimKey16BitUnitBtoF( KeySetR->Key16BSub.Unit ) ;

					// 16bit１辺りの値が 0.1f を超えてしまったら 16bit化しない
					if( KeySub.Unit >= 0.1f ) continue ;

					// 値を変換
					Key        = KeySetR->KeyFloat4 ;
					NewKey     = KeySetR->KeyFloat4B16 ;
					for( k = 0 ; k < KeySetR->Num ; k ++, Key ++, NewKey ++ )
					{
						*( ( WORD * )&NewKey->x ) = ( WORD )( _FTOL( ( Key->x - KeySub.Min ) / KeySub.Unit ) ) ;
						*( ( WORD * )&NewKey->y ) = ( WORD )( _FTOL( ( Key->y - KeySub.Min ) / KeySub.Unit ) ) ;
						*( ( WORD * )&NewKey->z ) = ( WORD )( _FTOL( ( Key->z - KeySub.Min ) / KeySub.Unit ) ) ;
						*( ( WORD * )&NewKey->w ) = ( WORD )( _FTOL( ( Key->w - KeySub.Min ) / KeySub.Unit ) ) ;
					}

					// キーサイズを減らして増やす
					ReadModel->AnimKeyDataSize -= sizeof( FLOAT4 )     * KeySetR->Num ;
					ReadModel->AnimKeyDataSize += sizeof( FLOAT4_B16 ) * KeySetR->Num ;
				}
				break ;

			case MV1_ANIMKEY_TYPE_MATRIX3X3 :
				{
					MV1_ANIM_KEY_MATRIX3X3_B16 *NewKey ;
					MV1_ANIM_KEY_MATRIX3X3 *Key ;

					// 最小値と最大値を取得
					KeySub.Min  =  1000000000000.0f ;
					KeySub.Unit = -1000000000000.0f ;
					Key  = KeySetR->KeyMatrix3x3 ;
					for( k = 0 ; k < KeySetR->Num ; k ++, Key ++ )
					{
						if( KeySub.Min  > Key->Matrix[ 0 ][ 0 ] ) KeySub.Min  = Key->Matrix[ 0 ][ 0 ] ;
						if( KeySub.Unit < Key->Matrix[ 0 ][ 0 ] ) KeySub.Unit = Key->Matrix[ 0 ][ 0 ] ;
						if( KeySub.Min  > Key->Matrix[ 0 ][ 1 ] ) KeySub.Min  = Key->Matrix[ 0 ][ 1 ] ;
						if( KeySub.Unit < Key->Matrix[ 0 ][ 1 ] ) KeySub.Unit = Key->Matrix[ 0 ][ 1 ] ;
						if( KeySub.Min  > Key->Matrix[ 0 ][ 2 ] ) KeySub.Min  = Key->Matrix[ 0 ][ 2 ] ;
						if( KeySub.Unit < Key->Matrix[ 0 ][ 2 ] ) KeySub.Unit = Key->Matrix[ 0 ][ 2 ] ;
						if( KeySub.Min  > Key->Matrix[ 1 ][ 0 ] ) KeySub.Min  = Key->Matrix[ 1 ][ 0 ] ;
						if( KeySub.Unit < Key->Matrix[ 1 ][ 0 ] ) KeySub.Unit = Key->Matrix[ 1 ][ 0 ] ;
						if( KeySub.Min  > Key->Matrix[ 1 ][ 1 ] ) KeySub.Min  = Key->Matrix[ 1 ][ 1 ] ;
						if( KeySub.Unit < Key->Matrix[ 1 ][ 1 ] ) KeySub.Unit = Key->Matrix[ 1 ][ 1 ] ;
						if( KeySub.Min  > Key->Matrix[ 1 ][ 2 ] ) KeySub.Min  = Key->Matrix[ 1 ][ 2 ] ;
						if( KeySub.Unit < Key->Matrix[ 1 ][ 2 ] ) KeySub.Unit = Key->Matrix[ 1 ][ 2 ] ;
						if( KeySub.Min  > Key->Matrix[ 2 ][ 0 ] ) KeySub.Min  = Key->Matrix[ 2 ][ 0 ] ;
						if( KeySub.Unit < Key->Matrix[ 2 ][ 0 ] ) KeySub.Unit = Key->Matrix[ 2 ][ 0 ] ;
						if( KeySub.Min  > Key->Matrix[ 2 ][ 1 ] ) KeySub.Min  = Key->Matrix[ 2 ][ 1 ] ;
						if( KeySub.Unit < Key->Matrix[ 2 ][ 1 ] ) KeySub.Unit = Key->Matrix[ 2 ][ 1 ] ;
						if( KeySub.Min  > Key->Matrix[ 2 ][ 2 ] ) KeySub.Min  = Key->Matrix[ 2 ][ 2 ] ;
						if( KeySub.Unit < Key->Matrix[ 2 ][ 2 ] ) KeySub.Unit = Key->Matrix[ 2 ][ 2 ] ;
					}

					// 16bit補助情報の作成
					KeySetR->Key16BSub.Min  = MV1AnimKey16BitMinFtoB( KeySub.Min  ) ;
					KeySub.Min              = MV1AnimKey16BitMinBtoF( KeySetR->Key16BSub.Min ) ;
					KeySub.Unit            -= KeySub.Min ;
					if( KeySub.Unit < 0.00000001f )
						KeySub.Unit = 1.0f ;
					KeySub.Unit            /= 60000.0f ;
					KeySetR->Key16BSub.Unit = MV1AnimKey16BitUnitFtoB( KeySub.Unit ) ;
					KeySub.Unit             = MV1AnimKey16BitUnitBtoF( KeySetR->Key16BSub.Unit ) ;

					// 16bit１辺りの値が 0.1f を超えてしまったら 16bit化しない
					if( KeySub.Unit >= 0.1f ) continue ;

					// 値を変換
					Key    = KeySetR->KeyMatrix3x3 ;
					NewKey = KeySetR->KeyMatrix3x3B16 ;
					for( k = 0 ; k < KeySetR->Num ; k ++, Key ++, NewKey ++ )
					{
						NewKey->Matrix[ 0 ][ 0 ] = ( WORD )( _FTOL( ( Key->Matrix[ 0 ][ 0 ] - KeySub.Min ) / KeySub.Unit ) ) ;
						NewKey->Matrix[ 0 ][ 1 ] = ( WORD )( _FTOL( ( Key->Matrix[ 0 ][ 1 ] - KeySub.Min ) / KeySub.Unit ) ) ;
						NewKey->Matrix[ 0 ][ 2 ] = ( WORD )( _FTOL( ( Key->Matrix[ 0 ][ 2 ] - KeySub.Min ) / KeySub.Unit ) ) ;
						NewKey->Matrix[ 1 ][ 0 ] = ( WORD )( _FTOL( ( Key->Matrix[ 1 ][ 0 ] - KeySub.Min ) / KeySub.Unit ) ) ;
						NewKey->Matrix[ 1 ][ 1 ] = ( WORD )( _FTOL( ( Key->Matrix[ 1 ][ 1 ] - KeySub.Min ) / KeySub.Unit ) ) ;
						NewKey->Matrix[ 1 ][ 2 ] = ( WORD )( _FTOL( ( Key->Matrix[ 1 ][ 2 ] - KeySub.Min ) / KeySub.Unit ) ) ;
						NewKey->Matrix[ 2 ][ 0 ] = ( WORD )( _FTOL( ( Key->Matrix[ 2 ][ 0 ] - KeySub.Min ) / KeySub.Unit ) ) ;
						NewKey->Matrix[ 2 ][ 1 ] = ( WORD )( _FTOL( ( Key->Matrix[ 2 ][ 1 ] - KeySub.Min ) / KeySub.Unit ) ) ;
						NewKey->Matrix[ 2 ][ 2 ] = ( WORD )( _FTOL( ( Key->Matrix[ 2 ][ 2 ] - KeySub.Min ) / KeySub.Unit ) ) ;
					}

					// キーサイズを減らして増やす
					ReadModel->AnimKeyDataSize -= sizeof( MV1_ANIM_KEY_MATRIX3X3     ) * KeySetR->Num ;
					ReadModel->AnimKeyDataSize += sizeof( MV1_ANIM_KEY_MATRIX3X3_B16 ) * KeySetR->Num ;
				}
				break ;
			}

			// タイムがキータイプの場合はキーサイズを減らして増やす
			if( KeySetR->TimeType == MV1_ANIMKEY_TIME_TYPE_KEY )
			{
				ReadModel->AnimKeyDataSize -= sizeof( float ) * KeySetR->Num ;
				ReadModel->AnimKeyDataSize += sizeof( WORD  ) * KeySetR->Num ;
			}

			// 16ビットフラグを立てる
			KeySetR->Bit16 = 1 ;
		}
	}

	// 終了
	return true ;
	*/
}

// キーセットのキーを最適化する
static void MV1OptimizeKeySetTimeType( MV1_MODEL_R *ReadModel, MV1_ANIMKEYSET_R *KeySetR, bool NotKeySub )
{
	float ktime = 0.0f, ntime, f ;
	void *AfterBuffer ;
	float *Time, *NewTime ;
	bool Ittei ;
	int k, num = 0, UnitSize = 0, AfterSizeK, AfterSizeU ;
	float rate ;

	// 各キーが一定間隔かどうかを調べる
	Ittei = false ;
	if( KeySetR->TimeType == MV1_ANIMKEY_TIME_TYPE_KEY )
	{
		if( KeySetR->Num > 2 )
		{
			Time = KeySetR->KeyTime ;
			ktime = Time[ 1 ] - Time[ 0 ] ;
			Time ++ ;
			for( k = 1 ; k < KeySetR->Num - 1 ; k ++, Time ++ )
			{
				ntime = ( Time[ 1 ] - Time[ 0 ] ) - ktime ;
				if( ntime > 0.000001f || ntime < -0.000001f ) break ;
			}
			if( k == KeySetR->Num - 1 )
			{
				Ittei = true ;
			}
		}
	}
	else
	{
		Ittei = true ;
	}

	AfterBuffer = NULL ;
	switch( KeySetR->Type )
	{
	case MV1_ANIMKEY_TYPE_LINEAR :
		{
			float *Key, *NewBuffer, *NewKey ;

			UnitSize = sizeof( float ) ;
			AfterBuffer = NewBuffer = ( float * )ADDMEMAREA( ( UnitSize + sizeof( float ) ) * KeySetR->Num, &ReadModel->Mem ) ;
			if( NewBuffer )
			{
				// 線形補間で済むキーを削除する
				num = 0 ;
				NewKey  = NewBuffer ;
				NewTime = ( float * )( NewBuffer + KeySetR->Num ) ;
				Key     = KeySetR->KeyLinear ;
				Time    = KeySetR->KeyTime ;
				if( KeySetR->TimeType == MV1_ANIMKEY_TIME_TYPE_ONE )
				{
					rate = 0.5f ;
					for( k = 0 ; k < KeySetR->Num ; k ++, Key ++ )
					{
						if( k != 0 && k != KeySetR->Num - 1 )
						{
							f = ( rate * ( Key[ 1 ] - NewKey[ -1 ] ) + NewKey[ -1 ] ) - Key[ 0 ] ; if( f > 0.00001f || f < -0.00001f ) goto FLOATADDONE ;

							continue ;
						}

						if( k == KeySetR->Num - 1 && num == 1 )
						{
							if( *( ( DWORD * )&Key[ 0 ] ) != *( ( DWORD * )&NewKey[ -1 ] ) ) goto FLOATADDONE ;
							continue ;
						}
FLOATADDONE :
						*NewKey  = *Key ;
						*NewTime = KeySetR->UnitTime * k + KeySetR->StartTime ;
						NewKey  ++ ;
						NewTime ++ ;
						num ++ ;
					}
				}
				else
				{
					for( k = 0 ; k < KeySetR->Num ; k ++, Key ++, Time ++ )
					{
						if( k != 0 && k != KeySetR->Num - 1 )
						{
							rate = ( Time[ 0 ] - NewTime[ -1 ] ) / ( Time[ 1 ] - NewTime[ -1 ] ) ;
							f = ( rate * ( Key[ 1 ] - NewKey[ -1 ] ) + NewKey[ -1 ] ) - Key[ 0 ] ; if( f > 0.00001f || f < -0.00001f ) goto FLOATADD ;

							continue ;
						}

						if( k == KeySetR->Num - 1 && num == 1 )
						{
							if( *( ( DWORD * )&Key[ 0 ] ) != *( ( DWORD * )&NewKey[ -1 ] ) ) goto FLOATADD ;
							continue ;
						}
FLOATADD :
						*NewKey  = *Key ;
						*NewTime = *Time ;
						NewKey  ++ ;
						NewTime ++ ;
						num ++ ;
					}
				}
			}
		}
		break ;

	case MV1_ANIMKEY_TYPE_VECTOR :
		{
			VECTOR *Key, *NewBuffer, *NewKey ;

			UnitSize = sizeof( VECTOR ) ;
			AfterBuffer = NewBuffer = ( VECTOR * )ADDMEMAREA( ( UnitSize + sizeof( float ) ) * KeySetR->Num, &ReadModel->Mem ) ;
			if( NewBuffer )
			{
				// 線形補間で済むキーを削除する
				num = 0 ;
				NewKey  = NewBuffer ;
				NewTime = ( float * )( NewBuffer + KeySetR->Num ) ;
				Key     = KeySetR->KeyVector ;
				Time    = KeySetR->KeyTime ;
				if( KeySetR->TimeType == MV1_ANIMKEY_TIME_TYPE_ONE )
				{
					rate = 0.5f ;
					for( k = 0 ; k < KeySetR->Num ; k ++, Key ++ )
					{
						if( k != 0 && k != KeySetR->Num - 1 )
						{
							f = ( rate * ( Key[ 1 ].x - NewKey[ -1 ].x ) + NewKey[ -1 ].x ) - Key[ 0 ].x ; if( f > 0.00001f || f < -0.00001f ) goto TRANSLATEADDONE ;
							f = ( rate * ( Key[ 1 ].y - NewKey[ -1 ].y ) + NewKey[ -1 ].y ) - Key[ 0 ].y ; if( f > 0.00001f || f < -0.00001f ) goto TRANSLATEADDONE ;
							f = ( rate * ( Key[ 1 ].z - NewKey[ -1 ].z ) + NewKey[ -1 ].z ) - Key[ 0 ].z ; if( f > 0.00001f || f < -0.00001f ) goto TRANSLATEADDONE ;

							continue ;
						}

						if( k == KeySetR->Num - 1 && num == 1 )
						{
							if( *( ( DWORD * )&Key[ 0 ].x ) != *( ( DWORD * )&NewKey[ -1 ].x ) ||
								*( ( DWORD * )&Key[ 0 ].y ) != *( ( DWORD * )&NewKey[ -1 ].y ) ||
								*( ( DWORD * )&Key[ 0 ].z ) != *( ( DWORD * )&NewKey[ -1 ].z ) ) goto TRANSLATEADDONE ;
							continue ;
						}
TRANSLATEADDONE :
						*NewKey  = *Key ;
						*NewTime = KeySetR->UnitTime * k + KeySetR->StartTime ;
						NewKey  ++ ;
						NewTime ++ ;
						num ++ ;
					}
				}
				else
				{
					for( k = 0 ; k < KeySetR->Num ; k ++, Key ++, Time ++ )
					{
						if( k != 0 && k != KeySetR->Num - 1 )
						{
							rate = ( Time[ 0 ] - NewTime[ -1 ] ) / ( Time[ 1 ] - NewTime[ -1 ] ) ;
							f = ( rate * ( Key[ 1 ].x - NewKey[ -1 ].x ) + NewKey[ -1 ].x ) - Key[ 0 ].x ; if( f > 0.00001f || f < -0.00001f ) goto TRANSLATEADD ;
							f = ( rate * ( Key[ 1 ].y - NewKey[ -1 ].y ) + NewKey[ -1 ].y ) - Key[ 0 ].y ; if( f > 0.00001f || f < -0.00001f ) goto TRANSLATEADD ;
							f = ( rate * ( Key[ 1 ].z - NewKey[ -1 ].z ) + NewKey[ -1 ].z ) - Key[ 0 ].z ; if( f > 0.00001f || f < -0.00001f ) goto TRANSLATEADD ;

							continue ;
						}

						if( k == KeySetR->Num - 1 && num == 1 )
						{
							if( *( ( DWORD * )&Key[ 0 ].x ) != *( ( DWORD * )&NewKey[ -1 ].x ) ||
								*( ( DWORD * )&Key[ 0 ].y ) != *( ( DWORD * )&NewKey[ -1 ].y ) ||
								*( ( DWORD * )&Key[ 0 ].z ) != *( ( DWORD * )&NewKey[ -1 ].z ) ) goto TRANSLATEADD ;
							continue ;
						}
TRANSLATEADD :
						*NewKey  = *Key ;
						*NewTime = *Time ;
						NewKey  ++ ;
						NewTime ++ ;
						num ++ ;
					}
				}
			}
		}
		break ;

	case MV1_ANIMKEY_TYPE_QUATERNION_VMD :
	case MV1_ANIMKEY_TYPE_QUATERNION_X :
		{
			FLOAT4 *Key, *NewBuffer, *NewKey ;

			UnitSize = sizeof( FLOAT4 ) ;
			AfterBuffer = NewBuffer = ( FLOAT4 * )ADDMEMAREA( ( UnitSize + sizeof( float ) ) * KeySetR->Num, &ReadModel->Mem ) ;
			if( NewBuffer )
			{
				// 線形補間で済むキーを削除する
				num = 0 ;
				NewKey  = NewBuffer ;
				NewTime = ( float * )( NewBuffer + KeySetR->Num ) ;
				Key     = KeySetR->KeyFloat4 ;
				Time    = KeySetR->KeyTime ;
				if( KeySetR->TimeType == MV1_ANIMKEY_TIME_TYPE_ONE )
				{
					rate = 0.5f ;
					for( k = 0 ; k < KeySetR->Num ; k ++, Key ++ )
					{
						if( k != 0 && k != KeySetR->Num - 1 )
						{
							f = ( rate * ( Key[ 1 ].x - NewKey[ -1 ].x ) + NewKey[ -1 ].x ) - Key[ 0 ].x ; if( f > 0.00001f || f < -0.00001f ) goto QUATERNIONADDONE ;
							f = ( rate * ( Key[ 1 ].y - NewKey[ -1 ].y ) + NewKey[ -1 ].y ) - Key[ 0 ].y ; if( f > 0.00001f || f < -0.00001f ) goto QUATERNIONADDONE ;
							f = ( rate * ( Key[ 1 ].z - NewKey[ -1 ].z ) + NewKey[ -1 ].z ) - Key[ 0 ].z ; if( f > 0.00001f || f < -0.00001f ) goto QUATERNIONADDONE ;
							f = ( rate * ( Key[ 1 ].w - NewKey[ -1 ].w ) + NewKey[ -1 ].w ) - Key[ 0 ].w ; if( f > 0.00001f || f < -0.00001f ) goto QUATERNIONADDONE ;

							continue ;
						}

						if( k == KeySetR->Num - 1 && num == 1 )
						{
							if( *( ( DWORD * )&Key[ 0 ].x ) != *( ( DWORD * )&NewKey[ -1 ].x ) ||
								*( ( DWORD * )&Key[ 0 ].y ) != *( ( DWORD * )&NewKey[ -1 ].y ) ||
								*( ( DWORD * )&Key[ 0 ].z ) != *( ( DWORD * )&NewKey[ -1 ].z ) ||
								*( ( DWORD * )&Key[ 0 ].w ) != *( ( DWORD * )&NewKey[ -1 ].w ) ) goto QUATERNIONADDONE ;
							continue ;
						}
QUATERNIONADDONE :
						*NewKey  = *Key ;
						*NewTime = KeySetR->UnitTime * k + KeySetR->StartTime ;
						NewKey  ++ ;
						NewTime ++ ;
						num ++ ;
					}
				}
				else
				{
					for( k = 0 ; k < KeySetR->Num ; k ++, Key ++, Time ++ )
					{
						if( k != 0 && k != KeySetR->Num - 1 )
						{
							rate = ( Time[ 0 ] - NewTime[ -1 ] ) / ( Time[ 1 ] - NewTime[ -1 ] ) ;
							f = ( rate * ( Key[ 1 ].x - NewKey[ -1 ].x ) + NewKey[ -1 ].x ) - Key[ 0 ].x ; if( f > 0.00001f || f < -0.00001f ) goto QUATERNIONADD ;
							f = ( rate * ( Key[ 1 ].y - NewKey[ -1 ].y ) + NewKey[ -1 ].y ) - Key[ 0 ].y ; if( f > 0.00001f || f < -0.00001f ) goto QUATERNIONADD ;
							f = ( rate * ( Key[ 1 ].z - NewKey[ -1 ].z ) + NewKey[ -1 ].z ) - Key[ 0 ].z ; if( f > 0.00001f || f < -0.00001f ) goto QUATERNIONADD ;
							f = ( rate * ( Key[ 1 ].w - NewKey[ -1 ].w ) + NewKey[ -1 ].w ) - Key[ 0 ].w ; if( f > 0.00001f || f < -0.00001f ) goto QUATERNIONADD ;

							continue ;
						}

						if( k == KeySetR->Num - 1 && num == 1 )
						{
							if( *( ( DWORD * )&Key[ 0 ].x ) != *( ( DWORD * )&NewKey[ -1 ].x ) ||
								*( ( DWORD * )&Key[ 0 ].y ) != *( ( DWORD * )&NewKey[ -1 ].y ) ||
								*( ( DWORD * )&Key[ 0 ].z ) != *( ( DWORD * )&NewKey[ -1 ].z ) ||
								*( ( DWORD * )&Key[ 0 ].w ) != *( ( DWORD * )&NewKey[ -1 ].w ) ) goto QUATERNIONADD ;
							continue ;
						}
QUATERNIONADD :
						*NewKey  = *Key ;
						*NewTime = *Time ;
						NewKey  ++ ;
						NewTime ++ ;
						num ++ ;
					}
				}
			}
		}
		break ;

	case MV1_ANIMKEY_TYPE_MATRIX3X3 :
		{
			MV1_ANIM_KEY_MATRIX3X3 *Key, *NewBuffer, *NewKey ;

			UnitSize = sizeof( MV1_ANIM_KEY_MATRIX3X3 ) ;
			AfterBuffer = NewBuffer = ( MV1_ANIM_KEY_MATRIX3X3 * )ADDMEMAREA( ( UnitSize + sizeof( float ) ) * KeySetR->Num, &ReadModel->Mem ) ;
			if( NewBuffer )
			{
				// 線形補間で済むキーを削除する
				num = 0 ;
				Key     = KeySetR->KeyMatrix3x3 ;
				Time    = KeySetR->KeyTime ;
				NewKey  = NewBuffer ;
				NewTime = ( float * )( NewBuffer + KeySetR->Num ) ;
				if( KeySetR->TimeType == MV1_ANIMKEY_TIME_TYPE_ONE )
				{
					rate = 0.5f ;
					for( k = 0 ; k < KeySetR->Num ; k ++, Key ++ )
					{
						if( k != 0 && k != KeySetR->Num - 1 )
						{
							f = ( rate * ( Key[ 1 ].Matrix[ 0 ][ 0 ] - NewKey[ -1 ].Matrix[ 0 ][ 0 ] ) + NewKey[ -1 ].Matrix[ 0 ][ 0 ] ) - Key[ 0 ].Matrix[ 0 ][ 0 ] ; if( f > 0.00001f || f < -0.00001f ) goto MATRIX3X3ADDONE ;
							f = ( rate * ( Key[ 1 ].Matrix[ 0 ][ 1 ] - NewKey[ -1 ].Matrix[ 0 ][ 1 ] ) + NewKey[ -1 ].Matrix[ 0 ][ 1 ] ) - Key[ 0 ].Matrix[ 0 ][ 1 ] ; if( f > 0.00001f || f < -0.00001f ) goto MATRIX3X3ADDONE ;
							f = ( rate * ( Key[ 1 ].Matrix[ 0 ][ 2 ] - NewKey[ -1 ].Matrix[ 0 ][ 2 ] ) + NewKey[ -1 ].Matrix[ 0 ][ 2 ] ) - Key[ 0 ].Matrix[ 0 ][ 2 ] ; if( f > 0.00001f || f < -0.00001f ) goto MATRIX3X3ADDONE ;
							f = ( rate * ( Key[ 1 ].Matrix[ 1 ][ 0 ] - NewKey[ -1 ].Matrix[ 1 ][ 0 ] ) + NewKey[ -1 ].Matrix[ 1 ][ 0 ] ) - Key[ 0 ].Matrix[ 1 ][ 0 ] ; if( f > 0.00001f || f < -0.00001f ) goto MATRIX3X3ADDONE ;
							f = ( rate * ( Key[ 1 ].Matrix[ 1 ][ 1 ] - NewKey[ -1 ].Matrix[ 1 ][ 1 ] ) + NewKey[ -1 ].Matrix[ 1 ][ 1 ] ) - Key[ 0 ].Matrix[ 1 ][ 1 ] ; if( f > 0.00001f || f < -0.00001f ) goto MATRIX3X3ADDONE ;
							f = ( rate * ( Key[ 1 ].Matrix[ 1 ][ 2 ] - NewKey[ -1 ].Matrix[ 1 ][ 2 ] ) + NewKey[ -1 ].Matrix[ 1 ][ 2 ] ) - Key[ 0 ].Matrix[ 1 ][ 2 ] ; if( f > 0.00001f || f < -0.00001f ) goto MATRIX3X3ADDONE ;
							f = ( rate * ( Key[ 1 ].Matrix[ 2 ][ 0 ] - NewKey[ -1 ].Matrix[ 2 ][ 0 ] ) + NewKey[ -1 ].Matrix[ 2 ][ 0 ] ) - Key[ 0 ].Matrix[ 2 ][ 0 ] ; if( f > 0.00001f || f < -0.00001f ) goto MATRIX3X3ADDONE ;
							f = ( rate * ( Key[ 1 ].Matrix[ 2 ][ 1 ] - NewKey[ -1 ].Matrix[ 2 ][ 1 ] ) + NewKey[ -1 ].Matrix[ 2 ][ 1 ] ) - Key[ 0 ].Matrix[ 2 ][ 1 ] ; if( f > 0.00001f || f < -0.00001f ) goto MATRIX3X3ADDONE ;
							f = ( rate * ( Key[ 1 ].Matrix[ 2 ][ 2 ] - NewKey[ -1 ].Matrix[ 2 ][ 2 ] ) + NewKey[ -1 ].Matrix[ 2 ][ 2 ] ) - Key[ 0 ].Matrix[ 2 ][ 2 ] ; if( f > 0.00001f || f < -0.00001f ) goto MATRIX3X3ADDONE ;

							continue ;
						}

						if( k == KeySetR->Num - 1 && num == 1 )
						{
							if( *( ( DWORD * )&Key[ 0 ].Matrix[ 0 ][ 0 ] ) != *( ( DWORD * )&NewKey[ -1 ].Matrix[ 0 ][ 0 ] ) ||
								*( ( DWORD * )&Key[ 0 ].Matrix[ 0 ][ 1 ] ) != *( ( DWORD * )&NewKey[ -1 ].Matrix[ 0 ][ 1 ] ) ||
								*( ( DWORD * )&Key[ 0 ].Matrix[ 0 ][ 2 ] ) != *( ( DWORD * )&NewKey[ -1 ].Matrix[ 0 ][ 2 ] ) ||
								*( ( DWORD * )&Key[ 0 ].Matrix[ 1 ][ 0 ] ) != *( ( DWORD * )&NewKey[ -1 ].Matrix[ 1 ][ 0 ] ) ||
								*( ( DWORD * )&Key[ 0 ].Matrix[ 1 ][ 1 ] ) != *( ( DWORD * )&NewKey[ -1 ].Matrix[ 1 ][ 1 ] ) ||
								*( ( DWORD * )&Key[ 0 ].Matrix[ 1 ][ 2 ] ) != *( ( DWORD * )&NewKey[ -1 ].Matrix[ 1 ][ 2 ] ) ||
								*( ( DWORD * )&Key[ 0 ].Matrix[ 2 ][ 0 ] ) != *( ( DWORD * )&NewKey[ -1 ].Matrix[ 2 ][ 0 ] ) ||
								*( ( DWORD * )&Key[ 0 ].Matrix[ 2 ][ 1 ] ) != *( ( DWORD * )&NewKey[ -1 ].Matrix[ 2 ][ 1 ] ) ||
								*( ( DWORD * )&Key[ 0 ].Matrix[ 2 ][ 2 ] ) != *( ( DWORD * )&NewKey[ -1 ].Matrix[ 2 ][ 2 ] ) ) goto MATRIX3X3ADDONE ;
							continue ;
						}
MATRIX3X3ADDONE :
						*NewKey  = *Key ;
						*NewTime = KeySetR->UnitTime * k + KeySetR->StartTime ;
						NewKey  ++ ;
						NewTime ++ ;
						num ++ ;
					}
				}
				else
				{
					for( k = 0 ; k < KeySetR->Num ; k ++, Key ++, Time ++ )
					{
						if( k != 0 && k != KeySetR->Num - 1 )
						{
							rate = ( Time[ 0 ] - NewTime[ -1 ] ) / ( Time[ 1 ] - NewTime[ -1 ] ) ;
							f = ( rate * ( Key[ 1 ].Matrix[ 0 ][ 0 ] - NewKey[ -1 ].Matrix[ 0 ][ 0 ] ) + NewKey[ -1 ].Matrix[ 0 ][ 0 ] ) - Key[ 0 ].Matrix[ 0 ][ 0 ] ; if( f > 0.00001f || f < -0.00001f ) goto MATRIX3X3ADD ;
							f = ( rate * ( Key[ 1 ].Matrix[ 0 ][ 1 ] - NewKey[ -1 ].Matrix[ 0 ][ 1 ] ) + NewKey[ -1 ].Matrix[ 0 ][ 1 ] ) - Key[ 0 ].Matrix[ 0 ][ 1 ] ; if( f > 0.00001f || f < -0.00001f ) goto MATRIX3X3ADD ;
							f = ( rate * ( Key[ 1 ].Matrix[ 0 ][ 2 ] - NewKey[ -1 ].Matrix[ 0 ][ 2 ] ) + NewKey[ -1 ].Matrix[ 0 ][ 2 ] ) - Key[ 0 ].Matrix[ 0 ][ 2 ] ; if( f > 0.00001f || f < -0.00001f ) goto MATRIX3X3ADD ;
							f = ( rate * ( Key[ 1 ].Matrix[ 1 ][ 0 ] - NewKey[ -1 ].Matrix[ 1 ][ 0 ] ) + NewKey[ -1 ].Matrix[ 1 ][ 0 ] ) - Key[ 0 ].Matrix[ 1 ][ 0 ] ; if( f > 0.00001f || f < -0.00001f ) goto MATRIX3X3ADD ;
							f = ( rate * ( Key[ 1 ].Matrix[ 1 ][ 1 ] - NewKey[ -1 ].Matrix[ 1 ][ 1 ] ) + NewKey[ -1 ].Matrix[ 1 ][ 1 ] ) - Key[ 0 ].Matrix[ 1 ][ 1 ] ; if( f > 0.00001f || f < -0.00001f ) goto MATRIX3X3ADD ;
							f = ( rate * ( Key[ 1 ].Matrix[ 1 ][ 2 ] - NewKey[ -1 ].Matrix[ 1 ][ 2 ] ) + NewKey[ -1 ].Matrix[ 1 ][ 2 ] ) - Key[ 0 ].Matrix[ 1 ][ 2 ] ; if( f > 0.00001f || f < -0.00001f ) goto MATRIX3X3ADD ;
							f = ( rate * ( Key[ 1 ].Matrix[ 2 ][ 0 ] - NewKey[ -1 ].Matrix[ 2 ][ 0 ] ) + NewKey[ -1 ].Matrix[ 2 ][ 0 ] ) - Key[ 0 ].Matrix[ 2 ][ 0 ] ; if( f > 0.00001f || f < -0.00001f ) goto MATRIX3X3ADD ;
							f = ( rate * ( Key[ 1 ].Matrix[ 2 ][ 1 ] - NewKey[ -1 ].Matrix[ 2 ][ 1 ] ) + NewKey[ -1 ].Matrix[ 2 ][ 1 ] ) - Key[ 0 ].Matrix[ 2 ][ 1 ] ; if( f > 0.00001f || f < -0.00001f ) goto MATRIX3X3ADD ;
							f = ( rate * ( Key[ 1 ].Matrix[ 2 ][ 2 ] - NewKey[ -1 ].Matrix[ 2 ][ 2 ] ) + NewKey[ -1 ].Matrix[ 2 ][ 2 ] ) - Key[ 0 ].Matrix[ 2 ][ 2 ] ; if( f > 0.00001f || f < -0.00001f ) goto MATRIX3X3ADD ;

							continue ;
						}

						if( k == KeySetR->Num - 1 && num == 1 )
						{
							if( *( ( DWORD * )&Key[ 0 ].Matrix[ 0 ][ 0 ] ) != *( ( DWORD * )&NewKey[ -1 ].Matrix[ 0 ][ 0 ] ) ||
								*( ( DWORD * )&Key[ 0 ].Matrix[ 0 ][ 1 ] ) != *( ( DWORD * )&NewKey[ -1 ].Matrix[ 0 ][ 1 ] ) ||
								*( ( DWORD * )&Key[ 0 ].Matrix[ 0 ][ 2 ] ) != *( ( DWORD * )&NewKey[ -1 ].Matrix[ 0 ][ 2 ] ) ||
								*( ( DWORD * )&Key[ 0 ].Matrix[ 1 ][ 0 ] ) != *( ( DWORD * )&NewKey[ -1 ].Matrix[ 1 ][ 0 ] ) ||
								*( ( DWORD * )&Key[ 0 ].Matrix[ 1 ][ 1 ] ) != *( ( DWORD * )&NewKey[ -1 ].Matrix[ 1 ][ 1 ] ) ||
								*( ( DWORD * )&Key[ 0 ].Matrix[ 1 ][ 2 ] ) != *( ( DWORD * )&NewKey[ -1 ].Matrix[ 1 ][ 2 ] ) ||
								*( ( DWORD * )&Key[ 0 ].Matrix[ 2 ][ 0 ] ) != *( ( DWORD * )&NewKey[ -1 ].Matrix[ 2 ][ 0 ] ) ||
								*( ( DWORD * )&Key[ 0 ].Matrix[ 2 ][ 1 ] ) != *( ( DWORD * )&NewKey[ -1 ].Matrix[ 2 ][ 1 ] ) ||
								*( ( DWORD * )&Key[ 0 ].Matrix[ 2 ][ 2 ] ) != *( ( DWORD * )&NewKey[ -1 ].Matrix[ 2 ][ 2 ] ) ) goto MATRIX3X3ADD ;
							continue ;
						}
MATRIX3X3ADD :
						*NewTime = *Time ;
						*NewKey  = *Key ;
						NewTime ++ ;
						NewKey  ++ ;
						num ++ ;
					}
				}
			}
		}
		break ;
	}

	if( AfterBuffer )
	{
		// 削減後のキーの数が１個か、
		// キーを減らして削減されるデータサイズが固定タイムにして
		// 削減できるデータサイズより大きいか、そもそも固定タイムに
		// できない場合はキーを減らす
		AfterSizeK = ( int )( num          * ( UnitSize + sizeof( float ) ) ) ;
		AfterSizeU = KeySetR->Num *   UnitSize ;
		if( num == 1 || ( ( Ittei == false || AfterSizeK < AfterSizeU ) && num < KeySetR->Num ) )
		{
			// キーが一つになる場合以外でキーを削除するフラグが無い場合は何もしない
			if( NotKeySub == false || num == 1 )
			{
				// キーデータのサイズを減らして増やす
				if( KeySetR->TimeType == MV1_ANIMKEY_TIME_TYPE_ONE )
				{
					ReadModel->AnimKeyDataSize -= UnitSize * KeySetR->Num ;
					ReadModel->AnimKeyDataSize += UnitSize * num ;
				}
				else
				{
					ReadModel->AnimKeyDataSize -= ( sizeof( float ) + UnitSize ) * KeySetR->Num ;
					ReadModel->AnimKeyDataSize += ( sizeof( float ) + UnitSize ) * num ;
				}
				KeySetR->KeyFloat4 = ( FLOAT4 * )AfterBuffer ;
				KeySetR->KeyTime = ( float * )( ( BYTE * )AfterBuffer + UnitSize * KeySetR->Num ) ;
				KeySetR->Num = num ;
			}

			// キーが一つの場合は更に固定タイムにする
			if( num == 1 && KeySetR->TimeType == MV1_ANIMKEY_TIME_TYPE_KEY )
			{
				// キーデータのサイズを減らす
				ReadModel->AnimKeyDataSize -= sizeof( float ) * num ;

				KeySetR->TimeType = MV1_ANIMKEY_TIME_TYPE_ONE ;
				KeySetR->StartTime = 0.0f ;
				KeySetR->UnitTime = KeySetR->TotalTime ;
			}
		}
		else
		// それ以外の場合で固定タイムにできる場合は固定タイムにする
		if( Ittei == true )
		{
			if( KeySetR->TimeType == MV1_ANIMKEY_TIME_TYPE_KEY )
			{
				// キーデータのサイズを減らす
				ReadModel->AnimKeyDataSize -= sizeof( float ) * KeySetR->Num ;

				KeySetR->TimeType = MV1_ANIMKEY_TIME_TYPE_ONE ;
				KeySetR->StartTime = KeySetR->KeyTime[ 0 ] ;
				KeySetR->UnitTime = ktime ;
			}
		}
	}
}

// メッシュ法線の自動計算( 全ポリゴンが三角形ポリゴンである必要があります )
static bool MV1MakeMeshNormals( MV1_MODEL_R *ReadModel, MV1_MESH_R *Mesh )
{
	MV1_MESHFACE_R *Face, *FaceT ;
	int i, j, k, m, Index ;
	VECTOR *P0, *P1, *P2, V1, V2, V3, Norm, tv, xv, yv, zv, FaceNorm, *Normal ;
	float SmoothCos, Sin ;
	float MinFaceRadi = 0, MinFaceRadi2 = 0, FaceRadi, FaceRadi2 ;
	int FaceCount, MinFaceRadi2Use = 0, FaceRadi2Use, CurFaceIndex, Issyuu ;
	MV1_MESHFACE_R *FaceList[ 1024 ], *CurFace, *MinFace, *BackCurFace ;
	BYTE FaceIndex[ 1024 ] ;
	DWORD IndexNum, UseNormalNum ;
	int MinFaceUra = 0, FaceUra = 0, CmpFlag, MinFaceIndex = 0 ;
	MV1_MAKEVERTINDEXINFO **VertexFaceList, *VertexFaceBuffer, *VFBuf ;
	BYTE *NormalUseTable ;
	DWORD *UseNormalIndex, *NewNormalIndex ;

	// スムージングを行う角度のコサイン値を求めておく
	if( Mesh->Container->SmoothingAngle < -0.00001f )
	{
		_SINCOS_PLATFORM( ( float )( 89.5f / 180.0f * DX_PI ), &Sin, &SmoothCos ) ;
	}
	else
	{
		_SINCOS_PLATFORM( Mesh->Container->SmoothingAngle, &Sin, &SmoothCos ) ;
	}

	// 面情報から幾つ法線が必要か調べる
	Face = Mesh->Faces ;
	Mesh->NormalNum = 0 ;
	for( i = 0 ; ( DWORD )i < Mesh->FaceNum ; i ++, Face ++ )
	{
		// ついでに面の法線を計算する
		P0 = &Mesh->Positions[ Face->VertexIndex[ 0 ] ] ;
		P1 = &Mesh->Positions[ Face->VertexIndex[ 1 ] ] ;
		P2 = &Mesh->Positions[ Face->VertexIndex[ 2 ] ] ;

		VectorSub( &V1, P1, P0 ) ;
		VectorSub( &V2, P2, P0 ) ;
		if( ReadModel->MeshFaceRightHand )
		{
			VectorOuterProduct( &Norm, &V2, &V1 ) ;
		}
		else
		{
			VectorOuterProduct( &Norm, &V1, &V2 ) ;
		}
		VectorNormalize( &Face->Normal, &Norm ) ;

		// 法線インデックスをセット
		for( j = 0 ; ( DWORD )j < Face->IndexNum ; j ++ )
			Face->NormalIndex[ j ] = Mesh->NormalNum + j ;

		// 法線の数をインデックスの数だけ増やす
		Mesh->NormalNum += Face->IndexNum ;
	}
	IndexNum = Mesh->NormalNum ;

	// 法線を格納するメモリを確保する
	Mesh->Normals = ( VECTOR * )DXALLOC( ( sizeof( VECTOR ) + sizeof( BYTE ) + sizeof( DWORD ) + sizeof( DWORD ) ) * Mesh->NormalNum ) ;
	if( Mesh->Normals == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x52\x00\x65\x00\x61\x00\x64\x00\x20\x00\x4d\x00\x6f\x00\x64\x00\x65\x00\x6c\x00\x20\x00\x43\x00\x6f\x00\x6e\x00\x76\x00\x65\x00\x72\x00\x74\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xd5\x6c\xda\x7d\xea\x81\xd5\x52\x5c\x4f\x10\x62\xe6\x51\x06\x74\x67\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xd5\x6c\xda\x7d\x6e\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Read Model Convert Error : 法線自動作成処理で格納する法線のメモリの確保に失敗しました\n" @*/ )) ;
		return false ;
	}
	_MEMSET( Mesh->Normals, 0, ( sizeof( VECTOR ) + sizeof( BYTE ) + sizeof( DWORD ) + sizeof( DWORD ) ) * Mesh->NormalNum ) ;
	Mesh->NormalSetFlag = ( BYTE * )( Mesh->Normals + Mesh->NormalNum ) ;
	NormalUseTable = ( BYTE * )( Mesh->Normals + Mesh->NormalNum ) ;
	UseNormalIndex = ( DWORD * )( NormalUseTable + Mesh->NormalNum ) ;
	NewNormalIndex = UseNormalIndex + Mesh->NormalNum ;

	// 面の頂点インデックスの末尾にインデックスを追加するためにバッファを拡張する
	if( MV1RSetupMeshFaceBuffer( ReadModel, Mesh, ( int )Mesh->FaceNum, ( int )( Mesh->FaceUnitMaxIndexNum * 2 ) ) < 0 )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x52\x00\x65\x00\x61\x00\x64\x00\x20\x00\x4d\x00\x6f\x00\x64\x00\x65\x00\x6c\x00\x20\x00\x43\x00\x6f\x00\x6e\x00\x76\x00\x65\x00\x72\x00\x74\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xd5\x6c\xda\x7d\xea\x81\xd5\x52\x5c\x4f\x10\x62\xe6\x51\x06\x74\x67\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\x02\x98\xb9\x70\xa4\x30\xf3\x30\xc7\x30\xc3\x30\xaf\x30\xb9\x30\x6e\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Read Model Convert Error : 法線自動作成処理で格納する頂点インデックスのメモリの確保に失敗しました\n" @*/ )) ;
		return false ;
	}

	// 全ての面の頂点インデックスの末尾にインデックスを代入する
	Face = Mesh->Faces ;
	for( i = 0 ; ( DWORD )i < Mesh->FaceNum ; i ++, Face ++ )
	{
		for( j = 0 ; ( DWORD )j < Face->IndexNum ; j ++ )
		{
			Face->VertexIndex[ Face->IndexNum + j ] = Face->VertexIndex[ j ] ;
		}
	}

	// 各頂点に関係する面のリストを作成する
	{
		VertexFaceList = ( MV1_MAKEVERTINDEXINFO ** )DXALLOC( sizeof( MV1_MAKEVERTINDEXINFO * ) * Mesh->PositionNum + sizeof( MV1_MAKEVERTINDEXINFO ) * IndexNum ) ;
		if( VertexFaceList == NULL )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x52\x00\x65\x00\x61\x00\x64\x00\x20\x00\x4d\x00\x6f\x00\x64\x00\x65\x00\x6c\x00\x20\x00\x43\x00\x6f\x00\x6e\x00\x76\x00\x65\x00\x72\x00\x74\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xa2\x95\xc2\x4f\x59\x30\x8b\x30\x62\x97\x6e\x30\xc5\x60\x31\x58\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Read Model Convert Error : 関係する面の情報を格納するメモリの確保に失敗しました\n" @*/ )) ;
			return false ;
		}
		_MEMSET( VertexFaceList, 0, sizeof( MV1_MAKEVERTINDEXINFO * ) * Mesh->PositionNum ) ;
		VertexFaceBuffer = ( MV1_MAKEVERTINDEXINFO * )( VertexFaceList + Mesh->PositionNum ) ;

		// 全ての面の参照している頂点の情報をセットする
		Face = Mesh->Faces ;
		VFBuf = VertexFaceBuffer ;
		for( i = 0 ; ( DWORD )i < Mesh->FaceNum ; i ++, Face ++ )
		{
			VFBuf->Face = Face ;
			VFBuf->Next = VertexFaceList[ Face->VertexIndex[ 0 ] ] ;
			VertexFaceList[ Face->VertexIndex[ 0 ] ] = VFBuf ;
			VFBuf ++ ;

			VFBuf->Face = Face ;
			VFBuf->Next = VertexFaceList[ Face->VertexIndex[ 1 ] ] ;
			VertexFaceList[ Face->VertexIndex[ 1 ] ] = VFBuf ;
			VFBuf ++ ;

			VFBuf->Face = Face ;
			VFBuf->Next = VertexFaceList[ Face->VertexIndex[ 2 ] ] ;
			VertexFaceList[ Face->VertexIndex[ 2 ] ] = VFBuf ;
			VFBuf ++ ;
		}
	}

	// 面の法線を構築する
	Face = Mesh->Faces ;
	for( i = 0 ; ( DWORD )i < Mesh->FaceNum ; i ++, Face ++ )
	{
		// 既に処理済みの場合は何もしない
		if( Mesh->NormalSetFlag[ Face->NormalIndex[ 0 ] ] &&
			Mesh->NormalSetFlag[ Face->NormalIndex[ 1 ] ] &&
			Mesh->NormalSetFlag[ Face->NormalIndex[ 2 ] ] )
			continue ;

		// 頂点の数だけ繰り返し
		for( j = 0 ; ( DWORD )j < Face->IndexNum ; j ++ )
		{
			// 既に計算済みの場合は何もしない
			if( Mesh->NormalSetFlag[ Face->NormalIndex[ j ] ] ) continue ;

			// 連結面をリストアップ
			{
				FaceList[ 0 ] = Face ;
				FaceNorm = Face->Normal ;
				FaceIndex[ 0 ] = ( BYTE )j ;
				Mesh->NormalSetFlag[ Face->NormalIndex[ j ] ] |= 2 ;
				FaceCount = 1 ;

				// 片方の連結面をリストアップ
				CurFace = Face ;
				BackCurFace = NULL ;
				CurFaceIndex = j ;
				Issyuu = 0 ;
				for(;;)
				{
					MinFace = NULL ;

					// 法線を割り出したい頂点から隣接しているか調べたい辺の頂点へ向かうベクトルをＺ、もう片方の頂点へ向かうベクトルをＸとした三軸の作成
					VectorSub( &zv, &Mesh->Positions[ CurFace->VertexIndex[ CurFaceIndex + 1 ] ], &Mesh->Positions[ CurFace->VertexIndex[ CurFaceIndex ] ] ) ;
					VectorSub( &xv, &Mesh->Positions[ CurFace->VertexIndex[ CurFaceIndex + 2 ] ], &Mesh->Positions[ CurFace->VertexIndex[ CurFaceIndex ] ] ) ;
					VectorOuterProduct( &yv, &zv, &xv ) ;
					VectorOuterProduct( &xv, &yv, &zv ) ;
					VectorNormalize( &xv, &xv ) ;
					VectorNormalize( &yv, &yv ) ;
					VectorNormalize( &zv, &zv ) ;

					// 隣接しているか調べたい辺の頂点とは別のもう一つの頂点へ向かうベクトルを上記で算出した三軸上に投影したベクトルを算出
					VectorSub( &tv, &Mesh->Positions[ CurFace->VertexIndex[ CurFaceIndex + 2 ] ], &Mesh->Positions[ CurFace->VertexIndex[ CurFaceIndex ] ] ) ;
					V1.x = xv.x * tv.x + xv.y * tv.y + xv.z * tv.z ;
					V1.y = yv.x * tv.x + yv.y * tv.y + yv.z * tv.z ;
					V1.z = zv.x * tv.x + zv.y * tv.y + zv.z * tv.z ;
					V1.z = 0.0f ;
					VectorNormalize( &V1, &V1 ) ;

					// 同一の辺を持つポリゴンの検索
					for( VFBuf = VertexFaceList[ CurFace->VertexIndex[ CurFaceIndex ] ] ; VFBuf ; VFBuf = VFBuf->Next )
					{
						FaceT = ( MV1_MESHFACE_R * )VFBuf->Face ;

						// 既に検出済みのポリゴンは無視
						if( ( Mesh->NormalSetFlag[ FaceT->NormalIndex[ 0 ] ] & 2 ) ||
							( Mesh->NormalSetFlag[ FaceT->NormalIndex[ 1 ] ] & 2 ) ||
							( Mesh->NormalSetFlag[ FaceT->NormalIndex[ 2 ] ] & 2 ) ) continue ;

						// 自分自身か、一つ前に自分だったポリゴンは無視
						if( FaceT == CurFace || FaceT == BackCurFace ) continue ;

						// 頂点の数だけ繰り返し
						for( m = 0 ; ( DWORD )m < FaceT->IndexNum ; m ++ )
						{
							CmpFlag = 0 ;

							// 同じ辺を持つか調べる、頂点指定の順番から面の方向を調べる
							if( CurFace->VertexIndex[ CurFaceIndex     ] == FaceT->VertexIndex[ m     ] &&
								CurFace->VertexIndex[ CurFaceIndex + 1 ] == FaceT->VertexIndex[ m + 1 ] )
							{
								CmpFlag = 1 ;
								FaceUra = 1 ;
							}

							if( CurFace->VertexIndex[ CurFaceIndex     ] == FaceT->VertexIndex[ m + 1 ] &&
								CurFace->VertexIndex[ CurFaceIndex + 1 ] == FaceT->VertexIndex[ m     ] )
							{
								CmpFlag = 1 ;
								FaceUra = 0 ;
							}

							// カレント面と同一の頂点を使用する１８０度回転した面( カレント面の裏面 )だったら検出したことを無かったことにする
							if( CmpFlag == 1 && CurFace->VertexIndex[ CurFaceIndex + 2 ] == FaceT->VertexIndex[ m + 2 ] )
							{
								CmpFlag = 0 ;
							}

							// 同一の辺が見つかったらループを抜ける
							if( CmpFlag != 0 ) break ;
						}

						// 同一の辺が見つかったら処理
						if( ( DWORD )m != FaceT->IndexNum )
						{
							// 見つけたポリゴンの、同一の辺を成す頂点とは別のもう一つの頂点へ向かうベクトルを先に算出した三軸へ投影したベクトルを算出する
							VectorSub( &tv, &Mesh->Positions[ FaceT->VertexIndex[ m + 2 ] ], &Mesh->Positions[ CurFace->VertexIndex[ CurFaceIndex ] ] ) ;
							V2.x = xv.x * tv.x + xv.y * tv.y + xv.z * tv.z ;
							V2.y = yv.x * tv.x + yv.y * tv.y + yv.z * tv.z ;
							V2.z = zv.x * tv.x + zv.y * tv.y + zv.z * tv.z ;
							V2.z = 0.0f ;

							// 二つの面を真横から見たときにできる線が成す角のコサイン値を調べる
							// ( カレント面の表向き方向に回転したときに二辺が成す角の角度を調べる )
							{
								// 外積を使って１８０度以上かどうかを判定
								VectorOuterProduct( &tv, &V1, &V2 ) ;
								VectorNormalize( &V2, &V2 ) ;
								if( tv.z < 0.0f )
								{
									// １８０度以上だった場合は１８０度以上だったことを記録した上で
									// カレント面側のベクトルを逆転して内積を求める( コサイン値を求める )
									FaceRadi = -1.0f ;
									FaceRadi2Use = 1 ;
									V3.x = -V1.x ;
									V3.y = -V1.y ;
									V3.z = -V1.z ;
									FaceRadi2 = VectorInnerProduct( &V3, &V2 ) ;
								}
								else
								{
									// １８０度以下だった場合はそのまま二つの線の内積を求める( コサイン値を求める )
									FaceRadi = VectorInnerProduct( &V1, &V2 ) ;
									FaceRadi2Use = 0 ;
									FaceRadi2 = 1.0f ;
								}
							}

							// まだ隣接する面を見つけていないか
							// 既に見つけているが今回の面の裏面か
							// 既に見つけている隣接面よりも角度的に近い面だったら隣接面として記録する
							if( MinFace == NULL ||
								( ( ( FaceT->VertexIndex[ 0 ] == MinFace->VertexIndex[ 0 ] &&
								      FaceT->VertexIndex[ 1 ] == MinFace->VertexIndex[ 2 ] &&
								      FaceT->VertexIndex[ 2 ] == MinFace->VertexIndex[ 1 ] ) ||
								    ( FaceT->VertexIndex[ 0 ] == MinFace->VertexIndex[ 1 ] &&
								      FaceT->VertexIndex[ 1 ] == MinFace->VertexIndex[ 0 ] &&
								      FaceT->VertexIndex[ 2 ] == MinFace->VertexIndex[ 2 ] ) ||
								    ( FaceT->VertexIndex[ 0 ] == MinFace->VertexIndex[ 2 ] &&
								      FaceT->VertexIndex[ 1 ] == MinFace->VertexIndex[ 1 ] &&
								      FaceT->VertexIndex[ 2 ] == MinFace->VertexIndex[ 0 ] ) ) && FaceUra == 0 && MinFaceUra == 1 ) ||
								( FaceRadi2Use == 0 && MinFaceRadi2Use == 1 ) ||
								( FaceRadi2Use == 1 && MinFaceRadi2Use == 1 && FaceRadi2 > MinFaceRadi2 ) ||
								( FaceRadi2Use == 0 && MinFaceRadi2Use == 0 && FaceRadi  > MinFaceRadi ) )
							{
								MinFace = FaceT ;
								MinFaceRadi = FaceRadi ;
								MinFaceRadi2Use = FaceRadi2Use ;
								MinFaceRadi2 = FaceRadi2 ;
								MinFaceUra = FaceUra ;
								MinFaceIndex = m + 1 ;
								if( ( DWORD )MinFaceIndex == FaceT->IndexNum ) MinFaceIndex = 0 ;
							}
						}
					}

					// 隣接面を発見できなかったか、隣接面が裏面だったか、隣接面との法線の角度差がスムージング対象の閾値を超えていたらエッジとみなす
					if( MinFace == NULL || MinFaceUra || VectorInnerProduct( &MinFace->Normal, &CurFace->Normal ) < SmoothCos )
						break ;

					// 発見した隣接面が検索開始の面だった場合は一周したということ
					if( MinFace == FaceList[ 0 ] )
					{
						Issyuu = 1 ;
						break ;
					}

					// 確定した隣接面を記録
					FaceList[ FaceCount ] = MinFace ;
					FaceIndex[ FaceCount ] = ( BYTE )MinFaceIndex ;
					VectorAdd( &FaceNorm, &FaceNorm, &MinFace->Normal ) ;
					Mesh->NormalSetFlag[ MinFace->NormalIndex[ MinFaceIndex ] ] |= 2 ;
					FaceCount ++ ;

					// カレント面の変更
					BackCurFace = CurFace ;
					CurFace = MinFace ;
					CurFaceIndex = MinFaceIndex ;
				}

				// もう片方の連結面をリストアップ
				if( Issyuu == 0 )
				{
					BackCurFace = NULL ;
					CurFace = Face ;
					CurFaceIndex = j ;
					for(;;)
					{
						MinFace = NULL ;

						// 法線を割り出したい頂点から隣接しているか調べたい辺の頂点へ向かうベクトルをＺ、もう片方の頂点へ向かうベクトルをＸとした３軸ベクトルの作成
						VectorSub( &zv, &Mesh->Positions[ CurFace->VertexIndex[ CurFaceIndex + CurFace->IndexNum - 1 ] ], &Mesh->Positions[ CurFace->VertexIndex[ CurFaceIndex ] ] ) ;
						VectorSub( &xv, &Mesh->Positions[ CurFace->VertexIndex[ CurFaceIndex + CurFace->IndexNum - 2 ] ], &Mesh->Positions[ CurFace->VertexIndex[ CurFaceIndex ] ] ) ;
						VectorOuterProduct( &yv, &zv, &xv ) ;
						VectorOuterProduct( &xv, &yv, &zv ) ;
						VectorNormalize( &xv, &xv ) ;
						VectorNormalize( &yv, &yv ) ;
						VectorNormalize( &zv, &zv ) ;

						// 隣接しているか調べたい辺の頂点とは別のもう一つの頂点へ向かうベクトルを上記で算出した三軸上に投影したベクトルを算出
						VectorSub( &tv, &Mesh->Positions[ CurFace->VertexIndex[ CurFaceIndex + CurFace->IndexNum - 2 ] ], &Mesh->Positions[ CurFace->VertexIndex[ CurFaceIndex ] ] ) ;
						V1.x = xv.x * tv.x + xv.y * tv.y + xv.z * tv.z ;
						V1.y = yv.x * tv.x + yv.y * tv.y + yv.z * tv.z ;
						V1.z = zv.x * tv.x + zv.y * tv.y + zv.z * tv.z ;
						V1.z = 0.0f ;
						VectorNormalize( &V1, &V1 ) ;

						// 同一の辺を持つポリゴンの検索
						for( VFBuf = VertexFaceList[ CurFace->VertexIndex[ CurFaceIndex ] ] ; VFBuf ; VFBuf = VFBuf->Next )
						{
							FaceT = ( MV1_MESHFACE_R * )VFBuf->Face ;

							// 既に検出済みのポリゴンは無視
							if( ( Mesh->NormalSetFlag[ FaceT->NormalIndex[ 0 ] ] & 2 ) ||
								( Mesh->NormalSetFlag[ FaceT->NormalIndex[ 1 ] ] & 2 ) ||
								( Mesh->NormalSetFlag[ FaceT->NormalIndex[ 2 ] ] & 2 ) ) continue ;

							// 自分自身か、一つ前に自分だったポリゴンは無視
							if( FaceT == CurFace || FaceT == BackCurFace ) continue ;

							// 頂点の数だけ繰り返し
							for( m = 0 ; ( DWORD )m < FaceT->IndexNum ; m ++ )
							{
								CmpFlag = 0 ;

								// 同じ辺を持つか調べる、頂点指定の順番から面の方向を調べる

								if( CurFace->VertexIndex[ CurFaceIndex                         ] == FaceT->VertexIndex[ m                       ] &&
									CurFace->VertexIndex[ CurFaceIndex + CurFace->IndexNum - 1 ] == FaceT->VertexIndex[ m + FaceT->IndexNum - 1 ] )
								{
									CmpFlag = 1 ;
									FaceUra = 1 ;
								}

								if( CurFace->VertexIndex[ CurFaceIndex                         ] == FaceT->VertexIndex[ m + FaceT->IndexNum - 1 ] &&
									CurFace->VertexIndex[ CurFaceIndex + CurFace->IndexNum - 1 ] == FaceT->VertexIndex[ m                       ] )
								{
									CmpFlag = 1 ;
									FaceUra = 0 ;
								}

								// カレント面と同一の頂点を使用する１８０度回転した面( カレント面の裏面 )だったら検出したことを無かったことにする
								if( CmpFlag == 1 && CurFace->VertexIndex[ CurFaceIndex + CurFace->IndexNum - 2 ] == FaceT->VertexIndex[ m + FaceT->IndexNum - 2 ] )
								{
									CmpFlag = 0 ;
								}

								// 同一の辺が見つかったらループを抜ける
								if( CmpFlag != 0 ) break ;
							}

							// 同一の辺が見つかったら処理
							if( ( DWORD )m != FaceT->IndexNum )
							{
								// 見つけたポリゴンの、同一の辺を成す頂点とは別のもう一つの頂点へ向かうベクトルを先に算出した三軸へ投影したベクトルを算出する
								VectorSub( &tv, &Mesh->Positions[ FaceT->VertexIndex[ m + FaceT->IndexNum - 2 ] ], &Mesh->Positions[ CurFace->VertexIndex[ CurFaceIndex ] ] ) ;
								V2.x = xv.x * tv.x + xv.y * tv.y + xv.z * tv.z ;
								V2.y = yv.x * tv.x + yv.y * tv.y + yv.z * tv.z ;
								V2.z = zv.x * tv.x + zv.y * tv.y + zv.z * tv.z ;
								V2.z = 0.0f ;

								// 二つの面を真横から見たときにできる線が成す角のコサイン値を調べる
								// ( カレント面の表向き方向に回転したときに二辺が成す角の角度を調べる )
								{
									// 外積を使って１８０度以上かどうかを判定
									VectorOuterProduct( &tv, &V1, &V2 ) ;
									VectorNormalize( &V2, &V2 ) ;
									if( tv.z > 0.0f )
									{
										// １８０度以上だった場合は１８０度以上だったことを記録した上で
										// カレント面側のベクトルを逆転して内積を求める( コサイン値を求める )
										FaceRadi = -1.0f ;
										FaceRadi2Use = 1 ;
										V3.x = -V1.x ;
										V3.y = -V1.y ;
										V3.z = -V1.z ;
										FaceRadi2 = VectorInnerProduct( &V3, &V2 ) ;
									}
									else
									{
										// １８０度以下だった場合はそのまま二つの線の内積を求める( コサイン値を求める )
										FaceRadi = VectorInnerProduct( &V1, &V2 ) ;
										FaceRadi2Use = 0 ;
										FaceRadi2 = 1.0f ;
									}
								}

								// まだ隣接する面を見つけていないか
								// 既に見つけているが今回の面の裏面か
								// 既に見つけている隣接面よりも角度的に近い面だったら隣接面として記録する
								if( MinFace == NULL ||
									( ( ( FaceT->VertexIndex[ 0 ] == MinFace->VertexIndex[ 0 ] &&
										  FaceT->VertexIndex[ 1 ] == MinFace->VertexIndex[ 2 ] &&
										  FaceT->VertexIndex[ 2 ] == MinFace->VertexIndex[ 1 ] ) ||
										( FaceT->VertexIndex[ 0 ] == MinFace->VertexIndex[ 1 ] &&
										  FaceT->VertexIndex[ 1 ] == MinFace->VertexIndex[ 0 ] &&
										  FaceT->VertexIndex[ 2 ] == MinFace->VertexIndex[ 2 ] ) ||
										( FaceT->VertexIndex[ 0 ] == MinFace->VertexIndex[ 2 ] &&
										  FaceT->VertexIndex[ 1 ] == MinFace->VertexIndex[ 1 ] &&
										  FaceT->VertexIndex[ 2 ] == MinFace->VertexIndex[ 0 ] ) ) && FaceUra == 0 && MinFaceUra == 1 ) ||
									( FaceRadi2Use == 0 && MinFaceRadi2Use == 1 ) ||
									( FaceRadi2Use == 1 && MinFaceRadi2Use == 1 && FaceRadi2 > MinFaceRadi2 ) ||
									( FaceRadi2Use == 0 && MinFaceRadi2Use == 0 && FaceRadi  > MinFaceRadi ) )
								{
									MinFace = FaceT ;
									MinFaceRadi = FaceRadi ;
									MinFaceRadi2Use = FaceRadi2Use ;
									MinFaceRadi2 = FaceRadi2 ;
									MinFaceUra = FaceUra ;
									MinFaceIndex = m - 1 ;
									if( MinFaceIndex < 0 ) MinFaceIndex += FaceT->IndexNum ;
								}
							}
						}

						// 隣接面を発見できなかったか、隣接面が裏面だったか、隣接面との法線の角度差がスムージング対象の閾値を超えていたらエッジとみなす
						if( MinFace == NULL || MinFaceUra || VectorInnerProduct( &MinFace->Normal, &CurFace->Normal ) < SmoothCos )
							break ;

						// 発見した隣接面が検索開始の面だった場合は一周したということ
						if( MinFace == FaceList[ 0 ] )
						{
							Issyuu = 1 ;
							break ;
						}

						// 確定した隣接面を記録
						FaceList[ FaceCount ] = MinFace ;
						FaceIndex[ FaceCount ] = ( BYTE )MinFaceIndex ;
						VectorAdd( &FaceNorm, &FaceNorm, &MinFace->Normal ) ;
						Mesh->NormalSetFlag[ MinFace->NormalIndex[ MinFaceIndex ] ] |= 2 ;
						FaceCount ++ ;

						// カレント面の変更
						BackCurFace = CurFace ;
						CurFace = MinFace ;
						CurFaceIndex = MinFaceIndex ;
					}
				}

				// 検出した面の法線を足したものを正規化
				VectorNormalize( &FaceNorm, &FaceNorm ) ;
			}

			// リストアップされた面に正規化した法線をセットする
			for( k = 0 ; k < FaceCount ; k ++ )
			{
				Index = ( int )FaceList[ k ]->NormalIndex[ FaceIndex[ k ] ] ;
				Mesh->Normals[ Index ] = FaceNorm ;
				Mesh->NormalSetFlag[ Index ] = 1 ;
				FaceList[ k ]->NormalIndex[ FaceIndex[ k ] ] = FaceList[ 0 ]->NormalIndex[ FaceIndex[ 0 ] ] ;
			}
		}
	}

	// 使用している法線をインデックスの若い順に詰める
	{
		// 使用されている法線のマップを作成する
		_MEMSET( NormalUseTable, 0, Mesh->NormalNum ) ;
		Face = Mesh->Faces ;
		UseNormalNum = 0 ;
		for( i = 0 ; ( DWORD )i < Mesh->FaceNum ; i ++, Face ++ )
		{
			if( NormalUseTable[ Face->NormalIndex[ 0 ] ] == 0 )
			{
				NormalUseTable[ Face->NormalIndex[ 0 ] ] = 1 ;
				UseNormalIndex[ UseNormalNum ] = Face->NormalIndex[ 0 ] ;
				NewNormalIndex[ Face->NormalIndex[ 0 ] ] = UseNormalNum ;
				UseNormalNum ++ ;
			}

			if( NormalUseTable[ Face->NormalIndex[ 1 ] ] == 0 )
			{
				NormalUseTable[ Face->NormalIndex[ 1 ] ] = 1 ;
				UseNormalIndex[ UseNormalNum ] = Face->NormalIndex[ 1 ] ;
				NewNormalIndex[ Face->NormalIndex[ 1 ] ] = UseNormalNum ;
				UseNormalNum ++ ;
			}

			if( NormalUseTable[ Face->NormalIndex[ 2 ] ] == 0 )
			{
				NormalUseTable[ Face->NormalIndex[ 2 ] ] = 1 ;
				UseNormalIndex[ UseNormalNum ] = Face->NormalIndex[ 2 ] ;
				NewNormalIndex[ Face->NormalIndex[ 2 ] ] = UseNormalNum ;
				UseNormalNum ++ ;
			}
		}

		// 最終版の法線を格納するためのメモリの確保
		Normal = Mesh->Normals ;
		Mesh->Normals = ( VECTOR * )ADDMEMAREA( sizeof( VECTOR ) * UseNormalNum, &ReadModel->Mem ) ;
		if( Mesh->Normals == NULL )
		{
			DXFREE( Normal ) ;
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x52\x00\x65\x00\x61\x00\x64\x00\x20\x00\x4d\x00\x6f\x00\x64\x00\x65\x00\x6c\x00\x20\x00\x43\x00\x6f\x00\x6e\x00\x76\x00\x65\x00\x72\x00\x74\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xd5\x6c\xda\x7d\xea\x81\xd5\x52\x5c\x4f\x10\x62\xe6\x51\x06\x74\x67\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xd5\x6c\xda\x7d\x6e\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x5f\x00\x20\x00\x32\x00\x0a\x00\x00"/*@ L"Read Model Convert Error : 法線自動作成処理で格納する法線のメモリの確保に失敗しました _ 2\n" @*/ )) ;
			return false ;
		}

		// 最終版の法線セットを作成する
		for( i = 0 ; ( DWORD )i < UseNormalNum ; i ++ )
		{
			Mesh->Normals[ i ] = Normal[ UseNormalIndex[ i ] ] ;
		}

		// 面データの法線インデックスを更新する
		Face = Mesh->Faces ;
		for( i = 0 ; ( DWORD )i < Mesh->FaceNum ; i ++, Face ++ )
		{
			Face->NormalIndex[ 0 ] = NewNormalIndex[ Face->NormalIndex[ 0 ] ] ;
			Face->NormalIndex[ 1 ] = NewNormalIndex[ Face->NormalIndex[ 1 ] ] ;
			Face->NormalIndex[ 2 ] = NewNormalIndex[ Face->NormalIndex[ 2 ] ] ;
		}

		// 最終的な法線の数をセット
		Mesh->NormalNum = UseNormalNum ;
	}

	// 一時的なデータを格納していたメモリ領域の解放
	DXFREE( Normal ) ;
	Normal = NULL ;

	// メモリの解放
	if( VertexFaceList )
	{
		DXFREE( VertexFaceList ) ;
		VertexFaceList = NULL ;
	}

	// 正常終了
	return true ;
}

// 回転行列を乗算する
static void MV1MultiplyRotateMatrix( MATRIX *DestBuffer, VECTOR *Rotate, int RotateOrder, bool RotateInverse )
{
	MATRIX RotMatX, RotMatY, RotMatZ ;

	if( RotateInverse )
	{
		CreateRotationXMatrix( &RotMatX, -Rotate->x ) ;
		CreateRotationYMatrix( &RotMatY, -Rotate->y ) ;
		CreateRotationZMatrix( &RotMatZ, -Rotate->z ) ;

		switch( RotateOrder )
		{
		case MV1_ROTATE_ORDER_XYZ :
			CreateMultiplyMatrix( DestBuffer, DestBuffer, &RotMatZ ) ;
			CreateMultiplyMatrix( DestBuffer, DestBuffer, &RotMatY ) ;
			CreateMultiplyMatrix( DestBuffer, DestBuffer, &RotMatX ) ;
			break ;

		case MV1_ROTATE_ORDER_XZY : 
			CreateMultiplyMatrix( DestBuffer, DestBuffer, &RotMatY ) ;
			CreateMultiplyMatrix( DestBuffer, DestBuffer, &RotMatZ ) ;
			CreateMultiplyMatrix( DestBuffer, DestBuffer, &RotMatX ) ;
			break ;

		case MV1_ROTATE_ORDER_YZX :
			CreateMultiplyMatrix( DestBuffer, DestBuffer, &RotMatX ) ;
			CreateMultiplyMatrix( DestBuffer, DestBuffer, &RotMatZ ) ;
			CreateMultiplyMatrix( DestBuffer, DestBuffer, &RotMatY ) ;
			break ;

		case MV1_ROTATE_ORDER_YXZ :
			CreateMultiplyMatrix( DestBuffer, DestBuffer, &RotMatZ ) ;
			CreateMultiplyMatrix( DestBuffer, DestBuffer, &RotMatX ) ;
			CreateMultiplyMatrix( DestBuffer, DestBuffer, &RotMatY ) ;
			break ;

		case MV1_ROTATE_ORDER_ZXY :
			CreateMultiplyMatrix( DestBuffer, DestBuffer, &RotMatY ) ;
			CreateMultiplyMatrix( DestBuffer, DestBuffer, &RotMatX ) ;
			CreateMultiplyMatrix( DestBuffer, DestBuffer, &RotMatZ ) ;
			break ;

		case MV1_ROTATE_ORDER_ZYX :
			CreateMultiplyMatrix( DestBuffer, DestBuffer, &RotMatX ) ;
			CreateMultiplyMatrix( DestBuffer, DestBuffer, &RotMatY ) ;
			CreateMultiplyMatrix( DestBuffer, DestBuffer, &RotMatZ ) ;
			break ;
		}
	}
	else
	{
		CreateRotationXMatrix( &RotMatX, Rotate->x ) ;
		CreateRotationYMatrix( &RotMatY, Rotate->y ) ;
		CreateRotationZMatrix( &RotMatZ, Rotate->z ) ;

		switch( RotateOrder )
		{
		case MV1_ROTATE_ORDER_XYZ :
			CreateMultiplyMatrix( DestBuffer, DestBuffer, &RotMatX ) ;
			CreateMultiplyMatrix( DestBuffer, DestBuffer, &RotMatY ) ;
			CreateMultiplyMatrix( DestBuffer, DestBuffer, &RotMatZ ) ;
			break ;

		case MV1_ROTATE_ORDER_XZY : 
			CreateMultiplyMatrix( DestBuffer, DestBuffer, &RotMatX ) ;
			CreateMultiplyMatrix( DestBuffer, DestBuffer, &RotMatZ ) ;
			CreateMultiplyMatrix( DestBuffer, DestBuffer, &RotMatY ) ;
			break ;

		case MV1_ROTATE_ORDER_YZX :
			CreateMultiplyMatrix( DestBuffer, DestBuffer, &RotMatY ) ;
			CreateMultiplyMatrix( DestBuffer, DestBuffer, &RotMatZ ) ;
			CreateMultiplyMatrix( DestBuffer, DestBuffer, &RotMatX ) ;
			break ;

		case MV1_ROTATE_ORDER_YXZ :
			CreateMultiplyMatrix( DestBuffer, DestBuffer, &RotMatY ) ;
			CreateMultiplyMatrix( DestBuffer, DestBuffer, &RotMatX ) ;
			CreateMultiplyMatrix( DestBuffer, DestBuffer, &RotMatZ ) ;
			break ;

		case MV1_ROTATE_ORDER_ZXY :
			CreateMultiplyMatrix( DestBuffer, DestBuffer, &RotMatZ ) ;
			CreateMultiplyMatrix( DestBuffer, DestBuffer, &RotMatX ) ;
			CreateMultiplyMatrix( DestBuffer, DestBuffer, &RotMatY ) ;
			break ;

		case MV1_ROTATE_ORDER_ZYX :
			CreateMultiplyMatrix( DestBuffer, DestBuffer, &RotMatZ ) ;
			CreateMultiplyMatrix( DestBuffer, DestBuffer, &RotMatY ) ;
			CreateMultiplyMatrix( DestBuffer, DestBuffer, &RotMatX ) ;
			break ;
		}
	}
}

// 座標変換行列の作成
extern int MV1RMakeMatrix( VECTOR *PreRotate, VECTOR *Rotate, VECTOR *PostRotate, VECTOR *Scale, VECTOR *Translate, VECTOR *MatrixRXYZ, MATRIX *DestBuffer, int RotateOrder )
{
	MATRIX TransMat, Matrix3x3 ;

	CreateScalingMatrix( DestBuffer, Scale->x, Scale->y, Scale->z ) ;

	if( PreRotate != NULL )
	{
		MV1MultiplyRotateMatrix( DestBuffer, PreRotate, RotateOrder, false ) ;
	}

	MV1MultiplyRotateMatrix( DestBuffer, Rotate, RotateOrder, false ) ;

	if( PostRotate != NULL )
	{
		MV1MultiplyRotateMatrix( DestBuffer, PostRotate, RotateOrder, true ) ;
	}

	if( MatrixRXYZ )
	{
		CreateRotationXYZMatrix( &Matrix3x3, MatrixRXYZ->x, MatrixRXYZ->y, MatrixRXYZ->z ) ;
		CreateMultiplyMatrix( DestBuffer, DestBuffer, &Matrix3x3 ) ;
	}

	CreateTranslationMatrix( &TransMat, Translate->x, Translate->y, Translate->z ) ;
	CreateMultiplyMatrix( DestBuffer, DestBuffer, &TransMat ) ;

	// 終了
	return 0 ;
}

// ファイルを丸ごと読み込む
extern int MV1RLoadFile( const char *FilePath, void **FileImage, int *FileSize )
{
	DWORD_PTR FileHandle ;
	size_t Size ;
	void *Image ;
	wchar_t FilePathW[ 1024 ] ;

	// テクスチャファイルを開く
	ConvString( ( const char * )FilePath, -1, DX_CHARCODEFORMAT_SHIFTJIS, ( char * )FilePathW, sizeof( FilePathW ), WCHAR_T_CHARCODEFORMAT ) ;
	FileHandle = DX_FOPEN( FilePathW ) ;

	// ファイルが開けなかったらカレントフォルダから開こうとしてみる
	if( FileHandle == 0 )
	{
		wchar_t FileName[ 512 ] ;

		AnalysisFileNameAndDirPathW_( FilePathW, FileName, sizeof( FileName ), NULL, 0 ) ;
		FileHandle = DX_FOPEN( FileName ) ;
	}

	// ファイルが無かったらエラー
	if( FileHandle == 0 )
		return -1 ;

	// ファイルサイズを取得
	DX_FSEEK( FileHandle, 0L, SEEK_END ) ;
	Size = ( size_t )DX_FTELL( FileHandle ) ;
	DX_FSEEK( FileHandle, 0L, SEEK_SET ) ;

	// テクスチャファイルが収まるメモリ領域の確保
	Image = DXALLOC( Size ) ;
	if( Image == NULL )
	{
		DX_FCLOSE( FileHandle ) ;
		return -1 ;
	}

	// ファイルを丸ごと読み込み
	DX_FREAD( Image, Size, 1, FileHandle ) ;

	// 開いたファイルを閉じる
	DX_FCLOSE( FileHandle ) ;

	// アドレスをセット
	if( FileImage ) *FileImage = Image ;
	if( FileSize ) *FileSize = ( int )Size ;

	// 終了
	return 0 ;
}

// ファイルを丸ごと読み込む
extern int MV1RLoadFileW( const wchar_t *FilePathW, void **FileImage, int *FileSize )
{
	DWORD_PTR FileHandle ;
	size_t Size ;
	void *Image ;

	// テクスチャファイルを開く
	FileHandle = DX_FOPEN( FilePathW ) ;

	// ファイルが開けなかったらカレントフォルダから開こうとしてみる
	if( FileHandle == 0 )
	{
		wchar_t FileNameW[ 512 ] ;

		AnalysisFileNameAndDirPathW_( FilePathW, FileNameW, sizeof( FileNameW ), NULL, 0 ) ;
		FileHandle = DX_FOPEN( FileNameW ) ;
	}

	// ファイルが無かったらエラー
	if( FileHandle == 0 )
		return -1 ;

	// ファイルサイズを取得
	DX_FSEEK( FileHandle, 0L, SEEK_END ) ;
	Size = ( size_t )DX_FTELL( FileHandle ) ;
	DX_FSEEK( FileHandle, 0L, SEEK_SET ) ;

	// テクスチャファイルが収まるメモリ領域の確保
	Image = DXALLOC( Size ) ;
	if( Image == NULL )
	{
		DX_FCLOSE( FileHandle ) ;
		return -1 ;
	}

	// ファイルを丸ごと読み込み
	DX_FREAD( Image, Size, 1, FileHandle ) ;

	// 開いたファイルを閉じる
	DX_FCLOSE( FileHandle ) ;

	// アドレスをセット
	if( FileImage ) *FileImage = Image ;
	if( FileSize ) *FileSize = ( int )Size ;

	// 終了
	return 0 ;
}

#ifndef UNICODE
// モデル基本データ用の文字列スペースを確保する
static char *MV1RGetStringSpace( MV1_MODEL_BASE *MBase, const char *String )
{
	char *Return ;

	Return = ( char * )( ( BYTE * )MBase->StringBufferA + MBase->StringSizeA ) ;
	_STRCPY( Return, String ) ;
	MBase->StringSizeA += ( ( ( int )_STRLEN( String ) + 1 ) * sizeof( char ) + 3 ) / 4 * 4 ;

	return Return ;
}
#endif

// モデル基本データ用の文字列スペースを確保する
static wchar_t *MV1RGetStringSpaceW( MV1_MODEL_BASE *MBase, const wchar_t *String )
{
	wchar_t *Return ;

	Return = ( wchar_t * )( ( BYTE * )MBase->StringBufferW + MBase->StringSizeW ) ;
	_WCSCPY( Return, String ) ;
	MBase->StringSizeW += ( int )( ( ( _WCSLEN( String ) + 1 ) * sizeof( wchar_t ) + 3 ) / 4 * 4 ) ;

	return Return ;
}

// 読み込み処理用モデル構造体の初期化
extern int MV1InitReadModel( MV1_MODEL_R *ReadModel )
{
	// ゼロ初期化
	_MEMSET( ReadModel, 0, sizeof( MV1_MODEL_R ) ) ;

	// 終了
	return 0 ;
}

// 読み込み処理用モデル構造体の後始末
extern int MV1TermReadModel( MV1_MODEL_R *ReadModel )
{
	// メモリの解放
	ClearMemArea( &ReadModel->Mem ) ;

	if( ReadModel->FilePath )
	{
		DXFREE( ReadModel->FilePath ) ;
		ReadModel->FilePath = NULL ;
	}

	if( ReadModel->Name )
	{
		DXFREE( ReadModel->Name ) ;
		ReadModel->Name = NULL ;
	}

	// 終了
	return 0 ;
}

#ifndef UNICODE

// 文字列の追加
extern char *MV1RAddString( MV1_MODEL_R *ReadModel, const char *String )
{
	int Length ;
	char *Buffer ;

	// 文字列の長さを取得
	Length = ( int )_STRLEN( String ) ;

	// メモリの確保
	Buffer = ( char * )AddMemArea( ( size_t )( ( Length + 1 ) * sizeof( char ) ), &ReadModel->Mem ) ;
	if( Buffer == NULL )
		return NULL ;

	// コピー
	_STRCPY( Buffer, String ) ;

	// 文字列データのサイズを加算
	ReadModel->StringSizeA += ( Length + 1 ) * sizeof( char ) ;

	// ４の倍数に合わせる
	ReadModel->StringSizeA = ( ReadModel->StringSizeA + 3 ) / 4 * 4 ;

	// 終了
	return Buffer ;
}

#endif

// 文字列の追加
extern wchar_t *MV1RAddStringW( MV1_MODEL_R *ReadModel, const wchar_t *StringW )
{
	int Length ;
	wchar_t *Buffer ;

	// 文字列の長さを取得
	Length = ( int )_WCSLEN( StringW ) ;

	// メモリの確保
	Buffer = ( wchar_t * )AddMemArea( ( size_t )( ( Length + 1 ) * sizeof( wchar_t ) ), &ReadModel->Mem ) ;
	if( Buffer == NULL )
		return NULL ;

	// コピー
	_WCSCPY( Buffer, StringW ) ;

	// 文字列データのサイズを加算
	ReadModel->StringSizeW += ( Length + 1 ) * sizeof( wchar_t ) ;

	// ４の倍数に合わせる
	ReadModel->StringSizeW = ( ReadModel->StringSizeW + 3 ) / 4 * 4 ;

	// 終了
	return Buffer ;
}

#ifndef UNICODE

// 文字列の追加
extern char *MV1RAddStringWToA(	MV1_MODEL_R *ReadModel, const wchar_t *String )
{
	char TempBuffer[ 512 ] ;

	ConvString( ( const char * )String, -1, WCHAR_T_CHARCODEFORMAT, TempBuffer, sizeof( TempBuffer ), ReadModel->CharCodeFormat ) ;
	return MV1RAddString( ReadModel, TempBuffer ) ;
}

#endif

// 文字列の追加
extern wchar_t *MV1RAddStringAToW( MV1_MODEL_R *ReadModel, const char *String )
{
	wchar_t TempBuffer[ 512 ] ;

	ConvString( String, -1, ReadModel->CharCodeFormat, ( char * )TempBuffer, sizeof( TempBuffer ), WCHAR_T_CHARCODEFORMAT ) ;
	return MV1RAddStringW( ReadModel, TempBuffer ) ;
}

// フレームの追加
static MV1_FRAME_R *MV1RAddFrameBase( MV1_MODEL_R *ReadModel, const char *NameA, const wchar_t *NameW, MV1_FRAME_R *Parent )
{
	MV1_FRAME_R *Frame ;
	MV1_FRAME_R *TempFrame ;
	int i ;
	BYTE TempBuffer[ 512 ] ;

	BYTE NameATempBuffer[ 512 ] ;
	if( NameA == NULL )
	{
		ConvString( ( const char * )NameW, -1, WCHAR_T_CHARCODEFORMAT, ( char * )TempBuffer, sizeof( TempBuffer ), CHAR_CHARCODEFORMAT ) ;
		NameA = ( const char * )TempBuffer ;
	}
	else
	{
		if( CHAR_CHARCODEFORMAT != ReadModel->CharCodeFormat )
		{
			ConvString( ( const char * )NameA, -1, ReadModel->CharCodeFormat, ( char * )NameATempBuffer, sizeof( NameATempBuffer ), CHAR_CHARCODEFORMAT ) ;
			NameA = ( const char * )NameATempBuffer ;
		}

		if( NameW == NULL )
		{
			ConvString( ( const char * )NameA, -1, CHAR_CHARCODEFORMAT, ( char * )TempBuffer, sizeof( TempBuffer ), WCHAR_T_CHARCODEFORMAT ) ;
			NameW = ( const wchar_t * )TempBuffer ;
		}
	}

	// メモリの確保
	Frame = ( MV1_FRAME_R * )AddMemArea( sizeof( MV1_FRAME_R ), &ReadModel->Mem ) ;
	if( Frame == NULL )
	{
		return NULL ;
	}

	// リストに追加
	if( ReadModel->FrameFirst == NULL )
	{
		ReadModel->FrameFirst = Frame ;
		ReadModel->FrameLast  = Frame ;
	}
	else
	{
		if( Parent == NULL )
		{
			TempFrame = ReadModel->FrameLast ;
		}
		else
		{
			if( Parent->ChildFirst == NULL )
			{
				TempFrame = Parent ;
			}
			else
			{
				for( TempFrame = Parent->ChildLast ; TempFrame->ChildLast ; TempFrame = TempFrame->ChildLast ){}
			}
		}

		Frame->DataPrev = TempFrame ;
		Frame->DataNext = TempFrame->DataNext ;
		TempFrame->DataNext = Frame ;
		if( Frame->DataNext ) Frame->DataNext->DataPrev = Frame ;

		if( ReadModel->FrameLast == TempFrame )
		{
			ReadModel->FrameLast = Frame ;
		}
	}

	// 親子リストに追加
	if( Parent )
	{
		if( Parent->ChildFirst == NULL )
		{
			Parent->ChildFirst = Frame ;
			Parent->ChildLast = Frame ;
		}
		else
		{
			Frame->Prev = Parent->ChildLast ;
			Parent->ChildLast->Next = Frame ;
			Parent->ChildLast = Frame ;
		}
		Frame->Parent = Parent ;
	}

	// インデックスの振りなおし
	for( i = 0, TempFrame = ReadModel->FrameFirst ; TempFrame ; i ++, TempFrame = TempFrame->DataNext )
	{
		TempFrame->Index = i ;
	}
	ReadModel->FrameNum ++ ;

	// 名前を保存
#ifndef UNICODE
	Frame->NameA = MV1RAddString( ReadModel, NameA ) ;
	if( Frame->NameA == NULL )
	{
		return NULL ;
	}
#endif
	Frame->NameW = MV1RAddStringW( ReadModel, NameW ) ;
	if( Frame->NameW == NULL )
	{
		return NULL ;
	}

	// 基本情報のセット
	CreateIdentityMatrix( &Frame->Matrix ) ;
	Frame->Scale.x = 1.0f ;
	Frame->Scale.y = 1.0f ;
	Frame->Scale.z = 1.0f ;
	Frame->Quaternion.w = 1.0f ; 
	Frame->RotateOrder = MV1_ROTATE_ORDER_XYZ ;
	Frame->Visible = 1 ;
	Frame->SmoothingAngle = DX_PI_F / 2.0f - 0.05f ;

	// 終了
	return Frame ;
}

// フレームの追加
extern MV1_FRAME_R *MV1RAddFrame( MV1_MODEL_R *ReadModel, const char *Name, MV1_FRAME_R *Parent )
{
	return MV1RAddFrameBase( ReadModel, Name, NULL, Parent ) ;
}

// フレームの追加
extern MV1_FRAME_R *MV1RAddFrameW( MV1_MODEL_R *ReadModel, const wchar_t *Name, MV1_FRAME_R *Parent )
{
	return MV1RAddFrameBase( ReadModel, NULL, Name, Parent ) ;
}

// メッシュの追加
extern MV1_MESH_R *MV1RAddMesh( MV1_MODEL_R *ReadModel, MV1_FRAME_R *Frame )
{
	MV1_MESH_R *Mesh ;

	// メモリの確保
	Mesh = ( MV1_MESH_R * )AddMemArea( sizeof( MV1_MESH_R ), &ReadModel->Mem ) ;
	if( Mesh == NULL )
		return NULL ;

	// リストに追加
	MODELLIST_ADD( ReadModel, Mesh ) ;
	if( Frame )
	{
		OBJLIST_ADD( Frame, Mesh ) ;
		Mesh->Container = Frame ;
	}

	// 終了
	return Mesh ;
}

// メッシュの面用のバッファをセットアップする
extern int MV1RSetupMeshFaceBuffer( MV1_MODEL_R *ReadModel, MV1_MESH_R *Mesh, int FaceNum, int MaxIndexNum )
{
	MV1_MESHFACE_R *OldFaces ;
//	DWORD *OldIndexBuffer ;
	DWORD OldMaxIndexNum ;
	DWORD OldFaceNum ;

	// 既に数が足りていたら何もしない
	if( Mesh->FaceIndexBuffer != NULL && ( int )Mesh->FaceUnitMaxIndexNum >= MaxIndexNum &&
		Mesh->Faces != NULL && ( int )Mesh->FaceNum >= FaceNum ) return 0 ;

	OldFaceNum     = Mesh->FaceNum ;
	OldFaces       = Mesh->Faces ;
//	OldIndexBuffer = Mesh->FaceIndexBuffer ;
	OldMaxIndexNum = Mesh->FaceUnitMaxIndexNum ;

	// メモリの確保
	Mesh->Faces = ( MV1_MESHFACE_R * )AddMemArea( ( ( MV1_READ_MAX_UV_NUM + 3 ) * MaxIndexNum * sizeof( DWORD ) + sizeof( MV1_MESHFACE_R ) ) * FaceNum, &ReadModel->Mem ) ;
	if( Mesh->Faces == NULL )
		return -1 ;
	Mesh->FaceIndexBuffer = ( DWORD * )( Mesh->Faces + FaceNum ) ;
	Mesh->FaceUnitMaxIndexNum = ( DWORD )MaxIndexNum ;
	Mesh->FaceNum             = ( DWORD )FaceNum ;

	// 各面のポインタをセットアップ
	{
		MV1_MESHFACE_R *Face ;
		DWORD *Index ;
		int i, j ;

		Face = Mesh->Faces ;
		Index = Mesh->FaceIndexBuffer ;
		for( i = 0 ; i < ( int )Mesh->FaceNum ; i ++, Face ++ )
		{
			Face->VertexIndex = Index ;			Index += Mesh->FaceUnitMaxIndexNum ;
			Face->NormalIndex = Index ;			Index += Mesh->FaceUnitMaxIndexNum ;
			Face->VertexColorIndex = Index ;	Index += Mesh->FaceUnitMaxIndexNum ;
			for( j = 0 ; j < MV1_READ_MAX_UV_NUM ; j ++ )
			{
				Face->UVIndex[ j ] = Index ;	Index += Mesh->FaceUnitMaxIndexNum ;
			}
		}
	}

	// 今までのデータがある場合はコピーと解放
	if( OldFaces )
	{
		MV1_MESHFACE_R *DestFace ;
		MV1_MESHFACE_R *SrcFace ;
		DWORD i, k, l ;

		DestFace = Mesh->Faces ;
		SrcFace = OldFaces ;
		for( i = 0 ; i < OldFaceNum ; i ++, DestFace ++, SrcFace ++ )
		{
			DestFace->IndexNum = SrcFace->IndexNum ;
			DestFace->PolygonNum = SrcFace->PolygonNum ;
			for( k = 0 ; k < OldMaxIndexNum ; k ++ )
			{
				DestFace->VertexIndex[ k ] = SrcFace->VertexIndex[ k ] ;
				DestFace->NormalIndex[ k ] = SrcFace->NormalIndex[ k ] ;
				DestFace->VertexColorIndex[ k ] = SrcFace->VertexColorIndex[ k ] ;
				for( l = 0 ; l < MV1_READ_MAX_UV_NUM ; l ++ )
				{
					DestFace->UVIndex[ l ][ k ] = SrcFace->UVIndex[ l ][ k ] ;
				}
			}
			DestFace->MaterialIndex = SrcFace->MaterialIndex ;
			DestFace->Normal = SrcFace->Normal ;
		}

		SubMemArea( &ReadModel->Mem, OldFaces ) ;
	}

	// 終了
	return 0 ;
}

// シェイプデータの追加
static MV1_SHAPE_R *MV1RAddShapeBase( MV1_MODEL_R *ReadModel, const char *NameA, const wchar_t *NameW, MV1_FRAME_R *Frame )
{
	MV1_SHAPE_R *Shape ;
	BYTE TempBuffer[ 512 ] ;

	BYTE NameATempBuffer[ 512 ] ;
	if( NameA == NULL )
	{
		ConvString( ( const char * )NameW, -1, WCHAR_T_CHARCODEFORMAT, ( char * )TempBuffer, sizeof( TempBuffer ), CHAR_CHARCODEFORMAT ) ;
		NameA = ( const char * )TempBuffer ;
	}
	else
	{
		if( CHAR_CHARCODEFORMAT != ReadModel->CharCodeFormat )
		{
			ConvString( ( const char * )NameA, -1, ReadModel->CharCodeFormat, ( char * )NameATempBuffer, sizeof( NameATempBuffer ), CHAR_CHARCODEFORMAT ) ;
			NameA = ( const char * )NameATempBuffer ;
		}

		if( NameW == NULL )
		{
			ConvString( ( const char * )NameA, -1, CHAR_CHARCODEFORMAT, ( char * )TempBuffer, sizeof( TempBuffer ), WCHAR_T_CHARCODEFORMAT ) ;
			NameW = ( const wchar_t * )TempBuffer ;
		}
	}

	// メモリの確保
	Shape = ( MV1_SHAPE_R * )AddMemArea( sizeof( MV1_SHAPE_R ), &ReadModel->Mem ) ;
	if( Shape == NULL )
		return NULL ;

	// 名前を保存
#ifndef UNICODE
	Shape->NameA = MV1RAddString( ReadModel, NameA ) ;
	if( Shape->NameA == NULL )
	{
		return NULL ;
	}
#endif
	Shape->NameW = MV1RAddStringW( ReadModel, NameW ) ;
	if( Shape->NameW == NULL )
	{
		return NULL ;
	}

	// リストに追加
	MODELLIST_ADD( ReadModel, Shape ) ;
	if( Frame )
	{
		OBJLIST_ADD( Frame, Shape ) ;
		Shape->Container = Frame ;
	}

	// 終了
	return Shape ;
}

// シェイプデータの追加
extern MV1_SHAPE_R *MV1RAddShape( MV1_MODEL_R *ReadModel, const char *Name, MV1_FRAME_R *Frame )
{
	return MV1RAddShapeBase( ReadModel, Name, NULL, Frame ) ;
}

// シェイプデータの追加
extern MV1_SHAPE_R *MV1RAddShapeW( MV1_MODEL_R *ReadModel, const wchar_t *Name, MV1_FRAME_R *Frame )
{
	return MV1RAddShapeBase( ReadModel, NULL, Name, Frame ) ;
}

// 物理演算用剛体データの追加
static MV1_PHYSICS_RIGIDBODY_R *MV1RAddPhysicsRididBodyBase( MV1_MODEL_R *ReadModel, const char *NameA, const wchar_t *NameW, MV1_FRAME_R *TargetFrame )
{
	MV1_PHYSICS_RIGIDBODY_R *PhysicsRigidBody ;
	BYTE TempBuffer[ 512 ] ;

	BYTE NameATempBuffer[ 512 ] ;
	if( NameA == NULL )
	{
		ConvString( ( const char * )NameW, -1, WCHAR_T_CHARCODEFORMAT, ( char * )TempBuffer, sizeof( TempBuffer ), CHAR_CHARCODEFORMAT ) ;
		NameA = ( const char * )TempBuffer ;
	}
	else
	{
		if( CHAR_CHARCODEFORMAT != ReadModel->CharCodeFormat )
		{
			ConvString( ( const char * )NameA, -1, ReadModel->CharCodeFormat, ( char * )NameATempBuffer, sizeof( NameATempBuffer ), CHAR_CHARCODEFORMAT ) ;
			NameA = ( const char * )NameATempBuffer ;
		}

		if( NameW == NULL )
		{
			ConvString( ( const char * )NameA, -1, CHAR_CHARCODEFORMAT, ( char * )TempBuffer, sizeof( TempBuffer ), WCHAR_T_CHARCODEFORMAT ) ;
			NameW = ( const wchar_t * )TempBuffer ;
		}
	}

	// メモリの確保
	PhysicsRigidBody = ( MV1_PHYSICS_RIGIDBODY_R * )AddMemArea( sizeof( MV1_PHYSICS_RIGIDBODY_R ), &ReadModel->Mem ) ;
	if( PhysicsRigidBody == NULL )
		return NULL ;

	// 名前を保存
#ifndef UNICODE
	PhysicsRigidBody->NameA = MV1RAddString( ReadModel, NameA ) ;
	if( PhysicsRigidBody->NameA == NULL )
	{
		return NULL ;
	}
#endif
	PhysicsRigidBody->NameW = MV1RAddStringW( ReadModel, NameW ) ;
	if( PhysicsRigidBody->NameW == NULL )
	{
		return NULL ;
	}

	// リストに追加
	MODELLIST_ADD( ReadModel, PhysicsRigidBody ) ;

	// 対象フレームをセット
	PhysicsRigidBody->TargetFrame = TargetFrame ;

	// 終了
	return PhysicsRigidBody ;
}

// 物理演算用剛体データの追加
extern MV1_PHYSICS_RIGIDBODY_R *MV1RAddPhysicsRididBody( MV1_MODEL_R *ReadModel, const char *Name, MV1_FRAME_R *TargetFrame )
{
	return MV1RAddPhysicsRididBodyBase( ReadModel, Name, NULL, TargetFrame ) ;
}

// 物理演算用剛体データの追加
extern MV1_PHYSICS_RIGIDBODY_R *MV1RAddPhysicsRididBodyW( MV1_MODEL_R *ReadModel, const wchar_t *Name, MV1_FRAME_R *TargetFrame )
{
	return MV1RAddPhysicsRididBodyBase( ReadModel, NULL, Name, TargetFrame ) ;
}


// 物理演算用剛体ジョイントデータの追加
static MV1_PHYSICS_JOINT_R *MV1RAddPhysicsJointBase( MV1_MODEL_R *ReadModel, const char *NameA, const wchar_t *NameW )
{
	MV1_PHYSICS_JOINT_R *PhysicsJoint ;
	BYTE TempBuffer[ 512 ] ;

	BYTE NameATempBuffer[ 512 ] ;
	if( NameA == NULL )
	{
		ConvString( ( const char * )NameW, -1, WCHAR_T_CHARCODEFORMAT, ( char * )TempBuffer, sizeof( TempBuffer ), CHAR_CHARCODEFORMAT ) ;
		NameA = ( const char * )TempBuffer ;
	}
	else
	{
		if( CHAR_CHARCODEFORMAT != ReadModel->CharCodeFormat )
		{
			ConvString( ( const char * )NameA, -1, ReadModel->CharCodeFormat, ( char * )NameATempBuffer, sizeof( NameATempBuffer ), CHAR_CHARCODEFORMAT ) ;
			NameA = ( const char * )NameATempBuffer ;
		}

		if( NameW == NULL )
		{
			ConvString( ( const char * )NameA, -1, CHAR_CHARCODEFORMAT, ( char * )TempBuffer, sizeof( TempBuffer ), WCHAR_T_CHARCODEFORMAT ) ;
			NameW = ( const wchar_t * )TempBuffer ;
		}
	}

	// メモリの確保
	PhysicsJoint = ( MV1_PHYSICS_JOINT_R * )AddMemArea( sizeof( MV1_PHYSICS_JOINT_R ), &ReadModel->Mem ) ;
	if( PhysicsJoint == NULL )
		return NULL ;

	// 名前を保存
#ifndef UNICODE
	PhysicsJoint->NameA = MV1RAddString( ReadModel, NameA ) ;
	if( PhysicsJoint->NameA == NULL )
	{
		return NULL ;
	}
#endif
	PhysicsJoint->NameW = MV1RAddStringW( ReadModel, NameW ) ;
	if( PhysicsJoint->NameW == NULL )
	{
		return NULL ;
	}

	// リストに追加
	MODELLIST_ADD( ReadModel, PhysicsJoint ) ;

	// 終了
	return PhysicsJoint ;
}

// 物理演算用剛体ジョイントデータの追加
extern MV1_PHYSICS_JOINT_R *MV1RAddPhysicsJoint( MV1_MODEL_R *ReadModel, const char *Name )
{
	return MV1RAddPhysicsJointBase( ReadModel, Name, NULL ) ;
}

// 物理演算用剛体ジョイントデータの追加
extern MV1_PHYSICS_JOINT_R *MV1RAddPhysicsJointW( MV1_MODEL_R *ReadModel, const wchar_t *Name )
{
	return MV1RAddPhysicsJointBase( ReadModel, NULL, Name ) ;
}

// メッシュの削除
extern void MV1RSubMesh( MV1_MODEL_R *ReadModel, MV1_FRAME_R *Frame, MV1_MESH_R *Mesh )
{
	// モデルのリストから外す
	if( Mesh->DataPrev )
	{
		Mesh->DataPrev->DataNext = Mesh->DataNext ;
	}
	else
	{
		ReadModel->MeshFirst = Mesh->DataNext ;
	}

	if( Mesh->DataNext )
	{
		Mesh->DataNext->DataPrev = Mesh->DataPrev ;
	}
	else
	{
		ReadModel->MeshLast = Mesh->DataPrev ;
	}
	ReadModel->MeshNum -- ;

	// フレームのリストから外す
	if( Mesh->Prev )
	{
		Mesh->Prev->Next = Mesh->Next ;
	}
	else
	{
		Frame->MeshFirst = Mesh->Next ;
	}

	if( Mesh->Next )
	{
		Mesh->Next->Prev = Mesh->Prev ;
	}
	else
	{
		Frame->MeshLast = Mesh->Prev ;
	}
	Frame->MeshNum -- ;
}

// マテリアルの追加
static MV1_MATERIAL_R *MV1RAddMaterialBase( MV1_MODEL_R *ReadModel, const char *NameA, const wchar_t *NameW )
{
	MV1_MATERIAL_R *Material ;
	BYTE TempBuffer[ 512 ] ;

	BYTE NameATempBuffer[ 512 ] ;
	if( NameA == NULL )
	{
		ConvString( ( const char * )NameW, -1, WCHAR_T_CHARCODEFORMAT, ( char * )TempBuffer, sizeof( TempBuffer ), CHAR_CHARCODEFORMAT ) ;
		NameA = ( const char * )TempBuffer ;
	}
	else
	{
		if( CHAR_CHARCODEFORMAT != ReadModel->CharCodeFormat )
		{
			ConvString( ( const char * )NameA, -1, ReadModel->CharCodeFormat, ( char * )NameATempBuffer, sizeof( NameATempBuffer ), CHAR_CHARCODEFORMAT ) ;
			NameA = ( const char * )NameATempBuffer ;
		}

		if( NameW == NULL )
		{
			ConvString( ( const char * )NameA, -1, CHAR_CHARCODEFORMAT, ( char * )TempBuffer, sizeof( TempBuffer ), WCHAR_T_CHARCODEFORMAT ) ;
			NameW = ( const wchar_t * )TempBuffer ;
		}
	}

	// メモリの確保
	Material = ( MV1_MATERIAL_R * )AddMemArea( sizeof( MV1_MATERIAL_R ), &ReadModel->Mem ) ;
	if( Material == NULL )
		return NULL ;

	// リストに追加
	MODELLIST_ADD( ReadModel, Material ) ;

	// 名前を保存
#ifndef UNICODE
	Material->NameA = MV1RAddString( ReadModel, NameA ) ;
	if( Material->NameA == NULL )
	{
		return NULL ;
	}
#endif
	Material->NameW = MV1RAddStringW( ReadModel, NameW ) ;
	if( Material->NameW == NULL )
	{
		return NULL ;
	}

	// 初期値をセット
	Material->Type = DX_MATERIAL_TYPE_NORMAL ;

	Material->DiffuseGradTextureDefault = -1 ;
	Material->SpecularGradTextureDefault = -1 ;
	Material->DiffuseGradBlendType = DX_MATERIAL_BLENDTYPE_MODULATE ;
	Material->SpecularGradBlendType = DX_MATERIAL_BLENDTYPE_ADDITIVE ;
	Material->SphereMapBlendType = DX_MATERIAL_BLENDTYPE_MODULATE ;

	Material->Diffuse.r = 1.0f ;
	Material->Diffuse.g = 1.0f ;
	Material->Diffuse.b = 1.0f ;
	Material->Diffuse.a = 1.0f ;

	Material->DrawBlendMode = DX_BLENDMODE_ALPHA ;
	Material->DrawBlendParam = 255 ;

	// 終了
	return Material ;
}

// マテリアルの追加
extern MV1_MATERIAL_R *MV1RAddMaterial( MV1_MODEL_R *ReadModel, const char *Name )
{
	return MV1RAddMaterialBase( ReadModel, Name, NULL ) ;
}

// マテリアルの追加
extern MV1_MATERIAL_R *MV1RAddMaterialW( MV1_MODEL_R *ReadModel, const wchar_t *Name )
{
	return MV1RAddMaterialBase( ReadModel, NULL, Name ) ;
}

// 指定インデックスのマテリアルを取得する
extern MV1_MATERIAL_R *MV1RGetMaterial( MV1_MODEL_R *ReadModel, int Index )
{
	MV1_MATERIAL_R *Material ;
	int i ;

	Material = ReadModel->MaterialFirst ;
	for( i = 0 ; i < Index ; i ++, Material = Material->DataNext ){}

	return Material ;
}

// テクスチャの追加
static MV1_TEXTURE_R *MV1RAddTextureBase(
	MV1_MODEL_R *ReadModel,
	const char *NameA,          const wchar_t *NameW,
	const char *ColorFilePathA, const wchar_t *ColorFilePathW,
	const char *AlphaFilePathA, const wchar_t *AlphaFilePathW,
	int BumpMapFlag, float BumpMapNextPixelLength,
	bool FilePathDoubleCancel,
	bool ReverseFlag,
	bool Bmp32AllZeroAlphaToXRGB8Flag,
	bool OpenFailedReturn )
{
	MV1_TEXTURE_R *Texture ;
	DWORD_PTR FileHandle ;
	DWORD i ;
	wchar_t ColorFileTempPath[ 512 ],      AlphaFileTempPath[ 512 ] ;
	wchar_t ColorFileRelativePath[ 1024 ], AlphaFileRelativePath[ 1024 ] ;
	const wchar_t *ColorOpenFilePathW = NULL, *AlphaOpenFilePathW = NULL ;
	const char    *ColorOpenFilePathA = NULL, *AlphaOpenFilePathA = NULL ;
	wchar_t CurrentDirectory[ 1024 ] ;
	BYTE TempNameBuffer[ 512 ] ;
	BYTE TempNameABuffer[ 512 ] ;
	BYTE TempColorFilePathBuffer[ 2048 ] ;
	BYTE TempAlphaFilePathBuffer[ 2048 ] ;
#ifndef UNICODE
	BYTE TempColorFilePathABuffer[ 2048 ] ;
	BYTE TempAlphaFilePathABuffer[ 2048 ] ;
#endif

	if( NameA == NULL )
	{
		ConvString( ( const char * )NameW, -1, WCHAR_T_CHARCODEFORMAT, ( char * )TempNameBuffer, sizeof( TempNameBuffer ), CHAR_CHARCODEFORMAT ) ;
		NameA = ( const char * )TempNameBuffer ;
	}
	else
	if( NameA != NULL && CHAR_CHARCODEFORMAT != ReadModel->CharCodeFormat )
	{
		ConvString( ( const char * )NameA, -1, ReadModel->CharCodeFormat, ( char * )TempNameABuffer, sizeof( TempNameABuffer ), CHAR_CHARCODEFORMAT ) ;
		NameA = ( const char * )TempNameABuffer ;
	}
	if( NameW == NULL )
	{
		ConvString( ( const char * )NameA, -1, CHAR_CHARCODEFORMAT, ( char * )TempNameBuffer, sizeof( TempNameBuffer ), WCHAR_T_CHARCODEFORMAT ) ;
		NameW = ( const wchar_t * )TempNameBuffer ;
	}

	if( ColorFilePathA != NULL || ColorFilePathW != NULL )
	{
#ifndef UNICODE
		if( ColorFilePathA == NULL )
		{
			ConvString( ( const char * )ColorFilePathW, -1, WCHAR_T_CHARCODEFORMAT, ( char * )TempColorFilePathBuffer, sizeof( TempColorFilePathBuffer ), CHAR_CHARCODEFORMAT ) ;
			ColorFilePathA = ( const char * )TempColorFilePathBuffer ;
		}
		else
		if( ColorFilePathA != NULL && CHAR_CHARCODEFORMAT != ReadModel->CharCodeFormat )
		{
			ConvString( ( const char * )ColorFilePathA, -1, ReadModel->CharCodeFormat, ( char * )TempColorFilePathABuffer, sizeof( TempColorFilePathABuffer ), CHAR_CHARCODEFORMAT ) ;
			ColorFilePathA = ( const char * )TempColorFilePathABuffer ;
		}
#endif
		if( ColorFilePathW == NULL )
		{
			ConvString( ( const char * )ColorFilePathA, -1, CHAR_CHARCODEFORMAT, ( char * )TempColorFilePathBuffer, sizeof( TempColorFilePathBuffer ), WCHAR_T_CHARCODEFORMAT ) ;
			ColorFilePathW = ( const wchar_t * )TempColorFilePathBuffer ;
		}
	}

	if( AlphaFilePathA != NULL || AlphaFilePathW != NULL )
	{
#ifndef UNICODE
		if( AlphaFilePathA == NULL )
		{
			ConvString( ( const char * )AlphaFilePathW, -1, WCHAR_T_CHARCODEFORMAT, ( char * )TempAlphaFilePathBuffer, sizeof( TempAlphaFilePathBuffer ), CHAR_CHARCODEFORMAT ) ;
			AlphaFilePathA = ( const char * )TempAlphaFilePathBuffer ;
		}
		else
		if( AlphaFilePathA != NULL && CHAR_CHARCODEFORMAT != ReadModel->CharCodeFormat )
		{
			ConvString( ( const char * )AlphaFilePathA, -1, ReadModel->CharCodeFormat, ( char * )TempAlphaFilePathABuffer, sizeof( TempAlphaFilePathABuffer ), CHAR_CHARCODEFORMAT ) ;
			ColorFilePathA = ( const char * )TempAlphaFilePathABuffer ;
		}
#endif
		if( AlphaFilePathW == NULL )
		{
			ConvString( ( const char * )AlphaFilePathA, -1, CHAR_CHARCODEFORMAT, ( char * )TempAlphaFilePathBuffer, sizeof( TempAlphaFilePathBuffer ), WCHAR_T_CHARCODEFORMAT ) ;
			AlphaFilePathW = ( const wchar_t * )TempAlphaFilePathBuffer ;
		}
	}

	// 相対パスを求める
	_WGETCWD( CurrentDirectory, sizeof( CurrentDirectory ) ) ;

	// カラーチャンネルファイルパスを保存
	if( ColorFilePathW )
	{
		CreateRelativePathW_( ColorFilePathW, CurrentDirectory, ColorFileRelativePath, sizeof( ColorFileRelativePath ) ) ;

		// ファイルが開けるか調べる
		FileHandle = DX_FOPEN( ColorFileRelativePath ) ;
		if( FileHandle )
		{
			ColorOpenFilePathW = ColorFileRelativePath ;
			DX_FCLOSE( FileHandle ) ;
		}
		else
		{
			// 開けなかったらカレントフォルダから開こうとしてみる
			AnalysisFileNameAndDirPathW_( ColorFileRelativePath, ColorFileTempPath, sizeof( ColorFileTempPath ), NULL, 0 ) ;
			FileHandle = DX_FOPEN( ColorFileTempPath ) ;
			if( FileHandle )
			{
				ColorOpenFilePathW = ColorFileTempPath ;
				DX_FCLOSE( FileHandle ) ;
			}
			else
			{
				// ファイルが開けなかったらリターンをする指定の場合は関数から抜ける
				if( OpenFailedReturn )
				{
					return NULL ;
				}
				ColorOpenFilePathW = ColorFileRelativePath ;
			}
		}

		// ファイルが開けた場合でバンプマップの指定だった場合はグレースケール画像か調べる
		if( BumpMapFlag )
		{
			void *ColorFileImage ;
			int ColorFileSize ;

			if( MV1RLoadFileW( ColorOpenFilePathW, &ColorFileImage, &ColorFileSize ) >= 0 )
			{
				BASEIMAGE ColorImage ;

				if( NS_CreateBaseImage( NULL, ColorFileImage, ColorFileSize, LOADIMAGE_TYPE_MEM, &ColorImage, FALSE ) >= 0 )
				{
					// バンプマップではなく法線マップの可能性があるので、FloatタイプでRGBチャンネルがあるか、
					// 8bitカラーよりビット深度が大きく、RGBチャンネルがある場合はグレースケールかどうかをチェックする
					if( ( ColorImage.ColorData.FloatTypeFlag == TRUE &&
						  ColorImage.ColorData.ChannelNum >= 3 ) ||

						( ColorImage.ColorData.FloatTypeFlag == FALSE &&
							ColorImage.ColorData.ColorBitDepth > 8 &&
						  ( ColorImage.ColorData.GreenWidth > 0 &&
							ColorImage.ColorData.BlueWidth  > 0 ) ) )
					{
						int j ;

						for( i = 0 ; i < ( DWORD )ColorImage.Height ; i ++ )
						{
							for( j = 0 ; j < ColorImage.Width ; j ++ )
							{
								int r, g, b ;

								NS_GetPixelBaseImage( &ColorImage, j, ( int )i, &r, &g, &b, NULL ) ;

								if( r != g || r != b )
								{
									BumpMapFlag = FALSE ;
									break ;
								}
							}
							if( j != ColorImage.Width )
							{
								break ;
							}
						}
					}

					NS_ReleaseBaseImage( &ColorImage ) ;
				}

				DXFREE( ColorFileImage ) ;
			}
		}
	}

	// アルファチャンネル用ファイルパスを保存
	if( AlphaFilePathW )
	{
		CreateRelativePathW_( AlphaFilePathW, CurrentDirectory, AlphaFileRelativePath, sizeof( AlphaFileRelativePath ) ) ;

		// ファイルが開けるか調べる
		FileHandle = DX_FOPEN( AlphaFileRelativePath ) ;
		if( FileHandle )
		{
			DX_FCLOSE( FileHandle ) ;
			AlphaOpenFilePathW = AlphaFileRelativePath ;
		}
		else
		{
			// 開けなかったらカレントフォルダから開こうとしてみる
			AnalysisFileNameAndDirPathW_( AlphaFileRelativePath, AlphaFileTempPath, sizeof( AlphaFileTempPath ), NULL, 0 ) ;
			FileHandle = DX_FOPEN( AlphaFileTempPath ) ;
			if( FileHandle )
			{
				DX_FCLOSE( FileHandle ) ;
				AlphaOpenFilePathW = AlphaFileTempPath ;
			}
			else
			{
				AlphaOpenFilePathW = AlphaFileRelativePath ;
			}
		}
	}

	// ファイルパスが同じテクスチャがあったらキャンセルするフラグが立っていたら
	// ファイルパスが同じテクスチャがあるか調べる
	if( FilePathDoubleCancel )
	{
		Texture = ReadModel->TextureFirst ;
		for( i = 0 ; ( DWORD )i < ReadModel->TextureNum ; i ++, Texture = Texture->DataNext )
		{
			if( ColorFilePathW )
			{
				if( Texture->ColorFileNameW == NULL ) continue ;
				if( _WCSCMP( Texture->ColorFileNameW, ColorOpenFilePathW ) != 0 ) continue ;
			}
			else
			{
				if( Texture->ColorFileNameW != NULL ) continue ;
			}

			if( AlphaFilePathW )
			{
				if( Texture->AlphaFileNameW == NULL ) continue ;
				if( _WCSCMP( Texture->AlphaFileNameW, AlphaOpenFilePathW ) != 0 ) continue ;
			}
			else
			{
				if( Texture->AlphaFileNameW != NULL ) continue ;
			}
			break ;
		}
		if( ( DWORD )i != ReadModel->TextureNum ) return Texture ;
	}

	// メモリの確保
	Texture = ( MV1_TEXTURE_R * )AddMemArea( sizeof( MV1_TEXTURE_R ), &ReadModel->Mem ) ;
	if( Texture == NULL )
	{
		return NULL ;
	}

	// リストに追加
	MODELLIST_ADD( ReadModel, Texture ) ;

	// 名前を保存
#ifndef UNICODE
	Texture->NameA = MV1RAddString( ReadModel, NameA ) ;
	if( Texture->NameA == NULL )
	{
		return NULL ;
	}
#endif
	Texture->NameW = MV1RAddStringW( ReadModel, NameW ) ;
	if( Texture->NameW == NULL )
	{
		return NULL ;
	}

	// デフォルトのアドレスモードをセットする
	Texture->AddressModeU = DX_TEXADDRESS_WRAP ;
	Texture->AddressModeV = DX_TEXADDRESS_WRAP ;

	// ＵＶ座標のデフォルトのスケール値をセットする
	Texture->ScaleU = 1.0f ;
	Texture->ScaleV = 1.0f ;
	
	// デフォルトのフィルタリングモードをセットする
	Texture->FilterMode = DX_DRAWMODE_ANISOTROPIC ;

	// カラーチャンネルがバンプマップかどうかのフラグを保存する
	Texture->BumpMapFlag = BumpMapFlag ;

	// カラーチャンネルがバンプマップの場合、隣のピクセルとの距離を保存する
	Texture->BumpMapNextPixelLength = BumpMapNextPixelLength ;

	// 反転フラグを保存する
	Texture->ReverseFlag = ReverseFlag ? 1 : 0 ;

	// ３２ビットＢＭＰのアルファ値が全部０だったら XRGB8 として扱うかどうかのフラグをセットする
	Texture->Bmp32AllZeroAlphaToXRGB8Flag = Bmp32AllZeroAlphaToXRGB8Flag ? 1 : 0 ;

	// カラーチャンネルファイルパスを保存
	if( ColorFilePathW )
	{
#ifndef UNICODE
		ConvString( ( const char * )ColorOpenFilePathW, -1, WCHAR_T_CHARCODEFORMAT, ( char * )TempColorFilePathBuffer, sizeof( TempColorFilePathBuffer ), ReadModel->CharCodeFormat ) ;
		ColorOpenFilePathA = ( const char * )TempColorFilePathBuffer ;

		Texture->ColorFileNameA = MV1RAddString( ReadModel, ColorOpenFilePathA ) ;
		if( Texture->ColorFileNameA == NULL )
		{
			return NULL ;
		}
#endif

		Texture->ColorFileNameW = MV1RAddStringW( ReadModel, ColorOpenFilePathW ) ;
		if( Texture->ColorFileNameW == NULL )
		{
			return NULL ;
		}
	}

	// アルファチャンネル用ファイルパスを保存
	if( AlphaFilePathW )
	{
#ifndef UNICODE
		ConvString( ( const char * )AlphaOpenFilePathW, -1, WCHAR_T_CHARCODEFORMAT, ( char * )TempAlphaFilePathBuffer, sizeof( TempAlphaFilePathBuffer ), ReadModel->CharCodeFormat ) ;
		AlphaOpenFilePathA = ( const char * )TempAlphaFilePathBuffer ;

		Texture->AlphaFileNameA = MV1RAddString( ReadModel, AlphaOpenFilePathA ) ;
		if( Texture->AlphaFileNameA == NULL ) 
		{
			return NULL ;
		}
#endif

		Texture->AlphaFileNameW = MV1RAddStringW( ReadModel, AlphaOpenFilePathW ) ;
		if( Texture->AlphaFileNameW == NULL ) 
		{
			return NULL ;
		}
	}

	// 終了
	return Texture ;
}

// テクスチャの追加
extern MV1_TEXTURE_R *MV1RAddTexture(
	MV1_MODEL_R *ReadModel,
	const char *Name,
	const char *ColorFilePath,
	const char *AlphaFilePath,
	int BumpMapFlag, float BumpMapNextPixelLength,
	bool FilePathDoubleCancel,
	bool ReverseFlag,
	bool Bmp32AllZeroAlphaToXRGB8Flag,
	bool OpenFailedReturn )
{
	return MV1RAddTextureBase( ReadModel, Name, NULL, ColorFilePath, NULL, AlphaFilePath, NULL, BumpMapFlag, BumpMapNextPixelLength, FilePathDoubleCancel, ReverseFlag, Bmp32AllZeroAlphaToXRGB8Flag, OpenFailedReturn) ;
}

// テクスチャの追加
extern MV1_TEXTURE_R *MV1RAddTextureW(
	MV1_MODEL_R *ReadModel,
	const wchar_t *Name,
	const wchar_t *ColorFilePath,
	const wchar_t *AlphaFilePath,
	int BumpMapFlag, float BumpMapNextPixelLength,
	bool FilePathDoubleCancel,
	bool ReverseFlag,
	bool Bmp32AllZeroAlphaToXRGB8Flag,
	bool OpenFailedReturn )
{
	return MV1RAddTextureBase( ReadModel, NULL, Name, NULL, ColorFilePath, NULL, AlphaFilePath, BumpMapFlag, BumpMapNextPixelLength, FilePathDoubleCancel, ReverseFlag, Bmp32AllZeroAlphaToXRGB8Flag, OpenFailedReturn ) ;
}

// スキンウエイト情報の追加
extern MV1_SKIN_WEIGHT_R *MV1RAddSkinWeight( MV1_MODEL_R *ReadModel )
{
	MV1_SKIN_WEIGHT_R *SkinWeight ;

	// メモリの確保
	SkinWeight = ( MV1_SKIN_WEIGHT_R * )AddMemArea( sizeof( MV1_SKIN_WEIGHT_R ), &ReadModel->Mem ) ;
	if( SkinWeight == NULL )
		return NULL ;

	// スキンウエイト情報の数をインクリメント
	ReadModel->SkinWeightNum ++ ;

	// 終了
	return SkinWeight ;
}

// アニメーションセットの追加
static MV1_ANIMSET_R *MV1RAddAnimSetBase( MV1_MODEL_R *ReadModel, const char *NameA, const wchar_t *NameW )
{
	MV1_ANIMSET_R *AnimSet ;
	BYTE TempBuffer[ 512 ] ;

	BYTE NameATempBuffer[ 512 ] ;
	if( NameA == NULL )
	{
		ConvString( ( const char * )NameW, -1, WCHAR_T_CHARCODEFORMAT, ( char * )TempBuffer, sizeof( TempBuffer ), CHAR_CHARCODEFORMAT ) ;
		NameA = ( const char * )TempBuffer ;
	}
	else
	{
		if( CHAR_CHARCODEFORMAT != ReadModel->CharCodeFormat )
		{
			ConvString( ( const char * )NameA, -1, ReadModel->CharCodeFormat, ( char * )NameATempBuffer, sizeof( NameATempBuffer ), CHAR_CHARCODEFORMAT ) ;
			NameA = ( const char * )NameATempBuffer ;
		}

		if( NameW == NULL )
		{
			ConvString( ( const char * )NameA, -1, CHAR_CHARCODEFORMAT, ( char * )TempBuffer, sizeof( TempBuffer ), WCHAR_T_CHARCODEFORMAT ) ;
			NameW = ( const wchar_t * )TempBuffer ;
		}
	}

	// メモリの確保
	AnimSet = ( MV1_ANIMSET_R * )AddMemArea( sizeof( MV1_ANIMSET_R ), &ReadModel->Mem ) ;
	if( AnimSet == NULL )
		return NULL ;

	// リストに追加
	MODELLIST_ADD( ReadModel, AnimSet ) ;

	// 名前を保存
#ifndef UNICODE
	AnimSet->NameA = MV1RAddString( ReadModel, NameA ) ;
	if( AnimSet->NameA == NULL )
	{
		return NULL ;
	}
#endif

	AnimSet->NameW = MV1RAddStringW( ReadModel, NameW ) ;
	if( AnimSet->NameW == NULL )
	{
		return NULL ;
	}

	// 終了
	return AnimSet ;
}

// アニメーションセットの追加
extern MV1_ANIMSET_R *MV1RAddAnimSet( MV1_MODEL_R *ReadModel, const char *Name )
{
	return MV1RAddAnimSetBase( ReadModel, Name, NULL ) ;
}

// アニメーションセットの追加
extern MV1_ANIMSET_R *MV1RAddAnimSetW( MV1_MODEL_R *ReadModel, const wchar_t *Name )
{
	return MV1RAddAnimSetBase( ReadModel, NULL, Name ) ;
}

// アニメーションの追加
extern MV1_ANIM_R *MV1RAddAnim( MV1_MODEL_R *ReadModel, MV1_ANIMSET_R *AnimSet )
{
	MV1_ANIM_R *Anim ;

	// メモリの確保
	Anim = ( MV1_ANIM_R * )AddMemArea( sizeof( MV1_ANIM_R ), &ReadModel->Mem ) ;
	if( Anim == NULL )
		return NULL ;

	// リストに追加
	MODELLIST_ADD( ReadModel, Anim ) ;
	OBJLIST_ADD( AnimSet, Anim ) ;
	Anim->Container = AnimSet ;

	// 終了
	return Anim ;
}

// アニメーションキーセットの追加
extern MV1_ANIMKEYSET_R *MV1RAddAnimKeySet( MV1_MODEL_R *ReadModel, MV1_ANIM_R *Anim )
{
	MV1_ANIMKEYSET_R *AnimKeySet ;

	// メモリの確保
	AnimKeySet = ( MV1_ANIMKEYSET_R * )AddMemArea( sizeof( MV1_ANIMKEYSET_R ), &ReadModel->Mem ) ;
	if( AnimKeySet == NULL )
		return NULL ;

	// リストに追加
	MODELLIST_ADD( ReadModel, AnimKeySet ) ;
	OBJLIST_ADD( Anim, AnimKeySet ) ;

	// パラメータの初期化
	AnimKeySet->TimeType = MV1_ANIMKEY_TIME_TYPE_KEY ;

	// 終了
	return AnimKeySet ;
}

// ライトの追加
static MV1_LIGHT_R *MV1RAddLightBase( MV1_MODEL_R *ReadModel, const char *NameA, const wchar_t *NameW )
{
	MV1_LIGHT_R *Light ;
	BYTE TempBuffer[ 512 ] ;

	BYTE NameATempBuffer[ 512 ] ;
	if( NameA == NULL )
	{
		ConvString( ( const char * )NameW, -1, WCHAR_T_CHARCODEFORMAT, ( char * )TempBuffer, sizeof( TempBuffer ), CHAR_CHARCODEFORMAT ) ;
		NameA = ( const char * )TempBuffer ;
	}
	else
	{
		if( CHAR_CHARCODEFORMAT != ReadModel->CharCodeFormat )
		{
			ConvString( ( const char * )NameA, -1, ReadModel->CharCodeFormat, ( char * )NameATempBuffer, sizeof( NameATempBuffer ), CHAR_CHARCODEFORMAT ) ;
			NameA = ( const char * )NameATempBuffer ;
		}

		if( NameW == NULL )
		{
			ConvString( ( const char * )NameA, -1, CHAR_CHARCODEFORMAT, ( char * )TempBuffer, sizeof( TempBuffer ), WCHAR_T_CHARCODEFORMAT ) ;
			NameW = ( const wchar_t * )TempBuffer ;
		}
	}

	// メモリの確保
	Light = ( MV1_LIGHT_R * )AddMemArea( sizeof( MV1_LIGHT_R ), &ReadModel->Mem ) ;
	if( Light == NULL )
		return NULL ;

	// リストに追加
	MODELLIST_ADD( ReadModel, Light ) ;

	// 名前を保存
#ifndef UNICODE
	Light->NameA = MV1RAddString( ReadModel, NameA ) ;
	if( Light->NameA == NULL )
	{
		return NULL ;
	}
#endif

	Light->NameW = MV1RAddStringW( ReadModel, NameW ) ;
	if( Light->NameW == NULL )
	{
		return NULL ;
	}

	// 終了
	return Light ;
}

// ライトの追加
extern MV1_LIGHT_R *MV1RAddLight( MV1_MODEL_R *ReadModel, const char *Name )
{
	return MV1RAddLightBase( ReadModel, Name, NULL ) ;
}

// ライトの追加
extern MV1_LIGHT_R *MV1RAddLightW( MV1_MODEL_R *ReadModel, const wchar_t *Name )
{
	return MV1RAddLightBase( ReadModel, NULL, Name ) ;
}

// 読み込み処理用モデルから基本モデルデータを作成する
extern int MV1LoadModelToReadModel(
	const MV1LOADMODEL_GPARAM *	GParam,
	      MV1_MODEL_R *			ReadModel,
	const wchar_t *				CurrentDir,
	const MV1_FILE_READ_FUNC *	ReadFunc,
	      int					ASyncThread
)
{
	MV1_MESH_R					*Mesh ;
	MV1_SKIN_WEIGHT_R			*SkinWeight ;
	MV1_SKIN_WEIGHT_ONE_R		*SkinWeightOne ;
	MV1_MATERIAL_POLY_R			*MPoly ;
	MV1_MESHFACE_R				*MeshFace ;
	MV1_TRIANGLELIST_R			*TList ;
	MV1_MATERIAL_R				*Material ;
	MV1_TEXTURE_R				*Texture ;
	MV1_TEXTURE_R				**Textures = NULL ;
	MV1_ANIMSET_R				*AnimSet ;
	MV1_ANIM_R					*Anim ;
	MV1_ANIMKEYSET_R			*AnimKeySet ;
	MV1_FRAME_R					*Frame ;
	MV1_SHAPE_R					*Shape ;
	MV1_SHAPE_VERTEX_R			*ShapeVertex ;
	MV1_PHYSICS_RIGIDBODY_R		*PhysicsRigidBody ;
	MV1_PHYSICS_JOINT_R			*PhysicsJoint ;
	MV1_MAKEVERTINDEXINFO		**VertInfoTable, *VInfo ;
	MV1_MAKEVERTINDEXINFO		*VertInfoBuffer ;
	int							*VertValidBuffer ;
	MV1_SKIN_BONE_USE_FRAME		*MBSkinWF ;
	int							VertInfoNum ;
//	int							PosIndexList[ MV1_READ_MAX_FACEVERTEX_NUM ] ;
	DWORD						MaxPositionNum ;
	DWORD						MaxTriangleNum ;
	DWORD						MeshStartNum, MeshNowNum ;
	DWORD						MeshPositionStartNum, MeshNormalStartNum, MeshBoneStartNum ;

	wchar_t						DirectoryPath[ FILEPATH_MAX + 2 ] ;
//	char						DirectoryPathA[ FILEPATH_MAX + 2 ] ;
//	wchar_t						DirectoryPathW[ FILEPATH_MAX + 2 ] ;

	MV1_FRAME_BASE				*MBFrame ;
	MV1_SKIN_BONE				*MBSkinW ;
	MV1_MESH_BASE				*MBMesh ;
	MV1_MESH_FACE				*MBFace ;
//	MV1_MESH_VERTEX				*MBMVert ;
//	MV1_MESH_POSITION			*MBMPos ;
//	MV1_MESH_NORMAL				*MBMNorm ;
//	MV1_TLIST_NORMAL_POS		/**NormTVP[  MV1_READ_MAX_FACEVERTEX_NUM ],*/ *NormV ;
//	MV1_TLIST_SKIN_POS_4B		/**Skin4TVP[ MV1_READ_MAX_FACEVERTEX_NUM ],*/ *Skin4BV ;
//	MV1_TLIST_SKIN_POS_8B		/**Skin8TVP[ MV1_READ_MAX_FACEVERTEX_NUM ],*/ *Skin8BV ;
//	MV1_TLIST_SKIN_POS_FREEB	/**SkinFVP[  MV1_READ_MAX_FACEVERTEX_NUM ],*/ *SkinFBV ;
	MV1_SKINBONE_BLEND			*VertB ;
	MV1_SHAPE_BASE				*MBShape ;
	MV1_SHAPE_MESH_BASE			*MBShapeMesh ;
	MV1_SHAPE_VERTEX_BASE		*MBShapeVertex ;
	MV1_PHYSICS_RIGIDBODY_BASE	*MBPhysicsRigidBody ;
	MV1_PHYSICS_JOINT_BASE		*MBPhysicsJoint ;
	MV1_TEXTURE_BASE			*MBTexture ;
	MV1_MATERIAL_BASE			*MBMaterial ;
	MV1_MATERIAL_LAYER			*MBMaterialLayer = NULL ;
	MV1_LIGHT					*MBLight ;
	MV1_TRIANGLE_LIST_BASE		*MBTList ;
	MV1_ANIMSET_BASE			*MBAnimSet ;
	MV1_ANIM_BASE				*MBAnim ;
	MV1_ANIM_KEYSET_BASE		*MBKeySet ;

	int						dirlen ;
	int						FrameMeshCounter ;
	WORD					BitBuf[ 8192 ] ;
//	BYTE					VertBuffer[ 2 ][ 32 * 1024 ] ;
//	COLOR_U8				OneColor ;
	COLOR_F					*ColorF ;
	DWORD					i = 0 ;
	int j, k, l, m, o, p, r, s, BoneNum, Size, Con, MaxCon, MaxFaceCon, NewHandle = -1, num = 0/*, StartMatrixIndex*/ ;
	MV1_MODEL_BASE MTBase, *MBase ;
	DWORD_PTR AllocSize ;

	// ディレクトリパスを取得
	dirlen = 0 ;
	if( CurrentDir != NULL )
	{
		_WCSCPY_S( DirectoryPath, sizeof( DirectoryPath ), CurrentDir ) ;
		dirlen = ( int )_WCSLEN( DirectoryPath ) ;
		if( DirectoryPath[ dirlen - 1 ] != L'\\' && DirectoryPath[ dirlen - 1 ] != L'/' )
		{
			_WCSCAT_S( DirectoryPath, sizeof( DirectoryPath ), L"/" ) ;
			dirlen = ( int )_WCSLEN( DirectoryPath ) ;
		}
	}

	// 頂点の一時バッファを初期化
//	_MEMSET( VertBuffer, 0, sizeof( VertBuffer ) ) ;

	// 同一頂点検出用情報のポインタを初期化
	VertInfoTable = NULL ;

	// アニメーションデータの最適化
	MV1OptimizeAnim( ReadModel ) ;

	// シェイプの数だけ繰り返しシェイプの対象になっているメッシュに印を付ける
	for( Shape = ReadModel->ShapeFirst ; Shape ; Shape = Shape->DataNext )
	{
		Shape->TargetMesh->IsShapeMesh = TRUE ;

		// ついでにテーブルも作る
		Shape->NextTable = ( DWORD * )ADDMEMAREA( sizeof( DWORD ) * Shape->TargetMesh->PositionNum, &ReadModel->Mem ) ;
		if( Shape->NextTable == NULL )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x52\x00\x65\x00\x61\x00\x64\x00\x20\x00\x4d\x00\x6f\x00\x64\x00\x65\x00\x6c\x00\x20\x00\x43\x00\x6f\x00\x6e\x00\x76\x00\x65\x00\x72\x00\x74\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xb7\x30\xa7\x30\xa4\x30\xd7\x30\xfe\x5b\x61\x8c\x02\x98\xb9\x70\xc6\x30\xfc\x30\xd6\x30\xeb\x30\x28\x75\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Read Model Convert Error : シェイプ対象頂点テーブル用メモリの確保に失敗しました\n" @*/ )) ;
			return -1 ;
		}
		_MEMSET( Shape->NextTable, 0xff, sizeof( DWORD ) * Shape->TargetMesh->PositionNum ) ;

		for( i = 0 ; i < ( DWORD )Shape->VertexNum ; i ++ )
		{
			Shape->NextTable[ Shape->Vertex[ i ].TargetPositionIndex ] = ( DWORD )i ;
		}
	}

	// マテリアルが割り当てられていないメッシュには仮マテリアルを追加する
	{
		for( Mesh = ReadModel->MeshFirst ; Mesh ; Mesh = Mesh->DataNext )
		{
			if( Mesh->MaterialNum == 0 )
			{
				break ;
			}
		}

		if( ReadModel->MaterialNum == 0 || Mesh != NULL )
		{
			// マテリアルが割り当てられていないメッシュ用のマテリアルを追加
			Material = MV1RAddMaterial( ReadModel, "NoMaterial" ) ;
			if( Material == NULL )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x52\x00\x65\x00\x61\x00\x64\x00\x20\x00\x4d\x00\x6f\x00\x64\x00\x65\x00\x6c\x00\x20\x00\x43\x00\x6f\x00\x6e\x00\x76\x00\x65\x00\x72\x00\x74\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x4d\x00\x61\x00\x74\x00\x65\x00\x72\x00\x69\x00\x61\x00\x6c\x00\x20\x00\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\xfd\x8f\xa0\x52\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Read Model Convert Error : Material オブジェクトの追加に失敗しました\n" @*/, i ) ) ;
				return -1 ;
			}

			// メッシュ座標データのサイズを加算
			for( Mesh = ReadModel->MeshFirst ; Mesh ; Mesh = Mesh->DataNext )
			{
				if( Mesh->MaterialNum == 0 )
				{
					Mesh->MaterialNum = 1 ;
					Mesh->Materials[ 0 ] = Material ;
				}
			}
		}
	}

	// メッシュの数だけ繰り返し
	ReadModel->TriangleNum = 0 ;
	MaxPositionNum = 0 ;
	MaxTriangleNum = 0 ;
	for( Mesh = ReadModel->MeshFirst ; Mesh ; Mesh = Mesh->DataNext )
	{
		// ポリゴンの三角形化
		if( MV1ConvertTrianglePolygon( ReadModel, Mesh ) == false )
			return -1 ;

		// 指定がある場合は座標の最適化
		if( GParam->LoadModelToPositionOptimize )
		{
			if( MV1OptimizePosition( ReadModel, Mesh ) == false )
				return -1 ;
		}

		// 法線が無いか指定がある場合は法線の再計算
		if( Mesh->Normals == NULL || ReadModel->AutoCreateNormal || GParam->LoadModelToReMakeNormal )
		{
			if( ReadModel->AutoCreateNormal == FALSE ) 
			{
				Mesh->Container->SmoothingAngle = GParam->LoadModelToReMakeNormalSmoothingAngle ;
			}

			if( MV1MakeMeshNormals( ReadModel, Mesh ) == false )
				return -1 ;

			// 従法線と接線を算出
			if( Mesh->UVNum[ 0 ] != 0 )
			{
				MV1MakeMeshBinormalsAndTangents( ReadModel, Mesh ) ;
			}
		}

		// 指定がある場合は、辺を共有していて、且つ法線の方向が異なる辺に面積０のポリゴンを追加する( ３角形ポリゴンの集合になっている必要あり )
		if( GParam->LoadModelToNotEqNormalSideAddZeroAreaPolygon )
		{
			if( MV1NotEqualNormalSideAddZeroAreaPolygon( ReadModel, Mesh ) == false )
				return -1 ;
		}

		// ポリゴンの数を算出
		Mesh->TriangleNum = 0 ;
		MeshFace = Mesh->Faces ;
		for( i = 0 ; i < Mesh->FaceNum ; i ++, MeshFace ++ )
		{
			MeshFace->PolygonNum = MeshFace->IndexNum - 2 ;
			Mesh->TriangleNum += MeshFace->PolygonNum ;
		}
		ReadModel->TriangleNum += Mesh->TriangleNum ;

		// ポジションの最大数の更新
		if( MaxPositionNum < Mesh->PositionNum )
			MaxPositionNum = Mesh->PositionNum ;

		// トライアングルの最大数の更新
		if( MaxTriangleNum < Mesh->TriangleNum )
			MaxTriangleNum = Mesh->TriangleNum ;

		// スキンメッシュの情報を取得
		if( Mesh->SkinWeightsNum )
		{
			// トライアングルタイプ配列格納用のメモリを確保
			Mesh->SkinFaceBoneNum = ( WORD * )ADDMEMAREA( ( sizeof( BYTE ) + sizeof( WORD ) ) * Mesh->FaceNum, &ReadModel->Mem ) ;
			Mesh->SkinFaceType = ( BYTE * )( Mesh->SkinFaceBoneNum + Mesh->FaceNum ) ;
			if( Mesh->SkinFaceBoneNum == NULL )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x52\x00\x65\x00\x61\x00\x64\x00\x20\x00\x4d\x00\x6f\x00\x64\x00\x65\x00\x6c\x00\x20\x00\x43\x00\x6f\x00\x6e\x00\x76\x00\x65\x00\x72\x00\x74\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xc8\x30\xe9\x30\xa4\x30\xa2\x30\xf3\x30\xb0\x30\xeb\x30\xbf\x30\xa4\x30\xd7\x30\x4d\x91\x17\x52\x3c\x68\x0d\x7d\x28\x75\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Read Model Convert Error : トライアングルタイプ配列格納用メモリの確保に失敗しました\n" @*/ )) ;
				return -1 ;
			}
			_MEMSET( Mesh->SkinFaceBoneNum, 0, ( sizeof( BYTE ) + sizeof( WORD ) ) * Mesh->FaceNum ) ;

			// 各頂点がどのボーンの影響を受けるかの情報の初期化
			if( InitBitList( &Mesh->SkinVerticesBlend, ( int )Mesh->SkinWeightsNum, ( int )Mesh->PositionNum, &ReadModel->Mem ) == -1 )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x52\x00\x65\x00\x61\x00\x64\x00\x20\x00\x4d\x00\x6f\x00\x64\x00\x65\x00\x6c\x00\x20\x00\x43\x00\x6f\x00\x6e\x00\x76\x00\x65\x00\x72\x00\x74\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x02\x98\xb9\x70\x78\x30\x6e\x30\xdc\x30\xfc\x30\xf3\x30\x6e\x30\x71\x5f\xff\x97\x6b\x30\xa2\x95\x59\x30\x8b\x30\xc7\x30\xfc\x30\xbf\x30\x6e\x30\x1d\x52\x1f\x67\x16\x53\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Read Model Convert Error : 頂点へのボーンの影響に関するデータの初期化に失敗しました\n" @*/ )) ;
				return -1 ;
			}

			// 各頂点のボーンの影響値を保存するメモリの確保
			Mesh->SkinVerticeWeightInfo = ( float * )ADDMEMAREA( sizeof( float ) * Mesh->SkinWeightsNum * Mesh->PositionNum, &ReadModel->Mem ) ;
			if( Mesh->SkinVerticeWeightInfo == NULL )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x52\x00\x65\x00\x61\x00\x64\x00\x20\x00\x4d\x00\x6f\x00\x64\x00\x65\x00\x6c\x00\x20\x00\x43\x00\x6f\x00\x6e\x00\x76\x00\x65\x00\x72\x00\x74\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x02\x98\xb9\x70\x78\x30\x6e\x30\xdc\x30\xfc\x30\xf3\x30\x6e\x30\x71\x5f\xff\x97\x24\x50\x6e\x30\xc5\x60\x31\x58\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Read Model Convert Error : 頂点へのボーンの影響値の情報を格納するメモリの確保に失敗しました\n" @*/ )) ;
				return -1 ;
			}

			// 各面がどのボーンの影響を受けるのかの情報の初期化
			if( InitBitList( &Mesh->SkinFacesBlend, ( int )Mesh->SkinWeightsNum, ( int )Mesh->FaceNum, &ReadModel->Mem ) == -1 )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x52\x00\x65\x00\x61\x00\x64\x00\x20\x00\x4d\x00\x6f\x00\x64\x00\x65\x00\x6c\x00\x20\x00\x43\x00\x6f\x00\x6e\x00\x76\x00\x65\x00\x72\x00\x74\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x62\x97\x78\x30\x6e\x30\xdc\x30\xfc\x30\xf3\x30\x6e\x30\x71\x5f\xff\x97\x6b\x30\xa2\x95\x59\x30\x8b\x30\xc7\x30\xfc\x30\xbf\x30\x6e\x30\x1d\x52\x1f\x67\x16\x53\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Read Model Convert Error : 面へのボーンの影響に関するデータの初期化に失敗しました\n" @*/ )) ;
				return -1 ;
			}

			// スキンウエイトの各頂点への反映リストを作成
			for( i = 0 ; i < Mesh->SkinWeightsNum ; i ++ )
			{
				SkinWeight = Mesh->SkinWeights[ i ] ;

				// 対象頂点数の数だけ繰り返し
				SkinWeightOne = SkinWeight->Data ;
				for( j = 0 ; j < ( int )SkinWeight->DataNum ; j ++, SkinWeightOne ++ )
				{
					// ウエイト値が０の場合は何もしない
					if( SkinWeightOne->Weight <= 0.0f || ( *( ( DWORD * )&SkinWeightOne->Weight ) & 0x7fffffff ) == 0 ) continue ;
					
					// 影響している頂点に情報を埋め込む
					SetBitList( &Mesh->SkinVerticesBlend, ( int )SkinWeightOne->TargetVertex, ( int )i ) ;
					Mesh->SkinVerticeWeightInfo[ SkinWeightOne->TargetVertex * Mesh->SkinWeightsNum + i ] = SkinWeightOne->Weight ;
				}
			}

			// フェイスのタイプをセット
			MeshFace = Mesh->Faces ;
			for( i = 0 ; ( DWORD )i < Mesh->FaceNum ; i ++, MeshFace ++ )
			{
				// ポリゴンが使用しているボーンの情報を構築
				GetBitList( &Mesh->SkinVerticesBlend, ( int )MeshFace->VertexIndex[ 0 ], BitBuf ) ; 
				for( j = 1 ; ( DWORD )j < MeshFace->IndexNum ; j ++ )
					OrBitList( &Mesh->SkinVerticesBlend, ( int )MeshFace->VertexIndex[ j ], BitBuf ) ;

				// ポリゴンが使用しているボーンの組み合わせ情報を保存
				CopyBitList( &Mesh->SkinFacesBlend, ( int )i, BitBuf ) ;

				// ポリゴンが使用しているボーンの数を取得
				MaxFaceCon = GetBitCount( BitBuf, Mesh->SkinVerticesBlend.UnitSize ) ;

				// １頂点が使用している最大ボーン数を取得
				MaxCon = 0 ;
				for( j = 0 ; j < ( int )MeshFace->IndexNum ; j ++ )
				{
					GetBitList( &Mesh->SkinVerticesBlend, ( int )MeshFace->VertexIndex[ j ], BitBuf ) ; 
					Con = GetBitCount( BitBuf, Mesh->SkinVerticesBlend.UnitSize ) ;
					if( MaxCon < Con ) MaxCon = Con ;
				}

				// ボーンが９つ以上使用されているかで処理を分岐
				if( MaxCon >= 9 )
				{
					Mesh->SkinFaceType[ i ] = MV1_VERTEX_TYPE_SKIN_FREEBONE ;
				}
				else
				// ボーンが５つ以上使用されているかで処理を分岐
				if( MaxCon >= 5 )
				{
					Mesh->SkinFaceType[ i ] = MV1_VERTEX_TYPE_SKIN_8BONE ;
				}
				else
				// ボーンが一つでも使用されているかどうかで処理を分岐
				if( MaxCon >= 1 )
				{
					Mesh->SkinFaceType[ i ] = MV1_VERTEX_TYPE_SKIN_4BONE ;
				}
				else
				// ここに来た場合は使用されているボーンは一つもないということ
				{
					Mesh->SkinFaceType[ i ] = MV1_VERTEX_TYPE_NORMAL ;
					MaxCon = 1 ;
//					return DXST_LOGFILE_ADDUTF16LE( L"Error ボーンの指定が一つもないポリゴンがあります by Fbx\n" ) ;
				}

				// 関わっているボーンの数を保存
				Mesh->SkinFaceBoneNum[ i ] = ( WORD )MaxFaceCon ;
			}
		}

		// マテリアル毎のポリゴンの情報を構築する
		{
			DWORD PolyNum, Ind ;

			// マテリアルの種類の数だけ確保
			Mesh->MaterialPolyList = ( MV1_MATERIAL_POLY_R * )ADDMEMAREA( sizeof( MV1_MATERIAL_POLY_R ) * ( Mesh->MaterialNum == 0 ? 1 : Mesh->MaterialNum ), &ReadModel->Mem ) ;
			if( Mesh->MaterialPolyList == NULL )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x52\x00\x65\x00\x61\x00\x64\x00\x20\x00\x4d\x00\x6f\x00\x64\x00\x65\x00\x6c\x00\x20\x00\x43\x00\x6f\x00\x6e\x00\x76\x00\x65\x00\x72\x00\x74\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xde\x30\xc6\x30\xea\x30\xa2\x30\xeb\x30\xce\x6b\x6e\x30\xdd\x30\xea\x30\xb4\x30\xf3\x30\xc7\x30\xfc\x30\xbf\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Read Model Convert Error : マテリアル毎のポリゴンデータを格納するメモリの確保に失敗しました\n" @*/ )) ;
				return -1 ;
			}

			// 実データでのマテリアルの数を加算
			ReadModel->MeshMaterialNum += Mesh->MaterialNum == 0 ? 1 : Mesh->MaterialNum ;

			// シェイプメッシュの場合はシェイプメッシュの数を加算する
			if( Mesh->IsShapeMesh )
			{
				ReadModel->ShapeMeshNum += ( Mesh->MaterialNum == 0 ? 1 : Mesh->MaterialNum ) * Mesh->Container->ShapeNum ;
			}

			// スキンメッシュかどうかで処理を分岐
			if( Mesh->SkinWeightsNum )
			{
				// 各フェイスが使用するトライアングルリスト番号を保存するメモリ領域の確保
				Mesh->FaceUseTriangleList = ( WORD * )ADDMEMAREA( sizeof( WORD ) * Mesh->FaceNum, &ReadModel->Mem ) ;
				if( Mesh->FaceUseTriangleList == NULL )
				{
					DXST_LOGFILEFMT_ADDUTF16LE(( "\x52\x00\x65\x00\x61\x00\x64\x00\x20\x00\x4d\x00\x6f\x00\x64\x00\x65\x00\x6c\x00\x20\x00\x43\x00\x6f\x00\x6e\x00\x76\x00\x65\x00\x72\x00\x74\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x04\x54\x62\x97\x68\x30\xc8\x30\xe9\x30\xa4\x30\xa2\x30\xf3\x30\xb0\x30\xeb\x30\xea\x30\xb9\x30\xc8\x30\x6e\x30\xa2\x95\xc2\x4f\x92\x30\xdd\x4f\x58\x5b\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Read Model Convert Error : 各面とトライアングルリストの関係を保存するメモリ領域の確保に失敗しました\n" @*/ )) ;
					return -1 ;
				}

				// スキンメッシュの場合
				for( i = 0 ; ( DWORD )i < Mesh->FaceNum ; i ++ )
				{
					WORD UseMatrix[ MV1_TRIANGLE_LIST_USE_BONE_MAX_NUM ] ;
					int UseBoneNum ;

					PolyNum = Mesh->Faces[ i ].PolygonNum ;
					Ind = Mesh->Faces[ i ].MaterialIndex ;
					switch( Mesh->SkinFaceType[ i ] )
					{
					case MV1_VERTEX_TYPE_SKIN_FREEBONE :
						// 関わるボーンが９個以上の場合

						// ポリゴンの数を加算
						Mesh->MaterialPolyList[ Ind ].TypeNum[ MV1_VERTEX_TYPE_SKIN_FREEBONE ] += PolyNum ;

						// 自分が使う行列のリストを取得する
						UseBoneNum = GetBitListNumber( &Mesh->SkinFacesBlend, ( int )i, UseMatrix ) ;

						// 関わるボーンの最大数を更新
						if( Mesh->MaterialPolyList[ Ind ].MaxBoneCount < Mesh->SkinFaceBoneNum[ i ] ) 
							Mesh->MaterialPolyList[ Ind ].MaxBoneCount = Mesh->SkinFaceBoneNum[ i ] ;
						break ;

					case MV1_VERTEX_TYPE_SKIN_4BONE :
						// 関わるボーンが４個以内の場合
						MPoly = &Mesh->MaterialPolyList[ Ind ] ;

						// ポリゴンの数を加算
						Mesh->MaterialPolyList[ Ind ].TypeNum[ Mesh->SkinFaceType[ i ] ] += PolyNum ;

						// 自分が使う行列のリストを取得する
						UseBoneNum = GetBitListNumber( &Mesh->SkinFacesBlend, ( int )i, UseMatrix ) ;

						// 自分が使う行列が既にあるトライアングルリスト内にあるか調べる
						TList = MPoly->SkinB4TriangleList ;
						for( j = 0 ; j < MPoly->SkinB4TriangleListNum ; j ++, TList ++ )
						{
							if( MV1_TRIANGLE_MAX_INDEX <= ( TList->PolyNum + PolyNum ) * 3 ) continue ;

							for( l = 0 ; l < UseBoneNum ; l ++ )
							{
								for( k = 0 ; k < TList->UseBoneNum && TList->UseBone[ k ] != UseMatrix[ l ] ; k ++ ){}
								if( k == TList->UseBoneNum ) break ;
							}

							// あった場合は情報を追加
							if( l == UseBoneNum )
							{
								TList->PolyNum += PolyNum ;
								Mesh->FaceUseTriangleList[ i ] = ( WORD )j ;
								break ;
							}
						}

						// 無かった場合は追加の処理
						if( j == MPoly->SkinB4TriangleListNum )
						{
							// トライアングルリストの数が０だったら１にする
							if( MPoly->SkinB4TriangleListNum == 0 )
							{
								MPoly->SkinB4TriangleListNum ++ ;
							}

							for(;;)
							{
								// 行列の数が MV1_TRIANGLE_LIST_USE_BONE_MAX_NUM 個に達していないトライアングルリストがある場合は
								// そのトライアングルリストで使用する行列の数を増やして格納する
								TList = MPoly->SkinB4TriangleList ;
								for( j = 0 ; j < MPoly->SkinB4TriangleListNum ; j ++, TList ++ )
								{
									if( MV1_TRIANGLE_MAX_INDEX <= ( TList->PolyNum + PolyNum ) * 3 ) continue ;

									for( l = 0 ; l < UseBoneNum ; l ++ )
									{
										for( k = 0 ; k < TList->UseBoneNum && TList->UseBone[ k ] != UseMatrix[ l ] ; k ++ ){}
										if( k == TList->UseBoneNum )
										{
											if( TList->UseBoneNum >= GParam->LoadModelToTriangleListUseMaxBoneNum ) break ;
											TList->UseBone[ TList->UseBoneNum ] = UseMatrix[ l ] ;
											TList->UseBoneNum ++ ;
										}
									}
									if( l == UseBoneNum )
									{
										TList->PolyNum += PolyNum ;
										Mesh->FaceUseTriangleList[ i ] = ( WORD )j ;
										break ;
									}
								}

								// 既存のトライアングルリストが一杯だったらあたらなトライアングルリストを作成する
								if( j == MPoly->SkinB4TriangleListNum )
								{
									MPoly->SkinB4TriangleListNum ++ ;
									continue ;
								}
								break ;
							}
						}

						// 関わるボーンの最大数を更新
						if( Mesh->MaterialPolyList[ Ind ].MaxBoneCount < 4 ) 
							Mesh->MaterialPolyList[ Ind ].MaxBoneCount = 4 ;
						break ;

					case MV1_VERTEX_TYPE_SKIN_8BONE :
						// 関わるボーンが８個以内の場合
						MPoly = &Mesh->MaterialPolyList[ Ind ] ;

						// ポリゴンの数を加算
						Mesh->MaterialPolyList[ Ind ].TypeNum[ Mesh->SkinFaceType[ i ] ] += PolyNum ;

						// 自分が使う行列のリストを取得する
						UseBoneNum = GetBitListNumber( &Mesh->SkinFacesBlend, ( int )i, UseMatrix ) ;

						// 自分が使う行列が既にあるトライアングルリスト内にあるか調べる
						TList = MPoly->SkinB8TriangleList ;
						for( j = 0 ; j < MPoly->SkinB8TriangleListNum ; j ++, TList ++ )
						{
							if( MV1_TRIANGLE_MAX_INDEX <= ( TList->PolyNum + PolyNum ) * 3 ) continue ;

							for( l = 0 ; l < UseBoneNum ; l ++ )
							{
								for( k = 0 ; k < TList->UseBoneNum && TList->UseBone[ k ] != UseMatrix[ l ] ; k ++ ){}
								if( k == TList->UseBoneNum ) break ;
							}

							// あった場合は情報を追加
							if( l == UseBoneNum )
							{
								TList->PolyNum += PolyNum ;
								Mesh->FaceUseTriangleList[ i ] = ( WORD )j ;
								break ;
							}
						}

						// 無かった場合は追加の処理
						if( j == MPoly->SkinB8TriangleListNum )
						{
							// トライアングルリストの数が０だったら１にする
							if( MPoly->SkinB8TriangleListNum == 0 )
							{
								MPoly->SkinB8TriangleListNum ++ ;
							}

							for(;;)
							{
								// 行列の数が MV1_TRIANGLE_LIST_USE_BONE_MAX_NUM 個に達していないトライアングルリストがある場合は
								// そのトライアングルリストで使用する行列の数を増やして格納する
								TList = MPoly->SkinB8TriangleList ;
								for( j = 0 ; j < MPoly->SkinB8TriangleListNum ; j ++, TList ++ )
								{
									if( MV1_TRIANGLE_MAX_INDEX <= ( TList->PolyNum + PolyNum ) * 3 ) continue ;

									for( l = 0 ; l < UseBoneNum ; l ++ )
									{
										for( k = 0 ; k < TList->UseBoneNum && TList->UseBone[ k ] != UseMatrix[ l ] ; k ++ ){}
										if( k == TList->UseBoneNum )
										{
											if( TList->UseBoneNum >= GParam->LoadModelToTriangleListUseMaxBoneNum ) break ;
											TList->UseBone[ TList->UseBoneNum ] = UseMatrix[ l ] ;
											TList->UseBoneNum ++ ;
										}
									}
									if( l == UseBoneNum )
									{
										TList->PolyNum += PolyNum ;
										Mesh->FaceUseTriangleList[ i ] = ( WORD )j ;
										break ;
									}
								}

								// 既存のトライアングルリストが一杯だったらあたらなトライアングルリストを作成する
								if( j == MPoly->SkinB8TriangleListNum )
								{
									MPoly->SkinB8TriangleListNum ++ ;
									continue ;
								}
								break ;
							}
						}

						// 関わるボーンの最大数を更新
						if( Mesh->MaterialPolyList[ Ind ].MaxBoneCount < 8 ) 
							Mesh->MaterialPolyList[ Ind ].MaxBoneCount = 8 ;
						break ;
					}
				}
			}
			else
			{
				// スキンメッシュではない場合
				for( i = 0 ; ( DWORD )i < Mesh->FaceNum ; i ++ )
				{
					PolyNum = Mesh->Faces[ i ].PolygonNum ;
					Ind = Mesh->Faces[ i ].MaterialIndex ;
					Mesh->MaterialPolyList[ Ind ].TypeNum[ MV1_VERTEX_TYPE_NORMAL ] += PolyNum ;
				}
			}
		}

		// 頂点データのサイズを算出
		{
			DWORD MatNum ;
			DWORD FaceNum ;
			DWORD Bytes ;

			// マテリアルの数をセット
			MatNum = Mesh->MaterialNum == 0 ? 1 : Mesh->MaterialNum ;

			// マテリアルの数だけ繰り返し
			Mesh->VertexDataTotalSize = 0 ;
			Mesh->MaxBoneUseNum = 0 ;
			MPoly = Mesh->MaterialPolyList ;
			for( i = 0 ; i < MatNum ; i ++, MPoly ++ )
			{
				// 面の数を初期化
				FaceNum = 0 ;

				// 法線の数を加算
				ReadModel->MeshNormalNum += Mesh->NormalNum ;

				// 最大同時使用ボーン数の更新
				if( ( DWORD )MPoly->MaxBoneCount > Mesh->MaxBoneUseNum )
				{
					Mesh->MaxBoneUseNum = ( DWORD )MPoly->MaxBoneCount ;
				}

				// １ボーンポリゴン
				if( MPoly->TypeNum[ MV1_VERTEX_TYPE_NORMAL ] )
				{
					FaceNum += MPoly->TypeNum[ MV1_VERTEX_TYPE_NORMAL ] ;
					ReadModel->MeshFaceNum += MPoly->TypeNum[ MV1_VERTEX_TYPE_NORMAL ] ;
					Bytes = sizeof( MV1_TLIST_NORMAL_POS ) * 3 * MPoly->TypeNum[ MV1_VERTEX_TYPE_NORMAL ] ;
					Mesh->VertexDataTotalSize                             += Bytes ;
					Mesh->TypeNumVertexDataSize[ MV1_VERTEX_TYPE_NORMAL ] += Bytes ;
					ReadModel->NormalPositionNum                          += 3 * MPoly->TypeNum[ MV1_VERTEX_TYPE_NORMAL ] ;
					ReadModel->MeshVertexIndexNum                         += 3 * MPoly->TypeNum[ MV1_VERTEX_TYPE_NORMAL ] ;
					if( Mesh->IsShapeMesh )
					{
						ReadModel->ShapeNormalPositionNum += 3 * MPoly->TypeNum[ MV1_VERTEX_TYPE_NORMAL ] ;
					}
				}

				// ５個未満ボーンポリゴン
				if( MPoly->TypeNum[ MV1_VERTEX_TYPE_SKIN_4BONE ] )
				{
					FaceNum += MPoly->TypeNum[ MV1_VERTEX_TYPE_SKIN_4BONE ] ;
					ReadModel->MeshFaceNum += MPoly->TypeNum[ MV1_VERTEX_TYPE_SKIN_4BONE ] ;
					Bytes = sizeof( MV1_TLIST_SKIN_POS_4B ) * 3 * MPoly->TypeNum[ MV1_VERTEX_TYPE_SKIN_4BONE ] ;
					Mesh->VertexDataTotalSize                                 += Bytes ;
					Mesh->TypeNumVertexDataSize[ MV1_VERTEX_TYPE_SKIN_4BONE ] += Bytes ;
					ReadModel->SkinPosition4BNum                              += 3 * MPoly->TypeNum[ MV1_VERTEX_TYPE_SKIN_4BONE ] ;
					ReadModel->MeshVertexIndexNum                             += 3 * MPoly->TypeNum[ MV1_VERTEX_TYPE_SKIN_4BONE ] ;
					if( Mesh->IsShapeMesh )
					{
						ReadModel->ShapeSkinPosition4BNum += 3 * MPoly->TypeNum[ MV1_VERTEX_TYPE_SKIN_4BONE ] ;
					}
				}

				// ９個未満ボーンポリゴン
				if( MPoly->TypeNum[ MV1_VERTEX_TYPE_SKIN_8BONE ] )
				{
					FaceNum += MPoly->TypeNum[ MV1_VERTEX_TYPE_SKIN_8BONE ] ;
					ReadModel->MeshFaceNum += MPoly->TypeNum[ MV1_VERTEX_TYPE_SKIN_8BONE ] ;
					Bytes = sizeof( MV1_TLIST_SKIN_POS_8B ) * 3 * MPoly->TypeNum[ MV1_VERTEX_TYPE_SKIN_8BONE ] ;
					Mesh->VertexDataTotalSize                                 += Bytes ;
					Mesh->TypeNumVertexDataSize[ MV1_VERTEX_TYPE_SKIN_8BONE ] += Bytes ;
					ReadModel->SkinPosition8BNum                              += 3 * MPoly->TypeNum[ MV1_VERTEX_TYPE_SKIN_8BONE ] ;
					ReadModel->MeshVertexIndexNum                             += 3 * MPoly->TypeNum[ MV1_VERTEX_TYPE_SKIN_8BONE ] ;
					if( Mesh->IsShapeMesh )
					{
						ReadModel->ShapeSkinPosition8BNum += 3 * MPoly->TypeNum[ MV1_VERTEX_TYPE_SKIN_8BONE ] ;
					}
				}

				// ボーン数無制限ポリゴン
				if( MPoly->TypeNum[ MV1_VERTEX_TYPE_SKIN_FREEBONE ] )
				{
					FaceNum += MPoly->TypeNum[ MV1_VERTEX_TYPE_SKIN_FREEBONE ] ;
					ReadModel->MeshFaceNum += MPoly->TypeNum[ MV1_VERTEX_TYPE_SKIN_FREEBONE ] ;
					Bytes = ( sizeof( MV1_TLIST_SKIN_POS_FREEB ) + ( MPoly->MaxBoneCount - 4 ) * sizeof( MV1_SKINBONE_BLEND ) ) * 3 * MPoly->TypeNum[ MV1_VERTEX_TYPE_SKIN_FREEBONE ] ;
					Bytes = ( Bytes + 15 ) / 16 * 16 ;
					Mesh->VertexDataTotalSize                                    += Bytes ;
					Mesh->TypeNumVertexDataSize[ MV1_VERTEX_TYPE_SKIN_FREEBONE ] += Bytes ;
					ReadModel->SkinPositionFREEBSize                             += Bytes ;
					ReadModel->MeshVertexIndexNum                                += 3 * MPoly->TypeNum[ MV1_VERTEX_TYPE_SKIN_FREEBONE ] ;
					if( Mesh->IsShapeMesh )
					{
						ReadModel->SkinPositionFREEBSize += Bytes ;
					}
				}

				// メッシュ頂点データのサイズを加算
				Bytes = ( sizeof( MV1_MESH_VERTEX ) + ( ( Mesh->MaterialNum == 0 || Mesh->Materials[ i ]->DiffuseTexNum == 0 ? 1 : Mesh->Materials[ i ]->DiffuseTexNum ) - 1 ) * sizeof( float ) * 2 ) * FaceNum * 3 ;
				Mesh->VertexDataTotalSize += Bytes ;
				ReadModel->MeshVertexSize += Bytes ;

				if( Mesh->IsShapeMesh )
				{
					ReadModel->ShapeVertexNum += FaceNum * 3 * Mesh->Container->ShapeNum ;
				}
			}

			// 最大同時使用ボーン数の更新
			if( Mesh->MaxBoneUseNum > Mesh->Container->MaxBoneUseNum )
				Mesh->Container->MaxBoneUseNum = Mesh->MaxBoneUseNum ;

			// メッシュ法線データのサイズを加算
			Bytes = sizeof( MV1_MESH_NORMAL ) * Mesh->NormalNum ;
			Mesh->VertexDataTotalSize += Bytes ;
			ReadModel->MeshNormalNum += Mesh->NormalNum ;

			// トライアングルリストとトライアングルインデックスの数を算出
			{
				Mesh->TriangleListNum = 0 ;
				Mesh->IndexDataTotalSize = 0 ;

				// スキンメッシュかどうかで処理を分岐
				if( Mesh->SkinWeightsNum )
				{
					// スキンメッシュの場合
					MPoly = Mesh->MaterialPolyList ;
					for( i = 0 ; i < MatNum ; i ++, MPoly ++ )
					{
						// ５ボーン未満のポリゴン
						if( MPoly->TypeNum[ MV1_VERTEX_TYPE_SKIN_4BONE ] )
						{
							Mesh->TriangleListNum += MPoly->SkinB4TriangleListNum ;
							Bytes = MPoly->TypeNum[ MV1_VERTEX_TYPE_SKIN_4BONE ] * 3 * sizeof( WORD ) ;
							Mesh->IndexDataTotalSize                                 += Bytes ;
							Mesh->TypeNumIndexDataSize[ MV1_VERTEX_TYPE_SKIN_4BONE ] += Bytes ;
						}

						// ９ボーン未満のポリゴン
						if( MPoly->TypeNum[ MV1_VERTEX_TYPE_SKIN_8BONE ] )
						{
							Mesh->TriangleListNum += MPoly->SkinB8TriangleListNum ;
							Bytes = MPoly->TypeNum[ MV1_VERTEX_TYPE_SKIN_8BONE ] * 3 * sizeof( WORD ) ;
							Mesh->IndexDataTotalSize                                 += Bytes ;
							Mesh->TypeNumIndexDataSize[ MV1_VERTEX_TYPE_SKIN_8BONE ] += Bytes ;
						}

						// ボーン数無制限のポリゴン
						if( MPoly->TypeNum[ MV1_VERTEX_TYPE_SKIN_FREEBONE ] )
						{
							Mesh->TriangleListNum += ( MPoly->TypeNum[ MV1_VERTEX_TYPE_SKIN_FREEBONE ] * 3 + MV1_TRIANGLE_MAX_INDEX - 1 ) / MV1_TRIANGLE_MAX_INDEX ;
							Bytes = MPoly->TypeNum[ MV1_VERTEX_TYPE_SKIN_FREEBONE ] * 3 * sizeof( WORD ) ;
							Mesh->IndexDataTotalSize                                    += Bytes ;
							Mesh->TypeNumIndexDataSize[ MV1_VERTEX_TYPE_SKIN_FREEBONE ] += Bytes ;
						}
					}
				}
				else
				{
					// スキンメッシュではない場合
					MPoly = Mesh->MaterialPolyList ;
					for( i = 0 ; i < MatNum ; i ++, MPoly ++ )
					{
						MPoly->SimpleTriangleListNum = ( int )( ( MPoly->TypeNum[ MV1_VERTEX_TYPE_NORMAL ] * 3 + MV1_TRIANGLE_MAX_INDEX - 1 ) / MV1_TRIANGLE_MAX_INDEX ) ;
						Mesh->TriangleListNum += MPoly->SimpleTriangleListNum ;
						Bytes = MPoly->TypeNum[ MV1_VERTEX_TYPE_NORMAL ] * 3 * sizeof( WORD ) ;
						Mesh->IndexDataTotalSize                             += Bytes ;
						Mesh->TypeNumIndexDataSize[ MV1_VERTEX_TYPE_NORMAL ] += Bytes ;
					}
				}

				// トライアングルリストの数を加算
				ReadModel->TriangleListNum += Mesh->TriangleListNum ;

				// 頂点インデックスの数を加算
				ReadModel->IndexNum += Mesh->IndexDataTotalSize / sizeof( WORD ) ;
			}
		}
	}

	// メッシュ座標データのサイズを加算
	for( Mesh = ReadModel->MeshFirst ; Mesh ; Mesh = Mesh->DataNext )
	{
		Size = ( int )( ( sizeof( MV1_MESH_POSITION ) + ( Mesh->Container->MaxBoneUseNum - 4 ) * sizeof( MV1_SKINBONE_BLEND ) ) * Mesh->PositionNum ) ;
		Mesh->VertexDataTotalSize   += Size ;
		ReadModel->MeshPositionSize += Size ;
	}

	// 状態変化管理テーブルに必要なメモリサイズを算出する
	{
		MV1_FRAME_R *TempFrame ;
		MV1_MESH_R *TempMesh ;
		int MeshNum ;

		// 各フレームが従える総フレーム数と総メッシュ数を算出する
		Frame = ReadModel->FrameFirst ;
		ReadModel->TotalMV1MeshNum = 0 ;
		for( i = 0 ; ( DWORD )i < ReadModel->FrameNum ; i ++, Frame = Frame->DataNext )
		{
			MeshNum = 0 ;
			for( TempMesh = Frame->MeshFirst ; TempMesh ; TempMesh = TempMesh->Next )
				MeshNum += TempMesh->MaterialNum ? TempMesh->MaterialNum : 1 ;
			ReadModel->TotalMV1MeshNum += MeshNum ;

			Frame->TotalChildNum = 0 ;
			Frame->TotalMeshNum = MeshNum ;

			for( TempFrame = Frame->Parent ; TempFrame ; TempFrame = TempFrame->Parent )
			{
				TempFrame->TotalChildNum ++ ;
				TempFrame->TotalMeshNum += MeshNum ;
			}
		}

		// 従える総フレーム数と総メッシュ数からテーブルに必要なメモリサイズを算出する
		ReadModel->ChangeDrawMaterialTableSize = 0 ;
		ReadModel->ChangeMatrixTableSize = 0 ;
		Frame = ReadModel->FrameFirst ;
		for( i = 0 ; ( DWORD )i < ReadModel->FrameNum ; i ++, Frame = Frame->DataNext )
		{
			ReadModel->ChangeMatrixTableSize       += ( 1 + Frame->TotalChildNum + ReadModel->FrameNum                       + 31 ) / 32 * 4 + 4 ;
			ReadModel->ChangeDrawMaterialTableSize += ( 1 + Frame->TotalChildNum + ReadModel->FrameNum + Frame->TotalMeshNum + 31 ) / 32 * 4 + 4 ;
		}
		ReadModel->ChangeDrawMaterialTableSize = ( ReadModel->ChangeDrawMaterialTableSize + 15 ) / 16 * 16 ;
		ReadModel->ChangeMatrixTableSize       = ( ReadModel->ChangeMatrixTableSize       + 15 ) / 16 * 16 ;
	}
	ReadModel->MeshVertexSize   = ( ReadModel->MeshVertexSize   + 15 ) / 16 * 16 ; 
	ReadModel->MeshPositionSize = ( ReadModel->MeshPositionSize + 15 ) / 16 * 16 ; 

	// メモリの確保サイズを算出
	{
		_MEMSET( &MTBase, 0, sizeof( MTBase ) ) ;
		AllocSize = 0 ;

		// ファイルパスを保存する相対アドレスをセット
		MTBase.FilePath = ( wchar_t * )AllocSize ;
		AllocSize += ( ( _WCSLEN( ReadModel->FilePath ) + 1 ) * sizeof( wchar_t ) + 3 ) / 4 * 4 ;

		// モデル名を保存する相対アドレスをセット
		MTBase.Name = ( wchar_t * )AllocSize ;
		AllocSize += ( ( _WCSLEN( ReadModel->Name ) + 1 ) * sizeof( wchar_t ) + 3 ) / 4 * 4 ;

		// ディレクトリパスを保存する相対アドレスをセット
		MTBase.DirectoryPath = ( wchar_t * )AllocSize ;
		AllocSize += ( ( dirlen + 1 ) * sizeof( wchar_t ) + 3 ) / 4 * 4 ;

		// 変更テーブル情報を保存する相対アドレスをセット
		MTBase.ChangeDrawMaterialTable = ( DWORD * )AllocSize ;
		AllocSize += ReadModel->ChangeDrawMaterialTableSize ;

		MTBase.ChangeMatrixTable = ( DWORD * )AllocSize ;
		AllocSize += ReadModel->ChangeMatrixTableSize ;

		// フレーム配列の確保メモリ先頭からの相対アドレスをセット
		MTBase.FrameNum = ( int )ReadModel->FrameNum ;
		MTBase.Frame = ( MV1_FRAME_BASE * )AllocSize ;
		AllocSize += MTBase.FrameNum * sizeof( MV1_FRAME_BASE ) ;
		MTBase.FrameUseSkinBoneNum = ( int )ReadModel->SkinWeightNum ;
		MTBase.FrameUseSkinBone = ( MV1_SKIN_BONE ** )AllocSize ;
		AllocSize += MTBase.FrameUseSkinBoneNum * sizeof( MV1_SKIN_BONE * ) ;

		// マテリアル配列の確保メモリ先頭からの相対アドレスをセット
		MTBase.MaterialNum = ( int )ReadModel->MaterialNum ;
		MTBase.Material = ( MV1_MATERIAL_BASE * )AllocSize ;
		AllocSize += MTBase.MaterialNum * sizeof( MV1_MATERIAL_BASE ) ;

		// テクスチャ配列の確保メモリ先頭からの相対アドレスをセット
		MTBase.TextureNum = ( int )ReadModel->TextureNum ;
		MTBase.Texture = ( MV1_TEXTURE_BASE * )AllocSize ;
		AllocSize += MTBase.TextureNum * sizeof( MV1_TEXTURE_BASE ) ;

		// ライトの数をセット
		MTBase.LightNum = ( int )ReadModel->LightNum ;
		MTBase.Light = ( MV1_LIGHT * )AllocSize ;
		AllocSize += MTBase.LightNum * sizeof( MV1_LIGHT ) ;

		// スキニングメッシュ用のボーン情報の数をセット
		MTBase.SkinBoneNum = ( int )ReadModel->SkinWeightNum ;
		MTBase.SkinBone = ( MV1_SKIN_BONE * )AllocSize ;
		AllocSize += MTBase.SkinBoneNum * sizeof( MV1_SKIN_BONE ) ;
		MTBase.SkinBoneUseFrameNum = MTBase.SkinBoneNum ;
		MTBase.SkinBoneUseFrame = ( MV1_SKIN_BONE_USE_FRAME * )AllocSize ;
		AllocSize += MTBase.SkinBoneUseFrameNum * sizeof( MV1_SKIN_BONE_USE_FRAME ) ;

		// シェイプデータ配列の確保メモリ先頭からの相対アドレスをセット
		MTBase.ShapeNum = ( int )ReadModel->ShapeNum ;
		MTBase.Shape = ( MV1_SHAPE_BASE * )AllocSize ;
		AllocSize += MTBase.ShapeNum * sizeof( MV1_SHAPE_BASE ) ;

		// シェイプメッシュのメモリアドレスをセット
		MTBase.ShapeMeshNum = ( int )ReadModel->ShapeMeshNum ;
		MTBase.ShapeMesh = ( MV1_SHAPE_MESH_BASE * )AllocSize ;
		AllocSize += MTBase.ShapeMeshNum * sizeof( MV1_SHAPE_MESH_BASE ) ;

		// 物理演算で使用する剛体データのメモリアドレスをセット
		MTBase.PhysicsRigidBodyNum = ( int )ReadModel->PhysicsRigidBodyNum ;
		MTBase.PhysicsRigidBody = ( MV1_PHYSICS_RIGIDBODY_BASE * )AllocSize ;
		AllocSize += MTBase.PhysicsRigidBodyNum * sizeof( MV1_PHYSICS_RIGIDBODY_BASE ) ;

		// 物理演算で使用するジョイントデータのメモリアドレスをセット
		MTBase.PhysicsJointNum = ( int )ReadModel->PhysicsJointNum ;
		MTBase.PhysicsJoint = ( MV1_PHYSICS_JOINT_BASE * )AllocSize ;
		AllocSize += MTBase.PhysicsJointNum * sizeof( MV1_PHYSICS_JOINT_BASE ) ;

		// メッシュの数をセット
		MTBase.MeshNum = ( int )ReadModel->MeshMaterialNum ;
		MTBase.Mesh = ( MV1_MESH_BASE * )AllocSize ;
		AllocSize += MTBase.MeshNum * sizeof( MV1_MESH_BASE ) ;

		// トライアングルリストのメモリアドレスをセット
		MTBase.TriangleListNum = ( int )ReadModel->TriangleListNum ;
		MTBase.TriangleList = ( MV1_TRIANGLE_LIST_BASE * )AllocSize ;
		AllocSize += MTBase.TriangleListNum * sizeof( MV1_TRIANGLE_LIST_BASE ) ;

		// 頂点インデックスのメモリアドレスをセット
		MTBase.TriangleListIndexNum = ( int )ReadModel->IndexNum ;
		MTBase.TriangleListIndex = ( WORD * )AllocSize ;
		AllocSize += ( MTBase.TriangleListIndexNum * 2 ) * sizeof( WORD ) ;
		AllocSize = ( AllocSize + 15 ) / 16 * 16 ; 

		// 文字列を保存するメモリアドレスのセット
#ifndef UNICODE
		MTBase.StringSizeA = ( int )ReadModel->StringSizeA ;
		MTBase.StringBufferA = ( char * )AllocSize ;
		AllocSize += MTBase.StringSizeA ;
#endif

		MTBase.StringSizeW = ( int )ReadModel->StringSizeW ;
		MTBase.StringBufferW = ( wchar_t * )AllocSize ;
		AllocSize += MTBase.StringSizeW ;

		// アニメーションセットを保存するメモリアドレスのセット
		MTBase.AnimSetNum = ( int )ReadModel->AnimSetNum ;
		MTBase.AnimSet = ( MV1_ANIMSET_BASE * )AllocSize ;
		AllocSize += MTBase.AnimSetNum * sizeof( MV1_ANIMSET_BASE ) ;

		// アニメーションを保存するメモリアドレスのセット
		MTBase.AnimNum = ( int )ReadModel->AnimNum ;
		MTBase.Anim = ( MV1_ANIM_BASE * )AllocSize ;
		AllocSize += MTBase.AnimNum * sizeof( MV1_ANIM_BASE ) ;

		// アニメーションキーセットを保存するメモリアドレスのセット
		MTBase.AnimKeySetNum = ( int )ReadModel->AnimKeySetNum ;
		MTBase.AnimKeySet = ( MV1_ANIM_KEYSET_BASE * )AllocSize ;
		AllocSize += MTBase.AnimKeySetNum * sizeof( MV1_ANIM_KEYSET_BASE ) ;

		// 各アニメーションキーを保存するメモリアドレスのセット
		MTBase.AnimKeyDataSize = ( int )ReadModel->AnimKeyDataSize ;
		MTBase.AnimKeyData = ( void * )AllocSize ;
		AllocSize += MTBase.AnimKeyDataSize ;

		// アニメーションとフレームの対応テーブルのメモリアドレスをセット
		MTBase.AnimTargetFrameTable = ( MV1_ANIM_BASE ** )AllocSize ;
		AllocSize += sizeof( MV1_ANIM_BASE * ) * MTBase.AnimSetNum * MTBase.FrameNum ;
	}

	// モデル基データハンドルの作成
	NewHandle = MV1AddModelBase( ASyncThread ) ;
	if( NewHandle < 0 )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x52\x00\x65\x00\x61\x00\x64\x00\x20\x00\x4d\x00\x6f\x00\x64\x00\x65\x00\x6c\x00\x20\x00\x43\x00\x6f\x00\x6e\x00\x76\x00\x65\x00\x72\x00\x74\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xe2\x30\xc7\x30\xeb\x30\xfa\x57\x2c\x67\xc7\x30\xfc\x30\xbf\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x6e\x30\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Read Model Convert Error : モデル基本データハンドルの取得に失敗しました\n" @*/ )) ;
		return -1 ;
	}
	MBase = ( MV1_MODEL_BASE * )GetHandleInfo( NewHandle ) ;

	// モデル基データの構築
	{
		// 情報を保存するメモリを一度に確保する
		MBase->DataBuffer = MDALLOCMEM( AllocSize ) ;
		if( MBase->DataBuffer == NULL )
		{
			DXST_LOGFILE_ADDUTF16LE( "\x52\x00\x65\x00\x61\x00\x64\x00\x20\x00\x4d\x00\x6f\x00\x64\x00\x65\x00\x6c\x00\x20\x00\x43\x00\x6f\x00\x6e\x00\x76\x00\x65\x00\x72\x00\x74\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xe2\x30\xc7\x30\xeb\x30\xfa\x57\x2c\x67\xc7\x30\xfc\x30\xbf\x30\xdd\x4f\x58\x5b\x28\x75\x6e\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Read Model Convert Error : モデル基本データ保存用のメモリの確保に失敗しました\n" @*/ ) ;
			goto ERRORLABEL ;
		}
		MBase->AllocMemorySize = AllocSize ;
		_MEMSET( MBase->DataBuffer, 0, AllocSize ) ;

		// トライアングルリスト構築用情報を格納するメモリの確保
		if( MaxPositionNum + MaxTriangleNum > 0 )
		{
			VertInfoTable = ( MV1_MAKEVERTINDEXINFO ** )DXALLOC( sizeof( MV1_MAKEVERTINDEXINFO * ) * MaxPositionNum + sizeof( MV1_MAKEVERTINDEXINFO ) * MaxTriangleNum * 3 ) ;
			if( VertInfoTable == NULL )
			{
				DXST_LOGFILE_ADDUTF16LE( "\x52\x00\x65\x00\x61\x00\x64\x00\x20\x00\x4d\x00\x6f\x00\x64\x00\x65\x00\x6c\x00\x20\x00\x43\x00\x6f\x00\x6e\x00\x76\x00\x65\x00\x72\x00\x74\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xe2\x30\xc7\x30\xeb\x30\xfa\x57\x2c\x67\xc7\x30\xfc\x30\xbf\x30\xcb\x69\xc9\x7b\x6b\x30\x7f\x4f\x28\x75\x59\x30\x8b\x30\xc6\x30\xf3\x30\xdd\x30\xe9\x30\xea\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Read Model Convert Error : モデル基本データ構築に使用するテンポラリバッファの確保に失敗しました\n" @*/ ) ;
				goto ERRORLABEL ;
			}
			VertInfoBuffer = ( MV1_MAKEVERTINDEXINFO * )( VertInfoTable + MaxPositionNum ) ;
			VertValidBuffer = ( int * )VertInfoTable ;
		}
		else
		{
			VertInfoBuffer  = NULL ;
			VertValidBuffer = NULL ;
		}

		// 各メモリアドレスの先頭をセット
		MBase->DirectoryPath		= ( wchar_t * )                   ( ( BYTE * )MBase->DataBuffer + ( DWORD_PTR )MTBase.DirectoryPath        ) ;
		MBase->FilePath				= ( wchar_t * )                   ( ( BYTE * )MBase->DataBuffer + ( DWORD_PTR )MTBase.FilePath             ) ;
		MBase->Name					= ( wchar_t * )                   ( ( BYTE * )MBase->DataBuffer + ( DWORD_PTR )MTBase.Name                 ) ;

		MBase->Frame                = ( MV1_FRAME_BASE * )            ( ( BYTE * )MBase->DataBuffer + ( DWORD_PTR )MTBase.Frame                ) ;
		MBase->FrameUseSkinBone     = ( MV1_SKIN_BONE ** )            ( ( BYTE * )MBase->DataBuffer + ( DWORD_PTR )MTBase.FrameUseSkinBone     ) ;
		MBase->ChangeDrawMaterialTable = ( DWORD * )                  ( ( BYTE * )MBase->DataBuffer + ( DWORD_PTR )MTBase.ChangeDrawMaterialTable ) ;
		MBase->ChangeMatrixTable    = ( DWORD * )                     ( ( BYTE * )MBase->DataBuffer + ( DWORD_PTR )MTBase.ChangeMatrixTable    ) ;
		MBase->Material             = ( MV1_MATERIAL_BASE * )         ( ( BYTE * )MBase->DataBuffer + ( DWORD_PTR )MTBase.Material             ) ;
		MBase->Texture              = ( MV1_TEXTURE_BASE * )	      ( ( BYTE * )MBase->DataBuffer + ( DWORD_PTR )MTBase.Texture              ) ;
		MBase->SkinBone             = ( MV1_SKIN_BONE * )             ( ( BYTE * )MBase->DataBuffer + ( DWORD_PTR )MTBase.SkinBone             ) ;
		MBase->SkinBoneUseFrame     = ( MV1_SKIN_BONE_USE_FRAME * )   ( ( BYTE * )MBase->DataBuffer + ( DWORD_PTR )MTBase.SkinBoneUseFrame     ) ;
		MBase->Mesh                 = ( MV1_MESH_BASE * )             ( ( BYTE * )MBase->DataBuffer + ( DWORD_PTR )MTBase.Mesh                 ) ;
		MBase->Light                = ( MV1_LIGHT * )                 ( ( BYTE * )MBase->DataBuffer + ( DWORD_PTR )MTBase.Light                ) ;
		MBase->TriangleList         = ( MV1_TRIANGLE_LIST_BASE * )    ( ( BYTE * )MBase->DataBuffer + ( DWORD_PTR )MTBase.TriangleList         ) ;
		MBase->TriangleListIndex    = ( WORD * )                      ( ( BYTE * )MBase->DataBuffer + ( DWORD_PTR )MTBase.TriangleListIndex    ) ;
#ifndef UNICODE
		MBase->StringBufferA        = ( char * )                      ( ( BYTE * )MBase->DataBuffer + ( DWORD_PTR )MTBase.StringBufferA        ) ;
#endif
		MBase->StringBufferW        = ( wchar_t * )                   ( ( BYTE * )MBase->DataBuffer + ( DWORD_PTR )MTBase.StringBufferW        ) ;
		MBase->AnimSet              = ( MV1_ANIMSET_BASE * )          ( ( BYTE * )MBase->DataBuffer + ( DWORD_PTR )MTBase.AnimSet              ) ;
		MBase->Anim                 = ( MV1_ANIM_BASE * )             ( ( BYTE * )MBase->DataBuffer + ( DWORD_PTR )MTBase.Anim                 ) ;
		MBase->AnimKeySet           = ( MV1_ANIM_KEYSET_BASE * )      ( ( BYTE * )MBase->DataBuffer + ( DWORD_PTR )MTBase.AnimKeySet           ) ;
		MBase->AnimKeyData          = ( void * )                      ( ( BYTE * )MBase->DataBuffer + ( DWORD_PTR )MTBase.AnimKeyData          ) ;
		MBase->AnimTargetFrameTable = ( MV1_ANIM_BASE ** )            ( ( BYTE * )MBase->DataBuffer + ( DWORD_PTR )MTBase.AnimTargetFrameTable ) ;
		MBase->Shape                = ( MV1_SHAPE_BASE * )            ( ( BYTE * )MBase->DataBuffer + ( DWORD_PTR )MTBase.Shape                ) ;
		MBase->ShapeMesh            = ( MV1_SHAPE_MESH_BASE * )       ( ( BYTE * )MBase->DataBuffer + ( DWORD_PTR )MTBase.ShapeMesh            ) ;
		MBase->PhysicsRigidBody		= ( MV1_PHYSICS_RIGIDBODY_BASE * )( ( BYTE * )MBase->DataBuffer + ( DWORD_PTR )MTBase.PhysicsRigidBody     ) ;
		MBase->PhysicsJoint		    = ( MV1_PHYSICS_JOINT_BASE * )    ( ( BYTE * )MBase->DataBuffer + ( DWORD_PTR )MTBase.PhysicsJoint         ) ;

		// 頂点データ格納用のメモリ領域の確保
		MBase->ShapeNormalPositionNum = 0 ;
		MBase->ShapeSkinPosition4BNum = 0 ;
		MBase->ShapeSkinPosition8BNum = 0 ;
		MBase->ShapeSkinPositionFREEBSize = 0 ;
		MBase->TriangleListNormalPositionNum = 0 ;
		MBase->TriangleListSkinPosition4BNum = 0 ;
		MBase->TriangleListSkinPosition8BNum = 0 ;
		MBase->TriangleListSkinPositionFREEB = 0 ;
		MBase->MeshPositionSize = 0 ;
		MBase->MeshNormalNum = 0 ;
		MBase->MeshVertexSize = 0 ;
		MBase->MeshFaceNum = 0 ;
		if( ReadModel->NormalPositionNum     ||
			ReadModel->SkinPosition4BNum     ||
			ReadModel->SkinPosition8BNum     ||
			ReadModel->SkinPositionFREEBSize ||
			ReadModel->MeshFaceNum           ||
			ReadModel->MeshPositionSize      ||
			ReadModel->MeshNormalNum         ||
			ReadModel->MeshVertexIndexNum    ||
			ReadModel->MeshVertexSize        ||
			ReadModel->ShapeVertexNum        )
		{
			MBase->VertexData = DXALLOC(
				sizeof( DWORD )                 * ReadModel->MeshVertexIndexNum      +
				sizeof( MV1_MESH_FACE )         * ReadModel->MeshFaceNum             +
				sizeof( MV1_MESH_NORMAL )       * ReadModel->MeshNormalNum           +
				sizeof( MV1_TLIST_NORMAL_POS )  * ReadModel->NormalPositionNum       +
				sizeof( MV1_TLIST_SKIN_POS_4B ) * ReadModel->SkinPosition4BNum       +
				sizeof( MV1_TLIST_SKIN_POS_8B ) * ReadModel->SkinPosition8BNum       +
				ReadModel->SkinPositionFREEBSize                                     + 
				ReadModel->MeshPositionSize                                          +
				ReadModel->MeshVertexSize                                            +
				sizeof( MV1_SHAPE_VERTEX_BASE ) * ReadModel->ShapeVertexNum          + 16 ) ;
			if( MBase->VertexData == NULL )
			{
				DXST_LOGFILE_ADDUTF16LE( "\x52\x00\x65\x00\x61\x00\x64\x00\x20\x00\x4d\x00\x6f\x00\x64\x00\x65\x00\x6c\x00\x20\x00\x43\x00\x6f\x00\x6e\x00\x76\x00\x65\x00\x72\x00\x74\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x02\x98\xb9\x70\xa7\x5e\x19\x6a\x68\x30\x02\x98\xb9\x70\xd5\x6c\xda\x7d\x92\x30\x00\x4e\x42\x66\x84\x76\x6b\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Read Model Convert Error : 頂点座標と頂点法線を一時的に格納するメモリ領域の確保に失敗しました\n" @*/ ) ;
				goto ERRORLABEL ;
			}
			MBase->TriangleListNormalPosition    = ( MV1_TLIST_NORMAL_POS     * )( ( ( DWORD_PTR )MBase->VertexData + 15 ) / 16 * 16 ) ;
			MBase->TriangleListSkinPosition4B    = ( MV1_TLIST_SKIN_POS_4B    * )( MBase->TriangleListNormalPosition              + ReadModel->NormalPositionNum          ) ;
			MBase->TriangleListSkinPosition8B    = ( MV1_TLIST_SKIN_POS_8B    * )( MBase->TriangleListSkinPosition4B              + ReadModel->SkinPosition4BNum          ) ;
			MBase->TriangleListSkinPositionFREEB = ( MV1_TLIST_SKIN_POS_FREEB * )( MBase->TriangleListSkinPosition8B              + ReadModel->SkinPosition8BNum          ) ;
			MBase->MeshVertexIndex               = ( DWORD                    * )( ( BYTE * )MBase->TriangleListSkinPositionFREEB + ReadModel->SkinPositionFREEBSize      ) ;
			MBase->MeshFace                      = ( MV1_MESH_FACE            * )( MBase->MeshVertexIndex                         + ReadModel->MeshVertexIndexNum         ) ;
			MBase->MeshNormal                    = ( MV1_MESH_NORMAL          * )( MBase->MeshFace                                + ReadModel->MeshFaceNum                ) ;
			MBase->MeshPosition                  = ( MV1_MESH_POSITION        * )( MBase->MeshNormal                              + ReadModel->MeshNormalNum              ) ;
			MBase->MeshVertex                    = ( MV1_MESH_VERTEX          * )( ( BYTE * )MBase->MeshPosition                  + ReadModel->MeshPositionSize           ) ;
			MBase->ShapeVertex                   = ( MV1_SHAPE_VERTEX_BASE    * )( ( BYTE * )MBase->MeshVertex                    + ReadModel->MeshVertexSize             ) ;
		}

		// モデル名とファイルパスとディレクトリパスを保存
		_WCSCPY( MBase->Name,          ReadModel->Name ) ;
		_WCSCPY( MBase->FilePath,      ReadModel->FilePath ) ;
		_WCSCPY( MBase->DirectoryPath, CurrentDir == NULL ? L"" : DirectoryPath ) ;

		// 同時複数描画に対応するかどうかを保存
		MBase->UsePackDraw = GParam->LoadModelToUsePackDraw ;

		// 右手座標系かどうかを保存
		MBase->RightHandType = FALSE ;

		// 割り当てられているマテリアルの番号が低いメッシュから描画するかどうかを保存
		MBase->MaterialNumberOrderDraw = ReadModel->MaterialNumberOrderDraw ;

		// 重力パラメータを保存
		if( GParam->LoadModelToWorldGravityInitialize == FALSE )
		{
			MBase->PhysicsGravity = MV1_PHYSICS_DEFAULT_GRAVITY ;
		}
		else
		{
			MBase->PhysicsGravity = GParam->LoadModelToWorldGravity ;
		}

		// ポリゴンの数を保存
		MBase->TriangleNum = ( int )ReadModel->TriangleNum ;

		// データをセット
		{
			// 要素が全部１のカラーを作成する
//			OneColor.r = 255 ;
//			OneColor.g = 255 ;
//			OneColor.b = 255 ;
//			OneColor.a = 255 ;

			// アニメーションを処理
			AnimSet = ReadModel->AnimSetFirst ;
			MBAnimSet = MBase->AnimSet ;
			MBase->AnimSetNum = ( int )ReadModel->AnimSetNum ;
			for( i = 0 ; i < ReadModel->AnimSetNum ; i ++, AnimSet = AnimSet->DataNext, MBAnimSet ++ )
			{
				// 名前のコピー
				MBAnimSet->NameAllocMem = FALSE ;
#ifndef UNICODE
				MBAnimSet->NameA = MV1RGetStringSpace(  MBase, AnimSet->NameA ) ;
#endif
				MBAnimSet->NameW = MV1RGetStringSpaceW( MBase, AnimSet->NameW ) ;

				// ループアニメーションかどうかをセット
				MBAnimSet->IsLoopAnim = AnimSet->IsLoopAnim ;

				// 加算アニメーションかどうかをセット
				MBAnimSet->IsAddAnim = AnimSet->IsAddAnim ;

				// キー同士の補間は行列の線形補間かどうかをセット
				MBAnimSet->IsMatrixLinearBlend = AnimSet->IsMatrixLinearBlend ;

				// インデックスのセット
				MBAnimSet->Index = AnimSet->Index ;

				// アニメーションの長さをセット
				MBAnimSet->MaxTime = AnimSet->EndTime ;

				// アニメーションの数をセット
				MBAnimSet->AnimNum = AnimSet->AnimNum ;

				// アニメーションの数だけ繰り返し
				MBAnimSet->Anim = MBase->Anim + MBase->AnimNum ;
				MBase->AnimNum += MBAnimSet->AnimNum ;
				MBAnim = MBAnimSet->Anim ;
				Anim = AnimSet->AnimFirst ;
				for( j = 0 ; j < AnimSet->AnimNum ; j ++, Anim = Anim->Next, MBAnim ++ )
				{
					// このアニメーションデータを所有するアニメーションセットデータへのポインタをセット
					MBAnim->Container = MBAnimSet ;

					// 対象となるフレームの名前をセット
					MBAnim->TargetFrame = MBase->Frame + Anim->TargetFrameIndex ;

					// 対象となるフレームのインデックスをセット
					MBAnim->TargetFrameIndex = Anim->TargetFrameIndex ;

					// アニメーションとフレームの対応テーブルにアドレスをセット
					MBase->AnimTargetFrameTable[ ReadModel->FrameNum * i + Anim->TargetFrameIndex ] = MBAnim ;

					// 各キーセットの中で一番長い時間値のセット
					MBAnim->MaxTime = Anim->MaxTime ;

					// 回転オーダーを保存
					MBAnim->RotateOrder = Anim->RotateOrder ;

					// キーセットへのポインタをセット
					MBAnim->KeySetNum = Anim->AnimKeySetNum ;
					MBAnim->KeySet = MBase->AnimKeySet + MBase->AnimKeySetNum ;
					MBase->AnimKeySetNum += Anim->AnimKeySetNum ;
					MBKeySet = MBAnim->KeySet ;

					// キーセットの数だけ繰り返し
					AnimKeySet = Anim->AnimKeySetFirst ;
					for( k = 0 ; k < Anim->AnimKeySetNum ; k ++, MBKeySet ++, AnimKeySet = AnimKeySet->Next )
					{
						// キータイプをセット
						MBKeySet->Type = AnimKeySet->Type ;
						MBKeySet->DataType = AnimKeySet->DataType ;

						// シェイプターゲットをセット
						MBKeySet->TargetShapeIndex = AnimKeySet->TargetShapeIndex ;

						// タイムタイプを保存
						MBKeySet->TimeType = AnimKeySet->TimeType ;

						// 単位時間と開始時間を保存
						if( MBKeySet->TimeType == MV1_ANIMKEY_TIME_TYPE_ONE )
						{
							MBKeySet->UnitTime = AnimKeySet->UnitTime ;
							MBKeySet->StartTime = AnimKeySet->StartTime ;
						}

						// 総時間をセット
//						MBKeySet->TotalTime = AnimKeySet->TotalTime ;

						// キーの数をセット
						MBKeySet->Num = AnimKeySet->Num ;

						// 要素情報のコピー
						MBKeySet->KeyFloat4 = ( FLOAT4 * )( ( BYTE * )MBase->AnimKeyData + MBase->AnimKeyDataSize ) ;

						// 要素タイプによってコピーサイズの変更
						Size = 0 ;
						switch( MBKeySet->Type )
						{
						case MV1_ANIMKEY_TYPE_QUATERNION_X :   Size = sizeof( FLOAT4 ) ;                  break ;
						case MV1_ANIMKEY_TYPE_QUATERNION_VMD : Size = sizeof( FLOAT4 ) ;                  break ;
						case MV1_ANIMKEY_TYPE_VECTOR :         Size = sizeof( VECTOR ) ;                  break ;
						case MV1_ANIMKEY_TYPE_MATRIX3X3 :      Size = sizeof( MV1_ANIM_KEY_MATRIX3X3 ) ;  break ;
						case MV1_ANIMKEY_TYPE_MATRIX4X4C :     Size = sizeof( MV1_ANIM_KEY_MATRIX4X4C ) ; break ;

						case MV1_ANIMKEY_TYPE_FLAT :
						case MV1_ANIMKEY_TYPE_LINEAR :
						case MV1_ANIMKEY_TYPE_BLEND :          Size = sizeof( float ) ;                   break ;
						}

						// キーサイズの保存
//						MBKeySet->UnitSize = Size ;

						// 転送
						if( GParam->LoadModelToIgnoreScaling &&
							( MBKeySet->DataType == MV1_ANIMKEY_DATATYPE_SCALE ||
							  MBKeySet->DataType == MV1_ANIMKEY_DATATYPE_SCALE_X ||
							  MBKeySet->DataType == MV1_ANIMKEY_DATATYPE_SCALE_Y ||
							  MBKeySet->DataType == MV1_ANIMKEY_DATATYPE_SCALE_Z ) )
						{
							switch( MBKeySet->Type )
							{
							case MV1_ANIMKEY_TYPE_VECTOR :
								for( l = 0; l < AnimKeySet->Num; l++ )
								{
									MBKeySet->KeyVector[ l ] = VGet( 1.0f, 1.0f, 1.0f ) ;
								}
								break ;

							case MV1_ANIMKEY_TYPE_FLAT :
							case MV1_ANIMKEY_TYPE_LINEAR :
							case MV1_ANIMKEY_TYPE_BLEND :
								for( l = 0; l < AnimKeySet->Num; l++ )
								{
									MBKeySet->KeyLinear[ l ] = 1.0f ;
								}
								break ;
							}
						}
						else
						{
							_MEMCPY( MBKeySet->KeyFloat4, AnimKeySet->KeyFloat4, ( size_t )( Size * MBKeySet->Num ) ) ;
						}
						MBase->AnimKeyDataSize += Size * MBKeySet->Num ;
						MBAnimSet->KeyDataSize += Size * MBKeySet->Num ;

						// キータイムデータの保存
						if( MBKeySet->TimeType == MV1_ANIMKEY_TIME_TYPE_KEY )
						{
							MBKeySet->KeyTime = ( float * )( ( BYTE * )MBase->AnimKeyData + MBase->AnimKeyDataSize ) ;
							_MEMCPY( MBKeySet->KeyTime, AnimKeySet->KeyTime, sizeof( float ) * MBKeySet->Num ) ;
							MBase->AnimKeyDataSize += sizeof( float ) * MBKeySet->Num ;
							MBAnimSet->KeyDataSize += sizeof( float ) * MBKeySet->Num ;
						}
					}
				}
			}

			// テクスチャデータを構築する
			MBase->TextureAllocMem = FALSE ;
			Texture = ReadModel->TextureFirst ;
			for( i = 0 ; i < ReadModel->TextureNum ; i ++, Texture = Texture->DataNext )
			{
				// アドレスをセット
				MBTexture = MBase->Texture + MBase->TextureNum ;
				MBase->TextureNum ++ ;

				// 反転フラグをセットする
				MBTexture->ReverseFlag = Texture->ReverseFlag ;

				// ３２ビットＢＭＰのアルファ値が全部０だったら XRGB8 として扱うかどうかのフラグをセットする
				MBTexture->Bmp32AllZeroAlphaToXRGB8Flag = Texture->Bmp32AllZeroAlphaToXRGB8Flag ;

				// ユーザーデータを初期化
				MBTexture->UserData[ 0 ] = 0 ;
				MBTexture->UserData[ 1 ] = 0 ;
				MBTexture->UseUserGraphHandle = 0 ;
				MBTexture->UserGraphHandle = 0 ;

				// 名前を保存
				MBTexture->NameAllocMem = FALSE ;
#ifndef UNICODE
				MBTexture->NameA = MV1RGetStringSpace(  MBase, Texture->NameA ) ;
#endif
				MBTexture->NameW = MV1RGetStringSpaceW( MBase, Texture->NameW ) ;

				// テクスチャの読み込み
				{
					wchar_t ColorPathW[ FILEPATH_MAX ] ;
					wchar_t AlphaPathW[ FILEPATH_MAX ] ;
					wchar_t *UseColorPath = NULL ;
					wchar_t *UseAlphaPath = NULL ;

					if( Texture->ColorFileNameW )
					{
						if( CurrentDir == NULL )
						{
							UseColorPath = Texture->ColorFileNameW ;
						}
						else
						{
							ConvertFullPathW_( Texture->ColorFileNameW, ColorPathW, sizeof( ColorPathW ), DirectoryPath ) ;
							UseColorPath = ColorPathW ;
						}
					}

					if( Texture->AlphaFileNameW )
					{
						if( CurrentDir == NULL )
						{
							UseAlphaPath = Texture->AlphaFileNameW ;
						}
						else
						{
							ConvertFullPathW_( Texture->AlphaFileNameW, AlphaPathW, sizeof( AlphaPathW ), DirectoryPath ) ;
							UseAlphaPath = AlphaPathW ;
						}
					}

					if( __MV1LoadTexture(
							&MBTexture->ColorImage, &MBTexture->ColorImageSize,
							&MBTexture->AlphaImage, &MBTexture->AlphaImageSize,
							&MBTexture->GraphHandle,
							&MBTexture->SemiTransFlag,
							&MBTexture->IsDefaultTexture,
#ifndef UNICODE
							NULL, NULL,
#endif
							NULL, NULL,
							Texture->ColorFileNameW ? UseColorPath : NULL,
							Texture->AlphaFileNameW ? UseAlphaPath : NULL,
							CurrentDir == NULL ? NULL : DirectoryPath,
							Texture->BumpMapFlag, Texture->BumpMapNextPixelLength,
							Texture->ReverseFlag,
							Texture->Bmp32AllZeroAlphaToXRGB8Flag,
							ReadFunc,
							false,
							TRUE,
							ASyncThread ) == -1 )
					{
						DXST_LOGFILEFMT_ADDW(( L"Read Model Convert Error : Texture Load Error : %s\n", Texture->NameW ) ) ;
						goto ERRORLABEL ;
					}
				}

				// ファイル名用にメモリを確保したフラグを倒す
				MBTexture->ColorImageFilePathAllocMem = FALSE ;
				MBTexture->AlphaImageFilePathAllocMem = FALSE ;

				// ファイルパスを保存
				if( MBTexture->ColorImage )
				{
#ifndef UNICODE
					MBTexture->ColorFilePathA = MV1RGetStringSpace(  MBase, Texture->ColorFileNameA ) ;
#endif
					MBTexture->ColorFilePathW = MV1RGetStringSpaceW( MBase, Texture->ColorFileNameW ) ;
				}
				if( MBTexture->AlphaImage && Texture->AlphaFileNameW )
				{
#ifndef UNICODE
					MBTexture->AlphaFilePathA = MV1RGetStringSpace(  MBase, Texture->AlphaFileNameA ) ;
#endif
					MBTexture->AlphaFilePathW = MV1RGetStringSpaceW( MBase, Texture->AlphaFileNameW ) ;
				}

				// アドレッシングモードのセット
				MBTexture->AddressModeU = Texture->AddressModeU ;
				MBTexture->AddressModeV = Texture->AddressModeV ;

				// テクスチャのスケール値をセット
				MBTexture->ScaleU = Texture->ScaleU ;
				MBTexture->ScaleV = Texture->ScaleV ;

				// フィルタリングモードのセット
				MBTexture->FilterMode = Texture->FilterMode ;

				// バンプマップ情報を保存
				MBTexture->BumpImageFlag = Texture->BumpMapFlag ;
				MBTexture->BumpImageNextPixelLength = Texture->BumpMapNextPixelLength ;

				// 画像のサイズを取得
				NS_GetGraphSize( MBTexture->GraphHandle, &MBTexture->Width, &MBTexture->Height ) ;
			}

			// マテリアルを処理
			Material = ReadModel->MaterialFirst ;
			for( i = 0 ; i < ReadModel->MaterialNum ; i ++, Material = Material->DataNext )
			{
				// アドレスをセット
				MBMaterial = MBase->Material + MBase->MaterialNum ;
				MBase->MaterialNum ++ ;

				// 名前を保存
#ifndef UNICODE
				MBMaterial->NameA = MV1RGetStringSpace(  MBase, Material->NameA ) ;
#endif
				MBMaterial->NameW = MV1RGetStringSpaceW( MBase, Material->NameW ) ;

				// パラメータのコピー
				MBMaterial->Type = Material->Type ;

				MBMaterial->Diffuse  = Material->Diffuse ;
				MBMaterial->Ambient  = Material->Ambient ;
				MBMaterial->Specular = Material->Specular ;
				MBMaterial->Emissive = Material->Emissive ;
				MBMaterial->Power    = Material->Power ;
//				if( ( MBMaterial->Specular.r >= 0.0000001f || 
//					  MBMaterial->Specular.g >= 0.0000001f ||
//					  MBMaterial->Specular.b >= 0.0000001f ||
//					  MBMaterial->Specular.a >= 0.0000001f ) && MBMaterial->Power <= 1.0f ) MBMaterial->Power = 1.0f ;
				MBMaterial->DrawBlendMode = Material->DrawBlendMode ;
				MBMaterial->DrawBlendParam = Material->DrawBlendParam ;

				// テクスチャがある場合はテクスチャを読み込む
				MBMaterial->DiffuseLayerNum  = Material->DiffuseTexNum ;
				MBMaterial->SpecularLayerNum = Material->SpecularTexNum ;
				MBMaterial->NormalLayerNum   = Material->NormalTexNum ;
				for( k = 0 ; k < 3 ; k ++ )
				{
					switch( k )
					{
					case 0 : num = MBMaterial->DiffuseLayerNum ;  Textures = Material->DiffuseTexs ;  MBMaterialLayer = MBMaterial->DiffuseLayer ;  break ;
					case 1 : num = MBMaterial->SpecularLayerNum ; Textures = Material->SpecularTexs ; MBMaterialLayer = MBMaterial->SpecularLayer ; break ;
					case 2 : num = MBMaterial->NormalLayerNum ;   Textures = Material->NormalTexs ;   MBMaterialLayer = MBMaterial->NormalLayer ;   break ;
					}

					for( j = 0 ; j < num ; j ++ )
					{
						if( Textures[ j ] == NULL ) continue ;
						Texture = Textures[ j ] ;

						// ブレンドタイプのセット
						MBMaterialLayer[ j ].BlendType = Texture->BlendType ;

						// テクスチャインデックスの保存
						MBMaterialLayer[ j ].Texture = Texture->Index ;
					}
				}

				MBMaterial->DiffuseGradTexture    = Material->DiffuseGradTexture  ? Material->DiffuseGradTexture->Index  : Material->DiffuseGradTextureDefault ;
				MBMaterial->SpecularGradTexture   = Material->SpecularGradTexture ? Material->SpecularGradTexture->Index : Material->SpecularGradTextureDefault ;
				MBMaterial->SphereMapTexture      = Material->SphereMapTexture    ? Material->SphereMapTexture->Index    : -1 ;
				MBMaterial->DiffuseGradBlendType  = Material->DiffuseGradBlendType ;
				MBMaterial->SpecularGradBlendType = Material->SpecularGradBlendType ;
				MBMaterial->SphereMapBlendType    = Material->SphereMapBlendType ;
				MBMaterial->OutLineWidth          = Material->OutLineWidth ;
				MBMaterial->OutLineDotWidth	      = Material->OutLineDotWidth ;
				MBMaterial->OutLineColor          = Material->OutLineColor ;
			}

			// フレームを処理
			Frame = ReadModel->FrameFirst ;
			MBase->ChangeDrawMaterialTableSize = 0 ;
			MBase->ChangeMatrixTableSize       = 0 ;
			FrameMeshCounter = 0 ;
			for( r = 0 ; r < ( int )ReadModel->FrameNum ; r ++, Frame = Frame->DataNext )
			{
				DWORD TypeNumVertexDataSize[ MV1_VERTEX_TYPE_NUM ] ;
//				DWORD TypeNumIndexDataSize ;
				MATRIX TempMatrix ;

				// 基本データにフレームを追加
				MBFrame = MBase->Frame + MBase->FrameNum ;

				// 自動法線計算用の角度閾値を保存
				MBFrame->SmoothingAngle = Frame->SmoothingAngle ;
				MBFrame->AutoCreateNormal = ReadModel->AutoCreateNormal ;

				// フレームの持ち主をセット
				MBFrame->Container = MBase ;

				// フレームのアドレスをセット
				Frame->MV1Frame = MBFrame ;

				// 名前をコピー
#ifndef UNICODE
				MBFrame->NameA = MV1RGetStringSpace(  MBase, Frame->NameA ) ;
#endif
				MBFrame->NameW = MV1RGetStringSpaceW( MBase, Frame->NameW ) ;

				// 親へのアドレスをセット
				if( Frame->Parent )
				{
					MBFrame->Parent = Frame->Parent->MV1Frame ;

					// 親からのリンクを追加
					if( MBFrame->Parent->FirstChild == NULL )
					{
						MBFrame->Parent->FirstChild = MBFrame ;
						MBFrame->Parent->LastChild  = MBFrame ;
					}
					else
					{
						MBFrame->Prev = MBFrame->Parent->LastChild->Next ;
						MBFrame->Parent->LastChild->Next = MBFrame ;
						MBFrame->Parent->LastChild = MBFrame ;
					}
				}
				else
				{
					if( MBase->FirstTopFrame == NULL )
					{
						MBase->FirstTopFrame = MBFrame ;
						MBase->LastTopFrame  = MBFrame ;
					}
					else
					{
						MBFrame->Prev = MBase->LastTopFrame->Next ;
						MBase->LastTopFrame->Next = MBFrame ;
						MBase->LastTopFrame = MBFrame ;
					}
					MBase->TopFrameNum ++ ;
				}

/*
				if( _STRCMP( MBFrame->Name, "Bone6_Parent_" ) == 0 )
				{
					MBFrame->Name[ 0 ] = MBFrame->Name[ 0 ] ;
				}
*/
				// 座標変換パラメータをセット
				{
					float One ;
					MATRIX ScaleMatrix, RotateMatrix ;
					VECTOR ScaleVector ;

					TempMatrix = Frame->Matrix ;
					One = 1.0f ;
					MBFrame->Translate   = Frame->Translate ;
					if( GParam->LoadModelToIgnoreScaling )
					{
						MBFrame->Scale   = VGet( 1.0f, 1.0f, 1.0f ) ;
					}
					else
					{
						MBFrame->Scale   = Frame->Scale ;
					}
					MBFrame->Rotate      = Frame->Rotate ;
					MBFrame->RotateOrder = Frame->RotateOrder ;
					MBFrame->Quaternion  = Frame->Quaternion ;

					MBFrame->PreRotate   = Frame->PreRotate ;
					MBFrame->PostRotate  = Frame->PostRotate ;

					// 平行移動要素が行列に仕込まれていたら行列から平行移動要素を取得する
					if( ( *( ( DWORD * )&TempMatrix.m[ 3 ][ 0 ] ) & 0x7fffffff ) != 0 ||
						( *( ( DWORD * )&TempMatrix.m[ 3 ][ 1 ] ) & 0x7fffffff ) != 0 ||
						( *( ( DWORD * )&TempMatrix.m[ 3 ][ 2 ] ) & 0x7fffffff ) != 0 )
					{
						MBFrame->Translate.x = TempMatrix.m[ 3 ][ 0 ] ;
						MBFrame->Translate.y = TempMatrix.m[ 3 ][ 1 ] ;
						MBFrame->Translate.z = TempMatrix.m[ 3 ][ 2 ] ;
						TempMatrix.m[ 3 ][ 0 ] = 0.0f ;
						TempMatrix.m[ 3 ][ 1 ] = 0.0f ;
						TempMatrix.m[ 3 ][ 2 ] = 0.0f ;
					}

					// 回転要素が行列に仕込まれていたら行列から回転要素を抽出する
					ScaleVector.x = _SQRT( TempMatrix.m[ 0 ][ 0 ] * TempMatrix.m[ 0 ][ 0 ] + 
									       TempMatrix.m[ 0 ][ 1 ] * TempMatrix.m[ 0 ][ 1 ] + 
									       TempMatrix.m[ 0 ][ 2 ] * TempMatrix.m[ 0 ][ 2 ] ) ;
					ScaleVector.y = _SQRT( TempMatrix.m[ 1 ][ 0 ] * TempMatrix.m[ 1 ][ 0 ] + 
									       TempMatrix.m[ 1 ][ 1 ] * TempMatrix.m[ 1 ][ 1 ] + 
									       TempMatrix.m[ 1 ][ 2 ] * TempMatrix.m[ 1 ][ 2 ] ) ;
					ScaleVector.z = _SQRT( TempMatrix.m[ 2 ][ 0 ] * TempMatrix.m[ 2 ][ 0 ] + 
									       TempMatrix.m[ 2 ][ 1 ] * TempMatrix.m[ 2 ][ 1 ] + 
									       TempMatrix.m[ 2 ][ 2 ] * TempMatrix.m[ 2 ][ 2 ] ) ;
					RotateMatrix.m[ 0 ][ 0 ] = TempMatrix.m[ 0 ][ 0 ] / ScaleVector.x ;
					RotateMatrix.m[ 0 ][ 1 ] = TempMatrix.m[ 0 ][ 1 ] / ScaleVector.x ;
					RotateMatrix.m[ 0 ][ 2 ] = TempMatrix.m[ 0 ][ 2 ] / ScaleVector.x ;
					RotateMatrix.m[ 1 ][ 0 ] = TempMatrix.m[ 1 ][ 0 ] / ScaleVector.y ;
					RotateMatrix.m[ 1 ][ 1 ] = TempMatrix.m[ 1 ][ 1 ] / ScaleVector.y ;
					RotateMatrix.m[ 1 ][ 2 ] = TempMatrix.m[ 1 ][ 2 ] / ScaleVector.y ;
					RotateMatrix.m[ 2 ][ 0 ] = TempMatrix.m[ 2 ][ 0 ] / ScaleVector.z ;
					RotateMatrix.m[ 2 ][ 1 ] = TempMatrix.m[ 2 ][ 1 ] / ScaleVector.z ;
					RotateMatrix.m[ 2 ][ 2 ] = TempMatrix.m[ 2 ][ 2 ] / ScaleVector.z ;
					if( *( ( DWORD * )&RotateMatrix.m[ 0 ][ 0 ] ) != *( ( DWORD * )&One ) ||
						*( ( DWORD * )&RotateMatrix.m[ 1 ][ 1 ] ) != *( ( DWORD * )&One ) ||
						*( ( DWORD * )&RotateMatrix.m[ 2 ][ 2 ] ) != *( ( DWORD * )&One ) ||
						( *( ( DWORD * )&RotateMatrix.m[ 0 ][ 1 ] ) & 0x7fffffff ) != 0 ||
						( *( ( DWORD * )&RotateMatrix.m[ 0 ][ 2 ] ) & 0x7fffffff ) != 0 ||
						( *( ( DWORD * )&RotateMatrix.m[ 1 ][ 0 ] ) & 0x7fffffff ) != 0 ||
						( *( ( DWORD * )&RotateMatrix.m[ 1 ][ 2 ] ) & 0x7fffffff ) != 0 ||
						( *( ( DWORD * )&RotateMatrix.m[ 2 ][ 0 ] ) & 0x7fffffff ) != 0 ||
						( *( ( DWORD * )&RotateMatrix.m[ 2 ][ 1 ] ) & 0x7fffffff ) != 0 )
					{
						MATRIX InvRotateMat ;
						bool MatchFlag ;

						MatchFlag = true ;
						do
						{
							GetMatrixXYZRotation( &RotateMatrix, &MBFrame->Rotate.x, &MBFrame->Rotate.y, &MBFrame->Rotate.z ) ;
							CreateRotationXYZMatrix( &InvRotateMat, MBFrame->Rotate.x, MBFrame->Rotate.y, MBFrame->Rotate.z ) ;
							if( MatrixRotateCmp( &InvRotateMat, &RotateMatrix, 0.00001f ) )
							{
								MBFrame->RotateOrder = MV1_ROTATE_ORDER_XYZ ;
								break ;
							}

							GetMatrixXZYRotation( &RotateMatrix, &MBFrame->Rotate.x, &MBFrame->Rotate.y, &MBFrame->Rotate.z ) ;
							CreateRotationXZYMatrix( &InvRotateMat, MBFrame->Rotate.x, MBFrame->Rotate.y, MBFrame->Rotate.z ) ;
							if( MatrixRotateCmp( &InvRotateMat, &RotateMatrix, 0.00001f ) )
							{
								MBFrame->RotateOrder = MV1_ROTATE_ORDER_XZY ;
								break ;
							}

							GetMatrixYXZRotation( &RotateMatrix, &MBFrame->Rotate.x, &MBFrame->Rotate.y, &MBFrame->Rotate.z ) ;
							CreateRotationYXZMatrix( &InvRotateMat, MBFrame->Rotate.x, MBFrame->Rotate.y, MBFrame->Rotate.z ) ;
							if( MatrixRotateCmp( &InvRotateMat, &RotateMatrix, 0.00001f ) )
							{
								MBFrame->RotateOrder = MV1_ROTATE_ORDER_YXZ ;
								break ;
							}

							GetMatrixYZXRotation( &RotateMatrix, &MBFrame->Rotate.x, &MBFrame->Rotate.y, &MBFrame->Rotate.z ) ;
							CreateRotationYZXMatrix( &InvRotateMat, MBFrame->Rotate.x, MBFrame->Rotate.y, MBFrame->Rotate.z ) ;
							if( MatrixRotateCmp( &InvRotateMat, &RotateMatrix, 0.00001f ) )
							{
								MBFrame->RotateOrder = MV1_ROTATE_ORDER_YZX ;
								break ;
							}

							GetMatrixZXYRotation( &RotateMatrix, &MBFrame->Rotate.x, &MBFrame->Rotate.y, &MBFrame->Rotate.z ) ;
							CreateRotationZXYMatrix( &InvRotateMat, MBFrame->Rotate.x, MBFrame->Rotate.y, MBFrame->Rotate.z ) ;
							if( MatrixRotateCmp( &InvRotateMat, &RotateMatrix, 0.00001f ) )
							{
								MBFrame->RotateOrder = MV1_ROTATE_ORDER_ZXY ;
								break ;
							}

							GetMatrixZYXRotation( &RotateMatrix, &MBFrame->Rotate.x, &MBFrame->Rotate.y, &MBFrame->Rotate.z ) ;
							CreateRotationZYXMatrix( &InvRotateMat, MBFrame->Rotate.x, MBFrame->Rotate.y, MBFrame->Rotate.z ) ;
							if( MatrixRotateCmp( &InvRotateMat, &RotateMatrix, 0.00001f ) )
							{
								MBFrame->RotateOrder = MV1_ROTATE_ORDER_ZYX ;
								break ;
							}
							GetMatrixXYZRotation( &RotateMatrix, &MBFrame->Rotate.x, &MBFrame->Rotate.y, &MBFrame->Rotate.z ) ;
							CreateRotationXYZMatrix( &InvRotateMat, MBFrame->Rotate.x, MBFrame->Rotate.y, MBFrame->Rotate.z ) ;
							MBFrame->RotateOrder = MV1_ROTATE_ORDER_XYZ ;
//							MBFrame->RotateOrder = 0 ;
//							ScaleMatrix = TempMatrix ;
//							MatchFlag = false ;
						}
						while( 0 ) ;

						if( MatchFlag )
						{
							CreateInverseMatrix( &InvRotateMat, &InvRotateMat ) ;
							CreateMultiplyMatrix( &ScaleMatrix, &TempMatrix, &InvRotateMat ) ;
						}
					}
					else
					{
						ScaleMatrix = TempMatrix ;
					}

					// 拡大要素が行列に仕込まれていたら行列から拡大要素を抽出する
					if( GParam->LoadModelToIgnoreScaling == FALSE )
					{
						ScaleVector.x = ScaleMatrix.m[ 0 ][ 0 ] ;
						ScaleVector.y = ScaleMatrix.m[ 1 ][ 1 ] ;
						ScaleVector.z = ScaleMatrix.m[ 2 ][ 2 ] ;
						if( ScaleVector.x < 0.99999f || ScaleVector.x > 1.00001f ||
							ScaleVector.y < 0.99999f || ScaleVector.y > 1.00001f ||
							ScaleVector.z < 0.99999f || ScaleVector.z > 1.00001f )
						{
							MBFrame->Scale.x = ScaleVector.x ;
							MBFrame->Scale.y = ScaleVector.y ;
							MBFrame->Scale.z = ScaleVector.z ;
						}
					}
				}

				// 子フレームの総数、メッシュの総数をセット
				MBFrame->TotalChildNum = Frame->TotalChildNum ;
				MBFrame->TotalMeshNum  = Frame->TotalMeshNum ;

				// 状態変化管理用基本情報のセット
				MV1ChangeInfoSetup( &MBFrame->ChangeDrawMaterialInfo, ( BYTE * )MBase->ChangeDrawMaterialTable + MBase->ChangeDrawMaterialTableSize, FrameMeshCounter + 1, MBFrame->TotalChildNum + MBFrame->TotalMeshNum + 1 ) ;
				MBase->ChangeDrawMaterialTableSize += MBFrame->ChangeDrawMaterialInfo.Size * 4 ;

				MV1ChangeInfoSetup( &MBFrame->ChangeMatrixInfo,       ( BYTE * )MBase->ChangeMatrixTable       + MBase->ChangeMatrixTableSize,       MBase->FrameNum + 1,  MBFrame->TotalChildNum + 1 ) ;
				MBase->ChangeMatrixTableSize += MBFrame->ChangeMatrixInfo.Size * 4 ;

				// フレームのインデックスをセット
				MBFrame->Index = MBase->FrameNum ;
				MBase->FrameNum ++ ;
				FrameMeshCounter ++ ;

				// 最大ボーンブレンド数と１頂点辺りのサイズをセットする
				MBFrame->MaxBoneBlendNum = ( int )Frame->MaxBoneUseNum ;
				MBFrame->PosUnitSize     = ( int )( sizeof( MV1_MESH_POSITION ) + ( MBFrame->MaxBoneBlendNum - 4 ) * sizeof( MV1_SKINBONE_BLEND ) ) ;

				// 各種フラグをセット
				MBFrame->Flag = 0 ;
				if( Frame->Visible )
				{
					MBFrame->Flag |= MV1_FRAMEFLAG_VISIBLE ;
				}

				if( Frame->EnablePreRotate != 0 )
				{
					MBFrame->Flag |= MV1_FRAMEFLAG_PREROTATE ;
				}

				if( Frame->EnablePostRotate != 0 )
				{
					MBFrame->Flag |= MV1_FRAMEFLAG_POSTROTATE ;
				}

				// 親の行列を無視するかどうかを保存する
				MBFrame->IgnoreParentTransform = Frame->IgnoreParentTransform ;

				// シェイプデータのセット
				if( Frame->ShapeNum )
				{
					MBFrame->ShapeNum = Frame->ShapeNum ;
					MBFrame->Shape = MBase->Shape + MBase->ShapeNum ;
					MBase->ShapeNum += MBFrame->ShapeNum ;

					Shape = Frame->ShapeFirst ;
					MBShape = MBFrame->Shape ;
					for( i = 0 ; i < ( DWORD )Frame->ShapeNum ; i ++, Shape = Shape->Next, MBShape ++ )
					{
						MBShape->Container = MBFrame ;
#ifndef UNICODE
						MBShape->NameA = MV1RGetStringSpace(  MBase, Shape->NameA ) ;
#endif
						MBShape->NameW = MV1RGetStringSpaceW( MBase, Shape->NameW ) ;
						MBShape->Mesh = NULL ;
						MBShape->MeshNum = 0 ;
					}
				}

				// メッシュが存在する場合はメッシュの情報をセット
				if( Frame->MeshNum && Frame->MeshFirst->Positions )
				{
					Mesh = Frame->MeshFirst ;
					for( i = 0 ; i < ( DWORD )Frame->MeshNum ; i ++, Mesh = Mesh->Next )
					{
						_MEMSET( TypeNumVertexDataSize, 0, sizeof( TypeNumVertexDataSize ) ) ;
//						TypeNumIndexDataSize = 0 ;

						// 法線データをセットする
						{
							MV1_MESH_NORMAL *MBNormal ;
							VECTOR *Normal ;

							if( MBFrame->Normal == NULL )
							{
								MBFrame->Normal = MBase->MeshNormal + MBase->MeshNormalNum ;
								MBFrame->NormalNum = 0 ;
							}

							MBNormal = MBFrame->Normal + MBFrame->NormalNum ;
							Normal = Mesh->Normals ;
							for( j = 0 ; ( DWORD )j < Mesh->NormalNum ; j ++, MBNormal ++, Normal ++ )
							{
								MBNormal->Normal = *Normal ;
							}

							MBNormal = MBFrame->Normal + MBFrame->NormalNum ;
							if( Mesh->Binormals != NULL )
							{
								MBFrame->Flag |= MV1_FRAMEFLAG_TANGENT_BINORMAL ;

								Normal = Mesh->Binormals ;
								for( j = 0 ; ( DWORD )j < Mesh->NormalNum ; j ++, MBNormal ++, Normal ++ )
								{
									MBNormal->Binormal = *Normal ;
								}
							}
							else
							{
								for( j = 0 ; ( DWORD )j < Mesh->NormalNum ; j ++, MBNormal ++ )
								{
									MBNormal->Binormal.x = 0.0f ;
									MBNormal->Binormal.y = 0.0f ;
									MBNormal->Binormal.z = 0.0f ;
								}
							}

							MBNormal = MBFrame->Normal + MBFrame->NormalNum ;
							if( Mesh->Binormals != NULL )
							{
								MBFrame->Flag |= MV1_FRAMEFLAG_TANGENT_BINORMAL ;

								Normal = Mesh->Tangents ;
								for( j = 0 ; ( DWORD )j < Mesh->NormalNum ; j ++, MBNormal ++, Normal ++ )
								{
									MBNormal->Tangent = *Normal ;
								}
							}
							else
							{
								for( j = 0 ; ( DWORD )j < Mesh->NormalNum ; j ++, MBNormal ++ )
								{
									MBNormal->Tangent.x = 0.0f ;
									MBNormal->Tangent.y = 0.0f ;
									MBNormal->Tangent.z = 0.0f ;
								}
							}

							MeshNormalStartNum    = ( DWORD )MBFrame->NormalNum ;
							MBFrame->NormalNum   += Mesh->NormalNum ;
							MBase->MeshNormalNum += Mesh->NormalNum ;
						}

						// 座標データをセットする
						{
							VECTOR *Position ;
							MV1_MESH_POSITION *MBPosition ;

							if( MBFrame->Position == NULL )
							{
								MBFrame->Position = ( MV1_MESH_POSITION * )( ( BYTE * )MBase->MeshPosition + MBase->MeshPositionSize ) ;
								MBFrame->PositionNum = 0 ;
							}

							MeshBoneStartNum = ( DWORD )MBFrame->UseSkinBoneNum ;
							MBPosition = ( MV1_MESH_POSITION * )( ( BYTE * )MBFrame->Position + MBFrame->PosUnitSize * MBFrame->PositionNum ) ;
							Position = Mesh->Positions ;
							for( j = 0 ; ( DWORD )j < Mesh->PositionNum ; j ++, MBPosition = ( MV1_MESH_POSITION * )( ( BYTE * )MBPosition + MBFrame->PosUnitSize ), Position ++ )
							{
								MBPosition->Position = *Position ;

								// 使用するボーンインデックスとウエイト値をセット
								if( MBFrame->MaxBoneBlendNum > 0 )
								{
									float TotalWeight = 0.0f ;

									BoneNum = GetBitListNumber( &Mesh->SkinVerticesBlend, j, BitBuf ) ;
									VertB   = MBPosition->BoneWeight ;
									for( m = 0 ; m < BoneNum ; m ++, VertB ++ )
									{
										VertB->Index = ( int )( BitBuf[ m ] + MeshBoneStartNum ) ;
										VertB->W     = Mesh->SkinVerticeWeightInfo[ Mesh->SkinWeightsNum * j + BitBuf[ m ] ] ;
										TotalWeight += VertB->W ;
									}
									if( TotalWeight != 1.0f )
									{
										float Scale = 1.0f / TotalWeight ;
										VertB = MBPosition->BoneWeight ;
										for( m = 0 ; m < BoneNum ; m ++, VertB ++ )
										{
											VertB->W *= Scale ;
										}
									}
									if( BoneNum != MBFrame->MaxBoneBlendNum )
									{
										VertB->Index = -1 ;
										VertB->W     = 0.0f ;
									}
								}
							}
							MeshPositionStartNum     = ( DWORD )MBFrame->PositionNum ;
							MBFrame->PositionNum    += Mesh->PositionNum ;
							MBase->MeshPositionSize += Mesh->PositionNum * MBFrame->PosUnitSize ;
						}

						// スキニングメッシュのウエイトの情報をセットする
//						StartMatrixIndex = MBase->SkinBoneNum ;
						MBSkinW = MBase->SkinBone + MBase->SkinBoneNum ;
						for( j = 0 ; ( DWORD )j < Mesh->SkinWeightsNum ; j ++, MBSkinW ++ )
						{
							MBSkinW->BoneFrame = ( int )Mesh->SkinWeights[ j ]->TargetFrame ;
							if( MBSkinW->BoneFrame == -1 )
							{
								char FrameNameUTF16LE[ 512 ] ;

								ConvString( ( const char * )Frame->NameW, -1, WCHAR_T_CHARCODEFORMAT, FrameNameUTF16LE, sizeof( FrameNameUTF16LE ), DX_CHARCODEFORMAT_UTF16LE ) ;
								DXST_LOGFILEFMT_ADDUTF16LE( ( "\x52\x00\x65\x00\x61\x00\x64\x00\x20\x00\x4d\x00\x6f\x00\x64\x00\x65\x00\x6c\x00\x20\x00\x43\x00\x6f\x00\x6e\x00\x76\x00\x65\x00\x72\x00\x74\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xd5\x30\xec\x30\xfc\x30\xe0\x30\x20\x00\x25\x00\x73\x00\x20\x00\x6e\x30\xb9\x30\xad\x30\xcb\x30\xf3\x30\xb0\x30\xe1\x30\xc3\x30\xb7\x30\xe5\x30\x6e\x30\xb9\x30\xad\x30\xf3\x30\xa6\x30\xa8\x30\xa4\x30\xc8\x30\xc5\x60\x31\x58\x67\x30\x00\x4e\xf4\x81\x59\x30\x8b\x30\xce\x30\xfc\x30\xc9\x30\x6e\x30\x21\x71\x44\x30\xea\x30\xf3\x30\xaf\x30\xc5\x60\x31\x58\x4c\x30\x42\x30\x8a\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Read Model Convert Error : フレーム %s のスキニングメッシュのスキンウエイト情報で一致するノードの無いリンク情報がありました\n" @*/, FrameNameUTF16LE ) ) ;
								goto ERRORLABEL ;
							}
							ConvertMatrixFToMatrix4x4cF( &MBSkinW->ModelLocalMatrix, &Mesh->SkinWeights[ j ]->ModelLocalMatrix ) ;
							if(                  MBSkinW->ModelLocalMatrix.m[ 0 ][ 0 ]                  == 1.0f &&
								( *( ( DWORD * )&MBSkinW->ModelLocalMatrix.m[ 1 ][ 0 ] ) & 0x7fffffff ) == 0 &&
								( *( ( DWORD * )&MBSkinW->ModelLocalMatrix.m[ 2 ][ 0 ] ) & 0x7fffffff ) == 0 &&
							    ( *( ( DWORD * )&MBSkinW->ModelLocalMatrix.m[ 0 ][ 1 ] ) & 0x7fffffff ) == 0 &&
								                 MBSkinW->ModelLocalMatrix.m[ 1 ][ 1 ]                  == 1.0f &&
								( *( ( DWORD * )&MBSkinW->ModelLocalMatrix.m[ 2 ][ 1 ] ) & 0x7fffffff ) == 0 &&
							    ( *( ( DWORD * )&MBSkinW->ModelLocalMatrix.m[ 0 ][ 2 ] ) & 0x7fffffff ) == 0 &&
								( *( ( DWORD * )&MBSkinW->ModelLocalMatrix.m[ 1 ][ 2 ] ) & 0x7fffffff ) == 0 &&
								                 MBSkinW->ModelLocalMatrix.m[ 2 ][ 2 ]                  == 1.0f )
							{
								MBSkinW->ModelLocalMatrixIsTranslateOnly = 1 ;
							}
							else
							{
								MBSkinW->ModelLocalMatrixIsTranslateOnly = 0 ;
							}

							MBSkinW->UseFrameNum = 1 ;
							MBSkinW->UseFrame = MBase->SkinBoneUseFrame + MBase->SkinBoneUseFrameNum ;
							MBSkinW->UseFrame[ 0 ].Index = MBFrame->Index ;
							MBase->SkinBoneUseFrameNum ++ ;

							if( MBFrame->UseSkinBone == NULL )
							{
								MBFrame->UseSkinBone = MBase->FrameUseSkinBone + MBase->FrameUseSkinBoneNum ;
								MBFrame->UseSkinBoneNum = 0 ;
							}
							MBFrame->UseSkinBone[ MBFrame->UseSkinBoneNum ] = MBSkinW ;
							MBFrame->UseSkinBoneNum ++ ;
							MBase->FrameUseSkinBoneNum ++ ;
						}
						MBase->SkinBoneNum += Mesh->SkinWeightsNum ;

						// マテリアルリストがある場合はメッシュの数はマテリアルの数だけ
						// 無い場合は一個
						if( MBFrame->Mesh == NULL )
						{
							MBFrame->Mesh = MBase->Mesh + MBase->MeshNum ;
						}
						MeshStartNum      = ( DWORD )MBFrame->MeshNum ;
						MeshNowNum        = Mesh->MaterialNum ? Mesh->MaterialNum : 1 ;
						MBFrame->MeshNum += MeshNowNum ;
						MBase->MeshNum   += MeshNowNum ;

						// トライアングルリストを作成
						{
							// メッシュの数だけ繰り返し
							MBMesh = MBFrame->Mesh + MeshStartNum ;
							for( o = 0 ; ( DWORD )o < MeshNowNum ; o ++, MBMesh ++ )
							{
								// 親アドレスを保存
								MBMesh->Container = MBFrame ;

								// 状態変化管理用基本情報のセット
								MV1ChangeInfoSetup( &MBMesh->ChangeInfo, NULL, FrameMeshCounter + 1, 0 ) ;
								FrameMeshCounter ++ ;

								// トライアングルリストのアドレスをセット
								MBMesh->TriangleListNum = 0 ;
								MBMesh->TriangleList = MBase->TriangleList + MBase->TriangleListNum ;

								// 基本的にメッシュの初期状態は表示
								MBMesh->Visible = 1 /*( MBFrame->Flag & MV1_FRAMEFLAG_VISIBLE ) != 0 ? 1 : 0*/ ;

								// 頂点のディフューズカラーとスペキュラカラーを使用するかどうかをセット
								MBMesh->UseVertexDiffuseColor = FALSE ;
								MBMesh->UseVertexSpecularColor = FALSE ;

								// マテリアルのアドレスをセット
								if( Mesh->MaterialNum )
								{
									MBMesh->Material = MBase->Material + Mesh->Materials[ o ]->Index ;
								}

								// バックカリングの有無のセット
								MBMesh->BackCulling = ( BYTE )( Mesh->Materials[ o ] == NULL || Mesh->Materials[ o ]->DisableBackCulling == FALSE ? TRUE : FALSE ) ;

								// 頂点データと面データを構築する
								{
									BYTE TVertBuf[ sizeof( MV1_MESH_VERTEX ) + sizeof( float ) * 2 * 32 ] ;
									MV1_MESH_VERTEX *TVertex ;

									TVertex = ( MV1_MESH_VERTEX * )TVertBuf ;

									// ＵＶの数をセット
									MBMesh->UVSetUnitNum = Mesh->Materials[ o ] == NULL || Mesh->Materials[ o ]->DiffuseTexNum == 0 ? 1 : Mesh->Materials[ o ]->DiffuseTexNum ;
									MBMesh->UVUnitNum = 2 ;

									// 頂点データの先頭アドレスをセット
									MBMesh->Vertex       = ( MV1_MESH_VERTEX * )( ( BYTE * )MBase->MeshVertex + MBase->MeshVertexSize ) ;
									MBMesh->VertUnitSize = ( int )( sizeof( MV1_MESH_VERTEX ) + MBMesh->UVSetUnitNum * MBMesh->UVUnitNum * sizeof( float ) - sizeof( float ) * 2 ) ;

									// 面データのアドレスをセット
									MBMesh->Face = MBase->MeshFace + MBase->MeshFaceNum ;

									// 頂点データと面データを構築
									MBMesh->VertexNum = 0 ;
									MeshFace = Mesh->Faces ;
									MBFace = MBMesh->Face ;
									_MEMSET( VertInfoTable, 0, sizeof( MV1_MAKEVERTINDEXINFO * ) * MaxPositionNum ) ;
									VertInfoNum = 0 ;
									for( j = 0 ; ( DWORD )j < Mesh->FaceNum ; j ++, MeshFace ++ )
									{
										// 使用するマテリアルが異なる場合は何もしない
										if( MeshFace->MaterialIndex != ( DWORD )o ) continue ;

										// 元が右手座標系だったらインデックスの順番を変更する
/*										if( ReadModel->MeshFaceRightHand )
										{
											p = MBFace->VertexIndex[ 1 ] ;
											MBFace->VertexIndex[ 1 ] = MBFace->VertexIndex[ 2 ] ;
											MBFace->VertexIndex[ 2 ] = p ;
										}*/

										// 頂点タイプと所属トライアングルリスト番号を保存
										MBFace->VertexType        = ( WORD )( Mesh->SkinFaceType        == NULL ? MV1_VERTEX_TYPE_NORMAL : ( WORD )Mesh->SkinFaceType[ j ]        ) ;
										MBFace->TriangleListIndex = ( WORD )( Mesh->FaceUseTriangleList == NULL ? -1                     : ( WORD )Mesh->FaceUseTriangleList[ j ] ) ;

										// ポリゴンの頂点の数だけ繰り返し
										for( m = 0 ; m < 3 ; m ++ )
										{
											// 追加しようとしている頂点データの作成
											TVertex->ToonOutLineScale = Mesh->PositionToonOutLineScale == NULL ? 1.0f : Mesh->PositionToonOutLineScale[ MeshFace->VertexIndex[ m ] ] ;
											TVertex->PositionIndex = MeshFace->VertexIndex[ m ] + MeshPositionStartNum ;
											TVertex->NormalIndex = MeshFace->NormalIndex[ m ] + MeshNormalStartNum ;
											if( Mesh->VertexColors )
											{
												ColorF = &Mesh->VertexColors[ MeshFace->VertexColorIndex[ m ] ] ;
												TVertex->DiffuseColor.r = ( BYTE )( ColorF->r >= 1.0f ? 255 : ( ColorF->r < 0.0f ? 0 : _FTOL( ColorF->r * 255.0f ) ) ) ;
												TVertex->DiffuseColor.g = ( BYTE )( ColorF->g >= 1.0f ? 255 : ( ColorF->g < 0.0f ? 0 : _FTOL( ColorF->g * 255.0f ) ) ) ;
												TVertex->DiffuseColor.b = ( BYTE )( ColorF->b >= 1.0f ? 255 : ( ColorF->b < 0.0f ? 0 : _FTOL( ColorF->b * 255.0f ) ) ) ;
												TVertex->DiffuseColor.a = ( BYTE )( ColorF->a >= 1.0f ? 255 : ( ColorF->a < 0.0f ? 0 : _FTOL( ColorF->a * 255.0f ) ) ) ;
											}
											else
											{
												if( MBMesh->Material == NULL )
												{
													TVertex->DiffuseColor.r = 255 ;
													TVertex->DiffuseColor.g = 255 ;
													TVertex->DiffuseColor.b = 255 ;
													TVertex->DiffuseColor.a = 255 ;
												}
												else
												{
													TVertex->DiffuseColor.r = ( BYTE )( MBMesh->Material->Diffuse.r >= 1.0f ? 255 : ( MBMesh->Material->Diffuse.r < 0.0f ? 0 : _FTOL( MBMesh->Material->Diffuse.r * 255.0f ) ) ) ;
													TVertex->DiffuseColor.g = ( BYTE )( MBMesh->Material->Diffuse.g >= 1.0f ? 255 : ( MBMesh->Material->Diffuse.g < 0.0f ? 0 : _FTOL( MBMesh->Material->Diffuse.g * 255.0f ) ) ) ;
													TVertex->DiffuseColor.b = ( BYTE )( MBMesh->Material->Diffuse.b >= 1.0f ? 255 : ( MBMesh->Material->Diffuse.b < 0.0f ? 0 : _FTOL( MBMesh->Material->Diffuse.b * 255.0f ) ) ) ;
													TVertex->DiffuseColor.a = ( BYTE )( MBMesh->Material->Diffuse.a >= 1.0f ? 255 : ( MBMesh->Material->Diffuse.a < 0.0f ? 0 : _FTOL( MBMesh->Material->Diffuse.a * 255.0f ) ) ) ;
												}
											}
											if( TVertex->DiffuseColor.a != 255 ) MBMesh->NotOneDiffuseAlpha = TRUE ;

											if( MBMesh->Material == NULL )
											{
												TVertex->SpecularColor.r = 255 ;
												TVertex->SpecularColor.g = 255 ;
												TVertex->SpecularColor.b = 255 ;
												TVertex->SpecularColor.a = 255 ;
											}
											else
											{
												TVertex->SpecularColor.r = ( BYTE )( MBMesh->Material->Specular.r >= 1.0f ? 255 : ( MBMesh->Material->Specular.r < 0.0f ? 0 : _FTOL( MBMesh->Material->Specular.r * 255.0f ) ) ) ;
												TVertex->SpecularColor.g = ( BYTE )( MBMesh->Material->Specular.g >= 1.0f ? 255 : ( MBMesh->Material->Specular.g < 0.0f ? 0 : _FTOL( MBMesh->Material->Specular.g * 255.0f ) ) ) ;
												TVertex->SpecularColor.b = ( BYTE )( MBMesh->Material->Specular.b >= 1.0f ? 255 : ( MBMesh->Material->Specular.b < 0.0f ? 0 : _FTOL( MBMesh->Material->Specular.b * 255.0f ) ) ) ;
												TVertex->SpecularColor.a = ( BYTE )( MBMesh->Material->Specular.a >= 1.0f ? 255 : ( MBMesh->Material->Specular.a < 0.0f ? 0 : _FTOL( MBMesh->Material->Specular.a * 255.0f ) ) ) ;
											}

											for( s = 0 ; s < MBMesh->UVSetUnitNum ; s ++ )
											{
												if( Mesh->UVs[ s ] )
												{
													TVertex->UVs[ s ][ 0 ] = Mesh->UVs[ s ][ MeshFace->UVIndex[ s ][ m ] ].x ;
													TVertex->UVs[ s ][ 1 ] = Mesh->UVs[ s ][ MeshFace->UVIndex[ s ][ m ] ].y ;
												}
												else
												{
													TVertex->UVs[ s ][ 0 ] = 0.0f ;
													TVertex->UVs[ s ][ 1 ] = 0.0f ;
												}
											}

											// 今までに同じ頂点データが無かったかどうかを調べる
											for( VInfo = VertInfoTable[ MeshFace->VertexIndex[ m ] ] ; VInfo ; VInfo = VInfo->Next )
											{
												if( VInfo->TriangleListIndex == MBFace->TriangleListIndex &&
													_MEMCMP( ( BYTE * )MBMesh->Vertex + VInfo->VertexIndex * MBMesh->VertUnitSize, TVertex, MBMesh->VertUnitSize ) == 0 )
													break ;
											}
											if( VInfo == NULL )
											{
												// 無かったらデータを追加
												VInfo = &VertInfoBuffer[ VertInfoNum ] ;
												VertInfoNum ++ ;

												VInfo->TriangleListIndex = MBFace->TriangleListIndex ;
												VInfo->VertexIndex = MBMesh->VertexNum ;
												VInfo->Next = VertInfoTable[ MeshFace->VertexIndex[ m ] ] ;
												VertInfoTable[ MeshFace->VertexIndex[ m ] ] = VInfo ;

												_MEMCPY( ( BYTE * )MBMesh->Vertex + MBMesh->VertexNum * MBMesh->VertUnitSize, TVertex, ( size_t )MBMesh->VertUnitSize ) ;
												MBFace->VertexIndex[ m ] = ( DWORD )MBMesh->VertexNum ;
												MBMesh->VertexNum ++ ;
											}
											else
											{
												// あったらインデックスをセット
												MBFace->VertexIndex[ m ] = ( DWORD )VInfo->VertexIndex ;
											}
										}

										// 面の数をインクリメント
										MBFace ++ ;
										MBMesh->FaceNum ++ ;
									}

									// 元が右手座標系だったらインデックスの順番を変更する
									if( ReadModel->MeshFaceRightHand )
									{
										MBFace = MBMesh->Face ;
										for( j = 0 ; j < MBMesh->FaceNum ; j ++, MBFace ++ )
										{
											p = ( int )MBFace->VertexIndex[ 1 ] ;
											MBFace->VertexIndex[ 1 ] = MBFace->VertexIndex[ 2 ] ;
											MBFace->VertexIndex[ 2 ] = ( DWORD )p ;
										}
									}

									// 面の総数と頂点データの総サイズを加算
									MBase->MeshFaceNum += MBMesh->FaceNum ;
									MBase->MeshVertexSize += MBMesh->VertexNum * MBMesh->VertUnitSize ;

									// 従法線と接線を算出する
//									MV1MakeMeshBinormalsAndTangents( MBMesh ) ;
								}

								// ボーン一つのトライアングルリストの作成
								if( Mesh->MaterialPolyList[ o ].TypeNum[ MV1_VERTEX_TYPE_NORMAL ] )
								{
									// トライアングルリストのアドレスをセット
									MBTList = MBMesh->TriangleList + MBMesh->TriangleListNum ;
									MBase->TriangleListNum  += Mesh->MaterialPolyList[ o ].SimpleTriangleListNum ;
									MBMesh->TriangleListNum += Mesh->MaterialPolyList[ o ].SimpleTriangleListNum ;

									// トライアングルリストの数だけ繰り返し
									l = 0 ;
									MBFace = MBMesh->Face ;
									for( j = 0 ; j < Mesh->MaterialPolyList[ o ].SimpleTriangleListNum ; j ++, MBTList ++ )
									{
										// トライアングルリストの情報をセット
										MBTList->Container = MBMesh ;
										MBTList->VertexType = MV1_VERTEX_TYPE_NORMAL ;

										// １頂点のサイズをセット
										MBTList->PosUnitSize = sizeof( MV1_TLIST_NORMAL_POS ) ;

										// 頂点インデックスデータの先頭アドレスをセット
										MBTList->Index = MBase->TriangleListIndex + MBase->TriangleListIndexNum ;
										MBTList->IndexNum = 0 ;
										MBTList->ToonOutLineIndex = MBTList->Index + MTBase.TriangleListIndexNum ;
										MBTList->ToonOutLineIndexNum = 0 ;

										// 頂点データの先頭アドレスをセット
										MBTList->NormalPosition = MBase->TriangleListNormalPosition + MBase->TriangleListNormalPositionNum ;
										MBTList->NormalPosition = ( MV1_TLIST_NORMAL_POS * )( ( ( DWORD_PTR )MBTList->NormalPosition + 15 ) / 16 * 16 ) ;

										// メッシュ頂点インデックスデータの先頭アドレスをセット
										MBTList->MeshVertexIndex = MBase->MeshVertexIndex + MBase->MeshVertexIndexNum ;
										MBTList->VertexNum = 0 ;

										// 頂点が既に存在しているかどうかのフラグを初期化する
										_MEMSET( VertValidBuffer, 0xff, MBMesh->VertexNum * sizeof( int ) ) ;

										// すべての面を処理するまでループ
//										NormV = MBTList->NormalPosition ;
										for( ; l < MBMesh->FaceNum && MBTList->IndexNum < MV1_TRIANGLE_MAX_INDEX ; l ++, MBFace ++ )
										{
											// メッシュのトライアングルリスト番号の更新
											MBFace->TriangleListIndex = ( WORD )( MBTList - MBMesh->TriangleList ) ;

											// ポリゴンの頂点の数だけ繰り返し
											for( p = 0 ; p < 3 ; p ++, MBTList->IndexNum ++ )
											{
												// トライアングルが使用している頂点データが既にあるかどうかを調べる
												if( VertValidBuffer[ MBFace->VertexIndex[ p ] ] == -1 )
												{
													// 無かったら追加する
//													MBMVert = ( MV1_MESH_VERTEX   * )( ( BYTE * )MBMesh->Vertex    + MBFace->VertexIndex[ p ] * MBMesh->VertUnitSize   ) ;
//													MBMPos  = ( MV1_MESH_POSITION * )( ( BYTE * )MBFrame->Position + MBFrame->PosUnitSize     * MBMVert->PositionIndex ) ;
//													MBMNorm = MBFrame->Normal + MBMVert->NormalIndex ;
//													*( ( VECTOR * )&NormV->Position ) = MBMPos->Position ;
//													NormV->Position.w = 1.0f ;
//													*( ( VECTOR * )&NormV->Normal ) = MBMNorm->Normal ;
//													NormV->Normal.w = 0.0f ;
//													NormV ++ ;

													VertValidBuffer[ MBFace->VertexIndex[ p ] ] = MBTList->VertexNum ;
													MBTList->Index[ MBTList->IndexNum ] = ( unsigned short )MBTList->VertexNum ;

													MBTList->MeshVertexIndex[ MBTList->VertexNum ] = MBFace->VertexIndex[ p ] ;
													MBTList->VertexNum ++ ;
												}
												else
												{
													MBTList->Index[ MBTList->IndexNum ] = ( unsigned short )VertValidBuffer[ MBFace->VertexIndex[ p ] ] ;
												}
											}
										}

										// 頂点インデックスの数を加算
										MBase->TriangleListIndexNum += MBTList->IndexNum ;

										// メッシュ頂点インデックスデータの数を加算する
										MBase->MeshVertexIndexNum += MBTList->VertexNum ;

										// ボーン無し頂点データの数を加算
										MBase->TriangleListNormalPositionNum += MBTList->VertexNum ;

										// 高速アクセス用頂点データのセットアップ
										MV1SetupTriangleListPositionAndNormal( MBTList ) ;

										// トゥーン輪郭線用頂点インデックスの作成
										MV1SetupToonOutLineTriangleList( MBTList ) ;
									}
								}

								// ボーン５個未満のトライアングルリストの作成
								if( Mesh->MaterialPolyList[ o ].TypeNum[ MV1_VERTEX_TYPE_SKIN_4BONE ] )
								{
									MBFrame->IsSkinMesh = TRUE ;

									// トライアングルリストのアドレスをセット
									MBTList = MBMesh->TriangleList + MBMesh->TriangleListNum ;
									MBase->TriangleListNum  += Mesh->MaterialPolyList[ o ].SkinB4TriangleListNum ;
									MBMesh->TriangleListNum += Mesh->MaterialPolyList[ o ].SkinB4TriangleListNum ;

									// トライアングルリストの数だけ繰り返し
									TList = Mesh->MaterialPolyList[ o ].SkinB4TriangleList ;
									for( j = 0 ; j < Mesh->MaterialPolyList[ o ].SkinB4TriangleListNum ; j ++, TList ++, MBTList ++ )
									{
										// トライアングルリストの情報をセット
										MBTList->Container = MBMesh ;
										MBTList->VertexType = MV1_VERTEX_TYPE_SKIN_4BONE ;

										// １頂点のサイズをセット
										MBTList->PosUnitSize = sizeof( MV1_TLIST_SKIN_POS_4B ) ;

										// 使用するボーンインデックスをセット
										for( l = 0 ; l < TList->UseBoneNum ; l ++ )
											MBTList->UseBone[ l ] = ( int )( TList->UseBone[ l ] + MeshBoneStartNum ) ;
										for( ; l < MV1_TRIANGLE_LIST_USE_BONE_MAX_NUM ; l ++ )
											MBTList->UseBone[ l ] = -1 ;
										MBTList->UseBoneNum = TList->UseBoneNum ;

										// 頂点インデックスデータの先頭アドレスをセット
										MBTList->Index = MBase->TriangleListIndex + MBase->TriangleListIndexNum ;
										MBTList->IndexNum = 0 ;
										MBTList->ToonOutLineIndex = MBTList->Index + MTBase.TriangleListIndexNum ;
										MBTList->ToonOutLineIndexNum = 0 ;

										// 頂点データの先頭アドレスをセット
										MBTList->SkinPosition4B = MBase->TriangleListSkinPosition4B + MBase->TriangleListSkinPosition4BNum ;
										MBTList->SkinPosition4B = ( MV1_TLIST_SKIN_POS_4B * )( ( ( DWORD_PTR )MBTList->SkinPosition4B + 15 ) / 16 * 16 ) ;

										// メッシュ頂点インデックスデータの先頭アドレスをセット
										MBTList->MeshVertexIndex = MBase->MeshVertexIndex + MBase->MeshVertexIndexNum ;
										MBTList->VertexNum = 0 ;

										// 頂点が既に存在しているかどうかのフラグを初期化する
										_MEMSET( VertValidBuffer, 0xff, MBMesh->VertexNum * sizeof( int ) ) ;

										// すべての面を処理するまでループ
										MBFace = MBMesh->Face ;
//										Skin4BV = MBTList->SkinPosition4B ;
										for( l = 0 ; l < MBMesh->FaceNum ; l ++, MBFace ++ )
										{
											// ボーン５個未満ではない場合は何もしない
											if( MBFace->VertexType != MV1_VERTEX_TYPE_SKIN_4BONE ) continue ;

											// トライアングルリスト番号が違う場合も何もしない
											if( MBFace->TriangleListIndex != j ) continue ;

											// メッシュのトライアングルリスト番号の更新
											MBFace->TriangleListIndex = ( WORD )( MBTList - MBMesh->TriangleList ) ;

											// 頂点データをセット
											for( p = 0 ; p < 3 ; p ++, MBTList->IndexNum ++ )
											{
												// トライアングルが使用している頂点データが既にあるかどうかを調べる
												if( VertValidBuffer[ MBFace->VertexIndex[ p ] ] == -1 )
												{
													// 無かったら追加する
//													MBMVert = ( MV1_MESH_VERTEX   * )( ( BYTE * )MBMesh->Vertex    + MBFace->VertexIndex[ p ] * MBMesh->VertUnitSize   ) ;
//													MBMPos  = ( MV1_MESH_POSITION * )( ( BYTE * )MBFrame->Position + MBFrame->PosUnitSize     * MBMVert->PositionIndex ) ;
//													MBMNorm = MBFrame->Normal + MBMVert->NormalIndex ;
//													*( ( VECTOR * )&Skin4BV->Position ) = MBMPos->Position ;
//													Skin4BV->Position.w = 1.0f ;
//													Skin4BV->Normal = MBMNorm->Normal ;
//													VertB = MBMPos->BoneWeight ;
//													for( k = 0 ; k < 4 && k < MBFrame->MaxBoneBlendNum ; k ++, VertB ++ )
//													{
//														if( VertB->Index == -1 ) break ;
//
//														for( m = 0 ; m < MBTList->UseBoneNum && MBTList->UseBone[ m ] != VertB->Index ; m ++ ){}
//														Skin4BV->MatrixIndex[ k ] = m ;
//														Skin4BV->MatrixWeight[ k ] = VertB->W ;
//													}
//													for( ; k < 4 ; k ++ )
//													{
//														Skin4BV->MatrixIndex[ k ] = 0 ;
//														Skin4BV->MatrixWeight[ k ] = 0.0f ;
//													}
//													Skin4BV ++ ;

													VertValidBuffer[ MBFace->VertexIndex[ p ] ] = MBTList->VertexNum ;
													MBTList->Index[ MBTList->IndexNum ] = ( unsigned short )MBTList->VertexNum ;

													MBTList->MeshVertexIndex[ MBTList->VertexNum ] = MBFace->VertexIndex[ p ] ;
													MBTList->VertexNum ++ ;
												}
												else
												{
													MBTList->Index[ MBTList->IndexNum ] = ( unsigned short )VertValidBuffer[ MBFace->VertexIndex[ p ] ] ;
												}
											}
										}

										// 頂点インデックスの数を加算
										MBase->TriangleListIndexNum += MBTList->IndexNum ;

										// メッシュ頂点インデックスデータの数を加算する
										MBase->MeshVertexIndexNum += MBTList->VertexNum ;

										// ４ボーン以内スキニングメッシュ頂点データの数を加算
										MBase->TriangleListSkinPosition4BNum += MBTList->VertexNum ;

										// 高速アクセス用頂点データのセットアップ
										MV1SetupTriangleListPositionAndNormal( MBTList ) ;

										// トゥーン輪郭線用頂点インデックスの作成
										MV1SetupToonOutLineTriangleList( MBTList ) ;
									}
								}

								// ボーン９個未満のトライアングルリストの作成
								if( Mesh->MaterialPolyList[ o ].TypeNum[ MV1_VERTEX_TYPE_SKIN_8BONE ] )
								{
									MBFrame->IsSkinMesh = TRUE ;

									// トライアングルリストのアドレスをセット
									MBTList = MBMesh->TriangleList + MBMesh->TriangleListNum ;
									MBase->TriangleListNum  += Mesh->MaterialPolyList[ o ].SkinB8TriangleListNum ;
									MBMesh->TriangleListNum += Mesh->MaterialPolyList[ o ].SkinB8TriangleListNum ;

									// トライアングルリストの数だけ繰り返し
									TList = Mesh->MaterialPolyList[ o ].SkinB8TriangleList ;
									for( j = 0 ; j < Mesh->MaterialPolyList[ o ].SkinB8TriangleListNum ; j ++, TList ++, MBTList ++ )
									{
										// トライアングルリストの情報をセット
										MBTList->Container = MBMesh ;
										MBTList->VertexType = MV1_VERTEX_TYPE_SKIN_8BONE ;

										// １頂点のサイズをセット
										MBTList->PosUnitSize = sizeof( MV1_TLIST_SKIN_POS_8B ) ;

										// 使用するボーンインデックスをセット
										for( l = 0 ; l < TList->UseBoneNum ; l ++ )
											MBTList->UseBone[ l ] = ( int )( TList->UseBone[ l ] + MeshBoneStartNum ) ;
										for( ; l < MV1_TRIANGLE_LIST_USE_BONE_MAX_NUM ; l ++ )
											MBTList->UseBone[ l ] = -1 ;
										MBTList->UseBoneNum = TList->UseBoneNum ;

										// 頂点インデックスデータの先頭アドレスをセット
										MBTList->Index = MBase->TriangleListIndex + MBase->TriangleListIndexNum ;
										MBTList->IndexNum = 0 ;
										MBTList->ToonOutLineIndex = MBTList->Index + MTBase.TriangleListIndexNum ;
										MBTList->ToonOutLineIndexNum = 0 ;

										// 頂点データの先頭アドレスをセット
										MBTList->SkinPosition8B = MBase->TriangleListSkinPosition8B + MBase->TriangleListSkinPosition8BNum ;
										MBTList->SkinPosition8B = ( MV1_TLIST_SKIN_POS_8B * )( ( ( DWORD_PTR )MBTList->SkinPosition8B + 15 ) / 16 * 16 ) ;

										// メッシュ頂点インデックスデータの先頭アドレスをセット
										MBTList->MeshVertexIndex = MBase->MeshVertexIndex + MBase->MeshVertexIndexNum ;
										MBTList->VertexNum = 0 ;

										// 頂点が既に存在しているかどうかのフラグを初期化する
										_MEMSET( VertValidBuffer, 0xff, MBMesh->VertexNum * sizeof( int ) ) ;

										// すべての面を処理するまでループ
										MBFace = MBMesh->Face ;
//										Skin8BV = MBTList->SkinPosition8B ;
										for( l = 0 ; l < MBMesh->FaceNum ; l ++, MBFace ++ )
										{
											// ボーン９個未満ではない場合は何もしない
											if( MBFace->VertexType != MV1_VERTEX_TYPE_SKIN_8BONE ) continue ;

											// トライアングルリスト番号が違う場合も何もしない
											if( MBFace->TriangleListIndex != j ) continue ;

											// メッシュのトライアングルリスト番号の更新
											MBFace->TriangleListIndex = ( WORD )( MBTList - MBMesh->TriangleList ) ;

											// 頂点データをセット
											for( p = 0 ; p < 3 ; p ++, MBTList->IndexNum ++ )
											{
												// トライアングルが使用している頂点データが既にあるかどうかを調べる
												if( VertValidBuffer[ MBFace->VertexIndex[ p ] ] == -1 )
												{
													// 無かったら追加する
/*													MBMVert = ( MV1_MESH_VERTEX   * )( ( BYTE * )MBMesh->Vertex    + MBFace->VertexIndex[ p ] * MBMesh->VertUnitSize   ) ;
													MBMPos  = ( MV1_MESH_POSITION * )( ( BYTE * )MBFrame->Position + MBFrame->PosUnitSize     * MBMVert->PositionIndex ) ;
													MBMNorm = MBFrame->Normal + MBMVert->NormalIndex ;
													Skin8BV->Position = MBMPos->Position ;
													Skin8BV->Normal   = MBMNorm->Normal ;
													VertB = MBMPos->BoneWeight ;
													for( k = 0 ; k < 8 && k < MBFrame->MaxBoneBlendNum ; k ++, VertB ++ )
													{
														if( VertB->Index == -1 ) break ;
														Skin8BV->MatrixWeight[ k ] = VertB->W ;
														for( m = 0 ; m < MBTList->UseBoneNum && MBTList->UseBone[ m ] != VertB->Index ; m ++ ){}
														if( k < 4 )
														{
															Skin8BV->MatrixIndex1[ k ]     = m ;
														}
														else
														{
															Skin8BV->MatrixIndex2[ k - 4 ] = m ;
														}
													}
													for( ; k < 8 ; k ++ )
													{
														Skin8BV->MatrixWeight[ k ] = 0.0f ;
														if( k < 4 )
														{
															Skin8BV->MatrixIndex1[ k ]     = 0 ;
														}
														else
														{
															Skin8BV->MatrixIndex2[ k - 4 ] = 0 ;
														}
													}
													Skin8BV ++ ;
*/
													VertValidBuffer[ MBFace->VertexIndex[ p ] ] = MBTList->VertexNum ;
													MBTList->Index[ MBTList->IndexNum ] = ( unsigned short )MBTList->VertexNum ;

													MBTList->MeshVertexIndex[ MBTList->VertexNum ] = MBFace->VertexIndex[ p ] ;
													MBTList->VertexNum ++ ;
												}
												else
												{
													MBTList->Index[ MBTList->IndexNum ] = ( unsigned short )VertValidBuffer[ MBFace->VertexIndex[ p ] ] ;
												}
											}
										}

										// 頂点インデックスの数を加算
										MBase->TriangleListIndexNum += MBTList->IndexNum ;

										// メッシュ頂点インデックスデータの数を加算する
										MBase->MeshVertexIndexNum += MBTList->VertexNum ;

										// ８ボーン以内スキニングメッシュ頂点データの数を加算
										MBase->TriangleListSkinPosition8BNum += MBTList->VertexNum ;

										// 高速アクセス用頂点データのセットアップ
										MV1SetupTriangleListPositionAndNormal( MBTList ) ;

										// トゥーン輪郭線用頂点インデックスの作成
										MV1SetupToonOutLineTriangleList( MBTList ) ;
									}
								}

								// ボーン数無制限のトライアングルリストの作成
								if( Mesh->MaterialPolyList[ o ].TypeNum[ MV1_VERTEX_TYPE_SKIN_FREEBONE ] )
								{
									MBFrame->IsSkinMesh = TRUE ;

									// トライアングルリストのアドレスをセット
									MBTList = MBMesh->TriangleList + MBMesh->TriangleListNum ;
									MBase->TriangleListNum  += ( Mesh->MaterialPolyList[ o ].TypeNum[ MV1_VERTEX_TYPE_SKIN_FREEBONE ] * 3 + MV1_TRIANGLE_MAX_INDEX - 1 ) / MV1_TRIANGLE_MAX_INDEX ;
									MBMesh->TriangleListNum += ( Mesh->MaterialPolyList[ o ].TypeNum[ MV1_VERTEX_TYPE_SKIN_FREEBONE ] * 3 + MV1_TRIANGLE_MAX_INDEX - 1 ) / MV1_TRIANGLE_MAX_INDEX ;

									// すべての面を処理するまでループ
									MBFace = MBMesh->Face ;
									for( l = 0 ; l < MBMesh->FaceNum ; MBTList ++ )
									{
										// トライアングルリストの情報をセット
										MBTList->Container = MBMesh ;
										MBTList->VertexType = MV1_VERTEX_TYPE_SKIN_FREEBONE ;

										// １頂点あたりのサイズをセット
										MBTList->PosUnitSize = ( unsigned short )( sizeof( MV1_TLIST_SKIN_POS_FREEB ) + sizeof( MV1_SKINBONE_BLEND ) * ( Mesh->MaterialPolyList[ o ].MaxBoneCount - 4 ) ) ;
										MBTList->PosUnitSize = ( unsigned short )( ( MBTList->PosUnitSize + 15 ) / 16 * 16 ) ;

										// 最大ボーン数を保存
										MBTList->MaxBoneNum = Mesh->MaterialPolyList[ o ].MaxBoneCount ;

										// 頂点インデックスデータの先頭アドレスをセット
										MBTList->Index = MBase->TriangleListIndex + MBase->TriangleListIndexNum ;
										MBTList->IndexNum = 0 ;
										MBTList->ToonOutLineIndex = MBTList->Index + MTBase.TriangleListIndexNum ;
										MBTList->ToonOutLineIndexNum = 0 ;

										// 頂点データの先頭アドレスをセット
										MBTList->SkinPositionFREEB = ( MV1_TLIST_SKIN_POS_FREEB * )( ( BYTE * )MBase->TriangleListSkinPositionFREEB + MBase->TriangleListSkinPositionFREEBSize ) ;
										MBTList->SkinPositionFREEB = ( MV1_TLIST_SKIN_POS_FREEB * )( ( ( DWORD_PTR )MBTList->SkinPositionFREEB + 15 ) / 16 * 16 ) ;

										// メッシュ頂点インデックスデータの先頭アドレスをセット
										MBTList->MeshVertexIndex = MBase->MeshVertexIndex + MBase->MeshVertexIndexNum ;
										MBTList->VertexNum = 0 ;

										// 頂点が既に存在しているかどうかのフラグを初期化する
										_MEMSET( VertValidBuffer, 0xff, MBMesh->VertexNum * sizeof( int ) ) ;

										// 頂点データのセット
//										SkinFBV  = MBTList->SkinPositionFREEB ;
										for( ; l < MBMesh->FaceNum && MBTList->IndexNum < MV1_TRIANGLE_MAX_INDEX ; l ++, MBFace ++ )
										{
											// ボーン数無制限ではない場合は何もしない
											if( MBFace->VertexType != MV1_VERTEX_TYPE_SKIN_FREEBONE ) continue ;

											// メッシュのトライアングルリスト番号の更新
											MBFace->TriangleListIndex = ( WORD )( MBTList - MBMesh->TriangleList ) ;

											// 頂点データをセット
											for( p = 0 ; p < 3 ; p ++, MBTList->IndexNum ++ )
											{
												// トライアングルが使用している頂点データが既にあるかどうかを調べる
												if( VertValidBuffer[ MBFace->VertexIndex[ p ] ] == -1 )
												{
													// 無かったら追加する
/*													MBMVert = ( MV1_MESH_VERTEX   * )( ( BYTE * )MBMesh->Vertex    + MBFace->VertexIndex[ p ] * MBMesh->VertUnitSize   ) ;
													MBMPos  = ( MV1_MESH_POSITION * )( ( BYTE * )MBFrame->Position + MBFrame->PosUnitSize     * MBMVert->PositionIndex ) ;
													MBMNorm = MBFrame->Normal + MBMVert->NormalIndex ;
													*( ( VECTOR * )&SkinFBV->Position ) = MBMPos->Position ;
													SkinFBV->Position.w = 1.0f ;
													*( ( VECTOR * )&SkinFBV->Normal ) = MBMNorm->Normal ;
													SkinFBV->Normal.w = 0.0f ;
													VertB = MBMPos->BoneWeight ;
													for( k = 0 ; k < MBTList->MaxBoneNum ; k ++, VertB ++ )
													{
														if( VertB->Index == -1 ) break ;
														SkinFBV->MatrixWeight[ k ].Index = VertB->Index ;
														SkinFBV->MatrixWeight[ k ].W     = VertB->W ;
													}
													if( k != MBTList->MaxBoneNum )
													{
														SkinFBV->MatrixWeight[ k ].Index = -1 ;
														SkinFBV->MatrixWeight[ k ].W     = 0.0f ;
													}
													SkinFBV = ( MV1_TLIST_SKIN_POS_FREEB * )( ( BYTE * )SkinFBV + MBTList->PosUnitSize ) ;
*/
													VertValidBuffer[ MBFace->VertexIndex[ p ] ] = MBTList->VertexNum ;
													MBTList->Index[ MBTList->IndexNum ] = ( unsigned short )MBTList->VertexNum ;

													MBTList->MeshVertexIndex[ MBTList->VertexNum ] = MBFace->VertexIndex[ p ] ;
													MBTList->VertexNum ++ ;
												}
												else
												{
													MBTList->Index[ MBTList->IndexNum ] = ( unsigned short )VertValidBuffer[ MBFace->VertexIndex[ p ] ] ;
												}
											}
										}

										// 頂点インデックスの数を加算
										MBase->TriangleListIndexNum += MBTList->IndexNum ;

										// メッシュ頂点インデックスデータの数を加算する
										MBase->MeshVertexIndexNum += MBTList->VertexNum ;

										// ボーン数無制限スキニングメッシュ頂点データのサイズを加算
										MBase->TriangleListSkinPositionFREEBSize += MBTList->VertexNum * MBTList->PosUnitSize ;

										// 高速アクセス用頂点データのセットアップ
										MV1SetupTriangleListPositionAndNormal( MBTList ) ;

										// トゥーン輪郭線用頂点インデックスの作成
										MV1SetupToonOutLineTriangleList( MBTList ) ;
									}
								}
							}
						}

						// シェイプデータが対象とするメッシュだった場合はシェイプメッシュの情報をセットアップする
						Shape = Frame->ShapeFirst ;
						MBShape = MBFrame->Shape ;
						for( j = 0 ; j < Frame->ShapeNum ; j ++, Shape = Shape->Next, MBShape ++ )
						{
							// 対象のメッシュでは無かったら何もしない
							if( Shape->TargetMesh != Mesh ) continue ;

							// メッシュの数だけ繰り返し
							MBMesh = MBFrame->Mesh + MeshStartNum ;
							for( o = 0 ; ( DWORD )o < MeshNowNum ; o ++, MBMesh ++ )
							{
								MV1_MESH_VERTEX *TVertex ;

								// アドレスをセット
								MBShapeMesh = MBase->ShapeMesh + MBase->ShapeMeshNum ;

								MBShapeMesh->TargetMesh = MBMesh ;
								MBShapeMesh->VertexNum = 0 ;
								MBShapeMesh->Vertex = MBase->ShapeVertex + MBase->ShapeVertexNum ;

								// シェイプ対象の頂点を列挙
								MBShapeVertex = MBShapeMesh->Vertex ;
								TVertex = MBMesh->Vertex ;
								for( l = 0 ; l < MBMesh->VertexNum ; l ++, TVertex = ( MV1_MESH_VERTEX * )( ( BYTE * )TVertex + MBMesh->VertUnitSize ) )
								{
									if( Shape->NextTable[ TVertex->PositionIndex - MeshPositionStartNum ] == 0xffffffff )
										continue ;
									ShapeVertex = &Shape->Vertex[ Shape->NextTable[ TVertex->PositionIndex - MeshPositionStartNum ] ] ;

									MBShapeVertex->TargetMeshVertex = ( DWORD )l ;
									MBShapeVertex->Position         = ShapeVertex->Position ;
									if( Shape->ValidVertexNormal )
									{
										MBShapeVertex->Normal = ShapeVertex->Normal ;
									}
									else
									{
										MBShapeVertex->Normal = VGet( 0.0f, 0.0f, 0.0f ) ;
									}

									MBShapeVertex ++ ;
									MBShapeMesh->VertexNum ++ ;
									MBase->ShapeVertexNum ++ ;
								}

								// 関わっている頂点が一つも無かったら何もしない
								if( MBShapeMesh->VertexNum == 0 ) continue ;

								// シェイプメッシュである印を付ける
								MBMesh->Shape = 1 ;

								// シェイプメッシュ内のトライアングルリストの頂点数を加算する
								MBTList = MBMesh->TriangleList ;
								for( l = 0 ; l < MBMesh->TriangleListNum ; l ++, MBTList ++ )
								{
									switch( MBTList->VertexType )
									{
									case MV1_VERTEX_TYPE_NORMAL :        MBase->ShapeNormalPositionNum     += MBTList->VertexNum ; break ;
									case MV1_VERTEX_TYPE_SKIN_4BONE :    MBase->ShapeSkinPosition4BNum     += MBTList->VertexNum ; break ;
									case MV1_VERTEX_TYPE_SKIN_8BONE :    MBase->ShapeSkinPosition8BNum     += MBTList->VertexNum ; break ;
									case MV1_VERTEX_TYPE_SKIN_FREEBONE : MBase->ShapeSkinPositionFREEBSize += MBTList->VertexNum * MBTList->PosUnitSize ; break ;
									}
								}

								// シェイプメッシュの頂点の数を加算
								MBase->ShapeTargetMeshVertexNum += MBMesh->VertexNum ;

								if( MBShape->MeshNum == 0 )
								{
									MBShape->Mesh = MBShapeMesh ;
								}

								MBShape->MeshNum ++ ;
								MBase->ShapeMeshNum ++ ;
							}
						}
					}
				}

				// ライトが存在する場合はライトの情報をセット
				if( Frame->Light )
				{
					MBLight = MBase->Light + MBase->LightNum ;
					MBFrame->Light = MBLight ;
					MBase->LightNum ++ ;

					// 情報のコピー
					MBLight->Index = Frame->Index ;
					MBLight->FrameIndex = Frame->Light->FrameIndex ;
					MBLight->Type = Frame->Light->Type ;
					MBLight->Diffuse = Frame->Light->Diffuse ;
					MBLight->Specular = Frame->Light->Specular ;
					MBLight->Ambient = Frame->Light->Ambient ;
					MBLight->Range = Frame->Light->Range ;
					MBLight->Falloff = Frame->Light->Falloff ;
					MBLight->Attenuation0 = Frame->Light->Attenuation0 ;
					MBLight->Attenuation1 = Frame->Light->Attenuation1 ;
					MBLight->Attenuation2 = Frame->Light->Attenuation2 ;
					MBLight->Theta = Frame->Light->Theta ;
					MBLight->Phi = Frame->Light->Phi ;

					// 名前をコピー
#ifndef UNICODE
					MBLight->NameA = MV1RGetStringSpace(  MBase, Frame->NameA ) ;
#endif
					MBLight->NameW = MV1RGetStringSpaceW( MBase, Frame->NameW ) ;
				}
			}

			// 物理演算で使用する剛体の情報を処理する
			PhysicsRigidBody = ReadModel->PhysicsRigidBodyFirst ;
			for( i = 0 ; i < ReadModel->PhysicsRigidBodyNum ; i ++, PhysicsRigidBody = PhysicsRigidBody->DataNext )
			{
				// アドレスをセット
				MBPhysicsRigidBody = MBase->PhysicsRigidBody + MBase->PhysicsRigidBodyNum ;
				MBase->PhysicsRigidBodyNum ++ ;

				// 名前を保存
#ifndef UNICODE
				MBPhysicsRigidBody->NameA = MV1RGetStringSpace(  MBase, PhysicsRigidBody->NameA ) ;
#endif
				MBPhysicsRigidBody->NameW = MV1RGetStringSpaceW( MBase, PhysicsRigidBody->NameW ) ;

				// インデックスのセット
				MBPhysicsRigidBody->Index = PhysicsRigidBody->Index ;

				// パラメータのコピー
				MBPhysicsRigidBody->TargetFrame = &MBase->Frame[ PhysicsRigidBody->TargetFrame->Index ] ;
				MBPhysicsRigidBody->TargetFrame->PhysicsRigidBody = MBPhysicsRigidBody ;
				MBPhysicsRigidBody->RigidBodyGroupIndex = PhysicsRigidBody->RigidBodyGroupIndex ;
				MBPhysicsRigidBody->RigidBodyGroupTarget = PhysicsRigidBody->RigidBodyGroupTarget ;
				MBPhysicsRigidBody->ShapeType = PhysicsRigidBody->ShapeType ;
				MBPhysicsRigidBody->ShapeW = PhysicsRigidBody->ShapeW ;
				MBPhysicsRigidBody->ShapeH = PhysicsRigidBody->ShapeH ;
				MBPhysicsRigidBody->ShapeD = PhysicsRigidBody->ShapeD ;
				MBPhysicsRigidBody->Position = PhysicsRigidBody->Position ;
				MBPhysicsRigidBody->Rotation = PhysicsRigidBody->Rotation ;
				MBPhysicsRigidBody->RigidBodyWeight = PhysicsRigidBody->RigidBodyWeight ;
				MBPhysicsRigidBody->RigidBodyPosDim = PhysicsRigidBody->RigidBodyPosDim ;
				MBPhysicsRigidBody->RigidBodyRotDim = PhysicsRigidBody->RigidBodyRotDim ;
				MBPhysicsRigidBody->RigidBodyRecoil = PhysicsRigidBody->RigidBodyRecoil ;
				MBPhysicsRigidBody->RigidBodyFriction = PhysicsRigidBody->RigidBodyFriction ;
				MBPhysicsRigidBody->RigidBodyType = PhysicsRigidBody->RigidBodyType ;
				MBPhysicsRigidBody->NoCopyToBone = PhysicsRigidBody->NoCopyToBone ;
			}

			// 物理演算で使用するジョイントの情報を処理する
			PhysicsJoint = ReadModel->PhysicsJointFirst ;
			for( i = 0 ; i < ReadModel->PhysicsJointNum ; i ++, PhysicsJoint = PhysicsJoint->DataNext )
			{
				// アドレスをセット
				MBPhysicsJoint = MBase->PhysicsJoint + MBase->PhysicsJointNum ;
				MBase->PhysicsJointNum ++ ;

				// 名前を保存
#ifndef UNICODE
				MBPhysicsJoint->NameA = MV1RGetStringSpace(  MBase, PhysicsJoint->NameA ) ;
#endif
				MBPhysicsJoint->NameW = MV1RGetStringSpaceW( MBase, PhysicsJoint->NameW ) ;

				// インデックスのセット
				MBPhysicsJoint->Index = PhysicsJoint->Index ;

				// パラメータのコピー
				MBPhysicsJoint->RigidBodyA = &MBase->PhysicsRigidBody[ PhysicsJoint->RigidBodyA ] ;
				MBPhysicsJoint->RigidBodyB = &MBase->PhysicsRigidBody[ PhysicsJoint->RigidBodyB ] ;
				MBPhysicsJoint->Position = PhysicsJoint->Position ;
				MBPhysicsJoint->Rotation = PhysicsJoint->Rotation ;
				MBPhysicsJoint->ConstrainPosition1 = PhysicsJoint->ConstrainPosition1 ;
				MBPhysicsJoint->ConstrainPosition2 = PhysicsJoint->ConstrainPosition2 ;
				MBPhysicsJoint->ConstrainRotation1 = PhysicsJoint->ConstrainRotation1 ;
				MBPhysicsJoint->ConstrainRotation2 = PhysicsJoint->ConstrainRotation2 ;
				MBPhysicsJoint->SpringPosition = PhysicsJoint->SpringPosition ;
				MBPhysicsJoint->SpringRotation = PhysicsJoint->SpringRotation ;
			}

			// 実際に使用したメモリサイズに応じて頂点データメモリを確保しなおす
			if( ReadModel->NormalPositionNum ||
				ReadModel->SkinPosition4BNum ||
				ReadModel->SkinPosition8BNum ||
				ReadModel->SkinPositionFREEBSize ||
				ReadModel->MeshVertexIndexNum ||
				ReadModel->ShapeVertexNum )
			{
				void *VertexData ;
				MV1_TLIST_NORMAL_POS	*NormalPosition ;
				MV1_TLIST_SKIN_POS_4B	*SkinPosition4B ;
				MV1_TLIST_SKIN_POS_8B	*SkinPosition8B ;
				MV1_TLIST_SKIN_POS_FREEB *SkinPositionFREEB ;
				DWORD					*MeshVertexIndex ;
				MV1_MESH_POSITION		*MeshPosition ;
				MV1_MESH_FACE			*MeshFaceB ;
				MV1_MESH_NORMAL			*MeshNormal ;
				MV1_MESH_VERTEX			*MeshVertex ;
				MV1_SHAPE_VERTEX_BASE	*ShapeVertexB ;

				VertexData        = MBase->VertexData ;
				NormalPosition    = MBase->TriangleListNormalPosition ;
				SkinPosition4B    = MBase->TriangleListSkinPosition4B ;
				SkinPosition8B    = MBase->TriangleListSkinPosition8B ;
				SkinPositionFREEB = MBase->TriangleListSkinPositionFREEB ;
				MeshVertexIndex   = MBase->MeshVertexIndex ;
				MeshPosition      = MBase->MeshPosition ;
				MeshFaceB         = MBase->MeshFace ;
				MeshNormal        = MBase->MeshNormal ;
				MeshVertex        = MBase->MeshVertex ;
				ShapeVertexB      = MBase->ShapeVertex ;

				MBase->VertexDataSize =
					sizeof( DWORD )                 * MBase->MeshVertexIndexNum +
					sizeof( MV1_MESH_FACE )         * MBase->MeshFaceNum        +
					sizeof( MV1_MESH_NORMAL )       * MBase->MeshNormalNum      +
					sizeof( MV1_TLIST_NORMAL_POS )  * MBase->TriangleListNormalPositionNum  +
					sizeof( MV1_TLIST_SKIN_POS_4B ) * MBase->TriangleListSkinPosition4BNum  +
					sizeof( MV1_TLIST_SKIN_POS_8B ) * MBase->TriangleListSkinPosition8BNum  +
					MBase->TriangleListSkinPositionFREEBSize                   + 
					MBase->MeshPositionSize                                    +
					MBase->MeshVertexSize                                      +
					sizeof( MV1_SHAPE_VERTEX_BASE ) * MBase->ShapeVertexNum    + 16 ;
				MBase->VertexData = DXALLOC( MBase->VertexDataSize ) ;
				if( MBase->VertexData == NULL )
				{
					DXFREE( VertexData ) ;
					DXST_LOGFILE_ADDUTF16LE( "\x52\x00\x65\x00\x61\x00\x64\x00\x20\x00\x4d\x00\x6f\x00\x64\x00\x65\x00\x6c\x00\x20\x00\x43\x00\x6f\x00\x6e\x00\x76\x00\x65\x00\x72\x00\x74\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x02\x98\xb9\x70\xa7\x5e\x19\x6a\x68\x30\x02\x98\xb9\x70\xd5\x6c\xda\x7d\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Read Model Convert Error : 頂点座標と頂点法線を格納するメモリ領域の確保に失敗しました\n" @*/ ) ;
					goto ERRORLABEL ;
				}
				MBase->TriangleListNormalPosition    = ( MV1_TLIST_NORMAL_POS     * )( ( ( DWORD_PTR )MBase->VertexData + 15 ) / 16 * 16 ) ;
				MBase->TriangleListSkinPosition4B    = ( MV1_TLIST_SKIN_POS_4B    * )( MBase->TriangleListNormalPosition              + MBase->TriangleListNormalPositionNum     ) ;
				MBase->TriangleListSkinPosition8B    = ( MV1_TLIST_SKIN_POS_8B    * )( MBase->TriangleListSkinPosition4B              + MBase->TriangleListSkinPosition4BNum     ) ;
				MBase->TriangleListSkinPositionFREEB = ( MV1_TLIST_SKIN_POS_FREEB * )( MBase->TriangleListSkinPosition8B              + MBase->TriangleListSkinPosition8BNum     ) ;
				MBase->MeshVertexIndex               = ( DWORD                    * )( ( BYTE * )MBase->TriangleListSkinPositionFREEB + MBase->TriangleListSkinPositionFREEBSize ) ;
				MBase->MeshFace                      = ( MV1_MESH_FACE            * )( MBase->MeshVertexIndex             + MBase->MeshVertexIndexNum    ) ;
				MBase->MeshNormal                    = ( MV1_MESH_NORMAL          * )( MBase->MeshFace                    + MBase->MeshFaceNum           ) ;
				MBase->MeshPosition                  = ( MV1_MESH_POSITION        * )( MBase->MeshNormal                  + MBase->MeshNormalNum         ) ;
				MBase->MeshVertex                    = ( MV1_MESH_VERTEX          * )( ( BYTE * )MBase->MeshPosition      + MBase->MeshPositionSize      ) ;
				MBase->ShapeVertex                   = ( MV1_SHAPE_VERTEX_BASE    * )( ( BYTE * )MBase->MeshVertex        + MBase->MeshVertexSize        ) ;

				// 頂点データをコピーする
				if( MBase->TriangleListNormalPositionNum     ) _MEMCPY( MBase->TriangleListNormalPosition,    NormalPosition,    sizeof( MV1_TLIST_NORMAL_POS )  * MBase->TriangleListNormalPositionNum ) ;
				else                                           MBase->TriangleListNormalPosition    = NULL ;

				if( MBase->TriangleListSkinPosition4BNum     ) _MEMCPY( MBase->TriangleListSkinPosition4B,    SkinPosition4B,    sizeof( MV1_TLIST_SKIN_POS_4B ) * MBase->TriangleListSkinPosition4BNum ) ;
				else                                           MBase->TriangleListSkinPosition4B    = NULL ;

				if( MBase->TriangleListSkinPosition8BNum     ) _MEMCPY( MBase->TriangleListSkinPosition8B,    SkinPosition8B,    sizeof( MV1_TLIST_SKIN_POS_8B ) * MBase->TriangleListSkinPosition8BNum ) ;
				else                                           MBase->TriangleListSkinPosition8B    = NULL ;

				if( MBase->TriangleListSkinPositionFREEBSize ) _MEMCPY( MBase->TriangleListSkinPositionFREEB, SkinPositionFREEB, ( size_t )MBase->TriangleListSkinPositionFREEBSize ) ;
				else                                           MBase->TriangleListSkinPositionFREEB = NULL ;

				if( MBase->MeshVertexIndexNum    ) _MEMCPY( MBase->MeshVertexIndex,   MeshVertexIndex,   sizeof( DWORD ) * MBase->MeshVertexIndexNum ) ;
				else                               MBase->MeshVertexIndex   = NULL ;

				if( MBase->MeshFaceNum           ) _MEMCPY( MBase->MeshFace,          MeshFaceB,         sizeof( MV1_MESH_FACE ) * MBase->MeshFaceNum ) ;
				else                               MBase->MeshFace          = NULL ;

				if( MBase->MeshNormalNum         ) _MEMCPY( MBase->MeshNormal,        MeshNormal,        sizeof( MV1_MESH_NORMAL ) * MBase->MeshNormalNum ) ;
				else                               MBase->MeshNormal        = NULL ;

				if( MBase->MeshPositionSize      ) _MEMCPY( MBase->MeshPosition,      MeshPosition,      ( size_t )MBase->MeshPositionSize ) ;
				else                               MBase->MeshPosition      = NULL ;

				if( MBase->MeshVertexSize        ) _MEMCPY( MBase->MeshVertex,        MeshVertex,        ( size_t )MBase->MeshVertexSize ) ;
				else                               MBase->MeshVertex        = NULL ;

				if( MBase->ShapeVertexNum        ) _MEMCPY( MBase->ShapeVertex,       ShapeVertexB,      sizeof( MV1_SHAPE_VERTEX_BASE ) * MBase->ShapeVertexNum ) ;
				else                               MBase->ShapeVertex       = NULL ;

				// 各フレームのメッシュに関するポインタのアドレスを変更する
				MBFrame = MBase->Frame ;
				for( i = 0 ; i < ( DWORD )MBase->FrameNum ; i ++, MBFrame ++ )
				{
					if( MBFrame->Position )
					{
						MBFrame->Position = ( MV1_MESH_POSITION * )( ( DWORD_PTR )MBFrame->Position - ( DWORD_PTR )MeshPosition + ( DWORD_PTR )MBase->MeshPosition ) ;
					}

					if( MBFrame->Normal )
					{
						MBFrame->Normal   = ( MV1_MESH_NORMAL   * )( ( DWORD_PTR )MBFrame->Normal   - ( DWORD_PTR )MeshNormal   + ( DWORD_PTR )MBase->MeshNormal   ) ;
					}
				}
				
				// 各メッシュの頂点データのアドレスを変更する
				MBMesh = MBase->Mesh ;
				for( i = 0 ; i < ( DWORD )MBase->MeshNum ; i ++, MBMesh ++ )
				{
					if( MBMesh->Face )
					{
						MBMesh->Face = ( MV1_MESH_FACE * )( ( DWORD_PTR )MBMesh->Face - ( DWORD_PTR )MeshFaceB + ( DWORD_PTR )MBase->MeshFace ) ;
					}

					if( MBMesh->Vertex )
					{
						MBMesh->Vertex = ( MV1_MESH_VERTEX * )( ( DWORD_PTR )MBMesh->Vertex - ( DWORD_PTR )MeshVertex + ( DWORD_PTR )MBase->MeshVertex ) ;
					}
				}

				// 各シェイプメッシュの頂点データのアドレスを変更する
				MBShapeMesh = MBase->ShapeMesh ;
				for( i = 0 ; i < ( DWORD )MBase->ShapeMeshNum ; i ++, MBShapeMesh ++ )
				{
					if( MBShapeMesh->Vertex )
					{
						MBShapeMesh->Vertex = ( MV1_SHAPE_VERTEX_BASE * )( ( DWORD_PTR )MBShapeMesh->Vertex - ( DWORD_PTR )ShapeVertexB + ( DWORD_PTR )MBase->ShapeVertex ) ;
					}
				}

				// 各トライアングルリストのポインタのアドレスを変更する
				MBMesh = MBase->Mesh ;
				for( i = 0 ; i < ( DWORD )MBase->MeshNum ; i ++, MBMesh ++ )
				{
					MBTList = MBMesh->TriangleList ;
					for( j = 0 ; j < MBMesh->TriangleListNum ; j ++, MBTList ++ )
					{
						if( MBTList->VertexNum == 0 ) continue ;

						MBTList->MeshVertexIndex = ( DWORD * )( ( DWORD_PTR )MBTList->MeshVertexIndex - ( DWORD_PTR )MeshVertexIndex + ( DWORD_PTR )MBase->MeshVertexIndex ) ;

						switch( MBTList->VertexType )
						{
						case MV1_VERTEX_TYPE_NORMAL        : MBTList->NormalPosition    = ( MV1_TLIST_NORMAL_POS     * )( ( DWORD_PTR )MBTList->NormalPosition    - ( DWORD_PTR )NormalPosition    + ( DWORD_PTR )MBase->TriangleListNormalPosition    ) ; break ;
						case MV1_VERTEX_TYPE_SKIN_4BONE    : MBTList->SkinPosition4B    = ( MV1_TLIST_SKIN_POS_4B    * )( ( DWORD_PTR )MBTList->SkinPosition4B    - ( DWORD_PTR )SkinPosition4B    + ( DWORD_PTR )MBase->TriangleListSkinPosition4B    ) ; break ;
						case MV1_VERTEX_TYPE_SKIN_8BONE    : MBTList->SkinPosition8B    = ( MV1_TLIST_SKIN_POS_8B    * )( ( DWORD_PTR )MBTList->SkinPosition8B    - ( DWORD_PTR )SkinPosition8B    + ( DWORD_PTR )MBase->TriangleListSkinPosition8B    ) ; break ;
						case MV1_VERTEX_TYPE_SKIN_FREEBONE : MBTList->SkinPositionFREEB = ( MV1_TLIST_SKIN_POS_FREEB * )( ( DWORD_PTR )MBTList->SkinPositionFREEB - ( DWORD_PTR )SkinPositionFREEB + ( DWORD_PTR )MBase->TriangleListSkinPositionFREEB ) ; break ;
						}
					}
				}

				// 一時的に使用していた頂点バッファを解放する
				DXFREE( VertexData ) ;
			}

			// 各フレームのポリゴンの数と頂点の数をセットする
			MBMesh = MBase->Mesh ;
			for( i = 0 ; i < ( DWORD )MBase->MeshNum ; i ++, MBMesh ++ )
			{
				MBTList = MBMesh->TriangleList ;
				for( j = 0 ; j < MBMesh->TriangleListNum ; j ++, MBTList ++ )
				{
					MBMesh->Container->TriangleNum += MBTList->IndexNum / 3 ;
					MBMesh->Container->VertexNum += MBTList->VertexNum ;
					MBase->TriangleListVertexNum += MBTList->VertexNum ;
				}
			}

			// 各ボーンとフレームの使用行列のリンク情報をセットする
			MBSkinW = MBase->SkinBone ;
			for( i = 0 ; i < ( DWORD )MBase->SkinBoneNum ; i ++, MBSkinW ++ )
			{
				MBSkinWF = MBSkinW->UseFrame ;
				for( j = 0 ; j < MBSkinW->UseFrameNum ; j ++, MBSkinWF ++ )
				{
					MBFrame = &MBase->Frame[ MBSkinWF->Index ] ;
					for( k = 0 ; k < MBFrame->UseSkinBoneNum && MBFrame->UseSkinBone[ k ] != MBSkinW ; k ++ ){}
					if( k == MBFrame->UseSkinBoneNum )
					{
						// ここにはこないはず
						DXST_LOGFILE_ADDUTF16LE( "\xa8\x30\xe9\x30\xfc\x30\x10\xff\x00"/*@ L"エラー０" @*/ ) ;
						return -1 ;
					}
					MBSkinWF->MatrixIndex = k ;
				}
			}

			// 変更情報管理用のデータサイズを１６の倍数にする
			MBase->ChangeDrawMaterialTableSize = ( MBase->ChangeDrawMaterialTableSize + 15 ) / 16 * 16 ;
			MBase->ChangeMatrixTableSize       = ( MBase->ChangeMatrixTableSize       + 15 ) / 16 * 16 ;

			// スキニングメッシュ用のボーン情報を最適化
			MV1OptimizeSkinBoneInfo( MBase ) ;
		}
	}

	// 半透明要素のあるマテリアルを持つメッシュをバックカリング無しにする
	if( ReadModel->TranslateIsBackCulling )
	{
		MBMesh = MBase->Mesh ;
		for( i = 0 ; i < ( DWORD )MBase->MeshNum ; i ++, MBMesh ++ )
		{
			if( MBMesh->Material->Diffuse.a < 0.99999999f )
				MBMesh->BackCulling = 0 ;
		}
	}

	// 初期行列のセットアップ
	MV1SetupInitializeMatrixBase( MBase ) ;

	// メッシュの半透明かどうかの情報をセットアップする
	MV1SetupMeshSemiTransStateBase( MBase ) ;

	// 同時複数描画関係の情報をセットアップする
	if( MBase->UsePackDraw )
	{
		MV1SetupPackDrawInfo( MBase ) ;
	}

	// メモリの解放
	if( VertInfoTable )
	{
		DXFREE( VertInfoTable ) ;
		VertInfoTable = NULL ;
	}

	// 指定がある場合は座標の最適化を行う
	if( GParam->LoadModelToPositionOptimize )
	{
		MV1PositionOptimizeBase( NewHandle ) ;
	}

	// 指定がある場合は法線の再計算を行う
	if( GParam->LoadModelToReMakeNormal )
	{
		MV1ReMakeNormalBase( NewHandle, GParam->LoadModelToReMakeNormalSmoothingAngle, ASyncThread ) ;
	}

	// 高速処理用頂点データの構築
	MBTList = MBase->TriangleList ;
	for( i = 0 ; i < ( DWORD )MBase->TriangleListNum ; i ++, MBTList ++ )
	{
		MV1SetupTriangleListPositionAndNormal( MBTList ) ;
		MV1SetupToonOutLineTriangleList( MBTList ) ;
	}

#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		DecASyncLoadCount( NewHandle ) ;
	}
#endif // DX_NON_ASYNCLOAD

	// ハンドルを返す
	return NewHandle ;

	// エラー処理
ERRORLABEL :

	// ハンドルが有効な場合はハンドルの削除
	if( NewHandle != -1 )
	{
#ifndef DX_NON_ASYNCLOAD
		if( ASyncThread )
		{
			DecASyncLoadCount( NewHandle ) ;
		}
#endif // DX_NON_ASYNCLOAD

		MV1SubModelBase( NewHandle ) ;
		NewHandle = -1 ;
	}

	// メモリの解放
	if( VertInfoTable )
	{
		DXFREE( VertInfoTable ) ;
		VertInfoTable = NULL ;
	}

	return -1 ;
}

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif






