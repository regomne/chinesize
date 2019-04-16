// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		ＤｉｒｅｃｔＳｏｕｎｄ制御プログラム
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

#include "DxSound.h"

#ifndef DX_NON_SOUND

// インクルード ------------------------------------------------------------------
#include "DxFile.h"
#include "DxBaseFunc.h"
#include "DxSystem.h"
#include "DxMemory.h"
#include "DxUseCLib.h"
#include "DxASyncLoad.h"
#include "DxLog.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 -------------------------------------

// ソフトサウンド関係の定義
#define SSND_PLAYER_SEC_DIVNUM				(256)		// １秒の分割数
#define SSND_PLAYER_STRM_BUFSEC				(256)		// プレイヤーのサウンドバッファのサイズ( 1 / SSND_PLAYER_SEC_DIVNUM　秒 )
#define SSND_PLAYER_STRM_ONECOPYSEC			(8)			// プレイヤーのサウンドバッファのサイズ( 1 / SSND_PLAYER_SEC_DIVNUM　秒 )
#define SSND_PLAYER_STRM_SAKICOPYSEC		(256 / 5)	// プレイヤーの再生時に音声データを先行展開しておく最大時間( 単位は SSND_PLAYER_SEC_DIVNUM分の1秒 )
#define SSND_PLAYER_STRM_MINSAKICOPYSEC		(256 / 20)	// プレイヤーの再生時に音声データを先行展開しておく最小時間( 単位は SSND_PLAYER_SEC_DIVNUM分の1秒 )

// ストリーム関係の定義
#define STS_DIVNUM							(256)		// １秒の分割数
#define STS_BUFSEC							(256)		// ストリーム風サウンド再生時に確保するサウンドバッファの時間( 単位は STS_DIVNUM分の1秒 )
#define STS_ONECOPYSEC						(8)			// ストリーム風サウンド再生時に一度に転送するサウンドデータの長さ( 1/STS_ONECOPYSEC 秒 )
#define STS_ADVANCECOPYSEC					(192)		// ストリーム風サウンド再生時に音声データを先行展開しておく最大時間( 単位は STS_DIVNUM分の1秒 )
#define STS_MINADVANCECOPYSEC				(96)		// ストリーム風サウンド再生時に音声データを先行展開しておく最小時間( 単位は STS_DIVNUM分の1秒 )
#define STS_CACHEBUFFERSEC					(8)			// ストリーム風サウンド再生時に、ファイルからのストリームのときにメモリ上に確保する一時バッファの長さ( 1/STS_CACHEBUFFERSEC 秒 )

#define DSMP3ONEFILLSEC						(8)			// DirectShow を使ったＭＰ３変換のフラグ一つ辺りがカバーする時間( 1/DSMP3ONEFILLSEC 秒 )

#define SOUNDSIZE( size, align )			( ( ( size ) + ( align - 1 ) ) / ( align ) * ( align ) )

// キャプチャで保存するサウンドのサンプリングレート
#define CAPTURE_SAMPLERATE					(44100)

// 標準ＷＡＶＥファイルのヘッダのサイズと標準ＷＡＶＥファイルの WAVEFORMATEX 構造体のサイズ
#define NORMALWAVE_HEADERSIZE				(46)
#define NORMALWAVE_FORMATSIZE				(18)


#define DX_DSBVOLUME_MIN					(-10000)
#define DX_DSBVOLUME_MAX					(0)

// ビープ音用のサウンドバッファの再生周波数
#define SOUND_BEEP_SAMPLEPERSEC				(44100)

// ビープ音用のサウンドバッファの１サンプルあたりのバイト数
#define SOUND_BEEP_SAMPLEPERBYTES			(2)

// ビープ音用のサウンドバッファの長さ
#define SOUND_BEEP_SOUNDBUFFER_SAMPLECOUNT	(SOUND_BEEP_SAMPLEPERSEC / 2)

// ビープ音用のサウンドで一度に書き込むサンプル数
#define SOUND_BEEP_ONE_WRITE_SAMPLES		(SOUND_BEEP_SOUNDBUFFER_SAMPLECOUNT / 4)

// デフォルトのビープ音周波数
#define SOUND_DEFAULT_BEEP_FREQUENCY		(1500)

// ビープ音を再生してからサウンドバッファの音量を上げるまでの時間
#define SOUND_BEEP_PLAY_VOLUME_DELAY		(16)

// ビープ音を停止してから実際にサウンドバッファの再生を停止するまでの時間
#define SOUND_BEEP_STOP_DELAY				(16)

// 構造体定義 --------------------------------------------------------------------

// 内部大域変数宣言 --------------------------------------------------------------

// サウンドシステムデータ
SOUNDSYSTEMDATA SoundSysData ;

// ＭＩＤＩ管理用データ
MIDISYSTEMDATA MidiSystemData ;	

// 関数プロトタイプ宣言-----------------------------------------------------------

static  int GetSoundBuffer( int SoundHandle, SOUND * Sound, SOUNDBUFFER **BufferP = NULL, bool BufferGet = true ) ;	// 使用可能なサウンドバッファを取得する(-1:サウンドバッファの使用権を得られなかった)
static	int CreateSoundBuffer( WAVEFORMATEX *WaveFormat, DWORD BufferSize, 
									 int SoundType, int BufferNum,
									 int SoundHandle, int SrcSoundHandle = -1, int ASyncThread = FALSE ) ;			// 新しいサウンドバッファの作成



// 補助系関数
static	int AdjustSoundDataBlock( int Length, SOUND * Sound ) ;										// サイズ補正

// ソフトウエアで制御するサウンドデータハンドル関係
//static int AddSoftSoundData( int IsPlayer, int Channels, int BitsPerSample, int SamplesPerSec, int SampleNum ) ;	// ソフトウエアで制御するサウンドデータハンドルの追加
//static int SubSoftSoundData( int SoftSoundHandle, int CheckIsPlayer ) ;											// ソフトウエアで制御するサウンドデータハンドルの削除
static	int _SoftSoundPlayerProcess( SOFTSOUND *SPlayer ) ;														// ソフトウエアで制御するサウンドデータプレイヤーの定期処理

// 補助系
extern	int CreateWaveFileImage( 	void **DestBufferP, int *DestBufferSizeP,
									const WAVEFORMATEX *Format, int FormatSize,
									const void *WaveData, int WaveDataSize ) ;											// 音声フォーマットと波形イメージからＷＡＶＥファイルイメージを作成する
static	int FileFullRead( const wchar_t *FileName, void **BufferP, int *SizeP, int OutputFileOpenErrorLog = TRUE ) ;	// ファイルを丸まるメモリに読み込む
extern	int StreamFullRead( STREAMDATA *Stream, void **BufferP, int *SizeP ) ;											// ストリームデータを丸まるメモリに読み込む
static	int SoundTypeChangeToStream( int SoundHandle ) ;																// サウンドハンドルのタイプがストリームハンドルではない場合にストリームハンドルに変更する
static	int MilliSecPositionToSamplePosition( int SamplesPerSec, int MilliSecTime ) ;									// ミリ秒単位の数値をサンプル単位の数値に変換する
static	int SamplePositionToMilliSecPosition( int SamplesPerSec, int SampleTime ) ;										// サンプル単位の数値をミリ秒単位の数値に変換する
static	int _PlaySetupSoundMem( SOUND * Sound, int TopPositionFlag ) ;													// サウンドハンドルの再生準備を行う( -1:エラー 0:正常終了 1:再生する必要なし )
static	int	_PlaySetupStreamSoundMem( int SoundHandle, SOUND * Sound, int PlayType, int TopPositionFlag, SOUNDBUFFER **DestSBuffer ) ;	// ストリームサウンドハンドルの再生準備を行う( -1:エラー 0:正常終了 1:再生する必要なし )
static	int _CreateSoundHandle( int Is3DSound, int NotInitSoundMemDelete, int ASyncThread ) ;									// サウンドハンドルを作成する
static	int SetupSoundPitchRateTimeStretchRateChangeHandle( int SoundHandle, LOADSOUND_GPARAM *GParam, int SSSHandleI, int BufferNum, int ASyncThread ) ;		// ピッチ変更やタイムストレッチ時の伸縮変更サウンドハンドルをセットアップするヘルパー関数

#ifndef DX_NON_BEEP

// BEEP音関係
static	int	BeepSound_Initialize( void ) ;										// BEEP音関係の初期化を行う
static	int BeepSound_Terminate( void ) ;										// BEEP音関係の後始末を行う
static	int BeepSound_Process( void ) ;											// BEEP音関係の周期的処理を行う
static	int	BeepSound_FillSamples( int BufferIndex ) ;							// BEEP音の波形データを書き込む

#endif // DX_NON_BEEP

// プログラム --------------------------------------------------------------------
/*
static __inline void TableSin( int Angle, float * RST Sin )
{
	*Sin = SoundSysData.SinTable[ Angle & ( SOUND_SINTABLE_DIV - 1 ) ] ;
}

static __inline void TableCos( float Angle, float * RST Sin, float * RST Cos )
{
	if( Angle > 10.0f || Angle < -10.0f )
	{
		_SINCOS_PLATFORM( Angle, Sin, Cos ) ;
	}
	else
	{
		Angle = Angle * ( MV1_SINTABLE_DIV / ( ( float )DX_PI * 2 ) ) + 12582912.0f ;
		*Sin = MV1Man.SinTable[ *( ( DWORD * )&Angle ) & ( MV1_SINTABLE_DIV - 1 ) ] ;
		*Cos = MV1Man.SinTable[ ( *( ( DWORD * )&Angle ) + MV1_SINTABLE_DIV / 4 ) & ( MV1_SINTABLE_DIV - 1 ) ] ;
	}
}
*/
// サウンドシステムを初期化する
extern int InitializeSoundSystem( void )
{
	if( SoundSysData.InitializeFlag )
	{
		return -1 ;
	}

	// 出力レートをセット
	if( SoundSysData.OutputSmaplesPerSec == 0 )
	{
		SoundSysData.OutputSmaplesPerSec = 44100 ;
	}

	// ３Ｄサウンドの１メートルを設定
	if( SoundSysData._3DSoundOneMetreEnable == FALSE )
	{
		SoundSysData._3DSoundOneMetre = 1.0f ;
	}

	// サウンドハンドル管理情報初期化
	InitializeHandleManage( DX_HANDLETYPE_SOUND, sizeof( SOUND ), MAX_SOUND_NUM, InitializeSoundHandle, TerminateSoundHandle, L"Sound" ) ;

	// ソフトウエアで扱う波形データハンドル管理情報初期化
	InitializeHandleManage( DX_HANDLETYPE_SOFTSOUND, sizeof( SOFTSOUND ), MAX_SOFTSOUND_NUM, InitializeSoftSoundHandle, TerminateSoftSoundHandle, L"SoftSound" ) ;

	// ＭＩＤＩハンドル管理情報初期化
	InitializeHandleManage( DX_HANDLETYPE_MUSIC, sizeof( MIDIHANDLEDATA ), MAX_MUSIC_NUM, InitializeMidiHandle, TerminateMidiHandle, L"Music" ) ;

	// クリティカルセクションの初期化
	CriticalSection_Initialize( &SoundSysData._3DSoundListCriticalSection ) ;
	CriticalSection_Initialize( &SoundSysData.Play3DSoundListCriticalSection ) ;
	CriticalSection_Initialize( &SoundSysData.StreamSoundListCriticalSection ) ;

	// ハンドルリストを初期化
	InitializeHandleList( &SoundSysData._3DSoundListFirst,				&SoundSysData._3DSoundListLast ) ;
	InitializeHandleList( &SoundSysData.StreamSoundListFirst,			&SoundSysData.StreamSoundListLast ) ;
	InitializeHandleList( &SoundSysData.SoftSoundPlayerListFirst,		&SoundSysData.SoftSoundPlayerListLast ) ;
	InitializeHandleList( &SoundSysData.PlayFinishDeleteSoundListFirst, &SoundSysData.PlayFinishDeleteSoundListLast ) ;
	InitializeHandleList( &SoundSysData.Play3DSoundListFirst,			&SoundSysData.Play3DSoundListLast ) ;

	// サインテーブルの初期化
	{
		int i ;
		float Sin ;
		float Cos ;

		for( i = 0 ; i < SOUND_SINTABLE_DIV ; i ++ )
		{
			_SINCOS( i * DX_PI_F * 2 / SOUND_SINTABLE_DIV, &Sin, &Cos ) ;
			SoundSysData.SinTable[ i ] = ( short )_FTOL( Sin * 16384.0f ) ;
		}
	}

	// 環境依存処理
	if( InitializeSoundSystem_PF_Timing0() < 0 )
	{
		return -1 ;
	}

	// 作成する音のデータタイプをセット
	SoundSysData.CreateSoundDataType = DX_SOUNDDATATYPE_MEMNOPRESS ;

	// 作成する音のピッチレートをセット
	SoundSysData.CreateSoundPitchRateEnable = FALSE ;
	SoundSysData.CreateSoundPitchRate = 1.0f ;

	// 作成する音のタイムストレッチレートをセット
	SoundSysData.CreateSoundTimeStretchRateEnable = FALSE ;
	SoundSysData.CreateSoundTimeStretchRate = 1.0f ;

#ifndef DX_NON_OGGVORBIS
	// ＯｇｇＶｏｒｂｉｓのＰＣＭデコード時の、ビット深度を１６bitにセット
	SoundSysData.OggVorbisBitDepth = 2 ;
#endif // DX_NON_OGGVORBIS

	// ３Ｄサウンド処理用のリスナー情報を初期化
	SoundSysData.ListenerInfo.Position			= VGet( 0.0f, 0.0f, 0.0f ) ;
	SoundSysData.ListenerInfo.FrontDirection	= VGet( 0.0f, 0.0f, 1.0f ) ;
	SoundSysData.ListenerInfo.Velocity			= VGet( 0.0f, 0.0f, 0.0f ) ;
	SoundSysData.ListenerInfo.InnerAngle		= DX_PI_F * 5.0f / 6.0f ;
	SoundSysData.ListenerInfo.OuterAngle		= DX_PI_F * 11.0f / 6.0f ;
	SoundSysData.ListenerInfo.InnerVolume		= 1.0f ;
	SoundSysData.ListenerInfo.OuterVolume		= 0.75f ;
	SoundSysData.ListenerSideDirection			= VGet( 1.0f, 0.0f, 0.0f ) ;

	// 初期化フラグを立てる
	SoundSysData.InitializeFlag = TRUE ;

	NS_InitSoundMem() ;
	NS_InitSoftSound() ;
	NS_InitSoftSoundPlayer() ;

	// 環境依存処理
	if( InitializeSoundSystem_PF_Timing1() < 0 )
	{
		return -1 ;
	}

#ifndef DX_NON_BEEP
	// BEEP音の初期化
	if( BeepSound_Initialize() < 0 )
	{
		return -1 ;
	}
#endif //DX_NON_BEEP

	// 終了
	return 0 ;
}

// サウンドシステムの後始末をする
extern int TerminateSoundSystem( void )
{
	int i ;

	if( SoundSysData.InitializeFlag == FALSE )
	{
		return -1 ;
	}

	// 環境依存処理
	if( TerminateSoundSystem_PF_Timing0() < 0 )
	{
		return -1 ;
	}

#ifndef DX_NON_BEEP
	// ビープ音関係の後始末を行う
	BeepSound_Terminate() ;
#endif // DX_NON_BEEP

	// サウンドキャプチャ処理を終了する
	EndSoundCapture() ;

	// すべてのサウンドデータを解放する
	NS_InitSoundMem() ;

	// 全てのソフトサウンドデータを解放する
	NS_InitSoftSound() ;
	NS_InitSoftSoundPlayer() ;

	// すべてのＭＩＤＩデータを解放する
	NS_InitMusicMem() ;

	// サウンドハンドル管理情報の後始末
	TerminateHandleManage( DX_HANDLETYPE_SOUND ) ;

	// ソフトサウンドハンドル管理情報の後始末
	TerminateHandleManage( DX_HANDLETYPE_SOFTSOUND ) ;

	// 高速フーリエ変換の作業用バッファの開放
	for( i = 0 ; i < SOUND_FFT_BITCOUNT_MAX ; i ++ )
	{
		if( SoundSysData.FFTVibrationWorkBuffer[ i ] != NULL )
		{
			DXFREE( SoundSysData.FFTVibrationWorkBuffer[ i ] ) ;
			SoundSysData.FFTVibrationWorkBuffer[ i ] = NULL ;
		}
	}

	// ＭＩＤＩハンドル管理情報の後始末
	TerminateHandleManage( DX_HANDLETYPE_MUSIC ) ;

	// クリティカルセクションの削除
	CriticalSection_Delete( &SoundSysData._3DSoundListCriticalSection ) ;
	CriticalSection_Delete( &SoundSysData.Play3DSoundListCriticalSection ) ;
	CriticalSection_Delete( &SoundSysData.StreamSoundListCriticalSection ) ;

	// 環境依存処理
	if( TerminateSoundSystem_PF_Timing1() < 0 )
	{
		return -1 ;
	}

	// 初期化フラグを倒す
	SoundSysData.InitializeFlag = FALSE ;

	// 終了
	return 0 ;
}









// サウンドデータ管理系関数

// InitSoundMem でメモリに読み込んだWAVEデータを削除するかどうかをチェックする関数
static int DeleteCancelCheckInitSoundMemFunction( HANDLEINFO *HandleInfo )
{
	SOUND * Sound = ( SOUND * )HandleInfo ;

	return Sound->NotInitSoundMemDelete ;
}

// メモリに読みこんだWAVEデータを削除し、初期化する
extern int NS_InitSoundMem( int /*LogOutFlag*/ )
{
//	int Ret ;

	if( SoundSysData.InitializeFlag == FALSE )
		return -1 ;

	// ハンドルを初期化
	/*Ret =*/ AllHandleSub( DX_HANDLETYPE_SOUND, DeleteCancelCheckInitSoundMemFunction ) ;

	// PlayWav用サウンドハンドルも初期化
	SoundSysData.PlayWavSoundHandle = -1 ;

	// 終了
	return 0 ;
}

// 新しいサウンドデータ領域を確保する
extern int NS_AddSoundData( int Handle )
{
	int Result ;

	if( SoundSysData.InitializeFlag == FALSE )
	{
		return -1 ;
	}

	Result = AddHandle( DX_HANDLETYPE_SOUND, FALSE, Handle ) ;

	return Result ;
}

// サウンドハンドルの初期化
extern int InitializeSoundHandle( HANDLEINFO *HandleInfo )
{
	SOUND *Sound = ( SOUND * )HandleInfo ;
	int i ;

	// 初期化
	Sound->PresetReverbParam = 0 ;
	NS_Get3DPresetReverbParamSoundMem( &Sound->ReverbParam, DX_REVERB_PRESET_DEFAULT ) ;

	for( i = 0 ; i < SOUNDBUFFER_MAX_CHANNEL_NUM ; i ++ )
	{
		Sound->BaseVolume[ i ] = 0 ;
		Sound->BaseVolume8bit[ i ] = 255 ;
	}
	Sound->BasePan = 0 ;
	Sound->BaseFrequency = 0 ;
	Sound->Base3DPosition = VGet( 0.0f, 0.0f, 0.0f ) ;
	Sound->Base3DRadius = 14.0f ;
	Sound->Base3DVelocity = VGet( 0.0f, 0.0f, 0.0f ) ;
	Sound->PitchRateEnable = FALSE ;
	Sound->PitchRate = 0.0f ;

	// サウンドの再生が終了したらハンドルを自動的に削除するかどうかのフラグを倒す
	Sound->PlayFinishDeleteFlag = FALSE ;

	// ファイルを使用しているかどうかを初期化
	Sound->Stream.FileUseFile = FALSE ;

	// データの数を初期化
	Sound->Stream.FileNum = 0 ;

	// 終了
	return 0 ;
}

// サウンドハンドルの後始末
extern int TerminateSoundHandle( HANDLEINFO *HandleInfo )
{
	SOUND *Sound = ( SOUND * )HandleInfo ;
	int i ;

	// 音の再生を止める
	if( Sound->Type == DX_SOUNDTYPE_STREAMSTYLE )
	{
		NS_StopStreamSoundMem( HandleInfo->Handle ) ;
	}
	else
	{
		if( Sound->Buffer[0].Valid ) NS_StopSoundMem( HandleInfo->Handle ) ;
	}

	if( Sound->Is3DSound != FALSE )
	{
		// クリティカルセクションの取得
		CRITICALSECTION_LOCK( &SoundSysData._3DSoundListCriticalSection ) ;

		// ３Ｄサウンドだった場合は３Ｄサウンドリストから外す
		SubHandleList( &Sound->_3DSoundList ) ;

		// クリティカルセクションの解放
		CriticalSection_Unlock( &SoundSysData._3DSoundListCriticalSection ) ;

		// 再生中リストに追加されていたら外す
		if( Sound->AddPlay3DSoundList )
		{
			Sound->AddPlay3DSoundList = FALSE ;

			// クリティカルセクションの取得
			CRITICALSECTION_LOCK( &SoundSysData.Play3DSoundListCriticalSection ) ;

			SubHandleList( &Sound->Play3DSoundList ) ;

			// クリティカルセクションの解放
			CriticalSection_Unlock( &SoundSysData.Play3DSoundListCriticalSection ) ;
		}
	}

	// 再生が終了したら削除する設定になっていたら、再生が終了したら削除するサウンドのリストから外す
	if( Sound->PlayFinishDeleteFlag )
	{
		SubHandleList( &Sound->PlayFinishDeleteSoundList ) ;
	}

	// サウンドタイプによって処理を分岐
	switch( Sound->Type )
	{
	case DX_SOUNDTYPE_NORMAL :
		for( i = 0 ; i < Sound->ValidBufferNum ; i ++ )
		{
			if( Sound->Buffer[ i ].Valid ) SoundBuffer_Terminate( &Sound->Buffer[ i ] ) ;
		}

		// サウンドデータを解放する
		if( Sound->Normal.WaveDataUseCount )
		{
			*Sound->Normal.WaveDataUseCount = *Sound->Normal.WaveDataUseCount - 1 ;
			if( *Sound->Normal.WaveDataUseCount == 0 )
			{
				if( Sound->Normal.WaveData != NULL )
				{
					DXFREE( Sound->Normal.WaveData ) ;
				}
				DXFREE( Sound->Normal.WaveDataUseCount ) ;
			}
		}
		Sound->Normal.WaveDataUseCount = NULL ;
		Sound->Normal.WaveData = NULL ;
		break ;

	case DX_SOUNDTYPE_STREAMSTYLE :
		{
			STREAMFILEDATA *PlayData ;
			SOUND *UniSound = NULL ;
			
			// サウンドバッファを共有している場合は処理を分岐
			if( Sound->Stream.BufferBorrowSoundHandle != -1 &&
				SOUNDHCHK( Sound->Stream.BufferBorrowSoundHandle, UniSound ) == false )
			{
				// サウンドバッファを使用しているの数を減らす
				UniSound->Stream.BufferBorrowSoundHandleCount-- ;
				
				// カウントが０になっていて、且つ自分以外のサウンドハンドルで、
				// 削除待ちをしていた場合は削除する
				if( Sound->Stream.BufferBorrowSoundHandle != HandleInfo->Handle &&
					UniSound->Stream.DeleteWaitFlag == TRUE )
				{
					NS_DeleteSoundMem( Sound->Stream.BufferBorrowSoundHandle ) ;
				}
			}
			
			// サウンドバッファが他のハンドルで使われていたら削除待ち状態にする
			if( Sound->Stream.BufferBorrowSoundHandleCount != 0 )
			{
				Sound->Stream.DeleteWaitFlag = TRUE ;

				// 削除キャンセルの戻り値
				return 1 ;
			}

			// クリティカルセクションの取得
			CRITICALSECTION_LOCK( &SoundSysData.StreamSoundListCriticalSection ) ;

			// ストリームサウンドリストから外す
			SubHandleList( &Sound->Stream.StreamSoundList ) ;

			// クリティカルセクションの解放
			CriticalSection_Unlock( &SoundSysData.StreamSoundListCriticalSection ) ;

			// 登録されているすべてのサウンドデータの領域を解放
			PlayData = Sound->Stream.File ;
			for( i = 0 ; i < Sound->Stream.FileNum ; i ++, PlayData ++ )
			{
				TerminateSoundConvert( &PlayData->ConvData ) ;
			
				if( PlayData->FileImage != NULL )
					DXFREE( PlayData->FileImage ) ;

				if( Sound->Stream.FileLibraryLoadFlag[ i ] == TRUE &&
					PlayData->DataType == DX_SOUNDDATATYPE_FILE &&
					PlayData->ConvData.Stream.DataPoint )
					PlayData->ConvData.Stream.ReadShred.Close( PlayData->ConvData.Stream.DataPoint ) ;

				if( PlayData->MemStream.DataPoint != NULL )
					PlayData->MemStream.ReadShred.Close( PlayData->MemStream.DataPoint ) ;
			}

			if( Sound->Buffer[0].Valid ) SoundBuffer_Terminate( &Sound->Buffer[0] ) ;
		}
		break ;
	}

	// 終了
	return 0 ;
}


// 使用可能なサウンドバッファを取得する(-1:サウンドバッファの使用権を得られなかった)
static int GetSoundBuffer( int SoundHandle, SOUND * Sound, SOUNDBUFFER **BufferP, bool BufferGet )
{
	SOUND * UniSound = NULL ;

	// ストリームではなかったら使用権は得られる
	if( Sound->Type != DX_SOUNDTYPE_STREAMSTYLE )
	{
		if( BufferP ) *BufferP = &Sound->Buffer[ 0 ] ;
		return 0 ;
	}
	
	// 共有バッファを使用しているかどうかで分岐
	if( BufferP ) *BufferP = &Sound->Buffer[0] ;
	if( Sound->Stream.BufferBorrowSoundHandle == -1 )
	{
		// 使用していない場合は、使用されていなければ使用権を得られる
		if( Sound->Stream.BufferUseSoundHandle == -1 )
		{
			Sound->Stream.BufferUseSoundHandle = SoundHandle ;
			return 0 ;
		}
		
		// 使用されている場合で使用しているのが自分ではなく、
		// 使用権を得る指定も無い場合は使用権を得られない
		if( SoundHandle != Sound->Stream.BufferUseSoundHandle && BufferGet == false ) return -1 ;
		
		// 使用権を持っているのが自分ではない場合は再生をストップする
		if( SoundHandle != Sound->Stream.BufferUseSoundHandle )
			NS_StopStreamSoundMem( Sound->Stream.BufferUseSoundHandle ) ;

		// 使用権を得る
		Sound->Stream.BufferUseSoundHandle = SoundHandle ;
		return 0 ;
	}
	else
	{
		// 借り先が無効だったら使用権を得られない
		if( SOUNDHCHK( Sound->Stream.BufferBorrowSoundHandle, UniSound ) == true ) return -1 ;
		if( BufferP ) *BufferP = &UniSound->Buffer[0] ;
		
		// 誰も使用していない場合は使用権を得ることが出来る
		if( UniSound->Stream.BufferUseSoundHandle == -1 )
		{
			UniSound->Stream.BufferUseSoundHandle = SoundHandle ;
			return 0 ;
		}
		
		// 使用されている場合で使用しているのが自分ではなく、
		// 使用権を得る指定も無い場合は使用権を得られない
		if( SoundHandle != UniSound->Stream.BufferUseSoundHandle && BufferGet == false ) return -1 ;

		// 使用権を持っているのが自分ではない場合は再生をストップする
		if( SoundHandle != UniSound->Stream.BufferUseSoundHandle )
			NS_StopStreamSoundMem( UniSound->Stream.BufferUseSoundHandle ) ;

		// 使用権を得る
		UniSound->Stream.BufferUseSoundHandle = SoundHandle ;
		return 0 ;
	}
}

// 新しいサウンドバッファの作成
static int CreateSoundBuffer( WAVEFORMATEX *WaveFormat , DWORD BufferSize, int SoundType , int BufferNum , int SoundHandle, int SrcSoundHandle, int ASyncThread )
{
	int i ;
	SOUND *Sound ;
	SOUND *SrcSound = NULL ;
	int hr ;
	
	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( ASyncThread )
	{
		if( SOUNDHCHK_ASYNC( SoundHandle, Sound ) )
			return -1 ;
	}
	else
	{
		if( SOUNDHCHK( SoundHandle, Sound ) )
			return -1 ;
	}

	// 元ハンドルがある場合の処理
	if( SrcSoundHandle != -1 )
	{
		if( SOUNDHCHK( SrcSoundHandle, SrcSound ) )
			return -1 ;

		// タイプがストリームだったらエラー
		if( SoundType != DX_SOUNDTYPE_NORMAL ) return -1 ;

		// パラメータは元ハンドルからもってくる
		WaveFormat = &SrcSound->BufferFormat ;
		BufferSize = ( DWORD )SrcSound->Normal.WaveSize ;
	}

	// タイプによって処理を分岐
	switch( SoundType )
	{
	case DX_SOUNDTYPE_NORMAL :	// ノーマルサウンド
		{
			// セカンダリサウンドバッファを作成

			// 数分作成
			for( i = 0 ; i < BufferNum ; i ++ )
			{
				// ソースハンドルが無い場合は１番目以降は同じバッファを複製する
				if( i == 0 && SrcSound == NULL )
				{
NORMAL_CREATEBUFFER :
					hr = SoundBuffer_Initialize( &Sound->Buffer[ i ], BufferSize, WaveFormat, NULL, TRUE, FALSE, Sound->Is3DSound ) ;
					if( hr != 0 )
					{
						i -- ;
						while( i != -1 )
						{
							if( Sound->Buffer[ i ].Valid ) SoundBuffer_Terminate( &Sound->Buffer[ i ] ) ;

							i -- ;
						}
						DXST_LOGFILE_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x53\x00\x6f\x00\x75\x00\x6e\x00\x64\x00\x42\x00\x75\x00\x66\x00\x66\x00\x65\x00\x72\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"サウンドバッファの作成に失敗しました\n" @*/ ) ;
						return -1 ;
					}
					
					// 複製かどうかフラグを倒す
					Sound->Normal.BufferDuplicateFlag[ i ] = FALSE ;
				}
				else
				{
					if( SrcSound )
					{
						hr = SoundBuffer_Duplicate( &Sound->Buffer[ i ], &SrcSound->Buffer[ 0 ], Sound->Is3DSound ) ;
					}
					else
					{
						hr = SoundBuffer_Duplicate( &Sound->Buffer[ i ], &Sound->Buffer[ 0 ], Sound->Is3DSound ) ;
					}

					// 複製かどうかフラグをとりあえず倒しておく
					Sound->Normal.BufferDuplicateFlag[ i ] = FALSE ;

					// 複製に失敗した場合は複数のサウンドバッファを使用しない
					if( hr != 0 )
					{
						// ソースサウンドがあって、且つ０で失敗したら実ハンドルを一つ作る
						if( SrcSound != NULL && i == 0 )
						{
							goto NORMAL_CREATEBUFFER ;
						}
						else
						{
							BufferNum = i ;
							break ;
						}
					}
				}
			}

			// サウンドバッファ数を保存
			Sound->ValidBufferNum = BufferNum ;
		}
		break ;

	case DX_SOUNDTYPE_STREAMSTYLE :		// ストリーム風サウンド
		if( SoundBuffer_Initialize(
				&Sound->Buffer[ 0 ],
				BufferSize,
				WaveFormat,
				NULL,
				TRUE,
				TRUE,
				Sound->Is3DSound ) != 0 )
		{
			DXST_LOGFILE_ADDUTF16LE( "\xb9\x30\xc8\x30\xea\x30\xfc\x30\xe0\x30\xa8\x98\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ストリーム風サウンドバッファの作成に失敗しました\n" @*/ ) ;
			return -1 ;
		}

		// サウンドバッファ数を保存
		Sound->ValidBufferNum = 1 ;
		break ;
	}

	// データを保存
	Sound->Type = SoundType ;

	// ストリーム形式の場合はストリームサウンドハンドルリストに追加
	if( SoundType == DX_SOUNDTYPE_STREAMSTYLE )
	{
		// クリティカルセクションの取得
		CRITICALSECTION_LOCK( &SoundSysData.StreamSoundListCriticalSection ) ;

		AddHandleList( &SoundSysData.StreamSoundListFirst, &Sound->Stream.StreamSoundList, SoundHandle, Sound ) ;

		// クリティカルセクションの解放
		CriticalSection_Unlock( &SoundSysData.StreamSoundListCriticalSection ) ;
	}

	// 終了
	return 0 ;
}


// AddStreamSoundMem のグローバル変数にアクセスしないバージョン
extern int AddStreamSoundMem_UseGParam(
	LOADSOUND_GPARAM *GParam,
	STREAMDATA *Stream,
	int LoopNum,
	int SoundHandle,
	int StreamDataType,
	int *CanStreamCloseFlag,
	int UnionHandle,
	int ASyncThread
)
{
	SOUND * Sound ;
	STREAMFILEDATA *PlayData ;
	WAVEFORMATEX Format ;
	SOUND * UniSound;
	SOUNDBUFFER *SBuffer ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( ASyncThread )
	{
		if( SOUNDHCHK_ASYNC( SoundHandle, Sound ) )
			goto ERR ;
	}
	else
	{
		if( SOUNDHCHK( SoundHandle, Sound ) )
			goto ERR ;
	}

	// 一つでもデータが存在する場合はチェック処理を入れる
	if( Sound->Stream.FileNum > 0 )
	{
		// ストリーム風サウンドではなかった場合は終了
		if( Sound->Type != DX_SOUNDTYPE_STREAMSTYLE )
		{
			DXST_LOGFILE_ADDUTF16LE( "\xb9\x30\xc8\x30\xea\x30\xfc\x30\xe0\x30\xa8\x98\xb5\x30\xa6\x30\xf3\x30\xc9\x30\x67\x30\x6f\x30\x6a\x30\x44\x30\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xc7\x30\xfc\x30\xbf\x30\x4c\x30\x21\x6e\x55\x30\x8c\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x69\x00\x6e\x00\x20\x00\x41\x00\x64\x00\x64\x00\x53\x00\x72\x00\x65\x00\x61\x00\x6d\x00\x53\x00\x6f\x00\x75\x00\x6e\x00\x64\x00\x0a\x00\x00"/*@ L"ストリーム風サウンドではないサウンドデータが渡されました in AddSreamSound\n" @*/ ) ;
			goto ERR ;
		}

		// 既に再生中の場合も終了
		if( GetSoundBuffer( SoundHandle, Sound, &SBuffer, false ) == 0 )
		{
			if( SoundBuffer_CheckPlay( SBuffer ) )
			{
				DXST_LOGFILE_ADDUTF16LE( "\xe2\x65\x6b\x30\x8d\x51\x1f\x75\x2d\x4e\x6e\x30\xb9\x30\xc8\x30\xea\x30\xfc\x30\xe0\x30\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xc7\x30\xfc\x30\xbf\x30\x6b\x30\xc7\x30\xfc\x30\xbf\x30\x92\x30\xfd\x8f\xa0\x52\x57\x30\x88\x30\x46\x30\x68\x30\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"既に再生中のストリームサウンドデータにデータを追加しようとしました\n" @*/ ) ;
				goto ERR ;
			}
		}

		// もうストリームサウンド制御データの領域に余裕がない場合も終了
		if( Sound->Stream.FileNum == STS_SETSOUNDNUM - 1 )
		{
			DXST_LOGFILE_ADDUTF16LE( "\xb9\x30\xc8\x30\xea\x30\xfc\x30\xe0\x30\xa8\x98\xb5\x30\xa6\x30\xf3\x30\xc9\x30\x6e\x30\x36\x52\xa1\x5f\xc7\x30\xfc\x30\xbf\x30\x18\x98\xdf\x57\x4c\x30\x44\x30\x63\x30\x71\x30\x44\x30\x67\x30\x59\x30\x0a\x00\x00"/*@ L"ストリーム風サウンドの制御データ領域がいっぱいです\n" @*/ ) ;
			goto ERR ;
		}
	}

	PlayData = &Sound->Stream.File[ Sound->Stream.FileNum ] ;
	_MEMSET( PlayData, 0, sizeof( STREAMFILEDATA ) ) ;

	// データタイプを保存
	PlayData->DataType = StreamDataType ;

	// ループ開始位置は無効
	PlayData->LoopStartSamplePosition = -1 ;

	// ストリームポインタを先頭位置へ
	Stream->ReadShred.Seek( Stream->DataPoint, 0, SEEK_SET ) ;

	// 新しいＷＡＶＥファイルのロード、データのタイプによって処理を分岐
	switch( StreamDataType )
	{
	case DX_SOUNDDATATYPE_MEMNOPRESS :
		// メモリ上に直接再生可能なデータを保持する場合
		{
			STREAMDATA MemStream ;
			void *WaveImage ;
			int WaveSize ;

			// ＰＣＭ形式に変換
			{
				SOUNDCONV ConvData ;
				int res ;
				void *SrcBuffer ;
				int SrcSize ;
				
				// 丸々メモリに読み込む
				if( StreamFullRead( Stream, &SrcBuffer, &SrcSize ) < 0 )
				{
					DXST_LOGFILEFMT_ADDUTF16LE(( "\xf3\x97\xf0\x58\xc7\x30\xfc\x30\xbf\x30\x6e\x30\xad\x8a\x7f\x30\xbc\x8f\x7f\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"音声データの読み込みに失敗しました" @*/ )) ;
					goto ERR ;
				}
				
				MemStream.DataPoint = MemStreamOpen( SrcBuffer, ( size_t )SrcSize ) ;
				MemStream.ReadShred = *GetMemStreamDataShredStruct() ;
			
				SetupSoundConvert( &ConvData, &MemStream, GParam->DisableReadSoundFunctionMask
#ifndef DX_NON_OGGVORBIS
									,GParam->OggVorbisBitDepth, GParam->OggVorbisFromTheoraFile
#endif
				) ;
				res = SoundConvertFast( &ConvData, &Format, &PlayData->FileImage, &PlayData->FileImageSize ) ;

				TerminateSoundConvert( &ConvData ) ;
				MemStreamClose( MemStream.DataPoint ) ;
				DXFREE( SrcBuffer ) ;

				if( res < 0 )
				{
					DXST_LOGFILEFMT_ADDUTF16LE(( "\xf3\x97\xf0\x58\xc7\x30\xfc\x30\xbf\x30\x6e\x30\x09\x59\xdb\x63\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"音声データの変換に失敗しました" @*/ )) ;
					goto ERR ;
				}
			}

			// ＷＡＶＥファイルをでっち上げる
			if( CreateWaveFileImage( &WaveImage, &WaveSize,
								 &Format, sizeof( WAVEFORMATEX ),
								 PlayData->FileImage, PlayData->FileImageSize ) < 0 )
				goto ERR ;

			// 展開されたデータをストリームとして再度開く				
			PlayData->MemStream.DataPoint = MemStreamOpen( WaveImage, ( size_t )WaveSize ) ;
			PlayData->MemStream.ReadShred = *GetMemStreamDataShredStruct() ;
			SetupSoundConvert( &PlayData->ConvData, &PlayData->MemStream, GParam->DisableReadSoundFunctionMask
#ifndef DX_NON_OGGVORBIS
				,GParam->OggVorbisBitDepth, GParam->OggVorbisFromTheoraFile
#endif
			) ;

			// メモリアドレスの入れ替え
			DXFREE( PlayData->FileImage ) ;
			PlayData->FileImage = WaveImage ;
			PlayData->FileImageSize = WaveSize ;

			// ストリーム解放フラグを立てる
			*CanStreamCloseFlag = TRUE ;
		}
		break ;

	case DX_SOUNDDATATYPE_MEMPRESS :
		// データはメモリ上に存在して、圧縮されている場合
		{
			// 丸々メモリに読み込む
			if( StreamFullRead( Stream, &PlayData->FileImage, &PlayData->FileImageSize ) < 0 )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\xf3\x97\xf0\x58\xc7\x30\xfc\x30\xbf\x30\x6e\x30\xad\x8a\x7f\x30\xbc\x8f\x7f\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"音声データの読み込みに失敗しました" @*/ )) ;
				goto ERR ;
			}

			// 展開されたデータをストリームとして再度開く				
			PlayData->MemStream.DataPoint = MemStreamOpen( PlayData->FileImage, ( size_t )PlayData->FileImageSize ) ;
			PlayData->MemStream.ReadShred = *GetMemStreamDataShredStruct() ;
			if( SetupSoundConvert( &PlayData->ConvData, &PlayData->MemStream, GParam->DisableReadSoundFunctionMask
#ifndef DX_NON_OGGVORBIS
									,GParam->OggVorbisBitDepth, GParam->OggVorbisFromTheoraFile
#endif
									) < 0 )
			{
				MemStreamClose( PlayData->MemStream.DataPoint ) ;
				DXFREE( PlayData->FileImage ) ;
				PlayData->FileImage = NULL ;
				
				DXST_LOGFILEFMT_ADDUTF16LE(( "\xf3\x97\xf0\x58\xc7\x30\xfc\x30\xbf\x30\x6e\x30\xaa\x30\xfc\x30\xd7\x30\xf3\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"音声データのオープンに失敗しました" @*/ )) ;
				goto ERR ;
			}

			// ループ情報がある場合はセット
			if( ( GParam->CreateSoundLoopStartTimePosition   == GParam->CreateSoundLoopEndTimePosition   ) &&
				( GParam->CreateSoundLoopStartSamplePosition == GParam->CreateSoundLoopEndSamplePosition ) &&
				GParam->CreateSoundIgnoreLoopAreaInfo == FALSE )
			{
				int LoopStartPos, LoopEndPos ;
				if( GetSoundConvertLoopAreaInfo( &PlayData->ConvData, &LoopStartPos, &LoopEndPos ) >= 0 )
				{
					GParam->CreateSoundLoopStartSamplePosition = LoopStartPos ;
					GParam->CreateSoundLoopEndSamplePosition   = LoopEndPos ;
				}
			}

			// ストリーム解放フラグを立てる
			*CanStreamCloseFlag = TRUE ;
		}
		break ;

	case DX_SOUNDDATATYPE_FILE :
		// データをファイルから逐次読み込んでくる場合
		{
			// データタイプが DX_SOUNDDATATYPE_FILE の場合は、ファイルから直接データを読み込んでいるフラグを立てる
			Sound->Stream.FileUseFile = TRUE ;

			// 使わないデータに NULL をセットしておく
			PlayData->FileImage = NULL ;
			PlayData->FileImageSize = 0 ;
			PlayData->MemStream.DataPoint = NULL ;

			// 展開されたデータをストリームとして再度開く				
			if( SetupSoundConvert( &PlayData->ConvData, Stream, GParam->DisableReadSoundFunctionMask
#ifndef DX_NON_OGGVORBIS
									,GParam->OggVorbisBitDepth, GParam->OggVorbisFromTheoraFile
#endif
								) < 0 )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\xf3\x97\xf0\x58\xc7\x30\xfc\x30\xbf\x30\x6e\x30\xaa\x30\xfc\x30\xd7\x30\xf3\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"音声データのオープンに失敗しました" @*/ )) ;
				goto ERR ;
			}

			// ループ情報がある場合はセット
			if( ( GParam->CreateSoundLoopStartTimePosition   == GParam->CreateSoundLoopEndTimePosition   ) &&
				( GParam->CreateSoundLoopStartSamplePosition == GParam->CreateSoundLoopEndSamplePosition ) &&
				GParam->CreateSoundIgnoreLoopAreaInfo == FALSE )
			{
				int LoopStartPos, LoopEndPos ;
				if( GetSoundConvertLoopAreaInfo( &PlayData->ConvData, &LoopStartPos, &LoopEndPos ) >= 0 )
				{
					GParam->CreateSoundLoopStartSamplePosition = LoopStartPos ;
					GParam->CreateSoundLoopEndSamplePosition   = LoopEndPos ;
				}
			}

			// ストリーム解放フラグを倒す
			*CanStreamCloseFlag = FALSE ;
		}
		break ;
	}
	
	// 一番最初のデータの場合は処理を分岐
	if( Sound->Stream.FileNum == 0 )
	{
		// フォーマットを取得
		GetOutSoundFormatInfo( &PlayData->ConvData, &Sound->BufferFormat ) ;

		// サウンドバッファを共有するかどうかを調べる
		UniSound = NULL;
		while( UnionHandle != -1 )
		{
			// 無効なハンドルの場合は無視
			if( SOUNDHCHK( UnionHandle, UniSound ) )
			{
				UniSound = NULL;
				UnionHandle = -1;
				break;
			}

			// フォーマットが違うか、オリジナルのバッファがないか、ストリーム形式でない場合もアウト
			if( UniSound->Type != DX_SOUNDTYPE_STREAMSTYLE ||
				UniSound->Stream.BufferBorrowSoundHandle != -1 ||
				_MEMCMP( &Sound->BufferFormat, &UniSound->BufferFormat, sizeof( WAVEFORMATEX ) ) != 0 )
			{
				UniSound = NULL;
				UnionHandle = -1;
				break;
			}
			break;
		}

		// サウンドバッファを共有するかどうかで処理を分岐
		if( UniSound == NULL )
		{
			DWORD BufferSec ;

			// サウンドバッファを作成
			BufferSec = StreamDataType == DX_SOUNDDATATYPE_FILE ? STS_BUFSEC_FILE : STS_BUFSEC ;
			CreateSoundBuffer(
				&Sound->BufferFormat,
				SOUNDSIZE( BufferSec * Sound->BufferFormat.nAvgBytesPerSec / STS_DIVNUM, Sound->BufferFormat.nBlockAlign ),
				DX_SOUNDTYPE_STREAMSTYLE, 1, SoundHandle, -1, ASyncThread
			) ;
			Sound->Stream.SoundBufferSize = SOUNDSIZE( BufferSec * Sound->BufferFormat.nAvgBytesPerSec / STS_DIVNUM, Sound->BufferFormat.nBlockAlign ) ;
			Sound->BaseFrequency          = ( int )Sound->BufferFormat.nSamplesPerSec ;
//			Sound->PitchRateEnable        = GParam->CreateSoundPitchRateEnable ;
//			Sound->PitchRate              = GParam->CreateSoundPitchRate ;

			// 共有情報をセット
			Sound->Stream.BufferBorrowSoundHandle = -1 ;
		}
		else
		{
			// クリティカルセクションの取得
			CRITICALSECTION_LOCK( &SoundSysData.StreamSoundListCriticalSection ) ;

			// ストリームサウンドハンドルリストに追加
			Sound->Type = DX_SOUNDTYPE_STREAMSTYLE ;
			Sound->ValidBufferNum = 1 ;
			AddHandleList( &SoundSysData.StreamSoundListFirst, &Sound->Stream.StreamSoundList, SoundHandle, Sound ) ;

			// クリティカルセクションの解放
			CriticalSection_Unlock( &SoundSysData.StreamSoundListCriticalSection ) ;

			// 共有情報をセット
			Sound->Stream.BufferBorrowSoundHandle = UnionHandle ;
			UniSound->Stream.BufferBorrowSoundHandleCount ++ ;
		}
		Sound->Stream.BufferBorrowSoundHandleCount = 0 ;
		Sound->Stream.BufferUseSoundHandle = -1 ;
		Sound->Stream.DeleteWaitFlag = FALSE ;
	}
	else
	{
		// 二つ目以降の場合はバッファのフォーマットと一致しているか調べる
		GetOutSoundFormatInfo( &PlayData->ConvData, &Format ) ;
		if( _MEMCMP( &Format, &Sound->BufferFormat, sizeof( WAVEFORMATEX ) ) != 0 )
		{
			TerminateSoundConvert( &PlayData->ConvData ) ;
			if( PlayData->MemStream.DataPoint != NULL )
			{
				MemStreamClose( PlayData->MemStream.DataPoint ) ;
				PlayData->MemStream.DataPoint = NULL ;
			}
			if( PlayData->FileImage != NULL )
			{
				DXFREE( PlayData->FileImage ) ;
				PlayData->FileImage = NULL ;
			}

			DXST_LOGFILEFMT_ADDUTF16LE(( "\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x6e\x30\x55\x90\x46\x30\xf3\x97\xf0\x58\xc7\x30\xfc\x30\xbf\x30\x6f\x30\x0c\x54\x42\x66\x6b\x30\x7f\x4f\x46\x30\x53\x30\x68\x30\x6f\x30\xfa\x51\x65\x67\x7e\x30\x5b\x30\x93\x30\x00"/*@ L"フォーマットの違う音声データは同時に使うことは出来ません" @*/ )) ;
			goto ERR ;
		}
	}

	// ループの数を保存
	PlayData->LoopNum = ( char )LoopNum ;

	// プレイデータの数を増やす
	Sound->Stream.FileNum ++ ;
	
	// ストリームデータの総サンプル数を更新
	{
		int i ;
		Sound->Stream.TotalSample = 0 ;
		for( i = 0 ; i < Sound->Stream.FileNum ; i ++ )
			Sound->Stream.TotalSample += GetSoundConvertDestSize_Fast( &Sound->Stream.File[i].ConvData ) / Sound->BufferFormat.nBlockAlign ;
	}

	// 終了
	return 0 ;

ERR :

	return -1 ;
}

// ストリーム風サウンドデータにサウンドデータを追加する
extern int NS_AddStreamSoundMem( STREAMDATA *Stream, int LoopNum, int SoundHandle, int StreamDataType, int *CanStreamCloseFlag, int UnionHandle )
{
	LOADSOUND_GPARAM GParam ;

	InitLoadSoundGParam( &GParam ) ;

	return AddStreamSoundMem_UseGParam( &GParam, Stream, LoopNum, SoundHandle, StreamDataType, CanStreamCloseFlag, UnionHandle, FALSE ) ;
}

// AddStreamSoundMemToFile のグローバル変数にアクセスしないバージョン
extern int AddStreamSoundMemToFile_UseGParam(
	LOADSOUND_GPARAM *GParam,
	const wchar_t *WaveFile,
	int LoopNum,
	int SoundHandle,
	int StreamDataType,
	int UnionHandle,
	int ASyncThread
)
{
	DWORD_PTR fp ;
	SOUND * Sound ;
	int CanStreamCloseFlag ;
	STREAMDATA Stream ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( ASyncThread )
	{
		if( SOUNDHCHK_ASYNC( SoundHandle, Sound ) )
			goto ERR ;
	}
	else
	{
		if( SOUNDHCHK( SoundHandle, Sound ) )
			goto ERR ;
	}

	// ファイルを開く
	fp = DX_FOPEN( WaveFile ) ;
	if( fp == 0 )
	{
		DXST_LOGFILEFMT_ADDW(( L"Sound File Open Error : %s \n" , WaveFile )) ;
		goto ERR ;
	}

	// ストリームデータ情報をセットする
	Stream.DataPoint = fp ;
	Stream.ReadShred = *GetFileStreamDataShredStruct() ;

	// ストリームハンドルを追加
	if( AddStreamSoundMem_UseGParam( GParam, &Stream, LoopNum, SoundHandle, StreamDataType, &CanStreamCloseFlag, UnionHandle, ASyncThread ) == -1 )
	{
		DX_FCLOSE( fp ) ;
		DXST_LOGFILEFMT_ADDW(( L"Sound File Setup Error : %s \n" , WaveFile )) ;
		goto ERR ;
	}

	// ファイルを閉じても良いとのお達しが来たらファイルを閉じる
	if( CanStreamCloseFlag == TRUE )
	{
		DX_FCLOSE( fp ) ;
	}

	// ライブラリでロードした、フラグを立てる
	Sound->Stream.FileLibraryLoadFlag[ Sound->Stream.FileNum - 1 ] = TRUE ;

	// 終了
	return 0 ;

ERR :

	// エラー終了
	return -1 ;
}

// AddStreamSoundMemToMem のグローバル変数にアクセスしないバージョン
extern int AddStreamSoundMemToMem_UseGParam(
	LOADSOUND_GPARAM *GParam,
	const void *FileImageBuffer,
	int ImageSize,
	int LoopNum,
	int SoundHandle,
	int StreamDataType,
	int UnionHandle,
	int ASyncThread
)
{
	DWORD_PTR StreamHandle ;
	SOUND * Sound ;
	int CanStreamCloseFlag ;
	STREAMDATA Stream ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( ASyncThread )
	{
		if( SOUNDHCHK_ASYNC( SoundHandle, Sound ) )
			goto ERR ;
	}
	else
	{
		if( SOUNDHCHK( SoundHandle, Sound ) )
			goto ERR ;
	}

	// データタイプがファイルからの逐次読み込みだった場合はメモリにイメージを置く、に変更
	if( StreamDataType == DX_SOUNDDATATYPE_FILE ) StreamDataType = DX_SOUNDDATATYPE_MEMPRESS ;

	// メモリイメージストリームを開く
	StreamHandle = MemStreamOpen( FileImageBuffer, ( size_t )ImageSize ) ;
	if( StreamHandle == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x37\xff\x21\xff\x36\xff\xd5\x30\xa1\x30\xa4\x30\xeb\x30\x6e\x30\xb9\x30\xc8\x30\xea\x30\xfc\x30\xe0\x30\xa4\x30\xe1\x30\xfc\x30\xb8\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x0a\x00\x00"/*@ L"ＷＡＶファイルのストリームイメージハンドルの作成に失敗しました \n" @*/ )) ;
		goto ERR ;
	}

	// ストリームデータ情報をセット
	Stream.DataPoint = StreamHandle ;
	Stream.ReadShred = *GetMemStreamDataShredStruct() ;

	// ストリームハンドルを追加
	if( AddStreamSoundMem_UseGParam( GParam, &Stream, LoopNum, SoundHandle, StreamDataType, &CanStreamCloseFlag, UnionHandle, ASyncThread ) == -1 )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x37\xff\x21\xff\x36\xff\xd5\x30\xa1\x30\xa4\x30\xeb\x30\x6e\x30\xb9\x30\xc8\x30\xea\x30\xfc\x30\xe0\x30\xa4\x30\xe1\x30\xfc\x30\xb8\x30\x6e\x30\xbb\x30\xc3\x30\xc8\x30\xa2\x30\xc3\x30\xd7\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x0a\x00\x00"/*@ L"ＷＡＶファイルのストリームイメージのセットアップに失敗しました \n" @*/ )) ;
		goto ERR ;
	}

	// ストリームを閉じても良いとのお達しが来たらストリームを閉じる
	if( CanStreamCloseFlag == TRUE )
	{
		MemStreamClose( StreamHandle ) ;
	}

	// ライブラリでロードした、フラグを立てる
	Sound->Stream.FileLibraryLoadFlag[ Sound->Stream.FileNum - 1 ] = TRUE ;

	// 終了
	return 0 ;

ERR:

	return -1 ;
}

// ストリーム風サウンドデータにサウンドデータを追加する
extern int NS_AddStreamSoundMemToFile( const TCHAR *WaveFile, int LoopNum,  int SoundHandle, int StreamDataType, int UnionHandle )
{
#ifdef UNICODE
	return AddStreamSoundMemToFile_WCHAR_T(
		WaveFile, LoopNum, SoundHandle, StreamDataType, UnionHandle
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( WaveFile, return -1 )

	Result = AddStreamSoundMemToFile_WCHAR_T(
		UseWaveFileBuffer, LoopNum, SoundHandle, StreamDataType, UnionHandle
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( WaveFile )

	return Result ;
#endif
}

// ストリーム再生タイプのサウンドハンドルにサウンドファイルを再生対象に追加する
extern int NS_AddStreamSoundMemToFileWithStrLen( const TCHAR *WaveFile, size_t WaveFileLength, int LoopNum,  int SoundHandle, int StreamDataType, int UnionHandle )
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( WaveFile, WaveFileLength, return -1 )
	Result = AddStreamSoundMemToFile_WCHAR_T( UseWaveFileBuffer, LoopNum, SoundHandle, StreamDataType, UnionHandle ) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( WaveFile )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( WaveFile, WaveFileLength, return -1 )
	Result = AddStreamSoundMemToFile_WCHAR_T( UseWaveFileBuffer, LoopNum, SoundHandle, StreamDataType, UnionHandle ) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( WaveFile )
#endif
	return Result ;
}

// ストリーム風サウンドデータにサウンドデータを追加する
extern int AddStreamSoundMemToFile_WCHAR_T( const wchar_t *WaveFile, int LoopNum,  int SoundHandle, int StreamDataType, int UnionHandle )
{
	LOADSOUND_GPARAM GParam ;

	InitLoadSoundGParam( &GParam ) ;
	return AddStreamSoundMemToFile_UseGParam( &GParam, WaveFile, LoopNum, SoundHandle, StreamDataType, UnionHandle, FALSE ) ;
}

// ストリーム風サウンドデータにサウンドデータを追加する
extern int NS_AddStreamSoundMemToMem( const void *FileImageBuffer, int ImageSize, int LoopNum, int SoundHandle, int StreamDataType, int UnionHandle )
{
	LOADSOUND_GPARAM GParam ;

	InitLoadSoundGParam( &GParam ) ;
	return AddStreamSoundMemToMem_UseGParam( &GParam, FileImageBuffer, ImageSize, LoopNum, SoundHandle, StreamDataType, UnionHandle, FALSE ) ;
}



// SetupStreamSoundMem のグローバル変数にアクセスしないバージョン
extern int SetupStreamSoundMem_UseGParam( int SoundHandle, int ASyncThread )
{
	SOUND * Sound ;
	SOUNDBUFFER *SBuffer ;

	if( SoundSysData.InitializeFlag == FALSE ||
		CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( ASyncThread )
	{
		if( SOUNDHCHK_ASYNC( SoundHandle, Sound ) )
			goto ERR ;
	}
	else
	{
		if( SOUNDHCHK( SoundHandle, Sound ) )
			goto ERR ;
	}

	// ストリームじゃなかったら何もせず終了
	if( Sound->Type != DX_SOUNDTYPE_STREAMSTYLE ) goto END ;

	// 再生中だったら何もせず終了
	if( ASyncThread == FALSE && NS_CheckStreamSoundMem( SoundHandle ) == 1 ) goto END ;

	// 使用権が無い場合は何もせず終了
	if( GetSoundBuffer( SoundHandle, Sound, &SBuffer, false ) == -1 ) goto END ;

	// 準備が完了していたら何もせず終了
	if( Sound->Stream.StartSetupCompFlag == TRUE ) goto END ;

	// 再生位置を先頭にセット
//	SBuffer->SetCurrentPosition( 0 ) ;
	SoundBuffer_SetCurrentPosition( SBuffer, 0 ) ;

	// パラメータを初期化
	Sound->Stream.SoundBufferCompCopyOffset	 = -800 ;
	Sound->Stream.SoundBufferCopyStartOffset = -1 ;
	Sound->Stream.PrevCheckPlayPosition      = 0 ;
	Sound->Stream.EndWaitFlag                = FALSE ;

	// 圧縮データを展開する準備を行う
	{
		STREAMFILEDATA *PlayData ;

		PlayData = &Sound->Stream.File[ Sound->Stream.FileActive ] ;
//		SetTimeSoundConvert( &PlayData->ConvData, Sound->Stream.FileCompCopyLength * 1000 / Sound->BufferFormat.nAvgBytesPerSec ) ;
		SetSampleTimeSoundConvert( &PlayData->ConvData, Sound->Stream.FileCompCopyLength / Sound->BufferFormat.nBlockAlign ) ;
	}

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// 準備完了
	Sound->Stream.StartSetupCompFlag = TRUE ;

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// 最初のサウンドデータを書きこむ
	ProcessStreamSoundMem_UseGParam( SoundHandle, ASyncThread ) ;

END :

	// 終了
	return 0 ;

ERR :

	// 終了
	return 0 ;
}

// ストリーム風サウンドデータの再生準備を行う
extern int NS_SetupStreamSoundMem( int SoundHandle )
{
	return SetupStreamSoundMem_UseGParam( SoundHandle, FALSE ) ;
}


// ストリームサウンドハンドルの再生準備を行う( -1:エラー 0:正常終了 1:再生する必要なし )
static	int	_PlaySetupStreamSoundMem( int SoundHandle, SOUND * Sound, int PlayType, int TopPositionFlag, SOUNDBUFFER **DestSBuffer )
{
	SOUNDBUFFER *SBuffer ;
	int i ;

	// 使用するサウンドバッファを取得
	GetSoundBuffer( SoundHandle, Sound, &SBuffer );
	if( DestSBuffer ) *DestSBuffer = SBuffer ;
	
	// 再生中で頭だしでもない場合は此処で終了
	if( TopPositionFlag == FALSE && NS_CheckStreamSoundMem( SoundHandle ) == 1 )
		return 1 ;

	// 再生中の場合は止める
	NS_StopStreamSoundMem( SoundHandle ) ;

	// 再生タイプが違ったら分岐
	if( Sound->PlayType != PlayType )
	{
		// 場合によっては頭だし再生にする
		if( ( Sound->PlayType & DX_PLAYTYPE_LOOPBIT ) != ( PlayType & DX_PLAYTYPE_LOOPBIT ) &&
			Sound->Stream.AllPlayFlag == TRUE )
		{
			Sound->Stream.AllPlayFlag = FALSE ;
			Sound->Stream.FileCompCopyLength = 0 ;
			Sound->Stream.CompPlayWaveLength = 0 ;
			Sound->Stream.LoopPositionValidFlag = FALSE ;
			Sound->Stream.FileLoopCount = 0 ;
			Sound->Stream.FileActive = 0 ;
			Sound->Stream.StartSetupCompFlag = FALSE ;		// 準備フラグを倒す
		}
	}

	// 頭からの再生だった場合の処理
	if( TopPositionFlag == TRUE )
	{
		Sound->Stream.AllPlayFlag = FALSE ;
		if( Sound->Stream.CompPlayWaveLength != 0 )
		{
			Sound->Stream.FileCompCopyLength = 0 ;
			Sound->Stream.CompPlayWaveLength = 0 ;
			Sound->Stream.LoopPositionValidFlag = FALSE ;
			Sound->Stream.FileLoopCount = 0 ;
			Sound->Stream.FileActive = 0 ;
			Sound->Stream.StartSetupCompFlag = FALSE ;		// 準備フラグを倒す
		}
	}

	// 再生タイプを保存
	Sound->PlayType = PlayType ;

	// 再生準備
	NS_SetupStreamSoundMem( SoundHandle ) ;

	// ボリュームをセットする
	for( i = 0 ; i < SOUNDBUFFER_MAX_CHANNEL_NUM ; i ++ )
	{
		if( Sound->ValidNextPlayVolume[ i ] )
		{
			SoundBuffer_SetVolumeAll( &Sound->Buffer[ 0 ], ( LONG )Sound->NextPlayVolume[ i ] ) ;
			Sound->ValidNextPlayVolume[ i ] = 0 ;
		}
		else
		{
			SoundBuffer_SetVolumeAll( &Sound->Buffer[ 0 ], ( LONG )Sound->BaseVolume[ i ] ) ;
		}
	}

	// パンをセットする
	if( Sound->ValidNextPlayPan )
	{
		SoundBuffer_SetPan( &Sound->Buffer[ 0 ], ( LONG )Sound->NextPlayPan ) ;
		Sound->ValidNextPlayPan = 0 ;
	}
	else
	{
		SoundBuffer_SetPan( &Sound->Buffer[ 0 ], ( LONG )Sound->BasePan ) ;
	}

	// 周波数をセットする
	{
		int Frequency ;

		if( Sound->ValidNextPlayFrequency )
		{
			Frequency = Sound->NextPlayFrequency ;
			Sound->ValidNextPlayFrequency = 0 ;
		}
		else
		{
			Frequency = Sound->BaseFrequency ;
		}

		if( Sound->PitchRateEnable )
		{
			Frequency = _FTOL( Frequency * Sound->PitchRate ) ;
		}
		SoundBuffer_SetFrequency( &Sound->Buffer[ 0 ], ( DWORD )Frequency ) ;
	}

	// ３Ｄサウンドの場合は３Ｄサウンドパラメータの更新を行う
	if( Sound->Is3DSound != FALSE )
	{
		VECTOR *Position ;
		float Radius ;
		VECTOR *Velocity ;

		if( Sound->ValidNextPlay3DPosition )
		{
			Position = &Sound->NextPlay3DPosition ;
			Sound->ValidNextPlay3DPosition = 0 ;
		}
		else
		{
			Position = &Sound->Base3DPosition ;
		}

		if( Sound->ValidNextPlay3DRadius )
		{
			Radius = Sound->NextPlay3DRadius ;
			Sound->ValidNextPlay3DRadius = 0 ;
		}
		else
		{
			Radius = Sound->Base3DRadius ;
		}

		if( Sound->ValidNextPlay3DVelocity )
		{
			Velocity = &Sound->NextPlay3DVelocity ;
			Sound->ValidNextPlay3DVelocity = 0 ;
		}
		else
		{
			Velocity = &Sound->Base3DVelocity ;
		}
		SoundBuffer_Set3DPosition( &Sound->Buffer[ 0 ], Position ) ;
		SoundBuffer_Set3DRadius(   &Sound->Buffer[ 0 ], Radius ) ;
		SoundBuffer_Set3DVelocity( &Sound->Buffer[ 0 ], Velocity ) ;

		SoundBuffer_Refresh3DSoundParam( &Sound->Buffer[ 0 ] ) ;
	}

	// 正常終了
	return 0 ;
}

// サウンドハンドルを作成する
static int _CreateSoundHandle( int Is3DSound, int NotInitSoundMemDelete, int ASyncThread )
{
	int SoundHandle ;
	SOUND *Sound ;

	SoundHandle = AddHandle( DX_HANDLETYPE_SOUND, ASyncThread, -1 ) ;
	if( SoundHandle == -1 )
		return -1 ;

	if( SOUNDHCHK_ASYNC( SoundHandle, Sound ) )
		return -1 ;

	Sound->Is3DSound = Is3DSound ;
	Sound->NotInitSoundMemDelete = NotInitSoundMemDelete ;

	if( Sound->Is3DSound != FALSE )
	{
		// クリティカルセクションの取得
		CRITICALSECTION_LOCK( &SoundSysData._3DSoundListCriticalSection ) ;

		// ３Ｄサウンド形式の場合は３Ｄサウンドハンドルリストに追加
		AddHandleList( &SoundSysData._3DSoundListFirst, &Sound->_3DSoundList, SoundHandle, Sound ) ;

		// クリティカルセクションの解放
		CriticalSection_Unlock( &SoundSysData._3DSoundListCriticalSection ) ;
	}

	return SoundHandle ;
}

// ストリーム風サウンドデータの再生開始
extern int NS_PlayStreamSoundMem( int SoundHandle , int PlayType, int TopPositionFlag )
{
	SOUND * Sound ;
	SOUNDBUFFER *SBuffer ;

	if( SoundSysData.InitializeFlag == FALSE )
	{
		return -1 ;
	}
	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// エラー判定
	if( SOUNDHCHK( SoundHandle, Sound ) )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

		// エラー終了
		return -1 ;
	}

	// 再生の準備
	if( _PlaySetupStreamSoundMem( SoundHandle, Sound, PlayType, TopPositionFlag, &SBuffer ) != 0 )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

		// エラー終了
		return -1 ;
	}

	// 再生開始
//	SBuffer->Play( 0 , 0 , D_DSBPLAY_LOOPING ) ; 
	if( SoundBuffer_Play( SBuffer, TRUE ) < 0 )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

		// エラー終了
		return -1 ;
	}

	// ３Ｄサウンドの場合は再生中の３Ｄサウンドリストに追加する
	if( Sound->Is3DSound != FALSE )
	{
		if( Sound->AddPlay3DSoundList == FALSE )
		{
			Sound->AddPlay3DSoundList = TRUE ;

			// クリティカルセクションの取得
			CRITICALSECTION_LOCK( &SoundSysData.Play3DSoundListCriticalSection ) ;

			AddHandleList( &SoundSysData.Play3DSoundListFirst, &Sound->Play3DSoundList, SoundHandle, Sound ) ;

			// クリティカルセクションの解放
			CriticalSection_Unlock( &SoundSysData.Play3DSoundListCriticalSection ) ;
		}
	}

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// 再生ステータスによっては再生終了を待つ
	if( PlayType == DX_PLAYTYPE_NORMAL )
	{
		while( NS_ProcessMessage() == 0 && NS_CheckSoundMem( SoundHandle ) == 1 ){ Thread_Sleep( 1 ) ; }
	}

	// 終了
	return 0 ;
}

// ストリーム風サウンドデータの再生状態を得る
extern int NS_CheckStreamSoundMem( int SoundHandle )
{
	SOUND * Sound ;
	SOUNDBUFFER *SBuffer ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	// サウンドバッファの使用権が無い場合は少なくとも再生は行っていない
	if( GetSoundBuffer( SoundHandle, Sound, &SBuffer, false ) == -1 ) return 0 ;

	// システム側で止めている場合は止める前の状態を返す
	if( Sound->BufferPlayStateBackupFlagValid[ 0 ] )
	{
		return Sound->BufferPlayStateBackupFlag[ 0 ] ? 1 : 0 ;
	}
	else
	{
		if( SoundBuffer_CheckPlay( SBuffer ) )
		{
			return 1 ;
		}
	}

	// ここまできていれば再生はされていない
	return 0 ;
}



// ストリーム風サウンドデータの再生終了
extern int NS_StopStreamSoundMem( int SoundHandle )
{
	SOUND *Sound, *UniSound = NULL ;
	int IsPlay ;
	SOUNDBUFFER *SBuffer ;

	if( SoundSysData.InitializeFlag == FALSE )
	{
		return -1 ;
	}
	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// エラー判定
	if( SOUNDHCHK( SoundHandle, Sound ) )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

		// エラー終了
		return -1 ;
	}

	// 再生時間を更新する
	RefreshStreamSoundPlayCompLength( SoundHandle ) ;

	// ハンドルの使用権が無い場合は少なくとも再生中ではない
	if( GetSoundBuffer( SoundHandle, Sound, &SBuffer, false ) == -1 )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

		return 0 ;
	}
	
	// 再生中の場合は止める
	if( Sound->Stream.BufferBorrowSoundHandle != -1 )
	{
		if( SOUNDHCHK( Sound->Stream.BufferBorrowSoundHandle, UniSound ) == false )
		{
			UniSound->Stream.BufferUseSoundHandle = -1;
		}
	}

	IsPlay = SoundBuffer_CheckPlay( SBuffer ) ;
	if( IsPlay == -1 )
	{
		return -1 ;
	}
	if( IsPlay )
	{
		SoundBuffer_Stop( SBuffer, TRUE ) ;

		Sound->BufferPlayStateBackupFlagValid[ 0 ] = FALSE ;
		Sound->BufferPlayStateBackupFlag[ 0 ] = FALSE ;
	}

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// 終了
	return 0 ;
}

// SetStreamSoundCurrentPosition のグローバル変数にアクセスしないバージョン
extern int SetStreamSoundCurrentPosition_UseGParam( int Byte, int SoundHandle, int ASyncThread )
{
	SOUND * sd ;
//	STREAMFILEDATA * pl ;
//	int i, pos ;
//	int datasize ;
//	WAVEFORMATEX *wf ;

	if( SoundSysData.InitializeFlag == FALSE )
	{
		return -1 ;
	}
	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// エラー判定
	if( ASyncThread )
	{
		if( SOUNDHCHK_ASYNC( SoundHandle, sd ) )
			goto ERR ;
	}
	else
	{
		if( SOUNDHCHK( SoundHandle, sd ) )
			goto ERR ;
	}

	if( Byte / sd->BufferFormat.nBlockAlign > sd->Stream.TotalSample )
		goto ERR ;

	// ファイルが二つ以上の場合はまだ未対応
	if( sd->Stream.FileNum > 1 ) goto ERR ;

/*
***	圧縮データの場合は正常に計算できなかった ****

	// 再生位置の算出
	pl = sd->Stream.File ;
	pos = Byte ;
	for( i = 0 ; i < sd->Stream.FileNum ; i ++, pl ++ )
	{
		wf = &sd->BufferFormat ;
		datasize = pl->ConvData.DataSize ;
		if( pos < datasize ) break ;
		pos -= datasize ;
	}
	if( i == sd->Stream.FileNum ) return -1 ;
*/
	sd->Stream.FileLoopCount         = 0 ;
	sd->Stream.LoopPositionValidFlag = FALSE ;
//	sd->Stream.FileActive            = i ;
	sd->Stream.FileActive            = 0 ;
	sd->Stream.CompPlayWaveLength    = ( DWORD )Byte ;
	sd->Stream.FileCompCopyLength    = Byte ;

	// 準備完了フラグを倒す
	sd->Stream.StartSetupCompFlag = FALSE ;

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// 終了
	return 0 ;

ERR :
	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	return -1 ;
}

// サウンドハンドルの再生位置をバイト単位で変更する(再生が止まっている時のみ有効)
extern int NS_SetStreamSoundCurrentPosition( int Byte, int SoundHandle )
{
	return SetStreamSoundCurrentPosition_UseGParam( Byte, SoundHandle, FALSE ) ;
}

// サウンドハンドルの再生位置をバイト単位で取得する
extern int NS_GetStreamSoundCurrentPosition( int SoundHandle )
{
	SOUND * sd ;
//	STREAMFILEDATA * pl ;
	int /*i,*/ pos ;
//	WAVEFORMATEX *wf ;

	if( SoundSysData.InitializeFlag == FALSE )
	{
		return -1 ;
	}
	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// エラー判定
	if( SOUNDHCHK( SoundHandle, sd ) )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

		// エラー終了
		return -1 ;
	}

	// 再生位置の算出
/*	pl = sd->Stream.File ;
	pos = 0 ;
	for( i = 0 ; i < sd->Stream.FileNum ; i ++, pl ++ )
	{
		wf = &sd->BufferFormat ;
		pos += pl->ConvData.DataSize ;
	}
	pos += sd->Stream.FileCompCopyLength ;
*/
	// 再生時間の更新
	RefreshStreamSoundPlayCompLength( SoundHandle ) ;

	// 再生時間の取得
	pos = ( int )sd->Stream.CompPlayWaveLength ;

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// 値を返す
	return pos ;
}

// サウンドハンドルの再生位置をミリ秒単位で設定する(圧縮形式の場合は正しく設定されない場合がある)
extern int NS_SetStreamSoundCurrentTime( int Time, int SoundHandle )
{
	SOUND * sd ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// 再生位置の変更
	return NS_SetStreamSoundCurrentPosition( ( int )( MilliSecPositionToSamplePosition( ( int )sd->BufferFormat.nSamplesPerSec, Time ) * sd->BufferFormat.nBlockAlign ), SoundHandle ) ;
}

// サウンドハンドルの再生位置をミリ秒単位で取得する(圧縮形式の場合は正しい値が返ってこない場合がある)
extern int NS_GetStreamSoundCurrentTime( int SoundHandle )
{
	SOUND * sd ;
//	STREAMFILEDATA * pl ;
	int /*i,*/ time ;
//	int datasize ;
//	WAVEFORMATEX *wf ;

	if( SoundSysData.InitializeFlag == FALSE )
	{
		return -1 ;
	}
	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// エラー判定
	if( SOUNDHCHK( SoundHandle, sd ) )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

		// エラー終了
		return -1 ;
	}

	// 再生時間の更新
	RefreshStreamSoundPlayCompLength( SoundHandle ) ;

	// 再生時間をミリ秒単位に変換
//	time = _DTOL( (double)sd->Stream.CompPlayWaveLength / sd->BufferFormat.nAvgBytesPerSec * 1000 ) ;
	time = SamplePositionToMilliSecPosition( ( int )sd->BufferFormat.nSamplesPerSec, ( int )( sd->Stream.CompPlayWaveLength / sd->BufferFormat.nBlockAlign ) ) ;
	
	// 再生時間が最初に取得した音の長さよりも長いことがあるので
	// もし再生時間の方が長くなってしまった場合は補正する
	if( time > NS_GetSoundTotalTime( SoundHandle ) )
		time = NS_GetSoundTotalTime( SoundHandle ) ;

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// 値を返す
	return time ;
}

// サウンドハンドルの再生完了時間を更新する
extern int RefreshStreamSoundPlayCompLength( int SoundHandle, int CurrentPosition, int ASyncThread )
{
	SOUND * sd ;
	DWORD CurPosition ;
	int UpdateFlag ;
	SOUNDBUFFER *SBuffer ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( ASyncThread )
	{
		if( SOUNDHCHK_ASYNC( SoundHandle, sd ) )
			return -1 ;
	}
	else
	{
		if( SOUNDHCHK( SoundHandle, sd ) )
			return -1 ;
	}

	// ストリーム再生用データではない場合は何もせず終了
	if( sd->Type != DX_SOUNDTYPE_STREAMSTYLE ) return 0 ;
	
	// サウンドバッファの使用権が得られない場合は少なくとも再生中ではない
	if( GetSoundBuffer( SoundHandle, sd, &SBuffer, false ) == -1 ) return 0 ;
	
	// 再生準備が完了していない時も終了
	if( sd->Stream.StartSetupCompFlag == FALSE ) return 0 ;

	// 現在の再生位置を取得する
	if( CurrentPosition == -1 )
	{
//		SBuffer->GetCurrentPosition( &CurPosition , NULL );
		SoundBuffer_GetCurrentPosition( SBuffer, &CurPosition , NULL ) ;
	}
	else
	{
		CurPosition = ( DWORD )CurrentPosition ;
	}

	// 前回の位置との差分を加算する
	if( sd->Stream.PrevCheckPlayPosition != CurPosition )
	{
		UpdateFlag = 0 ;

		// ループ指定がある場合と無い場合で処理を分岐
		if( sd->Stream.LoopPositionValidFlag == TRUE )
		{
			if( sd->Stream.PrevCheckPlayPosition < CurPosition )
			{
				if( sd->Stream.LoopPosition > sd->Stream.PrevCheckPlayPosition &&
					sd->Stream.LoopPosition <= CurPosition )
				{
					sd->Stream.CompPlayWaveLength = CurPosition - sd->Stream.LoopPosition ;
					UpdateFlag = 1 ;
				}
			}
			else
			{
				if( sd->Stream.LoopPosition > sd->Stream.PrevCheckPlayPosition )
				{
					sd->Stream.CompPlayWaveLength = sd->Stream.SoundBufferSize - sd->Stream.LoopPosition + CurPosition ;
					UpdateFlag = 1 ;
				}
				else
				if( sd->Stream.LoopPosition <= CurPosition )
				{
					sd->Stream.CompPlayWaveLength = CurPosition - sd->Stream.LoopPosition ;
					UpdateFlag = 1 ;
				}
			}
		}
		if( UpdateFlag == 1 )
		{
			sd->Stream.CompPlayWaveLength += sd->Stream.LoopAfterCompPlayWaveLength ;
			sd->Stream.LoopPositionValidFlag = FALSE ;
		}
		else
		{
			if( sd->Stream.PrevCheckPlayPosition < CurPosition )
			{
				sd->Stream.CompPlayWaveLength += CurPosition - sd->Stream.PrevCheckPlayPosition ;
			}
			else
			{
				sd->Stream.CompPlayWaveLength += ( sd->Stream.SoundBufferSize - sd->Stream.PrevCheckPlayPosition ) + CurPosition ;
			}
		}
		
		// 今回取得した位置をセットする
		sd->Stream.PrevCheckPlayPosition = CurPosition ;
	}

	// 終了
	return 0 ;
}


// ProcessStreamSoundMem 関数の補助関数
// 無音データを書き込む
void NoneSoundDataCopy( SOUND * Sound, SOUNDBUFFERLOCKDATA *LockData, DWORD MoveLength )
{
	DWORD MLen, MOff ;
	BYTE *WOff ;

	MOff = 0 ;

	while( MoveLength != 0 )
	{
		MLen = MoveLength ;

		if( LockData->Valid != 0 )
		{
			WOff = LockData->WriteP + LockData->Offset ;

			if( MLen > LockData->Valid ) MLen = LockData->Valid ;

			if( Sound->BufferFormat.wBitsPerSample == 8 )	_MEMSET( WOff, 127, MLen ) ;
			else											_MEMSET( WOff, 0,   MLen ) ;

			LockData->Valid -= MLen ;
			LockData->Offset += MLen ;
			MoveLength -= MLen ;
			MOff += MLen ;
		}
		else
		{
			if( LockData->Valid2 == 0 ) break ;

			WOff = LockData->WriteP2 + LockData->Offset2 ;

			if( MLen > LockData->Valid2 ) MLen = LockData->Valid2 ;

			if( Sound->BufferFormat.wBitsPerSample == 8 )	_MEMSET( WOff, 127, MLen ) ;
			else											_MEMSET( WOff, 0,   MLen ) ;

			LockData->Valid2 -= MLen ;
			LockData->Offset2 += MLen ;
			MoveLength -= MLen ;
			MOff += MLen ;
		}
	}
}


// ProcessStreamSoundMem 関数の補助関数
// 音声データを書き込む
int SoundDataCopy( SOUNDBUFFERLOCKDATA *LockData, SOUNDCONV *ConvData, DWORD MoveLength )
{
	DWORD MLen, MAllLen ;
	BYTE *WOff ;

	MAllLen = 0 ;

	while( MoveLength != 0 )
	{
		MLen = MoveLength ;

		if( LockData->Valid != 0 )
		{
			WOff = LockData->WriteP + LockData->Offset ;

			if( MLen > LockData->Valid ) MLen = LockData->Valid ;
			MLen = ( DWORD )RunSoundConvert( ConvData, WOff, ( int )MLen ) ;
			if( MLen == 0 ) break ;

			LockData->Valid -= MLen ;
			LockData->Offset += MLen ;
			MoveLength -= MLen ;
			MAllLen += MLen ;
		}
		else
		{
			WOff = LockData->WriteP2 + LockData->Offset2 ;

			if( MLen > LockData->Valid2 ) MLen = LockData->Valid2 ;
			MLen = ( DWORD )RunSoundConvert( ConvData, WOff, ( int )MLen ) ;
			if( MLen == 0 ) break ;

			LockData->Valid2 -= MLen ;
			LockData->Offset2 += MLen ;
			MoveLength -= MLen ;
			MAllLen += MLen ;
		}
	}
	
	return ( int )MAllLen ;
}

// ProcessStreamSoundMem関数の補助関数
// 次のファイルに移るかどうかをチェックする
int StreamSoundNextData( SOUND * Sound, SOUNDBUFFERLOCKDATA *LockData, int CurrentPosition )
{
	STREAMFILEDATA *PlayData ;
		
	// ループ数を増やす
	Sound->Stream.FileLoopCount ++ ;

	// 無限ループかどうかで処理を分岐
	if( Sound->Stream.File[Sound->Stream.FileActive].LoopNum == -1 )
	{
		WAVEFORMATEX *wfmt = &Sound->BufferFormat ;

		PlayData = &Sound->Stream.File[ Sound->Stream.FileActive ] ;

		// 既にループ位置をセットしてある場合は一度ループするまで待つ
		if( Sound->Stream.LoopPositionValidFlag == TRUE )
			return -1 ;

		// 再生位置をセットする
		Sound->Stream.FileCompCopyLength = PlayData->LoopSamplePosition * wfmt->nBlockAlign ;

		// 再生アドレス更新用情報をセット
		Sound->Stream.LoopAfterCompPlayWaveLength = ( DWORD )( PlayData->LoopSamplePosition * wfmt->nBlockAlign ) ;
		Sound->Stream.LoopPositionValidFlag       = TRUE ;
		if( LockData->Valid == 0 )
		{
			Sound->Stream.LoopPosition = LockData->Length2 - LockData->Valid2 ;
		}
		else
		{
			Sound->Stream.LoopPosition = LockData->StartOffst + LockData->Length - LockData->Valid ;
		}
	}
	else
	{
		// ループ回数が規定値にきていたら次のデータへ
		if( Sound->Stream.FileLoopCount > Sound->Stream.File[Sound->Stream.FileActive].LoopNum  )
		{
			// ループ再生指定でループする条件が揃った場合、既にループ位置をセットしてある場合は一度ループするまで待つ
			if( Sound->Stream.FileNum - 1 == Sound->Stream.FileActive &&
				Sound->PlayType == DX_PLAYTYPE_LOOP &&
				Sound->Stream.LoopPositionValidFlag == TRUE )
				return -1 ;

			Sound->Stream.FileLoopCount = 0 ;
			Sound->Stream.FileActive ++ ;
			if( Sound->Stream.FileNum == Sound->Stream.FileActive )
			{
				Sound->Stream.AllPlayFlag = TRUE ;
			
				if( Sound->PlayType == DX_PLAYTYPE_LOOP )
				{
					Sound->Stream.FileActive = 0  ;
	//				Sound->Stream.CompPlayWaveLength = 0 ;

					Sound->Stream.LoopAfterCompPlayWaveLength = 0 ;
					Sound->Stream.LoopPositionValidFlag = TRUE ;
					if( LockData->Valid == 0 )
					{
						Sound->Stream.LoopPosition = LockData->Length2 - LockData->Valid2 ;
					}
					else
					{
						Sound->Stream.LoopPosition = LockData->StartOffst + LockData->Length - LockData->Valid ;
					}
				}
				else
				{
					// 終了フラグを立てる
					Sound->Stream.EndWaitFlag = TRUE ;

					// 終了オフセットをセットする
					if( LockData->Valid == 0 )
					{
						Sound->Stream.EndOffset = LockData->Length2 - LockData->Valid2 ;
					}
					else
					{
						Sound->Stream.EndOffset = LockData->StartOffst + LockData->Length - LockData->Valid ;
					}
					Sound->Stream.EndStartOffset = ( DWORD )CurrentPosition ;
					NoneSoundDataCopy( Sound, LockData, ( DWORD )AdjustSoundDataBlock( ( int )( Sound->BufferFormat.nAvgBytesPerSec / STS_ONECOPYSEC ), Sound ) ) ;

					return 0 ;
				}
			}
		}

		// 再生情報の初期化
		Sound->Stream.FileCompCopyLength = 0 ;

	}

	// 圧縮データを展開する準備を行う
	PlayData = &Sound->Stream.File[ Sound->Stream.FileActive ] ;
//	SetTimeSoundConvert( &PlayData->ConvData,  Sound->Stream.FileCompCopyLength * 1000 / Sound->BufferFormat.nAvgBytesPerSec ) ;
	SetSampleTimeSoundConvert( &PlayData->ConvData,  Sound->Stream.FileCompCopyLength / Sound->BufferFormat.nBlockAlign ) ;

	// 終了
	return 0 ;
}


// ProcessStreamSoundMem のグローバル変数にアクセスしないバージョン
extern int ProcessStreamSoundMem_UseGParam( int SoundHandle, int ASyncThread )
{
	SOUND * Sound ;
	STREAMFILEDATA *PlayData ;
	DWORD CurPosition = 0 ;
	SOUNDBUFFERLOCKDATA LockData ;
	int MoveByte, MoveByte2, MoveStartOffset ;
	int MoveB ;
	SOUNDBUFFER *SBuffer ;
	int BreakFlag ;
	DWORD Use_STS_ADVANCECOPYSEC ;
	DWORD Use_STS_MINADVANCECOPYSEC ;

	if( SoundSysData.InitializeFlag == FALSE )
	{
		return -1 ;
	}
	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラーチェック
	if( ASyncThread )
	{
		if( SOUNDHCHK_ASYNC( SoundHandle, Sound ) )
			return -1 ;
	}
	else
	{
		if( SOUNDHCHK( SoundHandle, Sound ) )
			return -1 ;
	}

	// 再生形式がストリームタイプじゃなかったらここで終了
	if( Sound->Type != DX_SOUNDTYPE_STREAMSTYLE ) return 0 ;

	// サウンドバッファの使用権がない場合もここで終了
	if( GetSoundBuffer( SoundHandle, Sound, &SBuffer, false ) == -1 ) return 0 ;
	
	// 再生準備が完了していなかったら何もせず終了
	if( Sound->Stream.StartSetupCompFlag == FALSE ) return 0 ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// 再度ハンドルチェック
	if( ASyncThread )
	{
		if( SOUNDHCHK_ASYNC( SoundHandle, Sound ) )
		{
			// クリティカルセクションの解放
			CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

			// エラー終了
			return -1 ;
		}
	}
	else
	{
		if( SOUNDHCHK( SoundHandle, Sound ) )
		{
			// クリティカルセクションの解放
			CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

			// エラー終了
			return -1 ;
		}
	}

	// 再生完了時間の更新
	RefreshStreamSoundPlayCompLength( SoundHandle, -1, ASyncThread ) ;

	// ファイルストリームかどうかで先読みする秒数を変更
	if( Sound->Stream.FileUseFile )
	{
		Use_STS_ADVANCECOPYSEC    = STS_ADVANCECOPYSEC_FILE ;
		Use_STS_MINADVANCECOPYSEC = STS_MINADVANCECOPYSEC_FILE ;
	}
	else
	{
		Use_STS_ADVANCECOPYSEC    = STS_ADVANCECOPYSEC ;
		Use_STS_MINADVANCECOPYSEC = STS_MINADVANCECOPYSEC ;
	}

	// エンドフラグが立っている場合は終了判定処理を行う
	if( Sound->Stream.EndWaitFlag )
	{
		// 現在の再生位置を得る
//		SBuffer->GetCurrentPosition( &CurPosition , NULL );
		SoundBuffer_GetCurrentPosition( SBuffer, &CurPosition, NULL ) ;
  
		// 終了判定
		if(
		    (
			  Sound->Stream.EndStartOffset   < Sound->Stream.EndOffset &&
			  (
			    Sound->Stream.EndStartOffset > CurPosition ||
				Sound->Stream.EndOffset      < CurPosition
			  )
			) ||
			(
			  Sound->Stream.EndOffset        < CurPosition &&
			  Sound->Stream.EndStartOffset   > CurPosition &&
			  Sound->Stream.EndStartOffset   > Sound->Stream.EndOffset
			)
		  )
		{
			// 再生をとめる
//			SBuffer->Stop() ;
			SoundBuffer_Stop( SBuffer ) ;

			// エンドフラグを倒す
			Sound->Stream.EndWaitFlag = FALSE ;

			// 再生時間情報を初期化する
			Sound->Stream.FileCompCopyLength = 0 ;
			Sound->Stream.CompPlayWaveLength = 0 ;
			Sound->Stream.LoopPositionValidFlag = FALSE ;
			Sound->Stream.FileLoopCount = 0 ;
			Sound->Stream.FileActive = 0 ;
			Sound->Stream.StartSetupCompFlag = FALSE ;		// 準備フラグを倒す
		}
		else
		{
			MoveByte = ( int )AdjustSoundDataBlock( ( int )( Sound->BufferFormat.nAvgBytesPerSec / STS_ONECOPYSEC ), Sound ) ;
			MoveStartOffset = Sound->Stream.SoundBufferCompCopyOffset ;

			// 転送すべきかどうかの判定
			{
				DWORD C, S ;

//				SBuffer->GetCurrentPosition( &CurPosition , NULL );
				SoundBuffer_GetCurrentPosition( SBuffer, &CurPosition, NULL ) ;

				C = ( DWORD )Sound->Stream.SoundBufferCompCopyOffset ;
				S = ( DWORD )Sound->Stream.SoundBufferCopyStartOffset ;
				if( ( C < S && ( S > CurPosition && C < CurPosition ) ) || 
					( C > S && ( S > CurPosition || C < CurPosition ) ) )
				{
					goto ERR ;
				}
			}

			MoveB = MoveByte ;

			// ロック処理
			{
				LockData.StartOffst = ( DWORD )MoveStartOffset ;

				if( Sound->Stream.SoundBufferCompCopyOffset > ( int )Sound->Stream.SoundBufferSize )
					Sound->Stream.SoundBufferCompCopyOffset = Sound->Stream.SoundBufferCompCopyOffset ;

//				SBuffer->Lock( MoveStartOffset, MoveByte,
//								( void ** )&(LockData.WriteP), &LockData.Length,
//								( void ** )&(LockData.WriteP2), &LockData.Length2, 0 ) ; 
				SoundBuffer_Lock( SBuffer, ( DWORD )MoveStartOffset, ( DWORD )MoveByte,
								( void ** )&(LockData.WriteP), &LockData.Length,
								( void ** )&(LockData.WriteP2), &LockData.Length2 ) ; 
				LockData.Offset  = 0 ;
				LockData.Offset2 = 0 ;
				LockData.Valid  = LockData.Length ;
				LockData.Valid2 = LockData.Length2 ;
			}

			// データ転送処理
			while( MoveByte != 0 )
			{
				// 使用するデータをセット
				PlayData = &Sound->Stream.File[ Sound->Stream.FileActive ] ;

				// 転送バイトのセット
				MoveByte2 = MoveByte ;

				// 転送
				NoneSoundDataCopy( Sound, &LockData, ( DWORD )MoveByte2 ) ;
				MoveByte -= MoveByte2 ;
			}

			// ロック解除
//			SBuffer->Unlock( ( void * )LockData.WriteP, LockData.Length,
//							( void * )LockData.WriteP2, LockData.Length2 ) ; 
			SoundBuffer_Unlock( SBuffer, ( void * )LockData.WriteP, LockData.Length,
										( void * )LockData.WriteP2, LockData.Length2 ) ; 

			// 次に転送処理を行うオフセットの変更
			{
				Sound->Stream.SoundBufferCompCopyOffset += MoveB ;
				if( Sound->Stream.SoundBufferCompCopyOffset >= ( int )Sound->Stream.SoundBufferSize )
					Sound->Stream.SoundBufferCompCopyOffset -= ( int )Sound->Stream.SoundBufferSize ;

				Sound->Stream.SoundBufferCopyStartOffset = ( int )( Sound->Stream.SoundBufferCompCopyOffset - SOUNDSIZE( Sound->BufferFormat.nAvgBytesPerSec * Use_STS_ADVANCECOPYSEC / STS_DIVNUM, Sound->BufferFormat.nBlockAlign ) ) ;
				if( Sound->Stream.SoundBufferCopyStartOffset < 0 )
					Sound->Stream.SoundBufferCopyStartOffset += Sound->Stream.SoundBufferSize ;
			}
		}

		// 終了
		goto END ;
	}
	else
	{
		// 転送処理準備
		{
			// 現在演奏されているかチェック

			// 初期化中でもなく、演奏中でもない場合はここで終了
			if( SoundBuffer_CheckPlay( SBuffer ) == FALSE )
			{
				if( Sound->Stream.SoundBufferCompCopyOffset == -800 )
				{
					MoveByte = AdjustSoundDataBlock( ( int )( Sound->BufferFormat.nAvgBytesPerSec * Use_STS_ADVANCECOPYSEC / STS_DIVNUM ), Sound ) ;
					MoveStartOffset = 0 ;
				}
				else
				{
					goto END ;
				}
			}
			else
			{
				DWORD AdvanceReadingSec ;
				MoveByte = AdjustSoundDataBlock( ( int )( Sound->BufferFormat.nAvgBytesPerSec / STS_ONECOPYSEC ), Sound ) ;
				MoveStartOffset = Sound->Stream.SoundBufferCompCopyOffset ;

				// 転送すべきかどうかの判定、現在の先読みの秒数の算出
				{
					DWORD C, S ;

//					SBuffer->GetCurrentPosition( &CurPosition , NULL );
					SoundBuffer_GetCurrentPosition( SBuffer, &CurPosition, NULL ) ;
					
					C = ( DWORD )Sound->Stream.SoundBufferCompCopyOffset ;
					S = ( DWORD )Sound->Stream.SoundBufferCopyStartOffset ;
					if( ( C < S && ( S > CurPosition && C < CurPosition ) ) || 
						( C > S && ( S > CurPosition || C < CurPosition ) ) )
					{
						goto ERR ;
					}

					if( C < S )
					{
						if( CurPosition < C ) CurPosition += Sound->Stream.SoundBufferSize ;
						C += Sound->Stream.SoundBufferSize ;
					}
					AdvanceReadingSec = ( C - CurPosition ) * 0x200 / Sound->BufferFormat.nAvgBytesPerSec ;
				}

				// 先読み秒数が規定を下回っていたら規定の秒数まで一気に転送
				if( AdvanceReadingSec < Use_STS_MINADVANCECOPYSEC * 0x200 / STS_DIVNUM )
				{
					MoveByte = AdjustSoundDataBlock( ( int )( Sound->BufferFormat.nAvgBytesPerSec * ( Use_STS_MINADVANCECOPYSEC * 0x200 / STS_DIVNUM - AdvanceReadingSec ) / 0x200 ), Sound ) ;
				}
			}
		}

		MoveB = MoveByte ;

		// ロック処理
		{
			LockData.StartOffst = ( DWORD )MoveStartOffset ;

//			SBuffer->Lock( MoveStartOffset, MoveByte,
//							( void ** )&(LockData.WriteP), &LockData.Length,
//							( void ** )&(LockData.WriteP2), &LockData.Length2, 0 ) ; 
			SoundBuffer_Lock( SBuffer, ( DWORD )MoveStartOffset, ( DWORD )MoveByte,
							( void ** )&(LockData.WriteP), &LockData.Length,
							( void ** )&(LockData.WriteP2), &LockData.Length2 ) ; 
			LockData.Offset  = 0 ;
			LockData.Offset2 = 0 ;
			LockData.Valid  = LockData.Length ;
			LockData.Valid2 = LockData.Length2 ;
		}

		// データ転送処理
		while( MoveByte != 0 && Sound->Stream.EndWaitFlag == FALSE )
		{
			// 使用するデータをセット
			PlayData = &Sound->Stream.File[ Sound->Stream.FileActive ] ;

			// 転送バイトのセット
			MoveByte2 = MoveByte ;

			// ループ開始位置が設定されている場合はその位置より先まではデータをセットしない
			if( PlayData->LoopStartSamplePosition != -1 && MoveByte2 + Sound->Stream.FileCompCopyLength > PlayData->LoopStartSamplePosition * Sound->BufferFormat.nBlockAlign )
			{
				MoveByte2 = PlayData->LoopStartSamplePosition * Sound->BufferFormat.nBlockAlign - Sound->Stream.FileCompCopyLength ;
				if( MoveByte2 <= 0 )
					MoveByte2 = 0 ;
			}

			// 転送
			MoveByte2 = SoundDataCopy( &LockData, &PlayData->ConvData, ( DWORD )MoveByte2 ) ;

			// 転送量が０バイトの場合は次のファイルに移る
			BreakFlag = FALSE ;
			if( MoveByte2 == 0 )
			{
//				CurPosition = 0;
				if( StreamSoundNextData( Sound, &LockData, ( int )CurPosition ) < 0 )
				{
					BreakFlag = TRUE ;
				}
			}
			else
			{
				MoveByte -= MoveByte2 ;
				Sound->Stream.FileCompCopyLength += MoveByte2 ;
			}

			// ループすべき位置に来ているのに次の再生データに移行していない場合は再生ループ待ち( Sound->Stream.LoopPositionValidFlag が TRUE )の状態なので、ループから抜ける
			if( ( PlayData == &Sound->Stream.File[ Sound->Stream.FileActive ] &&
				  PlayData->LoopStartSamplePosition != -1 &&
				  Sound->Stream.FileCompCopyLength >= PlayData->LoopStartSamplePosition * Sound->BufferFormat.nBlockAlign ) ||
				BreakFlag )
				break ;
		}
		MoveB -= MoveByte ;

		// ロック解除
//		SBuffer->Unlock( ( void * )LockData.WriteP,  LockData.Length,
//						  ( void * )LockData.WriteP2, LockData.Length2 ) ; 
		SoundBuffer_Unlock( SBuffer, ( void * )LockData.WriteP,  LockData.Length,
						  			( void * )LockData.WriteP2, LockData.Length2 ) ; 

		// 次に転送処理を行うオフセットの変更
		if( Sound->Stream.SoundBufferCompCopyOffset == -800 )
		{
//			Sound->Stream.SoundBufferCompCopyOffset = SOUNDSIZE( Sound->BufferFormat.nAvgBytesPerSec * Use_STS_ADVANCECOPYSEC / STS_DIVNUM, Sound->BufferFormat.nBlockAlign ) ;
			Sound->Stream.SoundBufferCompCopyOffset = MoveB ;
			Sound->Stream.SoundBufferCopyStartOffset = 0 ;
		}
		else
		{
			Sound->Stream.SoundBufferCompCopyOffset += MoveB ;
			if( Sound->Stream.SoundBufferCompCopyOffset >= ( int )Sound->Stream.SoundBufferSize )
				Sound->Stream.SoundBufferCompCopyOffset -= ( int )Sound->Stream.SoundBufferSize ;

			Sound->Stream.SoundBufferCopyStartOffset = ( int )( Sound->Stream.SoundBufferCompCopyOffset - SOUNDSIZE( Sound->BufferFormat.nAvgBytesPerSec * Use_STS_ADVANCECOPYSEC / STS_DIVNUM, Sound->BufferFormat.nBlockAlign ) ) ;
			if( Sound->Stream.SoundBufferCopyStartOffset < 0 )
				Sound->Stream.SoundBufferCopyStartOffset += Sound->Stream.SoundBufferSize ;
		}
	}

END :

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// 終了
	return 0 ;

ERR :

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// エラー終了
	return -1 ;
}

// 全ての音の一時停止状態を変更する
extern int PauseSoundMemAll( int PauseFlag )
{
	HANDLELIST *List ;
	SOUND *Sound ;
	int i ;

	if( SoundSysData.InitializeFlag == FALSE )
	{
		return -1 ;
	}

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// 停止するか開始するかで処理を分岐
	if( PauseFlag )
	{
		// 停止する場合

		for( List = HandleManageArray[ DX_HANDLETYPE_SOUND ].ListFirst.Next ; List->Next != NULL ; List = List->Next )
		{
			Sound = ( SOUND * )List->Data ;

			for( i = 0 ; i < MAX_SOUNDBUFFER_NUM ; i ++ )
			{
				if( Sound->Buffer[ i ].Valid == FALSE || SoundBuffer_CheckEnable( &Sound->Buffer[ i ] ) == FALSE )
					continue ;

				// 既に状態保存済みの場合は何もしない
				if( Sound->BufferPlayStateBackupFlagValid[ i ] )
					continue ;

				// サウンドバッファの再生状態を保存
				{
					int IsPlay ;

					IsPlay = SoundBuffer_CheckPlay( &Sound->Buffer[ i ] ) ;
					if( IsPlay != -1 )
					{
						Sound->BufferPlayStateBackupFlagValid[ i ] = TRUE ;

						if( IsPlay )
						{
							Sound->BufferPlayStateBackupFlag[ i ] = TRUE ;

							// 再生されていたら再生を止める
							SoundBuffer_Stop( &Sound->Buffer[ i ], TRUE ) ;
						}
						else
						{
							Sound->BufferPlayStateBackupFlag[ i ] = FALSE ;
						}
					}
					else
					{
						Sound->BufferPlayStateBackupFlagValid[ i ] = FALSE ;
					}
				}
			}
		}

#ifndef DX_NON_BEEP
		// BEEP音も再生されていたら停止する
		if( SoundSysData.BeepPlay )
		{
			SoundBuffer_Stop( &SoundSysData.BeepSoundBuffer[ SoundSysData.BeepSoundBufferUseIndex ].Buffer ) ;
		}
#endif // DX_NON_BEEP
	}
	else
	{
		// 再開する場合

		for( List = HandleManageArray[ DX_HANDLETYPE_SOUND ].ListFirst.Next ; List->Next != NULL ; List = List->Next )
		{
			Sound = ( SOUND * )List->Data ;

			for( i = 0 ; i < MAX_SOUNDBUFFER_NUM ; i ++ )
			{
				if( Sound->Buffer[ i ].Valid == FALSE || SoundBuffer_CheckEnable( &Sound->Buffer[ i ] ) == FALSE ) continue ;

				// サウンドバッファの再生状態が有効で、且つ再生していた場合は再生を再開する
				if( Sound->BufferPlayStateBackupFlagValid[ i ] &&
					Sound->BufferPlayStateBackupFlag[ i ] )
				{
					SoundBuffer_Play( &Sound->Buffer[ i ], Sound->Buffer[ i ].Loop ) ;
				}

				Sound->BufferPlayStateBackupFlagValid[ i ] = FALSE ;
			}
		}

#ifndef DX_NON_BEEP
		// BEEP音も再生状態の場合は再生を再開する
		if( SoundSysData.BeepPlay )
		{
			SoundBuffer_Play( &SoundSysData.BeepSoundBuffer[ SoundSysData.BeepSoundBufferUseIndex ].Buffer, TRUE ) ;
		}
#endif // DX_NON_BEEP
	}

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// 終了
	return 0 ;
}

// ストリームサウンドの再生処理関数
extern int NS_ProcessStreamSoundMem( int SoundHandle )
{
	return ProcessStreamSoundMem_UseGParam( SoundHandle, FALSE ) ;
}

// 有効なストリームサウンドのすべて再生処理関数にかける
extern int NS_ProcessStreamSoundMemAll( void )
{
	HANDLELIST *List ;
	SOUND *Sound ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &SoundSysData.StreamSoundListCriticalSection ) ;

	for( List = SoundSysData.StreamSoundListFirst.Next ; List->Next != NULL ; List = List->Next )
	{
		// 再生準備が完了していなかったら何もせず次へ
		Sound = ( SOUND * )List->Data ;
		if( Sound->Stream.StartSetupCompFlag == FALSE ) continue ;

		NS_ProcessStreamSoundMem( List->Handle ) ;
	}

	// クリティカルセクションの解放
	CriticalSection_Unlock( &SoundSysData.StreamSoundListCriticalSection ) ;

#ifndef DX_NON_BEEP
	// ついでにBEEP音関係の周期的処理も行う
	BeepSound_Process() ;
#endif // DX_NON_BEEP

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// 終了
	return 0 ;
}

// 再生中のすべての３Ｄサウンドのパラメータを更新する
extern int Refresh3DSoundParamAll()
{
	HANDLELIST *List ;
	SOUND *Sound ;
	int i ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &SoundSysData._3DSoundListCriticalSection ) ;

	for( List = SoundSysData._3DSoundListFirst.Next ; List->Next != NULL ; List = List->Next )
	{
		Sound = ( SOUND * )List->Data ;

		// バッファーの数だけ繰り返し
		for( i = 0 ; i < Sound->ValidBufferNum ; i ++ )
		{
			// パラメータ更新予約
			Sound->Buffer[ i ].EmitterDataChangeFlag = TRUE ;

			// 再生中だったら即座に更新
			if( SoundBuffer_CheckPlay( &Sound->Buffer[ i ] ) )
			{
				SoundBuffer_Refresh3DSoundParam( &Sound->Buffer[ i ] ) ;
			}
		}
	}

	// クリティカルセクションの解放
	CriticalSection_Unlock( &SoundSysData._3DSoundListCriticalSection ) ;

	// 終了
	return 0 ;
}

// 再生が終了したらサウンドハンドルを削除するサウンドの処理を行う
extern int ProcessPlayFinishDeleteSoundMemAll( void )
{
	HANDLELIST *List ;
//	SOUND *Sound ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

LOOPSTART:

	for( List = SoundSysData.PlayFinishDeleteSoundListFirst.Next ; List->Next != NULL ; List = List->Next )
	{
		// 再生中ではなかったら削除
		if( NS_CheckSoundMem( List->Handle ) == 0 )
		{
			NS_DeleteSoundMem( List->Handle ) ;
			goto LOOPSTART ;
		}
	}

	// 終了
	return 0 ;
}

// ３Ｄサウンドを再生しているサウンドハンドルに対する処理を行う
extern int ProcessPlay3DSoundMemAll( void )
{
	HANDLELIST *List ;
	SOUND *Sound ;
	int i ;
	int Valid ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &SoundSysData.Play3DSoundListCriticalSection ) ;

LOOPSTART:
	for( List = SoundSysData.Play3DSoundListFirst.Next ; List->Next != NULL ; List = List->Next )
	{
		Sound = ( SOUND * )List->Data ;

		Valid = FALSE ;
		for( i = 0 ; i < Sound->ValidBufferNum ; i ++ )
		{
			if( Sound->Buffer[ i ].Valid == 0 )
			{
				continue ;
			}

			if( SoundBuffer_CycleProcess( &Sound->Buffer[ i ] ) == 0 )
			{
				Valid = TRUE ;
			}
		}

		if( Valid == FALSE )
		{
			Sound->AddPlay3DSoundList = FALSE ;
			SubHandleList( &Sound->Play3DSoundList ) ;
			goto LOOPSTART ;
		}
	}

	// クリティカルセクションの解放
	CriticalSection_Unlock( &SoundSysData.Play3DSoundListCriticalSection ) ;

	// 終了
	return 0 ;
}

// LoadSoundMem2 の実処理関数
extern int LoadSoundMem2_Static(
	LOADSOUND_GPARAM *GParam,
	int SoundHandle,
	const wchar_t *WaveName1,
	const wchar_t *WaveName2,
	int ASyncThread
)
{
	if( CheckSoundSystem_Initialize_PF() == FALSE )
		return -1 ;

	// 一つ目のサウンドデータの追加
	if( AddStreamSoundMemToFile_UseGParam( GParam, WaveName1, 0,  SoundHandle, GParam->CreateSoundDataType, -1, ASyncThread ) == -1 ) goto ERR ;

	// ２つ目のサウンドデータの追加
	if( AddStreamSoundMemToFile_UseGParam( GParam, WaveName2, -1, SoundHandle, GParam->CreateSoundDataType, -1, ASyncThread ) == -1 ) goto ERR ;

	// 再生準備
	SetupStreamSoundMem_UseGParam( SoundHandle, ASyncThread ) ;

	// 正常終了
	return 0 ;

ERR :
	// エラー終了
	return -1 ;
}

#ifndef DX_NON_ASYNCLOAD

// LoadSoundMem2 の非同期読み込みスレッドから呼ばれる関数
static void LoadSoundMem2_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	LOADSOUND_GPARAM *GParam ;
	int SoundHandle ;
	const wchar_t *WaveName1 ;
	const wchar_t *WaveName2 ;
	int Addr ;
	int Result ;

	Addr = 0 ;
	GParam = ( LOADSOUND_GPARAM * )GetASyncLoadParamStruct( AParam->Data, &Addr ) ;
	SoundHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	WaveName1 = GetASyncLoadParamString( AParam->Data, &Addr ) ;
	WaveName2 = GetASyncLoadParamString( AParam->Data, &Addr ) ;

	Result = LoadSoundMem2_Static( GParam, SoundHandle, WaveName1, WaveName2, TRUE ) ;
	DecASyncLoadCount( SoundHandle ) ;
	if( Result < 0 )
	{
		SubHandle( SoundHandle ) ;
	}
}

#endif // DX_NON_ASYNCLOAD

// LoadSoundMem2 のグローバル変数にアクセスしないバージョン
extern int LoadSoundMem2_UseGParam(
	LOADSOUND_GPARAM *GParam,
	const wchar_t *WaveName1,
	const wchar_t *WaveName2,
	int ASyncLoadFlag
)
{
	int SoundHandle = -1 ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
		return -1 ;

	SoundHandle = _CreateSoundHandle( GParam->Create3DSoundFlag, GParam->NotInitSoundMemDelete, FALSE ) ;
	if( SoundHandle == -1 )
		goto ERR ;

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;
		wchar_t FullPath1[ 1024 ] ;
		wchar_t FullPath2[ 1024 ] ;

		ConvertFullPathW_( WaveName1, FullPath1, sizeof( FullPath1 ) ) ;
		ConvertFullPathW_( WaveName2, FullPath2, sizeof( FullPath2 ) ) ;

		// パラメータに必要なメモリのサイズを算出
		Addr = 0 ;
		AddASyncLoadParamStruct( NULL, &Addr, GParam, sizeof( *GParam ) ) ;
		AddASyncLoadParamInt( NULL, &Addr, SoundHandle ) ;
		AddASyncLoadParamString( NULL, &Addr, FullPath1 ) ; 
		AddASyncLoadParamString( NULL, &Addr, FullPath2 ) ; 

		// メモリの確保
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// 処理に必要な情報をセット
		AParam->ProcessFunction = LoadSoundMem2_ASync ;
		Addr = 0 ;
		AddASyncLoadParamStruct( AParam->Data, &Addr, GParam, sizeof( *GParam ) ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, SoundHandle ) ;
		AddASyncLoadParamString( AParam->Data, &Addr, FullPath1 ) ; 
		AddASyncLoadParamString( AParam->Data, &Addr, FullPath2 ) ; 

		// データを追加
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto ERR ;
		}

		// 非同期読み込みカウントをインクリメント
		IncASyncLoadCount( SoundHandle, AParam->Index ) ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		if( LoadSoundMem2_Static( GParam, SoundHandle, WaveName1, WaveName2, FALSE ) < 0 )
			goto ERR ;
	}

	// ハンドルを返す
	return SoundHandle ;

ERR :
	SubHandle( SoundHandle ) ;
	SoundHandle = -1 ;

	// 終了
	return -1 ;
}

// 前奏部とループ部に分かれたサウンドデータの作成
extern int NS_LoadSoundMem2( const TCHAR *WaveName1 , const TCHAR *WaveName2 )
{
#ifdef UNICODE
	return LoadSoundMem2_WCHAR_T(
		WaveName1 , WaveName2
	) ;
#else
	int Result = -1 ;

	TCHAR_TO_WCHAR_T_STRING_BEGIN( WaveName1 )
	TCHAR_TO_WCHAR_T_STRING_BEGIN( WaveName2 )

	TCHAR_TO_WCHAR_T_STRING_SETUP( WaveName1, goto ERR )
	TCHAR_TO_WCHAR_T_STRING_SETUP( WaveName2, goto ERR )

	Result = LoadSoundMem2_WCHAR_T(
		UseWaveName1Buffer , UseWaveName2Buffer
	) ;

ERR :

	TCHAR_TO_WCHAR_T_STRING_END( WaveName1 )
	TCHAR_TO_WCHAR_T_STRING_END( WaveName2 )

	return Result ;
#endif
}

// 前奏部とループ部に分かれたサウンドファイルを読み込みサウンドハンドルを作成する
extern int NS_LoadSoundMem2WithStrLen( const TCHAR *FileName1, size_t FileName1Length, const TCHAR *FileName2, size_t FileName2Length )
{
	int Result = -1 ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_BEGIN( FileName1 )
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_BEGIN( FileName2 )
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_SETUP( FileName1, FileName1Length, goto ERR )
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_SETUP( FileName2, FileName2Length, goto ERR )
	Result = LoadSoundMem2_WCHAR_T( UseFileName1Buffer , UseFileName2Buffer ) ;
ERR :
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName1 )
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName2 )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_BEGIN( FileName1 )
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_BEGIN( FileName2 )
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_SETUP( FileName1, FileName1Length, goto ERR )
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_SETUP( FileName2, FileName2Length, goto ERR )
	Result = LoadSoundMem2_WCHAR_T( UseFileName1Buffer , UseFileName2Buffer ) ;
ERR :
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName1 )
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName2 )
#endif
	return Result ;
}

// 前奏部とループ部に分かれたサウンドデータの作成
extern int LoadSoundMem2_WCHAR_T( const wchar_t *WaveName1 , const wchar_t *WaveName2 )
{
	LOADSOUND_GPARAM GParam ;

	InitLoadSoundGParam( &GParam ) ;

	return LoadSoundMem2_UseGParam( &GParam, WaveName1, WaveName2, GetASyncLoadFlag() ) ;
}

// 主にＢＧＭを読み込むのに適した関数
extern int NS_LoadBGM( const TCHAR *WaveName )
{
#ifdef UNICODE
	return LoadBGM_WCHAR_T(
		WaveName
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( WaveName, return -1 )

	Result = LoadBGM_WCHAR_T(
		UseWaveNameBuffer
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( WaveName )

	return Result ;
#endif
}

// 主にＢＧＭを読み込みサウンドハンドルを作成するのに適した関数
extern int NS_LoadBGMWithStrLen( const TCHAR *FileName, size_t FileNameLength )
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = LoadBGM_WCHAR_T( UseFileNameBuffer ) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = LoadBGM_WCHAR_T( UseFileNameBuffer ) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#endif
	return Result ;
}

// 主にＢＧＭを読み込むのに適した関数
extern int LoadBGM_WCHAR_T( const wchar_t *WaveName )
{
	int Type = SoundSysData.CreateSoundDataType, SoundHandle ;

	if( _WCSICMP( WaveName + _WCSLEN( WaveName ) - 3, L"wav" ) == 0 )
	{
		NS_SetCreateSoundDataType( DX_SOUNDDATATYPE_FILE ) ;
	}
	else
	{
		NS_SetCreateSoundDataType( DX_SOUNDDATATYPE_MEMPRESS ) ;
	}

	SoundHandle = LoadSoundMem_WCHAR_T( WaveName, 1 ) ;
	NS_SetCreateSoundDataType( Type ) ;

	return SoundHandle ;
}

// LoadSoundMem2ByMemImage の実処理関数
static int LoadSoundMem2ByMemImage_Static( 
	LOADSOUND_GPARAM *GParam,
	int SoundHandle,
	const void *FileImageBuffer1,
	int ImageSize1,
	const void *FileImageBuffer2,
	int ImageSize2,
	int ASyncThread
)
{
	if( CheckSoundSystem_Initialize_PF() == FALSE )
		return -1 ;

	// 一つ目のサウンドデータの追加
	if( AddStreamSoundMemToMem_UseGParam( GParam, FileImageBuffer1, ImageSize1,  0, SoundHandle, GParam->CreateSoundDataType, -1, ASyncThread ) == -1 ) goto ERR ;

	// ２つ目のサウンドデータの追加
	if( AddStreamSoundMemToMem_UseGParam( GParam, FileImageBuffer2, ImageSize2, -1, SoundHandle, GParam->CreateSoundDataType, -1, ASyncThread ) == -1 ) goto ERR ;

	// 再生準備
	SetupStreamSoundMem_UseGParam( SoundHandle, ASyncThread ) ;

	// 正常終了
	return 0 ;

ERR :
	// エラー終了
	return -1 ;
}

#ifndef DX_NON_ASYNCLOAD
// LoadSoundMem2ByMemImage の非同期読み込みスレッドから呼ばれる関数
static void LoadSoundMem2ByMemImage_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	LOADSOUND_GPARAM *GParam ;
	int SoundHandle ;
	const void *FileImageBuffer1 ;
	int ImageSize1 ;
	const void *FileImageBuffer2 ;
	int ImageSize2 ;
	int Addr ;
	int Result ;

	Addr = 0 ;
	GParam = ( LOADSOUND_GPARAM * )GetASyncLoadParamStruct( AParam->Data, &Addr ) ;
	SoundHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	FileImageBuffer1 = GetASyncLoadParamVoidP( AParam->Data, &Addr ) ;
	ImageSize1 = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	FileImageBuffer2 = GetASyncLoadParamVoidP( AParam->Data, &Addr ) ;
	ImageSize2 = GetASyncLoadParamInt( AParam->Data, &Addr ) ;

	Result = LoadSoundMem2ByMemImage_Static( GParam, SoundHandle, FileImageBuffer1, ImageSize1, FileImageBuffer2, ImageSize2, TRUE ) ;
	DecASyncLoadCount( SoundHandle ) ;
	if( Result < 0 )
	{
		SubHandle( SoundHandle ) ;
	}
}
#endif // DX_NON_ASYNCLOAD

// LoadSoundMem2ByMemImage のグローバル変数にアクセスしないバージョン
extern int LoadSoundMem2ByMemImage_UseGParam(
	LOADSOUND_GPARAM *GParam,
	const void *FileImageBuffer1,
	int ImageSize1,
	const void *FileImageBuffer2,
	int ImageSize2,
	int ASyncLoadFlag
)
{
	int SoundHandle = -1 ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
		return -1 ;

	SoundHandle = _CreateSoundHandle( GParam->Create3DSoundFlag, GParam->NotInitSoundMemDelete, FALSE ) ;
	if( SoundHandle == -1 )
		goto ERR ;

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;

		// パラメータに必要なメモリのサイズを算出
		Addr = 0 ;
		AddASyncLoadParamStruct( NULL, &Addr, GParam, sizeof( *GParam ) ) ;
		AddASyncLoadParamInt( NULL, &Addr, SoundHandle ) ;
		AddASyncLoadParamConstVoidP( NULL, &Addr, FileImageBuffer1 ) ; 
		AddASyncLoadParamInt( NULL, &Addr, ImageSize1 ) ; 
		AddASyncLoadParamConstVoidP( NULL, &Addr, FileImageBuffer2 ) ; 
		AddASyncLoadParamInt( NULL, &Addr, ImageSize2 ) ; 

		// メモリの確保
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// 処理に必要な情報をセット
		AParam->ProcessFunction = LoadSoundMem2ByMemImage_ASync ;
		Addr = 0 ;
		AddASyncLoadParamStruct( AParam->Data, &Addr, GParam, sizeof( *GParam ) ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, SoundHandle ) ;
		AddASyncLoadParamConstVoidP( AParam->Data, &Addr, FileImageBuffer1 ) ; 
		AddASyncLoadParamInt( AParam->Data, &Addr, ImageSize1 ) ; 
		AddASyncLoadParamConstVoidP( AParam->Data, &Addr, FileImageBuffer2 ) ; 
		AddASyncLoadParamInt( AParam->Data, &Addr, ImageSize2 ) ; 

		// データを追加
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto ERR ;
		}

		// 非同期読み込みカウントをインクリメント
		IncASyncLoadCount( SoundHandle, AParam->Index ) ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		if( LoadSoundMem2ByMemImage_Static( GParam, SoundHandle, FileImageBuffer1, ImageSize1, FileImageBuffer2, ImageSize2, FALSE ) < 0 )
			goto ERR ;
	}

	// ハンドルを返す
	return SoundHandle ;

ERR :
	SubHandle( SoundHandle ) ;
	SoundHandle = -1 ;

	// 終了
	return -1 ;
}

// 前奏部とループ部に分かれたサウンドデータの作成
extern int NS_LoadSoundMem2ByMemImage( const void *FileImageBuffer1, int ImageSize1, const void *FileImageBuffer2, int ImageSize2 )
{
	LOADSOUND_GPARAM GParam ;

	InitLoadSoundGParam( &GParam ) ;

	return LoadSoundMem2ByMemImage_UseGParam( &GParam, FileImageBuffer1, ImageSize1, FileImageBuffer2, ImageSize2, GetASyncLoadFlag() ) ;
}


	
// LOADSOUND_GPARAM のデータをセットする
extern void InitLoadSoundGParam( LOADSOUND_GPARAM *GParam )
{
	GParam->NotInitSoundMemDelete = FALSE ;																// InitSoundMem で削除しないかどうかのフラグ( TRUE:InitSoundMemでは削除しない  FALSE:InitSoundMemで削除する )
	GParam->Create3DSoundFlag					= SoundSysData.Create3DSoundFlag ;						// 3Dサウンドを作成するかどうかのフラグ( TRUE:３Ｄサウンドを作成する  FALSE:３Ｄサウンドを作成しない )
	GParam->CreateSoundDataType					= SoundSysData.CreateSoundDataType ;					// 作成するサウンドデータのデータタイプ
	GParam->CreateSoundPitchRateEnable			= SoundSysData.CreateSoundPitchRateEnable ;				// 作成するサウンドデータのピッチレートが有効かどうか
	GParam->CreateSoundPitchRate				= SoundSysData.CreateSoundPitchRate ;					// 作成するサウンドデータのピッチレート
	GParam->CreateSoundTimeStretchRateEnable	= SoundSysData.CreateSoundTimeStretchRateEnable ;		// 作成するサウンドデータのタイムストレッチレートが有効かどうか
	GParam->CreateSoundTimeStretchRate			= SoundSysData.CreateSoundTimeStretchRate ;				// 作成するサウンドデータのタイムストレッチレート
	GParam->CreateSoundLoopStartTimePosition	= SoundSysData.CreateSoundLoopStartTimePosition ;		// 作成するサウンドデータのループ範囲の先端( ミリ秒 )
	GParam->CreateSoundLoopStartSamplePosition	= SoundSysData.CreateSoundLoopStartSamplePosition ;		// 作成するサウンドデータのループ範囲の先端( サンプル )
	GParam->CreateSoundLoopEndTimePosition		= SoundSysData.CreateSoundLoopEndTimePosition ;			// 作成するサウンドデータのループ範囲の終端( ミリ秒 )
	GParam->CreateSoundLoopEndSamplePosition	= SoundSysData.CreateSoundLoopEndSamplePosition ;		// 作成するサウンドデータのループ範囲の終端( サンプル )
	GParam->CreateSoundIgnoreLoopAreaInfo		= SoundSysData.CreateSoundIgnoreLoopAreaInfo ;			// 作成するサウンドデータでループ範囲情報を無視するかどうか

	// ループ位置の指定がある場合はストリームサウンドとして読み込む
	if( GParam->CreateSoundDataType == DX_SOUNDDATATYPE_MEMNOPRESS &&
		( GParam->CreateSoundLoopStartTimePosition   != GParam->CreateSoundLoopEndTimePosition ||
		  GParam->CreateSoundLoopStartSamplePosition != GParam->CreateSoundLoopEndSamplePosition ) )
	{
		GParam->CreateSoundDataType = DX_SOUNDDATATYPE_MEMPRESS ;
	}

	GParam->DisableReadSoundFunctionMask = SoundSysData.DisableReadSoundFunctionMask ;		// 使用しない読み込み処理のマスク
#ifndef DX_NON_OGGVORBIS
	GParam->OggVorbisBitDepth = SoundSysData.OggVorbisBitDepth ;							// ＯｇｇＶｏｒｂｉｓ使用時のビット深度(1:8bit 2:16bit)
	GParam->OggVorbisFromTheoraFile = SoundSysData.OggVorbisFromTheoraFile ;				// Ogg Theora ファイル中の Vorbis データを参照するかどうかのフラグ( TRUE:Theora ファイル中の Vorbis データを参照する )
#endif
}

// ピッチ変更やタイムストレッチ時の伸縮変更サウンドハンドルをセットアップするヘルパー関数
static int SetupSoundPitchRateTimeStretchRateChangeHandle( int SoundHandle, LOADSOUND_GPARAM *GParam, int SSSHandleI, int BufferNum, int ASyncThread )
{
	int SSSHandle = -1 ;
	int DSSHandle = -1 ;
	void *WaveImage = NULL ;
	int WaveSize ;
	int DSSSampleNum ;
	SOFTSOUND * SSSound = NULL ;
	LOADSOUND_GPARAM TempGParam ;
	int ErrorPos = -1 ;

	SSND_MASKHCHK( SSSHandleI, SSSound ) ;

	// float型のソフトウエアサウンドを作成
	SSSHandle = NS_MakeSoftSoundCustom( SSSound->BufferFormat.nChannels, 32, SSSound->BufferFormat.nSamplesPerSec, SSSound->Wave.BufferSampleNum, TRUE ) ;
	if( SSSHandle < 0 )
	{
		ErrorPos = -1 ;
		goto ERR ;
	}

	// 元がfloat型のソフトウエアサウンドではなかったらfloat型のソフトウエアサウンドに変換
	if( SSSound->BufferFormat.wFormatTag != WAVE_FORMAT_IEEE_FLOAT )
	{
		ConvertIntToFloatSoftSound( SSSHandleI, SSSHandle ) ;
	}
	else
	{
		// 元がfloat型のソフトウエアサウンドの場合はコピーを用意
		CopySoftSound( SSSHandleI, SSSHandle ) ;
	}

	// ピッチ変更がある場合
	if( GParam->CreateSoundPitchRateEnable )
	{
		// ２倍以上に周波数があがる場合は２倍以下になるまで２倍単位で変換を行う
		float PitchRate = GParam->CreateSoundPitchRate ;

		if( PitchRate > 2.0f )
		{
			DSSSampleNum = _FTOL( NS_GetSoftSoundSampleNum( SSSHandle ) * 2.0f ) ;
			DSSHandle = NS_MakeSoftSound( SSSHandle, DSSSampleNum ) ;
			if( DSSHandle < 0 )
			{
				ErrorPos = -2 ;
				goto ERR ;
			}

			while( PitchRate > 2.0f )
			{
				NS_WriteTimeStretchSoftSoundData( SSSHandle, DSSHandle ) ;
				NS_WritePitchShiftSoftSoundData(  DSSHandle, SSSHandle ) ;

				PitchRate /= 2.0f ;
			}

			NS_DeleteSoftSound( DSSHandle ) ;
			DSSHandle = -1 ;
		}

		DSSSampleNum = _FTOL( NS_GetSoftSoundSampleNum( SSSHandle ) * PitchRate ) ;
		DSSHandle = NS_MakeSoftSound( SSSHandle, DSSSampleNum ) ;
		if( DSSHandle < 0 )
		{
			ErrorPos = -3 ;
			goto ERR ;
		}

		NS_WriteTimeStretchSoftSoundData( SSSHandle, DSSHandle ) ;
		NS_WritePitchShiftSoftSoundData(  DSSHandle, SSSHandle ) ;
	}

	// タイムストレッチがある場合
	if( GParam->CreateSoundTimeStretchRateEnable )
	{
		int Temp ;

		DSSSampleNum = _FTOL( NS_GetSoftSoundSampleNum( SSSHandle ) * GParam->CreateSoundTimeStretchRate ) ;
		DSSHandle = NS_MakeSoftSound( SSSHandle, DSSSampleNum ) ;
		if( DSSHandle < 0 )
		{
			ErrorPos = -4 ;
			goto ERR ;
		}

		NS_WriteTimeStretchSoftSoundData( SSSHandle, DSSHandle ) ;

		Temp = SSSHandle ;
		SSSHandle = DSSHandle ;
		DSSHandle = Temp ;
	}

	if( SSND_MASKHCHK( SSSHandle, SSSound ) )
	{
		ErrorPos = -5 ;
		goto ERR ;
	}

	// ＷＡＶＥファイルをでっち上げる
	if( CreateWaveFileImage(
			&WaveImage,
			&WaveSize,
			&SSSound->BufferFormat,
			sizeof( WAVEFORMATEX ),
			SSSound->Wave.Buffer,
			SSSound->Wave.BufferSampleNum * SSSound->BufferFormat.nBlockAlign ) < 0 )
	{
		ErrorPos = -6 ;
		goto ERR ;
	}

	TempGParam = *GParam ;
	TempGParam.CreateSoundDataType				= DX_SOUNDDATATYPE_MEMNOPRESS ;
	TempGParam.CreateSoundPitchRateEnable		= FALSE ;
	TempGParam.CreateSoundPitchRate				= 1.0f ;
	TempGParam.CreateSoundTimeStretchRateEnable = FALSE ;
	TempGParam.CreateSoundTimeStretchRate       = 1.0f ;
	if( LoadSoundMemByMemImageBase_UseGParam( &TempGParam, FALSE, SoundHandle, WaveImage, WaveSize, BufferNum, -1, FALSE, ASyncThread ) < 0 )
	{
		ErrorPos = -7 ;
		goto ERR ;
	}

	// メモリの解放
	DXFREE( WaveImage ) ;
	WaveImage = NULL ;

	NS_DeleteSoftSound( SSSHandle ) ;
	NS_DeleteSoftSound( DSSHandle ) ;
	SSSHandle = -1 ;
	DSSHandle = -1 ;

	// 正常終了
	return 0 ;

	// エラー終了
ERR :
	if( WaveImage != NULL )
	{
		DXFREE( WaveImage ) ;
		WaveImage = NULL ;
	}

	if( SSSHandle >= 0 )
	{
		NS_DeleteSoftSound( SSSHandle ) ;
		SSSHandle = -1 ;
	}

	if( DSSHandle >= 0 )
	{
		NS_DeleteSoftSound( DSSHandle ) ;
		DSSHandle = -1 ;
	}

	return ErrorPos ;
}

// BEEP音関係
#ifndef DX_NON_BEEP

// BEEP音関係の初期化を行う
static	int	BeepSound_Initialize( void )
{
	// BEEP音用のサウンドバッファを作成する
	{
		int i ;
		WAVEFORMATEX wfmtx ;

		_MEMSET( &wfmtx, 0, sizeof( wfmtx ) ) ;
		wfmtx.cbSize			= 0 ;
		wfmtx.wFormatTag		= WAVE_FORMAT_PCM ;
		wfmtx.nChannels			= 1 ;
		wfmtx.nSamplesPerSec	= SOUND_BEEP_SAMPLEPERSEC ;
		wfmtx.wBitsPerSample	= SOUND_BEEP_SAMPLEPERBYTES * 8 ;
		wfmtx.nBlockAlign		= wfmtx.wBitsPerSample * wfmtx.nChannels / 8 ;
		wfmtx.nAvgBytesPerSec	= wfmtx.nSamplesPerSec * wfmtx.nBlockAlign ;

		for( i = 0 ; i < SOUND_BEEPSOUNDBUFFER_NUM ; i ++ )
		{
			if( SoundBuffer_Initialize(
					&SoundSysData.BeepSoundBuffer[ i ].Buffer,
					SOUND_BEEP_SOUNDBUFFER_SAMPLECOUNT * wfmtx.nBlockAlign,
					&wfmtx,
					NULL,
					TRUE,
					FALSE,
					FALSE ) != 0 )
			{
				DXST_LOGFILE_ADDUTF16LE( "\x42\x00\x45\x00\x45\x00\x50\x00\xf3\x97\x28\x75\x6e\x30\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"BEEP音用のサウンドバッファの作成に失敗しました\n" @*/ ) ;
				return -1 ;
			}
		}
	}

	// 終了
	return 0 ;
}

// BEEP音関係の後始末を行う
static	int BeepSound_Terminate( void )
{
	int i ;

	// サウンドバッファを解放
	for( i = 0 ; i < SOUND_BEEPSOUNDBUFFER_NUM ; i ++ )
	{
		// サウンドバッファを解放
		SoundBuffer_Terminate( &SoundSysData.BeepSoundBuffer[ i ].Buffer ) ;

		// メモリを確保していたら解放
		if( SoundSysData.BeepSoundBuffer[ i ].OneCycleSampleBuffer != NULL )
		{
			DXFREE( SoundSysData.BeepSoundBuffer[ i ].OneCycleSampleBuffer ) ;
			SoundSysData.BeepSoundBuffer[ i ].OneCycleSampleBuffer = NULL ;
		}

		SoundSysData.BeepSoundBuffer[ i ].VolumeUpRequest = FALSE ;
		SoundSysData.BeepSoundBuffer[ i ].PlayTime = 0 ;
		SoundSysData.BeepSoundBuffer[ i ].StopRequest = FALSE ;
		SoundSysData.BeepSoundBuffer[ i ].StopTime = 0 ;
	}

	// 情報をリセット
	SoundSysData.BeepFrequency = 0 ;
	SoundSysData.BeepSoundBufferUseIndex = 0 ;
	SoundSysData.BeepPlay = FALSE ;

	// 終了
	return 0 ;
}

// BEEP音関係の周期的処理を行う
static	int BeepSound_Process( void )
{
	int i ;
	int NowCount ;
	SOUND_BEEP_BUFFERDATA *BeepBuf ;

	NowCount = NS_GetNowCount() ;
	BeepBuf = SoundSysData.BeepSoundBuffer ;
	for( i = 0 ; i < SOUND_BEEPSOUNDBUFFER_NUM ; i ++, BeepBuf ++ )
	{
		// サウンドバッファの再生が一定まで進んでいたら次のデータをセットする
		if( SoundBuffer_CheckPlay( &BeepBuf->Buffer ) )
		{
			DWORD PlayPos ;

			SoundBuffer_GetCurrentPosition( &BeepBuf->Buffer, &PlayPos, NULL ) ;
			PlayPos /= SOUND_BEEP_SAMPLEPERBYTES ;
			if( ( PlayPos <  BeepBuf->NextFillBufferPosition && BeepBuf->NextFillBufferPosition - PlayPos <= SOUND_BEEP_ONE_WRITE_SAMPLES ) ||
				( PlayPos >= BeepBuf->NextFillBufferPosition && BeepBuf->NextFillBufferPosition + ( SOUND_BEEP_SOUNDBUFFER_SAMPLECOUNT - PlayPos ) <= SOUND_BEEP_ONE_WRITE_SAMPLES ) )
			{
				BeepSound_FillSamples( i ) ;
			}
		}

		// BEEP音用サウンドバッファの音量アップリクエストがあった場合は一定時間経過していたら停止する
		if( SoundSysData.BeepSoundBuffer[ i ].VolumeUpRequest )
		{
			if( NowCount -  SoundSysData.BeepSoundBuffer[ i ].PlayTime >= SOUND_BEEP_PLAY_VOLUME_DELAY ||
				NowCount <  SoundSysData.BeepSoundBuffer[ i ].PlayTime )
			{
				SoundBuffer_SetVolume( &SoundSysData.BeepSoundBuffer[ i ].Buffer, 0, 0 ) ;
				SoundSysData.BeepSoundBuffer[ i ].VolumeUpRequest = FALSE ;
				SoundSysData.BeepSoundBuffer[ i ].PlayTime = 0 ;
			}
		}

		// BEEP音用サウンドバッファの停止リクエストがあった場合は一定時間経過していたら停止する
		if( SoundSysData.BeepSoundBuffer[ i ].StopRequest )
		{
			if( NowCount -  SoundSysData.BeepSoundBuffer[ i ].StopTime >= SOUND_BEEP_STOP_DELAY ||
				NowCount <  SoundSysData.BeepSoundBuffer[ i ].StopTime )
			{
				SoundBuffer_Stop( &SoundSysData.BeepSoundBuffer[ i ].Buffer ) ;
				SoundSysData.BeepSoundBuffer[ i ].StopRequest = FALSE ;
				SoundSysData.BeepSoundBuffer[ i ].StopTime = 0 ;
			}
		}
	}

	// 終了
	return 0 ;
}

// BEEP音の波形データを書き込む
static	int	BeepSound_FillSamples( int BufferIndex )
{
	SOUND_BEEP_BUFFERDATA *BeepBuf ;
	LPVOID write1 ;
	DWORD length1 ;
	LPVOID write2 ;
	DWORD length2 ;

	BeepBuf = &SoundSysData.BeepSoundBuffer[ BufferIndex ] ;
	
	// バッファのロック
	if( SoundBuffer_Lock(
			&BeepBuf->Buffer,
			BeepBuf->NextFillBufferPosition * SOUND_BEEP_SAMPLEPERBYTES,
			SOUND_BEEP_ONE_WRITE_SAMPLES * SOUND_BEEP_SAMPLEPERBYTES,
			&write1,
			&length1,
			&write2,
			&length2 ) != 0 )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x42\x00\x45\x00\x45\x00\x50\x00\xf3\x97\x28\x75\x6e\x30\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\xed\x30\xc3\x30\xaf\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"BEEP音用のサウンドバッファのロックに失敗しました\n" @*/ ) ;
		return -1 ;
	}

	// 波形データの書き込み
	{
		DWORD i ;
		DWORD WriteSamples ;
		short *DestAddress ;

		DestAddress = ( short * )write1 ;
		WriteSamples = length1 / SOUND_BEEP_SAMPLEPERBYTES ;
		for( i = 0 ; i < WriteSamples ; i ++ )
		{
			*DestAddress = BeepBuf->OneCycleSampleBuffer[ BeepBuf->NextUseOneCycleSampleBufferPosition ] ;
			DestAddress ++ ;

			BeepBuf->NextFillBufferPosition ++ ;
			if( BeepBuf->NextFillBufferPosition == SOUND_BEEP_SOUNDBUFFER_SAMPLECOUNT )
			{
				BeepBuf->NextFillBufferPosition = 0 ;
			}

			BeepBuf->NextUseOneCycleSampleBufferPosition ++ ;
			if( BeepBuf->NextUseOneCycleSampleBufferPosition == BeepBuf->OneCycleSamples )
			{
				BeepBuf->NextUseOneCycleSampleBufferPosition = 0 ;
			}
		}

		if( write2 != 0 )
		{
			DestAddress = ( short * )write2 ;
			WriteSamples = length2 / SOUND_BEEP_SAMPLEPERBYTES ;
			for( i = 0 ; i < WriteSamples ; i ++ )
			{
				*DestAddress = BeepBuf->OneCycleSampleBuffer[ BeepBuf->NextUseOneCycleSampleBufferPosition ] ;
				DestAddress ++ ;

				BeepBuf->NextFillBufferPosition ++ ;
				if( BeepBuf->NextFillBufferPosition == SOUND_BEEP_SOUNDBUFFER_SAMPLECOUNT )
				{
					BeepBuf->NextFillBufferPosition = 0 ;
				}

				BeepBuf->NextUseOneCycleSampleBufferPosition ++ ;
				if( BeepBuf->NextUseOneCycleSampleBufferPosition == BeepBuf->OneCycleSamples )
				{
					BeepBuf->NextUseOneCycleSampleBufferPosition = 0 ;
				}
			}
		}
	}

	// バッファのロック解除
	SoundBuffer_Unlock(
		&BeepBuf->Buffer,
		write1,
		length1,
		write2,
		length2
	) ;

	// 正常終了
	return 0 ;
}

#endif // DX_NON_BEEP

// ループ範囲指定用テキストファイルのファイルパスを作成する
static int CreateSoundLoopAreaTxtFilePath( const wchar_t *Path, wchar_t *Dest, size_t BufferBytes )
{
	int   i, j ;
	int   LastPoint ;
	DWORD CharCode ;
	int   CharBytes ;

	// 一番後ろの . の位置を調べる
	LastPoint = -1 ;
	i = 0 ;
	for(;;)
	{
		CharCode = GetCharCode( ( const char * )&( ( BYTE * )Path )[ i ], WCHAR_T_CHARCODEFORMAT, &CharBytes ) ;
		if( CharCode == '\0' )
		{
			break ;
		}

		if( CharCode == '.' )
		{
			LastPoint = i ;
		}

		i += CharBytes ;
	}

	// ループ範囲指定用テキストファイルネームの作成
	i = 0 ;
	for(;;)
	{
		CharCode = GetCharCode( ( const char * )&( ( BYTE * )Path )[ i ], WCHAR_T_CHARCODEFORMAT, &CharBytes ) ;
		if( CharCode == '\0' || i == LastPoint )
		{
			break ;
		}

		PutCharCode( CharCode, WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )Dest )[ i ], BufferBytes - i ) ;
		i += CharBytes ;
	}

	j = i ;
	j += PutCharCode( '_',  WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )Dest )[ j ], BufferBytes - j ) ;
	j += PutCharCode( 'l',  WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )Dest )[ j ], BufferBytes - j ) ;
	j += PutCharCode( 'o',  WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )Dest )[ j ], BufferBytes - j ) ;
	j += PutCharCode( 'o',  WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )Dest )[ j ], BufferBytes - j ) ;
	j += PutCharCode( 'p',  WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )Dest )[ j ], BufferBytes - j ) ;
	j += PutCharCode( '.',  WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )Dest )[ j ], BufferBytes - j ) ;
	j += PutCharCode( 't',  WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )Dest )[ j ], BufferBytes - j ) ;
	j += PutCharCode( 'x',  WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )Dest )[ j ], BufferBytes - j ) ;
	j += PutCharCode( 't',  WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )Dest )[ j ], BufferBytes - j ) ;
	j += PutCharCode( '\0', WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )Dest )[ j ], BufferBytes - j ) ;

	return 0 ;
}

// ループ範囲指定用テキストファイルを読み込む
static int LoadSoundLoopAreaTxtFile( const wchar_t *LoopFilePath, int *LoopStart, int *LoopEnd )
{
	BYTE *FileImage ;
	int FileSize ;
	int Result = -1 ;
	wchar_t WCharStr[ 256 ] = { 0 } ;
	wchar_t *p, *p2 ;
	int LoopStartPos ;
	int LoopEndPos ;
	int IsLoopLength ;

	// ファイルを丸ごと読み込み
	if( FileFullRead( LoopFilePath, ( void ** )&FileImage, &FileSize, FALSE ) < 0 )
	{
		return -1 ;
	}

	if( FileSize < 3 )
		goto END ;

	// UTF-16LE UTF-16BE UTF-8 を Ascii コードに変換
	{
		int Count = 0 ;

		if( FileSize > Count + 0 && FileImage[ Count + 0 ] == 0xff &&
			FileSize > Count + 1 && FileImage[ Count + 1 ] == 0xfe )
		{
			Count += 2 ;
			ConvString( ( char * )&FileImage[ Count ], -1, DX_CHARCODEFORMAT_UTF16LE, ( char * )WCharStr, sizeof( WCharStr ) - 32, WCHAR_T_CHARCODEFORMAT ) ;
		}
		else
		if( FileSize > Count + 0 && FileImage[ Count + 0 ] == 0xfe &&
			FileSize > Count + 1 && FileImage[ Count + 1 ] == 0xff )
		{
			Count += 2 ;
			ConvString( ( char * )&FileImage[ Count ], -1, DX_CHARCODEFORMAT_UTF16BE, ( char * )WCharStr, sizeof( WCharStr ) - 32, WCHAR_T_CHARCODEFORMAT ) ;
		}
		else
		if( FileSize > Count + 0 && FileImage[ Count + 0 ] == 0xef &&
			FileSize > Count + 1 && FileImage[ Count + 1 ] == 0xbb &&
			FileSize > Count + 2 && FileImage[ Count + 2 ] == 0xbf )
		{
			Count += 3 ;
			ConvString( ( char * )&FileImage[ Count ], -1, DX_CHARCODEFORMAT_UTF8, ( char * )WCharStr, sizeof( WCharStr ) - 32, WCHAR_T_CHARCODEFORMAT ) ;
		}
		else
		{
			ConvString( ( char * )&FileImage[ Count ], -1, DX_CHARCODEFORMAT_SHIFTJIS, ( char * )WCharStr, sizeof( WCharStr ) - 32, WCHAR_T_CHARCODEFORMAT ) ;
		}
	}

	// LOOPSTART を読み込み
	{
		for( p = WCharStr ; *p != L'\0' && *p != L'L' ; p++ ){}
		if( CL_strncmp( WCHAR_T_CHARCODEFORMAT, ( char * )p, ( char * )L"LOOPSTART", 9 ) != 0 )
		{
			goto END ;
		}
		p += 9 ;

		for( ; *p != L'\0' && *p != L'=' ; p++ ){}
		if( *p == L'\0' )
		{
			goto END ;
		}
		for( ; *p != L'\0' && ( *p < L'0' || *p > L'9' ) ; p++ ){}
		if( *p == L'\0' )
		{
			goto END ;
		}

		for( p2 = p ; *p2 >= L'0' && *p2 <= L'9' ; p2++ ){}
		if( *p2 == L'\0' )
		{
			goto END ;
		}
		*p2 = L'\0' ;
		LoopStartPos = CL_atoi( WCHAR_T_CHARCODEFORMAT, ( char * )p ) ;
	}

	// LOOPEND または LOOPLENGTH を読み込み
	{
		for( p = p2 + 1 ; *p != L'\0' && *p != L'L' ; p++ ){}
		if( CL_strncmp( WCHAR_T_CHARCODEFORMAT, ( char * )p, ( char * )L"LOOPEND", 7 ) == 0 )
		{
			IsLoopLength = FALSE ;
			p += 7 ;
		}
		else
		if( CL_strncmp( WCHAR_T_CHARCODEFORMAT, ( char * )p, ( char * )L"LOOPLENGTH", 10 ) == 0 )
		{
			IsLoopLength = TRUE ;
			p += 10 ;
		}
		else
		{
			goto END ;
		}

		for( ; *p != L'\0' && *p != L'=' ; p++ ){}
		if( *p == L'\0' )
		{
			goto END ;
		}
		for( ; *p != L'\0' && ( *p < L'0' || *p > L'9' ) ; p++ ){}
		if( *p == L'\0' )
		{
			goto END ;
		}

		for( p2 = p ; *p2 >= L'0' && *p2 <= L'9' ; p2++ ){}
		*p2 = '\0' ;
		if( IsLoopLength )
		{
			LoopEndPos = LoopStartPos + CL_atoi( WCHAR_T_CHARCODEFORMAT, ( char * )p ) ;
		}
		else
		{
			LoopEndPos = CL_atoi( WCHAR_T_CHARCODEFORMAT, ( char * )p ) ;
		}
	}

	// パラメータのチェック
	if( LoopStartPos >= LoopEndPos )
	{
		goto END ;
	}

	// 値の保存
	if( LoopStart ) *LoopStart = LoopStartPos ;
	if( LoopEnd   ) *LoopEnd   = LoopEndPos ;

	// 正常終了値をセット
	Result = 0 ;

END :

	// メモリの解放
	DXFREE( FileImage ) ;
	FileImage = NULL ;

	// 終了
	return Result ;
}

// LoadSoundMemBase の実処理関数
static int LoadSoundMemBase_Static(
	LOADSOUND_GPARAM *GParam,
	int SoundHandle,
	const wchar_t *WaveName,
	int BufferNum,
	int UnionHandle,
	int ASyncThread
)
{
	void *SrcBuffer = NULL ;
	int SrcSize ;
	int SSSHandleI = -1 ;
	int SetLoopAreaCancel = FALSE ;
	wchar_t LoopPosTxtName[ FILEPATH_MAX ] ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
		return -1 ;

	if( BufferNum > MAX_SOUNDBUFFER_NUM ) BufferNum = MAX_SOUNDBUFFER_NUM ;
	
	// ループ範囲情報テキストファイルの読み込み
	if( ( GParam->CreateSoundLoopStartTimePosition   == GParam->CreateSoundLoopEndTimePosition   ) &&
		( GParam->CreateSoundLoopStartSamplePosition == GParam->CreateSoundLoopEndSamplePosition ) )
	{
		int LoopStartPos ;
		int LoopEndPos ;

		CreateSoundLoopAreaTxtFilePath( WaveName, LoopPosTxtName, sizeof( LoopPosTxtName ) ) ;
		if( LoadSoundLoopAreaTxtFile( LoopPosTxtName, &LoopStartPos, &LoopEndPos ) >= 0 )
		{
			GParam->CreateSoundLoopStartSamplePosition = LoopStartPos ;
			GParam->CreateSoundLoopEndSamplePosition   = LoopEndPos ;

			// ループ位置の指定があった場合はストリームサウンドとして読み込む
			if( GParam->CreateSoundDataType == DX_SOUNDDATATYPE_MEMNOPRESS )
			{
				GParam->CreateSoundDataType = DX_SOUNDDATATYPE_MEMPRESS ;
			}
		}
	}

	// ピッチ変更かタイムストレッチがある場合は伸縮して読み込む
	if( GParam->CreateSoundPitchRateEnable || GParam->CreateSoundTimeStretchRateEnable )
	{
		int Result ;

		SSSHandleI = LoadSoftSound_WCHAR_T( WaveName ) ;
		if( SSSHandleI < 0 )
		{
			DXST_LOGFILEFMT_ADDW(( L"Sound File Load Error 2 : %s" , WaveName )) ;
			goto ERR ;
		}

		Result = SetupSoundPitchRateTimeStretchRateChangeHandle( SoundHandle, GParam, SSSHandleI, BufferNum, ASyncThread ) ;
		if( Result < 0 )
		{
			DXST_LOGFILEFMT_ADDW(( L"Sound File Setup Error %d : %s" , -Result, WaveName )) ;
			goto ERR ;
		}

		NS_DeleteSoftSound( SSSHandleI ) ;
		SSSHandleI = -1 ;
	}
	else
	{
		// データタイプがメモリに展開する以外の場合、ストリームデータとしてサウンドデータを作成する
		if( GParam->CreateSoundDataType != DX_SOUNDDATATYPE_MEMNOPRESS )
		{
			// ストリーム系のサウンドデータの追加
			if( AddStreamSoundMemToFile_UseGParam( GParam, WaveName, 0, SoundHandle, GParam->CreateSoundDataType, UnionHandle, ASyncThread ) == -1 )
				goto ERR ;

			// 再生準備
			SetupStreamSoundMem_UseGParam( SoundHandle, ASyncThread ) ;
		}
		else
		{
			// 丸々メモリに読み込む
			if( FileFullRead( WaveName, &SrcBuffer, &SrcSize ) < 0 )
			{
				DXST_LOGFILEFMT_ADDW(( L"Sound File Load Error : %s", WaveName )) ;
				goto ERR ;
			}

			// メモリから読み込む関数に渡す
			if( LoadSoundMemByMemImageBase_UseGParam( GParam, FALSE, SoundHandle, SrcBuffer, SrcSize, BufferNum, -1, FALSE, ASyncThread ) < 0 )
			{
				DXST_LOGFILEFMT_ADDW(( L"Sound File Setup Error : %s" , WaveName )) ;
				goto ERR ;
			}

			// LoadSoundMemByMemImageBase_UseGParam の中でループ位置指定をするので、
			// LoadSoundMemByMemImageBase_UseGParam を実行した場合はループ位置指定処理はキャンセルする
			SetLoopAreaCancel = TRUE ;

			// メモリの解放
			DXFREE( SrcBuffer ) ;
			SrcBuffer = NULL ;
		}
	}

	// ループ位置の指定がある場合はループ位置を設定する
	if( SetLoopAreaCancel == FALSE )
	{
		if( GParam->CreateSoundLoopStartTimePosition != GParam->CreateSoundLoopEndTimePosition )
		{
			SetLoopAreaTimePosSoundMem_UseGParam( 
				GParam->CreateSoundLoopStartTimePosition,
				GParam->CreateSoundLoopEndTimePosition,
				SoundHandle,
				ASyncThread
			) ;
		}
		else
		if( GParam->CreateSoundLoopStartSamplePosition != GParam->CreateSoundLoopEndSamplePosition )
		{
			SetLoopAreaSamplePosSoundMem_UseGParam( 
				GParam->CreateSoundLoopStartSamplePosition,
				GParam->CreateSoundLoopEndSamplePosition,
				SoundHandle,
				ASyncThread
			) ;
		}
	}

	// 正常終了
	return 0 ;

ERR :
	if( SrcBuffer != NULL )
	{
		DXFREE( SrcBuffer ) ;
		SrcBuffer = NULL ;
	}

	if( SSSHandleI >= 0 )
	{
		NS_DeleteSoftSound( SSSHandleI ) ;
		SSSHandleI = -1 ;
	}

	return -1 ;
}


#ifndef DX_NON_ASYNCLOAD
// LoadSoundMemBase の非同期読み込みスレッドから呼ばれる関数
static void LoadSoundMemBase_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	LOADSOUND_GPARAM *GParam ;
	int SoundHandle ;
	const wchar_t *WaveName ;
	int BufferNum ;
	int UnionHandle ;
	int Addr ;
	int Result ;

	Addr = 0 ;
	GParam = ( LOADSOUND_GPARAM * )GetASyncLoadParamStruct( AParam->Data, &Addr ) ;
	SoundHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	WaveName = GetASyncLoadParamString( AParam->Data, &Addr ) ;
	BufferNum = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	UnionHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;

	Result = LoadSoundMemBase_Static( GParam, SoundHandle, WaveName, BufferNum, UnionHandle, TRUE ) ;
	DecASyncLoadCount( SoundHandle ) ;
	if( Result < 0 )
	{
		SubHandle( SoundHandle ) ;
	}
}
#endif // DX_NON_ASYNCLOAD

// LoadSoundMemBase のグローバル変数にアクセスしないバージョン
extern int LoadSoundMemBase_UseGParam(
	LOADSOUND_GPARAM *GParam,
	const wchar_t *WaveName,
	int BufferNum,
	int UnionHandle,
	int ASyncLoadFlag,
	int ASyncThread 
)
{
	int SoundHandle = -1 ;
	wchar_t FullPath[ 1024 ] ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
		return -1 ;

	SoundHandle = _CreateSoundHandle( GParam->Create3DSoundFlag, GParam->NotInitSoundMemDelete, ASyncThread ) ;
	if( SoundHandle == -1 )
		goto ERR ;

	ConvertFullPathW_( WaveName, FullPath, sizeof( FullPath ) ) ;

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag && ASyncThread == FALSE )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;

		// パラメータに必要なメモリのサイズを算出
		Addr = 0 ;
		AddASyncLoadParamStruct( NULL, &Addr, GParam, sizeof( *GParam ) ) ;
		AddASyncLoadParamInt( NULL, &Addr, SoundHandle ) ;
		AddASyncLoadParamString( NULL, &Addr, FullPath ) ; 
		AddASyncLoadParamInt( NULL, &Addr, BufferNum ) ;
		AddASyncLoadParamInt( NULL, &Addr, UnionHandle ) ;

		// メモリの確保
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// 処理に必要な情報をセット
		AParam->ProcessFunction = LoadSoundMemBase_ASync ;
		Addr = 0 ;
		AddASyncLoadParamStruct( AParam->Data, &Addr, GParam, sizeof( *GParam ) ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, SoundHandle ) ;
		AddASyncLoadParamString( AParam->Data, &Addr, FullPath ) ; 
		AddASyncLoadParamInt( AParam->Data, &Addr, BufferNum ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, UnionHandle ) ;

		// データを追加
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto ERR ;
		}

		// 非同期読み込みカウントをインクリメント
		IncASyncLoadCount( SoundHandle, AParam->Index ) ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		if( LoadSoundMemBase_Static( GParam, SoundHandle, FullPath, BufferNum, UnionHandle, ASyncThread ) < 0 )
			goto ERR ;
	}

#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		DecASyncLoadCount( SoundHandle ) ;
	}
#endif // DX_NON_ASYNCLOAD

	// ハンドルを返す
	return SoundHandle ;

ERR :
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		DecASyncLoadCount( SoundHandle ) ;
	}
#endif // DX_NON_ASYNCLOAD

	SubHandle( SoundHandle ) ;
	SoundHandle = -1 ;

	// 終了
	return -1 ;
}

// サウンドデータを追加する
extern int NS_LoadSoundMemBase( const TCHAR *WaveName, int BufferNum, int UnionHandle )
{
#ifdef UNICODE
	return LoadSoundMemBase_WCHAR_T(
		WaveName, BufferNum, UnionHandle
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( WaveName, return -1 )

	Result = LoadSoundMemBase_WCHAR_T(
		UseWaveNameBuffer, BufferNum, UnionHandle
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( WaveName )

	return Result ;
#endif
}

// サウンドファイルからサウンドハンドルを作成する
extern int NS_LoadSoundMemBaseWithStrLen( const TCHAR *FileName, size_t FileNameLength, int BufferNum, int UnionHandle )
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = LoadSoundMemBase_WCHAR_T( UseFileNameBuffer, BufferNum, UnionHandle ) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = LoadSoundMemBase_WCHAR_T( UseFileNameBuffer, BufferNum, UnionHandle ) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#endif
	return Result ;
}

// サウンドデータを追加する
extern int LoadSoundMemBase_WCHAR_T( const wchar_t *WaveName, int BufferNum, int UnionHandle )
{
	LOADSOUND_GPARAM GParam ;

	InitLoadSoundGParam( &GParam ) ;

	return LoadSoundMemBase_UseGParam( &GParam, WaveName, BufferNum, UnionHandle, GetASyncLoadFlag() ) ;
}

// サウンドデータを追加する
extern int NS_LoadSoundMem( const TCHAR *WaveName, int BufferNum, int UnionHandle )
{
#ifdef UNICODE
	return LoadSoundMem_WCHAR_T(
		WaveName, BufferNum, UnionHandle
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( WaveName, return -1 )

	Result = LoadSoundMem_WCHAR_T(
		UseWaveNameBuffer, BufferNum, UnionHandle
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( WaveName )

	return Result ;
#endif
}

// LoadSoundMemBase の別名関数
extern int NS_LoadSoundMemWithStrLen( const TCHAR *FileName, size_t FileNameLength, int BufferNum, int UnionHandle )
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = LoadSoundMem_WCHAR_T( UseFileNameBuffer, BufferNum, UnionHandle ) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = LoadSoundMem_WCHAR_T( UseFileNameBuffer, BufferNum, UnionHandle ) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#endif
	return Result ;
}

// サウンドデータを追加する
extern int LoadSoundMem_WCHAR_T( const wchar_t *WaveName, int BufferNum, int UnionHandle )
{
	return LoadSoundMemBase_WCHAR_T( WaveName, BufferNum, UnionHandle ) ;
}

// 同時再生数指定型サウンド追加関数
extern int NS_LoadSoundMemToBufNumSitei( const TCHAR *WaveName, int BufferNum )
{
#ifdef UNICODE
	return LoadSoundMemToBufNumSitei_WCHAR_T(
		WaveName, BufferNum
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( WaveName, return -1 )

	Result = LoadSoundMemToBufNumSitei_WCHAR_T(
		UseWaveNameBuffer, BufferNum
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( WaveName )

	return Result ;
#endif
}

// LoadSoundMem を使用して下さい
extern int NS_LoadSoundMemToBufNumSiteiWithStrLen( const TCHAR *FileName, size_t FileNameLength, int BufferNum )
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = LoadSoundMemToBufNumSitei_WCHAR_T( UseFileNameBuffer, BufferNum ) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = LoadSoundMemToBufNumSitei_WCHAR_T( UseFileNameBuffer, BufferNum ) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#endif
	return Result ;
}

// 同時再生数指定型サウンド追加関数
extern int LoadSoundMemToBufNumSitei_WCHAR_T( const wchar_t *WaveName, int BufferNum )
{
	return LoadSoundMemBase_WCHAR_T( WaveName, BufferNum, -1 ) ;
}

// 同じサウンドデータを使用するサウンドハンドルを作成する
extern int NS_DuplicateSoundMem( int SrcSoundHandle, int BufferNum )
{
	int i ;
	int Handle = -1 ;
	int hr ;
	void *write1,  *write2 ;
	DWORD length1, length2 ;
	SOUND *Sound, *SrcSound ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
		return -1 ;

	// エラー判定
	if( SOUNDHCHK( SrcSoundHandle, SrcSound ) )
		goto ERR ;

	// サウンドタイプがストリーム風サウンドだった場合は複製できない
	if( SrcSound->Type == DX_SOUNDTYPE_STREAMSTYLE )
		goto ERR ;

	if( BufferNum > MAX_SOUNDBUFFER_NUM ) BufferNum = MAX_SOUNDBUFFER_NUM ;

	// 新しいサウンドデータの追加
	Handle = _CreateSoundHandle( SrcSound->Is3DSound, SrcSound->NotInitSoundMemDelete, FALSE ) ;
	if( Handle < 0 )
	{
		goto ERR ;
	}

	Sound = ( SOUND * )HandleManageArray[ DX_HANDLETYPE_SOUND ].Handle[ Handle & DX_HANDLEINDEX_MASK ] ;

	// フォーマットを保存
	Sound->BufferFormat = SrcSound->BufferFormat ;

	// WaveData のポインタを保存
	Sound->Normal.WaveData = SrcSound->Normal.WaveData ;
	Sound->Normal.WaveSize = SrcSound->Normal.WaveSize ;

	// WaveData の参照数を増やす
	Sound->Normal.WaveDataUseCount = SrcSound->Normal.WaveDataUseCount ;
	*Sound->Normal.WaveDataUseCount = *Sound->Normal.WaveDataUseCount + 1 ;

	// ３Ｄサウンドの場合はバッファ数は１
	if( Sound->Is3DSound )
		BufferNum = 1 ;

	// サウンドバッファを作成
	if( ( CreateSoundBuffer( &Sound->BufferFormat, ( DWORD )Sound->Normal.WaveSize, DX_SOUNDTYPE_NORMAL, BufferNum, Handle, SrcSoundHandle, FALSE ) ) == -1 )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x07\x89\xfd\x88\xb5\x30\xa6\x30\xf3\x30\xc9\x30\x6e\x30\x20\x00\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x53\x00\x6f\x00\x75\x00\x6e\x00\x64\x00\x42\x00\x75\x00\x66\x00\x66\x00\x65\x00\x72\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"複製サウンドのサウンドバッファの作成に失敗しました\n" @*/ ) ;
		goto ERR ;
	}
	Sound->BaseFrequency   = ( int )Sound->BufferFormat.nSamplesPerSec ;

	Sound->PitchRateEnable = SrcSound->PitchRateEnable ;
	Sound->PitchRate       = SrcSound->PitchRate ;

	// サウンドデータをサウンドバッファにコピーする
	for( i = 0 ; i < Sound->ValidBufferNum ; i ++ )
	{
		// 別のサウンドバッファの複製だった場合は転送作業の必要が無いので転送しない
		// ( 正常に動作しない環境があったためとりあえずフラグは無視 )
//		if( Sound->Normal.BufferDuplicateFlag[ i ] == TRUE ) continue ;

		hr = SoundBuffer_Lock( &Sound->Buffer[ i ], 0, ( DWORD )Sound->Normal.WaveSize,
								&write1, &length1,
								&write2, &length2 ) ;
		if( hr != 0 )
		{
			DXST_LOGFILE_ADDUTF16LE( "\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xc7\x30\xfc\x30\xbf\x30\x6e\x30\xe2\x8e\x01\x90\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x5f\x00\x31\x00\x00"/*@ L"サウンドデータの転送に失敗しました_1" @*/ ) ;
			goto ERR ;
		}

		_MEMCPY( write1, Sound->Normal.WaveData, length1 ) ;
		if( write2 != 0 ) _MEMCPY( write2, (BYTE *)Sound->Normal.WaveData + length1, length2 ) ;

		hr = SoundBuffer_Unlock( &Sound->Buffer[ i ], write1, length1, write2, length2 ) ;
		if( hr != 0 )
		{
			DXST_LOGFILE_ADDUTF16LE( "\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xc7\x30\xfc\x30\xbf\x30\x6e\x30\xe2\x8e\x01\x90\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x5f\x00\x32\x00\x00"/*@ L"サウンドデータの転送に失敗しました_2" @*/ ) ;
			goto ERR ;
		}
	}

	// 終了、サウンドデータハンドルを返す
	return Handle ;

ERR :
	if( Handle != -1 )
	{
		NS_DeleteSoundMem( Handle ) ;
	}
	
	return -1 ;
}

// LoadSoundMemByMemImageBase の実処理関数
static int LoadSoundMemByMemImageBase_Static(
	LOADSOUND_GPARAM *GParam,
	int SoundHandle,
	const void *FileImageBuffer,
	int ImageSize,
	int BufferNum,
	int UnionHandle,
	int ASyncThread
)
{
	int i ;
	WAVEFORMATEX Format ;
	void *SoundBuffer ;
	int SoundSize ;
	int hr ;
	void *write1, *write2 ;
	DWORD length1, length2 ;
	SOUND * Sound ;
	SOUNDCONV ConvData ;
	STREAMDATA Stream ;
	int SSSHandleI = -1 ;

	if( CheckSoundSystem_Initialize_PF() == FALSE || BufferNum > MAX_SOUNDBUFFER_NUM )
		return -1 ;

//	_MEMSET( &ConvData, 0, sizeof( ConvData ) );
	ConvData.InitializeFlag = FALSE ;
	Stream.DataPoint = NULL ;
	SoundBuffer = NULL ;

	Sound = ( SOUND * )HandleManageArray[ DX_HANDLETYPE_SOUND ].Handle[ SoundHandle & DX_HANDLEINDEX_MASK ] ;

//	// ３Ｄサウンドの場合はバッファの数は一つだけ
//	if( Sound->Is3DSound != FALSE )
//		BufferNum = 1 ;

	// ピッチ変更かタイムストレッチがある場合は伸縮して読み込む
	if( GParam->CreateSoundPitchRateEnable || GParam->CreateSoundTimeStretchRateEnable )
	{
		int Result ;

		SSSHandleI = LoadSoftSoundBase_UseGParam( GParam, NULL, FileImageBuffer, ImageSize, ASyncThread ) ;
		if( SSSHandleI < 0 )
		{
			DXST_LOGFILEFMT_ADDW(( L"LoadSoundMemByMemImageBase Error 1" )) ;
			goto ERR ;
		}

		Result = SetupSoundPitchRateTimeStretchRateChangeHandle( SoundHandle, GParam, SSSHandleI, BufferNum, ASyncThread ) ;
		if( Result < 0 )
		{
			DXST_LOGFILEFMT_ADDW(( L"LoadSoundMemByMemImageBase Error %d" , -Result )) ;
			goto ERR ;
		}

		NS_DeleteSoftSound( SSSHandleI ) ;
		SSSHandleI = -1 ;

		// 正常終了
		return 0 ;
	}

	// データタイプがメモリに展開する以外の場合、ストリームデータとしてサウンドデータを作成する
	if( GParam->CreateSoundDataType != DX_SOUNDDATATYPE_MEMNOPRESS )
	{
STREAM_TYPE :
		if( AddStreamSoundMemToMem_UseGParam(
				GParam,
				FileImageBuffer,
				ImageSize,
				0,
				SoundHandle,
				GParam->CreateSoundDataType,
				UnionHandle,
				ASyncThread
			) == -1 )
			goto ERR ;

		// 再生準備
		SetupStreamSoundMem_UseGParam( SoundHandle, ASyncThread ) ;
	}
	else
	{
		int LoopStartPos = -1, LoopEndPos = -1 ;

		// ＰＣＭ形式に変換
		{
			Stream.DataPoint = MemStreamOpen( FileImageBuffer, ( size_t )ImageSize ) ;
			Stream.ReadShred = *GetMemStreamDataShredStruct() ;
			if( SetupSoundConvert( &ConvData, &Stream, GParam->DisableReadSoundFunctionMask
#ifndef DX_NON_OGGVORBIS
								,GParam->OggVorbisBitDepth, GParam->OggVorbisFromTheoraFile
#endif
								) < 0 )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\xf3\x97\xf0\x58\xd5\x30\xa1\x30\xa4\x30\xeb\x30\x6e\x30\x30\xff\x23\xff\x2d\xff\x78\x30\x6e\x30\x09\x59\xdb\x63\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"音声ファイルのＰＣＭへの変換に失敗しました" @*/ )) ;
				goto ERR ;
			}
			if( SoundConvertFast( &ConvData, &Format, &SoundBuffer, &SoundSize ) < 0 )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\xf3\x97\xf0\x58\xd5\x30\xa1\x30\xa4\x30\xeb\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"音声ファイルを格納するメモリ領域の確保に失敗しました" @*/ )) ;
				goto ERR ;
			}

			// ループ情報の取得
			if( ( GParam->CreateSoundLoopStartTimePosition   == GParam->CreateSoundLoopEndTimePosition   ) &&
				( GParam->CreateSoundLoopStartSamplePosition == GParam->CreateSoundLoopEndSamplePosition ) &&
				GParam->CreateSoundIgnoreLoopAreaInfo == FALSE )
			{
				GetSoundConvertLoopAreaInfo( &ConvData, &LoopStartPos, &LoopEndPos ) ;
			}

			TerminateSoundConvert( &ConvData ) ;
			MemStreamClose( Stream.DataPoint ) ;
			Stream.DataPoint = NULL ;
		}

		// ループ情報がある場合はセットしてストリーム形式で再生
		if( LoopStartPos >= 0 && LoopEndPos >= 0 )
		{
			GParam->CreateSoundLoopStartSamplePosition = LoopStartPos ;
			GParam->CreateSoundLoopEndSamplePosition   = LoopEndPos ;

			if( SoundBuffer != NULL )
			{
				DXFREE( SoundBuffer ) ;
				SoundBuffer = NULL ;
			}

			GParam->CreateSoundDataType = DX_SOUNDDATATYPE_MEMPRESS ;
			goto STREAM_TYPE ;
		}

		// 3分以上のサウンドデータはストリーム形式で再生する
		if( SoundSize / Format.nAvgBytesPerSec > 60 * 3 )
		{
			if( SoundBuffer != NULL )
			{
				DXFREE( SoundBuffer ) ;
				SoundBuffer = NULL ;
			}

			GParam->CreateSoundDataType = DX_SOUNDDATATYPE_MEMPRESS ;
			goto STREAM_TYPE ;
		}

		// サウンドバッファを作成
//		DXST_LOGFILEFMT_ADDW(( L"format:%d  channel:%d  samplespersec:%d  AvgBytes:%d  blockalign:%d  BitsPerSample:%d  cbsize:%d\n",
//				Format.wFormatTag, Format.nChannels, Format.nSamplesPerSec, Format.nAvgBytesPerSec, Format.nBlockAlign, Format.wBitsPerSample, Format.cbSize )) ;
		if( ( CreateSoundBuffer( &Format, ( DWORD )SoundSize, DX_SOUNDTYPE_NORMAL, BufferNum, SoundHandle, -1, ASyncThread ) ) == -1 )
		{
			// サウンドバッファの作成に失敗したらストリーム形式で開こうとしてみる
			if( SoundBuffer != NULL )
			{
				DXFREE( SoundBuffer ) ;
				SoundBuffer = NULL ;
			}

			GParam->CreateSoundDataType = DX_SOUNDDATATYPE_MEMPRESS ;
			goto STREAM_TYPE ;
		}
		Sound->BaseFrequency = ( int )Format.nSamplesPerSec ;
//		Sound->PitchRateEnable = GParam->CreateSoundPitchRateEnable ;
//		Sound->PitchRate       = GParam->CreateSoundPitchRate ;

		// 参照数を保存するメモリの確保
		Sound->Normal.WaveDataUseCount = ( int * )DXALLOC( sizeof( int ) ) ;
		if( Sound->Normal.WaveDataUseCount == NULL )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xc7\x30\xfc\x30\xbf\x30\x6e\x30\xc2\x53\x67\x71\x70\x65\x92\x30\xdd\x4f\x58\x5b\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"サウンドデータの参照数を保存するメモリ領域の確保に失敗しました" @*/ )) ;
			goto ERR ;
		}

		Sound->BufferFormat = Format ;			// フォーマットを保存
		Sound->Normal.WaveData = SoundBuffer ;	// データポインタを保存
		Sound->Normal.WaveSize = SoundSize ;	// データサイズを保存
		*Sound->Normal.WaveDataUseCount = 1 ;
		SoundBuffer = NULL ;

		// サウンドデータをサウンドバッファにコピーする
		{
			for( i = 0 ; i < Sound->ValidBufferNum ; i ++ )
			{
				// 別のサウンドバッファの複製だった場合は転送作業の必要が無いので転送しない
				if( Sound->Normal.BufferDuplicateFlag[ i ] == TRUE ) continue ;
			
				hr = SoundBuffer_Lock( &Sound->Buffer[ i ], 0, ( DWORD )SoundSize,
										&write1, &length1,
										&write2, &length2 ) ;
				if( hr != 0 )
				{
					DXST_LOGFILE_ADDUTF16LE( "\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xc7\x30\xfc\x30\xbf\x30\x6e\x30\xe2\x8e\x01\x90\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x5f\x00\x31\x00\x00"/*@ L"サウンドデータの転送に失敗しました_1" @*/ ) ;
					goto ERR ;
				}

				_MEMCPY( write1, Sound->Normal.WaveData, length1 ) ;
				if( write2 != 0 ) _MEMCPY( write2, (BYTE *)Sound->Normal.WaveData + length1, length2 ) ;

				hr = SoundBuffer_Unlock( &Sound->Buffer[ i ], write1, length1, write2, length2 ) ;
				if( hr != 0 )
				{
					DXST_LOGFILE_ADDUTF16LE( "\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xc7\x30\xfc\x30\xbf\x30\x6e\x30\xe2\x8e\x01\x90\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x5f\x00\x32\x00\x00"/*@ L"サウンドデータの転送に失敗しました_2" @*/ ) ;
					goto ERR ;
				}
			}
		}
	}

	// ループ位置の指定がある場合はループ位置を設定する
	{
		if( GParam->CreateSoundLoopStartTimePosition != GParam->CreateSoundLoopEndTimePosition )
		{
			SetLoopAreaTimePosSoundMem_UseGParam( 
				GParam->CreateSoundLoopStartTimePosition,
				GParam->CreateSoundLoopEndTimePosition,
				SoundHandle,
				ASyncThread
			) ;
		}
		else
		if( GParam->CreateSoundLoopStartSamplePosition != GParam->CreateSoundLoopEndSamplePosition )
		{
			SetLoopAreaSamplePosSoundMem_UseGParam( 
				GParam->CreateSoundLoopStartSamplePosition,
				GParam->CreateSoundLoopEndSamplePosition,
				SoundHandle,
				ASyncThread
			) ;
		}
	}
	
	// 正常終了
	return 0 ;

ERR :
	if( SoundBuffer != NULL )
	{
		DXFREE( SoundBuffer ) ;
	}

	if( Stream.DataPoint != NULL )
	{
		MemStreamClose( Stream.DataPoint ) ;
	}

	if( SSSHandleI >= 0 )
	{
		NS_DeleteSoftSound( SSSHandleI ) ;
		SSSHandleI = -1 ;
	}

	TerminateSoundConvert( &ConvData ) ;
	
	return -1 ;
}

#ifndef DX_NON_ASYNCLOAD
// LoadSoundMemByMemImageBase の非同期読み込みスレッドから呼ばれる関数
static void LoadSoundMemByMemImageBase_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	LOADSOUND_GPARAM *GParam ;
	int CreateSoundHandle ;
	int SoundHandle ;
	const void *FileImageBuffer ;
	int ImageSize ;
	int BufferNum ;
	int UnionHandle ;
	int Addr ;
	int Result ;

	Addr = 0 ;
	GParam = ( LOADSOUND_GPARAM * )GetASyncLoadParamStruct( AParam->Data, &Addr ) ;
	CreateSoundHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	SoundHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	FileImageBuffer = GetASyncLoadParamVoidP( AParam->Data, &Addr ) ;
	ImageSize = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	BufferNum = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	UnionHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;

	Result = LoadSoundMemByMemImageBase_Static( GParam, SoundHandle, FileImageBuffer, ImageSize, BufferNum, UnionHandle, TRUE ) ;
	DecASyncLoadCount( SoundHandle ) ;
	if( Result < 0 )
	{
		if( CreateSoundHandle )
		{
			SubHandle( SoundHandle ) ;
		}
	}
}
#endif // DX_NON_ASYNCLOAD

// LoadSoundMemByMemImageBase のグローバル変数にアクセスしないバージョン
extern int LoadSoundMemByMemImageBase_UseGParam(
	LOADSOUND_GPARAM *GParam,
	int CreateSoundHandle,
	int SoundHandle,
	const void *FileImageBuffer,
	int ImageSize,
	int BufferNum,
	int UnionHandle,
	int ASyncLoadFlag,
	int ASyncThread
)
{
	if( CheckSoundSystem_Initialize_PF() == FALSE )
		return -1 ;

	if( CreateSoundHandle )
	{
		SoundHandle = _CreateSoundHandle( GParam->Create3DSoundFlag, GParam->NotInitSoundMemDelete, ASyncThread ) ;
		if( SoundHandle == -1 )
		{
			goto ERR ;
		}
	}
	else
	{
#ifndef DX_NON_ASYNCLOAD
		if( ASyncThread )
		{
			IncASyncLoadCount( SoundHandle, -1 ) ;
		}
#endif // DX_NON_ASYNCLOAD
	}

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag && ASyncThread == FALSE )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;

		// パラメータに必要なメモリのサイズを算出
		Addr = 0 ;
		AddASyncLoadParamStruct( NULL, &Addr, GParam, sizeof( *GParam ) ) ;
		AddASyncLoadParamInt( NULL, &Addr, CreateSoundHandle ) ;
		AddASyncLoadParamInt( NULL, &Addr, SoundHandle ) ;
		AddASyncLoadParamConstVoidP( NULL, &Addr, FileImageBuffer ) ; 
		AddASyncLoadParamInt( NULL, &Addr, ImageSize ) ;
		AddASyncLoadParamInt( NULL, &Addr, BufferNum ) ;
		AddASyncLoadParamInt( NULL, &Addr, UnionHandle ) ;

		// メモリの確保
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// 処理に必要な情報をセット
		AParam->ProcessFunction = LoadSoundMemByMemImageBase_ASync ;
		Addr = 0 ;
		AddASyncLoadParamStruct( AParam->Data, &Addr, GParam, sizeof( *GParam ) ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, CreateSoundHandle ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, SoundHandle ) ;
		AddASyncLoadParamConstVoidP( AParam->Data, &Addr, FileImageBuffer ) ; 
		AddASyncLoadParamInt( AParam->Data, &Addr, ImageSize ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, BufferNum ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, UnionHandle ) ;

		// データを追加
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto ERR ;
		}

		// 非同期読み込みカウントをインクリメント
		IncASyncLoadCount( SoundHandle, AParam->Index ) ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		if( LoadSoundMemByMemImageBase_Static( GParam, SoundHandle, FileImageBuffer, ImageSize, BufferNum, UnionHandle, ASyncThread ) < 0 )
			goto ERR ;
	}

#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		DecASyncLoadCount( SoundHandle ) ;
	}
#endif // DX_NON_ASYNCLOAD

	// ハンドルを返す
	return CreateSoundHandle ? SoundHandle : 0 ;

ERR :
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		DecASyncLoadCount( SoundHandle ) ;
	}
#endif // DX_NON_ASYNCLOAD

	if( CreateSoundHandle )
	{
		SubHandle( SoundHandle ) ;
		SoundHandle = -1 ;
	}

	// 終了
	return -1 ;
}

// メモリ上に展開されたファイルイメージからハンドルを作成する(ベース関数)
extern int NS_LoadSoundMemByMemImageBase( const void *FileImageBuffer, int ImageSize, int BufferNum, int UnionHandle )
{
	LOADSOUND_GPARAM GParam ;

	InitLoadSoundGParam( &GParam ) ;

	return LoadSoundMemByMemImageBase_UseGParam( &GParam, TRUE, -1, FileImageBuffer, ImageSize, BufferNum, UnionHandle, GetASyncLoadFlag(), FALSE ) ;
}

// LoadSoundMemByMemImageBase の別名関数
extern int NS_LoadSoundMemByMemImage( const void *FileImageBuffer , int ImageSize, int BufferNum, int UnionHandle ) 
{
	return NS_LoadSoundMemByMemImageBase( FileImageBuffer, ImageSize, BufferNum, UnionHandle ) ;
}

// メモリ上に展開されたファイルイメージからハンドルを作成する2
extern int NS_LoadSoundMemByMemImage2( const void *UData, int UDataSize, const WAVEFORMATEX *UFormat, int UHeaderSize )
{
	void *WaveImage ;
	int WaveImageSize ;
	int res, Handle ;
	
	res = CreateWaveFileImage( &WaveImage, &WaveImageSize,
								 UFormat, UHeaderSize,
								 UData, UDataSize ) ;
	if( res < 0 ) return -1 ;
	
	Handle = NS_LoadSoundMemByMemImageBase( WaveImage, WaveImageSize, 1 ) ;
	
	DXFREE( WaveImage ) ;
	
	return Handle ;
}

// メモリ上に展開されたファイルイメージからハンドルを作成する(ベース関数)
extern int NS_LoadSoundMemByMemImageToBufNumSitei( const void *FileImageBuffer, int ImageSize, int BufferNum )
{
	return NS_LoadSoundMemByMemImageBase( FileImageBuffer, ImageSize, BufferNum ) ;
}







// メモリに読み込んだWAVEデータを削除する
extern int NS_DeleteSoundMem( int SoundHandle, int )
{
	return SubHandle( SoundHandle ) ;
}

// サウンドハンドルの再生準備を行う( -1:エラー 0:正常終了 1:再生する必要なし )
static	int _PlaySetupSoundMem( SOUND * Sound, int TopPositionFlag )
{
	ULONGLONG Position ;
	int IsPlay ;
	int i ;
	int j ;
//	D_XAUDIO2_VOICE_STATE XAState ;
	LONG Volume[ SOUNDBUFFER_MAX_CHANNEL_NUM ] ;
	LONG Pan ;
	DWORD Frequency ;

	// 前回の再生位置を取得する
	{
		DWORD Temp ;

//		Sound->Buffer[ Sound->Normal.BackPlayBufferNo ]->GetCurrentPosition( &Position, NULL ) ;
		SoundBuffer_GetCurrentPosition( &Sound->Buffer[ Sound->Normal.BackPlayBufferNo ], &Temp, NULL ) ;
		Position = Temp ;
	}

	// 再生するサウンドバッファのセット
	i = Sound->Normal.BackPlayBufferNo ;

	// 今回再生するサウンドバッファのセット
	i ++ ;
	if( i >= Sound->ValidBufferNum )
		i = 0 ;

	// 前回の再生位置の情報を更新する
	Sound->Normal.BackPlayBufferNo = i ;

	// 再生中だった場合は止める
	{
		IsPlay = SoundBuffer_CheckPlay( &Sound->Buffer[ i ] ) ;
		if( IsPlay == -1 )
		{
			return -1 ;
		}

		if( IsPlay )
		{
			// 先頭からの再生指定ではない場合は何もせず終了
			if( TopPositionFlag == FALSE )
				return 1 ;

			// 再生停止
			SoundBuffer_Stop( &Sound->Buffer[ i ] ) ;
		}

		// 再生位置を先頭にする
		if( TopPositionFlag == TRUE )	SoundBuffer_SetCurrentPosition( &Sound->Buffer[i], 0 ) ;
		else							SoundBuffer_SetCurrentPosition( &Sound->Buffer[i], ( DWORD )Position ) ;
	}

	// ボリュームをセットする
	for( j = 0 ; j < SOUNDBUFFER_MAX_CHANNEL_NUM ; j ++ )
	{
		if( Sound->ValidNextPlayVolume[ j ] )
		{
			Volume[ j ] = ( LONG )Sound->NextPlayVolume[ j ] ;
			Sound->ValidNextPlayVolume[ j ] = 0 ;
		}
		else
		{
			Volume[ j ] = ( LONG )Sound->BaseVolume[ j ] ;
		}
		SoundBuffer_SetVolume( &Sound->Buffer[ i ], j, Volume[ j ] ) ;
	}

	// パンをセットする
	{
		if( Sound->ValidNextPlayPan )
		{
			Pan = ( LONG )Sound->NextPlayPan ;
			Sound->ValidNextPlayPan = 0 ;
		}
		else
		{
			Pan = ( LONG )Sound->BasePan ;
		}
		SoundBuffer_SetPan(       &Sound->Buffer[ i ], Pan ) ;
	}

	// 周波数をセットする
	{
		if( Sound->ValidNextPlayFrequency )
		{
			Frequency = ( DWORD )Sound->NextPlayFrequency ;
			Sound->ValidNextPlayFrequency = 0 ;
		}
		else
		{
			Frequency = ( DWORD )Sound->BaseFrequency ;
		}

		if( Sound->PitchRateEnable )
		{
			Frequency = ( DWORD )_FTOL( Frequency * Sound->PitchRate ) ;
		}

		SoundBuffer_SetFrequency( &Sound->Buffer[ i ], Frequency ) ;
	}

	// ３Ｄサウンドの場合は３Ｄサウンドパラメータの更新を行う
	if( Sound->Is3DSound != FALSE )
	{
		VECTOR *Position3D ;
		float Radius ;
		VECTOR *Velocity ;

		if( Sound->ValidNextPlay3DPosition )
		{
			Position3D = &Sound->NextPlay3DPosition ;
			Sound->ValidNextPlay3DPosition = 0 ;
		}
		else
		{
			Position3D = &Sound->Base3DPosition ;
		}

		if( Sound->ValidNextPlay3DRadius )
		{
			Radius = Sound->NextPlay3DRadius ;
			Sound->ValidNextPlay3DRadius = 0 ;
		}
		else
		{
			Radius = Sound->Base3DRadius ;
		}

		if( Sound->ValidNextPlay3DVelocity )
		{
			Velocity = &Sound->NextPlay3DVelocity ;
			Sound->ValidNextPlay3DVelocity = 0 ;
		}
		else
		{
			Velocity = &Sound->Base3DVelocity ;
		}
		SoundBuffer_Set3DPosition( &Sound->Buffer[ i ], Position3D ) ;
		SoundBuffer_Set3DRadius(   &Sound->Buffer[ i ], Radius ) ;
		SoundBuffer_Set3DVelocity( &Sound->Buffer[ i ], Velocity ) ;

		SoundBuffer_Refresh3DSoundParam( &Sound->Buffer[ i ] ) ;
	}

	// 正常終了
	return 0 ;
}

// メモリに読みこんだWAVEデータを再生する
extern int NS_PlaySoundMem( int SoundHandle , int PlayType, int TopPositionFlag )
{
	SOUND * Sound ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// ソフトが非アクティブの場合はアクティブになるまで待つ
	CheckActiveState() ;

	// エラー判定
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	// サウンドタイプがストリーム風サウンドだった場合はそっちに処理を移す
	if( Sound->Type == DX_SOUNDTYPE_STREAMSTYLE )
	{
		return NS_PlayStreamSoundMem( SoundHandle , PlayType, TopPositionFlag ) ;
	}

	// 再生準備を行う
	if( _PlaySetupSoundMem( Sound, TopPositionFlag ) == 1 )
		return 0 ;

	// 再生
	if( SoundBuffer_Play( &Sound->Buffer[ Sound->Normal.BackPlayBufferNo ], PlayType == DX_PLAYTYPE_LOOP ) != 0 )
	{
		return -1 ;
	}

	// ３Ｄサウンドの場合は再生中の３Ｄサウンドリストに追加する
	if( Sound->Is3DSound != FALSE )
	{
		if( Sound->AddPlay3DSoundList == FALSE )
		{
			Sound->AddPlay3DSoundList = TRUE ;

			// クリティカルセクションの取得
			CRITICALSECTION_LOCK( &SoundSysData.Play3DSoundListCriticalSection ) ;

			AddHandleList( &SoundSysData.Play3DSoundListFirst, &Sound->Play3DSoundList, SoundHandle, Sound ) ;

			// クリティカルセクションの解放
			CriticalSection_Unlock( &SoundSysData.Play3DSoundListCriticalSection ) ;
		}
	}

	// 再生ステータスによっては再生終了を待つ
	if( PlayType == DX_PLAYTYPE_NORMAL )
	{
		while( NS_ProcessMessage() == 0 && NS_CheckSoundMem( SoundHandle ) == 1 ){ Thread_Sleep( 1 ) ; }
	}

	// 終了
	return 0 ;
}


// メモリに読み込んだ複数のWAVEデータをなるべく同時に再生する
extern int NS_MultiPlaySoundMem( const int *SoundHandleList, int SoundHandleNum, int PlayType, int TopPositionFlag )
{
	SOUND **SoundList, *SoundBuf[ 256 ], **SoundTempBuf = NULL, *Sound ;
	SOUNDBUFFER **SBufferList, *SBufferBuf[ 256 ], **SBufferTempBuf = NULL, *SBuffer ;
	int i, Loop ;

	if( SoundSysData.InitializeFlag == FALSE )
	{
		return -1 ;
	}
	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// サウンドポインタリストが足りなかったらメモリを確保する
	if( SoundHandleNum > 256 )
	{
		SoundTempBuf = ( SOUND ** )DXALLOC( ( sizeof( SOUND * ) + sizeof( SOUNDBUFFER * ) ) * SoundHandleNum ) ;
		if( SoundTempBuf == NULL )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x25\x00\x64\x00\x20\x00\x0b\x50\x6e\x30\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xc7\x30\xfc\x30\xbf\x30\x6e\x30\xdd\x30\xa4\x30\xf3\x30\xbf\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"%d 個のサウンドデータのポインタを格納するメモリ領域の確保に失敗しました" @*/ , SoundHandleNum )) ;
			goto END ;
		}
		SBufferTempBuf = ( SOUNDBUFFER ** )( SoundTempBuf + SoundHandleNum ) ;

		SBufferList = SBufferTempBuf ;
		SoundList = SoundTempBuf ;
	}
	else
	{
		SBufferList = SBufferBuf ;
		SoundList = SoundBuf ;
	}

	// ループかどうかを取得しておく
	Loop = PlayType == DX_PLAYTYPE_LOOP ;

	// 再生の準備
	for( i = 0 ; i < SoundHandleNum ; i ++ )
	{
		// エラー判定
		if( SOUNDHCHK( SoundHandleList[ i ], Sound ) )
		{
			SoundList[ i ] = NULL ;
		}
		else
		{
			SoundList[ i ] = Sound ;

			if( Sound->Type == DX_SOUNDTYPE_STREAMSTYLE )
			{
				_PlaySetupStreamSoundMem( SoundHandleList[ i ], Sound, PlayType, TopPositionFlag, &SBufferList[ i ] ) ;
			}
			else
			{
				_PlaySetupSoundMem( Sound, TopPositionFlag ) ;
				SBufferList[ i ] = &Sound->Buffer[ Sound->Normal.BackPlayBufferNo ] ;
			}
		}
	}

	// 一斉に再生
	if( SoundSysData.EnableSoundCaptureFlag )
	{
		for( i = 0 ; i < SoundHandleNum ; i ++ )
		{
			Sound = SoundList[ i ] ;
			if( Sound == NULL ) continue ;
			SBuffer = SBufferList[ i ] ;
			SBuffer->State = TRUE ;
			SBuffer->Loop = ( Loop || Sound->Type == DX_SOUNDTYPE_STREAMSTYLE ) ? 1 : 0 ;
		}
	}
	else
	{
		for( i = 0 ; i < SoundHandleNum ; i ++ )
		{
			Sound = SoundList[ i ] ;
			if( Sound == NULL )
			{
				continue ;
			}

			SBuffer = SBufferList[ i ] ;
			SoundBuffer_Play( SBuffer, Loop || Sound->Type == DX_SOUNDTYPE_STREAMSTYLE ? TRUE : FALSE ) ;
		}
	}

	// 再生ステータスによっては再生終了を待つ
	if( PlayType == DX_PLAYTYPE_NORMAL )
	{
		while( NS_ProcessMessage() == 0 )
		{
			for( i = 0 ; i < SoundHandleNum ; i ++ )
			{
				if( SoundList[ i ] == NULL ) continue ;
				if( NS_CheckSoundMem( SoundHandleList[ i ] ) == 1 ) break ;
			}
			if( i == SoundHandleNum ) break ;
			Thread_Sleep( 1 ) ;
		}
	}

END :

	// メモリの解放
	if( SoundTempBuf )
	{
		DXFREE( SoundTempBuf ) ;
	}

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// 終了
	return 0 ;
}


// メモリに読み込んだWAVEデータの再生を止める
extern int NS_StopSoundMem( int SoundHandle )
{
	SOUND * Sound ;
	int IsPlay ;
	int i ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	// サウンドタイプがストリーム風サウンドだった場合はそっちに処理を移す
	if( Sound->Type == DX_SOUNDTYPE_STREAMSTYLE )
	{
		return NS_StopStreamSoundMem( SoundHandle ) ;
	}

	// 再生停止
	for( i = 0 ; i < Sound->ValidBufferNum ; i ++ )
	{
		Sound->BufferPlayStateBackupFlagValid[ i ] = FALSE ;
		Sound->BufferPlayStateBackupFlag[ i ] = FALSE ;

		IsPlay = SoundBuffer_CheckPlay( &Sound->Buffer[ i ] ) ;
		if( IsPlay == -1 )
		{
			return -1 ;
		}

		if( IsPlay )
		{
			SoundBuffer_Stop( &Sound->Buffer[ i ], TRUE ) ;
		}
	}

	// 終了
	return 0 ;
}

// メモリに読みこんだWAVEデータが再生中か調べる
extern int NS_CheckSoundMem( int SoundHandle )
{
	SOUND * Sound ;
	int i ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	// サウンドタイプがストリーム風サウンドだった場合はそっちに処理を移す
	if( Sound->Type == DX_SOUNDTYPE_STREAMSTYLE )
	{
		return NS_CheckStreamSoundMem( SoundHandle ) ;
	}
	
	// ステータスを取得して調べる
	for( i = 0 ; i < Sound->ValidBufferNum ; i ++ )
	{
		// システム側で止めている場合は止める前の状態を返す
		if( Sound->BufferPlayStateBackupFlagValid[ i ] )
		{
			return Sound->BufferPlayStateBackupFlag[ i ] ? 1 : 0 ;
		}
		else
		{
			if( SoundBuffer_CheckPlay( &Sound->Buffer[ i ] ) )
			{
				return 1 ;
			}
		}
	}

	// ここまできていれば再生はされていない
	return 0 ;
}

// メモリに読みこんだWAVEデータの再生にパンを設定する
extern int NS_SetPanSoundMem( int PanPal , int SoundHandle )
{
	int i ;
	SOUND * Sound ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	// 再生がされていない場合は SetNextPlayPanSoundMem を呼ぶ
//	if( NS_CheckSoundMem( SoundHandle ) == 0 )
//	{
//		return NS_SetNextPlayPanSoundMem( PanPal, SoundHandle ) ;
//	}

	if( PanPal >  10000 ) PanPal =  10000 ;
	if( PanPal < -10000 ) PanPal = -10000 ;

	// パンをセットする
	for( i = 0 ; i < Sound->ValidBufferNum ; i ++ )
	{
//		Sound->Buffer[ i ]->SetPan( ( LONG )PanPal ) ;
		SoundBuffer_SetPan( &Sound->Buffer[ i ], ( LONG )PanPal ) ;
	}
//	SoundBuffer_SetPan( &Sound->Buffer[ Sound->Normal.BackPlayBufferNo ], ( LONG )PanPal ) ;
	Sound->BasePan = PanPal ;

	// 終了
	return 0 ;
}

// サウンドハンドルのパンを設定する( 0 〜 255 )
extern int NS_ChangePanSoundMem( int PanPal, int SoundHandle )
{
	int i ;
	int pan ;
	SOUND * Sound ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	if( PanPal >  255 ) PanPal =  255 ;
	if( PanPal < -255 ) PanPal = -255 ;

	if( PanPal == 0 )
	{
		pan = 0 ;
	}
	else
	{
		if( PanPal > 0 )
		{
			if( PanPal == 255 )
			{
				pan = 10000 ;
			}
			else
			{
				if( SoundSysData.OldVolumeTypeFlag )
				{
					pan = -(LONG)( _DTOL( _LOG10( ( 255 - PanPal ) / 255.0 ) * 10.0 * 100.0 ) ) ;
				}
				else
				{
					pan = -(LONG)( _DTOL( _LOG10( ( 255 - PanPal ) / 255.0 ) * 50.0 * 100.0 ) ) ;
				}
				if( pan > 10000 )
				{
					pan = 10000 ;
				}
			}
		}
		else
		{
			if( PanPal == -255 )
			{
				pan = -10000 ;
			}
			else
			{
				if( SoundSysData.OldVolumeTypeFlag )
				{
					pan = (LONG)( _DTOL( _LOG10( ( 255 + PanPal ) / 255.0 ) * 10.0 * 100.0 ) ) ;
				}
				else
				{
					pan = (LONG)( _DTOL( _LOG10( ( 255 + PanPal ) / 255.0 ) * 50.0 * 100.0 ) ) ;
				}
				if( pan < -10000 )
				{
					pan = -10000 ;
				}
			}
		}
	}

	// パンをセットする
	for( i = 0 ; i < Sound->ValidBufferNum ; i ++ )
	{
		SoundBuffer_SetPan( &Sound->Buffer[ i ], ( LONG )pan ) ;
	}

	Sound->BasePan = pan ;

	// 終了
	return 0 ;
}

// メモリに読みこんだWAVEデータのパンを取得する
extern int NS_GetPanSoundMem( int SoundHandle )
{
	SOUND * Sound ;
	LONG Result ;
	SOUNDBUFFER *SBuffer ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	// サウンドタイプがストリーム風サウンドだった場合はそっちに処理を移す
	if( Sound->Type == DX_SOUNDTYPE_STREAMSTYLE )
	{
		// 使用権が無い場合はエラー
		if( GetSoundBuffer( SoundHandle, Sound, &SBuffer, false ) == -1 ) return -1 ;
	
		// パンを取得する
		SoundBuffer_GetPan( SBuffer, &Result ) ;
	}
	else
	{
		SoundBuffer_GetPan( &Sound->Buffer[ Sound->Normal.BackPlayBufferNo ], &Result ) ;
	}

	// 終了
	return Result ; 
}

// メモリに読みこんだWAVEデータの再生にボリュームを設定する
extern int NS_SetVolumeSoundMem( int VolumePal , int SoundHandle )
{
	SOUND * Sound ;
	int i ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	// 再生がされていない場合は SetNextPlayVolumeSoundMem を呼ぶ
//	if( NS_CheckSoundMem( SoundHandle ) == 0 )
//	{
//		return NS_SetNextPlayVolumeSoundMem( VolumePal, SoundHandle ) ;
//	}

	VolumePal -= 10000 ;
//	if( VolumePal >  0 ) VolumePal =  0 ;
	if( VolumePal < -10000 ) VolumePal = -10000 ;

	// ボリュームをセットする
	for( i = 0 ; i < Sound->ValidBufferNum ; i ++ )
	{
//		Sound->Buffer[ i ]->SetVolume( ( LONG )VolumePal ) ;
		SoundBuffer_SetVolumeAll( &Sound->Buffer[ i ], ( LONG )VolumePal ) ;
	}
//	SoundBuffer_SetVolumeAll( &Sound->Buffer[ Sound->Normal.BackPlayBufferNo ], ( LONG )VolumePal ) ;

	int Vol8bit ;
	const double Min = 0.0000000001 ;
	const double Max = 1.0 ;
			
	if( SoundSysData.OldVolumeTypeFlag )
	{
		Vol8bit = _DTOL( ( _POW( (float)10, VolumePal / 10.0f / 100.0f ) / ( Max - Min ) ) * 256 ) ;
	}
	else
	{
		Vol8bit = _DTOL( ( _POW( (float)10, VolumePal / 50.0f / 100.0f ) / ( Max - Min ) ) * 256 ) ;
	}

	for( i = 0 ; i < SOUNDBUFFER_MAX_CHANNEL_NUM ; i ++ )
	{
		Sound->BaseVolume[ i ] = VolumePal ;
		Sound->BaseVolume8bit[ i ] = Vol8bit ;
	}

	// 終了
	return 0 ;
}

// メモリに読みこんだWAVEデータの再生にボリュームを設定する
extern int NS_ChangeVolumeSoundMem( int VolumePal, int SoundHandle )
{
	SOUND * Sound ;
	int i ;
	LONG vol ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

//	     if( VolumePal > 255 ) VolumePal = 255 ;
//	else if( VolumePal < 0   ) VolumePal = 0   ;
	if( VolumePal < 0   ) VolumePal = 0   ;

	if( VolumePal == 0 )
	{
		vol = DX_DSBVOLUME_MIN ;
	}
	else
	{
		if( SoundSysData.OldVolumeTypeFlag )
		{
			vol = (LONG)( _DTOL( _LOG10( VolumePal / 255.0 ) * 10.0 * 100.0 ) ) ;
		}
		else
		{
			vol = (LONG)( _DTOL( _LOG10( VolumePal / 255.0 ) * 50.0 * 100.0 ) ) ;
		}
		if( vol < DX_DSBVOLUME_MIN ) vol = DX_DSBVOLUME_MIN ;
	}

	// ボリュームをセットする
	for( i = 0 ; i < Sound->ValidBufferNum ; i ++ )
	{
//		Sound->Buffer[ i ]->SetVolume( vol ) ;
		SoundBuffer_SetVolumeAll( &Sound->Buffer[ i ], vol ) ;
	}
//	SoundBuffer_SetVolumeAll( &Sound->Buffer[ Sound->Normal.BackPlayBufferNo ], vol ) ;

	for( i = 0 ; i < SOUNDBUFFER_MAX_CHANNEL_NUM ; i ++ )
	{
		Sound->BaseVolume[ i ] = vol ;
		Sound->BaseVolume8bit[ i ] = VolumePal ;
	}

	// 終了
	return 0 ;
}

// メモリに読みこんだWAVEデータの再生のボリュームを取得する( 100分の1デシベル単位 0 〜 10000 )
extern int NS_GetVolumeSoundMem( int SoundHandle )
{
	return NS_GetChannelVolumeSoundMem( 0, SoundHandle ) ;
}


// メモリに読みこんだWAVEデータの再生のボリュームを取得する( 0 〜 255 )
extern int NS_GetVolumeSoundMem2( int SoundHandle )
{
	return NS_GetChannelVolumeSoundMem2( 0, SoundHandle ) ;
}

// サウンドハンドルの指定のチャンネルのボリュームを設定する( 100分の1デシベル単位 0 〜 10000 )
extern int NS_SetChannelVolumeSoundMem( int Channel, int VolumePal, int SoundHandle )
{
	SOUND * Sound ;
	int i ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	VolumePal -= 10000 ;
//	if( VolumePal >  0 ) VolumePal =  0 ;
	if( VolumePal < -10000 ) VolumePal = -10000 ;

	int Vol8bit ;
	const double Min = 0.0000000001 ;
	const double Max = 1.0 ;
			
	if( SoundSysData.OldVolumeTypeFlag )
	{
		Vol8bit = _DTOL( ( _POW( (float)10, VolumePal / 10.0f / 100.0f ) / ( Max - Min ) ) * 256 ) ;
	}
	else
	{
		Vol8bit = _DTOL( ( _POW( (float)10, VolumePal / 50.0f / 100.0f ) / ( Max - Min ) ) * 256 ) ;
	}

	// ボリュームをセットする
	for( i = 0 ; i < Sound->ValidBufferNum ; i ++ )
	{
		if( SoundBuffer_SetVolume( &Sound->Buffer[ i ], Channel, ( LONG )VolumePal ) < 0 )
		{
			return -1 ;
		}
	}
	Sound->BaseVolume[ Channel ] = VolumePal ;
	Sound->BaseVolume8bit[ Channel ] = Vol8bit ;

	// 終了
	return 0 ;
}

// サウンドハンドルの指定のチャンネルのボリュームを設定する( 0 〜 255 )
extern int NS_ChangeChannelVolumeSoundMem( int Channel, int VolumePal, int SoundHandle )
{
	SOUND * Sound ;
	int i ;
	LONG vol ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

//	     if( VolumePal > 255 ) VolumePal = 255 ;
//	else if( VolumePal < 0   ) VolumePal = 0   ;
	if( VolumePal < 0   ) VolumePal = 0   ;

	if( VolumePal == 0 )
	{
		vol = DX_DSBVOLUME_MIN ;
	}
	else
	{
		if( SoundSysData.OldVolumeTypeFlag )
		{
			vol = (LONG)( _DTOL( _LOG10( VolumePal / 255.0 ) * 10.0 * 100.0 ) ) ;
		}
		else
		{
			vol = (LONG)( _DTOL( _LOG10( VolumePal / 255.0 ) * 50.0 * 100.0 ) ) ;
		}
		if( vol < DX_DSBVOLUME_MIN ) vol = DX_DSBVOLUME_MIN ;
	}

	// ボリュームをセットする
	for( i = 0 ; i < Sound->ValidBufferNum ; i ++ )
	{
		if( SoundBuffer_SetVolume( &Sound->Buffer[ i ], Channel, vol ) < 0 )
		{
			return -1 ;
		}
	}

	Sound->BaseVolume[ Channel ] = vol ;
	Sound->BaseVolume8bit[ Channel ] = VolumePal ;

	// 終了
	return 0 ;
}

// サウンドハンドルの指定のチャンネルのボリュームを取得する( 100分の1デシベル単位 0 〜 10000 )
extern int NS_GetChannelVolumeSoundMem( int Channel, int SoundHandle )
{
	SOUND * Sound ;
	LONG Result ;
	SOUNDBUFFER *SBuffer ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	// サウンドタイプがストリーム風サウンドだった場合はそっちに処理を移す
	if( Sound->Type == DX_SOUNDTYPE_STREAMSTYLE )
	{
		// 使用権が無い場合はエラー
		if( GetSoundBuffer( SoundHandle, Sound, &SBuffer, false ) == -1 ) return -1 ;
	
		// ボリュームを取得する
		if( SoundBuffer_GetVolume( SBuffer, Channel, &Result ) < 0 )
		{
			return -1 ;
		}
	}
	else
	{
		if( SoundBuffer_GetVolume( &Sound->Buffer[ Sound->Normal.BackPlayBufferNo ], Channel, &Result ) < 0 )
		{
			return -1 ;
		}
	}

	// 終了
	return Result + 10000 ; 
}

// サウンドハンドルの指定のチャンネルのボリュームを取得する( 0 〜 255 )
extern int NS_GetChannelVolumeSoundMem2( int Channel, int SoundHandle )
{
	SOUND * Sound ;
	LONG Result ;
	SOUNDBUFFER *SBuffer ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	// サウンドタイプがストリーム風サウンドだった場合はそっちに処理を移す
	if( Sound->Type == DX_SOUNDTYPE_STREAMSTYLE )
	{
		// 使用権が無い場合はエラー
		if( GetSoundBuffer( SoundHandle, Sound, &SBuffer, false ) == -1 ) return -1 ;
	
		// ボリュームを取得する
		if( SoundBuffer_GetVolume( SBuffer, Channel, &Result ) < 0 )
		{
			return -1 ;
		}
	}
	else
	{
		if( SoundBuffer_GetVolume( &Sound->Buffer[ Sound->Normal.BackPlayBufferNo ], Channel, &Result ) < 0 )
		{
			return -1 ;
		}
	}

	int Vol8bit ;
	const double Min = 0.0000000001 ;
	const double Max = 1.0 ;
			
	if( SoundSysData.OldVolumeTypeFlag )
	{
		Vol8bit = _DTOL( ( _POW( (float)10, Result / 10.0f / 100.0f ) / ( Max - Min ) ) * 256 ) ;
	}
	else
	{
		Vol8bit = _DTOL( ( _POW( (float)10, Result / 50.0f / 100.0f ) / ( Max - Min ) ) * 256 ) ;
	}

	// 終了
	return Vol8bit ; 
}

// メモリに読み込んだWAVEデータの再生周波数を設定する
extern int NS_SetFrequencySoundMem( int FrequencyPal , int SoundHandle )
{
	SOUND * Sound ;
	int TempFrequency ;
	int i ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	if( FrequencyPal == -1 )
	{
		FrequencyPal = 0 ;
	}

	// 周波数をセットする
	TempFrequency = FrequencyPal ;
	if( Sound->PitchRateEnable )
	{
		TempFrequency = _FTOL( TempFrequency * Sound->PitchRate ) ;
	}
	for( i = 0 ; i < Sound->ValidBufferNum ; i ++ )
	{
		SoundBuffer_SetFrequency( &Sound->Buffer[ i ], ( DWORD )TempFrequency ) ;
	}

	Sound->BaseFrequency = FrequencyPal ;

	// 終了
	return 0 ;
}

// メモリに読み込んだWAVEデータの再生周波数を取得する
extern int NS_GetFrequencySoundMem( int SoundHandle )
{
	SOUND * Sound ;
	SOUNDBUFFER *SBuffer ;
	DWORD Result ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	// 周波数を返す
//	return (int)Sound->BufferFormat.nSamplesPerSec ;

	// サウンドタイプがストリーム風サウンドだった場合はそっちに処理を移す
	if( Sound->Type == DX_SOUNDTYPE_STREAMSTYLE )
	{
		// 使用権が無い場合はエラー
		if( GetSoundBuffer( SoundHandle, Sound, &SBuffer, false ) == -1 ) return -1 ;
	
		// 再生周波数を取得する
		SoundBuffer_GetFrequency( SBuffer, &Result ) ;
	}
	else
	{
		SoundBuffer_GetFrequency( &Sound->Buffer[ Sound->Normal.BackPlayBufferNo ], &Result ) ;
	}

	// 終了
	return ( int )Result ; 
}

// メモリに読み込んだWAVEデータの再生周波数を読み込み直後の状態に戻す
extern	int NS_ResetFrequencySoundMem( int SoundHandle )
{
	SOUND * Sound ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	// 周波数を初期状態に戻す
	return NS_SetFrequencySoundMem( (int)Sound->BufferFormat.nSamplesPerSec, SoundHandle ) ;
}


// メモリに読みこんだWAVEデータの再生にパンを設定する
extern	int	NS_SetNextPlayPanSoundMem( int PanPal, int SoundHandle )
{
	SOUND * Sound ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	if( PanPal >  10000 ) PanPal =  10000 ;
	if( PanPal < -10000 ) PanPal = -10000 ;

	Sound->NextPlayPan = PanPal ;
	Sound->ValidNextPlayPan = 1 ;

	// 終了
	return 0 ;
}

// サウンドハンドルの次の再生にのみ使用するパンを設定する( -255 〜 255 )
extern int NS_ChangeNextPlayPanSoundMem( int PanPal, int SoundHandle )
{
	SOUND * Sound ;
	int pan ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	if( PanPal >  255 ) PanPal =  255 ;
	if( PanPal < -255 ) PanPal = -255 ;

	if( PanPal == 0 )
	{
		pan = 0 ;
	}
	else
	{
		if( PanPal > 0 )
		{
			if( PanPal == 255 )
			{
				pan = 10000 ;
			}
			else
			{
				if( SoundSysData.OldVolumeTypeFlag )
				{
					pan = -(LONG)( _DTOL( _LOG10( ( 255 - PanPal ) / 255.0 ) * 10.0 * 100.0 ) ) ;
				}
				else
				{
					pan = -(LONG)( _DTOL( _LOG10( ( 255 - PanPal ) / 255.0 ) * 50.0 * 100.0 ) ) ;
				}
				if( pan > 10000 )
				{
					pan = 10000 ;
				}
			}
		}
		else
		{
			if( PanPal == -255 )
			{
				pan = -10000 ;
			}
			else
			{
				if( SoundSysData.OldVolumeTypeFlag )
				{
					pan = (LONG)( _DTOL( _LOG10( ( 255 + PanPal ) / 255.0 ) * 10.0 * 100.0 ) ) ;
				}
				else
				{
					pan = (LONG)( _DTOL( _LOG10( ( 255 + PanPal ) / 255.0 ) * 50.0 * 100.0 ) ) ;
				}
				if( pan < -10000 )
				{
					pan = -10000 ;
				}
			}
		}
	}

	Sound->NextPlayPan = pan ;
	Sound->ValidNextPlayPan = 1 ;

	// 終了
	return 0 ;
}

// メモリに読みこんだWAVEデータの再生にボリュームを設定する( 100分の1デシベル単位 )
extern	int	NS_SetNextPlayVolumeSoundMem( int VolumePal, int SoundHandle )
{
	SOUND * Sound ;
	int i ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	VolumePal -= 10000 ;
	if( VolumePal >  0 ) VolumePal =  0 ;
	if( VolumePal < -10000 ) VolumePal = -10000 ;

	for( i = 0 ; i < SOUNDBUFFER_MAX_CHANNEL_NUM ; i ++ )
	{
		Sound->NextPlayVolume[ i ] = VolumePal ;
		Sound->ValidNextPlayVolume[ i ] = 1 ;
	}

	// 終了
	return 0 ;
}

// メモリに読みこんだWAVEデータの再生にボリュームを設定する( パーセント指定 )
extern	int	NS_ChangeNextPlayVolumeSoundMem( int VolumePal, int SoundHandle )
{
	SOUND * Sound ;
	LONG vol ;
	int i ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	     if( VolumePal > 255 ) VolumePal = 255 ;
	else if( VolumePal < 0   ) VolumePal = 0   ;

	if( VolumePal == 0 )
	{
		vol = DX_DSBVOLUME_MIN ;
	}
	else
	{
		if( SoundSysData.OldVolumeTypeFlag )
		{
			vol = (LONG)( _DTOL( _LOG10( VolumePal / 255.0 ) * 10.0 * 100.0 ) ) ;
		}
		else
		{
			vol = (LONG)( _DTOL( _LOG10( VolumePal / 255.0 ) * 50.0 * 100.0 ) ) ;
		}
		if( vol < DX_DSBVOLUME_MIN ) vol = DX_DSBVOLUME_MIN ;
	}

	for( i = 0 ; i < SOUNDBUFFER_MAX_CHANNEL_NUM ; i ++ )
	{
		Sound->NextPlayVolume[ i ] = vol ;
		Sound->ValidNextPlayVolume[ i ] = 1 ;
	}

	// 終了
	return 0 ;
}

// サウンドハンドルの次の再生にのみ使用するチャンネルのボリュームを設定する( 100分の1デシベル単位 0 〜 10000 )
extern int NS_SetNextPlayChannelVolumeSoundMem( int Channel, int VolumePal, int SoundHandle )
{
	SOUND * Sound ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	VolumePal -= 10000 ;
	if( VolumePal >  0 ) VolumePal =  0 ;
	if( VolumePal < -10000 ) VolumePal = -10000 ;

	Sound->NextPlayVolume[ Channel ] = VolumePal ;
	Sound->ValidNextPlayVolume[ Channel ] = 1 ;

	// 終了
	return 0 ;
}

// サウンドハンドルの次の再生にのみ使用するチャンネルのボリュームを設定する( 0 〜 255 )
extern int NS_ChangeNextPlayChannelVolumeSoundMem( int Channel, int VolumePal, int SoundHandle )
{
	SOUND * Sound ;
	LONG vol ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	     if( VolumePal > 255 ) VolumePal = 255 ;
	else if( VolumePal < 0   ) VolumePal = 0   ;

	if( VolumePal == 0 )
	{
		vol = DX_DSBVOLUME_MIN ;
	}
	else
	{
		if( SoundSysData.OldVolumeTypeFlag )
		{
			vol = (LONG)( _DTOL( _LOG10( VolumePal / 255.0 ) * 10.0 * 100.0 ) ) ;
		}
		else
		{
			vol = (LONG)( _DTOL( _LOG10( VolumePal / 255.0 ) * 50.0 * 100.0 ) ) ;
		}
		if( vol < DX_DSBVOLUME_MIN ) vol = DX_DSBVOLUME_MIN ;
	}

	Sound->NextPlayVolume[ Channel ] = vol ;
	Sound->ValidNextPlayVolume[ Channel ] = 1 ;

	// 終了
	return 0 ;
}

// メモリに読み込んだWAVEデータの再生周波数を設定する
extern	int	NS_SetNextPlayFrequencySoundMem( int FrequencyPal, int SoundHandle )
{
	SOUND * Sound ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	if( FrequencyPal == -1 )
	{
		FrequencyPal = 0 ;
	}

	Sound->NextPlayFrequency = FrequencyPal ;
	Sound->ValidNextPlayFrequency = 1 ;

	// 終了
	return 0 ;
}


// サウンドハンドルの再生位置をサンプル単位で変更する(再生が止まっている時のみ有効)
extern int NS_SetCurrentPositionSoundMem( int SamplePosition, int SoundHandle )
{
	SOUND * sd ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// 変更
	return NS_SetSoundCurrentPosition( sd->BufferFormat.nBlockAlign * SamplePosition, SoundHandle ) ;
}

// サウンドハンドルの再生位置をサンプル単位で取得する
extern int NS_GetCurrentPositionSoundMem( int SoundHandle )
{
	SOUND * sd ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// 取得
	return NS_GetSoundCurrentPosition( SoundHandle ) / sd->BufferFormat.nBlockAlign ;
}

// サウンドハンドルの再生位置をバイト単位で変更する(再生が止まっている時のみ有効)
extern int NS_SetSoundCurrentPosition( int Byte, int SoundHandle )
{
	SOUND * sd ;
//	int i ;
//	DWORD State ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// サウンドタイプがストリーム風サウンドだった場合はそっちに処理を移す
	if( sd->Type == DX_SOUNDTYPE_STREAMSTYLE )
	{
		return NS_SetStreamSoundCurrentPosition( Byte, SoundHandle ) ;
	}

	// 再生位置を変更する
//	sd->Buffer[ i ]->SetCurrentPosition( Byte ) ;
//	SoundBuffer_SetCurrentPosition( &sd->Buffer[ i ], Byte ) ;
	SoundBuffer_SetCurrentPosition( &sd->Buffer[ sd->Normal.BackPlayBufferNo ], ( DWORD )Byte ) ;

	// 終了
	return 0 ;
}

// サウンドハンドルの再生位置をバイト単位で取得する
extern int NS_GetSoundCurrentPosition( int SoundHandle )
{
	SOUND * sd ;
	DWORD pos ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// サウンドタイプがストリーム風サウンドだった場合はそっちに処理を移す
	if( sd->Type == DX_SOUNDTYPE_STREAMSTYLE )
	{
		return NS_GetStreamSoundCurrentPosition( SoundHandle ) ;
	}

	// 再生中のバッファの再生位置を返す
//	sd->Buffer[ Sound->Normal.BackPlayBufferNo ]->GetCurrentPosition( &pos, NULL ) ;
	SoundBuffer_GetCurrentPosition( &sd->Buffer[ sd->Normal.BackPlayBufferNo ], &pos, NULL ) ;

	return (int)pos ;
}

// サウンドハンドルの再生位置をミリ秒単位で設定する(圧縮形式の場合は正しく設定されない場合がある)
extern int NS_SetSoundCurrentTime( int Time, int SoundHandle )
{
	SOUND * sd ;
	int /*i,*/ time ;
//	DWORD State ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// サウンドタイプがストリーム風サウンドだった場合はそっちに処理を移す
	if( sd->Type == DX_SOUNDTYPE_STREAMSTYLE )
	{
		return NS_SetStreamSoundCurrentTime( Time, SoundHandle ) ;
	}

	// 再生位置を変更する
//	time = _DTOL( ( (double)sd->BufferFormat.nSamplesPerSec * Time / 1000 ) * sd->BufferFormat.nBlockAlign ) ;
//	for( i = 0 ; i < sd->ValidBufferNum ; i ++ )
	time = MilliSecPositionToSamplePosition( ( int )sd->BufferFormat.nSamplesPerSec, Time ) * sd->BufferFormat.nBlockAlign ;

//	sd->Buffer[ i ]->SetCurrentPosition( time ) ;
//	SoundBuffer_SetCurrentPosition( &sd->Buffer[ i ], time ) ;
	SoundBuffer_SetCurrentPosition( &sd->Buffer[ sd->Normal.BackPlayBufferNo ], ( DWORD )time ) ;

	// 終了
	return 0 ;
}

// GetSoundTotalSample のグローバル変数にアクセスしないバージョン
extern int GetSoundTotalSample_UseGParam( int SoundHandle, int ASyncThread )
{
	SOUND * sd ;
	int sample = -1 ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( ASyncThread )
	{
		if( SOUNDHCHK_ASYNC( SoundHandle, sd ) )
			return -1 ;
	}
	else
	{
		if( SOUNDHCHK( SoundHandle, sd ) )
			return -1 ;
	}

	// サウンドタイプによって処理を分岐
	switch( sd->Type )
	{
	case DX_SOUNDTYPE_STREAMSTYLE : // ストリームタイプの場合
		sample = sd->Stream.TotalSample ;
		break ;
		
	case DX_SOUNDTYPE_NORMAL :		// ノーマルタイプの場合
		sample = sd->Normal.WaveSize / sd->BufferFormat.nBlockAlign ;
		break ;
	}
	
	// 終了
	return sample ;
}

// サウンドハンドルの音の総時間を取得する(単位はサンプル)
extern int NS_GetSoundTotalSample( int SoundHandle )
{
	return GetSoundTotalSample_UseGParam( SoundHandle, FALSE ) ;
}

// サウンドハンドルの音の総時間を取得する(単位はミリ秒)
extern int NS_GetSoundTotalTime( int SoundHandle )
{
	SOUND * sd ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;
	
	// ミリ秒に変換して返す
	return SamplePositionToMilliSecPosition( ( int )sd->BufferFormat.nSamplesPerSec, NS_GetSoundTotalSample( SoundHandle ) ) ;
}

// サウンドハンドルの再生位置をミリ秒単位で取得する(圧縮形式の場合は正しい値が返ってこない場合がある)
extern int NS_GetSoundCurrentTime( int SoundHandle )
{
	SOUND * sd ;
	DWORD time ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// サウンドタイプがストリーム風サウンドだった場合はそっちに処理を移す
	if( sd->Type == DX_SOUNDTYPE_STREAMSTYLE )
	{
		return NS_GetStreamSoundCurrentTime( SoundHandle ) ;
	}

	// サウンドバッファ０の再生位置を取得する
//	sd->Buffer[ 0 ]->GetCurrentPosition( &time, NULL ) ;
	SoundBuffer_GetCurrentPosition( &sd->Buffer[ sd->Normal.BackPlayBufferNo ], &time, NULL ) ;
//	return _DTOL( (double)time * 1000 / ( sd->BufferFormat.nSamplesPerSec * sd->BufferFormat.nBlockAlign ) ) ;
	return SamplePositionToMilliSecPosition( ( int )sd->BufferFormat.nSamplesPerSec, ( int )( time / sd->BufferFormat.nBlockAlign ) ) ;
}


// サウンドハンドルにループ位置を設定する
extern int NS_SetLoopPosSoundMem( int LoopTime, int SoundHandle )
{
	SOUND * sd ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// ループサンプル位置をセット
	NS_SetLoopSamplePosSoundMem( LoopTime == -1 ? -1 : MilliSecPositionToSamplePosition( ( int )sd->BufferFormat.nSamplesPerSec, LoopTime ), SoundHandle ) ;

	// 終了
	return 0 ;
}

// SetLoopTimePosSoundMem のグローバル変数にアクセスしないバージョン
extern int SetLoopTimePosSoundMem_UseGParam( int LoopTime, int SoundHandle, int ASyncThread )
{
	SOUND * sd ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( ASyncThread )
	{
		if( SOUNDHCHK_ASYNC( SoundHandle, sd ) )
			return -1 ;
	}
	else
	{
		if( SOUNDHCHK( SoundHandle, sd ) )
			return -1 ;
	}
	
	// ループサンプル位置をセット
	SetLoopSamplePosSoundMem_UseGParam(
		LoopTime == -1 ? -1 : MilliSecPositionToSamplePosition( ( int )sd->BufferFormat.nSamplesPerSec, LoopTime ),
		SoundHandle, ASyncThread
	) ;

	// 終了
	return 0 ;
}

// サウンドハンドルにループ位置を設定する
extern int NS_SetLoopTimePosSoundMem( int LoopTime, int SoundHandle )
{
	return SetLoopTimePosSoundMem_UseGParam( LoopTime, SoundHandle, FALSE ) ;
}

// SetLoopSamplePosSoundMem のグローバル変数にアクセスしないバージョン
extern int SetLoopSamplePosSoundMem_UseGParam( int LoopSamplePosition, int SoundHandle, int ASyncThread )
{
	SOUND * sd ;
	STREAMFILEDATA * pl ;

	if( SoundSysData.InitializeFlag == FALSE )
	{
		return -1 ;
	}
	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// エラー判定
	if( ASyncThread )
	{
		if( SOUNDHCHK_ASYNC( SoundHandle, sd ) )
			goto ERR ;
	}
	else
	{
		if( SOUNDHCHK( SoundHandle, sd ) )
			goto ERR ;
	}

	// 指定の位置が音声ファイル全体よりも長い場合はエラー
	if( GetSoundTotalSample_UseGParam( SoundHandle, ASyncThread ) <= LoopSamplePosition )
		goto ERR ;

	// ループ位置が -1 だった場合はループ位置設定を解除する
	if( LoopSamplePosition == -1 )
	{
		pl = &sd->Stream.File[0] ;
		pl->LoopSamplePosition = 0 ;
		pl->LoopNum = 0 ;
		goto END ;
	}

	// サウンドハンドルがストリーム再生ではなかったらストリーム再生のハンドルに変更する
	if( sd->Type == DX_SOUNDTYPE_NORMAL )
	{
		// 非同期スレッドの場合はエラー
		if( ASyncThread )
			goto ERR ;

		if( SoundTypeChangeToStream( SoundHandle ) < 0 )
			goto ERR ;

		// エラー判定
		if( SOUNDHCHK( SoundHandle, sd ) )
			goto ERR ;
	}

	// 今の所ストリームデータが２つ以上あったら無理
	if( sd->Stream.FileNum > 1 )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x8c\x4e\x64\x30\xe5\x4e\x0a\x4e\x6e\x30\xf3\x97\xf0\x58\xd5\x30\xa1\x30\xa4\x30\xeb\x30\x4b\x30\x89\x30\x5c\x4f\x10\x62\x55\x30\x8c\x30\x5f\x30\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x6b\x30\xeb\x30\xfc\x30\xd7\x30\xdd\x30\xa4\x30\xf3\x30\xc8\x30\x92\x30\x07\x63\x9a\x5b\x59\x30\x8b\x30\x8b\x4e\x6f\x30\xfa\x51\x65\x67\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"二つ以上の音声ファイルから作成されたサウンドハンドルにループポイントを指定する事は出来ません\n" @*/ ) ;
		goto ERR ;
	}

	pl = &sd->Stream.File[0] ;

/*	// 無圧縮 wave か OGG 以外は無理
	if( pl->DataType != DX_SOUNDDATATYPE_MEMNOPRESS &&
#ifndef DX_NON_OGGVORBIS
		pl->OggFlag == FALSE && 
#endif
		( pl->DataType == DX_SOUNDDATATYPE_MEMPRESS ||
			( pl->DataType == DX_SOUNDDATATYPE_FILE && pl->FileData.FormatMatchFlag == FALSE ) ) )
	{
		DXST_LOGFILE_ADDUTF16LE( L"無圧縮ＷＡＶＥファイル若しくはＯＧＧファイル以外はループポイントを指定する事は出来ません\n" ) ;

		
		
		return -1 ;
	}
*/
	// ループ位置を設定
	pl->LoopSamplePosition = LoopSamplePosition ;
	SetupSeekPosSoundConvert( &pl->ConvData, LoopSamplePosition ) ;

	// 無限ループに設定
	pl->LoopNum = -1 ;

END :

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// 終了
	return 0 ;

ERR :

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// エラー終了
	return -1 ;
}

// サウンドハンドルにループ位置を設定する
extern int NS_SetLoopSamplePosSoundMem( int LoopSamplePosition, int SoundHandle )
{
	return SetLoopSamplePosSoundMem_UseGParam( LoopSamplePosition, SoundHandle, FALSE ) ;
}

// SetLoopStartTimePosSoundMem のグローバル変数にアクセスしないバージョン
extern int SetLoopStartTimePosSoundMem_UseGParam( int LoopStartTime, int SoundHandle, int ASyncThread )
{
	SOUND * sd ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( ASyncThread )
	{
		if( SOUNDHCHK_ASYNC( SoundHandle, sd ) )
			return -1 ;
	}
	else
	{
		if( SOUNDHCHK( SoundHandle, sd ) )
			return -1 ;
	}

	// ループサンプル位置をセット
	SetLoopStartSamplePosSoundMem_UseGParam(
		LoopStartTime == -1 ? -1 : MilliSecPositionToSamplePosition( ( int )sd->BufferFormat.nSamplesPerSec, LoopStartTime ),
		SoundHandle, ASyncThread
	) ;

	// 終了
	return 0 ;
}

// サウンドハンドルにループ開始位置を設定する
extern int NS_SetLoopStartTimePosSoundMem( int LoopStartTime, int SoundHandle )
{
	return SetLoopStartTimePosSoundMem_UseGParam( LoopStartTime, SoundHandle, FALSE ) ;
}

// SetLoopStartTimePosSoundMem のグローバル変数にアクセスしないバージョン
extern int SetLoopStartSamplePosSoundMem_UseGParam( int LoopStartSamplePosition, int SoundHandle, int ASyncThread )
{
	SOUND * sd ;
	STREAMFILEDATA * pl ;

	if( SoundSysData.InitializeFlag == FALSE )
	{
		return -1 ;
	}
	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// エラー判定
	if( ASyncThread )
	{
		if( SOUNDHCHK_ASYNC( SoundHandle, sd ) )
			goto ERR ;
	}
	else
	{
		if( SOUNDHCHK( SoundHandle, sd ) )
			goto ERR ;
	}

	// 指定の位置が音声ファイル全体よりも長い場合はエラー
	if( GetSoundTotalSample_UseGParam( SoundHandle, ASyncThread ) <= LoopStartSamplePosition )
		goto ERR ;

	// ループ位置が -1 だった場合はループ位置設定を解除する
	if( LoopStartSamplePosition == -1 )
	{
		pl = &sd->Stream.File[0] ;
		pl->LoopStartSamplePosition = -1 ;
		goto END ;
	}

	// サウンドハンドルがストリーム再生ではなかったらストリーム再生のハンドルに変更する
	if( sd->Type == DX_SOUNDTYPE_NORMAL )
	{
		// 非同期スレッドの場合はエラー
		if( ASyncThread )
			goto ERR ;

		if( SoundTypeChangeToStream( SoundHandle ) < 0 )
			goto ERR ;

		// エラー判定
		if( SOUNDHCHK( SoundHandle, sd ) )
			goto ERR ;
	}

	// 今の所ストリームデータが２つ以上あったら無理
	if( sd->Stream.FileNum > 1 )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x8c\x4e\x64\x30\xe5\x4e\x0a\x4e\x6e\x30\xf3\x97\xf0\x58\xd5\x30\xa1\x30\xa4\x30\xeb\x30\x4b\x30\x89\x30\x5c\x4f\x10\x62\x55\x30\x8c\x30\x5f\x30\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x6b\x30\xeb\x30\xfc\x30\xd7\x30\x8b\x95\xcb\x59\x4d\x4f\x6e\x7f\x92\x30\x07\x63\x9a\x5b\x59\x30\x8b\x30\x8b\x4e\x6f\x30\xfa\x51\x65\x67\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"二つ以上の音声ファイルから作成されたサウンドハンドルにループ開始位置を指定する事は出来ません\n" @*/ ) ;
		goto ERR ;
	}

	pl = &sd->Stream.File[0] ;

	// ループ位置を設定
	pl->LoopStartSamplePosition = LoopStartSamplePosition ;

	// 転送完了位置が既にループ開始位置を越えてしまっていたらストリームサウンドデータのセットアップをやり直す
	if( sd->Stream.FileCompCopyLength > pl->LoopStartSamplePosition )
	{
		RefreshStreamSoundPlayCompLength( SoundHandle ) ;
		SetStreamSoundCurrentPosition_UseGParam( ( int )sd->Stream.CompPlayWaveLength, SoundHandle, ASyncThread ) ;
	}

END :
	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// 終了
	return 0 ;

ERR :
	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// エラー終了
	return -1 ;
}

// サウンドハンドルにループ開始位置を設定する
extern int NS_SetLoopStartSamplePosSoundMem( int LoopStartSamplePosition, int SoundHandle )
{
	return SetLoopStartSamplePosSoundMem_UseGParam( LoopStartSamplePosition, SoundHandle, FALSE ) ;
}

// SetLoopAreaTimePosSoundMem のグローバル変数にアクセスしないバージョン
extern int SetLoopAreaTimePosSoundMem_UseGParam( int LoopStartTime, int LoopEndTime, int SoundHandle, int ASyncThread )
{
	// パラメータチェック
	if( LoopStartTime >= LoopEndTime )
	{
		return -1 ;
	}

	// ループ範囲の先端をセット
	if( SetLoopTimePosSoundMem_UseGParam( LoopStartTime, SoundHandle, ASyncThread ) < 0 )
	{
		return -1 ;
	}

	// ループ範囲の終端をセット
	if( SetLoopStartTimePosSoundMem_UseGParam( LoopEndTime, SoundHandle, ASyncThread ) < 0 )
	{
		return -1 ;
	}

	// 正常終了
	return 0 ;
}

// サウンドハンドルにループ範囲を設定する(ミリ秒単位)
extern int NS_SetLoopAreaTimePosSoundMem( int LoopStartTime, int LoopEndTime, int SoundHandle )
{
	return SetLoopAreaTimePosSoundMem_UseGParam( LoopStartTime, LoopEndTime, SoundHandle, FALSE ) ;
}

// サウンドハンドルにループ範囲を取得する(ミリ秒単位)
extern int NS_GetLoopAreaTimePosSoundMem( int *LoopStartTime, int *LoopEndTime, int SoundHandle )
{
	SOUND * sd ;
	int LoopStartSamplePosition ;
	int LoopEndSamplePosition ;
	int Result ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;
	
	// ループサンプル位置を取得
	Result = NS_GetLoopAreaSamplePosSoundMem( &LoopStartSamplePosition, &LoopEndSamplePosition, SoundHandle ) ;
	if( Result < 0 )
	{
		return -1 ;
	}

	if( LoopStartTime != NULL ) *LoopStartTime = SamplePositionToMilliSecPosition( ( int )sd->BufferFormat.nSamplesPerSec, LoopStartSamplePosition ) ;
	if( LoopEndTime   != NULL ) *LoopEndTime   = SamplePositionToMilliSecPosition( ( int )sd->BufferFormat.nSamplesPerSec, LoopEndSamplePosition   ) ;

	// 終了
	return 0 ;
}

// SetLoopAreaSamplePosSoundMem のグローバル変数にアクセスしないバージョン
extern int SetLoopAreaSamplePosSoundMem_UseGParam( int LoopStartSamplePosition, int LoopEndSamplePosition, int SoundHandle, int ASyncThread )
{
	// パラメータチェック
	if( LoopStartSamplePosition >= LoopEndSamplePosition )
	{
		return -1 ;
	}

	// ループ範囲の先端をセット
	if( SetLoopSamplePosSoundMem_UseGParam( LoopStartSamplePosition, SoundHandle, ASyncThread ) < 0 )
	{
		return -1 ;
	}

	// ループ範囲の終端をセット
	if( SetLoopStartSamplePosSoundMem_UseGParam( LoopEndSamplePosition, SoundHandle, ASyncThread ) < 0 )
	{
		return -1 ;
	}

	// 正常終了
	return 0 ;
}

// サウンドハンドルにループ範囲を設定する(サンプル単位)
extern int NS_SetLoopAreaSamplePosSoundMem( int LoopStartSamplePosition, int LoopEndSamplePosition, int SoundHandle )
{
	return SetLoopAreaSamplePosSoundMem_UseGParam( LoopStartSamplePosition, LoopEndSamplePosition, SoundHandle, FALSE ) ;
}

// サウンドハンドルにループ範囲を取得する(サンプル単位)
extern int NS_GetLoopAreaSamplePosSoundMem( int *LoopStartSamplePosition, int *LoopEndSamplePosition, int SoundHandle )
{
	SOUND * sd ;
	STREAMFILEDATA * pl ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;
	
	pl = &sd->Stream.File[0] ;

	if( LoopStartSamplePosition != NULL ) *LoopStartSamplePosition = pl->LoopSamplePosition ;
	if( LoopEndSamplePosition   != NULL ) *LoopEndSamplePosition   = pl->LoopStartSamplePosition ;

	// 終了
	return 0 ;
}

// サウンドハンドルの再生が終了したら自動的にハンドルを削除するかどうかを設定する
extern int NS_SetPlayFinishDeleteSoundMem( int DeleteFlag, int SoundHandle )
{
	SOUND *sd ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// 今までとフラグが同じ場合は何もしない
	if( sd->PlayFinishDeleteFlag == DeleteFlag )
	{
		return 0 ;
	}

	// フラグを保存
	sd->PlayFinishDeleteFlag = DeleteFlag ;

	// フラグが立てられる場合はリストに追加する、倒される場合はリストから外す
	if( DeleteFlag )
	{
		AddHandleList( &SoundSysData.PlayFinishDeleteSoundListFirst, &sd->PlayFinishDeleteSoundList, SoundHandle, sd ) ;
	}
	else
	{
		SubHandleList( &sd->PlayFinishDeleteSoundList ) ;
	}

	// 正常終了
	return 0 ;
}

// サウンドハンドルの３Ｄサウンド用のリバーブパラメータを設定する
extern int NS_Set3DReverbParamSoundMem( const SOUND3D_REVERB_PARAM *Param, int SoundHandle )
{
	SOUND *sd ;
	int i ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// ３Ｄサウンドではない場合は何もしない
	if( sd->Is3DSound == FALSE )
		return -1 ;

	// パラメータを保存
	sd->PresetReverbParam = -1 ;
	_MEMCPY( &sd->ReverbParam, Param, sizeof( sd->ReverbParam ) ) ;

	// バッファの数だけ繰り返し
	for( i = 0 ; i < sd->ValidBufferNum ; i ++ )
	{
		// パラメータの更新
		SoundBuffer_SetReverbParam( &sd->Buffer[ i ], &sd->ReverbParam ) ;
	}

	// 終了
	return 0 ;
}

// サウンドハンドルの３Ｄサウンド用のリバーブパラメータをプリセットを使用して設定する
extern int NS_Set3DPresetReverbParamSoundMem( int PresetNo /* DX_REVERB_PRESET_DEFAULT 等 */ , int SoundHandle )
{
	SOUND *sd ;
	int i ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;
	if( PresetNo < 0 || PresetNo >= DX_REVERB_PRESET_NUM )
		return -1 ;

	// ３Ｄサウンドではない場合は何もしない
	if( sd->Is3DSound == FALSE )
		return -1 ;

	// パラメータを保存
	sd->PresetReverbParam = PresetNo ;

	// バッファの数だけ繰り返し
	for( i = 0 ; i < sd->ValidBufferNum ; i ++ )
	{
		// パラメータの更新
		SoundBuffer_SetPresetReverbParam( &sd->Buffer[ i ], PresetNo ) ;
	}

	// 終了
	return 0 ;
}

// 全ての３Ｄサウンドのサウンドハンドルにリバーブパラメータを設定する
extern int NS_Set3DReverbParamSoundMemAll( const SOUND3D_REVERB_PARAM *Param, int PlaySoundOnly )
{
	HANDLELIST *List ;
	SOUND *Sound ;
	int i ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	if( PlaySoundOnly )
	{
		// クリティカルセクションの取得
		CRITICALSECTION_LOCK( &SoundSysData.Play3DSoundListCriticalSection ) ;

		List = SoundSysData.Play3DSoundListFirst.Next ;
	}
	else
	{
		// クリティカルセクションの取得
		CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

		List = SoundSysData._3DSoundListFirst.Next ;
	}
	for( ; List->Next != NULL ; List = List->Next )
	{
		Sound = ( SOUND * )List->Data ;

		// パラメータを保存
		Sound->PresetReverbParam = -1 ;
		_MEMCPY( &Sound->ReverbParam, Param, sizeof( Sound->ReverbParam ) ) ;

		// バッファの数だけ繰り返し
		for( i = 0 ; i < Sound->ValidBufferNum ; i ++ )
		{
			// パラメータの更新
			SoundBuffer_SetReverbParam( &Sound->Buffer[ i ], &Sound->ReverbParam ) ;
		}
	}

	if( PlaySoundOnly )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &SoundSysData.Play3DSoundListCriticalSection ) ;
	}
	else
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &SoundSysData._3DSoundListCriticalSection ) ;
	}

	// 終了
	return 0 ;
}

// 全ての３Ｄサウンドのサウンドハンドルにリバーブパラメータをプリセットを使用して設定する
extern int NS_Set3DPresetReverbParamSoundMemAll( int PresetNo /* DX_REVERB_PRESET_DEFAULT 等 */, int PlaySoundOnly )
{
	HANDLELIST *List ;
	SOUND *Sound ;
	int i ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	if( PresetNo < 0 || PresetNo >= DX_REVERB_PRESET_NUM )
		return -1 ;

	if( PlaySoundOnly )
	{
		// クリティカルセクションの取得
		CRITICALSECTION_LOCK( &SoundSysData.Play3DSoundListCriticalSection ) ;

		List = SoundSysData.Play3DSoundListFirst.Next ;
	}
	else
	{
		// クリティカルセクションの取得
		CRITICALSECTION_LOCK( &SoundSysData._3DSoundListCriticalSection ) ;

		List = SoundSysData._3DSoundListFirst.Next ;
	}
	for( ; List->Next != NULL ; List = List->Next )
	{
		Sound = ( SOUND * )List->Data ;

		// パラメータを保存
		Sound->PresetReverbParam = PresetNo ;

		// バッファの数だけ繰り返し
		for( i = 0 ; i < Sound->ValidBufferNum ; i ++ )
		{
			// パラメータの更新
			SoundBuffer_SetPresetReverbParam( &Sound->Buffer[ i ], PresetNo ) ;
		}
	}

	if( PlaySoundOnly )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &SoundSysData.Play3DSoundListCriticalSection ) ;
	}
	else
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &SoundSysData._3DSoundListCriticalSection ) ;
	}

	// 終了
	return 0 ;
}

// サウンドハンドルに設定されている３Ｄサウンド用のリバーブパラメータを取得する
extern int NS_Get3DReverbParamSoundMem( SOUND3D_REVERB_PARAM *ParamBuffer, int SoundHandle )
{
	SOUND *sd ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// ３Ｄサウンドではない場合は何もしない
	if( sd->Is3DSound == FALSE )
		return -1 ;

	// パラメータをコピー
	if( ParamBuffer != NULL ) *ParamBuffer = sd->ReverbParam ;

	// 正常終了
	return 0 ;
}

// プリセットの３Ｄサウンド用のリバーブパラメータを取得する
extern int NS_Get3DPresetReverbParamSoundMem( SOUND3D_REVERB_PARAM *ParamBuffer, int PresetNo /* DX_REVERB_PRESET_DEFAULT 等 */ )
{
	// エラー判定
	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	if( PresetNo < 0 || PresetNo >= DX_REVERB_PRESET_NUM )
	{
		return -1 ;
	}

	// 環境依存処理
	if( Get3DPresetReverbParamSoundMem_PF( ParamBuffer, PresetNo ) < 0 )
	{
		return -1 ;
	}

	// 正常終了
	return 0 ;
}

// サウンドハンドルの３Ｄサウンド用の再生位置を設定する
extern int NS_Set3DPositionSoundMem( VECTOR Position, int SoundHandle )
{
	SOUND *sd ;
	int i ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// ３Ｄサウンドではない場合は何もしない
	if( sd->Is3DSound == FALSE )
		return -1 ;

	// 1.0f = １メートルの単位に変換する
	VectorScale( &Position, &Position, 1.0f / SoundSysData._3DSoundOneMetre ) ;

	// バッファの数だけ繰り返し
	for( i = 0 ; i < sd->ValidBufferNum ; i ++ )
	{
		// パラメータの更新
		SoundBuffer_Set3DPosition( &sd->Buffer[ i ], &Position ) ;
	}
	sd->Base3DPosition = Position ;

	// 終了
	return 0 ;
}

// サウンドハンドルの３Ｄサウンド用の音が聞こえる距離を設定する
extern int NS_Set3DRadiusSoundMem( float Radius, int SoundHandle )
{
	SOUND *sd ;
	int i ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// ３Ｄサウンドではない場合は何もしない
	if( sd->Is3DSound == FALSE )
		return -1 ;

	Radius /= SoundSysData._3DSoundOneMetre ;

	// バッファの数だけ繰り返し
	for( i = 0 ; i < sd->ValidBufferNum ; i ++ )
	{
		// パラメータの更新
		SoundBuffer_Set3DRadius( &sd->Buffer[ i ], Radius ) ;
	}
	sd->Base3DRadius = Radius ;

	// 終了
	return 0 ;
}

// サウンドハンドルの３Ｄサウンド用の音量１００％で聞こえる距離を設定する
extern int NS_Set3DInnerRadiusSoundMem( float Radius, int SoundHandle )
{
	SOUND *sd ;
	int i ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// ３Ｄサウンドではない場合は何もしない
	if( sd->Is3DSound == FALSE )
		return -1 ;

	Radius /= SoundSysData._3DSoundOneMetre ;

	for( i = 0 ; i < sd->ValidBufferNum ; i ++ )
	{
		// パラメータの更新
		SoundBuffer_Set3DInnerRadius( &sd->Buffer[ i ], Radius ) ;
	}

	// 終了
	return 0 ;
}

// サウンドハンドルの３Ｄサウンド用の移動速度を設定する
extern int NS_Set3DVelocitySoundMem( VECTOR Velocity, int SoundHandle )
{
	SOUND *sd ;
	int i ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// ３Ｄサウンドではない場合は何もしない
	if( sd->Is3DSound == FALSE )
		return -1 ;

	// 1.0f = １メートルの単位に変換する
	VectorScale( &Velocity, &Velocity, 1.0f / SoundSysData._3DSoundOneMetre ) ;

	// バッファの数だけ繰り返し
	for( i = 0 ; i < sd->ValidBufferNum ; i ++ )
	{
		// パラメータの更新
		SoundBuffer_Set3DVelocity( &sd->Buffer[ i ], &Velocity ) ;
	}
	sd->Base3DVelocity = Velocity ;

	// 終了
	return 0 ;
}

// サウンドハンドルの３Ｄサウンド用の方向を決定するための前方座標と上方向を設定する
extern int NS_Set3DFrontPositionSoundMem( VECTOR FrontPosition, VECTOR UpVector, int SoundHandle )
{
	SOUND *sd ;
	int i ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// ３Ｄサウンドではない場合は何もしない
	if( sd->Is3DSound == FALSE )
		return -1 ;

	// バッファの数だけ繰り返し
	for( i = 0 ; i < sd->ValidBufferNum ; i ++ )
	{
		// パラメータの更新
		SoundBuffer_Set3DFrontPosition( &sd->Buffer[ i ], &FrontPosition, &UpVector ) ;
	}

	// 終了
	return 0 ;
}

// サウンドハンドルの３Ｄサウンド用の方向を決定するための前方座標を設定する( 上方向はY軸 )
extern int NS_Set3DFrontPosition_UpVecYSoundMem( VECTOR FrontPosition, int SoundHandle )
{
	VECTOR UpVec = { 0.0f, 1.0f, 0.0f } ;

	return NS_Set3DFrontPositionSoundMem( FrontPosition, UpVec, SoundHandle ) ;
}

// サウンドハンドルの３Ｄサウンド用の方向の角度範囲を設定する
extern int NS_Set3DConeAngleSoundMem( float InnerAngle, float OuterAngle, int SoundHandle )
{
	SOUND *sd ;
	int i ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// ３Ｄサウンドではない場合は何もしない
	if( sd->Is3DSound == FALSE )
		return -1 ;

	if( OuterAngle < 0.0f )
	{
		OuterAngle = 0.0f ;
	}
	else
	if( OuterAngle > DX_PI_F * 2.0f )
	{
		OuterAngle = DX_PI_F * 2.0f ;
	}

	if( InnerAngle < 0.0f )
	{
		InnerAngle = 0.0f ;
	}
	else
	if( InnerAngle > DX_PI_F * 2.0f )
	{
		InnerAngle = DX_PI_F * 2.0f ;
	}

	if( OuterAngle < InnerAngle )
	{
		float Temp ;
		Temp = InnerAngle ;
		InnerAngle = OuterAngle ;
		OuterAngle = Temp ;
	}

	for( i = 0 ; i < sd->ValidBufferNum ; i ++ )
	{
		// パラメータの更新
		SoundBuffer_Set3DConeAngle( &sd->Buffer[ i ], InnerAngle, OuterAngle ) ;
	}

	// 終了
	return 0 ;
}

// サウンドハンドルの３Ｄサウンド用の方向の角度範囲の音量倍率を設定する
extern int NS_Set3DConeVolumeSoundMem( float InnerAngleVolume, float OuterAngleVolume, int SoundHandle )
{
	SOUND *sd ;
	int i ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// ３Ｄサウンドではない場合は何もしない
	if( sd->Is3DSound == FALSE )
		return -1 ;

	// バッファーの数だけ繰り返し
	for( i = 0 ; i < sd->ValidBufferNum ; i ++ )
	{
		// パラメータの更新
		SoundBuffer_Set3DConeVolume( &sd->Buffer[ i ], InnerAngleVolume, OuterAngleVolume ) ;
	}

	// 終了
	return 0 ;
}

// サウンドハンドルの次の再生のみに使用する３Ｄサウンド用の再生位置を設定する
extern int NS_SetNextPlay3DPositionSoundMem( VECTOR Position, int SoundHandle )
{
	SOUND * Sound ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	// ３Ｄサウンドではない場合は何もしない
	if( Sound->Is3DSound == FALSE )
		return -1 ;

	// 1.0f = １メートルの単位に変換する
	VectorScale( &Position, &Position, 1.0f / SoundSysData._3DSoundOneMetre ) ;

	Sound->NextPlay3DPosition = Position ;
	Sound->ValidNextPlay3DPosition = 1 ;

	// 終了
	return 0 ;
}

// サウンドハンドルの次の再生のみに使用する３Ｄサウンド用の音が聞こえる距離を設定する
extern int NS_SetNextPlay3DRadiusSoundMem( float Radius, int SoundHandle )
{
	SOUND * Sound ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	// ３Ｄサウンドではない場合は何もしない
	if( Sound->Is3DSound == FALSE )
		return -1 ;

	Radius /= SoundSysData._3DSoundOneMetre ;

	Sound->NextPlay3DRadius = Radius ;
	Sound->ValidNextPlay3DRadius = 1 ;

	// 終了
	return 0 ;
}

// サウンドハンドルの次の再生のみに使用する３Ｄサウンド用の移動速度を設定する
extern int NS_SetNextPlay3DVelocitySoundMem( VECTOR Velocity, int SoundHandle )
{
	SOUND * Sound ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	// ３Ｄサウンドではない場合は何もしない
	if( Sound->Is3DSound == FALSE )
		return -1 ;

	// 1.0f = １メートルの単位に変換する
	VectorScale( &Velocity, &Velocity, 1.0f / SoundSysData._3DSoundOneMetre ) ;

	Sound->NextPlay3DVelocity = Velocity ;
	Sound->ValidNextPlay3DVelocity = 1 ;

	// 終了
	return 0 ;
}



















// 特殊関数


// MP3ファイルのタグ情報を取得する
extern int NS_GetMP3TagInfo( const TCHAR *FileName, TCHAR *TitleBuffer, size_t TitleBufferBytes, TCHAR *ArtistBuffer, size_t ArtistBufferBytes, TCHAR *AlbumBuffer, size_t AlbumBufferBytes, TCHAR *YearBuffer, size_t YearBufferBytes, TCHAR *CommentBuffer, size_t CommentBufferBytes, TCHAR *TrackBuffer, size_t TrackBufferBytes, TCHAR *GenreBuffer, size_t GenreBufferBytes, int *PictureGrHandle )
{
#ifdef UNICODE
	return GetMP3TagInfo_WCHAR_T(
		FileName, TitleBuffer, TitleBufferBytes, ArtistBuffer, ArtistBufferBytes, AlbumBuffer, AlbumBufferBytes, YearBuffer, YearBufferBytes, CommentBuffer, CommentBufferBytes, TrackBuffer, TrackBufferBytes, GenreBuffer, GenreBufferBytes, PictureGrHandle
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, return -1 )

	Result = GetMP3TagInfo_WCHAR_T(
		UseFileNameBuffer, TitleBuffer, TitleBufferBytes, ArtistBuffer, ArtistBufferBytes, AlbumBuffer, AlbumBufferBytes, YearBuffer, YearBufferBytes, CommentBuffer, CommentBufferBytes, TrackBuffer, TrackBufferBytes, GenreBuffer, GenreBufferBytes, PictureGrHandle
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( FileName )

	return Result ;
#endif
}

extern int NS_GetMP3TagInfoWithStrLen( const TCHAR *FileName, size_t FileNameLength, TCHAR *TitleBuffer, size_t TitleBufferBytes, TCHAR *ArtistBuffer, size_t ArtistBufferBytes, TCHAR *AlbumBuffer, size_t AlbumBufferBytes, TCHAR *YearBuffer, size_t YearBufferBytes, TCHAR *CommentBuffer, size_t CommentBufferBytes, TCHAR *TrackBuffer, size_t TrackBufferBytes, TCHAR *GenreBuffer, size_t GenreBufferBytes, int *PictureGrHandle )
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = GetMP3TagInfo_WCHAR_T( UseFileNameBuffer, TitleBuffer, TitleBufferBytes, ArtistBuffer, ArtistBufferBytes, AlbumBuffer, AlbumBufferBytes, YearBuffer, YearBufferBytes, CommentBuffer, CommentBufferBytes, TrackBuffer, TrackBufferBytes, GenreBuffer, GenreBufferBytes, PictureGrHandle ) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = GetMP3TagInfo_WCHAR_T( UseFileNameBuffer, TitleBuffer, TitleBufferBytes, ArtistBuffer, ArtistBufferBytes, AlbumBuffer, AlbumBufferBytes, YearBuffer, YearBufferBytes, CommentBuffer, CommentBufferBytes, TrackBuffer, TrackBufferBytes, GenreBuffer, GenreBufferBytes, PictureGrHandle ) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#endif
	return Result ;
}

static DWORD ReadID3v2SizeData( int MainVer, DWORD_PTR fp )
{
	BYTE NumData[ 4 ] ;

	DX_FREAD( NumData, 1, 4, fp ) ;

	if( MainVer == 4 )
	{
		return ( DWORD )( ( ( NumData[ 0 ] & 0x7f ) << 21 ) + ( ( NumData[ 1 ] & 0x7f ) << 14 ) + ( ( NumData[ 2 ] & 0x7f ) << 7 ) + ( NumData[ 3 ] & 0x7f ) ) ;
	}
	else
	{
		return ( DWORD )( ( NumData[ 0 ] << 24 ) + ( NumData[ 1 ] << 16 ) + ( NumData[ 2 ] << 8 ) + NumData[ 3 ] ) ;
	}
}

static size_t ReadID3v2Text( BYTE *FrameData, TCHAR *StringBuffer, size_t StringBufferBytes, size_t DataOffset )
{
	size_t Bytes = 0 ;

	switch( FrameData[ 0 ] )
	{
	case 0 :	// ISO-8859-1
		Bytes = CL_strlen( DX_CHARCODEFORMAT_ASCII, ( char * )&FrameData[ DataOffset ] ) + 1 ;
		if( StringBuffer != NULL && StringBufferBytes > 0 )
		{
			ConvString( ( char * )&FrameData[ DataOffset ], -1, DX_CHARCODEFORMAT_ASCII, ( char * )StringBuffer, StringBufferBytes, _TCHARCODEFORMAT ) ;
		}
		break ;

	case 1 :	// UTF-16 / BOMあり
		if( FrameData[ DataOffset ] == 0xfe && FrameData[ DataOffset + 1 ] == 0xff )
		{
			Bytes = ( CL_strlen( DX_CHARCODEFORMAT_UTF16BE, ( char * )&FrameData[ DataOffset + 2 ] ) + 1 ) * 2 ;
			if( StringBuffer != NULL && StringBufferBytes > 0 )
			{
				ConvString( ( char * )&FrameData[ DataOffset + 2 ], -1, DX_CHARCODEFORMAT_UTF16BE, ( char * )StringBuffer, StringBufferBytes, _TCHARCODEFORMAT ) ;
			}
		}
		else
		{
			Bytes = ( CL_strlen( DX_CHARCODEFORMAT_UTF16LE, ( char * )&FrameData[ DataOffset + 2 ] ) + 1 ) * 2 ;
			if( StringBuffer != NULL && StringBufferBytes > 0 )
			{
				ConvString( ( char * )&FrameData[ DataOffset + 2 ], -1, DX_CHARCODEFORMAT_UTF16LE, ( char * )StringBuffer, StringBufferBytes, _TCHARCODEFORMAT ) ;
			}
		}
		Bytes += 2 ;
		break ;

	case 2 :	// UTF-16BE / BOMなし
		Bytes = ( CL_strlen( DX_CHARCODEFORMAT_UTF16BE, ( char * )&FrameData[ DataOffset ] ) + 1 ) * 2 ;
		if( StringBuffer != NULL && StringBufferBytes > 0 )
		{
			ConvString( ( char * )&FrameData[ DataOffset ], -1, DX_CHARCODEFORMAT_UTF16BE, ( char * )StringBuffer, StringBufferBytes, _TCHARCODEFORMAT ) ;
		}
		break ;

	case 3 :	// UTF-8
		Bytes = CL_strlen( DX_CHARCODEFORMAT_UTF8, ( char * )&FrameData[ DataOffset ] ) + 1 ;
		if( StringBuffer != NULL && StringBufferBytes > 0 )
		{
			ConvString( ( char * )&FrameData[ DataOffset ], -1, DX_CHARCODEFORMAT_UTF8, ( char * )StringBuffer, StringBufferBytes, _TCHARCODEFORMAT ) ;
		}
		break ;
	}

	return Bytes ;
}

extern int GetMP3TagInfo_WCHAR_T( const wchar_t *FileName, TCHAR *TitleBuffer, size_t TitleBufferBytes, TCHAR *ArtistBuffer, size_t ArtistBufferBytes, TCHAR *AlbumBuffer, size_t AlbumBufferBytes, TCHAR *YearBuffer, size_t YearBufferBytes, TCHAR *CommentBuffer, size_t CommentBufferBytes, TCHAR *TrackBuffer, size_t TrackBufferBytes, TCHAR *GenreBuffer, size_t GenreBufferBytes, int *PictureGrHandle )
{
	DWORD_PTR fp = 0 ;
	size_t FileSize ;
	char Tag[ 4 ] ;
	char TempStringBuffer[ 2048 ] ;
	int v1Valid = FALSE ;
	int v2Valid = FALSE ;
	BYTE *FrameBuffer = NULL ;
	size_t FrameBufferSize = 0 ;

	// ファイルを開く
	fp = DX_FOPEN( FileName ) ;
	if( fp == 0 )
	{
		goto ERR ;
	}

	if( PictureGrHandle != NULL )
	{
		*PictureGrHandle = -1 ;
	}

	// ファイルサイズを得る
	DX_FSEEK( fp, 0, SEEK_END ) ;
	FileSize = ( size_t )DX_FTELL( fp ) ;
	DX_FSEEK( fp, 0, SEEK_SET ) ;

	// ID3v1 タグのチェック
	if( FileSize > 128 )
	{
		BYTE ID3v1[ 128 ] ;

		// タグを丸ごと読み込み
		DX_FSEEK( fp, -128, SEEK_END ) ;
		DX_FREAD( ID3v1, 128, 1, fp ) ;
		DX_FSEEK( fp, 0, SEEK_SET ) ;

		Tag[ 0 ] = ID3v1[ 0 ] ;
		Tag[ 1 ] = ID3v1[ 1 ] ;
		Tag[ 2 ] = ID3v1[ 2 ] ;
		Tag[ 3 ] = '\0' ;

		if( CL_strcmp( DX_CHARCODEFORMAT_ASCII, Tag, "TAG" ) == 0 )
		{
			int TrackNo ;
			int Genre ;

			v1Valid = TRUE ;

			// 曲名の取得
			_MEMCPY( TempStringBuffer, &ID3v1[ 3 ], 30 ) ;
			TempStringBuffer[ 30 ] = '\0' ;
			if( TitleBufferBytes > 0 && TitleBuffer != NULL )
			{
				ConvString( TempStringBuffer, -1, DX_CHARCODEFORMAT_SHIFTJIS, ( char * )TitleBuffer, TitleBufferBytes, _TCHARCODEFORMAT ) ;
			}

			// アーティスト名の取得
			_MEMCPY( TempStringBuffer, &ID3v1[ 33 ], 30 ) ;
			TempStringBuffer[ 30 ] = '\0' ;
			if( ArtistBufferBytes > 0 && ArtistBuffer != NULL )
			{
				ConvString( TempStringBuffer, -1, DX_CHARCODEFORMAT_SHIFTJIS, ( char * )ArtistBuffer, ArtistBufferBytes, _TCHARCODEFORMAT ) ;
			}

			// アルバム名の取得
			_MEMCPY( TempStringBuffer, &ID3v1[ 63 ], 30 ) ;
			TempStringBuffer[ 30 ] = '\0' ;
			if( AlbumBufferBytes > 0 && AlbumBuffer != NULL )
			{
				ConvString( TempStringBuffer, -1, DX_CHARCODEFORMAT_SHIFTJIS, ( char * )AlbumBuffer, AlbumBufferBytes, _TCHARCODEFORMAT ) ;
			}

			// 作曲年の取得
			_MEMCPY( TempStringBuffer, &ID3v1[ 93 ], 4 ) ;
			TempStringBuffer[ 4 ] = '\0' ;
			if( YearBufferBytes > 0 && YearBuffer != NULL )
			{
				ConvString( TempStringBuffer, -1, DX_CHARCODEFORMAT_SHIFTJIS, ( char * )YearBuffer, CommentBufferBytes, _TCHARCODEFORMAT ) ;
			}

			// コメントの取得
			_MEMCPY( TempStringBuffer, &ID3v1[ 97 ], 30 ) ;
			TempStringBuffer[ 30 ] = '\0' ;
			if( CommentBufferBytes > 0 && CommentBuffer != NULL )
			{
				ConvString( TempStringBuffer, -1, DX_CHARCODEFORMAT_SHIFTJIS, ( char * )CommentBuffer, CommentBufferBytes, _TCHARCODEFORMAT ) ;
			}

			// トラック番号の取得
			if( ID3v1[ 125 ] == 0 )
			{
				TrackNo = ( int )ID3v1[ 126 ] ;
			}
			else
			{
				TrackNo = -1 ;
			}

			// ジャンル番号の取得
			Genre = ( int )ID3v1[ 127 ] ;
		}
	}

	// ID3v2 タグのチェック
	{
		BYTE MainVer, SubVer, Flag, SizeData[4] ;
		size_t TagSize ;
		DWORD ExHeadSize ;
		size_t ReadSize = 0 ;
		int ValidFooter ;
		size_t FrameHeadSize ;
		DWORD FrameSize ;
		TCHAR Desc[ 256 ] ;
		int JpnCommValid = FALSE ;

		Tag[ 3 ] = '\0' ;
		DX_FREAD( Tag, 1, 3, fp ) ;
		if( CL_strcmp( DX_CHARCODEFORMAT_ASCII, Tag, "ID3" ) == 0 )
		{
			v2Valid = TRUE ;

			// バージョンを得る
			DX_FREAD( &MainVer, 1, 1, fp ) ;
			DX_FREAD( &SubVer, 1, 1, fp ) ;
			FrameHeadSize = MainVer >= 3 ? 10 : 6 ;

			// フラグを得る
			DX_FREAD( &Flag, 1, 1, fp ) ;

			// 容量情報を得る
			DX_FREAD( SizeData, 1, 4, fp ) ;
			TagSize = ( DWORD )( ( ( SizeData[0] & 0x7f ) << 21 ) + ( ( SizeData[1] & 0x7f ) << 14 ) + ( ( SizeData[2] & 0x7f ) << 7 ) + ( SizeData[3] & 0x7f ) + 10 ) ;

			// フッタがあるかどうかチェック
			ValidFooter = MainVer == 4 && ( Flag & 0x10 ) != 0 ? TRUE : FALSE ;

			// 拡張ヘッダは無視
			if( ( Flag & 0x40 ) != 0 )
			{
				ExHeadSize = ReadID3v2SizeData( MainVer, fp ) ;
				if( ExHeadSize > 4 )
				{
					DX_FSEEK( fp, ExHeadSize - 4, SEEK_CUR ) ;
				}
				ReadSize = ExHeadSize ;
			}

			// フレーム解析
			while( ReadSize + FrameHeadSize + 1 < TagSize )
			{
				char FrameName[ 5 ] ;

				if( MainVer == 2 )
				{
					BYTE NumData[ 3 ] ;

					DX_FREAD( FrameName, 3, 1, fp ) ;
					FrameName[ 3 ] = 0 ;

					DX_FREAD( NumData, 1, 3, fp ) ;
					FrameSize = ( DWORD )( ( NumData[ 0 ] << 16 ) + ( NumData[ 1 ] << 8 ) + NumData[ 2 ] ) ;

					ReadSize += FrameSize + 6 ;

					if( FrameBufferSize < FrameSize + 2 )
					{
						if( FrameBuffer != NULL )
						{
							DXFREE( FrameBuffer ) ;
							FrameBuffer = NULL ;
						}

						FrameBufferSize = FrameSize + 2 ;
						FrameBuffer = ( BYTE * )DXALLOC( FrameBufferSize ) ;
						if( FrameBuffer == NULL )
						{
							goto ERR ;
						}
					}

					DX_FREAD( FrameBuffer, FrameSize, 1, fp ) ;
					FrameBuffer[ FrameSize + 0 ] = 0 ;
					FrameBuffer[ FrameSize + 1 ] = 0 ;

					if( CL_strcmp( DX_CHARCODEFORMAT_ASCII, FrameName, "TT2" ) == 0 )
					{
						ReadID3v2Text( FrameBuffer, TitleBuffer, TitleBufferBytes, 1 ) ;
					}
					else
					if( CL_strcmp( DX_CHARCODEFORMAT_ASCII, FrameName, "TP1" ) == 0 )
					{
						ReadID3v2Text( FrameBuffer, ArtistBuffer, ArtistBufferBytes, 1 ) ;
					}
					else
					if( CL_strcmp( DX_CHARCODEFORMAT_ASCII, FrameName, "TAL" ) == 0 )
					{
						ReadID3v2Text( FrameBuffer, AlbumBuffer, AlbumBufferBytes, 1 ) ;
					}
					else
					if( CL_strcmp( DX_CHARCODEFORMAT_ASCII, FrameName, "TYE" ) == 0 )
					{
						ReadID3v2Text( FrameBuffer, YearBuffer, YearBufferBytes, 1 ) ;
					}
					else
					if( CL_strcmp( DX_CHARCODEFORMAT_ASCII, FrameName, "TCO" ) == 0 )
					{
						ReadID3v2Text( FrameBuffer, GenreBuffer, GenreBufferBytes, 1 ) ;
					}
					else
					if( CL_strcmp( DX_CHARCODEFORMAT_ASCII, FrameName, "TRK" ) == 0 )
					{
						ReadID3v2Text( FrameBuffer, TrackBuffer, TrackBufferBytes, 1 ) ;
					}
					else
					if( CL_strcmp( DX_CHARCODEFORMAT_ASCII, FrameName, "COM" ) == 0 )
					{
						char LNG[ 4 ] ;

						LNG[ 0 ] = FrameBuffer[ 1 ] ;
						LNG[ 1 ] = FrameBuffer[ 2 ] ;
						LNG[ 2 ] = FrameBuffer[ 3 ] ;
						LNG[ 3 ] = '\0 ' ;
						if( JpnCommValid == FALSE || CL_strcmp( DX_CHARCODEFORMAT_ASCII, FrameName, "jpn" ) == 0 )
						{
							ReadID3v2Text( FrameBuffer, CommentBuffer, CommentBufferBytes, 4 ) ;
							if( CL_strcmp( DX_CHARCODEFORMAT_ASCII, FrameName, "jpn" ) == 0 )
							{
								JpnCommValid = TRUE ;
							}
						}
					}
					else
					if( CL_strcmp( DX_CHARCODEFORMAT_ASCII, FrameName, "PIC" ) == 0 )
					{
						// BYTE Encode = FrameBuffer[ 0 ] ;
						// char format[ 4 ] ;
						// format[ 0 ] = ( char )FrameBuffer[ 1 ] ;
						// format[ 1 ] = ( char )FrameBuffer[ 2 ] ;
						// format[ 2 ] = ( char )FrameBuffer[ 3 ] ;
						// format[ 3 ] = 0 ;
						// BYTE Type = FrameBuffer[ 4 ] ;
						size_t DescSize ;
						DescSize = ReadID3v2Text( FrameBuffer, Desc, sizeof( Desc ), 5 ) ;
						BYTE *ImageData = &FrameBuffer[ DescSize + 5 ] ;

						if( PictureGrHandle != NULL )
						{
							*PictureGrHandle = NS_CreateGraphFromMem( ImageData, ( int )( FrameSize - ( DescSize + 5 ) ) ) ;
						}
					}
				}
				else
				{
					DX_FREAD( FrameName, 4, 1, fp ) ;
					FrameName[ 4 ] = 0 ;

					FrameSize = ReadID3v2SizeData( MainVer, fp ) ;

					DX_FSEEK( fp, 2, SEEK_CUR ) ;

					ReadSize += FrameSize + 10 ;

					if( FrameBufferSize < FrameSize + 2 )
					{
						if( FrameBuffer != NULL )
						{
							DXFREE( FrameBuffer ) ;
							FrameBuffer = NULL ;
						}

						FrameBufferSize = FrameSize + 2 ;
						FrameBuffer = ( BYTE * )DXALLOC( FrameBufferSize ) ;
						if( FrameBuffer == NULL )
						{
							goto ERR ;
						}
					}

					DX_FREAD( FrameBuffer, FrameSize, 1, fp ) ;
					FrameBuffer[ FrameSize + 0 ] = 0 ;
					FrameBuffer[ FrameSize + 1 ] = 0 ;

					if( CL_strcmp( DX_CHARCODEFORMAT_ASCII, FrameName, "TIT2" ) == 0 )
					{
						ReadID3v2Text( FrameBuffer, TitleBuffer, TitleBufferBytes, 1 ) ;
					}
					else
					if( CL_strcmp( DX_CHARCODEFORMAT_ASCII, FrameName, "TPE1" ) == 0 )
					{
						ReadID3v2Text( FrameBuffer, ArtistBuffer, ArtistBufferBytes, 1 ) ;
					}
					else
					if( CL_strcmp( DX_CHARCODEFORMAT_ASCII, FrameName, "TALB" ) == 0 )
					{
						ReadID3v2Text( FrameBuffer, AlbumBuffer, AlbumBufferBytes, 1 ) ;
					}
					else
					if( CL_strcmp( DX_CHARCODEFORMAT_ASCII, FrameName, "TYER" ) == 0 )
					{
						ReadID3v2Text( FrameBuffer, YearBuffer, YearBufferBytes, 1 ) ;
					}
					else
					if( CL_strcmp( DX_CHARCODEFORMAT_ASCII, FrameName, "TCON" ) == 0 )
					{
						ReadID3v2Text( FrameBuffer, GenreBuffer, GenreBufferBytes, 1 ) ;
					}
					else
					if( CL_strcmp( DX_CHARCODEFORMAT_ASCII, FrameName, "TRCK" ) == 0 )
					{
						ReadID3v2Text( FrameBuffer, TrackBuffer, TrackBufferBytes, 1 ) ;
					}
					else
					if( CL_strcmp( DX_CHARCODEFORMAT_ASCII, FrameName, "COMM" ) == 0 )
					{
						char LNG[ 4 ] ;

						LNG[ 0 ] = FrameBuffer[ 1 ] ;
						LNG[ 1 ] = FrameBuffer[ 2 ] ;
						LNG[ 2 ] = FrameBuffer[ 3 ] ;
						LNG[ 3 ] = '\0 ' ;
						if( JpnCommValid == FALSE || CL_strcmp( DX_CHARCODEFORMAT_ASCII, FrameName, "jpn" ) == 0 )
						{
							ReadID3v2Text( FrameBuffer, CommentBuffer, CommentBufferBytes, 4 ) ;
							if( CL_strcmp( DX_CHARCODEFORMAT_ASCII, FrameName, "jpn" ) == 0 )
							{
								JpnCommValid = TRUE ;
							}
						}
					}
					else
					if( CL_strcmp( DX_CHARCODEFORMAT_ASCII, FrameName, "APIC" ) == 0 )
					{
						// BYTE Encode = FrameBuffer[ 0 ] ;
						char *Mime = ( char * )&FrameBuffer[ 1 ] ;
						size_t MimeSize = CL_strlen( DX_CHARCODEFORMAT_ASCII, Mime ) + 1 ;
						// BYTE Type = FrameBuffer[ MimeSize + 1 ] ;
						size_t DescSize = ReadID3v2Text( FrameBuffer, Desc, sizeof( Desc ), MimeSize + 2 ) ;
						BYTE *ImageData = &FrameBuffer[ MimeSize + 2 + DescSize ] ; 

						if( PictureGrHandle != NULL )
						{
							*PictureGrHandle = NS_CreateGraphFromMem( ImageData, ( int )( FrameSize - ( MimeSize + 2 + DescSize ) ) ) ;
						}
					}
				}
			}
		}
	}

	if( FrameBuffer != NULL )
	{
		DXFREE( FrameBuffer ) ;
		FrameBuffer = NULL ;
	}

	if( fp != 0 )
	{
		DX_FCLOSE( fp ) ;
		fp = 0 ;
	}

	return v1Valid || v2Valid ? 0 : -1 ;

ERR :
	if( FrameBuffer != NULL )
	{
		DXFREE( FrameBuffer ) ;
		FrameBuffer = NULL ;
	}

	if( fp != 0 )
	{
		DX_FCLOSE( fp ) ;
		fp = 0 ;
	}

	return -1 ;
}























#ifndef DX_NON_BEEP

// BEEP音再生用命令

// ビープ音周波数設定関数
extern int NS_SetBeepFrequency( int Freq )
{
	int OldUseIndex ;
	SOUND_BEEP_BUFFERDATA *BeepBuf ;

	// 不正な周波数の場合は何もせず終了
	if( Freq == 0 || Freq > SOUND_BEEP_SAMPLEPERSEC / 2 )
	{
		return -1 ;
	}

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// 周波数が全く同じ場合は何もせずに終了
	if( SoundSysData.BeepFrequency == Freq )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

		return 0 ;
	}

	// 使用するバッファを進める
	OldUseIndex = SoundSysData.BeepSoundBufferUseIndex ;
	SoundSysData.BeepSoundBufferUseIndex ++ ;
	if( SoundSysData.BeepSoundBufferUseIndex == SOUND_BEEPSOUNDBUFFER_NUM )
	{
		SoundSysData.BeepSoundBufferUseIndex = 0 ;
	}
	BeepBuf = &SoundSysData.BeepSoundBuffer[ SoundSysData.BeepSoundBufferUseIndex ] ;

	// バッファが使用中の場合は止める
	if( SoundBuffer_CheckPlay( &BeepBuf->Buffer ) )
	{
		SoundBuffer_Stop( &BeepBuf->Buffer ) ;
	}

	// 状態を初期化
	BeepBuf->NextFillBufferPosition = 0 ;
	BeepBuf->NextUseOneCycleSampleBufferPosition = 0 ;
	BeepBuf->OneCycleSamples = 0 ;
	BeepBuf->VolumeUpRequest = FALSE ;
	BeepBuf->PlayTime = 0 ;
	BeepBuf->StopRequest = FALSE ;
	BeepBuf->StopTime = 0 ;

	// パラメータを決定
	BeepBuf->OneCycleSamples = SOUND_BEEP_SAMPLEPERSEC / Freq ;

	// 1周分の波形データを作成する
	{
		int i ;
		float UnitAngle ;

		if( BeepBuf->OneCycleSampleBuffer == NULL ||
			BeepBuf->OneCycleSamples * SOUND_BEEP_SAMPLEPERBYTES > BeepBuf->OneCycleSampleBufferSize )
		{
			BeepBuf->OneCycleSampleBufferSize = BeepBuf->OneCycleSamples * SOUND_BEEP_SAMPLEPERBYTES ;
			BeepBuf->OneCycleSampleBuffer = ( short * )DXALLOC( BeepBuf->OneCycleSampleBufferSize ) ;
			if( BeepBuf->OneCycleSampleBuffer == NULL )
			{
				// クリティカルセクションの解放
				CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

				DXST_LOGFILE_ADDUTF16LE( "\x42\x00\x45\x00\x45\x00\x50\x00\xf3\x97\x28\x75\x6e\x30\x5c\x4f\x6d\x69\x28\x75\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"BEEP音用の作業用メモリの確保に失敗しました\n" @*/ ) ;
				return -1 ;
			}
		}

		UnitAngle = DX_TWO_PI_F / BeepBuf->OneCycleSamples ;
		for( i = 0 ; i < BeepBuf->OneCycleSamples ; i ++ )
		{
			float Sin, Cos ;

			_TABLE_SINCOS( UnitAngle * i, &Sin, &Cos ) ;
			BeepBuf->OneCycleSampleBuffer[ i ] = ( short )_FTOL( Sin * 32765.0f ) ;
		}
	}

	// 波形データをセットする
	if( BeepSound_FillSamples( SoundSysData.BeepSoundBufferUseIndex ) < 0 )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;
		return -1 ;
	}

	// 周波数を保存
	SoundSysData.BeepFrequency = Freq ;

	// BEEPを再生中だった場合は以前のBEEPの再生を終了して、新たなBEEPの再生を開始する
	if( SoundSysData.BeepPlay )
	{
		// 今まで再生していたバッファを止めるリクエストを設定する
//		SoundBuffer_Stop( &SoundSysData.BeepSoundBuffer[ OldUseIndex ] ) ;
		SoundBuffer_SetVolume( &SoundSysData.BeepSoundBuffer[ OldUseIndex ].Buffer, 0, -10000 ) ;
		SoundSysData.BeepSoundBuffer[ OldUseIndex ].VolumeUpRequest = FALSE ;
		SoundSysData.BeepSoundBuffer[ OldUseIndex ].PlayTime        = FALSE ;
		SoundSysData.BeepSoundBuffer[ OldUseIndex ].StopRequest     = TRUE ;
		SoundSysData.BeepSoundBuffer[ OldUseIndex ].StopTime        = NS_GetNowCount() ;

		SoundBuffer_SetVolume( &BeepBuf->Buffer, 0, -10000 ) ;
//		SoundBuffer_SetVolume( &BeepBuf->Buffer, 0, 0 ) ;
//		SoundBuffer_Play( &BeepBuf->Buffer, TRUE ) ;
		BeepBuf->VolumeUpRequest = TRUE ;
		BeepBuf->PlayTime        = NS_GetNowCount() ;
		BeepBuf->VolumeUpRequest = FALSE ;
		BeepBuf->PlayTime        = 0 ;
		BeepBuf->StopRequest     = FALSE ;
		BeepBuf->StopTime        = 0 ;
	}

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// 終了
	return 0 ;
}

// ビープ音を再生する
extern int NS_PlayBeep( void )
{
	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// 再生中だった場合はなにもしないで終了
	if( SoundSysData.BeepPlay )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;
		return 0 ;
	}

	// 周波数が設定されていなかったらデフォルトの周波数を設定
	if( SoundSysData.BeepFrequency == 0 )
	{
		if( NS_SetBeepFrequency( SOUND_DEFAULT_BEEP_FREQUENCY ) < 0 )
		{
			// クリティカルセクションの解放
			CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;
			return -1 ;
		}
	}

	// 再生
	SoundBuffer_SetVolume( &SoundSysData.BeepSoundBuffer[ SoundSysData.BeepSoundBufferUseIndex ].Buffer, 0, -10000 ) ;
//	SoundBuffer_SetVolume( &SoundSysData.BeepSoundBuffer[ SoundSysData.BeepSoundBufferUseIndex ].Buffer, 0, 0 ) ;
	SoundBuffer_Play( &SoundSysData.BeepSoundBuffer[ SoundSysData.BeepSoundBufferUseIndex ].Buffer, TRUE ) ;
	SoundSysData.BeepSoundBuffer[ SoundSysData.BeepSoundBufferUseIndex ].VolumeUpRequest = TRUE ;
	SoundSysData.BeepSoundBuffer[ SoundSysData.BeepSoundBufferUseIndex ].PlayTime        = NS_GetNowCount() ;
//	SoundSysData.BeepSoundBuffer[ SoundSysData.BeepSoundBufferUseIndex ].VolumeUpRequest = FALSE ;
//	SoundSysData.BeepSoundBuffer[ SoundSysData.BeepSoundBufferUseIndex ].PlayTime        = 0 ;
	SoundSysData.BeepSoundBuffer[ SoundSysData.BeepSoundBufferUseIndex ].StopRequest     = FALSE ;
	SoundSysData.BeepSoundBuffer[ SoundSysData.BeepSoundBufferUseIndex ].StopTime        = 0 ;

	// 再生中状態にする
	SoundSysData.BeepPlay = TRUE ;

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// 終了
	return 0 ;
}

// ビープ音を止める	
extern int NS_StopBeep( void )
{
	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// 再生中ではなかったら何もせずに終了
	if( SoundSysData.BeepPlay == FALSE )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

		return 0 ;
	}

	// 再生停止の代わりに音量を0にして、再生終了待ち状態にする
//	SoundBuffer_Stop( &SoundSysData.BeepSoundBuffer[ SoundSysData.BeepSoundBufferUseIndex ].Buffer ) ;
	SoundBuffer_SetVolume( &SoundSysData.BeepSoundBuffer[ SoundSysData.BeepSoundBufferUseIndex ].Buffer, 0, -5000 ) ;
	SoundSysData.BeepSoundBuffer[ SoundSysData.BeepSoundBufferUseIndex ].VolumeUpRequest = FALSE ;
	SoundSysData.BeepSoundBuffer[ SoundSysData.BeepSoundBufferUseIndex ].PlayTime        = FALSE ;
	SoundSysData.BeepSoundBuffer[ SoundSysData.BeepSoundBufferUseIndex ].StopRequest     = TRUE ;
	SoundSysData.BeepSoundBuffer[ SoundSysData.BeepSoundBufferUseIndex ].StopTime        = NS_GetNowCount() ;

	// 再生中フラグを倒す
	SoundSysData.BeepPlay = FALSE ;

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// 終了
	return 0 ;
}

#endif // DX_NON_BEEP





















// 設定関係関数

// 作成するサウンドのデータ形式を設定する
extern int NS_SetCreateSoundDataType( int SoundDataType )
{
	// 値が範囲外のデータ形式かどうか調べる
	if( SoundDataType >= DX_SOUNDDATATYPE_MEMNOPRESS && SoundDataType <= DX_SOUNDDATATYPE_FILE )
	{
		// 現在 DX_SOUNDDATATYPE_MEMNOPRESS_PLUS は非対応
		if( SoundDataType == DX_SOUNDDATATYPE_MEMNOPRESS_PLUS )
		{
			SoundSysData.CreateSoundDataType = DX_SOUNDDATATYPE_MEMNOPRESS ;
		}
		else
		{
			SoundSysData.CreateSoundDataType = SoundDataType ;
		}
	}
	else
	{
		return -1 ; 
	}

	// 終了
	return 0 ;
}

// 作成するサウンドのデータ形式を取得する( DX_SOUNDDATATYPE_MEMNOPRESS 等 )
extern	int NS_GetCreateSoundDataType( void )
{
	return SoundSysData.CreateSoundDataType ;
}

// 作成するサウンドハンドルのピッチレートを設定する( 単位はセント( 100.0fで半音、1200.0fで１オクターヴ )、プラスの値で音程が高く、マイナスの値で音程が低くなります )
extern	int NS_SetCreateSoundPitchRate( float Cents )
{
//	if( PitchRate < 0.25f || PitchRate > 2.0f )
//	{
//		return -1 ;
//	}

	if( Cents > -0.0000001 && Cents < 0.0000001f )
	{
		SoundSysData.CreateSoundPitchRateEnable = FALSE ;
		SoundSysData.CreateSoundPitchRate = 1.0f ;
		SoundSysData.CreateSoundPitchRate_Cents = 0.0f ;
	}
	else
	{
		SoundSysData.CreateSoundPitchRateEnable = TRUE ;
		SoundSysData.CreateSoundPitchRate = _POW( 2.0f, Cents / 1200.0f ) ;
		SoundSysData.CreateSoundPitchRate_Cents = Cents ;
	}

	// 正常終了
	return 0 ;
}

// 作成するサウンドハンドルのピッチレートを取得する( 単位はセント( 100.0fで半音、1200.0fで１オクターヴ )、プラスの値で音程が高く、マイナスの値で音程が低くなります )
extern	float NS_GetCreateSoundPitchRate( void )
{
	return SoundSysData.CreateSoundPitchRate_Cents ;
}

// 作成するサウンドハンドルのタイムストレッチ( 音程を変えずに音の長さを変更する )レートを設定する( 単位は倍率、2.0f で音の長さが２倍に、0.5f で音の長さが半分になります )
extern int NS_SetCreateSoundTimeStretchRate( float Rate )
{
	if( Rate > 0.9999999f && Rate < 1.0000001f )
	{
		SoundSysData.CreateSoundTimeStretchRateEnable = FALSE ;
		SoundSysData.CreateSoundTimeStretchRate = 1.0f ;
	}
	else
	{
		SoundSysData.CreateSoundTimeStretchRateEnable = TRUE ;
		SoundSysData.CreateSoundTimeStretchRate = Rate ;
	}

	// 正常終了
	return 0 ;
}

// 作成するサウンドハンドルのタイムストレッチ( 音程を変えずに音の長さを変更する )レートを取得する( 単位は倍率、2.0f で音の長さが２倍に、0.5f で音の長さが半分になります )
extern float NS_GetCreateSoundTimeStretchRate( void )
{
	return SoundSysData.CreateSoundTimeStretchRate ;
}

// 作成するサウンドハンドルのループ範囲を設定する( ミリ秒単位 )
extern int NS_SetCreateSoundLoopAreaTimePos( int  LoopStartTime, int  LoopEndTime )
{
	// サンプル単位のパラメータは無効化
	SoundSysData.CreateSoundLoopStartSamplePosition = 0 ;
	SoundSysData.CreateSoundLoopEndSamplePosition   = 0 ;

	// スタート位置とエンド位置が同じか、スタート位置のほうがエンド位置より値が大きい場合はパラメータを無効化
	if( LoopStartTime >= LoopEndTime )
	{
		SoundSysData.CreateSoundLoopStartTimePosition = 0 ;
		SoundSysData.CreateSoundLoopEndTimePosition   = 0 ;
	}
	else
	{
		// 有効な値の場合のみ保存
		SoundSysData.CreateSoundLoopStartTimePosition = LoopStartTime ;
		SoundSysData.CreateSoundLoopEndTimePosition   = LoopEndTime ;
	}

	// 正常終了
	return 0 ;
}

// 作成するサウンドハンドルのループ範囲を取得する( ミリ秒単位 )
extern int NS_GetCreateSoundLoopAreaTimePos( int *LoopStartTime, int *LoopEndTime )
{
	if( LoopStartTime != NULL ) *LoopStartTime = SoundSysData.CreateSoundLoopStartTimePosition ;
	if( LoopEndTime   != NULL ) *LoopEndTime   = SoundSysData.CreateSoundLoopEndTimePosition ;

	// 正常終了
	return 0 ;
}

// 作成するサウンドハンドルのループ範囲を設定する( サンプル単位 )
extern int NS_SetCreateSoundLoopAreaSamplePos( int  LoopStartSamplePosition, int  LoopEndSamplePosition )
{
	// ミリ秒単位のパラメータは無効化
	SoundSysData.CreateSoundLoopStartTimePosition = 0 ;
	SoundSysData.CreateSoundLoopEndTimePosition   = 0 ;

	// スタート位置とエンド位置が同じか、スタート位置のほうがエンド位置より値が大きい場合はパラメータを無効化
	if( LoopStartSamplePosition >= LoopEndSamplePosition )
	{
		SoundSysData.CreateSoundLoopStartSamplePosition = 0 ;
		SoundSysData.CreateSoundLoopEndSamplePosition   = 0 ;
	}
	else
	{
		// 有効な値の場合のみ保存
		SoundSysData.CreateSoundLoopStartSamplePosition = LoopStartSamplePosition ;
		SoundSysData.CreateSoundLoopEndSamplePosition   = LoopEndSamplePosition ;
	}

	// 正常終了
	return 0 ;
}

// 作成するサウンドハンドルのループ範囲を取得する( サンプル単位 )
extern int NS_GetCreateSoundLoopAreaSamplePos( int *LoopStartSamplePosition, int *LoopEndSamplePosition )
{
	if( LoopStartSamplePosition != NULL ) *LoopStartSamplePosition = SoundSysData.CreateSoundLoopStartSamplePosition ;
	if( LoopEndSamplePosition   != NULL ) *LoopEndSamplePosition   = SoundSysData.CreateSoundLoopEndSamplePosition ;

	// 正常終了
	return 0 ;
}

// LoadSoundMem などで読み込むサウンドデータにループ範囲情報があっても無視するかどうかを設定する( TRUE:無視する  FALSE:無視しない( デフォルト ) )
extern int NS_SetCreateSoundIgnoreLoopAreaInfo( int IgnoreFlag )
{
	SoundSysData.CreateSoundIgnoreLoopAreaInfo = IgnoreFlag ;

	// 終了
	return 0 ;
}

// LoadSoundMem などで読み込むサウンドデータにループ範囲情報があっても無視するかどうかを取得する( TRUE:無視する  FALSE:無視しない( デフォルト ) )
extern int NS_GetCreateSoundIgnoreLoopAreaInfo( void )
{
	return SoundSysData.CreateSoundIgnoreLoopAreaInfo ;
}

// 使用しないサウンドデータ読み込み処理のマスクを設定する
extern	int NS_SetDisableReadSoundFunctionMask( int Mask )
{
	SoundSysData.DisableReadSoundFunctionMask = Mask ;

	// 終了
	return 0 ;
}

// 使用しないサウンドデータ読み込み処理のマスクを取得する
extern	int	NS_GetDisableReadSoundFunctionMask( void )
{
	return SoundSysData.DisableReadSoundFunctionMask ;
}

// サウンドキャプチャを前提とした動作をするかどうかを設定する
extern	int NS_SetEnableSoundCaptureFlag( int Flag )
{
	// フラグが同じ場合は何もしない
	if( SoundSysData.EnableSoundCaptureFlag == Flag ) return 0 ;

	// 全てのサウンドハンドルを削除する
	NS_InitSoundMem() ;
	
	// フラグをセットする
	SoundSysData.EnableSoundCaptureFlag = Flag ;
	
	// 終了
	return 0 ;
}

// ChangeVolumeSoundMem, ChangeNextPlayVolumeSoundMem, ChangeMovieVolumeToGraph の音量計算式を Ver3.10c以前のものを使用するかどうかを設定する( TRUE:Ver3.10c以前の計算式を使用  FALSE:3.10d以降の計算式を使用( デフォルト ) )
extern int NS_SetUseOldVolumeCalcFlag( int Flag )
{
	// フラグを保存する
	SoundSysData.OldVolumeTypeFlag = Flag ;
	
	// 終了
	return 0 ;
}

// 次に作成するサウンドを３Ｄサウンド用にするかどうかを設定する( TRUE:３Ｄサウンド用にする  FALSE:３Ｄサウンド用にしない( デフォルト ) )
extern int NS_SetCreate3DSoundFlag( int Flag )
{
	// フラグを保存する
	SoundSysData.Create3DSoundFlag = Flag != FALSE ? TRUE : FALSE ;

	// 終了
	return 0 ;
}

// ３Ｄ空間の１メートルに当る距離を設定する( デフォルト:1.0f )
extern int NS_Set3DSoundOneMetre( float Distance )
{
	if( CheckSoundSystem_Initialize_PF() ) return -1 ;

	if( Distance <= 0.0f )
	{
		SoundSysData._3DSoundOneMetreEnable = FALSE ;
		SoundSysData._3DSoundOneMetre = 1.0f ;
	}
	else
	{
		SoundSysData._3DSoundOneMetreEnable = TRUE ;
		SoundSysData._3DSoundOneMetre = Distance ;
	}

	// 終了
	return 0 ;
}

// ３Ｄサウンドのリスナーの位置とリスナーの前方位置を設定する( リスナーの上方向位置はＹ軸固定 )
extern int NS_Set3DSoundListenerPosAndFrontPos_UpVecY( VECTOR Position, VECTOR FrontPosition )
{
	VECTOR UpVec = { 0.0f, 1.0f, 0.0f } ;

	return NS_Set3DSoundListenerPosAndFrontPosAndUpVec( Position, FrontPosition, UpVec ) ;
}

// ３Ｄサウンドのリスナーの位置とリスナーの前方位置とリスナーの上方向位置を設定する
extern int NS_Set3DSoundListenerPosAndFrontPosAndUpVec( VECTOR Position, VECTOR FrontPosition, VECTOR UpVector )
{
	VECTOR SideVec ;
	VECTOR DirVec ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	VectorSub( &DirVec, &FrontPosition, &Position ) ;
	VectorNormalize( &DirVec, &DirVec ) ;

	VectorScale( &Position, &Position, 1.0f / SoundSysData._3DSoundOneMetre ) ;

	VectorOuterProduct( &SideVec, &DirVec,  &UpVector ) ;
	VectorOuterProduct( &UpVector, &SideVec, &DirVec ) ;
	VectorNormalize( &UpVector, &UpVector ) ;
	VectorNormalize( &SideVec, &SideVec ) ;

	// 値がほとんど変化しない場合場合は何もしない
	if( _FABS( SoundSysData.ListenerInfo.Position.x       - Position.x ) < 0.001f &&
		_FABS( SoundSysData.ListenerInfo.Position.y       - Position.y ) < 0.001f &&
		_FABS( SoundSysData.ListenerInfo.Position.z       - Position.z ) < 0.001f &&
		_FABS( SoundSysData.ListenerInfo.FrontDirection.x - DirVec.x   ) < 0.001f &&
		_FABS( SoundSysData.ListenerInfo.FrontDirection.y - DirVec.y   ) < 0.001f &&
		_FABS( SoundSysData.ListenerInfo.FrontDirection.z - DirVec.z   ) < 0.001f &&
		_FABS( SoundSysData.ListenerInfo.UpDirection.x    - UpVector.x ) < 0.001f &&
		_FABS( SoundSysData.ListenerInfo.UpDirection.y    - UpVector.y ) < 0.001f &&
		_FABS( SoundSysData.ListenerInfo.UpDirection.z    - UpVector.z ) < 0.001f )
		return 0 ;

	SoundSysData.ListenerInfo.Position       = Position ;
	SoundSysData.ListenerInfo.FrontDirection = DirVec ;
	SoundSysData.ListenerInfo.UpDirection    = UpVector ;
	SoundSysData.ListenerSideDirection       = SideVec ;

	// 環境依存処理
	if( Set3DSoundListenerPosAndFrontPosAndUpVec_PF( Position, FrontPosition, UpVector ) < 0 )
	{
		return -1 ;
	}

	// 再生中の３Ｄサウンドのパラメータを更新する
	Refresh3DSoundParamAll() ;

	// 終了
	return 0 ;
}

// ３Ｄサウンドのリスナーの移動速度を設定する
extern int NS_Set3DSoundListenerVelocity( VECTOR Velocity )
{
	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	VectorScale( &Velocity, &Velocity, 1.0f / SoundSysData._3DSoundOneMetre ) ;

	// 値がほとんど変化しない場合場合は何もしない
	if( _FABS( SoundSysData.ListenerInfo.Velocity.x - Velocity.x ) < 0.001f &&
		_FABS( SoundSysData.ListenerInfo.Velocity.y - Velocity.y ) < 0.001f &&
		_FABS( SoundSysData.ListenerInfo.Velocity.z - Velocity.z ) < 0.001f )
	{
		return 0 ;
	}

	// 環境依存処理
	if( Set3DSoundListenerVelocity_PF( Velocity ) < 0 )
	{
		return -1 ;
	}

	SoundSysData.ListenerInfo.Velocity = Velocity ;


	// 再生中の３Ｄサウンドのパラメータを更新する
	Refresh3DSoundParamAll() ;

	// 終了
	return 0 ;
}

// ３Ｄサウンドのリスナーの可聴角度範囲を設定する
extern int NS_Set3DSoundListenerConeAngle( float InnerAngle, float OuterAngle )
{
	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	if( OuterAngle < 0.0f )
	{
		OuterAngle = 0.0f ;
	}
	else
	if( OuterAngle > DX_PI_F * 2.0f )
	{
		OuterAngle = DX_PI_F * 2.0f ;
	}

	if( InnerAngle < 0.0f )
	{
		InnerAngle = 0.0f ;
	}
	else
	if( InnerAngle > DX_PI_F * 2.0f )
	{
		InnerAngle = DX_PI_F * 2.0f ;
	}

	if( OuterAngle < InnerAngle )
	{
		float Temp ;
		Temp = InnerAngle ;
		InnerAngle = OuterAngle ;
		OuterAngle = Temp ;
	}

	// 値がほとんど変化しない場合場合は何もしない
	if( _FABS( SoundSysData.ListenerInfo.InnerAngle - InnerAngle ) < 0.001f &&
		_FABS( SoundSysData.ListenerInfo.OuterAngle - OuterAngle ) < 0.001f )
	{
		return 0 ;
	}

	SoundSysData.ListenerInfo.InnerAngle = InnerAngle ;
	SoundSysData.ListenerInfo.OuterAngle = OuterAngle ;

	// 環境依存処理
	if( Set3DSoundListenerConeAngle_PF( InnerAngle, OuterAngle ) < 0 )
	{
		return -1 ;
	}

	// 再生中の３Ｄサウンドのパラメータを更新する
	Refresh3DSoundParamAll() ;

	// 終了
	return 0 ;
}

// ３Ｄサウンドのリスナーの可聴角度範囲の音量倍率を設定する
extern int NS_Set3DSoundListenerConeVolume( float InnerAngleVolume, float OuterAngleVolume )
{
	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// 値がほとんど変化しない場合場合は何もしない
	if( _FABS( SoundSysData.ListenerInfo.InnerVolume - InnerAngleVolume ) < 0.001f &&
		_FABS( SoundSysData.ListenerInfo.OuterVolume - OuterAngleVolume ) < 0.001f )
	{
		return 0 ;
	}

	SoundSysData.ListenerInfo.InnerVolume = InnerAngleVolume ;
	SoundSysData.ListenerInfo.OuterVolume = OuterAngleVolume ;

	// 環境依存処理
	if( Set3DSoundListenerConeVolume_PF( InnerAngleVolume, OuterAngleVolume ) < 0 )
	{
		return -1 ;
	}

	// 再生中の３Ｄサウンドのパラメータを更新する
	Refresh3DSoundParamAll() ;

	// 終了
	return 0 ;
}










// 補助系関数

// サイズ補正
static	int AdjustSoundDataBlock( int Length, SOUND * Sound ) 
{
	return Length / Sound->BufferFormat.nBlockAlign * Sound->BufferFormat.nBlockAlign  ;
}


// 波形データ用
extern WAVEDATA *AllocWaveData( int Size, int UseDoubleSizeBuffer )
{
	WAVEDATA *Data ;
	unsigned int AllocSize ;

	AllocSize = sizeof( WAVEDATA ) + 16 + Size ;
	if( UseDoubleSizeBuffer )
	{
		AllocSize += Size * 2 + 32 ;
	}

	Data = ( WAVEDATA * )DXALLOC( AllocSize ) ;
	if( Data == NULL )
		return NULL ;
	Data->Buffer = ( void * )( ( ( DWORD_PTR )Data + sizeof( WAVEDATA ) + 15 ) / 16 * 16 ) ;
	Data->Bytes = Size ;
	Data->RefCount = 1 ;

	if( UseDoubleSizeBuffer )
	{
		Data->DoubleSizeBuffer = ( void * )( ( ( DWORD_PTR )Data->Buffer + Size + 15 ) / 16 * 16 ) ;
	}
	else
	{
		Data->DoubleSizeBuffer = NULL ;
	}

	return Data ;
}

extern int ReleaseWaveData( WAVEDATA *Data )
{
	Data->RefCount -- ;
	if( Data->RefCount > 0 )
		return 0 ;

	if( Data->RefCount < 0 )
		return -1 ;

	DXFREE( Data ) ;

	return 0 ;
}

extern WAVEDATA *DuplicateWaveData( WAVEDATA *Data )
{
	Data->RefCount ++ ;
	return Data ;
}




//サウンドバッファ用
extern int SoundBuffer_Initialize( SOUNDBUFFER *Buffer, DWORD Bytes, WAVEFORMATEX *Format, SOUNDBUFFER *Src, int UseGetCurrentPosition, int IsStream, int Is3DSound )
{
	int i ;

	// 初期化済みの場合はエラー
	if( Buffer->Valid )
	{
		return -1 ;
	}

	Buffer->Wave					= NULL ;
	Buffer->StopTimeState			= 0 ;
	Buffer->StopTime				= 0 ;
	Buffer->Is3DSound				= Is3DSound ;
	Buffer->UseGetCurrentPosition	= UseGetCurrentPosition ;
	Buffer->IsStream				= IsStream ;

	if( SoundSysData.EnableSoundCaptureFlag )
	{
		if( Src != NULL )
		{
			Buffer->Wave = DuplicateWaveData( Src->Wave ) ;
		}
		else
		{
			Buffer->Wave = AllocWaveData( ( int )Bytes ) ;
		}
		if( Buffer->Wave == NULL )
			goto ERR ;
	}
	else
	{
		// 環境依存処理
		if( SoundBuffer_Initialize_Timing0_PF( Buffer, Bytes, Format, Src, Is3DSound ) < 0 )
		{
			goto ERR ;
		}
	}

	Buffer->State	= FALSE ;
	Buffer->Pos		= 0 ;
	Buffer->CompPos	= 0 ;
	Buffer->Loop	= FALSE ;

	if( Src != NULL )
	{
		Buffer->Pan = Src->Pan ;
		for( i = 0 ; i < SOUNDBUFFER_MAX_CHANNEL_NUM ; i ++ )
		{
			Buffer->Volume[ i ] = Src->Volume[ i ] ;
		}
		Buffer->Frequency = Src->Frequency ;
		Buffer->SampleNum = Src->SampleNum ;
		Buffer->Format    = Src->Format ;
	}
	else
	{
		Buffer->Pan = 0 ;
		for( i = 0 ; i < SOUNDBUFFER_MAX_CHANNEL_NUM ; i ++ )
		{
			Buffer->Volume[ i ] = 0 ;
		}
		Buffer->Frequency	= -1 ;
		Buffer->SampleNum	= ( int )( Bytes / Format->nBlockAlign ) ;
		Buffer->Format		= *Format ;
	}

	// ３Ｄサウンドの場合は３Ｄサウンドパラメータを初期化
	if( Is3DSound != FALSE )
	{
		// 最初にセットアップを行うために変更したフラグを立てる
		Buffer->EmitterDataChangeFlag = TRUE ;

		if( SoundSysData.EnableSoundCaptureFlag == FALSE )
		{
			// 環境依存処理
			if( SoundBuffer_Initialize_Timing1_PF( Buffer, Src, Is3DSound ) < 0 )
			{
				goto ERR ;
			}
		}

		// エミッターの基本的な情報をセットする
		if( Src != NULL )
		{
			Buffer->EmitterInfo					= Src->EmitterInfo ;
			Buffer->EmitterRadius				= Src->EmitterRadius ;
			Buffer->EmitterInnerRadius			= Src->EmitterInnerRadius ;
		}
		else
		{
			Buffer->EmitterInfo.Position		= VGet( 0.0f, 0.0f, 0.0f ) ;
			Buffer->EmitterInfo.FrontDirection	= VGet( 0.0f, 0.0f, 1.0f ) ;
			Buffer->EmitterInfo.UpDirection		= VGet( 0.0f, 1.0f, 0.0f ) ;
			Buffer->EmitterInfo.Velocity		= VGet( 0.0f, 0.0f, 0.0f ) ;
		}
	}
	
	Buffer->Valid = TRUE ;
	
	return 0 ;

ERR :
	if( Buffer->Wave != NULL )
	{
		ReleaseWaveData( Buffer->Wave ) ;
		Buffer->Wave = NULL ;
	}

	return -1 ;
}

extern int SoundBuffer_Duplicate( SOUNDBUFFER *Buffer, SOUNDBUFFER *Src, int Is3DSound )
{
	return SoundBuffer_Initialize( Buffer, 0, NULL, Src, Src->UseGetCurrentPosition, Src->IsStream, Is3DSound ) ;
}


extern int SoundBuffer_Terminate(          SOUNDBUFFER *Buffer )
{
	if( Buffer->Valid == FALSE )
	{
		return -1 ;
	}

	// 再生状態だった場合はストップする
	if( Buffer->State == TRUE )
	{
		SoundBuffer_Stop( Buffer ) ;
	}

	// 環境依存処理
	SoundBuffer_Terminate_PF( Buffer ) ;

	if( Buffer->Wave != NULL )
	{
		ReleaseWaveData( Buffer->Wave ) ;
		Buffer->Wave = NULL ;
	}
	
	Buffer->Valid = FALSE ;
	
	return 0 ;
}

extern int SoundBuffer_CheckEnable(         SOUNDBUFFER *Buffer )
{
	if( Buffer->Valid == FALSE )
	{
		return FALSE ;
	}

	return SoundBuffer_CheckEnable_PF( Buffer ) ;
}

extern int SoundBuffer_Play(               SOUNDBUFFER *Buffer, int Loop )
{
	if( Buffer->Valid == FALSE ) return -1 ;
	
	Buffer->Loop = Loop ;
	if( SoundSysData.EnableSoundCaptureFlag )
	{
		Buffer->State = TRUE ;
	}
	else
	{
		// 環境依存処理
		if( SoundBuffer_Play_PF( Buffer, Loop ) < 0 )
		{
			return -1 ;
		}
	}
	
	return 0 ;
}

extern int SoundBuffer_Stop(               SOUNDBUFFER *Buffer, int EffectStop )
{
	if( Buffer->Valid == FALSE ) return -1 ;
	
	if( SoundSysData.EnableSoundCaptureFlag )
	{
		Buffer->State = FALSE ;
	}
	else
	{
		// 環境依存処理
		if( SoundBuffer_Stop_PF( Buffer, EffectStop ) < 0 )
		{
			return -1 ;
		}
	}

	return 0 ;
}

extern int SoundBuffer_CheckPlay(          SOUNDBUFFER *Buffer )
{
	if( Buffer->Valid == FALSE ) return -1 ;
	
	if( SoundSysData.EnableSoundCaptureFlag )
	{
		return Buffer->State ;
	}
	else
	{
		// 環境依存処理
		return SoundBuffer_CheckPlay_PF( Buffer ) ;
	}
}

extern int SoundBuffer_Lock(               SOUNDBUFFER *Buffer, DWORD WritePos , DWORD WriteSize, void **LockPos1, DWORD *LockSize1, void **LockPos2, DWORD *LockSize2 )
{
	if( Buffer->Valid == FALSE )
	{
		return -1 ;
	}
	
	if( SoundSysData.EnableSoundCaptureFlag )
	{
DEFAULTPROCESS :
		DWORD pos, sample, sample1 ;

		pos    = WritePos  / Buffer->Format.nBlockAlign ;
		sample = WriteSize / Buffer->Format.nBlockAlign ;
		if( pos + sample > ( DWORD )Buffer->SampleNum )
		{
			sample1 = Buffer->SampleNum - pos ;
			*LockPos1  = (BYTE *)Buffer->Wave->Buffer + WritePos ;
			*LockSize1 = sample1 * Buffer->Format.nBlockAlign ;
			*LockPos2  = (BYTE *)Buffer->Wave->Buffer ;
			*LockSize2 = ( sample - sample1 ) * Buffer->Format.nBlockAlign ;
		}
		else
		{
			*LockPos1  = (BYTE *)Buffer->Wave->Buffer + WritePos ;
			*LockSize1 = WriteSize ;
			*LockPos2  = NULL ;
			*LockSize2 = 0 ;
		}
	}
	else
	{
		int Result ;

		// 環境依存処理
		Result = SoundBuffer_Lock_PF( Buffer, WritePos, WriteSize, LockPos1, LockSize1, LockPos2, LockSize2 ) ;
		if( Result == 2 )
		{
			goto DEFAULTPROCESS ;
		}
		if( Result == -1 )
		{
			return -1 ;
		}
	}
	
	return 0 ;
}

extern int SoundBuffer_Unlock(             SOUNDBUFFER *Buffer, void *LockPos1, DWORD LockSize1, void *LockPos2, DWORD LockSize2 )
{
	if( Buffer->Valid == FALSE ) return -1 ;
	
	if( SoundSysData.EnableSoundCaptureFlag )
	{
		// 特に何もしない
	}
	else
	{
		// 環境依存処理
		if( SoundBuffer_Unlock_PF( Buffer, LockPos1, LockSize1, LockPos2, LockSize2 ) < 0 )
		{
			return -1 ;
		}
	}
	
	return 0 ;
}


extern int SoundBuffer_SetFrequency(       SOUNDBUFFER *Buffer, DWORD Frequency )
{
	if( Buffer->Valid == FALSE ) return -1 ;
	
	if( Frequency == 0 ) Buffer->Frequency = -1 ;
	else                 Buffer->Frequency = ( int )Frequency ;

	if( SoundSysData.EnableSoundCaptureFlag )
	{
	}
	else
	{
		// 環境依存処理
		if( SoundBuffer_SetFrequency_PF( Buffer, Frequency ) < 0 )
		{
			return -1 ;
		}
	}

	return 0 ;
}

extern int SoundBuffer_GetFrequency(             SOUNDBUFFER *Buffer, DWORD * Frequency )
{
	if( Buffer->Valid == FALSE ) return -1 ;
	
	if( SoundSysData.EnableSoundCaptureFlag )
	{
DEFAULTPROCESS :
		if( Buffer->Frequency < 0 )
		{
			*Frequency = Buffer->Format.nSamplesPerSec ;
		}
		else
		{
			*Frequency = ( DWORD )Buffer->Frequency ;
		}
	}
	else
	{
		int Result ;

		Result = SoundBuffer_GetFrequency_PF( Buffer, Frequency ) ;
		if( Result == 2 )
		{
			goto DEFAULTPROCESS ;
		}
		if( Result < 0 )
		{
			return -1 ;
		}
	}
	
	return 0 ;
}

extern int SoundBuffer_SetPan(             SOUNDBUFFER *Buffer, LONG Pan )
{
	if( Buffer->Valid == FALSE ) return -1 ;

	if( Pan < -10000 )
	{
		Pan = -10000 ;
	}
	else
	if( Pan > 10000 ) 
	{
		Pan = 10000 ;
	}
	Buffer->Pan = Pan ;

	return SoundBuffer_RefreshVolume( Buffer ) ;
}

extern int SoundBuffer_GetPan(             SOUNDBUFFER *Buffer, LPLONG Pan )
{
	if( Buffer->Valid == FALSE ) return -1 ;

	if( Pan != NULL )
	{
		*Pan = Buffer->Pan ;
	}

	return 0 ;
}

extern int SoundBuffer_RefreshVolume( SOUNDBUFFER *Buffer )
{
	if( Buffer->Valid == FALSE ) return -1 ;

	if( SoundSysData.EnableSoundCaptureFlag )
	{
		if( Buffer->Volume[ 0 ] <= -10000 )
		{
			Buffer->CalcVolume = 0 ;
		}
		else
		if( Buffer->Volume[ 0 ] >= 0 )
		{
			Buffer->CalcVolume = 256 ;
		}
		else
		{
			const double Min = 0.0000000001 ;
			const double Max = 1.0 ;
			
			if( SoundSysData.OldVolumeTypeFlag )
			{
				Buffer->CalcVolume = _DTOL( ( _POW( (float)10, Buffer->Volume[ 0 ] / 10.0f / 100.0f ) / ( Max - Min ) ) * 256 ) ;
			}
			else
			{
				Buffer->CalcVolume = _DTOL( ( _POW( (float)10, Buffer->Volume[ 0 ] / 50.0f / 100.0f ) / ( Max - Min ) ) * 256 ) ;
			}
		}

		if( Buffer->Pan == -10000 )
		{
			Buffer->CalcPan = -256 ;
		}
		else
		if( Buffer->Pan == 10000 ) 
		{
			Buffer->CalcPan = 256 ;
		}
		else
		if( Buffer->Pan == 0 )
		{
			Buffer->CalcPan = 0 ;
		}
		else
		{
			const double Min = 0.0000000001 ;
			const double Max = 1.0 ;
			int temp ;
			
			if( SoundSysData.OldVolumeTypeFlag )
			{
				temp = _DTOL( ( _POW( (float)10, -( _ABS( Buffer->Pan ) ) / 10.0f / 100.0f ) / ( Max - Min ) ) * 256 ) ;
			}
			else
			{
				temp = _DTOL( ( _POW( (float)10, -( _ABS( Buffer->Pan ) ) / 50.0f / 100.0f ) / ( Max - Min ) ) * 256 ) ;
			}
			Buffer->CalcPan = Buffer->Pan < 0 ? -temp : temp ;
		}
	}
	else
	{
		// 環境依存処理
		if( SoundBuffer_RefreshVolume_PF( Buffer ) < 0 )
		{
			return -1 ;
		}
	}
	
	return 0 ;
}

extern int SoundBuffer_SetVolumeAll( SOUNDBUFFER *Buffer, LONG Volume )
{
	int i ;

	if( Buffer->Valid == FALSE ) return -1 ;

	for( i = 0 ; i < SOUNDBUFFER_MAX_CHANNEL_NUM ; i ++ )
	{
		Buffer->Volume[ i ] = Volume ;
	}

	return SoundBuffer_RefreshVolume( Buffer ) ;
}

extern int SoundBuffer_SetVolume( SOUNDBUFFER *Buffer, int Channel, LONG Volume )
{
	if( Buffer->Valid == FALSE ) return -1 ;

	if( Channel < 0 || Channel >= SOUNDBUFFER_MAX_CHANNEL_NUM ) return -1 ;

	Buffer->Volume[ Channel ] = Volume ;

	return SoundBuffer_RefreshVolume( Buffer ) ;
}

extern int SoundBuffer_GetVolume( SOUNDBUFFER *Buffer, int Channel, LPLONG Volume )
{
	if( Buffer->Valid == FALSE ) return -1 ;

	if( Channel < 0 || Channel >= SOUNDBUFFER_MAX_CHANNEL_NUM ) return -1 ;

	if( Volume )
	{
		*Volume = Buffer->Volume[ Channel ] ;
	}

	return 0 ;
}

extern int SoundBuffer_GetCurrentPosition( SOUNDBUFFER *Buffer, DWORD *PlayPos, DWORD *WritePos )
{
	if( Buffer->Valid == FALSE ) return -1 ;
	
	if( SoundSysData.EnableSoundCaptureFlag )
	{
DEFAULTPROCESS :
		if( PlayPos  ) *PlayPos  = (DWORD)Buffer->Pos * Buffer->Format.nBlockAlign ;
		if( WritePos ) *WritePos = (DWORD)Buffer->Pos * Buffer->Format.nBlockAlign ;
	}
	else
	{
		int Result ;

		// 環境依存処理
		Result = SoundBuffer_GetCurrentPosition_PF( Buffer, PlayPos, WritePos ) ;
		if( Result == 2 )
		{
			goto DEFAULTPROCESS ;
		}

		if( Result < 0 )
		{
			return -1 ;
		}
	}
	
	return 0 ;
}

extern int SoundBuffer_SetCurrentPosition( SOUNDBUFFER *Buffer, DWORD NewPos )
{
	if( Buffer->Valid == FALSE ) return -1 ;

	if( SoundSysData.EnableSoundCaptureFlag )
	{
		Buffer->Pos     = ( int )( NewPos / Buffer->Format.nBlockAlign ) ;
		Buffer->CompPos = Buffer->Pos ;
	}
	else
	{
		// 環境依存処理
		if( SoundBuffer_SetCurrentPosition_PF( Buffer, NewPos ) < 0 )
		{
			return -1 ;
		}
	}
	
	return 0 ;
}

extern int SoundBuffer_CycleProcess( SOUNDBUFFER *Buffer )
{
	if( SoundSysData.EnableSoundCaptureFlag )
	{
		return -1 ;
	}
	else
	{
		// 環境依存処理
		return SoundBuffer_CycleProcess_PF( Buffer ) ;
	}
}


#define DESTADD	\
{									\
	if( vol != 255 )				\
	{								\
		d1 = ( d1 * vol ) >> 8 ;	\
		d2 = ( d2 * vol ) >> 8 ;	\
	}								\
	if( pan != 0 )					\
	{								\
		if( pan > 0 ) d2 = ( d2 * ( 256 -   pan  ) ) >> 8 ;		\
		else          d1 = ( d1 * ( 256 - (-pan) ) ) >> 8 ;		\
	}								\
									\
		 if( DestBuf[0] + d1 >  32767 ) DestBuf[0] =  32767 ;				\
	else if( DestBuf[0] + d1 < -32768 ) DestBuf[0] = -32768 ;				\
	else                                DestBuf[0] += ( short )d1 ; 		\
									\
		 if( DestBuf[1] + d2 >  32767 ) DestBuf[1] =  32767 ;				\
	else if( DestBuf[1] + d2 < -32768 ) DestBuf[1] = -32768 ;				\
	else                                DestBuf[1] += ( short )d2 ; 		\
}

#define CNV( S )		((int)((S) * 65535 / 255) - 32768)

extern int SoundBuffer_FrameProcess(       SOUNDBUFFER *Buffer, int Sample, short *DestBuf )
{
	if( Buffer->Valid == FALSE ) return -1 ;

	if( SoundSysData.EnableSoundCaptureFlag )
	{
		int i, pos, d1, d2, b, ch, rate, bit, vol, pan ;
		short *s ;
		BYTE *sb ;
		
		ch   = Buffer->Format.nChannels ;
		rate = ( int )Buffer->Format.nSamplesPerSec ;
		bit  = Buffer->Format.wBitsPerSample ;
		vol  = Buffer->Volume[ 0 ] ;
		pan  = Buffer->Pan ;
		s    = (short *)( (BYTE *)Buffer->Wave->Buffer + Buffer->Pos * Buffer->Format.nBlockAlign ) ;
		sb   = (BYTE *)s ;

		if( DestBuf != NULL &&
			( rate == 44100 || rate == 22050 ) &&
			( bit  == 16    || bit  == 8     ) &&
			( ch   == 2     || ch   == 1     ) )
		{
			if( rate == 44100 )
			{
				pos = Buffer->Pos ;
				for( i = 0 ; i < Sample ; i ++, DestBuf += 2 )
				{
					if( bit == 16 )
					{
						if( ch == 2 ){      d1 = s[0]; d2 = s[1]; s += 2 ; }
						else         { d2 = d1 = s[0];            s += 1 ; }
					}
					else
					{
						if( ch == 2 ){      d1 = CNV(sb[0]); d2 = CNV(sb[1]); sb += 2 ; }
						else         { d2 = d1 = CNV(sb[0]);                  sb += 1 ; }
					}
					
					DESTADD
					pos ++ ;
					if( pos >= Buffer->SampleNum )
					{
						if( Buffer->Loop == FALSE ) break ;
						
						s = (short *)Buffer->Wave->Buffer ;
						sb = (BYTE *)s ;
						pos = 0 ;
					}
				}
			}
			else
			if( rate == 22050 )
			{
				pos = Buffer->Pos ;
				b = Buffer->Pos & 1;
				for( i = 0 ; i < Sample ; i ++, DestBuf += 2 )
				{
					if( b )
					{
						if( pos + 1 >= Buffer->SampleNum )
						{
							if( Buffer->Loop == FALSE )
							{
								if( bit == 16 )
								{
									if( ch == 2 )
									{
										d1=s[0]+(s[0]-s[-2])/2;
										d2=s[1]+(s[1]-s[-3])/2;
									}
									else
									{
										d2 = d1=s[0]+(s[0]-s[-1])/2;
									}
								}
								else
								{
									if( ch == 2 )
									{
										d1=CNV(sb[0])+(CNV(sb[0])-CNV(sb[-2]))/2;
										d2=CNV(sb[1])+(CNV(sb[1])-CNV(sb[-3]))/2;
									}
									else
									{
										d2 = d1=CNV(sb[0])+(CNV(sb[0])-CNV(sb[-1]))/2;
									}
								}
								break;
							}
							else
							{
								if( bit == 16 )
								{
									if( ch == 2 )
									{
										d1 = s[0] ;
										d2 = s[1] ;
										s = (short *)Buffer->Wave->Buffer ;
										d1=d1+(s[0]-d1)/2;
										d2=d2+(s[1]-d2)/2;
									}
									else
									{
										d1 = s[0] ;
										s = (short *)Buffer->Wave->Buffer ;
										d2 = d1=d1+(s[0]-d1)/2;
									}
								}
								else
								{
									if( ch == 2 )
									{
										d1 = CNV(sb[0]) ;
										d2 = CNV(sb[1]) ;
										sb = (BYTE *)Buffer->Wave->Buffer ;
										d1=d1+(CNV(sb[0])-d1)/2;
										d2=d2+(CNV(sb[1])-d2)/2;
									}
									else
									{
										d1 = CNV(sb[0]) ;
										sb = (BYTE *)Buffer->Wave->Buffer ;
										d2 = d1=d1+(CNV(sb[0])-d1)/2;
									}
								}

								pos = 0 ;
								b = 0;
							}
						}
						else
						{
							if( bit == 16 )
							{
								if( ch == 2 )
								{
									d1=s[0]+(s[2]-s[0])/2;
									d2=s[1]+(s[3]-s[1])/2;
									s += 2 ;
								}
								else
								{
									d2 = d1=s[0]+(s[1]-s[0])/2;
									s += 1 ;
								}
							}
							else
							{
								if( ch == 2 )
								{
									d1=CNV(sb[0]);d1+=(CNV(sb[2])-d1)/2;
									d2=CNV(sb[1]);d2+=(CNV(sb[3])-d2)/2;
									sb += 2 ;
								}
								else
								{
									d1=CNV(sb[0]);d1+=(CNV(sb[1])-d1)/2;
									d2 = d1;
									sb += 1 ;
								}
							}
							pos ++ ;
							b = b ? 0 : 1;
						}
						DESTADD
					}
					else
					{
						b = b ? 0 : 1;
						if( bit == 16 )
						{
							if( ch == 2 ){      d1 = s[0]; d2 = s[1]; }
							else         { d2 = d1 = s[0];            }
						}
						else
						{
							if( ch == 2 ){      d1 = CNV(sb[0]); d2 = CNV(sb[1]); }
							else         { d2 = d1 = CNV(sb[0]);	              }
						}
						DESTADD
					}
				}
				Sample >>= 1;
			}
		}

		if( Buffer->Pos + Sample >= Buffer->SampleNum )
		{
			if( Buffer->Loop == TRUE )
			{
				Buffer->Pos = ( Buffer->Pos + Sample ) - Buffer->SampleNum ;
			}
			else
			{
				Buffer->Pos = Buffer->SampleNum ;
				Buffer->State = FALSE ;
			}
		}
		else
		{
			Buffer->Pos += Sample ;
		}
		Buffer->CompPos = Buffer->Pos ;
	}
	else
	{
	}
	
	return 0 ;
}

extern int SoundBuffer_Set3DPosition( SOUNDBUFFER *Buffer, VECTOR *Position )
{
	// ３Ｄサウンドではない場合は何もしない
	if( Buffer->Is3DSound == FALSE )
		return -1 ;

	// 値がほぼ変化しない場合は何もしない
	if( _FABS( Buffer->EmitterInfo.Position.x - Position->x ) < 0.001f &&
		_FABS( Buffer->EmitterInfo.Position.y - Position->y ) < 0.001f &&
		_FABS( Buffer->EmitterInfo.Position.z - Position->z ) < 0.001f )
	{
		return 0 ;
	}

	// 値を保存
	Buffer->EmitterInfo.Position = *Position ;

	// 環境依存処理
	if( SoundBuffer_Set3DPosition_PF( Buffer, Position ) < 0 )
	{
		return -1 ;
	}

	// データが変更されたフラグを立てる
	Buffer->EmitterDataChangeFlag = TRUE ;

	// 再生中だった場合はパラメータを更新
	if( SoundBuffer_CheckPlay( Buffer ) )
	{
		SoundBuffer_Refresh3DSoundParam( Buffer ) ;
	}

	return 0 ;
}

extern int SoundBuffer_Set3DRadius( SOUNDBUFFER *Buffer, float Radius )
{
	// ３Ｄサウンドではない場合は何もしない
	if( Buffer->Is3DSound == FALSE )
		return -1 ;

	// 値がほぼ変化しない場合は何もしない
	if( _FABS( Buffer->EmitterRadius - Radius ) < 0.001f )
	{
		return 0 ;
	}

	// 聞こえる距離を保存
	Buffer->EmitterRadius = Radius ;

	// 環境依存処理
	if( SoundBuffer_Set3DRadius_PF( Buffer, Radius ) < 0 )
	{
		return -1 ;
	}

	// データが変更されたフラグを立てる
	Buffer->EmitterDataChangeFlag = TRUE ;

	// 再生中だった場合はパラメータを更新
	if( SoundBuffer_CheckPlay( Buffer ) )
	{
		SoundBuffer_Refresh3DSoundParam( Buffer ) ;
	}

	return 0 ;
}

extern int SoundBuffer_Set3DInnerRadius( SOUNDBUFFER *Buffer, float Radius )
{
	// ３Ｄサウンドではない場合は何もしない
	if( Buffer->Is3DSound == FALSE )
		return -1 ;

	// 値がほぼ変化しない場合は何もしない
	if( _FABS( Buffer->EmitterInnerRadius - Radius ) < 0.001f )
	{
		return 0 ;
	}

	// １００％の音量で音が聞こえる距離を保存
	Buffer->EmitterInnerRadius = Radius ;

	// 環境依存処理
	if( SoundBuffer_Set3DInnerRadius_PF( Buffer, Radius ) < 0 )
	{
		return -1 ;
	}

	// データが変更されたフラグを立てる
	Buffer->EmitterDataChangeFlag = TRUE ;

	// 再生中だった場合はパラメータを更新
	if( SoundBuffer_CheckPlay( Buffer ) )
	{
		SoundBuffer_Refresh3DSoundParam( Buffer ) ;
	}

	return 0 ;
}

extern int SoundBuffer_Set3DVelocity( SOUNDBUFFER *Buffer, VECTOR *Velocity )
{
	// ３Ｄサウンドではない場合は何もしない
	if( Buffer->Is3DSound == FALSE )
		return -1 ;

	// 値がほとんど変化しない場合場合は何もしない
	if( _FABS( Buffer->EmitterInfo.Velocity.x - Velocity->x ) < 0.001f &&
		_FABS( Buffer->EmitterInfo.Velocity.y - Velocity->y ) < 0.001f &&
		_FABS( Buffer->EmitterInfo.Velocity.z - Velocity->z ) < 0.001f )
	{
		return 0 ;
	}

	// 値を保存
	Buffer->EmitterInfo.Velocity = *Velocity ;

	// 環境依存処理
	if( SoundBuffer_Set3DVelocity_PF( Buffer, Velocity ) < 0 )
	{
		return -1 ;
	}

	// データが変更されたフラグを立てる
	Buffer->EmitterDataChangeFlag = TRUE ;

	// 再生中だった場合はパラメータを更新
	if( SoundBuffer_CheckPlay( Buffer ) )
	{
		SoundBuffer_Refresh3DSoundParam( Buffer ) ;
	}

	return 0 ;
}

extern int SoundBuffer_Set3DFrontPosition( SOUNDBUFFER *Buffer, VECTOR *FrontPosition, VECTOR *UpVector )
{
	VECTOR SideVec ;
	VECTOR DirVec ;
	VECTOR Position ;
	VECTOR UpVectorT ;

	// ３Ｄサウンドではない場合は何もしない
	if( Buffer->Is3DSound == FALSE )
		return -1 ;

	Position.x = Buffer->EmitterInfo.Position.x * SoundSysData._3DSoundOneMetre ;
	Position.y = Buffer->EmitterInfo.Position.y * SoundSysData._3DSoundOneMetre ;
	Position.z = Buffer->EmitterInfo.Position.z * SoundSysData._3DSoundOneMetre ;

	VectorSub( &DirVec, FrontPosition, &Position ) ;
	VectorNormalize( &DirVec, &DirVec ) ;

	VectorOuterProduct( &SideVec, &DirVec,  UpVector ) ;
	VectorOuterProduct( &UpVectorT, &SideVec, &DirVec ) ;
	VectorNormalize( &UpVectorT, &UpVectorT ) ;

	// 値がほとんど変化しない場合場合は何もしない
	if( _FABS( Buffer->EmitterInfo.FrontDirection.x - DirVec.x ) < 0.0001f &&
		_FABS( Buffer->EmitterInfo.FrontDirection.y - DirVec.y ) < 0.0001f &&
		_FABS( Buffer->EmitterInfo.FrontDirection.z - DirVec.z ) < 0.0001f &&
		_FABS( Buffer->EmitterInfo.UpDirection.x - UpVectorT.x ) < 0.0001f &&
		_FABS( Buffer->EmitterInfo.UpDirection.y - UpVectorT.y ) < 0.0001f &&
		_FABS( Buffer->EmitterInfo.UpDirection.z - UpVectorT.z ) < 0.0001f )
	{
		return 0 ;
	}

	// 値を保存
	Buffer->EmitterInfo.FrontDirection = DirVec ;
	Buffer->EmitterInfo.UpDirection = UpVectorT ;

	// 環境依存処理
	if( SoundBuffer_Set3DFrontPosition_PF( Buffer, FrontPosition, UpVector ) < 0 )
	{
		return -1 ;
	}

	// データが変更されたフラグを立てる
	Buffer->EmitterDataChangeFlag = TRUE ;

	// 再生中だった場合はパラメータを更新
	if( SoundBuffer_CheckPlay( Buffer ) )
	{
		SoundBuffer_Refresh3DSoundParam( Buffer ) ;
	}

	return 0 ;
}

extern int SoundBuffer_Set3DConeAngle( SOUNDBUFFER *Buffer, float InnerAngle, float OuterAngle )
{
	// ３Ｄサウンドではない場合は何もしない
	if( Buffer->Is3DSound == FALSE )
		return -1 ;

	// 値がほぼ変化しない場合は何もしない
	if( _FABS( Buffer->EmitterInfo.InnerAngle - InnerAngle ) < 0.001f &&
		_FABS( Buffer->EmitterInfo.OuterAngle - OuterAngle ) < 0.001f )
	{
		return 0 ;
	}

	Buffer->EmitterInfo.InnerAngle = InnerAngle ;
	Buffer->EmitterInfo.OuterAngle = OuterAngle ;

	// 環境依存処理
	if( SoundBuffer_Set3DConeAngle_PF( Buffer, InnerAngle, OuterAngle ) < 0 )
	{
		return -1 ;
	}

	// データが変更されたフラグを立てる
	Buffer->EmitterDataChangeFlag = TRUE ;

	// 再生中だった場合はパラメータを更新
	if( SoundBuffer_CheckPlay( Buffer ) )
	{
		SoundBuffer_Refresh3DSoundParam( Buffer ) ;
	}

	return 0 ;
}

extern int SoundBuffer_Set3DConeVolume( SOUNDBUFFER *Buffer, float InnerAngleVolume, float OuterAngleVolume )
{
	// ３Ｄサウンドではない場合は何もしない
	if( Buffer->Is3DSound == FALSE )
		return -1 ;

	// 値がほぼ変化しない場合は何もしない
	if( _FABS( Buffer->EmitterInfo.InnerVolume - InnerAngleVolume ) < 0.001f &&
		_FABS( Buffer->EmitterInfo.OuterVolume - OuterAngleVolume ) < 0.001f )
	{
		return 0 ;
	}

	Buffer->EmitterInfo.InnerVolume = InnerAngleVolume ;
	Buffer->EmitterInfo.OuterVolume = OuterAngleVolume ;

	// 環境依存処理
	if( SoundBuffer_Set3DConeVolume_PF( Buffer, InnerAngleVolume, OuterAngleVolume ) < 0 )
	{
		return -1 ;
	}

	// データが変更されたフラグを立てる
	Buffer->EmitterDataChangeFlag = TRUE ;

	// 再生中だった場合はパラメータを更新
	if( SoundBuffer_CheckPlay( Buffer ) )
	{
		SoundBuffer_Refresh3DSoundParam( Buffer ) ;
	}

	return 0 ;
}


extern int SoundBuffer_Refresh3DSoundParam( SOUNDBUFFER *Buffer, int AlwaysFlag )
{
	if( Buffer->Is3DSound == FALSE || Buffer->Valid == FALSE )
	{
		return -1 ;
	}

	// 必ず実行するフラグが倒れていて、データが変更されたフラグも倒れていたら何もしない
	if( AlwaysFlag == FALSE && Buffer->EmitterDataChangeFlag == FALSE )
	{
		return 0 ;
	}

	if( SoundSysData.EnableSoundCaptureFlag )
	{
	}
	else
	{
		// 環境依存処理
		if( SoundBuffer_Refresh3DSoundParam_PF( Buffer, AlwaysFlag ) < 0 )
		{
			return -1 ;
		}
	}

	// データが変更されたフラグを倒す
	Buffer->EmitterDataChangeFlag = FALSE ;

	// 終了
	return 0 ;
}

extern int SoundBuffer_SetReverbParam( SOUNDBUFFER *Buffer, SOUND3D_REVERB_PARAM *Param )
{
	if( Buffer->Is3DSound == FALSE || Buffer->Valid == FALSE ) return -1 ;

	if( SoundSysData.EnableSoundCaptureFlag )
	{
	}
	else
	{
		// 環境依存処理
		if( SoundBuffer_SetReverbParam_PF( Buffer, Param ) < 0 )
		{
			return -1 ;
		}
	}

	// 終了
	return 0 ;
}

extern int SoundBuffer_SetPresetReverbParam( SOUNDBUFFER *Buffer, int PresetNo )
{
	if( Buffer->Is3DSound == FALSE || Buffer->Valid == FALSE ) return -1 ;

	if( SoundSysData.EnableSoundCaptureFlag )
	{
	}
	else
	{
		// 環境依存処理
		if( SoundBuffer_SetPresetReverbParam_PF( Buffer, PresetNo ) < 0 )
		{
			return 0 ;
		}
	}

	// 終了
	return 0 ;
}















// ラッパー関数

// PlaySoundFile の旧名称
extern int NS_PlaySound( const TCHAR *FileName, int PlayType )
{
	return NS_PlaySoundFile( FileName, PlayType ) ;
}

// PlaySoundFile の旧名称
extern int NS_PlaySoundWithStrLen( const TCHAR *FileName, size_t FileNameLength, int PlayType )
{
	int Result ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = NS_PlaySoundFile( UseFileNameBuffer, PlayType ) ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( FileName )
	return Result ;
}

// PlaySoundFile の旧名称
extern int PlaySound_WCHAR_T( const wchar_t *FileName, int PlayType )
{
	return PlaySoundFile_WCHAR_T( FileName, PlayType ) ;
}

// CheckSoundFile の旧名称
extern int NS_CheckSound( void )
{
	return NS_CheckSoundFile() ;
}

// StopSoundFile の旧名称
extern int NS_StopSound( void )
{
	return NS_StopSoundFile() ;
}

// SetVolumeSound の旧名称
extern int NS_SetVolumeSound( int VolumePal )
{
	return NS_SetVolumeSoundFile( VolumePal ) ;
}

// WAVEファイルを再生する
extern int NS_PlaySoundFile( const TCHAR *FileName , int PlayType )
{
#ifdef UNICODE
	return PlaySoundFile_WCHAR_T(
		FileName, PlayType
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, return -1 )

	Result = PlaySoundFile_WCHAR_T(
		UseFileNameBuffer, PlayType
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( FileName )

	return Result ;
#endif
}

// サウンドファイルを再生する
extern int NS_PlaySoundFileWithStrLen( const TCHAR *FileName, size_t FileNameLength, int PlayType )
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = PlaySoundFile_WCHAR_T( UseFileNameBuffer, PlayType ) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = PlaySoundFile_WCHAR_T( UseFileNameBuffer, PlayType ) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#endif
	return Result ;
}

// WAVEファイルを再生する
extern int PlaySoundFile_WCHAR_T( const wchar_t *FileName , int PlayType )
{
	LOADSOUND_GPARAM GParam ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// 以前再生中だったデータを止める
	if( SoundSysData.PlayWavSoundHandle != -1 )
	{
		NS_DeleteSoundMem( SoundSysData.PlayWavSoundHandle ) ;
	}

	// サウンドデータを読み込む
	InitLoadSoundGParam( &GParam ) ;
	SoundSysData.PlayWavSoundHandle = LoadSoundMemBase_UseGParam( &GParam, FileName, 1, -1, FALSE, FALSE ) ;
	if( SoundSysData.PlayWavSoundHandle == -1 )
	{
		return -1 ;
	}

	// サウンドを再生する
	NS_PlaySoundMem( SoundSysData.PlayWavSoundHandle , PlayType ) ;

	// 終了
	return 0 ;
}

// WAVEファイルが再生中か調べる
extern int NS_CheckSoundFile( void )
{
	int Result ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}
	if( SoundSysData.PlayWavSoundHandle == -1 )
	{
		return 0 ;
	}

	Result = NS_CheckSoundMem( SoundSysData.PlayWavSoundHandle ) ;

	return Result ;
}

// WAVEファイルの再生を止める
extern int NS_StopSoundFile( void )
{
	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}
	if( SoundSysData.PlayWavSoundHandle == -1 ) return 0 ;

	return NS_StopSoundMem( SoundSysData.PlayWavSoundHandle ) ;
}

// WAVEファイルの音量をセットする
extern int NS_SetVolumeSoundFile( int VolumePal )
{
	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}
	if( SoundSysData.PlayWavSoundHandle == -1 ) return 0 ;

	return NS_SetVolumeSoundMem( VolumePal, SoundSysData.PlayWavSoundHandle ) ;
}

// サウンドキャプチャの開始
extern	int StartSoundCapture( const wchar_t *SaveFilePath )
{
#ifdef DX_NON_SAVEFUNCTION

	return -1 ;

#else // DX_NON_SAVEFUNCTION

	BYTE temp[NORMALWAVE_HEADERSIZE] ;
	
	// サウンドキャプチャが無効な場合は何もせず終了
	if( SoundSysData.EnableSoundCaptureFlag == FALSE ) return -1 ;

	// 既にキャプチャを開始している場合は何もせず終了
	if( SoundSysData.SoundCaptureFlag == TRUE ) return -1 ;
	SoundSysData.SoundCaptureFlag = TRUE;
	
	// 保存用のファイルを開く
	SoundSysData.SoundCaptureFileHandle = WriteOnlyFileAccessOpen( SaveFilePath ) ;
	if( SoundSysData.SoundCaptureFileHandle == 0 )
	{
		DXST_LOGFILE_ADDUTF16LE( "\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xad\x30\xe3\x30\xd7\x30\xc1\x30\xe3\x30\xdd\x4f\x58\x5b\x28\x75\x6e\x30\xd5\x30\xa1\x30\xa4\x30\xeb\x30\x4c\x30\x8b\x95\x51\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x02\x30\x00"/*@ L"サウンドキャプチャ保存用のファイルが開けませんでした。" @*/ ) ;
		return -1 ;
	}
	
	// ヘッダー分の空データを書き出す
	_MEMSET( temp, 0, sizeof( temp ) ) ;
	WriteOnlyFileAccessWrite( SoundSysData.SoundCaptureFileHandle, temp, NORMALWAVE_HEADERSIZE ) ;
	
	// サウンドキャプチャのフラグを立てる
	SoundSysData.SoundCaptureFlag = TRUE ;
	
	// キャプチャしたサンプル数を０にする
	SoundSysData.SoundCaptureSample = 0 ;
	
	// 終了
	return 0 ;
#endif // DX_NON_SAVEFUNCTION
}

// サウンドキャプチャの周期的処理
extern	int SoundCaptureProcess( int CaptureSample )
{
	int i, j, num, k ;
	short *Temp = NULL ;
	SOUND *sound ;

	if( SoundSysData.EnableSoundCaptureFlag == FALSE ) return -1 ;
	
#ifndef DX_NON_SAVEFUNCTION
	// キャプチャを行う場合はメモリの確保
	if( SoundSysData.SoundCaptureFlag == TRUE )
	{
		Temp = (short *)DXALLOC( ( size_t )( CaptureSample * 4 ) ) ;
		_MEMSET( Temp, 0, ( size_t )( CaptureSample * 4 ) ) ;
	}
#endif // DX_NON_SAVEFUNCTION
	
	// サウンドバッファの進行処理を行う
	num = HandleManageArray[ DX_HANDLETYPE_SOUND ].Num ;
	for( i = 0, j = HandleManageArray[ DX_HANDLETYPE_SOUND ].AreaMin ; i < num ; j ++ )
	{
		if( HandleManageArray[ DX_HANDLETYPE_SOUND ].Handle[ j ] == NULL ) continue ;
		i ++ ;
		
		sound = ( SOUND * )HandleManageArray[ DX_HANDLETYPE_SOUND ].Handle[ j ] ;
		switch( sound->Type )
		{
		case DX_SOUNDTYPE_NORMAL :
			for( k = 0 ; k < MAX_SOUNDBUFFER_NUM ; k ++ )
			{
				if( sound->Buffer[ k ].Valid == FALSE || sound->Buffer[ k ].State == FALSE ) continue ;
				SoundBuffer_FrameProcess( &sound->Buffer[ k ], CaptureSample, Temp ) ;
			}
			break ;
		
		case DX_SOUNDTYPE_STREAMSTYLE :
			if( sound->Buffer[ 0 ].Valid && sound->Buffer[ 0 ].State )
				SoundBuffer_FrameProcess( &sound->Buffer[ 0 ], CaptureSample, Temp ) ;
			break ;
		}
	}

#ifndef DX_NON_SAVEFUNCTION
	
	// キャプチャ用のデータを書き出す
	if( SoundSysData.SoundCaptureFlag == TRUE )
	{
		WriteOnlyFileAccessWrite( SoundSysData.SoundCaptureFileHandle, Temp, ( DWORD )( CaptureSample * 4 ) ) ;
		SoundSysData.SoundCaptureSample += CaptureSample ;
		
		// メモリの解放
		DXFREE( Temp ) ;
	}

#endif // DX_NON_SAVEFUNCTION

	// 終了
	return 0 ;
}

// サウンドキャプチャの終了
extern	int EndSoundCapture( void )
{
#ifdef DX_NON_SAVEFUNCTION

	return 0 ;

#else // DX_NON_SAVEFUNCTION

	BYTE Header[NORMALWAVE_HEADERSIZE], *p ;
	WAVEFORMATEX *format;

	// サウンドキャプチャを実行していなかった場合は何もせず終了
	if( SoundSysData.SoundCaptureFlag == FALSE ) return -1 ;
	
	// フォーマットをセット
	format = (WAVEFORMATEX *)&Header[20]; 
	format->wFormatTag      = WAVE_FORMAT_PCM ;
	format->nChannels       = 2 ;
	format->nSamplesPerSec  = 44100 ;
	format->wBitsPerSample  = 16 ;
	format->nBlockAlign     = ( WORD )( format->wBitsPerSample / 8 * format->nChannels ) ;
	format->nAvgBytesPerSec = format->nSamplesPerSec * format->nBlockAlign ;
	format->cbSize          = 0 ;
	
	// ヘッダを書き出してファイルを閉じる
	p = Header;
	_MEMCPY( (char *)p, "RIFF", 4 ) ;												p += 4 ;
	*((DWORD *)p) = ( DWORD )( SoundSysData.SoundCaptureSample * format->nBlockAlign + NORMALWAVE_HEADERSIZE - 8 ) ;	p += 4 ;
	_MEMCPY( (char *)p, "WAVE", 4 ) ;												p += 4 ;

	_MEMCPY( (char *)p, "fmt ", 4 ) ;												p += 4 ;
	*((DWORD *)p) = NORMALWAVE_FORMATSIZE ;											p += 4 + NORMALWAVE_FORMATSIZE ;

	_MEMCPY( (char *)p, "data", 4 ) ;												p += 4 ;
	*((DWORD *)p) = ( DWORD )( SoundSysData.SoundCaptureSample * format->nBlockAlign ) ;	p += 4 ;
	
	WriteOnlyFileAccessSeek( SoundSysData.SoundCaptureFileHandle, 0, SEEK_SET ) ;
	WriteOnlyFileAccessWrite( SoundSysData.SoundCaptureFileHandle, Header, NORMALWAVE_HEADERSIZE ) ;
	WriteOnlyFileAccessClose( SoundSysData.SoundCaptureFileHandle ) ;
	SoundSysData.SoundCaptureFileHandle = 0 ;
	
	// キャプチャ終了
	SoundSysData.SoundCaptureFlag = FALSE ;
	
	// 終了
	return 0 ;

#endif // DX_NON_SAVEFUNCTION
}



















// ソフトウエア制御サウンド系関数

// ソフトサウンドハンドルをセットアップする
extern int SetupSoftSoundHandle(
	int SoftSoundHandle,
	int IsPlayer,
	int Channels,
	int BitsPerSample,
	int SamplesPerSec,
	int IsFloatType,
	int SampleNum
)
{
	SOFTSOUND * SSound ;

	if( SoundSysData.InitializeFlag == FALSE ) return -1 ;

	// エラー判定
	if( SSND_MASKHCHK_ASYNC( SoftSoundHandle, SSound ) )
		return -1 ;

	// パラメータの初期化
	SSound->IsPlayer = IsPlayer ;
	if( IsFloatType )
	{
		SSound->BufferFormat.wFormatTag  = WAVE_FORMAT_IEEE_FLOAT ;
		SSound->BufferFormat.wBitsPerSample  = ( WORD )32 ;
	}
	else
	{
		SSound->BufferFormat.wFormatTag  = WAVE_FORMAT_PCM ;
		SSound->BufferFormat.wBitsPerSample  = ( WORD )BitsPerSample ;
	}
	SSound->BufferFormat.nChannels       = ( WORD )Channels ;
	SSound->BufferFormat.nSamplesPerSec  = ( DWORD )SamplesPerSec ;
	SSound->BufferFormat.nBlockAlign     = ( WORD )( SSound->BufferFormat.wBitsPerSample / 8 * SSound->BufferFormat.nChannels ) ;
	SSound->BufferFormat.nAvgBytesPerSec = SSound->BufferFormat.nSamplesPerSec * SSound->BufferFormat.nBlockAlign ;
	SSound->BufferFormat.cbSize = 0 ;

	// プレイヤーかどうかで処理を分岐
	if( IsPlayer )
	{
		DWORD BufferSize ;

		// プレイヤーの場合

		// リングバッファの初期化
		RingBufInitialize( &SSound->Player.StockSample ) ;
		SSound->Player.StockSampleNum = 0 ;

		// 再生用サウンドバッファの作成
		BufferSize = SOUNDSIZE( SSND_PLAYER_STRM_BUFSEC * SSound->BufferFormat.nAvgBytesPerSec / SSND_PLAYER_SEC_DIVNUM, SSound->BufferFormat.nBlockAlign ) ;
		if( SoundBuffer_Initialize( &SSound->Player.SoundBuffer, BufferSize, &SSound->BufferFormat, NULL, TRUE, TRUE, FALSE ) != 0 )
		{
			DXST_LOGFILE_ADDUTF16LE( "\xbd\x30\xd5\x30\xc8\x30\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xd7\x30\xec\x30\xa4\x30\xe4\x30\xfc\x30\x28\x75\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"ソフトサウンドプレイヤー用サウンドバッファの作成に失敗しました" @*/ ) ;
			return -1 ;
		}
		SSound->Player.SoundBufferSize   = ( int )BufferSize ;
		SSound->Player.DataSetCompOffset = 0 ;

		SSound->Player.NoneDataSetCompOffset           = -1 ;
		SSound->Player.NoneDataPlayCheckBackPlayOffset = 0 ;
		SSound->Player.NoneDataPlayStartFlag           = FALSE ;

		SSound->Player.IsPlayFlag = FALSE ;

		// 先行バッファサイズをセット
		SSound->Player.MaxDataSetSize = ( int )SOUNDSIZE( SSND_PLAYER_STRM_SAKICOPYSEC    * SSound->BufferFormat.nAvgBytesPerSec / SSND_PLAYER_SEC_DIVNUM, SSound->BufferFormat.nBlockAlign ) ;
		SSound->Player.MinDataSetSize = ( int )SOUNDSIZE( SSND_PLAYER_STRM_MINSAKICOPYSEC * SSound->BufferFormat.nAvgBytesPerSec / SSND_PLAYER_SEC_DIVNUM, SSound->BufferFormat.nBlockAlign ) ;

		// クリティカルセクションの取得
		CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

		// ハンドルリストに追加
		AddHandleList( &SoundSysData.SoftSoundPlayerListFirst, &SSound->Player.SoftSoundPlayerList, -1, SSound ) ;

		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;
	}
	else
	{
		// サウンドデータの場合

		// データを格納するメモリ領域を確保
		SSound->Wave.BufferSampleNum = SampleNum ;
		SSound->Wave.Buffer          = DXALLOC( ( size_t )( SampleNum * SSound->BufferFormat.nBlockAlign ) ) ;
		if( SSound->Wave.Buffer == NULL )
		{
			DXST_LOGFILE_ADDUTF16LE( "\xbd\x30\xd5\x30\xc8\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x6e\x30\xe2\x6c\x62\x5f\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x69\x00\x6e\x00\x20\x00\x41\x00\x64\x00\x64\x00\x53\x00\x6f\x00\x66\x00\x74\x00\x53\x00\x6f\x00\x75\x00\x6e\x00\x64\x00\x44\x00\x61\x00\x74\x00\x61\x00\x00"/*@ L"ソフトハンドルの波形を格納するメモリ領域の確保に失敗しました in AddSoftSoundData" @*/ ) ;
			return -1 ;
		}
	}

	// 正常終了
	return 0 ;
}

// ソフトサウンドハンドルの初期化
extern int InitializeSoftSoundHandle( HANDLEINFO * )
{
	// 特に何もしない
	return 0 ;
}

// ソフトサウンドハンドルの後始末
extern int TerminateSoftSoundHandle( HANDLEINFO *HandleInfo )
{
	SOFTSOUND *SSound = ( SOFTSOUND * )HandleInfo ;

	// プレイヤーかどうかで処理を分岐
	if( SSound->IsPlayer == TRUE )
	{
		// リングバッファの解放
		RingBufTerminate( &SSound->Player.StockSample ) ;

		// サウンドバッファーの解放
		SoundBuffer_Terminate( &SSound->Player.SoundBuffer ) ;

		// ソフトサウンドプレイヤーリストから外す
		SubHandleList( &SSound->Player.SoftSoundPlayerList ) ;
	}
	else
	{
		// サウンドデータを格納していたメモリ領域を開放
		if( SSound->Wave.Buffer )
		{
			DXFREE( SSound->Wave.Buffer ) ;
			SSound->Wave.Buffer = NULL ;
		}
	}

	// 終了
	return 0 ;
}

// ソフトサウンドハンドルを削除するかどうかをチェックする関数
extern int DeleteCancelCheckSoftSoundFunction( HANDLEINFO *HandleInfo )
{
	SOFTSOUND *SSound = ( SOFTSOUND * )HandleInfo ;

	return SSound->IsPlayer != FALSE ;
}

// ソフトサウンドプレーヤーハンドルを削除するかどうかをチェックする関数
extern int DeleteCancelCheckSoftSoundPlayerFunction( HANDLEINFO *HandleInfo )
{
	SOFTSOUND *SSound = ( SOFTSOUND * )HandleInfo ;

	return SSound->IsPlayer == FALSE ;
}


// ソフトウエアで扱う波形データをすべて解放する
extern int NS_InitSoftSound( void )
{
	if( SoundSysData.InitializeFlag == FALSE )
		return -1 ;

	return AllHandleSub( DX_HANDLETYPE_SOFTSOUND, DeleteCancelCheckSoftSoundFunction );
}

// LoadSoftSoundBase の実処理関数
static int LoadSoftSoundBase_Static(
	LOADSOUND_GPARAM *GParam,
	int SoftSoundHandle,
	const wchar_t *FileName,
	const void *FileImage,
	int FileImageSize,
	int /*ASyncThread*/
)
{
	SOFTSOUND * SSound ;
	STREAMDATA Stream ;
	SOUNDCONV ConvData ;
	WAVEFORMATEX Format ;
	int SampleNum ;
	void *SrcBuffer = NULL ;
	int SoundSize ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
		return -1 ;

	if( SSND_MASKHCHK_ASYNC( SoftSoundHandle, SSound ) )
		return -1 ;

	// ファイル名が NULL ではない場合はファイルから読み込む
	if( FileName != NULL )
	{
		// ファイルを開く
		Stream.DataPoint = DX_FOPEN( FileName ) ;
		if( Stream.DataPoint == NULL ) return -1 ;
		Stream.ReadShred = *GetFileStreamDataShredStruct() ;
	}
	else
	// それ以外の場合はメモリから読み込み
	{
		Stream.DataPoint = MemStreamOpen( ( void *)FileImage, ( size_t )FileImageSize ) ;
		Stream.ReadShred = *GetMemStreamDataShredStruct() ;
	}

	// ＰＣＭ形式に変換
	{
		_MEMSET( &ConvData, 0, sizeof( ConvData ) ) ;
		if( SetupSoundConvert( &ConvData, &Stream, GParam->DisableReadSoundFunctionMask
#ifndef DX_NON_OGGVORBIS
								,GParam->OggVorbisBitDepth, GParam->OggVorbisFromTheoraFile
#endif
								) < 0 )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xbd\x30\xd5\x30\xc8\x30\xb5\x30\xa6\x30\xf3\x30\xc9\x30\x28\x75\x6e\x30\xf3\x97\xf0\x58\xd5\x30\xa1\x30\xa4\x30\xeb\x30\x6e\x30\x30\xff\x23\xff\x2d\xff\x78\x30\x6e\x30\x09\x59\xdb\x63\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"ソフトサウンド用の音声ファイルのＰＣＭへの変換に失敗しました" @*/ )) ;
			goto ERR ;
		}
		if( SoundConvertFast( &ConvData, &Format, &SrcBuffer, &SoundSize ) < 0 )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xbd\x30\xd5\x30\xc8\x30\xb5\x30\xa6\x30\xf3\x30\xc9\x30\x28\x75\x6e\x30\xf3\x97\xf0\x58\xd5\x30\xa1\x30\xa4\x30\xeb\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"ソフトサウンド用の音声ファイルを格納するメモリ領域の確保に失敗しました" @*/ )) ;
			goto ERR ;
		}

		TerminateSoundConvert( &ConvData ) ;
	}

	// ファイルを閉じる
	if( FileName != NULL )
	{
		DX_FCLOSE( ( DWORD_PTR )Stream.DataPoint ) ;
	}
	else
	{
		MemStreamClose( Stream.DataPoint ) ;
	}
	Stream.DataPoint = NULL ;

	// サンプルの数を算出する
	SampleNum = SoundSize / Format.nBlockAlign ;

	// ハンドルのセットアップ
	if( SetupSoftSoundHandle(
			SoftSoundHandle,
			FALSE,
			( int )Format.nChannels,
			( int )Format.wBitsPerSample,
			( int )Format.nSamplesPerSec,
			Format.wFormatTag == WAVE_FORMAT_IEEE_FLOAT ? TRUE : FALSE,
			SampleNum ) < 0 )
		goto ERR ;

	// サウンドデータのコピー
	_MEMCPY( SSound->Wave.Buffer, SrcBuffer, ( size_t )SoundSize ) ;

	// サウンドデータの解放
	if( SrcBuffer )
	{
		DXFREE( SrcBuffer ) ;
	}

	// 正常終了
	return 0 ;

ERR :
	if( Stream.DataPoint )
	{
		if( FileName != NULL )
		{
			DX_FCLOSE( ( DWORD_PTR )Stream.DataPoint ) ;
		}
		else
		{
			MemStreamClose( Stream.DataPoint ) ;
		}
	}
	TerminateSoundConvert( &ConvData ) ;
	if( SrcBuffer )
	{
		DXFREE( SrcBuffer ) ;
	}

	// エラー終了
	return -1 ;
}

#ifndef DX_NON_ASYNCLOAD
// LoadSoftSoundBase の非同期読み込みスレッドから呼ばれる関数
static void LoadSoftSoundBase_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	LOADSOUND_GPARAM *GParam ;
	int SoftSoundHandle ;
	const wchar_t *FileName ;
	const void *FileImage ;
	int FileImageSize ;
	int Addr ;
	int Result ;

	Addr = 0 ;
	GParam = ( LOADSOUND_GPARAM * )GetASyncLoadParamStruct( AParam->Data, &Addr ) ;
	SoftSoundHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	FileName = GetASyncLoadParamString( AParam->Data, &Addr ) ;
	FileImage = GetASyncLoadParamVoidP( AParam->Data, &Addr ) ;
	FileImageSize = GetASyncLoadParamInt( AParam->Data, &Addr ) ;

	Result = LoadSoftSoundBase_Static( GParam, SoftSoundHandle, FileName, FileImage, FileImageSize, TRUE ) ;

	DecASyncLoadCount( SoftSoundHandle ) ;
	if( Result < 0 )
	{
		SubHandle( SoftSoundHandle ) ;
	}
}
#endif // DX_NON_ASYNCLOAD

// ソフトウエアで扱う波形データをファイルまたはメモリ上に展開されたファイルイメージから作成する
extern int LoadSoftSoundBase_UseGParam(
	LOADSOUND_GPARAM *GParam,
	const wchar_t *FileName,
	const void *FileImage,
	int FileImageSize,
	int ASyncLoadFlag
)
{
	int SoftSoundHandle ;

	if( SoundSysData.InitializeFlag == FALSE )
		return -1 ;

	// ソフトが非アクティブの場合はアクティブになるまで待つ
	CheckActiveState() ;

	// ハンドルの作成
	SoftSoundHandle = AddHandle( DX_HANDLETYPE_SOFTSOUND, FALSE, -1 ) ;
	if( SoftSoundHandle == -1 ) return -1 ;

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;
		wchar_t FullPath[ 1024 ] ;

		ConvertFullPathW_( FileName, FullPath, sizeof( FullPath ) ) ;

		// パラメータに必要なメモリのサイズを算出
		Addr = 0 ;
		AddASyncLoadParamStruct( NULL, &Addr, GParam, sizeof( *GParam ) ) ;
		AddASyncLoadParamInt( NULL, &Addr, SoftSoundHandle ) ;
		AddASyncLoadParamString( NULL, &Addr, FullPath ) ;
		AddASyncLoadParamConstVoidP( NULL, &Addr, ( void * )FileImage ) ;
		AddASyncLoadParamInt( NULL, &Addr, FileImageSize ) ;

		// メモリの確保
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// 処理に必要な情報をセット
		AParam->ProcessFunction = LoadSoftSoundBase_ASync ;
		Addr = 0 ;
		AddASyncLoadParamStruct( AParam->Data, &Addr, GParam, sizeof( *GParam ) ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, SoftSoundHandle ) ;
		AddASyncLoadParamString( AParam->Data, &Addr, FullPath ) ;
		AddASyncLoadParamConstVoidP( AParam->Data, &Addr, ( void * )FileImage ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, FileImageSize ) ;

		// データを追加
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto ERR ;
		}

		// 非同期読み込みカウントをインクリメント
		IncASyncLoadCount( SoftSoundHandle, AParam->Index ) ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		if( LoadSoftSoundBase_Static( GParam, SoftSoundHandle, FileName, FileImage, FileImageSize, FALSE ) < 0 )
			goto ERR ;
	}

	// 終了
	return SoftSoundHandle ;

ERR :
	SubHandle( SoftSoundHandle ) ;

	return -1 ;
}

// ソフトウエアで扱う波形データをファイルから作成する
extern	int NS_LoadSoftSound( const TCHAR *FileName )
{
#ifdef UNICODE
	return LoadSoftSound_WCHAR_T(
		FileName
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, return -1 )

	Result = LoadSoftSound_WCHAR_T(
		UseFileNameBuffer
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( FileName )

	return Result ;
#endif
}

// ソフトウエアで扱う波形データハンドルをサウンドファイルから作成する
extern int NS_LoadSoftSoundWithStrLen( const TCHAR *FileName, size_t FileNameLength )
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = LoadSoftSound_WCHAR_T( UseFileNameBuffer ) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = LoadSoftSound_WCHAR_T( UseFileNameBuffer ) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#endif
	return Result ;
}

// ソフトウエアで扱う波形データをファイルから作成する
extern	int LoadSoftSound_WCHAR_T( const wchar_t *FileName )
{
	LOADSOUND_GPARAM GParam ;

	InitLoadSoundGParam( &GParam ) ;
	return LoadSoftSoundBase_UseGParam( &GParam, FileName, NULL, 0, GetASyncLoadFlag() ) ;
}

// ソフトウエアで扱う波形データをメモリ上に展開されたファイルイメージから作成する
extern	int	NS_LoadSoftSoundFromMemImage( const void *FileImageBuffer, int FileImageSize )
{
	LOADSOUND_GPARAM GParam ;

	InitLoadSoundGParam( &GParam ) ;
	return LoadSoftSoundBase_UseGParam( &GParam, NULL, FileImageBuffer, FileImageSize, GetASyncLoadFlag() ) ;
}

// MakeSoftSoundBase の実処理関数
static int MakeSoftSoundBase_Static(
	int SoftSoundHandle,
	int IsPlayer,
	int Channels,
	int BitsPerSample,
	int SamplesPerSec,
	int SampleNum,
	int IsFloatType,
	int UseFormat_SoftSoundHandle,
	int /*ASyncThread*/
)
{
	SOFTSOUND * SSound ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
		return -1 ;

	if( SSND_MASKHCHK_ASYNC( SoftSoundHandle, SSound ) )
		return -1 ;

	if( UseFormat_SoftSoundHandle >= 0 )
	{
		if( NS_GetSoftSoundFormat( UseFormat_SoftSoundHandle, &Channels, &BitsPerSample, &SamplesPerSec, &IsFloatType ) < 0 )
			return -1 ;
	}

	if( SetupSoftSoundHandle( SoftSoundHandle, IsPlayer, Channels, BitsPerSample, SamplesPerSec, IsFloatType, SampleNum ) < 0 )
		return -1 ;

	// 正常終了
	return 0 ;
}

#ifndef DX_NON_ASYNCLOAD
// MakeSoftSoundBase の非同期読み込みスレッドから呼ばれる関数
static void MakeSoftSoundBase_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	int SoftSoundHandle ;
	int IsPlayer ;
	int Channels ;
	int BitsPerSample ;
	int SamplesPerSec ;
	int SampleNum ;
	int IsFloatType ;
	int UseFormat_SoftSoundHandle ;
	int Addr ;
	int Result ;

	Addr = 0 ;
	SoftSoundHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	IsPlayer = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	Channels = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	BitsPerSample = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	SamplesPerSec = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	SampleNum = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	IsFloatType = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	UseFormat_SoftSoundHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;

	Result = MakeSoftSoundBase_Static( SoftSoundHandle, IsPlayer, Channels, BitsPerSample, SamplesPerSec, SampleNum, IsFloatType, UseFormat_SoftSoundHandle, TRUE ) ;

	DecASyncLoadCount( SoftSoundHandle ) ;
	if( Result < 0 )
	{
		SubHandle( SoftSoundHandle ) ;
	}
}
#endif // DX_NON_ASYNCLOAD

// ソフトウエアで制御するサウンドデータハンドルの作成
extern int MakeSoftSoundBase_UseGParam(
	int IsPlayer,
	int Channels,
	int BitsPerSample,
	int SamplesPerSec,
	int SampleNum,
	int IsFloatType,
	int UseFormat_SoftSoundHandle,
	int ASyncLoadFlag
)
{
	int SoftSoundHandle ;

	if( SoundSysData.InitializeFlag == FALSE )
		return -1 ;

	// ソフトが非アクティブの場合はアクティブになるまで待つ
	CheckActiveState() ;

	// ハンドルの作成
	SoftSoundHandle = AddHandle( DX_HANDLETYPE_SOFTSOUND, FALSE, -1 ) ;
	if( SoftSoundHandle == -1 ) return -1 ;

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;

		// パラメータに必要なメモリのサイズを算出
		Addr = 0 ;
		AddASyncLoadParamInt( NULL, &Addr, SoftSoundHandle ) ;
		AddASyncLoadParamInt( NULL, &Addr, IsPlayer ) ;
		AddASyncLoadParamInt( NULL, &Addr, Channels ) ;
		AddASyncLoadParamInt( NULL, &Addr, BitsPerSample ) ;
		AddASyncLoadParamInt( NULL, &Addr, SamplesPerSec ) ;
		AddASyncLoadParamInt( NULL, &Addr, SampleNum ) ;
		AddASyncLoadParamInt( NULL, &Addr, IsFloatType ) ;
		AddASyncLoadParamInt( NULL, &Addr, UseFormat_SoftSoundHandle ) ;

		// メモリの確保
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// 処理に必要な情報をセット
		AParam->ProcessFunction = MakeSoftSoundBase_ASync ;
		Addr = 0 ;
		AddASyncLoadParamInt( AParam->Data, &Addr, SoftSoundHandle ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, IsPlayer ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, Channels ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, BitsPerSample ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, SamplesPerSec ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, SampleNum ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, IsFloatType ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, UseFormat_SoftSoundHandle ) ;

		// データを追加
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto ERR ;
		}

		// 非同期読み込みカウントをインクリメント
		IncASyncLoadCount( SoftSoundHandle, AParam->Index ) ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		if( MakeSoftSoundBase_Static( SoftSoundHandle, IsPlayer, Channels, BitsPerSample, SamplesPerSec, SampleNum, IsFloatType, UseFormat_SoftSoundHandle, FALSE ) < 0 )
			goto ERR ;
	}

	// 終了
	return SoftSoundHandle ;

ERR :
	SubHandle( SoftSoundHandle ) ;

	return -1 ;
}

// ソフトウエアで扱う空の波形データを作成する( フォーマットは引数のソフトウエアサウンドハンドルと同じものにする )
extern	int NS_MakeSoftSound( int UseFormat_SoftSoundHandle, int SampleNum )
{
	if( UseFormat_SoftSoundHandle < 0 )
		return -1 ;

	return MakeSoftSoundBase_UseGParam( FALSE, 0, 0, 0, SampleNum, FALSE, UseFormat_SoftSoundHandle, GetASyncLoadFlag() ) ;
}

// ソフトウエアで扱う空の波形データを作成する( チャンネル数:2 量子化ビット数:16bit サンプリング周波数:44.1KHz )
extern	int NS_MakeSoftSound2Ch16Bit44KHz( int SampleNum )
{
	return MakeSoftSoundBase_UseGParam( FALSE, 2, 16, 44100, SampleNum, FALSE, -1, GetASyncLoadFlag() ) ;
}

// ソフトウエアで扱う空の波形データを作成する( チャンネル数:2 量子化ビット数:16bit サンプリング周波数:22KHz )
extern	int NS_MakeSoftSound2Ch16Bit22KHz( int SampleNum )
{
	return MakeSoftSoundBase_UseGParam( FALSE, 2, 16, 22050, SampleNum, FALSE, -1, GetASyncLoadFlag() ) ;
}

// ソフトウエアで扱う空の波形データを作成する( チャンネル数:2 量子化ビット数:8bit サンプリング周波数:44.1KHz )
extern	int NS_MakeSoftSound2Ch8Bit44KHz( int SampleNum )
{
	return MakeSoftSoundBase_UseGParam( FALSE, 2, 8, 44100, SampleNum, FALSE, -1, GetASyncLoadFlag() ) ;
}

// ソフトウエアで扱う空の波形データを作成する( チャンネル数:2 量子化ビット数:8bit サンプリング周波数:22KHz )
extern	int NS_MakeSoftSound2Ch8Bit22KHz( int SampleNum )
{
	return MakeSoftSoundBase_UseGParam( FALSE, 2, 8, 22050, SampleNum, FALSE, -1, GetASyncLoadFlag() ) ;
}

// ソフトウエアで扱う空の波形データを作成する( チャンネル数:1 量子化ビット数:16bit サンプリング周波数:44.1KHz )
extern	int NS_MakeSoftSound1Ch16Bit44KHz( int SampleNum )
{
	return MakeSoftSoundBase_UseGParam( FALSE, 1, 16, 44100, SampleNum, FALSE, -1, GetASyncLoadFlag() ) ;
}

// ソフトウエアで扱う空の波形データを作成する( チャンネル数:1 量子化ビット数:16bit サンプリング周波数:22KHz )
extern	int NS_MakeSoftSound1Ch16Bit22KHz( int SampleNum )
{
	return MakeSoftSoundBase_UseGParam( FALSE, 1, 16, 22050, SampleNum, FALSE, -1, GetASyncLoadFlag() ) ;
}

// ソフトウエアで扱う空の波形データを作成する( チャンネル数:1 量子化ビット数:8bit サンプリング周波数:44.1KHz )
extern	int NS_MakeSoftSound1Ch8Bit44KHz( int SampleNum )
{
	return MakeSoftSoundBase_UseGParam( FALSE, 1, 8, 44100, SampleNum, FALSE, -1, GetASyncLoadFlag() ) ;
}

// ソフトウエアで扱う空の波形データを作成する( チャンネル数:1 量子化ビット数:8bit サンプリング周波数:22KHz )
extern	int NS_MakeSoftSound1Ch8Bit22KHz( int SampleNum )
{
	return MakeSoftSoundBase_UseGParam( FALSE, 1, 8, 22050, SampleNum, FALSE, -1, GetASyncLoadFlag() ) ;
}

// ソフトウエアで扱う空の波形データを作成する
extern	int NS_MakeSoftSoundCustom( int ChannelNum, int BitsPerSample, int SamplesPerSec, int SampleNum, int IsFloatType )
{
	return MakeSoftSoundBase_UseGParam( FALSE, ChannelNum, BitsPerSample, SamplesPerSec, SampleNum, IsFloatType, -1, GetASyncLoadFlag() ) ;
}

// ソフトウエアで扱う波形データを解放する
extern	int NS_DeleteSoftSound( int SoftSoundHandle )
{
	SOFTSOUND *SSound ;

	if( SSND_MASKHCHK_ASYNC( SoftSoundHandle, SSound ) ) return -1 ;

	// プレイヤーかどうかのチェック
	if( SSound->IsPlayer != FALSE )
	{
		// 違ったらエラー
		return -1 ;
	}

	return SubHandle( SoftSoundHandle ) ;
}

#ifndef DX_NON_SAVEFUNCTION

// ソフトウエアで扱う波形データを無圧縮Wav形式で保存する
extern int NS_SaveSoftSound( int SoftSoundHandle, const TCHAR *FileName )
{
#ifdef UNICODE
	return SaveSoftSound_WCHAR_T(
		SoftSoundHandle, FileName
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, return -1 )

	Result = SaveSoftSound_WCHAR_T(
		SoftSoundHandle, UseFileNameBuffer
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( FileName )

	return Result ;
#endif
}

// ソフトウエアで扱う波形データハンドルをWAVEファイル(PCM)形式で保存する
extern int NS_SaveSoftSoundWithStrLen( int SoftSoundHandle, const TCHAR *FileName, size_t FileNameLength )
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = SaveSoftSound_WCHAR_T( SoftSoundHandle, UseFileNameBuffer ) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = SaveSoftSound_WCHAR_T( SoftSoundHandle, UseFileNameBuffer ) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#endif
	return Result ;
}

// ソフトウエアで扱う波形データを無圧縮Wav形式で保存する
extern int SaveSoftSound_WCHAR_T( int SoftSoundHandle, const wchar_t *FileName )
{
	DWORD_PTR fp ;
	BYTE Header[NORMALWAVE_HEADERSIZE], *p ;
	WAVEFORMATEX *format;
	SOFTSOUND * SSound ;

	// エラー判定
	if( SSND_MASKHCHK( SoftSoundHandle, SSound ) ) return -1 ;
	if( SSound->IsPlayer == 1 ) return -1 ;

	fp = WriteOnlyFileAccessOpen( FileName ) ;

	// フォーマットをセット
	format = (WAVEFORMATEX *)&Header[20]; 
	format->wFormatTag      = SSound->BufferFormat.wFormatTag ;
	format->nChannels       = SSound->BufferFormat.nChannels ;
	format->nSamplesPerSec  = SSound->BufferFormat.nSamplesPerSec ;
	format->wBitsPerSample  = SSound->BufferFormat.wBitsPerSample ;
	format->nBlockAlign     = ( WORD )( format->wBitsPerSample / 8 * format->nChannels ) ;
	format->nAvgBytesPerSec = format->nSamplesPerSec * format->nBlockAlign ;
	format->cbSize          = 0 ;
	
	// ヘッダを書き出してファイルを閉じる
	p = Header;
	_MEMCPY( (char *)p, "RIFF", 4 ) ;																	p += 4 ;
	*((DWORD *)p) = ( DWORD )( SSound->Wave.BufferSampleNum * format->nBlockAlign + NORMALWAVE_HEADERSIZE - 8 ) ;	p += 4 ;
	_MEMCPY( (char *)p, "WAVE", 4 ) ;																	p += 4 ;

	_MEMCPY( (char *)p, "fmt ", 4 ) ;																	p += 4 ;
	*((DWORD *)p) = NORMALWAVE_FORMATSIZE ;																p += 4 + NORMALWAVE_FORMATSIZE ;

	_MEMCPY( (char *)p, "data", 4 ) ;																	p += 4 ;
	*((DWORD *)p) = ( DWORD )( SSound->Wave.BufferSampleNum * format->nBlockAlign ) ;					p += 4 ;
	
	WriteOnlyFileAccessWrite( fp, Header, NORMALWAVE_HEADERSIZE ) ;
	WriteOnlyFileAccessWrite( fp, SSound->Wave.Buffer, ( size_t )( SSound->Wave.BufferSampleNum * format->nBlockAlign ) ) ;
	WriteOnlyFileAccessClose( fp ) ;

	// 終了
	return 0 ;
}

#endif // DX_NON_SAVEFUNCTION

// ソフトウエアで扱う波形データのサンプル数を取得する
extern	int NS_GetSoftSoundSampleNum( int SoftSoundHandle )
{
	SOFTSOUND * SSound ;

	// エラー判定
	if( SSND_MASKHCHK( SoftSoundHandle, SSound ) ) return -1 ;
	if( SSound->IsPlayer == 1 ) return -1 ;

	// サンプル数を返す
	return SSound->Wave.BufferSampleNum ;
}

// ソフトウエアで扱う波形データのフォーマットを取得する
extern	int NS_GetSoftSoundFormat( int SoftSoundHandle, int *Channels, int *BitsPerSample, int *SamplesPerSec, int *IsFloatType )
{
	SOFTSOUND * SSound ;

	// エラー判定
	if( SSND_MASKHCHK( SoftSoundHandle, SSound ) ) return -1 ;
	if( SSound->IsPlayer == 1 ) return -1 ;

	// データをセット
	if( Channels      ) *Channels      = SSound->BufferFormat.nChannels ;
	if( BitsPerSample ) *BitsPerSample = SSound->BufferFormat.wBitsPerSample ;
	if( SamplesPerSec ) *SamplesPerSec = ( int )SSound->BufferFormat.nSamplesPerSec ;
	if( IsFloatType   ) *IsFloatType   = SSound->BufferFormat.wFormatTag == WAVE_FORMAT_IEEE_FLOAT ? TRUE : FALSE ;

	// 終了
	return 0 ;
}

// ソフトウエアで扱う波形データのサンプルを読み取る
extern	int NS_ReadSoftSoundData( int SoftSoundHandle, int SamplePosition, int *Channel1, int *Channel2 )
{
	SOFTSOUND * SSound ;
	BYTE *Src ;

	// エラー判定
	if( SSND_MASKHCHK( SoftSoundHandle, SSound ) ) return -1 ;
	if( SSound->IsPlayer == 1 ) return -1 ;

	// サンプルの範囲外だった場合もエラー
	if( SSound->Wave.BufferSampleNum <= SamplePosition || SamplePosition < 0 )
		return -1 ;

	// サンプルの位置を算出
	Src = ( BYTE * )SSound->Wave.Buffer + SSound->BufferFormat.nBlockAlign * SamplePosition ;

	// サンプルを返す
	if( SSound->BufferFormat.wFormatTag == WAVE_FORMAT_IEEE_FLOAT )
	{
		switch( SSound->BufferFormat.wBitsPerSample )
		{
		case 32 :
			if( Channel1 )
			{
				*Channel1 = _FTOL( ( ( float * )Src )[ 0 ] * 32768.0f ) ;
			}

			if( Channel2 )
			{
				if( SSound->BufferFormat.nChannels == 1 )
				{
					*Channel2 = 0 ;
				}
				else
				{
					*Channel2 = _FTOL( ( ( float * )Src )[ 1 ] * 32768.0f ) ;
				}
			}
			break ;
		}
	}
	else
	{
		switch( SSound->BufferFormat.wBitsPerSample )
		{
		case 8 :
			if( Channel1 )
			{
				*Channel1 = ( int )Src[ 0 ] ;
			}

			if( Channel2 )
			{
				if( SSound->BufferFormat.nChannels == 1 )
				{
					*Channel2 = 127 ;
				}
				else
				{
					*Channel2 = ( int )Src[ 1 ] ;
				}
			}
			break ;

		case 16 :
			if( Channel1 )
			{
				*Channel1 = ( ( short * )Src )[ 0 ] ;
			}

			if( Channel2 )
			{
				if( SSound->BufferFormat.nChannels == 1 )
				{
					*Channel2 = 0 ;
				}
				else
				{
					*Channel2 = ( ( short * )Src )[ 1 ] ;
				}
			}
			break ;
		}
	}

	// 終了
	return 0 ;
}

// ソフトウエアで扱う波形データハンドルのサンプルを読み取る( float型版 )
extern	int NS_ReadSoftSoundDataF( int SoftSoundHandle, int SamplePosition, float *Channel1, float *Channel2 )
{
	SOFTSOUND * SSound ;
	BYTE *Src ;

	// エラー判定
	if( SSND_MASKHCHK( SoftSoundHandle, SSound ) ) return -1 ;
	if( SSound->IsPlayer == 1 ) return -1 ;

	// サンプルの範囲外だった場合もエラー
	if( SSound->Wave.BufferSampleNum <= SamplePosition || SamplePosition < 0 )
		return -1 ;

	// サンプルの位置を算出
	Src = ( BYTE * )SSound->Wave.Buffer + SSound->BufferFormat.nBlockAlign * SamplePosition ;

	// サンプルを返す
	if( SSound->BufferFormat.wFormatTag == WAVE_FORMAT_IEEE_FLOAT )
	{
		switch( SSound->BufferFormat.wBitsPerSample )
		{
		case 32 :
			if( Channel1 )
			{
				*Channel1 = ( ( float * )Src )[ 0 ] ;
			}

			if( Channel2 )
			{
				if( SSound->BufferFormat.nChannels == 1 )
				{
					*Channel2 = 0.0f ;
				}
				else
				{
					*Channel2 = ( ( float * )Src )[ 1 ] ;
				}
			}
			break ;
		}
	}
	else
	{
		switch( SSound->BufferFormat.wBitsPerSample )
		{
		case 8 :
			if( Channel1 )
			{
				*Channel1 = ( float )( ( int )Src[ 0 ] - 127 ) / 128.0f ;
			}

			if( Channel2 )
			{
				if( SSound->BufferFormat.nChannels == 1 )
				{
					*Channel2 = 0.0f ;
				}
				else
				{
					*Channel2 = ( float )( ( int )Src[ 1 ] - 127 ) / 128.0f ;
				}
			}
			break ;

		case 16 :
			if( Channel1 )
			{
				*Channel1 = ( float )( ( short * )Src )[ 0 ] / 32768.0f ;
			}

			if( Channel2 )
			{
				if( SSound->BufferFormat.nChannels == 1 )
				{
					*Channel2 = 0.0f ;
				}
				else
				{
					*Channel2 = ( float )( ( short * )Src )[ 1 ] / 32768.0f ;
				}
			}
			break ;
		}
	}

	// 終了
	return 0 ;
}

// ソフトウエアで扱う波形データのサンプルを書き込む
extern	int NS_WriteSoftSoundData( int SoftSoundHandle, int SamplePosition, int Channel1, int Channel2 )
{
	SOFTSOUND * SSound ;
	BYTE *Dest ;

	// エラー判定
	if( SSND_MASKHCHK( SoftSoundHandle, SSound ) ) return -1 ;
	if( SSound->IsPlayer == 1 ) return -1 ;

	// サンプルの範囲外だった場合もエラー
	if( SSound->Wave.BufferSampleNum <= SamplePosition || SamplePosition < 0 )
		return -1 ;

	// サンプルの位置を算出
	Dest = ( BYTE * )SSound->Wave.Buffer + SSound->BufferFormat.nBlockAlign * SamplePosition ;

	// サンプルを書き込む
	if( SSound->BufferFormat.wFormatTag == WAVE_FORMAT_IEEE_FLOAT )
	{
		switch( SSound->BufferFormat.wBitsPerSample )
		{
		case 32 :
			( ( float * )Dest )[ 0 ] = ( float )Channel1 / 32768.0f ;

			if( SSound->BufferFormat.nChannels != 1 )
			{
				( ( float * )Dest )[ 1 ] = ( float )Channel2 / 32768.0f ;
			}
			break ;
		}
	}
	else
	{
		switch( SSound->BufferFormat.wBitsPerSample )
		{
		case 8 :
				 if( Channel1 > 255 ) Channel1 = 255 ;
			else if( Channel1 <   0 ) Channel1 =   0 ;
			Dest[ 0 ] = ( BYTE )Channel1 ;

			if( SSound->BufferFormat.nChannels != 1 )
			{
					 if( Channel2 > 255 ) Channel2 = 255 ;
				else if( Channel2 <   0 ) Channel2 =   0 ;
				Dest[ 1 ] = ( BYTE )Channel2 ;
			}
			break ;

		case 16 :
				 if( Channel1 >  32767 ) Channel1 =  32767 ;
			else if( Channel1 < -32768 ) Channel1 = -32768 ;
			( ( short * )Dest )[ 0 ] = ( short )Channel1 ;

			if( SSound->BufferFormat.nChannels != 1 )
			{
					 if( Channel2 >  32767 ) Channel2 =  32767 ;
				else if( Channel2 < -32768 ) Channel2 = -32768 ;
				( ( short * )Dest )[ 1 ] = ( short )Channel2 ;
			}
			break ;
		}
	}

	// 終了
	return 0 ;
}

// ソフトウエアで扱う波形データハンドルのサンプルを書き込む( float型版 )
extern int NS_WriteSoftSoundDataF( int SoftSoundHandle, int SamplePosition, float Channel1, float Channel2 )
{
	SOFTSOUND * SSound ;
	BYTE *Dest ;
	int ChannelI ;

	// エラー判定
	if( SSND_MASKHCHK( SoftSoundHandle, SSound ) ) return -1 ;
	if( SSound->IsPlayer == 1 ) return -1 ;

	// サンプルの範囲外だった場合もエラー
	if( SSound->Wave.BufferSampleNum <= SamplePosition || SamplePosition < 0 )
		return -1 ;

	// サンプルの位置を算出
	Dest = ( BYTE * )SSound->Wave.Buffer + SSound->BufferFormat.nBlockAlign * SamplePosition ;

	// サンプルを書き込む
	if( SSound->BufferFormat.wFormatTag == WAVE_FORMAT_IEEE_FLOAT )
	{
		switch( SSound->BufferFormat.wBitsPerSample )
		{
		case 32 :
			( ( float * )Dest )[ 0 ] = Channel1 ;

			if( SSound->BufferFormat.nChannels != 1 )
			{
				( ( float * )Dest )[ 1 ] = Channel2 ;
			}
			break ;
		}
	}
	else
	{
		switch( SSound->BufferFormat.wBitsPerSample )
		{
		case 8 :
			ChannelI = _FTOL( Channel1 * 128.0f + 127.0f  ) ;
				 if( ChannelI > 255 ) ChannelI = 255 ;
			else if( ChannelI <   0 ) ChannelI =   0 ;
			Dest[ 0 ] = ( BYTE )ChannelI ;

			if( SSound->BufferFormat.nChannels != 1 )
			{
				ChannelI = _FTOL( Channel2 * 128.0f + 127.0f  ) ;
					 if( Channel2 > 255 ) Channel2 = 255 ;
				else if( Channel2 <   0 ) Channel2 =   0 ;
				Dest[ 1 ] = ( BYTE )Channel2 ;
			}
			break ;

		case 16 :
			ChannelI = _FTOL( Channel1 * 32768.0f  ) ;
				 if( ChannelI >  32767 ) ChannelI =  32767 ;
			else if( ChannelI < -32768 ) ChannelI = -32768 ;
			( ( short * )Dest )[ 0 ] = ( short )ChannelI ;

			if( SSound->BufferFormat.nChannels != 1 )
			{
				ChannelI = _FTOL( Channel2 * 32768.0f  ) ;
					 if( ChannelI >  32767 ) ChannelI =  32767 ;
				else if( ChannelI < -32768 ) ChannelI = -32768 ;
				( ( short * )Dest )[ 1 ] = ( short )ChannelI ;
			}
			break ;
		}
	}

	// 終了
	return 0 ;
}

// ソフトウエアで扱う波形データハンドルの波形データを音程を変えずにデータの長さを変更する
extern int NS_WriteTimeStretchSoftSoundData( int SrcSoftSoundHandle, int DestSoftSoundHandle )
{
	SOFTSOUND * SSSound ;
	SOFTSOUND * DSSound ;

	// エラー判定
	if( SSND_MASKHCHK_ASYNC( SrcSoftSoundHandle, SSSound ) ) return -1 ;
	if( SSSound->IsPlayer == 1 ) return -1 ;

	if( SSND_MASKHCHK_ASYNC( DestSoftSoundHandle, DSSound ) ) return -1 ;
	if( DSSound->IsPlayer == 1 ) return -1 ;

	// サンプル数が変わらない場合はエラー
	if( SSSound->Wave.BufferSampleNum == DSSound->Wave.BufferSampleNum )
	{
		return -1 ;
	}

	// フォーマットが異なる場合はエラー
	if( SSSound->BufferFormat.wFormatTag     != DSSound->BufferFormat.wFormatTag ||
		SSSound->BufferFormat.nChannels      != DSSound->BufferFormat.nChannels  ||
		SSSound->BufferFormat.nSamplesPerSec != DSSound->BufferFormat.nSamplesPerSec ||
		SSSound->BufferFormat.wBitsPerSample != DSSound->BufferFormat.wBitsPerSample )
	{
		return -1 ;
	}

	// 音が長くなるのか短くなるのかで処理を分岐
	if( SSSound->Wave.BufferSampleNum < DSSound->Wave.BufferSampleNum )
	{
		int n, m, template_size, pmin, pmax, p, q = 0, offset0, offset1 ;
		double rate ;
		int rateI ;
		int SSampleNum ;
		int DSampleNum ;

		rate = ( double )SSSound->Wave.BufferSampleNum / DSSound->Wave.BufferSampleNum ; // rateの範囲は0.5<=rate<1.0
		rateI = _DTOL( rate * 512.0 ) ;
		rate = rateI / 512.0 ;

//		// 8倍以上に長くなる場合はエラー
//		if( rate < 0.125 )
//		{
//			return -1 ;
//		}

		SSampleNum = SSSound->Wave.BufferSampleNum ;
		DSampleNum = DSSound->Wave.BufferSampleNum ;

		template_size = _DTOL( SSSound->BufferFormat.nSamplesPerSec * 0.01  ) ; // 10ms
		pmin          = _DTOL( SSSound->BufferFormat.nSamplesPerSec * 0.005 ) ; // 5ms
		pmax          = _DTOL( SSSound->BufferFormat.nSamplesPerSec * 0.02  ) ; // 20ms

		switch( SSSound->BufferFormat.wFormatTag )
		{
		case WAVE_FORMAT_PCM :

			switch( SSSound->BufferFormat.wBitsPerSample )
			{
			case 16 :
				{
					short *SBuffer ;
					short *DBuffer ;
					int r ;
					int max_of_r;

					SBuffer = ( short * )SSSound->Wave.Buffer ;
					DBuffer = ( short * )DSSound->Wave.Buffer ;

					switch( SSSound->BufferFormat.nChannels )
					{
					case 2 :
						offset0 = 0 ;
						offset1 = 0 ;
						while( offset0 + pmax * 2 < SSampleNum && offset1 + pmax * 2 < DSampleNum )
						{
							max_of_r = 0 ;
							p = pmin ;
							for( m = pmin ; m <= pmax ; m++ )
							{
								short *sp1 ;
								short *sp2 ;

								r = 0 ;
								sp1 = &SBuffer[ ( offset0     ) * 2 ] ;
								sp2 = &SBuffer[ ( offset0 + m ) * 2 ] ;
								for( n = 0 ; n < template_size ; n++, sp1 += 2, sp2 += 2 )
								{
									r += ( sp1[ 0 ] * sp2[ 0 ] ) / 32768 ; // 相関関数
								}
								if( r > max_of_r )
								{
									max_of_r = r ;	// 相関関数のピーク
									p = m ;			// 音データの基本周期
								}
							}

							{
								short *sp1, *dp1 ;
								short *sp2, *dp2 ;
								int movnum ;

								sp1 = &SBuffer[ ( offset0     ) * 2 ] ;
								sp2 = &SBuffer[ ( offset0 + p ) * 2 ] ;
								dp1 = &DBuffer[ ( offset1     ) * 2 ] ;
								dp2 = &DBuffer[ ( offset1 + p ) * 2 ] ;
								for( n = 0; n < p ; n++, sp1 += 2, sp2 += 2, dp1 += 2, dp2 += 2 )
								{
									dp1[ 0 ] =              sp1[ 0 ] ;
									dp2[ 0 ] = ( short )( ( sp2[ 0 ] * ( p - n ) + sp1[ 0 ] * n ) / p ) ;
								}

								if( offset1 + p * 2 < DSampleNum &&
									offset0 + p     < SSampleNum )
								{
									q = ( p * rateI * 512 / ( 512 - rateI ) + 256 ) / 512 ;
									sp1 = &SBuffer[ ( offset0 + p     ) * 2 ] ;
									dp1 = &DBuffer[ ( offset1 + p * 2 ) * 2 ] ;
									movnum = q - p ;
									if( offset0 + p + movnum > SSampleNum )
									{
										movnum = SSampleNum - ( offset0 + p ) ;
										q = p + movnum ;
									}
									if( offset1 + p * 2 + movnum > DSampleNum )
									{
										movnum = DSampleNum - ( offset1 + p * 2 ) ;
										q = p + movnum ;
									}
									for( n = 0 ; n < movnum ; n++, sp1 += 2, dp1 += 2 )
									{
										dp1[ 0 ] = sp1[ 0 ] ;
									}
								}
							}

							offset0 += q ;		// offset0の更新
							offset1 += p + q ;	// offset1の更新
						}

						while( offset1 < DSampleNum && offset0 < SSampleNum )
						{
							max_of_r = 0 ;
							p = pmin ;
							for( m = pmin ; m <= pmax ; m++ )
							{
								short *sp1 ;
								short *sp2 ;

								r = 0 ;
								sp1 = &SBuffer[ ( offset0     ) * 2 ] ;
								sp2 = &SBuffer[ ( offset0 + m ) * 2 ] ;
								for( n = 0 ; n < template_size ; n++, sp1 += 2, sp2 += 2 )
								{
									int sp1p ;
									int sp2p ;

									sp1p = offset0     + n >= SSampleNum ? 0 : sp1[ 0 ] ;
									sp2p = offset0 + m + n >= SSampleNum ? 0 : sp2[ 0 ] ;
									r += ( sp1p * sp2p ) / 32768 ; // 相関関数
								}
								if( r > max_of_r )
								{
									max_of_r = r ;	// 相関関数のピーク
									p = m ;			// 音データの基本周期
								}
							}

							{
								short *sp1, *dp1 ;
								short *sp2, *dp2 ;
								int movnum ;

								sp1 = &SBuffer[ ( offset0     ) * 2 ] ;
								sp2 = &SBuffer[ ( offset0 + p ) * 2 ] ;
								dp1 = &DBuffer[ ( offset1     ) * 2 ] ;
								dp2 = &DBuffer[ ( offset1 + p ) * 2 ] ;
								for( n = 0; n < p ; n++, sp1 += 2, sp2 += 2, dp1 += 2, dp2 += 2 )
								{
									short sp1p ;
									short sp2p ;

									sp1p = offset0     + n >= SSampleNum ? 0 : sp1[ 0 ] ;
									sp2p = offset0 + p + n >= SSampleNum ? 0 : sp2[ 0 ] ;
									if( offset1 + n < DSampleNum )
									{
										dp1[ 0 ] =              sp1p ;
									}
									if( offset1 + p + n < DSampleNum )
									{
										dp2[ 0 ] = ( short )( ( sp2p * ( p - n ) + sp1p * n ) / p ) ;
									}
								}

								if( offset1 + p * 2 < DSampleNum &&
									offset0 + p     < SSampleNum )
								{
									q = ( p * rateI * 512 / ( 512 - rateI ) + 256 ) / 512 ;
									sp1 = &SBuffer[ ( offset0 + p     ) * 2 ] ;
									dp1 = &DBuffer[ ( offset1 + p * 2 ) * 2 ] ;
									movnum = q - p ;
									if( offset0 + p + movnum > SSampleNum )
									{
										movnum = SSampleNum - ( offset0 + p ) ;
										q = p + movnum ;
									}
									if( offset1 + p * 2 + movnum > DSampleNum )
									{
										movnum = DSampleNum - ( offset1 + p * 2 ) ;
										q = p + movnum ;
									}
									for( n = 0 ; n < movnum ; n++, sp1 += 2, dp1 += 2 )
									{
										short sp1p ;

										sp1p = offset0 + p + n >= SSampleNum ? 0 : sp1[ 0 ] ;
										dp1[ 0 ] = sp1p ;
									}
								}
							}

							offset0 += q ;		// offset0の更新
							offset1 += p + q ;	// offset1の更新
						}
						if( offset1 < DSampleNum )
						{
							short *dp ;
							int loopnum ;

							loopnum = DSampleNum - offset1 ;
							dp = &DBuffer[ offset1 * 2 ] ;
							for( n = 0 ; n < loopnum ; n ++, dp += 2 )
							{
								*dp = 0 ;
							}
						}



						offset0 = 0 ;
						offset1 = 0 ;
						while( offset0 + pmax * 2 < SSampleNum && offset1 + pmax * 2 < DSampleNum )
						{
							max_of_r = 0 ;
							p = pmin ;
							for( m = pmin ; m <= pmax ; m++ )
							{
								short *sp1 ;
								short *sp2 ;

								r = 0 ;
								sp1 = &SBuffer[ ( offset0     ) * 2 + 1 ] ;
								sp2 = &SBuffer[ ( offset0 + m ) * 2 + 1 ] ;
								for( n = 0 ; n < template_size ; n++, sp1 += 2, sp2 += 2 )
								{
									r += ( sp1[ 0 ] * sp2[ 0 ] ) / 32768 ; // 相関関数
								}
								if( r > max_of_r )
								{
									max_of_r = r ;	// 相関関数のピーク
									p = m ;			// 音データの基本周期
								}
							}

							{
								short *sp1, *dp1 ;
								short *sp2, *dp2 ;
								int movnum ;

								sp1 = &SBuffer[ ( offset0     ) * 2 + 1 ] ;
								sp2 = &SBuffer[ ( offset0 + p ) * 2 + 1 ] ;
								dp1 = &DBuffer[ ( offset1     ) * 2 + 1 ] ;
								dp2 = &DBuffer[ ( offset1 + p ) * 2 + 1 ] ;
								for( n = 0; n < p ; n++, sp1 += 2, sp2 += 2, dp1 += 2, dp2 += 2 )
								{
									dp1[ 0 ] =              sp1[ 0 ] ;
									dp2[ 0 ] = ( short )( ( sp2[ 0 ] * ( p - n ) + sp1[ 0 ] * n ) / p ) ;
								}

								if( offset1 + p * 2 < DSampleNum &&
									offset0 + p     < SSampleNum )
								{
									q = ( p * rateI * 512 / ( 512 - rateI ) + 256 ) / 512 ;
									sp1 = &SBuffer[ ( offset0 + p     ) * 2 + 1 ] ;
									dp1 = &DBuffer[ ( offset1 + p * 2 ) * 2 + 1 ] ;
									movnum = q - p ;
									if( offset0 + p + movnum > SSampleNum )
									{
										movnum = SSampleNum - ( offset0 + p ) ;
										q = p + movnum ;
									}
									if( offset1 + p * 2 + movnum > DSampleNum )
									{
										movnum = DSampleNum - ( offset1 + p * 2 ) ;
										q = p + movnum ;
									}
									for( n = 0 ; n < movnum ; n++, sp1 += 2, dp1 += 2 )
									{
										dp1[ 0 ] = sp1[ 0 ] ;
									}
								}
							}

							offset0 += q ;		// offset0の更新
							offset1 += p + q ;	// offset1の更新
						}

						while( offset1 < DSampleNum && offset0 < SSampleNum )
						{
							max_of_r = 0 ;
							p = pmin ;
							for( m = pmin ; m <= pmax ; m++ )
							{
								short *sp1 ;
								short *sp2 ;

								r = 0 ;
								sp1 = &SBuffer[ ( offset0     ) * 2 + 1 ] ;
								sp2 = &SBuffer[ ( offset0 + m ) * 2 + 1 ] ;
								for( n = 0 ; n < template_size ; n++, sp1 += 2, sp2 += 2 )
								{
									int sp1p ;
									int sp2p ;

									sp1p = offset0     + n >= SSampleNum ? 0 : sp1[ 0 ] ;
									sp2p = offset0 + m + n >= SSampleNum ? 0 : sp2[ 0 ] ;
									r += ( sp1p * sp2p ) / 32768 ; // 相関関数
								}
								if( r > max_of_r )
								{
									max_of_r = r ;	// 相関関数のピーク
									p = m ;			// 音データの基本周期
								}
							}

							{
								short *sp1, *dp1 ;
								short *sp2, *dp2 ;
								int movnum ;

								sp1 = &SBuffer[ ( offset0     ) * 2 + 1 ] ;
								sp2 = &SBuffer[ ( offset0 + p ) * 2 + 1 ] ;
								dp1 = &DBuffer[ ( offset1     ) * 2 + 1 ] ;
								dp2 = &DBuffer[ ( offset1 + p ) * 2 + 1 ] ;
								for( n = 0; n < p ; n++, sp1 += 2, sp2 += 2, dp1 += 2, dp2 += 2 )
								{
									short sp1p ;
									short sp2p ;

									sp1p = offset0     + n >= SSampleNum ? 0 : sp1[ 0 ] ;
									sp2p = offset0 + p + n >= SSampleNum ? 0 : sp2[ 0 ] ;
									if( offset1 + n < DSampleNum )
									{
										dp1[ 0 ] =              sp1p ;
									}
									if( offset1 + p + n < DSampleNum )
									{
										dp2[ 0 ] = ( short )( ( sp2p * ( p - n ) + sp1p * n ) / p ) ;
									}
								}

								if( offset1 + p * 2 < DSampleNum &&
									offset0 + p     < SSampleNum )
								{
									q = ( p * rateI * 512 / ( 512 - rateI ) + 256 ) / 512 ;
									sp1 = &SBuffer[ ( offset0 + p     ) * 2 + 1 ] ;
									dp1 = &DBuffer[ ( offset1 + p * 2 ) * 2 + 1 ] ;
									movnum = q - p ;
									if( offset0 + p + movnum > SSampleNum )
									{
										movnum = SSampleNum - ( offset0 + p ) ;
										q = p + movnum ;
									}
									if( offset1 + p * 2 + movnum > DSampleNum )
									{
										movnum = DSampleNum - ( offset1 + p * 2 ) ;
										q = p + movnum ;
									}
									for( n = 0 ; n < movnum ; n++, sp1 += 2, dp1 += 2 )
									{
										short sp1p ;

										sp1p = offset0 + p + n >= SSampleNum ? 0 : sp1[ 0 ] ;
										dp1[ 0 ] = sp1p ;
									}
								}
							}

							offset0 += q ;		// offset0の更新
							offset1 += p + q ;	// offset1の更新
						}
						if( offset1 < DSampleNum )
						{
							short *dp ;
							int loopnum ;

							loopnum = DSampleNum - offset1 ;
							dp = &DBuffer[ offset1 * 2 + 1 ] ;
							for( n = 0 ; n < loopnum ; n ++, dp += 2 )
							{
								*dp = 0 ;
							}
						}
						break ;

					case 1 :
						offset0 = 0 ;
						offset1 = 0 ;
						while( offset0 + pmax * 2 < SSampleNum && offset1 + pmax * 2 < DSampleNum )
						{
							max_of_r = 0 ;
							p = pmin ;
							for( m = pmin ; m <= pmax ; m++ )
							{
								short *sp1 ;
								short *sp2 ;

								r = 0 ;
								sp1 = &SBuffer[ offset0     ] ;
								sp2 = &SBuffer[ offset0 + m ] ;
								for( n = 0 ; n < template_size ; n++, sp1 ++, sp2 ++ )
								{
									r += ( sp1[ 0 ] * sp2[ 0 ] ) / 32768 ; // 相関関数
								}
								if( r > max_of_r )
								{
									max_of_r = r ;	// 相関関数のピーク
									p = m ;			// 音データの基本周期
								}
							}

							{
								short *sp1, *dp1 ;
								short *sp2, *dp2 ;
								int movnum ;

								sp1 = &SBuffer[ offset0     ] ;
								sp2 = &SBuffer[ offset0 + p ] ;
								dp1 = &DBuffer[ offset1     ] ;
								dp2 = &DBuffer[ offset1 + p ] ;
								for( n = 0; n < p ; n++, sp1 ++, sp2 ++, dp1 ++, dp2 ++ )
								{
									dp1[ 0 ] =              sp1[ 0 ] ;
									dp2[ 0 ] = ( short )( ( sp2[ 0 ] * ( p - n ) + sp1[ 0 ] * n ) / p ) ;
								}

								if( offset1 + p * 2 < DSampleNum &&
									offset0 + p     < SSampleNum )
								{
									q = ( p * rateI * 512 / ( 512 - rateI ) + 256 ) / 512 ;
									sp1 = &SBuffer[ offset0 + p     ] ;
									dp1 = &DBuffer[ offset1 + p * 2 ] ;
									movnum = q - p ;
									if( offset0 + p + movnum > SSampleNum )
									{
										movnum = SSampleNum - ( offset0 + p ) ;
										q = p + movnum ;
									}
									if( offset1 + p * 2 + movnum > DSampleNum )
									{
										movnum = DSampleNum - ( offset1 + p * 2 ) ;
										q = p + movnum ;
									}
									for( n = 0 ; n < movnum ; n++, sp1 ++, dp1 ++ )
									{
										dp1[ 0 ] = sp1[ 0 ] ;
									}
								}
							}

							offset0 += q ;		// offset0の更新
							offset1 += p + q ;	// offset1の更新
						}

						while( offset1 < DSampleNum && offset0 < SSampleNum )
						{
							max_of_r = 0 ;
							p = pmin ;
							for( m = pmin ; m <= pmax ; m++ )
							{
								short *sp1 ;
								short *sp2 ;

								r = 0 ;
								sp1 = &SBuffer[ offset0     ] ;
								sp2 = &SBuffer[ offset0 + m ] ;
								for( n = 0 ; n < template_size ; n++, sp1 ++, sp2 ++ )
								{
									int sp1p ;
									int sp2p ;

									sp1p = offset0     + n >= SSampleNum ? 0 : sp1[ 0 ] ;
									sp2p = offset0 + m + n >= SSampleNum ? 0 : sp2[ 0 ] ;
									r += ( sp1p * sp2p ) / 32768 ; // 相関関数
								}
								if( r > max_of_r )
								{
									max_of_r = r ;	// 相関関数のピーク
									p = m ;			// 音データの基本周期
								}
							}

							{
								short *sp1, *dp1 ;
								short *sp2, *dp2 ;
								int movnum ;

								sp1 = &SBuffer[ offset0     ] ;
								sp2 = &SBuffer[ offset0 + p ] ;
								dp1 = &DBuffer[ offset1     ] ;
								dp2 = &DBuffer[ offset1 + p ] ;
								for( n = 0; n < p ; n++, sp1 ++, sp2 ++, dp1 ++, dp2 ++ )
								{
									short sp1p ;
									short sp2p ;

									sp1p = offset0     + n >= SSampleNum ? 0 : sp1[ 0 ] ;
									sp2p = offset0 + p + n >= SSampleNum ? 0 : sp2[ 0 ] ;
									if( offset1 + n < DSampleNum )
									{
										dp1[ 0 ] =              sp1p ;
									}
									if( offset1 + p + n < DSampleNum )
									{
										dp2[ 0 ] = ( short )( ( sp2p * ( p - n ) + sp1p * n ) / p ) ;
									}
								}

								if( offset1 + p * 2 < DSampleNum &&
									offset0 + p     < SSampleNum )
								{
									q = ( p * rateI * 512 / ( 512 - rateI ) + 256 ) / 512 ;
									sp1 = &SBuffer[ offset0 + p     ] ;
									dp1 = &DBuffer[ offset1 + p * 2 ] ;
									movnum = q - p ;
									if( offset0 + p + movnum > SSampleNum )
									{
										movnum = SSampleNum - ( offset0 + p ) ;
										q = p + movnum ;
									}
									if( offset1 + p * 2 + movnum > DSampleNum )
									{
										movnum = DSampleNum - ( offset1 + p * 2 ) ;
										q = p + movnum ;
									}
									for( n = 0 ; n < movnum ; n++, sp1 ++, dp1 ++ )
									{
										short sp1p ;

										sp1p = offset0 + p + n >= SSampleNum ? 0 : sp1[ 0 ] ;
										dp1[ 0 ] = sp1p ;
									}
								}
							}

							offset0 += q ;		// offset0の更新
							offset1 += p + q ;	// offset1の更新
						}
						if( offset1 < DSampleNum )
						{
							short *dp ;
							int loopnum ;

							loopnum = DSampleNum - offset1 ;
							dp = &DBuffer[ offset1 ] ;
							for( n = 0 ; n < loopnum ; n ++, dp ++ )
							{
								*dp = 0 ;
							}
						}
						break ;
					}
				}
				break ;

			case 8 :
				{
					BYTE *SBuffer ;
					BYTE *DBuffer ;
					int r ;
					int max_of_r;

					SBuffer = ( BYTE * )SSSound->Wave.Buffer ;
					DBuffer = ( BYTE * )DSSound->Wave.Buffer ;

					switch( SSSound->BufferFormat.nChannels )
					{
					case 2 :
						offset0 = 0 ;
						offset1 = 0 ;
						while( offset0 + pmax * 2 < SSampleNum && offset1 + pmax * 2 < DSampleNum )
						{
							max_of_r = 0 ;
							p = pmin ;
							for( m = pmin ; m <= pmax ; m++ )
							{
								BYTE *sp1 ;
								BYTE *sp2 ;

								r = 0 ;
								sp1 = &SBuffer[ ( offset0     ) * 2 ] ;
								sp2 = &SBuffer[ ( offset0 + m ) * 2 ] ;
								for( n = 0 ; n < template_size ; n++, sp1 += 2, sp2 += 2 )
								{
									r += ( ( ( int )sp1[ 0 ] - 128 ) * ( int )( sp2[ 0 ] - 128 ) ) / 128 ; // 相関関数
								}
								if( r > max_of_r )
								{
									max_of_r = r ;	// 相関関数のピーク
									p = m ;			// 音データの基本周期
								}
							}

							{
								BYTE *sp1, *dp1 ;
								BYTE *sp2, *dp2 ;
								int movnum ;

								sp1 = &SBuffer[ ( offset0     ) * 2 ] ;
								sp2 = &SBuffer[ ( offset0 + p ) * 2 ] ;
								dp1 = &DBuffer[ ( offset1     ) * 2 ] ;
								dp2 = &DBuffer[ ( offset1 + p ) * 2 ] ;
								for( n = 0; n < p ; n++, sp1 += 2, sp2 += 2, dp1 += 2, dp2 += 2 )
								{
									dp1[ 0 ] =             sp1[ 0 ] ;
									dp2[ 0 ] = ( BYTE )( ( sp2[ 0 ] * ( p - n ) + sp1[ 0 ] * n ) / p ) ;
								}

								if( offset1 + p * 2 < DSampleNum &&
									offset0 + p     < SSampleNum )
								{
									q = ( p * rateI * 512 / ( 512 - rateI ) + 256 ) / 512 ;
									sp1 = &SBuffer[ ( offset0 + p     ) * 2 ] ;
									dp1 = &DBuffer[ ( offset1 + p * 2 ) * 2 ] ;
									movnum = q - p ;
									if( offset0 + p + movnum > SSampleNum )
									{
										movnum = SSampleNum - ( offset0 + p ) ;
										q = p + movnum ;
									}
									if( offset1 + p * 2 + movnum > DSampleNum )
									{
										movnum = DSampleNum - ( offset1 + p * 2 ) ;
										q = p + movnum ;
									}
									for( n = 0 ; n < movnum ; n++, sp1 += 2, dp1 += 2 )
									{
										dp1[ 0 ] = sp1[ 0 ] ;
									}
								}
							}

							offset0 += q ;		// offset0の更新
							offset1 += p + q ;	// offset1の更新
						}

						while( offset1 < DSampleNum && offset0 < SSampleNum )
						{
							max_of_r = 0 ;
							p = pmin ;
							for( m = pmin ; m <= pmax ; m++ )
							{
								BYTE *sp1 ;
								BYTE *sp2 ;

								r = 0 ;
								sp1 = &SBuffer[ ( offset0     ) * 2 ] ;
								sp2 = &SBuffer[ ( offset0 + m ) * 2 ] ;
								for( n = 0 ; n < template_size ; n++, sp1 += 2, sp2 += 2 )
								{
									int sp1p ;
									int sp2p ;

									sp1p = offset0     + n >= SSampleNum ? 0 : ( int )sp1[ 0 ] - 128 ;
									sp2p = offset0 + m + n >= SSampleNum ? 0 : ( int )sp2[ 0 ] - 128 ;
									r += ( sp1p * sp2p ) / 128 ; // 相関関数
								}
								if( r > max_of_r )
								{
									max_of_r = r ;	// 相関関数のピーク
									p = m ;			// 音データの基本周期
								}
							}

							{
								BYTE *sp1, *dp1 ;
								BYTE *sp2, *dp2 ;
								int movnum ;

								sp1 = &SBuffer[ ( offset0     ) * 2 ] ;
								sp2 = &SBuffer[ ( offset0 + p ) * 2 ] ;
								dp1 = &DBuffer[ ( offset1     ) * 2 ] ;
								dp2 = &DBuffer[ ( offset1 + p ) * 2 ] ;
								for( n = 0; n < p ; n++, sp1 += 2, sp2 += 2, dp1 += 2, dp2 += 2 )
								{
									BYTE sp1p ;
									BYTE sp2p ;

									sp1p = offset0     + n >= SSampleNum ? 128 : sp1[ 0 ] ;
									sp2p = offset0 + p + n >= SSampleNum ? 128 : sp2[ 0 ] ;
									if( offset1 + n < DSampleNum )
									{
										dp1[ 0 ] =             sp1p ;
									}
									if( offset1 + p + n < DSampleNum )
									{
										dp2[ 0 ] = ( BYTE )( ( sp2p * ( p - n ) + sp1p * n ) / p ) ;
									}
								}

								if( offset1 + p * 2 < DSampleNum &&
									offset0 + p     < SSampleNum )
								{
									q = ( p * rateI * 512 / ( 512 - rateI ) + 256 ) / 512 ;
									sp1 = &SBuffer[ ( offset0 + p     ) * 2 ] ;
									dp1 = &DBuffer[ ( offset1 + p * 2 ) * 2 ] ;
									movnum = q - p ;
									if( offset0 + p + movnum > SSampleNum )
									{
										movnum = SSampleNum - ( offset0 + p ) ;
										q = p + movnum ;
									}
									if( offset1 + p * 2 + movnum > DSampleNum )
									{
										movnum = DSampleNum - ( offset1 + p * 2 ) ;
										q = p + movnum ;
									}
									for( n = 0 ; n < movnum ; n++, sp1 += 2, dp1 += 2 )
									{
										BYTE sp1p ;

										sp1p = offset0 + p + n >= SSampleNum ? 128 : sp1[ 0 ] ;
										dp1[ 0 ] = sp1p ;
									}
								}
							}

							offset0 += q ;		// offset0の更新
							offset1 += p + q ;	// offset1の更新
						}
						if( offset1 < DSampleNum )
						{
							BYTE *dp ;
							int loopnum ;

							loopnum = DSampleNum - offset1 ;
							dp = &DBuffer[ offset1 * 2 ] ;
							for( n = 0 ; n < loopnum ; n ++, dp += 2 )
							{
								*dp = 128 ;
							}
						}



						offset0 = 0 ;
						offset1 = 0 ;
						while( offset0 + pmax * 2 < SSampleNum && offset1 + pmax * 2 < DSampleNum )
						{
							max_of_r = 0 ;
							p = pmin ;
							for( m = pmin ; m <= pmax ; m++ )
							{
								BYTE *sp1 ;
								BYTE *sp2 ;

								r = 0 ;
								sp1 = &SBuffer[ ( offset0     ) * 2 + 1 ] ;
								sp2 = &SBuffer[ ( offset0 + m ) * 2 + 1 ] ;
								for( n = 0 ; n < template_size ; n++, sp1 += 2, sp2 += 2 )
								{
									r += ( ( ( int )sp1[ 0 ] - 128 ) * ( int )( sp2[ 0 ] - 128 ) ) / 128 ; // 相関関数
								}
								if( r > max_of_r )
								{
									max_of_r = r ;	// 相関関数のピーク
									p = m ;			// 音データの基本周期
								}
							}

							{
								BYTE *sp1, *dp1 ;
								BYTE *sp2, *dp2 ;
								int movnum ;

								sp1 = &SBuffer[ ( offset0     ) * 2 + 1 ] ;
								sp2 = &SBuffer[ ( offset0 + p ) * 2 + 1 ] ;
								dp1 = &DBuffer[ ( offset1     ) * 2 + 1 ] ;
								dp2 = &DBuffer[ ( offset1 + p ) * 2 + 1 ] ;
								for( n = 0; n < p ; n++, sp1 += 2, sp2 += 2, dp1 += 2, dp2 += 2 )
								{
									dp1[ 0 ] =             sp1[ 0 ] ;
									dp2[ 0 ] = ( BYTE )( ( sp2[ 0 ] * ( p - n ) + sp1[ 0 ] * n ) / p ) ;
								}

								if( offset1 + p * 2 < DSampleNum &&
									offset0 + p     < SSampleNum )
								{
									q = ( p * rateI * 512 / ( 512 - rateI ) + 256 ) / 512 ;
									sp1 = &SBuffer[ ( offset0 + p     ) * 2 + 1 ] ;
									dp1 = &DBuffer[ ( offset1 + p * 2 ) * 2 + 1 ] ;
									movnum = q - p ;
									if( offset0 + p + movnum > SSampleNum )
									{
										movnum = SSampleNum - ( offset0 + p ) ;
										q = p + movnum ;
									}
									if( offset1 + p * 2 + movnum > DSampleNum )
									{
										movnum = DSampleNum - ( offset1 + p * 2 ) ;
										q = p + movnum ;
									}
									for( n = 0 ; n < movnum ; n++, sp1 += 2, dp1 += 2 )
									{
										dp1[ 0 ] = sp1[ 0 ] ;
									}
								}
							}

							offset0 += q ;		// offset0の更新
							offset1 += p + q ;	// offset1の更新
						}

						while( offset1 < DSampleNum && offset0 < SSampleNum )
						{
							max_of_r = 0 ;
							p = pmin ;
							for( m = pmin ; m <= pmax ; m++ )
							{
								BYTE *sp1 ;
								BYTE *sp2 ;

								r = 0 ;
								sp1 = &SBuffer[ ( offset0     ) * 2 + 1 ] ;
								sp2 = &SBuffer[ ( offset0 + m ) * 2 + 1 ] ;
								for( n = 0 ; n < template_size ; n++, sp1 += 2, sp2 += 2 )
								{
									int sp1p ;
									int sp2p ;

									sp1p = offset0     + n >= SSampleNum ? 0 : ( int )sp1[ 0 ] - 128 ;
									sp2p = offset0 + m + n >= SSampleNum ? 0 : ( int )sp2[ 0 ] - 128 ;
									r += ( sp1p * sp2p ) / 128 ; // 相関関数
								}
								if( r > max_of_r )
								{
									max_of_r = r ;	// 相関関数のピーク
									p = m ;			// 音データの基本周期
								}
							}

							{
								BYTE *sp1, *dp1 ;
								BYTE *sp2, *dp2 ;
								int movnum ;

								sp1 = &SBuffer[ ( offset0     ) * 2 + 1 ] ;
								sp2 = &SBuffer[ ( offset0 + p ) * 2 + 1 ] ;
								dp1 = &DBuffer[ ( offset1     ) * 2 + 1 ] ;
								dp2 = &DBuffer[ ( offset1 + p ) * 2 + 1 ] ;
								for( n = 0; n < p ; n++, sp1 += 2, sp2 += 2, dp1 += 2, dp2 += 2 )
								{
									BYTE sp1p ;
									BYTE sp2p ;

									sp1p = offset0     + n >= SSampleNum ? 128 : sp1[ 0 ] ;
									sp2p = offset0 + p + n >= SSampleNum ? 128 : sp2[ 0 ] ;
									if( offset1 + n < DSampleNum )
									{
										dp1[ 0 ] =             sp1p ;
									}
									if( offset1 + p + n < DSampleNum )
									{
										dp2[ 0 ] = ( BYTE )( ( sp2p * ( p - n ) + sp1p * n ) / p ) ;
									}
								}

								if( offset1 + p * 2 < DSampleNum &&
									offset0 + p     < SSampleNum )
								{
									q = ( p * rateI * 512 / ( 512 - rateI ) + 256 ) / 512 ;
									sp1 = &SBuffer[ ( offset0 + p     ) * 2 + 1 ] ;
									dp1 = &DBuffer[ ( offset1 + p * 2 ) * 2 + 1 ] ;
									movnum = q - p ;
									if( offset0 + p + movnum > SSampleNum )
									{
										movnum = SSampleNum - ( offset0 + p ) ;
										q = p + movnum ;
									}
									if( offset1 + p * 2 + movnum > DSampleNum )
									{
										movnum = DSampleNum - ( offset1 + p * 2 ) ;
										q = p + movnum ;
									}
									for( n = 0 ; n < movnum ; n++, sp1 += 2, dp1 += 2 )
									{
										BYTE sp1p ;

										sp1p = offset0 + p + n >= SSampleNum ? 128 : sp1[ 0 ] ;
										dp1[ 0 ] = sp1p ;
									}
								}
							}

							offset0 += q ;		// offset0の更新
							offset1 += p + q ;	// offset1の更新
						}
						if( offset1 < DSampleNum )
						{
							BYTE *dp ;
							int loopnum ;

							loopnum = DSampleNum - offset1 ;
							dp = &DBuffer[ offset1 * 2 + 1 ] ;
							for( n = 0 ; n < loopnum ; n ++, dp += 2 )
							{
								*dp = 128 ;
							}
						}
						break ;

					case 1 :
						offset0 = 0 ;
						offset1 = 0 ;
						while( offset0 + pmax * 2 < SSampleNum && offset1 + pmax * 2 < DSampleNum )
						{
							max_of_r = 0 ;
							p = pmin ;
							for( m = pmin ; m <= pmax ; m++ )
							{
								BYTE *sp1 ;
								BYTE *sp2 ;

								r = 0 ;
								sp1 = &SBuffer[ offset0     ] ;
								sp2 = &SBuffer[ offset0 + m ] ;
								for( n = 0 ; n < template_size ; n++, sp1 ++, sp2 ++ )
								{
									r += ( ( ( int )sp1[ 0 ] - 128 ) * ( int )( sp2[ 0 ] - 128 ) ) / 128 ; // 相関関数
								}
								if( r > max_of_r )
								{
									max_of_r = r ;	// 相関関数のピーク
									p = m ;			// 音データの基本周期
								}
							}

							{
								BYTE *sp1, *dp1 ;
								BYTE *sp2, *dp2 ;
								int movnum ;

								sp1 = &SBuffer[ offset0     ] ;
								sp2 = &SBuffer[ offset0 + p ] ;
								dp1 = &DBuffer[ offset1     ] ;
								dp2 = &DBuffer[ offset1 + p ] ;
								for( n = 0; n < p ; n++, sp1 ++, sp2 ++, dp1 ++, dp2 ++ )
								{
									dp1[ 0 ] =             sp1[ 0 ] ;
									dp2[ 0 ] = ( BYTE )( ( sp2[ 0 ] * ( p - n ) + sp1[ 0 ] * n ) / p ) ;
								}

								if( offset1 + p * 2 < DSampleNum &&
									offset0 + p     < SSampleNum )
								{
									q = ( p * rateI * 512 / ( 512 - rateI ) + 256 ) / 512 ;
									sp1 = &SBuffer[ offset0 + p     ] ;
									dp1 = &DBuffer[ offset1 + p * 2 ] ;
									movnum = q - p ;
									if( offset0 + p + movnum > SSampleNum )
									{
										movnum = SSampleNum - ( offset0 + p ) ;
										q = p + movnum ;
									}
									if( offset1 + p * 2 + movnum > DSampleNum )
									{
										movnum = DSampleNum - ( offset1 + p * 2 ) ;
										q = p + movnum ;
									}
									for( n = 0 ; n < movnum ; n++, sp1 ++, dp1 ++ )
									{
										dp1[ 0 ] = sp1[ 0 ] ;
									}
								}
							}

							offset0 += q ;		// offset0の更新
							offset1 += p + q ;	// offset1の更新
						}

						while( offset1 < DSampleNum && offset0 < SSampleNum )
						{
							max_of_r = 0 ;
							p = pmin ;
							for( m = pmin ; m <= pmax ; m++ )
							{
								BYTE *sp1 ;
								BYTE *sp2 ;

								r = 0 ;
								sp1 = &SBuffer[ offset0     ] ;
								sp2 = &SBuffer[ offset0 + m ] ;
								for( n = 0 ; n < template_size ; n++, sp1 ++, sp2 ++ )
								{
									int sp1p ;
									int sp2p ;

									sp1p = offset0     + n >= SSampleNum ? 0 : ( int )sp1[ 0 ] - 128 ;
									sp2p = offset0 + m + n >= SSampleNum ? 0 : ( int )sp2[ 0 ] - 128 ;
									r += ( sp1p * sp2p ) / 128 ; // 相関関数
								}
								if( r > max_of_r )
								{
									max_of_r = r ;	// 相関関数のピーク
									p = m ;			// 音データの基本周期
								}
							}

							{
								BYTE *sp1, *dp1 ;
								BYTE *sp2, *dp2 ;
								int movnum ;

								sp1 = &SBuffer[ offset0     ] ;
								sp2 = &SBuffer[ offset0 + p ] ;
								dp1 = &DBuffer[ offset1     ] ;
								dp2 = &DBuffer[ offset1 + p ] ;
								for( n = 0; n < p ; n++, sp1 ++, sp2 ++, dp1 ++, dp2 ++ )
								{
									BYTE sp1p ;
									BYTE sp2p ;

									sp1p = offset0     + n >= SSampleNum ? 128 : sp1[ 0 ] ;
									sp2p = offset0 + p + n >= SSampleNum ? 128 : sp2[ 0 ] ;
									if( offset1 + n < DSampleNum )
									{
										dp1[ 0 ] =             sp1p ;
									}
									if( offset1 + p + n < DSampleNum )
									{
										dp2[ 0 ] = ( BYTE )( ( sp2p * ( p - n ) + sp1p * n ) / p ) ;
									}
								}

								if( offset1 + p * 2 < DSampleNum &&
									offset0 + p     < SSampleNum )
								{
									q = ( p * rateI * 512 / ( 512 - rateI ) + 256 ) / 512 ;
									sp1 = &SBuffer[ offset0 + p     ] ;
									dp1 = &DBuffer[ offset1 + p * 2 ] ;
									movnum = q - p ;
									if( offset0 + p + movnum > SSampleNum )
									{
										movnum = SSampleNum - ( offset0 + p ) ;
										q = p + movnum ;
									}
									if( offset1 + p * 2 + movnum > DSampleNum )
									{
										movnum = DSampleNum - ( offset1 + p * 2 ) ;
										q = p + movnum ;
									}
									for( n = 0 ; n < movnum ; n++, sp1 ++, dp1 ++ )
									{
										BYTE sp1p ;

										sp1p = offset0 + p + n >= SSampleNum ? 128 : sp1[ 0 ] ;
										dp1[ 0 ] = sp1p ;
									}
								}
							}

							offset0 += q ;		// offset0の更新
							offset1 += p + q ;	// offset1の更新
						}
						if( offset1 < DSampleNum )
						{
							BYTE *dp ;
							int loopnum ;

							loopnum = DSampleNum - offset1 ;
							dp = &DBuffer[ offset1 ] ;
							for( n = 0 ; n < loopnum ; n ++, dp ++ )
							{
								*dp = 128 ;
							}
						}
						break ;
					}
				}
				break ;
			}
			break ;

		case WAVE_FORMAT_IEEE_FLOAT :
			{
				float *SBuffer ;
				float *DBuffer ;
				float r ;
				float max_of_r;

				SBuffer = ( float * )SSSound->Wave.Buffer ;
				DBuffer = ( float * )DSSound->Wave.Buffer ;

				switch( SSSound->BufferFormat.nChannels )
				{
				case 2 :
					offset0 = 0 ;
					offset1 = 0 ;
					while( offset0 + pmax * 2 < SSampleNum && offset1 + pmax * 2 < DSampleNum )
					{
						max_of_r = 0.0f ;
						p = pmin ;
						for( m = pmin ; m <= pmax ; m++ )
						{
							float *sp1 ;
							float *sp2 ;

							r = 0.0f ;
							sp1 = &SBuffer[ ( offset0     ) * 2 ] ;
							sp2 = &SBuffer[ ( offset0 + m ) * 2 ] ;
							for( n = 0 ; n < template_size ; n++, sp1 += 2, sp2 += 2 )
							{
								r += sp1[ 0 ] * sp2[ 0 ] ; // 相関関数
							}
							if( r > max_of_r )
							{
								max_of_r = r ;	// 相関関数のピーク
								p = m ;			// 音データの基本周期
							}
						}

						{
							float *sp1, *dp1 ;
							float *sp2, *dp2 ;
							int movnum ;

							sp1 = &SBuffer[ ( offset0     ) * 2 ] ;
							sp2 = &SBuffer[ ( offset0 + p ) * 2 ] ;
							dp1 = &DBuffer[ ( offset1     ) * 2 ] ;
							dp2 = &DBuffer[ ( offset1 + p ) * 2 ] ;
							for( n = 0; n < p ; n++, sp1 += 2, sp2 += 2, dp1 += 2, dp2 += 2 )
							{
								dp1[ 0 ] =   sp1[ 0 ] ;
								dp2[ 0 ] = ( sp2[ 0 ] * ( p - n ) + sp1[ 0 ] * n ) / p ;
							}

							if( offset1 + p * 2 < DSampleNum &&
								offset0 + p     < SSampleNum )
							{
								q = ( p * rateI * 512 / ( 512 - rateI ) + 256 ) / 512 ;
								sp1 = &SBuffer[ ( offset0 + p     ) * 2 ] ;
								dp1 = &DBuffer[ ( offset1 + p * 2 ) * 2 ] ;
								movnum = q - p ;
								if( offset0 + p + movnum > SSampleNum )
								{
									movnum = SSampleNum - ( offset0 + p ) ;
									q = p + movnum ;
								}
								if( offset1 + p * 2 + movnum > DSampleNum )
								{
									movnum = DSampleNum - ( offset1 + p * 2 ) ;
									q = p + movnum ;
								}
								for( n = 0 ; n < movnum ; n++, sp1 += 2, dp1 += 2 )
								{
									dp1[ 0 ] = sp1[ 0 ] ;
								}
							}
						}

						offset0 += q ;		// offset0の更新
						offset1 += p + q ;	// offset1の更新
					}

					while( offset1 < DSampleNum && offset0 < SSampleNum )
					{
						max_of_r = 0.0f ;
						p = pmin ;
						for( m = pmin ; m <= pmax ; m++ )
						{
							float *sp1 ;
							float *sp2 ;

							r = 0.0f ;
							sp1 = &SBuffer[ ( offset0     ) * 2 ] ;
							sp2 = &SBuffer[ ( offset0 + m ) * 2 ] ;
							for( n = 0 ; n < template_size ; n++, sp1 += 2, sp2 += 2 )
							{
								float sp1p ;
								float sp2p ;

								sp1p = offset0     + n >= SSampleNum ? 0.0f : sp1[ 0 ] ;
								sp2p = offset0 + m + n >= SSampleNum ? 0.0f : sp2[ 0 ] ;
								r += sp1p * sp2p ; // 相関関数
							}
							if( r > max_of_r )
							{
								max_of_r = r ;	// 相関関数のピーク
								p = m ;			// 音データの基本周期
							}
						}

						{
							float *sp1, *dp1 ;
							float *sp2, *dp2 ;
							int movnum ;

							sp1 = &SBuffer[ ( offset0     ) * 2 ] ;
							sp2 = &SBuffer[ ( offset0 + p ) * 2 ] ;
							dp1 = &DBuffer[ ( offset1     ) * 2 ] ;
							dp2 = &DBuffer[ ( offset1 + p ) * 2 ] ;
							for( n = 0; n < p ; n++, sp1 += 2, sp2 += 2, dp1 += 2, dp2 += 2 )
							{
								float sp1p ;
								float sp2p ;

								sp1p = offset0     + n >= SSampleNum ? 0.0f : sp1[ 0 ] ;
								sp2p = offset0 + p + n >= SSampleNum ? 0.0f : sp2[ 0 ] ;
								if( offset1 + n < DSampleNum )
								{
									dp1[ 0 ] =   sp1p ;
								}
								if( offset1 + p + n < DSampleNum )
								{
									dp2[ 0 ] = ( sp2p * ( p - n ) + sp1p * n ) / p ;
								}
							}

							if( offset1 + p * 2 < DSampleNum &&
								offset0 + p     < SSampleNum )
							{
								q = ( p * rateI * 512 / ( 512 - rateI ) + 256 ) / 512 ;
								sp1 = &SBuffer[ ( offset0 + p     ) * 2 ] ;
								dp1 = &DBuffer[ ( offset1 + p * 2 ) * 2 ] ;
								movnum = q - p ;
								if( offset0 + p + movnum > SSampleNum )
								{
									movnum = SSampleNum - ( offset0 + p ) ;
									q = p + movnum ;
								}
								if( offset1 + p * 2 + movnum > DSampleNum )
								{
									movnum = DSampleNum - ( offset1 + p * 2 ) ;
									q = p + movnum ;
								}
								for( n = 0 ; n < movnum ; n++, sp1 += 2, dp1 += 2 )
								{
									float sp1p ;

									sp1p = offset0 + p + n >= SSampleNum ? 0.0f : sp1[ 0 ] ;
									dp1[ 0 ] = sp1p ;
								}
							}
						}

						offset0 += q ;		// offset0の更新
						offset1 += p + q ;	// offset1の更新
					}
					if( offset1 < DSampleNum )
					{
						float *dp ;
						int loopnum ;

						loopnum = DSampleNum - offset1 ;
						dp = &DBuffer[ offset1 * 2 ] ;
						for( n = 0 ; n < loopnum ; n ++, dp += 2 )
						{
							*dp = 0.0f ;
						}
					}



					offset0 = 0 ;
					offset1 = 0 ;
					while( offset0 + pmax * 2 < SSampleNum && offset1 + pmax * 2 < DSampleNum )
					{
						max_of_r = 0.0f ;
						p = pmin ;
						for( m = pmin ; m <= pmax ; m++ )
						{
							float *sp1 ;
							float *sp2 ;

							r = 0.0f ;
							sp1 = &SBuffer[ ( offset0     ) * 2 + 1 ] ;
							sp2 = &SBuffer[ ( offset0 + m ) * 2 + 1 ] ;
							for( n = 0 ; n < template_size ; n++, sp1 += 2, sp2 += 2 )
							{
								r += sp1[ 0 ] * sp2[ 0 ] ; // 相関関数
							}
							if( r > max_of_r )
							{
								max_of_r = r ;	// 相関関数のピーク
								p = m ;			// 音データの基本周期
							}
						}

						{
							float *sp1, *dp1 ;
							float *sp2, *dp2 ;
							int movnum ;

							sp1 = &SBuffer[ ( offset0     ) * 2 + 1 ] ;
							sp2 = &SBuffer[ ( offset0 + p ) * 2 + 1 ] ;
							dp1 = &DBuffer[ ( offset1     ) * 2 + 1 ] ;
							dp2 = &DBuffer[ ( offset1 + p ) * 2 + 1 ] ;
							for( n = 0; n < p ; n++, sp1 += 2, sp2 += 2, dp1 += 2, dp2 += 2 )
							{
								dp1[ 0 ] =   sp1[ 0 ] ;
								dp2[ 0 ] = ( sp2[ 0 ] * ( p - n ) + sp1[ 0 ] * n ) / p ;
							}

							if( offset1 + p * 2 < DSampleNum &&
								offset0 + p     < SSampleNum )
							{
								q = ( p * rateI * 512 / ( 512 - rateI ) + 256 ) / 512 ;
								sp1 = &SBuffer[ ( offset0 + p     ) * 2 + 1 ] ;
								dp1 = &DBuffer[ ( offset1 + p * 2 ) * 2 + 1 ] ;
								movnum = q - p ;
								if( offset0 + p + movnum > SSampleNum )
								{
									movnum = SSampleNum - ( offset0 + p ) ;
									q = p + movnum ;
								}
								if( offset1 + p * 2 + movnum > DSampleNum )
								{
									movnum = DSampleNum - ( offset1 + p * 2 ) ;
									q = p + movnum ;
								}
								for( n = 0 ; n < movnum ; n++, sp1 += 2, dp1 += 2 )
								{
									dp1[ 0 ] = sp1[ 0 ] ;
								}
							}
						}

						offset0 += q ;		// offset0の更新
						offset1 += p + q ;	// offset1の更新
					}

					while( offset1 < DSampleNum && offset0 < SSampleNum )
					{
						max_of_r = 0.0f ;
						p = pmin ;
						for( m = pmin ; m <= pmax ; m++ )
						{
							float *sp1 ;
							float *sp2 ;

							r = 0.0f ;
							sp1 = &SBuffer[ ( offset0     ) * 2 + 1 ] ;
							sp2 = &SBuffer[ ( offset0 + m ) * 2 + 1 ] ;
							for( n = 0 ; n < template_size ; n++, sp1 += 2, sp2 += 2 )
							{
								float sp1p ;
								float sp2p ;

								sp1p = offset0     + n >= SSampleNum ? 0.0f : sp1[ 0 ] ;
								sp2p = offset0 + m + n >= SSampleNum ? 0.0f : sp2[ 0 ] ;
								r += sp1p * sp2p ; // 相関関数
							}
							if( r > max_of_r )
							{
								max_of_r = r ;	// 相関関数のピーク
								p = m ;			// 音データの基本周期
							}
						}

						{
							float *sp1, *dp1 ;
							float *sp2, *dp2 ;
							int movnum ;

							sp1 = &SBuffer[ ( offset0     ) * 2 + 1 ] ;
							sp2 = &SBuffer[ ( offset0 + p ) * 2 + 1 ] ;
							dp1 = &DBuffer[ ( offset1     ) * 2 + 1 ] ;
							dp2 = &DBuffer[ ( offset1 + p ) * 2 + 1 ] ;
							for( n = 0; n < p ; n++, sp1 += 2, sp2 += 2, dp1 += 2, dp2 += 2 )
							{
								float sp1p ;
								float sp2p ;

								sp1p = offset0     + n >= SSampleNum ? 0 : sp1[ 0 ] ;
								sp2p = offset0 + p + n >= SSampleNum ? 0 : sp2[ 0 ] ;
								if( offset1 + n < DSampleNum )
								{
									dp1[ 0 ] =   sp1p ;
								}
								if( offset1 + p + n < DSampleNum )
								{
									dp2[ 0 ] = ( sp2p * ( p - n ) + sp1p * n ) / p ;
								}
							}

							if( offset1 + p * 2 < DSampleNum &&
								offset0 + p     < SSampleNum )
							{
								q = ( p * rateI * 512 / ( 512 - rateI ) + 256 ) / 512 ;
								sp1 = &SBuffer[ ( offset0 + p     ) * 2 + 1 ] ;
								dp1 = &DBuffer[ ( offset1 + p * 2 ) * 2 + 1 ] ;
								movnum = q - p ;
								if( offset0 + p + movnum > SSampleNum )
								{
									movnum = SSampleNum - ( offset0 + p ) ;
									q = p + movnum ;
								}
								if( offset1 + p * 2 + movnum > DSampleNum )
								{
									movnum = DSampleNum - ( offset1 + p * 2 ) ;
									q = p + movnum ;
								}
								for( n = 0 ; n < movnum ; n++, sp1 += 2, dp1 += 2 )
								{
									float sp1p ;

									sp1p = offset0 + p + n >= SSampleNum ? 0 : sp1[ 0 ] ;
									dp1[ 0 ] = sp1p ;
								}
							}
						}

						offset0 += q ;		// offset0の更新
						offset1 += p + q ;	// offset1の更新
					}
					if( offset1 < DSampleNum )
					{
						float *dp ;
						int loopnum ;

						loopnum = DSampleNum - offset1 ;
						dp = &DBuffer[ offset1 * 2 + 1 ] ;
						for( n = 0 ; n < loopnum ; n ++, dp += 2 )
						{
							*dp = 0.0f ;
						}
					}
					break ;

				case 1 :
					offset0 = 0 ;
					offset1 = 0 ;
					while( offset0 + pmax * 2 < SSampleNum && offset1 + pmax * 2 < DSampleNum )
					{
						max_of_r = 0.0f ;
						p = pmin ;
						for( m = pmin ; m <= pmax ; m++ )
						{
							float *sp1 ;
							float *sp2 ;

							r = 0.0f ;
							sp1 = &SBuffer[ offset0     ] ;
							sp2 = &SBuffer[ offset0 + m ] ;
							for( n = 0 ; n < template_size ; n++, sp1 ++, sp2 ++ )
							{
								r += sp1[ 0 ] * sp2[ 0 ] ; // 相関関数
							}
							if( r > max_of_r )
							{
								max_of_r = r ;	// 相関関数のピーク
								p = m ;			// 音データの基本周期
							}
						}

						{
							float *sp1, *dp1 ;
							float *sp2, *dp2 ;
							int movnum ;

							sp1 = &SBuffer[ offset0     ] ;
							sp2 = &SBuffer[ offset0 + p ] ;
							dp1 = &DBuffer[ offset1     ] ;
							dp2 = &DBuffer[ offset1 + p ] ;
							for( n = 0; n < p ; n++, sp1 ++, sp2 ++, dp1 ++, dp2 ++ )
							{
								dp1[ 0 ] =   sp1[ 0 ] ;
								dp2[ 0 ] = ( sp2[ 0 ] * ( p - n ) + sp1[ 0 ] * n ) / p ;
							}

							if( offset1 + p * 2 < DSampleNum &&
								offset0 + p     < SSampleNum )
							{
								q = ( p * rateI * 512 / ( 512 - rateI ) + 256 ) / 512 ;
								sp1 = &SBuffer[ offset0 + p     ] ;
								dp1 = &DBuffer[ offset1 + p * 2 ] ;
								movnum = q - p ;
								if( offset0 + p + movnum > SSampleNum )
								{
									movnum = SSampleNum - ( offset0 + p ) ;
									q = p + movnum ;
								}
								if( offset1 + p * 2 + movnum > DSampleNum )
								{
									movnum = DSampleNum - ( offset1 + p * 2 ) ;
									q = p + movnum ;
								}
								for( n = 0 ; n < movnum ; n++, sp1 ++, dp1 ++ )
								{
									dp1[ 0 ] = sp1[ 0 ] ;
								}
							}
						}

						offset0 += q ;		// offset0の更新
						offset1 += p + q ;	// offset1の更新
					}

					while( offset1 < DSampleNum && offset0 < SSampleNum )
					{
						max_of_r = 0.0f ;
						p = pmin ;
						for( m = pmin ; m <= pmax ; m++ )
						{
							float *sp1 ;
							float *sp2 ;

							r = 0.0f ;
							sp1 = &SBuffer[ offset0     ] ;
							sp2 = &SBuffer[ offset0 + m ] ;
							for( n = 0 ; n < template_size ; n++, sp1 ++, sp2 ++ )
							{
								float sp1p ;
								float sp2p ;

								sp1p = offset0     + n >= SSampleNum ? 0.0f : sp1[ 0 ] ;
								sp2p = offset0 + m + n >= SSampleNum ? 0.0f : sp2[ 0 ] ;
								r += sp1p * sp2p ; // 相関関数
							}
							if( r > max_of_r )
							{
								max_of_r = r ;	// 相関関数のピーク
								p = m ;			// 音データの基本周期
							}
						}

						{
							float *sp1, *dp1 ;
							float *sp2, *dp2 ;
							int movnum ;

							sp1 = &SBuffer[ offset0     ] ;
							sp2 = &SBuffer[ offset0 + p ] ;
							dp1 = &DBuffer[ offset1     ] ;
							dp2 = &DBuffer[ offset1 + p ] ;
							for( n = 0; n < p ; n++, sp1 ++, sp2 ++, dp1 ++, dp2 ++ )
							{
								float sp1p ;
								float sp2p ;

								sp1p = offset0     + n >= SSampleNum ? 0.0f : sp1[ 0 ] ;
								sp2p = offset0 + p + n >= SSampleNum ? 0.0f : sp2[ 0 ] ;
								if( offset1 + n < DSampleNum )
								{
									dp1[ 0 ] =   sp1p ;
								}
								if( offset1 + p + n < DSampleNum )
								{
									dp2[ 0 ] = ( sp2p * ( p - n ) + sp1p * n ) / p ;
								}
							}

							if( offset1 + p * 2 < DSampleNum &&
								offset0 + p     < SSampleNum )
							{
								q = ( p * rateI * 512 / ( 512 - rateI ) + 256 ) / 512 ;
								sp1 = &SBuffer[ offset0 + p     ] ;
								dp1 = &DBuffer[ offset1 + p * 2 ] ;
								movnum = q - p ;
								if( offset0 + p + movnum > SSampleNum )
								{
									movnum = SSampleNum - ( offset0 + p ) ;
									q = p + movnum ;
								}
								if( offset1 + p * 2 + movnum > DSampleNum )
								{
									movnum = DSampleNum - ( offset1 + p * 2 ) ;
									q = p + movnum ;
								}
								for( n = 0 ; n < movnum ; n++, sp1 ++, dp1 ++ )
								{
									float sp1p ;

									sp1p = offset0 + p + n >= SSampleNum ? 0.0f : sp1[ 0 ] ;
									dp1[ 0 ] = sp1p ;
								}
							}
						}

						offset0 += q ;		// offset0の更新
						offset1 += p + q ;	// offset1の更新
					}
					if( offset1 < DSampleNum )
					{
						float *dp ;
						int loopnum ;

						loopnum = DSampleNum - offset1 ;
						dp = &DBuffer[ offset1 ] ;
						for( n = 0 ; n < loopnum ; n ++, dp ++ )
						{
							*dp = 0.0f ;
						}
					}
					break ;
				}
			}
			break ;
		}
	}
	else
	{
		int n, m, template_size, pmin, pmax, p, q = 0, offset0, offset1 ;
		double rate ;
		int rateI ;
		int SSampleNum ;
		int DSampleNum ;

		rate = ( double )SSSound->Wave.BufferSampleNum / DSSound->Wave.BufferSampleNum ; // rateの範囲は0.5<=rate<1.0
		rateI = _DTOL( rate * 512.0 ) ;
		rate = rateI / 512.0 ;

		SSampleNum = SSSound->Wave.BufferSampleNum ;
		DSampleNum = DSSound->Wave.BufferSampleNum ;

		template_size = _DTOL( SSSound->BufferFormat.nSamplesPerSec * 0.01  ) ; // 10ms
		pmin          = _DTOL( SSSound->BufferFormat.nSamplesPerSec * 0.005 ) ; // 5ms
		pmax          = _DTOL( SSSound->BufferFormat.nSamplesPerSec * 0.02  ) ; // 20ms

		switch( SSSound->BufferFormat.wFormatTag )
		{
		case WAVE_FORMAT_PCM :

			switch( SSSound->BufferFormat.wBitsPerSample )
			{
			case 16 :
				{
					short *SBuffer ;
					short *DBuffer ;
					int r ;
					int max_of_r;

					SBuffer = ( short * )SSSound->Wave.Buffer ;
					DBuffer = ( short * )DSSound->Wave.Buffer ;

					switch( SSSound->BufferFormat.nChannels )
					{
					case 2 :
						offset0 = 0 ;
						offset1 = 0 ;
						while( offset0 + pmax * 2 < SSampleNum && offset1 + pmax * 2 < DSampleNum )
						{
							max_of_r = 0 ;
							p = pmin ;
							for( m = pmin ; m <= pmax ; m++ )
							{
								short *sp1 ;
								short *sp2 ;

								r = 0 ;
								sp1 = &SBuffer[ ( offset0     ) * 2 ] ;
								sp2 = &SBuffer[ ( offset0 + m ) * 2 ] ;
								for( n = 0 ; n < template_size ; n++, sp1 += 2, sp2 += 2 )
								{
									r += ( sp1[ 0 ] * sp2[ 0 ] ) / 32768 ; // 相関関数
								}
								if( r > max_of_r )
								{
									max_of_r = r ;	// 相関関数のピーク
									p = m ;			// 音データの基本周期
								}
							}

							{
								short *sp1, *dp1 ;
								short *sp2 ;
								int movnum ;

								sp1 = &SBuffer[ ( offset0     ) * 2 ] ;
								sp2 = &SBuffer[ ( offset0 + p ) * 2 ] ;
								dp1 = &DBuffer[ ( offset1     ) * 2 ] ;
								for( n = 0; n < p ; n++, sp1 += 2, sp2 += 2, dp1 += 2 )
								{
									dp1[ 0 ] = ( short )( ( sp1[ 0 ] * ( p - n ) + sp2[ 0 ] * n ) / p ) ;
								}

								if( offset1 + p     < DSampleNum &&
									offset0 + p * 2 < SSampleNum )
								{
									q = ( p * 512 * 512 / ( rateI - 512 ) + 256 ) / 512 ;
									sp1 = &SBuffer[ ( offset0 + p * 2 ) * 2 ] ;
									dp1 = &DBuffer[ ( offset1 + p     ) * 2 ] ;
									movnum = q - p ;
									if( offset0 + p * 2 + movnum > SSampleNum )
									{
										movnum = SSampleNum - ( offset0 + p * 2 ) ;
										q = p + movnum ;
									}
									if( offset1 + p + movnum > DSampleNum )
									{
										movnum = DSampleNum - ( offset1 + p ) ;
										q = p + movnum ;
									}
									for( n = 0 ; n < movnum ; n++, sp1 += 2, dp1 += 2 )
									{
										dp1[ 0 ] = sp1[ 0 ] ;
									}
								}
							}

							offset0 += p + q ;		// offset0の更新
							offset1 += q ;	// offset1の更新
						}

						while( offset1 < DSampleNum && offset0 < SSampleNum )
						{
							max_of_r = 0 ;
							p = pmin ;
							for( m = pmin ; m <= pmax ; m++ )
							{
								short *sp1 ;
								short *sp2 ;

								r = 0 ;
								sp1 = &SBuffer[ ( offset0     ) * 2 ] ;
								sp2 = &SBuffer[ ( offset0 + m ) * 2 ] ;
								for( n = 0 ; n < template_size ; n++, sp1 += 2, sp2 += 2 )
								{
									int sp1p ;
									int sp2p ;

									sp1p = offset0     + n >= SSampleNum ? 0 : sp1[ 0 ] ;
									sp2p = offset0 + m + n >= SSampleNum ? 0 : sp2[ 0 ] ;
									r += ( sp1p * sp2p ) / 32768 ; // 相関関数
								}
								if( r > max_of_r )
								{
									max_of_r = r ;	// 相関関数のピーク
									p = m ;			// 音データの基本周期
								}
							}

							{
								short *sp1, *dp1 ;
								short *sp2 ;
								int movnum ;

								sp1 = &SBuffer[ ( offset0     ) * 2 ] ;
								sp2 = &SBuffer[ ( offset0 + p ) * 2 ] ;
								dp1 = &DBuffer[ ( offset1     ) * 2 ] ;
								for( n = 0; n < p ; n++, sp1 += 2, sp2 += 2, dp1 += 2 )
								{
									short sp1p ;
									short sp2p ;

									sp1p = offset0     + n >= SSampleNum ? 0 : sp1[ 0 ] ;
									sp2p = offset0 + p + n >= SSampleNum ? 0 : sp2[ 0 ] ;
									if( offset1 + n < DSampleNum )
									{
										dp1[ 0 ] = ( short )( ( sp1p * ( p - n ) + sp2p * n ) / p ) ;
									}
								}

								if( offset1 + p     < DSampleNum &&
									offset0 + p * 2 < SSampleNum )
								{
									q = ( p * 512 * 512 / ( rateI - 512 ) + 256 ) / 512 ;
									sp1 = &SBuffer[ ( offset0 + p * 2 ) * 2 ] ;
									dp1 = &DBuffer[ ( offset1 + p     ) * 2 ] ;
									movnum = q - p ;
									if( offset0 + p * 2 + movnum > SSampleNum )
									{
										movnum = SSampleNum - ( offset0 + p * 2 ) ;
										q = p + movnum ;
									}
									if( offset1 + p + movnum > DSampleNum )
									{
										movnum = DSampleNum - ( offset1 + p ) ;
										q = p + movnum ;
									}
									for( n = 0 ; n < movnum ; n++, sp1 += 2, dp1 += 2 )
									{
										short sp1p ;

										sp1p = offset0 + p + n >= SSampleNum ? 0 : sp1[ 0 ] ;
										dp1[ 0 ] = sp1p ;
									}
								}
							}

							offset0 += p + q ;		// offset0の更新
							offset1 += q ;	// offset1の更新
						}
						if( offset1 < DSampleNum )
						{
							short *dp ;
							int loopnum ;

							loopnum = DSampleNum - offset1 ;
							dp = &DBuffer[ offset1 * 2 ] ;
							for( n = 0 ; n < loopnum ; n ++, dp += 2 )
							{
								*dp = 0 ;
							}
						}



						offset0 = 0 ;
						offset1 = 0 ;
						while( offset0 + pmax * 2 < SSampleNum && offset1 + pmax * 2 < DSampleNum )
						{
							max_of_r = 0 ;
							p = pmin ;
							for( m = pmin ; m <= pmax ; m++ )
							{
								short *sp1 ;
								short *sp2 ;

								r = 0 ;
								sp1 = &SBuffer[ ( offset0     ) * 2 + 1 ] ;
								sp2 = &SBuffer[ ( offset0 + m ) * 2 + 1 ] ;
								for( n = 0 ; n < template_size ; n++, sp1 += 2, sp2 += 2 )
								{
									r += ( sp1[ 0 ] * sp2[ 0 ] ) / 32768 ; // 相関関数
								}
								if( r > max_of_r )
								{
									max_of_r = r ;	// 相関関数のピーク
									p = m ;			// 音データの基本周期
								}
							}

							{
								short *sp1, *dp1 ;
								short *sp2 ;
								int movnum ;

								sp1 = &SBuffer[ ( offset0     ) * 2 + 1 ] ;
								sp2 = &SBuffer[ ( offset0 + p ) * 2 + 1 ] ;
								dp1 = &DBuffer[ ( offset1     ) * 2 + 1 ] ;
								for( n = 0; n < p ; n++, sp1 += 2, sp2 += 2, dp1 += 2 )
								{
									dp1[ 0 ] = ( short )( ( sp1[ 0 ] * ( p - n ) + sp2[ 0 ] * n ) / p ) ;
								}

								if( offset1 + p     < DSampleNum &&
									offset0 + p * 2 < SSampleNum )
								{
									q = ( p * 512 * 512 / ( rateI - 512 ) + 256 ) / 512 ;
									sp1 = &SBuffer[ ( offset0 + p * 2 ) * 2 + 1 ] ;
									dp1 = &DBuffer[ ( offset1 + p     ) * 2 + 1 ] ;
									movnum = q - p ;
									if( offset0 + p * 2 + movnum > SSampleNum )
									{
										movnum = SSampleNum - ( offset0 + p * 2 ) ;
										q = p + movnum ;
									}
									if( offset1 + p + movnum > DSampleNum )
									{
										movnum = DSampleNum - ( offset1 + p ) ;
										q = p + movnum ;
									}
									for( n = 0 ; n < movnum ; n++, sp1 += 2, dp1 += 2 )
									{
										dp1[ 0 ] = sp1[ 0 ] ;
									}
								}
							}

							offset0 += p + q ;	// offset0の更新
							offset1 += q ;		// offset1の更新
						}

						while( offset1 < DSampleNum && offset0 < SSampleNum )
						{
							max_of_r = 0 ;
							p = pmin ;
							for( m = pmin ; m <= pmax ; m++ )
							{
								short *sp1 ;
								short *sp2 ;

								r = 0 ;
								sp1 = &SBuffer[ ( offset0     ) * 2 + 1 ] ;
								sp2 = &SBuffer[ ( offset0 + m ) * 2 + 1 ] ;
								for( n = 0 ; n < template_size ; n++, sp1 += 2, sp2 += 2 )
								{
									int sp1p ;
									int sp2p ;

									sp1p = offset0     + n >= SSampleNum ? 0 : sp1[ 0 ] ;
									sp2p = offset0 + m + n >= SSampleNum ? 0 : sp2[ 0 ] ;
									r += ( sp1p * sp2p ) / 32768 ; // 相関関数
								}
								if( r > max_of_r )
								{
									max_of_r = r ;	// 相関関数のピーク
									p = m ;			// 音データの基本周期
								}
							}

							{
								short *sp1, *dp1 ;
								short *sp2 ;
								int movnum ;

								sp1 = &SBuffer[ ( offset0     ) * 2 + 1 ] ;
								sp2 = &SBuffer[ ( offset0 + p ) * 2 + 1 ] ;
								dp1 = &DBuffer[ ( offset1     ) * 2 + 1 ] ;
								for( n = 0; n < p ; n++, sp1 += 2, sp2 += 2, dp1 += 2 )
								{
									short sp1p ;
									short sp2p ;

									sp1p = offset0     + n >= SSampleNum ? 0 : sp1[ 0 ] ;
									sp2p = offset0 + p + n >= SSampleNum ? 0 : sp2[ 0 ] ;
									if( offset1 + n < DSampleNum )
									{
										dp1[ 0 ] = ( short )( ( sp1p * ( p - n ) + sp2p * n ) / p ) ;
									}
								}

								if( offset1 + p     < DSampleNum &&
									offset0 + p * 2 < SSampleNum )
								{
									q = ( p * 512 * 512 / ( rateI - 512 ) + 256 ) / 512 ;
									sp1 = &SBuffer[ ( offset0 + p * 2 ) * 2 + 1 ] ;
									dp1 = &DBuffer[ ( offset1 + p     ) * 2 + 1 ] ;
									movnum = q - p ;
									if( offset0 + p * 2 + movnum > SSampleNum )
									{
										movnum = SSampleNum - ( offset0 + p * 2 ) ;
										q = p + movnum ;
									}
									if( offset1 + p + movnum > DSampleNum )
									{
										movnum = DSampleNum - ( offset1 + p ) ;
										q = p + movnum ;
									}
									for( n = 0 ; n < movnum ; n++, sp1 += 2, dp1 += 2 )
									{
										short sp1p ;

										sp1p = offset0 + p + n >= SSampleNum ? 0 : sp1[ 0 ] ;
										dp1[ 0 ] = sp1p ;
									}
								}
							}

							offset0 += p + q ;	// offset0の更新
							offset1 += q ;		// offset1の更新
						}
						if( offset1 < DSampleNum )
						{
							short *dp ;
							int loopnum ;

							loopnum = DSampleNum - offset1 ;
							dp = &DBuffer[ offset1 * 2 + 1 ] ;
							for( n = 0 ; n < loopnum ; n ++, dp += 2 )
							{
								*dp = 0 ;
							}
						}
						break ;

					case 1 :
						offset0 = 0 ;
						offset1 = 0 ;
						while( offset0 + pmax * 2 < SSampleNum && offset1 + pmax * 2 < DSampleNum )
						{
							max_of_r = 0 ;
							p = pmin ;
							for( m = pmin ; m <= pmax ; m++ )
							{
								short *sp1 ;
								short *sp2 ;

								r = 0 ;
								sp1 = &SBuffer[ offset0     ] ;
								sp2 = &SBuffer[ offset0 + m ] ;
								for( n = 0 ; n < template_size ; n++, sp1 ++, sp2 ++ )
								{
									r += ( sp1[ 0 ] * sp2[ 0 ] ) / 32768 ; // 相関関数
								}
								if( r > max_of_r )
								{
									max_of_r = r ;	// 相関関数のピーク
									p = m ;			// 音データの基本周期
								}
							}

							{
								short *sp1, *dp1 ;
								short *sp2 ;
								int movnum ;

								sp1 = &SBuffer[ offset0     ] ;
								sp2 = &SBuffer[ offset0 + p ] ;
								dp1 = &DBuffer[ offset1     ] ;
								for( n = 0; n < p ; n++, sp1 ++, sp2 ++, dp1 ++ )
								{
									dp1[ 0 ] = ( short )( ( sp1[ 0 ] * ( p - n ) + sp2[ 0 ] * n ) / p ) ;
								}

								if( offset1 + p     < DSampleNum &&
									offset0 + p * 2 < SSampleNum )
								{
									q = ( p * 512 * 512 / ( rateI - 512 ) + 256 ) / 512 ;
									sp1 = &SBuffer[ offset0 + p * 2 ] ;
									dp1 = &DBuffer[ offset1 + p     ] ;
									movnum = q - p ;
									if( offset0 + p * 2 + movnum > SSampleNum )
									{
										movnum = SSampleNum - ( offset0 + p * 2 ) ;
										q = p + movnum ;
									}
									if( offset1 + p + movnum > DSampleNum )
									{
										movnum = DSampleNum - ( offset1 + p ) ;
										q = p + movnum ;
									}
									for( n = 0 ; n < movnum ; n++, sp1 ++, dp1 ++ )
									{
										dp1[ 0 ] = sp1[ 0 ] ;
									}
								}
							}

							offset0 += p + q ;	// offset0の更新
							offset1 += q ;		// offset1の更新
						}

						while( offset1 < DSampleNum && offset0 < SSampleNum )
						{
							max_of_r = 0 ;
							p = pmin ;
							for( m = pmin ; m <= pmax ; m++ )
							{
								short *sp1 ;
								short *sp2 ;

								r = 0 ;
								sp1 = &SBuffer[ offset0     ] ;
								sp2 = &SBuffer[ offset0 + m ] ;
								for( n = 0 ; n < template_size ; n++, sp1 ++, sp2 ++ )
								{
									int sp1p ;
									int sp2p ;

									sp1p = offset0     + n >= SSampleNum ? 0 : sp1[ 0 ] ;
									sp2p = offset0 + m + n >= SSampleNum ? 0 : sp2[ 0 ] ;
									r += ( sp1p * sp2p ) / 32768 ; // 相関関数
								}
								if( r > max_of_r )
								{
									max_of_r = r ;	// 相関関数のピーク
									p = m ;			// 音データの基本周期
								}
							}

							{
								short *sp1, *dp1 ;
								short *sp2 ;
								int movnum ;

								sp1 = &SBuffer[ offset0     ] ;
								sp2 = &SBuffer[ offset0 + p ] ;
								dp1 = &DBuffer[ offset1     ] ;
								for( n = 0; n < p ; n++, sp1 ++, sp2 ++, dp1 ++ )
								{
									short sp1p ;
									short sp2p ;

									sp1p = offset0     + n >= SSampleNum ? 0 : sp1[ 0 ] ;
									sp2p = offset0 + p + n >= SSampleNum ? 0 : sp2[ 0 ] ;
									if( offset1 + n < DSampleNum )
									{
										dp1[ 0 ] = ( short )( ( sp1p * ( p - n ) + sp2p * n ) / p ) ;
									}
								}

								if( offset1 + p     < DSampleNum &&
									offset0 + p * 2 < SSampleNum )
								{
									q = ( p * 512 * 512 / ( rateI - 512 ) + 256 ) / 512 ;
									sp1 = &SBuffer[ offset0 + p * 2 ] ;
									dp1 = &DBuffer[ offset1 + p     ] ;
									movnum = q - p ;
									if( offset0 + p * 2 + movnum > SSampleNum )
									{
										movnum = SSampleNum - ( offset0 + p * 2 ) ;
										q = p + movnum ;
									}
									if( offset1 + p + movnum > DSampleNum )
									{
										movnum = DSampleNum - ( offset1 + p ) ;
										q = p + movnum ;
									}
									for( n = 0 ; n < movnum ; n++, sp1 ++, dp1 ++ )
									{
										short sp1p ;

										sp1p = offset0 + p + n >= SSampleNum ? 0 : sp1[ 0 ] ;
										dp1[ 0 ] = sp1p ;
									}
								}
							}

							offset0 += p + q ;	// offset0の更新
							offset1 += q ;		// offset1の更新
						}
						if( offset1 < DSampleNum )
						{
							short *dp ;
							int loopnum ;

							loopnum = DSampleNum - offset1 ;
							dp = &DBuffer[ offset1 ] ;
							for( n = 0 ; n < loopnum ; n ++, dp ++ )
							{
								*dp = 0 ;
							}
						}
						break ;
					}
				}
				break ;

			case 8 :
				{
					BYTE *SBuffer ;
					BYTE *DBuffer ;
					int r ;
					int max_of_r;

					SBuffer = ( BYTE * )SSSound->Wave.Buffer ;
					DBuffer = ( BYTE * )DSSound->Wave.Buffer ;

					switch( SSSound->BufferFormat.nChannels )
					{
					case 2 :
						offset0 = 0 ;
						offset1 = 0 ;
						while( offset0 + pmax * 2 < SSampleNum && offset1 + pmax * 2 < DSampleNum )
						{
							max_of_r = 0 ;
							p = pmin ;
							for( m = pmin ; m <= pmax ; m++ )
							{
								BYTE *sp1 ;
								BYTE *sp2 ;

								r = 0 ;
								sp1 = &SBuffer[ ( offset0     ) * 2 ] ;
								sp2 = &SBuffer[ ( offset0 + m ) * 2 ] ;
								for( n = 0 ; n < template_size ; n++, sp1 += 2, sp2 += 2 )
								{
									r += ( ( ( int )sp1[ 0 ] - 128 ) * ( int )( sp2[ 0 ] - 128 ) ) / 128 ; // 相関関数
								}
								if( r > max_of_r )
								{
									max_of_r = r ;	// 相関関数のピーク
									p = m ;			// 音データの基本周期
								}
							}

							{
								BYTE *sp1, *dp1 ;
								BYTE *sp2 ;
								int movnum ;

								sp1 = &SBuffer[ ( offset0     ) * 2 ] ;
								sp2 = &SBuffer[ ( offset0 + p ) * 2 ] ;
								dp1 = &DBuffer[ ( offset1     ) * 2 ] ;
								for( n = 0; n < p ; n++, sp1 += 2, sp2 += 2, dp1 += 2 )
								{
									dp1[ 0 ] = ( BYTE )( ( sp1[ 0 ] * ( p - n ) + sp2[ 0 ] * n ) / p ) ;
								}

								if( offset1 + p     < DSampleNum &&
									offset0 + p * 2 < SSampleNum )
								{
									q = ( p * 512 * 512 / ( rateI - 512 ) + 256 ) / 512 ;
									sp1 = &SBuffer[ ( offset0 + p * 2 ) * 2 ] ;
									dp1 = &DBuffer[ ( offset1 + p     ) * 2 ] ;
									movnum = q - p ;
									if( offset0 + p * 2 + movnum > SSampleNum )
									{
										movnum = SSampleNum - ( offset0 + p * 2 ) ;
										q = p + movnum ;
									}
									if( offset1 + p + movnum > DSampleNum )
									{
										movnum = DSampleNum - ( offset1 + p ) ;
										q = p + movnum ;
									}
									for( n = 0 ; n < movnum ; n++, sp1 += 2, dp1 += 2 )
									{
										dp1[ 0 ] = sp1[ 0 ] ;
									}
								}
							}

							offset0 += p + q ;	// offset0の更新
							offset1 += q ;		// offset1の更新
						}

						while( offset1 < DSampleNum && offset0 < SSampleNum )
						{
							max_of_r = 0 ;
							p = pmin ;
							for( m = pmin ; m <= pmax ; m++ )
							{
								BYTE *sp1 ;
								BYTE *sp2 ;

								r = 0 ;
								sp1 = &SBuffer[ ( offset0     ) * 2 ] ;
								sp2 = &SBuffer[ ( offset0 + m ) * 2 ] ;
								for( n = 0 ; n < template_size ; n++, sp1 += 2, sp2 += 2 )
								{
									int sp1p ;
									int sp2p ;

									sp1p = offset0     + n >= SSampleNum ? 0 : ( int )sp1[ 0 ] - 128 ;
									sp2p = offset0 + m + n >= SSampleNum ? 0 : ( int )sp2[ 0 ] - 128 ;
									r += ( sp1p * sp2p ) / 128 ; // 相関関数
								}
								if( r > max_of_r )
								{
									max_of_r = r ;	// 相関関数のピーク
									p = m ;			// 音データの基本周期
								}
							}

							{
								BYTE *sp1, *dp1 ;
								BYTE *sp2 ;
								int movnum ;

								sp1 = &SBuffer[ ( offset0     ) * 2 ] ;
								sp2 = &SBuffer[ ( offset0 + p ) * 2 ] ;
								dp1 = &DBuffer[ ( offset1     ) * 2 ] ;
								for( n = 0; n < p ; n++, sp1 += 2, sp2 += 2, dp1 += 2 )
								{
									BYTE sp1p ;
									BYTE sp2p ;

									sp1p = offset0     + n >= SSampleNum ? 128 : sp1[ 0 ] ;
									sp2p = offset0 + p + n >= SSampleNum ? 128 : sp2[ 0 ] ;
									if( offset1 + n < DSampleNum )
									{
										dp1[ 0 ] = ( BYTE )( ( sp1p * ( p - n ) + sp2p * n ) / p ) ;
									}
								}

								if( offset1 + p     < DSampleNum &&
									offset0 + p * 2 < SSampleNum )
								{
									q = ( p * 512 * 512 / ( rateI - 512 ) + 256 ) / 512 ;
									sp1 = &SBuffer[ ( offset0 + p * 2 ) * 2 ] ;
									dp1 = &DBuffer[ ( offset1 + p     ) * 2 ] ;
									movnum = q - p ;
									if( offset0 + p * 2 + movnum > SSampleNum )
									{
										movnum = SSampleNum - ( offset0 + p * 2 ) ;
										q = p + movnum ;
									}
									if( offset1 + p + movnum > DSampleNum )
									{
										movnum = DSampleNum - ( offset1 + p ) ;
										q = p + movnum ;
									}
									for( n = 0 ; n < movnum ; n++, sp1 += 2, dp1 += 2 )
									{
										BYTE sp1p ;

										sp1p = offset0 + p + n >= SSampleNum ? 128 : sp1[ 0 ] ;
										dp1[ 0 ] = sp1p ;
									}
								}
							}

							offset0 += p + q ;	// offset0の更新
							offset1 += q ;		// offset1の更新
						}
						if( offset1 < DSampleNum )
						{
							BYTE *dp ;
							int loopnum ;

							loopnum = DSampleNum - offset1 ;
							dp = &DBuffer[ offset1 * 2 ] ;
							for( n = 0 ; n < loopnum ; n ++, dp += 2 )
							{
								*dp = 128 ;
							}
						}



						offset0 = 0 ;
						offset1 = 0 ;
						while( offset0 + pmax * 2 < SSampleNum && offset1 + pmax * 2 < DSampleNum )
						{
							max_of_r = 0 ;
							p = pmin ;
							for( m = pmin ; m <= pmax ; m++ )
							{
								BYTE *sp1 ;
								BYTE *sp2 ;

								r = 0 ;
								sp1 = &SBuffer[ ( offset0     ) * 2 + 1 ] ;
								sp2 = &SBuffer[ ( offset0 + m ) * 2 + 1 ] ;
								for( n = 0 ; n < template_size ; n++, sp1 += 2, sp2 += 2 )
								{
									r += ( ( ( int )sp1[ 0 ] - 128 ) * ( int )( sp2[ 0 ] - 128 ) ) / 128 ; // 相関関数
								}
								if( r > max_of_r )
								{
									max_of_r = r ;	// 相関関数のピーク
									p = m ;			// 音データの基本周期
								}
							}

							{
								BYTE *sp1, *dp1 ;
								BYTE *sp2 ;
								int movnum ;

								sp1 = &SBuffer[ ( offset0     ) * 2 + 1 ] ;
								sp2 = &SBuffer[ ( offset0 + p ) * 2 + 1 ] ;
								dp1 = &DBuffer[ ( offset1     ) * 2 + 1 ] ;
								for( n = 0; n < p ; n++, sp1 += 2, sp2 += 2, dp1 += 2 )
								{
									dp1[ 0 ] = ( BYTE )( ( sp1[ 0 ] * ( p - n ) + sp2[ 0 ] * n ) / p ) ;
								}

								if( offset1 + p     < DSampleNum &&
									offset0 + p * 2 < SSampleNum )
								{
									q = ( p * 512 * 512 / ( rateI - 512 ) + 256 ) / 512 ;
									sp1 = &SBuffer[ ( offset0 + p * 2 ) * 2 + 1 ] ;
									dp1 = &DBuffer[ ( offset1 + p     ) * 2 + 1 ] ;
									movnum = q - p ;
									if( offset0 + p * 2 + movnum > SSampleNum )
									{
										movnum = SSampleNum - ( offset0 + p * 2 ) ;
										q = p + movnum ;
									}
									if( offset1 + p + movnum > DSampleNum )
									{
										movnum = DSampleNum - ( offset1 + p ) ;
										q = p + movnum ;
									}
									for( n = 0 ; n < movnum ; n++, sp1 += 2, dp1 += 2 )
									{
										dp1[ 0 ] = sp1[ 0 ] ;
									}
								}
							}

							offset0 += p + q ;	// offset0の更新
							offset1 += q ;		// offset1の更新
						}

						while( offset1 < DSampleNum && offset0 < SSampleNum )
						{
							max_of_r = 0 ;
							p = pmin ;
							for( m = pmin ; m <= pmax ; m++ )
							{
								BYTE *sp1 ;
								BYTE *sp2 ;

								r = 0 ;
								sp1 = &SBuffer[ ( offset0     ) * 2 + 1 ] ;
								sp2 = &SBuffer[ ( offset0 + m ) * 2 + 1 ] ;
								for( n = 0 ; n < template_size ; n++, sp1 += 2, sp2 += 2 )
								{
									int sp1p ;
									int sp2p ;

									sp1p = offset0     + n >= SSampleNum ? 0 : ( int )sp1[ 0 ] - 128 ;
									sp2p = offset0 + m + n >= SSampleNum ? 0 : ( int )sp2[ 0 ] - 128 ;
									r += ( sp1p * sp2p ) / 128 ; // 相関関数
								}
								if( r > max_of_r )
								{
									max_of_r = r ;	// 相関関数のピーク
									p = m ;			// 音データの基本周期
								}
							}

							{
								BYTE *sp1, *dp1 ;
								BYTE *sp2 ;
								int movnum ;

								sp1 = &SBuffer[ ( offset0     ) * 2 + 1 ] ;
								sp2 = &SBuffer[ ( offset0 + p ) * 2 + 1 ] ;
								dp1 = &DBuffer[ ( offset1     ) * 2 + 1 ] ;
								for( n = 0; n < p ; n++, sp1 += 2, sp2 += 2, dp1 += 2 )
								{
									BYTE sp1p ;
									BYTE sp2p ;

									sp1p = offset0     + n >= SSampleNum ? 128 : sp1[ 0 ] ;
									sp2p = offset0 + p + n >= SSampleNum ? 128 : sp2[ 0 ] ;
									if( offset1 + n < DSampleNum )
									{
										dp1[ 0 ] = ( BYTE )( ( sp1p * ( p - n ) + sp2p * n ) / p ) ;
									}
								}

								if( offset1 + p     < DSampleNum &&
									offset0 + p * 2 < SSampleNum )
								{
									q = ( p * 512 * 512 / ( rateI - 512 ) + 256 ) / 512 ;
									sp1 = &SBuffer[ ( offset0 + p * 2 ) * 2 + 1 ] ;
									dp1 = &DBuffer[ ( offset1 + p     ) * 2 + 1 ] ;
									movnum = q - p ;
									if( offset0 + p * 2 + movnum > SSampleNum )
									{
										movnum = SSampleNum - ( offset0 + p * 2 ) ;
										q = p + movnum ;
									}
									if( offset1 + p + movnum > DSampleNum )
									{
										movnum = DSampleNum - ( offset1 + p ) ;
										q = p + movnum ;
									}
									for( n = 0 ; n < movnum ; n++, sp1 += 2, dp1 += 2 )
									{
										BYTE sp1p ;

										sp1p = offset0 + p + n >= SSampleNum ? 128 : sp1[ 0 ] ;
										dp1[ 0 ] = sp1p ;
									}
								}
							}

							offset0 += p + q ;	// offset0の更新
							offset1 += q ;		// offset1の更新
						}
						if( offset1 < DSampleNum )
						{
							BYTE *dp ;
							int loopnum ;

							loopnum = DSampleNum - offset1 ;
							dp = &DBuffer[ ( offset1 * 2 ) + 1 ] ;
							for( n = 0 ; n < loopnum ; n ++, dp += 2 )
							{
								*dp = 128 ;
							}
						}
						break ;

					case 1 :
						offset0 = 0 ;
						offset1 = 0 ;
						while( offset0 + pmax * 2 < SSampleNum && offset1 + pmax * 2 < DSampleNum )
						{
							max_of_r = 0 ;
							p = pmin ;
							for( m = pmin ; m <= pmax ; m++ )
							{
								BYTE *sp1 ;
								BYTE *sp2 ;

								r = 0 ;
								sp1 = &SBuffer[ offset0     ] ;
								sp2 = &SBuffer[ offset0 + m ] ;
								for( n = 0 ; n < template_size ; n++, sp1 ++, sp2 ++ )
								{
									r += ( ( ( int )sp1[ 0 ] - 128 ) * ( int )( sp2[ 0 ] - 128 ) ) / 128 ; // 相関関数
								}
								if( r > max_of_r )
								{
									max_of_r = r ;	// 相関関数のピーク
									p = m ;			// 音データの基本周期
								}
							}

							{
								BYTE *sp1, *dp1 ;
								BYTE *sp2 ;
								int movnum ;

								sp1 = &SBuffer[ offset0     ] ;
								sp2 = &SBuffer[ offset0 + p ] ;
								dp1 = &DBuffer[ offset1     ] ;
								for( n = 0; n < p ; n++, sp1 ++, sp2 ++, dp1 ++ )
								{
									dp1[ 0 ] = ( BYTE )( ( sp1[ 0 ] * ( p - n ) + sp2[ 0 ] * n ) / p ) ;
								}

								if( offset1 + p     < DSampleNum &&
									offset0 + p * 2 < SSampleNum )
								{
									q = ( p * 512 * 512 / ( rateI - 512 ) + 256 ) / 512 ;
									sp1 = &SBuffer[ offset0 + p * 2 ] ;
									dp1 = &DBuffer[ offset1 + p     ] ;
									movnum = q - p ;
									if( offset0 + p * 2 + movnum > SSampleNum )
									{
										movnum = SSampleNum - ( offset0 + p * 2 ) ;
										q = p + movnum ;
									}
									if( offset1 + p + movnum > DSampleNum )
									{
										movnum = DSampleNum - ( offset1 + p ) ;
										q = p + movnum ;
									}
									for( n = 0 ; n < movnum ; n++, sp1 ++, dp1 ++ )
									{
										dp1[ 0 ] = sp1[ 0 ] ;
									}
								}
							}

							offset0 += p + q ;	// offset0の更新
							offset1 += q ;		// offset1の更新
						}

						while( offset1 < DSampleNum && offset0 < SSampleNum )
						{
							max_of_r = 0 ;
							p = pmin ;
							for( m = pmin ; m <= pmax ; m++ )
							{
								BYTE *sp1 ;
								BYTE *sp2 ;

								r = 0 ;
								sp1 = &SBuffer[ offset0     ] ;
								sp2 = &SBuffer[ offset0 + m ] ;
								for( n = 0 ; n < template_size ; n++, sp1 ++, sp2 ++ )
								{
									int sp1p ;
									int sp2p ;

									sp1p = offset0     + n >= SSampleNum ? 0 : ( int )sp1[ 0 ] - 128 ;
									sp2p = offset0 + m + n >= SSampleNum ? 0 : ( int )sp2[ 0 ] - 128 ;
									r += ( sp1p * sp2p ) / 128 ; // 相関関数
								}
								if( r > max_of_r )
								{
									max_of_r = r ;	// 相関関数のピーク
									p = m ;			// 音データの基本周期
								}
							}

							{
								BYTE *sp1, *dp1 ;
								BYTE *sp2 ;
								int movnum ;

								sp1 = &SBuffer[ offset0     ] ;
								sp2 = &SBuffer[ offset0 + p ] ;
								dp1 = &DBuffer[ offset1     ] ;
								for( n = 0; n < p ; n++, sp1 ++, sp2 ++, dp1 ++ )
								{
									BYTE sp1p ;
									BYTE sp2p ;

									sp1p = offset0     + n >= SSampleNum ? 128 : sp1[ 0 ] ;
									sp2p = offset0 + p + n >= SSampleNum ? 128 : sp2[ 0 ] ;
									if( offset1 + n < DSampleNum )
									{
										dp1[ 0 ] = ( BYTE )( ( sp1p * ( p - n ) + sp2p * n ) / p ) ;
									}
								}

								if( offset1 + p     < DSampleNum &&
									offset0 + p * 2 < SSampleNum )
								{
									q = ( p * 512 * 512 / ( rateI - 512 ) + 256 ) / 512 ;
									sp1 = &SBuffer[ offset0 + p * 2 ] ;
									dp1 = &DBuffer[ offset1 + p     ] ;
									movnum = q - p ;
									if( offset0 + p * 2 + movnum > SSampleNum )
									{
										movnum = SSampleNum - ( offset0 + p * 2 ) ;
										q = p + movnum ;
									}
									if( offset1 + p + movnum > DSampleNum )
									{
										movnum = DSampleNum - ( offset1 + p ) ;
										q = p + movnum ;
									}
									for( n = 0 ; n < movnum ; n++, sp1 ++, dp1 ++ )
									{
										BYTE sp1p ;

										sp1p = offset0 + p + n >= SSampleNum ? 128 : sp1[ 0 ] ;
										dp1[ 0 ] = sp1p ;
									}
								}
							}

							offset0 += p + q ;	// offset0の更新
							offset1 += q ;		// offset1の更新
						}
						if( offset1 < DSampleNum )
						{
							BYTE *dp ;
							int loopnum ;

							loopnum = DSampleNum - offset1 ;
							dp = &DBuffer[ offset1 ] ;
							for( n = 0 ; n < loopnum ; n ++, dp ++ )
							{
								*dp = 128 ;
							}
						}
						break ;
					}
				}
				break ;
			}
			break ;

		case WAVE_FORMAT_IEEE_FLOAT :
			{
				float *SBuffer ;
				float *DBuffer ;
				float r ;
				float max_of_r;

				SBuffer = ( float * )SSSound->Wave.Buffer ;
				DBuffer = ( float * )DSSound->Wave.Buffer ;

				switch( SSSound->BufferFormat.nChannels )
				{
				case 2 :
					offset0 = 0 ;
					offset1 = 0 ;
					while( offset0 + pmax * 2 < SSampleNum && offset1 + pmax * 2 < DSampleNum )
					{
						max_of_r = 0.0f ;
						p = pmin ;
						for( m = pmin ; m <= pmax ; m++ )
						{
							float *sp1 ;
							float *sp2 ;

							r = 0.0f ;
							sp1 = &SBuffer[ ( offset0     ) * 2 ] ;
							sp2 = &SBuffer[ ( offset0 + m ) * 2 ] ;
							for( n = 0 ; n < template_size ; n++, sp1 += 2, sp2 += 2 )
							{
								r += sp1[ 0 ] * sp2[ 0 ] ; // 相関関数
							}
							if( r > max_of_r )
							{
								max_of_r = r ;	// 相関関数のピーク
								p = m ;			// 音データの基本周期
							}
						}

						{
							float *sp1, *dp1 ;
							float *sp2 ;
							int movnum ;

							sp1 = &SBuffer[ ( offset0     ) * 2 ] ;
							sp2 = &SBuffer[ ( offset0 + p ) * 2 ] ;
							dp1 = &DBuffer[ ( offset1     ) * 2 ] ;
							for( n = 0; n < p ; n++, sp1 += 2, sp2 += 2, dp1 += 2 )
							{
								dp1[ 0 ] = ( sp1[ 0 ] * ( p - n ) + sp2[ 0 ] * n ) / p ;
							}

							if( offset1 + p     < DSampleNum &&
								offset0 + p * 2 < SSampleNum )
							{
								q = ( p * 512 * 512 / ( rateI - 512 ) + 256 ) / 512 ;
								sp1 = &SBuffer[ ( offset0 + p * 2 ) * 2 ] ;
								dp1 = &DBuffer[ ( offset1 + p     ) * 2 ] ;
								movnum = q - p ;
								if( offset0 + p * 2 + movnum > SSampleNum )
								{
									movnum = SSampleNum - ( offset0 + p * 2 ) ;
									q = p + movnum ;
								}
								if( offset1 + p + movnum > DSampleNum )
								{
									movnum = DSampleNum - ( offset1 + p ) ;
									q = p + movnum ;
								}
								for( n = 0 ; n < movnum ; n++, sp1 += 2, dp1 += 2 )
								{
									dp1[ 0 ] = sp1[ 0 ] ;
								}
							}
						}

						offset0 += p + q ;	// offset0の更新
						offset1 += q ;		// offset1の更新
					}

					while( offset1 < DSampleNum && offset0 < SSampleNum )
					{
						max_of_r = 0.0f ;
						p = pmin ;
						for( m = pmin ; m <= pmax ; m++ )
						{
							float *sp1 ;
							float *sp2 ;

							r = 0.0f ;
							sp1 = &SBuffer[ ( offset0     ) * 2 ] ;
							sp2 = &SBuffer[ ( offset0 + m ) * 2 ] ;
							for( n = 0 ; n < template_size ; n++, sp1 += 2, sp2 += 2 )
							{
								float sp1p ;
								float sp2p ;

								sp1p = offset0     + n >= SSampleNum ? 0.0f : sp1[ 0 ] ;
								sp2p = offset0 + m + n >= SSampleNum ? 0.0f : sp2[ 0 ] ;
								r += sp1p * sp2p ; // 相関関数
							}
							if( r > max_of_r )
							{
								max_of_r = r ;	// 相関関数のピーク
								p = m ;			// 音データの基本周期
							}
						}

						{
							float *sp1, *dp1 ;
							float *sp2 ;
							int movnum ;

							sp1 = &SBuffer[ ( offset0     ) * 2 ] ;
							sp2 = &SBuffer[ ( offset0 + p ) * 2 ] ;
							dp1 = &DBuffer[ ( offset1     ) * 2 ] ;
							for( n = 0; n < p ; n++, sp1 += 2, sp2 += 2, dp1 += 2 )
							{
								float sp1p ;
								float sp2p ;

								sp1p = offset0     + n >= SSampleNum ? 0.0f : sp1[ 0 ] ;
								sp2p = offset0 + p + n >= SSampleNum ? 0.0f : sp2[ 0 ] ;
								if( offset1 + n < DSampleNum )
								{
									dp1[ 0 ] = ( sp1p * ( p - n ) + sp2p * n ) / p ;
								}
							}

							if( offset1 + p     < DSampleNum &&
								offset0 + p * 2 < SSampleNum )
							{
								q = ( p * 512 * 512 / ( rateI - 512 ) + 256 ) / 512 ;
								sp1 = &SBuffer[ ( offset0 + p * 2 ) * 2 ] ;
								dp1 = &DBuffer[ ( offset1 + p     ) * 2 ] ;
								movnum = q - p ;
								if( offset0 + p * 2 + movnum > SSampleNum )
								{
									movnum = SSampleNum - ( offset0 + p * 2 ) ;
									q = p + movnum ;
								}
								if( offset1 + p + movnum > DSampleNum )
								{
									movnum = DSampleNum - ( offset1 + p ) ;
									q = p + movnum ;
								}
								for( n = 0 ; n < movnum ; n++, sp1 += 2, dp1 += 2 )
								{
									float sp1p ;

									sp1p = offset0 + p + n >= SSampleNum ? 0.0f : sp1[ 0 ] ;
									dp1[ 0 ] = sp1p ;
								}
							}
						}

						offset0 += p + q ;	// offset0の更新
						offset1 += q ;		// offset1の更新
					}
					if( offset1 < DSampleNum )
					{
						float *dp ;
						int loopnum ;

						loopnum = DSampleNum - offset1 ;
						dp = &DBuffer[ offset1 * 2 ] ;
						for( n = 0 ; n < loopnum ; n ++, dp += 2 )
						{
							*dp = 0.0f ;
						}
					}



					offset0 = 0 ;
					offset1 = 0 ;
					while( offset0 + pmax * 2 < SSampleNum && offset1 + pmax * 2 < DSampleNum )
					{
						max_of_r = 0.0f ;
						p = pmin ;
						for( m = pmin ; m <= pmax ; m++ )
						{
							float *sp1 ;
							float *sp2 ;

							r = 0.0f ;
							sp1 = &SBuffer[ ( offset0     ) * 2 + 1 ] ;
							sp2 = &SBuffer[ ( offset0 + m ) * 2 + 1 ] ;
							for( n = 0 ; n < template_size ; n++, sp1 += 2, sp2 += 2 )
							{
								r += sp1[ 0 ] * sp2[ 0 ] ; // 相関関数
							}
							if( r > max_of_r )
							{
								max_of_r = r ;	// 相関関数のピーク
								p = m ;			// 音データの基本周期
							}
						}

						{
							float *sp1, *dp1 ;
							float *sp2 ;
							int movnum ;

							sp1 = &SBuffer[ ( offset0     ) * 2 + 1 ] ;
							sp2 = &SBuffer[ ( offset0 + p ) * 2 + 1 ] ;
							dp1 = &DBuffer[ ( offset1     ) * 2 + 1 ] ;
							for( n = 0; n < p ; n++, sp1 += 2, sp2 += 2, dp1 += 2 )
							{
								dp1[ 0 ] = ( sp1[ 0 ] * ( p - n ) + sp2[ 0 ] * n ) / p ;
							}

							if( offset1 + p     < DSampleNum &&
								offset0 + p * 2 < SSampleNum )
							{
								q = ( p * 512 * 512 / ( rateI - 512 ) + 256 ) / 512 ;
								sp1 = &SBuffer[ ( offset0 + p * 2 ) * 2 + 1 ] ;
								dp1 = &DBuffer[ ( offset1 + p     ) * 2 + 1 ] ;
								movnum = q - p ;
								if( offset0 + p * 2 + movnum > SSampleNum )
								{
									movnum = SSampleNum - ( offset0 + p * 2 ) ;
									q = p + movnum ;
								}
								if( offset1 + p + movnum > DSampleNum )
								{
									movnum = DSampleNum - ( offset1 + p ) ;
									q = p + movnum ;
								}
								for( n = 0 ; n < movnum ; n++, sp1 += 2, dp1 += 2 )
								{
									dp1[ 0 ] = sp1[ 0 ] ;
								}
							}
						}

						offset0 += p + q ;	// offset0の更新
						offset1 += q ;		// offset1の更新
					}

					while( offset1 < DSampleNum && offset0 < SSampleNum )
					{
						max_of_r = 0.0f ;
						p = pmin ;
						for( m = pmin ; m <= pmax ; m++ )
						{
							float *sp1 ;
							float *sp2 ;

							r = 0.0f ;
							sp1 = &SBuffer[ ( offset0     ) * 2 + 1 ] ;
							sp2 = &SBuffer[ ( offset0 + m ) * 2 + 1 ] ;
							for( n = 0 ; n < template_size ; n++, sp1 += 2, sp2 += 2 )
							{
								float sp1p ;
								float sp2p ;

								sp1p = offset0     + n >= SSampleNum ? 0.0f : sp1[ 0 ] ;
								sp2p = offset0 + m + n >= SSampleNum ? 0.0f : sp2[ 0 ] ;
								r += sp1p * sp2p ; // 相関関数
							}
							if( r > max_of_r )
							{
								max_of_r = r ;	// 相関関数のピーク
								p = m ;			// 音データの基本周期
							}
						}

						{
							float *sp1, *dp1 ;
							float *sp2 ;
							int movnum ;

							sp1 = &SBuffer[ ( offset0     ) * 2 + 1 ] ;
							sp2 = &SBuffer[ ( offset0 + p ) * 2 + 1 ] ;
							dp1 = &DBuffer[ ( offset1     ) * 2 + 1 ] ;
							for( n = 0; n < p ; n++, sp1 += 2, sp2 += 2, dp1 += 2 )
							{
								float sp1p ;
								float sp2p ;

								sp1p = offset0     + n >= SSampleNum ? 0 : sp1[ 0 ] ;
								sp2p = offset0 + p + n >= SSampleNum ? 0 : sp2[ 0 ] ;
								if( offset1 + n < DSampleNum )
								{
									dp1[ 0 ] = ( sp1p * ( p - n ) + sp2p * n ) / p ;
								}
							}

							if( offset1 + p     < DSampleNum &&
								offset0 + p * 2 < SSampleNum )
							{
								q = ( p * 512 * 512 / ( rateI - 512 ) + 256 ) / 512 ;
								sp1 = &SBuffer[ ( offset0 + p * 2 ) * 2 + 1 ] ;
								dp1 = &DBuffer[ ( offset1 + p     ) * 2 + 1 ] ;
								movnum = q - p ;
								if( offset0 + p * 2 + movnum > SSampleNum )
								{
									movnum = SSampleNum - ( offset0 + p * 2 ) ;
									q = p + movnum ;
								}
								if( offset1 + p + movnum > DSampleNum )
								{
									movnum = DSampleNum - ( offset1 + p ) ;
									q = p + movnum ;
								}
								for( n = 0 ; n < movnum ; n++, sp1 += 2, dp1 += 2 )
								{
									float sp1p ;

									sp1p = offset0 + p + n >= SSampleNum ? 0 : sp1[ 0 ] ;
									dp1[ 0 ] = sp1p ;
								}
							}
						}

						offset0 += p + q ;	// offset0の更新
						offset1 += q ;		// offset1の更新
					}
					if( offset1 < DSampleNum )
					{
						float *dp ;
						int loopnum ;

						loopnum = DSampleNum - offset1 ;
						dp = &DBuffer[ offset1 * 2 + 1 ] ;
						for( n = 0 ; n < loopnum ; n ++, dp += 2 )
						{
							*dp = 0.0f ;
						}
					}
					break ;

				case 1 :
					offset0 = 0 ;
					offset1 = 0 ;
					while( offset0 + pmax * 2 < SSampleNum && offset1 + pmax * 2 < DSampleNum )
					{
						max_of_r = 0.0f ;
						p = pmin ;
						for( m = pmin ; m <= pmax ; m++ )
						{
							float *sp1 ;
							float *sp2 ;

							r = 0.0f ;
							sp1 = &SBuffer[ offset0     ] ;
							sp2 = &SBuffer[ offset0 + m ] ;
							for( n = 0 ; n < template_size ; n++, sp1 ++, sp2 ++ )
							{
								r += sp1[ 0 ] * sp2[ 0 ] ; // 相関関数
							}
							if( r > max_of_r )
							{
								max_of_r = r ;	// 相関関数のピーク
								p = m ;			// 音データの基本周期
							}
						}

						{
							float *sp1, *dp1 ;
							float *sp2 ;
							int movnum ;

							sp1 = &SBuffer[ offset0     ] ;
							sp2 = &SBuffer[ offset0 + p ] ;
							dp1 = &DBuffer[ offset1     ] ;
							for( n = 0; n < p ; n++, sp1 ++, sp2 ++, dp1 ++ )
							{
								dp1[ 0 ] = ( sp1[ 0 ] * ( p - n ) + sp2[ 0 ] * n ) / p ;
							}

							if( offset1 + p     < DSampleNum &&
								offset0 + p * 2 < SSampleNum )
							{
								q = ( p * 512 * 512 / ( rateI - 512 ) + 256 ) / 512 ;
								sp1 = &SBuffer[ offset0 + p * 2 ] ;
								dp1 = &DBuffer[ offset1 + p     ] ;
								movnum = q - p ;
								if( offset0 + p * 2 + movnum > SSampleNum )
								{
									movnum = SSampleNum - ( offset0 + p * 2 ) ;
									q = p + movnum ;
								}
								if( offset1 + p + movnum > DSampleNum )
								{
									movnum = DSampleNum - ( offset1 + p ) ;
									q = p + movnum ;
								}
								for( n = 0 ; n < movnum ; n++, sp1 ++, dp1 ++ )
								{
									dp1[ 0 ] = sp1[ 0 ] ;
								}
							}
						}

						offset0 += p + q ;	// offset0の更新
						offset1 += q ;		// offset1の更新
					}

					while( offset1 < DSampleNum && offset0 < SSampleNum )
					{
						max_of_r = 0.0f ;
						p = pmin ;
						for( m = pmin ; m <= pmax ; m++ )
						{
							float *sp1 ;
							float *sp2 ;

							r = 0.0f ;
							sp1 = &SBuffer[ offset0     ] ;
							sp2 = &SBuffer[ offset0 + m ] ;
							for( n = 0 ; n < template_size ; n++, sp1 ++, sp2 ++ )
							{
								float sp1p ;
								float sp2p ;

								sp1p = offset0     + n >= SSampleNum ? 0.0f : sp1[ 0 ] ;
								sp2p = offset0 + m + n >= SSampleNum ? 0.0f : sp2[ 0 ] ;
								r += sp1p * sp2p ; // 相関関数
							}
							if( r > max_of_r )
							{
								max_of_r = r ;	// 相関関数のピーク
								p = m ;			// 音データの基本周期
							}
						}

						{
							float *sp1, *dp1 ;
							float *sp2 ;
							int movnum ;

							sp1 = &SBuffer[ offset0     ] ;
							sp2 = &SBuffer[ offset0 + p ] ;
							dp1 = &DBuffer[ offset1     ] ;
							for( n = 0; n < p ; n++, sp1 ++, sp2 ++, dp1 ++ )
							{
								float sp1p ;
								float sp2p ;

								sp1p = offset0     + n >= SSampleNum ? 0.0f : sp1[ 0 ] ;
								sp2p = offset0 + p + n >= SSampleNum ? 0.0f : sp2[ 0 ] ;
								if( offset1 + n < DSampleNum )
								{
									dp1[ 0 ] = ( sp1p * ( p - n ) + sp2p * n ) / p ;
								}
							}

							if( offset1 + p     < DSampleNum &&
								offset0 + p * 2 < SSampleNum )
							{
								q = ( p * 512 * 512 / ( rateI - 512 ) + 256 ) / 512 ;
								sp1 = &SBuffer[ offset0 + p * 2 ] ;
								dp1 = &DBuffer[ offset1 + p     ] ;
								movnum = q - p ;
								if( offset0 + p * 2 + movnum > SSampleNum )
								{
									movnum = SSampleNum - ( offset0 + p * 2 ) ;
									q = p + movnum ;
								}
								if( offset1 + p + movnum > DSampleNum )
								{
									movnum = DSampleNum - ( offset1 + p ) ;
									q = p + movnum ;
								}
								for( n = 0 ; n < movnum ; n++, sp1 ++, dp1 ++ )
								{
									float sp1p ;

									sp1p = offset0 + p + n >= SSampleNum ? 0.0f : sp1[ 0 ] ;
									dp1[ 0 ] = sp1p ;
								}
							}
						}

						offset0 += p + q ;	// offset0の更新
						offset1 += q ;		// offset1の更新
					}
					if( offset1 < DSampleNum )
					{
						float *dp ;
						int loopnum ;

						loopnum = DSampleNum - offset1 ;
						dp = &DBuffer[ offset1 ] ;
						for( n = 0 ; n < loopnum ; n ++, dp ++ )
						{
							*dp = 0.0f ;
						}
					}
					break ;
				}
			}
			break ;
		}
	}


	// 終了
	return 0 ;
}

// ソフトウエアで扱う波形データハンドルの波形データの長さを変更する
#define PITCHSHIFT_WIN_N			(128)
#define PITCHSHIFT_ANGLE_DIV_NUM	( ( PITCHSHIFT_WIN_N * 2 + 1 ) * 128 )
extern int NS_WritePitchShiftSoftSoundData( int SrcSoftSoundHandle, int DestSoftSoundHandle )
{
	SOFTSOUND * SSSound ;
	SOFTSOUND * DSSound ;
	int SSampleNum ;
	int DSampleNum ;
	int n, m, ta, tb ;
	int Start ;
	int End ;
	int T_M_I ;
	int mt, st ;
	int AngleI, SinCI, SinCosI, CosI ;

	// エラー判定
	if( SSND_MASKHCHK_ASYNC( SrcSoftSoundHandle, SSSound ) ) return -1 ;
	if( SSSound->IsPlayer == 1 ) return -1 ;

	if( SSND_MASKHCHK_ASYNC( DestSoftSoundHandle, DSSound ) ) return -1 ;
	if( DSSound->IsPlayer == 1 ) return -1 ;

	// サンプル数が変わらない場合はエラー
	if( SSSound->Wave.BufferSampleNum == DSSound->Wave.BufferSampleNum )
	{
		return -1 ;
	}

	// フォーマットが異なる場合はエラー
	if( SSSound->BufferFormat.wFormatTag     != DSSound->BufferFormat.wFormatTag ||
		SSSound->BufferFormat.nChannels      != DSSound->BufferFormat.nChannels  ||
		SSSound->BufferFormat.nSamplesPerSec != DSSound->BufferFormat.nSamplesPerSec ||
		SSSound->BufferFormat.wBitsPerSample != DSSound->BufferFormat.wBitsPerSample )
	{
		return -1 ;
	}

	SSampleNum = SSSound->Wave.BufferSampleNum ;
	DSampleNum = DSSound->Wave.BufferSampleNum ;

	mt = 0 ;
	st = 0 ;

	switch( SSSound->BufferFormat.wFormatTag )
	{
	case WAVE_FORMAT_PCM :
#if 1
		{
			// Float タイプに変換して実行する
			int TempFloatSSSHandle = -1 ;
			int TempFloatDSSHandle = -1 ;

			TempFloatSSSHandle = NS_MakeSoftSoundCustom( SSSound->BufferFormat.nChannels, 32, SSSound->BufferFormat.nSamplesPerSec, SSSound->Wave.BufferSampleNum, TRUE ) ;
			if( TempFloatSSSHandle < 0 )
			{
				return -1 ;
			}
			TempFloatDSSHandle = NS_MakeSoftSoundCustom( DSSound->BufferFormat.nChannels, 32, DSSound->BufferFormat.nSamplesPerSec, DSSound->Wave.BufferSampleNum, TRUE ) ;
			if( TempFloatDSSHandle < 0 )
			{
				NS_DeleteSoftSound( TempFloatSSSHandle ) ;
				TempFloatSSSHandle = -1 ;
				return -1 ;
			}

			ConvertIntToFloatSoftSound( SrcSoftSoundHandle, TempFloatSSSHandle ) ;
			NS_WritePitchShiftSoftSoundData( TempFloatSSSHandle, TempFloatDSSHandle ) ;
			ConvertFloatToIntSoftSound( TempFloatDSSHandle, DestSoftSoundHandle ) ;

			NS_DeleteSoftSound( TempFloatSSSHandle ) ;
			NS_DeleteSoftSound( TempFloatDSSHandle ) ;
			TempFloatSSSHandle = -1 ;
			TempFloatDSSHandle = -1 ;
		}
#else
		int DestInt ;
		switch( SSSound->BufferFormat.wBitsPerSample )
		{
		case 16 :
			{
				short *SBuffer ;
				short *DBuffer ;

				SBuffer = ( short * )SSSound->Wave.Buffer ;
				DBuffer = ( short * )DSSound->Wave.Buffer ;

				_MEMSET( DBuffer, 0, sizeof( short ) * SSSound->BufferFormat.nChannels * DSampleNum ) ;

				switch( SSSound->BufferFormat.nChannels )
				{
				case 1 :
					for( n = 0; n < DSampleNum ; n++ )
					{
						ta = mt ;
						tb = st == 0 ? ta : ta + 1 ;

						Start = tb - PITCHSHIFT_WIN_N / 2 ;
						End   = ta + PITCHSHIFT_WIN_N / 2 ;
						if( Start <           0 ) Start = 0 ;
						if( Start >= SSampleNum ) Start = SSampleNum - 1 ;
						if( End   <           0 ) End   = 0 ;
						if( End   >= SSampleNum ) End   = SSampleNum - 1 ;

						st += SSampleNum ;
						while( st >= DSampleNum )
						{
							st -= DSampleNum;
							mt++ ;
						}

						T_M_I = ( mt * 128 + st * 128 / DSampleNum ) - Start * 128 ;
						for( m = Start; m <= End; m++, T_M_I -= 128 )
						{
							AngleI = ( int )( DX_PI_F * 128.0f ) * T_M_I ;
							if( AngleI == 0 )
							{
								SinCI = 16384 ;
							}
							else
							{
								SinCI = SoundSysData.SinTable[ ( ( SOUND_SINTABLE_DIV / 2 ) * T_M_I / 128 ) & ( SOUND_SINTABLE_DIV - 1 ) ] * 16384 / AngleI ;
							}
							CosI = SoundSysData.SinTable[ ( ( SOUND_SINTABLE_DIV * T_M_I / PITCHSHIFT_ANGLE_DIV_NUM ) + SOUND_SINTABLE_DIV / 4 ) & ( SOUND_SINTABLE_DIV - 1 ) ] ;

							SinCosI = ( SinCI * ( 64 * 128 + ( 64 * 128 * CosI ) / 16384 ) ) / 16384 ;
							DestInt  = ( int )DBuffer[ n ] + ( ( int )SBuffer[ m ] * SinCosI ) / 16384 ;

							     if( DestInt < -32768 )	DBuffer[ n ] = -32768 ;
							else if( DestInt >  32767 )	DBuffer[ n ] =  32767 ;
							else						DBuffer[ n ] = ( short )DestInt ;
						}
					}
					break ;

				case 2 :
					for( n = 0; n < DSampleNum ; n++ )
					{
						ta = mt ;
						tb = st == 0 ? ta : ta + 1 ;

						Start = tb - PITCHSHIFT_WIN_N / 2 ;
						End   = ta + PITCHSHIFT_WIN_N / 2 ;
						if( Start <           0 ) Start = 0 ;
						if( Start >= SSampleNum ) Start = SSampleNum - 1 ;
						if( End   <           0 ) End   = 0 ;
						if( End   >= SSampleNum ) End   = SSampleNum - 1 ;

						st += SSampleNum ;
						while( st >= DSampleNum )
						{
							st -= DSampleNum;
							mt++ ;
						}

						T_M_I = ( mt * 128 + st * 128 / DSampleNum ) - Start * 128 ;
						for( m = Start; m <= End; m++, T_M_I -= 128 )
						{
							AngleI = ( int )( DX_PI_F * 128.0f ) * T_M_I ;
							if( AngleI == 0 )
							{
								SinCI = 16384 ;
							}
							else
							{
								SinCI = SoundSysData.SinTable[ ( ( SOUND_SINTABLE_DIV / 2 ) * T_M_I / 128 ) & ( SOUND_SINTABLE_DIV - 1 ) ] * 16384 / AngleI ;
							}
							CosI = SoundSysData.SinTable[ ( ( SOUND_SINTABLE_DIV * T_M_I / PITCHSHIFT_ANGLE_DIV_NUM ) + SOUND_SINTABLE_DIV / 4 ) & ( SOUND_SINTABLE_DIV - 1 ) ] ;

							SinCosI = ( SinCI * ( 64 * 128 + ( 64 * 128 * CosI ) / 16384 ) ) / 16384 ;

							DestInt  = ( int )DBuffer[ n * 2     ] + ( ( int )SBuffer[ m * 2     ] * SinCosI ) / 16384 ;
							     if( DestInt < -32768 )	DBuffer[ n * 2 ] = -32768 ;
							else if( DestInt >  32767 )	DBuffer[ n * 2 ] =  32767 ;
							else						DBuffer[ n * 2 ] = ( short )DestInt ;

							DestInt  = ( int )DBuffer[ n * 2 + 1 ] + ( ( int )SBuffer[ m * 2 + 1 ] * SinCosI ) / 16384 ;
							     if( DestInt < -32768 )	DBuffer[ n * 2 + 1 ] = -32768 ;
							else if( DestInt >  32767 )	DBuffer[ n * 2 + 1 ] =  32767 ;
							else						DBuffer[ n * 2 + 1 ] = ( short )DestInt ;
						}
					}
					break ;
				}
			}
			break ;

		case 8 :
			{
				BYTE *SBuffer ;
				BYTE *DBuffer ;

				SBuffer = ( BYTE * )SSSound->Wave.Buffer ;
				DBuffer = ( BYTE * )DSSound->Wave.Buffer ;

				_MEMSET( DBuffer, 127, sizeof( BYTE ) * DSSound->BufferFormat.nChannels * DSampleNum ) ;

				switch( SSSound->BufferFormat.nChannels )
				{
				case 1 :
					for( n = 0; n < DSampleNum ; n++ )
					{
						ta = mt ;
						tb = st == 0 ? ta : ta + 1 ;

						Start = tb - PITCHSHIFT_WIN_N / 2 ;
						End   = ta + PITCHSHIFT_WIN_N / 2 ;
						if( Start <           0 ) Start = 0 ;
						if( Start >= SSampleNum ) Start = SSampleNum - 1 ;
						if( End   <           0 ) End   = 0 ;
						if( End   >= SSampleNum ) End   = SSampleNum - 1 ;

						st += SSampleNum ;
						while( st >= DSampleNum )
						{
							st -= DSampleNum;
							mt++ ;
						}

						T_M_I = ( mt * 128 + st * 128 / DSampleNum ) - Start * 128 ;
						for( m = Start; m <= End; m++, T_M_I -= 128 )
						{
							AngleI = ( int )( DX_PI_F * 128.0f ) * T_M_I ;
							if( AngleI == 0 )
							{
								SinCI = 16384 ;
							}
							else
							{
								SinCI = SoundSysData.SinTable[ ( ( SOUND_SINTABLE_DIV / 2 ) * T_M_I / 128 ) & ( SOUND_SINTABLE_DIV - 1 ) ] * 16384 / AngleI ;
							}
							CosI = SoundSysData.SinTable[ ( ( SOUND_SINTABLE_DIV * T_M_I / PITCHSHIFT_ANGLE_DIV_NUM ) + SOUND_SINTABLE_DIV / 4 ) & ( SOUND_SINTABLE_DIV - 1 ) ] ;

							SinCosI = ( SinCI * ( 64 * 128 + ( 64 * 128 * CosI ) / 16384 ) ) / 16384 ;
							DestInt  = ( int )DBuffer[ n ] + ( ( ( int )SBuffer[ m ] - 127 ) * SinCosI ) / 16384 ;

							     if( DestInt <   0 )	DBuffer[ n ] = 0 ;
							else if( DestInt > 255 )	DBuffer[ n ] = 255 ;
							else						DBuffer[ n ] = ( BYTE )DestInt ;
						}
					}
					break ;

				case 2 :
					for( n = 0; n < DSampleNum ; n++ )
					{
						ta = mt ;
						tb = st == 0 ? ta : ta + 1 ;

						Start = tb - PITCHSHIFT_WIN_N / 2 ;
						End   = ta + PITCHSHIFT_WIN_N / 2 ;
						if( Start <           0 ) Start = 0 ;
						if( Start >= SSampleNum ) Start = SSampleNum - 1 ;
						if( End   <           0 ) End   = 0 ;
						if( End   >= SSampleNum ) End   = SSampleNum - 1 ;

						st += SSampleNum ;
						while( st >= DSampleNum )
						{
							st -= DSampleNum;
							mt++ ;
						}

						T_M_I = ( mt * 128 + st * 128 / DSampleNum ) - Start * 128 ;
						for( m = Start; m <= End; m++, T_M_I -= 128 )
						{
							AngleI = ( int )( DX_PI_F * 128.0f ) * T_M_I ;
							if( AngleI == 0 )
							{
								SinCI = 16384 ;
							}
							else
							{
								SinCI = SoundSysData.SinTable[ ( ( SOUND_SINTABLE_DIV / 2 ) * T_M_I / 128 ) & ( SOUND_SINTABLE_DIV - 1 ) ] * 16384 / AngleI ;
							}
							CosI = SoundSysData.SinTable[ ( ( SOUND_SINTABLE_DIV * T_M_I / PITCHSHIFT_ANGLE_DIV_NUM ) + SOUND_SINTABLE_DIV / 4 ) & ( SOUND_SINTABLE_DIV - 1 ) ] ;

							SinCosI = ( SinCI * ( 64 * 128 + ( 64 * 128 * CosI ) / 16384 ) ) / 16384 ;

							DestInt  = ( int )DBuffer[ n * 2     ] + ( ( ( int )SBuffer[ m * 2     ] - 127 ) * SinCosI ) / 16384 ;
							     if( DestInt <   0 )	DBuffer[ n * 2 ] = 0 ;
							else if( DestInt > 255 )	DBuffer[ n * 2 ] = 255 ;
							else						DBuffer[ n * 2 ] = ( BYTE )DestInt ;

							DestInt  = ( int )DBuffer[ n * 2 + 1 ] + ( ( ( int )SBuffer[ m * 2 + 1 ] - 127 ) * SinCosI ) / 16384 ;
							     if( DestInt <   0 )	DBuffer[ n * 2 + 1 ] = 0 ;
							else if( DestInt > 255 )	DBuffer[ n * 2 + 1 ] = 255 ;
							else						DBuffer[ n * 2 + 1 ] = ( BYTE )DestInt ;
						}
					}
					break ;
				}
			}
			break ;
		}
#endif
		break ;

	case WAVE_FORMAT_IEEE_FLOAT :
		{
			float *SBuffer ;
			float *DBuffer ;

			SBuffer = ( float * )SSSound->Wave.Buffer ;
			DBuffer = ( float * )DSSound->Wave.Buffer ;

			_MEMSET( DBuffer, 0, sizeof( float ) * SSSound->BufferFormat.nChannels * DSampleNum ) ;

			switch( SSSound->BufferFormat.nChannels )
			{
			case 1 :
				for( n = 0; n < DSampleNum ; n++ )
				{
					ta = mt ;
					tb = st == 0 ? ta : ta + 1 ;

					Start = tb - PITCHSHIFT_WIN_N / 2 ;
					End   = ta + PITCHSHIFT_WIN_N / 2 ;
					if( Start <           0 ) Start = 0 ;
					if( Start >= SSampleNum ) Start = SSampleNum - 1 ;
					if( End   <           0 ) End   = 0 ;
					if( End   >= SSampleNum ) End   = SSampleNum - 1 ;

					st += SSampleNum ;
					while( st >= DSampleNum )
					{
						st -= DSampleNum;
						mt++ ;
					}

					T_M_I = ( mt * 128 + st * 128 / DSampleNum ) - Start * 128 ;
					for( m = Start; m <= End; m++, T_M_I -= 128 )
					{
						AngleI = ( int )( DX_PI_F * 128.0f ) * T_M_I ;
						if( AngleI == 0 )
						{
							SinCI = 16384 ;
						}
						else
						{
							SinCI = SoundSysData.SinTable[ ( ( SOUND_SINTABLE_DIV / 2 ) * T_M_I / 128 ) & ( SOUND_SINTABLE_DIV - 1 ) ] * 16384 / AngleI ;
						}
						CosI = SoundSysData.SinTable[ ( ( SOUND_SINTABLE_DIV * T_M_I / PITCHSHIFT_ANGLE_DIV_NUM ) + SOUND_SINTABLE_DIV / 4 ) & ( SOUND_SINTABLE_DIV - 1 ) ] ;

						SinCosI = ( SinCI * ( 64 * 128 + ( 64 * 128 * CosI ) / 16384 ) ) / 16384 ;
						DBuffer[ n ] += ( SBuffer[ m ] * SinCosI ) / 16384.0f ;
					}
				}
				break ;

			case 2 :
				for( n = 0; n < DSampleNum ; n++ )
				{
					ta = mt ;
					tb = st == 0 ? ta : ta + 1 ;

					Start = tb - PITCHSHIFT_WIN_N / 2 ;
					End   = ta + PITCHSHIFT_WIN_N / 2 ;
					if( Start <           0 ) Start = 0 ;
					if( Start >= SSampleNum ) Start = SSampleNum - 1 ;
					if( End   <           0 ) End   = 0 ;
					if( End   >= SSampleNum ) End   = SSampleNum - 1 ;

					st += SSampleNum ;
					while( st >= DSampleNum )
					{
						st -= DSampleNum;
						mt++ ;
					}

					T_M_I = ( mt * 128 + st * 128 / DSampleNum ) - Start * 128 ;
					for( m = Start; m <= End; m++, T_M_I -= 128 )
					{
						AngleI = ( int )( DX_PI_F * 128.0f ) * T_M_I ;
						if( AngleI == 0 )
						{
							SinCI = 16384 ;
						}
						else
						{
							SinCI = SoundSysData.SinTable[ ( ( SOUND_SINTABLE_DIV / 2 ) * T_M_I / 128 ) & ( SOUND_SINTABLE_DIV - 1 ) ] * 16384 / AngleI ;
						}
						CosI = SoundSysData.SinTable[ ( ( SOUND_SINTABLE_DIV * T_M_I / PITCHSHIFT_ANGLE_DIV_NUM ) + SOUND_SINTABLE_DIV / 4 ) & ( SOUND_SINTABLE_DIV - 1 ) ] ;

						SinCosI = ( SinCI * ( 64 * 128 + ( 64 * 128 * CosI ) / 16384 ) ) / 16384 ;

						DBuffer[ n * 2     ] += ( SBuffer[ m * 2     ] * SinCosI ) / 16384.0f ;
						DBuffer[ n * 2 + 1 ] += ( SBuffer[ m * 2 + 1 ] * SinCosI ) / 16384.0f ;
					}
				}
				break ;
			}
		}
		break ;
	}

	// 正常終了
	return 0 ;
}

// ソフトウエアで扱う波形データの波形イメージが格納されているメモリアドレスを取得する
extern	void *NS_GetSoftSoundDataImage( int SoftSoundHandle )
{
	SOFTSOUND * SSound ;

	// エラー判定
	if( SSND_MASKHCHK_ASYNC( SoftSoundHandle, SSound ) ) return NULL ;
	if( SSound->IsPlayer == 1 ) return NULL ;

	// 終了
	return SSound->Wave.Buffer ;
}

// ソフトウエアで扱う波形データハンドルの指定の範囲を高速フーリエ変換を行い、各周波数域の振幅を取得する( SampleNum は 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536 の何れかである必要があります、Channel を -1 にすると二つのチャンネルを合成した結果になります )
extern int NS_GetFFTVibrationSoftSound( int SoftSoundHandle, int Channel, int SamplePosition, int SampleNum, float *Buffer, int BufferLength )
{
	return NS_GetFFTVibrationSoftSoundBase( SoftSoundHandle, Channel, SamplePosition, SampleNum, Buffer, NULL, BufferLength ) ;
}

// ソフトウエアで扱う波形データハンドルの指定の範囲を高速フーリエ変換を行い、各周波数域の振幅を取得する、結果の実数と虚数を別々に取得することができるバージョン( SampleNum は 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536 の何れかである必要があります、Channel を -1 にすると二つのチャンネルを合成した結果になります )
extern int NS_GetFFTVibrationSoftSoundBase( int SoftSoundHandle, int Channel, int SamplePosition, int SampleNum, float *RealBuffer_Array, float *ImagBuffer_Array, int BufferLength )
{
	SOFTSOUND * SSound ;
	int TableIndex ;
	float ( *Real_Imag )[ 2 ] ;
	float *TempBuffer ;
	int i, stage, number_of_stage, *index = NULL ;

	// エラー判定
	if( SSND_MASKHCHK_ASYNC( SoftSoundHandle, SSound ) ) return -1 ;
	if( SSound->IsPlayer == 1 ) return -1 ;

	// 配列を初期化
	_MEMSET( RealBuffer_Array, 0, sizeof( float ) * BufferLength ) ;
	if( ImagBuffer_Array != NULL )
	{
		_MEMSET( ImagBuffer_Array, 0, sizeof( float ) * BufferLength ) ;
	}

	if( SamplePosition >= SSound->Wave.BufferSampleNum )
	{
		return -1 ;
	}
	if( SamplePosition < 0 )
	{
		SamplePosition = 0 ;
	}
	if( SamplePosition + SampleNum > SSound->Wave.BufferSampleNum )
	{
		while( SampleNum > ( 1 << SOUND_FFT_BITCOUNT_MIN ) && SamplePosition + SampleNum > SSound->Wave.BufferSampleNum )
		{
			SampleNum /= 2 ;
		}
		if( SampleNum <= ( 1 << SOUND_FFT_BITCOUNT_MIN ) && SamplePosition + SampleNum > SSound->Wave.BufferSampleNum )
		{
			return -1 ;
		}
	}

	// 引数チェック
	for( i = SOUND_FFT_BITCOUNT_MIN ; i <= SOUND_FFT_BITCOUNT_MAX ; i ++ )
	{
		if( ( 1 << i ) == SampleNum )
		{
			break ;
		}
	}
	if( i == SOUND_FFT_BITCOUNT_MAX + 1 )
	{
		return -1 ;
	}
	TableIndex = i - 1 ;

	if( Channel > 1 )
	{
		return -1 ;
	}
	if( SSound->BufferFormat.nChannels == 1 && Channel < 0 )
	{
		Channel = 0 ;
	}

	// FFTの段数算出
	{
		int x = SampleNum ;

		for( number_of_stage = 0 ; x > 1 ; number_of_stage ++ )
		{
			x >>= 1 ;
		}
	}

	// 作業用バッファがまだ確保されていなかったら確保する
	if( SoundSysData.FFTVibrationWorkBuffer[ TableIndex ] == NULL )
	{
		// メモリを確保
		SoundSysData.FFTVibrationWorkBuffer[ TableIndex ] = DXALLOC( SampleNum * 2 * sizeof( float ) + SampleNum / 2 * sizeof( float ) + SampleNum * sizeof( int ) ) ;
		if( SoundSysData.FFTVibrationWorkBuffer[ TableIndex ] == NULL )
		{
			return -1 ;
		}

		// 参照用インデックスリストの初期化
		index = ( int * )( ( BYTE * )SoundSysData.FFTVibrationWorkBuffer[ TableIndex ] + SampleNum * 2 * sizeof( float ) + SampleNum / 2 * sizeof( float ) ) ;
		_MEMSET( index, 0, SampleNum * sizeof( int ) ) ;
		for( stage = 1 ; stage <= number_of_stage ; stage ++ )
		{
			int loopnum ;
			int offset ;

			loopnum = 1 << ( stage - 1 ) ;
			offset  = 1 << ( number_of_stage - stage ) ;
			for( i = 0; i < loopnum ; i++ )
			{
				index[ loopnum + i ] = index[ i ] + offset ;
			}
		}
	}

	// 作業用メモリのアドレスをセット
	Real_Imag = ( float (*)[ 2 ] )SoundSysData.FFTVibrationWorkBuffer[ TableIndex ] ;
	TempBuffer = ( float * )Real_Imag + SampleNum * 2 ;
	index = ( int * )( TempBuffer + SampleNum / 2 ) ;
	_MEMSET( Real_Imag, 0, sizeof( float ) * SampleNum * 2 ) ;

	// 変換元データの準備
	{
		BYTE *Src ;

		Src = ( BYTE * )SSound->Wave.Buffer + SSound->BufferFormat.nBlockAlign * SamplePosition ;
		if( Channel < 0 )
		{
			switch( SSound->BufferFormat.wBitsPerSample )
			{
			case 8 :
				for( i = 0 ; i < SampleNum ; i ++, Src += SSound->BufferFormat.nBlockAlign )
				{
					Real_Imag[ i ][ 0 ] = ( ( ( float )Src[ 0 ] - 127.0f ) + ( ( float )Src[ 1 ] - 127.0f ) ) / 256.0f ;
				}
				break ;

			case 16 :
				for( i = 0 ; i < SampleNum ; i ++, Src += SSound->BufferFormat.nBlockAlign )
				{
					Real_Imag[ i ][ 0 ] = ( ( float )( ( ( short * )Src )[ 0 ] ) + ( float )( ( ( short * )Src )[ 1 ] ) ) / 65536.0f ;
				}
				break ;
			}
		}
		else
		{
			switch( SSound->BufferFormat.wBitsPerSample )
			{
			case 8 :
				for( i = 0 ; i < SampleNum ; i ++, Src += SSound->BufferFormat.nBlockAlign )
				{
					Real_Imag[ i ][ 0 ] = ( ( float )Src[ Channel ] - 127.0f ) / 128.0f ;
				}
				break ;

			case 16 :
				for( i = 0 ; i < SampleNum ; i ++, Src += SSound->BufferFormat.nBlockAlign )
				{
					Real_Imag[ i ][ 0 ] = ( float )( ( ( short * )Src )[ Channel ] ) / 32768.0f ;
				}
				break ;
			}
		}
	}

	// バタフライ計算
	for( stage = 1 ; stage <= number_of_stage ; stage++ )
	{
		int loopnum1 ;
		int baseindex_m ;
		int baseindex_r ;

		baseindex_m = 1 << ( number_of_stage - stage ) ;
		baseindex_r = 1 << ( stage - 1 ) ;

		loopnum1 = 1 << ( stage - 1 ) ;
		for( i = 0 ; i < loopnum1 ; i++ )
		{
			int loopnum2 ;
			int baseindex_n ;
			float a_real, a_imag, b_real, b_imag ;
			int j, n, m, r ;

			baseindex_n = ( 1 << ( number_of_stage - stage + 1 ) ) * i ;

			loopnum2 = 1 << ( number_of_stage - stage ) ;
			if( stage < number_of_stage )
			{
				for( j = 0 ; j < loopnum2 ; j++ )
				{
					float Sin, Cos ;
					float c_real, c_imag ;

					n = baseindex_n + j ;
					m = baseindex_m + n ;
					r = baseindex_r * j ;

					a_real = Real_Imag[ n ][ 0 ] ;
					a_imag = Real_Imag[ n ][ 1 ] ;
					b_real = Real_Imag[ m ][ 0 ] ;
					b_imag = Real_Imag[ m ][ 1 ] ;

					_SINCOS_PLATFORM( ( 2.0f * DX_PI_F * r ) / SampleNum, &Sin, &Cos ) ;
					c_real =  Cos ;
					c_imag = -Sin ;

					Real_Imag[ n ][ 0 ] = a_real + b_real ;
					Real_Imag[ n ][ 1 ] = a_imag + b_imag ;
					Real_Imag[ m ][ 0 ] = ( a_real - b_real ) * c_real - ( a_imag - b_imag ) * c_imag ;
					Real_Imag[ m ][ 1 ] = ( a_imag - b_imag ) * c_real + ( a_real - b_real ) * c_imag ;
				}
			}
			else
			{
				for( j = 0 ; j < loopnum2 ; j++ )
				{
					n = baseindex_n + j ;
					m = baseindex_m + n ;

					a_real = Real_Imag[ n ][ 0 ] ;
					a_imag = Real_Imag[ n ][ 1 ] ;
					b_real = Real_Imag[ m ][ 0 ] ;
					b_imag = Real_Imag[ m ][ 1 ] ;

					Real_Imag[ n ][ 0 ] = a_real + b_real ;
					Real_Imag[ n ][ 1 ] = a_imag + b_imag ;
					Real_Imag[ m ][ 0 ] = a_real - b_real ;
					Real_Imag[ m ][ 1 ] = a_imag - b_imag ;
				}
			}
		}
	}

	// 振幅値に変換
	if( ImagBuffer_Array == NULL )
	{
		int loopnum ;
		float DivNum ;
		float RealT ;
		float ImagT ;

		loopnum = SampleNum / 2 ;
		DivNum = ( float )SampleNum / 2 ;
		for( i = 0; i < loopnum ; i++ )
		{
			RealT = Real_Imag[ index[ i ] ][ 0 ] / DivNum ;
			ImagT = Real_Imag[ index[ i ] ][ 1 ] / DivNum ;
			TempBuffer[ i ] = _SQRT( RealT * RealT + ImagT * ImagT ) ;
		}

		// 出力
		if( SampleNum / 2 == BufferLength )
		{
			for( i = 0 ; i < BufferLength ; i ++ )
			{
				RealBuffer_Array[ i ] = TempBuffer[ i ] ;
			}
		}
		else
		{
			float OneBufferNum ;
			int SrcOffset ;
			float Src ;
			float SrcRate ;
			float SetBufferNum ;

			OneBufferNum = ( float )( SampleNum / 2 ) / BufferLength ;

			SrcOffset = 0 ;
			Src = 0.0f ;
			SrcRate = 0.0f ;
			for( i = 0 ; i < BufferLength ; i ++ )
			{
				SetBufferNum = OneBufferNum ;
				while( SetBufferNum > 0.0000001f )
				{
					if( SrcRate < 0.0000001f )
					{
						Src = TempBuffer[ SrcOffset ] ;
						SrcRate += 1.0f ;
						SrcOffset ++ ;
					}

					if( SetBufferNum >= SrcRate )
					{
						RealBuffer_Array[ i ] += Src * SrcRate ;
						SetBufferNum -= SrcRate ;
						SrcRate = 0.0f ;
					}
					else
					{
						RealBuffer_Array[ i ] += Src * SetBufferNum ;
						SrcRate -= SetBufferNum ;
						SetBufferNum = 0.0f ;
					}
				}
				RealBuffer_Array[ i ] /= OneBufferNum ;
			}
		}
	}
	else
	{
		float DivNum ;

		DivNum = ( float )SampleNum / 2 ;

		// 出力
		if( SampleNum / 2 == BufferLength )
		{
			for( i = 0 ; i < BufferLength ; i ++ )
			{
				RealBuffer_Array[ i ] = Real_Imag[ index[ i ] ][ 0 ] / DivNum ;
				ImagBuffer_Array[ i ] = Real_Imag[ index[ i ] ][ 1 ] / DivNum ;
			}
		}
		else
		{
			float OneBufferNum ;
			int SrcOffset ;
			float SrcReal ;
			float SrcImag ;
			float SrcRate ;
			float SetBufferNum ;

			OneBufferNum = ( float )( SampleNum / 2 ) / BufferLength ;

			SrcOffset = 0 ;
			SrcReal = 0.0f ;
			SrcImag = 0.0f ;
			SrcRate = 0.0f ;
			for( i = 0 ; i < BufferLength ; i ++ )
			{
				SetBufferNum = OneBufferNum ;
				while( SetBufferNum > 0.0000001f )
				{
					if( SrcRate < 0.0000001f )
					{
						SrcReal = Real_Imag[ index[ SrcOffset ] ][ 0 ] / DivNum ;
						SrcImag = Real_Imag[ index[ SrcOffset ] ][ 1 ] / DivNum ;
						SrcRate += 1.0f ;
						SrcOffset ++ ;
					}

					if( SetBufferNum >= SrcRate )
					{
						RealBuffer_Array[ i ] += SrcReal * SrcRate ;
						ImagBuffer_Array[ i ] += SrcImag * SrcRate ;
						SetBufferNum -= SrcRate ;
						SrcRate = 0.0f ;
					}
					else
					{
						RealBuffer_Array[ i ] += SrcReal * SetBufferNum ;
						ImagBuffer_Array[ i ] += SrcImag * SetBufferNum ;
						SrcRate -= SetBufferNum ;
						SetBufferNum = 0.0f ;
					}
				}
				RealBuffer_Array[ i ] /= OneBufferNum ;
				ImagBuffer_Array[ i ] /= OneBufferNum ;
			}
		}
	}

	// 終了
	return 0 ;
}

// ソフトウエアで扱う波形データのプレイヤーをすべて解放する
extern	int NS_InitSoftSoundPlayer( void )
{
	if( SoundSysData.InitializeFlag == FALSE )
		return -1 ;

	return AllHandleSub( DX_HANDLETYPE_SOFTSOUND, DeleteCancelCheckSoftSoundPlayerFunction );
}

// ソフトウエアで扱う波形データのプレイヤーを作成する( フォーマットは引数のソフトウエアサウンドハンドルと同じものにする )
extern int NS_MakeSoftSoundPlayer( int UseFormat_SoftSoundHandle )
{
	if( UseFormat_SoftSoundHandle < 0 )
		return -1 ;

	return MakeSoftSoundBase_UseGParam( TRUE, 0, 0, 0, 0, FALSE, UseFormat_SoftSoundHandle, GetASyncLoadFlag() ) ;
}

// ソフトウエアで扱う波形データのプレイヤーを作成する( チャンネル数:2 量子化ビット数:16bit サンプリング周波数:44.1KHz )
extern	int NS_MakeSoftSoundPlayer2Ch16Bit44KHz( void )
{
	return MakeSoftSoundBase_UseGParam( TRUE, 2, 16, 44100, 0, FALSE, -1, GetASyncLoadFlag() ) ;
}

// ソフトウエアで扱う波形データのプレイヤーを作成する( チャンネル数:2 量子化ビット数:16bit サンプリング周波数:22KHz )
extern	int NS_MakeSoftSoundPlayer2Ch16Bit22KHz( void )
{
	return MakeSoftSoundBase_UseGParam( TRUE, 2, 16, 22050, 0, FALSE, -1, GetASyncLoadFlag() ) ;
}

// ソフトウエアで扱う波形データのプレイヤーを作成する( チャンネル数:2 量子化ビット数:8bit サンプリング周波数:44.1KHz )
extern	int NS_MakeSoftSoundPlayer2Ch8Bit44KHz( void )
{
	return MakeSoftSoundBase_UseGParam( TRUE, 2, 8, 44100, 0, FALSE, -1, GetASyncLoadFlag() ) ;
}

// ソフトウエアで扱う波形データのプレイヤーを作成する( チャンネル数:2 量子化ビット数:8bit サンプリング周波数:22KHz )
extern	int NS_MakeSoftSoundPlayer2Ch8Bit22KHz( void )
{
	return MakeSoftSoundBase_UseGParam( TRUE, 2, 8, 22050, 0, FALSE, -1, GetASyncLoadFlag() ) ;
}

// ソフトウエアで扱う波形データのプレイヤーを作成する( チャンネル数:1 量子化ビット数:16bit サンプリング周波数:44.1KHz )
extern	int NS_MakeSoftSoundPlayer1Ch16Bit44KHz( void )
{
	return MakeSoftSoundBase_UseGParam( TRUE, 1, 16, 44100, 0, FALSE, -1, GetASyncLoadFlag() ) ;
}

// ソフトウエアで扱う波形データのプレイヤーを作成する( チャンネル数:1 量子化ビット数:16bit サンプリング周波数:22KHz )
extern	int NS_MakeSoftSoundPlayer1Ch16Bit22KHz( void )
{
	return MakeSoftSoundBase_UseGParam( TRUE, 1, 16, 22050, 0, FALSE, -1, GetASyncLoadFlag() ) ;
}

// ソフトウエアで扱う波形データのプレイヤーを作成する( チャンネル数:1 量子化ビット数:8bit サンプリング周波数:44.1KHz )
extern	int NS_MakeSoftSoundPlayer1Ch8Bit44KHz( void )
{
	return MakeSoftSoundBase_UseGParam( TRUE, 1, 8, 44100, 0, FALSE, -1, GetASyncLoadFlag() ) ;
}

// ソフトウエアで扱う波形データのプレイヤーを作成する( チャンネル数:1 量子化ビット数:8bit サンプリング周波数:22KHz )
extern	int NS_MakeSoftSoundPlayer1Ch8Bit22KHz( void )
{
	return MakeSoftSoundBase_UseGParam( TRUE, 1, 8, 22050, 0, FALSE, -1, GetASyncLoadFlag() ) ;
}

// ソフトウエアで扱う波形データのプレイヤーを作成する
extern	int NS_MakeSoftSoundPlayerCustom( int ChannelNum, int BitsPerSample, int SamplesPerSec )
{
	return MakeSoftSoundBase_UseGParam( TRUE, ChannelNum, BitsPerSample, SamplesPerSec, 0, FALSE, -1, GetASyncLoadFlag() ) ;
}

// ソフトウエアで扱う波形データのプレイヤーを削除する
extern	int NS_DeleteSoftSoundPlayer( int SSoundPlayerHandle )
{
	SOFTSOUND *SSound ;

	if( SSND_MASKHCHK_ASYNC( SSoundPlayerHandle, SSound ) ) return -1 ;

	// プレイヤーかどうかのチェック
	if( SSound->IsPlayer != TRUE )
	{
		// 違ったらエラー
		return -1 ;
	}

	return SubHandle( SSoundPlayerHandle ) ;
}

// ソフトウエアで扱う波形データのプレイヤーに波形データを追加する( フォーマットが同じではない場合はエラー )
extern	int NS_AddDataSoftSoundPlayer( int SSoundPlayerHandle, int SoftSoundHandle, int AddSamplePosition, int AddSampleNum )
{
	SOFTSOUND *SPlayer, *SSound ;
	void *Src ;

	if( SoundSysData.InitializeFlag == FALSE ) return -1 ;

	// エラー判定
	if( SSND_MASKHCHK( SoftSoundHandle, SSound ) ) return -1 ;
	if( SSound->IsPlayer == 1 ) return -1 ;

	if( SSND_MASKHCHK( SSoundPlayerHandle, SPlayer ) ) return -1 ;
	if( SPlayer->IsPlayer == 0 ) return -1 ;

	// フォーマットが違ったらエラー
	if( SSound->BufferFormat.nChannels != SPlayer->BufferFormat.nChannels ||
		SSound->BufferFormat.wBitsPerSample != SPlayer->BufferFormat.wBitsPerSample ||
		SSound->BufferFormat.nSamplesPerSec != SPlayer->BufferFormat.nSamplesPerSec )
		return -1 ;

	// サンプル位置の指定が間違っていたらエラー
	if( AddSampleNum == 0 || SSound->Wave.BufferSampleNum < AddSamplePosition + AddSampleNum )
		return -1 ; 

	// 転送位置のアドレスを算出
	Src = ( BYTE * )SSound->Wave.Buffer + SSound->BufferFormat.nBlockAlign * AddSamplePosition ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

	// リングバッファにデータを追加
	if( RingBufDataAdd( &SPlayer->Player.StockSample, Src, AddSampleNum * SSound->BufferFormat.nBlockAlign ) < 0 )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;
		return -1 ;
	}

	// 追加した分サンプル数を増やす
	SPlayer->Player.StockSampleNum += AddSampleNum ;

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

	// 終了
	return 0 ;
}

// ソフトウエアで扱う波形データのプレイヤーにプレイヤーが対応したフォーマットの生波形データを追加する
extern	int	NS_AddDirectDataSoftSoundPlayer( int SSoundPlayerHandle, const void *SoundData, int AddSampleNum )
{
	SOFTSOUND * SPlayer ;

	if( SoundSysData.InitializeFlag == FALSE ) return -1 ;

	// エラー判定
	if( SSND_MASKHCHK( SSoundPlayerHandle, SPlayer ) ) return -1 ;
	if( SPlayer->IsPlayer == 0 ) return -1 ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

	// リングバッファにデータを追加
	if( RingBufDataAdd( &SPlayer->Player.StockSample, SoundData, AddSampleNum * SPlayer->BufferFormat.nBlockAlign ) < 0 )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;
		return -1 ;
	}

	// 追加した分サンプル数を増やす
	SPlayer->Player.StockSampleNum += AddSampleNum ;

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

	// 終了
	return 0 ;
}

// ソフトウエアで扱う波形データのプレイヤーに波形データを一つ追加する
extern int NS_AddOneDataSoftSoundPlayer( int SSoundPlayerHandle, int Channel1, int Channel2 )
{
	SOFTSOUND * SPlayer ;
	BYTE Dest[ 16 ] ;

	if( SoundSysData.InitializeFlag == FALSE ) return -1 ;

	// エラー判定
	if( SSND_MASKHCHK( SSoundPlayerHandle, SPlayer ) ) return -1 ;
	if( SPlayer->IsPlayer == 0 ) return -1 ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

	// データの準備
	switch( SPlayer->BufferFormat.wBitsPerSample )
	{
	case 8 :
		     if( Channel1 > 255 ) Channel1 = 255 ;
		else if( Channel1 <   0 ) Channel1 =   0 ;
		Dest[ 0 ] = ( BYTE )Channel1 ;

		if( SPlayer->BufferFormat.nChannels != 1 )
		{
				 if( Channel2 > 255 ) Channel2 = 255 ;
			else if( Channel2 <   0 ) Channel2 =   0 ;
			Dest[ 1 ] = ( BYTE )Channel2 ;
		}
		break ;

	case 16 :
			 if( Channel1 >  32767 ) Channel1 =  32767 ;
		else if( Channel1 < -32768 ) Channel1 = -32768 ;
		( ( short * )Dest )[ 0 ] = ( short )Channel1 ;

		if( SPlayer->BufferFormat.nChannels != 1 )
		{
				 if( Channel2 >  32767 ) Channel2 =  32767 ;
			else if( Channel2 < -32768 ) Channel2 = -32768 ;
			( ( short * )Dest )[ 1 ] = ( short )Channel2 ;
		}
		break ;
	}

	// リングバッファにデータを追加
	if( RingBufDataAdd( &SPlayer->Player.StockSample, Dest, SPlayer->BufferFormat.nBlockAlign ) < 0 )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;
		return -1 ;
	}

	// サンプル数を一つ増やす
	SPlayer->Player.StockSampleNum ++ ;

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

	// 終了
	return 0 ;
}


// ソフトウエアで扱う波形データのプレイヤーに再生用サウンドバッファに転送していない波形データが無く、再生用サウンドバッファにも無音データ以外無いかどうかを取得する( TRUE:無音データ以外無い  FALSE:有効データがある )
extern	int	NS_CheckSoftSoundPlayerNoneData( int SSoundPlayerHandle )
{
	SOFTSOUND * SPlayer ;
	int Result = -1 ;

	if( SoundSysData.InitializeFlag == FALSE ) return -1 ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

	// エラー判定
	if( SSND_MASKHCHK( SSoundPlayerHandle, SPlayer ) || SPlayer->IsPlayer == 0 )
		goto END ;

	// 更新処理を行う
	_SoftSoundPlayerProcess( SPlayer ) ;

	// 無音データ再生中かどうかのフラグをセット
	Result = SPlayer->Player.NoneDataPlayStartFlag ;

	// 音データのストックがある場合は無音データ再生中ではない
	if( SPlayer->Player.StockSampleNum != 0 )
		Result = FALSE ;

END :
	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

	// 状態を返す
	return Result ;
}

// ソフトウエアで扱う波形データのプレイヤーに追加した波形データでまだ再生用サウンドバッファに転送されていない波形データのサンプル数を取得する
extern	int	NS_GetStockDataLengthSoftSoundPlayer( int SSoundPlayerHandle )
{
	SOFTSOUND * SPlayer ;

	// エラー判定
	if( SSND_MASKHCHK( SSoundPlayerHandle, SPlayer ) || SPlayer->IsPlayer == 0 )
		return -1 ;

	// リングバッファにある未転送分のサンプルを返す
	return SPlayer->Player.StockSampleNum ;
}

// ソフトウエアで扱う波形データのプレイヤーが扱うデータフォーマットを取得する
extern	int	NS_GetSoftSoundPlayerFormat( int SSoundPlayerHandle, int *Channels, int *BitsPerSample, int *SamplesPerSec )
{
	SOFTSOUND * SPlayer ;

	// エラー判定
	if( SSND_MASKHCHK( SSoundPlayerHandle, SPlayer ) ) return -1 ;
	if( SPlayer->IsPlayer == 0 ) return -1 ;

	// データをセット
	if( Channels      ) *Channels      = SPlayer->BufferFormat.nChannels ;
	if( BitsPerSample ) *BitsPerSample = SPlayer->BufferFormat.wBitsPerSample ;
	if( SamplesPerSec ) *SamplesPerSec = ( int )SPlayer->BufferFormat.nSamplesPerSec ;

	// 終了
	return 0 ;
}

// ソフトウエアで扱う波形データのプレイヤーの再生処理を開始する
extern	int NS_StartSoftSoundPlayer( int SSoundPlayerHandle )
{
	SOFTSOUND * SPlayer ;

	if( SoundSysData.InitializeFlag == FALSE ) return -1 ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

	// エラー判定
	if( SSND_MASKHCHK( SSoundPlayerHandle, SPlayer ) || SPlayer->IsPlayer == 0 )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

		return -1 ;
	}

	// 既に再生されていたら何もしない
	if( SPlayer->Player.IsPlayFlag )
		goto END ;

	// 再生状態にする
	SPlayer->Player.IsPlayFlag = TRUE ;

	// 再生用データをサウンドバッファにセットする
	_SoftSoundPlayerProcess( SPlayer ) ;

	// サウンドバッファの再生開始
	if( SoundBuffer_Play( &SPlayer->Player.SoundBuffer, TRUE ) < 0 )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;
		return -1 ;
	}

END :

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

	// 終了
	return 0 ;
}

// ソフトウエアで扱う波形データのプレイヤーの再生処理が開始されているか取得する( TRUE:開始している  FALSE:停止している )
extern	int	NS_CheckStartSoftSoundPlayer( int SSoundPlayerHandle )
{
	SOFTSOUND * SPlayer ;

	// エラー判定
	if( SSND_MASKHCHK( SSoundPlayerHandle, SPlayer ) || SPlayer->IsPlayer == 0 )
		return -1 ;

	// 再生中フラグを返す
	return SPlayer->Player.IsPlayFlag ;
}


// ソフトウエアで扱う波形データのプレイヤーの再生処理を停止する
extern	int NS_StopSoftSoundPlayer( int SSoundPlayerHandle )
{
	SOFTSOUND * SPlayer ;

	if( SoundSysData.InitializeFlag == FALSE ) return -1 ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

	// エラー判定
	if( SSND_MASKHCHK( SSoundPlayerHandle, SPlayer ) || SPlayer->IsPlayer == 0 )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

		return -1 ;
	}

	// 再生されていなかったら何もしない
	if( SPlayer->Player.IsPlayFlag == FALSE )
		goto END ;

	// 再生を止める
	SoundBuffer_Stop( &SPlayer->Player.SoundBuffer, TRUE ) ;

	// 再生中フラグを倒す
	SPlayer->Player.IsPlayFlag = FALSE ;

END :

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

	// 終了
	return 0 ;
}

// ソフトウエアで扱う波形データのプレイヤーの状態を初期状態に戻す( 追加された波形データは削除され、再生状態だった場合は停止する )
extern	int NS_ResetSoftSoundPlayer( int SSoundPlayerHandle )
{
	SOFTSOUND * SPlayer ;

	if( SoundSysData.InitializeFlag == FALSE ) return -1 ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

	// エラー判定
	if( SSND_MASKHCHK( SSoundPlayerHandle, SPlayer ) || SPlayer->IsPlayer == 0 )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

		return -1 ;
	}

	// 再生を止める
	SoundBuffer_Stop( &SPlayer->Player.SoundBuffer, TRUE ) ;

	// 再生位置を先頭に戻す
	SoundBuffer_SetCurrentPosition( &SPlayer->Player.SoundBuffer, 0 ) ;

	// リングバッファを再初期化する
	RingBufTerminate( &SPlayer->Player.StockSample ) ;
	RingBufInitialize( &SPlayer->Player.StockSample ) ;
	SPlayer->Player.StockSampleNum = 0 ;

	// その他情報を初期化する
	SPlayer->Player.DataSetCompOffset = 0 ;
	SPlayer->Player.NoneDataSetCompOffset = 0 ;
	SPlayer->Player.NoneDataPlayCheckBackPlayOffset = 0 ;
	SPlayer->Player.NoneDataPlayStartFlag = FALSE ;
	SPlayer->Player.IsPlayFlag = FALSE ;

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

	// 終了
	return 0 ;
}

// 全てのソフトウエアサウンドの一時停止状態を変更する
extern int PauseSoftSoundAll( int PauseFlag )
{
	HANDLELIST *List ;
	SOFTSOUND * SPlayer ;

	if( SoundSysData.InitializeFlag == FALSE )
	{
		return -1 ;
	}

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

	// 停止するか開始するかで処理を分岐
	if( PauseFlag )
	{
		// 停止する場合

		for( List = SoundSysData.SoftSoundPlayerListFirst.Next ; List->Next != NULL ; List = List->Next )
		{
			SPlayer = ( SOFTSOUND * )List->Data ;

			if( SPlayer->Player.SoundBuffer.Valid == FALSE || SoundBuffer_CheckEnable( &SPlayer->Player.SoundBuffer ) == FALSE )
				continue ;

			// 既に状態保存済みの場合は何もしない
			if( SPlayer->Player.SoundBufferPlayStateBackupFlagValid )
				continue ;

			// サウンドバッファの再生状態を保存
			{
				int IsPlay ;

				IsPlay = SoundBuffer_CheckPlay( &SPlayer->Player.SoundBuffer ) ;
				if( IsPlay != -1 )
				{
					SPlayer->Player.SoundBufferPlayStateBackupFlagValid = TRUE ;

					if( IsPlay )
					{
						SPlayer->Player.SoundBufferPlayStateBackupFlag = TRUE ;

						// 再生されていたら再生を止める
						SoundBuffer_Stop( &SPlayer->Player.SoundBuffer, TRUE ) ;
					}
					else
					{
						SPlayer->Player.SoundBufferPlayStateBackupFlag = FALSE ;
					}
				}
				else
				{
					SPlayer->Player.SoundBufferPlayStateBackupFlagValid = FALSE ;
				}
			}
		}
	}
	else
	{
		// 再開する場合

		for( List = SoundSysData.SoftSoundPlayerListFirst.Next ; List->Next != NULL ; List = List->Next )
		{
			SPlayer = ( SOFTSOUND * )List->Data ;

			// 再生中ではない場合のみ処理
			if( SPlayer->Player.IsPlayFlag )
			{
				if( SPlayer->Player.SoundBuffer.Valid == FALSE || SoundBuffer_CheckEnable( &SPlayer->Player.SoundBuffer ) == FALSE ) continue ;

				// サウンドバッファの再生状態が有効で、且つ再生していた場合は再生を再開する
				if( SPlayer->Player.SoundBufferPlayStateBackupFlagValid &&
					SPlayer->Player.SoundBufferPlayStateBackupFlag )
				{
					SoundBuffer_Play( &SPlayer->Player.SoundBuffer, SPlayer->Player.SoundBuffer.Loop ) ;
				}
			}

			SPlayer->Player.SoundBufferPlayStateBackupFlagValid = FALSE ;
		}
	}

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

	// 終了
	return 0 ;
}

// サウンドデータを別のサウンドデータにコピーする
extern int CopySoftSound( int SrcSoftSoundHandle, int DestSoftSoundHandle )
{
	SOFTSOUND * SSSound ;
	SOFTSOUND * DSSound ;

	// エラー判定
	if( SSND_MASKHCHK_ASYNC( SrcSoftSoundHandle, SSSound ) ) return -1 ;
	if( SSSound->IsPlayer == 1 ) return -1 ;

	if( SSND_MASKHCHK_ASYNC( DestSoftSoundHandle, DSSound ) ) return -1 ;
	if( DSSound->IsPlayer == 1 ) return -1 ;

	// 元データと転送先のサンプル数かチャンネル数かビット数が異なる場合はエラー
	if( SSSound->Wave.BufferSampleNum        != DSSound->Wave.BufferSampleNum   ||
		SSSound->BufferFormat.nChannels      != DSSound->BufferFormat.nChannels ||
		SSSound->BufferFormat.wBitsPerSample != DSSound->BufferFormat.wBitsPerSample )
	{
		return -1 ;
	}

	// 元データと転送先のフォーマットが異なる場合はエラー
	if( SSSound->BufferFormat.wFormatTag != DSSound->BufferFormat.wFormatTag )
	{
		return -1 ;
	}

	// データをコピー
	_MEMCPY( DSSound->Wave.Buffer, SSSound->Wave.Buffer, SSSound->Wave.BufferSampleNum * SSSound->BufferFormat.nBlockAlign ) ;

	// 正常終了
	return 0 ;
}

// int型のサウンドデータから float型のサウンドデータをセットアップする
extern int ConvertIntToFloatSoftSound( int SrcSoftSoundHandle, int DestSoftSoundHandle )
{
	SOFTSOUND * SSSound ;
	SOFTSOUND * DSSound ;
	int i ;
	int SrcSampleNum ;
	float *Dest ;

	// エラー判定
	if( SSND_MASKHCHK_ASYNC( SrcSoftSoundHandle, SSSound ) ) return -1 ;
	if( SSSound->IsPlayer == 1 ) return -1 ;

	if( SSND_MASKHCHK_ASYNC( DestSoftSoundHandle, DSSound ) ) return -1 ;
	if( DSSound->IsPlayer == 1 ) return -1 ;

	// 元データと転送先のサンプル数かチャンネル数が異なる場合はエラー
	if( SSSound->Wave.BufferSampleNum   != DSSound->Wave.BufferSampleNum ||
		SSSound->BufferFormat.nChannels != DSSound->BufferFormat.nChannels )
	{
		return -1 ;
	}

	// 元データが float 型の場合はエラー
	if( SSSound->BufferFormat.wFormatTag == WAVE_FORMAT_IEEE_FLOAT )
	{
		return -1 ;
	}

	// 転送先が float 型ではない場合はエラー
	if( DSSound->BufferFormat.wFormatTag != WAVE_FORMAT_IEEE_FLOAT )
	{
		return -1 ;
	}

	SrcSampleNum = SSSound->Wave.BufferSampleNum ;
	Dest = ( float * )DSSound->Wave.Buffer ;
	switch( SSSound->BufferFormat.wBitsPerSample )
	{
	case 8 :
		{
			BYTE *Src ;

			Src = ( BYTE * )SSSound->Wave.Buffer ;
			if( SSSound->BufferFormat.nChannels == 1 )
			{
				for( i = 0 ; i < SrcSampleNum ; i ++ )
				{
					Dest[ i ] = ( float )( ( int )Src[ i ] - 127 ) / 128.0f ;
				}
			}
			else
			{
				for( i = 0 ; i < SrcSampleNum ; i ++, Src += 2, Dest += 2 )
				{
					Dest[ 0 ] = ( float )( ( int )Src[ 0 ] - 127 ) / 128.0f ;
					Dest[ 1 ] = ( float )( ( int )Src[ 1 ] - 127 ) / 128.0f ;
				}
			}
		}
		break ;

	case 16 :
		{
			short *Src ;

			Src = ( short * )SSSound->Wave.Buffer ;
			if( SSSound->BufferFormat.nChannels == 1 )
			{
				for( i = 0 ; i < SrcSampleNum ; i ++ )
				{
					Dest[ i ] = ( float )Src[ i ] / 32768.0f ;
				}
			}
			else
			{
				for( i = 0 ; i < SrcSampleNum ; i ++, Src += 2, Dest += 2 )
				{
					Dest[ 0 ] = ( float )Src[ 0 ] / 32768.0f ;
					Dest[ 1 ] = ( float )Src[ 1 ] / 32768.0f ;
				}
			}
		}
		break ;
	}

	// 正常終了
	return 0 ;
}

// float型のサウンドデータから int型のサウンドデータをセットアップする
extern int ConvertFloatToIntSoftSound( int SrcSoftSoundHandle, int DestSoftSoundHandle )
{
	SOFTSOUND * SSSound ;
	SOFTSOUND * DSSound ;
	int i ;
	int SrcSampleNum ;
	float *Src ;
	int ChannelI ;

	// エラー判定
	if( SSND_MASKHCHK_ASYNC( SrcSoftSoundHandle, SSSound ) ) return -1 ;
	if( SSSound->IsPlayer == 1 ) return -1 ;

	if( SSND_MASKHCHK_ASYNC( DestSoftSoundHandle, DSSound ) ) return -1 ;
	if( DSSound->IsPlayer == 1 ) return -1 ;

	// 元データと転送先のサンプル数かチャンネル数が異なる場合はエラー
	if( SSSound->Wave.BufferSampleNum   != DSSound->Wave.BufferSampleNum ||
		SSSound->BufferFormat.nChannels != DSSound->BufferFormat.nChannels )
	{
		return -1 ;
	}

	// 元データが int 型の場合はエラー
	if( SSSound->BufferFormat.wFormatTag == WAVE_FORMAT_PCM )
	{
		return -1 ;
	}

	// 転送先が int 型ではない場合はエラー
	if( DSSound->BufferFormat.wFormatTag != WAVE_FORMAT_PCM )
	{
		return -1 ;
	}

	SrcSampleNum = SSSound->Wave.BufferSampleNum ;
	Src = ( float * )SSSound->Wave.Buffer ;
	switch( DSSound->BufferFormat.wBitsPerSample )
	{
	case 8 :
		{
			BYTE *Dest ;

			Dest = ( BYTE * )DSSound->Wave.Buffer ;
			if( SSSound->BufferFormat.nChannels == 1 )
			{
				for( i = 0 ; i < SrcSampleNum ; i ++ )
				{
					ChannelI = _FTOL( Src[ i ] * 128.0f + 127.0f  ) ;
						 if( ChannelI > 255 ) ChannelI = 255 ;
					else if( ChannelI <   0 ) ChannelI =   0 ;
					Dest[ i ] = ( BYTE )ChannelI ;
				}
			}
			else
			{
				for( i = 0 ; i < SrcSampleNum ; i ++, Src += 2, Dest += 2 )
				{
					ChannelI = _FTOL( Src[ 0 ] * 128.0f + 127.0f  ) ;
						 if( ChannelI > 255 ) ChannelI = 255 ;
					else if( ChannelI <   0 ) ChannelI =   0 ;
					Dest[ 0 ] = ( BYTE )ChannelI ;

					ChannelI = _FTOL( Src[ 1 ] * 128.0f + 127.0f  ) ;
						 if( ChannelI > 255 ) ChannelI = 255 ;
					else if( ChannelI <   0 ) ChannelI =   0 ;
					Dest[ 1 ] = ( BYTE )ChannelI ;
				}
			}
		}
		break ;

	case 16 :
		{
			short *Dest ;

			Dest = ( short * )DSSound->Wave.Buffer ;
			if( SSSound->BufferFormat.nChannels == 1 )
			{
				for( i = 0 ; i < SrcSampleNum ; i ++ )
				{
					ChannelI = _FTOL( Src[ i ] * 32768.0f  ) ;
						 if( ChannelI >  32767 ) ChannelI =  32767 ;
					else if( ChannelI < -32768 ) ChannelI = -32768 ;
					Dest[ i ] = ( short )ChannelI ;
				}
			}
			else
			{
				for( i = 0 ; i < SrcSampleNum ; i ++, Src += 2, Dest += 2 )
				{
					ChannelI = _FTOL( Src[ 0 ] * 32768.0f  ) ;
						 if( ChannelI >  32767 ) ChannelI =  32767 ;
					else if( ChannelI < -32768 ) ChannelI = -32768 ;
					Dest[ 0 ] = ( short )ChannelI ;

					ChannelI = _FTOL( Src[ 1 ] * 32768.0f  ) ;
						 if( ChannelI >  32767 ) ChannelI =  32767 ;
					else if( ChannelI < -32768 ) ChannelI = -32768 ;
					Dest[ 1 ] = ( short )ChannelI ;
				}
			}
		}
		break ;
	}

	// 正常終了
	return 0 ;
}

// ソフトウエアで制御する全てのサウンドデータプレイヤーの定期処理を行う
extern	int ST_SoftSoundPlayerProcessAll( void )
{
	HANDLELIST *List ;
	SOFTSOUND * SPlayer ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	for( List = SoundSysData.SoftSoundPlayerListFirst.Next ; List->Next != NULL ; List = List->Next )
	{
		SPlayer = ( SOFTSOUND * )List->Data ;

		// 再生中ではない場合は何もせず次へ
		if( SPlayer->Player.IsPlayFlag == FALSE ) continue ;

		_SoftSoundPlayerProcess( SPlayer ) ;
	}

	// 終了
	return 0 ;
}

// ソフトウエアで制御するサウンドデータプレイヤーの定期処理
static int _SoftSoundPlayerProcess( SOFTSOUND * SPlayer )
{
	int WriteSize, NoneWriteSize, WriteStartPos ;
	DWORD PlayPos, WritePos ;
	int hr ;
	int Result = -1 ;
	DWORD MoveSize, MoveTempSize ;
	DWORD C, S, P, N ;
	SOUNDBUFFERLOCKDATA LockData ;

	if( SoundSysData.InitializeFlag == FALSE ) return -1 ;

	// 再生状態ではない場合は何もしない
	if( SPlayer->Player.IsPlayFlag == FALSE ) return 0 ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

	// 再生位置の取得
	SoundBuffer_GetCurrentPosition( &SPlayer->Player.SoundBuffer, &PlayPos, &WritePos ) ;

	// 無音データ再生チェック
	if( SPlayer->Player.NoneDataSetCompOffset != -1 && SPlayer->Player.NoneDataPlayStartFlag == FALSE )
	{
		P = ( DWORD )SPlayer->Player.NoneDataPlayCheckBackPlayOffset ;
		N = PlayPos ;
		if( ( N > P && ( P <= ( DWORD )SPlayer->Player.NoneDataSetCompOffset && N >= ( DWORD )SPlayer->Player.NoneDataSetCompOffset ) ) ||
			( N < P && ( P <= ( DWORD )SPlayer->Player.NoneDataSetCompOffset || N >= ( DWORD )SPlayer->Player.NoneDataSetCompOffset ) ) )
		{
			SPlayer->Player.NoneDataPlayStartFlag = TRUE ;
		}
		else
		{
			SPlayer->Player.NoneDataPlayCheckBackPlayOffset = ( int )N ;
		}
	}

	// 転送する必要があるかどうかを調べる
	WriteStartPos = ( int )SPlayer->Player.DataSetCompOffset - ( int )SPlayer->Player.MinDataSetSize ;
	if( WriteStartPos < 0 )
	{
		WriteStartPos += SPlayer->Player.SoundBufferSize ;
	}
	C = ( DWORD )SPlayer->Player.DataSetCompOffset ;
	S = ( DWORD )WriteStartPos ;
	if( ( S > C && ( S > PlayPos && C < PlayPos ) ) ||
		( S < C && ( S > PlayPos || C < PlayPos ) ) )
	{
		Result = 0 ;
		goto END ;
	}

	// 転送するサイズを算出する
	WriteSize = 0 ;
	if( S > C )
	{
		if( S < PlayPos )
		{
			WriteSize = ( int )( PlayPos - S ) ;
		}
		else
		{
			WriteSize = ( int )( ( SPlayer->Player.SoundBufferSize - S ) + PlayPos ) ;
		}
	}
	else
	{
		WriteSize = ( int )( PlayPos - S ) ;
	}
	WriteSize += SPlayer->Player.MinDataSetSize ;
	WriteSize /= SPlayer->BufferFormat.nBlockAlign ;
	if( WriteSize < 0 )
	{
		Result = 0 ;
		goto END ;
	}

	// サウンドバッファに転送する有効データのサイズと無効データのサイズを算出
	NoneWriteSize = 0 ;
	if( WriteSize > SPlayer->Player.StockSampleNum )
	{
		if( SPlayer->Player.StockSampleNum <= 0 )
		{
			NoneWriteSize = WriteSize ;
			WriteSize = 0 ;
		}
		else
		{
			WriteSize = SPlayer->Player.StockSampleNum ;
		}
	}

	// サウンドバッファをロックする
	MoveSize = ( DWORD )( ( NoneWriteSize + WriteSize ) * SPlayer->BufferFormat.nBlockAlign ) ;
	hr = SoundBuffer_Lock(
		&SPlayer->Player.SoundBuffer, ( DWORD )SPlayer->Player.DataSetCompOffset,
		MoveSize,
		( void ** )&LockData.WriteP,  &LockData.Length,
		( void ** )&LockData.WriteP2, &LockData.Length2 ) ;
	if( hr != 0 )
		goto END ;

	// 無音データをセットする場合は無音データのセットを開始したオフセットを保存する
	if( NoneWriteSize != 0 && SPlayer->Player.NoneDataSetCompOffset == -1 )
	{
		SPlayer->Player.NoneDataSetCompOffset = SPlayer->Player.DataSetCompOffset + WriteSize * SPlayer->BufferFormat.nBlockAlign ;
		if( SPlayer->Player.NoneDataSetCompOffset > SPlayer->Player.SoundBufferSize )
		{
			SPlayer->Player.NoneDataSetCompOffset -= SPlayer->Player.SoundBufferSize ;
		}
		SPlayer->Player.NoneDataPlayCheckBackPlayOffset = ( int )PlayPos ;
		SPlayer->Player.NoneDataPlayStartFlag           = FALSE ;
	}

	// 無音データ再生中に有効データを転送する場合は無音データ再生フラグを倒す
	if( SPlayer->Player.NoneDataPlayStartFlag && WriteSize != 0 )
	{
		SPlayer->Player.NoneDataPlayStartFlag = FALSE ;
		SPlayer->Player.NoneDataPlayCheckBackPlayOffset = 0 ;
		SPlayer->Player.NoneDataSetCompOffset = -1 ;
	}

	// 残サンプル数を減らす
	SPlayer->Player.StockSampleNum -= WriteSize ;

	// ロック情報をセット
	LockData.StartOffst = ( DWORD )SPlayer->Player.DataSetCompOffset ;
	LockData.Offset     = 0 ;
	LockData.Offset2    = 0 ;
	LockData.Valid      = LockData.Length ;
	LockData.Valid2     = LockData.Length2 ;

	// 有効データを転送
	if( LockData.Valid != 0 && WriteSize != 0 )
	{
		MoveTempSize = ( DWORD )( WriteSize * SPlayer->BufferFormat.nBlockAlign ) ;
		if( MoveTempSize > LockData.Valid )
			MoveTempSize = LockData.Valid ;

		RingBufDataGet( &SPlayer->Player.StockSample, LockData.WriteP + LockData.Offset, ( int )MoveTempSize, FALSE ) ;
		LockData.Offset += MoveTempSize ;
		LockData.Valid -= MoveTempSize ;
		WriteSize -= MoveTempSize / SPlayer->BufferFormat.nBlockAlign ;
	}
	if( LockData.Valid2 != 0 && WriteSize != 0 )
	{
		MoveTempSize = ( DWORD )( WriteSize * SPlayer->BufferFormat.nBlockAlign ) ;
		if( MoveTempSize > LockData.Valid2 )
			MoveTempSize = LockData.Valid2 ;

		RingBufDataGet( &SPlayer->Player.StockSample, LockData.WriteP2 + LockData.Offset2, ( int )MoveTempSize, FALSE ) ;
		LockData.Offset2 += MoveTempSize ;
		LockData.Valid2 -= MoveTempSize ;
		WriteSize -= MoveTempSize / SPlayer->BufferFormat.nBlockAlign ;
	}

	// 無音データを転送
	if( LockData.Valid != 0 && NoneWriteSize != 0 )
	{
		MoveTempSize = ( DWORD )( NoneWriteSize * SPlayer->BufferFormat.nBlockAlign ) ;
		if( MoveTempSize > LockData.Valid )
			MoveTempSize = LockData.Valid ;

		switch( SPlayer->BufferFormat.wBitsPerSample )
		{
		case 8  : _MEMSET( LockData.WriteP + LockData.Offset, 127, MoveTempSize ) ; break ;
		case 16 : _MEMSET( LockData.WriteP + LockData.Offset,   0, MoveTempSize ) ; break ;
		}

		LockData.Offset += MoveTempSize ;
		LockData.Valid -= MoveTempSize ;
		NoneWriteSize -= MoveTempSize / SPlayer->BufferFormat.nBlockAlign ;
	}
	if( LockData.Valid2 != 0 && NoneWriteSize != 0 )
	{
		MoveTempSize = ( DWORD )( NoneWriteSize * SPlayer->BufferFormat.nBlockAlign ) ;
		if( MoveTempSize > LockData.Valid2 )
			MoveTempSize = LockData.Valid2 ;

		switch( SPlayer->BufferFormat.wBitsPerSample )
		{
		case 8  : _MEMSET( LockData.WriteP2 + LockData.Offset2, 127, MoveTempSize ) ; break ;
		case 16 : _MEMSET( LockData.WriteP2 + LockData.Offset2,   0, MoveTempSize ) ; break ;
		}

		LockData.Offset2 += MoveTempSize ;
		LockData.Valid2 -= MoveTempSize ;
		NoneWriteSize -= MoveTempSize / SPlayer->BufferFormat.nBlockAlign ;
	}

	// サウンドバッファのロックを解除する
	SoundBuffer_Unlock(
		&SPlayer->Player.SoundBuffer,
		LockData.WriteP,  LockData.Length,
		LockData.WriteP2, LockData.Length2 ) ;

	// データセットが完了した位置を変更する
	SPlayer->Player.DataSetCompOffset += MoveSize ;
	if( SPlayer->Player.DataSetCompOffset >= SPlayer->Player.SoundBufferSize )
		SPlayer->Player.DataSetCompOffset -= SPlayer->Player.SoundBufferSize ;

	Result = 0 ;
END :

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

	// 終了
	return Result ;
}





































// ＭＩＤＩ制御関数

// ＭＩＤＩハンドルを初期化をする関数
extern int InitializeMidiHandle( HANDLEINFO *HandleInfo )
{
	MIDIHANDLEDATA *MusicData = ( MIDIHANDLEDATA * )HandleInfo ;

	// 音量をセット
	MusicData->Volume = 255 ;

	// 終了
	return 0 ;
}

// ＭＩＤＩハンドルの後始末を行う関数
extern int TerminateMidiHandle( HANDLEINFO *HandleInfo )
{
	MIDIHANDLEDATA *MusicData = ( MIDIHANDLEDATA * )HandleInfo ;

	// 音の再生を止める
	if( NS_CheckMusicMem( HandleInfo->Handle ) == TRUE )
		NS_StopMusicMem( HandleInfo->Handle ) ;

	// 環境依存処理
	if( TerminateMidiHandle_PF( MusicData ) < 0 )
	{
		return -1 ;
	}

	// ＭＩＤＩデータの解放
	if( MusicData->DataImage != NULL )
	{
		_MEMSET( MusicData->DataImage, 0, ( size_t )MusicData->DataSize );
		DXFREE( MusicData->DataImage ) ;
		MusicData->DataImage = NULL ;
	}

	// 終了
	return 0 ;
}

// 新しいＭＩＤＩハンドルを取得する
extern int NS_AddMusicData( void )
{
	int NewHandle ;

	NewHandle = AddHandle( DX_HANDLETYPE_MUSIC, FALSE, -1 ) ;

	return NewHandle ;
}

// ＭＩＤＩハンドルを削除する
extern int NS_DeleteMusicMem( int MusicHandle )
{
	return SubHandle( MusicHandle ) ;
}

// LoadMusicMemByMemImage の実処理関数
extern int LoadMusicMemByMemImage_Static(
	int MusicHandle,
	const void *FileImage,
	int FileImageSize,
	int ASyncThread
)
{
	MIDIHANDLEDATA *MusicData ;

	if( ASyncThread )
	{
		if( MIDI_MASKHCHK_ASYNC( MusicHandle, MusicData ) )
			return -1 ;
	}
	else
	{
		if( MIDI_MASKHCHK( MusicHandle, MusicData ) )
			return -1 ;
	}

	// イメージのコピーを作成
	MusicData->DataImage = DXALLOC( ( size_t )FileImageSize ) ;
	if( MusicData->DataImage == NULL )
	{
		DXST_LOGFILE_ADDUTF16LE( "\xdf\x30\xe5\x30\xfc\x30\xb8\x30\xc3\x30\xaf\x30\xc7\x30\xfc\x30\xbf\x30\x92\x30\x00\x4e\x42\x66\x84\x76\x6b\x30\xdd\x4f\x58\x5b\x57\x30\x66\x30\x4a\x30\x4f\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ミュージックデータを一時的に保存しておくメモリ領域の確保に失敗しました\n" @*/ ) ;
		return -1 ;
	}
	_MEMCPY( MusicData->DataImage, FileImage, ( size_t )FileImageSize ) ;
	MusicData->DataSize = FileImageSize ;

	// 環境依存処理
	if( LoadMusicMemByMemImage_Static_PF( MusicData, ASyncThread ) < 0 )
	{
		return -1 ;
	}

	// 再生中フラグを倒す
	MusicData->PlayFlag = FALSE ;

	// 終了
	return 0 ;
}

#ifndef DX_NON_ASYNCLOAD
// LoadMusicMemByMemImage の非同期読み込みスレッドから呼ばれる関数
static void LoadMusicMemByMemImage_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	int MusicHandle ;
	const void *FileImage ;
	int FileImageSize ;
	int Addr ;
	int Result ;

	Addr = 0 ;
	MusicHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	FileImage = GetASyncLoadParamVoidP( AParam->Data, &Addr ) ;
	FileImageSize = GetASyncLoadParamInt( AParam->Data, &Addr ) ;

	Result = LoadMusicMemByMemImage_Static( MusicHandle, FileImage, FileImageSize, TRUE ) ;

	DecASyncLoadCount( MusicHandle ) ;
	if( Result < 0 )
	{
		SubHandle( MusicHandle ) ;
	}
}
#endif // DX_NON_ASYNCLOAD

// LoadMusicMemByMemImage のグローバル変数にアクセスしないバージョン
extern int LoadMusicMemByMemImage_UseGParam(
	const void *FileImage,
	int FileImageSize,
	int ASyncLoadFlag
)
{
	int MusicHandle ;

	if( SoundSysData.InitializeFlag == FALSE )
		return -1 ;

	// ソフトが非アクティブの場合はアクティブになるまで待つ
	CheckActiveState() ;

	// ハンドルの作成
	MusicHandle = AddHandle( DX_HANDLETYPE_MUSIC, FALSE, -1 ) ;
	if( MusicHandle == -1 ) return -1 ;

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;

		// パラメータに必要なメモリのサイズを算出
		Addr = 0 ;
		AddASyncLoadParamInt( NULL, &Addr, MusicHandle ) ;
		AddASyncLoadParamConstVoidP( NULL, &Addr, FileImage ) ;
		AddASyncLoadParamInt( NULL, &Addr, FileImageSize ) ;

		// メモリの確保
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// 処理に必要な情報をセット
		AParam->ProcessFunction = LoadMusicMemByMemImage_ASync ;
		Addr = 0 ;
		AddASyncLoadParamInt( AParam->Data, &Addr, MusicHandle ) ;
		AddASyncLoadParamConstVoidP( AParam->Data, &Addr, FileImage ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, FileImageSize ) ;

		// データを追加
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto ERR ;
		}

		// 非同期読み込みカウントをインクリメント
		IncASyncLoadCount( MusicHandle, AParam->Index ) ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		if( LoadMusicMemByMemImage_Static( MusicHandle, FileImage, FileImageSize, FALSE ) < 0 )
			goto ERR ;
	}

	// 終了
	return MusicHandle ;

ERR :
	SubHandle( MusicHandle ) ;

	return -1 ;
}

// メモリ上に展開されたＭＩＤＩファイルを読み込む
extern int NS_LoadMusicMemByMemImage( const void *FileImageBuffer, int FileImageSize )
{
	return LoadMusicMemByMemImage_UseGParam( FileImageBuffer, FileImageSize, GetASyncLoadFlag() ) ;
}

// LoadMusicMem の実処理関数
static int LoadMusicMem_Static(
	int MusicHandle,
	const wchar_t *FileName,
	int ASyncThread
)
{
	MIDIHANDLEDATA * MusicData ;
	DWORD_PTR fp = 0 ;
	void *Buffer = NULL ;
	size_t FileSize ;
	int Ret ;

	if( ASyncThread )
	{
		if( MIDI_MASKHCHK_ASYNC( MusicHandle, MusicData ) )
			return -1 ;
	}
	else
	{
		if( MIDI_MASKHCHK( MusicHandle, MusicData ) )
			return -1 ;
	}

	// ファイルの読み込み
	{
		fp = DX_FOPEN( FileName ) ;
		if( fp == 0 )
		{
			DXST_LOGFILEFMT_ADDW(( L"Music File Open Error : %s", FileName )) ;
			goto ERR ;
		}
		DX_FSEEK( fp, 0L, SEEK_END ) ;
		FileSize = ( size_t )DX_FTELL( fp ) ;
		DX_FSEEK( fp, 0L, SEEK_SET ) ;
		Buffer = DXALLOC( FileSize ) ;
		if( Buffer == NULL )
		{
			DXST_LOGFILEFMT_ADDW(( L"Music File Memory Alloc Error : %s", FileName )) ;
			goto ERR ;
		}
		DX_FREAD( Buffer, FileSize, 1, fp ) ;
		DX_FCLOSE( fp ) ;
		fp = 0 ;
	}

	// ハンドルのセットアップ
	Ret = LoadMusicMemByMemImage_Static( MusicHandle, Buffer, ( int )FileSize, ASyncThread ) ;

	// メモリの解放
	DXFREE( Buffer ) ;

	// 結果を返す
	return Ret ;

ERR :
	if( fp != 0 ) DX_FCLOSE( fp ) ;
	if( Buffer != NULL ) DXFREE( Buffer ) ;

	return -1 ;
}

#ifndef DX_NON_ASYNCLOAD
// LoadMusicMem の非同期読み込みスレッドから呼ばれる関数
static void LoadMusicMem_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	int MusicHandle ;
	const wchar_t *FileName ;
	int Addr ;
	int Result ;

	Addr = 0 ;
	MusicHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	FileName = GetASyncLoadParamString( AParam->Data, &Addr ) ;

	Result = LoadMusicMem_Static( MusicHandle, FileName, TRUE ) ;

	DecASyncLoadCount( MusicHandle ) ;
	if( Result < 0 )
	{
		SubHandle( MusicHandle ) ;
	}
}
#endif // DX_NON_ASYNCLOAD

// LoadMusicMem のグローバル変数にアクセスしないバージョン
extern int LoadMusicMem_UseGParam(
	const wchar_t *FileName,
	int ASyncLoadFlag
)
{
	int MusicHandle ;

	if( SoundSysData.InitializeFlag == FALSE )
		return -1 ;

	// ソフトが非アクティブの場合はアクティブになるまで待つ
	CheckActiveState() ;

	// ハンドルの作成
	MusicHandle = AddHandle( DX_HANDLETYPE_MUSIC, FALSE, -1 ) ;
	if( MusicHandle == -1 ) return -1 ;

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;
		wchar_t FullPath[ 1024 ] ;

		ConvertFullPathW_( FileName, FullPath, sizeof( FullPath ) ) ;

		// パラメータに必要なメモリのサイズを算出
		Addr = 0 ;
		AddASyncLoadParamInt( NULL, &Addr, MusicHandle ) ;
		AddASyncLoadParamString( NULL, &Addr, FullPath ) ;

		// メモリの確保
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// 処理に必要な情報をセット
		AParam->ProcessFunction = LoadMusicMem_ASync ;
		Addr = 0 ;
		AddASyncLoadParamInt( AParam->Data, &Addr, MusicHandle ) ;
		AddASyncLoadParamString( AParam->Data, &Addr, FullPath ) ;

		// データを追加
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto ERR ;
		}

		// 非同期読み込みカウントをインクリメント
		IncASyncLoadCount( MusicHandle, AParam->Index ) ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		if( LoadMusicMem_Static( MusicHandle, FileName, FALSE ) < 0 )
			goto ERR ;
	}

	// 終了
	return MusicHandle ;

ERR :
	SubHandle( MusicHandle ) ;

	return -1 ;
}

// ＭＩＤＩファイルを読み込む
extern int NS_LoadMusicMem( const TCHAR *FileName )
{
#ifdef UNICODE
	return LoadMusicMem_WCHAR_T(
		FileName
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, return -1 )

	Result = LoadMusicMem_WCHAR_T(
		UseFileNameBuffer
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( FileName )

	return Result ;
#endif
}

// ＭＩＤＩファイルを読み込みＭＩＤＩハンドルを作成する
extern int NS_LoadMusicMemWithStrLen( const TCHAR *FileName, size_t FileNameLength )
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = LoadMusicMem_WCHAR_T( UseFileNameBuffer ) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = LoadMusicMem_WCHAR_T( UseFileNameBuffer ) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#endif
	return Result ;
}

// ＭＩＤＩファイルを読み込む
extern int LoadMusicMem_WCHAR_T( const wchar_t *FileName )
{
	return LoadMusicMem_UseGParam( FileName, GetASyncLoadFlag() ) ;
}

// 読み込んだＭＩＤＩデータの演奏を開始する
extern int NS_PlayMusicMem( int MusicHandle, int PlayType )
{
	MIDIHANDLEDATA * MusicData ;
	int IsDefaultHandle ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// デフォルトハンドルかどうかをチェック
	IsDefaultHandle = ( MusicHandle != 0 && MusicHandle == MidiSystemData.DefaultHandle ) ? TRUE : FALSE ;

	// ソフトが非アクティブの場合はアクティブになるまで待つ
	CheckActiveState() ;

	// エラー判定
	if( MIDI_MASKHCHK( MusicHandle, MusicData ) )
		return -1 ;

	// 演奏を停止する
	NS_StopMusicMem( MidiSystemData.PlayHandle ) ;

	// 環境依存処理
	if( PlayMusicMem_PF( MusicData, PlayType ) < 0 )
	{
		return -1 ;
	}

	MidiSystemData.PlayFlag		= TRUE ;								// 演奏フラグを立てる
	MidiSystemData.PlayHandle	= MusicHandle ;							// 演奏しているハンドルの更新
	MidiSystemData.LoopFlag		= PlayType == DX_PLAYTYPE_LOOP ;		// ループフラグをセットする

	if( MIDI_MASKHCHK( MusicHandle, MusicData ) )
	{
		MusicData = NULL ;
	}
	else
	{
		MusicData->PlayFlag			= TRUE ;								// 状態を再生中にする
		MusicData->PlayStartFlag	= FALSE ;								// 演奏が開始されたかフラグを倒す
	}

	// 演奏終了まで待つ指定の場合はここで待つ
	if( PlayType == DX_PLAYTYPE_NORMAL )
	{
		// 再生終了まで待つ
		while( NS_ProcessMessage() == 0 )
		{
			if( IsDefaultHandle )
			{
				if( NS_CheckMusic() == FALSE )
				{
					break ;
				}
			}
			else
			{
				if( NS_CheckMusicMem( MusicHandle ) == FALSE )
				{
					break ;
				}
			}
		}

		// 停止処理を行う
		if( IsDefaultHandle )
		{
			NS_StopMusic() ;
		}
		else
		{
			NS_StopMusicMem( MusicHandle ) ;
		}

		// テンポラリファイルから再生されていた場合は削除する
//		if( MidiSystemData.MemImagePlayFlag == TRUE )
//		{
//			DeleteFileW( MidiSystemData.FileName ) ;
//		}
//		MidiSystemData.MemImagePlayFlag = FALSE ;
	}

	// 終了
	return 0 ;
}

// ＭＩＤＩデータの演奏を停止する
extern int NS_StopMusicMem( int MusicHandle )
{
	MIDIHANDLEDATA * MusicData ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( MIDI_MASKHCHK( MusicHandle, MusicData ) )
		return -1 ;

	// サウンドハンドルの再生の場合はサウンドを止める
	if( MidiSystemData.DefaultHandle != 0 && MidiSystemData.DefaultHandleToSoundHandleFlag == TRUE )
	{
		NS_StopSoundMem( MidiSystemData.DefaultHandle ) ;
		NS_DeleteSoundMem( MidiSystemData.DefaultHandle ) ;

		MidiSystemData.DefaultHandle = 0 ;
		return 0 ;
	}

	// ループフラグを倒す
	MidiSystemData.LoopFlag = FALSE ;

	// 環境依存処理
	if( StopMusicMem_PF( MusicData ) < 0 )
	{
		return -1 ;
	}

	// 状態を停止中にする
	MusicData->PlayFlag = FALSE ;

	// 演奏が開始されたかフラグを倒す
	MusicData->PlayStartFlag = FALSE ;

	// 終了
	return 0 ;
}

// ＭＩＤＩデータが演奏中かどうかを取得する( TRUE:演奏中  FALSE:停止中 )
extern int NS_CheckMusicMem( int MusicHandle )
{
	MIDIHANDLEDATA * MusicData ;
	int Result = -1 ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( MIDI_MASKHCHK( MusicHandle, MusicData ) )
		return -1 ;

	// サウンドハンドルの再生の場合はサウンドの再生状態を返す
	if( MidiSystemData.DefaultHandle != 0 && MidiSystemData.DefaultHandleToSoundHandleFlag == TRUE )
	{
		return NS_CheckSoundMem( MidiSystemData.DefaultHandle ) ;
	}

	// 環境依存処理
	Result = CheckMusicMem_PF( MusicData ) ;

	return Result ;
}

// ＭＩＤＩデータの再生音量をセットする
extern int NS_SetVolumeMusicMem( int Volume, int MusicHandle )
{
	MIDIHANDLEDATA * MusicData ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( MIDI_MASKHCHK( MusicHandle, MusicData ) )
		return -1 ;

	// 音量をセット
	MusicData->Volume = Volume ;

	// サウンドハンドルの再生の場合はサウンドの音量を変更する
	if( MidiSystemData.DefaultHandle != 0 && MidiSystemData.DefaultHandleToSoundHandleFlag == TRUE )
	{
		return NS_ChangeVolumeSoundMem( Volume, MidiSystemData.DefaultHandle );
	}

	return NS_SetVolumeMusic( Volume )  ;
}

// ＭＩＤＩデータハンドルをすべて削除する
extern int NS_InitMusicMem( void )
{
	return AllHandleSub( DX_HANDLETYPE_MUSIC ) ;
}

// ＭＩＤＩデータの周期的処理
extern int NS_ProcessMusicMem( void )
{
	MIDIHANDLEDATA * MusicData ;
	int i ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
		return -1 ;

	if( HandleManageArray[ DX_HANDLETYPE_MUSIC ].InitializeFlag == FALSE )
		return -1 ;

	for( i = HandleManageArray[ DX_HANDLETYPE_MUSIC ].AreaMin ; i <= HandleManageArray[ DX_HANDLETYPE_MUSIC ].AreaMax ; i ++ )
	{
		MusicData = ( MIDIHANDLEDATA * )HandleManageArray[ DX_HANDLETYPE_MUSIC ].Handle[ i ] ;
		if( MusicData == NULL ) continue ;

		if( MusicData->PlayFlag == FALSE ) continue ;

		if( ProcessMusicMem_PF( MusicData ) < 0 )
		{
			return -1 ;
		}
	}

	// 終了
	return 0;
}

// ＭＩＤＩデータの現在の再生位置を取得する
extern int NS_GetMusicMemPosition( int MusicHandle )
{
	MIDIHANDLEDATA * MusicData ;
	int Result = -1 ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( MIDI_MASKHCHK( MusicHandle, MusicData ) )
		return -1 ;

	// 環境依存処理
	Result = GetMusicMemPosition_PF( MusicData ) ;

	return Result ;
}



// ＭＩＤＩファイルを再生する
extern int NS_PlayMusic( const TCHAR *FileName , int PlayType )
{
#ifdef UNICODE
	return PlayMusic_WCHAR_T(
		FileName, PlayType
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, return -1 )

	Result = PlayMusic_WCHAR_T(
		UseFileNameBuffer, PlayType
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( FileName )

	return Result ;
#endif
}

// ＭＩＤＩファイルを演奏する
extern int NS_PlayMusicWithStrLen( const TCHAR *FileName, size_t FileNameLength, int PlayType )
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = PlayMusic_WCHAR_T( UseFileNameBuffer, PlayType ) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = PlayMusic_WCHAR_T( UseFileNameBuffer, PlayType ) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#endif
	return Result ;
}


// ＭＩＤＩファイルを再生する
extern int PlayMusic_WCHAR_T( const wchar_t *FileName , int PlayType )
{
	// もし演奏中だったら止める
	if( MidiSystemData.DefaultHandle != 0 )
	{
		if( MidiSystemData.DefaultHandleToSoundHandleFlag == TRUE )
		{
			NS_DeleteSoundMem( MidiSystemData.DefaultHandle ) ;
		}
		else
		{
			NS_DeleteMusicMem( MidiSystemData.DefaultHandle ) ;
		}
		MidiSystemData.DefaultHandle = 0 ;
	}

	// 読み込み
	MidiSystemData.DefaultHandle = LoadMusicMem_UseGParam( FileName, FALSE ) ;
	if( MidiSystemData.DefaultHandle == -1 )
	{
		MidiSystemData.DefaultHandle = 0;
		return -1 ;
	}
	MidiSystemData.DefaultHandleToSoundHandleFlag = FALSE ;

	// 演奏
	if( NS_PlayMusicMem( MidiSystemData.DefaultHandle, PlayType ) == -1 )
	{
		LOADSOUND_GPARAM GParam ;

		// 演奏に失敗したら普通のサウンドファイルの可能性がある
		NS_DeleteMusicMem( MidiSystemData.DefaultHandle ) ;

		InitLoadSoundGParam( &GParam ) ;
		GParam.CreateSoundDataType = DX_SOUNDDATATYPE_MEMPRESS ;
		MidiSystemData.DefaultHandle = LoadSoundMemBase_UseGParam( &GParam, FileName, 1, -1, FALSE, FALSE ) ;
		if( MidiSystemData.DefaultHandle == -1 )
		{
			// それでも失敗したらファイルがないということ
			MidiSystemData.DefaultHandle = 0;
			return -1 ;
		}

		// 再生開始
		NS_PlaySoundMem( MidiSystemData.DefaultHandle, PlayType ) ;
		MidiSystemData.DefaultHandleToSoundHandleFlag = TRUE ;
	}

	return 0;
}





// メモリ上に展開されているＭＩＤＩファイルを演奏する
extern int NS_PlayMusicByMemImage( const void *FileImageBuffer, int FileImageSize, int PlayType )
{
	// もし演奏中だったら止める
	if( MidiSystemData.DefaultHandle != 0 )
	{
		if( MidiSystemData.DefaultHandleToSoundHandleFlag == TRUE )
		{
			NS_DeleteSoundMem( MidiSystemData.DefaultHandle ) ;
		}
		else
		{
			NS_DeleteMusicMem( MidiSystemData.DefaultHandle ) ;
		}
		MidiSystemData.DefaultHandle = 0 ;
	}

	// 読み込み
	MidiSystemData.DefaultHandle = LoadMusicMemByMemImage_UseGParam( FileImageBuffer, FileImageSize, FALSE ) ;
	if( MidiSystemData.DefaultHandle == -1 )
	{
		LOADSOUND_GPARAM GParam ;

		// 読み込みに失敗したら音声として再生する
		InitLoadSoundGParam( &GParam ) ;
		MidiSystemData.DefaultHandle = LoadSoundMemByMemImageBase_UseGParam( &GParam, TRUE, -1, FileImageBuffer, FileImageSize, 1, -1, FALSE, FALSE ) ;
		if( MidiSystemData.DefaultHandle == -1 )
		{
			// それでも失敗したらデータが壊れているということ
			MidiSystemData.DefaultHandle = 0;
			return -1 ;
		}
		MidiSystemData.DefaultHandleToSoundHandleFlag = TRUE ;
	}
	else
	{
		MidiSystemData.DefaultHandleToSoundHandleFlag = FALSE ;
	}

	// 演奏
	NS_PlayMusicMem( MidiSystemData.DefaultHandle, PlayType ) ;

	return 0;
}

// ＭＩＤＩファイルの演奏停止
extern int NS_StopMusic( void )
{
	if( MidiSystemData.DefaultHandle == 0 ) return 0 ;

	if( MidiSystemData.DefaultHandleToSoundHandleFlag == TRUE )
	{
		NS_StopSoundMem( MidiSystemData.DefaultHandle ) ;
		NS_DeleteSoundMem( MidiSystemData.DefaultHandle ) ;
	}
	else
	{
		NS_StopMusicMem( MidiSystemData.DefaultHandle ) ;
		NS_DeleteMusicMem( MidiSystemData.DefaultHandle ) ;
	}

	MidiSystemData.DefaultHandle = 0 ;

	return 0 ;
}



// ＭＩＤＩファイルが演奏中か否か情報を取得する
extern int NS_CheckMusic( void )
{
	if( MidiSystemData.DefaultHandle == 0 ) return 0 ;

	if( MidiSystemData.DefaultHandleToSoundHandleFlag == TRUE )
	{
		return NS_CheckSoundMem( MidiSystemData.DefaultHandle ) ;
	}
	else
	{
		return NS_CheckMusicMem( MidiSystemData.DefaultHandle ) ;
	}
}


// ＭＩＤＩの再生形式をセットする
extern int NS_SelectMidiMode( int Mode )
{
	// 値の有効性チェック
	if( Mode < 0 || Mode >= DX_MIDIMODE_NUM )
	{
		return -1 ;
	}

	// もしモードが今までと同じ場合はなにもせず終了
	if( Mode == SoundSysData.SoundMode ) return 0 ;

	// 再生モードをセット
	SoundSysData.SoundMode = Mode ;

	if( CheckSoundSystem_Initialize_PF() == FALSE ) return 0 ;

	// サウンドシステムの後始末
	TerminateSoundSystem() ;

	// サウンドシステム初期化
	return InitializeSoundSystem() ;
}

// ＭＩＤＩの再生音量をセットする
extern int NS_SetVolumeMusic( int Volume )
{
	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	if( MidiSystemData.DefaultHandleToSoundHandleFlag == FALSE )
	{
		// 環境依存処理
		SetVolumeMusic_PF( Volume ) ;
	}
	else
	{
		NS_ChangeVolumeSoundMem( Volume, MidiSystemData.DefaultHandle );
	}

	// 終了
	return 0 ;
}

// ＭＩＤＩの現在の再生位置を取得する
extern int NS_GetMusicPosition( void )
{
	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// 再生中ではなかったら何もしない
	if( NS_CheckMusic() == FALSE ) return -1 ;

	// 環境依存処理
	return GetMusicPosition_PF() ;
}
	





// 補助系

// 音声フォーマットと波形イメージからＷＡＶＥファイルイメージを作成する
extern	int CreateWaveFileImage( 	void **DestBufferP, int *DestBufferSizeP,
									const WAVEFORMATEX *Format, int FormatSize,
									const void *WaveData, int WaveDataSize )
{
	int BufferSize ;
	void *Buffer ;
	BYTE *p ;
	
	// バッファを確保
	BufferSize = FormatSize + WaveDataSize
					+ 12/*"RIFF" + ファイルサイズ + "WAVE"*/
					+ 8 * 2/*"fmt "チャンク + "data"チャンク*/ ;
	Buffer = DXALLOC( ( size_t )BufferSize ) ;
	if( Buffer == NULL )
	{
		DXST_LOGFILE_ADDUTF16LE( "\xee\x4e\x57\x00\x41\x00\x56\x00\x45\x00\xd5\x30\xa1\x30\xa4\x30\xeb\x30\xa4\x30\xe1\x30\xfc\x30\xb8\x30\x28\x75\x6e\x30\xe1\x30\xe2\x30\xea\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"仮WAVEファイルイメージ用のメモリ確保に失敗しました\n" @*/ ) ;
		return -1 ;
	}
	
	// データのセット
	p = (BYTE *)Buffer ;
	_MEMCPY( (char *)p, "RIFF", 4 ) ;					p += 4 ;
	*((DWORD *)p) = ( DWORD )( BufferSize - 8 ) ;		p += 4 ;
	_MEMCPY( (char *)p, "WAVE", 4 ) ;					p += 4 ;

	_MEMCPY( (char *)p, "fmt ", 4 ) ;					p += 4 ;
	*((DWORD *)p) = ( DWORD )FormatSize ;				p += 4 ;
	_MEMCPY( p, Format, ( size_t )FormatSize ) ;		p += FormatSize ;

	_MEMCPY( (char *)p, "data", 4 ) ;					p += 4 ;
	*((DWORD *)p) = ( DWORD )WaveDataSize ;				p += 4 ;
	_MEMCPY( p, WaveData, ( size_t )WaveDataSize ) ;	p += WaveDataSize ;

	// 情報を保存
	*DestBufferP = Buffer ;
	*DestBufferSizeP = BufferSize ;

	// 終了
	return 0 ;
}

// ファイルを丸まるメモリに読み込む
static	int FileFullRead( const wchar_t *FileName, void **BufferP, int *SizeP, int OutputFileOpenErrorLog )
{
	DWORD_PTR fp = 0 ;
	size_t Size ;
	void *Buffer ;
	
	fp = DX_FOPEN( FileName ) ;
	if( fp == 0 )
	{
		if( OutputFileOpenErrorLog )
		{
			DXST_LOGFILEFMT_ADDW(( L"File Open Error : %s", FileName )) ;
		}
		goto ERR ;
	}

	DX_FSEEK( fp, 0, SEEK_END ) ;
	Size = ( size_t )DX_FTELL( fp ) ;
	DX_FSEEK( fp, 0, SEEK_SET ) ;
	
	Buffer = DXALLOC( Size ) ;
	if( Buffer == NULL )
	{
		DXST_LOGFILEFMT_ADDW(( L"File Load : Memory Alloc Error : %s", FileName )) ;
		goto ERR ;
	}
	
	DX_FREAD( Buffer, Size, 1, fp ) ;
	DX_FCLOSE( fp ) ;
	
	*BufferP = Buffer ;
	*SizeP = ( int )Size ;
	
	// 終了
	return 0 ;
	
ERR :
	if( fp != 0 ) DX_FCLOSE( fp ) ;
	
	return -1 ;
}

// ストリームデータを丸まるメモリに読み込む
extern	int StreamFullRead( STREAMDATA *Stream, void **BufferP, int *SizeP )
{
	size_t Size ;
	void *Buffer ;
	
	Stream->ReadShred.Seek( Stream->DataPoint, 0, SEEK_END ) ;
	Size = ( size_t )Stream->ReadShred.Tell( Stream->DataPoint ) ;
	Stream->ReadShred.Seek( Stream->DataPoint, 0, SEEK_SET ) ;
	
	Buffer = DXALLOC( Size ) ;
	if( Buffer == NULL )
	{
		return DXST_LOGFILEFMT_ADDUTF16LE(( "\xb9\x30\xc8\x30\xea\x30\xfc\x30\xe0\x30\x6e\x30\xad\x8a\x7f\x30\xbc\x8f\x7f\x30\x6b\x30\xc5\x5f\x81\x89\x6a\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"ストリームの読み込みに必要なメモリの確保に失敗しました" @*/ )) ;
	}

	Stream->ReadShred.Read( Buffer, Size, 1, Stream->DataPoint ) ;
	
	*BufferP = Buffer ;
	*SizeP = ( int )Size ;
	
	// 終了
	return 0 ;
}

// ソフトウエアで扱う波形データからサウンドハンドルを作成する
extern int NS_LoadSoundMemFromSoftSound( int SoftSoundHandle, int BufferNum )
{
	SOFTSOUND * SSound ;
	int NewHandle ;
	void *WaveImage ;
	int WaveSize ;
	LOADSOUND_GPARAM GParam ;

	// エラー判定
	if( SSND_MASKHCHK( SoftSoundHandle, SSound ) ) return -1 ;
	if( SSound->IsPlayer == TRUE ) return -1 ;

	// ＷＡＶＥファイルをでっち上げる
	if( CreateWaveFileImage( &WaveImage, &WaveSize,
						 &SSound->BufferFormat, sizeof( WAVEFORMATEX ),
						 SSound->Wave.Buffer, SSound->Wave.BufferSampleNum * SSound->BufferFormat.nBlockAlign ) < 0 )
		 return -1 ;

	// ハンドルの作成
	InitLoadSoundGParam( &GParam ) ;
	if( GParam.CreateSoundDataType == DX_SOUNDDATATYPE_FILE )
		GParam.CreateSoundDataType = DX_SOUNDDATATYPE_MEMNOPRESS ;
	NewHandle = LoadSoundMemByMemImageBase_UseGParam( &GParam, TRUE, -1, WaveImage, WaveSize, BufferNum, -1, FALSE, FALSE ) ;

	// メモリの解放
	DXFREE( WaveImage ) ;
	WaveImage = NULL ;

	// 終了
	return NewHandle ;
}

// サウンドハンドルのタイプがストリームハンドルではない場合にストリームハンドルに変更する
static int SoundTypeChangeToStream( int SoundHandle )
{
	SOUND * sd ;
	void *WaveImage ;
	int WaveSize ;
	int Time, Volume, Frequency, Pan ;

	if( CheckSoundSystem_Initialize_PF() == FALSE )
	{
		return -1 ;
	}

	// エラー判定
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// 既にストリームハンドルだったら何もせずに終了
	if( sd->Type == DX_SOUNDTYPE_STREAMSTYLE )
		return 0 ;

	// ＷＡＶＥファイルをでっち上げる
	if( CreateWaveFileImage( &WaveImage, &WaveSize,
						 &sd->BufferFormat, sizeof( WAVEFORMATEX ),
						 sd->Normal.WaveData, sd->Normal.WaveSize ) < 0 )
		return -1 ;

	// 既存ハンドルの設定を取得しておく
	Volume = NS_GetVolumeSoundMem( SoundHandle ) ;
	Time = NS_GetSoundCurrentPosition( SoundHandle ) ;
	Frequency = NS_GetFrequencySoundMem( SoundHandle ) ;
	Pan = NS_GetPanSoundMem( SoundHandle ) ;

	// 既存ハンドルの削除
	{
		NS_StopSoundMem( SoundHandle ) ;					// 再生中だったときのことを考えて止めておく
		NS_DeleteSoundMem( SoundHandle ) ;					// ハンドルを削除
	}

	// ストリーム再生形式のハンドルとして作り直す
	{
		int f, res ;
		STREAMDATA Stream ;
		
		Stream.DataPoint = MemStreamOpen( WaveImage, ( size_t )WaveSize ) ;
		Stream.ReadShred = *GetMemStreamDataShredStruct() ;

		SoundHandle = AddHandle( DX_HANDLETYPE_SOUND, FALSE, SoundHandle ) ;
		res = NS_AddStreamSoundMem( &Stream, 0, SoundHandle, DX_SOUNDDATATYPE_MEMNOPRESS, &f ) ;
		if( res == -1 )
		{
			DXST_LOGFILE_ADDUTF16LE( "\xb9\x30\xc8\x30\xea\x30\xfc\x30\xe0\x30\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ストリームサウンドハンドルの作成に失敗しました\n" @*/ ) ;
			return -1 ;
		}

		Stream.ReadShred.Close( Stream.DataPoint ) ;
	}
	
	// メモリの解放
	DXFREE( WaveImage ) ;

	// 元のハンドルの設定をセットする
	NS_SetVolumeSoundMem( Volume, SoundHandle ) ;
	NS_SetSoundCurrentPosition( Time, SoundHandle ) ;
	NS_SetFrequencySoundMem( Frequency, SoundHandle ) ;
	NS_SetPanSoundMem( Pan, SoundHandle ) ;

	// 正常終了
	return 0 ;
}

// ミリ秒単位の数値をサンプル単位の数値に変換する
static	int MilliSecPositionToSamplePosition( int SamplesPerSec, int MilliSecTime )
{
	LONGLONG TempValue1, TempValue2 ;
	unsigned int Temp[ 4 ] ;

	TempValue1 = SamplesPerSec ;
	TempValue2 = MilliSecTime ;
	_MUL128_1( ( DWORD * )&TempValue1, ( DWORD * )&TempValue2, ( DWORD * )Temp ) ;
	TempValue2 = 1000 ;
	_DIV128_1( ( DWORD * )Temp, ( DWORD * )&TempValue2, ( DWORD * )&TempValue1 ) ;
	return ( int )TempValue1 ;
}

// サンプル単位の数値をミリ秒単位の数値に変換する
static	int SamplePositionToMilliSecPosition( int SamplesPerSec, int SampleTime )
{
	LONGLONG TempValue1, TempValue2 ;
	unsigned int Temp[ 4 ] ;

	TempValue1 = SampleTime ;
	TempValue2 = 1000 ;
	_MUL128_1( ( DWORD * )&TempValue1, ( DWORD * )&TempValue2, ( DWORD * )Temp ) ;
	TempValue2 = SamplesPerSec ;
	_DIV128_1( ( DWORD * )Temp, ( DWORD * )&TempValue2, ( DWORD * )&TempValue1 ) ;
	return ( int )TempValue1 ;
}

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_NON_SOUND







