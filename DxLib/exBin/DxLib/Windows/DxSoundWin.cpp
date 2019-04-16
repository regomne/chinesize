//-----------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		WindowsOS用サウンドプログラム
// 
//  	Ver 3.20c
// 
//-----------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

#include "../DxCompileConfig.h"

#ifndef DX_NON_SOUND

// インクルード----------------------------------------------------------------
#include "DxSoundWin.h"
#include "DxGuid.h"
#include "DxWinAPI.h"
#include "../DxFile.h"
#include "../DxSound.h"
#include "../DxSystem.h"
#include "../DxASyncLoad.h"
#include "../DxLog.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義------------------------------------------------------------------

// ＭＩＤＩ最小ボリューム
#define DM_MIN_VOLUME						(-10000)

// 型定義----------------------------------------------------------------------

// データ宣言------------------------------------------------------------------

static const D_X3DAUDIO_DISTANCE_CURVE_POINT D_X3DAudioDefault_LinearCurvePoints[2] = { 0.0f, 1.0f, 1.0f, 0.0f };
static const D_X3DAUDIO_DISTANCE_CURVE       D_X3DAudioDefault_LinearCurve          = { ( D_X3DAUDIO_DISTANCE_CURVE_POINT * )& D_X3DAudioDefault_LinearCurvePoints[ 0 ], 2 } ;

static const D_X3DAUDIO_CONE Listener_DirectionalCone = { DX_PI_F * 5.0f / 6.0f, DX_PI_F * 11.0f / 6.0f, 1.0f, 0.75f, 0.0f, 0.25f, 0.708f, 1.0f } ;

static const D_X3DAUDIO_DISTANCE_CURVE_POINT D_Emitter_LFE_CurvePoints[3]			= { 0.0f, 1.0f, 0.25f, 0.0f, 1.0f, 0.0f } ;
static const D_X3DAUDIO_DISTANCE_CURVE       D_Emitter_LFE_Curve					= { ( D_X3DAUDIO_DISTANCE_CURVE_POINT * )&D_Emitter_LFE_CurvePoints[ 0 ], 3 } ;

static const D_X3DAUDIO_DISTANCE_CURVE_POINT D_Emitter_Reverb_CurvePoints[3]		= { 0.0f, 0.5f, 0.75f, 1.0f, 1.0f, 0.0f } ;
static const D_X3DAUDIO_DISTANCE_CURVE       D_Emitter_Reverb_Curve					= { ( D_X3DAUDIO_DISTANCE_CURVE_POINT * )&D_Emitter_Reverb_CurvePoints[ 0 ], 3 } ;

static const D_XAUDIO2FX_REVERB_I3DL2_PARAMETERS D_PRESET_PARAMS[ D_XAUDIO2FX_PRESET_NUM ] =
{
	D_XAUDIO2FX_I3DL2_PRESET_FOREST,
	D_XAUDIO2FX_I3DL2_PRESET_DEFAULT,
	D_XAUDIO2FX_I3DL2_PRESET_GENERIC,
	D_XAUDIO2FX_I3DL2_PRESET_PADDEDCELL,
	D_XAUDIO2FX_I3DL2_PRESET_ROOM,
	D_XAUDIO2FX_I3DL2_PRESET_BATHROOM,
	D_XAUDIO2FX_I3DL2_PRESET_LIVINGROOM,
	D_XAUDIO2FX_I3DL2_PRESET_STONEROOM,
	D_XAUDIO2FX_I3DL2_PRESET_AUDITORIUM,
	D_XAUDIO2FX_I3DL2_PRESET_CONCERTHALL,
	D_XAUDIO2FX_I3DL2_PRESET_CAVE,
	D_XAUDIO2FX_I3DL2_PRESET_ARENA,
	D_XAUDIO2FX_I3DL2_PRESET_HANGAR,
	D_XAUDIO2FX_I3DL2_PRESET_CARPETEDHALLWAY,
	D_XAUDIO2FX_I3DL2_PRESET_HALLWAY,
	D_XAUDIO2FX_I3DL2_PRESET_STONECORRIDOR,
	D_XAUDIO2FX_I3DL2_PRESET_ALLEY,
	D_XAUDIO2FX_I3DL2_PRESET_CITY,
	D_XAUDIO2FX_I3DL2_PRESET_MOUNTAINS,
	D_XAUDIO2FX_I3DL2_PRESET_QUARRY,
	D_XAUDIO2FX_I3DL2_PRESET_PLAIN,
	D_XAUDIO2FX_I3DL2_PRESET_PARKINGLOT,
	D_XAUDIO2FX_I3DL2_PRESET_SEWERPIPE,
	D_XAUDIO2FX_I3DL2_PRESET_UNDERWATER,
	D_XAUDIO2FX_I3DL2_PRESET_SMALLROOM,
	D_XAUDIO2FX_I3DL2_PRESET_MEDIUMROOM,
	D_XAUDIO2FX_I3DL2_PRESET_LARGEROOM,
	D_XAUDIO2FX_I3DL2_PRESET_MEDIUMHALL,
	D_XAUDIO2FX_I3DL2_PRESET_LARGEHALL,
	D_XAUDIO2FX_I3DL2_PRESET_PLATE,
} ;

// 関数プロトタイプ宣言 -------------------------------------------------------


static	DWORD WINAPI StreamSoundThreadFunction( void * ) ;											// ストリームサウンド処理用スレッド

// XAudio2関係
static HRESULT D_XAudio2CreateReverb( D_IUnknown** ppApo, DWORD Flags = 0 ) ;
static HRESULT D_XAudio2CreateVolumeMeter( D_IUnknown** ppApo, DWORD Flags = 0 ) ;
static void D_ReverbConvertI3DL2ToNative( const D_XAUDIO2FX_REVERB_I3DL2_PARAMETERS* pI3DL2, D_XAUDIO2FX_REVERB_PARAMETERS* pNative ) ;
static void D_ReverbConvertI3DL2ToNative2_8( const D_XAUDIO2FX_REVERB_I3DL2_PARAMETERS* pI3DL2, D_XAUDIO2FX_REVERB_PARAMETERS2_8* pNative ) ;

// プログラム------------------------------------------------------------------

// XAudio2関係
static HRESULT D_XAudio2CreateVolumeMeter( D_IUnknown** ppApo, DWORD Flags )
{
	if( SoundSysData.PF.XAudio2_8DLL != NULL )
	{
		return SoundSysData.PF.CreateAudioVolumeMeterFunc( ppApo ) ;
	}
	else
	{
		SETUP_WIN_API
		return WinAPIData.Win32Func.CoCreateInstanceFunc((Flags & D_XAUDIO2FX_DEBUG) ? CLSID_AUDIOVOLUMEMeter2_7_DEBUG : CLSID_AUDIOVOLUMEMeter2_7, NULL, CLSCTX_INPROC_SERVER, IID_IUNKNOWN, (void**)ppApo ) ;
	}
}

static HRESULT D_XAudio2CreateReverb( D_IUnknown** ppApo, DWORD Flags )
{
	if( SoundSysData.PF.XAudio2_8DLL != NULL )
	{
		return SoundSysData.PF.CreateAudioReverbFunc( ppApo ) ;
	}
	else
	{
		SETUP_WIN_API
	    return WinAPIData.Win32Func.CoCreateInstanceFunc( ( Flags & D_XAUDIO2FX_DEBUG) ? CLSID_AUDIOREVERB2_7_DEBUG : CLSID_AUDIOREVERB2_7, NULL, CLSCTX_INPROC_SERVER, IID_IUNKNOWN, (void**)ppApo ) ;
	}
}

static void D_ReverbConvertI3DL2ToNative( const D_XAUDIO2FX_REVERB_I3DL2_PARAMETERS* pI3DL2, D_XAUDIO2FX_REVERB_PARAMETERS* pNative )
{
    float reflectionsDelay;
    float reverbDelay;

    // RoomRolloffFactor is ignored

    // These parameters have no equivalent in I3DL2
    pNative->RearDelay = D_XAUDIO2FX_REVERB_DEFAULT_REAR_DELAY; // 5
    pNative->PositionLeft = D_XAUDIO2FX_REVERB_DEFAULT_POSITION; // 6
    pNative->PositionRight = D_XAUDIO2FX_REVERB_DEFAULT_POSITION; // 6
    pNative->PositionMatrixLeft = D_XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX; // 27
    pNative->PositionMatrixRight = D_XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX; // 27
    pNative->RoomSize = D_XAUDIO2FX_REVERB_DEFAULT_ROOM_SIZE; // 100
    pNative->LowEQCutoff = 4;
    pNative->HighEQCutoff = 6;

    // The rest of the I3DL2 parameters map to the native property set
    pNative->RoomFilterMain = (float)pI3DL2->Room / 100.0f;
    pNative->RoomFilterHF = (float)pI3DL2->RoomHF / 100.0f;

    if (pI3DL2->DecayHFRatio >= 1.0f)
    {
        int index = _DTOL(-4.0 * _LOG10(pI3DL2->DecayHFRatio));
        if (index < -8) index = -8;
        pNative->LowEQGain = (BYTE)((index < 0) ? index + 8 : 8);
        pNative->HighEQGain = 8;
        pNative->DecayTime = pI3DL2->DecayTime * pI3DL2->DecayHFRatio;
    }
    else
    {
        int index = _DTOL(4.0 * _LOG10(pI3DL2->DecayHFRatio));
        if (index < -8) index = -8;
        pNative->LowEQGain = 8;
        pNative->HighEQGain = (BYTE)((index < 0) ? index + 8 : 8);
        pNative->DecayTime = pI3DL2->DecayTime;
    }

    reflectionsDelay = pI3DL2->ReflectionsDelay * 1000.0f;
    if (reflectionsDelay >= D_XAUDIO2FX_REVERB_MAX_REFLECTIONS_DELAY) // 300
    {
        reflectionsDelay = (float)(D_XAUDIO2FX_REVERB_MAX_REFLECTIONS_DELAY - 1);
    }
    else if (reflectionsDelay <= 1)
    {
        reflectionsDelay = 1;
    }
    pNative->ReflectionsDelay = (DWORD)reflectionsDelay;

    reverbDelay = pI3DL2->ReverbDelay * 1000.0f;
    if (reverbDelay >= D_XAUDIO2FX_REVERB_MAX_REVERB_DELAY) // 85
    {
        reverbDelay = (float)(D_XAUDIO2FX_REVERB_MAX_REVERB_DELAY - 1);
    }
    pNative->ReverbDelay = (BYTE)reverbDelay;

    pNative->ReflectionsGain = pI3DL2->Reflections / 100.0f;
    pNative->ReverbGain = pI3DL2->Reverb / 100.0f;
    pNative->EarlyDiffusion = (BYTE)(15.0f * pI3DL2->Diffusion / 100.0f);
    pNative->LateDiffusion = pNative->EarlyDiffusion;
    pNative->Density = pI3DL2->Density;
    pNative->RoomFilterFreq = pI3DL2->HFReference;

    pNative->WetDryMix = pI3DL2->WetDryMix;
}

static void D_ReverbConvertI3DL2ToNative2_8( const D_XAUDIO2FX_REVERB_I3DL2_PARAMETERS* pI3DL2, D_XAUDIO2FX_REVERB_PARAMETERS2_8* pNative )
{
    float reflectionsDelay;
    float reverbDelay;

    // RoomRolloffFactor is ignored

    // These parameters have no equivalent in I3DL2
    pNative->RearDelay = D_XAUDIO2FX_REVERB_DEFAULT_REAR_DELAY; // 5
    pNative->PositionLeft = D_XAUDIO2FX_REVERB_DEFAULT_POSITION; // 6
    pNative->PositionRight = D_XAUDIO2FX_REVERB_DEFAULT_POSITION; // 6
    pNative->PositionMatrixLeft = D_XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX; // 27
    pNative->PositionMatrixRight = D_XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX; // 27
    pNative->RoomSize = D_XAUDIO2FX_REVERB_DEFAULT_ROOM_SIZE; // 100
    pNative->LowEQCutoff = 4;
    pNative->HighEQCutoff = 6;

    // The rest of the I3DL2 parameters map to the native property set
    pNative->RoomFilterMain = (float)pI3DL2->Room / 100.0f;
    pNative->RoomFilterHF = (float)pI3DL2->RoomHF / 100.0f;

    if (pI3DL2->DecayHFRatio >= 1.0f)
    {
        int index = _DTOL(-4.0 * _LOG10(pI3DL2->DecayHFRatio));
        if (index < -8) index = -8;
        pNative->LowEQGain = (BYTE)((index < 0) ? index + 8 : 8);
        pNative->HighEQGain = 8;
        pNative->DecayTime = pI3DL2->DecayTime * pI3DL2->DecayHFRatio;
    }
    else
    {
        int index = _DTOL(4.0 * _LOG10(pI3DL2->DecayHFRatio));
        if (index < -8) index = -8;
        pNative->LowEQGain = 8;
        pNative->HighEQGain = (BYTE)((index < 0) ? index + 8 : 8);
        pNative->DecayTime = pI3DL2->DecayTime;
    }

    reflectionsDelay = pI3DL2->ReflectionsDelay * 1000.0f;
    if (reflectionsDelay >= D_XAUDIO2FX_REVERB_MAX_REFLECTIONS_DELAY) // 300
    {
        reflectionsDelay = (float)(D_XAUDIO2FX_REVERB_MAX_REFLECTIONS_DELAY - 1);
    }
    else if (reflectionsDelay <= 1)
    {
        reflectionsDelay = 1;
    }
    pNative->ReflectionsDelay = (DWORD)reflectionsDelay;

    reverbDelay = pI3DL2->ReverbDelay * 1000.0f;
    if (reverbDelay >= D_XAUDIO2FX_REVERB_MAX_REVERB_DELAY) // 85
    {
        reverbDelay = (float)(D_XAUDIO2FX_REVERB_MAX_REVERB_DELAY - 1);
    }
    pNative->ReverbDelay = (BYTE)reverbDelay;

    pNative->ReflectionsGain = pI3DL2->Reflections / 100.0f;
    pNative->ReverbGain = pI3DL2->Reverb / 100.0f;
    pNative->EarlyDiffusion = (BYTE)(15.0f * pI3DL2->Diffusion / 100.0f);
    pNative->LateDiffusion = pNative->EarlyDiffusion;
    pNative->Density = pI3DL2->Density;
    pNative->RoomFilterFreq = pI3DL2->HFReference;

    pNative->WetDryMix = pI3DL2->WetDryMix;
    pNative->DisableLateField = FALSE;
}












// デシベル値から XAudio2 の率値に変換する関数
__inline float D_XAudio2DecibelsToAmplitudeRatio( float Decibels )
{
    return _POW( 10.0f, Decibels / 20.0f ) ;
}

// XAudio2 の率値からデシベル値に変換する関数
__inline float D_XAudio2AmplitudeRatioToDecibels( float Volume )
{
    if( Volume == 0 )
    {
        return -3.402823466e+38f ;
    }
    return 20.0f * ( float )_LOG10( Volume ) ;
}












// ストリームサウンド処理用スレッド
static	DWORD WINAPI StreamSoundThreadFunction( void * )
{
	SETUP_WIN_API

	for(;;)
	{
		if( SoundSysData.PF.StreamSoundThreadEndFlag == 1 ) break ;
		if( SoundSysData.InitializeFlag == FALSE ) break ;

		// クリティカルセクションの取得
		CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

		// ストリーミング処理
		NS_ProcessStreamSoundMemAll() ;

		// 再生が終了したらハンドルを削除する処理を行う
//		ProcessPlayFinishDeleteSoundMemAll() ;

		// ３Ｄサウンドを再生しているサウンドハンドルに対する処理を行う
		ProcessPlay3DSoundMemAll() ;

		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;


		// クリティカルセクションの取得
		CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

		// ストリーミング処理
		ST_SoftSoundPlayerProcessAll() ;

		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

		// 待ち
		WinAPIData.Win32Func.SleepFunc( 10 ) ;
	}

	// スレッド終了
	SoundSysData.PF.StreamSoundThreadEndFlag = 2 ;
	WinAPIData.Win32Func.ExitThreadFunc( 0 ) ;

	return 0 ;
}






BOOL CALLBACK DSEnum( LPGUID /*lpGuid*/, LPCSTR lpcstrDescription, LPCSTR lpcstrModule, LPVOID /*lpContext*/ )
{
	DXST_LOGFILEFMT_ADDW(( L"Module Name : %15s   Description : %s ", lpcstrModule , lpcstrDescription )) ;

	return TRUE ;
}

