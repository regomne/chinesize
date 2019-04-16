// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		モデルデータ読み込み処理３用ヘッダ
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXMODELLOADER3_H__
#define __DXMODELLOADER3_H__

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

// PMDデータヘッダ( 283byte )
struct PMD_HEADER
{
	BYTE	Data[ 283 ] ;						// データ
/*
	char	Magic[ 3 ] ;						// "Pmd"
	float	Version ;							// 1.0f ( 0x3f800000 )
	char	Name[ 20 ] ;						// 名前
	char	Comment[ 256 ] ;					// コメント
*/
} ;

// 頂点データ一つ辺りの情報( 38byte )
struct PMD_VERTEX
{
	float	Position[ 3 ] ;						// 座標
	float	Normal[ 3 ] ;						// 法線
	float	Uv[ 2 ] ;							// テクスチャ座標
	WORD	BoneNo[ 2 ] ;						// ボーン番号
	BYTE	BoneWeight ;						// BoneNo[ 0 ] のボーンの影響度( 0〜100 ) BoneNo[ 1 ] の影響度は( 100 - BoneWeight )
	BYTE	Edge ;								// エッジフラグ  0:エッジ有効  1:エッジ無効
} ;

// 頂点データ
struct PMD_VERTEX_DATA
{
	DWORD	Count ;				// 頂点データの数
	/* ここに PMD_VERTEX が Count の数だけ存在する */
} ;

// 面リスト
struct PMD_FACE_DATA
{
	DWORD	VertexCount ;		// 頂点の数( 面の数は VertexCount / 3 )
	/* ここに WORD の頂点インデックスが VertexCount の数だけ存在する */
} ;

// マテリアル一つ辺りの情報( 70byte )
struct PMD_MATERIAL
{
	BYTE	Data[ 70 ] ;						// データ
/*
	float	DiffuseR, DiffuseG, DiffuseB ;		// ディフューズカラー
	float	Alpha ;								// α値
	float	Specularity ;						// スペキュラ係数
	float	SpecularR, SpecularG, SpecularB ;	// スペキュラカラー
	float	AmbientR, AmbientG, AmbientB ;		// アンビエントカラー
	BYTE	ToolImage ;							// トゥーンレンダリング用のテクスチャのインデックス
	BYTE	Edge ;								// 輪郭・影
	DWORD	FaceVertexCount ;					// このマテリアルを使用する面頂点の数
	char	TextureFileName[ 20 ] ;				// テクスチャファイル名
*/
} ;

// マテリアルの情報
struct PMD_MATERIAL_DATA
{
	DWORD	Count ;								// マテリアルの数
	/* ここに PMD_MATERIAL が Count の数だけ存在する */
} ;

// ボーン一つ辺りの情報( 39byte )
struct PMD_BONE
{
	BYTE	Data[ 39 ] ;						// データ
/*
	char	Name[ 20 ] ;						//  0:ボーン名
	WORD	Parent ;							// 20:親ボーン( 無い場合は 0xffff )
	WORD	TailPosBone ;						// 22:Tail 位置のボーン( チェーン末端の場合は 0xffff )
	BYTE	Type ;								// 24:ボーンのタイプ( 0:回転  1:回転と移動  2:IK  3:不明  4:IK影響下  5:回転影響下  6:IK接続先  7:非表示  8:捻り  9:回転運動 )
	WORD	IkParent ;							// 25:IKボーン番号( 影響IKボーン  無い場合は 0xffff )
	float	HeadPos[ 3 ] ;						// 27:ヘッドの位置
*/
} ;

// ボーンの情報
struct PMD_BONE_DATA
{
	WORD	Count ;								// ボーンの数
	/* ここに PMD_BONE が Count の数だけ存在する */
} ;

// IKデータ一つ辺りの情報( ChainBoneIndex を抜いたサイズ 11byte )
struct PMD_IK
{
	BYTE	Data[ 11 ] ;						// データ
/*
	WORD	Bone ;								// IKボーン
	WORD	TargetBone ;						// IKターゲットボーン
	BYTE	ChainLength ;						// IKチェーンの長さ(子の数)
	WORD	Iterations ;						// 再起演算回数
	float	ControlWeight ;						// IKの影響度
	WORD	ChainBoneIndexs[ ChainLength ] ;	// IK影響下のボーン番号
*/
} ;

