// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		標準Ｃライブラリ使用プログラムヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXUSECLIB_H__
#define __DXUSECLIB_H__

// インクルード ------------------------------------------------------------------
#include "DxCompileConfig.h"

#ifndef DX_NON_OGGTHEORA
#include "DxMovie.h"
#endif

#if !defined( DX_NON_OPUS ) || !defined( DX_NON_OGGVORBIS ) || !defined( DX_NON_MOVIE )
#include "DxSoundConvert.h"
#endif

#include "DxFile.h"
#include "DxBaseFunc.h"

//namespace DxLib
//{

// マクロ定義 --------------------------------------------------------------------

// 構造体定義 --------------------------------------------------------------------

// テーブル-----------------------------------------------------------------------

// 内部大域変数宣言 --------------------------------------------------------------

// 関数プロトタイプ宣言-----------------------------------------------------------










#ifndef DX_NON_MERSENNE_TWISTER

// Copyright SYN
extern unsigned long	CheckMMX(void) ;
extern void				srandMT(unsigned int seed) ;
extern void				generateMT(void) ;
extern unsigned long	randMT(void) ;

#endif // DX_NON_MERSENNE_TWISTER






#ifndef DX_NON_TIFFREAD
extern	int		LoadTiffImage( STREAMDATA *Src, BASEIMAGE *BaseImage, int GetFormatOnly ) ;						// ＴＩＦＦ画像の読みこみ
#endif
#ifndef DX_NON_PNGREAD
extern	int		LoadPngImage(     STREAMDATA *Src, BASEIMAGE *BaseImage, int GetFormatOnly ) ;					// ＰＮＧ画像の読みこみ
#ifndef DX_NON_SAVEFUNCTION
extern	int		SaveBaseImageToPngBase( const char *FilePathW, const char *FilePathA, BASEIMAGE *BaseImage, int CompressionLevel ) ;																					// 基本画像データをＰＮＧ画像として保存する
#endif
#endif
#ifndef DX_NON_JPEGREAD
extern	int		LoadJpegImageBase( STREAMDATA *Src, BASEIMAGE *BaseImage, int GetFormatOnly, int GetExifOnly, void *ExifBuffer, size_t ExifBufferSize ) ;	// ＪＰＥＧ画像の読みこみ (実体は DxUseCLib.cpp の中)
extern	int		LoadJpegImage( STREAMDATA *Src, BASEIMAGE *BaseImage, int GetFormatOnly ) ;						// ＪＰＥＧ画像の読みこみ (実体は DxUseCLib.cpp の中)
#ifndef DX_NON_SAVEFUNCTION
extern	int		SaveBaseImageToJpegBase( const char *FilePathW, const char *FilePathA, BASEIMAGE *BaseImage, int Quality, int Sample2x1 ) ;																			// 基本画像データをＪＰＥＧ画像として保存する
#endif
#endif




extern	int			INT64DIV(  const BYTE *Int64,  int   DivNum ) ;
extern	DWORD		UINT64DIV( const BYTE *UInt64, DWORD DivNum ) ;
extern	int			INT64MOD(  const BYTE *Int64,  int   ModNum ) ;
extern	DWORD		UINT64MOD( const BYTE *UInt64, DWORD ModNum ) ;






#ifndef DX_NON_MOVIE
#ifndef DX_NON_DSHOW_MOVIE
#ifndef DX_NON_DSHOW_MP3
extern	int SetupSoundConvert_DSMP3(            SOUNDCONV *SoundConv ) ;							// DirectShowを使ってMP3ファイルをコンバートするセットアップ処理を行う( [戻] -1:エラー )
#endif
#endif
#endif




#ifndef DX_NON_OGGVORBIS
extern	int SetupSoundConvert_OGG(            SOUNDCONV *SoundConv ) ;								// ＯＧＧファイルのセットアップ処理を行う( [戻] -1:エラー )
extern	int TerminateSoundConvert_OGG(        SOUNDCONV *SoundConv ) ;								// ＯＧＧファイルの後始末処理を行う
extern	int ConvertProcessSoundConvert_OGG(   SOUNDCONV *SoundConv ) ;								// 変換後のバッファにデータを補充する
//extern	int SetTimeSoundConvert_OGG(      SOUNDCONV *SoundConv, int Time ) ;					// 変換処理を位置を変更する( ミリ秒単位 )
extern	int SetSampleTimeSoundConvert_OGG(    SOUNDCONV *SoundConv, int SampleTime ) ;				// 変換処理の位置を変更する( サンプル単位 )
extern	int GetSoundConvertDestSize_Fast_OGG( SOUNDCONV *SoundConv ) ;								// 変換後の大凡のデータサイズを得る
extern	int GetSoundConvertLoopAreaInfo_OGG(  SOUNDCONV *SoundConv, int *LoopStartPos, int *LoopEndPos ) ;	// ループ情報を取得( [戻] -1:エラー )
#endif






