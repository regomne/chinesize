// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		サウンドデータ変換プログラム
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

#include "DxCompileConfig.h"

#ifndef DX_NON_SOUND

// インクルード ------------------------------------------------------------------
#include "DxSoundConvert.h"
#include "DxStatic.h"
#include "DxLog.h"

#include "DxBaseFunc.h"
#include "DxMemory.h"
#include "DxUseCLib.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

// 構造体定義 --------------------------------------------------------------------

// 内部大域変数宣言 --------------------------------------------------------------

// サウンド変換処理全体で使用するデータ
SOUNDCONVERTDATA GSoundConvertData ;

// 関数プロトタイプ宣言-----------------------------------------------------------

static	int CheckNormalWaveFormat( STREAMDATA *Stream ) ;											// 通常のＰＣＭ形式か調べる( TRUE:ＰＣＭ形式  FALSE:違う )

static	int SetupSoundConvert_WAVE(            SOUNDCONV *SoundConv ) ;								// ＷＡＶＥファイルのセットアップ処理を行う( [戻] -1:エラー )
static	int TerminateSoundConvert_WAVE(        SOUNDCONV *SoundConv ) ;								// ＷＡＶＥファイルの後始末処理を行う
static	int ConvertProcessSoundConvert_WAVE(   SOUNDCONV *SoundConv ) ;								// 変換後のバッファにデータを補充する
//static	int SetTimeSoundConvert_WAVE(      SOUNDCONV *SoundConv, int Time ) ;					// 変換処理の位置を変更する( ミリ秒単位 )
static	int SetSampleTimeSoundConvert_WAVE(    SOUNDCONV *SoundConv, int SampleTime ) ;				// 変換処理の位置を変更する( サンプル単位 )
static	int GetSoundConvertDestSize_Fast_WAVE( SOUNDCONV *SoundConv ) ;								// 変換後の大凡のデータサイズを得る
static	int GetSoundConvertLoopAreaInfo_WAVE(  SOUNDCONV *SoundConv, int *LoopStartPos, int *LoopEndPos ) ;	// ループ情報を取得( [戻] -1:エラー )

// プログラム --------------------------------------------------------------------

// 初期化・終了関数

// サウンドデータ変換処理の初期化を行う
extern	int InitializeSoundConvert( void )
{
	// 既に初期化済みの場合は何もしない
	if( GSoundConvertData.InitializeFlag == TRUE )
		return -1 ;

	// 環境依存処理
	if( InitializeSoundConvert_PF() < 0 )
	{
		return -1 ;
	}

	// 初期化完了フラグを立てる
	GSoundConvertData.InitializeFlag = TRUE ;

	// 正常終了
	return 0 ;
}

// サウンドデータ変換処理の終了処理を行う
extern	int TerminateSoundConvert( void )
{
	// 既に後始末処理済みの場合は何もしない
	if( GSoundConvertData.InitializeFlag == FALSE )
		return -1 ;

	// 環境依存処理
	if( TerminateSoundConvert_PF() < 0 )
	{
		return -1 ;
	}

	// 初期化完了フラグを倒す
	GSoundConvertData.InitializeFlag = FALSE ;

	// 正常終了
	return 0 ;
}

