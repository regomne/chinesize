// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		ピクセルシェーダー関連定義
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#define DX_PS_CONSTF_ZERO_HALF_ONE_TWO          0		// x=0.0f, y=0.5f, z=1.0f, w=2.0f
#define DX_PS_CONSTF_AMBIENT_EMISSIVE           1		// マテリアルエミッシブカラー + マテリアルアンビエントカラー * グローバルアンビエントカラー
#define DX_PS_CONSTF_MATERIAL_START             2		// マテリアル情報の格納開始アドレス
#define DX_PS_CONSTF_MATERIAL_SIZE              3		// マテリアル用のレジスタの数
#define DX_PS_CONSTF_MAT_DIFFUSE                0		// マテリアルのディフューズカラー
#define DX_PS_CONSTF_MAT_SPECULAR               1		// マテリアルのスペキュラカラー
#define DX_PS_CONSTF_MAT_POWER_TYPEPARAM012     2		// x:マテリアルのスペキュラハイライトのパワー  y.z.w:マテリアルタイプパラメータ0,1,2
#define DX_PS_CONSTF_FACTORCOLOR                5		// 透明度等
#define DX_PS_CONSTF_TOON_OUTLINE_COLOR         6		// トゥーンレンダリングの輪郭線の色
#define DX_PS_CONSTF_TOON_OUTLINE_WIDTH         7		// トゥーンレンダリングの輪郭線の太さ
#define DX_PS_CONSTF_FOG_COLOR                  8		// フォグカラー
#define DX_PS_CONSTF_LIGHT_START                9		// ライト情報の格納開始アドレス
#define DX_PS_CONSTF_LIGHT_UNITSIZE             3		// ライトの情報一つ辺りの使用レジスタの数
#define DX_PS_CONSTF_LGT_DIFFUSE                0		// ライトのディフューズカラー
#define DX_PS_CONSTF_LGT_SPECULAR               1		// ライトのスペキュラカラー
#define DX_PS_CONSTF_LGT_AMBIENT                2		// ライトのアンビエントカラーとマテリアルのアンビエントカラーを乗算したもの
#define DX_PS_CONSTF_SHADOW1_DADJ_GRAD_ENBL0_1             18	// シャドウマップ１用の情報( x:閾値深度補正値  y:グラデーション範囲  zw:ライト０・１への適用情報)
#define DX_PS_CONSTF_SHADOW1_ENBL2_SHADOW2_DADJ_GRAD_ENBL0 19	// シャドウマップ１用の情報( x:ライト２への適用情報 )、シャドウマップ２用の情報( y:閾値深度補正値  z:グラデーション範囲  w:ライト０への適用情報 )
#define DX_PS_CONSTF_SHADOW2_ENBL1_2_SHADOW3_DADJ_GRAD     20	// シャドウマップ２用の情報( xy:ライト１・２への適用情報)、シャドウマップ３用の情報( z:閾値深度補正値  w:グラデーション範囲 )
#define DX_PS_CONSTF_SHADOW3_ENBL0_1_2                     21	// シャドウマップ３用の情報( xyz:ライト０〜２への適用情報 )
#define DX_PS_CONSTF_MUL_ALPHA_COLOR            22		// カラーにアルファ値を乗算するかどうか( x( 0.0f:乗算しない  1.0f:乗算する ) yzw( 未使用 ) )
#define DX_PS_CONSTF_IGNORE_TEX_COLOR           23		// テクスチャカラー無視処理用カラー
#define DX_PS_CONSTF_LIGHT2_START               32		// ライト拡張情報の格納開始アドレス
#define DX_PS_CONSTF_LIGHT2_UNITSIZE            7		// ライト拡張情報一つ辺りに使用するレジスタの数
#define DX_PS_CONSTB_LIGHT2_NUM                 6		// 適用できるライトの最大数
#define DX_PS_CONSTF_LGT2_POSITION              0		// ライトの位置( ビュー空間 )
#define DX_PS_CONSTF_LGT2_DIRECTION             1		// ライトの方向( ビュー空間 )
#define DX_PS_CONSTF_LGT2_DIFFUSE               2		// ライトのディフーズ色
#define DX_PS_CONSTF_LGT2_SPECULAR              3		// ライトのスペキュラ色
#define DX_PS_CONSTF_LGT2_AMBIENT               4		// ライトのアンビエント色とマテリアルのアンビエントカラーを乗算したもの
#define DX_PS_CONSTF_LGT2_RANGE_FALLOFF_AT0_AT1 5		// ライトの x:有効距離の二乗 y:フォールオフ z:距離減衰パラメータ０ w:距離減衰パラメータ１
#define DX_PS_CONSTF_LGT2_AT2_SPOTP0_SPOTP1     6		// ライトの x:距離減衰パラメータ２ y:スポットライト用パラメータ０( cos( Phi / 2.0f ) ) z:スポットライト用パラメータ１( 1.0f / ( cos( Theta / 2.0f ) - cos( Phi / 2.0f ) ) )