#ifndef DX_NON_OPUS
extern	int SetupSoundConvert_OPUS(            SOUNDCONV *SoundConv ) ;								// Opusファイルのセットアップ処理を行う( [戻] -1:エラー )
extern	int TerminateSoundConvert_OPUS(        SOUNDCONV *SoundConv ) ;								// Opusファイルの後始末処理を行う
extern	int ConvertProcessSoundConvert_OPUS(   SOUNDCONV *SoundConv ) ;								// 変換後のバッファにデータを補充する
extern	int SetSampleTimeSoundConvert_OPUS(    SOUNDCONV *SoundConv, int SampleTime ) ;				// 変換処理の位置を変更する( サンプル単位 )
extern	int GetSoundConvertDestSize_Fast_OPUS( SOUNDCONV *SoundConv ) ;								// 変換後の大凡のデータサイズを得る
#endif






#ifndef DX_NON_OGGTHEORA

extern int	TheoraDecode_GrobalInitialize( void ) ;																					// ソフト的に一度だけ呼ぶべき初期化関数

// Ogg Theora 読み込み処理の準備を行う( 戻り値  0:失敗  1以上:初期化成功 )
extern DWORD_PTR  TheoraDecode_InitializeStream(
	STREAMDATASHRED *	StreamShred,
	DWORD_PTR			StreamData,
	int					StockFrameNum /* = 10 */,
	int					NotUseYUVFormatSurface /* = FALSE */,
	int					NotUseYUVGrHandle /* = TRUE */,
	int					ASyncTrhead /* = FALSE */
) ;
extern int	TheoraDecode_Terminate( DWORD_PTR Handle ) ;																			// Ogg Theora 読み込み処理の後始末を行う
extern int	TheoraDecode_SurfaceTerminate( DWORD_PTR Handle ) ;																		// Ogg Theora で使用しているサーフェスを解放する

extern int	TheoraDecode_GetInfo( DWORD_PTR Handle, THEORADECODE_INFO *Info ) ;														// 動画の情報を取得する
extern int	TheoraDecode_GetCurrentFrame( DWORD_PTR Handle ) ;																		// カレントフレームを取得する
	
extern int	TheoraDecode_IncToFrame( DWORD_PTR Handle, int AddNum = 1 ) ;															// カレントフレームを１フレーム分進める( 戻り値  0:キーフレームじゃなかった  1:キーフレーム  -1:エラー )
extern int	TheoraDecode_SeekToFrame( DWORD_PTR Handle, int Frame ) ;																// カレントフレームを指定のフレームに移動する
extern int	TheoraDecode_SeekToTime( DWORD_PTR Handle, LONGLONG Time ) ;															// カレントフレームを指定の再生時間に移動する( 単位はマイクロ秒 )

extern int	TheoraDecode_SetupImage( DWORD_PTR Handle, int BaseImage = 0, int YUVGrHandle = 0, int YUVImage = 0, int ASyncThread = FALSE ) ;				// カレントフレームのRGB画像を作成する( 戻り値  1:作成された  0:されなかった )
extern const BASEIMAGE *	TheoraDecode_GetBaseImage( DWORD_PTR Handle ) ;															// 一時バッファのアドレスを取得する
extern const void *			TheoraDecode_GetYUVImage( DWORD_PTR Handle ) ;															// 一時バッファの YUV フォーマットのサーフェスを得る
extern int	TheoraDecode_GetYGrHandle( DWORD_PTR Handle ) ;																			// YUVカラーの Y成分のグラフィックハンドルを取得する
extern int	TheoraDecode_GetUVGrHandle( DWORD_PTR Handle ) ;																		// YUVカラーの UV成分のグラフィックハンドルを取得する

extern int	TheoraDecode_IsIdle( DWORD_PTR Handle ) ;																				// デコードスレッドがアイドル状態かどうかを調べる( 戻り値  1:アイドル状態  0:稼動中 )

#endif






//}

#endif // __DXUSECLIB_H__