// IKの情報
struct PMD_IK_DATA
{
	WORD	Count ;								// IKデータの数
	/* ここに PMD_IK が Count の数だけ存在する */
} ;

// 表情の頂点情報
struct PMD_SKIN_VERT
{
	DWORD	TargetVertexIndex ;					// 対象の頂点のインデックス
	float	Position[ 3 ] ;						// 座標( PMD_SKIN の SkinType が 0 なら絶対座標、それ以外なら base に対する相対座標 )
} ;

// 表情一つ辺りの情報( Vertex を抜いたサイズ 25byte )
struct PMD_SKIN
{
	BYTE	Data[ 25 ] ;						// データ
/*
	char	Name[ 20 ] ;						// 表情名
	DWORD	VertexCount ;						// 頂点の数
	BYTE	SkinType ;							// 表情の種類( 0:base 1：まゆ、2：目、3：リップ、4：その他 )
	PMD_SKIN_VERT Vertex[ VertexCount ] ;		// 表情用の頂点データ
*/
} ;

// 表情の情報
struct PMD_SKIN_DATA
{
	WORD	Count ;								// 表情データの数
	/* ここに PMD_SKIN が Count の数だけ存在する */
} ;

// 物理演算データ一つ辺りの情報( 83byte )
struct PMD_PHYSICS
{
	BYTE	Data[ 83 ] ;	
/*
	char	RigidBodyName[ 20 ] ;				//  0 : 剛体名
	WORD	RigidBodyRelBoneIndex ;				// 20 : 剛体関連ボーン番号
	BYTE	RigidBodyGroupIndex ;				// 22 : 剛体グループ番号
	WORD	RigidBodyGroupTarget ;				// 23 : 剛体グループ対象
	BYTE	ShapeType ;							// 25 : 形状( 0:球  1:箱  2:カプセル )
	float	ShapeW ;							// 26 : 幅
	float	ShapeH ;							// 30 : 高さ
	float	ShapeD ;							// 34 : 奥行
	float	Position[ 3 ] ;						// 38 : 位置
	float	Rotation[ 3 ] ;						// 50 : 回転( ラジアン )
	float	RigidBodyWeight ;					// 62 : 質量
	float	RigidBodyPosDim ;					// 66 : 移動減
	float	RigidBodyRotDim ;					// 70 : 回転減
	float	RigidBodyRecoil ;					// 74 : 反発力
	float	RigidBodyFriction ;					// 78 : 摩擦力
	BYTE	RigidBodyType ;						// 82 : 剛体タイプ( 0:Bone追従  1:物理演算  2:物理演算(Bone位置合わせ) )
*/
} ;

// 物理演算データの情報
struct PMD_PHYSICS_DATA
{
	DWORD	Count ;								// 剛体データの数
	/* ここに PMD_PHYSICS が Count の数だけ存在する */
} ;

// 物理演算用ジョイントデータ一つ辺りの情報( 124byte )
struct PMD_PHYSICS_JOINT
{
	char	Name[ 20 ] ;						// 名前
	DWORD	RigidBodyA ;						// 接続先剛体Ａ
	DWORD	RigidBodyB ;						// 接続先剛体Ｂ
	float	Position[ 3 ] ;						// 位置
	float	Rotation[ 3 ] ;						// 回転( ラジアン )
	float	ConstrainPosition1[ 3 ] ;			// 移動制限値１
	float	ConstrainPosition2[ 3 ] ;			// 移動制限値２
	float	ConstrainRotation1[ 3 ] ;			// 回転制限値１
	float	ConstrainRotation2[ 3 ] ;			// 回転制限値２
	float	SpringPosition[ 3 ] ;				// ばね移動値
	float	SpringRotation[ 3 ] ;				// ばね回転値
} ;

