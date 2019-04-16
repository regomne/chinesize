// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		ＶＭＤファイルデータ構造体ヘッダ
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXMODELLOADERVMD_H__
#define __DXMODELLOADERVMD_H__

#include "DxCompileConfig.h"

#ifndef DX_NON_MODEL

// インクルード ---------------------------------
#include "DxLib.h"
#include "DxStatic.h"
#include "DxModel.h"
#include "DxModelRead.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 -----------------------------------

// データ型定義 ---------------------------------

// VMDキーデータ( 111byte )
struct VMD_KEY
{
	BYTE	Data[ 111 ] ;
/*
	char	Name[ 15 ] ;						// 名前
	DWORD	Frame ;								// フレーム
	float	Position[ 3 ] ;						// 座標
	float	Quaternion[ 4 ] ;					// クォータニオン
	float	PosXBezier[ 4 ] ;					// 座標Ｘ用ベジェ曲線情報
	float	PosYBezier[ 4 ] ;					// 座標Ｙ用ベジェ曲線情報
	float	PosZBezier[ 4 ] ;					// 座標Ｚ用ベジェ曲線情報
	float	RotBezier[ 4 ] ;					// 回転用ベジェ曲線情報
*/
} ;

// VMD表情キーデータ( 23byte )
struct VMD_FACE_KEY
{
	BYTE	Data[ 23 ] ;						// データ
/*
	char	Name[ 15 ] ;						// 表情名
	DWORD	Frame ;								// フレーム
	float	Factor ;							// ブレンド率
*/
} ;

struct VMD_CAMERA_LENGTH
{
	DWORD Count;
};

// VMDカメラキーデータ
struct VMD_CAMERA 
{
	BYTE	Data[ 61 ] ;						// データ
/*
	DWORD	FrameNo;							//  4:  0:フレーム番号
	float	Length;								//  8:  4: -(距離)
	float	Location[3];						// 20:  8:位置
	float	Rotate[3];							// 32: 20:オイラー角 // X軸は符号が反転しているので注意
	BYTE	Interpolation[24];					// 56: 32:補間情報 // おそらく[6][4](未検証)
	DWORD	ViewingAngle;						// 60: 56:向き
	BYTE	Perspective;						// 61: 60:射影カメラかどうか 0:射影カメラ 1:正射影カメラ
*/
};

struct VMD_LIGHT_LENGTH
{
	DWORD Count;
};

struct VMD_LIGHT 
{
	DWORD flameNo;
	DWORD RGB[3];
	DWORD Loc[3];
}; 

// VMD読みこみ処理用キー構造体
struct VMD_READ_KEY_INFO
{
	DWORD	Frame ;								// フレーム
	DWORD	DisablePhysics ;					// 物理無効
	float	Position[ 3 ] ;						// 座標
	float	Quaternion[ 4 ] ;					// クォータニオン
	int		Linear[ 4 ] ;						// 線形かどうか
	float	PosXBezier[ 4 ] ;					// 座標Ｘ用ベジェ曲線情報
	float	PosYBezier[ 4 ] ;					// 座標Ｙ用ベジェ曲線情報
	float	PosZBezier[ 4 ] ;					// 座標Ｚ用ベジェ曲線情報
	float	RotBezier[ 4 ] ;					// 回転用ベジェ曲線情報
	VECTOR	*MVRPosKey ;						// 算出した座標キーへのポインタ
	FLOAT4	*MVRRotKey ;						// 算出した回転キーへのポインタ
	float	*MVRDisablePhysicsKey ;				// 算出した物理無効キーへのポインタ
	VMD_READ_KEY_INFO *Prev ;					// 前のキーへのアドレス
	VMD_READ_KEY_INFO *Next ;					// 次のキーへのアドレス
} ;

// VMD読み込み処理用ノード構造体
struct VMD_READ_NODE_INFO
{
	DWORD				KeyNum ;				// キーの数
	DWORD				MaxFrame ;				// 最大フレーム番号
	DWORD				MinFrame ;				// 最小フレーム番号
	char				Name[ 16 ] ;			// 名前
	VMD_READ_KEY_INFO	*FirstKey ;				// キーリストの先頭
} ;