// サウンドシステムを初期化する関数の環境依存処理を行う関数
extern int InitializeSoundSystem_PF_Timing0( void )
{
	HRESULT hr ;
	WAVEFORMATEX wfmtx ;
	D_DSCAPS caps ;

	SETUP_WIN_API

	if( SoundSysData.PF.DirectSoundObject != NULL )
	{
		return -1 ;
	}

INITSTART:

	// 再生モードによって処理を分岐
	switch( SoundSysData.SoundMode )
	{
	case DX_MIDIMODE_MCI :
		if( SoundSysData.PF.EnableXAudioFlag )
		{
			int i ;
			const wchar_t *X3DAudioDLLName[] =
			{
				L"X3DAudio1_7.dll",
				//L"X3DAudio1_6.dll",
				//L"X3DAudio1_5.dll",
				//L"X3DAudio1_4.dll",
				//L"X3DAudio1_3.dll",
				//L"X3DAudio1_2.dll",
				//L"X3DAudio1_1.dll",
				//L"X3DAudio1_0.dll",
				NULL,
			} ;
			GUID *XAudioGuidList[][ 2 ] = 
			{
				{
					&CLSID_XAUDIO2_7,
					&CLSID_XAUDIO2_7_DEBUG,
				},
				//{
				//	&CLSID_XAUDIO2_6,
				//	&CLSID_XAUDIO2_6_DEBUG,
				//},
				//{
				//	&CLSID_XAUDIO2_5,
				//	&CLSID_XAUDIO2_5_DEBUG,
				//},
				//{
				//	&CLSID_XAUDIO2_4,
				//	&CLSID_XAUDIO2_4_DEBUG,
				//},
				//{
				//	&CLSID_XAUDIO2_3,
				//	&CLSID_XAUDIO2_3_DEBUG,
				//},
				//{
				//	&CLSID_XAUDIO2_2,
				//	&CLSID_XAUDIO2_2_DEBUG,
				//},
				//{
				//	&CLSID_XAUDIO2_1,
				//	&CLSID_XAUDIO2_1_DEBUG,
				//},
				//{
				//	&CLSID_XAUDIO2_0,
				//	&CLSID_XAUDIO2_0_DEBUG,
				//},
				{
					NULL,
					NULL
				},
			} ;
			DXST_LOGFILE_ADDUTF16LE( "\x58\x00\x41\x00\x75\x00\x64\x00\x69\x00\x6f\x00\x32\x00\x20\x00\x6e\x30\x1d\x52\x1f\x67\x16\x53\x92\x30\x4c\x88\x44\x30\x7e\x30\x59\x30\x0a\x00\x00"/*@ L"XAudio2 の初期化を行います\n" @*/ ) ;

			DXST_LOGFILE_TABADD ;

			// 最初にXAudio2_8.dllが無いか調べる
			SoundSysData.PF.XAudio2_8DLL = LoadLibraryW( L"XAudio2_8.dll" ) ;
			if( SoundSysData.PF.XAudio2_8DLL != NULL )
			{
				DXST_LOGFILE_ADDUTF16LE( "\x58\x00\x41\x00\x75\x00\x64\x00\x69\x00\x6f\x00\x32\x00\x5f\x00\x38\x00\x2e\x00\x64\x00\x6c\x00\x6c\x00\x20\x00\x92\x30\x7f\x4f\x28\x75\x57\x30\x7e\x30\x59\x30\x0a\x00\x00"/*@ L"XAudio2_8.dll を使用します\n" @*/ ) ;

				// あった場合の処理
				SoundSysData.PF.XAudio2CreateFunc          = ( HRESULT ( WINAPI * )( D_IXAudio2_8 **, DWORD, D_XAUDIO2_PROCESSOR ) )GetProcAddress( SoundSysData.PF.XAudio2_8DLL, "XAudio2Create" ) ;
				SoundSysData.PF.CreateAudioVolumeMeterFunc = ( HRESULT ( WINAPI * )( D_IUnknown** ppApo ) )GetProcAddress( SoundSysData.PF.XAudio2_8DLL, "CreateAudioVolumeMeter" ) ;
				SoundSysData.PF.CreateAudioReverbFunc      = ( HRESULT ( WINAPI * )( D_IUnknown** ppApo ) )GetProcAddress( SoundSysData.PF.XAudio2_8DLL, "CreateAudioReverb" ) ;
				SoundSysData.PF.X3DAudioInitializeFunc     = ( void ( __cdecl * )( DWORD, float, D_X3DAUDIO_HANDLE ) )GetProcAddress( SoundSysData.PF.XAudio2_8DLL, "X3DAudioInitialize" ) ;
				SoundSysData.PF.X3DAudioCalculateFunc      = ( void ( __cdecl * )( const D_X3DAUDIO_HANDLE, const D_X3DAUDIO_LISTENER *, const D_X3DAUDIO_EMITTER *, DWORD, D_X3DAUDIO_DSP_SETTINGS * ) )GetProcAddress( SoundSysData.PF.XAudio2_8DLL, "X3DAudioCalculate" ) ;

				DXST_LOGFILE_ADDUTF16LE( "\x58\x00\x41\x00\x75\x00\x64\x00\x69\x00\x6f\x00\x32\x00\x20\x00\xa4\x30\xf3\x30\xbf\x30\xfc\x30\xd5\x30\xa7\x30\xfc\x30\xb9\x30\x6e\x30\xd6\x53\x97\x5f\x68\x30\x1d\x52\x1f\x67\x16\x53\x92\x30\x4c\x88\x44\x30\x7e\x30\x59\x30\x2e\x00\x2e\x00\x2e\x00\x2e\x00\x20\x00\x20\x00\x00"/*@ L"XAudio2 インターフェースの取得と初期化を行います....  " @*/ ) ;

				hr = SoundSysData.PF.XAudio2CreateFunc( &SoundSysData.PF.XAudio2_8Object, 0, D_XAUDIO2_DEFAULT_PROCESSOR ) ;
				if( FAILED( hr ) )
				{
					DXST_LOGFILE_ADDUTF16LE( "\x1d\x52\x1f\x67\x16\x53\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"初期化に失敗しました\n" @*/ ) ;
					DXST_LOGFILEFMT_ADDA(( "\x83\x47\x83\x89\x81\x5b\x83\x52\x81\x5b\x83\x68 %x"/*@ "エラーコード %x" @*/, hr )) ;
					SoundSysData.PF.XAudio2_8Object->Release() ;
					SoundSysData.PF.XAudio2_8Object = NULL ;

					SoundSysData.PF.EnableXAudioFlag = FALSE ;
					goto INITSTART ;
				}
				DXST_LOGFILE_ADDUTF16LE( "\x10\x62\x9f\x52\x20\x00\x56\x00\x65\x00\x72\x00\x32\x00\x2e\x00\x38\x00\x0a\x00\x00"/*@ L"成功 Ver2.8\n" @*/ ) ;

				hr = SoundSysData.PF.XAudio2_8Object->CreateMasteringVoice( &SoundSysData.PF.XAudio2_8MasteringVoiceObject, D_XAUDIO2_DEFAULT_CHANNELS, SoundSysData.OutputSmaplesPerSec ) ;
				if( FAILED( hr ) )
				{
					DXST_LOGFILE_ADDUTF16LE( "\x58\x00\x41\x00\x75\x00\x64\x00\x69\x00\x6f\x00\x32\x00\x4d\x00\x61\x00\x73\x00\x74\x00\x65\x00\x72\x00\x69\x00\x6e\x00\x67\x00\x56\x00\x6f\x00\x69\x00\x63\x00\x65\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"XAudio2MasteringVoiceの作成に失敗しました\n" @*/ ) ;
					SoundSysData.PF.XAudio2_8Object->Release() ;
					SoundSysData.PF.XAudio2_8Object = NULL ;

					SoundSysData.PF.EnableXAudioFlag = FALSE ;
					goto INITSTART ;
				}

				// リバーブ計算用パラメータ準備
				for( i = 0 ; i < D_XAUDIO2FX_PRESET_NUM ; i ++ )
				{
					D_ReverbConvertI3DL2ToNative2_8( &D_PRESET_PARAMS[ i ], &SoundSysData.PF.XAudio2_8ReverbParameters[ i ] ) ;
				}
			}
			else
			{
				DXST_LOGFILE_ADDUTF16LE( "\x58\x00\x41\x00\x75\x00\x64\x00\x69\x00\x6f\x00\x32\x00\x20\x00\xa4\x30\xf3\x30\xbf\x30\xfc\x30\xd5\x30\xa7\x30\xfc\x30\xb9\x30\x6e\x30\xd6\x53\x97\x5f\x92\x30\x4c\x88\x44\x30\x7e\x30\x59\x30\x2e\x00\x2e\x00\x2e\x00\x2e\x00\x20\x00\x20\x00\x00"/*@ L"XAudio2 インターフェースの取得を行います....  " @*/ ) ;
				for( i = 0 ; XAudioGuidList[ i ][ 0 ] != NULL ; i ++ )
				{
					hr = WinAPIData.Win32Func.CoCreateInstanceFunc( *XAudioGuidList[ i ][ 0 ], NULL, CLSCTX_ALL, IID_IXAUDIO2, ( LPVOID *)&SoundSysData.PF.XAudio2Object );
					if( !FAILED( hr ) )
					{
						break ;
					}
				}
				if( XAudioGuidList[ i ][ 0 ] != NULL )
				{
					DXST_LOGFILEFMT_ADDUTF16LE(( "\x10\x62\x9f\x52\x20\x00\x56\x00\x65\x00\x72\x00\x32\x00\x2e\x00\x25\x00\x64\x00\x0a\x00\x00"/*@ L"成功 Ver2.%d\n" @*/, 7 - i )) ;
					DXST_LOGFILE_ADDUTF16LE(( "\x15\x5f\x4d\x30\x9a\x7d\x4d\x30\xa4\x30\xf3\x30\xbf\x30\xfc\x30\xd5\x30\xa7\x30\xfc\x30\xb9\x30\x6e\x30\x1d\x52\x1f\x67\x16\x53\xe6\x51\x06\x74\x2e\x00\x2e\x00\x2e\x00\x20\x00\x20\x00\x00"/*@ L"引き続きインターフェースの初期化処理...  " @*/ )) ;
					hr = SoundSysData.PF.XAudio2Object->Initialize( 0 ) ;
					if( FAILED( hr ) )
					{
						DXST_LOGFILE_ADDUTF16LE( "\x1d\x52\x1f\x67\x16\x53\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"初期化に失敗しました\n" @*/ ) ;
						DXST_LOGFILEFMT_ADDA(( "\x83\x47\x83\x89\x81\x5b\x83\x52\x81\x5b\x83\x68 %x"/*@ "エラーコード %x" @*/, hr )) ;
						SoundSysData.PF.XAudio2Object->Release() ;
						SoundSysData.PF.XAudio2Object = NULL ;

						SoundSysData.PF.EnableXAudioFlag = FALSE ;
						goto INITSTART ;
					}
					DXST_LOGFILE_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"成功\n" @*/ ) ;

					DXST_LOGFILE_ADDUTF16LE( "\x58\x00\x33\x00\x44\x00\x41\x00\x75\x00\x64\x00\x69\x00\x6f\x00\x2e\x00\x44\x00\x4c\x00\x4c\x00\x20\x00\x6e\x30\xad\x8a\x7f\x30\xbc\x8f\x7f\x30\x2d\x4e\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"X3DAudio.DLL の読み込み中... " @*/ ) ;
					for( i = 0 ; X3DAudioDLLName[ i ] != NULL ; i ++ )
					{
						SoundSysData.PF.X3DAudioDLL = LoadLibraryW( X3DAudioDLLName[ i ] ) ;
						if( SoundSysData.PF.X3DAudioDLL != NULL )
							break ;
					}
					if( SoundSysData.PF.X3DAudioDLL == NULL )
					{
						DXST_LOGFILE_ADDUTF16LE( "\x31\x59\x57\x65\x01\x30\x58\x00\x33\x00\x44\x00\x41\x00\x75\x00\x64\x00\x69\x00\x6f\x00\x01\x30\x58\x00\x41\x00\x75\x00\x64\x00\x69\x00\x6f\x00\x32\x00\x6f\x30\x7f\x4f\x28\x75\x57\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"失敗、X3DAudio、XAudio2は使用しません\n" @*/ ) ;
						SoundSysData.PF.XAudio2Object->Release() ;
						SoundSysData.PF.XAudio2Object = NULL ;

						SoundSysData.PF.EnableXAudioFlag = FALSE ;
						goto INITSTART ;
					}
					else
					{
						DXST_LOGFILE_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"成功\n" @*/ ) ;

						SoundSysData.PF.X3DAudioInitializeFunc = ( void ( __cdecl * )( DWORD, float, D_X3DAUDIO_HANDLE ) )GetProcAddress( SoundSysData.PF.X3DAudioDLL, "X3DAudioInitialize" ) ;
						SoundSysData.PF.X3DAudioCalculateFunc  = ( void ( __cdecl * )( const D_X3DAUDIO_HANDLE, const D_X3DAUDIO_LISTENER *, const D_X3DAUDIO_EMITTER *, DWORD, D_X3DAUDIO_DSP_SETTINGS * ) )GetProcAddress( SoundSysData.PF.X3DAudioDLL, "X3DAudioCalculate" ) ;
					}
				}
				else
				{
					SoundSysData.PF.XAudio2Object = NULL ;
					DXST_LOGFILE_ADDUTF16LE( "\x58\x00\x41\x00\x75\x00\x64\x00\x69\x00\x6f\x00\x32\x00\xa4\x30\xf3\x30\xbf\x30\xfc\x30\xd5\x30\xa7\x30\xfc\x30\xb9\x30\x6e\x30\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"XAudio2インターフェースの取得に失敗しました\n" @*/ ) ;

					SoundSysData.PF.EnableXAudioFlag = FALSE ;
					goto INITSTART ;
				}

				hr = SoundSysData.PF.XAudio2Object->CreateMasteringVoice( &SoundSysData.PF.XAudio2MasteringVoiceObject, D_XAUDIO2_DEFAULT_CHANNELS, SoundSysData.OutputSmaplesPerSec ) ;
				if( FAILED( hr ) )
				{
					DXST_LOGFILE_ADDUTF16LE( "\x58\x00\x41\x00\x75\x00\x64\x00\x69\x00\x6f\x00\x32\x00\x4d\x00\x61\x00\x73\x00\x74\x00\x65\x00\x72\x00\x69\x00\x6e\x00\x67\x00\x56\x00\x6f\x00\x69\x00\x63\x00\x65\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"XAudio2MasteringVoiceの作成に失敗しました\n" @*/ ) ;

					if( SoundSysData.PF.X3DAudioDLL != NULL )
					{
						FreeLibrary( SoundSysData.PF.X3DAudioDLL ) ;
						SoundSysData.PF.X3DAudioInitializeFunc = NULL ;
						SoundSysData.PF.X3DAudioCalculateFunc = NULL ;
						SoundSysData.PF.X3DAudioDLL = NULL ;
					}

					SoundSysData.PF.XAudio2Object->Release() ;
					SoundSysData.PF.XAudio2Object = NULL ;

					SoundSysData.PF.EnableXAudioFlag = FALSE ;
					goto INITSTART ;
				}

				// リバーブ計算用パラメータ準備
				for( i = 0 ; i < D_XAUDIO2FX_PRESET_NUM ; i ++ )
				{
					D_ReverbConvertI3DL2ToNative( &D_PRESET_PARAMS[ i ], &SoundSysData.PF.XAudio2ReverbParameters[ i ] ) ;
				}
			}
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x53\x00\x6f\x00\x75\x00\x6e\x00\x64\x00\x20\x00\x6e\x30\x1d\x52\x1f\x67\x16\x53\x92\x30\x4c\x88\x44\x30\x7e\x30\x59\x30\x0a\x00\x00"/*@ L"DirectSound の初期化を行います\n" @*/ ) ;
			DXST_LOGFILE_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x53\x00\x6f\x00\x75\x00\x6e\x00\x64\x00\x20\x00\xa4\x30\xf3\x30\xbf\x30\xfc\x30\xd5\x30\xa7\x30\xfc\x30\xb9\x30\x6e\x30\xd6\x53\x97\x5f\x92\x30\x4c\x88\x44\x30\x7e\x30\x59\x30\x2e\x00\x2e\x00\x2e\x00\x2e\x00\x20\x00\x20\x00\x00"/*@ L"DirectSound インターフェースの取得を行います....  " @*/ ) ; 
			hr = WinAPIData.Win32Func.CoCreateInstanceFunc( CLSID_DIRECTSOUND, NULL, CLSCTX_ALL, IID_IDIRECTSOUND, ( LPVOID *)&SoundSysData.PF.DirectSoundObject );
			if( !FAILED( hr ) )
			{
				DXST_LOGFILE_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"成功\n" @*/ ) ;
				DXST_LOGFILE_ADDUTF16LE( "\x15\x5f\x4d\x30\x9a\x7d\x4d\x30\xa4\x30\xf3\x30\xbf\x30\xfc\x30\xd5\x30\xa7\x30\xfc\x30\xb9\x30\x6e\x30\x1d\x52\x1f\x67\x16\x53\xe6\x51\x06\x74\x2e\x00\x2e\x00\x2e\x00\x20\x00\x20\x00\x00"/*@ L"引き続きインターフェースの初期化処理...  " @*/ ) ;
				hr = SoundSysData.PF.DirectSoundObject->Initialize( NULL ) ;
				if( FAILED( hr ) )
				{
					DXST_LOGFILE_ADDUTF16LE( "\x1d\x52\x1f\x67\x16\x53\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"初期化に失敗しました\n" @*/ ) ;
					DXST_LOGFILEFMT_ADDA(( "\x83\x47\x83\x89\x81\x5b\x83\x52\x81\x5b\x83\x68 %x"/*@ "エラーコード %x" @*/, hr )) ;
					SoundSysData.PF.DirectSoundObject->Release() ;
					SoundSysData.PF.DirectSoundObject = NULL ;
					return -1 ;
				}
				DXST_LOGFILE_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"成功\n" @*/ ) ;
			}
			else
			{
				SoundSysData.PF.DirectSoundObject = NULL ;
				DXST_LOGFILE_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x53\x00\x6f\x00\x75\x00\x6e\x00\x64\x00\xa4\x30\xf3\x30\xbf\x30\xfc\x30\xd5\x30\xa7\x30\xfc\x30\xb9\x30\x6e\x30\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"DirectSoundインターフェースの取得に失敗しました\n" @*/ ) ;
				return -1 ;
			}
	/*
			DXST_LOGFILE_ADDUTF16LE( L"DirectSound の初期化を行います\n" ) ; 
			hr = DirectSoundCreate( NULL , &SoundSysData.PF.DirectSoundObject , NULL ) ;
			if( hr != D_DS_OK )
			{
				SoundSysData.PF.DirectSoundObject = NULL ;
				DXST_LOGFILE_ADDUTF16LE( L"DirectSoundの初期化に失敗しました\n" ) ;
				return -1 ;
			}
	*/

			// 強調レベルをセットする
			hr = SoundSysData.PF.DirectSoundObject->SetCooperativeLevel( WinAPIData.Win32Func.GetDesktopWindowFunc(), D_DSSCL_PRIORITY ) ;
			if( hr != D_DS_OK )
			{
				DXST_LOGFILE_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x53\x00\x6f\x00\x75\x00\x6e\x00\x64\x00\x6e\x30\x54\x53\xbf\x8a\xec\x30\xd9\x30\xeb\x30\x6e\x30\x2d\x8a\x9a\x5b\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"DirectSoundの協調レベルの設定に失敗しました\n" @*/ ) ;
				SoundSysData.PF.DirectSoundObject->Release() ;
				SoundSysData.PF.DirectSoundObject = NULL ;
				return -1 ;
			}

			// プライマリサウンドバッファを作成する
			{
				D_DSBUFFERDESC dsbdesc ;
				DWORD ChNum , Rate , Byte ;

				// 作成ステータスをセットする
				_MEMSET( &dsbdesc, 0, sizeof( dsbdesc ) ) ;
				dsbdesc.dwSize = sizeof( dsbdesc ) ;
				dsbdesc.dwFlags = ( DWORD )( D_DSBCAPS_PRIMARYBUFFER | ( SoundSysData.PF.UseSoftwareMixing ? D_DSBCAPS_LOCSOFTWARE : 0 ) ) ;
				hr = SoundSysData.PF.DirectSoundObject->CreateSoundBuffer( &dsbdesc , &SoundSysData.PF.PrimarySoundBuffer , NULL ) ;
				if( hr != D_DS_OK )
				{
					DXST_LOGFILE_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x53\x00\x6f\x00\x75\x00\x6e\x00\x64\x00\x6e\x30\xd7\x30\xe9\x30\xa4\x30\xde\x30\xea\x30\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"DirectSoundのプライマリサウンドバッファの作成に失敗しました\n" @*/ ) ;
					SoundSysData.PF.DirectSoundObject->Release() ;
					SoundSysData.PF.DirectSoundObject = NULL ;
					return -1 ;
				}

				// 情報を得る
				_MEMSET( &caps, 0, sizeof( caps ) ) ;
				caps.dwSize = sizeof( caps ) ;
				SoundSysData.PF.DirectSoundObject->GetCaps( &caps ) ;


				// プライマリバッファのフォーマットをセットする
				ChNum = ( DWORD )( caps.dwFlags & D_DSCAPS_PRIMARYSTEREO ? 2 : 1 ) ;
				Rate  = caps.dwMaxSecondarySampleRate < 44100 ? caps.dwMaxSecondarySampleRate : 44100  ;
				Byte  = ( DWORD )( caps.dwFlags & D_DSCAPS_PRIMARY16BIT  ? 2 : 1 ) ;

				_MEMSET( &wfmtx, 0, sizeof( wfmtx ) ) ;
				wfmtx.wFormatTag		= WAVE_FORMAT_PCM ;											// PCMフォーマット
				wfmtx.nChannels			= ( WORD )ChNum ;											// チャンネル２つ＝ステレオ
				wfmtx.nSamplesPerSec	= Rate ;													// 再生レート
				wfmtx.wBitsPerSample	= ( WORD )( Byte * 8 ) ;									// １音にかかるデータビット数
				wfmtx.nBlockAlign		= ( WORD )( wfmtx.wBitsPerSample / 8 * wfmtx.nChannels ) ;	// １ヘルツにかかるデータバイト数
				wfmtx.nAvgBytesPerSec	= wfmtx.nSamplesPerSec * wfmtx.nBlockAlign ;				// １秒にかかるデータバイト数
				hr = SoundSysData.PF.PrimarySoundBuffer->SetFormat( &wfmtx ) ;
				if( hr != D_DS_OK )
				{
					DXST_LOGFILE_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x53\x00\x6f\x00\x75\x00\x6e\x00\x64\x00\x6e\x30\xd7\x30\xe9\x30\xa4\x30\xde\x30\xea\x30\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x2d\x8a\x9a\x5b\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"DirectSoundのプライマリサウンドバッファのフォーマット設定に失敗しました\n" @*/ ) ;
				}

			}
		}
		break ;

	case DX_MIDIMODE_DIRECT_MUSIC_REVERB :
	case DX_MIDIMODE_DIRECT_MUSIC_NORMAL :
		SoundSysData.PF.EnableXAudioFlag = FALSE ;

		// ＤｉｒｅｃｔＭｕｓｉｃによる再生の場合の処理
		{
			D_IDirectMusic *DMusic ;
			D_IDirectMusic8 *DMusic8 ;
			GUID MidiGuid = {0};
			bool MidiGuidValid ;
				
			DXST_LOGFILE_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x53\x00\x6f\x00\x75\x00\x6e\x00\x64\x00\x38\x00\x20\x00\x6e\x30\x1d\x52\x1f\x67\x16\x53\x92\x30\x4c\x88\x44\x30\x7e\x30\x59\x30\x0a\x00\x00"/*@ L"DirectSound8 の初期化を行います\n" @*/ ) ; 

			// DirectMusic オブジェクトの作成
//			if( FAILED(	WinAPIData.Win32Func.CoCreateInstanceFunc( CLSID_DirectMusicLoader , NULL , 
//											CLSCTX_INPROC , IID_IDirectMusicLoader8 , ( void ** )&SoundSysData.PF.DirectMusicLoaderObject ) ) )
			if( FAILED(	WinAPIData.Win32Func.CoCreateInstanceFunc( CLSID_DIRECTMUSICLOADER, NULL , 
											CLSCTX_INPROC , IID_IDIRECTMUSICLOADER8 , ( void ** )&SoundSysData.PF.DirectMusicLoaderObject ) ) )
			{
				DXST_LOGFILE_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x4d\x00\x75\x00\x73\x00\x69\x00\x63\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x65\x00\x72\x00\x38\x00\x20\x00\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"DirectMusicLoader8 オブジェクトの作成に失敗しました\n" @*/ ) ;
				DXST_LOGFILE_ADDUTF16LE( "\x4d\x52\xd0\x30\xfc\x30\xb8\x30\xe7\x30\xf3\x30\x67\x30\x6e\x30\x1d\x52\x1f\x67\x16\x53\xe6\x51\x06\x74\x6b\x30\xfb\x79\x8a\x30\x7e\x30\x59\x30\x0a\x00\x00"/*@ L"前バージョンでの初期化処理に移ります\n" @*/ ) ;
				SoundSysData.SoundMode = DX_MIDIMODE_MCI ;
				goto INITSTART ;
			}

//			if( FAILED( WinAPIData.Win32Func.CoCreateInstanceFunc( CLSID_DirectMusicPerformance , NULL , 
//											CLSCTX_INPROC , IID_IDirectMusicPerformance8 , ( void ** )&SoundSysData.PF.DirectMusicPerformanceObject ) ) )
			if( FAILED( WinAPIData.Win32Func.CoCreateInstanceFunc( CLSID_DIRECTMUSICPERFORMANCE, NULL , 
											CLSCTX_INPROC , IID_IDIRECTMUSICPERFORMANCE8 , ( void ** )&SoundSysData.PF.DirectMusicPerformanceObject ) ) )
			{
				// ローダーを解放する
				if( SoundSysData.PF.DirectMusicLoaderObject )
				{
					SoundSysData.PF.DirectMusicLoaderObject->Release() ; 
					SoundSysData.PF.DirectMusicLoaderObject = NULL ;
				}

				DXST_LOGFILE_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x4d\x00\x75\x00\x73\x00\x69\x00\x63\x00\x50\x00\x65\x00\x72\x00\x66\x00\x6f\x00\x72\x00\x6d\x00\x61\x00\x6e\x00\x63\x00\x65\x00\x20\x00\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"DirectMusicPerformance オブジェクトの作成に失敗しました\n" @*/ ) ;
				SoundSysData.SoundMode = DX_MIDIMODE_MCI ;
				goto INITSTART ;
//				TerminateSoundSystem() ;
//				return -1 ;
			}

			MidiGuidValid = false ;
			// MidiSystemData音源の列挙
			{
				D_DMUS_PORTCAPS Param ;
				WCHAR wStr[D_DMUS_MAX_DESCRIPTION+3] ;
				int i ;

				if( FAILED( WinAPIData.Win32Func.CoCreateInstanceFunc( CLSID_DIRECTMUSIC, NULL,
												CLSCTX_INPROC_SERVER, IID_IDIRECTMUSIC, (LPVOID*)&DMusic ) ) )
				{
					DXST_LOGFILE_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x4d\x00\x75\x00\x73\x00\x69\x00\x63\x00\x20\x00\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"DirectMusic オブジェクトの取得に失敗しました\n" @*/ ) ;
					TerminateSoundSystem() ;
					return -1 ;
				}
			
//				if( FAILED( DMusic->QueryInterface( IID_IDirectMusic8, (void **)&DMusic8 ) ) )
				if( FAILED( DMusic->QueryInterface( IID_IDIRECTMUSIC8, (void **)&DMusic8 ) ) )
				{
					DMusic->Release() ;
					DXST_LOGFILE_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x4d\x00\x75\x00\x73\x00\x69\x00\x63\x00\x38\x00\x20\x00\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"DirectMusic8 オブジェクトの取得に失敗しました\n" @*/ ) ;
					TerminateSoundSystem() ;
					return -1 ;
				}
				
				_MEMSET( &Param, 0, sizeof( Param ) ) ;
				Param.dwSize = sizeof(D_DMUS_PORTCAPS) ;
				
				for( i = 0 ; DMusic8->EnumPort( ( DWORD )i, &Param ) != S_FALSE ; i ++ )
				{
					_WCSCPY_S( wStr, sizeof( wStr ), Param.wszDescription ) ;
					wStr[D_DMUS_MAX_DESCRIPTION] = L'\0' ;
					DXST_LOGFILEFMT_ADDW(( L"%s  Device%d:%s ", Param.dwClass == D_DMUS_PC_OUTPUTCLASS ? L"Output port" : L"Input port", i, wStr )) ;
				}
				
				DMusic8->Release() ;
				DMusic->Release() ;
			}
	
			// DirectMusicPerformanceオブジェクトの初期化
			{
				D_DMUS_AUDIOPARAMS AudioParams ;

				_MEMSET( &AudioParams, 0, sizeof( AudioParams ) ) ;
				AudioParams.dwSize = sizeof( AudioParams ) ;
				AudioParams.fInitNow = TRUE ;
				AudioParams.dwValidData = D_DMUS_AUDIOPARAMS_SAMPLERATE | D_DMUS_AUDIOPARAMS_FEATURES ;
				AudioParams.dwSampleRate = 44100 ;
				AudioParams.dwFeatures = D_DMUS_AUDIOF_ALL ;
				if( MidiGuidValid == true )
				{
					AudioParams.dwValidData |= D_DMUS_AUDIOPARAMS_DEFAULTSYNTH  ;
					AudioParams.clsidDefaultSynth = MidiGuid ;
				}

				SoundSysData.PF.DirectSoundObject = NULL ;
				DMusic = NULL ;
				if( SoundSysData.PF.DirectMusicPerformanceObject->InitAudio(
											&DMusic ,									// IDirectMusicインターフェイスは不要。
											&SoundSysData.PF.DirectSoundObject,			// IDirectSoundインターフェイスポインタを渡す。
											NULL ,										// ウィンドウのハンドル。
											SoundSysData.SoundMode == DX_MIDIMODE_DIRECT_MUSIC_NORMAL ? D_DMUS_APATH_DYNAMIC_STEREO : D_DMUS_APATH_SHARED_STEREOPLUSREVERB,		// デフォルトのオーディオパスタイプ
											64 ,										// パフォーマンスチャンネルの数。
											D_DMUS_AUDIOF_ALL ,							// シンセサイザの機能。
											&AudioParams								// オーディオパラメータにはデフォルトを使用。
										) != S_OK )
				{
					DXST_LOGFILE_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x4d\x00\x75\x00\x73\x00\x69\x00\x63\x00\x50\x00\x65\x00\x72\x00\x66\x00\x6f\x00\x72\x00\x6d\x00\x61\x00\x6e\x00\x63\x00\x65\x00\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\x1d\x52\x1f\x67\x16\x53\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"DirectMusicPerformanceオブジェクトの初期化に失敗しました\n" @*/ ) ;
					TerminateSoundSystem() ;
					return -1 ;
				}
			}

			// 最大音量の取得
			SoundSysData.PF.DirectMusicPerformanceObject->GetGlobalParam( GUID_PERFMASTERVOLUME , ( void * )&SoundSysData.MaxVolume , sizeof( long ) ) ;
		}
		break ;
	}

	if( SoundSysData.PF.EnableXAudioFlag )
	{
		// 情報出力
		if( SoundSysData.PF.XAudio2_8DLL == NULL )
		{
			D_XAUDIO2_DEVICE_DETAILS Details ;

			SoundSysData.PF.XAudio2Object->GetDeviceDetails( 0, &Details ) ;

			DXST_LOGFILE_TABADD ;

#ifndef DX_GCC_COMPILE
			DXST_LOGFILEFMT_ADDW(( L"Device Name : %s", Details.DisplayName )) ;
#endif
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xc1\x30\xe3\x30\xf3\x30\xcd\x30\xeb\x30\x70\x65\x3a\x00\x25\x00\x64\x00\x00"/*@ L"チャンネル数:%d" @*/, Details.OutputFormat.Format.nChannels )) ;
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xd3\x30\xc3\x30\xc8\x30\xbe\x7c\xa6\x5e\x3a\x00\x25\x00\x64\x00\x62\x00\x69\x00\x74\x00\x00"/*@ L"ビット精度:%dbit" @*/, Details.OutputFormat.Format.wBitsPerSample )) ;
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xb5\x30\xf3\x30\xd7\x30\xea\x30\xf3\x30\xb0\x30\xec\x30\xfc\x30\xc8\x30\x3a\x00\x25\x00\x64\x00\x48\x00\x7a\x00\x00"/*@ L"サンプリングレート:%dHz" @*/, Details.OutputFormat.Format.nSamplesPerSec )) ;

			DXST_LOGFILE_TABSUB ;

			SoundSysData.PF.XAudio2OutputChannelMask = Details.OutputFormat.dwChannelMask ;
			SoundSysData.OutputChannels = Details.OutputFormat.Format.nChannels ;
		}
		else
		{
			int i ;

			SoundSysData.PF.XAudio2_8MasteringVoiceObject->GetChannelMask( &SoundSysData.PF.XAudio2OutputChannelMask ) ;
			SoundSysData.OutputChannels = 0 ;
			for( i = 0 ; i < 32 ; i ++ )
			{
				if( ( SoundSysData.PF.XAudio2OutputChannelMask & ( 1 << i ) ) != 0 )
					SoundSysData.OutputChannels ++ ;
			}
		}

		// X3DAudio の初期化
		SoundSysData.PF.X3DAudioInitializeFunc( SoundSysData.PF.XAudio2OutputChannelMask, D_X3DAUDIO_SPEED_OF_SOUND, SoundSysData.PF.X3DAudioInstance ) ;
	}
	else
	{
		// 無音バッファの初期化
		{
			D_DSBUFFERDESC dsbdesc ;
			WAVEFORMATEX wfmtex ;

			// バッファ生成ステータスのセット
			_MEMSET( &wfmtex, 0, sizeof( wfmtex ) ) ;
			wfmtex.wFormatTag		= WAVE_FORMAT_PCM ;								// PCMフォーマット
			wfmtex.nChannels			= 1 ;										// チャンネル１つ＝モノラル
			wfmtex.nSamplesPerSec	= 22050 ;										// サンプリングレート
			wfmtex.wBitsPerSample	= 16 ;											// １サンプルにかかるビット数
			wfmtex.nBlockAlign		= 2 ;											// １サンプルにかかるバイト数　×　チャンネル数
			wfmtex.nAvgBytesPerSec	= wfmtex.nSamplesPerSec * wfmtex.nBlockAlign ;	// １秒分のデータバイト数

			_MEMSET( &dsbdesc, 0, sizeof( dsbdesc ) ) ;
			dsbdesc.dwSize			= sizeof( dsbdesc ) ;
			dsbdesc.dwFlags			= ( DWORD )( D_DSBCAPS_GLOBALFOCUS | D_DSBCAPS_CTRLPAN | D_DSBCAPS_CTRLVOLUME | D_DSBCAPS_CTRLFREQUENCY | D_DSBCAPS_GETCURRENTPOSITION2 | ( SoundSysData.PF.UseSoftwareMixing ? D_DSBCAPS_LOCSOFTWARE : D_DSBCAPS_STATIC ) ) ;
			dsbdesc.dwBufferBytes	= ( DWORD )( wfmtex.nBlockAlign * 9000 ) ;
			dsbdesc.lpwfxFormat		= &wfmtex ;

			if( SoundSysData.PF.DirectSoundObject->CreateSoundBuffer( &dsbdesc , &SoundSysData.PF.NoSoundBuffer , NULL ) != D_DS_OK )
			{
				// 作成に失敗した場合は、含める機能を減らして再度作成する
				_MEMSET( &dsbdesc, 0, sizeof( dsbdesc ) ) ;
				dsbdesc.dwSize			= sizeof( dsbdesc ) ;
				dsbdesc.dwFlags			= ( DWORD )( D_DSBCAPS_CTRLPAN | D_DSBCAPS_CTRLVOLUME | ( SoundSysData.PF.UseSoftwareMixing ? D_DSBCAPS_LOCSOFTWARE : D_DSBCAPS_STATIC ) ) ;
				dsbdesc.dwBufferBytes	= ( DWORD )( wfmtex.nBlockAlign * 9000 ) ;
				dsbdesc.lpwfxFormat		= &wfmtex ;
				if( SoundSysData.PF.DirectSoundObject->CreateSoundBuffer( &dsbdesc , &SoundSysData.PF.NoSoundBuffer , NULL ) != D_DS_OK )
				{
					DXST_LOGFILE_ADDUTF16LE( "\x21\x71\xf3\x97\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"無音バッファの作成に失敗しました" @*/ ) ;
					SoundSysData.PF.NoSoundBuffer = NULL ;
					goto R1 ;
				}
			}

			// 無音で埋める
			{
				LPVOID write1 ;
				DWORD length1 ;
				LPVOID write2 ;
				DWORD length2 ;

				hr = SoundSysData.PF.NoSoundBuffer->Lock( 0 , dsbdesc.dwBufferBytes, &write1 , &length1 , &write2 , &length2 , 0 ) ;		// バッファのロック
				if( hr != D_DS_OK )
				{
					DXST_LOGFILE_ADDUTF16LE( "\x21\x71\xf3\x97\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\xed\x30\xc3\x30\xaf\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"無音バッファのロックに失敗しました" @*/ ) ;
					SoundSysData.PF.NoSoundBuffer->Release() ;
					SoundSysData.PF.NoSoundBuffer = NULL ;
				}

				_MEMSET( write1, 0, length1 ) ;
				if( write2 != 0 )
				{
					_MEMSET( write2, 0, length2 ) ;
				}

				hr = SoundSysData.PF.NoSoundBuffer->Unlock( write1, length1, write2, length2 ) ;								// バッファのロック解除
			}

			// 無音バッファの再生
			SoundSysData.PF.NoSoundBuffer->Play( 0, 0, D_DSBPLAY_LOOPING ) ;
		}

R1 :
		// 性能表示
		if( SoundSysData.PF.DirectSoundObject )
		{
			typedef HRESULT ( WINAPI *DIRECTSOUNDENUMERATEFUNC )( LPD_DSENUMCALLBACKA, LPVOID lpContext ) ;
			DIRECTSOUNDENUMERATEFUNC DirectSoundEnumerateFunc = NULL ;
			HINSTANCE DSoundDLL = NULL ;
	 
			DXST_LOGFILE_TABADD ;
			_MEMSET( &caps, 0, sizeof( caps ) ) ;
			caps.dwSize = sizeof( caps ) ;
			SoundSysData.PF.DirectSoundObject->GetCaps( &caps ) ;

			DXST_LOGFILE_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x53\x00\x6f\x00\x75\x00\x6e\x00\x64\x00\x20\x00\xc7\x30\xd0\x30\xa4\x30\xb9\x30\x92\x30\x17\x52\x19\x63\x57\x30\x7e\x30\x59\x30\x0a\x00\x00"/*@ L"DirectSound デバイスを列挙します\n" @*/ ) ;
			DXST_LOGFILE_TABADD ;
			
			// DirectSound DLL をロードする
			if( ( DSoundDLL = LoadLibraryA( "DSound.DLL" ) ) == NULL )
			{
				DXST_LOGFILE_ADDUTF16LE( "\x44\x00\x53\x00\x4f\x00\x55\x00\x4e\x00\x44\x00\x2e\x00\x44\x00\x4c\x00\x4c\x00\x20\x00\x6e\x30\xed\x30\xfc\x30\xc9\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"SoundSysData.DLL のロードに失敗しました\n" @*/ ) ;
				DXST_LOGFILE_TABSUB ;
				DXST_LOGFILE_TABSUB ;
				goto ENUMEND ;
			}
		
			// 列挙用関数の取得
			if( ( DirectSoundEnumerateFunc = ( DIRECTSOUNDENUMERATEFUNC )GetProcAddress( DSoundDLL, "DirectSoundEnumerateW" ) ) == NULL )
			{
				FreeLibrary( DSoundDLL );
				DXST_LOGFILE_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x53\x00\x6f\x00\x75\x00\x6e\x00\x64\x00\x20\x00\xc7\x30\xd0\x30\xa4\x30\xb9\x30\x6e\x30\x17\x52\x19\x63\x28\x75\xa2\x95\x70\x65\x6e\x30\xdd\x30\xa4\x30\xf3\x30\xbf\x30\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"DirectSound デバイスの列挙用関数のポインタ取得に失敗しました\n" @*/ ) ;
				DXST_LOGFILE_TABSUB ;
				DXST_LOGFILE_TABSUB ;
				goto ENUMEND ;
			}

			// 列挙
			DirectSoundEnumerateFunc( DSEnum , NULL ) ;

			DXST_LOGFILE_TABSUB ;

			DXST_LOGFILEFMT_ADDUTF16LE(( "\x00\x67\x27\x59\xb5\x30\xf3\x30\xd7\x30\xea\x30\xf3\x30\xb0\x30\xec\x30\xfc\x30\xc8\x30\x3a\x00\x25\x00\x2e\x00\x32\x00\x66\x00\x4b\x00\x48\x00\x7a\x00\x20\x00\x00\x67\x0f\x5c\xb5\x30\xf3\x30\xd7\x30\xea\x30\xf3\x30\xb0\x30\xec\x30\xfc\x30\xc8\x30\x3a\x00\x25\x00\x2e\x00\x32\x00\x66\x00\x4b\x00\x48\x00\x7a\x00\x20\x00\x00"/*@ L"最大サンプリングレート:%.2fKHz 最小サンプリングレート:%.2fKHz " @*/,
						( double )caps.dwMaxSecondarySampleRate / 1000 ,
						( double )caps.dwMinSecondarySampleRate / 1000 )) ;

			DXST_LOGFILEFMT_ADDUTF16LE(( "\xcf\x7d\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x3a\x00\x25\x00\x2e\x00\x32\x00\x66\x00\x4b\x00\x42\x00\x20\x00\x7a\x7a\x4d\x30\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x3a\x00\x25\x00\x2e\x00\x32\x00\x66\x00\x4b\x00\x42\x00\x20\x00\x0a\x00\x00"/*@ L"総サウンドメモリ領域:%.2fKB 空きサウンドメモリ領域:%.2fKB \n" @*/,
						( double )caps.dwTotalHwMemBytes / 0x100 ,
						( double )caps.dwFreeHwMemBytes / 0x100 )) ;

			DXST_LOGFILE_ADDUTF16LE( "\x29\x52\x28\x75\xef\x53\xfd\x80\xb5\x30\xf3\x30\xd7\x30\xea\x30\xf3\x30\xb0\x30\xbe\x7c\xa6\x5e\x0a\x00\x00"/*@ L"利用可能サンプリング精度\n" @*/ ) ;
			DXST_LOGFILEFMT_ADDW(( L"  Primary    16bit = %s  8bit = %s ",
				( caps.dwFlags & D_DSCAPS_PRIMARY16BIT ? L"OK" : L"NO" ) ,
				( caps.dwFlags & D_DSCAPS_PRIMARY8BIT  ? L"OK" : L"NO" ) )) ;

			DXST_LOGFILEFMT_ADDW(( L"  Secondary  16bit = %s  8bit = %s \n",
				( caps.dwFlags & D_DSCAPS_SECONDARY16BIT ? L"OK" : L"NO" ) ,
				( caps.dwFlags & D_DSCAPS_SECONDARY8BIT  ? L"OK" : L"NO" ) )) ;

			DXST_LOGFILE_ADDUTF16LE( "\x29\x52\x28\x75\xef\x53\xfd\x80\xc1\x30\xe3\x30\xf3\x30\xcd\x30\xeb\x30\x0a\x00\x00"/*@ L"利用可能チャンネル\n" @*/ ) ;
			DXST_LOGFILEFMT_ADDW(( L"  Primary    MONO = %s   STEREO = %s ",
				( caps.dwFlags & D_DSCAPS_PRIMARYMONO   ? L"OK" : L"NO" ) ,
				( caps.dwFlags & D_DSCAPS_PRIMARYSTEREO ? L"OK" : L"NO" ) )) ;

			DXST_LOGFILEFMT_ADDW(( L"  Secondary  MONO = %s   STEREO = %s \n",
				( caps.dwFlags & D_DSCAPS_SECONDARYMONO   ? L"OK" : L"NO" ) ,
				( caps.dwFlags & D_DSCAPS_SECONDARYSTEREO ? L"OK" : L"NO" ) )) ;

			DXST_LOGFILE_TABSUB ;

			// DirectSound DLL を解放する
			FreeLibrary( DSoundDLL );
		}
	}

ENUMEND :

	// ３Ｄサウンド処理用のリスナー情報を初期化
	SoundSysData.PF.X3DAudioListenerConeData = Listener_DirectionalCone ;
	SoundSysData.PF.X3DAudioListenerData.Position.x = 0.0f ;
	SoundSysData.PF.X3DAudioListenerData.Position.y = 0.0f ;
	SoundSysData.PF.X3DAudioListenerData.Position.z = 0.0f ;
	SoundSysData.PF.X3DAudioListenerData.Velocity.x = 0.0f ;
	SoundSysData.PF.X3DAudioListenerData.Velocity.y = 0.0f ;
	SoundSysData.PF.X3DAudioListenerData.Velocity.z = 0.0f ;
	SoundSysData.PF.X3DAudioListenerData.OrientFront.x = 0.0f ;
	SoundSysData.PF.X3DAudioListenerData.OrientFront.y = 0.0f ;
	SoundSysData.PF.X3DAudioListenerData.OrientFront.z = 1.0f ;
	SoundSysData.PF.X3DAudioListenerData.OrientTop.x = 0.0f ;
	SoundSysData.PF.X3DAudioListenerData.OrientTop.y = 1.0f ;
	SoundSysData.PF.X3DAudioListenerData.OrientTop.z = 0.0f ;
	SoundSysData.PF.X3DAudioListenerData.pCone = &SoundSysData.PF.X3DAudioListenerConeData ;

	// 終了
	return 0 ;
}


// サウンドシステムを初期化する関数の環境依存処理を行う関数( 実行箇所区別１ )
extern	int		InitializeSoundSystem_PF_Timing1( void )
{
#ifndef DX_NON_MULTITHREAD
	SETUP_WIN_API

	// ストリームサウンド再生用の処理を行うスレッドの作成
	SoundSysData.PF.StreamSoundThreadHandle = NULL ;
	if( WinData.ProcessorNum > 1 )
	{
		SoundSysData.PF.StreamSoundThreadHandle = WinAPIData.Win32Func.CreateThreadFunc( NULL, 0, StreamSoundThreadFunction, NULL, CREATE_SUSPENDED, &SoundSysData.PF.StreamSoundThreadID ) ;
		WinAPIData.Win32Func.SetThreadPriorityFunc( SoundSysData.PF.StreamSoundThreadHandle, THREAD_PRIORITY_TIME_CRITICAL ) ;
		WinAPIData.Win32Func.ResumeThreadFunc( SoundSysData.PF.StreamSoundThreadHandle ) ;
	}

#endif // DX_NON_MULTITHREAD
	
	if( SoundSysData.PF.EnableXAudioFlag )
	{
		DXST_LOGFILE_TABSUB ;
		DXST_LOGFILE_ADDUTF16LE( "\x58\x00\x41\x00\x75\x00\x64\x00\x69\x00\x6f\x00\x32\x00\x20\x00\x6e\x30\x1d\x52\x1f\x67\x16\x53\x6f\x30\x63\x6b\x38\x5e\x6b\x30\x42\x7d\x86\x4e\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"XAudio2 の初期化は正常に終了しました\n" @*/ ) ;
	}
	else
	{
		DXST_LOGFILE_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x53\x00\x6f\x00\x75\x00\x6e\x00\x64\x00\x20\x00\x6e\x30\x1d\x52\x1f\x67\x16\x53\x6f\x30\x63\x6b\x38\x5e\x6b\x30\x42\x7d\x86\x4e\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"DirectSound の初期化は正常に終了しました\n" @*/ ) ;
	}

	// 正常終了
	return 0 ;
}




// サウンドシステムの後始末をする関数の環境依存処理を行う関数( 実行箇所区別０ )
extern	int		TerminateSoundSystem_PF_Timing0( void )
{
	if( SoundSysData.PF.DirectSoundObject == NULL )
	{
		return -1 ;
	}

#ifndef DX_NON_MULTITHREAD
	SETUP_WIN_API

	// ストリームサウンド再生用スレッドを終了する
	if( SoundSysData.PF.StreamSoundThreadHandle != NULL )
	{
		SoundSysData.PF.StreamSoundThreadEndFlag = 1 ;
		while( NS_ProcessMessage() == 0 && SoundSysData.PF.StreamSoundThreadEndFlag == 1 ) WinAPIData.Win32Func.SleepFunc( 2 ) ;
	}

#endif // DX_NON_MULTITHREAD

	// 正常終了
	return 0 ;
}


// サウンドシステムの後始末をする関数の環境依存処理を行う関数( 実行箇所区別１ )
extern	int		TerminateSoundSystem_PF_Timing1( void )
{
	SETUP_WIN_API

	// テンポラリファイルの削除
	if( MidiSystemData.FileName[ 0 ] != L'\0' )
	{
		WinAPIData.Win32Func.DeleteFileWFunc( MidiSystemData.FileName ) ;
	}

	// 再生モードによって処理を分岐
	switch( SoundSysData.SoundMode )
	{
	case DX_MIDIMODE_MCI :
		// 通常の再生モードの時の処理

		// PlayWav関数で再生されているサウンドはないので-1をセットしておく
		SoundSysData.PlayWavSoundHandle = -1 ;

		break ;

	case DX_MIDIMODE_DIRECT_MUSIC_REVERB :
	case DX_MIDIMODE_DIRECT_MUSIC_NORMAL :
		// DirectMusic による演奏を行っていた場合の処理

		// すべてのサウンドの再生を止める
		if( SoundSysData.PF.DirectMusicPerformanceObject )
		{
			SoundSysData.PF.DirectMusicPerformanceObject->Stop( NULL , NULL , 0 , 0 ) ;
			SoundSysData.PF.DirectMusicPerformanceObject->CloseDown() ;
			SoundSysData.PF.DirectMusicPerformanceObject->Release() ;

			SoundSysData.PF.DirectMusicPerformanceObject = NULL ;
		}

		// ローダーを解放する
		if( SoundSysData.PF.DirectMusicLoaderObject )
		{
			SoundSysData.PF.DirectMusicLoaderObject->Release() ; 
			SoundSysData.PF.DirectMusicLoaderObject = NULL ;
		}

		break ;
	}

	// XAudio2 を使用しているかどうかで処理を分岐
	if( SoundSysData.PF.EnableXAudioFlag )
	{
		// マスタリングボイスの解放
		if( SoundSysData.PF.XAudio2MasteringVoiceObject != NULL )
		{
			SoundSysData.PF.XAudio2MasteringVoiceObject->DestroyVoice() ;
			SoundSysData.PF.XAudio2MasteringVoiceObject = NULL ;
		}
		if( SoundSysData.PF.XAudio2_8MasteringVoiceObject != NULL )
		{
			SoundSysData.PF.XAudio2_8MasteringVoiceObject->DestroyVoice() ;
			SoundSysData.PF.XAudio2_8MasteringVoiceObject = NULL ;
		}

		// XAudio2の解放
		if( SoundSysData.PF.XAudio2Object != NULL )
		{
			SoundSysData.PF.XAudio2Object->Release() ;
			SoundSysData.PF.XAudio2Object = NULL ;
		}

		// XAudio2.dllの後始末処理
		if( SoundSysData.PF.XAudio2_8DLL != NULL )
		{
			FreeLibrary( SoundSysData.PF.XAudio2_8DLL ) ;
			SoundSysData.PF.XAudio2_8DLL = NULL ;
			SoundSysData.PF.XAudio2CreateFunc = NULL ;
			SoundSysData.PF.CreateAudioVolumeMeterFunc = NULL ;
			SoundSysData.PF.CreateAudioReverbFunc = NULL ;
			SoundSysData.PF.X3DAudioInitializeFunc = NULL ;
			SoundSysData.PF.X3DAudioCalculateFunc = NULL ;
		}

		// X3DAudioの後始末処理
		if( SoundSysData.PF.X3DAudioDLL != NULL )
		{
			FreeLibrary( SoundSysData.PF.X3DAudioDLL ) ;
			SoundSysData.PF.X3DAudioDLL = NULL ;
			SoundSysData.PF.X3DAudioInitializeFunc = NULL ;
			SoundSysData.PF.X3DAudioCalculateFunc = NULL ;
		}
	}
	else
	{
		// プライマリサウンドバッファの再生停止、破棄
		if( SoundSysData.PF.PrimarySoundBuffer )
		{
			SoundSysData.PF.PrimarySoundBuffer->Stop() ;
			SoundSysData.PF.PrimarySoundBuffer->Release() ;
			SoundSysData.PF.PrimarySoundBuffer = NULL ;
		}

		// 無音バッファの再生停止、破棄
		if( SoundSysData.PF.NoSoundBuffer )
		{
			SoundSysData.PF.NoSoundBuffer->Stop() ;
			SoundSysData.PF.NoSoundBuffer->Release() ;
			SoundSysData.PF.NoSoundBuffer = NULL ;
		}

		// ＤｉｒｅｃｔＳｏｕｎｄオブジェクトの解放
		if( SoundSysData.PF.DirectSoundObject > (D_IDirectSound *)1 )
		{
			SoundSysData.PF.DirectSoundObject->Release() ;
		}
		SoundSysData.PF.DirectSoundObject = NULL ;


		DXST_LOGFILE_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x53\x00\x6f\x00\x75\x00\x6e\x00\x64\x00\x20\x00\x6e\x30\x42\x7d\x86\x4e\xe6\x51\x06\x74\x6f\x30\x63\x6b\x38\x5e\x6b\x30\x42\x7d\x86\x4e\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"DirectSound の終了処理は正常に終了しました\n" @*/ ) ;
	}

	// 正常終了
	return 0 ;
}

