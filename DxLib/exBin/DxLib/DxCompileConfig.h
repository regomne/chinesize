// ----------------------------------------------------------------------------
//
//		ＤＸライブラリ　コンパイルコンフィグヘッダファイル
//
//				Ver 3.20c
//
// ----------------------------------------------------------------------------

/*
<<注意>>
　　このヘッダファイルの #define のコメントアウト状態を変更しても、
  ＤＸライブラリ自体の再コンパイルを行わないと効果はありません。
*/

// 多重インクルード防止用マクロ
#ifndef __DXCOMPILECONFIG_H__
#define __DXCOMPILECONFIG_H__

// スタティックライブラリ生成時ライブラリ機能制限用定義 -----------------------

// namespace DxLib を使用しない場合は次のコメントを外してください
//#define DX_NON_NAMESPACE

// DxLib.h に書かれている using namespace DxLib ; を使用しない場合は次のコメントを外してください
//#define DX_NON_USING_NAMESPACE_DXLIB

// インラインアセンブラを使用しないソースコードでコンパイルする場合は以下のコメントアウトを外してください
//#define DX_NON_INLINE_ASM

// ＤＸライブラリのプログラムから文字列を一切排除する場合はコメントを外してください
// (バイナリエディタで実行ファイルを開いてもＤＸライブラリを使用していることが分かり難くなります(よく調べれば当然分かりますが)
//  因みに、エラー出力が一切されなくなりますので管理人の対応時に支障をきたすかもしれません)
//#define DX_NON_LITERAL_STRING

// すべての関数をスレッドセーフにする場合はコメントを外してください
//#define DX_THREAD_SAFE

// ネットワーク関連の関数だけスレッドセーフにする場合はコメントを外して下さい
#define DX_THREAD_SAFE_NETWORK_ONLY

// ＤＸアーカイブがいらない方は次のコメントを外してください
// ( ＤＸアーカイブを無効にすると、ＤＸアーカイブを内部で使っている関係上 DX_NON_MODEL と DX_NON_FILTER と DX_NON_MOVIE と DX_NON_NORMAL_DRAW_SHADER も有効になります )
//#define DX_NON_DXA

// ムービー機能がいらない方は次のコメントを外してください
#define DX_NON_MOVIE

// ＴＧＡ画像の読み込み機能がいらない方は次のコメントを外してください
#define DX_NON_TGA

// ＪＰＥＧ画像の読みこみ機能がいらない方は次のコメントを外してください
// ※DxUseCLib.lib も再コンパイルする必要があります
#define DX_NON_JPEGREAD

// ＰＮＧ画像の読みこみ機能がいらない方は次のコメントを外してください
// ※DxUseCLib.lib も再コンパイルする必要があります
#define DX_NON_PNGREAD

// ＴＩＦＦ画像の読みこみ機能がいらない方は次のコメントを外してください
// ※DxUseCLib.lib も再コンパイルする必要があります
#define DX_NON_TIFFREAD

// GraphFilter や GraphBlend を使用しない方は次のコメントを外して下さい
#define DX_NON_FILTER

// 通常描画にプログラマブルシェーダーを使用しない方は次のコメントを外してください
#define DX_NON_NORMAL_DRAW_SHADER

// グラフィックス処理がいらない方は次のコメントを外してください
//#define DX_NON_GRAPHICS

// ソフトウエア２Ｄ描画がいらない方は次のコメントを外してください
#define DX_NON_2DDRAW

// 標準 WAVE ファイル以外の音声ファイルを使用しない方は次のコメントを外してください
#define DX_NON_ACM

// DirectShow を使用した MP3 ファイルのデコードを行わない場合は次のコメントをはずしてください
#define DX_NON_DSHOW_MP3

// DirectShow を使用した動画ファイルのデコードを行わない場合は次のコメントをはずしてください
#define DX_NON_DSHOW_MOVIE

// Media Foundation を使用した動画ファイルのデコードを行わない場合は次のコメントをはずしてください
#define DX_NON_MEDIA_FOUNDATION

// キーボードの基本的な入力機能以外はいらない方は次のコメントを外してください
#define DX_NON_KEYEX

// 文字列入力機能はいらない方は次のコメントを外して下さい
#define DX_NON_INPUTSTRING

// 通信機能がいらない方は次のコメントを外してください
#define DX_NON_NETWORK

// マスク機能がいらない方は次のコメントを外してください
//#define DX_NON_MASK

// Ｏｇｇ Ｖｏｒｂｉｓ データを使用しない方は次のコメントをはずしてください
// ※DxUseCLib.lib も再コンパイルする必要があります
#define DX_NON_OGGVORBIS

// Ｏｇｇ Ｔｈｅｏｒａ データを使用しない方は次のコメントをはずしてください
// ※DxUseCLib.lib も再コンパイルする必要があります
#define DX_NON_OGGTHEORA