// VMD読みこみ処理用表情キー構造体
struct VMD_READ_FACE_KEY_INFO
{
	DWORD	Frame ;								// フレーム
	float	Factor ;							// ブレンド率
	VMD_READ_FACE_KEY_INFO *Prev ;				// 前のキーへのアドレス
	VMD_READ_FACE_KEY_INFO *Next ;				// 次のキーへのアドレス
} ;

// VMD読み込み処理用表情別情報構造体
struct VMD_READ_FACE_KEY_SET_INFO
{
	DWORD					KeyNum ;			// キーの数
	DWORD					MaxFrame ;			// 最大フレーム番号
	DWORD					MinFrame ;			// 最小フレーム番号
	char					Name[ 16 ] ;		// 名前
	VMD_READ_FACE_KEY_INFO	*FirstKey ;			// キーリストの先頭
} ;


// VMD読み込み処理用カメラキー構造体
struct VMD_READ_CAMERA_KEY_INFO
{
	DWORD	Frame;								//  4:  0:フレーム番号
	float	Length;								//  8:  4: -(距離)
	float	Location[3];						// 20:  8:位置
	float	Rotate[3];							// 32: 20:オイラー角 // X軸は符号が反転しているので注意
//	BYTE	Interpolation[24];					// 56: 32:補間情報 // おそらく[6][4](未検証)
	int		Linear[ 6 ] ;						// 線形かどうか
	float	PosXBezier[ 4 ] ;					// 座標Ｘ用ベジェ曲線情報
	float	PosYBezier[ 4 ] ;					// 座標Ｙ用ベジェ曲線情報
	float	PosZBezier[ 4 ] ;					// 座標Ｚ用ベジェ曲線情報
	float	RotBezier[ 4 ] ;					// 回転用ベジェ曲線情報
	float	LenBezier[ 4 ] ;					// 距離用ベジェ曲線情報
	float	ViewAngBezier[ 4 ] ;				// 視野角用ベジェ曲線情報
	DWORD	ViewingAngle;						// 60: 56:視野角
	BYTE	Perspective;						// 61: 60:射影カメラかどうか 0:射影カメラ 1:正射影カメラ
	VMD_READ_CAMERA_KEY_INFO *Prev ;			// 前のキーへのアドレス
	VMD_READ_CAMERA_KEY_INFO *Next ;			// 次のキーへのアドレス
} ;

// VMD読み込み処理用カメラ構造体
struct VMD_READ_CAMERA_INFO
{
	DWORD						KeyNum ;		// キーの数
	DWORD						MaxFrame ;		// 最大フレーム番号
	DWORD						MinFrame ;		// 最小フレーム番号
	VMD_READ_CAMERA_KEY_INFO	*FirstKey ;		// キーリストの先頭
} ;

// VMD読みこみ処理用構造体
struct VMD_READ_INFO
{
	DWORD						NodeNum ;			// ノードの数
	VMD_READ_NODE_INFO			*Node ;				// ノード配列

	DWORD						FaceKeySetNum ;		// 表情キーセットの数
	VMD_READ_FACE_KEY_SET_INFO	*FaceKeySet ;		// 表情キーセット配列

	VMD_READ_KEY_INFO			*KeyBuffer ;		// キーバッファ
	VMD_READ_FACE_KEY_INFO		*FaceKeyBuffer ;	// 表情キーバッファ
	VMD_READ_CAMERA_KEY_INFO	*CamKeyBuffer ;		// カメラキーバッファ

	VMD_READ_CAMERA_INFO		*Camera ;			// カメラキーの情報

	DWORD						MaxTime ;			// アニメーションの総時間
} ;

// 関数宣言 -------------------------------------

