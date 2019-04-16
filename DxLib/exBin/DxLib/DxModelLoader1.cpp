// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		ＦＢＸモデルデータ読み込みプログラム
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#define __DX_MAKE

#include "DxModelRead.h"

#ifndef DX_NON_MODEL
#ifdef DX_LOAD_FBX_MODEL

// インクルード ---------------------------------
#include "DxLog.h"

#include "fbxsdk.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 -----------------------------------

// データ宣言 -----------------------------------

// ＦＢＸデータ構造体
struct FBX_MODEL
{
	FbxManager					*pManager ;
	FbxIOSettings				*pIOSettings ;
	FbxImporter					*pImporter ;
	FbxScene					*pScene ;
	FbxNode						*pNode ;
} ;

// 関数宣言 -------------------------------------

static int AnalyseFbx( MV1_MODEL_R *RModel, FBX_MODEL *Model ) ;														// FBXファイルの解析( 0:成功  -1:失敗 )
static int AnalyseFbxNode( MV1_MODEL_R *RModel, FBX_MODEL *Model, MV1_FRAME_R *ParentFrame, FbxNode *pFbxNode ) ;		// ノードの解析( -1:エラー )
static int GetFbxAnimInfo( MV1_MODEL_R *RModel, FBX_MODEL *Model, MV1_FRAME_R *Frame, MV1_ANIMSET_R *AnimSet, MV1_ANIM_R **Anim, int DataType, FbxAnimCurve *FbxCurve, bool Reverse = false, bool DeggToRad = false ) ;	// ＦＢＸカーブからアニメーション情報を取得する( -1:エラー )
static MV1_TEXTURE_R *FbxAddTexture( MV1_MODEL_R *RModel, FbxTexture *_FbxTexture ) ;				// ＦＢＸテクスチャを追加する

// プログラム -----------------------------------

// ＦＢＸテクスチャを追加する
static MV1_TEXTURE_R *FbxAddTexture( MV1_MODEL_R *RModel, FbxTexture *_FbxTexture )
{
	MV1_TEXTURE_R *Texture ;
	FbxFileTexture *pFbxFileTexture ;
	FbxProceduralTexture *pProceduralTexture ;
	const char *RelativeFileName ;
	int i ;
	int UTF8Length ;
	int ShiftJISLength ;

	pFbxFileTexture = FbxCast< FbxFileTexture >( _FbxTexture ) ;
	pProceduralTexture = FbxCast< FbxProceduralTexture >( _FbxTexture ) ;

	// ファイルテクスチャではない場合はエラー
	if( pFbxFileTexture == NULL )
		return NULL ;

	// モデル上に既にこのテクスチャの情報がある場合はアドレスだけ保存する
	Texture = RModel->TextureFirst ;
	for( i = 0 ; i < ( int )RModel->TextureNum && Texture->UserData != _FbxTexture ; i ++, Texture = Texture->DataNext ){}
	if( i != RModel->TextureNum )
		return Texture ;

	// テクスチャの追加
	{
		// テクスチャパスの作成
		RelativeFileName = pFbxFileTexture->GetRelativeFileName() ;

		// UTF8 の文字列なのか ShiftJIS の文字列なのかを判定
		UTF8Length     = GetStringCharNum( ( const char * )RelativeFileName, DX_CHARCODEFORMAT_UTF8 ) ;
		ShiftJISLength = GetStringCharNum( ( const char * )RelativeFileName, DX_CHARCODEFORMAT_SHIFTJIS ) ;

		// 長さ無しの場合は特別処理
		if( UTF8Length == 0 && ShiftJISLength == 0 )
		{
			Texture = MV1RAddTexture( RModel, "NoName", "", NULL, FALSE, 0.1f, false ) ;
		}
		else
		{
			// UTF8 の場合はそのまま渡す
			if( UTF8Length > ShiftJISLength )
			{
				Texture = MV1RAddTexture( RModel, _FbxTexture->GetName(), RelativeFileName, NULL, FALSE, 0.1f, false ) ;
			}
			else
			{
				char *FilePathUTF8 ;

				// ShiftJIS の場合は UTF8 に変換する
				FilePathUTF8 = ( char * )DXALLOC( ShiftJISLength * 16 ) ;
				if( FilePathUTF8 == NULL )
				{
					return NULL ;
				}
				ConvString( ( const char * )RelativeFileName, -1, DX_CHARCODEFORMAT_SHIFTJIS, FilePathUTF8, ShiftJISLength * 16, DX_CHARCODEFORMAT_UTF8 ) ;

				// 文字数が短くなってしまった場合は UTF8 として処理する
				UTF8Length = GetStringCharNum( FilePathUTF8, DX_CHARCODEFORMAT_UTF8 ) ;
				if( UTF8Length < ShiftJISLength )
				{
					Texture = MV1RAddTexture( RModel, _FbxTexture->GetName(), RelativeFileName, NULL, FALSE, 0.1f, false ) ;
				}
				else
				{
					Texture = MV1RAddTexture( RModel, _FbxTexture->GetName(), FilePathUTF8, NULL, FALSE, 0.1f, false, false, true, true ) ;

					// ShiftJIS としての読み込みに失敗したら UTF8 として読み込んでみる
					if( Texture == NULL )
					{
						Texture = MV1RAddTexture( RModel, _FbxTexture->GetName(), RelativeFileName, NULL, FALSE, 0.1f, false, false, true, true ) ;

						// UTF8 としても失敗した場合は改めて ShiftJIS として読み込む
						if( Texture == NULL )
						{
							Texture = MV1RAddTexture( RModel, _FbxTexture->GetName(), FilePathUTF8, NULL, FALSE, 0.1f, false ) ;
						}
					}
				}

				DXFREE( FilePathUTF8 ) ;
			}
		}

		if( Texture == NULL )
		{
			return NULL ;
		}
	}

	// ＦＢＸのアドレスを保存
	Texture->UserData = _FbxTexture ;

	// ラップモードを保存
	Texture->AddressModeU = _FbxTexture->GetWrapModeU() == FbxTexture::eRepeat ? DX_TEXADDRESS_WRAP : DX_TEXADDRESS_CLAMP ;
	Texture->AddressModeV = _FbxTexture->GetWrapModeV() == FbxTexture::eRepeat ? DX_TEXADDRESS_WRAP : DX_TEXADDRESS_CLAMP ;

	// ＵＶスケールを保存
	Texture->ScaleU = ( float )_FbxTexture->GetScaleU() ;
	Texture->ScaleV = ( float )_FbxTexture->GetScaleV() ;

	// アドレスを返す
	return Texture ;
}

// ＦＢＸカーブからアニメーション情報を取得する( -1:エラー )
static int GetFbxAnimInfo(
	MV1_MODEL_R *RModel,
	FBX_MODEL *Model,
	MV1_FRAME_R *Frame,
	MV1_ANIMSET_R *AnimSet,
	MV1_ANIM_R **AnimP,
	int DataType,
	FbxAnimCurve *pFbxCurve,
	bool Reverse,
	bool DeggToRad
)
{
	MV1_ANIMKEYSET_R *KeySet ;
	float *KeyLinear ;
	float *KeyTime ;
	MV1_ANIM_R *Anim ;
	float FirstValue ;
	bool AllFirstValue ;
	int i, KeyNum ;

	// カーブが NULL だったら何もしない
	if( pFbxCurve == NULL ) return 0 ;

	// キーが無かったら何もしない
	KeyNum = pFbxCurve->KeyGetCount() ;
	if( KeyNum == 0 )
		return 0 ;

	// アニメーションがまだ追加されていない場合は追加
	if( *AnimP == NULL )
	{
		*AnimP = MV1RAddAnim( RModel, AnimSet ) ;
		if( *AnimP == NULL )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\xa2\x30\xcb\x30\xe1\x30\xfc\x30\xb7\x30\xe7\x30\xf3\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\xfd\x8f\xa0\x52\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : アニメーションオブジェクトの追加に失敗しました\n" @*/ )) ;
			return -1 ;
		}
	}
	Anim = *AnimP ;

	// キーセットの追加
	KeySet = MV1RAddAnimKeySet( RModel, Anim ) ;
	if( KeySet == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\xa2\x30\xcb\x30\xe1\x30\xfc\x30\xb7\x30\xe7\x30\xf3\x30\xad\x30\xfc\x30\xbb\x30\xc3\x30\xc8\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\xfd\x8f\xa0\x52\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : アニメーションキーセットオブジェクトの追加に失敗しました\n" @*/ )) ;
		return -1 ;
	}

	// データタイプをセット
	KeySet->DataType = DataType ;

	// キータイプはとりあえず線形補間
	KeySet->Type = MV1_ANIMKEY_TYPE_LINEAR ;

	// キーを格納するためのメモリ領域を確保
	KeySet->KeyLinear = ( float * )ADDMEMAREA( ( sizeof( float ) + sizeof( float ) ) * KeyNum, &RModel->Mem ) ;
	if( KeySet->KeyLinear == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\xa2\x30\xcb\x30\xe1\x30\xfc\x30\xb7\x30\xe7\x30\xf3\x30\xad\x30\xfc\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\x5f\x30\x81\x30\x6e\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : アニメーションキーを格納するためのメモリの確保に失敗しました\n" @*/ )) ;
		return -1 ;
	}
	KeySet->KeyTime = ( float * )( KeySet->KeyLinear + KeyNum ) ;

	// 最初のキーの値を取得
	AllFirstValue = true ;
	FirstValue = static_cast< float >( pFbxCurve->KeyGetValue( 0 ) ) ;
	if( Reverse   ) FirstValue = -FirstValue ;
	if( DeggToRad ) FirstValue =  FirstValue * DX_PI_F / 180.0f ;

	// キーの数だけ繰り返し
	KeyLinear = KeySet->KeyLinear ;
	KeyTime = KeySet->KeyTime ;
	KeySet->TotalTime = 0.0f ;
	for( i = 0 ; i < KeyNum ; i ++ )
	{
		// 時間の取得
		*KeyTime = ( float )pFbxCurve->KeyGetTime( i ).GetFrameCountPrecise() ;
		if( KeySet->TotalTime < *KeyTime ) KeySet->TotalTime = *KeyTime ;
		if( *KeyTime < AnimSet->StartTime ) AnimSet->StartTime = *KeyTime ;
		if( *KeyTime > AnimSet->EndTime   ) AnimSet->EndTime   = *KeyTime ;

		// 値の取得
		*KeyLinear = static_cast< float >( pFbxCurve->KeyGetValue( i ) ) ;
		if( Reverse ) *KeyLinear = -*KeyLinear ;
		if( DeggToRad ) *KeyLinear = *KeyLinear * DX_PI_F / 180.0f ;

		// 最初のパラメータと同じか比較する
		if( AllFirstValue && FirstValue != *KeyLinear )
			AllFirstValue = false ;

		// 二個以上同じキーが続いている場合は統合する
		if( KeySet->Num >= 2 &&
			KeyLinear[ -2 ] == KeyLinear[ -1 ] &&
			KeyLinear[ -1 ] == KeyLinear[  0 ] )
		{
			KeyTime[ -1 ] = KeyTime[ 0 ] ;
		}
		else
		{
			// キーの数を加算する
			KeySet->Num ++ ;
			KeyLinear ++ ;
			KeyTime ++ ;
		}
	}

	// 最初のキーと全部同じ場合はキーの数を１個にする
	if( AllFirstValue )
	{
		KeySet->Num = 1 ;
	}

	// キーデータのサイズを加算する
	RModel->AnimKeyDataSize += ( sizeof( float ) + sizeof( float ) ) * KeySet->Num ;

	// 終了
	return 0 ;
}