// Opus データを使用しない方は次のコメントをはずしてください
// ※DxUseCLib.lib も再コンパイルする必要があります
#define DX_NON_OPUS

// 乱数発生器に Mersenne Twister を使用しない場合は以下のコメントを外して下さい
// ※DxUseCLib.lib も再コンパイルする必要があります
#define DX_NON_MERSENNE_TWISTER

// ＤＸライブラリ内の確保メモリのダンプ機能を有効にする場合は次のコメントを外してください
#define DX_USE_DXLIB_MEM_DUMP

// モデルデータを扱う機能を使用しない方は次のコメントを外してください
//#define DX_NON_MODEL

// Bullet Physics をリンクしない場合は次のコメントを外してください
// ※DxUseCLib.lib も再コンパイルする必要があります
#define DX_NON_BULLET_PHYSICS

// ＦＢＸファイルを読み込む機能を使用する方は次のコメントを外してください( 使用には FBX SDK のセットアップが必要です )
#define DX_LOAD_FBX_MODEL

// ビープ音機能がいらない方は次のコメントを外してください
#define DX_NON_BEEP

// タスクスイッチをＯＦＦにする機能がいらない方は次のコメントを外してください
// ( タスクスイッチＯＦＦ機能は使用不可です )
#define DX_NON_STOPTASKSWITCH

// ログ出力を行わない場合は次のコメントを外して下さい
//#define DX_NON_LOG

// printfDx を無効にする場合は次のコメントを外して下さい
//#define DX_NON_PRINTF_DX

// 非同期読み込みを無効にする場合は次のコメントを外して下さい
//#define DX_NON_ASYNCLOAD

// ファイル保存機能を無効にする場合は次のコメントを外して下さい
//#define DX_NON_SAVEFUNCTION

// ソフトウェアで扱うイメージを無効にする場合は次のコメントを外して下さい
#define DX_NON_SOFTIMAGE

// フォント描画機能を無効にする場合は次のコメントを外して下さい
//#define DX_NON_FONT

// サウンド再生機能( ソフトウエアサウンド、MIDI含む )を無効にする場合は次のコメントを外して下さい
#define DX_NON_SOUND

// 入力機能を無効にする場合は次のコメントを外して下さい
#define DX_NON_INPUT

// マルチスレッドを使用しない場合は次のコメントを外してください
//#define DX_NON_MULTITHREAD

// 各ハンドルのエラーチェックを無効にする場合は次のコメントを外してください( 若干高速化される代わりに無効なハンドルを関数に渡すと即不正なメモリアクセスエラーが発生するようになります )
//#define DX_NON_HANDLE_ERROR_CHECK

// Direct3D11 を使用しない場合は以下のコメントを外してください( 現在開発中なので、必ずコメントを外した状態にしてください )
#define DX_NON_DIRECT3D11

// Direct3D9 を使用しない場合は以下のコメントを外してください
//#define DX_NON_DIRECT3D9

// 軽量バージョンのＤＸライブラリを生成する場合は次のコメントを外してください
#define DX_LIB_LITEVER

#ifndef __APPLE__
#ifndef __ANDROID__
	#define __WINDOWS__
#endif // __ANDROID__
#endif // __APPLE__

#if defined( DX_GCC_COMPILE ) || defined( __ANDROID__ ) || defined( __APPLE__ ) || defined( __BCC2 )
	#define __USE_ULL__
#endif


#ifndef __WINDOWS__
	#ifndef DX_NON_BEEP
		#define DX_NON_BEEP
	#endif // DX_NON_BEEP
	#ifndef DX_NON_ACM
		#define DX_NON_ACM
	#endif // DX_NON_ACM
	#ifndef DX_NON_DSHOW_MP3
		#define DX_NON_DSHOW_MP3
	#endif // DX_NON_DSHOW_MP3
	#ifndef DX_NON_DSHOW_MOVIE
		#define DX_NON_DSHOW_MOVIE
	#endif // DX_NON_DSHOW_MOVIE
	#ifndef DX_NON_MEDIA_FOUNDATION
		#define DX_NON_MEDIA_FOUNDATION
	#endif // DX_NON_MEDIA_FOUNDATION
#endif // __WINDOWS__

#if defined( __ANDROID__ ) || defined( __APPLE__ )
#define DX_NON_2DDRAW
#define DX_NON_ACM
#define DX_NON_DSHOW_MP3
#define DX_NON_DSHOW_MOVIE
#define DX_NON_MEDIA_FOUNDATION
#define DX_NON_KEYEX
#define DX_NON_INPUTSTRING
#define DX_NON_NETWORK
#define DX_NON_STOPTASKSWITCH
#define DX_NON_DIRECT3D11
#define DX_NON_DIRECT3D9
#endif