// サウンドシステムの初期化チェックの環境依存処理を行う関数( TRUE:初期化されている  FALSE:初期化されていない )
extern	int		CheckSoundSystem_Initialize_PF( void )
{
	return SoundSysData.PF.DirectSoundObject == NULL ? FALSE : TRUE ;
}

// ＭＩＤＩハンドルの後始末を行う関数の環境依存処理
extern	int		TerminateMidiHandle_PF( MIDIHANDLEDATA *MusicData )
{
	// DirectMusicSegment8 オブジェクトの解放
	if( MusicData->PF.DirectMusicSegmentObject != NULL )
	{
		// 音色データの解放
		MusicData->PF.DirectMusicSegmentObject->Unload( SoundSysData.PF.DirectMusicPerformanceObject ) ;

		// オブジェクトの解放
		SoundSysData.PF.DirectMusicLoaderObject->ReleaseObjectByUnknown( MusicData->PF.DirectMusicSegmentObject ) ;
		MusicData->PF.DirectMusicSegmentObject->Release() ;
		MusicData->PF.DirectMusicSegmentObject = NULL ;
	}

	// 正常終了
	return 0 ;
}

// プリセットの３Ｄサウンド用のリバーブパラメータを取得する処理の環境依存処理を行う関数
extern	int		Get3DPresetReverbParamSoundMem_PF( SOUND3D_REVERB_PARAM *ParamBuffer, int PresetNo /* DX_REVERB_PRESET_DEFAULT 等 */ )
{
	if( SoundSysData.PF.EnableXAudioFlag == FALSE )
		return -1 ;

	// パラメータをコピー
	if( ParamBuffer != NULL )
	{
		if( SoundSysData.PF.XAudio2_8DLL != NULL )
		{
			D_XAUDIO2FX_REVERB_PARAMETERS2_8 *Param2_8 ;

			Param2_8 = &SoundSysData.PF.XAudio2_8ReverbParameters[ PresetNo ] ;

			ParamBuffer->WetDryMix           = Param2_8->WetDryMix ;

			ParamBuffer->ReflectionsDelay    = Param2_8->ReflectionsDelay;
			ParamBuffer->ReverbDelay         = Param2_8->ReverbDelay ;
			ParamBuffer->RearDelay           = Param2_8->RearDelay ;

			ParamBuffer->PositionLeft        = Param2_8->PositionLeft ;
			ParamBuffer->PositionRight       = Param2_8->PositionRight ;
			ParamBuffer->PositionMatrixLeft  = Param2_8->PositionMatrixLeft ;
			ParamBuffer->PositionMatrixRight = Param2_8->PositionMatrixRight ;
			ParamBuffer->EarlyDiffusion      = Param2_8->EarlyDiffusion ;
			ParamBuffer->LateDiffusion       = Param2_8->LateDiffusion ;
			ParamBuffer->LowEQGain           = Param2_8->LowEQGain ;
			ParamBuffer->LowEQCutoff         = Param2_8->LowEQCutoff ;
			ParamBuffer->HighEQGain          = Param2_8->HighEQGain ;
			ParamBuffer->HighEQCutoff        = Param2_8->HighEQCutoff ;

			ParamBuffer->RoomFilterFreq      = Param2_8->RoomFilterFreq ;
			ParamBuffer->RoomFilterMain      = Param2_8->RoomFilterMain ;
			ParamBuffer->RoomFilterHF        = Param2_8->RoomFilterHF ;
			ParamBuffer->ReflectionsGain     = Param2_8->ReflectionsGain ;
			ParamBuffer->ReverbGain          = Param2_8->ReverbGain ;
			ParamBuffer->DecayTime           = Param2_8->DecayTime ;
			ParamBuffer->Density             = Param2_8->Density ;
			ParamBuffer->RoomSize            = Param2_8->RoomSize ;
		}
		else
		{
			D_XAUDIO2FX_REVERB_PARAMETERS *Param ;

			Param = &SoundSysData.PF.XAudio2ReverbParameters[ PresetNo ] ;

			ParamBuffer->WetDryMix           = Param->WetDryMix ;

			ParamBuffer->ReflectionsDelay    = Param->ReflectionsDelay;
			ParamBuffer->ReverbDelay         = Param->ReverbDelay ;
			ParamBuffer->RearDelay           = Param->RearDelay ;

			ParamBuffer->PositionLeft        = Param->PositionLeft ;
			ParamBuffer->PositionRight       = Param->PositionRight ;
			ParamBuffer->PositionMatrixLeft  = Param->PositionMatrixLeft ;
			ParamBuffer->PositionMatrixRight = Param->PositionMatrixRight ;
			ParamBuffer->EarlyDiffusion      = Param->EarlyDiffusion ;
			ParamBuffer->LateDiffusion       = Param->LateDiffusion ;
			ParamBuffer->LowEQGain           = Param->LowEQGain ;
			ParamBuffer->LowEQCutoff         = Param->LowEQCutoff ;
			ParamBuffer->HighEQGain          = Param->HighEQGain ;
			ParamBuffer->HighEQCutoff        = Param->HighEQCutoff ;

			ParamBuffer->RoomFilterFreq      = Param->RoomFilterFreq ;
			ParamBuffer->RoomFilterMain      = Param->RoomFilterMain ;
			ParamBuffer->RoomFilterHF        = Param->RoomFilterHF ;
			ParamBuffer->ReflectionsGain     = Param->ReflectionsGain ;
			ParamBuffer->ReverbGain          = Param->ReverbGain ;
			ParamBuffer->DecayTime           = Param->DecayTime ;
			ParamBuffer->Density             = Param->Density ;
			ParamBuffer->RoomSize            = Param->RoomSize ;
		}
	}

	return 0 ;
}

