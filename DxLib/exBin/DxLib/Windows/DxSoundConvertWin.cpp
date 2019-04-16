//-----------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		WindowsOS用サウンドデータ変換プログラム
// 
//  	Ver 3.20c
// 
//-----------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

#include "../DxCompileConfig.h"

#ifndef DX_NON_SOUND

// インクルード----------------------------------------------------------------
#include "DxSoundConvertWin.h"
#include "DxWinAPI.h"
#include "DxGuid.h"
#include "../DxSoundConvert.h"
#include "../DxSystem.h"
#include "../DxMovie.h"
#include "../DxUseCLib.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義------------------------------------------------------------------

// 型定義----------------------------------------------------------------------

// データ宣言------------------------------------------------------------------

SOUNDCONVERTDATA_WIN GSoundConvertData_Win ;

// 関数プロトタイプ宣言 -------------------------------------------------------

// プログラム------------------------------------------------------------------

// 初期化・終了関数

// サウンドデータ変換処理の環境依存の初期化を行う
extern	int InitializeSoundConvert_PF( void )
{
#ifndef DX_NON_ACM
	// MSACM32DLL をロードする
	if( GSoundConvertData_Win.msacm32DLL == NULL )
	{
		GSoundConvertData_Win.msacm32DLL = LoadLibrary( _T( "msacm32.dll" ) ) ;
		if( GSoundConvertData_Win.msacm32DLL == NULL )
			return -1 ;

		GSoundConvertData_Win.acmFormatSuggestFunc = ( MMRESULT ( WINAPI * )( HACMDRIVER, LPWAVEFORMATEX, LPWAVEFORMATEX, DWORD, DWORD ) )GetProcAddress( GSoundConvertData_Win.msacm32DLL, "acmFormatSuggest" ) ;
		if( GSoundConvertData_Win.acmFormatSuggestFunc == NULL )
			return -1 ;

		GSoundConvertData_Win.acmStreamOpenFunc = ( MMRESULT ( WINAPI * )( D_LPHACMSTREAM, HACMDRIVER, LPWAVEFORMATEX, LPWAVEFORMATEX, D_LPWAVEFILTER, DWORD, DWORD, DWORD ) )GetProcAddress( GSoundConvertData_Win.msacm32DLL, "acmStreamOpen" ) ;
		if( GSoundConvertData_Win.acmStreamOpenFunc == NULL )
			return -1 ;

		GSoundConvertData_Win.acmStreamCloseFunc = ( MMRESULT ( WINAPI * )( D_HACMSTREAM, DWORD ) )GetProcAddress( GSoundConvertData_Win.msacm32DLL, "acmStreamClose" ) ;
		if( GSoundConvertData_Win.acmStreamCloseFunc == NULL )
			return -1 ;

		GSoundConvertData_Win.acmMetricsFunc = ( MMRESULT ( WINAPI * )( D_HACMOBJ, UINT, LPVOID ) )GetProcAddress( GSoundConvertData_Win.msacm32DLL, "acmMetrics" ) ;
		if( GSoundConvertData_Win.acmMetricsFunc == NULL )
			return -1 ;

		GSoundConvertData_Win.acmStreamPrepareHeaderFunc = ( MMRESULT ( WINAPI * )( D_HACMSTREAM, D_LPACMSTREAMHEADER, DWORD ) )GetProcAddress( GSoundConvertData_Win.msacm32DLL, "acmStreamPrepareHeader" ) ;
		if( GSoundConvertData_Win.acmStreamPrepareHeaderFunc == NULL )
			return -1 ;

		GSoundConvertData_Win.acmStreamConvertFunc = ( MMRESULT ( WINAPI * )( D_HACMSTREAM, D_LPACMSTREAMHEADER, DWORD ) )GetProcAddress( GSoundConvertData_Win.msacm32DLL, "acmStreamConvert" ) ;
		if( GSoundConvertData_Win.acmStreamConvertFunc == NULL )
			return -1 ;

		GSoundConvertData_Win.acmStreamUnprepareHeaderFunc = ( MMRESULT ( WINAPI * )( D_HACMSTREAM, D_LPACMSTREAMHEADER, DWORD ) )GetProcAddress( GSoundConvertData_Win.msacm32DLL, "acmStreamUnprepareHeader" ) ;
		if( GSoundConvertData_Win.acmStreamUnprepareHeaderFunc == NULL )
			return -1 ;

		GSoundConvertData_Win.acmStreamSizeFunc = ( MMRESULT  ( WINAPI * )( D_HACMSTREAM, DWORD, LPDWORD, DWORD ) )GetProcAddress( GSoundConvertData_Win.msacm32DLL, "acmStreamSize" ) ;
		if( GSoundConvertData_Win.acmStreamSizeFunc == NULL )
			return -1 ;
	}
#endif // DX_NON_ACM

	// 正常終了
	return 0 ;
}

// サウンドデータ変換処理の環境依存の終了処理を行う
extern	int TerminateSoundConvert_PF( void )
{
#ifndef DX_NON_ACM
	// msacm32.DLL の解放
	if( GSoundConvertData_Win.msacm32DLL )
	{
		FreeLibrary( GSoundConvertData_Win.msacm32DLL ) ;
		GSoundConvertData_Win.msacm32DLL = NULL ;
		GSoundConvertData_Win.acmFormatSuggestFunc = NULL ;
		GSoundConvertData_Win.acmStreamOpenFunc = NULL ;
		GSoundConvertData_Win.acmStreamCloseFunc = NULL ;
		GSoundConvertData_Win.acmMetricsFunc = NULL ;
		GSoundConvertData_Win.acmStreamPrepareHeaderFunc = NULL ;
		GSoundConvertData_Win.acmStreamConvertFunc = NULL ;
		GSoundConvertData_Win.acmStreamUnprepareHeaderFunc = NULL ;
		GSoundConvertData_Win.acmStreamSizeFunc = NULL ;
	}
#endif // DX_NON_ACM

	// 正常終了
	return 0 ;
}



// (環境依存処理)変換処理のセットアップ( [戻] -1:エラー )
extern	int SetupSoundConvert_PF( SOUNDCONV *SoundConv, STREAMDATA *Stream, int DisableReadSoundFunctionMask )
{

#ifndef DX_NON_ACM
	if( ( DisableReadSoundFunctionMask & DX_READSOUNDFUNCTION_ACM ) == 0 )
	{
		if( SetupSoundConvert_ACM( SoundConv ) == 0 ) return 0 ;
		Stream->ReadShred.Seek( Stream->DataPoint, 0, SEEK_SET ) ;
	}
	if( ( DisableReadSoundFunctionMask & DX_READSOUNDFUNCTION_MP3 ) == 0 )
	{
		if( SetupSoundConvert_MP3( SoundConv ) == 0 ) return 0 ;
	}
#endif // DX_NON_ACM
#ifndef DX_NON_MOVIE
#ifndef DX_NON_DSHOW_MOVIE
#ifndef DX_NON_DSHOW_MP3
	if( ( DisableReadSoundFunctionMask & DX_READSOUNDFUNCTION_DSMP3 ) == 0 )
	{
		if( SetupSoundConvert_DSMP3( SoundConv ) == 0 ) return 0 ;
	}
#endif // DX_NON_DSHOW_MP3
#endif // DX_NON_DSHOW_MOVIE
#endif // DX_NON_MOVIE
#ifndef DX_NON_MEDIA_FOUNDATION
	if( ( DisableReadSoundFunctionMask & DX_READSOUNDFUNCTION_MF ) == 0 )
	{
		if( SetupSoundConvert_MF( SoundConv ) == 0 ) return 0 ;
		Stream->ReadShred.Seek( Stream->DataPoint, 0, SEEK_SET ) ;
	}
#endif // DX_NON_MEDIA_FOUNDATION
	return -1 ;
}

// (環境依存処理)変換処理の位置を変更する( サンプル単位 )
extern	int SetSampleTimeSoundConvert_PF(    SOUNDCONV *SoundConv, int SampleTime )
{
	int res = -1 ;

	switch( SoundConv->MethodType )
	{
#ifndef DX_NON_ACM
	case SOUND_METHODTYPE_ACM : res = SetSampleTimeSoundConvert_ACM( SoundConv, SampleTime ) ; break ;
#endif
#ifndef DX_NON_MOVIE
#ifndef DX_NON_DSHOW_MOVIE
#ifndef DX_NON_DSHOW_MP3
	case SOUND_METHODTYPE_DSMP3 : res = SetSampleTimeSoundConvert_DSMP3( SoundConv, SampleTime ) ; break ;
#endif
#endif
#endif

#ifndef DX_NON_MEDIA_FOUNDATION
	case SOUND_METHODTYPE_MF : return SetSampleTimeSoundConvert_MF( SoundConv, SampleTime ) ;
#endif
	}

	return res ;
}