// 行列の回転部分の正規化を行う
__inline void MV1LoadModelToVMD_NomalizedMatrix( MATRIX *Mat )
{
	VECTOR DivSize ;

	// スケーリングが使用されていなかったら行列の回転部分の正規化
	DivSize.x = 1.0f / _SQRT( Mat->m[ 0 ][ 0 ] * Mat->m[ 0 ][ 0 ] + Mat->m[ 0 ][ 1 ] * Mat->m[ 0 ][ 1 ] + Mat->m[ 0 ][ 2 ] * Mat->m[ 0 ][ 2 ] ) ;
	DivSize.y = 1.0f / _SQRT( Mat->m[ 1 ][ 0 ] * Mat->m[ 1 ][ 0 ] + Mat->m[ 1 ][ 1 ] * Mat->m[ 1 ][ 1 ] + Mat->m[ 1 ][ 2 ] * Mat->m[ 1 ][ 2 ] ) ;
	DivSize.z = 1.0f / _SQRT( Mat->m[ 2 ][ 0 ] * Mat->m[ 2 ][ 0 ] + Mat->m[ 2 ][ 1 ] * Mat->m[ 2 ][ 1 ] + Mat->m[ 2 ][ 2 ] * Mat->m[ 2 ][ 2 ] ) ;

	Mat->m[ 0 ][ 0 ] *= DivSize.x ;
	Mat->m[ 0 ][ 1 ] *= DivSize.x ;
	Mat->m[ 0 ][ 2 ] *= DivSize.x ;

	Mat->m[ 1 ][ 0 ] *= DivSize.y ;
	Mat->m[ 1 ][ 1 ] *= DivSize.y ;
	Mat->m[ 1 ][ 2 ] *= DivSize.y ;

	Mat->m[ 2 ][ 0 ] *= DivSize.z ;
	Mat->m[ 2 ][ 1 ] *= DivSize.z ;
	Mat->m[ 2 ][ 2 ] *= DivSize.z ;
}

// 一部処理を省略する行列の積を求める
__inline void MV1LoadModelToVMD_CreateMultiplyMatrix( MATRIX *Out, MATRIX *In1, MATRIX *In2 )
{
	Out->m[0][0] = In1->m[0][0] * In2->m[0][0] + In1->m[0][1] * In2->m[1][0] + In1->m[0][2] * In2->m[2][0] ;
	Out->m[0][1] = In1->m[0][0] * In2->m[0][1] + In1->m[0][1] * In2->m[1][1] + In1->m[0][2] * In2->m[2][1] ;
	Out->m[0][2] = In1->m[0][0] * In2->m[0][2] + In1->m[0][1] * In2->m[1][2] + In1->m[0][2] * In2->m[2][2] ;
	Out->m[0][3] = 0.0f ;

	Out->m[1][0] = In1->m[1][0] * In2->m[0][0] + In1->m[1][1] * In2->m[1][0] + In1->m[1][2] * In2->m[2][0] ;
	Out->m[1][1] = In1->m[1][0] * In2->m[0][1] + In1->m[1][1] * In2->m[1][1] + In1->m[1][2] * In2->m[2][1] ;
	Out->m[1][2] = In1->m[1][0] * In2->m[0][2] + In1->m[1][1] * In2->m[1][2] + In1->m[1][2] * In2->m[2][2] ;
	Out->m[1][3] = 0.0f ;

	Out->m[2][0] = In1->m[2][0] * In2->m[0][0] + In1->m[2][1] * In2->m[1][0] + In1->m[2][2] * In2->m[2][0] ;
	Out->m[2][1] = In1->m[2][0] * In2->m[0][1] + In1->m[2][1] * In2->m[1][1] + In1->m[2][2] * In2->m[2][1] ;
	Out->m[2][2] = In1->m[2][0] * In2->m[0][2] + In1->m[2][1] * In2->m[1][2] + In1->m[2][2] * In2->m[2][2] ;
	Out->m[2][3] = 0.0f ;

	Out->m[3][0] = In1->m[3][0] * In2->m[0][0] + In1->m[3][1] * In2->m[1][0] + In1->m[3][2] * In2->m[2][0] + In2->m[3][0] ;
	Out->m[3][1] = In1->m[3][0] * In2->m[0][1] + In1->m[3][1] * In2->m[1][1] + In1->m[3][2] * In2->m[2][1] + In2->m[3][1] ;
	Out->m[3][2] = In1->m[3][0] * In2->m[0][2] + In1->m[3][1] * In2->m[1][2] + In1->m[3][2] * In2->m[2][2] + In2->m[3][2] ;
	Out->m[3][3] = 1.0f ;
}

