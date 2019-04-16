// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		ＶＭＤデータ読み込みプログラム
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#define __DX_MAKE

#include "DxModelLoaderVMD.h"

#ifndef DX_NON_MODEL

// インクルード ---------------------------------
#include "DxModelRead.h"
#include "DxFile.h"
#include "DxLog.h"
#include "DxMemory.h"


#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 -----------------------------------

// データ宣言 -----------------------------------


// 関数宣言 -------------------------------------

// プログラム -----------------------------------

// 条件を限定した少し高速な逆行列計算
extern void MV1LoadModelToVMD_InverseMatrix( MATRIX &InMatrix, MATRIX &OutMatrix )
{
	OutMatrix.m[ 0 ][ 0 ] = InMatrix.m[ 0 ][ 0 ] ;
	OutMatrix.m[ 0 ][ 1 ] = InMatrix.m[ 1 ][ 0 ] ;
	OutMatrix.m[ 0 ][ 2 ] = InMatrix.m[ 2 ][ 0 ] ;
	OutMatrix.m[ 0 ][ 3 ] = 0.0f ;
	OutMatrix.m[ 1 ][ 0 ] = InMatrix.m[ 0 ][ 1 ] ;
	OutMatrix.m[ 1 ][ 1 ] = InMatrix.m[ 1 ][ 1 ] ;
	OutMatrix.m[ 1 ][ 2 ] = InMatrix.m[ 2 ][ 1 ] ;
	OutMatrix.m[ 1 ][ 3 ] = 0.0f ;
	OutMatrix.m[ 2 ][ 0 ] = InMatrix.m[ 0 ][ 2 ] ;
	OutMatrix.m[ 2 ][ 1 ] = InMatrix.m[ 1 ][ 2 ] ;
	OutMatrix.m[ 2 ][ 2 ] = InMatrix.m[ 2 ][ 2 ] ;
	OutMatrix.m[ 2 ][ 3 ] = 0.0f ;

	OutMatrix.m[ 3 ][ 0 ] = -( InMatrix.m[ 3 ][ 0 ] * OutMatrix.m[0][0] + InMatrix.m[ 3 ][ 1 ] * OutMatrix.m[1][0] + InMatrix.m[ 3 ][ 2 ] * OutMatrix.m[2][0] ) ;
	OutMatrix.m[ 3 ][ 1 ] = -( InMatrix.m[ 3 ][ 0 ] * OutMatrix.m[0][1] + InMatrix.m[ 3 ][ 1 ] * OutMatrix.m[1][1] + InMatrix.m[ 3 ][ 2 ] * OutMatrix.m[2][1] ) ;
	OutMatrix.m[ 3 ][ 2 ] = -( InMatrix.m[ 3 ][ 0 ] * OutMatrix.m[0][2] + InMatrix.m[ 3 ][ 1 ] * OutMatrix.m[1][2] + InMatrix.m[ 3 ][ 2 ] * OutMatrix.m[2][2] ) ;
	OutMatrix.m[ 3 ][ 3 ] = 1.0f ;
/*
	MATRIX tempMat ;
	int i, j ;
	float sa ;
	tempMat = MInverse( InMatrix ) ;

	for( i = 0 ; i < 4 ; i ++ )
	{
		for( j = 0 ; j < 4 ; j ++ )
		{
			sa = tempMat.m[ i ][ j ] - OutMatrix.m[ i ][ j ] ;
			if( sa < 0.0f ) sa = -sa ;
			if( sa > 0.01f )
			{
				i = i ;
			}
		}
	}
*/
}

// クォータニオンの正規化
extern void QuaternionNormalize( FLOAT4 *Out, FLOAT4 *Src )
{
	float r ;

	r = 1.0f / _SQRT( Src->x * Src->x + Src->y * Src->y + Src->z * Src->z + Src->w * Src->w ) ;

	Out->x = Src->x * r ;
	Out->y = Src->y * r ;
	Out->z = Src->z * r ;
	Out->w = Src->w * r ;
}

// クォータニオンからXYZ軸回転の取得
extern void QuaternionToEuler( VECTOR *Angle, const FLOAT4 *Qt )
{
	float x2, y2, z2 ;
	float xz2, wy2 ;
	float temp, YRad ;
	float xx2, xy2, zz2, wz2 ;
	float yz2, wx2, yy2 ;

	x2 = Qt->x + Qt->x ;
	y2 = Qt->y + Qt->y ;
	z2 = Qt->z + Qt->z ;
	xz2 = Qt->x * z2 ;
	wy2 = Qt->w * y2 ;
	temp = -( xz2 - wy2 ) ;

		 if( temp >=  1.0f ) temp =  1.0f ;
	else if( temp <= -1.0f ) temp = -1.0f ;

	YRad = _ASIN( temp ) ;

	xx2 = Qt->x * x2 ;
	xy2 = Qt->x * y2 ;
	zz2 = Qt->z * z2 ;
	wz2 = Qt->w * z2 ;

	if( YRad < 3.1415926f * 0.5f )
	{
		if( YRad > -3.1415926f * 0.5f )
		{
			yz2 = Qt->y * z2 ;
			wx2 = Qt->w * x2 ;
			yy2 = Qt->y * y2 ;
			Angle->x = _ATAN2( ( yz2 + wx2 ), ( 1.0f - ( xx2 + yy2 ) ) ) ;
			Angle->y = YRad ;
			Angle->z = _ATAN2( ( xy2 + wz2 ), ( 1.0f - ( yy2 + zz2 ) ) ) ;
		}
		else
		{
			Angle->x = -_ATAN2( ( xy2 - wz2 ), ( 1.0f - ( xx2 + zz2 ) ) ) ;
			Angle->y = YRad ;
			Angle->z = 0.0f ;
		}
	}
	else
	{
		Angle->x = _ATAN2( ( xy2 - wz2 ), ( 1.0f - ( xx2 + zz2 ) ) ) ;
		Angle->y = YRad ;
		Angle->z = 0.0f ;
	}
}

// XYZ回転値からクォータニオンの作成
extern void QuaternionCreateEuler( FLOAT4 *Qt, const VECTOR *Angle )
{
	float SinX, CosX, SinY, CosY, SinZ, CosZ ;

	_SINCOS_PLATFORM( Angle->x * 0.5f, &SinX, &CosX ) ;
	_SINCOS_PLATFORM( Angle->y * 0.5f, &SinY, &CosY ) ;
	_SINCOS_PLATFORM( Angle->z * 0.5f, &SinZ, &CosZ ) ;

	Qt->x = SinX * CosY * CosZ - CosX * SinY * SinZ;
	Qt->y = CosX * SinY * CosZ + SinX * CosY * SinZ;
	Qt->z = CosX * CosY * SinZ - SinX * SinY * CosZ;
	Qt->w = CosX * CosY * CosZ + SinX * SinY * SinZ;
}