// (環境依存処理)変換後のバッファにデータを補充する
extern	int ConvertProcessSoundConvert_PF(  SOUNDCONV *SoundConv )
{
	int res = -1 ;

	switch( SoundConv->MethodType )
	{
#ifndef DX_NON_ACM
	case SOUND_METHODTYPE_ACM : res = ConvertProcessSoundConvert_ACM( SoundConv ) ; break ;
#endif
#ifndef DX_NON_MOVIE
#ifndef DX_NON_DSHOW_MOVIE
#ifndef DX_NON_DSHOW_MP3
	case SOUND_METHODTYPE_DSMP3 : res = ConvertProcessSoundConvert_DSMP3( SoundConv ) ; break ;
#endif
#endif
#endif

#ifndef DX_NON_MEDIA_FOUNDATION
	case SOUND_METHODTYPE_MF : return ConvertProcessSoundConvert_MF( SoundConv ) ;
#endif
	}

	return res ;
}

// (環境依存処理)変換処理の後始末を行う
extern	int TerminateSoundConvert_PF(        SOUNDCONV *SoundConv )
{
	switch( SoundConv->MethodType )
	{
#ifndef DX_NON_ACM
	case SOUND_METHODTYPE_ACM : TerminateSoundConvert_ACM( SoundConv ) ; break ;
#endif
#ifndef DX_NON_MOVIE
#ifndef DX_NON_DSHOW_MOVIE
#ifndef DX_NON_DSHOW_MP3
	case SOUND_METHODTYPE_DSMP3 : TerminateSoundConvert_DSMP3( SoundConv ) ; break ;
#endif
#endif
#endif

#ifndef DX_NON_MEDIA_FOUNDATION
	case SOUND_METHODTYPE_MF : return TerminateSoundConvert_MF( SoundConv ) ;
#endif
	}

	return 0 ;
}

// (環境依存処理)変換後の大凡のデータサイズを得る
extern	int GetSoundConvertDestSize_Fast_PF( SOUNDCONV *SoundConv )
{
	switch( SoundConv->MethodType )
	{
#ifndef DX_NON_ACM
	case SOUND_METHODTYPE_ACM : return GetSoundConvertDestSize_Fast_ACM( SoundConv ) ;
#endif
#ifndef DX_NON_MOVIE
#ifndef DX_NON_DSHOW_MOVIE
#ifndef DX_NON_DSHOW_MP3
	case SOUND_METHODTYPE_DSMP3 : return GetSoundConvertDestSize_Fast_DSMP3( SoundConv ) ;
#endif
#endif
#endif

#ifndef DX_NON_MEDIA_FOUNDATION
	case SOUND_METHODTYPE_MF : return GetSoundConvertDestSize_Fast_MF( SoundConv ) ;
#endif
	}

	return 0 ;
}












#ifndef DX_NON_ACM

#ifdef DX_GCC_COMPILE
#define ACM_FORMATSUGGESTF_WFORMATTAG       0x00010000L
#define ACM_FORMATSUGGESTF_NCHANNELS        0x00020000L
#define ACM_FORMATSUGGESTF_NSAMPLESPERSEC   0x00040000L
#define ACM_FORMATSUGGESTF_WBITSPERSAMPLE   0x00080000L

#define ACM_STREAMOPENF_QUERY           0x00000001
#define ACM_STREAMOPENF_ASYNC           0x00000002
#define ACM_STREAMOPENF_NONREALTIME     0x00000004
#endif // DX_GCC_COMPILE

// ＡＣＭファイルのセットアップ処理を行う( [戻] -1:エラー )
extern	int SetupSoundConvert_ACM( SOUNDCONV *SoundConv )
{
	___RIFFCHUNK chunk ;
	char type[5] ;
	STREAMDATA *Stream = &SoundConv->Stream ;
//	SOUNDCONV_ACM *acmdata = &SoundConv->AcmTypeData ;
	SOUNDCONV_ACM *acmdata = (SOUNDCONV_ACM *)SoundConv->ConvFunctionBuffer ;
	int res, i ;
	
	acmdata->Format = NULL ;
	acmdata->AcmStreamHandle[0] = NULL ;
	acmdata->AcmStreamHandle[1] = NULL ;
	acmdata->SrcData = NULL ;
	acmdata->DestDataSampleNum = -1 ;
	acmdata->BeginSeekPosition = 0 ;
	acmdata->BeginSeekCompSrcSize = 0 ;
	acmdata->BeginSeekBuffer = NULL ;

	// RIFFファイルかどうか調べる
	Stream->ReadShred.Read( &chunk, sizeof( chunk ), 1, Stream->DataPoint ) ;
	if( chunk.fcc != RIFFCHUNKID( 'R', 'I', 'F', 'F' ) )
		goto ERR ;

	// WAVEファイルかどうか調べる
	type[4] = '\0' ;
	Stream->ReadShred.Read( type, 4, 1, Stream->DataPoint ) ;
	if( _STRCMP( type, "WAVE" ) != 0 )
		goto ERR ;

	// フォーマットを取得する
	{
		// 拡張情報があったらスキップする
		Stream->ReadShred.Read( &chunk, sizeof( chunk ), 1, Stream->DataPoint ) ;
		if( chunk.fcc == RIFFCHUNKID( 'b', 'e', 'x', 't' ) )
		{
			Stream->ReadShred.Seek( Stream->DataPoint, chunk.cb, SEEK_CUR ) ;
			Stream->ReadShred.Read( &chunk, sizeof( chunk ), 1, Stream->DataPoint ) ;
		}
		if( chunk.fcc != RIFFCHUNKID( 'f', 'm', 't', ' ' ) ) goto ERR ;

		acmdata->Format = (WAVEFORMATEX *)DXALLOC( chunk.cb ) ;
		if( acmdata->Format == NULL ) goto ERR ;

		// ヘッダの位置とサイズをセット
		SoundConv->HeaderPos = ( int )Stream->ReadShred.Tell( Stream->DataPoint ) ;
		SoundConv->HeaderSize = ( int )chunk.cb ;

		Stream->ReadShred.Read( acmdata->Format, chunk.cb, 1, Stream->DataPoint ) ;

		// 形式が WAVEFORMATEX ではなかった場合は補完する
		if( acmdata->Format->wFormatTag == WAVE_FORMAT_PCM && chunk.cb == 16 )
		{
			acmdata->Format->wBitsPerSample = ( WORD )( acmdata->Format->nBlockAlign / acmdata->Format->nChannels * 8 ) ;
			acmdata->Format->cbSize = 0 ;
		}
	}
	
	// データチャンクを探す
	while( Stream->ReadShred.Eof( Stream->DataPoint ) == 0 )
	{
		Stream->ReadShred.Read( &chunk , sizeof( chunk ) , 1 , Stream->DataPoint ) ;
		if( chunk.fcc == RIFFCHUNKID( 'd', 'a', 't', 'a' ) ) break ;
		Stream->ReadShred.Seek( Stream->DataPoint, chunk.cb + ( ( chunk.cb % 2 != 0 ) ? 1 : 0 ) , SEEK_CUR ) ; 
	}
	
	if( Stream->ReadShred.Eof( Stream->DataPoint ) != 0 )
		goto ERR ;

	// データチャンクの情報をセット
	SoundConv->DataPos = ( int )Stream->ReadShred.Tell( Stream->DataPoint ) ;
	SoundConv->DataSize = ( int )chunk.cb ;

	// 変換後の推奨フォーマットを得る
	_MEMSET( &SoundConv->OutFormat, 0, sizeof( SoundConv->OutFormat ) ) ;
	SoundConv->OutFormat.wFormatTag = WAVE_FORMAT_PCM ;
	res = ( int )GSoundConvertData_Win.acmFormatSuggestFunc( NULL, acmdata->Format, &SoundConv->OutFormat, sizeof( WAVEFORMATEX ), ACM_FORMATSUGGESTF_WFORMATTAG ) ;
	if( res != 0 ) goto ERR ;
	
	// 圧縮データを一時的に保存するメモリ領域の確保
//	acmdata->SrcDataSize = AdjustSoundDataBlock( acmdata->Format->nAvgBytesPerSec / STS_CACHEBUFFERSEC, acmdata->Format ) ;
	acmdata->SrcDataSize = SoundConv->DataSize ;  // ループ処理の関係で、丸々データが収まる領域を確保する
	acmdata->SrcData = DXALLOC( ( size_t )acmdata->SrcDataSize ) ;
	if( acmdata->SrcData == NULL ) goto ERR ;
//	acmdata->SrcDataValidSize = 0 ;
	acmdata->SrcDataPosition = 0 ;

	// 圧縮データを一括で読み込む
	Stream->ReadShred.Read( acmdata->SrcData, ( size_t )SoundConv->DataSize, 1, Stream->DataPoint ) ;
	Stream->ReadShred.Seek( Stream->DataPoint, SoundConv->DataPos, STREAM_SEEKTYPE_SET ) ;
//	acmdata->SrcDataValidSize = SoundConv->DataSize ;
	
	// ＡＣＭハンドルの作成
	for( i = 0 ; i < 2 ; i ++ )
	{
		res = ( int )GSoundConvertData_Win.acmStreamOpenFunc( &acmdata->AcmStreamHandle[i],
								NULL,
								acmdata->Format,
								&SoundConv->OutFormat,
								NULL,
								0,
								0,
								ACM_STREAMOPENF_NONREALTIME ) ;
		if( res != 0 ) goto ERR ;
	}

	// 変換後のＰＣＭデータを一時的に保存するメモリ領域のサイズをセット
	SoundConv->DestDataSize = ( int )SoundConv->OutFormat.nAvgBytesPerSec ;

	// シーク予定位置まで変換しておく処理で使用するメモリ領域の確保
	acmdata->BeginSeekBuffer = DXALLOC( ( size_t )SoundConv->DestDataSize ) ;
	if( acmdata->BeginSeekBuffer == NULL ) goto ERR ;

	// タイプセット
	SoundConv->MethodType = SOUND_METHODTYPE_ACM ;

	// 正常終了
	return 0 ;

ERR :
	if( acmdata->AcmStreamHandle[0] != NULL )
	{
		GSoundConvertData_Win.acmStreamCloseFunc( acmdata->AcmStreamHandle[0], 0 ) ;
		acmdata->AcmStreamHandle[0] = NULL ;
	}

	if( acmdata->AcmStreamHandle[1] != NULL )
	{
		GSoundConvertData_Win.acmStreamCloseFunc( acmdata->AcmStreamHandle[1], 0 ) ;
		acmdata->AcmStreamHandle[1] = NULL ;
	}

	if( acmdata->BeginSeekBuffer != NULL )
	{
		DXFREE( acmdata->BeginSeekBuffer ) ;
		acmdata->BeginSeekBuffer = NULL ;
	}

	if( acmdata->Format != NULL )
	{
		DXFREE( acmdata->Format ) ;
		acmdata->Format = NULL ;
	}

	if( acmdata->SrcData != NULL )
	{
		DXFREE( acmdata->SrcData ) ;
		acmdata->SrcData = NULL ;
	}
	
	return -1 ;
}