// 変換処理のセットアップ( [戻] -1:エラー )
extern	int SetupSoundConvert( SOUNDCONV *SoundConv, STREAMDATA *Stream, int DisableReadSoundFunctionMask, int OggVorbisBitDepth, int OggVorbisFromTheoraFile )
{
	// 試す
#ifndef DX_NON_OGGVORBIS
	SoundConv->OggVorbisBitDepth = OggVorbisBitDepth ;
	SoundConv->OggVorbisFromTheoraFile = OggVorbisFromTheoraFile ;
#endif // DX_NON_OGGVORBIS
	SoundConv->Stream = *Stream ;
	SoundConv->SeekLockPosition = 0 ;
	Stream->ReadShred.Seek( Stream->DataPoint, 0, SEEK_SET ) ;
	if( ( DisableReadSoundFunctionMask & DX_READSOUNDFUNCTION_PCM ) == 0 )
	{
		if( SetupSoundConvert_WAVE( SoundConv ) == 0 ) goto R1 ;
		Stream->ReadShred.Seek( Stream->DataPoint, 0, SEEK_SET ) ;
	}
#ifndef DX_NON_OGGVORBIS
	if( ( DisableReadSoundFunctionMask & DX_READSOUNDFUNCTION_OGG ) == 0 )
	{
		if( SetupSoundConvert_OGG( SoundConv ) == 0 ) goto R1 ;
		Stream->ReadShred.Seek( Stream->DataPoint, 0, SEEK_SET ) ;
	}
#endif // DX_NON_OGGVORBIS
#ifndef DX_NON_OPUS
	if( ( DisableReadSoundFunctionMask & DX_READSOUNDFUNCTION_OPUS ) == 0 )
	{
		if( SetupSoundConvert_OPUS( SoundConv ) == 0 ) goto R1 ;
		Stream->ReadShred.Seek( Stream->DataPoint, 0, SEEK_SET ) ;
	}
#endif // DX_NON_OPUS
	if( SetupSoundConvert_PF( SoundConv, Stream, DisableReadSoundFunctionMask ) == 0 )
	{
		goto R1 ;
	}
	return -1 ;
	
R1 :
	// 共通初期化処理を行う
	SoundConv->InitializeFlag = FALSE ;
	SoundConv->EndFlag = FALSE ;
	SoundConv->DestData = DXALLOC( ( size_t )SoundConv->DestDataSize ) ;
	if( SoundConv->DestData == NULL )
	{
		TerminateSoundConvert( SoundConv ) ;
		return -1 ;
	}
	SoundConv->DestDataValidSize = 0 ;
	SoundConv->DestDataCompSize = 0 ;
	SoundConv->DestDataCompSizeAll = 0 ;

	// 初期化完了
	SoundConv->InitializeFlag = TRUE ;

	// 終了
	return 0 ;
}

// 変換処理の位置を変更する( サンプル単位 )
extern int SetSampleTimeSoundConvert( SOUNDCONV *SoundConv, int SampleTime )
{
	int res = -1 ;
	int BytePos ;

	BytePos = SampleTime * SoundConv->OutFormat.nBlockAlign ;
	if( _ABS( BytePos - SoundConv->DestDataCompSizeAll ) < 16 ) return 0 ;

	switch( SoundConv->MethodType )
	{
	case SOUND_METHODTYPE_NORMAL : res = SetSampleTimeSoundConvert_WAVE( SoundConv, SampleTime ) ; break ;
#ifndef DX_NON_OGGVORBIS
	case SOUND_METHODTYPE_OGG : res = SetSampleTimeSoundConvert_OGG( SoundConv, SampleTime ) ; break ;
#endif
#ifndef DX_NON_OPUS
	case SOUND_METHODTYPE_OPUS : res = SetSampleTimeSoundConvert_OPUS( SoundConv, SampleTime ) ; break ;
#endif
	default: res = SetSampleTimeSoundConvert_PF( SoundConv, SampleTime ) ; break ;
	}
	SoundConv->EndFlag = FALSE ;
	return res ;
}

// シーク予定の位置をセットしておく(ＡＣＭの為に)
extern	int SetupSeekPosSoundConvert( SOUNDCONV *SoundConv, int SeekPos )
{
	SoundConv->SeekLockPosition = SeekPos * SoundConv->OutFormat.nBlockAlign ;

	return 0 ;
}