// ノードの解析( -1:エラー )
static int AnalyseFbxNode( MV1_MODEL_R *RModel, FBX_MODEL *Model, MV1_FRAME_R *ParentFrame, FbxNode *pFbxNode )
{
	FbxNode *pFbxChildNode ;
	FbxNodeAttribute *FbxAttr ;
	FbxVector4 *FbxVec ;
	FbxVector4 *FbxShapeVec ;
	FbxMesh *_FbxMesh ;
	FbxLayerElement::EMappingMode FbxMappingMode ;
	FbxLayerElement::EReferenceMode FbxRefMode ;
	FbxBlendShape *_FbxBlendShape ;
	FbxBlendShapeChannel *_FbxBlendShapeChannel ;
	FbxGeometryElementNormal *FbxNormalElem ;
	FbxGeometryElementBinormal *FbxBinormalElem ;
	FbxGeometryElementTangent *FbxTangentElem ;
	FbxGeometryElementVertexColor *FbxVertexColorElem ;
	FbxGeometryElementUV *FbxUVElem ;
	FbxGeometryElementMaterial *FbxMaterialElem ;
	FbxSurfaceMaterial *FbxMaterial, **FbxMaterialDim ;
	FbxSurfaceLambert *FbxLambert ;
	FbxSurfacePhong *FbxPhong ;
	FbxTexture *_FbxTexture ;
	FbxLayeredTexture *_FbxLayeredTexture ;
	FbxSkin *_FbxSkin ;
	FbxShape *_FbxShape ; 
	FbxCluster *_FbxCluster ;
	MV1_FRAME_R *Frame ;
	MV1_MESH_R *Mesh ;
	MV1_MESHFACE_R *MeshFace ;
	MV1_MATERIAL_R *Material ;
	MV1_SKIN_WEIGHT_R *SkinWeight ;
	MV1_SHAPE_R *Shape ;
	MV1_SHAPE_VERTEX_R *ShapeVert ;
	VECTOR *MeshPos ;
	MATRIX ReverseMat ;
	int i, j, k, l, Num, LayerNum, Index, MaterialNum ;
	char UTF16LE_Buffer[ 1024 ] ;

	// FbxNode が NULL だったらトップノードをセットアップする
	if( pFbxNode == NULL )
	{
		pFbxNode = Model->pScene->GetRootNode() ;
		Frame = NULL ;
	}
	else
	{
		// フレームを追加
		Frame = MV1RAddFrame( RModel, pFbxNode->GetName(), ParentFrame ) ;
		if( Frame == NULL )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\xd5\x30\xec\x30\xfc\x30\xe0\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\xfd\x8f\xa0\x52\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : フレームオブジェクトの追加に失敗しました\n" @*/ )) ;
			return -1 ;
		}

		// 右手座標系から左手座標系に変換するための行列の準備
		CreateIdentityMatrix( &ReverseMat ) ;
		ReverseMat.m[ 2 ][ 2 ] = -1.0f ;

		// FBXノードのアドレスを保存
		Frame->UserData = pFbxNode ;

		// 座標変換値のパラメータを取得する
		{
			FbxVector4 lTmpVector;

			Frame->Translate.x = ( float ) pFbxNode->LclTranslation.Get()[ 0 ] ;
			Frame->Translate.y = ( float ) pFbxNode->LclTranslation.Get()[ 1 ] ;
			Frame->Translate.z = ( float )-pFbxNode->LclTranslation.Get()[ 2 ] ;

			Frame->Scale.x = ( float )pFbxNode->LclScaling.Get()[ 0 ] ;
			Frame->Scale.y = ( float )pFbxNode->LclScaling.Get()[ 1 ] ;
			Frame->Scale.z = ( float )pFbxNode->LclScaling.Get()[ 2 ] ;

			Frame->Rotate.x = ( float )( -pFbxNode->LclRotation.Get()[ 0 ] * DX_PI / 180.0f ) ;
			Frame->Rotate.y = ( float )( -pFbxNode->LclRotation.Get()[ 1 ] * DX_PI / 180.0f ) ;
			Frame->Rotate.z = ( float )(  pFbxNode->LclRotation.Get()[ 2 ] * DX_PI / 180.0f ) ;

			Frame->RotateOrder = ( int )pFbxNode->RotationOrder.Get() ;

			lTmpVector = pFbxNode->GetPreRotation( FbxNode::eSourcePivot ) ;
			Frame->PreRotate.x = ( float )lTmpVector[ 0 ] ;
			Frame->PreRotate.y = ( float )lTmpVector[ 1 ] ;
			Frame->PreRotate.z = ( float )lTmpVector[ 2 ] ;
			if( *( ( DWORD * )&Frame->PreRotate.x ) != 0 || 
				*( ( DWORD * )&Frame->PreRotate.y ) != 0 ||
				*( ( DWORD * )&Frame->PreRotate.z ) != 0 )
			{
				Frame->PreRotate.x = -Frame->PreRotate.x * DX_PI_F / 180.0f ;
				Frame->PreRotate.y = -Frame->PreRotate.y * DX_PI_F / 180.0f ;
				Frame->PreRotate.z =  Frame->PreRotate.z * DX_PI_F / 180.0f ;
				Frame->EnablePreRotate = 1 ;
			}

			lTmpVector = pFbxNode->GetPostRotation( FbxNode::eSourcePivot ) ;
			Frame->PostRotate.x = ( float )lTmpVector[ 0 ] ;
			Frame->PostRotate.y = ( float )lTmpVector[ 1 ] ;
			Frame->PostRotate.z = ( float )lTmpVector[ 2 ] ;
			if( *( ( DWORD * )&Frame->PostRotate.x ) != 0 || 
				*( ( DWORD * )&Frame->PostRotate.y ) != 0 ||
				*( ( DWORD * )&Frame->PostRotate.z ) != 0 )
			{
				Frame->PostRotate.x = -Frame->PostRotate.x * DX_PI_F / 180.0f ;
				Frame->PostRotate.y = -Frame->PostRotate.y * DX_PI_F / 180.0f ;
				Frame->PostRotate.z =  Frame->PostRotate.z * DX_PI_F / 180.0f ;
				Frame->EnablePostRotate = 1 ;
			}
		}

		// アトリビュートを取得
		FbxAttr = pFbxNode->GetNodeAttribute() ;
		if( FbxAttr )
		{
			// メッシュノードの場合はメッシュを取得する
			if( FbxAttr->GetAttributeType() == FbxNodeAttribute::eMesh )
			{
				// メッシュのアドレスを取得
				_FbxMesh = ( FbxMesh * )FbxAttr ;

				// ポリゴンが１つもない場合は何もしない
				if( _FbxMesh->GetPolygonCount() > 0 )
				{
					// メッシュを追加
					Mesh = MV1RAddMesh( RModel, Frame ) ;
					if( Mesh == NULL )
					{
						DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\xe1\x30\xc3\x30\xb7\x30\xe5\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\xfd\x8f\xa0\x52\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : メッシュオブジェクトの追加に失敗しました\n" @*/ )) ;
						return -1 ;
					}

					// レイヤーの数を取得
					LayerNum = _FbxMesh->GetLayerCount() ;

					// 面の情報を取得
					{
						// 面情報を格納するメモリ領域の確保
						if( MV1RSetupMeshFaceBuffer( RModel, Mesh, _FbxMesh->GetPolygonCount(), 4 ) < 0 )
						{
							DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\x62\x97\xc5\x60\x31\x58\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : 面情報を格納するメモリの確保に失敗しました\n" @*/ )) ;
							return -1 ;
						}

						// 面の情報を取得
						MeshFace = Mesh->Faces ;
						for( i = 0 ; i < ( int )Mesh->FaceNum ; i ++, MeshFace ++ )
						{
							// インデックスの数を取得
							MeshFace->IndexNum = _FbxMesh->GetPolygonSize( i ) ;
							if( Mesh->FaceUnitMaxIndexNum < MeshFace->IndexNum )
							{
								if( MV1RSetupMeshFaceBuffer( RModel, Mesh, Mesh->FaceNum, MeshFace->IndexNum ) < 0 )
								{
									DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\x62\x97\xc5\x60\x31\x58\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\x8d\x51\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : 面情報を格納するメモリの再確保に失敗しました\n" @*/ )) ;
									return -1 ;
								}
								MeshFace = Mesh->Faces + i ;
							}

							// インデックスを取得
							for( j = 0 ; j < ( int )MeshFace->IndexNum ; j ++ )
							{
								MeshFace->VertexIndex[ j ] = _FbxMesh->GetPolygonVertex( i, j ) ;
							}
						}
					}

					// 頂点座標の情報を取得
					{
						// 座標情報の数を取得
						Mesh->PositionNum = _FbxMesh->GetControlPointsCount() ;

						// 座標情報を格納するメモリ領域の確保
						Mesh->Positions = ( VECTOR * )ADDMEMAREA( sizeof( VECTOR ) * Mesh->PositionNum, &RModel->Mem ) ;
						if( Mesh->Positions == NULL )
						{
							DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\x02\x98\xb9\x70\xa7\x5e\x19\x6a\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : 頂点座標を格納するメモリの確保に失敗しました\n" @*/ )) ;
							return -1 ;
						}

						// 座標の情報を取得
						MeshPos = Mesh->Positions ;
						FbxVec = _FbxMesh->GetControlPoints() ;
						for( i = 0 ; i < ( int )Mesh->PositionNum ; i ++, FbxVec ++, MeshPos ++ )
						{
							MeshPos->x = ( float ) FbxVec->mData[ 0 ] ;
							MeshPos->y = ( float ) FbxVec->mData[ 1 ] ;
							MeshPos->z = ( float )-FbxVec->mData[ 2 ] ;
						}
					}

					// 法線の情報を取得
					{

						// 法線エレメントの数だけ繰り返し
						for( i = 0 ; i < _FbxMesh->GetElementNormalCount() ; i ++ )
						{
							FbxNormalElem = _FbxMesh->GetElementNormal( i ) ;

							// モードの取得
							FbxRefMode     = FbxNormalElem->GetReferenceMode() ;
							FbxMappingMode = FbxNormalElem->GetMappingMode() ;

							switch( FbxRefMode )
							{
							case FbxGeometryElement::eDirect :
								// 法線情報の数を取得
								Mesh->NormalNum = FbxNormalElem->GetDirectArray().GetCount() ;

								// 法線情報を格納するメモリ領域の確保
								Mesh->Normals = ( VECTOR * )ADDMEMAREA( sizeof( VECTOR ) * Mesh->NormalNum, &RModel->Mem ) ;
								if( Mesh->Normals == NULL )
								{
									DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\xd5\x6c\xda\x7d\xc5\x60\x31\x58\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : 法線情報を格納するメモリの確保に失敗しました\n" @*/ )) ;
									return -1 ;
								}

								// 直接モードの場合はそのまま代入
								for( j = 0 ; ( DWORD )j < Mesh->NormalNum ; j ++ )
								{
									Mesh->Normals[ j ].x = ( float ) FbxNormalElem->GetDirectArray().GetAt( j )[ 0 ] ;
									Mesh->Normals[ j ].y = ( float ) FbxNormalElem->GetDirectArray().GetAt( j )[ 1 ] ;
									Mesh->Normals[ j ].z = ( float )-FbxNormalElem->GetDirectArray().GetAt( j )[ 2 ] ;
								}
								break ;

							case FbxGeometryElement::eIndexToDirect :
								// インデックスの数を取得
								Mesh->NormalNum = FbxNormalElem->GetIndexArray().GetCount() ;

								// 法線情報を格納するメモリ領域の確保
								Mesh->Normals = ( VECTOR * )ADDMEMAREA( sizeof( VECTOR ) * Mesh->NormalNum, &RModel->Mem ) ;
								if( Mesh->Normals == NULL )
								{
									DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\xd5\x6c\xda\x7d\xc5\x60\x31\x58\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : 法線情報を格納するメモリの確保に失敗しました\n" @*/ )) ;
									return -1 ;
								}

								// インデックスモードの場合は間接参照代入
								for( j = 0 ; ( DWORD )j < Mesh->NormalNum ; j ++ )
								{
									Index = FbxNormalElem->GetIndexArray().GetAt( j ) ;
									Mesh->Normals[ j ].x = ( float ) FbxNormalElem->GetDirectArray().GetAt( Index )[ 0 ] ;
									Mesh->Normals[ j ].y = ( float ) FbxNormalElem->GetDirectArray().GetAt( Index )[ 1 ] ;
									Mesh->Normals[ j ].z = ( float )-FbxNormalElem->GetDirectArray().GetAt( Index )[ 2 ] ;
								}
								break ;

							default :
								DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\x5e\x97\xfe\x5b\xdc\x5f\x6e\x30\xd5\x6c\xda\x7d\xea\x30\xd5\x30\xa1\x30\xec\x30\xf3\x30\xb9\x30\xe2\x30\xfc\x30\xc9\x30\x4c\x30\x7f\x4f\x28\x75\x55\x30\x8c\x30\x66\x30\x44\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : 非対応の法線リファレンスモードが使用されていました\n" @*/ )) ;
								return -1 ;
							}

							// 面の各頂点に対応する法線の情報を取得
							switch( FbxMappingMode )
							{
							case FbxGeometryElement::eByControlPoint :
								// 法線インデックスは頂点インデックスと等しくなる
								MeshFace = Mesh->Faces ;
								for( j = 0 ; ( DWORD )j < Mesh->FaceNum ; j ++, MeshFace ++ )
								{
									MeshFace->NormalIndex[ 0 ] = MeshFace->VertexIndex[ 0 ] ;
									MeshFace->NormalIndex[ 1 ] = MeshFace->VertexIndex[ 1 ] ;
									MeshFace->NormalIndex[ 2 ] = MeshFace->VertexIndex[ 2 ] ;
									MeshFace->NormalIndex[ 3 ] = MeshFace->VertexIndex[ 3 ] ;
								}
								break ;

							case FbxGeometryElement::eByPolygonVertex :
								// 法線インデックスは別個で存在する
								MeshFace = Mesh->Faces ;
								k = 0 ;
								for( j = 0 ; ( DWORD )j < Mesh->FaceNum ; j ++, MeshFace ++ )
								{
									for( l = 0 ; ( DWORD )l < MeshFace->IndexNum ; l ++ )
									{
										MeshFace->NormalIndex[ l ] = k ;
										k ++ ;

										// 法泉の数が足りないバグデータの対応
										if( k == Mesh->NormalNum )
										{
											k = 0 ;
										}
									}
								}
								break ;

							default :
								DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\x5e\x97\xfe\x5b\xdc\x5f\x6e\x30\xd5\x6c\xda\x7d\xde\x30\xc3\x30\xd4\x30\xf3\x30\xb0\x30\xe2\x30\xfc\x30\xc9\x30\x4c\x30\x7f\x4f\x28\x75\x55\x30\x8c\x30\x66\x30\x44\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : 非対応の法線マッピングモードが使用されていました\n" @*/ )) ;
								return -1 ;
							}
						}

						// 従法線エレメントの数だけ繰り返し
						for( i = 0 ; i < _FbxMesh->GetElementBinormalCount() ; i ++ )
						{
							FbxBinormalElem = _FbxMesh->GetElementBinormal( i ) ;

							// モードの取得
							FbxRefMode     = FbxBinormalElem->GetReferenceMode() ;
							FbxMappingMode = FbxBinormalElem->GetMappingMode() ;

							switch( FbxRefMode )
							{
							case FbxGeometryElement::eDirect :
								// 従法線情報の数が法線情報の数と異なっていたら無視
								if( Mesh->NormalNum != FbxBinormalElem->GetDirectArray().GetCount() )
								{
									continue ;
								}

								// 従法線情報を格納するメモリ領域の確保
								Mesh->Binormals = ( VECTOR * )ADDMEMAREA( sizeof( VECTOR ) * Mesh->NormalNum, &RModel->Mem ) ;
								if( Mesh->Binormals == NULL )
								{
									DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\x93\x5f\xd5\x6c\xda\x7d\xc5\x60\x31\x58\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : 従法線情報を格納するメモリの確保に失敗しました\n" @*/ )) ;
									return -1 ;
								}

								// 直接モードの場合はそのまま代入
								for( j = 0 ; ( DWORD )j < Mesh->NormalNum ; j ++ )
								{
									Mesh->Binormals[ j ].x = ( float ) FbxBinormalElem->GetDirectArray().GetAt( j )[ 0 ] ;
									Mesh->Binormals[ j ].y = ( float ) FbxBinormalElem->GetDirectArray().GetAt( j )[ 1 ] ;
									Mesh->Binormals[ j ].z = ( float )-FbxBinormalElem->GetDirectArray().GetAt( j )[ 2 ] ;
								}
								break ;

							case FbxGeometryElement::eIndexToDirect :
								// インデックスの数が法線情報の数と異なっていたら無視
								if( Mesh->NormalNum != FbxBinormalElem->GetIndexArray().GetCount() )
								{
									continue ;
								}

								// 従法線情報を格納するメモリ領域の確保
								Mesh->Binormals = ( VECTOR * )ADDMEMAREA( sizeof( VECTOR ) * Mesh->NormalNum, &RModel->Mem ) ;
								if( Mesh->Binormals == NULL )
								{
									DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\x93\x5f\xd5\x6c\xda\x7d\xc5\x60\x31\x58\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : 従法線情報を格納するメモリの確保に失敗しました\n" @*/ )) ;
									return -1 ;
								}

								// インデックスモードの場合は間接参照代入
								for( j = 0 ; ( DWORD )j < Mesh->NormalNum ; j ++ )
								{
									Index = FbxBinormalElem->GetIndexArray().GetAt( j ) ;
									Mesh->Binormals[ j ].x = ( float ) FbxBinormalElem->GetDirectArray().GetAt( Index )[ 0 ] ;
									Mesh->Binormals[ j ].y = ( float ) FbxBinormalElem->GetDirectArray().GetAt( Index )[ 1 ] ;
									Mesh->Binormals[ j ].z = ( float )-FbxBinormalElem->GetDirectArray().GetAt( Index )[ 2 ] ;
								}
								break ;

							default :
								DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\x5e\x97\xfe\x5b\xdc\x5f\x6e\x30\x93\x5f\xd5\x6c\xda\x7d\xea\x30\xd5\x30\xa1\x30\xec\x30\xf3\x30\xb9\x30\xe2\x30\xfc\x30\xc9\x30\x4c\x30\x7f\x4f\x28\x75\x55\x30\x8c\x30\x66\x30\x44\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : 非対応の従法線リファレンスモードが使用されていました\n" @*/ )) ;
								return -1 ;
							}
						}

						// 接線エレメントの数だけ繰り返し
						for( i = 0 ; i < _FbxMesh->GetElementTangentCount() ; i ++ )
						{
							FbxTangentElem = _FbxMesh->GetElementTangent( i ) ;

							// モードの取得
							FbxRefMode     = FbxTangentElem->GetReferenceMode() ;
							FbxMappingMode = FbxTangentElem->GetMappingMode() ;

							switch( FbxRefMode )
							{
							case FbxGeometryElement::eDirect :
								// 接線情報の数が法線情報の数と異なっていたら無視
								if( Mesh->NormalNum != FbxTangentElem->GetDirectArray().GetCount() )
								{
									continue ;
								}

								// 接線情報を格納するメモリ領域の確保
								Mesh->Tangents = ( VECTOR * )ADDMEMAREA( sizeof( VECTOR ) * Mesh->NormalNum, &RModel->Mem ) ;
								if( Mesh->Tangents == NULL )
								{
									DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\xa5\x63\xda\x7d\xc5\x60\x31\x58\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : 接線情報を格納するメモリの確保に失敗しました\n" @*/ )) ;
									return -1 ;
								}

								// 直接モードの場合はそのまま代入
								for( j = 0 ; ( DWORD )j < Mesh->NormalNum ; j ++ )
								{
									Mesh->Tangents[ j ].x = ( float ) FbxTangentElem->GetDirectArray().GetAt( j )[ 0 ] ;
									Mesh->Tangents[ j ].y = ( float ) FbxTangentElem->GetDirectArray().GetAt( j )[ 1 ] ;
									Mesh->Tangents[ j ].z = ( float )-FbxTangentElem->GetDirectArray().GetAt( j )[ 2 ] ;
								}
								break ;

							case FbxGeometryElement::eIndexToDirect :
								// インデックスの数が法線情報の数と異なっていたら無視
								if( Mesh->NormalNum != FbxTangentElem->GetIndexArray().GetCount() )
								{
									continue ;
								}

								// 接線情報を格納するメモリ領域の確保
								Mesh->Tangents = ( VECTOR * )ADDMEMAREA( sizeof( VECTOR ) * Mesh->NormalNum, &RModel->Mem ) ;
								if( Mesh->Tangents == NULL )
								{
									DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\xa5\x63\xda\x7d\xc5\x60\x31\x58\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : 接線情報を格納するメモリの確保に失敗しました\n" @*/ )) ;
									return -1 ;
								}

								// インデックスモードの場合は間接参照代入
								for( j = 0 ; ( DWORD )j < Mesh->NormalNum ; j ++ )
								{
									Index = FbxTangentElem->GetIndexArray().GetAt( j ) ;
									Mesh->Tangents[ j ].x = ( float ) FbxTangentElem->GetDirectArray().GetAt( Index )[ 0 ] ;
									Mesh->Tangents[ j ].y = ( float ) FbxTangentElem->GetDirectArray().GetAt( Index )[ 1 ] ;
									Mesh->Tangents[ j ].z = ( float )-FbxTangentElem->GetDirectArray().GetAt( Index )[ 2 ] ;
								}
								break ;

							default :
								DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\x5e\x97\xfe\x5b\xdc\x5f\x6e\x30\xa5\x63\xda\x7d\xea\x30\xd5\x30\xa1\x30\xec\x30\xf3\x30\xb9\x30\xe2\x30\xfc\x30\xc9\x30\x4c\x30\x7f\x4f\x28\x75\x55\x30\x8c\x30\x66\x30\x44\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : 非対応の接線リファレンスモードが使用されていました\n" @*/ )) ;
								return -1 ;
							}
						}
					}

					// 頂点カラーの情報を取得
					{
						// 頂点カラーエレメントの数だけ繰り返し
						for( i = 0 ; i < _FbxMesh->GetElementVertexColorCount() ; i ++ )
						{
							FbxVertexColorElem = _FbxMesh->GetElementVertexColor( i ) ;

							// モードの取得
							FbxMappingMode = FbxVertexColorElem->GetMappingMode() ;
							FbxRefMode = FbxVertexColorElem->GetReferenceMode() ;

							// 頂点カラー情報の取得
							switch( FbxRefMode )
							{
							case FbxGeometryElement::eDirect :
								// 頂点カラー情報の数を取得
								Mesh->VertexColorNum = FbxVertexColorElem->GetDirectArray().GetCount() ;

								// 頂点カラー情報を格納するメモリ領域の確保
								Mesh->VertexColors = ( COLOR_F * )ADDMEMAREA( sizeof( COLOR_F ) * Mesh->VertexColorNum, &RModel->Mem ) ;
								if( Mesh->VertexColors == NULL )
								{
									DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\x02\x98\xb9\x70\xab\x30\xe9\x30\xfc\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : 頂点カラーを格納するメモリの確保に失敗しました\n" @*/ )) ;
									return -1 ;
								}

								// 直接モードの場合はそのまま代入
								for( j = 0 ; ( DWORD )j < Mesh->VertexColorNum ; j ++ )
								{
									Mesh->VertexColors[ j ].r = ( float )FbxVertexColorElem->GetDirectArray().GetAt( j ).mRed ;
									Mesh->VertexColors[ j ].g = ( float )FbxVertexColorElem->GetDirectArray().GetAt( j ).mGreen ;
									Mesh->VertexColors[ j ].b = ( float )FbxVertexColorElem->GetDirectArray().GetAt( j ).mBlue ;
									Mesh->VertexColors[ j ].a = ( float )FbxVertexColorElem->GetDirectArray().GetAt( j ).mAlpha ;
								}
								break ;

							case FbxGeometryElement::eIndexToDirect :
								// インデックスの数を取得
								Mesh->VertexColorNum = FbxVertexColorElem->GetIndexArray().GetCount() ;

								// 頂点カラー情報を格納するメモリ領域の確保
								Mesh->VertexColors = ( COLOR_F * )ADDMEMAREA( sizeof( COLOR_F ) * Mesh->VertexColorNum, &RModel->Mem ) ;
								if( Mesh->VertexColors == NULL )
								{
									DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\x02\x98\xb9\x70\xab\x30\xe9\x30\xfc\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : 頂点カラーを格納するメモリの確保に失敗しました\n" @*/ )) ;
									return -1 ;
								}

								// インデックスモードの場合は間接参照代入
								for( j = 0 ; ( DWORD )j < Mesh->VertexColorNum ; j ++ )
								{
									Index = FbxVertexColorElem->GetIndexArray().GetAt( j ) ;
									Mesh->VertexColors[ j ].r = ( float )FbxVertexColorElem->GetDirectArray().GetAt( Index ).mRed ;
									Mesh->VertexColors[ j ].g = ( float )FbxVertexColorElem->GetDirectArray().GetAt( Index ).mGreen ;
									Mesh->VertexColors[ j ].b = ( float )FbxVertexColorElem->GetDirectArray().GetAt( Index ).mBlue ;
									Mesh->VertexColors[ j ].a = ( float )FbxVertexColorElem->GetDirectArray().GetAt( Index ).mAlpha ;
								}
								break ;

							default :
								DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\x5e\x97\xfe\x5b\xdc\x5f\x6e\x30\x02\x98\xb9\x70\xab\x30\xe9\x30\xfc\x30\xea\x30\xd5\x30\xa1\x30\xec\x30\xf3\x30\xb9\x30\xe2\x30\xfc\x30\xc9\x30\x4c\x30\x7f\x4f\x28\x75\x55\x30\x8c\x30\x66\x30\x44\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : 非対応の頂点カラーリファレンスモードが使用されていました\n" @*/ )) ;
								return -1 ;
							}

							// 面の各頂点に対応する頂点カラーの情報を取得
							switch( FbxMappingMode )
							{
							case FbxGeometryElement::eByControlPoint :
								// 頂点カラーインデックスは頂点インデックスと等しくなる
								MeshFace = Mesh->Faces ;
								for( j = 0 ; ( DWORD )j < Mesh->FaceNum ; j ++, MeshFace ++ )
								{
									MeshFace->VertexColorIndex[ 0 ] = MeshFace->VertexIndex[ 0 ] ;
									MeshFace->VertexColorIndex[ 1 ] = MeshFace->VertexIndex[ 1 ] ;
									MeshFace->VertexColorIndex[ 2 ] = MeshFace->VertexIndex[ 2 ] ;
									MeshFace->VertexColorIndex[ 3 ] = MeshFace->VertexIndex[ 3 ] ;
								}
								break ;

							case FbxGeometryElement::eByPolygonVertex :
								// 頂点カラーインデックスは別個で存在する
								MeshFace = Mesh->Faces ;
								k = 0 ;
								for( j = 0 ; ( DWORD )j < Mesh->FaceNum ; j ++, MeshFace ++ )
								{
									for( l = 0 ; ( DWORD )l < MeshFace->IndexNum ; l ++, k ++ )
										MeshFace->VertexColorIndex[ l ] = k ;
								}
								break ;

							default :
								DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\x5e\x97\xfe\x5b\xdc\x5f\x6e\x30\x02\x98\xb9\x70\xab\x30\xe9\x30\xfc\x30\xde\x30\xc3\x30\xd4\x30\xf3\x30\xb0\x30\xe2\x30\xfc\x30\xc9\x30\x4c\x30\x7f\x4f\x28\x75\x55\x30\x8c\x30\x66\x30\x44\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : 非対応の頂点カラーマッピングモードが使用されていました\n" @*/ )) ;
								return -1 ;
							}
						}
					}

					// ＵＶの情報を取得
					{
						// ＵＶエレメントの数だけ繰り返し
						int lUVCount = _FbxMesh->GetElementUVCount() ;
						if( lUVCount > MV1_READ_MAX_UV_NUM )
						{
							lUVCount = MV1_READ_MAX_UV_NUM ;
						}
						for( i = 0 ; i < lUVCount ; i ++ )
						{
							FbxUVElem = _FbxMesh->GetElementUV( i ) ;

							// モードの取得
							FbxMappingMode = FbxUVElem->GetMappingMode() ;
							FbxRefMode = FbxUVElem->GetReferenceMode() ;

							// ＵＶ情報の取得
							switch( FbxRefMode )
							{
							case FbxGeometryElement::eDirect :
								// ＵＶ情報の数を取得
								Mesh->UVNum[ i ] = FbxUVElem->GetDirectArray().GetCount() ;

								// ＵＶ情報を格納するメモリ領域の確保
								Mesh->UVs[ i ] = ( FLOAT4 * )ADDMEMAREA( sizeof( FLOAT4 ) * Mesh->UVNum[ i ], &RModel->Mem ) ;
								if( Mesh->UVs[ i ] == NULL )
								{
									DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\x55\x00\x56\x00\xa7\x5e\x19\x6a\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : UV座標を格納するメモリの確保に失敗しました\n" @*/ )) ;
									return -1 ;
								}

								// 直接モードの場合はそのまま代入
								for( j = 0 ; ( DWORD )j < Mesh->UVNum[ i ] ; j ++ )
								{
									Mesh->UVs[ i ][ j ].x = ( float )FbxUVElem->GetDirectArray().GetAt( j )[ 0 ] ;
									Mesh->UVs[ i ][ j ].y = 1.0f - ( float )FbxUVElem->GetDirectArray().GetAt( j )[ 1 ] ;
								}
								break ;

							case FbxGeometryElement::eIndexToDirect :
								// インデックスの数を取得
								Mesh->UVNum[ i ] = FbxUVElem->GetIndexArray().GetCount() ;

								// ＵＶ情報を格納するメモリ領域の確保
								Mesh->UVs[ i ] = ( FLOAT4 * )ADDMEMAREA( sizeof( FLOAT4 ) * Mesh->UVNum[ i ], &RModel->Mem ) ;
								if( Mesh->UVs[ i ] == NULL )
								{
									DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\x55\x00\x56\x00\xa7\x5e\x19\x6a\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : UV座標を格納するメモリの確保に失敗しました\n" @*/ )) ;
									return -1 ;
								}

								// インデックスモードの場合は間接参照代入
								for( j = 0 ; ( DWORD )j < Mesh->UVNum[ i ] ; j ++ )
								{
									Index = FbxUVElem->GetIndexArray().GetAt( j ) ;
									Mesh->UVs[ i ][ j ].x = ( float )FbxUVElem->GetDirectArray().GetAt( Index )[ 0 ] ;
									Mesh->UVs[ i ][ j ].y = 1.0f - ( float )FbxUVElem->GetDirectArray().GetAt( Index )[ 1 ] ;
								}
								break ;

							default :
								DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\x5e\x97\xfe\x5b\xdc\x5f\x6e\x30\x35\xff\x36\xff\xea\x30\xd5\x30\xa1\x30\xec\x30\xf3\x30\xb9\x30\xe2\x30\xfc\x30\xc9\x30\x4c\x30\x7f\x4f\x28\x75\x55\x30\x8c\x30\x66\x30\x44\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : 非対応のＵＶリファレンスモードが使用されていました\n" @*/ )) ;
								return -1 ;
							}

							// 面の各頂点に対応するＵＶの情報を取得
							switch( FbxMappingMode )
							{
							case FbxGeometryElement::eByControlPoint :
								// ＵＶインデックスは頂点インデックスと等しくなる
								MeshFace = Mesh->Faces ;
								for( j = 0 ; ( DWORD )j < Mesh->FaceNum ; j ++, MeshFace ++ )
								{
									MeshFace->UVIndex[ i ][ 0 ] = MeshFace->VertexIndex[ 0 ] ;
									MeshFace->UVIndex[ i ][ 1 ] = MeshFace->VertexIndex[ 1 ] ;
									MeshFace->UVIndex[ i ][ 2 ] = MeshFace->VertexIndex[ 2 ] ;
									MeshFace->UVIndex[ i ][ 3 ] = MeshFace->VertexIndex[ 3 ] ;
								}
								break ;

							case FbxGeometryElement::eByPolygonVertex :
								// ＵＶインデックスは別個で存在する
								MeshFace = Mesh->Faces ;
								k = 0 ;
								for( j = 0 ; ( DWORD )j < Mesh->FaceNum ; j ++, MeshFace ++ )
								{
									for( l = 0 ; ( DWORD )l < MeshFace->IndexNum ; l ++, k ++ )
										MeshFace->UVIndex[ i ][ l ] = k ;
								}
								break ;

							default :
								DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\x5e\x97\xfe\x5b\xdc\x5f\x6e\x30\x35\xff\x36\xff\xde\x30\xc3\x30\xd4\x30\xf3\x30\xb0\x30\xe2\x30\xfc\x30\xc9\x30\x4c\x30\x7f\x4f\x28\x75\x55\x30\x8c\x30\x66\x30\x44\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : 非対応のＵＶマッピングモードが使用されていました\n" @*/ )) ;
								return -1 ;
							}
						}
					}

					// マテリアルの情報を取得
					{
						// マテリアルの数を取得
						Mesh->MaterialNum = pFbxNode->GetMaterialCount() ;

						// マテリアルがある場合のみ処理
						if( Mesh->MaterialNum != 0 )
						{
							// マテリアルの数だけ繰り返し
							for( i = 0 ; ( DWORD )i < Mesh->MaterialNum ; i ++ )
							{
								// マテリアルのアドレスを取得
								FbxMaterial = pFbxNode->GetMaterial( i ) ;

								// モデル上に既にこのマテリアルの情報がある場合はアドレスだけ保存する
								Material = RModel->MaterialFirst ;
								for( j = 0 ; ( DWORD )j < RModel->MaterialNum && Material->UserData != FbxMaterial ; j ++, Material = Material->DataNext ){}
								if( j != RModel->MaterialNum )
								{
									Mesh->Materials[ i ] = Material ;
								}
								else
								{
									// マテリアルの追加
	//								FbxUTF8ToAnsi( FbxMaterial->GetName(), ANSIBuffer, &ANSISize ) ;
									Material = MV1RAddMaterial( RModel, FbxMaterial->GetName() ) ;
									if( Material == NULL )
									{
										DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\xde\x30\xc6\x30\xea\x30\xa2\x30\xeb\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\xfd\x8f\xa0\x52\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : マテリアルオブジェクトの追加に失敗しました\n" @*/ )) ;
										return -1 ;
									}
									Material->SubName = FbxMaterial->GetName() ;

									Mesh->Materials[ i ] = Material ;

									// ＦＢＸのアドレスを保存
									Material->UserData = FbxMaterial ;

									// マテリアルのタイプがランバートでもフォンでも無い場合は分岐
									if( FbxMaterial->GetClassId().Is( FbxSurfaceLambert::ClassId ) ||
										FbxMaterial->GetClassId().Is( FbxSurfacePhong::ClassId ) )
									{
										// ランバートの情報を取得する
										FbxLambert = ( FbxSurfaceLambert * )FbxMaterial ;

										// 基本的な情報を取得

										// アンビエントカラー
										Material->Ambient.r = ( float )( FbxLambert->Ambient.Get()[ 0 ] * FbxLambert->AmbientFactor.Get() ) ;
										Material->Ambient.g = ( float )( FbxLambert->Ambient.Get()[ 1 ] * FbxLambert->AmbientFactor.Get() ) ;
										Material->Ambient.b = ( float )( FbxLambert->Ambient.Get()[ 2 ] * FbxLambert->AmbientFactor.Get() ) ;
										Material->Ambient.a = 0.0f ;

										// ディフューズカラー
										Material->Diffuse.r = ( float )( FbxLambert->Diffuse.Get()[ 0 ] * FbxLambert->DiffuseFactor.Get() ) ;
										Material->Diffuse.g = ( float )( FbxLambert->Diffuse.Get()[ 1 ] * FbxLambert->DiffuseFactor.Get() ) ;
										Material->Diffuse.b = ( float )( FbxLambert->Diffuse.Get()[ 2 ] * FbxLambert->DiffuseFactor.Get() ) ;
										Material->Diffuse.a = 1.0f ;

										// エミッシブカラー
										Material->Emissive.r = ( float )( FbxLambert->Emissive.Get()[ 0 ] * FbxLambert->EmissiveFactor.Get() ) ;
										Material->Emissive.g = ( float )( FbxLambert->Emissive.Get()[ 1 ] * FbxLambert->EmissiveFactor.Get() ) ;
										Material->Emissive.b = ( float )( FbxLambert->Emissive.Get()[ 2 ] * FbxLambert->EmissiveFactor.Get() ) ;
										Material->Emissive.a = 0.0f ;

										// アルファ値を保存
		//								Material->Diffuse.a = 1.0f - FbxLambert->TransparencyFactor.Get() ;

										// フォンマテリアルの場合はフォンマテリアルの情報も取得する
										if( FbxMaterial->GetClassId().Is( FbxSurfacePhong::ClassId ) )
										{
											FbxPhong = ( FbxSurfacePhong * )FbxMaterial ;

											// スペキュラカラー
											Material->Specular.r = ( float )( FbxPhong->Specular.Get()[ 0 ] * FbxPhong->SpecularFactor.Get() ) ;
											Material->Specular.g = ( float )( FbxPhong->Specular.Get()[ 1 ] * FbxPhong->SpecularFactor.Get() ) ;
											Material->Specular.b = ( float )( FbxPhong->Specular.Get()[ 2 ] * FbxPhong->SpecularFactor.Get() ) ;
											Material->Specular.a = 0.0f ;

											// 光沢
		//									Material->Shininess = FbxPhong->Shininess.Get() ;

											// 反射
		//									Material->Reflection = FbxPhong->ReflectionFactor.Get() ;
											//Material->Power = ( float )FbxPhong->ReflectionFactor.Get() ;
											Material->Power = ( float )FbxPhong->Shininess.Get() ;
										}
									}
									else
									{
										// アンビエントカラー
										Material->Ambient.r = 0.25f ;
										Material->Ambient.g = 0.25f ;
										Material->Ambient.b = 0.25f ;
										Material->Ambient.a = 0.0f ;

										// ディフューズカラー
										Material->Diffuse.r = 1.0f ;
										Material->Diffuse.g = 1.0f ;
										Material->Diffuse.b = 1.0f ;
										Material->Diffuse.a = 1.0f ;

										// エミッシブカラー
										Material->Emissive.r = 0.0f ;
										Material->Emissive.g = 0.0f ;
										Material->Emissive.b = 0.0f ;
										Material->Emissive.a = 0.0f ;

	//									DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\x5e\x97\xfe\x5b\xdc\x5f\x6e\x30\xde\x30\xc6\x30\xea\x30\xa2\x30\xeb\x30\xe2\x30\xfc\x30\xc9\x30\x4c\x30\x7f\x4f\x28\x75\x55\x30\x8c\x30\x66\x30\x44\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : 非対応のマテリアルモードが使用されていました\n" @*/ )) ;
	//									return -1 ;
									}

									// 使用しているテクスチャの情報を取得する
									{
										FbxProperty _FbxProperty ;
										FbxLayeredTexture::EBlendMode BlendMode ;
										int LayeredTexNum, NormalTexNum ;

										// ディフューズマテリアルプロパティの取得
										{
											_FbxProperty = FbxMaterial->FindProperty( FbxSurfaceMaterial::sDiffuse ) ;

											// レイヤードテクスチャの場合とそれ以外で処理を分岐
											if( _FbxProperty.GetSrcObject< FbxLayeredTexture >( 0 ) != NULL )
											{
												// ２個以上のレイヤーには対応していない
												if( _FbxProperty.GetSrcObject< FbxLayeredTexture >( 1 ) != NULL )
												{
													DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\x44\x00\x69\x00\x66\x00\x66\x00\x75\x00\x73\x00\x65\x00\x20\x00\xde\x30\xc6\x30\xea\x30\xa2\x30\xeb\x30\x6f\x30\x11\xff\xec\x30\xa4\x30\xe4\x30\xfc\x30\xe5\x4e\x0a\x4e\x6b\x30\x6f\x30\xfe\x5b\xdc\x5f\x57\x30\x66\x30\x44\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"Fbx Load : Diffuse マテリアルは１レイヤー以上には対応していません\n" @*/ )) ;
													return -1 ;
												}

												// テクスチャの数だけ繰り返し
												LayeredTexNum = 1 ;
												for( j = 0 ; j < LayeredTexNum ; j ++ )
												{
													// テクスチャのアドレスを取得
													_FbxLayeredTexture = _FbxProperty.GetSrcObject< FbxLayeredTexture >( j ) ;

													// レイヤーの中に含まれているテクスチャの数を取得する
													NormalTexNum = _FbxLayeredTexture->GetSrcObjectCount< FbxTexture >() ;

													// テクスチャの数だけ繰り返し
													for( k = 0 ; k < NormalTexNum ; k ++ )
													{
														_FbxTexture = _FbxLayeredTexture->GetSrcObject< FbxTexture >( k ) ;
														if( _FbxTexture == NULL ) continue ;

														// モデルに追加
														Material->DiffuseTexs[ Material->DiffuseTexNum ] = FbxAddTexture( RModel, _FbxTexture ) ;
														if( Material->DiffuseTexs[ Material->DiffuseTexNum ] == NULL ) 
														{
															DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\x44\x00\x69\x00\x66\x00\x66\x00\x75\x00\x73\x00\x65\x00\x20\x00\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\xfd\x8f\xa0\x52\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : Diffuse テクスチャオブジェクトの追加に失敗しました\n" @*/ )) ;
															return -1 ;
														}

														// 合成方法を取得する
														_FbxLayeredTexture->GetTextureBlendMode( k, BlendMode ) ;
														switch( BlendMode )
														{
														case FbxLayeredTexture::eTranslucent : Material->DiffuseTexs[ Material->DiffuseTexNum ]->BlendType = MV1_LAYERBLEND_TYPE_TRANSLUCENT ; break ;
														case FbxLayeredTexture::eAdditive :    Material->DiffuseTexs[ Material->DiffuseTexNum ]->BlendType = MV1_LAYERBLEND_TYPE_ADDITIVE ;    break ;
														case FbxLayeredTexture::eModulate :    Material->DiffuseTexs[ Material->DiffuseTexNum ]->BlendType = MV1_LAYERBLEND_TYPE_MODULATE ;    break ;
														case FbxLayeredTexture::eModulate2 :   Material->DiffuseTexs[ Material->DiffuseTexNum ]->BlendType = MV1_LAYERBLEND_TYPE_MODULATE2 ;   break ;
														}

														// テクスチャの数をインクリメント
														Material->DiffuseTexNum ++ ;
													}
												}
											}
											else
											{
												// 通常のテクスチャの数を取得
												Material->DiffuseTexNum = _FbxProperty.GetSrcObjectCount< FbxTexture >() ;

												// 使用している場合は処理
												if( Material->DiffuseTexNum != 0 )
												{
													// テクスチャの数だけ繰り返し
													for( j = 0 ; j < Material->DiffuseTexNum ; j ++ )
													{
														// テクスチャのアドレスを取得
														_FbxTexture = _FbxProperty.GetSrcObject< FbxTexture >( j ) ;

														// モデルに追加
														Material->DiffuseTexs[ j ] = FbxAddTexture( RModel, _FbxTexture ) ;
														if( Material->DiffuseTexs[ j ] == NULL ) 
														{
															DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\x44\x00\x69\x00\x66\x00\x66\x00\x75\x00\x73\x00\x65\x00\x20\x00\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\xfd\x8f\xa0\x52\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : Diffuse テクスチャオブジェクトの追加に失敗しました\n" @*/ )) ;
															return -1 ;
														}
													}
												}
											}
										}

										// スペキュラマテリアルプロパティの取得
										{
											_FbxProperty = FbxMaterial->FindProperty( FbxSurfaceMaterial::sSpecular ) ;

											// レイヤードテクスチャの場合とそれ以外で処理を分岐
											LayeredTexNum = _FbxProperty.GetSrcObjectCount< FbxLayeredTexture >() ;
											if( LayeredTexNum )
											{
												// ２個以上のレイヤーには対応していない
												if( LayeredTexNum > 1 )
												{
													DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\x53\x00\x70\x00\x65\x00\x63\x00\x75\x00\x6c\x00\x61\x00\x72\x00\x20\x00\xde\x30\xc6\x30\xea\x30\xa2\x30\xeb\x30\x6f\x30\x11\xff\xec\x30\xa4\x30\xe4\x30\xfc\x30\xe5\x4e\x0a\x4e\x6b\x30\x6f\x30\xfe\x5b\xdc\x5f\x57\x30\x66\x30\x44\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"Fbx Load : Specular マテリアルは１レイヤー以上には対応していません\n" @*/ )) ;
													return -1 ;
												}

												// テクスチャの数だけ繰り返し
												for( j = 0 ; j < LayeredTexNum ; j ++ )
												{
													// テクスチャのアドレスを取得
													_FbxLayeredTexture = _FbxProperty.GetSrcObject< FbxLayeredTexture >( j ) ;

													// レイヤーの中に含まれているテクスチャの数を取得する
													NormalTexNum = _FbxLayeredTexture->GetSrcObjectCount< FbxTexture >() ;

													// テクスチャの数だけ繰り返し
													for( k = 0 ; k < NormalTexNum ; k ++ )
													{
														_FbxTexture = _FbxLayeredTexture->GetSrcObject< FbxTexture >( k ) ;
														if( _FbxTexture == NULL ) continue ;

														// モデルに追加
														Material->SpecularTexs[ Material->SpecularTexNum ] = FbxAddTexture( RModel, _FbxTexture ) ;
														if( Material->SpecularTexs[ Material->SpecularTexNum ] == NULL ) 
														{
															DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\x53\x00\x70\x00\x65\x00\x63\x00\x75\x00\x6c\x00\x61\x00\x72\x00\x20\x00\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\xfd\x8f\xa0\x52\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : Specular テクスチャオブジェクトの追加に失敗しました\n" @*/ )) ;
															return -1 ;
														}

														// 合成方法を取得する
														_FbxLayeredTexture->GetTextureBlendMode( k, BlendMode ) ;
														switch( BlendMode )
														{
														case FbxLayeredTexture::eTranslucent : Material->SpecularTexs[ Material->SpecularTexNum ]->BlendType = MV1_LAYERBLEND_TYPE_TRANSLUCENT ; break ;
														case FbxLayeredTexture::eAdditive :    Material->SpecularTexs[ Material->SpecularTexNum ]->BlendType = MV1_LAYERBLEND_TYPE_ADDITIVE ;    break ;
														case FbxLayeredTexture::eModulate :    Material->SpecularTexs[ Material->SpecularTexNum ]->BlendType = MV1_LAYERBLEND_TYPE_MODULATE ;    break ;
														case FbxLayeredTexture::eModulate2 :   Material->SpecularTexs[ Material->SpecularTexNum ]->BlendType = MV1_LAYERBLEND_TYPE_MODULATE2 ;   break ;
														}

														// テクスチャの数をインクリメント
														Material->SpecularTexNum ++ ;
													}
												}
											}
											else
											{
												// 通常のテクスチャの数を取得
												Material->SpecularTexNum = _FbxProperty.GetSrcObjectCount< FbxTexture >() ;

												// 使用している場合は処理
												if( Material->SpecularTexNum != 0 )
												{
													// テクスチャの数だけ繰り返し
													for( j = 0 ; j < Material->SpecularTexNum ; j ++ )
													{
														// テクスチャのアドレスを取得
														_FbxTexture = _FbxProperty.GetSrcObject< FbxTexture >( j ) ;

														// モデルに追加
														Material->SpecularTexs[ j ] = FbxAddTexture( RModel, _FbxTexture ) ;
														if( Material->SpecularTexs[ j ] == NULL ) 
														{
															DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\x53\x00\x70\x00\x65\x00\x63\x00\x75\x00\x6c\x00\x61\x00\x72\x00\x20\x00\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\xfd\x8f\xa0\x52\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : Specular テクスチャオブジェクトの追加に失敗しました\n" @*/ )) ;
															return -1 ;
														}
													}
												}
											}
										}

										// 法線マップマテリアルプロパティの取得
										{
											_FbxProperty = FbxMaterial->FindProperty( FbxSurfaceMaterial::sNormalMap ) ;

											// レイヤードテクスチャの場合とそれ以外で処理を分岐
											LayeredTexNum = _FbxProperty.GetSrcObjectCount< FbxLayeredTexture >() ;
											if( LayeredTexNum )
											{
												// ２個以上のレイヤーには対応していない
												if( LayeredTexNum > 1 )
												{
													DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\x42\x00\x75\x00\x6d\x00\x70\x00\x20\x00\xde\x30\xc6\x30\xea\x30\xa2\x30\xeb\x30\x6f\x30\x11\xff\xec\x30\xa4\x30\xe4\x30\xfc\x30\xe5\x4e\x0a\x4e\x6b\x30\x6f\x30\xfe\x5b\xdc\x5f\x57\x30\x66\x30\x44\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"Fbx Load : Bump マテリアルは１レイヤー以上には対応していません\n" @*/ )) ;
													return -1 ;
												}

												// テクスチャの数だけ繰り返し
												for( j = 0 ; j < LayeredTexNum ; j ++ )
												{
													// テクスチャのアドレスを取得
													_FbxLayeredTexture = _FbxProperty.GetSrcObject< FbxLayeredTexture >( j ) ;

													// レイヤーの中に含まれているテクスチャの数を取得する
													NormalTexNum = _FbxLayeredTexture->GetSrcObjectCount< FbxTexture >() ;

													// テクスチャの数だけ繰り返し
													for( k = 0 ; k < NormalTexNum ; k ++ )
													{
														_FbxTexture = _FbxLayeredTexture->GetSrcObject< FbxTexture >( k ) ;
														if( _FbxTexture == NULL ) continue ;

														// モデルに追加
														Material->NormalTexs[ Material->NormalTexNum ] = FbxAddTexture( RModel, _FbxTexture ) ;
														if( Material->NormalTexs[ Material->NormalTexNum ] == NULL ) 
														{
															DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\x42\x00\x75\x00\x6d\x00\x70\x00\x20\x00\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\xfd\x8f\xa0\x52\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : Bump テクスチャオブジェクトの追加に失敗しました\n" @*/ )) ;
															return -1 ;
														}

														// 合成方法を取得する
														_FbxLayeredTexture->GetTextureBlendMode( k, BlendMode ) ;
														switch( BlendMode )
														{
														case FbxLayeredTexture::eTranslucent : Material->NormalTexs[ Material->NormalTexNum ]->BlendType = MV1_LAYERBLEND_TYPE_TRANSLUCENT ; break ;
														case FbxLayeredTexture::eAdditive :    Material->NormalTexs[ Material->NormalTexNum ]->BlendType = MV1_LAYERBLEND_TYPE_ADDITIVE ;    break ;
														case FbxLayeredTexture::eModulate :    Material->NormalTexs[ Material->NormalTexNum ]->BlendType = MV1_LAYERBLEND_TYPE_MODULATE ;    break ;
														case FbxLayeredTexture::eModulate2 :   Material->NormalTexs[ Material->NormalTexNum ]->BlendType = MV1_LAYERBLEND_TYPE_MODULATE2 ;   break ;
														}

														// テクスチャの数をインクリメント
														Material->NormalTexNum ++ ;
													}
												}
											}
											else
											{
												// 通常のテクスチャの数を取得
												Material->NormalTexNum = _FbxProperty.GetSrcObjectCount< FbxTexture >() ;

												// 使用している場合は処理
												if( Material->NormalTexNum != 0 )
												{
													// テクスチャの数だけ繰り返し
													for( j = 0 ; j < Material->NormalTexNum ; j ++ )
													{
														// テクスチャのアドレスを取得
														_FbxTexture = _FbxProperty.GetSrcObject< FbxTexture >( j ) ;

														// モデルに追加
														Material->NormalTexs[ j ] = FbxAddTexture( RModel, _FbxTexture ) ;
														if( Material->NormalTexs[ j ] == NULL ) 
														{
															DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\x42\x00\x75\x00\x6d\x00\x70\x00\x20\x00\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\xfd\x8f\xa0\x52\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : Bump テクスチャオブジェクトの追加に失敗しました\n" @*/ )) ;
															return -1 ;
														}
													}
												}
											}
										}
									}
								}
							}

							// 各面に対応するマテリアルの情報を取得する
							{
								// マテリアルエレメントの数だけ繰り返し
								for( i = 0 ; i < _FbxMesh->GetElementMaterialCount() ; i ++ )
								{
									FbxMaterialElem = _FbxMesh->GetElementMaterial( i ) ;

									// モードの取得
									FbxMappingMode = FbxMaterialElem->GetMappingMode() ;
									FbxRefMode = FbxMaterialElem->GetReferenceMode() ;

									// マテリアル情報の取得
									switch( FbxRefMode )
									{
									case FbxGeometryElement::eDirect :
										// マテリアル情報の数を取得
										MaterialNum = pFbxNode->GetMaterialCount() ;

										// マテリアル情報を格納するメモリ領域の確保
										FbxMaterialDim = ( FbxSurfaceMaterial ** )DXCALLOC( sizeof( FbxSurfaceMaterial * ) * MaterialNum ) ;
										if( FbxMaterialDim == NULL ) 
										{
											DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\xde\x30\xc6\x30\xea\x30\xa2\x30\xeb\x30\xea\x30\xb9\x30\xc8\x30\x92\x30\x00\x4e\x42\x66\xdd\x4f\x58\x5b\x59\x30\x8b\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : マテリアルリストを一時保存するバッファの確保に失敗しました\n" @*/ )) ;
											return -1 ;
										}

										// 直接モードの場合はそのまま代入
										for( j = 0 ; j < MaterialNum ; j ++ )
										{
											FbxMaterialDim[ j ] = pFbxNode->GetMaterial( j ) ;
										}
										break ;

									case FbxGeometryElement::eIndexToDirect :
										// インデックスの数を取得
										MaterialNum = FbxMaterialElem->GetIndexArray().GetCount() ;

										// マテリアル情報を格納するメモリ領域の確保
										FbxMaterialDim = ( FbxSurfaceMaterial ** )DXCALLOC( sizeof( FbxSurfaceMaterial * ) * MaterialNum ) ;
										if( FbxMaterialDim == NULL ) 
										{
											DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\xde\x30\xc6\x30\xea\x30\xa2\x30\xeb\x30\xea\x30\xb9\x30\xc8\x30\x92\x30\x00\x4e\x42\x66\xdd\x4f\x58\x5b\x59\x30\x8b\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : マテリアルリストを一時保存するバッファの確保に失敗しました\n" @*/ )) ;
											return -1 ;
										}

										// インデックスモードの場合は間接参照代入
										for( j = 0 ; j < MaterialNum ; j ++ )
										{
											Index = FbxMaterialElem->GetIndexArray().GetAt( j ) ;
											FbxMaterialDim[ j ] = pFbxNode->GetMaterial( Index ) ;
										}
										break ;

									default :
										DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\x5e\x97\xfe\x5b\xdc\x5f\x6e\x30\xde\x30\xc6\x30\xea\x30\xa2\x30\xeb\x30\xea\x30\xd5\x30\xa1\x30\xec\x30\xf3\x30\xb9\x30\xe2\x30\xfc\x30\xc9\x30\x4c\x30\x7f\x4f\x28\x75\x55\x30\x8c\x30\x66\x30\x44\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : 非対応のマテリアルリファレンスモードが使用されていました\n" @*/ )) ;
										return -1 ;
									}

									// 面の各頂点に対応するマテリアルの情報を取得
									switch( FbxMappingMode )
									{
									case FbxGeometryElement::eByPolygon :
										// １ポリゴンに１マテリアル
										MeshFace = Mesh->Faces ;
										for( j = 0 ; ( DWORD )j < Mesh->FaceNum ; j ++, MeshFace ++ )
										{
											// 割り当てられているマテリアルがモデル中のどのマテリアルに当たるのかを調べる
											for( k = 0 ; ( DWORD )k < Mesh->MaterialNum && _STRCMP( Mesh->Materials[ k ]->SubName, FbxMaterialDim[ j ]->GetName() ) != 0 ; k ++ ){}
											if( k == Mesh->MaterialNum )
											{
	//											FbxUTF8ToAnsi( FbxMaterialDim[ j ]->GetName(), ANSIBuffer, &ANSISize ) ;
												ConvString( FbxMaterialDim[ j ]->GetName(), -1, DX_CHARCODEFORMAT_UTF8, ( char * )UTF16LE_Buffer, sizeof( UTF16LE_Buffer ), DX_CHARCODEFORMAT_UTF16LE ) ; 
												DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\xce\x30\xfc\x30\xc9\x30\x85\x51\x6b\x30\x00\x4e\xf4\x81\x59\x30\x8b\x30\xde\x30\xc6\x30\xea\x30\xa2\x30\xeb\x30\x4c\x30\x8b\x89\x64\x30\x4b\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x20\x00\x65\x00\x42\x00\x59\x00\x5f\x00\x43\x00\x4f\x00\x4e\x00\x54\x00\x52\x00\x4f\x00\x4c\x00\x5f\x00\x50\x00\x4f\x00\x49\x00\x4e\x00\x54\x00\x20\x00\x2c\x00\x20\x00\x25\x00\x73\x00\x20\x00\x0a\x00\x00"/*@ L"Fbx Load : ノード内に一致するマテリアルが見つかりませんでした eBY_CONTROL_POINT , %s \n" @*/, UTF16LE_Buffer )) ;
												DXFREE( FbxMaterialDim ) ;
												return -1 ;
											}

											// インデックスを保存
											MeshFace->MaterialIndex = k ;
										}
										break ;

									case FbxGeometryElement::eAllSame :
										// メッシュ全体で１マテリアル

										// 割り当てられているマテリアルがモデル中のどのマテリアルに当たるのかを調べる
										for( k = 0 ; ( DWORD )k < Mesh->MaterialNum && _STRCMP( Mesh->Materials[ k ]->SubName, FbxMaterialDim[ 0 ]->GetName() ) != 0 ; k ++ ){}
										if( k == Mesh->MaterialNum )
										{
											DXFREE( FbxMaterialDim ) ;
											DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\xce\x30\xfc\x30\xc9\x30\x85\x51\x6b\x30\x00\x4e\xf4\x81\x59\x30\x8b\x30\xde\x30\xc6\x30\xea\x30\xa2\x30\xeb\x30\x4c\x30\x8b\x89\x64\x30\x4b\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x20\x00\x65\x00\x41\x00\x4c\x00\x4c\x00\x5f\x00\x53\x00\x41\x00\x4d\x00\x45\x00\x0a\x00\x00"/*@ L"Fbx Load : ノード内に一致するマテリアルが見つかりませんでした eALL_SAME\n" @*/ )) ;
											return -1 ;
										}

										// すべてのメッシュに見つけたインデックスをセット
										MeshFace = Mesh->Faces ;
										for( j = 0 ; ( DWORD )j < Mesh->FaceNum ; j ++, MeshFace ++ )
										{
											// インデックスを保存
											MeshFace->MaterialIndex = k ;
										}
										break ;

									default :
										DXFREE( FbxMaterialDim ) ;
										DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\x5e\x97\xfe\x5b\xdc\x5f\x6e\x30\xde\x30\xc6\x30\xea\x30\xa2\x30\xeb\x30\xde\x30\xc3\x30\xd4\x30\xf3\x30\xb0\x30\xe2\x30\xfc\x30\xc9\x30\x4c\x30\x7f\x4f\x28\x75\x55\x30\x8c\x30\x66\x30\x44\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : 非対応のマテリアルマッピングモードが使用されていました\n" @*/ )) ;
										return -1 ;
									}

									DXFREE( FbxMaterialDim ) ;
								}
							}
						}
					}

					// スキンメッシュの情報を取得
					if( _FbxMesh->GetDeformerCount( FbxDeformer::eSkin ) )
					{
						int PointNum, ClusterCount ;
						int *Point ;
						double *Weight ;
						FbxAMatrix _FbxMatrix ;
						MATRIX InvMatrix ;
						BYTE *PositionFillFlag ;

						// ２個以上のスキンには未対応
						if( _FbxMesh->GetDeformerCount( FbxDeformer::eSkin ) > 1 )
						{
							DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\x11\xff\xe1\x30\xc3\x30\xb7\x30\xe5\x30\x6b\x30\x07\x89\x70\x65\x6e\x30\xb9\x30\xad\x30\xf3\x30\xe1\x30\xc3\x30\xb7\x30\xe5\x30\x6b\x30\x6f\x30\xfe\x5b\xdc\x5f\x57\x30\x66\x30\x44\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"Fbx Load : １メッシュに複数のスキンメッシュには対応していません\n" @*/ )) ;
							return -1 ;
						}

						// スキンメッシュ情報の取得
						_FbxSkin = ( FbxSkin * )_FbxMesh->GetDeformer( 0, FbxDeformer::eSkin ) ;

						// クラスタの数を取得
						ClusterCount = _FbxSkin->GetClusterCount() ;

						// 各頂点にウエイト値が設定されたかどうかを確認するためのフラグを格納するためのメモリ領域を確保する
						PositionFillFlag = ( BYTE * )ADDMEMAREA( ( Mesh->PositionNum + 31 ) / 32 * 4, &RModel->Mem ) ;
						if( PositionFillFlag == NULL )
						{
							DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\xa7\x5e\x19\x6a\x6b\x30\xfe\x5b\x59\x30\x8b\x30\xb9\x30\xad\x30\xf3\x30\xa6\x30\xa8\x30\xa4\x30\xc8\x30\x4c\x30\x58\x5b\x28\x57\x59\x30\x8b\x30\x4b\x30\x69\x30\x46\x30\x4b\x30\x92\x30\xba\x78\x8d\x8a\x59\x30\x8b\x30\x5f\x30\x81\x30\x6e\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : 座標に対するスキンウエイトが存在するかどうかを確認するためのメモリ領域の確保に失敗しました\n" @*/ )) ;
							return -1 ;
						}
						_MEMSET( PositionFillFlag, 0, ( Mesh->PositionNum + 31 ) / 32 * 4 ) ;

						// 有効な情報があるクラスタを取得する
						for( i = 0 ; i < ClusterCount ; i ++ )
						{
							FbxAMatrix FbxTransMatrix ;
							MATRIX TransMatrix ;

							_FbxCluster = _FbxSkin->GetCluster( i ) ;
							if( _FbxCluster->GetControlPointIndicesCount() == 0 ) continue ;

							// スキンウエイト情報の追加
							SkinWeight = MV1RAddSkinWeight( RModel ) ;
							Mesh->SkinWeights[ Mesh->SkinWeightsNum ] = SkinWeight ;
							Mesh->SkinWeights[ Mesh->SkinWeightsNum ]->UserData = _FbxCluster ;
							Mesh->SkinWeightsNum ++ ;

							// 各頂点への影響情報を格納するためのメモリの確保
							SkinWeight->DataNum = _FbxCluster->GetControlPointIndicesCount() ;
							SkinWeight->Data = ( MV1_SKIN_WEIGHT_ONE_R * )ADDMEMAREA( sizeof( MV1_SKIN_WEIGHT_ONE_R ) * SkinWeight->DataNum, &RModel->Mem ) ;
							if( SkinWeight->Data == NULL )
							{
								DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\xb9\x30\xad\x30\xf3\x30\xa6\x30\xa8\x30\xa4\x30\xc8\x30\xc5\x60\x31\x58\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : スキンウエイト情報を格納するメモリの確保に失敗しました\n" @*/ )) ;
								return -1 ;
							}

							// 影響情報を取得する
							PointNum = _FbxCluster->GetControlPointIndicesCount() ;
							Point = _FbxCluster->GetControlPointIndices() ;
							Weight = _FbxCluster->GetControlPointWeights() ;
							for( j = 0 ; ( DWORD )j < SkinWeight->DataNum ; j ++ )
							{
								SkinWeight->Data[ j ].TargetVertex = Point[ j ] ;
								SkinWeight->Data[ j ].Weight = ( float )Weight[ j ] ;
								PositionFillFlag[ Point[ j ] / 8 ] |= 1 << ( Point[ j ] & 7 ) ;
							}

							// ボーンのローカル座標に落とし込むための行列の取得
							_FbxCluster->GetTransformLinkMatrix( _FbxMatrix ) ;
							_FbxCluster->GetTransformMatrix( FbxTransMatrix ) ;
							InvMatrix.m[ 0 ][ 0 ] = ( float )_FbxMatrix.Double44()[ 0 ][ 0 ] ;
							InvMatrix.m[ 0 ][ 1 ] = ( float )_FbxMatrix.Double44()[ 0 ][ 1 ] ;
							InvMatrix.m[ 0 ][ 2 ] = ( float )_FbxMatrix.Double44()[ 0 ][ 2 ] ;
							InvMatrix.m[ 0 ][ 3 ] = ( float )_FbxMatrix.Double44()[ 0 ][ 3 ] ;

							InvMatrix.m[ 1 ][ 0 ] = ( float )_FbxMatrix.Double44()[ 1 ][ 0 ] ;
							InvMatrix.m[ 1 ][ 1 ] = ( float )_FbxMatrix.Double44()[ 1 ][ 1 ] ;
							InvMatrix.m[ 1 ][ 2 ] = ( float )_FbxMatrix.Double44()[ 1 ][ 2 ] ;
							InvMatrix.m[ 1 ][ 3 ] = ( float )_FbxMatrix.Double44()[ 1 ][ 3 ] ;

							InvMatrix.m[ 2 ][ 0 ] = ( float )_FbxMatrix.Double44()[ 2 ][ 0 ] ;
							InvMatrix.m[ 2 ][ 1 ] = ( float )_FbxMatrix.Double44()[ 2 ][ 1 ] ;
							InvMatrix.m[ 2 ][ 2 ] = ( float )_FbxMatrix.Double44()[ 2 ][ 2 ] ;
							InvMatrix.m[ 2 ][ 3 ] = ( float )_FbxMatrix.Double44()[ 2 ][ 3 ] ;

							InvMatrix.m[ 3 ][ 0 ] = ( float )_FbxMatrix.Double44()[ 3 ][ 0 ] ;
							InvMatrix.m[ 3 ][ 1 ] = ( float )_FbxMatrix.Double44()[ 3 ][ 1 ] ;
							InvMatrix.m[ 3 ][ 2 ] = ( float )_FbxMatrix.Double44()[ 3 ][ 2 ] ;
							InvMatrix.m[ 3 ][ 3 ] = ( float )_FbxMatrix.Double44()[ 3 ][ 3 ] ;

							TransMatrix.m[ 0 ][ 0 ] = ( float )FbxTransMatrix.Double44()[ 0 ][ 0 ] ;
							TransMatrix.m[ 0 ][ 1 ] = ( float )FbxTransMatrix.Double44()[ 0 ][ 1 ] ;
							TransMatrix.m[ 0 ][ 2 ] = ( float )FbxTransMatrix.Double44()[ 0 ][ 2 ] ;
							TransMatrix.m[ 0 ][ 3 ] = ( float )FbxTransMatrix.Double44()[ 0 ][ 3 ] ;

							TransMatrix.m[ 1 ][ 0 ] = ( float )FbxTransMatrix.Double44()[ 1 ][ 0 ] ;
							TransMatrix.m[ 1 ][ 1 ] = ( float )FbxTransMatrix.Double44()[ 1 ][ 1 ] ;
							TransMatrix.m[ 1 ][ 2 ] = ( float )FbxTransMatrix.Double44()[ 1 ][ 2 ] ;
							TransMatrix.m[ 1 ][ 3 ] = ( float )FbxTransMatrix.Double44()[ 1 ][ 3 ] ;

							TransMatrix.m[ 2 ][ 0 ] = ( float )FbxTransMatrix.Double44()[ 2 ][ 0 ] ;
							TransMatrix.m[ 2 ][ 1 ] = ( float )FbxTransMatrix.Double44()[ 2 ][ 1 ] ;
							TransMatrix.m[ 2 ][ 2 ] = ( float )FbxTransMatrix.Double44()[ 2 ][ 2 ] ;
							TransMatrix.m[ 2 ][ 3 ] = ( float )FbxTransMatrix.Double44()[ 2 ][ 3 ] ;

							TransMatrix.m[ 3 ][ 0 ] = ( float )FbxTransMatrix.Double44()[ 3 ][ 0 ] ;
							TransMatrix.m[ 3 ][ 1 ] = ( float )FbxTransMatrix.Double44()[ 3 ][ 1 ] ;
							TransMatrix.m[ 3 ][ 2 ] = ( float )FbxTransMatrix.Double44()[ 3 ][ 2 ] ;
							TransMatrix.m[ 3 ][ 3 ] = ( float )FbxTransMatrix.Double44()[ 3 ][ 3 ] ;

							CreateInverseMatrix( &TransMatrix, &TransMatrix ) ;
							CreateMultiplyMatrix( &InvMatrix, &InvMatrix, &TransMatrix ) ;

							CreateMultiplyMatrix( &InvMatrix, &ReverseMat, &InvMatrix ) ;
							CreateMultiplyMatrix( &InvMatrix, &InvMatrix, &ReverseMat ) ;

							CreateInverseMatrix( &SkinWeight->ModelLocalMatrix, &InvMatrix ) ;
						}

						// ウエイト値が割り当てられていない頂点があったらこのボーンの所属にする
						{
							int i, j, k, CheckNum ;
							int Result ;

							// ウエイト値の無い頂点があるかチェック
							CheckNum = Mesh->PositionNum / 32 ;
							for( i = 0 ; i < CheckNum && ( ( DWORD * )PositionFillFlag )[ i ] == 0xffffffff ; i ++ ){}
							Result  = i != CheckNum ? 1 : 0 ;

							CheckNum = Mesh->PositionNum % 32 ;
							for( j = 0 ; j < CheckNum && ( ( ( BYTE * )( &( ( DWORD * )PositionFillFlag )[ i ] ) )[ j / 8 ] & ( 1 << ( j % 8 ) ) ) != 0 ; j ++ ){}
							Result |= j != CheckNum ? 1 : 0 ;

							// 無かったらこのフレームの所属としてのウエイト情報を追加
							if( Result == 1 )
							{
								int WeightNoneNum ;
								DWORD CheckData ;
								BYTE CheckDataByte ;

								// 改めてウエイト値の無い頂点の数を数える
								WeightNoneNum = 0 ;
								CheckNum = Mesh->PositionNum / 32 ;
								for( i = 0 ; i < CheckNum ; i ++ )
								{
									CheckData = ( ( DWORD * )PositionFillFlag )[ i ] ;
									for( j = 0 ; j < 32 ; j ++ )
									{
										if( ( CheckData & ( 1 << j ) ) == 0 )
										{
											WeightNoneNum ++ ;
										}
									}
								}
								CheckData = ( ( DWORD * )PositionFillFlag )[ i ] ;
								CheckNum = Mesh->PositionNum % 32 ;
								for( j = 0 ; j < CheckNum ; j ++ )
								{
									if( ( ( ( BYTE * )&CheckData )[ j / 8 ] & ( 1 << ( j % 8 ) ) ) == 0 )
									{
										WeightNoneNum ++ ;
									}
								}

								// ウエイト情報の追加
								SkinWeight = MV1RAddSkinWeight( RModel ) ;
								Mesh->SkinWeights[ Mesh->SkinWeightsNum ] = SkinWeight ;
								Mesh->SkinWeights[ Mesh->SkinWeightsNum ]->UserData = NULL ;
								Mesh->SkinWeightsNum ++ ;
								CreateIdentityMatrix( &SkinWeight->ModelLocalMatrix ) ;

								// 各頂点への影響情報を格納するためのメモリの確保
								SkinWeight->DataNum = WeightNoneNum ;
								SkinWeight->Data = ( MV1_SKIN_WEIGHT_ONE_R * )ADDMEMAREA( sizeof( MV1_SKIN_WEIGHT_ONE_R ) * SkinWeight->DataNum, &RModel->Mem ) ;
								if( SkinWeight->Data == NULL )
								{
									DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\xb9\x30\xad\x30\xf3\x30\xa6\x30\xa8\x30\xa4\x30\xc8\x30\xc5\x60\x31\x58\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x32\x00\x0a\x00\x00"/*@ L"Fbx Load : スキンウエイト情報を格納するメモリの確保に失敗しました 2\n" @*/ )) ;
									return -1 ;
								}

								// 影響情報をセットする
								WeightNoneNum = 0 ;
								CheckNum = Mesh->PositionNum / 8 ;
								k = 0 ;
								for( i = 0 ; i < CheckNum ; i ++ )
								{
									CheckDataByte = PositionFillFlag[ i ] ;
									for( j = 0 ; j < 8 ; j ++, k ++ )
									{
										if( ( CheckDataByte & ( 1 << j ) ) == 0 )
										{
											SkinWeight->Data[ WeightNoneNum ].TargetVertex = k ;
											SkinWeight->Data[ WeightNoneNum ].Weight = 1.0f ;
											WeightNoneNum ++ ;
										}
									}
								}
								CheckDataByte = PositionFillFlag[ i ] ;
								CheckNum = Mesh->PositionNum % 8 ;
								for( j = 0 ; j < CheckNum ; j ++, k ++ )
								{
									if( ( CheckDataByte & ( 1 << j ) ) == 0 )
									{
										SkinWeight->Data[ WeightNoneNum ].TargetVertex = k ;
										SkinWeight->Data[ WeightNoneNum ].Weight = 1.0f ;
										WeightNoneNum ++ ;
									}
								}
							}
						}
					}

					// シェイプの情報を取得
					if( _FbxMesh->GetDeformerCount( FbxDeformer::eBlendShape ) > 0 )
					{
						int ChannelCount ;

						// ２個以上のシェイプには未対応
						if( _FbxMesh->GetDeformerCount( FbxDeformer::eBlendShape ) > 1 )
						{
							DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\x11\xff\xe1\x30\xc3\x30\xb7\x30\xe5\x30\x6b\x30\x07\x89\x70\x65\x6e\x30\xd6\x30\xec\x30\xf3\x30\xc9\x30\xb7\x30\xa7\x30\xa4\x30\xd7\x30\x6b\x30\x6f\x30\xfe\x5b\xdc\x5f\x57\x30\x66\x30\x44\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"Fbx Load : １メッシュに複数のブレンドシェイプには対応していません\n" @*/ )) ;
							return -1 ;
						}

						_FbxBlendShape = ( FbxBlendShape * )_FbxMesh->GetDeformer( 0, FbxDeformer::eBlendShape ) ;

						ChannelCount = _FbxBlendShape->GetBlendShapeChannelCount() ;

						for( i = 0 ; i < ChannelCount ; i ++ )
						{
							_FbxBlendShapeChannel = _FbxBlendShape->GetBlendShapeChannel( i ) ;
							if( _FbxBlendShapeChannel == NULL )
							{
								continue ;
							}

							_FbxShape = _FbxBlendShapeChannel->GetTargetShape( 0 ) ;
							if( _FbxShape == NULL )
							{
								continue ;
							}

							// シェイプ情報の追加
	//						FbxUTF8ToAnsi( _FbxShape->GetName(), ANSIBuffer, &ANSISize ) ;
							Shape = MV1RAddShape( RModel, _FbxShape->GetName(), Frame ) ; 

							// 対象メッシュのセット
							Shape->TargetMesh = Mesh ;

							// 法線は無し
							Shape->ValidVertexNormal = FALSE ;

							// 頂点の数を保存
							Shape->VertexNum = _FbxShape->GetControlPointsCount() ;

							// 頂点データを格納するメモリ領域の確保
							Shape->Vertex = ( MV1_SHAPE_VERTEX_R * )ADDMEMAREA( sizeof( MV1_SHAPE_VERTEX_R ) * Shape->VertexNum, &RModel->Mem ) ;
							if( Shape->Vertex == NULL )
							{
								DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\xb7\x30\xa7\x30\xa4\x30\xd7\x30\x02\x98\xb9\x70\xc7\x30\xfc\x30\xbf\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : シェイプ頂点データを格納するメモリ領域の確保に失敗しました\n" @*/ )) ;
								return -1 ;
							}

							// 頂点データを埋める
							ShapeVert = Shape->Vertex ;
							FbxShapeVec = _FbxShape->GetControlPoints() ;
							FbxVec = _FbxMesh->GetControlPoints() ;
							for( j = 0 ; j < Shape->VertexNum ; j ++, ShapeVert ++, FbxShapeVec ++, FbxVec ++ )
							{
								ShapeVert->TargetPositionIndex = j ;
								ShapeVert->Position.x = ( float ) ( FbxShapeVec->mData[ 0 ] - FbxVec->mData[ 0 ] ) ;
								ShapeVert->Position.y = ( float ) ( FbxShapeVec->mData[ 1 ] - FbxVec->mData[ 1 ] ) ;
								ShapeVert->Position.z = ( float )-( FbxShapeVec->mData[ 2 ] - FbxVec->mData[ 2 ] ) ;
								ShapeVert->Normal.x = 0.0f ;
								ShapeVert->Normal.y = 0.0f ;
								ShapeVert->Normal.z = 0.0f ;
							}
						}
					}
				}
			}
			else
			// ライトノードの場合はライトを取得する
			if( FbxAttr->GetAttributeType() == FbxNodeAttribute::eLight )
			{
				FbxLight *_FbxLight ;

				_FbxLight = ( FbxLight * )FbxAttr ;

				// ライトの追加
//				FbxUTF8ToAnsi( pFbxNode->GetName(), ANSIBuffer, &ANSISize ) ;
				Frame->Light = MV1RAddLight( RModel, pFbxNode->GetName() ) ;
				if( Frame->Light == NULL )
				{
					DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\xe9\x30\xa4\x30\xc8\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\xfd\x8f\xa0\x52\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : ライトオブジェクトの追加に失敗しました\n" @*/ )) ;
					return -1 ;
				}

				// ライトの情報を取得する
				switch( _FbxLight->LightType.Get() )
				{
				case FbxLight::ePoint :
					Frame->Light->Type = MV1_LIGHT_TYPE_POINT ;
					break ;

				case FbxLight::eSpot :
					Frame->Light->Type = MV1_LIGHT_TYPE_SPOT ;
					break ;

				case FbxLight::eDirectional :
					Frame->Light->Type = MV1_LIGHT_TYPE_DIRECTIONAL ;
					break ;
				}
				Frame->Light->Diffuse.r = ( float )_FbxLight->Color.Get()[ 0 ] ;
				Frame->Light->Diffuse.g = ( float )_FbxLight->Color.Get()[ 1 ] ;
				Frame->Light->Diffuse.b = ( float )_FbxLight->Color.Get()[ 2 ] ;
				Frame->Light->Range = ( float )_FbxLight->Intensity.Get() / 10.0f ;
				Frame->Light->Falloff = 0.1f ;
				Frame->Light->Attenuation0 = 1.0f ;
				Frame->Light->Attenuation1 = 0.0f ;
				Frame->Light->Attenuation2 = 0.0f ;
				Frame->Light->Theta = ( float )_FbxLight->InnerAngle.Get() ;
				Frame->Light->Phi = ( float )_FbxLight->OuterAngle.Get() ;
				Frame->Light->FrameIndex = Frame->Index ;
			}
		}
	}

	// 子のノードも解析する
	Num = pFbxNode->GetChildCount() ;
	for( i = 0 ; i < Num ; i ++ )
	{
		pFbxChildNode = pFbxNode->GetChild( i ) ;
		if( AnalyseFbxNode( RModel, Model, Frame, pFbxChildNode ) == -1 )
			return -1 ;
	}

	// 終了
	return 0 ;
}