#endif // DX_NON_ACM











#ifndef DX_NON_ACM

// ＭＰ３のヘッダファイルを作成するときに必要なデータの定義

// ビットレート情報の取得
int MP3BitRateTable[3][3][16] =
{
	// ＭＰＥＧ−１
	{
		// Layer-1
		0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, -1,

		// Layer-2
		0, 32, 48, 56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, 384, -1,

		// Layer-3
		0, 32, 40, 48,  56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, -1,
	},

	// ＭＰＥＧ−２
	{
		// Layer-1
		0, 32, 48, 56, 64, 80, 96, 112, 128, 144, 160, 176, 192, 224, 256, -1,

		// Layer-2
		0,  8, 16, 24, 32, 40, 48,  56,  64,  80,  96, 112, 128, 114, 160, -1,

		// Later-3
		0,  8, 16, 24, 32, 40, 48,  56,  64,  80,  96, 112, 128, 114, 160, -1,
	},

	// ＭＰＥＧ−２．５
	{
		// Layer-1
		0, 32, 48, 56, 64, 80, 96, 112, 128, 144, 160, 176, 192, 224, 256, -1,

		// Layer-2
		0,  8, 16, 24, 32, 40, 48,  56,  64,  80,  96, 112, 128, 114, 160, -1,

		// Later-3
		0,  8, 16, 24, 32, 40, 48,  56,  64,  80,  96, 112, 128, 114, 160, -1,
	},
};

// サンプリング周波数
int MP3FreqTable[3][4] =
{
	// ＭＰＥＧ−１
	44100,48000,32000,-1,

	// ＭＰＥＧ−２
	22050,24000,16000,-1,

	// ＭＰＥＧ−２．５
	11025,12000, 8000,-1,
};

// フレームヘッダ構造体
struct MP3_FRAMEHEADER
{
	BYTE Layer ;			// 0:Layer-1 1:Layer-2 2:Layer-3
	BYTE MpegV ;			// 0:MPEG1 1:MPEG2
	BYTE BitRateIndex ;
	BYTE SampleRateIndex ;
	BYTE PaddingBit ;
	BYTE ChannelNum ;
	DWORD SampleRate ;
	DWORD BitRate ;
	DWORD FrameSize ;
	DWORD SampleNum ;
} ;

static int AnalysisMP3_FrameHeader( BYTE *Header, MP3_FRAMEHEADER *FrHead )
{
	// ＭＰ３データか調べる
	if( Header[0] != 0xff || ( Header[1] & 0xe0 ) != 0xe0 ) return -2 ;

	// ＭＰＥＧのバージョンを得る( 00:MPEG2.5  01:予約  10:MPEG2  11:MPEG1 )
	FrHead->MpegV = 0 ;
	switch( ( BYTE )( ( Header[1] >> 3 ) & 0x03 ) )
	{
	case 0 : FrHead->MpegV = 2 ; break ;
	case 2 : FrHead->MpegV = 1 ; break ;
	case 3 : FrHead->MpegV = 0 ; break ;
	default:
		return -1 ;
	}

	// レイヤを得る(00:予約  01:Layer-3  10:Layer-2  11:Layer-1)
	switch( ( BYTE )( ( Header[1] >> 1 ) & 0x03 ) )
	{
	case 1:	FrHead->Layer = 2 ; break ;
	case 2:	FrHead->Layer = 1 ; break ;
	case 3:	FrHead->Layer = 0 ; break ;
	default : return -1 ;
	}

	// ビットレートのインデックス値を得る
	FrHead->BitRateIndex = ( BYTE )( ( Header[2] >> 4 ) & 0xf ) ;
	if( FrHead->BitRateIndex > 14 )
		return -1 ;

	// ビットレートを得る
	FrHead->BitRate = ( DWORD )MP3BitRateTable[FrHead->MpegV][FrHead->Layer][FrHead->BitRateIndex] ;

	// サンプリング周波数のインデックス値を得る
	FrHead->SampleRateIndex = ( BYTE )( ( Header[2] >> 2 ) & 0x3 ) ;
	if( FrHead->SampleRateIndex > 2 ) return -1 ;

	// サンプリング周波数を得る
	FrHead->SampleRate = ( DWORD )MP3FreqTable[FrHead->MpegV][FrHead->SampleRateIndex] ;

	// パディングビットを得る
	FrHead->PaddingBit = ( BYTE )( ( Header[2] >> 1 ) & 1 ) ;

	// チャンネルモードからチャンネル数を得る
	FrHead->ChannelNum = ( BYTE )( ( ( Header[3] >> 6 ) & 0x3 ) == 3 ? 1 : 2 ) ;
	
	// フレームサイズを計算する
	switch( FrHead->Layer )
	{
	case 0 :		// Layer-1
		FrHead->FrameSize = ( ( 12 * ( FrHead->BitRate * 1000 ) / FrHead->SampleRate ) + FrHead->PaddingBit ) * 4 ;
		FrHead->SampleNum = 12 * 8 ;
		break ;
			
	case 1 :		// Layer-2 Layer-3
	case 2 :
		FrHead->FrameSize = ( 144 * ( FrHead->BitRate * 1000 ) / FrHead->SampleRate ) + FrHead->PaddingBit ;
		FrHead->SampleNum = 144 * 8 ;
		break ;
	}

	// 正常終了	
	return 0 ;
}