// ３Ｄサウンドのリスナーの位置とリスナーの前方位置とリスナーの上方向位置を設定する処理の環境依存処理を行う関数
extern int Set3DSoundListenerPosAndFrontPosAndUpVec_PF( VECTOR /*Position*/, VECTOR /*FrontPosition*/, VECTOR /*UpVector*/ )
{
	SoundSysData.PF.X3DAudioListenerData.Position.x = SoundSysData.ListenerInfo.Position.x ;
	SoundSysData.PF.X3DAudioListenerData.Position.y = SoundSysData.ListenerInfo.Position.y ;
	SoundSysData.PF.X3DAudioListenerData.Position.z = SoundSysData.ListenerInfo.Position.z ;

	SoundSysData.PF.X3DAudioListenerData.OrientFront.x = SoundSysData.ListenerInfo.FrontDirection.x ;
	SoundSysData.PF.X3DAudioListenerData.OrientFront.y = SoundSysData.ListenerInfo.FrontDirection.y ;
	SoundSysData.PF.X3DAudioListenerData.OrientFront.z = SoundSysData.ListenerInfo.FrontDirection.z ;

	SoundSysData.PF.X3DAudioListenerData.OrientTop.x = SoundSysData.ListenerInfo.UpDirection.x ;
	SoundSysData.PF.X3DAudioListenerData.OrientTop.y = SoundSysData.ListenerInfo.UpDirection.y ;
	SoundSysData.PF.X3DAudioListenerData.OrientTop.z = SoundSysData.ListenerInfo.UpDirection.z ;

	return 0 ;
}

// ３Ｄサウンドのリスナーの移動速度を設定する処理の環境依存処理を行う関数
extern int Set3DSoundListenerVelocity_PF( VECTOR Velocity )
{
	SoundSysData.PF.X3DAudioListenerData.Velocity.x = Velocity.x ;
	SoundSysData.PF.X3DAudioListenerData.Velocity.y = Velocity.y ;
	SoundSysData.PF.X3DAudioListenerData.Velocity.z = Velocity.z ;

	return 0 ;
}

// ３Ｄサウンドのリスナーの可聴角度範囲を設定する処理の環境依存処理を行う関数
extern int Set3DSoundListenerConeAngle_PF( float InnerAngle, float OuterAngle )
{
	SoundSysData.PF.X3DAudioListenerConeData.InnerAngle = InnerAngle ;
	SoundSysData.PF.X3DAudioListenerConeData.OuterAngle = OuterAngle ;

	return 0 ;
}

// ３Ｄサウンドのリスナーの可聴角度範囲の音量倍率を設定する処理の環境依存処理を行う関数
extern int Set3DSoundListenerConeVolume_PF( float InnerAngleVolume, float OuterAngleVolume )
{
	SoundSysData.PF.X3DAudioListenerConeData.InnerVolume = InnerAngleVolume ;
	SoundSysData.PF.X3DAudioListenerConeData.OuterVolume = OuterAngleVolume ;

	return 0 ;
}

// サウンドをリソースから読み込む
extern int NS_LoadSoundMemByResource( const TCHAR *ResourceName, const TCHAR *ResourceType, int BufferNum )
{
#ifdef UNICODE
	return LoadSoundMemByResource_WCHAR_T(
		ResourceName, ResourceType, BufferNum
	) ;
#else
	int Result = -1 ;

	int IntResourceName ;
	int IntResourceType ;
	TCHAR_TO_WCHAR_T_STRING_BEGIN( ResourceName )
	TCHAR_TO_WCHAR_T_STRING_BEGIN( ResourceType )

	IntResourceName = ( ( ( DWORD_PTR )ResourceName ) >> 16 ) == 0 ? TRUE : FALSE ;
	IntResourceType = ( ( ( DWORD_PTR )ResourceType ) >> 16 ) == 0 ? TRUE : FALSE ;

	if( IntResourceName )
	{
		UseResourceNameBuffer = ( wchar_t * )ResourceName ;
	}
	else
	{
		TCHAR_TO_WCHAR_T_STRING_SETUP( ResourceName, goto ERR )
	}

	if( IntResourceType )
	{
		UseResourceTypeBuffer = ( wchar_t * )ResourceType ;
	}
	else
	{
		TCHAR_TO_WCHAR_T_STRING_SETUP( ResourceType, goto ERR )
	}

	Result = LoadSoundMemByResource_WCHAR_T(
		UseResourceNameBuffer, UseResourceTypeBuffer, BufferNum
	) ;

ERR :

	if( IntResourceName == FALSE )
	{
		TCHAR_TO_WCHAR_T_STRING_END( ResourceName )
	}

	if( IntResourceType == FALSE )
	{
		TCHAR_TO_WCHAR_T_STRING_END( ResourceType )
	}

	return Result ;
#endif
}

// サウンドリソースからサウンドハンドルを作成する
extern int NS_LoadSoundMemByResourceWithStrLen( const TCHAR *ResourceName, size_t ResourceNameLength, const TCHAR *ResourceType, size_t ResourceTypeLength, int BufferNum )
{
	int Result = -1 ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_BEGIN( ResourceName )
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_BEGIN( ResourceType )
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_SETUP( ResourceName, ResourceNameLength, goto ERR )
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_SETUP( ResourceType, ResourceTypeLength, goto ERR )

	Result = NS_LoadSoundMemByResource( UseResourceNameBuffer, UseResourceTypeBuffer, BufferNum ) ;

ERR :

	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( ResourceName )
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( ResourceType )
	return Result ;
}

// サウンドをリソースから読み込む
extern int LoadSoundMemByResource_WCHAR_T( const wchar_t *ResourceName, const wchar_t *ResourceType, int BufferNum )
{
	void *Image ;
	int ImageSize ;

	// リソースの情報を取得
	if( GetResourceInfo_WCHAR_T( ResourceName, ResourceType, &Image, &ImageSize ) < 0 )
	{
		return -1 ;
	}

	// ハンドルの作成
	return NS_LoadSoundMemByMemImageBase( Image, ImageSize, BufferNum ) ;
}

// LoadMusicMemByResource の実処理関数
static int LoadMusicMemByResource_Static(
	int MusicHandle,
	const wchar_t *ResourceName,
	const wchar_t *ResourceType,
	int ASyncThread
)
{
	MIDIHANDLEDATA * MusicData ;
	void *Image ;
	int ImageSize ;

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

	// リソースの情報を取得
	if( GetResourceInfo_WCHAR_T( ResourceName, ResourceType, &Image, &ImageSize ) < 0 )
	{
		return -1 ;
	}

	// ハンドルのセットアップ
	return LoadMusicMemByMemImage_Static( MusicHandle, Image, ImageSize, ASyncThread ) ;
}

#ifndef DX_NON_ASYNCLOAD
// LoadMusicMemByResource の非同期読み込みスレッドから呼ばれる関数
static void LoadMusicMemByResource_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	int MusicHandle ;
	const wchar_t *ResourceName ;
	const wchar_t *ResourceType ;
	int Addr ;
	int Result ;

	Addr = 0 ;
	MusicHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	ResourceName = GetASyncLoadParamString( AParam->Data, &Addr ) ;
	ResourceType = GetASyncLoadParamString( AParam->Data, &Addr ) ;

	Result = LoadMusicMemByResource_Static( MusicHandle, ResourceName, ResourceType, TRUE ) ;

	DecASyncLoadCount( MusicHandle ) ;
	if( Result < 0 )
	{
		SubHandle( MusicHandle ) ;
	}
}
#endif // DX_NON_ASYNCLOAD