// ＶＭＤファイルの基本情報を読み込む( -1:エラー )
extern int LoadVMDBaseData( VMD_READ_INFO * VmdData, void *DataBuffer, int /*DataSize*/ )
{
	DWORD i, j, keynum ;
	VMD_READ_NODE_INFO *VmdNode ;
	VMD_READ_KEY_INFO *VmdKey, *VmdKeyTemp1, *VmdKeyTemp2 ;
	VMD_READ_CAMERA_INFO *VmdCam ;
	VMD_READ_CAMERA_KEY_INFO *VmdCamKey, *VmdCamKeyTemp1, *VmdCamKeyTemp2 ;
	VMD_READ_FACE_KEY_SET_INFO *VmdFaceKeySet ;
	VMD_READ_FACE_KEY_INFO *VmdFaceKey, *VmdFaceKeyTemp1, *VmdFaceKeyTemp2 ;
	BYTE *Src, *KeyHead, *FaceKeyHead, *CameraKeyHead ;
	DWORD CameraKeyNum ;

	Src = ( BYTE * )DataBuffer ;
	VmdData->Node = NULL ;
	VmdData->FaceKeySet = NULL ;
	VmdData->Camera = NULL ;
	VmdData->KeyBuffer = NULL ;
	VmdData->FaceKeyBuffer = NULL ;
	VmdData->CamKeyBuffer = NULL ;

	// VMDファイルかどうかを確認
	if( _MEMCMP( Src, "Vocaloid Motion Data 0002", 26 ) != 0 )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x56\x00\x4d\x00\x44\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xfe\x5b\xdc\x5f\x57\x30\x66\x30\x44\x30\x6a\x30\x44\x30\x20\x00\x56\x00\x4d\x00\x44\x00\x20\x00\xd0\x30\xfc\x30\xb8\x30\xe7\x30\xf3\x30\x67\x30\x59\x30\x0a\x00\x00"/*@ L"VMD Load Error : 対応していない VMD バージョンです\n" @*/ )) ;
		return -1 ;
	}
	Src += 30 ;

	// モデル名のスキップ
	Src += 20 ;

	// ノード情報保存用のメモリを確保
	VmdData->NodeNum = 0 ;
	VmdData->Node = ( VMD_READ_NODE_INFO * )DXALLOC( sizeof( VMD_READ_NODE_INFO ) * 2048 ) ;
	if( VmdData->Node == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x56\x00\x4d\x00\x44\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xce\x30\xfc\x30\xc9\x30\xc5\x60\x31\x58\x3c\x68\x0d\x7d\x28\x75\x6e\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"VMD Load Error : ノード情報格納用のメモリの確保に失敗しました\n" @*/ )) ;
		goto ENDLABEL ;
	}

	// キーの総数とキーを保存するメモリ領域の確保
	keynum = GET_MEM_DWORD( Src ) ;
	Src += 4 ;
	if( keynum != 0 )
	{
		KeyHead = Src ;
		VmdData->KeyBuffer = ( VMD_READ_KEY_INFO * )DXALLOC( sizeof( VMD_READ_KEY_INFO ) * keynum ) ;
		if( VmdData->KeyBuffer == NULL )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x56\x00\x4d\x00\x44\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xa2\x30\xcb\x30\xe1\x30\xfc\x30\xb7\x30\xe7\x30\xf3\x30\xad\x30\xfc\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"VMD Load Error : アニメーションキーを格納するメモリの確保に失敗しました\n" @*/ )) ;
			goto ENDLABEL ;
		}

		// ノードの総数と各ノードのキーの数を取得
		Src = KeyHead ;
		VmdKey = VmdData->KeyBuffer ;
		for( i = 0 ; i < keynum ; i ++, Src += 111, VmdKey ++ )
		{
			// 既に登録されているノードかどうかを調べる
			VmdNode = VmdData->Node ;
			for( j = 0 ; ( DWORD )j < VmdData->NodeNum ; j ++, VmdNode ++ )
			{
				if( _MEMCMP( Src, VmdNode->Name, 15 ) == 0 ) break ;
			}

			// 無かったら登録する
			if( VmdData->NodeNum == ( DWORD )j )
			{
				VmdNode->KeyNum = 0 ;
				VmdNode->MaxFrame = 0 ;
				VmdNode->MinFrame = 0xffffffff ;
				_MEMCPY( VmdNode->Name, Src, 15 ) ;
				VmdNode->Name[ 15 ] = '\0' ;
				VmdNode->FirstKey = NULL ;
				VmdData->NodeNum ++ ;
			}

			// キーの情報を格納する
			VmdKey->Frame = GET_MEM_DWORD( &Src[ 15 ] ) ;
			READ_MEM_4BYTE( &VmdKey->Position[ 0 ], &Src[ 19 ] ) ;
			READ_MEM_4BYTE( &VmdKey->Position[ 1 ], &Src[ 23 ] ) ;
			READ_MEM_4BYTE( &VmdKey->Position[ 2 ], &Src[ 27 ] ) ;
			READ_MEM_4BYTE( &VmdKey->Quaternion[ 0 ], &Src[ 31 ] ) ;
			READ_MEM_4BYTE( &VmdKey->Quaternion[ 1 ], &Src[ 35 ] ) ;
			READ_MEM_4BYTE( &VmdKey->Quaternion[ 2 ], &Src[ 39 ] ) ;
			READ_MEM_4BYTE( &VmdKey->Quaternion[ 3 ], &Src[ 43 ] ) ;

			VmdKey->Linear[ 0 ] = 0 ;
			if( *( ( char * )&Src[ 47 ] ) == *( ( char * )&Src[ 51 ] ) &&
				*( ( char * )&Src[ 55 ] ) == *( ( char * )&Src[ 59 ] ) )
			{
				VmdKey->Linear[ 0 ] = 1 ;
			}
			VmdKey->Linear[ 1 ] = 0 ;
			if( *( ( char * )&Src[ 63 ] ) == *( ( char * )&Src[ 67 ] ) &&
				*( ( char * )&Src[ 71 ] ) == *( ( char * )&Src[ 75 ] ) )
			{
				VmdKey->Linear[ 1 ] = 1 ;
			}
			VmdKey->Linear[ 2 ] = 0 ;
			if( *( ( char * )&Src[ 79 ] ) == *( ( char * )&Src[ 83 ] ) &&
				*( ( char * )&Src[ 87 ] ) == *( ( char * )&Src[ 91 ] ) )
			{
				VmdKey->Linear[ 2 ] = 1 ;
			}
			VmdKey->Linear[ 3 ] = 0 ;
			if( *( ( char * )&Src[ 95 ] ) == *( ( char * )&Src[ 99 ] ) &&
				*( ( char * )&Src[ 103 ] ) == *( ( char * )&Src[ 107 ] ) )
			{
				VmdKey->Linear[ 3 ] = 1 ;
			}
			VmdKey->PosXBezier[ 0 ] = GET_MEM_SIGNED_BYTE( &Src[  47 ] ) / 127.0f * 3.0f ;
			VmdKey->PosXBezier[ 1 ] = GET_MEM_SIGNED_BYTE( &Src[  51 ] ) / 127.0f * 3.0f ;
			VmdKey->PosXBezier[ 2 ] = GET_MEM_SIGNED_BYTE( &Src[  55 ] ) / 127.0f * 3.0f ;
			VmdKey->PosXBezier[ 3 ] = GET_MEM_SIGNED_BYTE( &Src[  59 ] ) / 127.0f * 3.0f ;
			VmdKey->PosYBezier[ 0 ] = GET_MEM_SIGNED_BYTE( &Src[  63 ] ) / 127.0f * 3.0f ;
			VmdKey->PosYBezier[ 1 ] = GET_MEM_SIGNED_BYTE( &Src[  67 ] ) / 127.0f * 3.0f ;
			VmdKey->PosYBezier[ 2 ] = GET_MEM_SIGNED_BYTE( &Src[  71 ] ) / 127.0f * 3.0f ;
			VmdKey->PosYBezier[ 3 ] = GET_MEM_SIGNED_BYTE( &Src[  75 ] ) / 127.0f * 3.0f ;
			VmdKey->PosZBezier[ 0 ] = GET_MEM_SIGNED_BYTE( &Src[  79 ] ) / 127.0f * 3.0f ;
			VmdKey->PosZBezier[ 1 ] = GET_MEM_SIGNED_BYTE( &Src[  83 ] ) / 127.0f * 3.0f ;
			VmdKey->PosZBezier[ 2 ] = GET_MEM_SIGNED_BYTE( &Src[  87 ] ) / 127.0f * 3.0f ;
			VmdKey->PosZBezier[ 3 ] = GET_MEM_SIGNED_BYTE( &Src[  91 ] ) / 127.0f * 3.0f ;
			VmdKey->RotBezier[ 0 ]  = GET_MEM_SIGNED_BYTE( &Src[  95 ] ) / 127.0f * 3.0f ;
			VmdKey->RotBezier[ 1 ]  = GET_MEM_SIGNED_BYTE( &Src[  99 ] ) / 127.0f * 3.0f ;
			VmdKey->RotBezier[ 2 ]  = GET_MEM_SIGNED_BYTE( &Src[ 103 ] ) / 127.0f * 3.0f ;
			VmdKey->RotBezier[ 3 ]  = GET_MEM_SIGNED_BYTE( &Src[ 107 ] ) / 127.0f * 3.0f ;

			VmdKey->DisablePhysics = ( GET_MEM_BYTE( &Src[ 49 ] ) == 0x63 ) && ( GET_MEM_BYTE( &Src[ 50 ] ) == 0x0f ) ? TRUE : FALSE ;

			// キーをリストに追加する
			if( VmdNode->FirstKey == NULL )
			{
				VmdNode->FirstKey = VmdKey ;
				VmdKey->Prev = NULL ;
				VmdKey->Next = NULL ;
			}
			else
			{
				VmdKeyTemp1 = VmdNode->FirstKey ;
				VmdKeyTemp2 = NULL ;
				for( j = 0 ; ( DWORD )j < VmdNode->KeyNum ; j ++ )
				{
					if( VmdKeyTemp1->Frame > VmdKey->Frame ) break ;
					VmdKeyTemp2 = VmdKeyTemp1 ;
					VmdKeyTemp1 = VmdKeyTemp1->Next ;
				}
				if( ( DWORD )j == VmdNode->KeyNum )
				{
					VmdKey->Prev = VmdKeyTemp2 ;
					VmdKeyTemp2->Next = VmdKey ;
					VmdKey->Next = NULL ;
				}
				else
				{
					VmdKey->Prev = VmdKeyTemp1->Prev ;
					VmdKey->Next = VmdKeyTemp1 ;
					VmdKeyTemp1->Prev = VmdKey ;
					if( VmdKey->Prev == NULL )
					{
						VmdNode->FirstKey = VmdKey ;
					}
					else
					{
						VmdKey->Prev->Next = VmdKey ;
					}
				}
			}

			// キーの数を加算する
			VmdNode->KeyNum ++ ;

			// 最大フレーム番号と最小フレーム番号を更新
			if( VmdNode->MaxFrame < VmdKey->Frame ) VmdNode->MaxFrame = VmdKey->Frame ;
			if( VmdNode->MinFrame > VmdKey->Frame ) VmdNode->MinFrame = VmdKey->Frame ;
		}
	}

	// 表情キーセット用のメモリを確保
	VmdData->FaceKeySetNum = 0 ;
	VmdData->FaceKeySet = ( VMD_READ_FACE_KEY_SET_INFO * )DXALLOC( sizeof( VMD_READ_FACE_KEY_SET_INFO ) * 2048 ) ;
	if( VmdData->Node == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x56\x00\x4d\x00\x44\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x68\x88\xc5\x60\xad\x30\xfc\x30\xbb\x30\xc3\x30\xc8\x30\xc5\x60\x31\x58\x3c\x68\x0d\x7d\x28\x75\x6e\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"VMD Load Error : 表情キーセット情報格納用のメモリの確保に失敗しました\n" @*/ )) ;
		goto ENDLABEL ;
	}

	// 表情キーの総数とキーを保存するメモリ領域の確保
	keynum = GET_MEM_DWORD( Src ) ;
	Src += 4 ;
	if( keynum != 0 )
	{
		FaceKeyHead = Src ;
		VmdData->FaceKeyBuffer = ( VMD_READ_FACE_KEY_INFO * )DXALLOC( sizeof( VMD_READ_FACE_KEY_INFO ) * keynum ) ;
		if( VmdData->FaceKeyBuffer == NULL )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x56\x00\x4d\x00\x44\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\x68\x88\xc5\x60\xad\x30\xfc\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"VMD Load Error : 表情キーを格納するメモリの確保に失敗しました\n" @*/ )) ;
			goto ENDLABEL ;
		}

		// ノードの総数と各ノードのキーの数を取得
		Src = FaceKeyHead ;
		VmdFaceKey = VmdData->FaceKeyBuffer ;
		for( i = 0 ; i < keynum ; i ++, Src += 23, VmdFaceKey ++ )
		{
			// 既に登録されているキーセットかどうかを調べる
			VmdFaceKeySet = VmdData->FaceKeySet ;
			for( j = 0 ; ( DWORD )j < VmdData->FaceKeySetNum ; j ++, VmdFaceKeySet ++ )
			{
				if( _MEMCMP( Src, VmdFaceKeySet->Name, 15 ) == 0 ) break ;
			}

			// 無かったら登録する
			if( VmdData->FaceKeySetNum == ( DWORD )j )
			{
				VmdFaceKeySet->KeyNum = 0 ;
				VmdFaceKeySet->MaxFrame = 0 ;
				VmdFaceKeySet->MinFrame = 0xffffffff ;
				_MEMCPY( VmdFaceKeySet->Name, Src, 15 ) ;
				VmdFaceKeySet->Name[ 15 ] = '\0' ;
				VmdFaceKeySet->FirstKey = NULL ;
				VmdData->FaceKeySetNum ++ ;
			}

			// キーの情報を格納する
			READ_MEM_4BYTE( &VmdFaceKey->Frame, &Src[ 15 ] ) ;
			READ_MEM_4BYTE( &VmdFaceKey->Factor, &Src[ 19 ] ) ;

			// キーをリストに追加する
			if( VmdFaceKeySet->FirstKey == NULL )
			{
				VmdFaceKeySet->FirstKey = VmdFaceKey ;
				VmdFaceKey->Prev = NULL ;
				VmdFaceKey->Next = NULL ;
			}
			else
			{
				VmdFaceKeyTemp1 = VmdFaceKeySet->FirstKey ;
				VmdFaceKeyTemp2 = NULL ;
				for( j = 0 ; ( DWORD )j < VmdFaceKeySet->KeyNum ; j ++ )
				{
					if( VmdFaceKeyTemp1->Frame > VmdFaceKey->Frame ) break ;
					VmdFaceKeyTemp2 = VmdFaceKeyTemp1 ;
					VmdFaceKeyTemp1 = VmdFaceKeyTemp1->Next ;
				}
				if( ( DWORD )j == VmdFaceKeySet->KeyNum )
				{
					VmdFaceKey->Prev = VmdFaceKeyTemp2 ;
					VmdFaceKeyTemp2->Next = VmdFaceKey ;
					VmdFaceKey->Next = NULL ;
				}
				else
				{
					VmdFaceKey->Prev = VmdFaceKeyTemp1->Prev ;
					VmdFaceKey->Next = VmdFaceKeyTemp1 ;
					VmdFaceKeyTemp1->Prev = VmdFaceKey ;
					if( VmdFaceKey->Prev == NULL )
					{
						VmdFaceKeySet->FirstKey = VmdFaceKey ;
					}
					else
					{
						VmdFaceKey->Prev->Next = VmdFaceKey ;
					}
				}
			}

			// キーの数を加算する
			VmdFaceKeySet->KeyNum ++ ;

			// 最大フレーム番号と最小フレーム番号を更新
			if( VmdFaceKeySet->MaxFrame < VmdFaceKey->Frame ) VmdFaceKeySet->MaxFrame = VmdFaceKey->Frame ;
			if( VmdFaceKeySet->MinFrame > VmdFaceKey->Frame ) VmdFaceKeySet->MinFrame = VmdFaceKey->Frame ;
		}
	}

	// カメラキーの数を取得
	CameraKeyNum = GET_MEM_DWORD( Src ) ;
	Src += 4 ;
	if( CameraKeyNum != 0 )
	{
		CameraKeyHead = Src ;

		// カメラキーセット用のメモリを確保
		VmdData->Camera = ( VMD_READ_CAMERA_INFO * )DXALLOC( sizeof( VMD_READ_CAMERA_INFO ) ) ;
		if( VmdData->Camera == NULL )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x56\x00\x4d\x00\x44\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xab\x30\xe1\x30\xe9\x30\xad\x30\xfc\x30\xbb\x30\xc3\x30\xc8\x30\xc5\x60\x31\x58\x3c\x68\x0d\x7d\x28\x75\x6e\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"VMD Load Error : カメラキーセット情報格納用のメモリの確保に失敗しました\n" @*/ )) ;
			goto ENDLABEL ;
		}
		_MEMSET( VmdData->Camera, 0, sizeof( VMD_READ_CAMERA_INFO ) ) ;

		VmdData->CamKeyBuffer = ( VMD_READ_CAMERA_KEY_INFO * )DXALLOC( sizeof( VMD_READ_CAMERA_KEY_INFO ) * CameraKeyNum ) ;
		if( VmdData->CamKeyBuffer == NULL )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x56\x00\x4d\x00\x44\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xab\x30\xe1\x30\xe9\x30\xa2\x30\xcb\x30\xe1\x30\xfc\x30\xb7\x30\xe7\x30\xf3\x30\xad\x30\xfc\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"VMD Load Error : カメラアニメーションキーを格納するメモリの確保に失敗しました\n" @*/ )) ;
			goto ENDLABEL ;
		}

		// キーの情報を格納する
		VmdCam = VmdData->Camera ;
		Src = CameraKeyHead ;
		VmdCamKey = VmdData->CamKeyBuffer ;
		for( i = 0 ; ( DWORD )i < CameraKeyNum ; i ++, Src += 61, VmdCamKey ++ )
		{
			VmdCamKey->Frame = GET_MEM_DWORD( &Src[ 0 ] ) ;
			READ_MEM_4BYTE( &VmdCamKey->Length, &Src[ 4 ] ) ;
			READ_MEM_4BYTE( &VmdCamKey->Location[ 0 ], &Src[  8 ] ) ;
			READ_MEM_4BYTE( &VmdCamKey->Location[ 1 ], &Src[ 12 ] ) ;
			READ_MEM_4BYTE( &VmdCamKey->Location[ 2 ], &Src[ 16 ] ) ;
			READ_MEM_4BYTE( &VmdCamKey->Rotate[ 0 ], &Src[ 20 ] ) ;
			READ_MEM_4BYTE( &VmdCamKey->Rotate[ 1 ], &Src[ 24 ] ) ;
			READ_MEM_4BYTE( &VmdCamKey->Rotate[ 2 ], &Src[ 28 ] ) ;

			VmdCamKey->PosXBezier[ 0 ] = GET_MEM_SIGNED_BYTE( &Src[ 32 ] ) / 127.0f * 3.0f ;
			VmdCamKey->PosXBezier[ 2 ] = GET_MEM_SIGNED_BYTE( &Src[ 33 ] ) / 127.0f * 3.0f ;
			VmdCamKey->PosXBezier[ 1 ] = GET_MEM_SIGNED_BYTE( &Src[ 34 ] ) / 127.0f * 3.0f ;
			VmdCamKey->PosXBezier[ 3 ] = GET_MEM_SIGNED_BYTE( &Src[ 35 ] ) / 127.0f * 3.0f ;
			VmdCamKey->PosYBezier[ 0 ] = GET_MEM_SIGNED_BYTE( &Src[ 36 ] ) / 127.0f * 3.0f ;
			VmdCamKey->PosYBezier[ 2 ] = GET_MEM_SIGNED_BYTE( &Src[ 37 ] ) / 127.0f * 3.0f ;
			VmdCamKey->PosYBezier[ 1 ] = GET_MEM_SIGNED_BYTE( &Src[ 38 ] ) / 127.0f * 3.0f ;
			VmdCamKey->PosYBezier[ 3 ] = GET_MEM_SIGNED_BYTE( &Src[ 39 ] ) / 127.0f * 3.0f ;
			VmdCamKey->PosZBezier[ 0 ] = GET_MEM_SIGNED_BYTE( &Src[ 40 ] ) / 127.0f * 3.0f ;
			VmdCamKey->PosZBezier[ 2 ] = GET_MEM_SIGNED_BYTE( &Src[ 41 ] ) / 127.0f * 3.0f ;
			VmdCamKey->PosZBezier[ 1 ] = GET_MEM_SIGNED_BYTE( &Src[ 42 ] ) / 127.0f * 3.0f ;
			VmdCamKey->PosZBezier[ 3 ] = GET_MEM_SIGNED_BYTE( &Src[ 43 ] ) / 127.0f * 3.0f ;
			VmdCamKey->RotBezier[ 0 ]  = GET_MEM_SIGNED_BYTE( &Src[ 44 ] ) / 127.0f * 3.0f ;
			VmdCamKey->RotBezier[ 2 ]  = GET_MEM_SIGNED_BYTE( &Src[ 45 ] ) / 127.0f * 3.0f ;
			VmdCamKey->RotBezier[ 1 ]  = GET_MEM_SIGNED_BYTE( &Src[ 46 ] ) / 127.0f * 3.0f ;
			VmdCamKey->RotBezier[ 3 ]  = GET_MEM_SIGNED_BYTE( &Src[ 47 ] ) / 127.0f * 3.0f ;
			VmdCamKey->LenBezier[ 0 ]  = GET_MEM_SIGNED_BYTE( &Src[ 48 ] ) / 127.0f * 3.0f ;
			VmdCamKey->LenBezier[ 2 ]  = GET_MEM_SIGNED_BYTE( &Src[ 49 ] ) / 127.0f * 3.0f ;
			VmdCamKey->LenBezier[ 1 ]  = GET_MEM_SIGNED_BYTE( &Src[ 50 ] ) / 127.0f * 3.0f ;
			VmdCamKey->LenBezier[ 3 ]  = GET_MEM_SIGNED_BYTE( &Src[ 51 ] ) / 127.0f * 3.0f ;
			VmdCamKey->ViewAngBezier[ 0 ]  = GET_MEM_SIGNED_BYTE( &Src[ 52 ] ) / 127.0f * 3.0f ;
			VmdCamKey->ViewAngBezier[ 2 ]  = GET_MEM_SIGNED_BYTE( &Src[ 53 ] ) / 127.0f * 3.0f ;
			VmdCamKey->ViewAngBezier[ 1 ]  = GET_MEM_SIGNED_BYTE( &Src[ 54 ] ) / 127.0f * 3.0f ;
			VmdCamKey->ViewAngBezier[ 3 ]  = GET_MEM_SIGNED_BYTE( &Src[ 55 ] ) / 127.0f * 3.0f ;

			READ_MEM_4BYTE( &VmdCamKey->ViewingAngle, &Src[ 56 ] ) ;
			VmdCamKey->Perspective = Src[ 60 ] ;

			VmdCamKey->Linear[ 0 ] = 0 ;
			if( *( ( char * )&Src[ 32 ] ) == *( ( char * )&Src[ 34 ] ) &&
				*( ( char * )&Src[ 33 ] ) == *( ( char * )&Src[ 35 ] ) )
			{
				VmdCamKey->Linear[ 0 ] = 1 ;
			}
			VmdCamKey->Linear[ 1 ] = 0 ;
			if( *( ( char * )&Src[ 36 ] ) == *( ( char * )&Src[ 38 ] ) &&
				*( ( char * )&Src[ 37 ] ) == *( ( char * )&Src[ 39 ] ) )
			{
				VmdCamKey->Linear[ 1 ] = 1 ;
			}
			VmdCamKey->Linear[ 2 ] = 0 ;
			if( *( ( char * )&Src[ 40 ] ) == *( ( char * )&Src[ 42 ] ) &&
				*( ( char * )&Src[ 41 ] ) == *( ( char * )&Src[ 43 ] ) )
			{
				VmdCamKey->Linear[ 2 ] = 1 ;
			}
			VmdCamKey->Linear[ 3 ] = 0 ;
			if( *( ( char * )&Src[ 44 ] ) == *( ( char * )&Src[ 46 ] ) &&
				*( ( char * )&Src[ 45 ] ) == *( ( char * )&Src[ 47 ] ) )
			{
				VmdCamKey->Linear[ 3 ] = 1 ;
			}
			VmdCamKey->Linear[ 4 ] = 0 ;
			if( *( ( char * )&Src[ 48 ] ) == *( ( char * )&Src[ 50 ] ) &&
				*( ( char * )&Src[ 49 ] ) == *( ( char * )&Src[ 51 ] ) )
			{
				VmdCamKey->Linear[ 4 ] = 1 ;
			}
			VmdCamKey->Linear[ 5 ] = 0 ;
			if( *( ( char * )&Src[ 52 ] ) == *( ( char * )&Src[ 54 ] ) &&
				*( ( char * )&Src[ 53 ] ) == *( ( char * )&Src[ 55 ] ) )
			{
				VmdCamKey->Linear[ 5 ] = 1 ;
			}

			// キーをリストに追加する
			if( VmdCam->FirstKey == NULL )
			{
				VmdCam->FirstKey = VmdCamKey ;
				VmdCamKey->Prev = NULL ;
				VmdCamKey->Next = NULL ;
			}
			else
			{
				VmdCamKeyTemp1 = VmdCam->FirstKey ;
				VmdCamKeyTemp2 = NULL ;
				for( j = 0 ; ( DWORD )j < VmdCam->KeyNum ; j ++ )
				{
					if( VmdCamKeyTemp1->Frame > VmdCamKey->Frame ) break ;
					VmdCamKeyTemp2 = VmdCamKeyTemp1 ;
					VmdCamKeyTemp1 = VmdCamKeyTemp1->Next ;
				}
				if( ( DWORD )j == VmdCam->KeyNum )
				{
					VmdCamKey->Prev = VmdCamKeyTemp2 ;
					VmdCamKeyTemp2->Next = VmdCamKey ;
					VmdCamKey->Next = NULL ;
				}
				else
				{
					VmdCamKey->Prev = VmdCamKeyTemp1->Prev ;
					VmdCamKey->Next = VmdCamKeyTemp1 ;
					VmdCamKeyTemp1->Prev = VmdCamKey ;
					if( VmdCamKey->Prev == NULL )
					{
						VmdCam->FirstKey = VmdCamKey ;
					}
					else
					{
						VmdCamKey->Prev->Next = VmdCamKey ;
					}
				}
			}

			// キーの数を加算する
			VmdCam->KeyNum ++ ;

			// 最大フレーム番号と最小フレーム番号を更新
			if( VmdCam->MaxFrame < VmdCamKey->Frame ) VmdCam->MaxFrame = VmdCamKey->Frame ;
			if( VmdCam->MinFrame > VmdCamKey->Frame ) VmdCam->MinFrame = VmdCamKey->Frame ;
		}
	}

	// アニメーションの総時間を取得する
	VmdData->MaxTime = 0 ;
	VmdNode = VmdData->Node ;
	for( i = 0 ; ( DWORD )i < VmdData->NodeNum ; i ++, VmdNode ++ )
	{
		if( VmdData->MaxTime < VmdNode->MaxFrame )
			VmdData->MaxTime = VmdNode->MaxFrame ;
	}
	VmdFaceKeySet = VmdData->FaceKeySet ;
	for( i = 0 ; ( DWORD )i < VmdData->FaceKeySetNum ; i ++, VmdFaceKeySet ++ )
	{
		if( VmdData->MaxTime < VmdFaceKeySet->MaxFrame )
			VmdData->MaxTime = VmdFaceKeySet->MaxFrame ;
	}
	if( VmdData->Camera != NULL )
	{
		if( VmdData->MaxTime < VmdData->Camera->MaxFrame )
			VmdData->MaxTime = VmdData->Camera->MaxFrame ;
	}

	// 正常終了
	return 0 ;


	// エラー終了