// ＭＰ３ファイルのセットアップ処理を行う( [戻] -1:エラー )
extern int SetupSoundConvert_MP3( SOUNDCONV *SoundConv )
{
	BYTE Header[4] ;
//	BYTE Layer, MpegV, BitRateIndex, SampleRateIndex ;
//	BYTE PaddingBit, ChannelNum ;
//	DWORD SampleRate, BitRate, FrameSize ;
	D_MPEGLAYER3WAVEFORMAT *MP3Format = NULL ;
	MP3_FRAMEHEADER MP3Frame ;
	MP3_FRAMEHEADER FirstMP3Frame = { 0 } ;
	int DataSize, FileSize, /*StartPoint,*/ res, i;
	DWORD LastPoint = 0 ;
	DWORD SkipSize, EndSkipSize = 0, StartPos ;
	int MaxSampleRate = -1 ;
	int MaxChannelNum = -1 ;
//	SOUNDCONV_ACM *acmdata = &SoundConv->AcmTypeData ;
	SOUNDCONV_ACM *acmdata = (SOUNDCONV_ACM *)SoundConv->ConvFunctionBuffer ;
	STREAMDATA *Stream = &SoundConv->Stream ;
	

	acmdata->Format = NULL ;
	acmdata->SrcData = NULL ;
	acmdata->DestDataSampleNum = 0 ;
	acmdata->BeginSeekPosition = 0 ;
	acmdata->BeginSeekCompSrcSize = 0 ;

	// ファイルサイズを得る
	Stream->ReadShred.Seek( Stream->DataPoint, 0, STREAM_SEEKTYPE_END ) ;
	FileSize = ( int )Stream->ReadShred.Tell( Stream->DataPoint ) ;
	Stream->ReadShred.Seek( Stream->DataPoint, 0, STREAM_SEEKTYPE_SET ) ;

	// ID3v2 タグがある場合のスキップ処理を行う
	{
		char Tag[4] ;
		BYTE MainVer, SubVer, Flag, SizeData[4] ;

		Tag[3] = '\0' ;
		Stream->ReadShred.Read( Tag, 1, 3, Stream->DataPoint ) ;
		if( _STRCMP( Tag, "ID3" ) == 0 )
		{
			// バージョンを得る
			Stream->ReadShred.Read( &MainVer, 1, 1, Stream->DataPoint ) ;
			Stream->ReadShred.Read( &SubVer, 1, 1, Stream->DataPoint ) ;

			// フラグを得る
			Stream->ReadShred.Read( &Flag, 1, 1, Stream->DataPoint ) ;

			// 容量情報を得る
			Stream->ReadShred.Read( SizeData, 1, 4, Stream->DataPoint ) ;
			SkipSize = ( DWORD )( ( SizeData[0] << 21 ) + ( SizeData[1] << 14 ) + ( SizeData[2] << 7 ) + SizeData[3] + 10 ) ;

			// フッタ情報がある場合は更に１０バイト切る
			if( MainVer == 4 && ( Flag & 0x10 ) != 0 ) SkipSize += 10 ;
		}
		else
		{
			SkipSize = 0 ;
		}
	}

	// ファイルの終端を調べる。主に ID3v1 タグがあるかどうかで変化する
	if( FileSize > 128 )
	{
		char Tag[4] ;

		Tag[3] = '\0' ;
		Stream->ReadShred.Seek( Stream->DataPoint, -128, STREAM_SEEKTYPE_END ) ;
		Stream->ReadShred.Read( Tag, 1, 3, Stream->DataPoint ) ;

		if( _STRCMP( Tag, "TAG" ) == 0 )
		{
			EndSkipSize = 128 ;
		}
		else
		{
			EndSkipSize = 0 ;
		}
	}

	// データの末端を検索する
	for( i = 0 ; i < 2 ; i ++ )
	{
		// シークする
		Stream->ReadShred.Seek( Stream->DataPoint, SkipSize, STREAM_SEEKTYPE_SET ) ;

		StartPos = 0xffffffff ;
		for(;;)
		{
			for(;;)
			{
				if( Stream->ReadShred.Read( &Header[0], 1, 1, Stream->DataPoint ) == 0 ) goto R1 ;
				if( Header[0] == 0xff )
				{
					if( Stream->ReadShred.Read( &Header[1], 1, 1, Stream->DataPoint ) == 0 ) goto R1 ;
					if( ( Header[1] & 0xe0 ) == 0xe0 )
					{
						break ;
					}
				}
			}
			
			// 残りのヘッダを読み込む
			Stream->ReadShred.Read( &Header[2], 1, 2, Stream->DataPoint ) ;

			// ヘッダを解析
			if( AnalysisMP3_FrameHeader( Header, &MP3Frame ) < 0 )
				return -1 ;

			// もしフレームサイズが0だったらエラー
			if( MP3Frame.FrameSize <= 0 )
				return -1 ;

			// もし最初のフレームだったら位置を保存しておく
			if( StartPos == 0xffffffff )
			{
				// ２ループ目の場合は、最大のサンプリング周波数のフレームのみ有効なフレームとする
				if( i == 0 || ( i == 1 && MP3Frame.SampleRate == ( DWORD )MaxSampleRate ) )
				{
					StartPos = ( DWORD )Stream->ReadShred.Tell( Stream->DataPoint ) - 4 ;

					// 最初のフレームのヘッダを保存しておく
					FirstMP3Frame = MP3Frame ;
				}
			}
			else
			{
				// もし２ループ目で最初のフレームと周波数やチャンネル数が異なる場合は非対応
				if( i == 1 &&
					( FirstMP3Frame.ChannelNum != MP3Frame.ChannelNum ||
					  FirstMP3Frame.SampleRate != MP3Frame.SampleRate ) )
				{
					return -1 ;
				}
			}

			// 最大サンプル数と最大チャンネル数を更新
			if( i == 0 )
			{
				if( MaxSampleRate < 0 || MaxSampleRate < ( int )MP3Frame.SampleRate )
				{
					MaxSampleRate = ( int )MP3Frame.SampleRate ;
				}
				if( MaxChannelNum < 0 || MaxChannelNum < ( int )MP3Frame.ChannelNum )
				{
					MaxChannelNum = ( int )MP3Frame.ChannelNum ;
				}
			}

			// 総サンプル数を増やす
			acmdata->DestDataSampleNum += MP3Frame.SampleNum ;

			// データの末端アドレスを取得
			{
				LastPoint = ( DWORD )( Stream->ReadShred.Tell( Stream->DataPoint ) + MP3Frame.FrameSize - 4 ) ;
				if( LastPoint >= FileSize - EndSkipSize )
				{
					if( LastPoint > FileSize - EndSkipSize ) LastPoint = FileSize - EndSkipSize;
					break ;
				}
				Stream->ReadShred.Seek( Stream->DataPoint, MP3Frame.FrameSize - 4, STREAM_SEEKTYPE_CUR ) ;
			}
		}
	}

R1 :
	// フレームデータが見つからなかったらエラー
	if( StartPos == 0xffffffff )
		return -1 ;

	// 最初のフレームの位置へ移動
	Stream->ReadShred.Seek( Stream->DataPoint, StartPos, STREAM_SEEKTYPE_SET ) ;

	// ヘッダを読み込む
	Stream->ReadShred.Read( &Header[0], 1, 4, Stream->DataPoint ) ;

	// ヘッダを解析
	if( AnalysisMP3_FrameHeader( Header, &MP3Frame ) < 0 )
		return -1 ;

	// ヘッダのセット
	{
		// メモリの確保
		int headerSize = sizeof( D_MPEGLAYER3WAVEFORMAT );
		GSoundConvertData_Win.acmMetricsFunc( NULL, D_ACM_METRIC_MAX_SIZE_FORMAT, &headerSize );

		MP3Format = ( D_MPEGLAYER3WAVEFORMAT * )DXALLOC( ( size_t )headerSize ) ;
		if( MP3Format == NULL ) goto ERR ;

		// ＭＰ３ＷＡＶＥフォーマットのデータをセットする
		_MEMSET( MP3Format, 0, ( size_t )headerSize ) ;
		MP3Format->wfx.cbSize          = D_MPEGLAYER3_WFX_EXTRA_BYTES ;
		MP3Format->wfx.wFormatTag      = D_WAVE_FORMAT_MPEGLAYER3 ;
		MP3Format->wfx.nChannels       = MP3Frame.ChannelNum ;
		MP3Format->wfx.nSamplesPerSec  = MP3Frame.SampleRate ;
		MP3Format->wfx.nAvgBytesPerSec = MP3Frame.BitRate * 1000 / 8 ;
		MP3Format->wfx.nBlockAlign     = 1 ;
		MP3Format->wfx.wBitsPerSample  = 0 ;
		MP3Format->wID                 = D_MPEGLAYER3_ID_MPEG ;
		MP3Format->fdwFlags            = MP3Frame.PaddingBit/*0*//*MPEGLAYER3_FLAG_PADDING_OFF*/ ;
		MP3Format->nBlockSize          = ( WORD )MP3Frame.FrameSize ;
		MP3Format->nFramesPerBlock     = 1 ;
		MP3Format->nCodecDelay         = 700/*0x0571*/ ;
	}
/*
	// データのスタートアドレスを取得
	StartPoint = Stream->ReadShred.Tell( Stream->DataPoint ) - 4 ;
*/

	// 有効データサイズを取得
	DataSize = ( int )( LastPoint - StartPos ) ;
//	DataSize = LastPoint - StartPoint ;
//	DataSize = FileSize - SkipSize - EndSkipSize ;

	// ファイルポインタの操作
	Stream->ReadShred.Seek( Stream->DataPoint, StartPos, STREAM_SEEKTYPE_SET ) ;
//	Stream->ReadShred.Seek( Stream->DataPoint, StartPoint, STREAM_SEEKTYPE_SET ) ;

	// 情報のセット
	acmdata->Format = ( WAVEFORMATEX * )MP3Format ;
//	SoundConv->DataPos  = StartPoint ;
	SoundConv->DataPos  = ( int )StartPos ;
	SoundConv->DataSize = DataSize ;

	// 変換後の推奨フォーマットを得る
	_MEMSET( &SoundConv->OutFormat, 0, sizeof( SoundConv->OutFormat ) ) ;
	SoundConv->OutFormat.wFormatTag = WAVE_FORMAT_PCM ;
	res = ( int )GSoundConvertData_Win.acmFormatSuggestFunc( NULL, acmdata->Format, &SoundConv->OutFormat, sizeof( WAVEFORMATEX ), ACM_FORMATSUGGESTF_WFORMATTAG ) ;
	if( res != 0 ) goto ERR ;

	// 圧縮データを一時的に保存するメモリ領域の確保
//	acmdata->SrcDataSize = AdjustSoundDataBlock( acmdata->Format->nAvgBytesPerSec * 8/* / STS_CACHEBUFFERSEC*/, acmdata->Format ) ;
	acmdata->SrcDataSize = SoundConv->DataSize ;  // ループ処理の関係で、丸々データが収まる領域を確保する
	acmdata->SrcData = DXALLOC( ( size_t )acmdata->SrcDataSize ) ;
	if( acmdata->SrcData == NULL ) goto ERR ;
//	acmdata->SrcDataValidSize = 0 ;

	// 圧縮データを一括で読み込む
	Stream->ReadShred.Read( acmdata->SrcData, ( size_t )SoundConv->DataSize, 1, Stream->DataPoint ) ;
	Stream->ReadShred.Seek( Stream->DataPoint, StartPos, STREAM_SEEKTYPE_SET ) ;
//	acmdata->SrcDataValidSize = SoundConv->DataSize ;
	acmdata->SrcDataPosition = 0 ;

	// ＡＣＭハンドルの作成
	for( i = 0 ; i < 2 ; i ++ )
	{
		res = ( int )GSoundConvertData_Win.acmStreamOpenFunc( &acmdata->AcmStreamHandle[i],
								NULL,
								acmdata->Format,
								&SoundConv->OutFormat,
								NULL,
								0,
								0,
								ACM_STREAMOPENF_NONREALTIME ) ;
		if( res != 0 ) goto ERR ;
	}

	// 変換後のＰＣＭデータを一時的に保存するメモリ領域のサイズをセット(１秒分よりも少し大きめにとっておく)
	SoundConv->DestDataSize = ( int )( SoundConv->OutFormat.nAvgBytesPerSec * 4 / 3 ) ;
	
	// シーク予定位置まで変換しておく処理で使用するメモリ領域の確保
	acmdata->BeginSeekBuffer = DXALLOC( ( size_t )SoundConv->DestDataSize ) ;
	if( acmdata->BeginSeekBuffer == NULL ) goto ERR ;

	// タイプセット
	SoundConv->MethodType = SOUND_METHODTYPE_ACM ;

	// 終了
	return 0 ;
	
ERR :
	if( acmdata->AcmStreamHandle[0] != NULL )
	{
		GSoundConvertData_Win.acmStreamCloseFunc( acmdata->AcmStreamHandle[0], 0 ) ;
		acmdata->AcmStreamHandle[0] = NULL ;
	}

	if( acmdata->AcmStreamHandle[1] != NULL )
	{
		GSoundConvertData_Win.acmStreamCloseFunc( acmdata->AcmStreamHandle[1], 0 ) ;
		acmdata->AcmStreamHandle[1] = NULL ;
	}

	if( acmdata->BeginSeekBuffer != NULL )
	{
		DXFREE( acmdata->BeginSeekBuffer ) ;
		acmdata->BeginSeekBuffer = NULL ;
	}

	if( acmdata->Format != NULL )
	{
		DXFREE( acmdata->Format ) ;
		acmdata->Format = NULL ;
	}

	if( acmdata->SrcData != NULL )
	{
		DXFREE( acmdata->SrcData ) ;
		acmdata->SrcData = NULL ;
	}
	
	return -1 ;
}