// 指定のサイズ分だけ音声を解凍( [戻] -1:エラー  0以上:変換したサイズ )
extern	int RunSoundConvert( SOUNDCONV *SoundConv, void *DestBuffer, int DestSize )
{
	int pos, movesize, res = -1, copysize ;
	
	if( SoundConv->InitializeFlag == FALSE ) return -1 ;
	if( SoundConv->EndFlag == TRUE ) return 0 ;
	
	pos = 0 ;
	copysize = 0 ;
	while( DestSize != 0 )
	{
		if( SoundConv->DestDataValidSize == 0 )
		{
			SoundConv->DestDataCompSize = 0 ;
			switch( SoundConv->MethodType )
			{
			case SOUND_METHODTYPE_NORMAL : res = ConvertProcessSoundConvert_WAVE( SoundConv ) ; break ;
#ifndef DX_NON_OGGVORBIS
			case SOUND_METHODTYPE_OGG : res = ConvertProcessSoundConvert_OGG( SoundConv ) ; break ;
#endif
#ifndef DX_NON_OPUS
			case SOUND_METHODTYPE_OPUS : res = ConvertProcessSoundConvert_OPUS( SoundConv ) ; break ;
#endif
			default : res = ConvertProcessSoundConvert_PF( SoundConv ) ; break ;
			}
			if( res == -1 ) break ;
		}
	
		movesize = DestSize ;
		if( movesize > SoundConv->DestDataValidSize ) movesize = SoundConv->DestDataValidSize ;
/*
		int i ;
		int tmp ;
		short *dst, *src ;

		for( i = 0 ; i < movesize / 4 ; i ++ )
		{
			dst = ( short * )( (BYTE *)DestBuffer + pos + i * 4 ) ;
			src = ( short * )( (BYTE *)SoundConv->DestData + SoundConv->DestDataCompSize + i * 4 ) ;

			tmp = src[ 0 ] * 3 / 2 ;
			if( tmp >  32767 ) tmp =  32767 ;
			if( tmp < -32768 ) tmp = -32768 ;
			dst[ 0 ] = ( short )tmp ;

//			NS_DrawPixel( i, ( tmp + 32768 ) / 256, GetColor( 255,255,255 ) ) ;

			tmp = src[ 1 ] * 3 / 2 ;
			if( tmp >  32767 ) tmp =  32767 ;
			if( tmp < -32768 ) tmp = -32768 ;
			dst[ 1 ] = ( short )tmp ;
		}
//		NS_ScreenFlip() ;
//		WaitKey() ;
*/
		_MEMCPY( (BYTE *)DestBuffer + pos,
				(BYTE *)SoundConv->DestData + SoundConv->DestDataCompSize,
				( size_t )movesize ) ;
		SoundConv->DestDataValidSize -= movesize ;
		SoundConv->DestDataCompSize += movesize ;
		SoundConv->DestDataCompSizeAll += movesize ;
		
		DestSize -= movesize ;
		pos += movesize ;
		copysize += movesize ;
	}
	if( DestSize != 0 )
	{
		SoundConv->EndFlag = TRUE ;
	}

	// 終了
	return copysize ;
}

// 変換処理の後始末を行う
extern	int TerminateSoundConvert( SOUNDCONV *SoundConv )
{
	if( SoundConv->InitializeFlag == FALSE ) return -1 ;
	
	if( SoundConv->DestData != NULL )
	{
		DXFREE( SoundConv->DestData ) ;
		SoundConv->DestData = NULL ;
	}
	
	switch( SoundConv->MethodType )
	{
	case SOUND_METHODTYPE_NORMAL : TerminateSoundConvert_WAVE( SoundConv ) ; break ;
#ifndef DX_NON_OGGVORBIS
	case SOUND_METHODTYPE_OGG : TerminateSoundConvert_OGG( SoundConv ) ; break ;
#endif
#ifndef DX_NON_OPUS
	case SOUND_METHODTYPE_OPUS : TerminateSoundConvert_OPUS( SoundConv ) ; break ;
#endif
	default : TerminateSoundConvert_PF( SoundConv ) ; break ;
	}

	SoundConv->InitializeFlag = FALSE ;
	
	// 終了
	return 0 ;	
}

// 変換後の音声形式を取得( [戻] -1:エラー  0以上:変換後のサイズ )
extern	int GetOutSoundFormatInfo( SOUNDCONV *SoundConv, WAVEFORMATEX *OutWaveFormat )
{
	*OutWaveFormat = SoundConv->OutFormat ;
	return 0 ;
}