ENDLABEL :
	TerminateVMDBaseData( VmdData ) ;

	return -1 ;
}

// ＶＭＤファイルの基本情報の後始末をする
extern int TerminateVMDBaseData( VMD_READ_INFO *VmdData )
{
	// 動的に確保したメモリの解放
	if( VmdData->FaceKeySet )
	{
		DXFREE( VmdData->FaceKeySet ) ;
		VmdData->FaceKeySet = NULL ;
	}

	if( VmdData->Node )
	{
		DXFREE( VmdData->Node ) ;
		VmdData->Node = NULL ;
	}

	if( VmdData->Camera )
	{
		DXFREE( VmdData->Camera ) ;
		VmdData->Camera = NULL ;
	}

	if( VmdData->KeyBuffer )
	{
		DXFREE( VmdData->KeyBuffer ) ;
		VmdData->KeyBuffer = NULL ;
	}

	if( VmdData->FaceKeyBuffer )
	{
		DXFREE( VmdData->FaceKeyBuffer ) ;
		VmdData->FaceKeyBuffer = NULL ;
	}

	if( VmdData->CamKeyBuffer )
	{
		DXFREE( VmdData->CamKeyBuffer ) ;
		VmdData->CamKeyBuffer = NULL ;
	}

	// 終了
	return 0 ;
}