#endif // DX_NON_ACM










#ifndef DX_NON_ACM

// ＡＣＭを使用したファイルの後始末処理を行う
extern	int TerminateSoundConvert_ACM( SOUNDCONV *SoundConv )
{
//	SOUNDCONV_ACM *acmdata = &SoundConv->AcmTypeData ;
	SOUNDCONV_ACM *acmdata = (SOUNDCONV_ACM *)SoundConv->ConvFunctionBuffer ;

	if( acmdata->Format != NULL )
	{
		DXFREE( acmdata->Format ) ;
		acmdata->Format = NULL ;
	}

	if( acmdata->BeginSeekBuffer != NULL )
	{
		DXFREE( acmdata->BeginSeekBuffer ) ;
		acmdata->BeginSeekBuffer = NULL ;
	}
	
	if( acmdata->AcmStreamHandle[0] != NULL )
	{
		GSoundConvertData_Win.acmStreamCloseFunc( acmdata->AcmStreamHandle[0], 0 ) ;
		acmdata->AcmStreamHandle[0] = NULL ;
	}

	if( acmdata->AcmStreamHandle[1] != NULL )
	{
		GSoundConvertData_Win.acmStreamCloseFunc( acmdata->AcmStreamHandle[1], 0 ) ;
		acmdata->AcmStreamHandle[1] = NULL ;
	}
	
	if( acmdata->SrcData != NULL )
	{
		DXFREE( acmdata->SrcData ) ;
		acmdata->SrcData = NULL ;
	}

	return 0 ;
}

#endif // DX_NON_ACM








#ifndef DX_NON_MOVIE
#ifndef DX_NON_DSHOW_MOVIE
#ifndef DX_NON_DSHOW_MP3

// MP3を使用したファイルの後始末処理を行う
extern int TerminateSoundConvert_DSMP3( SOUNDCONV *SoundConv )
{
	SOUNDCONV_DSMP3 *dsmp3 = ( SOUNDCONV_DSMP3 * )SoundConv->ConvFunctionBuffer ;

	if( dsmp3->PCMBuffer )
	{
		DXFREE( dsmp3->PCMBuffer ) ;
		dsmp3->PCMBuffer = NULL ;
	}

	return 0 ;
}

#endif // DX_NON_DSHOW_MP3
#endif // DX_NON_DSHOW_MOVIE
#endif // DX_NON_MOVIE









#ifndef DX_NON_ACM

// 変換後のバッファにデータを補充する
extern	int ConvertProcessSoundConvert_ACM( SOUNDCONV *SoundConv )
{
	D_ACMSTREAMHEADER header ;
//	SOUNDCONV_ACM *acmdata = &SoundConv->AcmTypeData ;
	SOUNDCONV_ACM *acmdata = (SOUNDCONV_ACM *)SoundConv->ConvFunctionBuffer ;
//	STREAMDATA *Stream = &SoundConv->Stream ;
	DWORD ConvSize = 0 ;

	// シーク予定位置とサブACMハンドルで変換が完了している位置が違う場合は処理を行う
	if( SoundConv->SeekLockPosition != acmdata->BeginSeekPosition )
	{
		// 追い越してしまっている場合は最初からやり直す
		if( acmdata->BeginSeekPosition > SoundConv->SeekLockPosition )
		{
			GSoundConvertData_Win.acmStreamCloseFunc( acmdata->AcmStreamHandle[1], 0 ) ;
			GSoundConvertData_Win.acmStreamOpenFunc( &acmdata->AcmStreamHandle[1], NULL,
							acmdata->Format, &SoundConv->OutFormat,
							NULL, 0, 0, ACM_STREAMOPENF_NONREALTIME ) ;
			acmdata->BeginSeekPosition = 0 ;
			acmdata->BeginSeekCompSrcSize = 0 ;
		}

		if( SoundConv->SeekLockPosition - 1024 * 10 > acmdata->BeginSeekPosition )
		{
			// 変換済みの位置を進める
			_MEMSET( &header, 0, sizeof( D_ACMSTREAMHEADER ) ) ;
			header.cbStruct		= sizeof( D_ACMSTREAMHEADER ) ;
			header.pbSrc		= (BYTE *)acmdata->SrcData + acmdata->BeginSeekCompSrcSize ;
			if( acmdata->SrcDataSize - acmdata->BeginSeekCompSrcSize > 2048 ) header.cbSrcLength = 2048 ;
			else 	                                                          header.cbSrcLength = ( DWORD )( acmdata->SrcDataSize - acmdata->BeginSeekCompSrcSize ) ;
			header.pbDst		= (BYTE *)acmdata->BeginSeekBuffer ;
			header.cbDstLength	= ( DWORD )SoundConv->DestDataSize ;
			if( (signed)( header.cbDstLength + acmdata->BeginSeekPosition ) > SoundConv->SeekLockPosition ) 
				header.cbDstLength = ( DWORD )( SoundConv->SeekLockPosition - acmdata->BeginSeekPosition ) ;

			if( GSoundConvertData_Win.acmStreamPrepareHeaderFunc( acmdata->AcmStreamHandle[1], &header, 0 ) != 0 )
				return -1 ;

			// 展開
			GSoundConvertData_Win.acmStreamConvertFunc( acmdata->AcmStreamHandle[1], &header, D_ACM_STREAMCONVERTF_BLOCKALIGN ) ;

			// 展開が成功した分だけ座標を進める
			acmdata->BeginSeekPosition += header.cbDstLengthUsed ;
			acmdata->BeginSeekCompSrcSize += header.cbSrcLengthUsed ;

			// ヘッダの解除
			GSoundConvertData_Win.acmStreamUnprepareHeaderFunc( acmdata->AcmStreamHandle[1], &header, 0 ) ;
		}
	}

	// 終端まで来ていたら終了
	if( acmdata->SrcDataPosition == acmdata->SrcDataSize )
		return -1 ;

	// 変換元のデータが無かったらストリームから読み込みむ
//(ループ処理の関係でデータを丸々読み込んでいるためこの処理は必要なくなった)
/*	if( acmdata->SrcDataValidSize < acmdata->SrcDataSize )
	{
		int pos, readsize ;

		pos = Stream->ReadShred.Tell( Stream->DataPoint ) - SoundConv->DataPos ; 
		if( pos == SoundConv->DataSize && acmdata->SrcDataValidSize == 0 )
			return -1 ;

		readsize = SoundConv->DataSize - pos ;
		if( readsize != 0 )
		{
			if( readsize > acmdata->SrcDataSize - acmdata->SrcDataValidSize )
				readsize = acmdata->SrcDataSize - acmdata->SrcDataValidSize ;

			Stream->ReadShred.Read( (BYTE *)acmdata->SrcData + acmdata->SrcDataValidSize,
									readsize, 1, Stream->DataPoint ) ;
			acmdata->SrcDataValidSize += readsize ;
		}
	}
*/

	// 展開の準備
	_MEMSET( &header, 0, sizeof( D_ACMSTREAMHEADER ) ) ;
	header.cbStruct		= sizeof( D_ACMSTREAMHEADER ) ;
//	header.pbSrc		= (BYTE *)acmdata->SrcData ;
//	header.cbSrcLength	= acmdata->SrcDataValidSize ;
	header.pbSrc		= (BYTE *)acmdata->SrcData + acmdata->SrcDataPosition ;
	if( acmdata->SrcDataSize - acmdata->SrcDataPosition > 2048 ) header.cbSrcLength = 2048 ;
	else 	                                                     header.cbSrcLength = ( DWORD )( acmdata->SrcDataSize - acmdata->SrcDataPosition ) ;
	header.pbDst		= (BYTE *)SoundConv->DestData ;
	header.cbDstLength	= ( DWORD )SoundConv->DestDataSize ;


	if( GSoundConvertData_Win.acmStreamPrepareHeaderFunc( acmdata->AcmStreamHandle[0], &header, 0 ) != 0 )
		return -1 ;

	// 展開
	GSoundConvertData_Win.acmStreamConvertFunc( acmdata->AcmStreamHandle[0], &header, D_ACM_STREAMCONVERTF_BLOCKALIGN ) ;

	// データのセット
	ConvSize = header.cbDstLengthUsed ;
/*	if( acmdata->DestDataSampleNum != -1 )
	{
		if( ( SoundConv->DestDataCompSize + ConvSize ) / SoundConv->OutFormat.nBlockAlign >= acmdata->DestDataSampleNum )
		{
			ConvSize = ( acmdata->DestDataSampleNum - SoundConv->DestDataCompSize / SoundConv->OutFormat.nBlockAlign ) * SoundConv->OutFormat.nBlockAlign ;
		}
	}
*/
	SoundConv->DestDataValidSize = ( int )header.cbDstLengthUsed ;
	acmdata->SrcDataPosition += header.cbSrcLengthUsed ;
//	acmdata->SrcDataValidSize -= header.cbSrcLengthUsed ;

	// ヘッダの解除
	GSoundConvertData_Win.acmStreamUnprepareHeaderFunc( acmdata->AcmStreamHandle[0], &header, 0 ) ;

	// 使用されたデータのサイズが０で、且つコンバートサイズが０だったら音声が終了している
	// 可能性があるので、その場合はコンバート終了
	if( ConvSize == 0 )
	{
		SoundConv->DestDataValidSize = 0 ;
		acmdata->SrcDataPosition = acmdata->SrcDataSize ;
//		acmdata->SrcDataValidSize = 0 ;
		return -1 ;
	}

	// 残った部分をスライドさせる
//(ループ処理の関係でデータを丸々読み込んでいるためこの処理は必要なくなった)
/*	if( acmdata->SrcDataValidSize != 0 )
		_MEMMOVE( acmdata->SrcData,
				 (BYTE *)acmdata->SrcData + header.cbSrcLengthUsed,
				 acmdata->SrcDataValidSize ) ;
*/

	// 終了
	return 0 ;
}