#ifdef DX_LIB_LITEVER
#define DX_NON_ACM
#define DX_NON_2DDRAW
#define DX_NON_MOVIE
#define DX_NON_KEYEX
#define DX_NON_NETWORK
#define DX_NON_MASK
#define DX_NON_JPEGREAD
#define DX_NON_PNGREAD
#define DX_NON_BEEP
#define DX_NON_OGGVORBIS
#define DX_NON_OPUS
#define DX_NON_MODEL
#endif

#ifdef DX_NON_GRAPHICS
	#ifndef DX_NON_FONT
		#define DX_NON_FONT
	#endif
	#ifndef DX_NON_MOVIE
		#define DX_NON_MOVIE
	#endif
	#ifndef DX_NON_FILTER
		#define DX_NON_FILTER
	#endif
	#ifndef DX_NON_2DDRAW
		#define DX_NON_2DDRAW
	#endif
	#ifndef DX_NON_MASK
		#define DX_NON_MASK
	#endif
	#ifndef DX_NON_MODEL
		#define DX_NON_MODEL
	#endif
	#ifndef DX_NON_BULLET_PHYSICS
		#define DX_NON_BULLET_PHYSICS
	#endif
	#ifndef DX_NON_PRINTF_DX
		#define DX_NON_PRINTF_DX
	#endif
	#ifndef DX_NON_KEYEX
		#define DX_NON_KEYEX
	#endif
	#ifndef DX_NON_DIRECT3D11
		#define DX_NON_DIRECT3D11
	#endif
	#ifndef DX_NON_DIRECT3D9
		#define DX_NON_DIRECT3D9
	#endif
	#ifndef DX_NOTUSE_DRAWFUNCTION
		#define DX_NOTUSE_DRAWFUNCTION
	#endif
#endif // DX_NON_GRAPHICS

#ifdef DX_NON_SOUND
	#ifndef DX_NON_OPUS
		#define DX_NON_OPUS
	#endif
	#ifndef DX_NON_OGGVORBIS
		#define DX_NON_OGGVORBIS
	#endif
	#ifndef DX_NON_OGGTHEORA
		#define DX_NON_OGGTHEORA
	#endif
	#ifndef DX_NON_ACM
		#define DX_NON_ACM
	#endif
	#ifndef DX_NON_DSHOW_MP3
		#define DX_NON_DSHOW_MP3
	#endif
	#ifndef DX_NON_DSHOW_MOVIE
		#define DX_NON_DSHOW_MOVIE
	#endif
	#ifndef DX_NON_MEDIA_FOUNDATION
		#define DX_NON_MEDIA_FOUNDATION
	#endif
	#ifndef DX_NON_MOVIE
		#define DX_NON_MOVIE
	#endif
#endif // DX_NON_SOUND

#ifdef DX_NON_MULTITHREAD
	#ifndef DX_NON_ASYNCLOAD
		#define DX_NON_ASYNCLOAD
	#endif
	#ifdef DX_THREAD_SAFE
		#undef DX_THREAD_SAFE
	#endif
#endif

#ifdef DX_NON_INPUTSTRING
	#ifndef DX_NON_KEYEX
		#define DX_NON_KEYEX
	#endif
#endif

#ifdef DX_NON_DXA
	#ifndef DX_NON_NORMAL_DRAW_SHADER
		#define DX_NON_NORMAL_DRAW_SHADER
	#endif
	#ifndef DX_NON_MODEL
		#define DX_NON_MODEL
	#endif
	#ifndef DX_NON_FILTER
		#define DX_NON_FILTER
	#endif
	#ifndef DX_NON_MOVIE
		#define DX_NON_MOVIE
	#endif
#endif

#ifdef DX_NON_MOVIE
	#ifndef DX_NON_OGGTHEORA
		#define DX_NON_OGGTHEORA
	#endif
	#ifndef DX_NON_MEDIA_FOUNDATION
		#define DX_NON_MEDIA_FOUNDATION
	#endif // DX_NON_MEDIA_FOUNDATION
#endif

#ifdef DX_NON_INPUT
	#ifndef DX_NON_KEYEX
		#define DX_NON_KEYEX
	#endif
#endif

#ifdef DX_NON_FONT
	#ifndef DX_NON_PRINTF_DX
		#define DX_NON_PRINTF_DX
	#endif
	#ifndef DX_NON_KEYEX
		#define DX_NON_KEYEX
	#endif
#endif

#ifdef DX_NON_OGGVORBIS
	#ifndef DX_NON_OGGTHEORA
		#define DX_NON_OGGTHEORA
	#endif
	#ifndef DX_NON_OPUS
		#define DX_NON_OPUS
	#endif
#endif


#if defined( _WIN64 ) || defined( __LP64__ )
	#ifndef __64BIT__
		#define __64BIT__
	#endif
#endif



#if defined( _WIN64 ) || defined( __ANDROID__ ) || defined( __APPLE__ )
	#ifndef DX_NON_INLINE_ASM
		#define DX_NON_INLINE_ASM
	#endif
#endif




#include "DxDataType.h"

#endif // __DXCOMPILECONFIG_H__
