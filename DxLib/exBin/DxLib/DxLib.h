// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		ヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXLIB
#define __DXLIB

#include "DxCompileConfig.h"

// ＤＸライブラリのバージョン
#define DXLIB_VERSION 0x320c
#define DXLIB_VERSION_STR_T _T( "3.20c" )
#define DXLIB_VERSION_STR_W    L"3.20c"

// 設定 -----------------------------------------------------------------------

// ＤＸライブラリに必要な lib ファイルを、プロジェクトのカレントフォルダや
// コンパイラのデフォルト LIB パスに設定せずに使用される場合は以下の
// コメントを外してください
//#define DX_LIB_NOT_DEFAULTPATH

#ifndef __DX_MAKE

// 描画関連の関数を一切使用されない場合は以下のコメントを外して下さい
//#define DX_NOTUSE_DRAWFUNCTION

#endif // __DX_MAKE


// 定義---------------------------------------------------------------------------

// π
#define DX_PI										(3.1415926535897932384626433832795 )
#define DX_PI_F										(3.1415926535897932384626433832795f)
#define DX_TWO_PI									(3.1415926535897932384626433832795  * 2.0 )
#define DX_TWO_PI_F									(3.1415926535897932384626433832795f * 2.0f)

#define DX_CHAR										char

#define DX_DEFINE_START

#define MAX_IMAGE_NUM								(32768)				// 同時に持てるグラフィックハンドルの最大数( ハンドルエラーチェックのマスクに使用しているので 65536 以下の 2 のべき乗にして下さい )
#define MAX_2DSURFACE_NUM							(32768)				// ２Ｄサーフェスデータの最大数( ハンドルエラーチェックのマスクに使用しているので 65536 以下の 2 のべき乗にして下さい )
#define MAX_3DSURFACE_NUM							(65536)				// ３Ｄサーフェスデータの最大数( ハンドルエラーチェックのマスクに使用しているので 65536 以下の 2 のべき乗にして下さい )
#define MAX_IMAGE_DIVNUM							(64)				// 画像分割の最大数
#define MAX_SURFACE_NUM								(65536)				// サーフェスデータの最大数
#define MAX_SHADOWMAP_NUM							(8192)				// シャドウマップデータの最大数
#define MAX_SOFTIMAGE_NUM							(8192)				// 同時に持てるソフトイメージハンドルの最大数( ハンドルエラーチェックのマスクに使用しているので 65536 以下の 2 のべき乗にして下さい )

#define MAX_SOUND_NUM								(32768)				// 同時に持てるサウンドハンドルの最大数
#define MAX_SOFTSOUND_NUM							(8192)				// 同時に持てるソフトウエアサウンドハンドルの最大数
#define MAX_MUSIC_NUM								(256)				// 同時に持てるミュージックハンドルの最大数
#define MAX_MOVIE_NUM								(100)				// 同時に持てるムービーハンドルの最大数
#define MAX_MASK_NUM								(32768)				// 同時に持てるマスクハンドルの最大数
#define MAX_FONT_NUM								(40)				// 同時に持てるフォントハンドルの最大数
#define MAX_INPUT_NUM								(256)				// 同時に持てる文字列入力ハンドルの最大数
#define MAX_SOCKET_NUM								(8192)				// 同時に持てる通信ハンドルの最大数
#define MAX_LIGHT_NUM								(4096)				// 同時に持てるライトハンドルの最大数
#define MAX_SHADER_NUM								(4096)				// 同時に持てるシェーダーハンドルの最大数
#define MAX_CONSTANT_BUFFER_NUM						(8192)				// 同時に持てる定数バッファハンドルの最大数
#define MAX_MODEL_BASE_NUM							(32768)				// 同時に持てる３Ｄモデル基本データハンドルの最大数
#define MAX_MODEL_NUM								(32768)				// 同時に持てる３Ｄモデルデータハンドルの最大数
#define MAX_VERTEX_BUFFER_NUM						(16384)				// 同時に持てる頂点バッファハンドルの最大数
#define MAX_INDEX_BUFFER_NUM						(16384)				// 同時に持てるインデックスバッファの最大数
#define MAX_FILE_NUM								(32768)				// 同時に持てるファイルハンドルの最大数

#define MAX_JOYPAD_NUM								(16)				// ジョイパッドの最大数

#define DEFAULT_SCREEN_SIZE_X						(640)				// デフォルトの画面の幅
#define DEFAULT_SCREEN_SIZE_Y						(480)				// デフォルトの画面の高さ
#define DEFAULT_COLOR_BITDEPTH						(16)				// デフォルトの色ビット深度
#define DEFAULT_ZBUFFER_BITDEPTH					(16)				// デフォルトのＺバッファビット深度

#define DEFAULT_FOV									(60.0F * 3.1415926535897932384626433832795F / 180.0F)	// デフォルトの視野角
#define DEFAULT_TAN_FOV_HALF						(0.57735026918962576450914878050196F) // tan( FOV * 0.5 )
#define DEFAULT_NEAR								(0.0F)				// NEARクリップ面
#define DEFAULT_FAR									(20000.0F)			// FARクリップ面

#define DX_DEFAULT_FONT_HANDLE						(-2)				// デフォルトフォントを示す値

#define DEFAULT_FONT_SIZE							(16)				// フォントのデフォルトのサイズ
#define DEFAULT_FONT_THINCK							(6)					// フォントのデフォルトの太さ
#define DEFAULT_FONT_TYPE							( DX_FONTTYPE_NORMAL )	// フォントのデフォルトの形態
#define DEFAULT_FONT_EDGESIZE						(1)					// フォントのデフォルトの太さ

#define MAX_USERIMAGEREAD_FUNCNUM					(10)				// ユーザーが登録できるグラフィックロード関数の最大数

// ＷＩＮＤＯＷＳのバージョンマクロ
#define DX_WINDOWSVERSION_31						(0x000)
#define DX_WINDOWSVERSION_95						(0x001)
#define DX_WINDOWSVERSION_98						(0x002)
#define DX_WINDOWSVERSION_ME						(0x003)
#define DX_WINDOWSVERSION_NT31						(0x104)
#define DX_WINDOWSVERSION_NT40						(0x105)
#define DX_WINDOWSVERSION_2000						(0x106)
#define DX_WINDOWSVERSION_XP						(0x107)
#define DX_WINDOWSVERSION_VISTA						(0x108)
#define DX_WINDOWSVERSION_7							(0x109)
#define DX_WINDOWSVERSION_8							(0x10A)
#define DX_WINDOWSVERSION_8_1						(0x10B)
#define DX_WINDOWSVERSION_10						(0x10C)
#define DX_WINDOWSVERSION_NT_TYPE					(0x100)

// DirectXのバージョン
#define DX_DIRECTXVERSION_NON						(0)
#define DX_DIRECTXVERSION_1							(0x10000)
#define DX_DIRECTXVERSION_2							(0x20000)
#define DX_DIRECTXVERSION_3							(0x30000)
#define DX_DIRECTXVERSION_4							(0x40000)
#define DX_DIRECTXVERSION_5							(0x50000)
#define DX_DIRECTXVERSION_6							(0x60000)
#define DX_DIRECTXVERSION_6_1						(0x60100)
#define DX_DIRECTXVERSION_7							(0x70000)
#define DX_DIRECTXVERSION_8							(0x80000)
#define DX_DIRECTXVERSION_8_1						(0x80100)

// Direct3Dのバージョン
#define DX_DIRECT3D_NONE							(0)
#define DX_DIRECT3D_9								(1)
#define DX_DIRECT3D_9EX								(2)
#define DX_DIRECT3D_11								(3)

// Direct3D11 の Feature Level
#define DX_DIRECT3D_11_FEATURE_LEVEL_9_1			(0x9100)
#define DX_DIRECT3D_11_FEATURE_LEVEL_9_2			(0x9200)
#define DX_DIRECT3D_11_FEATURE_LEVEL_9_3			(0x9300)
#define DX_DIRECT3D_11_FEATURE_LEVEL_10_0			(0xa000)
#define DX_DIRECT3D_11_FEATURE_LEVEL_10_1			(0xa100)
#define DX_DIRECT3D_11_FEATURE_LEVEL_11_0			(0xb000)
#define DX_DIRECT3D_11_FEATURE_LEVEL_11_1			(0xb100)

// 文字セット
#define DX_CHARSET_DEFAULT							(0)				// デフォルト文字セット
#define DX_CHARSET_SHFTJIS							(1)				// シフトJIS
#define DX_CHARSET_HANGEUL							(2)				// ハングル文字セット
#define DX_CHARSET_BIG5								(3)				// 繁体文字セット
#define DX_CHARSET_GB2312							(4)				// 簡体文字セット
#define DX_CHARSET_WINDOWS_1252						(5)				// 欧文(ラテン文字の文字コード)
#define DX_CHARSET_ISO_IEC_8859_15					(6)				// 欧文(ラテン文字の文字コード)
#define DX_CHARSET_UTF8								(7)				// UTF-8
#define DX_CHARSET_NUM								(8)				// 文字セットの数

// 文字コード形式
#define DX_CHARCODEFORMAT_SHIFTJIS					(932)			// シフトJISコード
#define DX_CHARCODEFORMAT_GB2312					(936)			// 簡体字文字コード
#define DX_CHARCODEFORMAT_UHC						(949)			// ハングル文字コード
#define DX_CHARCODEFORMAT_BIG5						(950)			// 繁体文字コード
#define DX_CHARCODEFORMAT_UTF16LE					(1200)			// UTF-16 リトルエンディアン
#define DX_CHARCODEFORMAT_UTF16BE					(1201)			// UTF-16 ビッグエンディアン
#define DX_CHARCODEFORMAT_WINDOWS_1252				(1252)			// 欧文(ラテン文字の文字コード)
#define DX_CHARCODEFORMAT_ISO_IEC_8859_15			(32764)			// 欧文(ラテン文字の文字コード)
#define DX_CHARCODEFORMAT_UTF8						(65001)			// UTF-8
#define DX_CHARCODEFORMAT_ASCII						(32765)			// アスキー文字コード
#define DX_CHARCODEFORMAT_UTF32LE					(32766)			// UTF-32 リトルエンディアン
#define DX_CHARCODEFORMAT_UTF32BE					(32767)			// UTF-32 ビッグエンディアン

// ＭＩＤＩの演奏モード定義
#define DX_MIDIMODE_MCI								(0)				// ＭＣＩによる演奏
#define DX_MIDIMODE_DM								(1)				// ＤｉｒｅｃｔＭｕｓｉｃによる演奏
#define DX_MIDIMODE_DIRECT_MUSIC_REVERB				(1)				// ＤｉｒｅｃｔＭｕｓｉｃ(リバーブあり)による演奏
#define DX_MIDIMODE_DIRECT_MUSIC_NORMAL				(2)				// ＤｉｒｅｃｔＭｕｓｉｃ(リバーブなし)による演奏
#define DX_MIDIMODE_NUM								(3)				// ＭＩＤＩの演奏モードの数

// 描画モード定義
#define DX_DRAWMODE_NEAREST							(0)				// ネアレストネイバー法で描画
#define DX_DRAWMODE_BILINEAR						(1)				// バイリニア法で描画する
#define DX_DRAWMODE_ANISOTROPIC						(2)				// 異方性フィルタリング法で描画する
#define DX_DRAWMODE_OTHER							(3)				// それ以外
#define DX_DRAWMODE_NUM								(4)				// 描画モードの数

// フォントのタイプ
#define DX_FONTTYPE_NORMAL							(0x00)			// ノーマルフォント
#define DX_FONTTYPE_EDGE							(0x01)			// エッジつきフォント
#define DX_FONTTYPE_ANTIALIASING					(0x02)			// アンチエイリアスフォント( 標準機能アンチエイリアス )
#define DX_FONTTYPE_ANTIALIASING_4X4				(0x12)			// アンチエイリアスフォント( 4x4サンプリング )
#define DX_FONTTYPE_ANTIALIASING_8X8				(0x22)			// アンチエイリアスフォント( 8x8サンプリング )
#define DX_FONTTYPE_ANTIALIASING_EDGE				(0x03)			// アンチエイリアス＆エッジ付きフォント( 標準機能アンチエイリアス )
#define DX_FONTTYPE_ANTIALIASING_EDGE_4X4			(0x13)			// アンチエイリアス＆エッジ付きフォント( 4x4サンプリング )
#define DX_FONTTYPE_ANTIALIASING_EDGE_8X8			(0x23)			// アンチエイリアス＆エッジ付きフォント( 8x8サンプリング )

// フォント画像の階調ビット数
#define DX_FONTIMAGE_BIT_1							(0)
#define DX_FONTIMAGE_BIT_4							(1)
#define DX_FONTIMAGE_BIT_8							(2)

// 描画ブレンドモード定義
#define DX_BLENDMODE_NOBLEND						(0)				// ノーブレンド
#define DX_BLENDMODE_ALPHA							(1)				// αブレンド
#define DX_BLENDMODE_ADD							(2)				// 加算ブレンド
#define DX_BLENDMODE_SUB							(3)				// 減算ブレンド
#define DX_BLENDMODE_MUL							(4)				// 乗算ブレンド
   // (内部処理用)
#define DX_BLENDMODE_SUB2							(5)				// 内部処理用減算ブレンド２
//#define DX_BLENDMODE_BLINEALPHA					(7)				// 境界線ぼかし
#define DX_BLENDMODE_XOR							(6)				// XORブレンド( ソフトウエアレンダリングモードでのみ有効 )
#define DX_BLENDMODE_DESTCOLOR						(8)				// カラーは更新されない
#define DX_BLENDMODE_INVDESTCOLOR					(9)				// 描画先の色の反転値を掛ける
#define DX_BLENDMODE_INVSRC							(10)			// 描画元の色を反転する
#define DX_BLENDMODE_MULA							(11)			// アルファチャンネル考慮付き乗算ブレンド
#define DX_BLENDMODE_ALPHA_X4						(12)			// αブレンドの描画元の輝度を最大４倍にできるモード
#define DX_BLENDMODE_ADD_X4							(13)			// 加算ブレンドの描画元の輝度を最大４倍にできるモード
#define DX_BLENDMODE_SRCCOLOR						(14)			// 描画元のカラーでそのまま描画される
#define DX_BLENDMODE_HALF_ADD						(15)			// 半加算ブレンド
#define DX_BLENDMODE_SUB1							(16)			// 内部処理用減算ブレンド１
#define DX_BLENDMODE_PMA_ALPHA						(17)			// 乗算済みαブレンドモードのαブレンド
#define DX_BLENDMODE_PMA_ADD						(18)			// 乗算済みαブレンドモードの加算ブレンド
#define DX_BLENDMODE_PMA_SUB						(19)			// 乗算済みαブレンドモードの減算ブレンド
#define DX_BLENDMODE_PMA_INVSRC						(20)			// 乗算済みαブレンドモードの描画元の色を反転する
#define DX_BLENDMODE_PMA_ALPHA_X4					(21)			// 乗算済みαブレンドモードのαブレンドの描画元の輝度を最大４倍にできるモード
#define DX_BLENDMODE_PMA_ADD_X4						(22)			// 乗算済みαブレンドモードの加算ブレンドの描画元の輝度を最大４倍にできるモード
#define DX_BLENDMODE_NUM							(23)			// ブレンドモードの数

// DrawGraphF 等の浮動小数点値で座標を指定する関数における座標タイプ
#define DX_DRAWFLOATCOORDTYPE_DIRECT3D9				(0)				// Direct3D9タイプ( -0.5f の補正を行わないとテクスチャのピクセルが綺麗にマップされないタイプ )
#define DX_DRAWFLOATCOORDTYPE_DIRECT3D10			(1)				// Direct3D10タイプ( -0.5f の補正を行わななくてもテクスチャのピクセルが綺麗にマップされるタイプ )

// 画像合成タイプ
#define DX_BLENDGRAPHTYPE_NORMAL					(0)				// 通常合成
#define DX_BLENDGRAPHTYPE_WIPE						(1)				// ワイプ処理
#define DX_BLENDGRAPHTYPE_ALPHA						(2)				// ブレンド画像のα値と元画像のα値を掛け合わせる
#define DX_BLENDGRAPHTYPE_NUM						(3)

// 画像合成座標タイプ
#define DX_BLENDGRAPH_POSMODE_DRAWGRAPH				(0)				// 描画する画像基準で合成画像の座標を決定
#define DX_BLENDGRAPH_POSMODE_SCREEN				(1)				// スクリーン座標基準で合成画像の座標を決定
#define DX_BLENDGRAPH_POSMODE_NUM					(2)

// グラフィックフィルタータイプ
#define DX_GRAPH_FILTER_MONO						(0)				// モノトーンフィルタ
#define DX_GRAPH_FILTER_GAUSS						(1)				// ガウスフィルタ
#define DX_GRAPH_FILTER_DOWN_SCALE					(2)				// 縮小フィルタ
#define DX_GRAPH_FILTER_BRIGHT_CLIP					(3)				// 明るさクリップフィルタ
#define DX_GRAPH_FILTER_BRIGHT_SCALE				(4)				// 指定の明るさ領域を拡大するフィルタ
#define DX_GRAPH_FILTER_HSB							(5)				// 色相・彩度・明度フィルタ
#define DX_GRAPH_FILTER_INVERT						(6)				// 階調の反転フィルタ
#define DX_GRAPH_FILTER_LEVEL						(7)				// レベル補正フィルタ
#define DX_GRAPH_FILTER_TWO_COLOR					(8)				// ２階調化フィルタ
#define DX_GRAPH_FILTER_GRADIENT_MAP				(9)				// グラデーションマップフィルタ
#define DX_GRAPH_FILTER_PREMUL_ALPHA				(10)			// 通常のアルファチャンネル付き画像を乗算済みアルファ画像に変換するフィルタ
#define DX_GRAPH_FILTER_INTERP_ALPHA				(11)			// 乗算済みα画像を通常のアルファチャンネル付き画像に変換するフィルタ
#define DX_GRAPH_FILTER_YUV_TO_RGB					(12)			// YUVカラーをRGBカラーに変換するフィルタ
#define DX_GRAPH_FILTER_Y2UV1_TO_RGB				(13)			// YUVカラーをRGBカラーに変換するフィルタ( UV成分が Y成分の半分・又は４分の１( 横・縦片方若しくは両方 )の解像度しかない場合用 )
#define DX_GRAPH_FILTER_YUV_TO_RGB_RRA				(14)			// YUVカラーをRGBカラーに変換するフィルタ( 且つ右側半分のRの値をアルファ値として扱う )
#define DX_GRAPH_FILTER_Y2UV1_TO_RGB_RRA			(15)			// YUVカラーをRGBカラーに変換するフィルタ( UV成分が Y成分の半分・又は４分の１( 横・縦片方若しくは両方 )の解像度しかない場合用 )( 且つ右側半分のRの値をアルファ値として扱う )
#define DX_GRAPH_FILTER_BICUBIC_SCALE				(16)			// バイキュービックを使用した拡大・縮小フィルタ
#define DX_GRAPH_FILTER_LANCZOS3_SCALE				(17)			// Lanczos-3を使用した拡大・縮小フィルタ
#define DX_GRAPH_FILTER_PMA_BRIGHT_CLIP				(18)			// 明るさクリップフィルタ(乗算済みアルファ画像用)
#define DX_GRAPH_FILTER_PMA_BRIGHT_SCALE			(19)			// 指定の明るさ領域を拡大するフィルタ(乗算済みアルファ画像用)
#define DX_GRAPH_FILTER_PMA_HSB						(20)			// 色相・彩度・明度フィルタ(乗算済みアルファ画像用)
#define DX_GRAPH_FILTER_PMA_INVERT					(21)			// 階調の反転フィルタ(乗算済みアルファ画像用)
#define DX_GRAPH_FILTER_PMA_LEVEL					(22)			// レベル補正フィルタ(乗算済みアルファ画像用)
#define DX_GRAPH_FILTER_PMA_TWO_COLOR				(23)			// ２階調化フィルタ(乗算済みアルファ画像用)
#define DX_GRAPH_FILTER_PMA_GRADIENT_MAP			(24)			// グラデーションマップフィルタ(乗算済みアルファ画像用)
#define DX_GRAPH_FILTER_NUM							(25)

// グラフィックブレンドタイプ
#define DX_GRAPH_BLEND_NORMAL						(0)				// 通常
#define DX_GRAPH_BLEND_RGBA_SELECT_MIX				(1)				// RGBAの要素を選択して合成
#define DX_GRAPH_BLEND_MULTIPLE						(2)				// 乗算
#define DX_GRAPH_BLEND_DIFFERENCE					(3)				// 減算
#define DX_GRAPH_BLEND_ADD							(4)				// 加算
#define DX_GRAPH_BLEND_SCREEN						(5)				// スクリーン
#define DX_GRAPH_BLEND_OVERLAY						(6)				// オーバーレイ
#define DX_GRAPH_BLEND_DODGE						(7)				// 覆い焼き
#define DX_GRAPH_BLEND_BURN							(8)				// 焼き込み
#define DX_GRAPH_BLEND_DARKEN						(9)				// 比較(暗)
#define DX_GRAPH_BLEND_LIGHTEN						(10)			// 比較(明)
#define DX_GRAPH_BLEND_SOFTLIGHT					(11)			// ソフトライト
#define DX_GRAPH_BLEND_HARDLIGHT					(12)			// ハードライト
#define DX_GRAPH_BLEND_EXCLUSION					(13)			// 除外
#define DX_GRAPH_BLEND_NORMAL_ALPHACH				(14)			// αチャンネル付き画像の通常合成
#define DX_GRAPH_BLEND_ADD_ALPHACH					(15)			// αチャンネル付き画像の加算合成
#define DX_GRAPH_BLEND_MULTIPLE_A_ONLY				(16)			// アルファチャンネルのみの乗算
#define DX_GRAPH_BLEND_PMA_NORMAL					(17)			// 通常( 乗算済みα画像用 )
#define DX_GRAPH_BLEND_PMA_RGBA_SELECT_MIX			(18)			// RGBAの要素を選択して合成( 乗算済みα画像用 )
#define DX_GRAPH_BLEND_PMA_MULTIPLE					(19)			// 乗算( 乗算済みα画像用 )
#define DX_GRAPH_BLEND_PMA_DIFFERENCE				(20)			// 減算( 乗算済みα画像用 )
#define DX_GRAPH_BLEND_PMA_ADD						(21)			// 加算( 乗算済みα画像用 )
#define DX_GRAPH_BLEND_PMA_SCREEN					(22)			// スクリーン( 乗算済みα画像用 )
#define DX_GRAPH_BLEND_PMA_OVERLAY					(23)			// オーバーレイ( 乗算済みα画像用 )
#define DX_GRAPH_BLEND_PMA_DODGE					(24)			// 覆い焼き( 乗算済みα画像用 )
#define DX_GRAPH_BLEND_PMA_BURN						(25)			// 焼き込み( 乗算済みα画像用 )
#define DX_GRAPH_BLEND_PMA_DARKEN					(26)			// 比較(暗)( 乗算済みα画像用 )
#define DX_GRAPH_BLEND_PMA_LIGHTEN					(27)			// 比較(明)( 乗算済みα画像用 )
#define DX_GRAPH_BLEND_PMA_SOFTLIGHT				(28)			// ソフトライト( 乗算済みα画像用 )
#define DX_GRAPH_BLEND_PMA_HARDLIGHT				(29)			// ハードライト( 乗算済みα画像用 )
#define DX_GRAPH_BLEND_PMA_EXCLUSION				(30)			// 除外( 乗算済みα画像用 )
#define DX_GRAPH_BLEND_PMA_NORMAL_ALPHACH			(31)			// αチャンネル付き画像の通常合成( 乗算済みα画像用 )
#define DX_GRAPH_BLEND_PMA_ADD_ALPHACH				(32)			// αチャンネル付き画像の加算合成( 乗算済みα画像用 )
#define DX_GRAPH_BLEND_PMA_MULTIPLE_A_ONLY			(33)			// アルファチャンネルのみの乗算( 乗算済みα画像用 )
#define DX_GRAPH_BLEND_NUM							(34)

// DX_GRAPH_BLEND_RGBA_SELECT_MIX 用の色選択用定義
#define DX_RGBA_SELECT_SRC_R						(0)				// 元画像の赤成分
#define DX_RGBA_SELECT_SRC_G						(1)				// 元画像の緑成分
#define DX_RGBA_SELECT_SRC_B						(2)				// 元画像の青成分
#define DX_RGBA_SELECT_SRC_A						(3)				// 元画像のα成分
#define DX_RGBA_SELECT_BLEND_R						(4)				// ブレンド画像の赤成分
#define DX_RGBA_SELECT_BLEND_G						(5)				// ブレンド画像の緑成分
#define DX_RGBA_SELECT_BLEND_B						(6)				// ブレンド画像の青成分
#define DX_RGBA_SELECT_BLEND_A						(7)				// ブレンド画像のα成分

// フィルモード
#define DX_FILL_WIREFRAME							(2)				// ワイヤーフレーム
#define DX_FILL_SOLID								(3)				// ポリゴン

// ポリゴンカリングモード
#define DX_CULLING_NONE								(0)				// カリングなし
#define DX_CULLING_LEFT								(1)				// 背面を左回りでカリング
#define DX_CULLING_RIGHT							(2)				// 背面を右回りでカリング
#define DX_CULLING_NUM								(3)				// カリングモードの数

// クリッピング方向
#define DX_CAMERACLIP_LEFT							(0x01)			// 画面左方向にクリップ
#define DX_CAMERACLIP_RIGHT							(0x02)			// 画面右方向にクリップ
#define DX_CAMERACLIP_BOTTOM						(0x04)			// 画面下方向にクリップ
#define DX_CAMERACLIP_TOP							(0x08)			// 画面上方向にクリップ
#define DX_CAMERACLIP_BACK							(0x10)			// 画面後方向にクリップ
#define DX_CAMERACLIP_FRONT							(0x20)			// 画面前方向にクリップ

// MV1モデルの頂点タイプ
#define DX_MV1_VERTEX_TYPE_1FRAME					(0)				// １フレームの影響を受ける頂点
#define DX_MV1_VERTEX_TYPE_4FRAME					(1)				// １〜４フレームの影響を受ける頂点
#define DX_MV1_VERTEX_TYPE_8FRAME					(2)				// ５〜８フレームの影響を受ける頂点
#define DX_MV1_VERTEX_TYPE_FREE_FRAME				(3)				// ９フレーム以上の影響を受ける頂点
#define DX_MV1_VERTEX_TYPE_NMAP_1FRAME				(4)				// 法線マップ用の情報が含まれる１フレームの影響を受ける頂点
#define DX_MV1_VERTEX_TYPE_NMAP_4FRAME				(5)				// 法線マップ用の情報が含まれる１〜４フレームの影響を受ける頂点
#define DX_MV1_VERTEX_TYPE_NMAP_8FRAME				(6)				// 法線マップ用の情報が含まれる５〜８フレームの影響を受ける頂点
#define DX_MV1_VERTEX_TYPE_NMAP_FREE_FRAME			(7)				// 法線マップ用の情報が含まれる９フレーム以上の影響を受ける頂点
#define DX_MV1_VERTEX_TYPE_NUM						(8)				// 頂点タイプの数

// メッシュの種類
#define DX_MV1_MESHCATEGORY_NORMAL					(0)				// 普通のメッシュ
#define DX_MV1_MESHCATEGORY_OUTLINE					(1)				// 輪郭線描画用メッシュ
#define DX_MV1_MESHCATEGORY_OUTLINE_ORIG_SHADER		(2)				// 輪郭線描画用メッシュ( オリジナルシェーダーでの描画用 )
#define DX_MV1_MESHCATEGORY_NUM						(3)				// メッシュの種類の数

// シェイプ率の適用タイプ
#define DX_MV1_SHAPERATE_ADD						(0)				// 元の値に加算
#define DX_MV1_SHAPERATE_OVERWRITE					(1)				// 元の値に上書き

// MV1ファイルの保存タイプ
#define MV1_SAVETYPE_MESH							(0x0001)		// メッシュ情報のみ保存
#define MV1_SAVETYPE_ANIM							(0x0002)		// アニメーション情報のみ保存
#define MV1_SAVETYPE_NORMAL							( MV1_SAVETYPE_MESH | MV1_SAVETYPE_ANIM )	// 通常保存

// アニメーションキーデータタイプ
#define MV1_ANIMKEY_DATATYPE_ROTATE					(0)				// 回転
#define MV1_ANIMKEY_DATATYPE_ROTATE_X				(1)				// 回転Ｘ
#define MV1_ANIMKEY_DATATYPE_ROTATE_Y				(2)				// 回転Ｙ
#define MV1_ANIMKEY_DATATYPE_ROTATE_Z				(3)				// 回転Ｚ
#define MV1_ANIMKEY_DATATYPE_SCALE					(5)				// 拡大
#define MV1_ANIMKEY_DATATYPE_SCALE_X				(6)				// スケールＸ
#define MV1_ANIMKEY_DATATYPE_SCALE_Y				(7)				// スケールＹ
#define MV1_ANIMKEY_DATATYPE_SCALE_Z				(8)				// スケールＺ
#define MV1_ANIMKEY_DATATYPE_TRANSLATE				(10)			// 平行移動
#define MV1_ANIMKEY_DATATYPE_TRANSLATE_X			(11)			// 平行移動Ｘ
#define MV1_ANIMKEY_DATATYPE_TRANSLATE_Y			(12)			// 平行移動Ｙ
#define MV1_ANIMKEY_DATATYPE_TRANSLATE_Z			(13)			// 平行移動Ｚ
#define MV1_ANIMKEY_DATATYPE_MATRIX4X4C				(15)			// ４×４行列の４列目( 0,0,0,1 )固定版
#define MV1_ANIMKEY_DATATYPE_MATRIX3X3				(17)			// ３×３行列
#define MV1_ANIMKEY_DATATYPE_SHAPE					(18)			// シェイプ
#define MV1_ANIMKEY_DATATYPE_OTHRE					(20)			// その他

// タイムタイプ
#define MV1_ANIMKEY_TIME_TYPE_ONE					(0)				// 時間情報は全体で一つ
#define MV1_ANIMKEY_TIME_TYPE_KEY					(1)				// 時間情報は各キーに一つ

// アニメーションキータイプ
#define MV1_ANIMKEY_TYPE_QUATERNION_X				(0)				// クォータニオン( Xファイルタイプ )
#define MV1_ANIMKEY_TYPE_VECTOR						(1)				// ベクター
#define MV1_ANIMKEY_TYPE_MATRIX4X4C					(2)				// ４×４行列の４列目( 0,0,0,1 )固定版
#define MV1_ANIMKEY_TYPE_MATRIX3X3					(3)				// ３×３行列
#define MV1_ANIMKEY_TYPE_FLAT						(4)				// フラット
#define MV1_ANIMKEY_TYPE_LINEAR						(5)				// 線形補間
#define MV1_ANIMKEY_TYPE_BLEND						(6)				// 混合
#define MV1_ANIMKEY_TYPE_QUATERNION_VMD				(7)				// クォータニオン( VMDタイプ )

// 描画先画面指定用定義
#define DX_SCREEN_FRONT								(0xfffffffc)
#define DX_SCREEN_BACK								(0xfffffffe) 
#define DX_SCREEN_WORK 								(0xfffffffd)
#define DX_SCREEN_TEMPFRONT							(0xfffffff0)
#define DX_SCREEN_OTHER								(0xfffffffa)

#define DX_NONE_GRAPH								(0xfffffffb)	// グラフィックなしハンドル

// グラフィック減色時の画像劣化緩和処理モード
#define DX_SHAVEDMODE_NONE							(0)				// 画像劣化緩和処理を行わない
#define DX_SHAVEDMODE_DITHER						(1)				// ディザリング
#define DX_SHAVEDMODE_DIFFUS						(2)				// 誤差拡散

// 画像の保存タイプ
#define DX_IMAGESAVETYPE_BMP						(0)				// bitmap
#define DX_IMAGESAVETYPE_JPEG						(1)				// jpeg
#define DX_IMAGESAVETYPE_PNG						(2)				// Png
#define DX_IMAGESAVETYPE_DDS						(3)				// Direct Draw Surface

// サウンド再生形態指定用定義
#define DX_PLAYTYPE_LOOPBIT							(0x0002)		// ループ再生ビット
#define DX_PLAYTYPE_BACKBIT							(0x0001)		// バックグラウンド再生ビット

#define DX_PLAYTYPE_NORMAL							(0)												// ノーマル再生
#define DX_PLAYTYPE_BACK				  			( DX_PLAYTYPE_BACKBIT )							// バックグラウンド再生
#define DX_PLAYTYPE_LOOP							( DX_PLAYTYPE_LOOPBIT | DX_PLAYTYPE_BACKBIT )	// ループ再生

// 動画再生タイプ定義
#define DX_MOVIEPLAYTYPE_BCANCEL					(0)				// ボタンキャンセルあり
#define DX_MOVIEPLAYTYPE_NORMAL						(1)				// ボタンキャンセルなし

// サウンドのタイプ
#define DX_SOUNDTYPE_NORMAL							(0)				// ノーマルサウンド形式
#define DX_SOUNDTYPE_STREAMSTYLE					(1)				// ストリーム風サウンド形式

// サウンドデータタイプのマクロ
#define DX_SOUNDDATATYPE_MEMNOPRESS					(0)				// 圧縮された全データは再生が始まる前にサウンドメモリにすべて解凍され、格納される
#define DX_SOUNDDATATYPE_MEMNOPRESS_PLUS			(1)				// 圧縮された全データはシステムメモリに格納され、再生しながら逐次解凍され、最終的にすべてサウンドメモリに格納される(その後システムメモリに存在する圧縮データは破棄される)
#define DX_SOUNDDATATYPE_MEMPRESS					(2)				// 圧縮された全データはシステムメモリに格納され、再生する部分だけ逐次解凍しながらサウンドメモリに格納する(鳴らし終わると解凍したデータは破棄されるので何度も解凍処理が行われる)
#define DX_SOUNDDATATYPE_FILE						(3)				// 圧縮されたデータの再生する部分だけファイルから逐次読み込み解凍され、サウンドメモリに格納される(鳴らし終わると解凍したデータは破棄されるので何度も解凍処理が行われる)

// 読み込み処理のタイプ
#define DX_READSOUNDFUNCTION_PCM					(1 << 0)		// PCM の読み込み処理
#define DX_READSOUNDFUNCTION_OGG					(1 << 1)		// Ogg Vorbis の読み込み処理
#define DX_READSOUNDFUNCTION_OPUS					(1 << 2)		// Opus の読み込み処理
#define DX_READSOUNDFUNCTION_DEFAULT_NUM			(3)				// 環境非依存の読み込み処理タイプの数

// ３Ｄサウンドリバーブエフェクトのプリセット
#define DX_REVERB_PRESET_DEFAULT					(0)				// デフォルト
#define DX_REVERB_PRESET_GENERIC					(1)				// 一般的な空間
#define DX_REVERB_PRESET_PADDEDCELL					(2)				// 精神病患者室(？)
#define DX_REVERB_PRESET_ROOM						(3)				// 部屋
#define DX_REVERB_PRESET_BATHROOM					(4)				// バスルーム
#define DX_REVERB_PRESET_LIVINGROOM					(5)				// リビングルーム
#define DX_REVERB_PRESET_STONEROOM					(6)				// 石の部屋
#define DX_REVERB_PRESET_AUDITORIUM					(7)				// 講堂
#define DX_REVERB_PRESET_CONCERTHALL				(8)				// コンサートホール
#define DX_REVERB_PRESET_CAVE						(9)				// 洞穴
#define DX_REVERB_PRESET_ARENA						(10)			// 舞台
#define DX_REVERB_PRESET_HANGAR						(11)			// 格納庫
#define DX_REVERB_PRESET_CARPETEDHALLWAY			(12)			// カーペットが敷かれた玄関
#define DX_REVERB_PRESET_HALLWAY					(13)			// 玄関
#define DX_REVERB_PRESET_STONECORRIDOR				(14)			// 石の廊下
#define DX_REVERB_PRESET_ALLEY						(15)			// 裏通り
#define DX_REVERB_PRESET_FOREST						(16)			// 森
#define DX_REVERB_PRESET_CITY						(17)			// 都市
#define DX_REVERB_PRESET_MOUNTAINS					(18)			// 山
#define DX_REVERB_PRESET_QUARRY						(19)			// 採石場
#define DX_REVERB_PRESET_PLAIN						(20)			// 平原
#define DX_REVERB_PRESET_PARKINGLOT					(21)			// 駐車場
#define DX_REVERB_PRESET_SEWERPIPE					(22)			// 下水管
#define DX_REVERB_PRESET_UNDERWATER					(23)			// 水面下
#define DX_REVERB_PRESET_SMALLROOM					(24)			// 小部屋
#define DX_REVERB_PRESET_MEDIUMROOM					(25)			// 中部屋
#define DX_REVERB_PRESET_LARGEROOM					(26)			// 大部屋
#define DX_REVERB_PRESET_MEDIUMHALL					(27)			// 中ホール
#define DX_REVERB_PRESET_LARGEHALL					(28)			// 大ホール
#define DX_REVERB_PRESET_PLATE						(29)			// 板

#define DX_REVERB_PRESET_NUM						(30)			// プリセットの数

// マスク透過色モード
#define DX_MASKTRANS_WHITE							(0)				// マスク画像の白い部分を透過色とする
#define DX_MASKTRANS_BLACK							(1)				// マスク画像の黒い部分を透過色とする
#define DX_MASKTRANS_NONE							(2) 			// 透過色なし

// マスク画像チャンネル
#define DX_MASKGRAPH_CH_A							(0)				// アルファ
#define DX_MASKGRAPH_CH_R							(1)				// 赤
#define DX_MASKGRAPH_CH_G							(2)				// 緑
#define DX_MASKGRAPH_CH_B							(3)				// 青

// Ｚバッファ書き込みモード
#define DX_ZWRITE_MASK								(0)				// 書き込めないようにマスクする
#define DX_ZWRITE_CLEAR								(1)				// 書き込めるようにマスクをクリアする

// 比較モード
#define DX_CMP_NEVER								(1)				// FALSE
#define DX_CMP_LESS									(2)				// Src <  Dest		DrawAlpha <  TestParam
#define DX_CMP_EQUAL								(3)				// Src == Dest		DrawAlpha == TestParam
#define DX_CMP_LESSEQUAL							(4)				// Src <= Dest		DrawAlpha <= TestParam
#define DX_CMP_GREATER								(5)				// Src >  Dest		DrawAlpha >  TestParam
#define DX_CMP_NOTEQUAL								(6)				// Src != Dest		DrawAlpha != TestParam
#define DX_CMP_GREATEREQUAL							(7)				// Src >= Dest		DrawAlpha >= TestParam
#define DX_CMP_ALWAYS								(8)				// TRUE
#define DX_ZCMP_DEFAULT								( DX_CMP_LESSEQUAL )
#define DX_ZCMP_REVERSE								( DX_CMP_GREATEREQUAL )

// シェーディングモード
#define DX_SHADEMODE_FLAT							(1)				// D_D3DSHADE_FLAT
#define DX_SHADEMODE_GOURAUD						(2)				// D_D3DSHADE_GOURAUD

// フォグモード
#define DX_FOGMODE_NONE								(0)				// D_D3DFOG_NONE
#define DX_FOGMODE_EXP								(1)				// D_D3DFOG_EXP
#define DX_FOGMODE_EXP2								(2)				// D_D3DFOG_EXP2
#define DX_FOGMODE_LINEAR							(3)				// D_D3DFOG_LINEAR

// マテリアルタイプ
#define DX_MATERIAL_TYPE_NORMAL									(0)	// 標準マテリアル
#define DX_MATERIAL_TYPE_TOON									(1)	// トゥーンレンダリング用マテリアル
#define DX_MATERIAL_TYPE_TOON_2									(2)	// トゥーンレンダリング用マテリアル_タイプ２( MMD互換 )
#define DX_MATERIAL_TYPE_MAT_SPEC_LUMINANCE_UNORM				(3)	// マテリアルのスペキュラ色の輝度の指定の範囲の値を 0.0f 〜 1.0f の値に正規化して書き込むマテリアル
#define DX_MATERIAL_TYPE_MAT_SPEC_LUMINANCE_CLIP_UNORM			(4)	// DX_MATERIAL_TYPE_MAT_SPEC_LUMINANCE_UNORM に『指定の値未満の場合は書き込む値を 0.0f にする』という処理を加えたマテリアル
#define DX_MATERIAL_TYPE_MAT_SPEC_LUMINANCE_CMP_GREATEREQUAL	(5)	// マテリアルのスペキュラ色の輝度が指定の閾値以上 の場合は 1.0f を、未満の場合は 0.0f を書き込むマテリアル
#define DX_MATERIAL_TYPE_MAT_SPEC_POWER_UNORM					(6)	// マテリアルのスペキュラハイライトの鮮明度の指定の範囲の値を 0.0f 〜 1.0f の値に正規化して書き込むマテリアル
#define DX_MATERIAL_TYPE_MAT_SPEC_POWER_CLIP_UNORM				(7)	// DX_MATERIAL_TYPE_MAT_SPEC_POWER_UNORM に『指定の値未満の場合は書き込む値を 0.0f にする』という処理を加えたマテリアル
#define DX_MATERIAL_TYPE_MAT_SPEC_POWER_CMP_GREATEREQUAL		(8)	// マテリアルのスペキュラハイライトの鮮明度が指定の閾値以上 の場合は 1.0f を、未満の場合は 0.0f を書き込むマテリアル
#define DX_MATERIAL_TYPE_NUM									(9)

// マテリアルブレンドタイプ
#define DX_MATERIAL_BLENDTYPE_TRANSLUCENT			(0)				// アルファ合成
#define DX_MATERIAL_BLENDTYPE_ADDITIVE				(1)				// 加算
#define DX_MATERIAL_BLENDTYPE_MODULATE				(2)				// 乗算
#define DX_MATERIAL_BLENDTYPE_NONE					(3)				// 無効

// テクスチャアドレスタイプ
#define DX_TEXADDRESS_WRAP							(1)				// D_D3DTADDRESS_WRAP
#define DX_TEXADDRESS_MIRROR						(2)				// D_D3DTADDRESS_MIRROR
#define DX_TEXADDRESS_CLAMP							(3)				// D_D3DTADDRESS_CLAMP
#define DX_TEXADDRESS_BORDER						(4)				// D_D3DTADDRESS_BORDER
#define DX_TEXADDRESS_NUM							(5)				// テクスチャアドレスタイプの数

// シェーダータイプ
#define DX_SHADERTYPE_VERTEX						(0)				// 頂点シェーダー
#define DX_SHADERTYPE_PIXEL							(1)				// ピクセルシェーダー
#define DX_SHADERTYPE_GEOMETRY						(2)				// ジオメトリシェーダー
#define DX_SHADERTYPE_COMPUTE						(3)				// コンピュートシェーダー
#define DX_SHADERTYPE_DOMAIN						(4)				// ドメインシェーダー
#define DX_SHADERTYPE_HULL							(5)				// ハルシェーダー

// 頂点データタイプ
#define DX_VERTEX_TYPE_NORMAL_3D					(0)				// VERTEX3D構造体形式
#define DX_VERTEX_TYPE_SHADER_3D					(1)				// VERTEX3DSHADER構造体形式
#define DX_VERTEX_TYPE_NUM							(2)

// インデックスデータタイプ
#define DX_INDEX_TYPE_16BIT							(0)				// 16bitインデックス
#define DX_INDEX_TYPE_32BIT							(1)				// 32bitインデックス

// モデルファイル読み込み時の物理演算モード
#define DX_LOADMODEL_PHYSICS_DISABLE				(1)				// 物理演算を使用しない
#define DX_LOADMODEL_PHYSICS_LOADCALC				(0)				// 読み込み時に計算
#define DX_LOADMODEL_PHYSICS_REALTIME				(2)				// 実行時計算

// モデルファイル読み込み時の物理演算無効名前ワードのモード
#define DX_LOADMODEL_PHYSICS_DISABLENAMEWORD_ALWAYS	(0)						// 全てのファイルに対して無効名ワードを適用する
#define DX_LOADMODEL_PHYSICS_DISABLENAMEWORD_DISABLEPHYSICSFILEONLY	(1)		// vmdファイル名に NP を含めた「物理演算無効」のファイルに対してのみ無効名ワードを適用する( この場合、無効名ワードが適用されない剛体については物理演算が行われる )
#define DX_LOADMODEL_PHYSICS_DISABLENAMEWORD_NUM	(2)

// PMD, PMX ファイル読み込み時のアニメーションの FPS モード( 主に IK 部分の精度に影響します )
#define DX_LOADMODEL_PMD_PMX_ANIMATION_FPSMODE_30	(0)				// アニメーションを 30FPS で読み込む( IK部分の精度:低  データサイズ:小 )( デフォルト )
#define DX_LOADMODEL_PMD_PMX_ANIMATION_FPSMODE_60	(1)				// アニメーションを 60FPS で読み込む( IK部分の精度:高  データサイズ:大 )

// モデルの半透明要素がある箇所に関する描画モード
#define DX_SEMITRANSDRAWMODE_ALWAYS					(0)				// 半透明かどうか関係なく描画する
#define DX_SEMITRANSDRAWMODE_SEMITRANS_ONLY			(1)				// 半透明のもののみ描画する
#define DX_SEMITRANSDRAWMODE_NOT_SEMITRANS_ONLY		(2)				// 半透明ではないもののみ描画する

// キューブマップの面番号
#define DX_CUBEMAP_FACE_POSITIVE_X					(0)
#define DX_CUBEMAP_FACE_NEGATIVE_X					(1)
#define DX_CUBEMAP_FACE_POSITIVE_Y					(2)
#define DX_CUBEMAP_FACE_NEGATIVE_Y					(3)
#define DX_CUBEMAP_FACE_POSITIVE_Z					(4)
#define DX_CUBEMAP_FACE_NEGATIVE_Z					(5)

// ポリゴン描画タイプ
#define DX_PRIMTYPE_POINTLIST						(1)				// D_D3DPT_POINTLIST
#define DX_PRIMTYPE_LINELIST						(2)				// D_D3DPT_LINELIST
#define DX_PRIMTYPE_LINESTRIP						(3)				// D_D3DPT_LINESTRIP
#define DX_PRIMTYPE_TRIANGLELIST					(4)				// D_D3DPT_TRIANGLELIST
#define DX_PRIMTYPE_TRIANGLESTRIP					(5)				// D_D3DPT_TRIANGLESTRIP
#define DX_PRIMTYPE_TRIANGLEFAN						(6)				// D_D3DPT_TRIANGLEFAN
#define DX_PRIMTYPE_MIN								(1)
#define DX_PRIMTYPE_MAX								(6)

// ライトタイプ
#define DX_LIGHTTYPE_D3DLIGHT_POINT					(1)				// D_D3DLIGHT_POINT
#define DX_LIGHTTYPE_D3DLIGHT_SPOT					(2)				// D_D3DLIGHT_SPOT
#define DX_LIGHTTYPE_D3DLIGHT_DIRECTIONAL			(3)				// D_D3DLIGHT_DIRECTIONAL
#define DX_LIGHTTYPE_POINT							(1)				// D_D3DLIGHT_POINT
#define DX_LIGHTTYPE_SPOT							(2)				// D_D3DLIGHT_SPOT
#define DX_LIGHTTYPE_DIRECTIONAL					(3)				// D_D3DLIGHT_DIRECTIONAL

// グラフィックイメージフォーマットの定義
#define DX_GRAPHICSIMAGE_FORMAT_3D_PAL4						(0)		// １６色パレットカラー標準
#define DX_GRAPHICSIMAGE_FORMAT_3D_PAL8						(1)		// ２５６色パレットカラー標準
#define DX_GRAPHICSIMAGE_FORMAT_3D_ALPHA_PAL4				(2)		// αチャンネルつき１６色パレットカラー標準
#define DX_GRAPHICSIMAGE_FORMAT_3D_ALPHA_PAL8				(3)		// αチャンネルつき２５６色パレットカラー標準
#define DX_GRAPHICSIMAGE_FORMAT_3D_ALPHATEST_PAL4			(4)		// αテストつき１６色パレットカラー標準
#define DX_GRAPHICSIMAGE_FORMAT_3D_ALPHATEST_PAL8			(5)		// αテストつき２５６色パレットカラー標準
#define DX_GRAPHICSIMAGE_FORMAT_3D_RGB16					(6)		// １６ビットカラー標準
#define DX_GRAPHICSIMAGE_FORMAT_3D_RGB32					(7)		// ３２ビットカラー標準
#define DX_GRAPHICSIMAGE_FORMAT_3D_ALPHA_RGB16				(8)		// αチャンネル付き１６ビットカラー
#define DX_GRAPHICSIMAGE_FORMAT_3D_ALPHA_RGB32				(9)		// αチャンネル付き３２ビットカラー
#define DX_GRAPHICSIMAGE_FORMAT_3D_ALPHATEST_RGB16			(10)	// αテスト付き１６ビットカラー
#define DX_GRAPHICSIMAGE_FORMAT_3D_ALPHATEST_RGB32			(11)	// αテスト付き３２ビットカラー
#define DX_GRAPHICSIMAGE_FORMAT_3D_DXT1						(12)	// ＤＸＴ１
#define DX_GRAPHICSIMAGE_FORMAT_3D_DXT2						(13)	// ＤＸＴ２
#define DX_GRAPHICSIMAGE_FORMAT_3D_DXT3						(14)	// ＤＸＴ３
#define DX_GRAPHICSIMAGE_FORMAT_3D_DXT4						(15)	// ＤＸＴ４
#define DX_GRAPHICSIMAGE_FORMAT_3D_DXT5 					(16)	// ＤＸＴ５
#define DX_GRAPHICSIMAGE_FORMAT_3D_PLATFORM0				(17)	// プラットフォーム依存フォーマット０
#define DX_GRAPHICSIMAGE_FORMAT_3D_PLATFORM1				(18)	// プラットフォーム依存フォーマット１
#define DX_GRAPHICSIMAGE_FORMAT_3D_PLATFORM2				(19)	// プラットフォーム依存フォーマット２
#define DX_GRAPHICSIMAGE_FORMAT_3D_PLATFORM3				(20)	// プラットフォーム依存フォーマット３
#define DX_GRAPHICSIMAGE_FORMAT_3D_YUV	 					(21)	// ＹＵＶフォーマット
#define DX_GRAPHICSIMAGE_FORMAT_3D_ABGR_I16					(22)	// ARGB整数16ビット型カラー
#define DX_GRAPHICSIMAGE_FORMAT_3D_ABGR_F16					(23)	// ARGB浮動小数点16ビット型カラー
#define DX_GRAPHICSIMAGE_FORMAT_3D_ABGR_F32					(24)	// ARGB浮動小数点32ビット型カラー
#define DX_GRAPHICSIMAGE_FORMAT_3D_ONE_I8					(25)	// １チャンネル整数8ビット型カラー
#define DX_GRAPHICSIMAGE_FORMAT_3D_ONE_I16					(26)	// １チャンネル整数16ビット型カラー
#define DX_GRAPHICSIMAGE_FORMAT_3D_ONE_F16					(27)	// １チャンネル浮動少数16ビット型カラー
#define DX_GRAPHICSIMAGE_FORMAT_3D_ONE_F32					(28)	// １チャンネル浮動少数32ビット型カラー
#define DX_GRAPHICSIMAGE_FORMAT_3D_TWO_I8					(29)	// ２チャンネル整数8ビット型カラー
#define DX_GRAPHICSIMAGE_FORMAT_3D_TWO_I16					(30)	// ２チャンネル整数16ビット型カラー
#define DX_GRAPHICSIMAGE_FORMAT_3D_TWO_F16					(31)	// ２チャンネル浮動少数16ビット型カラー
#define DX_GRAPHICSIMAGE_FORMAT_3D_TWO_F32					(32)	// ２チャンネル浮動少数32ビット型カラー
#define DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_RGB16			(33)	// 描画可能１６ビットカラー
#define DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_RGB32			(34)	// 描画可能３２ビットカラー
#define DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ALPHA_RGB32	(35)	// 描画可能α付き３２ビットカラー
#define DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ABGR_I16		(36)	// 描画可能ARGB整数16ビット型カラー
#define DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ABGR_F16		(37)	// 描画可能ARGB浮動小数点16ビット型カラー
#define DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ABGR_F32		(38)	// 描画可能ARGB浮動小数点32ビット型カラー
#define DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_I8			(39)	// 描画可能１チャンネル整数8ビット型カラー
#define DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_I16		(40)	// 描画可能１チャンネル整数16ビット型カラー
#define DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_F16		(41)	// 描画可能１チャンネル浮動少数16ビット型カラー
#define DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_ONE_F32		(42)	// 描画可能１チャンネル浮動少数32ビット型カラー
#define DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_I8			(43)	// 描画可能２チャンネル整数8ビット型カラー
#define DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_I16		(44)	// 描画可能２チャンネル整数16ビット型カラー
#define DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_F16		(45)	// 描画可能２チャンネル浮動少数16ビット型カラー
#define DX_GRAPHICSIMAGE_FORMAT_3D_DRAWVALID_TWO_F32		(46)	// 描画可能２チャンネル浮動少数32ビット型カラー
#define DX_GRAPHICSIMAGE_FORMAT_3D_NUM						(47)
#define DX_GRAPHICSIMAGE_FORMAT_2D							(48)	// 標準( DirectDrawSurface の場合はこれのみ )
#define DX_GRAPHICSIMAGE_FORMAT_R5G6B5						(49)	// R5G6B5( MEMIMG 用 )
#define DX_GRAPHICSIMAGE_FORMAT_X8A8R5G6B5					(50)	// X8A8R5G6B5( MEMIMG 用 )
#define DX_GRAPHICSIMAGE_FORMAT_X8R8G8B8					(51)	// X8R8G8B8( MEMIMG 用 )
#define DX_GRAPHICSIMAGE_FORMAT_A8R8G8B8					(52)	// A8R8G8B8( MEMIMG 用 )

#define DX_GRAPHICSIMAGE_FORMAT_NUM							(53)	// グラフィックフォーマットの種類の数

// 基本イメージのピクセルフォーマット
#define DX_BASEIMAGE_FORMAT_NORMAL					(0)				// 普通の画像
#define DX_BASEIMAGE_FORMAT_DXT1					(1)				// ＤＸＴ１
#define DX_BASEIMAGE_FORMAT_DXT2					(2)				// ＤＸＴ２
#define DX_BASEIMAGE_FORMAT_DXT3					(3)				// ＤＸＴ３
#define DX_BASEIMAGE_FORMAT_DXT4					(4)				// ＤＸＴ４
#define DX_BASEIMAGE_FORMAT_DXT5					(5)				// ＤＸＴ５
#define DX_BASEIMAGE_FORMAT_PLATFORM0				(6)				// プラットフォーム依存フォーマット０
#define DX_BASEIMAGE_FORMAT_PLATFORM1				(7)				// プラットフォーム依存フォーマット１
#define DX_BASEIMAGE_FORMAT_PLATFORM2				(8)				// プラットフォーム依存フォーマット２
#define DX_BASEIMAGE_FORMAT_PLATFORM3				(9)				// プラットフォーム依存フォーマット３
#define DX_BASEIMAGE_FORMAT_YUV						(10)			// ＹＵＶ

// ムービーのサーフェスモード
#define DX_MOVIESURFACE_NORMAL						(0)
#define DX_MOVIESURFACE_OVERLAY						(1)
#define DX_MOVIESURFACE_FULLCOLOR					(2)

// ウインドウの奥行き位置設定タイプ
#define DX_WIN_ZTYPE_NORMAL							(0)				// 通常設定
#define DX_WIN_ZTYPE_BOTTOM							(1)				// 全てのウインドウの一番奥に配置する
#define DX_WIN_ZTYPE_TOP							(2)				// 全てのウインドウの一番手前に配置する
#define DX_WIN_ZTYPE_TOPMOST						(3)				// 全てのウインドウの一番手前に配置する( ウインドウがアクティブではないときも最前面に表示される )

// ツールバーのボタンの状態
#define TOOLBUTTON_STATE_ENABLE						(0)				// 入力可能な状態
#define TOOLBUTTON_STATE_PRESSED					(1)				// 押されている状態
#define TOOLBUTTON_STATE_DISABLE					(2)				// 入力不可能な状態
#define TOOLBUTTON_STATE_PRESSED_DISABLE			(3)				// 押されている状態で、入力不可能な状態
#define TOOLBUTTON_STATE_NUM						(4)				// ツールバーのボタンの状態の数

// ツールバーのボタンのタイプ
#define TOOLBUTTON_TYPE_NORMAL						(0)				// 普通のボタン
#define TOOLBUTTON_TYPE_CHECK						(1)				// 押すごとにＯＮ／ＯＦＦが切り替わるボタン
#define TOOLBUTTON_TYPE_GROUP						(2)				// 別の TOOLBUTTON_TYPE_GROUP タイプのボタンが押されるとＯＦＦになるタイプのボタン(グループの区切りは隙間で)
#define TOOLBUTTON_TYPE_SEP							(3)				// 隙間(ボタンではありません)
#define TOOLBUTTON_TYPE_NUM							(4)				// ツールバーのボタンのタイプの数

// 親メニューのＩＤ
#define MENUITEM_IDTOP								(0xabababab)

// メニューに追加する際のタイプ
#define MENUITEM_ADD_CHILD							(0)				// 指定の項目の子として追加する
#define MENUITEM_ADD_INSERT							(1)				// 指定の項目と指定の項目より一つ上の項目の間に追加する

// メニューの横に付くマークタイプ
#define MENUITEM_MARK_NONE							(0)				// 何も付け無い
#define MENUITEM_MARK_CHECK							(1)				// チェックマーク
#define MENUITEM_MARK_RADIO							(2)				// ラジオボタン

// 文字変換タイプ定義
#define DX_NUMMODE_10								(0)				// １０進数
#define DX_NUMMODE_16								(1)				// １６進数
#define DX_STRMODE_NOT0								(2)				// 空きを０で埋めない
#define DX_STRMODE_USE0								(3)				// 空きを０で埋める

// CheckHitKeyAll で調べる入力タイプ
#define DX_CHECKINPUT_KEY							(0x0001)		// キー入力を調べる
#define DX_CHECKINPUT_PAD							(0x0002)		// パッド入力を調べる
#define DX_CHECKINPUT_MOUSE							(0x0004)		// マウスボタン入力を調べる
#define DX_CHECKINPUT_ALL							(DX_CHECKINPUT_KEY | DX_CHECKINPUT_PAD | DX_CHECKINPUT_MOUSE)	// すべての入力を調べる

// パッド入力取得パラメータ
#define DX_INPUT_KEY_PAD1							(0x1001)		// キー入力とパッド１入力
#define DX_INPUT_PAD1								(0x0001)		// パッド１入力
#define DX_INPUT_PAD2								(0x0002)		// パッド２入力
#define DX_INPUT_PAD3								(0x0003)		// パッド３入力
#define DX_INPUT_PAD4								(0x0004)		// パッド４入力
#define DX_INPUT_PAD5								(0x0005)		// パッド５入力
#define DX_INPUT_PAD6								(0x0006)		// パッド６入力
#define DX_INPUT_PAD7								(0x0007)		// パッド７入力
#define DX_INPUT_PAD8								(0x0008)		// パッド８入力
#define DX_INPUT_PAD9								(0x0009)		// パッド９入力
#define DX_INPUT_PAD10								(0x000a)		// パッド１０入力
#define DX_INPUT_PAD11								(0x000b)		// パッド１１入力
#define DX_INPUT_PAD12								(0x000c)		// パッド１２入力
#define DX_INPUT_PAD13								(0x000d)		// パッド１３入力
#define DX_INPUT_PAD14								(0x000e)		// パッド１４入力
#define DX_INPUT_PAD15								(0x000f)		// パッド１５入力
#define DX_INPUT_PAD16								(0x0010)		// パッド１６入力
#define DX_INPUT_KEY								(0x1000)		// キー入力

// タッチの同時接触検出対応最大数
#define TOUCHINPUTPOINT_MAX							(16)

// パッド入力定義
#define PAD_INPUT_DOWN								(0x00000001)	// ↓チェックマスク
#define PAD_INPUT_LEFT								(0x00000002)	// ←チェックマスク
#define PAD_INPUT_RIGHT								(0x00000004)	// →チェックマスク
#define PAD_INPUT_UP								(0x00000008)	// ↑チェックマスク
#define PAD_INPUT_A									(0x00000010)	// Ａボタンチェックマスク
#define PAD_INPUT_B									(0x00000020)	// Ｂボタンチェックマスク
#define PAD_INPUT_C									(0x00000040)	// Ｃボタンチェックマスク
#define PAD_INPUT_X									(0x00000080)	// Ｘボタンチェックマスク
#define PAD_INPUT_Y									(0x00000100)	// Ｙボタンチェックマスク
#define PAD_INPUT_Z									(0x00000200)	// Ｚボタンチェックマスク
#define PAD_INPUT_L									(0x00000400)	// Ｌボタンチェックマスク
#define PAD_INPUT_R									(0x00000800)	// Ｒボタンチェックマスク
#define PAD_INPUT_START								(0x00001000)	// ＳＴＡＲＴボタンチェックマスク
#define PAD_INPUT_M									(0x00002000)	// Ｍボタンチェックマスク
#define PAD_INPUT_D									(0x00004000)
#define PAD_INPUT_F									(0x00008000)
#define PAD_INPUT_G									(0x00010000)
#define PAD_INPUT_H									(0x00020000)
#define PAD_INPUT_I									(0x00040000)
#define PAD_INPUT_J									(0x00080000)
#define PAD_INPUT_K									(0x00100000)
#define PAD_INPUT_LL								(0x00200000)
#define PAD_INPUT_N									(0x00400000)
#define PAD_INPUT_O									(0x00800000)
#define PAD_INPUT_P									(0x01000000)
#define PAD_INPUT_RR								(0x02000000)
#define PAD_INPUT_S									(0x04000000)
#define PAD_INPUT_T									(0x08000000)
#define PAD_INPUT_U									(0x10000000)
#define PAD_INPUT_V									(0x20000000)
#define PAD_INPUT_W									(0x40000000)
#define PAD_INPUT_XX								(0x80000000)

#define PAD_INPUT_1									(0x00000010)
#define PAD_INPUT_2									(0x00000020)
#define PAD_INPUT_3									(0x00000040)
#define PAD_INPUT_4									(0x00000080)
#define PAD_INPUT_5									(0x00000100)
#define PAD_INPUT_6									(0x00000200)
#define PAD_INPUT_7									(0x00000400)
#define PAD_INPUT_8									(0x00000800)
#define PAD_INPUT_9									(0x00001000)
#define PAD_INPUT_10								(0x00002000)
#define PAD_INPUT_11								(0x00004000)
#define PAD_INPUT_12								(0x00008000)
#define PAD_INPUT_13								(0x00010000)
#define PAD_INPUT_14								(0x00020000)
#define PAD_INPUT_15								(0x00040000)
#define PAD_INPUT_16								(0x00080000)
#define PAD_INPUT_17								(0x00100000)
#define PAD_INPUT_18								(0x00200000)
#define PAD_INPUT_19								(0x00400000)
#define PAD_INPUT_20								(0x00800000)
#define PAD_INPUT_21								(0x01000000)
#define PAD_INPUT_22								(0x02000000)
#define PAD_INPUT_23								(0x04000000)
#define PAD_INPUT_24								(0x08000000)
#define PAD_INPUT_25								(0x10000000)
#define PAD_INPUT_26								(0x20000000)
#define PAD_INPUT_27								(0x40000000)
#define PAD_INPUT_28								(0x80000000)

// XInputボタン入力定義
#define XINPUT_BUTTON_DPAD_UP						(0)				// デジタル方向ボタン上
#define XINPUT_BUTTON_DPAD_DOWN						(1)				// デジタル方向ボタン下
#define XINPUT_BUTTON_DPAD_LEFT						(2)				// デジタル方向ボタン左
#define XINPUT_BUTTON_DPAD_RIGHT					(3)				// デジタル方向ボタン右
#define XINPUT_BUTTON_START							(4)				// STARTボタン
#define XINPUT_BUTTON_BACK							(5)				// BACKボタン
#define XINPUT_BUTTON_LEFT_THUMB					(6)				// 左スティック押し込み
#define XINPUT_BUTTON_RIGHT_THUMB					(7)				// 右スティック押し込み
#define XINPUT_BUTTON_LEFT_SHOULDER					(8)				// LBボタン
#define XINPUT_BUTTON_RIGHT_SHOULDER				(9)				// RBボタン
#define XINPUT_BUTTON_A								(12)			// Aボタン
#define XINPUT_BUTTON_B								(13)			// Bボタン
#define XINPUT_BUTTON_X								(14)			// Xボタン
#define XINPUT_BUTTON_Y								(15)			// Yボタン

// マウス入力定義
#define MOUSE_INPUT_LEFT							(0x0001)		// マウス左ボタン
#define MOUSE_INPUT_RIGHT							(0x0002)		// マウス右ボタン
#define MOUSE_INPUT_MIDDLE							(0x0004)		// マウス中央ボタン
#define MOUSE_INPUT_1								(0x0001)		// マウス１ボタン
#define MOUSE_INPUT_2								(0x0002)		// マウス２ボタン
#define MOUSE_INPUT_3								(0x0004)		// マウス３ボタン
#define MOUSE_INPUT_4								(0x0008)		// マウス４ボタン
#define MOUSE_INPUT_5								(0x0010)		// マウス５ボタン
#define MOUSE_INPUT_6								(0x0020)		// マウス６ボタン
#define MOUSE_INPUT_7								(0x0040)		// マウス７ボタン
#define MOUSE_INPUT_8								(0x0080)		// マウス８ボタン

// マウスのログ情報タイプ
#define MOUSE_INPUT_LOG_DOWN						(0)				// ボタンを押した
#define MOUSE_INPUT_LOG_UP							(1)				// ボタンを離した

// キー定義
#define KEY_INPUT_BACK								(0x0E)			// BackSpaceキー	D_DIK_BACK
#define KEY_INPUT_TAB								(0x0F)			// Tabキー			D_DIK_TAB
#define KEY_INPUT_RETURN							(0x1C)			// Enterキー		D_DIK_RETURN

#define KEY_INPUT_LSHIFT							(0x2A)			// 左Shiftキー		D_DIK_LSHIFT
#define KEY_INPUT_RSHIFT							(0x36)			// 右Shiftキー		D_DIK_RSHIFT
#define KEY_INPUT_LCONTROL							(0x1D)			// 左Ctrlキー		D_DIK_LCONTROL
#define KEY_INPUT_RCONTROL							(0x9D)			// 右Ctrlキー		D_DIK_RCONTROL
#define KEY_INPUT_ESCAPE							(0x01)			// Escキー			D_DIK_ESCAPE
#define KEY_INPUT_SPACE								(0x39)			// スペースキー		D_DIK_SPACE
#define KEY_INPUT_PGUP								(0xC9)			// PageUpキー		D_DIK_PGUP
#define KEY_INPUT_PGDN								(0xD1)			// PageDownキー		D_DIK_PGDN
#define KEY_INPUT_END								(0xCF)			// Endキー			D_DIK_END
#define KEY_INPUT_HOME								(0xC7)			// Homeキー			D_DIK_HOME
#define KEY_INPUT_LEFT								(0xCB)			// 左キー			D_DIK_LEFT
#define KEY_INPUT_UP								(0xC8)			// 上キー			D_DIK_UP
#define KEY_INPUT_RIGHT								(0xCD)			// 右キー			D_DIK_RIGHT
#define KEY_INPUT_DOWN								(0xD0)			// 下キー			D_DIK_DOWN
#define KEY_INPUT_INSERT							(0xD2)			// Insertキー		D_DIK_INSERT
#define KEY_INPUT_DELETE							(0xD3)			// Deleteキー		D_DIK_DELETE

#define KEY_INPUT_MINUS								(0x0C)			// −キー			D_DIK_MINUS
#define KEY_INPUT_YEN								(0x7D)			// ￥キー			D_DIK_YEN
#define KEY_INPUT_PREVTRACK							(0x90)			// ＾キー			D_DIK_PREVTRACK
#define KEY_INPUT_PERIOD							(0x34)			// ．キー			D_DIK_PERIOD
#define KEY_INPUT_SLASH								(0x35)			// ／キー			D_DIK_SLASH
#define KEY_INPUT_LALT								(0x38)			// 左Altキー		D_DIK_LALT
#define KEY_INPUT_RALT								(0xB8)			// 右Altキー		D_DIK_RALT
#define KEY_INPUT_SCROLL							(0x46)			// ScrollLockキー	D_DIK_SCROLL
#define KEY_INPUT_SEMICOLON							(0x27)			// ；キー			D_DIK_SEMICOLON
#define KEY_INPUT_COLON								(0x92)			// ：キー			D_DIK_COLON
#define KEY_INPUT_LBRACKET							(0x1A)			// ［キー			D_DIK_LBRACKET
#define KEY_INPUT_RBRACKET							(0x1B)			// ］キー			D_DIK_RBRACKET
#define KEY_INPUT_AT								(0x91)			// ＠キー			D_DIK_AT
#define KEY_INPUT_BACKSLASH							(0x2B)			// ＼キー			D_DIK_BACKSLASH
#define KEY_INPUT_COMMA								(0x33)			// ，キー			D_DIK_COMMA
#define KEY_INPUT_KANJI								(0x94)			// 漢字キー			D_DIK_KANJI
#define KEY_INPUT_CONVERT							(0x79)			// 変換キー			D_DIK_CONVERT
#define KEY_INPUT_NOCONVERT							(0x7B)			// 無変換キー		D_DIK_NOCONVERT
#define KEY_INPUT_KANA								(0x70)			// カナキー			D_DIK_KANA
#define KEY_INPUT_APPS								(0xDD)			// アプリケーションメニューキー		D_DIK_APPS
#define KEY_INPUT_CAPSLOCK							(0x3A)			// CaspLockキー		D_DIK_CAPSLOCK
#define KEY_INPUT_SYSRQ								(0xB7)			// PrintScreenキー	D_DIK_SYSRQ
#define KEY_INPUT_PAUSE								(0xC5)			// PauseBreakキー	D_DIK_PAUSE
#define KEY_INPUT_LWIN								(0xDB)			// 左Winキー		D_DIK_LWIN
#define KEY_INPUT_RWIN								(0xDC)			// 右Winキー		D_DIK_RWIN

#define KEY_INPUT_NUMLOCK							(0x45)			// テンキーNumLockキー		D_DIK_NUMLOCK
#define KEY_INPUT_NUMPAD0							(0x52)			// テンキー０				D_DIK_NUMPAD0
#define KEY_INPUT_NUMPAD1							(0x4F)			// テンキー１				D_DIK_NUMPAD1
#define KEY_INPUT_NUMPAD2							(0x50)			// テンキー２				D_DIK_NUMPAD2
#define KEY_INPUT_NUMPAD3							(0x51)			// テンキー３				D_DIK_NUMPAD3
#define KEY_INPUT_NUMPAD4							(0x4B)			// テンキー４				D_DIK_NUMPAD4
#define KEY_INPUT_NUMPAD5							(0x4C)			// テンキー５				D_DIK_NUMPAD5
#define KEY_INPUT_NUMPAD6							(0x4D)			// テンキー６				D_DIK_NUMPAD6
#define KEY_INPUT_NUMPAD7							(0x47)			// テンキー７				D_DIK_NUMPAD7
#define KEY_INPUT_NUMPAD8							(0x48)			// テンキー８				D_DIK_NUMPAD8
#define KEY_INPUT_NUMPAD9							(0x49)			// テンキー９				D_DIK_NUMPAD9
#define KEY_INPUT_MULTIPLY							(0x37)			// テンキー＊キー			D_DIK_MULTIPLY
#define KEY_INPUT_ADD								(0x4E)			// テンキー＋キー			D_DIK_ADD
#define KEY_INPUT_SUBTRACT							(0x4A)			// テンキー−キー			D_DIK_SUBTRACT
#define KEY_INPUT_DECIMAL							(0x53)			// テンキー．キー			D_DIK_DECIMAL
#define KEY_INPUT_DIVIDE							(0xB5)			// テンキー／キー			D_DIK_DIVIDE
#define KEY_INPUT_NUMPADENTER						(0x9C)			// テンキーのエンターキー	D_DIK_NUMPADENTER

#define KEY_INPUT_F1								(0x3B)			// Ｆ１キー			D_DIK_F1
#define KEY_INPUT_F2								(0x3C)			// Ｆ２キー			D_DIK_F2
#define KEY_INPUT_F3								(0x3D)			// Ｆ３キー			D_DIK_F3
#define KEY_INPUT_F4								(0x3E)			// Ｆ４キー			D_DIK_F4
#define KEY_INPUT_F5								(0x3F)			// Ｆ５キー			D_DIK_F5
#define KEY_INPUT_F6								(0x40)			// Ｆ６キー			D_DIK_F6
#define KEY_INPUT_F7								(0x41)			// Ｆ７キー			D_DIK_F7
#define KEY_INPUT_F8								(0x42)			// Ｆ８キー			D_DIK_F8
#define KEY_INPUT_F9								(0x43)			// Ｆ９キー			D_DIK_F9
#define KEY_INPUT_F10								(0x44)			// Ｆ１０キー		D_DIK_F10
#define KEY_INPUT_F11								(0x57)			// Ｆ１１キー		D_DIK_F11
#define KEY_INPUT_F12								(0x58)			// Ｆ１２キー		D_DIK_F12

#define KEY_INPUT_A									(0x1E)			// Ａキー			D_DIK_A
#define KEY_INPUT_B									(0x30)			// Ｂキー			D_DIK_B
#define KEY_INPUT_C									(0x2E)			// Ｃキー			D_DIK_C
#define KEY_INPUT_D									(0x20)			// Ｄキー			D_DIK_D
#define KEY_INPUT_E									(0x12)			// Ｅキー			D_DIK_E
#define KEY_INPUT_F									(0x21)			// Ｆキー			D_DIK_F
#define KEY_INPUT_G									(0x22)			// Ｇキー			D_DIK_G
#define KEY_INPUT_H									(0x23)			// Ｈキー			D_DIK_H
#define KEY_INPUT_I									(0x17)			// Ｉキー			D_DIK_I
#define KEY_INPUT_J									(0x24)			// Ｊキー			D_DIK_J
#define KEY_INPUT_K									(0x25)			// Ｋキー			D_DIK_K
#define KEY_INPUT_L									(0x26)			// Ｌキー			D_DIK_L
#define KEY_INPUT_M									(0x32)			// Ｍキー			D_DIK_M
#define KEY_INPUT_N									(0x31)			// Ｎキー			D_DIK_N
#define KEY_INPUT_O									(0x18)			// Ｏキー			D_DIK_O
#define KEY_INPUT_P									(0x19)			// Ｐキー			D_DIK_P
#define KEY_INPUT_Q									(0x10)			// Ｑキー			D_DIK_Q
#define KEY_INPUT_R									(0x13)			// Ｒキー			D_DIK_R
#define KEY_INPUT_S									(0x1F)			// Ｓキー			D_DIK_S
#define KEY_INPUT_T									(0x14)			// Ｔキー			D_DIK_T
#define KEY_INPUT_U									(0x16)			// Ｕキー			D_DIK_U
#define KEY_INPUT_V									(0x2F)			// Ｖキー			D_DIK_V
#define KEY_INPUT_W									(0x11)			// Ｗキー			D_DIK_W
#define KEY_INPUT_X									(0x2D)			// Ｘキー			D_DIK_X
#define KEY_INPUT_Y									(0x15)			// Ｙキー			D_DIK_Y
#define KEY_INPUT_Z									(0x2C)			// Ｚキー			D_DIK_Z

#define KEY_INPUT_0 								(0x0B)			// ０キー			D_DIK_0
#define KEY_INPUT_1									(0x02)			// １キー			D_DIK_1
#define KEY_INPUT_2									(0x03)			// ２キー			D_DIK_2
#define KEY_INPUT_3									(0x04)			// ３キー			D_DIK_3
#define KEY_INPUT_4									(0x05)			// ４キー			D_DIK_4
#define KEY_INPUT_5									(0x06)			// ５キー			D_DIK_5
#define KEY_INPUT_6									(0x07)			// ６キー			D_DIK_6
#define KEY_INPUT_7									(0x08)			// ７キー			D_DIK_7
#define KEY_INPUT_8									(0x09)			// ８キー			D_DIK_8
#define KEY_INPUT_9									(0x0A)			// ９キー			D_DIK_9

// アスキーコントロールキーコード
#define CTRL_CODE_BS								(0x08)			// バックスペース
#define CTRL_CODE_TAB								(0x09)			// タブ
#define CTRL_CODE_CR								(0x0d)			// 改行
#define CTRL_CODE_DEL								(0x10)			// ＤＥＬキー

#define CTRL_CODE_COPY								(0x03)			// コピー
#define CTRL_CODE_PASTE								(0x16)			// ペースト
#define CTRL_CODE_CUT								(0x18)			// カット
#define CTRL_CODE_ALL								(0x01)			// 全て選択

#define CTRL_CODE_LEFT								(0x1d)			// ←キー
#define CTRL_CODE_RIGHT								(0x1c)			// →キー
#define CTRL_CODE_UP								(0x1e)			// ↑キー
#define CTRL_CODE_DOWN								(0x1f)			// ↓キー

#define CTRL_CODE_HOME								(0x1a)			// ＨＯＭＥボタン
#define CTRL_CODE_END								(0x19)			// ＥＮＤボタン
#define CTRL_CODE_PAGE_UP							(0x17)			// ＰＡＧＥ ＵＰ
#define CTRL_CODE_PAGE_DOWN							(0x15)			// ＰＡＧＥ ＤＯＷＮ

#define CTRL_CODE_ESC								(0x1b)			// ＥＳＣキー
#define CTRL_CODE_CMP								(0x20)			// 制御コード敷居値

// SetKeyInputStringColor2 に渡す色変更対象を指定するための識別子
#define DX_KEYINPSTRCOLOR_NORMAL_STR					(0)			// 入力文字列の色
#define DX_KEYINPSTRCOLOR_NORMAL_STR_EDGE				(1)			// 入力文字列の縁の色
#define DX_KEYINPSTRCOLOR_NORMAL_CURSOR					(2)			// ＩＭＥ非使用時のカーソルの色
#define DX_KEYINPSTRCOLOR_SELECT_STR					(3)			// 入力文字列の選択部分( SHIFTキーを押しながら左右キーで選択 )の色
#define DX_KEYINPSTRCOLOR_SELECT_STR_EDGE				(4)			// 入力文字列の選択部分( SHIFTキーを押しながら左右キーで選択 )の縁の色
#define DX_KEYINPSTRCOLOR_SELECT_STR_BACK				(5)			// 入力文字列の選択部分( SHIFTキーを押しながら左右キーで選択 )の周りの色
#define DX_KEYINPSTRCOLOR_IME_STR						(6)			// ＩＭＥ使用時の入力文字列の色
#define DX_KEYINPSTRCOLOR_IME_STR_EDGE					(7)			// ＩＭＥ使用時の入力文字列の縁の色
#define DX_KEYINPSTRCOLOR_IME_STR_BACK					(8)			// ＩＭＥ使用時の入力文字列の周りの色
#define DX_KEYINPSTRCOLOR_IME_CURSOR					(9)			// ＩＭＥ使用時のカーソルの色
#define DX_KEYINPSTRCOLOR_IME_LINE						(10)		// ＩＭＥ使用時の変換文字列の下線の色
#define DX_KEYINPSTRCOLOR_IME_SELECT_STR				(11)		// ＩＭＥ使用時の選択対象の変換候補文字列の色
#define DX_KEYINPSTRCOLOR_IME_SELECT_STR_EDGE			(12)		// ＩＭＥ使用時の選択対象の変換候補文字列の縁の色
#define DX_KEYINPSTRCOLOR_IME_SELECT_STR_BACK			(13)		// ＩＭＥ使用時の選択対象の変換候補文字列の周りの色
#define DX_KEYINPSTRCOLOR_IME_CONV_WIN_STR				(14)		// ＩＭＥ使用時の変換候補ウインドウ内の文字列の色
#define DX_KEYINPSTRCOLOR_IME_CONV_WIN_STR_EDGE			(15)		// ＩＭＥ使用時の変換候補ウインドウ内の文字列の縁の色
#define DX_KEYINPSTRCOLOR_IME_CONV_WIN_SELECT_STR		(16)		// ＩＭＥ使用時の変換候補ウインドウ内で選択している文字列の色
#define DX_KEYINPSTRCOLOR_IME_CONV_WIN_SELECT_STR_EDGE	(17)		// ＩＭＥ使用時の変換候補ウインドウ内で選択している文字列の縁の色
#define DX_KEYINPSTRCOLOR_IME_CONV_WIN_SELECT_STR_BACK	(18)		// ＩＭＥ使用時の変換候補ウインドウ内で選択している文字列の周りの色
#define DX_KEYINPSTRCOLOR_IME_CONV_WIN_EDGE				(19)		// ＩＭＥ使用時の変換候補ウインドウの縁の色
#define DX_KEYINPSTRCOLOR_IME_CONV_WIN_BACK				(20)		// ＩＭＥ使用時の変換候補ウインドウの下地の色
#define DX_KEYINPSTRCOLOR_IME_MODE_STR					(21)		// ＩＭＥ使用時の入力モード文字列の色(『全角ひらがな』等)
#define DX_KEYINPSTRCOLOR_IME_MODE_STR_EDGE				(22)		// ＩＭＥ使用時の入力モード文字列の縁の色
#define DX_KEYINPSTRCOLOR_NUM							(23)

// 文字列入力処理の入力文字数が限界に達している状態で、文字列の末端部分で入力が行われた場合の処理モード
#define DX_KEYINPSTR_ENDCHARAMODE_OVERWRITE				(0)			// 文字数が限界に達している状態で文字列の末端で文字が入力された場合は、最後の文字を上書き( デフォルト )
#define DX_KEYINPSTR_ENDCHARAMODE_NOTCHANGE				(1)			// 文字数が限界に達している状態で文字列の末端で文字が入力された場合は、何も変化しない

// フルスクリーン解像度モード定義
#define DX_FSRESOLUTIONMODE_DESKTOP					(0)				// モニターの画面モードをデスクトップ画面と同じにしてＤＸライブラリ画面を拡大して表示するモード
#define DX_FSRESOLUTIONMODE_NATIVE					(1)				// モニターの解像度をＤＸライブラリ画面の解像度に合わせるモード
#define DX_FSRESOLUTIONMODE_MAXIMUM					(2)				// モニターの解像度を最大にしてＤＸライブラリ画面を拡大して表示するモード

// フルスクリーン拡大モード定義
#define DX_FSSCALINGMODE_BILINEAR					(0)				// バイリニアモード( ピクセルが滲んでピクセルとピクセルの区切りがはっきりしない )
#define DX_FSSCALINGMODE_NEAREST					(1)				// 最近点モード( ピクセルが四角くくっきり表示される )

// SetGraphMode 戻り値定義
#define DX_CHANGESCREEN_OK							(0)				// 画面変更は成功した
#define DX_CHANGESCREEN_RETURN						(-1)			// 画面の変更は失敗し、元の画面モードに戻された
#define DX_CHANGESCREEN_DEFAULT						(-2)			// 画面の変更は失敗しデフォルトの画面モードに変更された
#define DX_CHANGESCREEN_REFRESHNORMAL				(-3)			// 画面の変更は成功したが、リフレッシュレートの変更は失敗した

// ストリームデータ読み込み処理コード簡略化関連
#define STTELL( st )								((st)->ReadShred.Tell( (st)->DataPoint ))
#define STSEEK( st, pos, type )						((st)->ReadShred.Seek( (st)->DataPoint, (pos), (type) ))
#define STREAD( buf, length, num, st )				((st)->ReadShred.Read( (buf), (length), (num), (st)->DataPoint ))
#define STWRITE( buf, length, num, st )				((st)->ReadShred.Write( (buf), (length), (num), (st)->DataPoint ))
#define STEOF( st )									((st)->ReadShred.Eof( (st)->DataPoint ))
#define STCLOSE( st )								((st)->ReadShred.Close( (st)->DataPoint ))

// ストリームデータ制御のシークタイプ定義
#define STREAM_SEEKTYPE_SET							(SEEK_SET)
#define STREAM_SEEKTYPE_END							(SEEK_END)
#define STREAM_SEEKTYPE_CUR							(SEEK_CUR)

// グラフィックロード時のイメージタイプ
#define LOADIMAGE_TYPE_FILE							(0)				// イメージはファイルである
#define LOADIMAGE_TYPE_MEM							(1)				// イメージはメモリである
#define LOADIMAGE_TYPE_NONE							(-1)			// イメージは無い



#ifndef DX_NON_NETWORK

// HTTP エラー
#define HTTP_ERR_SERVER								(0)				// サーバーエラー
#define HTTP_ERR_NOTFOUND							(1)				// ファイルが見つからなかった
#define HTTP_ERR_MEMORY								(2)				// メモリ確保の失敗
#define HTTP_ERR_LOST								(3)				// 途中で切断された
#define HTTP_ERR_NONE								(-1)			// エラーは報告されていない

// HTTP 処理の結果
#define HTTP_RES_COMPLETE							(0)				// 処理完了
#define HTTP_RES_STOP								(1)				// 処理中止
#define HTTP_RES_ERROR								(2)				// エラー終了
#define HTTP_RES_NOW								(-1)			// 現在進行中

#endif // DX_NON_NETWORK

#define DX_DEFINE_END

// データ型定義-------------------------------------------------------------------

// WAVEFORMATEX の定義
#ifndef _WAVEFORMATEX_
#define _WAVEFORMATEX_

struct tWAVEFORMATEX
{
	WORD					wFormatTag ;					// フォーマット( WAVE_FORMAT_PCM( 値は 1 ) 等 )
	WORD					nChannels ;						// チャンネル数
	DWORD					nSamplesPerSec ;				// １秒辺りのサンプル数
	DWORD					nAvgBytesPerSec ;				// １秒辺りのバイト数( PCMの場合 nSamplesPerSec * nBlockAlign )
	WORD					nBlockAlign ;					// 全チャンネルの１サンプルを合わせたバイト数( wBitsPerSample / 8 * nChannels )
	WORD					wBitsPerSample ;				// １サンプル辺りのビット数
	WORD					cbSize ;						// 拡張情報のバイト数( 拡張情報が無い場合は 0 )
} ;

typedef tWAVEFORMATEX	WAVEFORMATEX, *PWAVEFORMATEX, NEAR *NPWAVEFORMATEX, FAR *LPWAVEFORMATEX ;

#endif // _WAVEFORMATEX_

// WAVEFORMAT の定義
#ifndef WAVE_FORMAT_PCM

struct waveformat_tag
{
	WORD					wFormatTag ;					// フォーマット( WAVE_FORMAT_PCM( 値は 1 ) 等 )
	WORD					nChannels ;						// チャンネル数
	DWORD					nSamplesPerSec ;				// １秒辺りのサンプル数
	DWORD					nAvgBytesPerSec ;				// １秒辺りのバイト数( PCMの場合 nSamplesPerSec * nBlockAlign )
	WORD					nBlockAlign ;					// 全チャンネルの１サンプルを合わせたバイト数( wBitsPerSample / 8 * nChannels )
} ;

typedef waveformat_tag	WAVEFORMAT, *PWAVEFORMAT, NEAR *NPWAVEFORMAT, FAR *LPWAVEFORMAT ;

#define WAVE_FORMAT_PCM		1

#endif // WAVE_FORMAT_PCM

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

#define DX_STRUCT_START

// ＩＭＥ入力文字列の描画に必要な情報の内の文節情報
typedef struct tagIMEINPUTCLAUSEDATA
{
	int						Position ;						// 何文字目から
	int						Length ;						// 何文字か
} IMEINPUTCLAUSEDATA, *LPIMEINPUTCLAUSEDATA ;

// ＩＭＥ入力文字列の描画に必要な情報
typedef struct tagIMEINPUTDATA
{
	const TCHAR *				InputString ;				// 入力中の文字列

	int							CursorPosition ;			// カーソルの入力文字列中の位置(バイト単位)

	const IMEINPUTCLAUSEDATA *	ClauseData ;				// 文節情報
	int							ClauseNum ;					// 文節情報の数
	int							SelectClause ;				// 選択中の分節( -1 の場合はどの文節にも属していない( 末尾にカーソルがある ) )

	int							CandidateNum ;				// 変換候補の数( 0の場合は変換中ではない )
	const TCHAR **				CandidateList ;				// 変換候補文字列リスト( 例：ｎ番目の候補を描画する場合  DrawString( 0, 0, data.CandidateList[ n ], GetColor(255,255,255) ); )
	int							SelectCandidate ;			// 選択中の変換候補

	int							ConvertFlag ;				// 文字変換中かどうか( TRUE:変換中  FALSE:変換中ではない( 文字単位でカーソルが移動できる状態 ) )
} IMEINPUTDATA, *LPIMEINPUTDATA ;

// 描画文字列一文字の情報
typedef struct tagDRAWCHARINFO
{
	TCHAR					Char[ 13 ] ;					// 文字
	BYTE					Bytes ;							// 文字のバイト数
	float					DrawX, DrawY ;					// 描画位置
	float					SizeX, SizeY ;					// 描画サイズ
} DRAWCHARINFO, *LPDRAWCHARINFO ;

// 画面モード情報データ型
typedef struct tagDISPLAYMODEDATA
{
	int						Width ;							// 水平解像度
	int						Height ;						// 垂直解像度
	int						ColorBitDepth ;					// 色ビット深度
	int						RefreshRate ;					// リフレッシュレート( -1 の場合は規定値 )
} DISPLAYMODEDATA, *LPDISPLAYMODEDATA ;

// タイムデータ型
typedef struct tagDATEDATA
{
	int						Year ;							// 年
	int						Mon ;							// 月
	int						Day ;							// 日
	int						Hour ;							// 時間
	int						Min ;							// 分
	int						Sec ;							// 秒
} DATEDATA, *LPDATEDATA ;

// ファイル情報構造体
typedef struct tagFILEINFO
{
	TCHAR					Name[ 260 ] ;					// オブジェクト名
	int						DirFlag ;						// ディレクトリかどうか( TRUE:ディレクトリ  FALSE:ファイル )
	LONGLONG				Size ;							// サイズ
	DATEDATA				CreationTime ;					// 作成日時
	DATEDATA				LastWriteTime ;					// 最終更新日時
} FILEINFO, *LPFILEINFO ;

// ファイル情報構造体（ wchar_t 版 ）
typedef struct tagFILEINFOW
{
	wchar_t					Name[ 260 ] ;					// オブジェクト名
	int						DirFlag ;						// ディレクトリかどうか( TRUE:ディレクトリ  FALSE:ファイル )
	LONGLONG				Size ;							// サイズ
	DATEDATA				CreationTime ;					// 作成日時
	DATEDATA				LastWriteTime ;					// 最終更新日時
} FILEINFOW, *LPFILEINFOW ;

// 行列構造体
typedef struct tagMATRIX
{
	float					m[4][4] ;
} MATRIX, *LPMATRIX ;

typedef struct tagMATRIX_D
{
	double					m[4][4] ;
} MATRIX_D, *LPMATRIX_D ;

// ベクトルデータ型
typedef struct tagVECTOR
{
	float					x, y, z ;
} VECTOR, *LPVECTOR, FLOAT3, *LPFLOAT3 ;

typedef struct tagVECTOR_D
{
	double					x, y, z ;
} VECTOR_D, *LPVECTOR_D, DOUBLE3, *LPDOUBLE3 ;

// FLOAT2個データ型
typedef struct tagFLOAT2
{
	float					u, v ;
} FLOAT2 ;

// float 型のカラー値
typedef struct tagCOLOR_F
{
	float					r, g, b, a ;
} COLOR_F, *LPCOLOR_F ;

// unsigned char 型のカラー値
typedef struct tagCOLOR_U8
{
	BYTE					b, g, r, a ;
} COLOR_U8 ;

// FLOAT4個データ型
typedef struct tagFLOAT4
{
	float					x, y, z, w ;
} FLOAT4, *LPFLOAT4 ;

// DOUBLE4個データ型
typedef struct tagDOUBLE4
{
	double					x, y, z, w ;
} DOUBLE4, *LPDOUBLE4 ;

// INT4個データ型
typedef struct tagINT4
{
	int						x, y, z, w ;
} INT4 ;

#ifndef DX_NOTUSE_DRAWFUNCTION

// ２Ｄ描画に使用する頂点データ型(DrawPrimitive2D用)
typedef struct tagVERTEX2D
{
	VECTOR					pos ;
	float					rhw ;
	COLOR_U8				dif ;
	float					u, v ;
} VERTEX2D, *LPVERTEX2D ; 

// ２Ｄ描画に使用する頂点データ型(DrawPrimitive2DToShader用)
typedef struct tagVERTEX2DSHADER
{
	VECTOR					pos ;
	float					rhw ;
	COLOR_U8				dif ;
	COLOR_U8				spc ;
	float					u, v ;
	float					su, sv ;
} VERTEX2DSHADER, *LPVERTEX2DSHADER ; 

// ２Ｄ描画に使用する頂点データ型(公開用)
typedef struct tagVERTEX
{
	float					x, y ;
	float					u, v ;
	unsigned char			b, g, r, a ;
} VERTEX ;

// ３Ｄ描画に使用する頂点データ型( 旧バージョンのもの )
typedef struct tagVERTEX_3D
{
	VECTOR					pos ;
	unsigned char			b, g, r, a ;
	float					u, v ;
} VERTEX_3D, *LPVERTEX_3D ;

// ３Ｄ描画に使用する頂点データ型
typedef struct tagVERTEX3D
{
	VECTOR					pos ;							// 座標
	VECTOR					norm ;							// 法線
	COLOR_U8				dif ;							// ディフューズカラー
	COLOR_U8				spc ;							// スペキュラカラー
	float					u, v ;							// テクスチャ座標
	float					su, sv ;						// 補助テクスチャ座標
} VERTEX3D, *LPVERTEX3D ;

// ３Ｄ描画に使用する頂点データ型( DrawPrimitive3DToShader用 )
// 注意…メンバ変数に追加があるかもしれませんので、宣言時の初期化( VERTEX3DSHADER Vertex = { 0.0f, 0.0f, ... というようなもの )はしない方が良いです
typedef struct tagVERTEX3DSHADER
{
	VECTOR					pos ;							// 座標
	FLOAT4					spos ;							// 補助座標
	VECTOR					norm ;							// 法線
	VECTOR					tan ;							// 接線
	VECTOR					binorm ;						// 従法線
	COLOR_U8				dif ;							// ディフューズカラー
	COLOR_U8				spc ;							// スペキュラカラー
	float					u, v ;							// テクスチャ座標
	float					su, sv ;						// 補助テクスチャ座標
} VERTEX3DSHADER, *LPVERTEX3DSHADER ;

// ライトパラメータ
typedef struct tagLIGHTPARAM
{
	int						LightType ;						// ライトのタイプ( DX_LIGHTTYPE_D3DLIGHT_POINT 等 )
	COLOR_F					Diffuse ;						// ディフューズカラー
	COLOR_F					Specular ;						// スペキュラカラー
	COLOR_F					Ambient ;						// アンビエント色
	VECTOR					Position ;						// 位置
	VECTOR					Direction ;						// 方向
	float					Range ;							// 有効距離
	float					Falloff ;						// フォールオフ 1.0f にしておくのが好ましい
	float					Attenuation0 ;					// 距離による減衰係数０
	float					Attenuation1 ;					// 距離による減衰係数１
	float					Attenuation2 ;					// 距離による減衰係数２
	float					Theta ;							// スポットライトの内部コーンの照明角度( ラジアン )
	float					Phi ;							// スポットライトの外部コーンの照明角度
} LIGHTPARAM ;

// マテリアルパラメータ
typedef struct tagMATERIALPARAM
{
	COLOR_F					Diffuse ;						// ディフューズカラー
	COLOR_F					Ambient ;						// アンビエントカラー
	COLOR_F					Specular ;						// スペキュラカラー
	COLOR_F					Emissive ;						// エミッシブカラー
	float					Power ;							// スペキュラハイライトの鮮明度
} MATERIALPARAM ;

#endif // DX_NOTUSE_DRAWFUNCTION

// ラインヒットチェック結果格納用構造体
typedef struct tagHITRESULT_LINE
{
	int						HitFlag ;						// 当たったかどうか( 1:当たった  0:当たらなかった )
	VECTOR					Position ;						// 当たった座標
} HITRESULT_LINE ;

// ラインヒットチェック結果格納用構造体
typedef struct tagHITRESULT_LINE_D
{
	int						HitFlag ;						// 当たったかどうか( 1:当たった  0:当たらなかった )
	VECTOR_D				Position ;						// 当たった座標
} HITRESULT_LINE_D ;

// 関数 Segment_Segment_Analyse の結果を受け取る為の構造体
typedef struct tagSEGMENT_SEGMENT_RESULT
{
	float					SegA_SegB_MinDist_Square ;		// 線分Ａと線分Ｂが最も接近する座標間の距離の二乗

	float					SegA_MinDist_Pos1_Pos2_t ;		// 線分Ａと線分Ｂに最も接近する座標の線分Ａの t ( 0.0f 〜 1.0f 、最近点座標 = ( SegAPos2 - SegAPos1 ) * t + SegAPos1 )
	float					SegB_MinDist_Pos1_Pos2_t ;		// 線分Ｂが線分Ａに最も接近する座標の線分Ｂの t ( 0.0f 〜 1.0f 、最近点座標 = ( SegBPos2 - SegBPos1 ) * t + SegBPos1 )

	VECTOR					SegA_MinDist_Pos ;				// 線分Ａが線分Ｂに最も接近する線分Ａ上の座標
	VECTOR					SegB_MinDist_Pos ;				// 線分Ｂが線分Ａに最も接近する線分Ｂ上の座標
} SEGMENT_SEGMENT_RESULT ;

// 関数 Segment_Segment_Analyse の結果を受け取る為の構造体
typedef struct tagSEGMENT_SEGMENT_RESULT_D
{
	double					SegA_SegB_MinDist_Square ;		// 線分Ａと線分Ｂが最も接近する座標間の距離の二乗

	double					SegA_MinDist_Pos1_Pos2_t ;		// 線分Ａと線分Ｂに最も接近する座標の線分Ａの t ( 0.0 〜 1.0 、最近点座標 = ( SegAPos2 - SegAPos1 ) * t + SegAPos1 )
	double					SegB_MinDist_Pos1_Pos2_t ;		// 線分Ｂが線分Ａに最も接近する座標の線分Ｂの t ( 0.0 〜 1.0 、最近点座標 = ( SegBPos2 - SegBPos1 ) * t + SegBPos1 )

	VECTOR_D				SegA_MinDist_Pos ;				// 線分Ａが線分Ｂに最も接近する線分Ａ上の座標
	VECTOR_D				SegB_MinDist_Pos ;				// 線分Ｂが線分Ａに最も接近する線分Ｂ上の座標
} SEGMENT_SEGMENT_RESULT_D ;

// 関数 Segment_Point_Analyse の結果を受け取る為の構造体
typedef struct tagSEGMENT_POINT_RESULT
{
	float					Seg_Point_MinDist_Square ;		// 線分と点が最も接近する座標間の距離の二乗
	float					Seg_MinDist_Pos1_Pos2_t ;		// 線分が点に最も接近する座標の線分の t ( 0.0f 〜 1.0f 、最近点座標 = ( SegPos2 - SegPos1 ) * t + SegPos1 )
	VECTOR					Seg_MinDist_Pos ;				// 線分が点に最も接近する線分上の座標
} SEGMENT_POINT_RESULT ;

// 関数 Segment_Point_Analyse の結果を受け取る為の構造体
typedef struct tagSEGMENT_POINT_RESULT_D
{
	double					Seg_Point_MinDist_Square ;		// 線分と点が最も接近する座標間の距離の二乗
	double					Seg_MinDist_Pos1_Pos2_t ;		// 線分が点に最も接近する座標の線分の t ( 0.0 〜 1.0 、最近点座標 = ( SegPos2 - SegPos1 ) * t + SegPos1 )
	VECTOR_D				Seg_MinDist_Pos ;				// 線分が点に最も接近する線分上の座標
} SEGMENT_POINT_RESULT_D ;

// 関数 Segment_Triangle_Analyse の結果を受け取る為の構造体
typedef struct tagSEGMENT_TRIANGLE_RESULT
{
	float					Seg_Tri_MinDist_Square ;		// 線分と三角形が最も接近する座標間の距離の二乗

	float					Seg_MinDist_Pos1_Pos2_t ;		// 線分が三角形に最も接近する座標の線分の t ( 0.0f 〜 1.0f 、最近点座標 = ( SegPos2 - SegPos1 ) * t + SegPos1 )
	VECTOR					Seg_MinDist_Pos ;				// 線分が三角形に最も接近する線分上の座標

	float					Tri_MinDist_Pos1_w ;			// 三角形が線分に最も接近する座標の三角形座標１の重み( 最近点座標 = TriPos1 * TriPos1_w + TriPos2 * TriPos2_w + TriPos3 * TriPos3_w )
	float					Tri_MinDist_Pos2_w ;			// 三角形が線分に最も接近する座標の三角形座標２の重み
	float					Tri_MinDist_Pos3_w ;			// 三角形が線分に最も接近する座標の三角形座標３の重み
	VECTOR					Tri_MinDist_Pos ;				// 三角形が線分に最も接近する三角形上の座標
} SEGMENT_TRIANGLE_RESULT ;

// 関数 Segment_Triangle_Analyse の結果を受け取る為の構造体
typedef struct tagSEGMENT_TRIANGLE_RESULT_D
{
	double					Seg_Tri_MinDist_Square ;		// 線分と三角形が最も接近する座標間の距離の二乗

	double					Seg_MinDist_Pos1_Pos2_t ;		// 線分が三角形に最も接近する座標の線分の t ( 0.0 〜 1.0 、最近点座標 = ( SegPos2 - SegPos1 ) * t + SegPos1 )
	VECTOR_D				Seg_MinDist_Pos ;				// 線分が三角形に最も接近する線分上の座標

	double					Tri_MinDist_Pos1_w ;			// 三角形が線分に最も接近する座標の三角形座標１の重み( 最近点座標 = TriPos1 * TriPos1_w + TriPos2 * TriPos2_w + TriPos3 * TriPos3_w )
	double					Tri_MinDist_Pos2_w ;			// 三角形が線分に最も接近する座標の三角形座標２の重み
	double					Tri_MinDist_Pos3_w ;			// 三角形が線分に最も接近する座標の三角形座標３の重み
	VECTOR_D				Tri_MinDist_Pos ;				// 三角形が線分に最も接近する三角形上の座標
} SEGMENT_TRIANGLE_RESULT_D ;

// 関数 Triangle_Point_Analyse の結果を受け取る為の構造体
typedef struct tagTRIANGLE_POINT_RESULT
{
	float					Tri_Pnt_MinDist_Square ;		// 三角形と点が最も接近する座標間の距離の二乗

	float					Tri_MinDist_Pos1_w ;			// 三角形が点に最も接近する座標の三角形座標１の重み( 最近点座標 = TriPos1 * TriPos1_w + TriPos2 * TriPos2_w + TriPos3 * TriPos3_w )
	float					Tri_MinDist_Pos2_w ;			// 三角形が点に最も接近する座標の三角形座標２の重み
	float					Tri_MinDist_Pos3_w ;			// 三角形が点に最も接近する座標の三角形座標３の重み
	VECTOR					Tri_MinDist_Pos ;				// 三角形が点に最も接近する三角形上の座標
} TRIANGLE_POINT_RESULT ;

// 関数 Triangle_Point_Analyse の結果を受け取る為の構造体
typedef struct tagTRIANGLE_POINT_RESULT_D
{
	double					Tri_Pnt_MinDist_Square ;		// 三角形と点が最も接近する座標間の距離の二乗

	double					Tri_MinDist_Pos1_w ;			// 三角形が点に最も接近する座標の三角形座標１の重み( 最近点座標 = TriPos1 * TriPos1_w + TriPos2 * TriPos2_w + TriPos3 * TriPos3_w )
	double					Tri_MinDist_Pos2_w ;			// 三角形が点に最も接近する座標の三角形座標２の重み
	double					Tri_MinDist_Pos3_w ;			// 三角形が点に最も接近する座標の三角形座標３の重み
	VECTOR_D				Tri_MinDist_Pos ;				// 三角形が点に最も接近する三角形上の座標
} TRIANGLE_POINT_RESULT_D ;

// 関数 Plane_Point_Analyse の結果を受け取る為の構造体
typedef struct tagPLANE_POINT_RESULT
{
	int						Pnt_Plane_Normal_Side ;			// 点が平面の法線の側にあるかどうか( 1:法線の側にある  0:法線と反対側にある )
	float					Plane_Pnt_MinDist_Square ;		// 平面と点の距離
	VECTOR					Plane_MinDist_Pos ;				// 平面上の点との最近点座標
} PLANE_POINT_RESULT ;

// 関数 Plane_Point_Analyse の結果を受け取る為の構造体
typedef struct tagPLANE_POINT_RESULT_D
{
	int						Pnt_Plane_Normal_Side ;			// 点が平面の法線の側にあるかどうか( 1:法線の側にある  0:法線と反対側にある )
	double					Plane_Pnt_MinDist_Square ;		// 平面と点の距離
	VECTOR_D				Plane_MinDist_Pos ;				// 平面上の点との最近点座標
} PLANE_POINT_RESULT_D ;

#ifndef DX_NOTUSE_DRAWFUNCTION

// コリジョン結果代入用ポリゴン
typedef struct tagMV1_COLL_RESULT_POLY
{
	int						HitFlag ;						// ( MV1CollCheck_Line でのみ有効 )ヒットフラグ( 1:ヒットした  0:ヒットしなかった )
	VECTOR					HitPosition ;					// ( MV1CollCheck_Line でのみ有効 )ヒット座標

	int						FrameIndex ;					// 当たったポリゴンが含まれるフレームの番号
	int						PolygonIndex ;					// 当たったポリゴンの番号
	int						MaterialIndex ;					// 当たったポリゴンが使用しているマテリアルの番号
	VECTOR					Position[ 3 ] ;					// 当たったポリゴンを形成する三点の座標
	VECTOR					Normal ;						// 当たったポリゴンの法線
	float					PositionWeight[ 3 ] ;			// 当たった座標は、当たったポリゴンの三点それぞれどの割合で影響しているか、の値( 当たった座標 = Position[ 0 ] * PositionWeight[ 0 ] + Position[ 1 ] * PositionWeight[ 1 ] + Position[ 2 ] * PositionWeight[ 2 ] )
	int						PosMaxWeightFrameIndex[ 3 ] ;	// 当たったポリゴンの座標がそれぞれ最も影響を受けているフレームの番号
} MV1_COLL_RESULT_POLY ;

// コリジョン結果代入用ポリゴン配列
typedef struct tagMV1_COLL_RESULT_POLY_DIM
{
	int						HitNum ;						// ヒットしたポリゴンの数
	MV1_COLL_RESULT_POLY *	Dim ;							// ヒットしたポリゴンの配列( HitNum個分存在する )
} MV1_COLL_RESULT_POLY_DIM ;

// 参照用頂点構造体
typedef struct tagMV1_REF_VERTEX
{
	VECTOR					Position ;						// 位置
	VECTOR					Normal ;						// 法線
	FLOAT2					TexCoord[ 2 ] ;					// テクスチャ座標
	COLOR_U8				DiffuseColor ;					// ディフューズカラー
	COLOR_U8				SpecularColor ;					// スペキュラカラー
	int						MaxWeightFrameIndex ;			// 最も大きな影響を与えているフレーム( スキニングメッシュ用 )
} MV1_REF_VERTEX ;

// 参照用ポリゴン構造体
typedef struct tagMV1_REF_POLYGON
{
	unsigned short			FrameIndex ;					// このポリゴンが属しているフレーム
	unsigned short			MaterialIndex ;					// 使用しているマテリアル
	int						VIndexTarget ;					// VIndex が指すインデックスの参照先( 1:フレーム  0:モデル全体 )
	int						VIndex[ 3 ] ;					// ３角形ポリゴンを成す参照頂点のインデックス
	VECTOR					MinPosition ;					// ポリゴンを成す頂点座標の最小値
	VECTOR					MaxPosition ;					// ポリゴンを成す頂点座標の最大値
} MV1_REF_POLYGON ;

// 参照用ポリゴンデータ構造体
typedef struct tagMV1_REF_POLYGONLIST
{
	int						PolygonNum ;					// 参照用ポリゴンの数
	int						VertexNum ;						// 頂点の数
	VECTOR					MinPosition ;					// 頂点座標の最小値
	VECTOR					MaxPosition ;					// 頂点座標の最大値
	MV1_REF_POLYGON	*		Polygons ;						// 参照用ポリゴン配列
	MV1_REF_VERTEX *		Vertexs ;						// 参照用頂点配列
} MV1_REF_POLYGONLIST ;

#endif // DX_NOTUSE_DRAWFUNCTION




// ３Ｄサウンドリバーブエフェクトパラメータ構造体
// ( 注釈は MSDN の XAUDIO2FX_REVERB_PARAMETERS 構造体の解説をほぼ引用しています )
typedef struct tagSOUND3D_REVERB_PARAM
{
	float					WetDryMix ;						// リバーブとなる出力の割合( 指定可能範囲 0.0f 〜 100.0f )

	unsigned int			ReflectionsDelay ;				// ダイレクト パスに対する初期反射の遅延時間、単位はミリ秒( 指定可能範囲 0 〜 300 )
	BYTE					ReverbDelay ;					// 初期反射に対するリバーブの遅延時間、単位はミリ秒( 指定可能範囲 0 〜 85 )
	BYTE					RearDelay ;						// 左後方出力および右後方出力の遅延時間、単位はミリ秒( 指定可能範囲 0 〜 5 )

	BYTE					PositionLeft ;					// シミュレーション空間における視聴者に対する左入力の位置( 指定可能範囲 0 〜 30 )
															// PositionLeft を最小値に設定した場合、左入力は視聴者の近くに配置されます。
															// この位置では、サウンド フィールドにおいて初期反射が優勢になり、残響減衰は弱まって、振幅が小さくなります。
															// PositionLeft を最大値に設定した場合、左入力はシミュレーション室内で視聴者から最大限遠い位置に配置されます。
															// PositionLeft は残響減衰時間 (部屋の残響効果) に影響せず、視聴者に対する音源の見かけの位置のみに影響します。
	BYTE					PositionRight ;					// PositionLeft と同効果の右入力値( 指定可能範囲 0 〜 30 )、右入力にのみ影響を与える
	BYTE					PositionMatrixLeft ;			// 音源から視聴者までの距離によるインプレッションを増減させる値( 指定可能範囲 0 〜 30 )
	BYTE					PositionMatrixRight ;			// 音源から視聴者までの距離によるインプレッションを増減させま値( 指定可能範囲 0 〜 30 )
	BYTE					EarlyDiffusion ;				// 個々の壁の反射特性値( 指定可能範囲 0 〜 15 )、( 堅く平らな表面をシミュレートするには小さな値を設定し、散乱性の表面をシミュレートするには大きな値を設定します。)
	BYTE					LateDiffusion ;					// 個々の壁のリバーブ特性値( 指定可能範囲 0 〜 15 )、( 堅く平らな表面をシミュレートするには小さな値を設定し、散乱性の表面をシミュレートするには大きな値を設定します。)
	BYTE					LowEQGain ;						// 1 kHz における減衰時間を基準にして低周波数の減衰時間調整値( 指定可能範囲 0 〜 12 )
															// 値とゲイン (dB) の関係
															// 値          0  1  2  3  4  5  6  7  8  9 10 11 12
															// ゲイン(dB) -8 -7 -6 -5 -4 -3 -2 -1  0 +1 +2 +3 +4
															// LowEQGain の値が 8 の場合、低周波数の減衰時間と 1 kHz における減衰時間が等しくなることに注意してください
	BYTE					LowEQCutoff ;					// LowEQGain パラメーターにより制御されるローパス フィルターの折点周波数の設定値( 指定可能範囲 0 〜 9 )
															// 値と周波数 (Hz) の関係
															// 値          0   1   2   3   4   5   6   7   8   9
															// 周波数(Hz) 50 100 150 200 250 300 350 400 450 500
	BYTE					HighEQGain ;					// 1 kHz における減衰時間を基準にして高周波数の減衰時間調整値( 指定可能範囲 0 〜 8 )
															// 値とゲイン (dB) の関係
															// 値          0  1  2  3  4  5  6  7 8
															// ゲイン(dB) -8 -7 -6 -5 -4 -3 -2 -1 0
															// 0 に設定すると、高周波数の音が 1 kHz の場合と同じ割合で減衰します。最大値に設定すると、高周波数の音が 1 kHz の場合よりもはるかに高い割合で減衰します。
	BYTE					HighEQCutoff ;					// HighEQGain パラメーターにより制御されるハイパス フィルターの折点周波数設定値( 指定可能範囲 0 〜 14 )
															// 値と周波数 (kHz) の関係
															// 値          0    1    2     3    4     5    6     7    8     9   10    11   12    13   14
															// 周波数(kHz) 1  1.5    2   2.5    3   3.5    4   4.5    5   5.5    6   6.5    7   7.5    8

	float					RoomFilterFreq ;				// 室内エフェクトのローパス フィルターの折点周波数、単位は Hz ( 指定可能範囲 20.0f 〜 20000.0f )
	float					RoomFilterMain ;				// 初期反射と後期フィールド残響の両方に適用されるローパス フィルターのパス バンド強度レベル、単位は dB ( 指定可能範囲 -100.0f 〜 0.0f )
	float					RoomFilterHF ;					// 折点周波数 (RoomFilterFreq) での初期反射と後期フィールド残響の両方に適用されるローパス フィルターのパス バンド強度レベル、単位は dB ( 指定可能範囲 -100.0f 〜 0.0f )
	float					ReflectionsGain ;				// 初期反射の強度/レベルを調整値、単位は dB ( 指定可能範囲 -100.0f 〜 20.0f )
	float					ReverbGain ;					// リバーブの強度/レベルを調整値、単位は dB ( 指定可能範囲 -100.0f 〜 20.0f )
	float					DecayTime ;						// 1 kHz における残響減衰時間、単位は秒 ( 指定可能範囲 0.1f 〜 上限値特になし )、これは、フル スケールの入力信号が 60 dB 減衰するまでの時間です。
	float					Density ;						// 後期フィールド残響のモード密度を制御値、単位はパーセント( 指定可能範囲 0.0f 〜 100.0f )
															// 無色 (colorless) の空間では、Density を最大値 (100.0f ) に設定する必要があります。
															// Density を小さくすると、サウンドはくぐもった音 (くし形フィルターが適用された音) になります。
															// これはサイロをシミュレーションするときに有効なエフェクトです。
	float					RoomSize ;						// 音響空間の見かけ上のサイズ、単位はフィート( 指定可能範囲 1.0f (30.48 cm) 〜 100.0f (30.48 m) )
} SOUND3D_REVERB_PARAM ;





// ストリームデータ制御用関数ポインタ構造体タイプ２
typedef struct tagSTREAMDATASHREDTYPE2
{
	DWORD_PTR				(*Open		)( const TCHAR *Path, int UseCacheFlag, int BlockReadFlag, int UseASyncReadFlag ) ;
	int						(*Close		)( DWORD_PTR Handle ) ;
	LONGLONG				(*Tell		)( DWORD_PTR Handle ) ;
	int						(*Seek		)( DWORD_PTR Handle, LONGLONG SeekPoint, int SeekType ) ;
	size_t					(*Read		)( void *Buffer, size_t BlockSize, size_t DataNum, DWORD_PTR Handle ) ;
	int						(*Eof		)( DWORD_PTR Handle ) ;
	int						(*IdleCheck	)( DWORD_PTR Handle ) ;
	int						(*ChDir		)( const TCHAR *Path ) ;
	int						(*GetDir	)( TCHAR *Buffer ) ;
	int						(*GetDirS	)( TCHAR *Buffer, size_t BufferSize ) ;
	DWORD_PTR				(*FindFirst	)( const TCHAR *FilePath, FILEINFO *Buffer ) ;		// 戻り値: -1=エラー  -1以外=FindHandle
	int						(*FindNext	)( DWORD_PTR FindHandle, FILEINFO *Buffer ) ;		// 戻り値: -1=エラー  0=成功
	int						(*FindClose	)( DWORD_PTR FindHandle ) ;							// 戻り値: -1=エラー  0=成功
} STREAMDATASHREDTYPE2 ;

// ストリームデータ制御用関数ポインタ構造体タイプ２の wchar_t 使用版
typedef struct tagSTREAMDATASHREDTYPE2W
{
	DWORD_PTR				(*Open		)( const wchar_t *Path, int UseCacheFlag, int BlockReadFlag, int UseASyncReadFlag ) ;
	int						(*Close		)( DWORD_PTR Handle ) ;
	LONGLONG				(*Tell		)( DWORD_PTR Handle ) ;
	int						(*Seek		)( DWORD_PTR Handle, LONGLONG SeekPoint, int SeekType ) ;
	size_t					(*Read		)( void *Buffer, size_t BlockSize, size_t DataNum, DWORD_PTR Handle ) ;
	int						(*Eof		)( DWORD_PTR Handle ) ;
	int						(*IdleCheck	)( DWORD_PTR Handle ) ;
	int						(*ChDir		)( const wchar_t *Path ) ;
	int						(*GetDir	)( wchar_t *Buffer ) ;
	int						(*GetDirS	)( wchar_t *Buffer, size_t BufferSize ) ;
	DWORD_PTR				(*FindFirst	)( const wchar_t *FilePath, FILEINFOW *Buffer ) ;		// 戻り値: -1=エラー  -1以外=FindHandle
	int						(*FindNext	)( DWORD_PTR FindHandle, FILEINFOW *Buffer ) ;			// 戻り値: -1=エラー  0=成功
	int						(*FindClose	)( DWORD_PTR FindHandle ) ;								// 戻り値: -1=エラー  0=成功
} STREAMDATASHREDTYPE2W ;

// ストリームデータ制御用関数ポインタ構造体
typedef struct tagSTREAMDATASHRED
{
	LONGLONG				(*Tell		)( DWORD_PTR StreamDataPoint ) ;
	int						(*Seek		)( DWORD_PTR StreamDataPoint, LONGLONG SeekPoint, int SeekType ) ;
	size_t					(*Read		)( void *Buffer, size_t BlockSize, size_t DataNum, DWORD_PTR StreamDataPoint ) ;
//	size_t					(*Write		)( void *Buffer, size_t BlockSize, size_t DataNum, DWORD_PTR StreamDataPoint ) ;
	int						(*Eof		)( DWORD_PTR StreamDataPoint ) ;
	int						(*IdleCheck	)( DWORD_PTR StreamDataPoint ) ;
	int						(*Close		)( DWORD_PTR StreamDataPoint ) ;
} STREAMDATASHRED, *LPSTREAMDATASHRED ;

// ストリームデータ制御用データ構造体
typedef struct tagSTREAMDATA
{
	STREAMDATASHRED			ReadShred ;
	DWORD_PTR				DataPoint ;
} STREAMDATA ;





// パレット情報構造体
typedef struct tagCOLORPALETTEDATA
{
	unsigned char			Blue ;
	unsigned char			Green ;
	unsigned char			Red ;
	unsigned char			Alpha ;
} COLORPALETTEDATA ;

// カラー構造情報構造体
typedef struct tagCOLORDATA
{
	unsigned char			Format ;										// フォーマット( DX_BASEIMAGE_FORMAT_NORMAL 等 )

	unsigned char			ChannelNum ;									// チャンネル数
	unsigned char			ChannelBitDepth ;								// １チャンネル辺りのビット深度
	unsigned char			FloatTypeFlag ;									// 浮動小数点型かどうか( TRUE:浮動小数点型  FALSE:整数型 )
	unsigned char			PixelByte ;										// １ピクセルあたりのバイト数

	// 以下は ChannelNum 又は ChannelBitDepth が 0 の時のみ有効
	unsigned char			ColorBitDepth ;									// ビット深度
	unsigned char			NoneLoc, NoneWidth ;							// 使われていないビットのアドレスと幅
	unsigned char			RedWidth, GreenWidth, BlueWidth, AlphaWidth ;	// 各色のビット幅
	unsigned char			RedLoc	, GreenLoc  , BlueLoc  , AlphaLoc   ;	// 各色の配置されているビットアドレス
	unsigned int			RedMask , GreenMask , BlueMask , AlphaMask  ;	// 各色のビットマスク
	unsigned int			NoneMask ;										// 使われていないビットのマスク
	int						MaxPaletteNo ;									// 使用しているパレット番号の最大値( 0 の場合は 255 とみなす )

	// memo : ここより上にメンバー変数を追加したら DxBaseImage.cpp の NS_GraphColorMatchBltVer2 のアセンブラでのパレットデータ参照のリテラル値を修正する必要あり
	COLORPALETTEDATA		Palette[ 256 ] ;								// パレット( ColorBitDepth が８以下の場合のみ有効 )
} COLORDATA, *LPCOLORDATA ;

// 基本イメージデータ構造体
typedef struct tagBASEIMAGE
{
	COLORDATA				ColorData ;						// 色情報
	int						Width, Height, Pitch ;			// 幅、高さ、ピッチ
	void					*GraphData ;					// グラフィックイメージ
	int						MipMapCount ;					// ミップマップの数
	int						GraphDataCount ;				// グラフィックイメージの数
} BASEIMAGE, GRAPHIMAGE, *LPGRAPHIMAGE ;

// ラインデータ型
typedef struct tagLINEDATA
{
	int						x1, y1, x2, y2 ;				// 座標
	unsigned int			color ;							// 色
	int						pal ;							// パラメータ
} LINEDATA, *LPLINEDATA ;

// 座標データ型
typedef struct tagPOINTDATA
{
	int						x, y ;							// 座標
	unsigned int			color ;							// 色
	int						pal ;							// パラメータ
} POINTDATA, *LPPOINTDATA ;

#ifndef DX_NOTUSE_DRAWFUNCTION

// イメージフォーマットデータ
typedef struct tagIMAGEFORMATDESC
{
	unsigned char			TextureFlag ;					// テクスチャか、フラグ( TRUE:テクスチャ  FALSE:標準サーフェス )
	unsigned char			CubeMapTextureFlag ;			// キューブマップテクスチャか、フラグ( TRUE:キューブマップテクスチャ　FALSE:それ以外 )
	unsigned char			AlphaChFlag ;					// αチャンネルはあるか、フラグ	( TRUE:ある  FALSE:ない )
	unsigned char			DrawValidFlag ;					// 描画可能か、フラグ( TRUE:可能  FALSE:不可能 )
	unsigned char			SystemMemFlag ;					// システムメモリ上に存在しているか、フラグ( TRUE:システムメモリ上  FALSE:ＶＲＡＭ上 )( 標準サーフェスの時のみ有効 )
	unsigned char			UseManagedTextureFlag ;			// マネージドテクスチャを使用するか、フラグ
	unsigned char			UseLinearMapTextureFlag ;		// テクスチャのメモリデータ配置にリニアが選択できる場合はデータ配置方式をリニアにするかどうか( TRUE:リニアが可能な場合はリニアにする  FALSE:リニアが可能な場合も特にリニアを指定しない )
	unsigned char			PlatformTextureFormat ;			// 環境依存のテクスチャフォーマットを直接指定するために使用するための変数( DX_TEXTUREFORMAT_DIRECT3D9_R8G8B8 など )

	unsigned char			BaseFormat ;					// 基本フォーマット( DX_BASEIMAGE_FORMAT_NORMAL 等 )
	unsigned char			MipMapCount ;					// ミップマップの数
	unsigned char			AlphaTestFlag ;					// αテストチャンネルはあるか、フラグ( TRUE:ある  FALSE:ない )( テクスチャの場合のみ有効 )
	unsigned char			FloatTypeFlag ;					// 浮動小数点型かどうか
	unsigned char			ColorBitDepth ;					// 色深度( テクスチャの場合のみ有効 )
	unsigned char			ChannelNum ;					// チャンネルの数
	unsigned char			ChannelBitDepth ;				// １チャンネル辺りのビット深度( テクスチャの場合のみ有効、0 の場合は ColorBitDepth が使用される )
	unsigned char			BlendGraphFlag ;				// ブレンド用画像か、フラグ
	unsigned char			UsePaletteFlag ;				// パレットを使用しているか、フラグ( SystemMemFlag が TRUE の場合のみ有効 )

	unsigned char			MSSamples ;						// マルチサンプリング数( 描画対象の場合使用 )
	unsigned char			MSQuality ;						// マルチサンプリングクオリティ( 描画対象の場合使用 )
} IMAGEFORMATDESC ;

#endif // DX_NOTUSE_DRAWFUNCTION

// DirectInput のジョイパッド入力情報
typedef struct tagDINPUT_JOYSTATE
{
	int						X ;								// スティックのＸ軸パラメータ( -1000〜1000 )
	int						Y ;								// スティックのＹ軸パラメータ( -1000〜1000 )
	int						Z ;								// スティックのＺ軸パラメータ( -1000〜1000 )
	int						Rx ;							// スティックのＸ軸回転パラメータ( -1000〜1000 )
	int						Ry ;							// スティックのＹ軸回転パラメータ( -1000〜1000 )
	int						Rz ;							// スティックのＺ軸回転パラメータ( -1000〜1000 )
	int						Slider[ 2 ] ;					// スライダー二つ( 0〜65535 )
	unsigned int			POV[ 4 ] ;						// ハットスイッチ４つ( 0xffffffff:入力なし 0:上 4500:右上 9000:右 13500:右下 18000:下 22500:左下 27000:左 31500:左上 )
	unsigned char			Buttons[ 32 ] ;					// ボタン３２個( 押されたボタンは 128 になる )
} DINPUT_JOYSTATE ;

// XInput のジョイパッド入力情報
typedef struct tagXINPUT_STATE
{
	unsigned char			Buttons[ 16 ] ;					// ボタン１６個( 添字には XINPUT_BUTTON_DPAD_UP 等を使用する、0:押されていない  1:押されている )
	unsigned char			LeftTrigger ;					// 左トリガー( 0〜255 )
	unsigned char			RightTrigger ;					// 右トリガー( 0〜255 )
	short					ThumbLX ;						// 左スティックの横軸値( -32768 〜 32767 )
	short					ThumbLY ;						// 左スティックの縦軸値( -32768 〜 32767 )
	short					ThumbRX ;						// 右スティックの横軸値( -32768 〜 32767 )
	short					ThumbRY ;						// 右スティックの縦軸値( -32768 〜 32767 )
} XINPUT_STATE ;

// タッチパネルの１箇所分のタッチの情報
typedef struct tagTOUCHINPUTPOINT
{
	DWORD					Device ;						// タッチされたデバイス
	DWORD					ID ;							// タッチを判別するためのＩＤ
	int						PositionX ;						// タッチされた座標X
	int						PositionY ;						// タッチされた座標Y
} TOUCHINPUTPOINT ;

// タッチパネルのタッチの情報
typedef struct tagTOUCHINPUTDATA
{
	LONGLONG				Time ;							// 情報の時間

	int						PointNum ;						// 有効なタッチ情報の数
	TOUCHINPUTPOINT			Point[ TOUCHINPUTPOINT_MAX ] ;	// タッチ情報
} TOUCHINPUTDATA ;






// WinSockets使用時のアドレス指定用構造体
typedef struct tagIPDATA
{
	unsigned char			d1, d2, d3, d4 ;				// アドレス値
} IPDATA, *LPIPDATA ;

typedef struct tagIPDATA_IPv6
{
	union
	{
		unsigned char			Byte[ 16 ] ;
		unsigned short			Word[ 8 ] ;
	} ;
	unsigned long				ScopeID ;
} IPDATA_IPv6 ;

#define DX_STRUCT_END


#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

// 関数プロトタイプ宣言------------------------------------------------------------------

#ifdef __WINDOWS__
#include "DxFunctionWin.h"
#endif

#ifdef __ANDROID__
#include "DxFunctionAndroid.h"
#endif // __ANDROID__

#ifdef __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE
		#include "DxFunctioniOS.h"
	#endif
#endif // __APPLE__





#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

#define DX_FUNCTION_START

// DxSystem.cpp関数プロトタイプ宣言

// 初期化終了系関数
extern	int			DxLib_Init( void ) ;													// ライブラリ初期化を行う
extern	int			DxLib_End( void ) ;														// ライブラリ使用の後始末を行う

extern	int			DxLib_GlobalStructInitialize( void ) ;									// ライブラリの内部で使用している構造体をゼロ初期化して、DxLib_Init の前に行った設定を無効化する( DxLib_Init の前でのみ有効 )
extern	int			DxLib_IsInit( void ) ;													// ライブラリが初期化されているかどうかを取得する( 戻り値: TRUE=初期化されている  FALSE=されていない )

// エラー関係関数
extern	int			GetLastErrorCode( void ) ;												// 最後に発生したエラーのエラーコードを取得する( 戻り値　0:エラーが発生していない、又はエラーコード出力に対応したエラーが発生していない　　0以外：エラーコード、DX_ERRORCODE_WIN_DESKTOP_24BIT_COLOR など )
extern	int			GetLastErrorMessage( TCHAR *StringBuffer, int StringBufferBytes ) ;		// 最後に発生したエラーのエラーメッセージを指定の文字列バッファに取得する

// メッセージ処理関数
extern	int			ProcessMessage( void ) ;												// ウインドウズのメッセージループに代わる処理を行う

// 設定系関数
extern	int			SetAlwaysRunFlag( int Flag ) ;											// アプリが非アクティブ状態でも処理を実行するかどうかを設定する( TRUE:実行する  FALSE:停止する( デフォルト ) )

// ウエイト系関数
extern	int			WaitTimer( int WaitTime ) ;												// 指定の時間だけ処理をとめる
#ifndef DX_NON_INPUT
extern	int			WaitKey( void ) ;														// キーの入力待ちを行う
#endif // DX_NON_INPUT

// カウンタ及び時刻取得系関数
extern	int			GetNowCount(				int UseRDTSCFlag = FALSE ) ;				// ミリ秒単位の精度を持つカウンタの現在値を得る
extern	LONGLONG	GetNowHiPerformanceCount(	int UseRDTSCFlag = FALSE ) ;				// GetNowCountの高精度バージョン
extern	int			GetDateTime(				DATEDATA *DateBuf ) ;						// 現在時刻を取得する 

// 乱数取得
extern	int			GetRand( int RandMax ) ;												// 乱数を取得する( RandMax : 返って来る値の最大値 )
extern	int			SRand(	 int Seed ) ;													// 乱数の初期値を設定する

// バッテリー関連
extern	int			GetBatteryLifePercent( void ) ;											// 電池の残量を % で取得する( 戻り値： 100=フル充電状態  0=充電残量無し )

// クリップボード関係
extern	int			GetClipboardText(			TCHAR *DestBuffer ) ;						// クリップボードに格納されているテキストデータを読み出す( DestBuffer:文字列を格納するバッファの先頭アドレス   戻り値  -1:クリップボードにテキストデータが無い  -1以外:クリップボードに格納されている文字列データのサイズ( 単位:byte ) ) 
extern	int			SetClipboardText(			const TCHAR *Text                    ) ;	// クリップボードにテキストデータを格納する
extern	int			SetClipboardTextWithStrLen(	const TCHAR *Text, size_t TextLength ) ;	// クリップボードにテキストデータを格納する












// DxLog.cpp関数プロトタイプ宣言

#ifndef DX_NON_LOG
// ログファイル関数
extern	int			LogFileAdd(				const TCHAR *String ) ;							// ログファイル( Log.txt ) に文字列を出力する
extern	int			LogFileAddWithStrLen(	const TCHAR *String, size_t StringLength ) ;	// ログファイル( Log.txt ) に文字列を出力する
extern 	int			LogFileFmtAdd(			const TCHAR *FormatString , ... ) ;				// 書式付きで ログファイル( Log.txt ) に文字列を出力する( 書式は printf と同じ )
extern	int			LogFileTabAdd(			void ) ;										// ログファイル( Log.txt ) に出力する文字列の前に付けるタブの数を一つ増やす
extern	int			LogFileTabSub(			void ) ;										// ログファイル( Log.txt ) に出力する文字列の前に付けるタブの数を一つ減らす
extern	int			ErrorLogAdd(			const TCHAR *String ) ;							// LogFileAdd の旧名称関数
extern 	int			ErrorLogFmtAdd(			const TCHAR *FormatString , ... ) ;				// LogFileFmtAdd の旧名称関数
extern	int			ErrorLogTabAdd(			void ) ;										// LogFileTabAdd の旧名称関数
extern	int			ErrorLogTabSub(			void ) ;										// LogFileTabSub の旧名称関数
extern	int			SetUseTimeStampFlag(	int UseFlag ) ;									// ログファイル( Log.txt ) に出力する文字列の前に起動してからの時間を付けるかどうかを設定する( TRUE:付ける( デフォルト)  FALSE:付けない )
extern 	int			AppLogAdd(				const TCHAR *String , ... ) ;					// LogFileFmtAdd と同じ機能の関数

// ログ出力設定関数
extern	int			SetOutApplicationLogValidFlag(	          int Flag ) ;													// ログファイル( Log.txt ) を作成するかどうかを設定する( TRUE:作成する( デフォルト )  FALSE:作成しない )、DxLib_Init の前でのみ使用可能　
extern	int			SetApplicationLogFileName(                const TCHAR *FileName ) ;										// ログファイルの名前を設定する( Log.txt 以外にしたい場合に使用 )
extern	int			SetApplicationLogFileNameWithStrLen(      const TCHAR *FileName, size_t FileNameLength ) ;				// ログファイルの名前を設定する( Log.txt 以外にしたい場合に使用 )
extern	int			SetApplicationLogSaveDirectory(	          const TCHAR *DirectoryPath ) ;								// ログファイル( Log.txt ) を保存するディレクトリパスを設定する
extern	int			SetApplicationLogSaveDirectoryWithStrLen( const TCHAR *DirectoryPath, size_t DirectoryPathLength ) ;	// ログファイル( Log.txt ) を保存するディレクトリパスを設定する
extern	int			SetUseDateNameLogFile(			          int Flag ) ;													// ログファイル名に日付をつけるかどうかをセットする( TRUE:付ける  FALSE:付けない( デフォルト ) )

#ifndef DX_NON_PRINTF_DX

// ログ出力機能関数
extern	int			SetLogDrawOutFlag(	 int DrawFlag ) ;									// printfDx の結果を画面に出力するかどうかを設定する、TRUE:出力を行う  FALSE:出力を行わない( printfDx を実行すると内部で SetLogDrawOutFlag( TRUE ) ; が呼ばれます )
extern 	int			GetLogDrawFlag(		 void ) ;											// printfDx の結果を画面に出力するかどうかの設定を取得する( 戻り値  TRUE:出力を行う  FALSE:出力を行わない )
extern	int			SetLogFontSize(		 int Size ) ;										// printfDx の結果を画面に出力する際に使用するフォントのサイズを設定する
extern	int			SetLogFontHandle(	 int FontHandle ) ;									// printfDx の結果を画面に出力する際に使用するフォントのハンドルを変更する
extern	int			SetLogDrawArea(		 int x1, int y1, int x2, int y2 ) ;					// printfDx の結果を画面に出力する際の描画する領域を設定する

// 簡易画面出力関数
extern 	int			printfDx(			 const TCHAR *FormatString , ... ) ;								// printf と同じ引数で画面に文字列を表示するための関数
extern	int			putsDx(				 const TCHAR *String, int NewLine = TRUE ) ;						// puts と同じ引数で画面に文字列を表示するための関数
extern	int			putsDxWithStrLen(	 const TCHAR *String, size_t StringLength, int NewLine = TRUE ) ;	// puts と同じ引数で画面に文字列を表示するための関数
extern	int			clsDx(				 void ) ;															// printfDx の結果をリセットするための関数
#endif // DX_NON_PRINTF_DX

#endif // DX_NON_LOG












#ifndef DX_NON_ASYNCLOAD

// DxASyncLoad.cpp関数プロトタイプ宣言

// 非同期読み込み関係
extern	int			SetUseASyncLoadFlag(			int Flag ) ;								// 読み込み処理系の関数で非同期読み込みを行うかどうかを設定する( 非同期読み込みに対応している関数のみ有効 )( TRUE:非同期読み込みを行う  FALSE:非同期読み込みを行わない( デフォルト ) )
extern	int			GetUseASyncLoadFlag(			void ) ;									// 読み込み処理系の関数で非同期読み込みを行うかどうかを取得する( 非同期読み込みに対応している関数のみ有効 )( TRUE:非同期読み込みを行う  FALSE:非同期読み込みを行わない( デフォルト ) )
extern	int			CheckHandleASyncLoad(			int Handle ) ;								// ハンドルの非同期読み込みが完了しているかどうかを取得する( TRUE:まだ完了していない  FALSE:完了している  -1:エラー )
extern	int			GetHandleASyncLoadResult(		int Handle ) ;								// ハンドルの非同期読み込み処理の戻り値を取得する( 非同期読み込み中の場合は一つ前の非同期読み込み処理の戻り値が返ってきます )
extern	int			SetASyncLoadFinishDeleteFlag(	int Handle ) ;								// ハンドルの非同期読み込み処理が完了したらハンドルを削除するフラグを立てる
extern	int			GetASyncLoadNum(				void ) ;									// 非同期読み込み中の処理の数を取得する
extern	int			SetASyncLoadThreadNum(			int ThreadNum ) ;							// 非同期読み込み処理を行うスレッドの数を設定する( ThreadNum に指定できる数は 1 〜 32 )

#endif // DX_NON_ASYNCLOAD











// DxHandle.cpp関数プロトタイプ宣言

extern	int			SetDeleteHandleFlag(		int Handle, int *DeleteFlag ) ;				// ハンドルが削除されたときに−１が設定される変数を登録する















// マウス関係関数
extern	int			SetMouseDispFlag(		int DispFlag ) ;												// マウスポインタの表示状態を設定する( DispFlag:マウスポインタを表示するかどうか( TRUE:表示する  FALSE:表示しない )
#ifndef DX_NON_INPUT
extern	int			GetMousePoint(			int *XBuf, int *YBuf ) ;										// マウスポインタの位置を取得する
extern	int			SetMousePoint(			int PointX, int PointY ) ;										// マウスポインタの位置を設定する
extern	int			GetMouseInput(			void ) ;														// マウスのボタンの押下状態を取得する
extern	int			GetMouseWheelRotVol(	int CounterReset = TRUE ) ;										// 垂直マウスホイールの回転量を取得する
extern	int			GetMouseHWheelRotVol(	int CounterReset = TRUE ) ;										// 水平マウスホイールの回転量を取得する
extern	float		GetMouseWheelRotVolF(	int CounterReset = TRUE ) ;										// 垂直マウスホイールの回転量を取得する( 戻り値が float 型 )
extern	float		GetMouseHWheelRotVolF(	int CounterReset = TRUE ) ;										// 水平マウスホイールの回転量を取得する( 戻り値が float 型 )
extern	int			GetMouseInputLog(		int *Button, int *ClickX, int *ClickY, int LogDelete = TRUE ) ;					// マウスのボタンを押した情報を一つ取得する( Button:押されたボタン( MOUSE_INPUT_LEFT 等 )を格納する変数のアドレス  ClickX:押された時のＸ座標を格納する変数のアドレス  ClickY:押された時のＹ座標を格納する変数のアドレス   LogDelete:取得した押下情報一つ分をログから削除するかどうか( TRUE:削除する  FALSE:削除しない、つまり次にこの関数が呼ばれたときに同じ値を取得することになる )　　戻り値  0:押された情報取得できた　-1:押された情報が無かった、つまり前回の呼び出し( または起動時から最初の呼び出し )の間に一度もマウスのボタンが押されなかった )
extern	int			GetMouseInputLog2(		int *Button, int *ClickX, int *ClickY, int *LogType, int LogDelete = TRUE ) ;	// マウスのボタンを押したり離したりした情報を一つ取得する( Button:押されたり離されたりしたボタン( MOUSE_INPUT_LEFT 等 )を格納する変数のアドレス  ClickX:押されたり離されたりした時のＸ座標を格納する変数のアドレス  ClickY:押されたり離されたりした時のＹ座標を格納する変数のアドレス   LogDelete:取得した押されたり離されたりした情報一つ分をログから削除するかどうか( TRUE:削除する  FALSE:削除しない、つまり次にこの関数が呼ばれたときに同じ値を取得することになる )　　戻り値  0:押されたり離されたりした情報取得できた　-1:押されたり離されたりした情報が無かった、つまり前回の呼び出し( または起動時から最初の呼び出し )の間に一度もマウスのボタンが押されたり離されたりしなかった )
#endif // DX_NON_INPUT












// タッチパネル入力関係関数
#ifndef DX_NON_INPUT
extern	int				GetTouchInputNum( void ) ;																// タッチされている数を取得する
extern	int				GetTouchInput( int InputNo, int *PositionX, int *PositionY, int *ID = NULL , int *Device = NULL ) ;		// タッチの情報を取得する

extern	int				GetTouchInputLogNum( void ) ;															// ストックされているタッチ情報の数を取得する
extern	int				ClearTouchInputLog( void ) ;															// ストックされているタッチ情報をクリアする
extern	TOUCHINPUTDATA	GetTouchInputLogOne( int PeekFlag = FALSE ) ;											// ストックされているタッチ情報から一番古い情報をひとつ取得する
extern	int				GetTouchInputLog( TOUCHINPUTDATA *TouchData, int GetNum, int PeekFlag = FALSE ) ;		// ストックされているタッチ情報から古い順に指定数バッファに取得する( 戻り値  -1:エラー  0以上:取得した情報の数 )

extern	int				GetTouchInputDownLogNum( void ) ;														// ストックされているタッチされ始めた情報の数を取得する
extern	int				ClearTouchInputDownLog( void ) ;														// ストックされているタッチされ始めた情報をクリアする
extern	TOUCHINPUTPOINT	GetTouchInputDownLogOne( int PeekFlag = FALSE ) ;										// ストックされているタッチされ始めた情報から一番古い情報をひとつ取得する
extern	int				GetTouchInputDownLog( TOUCHINPUTPOINT *PointData, int GetNum, int PeekFlag = FALSE ) ;	// ストックされているタッチされ始めた情報から古い順に指定数バッファに取得する( 戻り値  -1:エラー  0以上:取得した情報の数 )

extern	int				GetTouchInputUpLogNum( void ) ;															// ストックされているタッチが離された情報の数を取得する
extern	int				ClearTouchInputUpLog( void ) ;															// ストックされているタッチが離された情報をクリアする
extern	TOUCHINPUTPOINT	GetTouchInputUpLogOne( int PeekFlag = FALSE ) ;											// ストックされているタッチが離された情報から一番古い情報をひとつ取得する
extern	int				GetTouchInputUpLog( TOUCHINPUTPOINT *PointData, int GetNum, int PeekFlag = FALSE ) ;	// ストックされているタッチが離された情報から古い順に指定数バッファに取得する( 戻り値  -1:エラー  0以上:取得した情報の数 )

#endif // DX_NON_INPUT















// DxMemory.cpp関数プロトタイプ宣言

// メモリ確保系関数
extern	void*		DxAlloc(						size_t AllocSize , const char *File = NULL , int Line = -1 ) ;					// 指定のサイズのメモリを確保する( AllocSize:確保するメモリのサイズ( 単位:byte )  File:DxAllocを呼んだソースファイル名( デバッグ用 )  Line:DxAllocを呼んだソースファイル中の行番号( デバッグ用 )　　戻り値  NULL:メモリの確保失敗   NULL以外:確保したメモリ領域の先頭アドレス )
extern	void*		DxCalloc(						size_t AllocSize , const char *File = NULL , int Line = -1 ) ;					// 指定のサイズのメモリを確保して、０で埋める、初期化後に０で埋める以外は DxAlloc と動作は同じ
extern	void*		DxRealloc(						void *Memory , size_t AllocSize , const char *File = NULL , int Line = -1 ) ;	// メモリの再確保を行う( Memory:再確保を行うメモリ領域の先頭アドレス( DxAlloc の戻り値 )  AllocSize:新しい確保サイズ　FileとLine の説明は DxAlloc の注釈の通り 　戻り値 NULL:メモリの再確保失敗　NULL以外:再確保した新しいメモリ領域の先頭アドレス　)
extern	void		DxFree(							void *Memory ) ;																// メモリを解放する( Memory:解放するメモリ領域の先頭アドレス( DxAlloc の戻り値 ) )
extern	size_t		DxSetAllocSizeTrap(				size_t Size ) ;																	// 列挙対象にするメモリの確保容量をセットする
extern	int			DxSetAllocPrintFlag(			int Flag ) ;																	// ＤＸライブラリ内でメモリ確保が行われる時に情報を出力するかどうかをセットする
extern	size_t		DxGetAllocSize(					void ) ;																		// DxAlloc や DxCalloc で確保しているメモリサイズを取得する
extern	int			DxGetAllocNum(					void ) ;																		// DxAlloc や DxCalloc で確保しているメモリの数を取得する
extern	void		DxDumpAlloc(					void ) ;																		// DxAlloc や DxCalloc で確保しているメモリを列挙する
extern	void		DxDrawAlloc(					int x, int y, int Width, int Height ) ;											// DxAlloc や DxCalloc で確保しているメモリの状況を描画する
extern	int			DxErrorCheckAlloc(				void ) ;																		// 確保したメモリ情報が破壊されていないか調べる( -1:破壊あり  0:なし )
extern	int			DxSetAllocSizeOutFlag(			int Flag ) ;																	// メモリが確保、解放が行われる度に確保しているメモリの容量を出力するかどうかのフラグをセットする
extern	int			DxSetAllocMemoryErrorCheckFlag(	int Flag ) ;																	// メモリの確保、解放が行われる度に確保しているメモリ確保情報が破損していないか調べるかどうかのフラグをセットする














// DxBaseFunc.cpp 関数プロトタイプ宣言

// 文字コード関係
extern	int				GetCharBytes( int CharCodeFormat /* DX_CHARCODEFORMAT_SHIFTJIS 等 */ , const void *String ) ;	// 文字列の先頭の文字のバイト数を取得する
extern	int				ConvertStringCharCodeFormat( int SrcCharCodeFormat /* DX_CHARCODEFORMAT_SHIFTJIS 等 */ , const void *SrcString, int DestCharCodeFormat /* DX_CHARCODEFORMAT_SHIFTJIS 等 */ , void *DestStringBuffer ) ;		// 文字列の文字コード形式を別の文字コード形式に変換する
extern	int				SetUseCharCodeFormat( int CharCodeFormat /* DX_CHARCODEFORMAT_SHIFTJIS 等 */ ) ;		// 文字列の引数の文字コード形式を設定する( 文字列描画系関数とその他一部関数を除く )( UNICODE版では無効 )
extern	int				Get_wchar_t_CharCodeFormat( void ) ;													// wchar_t型の文字コード形式を取得する( 戻り値： DX_CHARCODEFORMAT_UTF16LE など )

// 文字列関係
extern	void			strcpyDx(      TCHAR *Dest,                   const TCHAR *Src ) ;						// strcpy と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			strcpy_sDx(    TCHAR *Dest, size_t DestBytes, const TCHAR *Src ) ;						// strcpy_s と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			strpcpyDx(     TCHAR *Dest,                   const TCHAR *Src, int Pos ) ;				// 位置指定付き strcpy、Pos はコピー開始位置　( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			strpcpy_sDx(   TCHAR *Dest, size_t DestBytes, const TCHAR *Src, int Pos ) ;				// 位置指定付き strcpy_s、Pos はコピー開始位置　( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			strpcpy2Dx(    TCHAR *Dest,                   const TCHAR *Src, int Pos ) ;				// 位置指定付き strcpy、Pos はコピー開始位置( 全角文字も 1 扱い )　( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			strpcpy2_sDx(  TCHAR *Dest, size_t DestBytes, const TCHAR *Src, int Pos ) ;				// 位置指定付き strcpy_s、Pos はコピー開始位置( 全角文字も 1 扱い )　( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			strncpyDx(     TCHAR *Dest,                   const TCHAR *Src, int Num ) ;				// strncpy と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			strncpy_sDx(   TCHAR *Dest, size_t DestBytes, const TCHAR *Src, int Num ) ;				// strncpy_s と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			strncpy2Dx(    TCHAR *Dest,                   const TCHAR *Src, int Num ) ;				// strncpy の Num が文字数( 全角文字も 1 扱い )になったもの、終端に必ずヌル文字が代入される( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			strncpy2_sDx(  TCHAR *Dest, size_t DestBytes, const TCHAR *Src, int Num ) ;				// strncpy_s の Num が文字数( 全角文字も 1 扱い )になったもの、終端に必ずヌル文字が代入される( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			strrncpyDx(    TCHAR *Dest,                   const TCHAR *Src, int Num ) ;				// strncpy の文字列の終端からの文字数指定版( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			strrncpy_sDx(  TCHAR *Dest, size_t DestBytes, const TCHAR *Src, int Num ) ;				// strncpy_s の文字列の終端からの文字数指定版( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			strrncpy2Dx(   TCHAR *Dest,                   const TCHAR *Src, int Num ) ;				// strncpy の文字列の終端からの文字数( 全角文字も 1 扱い )指定版、終端に必ずヌル文字が代入される( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			strrncpy2_sDx( TCHAR *Dest, size_t DestBytes, const TCHAR *Src, int Num ) ;				// strncpy_s の文字列の終端からの文字数( 全角文字も 1 扱い )指定版、終端に必ずヌル文字が代入される( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			strpncpyDx(    TCHAR *Dest,                   const TCHAR *Src, int Pos, int Num ) ;	// strncpy のコピー開始位置指定版、Pos はコピー開始位置、Num は文字数( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			strpncpy_sDx(  TCHAR *Dest, size_t DestBytes, const TCHAR *Src, int Pos, int Num ) ;	// strncpy_s のコピー開始位置指定版、Pos はコピー開始位置、Num は文字数( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			strpncpy2Dx(   TCHAR *Dest,                   const TCHAR *Src, int Pos, int Num ) ;	// strncpy のコピー開始位置指定版、Pos はコピー開始位置( 全角文字も 1 扱い )、Num は文字数( 全角文字も 1 扱い )、終端に必ずヌル文字が代入される( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			strpncpy2_sDx( TCHAR *Dest, size_t DestBytes, const TCHAR *Src, int Pos, int Num ) ;	// strncpy_s のコピー開始位置指定版、Pos はコピー開始位置( 全角文字も 1 扱い )、Num は文字数( 全角文字も 1 扱い )、終端に必ずヌル文字が代入される( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			strcatDx(      TCHAR *Dest,                   const TCHAR *Src ) ;						// strcat と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			strcat_sDx(    TCHAR *Dest, size_t DestBytes, const TCHAR *Src ) ;						// strcat_s と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	size_t			strlenDx(      const TCHAR *Str ) ;												// strlen と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	size_t			strlen2Dx(     const TCHAR *Str ) ;												// strlen の戻り値が文字数( 全角文字も 1 扱い )になったもの( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				strcmpDx(      const TCHAR *Str1, const TCHAR *Str2 ) ;							// strcmp と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				stricmpDx(     const TCHAR *Str1, const TCHAR *Str2 ) ;							// stricmp と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				strncmpDx(     const TCHAR *Str1, const TCHAR *Str2, int Num ) ;				// strncmp と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				strncmp2Dx(    const TCHAR *Str1, const TCHAR *Str2, int Num ) ;				// strncmp の Num が文字数( 全角文字も 1 扱い )になったもの( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				strpncmpDx(    const TCHAR *Str1, const TCHAR *Str2, int Pos, int Num ) ;		// strncmp の比較開始位置指定版、Pos が Str1 の比較開始位置、Num が文字数( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				strpncmp2Dx(   const TCHAR *Str1, const TCHAR *Str2, int Pos, int Num ) ;		// strncmp の比較開始位置指定版、Pos が Str1 の比較開始位置( 全角文字も 1 扱い )、Num が文字数( 全角文字も 1 扱い )( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	DWORD			strgetchrDx(   const TCHAR *Str, int Pos, int *CharNums = NULL ) ;				// 文字列の指定の位置の文字コードを取得する、Pos は取得する位置、CharNums は文字数を代入する変数のアドレス、戻り値は文字コード( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	DWORD			strgetchr2Dx(  const TCHAR *Str, int Pos, int *CharNums = NULL ) ;				// 文字列の指定の位置の文字コードを取得する、Pos は取得する位置( 全角文字も 1 扱い )、CharNums は文字数を代入する変数のアドレス、戻り値は文字コード( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				strputchrDx(   TCHAR *Str, int Pos, DWORD CharCode ) ;							// 文字列の指定の位置に文字コードを書き込む、Pos は書き込む位置、CharCode は文字コード、戻り値は書き込んだ文字数( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				strputchr2Dx(  TCHAR *Str, int Pos, DWORD CharCode ) ;							// 文字列の指定の位置に文字コードを書き込む、Pos は書き込む位置( 全角文字も 1 扱い )、CharCode は文字コード、戻り値は書き込んだ文字数( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	const TCHAR *	strposDx(      const TCHAR *Str, int Pos ) ;									// 文字列の指定の位置のアドレスを取得する、Pos は取得する位置　( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	const TCHAR *	strpos2Dx(     const TCHAR *Str, int Pos ) ;									// 文字列の指定の位置のアドレスを取得する、Pos は取得する位置( 全角文字も 1 扱い )　( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	const TCHAR *	strstrDx(      const TCHAR *Str1, const TCHAR *Str2 ) ;							// strstr と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				strstr2Dx(     const TCHAR *Str1, const TCHAR *Str2 ) ;							// strstr の戻り値が文字列先頭からの文字数( 全角文字も 1 扱い ) になったもの( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	const TCHAR *	strrstrDx(     const TCHAR *Str1, const TCHAR *Str2 ) ;							// strrstr と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				strrstr2Dx(    const TCHAR *Str1, const TCHAR *Str2 ) ;							// strrstr の戻り値が文字列先頭からの文字数( 全角文字も 1 扱い ) になったもの( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	const TCHAR *	strchrDx(      const TCHAR *Str, DWORD CharCode ) ;								// strchr と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				strchr2Dx(     const TCHAR *Str, DWORD CharCode ) ;								// strchr の戻り値が文字列先頭からの文字数( 全角文字も 1 扱い ) になったもの( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	const TCHAR *	strrchrDx(     const TCHAR *Str, DWORD CharCode ) ;								// strrchr と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				strrchr2Dx(    const TCHAR *Str, DWORD CharCode ) ;								// strrchr の戻り値が文字列先頭からの文字数( 全角文字も 1 扱い ) になったもの( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	TCHAR *			struprDx(      TCHAR *Str ) ;													// strupr と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				vsprintfDx(    TCHAR *Buffer,                    const TCHAR *FormatString, va_list Arg ) ;	// vsprintf と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				vsnprintfDx(   TCHAR *Buffer, size_t BufferSize, const TCHAR *FormatString, va_list Arg ) ;	// vsnprintf と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				sprintfDx(     TCHAR *Buffer,                    const TCHAR *FormatString, ... ) ;			// sprintf と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				snprintfDx(    TCHAR *Buffer, size_t BufferSize, const TCHAR *FormatString, ... ) ;			// snprintf と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	TCHAR *			itoaDx(        int Value, TCHAR *Buffer,                     int Radix ) ;		// itoa と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	TCHAR *			itoa_sDx(      int Value, TCHAR *Buffer, size_t BufferBytes, int Radix ) ;		// itoa_s と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				atoiDx(        const TCHAR *Str ) ;												// atoi と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	double			atofDx(        const TCHAR *Str ) ;												// atof と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				vsscanfDx(     const TCHAR *String, const TCHAR *FormatString, va_list Arg ) ;	// vsscanf と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				sscanfDx(      const TCHAR *String, const TCHAR *FormatString, ... ) ;			// sscanf と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )


















// DxNetwork.cpp関数プロトタイプ宣言

#ifndef DX_NON_NETWORK

// 通信関係
extern	int			ProcessNetMessage(				int RunReleaseProcess = FALSE ) ;														// 通信メッセージの処理をする関数

extern	int			GetHostIPbyName(				const TCHAR *HostName,							IPDATA      *IPDataBuf, int IPDataBufLength = 1 , int *IPDataGetNum = NULL ) ;				// ＤＮＳサーバーを使ってホスト名からＩＰアドレスを取得する( IPv4版 )
extern	int			GetHostIPbyNameWithStrLen(		const TCHAR *HostName, size_t HostNameLength,	IPDATA      *IPDataBuf, int IPDataBufLength = 1 , int *IPDataGetNum = NULL ) ;				// ＤＮＳサーバーを使ってホスト名からＩＰアドレスを取得する( IPv4版 )
extern	int			GetHostIPbyName_IPv6(			const TCHAR *HostName,							IPDATA_IPv6 *IPDataBuf, int IPDataBufLength = 1 , int *IPDataGetNum = NULL ) ;				// ＤＮＳサーバーを使ってホスト名からＩＰアドレスを取得する( IPv6版 )
extern	int			GetHostIPbyName_IPv6WithStrLen(	const TCHAR *HostName, size_t HostNameLength,	IPDATA_IPv6 *IPDataBuf, int IPDataBufLength = 1 , int *IPDataGetNum = NULL ) ;				// ＤＮＳサーバーを使ってホスト名からＩＰアドレスを取得する( IPv6版 )
extern 	int			ConnectNetWork(					IPDATA      IPData, int Port = -1 ) ;													// 他マシンに接続する( IPv4版 )
extern	int			ConnectNetWork_IPv6(			IPDATA_IPv6 IPData, int Port = -1 ) ;													// 他マシンに接続する( IPv6版 )
extern 	int			ConnectNetWork_ASync(			IPDATA      IPData, int Port = -1 ) ;													// 他マシンに接続する( IPv4版 )、非同期版
extern	int			ConnectNetWork_IPv6_ASync(		IPDATA_IPv6 IPData, int Port = -1 ) ;													// 他マシンに接続する( IPv6版 )、非同期版
extern 	int			PreparationListenNetWork(		int Port = -1 ) ;																		// 接続を受けられる状態にする( IPv4版 )
extern 	int			PreparationListenNetWork_IPv6(	int Port = -1 ) ;																		// 接続を受けられる状態にする( IPv6版 )
extern 	int			StopListenNetWork(				void ) ;																				// 接続を受けつけ状態の解除
extern 	int			CloseNetWork(					int NetHandle ) ;																		// 接続を終了する

extern 	int			GetNetWorkAcceptState(			int NetHandle ) ;																		// 接続状態を取得する
extern 	int			GetNetWorkDataLength(			int NetHandle ) ;																		// 受信データの量を得る
extern	int			GetNetWorkSendDataLength(		int NetHandle ) ;																		// 未送信のデータの量を得る 
extern 	int			GetNewAcceptNetWork(			void ) ;																				// 新たに接続した通信回線を得る
extern 	int			GetLostNetWork(					void ) ;																				// 接続を切断された通信回線を得る
extern 	int			GetNetWorkIP(					int NetHandle, IPDATA      *IpBuf ) ;													// 接続先のＩＰを得る( IPv4版 )
extern 	int			GetNetWorkIP_IPv6(				int NetHandle, IPDATA_IPv6 *IpBuf ) ;													// 接続先のＩＰを得る( IPv6版 )
extern	int			GetMyIPAddress(					IPDATA      *IpBuf, int IpBufLength = 1 , int *IpNum = NULL ) ;							// 自分のIPv4を得る
extern	int			GetMyIPAddress_IPv6(			IPDATA_IPv6 *IpBuf, int IpBufLength = 1 , int *IpNum = NULL ) ;							// 自分のIPv6を得る
extern	int			SetConnectTimeOutWait(			int Time ) ;																			// 接続のタイムアウトまでの時間を設定する
extern	int			SetUseDXNetWorkProtocol(		int Flag ) ;																			// ＤＸライブラリの通信形態を使うかどうかをセットする
extern	int			GetUseDXNetWorkProtocol(		void ) ; 																				// ＤＸライブラリの通信形態を使うかどうかを取得する
extern	int			SetUseDXProtocol(				int Flag ) ;																			// SetUseDXNetWorkProtocol の別名
extern	int			GetUseDXProtocol(				void ) ; 																				// GetUseDXNetWorkProtocol の別名
extern	int			SetNetWorkCloseAfterLostFlag(	int Flag ) ;																			// 接続が切断された直後に接続ハンドルを解放するかどうかのフラグをセットする
extern	int			GetNetWorkCloseAfterLostFlag(	void ) ;																				// 接続が切断された直後に接続ハンドルを解放するかどうかのフラグを取得する
//extern	int			SetProxySetting( int UseFlag, const char *Address, int Port ) ;														// ＨＴＴＰ通信で使用するプロキシ設定を行う
//extern	int			GetProxySetting( int *UseFlagBuffer, char *AddressBuffer, int *PortBuffer ) ;										// ＨＴＴＰ通信で使用するプロキシ設定を取得する
//extern	int			SetIEProxySetting( void ) ;																							// ＩＥのプロキシ設定を適応する

extern 	int			NetWorkRecv(			int NetHandle, void *Buffer, int Length ) ;														// 受信したデータを読み込む
extern	int			NetWorkRecvToPeek(		int NetHandle, void *Buffer, int Length ) ;														// 受信したデータを読み込む、読み込んだデータはバッファから削除されない
extern	int			NetWorkRecvBufferClear(	int NetHandle ) ;																				// 受信したデータをクリアする
extern 	int			NetWorkSend(			int NetHandle, const void *Buffer, int Length ) ;												// データを送信する

extern	int			MakeUDPSocket(			int RecvPort = -1 ) ;																			// UDPを使用した通信を行うソケットハンドルを作成する( RecvPort を -1 にすると送信専用のソケットハンドルになります )
extern	int			MakeUDPSocket_IPv6(		int RecvPort = -1 ) ;																			// UDPを使用した通信を行うソケットハンドルを作成する( RecvPort を -1 にすると送信専用のソケットハンドルになります )( IPv6版 )
extern	int			DeleteUDPSocket(		int NetUDPHandle ) ;																			// UDPを使用した通信を行うソケットハンドルを削除する
extern	int			NetWorkSendUDP(			int NetUDPHandle, IPDATA       SendIP, int SendPort,  const void *Buffer, int Length ) ;			// UDPを使用した通信で指定のＩＰにデータを送信する、Length は最大65507、SendPort を -1 にすると MakeUDPSocket に RecvPort で渡したポートが使用されます( 戻り値  0以上;送信できたデータサイズ  -1:エラー  -2:送信データが大きすぎる  -3:送信準備ができていない  )
extern	int			NetWorkSendUDP_IPv6(	int NetUDPHandle, IPDATA_IPv6  SendIP, int SendPort,  const void *Buffer, int Length ) ;			// UDPを使用した通信で指定のＩＰにデータを送信する、Length は最大65507、SendPort を -1 にすると MakeUDPSocket に RecvPort で渡したポートが使用されます( 戻り値  0以上;送信できたデータサイズ  -1:エラー  -2:送信データが大きすぎる  -3:送信準備ができていない  )( IPv6版 )
extern	int			NetWorkRecvUDP(			int NetUDPHandle, IPDATA      *RecvIP, int *RecvPort,       void *Buffer, int Length, int Peek ) ;	// UDPを使用した通信でデータを受信する、Peek に TRUE を渡すと受信に成功してもデータを受信キューから削除しません( 戻り値  0以上:受信したデータのサイズ  -1:エラー  -2:バッファのサイズが足りない  -3:受信データがない )
extern	int			NetWorkRecvUDP_IPv6(	int NetUDPHandle, IPDATA_IPv6 *RecvIP, int *RecvPort,       void *Buffer, int Length, int Peek ) ;	// UDPを使用した通信でデータを受信する、Peek に TRUE を渡すと受信に成功してもデータを受信キューから削除しません( 戻り値  0以上:受信したデータのサイズ  -1:エラー  -2:バッファのサイズが足りない  -3:受信データがない )( IPv6版 )
//extern int		CheckNetWorkSendUDP(	int NetUDPHandle ) ;																			// UDPを使用した通信でデータが送信できる状態かどうかを調べる( 戻り値  -1:エラー  TRUE:送信可能  FALSE:送信不可能 )
extern	int			CheckNetWorkRecvUDP(	int NetUDPHandle ) ;																			// UDPを使用した通信で新たな受信データが存在するかどうかを調べる( 戻り値  -1:エラー  TRUE:受信データあり  FALSE:受信データなし )

/*	使用不可
extern	int			HTTP_FileDownload(			const char *FileURL, const char *SavePath = NULL , void **SaveBufferP = NULL , int *FileSize = NULL , char **ParamList = NULL ) ;						// HTTP を使用してネットワーク上のファイルをダウンロードする
extern	int			HTTP_GetFileSize(			const char *FileURL ) ;																		// HTTP を使用してネットワーク上のファイルのサイズを得る

extern	int			HTTP_StartFileDownload(		const char *FileURL, const char *SavePath, void **SaveBufferP = NULL , char **ParamList = NULL ) ;	// HTTP を使用したネットワーク上のファイルをダウンロードする処理を開始する
extern	int			HTTP_StartGetFileSize(		const char *FileURL ) ;																		// HTTP を使用したネットワーク上のファイルのサイズを得る処理を開始する
extern	int			HTTP_Close(					int HttpHandle ) ;																			// HTTP の処理を終了し、ハンドルを解放する
extern	int			HTTP_CloseAll(				void ) ;																					// 全てのハンドルに対して HTTP_Close を行う
extern	int			HTTP_GetState(				int HttpHandle ) ;																			// HTTP 処理の現在の状態を得る( NET_RES_COMPLETE 等 )
extern	int			HTTP_GetError(				int HttpHandle ) ;																			// HTTP 処理でエラーが発生した場合、エラーの内容を得る( HTTP_ERR_NONE 等 )
extern	int			HTTP_GetDownloadFileSize(	int HttpHandle ) ;																			// HTTP 処理で対象となっているファイルのサイズを得る( 戻り値: -1 = エラー・若しくはまだファイルのサイズを取得していない  0以上 = ファイルのサイズ )
extern	int			HTTP_GetDownloadedFileSize( int HttpHandle ) ;																			// HTTP 処理で既にダウンロードしたファイルのサイズを取得する

extern	int			fgetsForNetHandle(			int NetHandle, char *strbuffer ) ;															// fgets のネットワークハンドル版( -1:取得できず 0:取得できた )
extern	int			URLAnalys(					const char *URL, char *HostBuf = NULL , char *PathBuf = NULL , char *FileNameBuf = NULL , int *PortBuf = NULL ) ;	// ＵＲＬを解析する
extern	int			URLConvert(					char *URL, int ParamConvert = TRUE , int NonConvert = FALSE ) ;								// HTTP に渡せない記号が使われた文字列を渡せるような文字列に変換する( 戻り値: -1 = エラー  0以上 = 変換後の文字列のサイズ )
extern	int			URLParamAnalysis(			char **ParamList, char **ParamStringP ) ;													// HTTP 用パラメータリストから一つのパラメータ文字列を作成する( 戻り値:  -1 = エラー  0以上 = パラメータの文字列の長さ )
*/

#endif // DX_NON_NETWORK
















// DxInputString.cpp関数プロトタイプ宣言

#ifndef DX_NON_INPUTSTRING

// 文字コードバッファ操作関係
extern	int			StockInputChar(		TCHAR CharCode ) ;								// 文字コードバッファに文字コードをストックする
extern	int			ClearInputCharBuf(	void ) ;										// 文字コードバッファをクリアする
extern	TCHAR		GetInputChar(		int DeleteFlag ) ;								// 文字コードバッファに溜まったデータから文字コードを一つ取得する
extern	TCHAR		GetInputCharWait(	int DeleteFlag ) ;								// 文字コードバッファに溜まったデータから文字コードを一つ取得する、バッファになにも文字コードがない場合は文字コードがバッファに一文字分溜まるまで待つ

extern	int			GetOneChar(			TCHAR *CharBuffer, int DeleteFlag ) ;			// 文字コードバッファに溜まったデータから１文字分取得する
extern	int			GetOneCharWait(		TCHAR *CharBuffer, int DeleteFlag ) ;			// 文字コードバッファに溜まったデータから１文字分取得する、バッファに何も文字コードがない場合は文字コードがバッファに一文字分溜まるまで待つ
extern	int			GetCtrlCodeCmp(		TCHAR Char ) ;									// 指定の文字コードがアスキーコントロールコードか調べる

#endif // DX_NON_INPUTSTRING

#ifndef DX_NON_KEYEX

extern	int			DrawIMEInputString(				int x, int y, int SelectStringNum , int DrawCandidateList = TRUE ) ;	// 画面上に入力中の文字列を描画する
extern	int			SetUseIMEFlag(					int UseFlag ) ;							// ＩＭＥを使用するかどうかを設定する
extern	int			GetUseIMEFlag(					void ) ;								// ＩＭＥを使用するかどうかの設定を取得する
extern	int			SetInputStringMaxLengthIMESync(	int Flag ) ;							// ＩＭＥで入力できる最大文字数を MakeKeyInput の設定に合わせるかどうかをセットする( TRUE:あわせる  FALSE:あわせない(デフォルト) )
extern	int			SetIMEInputStringMaxLength(		int Length ) ;							// ＩＭＥで一度に入力できる最大文字数を設定する( 0:制限なし  1以上:指定の文字数で制限 )

#endif // DX_NON_KEYEX

extern	int			GetStringPoint(				const TCHAR *String,                      int Point ) ;		// 全角文字、半角文字入り乱れる中から指定の文字数での半角文字数を得る
extern	int			GetStringPointWithStrLen(	const TCHAR *String, size_t StringLength, int Point ) ;		// 全角文字、半角文字入り乱れる中から指定の文字数での半角文字数を得る
extern	int			GetStringPoint2(			const TCHAR *String,                      int Point ) ;		// 全角文字、半角文字入り乱れる中から指定の半角文字数での文字数を得る
extern	int			GetStringPoint2WithStrLen(	const TCHAR *String, size_t StringLength, int Point ) ;		// 全角文字、半角文字入り乱れる中から指定の半角文字数での文字数を得る
extern	int			GetStringLength(			const TCHAR *String ) ;										// 全角文字、半角文字入り乱れる中から文字数を取得する

#ifndef DX_NON_FONT
extern	int			DrawObtainsString(						int x, int y, int AddY, const TCHAR *String,                      unsigned int StrColor, unsigned int StrEdgeColor = 0 , int FontHandle = -1 , unsigned int SelectBackColor = 0xffffffff , unsigned int SelectStrColor = 0 , unsigned int SelectStrEdgeColor = 0xffffffff , int SelectStart = -1 , int SelectEnd = -1 ) ;		// 描画可能領域に収まるように改行しながら文字列を描画
extern	int			DrawObtainsNString(						int x, int y, int AddY, const TCHAR *String, size_t StringLength, unsigned int StrColor, unsigned int StrEdgeColor = 0 , int FontHandle = -1 , unsigned int SelectBackColor = 0xffffffff , unsigned int SelectStrColor = 0 , unsigned int SelectStrEdgeColor = 0xffffffff , int SelectStart = -1 , int SelectEnd = -1 ) ;		// 描画可能領域に収まるように改行しながら文字列を描画
extern	int			DrawObtainsString_CharClip(				int x, int y, int AddY, const TCHAR *String,                      unsigned int StrColor, unsigned int StrEdgeColor = 0 , int FontHandle = -1 , unsigned int SelectBackColor = 0xffffffff , unsigned int SelectStrColor = 0 , unsigned int SelectStrEdgeColor = 0xffffffff , int SelectStart = -1 , int SelectEnd = -1 ) ;		// 描画可能領域に収まるように改行しながら文字列を描画( クリップが文字単位 )
extern	int			DrawObtainsNString_CharClip(			int x, int y, int AddY, const TCHAR *String, size_t StringLength, unsigned int StrColor, unsigned int StrEdgeColor = 0 , int FontHandle = -1 , unsigned int SelectBackColor = 0xffffffff , unsigned int SelectStrColor = 0 , unsigned int SelectStrEdgeColor = 0xffffffff , int SelectStart = -1 , int SelectEnd = -1 ) ;		// 描画可能領域に収まるように改行しながら文字列を描画( クリップが文字単位 )
extern	int			GetObtainsStringCharPosition(			int x, int y, int AddY, const TCHAR *String, int StrLen, int *PosX, int *PosY, int FontHandle = -1 ) ;		// 描画可能領域に収まるように改行しながら文字列を描画した場合の文字列の末端の座標を取得する
extern	int			GetObtainsStringCharPosition_CharClip(	int x, int y, int AddY, const TCHAR *String, int StrLen, int *PosX, int *PosY, int FontHandle = -1 ) ;		// 描画可能領域に収まるように改行しながら文字列を描画した場合の文字列の末端の座標を取得する( クリップが文字単位 )
#endif // DX_NON_FONT
extern	int			DrawObtainsBox(					int x1, int y1, int x2, int y2, int AddY, unsigned int Color, int FillFlag ) ;																																										// 描画可能領域に収まるように補正を加えながら矩形を描画

#ifndef DX_NON_KEYEX

extern	int			InputStringToCustom(			int x, int y, size_t BufLength, TCHAR *StrBuffer, int CancelValidFlag, int SingleCharOnlyFlag, int NumCharOnlyFlag, int DoubleCharOnlyFlag = FALSE , int EnableNewLineFlag = FALSE , int DisplayCandidateList = TRUE ) ;		// 文字列の入力取得

extern	int			KeyInputString(					int x, int y, size_t CharMaxLength, TCHAR *StrBuffer, int CancelValidFlag ) ;													// 文字列の入力取得
extern	int			KeyInputSingleCharString(		int x, int y, size_t CharMaxLength, TCHAR *StrBuffer, int CancelValidFlag ) ;													// 半角文字列のみの入力取得
extern	int			KeyInputNumber(					int x, int y, int MaxNum, int MinNum, int CancelValidFlag ) ;																	// 数値の入力取得

extern	int			GetIMEInputModeStr(				TCHAR *GetBuffer ) ;																											// IMEの入力モード文字列を取得する
extern	const IMEINPUTDATA* GetIMEInputData(		void ) ;																														// IMEで入力中の文字列の情報を取得する
extern	int			SetKeyInputStringColor(			ULONGLONG NmlStr, ULONGLONG NmlCur, ULONGLONG IMEStrBack, ULONGLONG IMECur, ULONGLONG IMELine, ULONGLONG IMESelectStr, ULONGLONG IMEModeStr , ULONGLONG NmlStrE = 0 , ULONGLONG IMESelectStrE = 0 , ULONGLONG IMEModeStrE = 0 , ULONGLONG IMESelectWinE = ULL_PARAM( 0xffffffffffffffff ) ,	ULONGLONG IMESelectWinF = ULL_PARAM( 0xffffffffffffffff ) , ULONGLONG SelectStrBackColor = ULL_PARAM( 0xffffffffffffffff ) , ULONGLONG SelectStrColor = ULL_PARAM( 0xffffffffffffffff ) , ULONGLONG SelectStrEdgeColor = ULL_PARAM( 0xffffffffffffffff ), ULONGLONG IMEStr = ULL_PARAM( 0xffffffffffffffff ), ULONGLONG IMEStrE = ULL_PARAM( 0xffffffffffffffff )  ) ;	// ( SetKeyInputStringColor2 の旧関数 )InputString関数使用時の文字の各色を変更する
extern	int			SetKeyInputStringColor2(		int TargetColor /* DX_KEYINPSTRCOLOR_NORMAL_STR 等 */ , unsigned int Color ) ;													// InputString関数使用時の文字の各色を変更する
extern	int			ResetKeyInputStringColor2(		int TargetColor /* DX_KEYINPSTRCOLOR_NORMAL_STR 等 */ ) ;																		// SetKeyInputStringColor2 で設定した色をデフォルトに戻す
extern	int			SetKeyInputStringFont(			int FontHandle ) ;																												// キー入力文字列描画関連で使用するフォントのハンドルを変更する(-1でデフォルトのフォントハンドル)
extern	int			SetKeyInputStringEndCharaMode(	int EndCharaMode /* DX_KEYINPSTR_ENDCHARAMODE_OVERWRITE 等 */ ) ;																// キー入力文字列処理の入力文字数が限界に達している状態で、文字列の末端部分で入力が行われた場合の処理モードを変更する
extern	int			DrawKeyInputModeString(			int x, int y ) ;																												// 入力モード文字列を描画する

extern	int			InitKeyInput(					void ) ;																														// キー入力データ初期化
extern	int			MakeKeyInput(					size_t MaxStrLength, int CancelValidFlag, int SingleCharOnlyFlag, int NumCharOnlyFlag, int DoubleCharOnlyFlag = FALSE , int EnableNewLineFlag = FALSE ) ;			// 新しいキー入力ハンドルの作成
extern	int			DeleteKeyInput(					int InputHandle ) ;																												// キー入力ハンドルの削除
extern	int			SetActiveKeyInput(				int InputHandle ) ;																												// 指定のキー入力ハンドルをアクティブにする( -1 を指定するとアクティブなキー入力ハンドルが無い状態になります )
extern	int			GetActiveKeyInput(				void ) ;																														// 現在アクティブになっているキー入力ハンドルを取得する
extern	int			CheckKeyInput(					int InputHandle ) ;																												// キー入力ハンドルの入力が終了しているか取得する
extern	int			ReStartKeyInput(				int InputHandle ) ;																												// 入力が完了したキー入力ハンドルを再度編集状態に戻す
extern	int			ProcessActKeyInput(				void ) ;																														// キー入力ハンドル処理関数
extern	int			DrawKeyInputString(				int x, int y, int InputHandle , int DrawCandidateList = TRUE ) ;																// キー入力ハンドルの入力中情報の描画
extern	int			SetKeyInputDrawArea(			int x1, int y1, int x2, int y2, int InputHandle ) ;																				// キー入力ハンドルの入力中文字列を描画する際の描画範囲を設定する

extern	int			SetKeyInputSelectArea(			int  SelectStart, int  SelectEnd, int InputHandle ) ;																			// キー入力ハンドルの指定の領域を選択状態にする( SelectStart と SelectEnd に -1 を指定すると選択状態が解除されます )
extern	int			GetKeyInputSelectArea(			int *SelectStart, int *SelectEnd, int InputHandle ) ;																			// キー入力ハンドルの選択領域を取得する
extern	int			SetKeyInputDrawStartPos(		int DrawStartPos, int InputHandle ) ;																							// キー入力ハンドルの描画開始文字位置を設定する
extern	int			GetKeyInputDrawStartPos(		int InputHandle ) ;																												// キー入力ハンドルの描画開始文字位置を取得する
extern	int			SetKeyInputCursorBrinkTime(		int Time ) ;																													// キー入力ハンドルのキー入力時のカーソルの点滅する早さをセットする
extern	int			SetKeyInputCursorBrinkFlag(		int Flag ) ;																													// キー入力ハンドルのキー入力時のカーソルを点滅させるかどうかをセットする
extern	int			SetKeyInputString(				const TCHAR *String,                      int InputHandle ) ;																	// キー入力ハンドルに指定の文字列をセットする
extern	int			SetKeyInputStringWithStrLen(	const TCHAR *String, size_t StringLength, int InputHandle ) ;																	// キー入力ハンドルに指定の文字列をセットする
extern	int			SetKeyInputNumber(				int   Number,                             int InputHandle ) ;																	// キー入力ハンドルに指定の数値を文字に置き換えてセットする
extern	int			SetKeyInputNumberToFloat(		float Number,                             int InputHandle ) ;																	// キー入力ハンドルに指定の浮動小数点値を文字に置き換えてセットする
extern	int			GetKeyInputString(				TCHAR *StrBuffer,                         int InputHandle ) ;																	// キー入力ハンドルの入力中の文字列を取得する
extern	int			GetKeyInputNumber(				int InputHandle ) ;																												// キー入力ハンドルの入力中の文字列を整数値として取得する
extern	float		GetKeyInputNumberToFloat(		int InputHandle ) ;																												// キー入力ハンドルの入力中の文字列を浮動小数点値として取得する
extern	int			SetKeyInputCursorPosition(		int Position,        int InputHandle ) ;																						// キー入力ハンドルの現在のカーソル位置を設定する
extern	int			GetKeyInputCursorPosition(		int InputHandle ) ;																												// キー入力ハンドルの現在のカーソル位置を取得する

#endif // DX_NON_KEYEX











// DxFile.cpp関数プロトタイプ宣言

// ファイルアクセス関数
extern	int			FileRead_open(						const TCHAR *FilePath,                        int ASync = FALSE ) ;	// ファイルを開く
extern	int			FileRead_open_WithStrLen(			const TCHAR *FilePath, size_t FilePathLength, int ASync = FALSE ) ;	// ファイルを開く
extern	int			FileRead_open_mem(					const void *FileImage, size_t FileImageSize ) ;					// メモリに展開されたファイルを開く
extern	LONGLONG	FileRead_size(						const TCHAR *FilePath ) ;										// ファイルのサイズを取得する
extern	LONGLONG	FileRead_size_WithStrLen(			const TCHAR *FilePath, size_t FilePathLength ) ;				// ファイルのサイズを取得する
extern	int			FileRead_close(						int FileHandle ) ;												// ファイルを閉じる
extern	LONGLONG	FileRead_tell(						int FileHandle ) ;												// ファイルポインタの読み込み位置を取得する
extern	int			FileRead_seek(						int FileHandle , LONGLONG Offset , int Origin ) ;				// ファイルポインタの読み込み位置を変更する
extern	int			FileRead_read(						void *Buffer , int ReadSize , int FileHandle ) ;				// ファイルからデータを読み込む
extern	int			FileRead_idle_chk(					int FileHandle ) ;												// ファイル読み込みが完了しているかどうかを取得する
extern	int			FileRead_eof(						int FileHandle ) ;												// ファイルの読み込み位置が終端に達しているかどうかを取得する
extern	int			FileRead_set_format(				int FileHandle, int CharCodeFormat /* DX_CHARCODEFORMAT_SHIFTJIS 等 */ ) ;	// ファイルの文字コード形式を設定する( テキストファイル用 )
extern	int			FileRead_gets(						TCHAR *Buffer , int BufferSize , int FileHandle ) ;				// ファイルから文字列を読み出す
extern	TCHAR		FileRead_getc(						int FileHandle ) ;												// ファイルから一文字読み出す
extern	int			FileRead_scanf(						int FileHandle , const TCHAR *Format , ... ) ;					// ファイルから書式化されたデータを読み出す

extern	DWORD_PTR	FileRead_createInfo(				const TCHAR *ObjectPath ) ;										// ファイル情報ハンドルを作成する( 戻り値  -1:エラー  -1以外:ファイル情報ハンドル )
extern	DWORD_PTR	FileRead_createInfo_WithStrLen(		const TCHAR *ObjectPath, size_t ObjectPathLength ) ;			// ファイル情報ハンドルを作成する( 戻り値  -1:エラー  -1以外:ファイル情報ハンドル )
extern	int			FileRead_getInfoNum(				DWORD_PTR FileInfoHandle ) ;									// ファイル情報ハンドル中のファイルの数を取得する
extern	int			FileRead_getInfo(					int Index , FILEINFO *Buffer , DWORD_PTR FileInfoHandle ) ;		// ファイル情報ハンドル中のファイルの情報を取得する
extern	int			FileRead_deleteInfo(				DWORD_PTR FileInfoHandle ) ;									// ファイル情報ハンドルを削除する

extern	DWORD_PTR	FileRead_findFirst(					const TCHAR *FilePath,                        FILEINFO *Buffer ) ; // 指定のファイル又はフォルダの情報を取得し、ファイル検索ハンドルも作成する( 戻り値: -1=エラー  -1以外=ファイル検索ハンドル )
extern	DWORD_PTR	FileRead_findFirst_WithStrLen(		const TCHAR *FilePath, size_t FilePathLength, FILEINFO *Buffer ) ; // 指定のファイル又はフォルダの情報を取得し、ファイル検索ハンドルも作成する( 戻り値: -1=エラー  -1以外=ファイル検索ハンドル )
extern	int			FileRead_findNext(					DWORD_PTR FindHandle, FILEINFO *Buffer ) ;						// 条件の合致する次のファイルの情報を取得する( 戻り値: -1=エラー  0=成功 )
extern	int			FileRead_findClose(					DWORD_PTR FindHandle ) ;										// ファイル検索ハンドルを閉じる( 戻り値: -1=エラー  0=成功 )

extern	int			FileRead_fullyLoad(					const TCHAR *FilePath ) ;										// 指定のファイルの内容を全てメモリに読み込み、その情報のアクセスに必要なハンドルを返す( 戻り値  -1:エラー  -1以外:ハンドル )、使い終わったらハンドルは FileRead_fullyLoad_delete で削除する必要があります
extern	int			FileRead_fullyLoad_WithStrLen(		const TCHAR *FilePath, size_t FilePathLength ) ;				// 指定のファイルの内容を全てメモリに読み込み、その情報のアクセスに必要なハンドルを返す( 戻り値  -1:エラー  -1以外:ハンドル )、使い終わったらハンドルは FileRead_fullyLoad_delete で削除する必要があります
extern	int			FileRead_fullyLoad_delete(			int FLoadHandle ) ;												// FileRead_fullyLoad で読み込んだファイルのハンドルを削除する
extern	const void*	FileRead_fullyLoad_getImage(		int FLoadHandle ) ;												// FileRead_fullyLoad で読み込んだファイルの内容を格納したメモリアドレスを取得する
extern	LONGLONG	FileRead_fullyLoad_getSize(			int FLoadHandle ) ;												// FileRead_fullyLoad で読み込んだファイルのサイズを取得する

// 設定関係関数
extern	int			GetStreamFunctionDefault(			void ) ;														// ＤＸライブラリでストリームデータアクセスに使用する関数がデフォルトのものか調べる( TRUE:デフォルトのもの  FALSE:デフォルトではない )
extern	int			ChangeStreamFunction(				const STREAMDATASHREDTYPE2  *StreamThread  ) ;					// ＤＸライブラリでストリームデータアクセスに使用する関数を変更する
extern	int			ChangeStreamFunctionW(				const STREAMDATASHREDTYPE2W *StreamThreadW ) ;					// ＤＸライブラリでストリームデータアクセスに使用する関数を変更する( wchar_t 使用版 )

// 補助関係関数
extern int			ConvertFullPath(					const TCHAR *Src,                   TCHAR *Dest, const TCHAR *CurrentDir = NULL                              ) ;	// フルパスではないパス文字列をフルパスに変換する( CurrentDir はフルパスである必要がある(語尾に『\』があっても無くても良い) )( CurrentDir が NULL の場合は現在のカレントディレクトリを使用する )
extern int			ConvertFullPathWithStrLen(			const TCHAR *Src, size_t SrcLength, TCHAR *Dest, const TCHAR *CurrentDir = NULL, size_t CurrentDirLength = 0 ) ;	// フルパスではないパス文字列をフルパスに変換する( CurrentDir はフルパスである必要がある(語尾に『\』があっても無くても良い) )( CurrentDir が NULL の場合は現在のカレントディレクトリを使用する )












// DxInput.cpp関数プロトタイプ宣言

#ifndef DX_NON_INPUT

// 入力状態取得関数
extern	int			CheckHitKey(							int KeyCode ) ;															// キーボードの押下状態を取得する
extern	int			CheckHitKeyAll(							int CheckType = DX_CHECKINPUT_ALL ) ;									// どれか一つでもキーが押されているかどうかを取得( 押されていたら戻り値が 0 以外になる )
extern	int			GetHitKeyStateAll(						DX_CHAR *KeyStateArray ) ;												// すべてのキーの押下状態を取得する( KeyStateBuf:char型256個分の配列の先頭アドレス )
extern	int			GetJoypadNum(							void ) ;																// ジョイパッドが接続されている数を取得する
extern	int			GetJoypadInputState(					int InputType ) ;														// ジョイパッドの入力状態を取得する
extern	int			GetJoypadAnalogInput(					int *XBuf, int *YBuf, int InputType ) ;									// ジョイパッドのアナログ的なスティック入力情報を得る
extern	int			GetJoypadAnalogInputRight(				int *XBuf, int *YBuf, int InputType ) ;									// ( 使用非推奨 )ジョイパッドのアナログ的なスティック入力情報を得る(右スティック用)
extern	int			GetJoypadDirectInputState(				int InputType, DINPUT_JOYSTATE *DInputState ) ;							// DirectInput から得られるジョイパッドの生のデータを取得する( DX_INPUT_KEY や DX_INPUT_KEY_PAD1 など、キーボードが絡むタイプを InputType に渡すとエラーとなり -1 を返す )
extern	int			CheckJoypadXInput(						int InputType ) ;														// 指定の入力デバイスが XInput に対応しているかどうかを取得する( 戻り値  TRUE:XInput対応の入力デバイス  FALSE:XInput非対応の入力デバイス   -1:エラー )( DX_INPUT_KEY や DX_INPUT_KEY_PAD1 など、キーボードが絡むタイプを InputType に渡すとエラーとなり -1 を返す )
extern	int			GetJoypadXInputState(					int InputType, XINPUT_STATE *XInputState ) ;							// XInput から得られる入力デバイス( Xbox360コントローラ等 )の生のデータを取得する( XInput非対応のパッドの場合はエラーとなり -1 を返す、DX_INPUT_KEY や DX_INPUT_KEY_PAD1 など、キーボードが絡むタイプを InputType に渡すとエラーとなり -1 を返す )
extern	int			SetJoypadInputToKeyInput(				int InputType, int PadInput, int KeyInput1, int KeyInput2 = -1 , int KeyInput3 = -1 , int KeyInput4 = -1  ) ; // ジョイパッドの入力に対応したキーボードの入力を設定する( InputType:設定を変更するパッドの識別子( DX_INPUT_PAD1等 )　　PadInput:設定を変更するパッドボタンの識別子( PAD_INPUT_1 等 )　　KeyInput1:PadInput を押下したことにするキーコード( KEY_INPUT_A など )その１　　KeyInput2:その２、-1で設定なし　　KeyInput3:その３、-1で設定なし　　KeyInput4:その４、-1で設定なし )
extern	int			SetJoypadDeadZone(						int InputType, double Zone ) ;											// ジョイパッドの無効ゾーンの設定を行う( InputType:設定を変更するパッドの識別子( DX_INPUT_PAD1等 )   Zone:新しい無効ゾーン( 0.0 〜 1.0 )、デフォルト値は 0.35 )
extern	double		GetJoypadDeadZone(						int InputType ) ;														// ジョイパッドの無効ゾーンの設定を取得する( InputType:設定を変更するパッドの識別子( DX_INPUT_PAD1等 )   戻り値:無効ゾーン( 0.0 〜 1.0 )
extern	int			StartJoypadVibration(					int InputType, int Power, int Time, int EffectIndex = -1 ) ;			// ジョイパッドの振動を開始する
extern	int			StopJoypadVibration(					int InputType, int EffectIndex = -1 ) ;									// ジョイパッドの振動を停止する
extern	int			GetJoypadPOVState(						int InputType, int POVNumber ) ;										// ジョイパッドのＰＯＶ入力の状態を得る( 戻り値　指定のPOVデータの角度、単位は角度の１００倍( 90度なら 9000 ) 中心位置にある場合は -1 が返る )
extern	int			ReSetupJoypad(							void ) ;																// ジョイパッドの再セットアップを行う( 新たに接続されたジョイパッドがあったら検出される )

extern	int			SetUseJoypadVibrationFlag(				int Flag ) ;															// ジョイパッドの振動機能を使用するかどうかを設定する( TRUE:使用する　　FALSE:使用しない )

#endif // DX_NON_INPUT







#ifndef DX_NOTUSE_DRAWFUNCTION

// 画像処理系関数プロトタイプ宣言

// グラフィックハンドル作成関係関数
extern	int			MakeGraph(							int SizeX, int SizeY, int NotUse3DFlag = FALSE ) ;							// 指定サイズのグラフィックハンドルを作成する
extern	int			MakeScreen(							int SizeX, int SizeY, int UseAlphaChannel = FALSE ) ;						// SetDrawScreen で描画対象にできるグラフィックハンドルを作成する
extern	int			DerivationGraph(					int   SrcX, int   SrcY, int   Width, int   Height, int SrcGraphHandle ) ;	// 指定のグラフィックハンドルの指定部分だけを抜き出して新たなグラフィックハンドルを作成する
extern	int			DerivationGraphF(					float SrcX, float SrcY, float Width, float Height, int SrcGraphHandle ) ;	// 指定のグラフィックハンドルの指定部分だけを抜き出して新たなグラフィックハンドルを作成する( float版 )
extern	int			DeleteGraph(						int GrHandle, int LogOutFlag = FALSE ) ;									// グラフィックハンドルを削除する
extern	int			DeleteSharingGraph(					int GrHandle ) ;															// 指定のグラフィックハンドルと、同じグラフィックハンドルから派生しているグラフィックハンドル( DerivationGraph で派生したハンドル、LoadDivGraph 読み込んで作成された複数のハンドル )を一度に削除する
extern	int			GetGraphNum(						void ) ;																	// 有効なグラフィックハンドルの数を取得する
extern	int			FillGraph(							int GrHandle, int Red, int Green, int Blue, int Alpha = 255 ) ;				// グラフィックハンドルを指定の色で塗りつぶす
extern	int			FillRectGraph(						int GrHandle, int x, int y, int Width, int Height, int Red, int Green, int Blue, int Alpha = 255 ) ;	// グラフィックハンドルの指定の範囲を指定の色で塗りつぶす
extern	int			SetGraphLostFlag(					int GrHandle, int *LostFlag ) ;												// 指定のグラフィックハンドルが削除された際に 1 にする変数のアドレスを設定する
extern	int			InitGraph(							int LogOutFlag = FALSE ) ;													// すべてのグラフィックハンドルを削除する
extern	int			ReloadFileGraphAll(					void ) ;																	// ファイルから画像を読み込んだ全てのグラフィックハンドルについて、再度ファイルから画像を読み込む

// シャドウマップハンドル関係関数
extern	int			MakeShadowMap(						int SizeX, int SizeY ) ;													// シャドウマップハンドルを作成する
extern	int			DeleteShadowMap(					int SmHandle ) ;															// シャドウマップハンドルを削除する
extern	int			SetShadowMapLightDirection(			int SmHandle, VECTOR Direction ) ;											// シャドウマップが想定するライトの方向を設定する
extern	int			ShadowMap_DrawSetup(				int SmHandle ) ;															// シャドウマップへの描画の準備を行う
extern	int			ShadowMap_DrawEnd(					void ) ;																	// シャドウマップへの描画を終了する
extern	int			SetUseShadowMap(					int SmSlotIndex, int SmHandle ) ;											// 描画で使用するシャドウマップを指定する、有効なスロットは０〜２、SmHandle に -1 を渡すと指定のスロットのシャドウマップを解除
extern	int			SetShadowMapDrawArea(				int SmHandle, VECTOR MinPosition, VECTOR MaxPosition ) ;					// シャドウマップに描画する際の範囲を設定する( この関数で描画範囲を設定しない場合は視錐台を拡大した範囲が描画範囲となる )
extern	int			ResetShadowMapDrawArea(				int SmHandle ) ;															// SetShadowMapDrawArea の設定を解除する
extern	int			SetShadowMapAdjustDepth(			int SmHandle, float Depth ) ;												// シャドウマップを使用した描画時の補正深度を設定する
extern	int			GetShadowMapViewProjectionMatrix(	int SmHandle, MATRIX *MatrixBuffer ) ;										// シャドウマップ作成時や適用時に使用するビュー行列と射影行列を乗算した行列を取得する
extern	int			TestDrawShadowMap(					int SmHandle, int x1, int y1, int x2, int y2 ) ;							// シャドウマップを画面にテスト描画する

// グラフィックハンドルへの画像転送関数
extern	int			BltBmpToGraph(						const COLORDATA *BmpColorData, HBITMAP RgbBmp, HBITMAP AlphaBmp,                                                                      int CopyPointX, int CopyPointY,                              int  GrHandle ) ;					// ＢＭＰの内容をグラフィックハンドルに転送
extern	int			BltBmpToDivGraph(					const COLORDATA *BmpColorData, HBITMAP RgbBmp, HBITMAP AlphaBmp,                                                                      int AllNum, int XNum, int YNum, int Width, int Height, const int *GrHandle, int ReverseFlag ) ;	// ＢＭＰの内容を分割作成したグラフィックハンドルたちに転送
extern	int			BltBmpOrGraphImageToGraph(			const COLORDATA *BmpColorData, HBITMAP RgbBmp, HBITMAP AlphaBmp, int BmpFlag, const BASEIMAGE *RgbBaseImage, const BASEIMAGE *AlphaBaseImage, int CopyPointX, int CopyPointY,                              int  GrHandle ) ;					// ＢＭＰ か BASEIMAGE をグラフィックハンドルに転送
extern	int			BltBmpOrGraphImageToGraph2(			const COLORDATA *BmpColorData, HBITMAP RgbBmp, HBITMAP AlphaBmp, int BmpFlag, const BASEIMAGE *RgbBaseImage, const BASEIMAGE *AlphaBaseImage, const RECT *SrcRect, int DestX, int DestY,                   int  GrHandle ) ;					// ＢＭＰ か BASEIMAGE の指定の領域をグラフィックハンドルに転送
extern	int			BltBmpOrGraphImageToDivGraph(		const COLORDATA *BmpColorData, HBITMAP RgbBmp, HBITMAP AlphaBmp, int BmpFlag, const BASEIMAGE *RgbBaseImage, const BASEIMAGE *AlphaBaseImage, int AllNum, int XNum, int YNum, int   Width, int   Height, const int *GrHandle, int ReverseFlag ) ;	// ＢＭＰ か BASEIMAGE を分割作成したグラフィックハンドルたちに転送
extern	int			BltBmpOrGraphImageToDivGraphF(		const COLORDATA *BmpColorData, HBITMAP RgbBmp, HBITMAP AlphaBmp, int BmpFlag, const BASEIMAGE *RgbBaseImage, const BASEIMAGE *AlphaBaseImage, int AllNum, int XNum, int YNum, float Width, float Height, const int *GrHandle, int ReverseFlag ) ;	// ＢＭＰ か BASEIMAGE を分割作成したグラフィックハンドルたちに転送( float型 )

// 画像からグラフィックハンドルを作成する関数
extern	int			LoadBmpToGraph(						const TCHAR *FileName,                        int TextureFlag, int ReverseFlag, int SurfaceMode = DX_MOVIESURFACE_NORMAL ) ;										// 画像ファイルからグラフィックハンドルを作成する
extern	int			LoadBmpToGraphWithStrLen(			const TCHAR *FileName, size_t FileNameLength, int TextureFlag, int ReverseFlag, int SurfaceMode = DX_MOVIESURFACE_NORMAL ) ;										// 画像ファイルからグラフィックハンドルを作成する
extern	int			LoadGraph(							const TCHAR *FileName,                        int NotUse3DFlag = FALSE ) ;																							// 画像ファイルからグラフィックハンドルを作成する
extern	int			LoadGraphWithStrLen(				const TCHAR *FileName, size_t FileNameLength, int NotUse3DFlag = FALSE ) ;																							// 画像ファイルからグラフィックハンドルを作成する
extern	int			LoadReverseGraph(					const TCHAR *FileName,                        int NotUse3DFlag = FALSE ) ;																							// 画像ファイルを反転したものでグラフィックハンドルを作成する
extern	int			LoadReverseGraphWithStrLen(			const TCHAR *FileName, size_t FileNameLength, int NotUse3DFlag = FALSE ) ;																							// 画像ファイルを反転したものでグラフィックハンドルを作成する
extern	int			LoadDivGraph(						const TCHAR *FileName,                        int AllNum, int XNum, int YNum, int   XSize, int   YSize, int *HandleArray, int NotUse3DFlag = FALSE ) ;				// 画像ファイルを分割してグラフィックハンドルを作成する
extern	int			LoadDivGraphWithStrLen(				const TCHAR *FileName, size_t FileNameLength, int AllNum, int XNum, int YNum, int   XSize, int   YSize, int *HandleArray, int NotUse3DFlag = FALSE ) ;				// 画像ファイルを分割してグラフィックハンドルを作成する
extern	int			LoadDivGraphF(						const TCHAR *FileName,                        int AllNum, int XNum, int YNum, float XSize, float YSize, int *HandleArray, int NotUse3DFlag = FALSE ) ;				// 画像ファイルを分割してグラフィックハンドルを作成する
extern	int			LoadDivGraphFWithStrLen(			const TCHAR *FileName, size_t FileNameLength, int AllNum, int XNum, int YNum, float XSize, float YSize, int *HandleArray, int NotUse3DFlag = FALSE ) ;				// 画像ファイルを分割してグラフィックハンドルを作成する
extern	int			LoadDivBmpToGraph(					const TCHAR *FileName,                        int AllNum, int XNum, int YNum, int   SizeX, int   SizeY, int *HandleArray, int TextureFlag, int ReverseFlag ) ;		// 画像ファイルを分割してグラフィックハンドルを作成する
extern	int			LoadDivBmpToGraphWithStrLen(		const TCHAR *FileName, size_t FileNameLength, int AllNum, int XNum, int YNum, int   SizeX, int   SizeY, int *HandleArray, int TextureFlag, int ReverseFlag ) ;		// 画像ファイルを分割してグラフィックハンドルを作成する
extern	int			LoadDivBmpToGraphF(					const TCHAR *FileName,                        int AllNum, int XNum, int YNum, float SizeX, float SizeY, int *HandleArray, int TextureFlag, int ReverseFlag ) ;		// 画像ファイルを分割してグラフィックハンドルを作成する
extern	int			LoadDivBmpToGraphFWithStrLen(		const TCHAR *FileName, size_t FileNameLength, int AllNum, int XNum, int YNum, float SizeX, float SizeY, int *HandleArray, int TextureFlag, int ReverseFlag ) ;		// 画像ファイルを分割してグラフィックハンドルを作成する
extern	int			LoadReverseDivGraph(				const TCHAR *FileName,                        int AllNum, int XNum, int YNum, int   XSize, int   YSize, int *HandleArray, int NotUse3DFlag = FALSE ) ;				// 画像ファイルを反転したものを分割してグラフィックハンドルを作成する
extern	int			LoadReverseDivGraphWithStrLen(		const TCHAR *FileName, size_t FileNameLength, int AllNum, int XNum, int YNum, int   XSize, int   YSize, int *HandleArray, int NotUse3DFlag = FALSE ) ;				// 画像ファイルを反転したものを分割してグラフィックハンドルを作成する
extern	int			LoadReverseDivGraphF(				const TCHAR *FileName,                        int AllNum, int XNum, int YNum, float XSize, float YSize, int *HandleArray, int NotUse3DFlag = FALSE ) ;				// 画像ファイルを反転したものを分割してグラフィックハンドルを作成する
extern	int			LoadReverseDivGraphFWithStrLen(		const TCHAR *FileName, size_t FileNameLength, int AllNum, int XNum, int YNum, float XSize, float YSize, int *HandleArray, int NotUse3DFlag = FALSE ) ;				// 画像ファイルを反転したものを分割してグラフィックハンドルを作成する
extern	int			LoadBlendGraph(						const TCHAR *FileName ) ;																																			// 画像ファイルからブレンド用グラフィックハンドルを作成する
extern	int			LoadBlendGraphWithStrLen(			const TCHAR *FileName, size_t FileNameLength ) ;																													// 画像ファイルからブレンド用グラフィックハンドルを作成する

extern	int			CreateGraphFromMem(					const void *RGBFileImage, int RGBFileImageSize,               const void *AlphaFileImage = NULL , int AlphaFileImageSize = 0 ,                  int TextureFlag = TRUE , int ReverseFlag = FALSE ) ;																			// メモリ上の画像イメージからグラフィックハンドルを作成する
extern	int			ReCreateGraphFromMem(				const void *RGBFileImage, int RGBFileImageSize, int GrHandle, const void *AlphaFileImage = NULL , int AlphaFileImageSize = 0 ,                  int TextureFlag = TRUE , int ReverseFlag = FALSE ) ;																			// メモリ上の画像イメージから既存のグラフィックハンドルにデータを転送する
extern	int			CreateDivGraphFromMem(				const void *RGBFileImage, int RGBFileImageSize, int AllNum, int XNum, int YNum, int   SizeX, int   SizeY,       int *HandleArray,               int TextureFlag = TRUE , int ReverseFlag = FALSE , const void *AlphaFileImage = NULL , int AlphaFileImageSize = 0 ) ;			// メモリ上の画像イメージから分割グラフィックハンドルを作成する
extern	int			CreateDivGraphFFromMem(				const void *RGBFileImage, int RGBFileImageSize, int AllNum, int XNum, int YNum, float SizeX, float SizeY,       int *HandleArray,               int TextureFlag = TRUE , int ReverseFlag = FALSE , const void *AlphaFileImage = NULL , int AlphaFileImageSize = 0 ) ;			// メモリ上の画像イメージから分割グラフィックハンドルを作成する( float版 )
extern	int			ReCreateDivGraphFromMem(			const void *RGBFileImage, int RGBFileImageSize, int AllNum, int XNum, int YNum, int   SizeX, int   SizeY, const int *HandleArray,               int TextureFlag = TRUE , int ReverseFlag = FALSE , const void *AlphaFileImage = NULL , int AlphaFileImageSize = 0 ) ;			// メモリ上の画像イメージから既存の分割グラフィックハンドルにデータを転送する
extern	int			ReCreateDivGraphFFromMem(			const void *RGBFileImage, int RGBFileImageSize, int AllNum, int XNum, int YNum, float SizeX, float SizeY, const int *HandleArray,               int TextureFlag = TRUE , int ReverseFlag = FALSE , const void *AlphaFileImage = NULL , int AlphaFileImageSize = 0 ) ;			// メモリ上の画像イメージから既存の分割グラフィックハンドルにデータを転送する( float版 )
extern	int			CreateGraphFromBmp(					const BITMAPINFO *RGBBmpInfo, const void *RGBBmpImage,               const BITMAPINFO *AlphaBmpInfo = NULL , const void *AlphaBmpImage = NULL , int TextureFlag = TRUE , int ReverseFlag = FALSE ) ;																			// ビットマップデータからグラフィックハンドルを作成する
extern	int			ReCreateGraphFromBmp(				const BITMAPINFO *RGBBmpInfo, const void *RGBBmpImage, int GrHandle, const BITMAPINFO *AlphaBmpInfo = NULL , const void *AlphaBmpImage = NULL , int TextureFlag = TRUE , int ReverseFlag = FALSE ) ;																			// ビットマップデータから既存のグラフィックハンドルにデータを転送する
extern	int			CreateDivGraphFromBmp(				const BITMAPINFO *RGBBmpInfo, const void *RGBBmpImage, int AllNum, int XNum, int YNum, int   SizeX, int   SizeY,       int *HandleArray,        int TextureFlag = TRUE , int ReverseFlag = FALSE , const BITMAPINFO *AlphaBmpInfo = NULL , const void *AlphaBmpImage = NULL ) ;	// ビットマップデータから分割グラフィックハンドルを作成する
extern	int			CreateDivGraphFFromBmp(				const BITMAPINFO *RGBBmpInfo, const void *RGBBmpImage, int AllNum, int XNum, int YNum, float SizeX, float SizeY,       int *HandleArray,        int TextureFlag = TRUE , int ReverseFlag = FALSE , const BITMAPINFO *AlphaBmpInfo = NULL , const void *AlphaBmpImage = NULL ) ;	// ビットマップデータから分割グラフィックハンドルを作成する( float版 )
extern	int			ReCreateDivGraphFromBmp(			const BITMAPINFO *RGBBmpInfo, const void *RGBBmpImage, int AllNum, int XNum, int YNum, int   SizeX, int   SizeY, const int *HandleArray,        int TextureFlag = TRUE , int ReverseFlag = FALSE , const BITMAPINFO *AlphaBmpInfo = NULL , const void *AlphaBmpImage = NULL ) ;	// ビットマップデータから既存の分割グラフィックハンドルにデータを転送する
extern	int			ReCreateDivGraphFFromBmp(			const BITMAPINFO *RGBBmpInfo, const void *RGBBmpImage, int AllNum, int XNum, int YNum, float SizeX, float SizeY, const int *HandleArray,        int TextureFlag = TRUE , int ReverseFlag = FALSE , const BITMAPINFO *AlphaBmpInfo = NULL , const void *AlphaBmpImage = NULL ) ;	// ビットマップデータから既存の分割グラフィックハンドルにデータを転送する( float版 )
extern	int			CreateDXGraph(						const BASEIMAGE *RgbBaseImage, const BASEIMAGE *AlphaBaseImage,                                                                                 int TextureFlag ) ;																									// 基本イメージデータからサイズを割り出し、それに合ったグラフィックハンドルを作成する
extern	int			CreateGraphFromGraphImage(			const BASEIMAGE *RgbBaseImage,                                                                                                                  int TextureFlag = TRUE , int ReverseFlag = FALSE ) ;																// 基本イメージデータからグラフィックハンドルを作成する
extern	int			CreateGraphFromGraphImage(			const BASEIMAGE *RgbBaseImage, const BASEIMAGE *AlphaBaseImage,                                                                                 int TextureFlag = TRUE , int ReverseFlag = FALSE ) ;																// 基本イメージデータからグラフィックハンドルを作成する
extern	int			ReCreateGraphFromGraphImage(		const BASEIMAGE *RgbBaseImage,                                  int GrHandle,                                                                   int TextureFlag = TRUE , int ReverseFlag = FALSE ) ;																// 基本イメージデータから既存のグラフィックハンドルにデータを転送する
extern	int			ReCreateGraphFromGraphImage(		const BASEIMAGE *RgbBaseImage, const BASEIMAGE *AlphaBaseImage, int GrHandle,                                                                   int TextureFlag = TRUE , int ReverseFlag = FALSE ) ;																// 基本イメージデータから既存のグラフィックハンドルにデータを転送する
extern	int			CreateDivGraphFromGraphImage(		      BASEIMAGE *RgbBaseImage,                                  int AllNum, int XNum, int YNum, int   SizeX, int   SizeY,       int *HandleArray, int TextureFlag = TRUE , int ReverseFlag = FALSE ) ;																// 基本イメージデータから分割グラフィックハンドルを作成する
extern	int			CreateDivGraphFFromGraphImage(		      BASEIMAGE *RgbBaseImage,                                  int AllNum, int XNum, int YNum, float SizeX, float SizeY,       int *HandleArray, int TextureFlag = TRUE , int ReverseFlag = FALSE ) ;																// 基本イメージデータから分割グラフィックハンドルを作成する( float版 )
extern	int			CreateDivGraphFromGraphImage(		      BASEIMAGE *RgbBaseImage, const BASEIMAGE *AlphaBaseImage, int AllNum, int XNum, int YNum, int   SizeX, int   SizeY,       int *HandleArray, int TextureFlag = TRUE , int ReverseFlag = FALSE ) ;																// 基本イメージデータから分割グラフィックハンドルを作成する
extern	int			CreateDivGraphFFromGraphImage(		      BASEIMAGE *RgbBaseImage, const BASEIMAGE *AlphaBaseImage, int AllNum, int XNum, int YNum, float SizeX, float SizeY,       int *HandleArray, int TextureFlag = TRUE , int ReverseFlag = FALSE ) ;																// 基本イメージデータから分割グラフィックハンドルを作成する( float版 )
extern	int			ReCreateDivGraphFromGraphImage(		      BASEIMAGE *RgbBaseImage,                                  int AllNum, int XNum, int YNum, int   SizeX, int   SizeY, const int *HandleArray, int TextureFlag = TRUE , int ReverseFlag = FALSE ) ;																// 基本イメージデータから既存の分割グラフィックハンドルにデータを転送する
extern	int			ReCreateDivGraphFFromGraphImage(	      BASEIMAGE *RgbBaseImage,                                  int AllNum, int XNum, int YNum, float SizeX, float SizeY, const int *HandleArray, int TextureFlag = TRUE , int ReverseFlag = FALSE ) ;																// 基本イメージデータから既存の分割グラフィックハンドルにデータを転送する( float版 )
extern	int			ReCreateDivGraphFromGraphImage(		      BASEIMAGE *RgbBaseImage, const BASEIMAGE *AlphaBaseImage, int AllNum, int XNum, int YNum, int   SizeX, int   SizeY, const int *HandleArray, int TextureFlag = TRUE , int ReverseFlag = FALSE ) ;																// 基本イメージデータから既存の分割グラフィックハンドルにデータを転送する
extern	int			ReCreateDivGraphFFromGraphImage(	      BASEIMAGE *RgbBaseImage, const BASEIMAGE *AlphaBaseImage, int AllNum, int XNum, int YNum, float SizeX, float SizeY, const int *HandleArray, int TextureFlag = TRUE , int ReverseFlag = FALSE ) ;																// 基本イメージデータから既存の分割グラフィックハンドルにデータを転送する( float版 )
extern	int			CreateGraph(						int Width, int Height, int Pitch, const void *RGBImage, const void *AlphaImage = NULL , int GrHandle = -1 ) ;																																						// メモリ上のビットマップイメージからグラフィックハンドルを作成する
extern	int			CreateDivGraph(						int Width, int Height, int Pitch, const void *RGBImage, int AllNum, int XNum, int YNum, int   SizeX, int   SizeY, int *HandleArray, const void *AlphaImage = NULL ) ;																								// メモリ上のビットマップイメージから分割グラフィックハンドルを作成する
extern	int			CreateDivGraphF(					int Width, int Height, int Pitch, const void *RGBImage, int AllNum, int XNum, int YNum, float SizeX, float SizeY, int *HandleArray, const void *AlphaImage = NULL ) ;																								// メモリ上のビットマップイメージから分割グラフィックハンドルを作成する( float版 )
extern	int			ReCreateGraph(						int Width, int Height, int Pitch, const void *RGBImage, int GrHandle, const void *AlphaImage = NULL ) ;																																								// メモリ上のビットマップイメージからグラフィックハンドルを再作成する
#ifndef DX_NON_SOFTIMAGE
extern	int			CreateBlendGraphFromSoftImage(		int SIHandle ) ;																														// ソフトウエアで扱うイメージからブレンド用画像グラフィックハンドルを作成する( -1:エラー  -1以外:ブレンド用グラフィックハンドル )
extern	int			CreateGraphFromSoftImage(			int SIHandle ) ;																														// ソフトウエアで扱うイメージからグラフィックハンドルを作成する( -1:エラー  -1以外:グラフィックハンドル )
extern	int			CreateGraphFromRectSoftImage(		int SIHandle, int x, int y, int SizeX, int SizeY ) ;																					// ソフトウエアで扱うイメージの指定の領域を使ってグラフィックハンドルを作成する( -1:エラー  -1以外:グラフィックハンドル )
extern	int			ReCreateGraphFromSoftImage(			int SIHandle, int GrHandle ) ;																											// ソフトウエアで扱うイメージから既存のグラフィックハンドルに画像データを転送する
extern	int			ReCreateGraphFromRectSoftImage(		int SIHandle, int x, int y, int SizeX, int SizeY, int GrHandle ) ;																		// ソフトウエアで扱うイメージから既存のグラフィックハンドルに画像データを転送する
extern	int			CreateDivGraphFromSoftImage(		int SIHandle, int AllNum, int XNum, int YNum, int   SizeX, int   SizeY, int *HandleArray ) ;											// ソフトウエアで扱うイメージから分割グラフィックハンドルを作成する
extern	int			CreateDivGraphFFromSoftImage(		int SIHandle, int AllNum, int XNum, int YNum, float SizeX, float SizeY, int *HandleArray ) ;											// ソフトウエアで扱うイメージから分割グラフィックハンドルを作成する( float版 )
#endif // DX_NON_SOFTIMAGE
extern	int			CreateGraphFromBaseImage(			const BASEIMAGE *BaseImage ) ;																											// 基本イメージデータからグラフィックハンドルを作成する
extern	int			CreateGraphFromRectBaseImage(		const BASEIMAGE *BaseImage, int x, int y, int SizeX, int SizeY ) ;																		// 基本イメージデータの指定の領域を使ってグラフィックハンドルを作成する
extern	int			ReCreateGraphFromBaseImage(			const BASEIMAGE *BaseImage,                                     int GrHandle ) ;														// 基本イメージデータから既存のグラフィックハンドルに画像データを転送する
extern	int			ReCreateGraphFromRectBaseImage(		const BASEIMAGE *BaseImage, int x, int y, int SizeX, int SizeY, int GrHandle ) ;														// 基本イメージデータの指定の領域を使って既存のグラフィックハンドルに画像データを転送する
extern	int			CreateDivGraphFromBaseImage(		      BASEIMAGE *BaseImage, int AllNum, int XNum, int YNum, int   SizeX, int   SizeY, int *HandleArray ) ;								// 基本イメージデータから分割グラフィックハンドルを作成する
extern	int			CreateDivGraphFFromBaseImage(		      BASEIMAGE *BaseImage, int AllNum, int XNum, int YNum, float SizeX, float SizeY, int *HandleArray ) ;								// 基本イメージデータから分割グラフィックハンドルを作成する( float版 )
extern	int			ReloadGraph(						const TCHAR *FileName,                        int GrHandle, int ReverseFlag = FALSE ) ;																		// 画像ファイルからグラフィックハンドルへ画像データを転送する
extern	int			ReloadGraphWithStrLen(				const TCHAR *FileName, size_t FileNameLength, int GrHandle, int ReverseFlag = FALSE ) ;																		// 画像ファイルからグラフィックハンドルへ画像データを転送する
extern	int			ReloadDivGraph(						const TCHAR *FileName,                        int AllNum, int XNum, int YNum, int   XSize, int   YSize, const int *HandleArray, int ReverseFlag = FALSE ) ;	// 画像ファイルからグラフィックハンドルたちへ画像データを分割転送する
extern	int			ReloadDivGraphWithStrLen(			const TCHAR *FileName, size_t FileNameLength, int AllNum, int XNum, int YNum, int   XSize, int   YSize, const int *HandleArray, int ReverseFlag = FALSE ) ;	// 画像ファイルからグラフィックハンドルたちへ画像データを分割転送する
extern	int			ReloadDivGraphF(					const TCHAR *FileName,                        int AllNum, int XNum, int YNum, float XSize, float YSize, const int *HandleArray, int ReverseFlag = FALSE ) ;	// 画像ファイルからグラフィックハンドルたちへ画像データを分割転送する( float版 )
extern	int			ReloadDivGraphFWithStrLen(			const TCHAR *FileName, size_t FileNameLength, int AllNum, int XNum, int YNum, float XSize, float YSize, const int *HandleArray, int ReverseFlag = FALSE ) ;	// 画像ファイルからグラフィックハンドルたちへ画像データを分割転送する( float版 )
extern	int			ReloadReverseGraph(					const TCHAR *FileName,                        int GrHandle ) ;																			// ReloadGraph の画像反転処理追加版
extern	int			ReloadReverseGraphWithStrLen(		const TCHAR *FileName, size_t FileNameLength, int GrHandle ) ;																			// ReloadGraph の画像反転処理追加版
extern	int			ReloadReverseDivGraph(				const TCHAR *FileName,                        int AllNum, int XNum, int YNum, int   XSize, int   YSize, const int *HandleArray ) ;		// ReloadDivGraph の画像反転処理追加版
extern	int			ReloadReverseDivGraphWithStrLen(	const TCHAR *FileName, size_t FileNameLength, int AllNum, int XNum, int YNum, int   XSize, int   YSize, const int *HandleArray ) ;		// ReloadDivGraph の画像反転処理追加版
extern	int			ReloadReverseDivGraphF(				const TCHAR *FileName,                        int AllNum, int XNum, int YNum, float XSize, float YSize, const int *HandleArray ) ;		// ReloadDivGraph の画像反転処理追加版( float版 )
extern	int			ReloadReverseDivGraphFWithStrLen(	const TCHAR *FileName, size_t FileNameLength, int AllNum, int XNum, int YNum, float XSize, float YSize, const int *HandleArray ) ;		// ReloadDivGraph の画像反転処理追加版( float版 )

// グラフィックハンドル作成時設定係関数
extern	int			SetGraphColorBitDepth(						int ColorBitDepth ) ;							// SetCreateGraphColorBitDepth の旧名称
extern 	int			GetGraphColorBitDepth(						void ) ;										// GetCreateGraphColorBitDepth の旧名称
extern	int			SetCreateGraphColorBitDepth(				int BitDepth ) ;								// 作成するグラフィックハンドルの色深度を設定する
extern	int			GetCreateGraphColorBitDepth(				void ) ;										// 作成するグラフィックハンドルの色深度を取得する
extern	int			SetCreateGraphChannelBitDepth(				int BitDepth ) ;								// 作成するグラフィックハンドルの１チャンネル辺りのビット深度を設定する
extern	int			GetCreateGraphChannelBitDepth(				void ) ;										// 作成するグラフィックハンドルの１チャンネル辺りのビット深度を取得する
extern	int			SetDrawValidGraphCreateFlag(				int Flag ) ;									// SetDrawScreen に引数として渡せる( 描画対象として使用できる )グラフィックハンドルを作成するかどうかを設定する( TRUE:描画可能グラフィックハンドルを作成する  FLASE:通常のグラフィックハンドルを作成する( デフォルト ) )
extern	int			GetDrawValidGraphCreateFlag(				void ) ;										// SetDrawScreen に引数として渡せる( 描画対象として使用できる )グラフィックハンドルを作成するかどうかを設定を取得する
extern	int			SetDrawValidFlagOf3DGraph(					int Flag ) ;									// SetDrawValidGraphCreateFlag の旧名称
extern	int			SetLeftUpColorIsTransColorFlag(				int Flag ) ;									// 画像ファイルからグラフィックハンドルを作成する際に画像左上の色を透過色として扱うかどうかを設定する( TRUE:透過色として扱う 　FALSE:透過色として扱わない( デフォルト ) )
extern	int			SetUsePaletteGraphFlag(						int Flag ) ;									// 読み込む画像がパレット画像の場合、パレット画像として使用できる場合はパレット画像として使用するかどうかを設定する( TRUE:パレット画像として使用できる場合はパレット画像として使用する( デフォルト )  FALSE:パレット画像として使用できる場合もパレット画像としては使用しない( 通常タイプの画像に変換して使用する ) )
extern	int			SetUseBlendGraphCreateFlag(					int Flag ) ;									// ブレンド処理用画像を作成するかどうか( 要は画像の赤成分をα成分として扱うかどうか )の設定を行う( TRUE:ブレンド画像として読み込む  FALSE:通常画像として読み込む( デフォルト ) )
extern	int			GetUseBlendGraphCreateFlag(					void ) ;										// ブレンド処理用画像を作成するかどうか( 要は画像の赤成分をα成分として扱うかどうか )の設定を取得する
extern	int			SetUseAlphaTestGraphCreateFlag(				int Flag ) ;									// アルファテストを使用するグラフィックハンドルを作成するかどうかを設定する( TRUE:アルファテストを使用する( デフォルト )  FALSE:アルファテストを使用しない )
extern	int			GetUseAlphaTestGraphCreateFlag(				void ) ;										// アルファテストを使用するグラフィックハンドルを作成するかどうかを取得する
extern	int			SetUseAlphaTestFlag(						int Flag ) ;									// SetUseAlphaTestGraphCreateFlag の旧名称
extern	int			GetUseAlphaTestFlag(						void ) ;										// GetUseAlphaTestGraphCreateFlag の旧名称
extern	int			SetCubeMapTextureCreateFlag(				int Flag ) ;									// キューブマップテクスチャを作成するかどうかのフラグを設定する
extern	int			GetCubeMapTextureCreateFlag(				void ) ;										// キューブマップテクスチャを作成するかどうかのフラグを取得する
extern	int			SetUseNoBlendModeParam(						int Flag ) ;									// SetDrawBlendMode 関数の第一引数に DX_BLENDMODE_NOBLEND を代入した際に、デフォルトでは第二引数は内部で２５５を指定したことになるが、その自動２５５化をしないかどうかを設定する( TRUE:しない(第二引数の値が使用される)   FALSE:する(第二引数の値は無視されて 255 が常に使用される)(デフォルト) )αチャンネル付き画像に対して描画を行う場合のみ意味がある関数
extern	int			SetDrawValidAlphaChannelGraphCreateFlag(	int Flag ) ;									// SetDrawScreen の引数として渡せる( 描画対象として使用できる )αチャンネル付きグラフィックハンドルを作成するかどうかを設定する( SetDrawValidGraphCreateFlag 関数で描画対象として使用できるグラフィックハンドルを作成するように設定されていないと効果ありません )( TRUE:αチャンネル付き   FALSE:αチャンネルなし( デフォルト ) )
extern	int			GetDrawValidAlphaChannelGraphCreateFlag(	void ) ;										// SetDrawScreen の引数として渡せる( 描画対象として使用できる )αチャンネル付きグラフィックハンドルを作成するかどうかを取得する
extern	int			SetDrawValidFloatTypeGraphCreateFlag(		int Flag ) ;									// SetDrawScreen の引数として渡せる( 描画対象として使用できる )ピクセルフォーマットが浮動小数点型のグラフィックハンドルを作成するかどうかを設定する( SetDrawValidGraphCreateFlag 関数で描画対象として使用できるグラフィックハンドルを作成するように設定されていないと効果ありません )、グラフィックスデバイスが浮動小数点型のピクセルフォーマットに対応していない場合はグラフィックハンドルの作成に失敗する( TRUE:浮動小数点型　　FALSE:整数型( デフォルト ) )
extern	int			GetDrawValidFloatTypeGraphCreateFlag(		void ) ;										// SetDrawScreen の引数として渡せる( 描画対象として使用できる )ピクセルフォーマットが浮動小数点型のグラフィックハンドルを作成するかどうかを取得する
extern	int			SetDrawValidGraphCreateZBufferFlag(			int Flag ) ;									// SetDrawScreen の引数として渡せる( 描画対象として使用できる )グラフィックハンドルを作成する際に専用のＺバッファも作成するかどうかを設定する( TRUE:専用のＺバッファを作成する( デフォルト )  FALSE:専用のＺバッファは作成しない )
extern	int			GetDrawValidGraphCreateZBufferFlag(			void ) ;										// SetDrawScreen の引数として渡せる( 描画対象として使用できる )グラフィックハンドルを作成する際に専用のＺバッファも作成するかどうかを取得する
extern	int			SetCreateDrawValidGraphZBufferBitDepth(		int BitDepth ) ;								// SetDrawScreen の引数として渡せる( 描画対象として使用できる )グラフィックハンドルに適用するＺバッファのビット深度を設定する( BitDepth:ビット深度( 指定可能な値は 16, 24, 32 の何れか( SetDrawValidGraphCreateFlag 関数で描画対象として使用できるグラフィックハンドルを作成するように設定されていないと効果ありません )
extern	int			GetCreateDrawValidGraphZBufferBitDepth(		void ) ;										// SetDrawScreen の引数として渡せる( 描画対象として使用できる )グラフィックハンドルに適用するＺバッファのビット深度を取得する
extern	int			SetCreateDrawValidGraphMipLevels(			int MipLevels ) ;								// SetDrawScreen の引数として渡せる( 描画対象として使用できる )グラフィックハンドルに適用するMipMapのレベルを設定する
extern	int			GetCreateDrawValidGraphMipLevels(			void ) ;										// SetDrawScreen の引数として渡せる( 描画対象として使用できる )グラフィックハンドルに適用するMipMapのレベルを取得する
extern	int			SetCreateDrawValidGraphChannelNum(			int ChannelNum ) ;								// SetDrawScreen の引数として渡せる( 描画対象として使用できる )グラフィックハンドルに適用する色のチャンネル数を設定する( ChannelNum:チャンネル数( 指定可能な値は 1, 2, 4 の何れか( SetDrawValidGraphCreateFlag 関数で描画対象として使用できるグラフィックハンドルを作成するように設定されていないと効果ありません )
extern	int			GetCreateDrawValidGraphChannelNum(			void ) ;										// SetDrawScreen の引数として渡せる( 描画対象として使用できる )グラフィックハンドルに適用する色のチャンネル数を取得する
extern	int			SetCreateDrawValidGraphMultiSample(			int Samples, int Quality ) ;					// SetDrawScreen の引数として渡せる( 描画対象として使用できる )グラフィックハンドルに適用するマルチサンプリング( アンチエイリアシング )設定を行う( Samples:マルチサンプル処理に使用するドット数( 多いほど重くなります )  Quality:マルチサンプル処理の品質 )
extern	int			SetDrawValidMultiSample(					int Samples, int Quality ) ;					// SetCreateDrawValidGraphMultiSample の旧名称
extern	int			GetMultiSampleQuality(						int Samples ) ;									// 指定のマルチサンプル数で使用できる最大クオリティ値を取得する( 戻り値がマイナスの場合は引数のサンプル数が使用できないことを示します )
extern	int			SetUseTransColor(							int Flag ) ;									// 透過色機能を使用するかどうかを設定する( TRUE:使用する( デフォルト )  FALSE:使用しない )
extern	int			SetUseTransColorGraphCreateFlag(			int Flag ) ;									// 透過色機能を使用することを前提とした画像データの読み込み処理を行うかどうかを設定する( TRUE にすると SetDrawMode( DX_DRAWMODE_BILINEAR ); をした状態で DrawGraphF 等の浮動小数点型座標を受け取る関数で小数点以下の値を指定した場合に発生する描画結果の不自然を緩和する効果があります ( デフォルトは FALSE ) )
extern 	int			SetUseGraphAlphaChannel(					int Flag ) ;									// SetUseAlphaChannelGraphCreateFlag の旧名称
extern 	int			GetUseGraphAlphaChannel(					void ) ;										// GetUseAlphaChannelGraphCreateFlag の旧名称
extern 	int			SetUseAlphaChannelGraphCreateFlag(			int Flag ) ;									// αチャンネル付きグラフィックハンドルを作成するかどうかを設定する( TRUE:αチャンネル付き   FALSE:αチャンネル無し )
extern 	int			GetUseAlphaChannelGraphCreateFlag(			void ) ;										// αチャンネル付きグラフィックハンドルを作成するかどうかを取得する( TRUE:αチャンネル付き   FALSE:αチャンネル無し )
extern	int			SetUseNotManageTextureFlag(					int Flag ) ;									// Direct3D の管理テクスチャ機能を使用するグラフィックハンドルを作成するかどうかを設定する( TRUE:管理機能を使用する( デフォルト )  FALSE:管理機能を使用しない )、管理機能を使用するとグラフィックスデバイスのＶＲＡＭ容量以上の画像を扱うことができる代わりにシステムメモリの使用量が増えます
extern	int			GetUseNotManageTextureFlag(					void ) ;										// Direct3D の管理テクスチャ機能を使用するグラフィックハンドルを作成するかどうかを取得する
extern	int			SetUsePlatformTextureFormat(				int PlatformTextureFormat ) ;					// 作成するグラフィックハンドルで使用する環境依存のテクスチャフォーマットを指定する( Direct3D9環境なら DX_TEXTUREFORMAT_DIRECT3D9_R8G8B8 など、0 を渡すと解除 )
extern	int			GetUsePlatformTextureFormat(				void ) ;										// 作成するグラフィックハンドルで使用する環境依存のテクスチャフォーマットを取得する
extern	int			SetTransColor(								int Red, int Green, int Blue ) ;				// 作成するグラフィックハンドルに適用する透過色を設定する( Red,Green,Blue:透過色を光の３原色で表したもの( 各色０〜２５５ ) )
extern	int			GetTransColor(								int *Red, int *Green, int *Blue ) ;				// 作成するグラフィックハンドルに適用する透過色を取得する
extern	int			SetUseDivGraphFlag(							int Flag ) ;									// ２のｎ乗ではないサイズの画像を複数のテクスチャを使用してＶＲＡＭの無駄を省くかどうかを設定する( TRUE:複数のテクスチャを使用する   FALSE:なるべく一枚のテクスチャで済ます( デフォルト ) )、複数のテクスチャを使用する場合はＶＲＡＭ容量の節約ができる代わりに速度の低下やバイリニアフィルタリング描画時にテクスチャとテクスチャの境目が良く見るとわかる等の弊害があります
extern	int			SetUseAlphaImageLoadFlag(					int Flag ) ;									// LoadGraph などの際にファイル名の末尾に _a が付いたアルファチャンネル用の画像ファイルを追加で読み込む処理を行うかどうかを設定する( TRUE:行う( デフォルト )  FALSE:行わない )
extern	int			SetUseMaxTextureSize(						int Size ) ;									// 使用するテクスチャーの最大サイズを設定する( デフォルトではグラフィックスデバイスが対応している最大テクスチャーサイズ、引数に 0 を渡すとデフォルト設定になります )
extern	int			SetUseGraphBaseDataBackup(					int Flag ) ;									// グラフィックハンドルを作成する際に使用した画像データのバックアップをして Direct3DDevice のデバイスロスト時に使用するかどうかを設定する( TRUE:バックアップをする( デフォルト )  FALSE:バックアップをしない )、バックアップをしないとメモリの節約になりますが、復帰に掛かる時間が長くなり、メモリ上のファイルイメージからグラフィックハンドルを作成した場合は自動復帰ができないなどの弊害があります
extern	int			GetUseGraphBaseDataBackup(					void ) ;										// グラフィックハンドルを作成する際に使用した画像データのバックアップをして Direct3DDevice のデバイスロスト時に使用するかどうかを取得する
extern	int			SetUseSystemMemGraphCreateFlag(				int Flag ) ;									// ( 現在効果なし )グラフィックハンドルが持つ画像データをシステムメモリ上に作成するかどうかを設定する( TRUE:システムメモリ上に作成  FALSE:ＶＲＡＭ上に作成( デフォルト ) )
extern	int			GetUseSystemMemGraphCreateFlag(				void ) ;										// ( 現在効果なし )グラフィックハンドルが持つ画像データをシステムメモリ上に作成するかどうかを取得する

// 画像情報関係関数
extern	const unsigned int* GetFullColorImage(				int GrHandle ) ;																// 指定のグラフィックハンドルのＡＲＧＢ８イメージを取得する( 現在動画ファイルをグラフィックハンドルで読み込んだ場合のみ使用可能 )

extern	int			GraphLock(						int GrHandle, int *PitchBuf, void **DataPointBuf, COLORDATA **ColorDataPP = NULL, int WriteOnly = FALSE ) ;	// グラフィックメモリ領域のロック
extern	int			GraphUnLock(					int GrHandle ) ;																							// グラフィックメモリ領域のロック解除

extern	int			SetUseGraphZBuffer(				int GrHandle, int UseFlag, int BitDepth = -1 ) ;										// グラフィックハンドル専用のＺバッファを持つかどうかを設定する( GrHandle:対象となるグラフィックハンドル( 描画対象として使用可能なグラフィックハンドルのみ有効 )  UseFlag:専用のＺバッファを持つかどうか( TRUE:持つ( デフォルト )  FALSE:持たない )  BitDepth:ビット深度( 16 or 24 or 32 ) )
extern	int			CopyGraphZBufferImage(			int DestGrHandle, int SrcGrHandle ) ;													// グラフィックハンドルのＺバッファの状態を別のグラフィックハンドルのＺバッファにコピーする( DestGrHandle も SrcGrHandle もＺバッファを持っている描画対象にできるグラフィックハンドルで、サイズが同じであり、且つマルチサンプリング( アンチエイリアス )設定が無いことが条件 )

extern	int			SetDeviceLostDeleteGraphFlag(	int GrHandle, int DeleteFlag ) ;														// グラフィックスデバイスのデバイスロスト発生時に指定のグラフィックハンドルを削除するかどうかを設定する( TRUE:デバイスロスト時に削除する  FALSE:デバイスロストが発生しても削除しない )

extern	int			GetGraphSize(					int GrHandle, int   *SizeXBuf, int   *SizeYBuf ) ;										// グラフィックハンドルが持つ画像のサイズを得る
extern	int			GetGraphSizeF(					int GrHandle, float *SizeXBuf, float *SizeYBuf ) ;										// グラフィックハンドルが持つ画像のサイズを得る( float型 )
extern	int			GetGraphTextureSize(			int GrHandle, int   *SizeXBuf, int   *SizeYBuf ) ;										// グラフィックハンドルが持つ一つ目のテクスチャのサイズを得る
extern	int			GetGraphUseBaseGraphArea(		int GrHandle, int   *UseX,     int   *UseY,    int *UseSizeX, int *UseSizeY ) ;			// LoadDivGraph や DerivationGraph で元画像の一部分を使用している場合に、指定のグラフィックハンドルが使用している元画像の範囲を取得する
extern	int			GetGraphMipmapCount(			int GrHandle ) ;																		// グラフィックハンドルが持つテクスチャのミップマップレベル数を取得する
extern	int			GetGraphFilePath(				int GrHandle, TCHAR *FilePathBuffer ) ;													// グラフィックハンドルが画像ファイルから読み込まれていた場合、その画像のファイルパスを取得する
extern	int			CheckDrawValidGraph(			int GrHandle ) ;																		// 指定のグラフィックハンドルが描画対象にできる( SetDrawScreen の引数に渡せる )グラフィックハンドルかどうかを取得する( 戻り値　TRUE:描画対象にできるグラフィックハンドル　FALSE:描画対象にできないグラフィックハンドル )

extern	const COLORDATA* GetTexColorData(			int AlphaCh, int AlphaTest, int ColorBitDepth, int DrawValid = FALSE ) ;				// カラーデータを得る
extern	const COLORDATA* GetTexColorData(			const IMAGEFORMATDESC *Format ) ;														// フォーマットに基づいたカラーデータを得る
extern	const COLORDATA* GetTexColorData(			int FormatIndex ) ;																		// 指定のフォーマットインデックスのカラーデータを得る
extern	int			GetMaxGraphTextureSize(			int *SizeX, int *SizeY ) ;																// グラフィックスデバイスが対応している最大テクスチャサイズを取得する
extern	int			GetValidRestoreShredPoint(		void ) ;																				// グラフィックハンドルの画像を復元する関数が登録されているかどうかを取得する( TRUE:登録されている  FALSE:登録されていない )
extern	int			GetCreateGraphColorData(		COLORDATA *ColorData, IMAGEFORMATDESC *Format ) ;										// ( 現在効果なし )これから新たにグラフィックを作成する場合に使用するカラー情報を取得する

// 画像パレット操作関係関数( ソフトウエア画像のみ使用可能 )
extern	int			GetGraphPalette(				int GrHandle, int ColorIndex, int *Red, int *Green, int *Blue ) ;						// グラフィックハンドルのパレットを取得する( ソフトウエアレンダリングモードで、且つパレット画像の場合のみ使用可能 )
extern  int			GetGraphOriginalPalette(		int GrHandle, int ColorIndex, int *Red, int *Green, int *Blue ) ;						// グラフィックハンドルの SetGraphPalette で変更する前のパレットを取得する( ソフトウエアレンダリングモードで、且つパレット画像の場合のみ使用可能 )
extern	int			SetGraphPalette(				int GrHandle, int ColorIndex, unsigned int Color ) ;									// グラフィックハンドルのパレットを変更する( ソフトウエアレンダリングモードで、且つパレット画像の場合のみ使用可能 )
extern	int			ResetGraphPalette(				int GrHandle ) ;																		// SetGraphPalette で変更したパレットを全て元に戻す( ソフトウエアレンダリングモードで、且つパレット画像の場合のみ使用可能 )

// 図形描画関数
extern	int			DrawLine(         int   x1, int   y1, int   x2, int   y2,                                         unsigned int Color, int   Thickness = 1    ) ;							// 線を描画する
extern	int			DrawLineAA(       float x1, float y1, float x2, float y2,                                         unsigned int Color, float Thickness = 1.0f ) ;							// 線を描画する( アンチエイリアス付き )
extern	int			DrawBox(          int   x1, int   y1, int   x2, int   y2,                                         unsigned int Color, int FillFlag ) ;										// 四角形を描画する
extern	int			DrawBoxAA(        float x1, float y1, float x2, float y2,                                         unsigned int Color, int FillFlag, float LineThickness = 1.0f ) ;			// 四角形を描画する( アンチエイリアス付き )
extern	int			DrawFillBox(      int   x1, int   y1, int   x2, int   y2,                                         unsigned int Color ) ;													// 中身を塗りつぶす四角形を描画する
extern	int			DrawLineBox(      int   x1, int   y1, int   x2, int   y2,                                         unsigned int Color ) ;													// 枠だけの四角形の描画 する
extern	int			DrawCircle(       int   x,  int   y,  int   r,                                                    unsigned int Color, int FillFlag = TRUE, int   LineThickness = 1    ) ;	// 円を描画する
extern	int			DrawCircleAA(     float x,  float y,  float r,            int posnum,                             unsigned int Color, int FillFlag = TRUE, float LineThickness = 1.0f ) ;	// 円を描画する( アンチエイリアス付き )
extern	int			DrawOval(         int   x,  int   y,  int   rx, int   ry,                                         unsigned int Color, int FillFlag,        int   LineThickness = 1    ) ;	// 楕円を描画する
extern	int			DrawOvalAA(       float x,  float y,  float rx, float ry, int posnum,                             unsigned int Color, int FillFlag,        float LineThickness = 1.0f ) ;	// 楕円を描画する( アンチエイリアス付き )
extern	int			DrawOval_Rect(    int   x1, int   y1, int   x2, int   y2,                                         unsigned int Color, int FillFlag ) ;										// 指定の矩形に収まる円( 楕円 )を描画する
extern	int			DrawTriangle(     int   x1, int   y1, int   x2, int   y2, int   x3, int   y3,                     unsigned int Color, int FillFlag ) ;										// 三角形を描画する
extern	int			DrawTriangleAA(   float x1, float y1, float x2, float y2, float x3, float y3,                     unsigned int Color, int FillFlag, float LineThickness = 1.0f ) ;			// 三角形を描画する( アンチエイリアス付き )
extern	int			DrawQuadrangle(   int   x1, int   y1, int   x2, int   y2, int   x3, int   y3, int   x4, int   y4, unsigned int Color, int FillFlag ) ;										// 四角形を描画する
extern	int			DrawQuadrangleAA( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int Color, int FillFlag, float LineThickness = 1.0f ) ;			// 四角形を描画する( アンチエイリアス付き )
extern	int			DrawRoundRect(    int   x1, int   y1, int   x2, int   y2, int   rx, int   ry,                     unsigned int Color, int FillFlag ) ;										// 角の丸い四角形を描画する
extern	int			DrawRoundRectAA(  float x1, float y1, float x2, float y2, float rx, float ry, int posnum,         unsigned int Color, int FillFlag, float LineThickness = 1.0f ) ;			// 角の丸い四角形を描画する( アンチエイリアス付き )
extern	int			BeginAADraw(      void ) ;																																					// DrawTriangleAA などのアンチエイリアス付き図形描画の準備を行う
extern	int			EndAADraw(        void ) ;																																					// DrawTriangleAA などのアンチエイリアス付き図形描画の後始末を行う
extern 	int			DrawPixel(        int   x,  int   y,                                                              unsigned int Color ) ;													// 点を描画する

extern	int			Paint(			int x, int y, unsigned int FillColor, ULONGLONG BoundaryColor = ULL_PARAM( 0xffffffffffffffff ) ) ;								// 指定点から境界色があるところまで塗りつぶす(境界色を -1 にすると指定点の色の領域を塗りつぶす)

extern 	int			DrawPixelSet(   const POINTDATA *PointDataArray, int Num ) ;																					// 点の集合を描画する
extern	int			DrawLineSet(    const LINEDATA *LineDataArray,   int Num ) ;																					// 線の集合を描画する

extern	int			DrawPixel3D(     VECTOR   Pos,                                                                 unsigned int Color ) ;							// ３Ｄの点を描画する
extern	int			DrawPixel3DD(    VECTOR_D Pos,                                                                 unsigned int Color ) ;							// ３Ｄの点を描画する
extern	int			DrawLine3D(      VECTOR   Pos1,   VECTOR   Pos2,                                               unsigned int Color ) ;							// ３Ｄの線分を描画する
extern	int			DrawLine3DD(     VECTOR_D Pos1,   VECTOR_D Pos2,                                               unsigned int Color ) ;							// ３Ｄの線分を描画する
extern	int			DrawTriangle3D(  VECTOR   Pos1,   VECTOR   Pos2, VECTOR   Pos3,                                unsigned int Color, int FillFlag ) ;				// ３Ｄの三角形を描画する
extern	int			DrawTriangle3DD( VECTOR_D Pos1,   VECTOR_D Pos2, VECTOR_D Pos3,                                unsigned int Color, int FillFlag ) ;				// ３Ｄの三角形を描画する
extern	int			DrawCube3D(      VECTOR   Pos1,   VECTOR   Pos2,                            unsigned int DifColor, unsigned int SpcColor, int FillFlag ) ;		// ３Ｄの立方体を描画する
extern	int			DrawCube3DD(     VECTOR_D Pos1,   VECTOR_D Pos2,                            unsigned int DifColor, unsigned int SpcColor, int FillFlag ) ;		// ３Ｄの立方体を描画する
extern	int			DrawSphere3D(    VECTOR   CenterPos,                  float  r, int DivNum, unsigned int DifColor, unsigned int SpcColor, int FillFlag ) ;		// ３Ｄの球体を描画する
extern	int			DrawSphere3DD(   VECTOR_D CenterPos,                  double r, int DivNum, unsigned int DifColor, unsigned int SpcColor, int FillFlag ) ;		// ３Ｄの球体を描画する
extern	int			DrawCapsule3D(   VECTOR   Pos1,   VECTOR   Pos2,      float  r, int DivNum, unsigned int DifColor, unsigned int SpcColor, int FillFlag ) ;		// ３Ｄのカプセルを描画する
extern	int			DrawCapsule3DD(  VECTOR_D Pos1,   VECTOR_D Pos2,      double r, int DivNum, unsigned int DifColor, unsigned int SpcColor, int FillFlag ) ;		// ３Ｄのカプセルを描画する
extern	int			DrawCone3D(      VECTOR   TopPos, VECTOR   BottomPos, float  r, int DivNum, unsigned int DifColor, unsigned int SpcColor, int FillFlag ) ;		// ３Ｄの円錐を描画する
extern	int			DrawCone3DD(     VECTOR_D TopPos, VECTOR_D BottomPos, double r, int DivNum, unsigned int DifColor, unsigned int SpcColor, int FillFlag ) ;		// ３Ｄの円錐を描画する

// 画像描画関数
extern	int			LoadGraphScreen(           int x, int y, const TCHAR *GraphName,                         int TransFlag ) ;										// 画像ファイルを読みこんで画面に描画する
extern	int			LoadGraphScreenWithStrLen( int x, int y, const TCHAR *GraphName, size_t GraphNameLength, int TransFlag ) ;										// 画像ファイルを読みこんで画面に描画する

extern	int			DrawGraph(                int x, int y,                                                                 int GrHandle, int TransFlag ) ;															// 画像の等倍描画
extern	int			DrawExtendGraph(          int x1, int y1, int x2, int y2,                                               int GrHandle, int TransFlag ) ;															// 画像の拡大描画
extern	int			DrawRotaGraph(            int x, int y,                 double ExRate,                    double Angle, int GrHandle, int TransFlag, int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ;	// 画像の回転描画
extern	int			DrawRotaGraph2(           int x, int y, int cx, int cy, double ExtRate,                   double Angle, int GrHandle, int TransFlag, int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ;	// 画像の回転描画２( 回転中心指定付き )
extern	int			DrawRotaGraph3(           int x, int y, int cx, int cy, double ExtRateX, double ExtRateY, double Angle, int GrHandle, int TransFlag, int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ; 	// 画像の回転描画３( 回転中心指定付き＋縦横拡大率別指定版 )
extern	int			DrawRotaGraphFast(        int x, int y,                 float  ExRate,                    float  Angle, int GrHandle, int TransFlag, int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ;	// 画像の回転描画( 高速版、座標計算のアルゴリズムが簡略化されています、描画結果に不都合が無ければこちらの方が高速です )
extern	int			DrawRotaGraphFast2(       int x, int y, int cx, int cy, float  ExtRate,                   float  Angle, int GrHandle, int TransFlag, int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ;	// 画像の回転描画２( 回転中心指定付き )( 高速版、座標計算のアルゴリズムが簡略化されています、描画結果に不都合が無ければこちらの方が高速です )
extern	int			DrawRotaGraphFast3(       int x, int y, int cx, int cy, float  ExtRateX, float  ExtRateY, float  Angle, int GrHandle, int TransFlag, int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ; 	// 画像の回転描画３( 回転中心指定付き＋縦横拡大率別指定版 )( 高速版、座標計算のアルゴリズムが簡略化されています、描画結果に不都合が無ければこちらの方が高速です )
extern	int			DrawModiGraph(            int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4,               int GrHandle, int TransFlag ) ;															// 画像の自由変形描画
extern	int			DrawTurnGraph(            int x, int y,                                                                 int GrHandle, int TransFlag ) ;															// 画像の左右反転描画
extern	int			DrawReverseGraph(         int x, int y,                                                                 int GrHandle, int TransFlag, int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ;	// 画像の反転描画

extern	int			DrawGraphF(               float xf, float yf,                                                                       int GrHandle, int TransFlag ) ;															// 画像の描画( 座標指定が float 版 )
extern	int			DrawExtendGraphF(         float x1f, float y1f, float x2f, float y2,                                                int GrHandle, int TransFlag ) ;															// 画像の拡大描画( 座標指定が float 版 )
extern	int			DrawRotaGraphF(           float xf, float yf,                       double ExRate,                    double Angle, int GrHandle, int TransFlag, int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ;	// 画像の回転描画( 座標指定が float 版 )
extern	int			DrawRotaGraph2F(          float xf, float yf, float cxf, float cyf, double ExtRate,                   double Angle, int GrHandle, int TransFlag, int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ;	// 画像の回転描画２( 回転中心指定付き )( 座標指定が float 版 )
extern	int			DrawRotaGraph3F(          float xf, float yf, float cxf, float cyf, double ExtRateX, double ExtRateY, double Angle, int GrHandle, int TransFlag, int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ; 	// 画像の回転描画３( 回転中心指定付き＋縦横拡大率別指定版 )( 座標指定が float 版 )
extern	int			DrawRotaGraphFastF(       float xf, float yf,                       float  ExRate,                    float  Angle, int GrHandle, int TransFlag, int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ;	// 画像の回転描画( 座標指定が float 版 )( 高速版、座標計算のアルゴリズムが簡略化されています、描画結果に不都合が無ければこちらの方が高速です )
extern	int			DrawRotaGraphFast2F(      float xf, float yf, float cxf, float cyf, float  ExtRate,                   float  Angle, int GrHandle, int TransFlag, int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ;	// 画像の回転描画２( 回転中心指定付き )( 座標指定が float 版 )( 高速版、座標計算のアルゴリズムが簡略化されています、描画結果に不都合が無ければこちらの方が高速です )
extern	int			DrawRotaGraphFast3F(      float xf, float yf, float cxf, float cyf, float  ExtRateX, float  ExtRateY, float  Angle, int GrHandle, int TransFlag, int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ; 	// 画像の回転描画３( 回転中心指定付き＋縦横拡大率別指定版 )( 座標指定が float 版 )( 高速版、座標計算のアルゴリズムが簡略化されています、描画結果に不都合が無ければこちらの方が高速です )
extern	int			DrawModiGraphF(           float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4,           int GrHandle, int TransFlag ) ;															// 画像の自由変形描画( 座標指定が float 版 )
extern	int			DrawTurnGraphF(           float xf, float yf,                                                                       int GrHandle, int TransFlag ) ;															// 画像の左右反転描画( 座標指定が float 版 )
extern	int			DrawReverseGraphF(        float xf, float yf,                                                                       int GrHandle, int TransFlag, int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ;	// 画像の反転描画( 座標指定が float 版 )

extern	int			DrawChipMap(              int Sx, int Sy, int XNum, int YNum, const int *MapData, int ChipTypeNum, int MapDataPitch, const int *ChipGrHandle, int TransFlag ) ;																											// チップ画像を使った２Ｄマップ描画
extern	int			DrawChipMap(              int MapWidth, int MapHeight,        const int *MapData, int ChipTypeNum,                   const int *ChipGrHandle, int TransFlag, int MapDrawPointX, int MapDrawPointY, int MapDrawWidth, int MapDrawHeight, int ScreenX, int ScreenY ) ;	// チップ画像を使った２Ｄマップ描画
extern	int			DrawTile(                 int x1, int y1, int x2, int y2, int Tx, int Ty, double ExtRate, double Angle, int GrHandle, int TransFlag ) ;																																	// 画像を指定領域にタイル状に描画する

extern	int			DrawRectGraph(            int DestX,  int DestY,                          int SrcX, int SrcY, int    Width, int    Height,                         int GraphHandle, int TransFlag, int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ;				// 画像の指定矩形部分のみを等倍描画
extern	int			DrawRectExtendGraph(      int DestX1, int DestY1, int DestX2, int DestY2, int SrcX, int SrcY, int SrcWidth, int SrcHeight,                         int GraphHandle, int TransFlag ) ;																	// 画像の指定矩形部分のみを拡大描画
extern	int			DrawRectRotaGraph(        int x, int y, int SrcX, int SrcY, int Width, int Height, double ExtRate, double Angle,                                   int GraphHandle, int TransFlag, int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ;				// 画像の指定矩形部分のみを回転描画
extern	int			DrawRectRotaGraph2(       int x, int y, int SrcX, int SrcY, int Width, int Height, int cx, int cy, double ExtRate,  double Angle,                  int GraphHandle, int TransFlag, int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ;				// 画像の指定矩形部分のみを回転描画２( 回転中心指定付き )
extern	int			DrawRectRotaGraph3(       int x, int y, int SrcX, int SrcY, int Width, int Height, int cx, int cy, double ExtRateX, double ExtRateY, double Angle, int GraphHandle, int TransFlag, int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ;				// 画像の指定矩形部分のみを回転描画３( 回転中心指定付き＋縦横拡大率別指定版 )
extern	int			DrawRectRotaGraphFast(    int x, int y, int SrcX, int SrcY, int Width, int Height, float ExtRate, float Angle,                                     int GraphHandle, int TransFlag, int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ;				// 画像の指定矩形部分のみを回転描画( 高速版、座標計算のアルゴリズムが簡略化されています、描画結果に不都合が無ければこちらの方が高速です )
extern	int			DrawRectRotaGraphFast2(   int x, int y, int SrcX, int SrcY, int Width, int Height, int cx, int cy, float ExtRate,  float Angle,                    int GraphHandle, int TransFlag, int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ;				// 画像の指定矩形部分のみを回転描画２( 回転中心指定付き )( 高速版、座標計算のアルゴリズムが簡略化されています、描画結果に不都合が無ければこちらの方が高速です )
extern	int			DrawRectRotaGraphFast3(   int x, int y, int SrcX, int SrcY, int Width, int Height, int cx, int cy, float ExtRateX, float ExtRateY, float Angle,    int GraphHandle, int TransFlag, int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ;				// 画像の指定矩形部分のみを回転描画３( 回転中心指定付き＋縦横拡大率別指定版 )( 高速版、座標計算のアルゴリズムが簡略化されています、描画結果に不都合が無ければこちらの方が高速です )
extern	int			DrawRectModiGraph(        int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int SrcX, int SrcY, int Width, int Height,               int GraphHandle, int TransFlag ) ;																	// 画像の指定矩形部分のみを自由変形描画

extern	int			DrawRectGraphF(           float DestX,  float DestY,                              int SrcX, int SrcY, int    Width, int    Height,                           int GraphHandle, int TransFlag, int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ;	// 画像の指定矩形部分のみを等倍描画( 座標指定が float 版 )
extern	int			DrawRectExtendGraphF(     float DestX1, float DestY1, float DestX2, float DestY2, int SrcX, int SrcY, int SrcWidth, int SrcHeight,                           int GraphHandle, int TransFlag ) ;															// 画像の指定矩形部分のみを拡大描画( 座標指定が float 版 )
extern	int			DrawRectRotaGraphF(       float x, float y, int SrcX, int SrcY, int Width, int Height,                       double ExtRate,                   double Angle, int GraphHandle, int TransFlag, int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ;	// 画像の指定矩形部分のみを回転描画( 座標指定が float 版 )
extern	int			DrawRectRotaGraph2F(      float x, float y, int SrcX, int SrcY, int Width, int Height, float cxf, float cyf, double ExtRate,                   double Angle, int GraphHandle, int TransFlag, int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ;	// 画像の指定矩形部分のみを回転描画２( 回転中心指定付き )( 座標指定が float 版 )
extern	int			DrawRectRotaGraph3F(      float x, float y, int SrcX, int SrcY, int Width, int Height, float cxf, float cyf, double ExtRateX, double ExtRateY, double Angle, int GraphHandle, int TransFlag, int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ;	// 画像の指定矩形部分のみを回転描画３( 回転中心指定付き＋縦横拡大率別指定版 )( 座標指定が float 版 )
extern	int			DrawRectRotaGraphFastF(   float x, float y, int SrcX, int SrcY, int Width, int Height,                       float ExtRate,                    float  Angle, int GraphHandle, int TransFlag, int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ;	// 画像の指定矩形部分のみを回転描画( 座標指定が float 版 )( 高速版、座標計算のアルゴリズムが簡略化されています、描画結果に不都合が無ければこちらの方が高速です )
extern	int			DrawRectRotaGraphFast2F(  float x, float y, int SrcX, int SrcY, int Width, int Height, float cxf, float cyf, float ExtRate,                    float  Angle, int GraphHandle, int TransFlag, int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ;	// 画像の指定矩形部分のみを回転描画２( 回転中心指定付き )( 座標指定が float 版 )( 高速版、座標計算のアルゴリズムが簡略化されています、描画結果に不都合が無ければこちらの方が高速です )
extern	int			DrawRectRotaGraphFast3F(  float x, float y, int SrcX, int SrcY, int Width, int Height, float cxf, float cyf, float ExtRateX,  float  ExtRateY, float  Angle, int GraphHandle, int TransFlag, int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ;	// 画像の指定矩形部分のみを回転描画３( 回転中心指定付き＋縦横拡大率別指定版 )( 座標指定が float 版 )( 高速版、座標計算のアルゴリズムが簡略化されています、描画結果に不都合が無ければこちらの方が高速です )
extern	int			DrawRectModiGraphF(       float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, int SrcX, int SrcY, int Width, int Height,         int GraphHandle, int TransFlag ) ;															// 画像の指定矩形部分のみを自由変形描画( 座標指定が float 版 )

extern	int			DrawBlendGraph(           int x, int y, int GrHandle, int TransFlag,                 int BlendGraph, int BorderParam, int BorderRange ) ;									// ブレンド画像と合成して画像を等倍描画する
extern	int			DrawBlendGraphPos(        int x, int y, int GrHandle, int TransFlag, int bx, int by, int BlendGraph, int BorderParam, int BorderRange ) ;									// ブレンド画像と合成して画像を等倍描画する( ブレンド画像の起点座標を指定する引数付き )

extern	int			DrawCircleGauge(          int CenterX, int CenterY, double Percent, int GrHandle, double StartPercent = 0.0 ) ;																// 円グラフ的な描画を行う( GrHandle の画像の上下左右の端は透過色にしておく必要があります )

extern	int			DrawGraphToZBuffer(       int X, int Y,                                                                 int GrHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;														// Ｚバッファに対して画像の等倍描画
extern	int			DrawTurnGraphToZBuffer(   int x, int y,                                                                 int GrHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;														// Ｚバッファに対して画像の左右反転描画
extern	int			DrawReverseGraphToZBuffer( int x, int y,                                                                int GrHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ;	// Ｚバッファに対して画像の反転描画
extern	int			DrawExtendGraphToZBuffer( int x1, int y1, int x2, int y2,                                               int GrHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;														// Ｚバッファに対して画像の拡大描画
extern	int			DrawRotaGraphToZBuffer(   int x, int y, double ExRate, double Angle,                                    int GrHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ;	// Ｚバッファに対して画像の回転描画
extern	int			DrawRotaGraph2ToZBuffer(  int x, int y, int cx, int cy, double ExtRate,                   double Angle, int GrHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ;	// Ｚバッファに対して画像の回転描画２( 回転中心指定付き )
extern	int			DrawRotaGraph3ToZBuffer(  int x, int y, int cx, int cy, double ExtRateX, double ExtRateY, double Angle, int GrHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ; 	// Ｚバッファに対して画像の回転描画３( 回転中心指定付き＋縦横拡大率別指定版 )
extern	int			DrawRotaGraphFastToZBuffer(  int x, int y, float ExRate, float Angle,                                   int GrHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ;	// Ｚバッファに対して画像の回転描画( 高速版、座標計算のアルゴリズムが簡略化されています、描画結果に不都合が無ければこちらの方が高速です )
extern	int			DrawRotaGraphFast2ToZBuffer( int x, int y, int cx, int cy, float ExtRate,                  float Angle, int GrHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ;	// Ｚバッファに対して画像の回転描画２( 回転中心指定付き )( 高速版、座標計算のアルゴリズムが簡略化されています、描画結果に不都合が無ければこちらの方が高速です )
extern	int			DrawRotaGraphFast3ToZBuffer( int x, int y, int cx, int cy, float ExtRateX, float ExtRateY, float Angle, int GrHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ; 	// Ｚバッファに対して画像の回転描画３( 回転中心指定付き＋縦横拡大率別指定版 )( 高速版、座標計算のアルゴリズムが簡略化されています、描画結果に不都合が無ければこちらの方が高速です )
extern	int			DrawModiGraphToZBuffer(   int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4,               int GrHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;														// Ｚバッファに対して画像の自由変形描画
extern	int			DrawBoxToZBuffer(         int x1, int y1, int x2, int y2,                                               int FillFlag, int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;														// Ｚバッファに対して矩形の描画
extern	int			DrawCircleToZBuffer(      int x, int y, int r,                                                          int FillFlag, int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;														// Ｚバッファに対して円の描画
extern	int			DrawTriangleToZBuffer(    int x1, int y1, int x2, int y2, int x3, int y3,                               int FillFlag, int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;														// Ｚバッファに対して三角形を描画する
extern	int			DrawQuadrangleToZBuffer(  int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4,               int FillFlag, int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;														// Ｚバッファに対して四角形を描画する
extern	int			DrawRoundRectToZBuffer(   int x1, int y1, int x2, int y2, int rx, int ry,                               int FillFlag, int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;														// Ｚバッファに対して角の丸い四角形を描画する

extern	int			DrawPolygon(                             const VERTEX    *VertexArray, int PolygonNum,                                                                                                       int GrHandle, int TransFlag, int UVScaling = FALSE ) ;		// ２Ｄポリゴンを描画する( Vertex:三角形を形成する頂点配列の先頭アドレス( 頂点の数はポリゴンの数×３ )  PolygonNum:描画するポリゴンの数  GrHandle:使用するグラフィックハンドル  TransFlag:透過色処理を行うかどうか( TRUE:行う  FALSE:行わない )  UVScaling:基本FALSEでOK )
extern	int			DrawPolygon2D(                           const VERTEX2D  *VertexArray, int PolygonNum,                                                                                                       int GrHandle, int TransFlag ) ;							// ２Ｄポリゴンを描画する
extern	int			DrawPolygon3D(                           const VERTEX3D  *VertexArray, int PolygonNum,                                                                                                       int GrHandle, int TransFlag ) ;							// ３Ｄポリゴンを描画する
extern	int			DrawPolygonIndexed2D(                    const VERTEX2D  *VertexArray, int VertexNum, const unsigned short *IndexArray, int PolygonNum,                                                      int GrHandle, int TransFlag ) ;							// ２Ｄポリゴンを描画する( 頂点インデックスを使用 )
extern	int			DrawPolygonIndexed3D(                    const VERTEX3D  *VertexArray, int VertexNum, const unsigned short *IndexArray, int PolygonNum,                                                      int GrHandle, int TransFlag ) ;							// ３Ｄポリゴンを描画する( 頂点インデックスを使用 )
extern	int			DrawPolygonIndexed3DBase(                const VERTEX_3D *VertexArray, int VertexNum, const unsigned short *IndexArray, int IndexNum,   int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */, int GrHandle, int TransFlag ) ;							// ３Ｄポリゴンを描画する( 頂点インデックスを使用 )( 旧バージョン用 )
extern	int			DrawPolygon3DBase(                       const VERTEX_3D *VertexArray, int VertexNum,                                                   int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */, int GrHandle, int TransFlag ) ;							// ３Ｄポリゴンを描画する( 旧バージョン用 )
extern	int			DrawPolygon3D(                           const VERTEX_3D *VertexArray, int PolygonNum,                                                                                                       int GrHandle, int TransFlag ) ;							// ３Ｄポリゴンを描画する( 旧バージョン用 )

extern	int			DrawPolygonBase(                         const VERTEX    *VertexArray, int VertexNum,                                                   int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */, int GrHandle, int TransFlag, int UVScaling = FALSE ) ;		// ２Ｄプリミティブを描画する
extern	int			DrawPrimitive2D(                         const VERTEX2D  *VertexArray, int VertexNum,                                                   int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */, int GrHandle, int TransFlag ) ;							// ２Ｄプリミティブを描画する
extern	int			DrawPrimitive3D(                         const VERTEX3D  *VertexArray, int VertexNum,                                                   int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */, int GrHandle, int TransFlag ) ;							// ３Ｄプリミティブを描画する
extern	int			DrawPrimitiveIndexed2D(                  const VERTEX2D  *VertexArray, int VertexNum, const unsigned short *IndexArray, int IndexNum,   int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */, int GrHandle, int TransFlag ) ;							// ２Ｄプリミティブを描画する(頂点インデックス使用)
extern	int			DrawPrimitiveIndexed3D(                  const VERTEX3D  *VertexArray, int VertexNum, const unsigned short *IndexArray, int IndexNum,   int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */, int GrHandle, int TransFlag ) ;							// ３Ｄプリミティブを描画する(頂点インデックス使用)

extern	int			DrawPolygon3D_UseVertexBuffer(           int VertexBufHandle,                                                                                                                                                               int GrHandle, int TransFlag ) ;		// 頂点バッファを使用して３Ｄポリゴンを描画する
extern	int			DrawPrimitive3D_UseVertexBuffer(         int VertexBufHandle,                     int PrimitiveType  /* DX_PRIMTYPE_TRIANGLELIST 等 */,                                                                                     int GrHandle, int TransFlag ) ;		// 頂点バッファを使用して３Ｄプリミティブを描画する
extern	int			DrawPrimitive3D_UseVertexBuffer2(        int VertexBufHandle,                     int PrimitiveType  /* DX_PRIMTYPE_TRIANGLELIST 等 */,                 int StartVertex, int UseVertexNum,                                  int GrHandle, int TransFlag ) ;		// 頂点バッファを使用して３Ｄプリミティブを描画する
extern	int			DrawPolygonIndexed3D_UseVertexBuffer(    int VertexBufHandle, int IndexBufHandle,                                                                                                                                           int GrHandle, int TransFlag ) ;		// 頂点バッファとインデックスバッファを使用して３Ｄポリゴンを描画する
extern	int			DrawPrimitiveIndexed3D_UseVertexBuffer(  int VertexBufHandle, int IndexBufHandle, int PrimitiveType  /* DX_PRIMTYPE_TRIANGLELIST 等 */,                                                                                     int GrHandle, int TransFlag ) ;		// 頂点バッファとインデックスバッファを使用して３Ｄプリミティブを描画する
extern	int			DrawPrimitiveIndexed3D_UseVertexBuffer2( int VertexBufHandle, int IndexBufHandle, int PrimitiveType  /* DX_PRIMTYPE_TRIANGLELIST 等 */, int BaseVertex, int StartVertex, int UseVertexNum, int StartIndex, int UseIndexNum, int GrHandle, int TransFlag ) ;		// 頂点バッファとインデックスバッファを使用して３Ｄプリミティブを描画する

extern	int			DrawGraph3D(                             float x, float y, float z,                                                                     int GrHandle, int TransFlag ) ;																// 画像の３Ｄ描画
extern	int			DrawExtendGraph3D(                       float x, float y, float z, double ExRateX, double ExRateY,                                     int GrHandle, int TransFlag ) ;																// 画像の拡大３Ｄ描画
extern	int			DrawRotaGraph3D(                         float x, float y, float z, double ExRate, double Angle,                                        int GrHandle, int TransFlag, int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ;		// 画像の回転３Ｄ描画
extern	int			DrawRota2Graph3D(                        float x, float y, float z, float cx, float cy, double ExtRateX, double ExtRateY, double Angle, int GrHandle, int TransFlag, int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ;		// 画像の回転３Ｄ描画(回転中心指定型)
extern	int			DrawModiBillboard3D(                     VECTOR Pos, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4,    int GrHandle, int TransFlag ) ;																// 画像の自由変形３Ｄ描画
extern	int			DrawBillboard3D(                         VECTOR Pos, float cx, float cy, float Size, float Angle,                                       int GrHandle, int TransFlag, int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ;		// ３Ｄ空間上に画像を描画


// 描画設定関係関数
extern	int			SetDrawMode(						int DrawMode ) ;												// 描画モードを設定する
extern	int			GetDrawMode(						void ) ;														// 描画モードを取得する
extern	int			SetDrawBlendMode(					int BlendMode, int BlendParam ) ;								// 描画ブレンドモードを設定する
extern	int			GetDrawBlendMode(					int *BlendMode, int *BlendParam ) ;								// 描画ブレンドモードを取得する
extern	int			SetDrawAlphaTest(					int TestMode, int TestParam ) ;									// 描画時のアルファテストの設定を行う( TestMode:テストモード( DX_CMP_GREATER等 -1でデフォルト動作に戻す )  TestParam:描画アルファ値との比較に使用する値( 0〜255 ) )
extern	int			SetBlendGraph(						int BlendGraph, int BorderParam, int BorderRange ) ;			// ( SetBlendGraphParam の BlendType = DX_BLENDGRAPHTYPE_WIPE の処理を行う旧関数 )描画処理時に描画する画像とブレンドするαチャンネル付き画像をセットする( BlendGraph を -1 でブレンド機能を無効 )
extern	int			SetBlendGraphParam(					int BlendGraph, int BlendType, ... ) ;							// 描画処理時に描画する画像とブレンドする画像のブレンド設定を行う、BlendGraph を -1 にすれば設定を解除、その場合 BlendType とその後ろのパラメータは無視される
//		int			SetBlendGraphParam(					int BlendGraph, int BlendType = DX_BLENDGRAPHTYPE_NORMAL, int Ratio = ( 0( ブレンド率０％ )〜255( ブレンド率１００％ ) ) ) ;
//		int			SetBlendGraphParam(					int BlendGraph, int BlendType = DX_BLENDGRAPHTYPE_WIPE, int BorderParam = 境界位置(０〜２５５), int BorderRange = 境界幅(指定できる値は１、６４、１２８、２５５の４つ) ) ;
//		int			SetBlendGraphParam(					int BlendGraph, int BlendType = DX_BLENDGRAPHTYPE_ALPHA ) ;
extern	int			SetBlendGraphPosition(				int x, int y ) ;												// ブレンド画像の起点座標をセットする
extern	int			SetBlendGraphPositionMode(			int BlendGraphPositionMode /* DX_BLENDGRAPH_POSMODE_DRAWGRAPH など */ ) ;	// ブレンド画像の適応座標モードを設定する
extern	int			SetDrawBright(						int RedBright, int GreenBright, int BlueBright ) ;				// 描画輝度を設定する
extern	int			GetDrawBright(						int *Red, int *Green, int *Blue ) ;								// 描画輝度を取得する
extern	int			SetIgnoreDrawGraphColor(			int EnableFlag ) ;												// 描画する画像のＲＧＢ成分を無視するかどうかを指定する( EnableFlag:この機能を使うかどうか( TRUE:使う  FALSE:使わない( デフォルト ) ) )
extern	int			SetMaxAnisotropy(					int MaxAnisotropy ) ;											// 最大異方性値を設定する
extern	int			SetUseLarge3DPositionSupport(		int UseFlag ) ;													// ３Ｄ処理で使用する座標値が 10000000.0f などの大きな値になっても描画の崩れを小さく抑える処理を使用するかどうかを設定する、DxLib_Init の呼び出し前でのみ使用可能( TRUE:描画の崩れを抑える処理を使用する( CPU負荷が上がります )　　FALSE:描画の崩れを抑える処理は使用しない( デフォルト ) )

extern	int			SetUseZBufferFlag(					int Flag ) ;													// Ｚバッファを使用するかどうかを設定する( ２Ｄと３Ｄ描画に影響 )( TRUE:Ｚバッファを使用する  FALSE:Ｚバッファを使用しない( デフォルト ) )
extern	int			SetWriteZBufferFlag(				int Flag ) ;													// Ｚバッファに書き込みを行うかどうかを設定する( ２Ｄと３Ｄ描画に影響 )( TRUE:書き込みを行う  FALSE:書き込みを行わない( デフォルト ) )
extern	int			SetZBufferCmpType(					int CmpType /* DX_CMP_NEVER 等 */ ) ;							// ＺバッファのＺ値と書き込むＺ値との比較モードを設定する( ２Ｄと３Ｄ描画に影響 )( CmpType:DX_CMP_NEVER等( デフォルト:DX_CMP_LESSEQUAL ) )
extern	int			SetZBias(							int Bias ) ;													// 書き込むＺ値のバイアスを設定する( ２Ｄと３Ｄ描画に影響 )( Bias:バイアス値( デフォルト:0 ) )
extern	int			SetUseZBuffer3D(					int Flag ) ;													// Ｚバッファを使用するかどうかを設定する( ３Ｄ描画のみに影響 )( TRUE:Ｚバッファを使用する  FALSE:Ｚバッファを使用しない( デフォルト ) )
extern	int			SetWriteZBuffer3D(					int Flag ) ;													// Ｚバッファに書き込みを行うかどうかを設定する( ３Ｄ描画のみに影響 )( TRUE:書き込みを行う  FALSE:書き込みを行わない( デフォルト ) )
extern	int			SetZBufferCmpType3D(				int CmpType /* DX_CMP_NEVER 等 */ ) ;							// ＺバッファのＺ値と書き込むＺ値との比較モードを設定する( ３Ｄ描画のみに影響 )( CmpType:DX_CMP_NEVER等( デフォルト:DX_CMP_LESSEQUAL ) )
extern	int			SetZBias3D(							int Bias ) ;													// 書き込むＺ値のバイアスを設定する( ３Ｄ描画のみに影響 )( Bias:バイアス値( デフォルト:0 ) )
extern	int			SetDrawZ(							float Z ) ;														// ２Ｄ描画でＺバッファに書き込むＺ値を設定する( Z:書き込むＺ値( デフォルト:0.2f ) )

extern	int			SetDrawArea(						int x1, int y1, int x2, int y2 ) ;								// 描画可能領域の設定する
extern	int			GetDrawArea(						RECT *Rect ) ;													// 描画可能領域を取得する
extern	int			SetDrawAreaFull(					void ) ;														// 描画可能領域を描画対象画面全体にする
extern	int			SetDraw3DScale(						float Scale ) ;													// ３Ｄ描画の拡大率を設定する

extern	int			SetRestoreShredPoint(				void (* ShredPoint )( void ) ) ;								// SetRestoreGraphCallback の旧名
extern	int			SetRestoreGraphCallback(			void (* Callback )( void ) ) ;									// グラフィックハンドル復元関数を登録する
extern	int			RunRestoreShred(					void ) ;														// グラフィック復元関数を実行する
extern	int			SetGraphicsDeviceRestoreCallbackFunction( void (* Callback )( void *Data ), void *CallbackData ) ;	// グラフィックスデバイスがロストから復帰した際に呼ばれるコールバック関数を設定する
extern	int			SetGraphicsDeviceLostCallbackFunction(    void (* Callback )( void *Data ), void *CallbackData ) ;	// グラフィックスデバイスがロストから復帰する前に呼ばれるコールバック関数を設定する

extern	int			SetTransformTo2D(					const MATRIX   *Matrix ) ;										// ２Ｄ描画に使用される変換行列を設定する
extern	int			SetTransformTo2DD(					const MATRIX_D *Matrix ) ;										// ２Ｄ描画に使用される変換行列を設定する
extern	int			ResetTransformTo2D(					void ) ;														// ２Ｄ描画用に使用する変換行列の設定を初期状態に戻す
extern	int			SetTransformToWorld(				const MATRIX   *Matrix ) ;										// ローカル座標からワールド座標に変換するための行列を設定する
extern	int			SetTransformToWorldD(				const MATRIX_D *Matrix ) ;										// ローカル座標からワールド座標に変換するための行列を設定する
extern	int			GetTransformToWorldMatrix(			      MATRIX   *MatBuf ) ;										// ローカル座標からワールド座標に変換するための行列を取得する
extern	int			GetTransformToWorldMatrixD(			      MATRIX_D *MatBuf ) ;										// ローカル座標からワールド座標に変換するための行列を取得する
extern	int			SetTransformToView(					const MATRIX   *Matrix ) ;										// ワールド座標からビュー座標に変換するための行列を設定する
extern	int			SetTransformToViewD(				const MATRIX_D *Matrix ) ;										// ワールド座標からビュー座標に変換するための行列を設定する
extern	int			GetTransformToViewMatrix(			      MATRIX   *MatBuf ) ;										// ワールド座標からビュー座標に変換するための行列を取得する
extern	int			GetTransformToViewMatrixD(			      MATRIX_D *MatBuf ) ;										// ワールド座標からビュー座標に変換するための行列を取得する
extern	int			SetTransformToProjection(			const MATRIX   *Matrix ) ;										// ビュー座標からプロジェクション座標に変換するための行列を設定する
extern	int			SetTransformToProjectionD(			const MATRIX_D *Matrix ) ;										// ビュー座標からプロジェクション座標に変換するための行列を設定する
extern	int			GetTransformToProjectionMatrix(		      MATRIX   *MatBuf ) ;										// ビュー座標からプロジェクション座標に変換するための行列を取得する
extern	int			GetTransformToProjectionMatrixD(	      MATRIX_D *MatBuf ) ;										// ビュー座標からプロジェクション座標に変換するための行列を取得する
extern	int			SetTransformToViewport(				const MATRIX   *Matrix ) ;										// ビューポート行列を設定する
extern	int			SetTransformToViewportD(			const MATRIX_D *Matrix ) ;										// ビューポート行列を設定する
extern	int			GetTransformToViewportMatrix(	    MATRIX   *MatBuf ) ;											// ビューポート行列を取得する
extern	int			GetTransformToViewportMatrixD(	    MATRIX_D *MatBuf ) ;											// ビューポート行列を取得する
extern	int			GetTransformToAPIViewportMatrix(    MATRIX   *MatBuf ) ;											// Direct3Dで自動適用されるビューポート行列を取得する
extern	int			GetTransformToAPIViewportMatrixD(   MATRIX_D *MatBuf ) ;											// Direct3Dで自動適用されるビューポート行列を取得する
extern	int			SetDefTransformMatrix(				void ) ;														// デフォルトの変換行列を設定する
extern	int			GetTransformPosition(				VECTOR   *LocalPos, float  *x, float  *y ) ;					// ローカル座標からスクリーン座標を取得する
extern	int			GetTransformPositionD(				VECTOR_D *LocalPos, double *x, double *y ) ;					// ローカル座標からスクリーン座標を取得する
extern	float		GetBillboardPixelSize(				VECTOR   WorldPos, float  WorldSize ) ;							// ワールド空間上のビルボードのサイズからスクリーンに投影した場合のピクセル単位のサイズを取得する
extern	double		GetBillboardPixelSizeD(				VECTOR_D WorldPos, double WorldSize ) ;							// ワールド空間上のビルボードのサイズからスクリーンに投影した場合のピクセル単位のサイズを取得する
extern	VECTOR		ConvWorldPosToViewPos(				VECTOR   WorldPos ) ;											// ワールド座標をビュー座標に変換する
extern	VECTOR_D	ConvWorldPosToViewPosD(				VECTOR_D WorldPos ) ;											// ワールド座標をビュー座標に変換する
extern	VECTOR		ConvWorldPosToScreenPos(			VECTOR   WorldPos ) ;											// ワールド座標をスクリーン座標に変換する
extern	VECTOR_D	ConvWorldPosToScreenPosD(			VECTOR_D WorldPos ) ;											// ワールド座標をスクリーン座標に変換する
extern	FLOAT4		ConvWorldPosToScreenPosPlusW(		VECTOR   WorldPos ) ;											// ワールド座標をスクリーン座標に変換する、最後のＸＹＺ座標をＷで割る前の値を得る
extern	DOUBLE4		ConvWorldPosToScreenPosPlusWD(		VECTOR_D WorldPos ) ;											// ワールド座標をスクリーン座標に変換する、最後のＸＹＺ座標をＷで割る前の値を得る
extern	VECTOR		ConvScreenPosToWorldPos(			VECTOR   ScreenPos ) ;											// スクリーン座標をワールド座標に変換する
extern	VECTOR_D	ConvScreenPosToWorldPosD(			VECTOR_D ScreenPos ) ;											// スクリーン座標をワールド座標に変換する
extern	VECTOR		ConvScreenPosToWorldPos_ZLinear(	VECTOR   ScreenPos ) ;											// スクリーン座標をワールド座標に変換する( Z座標が線形 )
extern	VECTOR_D	ConvScreenPosToWorldPos_ZLinearD(	VECTOR_D ScreenPos ) ;											// スクリーン座標をワールド座標に変換する( Z座標が線形 )

extern	int			SetUseCullingFlag(					int Flag ) ;													// SetUseBackCulling の旧名称
extern	int			SetUseBackCulling(					int Flag /* DX_CULLING_LEFT 等 */ ) ;							// ポリゴンカリングモードを設定する
extern	int			GetUseBackCulling(					void ) ;														// ポリゴンカリングモードを取得する

extern	int			SetTextureAddressMode(				int Mode /* DX_TEXADDRESS_WRAP 等 */ , int Stage = -1 ) ;		// テクスチャアドレスモードを設定する
extern	int			SetTextureAddressModeUV(			int ModeU, int ModeV, int Stage = -1 ) ;						// テクスチャアドレスモードを設定する( U と V を別々に設定する )
extern	int			SetTextureAddressTransform(			float TransU, float TransV, float ScaleU, float ScaleV, float RotCenterU, float RotCenterV, float Rotate ) ;	// テクスチャ座標変換パラメータを設定する
extern	int			SetTextureAddressTransformMatrix(	MATRIX Matrix ) ;												// テクスチャ座標変換行列を設定する
extern	int			ResetTextureAddressTransform(		void ) ;														// テクスチャ座標変換設定をリセットする

extern	int			SetFogEnable(						int Flag ) ;													// フォグを有効にするかどうかを設定する( TRUE:有効  FALSE:無効 )
extern	int			GetFogEnable(						void ) ;														// フォグが有効かどうかを取得する( TRUE:有効  FALSE:無効 )
extern	int			SetFogMode(							int Mode /* DX_FOGMODE_NONE 等 */ ) ;							// フォグモードを設定する
extern	int			GetFogMode(							void ) ;														// フォグモードを取得する
extern	int			SetFogColor(						int  r, int  g, int  b ) ;										// フォグカラーを設定する
extern	int			GetFogColor(						int *r, int *g, int *b ) ;										// フォグカラーを取得する
extern	int			SetFogStartEnd(						float  start, float  end ) ;									// フォグが始まる距離と終了する距離を設定する( 0.0f 〜 1.0f )
extern	int			GetFogStartEnd(						float *start, float *end ) ;									// フォグが始まる距離と終了する距離を取得する( 0.0f 〜 1.0f )
extern	int			SetFogDensity(						float density ) ;												// フォグの密度を設定する( 0.0f 〜 1.0f )
extern	float		GetFogDensity(						void ) ;														// フォグの密度を取得する( 0.0f 〜 1.0f )


// 画面関係関数
extern	unsigned int	GetPixel(									int x, int y ) ;																// 指定座標の色を取得する
extern	COLOR_F			GetPixelF(									int x, int y ) ;																// 指定座標の色を取得する( float型 )
extern	int				SetBackgroundColor(							int Red,  int  Green, int  Blue ) ;												// メインウインドウの背景色を設定する( Red,Green,Blue:それぞれ ０〜２５５ )
extern	int				GetBackgroundColor(							int *Red, int *Green, int *Blue ) ;												// メインウインドウの背景色を取得する( Red,Green,Blue:それぞれ ０〜２５５ )
extern	int				GetDrawScreenGraph(							                             int x1, int y1, int x2, int y2,                       int GrHandle, int UseClientFlag = TRUE ) ;	// 描画先の画面から指定領域の画像情報をグラフィックハンドルに転送する
extern	int				BltDrawValidGraph(							int TargetDrawValidGrHandle, int x1, int y1, int x2, int y2, int DestX, int DestY, int DestGrHandle ) ;							// SetDrawScreen で描画対象にできるグラフィックハンドルから指定領域の画像情報を別のグラフィックハンドルに転送する
extern	int				ScreenFlip(									void ) ;																		// 裏画面と表画面の内容を交換する
extern 	int				ScreenCopy(									void ) ;																		// 裏画面の内容を表画面に転送する
extern	int				WaitVSync(									int SyncNum ) ;																	// 垂直同期信号を待つ
extern	int				ClearDrawScreen(					        const RECT *ClearRect = NULL ) ;												// 画面をクリアする
extern	int				ClearDrawScreenZBuffer(						const RECT *ClearRect = NULL ) ;												// 画面のＺバッファをクリアする
extern	int				ClsDrawScreen(								void ) ;																		// ClearDrawScreenの旧名称
extern	int				SetDrawScreen(								int DrawScreen ) ;																// 描画先画面を設定する( MakeScreen で作成したグラフィックハンドルも渡すことができます )
extern	int				GetDrawScreen(								void ) ;																		// 描画先画面を取得する
extern	int				GetActiveGraph(								void ) ;																		// GetDrawScreen の旧名称
extern	int				SetUseSetDrawScreenSettingReset(			int UseFlag ) ;																	// SetDrawScreen を実行した際にカメラや描画範囲の設定をリセットするかを設定する( UseFlag  TRUE:リセットする( デフォルト )  FALSE:リセットしない )
extern	int				GetUseSetDrawScreenSettingReset(			void ) ;																		// SetDrawScreen を実行した際にカメラや描画範囲の設定をリセットするかを取得する
extern	int				SetDrawZBuffer(								int DrawScreen ) ;																// 描画先Ｚバッファのセット( DrawScreen 付属のＺバッファを描画先Ｚバッファにする、DrawScreen を -1 にするとデフォルトの描画先Ｚバッファに戻る )
extern	int				SetGraphMode(								int ScreenSizeX, int ScreenSizeY, int ColorBitDepth, int RefreshRate = 60 ) ;	// 画面モードを設定する
extern	int				SetFullScreenResolutionMode(				int ResolutionMode /* DX_FSRESOLUTIONMODE_NATIVE 等 */ ) ;						// フルスクリーン解像度モードを設定する
extern	int				GetFullScreenResolutionMode(				int *ResolutionMode, int *UseResolutionMode ) ;									// フルスクリーン解像度モードを取得する( UseResolutionMode は実際に使用されている解像度モード( 例えば DX_FSRESOLUTIONMODE_NATIVE を指定していてもモニタが指定の解像度に対応していない場合は UseResolutionMode が DX_FSRESOLUTIONMODE_DESKTOP や DX_FSRESOLUTIONMODE_MAXIMUM になります ) )
extern	int				SetFullScreenScalingMode(					int ScalingMode /* DX_FSSCALINGMODE_NEAREST 等 */ , int FitScaling = FALSE ) ;	// フルスクリーンモード時の画面拡大モードを設定する
extern	int				SetEmulation320x240(						int Flag ) ;																	// ６４０ｘ４８０の画面で３２０ｘ２４０の画面解像度にするかどうかを設定する、６４０ｘ４８０以外の解像度では無効( TRUE:有効  FALSE:無効 )
extern	int				SetZBufferSize(								int ZBufferSizeX, int ZBufferSizeY ) ;											// 画面用のＺバッファのサイズを設定する
extern	int				SetZBufferBitDepth(							int BitDepth ) ;																// 画面用のＺバッファのビット深度を設定する( 16 or 24 or 32 )
extern	int				SetWaitVSyncFlag(							int Flag ) ;																	// ScreenFlip 実行時にＶＳＹＮＣ待ちをするかどうかを設定する
extern	int				GetWaitVSyncFlag(							void ) ;																		// ScreenFlip 実行時にＶＳＹＮＣ待ちをするかどうかを取得する
extern	int				SetFullSceneAntiAliasingMode(				int Samples, int Quality ) ;													// 画面のフルスクリーンアンチエイリアスモードの設定を行う( DxLib_Init の前でのみ使用可能 )
extern	int				SetGraphDisplayArea(						int x1, int y1, int x2, int y2 ) ;												// ScreenFlip 時に表画面全体に転送する裏画面の領域を設定する( DxLib_Init の前でのみ使用可能 )
extern	int				SetChangeScreenModeGraphicsSystemResetFlag(	int Flag ) ;																	// 画面モード変更時( とウインドウモード変更時 )にグラフィックスシステムの設定やグラフィックハンドルをリセットするかどうかを設定する( TRUE:リセットする( デフォルト )  FALSE:リセットしない )
extern	int				GetScreenState(								int *SizeX, int *SizeY, int *ColorBitDepth ) ;									// 現在の画面の解像度とカラービット数を得る 
extern	int				GetDrawScreenSize(							int *XBuf, int *YBuf ) ;														// 描画先のサイズを取得する
extern	int				GetScreenBitDepth(							void ) ;																		// 画面のカラービット数を取得する
extern	int				GetColorBitDepth(							void ) ;																		// GetScreenBitDepth の旧名称
extern	int				GetChangeDisplayFlag(						void ) ;																		// 画面モードが変更されているかどうかを取得する
extern	int				GetVideoMemorySize(							int *AllSize, int *FreeSize ) ;													// ( 現在正常に動作しません )ビデオメモリの容量を得る
extern	int				GetRefreshRate(								void ) ;																		// 現在の画面のリフレッシュレートを取得する
extern	int				GetDisplayNum(								void ) ;																		// ディスプレイの数を取得
extern	int				GetDisplayInfo(								int DisplayIndex, int *DesktopRectX, int *DesktopRectY, int *DesktopSizeX, int *DesktopSizeY, int *IsPrimary ) ;	// ディスプレイのデスクトップ上での矩形位置を取得する
extern	int				GetDisplayModeNum(							int DisplayIndex = 0 ) ;														// 変更可能なディスプレイモードの数を取得する
extern	DISPLAYMODEDATA	GetDisplayMode(								int ModeIndex, int DisplayIndex = 0 ) ;											// 変更可能なディスプレイモードの情報を取得する( ModeIndex は 0 〜 GetDisplayModeNum の戻り値-1 )
extern	int				GetDisplayMaxResolution(					int *SizeX, int *SizeY, int DisplayIndex = 0 ) ;								// ディスプレイの最大解像度を取得する
extern	const COLORDATA* GetDispColorData(							void ) ;																		// ディスプレイのカラーデータアドレスを取得する
extern	int				GetMultiDrawScreenNum(						void ) ;																		// 同時に描画を行うことができる画面の数を取得する
extern	int				GetDrawFloatCoordType(						void ) ;																		// DrawGraphF 等の浮動小数点値で座標を指定する関数における座標タイプを取得する( 戻り値 : DX_DRAWFLOATCOORDTYPE_DIRECT3D9 など )

// その他設定関係関数
extern	int			SetUseNormalDrawShader(						int Flag ) ;									// 通常描画にプログラマブルシェーダーを使用するかどうかを設定する( TRUE:使用する( デフォルト )  FALSE:使用しない )
extern	int			SetUseSoftwareRenderModeFlag(				int Flag ) ;									// ソフトウエアレンダリングモードを使用するかどうかを設定する( TRUE:使用する  FALSE:使用しない( デフォルト ) )( DxLib_Init の前に呼ぶ必要があります )
extern	int			SetNotUse3DFlag(							int Flag ) ;									// ( 同効果のSetUseSoftwareRenderModeFlag を使用して下さい )３Ｄ機能を使わないかどうかを設定する
extern	int			SetUse3DFlag(								int Flag ) ;									// ( 同効果のSetUseSoftwareRenderModeFlag を使用して下さい )３Ｄ機能を使うかどうかを設定する
extern	int			GetUse3DFlag(								void ) ;										// 描画に３Ｄ機能を使うかどうかを取得する
extern	int			SetScreenMemToVramFlag(						int Flag ) ;									// ( 同効果のSetUseSoftwareRenderModeFlag を使用して下さい )画面のピクセルデータをＶＲＡＭに置くかどうかを設定する
extern	int			GetScreenMemToSystemMemFlag(				void ) ;										// 画面のピクセルデータがシステムメモリ上に存在するかを取得する

extern	int			SetWindowDrawRect(							const RECT *DrawRect ) ;						// 通常使用しない
extern	int			RestoreGraphSystem(							void ) ;										// ＤＸライブラリのグラフィックス処理関連の復帰処理を行う
extern	int			SetUseHardwareVertexProcessing(				int Flag ) ;									// ハードウエアの頂点演算処理機能を使用するかどうかを設定する( TRUE:使用する( デフォルト )  FALSE:使用しない )( DxLib_Init の前に呼ぶ必要があります )
extern	int			SetUsePixelLighting(						int Flag ) ;									// ピクセル単位でライティングを行うかどうかを設定する、要 ShaderModel 3.0( TRUE:ピクセル単位のライティングを行う  FALSE:頂点単位のライティングを行う( デフォルト ) )
extern	int			SetUseOldDrawModiGraphCodeFlag(				int Flag ) ;									// 古いバージョンの DrawModiGraph 関数のコードを使用するかどうかを設定する
extern	int			SetUseVramFlag(								int Flag ) ;									// ( 現在効果なし )ＶＲＡＭを使用するかのフラグをセットする
extern	int			GetUseVramFlag(								void ) ;										// ( 現在効果なし )２Ｄグラフィックサーフェス作成時にシステムメモリーを使用するかのフラグ取得
extern	int			SetBasicBlendFlag(							int Flag ) ;									// ( 現在効果なし )簡略化ブレンド処理を行うか否かのフラグをセットする
extern	int			SetUseBasicGraphDraw3DDeviceMethodFlag(		int Flag ) ;									// ( 現在効果なし )単純図形の描画に３Ｄデバイスの機能を使用するかどうかを設定する
extern	int			SetUseDisplayIndex(							int Index ) ;									// ＤＸライブラリのウインドウを表示するディスプレイデバイスを設定する( -1 を指定するとマウスカーソルがあるディスプレイデバイスにＤＸライブラリのウインドウを表示する )
extern	int			RenderVertex(								void ) ;										// 頂点バッファに溜まった頂点データを描画する( 特殊用途 )

// 描画パフォーマンス関係関数
extern	int			GetDrawCallCount(							void ) ;										// 前々回の ScreenFlip 呼び出しから、前回の ScreenFlip 呼び出しまでの間に行われた描画コールの回数を取得する
extern	float		GetFPS(										void ) ;										// フレームレート( １秒間に呼ばれる ScreenFlip の回数 )を取得する

#ifndef DX_NON_SAVEFUNCTION

// 描画先画面保存関数
// Jpeg_Quality         = 0:低画質〜100:高画質
// Png_CompressionLevel = 0:無圧縮〜  9:最高圧縮
extern	int			SaveDrawScreen(                 int x1, int y1, int x2, int y2, const TCHAR *FileName,                        int SaveType = DX_IMAGESAVETYPE_BMP , int Jpeg_Quality = 80 , int Jpeg_Sample2x1 = TRUE , int Png_CompressionLevel = -1 ) ;		// 現在描画対象になっている画面をファイルで保存する
extern	int			SaveDrawScreenWithStrLen(       int x1, int y1, int x2, int y2, const TCHAR *FileName, size_t FileNameLength, int SaveType = DX_IMAGESAVETYPE_BMP , int Jpeg_Quality = 80 , int Jpeg_Sample2x1 = TRUE , int Png_CompressionLevel = -1 ) ;		// 現在描画対象になっている画面をファイルで保存する
extern	int			SaveDrawScreenToBMP(            int x1, int y1, int x2, int y2, const TCHAR *FileName                        ) ;																																// 現在描画対象になっている画面をＢＭＰ形式で保存する
extern	int			SaveDrawScreenToBMPWithStrLen(  int x1, int y1, int x2, int y2, const TCHAR *FileName, size_t FileNameLength ) ;																																// 現在描画対象になっている画面をＢＭＰ形式で保存する
extern	int			SaveDrawScreenToDDS(            int x1, int y1, int x2, int y2, const TCHAR *FileName                        ) ;																																// 現在描画対象になっている画面をＤＤＳ形式で保存する
extern	int			SaveDrawScreenToDDSWithStrLen(  int x1, int y1, int x2, int y2, const TCHAR *FileName, size_t FileNameLength ) ;																																// 現在描画対象になっている画面をＤＤＳ形式で保存する
extern	int			SaveDrawScreenToJPEG(           int x1, int y1, int x2, int y2, const TCHAR *FileName,                        int Quality = 80 , int Sample2x1 = TRUE ) ;																						// 現在描画対象になっている画面をＪＰＥＧ形式で保存する Quality = 画質、値が大きいほど低圧縮高画質,0〜100 
extern	int			SaveDrawScreenToJPEGWithStrLen( int x1, int y1, int x2, int y2, const TCHAR *FileName, size_t FileNameLength, int Quality = 80 , int Sample2x1 = TRUE ) ;																						// 現在描画対象になっている画面をＪＰＥＧ形式で保存する Quality = 画質、値が大きいほど低圧縮高画質,0〜100 
extern	int			SaveDrawScreenToPNG(            int x1, int y1, int x2, int y2, const TCHAR *FileName,                        int CompressionLevel = -1 ) ;																										// 現在描画対象になっている画面をＰＮＧ形式で保存する CompressionLevel = 圧縮率、値が大きいほど高圧縮率高負荷、０は無圧縮,0〜9
extern	int			SaveDrawScreenToPNGWithStrLen(  int x1, int y1, int x2, int y2, const TCHAR *FileName, size_t FileNameLength, int CompressionLevel = -1 ) ;																										// 現在描画対象になっている画面をＰＮＧ形式で保存する CompressionLevel = 圧縮率、値が大きいほど高圧縮率高負荷、０は無圧縮,0〜9

// 描画対象にできるグラフィックハンドル保存関数
// Jpeg_Quality         = 0:低画質〜100:高画質
// Png_CompressionLevel = 0:無圧縮〜  9:最高圧縮
extern	int			SaveDrawValidGraph(                 int GrHandle, int x1, int y1, int x2, int y2, const TCHAR *FileName,                        int SaveType = DX_IMAGESAVETYPE_BMP , int Jpeg_Quality = 80 , int Jpeg_Sample2x1 = TRUE , int Png_CompressionLevel = -1 ) ;		// 描画対象にできるグラフィックハンドルをファイルで保存する
extern	int			SaveDrawValidGraphWithStrLen(       int GrHandle, int x1, int y1, int x2, int y2, const TCHAR *FileName, size_t FileNameLength, int SaveType = DX_IMAGESAVETYPE_BMP , int Jpeg_Quality = 80 , int Jpeg_Sample2x1 = TRUE , int Png_CompressionLevel = -1 ) ;		// 描画対象にできるグラフィックハンドルをファイルで保存する
extern	int			SaveDrawValidGraphToBMP(            int GrHandle, int x1, int y1, int x2, int y2, const TCHAR *FileName                        ) ;																																// 描画対象にできるグラフィックハンドルをＢＭＰ形式で保存する
extern	int			SaveDrawValidGraphToBMPWithStrLen(  int GrHandle, int x1, int y1, int x2, int y2, const TCHAR *FileName, size_t FileNameLength ) ;																																// 描画対象にできるグラフィックハンドルをＢＭＰ形式で保存する
extern	int			SaveDrawValidGraphToDDS(            int GrHandle, int x1, int y1, int x2, int y2, const TCHAR *FileName                        ) ;																																// 描画対象にできるグラフィックハンドルをＤＤＳ形式で保存する
extern	int			SaveDrawValidGraphToDDSWithStrLen(  int GrHandle, int x1, int y1, int x2, int y2, const TCHAR *FileName, size_t FileNameLength ) ;																																// 描画対象にできるグラフィックハンドルをＤＤＳ形式で保存する
extern	int			SaveDrawValidGraphToJPEG(           int GrHandle, int x1, int y1, int x2, int y2, const TCHAR *FileName,                        int Quality = 80 , int Sample2x1 = TRUE ) ;																						// 描画対象にできるグラフィックハンドルをＪＰＥＧ形式で保存する Quality = 画質、値が大きいほど低圧縮高画質,0〜100 
extern	int			SaveDrawValidGraphToJPEGWithStrLen( int GrHandle, int x1, int y1, int x2, int y2, const TCHAR *FileName, size_t FileNameLength, int Quality = 80 , int Sample2x1 = TRUE ) ;																						// 描画対象にできるグラフィックハンドルをＪＰＥＧ形式で保存する Quality = 画質、値が大きいほど低圧縮高画質,0〜100 
extern	int			SaveDrawValidGraphToPNG(            int GrHandle, int x1, int y1, int x2, int y2, const TCHAR *FileName,                        int CompressionLevel = -1 ) ;																									// 描画対象にできるグラフィックハンドルをＰＮＧ形式で保存する CompressionLevel = 圧縮率、値が大きいほど高圧縮率高負荷、０は無圧縮,0〜9
extern	int			SaveDrawValidGraphToPNGWithStrLen(  int GrHandle, int x1, int y1, int x2, int y2, const TCHAR *FileName, size_t FileNameLength, int CompressionLevel = -1 ) ;																									// 描画対象にできるグラフィックハンドルをＰＮＧ形式で保存する CompressionLevel = 圧縮率、値が大きいほど高圧縮率高負荷、０は無圧縮,0〜9

#endif // DX_NON_SAVEFUNCTION

// 頂点バッファ関係関数
extern	int			CreateVertexBuffer(		int VertexNum, int VertexType /* DX_VERTEX_TYPE_NORMAL_3D 等 */ ) ;						// 頂点バッファを作成する( -1:エラー  0以上:頂点バッファハンドル )
extern	int			DeleteVertexBuffer(		int VertexBufHandle ) ;																	// 頂点バッファを削除する
extern	int			InitVertexBuffer(		void ) ;																				// すべての頂点バッファを削除する
extern	int			SetVertexBufferData(	int SetIndex, const void *VertexArray, int VertexNum, int VertexBufHandle ) ;			// 頂点バッファに頂点データを転送する
extern	int			CreateIndexBuffer(		int IndexNum, int IndexType /* DX_INDEX_TYPE_16BIT 等 */  ) ;							// インデックスバッファを作成する( -1:エラー　0以上：インデックスバッファハンドル )
extern	int			DeleteIndexBuffer(		int IndexBufHandle ) ;																	// インデックスバッファを削除する
extern	int			InitIndexBuffer(		void ) ;																				// すべてのインデックスバッファを削除する
extern	int			SetIndexBufferData(		int SetIndex, const void *IndexArray, int IndexNum, int IndexBufHandle ) ;				// インデックスバッファにインデックスデータを転送する
extern	int			GetMaxPrimitiveCount(	void ) ;																				// グラフィックスデバイスが対応している一度に描画できるプリミティブの最大数を取得する
extern	int			GetMaxVertexIndex(		void ) ;																				// グラフィックスデバイスが対応している一度に使用することのできる最大頂点数を取得する

// シェーダー関係関数
extern	int			GetValidShaderVersion(		void ) ;												// 使用できるシェーダーのバージョンを取得する( 0=使えない  200=シェーダーモデル２．０が使用可能  300=シェーダーモデル３．０が使用可能 )

extern	int			LoadVertexShader(			const TCHAR *FileName                        ) ;		// 頂点シェーダーバイナリをファイルから読み込み頂点シェーダーハンドルを作成する( 戻り値 -1:エラー  -1以外:シェーダーハンドル )
extern	int			LoadVertexShaderWithStrLen(	const TCHAR *FileName, size_t FileNameLength ) ;		// 頂点シェーダーバイナリをファイルから読み込み頂点シェーダーハンドルを作成する( 戻り値 -1:エラー  -1以外:シェーダーハンドル )
extern	int			LoadPixelShader(			const TCHAR *FileName                        ) ;		// ピクセルシェーダーバイナリをファイルから読み込みピクセルシェーダーハンドルを作成する( 戻り値 -1:エラー  -1以外:シェーダーハンドル )
extern	int			LoadPixelShaderWithStrLen(	const TCHAR *FileName, size_t FileNameLength ) ;		// ピクセルシェーダーバイナリをファイルから読み込みピクセルシェーダーハンドルを作成する( 戻り値 -1:エラー  -1以外:シェーダーハンドル )
extern	int			LoadVertexShaderFromMem(	const void *ImageAddress, int ImageSize ) ;				// メモリに読み込まれた頂点シェーダーバイナリから頂点シェーダーハンドルを作成する( 戻り値 -1:エラー  -1以外:シェーダーハンドル )
extern	int			LoadPixelShaderFromMem(		const void *ImageAddress, int ImageSize ) ;				// メモリに読み込まれたピクセルシェーダーバイナリからピクセルシェーダーハンドルを作成する( 戻り値 -1:エラー  -1以外:シェーダーハンドル )
extern	int			DeleteShader(				int ShaderHandle ) ;									// シェーダーハンドルの削除( 頂点シェーダー・ピクセルシェーダー共通 )
extern	int			InitShader(					void ) ;												// シェーダーハンドルを全て削除する( 頂点シェーダーハンドル・ピクセルシェーダーハンドルどちらもすべて削除 )

extern	int			GetConstIndexToShader(                     const TCHAR *ConstantName,                            int ShaderHandle ) ;	// 指定の名前を持つ定数が使用するシェーダー定数の番号を取得する
extern	int			GetConstIndexToShaderWithStrLen(           const TCHAR *ConstantName, size_t ConstantNameLength, int ShaderHandle ) ;	// 指定の名前を持つ定数が使用するシェーダー定数の番号を取得する
extern	int			GetConstCountToShader(                     const TCHAR *ConstantName,                            int ShaderHandle ) ;	// 指定の名前を持つ定数が使用するシェーダー定数の数を取得する
extern	int			GetConstCountToShaderWithStrLen(           const TCHAR *ConstantName, size_t ConstantNameLength, int ShaderHandle ) ;	// 指定の名前を持つ定数が使用するシェーダー定数の数を取得する
extern	const FLOAT4 *GetConstDefaultParamFToShader(           const TCHAR *ConstantName,                            int ShaderHandle ) ;	// 指定の名前を持つ浮動小数点数定数のデフォルトパラメータが格納されているメモリアドレスを取得する
extern	const FLOAT4 *GetConstDefaultParamFToShaderWithStrLen( const TCHAR *ConstantName, size_t ConstantNameLength, int ShaderHandle ) ;	// 指定の名前を持つ浮動小数点数定数のデフォルトパラメータが格納されているメモリアドレスを取得する
extern	int			SetVSConstSF(         int ConstantIndex,       float  Param ) ;						// 頂点シェーダーの float 型定数を設定する
extern	int			SetVSConstF(          int ConstantIndex,       FLOAT4 Param ) ;						// 頂点シェーダーの float 型定数を設定する
extern	int			SetVSConstFMtx(       int ConstantIndex,       MATRIX Param ) ;						// 頂点シェーダーの float 型定数に行列を設定する
extern	int			SetVSConstFMtxT(      int ConstantIndex,       MATRIX Param ) ;						// 頂点シェーダーの float 型定数に転置した行列を設定する
extern	int			SetVSConstSI(         int ConstantIndex,       int    Param ) ;						// 頂点シェーダーの int   型定数を設定する
extern	int			SetVSConstI(          int ConstantIndex,       INT4   Param ) ;						// 頂点シェーダーの int   型定数を設定する
extern	int			SetVSConstB(          int ConstantIndex,       BOOL   Param ) ;						// 頂点シェーダーの BOOL  型定数を設定する
extern	int			SetVSConstSFArray(    int ConstantIndex, const float  *ParamArray, int ParamNum ) ;	// 頂点シェーダーの float 型定数を設定する( 配列を使って連番インデックスに一度に設定 )
extern	int			SetVSConstFArray(     int ConstantIndex, const FLOAT4 *ParamArray, int ParamNum ) ;	// 頂点シェーダーの float 型定数を設定する( 配列を使って連番インデックスに一度に設定 )
extern	int			SetVSConstFMtxArray(  int ConstantIndex, const MATRIX *ParamArray, int ParamNum ) ;	// 頂点シェーダーの float 型定数に行列を設定する( 配列を使って連番インデックスに一度に設定 )
extern	int			SetVSConstFMtxTArray( int ConstantIndex, const MATRIX *ParamArray, int ParamNum ) ;	// 頂点シェーダーの float 型定数に転置した行列を設定する( 配列を使って連番インデックスに一度に設定 )
extern	int			SetVSConstSIArray(    int ConstantIndex, const int    *ParamArray, int ParamNum ) ;	// 頂点シェーダーの int   型定数を設定する( 配列を使って連番インデックスに一度に設定 )
extern	int			SetVSConstIArray(     int ConstantIndex, const INT4   *ParamArray, int ParamNum ) ;	// 頂点シェーダーの int   型定数を設定する( 配列を使って連番インデックスに一度に設定 )
extern	int			SetVSConstBArray(     int ConstantIndex, const BOOL   *ParamArray, int ParamNum ) ;	// 頂点シェーダーの BOOL  型定数を設定する( 配列を使って連番インデックスに一度に設定 )
extern	int			ResetVSConstF(        int ConstantIndex, int ParamNum ) ;							// 頂点シェーダーの float 型定数の設定をリセットする
extern	int			ResetVSConstI(        int ConstantIndex, int ParamNum ) ;							// 頂点シェーダーの int   型定数の設定をリセットする
extern	int			ResetVSConstB(        int ConstantIndex, int ParamNum ) ;							// 頂点シェーダーの BOOL  型定数の設定をリセットする

extern	int			SetPSConstSF(         int ConstantIndex,       float  Param ) ;						// ピクセルシェーダーの float 型定数を設定する
extern	int			SetPSConstF(          int ConstantIndex,       FLOAT4 Param ) ;						// ピクセルシェーダーの float 型定数を設定する
extern	int			SetPSConstFMtx(       int ConstantIndex,       MATRIX Param ) ;						// ピクセルシェーダーの float 型定数に行列を設定する
extern	int			SetPSConstFMtxT(      int ConstantIndex,       MATRIX Param ) ;						// ピクセルシェーダーの float 型定数に転置した行列を設定する
extern	int			SetPSConstSI(         int ConstantIndex,       int    Param ) ;						// ピクセルシェーダーの int   型定数を設定する
extern	int			SetPSConstI(          int ConstantIndex,       INT4   Param ) ;						// ピクセルシェーダーの int   型定数を設定する
extern	int			SetPSConstB(          int ConstantIndex,       BOOL   Param ) ;						// ピクセルシェーダーの BOOL  型定数を設定する
extern	int			SetPSConstSFArray(    int ConstantIndex, const float  *ParamArray, int ParamNum ) ;	// ピクセルシェーダーの float 型定数を設定する( 配列を使って連番インデックスに一度に設定 )
extern	int			SetPSConstFArray(     int ConstantIndex, const FLOAT4 *ParamArray, int ParamNum ) ;	// ピクセルシェーダーの float 型定数を設定する( 配列を使って連番インデックスに一度に設定 )
extern	int			SetPSConstFMtxArray(  int ConstantIndex, const MATRIX *ParamArray, int ParamNum ) ;	// ピクセルシェーダーの float 型定数に行列を設定する( 配列を使って連番インデックスに一度に設定 )
extern	int			SetPSConstFMtxTArray( int ConstantIndex, const MATRIX *ParamArray, int ParamNum ) ;	// ピクセルシェーダーの float 型定数に転置した行列を設定する( 配列を使って連番インデックスに一度に設定 )
extern	int			SetPSConstSIArray(    int ConstantIndex, const int    *ParamArray, int ParamNum ) ;	// ピクセルシェーダーの int   型定数を設定する( 配列を使って連番インデックスに一度に設定 )
extern	int			SetPSConstIArray(     int ConstantIndex, const INT4   *ParamArray, int ParamNum ) ;	// ピクセルシェーダーの int   型定数を設定する( 配列を使って連番インデックスに一度に設定 )
extern	int			SetPSConstBArray(     int ConstantIndex, const BOOL   *ParamArray, int ParamNum ) ;	// ピクセルシェーダーの BOOL  型定数を設定する( 配列を使って連番インデックスに一度に設定 )
extern	int			ResetPSConstF(        int ConstantIndex, int ParamNum ) ;							// ピクセルシェーダーの float 型定数の設定をリセットする
extern	int			ResetPSConstI(        int ConstantIndex, int ParamNum ) ;							// ピクセルシェーダーの int   型定数の設定をリセットする
extern	int			ResetPSConstB(        int ConstantIndex, int ParamNum ) ;							// ピクセルシェーダーの BOOL  型定数の設定をリセットする

extern	int			SetRenderTargetToShader( int TargetIndex, int DrawScreen, int SurfaceIndex = 0 , int MipLevel = 0 ) ;	// シェーダーを使用した描画での描画先を設定する( DrawScreen に -1 を渡すと無効化 )
extern	int			SetUseTextureToShader(   int StageIndex, int GraphHandle ) ;						// シェーダーを使用した描画で使用するグラフィックハンドルを設定する
extern	int			SetUseVertexShader(      int ShaderHandle ) ;										// シェーダーを使用した描画に使用する頂点シェーダーを設定する( -1を渡すと解除 )
extern	int			SetUsePixelShader(       int ShaderHandle ) ;										// シェーダーを使用した描画に使用するピクセルシェーダーを設定する( -1を渡すと解除 )

extern	int			CalcPolygonBinormalAndTangentsToShader(        VERTEX3DSHADER *VertexArray, int PolygonNum ) ;														// ポリゴンの頂点の接線と従法線をＵＶ座標から計算してセットする
extern	int			CalcPolygonIndexedBinormalAndTangentsToShader( VERTEX3DSHADER *VertexArray, int VertexNum, const unsigned short *IndexArray, int PolygonNum ) ;		// ポリゴンの頂点の接線と従法線をＵＶ座標から計算してセットする( 頂点インデックスを使用する )

extern	int			DrawBillboard3DToShader( VECTOR Pos, float cx, float cy, float Size, float Angle, int GrHandle, int TransFlag, int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ;							// シェーダーを使ってビルボードを描画する
extern	int			DrawPolygon2DToShader(          const VERTEX2DSHADER *VertexArray, int PolygonNum ) ;																											// シェーダーを使って２Ｄポリゴンを描画する
extern	int			DrawPolygon3DToShader(          const VERTEX3DSHADER *VertexArray, int PolygonNum ) ;																											// シェーダーを使って３Ｄポリゴンを描画する
extern	int			DrawPolygonIndexed2DToShader(   const VERTEX2DSHADER *VertexArray, int VertexNum, const unsigned short *IndexArray, int PolygonNum ) ;															// シェーダーを使って２Ｄポリゴンを描画する( 頂点インデックスを使用する )
extern	int			DrawPolygonIndexed3DToShader(   const VERTEX3DSHADER *VertexArray, int VertexNum, const unsigned short *IndexArray, int PolygonNum ) ;															// シェーダーを使って３Ｄポリゴンを描画する( 頂点インデックスを使用する )
extern	int			DrawPrimitive2DToShader(        const VERTEX2DSHADER *VertexArray, int VertexNum,                                                 int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ ) ;		// シェーダーを使って２Ｄプリミティブを描画する
extern	int			DrawPrimitive3DToShader(        const VERTEX3DSHADER *VertexArray, int VertexNum,                                                 int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ ) ;		// シェーダーを使って３Ｄプリミティブを描画する
extern	int			DrawPrimitiveIndexed2DToShader( const VERTEX2DSHADER *VertexArray, int VertexNum, const unsigned short *IndexArray, int IndexNum, int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ ) ;		// シェーダーを使って２Ｄプリミティブを描画する( 頂点インデックスを使用する )
extern	int			DrawPrimitiveIndexed3DToShader( const VERTEX3DSHADER *VertexArray, int VertexNum, const unsigned short *IndexArray, int IndexNum, int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ ) ;		// シェーダーを使って３Ｄプリミティブを描画する( 頂点インデックスを使用する )
extern	int			DrawPolygon3DToShader_UseVertexBuffer(           int VertexBufHandle ) ;																														// シェーダーを使って３Ｄポリゴンを描画する( 頂点バッファ使用版 )
extern	int			DrawPolygonIndexed3DToShader_UseVertexBuffer(    int VertexBufHandle, int IndexBufHandle ) ;																									// シェーダーを使って３Ｄポリゴンを描画する( 頂点バッファとインデックスバッファ使用版 )
extern	int			DrawPrimitive3DToShader_UseVertexBuffer(         int VertexBufHandle,                     int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ ) ;												// シェーダーを使って３Ｄプリミティブを描画する( 頂点バッファ使用版 )
extern	int			DrawPrimitive3DToShader_UseVertexBuffer2(        int VertexBufHandle,                     int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */, int StartVertex, int UseVertexNum ) ;			// シェーダーを使って３Ｄプリミティブを描画する( 頂点バッファ使用版 )
extern	int			DrawPrimitiveIndexed3DToShader_UseVertexBuffer(  int VertexBufHandle, int IndexBufHandle, int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ ) ;												// シェーダーを使って３Ｄプリミティブを描画する( 頂点バッファとインデックスバッファ使用版 )
extern	int			DrawPrimitiveIndexed3DToShader_UseVertexBuffer2( int VertexBufHandle, int IndexBufHandle, int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */, int BaseVertex, int StartVertex, int UseVertexNum, int StartIndex, int UseIndexNum ) ;	// シェーダーを使って３Ｄプリミティブを描画する( 頂点バッファとインデックスバッファ使用版 )

// シェーダー用定数バッファ関係関数
extern	int			InitShaderConstantBuffer(		void ) ;																					// 全てのシェーダー用定数バッファハンドルを削除する
extern	int			CreateShaderConstantBuffer(		int BufferSize ) ;																			// シェーダー用定数バッファハンドルを初期化する
extern	int			DeleteShaderConstantBuffer(		int SConstBufHandle ) ;																		// シェーダー用定数バッファハンドルを削除する
extern	void *		GetBufferShaderConstantBuffer(	int SConstBufHandle ) ;																		// シェーダー用定数バッファハンドルの定数バッファのアドレスを取得する
extern	int			UpdateShaderConstantBuffer(		int SConstBufHandle ) ;																		// シェーダー用定数バッファハンドルの定数バッファへの変更を適用する
extern	int			SetShaderConstantBuffer(		int SConstBufHandle, int TargetShader /* DX_SHADERTYPE_VERTEX など */ , int Slot ) ;		// シェーダー用定数バッファハンドルの定数バッファを指定のシェーダーの指定のスロットにセットする

// フィルター関係関数
#ifndef DX_NON_FILTER
extern	int			GraphFilter(         int    GrHandle,                                                                                                               int FilterType /* DX_GRAPH_FILTER_GAUSS 等 */ , ... ) ;		// 画像にフィルター処理を行う
extern	int			GraphFilterBlt(      int SrcGrHandle, int DestGrHandle,                                                                                             int FilterType /* DX_GRAPH_FILTER_GAUSS 等 */ , ... ) ;		// 画像のフィルター付き転送を行う
extern	int			GraphFilterRectBlt(  int SrcGrHandle, int DestGrHandle, int SrcX1, int SrcY1, int SrcX2, int SrcY2, int DestX,  int DestY,                          int FilterType /* DX_GRAPH_FILTER_GAUSS 等 */ , ... ) ;		// 画像のフィルター付き転送を行う( 矩形指定 )
//		int			GraphFilter( int GrHandle, int FilterType = DX_GRAPH_FILTER_MONO, int Cb = 青色差( -255 〜 255 ), int Cr = 赤色差( -255 〜 255 ) ) ;
//		int			GraphFilter( int GrHandle, int FilterType = DX_GRAPH_FILTER_GAUSS, int PixelWidth = 使用ピクセル幅( 8 , 16 , 32 の何れか ), int Param = ぼかしパラメータ( 100 で約1ピクセル分の幅 ) ) ;
//		int			GraphFilter( int GrHandle, int FilterType = DX_GRAPH_FILTER_DOWN_SCALE, int DivNum = 元のサイズの何分の１か、という値( 2 , 4 , 8 の何れか ) ) ;
//		int			GraphFilter( int GrHandle, int FilterType = DX_GRAPH_FILTER_BRIGHT_CLIP, int CmpType = クリップタイプ( DX_CMP_LESS:CmpParam以下をクリップ  又は  DX_CMP_GREATER:CmpParam以上をクリップ ), int CmpParam = クリップパラメータ( 0 〜 255 ), int ClipFillFlag = クリップしたピクセルを塗りつぶすかどうか( TRUE:塗りつぶす  FALSE:塗りつぶさない ), unsigned int ClipFillColor = クリップしたピクセルに塗る色値( GetColor で取得する )( ClipFillFlag が FALSE の場合は使用しない ), int ClipFillAlpha = クリップしたピクセルに塗るα値( 0 〜 255 )( ClipFillFlag が FALSE の場合は使用しない ) ) ;
//		int			GraphFilter( int GrHandle, int FilterType = DX_GRAPH_FILTER_BRIGHT_SCALE, int MinBright = 変換後に真っ暗になる明るさ( 0 〜 255 ), int MaxBright = 変換後に真っ白になる明るさ( 0 〜 255 ) ) ;
//		int			GraphFilter( int GrHandle, int FilterType = DX_GRAPH_FILTER_HSB, int HueType = Hue の意味( 0:相対値  1:絶対値 ), int Hue = 色相パラメータ( HueType が 0 の場合 = ピクセルの色相に対する相対値( -180 〜 180 )   HueType が 1 の場合 = 色相の絶対値( 0 〜 360 ) ), int Saturation = 彩度( -255 〜 ), int Bright = 輝度( -255 〜 255 ) ) ;
//		int			GraphFilter( int GrHandle, int FilterType = DX_GRAPH_FILTER_INVERT ) ;
//		int			GraphFilter( int GrHandle, int FilterType = DX_GRAPH_FILTER_LEVEL, int Min = 変換元の下限値( 0 〜 255 ), int Max = 変換元の上限値( 0 〜 255 ), int Gamma = ガンマ値( 100 でガンマ補正無し、0 とそれ以下の値は不可 ), int AfterMin = 変換後の最低値( 0 〜 255 ), int AfterMax = 変換後の最大値( 0 〜 255 ) ) ;
//		int			GraphFilter( int GrHandle, int FilterType = DX_GRAPH_FILTER_TWO_COLOR, int Threshold = 閾値( 0 〜 255 ), unsigned int LowColor = 閾値より値が低かったピクセルの変換後の色値( GetColor で取得する ), int LowAlpha = 閾値より値が低かったピクセルの変換後のα値( 0 〜 255 ), unsigned int HighColor = 閾値より値が高かったピクセルの変換後の色値( GetColor で取得する ), int HighAlpha = 閾値より値が高かったピクセルの変換後のα値( 0 〜 255 ) ) ;
//		int			GraphFilter( int GrHandle, int FilterType = DX_GRAPH_FILTER_GRADIENT_MAP, int MapGrHandle = グラデーションマップのグラフィックハンドル( 元画像の輝度からグラデーションマップ画像の x 座標を算出しますので縦幅は1dotでもＯＫ ), int Reverse = グラデーションマップ左右反転フラグ( TRUE : グラデーションマップを左右反転して使う  FALSE : 左右反転しない ) ) ;
//		int			GraphFilter( int GrHandle, int FilterType = DX_GRAPH_FILTER_PREMUL_ALPHA ) ;			// 通常のアルファチャンネル付き画像を乗算済みアルファ画像に変換するフィルタ
//		int			GraphFilter( int GrHandle, int FilterType = DX_GRAPH_FILTER_INTERP_ALPHA ) ;			// 乗算済みα画像を通常のアルファチャンネル付き画像に変換するフィルタ
//		int			GraphFilter( int GrHandle, int FilterType = DX_GRAPH_FILTER_YUV_TO_RGB ) ;				// YUVカラーをRGBカラーに変換するフィルタ
//		int			GraphFilter( int GrHandle, int FilterType = DX_GRAPH_FILTER_Y2UV1_TO_RGB, int UVGrHandle = YUVカラーのUV成分のみで、且つYに対して解像度が半分( 若しくは 4 分の 1 )のグラフィックハンドル( U=R, V=G ) ) ;		// YUVカラーをRGBカラーに変換するフィルタ( UV成分が Y成分の半分・又は４分の１( 横・縦片方若しくは両方 )の解像度しかない場合用 )
//		int			GraphFilter( int GrHandle, int FilterType = DX_GRAPH_FILTER_YUV_TO_RGB_RRA ) ;				// YUVカラーをRGBカラーに変換するフィルタ( 且つ右側半分のRの値をアルファ値として扱う )
//		int			GraphFilter( int GrHandle, int FilterType = DX_GRAPH_FILTER_Y2UV1_TO_RGB_RRA, int UVGrHandle = YUVカラーのUV成分のみで、且つYに対して解像度が半分( 若しくは 4 分の 1 )のグラフィックハンドル( U=R, V=G ) ) ;		// YUVカラーをRGBカラーに変換するフィルタ( UV成分が Y成分の半分・又は４分の１( 横・縦片方若しくは両方 )の解像度しかない場合用 )( 且つ右側半分のRの値をアルファ値として扱う )
//		int			GraphFilter( int GrHandle, int FilterType = DX_GRAPH_FILTER_BICUBIC_SCALE, int DestSizeX = スケーリング後の横ピクセル数, int DestSizeY = スケーリング後の縦ピクセル数 ) ;
//		int			GraphFilter( int GrHandle, int FilterType = DX_GRAPH_FILTER_LANCZOS3_SCALE, int DestSizeX = スケーリング後の横ピクセル数, int DestSizeY = スケーリング後の縦ピクセル数 ) ;

extern	int			GraphBlend(         int    GrHandle, int BlendGrHandle,                                                                                                              int BlendRatio /* ブレンド効果の影響度( 0:０％  255:１００％ ) */ , int BlendType /* DX_GRAPH_BLEND_ADD 等 */ , ... ) ;	// 二つの画像をブレンドする
extern	int			GraphBlendBlt(      int SrcGrHandle, int BlendGrHandle, int DestGrHandle,                                                                                            int BlendRatio /* ブレンド効果の影響度( 0:０％  255:１００％ ) */ , int BlendType /* DX_GRAPH_BLEND_ADD 等 */ , ... ) ;	// 二つの画像をブレンドして結果を指定の画像に出力する
extern	int			GraphBlendRectBlt(  int SrcGrHandle, int BlendGrHandle, int DestGrHandle, int SrcX1, int SrcY1, int SrcX2, int SrcY2, int BlendX,  int BlendY, int DestX, int DestY, int BlendRatio /* ブレンド効果の影響度( 0:０％  255:１００％ ) */ , int BlendType /* DX_GRAPH_BLEND_ADD 等 */ , ... ) ;	// 二つの画像をブレンドして結果を指定の画像に出力する( 矩形指定 )
//		int			GraphBlend( int GrHandle, int BlendGrHandle, int BlendRatio, int BlendType = DX_GRAPH_BLEND_NORMAL ) ;
//		int			GraphBlend( int GrHandle, int BlendGrHandle, int BlendRatio, int BlendType = DX_GRAPH_BLEND_RGBA_SELECT_MIX, int SelectR = ( 出力の赤分となる成分 DX_RGBA_SELECT_SRC_R 等 ), int SelectG = ( 出力の緑成分となる成分 DX_RGBA_SELECT_SRC_R 等 ), int SelectB = ( 出力の青成分となる成分 DX_RGBA_SELECT_SRC_R 等 ), int SelectA = ( 出力のα成分となる成分 DX_RGBA_SELECT_SRC_R 等 ) ) ;
//		int			GraphBlend( int GrHandle, int BlendGrHandle, int BlendRatio, int BlendType = DX_GRAPH_BLEND_MULTIPLE ) ;
//		int			GraphBlend( int GrHandle, int BlendGrHandle, int BlendRatio, int BlendType = DX_GRAPH_BLEND_DIFFERENCE ) ;
//		int			GraphBlend( int GrHandle, int BlendGrHandle, int BlendRatio, int BlendType = DX_GRAPH_BLEND_ADD ) ;
//		int			GraphBlend( int GrHandle, int BlendGrHandle, int BlendRatio, int BlendType = DX_GRAPH_BLEND_SCREEN ) ;
//		int			GraphBlend( int GrHandle, int BlendGrHandle, int BlendRatio, int BlendType = DX_GRAPH_BLEND_OVERLAY ) ;
//		int			GraphBlend( int GrHandle, int BlendGrHandle, int BlendRatio, int BlendType = DX_GRAPH_BLEND_DODGE ) ;
//		int			GraphBlend( int GrHandle, int BlendGrHandle, int BlendRatio, int BlendType = DX_GRAPH_BLEND_BURN ) ;
//		int			GraphBlend( int GrHandle, int BlendGrHandle, int BlendRatio, int BlendType = DX_GRAPH_BLEND_DARKEN ) ;
//		int			GraphBlend( int GrHandle, int BlendGrHandle, int BlendRatio, int BlendType = DX_GRAPH_BLEND_LIGHTEN ) ;
//		int			GraphBlend( int GrHandle, int BlendGrHandle, int BlendRatio, int BlendType = DX_GRAPH_BLEND_SOFTLIGHT ) ;
//		int			GraphBlend( int GrHandle, int BlendGrHandle, int BlendRatio, int BlendType = DX_GRAPH_BLEND_HARDLIGHT ) ;
//		int			GraphBlend( int GrHandle, int BlendGrHandle, int BlendRatio, int BlendType = DX_GRAPH_BLEND_EXCLUSION ) ;
//		int			GraphBlend( int GrHandle, int BlendGrHandle, int BlendRatio, int BlendType = DX_GRAPH_BLEND_NORMAL_ALPHACH ) ;
//		int			GraphBlend( int GrHandle, int BlendGrHandle, int BlendRatio, int BlendType = DX_GRAPH_BLEND_ADD_ALPHACH ) ;
//		int			GraphBlend( int GrHandle, int BlendGrHandle, int BlendRatio, int BlendType = DX_GRAPH_BLEND_MULTIPLE_A_ONLY ) ;
//		int			GraphBlend( int GrHandle, int BlendGrHandle, int BlendRatio, int BlendType = DX_GRAPH_BLEND_PMA_MULTIPLE_A_ONLY ) ;
#endif // DX_NON_FILTER

#ifndef DX_NON_MOVIE
// ムービーグラフィック関係関数
extern	int			PlayMovie(							const TCHAR *FileName,                        int ExRate, int PlayType ) ;	// 動画ファイルの再生
extern	int			PlayMovieWithStrLen(				const TCHAR *FileName, size_t FileNameLength, int ExRate, int PlayType ) ;	// 動画ファイルの再生
extern	int			GetMovieImageSize_File(             const TCHAR *FileName,                        int *SizeX, int *SizeY ) ;	// 動画ファイルの横ピクセル数と縦ピクセル数を取得する
extern	int			GetMovieImageSize_File_WithStrLen(  const TCHAR *FileName, size_t FileNameLength, int *SizeX, int *SizeY ) ;	// 動画ファイルの横ピクセル数と縦ピクセル数を取得する
extern	int			GetMovieImageSize_Mem(              const void *FileImage, int FileImageSize, int *SizeX, int *SizeY ) ;		// メモリ上に展開された動画ファイルの横ピクセル数と縦ピクセル数を取得する
extern	int			OpenMovieToGraph(					const TCHAR *FileName,                        int FullColor = TRUE ) ;		// 動画ファイルを開く
extern	int			OpenMovieToGraphWithStrLen(			const TCHAR *FileName, size_t FileNameLength, int FullColor = TRUE ) ;		// 動画ファイルを開く
extern 	int			PlayMovieToGraph(					int GraphHandle, int PlayType = DX_PLAYTYPE_BACK , int SysPlay = 0 ) ;		// 動画ファイルを使用するグラフィックハンドルの動画ファイルの再生を開始する
extern 	int			PauseMovieToGraph(					int GraphHandle, int SysPause = 0 ) ;										// 動画ファイルを使用するグラフィックハンドルの動画ファイルの再生を停止する
extern	int			AddMovieFrameToGraph(				int GraphHandle, unsigned int FrameNum ) ;									// 動画ファイルの再生フレームを進める、戻すことは出来ない( 動画ファイルが停止状態で、且つ Ogg Theora のみ有効 )
extern	int			SeekMovieToGraph(					int GraphHandle, int Time ) ;												// 動画ファイルの再生位置を設定する(ミリ秒単位)
extern	int			SetPlaySpeedRateMovieToGraph(		int GraphHandle, double SpeedRate ) ;										// 動画ファイルの再生速度を設定する( 1.0 = 等倍速  2.0 = ２倍速 )、一部のファイルフォーマットのみで有効な機能です
extern 	int			GetMovieStateToGraph(				int GraphHandle ) ;															// 動画ファイルの再生状態を得る
extern	int			SetMovieVolumeToGraph(				int Volume, int GraphHandle ) ;												// 動画ファイルの音量を設定する(0〜10000)
extern	int			ChangeMovieVolumeToGraph(			int Volume, int GraphHandle ) ;												// 動画ファイルの音量を設定する(0〜255)
extern	const BASEIMAGE* GetMovieBaseImageToGraph(		int GraphHandle, int *ImageUpdateFlag = NULL , int ImageUpdateFlagSetOnly = FALSE ) ;	// 動画ファイルの基本イメージデータを取得する( ImageUpdateFlag に int 型変数のアドレスを渡すと、イメージが更新された場合は 1 が、更新されていない場合は 0 が格納されます、 ImageUpdateFlagSetOnly を TRUE にすると戻り値の BASEIMAGE は有効な画像データではなくなりますが、BASEIMAGE の更新処理が行われませんので、ImageUpdateFlag を利用して画像が更新されたかどうかだけをチェックしたい場合は TRUE にしてください )
extern	int			GetMovieTotalFrameToGraph(			int GraphHandle ) ;															// 動画ファイルの総フレーム数を得る( Ogg Theora でのみ有効 )
extern	int			TellMovieToGraph(					int GraphHandle ) ;															// 動画ファイルの再生位置を取得する(ミリ秒単位)
extern	int			TellMovieToGraphToFrame(			int GraphHandle ) ;															// 動画ファイルの再生位置を取得する(フレーム単位)
extern	int			SeekMovieToGraphToFrame(			int GraphHandle, int Frame ) ;												// 動画ファイルの再生位置を設定する(フレーム単位)
extern	LONGLONG	GetOneFrameTimeMovieToGraph(		int GraphHandle ) ;															// 動画ファイルの１フレームあたりの時間を取得する(戻り値：１フレームの時間(単位:マイクロ秒))
extern	int			GetLastUpdateTimeMovieToGraph(		int GraphHandle ) ;															// 動画ファイルのイメージを最後に更新した時間を得る(ミリ秒単位)
extern	int			SetMovieRightImageAlphaFlag(		int Flag ) ;																// 読み込む動画ファイル映像の右半分の赤成分をα情報として扱うかどうかをセットする( TRUE:α情報として扱う  FALSE:α情報として扱わない( デフォルト ) )
extern	int			SetMovieColorA8R8G8B8Flag(			int Flag ) ;																// 読み込む動画ファイルが32bitカラーだった場合、A8R8G8B8 形式として扱うかどうかをセットする、32bitカラーではない動画ファイルに対しては無効( Flag  TRUE:A8R8G8B8として扱う  FALSE:X8R8G8B8として扱う( デフォルト ) )
extern	int			SetMovieUseYUVFormatSurfaceFlag(	int Flag ) ;																// ＹＵＶフォーマットのサーフェスが使用できる場合はＹＵＶフォーマットのサーフェスを使用するかどうかを設定する( TRUE:使用する( デフォルト ) FALSE:ＲＧＢフォーマットのサーフェスを使用する )
#endif // DX_NON_MOVIE

// カメラ関係関数
extern	int			SetCameraNearFar(					float  Near, float  Far ) ;												// カメラの Nearクリップ面と Farクリップ面の距離を設定する
extern	int			SetCameraNearFarD(					double Near, double Far ) ;												// カメラの Nearクリップ面と Farクリップ面の距離を設定する
extern	int			SetCameraPositionAndTarget_UpVecY(  VECTOR    Position, VECTOR   Target ) ;									// カメラの視点、注視点、アップベクトルを設定する( アップベクトルはＹ軸方向から導き出す )
extern	int			SetCameraPositionAndTarget_UpVecYD( VECTOR_D  Position, VECTOR_D Target ) ;									// カメラの視点、注視点、アップベクトルを設定する( アップベクトルはＹ軸方向から導き出す )
extern	int			SetCameraPositionAndTargetAndUpVec( VECTOR    Position, VECTOR   TargetPosition, VECTOR   UpVector ) ;		// カメラの視点、注視点、アップベクトルを設定する
extern	int			SetCameraPositionAndTargetAndUpVecD( VECTOR_D Position, VECTOR_D TargetPosition, VECTOR_D UpVector ) ;		// カメラの視点、注視点、アップベクトルを設定する
extern	int			SetCameraPositionAndAngle(			VECTOR   Position, float  VRotate, float  HRotate, float  TRotate ) ;	// カメラの視点、注視点、アップベクトルを設定する( 注視点とアップベクトルは垂直回転角度、水平回転角度、捻り回転角度から導き出す )
extern	int			SetCameraPositionAndAngleD(			VECTOR_D Position, double VRotate, double HRotate, double TRotate ) ;	// カメラの視点、注視点、アップベクトルを設定する( 注視点とアップベクトルは垂直回転角度、水平回転角度、捻り回転角度から導き出す )
extern	int			SetCameraViewMatrix(				MATRIX   ViewMatrix ) ;													// ビュー行列を直接設定する
extern	int			SetCameraViewMatrixD(				MATRIX_D ViewMatrix ) ;													// ビュー行列を直接設定する
extern	int			SetCameraScreenCenter(				float x, float y ) ;													// 画面上におけるカメラが見ている映像の中心の座標を設定する
extern	int			SetCameraScreenCenterD(				double x, double y ) ;													// 画面上におけるカメラが見ている映像の中心の座標を設定する

extern	int			SetupCamera_Perspective(			float  Fov ) ;															// 遠近法カメラをセットアップする
extern	int			SetupCamera_PerspectiveD(			double Fov ) ;															// 遠近法カメラをセットアップする
extern	int			SetupCamera_Ortho(					float  Size ) ;															// 正射影カメラをセットアップする
extern	int			SetupCamera_OrthoD(					double Size ) ;															// 正射影カメラをセットアップする
extern	int			SetupCamera_ProjectionMatrix(		MATRIX   ProjectionMatrix ) ;											// 射影行列を直接設定する
extern	int			SetupCamera_ProjectionMatrixD(		MATRIX_D ProjectionMatrix ) ;											// 射影行列を直接設定する
extern	int			SetCameraDotAspect(					float  DotAspect ) ;													// カメラのドットアスペクト比を設定する
extern	int			SetCameraDotAspectD(				double DotAspect ) ;													// カメラのドットアスペクト比を設定する

extern	int			CheckCameraViewClip(				VECTOR   CheckPos ) ;													// 指定の座標がカメラの視界に入っているかどうかを判定する( 戻り値 TRUE:視界に入っていない  FALSE:視界に入っている )
extern	int			CheckCameraViewClipD(				VECTOR_D CheckPos ) ;													// 指定の座標がカメラの視界に入っているかどうかを判定する( 戻り値 TRUE:視界に入っていない  FALSE:視界に入っている )
extern	int			CheckCameraViewClip_Dir(			VECTOR   CheckPos ) ;													// 指定の座標がカメラの視界に入っているかどうかを判定する、戻り値で外れている方向も知ることができる( 戻り値 0:視界に入っている  0以外:視界に入っていない( DX_CAMERACLIP_LEFT や DX_CAMERACLIP_RIGHT が or 演算で混合されたもの、and 演算で方向を確認できる ) )
extern	int			CheckCameraViewClip_DirD(			VECTOR_D CheckPos ) ;													// 指定の座標がカメラの視界に入っているかどうかを判定する、戻り値で外れている方向も知ることができる( 戻り値 0:視界に入っている  0以外:視界に入っていない( DX_CAMERACLIP_LEFT や DX_CAMERACLIP_RIGHT が or 演算で混合されたもの、and 演算で方向を確認できる ) )
extern	int			CheckCameraViewClip_Box(			VECTOR   BoxPos1, VECTOR   BoxPos2 ) ;									// 二つの座標で表されるボックスがカメラの視界に入っているかどうかを判定する( 戻り値 TRUE:視界に入っていない  FALSE:視界に入っている )
extern	int			CheckCameraViewClip_BoxD(			VECTOR_D BoxPos1, VECTOR_D BoxPos2 ) ;									// 二つの座標で表されるボックスがカメラの視界に入っているかどうかを判定する( 戻り値 TRUE:視界に入っていない  FALSE:視界に入っている )

extern	float		GetCameraNear(						void ) ;																// カメラの Near クリップ面の距離を取得する
extern	double		GetCameraNearD(						void ) ;																// カメラの Near クリップ面の距離を取得する
extern	float		GetCameraFar(						void ) ;																// カメラの Far クリップ面の距離を取得する
extern	double		GetCameraFarD(						void ) ;																// カメラの Far クリップ面の距離を取得する

extern	VECTOR		GetCameraPosition(					void ) ;																// カメラの位置を取得する
extern	VECTOR_D	GetCameraPositionD(					void ) ;																// カメラの位置を取得する
extern	VECTOR		GetCameraTarget(					void ) ;																// カメラの注視点を取得する
extern	VECTOR_D	GetCameraTargetD(					void ) ;																// カメラの注視点を取得する
extern	VECTOR		GetCameraUpVector(					void ) ;																// カメラの正面方向に垂直な上方向のベクトルを取得する
extern	VECTOR_D	GetCameraUpVectorD(					void ) ;																// カメラの正面方向に垂直な上方向のベクトルを取得する
extern	VECTOR		GetCameraDownVector(				void ) ;																// カメラの正面方向に垂直な下方向のベクトルを取得する
extern	VECTOR_D	GetCameraDownVectorD(				void ) ;																// カメラの正面方向に垂直な下方向のベクトルを取得する
extern	VECTOR		GetCameraRightVector(				void ) ;																// カメラの正面方向に垂直な右方向のベクトルを取得する
extern	VECTOR_D	GetCameraRightVectorD(				void ) ;																// カメラの正面方向に垂直な右方向のベクトルを取得する
extern	VECTOR		GetCameraLeftVector(				void ) ;																// カメラの正面方向に垂直な左方向のベクトルを取得する
extern	VECTOR_D	GetCameraLeftVectorD(				void ) ;																// カメラの正面方向に垂直な左方向のベクトルを取得する
extern	VECTOR		GetCameraFrontVector(				void ) ;																// カメラの正面方向のベクトルを取得する
extern	VECTOR_D	GetCameraFrontVectorD(				void ) ;																// カメラの正面方向のベクトルを取得する
extern	VECTOR		GetCameraBackVector(				void ) ;																// カメラの後ろ方向のベクトルを取得する
extern	VECTOR_D	GetCameraBackVectorD(				void ) ;																// カメラの後ろ方向のベクトルを取得する
extern	float		GetCameraAngleHRotate(				void ) ;																// カメラの水平方向の向きを取得する
extern	double		GetCameraAngleHRotateD(				void ) ;																// カメラの水平方向の向きを取得する
extern	float		GetCameraAngleVRotate(				void ) ;																// カメラの垂直方向の向きを取得する
extern	double		GetCameraAngleVRotateD(				void ) ;																// カメラの垂直方向の向きを取得する
extern	float		GetCameraAngleTRotate(				void ) ;																// カメラの向きの捻り角度を取得する
extern	double		GetCameraAngleTRotateD(				void ) ;																// カメラの向きの捻り角度を取得する

extern	MATRIX		GetCameraViewMatrix(				void ) ;																// ビュー行列を取得する
extern	MATRIX_D	GetCameraViewMatrixD(				void ) ;																// ビュー行列を取得する
extern	MATRIX		GetCameraBillboardMatrix(			void ) ;																// ビルボード行列を取得する
extern	MATRIX_D	GetCameraBillboardMatrixD(			void ) ;																// ビルボード行列を取得する
extern	int			GetCameraScreenCenter(				float  *x, float  *y ) ;												// 画面上におけるカメラが見ている映像の中心の座標を取得する
extern	int			GetCameraScreenCenterD(				double *x, double *y ) ;												// 画面上におけるカメラが見ている映像の中心の座標を取得する
extern	float		GetCameraFov(						void ) ;																// カメラの視野角を取得する
extern	double		GetCameraFovD(						void ) ;																// カメラの視野角を取得する
extern	float		GetCameraSize(						void ) ;																// カメラの視野サイズを取得する
extern	double		GetCameraSizeD(						void ) ;																// カメラの視野サイズを取得する
extern	MATRIX		GetCameraProjectionMatrix(			void ) ;																// 射影行列を取得する
extern	MATRIX_D	GetCameraProjectionMatrixD(			void ) ;																// 射影行列を取得する
extern	float		GetCameraDotAspect(					void ) ;																// カメラのドットアスペクト比を得る
extern	double		GetCameraDotAspectD(				void ) ;																// カメラのドットアスペクト比を得る
extern	MATRIX		GetCameraViewportMatrix(			void ) ;																// ビューポート行列を取得する
extern	MATRIX_D	GetCameraViewportMatrixD(			void ) ;																// ビューポート行列を取得する
extern	MATRIX		GetCameraAPIViewportMatrix(			void ) ;																// Direct3Dで自動適用されるビューポート行列を取得する
extern	MATRIX_D	GetCameraAPIViewportMatrixD(		void ) ;																// Direct3Dで自動適用されるビューポート行列を取得する

// ライト関係関数
extern	int			SetUseLighting(				int Flag ) ;																	// ライティングを使用するかどうかを設定する
extern	int			SetMaterialUseVertDifColor( int UseFlag ) ;																	// ３Ｄ描画のライティング計算で頂点カラーのディフューズカラーを使用するかどうかを設定する
extern	int			SetMaterialUseVertSpcColor( int UseFlag ) ;																	// ３Ｄ描画のライティング計算で頂点カラーのスペキュラカラーを使用するかどうかを設定する
extern	int			SetMaterialParam(			MATERIALPARAM Material ) ;														// ３Ｄ描画のライティング計算で使用するマテリアルパラメータを設定する
extern	int			SetUseSpecular(				int UseFlag ) ;																	// ３Ｄ描画にスペキュラを使用するかどうかを設定する
extern	int			SetGlobalAmbientLight(		COLOR_F Color ) ;																// グローバルアンビエントライトカラーを設定する

extern	int			ChangeLightTypeDir(			VECTOR Direction ) ;															// デフォルトライトのタイプをディレクショナルライトにする
extern	int			ChangeLightTypeSpot(		VECTOR Position, VECTOR Direction, float OutAngle, float InAngle, float Range, float Atten0, float Atten1, float Atten2 ) ;	// デフォルトライトのタイプをスポットライトにする
extern	int			ChangeLightTypePoint(		VECTOR Position, float Range, float Atten0, float Atten1, float Atten2 ) ;		// デフォルトライトのタイプをポイントライトにする
extern	int			GetLightType(				void ) ;																		// デフォルトライトのタイプを取得する( 戻り値は DX_LIGHTTYPE_DIRECTIONAL 等 )
extern	int			SetLightEnable(				int EnableFlag ) ;																// デフォルトライトを使用するかどうかを設定する
extern	int			GetLightEnable(				void ) ;																		// デフォルトライトを使用するかどうかを取得する( 戻り値　TRUE:有効  FALSE:無効 )
extern	int			SetLightDifColor(			COLOR_F Color ) ;																// デフォルトライトのディフューズカラーを設定する
extern	COLOR_F		GetLightDifColor(			void ) ;																		// デフォルトライトのディフューズカラーを取得する
extern	int			SetLightSpcColor(			COLOR_F Color ) ;																// デフォルトライトのスペキュラカラーを設定する
extern	COLOR_F		GetLightSpcColor(			void ) ;																		// デフォルトライトのスペキュラカラーを取得する
extern	int			SetLightAmbColor(			COLOR_F Color ) ;																// デフォルトライトのアンビエントカラーを設定する
extern	COLOR_F		GetLightAmbColor(			void ) ;																		// デフォルトライトのアンビエントカラーを取得する
extern	int			SetLightDirection(			VECTOR Direction ) ;															// デフォルトライトの方向を設定する
extern	VECTOR		GetLightDirection(			void ) ;																		// デフォルトライトの方向を取得する
extern	int			SetLightPosition(			VECTOR Position ) ;																// デフォルトライトの位置を設定する
extern	VECTOR		GetLightPosition(			void ) ;																		// デフォルトライトの位置を取得する
extern	int			SetLightRangeAtten(			float Range, float Atten0, float Atten1, float Atten2 ) ;						// デフォルトライトの距離減衰パラメータを設定する( 有効距離、距離減衰係数０、１、２ )
extern	int			GetLightRangeAtten(			float *Range, float *Atten0, float *Atten1, float *Atten2 )	;					// デフォルトライトの距離減衰パラメータを取得する( 有効距離、距離減衰係数０、１、２ )
extern	int			SetLightAngle(				float OutAngle, float InAngle ) ;												// デフォルトライトのスポットライトのパラメータを設定する( 外部コーン角度、内部コーン角度 )
extern	int			GetLightAngle(				float *OutAngle, float *InAngle ) ;												// デフォルトライトのスポットライトのパラメータを取得する( 外部コーン角度、内部コーン角度 )
extern	int			SetLightUseShadowMap(		int SmSlotIndex, int UseFlag ) ;												// デフォルトライトに SetUseShadowMap で指定したシャドウマップを適用するかどうかを設定する( SmSlotIndex:シャドウマップスロット( SetUseShadowMap の第一引数に設定する値 ) UseFlag:適用にするかどうかのフラグ( TRUE:適用する( デフォルト )  FALSE:適用しない ) )

extern	int			CreateDirLightHandle(       VECTOR Direction ) ;															// ディレクショナルライトハンドルを作成する
extern	int			CreateSpotLightHandle(      VECTOR Position, VECTOR Direction, float OutAngle, float InAngle, float Range, float Atten0, float Atten1, float Atten2 ) ;	// スポットライトハンドルを作成する
extern	int			CreatePointLightHandle(     VECTOR Position, float Range, float Atten0, float Atten1, float Atten2 ) ;		// ポイントライトハンドルを作成する
extern	int			DeleteLightHandle(          int LHandle ) ;																	// ライトハンドルを削除する
extern	int			DeleteLightHandleAll(       void ) ;																		// ライトハンドルを全て削除する
extern	int			SetLightTypeHandle(         int LHandle, int LightType ) ;													// ライトハンドルのライトのタイプを変更する( DX_LIGHTTYPE_DIRECTIONAL 等 )
extern	int			SetLightEnableHandle(       int LHandle, int EnableFlag ) ;													// ライトハンドルのライト効果の有効、無効を設定する( TRUE:有効  FALSE:無効 )
extern	int			SetLightDifColorHandle(     int LHandle, COLOR_F Color ) ;													// ライトハンドルのライトのディフューズカラーを設定する
extern	int			SetLightSpcColorHandle(     int LHandle, COLOR_F Color ) ;													// ライトハンドルのライトのスペキュラカラーを設定する
extern	int			SetLightAmbColorHandle(     int LHandle, COLOR_F Color ) ;													// ライトハンドルのライトのアンビエントカラーを設定する
extern	int			SetLightDirectionHandle(    int LHandle, VECTOR Direction ) ;												// ライトハンドルのライトの方向を設定する
extern	int			SetLightPositionHandle(     int LHandle, VECTOR Position ) ;												// ライトハンドルのライトの位置を設定する
extern	int			SetLightRangeAttenHandle(   int LHandle, float Range, float Atten0, float Atten1, float Atten2 ) ;			// ライトハンドルのライトの距離減衰パラメータを設定する( 有効距離、距離減衰係数０、１、２ )
extern	int			SetLightAngleHandle(        int LHandle, float OutAngle, float InAngle ) ;									// ライトハンドルのライトのスポットライトのパラメータを設定する( 外部コーン角度、内部コーン角度 )
extern	int			SetLightUseShadowMapHandle(	int LHandle, int SmSlotIndex, int UseFlag ) ;									// ライトハンドルのライトに SetUseShadowMap で指定したシャドウマップを適用するかどうかを設定する( SmSlotIndex:シャドウマップスロット( SetUseShadowMap の第一引数に設定する値 ) UseFlag:適用にするかどうかのフラグ( TRUE:適用する( デフォルト )  FALSE:適用しない ) )
extern	int			GetLightTypeHandle(         int LHandle ) ;																	// ライトハンドルのライトのタイプを取得する( 戻り値は DX_LIGHTTYPE_DIRECTIONAL 等 )
extern	int			GetLightEnableHandle(       int LHandle ) ;																	// ライトハンドルのライト効果の有効、無効を取得する( TRUE:有効  FALSE:無効 )
extern	COLOR_F		GetLightDifColorHandle(     int LHandle ) ;																	// ライトハンドルのライトのディフューズカラーを取得する
extern	COLOR_F		GetLightSpcColorHandle(     int LHandle ) ;																	// ライトハンドルのライトのスペキュラカラーを取得する
extern	COLOR_F		GetLightAmbColorHandle(     int LHandle ) ;																	// ライトハンドルのライトのアンビエントカラーを取得する
extern	VECTOR		GetLightDirectionHandle(    int LHandle ) ;																	// ライトハンドルのライトの方向を取得する
extern	VECTOR		GetLightPositionHandle(     int LHandle ) ;																	// ライトハンドルのライトの位置を取得する
extern	int			GetLightRangeAttenHandle(   int LHandle, float *Range, float *Atten0, float *Atten1, float *Atten2 ) ;		// ライトハンドルのライトの距離減衰パラメータを取得する( 有効距離、距離減衰係数０、１、２ )
extern	int			GetLightAngleHandle(        int LHandle, float *OutAngle, float *InAngle ) ;								// ライトハンドルのライトのスポットライトのパラメータを取得する( 外部コーン角度、内部コーン角度 )

extern	int			GetEnableLightHandleNum(	void ) ;																		// 有効になっているライトハンドルの数を取得する
extern	int			GetEnableLightHandle(		int Index ) ;																	// 有効になっているライトハンドルを取得する

// 色情報取得用関数
extern	int			GetTexFormatIndex(			const IMAGEFORMATDESC *Format ) ;												// テクスチャフォーマットのインデックスを得る








// DxMask.cpp 関数プロトタイプ宣言

#ifndef DX_NON_MASK

// マスク関係 
extern	int			CreateMaskScreen(					void ) ;																						// マスクスクリーンを作成する
extern	int			DeleteMaskScreen(					void ) ;																						// マスクスクリーンを削除する
extern	int			DrawMaskToDirectData(				int x, int y, int Width, int Height, const void *MaskData , int TransMode ) ;					// マスクスクリーンにメモリ上のビットマップデータを転送する( MaskData は 1byte で 1dot 表す１ラインあたり Width byte のデータ配列の先頭アドレス )
extern	int			DrawFillMaskToDirectData(			int x1, int y1, int x2, int y2,  int Width, int Height, const void *MaskData ) ;				// マスクスクリーンにメモリ上のビットマップデータで指定矩形内を塗りつぶすように転送する

extern	int			SetUseMaskScreenFlag(				int ValidFlag ) ;																				// マスクスクリーンを使用するかどうかを設定する( ValidFlag:使用するかどうか( TRUE:使用する  FALSE:使用しない( デフォルト ) )
extern	int			GetUseMaskScreenFlag(				void ) ;																						// マスクスクリーンを使用するかどうかを取得する
extern	int			FillMaskScreen(						int Flag ) ;																					// マスクスクリーンを指定の色で塗りつぶす
extern	int			SetMaskScreenGraph(					int GraphHandle ) ;																				// マスクスクリーンとして使用するグラフィックのハンドルを設定する、-1を渡すと解除( 引数で渡すグラフィックハンドルは MakeScreen で作成した「アルファチャンネル付きの描画対象にできるグラフィックハンドル」である必要があります( アルファチャンネルがマスクに使用されます ) )
extern	int			SetMaskScreenGraphUseChannel(		int UseChannel /* DX_MASKGRAPH_CH_A 等 */ ) ;													// マスクスクリーンとして使用するグラフィックの、どのチャンネルをマスクとして使用するかを設定する( デフォルトは DX_MASKGRAPH_CH_A、 尚、DX_MASKGRAPH_CH_A以外を使用する場合はグラフィックスデバイスがシェーダーモデル2.0以降に対応している必要があります )

extern	int			InitMask(							void ) ;																						// マスクハンドルをすべて削除する
extern	int			MakeMask(							int Width, int Height ) ;																		// マスクハンドルを作成する
extern	int			GetMaskSize(						int *WidthBuf, int *HeightBuf, int MaskHandle ) ;												// マスクハンドルが持つマスクイメージのサイズを取得する
extern	int			SetDataToMask(						int Width, int Height, const void *MaskData, int MaskHandle ) ;									// マスクハンドルにビットマップイメージを転送する
extern	int			DeleteMask(							int MaskHandle ) ;																				// マスクハンドルを削除する
extern	int			GraphImageBltToMask(				const BASEIMAGE *BaseImage, int ImageX, int ImageY, int MaskHandle ) ;							// マスクハンドルにBASEIMAGEデータを転送する
extern	int			LoadMask(							const TCHAR *FileName                        ) ;																			// 画像ファイルを読み込みマスクハンドルを作成する
extern	int			LoadMaskWithStrLen(					const TCHAR *FileName, size_t FileNameLength ) ;																			// 画像ファイルを読み込みマスクハンドルを作成する
extern	int			LoadDivMask(						const TCHAR *FileName,                        int AllNum, int XNum, int YNum, int XSize, int YSize, int *HandleArray ) ;	// 画像ファイルを分割読み込みしてマスクハンドルを作成する
extern	int			LoadDivMaskWithStrLen(				const TCHAR *FileName, size_t FileNameLength, int AllNum, int XNum, int YNum, int XSize, int YSize, int *HandleArray ) ;	// 画像ファイルを分割読み込みしてマスクハンドルを作成する
extern	int			CreateMaskFromMem(					const void *FileImage, int FileImageSize ) ;																				// メモリ上にある画像ファイルイメージを読み込みマスクハンドルを作成する
extern	int			CreateDivMaskFromMem(				const void *FileImage, int FileImageSize, int AllNum, int XNum, int YNum, int XSize, int YSize, int *HandleArray ) ;		// メモリ上にある画像ファイルイメージを分割読み込みしてマスクハンドルを作成する
extern	int			DrawMask(							int x, int y, int MaskHandle, int TransMode ) ;													// マスクスクリーンにマスクハンドルの持つマスクイメージを書き込む
#ifndef DX_NON_FONT
extern	int			DrawFormatStringMask(				int x, int y, int Flag,                 const TCHAR *FormatString, ... ) ;						// 書式指定ありの文字列をマスクスクリーンに描画する
extern	int			DrawFormatStringMaskToHandle(		int x, int y, int Flag, int FontHandle, const TCHAR *FormatString, ... ) ;						// 書式指定ありの文字列をマスクスクリーンに描画する( フォントハンドル指定版 )( SetFontCacheToTextureFlag( FALSE ) ; にして作成したフォントハンドルのみ使用可能 )
extern	int			DrawStringMask(						int x, int y, int Flag,                 const TCHAR *String ) ;									// 文字列をマスクスクリーンに描画する
extern	int			DrawNStringMask(					int x, int y, int Flag,                 const TCHAR *String, size_t StringLength ) ;			// 文字列をマスクスクリーンに描画する
extern	int			DrawStringMaskToHandle(				int x, int y, int Flag, int FontHandle, const TCHAR *String ) ;									// 文字列をマスクスクリーンに描画する( フォントハンドル指定版 )( SetFontCacheToTextureFlag( FALSE ) ; にして作成したフォントハンドルのみ使用可能 )
extern	int			DrawNStringMaskToHandle(			int x, int y, int Flag, int FontHandle, const TCHAR *String, size_t StringLength ) ;			// 文字列をマスクスクリーンに描画する( フォントハンドル指定版 )( SetFontCacheToTextureFlag( FALSE ) ; にして作成したフォントハンドルのみ使用可能 )
#endif // DX_NON_FONT
extern	int			DrawFillMask(						int x1, int y1, int x2, int y2, int MaskHandle ) ;												// マスクハンドルが持つマスクイメージをマスクスクリーンいっぱいに描画する( タイル状に並べる )
extern	int			SetMaskReverseEffectFlag(			int ReverseFlag ) ;																				// マスクイメージ内の数値に対する効果を逆転させる( 旧バージョンとの互換性の為の機能 )

extern 	int			GetMaskScreenData(					int x1, int y1, int x2, int y2, int MaskHandle ) ;												// マスクスクリーンの指定矩形部分をマスクハンドルに転送する
extern	int			GetMaskUseFlag(						void ) ;																						// マスクスクリーンを使用する設定になっているかどうかを取得する

#endif // DX_NON_MASK

#endif // DX_NOTUSE_DRAWFUNCTION















// DxFont.cpp 関数プロトタイプ宣言

#ifndef DX_NON_FONT

// フォント、文字列描画関係関数
extern	int			EnumFontName(                           TCHAR *NameBuffer, int NameBufferNum, int JapanOnlyFlag = TRUE ) ;																				// 使用可能なフォントの名前を列挙する( NameBuffer に 64バイト区切りで名前が格納されます )
extern	int			EnumFontNameEx(                         TCHAR *NameBuffer, int NameBufferNum,                                                       int CharSet = -1 /* DX_CHARSET_DEFAULT 等 */ ) ;	// 使用可能なフォントの名前を列挙する( NameBuffer に 64バイト区切りで名前が格納されます )( 文字セット指定版 )
extern	int			EnumFontNameEx2(						TCHAR *NameBuffer, int NameBufferNum, const TCHAR *EnumFontName,                            int CharSet = -1 /* DX_CHARSET_DEFAULT 等 */ ) ;	// 指定のフォント名のフォントを列挙する
extern	int			EnumFontNameEx2WithStrLen(				TCHAR *NameBuffer, int NameBufferNum, const TCHAR *EnumFontName, size_t EnumFontNameLength, int CharSet = -1 /* DX_CHARSET_DEFAULT 等 */ ) ;	// 指定のフォント名のフォントを列挙する
extern	int			CheckFontName(							const TCHAR *FontName,                        int CharSet = -1 /* DX_CHARSET_DEFAULT 等 */ ) ;													// 指定のフォント名のフォントが存在するかどうかをチェックする( 戻り値  TRUE:存在する  FALSE:存在しない )
extern	int			CheckFontNameWithStrLen(				const TCHAR *FontName, size_t FontNameLength, int CharSet = -1 /* DX_CHARSET_DEFAULT 等 */ ) ;													// 指定のフォント名のフォントが存在するかどうかをチェックする( 戻り値  TRUE:存在する  FALSE:存在しない )

extern	int			InitFontToHandle(                       void ) ;																						// 全てのフォントハンドルを削除する

extern	int			CreateFontToHandle(						const TCHAR *FontName,                        int Size, int Thick, int FontType = -1 , int CharSet = -1 , int EdgeSize = -1 , int Italic = FALSE , int Handle = -1 ) ;		// フォントハンドルを作成する
extern	int			CreateFontToHandleWithStrLen(			const TCHAR *FontName, size_t FontNameLength, int Size, int Thick, int FontType = -1 , int CharSet = -1 , int EdgeSize = -1 , int Italic = FALSE , int Handle = -1 ) ;		// フォントハンドルを作成する
extern	int			LoadFontDataToHandle(					const TCHAR *FileName,                            int EdgeSize = 0 ) ;							// フォントデータファイルからフォントハンドルを作成する
extern	int			LoadFontDataToHandleWithStrLen(			const TCHAR *FileName, size_t FileNameLength,     int EdgeSize = 0 ) ;							// フォントデータファイルからフォントハンドルを作成する
extern	int			LoadFontDataFromMemToHandle(			const void *FontDataImage, int FontDataImageSize, int EdgeSize = 0 ) ;							// メモリ上のフォントデータファイルイメージからフォントハンドルを作成する
extern	int			SetFontSpaceToHandle(                   int Pixel, int FontHandle ) ;																	// フォントハンドルの字間を変更する
extern	int			SetFontLineSpaceToHandle(               int Pixel, int FontHandle ) ;																	// フォントハンドルの行間を変更する
extern	int			SetFontCharCodeFormatToHandle(			int CharCodeFormat /* DX_CHARCODEFORMAT_SHIFTJIS 等 */ , int FontHandle ) ;						// 指定のフォントハンドルを使用する関数の引数に渡す文字列の文字コード形式を設定する( UNICODE版では無効 )
extern	int			DeleteFontToHandle(                     int FontHandle ) ;																				// フォントハンドルを削除する
extern	int			SetFontLostFlag(                        int FontHandle, int *LostFlag ) ;																// フォントハンドルを削除した際に TRUE を代入する変数のアドレスを設定する
extern	int			AddFontImageToHandle(					int FontHandle, const TCHAR *Char,                    int GrHandle, int DrawX, int DrawY, int AddX ) ;	// 指定の文字の代わりに描画するグラフィックハンドルを登録する
extern	int			AddFontImageToHandleWithStrLen(			int FontHandle, const TCHAR *Char, size_t CharLength, int GrHandle, int DrawX, int DrawY, int AddX ) ;	// 指定の文字の代わりに描画するグラフィックハンドルを登録する
extern	int			SubFontImageToHandle(					int FontHandle, const TCHAR *Char                    ) ;												// 指定の文字の代わりに描画するグラフィックハンドルの登録を解除する
extern	int			SubFontImageToHandleWithStrLen(			int FontHandle, const TCHAR *Char, size_t CharLength ) ;												// 指定の文字の代わりに描画するグラフィックハンドルの登録を解除する
extern	int			AddSubstitutionFontToHandle(			int FontHandle, int SubstitutionFontHandle, int DrawX, int DrawY ) ;							// 代替フォントハンドル( FontHandle に無い文字を描画しようとしたときに代わりに使用されるフォントハンドル )を登録する
extern	int			SubSubstitutionFontToHandle(			int FontHandle, int SubstitutionFontHandle ) ;													// 代替フォントハンドルの登録を解除する

extern	int			ChangeFont(                             const TCHAR *FontName,                        int CharSet = -1 /* DX_CHARSET_SHFTJIS 等 */ ) ;	// デフォルトフォントハンドルで使用するフォントを変更
extern	int			ChangeFontWithStrLen(                   const TCHAR *FontName, size_t FontNameLength, int CharSet = -1 /* DX_CHARSET_SHFTJIS 等 */ ) ;	// デフォルトフォントハンドルで使用するフォントを変更
extern	int			ChangeFontType(                         int FontType ) ;																				// デフォルトフォントハンドルのフォントタイプの変更
extern	const TCHAR *GetFontName(							void ) ;																						// デフォルトフォントハンドルのフォント名を取得する
extern	int			SetFontSize(                            int FontSize ) ;																				// デフォルトフォントハンドルのサイズを設定する
extern	int			GetFontSize(                            void ) ;																						// デフォルトフォントハンドルのサイズを取得する
extern	int			GetFontEdgeSize(                        void ) ;																						// デフォルトフォントハンドルの縁サイズを取得する
extern	int			SetFontThickness(                       int ThickPal ) ;																				// デフォルトフォントハンドルの太さを設定する
extern	int			SetFontSpace(                           int Pixel ) ;																					// デフォルトフォントハンドルの字間を変更する
extern	int			GetFontSpace(                           void ) ;																						// デフォルトフォントハンドルの字間を取得する
extern	int			SetFontLineSpace(                       int Pixel ) ;																					// デフォルトフォントハンドルの行間を変更する
extern	int			GetFontLineSpace(                       void ) ;																						// デフォルトフォントハンドルの行間を取得する
extern	int			SetFontCharCodeFormat(					int CharCodeFormat /* DX_CHARCODEFORMAT_SHIFTJIS 等 */ ) ;										// デフォルトフォントハンドルを使用する関数の引数に渡す文字列の文字コード形式を設定する( UNICODE版では無効 )
extern	int			SetDefaultFontState(                    const TCHAR *FontName,                        int Size, int Thick, int FontType = -1 , int CharSet = -1 , int EdgeSize = -1 , int Italic = FALSE ) ;	// デフォルトフォントハンドルの設定を変更する
extern	int			SetDefaultFontStateWithStrLen(          const TCHAR *FontName, size_t FontNameLength, int Size, int Thick, int FontType = -1 , int CharSet = -1 , int EdgeSize = -1 , int Italic = FALSE ) ;	// デフォルトフォントハンドルの設定を変更する
extern	int			GetDefaultFontHandle(                   void ) ;																								// デフォルトフォントハンドルを取得する
extern	int			GetFontMaxWidth(                        void ) ;																								// デフォルトフォントハンドルの文字の最大幅を取得する
extern	int			GetFontAscent(                          void ) ;																								// デフォルトフォントハンドルの描画位置からベースラインまでの高さを取得する
extern	int			GetDrawStringWidth(                     const TCHAR *String, int StrLen,          int VerticalFlag = FALSE ) ;									// デフォルトフォントハンドルを使用した文字列の描画幅を取得する
extern	int			GetDrawNStringWidth(                    const TCHAR *String, size_t StringLength, int VerticalFlag = FALSE ) ;									// デフォルトフォントハンドルを使用した文字列の描画幅を取得する
extern	int			GetDrawFormatStringWidth(               const TCHAR *FormatString, ... ) ;																		// デフォルトフォントハンドルを使用した書式付き文字列の描画幅を取得する
extern	int			GetDrawExtendStringWidth(               double ExRateX, const TCHAR *String, int StrLen,          int VerticalFlag = FALSE ) ;					// デフォルトフォントハンドルを使用した文字列の描画幅を取得する( 拡大率付き )
extern	int			GetDrawExtendNStringWidth(              double ExRateX, const TCHAR *String, size_t StringLength, int VerticalFlag = FALSE ) ;					// デフォルトフォントハンドルを使用した文字列の描画幅を取得する( 拡大率付き )
extern	int			GetDrawExtendFormatStringWidth(         double ExRateX, const TCHAR *FormatString, ... ) ;														// デフォルトフォントハンドルを使用した書式付き文字列の描画幅を取得する( 拡大率付き )
extern	int			GetDrawStringSize(                      int *SizeX, int *SizeY, int *LineCount, const TCHAR *String, int StrLen,          int VerticalFlag = FALSE ) ;											// デフォルトフォントハンドルを使用した文字列の描画幅・高さ・行数を取得する
extern	int			GetDrawNStringSize(                     int *SizeX, int *SizeY, int *LineCount, const TCHAR *String, size_t StringLength, int VerticalFlag = FALSE ) ;											// デフォルトフォントハンドルを使用した文字列の描画幅・高さ・行数を取得する
extern	int			GetDrawFormatStringSize(                int *SizeX, int *SizeY, int *LineCount, const TCHAR *FormatString, ... ) ;																				// デフォルトフォントハンドルを使用した書式付き文字列の描画幅・高さ・行数を取得する
extern	int			GetDrawExtendStringSize(                int *SizeX, int *SizeY, int *LineCount, double ExRateX, double ExRateY, const TCHAR *String, int StrLen,          int VerticalFlag = FALSE ) ;			// デフォルトフォントハンドルを使用した文字列の描画幅・高さ・行数を取得する( 拡大率付き )
extern	int			GetDrawExtendNStringSize(               int *SizeX, int *SizeY, int *LineCount, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, int VerticalFlag = FALSE ) ;			// デフォルトフォントハンドルを使用した文字列の描画幅・高さ・行数を取得する( 拡大率付き )
extern	int			GetDrawExtendFormatStringSize(          int *SizeX, int *SizeY, int *LineCount, double ExRateX, double ExRateY, const TCHAR *FormatString, ... ) ;												// デフォルトフォントハンドルを使用した書式付き文字列の描画幅・高さ・行数を取得する( 拡大率付き )
extern	int			GetDrawStringCharInfo(                  DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, const TCHAR *String, int StrLen,          int VerticalFlag = FALSE ) ;									// デフォルトフォントハンドルを使用した文字列の１文字毎の情報を取得する
extern	int			GetDrawNStringCharInfo(                 DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, const TCHAR *String, size_t StringLength, int VerticalFlag = FALSE ) ;									// デフォルトフォントハンドルを使用した文字列の１文字毎の情報を取得する
extern	int			GetDrawFormatStringCharInfo(            DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, const TCHAR *FormatString, ... ) ;																		// デフォルトフォントハンドルを使用した書式付き文字列の１文字毎の情報を取得する
extern	int			GetDrawExtendStringCharInfo(            DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, double ExRateX, double ExRateY, const TCHAR *String, int StrLen,          int VerticalFlag = FALSE ) ;	// デフォルトフォントハンドルを使用した文字列の１文字毎の情報を取得する
extern	int			GetDrawExtendNStringCharInfo(           DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, int VerticalFlag = FALSE ) ;	// デフォルトフォントハンドルを使用した文字列の１文字毎の情報を取得する
extern	int			GetDrawExtendFormatStringCharInfo(      DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, double ExRateX, double ExRateY, const TCHAR *FormatString, ... ) ;										// デフォルトフォントハンドルを使用した書式付き文字列の１文字毎の情報を取得する

extern	const TCHAR *GetFontNameToHandle(					int FontHandle ) ;																				// フォントハンドルのフォント名を取得する
extern	int			GetFontMaxWidthToHandle(                int FontHandle ) ;																				// フォントハンドルの文字の最大幅を取得する
extern	int			GetFontAscentToHandle(                  int FontHandle ) ;																				// フォントハンドルの描画位置からベースラインまでの高さを取得する
extern	int			GetFontSizeToHandle(                    int FontHandle ) ;																				// フォントハンドルのサイズを取得する
extern	int			GetFontEdgeSizeToHandle(                int FontHandle ) ;																				// フォントハンドルの縁サイズを取得する
extern	int			GetFontSpaceToHandle(                   int FontHandle ) ;																				// フォントハンドルの字間を取得する
extern	int			GetFontLineSpaceToHandle(               int FontHandle ) ;																				// フォントハンドルの行間を取得する
extern	int			GetFontCharInfo(                        int FontHandle, const TCHAR *Char,                    int *DrawX, int *DrawY, int *NextCharX, int *SizeX, int *SizeY ) ;	// フォントハンドルの指定の文字の描画情報を取得する
extern	int			GetFontCharInfoWithStrLen(              int FontHandle, const TCHAR *Char, size_t CharLength, int *DrawX, int *DrawY, int *NextCharX, int *SizeX, int *SizeY ) ;	// フォントハンドルの指定の文字の描画情報を取得する
extern	int			GetDrawStringWidthToHandle(             const TCHAR   *String, int StrLen,          int FontHandle, int VerticalFlag = FALSE ) ;						// フォントハンドルを使用した文字列の描画幅を取得する
extern	int			GetDrawNStringWidthToHandle(            const TCHAR   *String, size_t StringLength, int FontHandle, int VerticalFlag = FALSE ) ;						// フォントハンドルを使用した文字列の描画幅を取得する
extern	int			GetDrawFormatStringWidthToHandle(       int FontHandle, const TCHAR *FormatString, ... ) ;																// フォントハンドルを使用した書式付き文字列の描画幅を取得する
extern	int			GetDrawExtendStringWidthToHandle(       double ExRateX, const TCHAR *String, int StrLen,          int FontHandle, int VerticalFlag = FALSE ) ;			// フォントハンドルを使用した文字列の描画幅を取得する
extern	int			GetDrawExtendNStringWidthToHandle(      double ExRateX, const TCHAR *String, size_t StringLength, int FontHandle, int VerticalFlag = FALSE ) ;			// フォントハンドルを使用した文字列の描画幅を取得する
extern	int			GetDrawExtendFormatStringWidthToHandle( double ExRateX, int FontHandle, const TCHAR *FormatString, ... ) ;												// フォントハンドルを使用した書式付き文字列の描画幅を取得する
extern	int			GetDrawStringSizeToHandle(              int *SizeX, int *SizeY, int *LineCount, const TCHAR   *String, int StrLen,          int FontHandle, int VerticalFlag = FALSE ) ;											// フォントハンドルを使用した文字列の描画幅・高さ・行数を取得する
extern	int			GetDrawNStringSizeToHandle(             int *SizeX, int *SizeY, int *LineCount, const TCHAR   *String, size_t StringLength, int FontHandle, int VerticalFlag = FALSE ) ;											// フォントハンドルを使用した文字列の描画幅・高さ・行数を取得する
extern	int			GetDrawFormatStringSizeToHandle(        int *SizeX, int *SizeY, int *LineCount, int FontHandle, const TCHAR *FormatString, ... ) ;																					// フォントハンドルを使用した書式付き文字列の描画幅・高さ・行数を取得する
extern	int			GetDrawExtendStringSizeToHandle(        int *SizeX, int *SizeY, int *LineCount, double ExRateX, double ExRateY, const TCHAR *String, int StrLen,          int FontHandle, int VerticalFlag = FALSE ) ;				// フォントハンドルを使用した文字列の描画幅・高さ・行数を取得する
extern	int			GetDrawExtendNStringSizeToHandle(       int *SizeX, int *SizeY, int *LineCount, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, int FontHandle, int VerticalFlag = FALSE ) ;				// フォントハンドルを使用した文字列の描画幅・高さ・行数を取得する
extern	int			GetDrawExtendFormatStringSizeToHandle(  int *SizeX, int *SizeY, int *LineCount, double ExRateX, double ExRateY, int FontHandle, const TCHAR *FormatString, ... ) ;													// フォントハンドルを使用した書式付き文字列の描画幅・高さ・行数を取得する
extern	int			GetDrawStringCharInfoToHandle(              DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, const TCHAR *String, int StrLen,          int FontHandle, int VerticalFlag = FALSE ) ;									// フォントハンドルを使用した文字列の１文字毎の情報を取得する
extern	int			GetDrawNStringCharInfoToHandle(             DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, const TCHAR *String, size_t StringLength, int FontHandle, int VerticalFlag = FALSE ) ;									// フォントハンドルを使用した文字列の１文字毎の情報を取得する
extern	int			GetDrawFormatStringCharInfoToHandle(        DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, int FontHandle, const TCHAR *FormatString, ... ) ;																		// フォントハンドルを使用した書式付き文字列の１文字毎の情報を取得する
extern	int			GetDrawExtendStringCharInfoToHandle(        DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, double ExRateX, double ExRateY, const TCHAR *String, int StrLen,          int FontHandle, int VerticalFlag = FALSE ) ;	// フォントハンドルを使用した文字列の１文字毎の情報を取得する
extern	int			GetDrawExtendNStringCharInfoToHandle(       DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, int FontHandle, int VerticalFlag = FALSE ) ;	// フォントハンドルを使用した文字列の１文字毎の情報を取得する
extern	int			GetDrawExtendFormatStringCharInfoToHandle(  DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, double ExRateX, double ExRateY, int FontHandle, const TCHAR *FormatString, ... ) ;										// フォントハンドルを使用した書式付き文字列の１文字毎の情報を取得する
extern	int			GetFontStateToHandle(                   TCHAR   *FontName, int *Size, int *Thick, int FontHandle, int *FontType = NULL , int *CharSet = NULL , int *EdgeSize = NULL , int *Italic = NULL ) ;						// フォントハンドルの情報を取得する
extern	int			CheckFontCacheToTextureFlag(            int FontHandle ) ;																				// フォントハンドルがテクスチャキャッシュを使用しているかどうかを取得する
extern	int			CheckFontChacheToTextureFlag(           int FontHandle ) ;																				// CheckFontCacheToTextureFlag の誤字版
extern	int			CheckFontHandleValid(                   int FontHandle ) ;																				// フォントハンドルが有効かどうかを取得する
extern	int			ClearFontCacheToHandle(					int FontHandle ) ;																				// フォントハンドルのキャッシュ情報を初期化する

extern	int			SetFontCacheToTextureFlag(              int Flag ) ;																					// フォントのキャッシュにテクスチャを使用するかどうかを設定する( TRUE:テクスチャを使用する( デフォルト )  FALSE:テクスチャは使用しない )
extern	int			GetFontCacheToTextureFlag(              void ) ;																						// フォントのキャッシュにテクスチャを使用するかどうかを設定する
extern	int			SetFontChacheToTextureFlag(             int Flag ) ;																					// SetFontCacheToTextureFlag の誤字版
extern	int			GetFontChacheToTextureFlag(             void ) ;																						// GetFontCacheToTextureFlag の誤字版
extern	int			SetFontCacheTextureColorBitDepth(		int ColorBitDepth ) ;																			// フォントのキャッシュとして使用するテクスチャのカラービット深度を設定する( 16 又は 32 のみ指定可能  デフォルトは 32 )
extern	int			GetFontCacheTextureColorBitDepth(		void ) ;																						// フォントのキャッシュとして使用するテクスチャのカラービット深度を取得する
extern	int			SetFontCacheCharNum(                    int CharNum ) ;																					// フォントキャッシュでキャッシュできる文字数を設定する
extern	int			GetFontCacheCharNum(                    void ) ;																						// フォントキャッシュでキャッシュできる文字数を取得する( 戻り値  0:デフォルト  1以上:指定文字数 )
extern	int			SetFontCacheUsePremulAlphaFlag(         int Flag ) ;																					// フォントキャッシュとして保存する画像の形式を乗算済みαチャンネル付き画像にするかどうかを設定する( TRUE:乗算済みαを使用する  FLASE:乗算済みαを使用しない( デフォルト ) )
extern	int			GetFontCacheUsePremulAlphaFlag(         void ) ;																						// フォントキャッシュとして保存する画像の形式を乗算済みαチャンネル付き画像にするかどうかを取得する
extern	int			SetFontUseAdjustSizeFlag(               int Flag ) ;																					// フォントのサイズを補正する処理を行うかどうかを設定する( Flag  TRUE:行う( デフォルト )  FALSE:行わない )
extern	int			GetFontUseAdjustSizeFlag(               void ) ;																						// フォントのサイズを補正する処理を行うかどうかを取得する


// FontCacheStringDraw の代わりに DrawString を使ってください
extern	int			FontCacheStringDrawToHandle(            int x, int y, const TCHAR *StrData,                       unsigned int Color, unsigned int EdgeColor, BASEIMAGE *DestImage, const RECT *ClipRect /* NULL 可 */ , int FontHandle, int VerticalFlag = FALSE , SIZE *DrawSizeP = NULL ) ;
extern	int			FontCacheStringDrawToHandleWithStrLen(  int x, int y, const TCHAR *StrData, size_t StrDataLength, unsigned int Color, unsigned int EdgeColor, BASEIMAGE *DestImage, const RECT *ClipRect /* NULL 可 */ , int FontHandle, int VerticalFlag = FALSE , SIZE *DrawSizeP = NULL ) ;
extern	int			FontBaseImageBlt(                       int x, int y, const TCHAR *StrData,                       BASEIMAGE *DestImage, BASEIMAGE *DestEdgeImage,                 int VerticalFlag = FALSE ) ;	// 基本イメージに文字列を描画する( デフォルトフォントハンドルを使用する )
extern	int			FontBaseImageBltWithStrLen(             int x, int y, const TCHAR *StrData, size_t StrDataLength, BASEIMAGE *DestImage, BASEIMAGE *DestEdgeImage,                 int VerticalFlag = FALSE ) ;	// 基本イメージに文字列を描画する( デフォルトフォントハンドルを使用する )
extern	int			FontBaseImageBltToHandle(               int x, int y, const TCHAR *StrData,                       BASEIMAGE *DestImage, BASEIMAGE *DestEdgeImage, int FontHandle, int VerticalFlag = FALSE ) ;	// 基本イメージに文字列を描画する
extern	int			FontBaseImageBltToHandleWithStrLen(     int x, int y, const TCHAR *StrData, size_t StrDataLength, BASEIMAGE *DestImage, BASEIMAGE *DestEdgeImage, int FontHandle, int VerticalFlag = FALSE ) ;	// 基本イメージに文字列を描画する

extern	int			MultiByteCharCheck(                     const char *Buf, int CharSet /* DX_CHARSET_SHFTJIS */ ) ;										// ２バイト文字か調べる( TRUE:２バイト文字  FALSE:１バイト文字 )

// 文字列描画関数
extern	int			DrawString(                             int x, int y,                                              const TCHAR *String,                      unsigned int Color, unsigned int EdgeColor = 0 ) ;							// デフォルトフォントハンドルを使用して文字列を描画する
extern	int			DrawNString(                            int x, int y,                                              const TCHAR *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor = 0 ) ;							// デフォルトフォントハンドルを使用して文字列を描画する
extern	int			DrawVString(                            int x, int y,                                              const TCHAR *String,                      unsigned int Color, unsigned int EdgeColor = 0 ) ;							// デフォルトフォントハンドルを使用して文字列を描画する( 縦書き )
extern	int			DrawNVString(                           int x, int y,                                              const TCHAR *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor = 0 ) ;							// デフォルトフォントハンドルを使用して文字列を描画する( 縦書き )
extern	int			DrawFormatString(                       int x, int y,                                 unsigned int Color, const TCHAR *FormatString, ... ) ;																			// デフォルトフォントハンドルを使用して書式指定文字列を描画する
extern	int			DrawFormatVString(                      int x, int y,                                 unsigned int Color, const TCHAR *FormatString, ... ) ;																			// デフォルトフォントハンドルを使用して書式指定文字列を描画する( 縦書き )
extern	int			DrawExtendString(                       int x, int y, double ExRateX, double ExRateY,              const TCHAR *String,                      unsigned int Color, unsigned int EdgeColor = 0 ) ;							// デフォルトフォントハンドルを使用して文字列の拡大描画
extern	int			DrawExtendNString(                      int x, int y, double ExRateX, double ExRateY,              const TCHAR *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor = 0 ) ;							// デフォルトフォントハンドルを使用して文字列の拡大描画
extern	int			DrawExtendVString(                      int x, int y, double ExRateX, double ExRateY,              const TCHAR *String,                      unsigned int Color, unsigned int EdgeColor = 0 ) ;							// デフォルトフォントハンドルを使用して文字列の拡大描画( 縦書き )
extern	int			DrawExtendNVString(                     int x, int y, double ExRateX, double ExRateY,              const TCHAR *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor = 0 ) ;							// デフォルトフォントハンドルを使用して文字列の拡大描画( 縦書き )
extern	int			DrawExtendFormatString(                 int x, int y, double ExRateX, double ExRateY, unsigned int Color, const TCHAR *FormatString, ... ) ;																			// デフォルトフォントハンドルを使用して書式指定文字列を拡大描画する
extern	int			DrawExtendFormatVString(                int x, int y, double ExRateX, double ExRateY, unsigned int Color, const TCHAR *FormatString, ... ) ;																			// デフォルトフォントハンドルを使用して書式指定文字列を拡大描画する( 縦書き )
extern	int			DrawRotaString(							int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE , const TCHAR *String       = NULL                           ) ;		// デフォルトフォントハンドルを使用して文字列を回転描画する
extern	int			DrawRotaNString(						int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE , const TCHAR *String       = NULL , size_t StringLength = 0 ) ;		// デフォルトフォントハンドルを使用して文字列を回転描画する
extern	int			DrawRotaFormatString(					int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE , const TCHAR *FormatString = NULL , ...                     ) ;		// デフォルトフォントハンドルを使用して書式指定文字列を回転描画する
extern	int			DrawModiString(							int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int Color, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE , const TCHAR *String       = NULL                           ) ;		// デフォルトフォントハンドルを使用して文字列を変形描画する
extern	int			DrawModiNString(						int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int Color, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE , const TCHAR *String       = NULL , size_t StringLength = 0 ) ;		// デフォルトフォントハンドルを使用して文字列を変形描画する
extern	int			DrawModiFormatString(					int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int Color, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE , const TCHAR *FormatString = NULL , ...                     ) ;		// デフォルトフォントハンドルを使用して書式指定文字列を変形描画する

extern	int			DrawStringF(                            float x, float y,                                              const TCHAR *String,                      unsigned int Color, unsigned int EdgeColor = 0 ) ;						// デフォルトフォントハンドルを使用して文字列を描画する( 座標指定が float 版 )
extern	int			DrawNStringF(                           float x, float y,                                              const TCHAR *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor = 0 ) ;						// デフォルトフォントハンドルを使用して文字列を描画する( 座標指定が float 版 )
extern	int			DrawVStringF(                           float x, float y,                                              const TCHAR *String,                      unsigned int Color, unsigned int EdgeColor = 0 ) ;						// デフォルトフォントハンドルを使用して文字列を描画する( 縦書き )( 座標指定が float 版 )
extern	int			DrawNVStringF(                          float x, float y,                                              const TCHAR *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor = 0 ) ;						// デフォルトフォントハンドルを使用して文字列を描画する( 縦書き )( 座標指定が float 版 )
extern	int			DrawFormatStringF(                      float x, float y,                                 unsigned int Color, const TCHAR *FormatString, ... ) ;																		// デフォルトフォントハンドルを使用して書式指定文字列を描画する( 座標指定が float 版 )
extern	int			DrawFormatVStringF(                     float x, float y,                                 unsigned int Color, const TCHAR *FormatString, ... ) ;																		// デフォルトフォントハンドルを使用して書式指定文字列を描画する( 縦書き )( 座標指定が float 版 )
extern	int			DrawExtendStringF(                      float x, float y, double ExRateX, double ExRateY,              const TCHAR *String,                      unsigned int Color, unsigned int EdgeColor = 0 ) ;						// デフォルトフォントハンドルを使用して文字列の拡大描画( 座標指定が float 版 )
extern	int			DrawExtendNStringF(                     float x, float y, double ExRateX, double ExRateY,              const TCHAR *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor = 0 ) ;						// デフォルトフォントハンドルを使用して文字列の拡大描画( 座標指定が float 版 )
extern	int			DrawExtendVStringF(                     float x, float y, double ExRateX, double ExRateY,              const TCHAR *String,                      unsigned int Color, unsigned int EdgeColor = 0 ) ;						// デフォルトフォントハンドルを使用して文字列の拡大描画( 縦書き )( 座標指定が float 版 )
extern	int			DrawExtendNVStringF(                    float x, float y, double ExRateX, double ExRateY,              const TCHAR *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor = 0 ) ;						// デフォルトフォントハンドルを使用して文字列の拡大描画( 縦書き )( 座標指定が float 版 )
extern	int			DrawExtendFormatStringF(                float x, float y, double ExRateX, double ExRateY, unsigned int Color, const TCHAR *FormatString, ... ) ;																		// デフォルトフォントハンドルを使用して書式指定文字列を拡大描画する( 座標指定が float 版 )
extern	int			DrawExtendFormatVStringF(               float x, float y, double ExRateX, double ExRateY, unsigned int Color, const TCHAR *FormatString, ... ) ;																		// デフォルトフォントハンドルを使用して書式指定文字列を拡大描画する( 縦書き )( 座標指定が float 版 )
extern	int			DrawRotaStringF(						float x, float y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE , const TCHAR *String       = NULL                           ) ;		// デフォルトフォントハンドルを使用して文字列を回転描画する( 座標指定が float 版 )
extern	int			DrawRotaNStringF(						float x, float y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE , const TCHAR *String       = NULL , size_t StringLength = 0 ) ;		// デフォルトフォントハンドルを使用して文字列を回転描画する( 座標指定が float 版 )
extern	int			DrawRotaFormatStringF(					float x, float y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE , const TCHAR *FormatString = NULL , ...                     ) ;		// デフォルトフォントハンドルを使用して書式指定文字列を回転描画する( 座標指定が float 版 )
extern	int			DrawModiStringF(						float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int Color, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE , const TCHAR *String       = NULL                           ) ;		// デフォルトフォントハンドルを使用して文字列を変形描画する( 座標指定が float 版 )
extern	int			DrawModiNStringF(						float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int Color, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE , const TCHAR *String       = NULL , size_t StringLength = 0 ) ;		// デフォルトフォントハンドルを使用して文字列を変形描画する( 座標指定が float 版 )
extern	int			DrawModiFormatStringF(					float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int Color, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE , const TCHAR *FormatString = NULL , ...                     ) ;		// デフォルトフォントハンドルを使用して書式指定文字列を変形描画する( 座標指定が float 版 )

extern	int			DrawNumberToI(                          int x, int y,                          int    Num, int RisesNum, unsigned int Color, unsigned int EdgeColor = 0 ) ;											// デフォルトフォントハンドルを使用して整数型の数値を描画する
extern	int			DrawNumberToF(                          int x, int y,                          double Num, int Length,   unsigned int Color, unsigned int EdgeColor = 0 ) ;											// デフォルトフォントハンドルを使用して浮動小数点型の数値を描画する
extern	int			DrawNumberPlusToI(                      int x, int y, const TCHAR *NoteString, int    Num, int RisesNum, unsigned int Color, unsigned int EdgeColor = 0 ) ;											// デフォルトフォントハンドルを使用して整数型の数値とその説明の文字列を一度に描画する
extern 	int			DrawNumberPlusToF(                      int x, int y, const TCHAR *NoteString, double Num, int Length,   unsigned int Color, unsigned int EdgeColor = 0 ) ;											// デフォルトフォントハンドルを使用して浮動小数点型の数値とその説明の文字列を一度に描画する

extern	int			DrawStringToZBuffer(                    int x, int y, const TCHAR *String,                                                                                        int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;												// デフォルトフォントハンドルを使用してＺバッファに対して文字列を描画する
extern	int			DrawNStringToZBuffer(                   int x, int y, const TCHAR *String, size_t StringLength,                                                                   int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;												// デフォルトフォントハンドルを使用してＺバッファに対して文字列を描画する
extern	int			DrawVStringToZBuffer(                   int x, int y, const TCHAR *String,                                                                                        int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;												// デフォルトフォントハンドルを使用してＺバッファに対して文字列を描画する( 縦書き )
extern	int			DrawNVStringToZBuffer(                  int x, int y, const TCHAR *String, size_t StringLength,                                                                   int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;												// デフォルトフォントハンドルを使用してＺバッファに対して文字列を描画する( 縦書き )
extern	int			DrawFormatStringToZBuffer(              int x, int y,                                                                                        int WriteZMode /* DX_ZWRITE_MASK 等 */ , const TCHAR *FormatString, ... ) ;									// デフォルトフォントハンドルを使用してＺバッファに対して書式指定文字列を描画する
extern	int			DrawFormatVStringToZBuffer(             int x, int y,                                                                                        int WriteZMode /* DX_ZWRITE_MASK 等 */ , const TCHAR *FormatString, ... ) ;									// デフォルトフォントハンドルを使用してＺバッファに対して書式指定文字列を描画する( 縦書き )
extern	int			DrawExtendStringToZBuffer(              int x, int y, double ExRateX, double ExRateY, const TCHAR *String,                                                        int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;												// デフォルトフォントハンドルを使用してＺバッファに対して文字列を拡大描画する
extern	int			DrawExtendNStringToZBuffer(             int x, int y, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength,                                   int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;												// デフォルトフォントハンドルを使用してＺバッファに対して文字列を拡大描画する
extern	int			DrawExtendVStringToZBuffer(             int x, int y, double ExRateX, double ExRateY, const TCHAR *String,                                                        int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;												// デフォルトフォントハンドルを使用してＺバッファに対して文字列を拡大描画する( 縦書き )
extern	int			DrawExtendNVStringToZBuffer(            int x, int y, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength,                                   int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;												// デフォルトフォントハンドルを使用してＺバッファに対して文字列を拡大描画する( 縦書き )
extern	int			DrawExtendFormatStringToZBuffer(        int x, int y, double ExRateX, double ExRateY,                                                        int WriteZMode /* DX_ZWRITE_MASK 等 */ , const TCHAR *FormatString, ... ) ;									// デフォルトフォントハンドルを使用してＺバッファに対して書式指定文字列を拡大描画する
extern	int			DrawExtendFormatVStringToZBuffer(       int x, int y, double ExRateX, double ExRateY,                                                        int WriteZMode /* DX_ZWRITE_MASK 等 */ , const TCHAR *FormatString, ... ) ;									// デフォルトフォントハンドルを使用してＺバッファに対して書式指定文字列を拡大描画する( 縦書き )
extern	int			DrawRotaStringToZBuffer(				int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag , const TCHAR *String                            ) ;	// デフォルトフォントハンドルを使用してＺバッファに対して文字列を回転描画する
extern	int			DrawRotaNStringToZBuffer(				int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag , const TCHAR *String,       size_t StringLength ) ;	// デフォルトフォントハンドルを使用してＺバッファに対して文字列を回転描画する
extern	int			DrawRotaFormatStringToZBuffer(			int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag , const TCHAR *FormatString , ...                ) ;	// デフォルトフォントハンドルを使用してＺバッファに対して書式指定文字列を回転描画する
extern	int			DrawModiStringToZBuffer(				int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4,                                      int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag , const TCHAR *String                            ) ;	// デフォルトフォントハンドルを使用してＺバッファに対して文字列を変形描画する
extern	int			DrawModiNStringToZBuffer(				int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4,                                      int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag , const TCHAR *String,       size_t StringLength ) ;	// デフォルトフォントハンドルを使用してＺバッファに対して文字列を変形描画する
extern	int			DrawModiFormatStringToZBuffer(			int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4,                                      int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag , const TCHAR *FormatString , ...                ) ;	// デフォルトフォントハンドルを使用してＺバッファに対して書式指定文字列を変形描画する


extern	int			DrawStringToHandle(                     int x, int y, const TCHAR *String,                      unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE ) ;											// フォントハンドルを使用して文字列を描画する
extern	int			DrawNStringToHandle(                    int x, int y, const TCHAR *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE ) ;											// フォントハンドルを使用して文字列を描画する
extern	int			DrawVStringToHandle(                    int x, int y, const TCHAR *String,                      unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 ) ;																		// フォントハンドルを使用して文字列を描画する( 縦書き )
extern	int			DrawNVStringToHandle(                   int x, int y, const TCHAR *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 ) ;																		// フォントハンドルを使用して文字列を描画する( 縦書き )
extern	int			DrawFormatStringToHandle(               int x, int y, unsigned int Color, int FontHandle, const TCHAR *FormatString, ... ) ;																											// フォントハンドルを使用して書式指定文字列を描画する
extern	int			DrawFormatVStringToHandle(              int x, int y, unsigned int Color, int FontHandle, const TCHAR *FormatString, ... ) ;																											// フォントハンドルを使用して書式指定文字列を描画する( 縦書き )
extern	int			DrawExtendStringToHandle(               int x, int y, double ExRateX, double ExRateY, const TCHAR *String,                      unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE ) ;			// フォントハンドルを使用して文字列を拡大描画する
extern	int			DrawExtendNStringToHandle(              int x, int y, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE ) ;			// フォントハンドルを使用して文字列を拡大描画する
extern	int			DrawExtendVStringToHandle(              int x, int y, double ExRateX, double ExRateY, const TCHAR *String,                      unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 ) ;										// フォントハンドルを使用して文字列を拡大描画する( 縦書き )
extern	int			DrawExtendNVStringToHandle(             int x, int y, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 ) ;										// フォントハンドルを使用して文字列を拡大描画する( 縦書き )
extern	int			DrawExtendFormatStringToHandle(         int x, int y, double ExRateX, double ExRateY, unsigned int Color, int FontHandle, const TCHAR *FormatString, ... ) ;																			// フォントハンドルを使用して書式指定文字列を拡大描画する
extern	int			DrawExtendFormatVStringToHandle(        int x, int y, double ExRateX, double ExRateY, unsigned int Color, int FontHandle, const TCHAR *FormatString, ... ) ;																			// フォントハンドルを使用して書式指定文字列を拡大描画する( 縦書き )
extern	int			DrawRotaStringToHandle(					int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag, const TCHAR *String                            ) ;		// フォントハンドルを使用して文字列を回転描画する
extern	int			DrawRotaNStringToHandle(				int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag, const TCHAR *String,       size_t StringLength ) ;		// フォントハンドルを使用して文字列を回転描画する
extern	int			DrawRotaFormatStringToHandle(			int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag, const TCHAR *FormatString, ...                 ) ;		// フォントハンドルを使用して書式指定文字列を回転描画する
extern	int			DrawModiStringToHandle(					int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag, const TCHAR *String                            ) ;	// フォントハンドルを使用して文字列を変形描画する
extern	int			DrawModiNStringToHandle(				int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag, const TCHAR *String,       size_t StringLength ) ;	// フォントハンドルを使用して文字列を変形描画する
extern	int			DrawModiFormatStringToHandle(			int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag, const TCHAR *FormatString, ...                 ) ;	// フォントハンドルを使用して書式指定文字列を変形描画する

extern	int			DrawStringFToHandle(                    float x, float y, const TCHAR *String,                      unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE ) ;										// フォントハンドルを使用して文字列を描画する( 座標指定が float 版 )
extern	int			DrawNStringFToHandle(                   float x, float y, const TCHAR *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE ) ;										// フォントハンドルを使用して文字列を描画する( 座標指定が float 版 )
extern	int			DrawVStringFToHandle(                   float x, float y, const TCHAR *String,                      unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 ) ;																	// フォントハンドルを使用して文字列を描画する( 縦書き )( 座標指定が float 版 )
extern	int			DrawNVStringFToHandle(                  float x, float y, const TCHAR *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 ) ;																	// フォントハンドルを使用して文字列を描画する( 縦書き )( 座標指定が float 版 )
extern	int			DrawFormatStringFToHandle(              float x, float y, unsigned int Color, int FontHandle, const TCHAR *FormatString, ... ) ;																										// フォントハンドルを使用して書式指定文字列を描画する( 座標指定が float 版 )
extern	int			DrawFormatVStringFToHandle(             float x, float y, unsigned int Color, int FontHandle, const TCHAR *FormatString, ... ) ;																										// フォントハンドルを使用して書式指定文字列を描画する( 縦書き )( 座標指定が float 版 )
extern	int			DrawExtendStringFToHandle(              float x, float y, double ExRateX, double ExRateY, const TCHAR *String,                      unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE ) ;		// フォントハンドルを使用して文字列を拡大描画する( 座標指定が float 版 )
extern	int			DrawExtendNStringFToHandle(             float x, float y, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE ) ;		// フォントハンドルを使用して文字列を拡大描画する( 座標指定が float 版 )
extern	int			DrawExtendVStringFToHandle(             float x, float y, double ExRateX, double ExRateY, const TCHAR *String,                      unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 ) ;									// フォントハンドルを使用して文字列を拡大描画する( 縦書き )( 座標指定が float 版 )
extern	int			DrawExtendNVStringFToHandle(            float x, float y, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 ) ;									// フォントハンドルを使用して文字列を拡大描画する( 縦書き )( 座標指定が float 版 )
extern	int			DrawExtendFormatStringFToHandle(        float x, float y, double ExRateX, double ExRateY, unsigned int Color, int FontHandle, const TCHAR *FormatString, ... ) ;																		// フォントハンドルを使用して書式指定文字列を拡大描画する( 座標指定が float 版 )
extern	int			DrawExtendFormatVStringFToHandle(       float x, float y, double ExRateX, double ExRateY, unsigned int Color, int FontHandle, const TCHAR *FormatString, ... ) ;																		// フォントハンドルを使用して書式指定文字列を拡大描画する( 縦書き )( 座標指定が float 版 )
extern	int			DrawRotaStringFToHandle(				float x, float y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE , const TCHAR *String       = NULL                           ) ;		// フォントハンドルを使用して文字列を回転描画する( 座標指定が float 版 )
extern	int			DrawRotaNStringFToHandle(				float x, float y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE , const TCHAR *String       = NULL , size_t StringLength = 0 ) ;		// フォントハンドルを使用して文字列を回転描画する( 座標指定が float 版 )
extern	int			DrawRotaFormatStringFToHandle(			float x, float y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE , const TCHAR *FormatString = NULL , ...                     ) ;		// フォントハンドルを使用して書式指定文字列を回転描画する( 座標指定が float 版 )
extern	int			DrawModiStringFToHandle(				float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag, const TCHAR *String                            ) ;		// フォントハンドルを使用して文字列を変形描画する( 座標指定が float 版 )
extern	int			DrawModiNStringFToHandle(				float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag, const TCHAR *String,       size_t StringLength ) ;		// フォントハンドルを使用して文字列を変形描画する( 座標指定が float 版 )
extern	int			DrawModiFormatStringFToHandle(			float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag, const TCHAR *FormatString, ...                 ) ;		// フォントハンドルを使用して書式指定文字列を変形描画する( 座標指定が float 版 )

extern	int			DrawNumberToIToHandle(                  int x, int y,                          int    Num, int RisesNum, unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 ) ;											// フォントハンドルを使用して整数型の数値を描画する
extern	int			DrawNumberToFToHandle(                  int x, int y,                          double Num, int Length,   unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 ) ;											// フォントハンドルを使用して浮動小数点型の数値を描画する
extern	int			DrawNumberPlusToIToHandle(              int x, int y, const TCHAR *NoteString, int    Num, int RisesNum, unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 ) ;											// フォントハンドルを使用して整数型の数値とその説明の文字列を一度に描画する
extern	int			DrawNumberPlusToFToHandle(              int x, int y, const TCHAR *NoteString, double Num, int Length,   unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 ) ;											// フォントハンドルを使用して浮動小数点型の数値とその説明の文字列を一度に描画する

extern	int			DrawStringToHandleToZBuffer(            int x, int y, const TCHAR *String,                      int FontHandle,                                                                   int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag = FALSE ) ;							// フォントハンドルを使用してＺバッファに対して文字列を描画する
extern	int			DrawNStringToHandleToZBuffer(           int x, int y, const TCHAR *String, size_t StringLength, int FontHandle,                                                                   int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag = FALSE ) ;							// フォントハンドルを使用してＺバッファに対して文字列を描画する
extern	int			DrawVStringToHandleToZBuffer(           int x, int y, const TCHAR *String,                      int FontHandle,                                                                   int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;													// フォントハンドルを使用してＺバッファに対して文字列を描画する( 縦書き )
extern	int			DrawNVStringToHandleToZBuffer(          int x, int y, const TCHAR *String, size_t StringLength, int FontHandle,                                                                   int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;													// フォントハンドルを使用してＺバッファに対して文字列を描画する( 縦書き )
extern	int			DrawFormatStringToHandleToZBuffer(      int x, int y, int FontHandle,                                                                                        int WriteZMode /* DX_ZWRITE_MASK 等 */ , const TCHAR *FormatString, ... ) ;										// フォントハンドルを使用してＺバッファに対して書式指定文字列を描画する
extern	int			DrawFormatVStringToHandleToZBuffer(     int x, int y, int FontHandle,                                                                                        int WriteZMode /* DX_ZWRITE_MASK 等 */ , const TCHAR *FormatString, ... ) ;										// フォントハンドルを使用してＺバッファに対して書式指定文字列を描画する( 縦書き )
extern	int			DrawExtendStringToHandleToZBuffer(      int x, int y, double ExRateX, double ExRateY, const TCHAR *String,                      int FontHandle,                                   int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag = FALSE ) ;							// フォントハンドルを使用してＺバッファに対して文字列を拡大描画する
extern	int			DrawExtendNStringToHandleToZBuffer(     int x, int y, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, int FontHandle,                                   int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag = FALSE ) ;							// フォントハンドルを使用してＺバッファに対して文字列を拡大描画する
extern	int			DrawExtendVStringToHandleToZBuffer(     int x, int y, double ExRateX, double ExRateY, const TCHAR *String,                      int FontHandle,                                   int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;													// フォントハンドルを使用してＺバッファに対して文字列を拡大描画する( 縦書き )
extern	int			DrawExtendNVStringToHandleToZBuffer(    int x, int y, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, int FontHandle,                                   int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;													// フォントハンドルを使用してＺバッファに対して文字列を拡大描画する( 縦書き )
extern	int			DrawExtendFormatStringToHandleToZBuffer(   int x, int y, double ExRateX, double ExRateY, int FontHandle,                                                      int WriteZMode /* DX_ZWRITE_MASK 等 */ , const TCHAR *FormatString, ... ) ;										// フォントハンドルを使用してＺバッファに対して書式指定文字列を拡大描画する
extern	int			DrawExtendFormatVStringToHandleToZBuffer(  int x, int y, double ExRateX, double ExRateY, int FontHandle,                                                      int WriteZMode /* DX_ZWRITE_MASK 等 */ , const TCHAR *FormatString, ... ) ;										// フォントハンドルを使用してＺバッファに対して書式指定文字列を拡大描画する( 縦書き )
extern	int			DrawRotaStringToHandleToZBuffer(		int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, int FontHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag, const TCHAR *String                            ) ;		// フォントハンドルを使用してＺバッファに対して文字列を回転描画する
extern	int			DrawRotaNStringToHandleToZBuffer(		int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, int FontHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag, const TCHAR *String,       size_t StringLength ) ;		// フォントハンドルを使用してＺバッファに対して文字列を回転描画する
extern	int			DrawRotaFormatStringToHandleToZBuffer(	int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, int FontHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag, const TCHAR *FormatString, ...                 ) ;		// フォントハンドルを使用してＺバッファに対して書式指定文字列を回転描画する
extern	int			DrawModiStringToHandleToZBuffer(		int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int FontHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag, const TCHAR *String                            ) ;											// フォントハンドルを使用してＺバッファに対して文字列を変形描画する
extern	int			DrawModiNStringToHandleToZBuffer(		int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int FontHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag, const TCHAR *String,       size_t StringLength ) ;											// フォントハンドルを使用してＺバッファに対して文字列を変形描画する
extern	int			DrawModiFormatStringToHandleToZBuffer(	int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int FontHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag, const TCHAR *FormatString, ...                 ) ;											// フォントハンドルを使用してＺバッファに対して書式指定文字列を変形描画する

#endif // DX_NON_FONT








// DxMath.cpp 関数プロトタイプ宣言

// 演算ライブラリ
extern	int			ConvertMatrixFtoD(             MATRIX_D *Out, const MATRIX   *In ) ;															// float型要素の行列をdouble型要素の行列に変換する
extern	int			ConvertMatrixDtoF(             MATRIX   *Out, const MATRIX_D *In ) ;															// double型要素の行列をfloat型要素の行列に変換する
extern	int			CreateIdentityMatrix(          MATRIX   *Out ) ;																				// 単位行列を作成する
extern	int			CreateIdentityMatrixD(         MATRIX_D *Out ) ;																				// 単位行列を作成する
extern	int			CreateLookAtMatrix(            MATRIX   *Out, const VECTOR   *Eye, const VECTOR   *At, const VECTOR   *Up ) ;					// ビュー行列を作成する
extern	int			CreateLookAtMatrixD(           MATRIX_D *Out, const VECTOR_D *Eye, const VECTOR_D *At, const VECTOR_D *Up ) ;					// ビュー行列を作成する
extern	int			CreateLookAtMatrix2(           MATRIX   *Out, const VECTOR   *Eye, double XZAngle, double Oira ) ;								// ビュー行列を作成する(方向を回転値で指定)
extern	int			CreateLookAtMatrix2D(          MATRIX_D *Out, const VECTOR_D *Eye, double XZAngle, double Oira ) ;								// ビュー行列を作成する(方向を回転値で指定)
extern	int			CreateLookAtMatrixRH(          MATRIX   *Out, const VECTOR   *Eye, const VECTOR   *At, const VECTOR   *Up ) ;					// ビュー行列を作成する(右手座標系用)
extern	int			CreateLookAtMatrixRHD(         MATRIX_D *Out, const VECTOR_D *Eye, const VECTOR_D *At, const VECTOR_D *Up ) ;					// ビュー行列を作成する(右手座標系用)
extern	int			CreateMultiplyMatrix(          MATRIX   *Out, const MATRIX   *In1, const MATRIX   *In2 ) ;										// 行列の積を求める
extern	int			CreateMultiplyMatrixD(         MATRIX_D *Out, const MATRIX_D *In1, const MATRIX_D *In2 ) ;										// 行列の積を求める
extern	int			CreatePerspectiveFovMatrix(    MATRIX   *Out, float  fov,  float  zn, float  zf, float  aspect = -1.0f ) ;						// 射影行列を作成する
extern	int			CreatePerspectiveFovMatrixD(   MATRIX_D *Out, double fov,  double zn, double zf, double aspect = -1.0f ) ;						// 射影行列を作成する
extern	int			CreatePerspectiveFovMatrixRH(  MATRIX   *Out, float  fov,  float  zn, float  zf, float  aspect = -1.0f ) ;						// 射影行列を作成する(右手座標系用)
extern	int			CreatePerspectiveFovMatrixRHD( MATRIX_D *Out, double fov,  double zn, double zf, double aspect = -1.0f ) ;						// 射影行列を作成する(右手座標系用)
extern	int			CreateOrthoMatrix(             MATRIX   *Out, float  size, float  zn, float  zf, float  aspect = -1.0f ) ;						// 正射影行列を作成する
extern	int			CreateOrthoMatrixD(            MATRIX_D *Out, double size, double zn, double zf, double aspect = -1.0f ) ;						// 正射影行列を作成する
extern	int			CreateOrthoMatrixRH(           MATRIX   *Out, float  size, float  zn, float  zf, float  aspect = -1.0f ) ;						// 正射影行列を作成する(右手座標系用)
extern	int			CreateOrthoMatrixRHD(          MATRIX_D *Out, double size, double zn, double zf, double aspect = -1.0f ) ;						// 正射影行列を作成する(右手座標系用)
extern	int			CreateScalingMatrix(           MATRIX   *Out, float  sx, float  sy, float  sz ) ;												// スケーリング行列を作成する
extern	int			CreateScalingMatrixD(          MATRIX_D *Out, double sx, double sy, double sz ) ;												// スケーリング行列を作成する
extern	int			CreateRotationXMatrix(         MATRIX   *Out, float  Angle ) ;																	// Ｘ軸を中心とした回転行列を作成する
extern	int			CreateRotationXMatrixD(        MATRIX_D *Out, double Angle ) ;																	// Ｘ軸を中心とした回転行列を作成する
extern	int			CreateRotationYMatrix(         MATRIX   *Out, float  Angle ) ;																	// Ｙ軸を中心とした回転行列を作成する
extern	int			CreateRotationYMatrixD(        MATRIX_D *Out, double Angle ) ;																	// Ｙ軸を中心とした回転行列を作成する
extern	int			CreateRotationZMatrix(         MATRIX   *Out, float  Angle ) ;																	// Ｚ軸を中心とした回転行列を作成する
extern	int			CreateRotationZMatrixD(        MATRIX_D *Out, double Angle ) ;																	// Ｚ軸を中心とした回転行列を作成する
extern	int			CreateTranslationMatrix(       MATRIX   *Out, float  x, float  y, float  z ) ;													// 平行移動行列を作成する
extern	int			CreateTranslationMatrixD(      MATRIX_D *Out, double x, double y, double z ) ;													// 平行移動行列を作成する
extern	int			CreateTransposeMatrix(         MATRIX   *Out, const MATRIX   *In ) ;															// 転置行列を作成する
extern	int			CreateTransposeMatrixD(        MATRIX_D *Out, const MATRIX_D *In ) ;															// 転置行列を作成する
extern	int			CreateInverseMatrix(           MATRIX   *Out, const MATRIX   *In ) ;															// 逆行列を作成する
extern	int			CreateInverseMatrixD(          MATRIX_D *Out, const MATRIX_D *In ) ;															// 逆行列を作成する
extern	int			CreateViewportMatrix(          MATRIX   *Out, float  CenterX, float  CenterY, float  Width, float  Height ) ;					// ビューポート行列を作成する
extern	int			CreateViewportMatrixD(         MATRIX_D *Out, double CenterX, double CenterY, double Width, double Height ) ;					// ビューポート行列を作成する
extern	int			CreateRotationXYZMatrix(       MATRIX   *Out, float  XRot, float  YRot, float  ZRot ) ;											// Ｘ軸回転→Ｙ軸回転→Ｚ軸回転を合成した行列を作成する
extern	int			CreateRotationXYZMatrixD(      MATRIX_D *Out, double XRot, double YRot, double ZRot ) ;											// Ｘ軸回転→Ｙ軸回転→Ｚ軸回転を合成した行列を作成する
extern	int			CreateRotationXZYMatrix(       MATRIX   *Out, float  XRot, float  YRot, float  ZRot ) ;											// Ｘ軸回転→Ｚ軸回転→Ｙ軸回転を合成した行列を作成する
extern	int			CreateRotationXZYMatrixD(      MATRIX_D *Out, double XRot, double YRot, double ZRot ) ;											// Ｘ軸回転→Ｚ軸回転→Ｙ軸回転を合成した行列を作成する
extern	int			CreateRotationYXZMatrix(       MATRIX   *Out, float  XRot, float  YRot, float  ZRot ) ;											// Ｙ軸回転→Ｘ軸回転→Ｚ軸回転を合成した行列を作成する
extern	int			CreateRotationYXZMatrixD(      MATRIX_D *Out, double XRot, double YRot, double ZRot ) ;											// Ｙ軸回転→Ｘ軸回転→Ｚ軸回転を合成した行列を作成する
extern	int			CreateRotationYZXMatrix(       MATRIX   *Out, float  XRot, float  YRot, float  ZRot ) ;											// Ｙ軸回転→Ｚ軸回転→Ｘ軸回転を合成した行列を作成する
extern	int			CreateRotationYZXMatrixD(      MATRIX_D *Out, double XRot, double YRot, double ZRot ) ;											// Ｙ軸回転→Ｚ軸回転→Ｘ軸回転を合成した行列を作成する
extern	int			CreateRotationZXYMatrix(       MATRIX   *Out, float  XRot, float  YRot, float  ZRot ) ;											// Ｚ軸回転→Ｘ軸回転→Ｙ軸回転を合成した行列を作成する
extern	int			CreateRotationZXYMatrixD(      MATRIX_D *Out, double XRot, double YRot, double ZRot ) ;											// Ｚ軸回転→Ｘ軸回転→Ｙ軸回転を合成した行列を作成する
extern	int			CreateRotationZYXMatrix(       MATRIX   *Out, float  XRot, float  YRot, float  ZRot ) ;											// Ｚ軸回転→Ｙ軸回転→Ｘ軸回転を合成した行列を作成する
extern	int			CreateRotationZYXMatrixD(      MATRIX_D *Out, double XRot, double YRot, double ZRot ) ;											// Ｚ軸回転→Ｙ軸回転→Ｘ軸回転を合成した行列を作成する
extern	int			GetMatrixXYZRotation(          const MATRIX   *In, float  *OutXRot, float  *OutYRot, float  *OutZRot ) ;						// 行列からＸＹＺ軸回転の値を取得する( 戻り値　-1:ジンバルロック発生  0:成功 )
extern	int			GetMatrixXYZRotationD(         const MATRIX_D *In, double *OutXRot, double *OutYRot, double *OutZRot ) ;						// 行列からＸＹＺ軸回転の値を取得する( 戻り値　-1:ジンバルロック発生  0:成功 )
extern	int			GetMatrixXZYRotation(          const MATRIX   *In, float  *OutXRot, float  *OutYRot, float  *OutZRot ) ;						// 行列からＸＺＹ軸回転の値を取得する( 戻り値　-1:ジンバルロック発生  0:成功 )
extern	int			GetMatrixXZYRotationD(         const MATRIX_D *In, double *OutXRot, double *OutYRot, double *OutZRot ) ;						// 行列からＸＺＹ軸回転の値を取得する( 戻り値　-1:ジンバルロック発生  0:成功 )
extern	int			GetMatrixYXZRotation(          const MATRIX   *In, float  *OutXRot, float  *OutYRot, float  *OutZRot ) ;						// 行列からＹＸＺ軸回転の値を取得する( 戻り値　-1:ジンバルロック発生  0:成功 )
extern	int			GetMatrixYXZRotationD(         const MATRIX_D *In, double *OutXRot, double *OutYRot, double *OutZRot ) ;						// 行列からＹＸＺ軸回転の値を取得する( 戻り値　-1:ジンバルロック発生  0:成功 )
extern	int			GetMatrixYZXRotation(          const MATRIX   *In, float  *OutXRot, float  *OutYRot, float  *OutZRot ) ;						// 行列からＹＺＸ軸回転の値を取得する( 戻り値　-1:ジンバルロック発生  0:成功 )
extern	int			GetMatrixYZXRotationD(         const MATRIX_D *In, double *OutXRot, double *OutYRot, double *OutZRot ) ;						// 行列からＹＺＸ軸回転の値を取得する( 戻り値　-1:ジンバルロック発生  0:成功 )
extern	int			GetMatrixZXYRotation(          const MATRIX   *In, float  *OutXRot, float  *OutYRot, float  *OutZRot ) ;						// 行列からＺＸＹ軸回転の値を取得する( 戻り値　-1:ジンバルロック発生  0:成功 )
extern	int			GetMatrixZXYRotationD(         const MATRIX_D *In, double *OutXRot, double *OutYRot, double *OutZRot ) ;						// 行列からＺＸＹ軸回転の値を取得する( 戻り値　-1:ジンバルロック発生  0:成功 )
extern	int			GetMatrixZYXRotation(          const MATRIX   *In, float  *OutXRot, float  *OutYRot, float  *OutZRot ) ;						// 行列からＺＹＸ軸回転の値を取得する( 戻り値　-1:ジンバルロック発生  0:成功 )
extern	int			GetMatrixZYXRotationD(         const MATRIX_D *In, double *OutXRot, double *OutYRot, double *OutZRot ) ;						// 行列からＺＹＸ軸回転の値を取得する( 戻り値　-1:ジンバルロック発生  0:成功 )
extern	int			VectorConvertFtoD(             VECTOR_D *Out, const VECTOR   *In ) ;															// float型のベクトルをdouble型のベクトルに変換する
extern	int			VectorConvertDtoF(             VECTOR   *Out, const VECTOR_D *In ) ;															// double型のベクトルをfloat型のベクトルに変換する
extern	int			VectorNormalize(               VECTOR   *Out, const VECTOR   *In ) ;															// ベクトルを正規化する
extern	int			VectorNormalizeD(              VECTOR_D *Out, const VECTOR_D *In ) ;															// ベクトルを正規化する
extern	int			VectorScale(                   VECTOR   *Out, const VECTOR   *In, float  Scale ) ;												// ベクトルをスカラー倍する
extern	int			VectorScaleD(                  VECTOR_D *Out, const VECTOR_D *In, double Scale ) ;												// ベクトルをスカラー倍する
extern	int			VectorMultiply(                VECTOR   *Out, const VECTOR   *In1, const VECTOR   *In2 ) ;										// ベクトルの掛け算をする
extern	int			VectorMultiplyD(               VECTOR_D *Out, const VECTOR_D *In1, const VECTOR_D *In2 ) ;										// ベクトルの掛け算をする
extern	int			VectorSub(                     VECTOR   *Out, const VECTOR   *In1, const VECTOR   *In2 ) ;										// Out = In1 - In2 のベクトル計算をする 
extern	int			VectorSubD(                    VECTOR_D *Out, const VECTOR_D *In1, const VECTOR_D *In2 ) ;										// Out = In1 - In2 のベクトル計算をする 
extern	int			VectorAdd(                     VECTOR   *Out, const VECTOR   *In1, const VECTOR   *In2 ) ;										// Out = In1 + In2 のベクトル計算をする 
extern	int			VectorAddD(                    VECTOR_D *Out, const VECTOR_D *In1, const VECTOR_D *In2 ) ;										// Out = In1 + In2 のベクトル計算をする 
extern	int			VectorOuterProduct(            VECTOR   *Out, const VECTOR   *In1, const VECTOR   *In2 ) ;										// In1とIn2の外積を計算する
extern	int			VectorOuterProductD(           VECTOR_D *Out, const VECTOR_D *In1, const VECTOR_D *In2 ) ;										// In1とIn2の外積を計算する
extern	float 		VectorInnerProduct(            const VECTOR   *In1, const VECTOR   *In2 ) ;														// In1とIn2の内積を計算する
extern	double		VectorInnerProductD(           const VECTOR_D *In1, const VECTOR_D *In2 ) ;														// In1とIn2の内積を計算する
extern	int			VectorRotationX(               VECTOR   *Out, const VECTOR   *In, double Angle ) ;												// ベクトルのＸ軸を軸にした回転を行う
extern	int			VectorRotationXD(              VECTOR_D *Out, const VECTOR_D *In, double Angle ) ;												// ベクトルのＸ軸を軸にした回転を行う
extern	int			VectorRotationY(               VECTOR   *Out, const VECTOR   *In, double Angle ) ;												// ベクトルのＹ軸を軸にした回転を行う
extern	int			VectorRotationYD(              VECTOR_D *Out, const VECTOR_D *In, double Angle ) ;												// ベクトルのＹ軸を軸にした回転を行う
extern	int			VectorRotationZ(               VECTOR   *Out, const VECTOR   *In, double Angle ) ;												// ベクトルのＺ軸を軸にした回転を行う
extern	int			VectorRotationZD(              VECTOR_D *Out, const VECTOR_D *In, double Angle ) ;												// ベクトルのＺ軸を軸にした回転を行う
extern	int			VectorTransform(               VECTOR   *Out, const VECTOR   *InVec, const MATRIX   *InMatrix ) ;								// ベクトル行列と4x4正方行列を乗算する( w は 1 と仮定 )
extern	int			VectorTransformD(              VECTOR_D *Out, const VECTOR_D *InVec, const MATRIX_D *InMatrix ) ;								// ベクトル行列と4x4正方行列を乗算する( w は 1 と仮定 )
extern	int			VectorTransformSR(             VECTOR   *Out, const VECTOR   *InVec, const MATRIX   *InMatrix ) ;								// ベクトル行列と4x4正方行列の回転部分のみを乗算する
extern	int			VectorTransformSRD(            VECTOR_D *Out, const VECTOR_D *InVec, const MATRIX_D *InMatrix ) ;								// ベクトル行列と4x4正方行列の回転部分のみを乗算する
extern	int			VectorTransform4(              VECTOR   *Out, float  *V4Out, const VECTOR   *InVec, const float  *V4In, const MATRIX   *InMatrix ) ; // ベクトル行列と4x4正方行列を乗算する( w の要素を渡す )
extern	int			VectorTransform4D(             VECTOR_D *Out, double *V4Out, const VECTOR_D *InVec, const double *V4In, const MATRIX_D *InMatrix ) ; // ベクトル行列と4x4正方行列を乗算する( w の要素を渡す )

extern	int			Segment_Segment_Analyse(      const VECTOR   *SegmentAPos1, const VECTOR   *SegmentAPos2, const VECTOR   *SegmentBPos1, const VECTOR   *SegmentBPos2, SEGMENT_SEGMENT_RESULT   *Result ) ;								// 二つの線分の最接近点情報を解析する
extern	int			Segment_Segment_AnalyseD(     const VECTOR_D *SegmentAPos1, const VECTOR_D *SegmentAPos2, const VECTOR_D *SegmentBPos1, const VECTOR_D *SegmentBPos2, SEGMENT_SEGMENT_RESULT_D *Result ) ;								// 二つの線分の最接近点情報を解析する
extern	int			Segment_Point_Analyse(        const VECTOR   *SegmentPos1, const VECTOR   *SegmentPos2, const VECTOR   *PointPos, SEGMENT_POINT_RESULT   *Result ) ;																	// 線分と点の最接近点情報を解析する
extern	int			Segment_Point_AnalyseD(       const VECTOR_D *SegmentPos1, const VECTOR_D *SegmentPos2, const VECTOR_D *PointPos, SEGMENT_POINT_RESULT_D *Result ) ;																	// 線分と点の最接近点情報を解析する
extern	int			Segment_Triangle_Analyse(     const VECTOR   *SegmentPos1, const VECTOR   *SegmentPos2, const VECTOR   *TrianglePos1, const VECTOR   *TrianglePos2, const VECTOR   *TrianglePos3, SEGMENT_TRIANGLE_RESULT   *Result ) ;	// 線分と三角形の最接近点情報を解析する
extern	int			Segment_Triangle_AnalyseD(    const VECTOR_D *SegmentPos1, const VECTOR_D *SegmentPos2, const VECTOR_D *TrianglePos1, const VECTOR_D *TrianglePos2, const VECTOR_D *TrianglePos3, SEGMENT_TRIANGLE_RESULT_D *Result ) ;	// 線分と三角形の最接近点情報を解析する
extern	int			Triangle_Point_Analyse(       const VECTOR   *TrianglePos1, const VECTOR   *TrianglePos2, const VECTOR   *TrianglePos3, const VECTOR   *PointPos, TRIANGLE_POINT_RESULT   *Result ) ;									// 三角形と点の最接近点情報を解析する
extern	int			Triangle_Point_AnalyseD(      const VECTOR_D *TrianglePos1, const VECTOR_D *TrianglePos2, const VECTOR_D *TrianglePos3, const VECTOR_D *PointPos, TRIANGLE_POINT_RESULT_D *Result ) ;									// 三角形と点の最接近点情報を解析する
extern	int			Plane_Point_Analyse(          const VECTOR   *PlanePos, const VECTOR   *PlaneNormal, const VECTOR   *PointPos, PLANE_POINT_RESULT   *Result ) ;																			// 平面と点の最近点情報を解析する
extern	int			Plane_Point_AnalyseD(         const VECTOR_D *PlanePos, const VECTOR_D *PlaneNormal, const VECTOR_D *PointPos, PLANE_POINT_RESULT_D *Result ) ;																			// 平面と点の最近点情報を解析する

extern	void		TriangleBarycenter(           VECTOR   TrianglePos1, VECTOR   TrianglePos2, VECTOR   TrianglePos3, VECTOR   Position, float  *TrianglePos1Weight, float  *TrianglePos2Weight, float  *TrianglePos3Weight ) ;	// 指定の座標から三角形の重心を求める
extern	void		TriangleBarycenterD(          VECTOR_D TrianglePos1, VECTOR_D TrianglePos2, VECTOR_D TrianglePos3, VECTOR_D Position, double *TrianglePos1Weight, double *TrianglePos2Weight, double *TrianglePos3Weight ) ;	// 指定の座標から三角形の重心を求める

extern	float 		Segment_Segment_MinLength(           VECTOR   SegmentAPos1, VECTOR   SegmentAPos2, VECTOR   SegmentBPos1, VECTOR   SegmentBPos2 ) ;														// 二つの線分の最近点間の距離を得る
extern	double		Segment_Segment_MinLengthD(          VECTOR_D SegmentAPos1, VECTOR_D SegmentAPos2, VECTOR_D SegmentBPos1, VECTOR_D SegmentBPos2 ) ;														// 二つの線分の最近点間の距離を得る
extern	float 		Segment_Segment_MinLength_Square(    VECTOR   SegmentAPos1, VECTOR   SegmentAPos2, VECTOR   SegmentBPos1, VECTOR   SegmentBPos2 ) ;														// 二つの線分の最近点間の距離の二乗を得る
extern	double		Segment_Segment_MinLength_SquareD(   VECTOR_D SegmentAPos1, VECTOR_D SegmentAPos2, VECTOR_D SegmentBPos1, VECTOR_D SegmentBPos2 ) ;														// 二つの線分の最近点間の距離の二乗を得る
extern	float 		Segment_Triangle_MinLength(          VECTOR   SegmentPos1, VECTOR   SegmentPos2, VECTOR   TrianglePos1, VECTOR   TrianglePos2, VECTOR   TrianglePos3 ) ;								// 線分と三角形の最近点間の距離を得る 
extern	double		Segment_Triangle_MinLengthD(         VECTOR_D SegmentPos1, VECTOR_D SegmentPos2, VECTOR_D TrianglePos1, VECTOR_D TrianglePos2, VECTOR_D TrianglePos3 ) ;								// 線分と三角形の最近点間の距離を得る 
extern	float 		Segment_Triangle_MinLength_Square(   VECTOR   SegmentPos1, VECTOR   SegmentPos2, VECTOR   TrianglePos1, VECTOR   TrianglePos2, VECTOR   TrianglePos3 ) ;								// 線分と三角形の最近点間の距離の二乗を得る 
extern	double		Segment_Triangle_MinLength_SquareD(  VECTOR_D SegmentPos1, VECTOR_D SegmentPos2, VECTOR_D TrianglePos1, VECTOR_D TrianglePos2, VECTOR_D TrianglePos3 ) ;								// 線分と三角形の最近点間の距離の二乗を得る 
extern	float 		Segment_Point_MinLength(             VECTOR   SegmentPos1, VECTOR   SegmentPos2, VECTOR   PointPos ) ;																					// 線分と点の一番近い距離を得る
extern	double		Segment_Point_MinLengthD(            VECTOR_D SegmentPos1, VECTOR_D SegmentPos2, VECTOR_D PointPos ) ;																					// 線分と点の一番近い距離を得る
extern	float 		Segment_Point_MinLength_Square(      VECTOR   SegmentPos1, VECTOR   SegmentPos2, VECTOR   PointPos ) ;																					// 線分と点の一番近い距離の二乗を得る
extern	double		Segment_Point_MinLength_SquareD(     VECTOR_D SegmentPos1, VECTOR_D SegmentPos2, VECTOR_D PointPos ) ;																					// 線分と点の一番近い距離の二乗を得る
extern	float 		Triangle_Point_MinLength(            VECTOR   TrianglePos1, VECTOR   TrianglePos2, VECTOR   TrianglePos3, VECTOR   PointPos ) ;															// 三角形と点の一番近い距離を得る
extern	double		Triangle_Point_MinLengthD(           VECTOR_D TrianglePos1, VECTOR_D TrianglePos2, VECTOR_D TrianglePos3, VECTOR_D PointPos ) ;															// 三角形と点の一番近い距離を得る
extern	float 		Triangle_Point_MinLength_Square(     VECTOR   TrianglePos1, VECTOR   TrianglePos2, VECTOR   TrianglePos3, VECTOR   PointPos ) ;															// 三角形と点の一番近い距離の二乗を得る
extern	double		Triangle_Point_MinLength_SquareD(    VECTOR_D TrianglePos1, VECTOR_D TrianglePos2, VECTOR_D TrianglePos3, VECTOR_D PointPos ) ;															// 三角形と点の一番近い距離の二乗を得る
extern	float 		Triangle_Triangle_MinLength(         VECTOR   Triangle1Pos1, VECTOR   Triangle1Pos2, VECTOR   Triangle1Pos3, VECTOR   Triangle2Pos1, VECTOR   Triangle2Pos2, VECTOR   Triangle2Pos3 ) ;	// 二つの三角形の最近点間の距離を得る
extern	double		Triangle_Triangle_MinLengthD(        VECTOR_D Triangle1Pos1, VECTOR_D Triangle1Pos2, VECTOR_D Triangle1Pos3, VECTOR_D Triangle2Pos1, VECTOR_D Triangle2Pos2, VECTOR_D Triangle2Pos3 ) ;	// 二つの三角形の最近点間の距離を得る
extern	float 		Triangle_Triangle_MinLength_Square(  VECTOR   Triangle1Pos1, VECTOR   Triangle1Pos2, VECTOR   Triangle1Pos3, VECTOR   Triangle2Pos1, VECTOR   Triangle2Pos2, VECTOR   Triangle2Pos3 ) ;	// 二つの三角形の最近点間の距離の二乗を得る
extern	double		Triangle_Triangle_MinLength_SquareD( VECTOR_D Triangle1Pos1, VECTOR_D Triangle1Pos2, VECTOR_D Triangle1Pos3, VECTOR_D Triangle2Pos1, VECTOR_D Triangle2Pos2, VECTOR_D Triangle2Pos3 ) ;	// 二つの三角形の最近点間の距離の二乗を得る
extern	VECTOR  	Plane_Point_MinLength_Position(      VECTOR   PlanePos, VECTOR   PlaneNormal, VECTOR   PointPos ) ;																						// 点に一番近い平面上の座標を得る
extern	VECTOR_D	Plane_Point_MinLength_PositionD(     VECTOR_D PlanePos, VECTOR_D PlaneNormal, VECTOR_D PointPos ) ;																						// 点に一番近い平面上の座標を得る
extern	float 		Plane_Point_MinLength(               VECTOR   PlanePos, VECTOR   PlaneNormal, VECTOR   PointPos ) ;																						// 平面と点の一番近い距離を得る
extern	double		Plane_Point_MinLengthD(              VECTOR_D PlanePos, VECTOR_D PlaneNormal, VECTOR_D PointPos ) ;																						// 平面と点の一番近い距離を得る

extern	HITRESULT_LINE   HitCheck_Line_Triangle(         VECTOR   LinePos1, VECTOR   LinePos2, VECTOR   TrianglePos1, VECTOR   TrianglePos2, VECTOR   TrianglePos3 ) ;										// 三角形と線の当たり判定
extern	HITRESULT_LINE_D HitCheck_Line_TriangleD(        VECTOR_D LinePos1, VECTOR_D LinePos2, VECTOR_D TrianglePos1, VECTOR_D TrianglePos2, VECTOR_D TrianglePos3 ) ;										// 三角形と線の当たり判定
extern	int			HitCheck_Triangle_Triangle(          VECTOR   Triangle1Pos1, VECTOR   Triangle1Pos2, VECTOR   Triangle1Pos3, VECTOR   Triangle2Pos1, VECTOR   Triangle2Pos2, VECTOR   Triangle2Pos3 ) ;	// 三角形と三角形の当たり判定( TRUE:当たっている  FALSE:当たっていない )
extern	int			HitCheck_Triangle_TriangleD(         VECTOR_D Triangle1Pos1, VECTOR_D Triangle1Pos2, VECTOR_D Triangle1Pos3, VECTOR_D Triangle2Pos1, VECTOR_D Triangle2Pos2, VECTOR_D Triangle2Pos3 ) ;	// 三角形と三角形の当たり判定( TRUE:当たっている  FALSE:当たっていない )
extern	int			HitCheck_Triangle_Triangle_2D(       VECTOR   Triangle1Pos1, VECTOR   Triangle1Pos2, VECTOR   Triangle1Pos3, VECTOR   Triangle2Pos1, VECTOR   Triangle2Pos2, VECTOR   Triangle2Pos3 ) ;	// 三角形と三角形の当たり判定( ２Ｄ版 )( TRUE:当たっている  FALSE:当たっていない )
extern	int			HitCheck_Triangle_TriangleD_2D(      VECTOR_D Triangle1Pos1, VECTOR_D Triangle1Pos2, VECTOR_D Triangle1Pos3, VECTOR_D Triangle2Pos1, VECTOR_D Triangle2Pos2, VECTOR_D Triangle2Pos3 ) ;	// 三角形と三角形の当たり判定( ２Ｄ版 )( TRUE:当たっている  FALSE:当たっていない )
extern	HITRESULT_LINE   HitCheck_Line_Cube(             VECTOR   LinePos1, VECTOR   LinePos2, VECTOR   CubePos1, VECTOR   CubePos2 );																		// 線と箱の当たり判定
extern	HITRESULT_LINE_D HitCheck_Line_CubeD(            VECTOR_D LinePos1, VECTOR_D LinePos2, VECTOR_D CubePos1, VECTOR_D CubePos2 );																		// 線と箱の当たり判定
extern	int			HitCheck_Line_Sphere(                VECTOR   LinePos1, VECTOR   LinePos2, VECTOR   SphereCenterPos, float  SphereR ) ;																	// 線と球の当たり判定( TRUE:当たっている  FALSE:当たっていない )
extern	int			HitCheck_Line_SphereD(               VECTOR_D LinePos1, VECTOR_D LinePos2, VECTOR_D SphereCenterPos, double SphereR ) ;																	// 線と球の当たり判定( TRUE:当たっている  FALSE:当たっていない )
extern	int			HitCheck_Sphere_Sphere(              VECTOR   Sphere1CenterPos, float  Sphere1R, VECTOR   Sphere2CenterPos, float  Sphere2R ) ;															// 球と球の当たり判定( TRUE:当たっている  FALSE:当たっていない )
extern	int			HitCheck_Sphere_SphereD(             VECTOR_D Sphere1CenterPos, double Sphere1R, VECTOR_D Sphere2CenterPos, double Sphere2R ) ;															// 球と球の当たり判定( TRUE:当たっている  FALSE:当たっていない )
extern	int			HitCheck_Sphere_Capsule(             VECTOR   SphereCenterPos, float  SphereR, VECTOR   CapPos1, VECTOR   CapPos2, float  CapR ) ;														// 球とカプセルの当たり判定( TRUE:当たっている  FALSE:当たっていない )
extern	int			HitCheck_Sphere_CapsuleD(            VECTOR_D SphereCenterPos, double SphereR, VECTOR_D CapPos1, VECTOR_D CapPos2, double CapR ) ;														// 球とカプセルの当たり判定( TRUE:当たっている  FALSE:当たっていない )
extern	int			HitCheck_Sphere_Triangle(            VECTOR   SphereCenterPos, float  SphereR, VECTOR   TrianglePos1, VECTOR   TrianglePos2, VECTOR   TrianglePos3 ) ;									// 球と三角形の当たり判定( TRUE:当たっている  FALSE:当たっていない )
extern	int			HitCheck_Sphere_TriangleD(           VECTOR_D SphereCenterPos, double SphereR, VECTOR_D TrianglePos1, VECTOR_D TrianglePos2, VECTOR_D TrianglePos3 ) ;									// 球と三角形の当たり判定( TRUE:当たっている  FALSE:当たっていない )
extern	int			HitCheck_Capsule_Capsule(            VECTOR   Cap1Pos1, VECTOR   Cap1Pos2, float  Cap1R, VECTOR   Cap2Pos1, VECTOR   Cap2Pos2, float  Cap2R ) ;											// カプセル同士の当たり判定( TRUE:当たっている  FALSE:当たっていない )
extern	int			HitCheck_Capsule_CapsuleD(           VECTOR_D Cap1Pos1, VECTOR_D Cap1Pos2, double Cap1R, VECTOR_D Cap2Pos1, VECTOR_D Cap2Pos2, double Cap2R ) ;											// カプセル同士の当たり判定( TRUE:当たっている  FALSE:当たっていない )
extern	int			HitCheck_Capsule_Triangle(           VECTOR   CapPos1, VECTOR   CapPos2, float  CapR, VECTOR   TrianglePos1, VECTOR   TrianglePos2, VECTOR   TrianglePos3 ) ;							// カプセルと三角形の当たり判定( TRUE:当たっている  FALSE:当たっていない )
extern	int			HitCheck_Capsule_TriangleD(          VECTOR_D CapPos1, VECTOR_D CapPos2, double CapR, VECTOR_D TrianglePos1, VECTOR_D TrianglePos2, VECTOR_D TrianglePos3 ) ;							// カプセルと三角形の当たり判定( TRUE:当たっている  FALSE:当たっていない )

// その他
extern	int			RectClipping( RECT *Rect, const RECT *ClippuRect ) ;									// 矩形のクリッピング
extern	int			RectAdjust(   RECT *Rect ) ;															// 矩形の left が right より値が大きい場合などの誤りを補正する
extern	int			GetRectSize(  const RECT *Rect, int *Width, int *Height ) ;								// 矩形の幅と高さを求める

// 表記簡略版
extern	MATRIX		MGetIdent(          void ) ;															// 単位行列を取得する
extern	MATRIX_D	MGetIdentD(         void ) ;															// 単位行列を取得する
extern	MATRIX		MMult(              MATRIX   In1, MATRIX   In2 ) ;										// 行列の乗算を行う
extern	MATRIX_D	MMultD(             MATRIX_D In1, MATRIX_D In2 ) ;										// 行列の乗算を行う
extern	MATRIX		MScale(             MATRIX   InM, float  Scale ) ;										// 行列のスケーリングを行う
extern	MATRIX_D	MScaleD(            MATRIX_D InM, double Scale ) ;										// 行列のスケーリングを行う
extern	MATRIX		MAdd(               MATRIX   In1, MATRIX   In2 ) ;										// 行列の足し算を行う
extern	MATRIX_D	MAddD(              MATRIX_D In1, MATRIX_D In2 ) ;										// 行列の足し算を行う
extern	MATRIX		MGetScale(          VECTOR   Scale ) ;													// 拡大行列を取得する
extern	MATRIX_D	MGetScaleD(         VECTOR_D Scale ) ;													// 拡大行列を取得する
extern	MATRIX		MGetRotX(           float  XAxisRotate ) ;												// Ｘ軸回転行列を取得する
extern	MATRIX_D	MGetRotXD(          double XAxisRotate ) ;												// Ｘ軸回転行列を取得する
extern	MATRIX		MGetRotY(           float  YAxisRotate ) ;												// Ｙ軸回転行列を取得する
extern	MATRIX_D	MGetRotYD(          double YAxisRotate ) ;												// Ｙ軸回転行列を取得する
extern	MATRIX		MGetRotZ(           float  ZAxisRotate ) ;												// Ｚ軸回転行列を取得する
extern	MATRIX_D	MGetRotZD(          double ZAxisRotate ) ;												// Ｚ軸回転行列を取得する
extern	MATRIX		MGetRotAxis(        VECTOR   RotateAxis, float  Rotate ) ;								// 指定軸で指定角度回転する行列を取得する
extern	MATRIX_D	MGetRotAxisD(       VECTOR_D RotateAxis, double Rotate ) ;								// 指定軸で指定角度回転する行列を取得する
extern	MATRIX		MGetRotVec2(        VECTOR   In1, VECTOR   In2 ) ;										// In1 の向きから In2 の向きへ変換する回転行列を取得する( In2 と In1 が真逆のベクトルの場合は回転軸は不定 )
extern	MATRIX_D	MGetRotVec2D(       VECTOR_D In1, VECTOR_D In2 ) ;										// In1 の向きから In2 の向きへ変換する回転行列を取得する( In2 と In1 が真逆のベクトルの場合は回転軸は不定 )
extern	MATRIX		MGetTranslate(      VECTOR   Trans ) ;													// 平行移動行列を取得する
extern	MATRIX_D	MGetTranslateD(     VECTOR_D Trans ) ;													// 平行移動行列を取得する
extern	MATRIX		MGetAxis1(          VECTOR   XAxis, VECTOR   YAxis, VECTOR   ZAxis, VECTOR   Pos ) ;	// 指定の３軸ローカルのベクトルを基本軸上のベクトルに変換する行列を取得する( x' = XAxis.x * x + YAixs.x * y + ZAxis.z * z + Pos.x   y' = XAxis.y * x + YAixs.y * y + ZAxis.y * z + Pos.y      z' = XAxis.z * x + YAixs.z * y + ZAxis.z * z + Pos.z )
extern	MATRIX_D	MGetAxis1D(         VECTOR_D XAxis, VECTOR_D YAxis, VECTOR_D ZAxis, VECTOR_D Pos ) ;	// 指定の３軸ローカルのベクトルを基本軸上のベクトルに変換する行列を取得する( x' = XAxis.x * x + YAixs.x * y + ZAxis.z * z + Pos.x   y' = XAxis.y * x + YAixs.y * y + ZAxis.y * z + Pos.y      z' = XAxis.z * x + YAixs.z * y + ZAxis.z * z + Pos.z )
extern	MATRIX		MGetAxis2(          VECTOR   XAxis, VECTOR   YAxis, VECTOR   ZAxis, VECTOR   Pos ) ;	// 基本軸上のベクトルを指定の３軸上に投影したベクトルに変換する行列を取得する( x' = XAxis.x * ( x - Pos.x ) + XAxis.y * ( x - Pos.x ) + XAxis.z * ( x - Pos.x )    y' = YAxis.x * ( x - Pos.x ) + YAxis.y * ( x - Pos.x ) + YAxis.z * ( x - Pos.x )    z' = ZAxis.x * ( x - Pos.x ) + ZAxis.y * ( x - Pos.x ) + ZAxis.z * ( x - Pos.x ) )
extern	MATRIX_D	MGetAxis2D(         VECTOR_D XAxis, VECTOR_D YAxis, VECTOR_D ZAxis, VECTOR_D Pos ) ;	// 基本軸上のベクトルを指定の３軸上に投影したベクトルに変換する行列を取得する( x' = XAxis.x * ( x - Pos.x ) + XAxis.y * ( x - Pos.x ) + XAxis.z * ( x - Pos.x )    y' = YAxis.x * ( x - Pos.x ) + YAxis.y * ( x - Pos.x ) + YAxis.z * ( x - Pos.x )    z' = ZAxis.x * ( x - Pos.x ) + ZAxis.y * ( x - Pos.x ) + ZAxis.z * ( x - Pos.x ) )
extern	MATRIX		MTranspose(         MATRIX   InM ) ;													// 転置行列を取得する
extern	MATRIX_D	MTransposeD(        MATRIX_D InM ) ;													// 転置行列を取得する
extern	MATRIX		MInverse(           MATRIX   InM ) ;													// 逆行列を取得する
extern	MATRIX_D	MInverseD(          MATRIX_D InM ) ;													// 逆行列を取得する
extern  VECTOR		MGetSize(           MATRIX   InM ) ;													// 拡大行列のＸ軸、Ｙ軸、Ｚ軸の拡大率を取得する
extern  VECTOR_D	MGetSizeD(          MATRIX_D InM ) ;													// 拡大行列のＸ軸、Ｙ軸、Ｚ軸の拡大率を取得する
extern	MATRIX		MGetRotElem(        MATRIX   InM ) ;													// 行列の回転成分を取得する
extern	MATRIX_D	MGetRotElemD(       MATRIX_D InM ) ;													// 行列の回転成分を取得する


// 行列の平行移動成分を取得する
__inline VECTOR MGetTranslateElem( MATRIX &InM )
{
	VECTOR Result ;

	Result.x = InM.m[ 3 ][ 0 ] ;
	Result.y = InM.m[ 3 ][ 1 ] ;
	Result.z = InM.m[ 3 ][ 2 ] ;

	return Result ;
}

// 行列の平行移動成分を取得する
__inline VECTOR_D MGetTranslateElemD( MATRIX_D &InM )
{
	VECTOR_D Result ;

	Result.x = InM.m[ 3 ][ 0 ] ;
	Result.y = InM.m[ 3 ][ 1 ] ;
	Result.z = InM.m[ 3 ][ 2 ] ;

	return Result ;
}

// ベクトル値の変換
__inline VECTOR_D VConvFtoD( const VECTOR &In )
{
	VECTOR_D Result ;

	Result.x = In.x ;
	Result.y = In.y ;
	Result.z = In.z ;

	return Result ;
}

__inline VECTOR VConvDtoF( const VECTOR_D &In )
{
	VECTOR Result ;

	Result.x = ( float )In.x ;
	Result.y = ( float )In.y ;
	Result.z = ( float )In.z ;

	return Result ;
}

// ベクトル値の生成
__inline VECTOR VGet( float x, float y, float z )
{
	VECTOR Result;
	Result.x = x ;
	Result.y = y ;
	Result.z = z ;
	return Result ;
}

__inline VECTOR_D VGetD( double x, double y, double z )
{
	VECTOR_D Result;
	Result.x = x ;
	Result.y = y ;
	Result.z = z ;
	return Result ;
}

__inline FLOAT2 F2Get( float u, float v )
{
	FLOAT2 Result;
	Result.u = u ;
	Result.v = v ;
	return Result ;
}

// ４要素ベクトル値の生成
__inline FLOAT4 F4Get( float x, float y, float z, float w )
{
	FLOAT4 Result;
	Result.x = x ;
	Result.y = y ;
	Result.z = z ;
	Result.w = w ;
	return Result ;
}

__inline DOUBLE4 D4Get( double x, double y, double z, double w )
{
	DOUBLE4 Result;
	Result.x = x ;
	Result.y = y ;
	Result.z = z ;
	Result.w = w ;
	return Result ;
}

// ベクトルの加算
__inline VECTOR		VAdd( const VECTOR &In1, const VECTOR &In2 )
{
	VECTOR Result ;
	Result.x = In1.x + In2.x ;
	Result.y = In1.y + In2.y ;
	Result.z = In1.z + In2.z ;
	return Result ;
}

__inline VECTOR_D	VAddD( const VECTOR_D &In1, const VECTOR_D &In2 )
{
	VECTOR_D Result ;
	Result.x = In1.x + In2.x ;
	Result.y = In1.y + In2.y ;
	Result.z = In1.z + In2.z ;
	return Result ;
}

// ベクトルの減算
__inline VECTOR		VSub( const VECTOR &In1, const VECTOR &In2 )
{
	VECTOR Result ;
	Result.x = In1.x - In2.x ;
	Result.y = In1.y - In2.y ;
	Result.z = In1.z - In2.z ;
	return Result ;
}

__inline VECTOR_D	VSubD( const VECTOR_D &In1, const VECTOR_D &In2 )
{
	VECTOR_D Result ;
	Result.x = In1.x - In2.x ;
	Result.y = In1.y - In2.y ;
	Result.z = In1.z - In2.z ;
	return Result ;
}

__inline FLOAT2		F2Add( const FLOAT2 &In1, const FLOAT2 &In2 )
{
	FLOAT2 Result ;
	Result.u = In1.u + In2.u ;
	Result.v = In1.v + In2.v ;
	return Result ;
}

// ４要素ベクトルの加算
__inline FLOAT4		F4Add( const FLOAT4 &In1, const FLOAT4 &In2 )
{
	FLOAT4 Result ;
	Result.x = In1.x + In2.x ;
	Result.y = In1.y + In2.y ;
	Result.z = In1.z + In2.z ;
	Result.w = In1.w + In2.w ;
	return Result ;
}

__inline DOUBLE4	D4Add( const DOUBLE4 &In1, const DOUBLE4 &In2 )
{
	DOUBLE4 Result ;
	Result.x = In1.x + In2.x ;
	Result.y = In1.y + In2.y ;
	Result.z = In1.z + In2.z ;
	Result.w = In1.w + In2.w ;
	return Result ;
}

__inline FLOAT2		F2Sub( const FLOAT2 &In1, const FLOAT2 &In2 )
{
	FLOAT2 Result ;
	Result.u = In1.u - In2.u ;
	Result.v = In1.v - In2.v ;
	return Result ;
}

// ４要素ベクトルの減算
__inline FLOAT4		F4Sub( const FLOAT4 &In1, const FLOAT4 &In2 )
{
	FLOAT4 Result ;
	Result.x = In1.x - In2.x ;
	Result.y = In1.y - In2.y ;
	Result.z = In1.z - In2.z ;
	Result.w = In1.w - In2.w ;
	return Result ;
}

__inline DOUBLE4	D4Sub( const DOUBLE4 &In1, const DOUBLE4 &In2 )
{
	DOUBLE4 Result ;
	Result.x = In1.x - In2.x ;
	Result.y = In1.y - In2.y ;
	Result.z = In1.z - In2.z ;
	Result.w = In1.w - In2.w ;
	return Result ;
}

// ベクトルの内積
__inline float		VDot( const VECTOR &In1, const VECTOR &In2 )
{
	return In1.x * In2.x + In1.y * In2.y + In1.z * In2.z ;
}

__inline double		VDotD( const VECTOR_D &In1, const VECTOR_D &In2 )
{
	return In1.x * In2.x + In1.y * In2.y + In1.z * In2.z ;
}

// ベクトルの外積
__inline VECTOR		VCross( const VECTOR &In1, const VECTOR &In2 )
{
	VECTOR Result ;
	Result.x = In1.y * In2.z - In1.z * In2.y ;
	Result.y = In1.z * In2.x - In1.x * In2.z ;
	Result.z = In1.x * In2.y - In1.y * In2.x ;
	return Result ;
}

__inline VECTOR_D	VCrossD( const VECTOR_D &In1, const VECTOR_D &In2 )
{
	VECTOR_D Result ;
	Result.x = In1.y * In2.z - In1.z * In2.y ;
	Result.y = In1.z * In2.x - In1.x * In2.z ;
	Result.z = In1.x * In2.y - In1.y * In2.x ;
	return Result ;
}

// ベクトルのスケーリング
__inline VECTOR		VScale( const VECTOR &In, float Scale )
{
	VECTOR Result ;
	Result.x = In.x * Scale ;
	Result.y = In.y * Scale ;
	Result.z = In.z * Scale ;
	return Result ;
}

__inline VECTOR_D	VScaleD( const VECTOR_D &In, double Scale )
{
	VECTOR_D Result ;
	Result.x = In.x * Scale ;
	Result.y = In.y * Scale ;
	Result.z = In.z * Scale ;
	return Result ;
}

__inline FLOAT2		F2Scale( const FLOAT2 &In, float Scale )
{
	FLOAT2 Result ;
	Result.u = In.u * Scale ;
	Result.v = In.v * Scale ;
	return Result ;
}

// ４要素ベクトルのスケーリング
__inline FLOAT4		F4Scale( const FLOAT4 &In, float Scale )
{
	FLOAT4 Result ;
	Result.x = In.x * Scale ;
	Result.y = In.y * Scale ;
	Result.z = In.z * Scale ;
	Result.w = In.w * Scale ;
	return Result ;
}

__inline DOUBLE4	D4Scale( const DOUBLE4 &In, double Scale )
{
	DOUBLE4 Result ;
	Result.x = In.x * Scale ;
	Result.y = In.y * Scale ;
	Result.z = In.z * Scale ;
	Result.w = In.w * Scale ;
	return Result ;
}

// ベクトルの正規化
extern VECTOR		VNorm(  VECTOR   In ) ;
extern VECTOR_D		VNormD( VECTOR_D In ) ;

// ベクトルのサイズ
extern float		VSize(  VECTOR   In ) ;
extern double		VSizeD( VECTOR_D In ) ;

// ベクトルのサイズの２乗
__inline float		VSquareSize(  const VECTOR   &In )
{
	return In.x * In.x + In.y * In.y + In.z * In.z ;
}

__inline double		VSquareSizeD( const VECTOR_D &In )
{
	return In.x * In.x + In.y * In.y + In.z * In.z ;
}

// 行列を使った座標変換
__inline VECTOR		VTransform( const VECTOR &InV, const MATRIX &InM )
{
	VECTOR Result ;
	Result.x = InV.x * InM.m[0][0] + InV.y * InM.m[1][0] + InV.z * InM.m[2][0] + InM.m[3][0] ;
	Result.y = InV.x * InM.m[0][1] + InV.y * InM.m[1][1] + InV.z * InM.m[2][1] + InM.m[3][1] ;
	Result.z = InV.x * InM.m[0][2] + InV.y * InM.m[1][2] + InV.z * InM.m[2][2] + InM.m[3][2] ;
	return Result ;
}

__inline VECTOR_D	VTransformD( const VECTOR_D &InV, const MATRIX_D &InM )
{
	VECTOR_D Result ;
	Result.x = InV.x * InM.m[0][0] + InV.y * InM.m[1][0] + InV.z * InM.m[2][0] + InM.m[3][0] ;
	Result.y = InV.x * InM.m[0][1] + InV.y * InM.m[1][1] + InV.z * InM.m[2][1] + InM.m[3][1] ;
	Result.z = InV.x * InM.m[0][2] + InV.y * InM.m[1][2] + InV.z * InM.m[2][2] + InM.m[3][2] ;
	return Result ;
}

// 行列を使った座標変換( スケーリング＋回転成分のみ )
__inline VECTOR		VTransformSR( const VECTOR &InV, const MATRIX &InM )
{
	VECTOR Result ;
	Result.x = InV.x * InM.m[0][0] + InV.y * InM.m[1][0] + InV.z * InM.m[2][0] ;
	Result.y = InV.x * InM.m[0][1] + InV.y * InM.m[1][1] + InV.z * InM.m[2][1] ;
	Result.z = InV.x * InM.m[0][2] + InV.y * InM.m[1][2] + InV.z * InM.m[2][2] ;
	return Result ;
}

__inline VECTOR_D	VTransformSRD( const VECTOR_D &InV, const MATRIX_D &InM )
{
	VECTOR_D Result ;
	Result.x = InV.x * InM.m[0][0] + InV.y * InM.m[1][0] + InV.z * InM.m[2][0] ;
	Result.y = InV.x * InM.m[0][1] + InV.y * InM.m[1][1] + InV.z * InM.m[2][1] ;
	Result.z = InV.x * InM.m[0][2] + InV.y * InM.m[1][2] + InV.z * InM.m[2][2] ;
	return Result ;
}

// 二つのベクトルが成す角のコサイン値を得る
extern	float		VCos(  VECTOR   In1, VECTOR   In2 ) ;
extern	double		VCosD( VECTOR_D In1, VECTOR_D In2 ) ;

// 二つのベクトルが成す角の角度を得る( 単位：ラジアン )
extern	float		VRad(  VECTOR   In1, VECTOR   In2 ) ;
extern	double		VRadD( VECTOR_D In1, VECTOR_D In2 ) ;



// 2つのクォータニオンの乗算の結果を返す
__inline FLOAT4 QTCross( const FLOAT4 &A, const FLOAT4 &B )
{
    FLOAT4 Result ;
    Result.w = A.w * B.w - ( A.x * B.x + A.y * B.y + A.z * B.z ) ;
	Result.x = B.x * A.w + A.x * B.w + ( A.y * B.z - A.z * B.y ) ;
	Result.y = B.y * A.w + A.y * B.w + ( A.z * B.x - A.x * B.z ) ;
	Result.z = B.z * A.w + A.z * B.w + ( A.x * B.y - A.y * B.x ) ;
    return Result ;
}
__inline DOUBLE4 QTCrossD( const DOUBLE4 &A, const DOUBLE4 &B )
{
    DOUBLE4 Result ;
    Result.w = A.w * B.w - ( A.x * B.x + A.y * B.y + A.z * B.z ) ;
	Result.x = B.x * A.w + A.x * B.w + ( A.y * B.z - A.z * B.y ) ;
	Result.y = B.y * A.w + A.y * B.w + ( A.z * B.x - A.x * B.z ) ;
	Result.z = B.z * A.w + A.z * B.w + ( A.x * B.y - A.y * B.x ) ;
    return Result ;
}

// 共役クォータニオンを返す
__inline FLOAT4 QTConj( const FLOAT4 &A )
{
	FLOAT4 Result ;
	Result.w =  A.w ;
	Result.x = -A.x ;
	Result.y = -A.y ;
	Result.z = -A.z ;
	return Result ;
}
__inline DOUBLE4 QTConjD( const DOUBLE4 &A )
{
	DOUBLE4 Result ;
	Result.w =  A.w ;
	Result.x = -A.x ;
	Result.y = -A.y ;
	Result.z = -A.z ;
	return Result ;
}

// 回転を表すクォータニオンを返す
extern FLOAT4  QTRot(  VECTOR   Axis, float  Angle ) ;
extern DOUBLE4 QTRotD( VECTOR_D Axis, double Angle ) ;

// 3次元空間上の点を任意の軸の周りに任意の角度だけ回転させる関数
extern VECTOR   VRotQ(  VECTOR   P, VECTOR   Axis, float  Angle ) ;
extern VECTOR_D VRotQD( VECTOR_D P, VECTOR_D Axis, double Angle ) ;










// DxBaseImage.cpp 関数プロトタイプ宣言

// 基本イメージデータのロード＋ＤＩＢ関係
extern	int			CreateGraphImageOrDIBGraph(            const TCHAR *FileName,                        const void *DataImage, int DataImageSize, int DataImageType /* LOADIMAGE_TYPE_FILE 等 */ , int BmpFlag, int ReverseFlag, BASEIMAGE *BaseImage, BITMAPINFO **BmpInfo, void **GraphData ) ;					// 画像ファイル若しくはメモリ上に展開された画像ファイルイメージを読み込み、基本イメージデータ若しくはＢＭＰデータを構築する
extern	int			CreateGraphImageOrDIBGraphWithStrLen(  const TCHAR *FileName, size_t FileNameLength, const void *DataImage, int DataImageSize, int DataImageType /* LOADIMAGE_TYPE_FILE 等 */ , int BmpFlag, int ReverseFlag, BASEIMAGE *BaseImage, BITMAPINFO **BmpInfo, void **GraphData ) ;					// 画像ファイル若しくはメモリ上に展開された画像ファイルイメージを読み込み、基本イメージデータ若しくはＢＭＰデータを構築する
extern	int			CreateGraphImageType2(                 STREAMDATA *Src, BASEIMAGE *Dest ) ;																																																						// 画像データから構築したストリーム読み込み用データを使用して基本イメージデータを構築する
extern	int			CreateBmpInfo(                         BITMAPINFO *BmpInfo, int Width, int Height, int Pitch, const void *SrcGrData, void **DestGrData ) ;																																						// 24bitカラー形式のビットマップデータからＢＭＰデータを構築する
extern	int			GetImageSize_File(                     const TCHAR *FileName,                        int *SizeX, int *SizeY ) ;																																													// 画像ファイルの横ピクセル数と縦ピクセル数を取得する
extern	int			GetImageSize_FileWithStrLen(           const TCHAR *FileName, size_t FileNameLength, int *SizeX, int *SizeY ) ;																																													// 画像ファイルの横ピクセル数と縦ピクセル数を取得する
extern	int			GetImageSize_Mem(                      const void *FileImage, int FileImageSize, int *SizeX, int *SizeY ) ;																																														// メモリ上に展開された画像ファイルの横ピクセル数と縦ピクセル数を取得する
extern	unsigned int GetGraphImageFullColorCode(           const BASEIMAGE *GraphImage, int x, int y ) ;																																																			// 基本イメージデータの指定座標のフルカラーコードを取得する
extern	int			CreateGraphImage_plus_Alpha(           const TCHAR *FileName,                        const void *RgbImage, int RgbImageSize, int RgbImageType, const void *AlphaImage, int AlphaImageSize, int AlphaImageType, BASEIMAGE *RgbGraphImage, BASEIMAGE *AlphaGraphImage, int ReverseFlag ) ;		// 画像ファイル若しくはメモリ上に展開された画像ファイルイメージを読み込み、基本イメージデータを構築する
extern	int			CreateGraphImage_plus_AlphaWithStrLen( const TCHAR *FileName, size_t FileNameLength, const void *RgbImage, int RgbImageSize, int RgbImageType, const void *AlphaImage, int AlphaImageSize, int AlphaImageType, BASEIMAGE *RgbGraphImage, BASEIMAGE *AlphaGraphImage, int ReverseFlag ) ;		// 画像ファイル若しくはメモリ上に展開された画像ファイルイメージを読み込み、基本イメージデータを構築する
extern	int			ReverseGraphImage(                     BASEIMAGE *GraphImage ) ;																																																								// 基本イメージデータを左右反転する

//extern int		AddUserGraphLoadFunction( int ( *UserLoadFunc )( FILE *fp, BITMAPINFO **BmpInfo, void **GraphData ) ) ;																						// ユーザー定義の画像データ読み込み関数を登録する
//extern int		AddUserGraphLoadFunction2( int ( *UserLoadFunc )( void *Image, int ImageSize, int ImageType, BITMAPINFO **BmpInfo, void **GraphData ) ) ;													// ユーザー定義の画像データ読み込み関数を登録する
//extern int		AddUserGraphLoadFunction3( int ( *UserLoadFunc )( void *DataImage, int DataImageSize, int DataImageType, int BmpFlag, BASEIMAGE *BaseImage, BITMAPINFO **BmpInfo, void **GraphData ) ) ;	// ユーザー定義の画像データ読み込み関数Ver3を登録する
extern	int			AddUserGraphLoadFunction4( int (* UserLoadFunc )( STREAMDATA *Src, BASEIMAGE *BaseImage ) ) ; 																								// ユーザー定義の画像データ読み込み関数Ver4を登録する
//extern int		SubUserGraphLoadFunction( int ( *UserLoadFunc )( FILE *fp, BITMAPINFO **BmpInfo, void **GraphData ) ) ;																						// ユーザー定義の画像データ読み込み関数を登録から抹消する
//extern int		SubUserGraphLoadFunction2( int ( *UserLoadFunc )( void *Image, int ImageSize, int ImageType, BITMAPINFO **BmpInfo, void **GraphData ) ) ;													// ユーザー定義の画像データ読み込み関数を登録から抹消する
//extern int		SubUserGraphLoadFunction3( int ( *UserLoadFunc )( void *DataImage, int DataImageSize, int DataImageType, int BmpFlag, BASEIMAGE *BaseImage, BITMAPINFO **BmpInfo, void **GraphData ) ) ;	// ユーザー定義の画像データ読み込み関数Ver3を登録から抹消する
extern	int			SubUserGraphLoadFunction4( int (* UserLoadFunc )( STREAMDATA *Src, BASEIMAGE *BaseImage ) ) ; 																								// ユーザー定義の画像データ読み込み関数Ver4を登録から抹消する

extern	int			SetUseFastLoadFlag(              int Flag ) ;														// 高速読み込みルーチンを使用するかどうかを設定する( TRUE:使用する( デフォルト )  FALSE:使用しない )
extern	int			SetGraphDataShavedMode(          int ShavedMode /* DX_SHAVEDMODE_NONE 等 */ ) ;						// 画像減色時の画像劣化緩和処理モードを設定する( デフォルトでは緩和処理無しの DX_SHAVEDMODE_NONE )
extern	int			GetGraphDataShavedMode(          void ) ;															// 画像減色時の画像劣化緩和処理モードを取得する
extern	int			SetUsePremulAlphaConvertLoad(    int UseFlag ) ;													// 画像ファイル読み込み時に乗算済みアルファ画像に変換するかどうかを設定する( TRUE:変換処理を行う  FALSE:変換処理を行わない( デフォルト ) )

// 基本イメージデータ構造体関係
extern	int			CreateBaseImage(                 const TCHAR *FileName,                        const void *FileImage, int FileImageSize, int DataType /*=LOADIMAGE_TYPE_FILE*/ , BASEIMAGE *BaseImage,  int ReverseFlag ) ;			// 画像ファイル若しくはメモリ上に展開された画像ファイルイメージから基本イメージデータを構築する
extern	int			CreateBaseImageWithStrLen(       const TCHAR *FileName, size_t FileNameLength, const void *FileImage, int FileImageSize, int DataType /*=LOADIMAGE_TYPE_FILE*/ , BASEIMAGE *BaseImage,  int ReverseFlag ) ;			// 画像ファイル若しくはメモリ上に展開された画像ファイルイメージから基本イメージデータを構築する
extern	int			CreateGraphImage(                const TCHAR *FileName,                        const void *DataImage, int DataImageSize, int DataImageType,                      BASEIMAGE *GraphImage, int ReverseFlag ) ;			// CreateBaseImage の旧名称
extern	int			CreateBaseImageToFile(           const TCHAR *FileName,                                                                                                          BASEIMAGE *BaseImage,  int ReverseFlag = FALSE ) ;	// 画像ファイルから基本イメージデータを構築する
extern	int			CreateBaseImageToFileWithStrLen( const TCHAR *FileName, size_t FileNameLength,                                                                                   BASEIMAGE *BaseImage,  int ReverseFlag = FALSE ) ;	// 画像ファイルから基本イメージデータを構築する
extern	int			CreateBaseImageToMem(                                                          const void *FileImage, int FileImageSize,                                         BASEIMAGE *BaseImage,  int ReverseFlag = FALSE ) ;	// メモリ上に展開された画像ファイルイメージから基本イメージデータを構築する
extern	int			CreateARGBF32ColorBaseImage(     int SizeX, int SizeY,                             BASEIMAGE *BaseImage ) ;																							// ＡＲＧＢ各チャンネル 32bit 浮動小数点型 カラーの基本イメージデータを作成する
extern	int			CreateARGBF16ColorBaseImage(     int SizeX, int SizeY,                             BASEIMAGE *BaseImage ) ;																							// ＡＲＧＢ各チャンネル 16bit 浮動小数点型 カラーの基本イメージデータを作成する
extern	int			CreateARGB8ColorBaseImage(       int SizeX, int SizeY,                             BASEIMAGE *BaseImage ) ;																							// ＡＲＧＢ８カラーの基本イメージデータを作成する
extern	int			CreateXRGB8ColorBaseImage(       int SizeX, int SizeY,                             BASEIMAGE *BaseImage ) ;																							// ＸＲＧＢ８カラーの基本イメージデータを作成する
extern	int			CreateRGB8ColorBaseImage(        int SizeX, int SizeY,                             BASEIMAGE *BaseImage ) ;																							// ＲＧＢ８カラーの基本イメージデータを作成する
extern	int			CreateARGB4ColorBaseImage(       int SizeX, int SizeY,                             BASEIMAGE *BaseImage ) ;																							// ＡＲＧＢ４カラーの基本イメージデータを作成する
extern	int			CreateA1R5G5B5ColorBaseImage(    int SizeX, int SizeY,                             BASEIMAGE *BaseImage ) ;																							// Ａ１Ｒ５Ｇ５Ｂ５カラーの基本イメージデータを作成する
extern	int			CreateX1R5G5B5ColorBaseImage(    int SizeX, int SizeY,                             BASEIMAGE *BaseImage ) ;																							// Ｘ１Ｒ５Ｇ５Ｂ５カラーの基本イメージデータを作成する
extern	int			CreateR5G5B5A1ColorBaseImage(    int SizeX, int SizeY,                             BASEIMAGE *BaseImage ) ;																							// Ｒ５Ｇ５Ｂ５Ａ１カラーの基本イメージデータを作成する
extern	int			CreateR5G6B5ColorBaseImage(      int SizeX, int SizeY,                             BASEIMAGE *BaseImage ) ;																							// Ｒ５Ｇ６Ｂ５カラーの基本イメージデータを作成する
extern	int			CreatePAL8ColorBaseImage(        int SizeX, int SizeY,                             BASEIMAGE *BaseImage, int UseAlpha = FALSE ) ;																	// パレット８ビットカラーの基本イメージデータを作成する
extern	int			CreateColorDataBaseImage(        int SizeX, int SizeY, const COLORDATA *ColorData, BASEIMAGE *BaseImage ) ;																							// 指定のカラーフォーマットの基本イメージデータを作成する
extern	int			GetBaseImageGraphDataSize(       const BASEIMAGE *BaseImage ) ;																																		// 基本イメージデータのイメージサイズを取得する( 単位：byte )
extern	int			DerivationBaseImage(             const BASEIMAGE *BaseImage, int x1, int y1, int x2, int y2, BASEIMAGE *NewBaseImage ) ;																			// 基本イメージデータの指定の部分だけを使う基本イメージデータの情報を作成する( BaseImage にはフォーマットが DX_BASEIMAGE_FORMAT_NORMAL でミップマップを使用していない画像のみ使用可能 )

extern	int			ReleaseBaseImage(                BASEIMAGE *BaseImage ) ;																																			// 基本イメージデータの後始末を行う
extern	int			ReleaseGraphImage(               BASEIMAGE *GraphImage ) ;																																			// ReleaseBaseImage の旧名称

extern	int			ConvertNormalFormatBaseImage(    BASEIMAGE *BaseImage, int ReleaseOrigGraphData = TRUE ) ;																											// DX_BASEIMAGE_FORMAT_NORMAL 以外の形式のイメージを DX_BASEIMAGE_FORMAT_NORMAL 形式のイメージに変換する
extern	int			ConvertPremulAlphaBaseImage(     BASEIMAGE *BaseImage ) ;																																			// 通常のαチャンネル付き画像を乗算済みαチャンネル付き画像に変換する( ピクセルフォーマットが ARGB8 以外の場合は ARGB8 に変換されます )
extern	int			ConvertInterpAlphaBaseImage(     BASEIMAGE *BaseImage ) ;																																			// 乗算済みαチャンネル付き画像を通常のαチャンネル付き画像に変換する( ピクセルフォーマットが ARGB8 以外の場合は ARGB8 に変換されます )

extern	int			GetDrawScreenBaseImage(          int x1, int y1, int x2, int y2, BASEIMAGE *BaseImage ) ;																											// 描画対象の画面から指定領域を基本イメージデータに転送する
extern	int			GetDrawScreenBaseImageDestPos(   int x1, int y1, int x2, int y2, BASEIMAGE *BaseImage, int DestX, int DestY ) ;																						// 描画対象の画面から指定領域を基本イメージデータに転送する( 転送先座標指定版 )
extern	int			FillBaseImage(                         BASEIMAGE *BaseImage, int r, int g, int b, int a ) ;																											// 基本イメージデータを指定の色で塗りつぶす
extern	int			FillRectBaseImage(                     BASEIMAGE *BaseImage, int x, int y, int w, int h, int r, int g, int b, int a ) ;																				// 基本イメージデータの指定の領域を指定の色で塗りつぶす
extern	int			ClearRectBaseImage(                    BASEIMAGE *BaseImage, int x, int y, int w, int h ) ;																											// 基本イメージデータの指定の領域を０クリアする
extern	int			GetPaletteBaseImage(             const BASEIMAGE *BaseImage, int PaletteNo, int *r, int *g, int *b, int *a ) ;																						// 基本イメージデータのパレットを取得する
extern	int			SetPaletteBaseImage(                   BASEIMAGE *BaseImage, int PaletteNo, int  r, int  g, int  b, int  a ) ;																						// 基本イメージデータのパレットをセットする
extern	int			SetPixelPalCodeBaseImage(              BASEIMAGE *BaseImage, int x, int y, int palNo ) ;																											// 基本イメージデータの指定の座標の色コードを変更する(パレット画像用)
extern	int			GetPixelPalCodeBaseImage(        const BASEIMAGE *BaseImage, int x, int y ) ;																														// 基本イメージデータの指定の座標の色コードを取得する(パレット画像用)
extern	int			SetPixelBaseImage(                     BASEIMAGE *BaseImage, int x, int y, int    r, int    g, int    b, int    a ) ;																				// 基本イメージデータの指定の座標の色を変更する(各色要素は０〜２５５)
extern	int			SetPixelBaseImageF(                    BASEIMAGE *BaseImage, int x, int y, float  r, float  g, float  b, float  a ) ;																				// 基本イメージデータの指定の座標の色を変更する(各色要素は浮動小数点数)
extern	int			GetPixelBaseImage(               const BASEIMAGE *BaseImage, int x, int y, int   *r, int   *g, int   *b, int   *a ) ;																				// 基本イメージデータの指定の座標の色を取得する(各色要素は０〜２５５)
extern	int			GetPixelBaseImageF(              const BASEIMAGE *BaseImage, int x, int y, float *r, float *g, float *b, float *a ) ;																				// 基本イメージデータの指定の座標の色を取得する(各色要素は浮動小数点数)
extern	int			DrawLineBaseImage(                     BASEIMAGE *BaseImage, int x1, int y1, int x2, int y2, int r, int g, int b, int a ) ;																			// 基本イメージデータの指定の座標に線を描画する(各色要素は０〜２５５)
extern	int			DrawCircleBaseImage(                   BASEIMAGE *BaseImage, int x, int y, int radius, int r, int g, int b, int a, int FillFlag = TRUE ) ;															// 基本イメージデータの指定の座標に円を描画する(各色要素は０〜２５５)
extern	int			BltBaseImage(                          int SrcX, int SrcY, int SrcSizeX, int SrcSizeY, int DestX, int DestY, BASEIMAGE *SrcBaseImage, BASEIMAGE *DestBaseImage ) ;									// 基本イメージデータを別の基本イメージデータに転送する
extern	int			BltBaseImage(                                                                          int DestX, int DestY, BASEIMAGE *SrcBaseImage, BASEIMAGE *DestBaseImage ) ;									// 基本イメージデータを別の基本イメージデータに転送する
extern	int			BltBaseImageWithTransColor(            int SrcX, int SrcY, int SrcSizeX, int SrcSizeY, int DestX, int DestY, BASEIMAGE *SrcBaseImage, BASEIMAGE *DestBaseImage, int Tr, int Tg, int Tb, int Ta ) ;	// 基本イメージデータを別の基本イメージデータに透過色処理付きで転送する
extern	int			BltBaseImageWithAlphaBlend(            int SrcX, int SrcY, int SrcSizeX, int SrcSizeY, int DestX, int DestY, BASEIMAGE *SrcBaseImage, BASEIMAGE *DestBaseImage, int Opacity = 255 ) ;				// 基本イメージデータを別の基本イメージデータにアルファ値のブレンドを考慮した上で転送する( Opacity は透明度 : 0( 完全透明 ) 〜 255( 完全不透明 ) )( 出力先が ARGB8 形式以外の場合はエラーになります )
extern	int			ReverseBaseImageH(                     BASEIMAGE *BaseImage ) ;																																		// 基本イメージデータを左右反転する
extern	int			ReverseBaseImageV(                     BASEIMAGE *BaseImage ) ;																																		// 基本イメージデータを上下反転する
extern	int			ReverseBaseImage(                      BASEIMAGE *BaseImage ) ;																																		// 基本イメージデータを上下左右反転する
extern	int			CheckPixelAlphaBaseImage(        const BASEIMAGE *BaseImage ) ;																																		// 基本イメージデータに含まれるピクセルのアルファ値をチェックする( 戻り値   -1:エラー  0:画像にアルファ成分が無い  1:画像にアルファ成分があり、すべて最大(255)値  2:画像にアルファ成分があり、存在するアルファ値は最小(0)と最大(255)もしくは最小(0)のみ　3:画像にアルファ成分があり、最小と最大以外の中間の値がある )  
extern	int			GetBaseImageUseMaxPaletteNo(     const BASEIMAGE *BaseImage ) ;																																		// 基本イメージデータで使用されているパレット番号の最大値を取得する( パレット画像では無い場合は -1 が返る )

#ifndef DX_NON_JPEGREAD
extern	int			ReadJpegExif(                    const TCHAR *JpegFilePath,                            BYTE *ExifBuffer_Array, size_t ExifBufferSize ) ;															// JPEGファイルの Exif情報を取得する、ExifBuffer を NULL に渡すと、戻り値の情報のサイズのみ取得できます( 戻り値  -1:エラー  -1以外：Exif情報のサイズ )
extern	int			ReadJpegExifWithStrLen(          const TCHAR *JpegFilePath, size_t JpegFilePathLength, BYTE *ExifBuffer_Array, size_t ExifBufferSize ) ;															// JPEGファイルの Exif情報を取得する、ExifBuffer を NULL に渡すと、戻り値の情報のサイズのみ取得できます( 戻り値  -1:エラー  -1以外：Exif情報のサイズ )
#endif // DX_NON_JPEGREAD

#ifndef DX_NON_SAVEFUNCTION

extern	int			SaveBaseImageToBmp(              const TCHAR *FilePath,                        const BASEIMAGE *BaseImage ) ;																						// 基本イメージデータをＢＭＰ画像として保存する
extern	int			SaveBaseImageToBmpWithStrLen(    const TCHAR *FilePath, size_t FilePathLength, const BASEIMAGE *BaseImage ) ;																						// 基本イメージデータをＢＭＰ画像として保存する
extern	int			SaveBaseImageToDds(              const TCHAR *FilePath,                        const BASEIMAGE *BaseImage ) ;																						// 基本イメージデータをＤＤＳ画像として保存する
extern	int			SaveBaseImageToDdsWithStrLen(    const TCHAR *FilePath, size_t FilePathLength, const BASEIMAGE *BaseImage ) ;																						// 基本イメージデータをＤＤＳ画像として保存する
#ifndef DX_NON_PNGREAD
extern	int			SaveBaseImageToPng(              const TCHAR *FilePath,                        BASEIMAGE *BaseImage, int CompressionLevel ) ;																		// 基本イメージデータをＰＮＧ画像として保存する
extern	int			SaveBaseImageToPngWithStrLen(    const TCHAR *FilePath, size_t FilePathLength, BASEIMAGE *BaseImage, int CompressionLevel ) ;																		// 基本イメージデータをＰＮＧ画像として保存する
#endif // DX_NON_PNGREAD
#ifndef DX_NON_JPEGREAD
extern	int			SaveBaseImageToJpeg(             const TCHAR *FilePath,                        BASEIMAGE *BaseImage, int Quality, int Sample2x1 ) ;																	// 基本イメージデータをＪＰＥＧ画像として保存する
extern	int			SaveBaseImageToJpegWithStrLen(   const TCHAR *FilePath, size_t FilePathLength, BASEIMAGE *BaseImage, int Quality, int Sample2x1 ) ;																	// 基本イメージデータをＪＰＥＧ画像として保存する
#endif // DX_NON_JPEGREAD

#endif // DX_NON_SAVEFUNCTION

// 基本イメージ描画
extern	int			DrawBaseImage(					int x, int y, BASEIMAGE *BaseImage ) ;																																// 基本イメージデータを描画する

// カラーマッチングしながらイメージデータ間転送を行う Ver2
extern int			GraphColorMatchBltVer2(       void *DestGraphData, int DestPitch,  const COLORDATA *DestColorData,
											const void *SrcGraphData,  int SrcPitch,   const COLORDATA *SrcColorData,
											const void *AlphaMask,     int AlphaPitch, const COLORDATA *AlphaColorData,
											POINT DestPoint, const RECT *SrcRect, int ReverseFlag,
											int TransColorAlphaTestFlag, unsigned int TransColor,
											int ImageShavedMode, int AlphaOnlyFlag = FALSE ,
											int RedIsAlphaFlag = FALSE , int TransColorNoMoveFlag = FALSE ,
											int Pal8ColorMatch = FALSE ) ;


// 色情報取得関係
extern	COLOR_F			GetColorF(               float Red, float Green, float Blue, float Alpha ) ;													// 浮動小数点型のカラー値を作成する
extern	COLOR_U8		GetColorU8(              int Red, int Green, int Blue, int Alpha ) ;															// 符号なし整数８ビットのカラー値を作成する
extern	unsigned int	GetColor(                int Red, int Green, int Blue ) ;																		// DrawPixel 等の描画関数で使用するカラー値を取得する
extern	int				GetColor2(               unsigned int Color, int *Red, int *Green, int *Blue ) ;												// カラー値から赤、緑、青の値を取得する
extern	unsigned int	GetColor3(               const COLORDATA *ColorData, int Red, int Green, int Blue, int Alpha = 255 ) ;							// 指定のピクセルフォーマットに対応したカラー値を得る
extern	unsigned int	GetColor4(               const COLORDATA *DestColorData, const COLORDATA* SrcColorData, unsigned int SrcColor ) ;				// 指定のカラーフォーマットのカラー値を別のカラーフォーマットのカラー値に変換する
extern	int				GetColor5(               const COLORDATA *ColorData, unsigned int Color, int *Red, int *Green, int *Blue, int *Alpha = NULL ) ;	// 指定のカラーフォーマットのカラー値を赤、緑、青、アルファの値を取得する
extern	int				CreatePaletteColorData(  COLORDATA *ColorDataBuf ) ;																			// パレットカラーのカラーフォーマットを構築する
extern	int				CreateARGBF32ColorData(  COLORDATA *ColorDataBuf ) ;																			// ＡＲＧＢ各チャンネル 32bit 浮動小数点型カラーのカラーフォーマットを構築する
extern	int				CreateARGBF16ColorData(  COLORDATA *ColorDataBuf ) ;																			// ＡＲＧＢ各チャンネル 16bit 浮動小数点型カラーのカラーフォーマットを構築する
extern	int				CreateXRGB8ColorData(    COLORDATA *ColorDataBuf ) ;																			// ＸＲＧＢ８カラーのカラーフォーマットを構築する
extern	int				CreateARGB8ColorData(    COLORDATA *ColorDataBuf ) ;																			// ＡＲＧＢ８カラーのカラーフォーマットを構築する
extern	int				CreateARGB4ColorData(    COLORDATA *ColorDataBuf ) ;																			// ＡＲＧＢ４カラーのカラーフォーマットを構築する
extern	int				CreateA1R5G5B5ColorData( COLORDATA *ColorDataBuf ) ;																			// Ａ１Ｒ５Ｇ５Ｂ５カラーのカラーフォーマットを構築する
extern	int				CreateX1R5G5B5ColorData( COLORDATA *ColorDataBuf ) ;																			// Ｘ１Ｒ５Ｇ５Ｂ５カラーのカラーフォーマットを構築する
extern	int				CreateR5G5B5A1ColorData( COLORDATA *ColorDataBuf ) ;																			// Ｒ５Ｇ５Ｂ５Ａ１カラーのカラーフォーマットを構築する
extern	int				CreateR5G6B5ColorData(   COLORDATA *ColorDataBuf ) ;																			// Ｒ５Ｇ６Ｂ５カラーのカラーフォーマットを構築する
extern	int				CreateFullColorData(     COLORDATA *ColorDataBuf ) ;																			// ２４ビットカラーのカラーフォーマットを構築する
extern	int				CreateGrayColorData(     COLORDATA *ColorDataBuf ) ;																			// グレースケールのカラーフォーマットを構築する
extern	int				CreatePal8ColorData(     COLORDATA *ColorDataBuf, int UseAlpha = FALSE ) ;														// パレット２５６色のカラーフォーマットを構築する
extern	int				CreateColorData(         COLORDATA *ColorDataBuf, int ColorBitDepth,
										         DWORD RedMask, DWORD GreenMask, DWORD BlueMask, DWORD AlphaMask,
												 int ChannelNum = 0, int ChannelBitDepth = 0, int FloatTypeFlag = FALSE ) ;								// カラーフォーマットを作成する
extern	void			SetColorDataNoneMask(    COLORDATA *ColorData ) ;																				// NoneMask 以外の要素を埋めた COLORDATA 構造体の情報を元に NoneMask をセットする
extern	int				CmpColorData(            const COLORDATA *ColorData1, const COLORDATA *ColorData2 ) ;											// 二つのカラーフォーマットが等しいかどうか調べる( 戻り値　TRUE:等しい  FALSE:等しくない )













// DxSoftImage.cpp関数プロトタイプ宣言
#ifndef DX_NON_SOFTIMAGE
extern	int			InitSoftImage(                        void ) ;																			// ソフトウエアイメージハンドルを全て削除する
extern	int			LoadSoftImage(                        const TCHAR *FileName                        ) ;									// 画像ファイルを読み込みソフトウエアイメージハンドルを作成する( -1:エラー  -1以外:イメージハンドル )
extern	int			LoadSoftImageWithStrLen(              const TCHAR *FileName, size_t FileNameLength ) ;									// 画像ファイルを読み込みソフトウエアイメージハンドルを作成する( -1:エラー  -1以外:イメージハンドル )
extern	int			LoadARGB8ColorSoftImage(              const TCHAR *FileName                        ) ;									// 画像ファイルを読み込みソフトウエアイメージハンドルを作成する( -1:エラー  -1以外:イメージハンドル )( 読み込んだ画像が RGBA8 以外のフォーマットだった場合は RGBA8 カラーに変換 )
extern	int			LoadARGB8ColorSoftImageWithStrLen(    const TCHAR *FileName, size_t FileNameLength ) ;									// 画像ファイルを読み込みソフトウエアイメージハンドルを作成する( -1:エラー  -1以外:イメージハンドル )( 読み込んだ画像が RGBA8 以外のフォーマットだった場合は RGBA8 カラーに変換 )
extern	int			LoadXRGB8ColorSoftImage(              const TCHAR *FileName                        ) ;									// 画像ファイルを読み込みソフトウエアイメージハンドルを作成する( -1:エラー  -1以外:イメージハンドル )( 読み込んだ画像が XGBA8 以外のフォーマットだった場合は XGBA8 カラーに変換 )
extern	int			LoadXRGB8ColorSoftImageWithStrLen(    const TCHAR *FileName, size_t FileNameLength ) ;									// 画像ファイルを読み込みソフトウエアイメージハンドルを作成する( -1:エラー  -1以外:イメージハンドル )( 読み込んだ画像が XGBA8 以外のフォーマットだった場合は XGBA8 カラーに変換 )
extern	int			LoadSoftImageToMem(                   const void *FileImage, int FileImageSize ) ;										// メモリ上に展開された画像ファイルイメージからソフトウエアイメージハンドルを作成する( -1:エラー  -1以外:イメージハンドル )
extern	int			LoadARGB8ColorSoftImageToMem(         const void *FileImage, int FileImageSize ) ;										// メモリ上に展開された画像ファイルイメージからソフトウエアイメージハンドルを作成する( -1:エラー  -1以外:イメージハンドル )( 読み込んだ画像が RGBA8 以外のフォーマットだった場合は RGBA8 カラーに変換 )
extern	int			LoadXRGB8ColorSoftImageToMem(         const void *FileImage, int FileImageSize ) ;										// メモリ上に展開された画像ファイルイメージからソフトウエアイメージハンドルを作成する( -1:エラー  -1以外:イメージハンドル )( 読み込んだ画像が XGBA8 以外のフォーマットだった場合は XGBA8 カラーに変換 )
extern	int			MakeSoftImage(                        int SizeX, int SizeY ) ;															// ソフトウエアイメージハンドルの作成( -1:エラー  -1以外:イメージハンドル )
extern	int			MakeARGBF32ColorSoftImage(            int SizeX, int SizeY ) ;															// ソフトウエアイメージハンドルの作成( RGBA 各チャンネル 32bit 浮動小数点型 カラー )
extern	int			MakeARGBF16ColorSoftImage(            int SizeX, int SizeY ) ;															// ソフトウエアイメージハンドルの作成( RGBA 各チャンネル 16bit 浮動小数点型 カラー )
extern	int			MakeARGB8ColorSoftImage(              int SizeX, int SizeY ) ;															// ソフトウエアイメージハンドルの作成( RGBA8 カラー )
extern	int			MakeXRGB8ColorSoftImage(              int SizeX, int SizeY ) ;															// ソフトウエアイメージハンドルの作成( XRGB8 カラー )
extern	int			MakeARGB4ColorSoftImage(              int SizeX, int SizeY ) ;															// ソフトウエアイメージハンドルの作成( ARGB4 カラー )
extern	int			MakeA1R5G5B5ColorSoftImage(           int SizeX, int SizeY ) ;															// ソフトウエアイメージハンドルの作成( A1R5G5B5 カラー )
extern	int			MakeX1R5G5B5ColorSoftImage(           int SizeX, int SizeY ) ;															// ソフトウエアイメージハンドルの作成( X1R5G5B5 カラー )
extern	int			MakeR5G5B5A1ColorSoftImage(           int SizeX, int SizeY ) ;															// ソフトウエアイメージハンドルの作成( R5G5B5A1 カラー )
extern	int			MakeR5G6B5ColorSoftImage(             int SizeX, int SizeY ) ;															// ソフトウエアイメージハンドルの作成( R5G6B5 カラー )
extern	int			MakeRGB8ColorSoftImage(               int SizeX, int SizeY ) ;															// ソフトウエアイメージハンドルの作成( RGB8 カラー )
extern	int			MakePAL8ColorSoftImage(               int SizeX, int SizeY, int UseAlpha = FALSE ) ;									// ソフトウエアイメージハンドルの作成( パレット２５６色 カラー )

extern	int			DeleteSoftImage(                      int SIHandle ) ;																	// ソフトウエアイメージハンドルの削除する

extern	int			GetSoftImageSize(                     int SIHandle, int *Width, int *Height ) ;											// ソフトウエアイメージハンドルのサイズを取得する
extern	int			CheckPaletteSoftImage(                int SIHandle ) ;																	// ソフトウエアイメージハンドルがパレット画像かどうかを取得する( TRUE:パレット画像  FALSE:パレット画像じゃない )
extern	int			CheckAlphaSoftImage(                  int SIHandle ) ;																	// ソフトウエアイメージハンドルのフォーマットにα要素があるかどうかを取得する( TRUE:ある  FALSE:ない )
extern	int			CheckPixelAlphaSoftImage(             int SIHandle ) ;																	// ソフトウエアイメージハンドルに含まれるピクセルのα値をチェックする( 戻り値   -1:エラー  0:画像にα成分が無い  1:画像にα成分があり、すべて最大(255)値  2:画像にα成分があり、存在するα値は最小(0)と最大(255)もしくは最小(0)のみ　3:画像にα成分があり、最小と最大以外の中間の値がある )  

extern	int			GetDrawScreenSoftImage(               int x1, int y1, int x2, int y2, int SIHandle ) ;									// 描画対象の画面から指定領域をソフトウエアイメージハンドルに転送する
extern	int			GetDrawScreenSoftImageDestPos(        int x1, int y1, int x2, int y2, int SIHandle, int DestX, int DestY ) ;			// 描画対象の画面から指定領域をソフトウエアイメージハンドルに転送する( 転送先座標指定版 )
extern	int			FillSoftImage(                        int SIHandle, int r, int g, int b, int a ) ;										// ソフトウエアイメージハンドルを指定色で塗りつぶす(各色要素は０〜２５５)
extern	int			ClearRectSoftImage(                   int SIHandle, int x, int y, int w, int h ) ;										// ソフトウエアイメージハンドルの指定の領域を０クリアする
extern	int			GetPaletteSoftImage(                  int SIHandle, int PaletteNo, int *r, int *g, int *b, int *a ) ;					// ソフトウエアイメージハンドルのパレットを取得する(各色要素は０〜２５５)
extern	int			SetPaletteSoftImage(                  int SIHandle, int PaletteNo, int  r, int  g, int  b, int  a ) ;					// ソフトウエアイメージハンドルのパレットを設定する(各色要素は０〜２５５)
extern	int			DrawPixelPalCodeSoftImage(            int SIHandle, int x, int y, int palNo ) ;											// ソフトウエアイメージハンドルの指定座標にドットを描画する(パレット画像用、有効値は０〜２５５)
extern	int			GetPixelPalCodeSoftImage(             int SIHandle, int x, int y ) ;													// ソフトウエアイメージハンドルの指定座標の色コードを取得する(パレット画像用、戻り値は０〜２５５)
extern	void		*GetImageAddressSoftImage(            int SIHandle ) ;																	// ソフトウエアイメージハンドルの画像が格納されているメモリ領域の先頭アドレスを取得する
extern	int			GetPitchSoftImage(                    int SIHandle ) ;																	// ソフトウエアイメージハンドルのメモリに格納されている画像データの1ライン辺りのバイト数を取得する
extern	int			DrawPixelSoftImage(                   int SIHandle, int x, int y, int    r, int    g, int    b, int    a ) ;			// ソフトウエアイメージハンドルの指定座標にドットを描画する(各色要素は０〜２５５)
extern	int			DrawPixelSoftImageF(                  int SIHandle, int x, int y, float  r, float  g, float  b, float  a ) ;			// ソフトウエアイメージハンドルの指定座標にドットを描画する(各色要素は浮動小数点数)
extern	void		DrawPixelSoftImage_Unsafe_XRGB8(      int SIHandle, int x, int y, int    r, int    g, int    b ) ;						// ソフトウエアイメージハンドルの指定座標にドットを描画する(各色要素は０〜２５５)、エラーチェックをしない代わりに高速ですが、範囲外の座標や ARGB8 以外のフォーマットのソフトハンドルを渡すと不正なメモリアクセスで強制終了します
extern	void		DrawPixelSoftImage_Unsafe_ARGB8(      int SIHandle, int x, int y, int    r, int    g, int    b, int    a ) ;			// ソフトウエアイメージハンドルの指定座標にドットを描画する(各色要素は０〜２５５)、エラーチェックをしない代わりに高速ですが、範囲外の座標や XRGB8 以外のフォーマットのソフトハンドルを渡すと不正なメモリアクセスで強制終了します
extern	int			GetPixelSoftImage(                    int SIHandle, int x, int y, int   *r, int   *g, int   *b, int   *a ) ;			// ソフトウエアイメージハンドルの指定座標の色を取得する(各色要素は０〜２５５)
extern	int			GetPixelSoftImageF(                   int SIHandle, int x, int y, float *r, float *g, float *b, float *a ) ;			// ソフトウエアイメージハンドルの指定座標の色を取得する(各色要素は浮動小数点数)
extern	void		GetPixelSoftImage_Unsafe_XRGB8(       int SIHandle, int x, int y, int   *r, int   *g, int   *b ) ;						// ソフトウエアイメージハンドルの指定座標の色を取得する(各色要素は０〜２５５)、エラーチェックをしない代わりに高速ですが、範囲外の座標や XRGB8 以外のフォーマットのソフトハンドルを渡すと不正なメモリアクセスで強制終了します
extern	void		GetPixelSoftImage_Unsafe_ARGB8(       int SIHandle, int x, int y, int   *r, int   *g, int   *b, int   *a ) ;			// ソフトウエアイメージハンドルの指定座標の色を取得する(各色要素は０〜２５５)、エラーチェックをしない代わりに高速ですが、範囲外の座標や ARGB8 以外のフォーマットのソフトハンドルを渡すと不正なメモリアクセスで強制終了します
extern	int			DrawLineSoftImage(                    int SIHandle, int x1, int y1, int x2, int y2, int r, int g, int b, int a ) ;		// ソフトウエアイメージハンドルの指定座標に線を描画する(各色要素は０〜２５５)
extern	int			DrawCircleSoftImage(                  int SIHandle, int x, int y, int radius, int r, int g, int b, int a, int FillFlag = TRUE ) ;	// ソフトウエアイメージハンドルの指定座標に円を描画する(各色要素は０〜２５５)
extern	int			BltSoftImage(                         int SrcX, int SrcY, int SrcSizeX, int SrcSizeY, int SrcSIHandle, int DestX, int DestY, int DestSIHandle ) ;									// ソフトウエアイメージハンドルを別のソフトウエアイメージハンドルに転送する
extern	int			BltSoftImageWithTransColor(           int SrcX, int SrcY, int SrcSizeX, int SrcSizeY, int SrcSIHandle, int DestX, int DestY, int DestSIHandle, int Tr, int Tg, int Tb, int Ta ) ;	// ソフトウエアイメージハンドルを別のソフトウエアイメージハンドルに透過色処理付きで転送する
extern	int			BltSoftImageWithAlphaBlend(           int SrcX, int SrcY, int SrcSizeX, int SrcSizeY, int SrcSIHandle, int DestX, int DestY, int DestSIHandle, int Opacity = 255 ) ;				// ソフトウエアイメージハンドルを別のソフトウエアイメージハンドルにアルファ値のブレンドを考慮した上で転送する( Opacity は透明度 : 0( 完全透明 ) 〜 255( 完全不透明 ) )( 出力先が ARGB8 形式以外の場合はエラーになります )
extern	int			ReverseSoftImageH(                    int SIHandle ) ;																	// ソフトウエアイメージハンドルを左右反転する
extern	int			ReverseSoftImageV(                    int SIHandle ) ;																	// ソフトウエアイメージハンドルを上下反転する
extern	int			ReverseSoftImage(                     int SIHandle ) ;																	// ソフトウエアイメージハンドルを上下左右反転する

#ifndef DX_NON_FONT
extern	int			BltStringSoftImage(                   int x, int y, const TCHAR *StrData,                       int DestSIHandle, int DestEdgeSIHandle = -1 ,                                        int VerticalFlag = FALSE ) ;		// ソフトウエアイメージハンドルに文字列を描画する( デフォルトフォントハンドルを使用する )
extern	int			BltStringSoftImageWithStrLen(         int x, int y, const TCHAR *StrData, size_t StrDataLength, int DestSIHandle, int DestEdgeSIHandle = -1 ,                                        int VerticalFlag = FALSE ) ;		// ソフトウエアイメージハンドルに文字列を描画する( デフォルトフォントハンドルを使用する )
extern	int			BltStringSoftImageToHandle(           int x, int y, const TCHAR *StrData,                       int DestSIHandle, int DestEdgeSIHandle /* 縁が必要ない場合は -1 */ , int FontHandle, int VerticalFlag = FALSE ) ;		// ソフトウエアイメージハンドルに文字列を描画する( フォントハンドル使用版 )
extern	int			BltStringSoftImageToHandleWithStrLen( int x, int y, const TCHAR *StrData, size_t StrDataLength, int DestSIHandle, int DestEdgeSIHandle /* 縁が必要ない場合は -1 */ , int FontHandle, int VerticalFlag = FALSE ) ;		// ソフトウエアイメージハンドルに文字列を描画する( フォントハンドル使用版 )
#endif // DX_NON_FONT

extern	int			DrawSoftImage(                        int x, int y, int SIHandle ) ;														// ソフトウエアイメージハンドルを画面に描画する

#ifndef DX_NON_SAVEFUNCTION

extern	int			SaveSoftImageToBmp(                   const TCHAR *FilePath,                        int SIHandle ) ;											// ソフトウエアイメージハンドルをＢＭＰ画像ファイルとして保存する
extern	int			SaveSoftImageToBmpWithStrLen(         const TCHAR *FilePath, size_t FilePathLength, int SIHandle ) ;											// ソフトウエアイメージハンドルをＢＭＰ画像ファイルとして保存する
extern	int			SaveSoftImageToDds(                   const TCHAR *FilePath,                        int SIHandle ) ;											// ソフトウエアイメージハンドルをＤＤＳ画像ファイルとして保存する
extern	int			SaveSoftImageToDdsWithStrLen(         const TCHAR *FilePath, size_t FilePathLength, int SIHandle ) ;											// ソフトウエアイメージハンドルをＤＤＳ画像ファイルとして保存する
#ifndef DX_NON_PNGREAD
extern	int			SaveSoftImageToPng(                   const TCHAR *FilePath,                        int SIHandle, int CompressionLevel ) ;						// ソフトウエアイメージハンドルをＰＮＧ画像ファイルとして保存する CompressionLevel = 圧縮率、値が大きいほど高圧縮率高負荷、０は無圧縮,0〜9
extern	int			SaveSoftImageToPngWithStrLen(         const TCHAR *FilePath, size_t FilePathLength, int SIHandle, int CompressionLevel ) ;						// ソフトウエアイメージハンドルをＰＮＧ画像ファイルとして保存する CompressionLevel = 圧縮率、値が大きいほど高圧縮率高負荷、０は無圧縮,0〜9
#endif // DX_NON_PNGREAD
#ifndef DX_NON_JPEGREAD
extern	int			SaveSoftImageToJpeg(                  const TCHAR *FilePath,                        int SIHandle, int Quality, int Sample2x1 ) ;				// ソフトウエアイメージハンドルをＪＰＥＧ画像ファイルとして保存する Quality = 画質、値が大きいほど低圧縮高画質,0〜100 
extern	int			SaveSoftImageToJpegWithStrLen(        const TCHAR *FilePath, size_t FilePathLength, int SIHandle, int Quality, int Sample2x1 ) ;				// ソフトウエアイメージハンドルをＪＰＥＧ画像ファイルとして保存する Quality = 画質、値が大きいほど低圧縮高画質,0〜100 
#endif // DX_NON_JPEGREAD

#endif // DX_NON_SAVEFUNCTION

#endif // DX_NON_SOFTIMAGE


















#ifndef DX_NON_SOUND

// DxSound.cpp関数プロトタイプ宣言

// サウンドデータ管理系関数
extern	int			InitSoundMem(                        int LogOutFlag = FALSE ) ;																	// 全てのサウンドハンドルを削除する

extern	int			AddSoundData(                        int Handle = -1 ) ;																											// サウンドハンドルを作成する
extern	int			AddStreamSoundMem(                   STREAMDATA *Stream, int LoopNum,  int SoundHandle, int StreamDataType, int *CanStreamCloseFlag, int UnionHandle = -1 ) ;		// ストリーム再生タイプのサウンドハンドルにストリームデータを再生対象に追加する
extern	int			AddStreamSoundMemToMem(              const void *FileImage, int FileImageSize, int LoopNum,  int SoundHandle, int StreamDataType, int UnionHandle = -1 ) ;			// ストリーム再生タイプのサウンドハンドルにメモリ上に展開したサウンドファイルイメージを再生対象に追加する
extern	int			AddStreamSoundMemToFile(             const TCHAR *WaveFile,                            int LoopNum,  int SoundHandle, int StreamDataType, int UnionHandle = -1 ) ;	// ストリーム再生タイプのサウンドハンドルにサウンドファイルを再生対象に追加する
extern	int			AddStreamSoundMemToFileWithStrLen(   const TCHAR *WaveFile, size_t WaveFilePathLength, int LoopNum,  int SoundHandle, int StreamDataType, int UnionHandle = -1 ) ;	// ストリーム再生タイプのサウンドハンドルにサウンドファイルを再生対象に追加する
extern	int			SetupStreamSoundMem(                 int SoundHandle ) ;																		// ストリーム再生タイプのサウンドハンドルの再生準備をする
extern	int			PlayStreamSoundMem(                  int SoundHandle, int PlayType = DX_PLAYTYPE_LOOP , int TopPositionFlag = TRUE ) ;			// ストリーム再生タイプのサウンドハンドルの再生を開始する
extern	int			CheckStreamSoundMem(                 int SoundHandle ) ;																		// ストリーム再生タイプのサウンドハンドルの再生状態を取得する
extern	int			StopStreamSoundMem(                  int SoundHandle ) ;																		// ストリーム再生タイプのサウンドハンドルの再生を停止する
extern	int			SetStreamSoundCurrentPosition(       int Byte, int SoundHandle ) ;																// サウンドハンドルの再生位置をバイト単位で変更する(再生が止まっている時のみ有効)
extern	int			GetStreamSoundCurrentPosition(       int SoundHandle ) ;																		// サウンドハンドルの再生位置をバイト単位で取得する
extern	int			SetStreamSoundCurrentTime(           int Time, int SoundHandle ) ;																// サウンドハンドルの再生位置をミリ秒単位で設定する(圧縮形式の場合は正しく設定されない場合がある)
extern	int			GetStreamSoundCurrentTime(           int SoundHandle ) ;																		// サウンドハンドルの再生位置をミリ秒単位で取得する(圧縮形式の場合は正しい値が返ってこない場合がある)
extern	int			ProcessStreamSoundMem(               int SoundHandle ) ;																		// ストリーム再生タイプのサウンドハンドルの周期的な処理を行う関数( 内部で自動的に呼ばれます )
extern	int			ProcessStreamSoundMemAll(            void ) ;																					// 有効なストリーム再生タイプのサウンドハンドルに対して ProcessStreamSoundMem を実行する( 内部で自動的に呼ばれます )


extern	int			LoadSoundMem2(                       const TCHAR *FileName1,                         const TCHAR *FileName2                         ) ;	// 前奏部とループ部に分かれたサウンドファイルを読み込みサウンドハンドルを作成する
extern	int			LoadSoundMem2WithStrLen(             const TCHAR *FileName1, size_t FileName1Length, const TCHAR *FileName2, size_t FileName2Length ) ;	// 前奏部とループ部に分かれたサウンドファイルを読み込みサウンドハンドルを作成する
extern	int			LoadBGM(                             const TCHAR *FileName                        ) ;													// 主にＢＧＭを読み込みサウンドハンドルを作成するのに適した関数
extern	int			LoadBGMWithStrLen(                   const TCHAR *FileName, size_t FileNameLength ) ;													// 主にＢＧＭを読み込みサウンドハンドルを作成するのに適した関数

extern	int			LoadSoundMemBase(                    const TCHAR *FileName,                        int BufferNum,      int UnionHandle = -1 ) ;			// サウンドファイルからサウンドハンドルを作成する
extern	int			LoadSoundMemBaseWithStrLen(          const TCHAR *FileName, size_t FileNameLength, int BufferNum,      int UnionHandle = -1 ) ;			// サウンドファイルからサウンドハンドルを作成する
extern	int			LoadSoundMem(                        const TCHAR *FileName,                        int BufferNum = 3 , int UnionHandle = -1 ) ;			// LoadSoundMemBase の別名関数
extern	int			LoadSoundMemWithStrLen(              const TCHAR *FileName, size_t FileNameLength, int BufferNum = 3 , int UnionHandle = -1 ) ;			// LoadSoundMemBase の別名関数
extern	int			LoadSoundMemToBufNumSitei(           const TCHAR *FileName,                        int BufferNum ) ;									// LoadSoundMem を使用して下さい
extern	int			LoadSoundMemToBufNumSiteiWithStrLen( const TCHAR *FileName, size_t FileNameLength, int BufferNum ) ;									// LoadSoundMem を使用して下さい
extern	int			DuplicateSoundMem(                   int SrcSoundHandle, int BufferNum = 3 ) ;															// 同じサウンドデータを使用するサウンドハンドルを作成する( DX_SOUNDDATATYPE_MEMNOPRESS タイプのサウンドハンドルのみ可能 )

extern	int			LoadSoundMemByMemImageBase(          const void *FileImage, int FileImageSize, int BufferNum,      int UnionHandle = -1 ) ;				// メモリ上に展開されたサウンドファイルイメージからサウンドハンドルを作成する
extern	int			LoadSoundMemByMemImage(              const void *FileImage, int FileImageSize, int BufferNum = 3 , int UnionHandle = -1 ) ;				// LoadSoundMemByMemImageBase の別名関数
extern	int			LoadSoundMemByMemImage2(             const void *WaveImage, int WaveImageSize, const WAVEFORMATEX *WaveFormat, int WaveHeaderSize ) ;	// メモリ上に展開されたＰＣＭデータからサウンドハンドルを作成する
extern	int			LoadSoundMemByMemImageToBufNumSitei( const void *FileImage, int FileImageSize, int BufferNum ) ;										// LoadSoundMemByMemImageBase を使用して下さい
extern	int			LoadSoundMem2ByMemImage(             const void *FileImage1, int FileImageSize1, const void *FileImage2, int FileImageSize2 ) ;			// 前奏部とループ部に分かれた二つのメモリ上に展開されたサウンドファイルイメージからサウンドハンドルを作成する
extern	int			LoadSoundMemFromSoftSound(           int SoftSoundHandle, int BufferNum = 3 ) ;															// ソフトウエアサウンドハンドルが持つサウンドデータからサウンドハンドルを作成する

extern	int			DeleteSoundMem(                      int SoundHandle, int LogOutFlag = FALSE ) ;												// サウンドハンドルを削除する

extern	int			PlaySoundMem(                        int SoundHandle, int PlayType, int TopPositionFlag = TRUE ) ;								// サウンドハンドルを再生する
extern	int			StopSoundMem(                                                                        int SoundHandle ) ;						// サウンドハンドルの再生を停止する
extern	int			CheckSoundMem(                                                                       int SoundHandle ) ;						// サウンドハンドルが再生中かどうかを取得する
extern	int			SetPanSoundMem(                      int PanPal,                                     int SoundHandle ) ;						// サウンドハンドルのパンを設定する( 100分の1デシベル単位 0 〜 10000 )
extern	int			ChangePanSoundMem(                   int PanPal,                                     int SoundHandle ) ;						// サウンドハンドルのパンを設定する( -255 〜 255 )
extern	int			GetPanSoundMem(                                                                      int SoundHandle ) ;						// サウンドハンドルのパンを取得する
extern	int			SetVolumeSoundMem(                   int VolumePal,                                  int SoundHandle ) ;						// サウンドハンドルのボリュームを設定する( 100分の1デシベル単位 0 〜 10000 )
extern	int			ChangeVolumeSoundMem(                int VolumePal,                                  int SoundHandle ) ;						// サウンドハンドルのボリュームを設定する( 0 〜 255 )
extern	int			GetVolumeSoundMem(                                                                   int SoundHandle ) ;						// サウンドハンドルのボリュームを取得する( 100分の1デシベル単位 0 〜 10000 )
extern	int			GetVolumeSoundMem2(                                                                  int SoundHandle ) ;						// サウンドハンドルのボリュームを取得する( 0 〜 255 )
extern	int			SetChannelVolumeSoundMem(            int Channel, int VolumePal,                     int SoundHandle ) ;						// サウンドハンドルの指定のチャンネルのボリュームを設定する( 100分の1デシベル単位 0 〜 10000 )
extern	int			ChangeChannelVolumeSoundMem(         int Channel, int VolumePal,                     int SoundHandle ) ;						// サウンドハンドルの指定のチャンネルのボリュームを設定する( 0 〜 255 )
extern	int			GetChannelVolumeSoundMem(            int Channel,                                    int SoundHandle ) ;						// サウンドハンドルの指定のチャンネルのボリュームを取得する( 100分の1デシベル単位 0 〜 10000 )
extern	int			GetChannelVolumeSoundMem2(           int Channel,                                    int SoundHandle ) ;						// サウンドハンドルの指定のチャンネルのボリュームを取得する( 0 〜 255 )
extern	int			SetFrequencySoundMem(                int FrequencyPal,                               int SoundHandle ) ;						// サウンドハンドルの再生周波数を設定する
extern	int			GetFrequencySoundMem(                                                                int SoundHandle ) ;						// サウンドハンドルの再生周波数を取得する
extern	int			ResetFrequencySoundMem(                                                              int SoundHandle ) ;						// サウンドハンドルの再生周波数を読み込み直後の状態に戻す

extern	int			SetNextPlayPanSoundMem(              int PanPal,                                     int SoundHandle ) ;						// サウンドハンドルの次の再生にのみ使用するパンを設定する( 100分の1デシベル単位 0 〜 10000 )
extern	int			ChangeNextPlayPanSoundMem(           int PanPal,                                     int SoundHandle ) ;						// サウンドハンドルの次の再生にのみ使用するパンを設定する( -255 〜 255 )
extern	int			SetNextPlayVolumeSoundMem(           int VolumePal,                                  int SoundHandle ) ;						// サウンドハンドルの次の再生にのみ使用するボリュームを設定する( 100分の1デシベル単位 0 〜 10000 )
extern	int			ChangeNextPlayVolumeSoundMem(        int VolumePal,                                  int SoundHandle ) ;						// サウンドハンドルの次の再生にのみ使用するボリュームを設定する( 0 〜 255 )
extern	int			SetNextPlayChannelVolumeSoundMem(    int Channel, int VolumePal,                     int SoundHandle ) ;						// サウンドハンドルの次の再生にのみ使用するチャンネルのボリュームを設定する( 100分の1デシベル単位 0 〜 10000 )
extern	int			ChangeNextPlayChannelVolumeSoundMem( int Channel, int VolumePal,                     int SoundHandle ) ;						// サウンドハンドルの次の再生にのみ使用するチャンネルのボリュームを設定する( 0 〜 255 )
extern	int			SetNextPlayFrequencySoundMem(        int FrequencyPal,                               int SoundHandle ) ;						// サウンドハンドルの次の再生にのみ使用する再生周波数を設定する

extern	int			SetCurrentPositionSoundMem(          int SamplePosition,                             int SoundHandle ) ;						// サウンドハンドルの再生位置をサンプル単位で設定する(再生が止まっている時のみ有効)
extern	int			GetCurrentPositionSoundMem(                                                          int SoundHandle ) ;						// サウンドハンドルの再生位置をサンプル単位で取得する
extern	int			SetSoundCurrentPosition(             int Byte,                                       int SoundHandle ) ;						// サウンドハンドルの再生位置をバイト単位で設定する(再生が止まっている時のみ有効)
extern	int			GetSoundCurrentPosition(                                                             int SoundHandle ) ;						// サウンドハンドルの再生位置をバイト単位で取得する
extern	int			SetSoundCurrentTime(                 int Time,                                       int SoundHandle ) ;						// サウンドハンドルの再生位置をミリ秒単位で設定する(圧縮形式の場合は正しく設定されない場合がある)
extern	int			GetSoundCurrentTime(                                                                 int SoundHandle ) ;						// サウンドハンドルの再生位置をミリ秒単位で取得する(圧縮形式の場合は正しい値が返ってこない場合がある)
extern	int			GetSoundTotalSample(                                                                 int SoundHandle ) ;						// サウンドハンドルの音の総時間をサンプル単位で取得する
extern	int			GetSoundTotalTime(                                                                   int SoundHandle ) ;						// サウンドハンドルの音の総時間をミリ秒単位で取得する

extern	int			SetLoopPosSoundMem(                  int LoopTime,                                   int SoundHandle ) ;						// SetLoopTimePosSoundMem の別名関数
extern	int			SetLoopTimePosSoundMem(              int LoopTime,                                   int SoundHandle ) ;						// サウンドハンドルにループ位置を設定する(ミリ秒単位)
extern	int			SetLoopSamplePosSoundMem(            int LoopSamplePosition,                         int SoundHandle ) ;						// サウンドハンドルにループ位置を設定する(サンプル単位)

extern	int			SetLoopStartTimePosSoundMem(         int LoopStartTime,                              int SoundHandle ) ;						// サウンドハンドルにループ開始位置を設定する(ミリ秒単位)
extern	int			SetLoopStartSamplePosSoundMem(       int LoopStartSamplePosition,                    int SoundHandle ) ;						// サウンドハンドルにループ開始位置を設定する(サンプル単位)

extern	int			SetLoopAreaTimePosSoundMem(          int  LoopStartTime, int  LoopEndTime,                     int SoundHandle ) ;				// サウンドハンドルにループ範囲を設定する(ミリ秒単位)
extern	int			GetLoopAreaTimePosSoundMem(          int *LoopStartTime, int *LoopEndTime,                     int SoundHandle ) ;				// サウンドハンドルにループ範囲を取得する(ミリ秒単位)
extern	int			SetLoopAreaSamplePosSoundMem(        int  LoopStartSamplePosition, int  LoopEndSamplePosition, int SoundHandle ) ;				// サウンドハンドルにループ範囲を設定する(サンプル単位)
extern	int			GetLoopAreaSamplePosSoundMem(        int *LoopStartSamplePosition, int *LoopEndSamplePosition, int SoundHandle ) ;				// サウンドハンドルにループ範囲を取得する(サンプル単位)

extern	int			SetPlayFinishDeleteSoundMem(         int DeleteFlag,                                 int SoundHandle ) ;						// サウンドハンドルの再生が終了したら自動的にハンドルを削除するかどうかを設定する

extern	int			Set3DReverbParamSoundMem(            const SOUND3D_REVERB_PARAM *Param,              int SoundHandle ) ;						// サウンドハンドルの３Ｄサウンド用のリバーブパラメータを設定する
extern	int			Set3DPresetReverbParamSoundMem(      int PresetNo /* DX_REVERB_PRESET_DEFAULT 等 */ , int SoundHandle ) ;						// サウンドハンドルの３Ｄサウンド用のリバーブパラメータをプリセットを使用して設定する
extern	int			Set3DReverbParamSoundMemAll(         const SOUND3D_REVERB_PARAM *Param, int PlaySoundOnly = FALSE ) ;							// 全ての３Ｄサウンドのサウンドハンドルにリバーブパラメータを設定する( PlaySoundOnly TRUE:再生中のサウンドにのみ設定する  FALSE:再生していないサウンドにも設定する )
extern	int			Set3DPresetReverbParamSoundMemAll(   int PresetNo /* DX_REVERB_PRESET_DEFAULT 等 */ , int PlaySoundOnly = FALSE  ) ;			// 全ての３Ｄサウンドのサウンドハンドルにリバーブパラメータをプリセットを使用して設定する( PlaySoundOnly TRUE:再生中のサウンドにのみ設定する  FALSE:再生していないサウンドにも設定する )
extern	int			Get3DReverbParamSoundMem(            SOUND3D_REVERB_PARAM *ParamBuffer,              int SoundHandle ) ;						// サウンドハンドルに設定されている３Ｄサウンド用のリバーブパラメータを取得する
extern	int			Get3DPresetReverbParamSoundMem(      SOUND3D_REVERB_PARAM *ParamBuffer, int PresetNo /* DX_REVERB_PRESET_DEFAULT 等 */ ) ;		// プリセットの３Ｄサウンド用のリバーブパラメータを取得する

extern	int			Set3DPositionSoundMem(               VECTOR Position,                                int SoundHandle ) ;						// サウンドハンドルの３Ｄサウンド用の再生位置を設定する
extern	int			Set3DRadiusSoundMem(                 float Radius,                                   int SoundHandle ) ;						// サウンドハンドルの３Ｄサウンド用の音が聞こえる距離を設定する
extern	int			Set3DVelocitySoundMem(               VECTOR Velocity,                                int SoundHandle ) ;						// サウンドハンドルの３Ｄサウンド用の移動速度を設定する

extern	int			SetNextPlay3DPositionSoundMem(       VECTOR Position,                                int SoundHandle ) ;						// サウンドハンドルの次の再生のみに使用する３Ｄサウンド用の再生位置を設定する
extern	int			SetNextPlay3DRadiusSoundMem(         float Radius,                                   int SoundHandle ) ;						// サウンドハンドルの次の再生のみに使用する３Ｄサウンド用の音が聞こえる距離を設定する
extern	int			SetNextPlay3DVelocitySoundMem(       VECTOR Velocity,                                int SoundHandle ) ;						// サウンドハンドルの次の再生のみに使用する３Ｄサウンド用の移動速度を設定する


// 特殊関数
extern	int			GetMP3TagInfo(           const TCHAR *FileName,                        TCHAR *TitleBuffer, size_t TitleBufferBytes, TCHAR *ArtistBuffer, size_t ArtistBufferBytes, TCHAR *AlbumBuffer, size_t AlbumBufferBytes, TCHAR *YearBuffer, size_t YearBufferBytes, TCHAR *CommentBuffer, size_t CommentBufferBytes, TCHAR *TrackBuffer, size_t TrackBufferBytes, TCHAR *GenreBuffer, size_t GenreBufferBytes, int *PictureGrHandle ) ;		// MP3ファイルのタグ情報を取得する
extern	int			GetMP3TagInfoWithStrLen( const TCHAR *FileName, size_t FileNameLength, TCHAR *TitleBuffer, size_t TitleBufferBytes, TCHAR *ArtistBuffer, size_t ArtistBufferBytes, TCHAR *AlbumBuffer, size_t AlbumBufferBytes, TCHAR *YearBuffer, size_t YearBufferBytes, TCHAR *CommentBuffer, size_t CommentBufferBytes, TCHAR *TrackBuffer, size_t TrackBufferBytes, TCHAR *GenreBuffer, size_t GenreBufferBytes, int *PictureGrHandle ) ;		// MP3ファイルのタグ情報を取得する
						

// 設定関係関数
extern	int			SetCreateSoundDataType(              int SoundDataType ) ;																		// 作成するサウンドハンドルの再生タイプを設定する( DX_SOUNDDATATYPE_MEMNOPRESS 等 )
extern	int			GetCreateSoundDataType(              void ) ;																					// 作成するサウンドハンドルの再生タイプを取得する( DX_SOUNDDATATYPE_MEMNOPRESS 等 )
extern	int			SetCreateSoundPitchRate(             float Cents ) ;																			// 作成するサウンドハンドルのピッチ( 音の長さを変えずに音程を変更する )レートを設定する( 単位はセント( 100.0fで半音、1200.0fで１オクターヴ )、プラスの値で音程が高く、マイナスの値で音程が低くなります )
extern	float		GetCreateSoundPitchRate(             void ) ;																					// 作成するサウンドハンドルのピッチ( 音の長さを変えずに音程を変更する )レートを取得する( 単位はセント( 100.0fで半音、1200.0fで１オクターヴ )、プラスの値で音程が高く、マイナスの値で音程が低くなります )
extern	int			SetCreateSoundTimeStretchRate(       float Rate ) ;																				// 作成するサウンドハンドルのタイムストレッチ( 音程を変えずに音の長さを変更する )レートを設定する( 単位は倍率、2.0f で音の長さが２倍に、0.5f で音の長さが半分になります )
extern	float		GetCreateSoundTimeStretchRate(       void ) ;																					// 作成するサウンドハンドルのタイムストレッチ( 音程を変えずに音の長さを変更する )レートを取得する( 単位は倍率、2.0f で音の長さが２倍に、0.5f で音の長さが半分になります )
extern	int			SetCreateSoundLoopAreaTimePos(       int  LoopStartTime,           int  LoopEndTime ) ;											// 作成するサウンドハンドルのループ範囲を設定する( ミリ秒単位 )
extern	int			GetCreateSoundLoopAreaTimePos(       int *LoopStartTime,           int *LoopEndTime ) ;											// 作成するサウンドハンドルのループ範囲を取得する( ミリ秒単位 )
extern	int			SetCreateSoundLoopAreaSamplePos(     int  LoopStartSamplePosition, int  LoopEndSamplePosition ) ;								// 作成するサウンドハンドルのループ範囲を設定する( サンプル単位 )
extern	int			GetCreateSoundLoopAreaSamplePos(     int *LoopStartSamplePosition, int *LoopEndSamplePosition ) ;								// 作成するサウンドハンドルのループ範囲を取得する( サンプル単位 )
extern	int			SetCreateSoundIgnoreLoopAreaInfo(    int IgnoreFlag ) ;																			// LoadSoundMem などで読み込むサウンドデータにループ範囲情報があっても無視するかどうかを設定する( TRUE:無視する  FALSE:無視しない( デフォルト ) )
extern	int			GetCreateSoundIgnoreLoopAreaInfo(    void ) ;																					// LoadSoundMem などで読み込むサウンドデータにループ範囲情報があっても無視するかどうかを取得する( TRUE:無視する  FALSE:無視しない( デフォルト ) )
extern	int			SetDisableReadSoundFunctionMask(     int Mask ) ;																				// 使用しないサウンドデータ読み込み処理のマスクを設定する( DX_READSOUNDFUNCTION_PCM 等 )
extern	int			GetDisableReadSoundFunctionMask(     void ) ;																					// 使用しないサウンドデータ読み込み処理のマスクを取得する( DX_READSOUNDFUNCTION_PCM 等 )
extern	int			SetEnableSoundCaptureFlag(           int Flag ) ;																				// サウンドキャプチャを前提とした動作をするかどうかを設定する
extern	int			SetUseOldVolumeCalcFlag(             int Flag ) ;																				// ChangeVolumeSoundMem, ChangeNextPlayVolumeSoundMem, ChangeMovieVolumeToGraph の音量計算式を Ver3.10c以前のものを使用するかどうかを設定する( TRUE:Ver3.10c以前の計算式を使用  FALSE:3.10d以降の計算式を使用( デフォルト ) )

extern	int			SetCreate3DSoundFlag(                     int Flag ) ;																			// 次に作成するサウンドハンドルを３Ｄサウンド用にするかどうかを設定する( TRUE:３Ｄサウンド用にする  FALSE:３Ｄサウンド用にしない( デフォルト ) )
extern	int			Set3DSoundOneMetre(                       float Distance ) ;																	// ３Ｄ空間の１メートルに相当する距離を設定する、DxLib_Init を呼び出す前でのみ呼び出し可能( デフォルト:1.0f )
extern	int			Set3DSoundListenerPosAndFrontPos_UpVecY(  VECTOR Position, VECTOR FrontPosition ) ;												// ３Ｄサウンドのリスナーの位置とリスナーの前方位置を設定する( リスナーの上方向はＹ軸固定 )
extern	int			Set3DSoundListenerPosAndFrontPosAndUpVec( VECTOR Position, VECTOR FrontPosition, VECTOR UpVector ) ;							// ３Ｄサウンドのリスナーの位置とリスナーの前方位置とリスナーの上方向を設定する
extern	int			Set3DSoundListenerVelocity(               VECTOR Velocity ) ;																	// ３Ｄサウンドのリスナーの移動速度を設定する
extern	int			Set3DSoundListenerConeAngle(              float InnerAngle, float OuterAngle ) ;												// ３Ｄサウンドのリスナーの可聴角度範囲を設定する
extern	int			Set3DSoundListenerConeVolume(             float InnerAngleVolume, float OuterAngleVolume ) ;									// ３Ｄサウンドのリスナーの可聴角度範囲の音量倍率を設定する

#ifndef DX_NON_BEEP
// BEEP音再生用命令
extern	int			SetBeepFrequency(					int Freq ) ;																				// ビープ音周波数設定関数
extern	int			PlayBeep(							void ) ;																					// ビープ音を再生する
extern	int			StopBeep(							void ) ;																					// ビープ音を止める
#endif // DX_NON_BEEP

// ラッパー関数
extern	int			PlaySoundFile(						const TCHAR *FileName,                        int PlayType ) ;								// サウンドファイルを再生する
extern	int			PlaySoundFileWithStrLen(			const TCHAR *FileName, size_t FileNameLength, int PlayType ) ;								// サウンドファイルを再生する
extern	int			PlaySound(							const TCHAR *FileName,                        int PlayType ) ;								// PlaySoundFile の旧名称
extern	int			PlaySoundWithStrLen(				const TCHAR *FileName, size_t FileNameLength, int PlayType ) ;								// PlaySoundFile の旧名称
extern	int			CheckSoundFile(						void ) ;																					// サウンドファイルの再生中かどうかを取得する
extern	int			CheckSound(							void ) ;																					// CheckSoundFile の旧名称
extern	int			StopSoundFile(						void ) ;																					// サウンドファイルの再生を停止する
extern	int			StopSound(							void ) ;																					// StopSoundFile の旧名称
extern	int			SetVolumeSoundFile(					int VolumePal ) ;																			// サウンドファイルの音量を設定する
extern	int			SetVolumeSound(						int VolumePal ) ;																			// SetVolumeSound の旧名称

// ソフトウエア制御サウンド系関数
extern	int			InitSoftSound(						void ) ;																					// ソフトウエアで扱う波形データハンドルをすべて削除する
extern	int			LoadSoftSound(						const TCHAR *FileName                        ) ;											// ソフトウエアで扱う波形データハンドルをサウンドファイルから作成する
extern	int			LoadSoftSoundWithStrLen(			const TCHAR *FileName, size_t FileNameLength ) ;											// ソフトウエアで扱う波形データハンドルをサウンドファイルから作成する
extern	int			LoadSoftSoundFromMemImage(			const void *FileImage, int FileImageSize ) ;												// ソフトウエアで扱う波形データハンドルをメモリ上に展開されたサウンドファイルイメージから作成する
extern	int			MakeSoftSound(						int UseFormat_SoftSoundHandle, int SampleNum ) ;											// ソフトウエアで扱う空の波形データハンドルを作成する( フォーマットは引数のソフトウエアサウンドハンドルと同じものにする )
extern	int			MakeSoftSound2Ch16Bit44KHz(			int SampleNum ) ;																			// ソフトウエアで扱う空の波形データハンドルを作成する( チャンネル数:2 量子化ビット数:16bit サンプリング周波数:44.1KHz )
extern	int			MakeSoftSound2Ch16Bit22KHz(			int SampleNum ) ;																			// ソフトウエアで扱う空の波形データハンドルを作成する( チャンネル数:2 量子化ビット数:16bit サンプリング周波数:22KHz )
extern	int			MakeSoftSound2Ch8Bit44KHz(			int SampleNum ) ;																			// ソフトウエアで扱う空の波形データハンドルを作成する( チャンネル数:2 量子化ビット数: 8bit サンプリング周波数:44.1KHz )
extern	int			MakeSoftSound2Ch8Bit22KHz(			int SampleNum ) ;																			// ソフトウエアで扱う空の波形データハンドルを作成する( チャンネル数:2 量子化ビット数: 8bit サンプリング周波数:22KHz )
extern	int			MakeSoftSound1Ch16Bit44KHz(			int SampleNum ) ;																			// ソフトウエアで扱う空の波形データハンドルを作成する( チャンネル数:1 量子化ビット数:16bit サンプリング周波数:44.1KHz )
extern	int			MakeSoftSound1Ch16Bit22KHz(			int SampleNum ) ;																			// ソフトウエアで扱う空の波形データハンドルを作成する( チャンネル数:1 量子化ビット数:16bit サンプリング周波数:22KHz )
extern	int			MakeSoftSound1Ch8Bit44KHz(			int SampleNum ) ;																			// ソフトウエアで扱う空の波形データハンドルを作成する( チャンネル数:1 量子化ビット数: 8bit サンプリング周波数:44.1KHz )
extern	int			MakeSoftSound1Ch8Bit22KHz(			int SampleNum ) ;																			// ソフトウエアで扱う空の波形データハンドルを作成する( チャンネル数:1 量子化ビット数: 8bit サンプリング周波数:22KHz )
extern	int			MakeSoftSoundCustom(				int ChannelNum, int BitsPerSample, int SamplesPerSec, int SampleNum, int IsFloatType = 0 ) ;	// ソフトウエアで扱う空の波形データハンドルを作成する
extern	int			DeleteSoftSound(					int SoftSoundHandle ) ;																		// ソフトウエアで扱う波形データハンドルを削除する
#ifndef DX_NON_SAVEFUNCTION
extern	int			SaveSoftSound(						int SoftSoundHandle, const TCHAR *FileName                        ) ;						// ソフトウエアで扱う波形データハンドルをWAVEファイル(PCM)形式で保存する
extern	int			SaveSoftSoundWithStrLen(			int SoftSoundHandle, const TCHAR *FileName, size_t FileNameLength ) ;						// ソフトウエアで扱う波形データハンドルをWAVEファイル(PCM)形式で保存する
#endif // DX_NON_SAVEFUNCTION
extern	int			GetSoftSoundSampleNum(				int SoftSoundHandle ) ;																		// ソフトウエアで扱う波形データハンドルのサンプル数を取得する
extern	int			GetSoftSoundFormat(					int SoftSoundHandle, int *Channels, int *BitsPerSample, int *SamplesPerSec, int *IsFloatType = NULL ) ;				// ソフトウエアで扱う波形データハンドルのフォーマットを取得する
extern	int			ReadSoftSoundData(					int SoftSoundHandle, int SamplePosition, int *Channel1, int *Channel2 ) ;					// ソフトウエアで扱う波形データハンドルのサンプルを読み取る
extern	int			ReadSoftSoundDataF(					int SoftSoundHandle, int SamplePosition, float *Channel1, float *Channel2 ) ;				// ソフトウエアで扱う波形データハンドルのサンプルを読み取る( float型版 )
extern	int			WriteSoftSoundData(					int SoftSoundHandle, int SamplePosition, int Channel1, int Channel2 ) ;						// ソフトウエアで扱う波形データハンドルのサンプルを書き込む
extern	int			WriteSoftSoundDataF(				int SoftSoundHandle, int SamplePosition, float Channel1, float Channel2 ) ;					// ソフトウエアで扱う波形データハンドルのサンプルを書き込む( float型版 )
extern	int			WriteTimeStretchSoftSoundData(		int SrcSoftSoundHandle, int DestSoftSoundHandle ) ;											// ソフトウエアで扱う波形データハンドルの波形データを音程を変えずにデータの長さを変更する
extern	int			WritePitchShiftSoftSoundData(		int SrcSoftSoundHandle, int DestSoftSoundHandle ) ;											// ソフトウエアで扱う波形データハンドルの波形データの長さを変更する
extern	void*		GetSoftSoundDataImage(				int SoftSoundHandle ) ;																		// ソフトウエアで扱う波形データハンドルの波形イメージが格納されているメモリアドレスを取得する
extern	int			GetFFTVibrationSoftSound(			int SoftSoundHandle, int Channel, int SamplePosition, int SampleNum, float *Buffer_Array, int BufferLength ) ;								// ソフトウエアで扱う波形データハンドルの指定の範囲を高速フーリエ変換を行い、各周波数域の振幅を取得する( SampleNum は 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536 の何れかである必要があります、Channel を -1 にすると二つのチャンネルを合成した結果になります )
extern	int			GetFFTVibrationSoftSoundBase(		int SoftSoundHandle, int Channel, int SamplePosition, int SampleNum, float *RealBuffer_Array, float *ImagBuffer_Array, int BufferLength ) ;	// ソフトウエアで扱う波形データハンドルの指定の範囲を高速フーリエ変換を行い、各周波数域の振幅を取得する、結果の実数と虚数を別々に取得することができるバージョン( SampleNum は 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536 の何れかである必要があります、Channel を -1 にすると二つのチャンネルを合成した結果になります )

extern	int			InitSoftSoundPlayer(				void ) ;																					// ソフトウエアで扱う波形データのプレイヤーハンドルをすべて解放する
extern	int			MakeSoftSoundPlayer(				int UseFormat_SoftSoundHandle ) ;															// ソフトウエアで扱う波形データのプレイヤーハンドルを作成する( フォーマットは引数のソフトウエアサウンドハンドルと同じものにする )
extern	int			MakeSoftSoundPlayer2Ch16Bit44KHz(	void ) ;																					// ソフトウエアで扱う波形データのプレイヤーハンドルを作成する( チャンネル数:2 量子化ビット数:16bit サンプリング周波数:44.1KHz )
extern	int			MakeSoftSoundPlayer2Ch16Bit22KHz(	void ) ;																					// ソフトウエアで扱う波形データのプレイヤーハンドルを作成する( チャンネル数:2 量子化ビット数:16bit サンプリング周波数:22KHz )
extern	int			MakeSoftSoundPlayer2Ch8Bit44KHz(	void ) ;																					// ソフトウエアで扱う波形データのプレイヤーハンドルを作成する( チャンネル数:2 量子化ビット数: 8bit サンプリング周波数:44.1KHz )
extern	int			MakeSoftSoundPlayer2Ch8Bit22KHz(	void ) ;																					// ソフトウエアで扱う波形データのプレイヤーハンドルを作成する( チャンネル数:2 量子化ビット数: 8bit サンプリング周波数:22KHz )
extern	int			MakeSoftSoundPlayer1Ch16Bit44KHz(	void ) ;																					// ソフトウエアで扱う波形データのプレイヤーハンドルを作成する( チャンネル数:1 量子化ビット数:16bit サンプリング周波数:44.1KHz )
extern	int			MakeSoftSoundPlayer1Ch16Bit22KHz(	void ) ;																					// ソフトウエアで扱う波形データのプレイヤーハンドルを作成する( チャンネル数:1 量子化ビット数:16bit サンプリング周波数:22KHz )
extern	int			MakeSoftSoundPlayer1Ch8Bit44KHz(	void ) ;																					// ソフトウエアで扱う波形データのプレイヤーハンドルを作成する( チャンネル数:1 量子化ビット数: 8bit サンプリング周波数:44.1KHz )
extern	int			MakeSoftSoundPlayer1Ch8Bit22KHz(	void ) ;																					// ソフトウエアで扱う波形データのプレイヤーハンドルを作成する( チャンネル数:1 量子化ビット数: 8bit サンプリング周波数:22KHz )
extern	int			MakeSoftSoundPlayerCustom(			int ChannelNum, int BitsPerSample, int SamplesPerSec ) ;									// ソフトウエアで扱う波形データのプレイヤーハンドルを作成する
extern	int			DeleteSoftSoundPlayer(				int SSoundPlayerHandle ) ;																	// ソフトウエアで扱う波形データのプレイヤーハンドルを削除する
extern	int			AddDataSoftSoundPlayer(				int SSoundPlayerHandle, int SoftSoundHandle, int AddSamplePosition, int AddSampleNum ) ;	// ソフトウエアで扱う波形データのプレイヤーハンドルに波形データを追加する( フォーマットが同じではない場合はエラー )
extern	int			AddDirectDataSoftSoundPlayer(		int SSoundPlayerHandle, const void *SoundData, int AddSampleNum ) ;							// ソフトウエアで扱う波形データのプレイヤーハンドルにプレイヤーが対応したフォーマットの生波形データを追加する
extern	int			AddOneDataSoftSoundPlayer(			int SSoundPlayerHandle, int Channel1, int Channel2 ) ;										// ソフトウエアで扱う波形データのプレイヤーハンドルに波形データを一つ追加する
extern	int			GetSoftSoundPlayerFormat(			int SSoundPlayerHandle, int *Channels, int *BitsPerSample, int *SamplesPerSec ) ;			// ソフトウエアで扱う波形データのプレイヤーハンドルが扱うデータフォーマットを取得する
extern	int			StartSoftSoundPlayer(				int SSoundPlayerHandle ) ;																	// ソフトウエアで扱う波形データのプレイヤーハンドルの再生処理を開始する
extern	int			CheckStartSoftSoundPlayer(			int SSoundPlayerHandle ) ;																	// ソフトウエアで扱う波形データのプレイヤーハンドルの再生処理が開始されているか取得する( TRUE:開始している  FALSE:停止している )
extern	int			StopSoftSoundPlayer(				int SSoundPlayerHandle ) ;																	// ソフトウエアで扱う波形データのプレイヤーハンドルの再生処理を停止する
extern	int			ResetSoftSoundPlayer(				int SSoundPlayerHandle ) ;																	// ソフトウエアで扱う波形データのプレイヤーハンドルの状態を初期状態に戻す( 追加された波形データは削除され、再生状態だった場合は停止する )
extern	int			GetStockDataLengthSoftSoundPlayer(	int SSoundPlayerHandle ) ;																	// ソフトウエアで扱う波形データのプレイヤーハンドルに追加した波形データでまだ再生用サウンドバッファに転送されていない波形データのサンプル数を取得する
extern	int			CheckSoftSoundPlayerNoneData(		int SSoundPlayerHandle ) ;																	// ソフトウエアで扱う波形データのプレイヤーハンドルに再生用サウンドバッファに転送していない波形データが無く、再生用サウンドバッファにも無音データ以外無いかどうかを取得する( TRUE:無音データ以外無い  FALSE:有効データがある )




// ＭＩＤＩ制御関数
extern	int			DeleteMusicMem(						int MusicHandle ) ;																			// ＭＩＤＩハンドルを削除する
extern	int			LoadMusicMem(						const TCHAR *FileName                        ) ;											// ＭＩＤＩファイルを読み込みＭＩＤＩハンドルを作成する
extern	int			LoadMusicMemWithStrLen(				const TCHAR *FileName, size_t FileNameLength ) ;											// ＭＩＤＩファイルを読み込みＭＩＤＩハンドルを作成する
extern	int			LoadMusicMemByMemImage(				const void *FileImage, int FileImageSize ) ;												// メモリ上に展開されたＭＩＤＩファイルイメージからＭＩＤＩハンドルを作成する
extern	int			PlayMusicMem(						int MusicHandle, int PlayType ) ;															// ＭＩＤＩハンドルの演奏を開始する
extern	int			StopMusicMem(						int MusicHandle ) ;																			// ＭＩＤＩハンドルの演奏を停止する
extern	int			CheckMusicMem(						int MusicHandle ) ;																			// ＭＩＤＩハンドルが演奏中かどうかを取得する( TRUE:演奏中  FALSE:停止中 )
extern	int			SetVolumeMusicMem(					int Volume, int MusicHandle ) ;																// ＭＩＤＩハンドルの再生音量をセットする
extern	int			GetMusicMemPosition(				int MusicHandle ) ;																			// ＭＩＤＩハンドルの現在の再生位置を取得する
extern	int			InitMusicMem(						void ) ;																					// ＭＩＤＩハンドルをすべて削除する
extern	int			ProcessMusicMem(					void ) ;																					// ＭＩＤＩハンドルの周期的処理( 内部で呼ばれます )

extern	int			PlayMusic(							const TCHAR *FileName,                        int PlayType ) ;								// ＭＩＤＩファイルを演奏する
extern	int			PlayMusicWithStrLen(				const TCHAR *FileName, size_t FileNameLength, int PlayType ) ;								// ＭＩＤＩファイルを演奏する
extern	int			PlayMusicByMemImage(				const void *FileImage, int FileImageSize, int PlayType ) ;									// メモリ上に展開されているＭＩＤＩファイルを演奏する
extern	int			SetVolumeMusic(						int Volume ) ;																				// ＭＩＤＩの再生音量をセットする
extern	int			StopMusic(							void ) ;																					// ＭＩＤＩファイルの演奏停止
extern	int			CheckMusic(							void ) ;																					// ＭＩＤＩファイルが演奏中か否か情報を取得する
extern	int			GetMusicPosition(					void ) ;																					// ＭＩＤＩの現在の再生位置を取得する

extern	int			SelectMidiMode(						int Mode ) ;																				// ＭＩＤＩの再生形式を設定する

#endif // DX_NON_SOUND










// DxArchive_.cpp 関数 プロトタイプ宣言
extern	int			SetUseDXArchiveFlag(				int Flag ) ;															// ＤＸアーカイブファイルの読み込み機能を使うかどうかを設定する( FALSE:使用しない  TRUE:使用する )
extern	int			SetDXArchivePriority(				int Priority = 0 ) ;													// 同名のＤＸアーカイブファイルとフォルダが存在した場合、どちらを優先させるかを設定する( 1:フォルダを優先　 0:ＤＸアーカイブファイルを優先( デフォルト ) )
extern	int			SetDXArchiveExtension(				const TCHAR *Extension = NULL ) ;										// 検索するＤＸアーカイブファイルの拡張子を設定する( Extension:拡張子名文字列 )
extern	int			SetDXArchiveExtensionWithStrLen(	const TCHAR *Extension = NULL , size_t ExtensionLength = 0 ) ;			// 検索するＤＸアーカイブファイルの拡張子を設定する( Extension:拡張子名文字列 )
extern	int			SetDXArchiveKeyString(				const TCHAR *KeyString = NULL ) ;										// ＤＸアーカイブファイルの鍵文字列を設定する( KeyString:鍵文字列 )
extern	int			SetDXArchiveKeyStringWithStrLen(	const TCHAR *KeyString = NULL , size_t KeyStringLength = 0 ) ;			// ＤＸアーカイブファイルの鍵文字列を設定する( KeyString:鍵文字列 )

extern	int			DXArchivePreLoad(					const TCHAR *FilePath,                        int ASync = FALSE ) ;		// 指定のＤＸＡファイルを丸ごとメモリに読み込む( 戻り値  -1:エラー  0:成功 )
extern	int			DXArchivePreLoadWithStrLen(			const TCHAR *FilePath, size_t FilePathLength, int ASync = FALSE ) ;		// 指定のＤＸＡファイルを丸ごとメモリに読み込む( 戻り値  -1:エラー  0:成功 )
extern	int			DXArchiveCheckIdle(					const TCHAR *FilePath                        ) ;						// 指定のＤＸＡファイルの事前読み込みが完了したかどうかを取得する( 戻り値  TRUE:完了した FALSE:まだ )
extern	int			DXArchiveCheckIdleWithStrLen(		const TCHAR *FilePath, size_t FilePathLength ) ;						// 指定のＤＸＡファイルの事前読み込みが完了したかどうかを取得する( 戻り値  TRUE:完了した FALSE:まだ )
extern	int			DXArchiveRelease(					const TCHAR *FilePath                        ) ;						// 指定のＤＸＡファイルをメモリから解放する
extern	int			DXArchiveReleaseWithStrLen(			const TCHAR *FilePath, size_t FilePathLength ) ;						// 指定のＤＸＡファイルをメモリから解放する
extern	int			DXArchiveCheckFile(					const TCHAR *FilePath,                        const TCHAR *TargetFilePath                              ) ;	// ＤＸＡファイルの中に指定のファイルが存在するかどうかを調べる、TargetFilePath はＤＸＡファイルをカレントフォルダとした場合のパス( 戻り値:  -1=エラー  0:無い  1:ある )
extern	int			DXArchiveCheckFileWithStrLen(		const TCHAR *FilePath, size_t FilePathLength, const TCHAR *TargetFilePath, size_t TargetFilePathLength ) ;	// ＤＸＡファイルの中に指定のファイルが存在するかどうかを調べる、TargetFilePath はＤＸＡファイルをカレントフォルダとした場合のパス( 戻り値:  -1=エラー  0:無い  1:ある )
extern	int			DXArchiveSetMemImage(				void *ArchiveImage, int ArchiveImageSize, const TCHAR *EmulateFilePath,                               int ArchiveImageCopyFlag = FALSE , int ArchiveImageReadOnly = TRUE ) ;	// メモリ上に展開されたＤＸＡファイルを指定のファイルパスにあることにする( EmulateFilePath は見立てる dxa ファイルのパス、例えばＤＸＡファイルイメージを Image.dxa というファイル名で c:\Temp にあることにしたい場合は EmulateFilePath に "c:\\Temp\\Image.dxa" を渡す、SetDXArchiveExtension で拡張子を変更している場合は EmulateFilePath に渡すファイルパスの拡張子もそれに合わせる必要あり )
extern	int			DXArchiveSetMemImageWithStrLen(		void *ArchiveImage, int ArchiveImageSize, const TCHAR *EmulateFilePath, size_t EmulateFilePathLength, int ArchiveImageCopyFlag = FALSE , int ArchiveImageReadOnly = TRUE ) ;	// メモリ上に展開されたＤＸＡファイルを指定のファイルパスにあることにする( EmulateFilePath は見立てる dxa ファイルのパス、例えばＤＸＡファイルイメージを Image.dxa というファイル名で c:\Temp にあることにしたい場合は EmulateFilePath に "c:\\Temp\\Image.dxa" を渡す、SetDXArchiveExtension で拡張子を変更している場合は EmulateFilePath に渡すファイルパスの拡張子もそれに合わせる必要あり )
extern	int			DXArchiveReleaseMemImage(			void *ArchiveImage ) ;													// DXArchiveSetMemImage の設定を解除する

extern	DWORD		HashCRC32(							const void *SrcData, size_t SrcDataSize ) ;								// バイナリデータを元に CRC32 のハッシュ値を計算する





















// DxModel.cpp 関数 プロトタイプ宣言

#ifndef DX_NON_MODEL

// モデルの読み込み・複製関係
extern	int			MV1LoadModel(						const TCHAR *FileName ) ;											// モデルの読み込み( -1:エラー  0以上:モデルハンドル )
extern	int			MV1LoadModelWithStrLen(				const TCHAR *FileName, size_t FileNameLength ) ;					// モデルの読み込み( -1:エラー  0以上:モデルハンドル )
extern	int			MV1LoadModelFromMem(				const void *FileImage, int FileSize, int (* FileReadFunc )( const TCHAR *FilePath, void **FileImageAddr, int *FileSize, void *FileReadFuncData ), int (* FileReleaseFunc )( void *MemoryAddr, void *FileReadFuncData ), void *FileReadFuncData = NULL ) ;	// メモリ上のモデルファイルイメージと独自の読み込みルーチンを使用してモデルを読み込む
extern	int			MV1DuplicateModel(					int SrcMHandle ) ;													// 指定のモデルと同じモデル基本データを使用してモデルを作成する( -1:エラー  0以上:モデルハンドル )
extern	int			MV1CreateCloneModel(				int SrcMHandle ) ;													// 指定のモデルをモデル基本データも含め複製する( MV1DuplicateModel はモデル基本データは共有しますが、こちらは複製元のモデルとは一切共有データの無いモデルハンドルを作成します )( -1:エラー  0以上:モデルハンドル )

extern	int			MV1DeleteModel(						int MHandle ) ;														// モデルを削除する
extern	int			MV1InitModel(						void ) ;															// すべてのモデルを削除する

extern	int			MV1SetLoadModelReMakeNormal(						int Flag ) ;														// モデルを読み込む際に法線の再計算を行うかどうかを設定する( TRUE:行う  FALSE:行わない( デフォルト ) )
extern	int			MV1SetLoadModelReMakeNormalSmoothingAngle(			float SmoothingAngle = 1.562069f ) ;								// モデルを読み込む際に行う法泉の再計算で使用するスムージング角度を設定する( 単位はラジアン )
extern	int			MV1SetLoadModelIgnoreScaling(						int Flag ) ;														// モデルを読み込む際にスケーリングデータを無視するかどうかを設定する( TRUE:無視する  FALSE:無視しない( デフォルト ) )
extern	int			MV1SetLoadModelPositionOptimize(					int Flag ) ;														// モデルを読み込む際に座標データの最適化を行うかどうかを設定する( TRUE:行う  FALSE:行わない( デフォルト ) )
extern	int			MV1SetLoadModelNotEqNormalSide_AddZeroAreaPolygon(	int Flag ) ;														// モデルを読み込む際にポリゴンの辺が接していて、且つ法線の方向が異なる辺に面積０のポリゴンを埋め込むかどうかを設定する( TRUE:埋め込む　FALSE:埋め込まない( デフォルト ) )、( MV1ファイルの読み込みではこの関数の設定は無視され、ポリゴンの埋め込みは実行されません )
extern	int			MV1SetLoadModelUsePhysicsMode(						int PhysicsMode /* DX_LOADMODEL_PHYSICS_LOADCALC 等 */ ) ;			// 読み込むモデルの物理演算モードを設定する
extern	int			MV1SetLoadModelPhysicsWorldGravity(					float Gravity ) ;													// 読み込むモデルの物理演算に適用する重力パラメータを設定する
extern	float		MV1GetLoadModelPhysicsWorldGravity(					void ) ;															// 読み込むモデルの物理演算に適用する重力パラメータを取得する
extern	int			MV1SetLoadCalcPhysicsWorldGravity(					int GravityNo, VECTOR Gravity ) ;									// 読み込むモデルの物理演算モードが事前計算( DX_LOADMODEL_PHYSICS_LOADCALC )だった場合に適用される重力の設定をする
extern	VECTOR		MV1GetLoadCalcPhysicsWorldGravity(					int GravityNo ) ;													// 読み込むモデルの物理演算モードが事前計算( DX_LOADMODEL_PHYSICS_LOADCALC )だった場合に適用される重力を取得する
extern	int			MV1SetLoadModelPhysicsCalcPrecision(				int Precision ) ;													// 読み込むモデルの物理演算モードが事前計算( DX_LOADMODEL_PHYSICS_LOADCALC )だった場合に適用される物理演算の時間進行の精度を設定する( 0:60FPS  1:120FPS  2:240FPS  3:480FPS  4:960FPS  5:1920FPS )
extern	int			MV1SetLoadModel_PMD_PMX_AnimationFPSMode(			int FPSMode /* DX_LOADMODEL_PMD_PMX_ANIMATION_FPSMODE_30 等 */ ) ;	// PMD, PMX ファイルを読み込んだ際のアニメーションの FPS モードを設定する
extern	int			MV1AddLoadModelDisablePhysicsNameWord(				const TCHAR *NameWord ) ;											// 読み込むモデルの物理演算を特定の剛体のみ無効にするための名前のワードを追加する、追加できるワード文字列の最大長は 63 文字、追加できるワードの数は最大 256 個
extern	int			MV1AddLoadModelDisablePhysicsNameWordWithStrLen(	const TCHAR *NameWord, size_t NameWordLength ) ;					// 読み込むモデルの物理演算を特定の剛体のみ無効にするための名前のワードを追加する、追加できるワード文字列の最大長は 63 文字、追加できるワードの数は最大 256 個
extern	int			MV1ResetLoadModelDisablePhysicsNameWord(			void ) ;															// MV1AddLoadModelDisablePhysicsNameWord で追加した剛体の無効ワードをリセットして無効ワード無しの初期状態に戻す
extern	int			MV1SetLoadModelDisablePhysicsNameWordMode(			int DisableNameWordMode /* DX_LOADMODEL_PHYSICS_DISABLENAMEWORD_ALWAYS 等 */ ) ;	// MV1AddLoadModelDisablePhysicsNameWord で追加した剛体の無効ワードの適用ルールを変更する
extern	int			MV1SetLoadModelAnimFilePath(						const TCHAR *FileName ) ;											// 読み込むモデルに適用するアニメーションファイルのパスを設定する、NULLを渡すと設定リセット( 現在は PMD,PMX のみに効果あり )
extern	int			MV1SetLoadModelAnimFilePathWithStrLen(				const TCHAR *FileName, size_t FileNameLength ) ;					// 読み込むモデルに適用するアニメーションファイルのパスを設定する、NULLを渡すと設定リセット( 現在は PMD,PMX のみに効果あり )
extern	int			MV1SetLoadModelUsePackDraw(							int Flag ) ;														// 読み込むモデルを同時複数描画に対応させるかどうかを設定する( TRUE:対応させる  FALSE:対応させない( デフォルト ) )、( 「対応させる」にすると描画が高速になる可能性がある代わりに消費VRAMが増えます )
extern	int			MV1SetLoadModelTriangleListUseMaxBoneNum(			int UseMaxBoneNum ) ;												// 読み込むモデルのひとつのトライアングルリストで使用できる最大ボーン数を設定する( UseMaxBoneNum で指定できる値の範囲は 8 〜 54、 0 を指定するとデフォルト動作に戻る )

// モデル保存関係
extern	int			MV1SaveModelToMV1File(				int MHandle, const TCHAR *FileName,                        int SaveType = MV1_SAVETYPE_NORMAL , int AnimMHandle = -1 , int AnimNameCheck = TRUE , int Normal8BitFlag = 1 , int Position16BitFlag = 1 , int Weight8BitFlag = 0 , int Anim16BitFlag = 1 ) ;		// 指定のパスにモデルを保存する( 戻り値  0:成功  -1:メモリ不足  -2:使われていないアニメーションがあった )
extern	int			MV1SaveModelToMV1FileWithStrLen(	int MHandle, const TCHAR *FileName, size_t FileNameLength, int SaveType = MV1_SAVETYPE_NORMAL , int AnimMHandle = -1 , int AnimNameCheck = TRUE , int Normal8BitFlag = 1 , int Position16BitFlag = 1 , int Weight8BitFlag = 0 , int Anim16BitFlag = 1 ) ;		// 指定のパスにモデルを保存する( 戻り値  0:成功  -1:メモリ不足  -2:使われていないアニメーションがあった )
#ifndef DX_NON_SAVEFUNCTION
extern	int			MV1SaveModelToXFile(				int MHandle, const TCHAR *FileName,                        int SaveType = MV1_SAVETYPE_NORMAL , int AnimMHandle = -1 , int AnimNameCheck = TRUE ) ;	// 指定のパスにモデルをＸファイル形式で保存する( 戻り値  0:成功  -1:メモリ不足  -2:使われていないアニメーションがあった )
extern	int			MV1SaveModelToXFileWithStrLen(		int MHandle, const TCHAR *FileName, size_t FileNameLength, int SaveType = MV1_SAVETYPE_NORMAL , int AnimMHandle = -1 , int AnimNameCheck = TRUE ) ;	// 指定のパスにモデルをＸファイル形式で保存する( 戻り値  0:成功  -1:メモリ不足  -2:使われていないアニメーションがあった )
#endif // DX_NON_SAVEFUNCTION

// モデル描画関係
extern	int			MV1DrawModel(						int MHandle ) ;														// モデルを描画する
extern	int			MV1DrawFrame(						int MHandle, int FrameIndex ) ;										// モデルの指定のフレームを描画する
extern	int			MV1DrawMesh(						int MHandle, int MeshIndex ) ;										// モデルの指定のメッシュを描画する
extern	int			MV1DrawTriangleList(				int MHandle, int TriangleListIndex ) ;								// モデルの指定のトライアングルリストを描画する
extern	int			MV1DrawModelDebug(					int MHandle, unsigned int Color, int IsNormalLine, float NormalLineLength, int IsPolyLine, int IsCollisionBox ) ;	// モデルのデバッグ描画

// 描画設定関係
extern	int			MV1SetUseOrigShader(				int UseFlag ) ;														// モデルの描画に SetUseVertexShader, SetUsePixelShader で指定したシェーダーを使用するかどうかを設定する( TRUE:使用する  FALSE:使用しない( デフォルト ) )
extern	int			MV1SetSemiTransDrawMode(			int DrawMode /* DX_SEMITRANSDRAWMODE_ALWAYS 等 */ ) ;				// モデルの半透明要素がある部分についての描画モードを設定する

// モデル基本制御関係
extern	MATRIX		MV1GetLocalWorldMatrix(				int MHandle ) ;														// モデルのローカル座標からワールド座標に変換する行列を得る
extern	MATRIX_D	MV1GetLocalWorldMatrixD(			int MHandle ) ;														// モデルのローカル座標からワールド座標に変換する行列を得る
extern	int			MV1SetPosition(						int MHandle, VECTOR   Position ) ;									// モデルの座標をセット
extern	int			MV1SetPositionD(					int MHandle, VECTOR_D Position ) ;									// モデルの座標をセット
extern	VECTOR		MV1GetPosition(						int MHandle ) ;														// モデルの座標を取得
extern	VECTOR_D	MV1GetPositionD(					int MHandle ) ;														// モデルの座標を取得
extern	int			MV1SetScale(						int MHandle, VECTOR Scale ) ;										// モデルの拡大値をセット
extern	VECTOR		MV1GetScale(						int MHandle ) ;														// モデルの拡大値を取得
extern	int			MV1SetRotationXYZ(					int MHandle, VECTOR Rotate ) ;										// モデルの回転値をセット( X軸回転→Y軸回転→Z軸回転方式 )
extern	VECTOR		MV1GetRotationXYZ(					int MHandle ) ;														// モデルの回転値を取得( X軸回転→Y軸回転→Z軸回転方式 )
extern	int			MV1SetRotationZYAxis(				int MHandle, VECTOR ZAxisDirection, VECTOR YAxisDirection, float ZAxisTwistRotate ) ;	// モデルのＺ軸とＹ軸の向きをセットする
extern	int			MV1SetRotationYUseDir(				int MHandle, VECTOR Direction, float OffsetYAngle ) ;				// モデルのＹ軸の回転値を指定のベクトルの向きを元に設定する、モデルはZ軸のマイナス方向を向いていることを想定するので、そうではない場合は OffsetYAngle で補正する、Ｘ軸回転、Ｚ軸回転は０で固定
extern	int			MV1SetRotationMatrix(				int MHandle, MATRIX Matrix ) ;										// モデルの回転用行列をセットする
extern	MATRIX		MV1GetRotationMatrix(				int MHandle ) ;														// モデルの回転用行列を取得する
extern	int			MV1SetMatrix(						int MHandle, MATRIX   Matrix ) ;									// モデルの変形用行列をセットする
extern	int			MV1SetMatrixD(						int MHandle, MATRIX_D Matrix ) ;									// モデルの変形用行列をセットする
extern	MATRIX		MV1GetMatrix(						int MHandle ) ;														// モデルの変形用行列を取得する
extern	MATRIX_D	MV1GetMatrixD(						int MHandle ) ;														// モデルの変形用行列を取得する
extern	int			MV1SetVisible(						int MHandle, int VisibleFlag ) ;									// モデルの表示、非表示状態を変更する( TRUE:表示  FALSE:非表示 )
extern	int			MV1GetVisible(						int MHandle ) ;														// モデルの表示、非表示状態を取得する( TRUE:表示  FALSE:非表示 )
extern	int			MV1SetMeshCategoryVisible(			int MHandle, int MeshCategory, int VisibleFlag ) ;					// モデルのメッシュの種類( DX_MV1_MESHCATEGORY_NORMAL など )毎の表示、非表示を設定する( TRUE:表示  FALSE:非表示 )
extern	int			MV1GetMeshCategoryVisible(			int MHandle, int MeshCategory ) ;									// モデルのメッシュの種類( DX_MV1_MESHCATEGORY_NORMAL など )毎の表示、非表示を取得する( TRUE:表示  FALSE:非表示 )
extern	int			MV1SetDifColorScale(				int MHandle, COLOR_F Scale ) ;										// モデルのディフューズカラーのスケール値を設定する( デフォルト値は 1.0f )
extern	COLOR_F		MV1GetDifColorScale(				int MHandle ) ;														// モデルのディフューズカラーのスケール値を取得する( デフォルト値は 1.0f )
extern	int			MV1SetSpcColorScale(				int MHandle, COLOR_F Scale ) ;										// モデルのスペキュラカラーのスケール値を設定する( デフォルト値は 1.0f )
extern	COLOR_F		MV1GetSpcColorScale(				int MHandle ) ;														// モデルのスペキュラカラーのスケール値を取得する( デフォルト値は 1.0f )
extern	int			MV1SetEmiColorScale(				int MHandle, COLOR_F Scale ) ;										// モデルのエミッシブカラーのスケール値を設定する( デフォルト値は 1.0f )
extern	COLOR_F		MV1GetEmiColorScale(				int MHandle ) ;														// モデルのエミッシブカラーのスケール値を取得する( デフォルト値は 1.0f )
extern	int			MV1SetAmbColorScale(				int MHandle, COLOR_F Scale ) ;										// モデルのアンビエントカラーのスケール値を設定する( デフォルト値は 1.0f )
extern	COLOR_F		MV1GetAmbColorScale(				int MHandle ) ;														// モデルのアンビエントカラーのスケール値を取得する( デフォルト値は 1.0f )
extern	int			MV1GetSemiTransState(				int MHandle ) ;														// モデルに半透明要素があるかどうかを取得する( 戻り値 TRUE:ある  FALSE:ない )
extern	int			MV1SetOpacityRate(					int MHandle, float Rate ) ;											// モデルの不透明度を設定する( 不透明 1.0f 〜 透明 0.0f )
extern	float		MV1GetOpacityRate(					int MHandle ) ;														// モデルの不透明度を取得する( 不透明 1.0f 〜 透明 0.0f )
extern	int			MV1SetUseDrawMulAlphaColor(			int MHandle, int Flag ) ;											// モデルを描画する際にRGB値に対してA値を乗算するかどうかを設定する( 描画結果が乗算済みアルファ画像になります )( Flag   TRUE:RGB値に対してA値を乗算する  FALSE:乗算しない(デフォルト) )
extern	int			MV1GetUseDrawMulAlphaColor(			int MHandle ) ;														// モデルを描画する際にRGB値に対してA値を乗算するかどうかを取得する( 描画結果が乗算済みアルファ画像になります )( 戻り値 TRUE:RGB値に対してA値を乗算する  FALSE:乗算しない(デフォルト) )
extern	int			MV1SetUseZBuffer(					int MHandle, int Flag ) ;											// モデルを描画する際にＺバッファを使用するかどうかを設定する
extern	int			MV1SetWriteZBuffer(					int MHandle, int Flag ) ;											// モデルを描画する際にＺバッファに書き込みを行うかどうかを設定する
extern	int			MV1SetZBufferCmpType(				int MHandle, int CmpType /* DX_CMP_NEVER 等 */ ) ;					// モデルの描画時のＺ値の比較モードを設定する
extern	int			MV1SetZBias(						int MHandle, int Bias ) ;											// モデルの描画時の書き込むＺ値のバイアスを設定する
extern	int			MV1SetUseVertDifColor(				int MHandle, int UseFlag ) ;										// モデルの含まれるメッシュの頂点ディフューズカラーをマテリアルのディフューズカラーの代わりに使用するかどうかを設定する( TRUE:マテリアルカラーの代わりに使用する  FALSE:マテリアルカラーを使用する )
extern	int			MV1SetUseVertSpcColor(				int MHandle, int UseFlag ) ;										// モデルに含まれるメッシュの頂点スペキュラカラーをマテリアルのスペキュラカラーの代わりに使用するかどうかを設定する( TRUE:マテリアルカラーの代わりに使用する  FALSE:マテリアルカラーを使用する )
extern	int			MV1SetSampleFilterMode(				int MHandle, int FilterMode ) ;										// モデルのテクスチャのサンプルフィルターモードを変更する( FilterMode は DX_DRAWMODE_NEAREST 等 )
extern	int			MV1SetMaxAnisotropy(				int MHandle, int MaxAnisotropy ) ;									// モデルの異方性フィルタリングの最大次数を設定する
extern	int			MV1SetWireFrameDrawFlag(			int MHandle, int Flag ) ;											// モデルをワイヤーフレームで描画するかどうかを設定する
extern	int			MV1RefreshVertColorFromMaterial(	int MHandle ) ;														// モデルの頂点カラーを現在設定されているマテリアルのカラーにする
extern	int			MV1SetPhysicsWorldGravity(			int MHandle, VECTOR Gravity ) ;										// モデルの物理演算の重力を設定する
extern	int			MV1PhysicsCalculation(				int MHandle, float MillisecondTime ) ;								// モデルの物理演算を指定時間分経過したと仮定して計算する( MillisecondTime で指定する時間の単位はミリ秒 )
extern	int			MV1PhysicsResetState(				int MHandle ) ;														// モデルの物理演算の状態をリセットする( 位置がワープしたとき用 )
extern	int			MV1SetUseShapeFlag(					int MHandle, int UseFlag ) ;										// モデルのシェイプ機能を使用するかどうかを設定する( UseFlag  TRUE:使用する( デフォルト )  FALSE:使用しない )
extern	int			MV1GetMaterialNumberOrderFlag(		int MHandle ) ;														// モデルのマテリアル番号順にメッシュを描画するかどうかのフラグを取得する( TRUE:マテリアル番号順に描画  FALSE:不透明メッシュの後半透明メッシュ )

// アニメーション関係
extern	int			MV1AttachAnim(						int MHandle, int AnimIndex, int AnimSrcMHandle = -1 , int NameCheck = TRUE ) ;		// アニメーションをアタッチする( 戻り値  -1:エラー  0以上:アタッチインデックス )
extern	int			MV1DetachAnim(						int MHandle, int AttachIndex ) ;													// アニメーションをデタッチする
extern	int			MV1SetAttachAnimTime(				int MHandle, int AttachIndex, float Time ) ;										// アタッチしているアニメーションの再生時間を設定する
extern	float		MV1GetAttachAnimTime(				int MHandle, int AttachIndex ) ;													// アタッチしているアニメーションの再生時間を取得する
extern	float		MV1GetAttachAnimTotalTime(			int MHandle, int AttachIndex ) ;													// アタッチしているアニメーションの総時間を得る
extern	int			MV1SetAttachAnimBlendRate(			int MHandle, int AttachIndex, float Rate = 1.0f ) ;									// アタッチしているアニメーションのブレンド率を設定する
extern	float		MV1GetAttachAnimBlendRate(			int MHandle, int AttachIndex ) ;													// アタッチしているアニメーションのブレンド率を取得する
extern	int			MV1SetAttachAnimBlendRateToFrame(	int MHandle, int AttachIndex, int FrameIndex, float Rate, int SetChild = TRUE ) ;	// アタッチしているアニメーションのブレンド率を設定する( フレーム単位 )
extern	float		MV1GetAttachAnimBlendRateToFrame(	int MHandle, int AttachIndex, int FrameIndex ) ;									// アタッチしているアニメーションのブレンド率を設定する( フレーム単位 )
extern	int			MV1GetAttachAnim(					int MHandle, int AttachIndex ) ;													// アタッチしているアニメーションのアニメーションインデックスを取得する
extern	int			MV1SetAttachAnimUseShapeFlag(		int MHandle, int AttachIndex, int UseFlag ) ;										// アタッチしているアニメーションのシェイプを使用するかどうかを設定する( UseFlag  TRUE:使用する( デフォルト )  FALSE:使用しない )
extern	int			MV1GetAttachAnimUseShapeFlag(		int MHandle, int AttachIndex ) ;													// アタッチしているアニメーションのシェイプを使用するかどうかを取得する
extern	VECTOR		MV1GetAttachAnimFrameLocalPosition(	int MHandle, int AttachIndex, int FrameIndex ) ;									// アタッチしているアニメーションの指定のフレームの現在のローカル座標を取得する
extern	MATRIX		MV1GetAttachAnimFrameLocalMatrix(	int MHandle, int AttachIndex, int FrameIndex ) ;									// アタッチしているアニメーションの指定のフレームの現在のローカル変換行列を取得する

extern	int			MV1GetAnimNum(						int MHandle ) ;																		// アニメーションの数を取得する
extern	const TCHAR *MV1GetAnimName(					int MHandle, int AnimIndex ) ;														// 指定番号のアニメーション名を取得する( NULL:エラー )
extern	int			MV1SetAnimName(						int MHandle, int AnimIndex, const TCHAR *AnimName                        ) ;		// 指定番号のアニメーション名を変更する
extern	int			MV1SetAnimNameWithStrLen(			int MHandle, int AnimIndex, const TCHAR *AnimName, size_t AnimNameLength ) ;		// 指定番号のアニメーション名を変更する
extern	int			MV1GetAnimIndex(					int MHandle, const TCHAR *AnimName                        ) ;						// 指定名のアニメーション番号を取得する( -1:エラー )
extern	int			MV1GetAnimIndexWithStrLen(			int MHandle, const TCHAR *AnimName, size_t AnimNameLength ) ;						// 指定名のアニメーション番号を取得する( -1:エラー )
extern	float		MV1GetAnimTotalTime(				int MHandle, int AnimIndex ) ;														// 指定番号のアニメーションの総時間を得る
extern	int			MV1GetAnimTargetFrameNum(			int MHandle, int AnimIndex ) ;														// 指定のアニメーションがターゲットとするフレームの数を取得する
extern	const TCHAR *MV1GetAnimTargetFrameName(			int MHandle, int AnimIndex, int AnimFrameIndex ) ;									// 指定のアニメーションがターゲットとするフレームの名前を取得する
extern	int			MV1GetAnimTargetFrame(				int MHandle, int AnimIndex, int AnimFrameIndex ) ;									// 指定のアニメーションがターゲットとするフレームの番号を取得する
extern	int			MV1GetAnimTargetFrameKeySetNum(		int MHandle, int AnimIndex, int AnimFrameIndex ) ;									// 指定のアニメーションがターゲットとするフレーム用のアニメーションキーセットの数を取得する
extern	int			MV1GetAnimTargetFrameKeySet(		int MHandle, int AnimIndex, int AnimFrameIndex, int Index ) ;						// 指定のアニメーションがターゲットとするフレーム用のアニメーションキーセットキーセットインデックスを取得する

extern	int			MV1GetAnimKeySetNum(				int MHandle ) ;																		// モデルに含まれるアニメーションキーセットの総数を得る
extern	int			MV1GetAnimKeySetType(				int MHandle, int AnimKeySetIndex ) ;												// 指定のアニメーションキーセットのタイプを取得する( MV1_ANIMKEY_TYPE_QUATERNION 等 )
extern	int			MV1GetAnimKeySetDataType(			int MHandle, int AnimKeySetIndex ) ;												// 指定のアニメーションキーセットのデータタイプを取得する( MV1_ANIMKEY_DATATYPE_ROTATE 等 )
extern	int			MV1GetAnimKeySetTimeType(			int MHandle, int AnimKeySetIndex ) ;												// 指定のアニメーションキーセットのキーの時間データタイプを取得する( MV1_ANIMKEY_TIME_TYPE_ONE 等 )
extern	int			MV1GetAnimKeySetDataNum(			int MHandle, int AnimKeySetIndex ) ;												// 指定のアニメーションキーセットのキーの数を取得する
extern	float		MV1GetAnimKeyDataTime(				int MHandle, int AnimKeySetIndex, int Index ) ;										// 指定のアニメーションキーセットのキーの時間を取得する
extern	int			MV1GetAnimKeyDataIndexFromTime(		int MHandle, int AnimKeySetIndex, float Time ) ;									// 指定のアニメーションキーセットの指定の時間でのキーの番号を取得する
extern	FLOAT4		MV1GetAnimKeyDataToQuaternion(		int MHandle, int AnimKeySetIndex, int Index ) ;										// 指定のアニメーションキーセットのキーを取得する、キータイプが MV1_ANIMKEY_TYPE_QUATERNION では無かった場合は失敗する
extern	FLOAT4		MV1GetAnimKeyDataToQuaternionFromTime( int MHandle, int AnimKeySetIndex, float Time ) ;									// 指定のアニメーションキーセットのキーを取得する、キータイプが MV1_ANIMKEY_TYPE_QUATERNION では無かった場合は失敗する( 時間指定版 )
extern	VECTOR		MV1GetAnimKeyDataToVector(			int MHandle, int AnimKeySetIndex, int Index ) ;										// 指定のアニメーションキーセットのキーを取得する、キータイプが MV1_ANIMKEY_TYPE_VECTOR では無かった場合は失敗する
extern	VECTOR		MV1GetAnimKeyDataToVectorFromTime(	int MHandle, int AnimKeySetIndex, float Time ) ;									// 指定のアニメーションキーセットのキーを取得する、キータイプが MV1_ANIMKEY_TYPE_VECTOR では無かった場合は失敗する( 時間指定版 )
extern	MATRIX		MV1GetAnimKeyDataToMatrix(			int MHandle, int AnimKeySetIndex, int Index ) ;										// 指定のアニメーションキーセットのキーを取得する、キータイプが MV1_ANIMKEY_TYPE_MATRIX4X4C か MV1_ANIMKEY_TYPE_MATRIX3X3 では無かった場合は失敗する
extern	MATRIX		MV1GetAnimKeyDataToMatrixFromTime(	int MHandle, int AnimKeySetIndex, float Time ) ;									// 指定のアニメーションキーセットのキーを取得する、キータイプが MV1_ANIMKEY_TYPE_MATRIX4X4C か MV1_ANIMKEY_TYPE_MATRIX3X3 では無かった場合は失敗する( 時間指定版 )
extern	float		MV1GetAnimKeyDataToFlat(			int MHandle, int AnimKeySetIndex, int Index ) ;										// 指定のアニメーションキーセットのキーを取得する、キータイプが MV1_ANIMKEY_TYPE_FLAT では無かった場合は失敗する
extern	float		MV1GetAnimKeyDataToFlatFromTime(	int MHandle, int AnimKeySetIndex, float Time ) ;									// 指定のアニメーションキーセットのキーを取得する、キータイプが MV1_ANIMKEY_TYPE_FLAT では無かった場合は失敗する( 時間指定版 )
extern	float		MV1GetAnimKeyDataToLinear(			int MHandle, int AnimKeySetIndex, int Index ) ;										// 指定のアニメーションキーセットのキーを取得する、キータイプが MV1_ANIMKEY_TYPE_LINEAR では無かった場合は失敗する
extern	float		MV1GetAnimKeyDataToLinearFromTime(	int MHandle, int AnimKeySetIndex, float Time ) ;									// 指定のアニメーションキーセットのキーを取得する、キータイプが MV1_ANIMKEY_TYPE_LINEAR では無かった場合は失敗する( 時間指定版 )

// マテリアル関係
extern	int			MV1GetMaterialNum(					int MHandle ) ;															// モデルで使用しているマテリアルの数を取得する
extern	const TCHAR *MV1GetMaterialName(				int MHandle, int MaterialIndex ) ;										// 指定のマテリアルの名前を取得する
extern	int			MV1SetMaterialTypeAll(				int MHandle,                    int Type ) ;							// 全てのマテリアルのタイプを変更する( Type : DX_MATERIAL_TYPE_NORMAL など )
extern	int			MV1SetMaterialType(					int MHandle, int MaterialIndex, int Type ) ;							// 指定のマテリアルのタイプを変更する( Type : DX_MATERIAL_TYPE_NORMAL など )
extern	int			MV1GetMaterialType(					int MHandle, int MaterialIndex ) ;										// 指定のマテリアルのタイプを取得する( 戻り値 : DX_MATERIAL_TYPE_NORMAL など )
extern	int			MV1SetMaterialTypeParamAll(			int MHandle,                    ... ) ;									// 全てのマテリアルのタイプ別パラメータを変更する( マテリアルタイプ DX_MATERIAL_TYPE_MAT_SPEC_LUMINANCE_TWO_COLOR などで使用 )
extern	int			MV1SetMaterialTypeParam(			int MHandle, int MaterialIndex, ... ) ;									// 指定のマテリアルのタイプ別パラメータを変更する( マテリアルタイプ DX_MATERIAL_TYPE_MAT_SPEC_LUMINANCE_TWO_COLOR などで使用 )
//		int			MV1SetMaterialTypeParam(			int MHandle, int MaterialIndex, /* DX_MATERIAL_TYPE_MAT_SPEC_LUMINANCE_UNORM            の場合 */ float MinParam = 正規化の下限値( この値以下が 0.0f になる ), float MaxParam = 正規化の上限値( この値以上が 1.0f になる ) ) ;
//		int			MV1SetMaterialTypeParam(			int MHandle, int MaterialIndex, /* DX_MATERIAL_TYPE_MAT_SPEC_LUMINANCE_CLIP_UNORM       の場合 */ float MinParam = 正規化の下限値( この値以下が 0.0f になる ), float MaxParam = 正規化の上限値( この値以上が 1.0f になる ), float ClipParam = 閾値( この値未満が 0.0f になる ) ) ;
//		int			MV1SetMaterialTypeParam(			int MHandle, int MaterialIndex, /* DX_MATERIAL_TYPE_MAT_SPEC_LUMINANCE_CMP_GREATEREQUAL の場合 */ float CmpParam = 比較値( この値以上の場合は 1.0f が、未満の場合は 0.0f が書き込まれる ) ) ;
//		int			MV1SetMaterialTypeParam(			int MHandle, int MaterialIndex, /* DX_MATERIAL_TYPE_MAT_SPEC_POWER_UNORM                の場合 */ float MinParam = 正規化の下限値( この値以下が 0.0f になる ), float MaxParam = 正規化の上限値( この値以上が 1.0f になる ) ) ;
//		int			MV1SetMaterialTypeParam(			int MHandle, int MaterialIndex, /* DX_MATERIAL_TYPE_MAT_SPEC_POWER_CLIP_UNORM           の場合 */ float MinParam = 正規化の下限値( この値以下が 0.0f になる ), float MaxParam = 正規化の上限値( この値以上が 1.0f になる ), float ClipParam = 閾値( この値未満が 0.0f になる ) ) ;
//		int			MV1SetMaterialTypeParam(			int MHandle, int MaterialIndex, /* DX_MATERIAL_TYPE_MAT_SPEC_POWER_CMP_GREATEREQUAL     の場合 */ float CmpParam = 比較値( この値以上の場合は 1.0f が、未満の場合は 0.0f が書き込まれる ) ) ;
extern	int			MV1SetMaterialDifColor(				int MHandle, int MaterialIndex, COLOR_F Color ) ;						// 指定のマテリアルのディフューズカラーを設定する
extern	COLOR_F		MV1GetMaterialDifColor( 			int MHandle, int MaterialIndex ) ;										// 指定のマテリアルのディフューズカラーを取得する
extern	int			MV1SetMaterialSpcColor( 			int MHandle, int MaterialIndex, COLOR_F Color ) ;						// 指定のマテリアルのスペキュラカラーを設定する
extern	COLOR_F		MV1GetMaterialSpcColor( 			int MHandle, int MaterialIndex ) ;										// 指定のマテリアルのスペキュラカラーを取得する
extern	int			MV1SetMaterialEmiColor( 			int MHandle, int MaterialIndex, COLOR_F Color ) ;						// 指定のマテリアルのエミッシブカラーを設定する
extern	COLOR_F		MV1GetMaterialEmiColor( 			int MHandle, int MaterialIndex ) ;										// 指定のマテリアルのエミッシブカラーを取得する
extern	int			MV1SetMaterialAmbColor( 			int MHandle, int MaterialIndex, COLOR_F Color ) ;						// 指定のマテリアルのアンビエントカラーを設定する
extern	COLOR_F		MV1GetMaterialAmbColor( 			int MHandle, int MaterialIndex ) ;										// 指定のマテリアルのアンビエントカラーを取得する
extern	int			MV1SetMaterialSpcPower( 			int MHandle, int MaterialIndex, float Power ) ;							// 指定のマテリアルのスペキュラの強さを設定する
extern	float		MV1GetMaterialSpcPower( 			int MHandle, int MaterialIndex ) ;										// 指定のマテリアルのスペキュラの強さを取得する
extern	int			MV1SetMaterialDifMapTexture(		int MHandle, int MaterialIndex, int TexIndex ) ;						// 指定のマテリアルでディフューズマップとして使用するテクスチャを指定する
extern	int			MV1GetMaterialDifMapTexture(		int MHandle, int MaterialIndex ) ;										// 指定のマテリアルでディフューズマップとして使用されているテクスチャのインデックスを取得する
extern	int			MV1SetMaterialSpcMapTexture(		int MHandle, int MaterialIndex, int TexIndex ) ;						// 指定のマテリアルでスペキュラマップとして使用するテクスチャを指定する
extern	int			MV1GetMaterialSpcMapTexture(		int MHandle, int MaterialIndex ) ;										// 指定のマテリアルでスペキュラマップとして使用されているテクスチャのインデックスを取得する
extern	int			MV1GetMaterialNormalMapTexture(		int MHandle, int MaterialIndex ) ;										// 指定のマテリアルで法線マップとして使用されているテクスチャのインデックスを取得する
extern	int			MV1SetMaterialDifGradTexture(		int MHandle, int MaterialIndex, int TexIndex ) ;						// 指定のマテリアルでトゥーンレンダリングのディフューズグラデーションマップとして使用するテクスチャを設定する
extern	int			MV1GetMaterialDifGradTexture(		int MHandle, int MaterialIndex ) ;										// 指定のマテリアルでトゥーンレンダリングのディフューズグラデーションマップとして使用するテクスチャを取得する
extern	int			MV1SetMaterialSpcGradTexture(		int MHandle, int MaterialIndex, int TexIndex ) ;						// 指定のマテリアルでトゥーンレンダリングのスペキュラグラデーションマップとして使用するテクスチャを設定する
extern	int			MV1GetMaterialSpcGradTexture(		int MHandle, int MaterialIndex ) ;										// 指定のマテリアルでトゥーンレンダリングのスペキュラグラデーションマップとして使用するテクスチャを取得する
extern	int			MV1SetMaterialSphereMapTexture(		int MHandle, int MaterialIndex, int TexIndex ) ;						// 指定のマテリアルでトゥーンレンダリングのスフィアマップとして使用するテクスチャを設定する
extern	int			MV1GetMaterialSphereMapTexture(		int MHandle, int MaterialIndex ) ;										// 指定のマテリアルでトゥーンレンダリングのスフィアマップとして使用するテクスチャを取得する
extern	int			MV1SetMaterialDifGradBlendTypeAll(	int MHandle,                    int BlendType ) ;						// 全てのマテリアルのトゥーンレンダリングで使用するディフューズグラデーションマップとディフューズカラーの合成方法を設定する( DX_MATERIAL_BLENDTYPE_ADDITIVE など )
extern	int			MV1SetMaterialDifGradBlendType(		int MHandle, int MaterialIndex, int BlendType ) ;						// 指定のマテリアルのトゥーンレンダリングで使用するディフューズグラデーションマップとディフューズカラーの合成方法を設定する( DX_MATERIAL_BLENDTYPE_ADDITIVE など )
extern	int			MV1GetMaterialDifGradBlendType(		int MHandle, int MaterialIndex ) ;										// 指定のマテリアルのトゥーンレンダリングで使用するディフューズグラデーションマップとディフューズカラーの合成方法を取得する( DX_MATERIAL_BLENDTYPE_ADDITIVE など )
extern	int			MV1SetMaterialSpcGradBlendTypeAll(	int MHandle,                    int BlendType ) ;						// 全てのマテリアルのトゥーンレンダリングで使用するスペキュラグラデーションマップとスペキュラカラーの合成方法を設定する( DX_MATERIAL_BLENDTYPE_ADDITIVE など )
extern	int			MV1SetMaterialSpcGradBlendType(		int MHandle, int MaterialIndex, int BlendType ) ;						// 指定のマテリアルのトゥーンレンダリングで使用するスペキュラグラデーションマップとスペキュラカラーの合成方法を設定する( DX_MATERIAL_BLENDTYPE_ADDITIVE など )
extern	int			MV1GetMaterialSpcGradBlendType(		int MHandle, int MaterialIndex ) ;										// 指定のマテリアルのトゥーンレンダリングで使用するスペキュラグラデーションマップとスペキュラカラーの合成方法を取得する( DX_MATERIAL_BLENDTYPE_ADDITIVE など )
extern	int			MV1SetMaterialSphereMapBlendTypeAll( int MHandle,                   int BlendType ) ;						// 全てのマテリアルのトゥーンレンダリングで使用するスフィアマップの合成方法を設定する( DX_MATERIAL_BLENDTYPE_ADDITIVE など )
extern	int			MV1SetMaterialSphereMapBlendType(	int MHandle, int MaterialIndex, int BlendType ) ;						// 指定のマテリアルのトゥーンレンダリングで使用するスフィアマップの合成方法を設定する( DX_MATERIAL_BLENDTYPE_ADDITIVE など )
extern	int			MV1GetMaterialSphereMapBlendType(	int MHandle, int MaterialIndex ) ;										// 指定のマテリアルのトゥーンレンダリングで使用するスフィアマップの合成方法を取得する( DX_MATERIAL_BLENDTYPE_ADDITIVE など )
extern	int			MV1SetMaterialOutLineWidthAll(		int MHandle,                    float Width ) ;							// 全てのマテリアルのトゥーンレンダリングで使用する輪郭線の太さを設定する
extern	int			MV1SetMaterialOutLineWidth(			int MHandle, int MaterialIndex, float Width ) ;							// 指定のマテリアルのトゥーンレンダリングで使用する輪郭線の太さを設定する
extern	float		MV1GetMaterialOutLineWidth(			int MHandle, int MaterialIndex ) ;										// 指定のマテリアルのトゥーンレンダリングで使用する輪郭線の太さを取得する
extern	int			MV1SetMaterialOutLineDotWidthAll(	int MHandle,                    float Width ) ;							// 全てのマテリアルのトゥーンレンダリングで使用する輪郭線のドット単位の太さを設定する
extern	int			MV1SetMaterialOutLineDotWidth(		int MHandle, int MaterialIndex, float Width ) ;							// 指定のマテリアルのトゥーンレンダリングで使用する輪郭線のドット単位の太さを設定する
extern	float		MV1GetMaterialOutLineDotWidth(		int MHandle, int MaterialIndex ) ;										// 指定のマテリアルのトゥーンレンダリングで使用する輪郭線のドット単位の太さを取得する
extern	int			MV1SetMaterialOutLineColorAll(		int MHandle,                    COLOR_F Color ) ;						// 全てのマテリアルのトゥーンレンダリングで使用する輪郭線の色を設定する
extern	int			MV1SetMaterialOutLineColor(			int MHandle, int MaterialIndex, COLOR_F Color ) ;						// 指定のマテリアルのトゥーンレンダリングで使用する輪郭線の色を設定する
extern	COLOR_F		MV1GetMaterialOutLineColor(			int MHandle, int MaterialIndex ) ;										// 指定のマテリアルのトゥーンレンダリングで使用する輪郭線の色を取得する
extern	int			MV1SetMaterialDrawBlendModeAll(		int MHandle,                    int BlendMode ) ;						// 全てのマテリアルの描画ブレンドモードを設定する( DX_BLENDMODE_ALPHA 等 )
extern	int			MV1SetMaterialDrawBlendMode(		int MHandle, int MaterialIndex, int BlendMode ) ;						// 指定のマテリアルの描画ブレンドモードを設定する( DX_BLENDMODE_ALPHA 等 )
extern	int			MV1GetMaterialDrawBlendMode(		int MHandle, int MaterialIndex ) ;										// 指定のマテリアルの描画ブレンドモードを取得する( DX_BLENDMODE_ALPHA 等 )
extern	int			MV1SetMaterialDrawBlendParamAll(	int MHandle,                    int BlendParam ) ;						// 全てのマテリアルの描画ブレンドパラメータを設定する
extern	int			MV1SetMaterialDrawBlendParam(		int MHandle, int MaterialIndex, int BlendParam ) ;						// 指定のマテリアルの描画ブレンドパラメータを設定する
extern	int			MV1GetMaterialDrawBlendParam(		int MHandle, int MaterialIndex ) ;										// 指定のマテリアルの描画ブレンドパラメータを設定する
extern	int			MV1SetMaterialDrawAlphaTestAll(		int MHandle,                    int Enable, int Mode, int Param ) ;		// 全てのマテリアルの描画時のアルファテストの設定を行う( Enable:αテストを行うかどうか( TRUE:行う  FALSE:行わない( デフォルト ) ) Mode:テストモード( DX_CMP_GREATER等 )  Param:描画アルファ値との比較に使用する値( 0〜255 ) )
extern	int			MV1SetMaterialDrawAlphaTest(		int MHandle, int MaterialIndex,	int Enable, int Mode, int Param ) ;		// 指定のマテリアルの描画時のアルファテストの設定を行う( Enable:αテストを行うかどうか( TRUE:行う  FALSE:行わない( デフォルト ) ) Mode:テストモード( DX_CMP_GREATER等 )  Param:描画アルファ値との比較に使用する値( 0〜255 ) )
extern	int			MV1GetMaterialDrawAlphaTestEnable(	int MHandle, int MaterialIndex ) ;										// 指定のマテリアルの描画時のアルファテストを行うかどうかを取得する( 戻り値  TRUE:アルファテストを行う  FALSE:アルファテストを行わない )
extern	int			MV1GetMaterialDrawAlphaTestMode(	int MHandle, int MaterialIndex ) ;										// 指定のマテリアルの描画時のアルファテストのテストモードを取得する( 戻り値  テストモード( DX_CMP_GREATER等 ) )
extern	int			MV1GetMaterialDrawAlphaTestParam(	int MHandle, int MaterialIndex ) ;										// 指定のマテリアルの描画時のアルファテストの描画アルファ地との比較に使用する値( 0〜255 )を取得する

// テクスチャ関係
extern	int			MV1GetTextureNum(						int MHandle ) ;														// テクスチャの数を取得
extern	const TCHAR *MV1GetTextureName(						int MHandle, int TexIndex ) ;										// テクスチャの名前を取得
extern	int			MV1SetTextureColorFilePath(				int MHandle, int TexIndex, const TCHAR *FilePath                        ) ;	// カラーテクスチャのファイルパスを変更する
extern	int			MV1SetTextureColorFilePathWithStrLen(	int MHandle, int TexIndex, const TCHAR *FilePath, size_t FilePathLength ) ;	// カラーテクスチャのファイルパスを変更する
extern	const TCHAR *MV1GetTextureColorFilePath(			int MHandle, int TexIndex ) ;										// カラーテクスチャのファイルパスを取得
extern	int			MV1SetTextureAlphaFilePath(				int MHandle, int TexIndex, const TCHAR *FilePath                        ) ;	// アルファテクスチャのファイルパスを変更する
extern	int			MV1SetTextureAlphaFilePathWithStrLen(	int MHandle, int TexIndex, const TCHAR *FilePath, size_t FilePathLength ) ;	// アルファテクスチャのファイルパスを変更する
extern	const TCHAR *MV1GetTextureAlphaFilePath(			int MHandle, int TexIndex ) ;										// アルファテクスチャのファイルパスを取得
extern	int			MV1SetTextureGraphHandle(				int MHandle, int TexIndex, int GrHandle, int SemiTransFlag ) ;		// テクスチャで使用するグラフィックハンドルを変更する( GrHandle を -1 にすると解除 )
extern	int			MV1GetTextureGraphHandle(				int MHandle, int TexIndex ) ;										// テクスチャのグラフィックハンドルを取得する
extern	int			MV1SetTextureAddressMode(				int MHandle, int TexIndex, int AddrUMode, int AddrVMode ) ;			// テクスチャのアドレスモードを設定する( AddUMode の値は DX_TEXADDRESS_WRAP 等 )
extern	int			MV1GetTextureAddressModeU(				int MHandle, int TexIndex ) ;										// テクスチャのＵ値のアドレスモードを取得する( 戻り値:DX_TEXADDRESS_WRAP 等 )
extern	int			MV1GetTextureAddressModeV(				int MHandle, int TexIndex ) ;										// テクスチャのＶ値のアドレスモードを取得する( 戻り値:DX_TEXADDRESS_WRAP 等 )
extern	int			MV1GetTextureWidth(						int MHandle, int TexIndex ) ;										// テクスチャの幅を取得する
extern	int			MV1GetTextureHeight(					int MHandle, int TexIndex ) ;										// テクスチャの高さを取得する
extern	int			MV1GetTextureSemiTransState(			int MHandle, int TexIndex ) ;										// テクスチャに半透明要素があるかどうかを取得する( 戻り値  TRUE:ある  FALSE:ない )
extern	int			MV1SetTextureBumpImageFlag(				int MHandle, int TexIndex, int Flag ) ;								// テクスチャで使用している画像がバンプマップかどうかを設定する
extern	int			MV1GetTextureBumpImageFlag(				int MHandle, int TexIndex ) ;										// テクスチャがバンプマップかどうかを取得する( 戻り値  TRUE:バンプマップ  FALSE:違う )
extern	int			MV1SetTextureBumpImageNextPixelLength(	int MHandle, int TexIndex, float Length ) ;							// バンプマップ画像の場合の隣のピクセルとの距離を設定する
extern	float		MV1GetTextureBumpImageNextPixelLength(	int MHandle, int TexIndex ) ;										// バンプマップ画像の場合の隣のピクセルとの距離を取得する
extern	int			MV1SetTextureSampleFilterMode(			int MHandle, int TexIndex, int FilterMode ) ;						// テクスチャのフィルタリングモードを設定する
extern	int			MV1GetTextureSampleFilterMode(			int MHandle, int TexIndex ) ;										// テクスチャのフィルタリングモードを取得する( 戻り値  DX_DRAWMODE_BILINEAR等 )
extern	int			MV1LoadTexture(							const TCHAR *FilePath                        ) ;					// ３Ｄモデルに貼り付けるのに向いた画像の読み込み方式で画像を読み込む( 戻り値  -1:エラー  0以上:グラフィックハンドル )
extern	int			MV1LoadTextureWithStrLen(				const TCHAR *FilePath, size_t FilePathLength ) ;					// ３Ｄモデルに貼り付けるのに向いた画像の読み込み方式で画像を読み込む( 戻り値  -1:エラー  0以上:グラフィックハンドル )

// フレーム関係
extern	int			MV1GetFrameNum(						int MHandle ) ;															// フレームの数を取得する
extern	int			MV1SearchFrame(						int MHandle, const TCHAR *FrameName                         ) ;			// フレームの名前からモデル中のフレームのフレームインデックスを取得する( 無かった場合は戻り値が-1 )
extern	int			MV1SearchFrameWithStrLen(			int MHandle, const TCHAR *FrameName, size_t FrameNameLength ) ;			// フレームの名前からモデル中のフレームのフレームインデックスを取得する( 無かった場合は戻り値が-1 )
extern	int			MV1SearchFrameChild(				int MHandle, int FrameIndex = -1 , const TCHAR *ChildName = NULL                              ) ;	// フレームの名前から指定のフレームの子フレームのフレームインデックスを取得する( 名前指定版 )( FrameIndex を -1 にすると親を持たないフレームを ChildIndex で指定する )( 無かった場合は戻り値が-1 )
extern	int			MV1SearchFrameChildWithStrLen(		int MHandle, int FrameIndex = -1 , const TCHAR *ChildName = NULL , size_t ChildNameLength = 0 ) ;	// フレームの名前から指定のフレームの子フレームのフレームインデックスを取得する( 名前指定版 )( FrameIndex を -1 にすると親を持たないフレームを ChildIndex で指定する )( 無かった場合は戻り値が-1 )
extern	const TCHAR *MV1GetFrameName(					int MHandle, int FrameIndex ) ;											// 指定のフレームの名前を取得する( エラーの場合は戻り値が NULL )
extern	int			MV1GetFrameName2(					int MHandle, int FrameIndex, TCHAR *StrBuffer ) ;						// 指定のフレームの名前を取得する( 戻り値   -1:エラー  -1以外:文字列のサイズ )
extern	int			MV1GetFrameParent(					int MHandle, int FrameIndex ) ;											// 指定のフレームの親フレームのインデックスを得る( 親がいない場合は -2 が返る )
extern	int			MV1GetFrameChildNum(				int MHandle, int FrameIndex = -1 ) ;									// 指定のフレームの子フレームの数を取得する( FrameIndex を -1 にすると親を持たないフレームの数が返ってくる )
extern	int			MV1GetFrameChild(					int MHandle, int FrameIndex = -1 , int ChildIndex = 0 ) ;				// 指定のフレームの子フレームのフレームインデックスを取得する( 番号指定版 )( FrameIndex を -1 にすると親を持たないフレームを ChildIndex で指定する )( エラーの場合は戻り値が-1 )
extern	VECTOR		MV1GetFramePosition(				int MHandle, int FrameIndex ) ;											// 指定のフレームの座標を取得する
extern	VECTOR_D	MV1GetFramePositionD(				int MHandle, int FrameIndex ) ;											// 指定のフレームの座標を取得する
extern	MATRIX		MV1GetFrameBaseLocalMatrix(			int MHandle, int FrameIndex ) ;											// 指定のフレームの初期状態での座標変換行列を取得する
extern	MATRIX_D	MV1GetFrameBaseLocalMatrixD(		int MHandle, int FrameIndex ) ;											// 指定のフレームの初期状態での座標変換行列を取得する
extern	MATRIX		MV1GetFrameLocalMatrix(				int MHandle, int FrameIndex ) ;											// 指定のフレームの座標変換行列を取得する
extern	MATRIX_D	MV1GetFrameLocalMatrixD(			int MHandle, int FrameIndex ) ;											// 指定のフレームの座標変換行列を取得する
extern	MATRIX		MV1GetFrameLocalWorldMatrix(		int MHandle, int FrameIndex ) ;											// 指定のフレームのローカル座標からワールド座標に変換する行列を得る
extern	MATRIX_D	MV1GetFrameLocalWorldMatrixD(		int MHandle, int FrameIndex ) ;											// 指定のフレームのローカル座標からワールド座標に変換する行列を得る
extern	int			MV1SetFrameUserLocalMatrix(			int MHandle, int FrameIndex, MATRIX   Matrix ) ;						// 指定のフレームの座標変換行列( ローカル行列 )を設定する
extern	int			MV1SetFrameUserLocalMatrixD(		int MHandle, int FrameIndex, MATRIX_D Matrix ) ;						// 指定のフレームの座標変換行列( ローカル行列 )を設定する
extern	int			MV1ResetFrameUserLocalMatrix(		int MHandle, int FrameIndex ) ;											// 指定のフレームの座標変換行列( ローカル行列 )をデフォルトに戻す
extern	int			MV1SetFrameUserLocalWorldMatrix(	int MHandle, int FrameIndex, MATRIX   Matrix ) ;						// 指定のフレームの座標変換行列( ローカル座標からワールド座標に変換する行列 )を設定する
extern	int			MV1SetFrameUserLocalWorldMatrixD(	int MHandle, int FrameIndex, MATRIX_D Matrix ) ;						// 指定のフレームの座標変換行列( ローカル座標からワールド座標に変換する行列 )を設定する
extern	int			MV1ResetFrameUserLocalWorldMatrix(	int MHandle, int FrameIndex ) ;											// 指定のフレームの座標変換行列( ローカル座標からワールド座標に変換する行列 )をデフォルトに戻す
extern	VECTOR		MV1GetFrameMaxVertexLocalPosition(	int MHandle, int FrameIndex ) ;											// 指定のフレームが持つメッシュ頂点のローカル座標での最大値を得る
extern	VECTOR_D	MV1GetFrameMaxVertexLocalPositionD(	int MHandle, int FrameIndex ) ;											// 指定のフレームが持つメッシュ頂点のローカル座標での最大値を得る
extern	VECTOR		MV1GetFrameMinVertexLocalPosition(	int MHandle, int FrameIndex ) ;											// 指定のフレームが持つメッシュ頂点のローカル座標での最小値を得る
extern	VECTOR_D	MV1GetFrameMinVertexLocalPositionD(	int MHandle, int FrameIndex ) ;											// 指定のフレームが持つメッシュ頂点のローカル座標での最小値を得る
extern	VECTOR		MV1GetFrameAvgVertexLocalPosition(	int MHandle, int FrameIndex ) ;											// 指定のフレームが持つメッシュ頂点のローカル座標での平均値を得る
extern	VECTOR_D	MV1GetFrameAvgVertexLocalPositionD(	int MHandle, int FrameIndex ) ;											// 指定のフレームが持つメッシュ頂点のローカル座標での平均値を得る
extern	int			MV1GetFrameTriangleNum(				int MHandle, int FrameIndex ) ;											// 指定のフレームに含まれるポリゴンの数を取得する
extern	int			MV1GetFrameMeshNum(					int MHandle, int FrameIndex ) ;											// 指定のフレームが持つメッシュの数を取得する
extern	int			MV1GetFrameMesh(					int MHandle, int FrameIndex, int Index ) ;								// 指定のフレームが持つメッシュのメッシュインデックスを取得する
extern	int			MV1SetFrameVisible(					int MHandle, int FrameIndex, int VisibleFlag ) ;						// 指定のフレームの表示、非表示状態を変更する( TRUE:表示  FALSE:非表示 )
extern	int			MV1GetFrameVisible(					int MHandle, int FrameIndex ) ;											// 指定のフレームの表示、非表示状態を取得する( TRUE:表示  FALSE:非表示 )
extern	int			MV1SetFrameDifColorScale(			int MHandle, int FrameIndex, COLOR_F Scale ) ;							// 指定のフレームのディフューズカラーのスケール値を設定する( デフォルト値は 1.0f )
extern	int			MV1SetFrameSpcColorScale(			int MHandle, int FrameIndex, COLOR_F Scale ) ;							// 指定のフレームのスペキュラカラーのスケール値を設定する( デフォルト値は 1.0f )
extern	int			MV1SetFrameEmiColorScale(			int MHandle, int FrameIndex, COLOR_F Scale ) ;							// 指定のフレームのエミッシブカラーのスケール値を設定する( デフォルト値は 1.0f )
extern	int			MV1SetFrameAmbColorScale(			int MHandle, int FrameIndex, COLOR_F Scale ) ;							// 指定のフレームのアンビエントカラーのスケール値を設定する( デフォルト値は 1.0f )
extern	COLOR_F		MV1GetFrameDifColorScale(			int MHandle, int FrameIndex ) ;											// 指定のフレームのディフューズカラーのスケール値を取得する( デフォルト値は 1.0f )
extern	COLOR_F		MV1GetFrameSpcColorScale(			int MHandle, int FrameIndex ) ;											// 指定のフレームのスペキュラカラーのスケール値を取得する( デフォルト値は 1.0f )
extern	COLOR_F		MV1GetFrameEmiColorScale(			int MHandle, int FrameIndex ) ;											// 指定のフレームのエミッシブカラーのスケール値を取得する( デフォルト値は 1.0f )
extern	COLOR_F		MV1GetFrameAmbColorScale(			int MHandle, int FrameIndex ) ;											// 指定のフレームのアンビエントカラーのスケール値を取得する( デフォルト値は 1.0f )
extern	int			MV1GetFrameSemiTransState(			int MHandle, int FrameIndex ) ;											// 指定のフレームに半透明要素があるかどうかを取得する( 戻り値 TRUE:ある  FALSE:ない )
extern	int			MV1SetFrameOpacityRate(				int MHandle, int FrameIndex, float Rate ) ;								// 指定のフレームの不透明度を設定する( 不透明 1.0f 〜 透明 0.0f )
extern	float		MV1GetFrameOpacityRate(				int MHandle, int FrameIndex ) ;											// 指定のフレームの不透明度を取得する( 不透明 1.0f 〜 透明 0.0f )
extern	int			MV1SetFrameBaseVisible(				int MHandle, int FrameIndex, int VisibleFlag ) ;						// 指定のフレームの初期表示状態を設定する( TRUE:表示  FALSE:非表示 )
extern	int			MV1GetFrameBaseVisible(				int MHandle, int FrameIndex ) ;											// 指定のフレームの初期表示状態を取得する( TRUE:表示  FALSE:非表示 )
extern	int			MV1SetFrameTextureAddressTransform( int MHandle, int FrameIndex, float TransU, float TransV, float ScaleU, float ScaleV, float RotCenterU, float RotCenterV, float Rotate ) ;	// 指定のフレームのテクスチャ座標変換パラメータを設定する
extern	int			MV1SetFrameTextureAddressTransformMatrix( int MHandle, int FrameIndex, MATRIX Matrix ) ;					// 指定のフレームのテクスチャ座標変換行列をセットする
extern	int			MV1ResetFrameTextureAddressTransform( int MHandle, int FrameIndex ) ;										// 指定のフレームのテクスチャ座標変換パラメータをリセットする

// メッシュ関係
extern	int			MV1GetMeshNum(						int MHandle ) ;															// モデルに含まれるメッシュの数を取得する
extern	int			MV1GetMeshMaterial(					int MHandle, int MeshIndex ) ;											// 指定メッシュが使用しているマテリアルのインデックスを取得する
extern	int			MV1GetMeshTriangleNum(				int MHandle, int MeshIndex ) ;											// 指定メッシュに含まれる三角形ポリゴンの数を取得する
extern	int			MV1SetMeshVisible(					int MHandle, int MeshIndex, int VisibleFlag ) ;							// 指定メッシュの表示、非表示状態を変更する( TRUE:表示  FALSE:非表示 )
extern	int			MV1GetMeshVisible(					int MHandle, int MeshIndex ) ;											// 指定メッシュの表示、非表示状態を取得する( TRUE:表示  FALSE:非表示 )
extern	int			MV1SetMeshDifColorScale(			int MHandle, int MeshIndex, COLOR_F Scale ) ;							// 指定のメッシュのディフューズカラーのスケール値を設定する( デフォルト値は 1.0f )
extern	int			MV1SetMeshSpcColorScale( 			int MHandle, int MeshIndex, COLOR_F Scale ) ;							// 指定のメッシュのスペキュラカラーのスケール値を設定する( デフォルト値は 1.0f )
extern	int			MV1SetMeshEmiColorScale( 			int MHandle, int MeshIndex, COLOR_F Scale ) ;							// 指定のメッシュのエミッシブカラーのスケール値を設定する( デフォルト値は 1.0f )
extern	int			MV1SetMeshAmbColorScale( 			int MHandle, int MeshIndex, COLOR_F Scale ) ;							// 指定のメッシュのアンビエントカラーのスケール値を設定する( デフォルト値は 1.0f )
extern	COLOR_F		MV1GetMeshDifColorScale( 			int MHandle, int MeshIndex ) ;											// 指定のメッシュのディフューズカラーのスケール値を取得する( デフォルト値は 1.0f )
extern	COLOR_F		MV1GetMeshSpcColorScale( 			int MHandle, int MeshIndex ) ;											// 指定のメッシュのスペキュラカラーのスケール値を取得する( デフォルト値は 1.0f )
extern	COLOR_F		MV1GetMeshEmiColorScale( 			int MHandle, int MeshIndex ) ;											// 指定のメッシュのエミッシブカラーのスケール値を取得する( デフォルト値は 1.0f )
extern	COLOR_F		MV1GetMeshAmbColorScale( 			int MHandle, int MeshIndex ) ;											// 指定のメッシュのアンビエントカラーのスケール値を取得する( デフォルト値は 1.0f )
extern	int			MV1SetMeshOpacityRate( 				int MHandle, int MeshIndex, float Rate ) ;								// 指定のメッシュの不透明度を設定する( 不透明 1.0f 〜 透明 0.0f )
extern	float		MV1GetMeshOpacityRate( 				int MHandle, int MeshIndex ) ;											// 指定のメッシュの不透明度を取得する( 不透明 1.0f 〜 透明 0.0f )
extern	int			MV1SetMeshDrawBlendMode( 			int MHandle, int MeshIndex, int BlendMode ) ;							// 指定のメッシュの描画ブレンドモードを設定する( DX_BLENDMODE_ALPHA 等 )
extern	int			MV1SetMeshDrawBlendParam( 			int MHandle, int MeshIndex, int BlendParam ) ;							// 指定のメッシュの描画ブレンドパラメータを設定する
extern	int			MV1GetMeshDrawBlendMode( 			int MHandle, int MeshIndex ) ;											// 指定のメッシュの描画ブレンドモードを取得する( DX_BLENDMODE_ALPHA 等 )
extern	int			MV1GetMeshDrawBlendParam( 			int MHandle, int MeshIndex ) ;											// 指定のメッシュの描画ブレンドパラメータを設定する
extern	int			MV1SetMeshBaseVisible( 				int MHandle, int MeshIndex, int VisibleFlag ) ;							// 指定のメッシュの初期表示状態を設定する( TRUE:表示  FALSE:非表示 )
extern	int			MV1GetMeshBaseVisible( 				int MHandle, int MeshIndex ) ;											// 指定のメッシュの初期表示状態を取得する( TRUE:表示  FALSE:非表示 )
extern	int			MV1SetMeshBackCulling( 				int MHandle, int MeshIndex, int CullingFlag ) ;							// 指定のメッシュのバックカリングを行うかどうかを設定する( DX_CULLING_LEFT 等 )
extern	int			MV1GetMeshBackCulling( 				int MHandle, int MeshIndex ) ;											// 指定のメッシュのバックカリングを行うかどうかを取得する( DX_CULLING_LEFT 等 )
extern	VECTOR		MV1GetMeshMaxPosition( 				int MHandle, int MeshIndex ) ;											// 指定のメッシュに含まれるポリゴンの最大ローカル座標を取得する
extern	VECTOR		MV1GetMeshMinPosition( 				int MHandle, int MeshIndex ) ;											// 指定のメッシュに含まれるポリゴンの最小ローカル座標を取得する
extern	int			MV1GetMeshTListNum( 				int MHandle, int MeshIndex ) ;											// 指定のメッシュに含まれるトライアングルリストの数を取得する
extern	int			MV1GetMeshTList( 					int MHandle, int MeshIndex, int Index ) ;								// 指定のメッシュに含まれるトライアングルリストのインデックスを取得する
extern	int			MV1GetMeshSemiTransState( 			int MHandle, int MeshIndex ) ;											// 指定のメッシュに半透明要素があるかどうかを取得する( 戻り値 TRUE:ある  FALSE:ない )
extern	int			MV1SetMeshUseVertDifColor( 			int MHandle, int MeshIndex, int UseFlag ) ;								// 指定のメッシュの頂点ディフューズカラーをマテリアルのディフューズカラーの代わりに使用するかどうかを設定する( TRUE:マテリアルカラーの代わりに使用する  FALSE:マテリアルカラーを使用する )
extern	int			MV1SetMeshUseVertSpcColor( 			int MHandle, int MeshIndex, int UseFlag ) ;								// 指定のメッシュの頂点スペキュラカラーをマテリアルのスペキュラカラーの代わりに使用するかどうかを設定する( TRUE:マテリアルカラーの代わりに使用する  FALSE:マテリアルカラーを使用する )
extern	int			MV1GetMeshUseVertDifColor( 			int MHandle, int MeshIndex ) ;											// 指定のメッシュの頂点ディフューズカラーをマテリアルのディフューズカラーの代わりに使用するかどうかの設定を取得する( 戻り値  TRUE:マテリアルカラーの代わりに使用する  FALSE:マテリアルカラーを使用する )
extern	int			MV1GetMeshUseVertSpcColor( 			int MHandle, int MeshIndex ) ;											// 指定のメッシュの頂点スペキュラカラーをマテリアルのスペキュラカラーの代わりに使用するかどうかの設定を取得する( 戻り値  TRUE:マテリアルカラーの代わりに使用する  FALSE:マテリアルカラーを使用する )
extern	int			MV1GetMeshShapeFlag(				int MHandle, int MeshIndex ) ;											// 指定のメッシュがシェイプメッシュかどうかを取得する( 戻り値 TRUE:シェイプメッシュ  FALSE:通常メッシュ )

// シェイプ関係
extern	int			MV1GetShapeNum(						int MHandle ) ;															// モデルに含まれるシェイプの数を取得する
extern	int			MV1SearchShape(						int MHandle, const TCHAR *ShapeName                         ) ;			// シェイプの名前からモデル中のシェイプのシェイプインデックスを取得する( 無かった場合は戻り値が-1 )
extern	int			MV1SearchShapeWithStrLen(			int MHandle, const TCHAR *ShapeName, size_t ShapeNameLength ) ;			// シェイプの名前からモデル中のシェイプのシェイプインデックスを取得する( 無かった場合は戻り値が-1 )
extern	const TCHAR *MV1GetShapeName(					int MHandle, int ShapeIndex ) ;											// 指定シェイプの名前を取得する
extern	int			MV1GetShapeTargetMeshNum(			int MHandle, int ShapeIndex ) ;											// 指定シェイプが対象としているメッシュの数を取得する
extern	int			MV1GetShapeTargetMesh(				int MHandle, int ShapeIndex, int Index ) ;								// 指定シェイプが対象としているメッシュのメッシュインデックスを取得する
extern	int			MV1SetShapeRate(					int MHandle, int ShapeIndex, float Rate, int Type = DX_MV1_SHAPERATE_ADD ) ;	// 指定シェイプの有効率を設定する( Rate  0.0f:0% 〜 1.0f:100% )
extern	float		MV1GetShapeRate(					int MHandle, int ShapeIndex ) ;											// 指定シェイプの有効率を取得する( 戻り値  0.0f:0% 〜 1.0f:100% )
extern	float		MV1GetShapeApplyRate(				int MHandle, int ShapeIndex ) ;											// 指定シェイプの有効率を取得する( 戻り値  0.0f:0% 〜 1.0f:100% )( MV1SetShapeRate で指定した値がそのまま戻り値となる MV1GetShapeRate と異なりアニメーションのシェイプ情報なども加味した値が戻り値となります )

// トライアングルリスト関係
extern	int			MV1GetTriangleListNum(						int MHandle ) ;																		// モデルに含まれるトライアングルリストの数を取得する
extern	int			MV1GetTriangleListVertexType(				int MHandle, int TListIndex ) ;														// 指定のトライアングルリストの頂点データタイプを取得する( DX_MV1_VERTEX_TYPE_1FRAME 等 )
extern	int			MV1GetTriangleListPolygonNum(				int MHandle, int TListIndex ) ;														// 指定のトライアングルリストに含まれるポリゴンの数を取得する
extern	int			MV1GetTriangleListVertexNum(				int MHandle, int TListIndex ) ;														// 指定のトライアングルリストに含まれる頂点データの数を取得する
extern	int			MV1GetTriangleListLocalWorldMatrixNum(		int MHandle, int TListIndex ) ;														// 指定のトライアングルリストが使用する座標変換行列の数を取得する
extern	MATRIX		MV1GetTriangleListLocalWorldMatrix(			int MHandle, int TListIndex, int LWMatrixIndex ) ;									// 指定のトライアングルリストが使用する座標変換行列( ローカル→ワールド )を取得する
extern	int			MV1GetTriangleListPolygonVertexPosition(	int MHandle, int TListIndex, int PolygonIndex, VECTOR *VertexPositionArray = NULL , float *MatrixWeightArray = NULL ) ;	// 指定のトライアングルリストの指定のポリゴンが使用している頂点の座標を取得する( 戻り値  エラー：-1  0以上：ポリゴンが使用している頂点の数 )
extern	int			MV1GetTriangleListUseMaterial(				int MHandle, int TListIndex ) ;														// 指定のトライアングルリストが使用しているマテリアルのインデックスを取得する

// コリジョン関係
extern	int							MV1SetupCollInfo(				int MHandle, int FrameIndex = -1 , int XDivNum = 32 , int YDivNum = 8 , int ZDivNum = 32 ) ;	// コリジョン情報を構築する
extern	int							MV1TerminateCollInfo(			int MHandle, int FrameIndex = -1 ) ;															// コリジョン情報の後始末
extern	int							MV1RefreshCollInfo(				int MHandle, int FrameIndex = -1 ) ;															// コリジョン情報を更新する
extern	MV1_COLL_RESULT_POLY		MV1CollCheck_Line(				int MHandle, int FrameIndex, VECTOR PosStart, VECTOR PosEnd ) ;									// 線とモデルの当たり判定
extern	MV1_COLL_RESULT_POLY_DIM	MV1CollCheck_LineDim(			int MHandle, int FrameIndex, VECTOR PosStart, VECTOR PosEnd ) ;									// 線とモデルの当たり判定( 戻り値が MV1_COLL_RESULT_POLY_DIM )
extern	MV1_COLL_RESULT_POLY_DIM	MV1CollCheck_Sphere(			int MHandle, int FrameIndex, VECTOR CenterPos, float r ) ;										// 球とモデルの当たり判定
extern	MV1_COLL_RESULT_POLY_DIM	MV1CollCheck_Capsule(			int MHandle, int FrameIndex, VECTOR Pos1, VECTOR Pos2, float r ) ;								// カプセルとモデルの当たり判定
extern	MV1_COLL_RESULT_POLY_DIM	MV1CollCheck_Triangle(			int MHandle, int FrameIndex, VECTOR Pos1, VECTOR Pos2, VECTOR Pos3 ) ;							// 三角形とモデルの当たり判定
extern	MV1_COLL_RESULT_POLY		MV1CollCheck_GetResultPoly(		MV1_COLL_RESULT_POLY_DIM ResultPolyDim, int PolyNo ) ;											// コリジョン結果ポリゴン配列から指定番号のポリゴン情報を取得する
extern	int							MV1CollResultPolyDimTerminate(	MV1_COLL_RESULT_POLY_DIM ResultPolyDim ) ;														// コリジョン結果ポリゴン配列の後始末をする

// 参照用メッシュ関係
extern	int					MV1SetupReferenceMesh(		int MHandle, int FrameIndex, int IsTransform, int IsPositionOnly = FALSE ) ;						// 参照用メッシュのセットアップ
extern	int					MV1TerminateReferenceMesh(	int MHandle, int FrameIndex, int IsTransform, int IsPositionOnly = FALSE ) ;						// 参照用メッシュの後始末
extern	int					MV1RefreshReferenceMesh(	int MHandle, int FrameIndex, int IsTransform, int IsPositionOnly = FALSE ) ;						// 参照用メッシュの更新
extern	MV1_REF_POLYGONLIST	MV1GetReferenceMesh(		int MHandle, int FrameIndex, int IsTransform, int IsPositionOnly = FALSE ) ;						// 参照用メッシュを取得する

#endif // DX_NON_MODEL

#undef DX_FUNCTION_START
#define DX_FUNCTION_END
#undef DX_FUNCTION_END

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

// ネームスペース DxLib を使用する ------------------------------------------------------
#ifndef DX_NON_NAMESPACE
#ifndef DX_NON_USING_NAMESPACE_DXLIB

using namespace DxLib ;

#endif // DX_NON_USING_NAMESPACE_DXLIB
#endif // DX_NON_NAMESPACE

// ＤＸライブラリ内部でのみ使用するヘッダファイルのインクルード -------------------------

#ifdef __DX_MAKE
//	#include "DxStatic.h"
#endif

#endif


