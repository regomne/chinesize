// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		動画プログラムヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXMOVIE_H__
#define __DXMOVIE_H__

#include "DxCompileConfig.h"

#ifndef DX_NON_MOVIE

// インクルード ------------------------------------------------------------------
#include "DxHandle.h"
#include "DxBaseImage.h"
#include "DxFile.h"

#ifdef __WINDOWS__
	#include "Windows/DxMovieWin.h"
#endif // __WINDOWS__

#ifdef __ANDROID__
	#include "Android/DxMovieAndroid.h"
#endif // __ANDROID__

#ifdef __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE
		#include "iOS/DxMovieiOS.h"
    #endif // TARGET_OS_IPHONE
#endif // __APPLE__




#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

// 構造体定義 --------------------------------------------------------------------

// ムービーファイルのオープンに必要なグローバルデータを纏めたもの
struct OPENMOVIE_GPARAM
{
	int							RightAlphaFlag;					// 動画像の右側をアルファとみなす動画ハンドルを作成するか(TRUE:作成する)
	int							A8R8G8B8Flag ;					// 32bitカラーフォーマットの動画を A8R8G8B8 形式として扱うかどうかのフラグ
	int							NotUseYUVFormatSurfaceFlag ;	// ＹＵＶフォーマットのサーフェスを使用しないかどうか
} ;

// Ogg Theora のデコード時の情報
struct THEORADECODE_INFO
{
	double						FrameRate ;						// １秒間辺りのコマ数
	int							TotalFrame ;					// 総フレーム数
	int							Width, Height ;					// 幅と高さ
} ;

// ムービーグラフィックデータ型
struct MOVIEGRAPH
{
	HANDLEINFO				HandleInfo ;					// ハンドル共通データ

	int						ParentGrHandle ;				// このムービーグラフィックハンドルを使用しているグラフィックハンドル

	int						PlayType ;						// 再生タイプ

#ifndef DX_NON_OGGTHEORA
	int						TheoraFlag ;					// Theora を使用しているかどうかのフラグ( 1:使用している  0:使用していない )
	DWORD_PTR				TheoraHandle ;					// Theora ファイルハンドル
//	LONGLONG				TheoraPlayTime ;				// 再生開始時間
	LONGLONG				TheoraPrevTimeCount ;			// 前回の計測時間
	LONGLONG				TheoraPlayNowTime ;				// 再生時間
	int						TheoraTotalPlayTime ;			// 再生総時間( ミリ秒 )
	DWORD_PTR				TheoraStreamData ;				// ストリーム処理用データ
	int						TheoraStreamDataIsFileImage ;	// ストリーム処理用データがファイルをメモリに読み込んだものかどうか
	double					TheoraFrameRate ;				// フレームレート
	int						TheoraLoopType ;				// ループタイプ( 0:動画データに合わせてループ  1:サウンドデータに合わせてループ )
	int						TheoraSetupGraphHandleImage ;	// 画像構築の後、グラフィックハンドルのセットアップを終えているかどうか( TRUE:終えている  FALSE:終えていない )
#ifndef DX_NON_SOUND
	int						TheoraVorbisHandle ;			// Vorbis用サウンドハンドル
	int						TheoraVorbisFrequency ;			// Vorbis用サウンドの周波数
	int						TheoraVorbisTotalTime ;			// Vorbisサウンドデータの再生総時間( ミリ秒 )
#endif // DX_NON_SOUND
	double					TheoraPlaySpeedRate ;			// Theora用再生速度
#endif // DX_NON_OGGTHEORA

	RECT					SrcRect ;						// ブロック転送操作の転送元矩形

	LONGLONG				RefreshFrame ;					// 前回更新したフレーム
	LONGLONG				RefreshTime ;					// 前回更新した時間

	LONGLONG				StopTime ;						// 終了タイム
	int						SufIsSystemMemory ;				// サーフェスがシステムメモリ上にある場合ＴＲＵＥ
	int						Width, Height ;					// ムービーグラフィックのサイズ
	int						RightAlpha ;					// ムービーの右側をアルファチャンネルとして扱うか(TRUE:扱う)
	int						A8R8G8B8Flag ;					// 32bitカラーフォーマットの動画を A8R8G8B8 形式として扱うかどうかのフラグ
	int						NotUseYUVFormatSurfaceFlag ;	// ＹＵＶフォーマットのサーフェスを使用しないかどうか
	int						PlayFlag ;						// 再生中フラグ
	int						SysPauseFlag ;					// 内部一時停止フラグ
	int						FirstUpdateFlag ;				// 最初のアップデートが行われたかどうか( TRUE:行われた  FALSE:まだ )

	int						YUVFlag ;						// ＹＵＶ形式を使用しているかどうか( TRUE:している  FALSE:していない )
	int						SurfaceMode ;					// 使用しているサーフェスタイプ