// ループ情報を取得( [戻] -1:エラー )
extern int GetSoundConvertLoopAreaInfo( SOUNDCONV *SoundConv, int *LoopStartPos, int *LoopEndPos )
{
	if( LoopStartPos != NULL ) *LoopStartPos = -1 ;
	if( LoopEndPos   != NULL ) *LoopEndPos   = -1 ;

	switch( SoundConv->MethodType )
	{
	case SOUND_METHODTYPE_NORMAL : return GetSoundConvertLoopAreaInfo_WAVE( SoundConv, LoopStartPos, LoopEndPos ) ;
#ifndef DX_NON_OGGVORBIS
	case SOUND_METHODTYPE_OGG : return GetSoundConvertLoopAreaInfo_OGG( SoundConv, LoopStartPos, LoopEndPos ) ;
#endif
	}
	return -1 ;
}

// 変換処理が終了しているかどうかを取得する( [戻] TRUE:終了した  FALSE:まだ終了していない )
extern	int GetSoundConvertEndState( SOUNDCONV *SoundConv )
{
	return SoundConv->EndFlag ;
}

// 変換後の大凡のデータサイズを得る
extern	int GetSoundConvertDestSize_Fast( SOUNDCONV *SoundConv )
{
	switch( SoundConv->MethodType )
	{
	case SOUND_METHODTYPE_NORMAL : return GetSoundConvertDestSize_Fast_WAVE( SoundConv ) ;
#ifndef DX_NON_OGGVORBIS
	case SOUND_METHODTYPE_OGG : return GetSoundConvertDestSize_Fast_OGG( SoundConv ) ;
#endif
#ifndef DX_NON_OPUS
	case SOUND_METHODTYPE_OPUS : return GetSoundConvertDestSize_Fast_OPUS( SoundConv ) ;
#endif
	default : return GetSoundConvertDestSize_Fast_PF( SoundConv ) ;
	}
//	return 0 ;
}

//extern void *SoundBuf ;
//extern int SoundSize ;

// 高速な一括変換
extern	int SoundConvertFast( SOUNDCONV *SoundConv, 
									WAVEFORMATEX *FormatP, void **DestBufferP, int *DestSizeP )
{
	void *Buffer ;
	int BufferSize ;

	// バッファの確保
	BufferSize = GetSoundConvertDestSize_Fast( SoundConv ) ;

	Buffer = DXALLOC( ( size_t )BufferSize ) ;
	if( Buffer == NULL ) return -1 ;

	// 変換位置のセット
//	SetTimeSoundConvert( SoundConv, 0 ) ;
	SetSampleTimeSoundConvert( SoundConv, 0 ) ;

	// データ形式が無圧縮ＰＣＭの場合のみそのままコピー
	if( SoundConv->MethodType == SOUND_METHODTYPE_NORMAL &&
		( ( SOUNDCONV_WAVE * )SoundConv->ConvFunctionBuffer )->SrcFormat.wFormatTag == WAVE_FORMAT_PCM )
	{
		SoundConv->Stream.ReadShred.Read( Buffer, ( size_t )BufferSize, 1, SoundConv->Stream.DataPoint ) ;
	}
	else
	{
		// それ以外の場合は普通に展開
		BufferSize = RunSoundConvert( SoundConv, Buffer, BufferSize ) ;
	}

	// 情報を保存
	*FormatP = SoundConv->OutFormat ;
	*DestBufferP = Buffer ;
	*DestSizeP = BufferSize ;
	
	// 終了
	return 0 ;
}















// フォーマット変換用

struct D_WAVE_SAMPLELOOP
{
	DWORD				dwIdentifier ;
	DWORD				dwType ;
	DWORD				dwStart ;
	DWORD				dwEnd ;
	DWORD				dwFraction ;
	DWORD				dwPlayCount ;
} ;

struct D_WAVE_SAMPLERCHUNK
{
	DWORD				dwManufacturer ;
	DWORD				dwProduct ;
	DWORD				dwSamplePeriod ;
	DWORD				dwMIDIUnityNote ;
	DWORD				dwMIDIPitchFraction ;
	DWORD				dwSMPTEFormat ;
	DWORD				dwSMPTEOffset ;
	DWORD				cSampleLoops ;
	DWORD				cbSamplerData ;
	D_WAVE_SAMPLELOOP	Loops[ 1 ] ;
} ;