// LoadMusicMemByResource のグローバル変数にアクセスしないバージョン
extern int LoadMusicMemByResource_UseGParam(
	const wchar_t *ResourceName,
	const wchar_t *ResourceType,
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
		AddASyncLoadParamString( NULL, &Addr, ResourceName ) ;
		AddASyncLoadParamString( NULL, &Addr, ResourceType ) ;

		// メモリの確保
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// 処理に必要な情報をセット
		AParam->ProcessFunction = LoadMusicMemByResource_ASync ;
		Addr = 0 ;
		AddASyncLoadParamInt( AParam->Data, &Addr, MusicHandle ) ;
		AddASyncLoadParamString( AParam->Data, &Addr, ResourceName ) ;
		AddASyncLoadParamString( AParam->Data, &Addr, ResourceType ) ;

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
		if( LoadMusicMemByResource_Static( MusicHandle, ResourceName, ResourceType, FALSE ) < 0 )
			goto ERR ;
	}

	// 終了
	return MusicHandle ;

ERR :
	SubHandle( MusicHandle ) ;

	return -1 ;
}

// リソース上のＭＩＤＩファイルを読み込む
extern int NS_LoadMusicMemByResource( const TCHAR *ResourceName, const TCHAR *ResourceType )
{
#ifdef UNICODE
	return LoadMusicMemByResource_WCHAR_T(
		ResourceName, ResourceType
	) ;
#else
	int Result = -1 ;

	int IntResourceName ;
	int IntResourceType ;
	TCHAR_TO_WCHAR_T_STRING_BEGIN( ResourceName )
	TCHAR_TO_WCHAR_T_STRING_BEGIN( ResourceType )

	IntResourceName = ( ( ( DWORD_PTR )ResourceName ) >> 16 ) == 0 ? TRUE : FALSE ;
	IntResourceType = ( ( ( DWORD_PTR )ResourceType ) >> 16 ) == 0 ? TRUE : FALSE ;

	if( IntResourceName )
	{
		UseResourceNameBuffer = ( wchar_t * )ResourceName ;
	}
	else
	{
		TCHAR_TO_WCHAR_T_STRING_SETUP( ResourceName, goto ERR )
	}

	if( IntResourceType )
	{
		UseResourceTypeBuffer = ( wchar_t * )ResourceType ;
	}
	else
	{
		TCHAR_TO_WCHAR_T_STRING_SETUP( ResourceType, goto ERR )
	}

	Result = LoadMusicMemByResource_WCHAR_T(
		UseResourceNameBuffer, UseResourceTypeBuffer
	) ;

ERR :

	if( IntResourceName == FALSE )
	{
		TCHAR_TO_WCHAR_T_STRING_END( ResourceName )
	}

	if( IntResourceType == FALSE )
	{
		TCHAR_TO_WCHAR_T_STRING_END( ResourceType )
	}

	return Result ;
#endif
}

// リソース上のＭＩＤＩファイルからＭＩＤＩハンドルを作成する
extern int NS_LoadMusicMemByResourceWithStrLen( const TCHAR *ResourceName, size_t ResourceNameLength, const TCHAR *ResourceType, size_t ResourceTypeLength )
{
	int Result = -1 ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_BEGIN( ResourceName )
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_BEGIN( ResourceType )
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_SETUP( ResourceName, ResourceNameLength, goto ERR )
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_SETUP( ResourceType, ResourceTypeLength, goto ERR )

	Result = NS_LoadMusicMemByResource( UseResourceNameBuffer, UseResourceTypeBuffer ) ;

ERR :

	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( ResourceName )
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( ResourceType )
	return Result ;
}

// リソース上のＭＩＤＩファイルを読み込む
extern int LoadMusicMemByResource_WCHAR_T( const wchar_t *ResourceName, const wchar_t *ResourceType )
{
	return LoadMusicMemByResource_UseGParam( ResourceName, ResourceType, GetASyncLoadFlag() ) ;
}

// LoadMusicMemByMemImage の実処理関数の環境依存処理を行う関数
extern int LoadMusicMemByMemImage_Static_PF( MIDIHANDLEDATA *MusicData, int /*ASyncThread*/ )
{
	switch( SoundSysData.SoundMode )
	{
	case DX_MIDIMODE_MCI :
		break ;

	case DX_MIDIMODE_DIRECT_MUSIC_REVERB :
	case DX_MIDIMODE_DIRECT_MUSIC_NORMAL :
		// DirectMusic を使用する場合はメモリから DirectMusicSegment8 を作成する
		{
			D_DMUS_OBJECTDESC ObjDesc ;

			_MEMSET( &ObjDesc, 0, sizeof( ObjDesc ) ) ;
			ObjDesc.dwSize = sizeof( D_DMUS_OBJECTDESC ) ;
			ObjDesc.dwValidData = D_DMUS_OBJ_MEMORY | D_DMUS_OBJ_CLASS ;
			ObjDesc.guidClass = CLSID_DIRECTMUSICSEGMENT ;
			ObjDesc.pbMemData = (BYTE *)MusicData->DataImage ;
			ObjDesc.llMemLength = MusicData->DataSize ;

			SoundSysData.PF.DirectMusicLoaderObject->ClearCache( IID_IDIRECTMUSICSEGMENT8 );
//			if( SoundSysData.PF.DirectMusicLoaderObject->GetObject( &ObjDesc, IID_IDirectMusicSegment8, ( void ** )&MusicData->PF.DirectMusicSegmentObject ) != S_OK )
			if( SoundSysData.PF.DirectMusicLoaderObject->GetObject( &ObjDesc, IID_IDIRECTMUSICSEGMENT8, ( void ** )&MusicData->PF.DirectMusicSegmentObject ) != S_OK )
			{
				DXST_LOGFILE_ADDUTF16LE( "\xdf\x30\xe5\x30\xfc\x30\xb8\x30\xc3\x30\xaf\x30\xc7\x30\xfc\x30\xbf\x30\x4b\x30\x89\x30\x20\x00\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x4d\x00\x75\x00\x73\x00\x69\x00\x63\x00\x53\x00\x65\x00\x67\x00\x6d\x00\x65\x00\x6e\x00\x74\x00\x38\x00\x20\x00\x92\x30\xd6\x53\x97\x5f\x59\x30\x8b\x30\xe6\x51\x06\x74\x4c\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ミュージックデータから DirectMusicSegment8 を取得する処理が失敗しました\n" @*/ ) ;
				return -1 ;
			}

			// データ形式を MidiSystemData にセット
			MusicData->PF.DirectMusicSegmentObject->SetParam( GUID_STANDARDMIDIFILE, 0xFFFFFFFF, 0, 0, NULL);

			// 音色データのダウンロード
			MusicData->PF.DirectMusicSegmentObject->Download( SoundSysData.PF.DirectMusicPerformanceObject ) ;
		}
		break ;
	}

	return 0 ;
}

// リソースからＭＩＤＩファイルを読み込んで演奏する
extern int NS_PlayMusicByResource( const TCHAR *ResourceName, const TCHAR *ResourceType, int PlayType )
{
#ifdef UNICODE
	return PlayMusicByResource_WCHAR_T(
		ResourceName, ResourceType, PlayType
	) ;
#else
	int Result = -1 ;

	int IntResourceName ;
	int IntResourceType ;
	TCHAR_TO_WCHAR_T_STRING_BEGIN( ResourceName )
	TCHAR_TO_WCHAR_T_STRING_BEGIN( ResourceType )

	IntResourceName = ( ( ( DWORD_PTR )ResourceName ) >> 16 ) == 0 ? TRUE : FALSE ;
	IntResourceType = ( ( ( DWORD_PTR )ResourceType ) >> 16 ) == 0 ? TRUE : FALSE ;

	if( IntResourceName )
	{
		UseResourceNameBuffer = ( wchar_t * )ResourceName ;
	}
	else
	{
		TCHAR_TO_WCHAR_T_STRING_SETUP( ResourceName, goto ERR )
	}

	if( IntResourceType )
	{
		UseResourceTypeBuffer = ( wchar_t * )ResourceType ;
	}
	else
	{
		TCHAR_TO_WCHAR_T_STRING_SETUP( ResourceType, goto ERR )
	}

	Result = PlayMusicByResource_WCHAR_T(
		UseResourceNameBuffer, UseResourceTypeBuffer, PlayType
	) ;

ERR :

	if( IntResourceName == FALSE )
	{
		TCHAR_TO_WCHAR_T_STRING_END( ResourceName )
	}

	if( IntResourceType == FALSE )
	{
		TCHAR_TO_WCHAR_T_STRING_END( ResourceType )
	}

	return Result ;
#endif
}

// リソースからＭＩＤＩファイルを読み込んで演奏する
extern int NS_PlayMusicByResourceWithStrLen( const TCHAR *ResourceName, size_t ResourceNameLength, const TCHAR *ResourceType, size_t ResourceTypeLength, int PlayType )
{
	int Result = -1 ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_BEGIN( ResourceName )
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_BEGIN( ResourceType )
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_SETUP( ResourceName, ResourceNameLength, goto ERR )
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_SETUP( ResourceType, ResourceTypeLength, goto ERR )

	Result = NS_PlayMusicByResource( UseResourceNameBuffer, UseResourceTypeBuffer, PlayType ) ;

ERR :

	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( ResourceName )
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( ResourceType )
	return Result ;
}

// リソースからＭＩＤＩファイルを読み込んで演奏する
extern int PlayMusicByResource_WCHAR_T( const wchar_t *ResourceName, const wchar_t *ResourceType, int PlayType )
{
	void *Image ;
	int ImageSize ;

	// リソースの情報を取得
	if( GetResourceInfo_WCHAR_T( ResourceName, ResourceType, &Image, &ImageSize ) < 0 )
	{
		return -1 ;
	}

	// 演奏開始
	return NS_PlayMusicByMemImage( Image, ImageSize, PlayType ) ;
}

// 読み込んだＭＩＤＩデータの演奏を開始する処理の環境依存処理を行う関数
extern int PlayMusicMem_PF( MIDIHANDLEDATA *MusicData, int PlayType )
{
	SETUP_WIN_API

	switch( SoundSysData.SoundMode )
	{
	case DX_MIDIMODE_MCI :
		{
			int Result ;
			MCI_OPEN_PARMSW		mciOpenParms;
			MCI_PLAY_PARMS		mciPlayParms;
			MCI_STATUS_PARMS	mciStatusParms;
			HANDLE FileHandle ;
			DWORD WriteSize ;

			// 以前のテンポラリファイルが残っているかもしれないので、一応削除
			WinAPIData.Win32Func.DeleteFileWFunc( MidiSystemData.FileName ) ;

			// テンポラリファイルを開く
			FileHandle = CreateTemporaryFile( MidiSystemData.FileName, sizeof( MidiSystemData.FileName ) ) ;
			if( FileHandle == NULL ) return -1 ;

			// テンポラリファイルにデータを書き込む
			WinAPIData.Win32Func.WriteFileFunc( FileHandle, MusicData->DataImage, ( DWORD )MusicData->DataSize, &WriteSize, NULL ) ;
			WinAPIData.Win32Func.CloseHandleFunc( FileHandle ) ;

			// オープンステータスセット
			mciOpenParms.lpstrElementName = MidiSystemData.FileName ;
			mciOpenParms.lpstrDeviceType  = L"sequencer" ;

			// ＭＣＩのオープン
			Result = ( int )WinAPIData.Win32Func.mciSendCommandFunc( 0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, ( DWORD_PTR )( LPVOID )&mciOpenParms ) ;
			if( Result != 0 )
			{
				LOADSOUND_GPARAM GParam ;
				int OldHandle ;

	//			DXST_LOGFILE_ADDUTF16LE( L"ＭＣＩのオープンに失敗しました\n" ) ;

				// エラーが起きたら一時ファイルを削除
				WinAPIData.Win32Func.DeleteFileWFunc( MidiSystemData.FileName ) ;

				// 演奏に失敗したら普通のサウンドファイルの可能性がある
				OldHandle = MidiSystemData.DefaultHandle ;
				InitLoadSoundGParam( &GParam ) ;
				GParam.CreateSoundDataType = DX_SOUNDDATATYPE_MEMPRESS ;
				MidiSystemData.DefaultHandle = LoadSoundMemByMemImageBase_UseGParam( &GParam, TRUE, -1, MusicData->DataImage, MusicData->DataSize, 1, -1, 0 ) ;
				if( MidiSystemData.DefaultHandle == -1 )
				{
					// それでも失敗したらファイルがないということ
					MidiSystemData.DefaultHandle = 0;
					return -1 ;
				}

				// 音量設定
				NS_ChangeVolumeSoundMem( MusicData->Volume, MidiSystemData.DefaultHandle ) ;

				// 今までのハンドルは削除
				if( OldHandle != 0 )
				{
					if( MidiSystemData.DefaultHandleToSoundHandleFlag == TRUE )
					{
						NS_DeleteSoundMem( OldHandle ) ;
					}
					else
					{
						NS_DeleteMusicMem( OldHandle ) ;
					}
					OldHandle = 0 ;
				}
				MidiSystemData.DefaultHandleToSoundHandleFlag = TRUE ;

				// 再生開始
				NS_PlaySoundMem( MidiSystemData.DefaultHandle, PlayType ) ;
				return 0 ;
			}

			// ＭＩＤＩのデバイスＩＤを保存
			MidiSystemData.PF.MidiDeviceID = mciOpenParms.wDeviceID ;

			// ＭＩＤＩマッパーか判定
			mciStatusParms.dwItem = MCI_SEQ_STATUS_PORT ;
			if( WinAPIData.Win32Func.mciSendCommandFunc( MidiSystemData.PF.MidiDeviceID , MCI_STATUS , MCI_STATUS_ITEM , ( DWORD_PTR )( LPVOID )&mciStatusParms ) )
			{
				WinAPIData.Win32Func.mciSendCommandFunc( MidiSystemData.PF.MidiDeviceID , MCI_CLOSE , 0 , 0 ) ;
				DXST_LOGFILE_ADDUTF16LE( "\x2d\xff\x29\xff\x24\xff\x29\xff\x14\x6f\x4f\x59\x8b\x95\xcb\x59\xe6\x51\x06\x74\x67\x30\xa8\x30\xe9\x30\xfc\x30\x4c\x30\x77\x8d\x4d\x30\x7e\x30\x57\x30\x5f\x30\x11\xff\x0a\x00\x00"/*@ L"ＭＩＤＩ演奏開始処理でエラーが起きました１\n" @*/ ) ;
				goto MCI_ERROR ;
			}
			if( LOWORD( mciStatusParms.dwReturn ) != LOWORD( MIDI_MAPPER ) )
			{
				WinAPIData.Win32Func.mciSendCommandFunc( MidiSystemData.PF.MidiDeviceID , MCI_CLOSE , 0 , 0 ) ;
				DXST_LOGFILE_ADDUTF16LE( "\x2d\xff\x29\xff\x24\xff\x29\xff\x14\x6f\x4f\x59\x8b\x95\xcb\x59\xe6\x51\x06\x74\x67\x30\xa8\x30\xe9\x30\xfc\x30\x4c\x30\x77\x8d\x4d\x30\x7e\x30\x57\x30\x5f\x30\x12\xff\x0a\x00\x00"/*@ L"ＭＩＤＩ演奏開始処理でエラーが起きました２\n" @*/ ) ;
				goto MCI_ERROR ;
			}

			// コールバック対象をメインウインドウに設定して演奏開始
			mciPlayParms.dwCallback = ( DWORD_PTR )NS_GetMainWindowHandle() ;
			if( WinAPIData.Win32Func.mciSendCommandFunc( MidiSystemData.PF.MidiDeviceID , MCI_PLAY , MCI_NOTIFY , ( DWORD_PTR )( LPVOID )&mciPlayParms ) )
			{
				WinAPIData.Win32Func.mciSendCommandFunc( MidiSystemData.PF.MidiDeviceID , MCI_CLOSE , 0 , 0 ) ;
				DXST_LOGFILE_ADDUTF16LE( "\x2d\xff\x29\xff\x24\xff\x29\xff\x14\x6f\x4f\x59\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ＭＩＤＩ演奏に失敗しました\n" @*/ ) ;
				goto MCI_ERROR ;
			}

			// テンポラリファイルから再生していることを示すフラグを立てる
			MidiSystemData.MemImagePlayFlag = TRUE ;
		}
		break ;

	case DX_MIDIMODE_DIRECT_MUSIC_REVERB :
	case DX_MIDIMODE_DIRECT_MUSIC_NORMAL :
		{
			D_REFERENCE_TIME RTime ;
			HRESULT hr ;

			// ループ設定
			MusicData->PF.DirectMusicSegmentObject->SetRepeats( PlayType == DX_PLAYTYPE_LOOP ? D_DMUS_SEG_REPEAT_INFINITE : 0 ) ;

			// 演奏開始
			hr = SoundSysData.PF.DirectMusicPerformanceObject->PlaySegmentEx( 
				MusicData->PF.DirectMusicSegmentObject,	// 演奏するセグメント。
				NULL,									// ソングに使用するパラメータ。実装されていない。
				NULL,									// トランジションに関するパラメータ。
				D_DMUS_SEGF_REFTIME,					// フラグ。
				0,										// 開始タイム。0は直ちに開始。
				NULL,									// セグメント状態を受け取るポインタ。
				NULL,									// 停止するオブジェクト。
				NULL									// デフォルトでない場合はオーディオパス。
			) ;

			// 演奏開始時間の保存
			SoundSysData.PF.DirectMusicPerformanceObject->GetTime( &RTime, NULL ) ;
			MusicData->StartTime = _DTOL( (double)RTime / 10000.0 ) ;
		}
		// テンポラリファイルから再生していることを示すフラグを倒す
		MidiSystemData.MemImagePlayFlag = FALSE ;
		break ;
	}

	return 0 ;

MCI_ERROR:
	// エラーが起きたら一時ファイルを削除して終了
	WinAPIData.Win32Func.DeleteFileWFunc( MidiSystemData.FileName ) ;

	// エラー終了
	return -1 ;
}

// ＭＩＤＩデータの演奏を停止する処理の環境依存処理を行う
extern int StopMusicMem_PF( MIDIHANDLEDATA *MusicData )
{
	int i ;

	SETUP_WIN_API

	switch( SoundSysData.SoundMode )
	{
	case DX_MIDIMODE_MCI :
		// 演奏終了関数を呼ぶ
		MidiCallBackProcess() ;

		// ウエイト
		for( i = 0 ; i <= 4 ; i++ ) NS_ProcessMessage() ;
		break ;

	case DX_MIDIMODE_DIRECT_MUSIC_REVERB :
	case DX_MIDIMODE_DIRECT_MUSIC_NORMAL :
		// 演奏を停止する
		SoundSysData.PF.DirectMusicPerformanceObject->StopEx( MusicData->PF.DirectMusicSegmentObject, 0, 0 ) ;
		break ;
	}

	// テンポラリファイルから再生されていた場合は削除する
	if( MidiSystemData.MemImagePlayFlag == TRUE )
	{
		WinAPIData.Win32Func.DeleteFileWFunc( MidiSystemData.FileName ) ;
		MidiSystemData.MemImagePlayFlag = FALSE ;
	}

	return 0 ;
}

// ＭＩＤＩデータが演奏中かどうかを取得する( TRUE:演奏中  FALSE:停止中 )処理の環境依存処理を行う関数
extern int CheckMusicMem_PF( MIDIHANDLEDATA *MusicData )
{
	int Result = -1 ;

	switch( SoundSysData.SoundMode )
	{
	case DX_MIDIMODE_MCI :
		Result = MidiSystemData.PlayFlag ;
		break ;

	case DX_MIDIMODE_DIRECT_MUSIC_REVERB :
	case DX_MIDIMODE_DIRECT_MUSIC_NORMAL :
		Result = MusicData->PlayFlag ;
		break ;
	}

	return Result ;
}

// ＭＩＤＩデータの周期的処理の環境依存処理を行う関数
extern int ProcessMusicMem_PF( MIDIHANDLEDATA *MusicData )
{
	int play ;

	switch( SoundSysData.SoundMode )
	{
	case DX_MIDIMODE_MCI :
		break ;

	case DX_MIDIMODE_DIRECT_MUSIC_REVERB :
	case DX_MIDIMODE_DIRECT_MUSIC_NORMAL :
		play = SoundSysData.PF.DirectMusicPerformanceObject->IsPlaying( MusicData->PF.DirectMusicSegmentObject , NULL ) != S_FALSE ;
		if( MusicData->PlayStartFlag == FALSE )
		{
			// まだ演奏が始まっていなかった場合は状態が演奏中になったことで
			// 初めて演奏中ということになる
			if( play == TRUE ) MusicData->PlayStartFlag = TRUE ;
		}
		else
		{
			// 演奏が始まったあとは現在の状態がそのまま反映される
			MusicData->PlayFlag = play ;
		}
		break ;
	}

	return 0 ;
}