// カメラのアニメーションを読み込みようデータに追加する
extern int SetupVMDCameraAnim( VMD_READ_INFO *VmdData, MV1_MODEL_R *RModel, const wchar_t *Name, MV1_ANIMSET_R *AnimSet )
{
	MV1_FRAME_R *Frame ;
	MV1_ANIM_R *Anim ;
	VMD_READ_NODE_INFO *VmdNode ;
	VMD_READ_CAMERA_KEY_INFO *VmdCamKey, *VmdCamKeyTemp1, *VmdCamKeyTemp2 ;
	MV1_ANIMKEYSET_R *KeyPosSet, *KeyRotSet, *KeyLenSet, *KeyViewAngSet, *KeyPersSet ;
	float *KeyPosTime, *KeyRotTime, *KeyLenTime, *KeyViewAngTime, *KeyPersTime ;
	float *KeyLen, *KeyViewAng, *KeyPers ;
	VECTOR *KeyPos, *KeyRotOir ;
	DWORD j, k ;

	// カメラのモーション情報がある場合はカメラ情報を追加する
	if( VmdData->Camera == NULL )
		return 0 ;

	// VmdNode のアドレスをセット
	VmdNode = VmdData->Node ;

	// カメラノードの追加
	Frame = MV1RAddFrameW( RModel, Name, NULL ) ;
	if( Frame == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x56\x00\x4d\x00\x44\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xab\x30\xe1\x30\xe9\x30\x28\x75\xd5\x30\xec\x30\xfc\x30\xe0\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\xfd\x8f\xa0\x52\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"VMD Load Error : カメラ用フレームオブジェクトの追加に失敗しました\n" @*/ )) ;
		goto ENDLABEL ;
	}

	// アニメーションの追加
	Anim = MV1RAddAnim( RModel, AnimSet ) ;
	if( Anim == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x56\x00\x4d\x00\x44\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xab\x30\xe1\x30\xe9\x30\x28\x75\xa2\x30\xcb\x30\xe1\x30\xfc\x30\xb7\x30\xe7\x30\xf3\x30\x6e\x30\xfd\x8f\xa0\x52\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"VMD Load Error : カメラ用アニメーションの追加に失敗しました\n" @*/ )) ;
		goto ENDLABEL ;
	}

	// 対象ノードのセット
	Anim->TargetFrameIndex = Frame->Index ;

	// 最大時間をセット
	Anim->MaxTime = ( float )VmdData->Camera->MaxFrame ;

	// アニメーションキーの情報をセット
	KeyPosSet = MV1RAddAnimKeySet( RModel, Anim ) ;
	if( KeyPosSet == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x56\x00\x4d\x00\x44\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xa2\x30\xcb\x30\xe1\x30\xfc\x30\xb7\x30\xe7\x30\xf3\x30\xab\x30\xe1\x30\xe9\x30\xa7\x5e\x19\x6a\xad\x30\xfc\x30\xbb\x30\xc3\x30\xc8\x30\x6e\x30\xfd\x8f\xa0\x52\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"VMD Load Error : アニメーションカメラ座標キーセットの追加に失敗しました\n" @*/ )) ;
		goto ENDLABEL ;
	}
	KeyRotSet = MV1RAddAnimKeySet( RModel, Anim ) ;
	if( KeyRotSet == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x56\x00\x4d\x00\x44\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xa2\x30\xcb\x30\xe1\x30\xfc\x30\xb7\x30\xe7\x30\xf3\x30\xab\x30\xe1\x30\xe9\x30\xde\x56\xe2\x8e\xad\x30\xfc\x30\xbb\x30\xc3\x30\xc8\x30\x6e\x30\xfd\x8f\xa0\x52\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"VMD Load Error : アニメーションカメラ回転キーセットの追加に失敗しました\n" @*/ )) ;
		goto ENDLABEL ;
	}
	KeyLenSet = MV1RAddAnimKeySet( RModel, Anim ) ;
	if( KeyLenSet == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x56\x00\x4d\x00\x44\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xa2\x30\xcb\x30\xe1\x30\xfc\x30\xb7\x30\xe7\x30\xf3\x30\xab\x30\xe1\x30\xe9\x30\xdd\x8d\xe2\x96\xad\x30\xfc\x30\xbb\x30\xc3\x30\xc8\x30\x6e\x30\xfd\x8f\xa0\x52\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"VMD Load Error : アニメーションカメラ距離キーセットの追加に失敗しました\n" @*/ )) ;
		goto ENDLABEL ;
	}
	KeyViewAngSet = MV1RAddAnimKeySet( RModel, Anim ) ;
	if( KeyLenSet == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x56\x00\x4d\x00\x44\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xa2\x30\xcb\x30\xe1\x30\xfc\x30\xb7\x30\xe7\x30\xf3\x30\xab\x30\xe1\x30\xe9\x30\xdd\x8d\xe2\x96\xad\x30\xfc\x30\xbb\x30\xc3\x30\xc8\x30\x6e\x30\xfd\x8f\xa0\x52\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"VMD Load Error : アニメーションカメラ距離キーセットの追加に失敗しました\n" @*/ )) ;
		goto ENDLABEL ;
	}
	KeyPersSet = MV1RAddAnimKeySet( RModel, Anim ) ;
	if( KeyLenSet == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x56\x00\x4d\x00\x44\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xa2\x30\xcb\x30\xe1\x30\xfc\x30\xb7\x30\xe7\x30\xf3\x30\xab\x30\xe1\x30\xe9\x30\xdd\x8d\xe2\x96\xad\x30\xfc\x30\xbb\x30\xc3\x30\xc8\x30\x6e\x30\xfd\x8f\xa0\x52\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"VMD Load Error : アニメーションカメラ距離キーセットの追加に失敗しました\n" @*/ )) ;
		goto ENDLABEL ;
	}

	KeyPosSet->Type = MV1_ANIMKEY_TYPE_VECTOR ;
	KeyPosSet->DataType = MV1_ANIMKEY_DATATYPE_TRANSLATE ;
	KeyPosSet->TimeType = MV1_ANIMKEY_TIME_TYPE_KEY ;
	KeyPosSet->TotalTime = ( float )VmdNode->MaxFrame ;
	KeyPosSet->Num = ( int )( ( VmdData->Camera->MaxFrame - VmdData->Camera->MinFrame ) + 1 ) ;

	KeyRotSet->Type = MV1_ANIMKEY_TYPE_VECTOR ;
	KeyRotSet->DataType = MV1_ANIMKEY_DATATYPE_ROTATE ;
	KeyRotSet->TimeType = MV1_ANIMKEY_TIME_TYPE_KEY ;
	KeyRotSet->TotalTime = ( float )VmdNode->MaxFrame ;
	KeyRotSet->Num = KeyPosSet->Num ;

	KeyLenSet->Type = MV1_ANIMKEY_TYPE_LINEAR ;
	KeyLenSet->DataType = MV1_ANIMKEY_DATATYPE_OTHRE ;
	KeyLenSet->TimeType = MV1_ANIMKEY_TIME_TYPE_KEY ;
	KeyLenSet->TotalTime = ( float )VmdNode->MaxFrame ;
	KeyLenSet->Num = KeyPosSet->Num ;

	KeyViewAngSet->Type = MV1_ANIMKEY_TYPE_LINEAR ;
	KeyViewAngSet->DataType = MV1_ANIMKEY_DATATYPE_OTHRE ;
	KeyViewAngSet->TimeType = MV1_ANIMKEY_TIME_TYPE_KEY ;
	KeyViewAngSet->TotalTime = ( float )VmdNode->MaxFrame ;
	KeyViewAngSet->Num = KeyPosSet->Num ;

	KeyPersSet->Type = MV1_ANIMKEY_TYPE_FLAT ;
	KeyPersSet->DataType = MV1_ANIMKEY_DATATYPE_OTHRE ;
	KeyPersSet->TimeType = MV1_ANIMKEY_TIME_TYPE_KEY ;
	KeyPersSet->TotalTime = ( float )VmdNode->MaxFrame ;
	KeyPersSet->Num = KeyPosSet->Num ;

	KeyPosSet->KeyTime = ( float * )ADDMEMAREA( sizeof( float ) * KeyPosSet->Num, &RModel->Mem ) ;
	if( KeyPosSet->KeyTime == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x56\x00\x4d\x00\x44\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xab\x30\xe1\x30\xe9\x30\xa7\x5e\x19\x6a\xa2\x30\xcb\x30\xe1\x30\xfc\x30\xb7\x30\xe7\x30\xf3\x30\xad\x30\xfc\x30\xbf\x30\xa4\x30\xe0\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"VMD Load Error : カメラ座標アニメーションキータイムを格納するメモリ領域の確保に失敗しました\n" @*/ )) ;
		goto ENDLABEL ;
	}
	KeyPosSet->KeyVector = ( VECTOR * )ADDMEMAREA( sizeof( VECTOR ) * KeyPosSet->Num, &RModel->Mem ) ;
	if( KeyPosSet->KeyVector == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x56\x00\x4d\x00\x44\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xab\x30\xe1\x30\xe9\x30\xa7\x5e\x19\x6a\xa2\x30\xcb\x30\xe1\x30\xfc\x30\xb7\x30\xe7\x30\xf3\x30\xad\x30\xfc\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"VMD Load Error : カメラ座標アニメーションキーを格納するメモリ領域の確保に失敗しました\n" @*/ )) ;
		goto ENDLABEL ;
	}

	KeyRotSet->KeyTime = ( float * )ADDMEMAREA( sizeof( float ) * KeyRotSet->Num, &RModel->Mem ) ;
	if( KeyRotSet->KeyTime == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x56\x00\x4d\x00\x44\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\xab\x30\xe1\x30\xe9\x30\xde\x56\xe2\x8e\xa2\x30\xcb\x30\xe1\x30\xfc\x30\xb7\x30\xe7\x30\xf3\x30\xad\x30\xfc\x30\xbf\x30\xa4\x30\xe0\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"VMD Load Error :カメラ回転アニメーションキータイムを格納するメモリ領域の確保に失敗しました\n" @*/ )) ;
		goto ENDLABEL ;
	}
	KeyRotSet->KeyVector = ( VECTOR * )ADDMEMAREA( sizeof( VECTOR ) * KeyRotSet->Num, &RModel->Mem ) ;
	if( KeyRotSet->KeyVector == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x56\x00\x4d\x00\x44\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xab\x30\xe1\x30\xe9\x30\xde\x56\xe2\x8e\xa2\x30\xcb\x30\xe1\x30\xfc\x30\xb7\x30\xe7\x30\xf3\x30\xad\x30\xfc\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"VMD Load Error : カメラ回転アニメーションキーを格納するメモリ領域の確保に失敗しました\n" @*/ )) ;
		goto ENDLABEL ;
	}

	KeyLenSet->KeyTime = ( float * )ADDMEMAREA( sizeof( float ) * KeyLenSet->Num, &RModel->Mem ) ;
	if( KeyLenSet->KeyTime == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x56\x00\x4d\x00\x44\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\xab\x30\xe1\x30\xe9\x30\xdd\x8d\xe2\x96\xa2\x30\xcb\x30\xe1\x30\xfc\x30\xb7\x30\xe7\x30\xf3\x30\xad\x30\xfc\x30\xbf\x30\xa4\x30\xe0\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"VMD Load Error :カメラ距離アニメーションキータイムを格納するメモリ領域の確保に失敗しました\n" @*/ )) ;
		goto ENDLABEL ;
	}
	KeyLenSet->KeyLinear = ( float * )ADDMEMAREA( sizeof( float ) * KeyLenSet->Num, &RModel->Mem ) ;
	if( KeyLenSet->KeyVector == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x56\x00\x4d\x00\x44\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xab\x30\xe1\x30\xe9\x30\xdd\x8d\xe2\x96\xa2\x30\xcb\x30\xe1\x30\xfc\x30\xb7\x30\xe7\x30\xf3\x30\xad\x30\xfc\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"VMD Load Error : カメラ距離アニメーションキーを格納するメモリ領域の確保に失敗しました\n" @*/ )) ;
		goto ENDLABEL ;
	}

	KeyViewAngSet->KeyTime = ( float * )ADDMEMAREA( sizeof( float ) * KeyViewAngSet->Num, &RModel->Mem ) ;
	if( KeyViewAngSet->KeyTime == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x56\x00\x4d\x00\x44\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\xab\x30\xe1\x30\xe9\x30\x96\x89\xce\x91\xd2\x89\xa2\x30\xcb\x30\xe1\x30\xfc\x30\xb7\x30\xe7\x30\xf3\x30\xad\x30\xfc\x30\xbf\x30\xa4\x30\xe0\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"VMD Load Error :カメラ視野角アニメーションキータイムを格納するメモリ領域の確保に失敗しました\n" @*/ )) ;
		goto ENDLABEL ;
	}
	KeyViewAngSet->KeyLinear = ( float * )ADDMEMAREA( sizeof( float ) * KeyViewAngSet->Num, &RModel->Mem ) ;
	if( KeyViewAngSet->KeyVector == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x56\x00\x4d\x00\x44\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xab\x30\xe1\x30\xe9\x30\x96\x89\xce\x91\xd2\x89\xa2\x30\xcb\x30\xe1\x30\xfc\x30\xb7\x30\xe7\x30\xf3\x30\xad\x30\xfc\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"VMD Load Error : カメラ視野角アニメーションキーを格納するメモリ領域の確保に失敗しました\n" @*/ )) ;
		goto ENDLABEL ;
	}

	KeyPersSet->KeyTime = ( float * )ADDMEMAREA( sizeof( float ) * KeyPersSet->Num, &RModel->Mem ) ;
	if( KeyPersSet->KeyTime == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x56\x00\x4d\x00\x44\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\xab\x30\xe1\x30\xe9\x30\x04\x5c\x71\x5f\xa2\x30\xcb\x30\xe1\x30\xfc\x30\xb7\x30\xe7\x30\xf3\x30\xad\x30\xfc\x30\xbf\x30\xa4\x30\xe0\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"VMD Load Error :カメラ射影アニメーションキータイムを格納するメモリ領域の確保に失敗しました\n" @*/ )) ;
		goto ENDLABEL ;
	}
	KeyPersSet->KeyFlat = ( float * )ADDMEMAREA( sizeof( float ) * KeyPersSet->Num, &RModel->Mem ) ;
	if( KeyPersSet->KeyVector == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x56\x00\x4d\x00\x44\x00\x20\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x20\x00\x45\x00\x72\x00\x72\x00\x6f\x00\x72\x00\x20\x00\x3a\x00\x20\x00\xab\x30\xe1\x30\xe9\x30\x04\x5c\x71\x5f\xa2\x30\xcb\x30\xe1\x30\xfc\x30\xb7\x30\xe7\x30\xf3\x30\xad\x30\xfc\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"VMD Load Error : カメラ射影アニメーションキーを格納するメモリ領域の確保に失敗しました\n" @*/ )) ;
		goto ENDLABEL ;
	}

	RModel->AnimKeyDataSize += KeyRotSet->Num * ( sizeof( float ) * 5 + sizeof( VECTOR ) + sizeof( VECTOR ) + sizeof( float ) + sizeof( float ) + sizeof( float ) ) ;

	VmdCamKey = VmdData->Camera->FirstKey ;
	KeyPos = KeyPosSet->KeyVector ;
	KeyRotOir = KeyRotSet->KeyVector ;
	KeyLen = KeyLenSet->KeyLinear ;
	KeyViewAng = KeyViewAngSet->KeyLinear ;
	KeyPers = KeyPersSet->KeyFlat ;
	KeyPosTime = KeyPosSet->KeyTime ;
	KeyRotTime = KeyRotSet->KeyTime ;
	KeyLenTime = KeyLenSet->KeyTime ;
	KeyViewAngTime = KeyViewAngSet->KeyTime ;
	KeyPersTime = KeyPersSet->KeyTime ;
	if( VmdData->Camera->KeyNum == 1 )
	{
		*KeyPosTime = ( float )VmdCamKey->Frame ;
		*KeyRotTime = ( float )VmdCamKey->Frame ;

		KeyPos->x = VmdCamKey->Location[ 0 ] ;
		KeyPos->y = VmdCamKey->Location[ 1 ] ;
		KeyPos->z = VmdCamKey->Location[ 2 ] ;
		KeyRotOir->x = VmdCamKey->Rotate[ 0 ] ;
		KeyRotOir->y = VmdCamKey->Rotate[ 1 ] ;
		KeyRotOir->z = VmdCamKey->Rotate[ 2 ] ;
		*KeyLen = VmdCamKey->Length ;
		*KeyViewAng = ( float )VmdCamKey->ViewingAngle ;
		*KeyPers = ( float )VmdCamKey->Perspective ;
	}
	else
	{
		for( j = 0 ; ( DWORD )j < VmdData->Camera->KeyNum ; j ++, VmdCamKey = VmdCamKey->Next )
		{
			int XLinear, YLinear, ZLinear, RLinear, LLinear, VLinear ;
			float XX1, XY1, XX2, XY2 ;
			float YX1, YY1, YX2, YY2 ;
			float ZX1, ZY1, ZX2, ZY2 ;
			float RX1, RY1, RX2, RY2 ;
			float LX1, LY1, LX2, LY2 ;
			float VX1, VY1, VX2, VY2 ;

			if( j == 0 ) continue ;

			VmdCamKeyTemp1 = VmdCamKey->Prev ;
			VmdCamKeyTemp2 = VmdCamKey ;

			XLinear = VmdCamKeyTemp2->Linear[ 0 ] ;
			YLinear = VmdCamKeyTemp2->Linear[ 1 ] ;
			ZLinear = VmdCamKeyTemp2->Linear[ 2 ] ;
			RLinear = VmdCamKeyTemp2->Linear[ 3 ] ;
			LLinear = VmdCamKeyTemp2->Linear[ 4 ] ;
			VLinear = VmdCamKeyTemp2->Linear[ 5 ] ;

			XX1 = VmdCamKeyTemp2->PosXBezier[ 0 ] ;
			XY1 = VmdCamKeyTemp2->PosXBezier[ 1 ] ;
			XX2 = VmdCamKeyTemp2->PosXBezier[ 2 ] ;
			XY2 = VmdCamKeyTemp2->PosXBezier[ 3 ] ;
			YX1 = VmdCamKeyTemp2->PosYBezier[ 0 ] ;
			YY1 = VmdCamKeyTemp2->PosYBezier[ 1 ] ;
			YX2 = VmdCamKeyTemp2->PosYBezier[ 2 ] ;
			YY2 = VmdCamKeyTemp2->PosYBezier[ 3 ] ;
			ZX1 = VmdCamKeyTemp2->PosZBezier[ 0 ] ;
			ZY1 = VmdCamKeyTemp2->PosZBezier[ 1 ] ;
			ZX2 = VmdCamKeyTemp2->PosZBezier[ 2 ] ;
			ZY2 = VmdCamKeyTemp2->PosZBezier[ 3 ] ;
			RX1 = VmdCamKeyTemp2->RotBezier[ 0 ] ;
			RY1 = VmdCamKeyTemp2->RotBezier[ 1 ] ;
			RX2 = VmdCamKeyTemp2->RotBezier[ 2 ] ;
			RY2 = VmdCamKeyTemp2->RotBezier[ 3 ] ;
			LX1 = VmdCamKeyTemp2->LenBezier[ 0 ] ;
			LY1 = VmdCamKeyTemp2->LenBezier[ 1 ] ;
			LX2 = VmdCamKeyTemp2->LenBezier[ 2 ] ;
			LY2 = VmdCamKeyTemp2->LenBezier[ 3 ] ;
			VX1 = VmdCamKeyTemp2->ViewAngBezier[ 0 ] ;
			VY1 = VmdCamKeyTemp2->ViewAngBezier[ 1 ] ;
			VX2 = VmdCamKeyTemp2->ViewAngBezier[ 2 ] ;
			VY2 = VmdCamKeyTemp2->ViewAngBezier[ 3 ] ;

			for( k = VmdCamKeyTemp1->Frame ; ( DWORD )k < VmdCamKeyTemp2->Frame ; k ++ )
			{
				float Rate, RateH ;

				Rate = ( k - VmdCamKeyTemp1->Frame ) / ( float )( VmdCamKeyTemp2->Frame - VmdCamKeyTemp1->Frame ) ;
				*KeyPosTime = ( float )k ;
				*KeyRotTime = ( float )k ;
				*KeyLenTime = ( float )k ;
				*KeyViewAngTime = ( float )k ;
				*KeyPersTime = ( float )k ;

				VmdCalcLine( XLinear, Rate, RateH, XX1, XX2, XY1, XY2 ) ;
				KeyPos->x = VmdCamKeyTemp1->Location[ 0 ] + ( VmdCamKeyTemp2->Location[ 0 ] - VmdCamKeyTemp1->Location[ 0 ] ) * RateH + Frame->Translate.x ;

				VmdCalcLine( YLinear, Rate, RateH, YX1, YX2, YY1, YY2 ) ;
				KeyPos->y = VmdCamKeyTemp1->Location[ 1 ] + ( VmdCamKeyTemp2->Location[ 1 ] - VmdCamKeyTemp1->Location[ 1 ] ) * RateH + Frame->Translate.y ;

				VmdCalcLine( ZLinear, Rate, RateH, ZX1, ZX2, ZY1, ZY2 ) ;
				KeyPos->z = VmdCamKeyTemp1->Location[ 2 ] + ( VmdCamKeyTemp2->Location[ 2 ] - VmdCamKeyTemp1->Location[ 2 ] ) * RateH + Frame->Translate.z ;

				VmdCalcLine( RLinear, Rate, RateH, RX1, RX2, RY1, RY2 ) ;
				KeyRotOir->x = VmdCamKeyTemp1->Rotate[ 0 ] + ( VmdCamKeyTemp2->Rotate[ 0 ] - VmdCamKeyTemp1->Rotate[ 0 ] ) * RateH ;
				KeyRotOir->y = VmdCamKeyTemp1->Rotate[ 1 ] + ( VmdCamKeyTemp2->Rotate[ 1 ] - VmdCamKeyTemp1->Rotate[ 1 ] ) * RateH ;
				KeyRotOir->z = VmdCamKeyTemp1->Rotate[ 2 ] + ( VmdCamKeyTemp2->Rotate[ 2 ] - VmdCamKeyTemp1->Rotate[ 2 ] ) * RateH ;

				VmdCalcLine( LLinear, Rate, RateH, LX1, LX2, LY1, LY2 ) ;
				*KeyLen = VmdCamKeyTemp1->Length + ( VmdCamKeyTemp2->Length - VmdCamKeyTemp1->Length ) * RateH ;

				VmdCalcLine( VLinear, Rate, RateH, VX1, VX2, VY1, VY2 ) ;
				*KeyViewAng = ( float )( ( int )VmdCamKeyTemp1->ViewingAngle + ( ( int )VmdCamKeyTemp2->ViewingAngle - ( int )VmdCamKeyTemp1->ViewingAngle ) * RateH ) ;

				*KeyPers = ( float )VmdCamKeyTemp1->Perspective ;

				KeyPos ++ ;
				KeyRotOir ++ ;
				KeyLen ++ ;
				KeyViewAng ++ ;
				KeyPers ++ ;

				KeyPosTime ++ ;
				KeyRotTime ++ ;
				KeyLenTime ++ ;
				KeyViewAngTime ++ ;
				KeyPersTime ++ ;
			}
			if( ( DWORD )j == VmdData->Camera->KeyNum - 1 )
			{
				*KeyPosTime = ( float )VmdCamKey->Frame ;
				*KeyRotTime = ( float )VmdCamKey->Frame ;
				*KeyLenTime = ( float )VmdCamKey->Frame ;
				*KeyViewAngTime = ( float )VmdCamKey->Frame ;
				*KeyPersTime = ( float )VmdCamKey->Frame ;
				KeyPos->x = VmdCamKey->Location[ 0 ] ;
				KeyPos->y = VmdCamKey->Location[ 1 ] ;
				KeyPos->z = VmdCamKey->Location[ 2 ] ;
				KeyRotOir->x = VmdCamKey->Rotate[ 0 ] ;
				KeyRotOir->y = VmdCamKey->Rotate[ 1 ] ;
				KeyRotOir->z = VmdCamKey->Rotate[ 2 ] ;
				*KeyLen = VmdCamKey->Length ;
				*KeyViewAng = ( float )VmdCamKey->ViewingAngle ;
				*KeyPers = ( float )VmdCamKey->Perspective ;

				KeyPos ++ ;
				KeyRotOir ++ ;
				KeyLen ++ ;
				KeyViewAng ++ ;
				KeyPers ++ ;

				KeyPosTime ++ ;
				KeyRotTime ++ ;
				KeyLenTime ++ ;
				KeyViewAngTime ++ ;
				KeyPersTime ++ ;
			}
		}
	}

	// 正常終了
	return 0 ;