	BASEIMAGE				NowImage ;
	int						NowImageGraphOutAlloc ;			// NowImage の GraphData が外部で確保されたバッファを使用しているかどうか
	int						NowImageUpdateFlag ;			// NowImage の内容が更新されたかどうかのフラグ( GetMovieBaseImageToGraph 用 )
	BASEIMAGE				*UseNowImage ;					// Theora 内の BASEIMAGE を使用している可能性もあるので外部のプログラムはこちらを使うべし
#ifndef DX_NON_FILTER
	int						YGrHandle ;						// YUVカラーのY成分のみのグラフィックハンドル
	int						UVGrHandle ;					// YUVカラーのUV成分のみのグラフィックハンドル
#endif // DX_NON_FILTER
	int						OverlayDestX, OverlayDestY ;	// 最後に設定されたオーバーレイサーフェスの出力位置
	int						OverlayDestExRate ;				// 最後に設定されたオーバーレイサーフェスの出力倍率
	int						OverlayDispFlag ;				// 最後に設定されたオーバーレイサーフェスの表示フラグ
	RECT					OverlaySrcRect, OverlayDestRect ;// 最後にセットされたオーバーレイソース矩形と出力矩形

	void					( *UpdateFunction )( struct MOVIEGRAPH *Movie, void *Data ) ;		// 動画更新時に呼ぶコールバック関数
	void					*UpdateFunctionData ;												// コールバック関数に渡すポインタ

	MOVIEGRAPH_PF			PF ;							// 環境依存情報
} ;

// ムービーデータ管理構造体
struct MOVIEGRAPHMANAGE
{
	int						InitializeFlag ;				// 初期化フラグ

	int						RightAlphaFlag ;				// 動画像の右側をアルファとみなす動画ハンドルを作成するか(TRUE:作成する)
	int						A8R8G8B8Flag ;					// 32bitカラーフォーマットを A8R8G8B8 形式として扱うかどうかのフラグ
	int						NotUseYUVFormatSurfaceFlag ;	// ＹＵＶフォーマットのサーフェスを使用しないかどうか

	MOVIEGRAPHMANAGE_PF		PF ;							// 環境依存情報
} ;

// テーブル-----------------------------------------------------------------------

// 内部大域変数宣言 --------------------------------------------------------------

extern MOVIEGRAPHMANAGE MovieGraphManageData ;								// 動画関連データ

// 関数プロトタイプ宣言-----------------------------------------------------------

extern	int		InitializeMovieManage( void ) ;																		// ムービー関連の管理処理の初期化
extern	int		TerminateMovieManage( void ) ;																		// ムービー関連の管理処理の後始末

// OpenMovie のグローバル変数にアクセスしないバージョン
extern	int		OpenMovie_UseGParam(
					int GrHandle,
					OPENMOVIE_GPARAM *GParam,
					const wchar_t *FileName,
					const void *FileImage,
					size_t FileImageSize,
					int *Width,
					int *Height,
					int SurfaceMode,
					int ImageSizeGetOnly = FALSE,
					int ASyncThread = FALSE ) ;

extern	int		OpenMovie( const wchar_t *FileName, int *Width, int *Height, int SurfaceMode ) ;						// ムービーを開く
extern	int		CloseMovie( int MovieHandle ) ;																		// ムービーを閉じる
extern 	int		PlayMovie_( int MovieHandle, int PlayType = DX_PLAYTYPE_BACK, int SysPlay = 0 ) ;					// ムービーの再生を開始する
extern 	int		PauseMovie( int MovieHandle, int SysPause = 0 ) ;													// ムービーの再生をストップする
extern	int		AddMovieFrame( int MovieHandle, unsigned int FrameNum ) ;											// ムービーのフレームを進める、戻すことは出来ない( ムービーが停止状態で、且つ Ogg Theora のみ有効 )
extern	int		SeekMovie( int MovieHandle, int Time ) ;															// ムービーの再生位置を設定する(ミリ秒単位)
extern	int		SetPlaySpeedRateMovie( int MovieHandle, double SpeedRate ) ;										// ムービーの再生速度を設定する( 1.0 = 等倍速  2.0 = ２倍速 )、一部のファイルフォーマットのみで有効な機能です
extern 	int		GetMovieState( int MovieHandle ) ;																	// ムービーの再生状態を得る
extern	int		SetMovieVolume( int Volume, int MovieHandle ) ;														// ムービーのボリュームをセットする(0〜10000)
extern	BASEIMAGE *GetMovieBaseImage( int MovieHandle, int *ImageUpdateFlag, int ImageUpdateFlagSetOnly ) ;			// ムービーの基本イメージデータを取得する
extern	int		GetMovieTotalFrame( int MovieHandle ) ;																// ムービーの総フレーム数を得る( Ogg Theora でのみ有効 )
extern	int		TellMovie( int MovieHandle ) ;																		// ムービーの再生位置を取得する(ミリ秒単位)
extern	int		TellMovieToFrame( int MovieHandle ) ;																// ムービーの再生位置を取得する(フレーム単位)
extern	int		SeekMovieToFrame( int MovieHandle, int Frame ) ;													// ムービーの再生位置を設定する(フレーム単位)
extern	LONGLONG GetOneFrameTimeMovie( int MovieHandle ) ;															// ムービーの１フレームあたりの時間を得る
extern	MOVIEGRAPH *GetMovieData( int MovieHandle ) ;																// ムービーグラフィックのデータを取得する