// ＭＩＤＩデータの現在の再生位置を取得する処理の環境依存処理を行う関数
extern int GetMusicMemPosition_PF( MIDIHANDLEDATA *MusicData )
{
	int Result = -1 ;

	switch( SoundSysData.SoundMode )
	{
	case DX_MIDIMODE_MCI :
		Result = NS_GetMusicPosition() ;
		break ;

	case DX_MIDIMODE_DIRECT_MUSIC_REVERB :
	case DX_MIDIMODE_DIRECT_MUSIC_NORMAL :
		{
			D_REFERENCE_TIME RTime ;
			D_MUSIC_TIME Time ;
			D_IDirectMusicSegmentState *State ;

			SoundSysData.PF.DirectMusicPerformanceObject->GetTime( &RTime, &Time ) ;
			if( SoundSysData.PF.DirectMusicPerformanceObject->GetSegmentState( &State, Time ) != S_OK )
				return -1 ;

			State->GetSeek( &Time ) ;
			State->Release() ;

			// 時間を返す
			Result = _DTOL( (double)RTime / 10000.0 ) - MusicData->StartTime ;
		}
		break ;
	}

	return Result ;
}

// ＭＩＤＩの再生音量をセットする処理の環境依存処理を行う関数
extern int SetVolumeMusic_PF( int Volume )
{
	long V ;

	switch( SoundSysData.SoundMode )
	{
	case DX_MIDIMODE_MCI :
		// ＭＣＩの場合は音量の変更は出来ない
		break ;

	case DX_MIDIMODE_DIRECT_MUSIC_REVERB :
	case DX_MIDIMODE_DIRECT_MUSIC_NORMAL :
		// 音量のセット
		V = _DTOL( ( double )( SoundSysData.MaxVolume - DM_MIN_VOLUME ) / 256 * Volume ) + DM_MIN_VOLUME ;
		if( Volume == 255 ) V = SoundSysData.MaxVolume ;
		SoundSysData.PF.DirectMusicPerformanceObject->SetGlobalParam( GUID_PERFMASTERVOLUME , &V , sizeof( long ) ) ;
		
		break ;
	}

	return 0 ;
}

// ＭＩＤＩの現在の再生位置を取得する処理の環境依存処理を行う関数
extern int GetMusicPosition_PF( void )
{
	SETUP_WIN_API

	switch( SoundSysData.SoundMode )
	{
	case DX_MIDIMODE_MCI :
		{
			MCI_SET_PARMS mciSetParms ;
			MCI_STATUS_PARMS mciStatusParms ;

			// 取得する時間の単位をミリ秒単位にする
			_MEMSET( &mciSetParms, 0, sizeof( mciSetParms ) ) ;
			mciSetParms.dwTimeFormat = MCI_FORMAT_MILLISECONDS ;
			if( WinAPIData.Win32Func.mciSendCommandFunc( MidiSystemData.PF.MidiDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD_PTR)&mciSetParms ) != 0 )
				return -1 ;

			// 時間を取得する
			_MEMSET( &mciStatusParms, 0, sizeof( mciStatusParms ) ) ;
			mciStatusParms.dwItem = MCI_STATUS_POSITION ;
			if( WinAPIData.Win32Func.mciSendCommandFunc( MidiSystemData.PF.MidiDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)&mciStatusParms ) != 0 )
				return -1 ;

			// 時間を返す
			return ( int )mciStatusParms.dwReturn ;
		}
		break ;

	case DX_MIDIMODE_DIRECT_MUSIC_REVERB :
	case DX_MIDIMODE_DIRECT_MUSIC_NORMAL :
		{
			D_REFERENCE_TIME RTime ;
			D_MUSIC_TIME /*StartTime,*/ Time ;
			D_IDirectMusicSegmentState *State ;

			SoundSysData.PF.DirectMusicPerformanceObject->GetTime( &RTime, &Time ) ;
			if( SoundSysData.PF.DirectMusicPerformanceObject->GetSegmentState( &State, Time ) != S_OK )
				return -1 ;

			State->GetSeek( &Time ) ;
			State->Release() ;

//			SoundSysData.PF.DirectMusicPerformanceObject->MusicToReferenceTime( Time, &RTime ) ;

			// 時間を返す
//			return (int)Time ;
			return _DTOL( (double)RTime / 10000.0 ) - MidiSystemData.StartTime ;
		}
		break ;
	}

	return -1 ;
}


















void __stdcall SOUNDBUFFER_CALLBACK::OnVoiceProcessingPassStart( DWORD BytesRequired )
{
	int CompPos = Buffer->CompPos ;
	int Loop = Buffer->Loop ;
	D_XAUDIO2_BUFFER XBuffer = { 0 } ;
	DWORD MoveData ;
	DWORD MaxSendSamples ;
	void *SampleBuffer ;
	int BlockAlign ;

	MaxSendSamples = Buffer->Format.nSamplesPerSec / 60 ;
	SampleBuffer = Buffer->Wave->DoubleSizeBuffer ? Buffer->Wave->DoubleSizeBuffer : Buffer->Wave->Buffer ;
	BlockAlign = Buffer->Format.wBitsPerSample * Buffer->PF.XAudioChannels / 8 ;

	MoveData = 0 ;
	while( MoveData < BytesRequired )
	{
		if( CompPos >= Buffer->SampleNum )
		{
			if( Loop )
			{
				CompPos = 0 ;
				goto COPYDATA ;
			}
			else
			{
				SoundBuffer_Stop( Buffer ) ;
				break ;
			}
		}
		else
		{
			DWORD NowBytes ;
			DWORD AddSamples ;

COPYDATA :
			NowBytes           = ( DWORD )( CompPos * BlockAlign ) ;
			XBuffer.pAudioData = ( BYTE * )SampleBuffer + NowBytes ;
			AddSamples         = ( DWORD )( Buffer->SampleNum - CompPos ) ;
			if( AddSamples > MaxSendSamples )
			{
				AddSamples = MaxSendSamples ;
			}
			XBuffer.AudioBytes = AddSamples * BlockAlign ;
			if( XBuffer.AudioBytes > BytesRequired )
			{
				AddSamples = BytesRequired / BlockAlign ;
				XBuffer.AudioBytes = AddSamples * BlockAlign ;
			}
			CompPos += AddSamples ;
			MoveData += XBuffer.AudioBytes ;

			XBuffer.pContext = ( void * )Buffer ;

			if( SoundSysData.PF.XAudio2_8DLL != NULL )
			{
				Buffer->PF.XA2_8SourceVoice->SubmitSourceBuffer( &XBuffer ) ;
			}
			else
			{
				Buffer->PF.XA2SourceVoice->SubmitSourceBuffer( &XBuffer ) ;
			}
		}
	}
	Buffer->CompPos = CompPos ;
}

void __stdcall SOUNDBUFFER_CALLBACK::OnVoiceProcessingPassEnd()
{
}

void __stdcall SOUNDBUFFER_CALLBACK::OnStreamEnd()
{
}

void __stdcall SOUNDBUFFER_CALLBACK::OnBufferStart( void* /*pBufferContext*/ )
{
}

void __stdcall SOUNDBUFFER_CALLBACK::OnBufferEnd( void* pBufferContext )
{
	SOUNDBUFFER *SBuffer = ( SOUNDBUFFER * )pBufferContext ;

	SBuffer->Pos = SBuffer->CompPos ;
}

void __stdcall SOUNDBUFFER_CALLBACK::OnLoopEnd( void* /*pBufferContext*/ )
{
}

void __stdcall SOUNDBUFFER_CALLBACK::OnVoiceError( void* /*pBufferContext*/, HRESULT /*Error*/ )
{
}











extern int SoundBuffer_Initialize_Timing0_PF( SOUNDBUFFER *Buffer, DWORD Bytes, WAVEFORMATEX *Format, SOUNDBUFFER *Src, int Is3DSound )
{
	int i ;
	HRESULT hr ;
	WAVEFORMATEX waveformat ;
	int UseDoubleSizeBuffer = FALSE ;

	Buffer->PF.DSBuffer			= NULL ;
	Buffer->PF.XA2SourceVoice	= NULL ;
	Buffer->PF.XA2_8SourceVoice	= NULL ;
	Buffer->PF.XA2SubmixVoice	= NULL ;
	Buffer->PF.XA2_8SubmixVoice	= NULL ;
	Buffer->PF.XA2ReverbEffect	= NULL ;
	Buffer->PF.XA2Callback		= NULL ;

	if( Src != NULL )
	{
		waveformat = Src->Format ;
		Bytes      = Src->SampleNum * Src->Format.nBlockAlign ;
	}
	else
	{
		waveformat = *Format ;
	}

	if( SoundSysData.PF.EnableXAudioFlag )
	{
		// ３Ｄサウンドかどうかで分岐
		if( Is3DSound != FALSE )
		{
			D_XAUDIO2_EFFECT_DESCRIPTOR effects[ 1 ] ;
			D_XAUDIO2_EFFECT_CHAIN effectChain ;

			// ３Ｄサウンドの場合はサブミックスボイスも作成する
			hr = D_XAudio2CreateReverb( &Buffer->PF.XA2ReverbEffect, 0 ) ;
			if( FAILED( hr ) )
				goto ERR ;

			effects[ 0 ].pEffect = Buffer->PF.XA2ReverbEffect ;
			effects[ 0 ].InitialState = TRUE ;
			effects[ 0 ].OutputChannels = waveformat.nChannels ;

			effectChain.EffectCount = 1 ;
			effectChain.pEffectDescriptors = effects ;

			if( SoundSysData.PF.XAudio2_8DLL != NULL )
			{
				if( FAILED( SoundSysData.PF.XAudio2_8Object->CreateSubmixVoice(
					&Buffer->PF.XA2_8SubmixVoice,
					waveformat.nChannels,
					SoundSysData.OutputSmaplesPerSec,
					0,
					0,
					NULL,
					&effectChain ) ) )
					goto ERR ;
				_MEMCPY( &Buffer->PF.XAudio2_8ReverbParameter, &SoundSysData.PF.XAudio2_8ReverbParameters[ D_XAUDIO2FX_PRESET_DEFAULT ], sizeof( SoundSysData.PF.XAudio2_8ReverbParameters[ D_XAUDIO2FX_PRESET_DEFAULT ] ) ) ;
				Buffer->PF.XA2_8SubmixVoice->SetEffectParameters( 0, &Buffer->PF.XAudio2_8ReverbParameter, sizeof( Buffer->PF.XAudio2_8ReverbParameter ) ) ;
				Buffer->PF.XA2_8SubmixVoice->DisableEffect( 0 ) ;
			}
			else
			{
				if( FAILED( SoundSysData.PF.XAudio2Object->CreateSubmixVoice(
					&Buffer->PF.XA2SubmixVoice,
					waveformat.nChannels,
					SoundSysData.OutputSmaplesPerSec,
					0,
					0,
					NULL,
					&effectChain ) ) )
					goto ERR ;
				_MEMCPY( &Buffer->PF.XAudio2ReverbParameter, &SoundSysData.PF.XAudio2ReverbParameters[ D_XAUDIO2FX_PRESET_DEFAULT ], sizeof( SoundSysData.PF.XAudio2ReverbParameters[ D_XAUDIO2FX_PRESET_DEFAULT ] ) ) ;
				Buffer->PF.XA2SubmixVoice->SetEffectParameters( 0, &Buffer->PF.XAudio2ReverbParameter, sizeof( Buffer->PF.XAudio2ReverbParameter ) ) ;
				Buffer->PF.XA2SubmixVoice->DisableEffect( 0 ) ;
			}
		}
		else
		{
			// モノラルサウンドでも３Ｄサウンドではない場合はパンのためにステレオに変更
			if( waveformat.nChannels == 1 )
			{
				waveformat.nChannels = 2 ;
				waveformat.nBlockAlign = ( WORD )( waveformat.nChannels * waveformat.wBitsPerSample / 8 ) ;
				waveformat.nAvgBytesPerSec = waveformat.nBlockAlign * waveformat.nSamplesPerSec ;
				UseDoubleSizeBuffer = TRUE ;
			}
		}
		Buffer->PF.XAudioChannels = waveformat.nChannels ;

		if( Src != NULL )
		{
			Buffer->Wave = DuplicateWaveData( Src->Wave ) ;
		}
		else
		{
			Buffer->Wave = AllocWaveData( ( int )Bytes, UseDoubleSizeBuffer ) ;
		}
		if( Buffer->Wave == NULL )
			goto ERR ;

		Buffer->PF.XA2Callback = new SOUNDBUFFER_CALLBACK ;
		if( Buffer->PF.XA2Callback == NULL )
			goto ERR ;

		Buffer->PF.XA2Callback->Buffer = Buffer ;

		if( SoundSysData.PF.XAudio2_8DLL != NULL )
		{
			if( Is3DSound != FALSE )
			{
				D_XAUDIO2_VOICE_SENDS2_8 sendList ;
				D_XAUDIO2_SEND_DESCRIPTOR2_8 sendDescriptors[ 2 ] ;

				sendDescriptors[0].Flags = D_XAUDIO2_SEND_USEFILTER ;
				sendDescriptors[0].pOutputVoice = SoundSysData.PF.XAudio2_8MasteringVoiceObject ;
				sendDescriptors[1].Flags = D_XAUDIO2_SEND_USEFILTER ;
				sendDescriptors[1].pOutputVoice = Buffer->PF.XA2_8SubmixVoice ;
				sendList.SendCount = 2 ;
				sendList.pSends = sendDescriptors ;

				hr = SoundSysData.PF.XAudio2_8Object->CreateSourceVoice( &Buffer->PF.XA2_8SourceVoice, &waveformat, 0, D_XAUDIO2_DEFAULT_FREQ_RATIO, Buffer->PF.XA2Callback, &sendList ) ;
			}
			else
			{
				hr = SoundSysData.PF.XAudio2_8Object->CreateSourceVoice( &Buffer->PF.XA2_8SourceVoice, &waveformat, 0, D_XAUDIO2_DEFAULT_FREQ_RATIO, Buffer->PF.XA2Callback ) ;
			}
		}
		else
		{
			if( Is3DSound != FALSE )
			{
				D_XAUDIO2_VOICE_SENDS sendList ;
				D_XAUDIO2_SEND_DESCRIPTOR sendDescriptors[ 2 ] ;

				sendDescriptors[0].Flags = D_XAUDIO2_SEND_USEFILTER ;
				sendDescriptors[0].pOutputVoice = SoundSysData.PF.XAudio2MasteringVoiceObject ;
				sendDescriptors[1].Flags = D_XAUDIO2_SEND_USEFILTER ;
				sendDescriptors[1].pOutputVoice = Buffer->PF.XA2SubmixVoice ;
				sendList.SendCount = 2 ;
				sendList.pSends = sendDescriptors ;

				hr = SoundSysData.PF.XAudio2Object->CreateSourceVoice( &Buffer->PF.XA2SourceVoice, &waveformat, 0, D_XAUDIO2_DEFAULT_FREQ_RATIO, Buffer->PF.XA2Callback, &sendList ) ;
			}
			else
			{
				hr = SoundSysData.PF.XAudio2Object->CreateSourceVoice( &Buffer->PF.XA2SourceVoice, &waveformat, 0, D_XAUDIO2_DEFAULT_FREQ_RATIO, Buffer->PF.XA2Callback ) ;
			}
		}
		
		if( FAILED( hr ) )
			goto ERR ;

		if( Src != NULL )
		{
			for( i = 0 ; i < SOUNDBUFFER_MAX_CHANNEL_NUM ; i ++ )
			{
				SoundBuffer_SetVolume( Buffer, i, Src->Volume[ i ] ) ;
			}
			SoundBuffer_SetFrequency( Buffer, ( DWORD )Src->Frequency ) ;
			SoundBuffer_SetPan( Buffer, Src->Pan ) ;
		}
	}
	else
	{
		D_DSBUFFERDESC Desc ;

		_MEMSET( &Desc, 0, sizeof( Desc ) ) ;
		Desc.dwSize			= sizeof( Desc ) ;
		Desc.dwFlags		= ( DWORD )( D_DSBCAPS_GLOBALFOCUS | D_DSBCAPS_CTRLPAN | D_DSBCAPS_CTRLVOLUME | D_DSBCAPS_CTRLFREQUENCY | D_DSBCAPS_GETCURRENTPOSITION2 | ( SoundSysData.PF.UseSoftwareMixing ? D_DSBCAPS_LOCSOFTWARE : D_DSBCAPS_STATIC ) ) ;
		Desc.dwBufferBytes	= Bytes ;
		Desc.lpwfxFormat	= &waveformat ;

		if( SoundSysData.PF.DirectSoundObject->CreateSoundBuffer( &Desc, &Buffer->PF.DSBuffer, NULL ) != D_DS_OK )
		{
			// 作成に失敗した場合は、含める機能を減らして再度作成する
			_MEMSET( &Desc, 0, sizeof( Desc ) ) ;
			Desc.dwSize			= sizeof( Desc ) ;
			Desc.dwFlags		= ( DWORD )( D_DSBCAPS_CTRLPAN | D_DSBCAPS_CTRLVOLUME | ( SoundSysData.PF.UseSoftwareMixing ? D_DSBCAPS_LOCSOFTWARE : D_DSBCAPS_STATIC ) ) ;
			Desc.dwBufferBytes	= Bytes ;
			Desc.lpwfxFormat	= &waveformat ;
			if( SoundSysData.PF.DirectSoundObject->CreateSoundBuffer( &Desc, &Buffer->PF.DSBuffer, NULL ) != D_DS_OK )
			{
				goto ERR ;
			}
		}
	}
	
	return 0 ;

ERR :
	if( SoundSysData.PF.EnableXAudioFlag )
	{
		if( SoundSysData.PF.XAudio2_8DLL != NULL )
		{
			if( Buffer->PF.XA2_8SourceVoice != NULL )
			{
				Buffer->PF.XA2_8SourceVoice->DestroyVoice() ;
				Buffer->PF.XA2_8SourceVoice = NULL ;
			}

			if( Buffer->PF.XA2_8SubmixVoice != NULL )
			{
				Buffer->PF.XA2_8SubmixVoice->DestroyVoice() ;
				Buffer->PF.XA2_8SubmixVoice = NULL ;
			}
		}
		else
		{
			if( Buffer->PF.XA2SourceVoice != NULL )
			{
				Buffer->PF.XA2SourceVoice->DestroyVoice() ;
				Buffer->PF.XA2SourceVoice = NULL ;
			}

			if( Buffer->PF.XA2SubmixVoice != NULL )
			{
				Buffer->PF.XA2SubmixVoice->DestroyVoice() ;
				Buffer->PF.XA2SubmixVoice = NULL ;
			}
		}

		if( Buffer->PF.XA2Callback != NULL )
		{
			delete Buffer->PF.XA2Callback ;
			Buffer->PF.XA2Callback = NULL ;
		}
	}
	else
	{
		if( Buffer->PF.DSBuffer != NULL )
		{
			Buffer->PF.DSBuffer->Release() ;
			Buffer->PF.DSBuffer = NULL ;
		}
	}

	return -1 ;
}