// 物理演算用ジョイント情報
struct PMD_PHYSICS_JOINT_DATA
{
	DWORD	Count ;								// ジョイントデータの数
	/* ここに PMD_PHYSICS_JOINT が Count の数だけ存在する */
} ;

// VMD読み込み処理用PMDボーンデータ
struct PMD_READ_BONE_INFO
{
	PMD_BONE			*Base ;					// ボーン基データ
	MV1_ANIM_R			*Anim ;					// このボーン用のアニメーション情報へのポインタ
	MV1_FRAME_R			*Frame ;				// このボーン用のフレーム情報へのポインタ

	VECTOR				*KeyPos ;				// 座標アニメーションキー配列の先頭
	FLOAT4				*KeyRot ;				// 回転アニメーションキー配列の先頭
	float				*KeyDisablePhysics ;	// 物理演算無効化キー配列の先頭
	float				*KeyPosTime ;			// 座標アニメーションタイムキー配列の先頭
	float				*KeyRotTime ;			// 回転アニメーションタイムキー配列の先頭
	MATRIX				*KeyMatrix ;			// 行列アニメーションキー配列の先頭
	MATRIX				*KeyMatrix2 ;			// 行列アニメーションキー配列の先頭( IsIKChild が TRUE のボーン用 )

	int					Type ;					// タイプ
	int					IsPhysics ;				// 物理を使用しているかどうか
	int					IsIK ;					// ＩＫボーンかどうか
	int					IsIKAnim ;				// ＩＫアニメーションをつけるかどうか
	int					IsIKChild ;				// ＩＫアニメーションするボーンの子ボーン( 且つＩＫの影響を受けないボーン )かどうか
	int					IKLimitAngle ;			// ＩＫ時に角度制限をするかどうか
	MATRIX				LocalMatrix ;			// 構築したローカル行列
	int					SetupLocalWorldMatrix ;	// ローカル→ワールド行列が構築されているかどうか
	MATRIX				LocalWorldMatrix ;		// 構築したローカル→ワールド行列
	int					BackupLocalWorldMatrix_Valid ;	// BackupLocalWorldMatrix が有効かどうか
	MATRIX				BackupLocalWorldMatrix ;		// 構築したローカル→ワールド行列( 物理演算処理時のバックアップ用 )
	VECTOR				Translate ;				// 平行移動値
	FLOAT4				Rotate ;				// 回転値
	float				DisablePhysics ;		// 物理演算無効化率
	VECTOR				TimeDivLoopCount0_Translate ;			// TimeDivLoopCount == 0 の平行移動値
	FLOAT4				TimeDivLoopCount0_Rotate ;				// TimeDivLoopCount == 0 の回転値
	MATRIX				TimeDivLoopCount0_LocalWorldMatrix ;	// TimeDivLoopCount == 0 の構築したローカル→ワールド行列
	VECTOR				OrgTranslate ;			// 原点からの初期位置

	MATRIX				IKmat ;					// IK処理で使用する行列構造体
	FLOAT4				IKQuat ;				// IK処理で使用するクォータニオン

	VECTOR				InitTranslate ;			// 平行移動値の初期値
	FLOAT4				InitRotate ;			// 回転値の初期値
#ifndef DX_NON_BULLET_PHYSICS
	int					PhysicsIndex ;			// 物理演算用情報の番号
	int					SetupPhysicsAnim ;		// ボーンアニメーションを付けたかどうか
#endif

	struct VMD_READ_NODE_INFO	*Node ;			// ボーンと関連付けられているノード
	struct VMD_READ_KEY_INFO	*NowKey ;		// 現在再生しているキー
} ;

// VMD読み込み処理用PMDIKボーンデータ
struct PMD_READ_IK_INFO
{
	PMD_IK				*Base ;					// IKボーン基データ
	PMD_READ_BONE_INFO	*Bone ;					// IKターゲットボーン
	PMD_READ_BONE_INFO	*TargetBone ;			// IK先端ボーン

	WORD				LimitAngleIK ;			// 角度制限ボーンが含まれたＩＫかどうか( 0:違う 1:そう ) 
	WORD				Iterations ;			// 再起演算回数
	float				ControlWeight ;			// IKの影響度
	float				IKTotalLength ;			// IK処理の根元からチェインの最後までの距離