ENDLABEL:

	// エラー終了
	return -1 ;
}

// VMDファイルバイナリをメモリに読み込む
extern int LoadFile_VMD(
	void **					VmdData,
	int *					FileSize, 
	int						FileNumber,
	MV1_FILE_READ_FUNC *	FileReadFunc,
	const wchar_t *			Name,
	int						NameLen,
	const wchar_t *			CurrentDir,
	int *					LoopMotionFlag,
	int *					DisablePhysicsFlag,
	float *					BaseGravity,
	int *					GravityNo,
	int *					GravityEnable,
	VECTOR *				Gravity
)
{
	int j ;
	int itemp ;
	wchar_t *Strp ;
	int GravityLoopEnd ;
	int GravityNoStr[ 64 ] ;
	int GravityNoNum ;
	wchar_t StringW[ 1024 ] ;
#ifndef UNICODE
	char StringA[ 2048 ] ;
#endif
	DWORD_PTR FileHandle ;
	DWORD_PTR FindHandle ;
	FILEINFOW FindData ;

	*LoopMotionFlag = FALSE ;
	*DisablePhysicsFlag = FALSE ;
	*GravityNo = -1 ;
	*GravityEnable = FALSE ;
	Gravity->x = 0.0f ;
	Gravity->y = 0.0f ;
	Gravity->z = 0.0f ;
	if( CurrentDir == NULL )
	{
		_SWNPRINTF( StringW, sizeof( StringW ) / 2, L"%s%03dL.vmd", Name, FileNumber ) ;

#ifdef UNICODE
		if( FileReadFunc->Read( StringW, VmdData, FileSize, FileReadFunc->Data ) != -1 )
#else
		ConvString( ( const char * )StringW, -1, WCHAR_T_CHARCODEFORMAT, StringA, sizeof( StringA ), _TCHARCODEFORMAT ) ;
		if( FileReadFunc->Read( StringA, VmdData, FileSize, FileReadFunc->Data ) != -1 )
#endif
		{
			*LoopMotionFlag = TRUE ;
		}
		else
		{
			_SWNPRINTF( StringW, sizeof( StringW ) / 2, L"%s%03d.vmd", Name, FileNumber ) ;
#ifdef UNICODE
			if( FileReadFunc->Read( StringW, VmdData, FileSize, FileReadFunc->Data ) == -1 )
#else
			ConvString( ( const char * )StringW, -1, WCHAR_T_CHARCODEFORMAT, StringA, sizeof( StringA ), _TCHARCODEFORMAT ) ;
			if( FileReadFunc->Read( StringA, VmdData, FileSize, FileReadFunc->Data ) == -1 )
#endif
			{
				return FALSE ;
			}
		}
	}
	else
	{
		_SWNPRINTF( StringW, sizeof( StringW ) / 2, L"%s\\%s%03d*.vmd", CurrentDir, Name, FileNumber ) ;

		FindHandle = DX_FFINDFIRST( StringW, &FindData ) ;
		if( FindHandle == ( DWORD_PTR )-1 )
			return FALSE ;

		Strp = &FindData.Name[ NameLen + 3 ] ;
		while( *Strp != _T( '.' ) )
		{
			switch( *Strp )
			{
			case _T( 'N' ) :
				Strp ++ ;
				if( *Strp == _T( 'P' ) )
				{
					*DisablePhysicsFlag = TRUE ;
				}
				break ;

			case _T( 'L' ) :
				*LoopMotionFlag = TRUE ;
				break ;

			case _T( 'G' ) :
				Strp ++ ;

				if( *Strp >= _T( '0' ) && *Strp <= _T( '9' ) )
				{
					for( GravityNoNum = 0 ; *Strp >= _T( '0' ) && *Strp <= _T( '9' ) ; GravityNoNum ++, Strp ++ )
					{
						GravityNoStr[ GravityNoNum ] = *Strp - _T( '0' ) ;
					}
					*GravityNo = 0 ;
					itemp = 1 ;
					for( j = 0 ; j < GravityNoNum ; j ++, itemp *= 10 )
					{
						*GravityNo += itemp * GravityNoStr[ GravityNoNum - j - 1 ] ;
					}

					if( *GravityNo >= MV1_LOADCALC_PHYSICS_GRAVITY_NUM )
					{
						*GravityNo = -1 ;
					}
				}
				else
				{
					*GravityEnable = TRUE ;

					if( *BaseGravity < 0.0f )
						*BaseGravity = -*BaseGravity ;
					GravityLoopEnd = FALSE ;
					while( GravityLoopEnd == FALSE )
					{
						switch( *Strp )
						{
						case _T( 'X' ) :
							Strp ++ ;
							if( *Strp == _T( 'M' ) )
							{
								Gravity->x = -*BaseGravity ;
								Strp ++ ;
							}
							else
							if( *Strp == _T( 'P' ) )
							{
								Gravity->x = *BaseGravity ;
								Strp ++ ;
							}
							break ;

						case _T( 'Y' ) :
							Strp ++ ;
							if( *Strp == _T( 'M' ) )
							{
								Gravity->y = -*BaseGravity ;
								Strp ++ ;
							}
							else
							if( *Strp == _T( 'P' ) )
							{
								Gravity->y = *BaseGravity ;
								Strp ++ ;
							}
							break ;

						case _T( 'Z' ) :
							Strp ++ ;
							if( *Strp == _T( 'M' ) )
							{
								Gravity->z = -*BaseGravity ;
								Strp ++ ;
							}
							else
							if( *Strp == _T( 'P' ) )
							{
								Gravity->z = *BaseGravity ;
								Strp ++ ;
							}
							break ;

						default :
							GravityLoopEnd = TRUE ;
							break ;
						}
					}
				}

				Strp -- ;
				break ;
			}

			Strp ++ ;
		}

		_SWNPRINTF( StringW, sizeof( StringW ) / 2, L"%s\\%s", CurrentDir, FindData.Name ) ;

		DX_FFINDCLOSE( FindHandle ) ;

		FileHandle = DX_FOPEN( StringW ) ;
		if( FileHandle == 0 )
			return FALSE ;

		DX_FSEEK( FileHandle, 0, SEEK_END ) ;
		*FileSize = ( int )DX_FTELL( FileHandle ) ;
		DX_FSEEK( FileHandle, 0, SEEK_SET ) ;

		*VmdData = DXALLOC( ( size_t )*FileSize ) ;
		if( *VmdData == NULL )
		{
			DX_FCLOSE( FileHandle ) ;
			return FALSE ;
		}

		DX_FREAD( *VmdData, *FileSize, 1, FileHandle ) ;
		DX_FCLOSE( FileHandle ) ;
	}

	// 終了
	return TRUE ;
}







#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif
