// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		モデルデータ読み込み処理４用ヘッダ
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXMODELLOADER4_H__
#define __DXMODELLOADER4_H__

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

#define PMX_MAX_IKLINKNUM		(64)			// 対応するIKリンクの最大数

// データ型定義 ---------------------------------

// PMXファイルの情報を格納する構造体
struct PMX_BASEINFO
{
	BYTE	EncodeType ;						// 文字コードのエンコードタイプ 0:UTF16 1:UTF8
	BYTE	UVNum ;								// 追加ＵＶ数 ( 0〜4 )
	BYTE	VertexIndexSize ;					// 頂点Indexサイズ ( 1 or 2 or 4 )
	BYTE	TextureIndexSize ;					// テクスチャIndexサイズ ( 1 or 2 or 4 )
	BYTE	MaterialIndexSize ;					// マテリアルIndexサイズ ( 1 or 2 or 4 )
	BYTE	BoneIndexSize ;						// ボーンIndexサイズ ( 1 or 2 or 4 )
	BYTE	MorphIndexSize ;					// モーフIndexサイズ ( 1 or 2 or 4 )
	BYTE	RigidIndexSize ;					// 剛体Indexサイズ ( 1 or 2 or 4 )
} ;

// 頂点データ
struct PMX_VERTEX
{
	float	Position[ 3 ] ;						// 座標
	float	Normal[ 3 ] ;						// 法線
	float	UV[ 2 ] ;							// 標準UV値
	float	AddUV[ 4 ][ 4 ] ;					// 追加UV値
	BYTE	WeightType ;						// ウエイトタイプ( 0:BDEF1 1:BDEF2 2:BDEF4 3:SDEF )
	int		BoneIndex[ 4 ] ;					// ボーンインデックス
	float	BoneWeight[ 4 ] ;					// ボーンウェイト
	float	SDEF_C[ 3 ] ;						// SDEF-C
	float	SDEF_R0[ 3 ] ;						// SDEF-R0
	float	SDEF_R1[ 3 ] ;						// SDEF-R1
	float	ToonEdgeScale ;						// トゥーンエッジのスケール
} ;

// 面リスト
struct PMX_FACE
{
	int		VertexIndex[ 3 ] ;
} ;

// テクスチャ情報
struct PMX_TEXTURE
{
	wchar_t	TexturePath[ 512 ] ;
} ;

// マテリアル情報
struct PMX_MATERIAL
{
	wchar_t	Name[ 128 ] ;						// 名前

	float	Diffuse[ 4 ] ;						// ディフューズカラー
	float	Specular[ 3 ] ;						// スペキュラカラー
	float	SpecularPower ;						// スペキュラ定数
	float	Ambient[ 3 ] ;						// アンビエントカラー

	BYTE	CullingOff ;						// 両面描画
	BYTE	GroundShadow ;						// 地面影
	BYTE	SelfShadowMap ;						// セルフシャドウマップへの描画
	BYTE	SelfShadowDraw ;					// セルフシャドウの描画
	BYTE	EdgeDraw ;							// エッジの描画

	float	EdgeColor[ 4 ] ;					// エッジカラー
	float	EdgeSize ;							// エッジサイズ

	int		TextureIndex ;						// 通常テクスチャインデックス
	int		SphereTextureIndex ;				// スフィアテクスチャインデックス
	BYTE	SphereMode ;						// スフィアモード( 0:無効 1:乗算 2:加算 3:サブテクスチャ(追加UV1のx,yをUV参照して通常テクスチャ描画を行う)

	BYTE	ToonFlag ;							// 共有トゥーンフラグ( 0:個別Toon 1:共有Toon )
	int		ToonTextureIndex ;					// トゥーンテクスチャインデックス

	int		MaterialFaceNum ;					// マテリアルが適応されている面の数
} ;

// ＩＫリンク情報
struct PMX_IKLINK
{
	int		BoneIndex ;							// リンクボーンのインデックス
	BYTE	RotLockFlag ;						// 回転制限( 0:OFF 1:ON )
	float	RotLockMin[ 3 ] ;					// 回転制限、下限
	float	RotLockMax[ 3 ] ;					// 回転制限、上限
} ;

// ＩＫ情報
struct PMX_IK
{
	int		TargetBoneIndex ;					// IKターゲットのボーンインデックス
	int		LoopNum ;							// IK計算のループ回数
	float	RotLimit ;							// 計算一回辺りの制限角度

	int		LinkNum ;							// ＩＫリンクの数
	PMX_IKLINK Link[ PMX_MAX_IKLINKNUM ] ;		// ＩＫリンク情報
} ;

// ボーン情報
struct PMX_BONE
{
	wchar_t	Name[ 128 ] ;						// 名前
	float	Position[ 3 ] ;						// 座標
	int		ParentBoneIndex ;					// 親ボーンインデックス
	int		TransformLayer ;					// 変形階層
	
	BYTE	Flag_LinkDest ;						// 接続先
	BYTE	Flag_EnableRot ;					// 回転ができるか
	BYTE	Flag_EnableMov ;					// 移動ができるか
	BYTE	Flag_Disp ;							// 表示
	BYTE	Flag_EnableControl ;				// 操作ができるか
	BYTE	Flag_IK ;							// IK
	BYTE	Flag_AddRot ;						// 回転付与
	BYTE	Flag_AddMov ;						// 移動付与
	BYTE	Flag_LockAxis ;						// 軸固定
	BYTE	Flag_LocalAxis ;					// ローカル軸
	BYTE	Flag_AfterPhysicsTransform ;		// 物理後変形
	BYTE	Flag_OutParentTransform ;			// 外部親変形

	float	OffsetPosition[ 3 ] ;				// オフセット座標
	int		LinkBoneIndex ;						// 接続先ボーンインデックス
	int		AddParentBoneIndex ;				// 付与親ボーンインデックス
	float	AddRatio ;							// 付与率
	float	LockAxisVector[ 3 ] ;				// 軸固定時の軸の方向ベクトル
	float	LocalAxisXVector[ 3 ] ;				// ローカル軸のＸ軸
	float	LocalAxisZVector[ 3 ] ;				// ローカル軸のＺ軸
	int		OutParentTransformKey ;				// 外部親変形時の Key値

	PMX_IK	IKInfo ;							// ＩＫ情報
} ;

// 頂点モーフ情報
struct PMX_MORPH_VERTEX
{
	int		Index ;								// 頂点インデックス
	float	Offset[ 3 ] ;						// 頂点座標オフセット
} ;

// ＵＶモーフ情報
struct PMX_MORPH_UV
{
	int		Index ;								// 頂点インデックス
	FLOAT4	Offset ;							// 頂点ＵＶオフセット
} ;

// ボーンモーフ情報
struct PMX_MORPH_BONE
{
	int		Index ;								// ボーンインデックス
	float	Offset[ 3 ] ;						// 座標オフセット
	float	Quat[ 4 ] ;							// 回転クォータニオン
} ;

// 材質モーフ情報
struct PMX_MORPH_MATERIAL
{
	int		Index ;								// マテリアルインデックス
	BYTE	CalcType ;							// 計算タイプ( 0:乗算  1:加算 )
	float	Diffuse[ 4 ] ;						// ディフューズカラー
	float	Specular[ 3 ] ;						// スペキュラカラー
	float	SpecularPower ;						// スペキュラ係数
	float	Ambient[ 3 ] ;						// アンビエントカラー
	float	EdgeColor[ 4 ] ;					// エッジカラー
	float	EdgeSize ;							// エッジサイズ
	float	TextureScale[ 4 ] ;					// テクスチャ係数
	float	SphereTextureScale[ 4 ] ;			// スフィアテクスチャ係数
	float	ToonTextureScale[ 4 ] ;				// トゥーンテクスチャ係数
} ;

// グループモーフ
struct PMX_MORPH_GROUP
{
	int		Index ;								// モーフインデックス
	float	Ratio ;								// モーフ率
} ;

// モーフ情報
struct PMX_MORPH
{
	wchar_t	Name[ 128 ] ;						// 名前

	BYTE	ControlPanel ;						// 操作パネル
	BYTE	Type ;								// モーフの種類  0:グループ 1:頂点 2:ボーン 3:UV 4:追加UV1 5:追加UV2 6:追加UV3 7:追加UV4 8:材質

	int		DataNum ;							// モーフ情報の数

	union
	{
		PMX_MORPH_VERTEX *Vertex ;				// 頂点モーフ
		PMX_MORPH_UV *UV ;						// UVモーフ
		PMX_MORPH_BONE *Bone ;					// ボーンモーフ
		PMX_MORPH_MATERIAL *Material ;			// マテリアルモーフ
		PMX_MORPH_GROUP *Group ;				// グループモーフ
	} ;
} ;

// 剛体情報
struct PMX_RIGIDBODY
{
	wchar_t	Name[ 128 ] ;						// 名前

	int		BoneIndex ;							// 対象ボーン番号

	BYTE	RigidBodyGroupIndex ;				// 剛体グループ番号
	WORD	RigidBodyGroupTarget ;				// 剛体グループ対象

	BYTE	ShapeType ;							// 形状( 0:球  1:箱  2:カプセル )
	float	ShapeW ;							// 幅
	float	ShapeH ;							// 高さ
	float	ShapeD ;							// 奥行

	float	Position[ 3 ] ;						// 位置
	float	Rotation[ 3 ] ;						// 回転( ラジアン )

	float	RigidBodyWeight ;					// 質量
	float	RigidBodyPosDim ;					// 移動減衰
	float	RigidBodyRotDim ;					// 回転減衰
	float	RigidBodyRecoil ;					// 反発力
	float	RigidBodyFriction ;					// 摩擦力

	BYTE	RigidBodyType ;						// 剛体タイプ( 0:Bone追従  1:物理演算  2:物理演算(Bone位置合わせ) )
} ;

// ジョイント情報
struct PMX_JOINT
{
	wchar_t	Name[ 128 ] ;						// 名前

	BYTE	Type ;								// 種類( 0:スプリング6DOF ( PMX2.0 では 0 のみ )

	int		RigidBodyAIndex ;					// 接続先剛体Ａ
	int		RigidBodyBIndex ;					// 接続先剛体Ｂ

	float	Position[ 3 ] ;						// 位置
	float	Rotation[ 3 ] ;						// 回転( ラジアン )

	float	ConstrainPositionMin[ 3 ] ;			// 移動制限-下限
	float	ConstrainPositionMax[ 3 ] ;			// 移動制限-上限
	float	ConstrainRotationMin[ 3 ] ;			// 回転制限-下限
	float	ConstrainRotationMax[ 3 ] ;			// 回転制限-上限

	float	SpringPosition[ 3 ] ;				// バネ定数-移動
	float	SpringRotation[ 3 ] ;				// バネ定数-回転
} ;

// VMD読み込み処理用PMXボーンデータ
struct PMX_READ_BONE_INFO
{
	PMX_BONE			*Base ;					// ボーン基データ
	MV1_ANIM_R			*Anim ;					// このボーン用のアニメーション情報へのポインタ
	MV1_FRAME_R			*Frame ;				// このボーン用のフレーム情報へのポインタ

	VECTOR				*KeyPos ;				// 座標アニメーションキー配列の先頭
	FLOAT4				*KeyRot ;				// 回転アニメーションキー配列の先頭
	float				*KeyDisablePhysics ;	// 物理演算無効化キー配列の先頭
	float				*KeyPosTime ;			// 座標アニメーションタイムキー配列の先頭
	float				*KeyRotTime ;			// 回転アニメーションタイムキー配列の先頭
	MATRIX				*KeyMatrix ;			// 行列アニメーションキー配列の先頭
	MATRIX				*KeyMatrix2 ;			// 行列アニメーションキー配列の先頭( IsIKChild が TRUE のボーン用 )

	int					IsPhysics ;				// 物理を使用しているかどうか
	int					IsAddParent ;			// 付与親機能を使用しているかどうか
	int					IsIK ;					// ＩＫボーンかどうか
	int					IsIKAnim ;				// ＩＫアニメーションをつけるかどうか
	int					IsIKChild ;				// ＩＫアニメーションするボーンの子ボーン( 且つＩＫの影響を受けないボーン )かどうか
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

// VMD読み込み処理用PMXIKボーンデータ
struct PMX_READ_IK_INFO
{
	PMX_IK				*Base ;					// IKボーン基データ
	PMX_READ_BONE_INFO	*Bone ;					// IKターゲットボーン
	PMX_READ_BONE_INFO	*TargetBone ;			// IK先端ボーン

//	WORD				LimitAngleIK ;			// 角度制限ボーンが含まれたＩＫかどうか( 0:違う 1:そう ) 
//	WORD				Iterations ;			// 再起演算回数
//	float				ControlWeight ;			// IKの影響度
//	float				IKTotalLength ;			// IK処理の根元からチェインの最後までの距離

//	int					ChainBoneNum ;			// IKチェーンの長さ(子の数)
//	WORD				*ChainBone ;			// IK影響下のボーンへのインデックス配列へのポインタ

	PMX_READ_IK_INFO	*Prev ;					// リストの前のデータへのアドレス
	PMX_READ_IK_INFO	*Next ;					// リストの次のデータへのアドレス
} ;

// PMX読み込み処理用物理演算処理用データ
struct PMX_READ_PHYSICS_INFO
{
	PMX_RIGIDBODY		*Base ;					// 基データ

	PMX_READ_BONE_INFO	*Bone ;					// ボーンの情報へのポインタ
	bool				NoCopyToBone ;			// ボーンへ剛体の状態をあらわす行列をコピーしないかどうか

	DWORD				BulletInfo[ 320 ] ;		// BULLET_RIGIDBODY_INFO を格納するバッファ
} ;

// PMX読み込み処理用PMD物理演算処理用ジョイントデータ
struct PMX_READ_PHYSICS_JOINT_INFO
{
	PMX_JOINT			*Base ;					// ジョイント基データ

	DWORD				BulletInfo[ 32 ] ;		// BULLET_JOINT_INFO を格納するバッファ
} ;

// ＰＭＸ読み込み処理中の物理関係の情報を纏めた構造体
struct DX_MODELLOADER3_PMX_PHYSICS_INFO
{
	VECTOR Gravity ;
	int MotionTotalFrameNum ;

	int LoopMotionFlag ;
	int LoopMotionNum ;

	int	PhysicsCalcPrecision ;

	int PmxPhysicsNum ;
	PMX_READ_PHYSICS_INFO *PmxPhysicsInfoDim ;

	int PmxPhysicsJointNum ;
	PMX_READ_PHYSICS_JOINT_INFO *PmxPhysicsJointInfoDim ;

	MV1_ANIMKEYSET_R *KeyMatrixSet ;

	DWORD BulletPhysicsDataBuffer[ 32 ] ;			// BULLET_PHYSICS を格納するバッファ
} ;


// 関数宣言 -------------------------------------

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#ifndef DX_NON_BULLET_PHYSICS

extern int SetupPhysicsObject_PMXPhysicsInfo(		DX_MODELLOADER3_PMX_PHYSICS_INFO *MLPhysicsInfoint, int DisablePhysicsFile ) ;
extern int ReleasePhysicsObject_PMXPhysicsInfo(		DX_MODELLOADER3_PMX_PHYSICS_INFO *MLPhysicsInfo ) ;
extern int OneFrameProcess_PMXPhysicsInfo(			DX_MODELLOADER3_PMX_PHYSICS_INFO *MLPhysicsInfo, int FrameNo, int LoopNo, bool FPS60, int ValidNextRate, int TimeDivNum ) ;
extern int CheckDisablePhysicsAnim_PMXPhysicsInfo(	DX_MODELLOADER3_PMX_PHYSICS_INFO *MLPhysicsInfo, int PhysicsIndex ) ;

#endif


#endif
#endif