// FBXファイルの解析( 0:成功  -1:失敗 )
static int AnalyseFbx( MV1_MODEL_R *RModel, FBX_MODEL *Model )
{
	int i, j, k ;
	MV1_MESH_R *Mesh ;
	MV1_FRAME_R *TargetFrame ;
	MV1_SKIN_WEIGHT_R *SkinWeight ;
	void *Node ;
//	char *ANSIBuffer ;
//	size_t ANSISize ;

	// ノードの解析
	if( AnalyseFbxNode( RModel, Model, NULL, NULL ) == -1 )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\xce\x30\xfc\x30\xc9\x30\x6e\x30\xe3\x89\x90\x67\x67\x30\xa8\x30\xe9\x30\xfc\x30\x4c\x30\x7a\x76\x1f\x75\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : ノードの解析でエラーが発生しました\n" @*/ )) ;
		return -1 ;
	}

	// スキンウエイト情報に関連するフレームを割り出しておく
	{
		// メッシュの数だけ繰り返し
		Mesh = RModel->MeshFirst ;
		for( i = 0 ; ( DWORD )i < RModel->MeshNum ; i ++, Mesh = Mesh->DataNext )
		{
			// スキンウエイトの数だけ繰り返し
			for( j = 0 ; ( DWORD )j < Mesh->SkinWeightsNum ; j ++ )
			{
				SkinWeight = Mesh->SkinWeights[ j ] ;

				// UserData が NULL の場合はメッシュが所属しているフレームが対象フレーム
				if( SkinWeight->UserData == NULL )
				{
					TargetFrame = Mesh->Container ;
					SkinWeight->TargetFrame = TargetFrame->Index ;
				}
				else
				{
					// 一致するフレームを検索する
					Node = ( ( FbxCluster * )SkinWeight->UserData )->GetLink() ;
					TargetFrame = RModel->FrameFirst ;
					for( k = 0 ; ( DWORD )k < RModel->FrameNum && TargetFrame->UserData != Node ; k ++, TargetFrame = TargetFrame->DataNext ){}
					SkinWeight->TargetFrame = k ;
				}
			}
		}
	}

	// アニメーションの解析
	{
		int TakeNum ;
		MV1_ANIMSET_R *AnimSet ;
		MV1_ANIM_R *Anim ;
		MV1_FRAME_R *Frame ;
		FbxAnimStack *lFbxAnimStack ;

		// アニメーションセットの数を取得
		TakeNum = Model->pScene->GetSrcObjectCount< FbxAnimStack >() ;

		// アニメーションの数だけ繰り返し
		for( i = 0 ; i < TakeNum ; i ++ )
		{
			// テイク情報を取得する
			lFbxAnimStack = Model->pScene->GetSrcObject< FbxAnimStack >( i ) ;

			// アニメーション情報が無かったらここで終了
			if( lFbxAnimStack == NULL ) continue ;
			if( lFbxAnimStack->GetMemberCount< FbxAnimLayer >() == 0 ) continue ;

			// アニメーションセットを追加
//			FbxUTF8ToAnsi( lFbxAnimStack->GetName(), ANSIBuffer, &ANSISize ) ;
			AnimSet = MV1RAddAnimSet( RModel, lFbxAnimStack->GetName() ) ;
			if( AnimSet == NULL )
			{
//				DeleteAndClear( FbxTakeName ) ;
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\xa2\x30\xcb\x30\xe1\x30\xfc\x30\xb7\x30\xe7\x30\xf3\x30\xbb\x30\xc3\x30\xc8\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\xfd\x8f\xa0\x52\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : アニメーションセットオブジェクトの追加に失敗しました\n" @*/ )) ;
				return -1 ;
			}

			// 開始時刻と終了時刻の初期化
			AnimSet->StartTime =  1000000000.0f ;
			AnimSet->EndTime   = -1000000000.0f ;

			FbxAnimLayer *pFbxAnimLayer = lFbxAnimStack->GetMember< FbxAnimLayer >( 0 ) ;

			// アニメーションの情報を取得する
			Frame = RModel->FrameFirst ;
			for( j = 0 ; ( DWORD )j < RModel->FrameNum ; j ++, Frame = Frame->DataNext )
			{
				// テイクノードの取得
				FbxNode *pFbxNode = ( FbxNode * )Frame->UserData ;

				// カーブデータが存在する要素のアニメーション情報を取得する
				Anim = NULL ;
				if( GetFbxAnimInfo( RModel, Model, Frame, AnimSet, &Anim, MV1_ANIMKEY_DATATYPE_TRANSLATE_X, pFbxNode->LclTranslation.GetCurve( pFbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_X )              ) == -1 ) return -1 ;
				if( GetFbxAnimInfo( RModel, Model, Frame, AnimSet, &Anim, MV1_ANIMKEY_DATATYPE_TRANSLATE_Y, pFbxNode->LclTranslation.GetCurve( pFbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y )              ) == -1 ) return -1 ;
				if( GetFbxAnimInfo( RModel, Model, Frame, AnimSet, &Anim, MV1_ANIMKEY_DATATYPE_TRANSLATE_Z, pFbxNode->LclTranslation.GetCurve( pFbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z ), true        ) == -1 ) return -1 ;
				if( GetFbxAnimInfo( RModel, Model, Frame, AnimSet, &Anim, MV1_ANIMKEY_DATATYPE_SCALE_X,     pFbxNode->LclScaling.GetCurve(     pFbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_X )              ) == -1 ) return -1 ;
				if( GetFbxAnimInfo( RModel, Model, Frame, AnimSet, &Anim, MV1_ANIMKEY_DATATYPE_SCALE_Y,     pFbxNode->LclScaling.GetCurve(     pFbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y )              ) == -1 ) return -1 ;
				if( GetFbxAnimInfo( RModel, Model, Frame, AnimSet, &Anim, MV1_ANIMKEY_DATATYPE_SCALE_Z,     pFbxNode->LclScaling.GetCurve(     pFbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z )              ) == -1 ) return -1 ;
				if( GetFbxAnimInfo( RModel, Model, Frame, AnimSet, &Anim, MV1_ANIMKEY_DATATYPE_ROTATE_X,    pFbxNode->LclRotation.GetCurve(    pFbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_X ), true,  true ) == -1 ) return -1 ;
				if( GetFbxAnimInfo( RModel, Model, Frame, AnimSet, &Anim, MV1_ANIMKEY_DATATYPE_ROTATE_Y,    pFbxNode->LclRotation.GetCurve(    pFbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y ), true,  true ) == -1 ) return -1 ;
				if( GetFbxAnimInfo( RModel, Model, Frame, AnimSet, &Anim, MV1_ANIMKEY_DATATYPE_ROTATE_Z,    pFbxNode->LclRotation.GetCurve(    pFbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z ), false, true ) == -1 ) return -1 ;

				// キーセットが一つも無かったらこのノードにはアニメーションが無いということ
				if( Anim == NULL )
					continue ;

				// 時間のセット
				Anim->MaxTime = AnimSet->EndTime ;

				// ノードの名前を保存
				Anim->TargetFrameIndex = Frame->Index ;

				// 回転オーダーのセット
				Anim->RotateOrder = Frame->RotateOrder ;
			}
		}
	}

	// 成功
	return 0 ;
}