// 通常のＰＣＭ形式か調べる( TRUE:ＰＣＭ形式  FALSE:違う )
extern	int CheckNormalWaveFormat( STREAMDATA *Stream )
{
	___RIFFCHUNK chunk ;
	char type[5] ;
	WAVEFORMAT format ;

	// RIFFファイルかどうか調べる
	Stream->ReadShred.Read( &chunk, sizeof( chunk ), 1, Stream->DataPoint ) ;
	if( chunk.fcc != RIFFCHUNKID( 'R', 'I', 'F', 'F' ) )
		return FALSE ;
		
	// WAVEファイルかどうか調べる
	type[4] = '\0' ;
	Stream->ReadShred.Read( type, 4, 1, Stream->DataPoint ) ;
	if( _STRCMP( type, "WAVE" ) != 0 )
		return FALSE ;
		
	// ノーマルなＰＣＭ形式か調べる
	Stream->ReadShred.Read( &chunk, sizeof( chunk ), 1, Stream->DataPoint ) ;
	if( chunk.fcc != RIFFCHUNKID( 'f', 'm', 't', ' ' ) )
		return FALSE ;

	Stream->ReadShred.Read( &format, sizeof( WAVEFORMAT ), 1, Stream->DataPoint ) ;
	if( format.wFormatTag != WAVE_FORMAT_PCM )
		return FALSE ;
		
	// ここまで来たら通常のフォーマットということ
	return TRUE ;
}