// 回転成分だけの行列の積を求める( ３×３以外の部分には値も代入しない )
__inline void MV1LoadModelToVMD_CreateMultiplyMatrixRotOnly( MATRIX *Out, MATRIX *In1, MATRIX *In2 )
{
	Out->m[0][0] = In1->m[0][0] * In2->m[0][0] + In1->m[0][1] * In2->m[1][0] + In1->m[0][2] * In2->m[2][0] ;
	Out->m[0][1] = In1->m[0][0] * In2->m[0][1] + In1->m[0][1] * In2->m[1][1] + In1->m[0][2] * In2->m[2][1] ;
	Out->m[0][2] = In1->m[0][0] * In2->m[0][2] + In1->m[0][1] * In2->m[1][2] + In1->m[0][2] * In2->m[2][2] ;

	Out->m[1][0] = In1->m[1][0] * In2->m[0][0] + In1->m[1][1] * In2->m[1][0] + In1->m[1][2] * In2->m[2][0] ;
	Out->m[1][1] = In1->m[1][0] * In2->m[0][1] + In1->m[1][1] * In2->m[1][1] + In1->m[1][2] * In2->m[2][1] ;
	Out->m[1][2] = In1->m[1][0] * In2->m[0][2] + In1->m[1][1] * In2->m[1][2] + In1->m[1][2] * In2->m[2][2] ;

	Out->m[2][0] = In1->m[2][0] * In2->m[0][0] + In1->m[2][1] * In2->m[1][0] + In1->m[2][2] * In2->m[2][0] ;
	Out->m[2][1] = In1->m[2][0] * In2->m[0][1] + In1->m[2][1] * In2->m[1][1] + In1->m[2][2] * In2->m[2][1] ;
	Out->m[2][2] = In1->m[2][0] * In2->m[0][2] + In1->m[2][1] * In2->m[1][2] + In1->m[2][2] * In2->m[2][2] ;
}

// 条件を限定した少し高速な座標変換計算
__inline void MV1LoadModelToVMD_VectorTransform( VECTOR &Out, VECTOR &InVec, MATRIX &InMatrix )
{
	Out.x = InVec.x * InMatrix.m[0][0] + InVec.y * InMatrix.m[1][0] + InVec.z * InMatrix.m[2][0] + InMatrix.m[3][0] ;
	Out.y = InVec.x * InMatrix.m[0][1] + InVec.y * InMatrix.m[1][1] + InVec.z * InMatrix.m[2][1] + InMatrix.m[3][1] ;
	Out.z = InVec.x * InMatrix.m[0][2] + InVec.y * InMatrix.m[1][2] + InVec.z * InMatrix.m[2][2] + InMatrix.m[3][2] ;
}

// クォータニオンの乗算
__inline FLOAT4 QuatMul( FLOAT4 &q1, FLOAT4 &q2 )
{
	FLOAT4 res ;

	res.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y ;
	res.y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x ;
	res.z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w ;
	res.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z ;

	return res;
}

// クォータニオンの正規化
__inline FLOAT4 QuatNorm( FLOAT4 &q )
{
	FLOAT4 res ;
	float divnum ;

	divnum = _SQRT( q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w ) ;
	res.x = q.x / divnum ;
	res.y = q.y / divnum ;
	res.z = q.z / divnum ;
	res.w = q.w / divnum ;

	return res;
}

// クォータニオンから行列に変換
__inline void QuatConvertToMatrix( MATRIX &mat, FLOAT4 &q, VECTOR &trans )
{
	float sx = q.x * q.x * 2.0f ;
	float sy = q.y * q.y * 2.0f ;
	float sz = q.z * q.z * 2.0f ;
	float cx = q.y * q.z * 2.0f ;
	float cy = q.x * q.z * 2.0f ;
	float cz = q.x * q.y * 2.0f ;
	float wx = q.w * q.x * 2.0f ;
	float wy = q.w * q.y * 2.0f ;
	float wz = q.w * q.z * 2.0f ;

	mat.m[0][0] = 1.0f - ( sy + sz ) ;	mat.m[0][1] = cz + wz ;				mat.m[0][2] = cy - wy ;				mat.m[0][3] = 0.0f ;
	mat.m[1][0] = cz - wz ;				mat.m[1][1] = 1.0f - ( sx + sz ) ;	mat.m[1][2] = cx + wx ;				mat.m[1][3] = 0.0f ;
	mat.m[2][0] = cy + wy ;				mat.m[2][1] = cx - wx ;				mat.m[2][2] = 1.0f - ( sx + sy ) ;	mat.m[2][3] = 0.0f ;
	mat.m[3][0] = trans.x ;				mat.m[3][1] = trans.y ;				mat.m[3][2] = trans.z ;				mat.m[3][3] = 1.0f ;
}