#endif // DX_NON_ACM

#ifndef DX_NON_MOVIE
#ifndef DX_NON_DSHOW_MOVIE
#ifndef DX_NON_DSHOW_MP3

// 変換後のバッファにデータを補充する
extern int ConvertProcessSoundConvert_DSMP3( SOUNDCONV *SoundConv )
{
	SOUNDCONV_DSMP3 *dsmp3 = ( SOUNDCONV_DSMP3 * )SoundConv->ConvFunctionBuffer ;
	int readsize ;

	// 全て転送しきっていたら終了
	if( dsmp3->PCMValidDataSize == dsmp3->PCMDestCopySize ) return -1 ;
	
	// 転送するデータサイズを決定する
	readsize = ( int )( dsmp3->PCMValidDataSize - dsmp3->PCMDestCopySize ) ;
	if( SoundConv->DestDataSize < readsize ) readsize = SoundConv->DestDataSize ;
	
	// 転送する
	_MEMCPY( SoundConv->DestData, ( BYTE * )dsmp3->PCMBuffer + dsmp3->PCMDestCopySize, ( size_t )readsize ) ;
	SoundConv->DestDataValidSize = readsize ;

	// 転送した分オフセットを進める
	dsmp3->PCMDestCopySize += readsize ;

	return 0 ;
}

#endif // DX_NON_DSHOW_MP3
#endif // DX_NON_DSHOW_MOVIE
#endif // DX_NON_MOVIE









#ifndef DX_NON_ACM

// 変換処理の位置を変更する( サンプル単位 )
extern int SetSampleTimeSoundConvert_ACM( SOUNDCONV *SoundConv, int SampleTime )
{
	int BytePos, CurPos, res, DelByte ;
	STREAMDATA *Stream = &SoundConv->Stream ;
//	SOUNDCONV_ACM *acmdata = &SoundConv->AcmTypeData ;
	SOUNDCONV_ACM *acmdata = (SOUNDCONV_ACM *)SoundConv->ConvFunctionBuffer ;

	BytePos = SampleTime * SoundConv->OutFormat.nBlockAlign ;
	CurPos = SoundConv->DestDataCompSizeAll ;
	
	// 丁度現在解凍してある部分の指定だった場合は処理を分岐
	if( BytePos >= CurPos && BytePos <= CurPos + SoundConv->DestDataValidSize )
	{
		DelByte = BytePos - CurPos ;
		SoundConv->DestDataCompSize += DelByte ;
		SoundConv->DestDataCompSizeAll += DelByte ;
		SoundConv->DestDataValidSize -= DelByte ;
		
		// 終了
		return 0 ;
	}
	
	// もう変換処理が済んでしまっている地点に戻る場合はＡＣＭハンドルを作成しなおし、
	// ストリームポイントもデータの最初に戻す
	if( BytePos < CurPos )
	{
		// もう一つのＡＣＭハンドルが指定の位置と同じか、それより位置的に手前である場合はそちらを使用する
		if( BytePos >= acmdata->BeginSeekPosition )
		{
			D_HACMSTREAM temp ;

			// ＡＣＭハンドルの交換
			temp = acmdata->AcmStreamHandle[0] ;
			acmdata->AcmStreamHandle[0] = acmdata->AcmStreamHandle[1] ;
			acmdata->AcmStreamHandle[1] = temp ;

			// 情報交換
			acmdata->SrcDataPosition = acmdata->BeginSeekCompSrcSize ;
			SoundConv->DestDataCompSize = acmdata->BeginSeekPosition ;
			SoundConv->DestDataCompSizeAll = acmdata->BeginSeekPosition ;
			SoundConv->DestDataValidSize = 0 ;

			// 今までメインだったＡＣＭハンドルは初期化
			GSoundConvertData_Win.acmStreamCloseFunc( acmdata->AcmStreamHandle[1], 0 ) ;
			GSoundConvertData_Win.acmStreamOpenFunc( &acmdata->AcmStreamHandle[1], NULL,
							acmdata->Format, &SoundConv->OutFormat,
							NULL, 0, 0, ACM_STREAMOPENF_NONREALTIME ) ;
			acmdata->BeginSeekCompSrcSize = 0 ;
			acmdata->BeginSeekPosition = 0 ;
		}
		else
		{
			// サブも駄目な場合はメインを作り直して一から追うしかない
			GSoundConvertData_Win.acmStreamCloseFunc( acmdata->AcmStreamHandle[0], 0 ) ;
			GSoundConvertData_Win.acmStreamOpenFunc( &acmdata->AcmStreamHandle[0], NULL,
							acmdata->Format, &SoundConv->OutFormat,
							NULL, 0, 0, ACM_STREAMOPENF_NONREALTIME ) ;
			Stream->ReadShred.Seek( Stream->DataPoint, SoundConv->DataPos, SEEK_SET ) ;
			acmdata->SrcDataPosition = 0 ;
//			acmdata->SrcDataValidSize = 0 ;

			SoundConv->DestDataValidSize = 0 ;
			SoundConv->DestDataCompSize = 0 ;
			SoundConv->DestDataCompSizeAll = 0 ;
		}
	}
	
	// 指定の位置まで解凍処理を進める
	res = 0 ;
	while( SoundConv->DestDataCompSizeAll + SoundConv->DestDataValidSize < BytePos )
	{
		SoundConv->DestDataCompSizeAll += SoundConv->DestDataValidSize ;
		SoundConv->DestDataValidSize = 0 ;
		SoundConv->DestDataCompSize = 0 ;
		res = ConvertProcessSoundConvert_ACM( SoundConv ) ;
		if( res == -1 ) break ;
	}
	
	// 終端に到達してしまったら変換終了状態にする
	if( res == -1 )
	{
		SoundConv->EndFlag = TRUE ;
		SoundConv->DestDataValidSize = 0 ;
		return 0 ;
	}
	
	DelByte = BytePos - SoundConv->DestDataCompSizeAll ;
	SoundConv->DestDataValidSize -= DelByte ;
	SoundConv->DestDataCompSize += DelByte ;
	SoundConv->DestDataCompSizeAll += DelByte ;

	// 終了
	return 0 ;
}