// ＷＡＶＥファイルのセットアップ処理を行う( [戻] -1:エラー )
static	int SetupSoundConvert_WAVE( SOUNDCONV *SoundConv )
{
	___RIFFCHUNK chunk ;
	char type[5] ;
	WAVEFORMATEX *format = NULL ;
	STREAMDATA *Stream = &SoundConv->Stream ;
	SOUNDCONV_WAVE *Wave = ( SOUNDCONV_WAVE * )SoundConv->ConvFunctionBuffer ;
	LONGLONG ChunkStart ;

	Wave->SrcBuffer = NULL ;
	Wave->LoopStart = -1 ;
	Wave->LoopEnd = -1 ;

	// RIFFファイルかどうか調べる
	Stream->ReadShred.Read( &chunk, sizeof( chunk ), 1, Stream->DataPoint ) ;
	if( chunk.fcc != RIFFCHUNKID( 'R', 'I', 'F', 'F' ) )
		goto ERR ;

	// WAVEファイルかどうか調べる
	type[4] = '\0' ;
	Stream->ReadShred.Read( type, 4, 1, Stream->DataPoint ) ;
	if( _STRCMP( type, "WAVE" ) != 0 )
		goto ERR ;

	// チャンクの開始位置を取得しておく
	ChunkStart = Stream->ReadShred.Tell( Stream->DataPoint ) ;

	// ノーマルなＰＣＭ形式か調べる
	{
		// フォーマットチャンクがあるまでスキップ
		while( Stream->ReadShred.Eof( Stream->DataPoint ) == 0 )
		{
			Stream->ReadShred.Read( &chunk, sizeof( chunk ), 1, Stream->DataPoint ) ;
			if( chunk.fcc == RIFFCHUNKID( 'f', 'm', 't', ' ' ) )
			{
				break ;
			}
			Stream->ReadShred.Seek( Stream->DataPoint, chunk.cb + ( ( chunk.cb % 2 != 0 ) ? 1 : 0 ) , SEEK_CUR ) ;
		}
		if( Stream->ReadShred.Eof( Stream->DataPoint ) != 0 )
			goto ERR ;

		format = (WAVEFORMATEX *)DXALLOC( chunk.cb ) ;
		if( format == NULL ) goto ERR ;

		// ヘッダの位置とサイズをセット
		SoundConv->HeaderPos = ( int )Stream->ReadShred.Tell( Stream->DataPoint ) ;
		SoundConv->HeaderSize = ( int )chunk.cb ;

		_MEMSET( SoundConv->ConvFunctionBuffer, 0, sizeof( SoundConv->ConvFunctionBuffer ) ) ;
		Wave->LoopStart = -1 ;
		Wave->LoopEnd = -1 ;

		_MEMSET( &SoundConv->OutFormat, 0, sizeof( SoundConv->OutFormat ) ) ;
		Stream->ReadShred.Read( format, chunk.cb, 1, Stream->DataPoint ) ;
		switch( format->wFormatTag )
		{
		case WAVE_FORMAT_PCM :

			// 形式が WAVEFORMATEX ではなかった場合は補完する
			_MEMCPY( &SoundConv->OutFormat, format, sizeof( SoundConv->OutFormat ) < chunk.cb ? sizeof( SoundConv->OutFormat ) : chunk.cb ) ;
			if( chunk.cb == 16 )
			{
				SoundConv->OutFormat.wBitsPerSample = ( WORD )( format->nBlockAlign / format->nChannels * 8 ) ;
				SoundConv->OutFormat.cbSize = 0 ;
			}
			break ;


		case WAVE_FORMAT_IEEE_FLOAT :

			// 出力フォーマットをセット
			SoundConv->OutFormat.nSamplesPerSec  = format->nSamplesPerSec ;
			SoundConv->OutFormat.nChannels       = format->nChannels ;
			SoundConv->OutFormat.wBitsPerSample  = 16 ;
			SoundConv->OutFormat.wFormatTag      = WAVE_FORMAT_PCM ;
			SoundConv->OutFormat.nBlockAlign     = ( WORD )( SoundConv->OutFormat.wBitsPerSample  * SoundConv->OutFormat.nChannels / 8 ) ;
			SoundConv->OutFormat.nAvgBytesPerSec = SoundConv->OutFormat.nBlockAlign * SoundConv->OutFormat.nSamplesPerSec ;

			Wave->SrcSampleNum = SoundConv->OutFormat.nSamplesPerSec ;
			Wave->SrcBufferSize = format->nAvgBytesPerSec ;
			Wave->SrcBuffer = ( BYTE * )DXALLOC( Wave->SrcBufferSize ) ;
			if( Wave->SrcBuffer == NULL )
				goto ERR ;
			break ;


		default :
			goto ERR ;
		}

		// フォーマットのコピー
		_MEMCPY( &Wave->SrcFormat, format, sizeof( Wave->SrcFormat ) < chunk.cb ? sizeof( Wave->SrcFormat ) : chunk.cb ) ;

		// メモリの解放
		DXFREE( format ) ;
		format = NULL ;
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

	SoundConv->DataPos = ( int )Stream->ReadShred.Tell( Stream->DataPoint ) ;
	SoundConv->DataSize = ( int )( chunk.cb / SoundConv->OutFormat.nBlockAlign * SoundConv->OutFormat.nBlockAlign ) ;

	// 変換後のＰＣＭデータを一時的に保存するメモリ領域のサイズをセット
	SoundConv->DestDataSize = ( int )SoundConv->OutFormat.nAvgBytesPerSec ;

	// smplチャンクを探す
	{
		LONGLONG NowPosition = Stream->ReadShred.Tell( Stream->DataPoint ) ;
		int Flag = FALSE ;

		Stream->ReadShred.Seek( Stream->DataPoint, chunk.cb + ( ( chunk.cb % 2 != 0 ) ? 1 : 0 ) , SEEK_CUR ) ; 
		while( Stream->ReadShred.Eof( Stream->DataPoint ) == 0 )
		{
			Stream->ReadShred.Read( &chunk , sizeof( chunk ) , 1 , Stream->DataPoint ) ;
			if( chunk.fcc == RIFFCHUNKID( 's', 'm', 'p', 'l' ) )
			{
				Flag = TRUE ;
				break ;
			}
			if( Stream->ReadShred.Eof( Stream->DataPoint ) )
			{
				break ;
			}
			Stream->ReadShred.Seek( Stream->DataPoint, chunk.cb + ( ( chunk.cb % 2 != 0 ) ? 1 : 0 ) , SEEK_CUR ) ; 
		}
		if( Flag == FALSE )
		{
			Stream->ReadShred.Seek( Stream->DataPoint, ChunkStart, SEEK_SET ) ;
			while( Stream->ReadShred.Eof( Stream->DataPoint ) == 0 &&
				   Stream->ReadShred.Tell( Stream->DataPoint ) < NowPosition )
			{
				Stream->ReadShred.Read( &chunk , sizeof( chunk ) , 1 , Stream->DataPoint ) ;
				if( chunk.fcc == RIFFCHUNKID( 's', 'm', 'p', 'l' ) )
				{
					Flag = TRUE ;
					break ;
				}
				if( Stream->ReadShred.Eof( Stream->DataPoint ) )
				{
					break ;
				}
				Stream->ReadShred.Seek( Stream->DataPoint, chunk.cb + ( ( chunk.cb % 2 != 0 ) ? 1 : 0 ) , SEEK_CUR ) ; 
			}
		}
		if( Flag )
		{
			D_WAVE_SAMPLERCHUNK *smpl ;

			smpl = ( D_WAVE_SAMPLERCHUNK * )DXALLOC( chunk.cb ) ;
			if( smpl == NULL ) goto ERR ;

			Stream->ReadShred.Read( smpl, chunk.cb, 1, Stream->DataPoint ) ;
			if( chunk.cb - ( sizeof( D_WAVE_SAMPLERCHUNK ) - sizeof( D_WAVE_SAMPLELOOP ) ) >= sizeof( D_WAVE_SAMPLELOOP ) )
			{
				Wave->LoopStart = ( int )smpl->Loops[ 0 ].dwStart ;
				Wave->LoopEnd   = ( int )smpl->Loops[ 0 ].dwEnd ;
			}

			DXFREE( smpl ) ;
		}

		Stream->ReadShred.Seek( Stream->DataPoint, SoundConv->DataPos , SEEK_SET ) ; 
	}


	// タイプセット
	SoundConv->MethodType = SOUND_METHODTYPE_NORMAL ;

	// 正常終了
	return 0 ;

ERR :
	if( Wave->SrcBuffer != NULL )
	{
		DXFREE( Wave->SrcBuffer ) ;
		Wave->SrcBuffer = NULL ;
	}

	if( format != NULL )
	{
		DXFREE( format ) ;
		format = NULL ;
	}

	return -1 ;
}




// ＷＡＶＥファイルの後始末処理を行う
static	int TerminateSoundConvert_WAVE( SOUNDCONV *SoundConv )
{
	SOUNDCONV_WAVE *Wave = ( SOUNDCONV_WAVE * )SoundConv->ConvFunctionBuffer ;

	// メモリ解放
	if( Wave->SrcBuffer )
	{
		DXFREE( Wave->SrcBuffer ) ;
		Wave->SrcBuffer = NULL ;
	}

	// 終了
	return 0 ;
}


// 変換後のバッファにデータを補充する
static	int ConvertProcessSoundConvert_WAVE( SOUNDCONV *SoundConv )
{
	int readsize, pos ;
	STREAMDATA *Stream = &SoundConv->Stream ;
	SOUNDCONV_WAVE *Wave = ( SOUNDCONV_WAVE * )SoundConv->ConvFunctionBuffer ;

	switch( Wave->SrcFormat.wFormatTag )
	{
	case WAVE_FORMAT_PCM :
		pos = ( int )( Stream->ReadShred.Tell( Stream->DataPoint ) - SoundConv->DataPos ) ;
		if( pos == SoundConv->DataSize ) return -1 ;

		// 読み込むデータサイズを決定する
		readsize = SoundConv->DataSize - pos ;
		if( SoundConv->DestDataSize < readsize ) readsize = SoundConv->DestDataSize ;

		// 読み込む
		Stream->ReadShred.Read( SoundConv->DestData,
								( size_t )readsize, 1, Stream->DataPoint ) ;
		SoundConv->DestDataValidSize = readsize ;
		break ;

	case WAVE_FORMAT_IEEE_FLOAT :
		{
			short *Dest ;
			float *Src ;
			DWORD i ;
			DWORD SampleNum ;
			int j ;
			int DestI ;

			pos = ( int )( Stream->ReadShred.Tell( Stream->DataPoint ) - SoundConv->DataPos ) ;
			if( pos == SoundConv->DataSize ) return -1 ;

			// 読み込むデータサイズを決定する
			readsize = SoundConv->DataSize - pos ;
			if( Wave->SrcBufferSize < ( DWORD )readsize ) readsize = ( int )Wave->SrcBufferSize ;

			// 読み込む
			Stream->ReadShred.Read( Wave->SrcBuffer,
									( size_t )readsize, 1, Stream->DataPoint ) ;

			// 整数型に変換
			Dest = ( short * )SoundConv->DestData ;
			Src = ( float * )Wave->SrcBuffer ;
			SampleNum = ( DWORD )( readsize / Wave->SrcFormat.nBlockAlign ) ;
			for( i = 0 ; i < SampleNum ; i ++ )
			{
				for( j = 0 ; j < Wave->SrcFormat.nChannels ; j ++, Dest ++, Src ++ )
				{
					DestI = _FTOL( *Src * 32768.0f ) ;
					if( DestI < -32768 )
					{
						*Dest = -32768 ;
					}
					else
					if( DestI > 32767 )
					{
						*Dest = 32767 ;
					}
					else
					{
						*Dest = ( short )DestI ;
					}
				}
			}
			SoundConv->DestDataValidSize = ( int )( SampleNum * SoundConv->OutFormat.nBlockAlign ) ;
		}
		break ;
	}

	return 0 ;
}

// 変換処理の位置を変更する( サンプル単位 )
static int SetSampleTimeSoundConvert_WAVE( SOUNDCONV *SoundConv, int SampleTime )
{
	int BytePos ;
	STREAMDATA *Stream = &SoundConv->Stream ;
	
	BytePos = SampleTime * SoundConv->OutFormat.nBlockAlign + SoundConv->DataPos ;
	Stream->ReadShred.Seek( Stream->DataPoint, BytePos, SEEK_SET ) ;

	SoundConv->DestDataValidSize = 0 ;
	SoundConv->DestDataCompSize = 0 ;
	SoundConv->DestDataCompSizeAll = BytePos ;

	return 0 ;
}


// 変換後の大凡のデータサイズを得る
static	int GetSoundConvertDestSize_Fast_WAVE( SOUNDCONV *SoundConv )
{
	SOUNDCONV_WAVE *Wave = ( SOUNDCONV_WAVE * )SoundConv->ConvFunctionBuffer ;

	switch( Wave->SrcFormat.wFormatTag )
	{
	case WAVE_FORMAT_PCM :
		return SoundConv->DataSize ;

	case WAVE_FORMAT_IEEE_FLOAT :
		return SoundConv->DataSize / ( Wave->SrcFormat.wBitsPerSample / SoundConv->OutFormat.wBitsPerSample ) ;
	}

	return -1 ;
}


// ループ情報を取得( [戻] -1:エラー )
static	int GetSoundConvertLoopAreaInfo_WAVE(  SOUNDCONV *SoundConv, int *LoopStartPos, int *LoopEndPos )
{
	SOUNDCONV_WAVE *Wave = ( SOUNDCONV_WAVE * )SoundConv->ConvFunctionBuffer ;

	// ループ情報がない場合はエラー
	if( Wave->LoopStart < 0 || Wave->LoopEnd < 0 )
	{
		return -1 ;
	}

	if( LoopStartPos != NULL ) *LoopStartPos = Wave->LoopStart ;
	if( LoopEndPos   != NULL ) *LoopEndPos   = Wave->LoopEnd ;

	// 正常終了
	return 0 ;
}


#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE


#endif // DX_NON_SOUND