extern int SoundBuffer_Initialize_Timing1_PF( SOUNDBUFFER *Buffer, SOUNDBUFFER *Src, int Is3DSound )
{
	int i ;

	// ３Ｄサウンドの場合は３Ｄサウンドパラメータを初期化
	if( Is3DSound != FALSE )
	{
		// エミッターの基本的な情報をセットする
		if( Src != NULL )
		{
			Buffer->PF.X3DAudioEmitterConeData              = Src->PF.X3DAudioEmitterConeData ;
			Buffer->PF.X3DAudioEmitterData                  = Src->PF.X3DAudioEmitterData ;
			Buffer->PF.X3DAudioEmitterData.pCone            = &Buffer->PF.X3DAudioEmitterConeData ;
			Buffer->PF.X3DAudioEmitterData.pChannelAzimuths = Buffer->PF.X3DAudioEmitterChannelAzimuths ;
		}
		else
		{
			Buffer->PF.X3DAudioEmitterData.pCone = &Buffer->PF.X3DAudioEmitterConeData ;
			Buffer->PF.X3DAudioEmitterData.pCone->InnerAngle = 0.0f ;
			Buffer->PF.X3DAudioEmitterData.pCone->OuterAngle = 0.0f ;
			Buffer->PF.X3DAudioEmitterData.pCone->InnerVolume = 0.0f ;
			Buffer->PF.X3DAudioEmitterData.pCone->OuterVolume = 1.0f ;
			Buffer->PF.X3DAudioEmitterData.pCone->InnerLPF = 0.0f ;
			Buffer->PF.X3DAudioEmitterData.pCone->OuterLPF = 1.0f ;
			Buffer->PF.X3DAudioEmitterData.pCone->InnerReverb = 0.0f ;
			Buffer->PF.X3DAudioEmitterData.pCone->OuterReverb = 1.0f ;

			Buffer->PF.X3DAudioEmitterData.Position.x = 0.0f ;
			Buffer->PF.X3DAudioEmitterData.Position.y = 0.0f ;
			Buffer->PF.X3DAudioEmitterData.Position.z = 0.0f ;
			Buffer->PF.X3DAudioEmitterData.OrientFront.x = 0.0f ;
			Buffer->PF.X3DAudioEmitterData.OrientFront.y = 0.0f ;
			Buffer->PF.X3DAudioEmitterData.OrientFront.z = 1.0f ;
			Buffer->PF.X3DAudioEmitterData.OrientTop.x = 0.0f ;
			Buffer->PF.X3DAudioEmitterData.OrientTop.y = 1.0f ;
			Buffer->PF.X3DAudioEmitterData.OrientTop.z = 0.0f ;
			Buffer->PF.X3DAudioEmitterData.Velocity.x = 0.0f ;
			Buffer->PF.X3DAudioEmitterData.Velocity.y = 0.0f ;
			Buffer->PF.X3DAudioEmitterData.Velocity.z = 0.0f ;
			Buffer->PF.X3DAudioEmitterData.ChannelRadius = 0.1f ;
			Buffer->PF.X3DAudioEmitterData.pChannelAzimuths = Buffer->PF.X3DAudioEmitterChannelAzimuths ;

			Buffer->PF.X3DAudioEmitterData.InnerRadius = 2.0f;
			Buffer->PF.X3DAudioEmitterData.InnerRadiusAngle = DX_PI_F / 4.0f ;

			Buffer->PF.X3DAudioEmitterData.pVolumeCurve = ( D_X3DAUDIO_DISTANCE_CURVE * )&D_X3DAudioDefault_LinearCurve ;
			Buffer->PF.X3DAudioEmitterData.pLFECurve    = ( D_X3DAUDIO_DISTANCE_CURVE * )&D_Emitter_LFE_Curve ;
			Buffer->PF.X3DAudioEmitterData.pLPFDirectCurve = NULL ;
			Buffer->PF.X3DAudioEmitterData.pLPFReverbCurve = NULL ;
			Buffer->PF.X3DAudioEmitterData.pReverbCurve    = ( D_X3DAUDIO_DISTANCE_CURVE * )&D_Emitter_Reverb_Curve ;
			Buffer->PF.X3DAudioEmitterData.CurveDistanceScaler = 14.0f ;
			Buffer->PF.X3DAudioEmitterData.DopplerScaler = 1.0f ;

			Buffer->EmitterInfo.InnerAngle = Buffer->PF.X3DAudioEmitterConeData.InnerAngle ;
			Buffer->EmitterInfo.OuterAngle = Buffer->PF.X3DAudioEmitterConeData.OuterAngle ;
			Buffer->EmitterInfo.InnerVolume = Buffer->PF.X3DAudioEmitterConeData.InnerVolume ;
			Buffer->EmitterInfo.OuterVolume = Buffer->PF.X3DAudioEmitterConeData.OuterVolume ;
			Buffer->EmitterRadius = Buffer->PF.X3DAudioEmitterData.CurveDistanceScaler ;
			Buffer->EmitterInnerRadius = Buffer->PF.X3DAudioEmitterData.InnerRadius ;

			// チャンネル数を保存する
			Buffer->PF.X3DAudioEmitterData.ChannelCount = Buffer->Format.nChannels/*D_X3DAUDIO_INPUTCHANNELS*/ ;
			for( i = 0 ; i < Buffer->Format.nChannels ; i ++ )
			{
				Buffer->PF.X3DAudioEmitterChannelAzimuths[ i ] = 0.0f ;
			}
		}
	}
	
	return 0 ;
}

extern int SoundBuffer_Terminate_PF( SOUNDBUFFER *Buffer )
{
	if( SoundSysData.PF.EnableXAudioFlag )
	{
		if( SoundSysData.PF.XAudio2_8DLL != NULL )
		{
			if( Buffer->PF.XA2_8SourceVoice != NULL )
			{
				Buffer->PF.XA2_8SourceVoice->DestroyVoice() ;
				Buffer->PF.XA2_8SourceVoice = NULL ;
			}

			if( Buffer->PF.XA2_8SubmixVoice != NULL )
			{
				Buffer->PF.XA2_8SubmixVoice->DestroyVoice() ;
				Buffer->PF.XA2_8SubmixVoice = NULL ;
			}
		}
		else
		{
			if( Buffer->PF.XA2SourceVoice != NULL )
			{
				Buffer->PF.XA2SourceVoice->DestroyVoice() ;
				Buffer->PF.XA2SourceVoice = NULL ;
			}

			if( Buffer->PF.XA2SubmixVoice != NULL )
			{
				Buffer->PF.XA2SubmixVoice->DestroyVoice() ;
				Buffer->PF.XA2SubmixVoice = NULL ;
			}
		}

		if( Buffer->PF.XA2Callback != NULL )
		{
			delete Buffer->PF.XA2Callback ;
			Buffer->PF.XA2Callback = NULL ;
		}
	}
	else
	{
		if( Buffer->PF.DSBuffer != NULL )
		{
			Buffer->PF.DSBuffer->Release() ;
			Buffer->PF.DSBuffer = NULL ;
		}
	}

	return 0 ;
}

extern int SoundBuffer_CheckEnable_PF( SOUNDBUFFER *Buffer )
{
	return Buffer->PF.DSBuffer != NULL ? TRUE : FALSE ;
}

extern int SoundBuffer_Play_PF( SOUNDBUFFER *Buffer, int Loop )
{
	if( SoundSysData.PF.EnableXAudioFlag )
	{
		Buffer->State = TRUE ;
		if( SoundSysData.PF.XAudio2_8DLL != NULL )
		{
			Buffer->PF.XA2_8SourceVoice->Start( 0 ) ;
			if( Buffer->PF.XA2_8SubmixVoice )
			{
				Buffer->PF.XA2_8SubmixVoice->EnableEffect( 0 ) ;
			}
		}
		else
		{
			Buffer->PF.XA2SourceVoice->Start( 0 ) ;
			if( Buffer->PF.XA2SubmixVoice )
			{
				Buffer->PF.XA2SubmixVoice->EnableEffect( 0 ) ;
			}
		}
		Buffer->StopTimeState = 1 ;
		Buffer->StopTime = 0 ;
	}
	else
	{
		if( Buffer->PF.DSBuffer->Play( 0, 0, ( DWORD )( Loop ? D_DSBPLAY_LOOPING : 0 ) ) != D_DS_OK )
		{
			return -1 ;
		}
	}
	
	return 0 ;
}

extern int SoundBuffer_Stop_PF(	SOUNDBUFFER *Buffer, int EffectStop )
{
	if( SoundSysData.PF.EnableXAudioFlag )
	{
		if( SoundSysData.PF.XAudio2_8DLL != NULL )
		{
			Buffer->PF.XA2_8SourceVoice->Stop( 0 ) ;
			if( Buffer->PF.XA2_8SubmixVoice && EffectStop )
			{
				Buffer->PF.XA2_8SubmixVoice->DisableEffect( 0 ) ;
			}
		}
		else
		{
			Buffer->PF.XA2SourceVoice->Stop( 0 ) ;
			if( Buffer->PF.XA2SubmixVoice && EffectStop )
			{
				Buffer->PF.XA2SubmixVoice->DisableEffect( 0 ) ;
			}
		}
		Buffer->State = FALSE ;
	}
	else
	{
		Buffer->PF.DSBuffer->Stop() ;
	}

	return 0 ;
}

extern int SoundBuffer_CheckPlay_PF( SOUNDBUFFER *Buffer )
{
	if( SoundSysData.PF.EnableXAudioFlag )
	{
		return Buffer->State ;
	}
	else
	{
		DWORD Status ;
		if( Buffer->PF.DSBuffer->GetStatus( &Status ) != D_DS_OK )
		{
			return -1 ;
		}

		if( Status & D_DSBSTATUS_PLAYING )
		{
			return TRUE ;
		}
		else
		{
			return FALSE ;
		}
	}
}

extern int SoundBuffer_Lock_PF( SOUNDBUFFER *Buffer, DWORD WritePos , DWORD WriteSize, void **LockPos1, DWORD *LockSize1, void **LockPos2, DWORD *LockSize2 )
{
	if( SoundSysData.PF.EnableXAudioFlag )
	{
		return 2 ;
	}
	else
	{
		if( Buffer->PF.DSBuffer->Lock( WritePos, WriteSize, LockPos1, LockSize1, LockPos2, LockSize2, 0 ) != D_DS_OK )
		{
			return -1 ;
		}
	}

	return 0 ;
}

extern int SoundBuffer_Unlock_PF( SOUNDBUFFER *Buffer, void *LockPos1, DWORD LockSize1, void *LockPos2, DWORD LockSize2 )
{
	if( SoundSysData.PF.EnableXAudioFlag )
	{
		DWORD i ;

		// ダブルサイズの場合はステレオデータにする
		if( Buffer->Wave->DoubleSizeBuffer != NULL )
		{
			switch( Buffer->Format.wBitsPerSample )
			{
			case 8 :
				{
					BYTE *Src8bit ;
					WORD *Dest8bit ;
					DWORD SampleNum ;

					Src8bit = ( BYTE * )LockPos1 ;
					Dest8bit = ( WORD * )Buffer->Wave->DoubleSizeBuffer + ( ( BYTE * )LockPos1 - ( BYTE * )Buffer->Wave->Buffer ) ;
					SampleNum = LockSize1 ;
					for( i = 0 ; i < SampleNum ; i ++ )
					{
						Dest8bit[ i ] = ( WORD )( Src8bit[ i ] + ( Src8bit[ i ] << 8 ) ) ;
					}

					Src8bit = ( BYTE * )LockPos2 ;
					Dest8bit = ( WORD * )Buffer->Wave->DoubleSizeBuffer + ( ( BYTE * )LockPos2 - ( BYTE * )Buffer->Wave->Buffer ) ;
					SampleNum = LockSize2 ;
					for( i = 0 ; i < SampleNum ; i ++ )
					{
						Dest8bit[ i ] = ( WORD )( Src8bit[ i ] + ( Src8bit[ i ] << 8 ) ) ;
					}
				}
				break ;

			case 16 :
				{
					WORD *Src16bit ;
					DWORD *Dest16bit ;
					DWORD SampleNum ;

					Src16bit = ( WORD * )LockPos1 ;
					Dest16bit = ( DWORD * )Buffer->Wave->DoubleSizeBuffer + ( ( WORD * )LockPos1 - ( WORD * )Buffer->Wave->Buffer ) ;
					SampleNum = LockSize1 / 2 ;
					for( i = 0 ; i < SampleNum ; i ++ )
					{
						Dest16bit[ i ] = ( DWORD )( Src16bit[ i ] + ( Src16bit[ i ] << 16 ) ) ;
					}

					Src16bit = ( WORD * )LockPos2 ;
					Dest16bit = ( DWORD * )Buffer->Wave->DoubleSizeBuffer + ( ( WORD * )LockPos2 - ( WORD * )Buffer->Wave->Buffer ) ;
					SampleNum = LockSize2 / 2 ;
					for( i = 0 ; i < SampleNum ; i ++ )
					{
						Dest16bit[ i ] = ( DWORD )( Src16bit[ i ] + ( Src16bit[ i ] << 16 ) ) ;
					}
				}
				break ;
			}
		}
	}
	else
	{
		if( Buffer->PF.DSBuffer->Unlock( LockPos1, LockSize1, LockPos2, LockSize2 ) != D_DS_OK )
		{
			return -1 ;
		}
	}

	return 0 ;
}

extern int SoundBuffer_SetFrequency_PF( SOUNDBUFFER *Buffer, DWORD Frequency )
{
	if( SoundSysData.PF.EnableXAudioFlag )
	{
		float SetFreq ;

		if( Frequency == D_DSBFREQUENCY_ORIGINAL )
		{
			SetFreq = 1.0f ;
		}
		else
		{
			SetFreq = ( float )Frequency / Buffer->Format.nSamplesPerSec ;
		}

		if( SoundSysData.PF.XAudio2_8DLL != NULL )
		{
			Buffer->PF.XA2_8SourceVoice->SetFrequencyRatio( SetFreq ) ;
		}
		else
		{
			Buffer->PF.XA2SourceVoice->SetFrequencyRatio( SetFreq ) ;
		}
	}
	else
	{
		Buffer->PF.DSBuffer->SetFrequency( Frequency ) ;
	}

	return 0 ;
}

extern int SoundBuffer_GetFrequency_PF( SOUNDBUFFER *Buffer, LPDWORD Frequency )
{
	if( SoundSysData.PF.EnableXAudioFlag )
	{
		return 2 ;
	}
	else
	{
		if( Buffer->PF.DSBuffer->GetFrequency( Frequency ) != D_DS_OK )
		{
			return -1 ;
		}
	}

	return 0 ;
}

extern int SoundBuffer_RefreshVolume_PF( SOUNDBUFFER *Buffer )
{
	int i ;
	int ChannelNum ;

	ChannelNum = Buffer->Format.nChannels > SOUNDBUFFER_MAX_CHANNEL_NUM ? SOUNDBUFFER_MAX_CHANNEL_NUM : Buffer->Format.nChannels ;

	if( SoundSysData.PF.EnableXAudioFlag )
	{
		float ChannelVolume[ 16 ] ;

		for( i = 0 ; i < SOUNDBUFFER_MAX_CHANNEL_NUM ; i ++ )
		{
			ChannelVolume[ i ] = D_XAudio2DecibelsToAmplitudeRatio( Buffer->Volume[ i ] / 100.0f ) ;
		}

		if( ChannelNum <= 2 )
		{
			if( ChannelNum == 1 )
			{
				ChannelVolume[ 1 ] = ChannelVolume[ 0 ] ;
			}

			if( Buffer->Pan < 0 )
			{
				ChannelVolume[ 1 ] *= D_XAudio2DecibelsToAmplitudeRatio( Buffer->Pan / 100.0f ) ;
			}
			else
			if( Buffer->Pan > 0 )
			{
				ChannelVolume[ 0 ] *= D_XAudio2DecibelsToAmplitudeRatio( -Buffer->Pan / 100.0f ) ;
			}
		}

		if( SoundSysData.PF.XAudio2_8DLL != NULL )
		{
			Buffer->PF.XA2_8SourceVoice->SetChannelVolumes( ( DWORD )Buffer->PF.XAudioChannels, ChannelVolume ) ;
		}
		else
		{
			Buffer->PF.XA2SourceVoice->SetChannelVolumes( ( DWORD )Buffer->PF.XAudioChannels, ChannelVolume ) ;
		}
	}
	else
	{
		if( ChannelNum > 2 )
		{
			LONG Volume ;

			Volume = ( LONG )Buffer->Volume[ 0 ] ;
			if( Volume > 0 )
			{
				Volume = 0 ;
			}
			else
			if( Volume < -10000 )
			{
				Volume = -10000 ;
			}
			Buffer->PF.DSBuffer->SetVolume( Volume ) ;
			Buffer->PF.DSBuffer->SetPan( Buffer->Pan ) ;
		}
		else
		{
			LONG CalcVolume[ 2 ] ;
			LONG TempVolume[ 2 ] ;
			FLOAT OrigVolume[ 2 ] ;
			LONG Volume ;
			LONG Pan ;

			if( Buffer->Is3DSound )
			{
				OrigVolume[ 0 ] = D_XAudio2DecibelsToAmplitudeRatio( Buffer->Volume[ 0 ] / 100.0f ) ;
				OrigVolume[ 1 ] = D_XAudio2DecibelsToAmplitudeRatio( Buffer->Volume[ 1 ] / 100.0f ) ;

				if( Buffer->DSound_Calc3DPan < 0.0f )
				{
					OrigVolume[ 0 ] *= 1.0f + Buffer->DSound_Calc3DPan ;
				}
				else
				if( Buffer->DSound_Calc3DPan > 0.0f )
				{
					OrigVolume[ 1 ] *= 1.0f - Buffer->DSound_Calc3DPan ;
				}

				TempVolume[ 0 ] = ( LONG )_DTOL( D_XAudio2AmplitudeRatioToDecibels( OrigVolume[ 0 ] * Buffer->DSound_Calc3DVolume ) * 100.0f ) ;
				TempVolume[ 1 ] = ( LONG )_DTOL( D_XAudio2AmplitudeRatioToDecibels( OrigVolume[ 1 ] * Buffer->DSound_Calc3DVolume ) * 100.0f ) ;
			}
			else
			{
				TempVolume[ 0 ] = Buffer->Volume[ 0 ] ;
				if( ChannelNum == 1 )
				{
					TempVolume[ 1 ] = Buffer->Volume[ 0 ] ;
				}
				else
				{
					TempVolume[ 1 ] = Buffer->Volume[ 1 ] ;
				}
			}

			if( Buffer->Pan < 0 )
			{
				CalcVolume[ 0 ] = 10000 ;
				CalcVolume[ 1 ] = 10000 + Buffer->Pan ;
			}
			else
			{
				CalcVolume[ 0 ] = 10000 - Buffer->Pan ;
				CalcVolume[ 1 ] = 10000 ;
			}

			if( TempVolume[ 0 ] > 0 )
			{
				TempVolume[ 0 ] = 0 ;
			}
			else
			if( TempVolume[ 0 ] < -10000 )
			{
				TempVolume[ 0 ] = -10000 ;
			}
			if( TempVolume[ 1 ] > 0 )
			{
				TempVolume[ 1 ] = 0 ;
			}
			else
			if( TempVolume[ 1 ] < -10000 )
			{
				TempVolume[ 1 ] = -10000 ;
			}

			CalcVolume[ 0 ] = CalcVolume[ 0 ] * ( TempVolume[ 0 ] + 10000 ) / 10000 ;
			CalcVolume[ 1 ] = CalcVolume[ 1 ] * ( TempVolume[ 0 ] + 10000 ) / 10000 ;

			if( CalcVolume[ 0 ] > CalcVolume[ 1 ] )
			{
				Volume = CalcVolume[ 0 ] - 10000 ;
				Pan =    _FTOL( CalcVolume[ 1 ] * ( 10000.0f / CalcVolume[ 0 ] ) ) - 10000 ;
			}
			else
			if( CalcVolume[ 0 ] < CalcVolume[ 1 ] )
			{
				Volume = CalcVolume[ 1 ] - 10000 ;
				Pan = -( _FTOL( CalcVolume[ 0 ] * ( 10000.0f / CalcVolume[ 1 ] ) ) - 10000 ) ;
			}
			else
			{
				Volume = CalcVolume[ 0 ] - 10000 ;
				Pan = 0 ;
			}

			Buffer->PF.DSBuffer->SetPan( ( LONG )Pan ) ;
			Buffer->PF.DSBuffer->SetVolume( ( LONG )Volume ) ;
		}
	}

	return 0 ;
}

extern int SoundBuffer_GetCurrentPosition_PF(	SOUNDBUFFER *Buffer, LPDWORD PlayPos, LPDWORD WritePos )
{
	if( SoundSysData.PF.EnableXAudioFlag )
	{
		return 2 ;
	}
	else
	{
		Buffer->PF.DSBuffer->GetCurrentPosition( PlayPos, WritePos ) ;
	}

	return 0 ;
}

extern int SoundBuffer_SetCurrentPosition_PF( SOUNDBUFFER *Buffer, DWORD NewPos )
{
	if( SoundSysData.PF.EnableXAudioFlag )
	{
		Buffer->Pos     = ( int )( NewPos / Buffer->Format.nBlockAlign ) ;
		Buffer->CompPos = Buffer->Pos ;

		if( SoundSysData.PF.XAudio2_8DLL != NULL )
		{
			Buffer->PF.XA2_8SourceVoice->FlushSourceBuffers() ;
		}
		else
		{
			Buffer->PF.XA2SourceVoice->FlushSourceBuffers() ;
		}
	}
	else
	{
		Buffer->PF.DSBuffer->SetCurrentPosition( NewPos ) ;
	}

	return 0 ;
}

extern int SoundBuffer_CycleProcess_PF( SOUNDBUFFER *Buffer )
{
	if( SoundSysData.PF.EnableXAudioFlag )
	{
		int NowCount ;
		int Time ;

		if( Buffer->PF.XA2SubmixVoice == NULL )
		{
			return -1 ;
		}

		if( Buffer->StopTimeState == 0 )
		{
			return -1 ;
		}

		if( Buffer->State == TRUE )
		{
			return 0 ;
		}

		if( Buffer->StopTimeState == 1 )
		{
			Buffer->StopTimeState = 2 ;
			Buffer->StopTime = NS_GetNowCount() ;

			return 0 ;
		}

		NowCount = NS_GetNowCount() ;

		// 再生が停止してから３秒後にエフェクトも停止する
		if( NowCount < Buffer->StopTime )
		{
			Time = 0x7ffffff - Buffer->StopTime + NowCount ;
		}
		else
		{
			Time = NowCount - Buffer->StopTime ;
		}
		if( Time < 3000 )
		{
			return 0 ;
		}

		Buffer->StopTimeState = 0 ;

		if( SoundSysData.PF.XAudio2_8DLL != NULL )
		{
			if( Buffer->PF.XA2_8SubmixVoice )
			{
				Buffer->PF.XA2_8SubmixVoice->DisableEffect( 0 ) ;
			}
		}
		else
		{
			if( Buffer->PF.XA2SubmixVoice )
			{
				Buffer->PF.XA2SubmixVoice->DisableEffect( 0 ) ;
			}
		}

		return -1 ;
	}

	return -1 ;
}