// 行列からクォータニオンに変換
__inline void QuatConvertFromMatrix( FLOAT4 &q, MATRIX &mat )
{
	float s ;
	float tr = mat.m[0][0] + mat.m[1][1] + mat.m[2][2] + 1.0f ;
	if( tr >= 1.0f )
	{
		s = 0.5f / _SQRT( tr ) ;
		q.w = 0.25f / s ;
		q.x = ( mat.m[1][2] - mat.m[2][1] ) * s ;
		q.y = ( mat.m[2][0] - mat.m[0][2] ) * s ;
		q.z = ( mat.m[0][1] - mat.m[1][0] ) * s ;
	}
	else
	{
		float max ;
		max = mat.m[1][1] > mat.m[2][2] ? mat.m[1][1] : mat.m[2][2] ;
		
		if( max < mat.m[0][0] )
		{
			s = _SQRT( mat.m[0][0] - ( mat.m[1][1] + mat.m[2][2] ) + 1.0f ) ;

			float x = s * 0.5f ;
			s = 0.5f / s ;
			q.x = x ;
			q.y = ( mat.m[0][1] + mat.m[1][0] ) * s ;
			q.z = ( mat.m[2][0] + mat.m[0][2] ) * s ;
			q.w = ( mat.m[1][2] - mat.m[2][1] ) * s ;
		}
		else
		if( max == mat.m[1][1] )
		{
			s = _SQRT( mat.m[1][1] - ( mat.m[2][2] + mat.m[0][0]) + 1.0f ) ;

			float y = s * 0.5f ;
			s = 0.5f / s ;
			q.x = ( mat.m[0][1] + mat.m[1][0] ) * s ;
			q.y = y ;
			q.z = ( mat.m[1][2] + mat.m[2][1] ) * s ;
			q.w = ( mat.m[2][0] - mat.m[0][2] ) * s ;
		}
		else
		{
			s = _SQRT( mat.m[2][2] - (mat.m[0][0] + mat.m[1][1]) + 1.0f ) ;

			float z = s * 0.5f ;
			s = 0.5f / s ;
			q.x = ( mat.m[2][0] + mat.m[0][2] ) * s ;
			q.y = ( mat.m[1][2] + mat.m[2][1] ) * s ;
			q.z = z ;
			q.w = ( mat.m[0][1] - mat.m[1][0] ) * s ;
		}
	}
}

__inline static void VmdCalcLine( int &Linear, float &Rate, float &RateH, float &X1, float &X2, float &Y1, float &Y2 )
{
	if( Linear )
	{
		RateH = Rate ;
	}
	else
	{
		int l ;
		float T, InvT ;

		for( l = 0, T = Rate, InvT = 1.0f - T ; l < 32 ; l ++ )
		{
			float TempX = InvT * InvT * T * X1 + InvT * T * T * X2 + T * T * T - Rate ;
			if( TempX < 0.0001f && TempX > -0.0001f ) break ;
			T -= TempX * 0.5f ;
			InvT = 1.0f - T ;
		}
		RateH = InvT * InvT * T * Y1 + InvT * T * T * Y2 + T * T * T ;
	}
}

// クォータニオンの正規化
extern void QuaternionNormalize( FLOAT4 *Out, FLOAT4 *Src ) ;

// クォータニオンからXYZ軸回転の取得
extern void QuaternionToEuler( VECTOR *Angle, const FLOAT4 *Qt ) ;

// XYZ回転値からクォータニオンの作成
extern void QuaternionCreateEuler( FLOAT4 *Qt, const VECTOR *Angle ) ;

// 条件を限定した少し高速な逆行列計算
extern void MV1LoadModelToVMD_InverseMatrix( MATRIX &InMatrix, MATRIX &OutMatrix ) ;

// ＶＭＤファイルの基本情報を読み込む( -1:エラー )
extern int LoadVMDBaseData( VMD_READ_INFO *VmdData, void *DataBuffer, int DataSize ) ;

// ＶＭＤファイルの基本情報の後始末をする
extern int TerminateVMDBaseData( VMD_READ_INFO *VmdData ) ;

// カメラのアニメーションを読み込みようデータに追加する
extern int SetupVMDCameraAnim( VMD_READ_INFO *VmdData, MV1_MODEL_R *RModel, const wchar_t *Name, MV1_ANIMSET_R *AnimSet ) ;

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
) ;


#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif

#endif