#endif // DX_NON_ACM

#ifndef DX_NON_MOVIE
#ifndef DX_NON_DSHOW_MOVIE
#ifndef DX_NON_DSHOW_MP3

// 変換処理の位置を変更する( サンプル単位 )
extern	int SetSampleTimeSoundConvert_DSMP3( SOUNDCONV *SoundConv, int SampleTime )
{
	int BytePos ;
	SOUNDCONV_DSMP3 *dsmp3 = ( SOUNDCONV_DSMP3 * )SoundConv->ConvFunctionBuffer ;
	
	BytePos = SampleTime * SoundConv->OutFormat.nBlockAlign ;

	SoundConv->DestDataValidSize   = 0 ;
	SoundConv->DestDataCompSize    = 0 ;
	SoundConv->DestDataCompSizeAll = BytePos ;
	dsmp3->PCMDestCopySize = ( DWORD )BytePos ;

	return 0 ;
}

#endif // DX_NON_DSHOW_MP3
#endif // DX_NON_DSHOW_MOVIE
#endif // DX_NON_MOVIE








#ifndef DX_NON_ACM

// 変換後の大凡のデータサイズを得る
extern	int GetSoundConvertDestSize_Fast_ACM( SOUNDCONV *SoundConv )
{
//	SOUNDCONV_ACM *acmdata = &SoundConv->AcmTypeData ;
	SOUNDCONV_ACM *acmdata = (SOUNDCONV_ACM *)SoundConv->ConvFunctionBuffer ;
	DWORD size ;
	
	if( acmdata->DestDataSampleNum != -1 )
	{
		size = (DWORD)( acmdata->DestDataSampleNum * SoundConv->OutFormat.nBlockAlign ) ;
	}
	else
	{
		GSoundConvertData_Win.acmStreamSizeFunc( acmdata->AcmStreamHandle[0], ( DWORD )SoundConv->DataSize, &size, D_ACM_STREAMSIZEF_SOURCE ) ;
	}
	return ( int )size ;
}

#endif // DX_NON_ACM


#ifndef DX_NON_MOVIE
#ifndef DX_NON_DSHOW_MOVIE
#ifndef DX_NON_DSHOW_MP3

// 変換後の大凡のデータサイズを得る
extern int GetSoundConvertDestSize_Fast_DSMP3( SOUNDCONV *SoundConv )
{
	SOUNDCONV_DSMP3 *dsmp3 = ( SOUNDCONV_DSMP3 * )SoundConv->ConvFunctionBuffer ;
	return ( int )dsmp3->PCMValidDataSize ;
}

#endif // DX_NON_DSHOW_MP3
#endif // DX_NON_DSHOW_MOVIE
#endif // DX_NON_MOVIE






#ifndef DX_NON_MEDIA_FOUNDATION

// Media Foundation を使用したファイルのセットアップ処理を行う( [戻] -1:エラー )
extern int SetupSoundConvert_MF( SOUNDCONV *SoundConv )
{
	SOUNDCONV_MF *mfdata = (SOUNDCONV_MF *)SoundConv->ConvFunctionBuffer ;
	STREAMDATA *Stream = &SoundConv->Stream ;
	HRESULT hr ;
	ULONGLONG StreamSize ;
	ULONGLONG Pos ;
	WAVEFORMATEX *pWav = NULL ;
	UINT32 cbFormat = 0;

	SETUP_WIN_API

	// Media Foundation 関連の DLL が無かったらエラー
	if( WinAPIData.Win32Func.MFPLATDLL == NULL ||
		WinAPIData.Win32Func.MFREADWRITEDLL == NULL ||
		WinAPIData.Win32Func.MFStartupFunc == NULL ||
		WinAPIData.Win32Func.MFCreateSourceReaderFromURLFunc == NULL ||
		WinAPIData.Win32Func.MFCreateMediaTypeFunc == NULL ||
		WinAPIData.Win32Func.MFCreateAttributesFunc == NULL ||
		WinAPIData.Win32Func.MFCreateAsyncResultFunc == NULL ||
		WinAPIData.Win32Func.MFInvokeCallbackFunc == NULL ||
		WinAPIData.Win32Func.MFCreateWaveFormatExFromMFMediaTypeFunc == NULL )
	{
		return -1 ;
	}

	Pos = Stream->ReadShred.Tell( Stream->DataPoint ) ;
	Stream->ReadShred.Seek( Stream->DataPoint, 0, SEEK_END ) ;
	StreamSize = Stream->ReadShred.Tell( Stream->DataPoint ) ;
	Stream->ReadShred.Seek( Stream->DataPoint, Pos, SEEK_SET ) ;

	mfdata->pReader = NULL ;
	mfdata->pMediaTypeAudioStream = NULL ;
	mfdata->pMediaTypeOutputAudioStream = NULL ;
	mfdata->pByteStream = NULL ;
	mfdata->pSampleBuffer = NULL ;

	// まだ MFStartup を呼んでいなかったら呼ぶ
	if( MovieGraphManageData.PF.MFStartupRunFlag == FALSE )
	{
		MovieGraphManageData.PF.MFStartupRunFlag = TRUE ;

		hr = WinAPIData.Win32Func.MFStartupFunc( D_MF_VERSION, D_MFSTARTUP_FULL ) ;
		if( hr != S_OK )
		{
			goto ERR ;
		}
	}

	mfdata->pByteStream = New_D_SoundConvertMFByteStream( Stream, StreamSize ) ;
	if( mfdata->pByteStream == NULL )
	{
		goto ERR ;
	}
	mfdata->pByteStream->Initialize() ;

	// ファイルを開く
	hr = WinAPIData.Win32Func.MFCreateSourceReaderFromByteStreamFunc(
		mfdata->pByteStream,
		NULL,
		&mfdata->pReader
	) ;
	if( hr != S_OK )
	{
		goto ERR ;
	}

	// 出力形式を作成
	WinAPIData.Win32Func.MFCreateMediaTypeFunc( &mfdata->pMediaTypeOutputAudioStream ) ;
	mfdata->pMediaTypeOutputAudioStream->SetGUID( D_MF_MT_MAJOR_TYPE, D_MFMEDIATYPE_AUDIO ) ;
	mfdata->pMediaTypeOutputAudioStream->SetGUID( D_MF_MT_SUBTYPE, D_MFAUDIOFORMAT_PCM ) ;

	// 出力形式をセット
	hr = mfdata->pReader->SetCurrentMediaType(
			( DWORD )D_MF_SOURCE_READER_FIRST_AUDIO_STREAM,
			NULL,
			mfdata->pMediaTypeOutputAudioStream ) ;
	if( hr != S_OK )
		goto ERR ;

	// ファイルの情報を取得する
	if( mfdata->pReader->GetCurrentMediaType(
			( DWORD )D_MF_SOURCE_READER_FIRST_AUDIO_STREAM, 
			&mfdata->pMediaTypeAudioStream ) != S_OK )
		goto ERR ;

	// オーディオストリームを選択
	hr = mfdata->pReader->SetStreamSelection(
		( DWORD )D_MF_SOURCE_READER_FIRST_AUDIO_STREAM, 
		TRUE ) ;
	if( hr != S_OK )
		goto ERR ;

	// 出力フォーマットを取得
	hr = WinAPIData.Win32Func.MFCreateWaveFormatExFromMFMediaTypeFunc(
		mfdata->pMediaTypeAudioStream,
		&pWav,
		&cbFormat,
		D_MFWaveFormatExConvertFlag_Normal
	) ;
	if( hr != S_OK )
		goto ERR ;

	// 再生時間を取得
	{
		D_PROPVARIANT variant ;

		if( mfdata->pReader->GetPresentationAttribute(
			( DWORD )D_MF_SOURCE_READER_MEDIASOURCE,
			D_MF_PD_DURATION,
			&variant ) != S_OK )
			goto ERR ;
		mfdata->Duration = variant.hVal.QuadPart ;
	}

	// 変換後のPCMデータを一時的に保存するメモリ領域の確保
	mfdata->SampleBufferBytes = 4096 ;
	mfdata->pSampleBuffer = DXALLOC( mfdata->SampleBufferBytes ) ;
	if( mfdata->pSampleBuffer == NULL )
		goto ERR ;
	mfdata->UseSampleBufferBytes = 0 ;
	mfdata->OutSampleBufferBytes = 0 ;

	// ＷＡＶＥフォーマットをセットする
	SoundConv->OutFormat.cbSize          = 0 ;
	SoundConv->OutFormat.wFormatTag      = WAVE_FORMAT_PCM ;
	SoundConv->OutFormat.nChannels       = pWav->nChannels ;
	SoundConv->OutFormat.nSamplesPerSec  = pWav->nSamplesPerSec ;
	SoundConv->OutFormat.wBitsPerSample  = pWav->wBitsPerSample ;
	SoundConv->OutFormat.nBlockAlign     = SoundConv->OutFormat.wBitsPerSample / 8 * SoundConv->OutFormat.nChannels ;
	SoundConv->OutFormat.nAvgBytesPerSec = SoundConv->OutFormat.nSamplesPerSec * SoundConv->OutFormat.nBlockAlign ;

	// 変換後のＰＣＭデータを一時的に保存するメモリ領域のサイズをセット
	SoundConv->DestDataSize = 4096 ;

	// 変換後のＰＣＭデータのサイズをセット
	SoundConv->DataSize = ( int )( mfdata->Duration * ( ULONGLONG )SoundConv->OutFormat.nAvgBytesPerSec / ( ULONGLONG )10000000 ) ;

	// タイプセット
	SoundConv->MethodType = SOUND_METHODTYPE_MF ;

	if( pWav != NULL								){		WinAPIData.Win32Func.CoTaskMemFreeFunc( pWav ) ;	pWav = NULL ; }
	
	// 終了
	return 0 ;

ERR :
	if( mfdata->pReader != NULL						){		mfdata->pReader->Release() ;						mfdata->pReader = NULL ; }
	if( mfdata->pMediaTypeAudioStream != NULL		){		mfdata->pMediaTypeAudioStream->Release() ;			mfdata->pMediaTypeAudioStream = NULL ; }
	if( mfdata->pMediaTypeOutputAudioStream != NULL	){		mfdata->pMediaTypeOutputAudioStream->Release() ;	mfdata->pMediaTypeOutputAudioStream = NULL ; }
	if( mfdata->pByteStream != NULL					){		mfdata->pByteStream->Release() ;					mfdata->pByteStream = NULL ; }
	if( mfdata->pSampleBuffer != NULL				){		DXFREE( mfdata->pSampleBuffer ) ;					mfdata->pSampleBuffer = NULL ; }
	if( pWav != NULL								){		WinAPIData.Win32Func.CoTaskMemFreeFunc( pWav ) ;	pWav = NULL ; }

	return -1 ;
}