extern int SoundBuffer_Set3DPosition_PF( SOUNDBUFFER *Buffer, VECTOR *Position )
{
	// 値を保存
	Buffer->PF.X3DAudioEmitterData.Position.x = Position->x ;
	Buffer->PF.X3DAudioEmitterData.Position.y = Position->y ;
	Buffer->PF.X3DAudioEmitterData.Position.z = Position->z ;

	return 0 ;
}

extern int SoundBuffer_Set3DRadius_PF( SOUNDBUFFER *Buffer, float Radius )
{
	// 聞こえる距離を保存
	Buffer->PF.X3DAudioEmitterData.CurveDistanceScaler = Radius ;

	return 0 ;
}

extern int SoundBuffer_Set3DInnerRadius_PF(	SOUNDBUFFER *Buffer, float Radius )
{
	Buffer->PF.X3DAudioEmitterData.InnerRadius = Radius ;

	return 0 ;
}

extern int SoundBuffer_Set3DVelocity_PF( SOUNDBUFFER *Buffer, VECTOR *Velocity )
{
	// 値を保存
	Buffer->PF.X3DAudioEmitterData.Velocity.x = Velocity->x ;
	Buffer->PF.X3DAudioEmitterData.Velocity.y = Velocity->y ;
	Buffer->PF.X3DAudioEmitterData.Velocity.z = Velocity->z ;

	return 0 ;
}

extern int SoundBuffer_Set3DFrontPosition_PF( SOUNDBUFFER *Buffer, VECTOR * /*FrontPosition*/, VECTOR * /*UpVector*/ )
{
	// 値を保存
	Buffer->PF.X3DAudioEmitterData.OrientFront.x = Buffer->EmitterInfo.FrontDirection.x ;
	Buffer->PF.X3DAudioEmitterData.OrientFront.y = Buffer->EmitterInfo.FrontDirection.y ;
	Buffer->PF.X3DAudioEmitterData.OrientFront.z = Buffer->EmitterInfo.FrontDirection.z ;

	Buffer->PF.X3DAudioEmitterData.OrientTop.x = Buffer->EmitterInfo.UpDirection.x ;
	Buffer->PF.X3DAudioEmitterData.OrientTop.y = Buffer->EmitterInfo.UpDirection.y ;
	Buffer->PF.X3DAudioEmitterData.OrientTop.z = Buffer->EmitterInfo.UpDirection.z ;

	return 0 ;
}

extern int SoundBuffer_Set3DConeAngle_PF( SOUNDBUFFER *Buffer, float InnerAngle, float OuterAngle )
{
	Buffer->PF.X3DAudioEmitterConeData.InnerAngle = InnerAngle ;
	Buffer->PF.X3DAudioEmitterConeData.OuterAngle = OuterAngle ;

	return 0 ;
}

extern int SoundBuffer_Set3DConeVolume_PF( SOUNDBUFFER *Buffer, float InnerAngleVolume, float OuterAngleVolume )
{
	Buffer->PF.X3DAudioEmitterConeData.InnerVolume = InnerAngleVolume ;
	Buffer->PF.X3DAudioEmitterConeData.OuterVolume = OuterAngleVolume ;

	return 0 ;
}

extern int SoundBuffer_Refresh3DSoundParam_PF(	SOUNDBUFFER *Buffer, int /*AlwaysFlag*/ )
{
	DWORD CalcFlags ;
	D_X3DAUDIO_DSP_SETTINGS	DspSettings ;
	float MatrixCoefficients[ D_X3DAUDIO_INPUTCHANNELS * 32 ] ;
	D_XAUDIO2_FILTER_PARAMETERS FilterParametersDirect ;
	D_XAUDIO2_FILTER_PARAMETERS FilterParametersReverb ;
	float Sin, Cos ;
	int i ;
	int Num ;

	// XAudio2 を使用するかどうかで処理を分岐
	if( SoundSysData.PF.EnableXAudioFlag )
	{
		// XAudio2 を使用する場合
		CalcFlags =
			D_X3DAUDIO_CALCULATE_MATRIX |
			D_X3DAUDIO_CALCULATE_DOPPLER |
			D_X3DAUDIO_CALCULATE_LPF_DIRECT |
			D_X3DAUDIO_CALCULATE_LPF_REVERB |
			D_X3DAUDIO_CALCULATE_REVERB ;
		if( ( SoundSysData.PF.XAudio2OutputChannelMask & D_SPEAKER_LOW_FREQUENCY ) != 0 )
		{
			CalcFlags |= D_X3DAUDIO_CALCULATE_REDIRECT_TO_LFE ;
		}

		_MEMSET( MatrixCoefficients, 0, sizeof( MatrixCoefficients ) ) ;
		_MEMSET( &DspSettings, 0, sizeof( DspSettings ) ) ;
	//	DspSettings.SrcChannelCount = D_X3DAUDIO_INPUTCHANNELS ;
		DspSettings.SrcChannelCount = Buffer->Format.nChannels ;
		DspSettings.DstChannelCount = SoundSysData.OutputChannels ;
		DspSettings.pMatrixCoefficients = MatrixCoefficients ;

		SoundSysData.PF.X3DAudioCalculateFunc(
			SoundSysData.PF.X3DAudioInstance,
			&SoundSysData.PF.X3DAudioListenerData,
			&Buffer->PF.X3DAudioEmitterData,
			CalcFlags,
			&DspSettings ) ;

		_SINCOS( DX_PI_F / 6.0f * DspSettings.LPFDirectCoefficient, &Sin, &Cos ) ;
		FilterParametersDirect.Type = D_LowPassFilter ;
		FilterParametersDirect.Frequency = 2.0f * Sin ;
		FilterParametersDirect.OneOverQ = 1.0f ;

		_SINCOS( DX_PI_F / 6.0f * DspSettings.LPFReverbCoefficient, &Sin, &Cos ) ;
		FilterParametersReverb.Type = D_LowPassFilter ;
		FilterParametersReverb.Frequency = 2.0f * Sin ;
		FilterParametersReverb.OneOverQ = 1.0f ;
		if( SoundSysData.PF.XAudio2_8DLL != NULL )
		{
			Buffer->PF.XA2_8SourceVoice->SetFrequencyRatio( DspSettings.DopplerFactor ) ;

			Buffer->PF.XA2_8SourceVoice->SetOutputMatrix(
				SoundSysData.PF.XAudio2_8MasteringVoiceObject,
				Buffer->Format.nChannels,
				SoundSysData.OutputChannels,
				MatrixCoefficients
			) ;

			Num = Buffer->Format.nChannels * Buffer->Format.nChannels ;
			for( i = 0 ; i < Num ; i ++ )
			{
				MatrixCoefficients[ i ] = DspSettings.ReverbLevel ;
			}
			Buffer->PF.XA2_8SourceVoice->SetOutputMatrix(
				Buffer->PF.XA2_8SubmixVoice,
				Buffer->Format.nChannels,
				Buffer->Format.nChannels,
				MatrixCoefficients
			) ;

			Buffer->PF.XA2_8SourceVoice->SetOutputFilterParameters( SoundSysData.PF.XAudio2_8MasteringVoiceObject, &FilterParametersDirect ) ;
			Buffer->PF.XA2_8SourceVoice->SetOutputFilterParameters( Buffer->PF.XA2_8SubmixVoice,             &FilterParametersReverb ) ;
		}
		else
		{
			Buffer->PF.XA2SourceVoice->SetFrequencyRatio( DspSettings.DopplerFactor ) ;
			Buffer->PF.XA2SourceVoice->SetOutputMatrix(
				SoundSysData.PF.XAudio2MasteringVoiceObject,
				Buffer->Format.nChannels,
				SoundSysData.OutputChannels,
				MatrixCoefficients
			) ;

			Num = Buffer->Format.nChannels * Buffer->Format.nChannels ;
			for( i = 0 ; i < Num ; i ++ )
			{
				MatrixCoefficients[ i ] = DspSettings.ReverbLevel ;
			}
			Buffer->PF.XA2SourceVoice->SetOutputMatrix(
				Buffer->PF.XA2SubmixVoice,
				Buffer->Format.nChannels,
				Buffer->Format.nChannels,
				MatrixCoefficients
			) ;

			Buffer->PF.XA2SourceVoice->SetOutputFilterParameters( SoundSysData.PF.XAudio2MasteringVoiceObject, &FilterParametersDirect ) ;
			Buffer->PF.XA2SourceVoice->SetOutputFilterParameters( Buffer->PF.XA2SubmixVoice,             &FilterParametersReverb ) ;
		}
	}
	else
	{
		float Distance ;
		float Angle ;
		float DistanceVolumeRatio ;
		float AngleVolumeRatio ;
		float fVolume ;
		float fPan ;
		VECTOR ListenerToEmitterVec ;
		VECTOR PanVec ;

		// 距離での減衰率を計算
		ListenerToEmitterVec = VSub( Buffer->EmitterInfo.Position, SoundSysData.ListenerInfo.Position ) ;
		Distance = VSize( ListenerToEmitterVec ) ;
		if( Distance < 0.0000000000001f )
		{
			Distance = 0.0000000000001f ;
		}
		if( Distance > Buffer->EmitterRadius )
		{
			DistanceVolumeRatio = 0.0f ;
		}
		else
		{
			DistanceVolumeRatio = 1.0f - Distance / Buffer->EmitterRadius ;
		}
		ListenerToEmitterVec = VScale( ListenerToEmitterVec, 1.0f / Distance ) ;

		// 角度での減衰率を計算
		Angle = _ACOS( VDot( SoundSysData.ListenerInfo.FrontDirection, ListenerToEmitterVec ) ) * 2.0f ;
		if( Angle < SoundSysData.ListenerInfo.InnerAngle )
		{
			AngleVolumeRatio = SoundSysData.ListenerInfo.InnerVolume ;
		}
		else
		if( Angle > SoundSysData.ListenerInfo.OuterAngle )
		{
			AngleVolumeRatio = SoundSysData.ListenerInfo.OuterVolume ;
		}
		else
		{
			AngleVolumeRatio = ( Angle - SoundSysData.ListenerInfo.InnerAngle ) / ( SoundSysData.ListenerInfo.OuterAngle - SoundSysData.ListenerInfo.InnerAngle ) ;
			AngleVolumeRatio = ( SoundSysData.ListenerInfo.OuterVolume - SoundSysData.ListenerInfo.InnerVolume ) * AngleVolumeRatio + SoundSysData.ListenerInfo.InnerVolume ;
		}
		// 要素の掛け合わせ
		fVolume = DistanceVolumeRatio * AngleVolumeRatio ;

		// 左右バランスを計算
		PanVec.x = VDot( ListenerToEmitterVec, SoundSysData.ListenerSideDirection ) ;
		PanVec.y = VDot( ListenerToEmitterVec, SoundSysData.ListenerInfo.UpDirection ) ;
		PanVec.z = VDot( ListenerToEmitterVec, SoundSysData.ListenerInfo.FrontDirection ) ;
		fPan = PanVec.x < 0.0f ? -PanVec.x : PanVec.x ;
		if( fPan > 0.80f ) fPan = 0.80f ;
		if( PanVec.x < 0.0f )
		{
			fPan = -fPan ;
		}

		Buffer->DSound_Calc3DVolume = fVolume ;
		Buffer->DSound_Calc3DPan = fPan ;

		SoundBuffer_RefreshVolume( Buffer ) ;
	}

	// 終了
	return 0 ;
}

extern int SoundBuffer_SetReverbParam_PF( SOUNDBUFFER *Buffer, SOUND3D_REVERB_PARAM *Param )
{
	// XAudio2 を使用する場合のみ有効
	if( SoundSysData.PF.EnableXAudioFlag )
	{
		if( SoundSysData.PF.XAudio2_8DLL != NULL )
		{
			Buffer->PF.XAudio2_8ReverbParameter.WetDryMix           = Param->WetDryMix ;

			Buffer->PF.XAudio2_8ReverbParameter.ReflectionsDelay    = Param->ReflectionsDelay;
			Buffer->PF.XAudio2_8ReverbParameter.ReverbDelay         = Param->ReverbDelay ;
			Buffer->PF.XAudio2_8ReverbParameter.RearDelay           = Param->RearDelay ;

			Buffer->PF.XAudio2_8ReverbParameter.PositionLeft        = Param->PositionLeft ;
			Buffer->PF.XAudio2_8ReverbParameter.PositionRight       = Param->PositionRight ;
			Buffer->PF.XAudio2_8ReverbParameter.PositionMatrixLeft  = Param->PositionMatrixLeft ;
			Buffer->PF.XAudio2_8ReverbParameter.PositionMatrixRight = Param->PositionMatrixRight ;
			Buffer->PF.XAudio2_8ReverbParameter.EarlyDiffusion      = Param->EarlyDiffusion ;
			Buffer->PF.XAudio2_8ReverbParameter.LateDiffusion       = Param->LateDiffusion ;
			Buffer->PF.XAudio2_8ReverbParameter.LowEQGain           = Param->LowEQGain ;
			Buffer->PF.XAudio2_8ReverbParameter.LowEQCutoff         = Param->LowEQCutoff ;
			Buffer->PF.XAudio2_8ReverbParameter.HighEQGain          = Param->HighEQGain ;
			Buffer->PF.XAudio2_8ReverbParameter.HighEQCutoff        = Param->HighEQCutoff ;

			Buffer->PF.XAudio2_8ReverbParameter.RoomFilterFreq      = Param->RoomFilterFreq ;
			Buffer->PF.XAudio2_8ReverbParameter.RoomFilterMain      = Param->RoomFilterMain ;
			Buffer->PF.XAudio2_8ReverbParameter.RoomFilterHF        = Param->RoomFilterHF ;
			Buffer->PF.XAudio2_8ReverbParameter.ReflectionsGain     = Param->ReflectionsGain ;
			Buffer->PF.XAudio2_8ReverbParameter.ReverbGain          = Param->ReverbGain ;
			Buffer->PF.XAudio2_8ReverbParameter.DecayTime           = Param->DecayTime ;
			Buffer->PF.XAudio2_8ReverbParameter.Density             = Param->Density ;
			Buffer->PF.XAudio2_8ReverbParameter.RoomSize            = Param->RoomSize ;

			Buffer->PF.XAudio2_8ReverbParameter.DisableLateField    = FALSE ;

			Buffer->PF.XA2_8SubmixVoice->SetEffectParameters( 0, &Buffer->PF.XAudio2_8ReverbParameter, sizeof( Buffer->PF.XAudio2_8ReverbParameter ) ) ;
		}
		else
		{
			Buffer->PF.XAudio2ReverbParameter.WetDryMix           = Param->WetDryMix ;

			Buffer->PF.XAudio2ReverbParameter.ReflectionsDelay    = Param->ReflectionsDelay;
			Buffer->PF.XAudio2ReverbParameter.ReverbDelay         = Param->ReverbDelay ;
			Buffer->PF.XAudio2ReverbParameter.RearDelay           = Param->RearDelay ;

			Buffer->PF.XAudio2ReverbParameter.PositionLeft        = Param->PositionLeft ;
			Buffer->PF.XAudio2ReverbParameter.PositionRight       = Param->PositionRight ;
			Buffer->PF.XAudio2ReverbParameter.PositionMatrixLeft  = Param->PositionMatrixLeft ;
			Buffer->PF.XAudio2ReverbParameter.PositionMatrixRight = Param->PositionMatrixRight ;
			Buffer->PF.XAudio2ReverbParameter.EarlyDiffusion      = Param->EarlyDiffusion ;
			Buffer->PF.XAudio2ReverbParameter.LateDiffusion       = Param->LateDiffusion ;
			Buffer->PF.XAudio2ReverbParameter.LowEQGain           = Param->LowEQGain ;
			Buffer->PF.XAudio2ReverbParameter.LowEQCutoff         = Param->LowEQCutoff ;
			Buffer->PF.XAudio2ReverbParameter.HighEQGain          = Param->HighEQGain ;
			Buffer->PF.XAudio2ReverbParameter.HighEQCutoff        = Param->HighEQCutoff ;

			Buffer->PF.XAudio2ReverbParameter.RoomFilterFreq      = Param->RoomFilterFreq ;
			Buffer->PF.XAudio2ReverbParameter.RoomFilterMain      = Param->RoomFilterMain ;
			Buffer->PF.XAudio2ReverbParameter.RoomFilterHF        = Param->RoomFilterHF ;
			Buffer->PF.XAudio2ReverbParameter.ReflectionsGain     = Param->ReflectionsGain ;
			Buffer->PF.XAudio2ReverbParameter.ReverbGain          = Param->ReverbGain ;
			Buffer->PF.XAudio2ReverbParameter.DecayTime           = Param->DecayTime ;
			Buffer->PF.XAudio2ReverbParameter.Density             = Param->Density ;
			Buffer->PF.XAudio2ReverbParameter.RoomSize            = Param->RoomSize ;

			Buffer->PF.XA2SubmixVoice->SetEffectParameters( 0, &Buffer->PF.XAudio2ReverbParameter, sizeof( Buffer->PF.XAudio2ReverbParameter ) ) ;
		}
	}
	else
	{
	}

	return 0 ;
}

extern int SoundBuffer_SetPresetReverbParam_PF( SOUNDBUFFER *Buffer, int PresetNo )
{
	// XAudio2 を使用する場合のみ有効
	if( SoundSysData.PF.EnableXAudioFlag )
	{
		if( SoundSysData.PF.XAudio2_8DLL != NULL )
		{
			Buffer->PF.XAudio2_8ReverbParameter = SoundSysData.PF.XAudio2_8ReverbParameters[ PresetNo ] ;
			Buffer->PF.XA2_8SubmixVoice->SetEffectParameters( 0, &Buffer->PF.XAudio2_8ReverbParameter, sizeof( Buffer->PF.XAudio2_8ReverbParameter ) ) ;
		}
		else
		{
			Buffer->PF.XAudio2ReverbParameter = SoundSysData.PF.XAudio2ReverbParameters[ PresetNo ] ;
			Buffer->PF.XA2SubmixVoice->SetEffectParameters( 0, &Buffer->PF.XAudio2ReverbParameter, sizeof( Buffer->PF.XAudio2ReverbParameter ) ) ;
		}
	}
	else
	{
	}

	return 0 ;
}





















// ＭＩＤＩ演奏終了時呼ばれるコールバック関数
extern int MidiCallBackProcess( void )
{
	MCI_PLAY_PARMS		mciPlayParms;

	SETUP_WIN_API

	// ループ指定がある場合再び演奏を開始する
	if( MidiSystemData.LoopFlag == TRUE )
	{
		mciPlayParms.dwCallback = (DWORD_PTR) NS_GetMainWindowHandle() ;
		mciPlayParms.dwFrom		= 0 ;
		WinAPIData.Win32Func.mciSendCommandFunc( MidiSystemData.PF.MidiDeviceID , MCI_PLAY, MCI_NOTIFY | MCI_FROM, (DWORD_PTR)(LPVOID) &mciPlayParms );

		MidiSystemData.PlayFlag = TRUE ;
	}
	else 
	{
		// 演奏中だった場合は止める
		if( MidiSystemData.PlayFlag == TRUE )
		{
			WinAPIData.Win32Func.mciSendCommandFunc( MidiSystemData.PF.MidiDeviceID , MCI_CLOSE, 0, 0 );
			MidiSystemData.PlayFlag = FALSE;
		}
	}

	return 0 ;
}












// サウンドの処理をソフトウエアで行うかどうかを設定する( TRUE:ソフトウエア  FALSE:ハードウエア( デフォルト ) )
extern int NS_SetUseSoftwareMixingSoundFlag( int Flag )
{
	// フラグを保存
	SoundSysData.PF.UseSoftwareMixing = Flag ;

	// 終了
	return 0 ;
}

// サウンドの再生にXAudioを使用するかどうかを設定する( TRUE:使用する  FALSE:使用しない( デフォルト ) )
extern int NS_SetEnableXAudioFlag( int Flag )
{
	// 初期化済みの場合はエラー
	if( SoundSysData.InitializeFlag != FALSE )
		return -1 ;

	// フラグを保存する
	SoundSysData.PF.EnableXAudioFlag = Flag ;
	
	// 終了
	return 0 ;
}



// 情報取得系関数

// ＤＸライブラリが使用している DirectSound オブジェクトを取得する
extern const void *NS_GetDSoundObj( void )
{
	return SoundSysData.PF.DirectSoundObject ;
}







#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_NON_SOUND