extern	int		SetCallbackMovie( int MovieHandle, void ( *Callback )( MOVIEGRAPH *Movie, void *Data ), void *Data );	// ムービーの更新時に呼び出すコールバック関数を登録する
extern	int		UpdateMovie( int MovieHandle, int AlwaysFlag = FALSE ) ;											// ムービーの更新を行う
extern	int		ReleaseMovieSurface( int MovieHandle ) ;															// ムービーで使用しているサーフェスの解放を行う

extern	int		DisableMovieAll( void ) ;																			// ムービーの再生状態を停止する
extern	int		RestoreMovieAll( void ) ;																			// ムービーの再生状態を再生する

extern 	int		PlayMovieAll( void ) ;																				// すべてのムービーグラフィックをスタート
extern 	int		PauseMovieAll( void ) ;																				// すべてのムービーグラフィックをストップ

extern	int		InitializeMovieHandle( HANDLEINFO *HandleInfo ) ;													// ムービーハンドルを初期化をする関数
extern	int		TerminateMovieHandle( HANDLEINFO *HandleInfo ) ;													// ムービーハンドルの後始末を行う関数


// 環境依存関数

extern	int		InitializeMovieManage_PF( void ) ;																	// ムービー関連の管理処理の初期化の環境依存処理
extern	int		TerminateMovieManage_PF( void ) ;																	// ムービー関連の管理処理の後始末の環境依存処理

extern	int		TerminateMovieHandle_PF( HANDLEINFO *HandleInfo ) ;													// ムービーハンドルの後始末を行う
extern	int		OpenMovie_UseGParam_PF( MOVIEGRAPH * Movie, OPENMOVIE_GPARAM *GParam, const wchar_t *FileName, int *Width, int *Height, int SurfaceMode, int ImageSizeGetOnly = FALSE, int ASyncThread = FALSE ) ;	// OpenMovie のグローバル変数にアクセスしないバージョンの環境依存処理
extern	int		OpenMovie_CustomGraphHandleGParam_PF( MOVIEGRAPH * Movie, struct SETUP_GRAPHHANDLE_GPARAM *InitGraphHandleGParam ) ;	// ムービー用のグラフィックハンドル作成用のパラメータをカスタマイズする
extern 	int		PlayMovie__PF( MOVIEGRAPH * Movie, int PlayType = DX_PLAYTYPE_BACK, int SysPlay = 0 ) ;				// ムービーの再生を開始する処理の環境依存処理
extern 	int		PauseMovie_PF( MOVIEGRAPH * Movie, int SysPause = 0 ) ;												// ムービーの再生をストップする処理の環境依存処理
extern	int		SeekMovie_PF( MOVIEGRAPH * Movie, int Time ) ;														// ムービーの再生位置を設定する(ミリ秒単位)処理の環境依存処理
extern	int		SetPlaySpeedRateMovie_PF( MOVIEGRAPH * Movie, double SpeedRate ) ;									// ムービーの再生速度を設定する( 1.0 = 等倍速  2.0 = ２倍速 )処理の環境依存処理
extern 	int		GetMovieState_PF( MOVIEGRAPH * Movie ) ;															// ムービーの再生状態を得る処理の環境依存処理
extern	int		SetMovieVolume_PF( MOVIEGRAPH * Movie, int Volume ) ;												// ムービーのボリュームをセットする(0〜10000)処理の環境依存処理
extern	BASEIMAGE *GetMovieBaseImage_PF( MOVIEGRAPH * Movie, int *ImageUpdateFlag, int ImageUpdateFlagSetOnly ) ;		// ムービーの基本イメージデータを取得する処理の環境依存処理
extern	int		GetMovieTotalFrame_PF( MOVIEGRAPH * Movie ) ;														// ムービーの総フレーム数を得る
extern	int		TellMovie_PF( MOVIEGRAPH * Movie ) ;																// ムービーの再生位置を取得する(ミリ秒単位)処理の環境依存処理
extern	int		TellMovieToFrame_PF( MOVIEGRAPH * Movie ) ;															// ムービーの再生位置を取得する(フレーム単位)処理の環境依存処理
extern	int		SeekMovieToFrame_PF( MOVIEGRAPH * Movie, int Frame ) ;												// ムービーの再生位置を設定する(フレーム単位)処理の環境依存処理
extern	LONGLONG GetOneFrameTimeMovie_PF( MOVIEGRAPH * Movie ) ;													// ムービーの１フレームあたりの時間を得る処理の環境依存処理

extern	int		UpdateMovie_PF( MOVIEGRAPH * Movie, int AlwaysFlag = FALSE ) ;										// ムービーの更新を行う処理の環境依存処理



#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_NON_MOVIE

#endif // __DXMOVIE_H__