	int					ChainBoneNum ;			// IKチェーンの長さ(子の数)
	WORD				*ChainBone ;			// IK影響下のボーンへのインデックス配列へのポインタ

	PMD_READ_IK_INFO	*Prev ;					// リストの前のデータへのアドレス
	PMD_READ_IK_INFO	*Next ;					// リストの次のデータへのアドレス
} ;

// PMD読み込み処理用物理演算処理用データ
struct PMD_READ_PHYSICS_INFO
{
	PMD_PHYSICS			*Base ;					// 基データ

	char				Name[ 24 ] ;			// 名前
	WORD				BoneIndex ;				// 対象のボーン

	PMD_READ_BONE_INFO	*Bone ;					// ボーンの情報へのポインタ
	int					ShapeType ;				// 形状タイプ( 0:球  1:箱  2:カプセル )
	int					RigidBodyType ;			// 剛体タイプ( 0:Bone追従  1:物理演算  2:物理演算(Bone位置合わせ) )
	bool				NoCopyToBone ;			// ボーンへ剛体の状態をあらわす行列をコピーしないかどうか

	DWORD				BulletInfo[ 160 ] ;		// BULLET_RIGIDBODY_INFO を格納するバッファ
} ;

// PMD読み込み処理用PMD物理演算処理用ジョイントデータ
struct PMD_READ_PHYSICS_JOINT_INFO
{
	PMD_PHYSICS_JOINT	Base ;					// ジョイント基データ

	DWORD				BulletInfo[ 16 ] ;		// BULLET_JOINT_INFO を格納するバッファ
} ;


// ＰＭＤ読み込み処理中の物理関係の情報を纏めた構造体
struct DX_MODELLOADER3_PMD_PHYSICS_INFO
{
	VECTOR Gravity ;
	int MotionTotalFrameNum ;

	int LoopMotionFlag ;
	int LoopMotionNum ;

	int	PhysicsCalcPrecision ;

	int PmdPhysicsNum ;
	PMD_READ_PHYSICS_INFO *PmdPhysicsInfoDim ;

	int PmdPhysicsJointNum ;
	PMD_READ_PHYSICS_JOINT_INFO *PmdPhysicsJointInfoDim ;

	MV1_ANIMKEYSET_R *KeyMatrixSet ;

	DWORD BulletPhysicsDataBuffer[ 32 ] ;			// BULLET_PHYSICS を格納するバッファ
} ;


// 関数宣言 -------------------------------------

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#ifndef DX_NON_BULLET_PHYSICS

extern int SetupPhysicsObject_PMDPhysicsInfo(		DX_MODELLOADER3_PMD_PHYSICS_INFO *MLPhysicsInfo, int DisablePhysicsFile ) ;
extern int ReleasePhysicsObject_PMDPhysicsInfo(		DX_MODELLOADER3_PMD_PHYSICS_INFO *MLPhysicsInfo ) ;
extern int OneFrameProcess_PMDPhysicsInfo(			DX_MODELLOADER3_PMD_PHYSICS_INFO *MLPhysicsInfo, int FrameNo, int LoopNo, bool FPS60, int ValidNextRate, int TimeDivNum ) ;
extern int CheckDisablePhysicsAnim_PMDPhysicsInfo(	DX_MODELLOADER3_PMD_PHYSICS_INFO *MLPhysicsInfo, int PhysicsIndex ) ;

extern int SetupPhysicsObject_ModelPhysicsInfo(		MV1_MODEL *Model ) ;
extern int ReleasePhysicsObject_ModelPhysicsInfo(	MV1_MODEL *Model ) ;
extern int ResetState_ModelPhysicsInfo(				MV1_MODEL *Model ) ;
extern int StepSimulation_ModelPhysicsInfo(			MV1_MODEL *Model, float TimeStep ) ;
extern int SetWorldGravity_ModelPhysiceInfo(		MV1_MODEL *Model, VECTOR Gravity ) ;

#endif

#endif
#endif