extern int MV1LoadModelToFBX( const MV1_MODEL_LOAD_PARAM *LoadParam, int ASyncThread )
{
	int NewHandle = -1 ;
	int iFileFormat = -1 ;
	int ErrorFlag = 1 ;
	FBX_MODEL FbxModel ;
	MV1_MODEL_R RModel ;
	int Major, Minor, Revision ;
	char UTF8Buffer[ 512 * 3 + 16 ] ;

	// 読み込みようデータの初期化
	MV1InitReadModel( &RModel ) ;
	RModel.MeshFaceRightHand = TRUE ;

	// モデル名とファイル名と文字コード形式をセット
	RModel.CharCodeFormat = DX_CHARCODEFORMAT_UTF8 ;
	RModel.FilePath = ( wchar_t * )DXALLOC( ( _WCSLEN( LoadParam->FilePath ) + 1 ) * sizeof( wchar_t ) ) ;
	RModel.Name     = ( wchar_t * )DXALLOC( ( _WCSLEN( LoadParam->Name     ) + 1 ) * sizeof( wchar_t ) ) ;
	_WCSCPY( RModel.FilePath, LoadParam->FilePath ) ;
	_WCSCPY( RModel.Name,     LoadParam->Name ) ;

	// FBXモデルデータを０初期化
	_MEMSET( &FbxModel, 0, sizeof( FbxModel ) ) ;

	// SDKマネージャ生成
	FbxModel.pManager = FbxManager::Create();
	if( FbxModel.pManager == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\x46\x00\x42\x00\x58\x00\x20\x00\x4d\x00\x61\x00\x6e\x00\x61\x00\x67\x00\x65\x00\x72\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : FBX Manager の作成に失敗しました\n" @*/ )) ;
		goto FUNCTIONEND ;
	}
	
	// IOSettings クラスの作成
	FbxModel.pIOSettings = FbxIOSettings::Create( FbxModel.pManager, IOSROOT ) ;
	FbxModel.pManager->SetIOSettings( FbxModel.pIOSettings ) ;

	// プラグインパスの設定
	{
		FbxString lPath = FbxGetApplicationDirectory() ;
		FbxModel.pManager->LoadPluginsDirectory( lPath.Buffer() ) ;
	}

	// シーンの作成
	FbxModel.pScene = FbxScene::Create( FbxModel.pManager, "Scene" ) ;
	if( FbxModel.pScene == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\x46\x00\x42\x00\x58\x00\x20\x00\x53\x00\x63\x00\x65\x00\x6e\x00\x65\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : FBX Scene の作成に失敗しました\n" @*/ )) ;
		goto FUNCTIONEND ;
	}

	// インポーターの作成
	FbxModel.pImporter = FbxImporter::Create( FbxModel.pManager, "" ) ;
	if( FbxModel.pImporter == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\xa4\x30\xf3\x30\xdd\x30\xfc\x30\xbf\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : インポーターの作成に失敗しました\n" @*/ )) ;
		goto FUNCTIONEND ;
	}

	// FBXフォーマットのチェック
	ConvString( ( const char * )LoadParam->FilePath, -1, WCHAR_T_CHARCODEFORMAT, ( char * )UTF8Buffer, sizeof( UTF8Buffer ), DX_CHARCODEFORMAT_UTF8 ) ;
	if( FbxModel.pManager->GetIOPluginRegistry()->DetectReaderFileFormat( UTF8Buffer, iFileFormat ) == false )
	{
		goto FUNCTIONEND ;
	}

	// 読み込み
	if( FbxModel.pImporter->Initialize( UTF8Buffer, iFileFormat, FbxModel.pIOSettings ) == false )
	{
		goto FUNCTIONEND ;
	}

	// ＦＢＸかチェック
	if( FbxModel.pImporter->IsFBX() == false )
		goto FUNCTIONEND ;

	// 読み取り情報の設定
	FbxModel.pIOSettings->SetBoolProp( IMP_FBX_MATERIAL,		true ) ;
	FbxModel.pIOSettings->SetBoolProp( IMP_FBX_TEXTURE,			true ) ;
	FbxModel.pIOSettings->SetBoolProp( IMP_FBX_LINK,			true ) ;
	FbxModel.pIOSettings->SetBoolProp( IMP_FBX_SHAPE,			true ) ;
	FbxModel.pIOSettings->SetBoolProp( IMP_FBX_GOBO,			true ) ;
	FbxModel.pIOSettings->SetBoolProp( IMP_FBX_ANIMATION,		true ) ;
	FbxModel.pIOSettings->SetBoolProp( IMP_FBX_GLOBAL_SETTINGS, true ) ;

	// インポート
	if( FbxModel.pImporter->Import( FbxModel.pScene ) == false )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x46\x00\x62\x00\x78\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x3a\x00\x20\x00\xb7\x30\xfc\x30\xf3\x30\x6e\x30\xa4\x30\xf3\x30\xdd\x30\xfc\x30\xc8\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Fbx Load : シーンのインポートに失敗しました\n" @*/ )) ;
		goto FUNCTIONEND ;
	}

	// バージョンを取得
	FbxModel.pImporter->GetFileVersion( Major, Minor, Revision ) ;

	// インポータの削除
	FbxModel.pImporter->Destroy() ;
	FbxModel.pImporter = NULL ;

	// ノードを手繰る
	if( AnalyseFbx( &RModel, &FbxModel ) == -1 )
		goto FUNCTIONEND ;

	// モデル基データハンドルの作成
	NewHandle = MV1LoadModelToReadModel( &LoadParam->GParam, &RModel, LoadParam->CurrentDir, LoadParam->FileReadFunc, ASyncThread ) ;
	if( NewHandle < 0 ) goto FUNCTIONEND ;

	// エラーフラグを倒す
	ErrorFlag = 0 ;

FUNCTIONEND :

	// エラーフラグが立っていたらモデルハンドルを解放
	if( ErrorFlag == 1 && NewHandle != -1 )
	{
		MV1SubModelBase( NewHandle ) ;
		NewHandle = -1 ;
	}

	// インポーターの解放
	if( FbxModel.pImporter )
	{
		FbxModel.pImporter->Destroy() ;
		FbxModel.pImporter = NULL ;
	}

	if( FbxModel.pIOSettings )
	{
		FbxModel.pIOSettings->Destroy() ;
		FbxModel.pIOSettings = NULL ;
	}

	// シーンの解放
	if( FbxModel.pScene )
	{
		FbxModel.pScene->Destroy() ;
		FbxModel.pScene = NULL ;
	}

	// SDKマネージャの解放
	if( FbxModel.pManager )
	{
		FbxModel.pManager->Destroy() ;
		FbxModel.pManager = NULL ;
	}

	// 読み込みモデルの後始末
	MV1TermReadModel( &RModel ) ;

	// ハンドルを返す
	return NewHandle ;
}

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif

#endif
