// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		バーテックスシェーダー関連定義
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

// Float4 型定数
#define DX_VS_CONSTF_ZERO_ONE                          0			// x:0.0f y:1.0f
#define DX_VS_CONSTF_AMBIENT_EMISSIVE                  1			// マテリアルエミッシブカラー + マテリアルアンビエントカラー * グローバルアンビエントカラー
#define DX_VS_CONSTF_PROJECTION_MAT                    2			// 射影行列の転置行列の定数アドレス
#define DX_VS_CONSTF_VIEW_MAT                          6			// ビュー行列の転置行列の定数アドレス
#define DX_VS_CONSTF_FOG                               10			// フォグ用パラメータを格納するアドレス( x:end/(end - start)  y:-1/(end - start)  z:density  w:自然対数の低 )

#define DX_VS_CONSTF_MATERIAL_START                    11			// マテリアルのパラメータが格納される定数アドレス
#define DX_VS_CONSTF_MATERIAL_SIZE                     3			// マテリアル用のレジスタの数
#define DX_VS_CONSTF_MAT_DIFFUSE                       0			// マテリアルのディフーズ色
#define DX_VS_CONSTF_MAT_SPECULAR                      1			// マテリアルのスペキュラー色
#define DX_VS_CONSTF_MAT_POWER                         2			// マテリアルのスペキュラハイライトのパワー

#define DX_VS_CONSTF_LIGHT_START                       14			// ライトのパラメータが格納される定数アドレス
#define DX_VS_CONSTF_LIGHT_NUM                         4			// トライアングルリスト一つで同時に使用するライトの最大数
#define DX_VS_CONSTF_LIGHT_UNITSIZE                    7			// ライトパラメータ一つ辺りに使用するレジスタの数
#define DX_VS_CONSTF_LGT_POSITION                      0			// ライトの位置( ビュー空間 )
#define DX_VS_CONSTF_LGT_DIRECTION                     1			// ライトの方向( ビュー空間 )
#define DX_VS_CONSTF_LGT_DIFFUSE                       2			// ライトのディフーズ色
#define DX_VS_CONSTF_LGT_SPECULAR                      3			// ライトのスペキュラ色
#define DX_VS_CONSTF_LGT_AMBIENT                       4			// ライトのアンビエント色とマテリアルのアンビエントカラーを乗算したもの
#define DX_VS_CONSTF_LGT_RANGE_FALLOFF_AT0_AT1         5			// ライトの x:有効距離の二乗 y:フォールオフ z:距離減衰パラメータ０ w:距離減衰パラメータ１
#define DX_VS_CONSTF_LGT_AT2_SPOTP0_SPOTP1             6			// ライトの x:距離減衰パラメータ２ y:スポットライト用パラメータ０( cos( Phi / 2.0f ) ) z:スポットライト用パラメータ１( 1.0f / ( cos( Theta / 2.0f ) - cos( Phi / 2.0f ) ) )

#define DX_VS_CONSTF_TOON_OUTLINE_SIZE                 42			// トゥーンレンダリング用の輪郭線の太さ
#define DX_VS_CONSTF_DIF_SPE_SOURCE                    43			// ディフューズカラーとスペキュラカラーのソース  x:ディフューズカラー( 0.0f:マテリアル  1.0f:頂点 )  y:スペキュラカラー( 0.0f:マテリアル  1.0f:頂点 )
#define DX_VS_CONSTF_SHADOWMAP1_LIGHT_VIEW_PROJECTION_MAT 44		// シャドウマップ１用のライトビュー行列とライト射影行列を乗算したもの
#define DX_VS_CONSTF_SHADOWMAP2_LIGHT_VIEW_PROJECTION_MAT 48		// シャドウマップ２用のライトビュー行列とライト射影行列を乗算したもの
#define DX_VS_CONSTF_SHADOWMAP3_LIGHT_VIEW_PROJECTION_MAT 52		// シャドウマップ３用のライトビュー行列とライト射影行列を乗算したもの

#define DX_VS_CONSTF_TEXTURE_MATRIX_START              88			// テクスチャ座標変換行列の転置行列が格納される定数アドレス
#define DX_VS_CONSTF_TEXTURE_MATRIX_NUM                3			// テクスチャ座標変換行列の転置行列の数
#define DX_VS_CONSTF_TEXTURE_MATRIX_UNITSIZE           2			// テクスチャ座標変換行列の転置行列一つあたりに使用されるレジスタの数

#define DX_VS_CONSTF_WORLD_MAT_START                   94			// ローカル→ワールド行列の転置行列が格納される定数アドレス
#define DX_VS_CONSTF_WORLD_MAT_NUM                     54			// トライアングルリスト一つで同時に使用するローカル→ワールド行列の最大数

// Bool 型定数
#define DX_VS_CONSTB_FOG_LINEAR                        0			// 線形フォグ使用の有無
#define DX_VS_CONSTB_FOG_EXP                           1			// 指数関数フォグ使用の有無
#define DX_VS_CONSTB_FOG_EXP2                          2			// 指数関数フォグ２使用の有無
#define DX_VS_CONSTB_FOG_USE                           3			// フォグ使用の有無
#define DX_VS_CONSTB_LIGHT_START                       4			// ライト情報の開始定数アドレス
#define DX_VS_CONSTB_LIGHT_NUM                         4			// トライアングルリスト一つで同時に使用するライトの最大数
#define DX_VS_CONSTB_LIGHT_UNITSIZE                    3			// ライト一つ当たりに使用するブール型定数の数
#define DX_VS_CONSTB_LGT_USE                           0			// ライト使用の有無
#define DX_VS_CONSTB_LGT_POINT_OR_SPOT                 1			// ポイントライトか若しくはスポットライトかどうか
#define DX_VS_CONSTB_LGT_SPOT                          2			// スポットライトかどうか


