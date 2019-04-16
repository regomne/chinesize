// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		WindowsOS用動画プログラムヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXMOVIEWIN_H__
#define __DXMOVIEWIN_H__

#include "../DxCompileConfig.h"

#ifndef DX_NON_MOVIE

// インクルード ------------------------------------------------------------------
#include "DxUseCStrmBaseFilter.h"
#include "DxGuid.h"
#include "../DxLib.h"
#include "../DxFile.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

// 構造体定義 --------------------------------------------------------------------

// ムービーグラフィック環境依存データ型
struct MOVIEGRAPH_PF
{
	int						Dummy ;

#ifndef DX_NON_MEDIA_FOUNDATION
	D_IMFSourceReader		*pMFReader ;					// ソースリーダー
	D_IMFMediaType			*pMFMediaTypeVideoStream ;		// 変換前のビデオストリームメディア属性
	D_IMFMediaType			*pMFMediaTypeOutputVideoStream ;// 変換後のビデオストリームメディア属性
	UINT32					MFFrameSizeX ;					// 幅
	UINT32					MFFrameSizeY ;					// 高さ
	UINT32					MFFrameRateNumerator ;			// フレームレート( 分子 )
	UINT32					MFFrameRateDenominator ;		// フレームレート( 分母 )
	UINT32					MFAspectRatioX ;				// アスペクトX
	UINT32					MFAspectRatioY ;				// アスペクトY
	LONGLONG				MFLastReadSampleTimeStamp ;		// 最後に行った ReadSample のタイムスタンプ
	int						MFLastReadSampleFrame ;			// 最後に行った ReadSample のフレーム
	D_PROPVARIANT			MFDuration ;					// 動画の長さ
	int						MFTotalFrame ;					// 総フレーム数
	void *					MFImageBuffer ;					// イメージ保存用バッファ

	int						MFCurrentFrame ;				// 表示しているフレーム
	LONGLONG				MFPrevTimeCount ;				// 前回の計測時間
	LONGLONG				MFPlayNowTime ;					// 再生時間
	int						MFLoopType ;					// ループタイプ( 0:動画データに合わせてループ  1:サウンドデータに合わせてループ )
	int						MFSetupGraphHandleImage ;		// 画像構築の後、グラフィックハンドルのセットアップを終えているかどうか( TRUE:終えている  FALSE:終えていない )
#ifndef DX_NON_SOUND
	int						MFSoundHandle ;					// サウンドハンドル
	int						MFSoundFrequency ;				// サウンドの周波数
	int						MFSoundTotalTime ;				// サウンドデータの再生総時間( ミリ秒 )
#endif // DX_NON_SOUND
	double					MFPlaySpeedRate ;				// 再生速度

	void					*MFYBuffer ;					// Ｙイメージへのアドレス
	void					*MFUVBuffer ;					// ＵＶイメージへのアドレス
	UINT32					MFYWidth, MFYHeight ;			// Ｙイメージの幅と高さ
	UINT32					MFYStride ;						// Ｙバッファのピッチ
	UINT32					MFUVWidth, MFUVHeight ;			// ＵＶイメージの幅と高さ
	UINT32					MFUVStride ;					// ＵＶバッファのピッチ

//	BASEIMAGE				BaseImage ;						// カレントフレームが格納されたフレームスタック中のイメージのコピー
	volatile int			MFBaseImageSetup ;				// カレントフレームの RGB イメージが構築されているかどうか( 1:されている  0:されていない )
#ifndef DX_NON_FILTER
	volatile int			MFNotUseYUVGrHandle ;			// YUVカラーのグラフィックハンドルを使用しないかどうか
	volatile int			MFYUVGrHandleSetup ;			// YUVカラーのグラフィックハンドルのセットアップが完了しているかどうか( 1:完了している  0:完了していない )
#endif // DX_NON_FILTER
#endif // DX_NON_MEDIA_FOUNDATION

#ifndef DX_NON_DSHOW_MOVIE
	D_IGraphBuilder			*pGraph ;						// フィルタグラフマネージャ
	D_IMediaControl			*pMediaControl ;				// メディアコントローラ
	D_IMediaSeeking			*pMediaSeeking ;				// メディアシーキング
	D_IBasicAudio			*pBasicAudio ;					// BasicAudio インターフェイス

//	D_ISampleGrabber		*SampleGrabber ;				// サンプルグラッバオブジェクト

	D_CMovieRender			*pMovieImage ;					// 動画のイメージ
#endif // DX_NON_DSHOW_MOVIE

#if !defined( DX_NON_MEDIA_FOUNDATION ) || !defined( DX_NON_DSHOW_MOVIE )
	D_STREAM_TIME			FrameTime ;						// １フレーム当たりの時間
	int						UseTemporaryFile ;				// テンポラリファイルを使用しているかどうか、フラグ
	wchar_t					FileName[ FILEPATH_MAX ] ;		// ファイルネーム
#endif // !defined( DX_NON_MEDIA_FOUNDATION ) || !defined( DX_NON_DSHOW_MOVIE )
} ;

// ムービーデータ環境依存管理構造体
struct MOVIEGRAPHMANAGE_PF
{
	int						Dummy ;

#ifndef DX_NON_MEDIA_FOUNDATION
	int						DisableMediaFoundation ;		// Media Foundation を使用しないかどうか( TRUE:使用しない  FALSE:使用する )
	int						MFStartupRunFlag ;				// MFStartup を呼んでいるか( TRUE:呼んだ  FALSE:まだ呼んでいない )
#endif // DX_NON_MEDIA_FOUNDATION
} ;

// 内部大域変数宣言 --------------------------------------------------------------

// 関数プロトタイプ宣言-----------------------------------------------------------

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_NON_MOVIE

#endif // __DXMOVIEWIN_H__