// Media Foundation を使用したファイルの後始末処理を行う
extern	int TerminateSoundConvert_MF( SOUNDCONV *SoundConv )
{
	SOUNDCONV_MF *mfdata = (SOUNDCONV_MF *)SoundConv->ConvFunctionBuffer ;

	if( mfdata->pReader != NULL						){		mfdata->pReader->Release() ;						mfdata->pReader = NULL ; }
	if( mfdata->pMediaTypeAudioStream != NULL		){		mfdata->pMediaTypeAudioStream->Release() ;			mfdata->pMediaTypeAudioStream = NULL ; }
	if( mfdata->pMediaTypeOutputAudioStream != NULL	){		mfdata->pMediaTypeOutputAudioStream->Release() ;	mfdata->pMediaTypeOutputAudioStream = NULL ; }
	if( mfdata->pByteStream != NULL					){		mfdata->pByteStream->Release() ;					mfdata->pByteStream = NULL ; }
	if( mfdata->pSampleBuffer != NULL				){		DXFREE( mfdata->pSampleBuffer ) ;					mfdata->pSampleBuffer = NULL ; }

	return 0 ;
}

// 変換後のバッファにデータを補充する
extern int ConvertProcessSoundConvert_MF( SOUNDCONV *SoundConv )
{
	SOUNDCONV_MF *mfdata = (SOUNDCONV_MF *)SoundConv->ConvFunctionBuffer ;
	HRESULT hr ;
	DWORD dwFlags = 0 ;
	D_IMFSample *pSample = NULL ;
	D_IMFMediaBuffer *pBuffer = NULL ;
	DWORD cbBuffer = 0 ;
	BYTE *pAudioData = NULL ;
	LONGLONG llTimestamp ;

	// 出力できるデータが無い場合はデコードする
	if( mfdata->UseSampleBufferBytes - mfdata->OutSampleBufferBytes <= 0 )
	{
		for(;;)
		{
			int BytesPos ;

			if( pSample != NULL )
			{
				pSample->Release() ;
				pSample = NULL ;
			}

			hr = mfdata->pReader->ReadSample(
				( DWORD )D_MF_SOURCE_READER_FIRST_AUDIO_STREAM,
				0,
				NULL,
				&dwFlags,
				&llTimestamp,
				&pSample
			) ;
			if( hr != S_OK )
				goto ERR ;

			if( dwFlags & D_MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED )
				goto ERR ;

			if( dwFlags & D_MF_SOURCE_READERF_ENDOFSTREAM )
				goto ERR ;

			if( pSample == NULL )
				goto ERR ;

			// バイト位置の算出
			{
				LONGLONG Temp64_1, Temp64_2 ;
				DWORD Temp128[ 4 ] ;

				Temp64_1 = llTimestamp ;
				Temp64_2 = SoundConv->OutFormat.nAvgBytesPerSec ;
				_MUL128_1( ( DWORD * )&Temp64_1, ( DWORD * )&Temp64_2, Temp128 ) ;

				Temp64_2 = 10000000 ;
				_DIV128_1( Temp128, ( DWORD * )&Temp64_2, ( DWORD * )&Temp64_1 ) ;

				BytesPos = ( int )Temp64_1 ;
			}

			if( BytesPos >= SoundConv->DestDataCompSizeAll )
			{
				break ;
			}
		}

		hr = pSample->ConvertToContiguousBuffer( &pBuffer ) ;
		if( hr != S_OK )
			goto ERR ;

		hr = pBuffer->Lock( &pAudioData, NULL, &cbBuffer ) ;
		if( hr != S_OK )
			goto ERR ;

		if( cbBuffer == 0 )
			goto ERR ;

		// サイズが足りない場合は拡張する
		if( cbBuffer > mfdata->SampleBufferBytes )
		{
			DXFREE( mfdata->pSampleBuffer ) ;
			mfdata->pSampleBuffer = NULL ;

			mfdata->SampleBufferBytes = cbBuffer + 4096 ;
			mfdata->pSampleBuffer = DXALLOC( mfdata->SampleBufferBytes ) ;
			if( mfdata->pSampleBuffer == NULL )
				goto ERR ;
		}

		_MEMCPY( mfdata->pSampleBuffer, pAudioData, cbBuffer ) ;
		mfdata->OutSampleBufferBytes = 0 ;
		mfdata->UseSampleBufferBytes = cbBuffer ;

		hr = pBuffer->Unlock() ;
		if( hr != S_OK )
			goto ERR ;
	}

	// デコードしたデータを出力
	{
		DWORD outsize ;

		outsize = mfdata->UseSampleBufferBytes - mfdata->OutSampleBufferBytes ;
		if( outsize > ( DWORD )SoundConv->DestDataSize )
		{
			outsize = ( DWORD )SoundConv->DestDataSize ;
		}
		outsize = ( outsize + SoundConv->OutFormat.nBlockAlign - 1 ) / SoundConv->OutFormat.nBlockAlign * SoundConv->OutFormat.nBlockAlign ;
		_MEMCPY( SoundConv->DestData, ( BYTE * )mfdata->pSampleBuffer + mfdata->OutSampleBufferBytes, outsize ) ;

		mfdata->OutSampleBufferBytes += outsize ;
		SoundConv->DestDataValidSize = outsize ;
	}

	if( pSample != NULL		){		pSample->Release() ;		pSample = NULL ; }
	if( pBuffer != NULL		){		pBuffer->Release() ;		pBuffer = NULL ; }

	return 0 ;

ERR :
	if( pSample != NULL		){		pSample->Release() ;		pSample = NULL ; }
	if( pBuffer != NULL		){		pBuffer->Release() ;		pBuffer = NULL ; }

	return -1 ;
}

//extern	int SetTimeSoundConvert_MF(      SOUNDCONV *SoundConv, int Time ) ;								// 変換処理を位置を変更する( ミリ秒単位 )

// 変換処理の位置を変更する( サンプル単位 )
extern	int SetSampleTimeSoundConvert_MF( SOUNDCONV *SoundConv, int SampleTime )
{
	SOUNDCONV_MF *mfdata = (SOUNDCONV_MF *)SoundConv->ConvFunctionBuffer ;
	D_PROPVARIANT variant ;

	_MEMSET( &variant, 0, sizeof( variant ) ) ;
	variant.vt = D_VT_I8 ;
	variant.hVal.QuadPart = ( ULONGLONG )SampleTime * 10000000 / SoundConv->OutFormat.nSamplesPerSec ;
	mfdata->pReader->SetCurrentPosition( _GUID_NULL, variant ) ;

	mfdata->OutSampleBufferBytes = 0 ;
	mfdata->UseSampleBufferBytes = 0 ;

	SoundConv->DestDataValidSize = 0 ;
	SoundConv->DestDataCompSize = 0 ;
	SoundConv->DestDataCompSizeAll = SampleTime * SoundConv->OutFormat.nBlockAlign ;

	// 正常終了
	return 0 ;
}

// 変換後の大凡のデータサイズを得る
extern	int GetSoundConvertDestSize_Fast_MF( SOUNDCONV *SoundConv )
{
	SOUNDCONV_MF *mfdata = (SOUNDCONV_MF *)SoundConv->ConvFunctionBuffer ;

	return ( int )( mfdata->Duration * ( ULONGLONG )SoundConv->OutFormat.nAvgBytesPerSec / ( ULONGLONG )10000000 ) ;
}

#endif // DX_NON_MEDIA_FOUNDATION






#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE


#endif // DX_NON_SOUND
