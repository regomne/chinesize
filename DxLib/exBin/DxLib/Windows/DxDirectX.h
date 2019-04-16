// ----------------------------------------------------------------------------
//
//		ＤＸライブラリ		DirectX 関連定義用ヘッダファイル
//
//				Ver 3.20c
//
// ----------------------------------------------------------------------------

// 多重インクルード防止用マクロ
#ifndef __DXDIRECTX_H__
#define __DXDIRECTX_H__

#include "../DxCompileConfig.h"
#ifdef __WINDOWS__
	#ifndef DX_GCC_COMPILE
	#include <mmreg.h>
	#endif
	#include <unknwn.h>
#endif

#ifndef DX_NON_NAMESPACE

//namespace DxLib
//{

#endif // DX_NON_NAMESPACE

typedef void *D_HMONITOR ;
typedef unsigned char D_UINT8 ;

struct D_PROPVARIANT
{
	WORD										vt ;
	WORD										wReserved1 ;
	WORD										wReserved2 ;
	WORD										wReserved3 ;
	LARGE_INTEGER								hVal ;
	BYTE										Padding[ 8 ] ;
} ;

// IUnknown, IPersist, IDispatch -----------------------------------------------------

class D_IUnknown
{
public:
	virtual HRESULT	__stdcall QueryInterface			( REFIID riid, void **ppvObject ) = 0 ;
	virtual ULONG	__stdcall AddRef					( void ) = 0 ;
	virtual ULONG	__stdcall Release					( void ) = 0 ;
} ;

class D_IPersist : public D_IUnknown
{
public:
	virtual HRESULT	__stdcall GetClassID				( CLSID *pClassID ) = 0 ;
} ;

class D_IDispatch : public D_IUnknown
{
public:
	virtual HRESULT	__stdcall GetTypeInfoCount			( UINT *pctinfo ) = 0 ;
	virtual HRESULT	__stdcall GetTypeInfo				( UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo ) = 0 ;
	virtual HRESULT	__stdcall GetIDsOfNames				( REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId ) = 0 ;
	virtual HRESULT	__stdcall Invoke					( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr ) = 0 ;
} ;

// ＤｉｒｅｃｔＳｏｕｎｄ -----------------------------------------------------

#define D_DS_OK									(S_OK)

#define D_DSBVOLUME_MIN							(-10000)
#define D_DSBVOLUME_MAX							(0)

#define D_DSSCL_NORMAL							(0x00000001)
#define D_DSSCL_PRIORITY						(0x00000002)
#define D_DSSCL_EXCLUSIVE						(0x00000003)
#define D_DSSCL_WRITEPRIMARY					(0x00000004)

#define D_DSBPLAY_LOOPING						(0x00000001)
#define D_DSBSTATUS_PLAYING						(0x00000001)
#define D_DSBSTATUS_BUFFERLOST					(0x00000002)
#define D_DSBSTATUS_LOOPING						(0x00000004)
#define D_DSBSTATUS_LOCHARDWARE					(0x00000008)
#define D_DSBSTATUS_LOCSOFTWARE					(0x00000010)
#define D_DSBSTATUS_TERMINATED					(0x00000020)
#define D_DSBFREQUENCY_ORIGINAL					(0)

#define D_DSBCAPS_PRIMARYBUFFER					(0x00000001)
#define D_DSBCAPS_STATIC						(0x00000002)
#define D_DSBCAPS_LOCHARDWARE					(0x00000004)
#define D_DSBCAPS_LOCSOFTWARE					(0x00000008)
#define D_DSBCAPS_CTRLFREQUENCY					(0x00000020)
#define D_DSBCAPS_CTRLPAN						(0x00000040)
#define D_DSBCAPS_CTRLVOLUME					(0x00000080)
#define D_DSBCAPS_GLOBALFOCUS					(0x00008000)
#define D_DSBCAPS_GETCURRENTPOSITION2			(0x00010000)

#define D_DSCAPS_PRIMARYMONO					(0x00000001)
#define D_DSCAPS_PRIMARYSTEREO					(0x00000002)
#define D_DSCAPS_PRIMARY8BIT					(0x00000004)
#define D_DSCAPS_PRIMARY16BIT					(0x00000008)
#define D_DSCAPS_SECONDARYMONO					(0x00000100)
#define D_DSCAPS_SECONDARYSTEREO				(0x00000200)
#define D_DSCAPS_SECONDARY8BIT					(0x00000400)
#define D_DSCAPS_SECONDARY16BIT					(0x00000800)

typedef struct tagD_DSCAPS
{
	DWORD										dwSize;
	DWORD										dwFlags;
	DWORD										dwMinSecondarySampleRate;
	DWORD										dwMaxSecondarySampleRate;
	DWORD										dwPrimaryBuffers;
	DWORD										dwMaxHwMixingAllBuffers;
	DWORD										dwMaxHwMixingStaticBuffers;
	DWORD										dwMaxHwMixingStreamingBuffers;
	DWORD										dwFreeHwMixingAllBuffers;
	DWORD										dwFreeHwMixingStaticBuffers;
	DWORD										dwFreeHwMixingStreamingBuffers;
	DWORD										dwMaxHw3DAllBuffers;
	DWORD										dwMaxHw3DStaticBuffers;
	DWORD										dwMaxHw3DStreamingBuffers;
	DWORD										dwFreeHw3DAllBuffers;
	DWORD										dwFreeHw3DStaticBuffers;
	DWORD										dwFreeHw3DStreamingBuffers;
	DWORD										dwTotalHwMemBytes;
	DWORD										dwFreeHwMemBytes;
	DWORD										dwMaxContigFreeHwMemBytes;
	DWORD										dwUnlockTransferRateHwBuffers;
	DWORD										dwPlayCpuOverheadSwBuffers;
	DWORD										dwReserved1;
	DWORD										dwReserved2;
} D_DSCAPS ;

typedef struct tagD_DSBUFFERDESC
{
	DWORD										dwSize;
	DWORD										dwFlags;
	DWORD										dwBufferBytes;
	DWORD										dwReserved;
	WAVEFORMATEX *								lpwfxFormat;

	GUID										guid3DAlgorithm;
} D_DSBUFFERDESC ;

typedef struct tagD_DSBPOSITIONNOTIFY
{
	DWORD										dwOffset;
	HANDLE										hEventNotify;
} D_DSBPOSITIONNOTIFY ;

typedef struct tagD_DSEFFECTDESC
{
	DWORD										dwSize;
	DWORD										dwFlags;
	GUID										guidDSFXClass;
	DWORD_PTR									dwReserved1;
	DWORD_PTR									dwReserved2;
} D_DSEFFECTDESC ;

typedef BOOL ( CALLBACK *LPD_DSENUMCALLBACKA )	( LPGUID, LPCSTR, LPCSTR, LPVOID ) ;
typedef BOOL ( CALLBACK *LPD_DSENUMCALLBACKW )	( LPGUID, LPCWSTR, LPCWSTR, LPVOID ) ;

class D_IDirectSound : public D_IUnknown
{
public :
	virtual HRESULT __stdcall CreateSoundBuffer			( const D_DSBUFFERDESC *pcDSBufferDesc, class D_IDirectSoundBuffer **ppDSBuffer, D_IUnknown *pUnkOuter ) = 0 ;
	virtual HRESULT __stdcall GetCaps					( D_DSCAPS *pDSCaps ) = 0 ;
	virtual HRESULT __stdcall DuplicateSoundBuffer		( class D_IDirectSoundBuffer *pDSBufferOriginal, class D_IDirectSoundBuffer **ppDSBufferDuplicate ) = 0 ;
	virtual HRESULT __stdcall SetCooperativeLevel		( HWND hwnd, DWORD dwLevel ) = 0 ;
	virtual HRESULT __stdcall NonUse01					( void ) = 0 ;				// Compact( void ) = 0 ;
	virtual HRESULT __stdcall NonUse02					( void ) = 0 ;				// GetSpeakerConfig( LPDWORD pdwSpeakerConfig ) = 0 ;
	virtual HRESULT __stdcall NonUse03					( void ) = 0 ;				// SetSpeakerConfig( DWORD dwSpeakerConfig ) = 0 ;
	virtual HRESULT __stdcall Initialize				( const GUID *pcGuidDevice ) = 0 ;
} ;

class D_IDirectSound8 : public D_IDirectSound
{
public :
	virtual HRESULT __stdcall NonUse00					( void ) = 0 ;				// VerifyCertification( LPDWORD pdwCertified ) = 0 ;
} ;

class D_IDirectSoundBuffer : public D_IUnknown
{
public :
	virtual HRESULT __stdcall GetCaps					( void ) = 0 ;				// GetCaps( LPDSBCAPS pDSBufferCaps ) = 0 ;
	virtual HRESULT __stdcall GetCurrentPosition		( DWORD *pdwCurrentPlayCursor, DWORD *pdwCurrentWriteCursor ) = 0 ;
	virtual HRESULT __stdcall NonUse00					( void ) = 0 ;				// GetFormat( LPWAVEFORMATEX pwfxFormat, DWORD dwSizeAllocated, LPDWORD pdwSizeWritten ) = 0 ;
	virtual HRESULT __stdcall GetVolume					( LPLONG plVolume ) = 0 ;
	virtual HRESULT __stdcall GetPan					( LPLONG plPan ) = 0 ;
	virtual HRESULT __stdcall GetFrequency				( LPDWORD pdwFrequency ) = 0 ;
	virtual HRESULT __stdcall GetStatus					( DWORD *pdwStatus ) = 0 ;
	virtual HRESULT __stdcall NonUse04					( void ) = 0 ;				// Initialize( LPDIRECTSOUND pDirectSound, LPCDSBUFFERDESC pcDSBufferDesc ) = 0 ;
	virtual HRESULT __stdcall Lock						( DWORD dwOffset, DWORD dwBytes, void **ppvAudioPtr1, DWORD *pdwAudioBytes1, void **ppvAudioPtr2, DWORD *pdwAudioBytes2, DWORD dwFlags ) = 0 ;
	virtual HRESULT __stdcall Play						( DWORD dwReserved1, DWORD dwPriority, DWORD dwFlags ) = 0 ;
	virtual HRESULT __stdcall SetCurrentPosition		( DWORD dwNewPosition ) = 0 ;
	virtual HRESULT __stdcall SetFormat					( const WAVEFORMATEX *pcfxFormat ) = 0 ;
	virtual HRESULT __stdcall SetVolume					( LONG lVolume ) = 0 ;
	virtual HRESULT __stdcall SetPan					( LONG lPan ) = 0 ;
	virtual HRESULT __stdcall SetFrequency				( DWORD dwFrequency ) = 0 ;
	virtual HRESULT __stdcall Stop						( void ) = 0 ;
	virtual HRESULT __stdcall Unlock					( void *pvAudioPtr1, DWORD dwAudioBytes1, void *pvAudioPtr2, DWORD dwAudioBytes2 ) = 0 ;
	virtual HRESULT __stdcall NonUse05					( void ) = 0 ;				// Restore( void ) = 0 ;
} ;

class D_IDirectSoundBuffer8 : public D_IDirectSoundBuffer
{
public :
	virtual HRESULT __stdcall SetFX						( DWORD dwEffectsCount, D_DSEFFECTDESC * pDSFXDesc, LPDWORD pdwResultCodes ) = 0 ;
	virtual HRESULT __stdcall NonUse01					( void ) = 0 ;				// AcquireResources( DWORD dwFlags, DWORD dwEffectsCount, LPDWORD pdwResultCodes ) = 0 ;
	virtual HRESULT __stdcall NonUse02					( void ) = 0 ;				// GetObjectInPath( REFGUID rguidObject, DWORD dwIndex, REFGUID rguidInterface, LPVOID *ppObject ) = 0 ;
} ;

class D_IDirectSoundNotify : public D_IUnknown
{
public :
	virtual HRESULT __stdcall SetNotificationPositions	( DWORD dwPositionNotifies, const D_DSBPOSITIONNOTIFY *pcPositionNotifies ) = 0 ;
} ;

// Ｘ３ＤＡｕｄｉｏ -----------------------------------------------------------

#if !defined(D__SPEAKER_POSITIONS_)
	#define D__SPEAKER_POSITIONS_
	#define D_SPEAKER_FRONT_LEFT				(0x00000001)
	#define D_SPEAKER_FRONT_RIGHT				(0x00000002)
	#define D_SPEAKER_FRONT_CENTER				(0x00000004)
	#define D_SPEAKER_LOW_FREQUENCY				(0x00000008)
	#define D_SPEAKER_BACK_LEFT					(0x00000010)
	#define D_SPEAKER_BACK_RIGHT				(0x00000020)
	#define D_SPEAKER_FRONT_LEFT_OF_CENTER		(0x00000040)
	#define D_SPEAKER_FRONT_RIGHT_OF_CENTER		(0x00000080)
	#define D_SPEAKER_BACK_CENTER				(0x00000100)
	#define D_SPEAKER_SIDE_LEFT					(0x00000200)
	#define D_SPEAKER_SIDE_RIGHT				(0x00000400)
	#define D_SPEAKER_TOP_CENTER				(0x00000800)
	#define D_SPEAKER_TOP_FRONT_LEFT			(0x00001000)
	#define D_SPEAKER_TOP_FRONT_CENTER			(0x00002000)
	#define D_SPEAKER_TOP_FRONT_RIGHT			(0x00004000)
	#define D_SPEAKER_TOP_BACK_LEFT				(0x00008000)
	#define D_SPEAKER_TOP_BACK_CENTER			(0x00010000)
	#define D_SPEAKER_TOP_BACK_RIGHT			(0x00020000)
	#define D_SPEAKER_RESERVED					(0x7FFC0000)
	#define D_SPEAKER_ALL						(0x80000000)
#endif

#if !defined(D_SPEAKER_MONO)
	#define D_SPEAKER_MONO						(D_SPEAKER_FRONT_CENTER)
	#define D_SPEAKER_STEREO					(D_SPEAKER_FRONT_LEFT | D_SPEAKER_FRONT_RIGHT)
	#define D_SPEAKER_2POINT1					(D_SPEAKER_FRONT_LEFT | D_SPEAKER_FRONT_RIGHT | D_SPEAKER_LOW_FREQUENCY)
	#define D_SPEAKER_SURROUND					(D_SPEAKER_FRONT_LEFT | D_SPEAKER_FRONT_RIGHT | D_SPEAKER_FRONT_CENTER | D_SPEAKER_BACK_CENTER)
	#define D_SPEAKER_QUAD						(D_SPEAKER_FRONT_LEFT | D_SPEAKER_FRONT_RIGHT | D_SPEAKER_BACK_LEFT | D_SPEAKER_BACK_RIGHT)
	#define D_SPEAKER_4POINT1					(D_SPEAKER_FRONT_LEFT | D_SPEAKER_FRONT_RIGHT | D_SPEAKER_LOW_FREQUENCY | D_SPEAKER_BACK_LEFT | D_SPEAKER_BACK_RIGHT)
	#define D_SPEAKER_5POINT1					(D_SPEAKER_FRONT_LEFT | D_SPEAKER_FRONT_RIGHT | D_SPEAKER_FRONT_CENTER | D_SPEAKER_LOW_FREQUENCY | D_SPEAKER_BACK_LEFT | D_SPEAKER_BACK_RIGHT)
	#define D_SPEAKER_7POINT1					(D_SPEAKER_FRONT_LEFT | D_SPEAKER_FRONT_RIGHT | D_SPEAKER_FRONT_CENTER | D_SPEAKER_LOW_FREQUENCY | D_SPEAKER_BACK_LEFT | D_SPEAKER_BACK_RIGHT | D_SPEAKER_FRONT_LEFT_OF_CENTER | D_SPEAKER_FRONT_RIGHT_OF_CENTER)
	#define D_SPEAKER_5POINT1_SURROUND			(D_SPEAKER_FRONT_LEFT | D_SPEAKER_FRONT_RIGHT | D_SPEAKER_FRONT_CENTER | D_SPEAKER_LOW_FREQUENCY | D_SPEAKER_SIDE_LEFT | D_SPEAKER_SIDE_RIGHT)
	#define D_SPEAKER_7POINT1_SURROUND			(D_SPEAKER_FRONT_LEFT | D_SPEAKER_FRONT_RIGHT | D_SPEAKER_FRONT_CENTER | D_SPEAKER_LOW_FREQUENCY | D_SPEAKER_BACK_LEFT | D_SPEAKER_BACK_RIGHT | D_SPEAKER_SIDE_LEFT | D_SPEAKER_SIDE_RIGHT)
#endif

#define D_X3DAUDIO_CALCULATE_MATRIX				(0x00000001)
#define D_X3DAUDIO_CALCULATE_DELAY				(0x00000002)
#define D_X3DAUDIO_CALCULATE_LPF_DIRECT			(0x00000004)
#define D_X3DAUDIO_CALCULATE_LPF_REVERB			(0x00000008)
#define D_X3DAUDIO_CALCULATE_REVERB				(0x00000010)
#define D_X3DAUDIO_CALCULATE_DOPPLER			(0x00000020)
#define D_X3DAUDIO_CALCULATE_EMITTER_ANGLE		(0x00000040)
#define D_X3DAUDIO_CALCULATE_ZEROCENTER			(0x00010000)
#define D_X3DAUDIO_CALCULATE_REDIRECT_TO_LFE	(0x00020000)

#define D_X3DAUDIO_SPEED_OF_SOUND				(343.5f)

typedef struct tagD_X3DAUDIO_VECTOR
{
	float										x ;
	float			  							y ;
	float										z ;
} D_X3DAUDIO_VECTOR ;

#ifndef DX_GCC_COMPILE
#pragma pack(push)
#pragma pack(1)
#else
#pragma pack(push,1)
#endif

#define D_X3DAUDIO_HANDLE_BYTESIZE				(20)

typedef BYTE D_X3DAUDIO_HANDLE[ D_X3DAUDIO_HANDLE_BYTESIZE ] ;

typedef struct tagD_X3DAUDIO_CONE
{
	float										InnerAngle ;
	float										OuterAngle ;
	float										InnerVolume ;
	float										OuterVolume ;
	float										InnerLPF ;
	float										OuterLPF ;
	float										InnerReverb ;
	float										OuterReverb ;
} D_X3DAUDIO_CONE ;

typedef struct tagD_X3DAUDIO_DISTANCE_CURVE_POINT
{
	float										Distance ;
	float										DSPSetting ;
} D_X3DAUDIO_DISTANCE_CURVE_POINT ;

typedef struct tagD_X3DAUDIO_DISTANCE_CURVE
{
	D_X3DAUDIO_DISTANCE_CURVE_POINT*			pPoints ;
	DWORD										PointCount ;
} D_X3DAUDIO_DISTANCE_CURVE ;

typedef struct tagD_X3DAUDIO_EMITTER
{
	D_X3DAUDIO_CONE*							pCone ;
	D_X3DAUDIO_VECTOR							OrientFront ;
	D_X3DAUDIO_VECTOR							OrientTop ;

	D_X3DAUDIO_VECTOR							Position ;
	D_X3DAUDIO_VECTOR							Velocity ;

	float										InnerRadius ;
	float										InnerRadiusAngle ;

	DWORD										ChannelCount ;
	float										ChannelRadius ;
	float*										pChannelAzimuths ;

	D_X3DAUDIO_DISTANCE_CURVE*					pVolumeCurve ;
	D_X3DAUDIO_DISTANCE_CURVE*					pLFECurve ;
	D_X3DAUDIO_DISTANCE_CURVE*					pLPFDirectCurve ;
	D_X3DAUDIO_DISTANCE_CURVE*					pLPFReverbCurve ;
	D_X3DAUDIO_DISTANCE_CURVE*					pReverbCurve ;

	float										CurveDistanceScaler ;
	float										DopplerScaler ;
} D_X3DAUDIO_EMITTER ;

typedef struct tagD_X3DAUDIO_LISTENER
{
	D_X3DAUDIO_VECTOR							OrientFront ;
	D_X3DAUDIO_VECTOR							OrientTop ;

	D_X3DAUDIO_VECTOR							Position ;
	D_X3DAUDIO_VECTOR							Velocity ;

	D_X3DAUDIO_CONE*							pCone ;
} D_X3DAUDIO_LISTENER ;

typedef struct tagD_X3DAUDIO_DSP_SETTINGS
{
	float*										pMatrixCoefficients ;
	float*										pDelayTimes ;
	DWORD										SrcChannelCount ;
	DWORD										DstChannelCount ;

	float										LPFDirectCoefficient ;
	float										LPFReverbCoefficient ;
	float										ReverbLevel ;
	float										DopplerFactor ;
	float										EmitterToListenerAngle ;

	float										EmitterToListenerDistance ;
	float										EmitterVelocityComponent ;
	float										ListenerVelocityComponent ;
} D_X3DAUDIO_DSP_SETTINGS ;

enum D_XAUDIO2FX_PRESET
{
	D_XAUDIO2FX_PRESET_DEFAULT,
	D_XAUDIO2FX_PRESET_GENERIC,
	D_XAUDIO2FX_PRESET_PADDEDCELL,
	D_XAUDIO2FX_PRESET_ROOM,
	D_XAUDIO2FX_PRESET_BATHROOM,
	D_XAUDIO2FX_PRESET_LIVINGROOM,
	D_XAUDIO2FX_PRESET_STONEROOM,
	D_XAUDIO2FX_PRESET_AUDITORIUM,
	D_XAUDIO2FX_PRESET_CONCERTHALL,
	D_XAUDIO2FX_PRESET_CAVE,
	D_XAUDIO2FX_PRESET_ARENA,
	D_XAUDIO2FX_PRESET_HANGAR,
	D_XAUDIO2FX_PRESET_CARPETEDHALLWAY,
	D_XAUDIO2FX_PRESET_HALLWAY,
	D_XAUDIO2FX_PRESET_STONECORRIDOR,
	D_XAUDIO2FX_PRESET_ALLEY,
	D_XAUDIO2FX_PRESET_FOREST,
	D_XAUDIO2FX_PRESET_CITY,
	D_XAUDIO2FX_PRESET_MOUNTAINS,
	D_XAUDIO2FX_PRESET_QUARRY,
	D_XAUDIO2FX_PRESET_PLAIN,
	D_XAUDIO2FX_PRESET_PARKINGLOT,
	D_XAUDIO2FX_PRESET_SEWERPIPE,
	D_XAUDIO2FX_PRESET_UNDERWATER,
	D_XAUDIO2FX_PRESET_SMALLROOM,
	D_XAUDIO2FX_PRESET_MEDIUMROOM,
	D_XAUDIO2FX_PRESET_LARGEROOM,
	D_XAUDIO2FX_PRESET_MEDIUMHALL,
	D_XAUDIO2FX_PRESET_LARGEHALL,
	D_XAUDIO2FX_PRESET_PLATE,

	D_XAUDIO2FX_PRESET_NUM,
} ;

#define D_XAUDIO2FX_DEBUG								(1)

#define D_XAUDIO2FX_REVERB_MIN_FRAMERATE				(20000)
#define D_XAUDIO2FX_REVERB_MAX_FRAMERATE				(48000)

#define D_XAUDIO2FX_REVERB_MIN_WET_DRY_MIX				(0.0f)
#define D_XAUDIO2FX_REVERB_MIN_REFLECTIONS_DELAY		(0)
#define D_XAUDIO2FX_REVERB_MIN_REVERB_DELAY				(0)
#define D_XAUDIO2FX_REVERB_MIN_REAR_DELAY				(0)
#define D_XAUDIO2FX_REVERB_MIN_POSITION					(0)
#define D_XAUDIO2FX_REVERB_MIN_DIFFUSION				(0)
#define D_XAUDIO2FX_REVERB_MIN_LOW_EQ_GAIN				(0)
#define D_XAUDIO2FX_REVERB_MIN_LOW_EQ_CUTOFF			(0)
#define D_XAUDIO2FX_REVERB_MIN_HIGH_EQ_GAIN				(0)
#define D_XAUDIO2FX_REVERB_MIN_HIGH_EQ_CUTOFF			(0)
#define D_XAUDIO2FX_REVERB_MIN_ROOM_FILTER_FREQ			(20.0f)
#define D_XAUDIO2FX_REVERB_MIN_ROOM_FILTER_MAIN			(-100.0f)
#define D_XAUDIO2FX_REVERB_MIN_ROOM_FILTER_HF			(-100.0f)
#define D_XAUDIO2FX_REVERB_MIN_REFLECTIONS_GAIN			(-100.0f)
#define D_XAUDIO2FX_REVERB_MIN_REVERB_GAIN				(-100.0f)
#define D_XAUDIO2FX_REVERB_MIN_DECAY_TIME				(0.1f)
#define D_XAUDIO2FX_REVERB_MIN_DENSITY					(0.0f)
#define D_XAUDIO2FX_REVERB_MIN_ROOM_SIZE				(0.0f)

#define D_XAUDIO2FX_REVERB_MAX_WET_DRY_MIX				(100.0f)
#define D_XAUDIO2FX_REVERB_MAX_REFLECTIONS_DELAY		(300)
#define D_XAUDIO2FX_REVERB_MAX_REVERB_DELAY				(85)
#define D_XAUDIO2FX_REVERB_MAX_REAR_DELAY				(5)
#define D_XAUDIO2FX_REVERB_MAX_POSITION					(30)
#define D_XAUDIO2FX_REVERB_MAX_DIFFUSION				(15)
#define D_XAUDIO2FX_REVERB_MAX_LOW_EQ_GAIN				(12)
#define D_XAUDIO2FX_REVERB_MAX_LOW_EQ_CUTOFF			(9)
#define D_XAUDIO2FX_REVERB_MAX_HIGH_EQ_GAIN				(8)
#define D_XAUDIO2FX_REVERB_MAX_HIGH_EQ_CUTOFF			(14)
#define D_XAUDIO2FX_REVERB_MAX_ROOM_FILTER_FREQ			(20000.0f)
#define D_XAUDIO2FX_REVERB_MAX_ROOM_FILTER_MAIN			(0.0f)
#define D_XAUDIO2FX_REVERB_MAX_ROOM_FILTER_HF			(0.0f)
#define D_XAUDIO2FX_REVERB_MAX_REFLECTIONS_GAIN			(20.0f)
#define D_XAUDIO2FX_REVERB_MAX_REVERB_GAIN				(20.0f)
#define D_XAUDIO2FX_REVERB_MAX_DENSITY					(100.0f)
#define D_XAUDIO2FX_REVERB_MAX_ROOM_SIZE				(100.0f)

#define D_XAUDIO2FX_REVERB_DEFAULT_WET_DRY_MIX			(100.0f)
#define D_XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_DELAY	(5)
#define D_XAUDIO2FX_REVERB_DEFAULT_REVERB_DELAY			(5)
#define D_XAUDIO2FX_REVERB_DEFAULT_REAR_DELAY			(5)
#define D_XAUDIO2FX_REVERB_DEFAULT_POSITION				(6)
#define D_XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX		(27)
#define D_XAUDIO2FX_REVERB_DEFAULT_EARLY_DIFFUSION		(8)
#define D_XAUDIO2FX_REVERB_DEFAULT_LATE_DIFFUSION		(8)
#define D_XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_GAIN			(8)
#define D_XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_CUTOFF		(4)
#define D_XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_GAIN			(8)
#define D_XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_CUTOFF		(4)
#define D_XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_FREQ		(5000.0f)
#define D_XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_MAIN		(0.0f)
#define D_XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_HF		(0.0f)
#define D_XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_GAIN		(0.0f)
#define D_XAUDIO2FX_REVERB_DEFAULT_REVERB_GAIN			(0.0f)
#define D_XAUDIO2FX_REVERB_DEFAULT_DECAY_TIME			(1.0f)
#define D_XAUDIO2FX_REVERB_DEFAULT_DENSITY				(100.0f)
#define D_XAUDIO2FX_REVERB_DEFAULT_ROOM_SIZE			(100.0f)
#define D_XAUDIO2FX_REVERB_DEFAULT_DISABLE_LATE_FIELD	(FALSE)

typedef struct tagD_XAUDIO2FX_REVERB_PARAMETERS
{
	float										WetDryMix ;

	UINT32										ReflectionsDelay ;
	BYTE										ReverbDelay ;
	BYTE										RearDelay ;

	BYTE										PositionLeft ;
	BYTE										PositionRight ;
	BYTE										PositionMatrixLeft ;
	BYTE										PositionMatrixRight ;
	BYTE										EarlyDiffusion ;
	BYTE										LateDiffusion ;
	BYTE										LowEQGain ;
	BYTE										LowEQCutoff ;
	BYTE										HighEQGain ;
	BYTE										HighEQCutoff ;

	float										RoomFilterFreq ;
	float										RoomFilterMain ;
	float										RoomFilterHF ;
	float										ReflectionsGain ;
	float										ReverbGain ;
	float										DecayTime ;
	float										Density ;
	float										RoomSize ;
} D_XAUDIO2FX_REVERB_PARAMETERS;

typedef struct tagD_XAUDIO2FX_REVERB_PARAMETERS2_8
{
	float										WetDryMix ;

	UINT32										ReflectionsDelay ;
	BYTE										ReverbDelay ;
	BYTE										RearDelay ;

	BYTE										PositionLeft ;
	BYTE										PositionRight ;
	BYTE										PositionMatrixLeft ;
	BYTE										PositionMatrixRight ;
	BYTE										EarlyDiffusion ;
	BYTE										LateDiffusion ;
	BYTE										LowEQGain ;
	BYTE										LowEQCutoff ;
	BYTE										HighEQGain ;
	BYTE										HighEQCutoff ;

	float										RoomFilterFreq ;
	float										RoomFilterMain ;
	float										RoomFilterHF ;
	float										ReflectionsGain ;
	float										ReverbGain ;
	float										DecayTime ;
	float										Density ;
	float										RoomSize ;

	BOOL										DisableLateField ;
} D_XAUDIO2FX_REVERB_PARAMETERS2_8 ;

typedef struct tagD_XAUDIO2FX_REVERB_I3DL2_PARAMETERS
{
	float										WetDryMix ;

	int											Room ;
	int											RoomHF ;
	float										RoomRolloffFactor ;
	float										DecayTime ;
	float										DecayHFRatio ;
	int											Reflections ;
	float										ReflectionsDelay ;
	int											Reverb ;
	float										ReverbDelay ;
	float										Diffusion ;
	float										Density ;
	float										HFReference ;
} D_XAUDIO2FX_REVERB_I3DL2_PARAMETERS ;

#define D_XAUDIO2FX_I3DL2_PRESET_DEFAULT         {100,-10000,    0,0.0f, 1.00f,0.50f,-10000,0.020f,-10000,0.040f,100.0f,100.0f,5000.0f}
#define D_XAUDIO2FX_I3DL2_PRESET_GENERIC         {100, -1000, -100,0.0f, 1.49f,0.83f, -2602,0.007f,   200,0.011f,100.0f,100.0f,5000.0f}
#define D_XAUDIO2FX_I3DL2_PRESET_PADDEDCELL      {100, -1000,-6000,0.0f, 0.17f,0.10f, -1204,0.001f,   207,0.002f,100.0f,100.0f,5000.0f}
#define D_XAUDIO2FX_I3DL2_PRESET_ROOM            {100, -1000, -454,0.0f, 0.40f,0.83f, -1646,0.002f,    53,0.003f,100.0f,100.0f,5000.0f}
#define D_XAUDIO2FX_I3DL2_PRESET_BATHROOM        {100, -1000,-1200,0.0f, 1.49f,0.54f,  -370,0.007f,  1030,0.011f,100.0f, 60.0f,5000.0f}
#define D_XAUDIO2FX_I3DL2_PRESET_LIVINGROOM      {100, -1000,-6000,0.0f, 0.50f,0.10f, -1376,0.003f, -1104,0.004f,100.0f,100.0f,5000.0f}
#define D_XAUDIO2FX_I3DL2_PRESET_STONEROOM       {100, -1000, -300,0.0f, 2.31f,0.64f,  -711,0.012f,    83,0.017f,100.0f,100.0f,5000.0f}
#define D_XAUDIO2FX_I3DL2_PRESET_AUDITORIUM      {100, -1000, -476,0.0f, 4.32f,0.59f,  -789,0.020f,  -289,0.030f,100.0f,100.0f,5000.0f}
#define D_XAUDIO2FX_I3DL2_PRESET_CONCERTHALL     {100, -1000, -500,0.0f, 3.92f,0.70f, -1230,0.020f,    -2,0.029f,100.0f,100.0f,5000.0f}
#define D_XAUDIO2FX_I3DL2_PRESET_CAVE            {100, -1000,    0,0.0f, 2.91f,1.30f,  -602,0.015f,  -302,0.022f,100.0f,100.0f,5000.0f}
#define D_XAUDIO2FX_I3DL2_PRESET_ARENA           {100, -1000, -698,0.0f, 7.24f,0.33f, -1166,0.020f,    16,0.030f,100.0f,100.0f,5000.0f}
#define D_XAUDIO2FX_I3DL2_PRESET_HANGAR          {100, -1000,-1000,0.0f,10.05f,0.23f,  -602,0.020f,   198,0.030f,100.0f,100.0f,5000.0f}
#define D_XAUDIO2FX_I3DL2_PRESET_CARPETEDHALLWAY {100, -1000,-4000,0.0f, 0.30f,0.10f, -1831,0.002f, -1630,0.030f,100.0f,100.0f,5000.0f}
#define D_XAUDIO2FX_I3DL2_PRESET_HALLWAY         {100, -1000, -300,0.0f, 1.49f,0.59f, -1219,0.007f,   441,0.011f,100.0f,100.0f,5000.0f}
#define D_XAUDIO2FX_I3DL2_PRESET_STONECORRIDOR   {100, -1000, -237,0.0f, 2.70f,0.79f, -1214,0.013f,   395,0.020f,100.0f,100.0f,5000.0f}
#define D_XAUDIO2FX_I3DL2_PRESET_ALLEY           {100, -1000, -270,0.0f, 1.49f,0.86f, -1204,0.007f,    -4,0.011f,100.0f,100.0f,5000.0f}
#define D_XAUDIO2FX_I3DL2_PRESET_FOREST          {100, -1000,-3300,0.0f, 1.49f,0.54f, -2560,0.162f,  -613,0.088f, 79.0f,100.0f,5000.0f}
#define D_XAUDIO2FX_I3DL2_PRESET_CITY            {100, -1000, -800,0.0f, 1.49f,0.67f, -2273,0.007f, -2217,0.011f, 50.0f,100.0f,5000.0f}
#define D_XAUDIO2FX_I3DL2_PRESET_MOUNTAINS       {100, -1000,-2500,0.0f, 1.49f,0.21f, -2780,0.300f, -2014,0.100f, 27.0f,100.0f,5000.0f}
#define D_XAUDIO2FX_I3DL2_PRESET_QUARRY          {100, -1000,-1000,0.0f, 1.49f,0.83f,-10000,0.061f,   500,0.025f,100.0f,100.0f,5000.0f}
#define D_XAUDIO2FX_I3DL2_PRESET_PLAIN           {100, -1000,-2000,0.0f, 1.49f,0.50f, -2466,0.179f, -2514,0.100f, 21.0f,100.0f,5000.0f}
#define D_XAUDIO2FX_I3DL2_PRESET_PARKINGLOT      {100, -1000,    0,0.0f, 1.65f,1.50f, -1363,0.008f, -1153,0.012f,100.0f,100.0f,5000.0f}
#define D_XAUDIO2FX_I3DL2_PRESET_SEWERPIPE       {100, -1000,-1000,0.0f, 2.81f,0.14f,   429,0.014f,   648,0.021f, 80.0f, 60.0f,5000.0f}
#define D_XAUDIO2FX_I3DL2_PRESET_UNDERWATER      {100, -1000,-4000,0.0f, 1.49f,0.10f,  -449,0.007f,  1700,0.011f,100.0f,100.0f,5000.0f}
#define D_XAUDIO2FX_I3DL2_PRESET_SMALLROOM       {100, -1000, -600,0.0f, 1.10f,0.83f,  -400,0.005f,   500,0.010f,100.0f,100.0f,5000.0f}
#define D_XAUDIO2FX_I3DL2_PRESET_MEDIUMROOM      {100, -1000, -600,0.0f, 1.30f,0.83f, -1000,0.010f,  -200,0.020f,100.0f,100.0f,5000.0f}
#define D_XAUDIO2FX_I3DL2_PRESET_LARGEROOM       {100, -1000, -600,0.0f, 1.50f,0.83f, -1600,0.020f, -1000,0.040f,100.0f,100.0f,5000.0f}
#define D_XAUDIO2FX_I3DL2_PRESET_MEDIUMHALL      {100, -1000, -600,0.0f, 1.80f,0.70f, -1300,0.015f,  -800,0.030f,100.0f,100.0f,5000.0f}
#define D_XAUDIO2FX_I3DL2_PRESET_LARGEHALL       {100, -1000, -600,0.0f, 1.80f,0.70f, -2000,0.030f, -1400,0.060f,100.0f,100.0f,5000.0f}
#define D_XAUDIO2FX_I3DL2_PRESET_PLATE           {100, -1000, -200,0.0f, 1.30f,0.90f,     0,0.002f,     0,0.010f,100.0f, 75.0f,5000.0f}

#ifndef DX_GCC_COMPILE
#pragma pack(pop)
#pragma pack()
#else
#pragma pack(pop)
#endif


// ＸＡｕｄｉｏ ---------------------------------------------------------------

#ifndef DX_GCC_COMPILE
#pragma pack(push)
#pragma pack(1)
#else
#pragma pack(push,1)
#endif

#define D_XAUDIO2_MAX_BUFFER_BYTES				(0x80000000)
#define D_XAUDIO2_MAX_QUEUED_BUFFERS			(64)
#define D_XAUDIO2_MAX_BUFFERS_SYSTEM			(2)
#define D_XAUDIO2_MAX_AUDIO_CHANNELS			(64)
#define D_XAUDIO2_MIN_SAMPLE_RATE				(1000)
#define D_XAUDIO2_MAX_SAMPLE_RATE				(200000)
#define D_XAUDIO2_MAX_VOLUME_LEVEL				(16777216.0f)
#define D_XAUDIO2_MIN_FREQ_RATIO				(1/1024.0f)
#define D_XAUDIO2_MAX_FREQ_RATIO				(1024.0f)
#define D_XAUDIO2_DEFAULT_FREQ_RATIO			(2.0f)
#define D_XAUDIO2_MAX_FILTER_ONEOVERQ			(1.5f)
#define D_XAUDIO2_MAX_FILTER_FREQUENCY			(1.0f)
#define D_XAUDIO2_MAX_LOOP_COUNT				(254)
#define D_XAUDIO2_MAX_INSTANCES					(8)

#define D_XAUDIO2_COMMIT_NOW					(0)
#define D_XAUDIO2_COMMIT_ALL					(0)
#define D_XAUDIO2_INVALID_OPSET					((DWORD)(-1))
#define D_XAUDIO2_NO_LOOP_REGION				(0)
#define D_XAUDIO2_LOOP_INFINITE					(255)
#define D_XAUDIO2_DEFAULT_CHANNELS				(0)
#define D_XAUDIO2_DEFAULT_SAMPLERATE			(0)

#define D_XAUDIO2_DEBUG_ENGINE					(0x0001)
#define D_XAUDIO2_VOICE_NOPITCH					(0x0002)
#define D_XAUDIO2_VOICE_NOSRC					(0x0004)
#define D_XAUDIO2_VOICE_USEFILTER				(0x0008)
#define D_XAUDIO2_VOICE_MUSIC					(0x0010)
#define D_XAUDIO2_PLAY_TAILS					(0x0020)
#define D_XAUDIO2_END_OF_STREAM					(0x0040)
#define D_XAUDIO2_SEND_USEFILTER				(0x0080)

typedef enum tagD_XAUDIO2_WINDOWS_PROCESSOR_SPECIFIER
{
	D_Processor1 								= 0x00000001,
	D_Processor2 								= 0x00000002,
	D_Processor3 								= 0x00000004,
	D_Processor4 								= 0x00000008,
	D_Processor5 								= 0x00000010,
	D_Processor6 								= 0x00000020,
	D_Processor7 								= 0x00000040,
	D_Processor8 								= 0x00000080,
	D_Processor9 								= 0x00000100,
	D_Processor10								= 0x00000200,
	D_Processor11								= 0x00000400,
	D_Processor12								= 0x00000800,
	D_Processor13								= 0x00001000,
	D_Processor14								= 0x00002000,
	D_Processor15								= 0x00004000,
	D_Processor16								= 0x00008000,
	D_Processor17								= 0x00010000,
	D_Processor18								= 0x00020000,
	D_Processor19								= 0x00040000,
	D_Processor20								= 0x00080000,
	D_Processor21								= 0x00100000,
	D_Processor22								= 0x00200000,
	D_Processor23								= 0x00400000,
	D_Processor24								= 0x00800000,
	D_Processor25								= 0x01000000,
	D_Processor26								= 0x02000000,
	D_Processor27								= 0x04000000,
	D_Processor28								= 0x08000000,
	D_Processor29								= 0x10000000,
	D_Processor30								= 0x20000000,
	D_Processor31								= 0x40000000,
	D_Processor32								= 0x80000000,
	D_XAUDIO2_ANY_PROCESSOR						= 0xffffffff,
	D_XAUDIO2_DEFAULT_PROCESSOR					= D_XAUDIO2_ANY_PROCESSOR
} D_XAUDIO2_WINDOWS_PROCESSOR_SPECIFIER, D_XAUDIO2_PROCESSOR ;

enum D_XAUDIO2_DEVICE_ROLE
{
	D_NotDefaultDevice							= 0x0,
	D_DefaultConsoleDevice						= 0x1,
	D_DefaultMultimediaDevice					= 0x2,
	D_DefaultCommunicationsDevice				= 0x4,
	D_DefaultGameDevice							= 0x8,
	D_GlobalDefaultDevice						= 0xf,
	D_InvalidDeviceRole							= ~D_GlobalDefaultDevice
} ;

enum D_XAUDIO2_FILTER_TYPE
{
	D_LowPassFilter,
	D_BandPassFilter,
	D_HighPassFilter,
	D_NotchFilter
} ;

enum D_AUDIO_STREAM_CATEGORY
{
	D_AudioCategory_Other = 0,
	D_AudioCategory_ForegroundOnlyMedia,
	D_AudioCategory_BackgroundCapableMedia,
	D_AudioCategory_Communications,
	D_AudioCategory_Alerts,
	D_AudioCategory_SoundEffects,
	D_AudioCategory_GameEffects,
	D_AudioCategory_GameMedia,
} ;

typedef struct tagD_WAVEFORMATEXTENSIBLE
{
	WAVEFORMATEX								Format ;
	union
	{
		WORD									wValidBitsPerSample ;
		WORD									wSamplesPerBlock ;
		WORD									wReserved ;
	} Samples ;
	DWORD										dwChannelMask ;
	GUID										SubFormat ;
} D_WAVEFORMATEXTENSIBLE ;

typedef struct tagD_XAUDIO2_VOICE_STATE
{
	void *										pCurrentBufferContext ;
	DWORD										BuffersQueued ;
	ULONGLONG									SamplesPlayed ;
} D_XAUDIO2_VOICE_STATE;

typedef struct tagD_XAUDIO2_EFFECT_DESCRIPTOR
{
	D_IUnknown*									pEffect ;
	BOOL										InitialState ;
	DWORD										OutputChannels ;
} D_XAUDIO2_EFFECT_DESCRIPTOR ;

typedef struct tagD_XAUDIO2_DEVICE_DETAILS
{
	WCHAR										DeviceID[256] ;
	WCHAR										DisplayName[256] ;
	D_XAUDIO2_DEVICE_ROLE						Role ;
	D_WAVEFORMATEXTENSIBLE						OutputFormat ;
} D_XAUDIO2_DEVICE_DETAILS ;

typedef struct tagD_XAUDIO2_VOICE_DETAILS
{
	DWORD										CreationFlags ;
	DWORD										InputChannels ;
	DWORD										InputSampleRate ;
} D_XAUDIO2_VOICE_DETAILS ;

typedef struct tagD_XAUDIO2_VOICE_DETAILS2_8
{
	DWORD										CreationFlags ;
	DWORD										ActiveFlags ;
	DWORD										InputChannels ;
	DWORD										InputSampleRate ;
} D_XAUDIO2_VOICE_DETAILS2_8 ;

typedef struct tagD_XAUDIO2_SEND_DESCRIPTOR
{
	DWORD										Flags ;
	class D_IXAudio2Voice*						pOutputVoice ;
} D_XAUDIO2_SEND_DESCRIPTOR ;

typedef struct tagD_XAUDIO2_SEND_DESCRIPTOR2_8
{
	DWORD										Flags ;
	class D_IXAudio2_8Voice*					pOutputVoice ;
} D_XAUDIO2_SEND_DESCRIPTOR2_8 ;

typedef struct tagD_XAUDIO2_VOICE_SENDS
{
	DWORD										SendCount ;
	D_XAUDIO2_SEND_DESCRIPTOR*					pSends ;
} D_XAUDIO2_VOICE_SENDS ;

typedef struct tagD_XAUDIO2_VOICE_SENDS2_8
{
	DWORD										SendCount ;
	D_XAUDIO2_SEND_DESCRIPTOR2_8*				pSends ;
} D_XAUDIO2_VOICE_SENDS2_8 ;

typedef struct tagD_XAUDIO2_PERFORMANCE_DATA
{
	ULONGLONG									AudioCyclesSinceLastQuery ;
	ULONGLONG									TotalCyclesSinceLastQuery ;
	DWORD										MinimumCyclesPerQuantum ;
	DWORD										MaximumCyclesPerQuantum ;
	DWORD										MemoryUsageInBytes ;
	DWORD										CurrentLatencyInSamples ;
	DWORD										GlitchesSinceEngineStarted ;
	DWORD										ActiveSourceVoiceCount ;
	DWORD										TotalSourceVoiceCount ;
	DWORD										ActiveSubmixVoiceCount ;
	DWORD										ActiveResamplerCount ;
	DWORD										ActiveMatrixMixCount ;
	DWORD										ActiveXmaSourceVoices ;
	DWORD										ActiveXmaStreams ;
} D_XAUDIO2_PERFORMANCE_DATA ;

typedef struct tagD_XAUDIO2_DEBUG_CONFIGURATION
{
	DWORD										TraceMask ;
	DWORD										BreakMask ;
	BOOL										LogThreadID ;
	BOOL										LogFileline ;
	BOOL										LogFunctionName ;
	BOOL										LogTiming ;
} D_XAUDIO2_DEBUG_CONFIGURATION ;

typedef struct tagD_XAUDIO2_EFFECT_CHAIN
{
	DWORD										EffectCount ;
	D_XAUDIO2_EFFECT_DESCRIPTOR*				pEffectDescriptors ;
} D_XAUDIO2_EFFECT_CHAIN ;

typedef struct tagD_XAUDIO2_BUFFER
{
	DWORD										Flags ;
	DWORD										AudioBytes ;
	const BYTE*									pAudioData ;
	DWORD										PlayBegin ;
	DWORD										PlayLength ;
	DWORD										LoopBegin ;
	DWORD										LoopLength ;
	DWORD										LoopCount ;
	void*										pContext ;
} D_XAUDIO2_BUFFER;

typedef struct tagD_XAUDIO2_FILTER_PARAMETERS
{
	D_XAUDIO2_FILTER_TYPE						Type;
	float										Frequency;
	float										OneOverQ;
} D_XAUDIO2_FILTER_PARAMETERS ;

typedef struct tagD_XAUDIO2_BUFFER_WMA
{
	const DWORD*								pDecodedPacketCumulativeBytes ;
	DWORD										PacketCount ;
} D_XAUDIO2_BUFFER_WMA ;

class D_IXAudio2VoiceCallback
{
public:
	virtual void    __stdcall OnVoiceProcessingPassStart( DWORD BytesRequired ) = 0 ;
	virtual void    __stdcall OnVoiceProcessingPassEnd	() = 0 ;
	virtual void    __stdcall OnStreamEnd				() = 0 ;
	virtual void    __stdcall OnBufferStart				( void* pBufferContext ) = 0 ;
	virtual void    __stdcall OnBufferEnd				( void* pBufferContext ) = 0 ;
	virtual void    __stdcall OnLoopEnd					( void* pBufferContext ) = 0 ;
	virtual void    __stdcall OnVoiceError				( void* pBufferContext, HRESULT Error ) = 0 ;
};

class D_IXAudio2EngineCallback
{
public:
	virtual void    __stdcall OnProcessingPassStart		() = 0 ;
	virtual void    __stdcall OnProcessingPassEnd		() = 0 ;
	virtual void    __stdcall OnCriticalError			( HRESULT Error ) = 0 ;
};

class D_IXAudio2 : public D_IUnknown
{
public:
	virtual HRESULT __stdcall QueryInterface			( REFIID riid,  void** ppvInterface ) = 0 ;
	virtual ULONG   __stdcall AddRef					( void ) = 0 ;
	virtual ULONG   __stdcall Release					( void ) = 0 ;
	virtual HRESULT __stdcall GetDeviceCount			( DWORD* pCount ) = 0 ;
	virtual HRESULT __stdcall GetDeviceDetails			( DWORD Index,  D_XAUDIO2_DEVICE_DETAILS* pDeviceDetails ) = 0 ;
	virtual HRESULT __stdcall Initialize				( DWORD Flags = 0, D_XAUDIO2_PROCESSOR XAudio2Processor = D_XAUDIO2_DEFAULT_PROCESSOR ) = 0 ;
	virtual HRESULT __stdcall RegisterForCallbacks		( D_IXAudio2EngineCallback* pCallback ) = 0 ;
	virtual void    __stdcall UnregisterForCallbacks	( D_IXAudio2EngineCallback* pCallback ) = 0 ;
	virtual HRESULT __stdcall CreateSourceVoice			( class D_IXAudio2SourceVoice** ppSourceVoice, const WAVEFORMATEX* pSourceFormat, DWORD Flags = 0, float MaxFrequencyRatio = D_XAUDIO2_DEFAULT_FREQ_RATIO, class D_IXAudio2VoiceCallback* pCallback = NULL, const D_XAUDIO2_VOICE_SENDS* pSendList = NULL, const D_XAUDIO2_EFFECT_CHAIN* pEffectChain = NULL ) = 0 ;
	virtual HRESULT __stdcall CreateSubmixVoice			( class D_IXAudio2SubmixVoice** ppSubmixVoice, DWORD InputChannels, DWORD InputSampleRate, DWORD Flags = 0, DWORD ProcessingStage = 0, const D_XAUDIO2_VOICE_SENDS* pSendList = NULL, const D_XAUDIO2_EFFECT_CHAIN* pEffectChain = NULL ) = 0 ;
	virtual HRESULT __stdcall CreateMasteringVoice		( class D_IXAudio2MasteringVoice** ppMasteringVoice, DWORD InputChannels = D_XAUDIO2_DEFAULT_CHANNELS, DWORD InputSampleRate = D_XAUDIO2_DEFAULT_SAMPLERATE, DWORD Flags = 0, DWORD DeviceIndex = 0, const D_XAUDIO2_EFFECT_CHAIN* pEffectChain = NULL ) = 0 ;
	virtual HRESULT __stdcall StartEngine				( void ) = 0 ;
	virtual void    __stdcall StopEngine				( void ) = 0 ;
	virtual HRESULT __stdcall CommitChanges				( DWORD OperationSet ) = 0 ;
	virtual void    __stdcall GetPerformanceData		( D_XAUDIO2_PERFORMANCE_DATA* pPerfData ) = 0 ;
	virtual void    __stdcall SetDebugConfigurationv	( const D_XAUDIO2_DEBUG_CONFIGURATION* pDebugConfiguration, void* pReserved = NULL ) = 0 ;
} ;

class D_IXAudio2_8 : public D_IUnknown
{
public :
    virtual HRESULT __stdcall QueryInterface			( REFIID riid, void** ppvInterface ) = 0 ;
    virtual ULONG   __stdcall AddRef					( void ) = 0 ;
    virtual ULONG   __stdcall Release					( void ) = 0 ;
    virtual HRESULT __stdcall RegisterForCallbacks		( D_IXAudio2EngineCallback* pCallback ) = 0 ;
    virtual void    __stdcall UnregisterForCallbacks	( D_IXAudio2EngineCallback* pCallback ) = 0 ;
    virtual HRESULT __stdcall CreateSourceVoice			( class D_IXAudio2_8SourceVoice** ppSourceVoice, const WAVEFORMATEX* pSourceFormat, DWORD Flags = 0, float MaxFrequencyRatio = D_XAUDIO2_DEFAULT_FREQ_RATIO, D_IXAudio2VoiceCallback* pCallback = NULL, const D_XAUDIO2_VOICE_SENDS2_8* pSendList = NULL, const D_XAUDIO2_EFFECT_CHAIN* pEffectChain = NULL ) = 0 ;
    virtual HRESULT __stdcall CreateSubmixVoice			( class D_IXAudio2_8SubmixVoice** ppSubmixVoice, DWORD InputChannels, DWORD InputSampleRate, DWORD Flags = 0, DWORD ProcessingStage = 0, const D_XAUDIO2_VOICE_SENDS* pSendList = NULL, const D_XAUDIO2_EFFECT_CHAIN* pEffectChain = NULL ) = 0 ;
    virtual HRESULT __stdcall CreateMasteringVoice		( class D_IXAudio2_8MasteringVoice** ppMasteringVoice, DWORD InputChannels = D_XAUDIO2_DEFAULT_CHANNELS, DWORD InputSampleRate = D_XAUDIO2_DEFAULT_SAMPLERATE, DWORD Flags = 0, LPCWSTR szDeviceId = NULL, const D_XAUDIO2_EFFECT_CHAIN* pEffectChain = NULL, D_AUDIO_STREAM_CATEGORY StreamCategory = D_AudioCategory_GameEffects ) = 0 ;
    virtual HRESULT __stdcall StartEngine				( void ) = 0 ;
    virtual void    __stdcall StopEngine				( void ) = 0 ;
    virtual HRESULT __stdcall CommitChanges				( DWORD OperationSet ) = 0 ;
    virtual void    __stdcall GetPerformanceData		( D_XAUDIO2_PERFORMANCE_DATA* pPerfData ) = 0 ;
    virtual void    __stdcall SetDebugConfiguration		( const D_XAUDIO2_DEBUG_CONFIGURATION* pDebugConfiguration, void* pReserved = NULL ) = 0 ;
} ;

class D_IXAudio2Voice
{
public:
	virtual void    __stdcall GetVoiceDetailsv			( D_XAUDIO2_VOICE_DETAILS* pVoiceDetails ) = 0 ;
	virtual HRESULT __stdcall SetOutputVoices			( const D_XAUDIO2_VOICE_SENDS* pSendList ) = 0 ;
	virtual HRESULT __stdcall SetEffectChain			( const D_XAUDIO2_EFFECT_CHAIN* pEffectChain ) = 0 ;
	virtual HRESULT __stdcall EnableEffect				( DWORD EffectIndex, DWORD OperationSet = D_XAUDIO2_COMMIT_NOW ) = 0 ;
	virtual HRESULT __stdcall DisableEffect				( DWORD EffectIndex, DWORD OperationSet = D_XAUDIO2_COMMIT_NOW ) = 0 ;
	virtual void    __stdcall GetEffectState			( DWORD EffectIndex,  BOOL* pEnabled ) = 0 ;
	virtual HRESULT __stdcall SetEffectParameters		( DWORD EffectIndex, const void* pParameters, DWORD ParametersByteSize, DWORD OperationSet = D_XAUDIO2_COMMIT_NOW ) = 0 ;
	virtual HRESULT __stdcall GetEffectParameters		( DWORD EffectIndex, void* pParameters, DWORD ParametersByteSize ) = 0 ;
	virtual HRESULT __stdcall SetFilterParameters		( const D_XAUDIO2_FILTER_PARAMETERS* pParameters, DWORD OperationSet = D_XAUDIO2_COMMIT_NOW ) = 0 ;
	virtual void    __stdcall GetFilterParameters		( D_XAUDIO2_FILTER_PARAMETERS* pParameters ) = 0 ;
	virtual HRESULT __stdcall SetOutputFilterParameters	( D_IXAudio2Voice* pDestinationVoice, const D_XAUDIO2_FILTER_PARAMETERS* pParameters, DWORD OperationSet = D_XAUDIO2_COMMIT_NOW ) = 0 ;
	virtual void    __stdcall GetOutputFilterParameters	( D_IXAudio2Voice* pDestinationVoice, D_XAUDIO2_FILTER_PARAMETERS* pParameters ) = 0 ;
	virtual HRESULT __stdcall SetVolume					( float Volume, DWORD OperationSet = D_XAUDIO2_COMMIT_NOW ) = 0 ;
	virtual void    __stdcall GetVolume					( float* pVolume ) = 0 ;
	virtual HRESULT __stdcall SetChannelVolumes			( DWORD Channels, const float* pVolumes, DWORD OperationSet = D_XAUDIO2_COMMIT_NOW ) = 0 ;
	virtual void    __stdcall GetChannelVolumes			( DWORD Channels, float* pVolumes ) = 0 ; 
	virtual HRESULT __stdcall SetOutputMatrix			( D_IXAudio2Voice* pDestinationVoice, DWORD SourceChannels, DWORD DestinationChannels, const float* pLevelMatrix, DWORD OperationSet = D_XAUDIO2_COMMIT_NOW ) = 0 ;
	virtual void    __stdcall GetOutputMatrix			( D_IXAudio2Voice* pDestinationVoice, DWORD SourceChannels, DWORD DestinationChannels, float* pLevelMatrix ) = 0 ;
	virtual void    __stdcall DestroyVoice				( void ) = 0 ;
} ;

class D_IXAudio2_8Voice
{
public:
	virtual void    __stdcall GetVoiceDetails			( D_XAUDIO2_VOICE_DETAILS2_8* pVoiceDetails ) = 0 ;
	virtual HRESULT __stdcall SetOutputVoices			( const D_XAUDIO2_VOICE_SENDS* pSendList ) = 0 ;
	virtual HRESULT __stdcall SetEffectChain			( const D_XAUDIO2_EFFECT_CHAIN* pEffectChain ) = 0 ;
	virtual HRESULT __stdcall EnableEffect				( DWORD EffectIndex, DWORD OperationSet = D_XAUDIO2_COMMIT_NOW ) = 0 ;
	virtual HRESULT __stdcall DisableEffect				( DWORD EffectIndex, DWORD OperationSet = D_XAUDIO2_COMMIT_NOW ) = 0 ;
	virtual void    __stdcall GetEffectState			( DWORD EffectIndex, BOOL* pEnabled ) = 0 ;
	virtual HRESULT __stdcall SetEffectParameters		( DWORD EffectIndex, const void* pParameters, DWORD ParametersByteSize, DWORD OperationSet = D_XAUDIO2_COMMIT_NOW ) = 0 ;
	virtual HRESULT __stdcall GetEffectParameters		( DWORD EffectIndex, void* pParameters, DWORD ParametersByteSize ) = 0 ;
	virtual HRESULT __stdcall SetFilterParameters		( const D_XAUDIO2_FILTER_PARAMETERS* pParameters, DWORD OperationSet = D_XAUDIO2_COMMIT_NOW ) = 0 ;
	virtual void    __stdcall GetFilterParameters		( D_XAUDIO2_FILTER_PARAMETERS* pParameters ) = 0 ;
	virtual HRESULT __stdcall SetOutputFilterParameters	( D_IXAudio2_8Voice* pDestinationVoice, const D_XAUDIO2_FILTER_PARAMETERS* pParameters, DWORD OperationSet = D_XAUDIO2_COMMIT_NOW ) = 0 ;
	virtual void    __stdcall GetOutputFilterParameters	( D_IXAudio2_8Voice* pDestinationVoice, D_XAUDIO2_FILTER_PARAMETERS* pParameters ) = 0 ;
	virtual HRESULT __stdcall SetVolume					( float Volume, DWORD OperationSet = D_XAUDIO2_COMMIT_NOW ) = 0 ;
	virtual void    __stdcall GetVolume					( float* pVolume ) = 0 ;
	virtual HRESULT __stdcall SetChannelVolumes			( DWORD Channels, const float* pVolumes, DWORD OperationSet = D_XAUDIO2_COMMIT_NOW ) = 0 ;
	virtual void    __stdcall GetChannelVolumes			( DWORD Channels, float* pVolumes ) = 0 ;
	virtual HRESULT __stdcall SetOutputMatrix			( D_IXAudio2_8Voice* pDestinationVoice, DWORD SourceChannels, DWORD DestinationChannels, const float* pLevelMatrix, DWORD OperationSet = D_XAUDIO2_COMMIT_NOW ) = 0 ;
	virtual void    __stdcall GetOutputMatrix			( D_IXAudio2_8Voice* pDestinationVoice, DWORD SourceChannels, DWORD DestinationChannels, float* pLevelMatrix ) = 0 ;
	virtual void    __stdcall DestroyVoice				( void ) = 0 ;
} ;

class D_IXAudio2SubmixVoice : public D_IXAudio2Voice
{
};

class D_IXAudio2_8SubmixVoice : public D_IXAudio2_8Voice
{
};

class D_IXAudio2MasteringVoice : public D_IXAudio2Voice
{
};

class D_IXAudio2_8MasteringVoice : public D_IXAudio2_8Voice
{
public :
	virtual HRESULT __stdcall GetChannelMask			( DWORD * pChannelmask ) = 0 ;
};

class D_IXAudio2SourceVoice : public D_IXAudio2Voice
{
public:
	virtual HRESULT __stdcall Start						( DWORD Flags = 0, DWORD OperationSet = D_XAUDIO2_COMMIT_NOW ) = 0 ;
	virtual HRESULT __stdcall Stop						( DWORD Flags = 0, DWORD OperationSet = D_XAUDIO2_COMMIT_NOW ) = 0 ;
	virtual HRESULT __stdcall SubmitSourceBuffer		( const D_XAUDIO2_BUFFER* pBuffer,  const D_XAUDIO2_BUFFER_WMA* pBufferWMA = NULL ) = 0 ;
	virtual HRESULT __stdcall FlushSourceBuffers		( void ) = 0 ;
	virtual HRESULT __stdcall Discontinuity				( void ) = 0 ;
	virtual HRESULT __stdcall ExitLoop					( DWORD OperationSet = D_XAUDIO2_COMMIT_NOW ) = 0 ;
	virtual void    __stdcall GetState					( D_XAUDIO2_VOICE_STATE* pVoiceState ) = 0 ;
	virtual HRESULT __stdcall SetFrequencyRatio			( float Ratio, DWORD OperationSet = D_XAUDIO2_COMMIT_NOW ) = 0 ;
	virtual void    __stdcall GetFrequencyRatio			( float* pRatio ) = 0 ;
	virtual HRESULT __stdcall SetSourceSampleRate		( DWORD NewSourceSampleRate ) = 0 ;
};

class D_IXAudio2_8SourceVoice : public D_IXAudio2_8Voice
{
public:
	virtual HRESULT __stdcall Start						( DWORD Flags = 0, DWORD OperationSet = D_XAUDIO2_COMMIT_NOW ) = 0 ;
	virtual HRESULT __stdcall Stop						( DWORD Flags = 0, DWORD OperationSet = D_XAUDIO2_COMMIT_NOW ) = 0 ;
	virtual HRESULT __stdcall SubmitSourceBuffer		( const D_XAUDIO2_BUFFER* pBuffer, const D_XAUDIO2_BUFFER_WMA* pBufferWMA = NULL ) = 0 ;
	virtual HRESULT __stdcall FlushSourceBuffers		( void ) = 0 ;
	virtual HRESULT __stdcall Discontinuity				( void ) = 0 ;
	virtual HRESULT __stdcall ExitLoop					( DWORD OperationSet = D_XAUDIO2_COMMIT_NOW ) = 0 ;
	virtual void    __stdcall GetState					( D_XAUDIO2_VOICE_STATE* pVoiceState, DWORD Flags = 0 ) = 0 ;
	virtual HRESULT __stdcall SetFrequencyRatio			( float Ratio,  DWORD OperationSet = D_XAUDIO2_COMMIT_NOW ) = 0 ;
	virtual void    __stdcall GetFrequencyRatio			( float* pRatio ) = 0 ;
	virtual HRESULT __stdcall SetSourceSampleRate		( DWORD NewSourceSampleRate ) = 0 ;
} ;

#ifndef DX_GCC_COMPILE
#pragma pack(pop)
#pragma pack()
#else
#pragma pack(pop)
#endif

// ＷＡＳＡＰＩ ---------------------------------------------------------------

typedef long									D_MUSIC_TIME ;
typedef LONGLONG								D_REFERENCE_TIME ;

#if 0

enum D_EDataFlow
{
	D_eRender,
	D_eCapture,
	D_eAll,
	D_EDataFlow_enum_count 
} ;

enum D_ERole
{
	D_eConsole,
	D_eMultimedia,
	D_eCommunications,
	D_ERole_enum_count 
} ;

enum D_AUDCLNT_SHAREMODE
{ 
	D_AUDCLNT_SHAREMODE_SHARED, 
	D_AUDCLNT_SHAREMODE_EXCLUSIVE 
} ;

#define D_DEVICE_STATE_ACTIVE							(0x00000001)
#define D_DEVICE_STATE_DISABLED							(0x00000002)
#define D_DEVICE_STATE_NOTPRESENT						(0x00000004)
#define D_DEVICE_STATE_UNPLUGGED						(0x00000008)
#define D_DEVICE_STATEMASK_ALL							(0x0000000f)

#define D_AUDCLNT_STREAMFLAGS_CROSSPROCESS				(0x00010000)
#define D_AUDCLNT_STREAMFLAGS_LOOPBACK					(0x00020000)
#define D_AUDCLNT_STREAMFLAGS_EVENTCALLBACK				(0x00040000)
#define D_AUDCLNT_STREAMFLAGS_NOPERSIST					(0x00080000)
#define D_AUDCLNT_STREAMFLAGS_RATEADJUST				(0x00100000)
#define D_AUDCLNT_SESSIONFLAGS_EXPIREWHENUNOWNED		(0x10000000)
#define D_AUDCLNT_SESSIONFLAGS_DISPLAY_HIDE				(0x20000000)
#define D_AUDCLNT_SESSIONFLAGS_DISPLAY_HIDEWHENEXPIRED	(0x40000000)

class D_IPropertyStore : public D_IUnknown
{
public:
	virtual HRESULT __stdcall GetCount					( DWORD *cProps ) = 0 ;
	virtual HRESULT __stdcall GetAt						( DWORD iProp, PROPERTYKEY *pkey ) = 0 ;
	virtual HRESULT __stdcall GetValue					( PROPERTYKEY &key, D_PROPVARIANT *pv ) = 0 ;
	virtual HRESULT __stdcall SetValue					( PROPERTYKEY &key, D_PROPVARIANT &propvar ) = 0 ;
	virtual HRESULT __stdcall Commit					( void ) = 0 ;
};

class D_IMMDevice : public D_IUnknown
{
public:
	virtual  HRESULT __stdcall Activate					( REFIID iid, DWORD dwClsCtx, D_PROPVARIANT *pActivationParams, void **ppInterface ) = 0 ;
	virtual  HRESULT __stdcall OpenPropertyStore		( DWORD stgmAccess, D_IPropertyStore **ppProperties ) = 0 ;
	virtual  HRESULT __stdcall GetId					( LPWSTR *ppstrId ) = 0 ;
	virtual  HRESULT __stdcall GetState					( DWORD *pdwState ) = 0 ;
} ;

class D_IMMNotificationClient : public D_IUnknown
{
public:
	virtual HRESULT __stdcall OnDeviceStateChanged		( LPCWSTR pwstrDeviceId, DWORD dwNewState ) = 0 ;
	virtual HRESULT __stdcall OnDeviceAdded				( LPCWSTR pwstrDeviceId ) = 0 ;
	virtual HRESULT __stdcall OnDeviceRemoved			( LPCWSTR pwstrDeviceId ) = 0 ;
	virtual HRESULT __stdcall OnDefaultDeviceChanged	( D_EDataFlow flow, D_ERole role, LPCWSTR pwstrDefaultDeviceId ) = 0 ;
	virtual HRESULT __stdcall OnPropertyValueChanged	( LPCWSTR pwstrDeviceId, const PROPERTYKEY key ) = 0 ;
} ;

class D_IMMDeviceCollection : public D_IUnknown
{
public:
	virtual HRESULT __stdcall GetCount					( UINT *pcDevices ) = 0 ;
	virtual HRESULT __stdcall Item						( UINT nDevice, D_IMMDevice **ppDevice ) = 0 ;
} ;

class D_IMMDeviceEnumerator : public D_IUnknown
{
public:
	virtual HRESULT __stdcall EnumAudioEndpoints		( D_EDataFlow dataFlow, DWORD dwStateMask, D_IMMDeviceCollection **ppDevices ) = 0 ;
	virtual HRESULT __stdcall GetDefaultAudioEndpoint	( D_EDataFlow dataFlow, D_ERole role, D_IMMDevice **ppEndpoint ) = 0 ;
	virtual HRESULT __stdcall GetDevice					( LPCWSTR pwstrId, D_IMMDevice **ppDevice ) = 0 ;
	virtual HRESULT __stdcall RegisterEndpointNotificationCallback( D_IMMNotificationClient *pClient ) = 0 ;
	virtual HRESULT __stdcall UnregisterEndpointNotificationCallback( D_IMMNotificationClient *pClient ) = 0 ;
} ;

class D_IAudioRenderClient : public D_IUnknown
{
public:
	virtual HRESULT __stdcall GetBuffer					( DWORD NumFramesRequested, BYTE **ppData ) = 0 ;
	virtual HRESULT __stdcall ReleaseBuffer				( DWORD NumFramesWritten, DWORD dwFlags ) = 0 ;
} ;

class D_IAudioClient : public D_IUnknown
{
public:
	virtual HRESULT __stdcall Initialize				( D_AUDCLNT_SHAREMODE ShareMode, DWORD StreamFlags, D_REFERENCE_TIME hnsBufferDuration, D_REFERENCE_TIME hnsPeriodicity, const WAVEFORMATEX *pFormat, LPCGUID AudioSessionGuid ) = 0 ;
	virtual HRESULT __stdcall GetBufferSize				( DWORD *pNumBufferFrames ) = 0 ;
	virtual HRESULT __stdcall GetStreamLatency			( D_REFERENCE_TIME *phnsLatency ) = 0 ;
	virtual HRESULT __stdcall GetCurrentPadding			( DWORD *pNumPaddingFrames ) = 0 ;
	virtual HRESULT __stdcall IsFormatSupported			( D_AUDCLNT_SHAREMODE ShareMode, const WAVEFORMATEX *pFormat, WAVEFORMATEX **ppClosestMatch ) = 0 ;
	virtual HRESULT __stdcall GetMixFormat				( WAVEFORMATEX **ppDeviceFormat ) = 0 ;
	virtual HRESULT __stdcall GetDevicePeriod			( D_REFERENCE_TIME *phnsDefaultDevicePeriod, D_REFERENCE_TIME *phnsMinimumDevicePeriod ) = 0 ;
	virtual HRESULT __stdcall Start						( void ) = 0 ;
	virtual HRESULT __stdcall Stop						( void ) = 0 ;
	virtual HRESULT __stdcall Reset						( void ) = 0 ;
	virtual HRESULT __stdcall SetEventHandle			( HANDLE eventHandle ) = 0 ;
	virtual HRESULT __stdcall GetService				( REFIID riid, void **ppv ) = 0 ;
} ;

#endif

// ＤｉｒｅｃｔＭｕｓｉｃ -----------------------------------------------------

#define D_DMUS_APATH_SHARED_STEREOPLUSREVERB	(1)
#define D_DMUS_APATH_DYNAMIC_3D					(6)
#define D_DMUS_APATH_DYNAMIC_MONO				(7)
#define D_DMUS_APATH_DYNAMIC_STEREO				(8)

#define D_DMUS_AUDIOF_3D						(0x1)
#define D_DMUS_AUDIOF_ENVIRON					(0x2)
#define D_DMUS_AUDIOF_EAX						(0x4)
#define D_DMUS_AUDIOF_DMOS						(0x8)
#define D_DMUS_AUDIOF_STREAMING					(0x10)
#define D_DMUS_AUDIOF_BUFFERS					(0x20)
#define D_DMUS_AUDIOF_ALL						(0x3F)

#define D_DMUS_PC_OUTPUTCLASS					(1)
#define D_DMUS_SEG_REPEAT_INFINITE				(0xFFFFFFFF)

#define D_DMUS_MAX_DESCRIPTION					(128)
#define D_DMUS_MAX_CATEGORY						(64)
#define D_DMUS_MAX_NAME							(64)
#define D_DMUS_MAX_FILENAME						MAX_PATH

#define D_DMUS_AUDIOPARAMS_FEATURES				(0x00000001)
#define D_DMUS_AUDIOPARAMS_SAMPLERATE			(0x00000004)
#define D_DMUS_AUDIOPARAMS_DEFAULTSYNTH			(0x00000008)

#define D_DMUS_OBJ_CLASS						(1 << 1)
#define D_DMUS_OBJ_MEMORY						(1 << 10)

#define D_DMUS_PATH_SEGMENT						(0x1000)
#define D_DMUS_PATH_SEGMENT_TRACK				(0x1100)
#define D_DMUS_PATH_SEGMENT_GRAPH				(0x1200)
#define D_DMUS_PATH_SEGMENT_TOOL				(0x1300)
#define D_DMUS_PATH_AUDIOPATH					(0x2000)
#define D_DMUS_PATH_AUDIOPATH_GRAPH				(0x2200)
#define D_DMUS_PATH_AUDIOPATH_TOOL				(0x2300)
#define D_DMUS_PATH_PERFORMANCE					(0x3000)
#define D_DMUS_PATH_PERFORMANCE_GRAPH			(0x3200)
#define D_DMUS_PATH_PERFORMANCE_TOOL			(0x3300)
#define D_DMUS_PATH_PORT						(0x4000)
#define D_DMUS_PATH_BUFFER						(0x6000)
#define D_DMUS_PATH_BUFFER_DMO					(0x6100)
#define D_DMUS_PATH_MIXIN_BUFFER				(0x7000)
#define D_DMUS_PATH_MIXIN_BUFFER_DMO			(0x7100)
#define D_DMUS_PATH_PRIMARY_BUFFER				(0x8000)

#define D_DMUS_PCHANNEL_ALL						(0xFFFFFFFB)

typedef enum tagD_DMUS_SEGF_FLAGS
{
	D_DMUS_SEGF_REFTIME							= 1 << 6,
} D_DMUS_SEGF_FLAGS ;

typedef struct tagD_DMUS_PORTCAPS
{
	DWORD										dwSize;
	DWORD										dwFlags;
	GUID										guidPort;
	DWORD										dwClass;
	DWORD										dwType;
	DWORD										dwMemorySize;
	DWORD										dwMaxChannelGroups;
	DWORD										dwMaxVoices;	
	DWORD										dwMaxAudioChannels;
	DWORD										dwEffectFlags;
	WCHAR										wszDescription[D_DMUS_MAX_DESCRIPTION];
} D_DMUS_PORTCAPS ;

typedef struct tagD_DMUS_VERSION
{
	DWORD										dwVersionMS;
	DWORD										dwVersionLS;
} D_DMUS_VERSION ;

typedef struct tagD_DMUS_OBJECTDESC
{
	DWORD										dwSize;
	DWORD										dwValidData;
	GUID										guidObject;
	GUID										guidClass;
	FILETIME									ftDate;
	D_DMUS_VERSION								vVersion;
	WCHAR										wszName[D_DMUS_MAX_NAME];
	WCHAR										wszCategory[D_DMUS_MAX_CATEGORY];
	WCHAR										wszFileName[D_DMUS_MAX_FILENAME];
	LONGLONG									llMemLength;
	BYTE										*pbMemData;
	IStream 									*pStream;
} D_DMUS_OBJECTDESC ;

typedef struct tagD_DMUS_AUDIOPARAMS
{
	DWORD										dwSize;
	BOOL										fInitNow;
	DWORD 										dwValidData;
	DWORD										dwFeatures;
	DWORD										dwVoices;
	DWORD										dwSampleRate;
	CLSID										clsidDefaultSynth;
} D_DMUS_AUDIOPARAMS ;

class D_IDirectMusic : public D_IUnknown
{
public :
	virtual HRESULT __stdcall EnumPort					( DWORD dwIndex, D_DMUS_PORTCAPS *pPortCaps ) = 0 ;
	virtual HRESULT __stdcall NonUse00					( void ) = 0 ;				// CreateMusicBuffer( LPDMUS_BUFFERDESC pBufferDesc, LPDIRECTMUSICBUFFER *ppBuffer, D_IUnknown * pUnkOuter ) = 0 ;
	virtual HRESULT __stdcall NonUse01					( void ) = 0 ;				// CreatePort( REFCLSID rclsidPort, LPDMUS_PORTPARAMS pPortParams, LPDIRECTMUSICPORT *ppPort, D_IUnknown * pUnkOuter ) = 0 ;
	virtual HRESULT __stdcall NonUse02					( void ) = 0 ;				// EnumMasterClock( DWORD dwIndex, LPDMUS_CLOCKINFO lpClockInfo ) = 0 ;
	virtual HRESULT __stdcall NonUse03					( void ) = 0 ;				// GetMasterClock( LPGUID pguidClock, IReferenceClock **ppReferenceClock ) = 0 ;
	virtual HRESULT __stdcall NonUse04					( void ) = 0 ;				// SetMasterClock( REFGUID rguidClock ) = 0 ;
	virtual HRESULT __stdcall NonUse05					( void ) = 0 ;				// Activate( BOOL fEnable ) = 0 ;
	virtual HRESULT __stdcall NonUse06					( void ) = 0 ;				// GetDefaultPort( LPGUID pguidPort ) = 0 ;
	virtual HRESULT __stdcall NonUse07					( void ) = 0 ;				// SetDirectSound( LPDIRECTSOUND pDirectSound, HWND hWnd ) = 0 ;
} ;

class D_IDirectMusic8 : public D_IDirectMusic
{
public :
	virtual HRESULT __stdcall NonUse08					( void ) = 0 ;				// SetExternalMasterClock( IReferenceClock *pClock ) = 0 ;
} ;

class D_IDirectMusicSegmentState : public D_IUnknown
{
public :
	virtual HRESULT __stdcall NonUse00					( void ) = 0 ;				// GetRepeats( DWORD* pdwRepeats ) = 0 ;
	virtual HRESULT __stdcall NonUse01					( void ) = 0 ;				// GetSegment( class IDirectMusicSegment** ppSegment ) = 0 ;
	virtual HRESULT __stdcall NonUse02					( void ) = 0 ;				// GetStartTime( MUSIC_TIME* pmtStart ) = 0 ;
	virtual HRESULT __stdcall GetSeek					( D_MUSIC_TIME *pmtSeek ) = 0 ;
	virtual HRESULT __stdcall NonUse03					( void ) = 0 ;				// GetStartPoint( MUSIC_TIME* pmtStart ) = 0 ;
} ;

class D_IDirectMusicSegment : public D_IUnknown
{
public :
	virtual HRESULT __stdcall NonUse00					( void ) = 0 ;				// GetLength( MUSIC_TIME* pmtLength ) = 0 ;
	virtual HRESULT __stdcall NonUse01					( void ) = 0 ;				// SetLength( MUSIC_TIME mtLength ) = 0 ;
	virtual HRESULT __stdcall NonUse02					( void ) = 0 ;				// GetRepeats( DWORD* pdwRepeats ) = 0 ;
	virtual HRESULT __stdcall SetRepeats				( DWORD	dwRepeats ) = 0 ;
	virtual HRESULT __stdcall NonUse03					( void ) = 0 ;				// GetDefaultResolution( DWORD* pdwResolution ) = 0 ;
	virtual HRESULT __stdcall NonUse04					( void ) = 0 ;				// SetDefaultResolution( DWORD	dwResolution ) = 0 ;
	virtual HRESULT __stdcall NonUse05					( void ) = 0 ;				// GetTrack( REFGUID rguidType, DWORD dwGroupBits, DWORD dwIndex, IDirectMusicTrack** ppTrack ) = 0 ;
	virtual HRESULT __stdcall NonUse06					( void ) = 0 ;				// GetTrackGroup( IDirectMusicTrack* pTrack, DWORD* pdwGroupBits ) = 0 ;
	virtual HRESULT __stdcall NonUse07					( void ) = 0 ;				// InsertTrack( IDirectMusicTrack* pTrack, DWORD dwGroupBits ) = 0 ;
	virtual HRESULT __stdcall NonUse08					( void ) = 0 ;				// RemoveTrack( IDirectMusicTrack* pTrack ) = 0 ;
	virtual HRESULT __stdcall NonUse09					( void ) = 0 ;				// InitPlay( IDirectMusicSegmentState** ppSegState, class IDirectMusicPerformance* pPerformance, DWORD dwFlags ) = 0 ;
	virtual HRESULT __stdcall NonUse10					( void ) = 0 ;				// GetGraph( IDirectMusicGraph** ppGraph ) = 0 ;
	virtual HRESULT __stdcall NonUse11					( void ) = 0 ;				// SetGraph( IDirectMusicGraph* pGraph ) = 0 ;
	virtual HRESULT __stdcall NonUse12					( void ) = 0 ;				// AddNotificationType( REFGUID rguidNotificationType ) = 0 ;
	virtual HRESULT __stdcall NonUse13					( void ) = 0 ;				// RemoveNotificationType( REFGUID rguidNotificationType ) = 0 ;
	virtual HRESULT __stdcall NonUse14					( void ) = 0 ;				// GetParam( REFGUID rguidType, DWORD dwGroupBits, DWORD dwIndex, MUSIC_TIME mtTime, MUSIC_TIME* pmtNext, void* pParam ) = 0 ; 
	virtual HRESULT __stdcall SetParam					( REFGUID rguidType, DWORD dwGroupBits, DWORD dwIndex, D_MUSIC_TIME mtTime, void *pParam ) = 0 ;
	virtual HRESULT __stdcall NonUse15					( void ) = 0 ;				// Clone( MUSIC_TIME mtStart, MUSIC_TIME mtEnd, IDirectMusicSegment** ppSegment ) = 0 ;
	virtual HRESULT __stdcall NonUse16					( void ) = 0 ;				// SetStartPoint( MUSIC_TIME mtStart ) = 0 ;
	virtual HRESULT __stdcall NonUse17					( void ) = 0 ;				// GetStartPoint( MUSIC_TIME* pmtStart ) = 0 ;
	virtual HRESULT __stdcall NonUse18					( void ) = 0 ;				// SetLoopPoints( MUSIC_TIME mtStart, MUSIC_TIME mtEnd ) = 0 ;
	virtual HRESULT __stdcall NonUse19					( void ) = 0 ;				// GetLoopPoints( MUSIC_TIME* pmtStart, MUSIC_TIME* pmtEnd ) = 0 ;
	virtual HRESULT __stdcall NonUse20					( void ) = 0 ;				// SetPChannelsUsed( DWORD dwNumPChannels, DWORD* paPChannels ) = 0 ;
} ;

class D_IDirectMusicSegment8 : public D_IDirectMusicSegment
{
public :
	virtual HRESULT __stdcall NonUse21					( void ) = 0 ;				// SetTrackConfig( REFGUID rguidTrackClassID, DWORD dwGroupBits, DWORD dwIndex, DWORD dwFlagsOn, DWORD dwFlagsOff ) = 0 ;
	virtual HRESULT __stdcall NonUse22					( void ) = 0 ;				// GetAudioPathConfig( D_IUnknown ** ppAudioPathConfig ) = 0 ;
	virtual HRESULT __stdcall NonUse23					( void ) = 0 ;				// Compose( MUSIC_TIME mtTime, IDirectMusicSegment* pFromSegment, IDirectMusicSegment* pToSegment, IDirectMusicSegment** ppComposedSegment ) = 0 ;
	virtual HRESULT __stdcall Download					( D_IUnknown *pAudioPath ) = 0 ;
	virtual HRESULT __stdcall Unload					( D_IUnknown *pAudioPath ) = 0 ;
} ;

class D_IDirectMusicLoader : public D_IUnknown
{
public :
	virtual HRESULT __stdcall GetObject					( D_DMUS_OBJECTDESC *pDesc, REFIID riid, void **ppv ) = 0 ;
	virtual HRESULT __stdcall NonUse00					( void ) = 0 ;				// SetObject( LPDMUS_OBJECTDESC pDesc ) = 0 ;
	virtual HRESULT __stdcall SetSearchDirectory		( REFGUID rguidClass, WCHAR *pwzPath, BOOL fClear ) = 0 ;
	virtual HRESULT __stdcall NonUse01					( void ) = 0 ;				// ScanDirectory( REFGUID rguidClass, WCHAR *pwzFileExtension, WCHAR *pwzScanFileName ) = 0 ;
	virtual HRESULT __stdcall NonUse02					( void ) = 0 ;				// CacheObject( IDirectMusicObject * pObject ) = 0 ;
	virtual HRESULT __stdcall NonUse03					( void ) = 0 ;				// ReleaseObject( IDirectMusicObject * pObject ) = 0 ;
	virtual HRESULT __stdcall ClearCache				( REFGUID rguidClass ) = 0 ;
	virtual HRESULT __stdcall NonUse05					( void ) = 0 ;				// EnableCache( REFGUID rguidClass, BOOL fEnable ) = 0 ;
	virtual HRESULT __stdcall NonUse06					( void ) = 0 ;				// EnumObject( REFGUID rguidClass, DWORD dwIndex, LPDMUS_OBJECTDESC pDesc ) = 0 ;
} ;

class D_IDirectMusicLoader8 : public D_IDirectMusicLoader
{
public :
	virtual void	__stdcall NonUse07					( void ) = 0 ;				// CollectGarbage( void ) = 0 ;
	virtual HRESULT __stdcall ReleaseObjectByUnknown( D_IUnknown *pObject ) = 0 ;
	virtual HRESULT __stdcall LoadObjectFromFile		( REFGUID rguidClassID, REFIID iidInterfaceID, WCHAR *pwzFilePath, void **ppObject ) = 0 ;
} ;

class D_IDirectMusicAudioPath : public D_IUnknown
{
public :
	virtual HRESULT __stdcall GetObjectInPath			( DWORD dwPChannel, DWORD dwStage, DWORD dwBuffer, REFGUID guidObject, DWORD dwIndex, REFGUID iidInterface, void ** ppObject ) = 0 ;
	virtual HRESULT __stdcall Activate					( BOOL fActivate ) = 0 ;
	virtual HRESULT __stdcall SetVolume					( long lVolume, DWORD dwDuration ) = 0 ;
	virtual HRESULT __stdcall ConvertPChannel			( DWORD dwPChannelIn, DWORD *pdwPChannelOut ) = 0 ;
} ;

class D_IDirectMusicPerformance : public D_IUnknown
{
public :
	virtual HRESULT __stdcall NonUse00					( void ) = 0 ;				// Init( IDirectMusic** ppDirectMusic, LPDIRECTSOUND pDirectSound, HWND hWnd ) = 0 ;
	virtual HRESULT __stdcall NonUse01					( void ) = 0 ;				// PlaySegment( IDirectMusicSegment* pSegment, DWORD dwFlags, __int64 i64StartTime, IDirectMusicSegmentState** ppSegmentState ) = 0 ;
	virtual HRESULT __stdcall Stop						( D_IDirectMusicSegment *pSegment, D_IDirectMusicSegmentState *pSegmentState, D_MUSIC_TIME mtTime, DWORD dwFlags ) = 0 ;
	virtual HRESULT __stdcall GetSegmentState			( D_IDirectMusicSegmentState **ppSegmentState, D_MUSIC_TIME mtTime ) = 0 ;
	virtual HRESULT __stdcall NonUse02					( void ) = 0 ;				// SetPrepareTime( DWORD dwMilliSeconds ) = 0 ;
	virtual HRESULT __stdcall NonUse03					( void ) = 0 ;				// GetPrepareTime( DWORD* pdwMilliSeconds ) = 0 ;
	virtual HRESULT __stdcall NonUse04					( void ) = 0 ;				// SetBumperLength( DWORD dwMilliSeconds ) = 0 ;
	virtual HRESULT __stdcall NonUse05					( void ) = 0 ;				// GetBumperLength( DWORD* pdwMilliSeconds ) = 0 ;
	virtual HRESULT __stdcall NonUse06					( void ) = 0 ;				// SendPMsg( DMUS_PMSG* pPMSG ) = 0 ;
	virtual HRESULT __stdcall NonUse07					( void ) = 0 ;				// MusicToReferenceTime( MUSIC_TIME mtTime, D_REFERENCE_TIME* prtTime ) = 0 ;
	virtual HRESULT __stdcall NonUse08					( void ) = 0 ;				// ReferenceToMusicTime( D_REFERENCE_TIME rtTime, MUSIC_TIME* pmtTime ) = 0 ;
	virtual HRESULT __stdcall IsPlaying					( D_IDirectMusicSegment *pSegment, D_IDirectMusicSegmentState *pSegState ) = 0 ;
	virtual HRESULT __stdcall GetTime					( D_REFERENCE_TIME* prtNow, D_MUSIC_TIME *pmtNow ) = 0 ;
	virtual HRESULT __stdcall NonUse09					( void ) = 0 ;				// AllocPMsg( ULONG cb, DMUS_PMSG** ppPMSG ) = 0 ;
	virtual HRESULT __stdcall NonUse10					( void ) = 0 ;				// FreePMsg( DMUS_PMSG* pPMSG ) = 0 ;
	virtual HRESULT __stdcall NonUse11					( void ) = 0 ;				// GetGraph( IDirectMusicGraph** ppGraph ) = 0 ;
	virtual HRESULT __stdcall NonUse12					( void ) = 0 ;				// SetGraph( IDirectMusicGraph* pGraph ) = 0 ;
	virtual HRESULT __stdcall NonUse13					( void ) = 0 ;				// SetNotificationHandle( HANDLE hNotification, D_REFERENCE_TIME rtMinimum ) = 0 ;
	virtual HRESULT __stdcall NonUse14					( void ) = 0 ;				// GetNotificationPMsg( DMUS_NOTIFICATION_PMSG** ppNotificationPMsg ) = 0 ;
	virtual HRESULT __stdcall NonUse15					( void ) = 0 ;				// AddNotificationType( REFGUID rguidNotificationType ) = 0 ;
	virtual HRESULT __stdcall NonUse16					( void ) = 0 ;				// RemoveNotificationType( REFGUID rguidNotificationType ) = 0 ;
	virtual HRESULT __stdcall NonUse17					( void ) = 0 ;				// AddPort( IDirectMusicPort* pPort ) = 0 ;
	virtual HRESULT __stdcall NonUse18					( void ) = 0 ;				// RemovePort( IDirectMusicPort* pPort	) = 0 ;
	virtual HRESULT __stdcall NonUse19					( void ) = 0 ;				// AssignPChannelBlock( DWORD dwBlockNum, IDirectMusicPort* pPort, DWORD dwGroup	) = 0 ;
	virtual HRESULT __stdcall NonUse20					( void ) = 0 ;				// AssignPChannel( DWORD dwPChannel, IDirectMusicPort* pPort, DWORD dwGroup, DWORD dwMChannel ) = 0 ;
	virtual HRESULT __stdcall NonUse21					( void ) = 0 ;				// PChannelInfo( DWORD dwPChannel, IDirectMusicPort** ppPort, DWORD* pdwGroup, DWORD* pdwMChannel ) = 0 ;
	virtual HRESULT __stdcall NonUse22					( void ) = 0 ;				// DownloadInstrument( IDirectMusicInstrument* pInst, DWORD dwPChannel, IDirectMusicDownloadedInstrument** ppDownInst, DMUS_NOTERANGE* pNoteRanges, DWORD dwNumNoteRanges, IDirectMusicPort** ppPort, DWORD* pdwGroup, DWORD* pdwMChannel ) = 0 ;
	virtual HRESULT __stdcall NonUse23					( void ) = 0 ;				// Invalidate( MUSIC_TIME mtTime, DWORD dwFlags ) = 0 ;
	virtual HRESULT __stdcall NonUse24					( void ) = 0 ;				// GetParam( REFGUID rguidType, DWORD dwGroupBits, DWORD dwIndex, MUSIC_TIME mtTime, MUSIC_TIME* pmtNext, void* pParam ) = 0 ; 
	virtual HRESULT __stdcall NonUse25					( void ) = 0 ;				// SetParam( REFGUID rguidType, DWORD dwGroupBits, DWORD dwIndex, MUSIC_TIME mtTime, void* pParam ) = 0 ;
	virtual HRESULT __stdcall GetGlobalParam			( REFGUID rguidType, void *pParam, DWORD dwSize ) = 0 ;
	virtual HRESULT __stdcall SetGlobalParam			( REFGUID rguidType, void *pParam, DWORD dwSize ) = 0 ;
	virtual HRESULT __stdcall NonUse26					( void ) = 0 ;				// GetLatencyTime( D_REFERENCE_TIME* prtTime ) = 0 ;
	virtual HRESULT __stdcall NonUse27					( void ) = 0 ;				// GetQueueTime( D_REFERENCE_TIME* prtTime ) = 0 ;
	virtual HRESULT __stdcall NonUse28					( void ) = 0 ;				// AdjustTime( D_REFERENCE_TIME rtAmount ) = 0 ;
	virtual HRESULT __stdcall CloseDown					( void ) = 0 ;
	virtual HRESULT __stdcall NonUse29					( void ) = 0 ;				// GetResolvedTime( D_REFERENCE_TIME rtTime, D_REFERENCE_TIME* prtResolved, DWORD dwTimeResolveFlags ) = 0 ;
	virtual HRESULT __stdcall NonUse30					( void ) = 0 ;				// MIDIToMusic( BYTE bMIDIValue, DMUS_CHORD_KEY* pChord, BYTE bPlayMode, BYTE bChordLevel, WORD *pwMusicValue ) = 0 ;
	virtual HRESULT __stdcall NonUse31					( void ) = 0 ;				// MusicToMIDI( WORD wMusicValue, DMUS_CHORD_KEY* pChord, BYTE bPlayMode, BYTE bChordLevel, BYTE *pbMIDIValue ) = 0 ;
	virtual HRESULT __stdcall NonUse32					( void ) = 0 ;				// TimeToRhythm( MUSIC_TIME mtTime, DMUS_TIMESIGNATURE *pTimeSig, WORD *pwMeasure, BYTE *pbBeat, BYTE *pbGrid, short *pnOffset ) = 0 ;
	virtual HRESULT __stdcall NonUse33					( void ) = 0 ;				// RhythmToTime( WORD wMeasure, BYTE bBeat, BYTE bGrid, short nOffset, DMUS_TIMESIGNATURE *pTimeSig, MUSIC_TIME *pmtTime ) = 0 ;	
} ;

class D_IDirectMusicPerformance8 : public D_IDirectMusicPerformance
{
public :
	virtual HRESULT __stdcall InitAudio					( D_IDirectMusic **ppDirectMusic, D_IDirectSound **ppDirectSound, HWND hWnd, DWORD dwDefaultPathType, DWORD dwPChannelCount, DWORD dwFlags, D_DMUS_AUDIOPARAMS *pParams ) = 0 ;
	virtual HRESULT __stdcall PlaySegmentEx				( D_IUnknown *pSource, WCHAR *pwzSegmentName, D_IUnknown *pTransition, DWORD dwFlags, __int64 i64StartTime, D_IDirectMusicSegmentState **ppSegmentState, D_IUnknown *pFrom, D_IUnknown *pAudioPath ) = 0 ;
	virtual HRESULT __stdcall StopEx					( D_IUnknown *pObjectToStop, __int64 i64StopTime, DWORD dwFlags ) = 0 ;
	virtual HRESULT __stdcall NonUse34					( void ) = 0 ;				// ClonePMsg( DMUS_PMSG* pSourcePMSG, DMUS_PMSG** ppCopyPMSG ) = 0 ;
	virtual HRESULT __stdcall NonUse35					( void ) = 0 ;				// CreateAudioPath( D_IUnknown *pSourceConfig, BOOL fActivate, IDirectMusicAudioPath **ppNewPath ) = 0 ;
	virtual HRESULT __stdcall NonUse36					( void ) = 0 ;				// CreateStandardAudioPath( DWORD dwType, DWORD dwPChannelCount, BOOL fActivate, IDirectMusicAudioPath **ppNewPath ) = 0 ;
	virtual HRESULT __stdcall NonUse37					( void ) = 0 ;				// SetDefaultAudioPath( IDirectMusicAudioPath *pAudioPath ) = 0 ;
	virtual HRESULT __stdcall GetDefaultAudioPath		( D_IDirectMusicAudioPath **ppAudioPath ) = 0 ;
	virtual HRESULT __stdcall NonUse39					( void ) = 0 ;				// GetParamEx( REFGUID rguidType, DWORD dwTrackID, DWORD dwGroupBits, DWORD dwIndex, MUSIC_TIME mtTime, MUSIC_TIME* pmtNext, void* pParam ) = 0 ;
} ;

// ＤｉｒｅｃｔＤｒａｗ -------------------------------------------------------

#define D_DD_OK									S_OK

#define D_DD_ROP_SPACE							(256/32)
#define D_MAX_DDDEVICEID_STRING					(512)

#define D_DDPF_ALPHAPIXELS						(0x00000001l)
#define D_DDPF_ALPHA							(0x00000002l)
#define D_DDPF_FOURCC							(0x00000004l)
#define D_DDPF_RGB								(0x00000040l)
#define D_DDPF_ZBUFFER							(0x00000400l)
#define D_DDPF_LUMINANCE						(0x00020000l)
#define D_DDPF_BUMPLUMINANCE					(0x00040000l)
#define D_DDPF_BUMPDUDV							(0x00080000l)
#define D_DDPF_RGBTOYUV							(0x000001001)
#define D_DDPF_YUV								(0x000002001)

#define D_DDWAITVB_BLOCKBEGIN					(0x00000001l)

#define D_DDCAPS_ALIGNBOUNDARYDEST				(0x00000002l)
#define D_DDCAPS_ALIGNSIZEDEST					(0x00000004l)
#define D_DDCAPS_ALIGNBOUNDARYSRC				(0x00000008l)
#define D_DDCAPS_ALIGNSIZESRC					(0x00000010l)

#define D_DDENUMSURFACES_ALL					(0x00000001l)
#define D_DDENUMSURFACES_MATCH					(0x00000002l)
#define D_DDENUMSURFACES_NOMATCH				(0x00000004l)
#define D_DDENUMSURFACES_CANBECREATED			(0x00000008l)
#define D_DDENUMSURFACES_DOESEXIST				(0x00000010l)

#define D_DDOVER_SHOW							(0x00004000l)
#define D_DDOVER_HIDE							(0x00000200l)

#define D_DDSCAPS_BACKBUFFER					(0x00000004l)
#define D_DDSCAPS_COMPLEX						(0x00000008l)
#define D_DDSCAPS_FLIP							(0x00000010l)
#define D_DDSCAPS_OFFSCREENPLAIN				(0x00000040l)
#define D_DDSCAPS_OVERLAY						(0x00000080l)
#define D_DDSCAPS_PRIMARYSURFACE				(0x00000200l)
#define D_DDSCAPS_SYSTEMMEMORY					(0x00000800l)
#define D_DDSCAPS_TEXTURE						(0x00001000l)
#define D_DDSCAPS_3DDEVICE						(0x00002000l)
#define D_DDSCAPS_VIDEOMEMORY					(0x00004000l)
#define D_DDSCAPS_ZBUFFER						(0x00020000l)
#define D_DDSCAPS_MIPMAP						(0x00400000l)
#define D_DDSCAPS_LOCALVIDMEM					(0x10000000l)

#define D_DDCKEY_SRCBLT							(0x00000008l)

#define D_DDFLIP_NOVSYNC						(0x00000008L)
#define D_DDFLIP_WAIT							(0x00000001L)

#define D_DDPCAPS_8BIT							(0x00000004l)

#define D_DDSCAPS2_RESERVED4					(0x00000002L)
#define D_DDSCAPS2_HARDWAREDEINTERLACE			(0x00000000L)
#define D_DDSCAPS2_HINTDYNAMIC					(0x00000004L)
#define D_DDSCAPS2_HINTSTATIC					(0x00000008L)
#define D_DDSCAPS2_TEXTUREMANAGE				(0x00000010L)
#define D_DDSCAPS2_RESERVED1					(0x00000020L)
#define D_DDSCAPS2_RESERVED2					(0x00000040L)
#define D_DDSCAPS2_OPAQUE						(0x00000080L)
#define D_DDSCAPS2_HINTANTIALIASING				(0x00000100L)
#define D_DDSCAPS2_CUBEMAP						(0x00000200L)
#define D_DDSCAPS2_CUBEMAP_POSITIVEX			(0x00000400L)
#define D_DDSCAPS2_CUBEMAP_NEGATIVEX			(0x00000800L)
#define D_DDSCAPS2_CUBEMAP_POSITIVEY			(0x00001000L)
#define D_DDSCAPS2_CUBEMAP_NEGATIVEY			(0x00002000L)
#define D_DDSCAPS2_CUBEMAP_POSITIVEZ			(0x00004000L)
#define D_DDSCAPS2_CUBEMAP_NEGATIVEZ			(0x00008000L)
#define D_DDSCAPS2_CUBEMAP_ALLFACES				(	D_DDSCAPS2_CUBEMAP_POSITIVEX |\
													D_DDSCAPS2_CUBEMAP_NEGATIVEX |\
													D_DDSCAPS2_CUBEMAP_POSITIVEY |\
													D_DDSCAPS2_CUBEMAP_NEGATIVEY |\
													D_DDSCAPS2_CUBEMAP_POSITIVEZ |\
													D_DDSCAPS2_CUBEMAP_NEGATIVEZ )
#define D_DDSCAPS2_MIPMAPSUBLEVEL				(0x00010000L)
#define D_DDSCAPS2_D3DTEXTUREMANAGE				(0x00020000L)
#define D_DDSCAPS2_DONOTPERSIST					(0x00040000L)
#define D_DDSCAPS2_STEREOSURFACELEFT			(0x00080000L)
#define D_DDSCAPS2_VOLUME						(0x00200000L)
#define D_DDSCAPS2_NOTUSERLOCKABLE				(0x00400000L)
#define D_DDSCAPS2_POINTS						(0x00800000L)
#define D_DDSCAPS2_RTPATCHES					(0x01000000L)
#define D_DDSCAPS2_NPATCHES						(0x02000000L)
#define D_DDSCAPS2_RESERVED3					(0x04000000L)
#define D_DDSCAPS2_DISCARDBACKBUFFER			(0x10000000L)
#define D_DDSCAPS2_ENABLEALPHACHANNEL			(0x20000000L)
#define D_DDSCAPS2_EXTENDEDFORMATPRIMARY		(0x40000000L)
#define D_DDSCAPS2_ADDITIONALPRIMARY			(0x80000000L)
#define D_DDSCAPS3_MULTISAMPLE_MASK				(0x0000001FL)
#define D_DDSCAPS3_MULTISAMPLE_QUALITY_MASK		(0x000000E0L)
#define D_DDSCAPS3_MULTISAMPLE_QUALITY_SHIFT	(5)
#define D_DDSCAPS3_RESERVED1					(0x00000100L)
#define D_DDSCAPS3_RESERVED2					(0x00000200L)
#define D_DDSCAPS3_LIGHTWEIGHTMIPMAP			(0x00000400L)
#define D_DDSCAPS3_AUTOGENMIPMAP				(0x00000800L)
#define D_DDSCAPS3_DMAP							(0x00001000L)

#define D_DDBLT_COLORFILL						(0x00000400l)
#define D_DDBLT_WAIT							(0x01000000l)
#define D_DDBLT_DEPTHFILL						(0x02000000l)

#define D_DDENUM_ATTACHEDSECONDARYDEVICES		(0x00000001L)
#define D_DDENUMRET_CANCEL						(0)
#define D_DDENUMRET_OK							(1)

#define D_DDLOCK_WAIT							(0x00000001L)
#define D_DDLOCK_READONLY						(0x00000010L)
#define D_DDLOCK_WRITEONLY						(0x00000020L)
#define D_DDLOCK_NOSYSLOCK						(0x00000800L)

#define D_DDBLTFAST_NOCOLORKEY					(0x00000000)
#define D_DDBLTFAST_SRCCOLORKEY					(0x00000001)
#define D_DDBLTFAST_DESTCOLORKEY				(0x00000002)
#define D_DDBLTFAST_WAIT						(0x00000010)
#define D_DDBLTFAST_DONOTWAIT					(0x00000020)

#define D_DDSD_CAPS								(0x00000001l)
#define D_DDSD_HEIGHT							(0x00000002l)
#define D_DDSD_WIDTH							(0x00000004l)
#define D_DDSD_PITCH							(0x00000008l)
#define D_DDSD_BACKBUFFERCOUNT					(0x00000020l)
#define D_DDSD_ZBUFFERBITDEPTH					(0x00000040l)
#define D_DDSD_ALPHABITDEPTH					(0x00000080l)
#define D_DDSD_LPSURFACE						(0x00000800l)
#define D_DDSD_PIXELFORMAT						(0x00001000l)
#define D_DDSD_CKDESTOVERLAY					(0x00002000l)
#define D_DDSD_CKDESTBLT						(0x00004000l)
#define D_DDSD_CKSRCOVERLAY						(0x00008000l)
#define D_DDSD_CKSRCBLT							(0x00010000l)
#define D_DDSD_MIPMAPCOUNT						(0x00020000l)
#define D_DDSD_REFRESHRATE						(0x00040000l)
#define D_DDSD_LINEARSIZE						(0x00080000l)
#define D_DDSD_TEXTURESTAGE						(0x00100000l)
#define D_DDSD_FVF								(0x00200000l)
#define D_DDSD_SRCVBHANDLE						(0x00400000l)
#define D_DDSD_DEPTH							(0x00800000l)

#define D_DDBD_16								(0x00000400l)
#define D_DDBD_32								(0x00000100l)

#define D_DDSCL_FULLSCREEN						(0x00000001l)
#define D_DDSCL_NORMAL							(0x00000008l)
#define D_DDSCL_EXCLUSIVE						(0x00000010l)
#define D_DDSCL_MULTITHREADED					(0x00000400l)
#define D_DDSCL_FPUPRESERVE						(0x00001000l)

#define D_MAKE_DDHRESULT( code )				MAKE_HRESULT( 1, 0x876, code )
#define D_DDERR_SURFACEBUSY						D_MAKE_DDHRESULT( 430 )
#define D_DDERR_SURFACELOST						D_MAKE_DDHRESULT( 450 )
#define D_DDERR_HWNDSUBCLASSED					D_MAKE_DDHRESULT( 570 )
#define D_DDERR_HWNDALREADYSET					D_MAKE_DDHRESULT( 571 )
#define D_DDERR_EXCLUSIVEMODEALREADYSET			D_MAKE_DDHRESULT( 581 )

#define D_DDEDM_REFRESHRATES					(0x00000001l)
#define D_DDEDM_STANDARDVGAMODES				(0x00000002l)

typedef struct tagD_DDSCAPS2
{
	DWORD										dwCaps;
	DWORD										dwCaps2;
	DWORD										dwCaps3;
	union
	{
		DWORD									dwCaps4;
		DWORD									dwVolumeDepth;
	} ;
} D_DDSCAPS2 ;

typedef struct tagD_DDSCAPS
{
	DWORD										dwCaps;
} D_DDSCAPS ;

typedef struct tagD_DDCAPS
{
	DWORD										dwSize;
	DWORD										dwCaps;
	DWORD										dwCaps2;
	DWORD										dwCKeyCaps;
	DWORD										dwFXCaps;
	DWORD										dwFXAlphaCaps;
	DWORD										dwPalCaps;
	DWORD										dwSVCaps;
	DWORD										dwAlphaBltConstBitDepths;
	DWORD										dwAlphaBltPixelBitDepths;
	DWORD										dwAlphaBltSurfaceBitDepths;
	DWORD										dwAlphaOverlayConstBitDepths;
	DWORD										dwAlphaOverlayPixelBitDepths;
	DWORD										dwAlphaOverlaySurfaceBitDepths;
	DWORD										dwZBufferBitDepths;
	DWORD										dwVidMemTotal;
	DWORD										dwVidMemFree;
	DWORD										dwMaxVisibleOverlays;
	DWORD										dwCurrVisibleOverlays;
	DWORD										dwNumFourCCCodes;
	DWORD										dwAlignBoundarySrc;
	DWORD										dwAlignSizeSrc;
	DWORD										dwAlignBoundaryDest;
	DWORD										dwAlignSizeDest;
	DWORD										dwAlignStrideAlign;
	DWORD										dwRops[D_DD_ROP_SPACE];
	D_DDSCAPS									ddsOldCaps;
	DWORD										dwMinOverlayStretch;
	DWORD										dwMaxOverlayStretch;
	DWORD										dwMinLiveVideoStretch;
	DWORD										dwMaxLiveVideoStretch;
	DWORD										dwMinHwCodecStretch;
	DWORD										dwMaxHwCodecStretch;
	DWORD										dwReserved1;
	DWORD										dwReserved2;
	DWORD										dwReserved3;
	DWORD										dwSVBCaps;
	DWORD										dwSVBCKeyCaps;
	DWORD										dwSVBFXCaps;
	DWORD										dwSVBRops[D_DD_ROP_SPACE];
	DWORD										dwVSBCaps;
	DWORD										dwVSBCKeyCaps;
	DWORD										dwVSBFXCaps;
	DWORD										dwVSBRops[D_DD_ROP_SPACE];
	DWORD										dwSSBCaps;
	DWORD										dwSSBCKeyCaps;
	DWORD										dwSSBFXCaps;
	DWORD										dwSSBRops[D_DD_ROP_SPACE];
	DWORD										dwMaxVideoPorts;
	DWORD										dwCurrVideoPorts;
	DWORD										dwSVBCaps2;
	DWORD										dwNLVBCaps;
	DWORD										dwNLVBCaps2;
	DWORD										dwNLVBCKeyCaps;
	DWORD										dwNLVBFXCaps;
	DWORD										dwNLVBRops[D_DD_ROP_SPACE];
	// DirectX6
	D_DDSCAPS2									ddsCaps;
} D_DDCAPS ;

typedef struct tagD_DDPIXELFORMAT
{
	DWORD										dwSize;
	DWORD										dwFlags;
	DWORD										dwFourCC;
	union
	{
		DWORD									dwRGBBitCount;
		DWORD									dwYUVBitCount;
		DWORD									dwZBufferBitDepth;
		DWORD									dwAlphaBitDepth;
		DWORD									dwLuminanceBitCount;
		DWORD									dwBumpBitCount;
		DWORD									dwPrivateFormatBitCount;
	} ;
	union
	{
		DWORD									dwRBitMask;
		DWORD									dwYBitMask;
		DWORD									dwStencilBitDepth;
		DWORD									dwLuminanceBitMask;
		DWORD									dwBumpDuBitMask;
		DWORD									dwOperations;
	} ;
	union
	{
		DWORD									dwGBitMask;
		DWORD									dwUBitMask;
		DWORD									dwZBitMask;
		DWORD									dwBumpDvBitMask;
		struct
		{
			WORD								wFlipMSTypes;
			WORD								wBltMSTypes;
		} MultiSampleCaps;

	} ;
	union
	{
		DWORD									dwBBitMask;
		DWORD									dwVBitMask;
		DWORD									dwStencilBitMask;
		DWORD									dwBumpLuminanceBitMask;
	} ;
	union
	{
		DWORD									dwRGBAlphaBitMask;
		DWORD									dwYUVAlphaBitMask;
		DWORD									dwLuminanceAlphaBitMask;
		DWORD									dwRGBZBitMask;
		DWORD									dwYUVZBitMask;
	} ;
} D_DDPIXELFORMAT ;

typedef struct tagD_DDCOLORKEY
{
	DWORD										dwColorSpaceLowValue;
	DWORD										dwColorSpaceHighValue;
} D_DDCOLORKEY ;

typedef struct tagD_DDSURFACEDESC
{
	DWORD										dwSize;
	DWORD										dwFlags;
	DWORD										dwHeight;
	DWORD										dwWidth;
	union
	{
		LONG									lPitch;
		DWORD									dwLinearSize;
	} ;
	DWORD										dwBackBufferCount;
	union
	{
		DWORD									dwMipMapCount;
		DWORD									dwZBufferBitDepth;
		DWORD									dwRefreshRate;
	} ;
	DWORD										dwAlphaBitDepth;
	DWORD										dwReserved;
	LPVOID										lpSurface;
	D_DDCOLORKEY								ddckCKDestOverlay;
	D_DDCOLORKEY								ddckCKDestBlt;
	D_DDCOLORKEY								ddckCKSrcOverlay;
	D_DDCOLORKEY								ddckCKSrcBlt;
	D_DDPIXELFORMAT								ddpfPixelFormat;
	D_DDSCAPS									ddsCaps;
} D_DDSURFACEDESC ;

typedef struct tagD_DDSURFACEDESC2
{
	DWORD										dwSize;
	DWORD										dwFlags;
	DWORD										dwHeight;
	DWORD										dwWidth;
	union
	{
		LONG									lPitch;
		DWORD									dwLinearSize;
	} ;
	union
	{
		DWORD									dwBackBufferCount;
		DWORD									dwDepth;
	} ;
	union
	{
		DWORD									dwMipMapCount;
		DWORD									dwRefreshRate;
		DWORD									dwSrcVBHandle;
	} ;
	DWORD										dwAlphaBitDepth;
	DWORD										dwReserved;
	LPVOID										lpSurface;
	union
	{
		D_DDCOLORKEY							ddckCKDestOverlay;
		DWORD									dwEmptyFaceColor;
	} ;
	D_DDCOLORKEY								ddckCKDestBlt;
	D_DDCOLORKEY								ddckCKSrcOverlay;
	D_DDCOLORKEY								ddckCKSrcBlt;
	union
	{
		D_DDPIXELFORMAT							ddpfPixelFormat;
		DWORD									dwFVF;
	} ;
	D_DDSCAPS2									ddsCaps;
	DWORD										dwTextureStage;
} D_DDSURFACEDESC2 ;

typedef struct tagD_DDDEVICEIDENTIFIER2
{
	char										szDriver[D_MAX_DDDEVICEID_STRING];
	char										szDescription[D_MAX_DDDEVICEID_STRING];

	LARGE_INTEGER								liDriverVersion;

	DWORD										dwVendorId;
	DWORD										dwDeviceId;
	DWORD										dwSubSysId;
	DWORD										dwRevision;
	GUID										guidDeviceIdentifier;
	DWORD										dwWHQLLevel;

} D_DDDEVICEIDENTIFIER2 ;

typedef struct tagD_DDBLTFX
{
	DWORD										dwSize;
	DWORD										dwDDFX;
	DWORD										dwROP;
	DWORD										dwDDROP;
	DWORD										dwRotationAngle;
	DWORD										dwZBufferOpCode;
	DWORD										dwZBufferLow;
	DWORD										dwZBufferHigh;
	DWORD										dwZBufferBaseDest;
	DWORD										dwZDestConstBitDepth;
	union
	{
		DWORD									dwZDestConst;
		class D_IDirectDrawSurface 				*lpDDSZBufferDest;
	} ;
	DWORD										dwZSrcConstBitDepth;
	union
	{
		DWORD									dwZSrcConst;
		class D_IDirectDrawSurface 				*lpDDSZBufferSrc;
	} ;
	DWORD										dwAlphaEdgeBlendBitDepth;
	DWORD										dwAlphaEdgeBlend;
	DWORD										dwReserved;
	DWORD										dwAlphaDestConstBitDepth;
	union
	{
		DWORD									dwAlphaDestConst;
		class D_IDirectDrawSurface 				*lpDDSAlphaDest;
	} ;
	DWORD										dwAlphaSrcConstBitDepth;
	union
	{
		DWORD									dwAlphaSrcConst;
		class D_IDirectDrawSurface 				*lpDDSAlphaSrc;
	} ;
	union
	{
		DWORD									dwFillColor;
		DWORD									dwFillDepth;
		DWORD									dwFillPixel;
		class D_IDirectDrawSurface 				*lpDDSPattern;
	} ;
	D_DDCOLORKEY								ddckDestColorkey;
	D_DDCOLORKEY								ddckSrcColorkey;
} D_DDBLTFX ;

typedef struct tagD_DDOVERLAYFX
{
	DWORD										dwSize;
	DWORD										dwAlphaEdgeBlendBitDepth;
	DWORD										dwAlphaEdgeBlend;
	DWORD										dwReserved;
	DWORD										dwAlphaDestConstBitDepth;
	union
	{
		DWORD									dwAlphaDestConst;
		class D_IDirectDrawSurface 				*lpDDSAlphaDest;
	} ;
	DWORD										dwAlphaSrcConstBitDepth;
	union
	{
		DWORD									dwAlphaSrcConst;
		class D_IDirectDrawSurface 				*lpDDSAlphaSrc;
	} ;
	D_DDCOLORKEY								dckDestColorkey;
	D_DDCOLORKEY								dckSrcColorkey;
	DWORD										dwDDFX;
	DWORD										dwFlags;
} D_DDOVERLAYFX ;

typedef HRESULT ( FAR PASCAL * LPD_DDENUMSURFACESCALLBACK7 )	( class D_IDirectDrawSurface7 *, D_DDSURFACEDESC2 *, void * ) ;
typedef HRESULT ( FAR PASCAL * LPD_DDENUMSURFACESCALLBACK2 )	( class D_IDirectDrawSurface4 *, D_DDSURFACEDESC2 *, void * ) ;
typedef HRESULT ( FAR PASCAL * LPD_DDENUMSURFACESCALLBACK )		( class D_IDirectDrawSurface *,  D_DDSURFACEDESC *,  void * ) ;
typedef HRESULT ( FAR PASCAL * LPD_DDENUMMODESCALLBACK2)		( D_DDSURFACEDESC2 *, void * ) ;
typedef BOOL 	( FAR PASCAL * LPD_DDENUMCALLBACKEXA )			( GUID *, LPSTR,  LPSTR,  void *, void * ) ;
typedef BOOL 	( FAR PASCAL * LPD_DDENUMCALLBACKEXW )			( GUID *, LPWSTR, LPWSTR, void *, void * ) ;

class D_IDirectDraw : public D_IUnknown
{
public :
	virtual HRESULT __stdcall NonUse00					( void ) = 0 ;				// Compact( void ) = 0 ;
	virtual HRESULT __stdcall NonUse01					( void ) = 0 ;				// CreateClipper( DWORD, LPDIRECTDRAWCLIPPER FAR*, D_IUnknown FAR * ) = 0 ;
	virtual HRESULT __stdcall NonUse02					( void ) = 0 ;				// CreatePalette( DWORD, LPPALETTEENTRY, LPDIRECTDRAWPALETTE FAR*, D_IUnknown FAR * ) = 0 ;
	virtual HRESULT __stdcall CreateSurface				( D_DDSURFACEDESC *, D_IDirectDrawSurface **, D_IUnknown * ) = 0 ;
	virtual HRESULT __stdcall NonUse03					( void ) = 0 ;				// DuplicateSurface( LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE FAR * ) = 0 ;
	virtual HRESULT __stdcall NonUse04					( void ) = 0 ;				// EnumDisplayModes( DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMMODESCALLBACK ) = 0 ;
	virtual HRESULT __stdcall NonUse05					( void ) = 0 ;				// EnumSurfaces( DWORD, LPDDSURFACEDESC, LPVOID,LPDDENUMSURFACESCALLBACK ) = 0 ;
	virtual HRESULT __stdcall NonUse06					( void ) = 0 ;				// FlipToGDISurface( void ) = 0 ;
	virtual HRESULT __stdcall GetCaps					( D_DDCAPS *, D_DDCAPS * ) = 0 ;
	virtual HRESULT __stdcall NonUse07					( void ) = 0 ;				// GetDisplayMode( LPDDSURFACEDESC ) = 0 ;
	virtual HRESULT __stdcall NonUse08					( void ) = 0 ;				// GetFourCCCodes( LPDWORD, LPDWORD	) = 0 ;
	virtual HRESULT __stdcall NonUse09					( void ) = 0 ;				// GetGDISurface( LPDIRECTDRAWSURFACE FAR * ) = 0 ;
	virtual HRESULT __stdcall NonUse10					( void ) = 0 ;				// GetMonitorFrequency( LPDWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse11					( void ) = 0 ;				// GetScanLine( LPDWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse12					( void ) = 0 ;				// GetVerticalBlankStatus( LPBOOL ) = 0 ;
	virtual HRESULT __stdcall NonUse13					( void ) = 0 ;				// Initialize( GUID FAR * ) = 0 ;
	virtual HRESULT __stdcall NonUse14					( void ) = 0 ;				// RestoreDisplayMode( void ) = 0 ;
	virtual HRESULT __stdcall SetCooperativeLevel		( HWND, DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse15					( void ) = 0 ;				// SetDisplayMode( DWORD, DWORD,DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse16					( void ) = 0 ;				// WaitForVerticalBlank( DWORD, HANDLE ) = 0 ;
} ;

class D_IDirectDraw2 : public D_IDirectDraw
{
public :
	virtual HRESULT __stdcall GetAvailableVidMem		( D_DDSCAPS2 *, DWORD *, DWORD * ) = 0 ;
} ;

class D_IDirectDraw4 : public D_IUnknown
{
public :
	virtual HRESULT __stdcall NonUse00					( void ) = 0 ;				// Compact( void ) = 0 ;
	virtual HRESULT __stdcall NonUse01					( void ) = 0 ;				// CreateClipper( DWORD, LPDIRECTDRAWCLIPPER FAR*, D_IUnknown FAR * ) = 0 ;
	virtual HRESULT __stdcall NonUse02					( void ) = 0 ;				// CreatePalette( DWORD, LPPALETTEENTRY, LPDIRECTDRAWPALETTE FAR*, D_IUnknown FAR * ) = 0 ;
	virtual HRESULT __stdcall NonUse03					( void ) = 0 ;				// CreateSurface(	LPDDSURFACEDESC2, LPDIRECTDRAWSURFACE4 FAR *, D_IUnknown FAR * ) = 0 ;
	virtual HRESULT __stdcall NonUse04					( void ) = 0 ;				// DuplicateSurface( LPDIRECTDRAWSURFACE4, LPDIRECTDRAWSURFACE4 FAR * ) = 0 ;
	virtual HRESULT __stdcall NonUse05					( void ) = 0 ;				// EnumDisplayModes( DWORD, LPDDSURFACEDESC2, LPVOID, LPDDENUMMODESCALLBACK2 ) = 0 ;
	virtual HRESULT __stdcall NonUse06					( void ) = 0 ;				// EnumSurfaces( DWORD, LPDDSURFACEDESC2, LPVOID,LPDDENUMSURFACESCALLBACK2 ) = 0 ;
	virtual HRESULT __stdcall NonUse07					( void ) = 0 ;				// FlipToGDISurface( void ) = 0 ;
	virtual HRESULT __stdcall NonUse08					( void ) = 0 ;				// GetCaps( LPDDCAPS, LPDDCAPS ) = 0 ;
	virtual HRESULT __stdcall NonUse09					( void ) = 0 ;				// GetDisplayMode( LPDDSURFACEDESC2 ) = 0 ;
	virtual HRESULT __stdcall NonUse10					( void ) = 0 ;				// GetFourCCCodes( LPDWORD, LPDWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse11					( void ) = 0 ;				// GetGDISurface( LPDIRECTDRAWSURFACE4 FAR * ) = 0 ;
	virtual HRESULT __stdcall NonUse12					( void ) = 0 ;				// GetMonitorFrequency( LPDWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse13					( void ) = 0 ;				// GetScanLine( LPDWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse14					( void ) = 0 ;				// GetVerticalBlankStatus( LPBOOL ) = 0 ;
	virtual HRESULT __stdcall NonUse15					( void ) = 0 ;				// Initialize( GUID FAR * ) = 0 ;
	virtual HRESULT __stdcall NonUse16					( void ) = 0 ;				// RestoreDisplayMode( void ) = 0 ;
	virtual HRESULT __stdcall NonUse17					( void ) = 0 ;				// SetCooperativeLevel( HWND, DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse18					( void ) = 0 ;				// SetDisplayMode( DWORD, DWORD,DWORD, DWORD, DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse19					( void ) = 0 ;				// WaitForVerticalBlank( DWORD, HANDLE ) = 0 ;

	virtual HRESULT __stdcall NonUse20					( void ) = 0 ;				// GetAvailableVidMem( LPDDSCAPS2, LPDWORD, LPDWORD ) = 0 ;

	virtual HRESULT __stdcall NonUse21					( void ) = 0 ;				// GetSurfaceFromDC( HDC, LPDIRECTDRAWSURFACE4 * ) = 0 ;
	virtual HRESULT __stdcall NonUse22					( void ) = 0 ;				// RestoreAllSurfaces( void ) = 0 ;
	virtual HRESULT __stdcall NonUse23					( void ) = 0 ;				// TestCooperativeLevel( void ) = 0 ;
	virtual HRESULT __stdcall NonUse24					( void ) = 0 ;				// GetDeviceIdentifier( LPDDDEVICEIDENTIFIER, DWORD ) = 0 ;
} ;

class D_IDirectDraw7 : public D_IUnknown
{
public :
	virtual HRESULT __stdcall NonUse00					( void ) = 0 ;				// Compact( void ) = 0 ;
	virtual HRESULT __stdcall CreateClipper				( DWORD, class D_IDirectDrawClipper **, D_IUnknown * ) = 0 ;
	virtual HRESULT __stdcall CreatePalette				( DWORD, LPPALETTEENTRY, class D_IDirectDrawPalette **, D_IUnknown * ) = 0 ;
	virtual HRESULT __stdcall CreateSurface				( D_DDSURFACEDESC2 *, D_IDirectDrawSurface7 **, D_IUnknown * ) = 0 ;
	virtual HRESULT __stdcall NonUse01					( void ) = 0 ;				// DuplicateSurface( D_IDirectDrawSurface7 *, D_IDirectDrawSurface7 * FAR * ) = 0 ;
	virtual HRESULT __stdcall EnumDisplayModes			( DWORD, D_DDSURFACEDESC2 *, LPVOID, LPD_DDENUMMODESCALLBACK2 ) = 0 ;
	virtual HRESULT __stdcall EnumSurfaces				( DWORD, D_DDSURFACEDESC2 *, LPVOID, LPD_DDENUMSURFACESCALLBACK7 ) = 0 ;
	virtual HRESULT __stdcall FlipToGDISurface			( void ) = 0 ;
	virtual HRESULT __stdcall GetCaps					( D_DDCAPS *, D_DDCAPS * ) = 0 ;
	virtual HRESULT __stdcall GetDisplayMode			( D_DDSURFACEDESC2 * ) = 0 ;
	virtual HRESULT __stdcall NonUse04					( void ) = 0 ;				// GetFourCCCodes( LPDWORD, LPDWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse05					( void ) = 0 ;				// GetGDISurface( D_IDirectDrawSurface7 * FAR * ) = 0 ;
	virtual HRESULT __stdcall NonUse06					( void ) = 0 ;				// GetMonitorFrequency( LPDWORD ) = 0 ;
	virtual HRESULT __stdcall GetScanLine				( DWORD * ) = 0 ;
	virtual HRESULT __stdcall GetVerticalBlankStatus	( BOOL * ) = 0 ;
	virtual HRESULT __stdcall Initialize				( GUID * ) = 0 ;
	virtual HRESULT __stdcall RestoreDisplayMode		( void ) = 0 ;
	virtual HRESULT __stdcall SetCooperativeLevel		( HWND, DWORD ) = 0 ;
	virtual HRESULT __stdcall SetDisplayMode			( DWORD, DWORD,DWORD, DWORD, DWORD ) = 0 ;
	virtual HRESULT __stdcall WaitForVerticalBlank		( DWORD, HANDLE ) = 0 ;

	virtual HRESULT __stdcall GetAvailableVidMem		( D_DDSCAPS2 *, DWORD *, DWORD * ) = 0 ;

	virtual HRESULT __stdcall NonUse07					( void ) = 0 ;				// GetSurfaceFromDC( HDC, D_IDirectDrawSurface7 * * ) = 0 ;
	virtual HRESULT __stdcall NonUse08					( void ) = 0 ;				// RestoreAllSurfaces( void ) = 0 ;
	virtual HRESULT __stdcall NonUse09					( void ) = 0 ;				// TestCooperativeLevel( void ) = 0 ;
	virtual HRESULT __stdcall GetDeviceIdentifier		( D_DDDEVICEIDENTIFIER2 *, DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse10					( void ) = 0 ;				// StartModeTest( LPSIZE, DWORD, DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse11					( void ) = 0 ;				// EvaluateMode( DWORD, DWORD * ) = 0 ;
} ;

class D_IDirectDrawSurface : public D_IUnknown
{
public :
	virtual HRESULT __stdcall NonUse00					( void ) = 0 ;				// AddAttachedSurface( LPDIRECTDRAWSURFACE ) = 0 ;
	virtual HRESULT __stdcall NonUse01					( void ) = 0 ;				// AddOverlayDirtyRect( LPRECT ) = 0 ;
	virtual HRESULT __stdcall NonUse02					( void ) = 0 ;				// Blt( LPRECT,LPDIRECTDRAWSURFACE, LPRECT,DWORD, LPDDBLTFX ) = 0 ;
	virtual HRESULT __stdcall NonUse03					( void ) = 0 ;				// BltBatch( LPDDBLTBATCH, DWORD, DWORD	) = 0 ;
	virtual HRESULT __stdcall BltFast					( DWORD, DWORD, D_IDirectDrawSurface *, LPRECT, DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse04					( void ) = 0 ;				// DeleteAttachedSurface( DWORD,LPDIRECTDRAWSURFACE ) = 0 ;
	virtual HRESULT __stdcall NonUse05					( void ) = 0 ;				// EnumAttachedSurfaces( LPVOID,LPDDENUMSURFACESCALLBACK ) = 0 ;
	virtual HRESULT __stdcall NonUse06					( void ) = 0 ;				// EnumOverlayZOrders( DWORD,LPVOID,LPDDENUMSURFACESCALLBACK7 ) = 0 ;
	virtual HRESULT __stdcall NonUse07					( void ) = 0 ;				// Flip( LPDIRECTDRAWSURFACE, DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse08					( void ) = 0 ;				// GetAttachedSurface( LPDDSCAPS, LPDIRECTDRAWSURFACE FAR * ) = 0 ;
	virtual HRESULT __stdcall NonUse09					( void ) = 0 ;				// GetBltStatus( DWORD ) = 0 ;
	virtual HRESULT __stdcall GetCaps					( D_DDSCAPS * ) = 0 ;
	virtual HRESULT __stdcall NonUse10					( void ) = 0 ;				// GetClipper( LPDIRECTDRAWCLIPPER FAR* ) = 0 ;
	virtual HRESULT __stdcall NonUse11					( void ) = 0 ;				// GetColorKey( DWORD, LPDDCOLORKEY ) = 0 ;
	virtual HRESULT __stdcall NonUse12					( void ) = 0 ;				// GetDC( HDC FAR * ) = 0 ;
	virtual HRESULT __stdcall NonUse13					( void ) = 0 ;				// GetFlipStatus( DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse14					( void ) = 0 ;				// GetOverlayPosition( LPLONG, LPLONG	) = 0 ;
	virtual HRESULT __stdcall NonUse15					( void ) = 0 ;				// GetPalette( LPDIRECTDRAWPALETTE FAR* ) = 0 ;
	virtual HRESULT __stdcall GetPixelFormat			( D_DDPIXELFORMAT * ) = 0 ;
	virtual HRESULT __stdcall GetSurfaceDesc			( D_DDSURFACEDESC * ) = 0 ;
	virtual HRESULT __stdcall Initialize				( D_IDirectDraw *, D_DDSURFACEDESC * ) = 0 ;
	virtual HRESULT __stdcall NonUse16					( void ) = 0 ;				// IsLost( void	) = 0 ;
	virtual HRESULT __stdcall Lock						( LPRECT, D_DDSURFACEDESC *, DWORD, HANDLE ) = 0 ;
	virtual HRESULT __stdcall NonUse17					( void ) = 0 ;				// ReleaseDC( HDC ) = 0 ;
	virtual HRESULT __stdcall Restore					( void ) = 0 ;
	virtual HRESULT __stdcall NonUse18					( void ) = 0 ;				// SetClipper( LPDIRECTDRAWCLIPPER ) = 0 ;
	virtual HRESULT __stdcall NonUse19					( void ) = 0 ;				// SetColorKey( DWORD, LPDDCOLORKEY ) = 0 ;
	virtual HRESULT __stdcall NonUse20					( void ) = 0 ;				// SetOverlayPosition( LONG, LONG	) = 0 ;
	virtual HRESULT __stdcall NonUse21					( void ) = 0 ;				// SetPalette( LPDIRECTDRAWPALETTE ) = 0 ;
	virtual HRESULT __stdcall Unlock					( LPRECT ) = 0 ;
	virtual HRESULT __stdcall NonUse22					( void ) = 0 ;				// UpdateOverlay( LPRECT, LPDIRECTDRAWSURFACE,LPRECT,DWORD, LPDDOVERLAYFX ) = 0 ;
	virtual HRESULT __stdcall NonUse23					( void ) = 0 ;				// UpdateOverlayDisplay( DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse24					( void ) = 0 ;				// UpdateOverlayZOrder( DWORD, LPDIRECTDRAWSURFACE ) = 0 ;
} ;

class D_IDirectDrawSurface2 : public D_IDirectDrawSurface
{
public :
	virtual HRESULT __stdcall NonUse25					( void ) = 0 ;				// GetDDInterface( LPVOID FAR * ) = 0 ;
	virtual HRESULT __stdcall PageLock					( DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse27					( void ) = 0 ;				// PageUnlock( DWORD ) = 0 ;
} ;

class D_IDirectDrawSurface3 : public D_IDirectDrawSurface2
{
public :
	virtual HRESULT __stdcall NonUse28					( void ) = 0 ;				// SetSurfaceDesc( LPDDSURFACEDESC2, DWORD ) = 0 ;
} ;

class D_IDirectDrawSurface4 : public D_IUnknown
{
public :
	virtual HRESULT __stdcall NonUse00					( void ) = 0 ;				// AddAttachedSurface( LPDIRECTDRAWSURFACE4 ) = 0 ;
	virtual HRESULT __stdcall NonUse01					( void ) = 0 ;				// AddOverlayDirtyRect( LPRECT ) = 0 ;
	virtual HRESULT __stdcall Blt						( LPRECT, D_IDirectDrawSurface4 *, LPRECT, DWORD, D_DDBLTFX * ) = 0 ;
	virtual HRESULT __stdcall NonUse02					( void ) = 0 ;				// BltBatch( LPDDBLTBATCH, DWORD, DWORD	) = 0 ;
	virtual HRESULT __stdcall NonUse03					( void ) = 0 ;				// BltFast( DWORD,DWORD,LPDIRECTDRAWSURFACE4, LPRECT,DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse04					( void ) = 0 ;				// DeleteAttachedSurface( DWORD,LPDIRECTDRAWSURFACE4 ) = 0 ;
	virtual HRESULT __stdcall NonUse05					( void ) = 0 ;				// EnumAttachedSurfaces( LPVOID,LPDDENUMSURFACESCALLBACK2 ) = 0 ;
	virtual HRESULT __stdcall NonUse06					( void ) = 0 ;				// EnumOverlayZOrders( DWORD,LPVOID,LPDDENUMSURFACESCALLBACK2 ) = 0 ;
	virtual HRESULT __stdcall NonUse07					( void ) = 0 ;				// Flip( LPDIRECTDRAWSURFACE4, DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse08					( void ) = 0 ;				// GetAttachedSurface( LPDDSCAPS2, LPDIRECTDRAWSURFACE4 FAR * ) = 0 ;
	virtual HRESULT __stdcall NonUse09					( void ) = 0 ;				// GetBltStatus( DWORD ) = 0 ;
	virtual HRESULT __stdcall GetCaps					( D_DDSCAPS2 * ) = 0 ;
	virtual HRESULT __stdcall NonUse10					( void ) = 0 ;				// GetClipper( LPDIRECTDRAWCLIPPER FAR* ) = 0 ;
	virtual HRESULT __stdcall NonUse11					( void ) = 0 ;				// GetColorKey( DWORD, LPDDCOLORKEY ) = 0 ;
	virtual HRESULT __stdcall NonUse12					( void ) = 0 ;				// GetDC( HDC FAR * ) = 0 ;
	virtual HRESULT __stdcall NonUse13					( void ) = 0 ;				// GetFlipStatus( DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse14					( void ) = 0 ;				// GetOverlayPosition( LPLONG, LPLONG	) = 0 ;
	virtual HRESULT __stdcall NonUse15					( void ) = 0 ;				// GetPalette( LPDIRECTDRAWPALETTE FAR* ) = 0 ;
	virtual HRESULT __stdcall NonUse16					( void ) = 0 ;				// GetPixelFormat( LPDDPIXELFORMAT ) = 0 ;
	virtual HRESULT __stdcall GetSurfaceDesc			( D_DDSURFACEDESC2 * ) = 0 ;
	virtual HRESULT __stdcall NonUse17					( void ) = 0 ;				// Initialize( LPDIRECTDRAW, LPDDSURFACEDESC2 ) = 0 ;
	virtual HRESULT __stdcall IsLost					( void ) = 0 ;
	virtual HRESULT __stdcall Lock						( LPRECT, D_DDSURFACEDESC2 *, DWORD, HANDLE ) = 0 ;
	virtual HRESULT __stdcall NonUse18					( void ) = 0 ;				// ReleaseDC( HDC ) = 0 ;
	virtual HRESULT __stdcall Restore					( void ) = 0 ;
	virtual HRESULT __stdcall NonUse19					( void ) = 0 ;				// SetClipper( LPDIRECTDRAWCLIPPER ) = 0 ;
	virtual HRESULT __stdcall NonUse20					( void ) = 0 ;				// SetColorKey( DWORD, LPDDCOLORKEY ) = 0 ;
	virtual HRESULT __stdcall NonUse21					( void ) = 0 ;				// SetOverlayPosition( LONG, LONG	) = 0 ;
	virtual HRESULT __stdcall NonUse22					( void ) = 0 ;				// SetPalette( LPDIRECTDRAWPALETTE ) = 0 ;
	virtual HRESULT __stdcall Unlock					( LPRECT ) = 0 ;
	virtual HRESULT __stdcall NonUse23					( void ) = 0 ;				// UpdateOverlay( LPRECT, LPDIRECTDRAWSURFACE4,LPRECT,DWORD, LPDDOVERLAYFX ) = 0 ;
	virtual HRESULT __stdcall NonUse24					( void ) = 0 ;				// UpdateOverlayDisplay( DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse25					( void ) = 0 ;				// UpdateOverlayZOrder( DWORD, LPDIRECTDRAWSURFACE4 ) = 0 ;

	virtual HRESULT __stdcall NonUse26					( void ) = 0 ;				// GetDDInterface( LPVOID FAR * ) = 0 ;
	virtual HRESULT __stdcall PageLock					( DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse28					( void ) = 0 ;				// PageUnlock( DWORD ) = 0 ;
	
	virtual HRESULT __stdcall NonUse29					( void ) = 0 ;				// SetSurfaceDesc( LPDDSURFACEDESC2, DWORD ) = 0 ;

	virtual HRESULT __stdcall NonUse30					( void ) = 0 ;				// SetPrivateData( REFGUID, LPVOID, DWORD, DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse31					( void ) = 0 ;				// GetPrivateData( REFGUID, LPVOID, LPDWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse32					( void ) = 0 ;				// FreePrivateData( REFGUID ) = 0 ;
	virtual HRESULT __stdcall NonUse33					( void ) = 0 ;				// GetUniquenessValue( LPDWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse34					( void ) = 0 ;				// ChangeUniquenessValue( void ) = 0 ;
} ;

class D_IDirectDrawSurface7 : public D_IUnknown
{
public :
	virtual HRESULT __stdcall AddAttachedSurface		( D_IDirectDrawSurface7 * ) = 0 ;
	virtual HRESULT __stdcall NonUse00					( void ) = 0 ;				// AddOverlayDirtyRect( LPRECT ) = 0 ;
	virtual HRESULT __stdcall Blt						( LPRECT, D_IDirectDrawSurface7 *, LPRECT, DWORD, D_DDBLTFX * ) = 0 ;
	virtual HRESULT __stdcall NonUse01					( void ) = 0 ;				// BltBatch( LPDDBLTBATCH, DWORD, DWORD	) = 0 ;
	virtual HRESULT __stdcall BltFast					( DWORD, DWORD, D_IDirectDrawSurface7 *, LPRECT, DWORD ) = 0 ;
	virtual HRESULT __stdcall DeleteAttachedSurface		( DWORD, D_IDirectDrawSurface7 * ) = 0 ;
	virtual HRESULT __stdcall EnumAttachedSurfaces		( LPVOID, LPD_DDENUMSURFACESCALLBACK7 ) = 0 ;
	virtual HRESULT __stdcall NonUse02					( void ) = 0 ;				// EnumOverlayZOrders( void ) = 0 ;		// EnumOverlayZOrders( DWORD,LPVOID,LPDDENUMSURFACESCALLBACK7 ) = 0 ;
	virtual HRESULT __stdcall Flip						( D_IDirectDrawSurface7 *, DWORD ) = 0 ;
	virtual HRESULT __stdcall GetAttachedSurface		( D_DDSCAPS2 *, D_IDirectDrawSurface7 ** ) = 0 ;
	virtual HRESULT __stdcall NonUse03					( void ) = 0 ;				// GetBltStatus( DWORD ) = 0 ;
	virtual HRESULT __stdcall GetCaps					( D_DDSCAPS2 * ) = 0 ;
	virtual HRESULT __stdcall NonUse04					( void ) = 0 ;				// GetClipper( LPDIRECTDRAWCLIPPER FAR* ) = 0 ;
	virtual HRESULT __stdcall GetColorKey				( DWORD, D_DDCOLORKEY * ) = 0 ;
	virtual HRESULT __stdcall GetDC						( HDC * ) = 0 ;
	virtual HRESULT __stdcall NonUse05					( void ) = 0 ;				// GetFlipStatus( DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse06					( void ) = 0 ;				// GetOverlayPosition( LPLONG, LPLONG	) = 0 ;
	virtual HRESULT __stdcall NonUse07					( void ) = 0 ;				// GetPalette( LPDIRECTDRAWPALETTE FAR* ) = 0 ;
	virtual HRESULT __stdcall NonUse08					( void ) = 0 ;				// GetPixelFormat( LPDDPIXELFORMAT ) = 0 ;
	virtual HRESULT __stdcall GetSurfaceDesc			( D_DDSURFACEDESC2 * ) = 0 ;
	virtual HRESULT __stdcall NonUse09					( void ) = 0 ;				// Initialize( LPDIRECTDRAW, LPDDSURFACEDESC2 ) = 0 ;
	virtual HRESULT __stdcall IsLost					( void ) = 0 ;
	virtual HRESULT __stdcall Lock						( LPRECT, D_DDSURFACEDESC2 *, DWORD, HANDLE ) = 0 ;
	virtual HRESULT __stdcall ReleaseDC					( HDC ) = 0 ;
	virtual HRESULT __stdcall Restore					( void	) = 0 ;
	virtual HRESULT __stdcall SetClipper				( class D_IDirectDrawClipper * ) = 0 ;
	virtual HRESULT __stdcall SetColorKey				( DWORD, D_DDCOLORKEY * ) = 0 ;
	virtual HRESULT __stdcall NonUse10					( void ) = 0 ;				// SetOverlayPosition( LONG, LONG	) = 0 ;
	virtual HRESULT __stdcall SetPalette				( class D_IDirectDrawPalette * ) = 0 ;
	virtual HRESULT __stdcall Unlock					( LPRECT ) = 0 ;
	virtual HRESULT __stdcall UpdateOverlay				( LPRECT, D_IDirectDrawSurface7 *, LPRECT, DWORD, D_DDOVERLAYFX * ) = 0 ;
	virtual HRESULT __stdcall NonUse11					( void ) = 0 ;				// UpdateOverlayDisplay( DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse12					( void ) = 0 ;				// UpdateOverlayZOrder( DWORD, D_IDirectDrawSurface7 * ) = 0 ;

	virtual HRESULT __stdcall NonUse13					( void ) = 0 ;				// GetDDInterface( LPVOID FAR * ) = 0 ;
	virtual HRESULT __stdcall PageLock					( DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse15					( void ) = 0 ;				// PageUnlock( DWORD ) = 0 ;

	virtual HRESULT __stdcall NonUse16					( void ) = 0 ;				// SetSurfaceDesc( LPDDSURFACEDESC2, DWORD ) = 0 ;

	virtual HRESULT __stdcall NonUse17					( void ) = 0 ;				// SetPrivateData( REFGUID, LPVOID, DWORD, DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse18					( void ) = 0 ;				// GetPrivateData( REFGUID, LPVOID, LPDWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse19					( void ) = 0 ;				// FreePrivateData( REFGUID ) = 0 ;
	virtual HRESULT __stdcall NonUse20					( void ) = 0 ;				// GetUniquenessValue( LPDWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse21					( void ) = 0 ;				// ChangeUniquenessValue( void ) = 0 ;

	virtual HRESULT __stdcall NonUse22					( void ) = 0 ;				// SetPriority( DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse23					( void ) = 0 ;				// GetPriority( LPDWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse24					( void ) = 0 ;				// SetLOD( DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse25					( void ) = 0 ;				// GetLOD( LPDWORD ) = 0 ;
} ;

class D_IDirectDrawClipper : public D_IUnknown
{
public :
	virtual HRESULT __stdcall NonUse00					( void ) = 0 ;				// GetClipList( LPRECT, LPRGNDATA, LPDWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse01					( void ) = 0 ;				// GetHWnd( HWND FAR * ) = 0 ;
	virtual HRESULT __stdcall NonUse02					( void ) = 0 ;				// Initialize( LPDIRECTDRAW, DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse03					( void ) = 0 ;				// IsClipListChanged( BOOL FAR * ) = 0 ;
	virtual HRESULT __stdcall NonUse04					( void ) = 0 ;				// SetClipList( LPRGNDATA,DWORD ) = 0 ;
	virtual HRESULT __stdcall SetHWnd					( DWORD, HWND ) = 0 ;
} ;

class D_IDirectDrawPalette : public D_IUnknown 
{
public :
	virtual HRESULT __stdcall NonUse00					( void ) = 0 ;				// GetCaps( LPDWORD ) = 0 ;
	virtual HRESULT __stdcall GetEntries				( DWORD, DWORD, DWORD, LPPALETTEENTRY ) = 0 ;
	virtual HRESULT __stdcall NonUse01					( void ) = 0 ;				// Initialize( LPDIRECTDRAW, DWORD, LPPALETTEENTRY ) = 0 ;
	virtual HRESULT __stdcall SetEntries				( DWORD, DWORD, DWORD, LPPALETTEENTRY ) = 0 ;
} ;



// Ｄｉｒｅｃｔ３Ｄ１１ -------------------------------------------------------

#define D_D3D10_SHADER_DEBUG							(1 << 0)
#define D_D3D10_SHADER_SKIP_VALIDATION					(1 << 1)
#define D_D3D10_SHADER_SKIP_OPTIMIZATION				(1 << 2)
#define D_D3D10_SHADER_PACK_MATRIX_ROW_MAJOR			(1 << 3)
#define D_D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR			(1 << 4)
#define D_D3D10_SHADER_PARTIAL_PRECISION				(1 << 5)
#define D_D3D10_SHADER_FORCE_VS_SOFTWARE_NO_OPT			(1 << 6)
#define D_D3D10_SHADER_FORCE_PS_SOFTWARE_NO_OPT			(1 << 7)
#define D_D3D10_SHADER_NO_PRESHADER						(1 << 8)
#define D_D3D10_SHADER_AVOID_FLOW_CONTROL				(1 << 9)
#define D_D3D10_SHADER_PREFER_FLOW_CONTROL				(1 << 10)
#define D_D3D10_SHADER_ENABLE_STRICTNESS				(1 << 11)
#define D_D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY	(1 << 12)
#define D_D3D10_SHADER_IEEE_STRICTNESS					(1 << 13)
#define D_D3D10_SHADER_WARNINGS_ARE_ERRORS				(1 << 18)

#define D_D3D10_SHADER_OPTIMIZATION_LEVEL0				(1 << 14)
#define D_D3D10_SHADER_OPTIMIZATION_LEVEL1				0
#define D_D3D10_SHADER_OPTIMIZATION_LEVEL2				((1 << 14) | (1 << 15))
#define D_D3D10_SHADER_OPTIMIZATION_LEVEL3				(1 << 15)

typedef enum tagD_D3D_INCLUDE_TYPE
{
	D_D3D_INCLUDE_LOCAL							= 0,
	D_D3D_INCLUDE_SYSTEM						= ( D_D3D_INCLUDE_LOCAL + 1 ) ,
	D_D3D_INCLUDE_FORCE_DWORD					= 0x7fffffff
} D_D3D_INCLUDE_TYPE ;

#define D_DXGI_CPU_ACCESS_NONE					( 0 )
#define D_DXGI_CPU_ACCESS_DYNAMIC				( 1 )
#define D_DXGI_CPU_ACCESS_READ_WRITE			( 2 )
#define D_DXGI_CPU_ACCESS_SCRATCH				( 3 )
#define D_DXGI_CPU_ACCESS_FIELD					15
#define D_DXGI_USAGE_SHADER_INPUT				( 1L << (0 + 4) )
#define D_DXGI_USAGE_RENDER_TARGET_OUTPUT		( 1L << (1 + 4) )
#define D_DXGI_USAGE_BACK_BUFFER				( 1L << (2 + 4) )
#define D_DXGI_USAGE_SHARED						( 1L << (3 + 4) )
#define D_DXGI_USAGE_READ_ONLY					( 1L << (4 + 4) )
#define D_DXGI_USAGE_DISCARD_ON_PRESENT			( 1L << (5 + 4) )
#define D_DXGI_USAGE_UNORDERED_ACCESS			( 1L << (6 + 4) )
typedef UINT D_DXGI_USAGE ;

#define D_DXGI_MWA_NO_WINDOW_CHANGES			( 1 << 0 )
#define D_DXGI_MWA_NO_ALT_ENTER					( 1 << 1 )
#define D_DXGI_MWA_NO_PRINT_SCREEN				( 1 << 2 )
#define D_DXGI_MWA_VALID						( 0x7 )

#define	D_D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT		( 8 )
#define	D_D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT	( 128 )
#define D_D3D11_FLOAT32_MAX						( 3.402823466e+38f )

#define	D_D3D11_DEFAULT_STENCIL_READ_MASK		( 0xff )
#define	D_D3D11_DEFAULT_STENCIL_WRITE_MASK		( 0xff )

#define D_D3D11CalcSubresource( MipSlice, ArraySlice, MipLevels )		( (MipSlice) + (ArraySlice) * (MipLevels) )

typedef enum tagD_DXGI_SWAP_CHAIN_FLAG
{
	D_DXGI_SWAP_CHAIN_FLAG_NONPREROTATED		= 1,
	D_DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH	= 2,
	D_DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE		= 4,
} D_DXGI_SWAP_CHAIN_FLAG ;

typedef enum tagD_DXGI_SWAP_EFFECT
{
	D_DXGI_SWAP_EFFECT_DISCARD					= 0,
	D_DXGI_SWAP_EFFECT_SEQUENTIAL				= 1
} D_DXGI_SWAP_EFFECT;

typedef enum tagD_DXGI_RESIDENCY
{
	D_DXGI_RESIDENCY_FULLY_RESIDENT				= 1,
	D_DXGI_RESIDENCY_RESIDENT_IN_SHARED_MEMORY	= 2,
	D_DXGI_RESIDENCY_EVICTED_TO_DISK				= 3
} D_DXGI_RESIDENCY;

typedef enum tagD_DXGI_MODE_SCANLINE_ORDER
{
	D_DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED		= 0,
	D_DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE		= 1,
	D_DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST= 2,
	D_DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST= 3
} D_DXGI_MODE_SCANLINE_ORDER ;

typedef enum tagD_DXGI_MODE_SCALING
{
	D_DXGI_MODE_SCALING_UNSPECIFIED				= 0,
	D_DXGI_MODE_SCALING_CENTERED				= 1,
	D_DXGI_MODE_SCALING_STRETCHED				= 2
} D_DXGI_MODE_SCALING;

typedef enum tagD_DXGI_MODE_ROTATION
{
	D_DXGI_MODE_ROTATION_UNSPECIFIED			= 0,
	D_DXGI_MODE_ROTATION_IDENTITY				= 1,
	D_DXGI_MODE_ROTATION_ROTATE90				= 2,
	D_DXGI_MODE_ROTATION_ROTATE180				= 3,
	D_DXGI_MODE_ROTATION_ROTATE270				= 4
} D_DXGI_MODE_ROTATION ;

typedef enum tagD_DXGI_FORMAT
{
	D_DXGI_FORMAT_UNKNOWN						= 0,
	D_DXGI_FORMAT_R32G32B32A32_TYPELESS			= 1,
	D_DXGI_FORMAT_R32G32B32A32_FLOAT			= 2,
	D_DXGI_FORMAT_R32G32B32A32_UINT				= 3,
	D_DXGI_FORMAT_R32G32B32A32_SINT				= 4,
	D_DXGI_FORMAT_R32G32B32_TYPELESS			= 5,
	D_DXGI_FORMAT_R32G32B32_FLOAT				= 6,
	D_DXGI_FORMAT_R32G32B32_UINT				= 7,
	D_DXGI_FORMAT_R32G32B32_SINT				= 8,
	D_DXGI_FORMAT_R16G16B16A16_TYPELESS			= 9,
	D_DXGI_FORMAT_R16G16B16A16_FLOAT			= 10,
	D_DXGI_FORMAT_R16G16B16A16_UNORM			= 11,
	D_DXGI_FORMAT_R16G16B16A16_UINT				= 12,
	D_DXGI_FORMAT_R16G16B16A16_SNORM			= 13,
	D_DXGI_FORMAT_R16G16B16A16_SINT				= 14,
	D_DXGI_FORMAT_R32G32_TYPELESS				= 15,
	D_DXGI_FORMAT_R32G32_FLOAT					= 16,
	D_DXGI_FORMAT_R32G32_UINT					= 17,
	D_DXGI_FORMAT_R32G32_SINT					= 18,
	D_DXGI_FORMAT_R32G8X24_TYPELESS				= 19,
	D_DXGI_FORMAT_D32_FLOAT_S8X24_UINT			= 20,
	D_DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS		= 21,
	D_DXGI_FORMAT_X32_TYPELESS_G8X24_UINT		= 22,
	D_DXGI_FORMAT_R10G10B10A2_TYPELESS			= 23,
	D_DXGI_FORMAT_R10G10B10A2_UNORM				= 24,
	D_DXGI_FORMAT_R10G10B10A2_UINT				= 25,
	D_DXGI_FORMAT_R11G11B10_FLOAT				= 26,
	D_DXGI_FORMAT_R8G8B8A8_TYPELESS				= 27,
	D_DXGI_FORMAT_R8G8B8A8_UNORM				= 28,
	D_DXGI_FORMAT_R8G8B8A8_UNORM_SRGB			= 29,
	D_DXGI_FORMAT_R8G8B8A8_UINT					= 30,
	D_DXGI_FORMAT_R8G8B8A8_SNORM				= 31,
	D_DXGI_FORMAT_R8G8B8A8_SINT					= 32,
	D_DXGI_FORMAT_R16G16_TYPELESS				= 33,
	D_DXGI_FORMAT_R16G16_FLOAT					= 34,
	D_DXGI_FORMAT_R16G16_UNORM					= 35,
	D_DXGI_FORMAT_R16G16_UINT					= 36,
	D_DXGI_FORMAT_R16G16_SNORM					= 37,
	D_DXGI_FORMAT_R16G16_SINT					= 38,
	D_DXGI_FORMAT_R32_TYPELESS					= 39,
	D_DXGI_FORMAT_D32_FLOAT						= 40,
	D_DXGI_FORMAT_R32_FLOAT						= 41,
	D_DXGI_FORMAT_R32_UINT						= 42,
	D_DXGI_FORMAT_R32_SINT						= 43,
	D_DXGI_FORMAT_R24G8_TYPELESS				= 44,
	D_DXGI_FORMAT_D24_UNORM_S8_UINT				= 45,
	D_DXGI_FORMAT_R24_UNORM_X8_TYPELESS			= 46,
	D_DXGI_FORMAT_X24_TYPELESS_G8_UINT			= 47,
	D_DXGI_FORMAT_R8G8_TYPELESS					= 48,
	D_DXGI_FORMAT_R8G8_UNORM					= 49,
	D_DXGI_FORMAT_R8G8_UINT						= 50,
	D_DXGI_FORMAT_R8G8_SNORM					= 51,
	D_DXGI_FORMAT_R8G8_SINT						= 52,
	D_DXGI_FORMAT_R16_TYPELESS					= 53,
	D_DXGI_FORMAT_R16_FLOAT						= 54,
	D_DXGI_FORMAT_D16_UNORM						= 55,
	D_DXGI_FORMAT_R16_UNORM						= 56,
	D_DXGI_FORMAT_R16_UINT						= 57,
	D_DXGI_FORMAT_R16_SNORM						= 58,
	D_DXGI_FORMAT_R16_SINT						= 59,
	D_DXGI_FORMAT_R8_TYPELESS					= 60,
	D_DXGI_FORMAT_R8_UNORM						= 61,
	D_DXGI_FORMAT_R8_UINT						= 62,
	D_DXGI_FORMAT_R8_SNORM						= 63,
	D_DXGI_FORMAT_R8_SINT						= 64,
	D_DXGI_FORMAT_A8_UNORM						= 65,
	D_DXGI_FORMAT_R1_UNORM						= 66,
	D_DXGI_FORMAT_R9G9B9E5_SHAREDEXP			= 67,
	D_DXGI_FORMAT_R8G8_B8G8_UNORM				= 68,
	D_DXGI_FORMAT_G8R8_G8B8_UNORM				= 69,
	D_DXGI_FORMAT_BC1_TYPELESS					= 70,
	D_DXGI_FORMAT_BC1_UNORM						= 71,
	D_DXGI_FORMAT_BC1_UNORM_SRGB				= 72,
	D_DXGI_FORMAT_BC2_TYPELESS					= 73,
	D_DXGI_FORMAT_BC2_UNORM						= 74,
	D_DXGI_FORMAT_BC2_UNORM_SRGB				= 75,
	D_DXGI_FORMAT_BC3_TYPELESS					= 76,
	D_DXGI_FORMAT_BC3_UNORM						= 77,
	D_DXGI_FORMAT_BC3_UNORM_SRGB				= 78,
	D_DXGI_FORMAT_BC4_TYPELESS					= 79,
	D_DXGI_FORMAT_BC4_UNORM						= 80,
	D_DXGI_FORMAT_BC4_SNORM						= 81,
	D_DXGI_FORMAT_BC5_TYPELESS					= 82,
	D_DXGI_FORMAT_BC5_UNORM						= 83,
	D_DXGI_FORMAT_BC5_SNORM						= 84,
	D_DXGI_FORMAT_B5G6R5_UNORM					= 85,
	D_DXGI_FORMAT_B5G5R5A1_UNORM				= 86,
	D_DXGI_FORMAT_B8G8R8A8_UNORM				= 87,
	D_DXGI_FORMAT_B8G8R8X8_UNORM				= 88,
	D_DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM	= 89,
	D_DXGI_FORMAT_B8G8R8A8_TYPELESS				= 90,
	D_DXGI_FORMAT_B8G8R8A8_UNORM_SRGB			= 91,
	D_DXGI_FORMAT_B8G8R8X8_TYPELESS				= 92,
	D_DXGI_FORMAT_B8G8R8X8_UNORM_SRGB			= 93,
	D_DXGI_FORMAT_BC6H_TYPELESS					= 94,
	D_DXGI_FORMAT_BC6H_UF16						= 95,
	D_DXGI_FORMAT_BC6H_SF16						= 96,
	D_DXGI_FORMAT_BC7_TYPELESS					= 97,
	D_DXGI_FORMAT_BC7_UNORM						= 98,
	D_DXGI_FORMAT_BC7_UNORM_SRGB				= 99,
	D_DXGI_FORMAT_AYUV							= 100,
	D_DXGI_FORMAT_Y410							= 101,
	D_DXGI_FORMAT_Y416							= 102,
	D_DXGI_FORMAT_NV12							= 103,
	D_DXGI_FORMAT_P010							= 104,
	D_DXGI_FORMAT_P016							= 105,
	D_DXGI_FORMAT_420_OPAQUE					= 106,
	D_DXGI_FORMAT_YUY2							= 107,
	D_DXGI_FORMAT_Y210							= 108,
	D_DXGI_FORMAT_Y216							= 109,
	D_DXGI_FORMAT_NV11							= 110,
	D_DXGI_FORMAT_AI44							= 111,
	D_DXGI_FORMAT_IA44							= 112,
	D_DXGI_FORMAT_P8							= 113,
	D_DXGI_FORMAT_A8P8							= 114,
	D_DXGI_FORMAT_B4G4R4A4_UNORM				= 115,
	D_DXGI_FORMAT_FORCE_UINT					= 0xffffffff
} D_DXGI_FORMAT ;

#define	D_D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT				(14)

typedef enum tagD_D3D_DRIVER_TYPE
{
	D_D3D_DRIVER_TYPE_UNKNOWN					= 0,
	D_D3D_DRIVER_TYPE_HARDWARE					= ( D_D3D_DRIVER_TYPE_UNKNOWN + 1 ) ,
	D_D3D_DRIVER_TYPE_REFERENCE					= ( D_D3D_DRIVER_TYPE_HARDWARE + 1 ) ,
	D_D3D_DRIVER_TYPE_NULL						= ( D_D3D_DRIVER_TYPE_REFERENCE + 1 ) ,
	D_D3D_DRIVER_TYPE_SOFTWARE					= ( D_D3D_DRIVER_TYPE_NULL + 1 ) ,
	D_D3D_DRIVER_TYPE_WARP						= ( D_D3D_DRIVER_TYPE_SOFTWARE + 1 ) 
} D_D3D_DRIVER_TYPE ;

typedef enum tagD_D3D_FEATURE_LEVEL
{
	D_D3D_FEATURE_LEVEL_9_1						= 0x9100,
	D_D3D_FEATURE_LEVEL_9_2						= 0x9200,
	D_D3D_FEATURE_LEVEL_9_3						= 0x9300,
	D_D3D_FEATURE_LEVEL_10_0					= 0xa000,
	D_D3D_FEATURE_LEVEL_10_1					= 0xa100,
	D_D3D_FEATURE_LEVEL_11_0					= 0xb000,
	D_D3D_FEATURE_LEVEL_11_1					= 0xb100
} D_D3D_FEATURE_LEVEL ;

typedef enum tagD_D3D11_FORMAT_SUPPORT
{
	D_D3D11_FORMAT_SUPPORT_BUFFER				= 0x1,
	D_D3D11_FORMAT_SUPPORT_IA_VERTEX_BUFFER		= 0x2,
	D_D3D11_FORMAT_SUPPORT_IA_INDEX_BUFFER		= 0x4,
	D_D3D11_FORMAT_SUPPORT_SO_BUFFER			= 0x8,
	D_D3D11_FORMAT_SUPPORT_TEXTURE1D			= 0x10,
	D_D3D11_FORMAT_SUPPORT_TEXTURE2D			= 0x20,
	D_D3D11_FORMAT_SUPPORT_TEXTURE3D			= 0x40,
	D_D3D11_FORMAT_SUPPORT_TEXTURECUBE			= 0x80,
	D_D3D11_FORMAT_SUPPORT_SHADER_LOAD			= 0x100,
	D_D3D11_FORMAT_SUPPORT_SHADER_SAMPLE		= 0x200,
	D_D3D11_FORMAT_SUPPORT_SHADER_SAMPLE_COMPARISON = 0x400,
	D_D3D11_FORMAT_SUPPORT_SHADER_SAMPLE_MONO_TEXT = 0x800,
	D_D3D11_FORMAT_SUPPORT_MIP					= 0x1000,
	D_D3D11_FORMAT_SUPPORT_MIP_AUTOGEN			= 0x2000,
	D_D3D11_FORMAT_SUPPORT_RENDER_TARGET		= 0x4000,
	D_D3D11_FORMAT_SUPPORT_BLENDABLE			= 0x8000,
	D_D3D11_FORMAT_SUPPORT_DEPTH_STENCIL		= 0x10000,
	D_D3D11_FORMAT_SUPPORT_CPU_LOCKABLE			= 0x20000,
	D_D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE	= 0x40000,
	D_D3D11_FORMAT_SUPPORT_DISPLAY				= 0x80000,
	D_D3D11_FORMAT_SUPPORT_CAST_WITHIN_BIT_LAYOUT = 0x100000,
	D_D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET = 0x200000,
	D_D3D11_FORMAT_SUPPORT_MULTISAMPLE_LOAD		= 0x400000,
	D_D3D11_FORMAT_SUPPORT_SHADER_GATHER		= 0x800000,
	D_D3D11_FORMAT_SUPPORT_BACK_BUFFER_CAST		= 0x1000000,
	D_D3D11_FORMAT_SUPPORT_TYPED_UNORDERED_ACCESS_VIEW = 0x2000000,
	D_D3D11_FORMAT_SUPPORT_SHADER_GATHER_COMPARISON = 0x4000000,
} D_D3D11_FORMAT_SUPPORT;

typedef enum tagD_D3D11_CPU_ACCESS_FLAG
{
	D_D3D11_CPU_ACCESS_WRITE					= 0x10000L,
	D_D3D11_CPU_ACCESS_READ						= 0x20000L
} D_D3D11_CPU_ACCESS_FLAG ;

typedef enum tagD_D3D11_RESOURCE_DIMENSION
{
	D_D3D11_RESOURCE_DIMENSION_UNKNOWN			= 0,
	D_D3D11_RESOURCE_DIMENSION_BUFFER			= 1,
	D_D3D11_RESOURCE_DIMENSION_TEXTURE1D		= 2,
	D_D3D11_RESOURCE_DIMENSION_TEXTURE2D		= 3,
	D_D3D11_RESOURCE_DIMENSION_TEXTURE3D		= 4
} D_D3D11_RESOURCE_DIMENSION ;

typedef enum tagD_D3D11_RESOURCE_MISC_FLAG
{
	D_D3D11_RESOURCE_MISC_GENERATE_MIPS			= 0x1L,
	D_D3D11_RESOURCE_MISC_SHARED				= 0x2L,
	D_D3D11_RESOURCE_MISC_TEXTURECUBE			= 0x4L,
	D_D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS		= 0x10L,
	D_D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS	= 0x20L,
	D_D3D11_RESOURCE_MISC_BUFFER_STRUCTURED		= 0x40L,
	D_D3D11_RESOURCE_MISC_RESOURCE_CLAMP		= 0x80L,
	D_D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX		= 0x100L,
	D_D3D11_RESOURCE_MISC_GDI_COMPATIBLE		= 0x200L
} D_D3D11_RESOURCE_MISC_FLAG ;

typedef enum tagD_D3D11_SRV_DIMENSION
{
	D_D3D11_SRV_DIMENSION_UNKNOWN				= 0,
	D_D3D11_SRV_DIMENSION_BUFFER				= 1,
	D_D3D11_SRV_DIMENSION_TEXTURE1D				= 2,
	D_D3D11_SRV_DIMENSION_TEXTURE1DARRAY		= 3,
	D_D3D11_SRV_DIMENSION_TEXTURE2D				= 4,
	D_D3D11_SRV_DIMENSION_TEXTURE2DARRAY		= 5,
	D_D3D11_SRV_DIMENSION_TEXTURE2DMS			= 6,
	D_D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY		= 7,
	D_D3D11_SRV_DIMENSION_TEXTURE3D				= 8,
	D_D3D11_SRV_DIMENSION_TEXTURECUBE			= 9,
	D_D3D11_SRV_DIMENSION_TEXTURECUBEARRAY		= 10,
	D_D3D11_SRV_DIMENSION_BUFFEREX				= 11
} D_D3D11_SRV_DIMENSION ;

typedef enum tagD_D3D11_DSV_DIMENSION
{
	D_D3D11_DSV_DIMENSION_UNKNOWN				= 0,
	D_D3D11_DSV_DIMENSION_TEXTURE1D				= 1,
	D_D3D11_DSV_DIMENSION_TEXTURE1DARRAY		= 2,
	D_D3D11_DSV_DIMENSION_TEXTURE2D				= 3,
	D_D3D11_DSV_DIMENSION_TEXTURE2DARRAY		= 4,
	D_D3D11_DSV_DIMENSION_TEXTURE2DMS			= 5,
	D_D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY		= 6
} D_D3D11_DSV_DIMENSION ;

typedef enum tagD_D3D11_RTV_DIMENSION
{
	D_D3D11_RTV_DIMENSION_UNKNOWN				= 0,
	D_D3D11_RTV_DIMENSION_BUFFER				= 1,
	D_D3D11_RTV_DIMENSION_TEXTURE1D				= 2,
	D_D3D11_RTV_DIMENSION_TEXTURE1DARRAY		= 3,
	D_D3D11_RTV_DIMENSION_TEXTURE2D				= 4,
	D_D3D11_RTV_DIMENSION_TEXTURE2DARRAY		= 5,
	D_D3D11_RTV_DIMENSION_TEXTURE2DMS			= 6,
	D_D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY		= 7,
	D_D3D11_RTV_DIMENSION_TEXTURE3D				= 8
} D_D3D11_RTV_DIMENSION ;

typedef enum tagD_D3D11_UAV_DIMENSION
{
	D_D3D11_UAV_DIMENSION_UNKNOWN				= 0,
	D_D3D11_UAV_DIMENSION_BUFFER				= 1,
	D_D3D11_UAV_DIMENSION_TEXTURE1D				= 2,
	D_D3D11_UAV_DIMENSION_TEXTURE1DARRAY		= 3,
	D_D3D11_UAV_DIMENSION_TEXTURE2D				= 4,
	D_D3D11_UAV_DIMENSION_TEXTURE2DARRAY		= 5,
	D_D3D11_UAV_DIMENSION_TEXTURE3D				= 8
} D_D3D11_UAV_DIMENSION ;

typedef enum tagD_D3D11_USAGE
{
	D_D3D11_USAGE_DEFAULT						= 0,
	D_D3D11_USAGE_IMMUTABLE						= 1,
	D_D3D11_USAGE_DYNAMIC						= 2,
	D_D3D11_USAGE_STAGING						= 3
} D_D3D11_USAGE ;

typedef enum tagD_D3D11_BIND_FLAG
{
	D_D3D11_BIND_VERTEX_BUFFER					= 0x1L,
	D_D3D11_BIND_INDEX_BUFFER					= 0x2L,
	D_D3D11_BIND_CONSTANT_BUFFER				= 0x4L,
	D_D3D11_BIND_SHADER_RESOURCE				= 0x8L,
	D_D3D11_BIND_STREAM_OUTPUT					= 0x10L,
	D_D3D11_BIND_RENDER_TARGET					= 0x20L,
	D_D3D11_BIND_DEPTH_STENCIL					= 0x40L,
	D_D3D11_BIND_UNORDERED_ACCESS				= 0x80L
} D_D3D11_BIND_FLAG ;

typedef enum tagD_D3D11_INPUT_CLASSIFICATION
{
	D_D3D11_INPUT_PER_VERTEX_DATA				= 0,
	D_D3D11_INPUT_PER_INSTANCE_DATA				= 1
} D_D3D11_INPUT_CLASSIFICATION;

#define	D_D3D11_APPEND_ALIGNED_ELEMENT			( 0xffffffff )

typedef enum tagD_D3D11_STANDARD_MULTISAMPLE_QUALITY_LEVELS
{
	D_D3D11_STANDARD_MULTISAMPLE_PATTERN		= 0xffffffff,
	D_D3D11_CENTER_MULTISAMPLE_PATTERN			= 0xfffffffe
} D_D3D11_STANDARD_MULTISAMPLE_QUALITY_LEVELS;

typedef enum tagD_D3D11_DEVICE_CONTEXT_TYPE
{
	D_D3D11_DEVICE_CONTEXT_IMMEDIATE			= 0,
	D_D3D11_DEVICE_CONTEXT_DEFERRED				= ( D_D3D11_DEVICE_CONTEXT_IMMEDIATE + 1 ) 
} D_D3D11_DEVICE_CONTEXT_TYPE;

typedef enum tagD_D3D11_CREATE_DEVICE_FLAG
{
	D_D3D11_CREATE_DEVICE_SINGLETHREADED		= 0x1,
	D_D3D11_CREATE_DEVICE_DEBUG					= 0x2,
	D_D3D11_CREATE_DEVICE_SWITCH_TO_REF			= 0x4,
	D_D3D11_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS = 0x8,
	D_D3D11_CREATE_DEVICE_BGRA_SUPPORT			= 0x20,
} D_D3D11_CREATE_DEVICE_FLAG ;

typedef enum tagD_D3D11_BLEND
{
	D_D3D11_BLEND_ZERO							= 1,
	D_D3D11_BLEND_ONE							= 2,
	D_D3D11_BLEND_SRC_COLOR						= 3,
	D_D3D11_BLEND_INV_SRC_COLOR					= 4,
	D_D3D11_BLEND_SRC_ALPHA						= 5,
	D_D3D11_BLEND_INV_SRC_ALPHA					= 6,
	D_D3D11_BLEND_DEST_ALPHA					= 7,
	D_D3D11_BLEND_INV_DEST_ALPHA				= 8,
	D_D3D11_BLEND_DEST_COLOR					= 9,
	D_D3D11_BLEND_INV_DEST_COLOR				= 10,
	D_D3D11_BLEND_SRC_ALPHA_SAT					= 11,
	D_D3D11_BLEND_BLEND_FACTOR					= 14,
	D_D3D11_BLEND_INV_BLEND_FACTOR				= 15,
	D_D3D11_BLEND_SRC1_COLOR					= 16,
	D_D3D11_BLEND_INV_SRC1_COLOR				= 17,
	D_D3D11_BLEND_SRC1_ALPHA					= 18,
	D_D3D11_BLEND_INV_SRC1_ALPHA				= 19
} D_D3D11_BLEND ;

typedef enum tagD_D3D11_BLEND_OP
{
	D_D3D11_BLEND_OP_ADD						= 1,
	D_D3D11_BLEND_OP_SUBTRACT					= 2,
	D_D3D11_BLEND_OP_REV_SUBTRACT				= 3,
	D_D3D11_BLEND_OP_MIN						= 4,
	D_D3D11_BLEND_OP_MAX						= 5
} D_D3D11_BLEND_OP ;

typedef enum tagD_D3D11_COMPARISON_FUNC
{
	D_D3D11_COMPARISON_NEVER					= 1,
	D_D3D11_COMPARISON_LESS						= 2,
	D_D3D11_COMPARISON_EQUAL					= 3,
	D_D3D11_COMPARISON_LESS_EQUAL				= 4,
	D_D3D11_COMPARISON_GREATER					= 5,
	D_D3D11_COMPARISON_NOT_EQUAL				= 6,
	D_D3D11_COMPARISON_GREATER_EQUAL			= 7,
	D_D3D11_COMPARISON_ALWAYS					= 8
} D_D3D11_COMPARISON_FUNC ;

typedef enum tagD_D3D11_DEPTH_WRITE_MASK
{
	D_D3D11_DEPTH_WRITE_MASK_ZERO				= 0,
	D_D3D11_DEPTH_WRITE_MASK_ALL				= 1
} D_D3D11_DEPTH_WRITE_MASK ;

typedef enum D_D3D11_STENCIL_OP
{
	D_D3D11_STENCIL_OP_KEEP						= 1,
	D_D3D11_STENCIL_OP_ZERO						= 2,
	D_D3D11_STENCIL_OP_REPLACE					= 3,
	D_D3D11_STENCIL_OP_INCR_SAT					= 4,
	D_D3D11_STENCIL_OP_DECR_SAT					= 5,
	D_D3D11_STENCIL_OP_INVERT					= 6,
	D_D3D11_STENCIL_OP_INCR						= 7,
	D_D3D11_STENCIL_OP_DECR						= 8
} D_D3D11_STENCIL_OP ;

typedef enum tagD_D3D11_FILL_MODE
{
	D_D3D11_FILL_WIREFRAME						= 2,
	D_D3D11_FILL_SOLID							= 3
} D_D3D11_FILL_MODE ;

typedef enum tagD_D3D11_CULL_MODE
{
	D_D3D11_CULL_NONE							= 1,
	D_D3D11_CULL_FRONT							= 2,
	D_D3D11_CULL_BACK							= 3
} D_D3D11_CULL_MODE ;

typedef enum tagD_D3D11_PRIMITIVE_TOPOLOGY
{
	D_D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED					= 0,
	D_D3D11_PRIMITIVE_TOPOLOGY_POINTLIST					= 1,
	D_D3D11_PRIMITIVE_TOPOLOGY_LINELIST						= 2,
	D_D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP					= 3,
	D_D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST					= 4,
	D_D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP				= 5,
	D_D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ					= 10,
	D_D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ				= 11,
	D_D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ				= 12,
	D_D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ			= 13,
	D_D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST	= 33,
	D_D3D11_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST	= 34,
	D_D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST	= 35,
	D_D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST	= 36,
	D_D3D11_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST	= 37,
	D_D3D11_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST	= 38,
	D_D3D11_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST	= 39,
	D_D3D11_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST	= 40,
	D_D3D11_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST	= 41,
	D_D3D11_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST	= 42,
	D_D3D11_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST	= 43,
	D_D3D11_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST	= 44,
	D_D3D11_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST	= 45,
	D_D3D11_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST	= 46,
	D_D3D11_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST	= 47,
	D_D3D11_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST	= 48,
	D_D3D11_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST	= 49,
	D_D3D11_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST	= 50,
	D_D3D11_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST	= 51,
	D_D3D11_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST	= 52,
	D_D3D11_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST	= 53,
	D_D3D11_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST	= 54,
	D_D3D11_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST	= 55,
	D_D3D11_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST	= 56,
	D_D3D11_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST	= 57,
	D_D3D11_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST	= 58,
	D_D3D11_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST	= 59,
	D_D3D11_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST	= 60,
	D_D3D11_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST	= 61,
	D_D3D11_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST	= 62,
	D_D3D11_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST	= 63,
	D_D3D11_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST	= 64
} D_D3D11_PRIMITIVE_TOPOLOGY;

typedef enum tagD_D3D11_FILTER
{
	D_D3D11_FILTER_MIN_MAG_MIP_POINT							= 0,
	D_D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR						= 0x1,
	D_D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT				= 0x4,
	D_D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR						= 0x5,
	D_D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT						= 0x10,
	D_D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR				= 0x11,
	D_D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT						= 0x14,
	D_D3D11_FILTER_MIN_MAG_MIP_LINEAR							= 0x15,
	D_D3D11_FILTER_ANISOTROPIC									= 0x55,
	D_D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT					= 0x80,
	D_D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR			= 0x81,
	D_D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT	= 0x84,
	D_D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR			= 0x85,
	D_D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT			= 0x90,
	D_D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR	= 0x91,
	D_D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT			= 0x94,
	D_D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR				= 0x95,
	D_D3D11_FILTER_COMPARISON_ANISOTROPIC						= 0xd5
} D_D3D11_FILTER ;

typedef enum tagD_D3D11_FILTER_TYPE
{
	D_D3D11_FILTER_TYPE_POINT					= 0,
	D_D3D11_FILTER_TYPE_LINEAR					= 1
} D_D3D11_FILTER_TYPE ;

#define	D_D3D11_FILTER_TYPE_MASK				( 0x3 )
#define	D_D3D11_MIN_FILTER_SHIFT				( 4 )
#define	D_D3D11_MAG_FILTER_SHIFT				( 2 )
#define	D_D3D11_MIP_FILTER_SHIFT				( 0 )
#define	D_D3D11_COMPARISON_FILTERING_BIT		( 0x80 )
#define	D_D3D11_ANISOTROPIC_FILTERING_BIT		( 0x40 )
#define D_D3D11_ENCODE_BASIC_FILTER( min, mag, mip, bComparison )												\
									( ( D_D3D11_FILTER ) (														\
									( ( bComparison ) ? D_D3D11_COMPARISON_FILTERING_BIT : 0 ) |				\
									( ( ( min ) & D_D3D11_FILTER_TYPE_MASK ) << D_D3D11_MIN_FILTER_SHIFT ) |	\
									( ( ( mag ) & D_D3D11_FILTER_TYPE_MASK ) << D_D3D11_MAG_FILTER_SHIFT ) |	\
									( ( ( mip ) & D_D3D11_FILTER_TYPE_MASK ) << D_D3D11_MIP_FILTER_SHIFT ) ) )   
#define D_D3D11_ENCODE_ANISOTROPIC_FILTER( bComparison )										\
									( ( D_D3D11_FILTER ) (										\
									D_D3D11_ANISOTROPIC_FILTERING_BIT |							\
									D_D3D11_ENCODE_BASIC_FILTER( D_D3D11_FILTER_TYPE_LINEAR,	\
														D_D3D11_FILTER_TYPE_LINEAR,				\
														D_D3D11_FILTER_TYPE_LINEAR,				\
														bComparison ) ) )                   
#define D_D3D11_DECODE_MIN_FILTER( d3d11Filter )		( ( D_D3D11_FILTER_TYPE )( ( ( d3d11Filter ) >> D_D3D11_MIN_FILTER_SHIFT ) & D_D3D11_FILTER_TYPE_MASK ) ) 
#define D_D3D11_DECODE_MAG_FILTER( d3d11Filter )		( ( D_D3D11_FILTER_TYPE )( ( ( d3d11Filter ) >> D_D3D11_MAG_FILTER_SHIFT ) & D_D3D11_FILTER_TYPE_MASK ) ) 
#define D_D3D11_DECODE_MIP_FILTER( d3d11Filter )		( ( D_D3D11_FILTER_TYPE )( ( ( d3d11Filter ) >> D_D3D11_MIP_FILTER_SHIFT ) & D_D3D11_FILTER_TYPE_MASK ) ) 
#define D_D3D11_DECODE_IS_COMPARISON_FILTER( d3d11Filter )		( ( d3d11Filter ) & D_D3D11_COMPARISON_FILTERING_BIT )                         
#define D_D3D11_DECODE_IS_ANISOTROPIC_FILTER( d3d11Filter )															\
									( ( ( d3d11Filter ) & D_D3D11_ANISOTROPIC_FILTERING_BIT ) &&					\
									( D_D3D11_FILTER_TYPE_LINEAR == D_D3D11_DECODE_MIN_FILTER( d3d11Filter ) ) &&   \
									( D_D3D11_FILTER_TYPE_LINEAR == D_D3D11_DECODE_MAG_FILTER( d3d11Filter ) ) &&   \
									( D_D3D11_FILTER_TYPE_LINEAR == D_D3D11_DECODE_MIP_FILTER( d3d11Filter ) ) )      

typedef enum tagD_D3D11_TEXTURE_ADDRESS_MODE
{
	D_D3D11_TEXTURE_ADDRESS_WRAP				= 1,
	D_D3D11_TEXTURE_ADDRESS_MIRROR				= 2,
	D_D3D11_TEXTURE_ADDRESS_CLAMP				= 3,
	D_D3D11_TEXTURE_ADDRESS_BORDER				= 4,
	D_D3D11_TEXTURE_ADDRESS_MIRROR_ONCE			= 5
} D_D3D11_TEXTURE_ADDRESS_MODE ;

typedef enum tagD_D3D11_ASYNC_GETDATA_FLAG
{
	D_D3D11_ASYNC_GETDATA_DONOTFLUSH			= 0x1
} D_D3D11_ASYNC_GETDATA_FLAG ;

typedef enum tagD_D3D11_QUERY
{
	D_D3D11_QUERY_EVENT							= 0,
	D_D3D11_QUERY_OCCLUSION						= ( D_D3D11_QUERY_EVENT + 1 ) ,
	D_D3D11_QUERY_TIMESTAMP						= ( D_D3D11_QUERY_OCCLUSION + 1 ) ,
	D_D3D11_QUERY_TIMESTAMP_DISJOINT			= ( D_D3D11_QUERY_TIMESTAMP + 1 ) ,
	D_D3D11_QUERY_PIPELINE_STATISTICS			= ( D_D3D11_QUERY_TIMESTAMP_DISJOINT + 1 ) ,
	D_D3D11_QUERY_OCCLUSION_PREDICATE			= ( D_D3D11_QUERY_PIPELINE_STATISTICS + 1 ) ,
	D_D3D11_QUERY_SO_STATISTICS					= ( D_D3D11_QUERY_OCCLUSION_PREDICATE + 1 ) ,
	D_D3D11_QUERY_SO_OVERFLOW_PREDICATE			= ( D_D3D11_QUERY_SO_STATISTICS + 1 ) ,
	D_D3D11_QUERY_SO_STATISTICS_STREAM0			= ( D_D3D11_QUERY_SO_OVERFLOW_PREDICATE + 1 ) ,
	D_D3D11_QUERY_SO_OVERFLOW_PREDICATE_STREAM0	= ( D_D3D11_QUERY_SO_STATISTICS_STREAM0 + 1 ) ,
	D_D3D11_QUERY_SO_STATISTICS_STREAM1			= ( D_D3D11_QUERY_SO_OVERFLOW_PREDICATE_STREAM0 + 1 ) ,
	D_D3D11_QUERY_SO_OVERFLOW_PREDICATE_STREAM1	= ( D_D3D11_QUERY_SO_STATISTICS_STREAM1 + 1 ) ,
	D_D3D11_QUERY_SO_STATISTICS_STREAM2			= ( D_D3D11_QUERY_SO_OVERFLOW_PREDICATE_STREAM1 + 1 ) ,
	D_D3D11_QUERY_SO_OVERFLOW_PREDICATE_STREAM2	= ( D_D3D11_QUERY_SO_STATISTICS_STREAM2 + 1 ) ,
	D_D3D11_QUERY_SO_STATISTICS_STREAM3			= ( D_D3D11_QUERY_SO_OVERFLOW_PREDICATE_STREAM2 + 1 ) ,
	D_D3D11_QUERY_SO_OVERFLOW_PREDICATE_STREAM3	= ( D_D3D11_QUERY_SO_STATISTICS_STREAM3 + 1 ) 
} D_D3D11_QUERY ;

typedef enum tagD_D3D11_QUERY_MISC_FLAG
{
	D_D3D11_QUERY_MISC_PREDICATEHINT			= 0x1
} D_D3D11_QUERY_MISC_FLAG ;

typedef enum tagD_D3D11_CLEAR_FLAG
{
	D_D3D11_CLEAR_DEPTH							= 0x1,
	D_D3D11_CLEAR_STENCIL						= 0x2
} D_D3D11_CLEAR_FLAG ;

typedef enum tagD_D3D11_COUNTER
{
	D_D3D11_COUNTER_DEVICE_DEPENDENT_0			= 0x40000000
} D_D3D11_COUNTER;

typedef enum tagD_D3D11_COUNTER_TYPE
{
	D_D3D11_COUNTER_TYPE_FLOAT32				= 0,
	D_D3D11_COUNTER_TYPE_UINT16					= ( D_D3D11_COUNTER_TYPE_FLOAT32 + 1 ) ,
	D_D3D11_COUNTER_TYPE_UINT32					= ( D_D3D11_COUNTER_TYPE_UINT16 + 1 ) ,
	D_D3D11_COUNTER_TYPE_UINT64					= ( D_D3D11_COUNTER_TYPE_UINT32 + 1 ) 
} D_D3D11_COUNTER_TYPE;

typedef enum tagD_D3D11_MAP
{
	D_D3D11_MAP_READ							= 1,
	D_D3D11_MAP_WRITE							= 2,
	D_D3D11_MAP_READ_WRITE						= 3,
	D_D3D11_MAP_WRITE_DISCARD					= 4,
	D_D3D11_MAP_WRITE_NO_OVERWRITE				= 5
} D_D3D11_MAP;

typedef enum tagD_D3D11_FEATURE
{
	D_D3D11_FEATURE_THREADING					= 0,
	D_D3D11_FEATURE_DOUBLES						= ( D_D3D11_FEATURE_THREADING + 1 ) ,
	D_D3D11_FEATURE_FORMAT_SUPPORT				= ( D_D3D11_FEATURE_DOUBLES + 1 ) ,
	D_D3D11_FEATURE_FORMAT_SUPPORT2				= ( D_D3D11_FEATURE_FORMAT_SUPPORT + 1 ) ,
	D_D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS	= ( D_D3D11_FEATURE_FORMAT_SUPPORT2 + 1 ) 
} D_D3D11_FEATURE;







typedef struct tagD_DXGI_RATIONAL
{
	UINT										Numerator ;
	UINT										Denominator ;
} D_DXGI_RATIONAL ;

typedef struct tagD_DXGI_ADAPTER_DESC
{
	WCHAR										Description[ 128 ] ;
	UINT										VendorId ;
	UINT										DeviceId ;
	UINT										SubSysId ;
	UINT										Revision ;
	SIZE_T										DedicatedVideoMemory ;
	SIZE_T										DedicatedSystemMemory ;
	SIZE_T										SharedSystemMemory ;
	LUID										AdapterLuid ;
} D_DXGI_ADAPTER_DESC ;

typedef struct tagD_DXGI_ADAPTER_DESC1
{
	WCHAR										Description[ 128 ] ;
	UINT										VendorId ;
	UINT										DeviceId ;
	UINT										SubSysId ;
	UINT										Revision ;
	SIZE_T										DedicatedVideoMemory ;
	SIZE_T										DedicatedSystemMemory ;
	SIZE_T										SharedSystemMemory ;
	LUID										AdapterLuid ;
	UINT										Flags ;
} D_DXGI_ADAPTER_DESC1 ;

typedef struct tagD_D3D11_TEX1D_DSV
{
	UINT										MipSlice ;
} D_D3D11_TEX1D_DSV ;

typedef struct tagD_D3D11_TEX1D_ARRAY_DSV
{
	UINT										MipSlice ;
	UINT										FirstArraySlice ;
	UINT										ArraySize ;
} D_D3D11_TEX1D_ARRAY_DSV ;

typedef struct tagD_D3D11_TEX2D_DSV
{
	UINT										MipSlice ;
} D_D3D11_TEX2D_DSV ;

typedef struct tagD_D3D11_TEX2D_ARRAY_DSV
{
	UINT										MipSlice ;
	UINT										FirstArraySlice ;
	UINT										ArraySize ;
} D_D3D11_TEX2D_ARRAY_DSV ;

typedef struct tagD_D3D11_TEX2DMS_DSV
{
	UINT										UnusedField_NothingToDefine ;
} D_D3D11_TEX2DMS_DSV ;

typedef struct tagD_D3D11_TEX2DMS_ARRAY_DSV
{
	UINT										FirstArraySlice ;
	UINT										ArraySize ;
} D_D3D11_TEX2DMS_ARRAY_DSV ;

typedef enum tagD_D3D11_DSV_FLAG
{
	D_D3D11_DSV_READ_ONLY_DEPTH					= 0x1L,
	D_D3D11_DSV_READ_ONLY_STENCIL				= 0x2L
} D_D3D11_DSV_FLAG ;

typedef struct tagD_D3D11_DEPTH_STENCIL_VIEW_DESC
{
	D_DXGI_FORMAT								Format ;
	D_D3D11_DSV_DIMENSION						ViewDimension ;
	UINT										Flags ;
	union 
	{
		D_D3D11_TEX1D_DSV						Texture1D ;
		D_D3D11_TEX1D_ARRAY_DSV					Texture1DArray ;
		D_D3D11_TEX2D_DSV						Texture2D ;
		D_D3D11_TEX2D_ARRAY_DSV					Texture2DArray ;
		D_D3D11_TEX2DMS_DSV						Texture2DMS ;
		D_D3D11_TEX2DMS_ARRAY_DSV				Texture2DMSArray ;
	} ;
} D_D3D11_DEPTH_STENCIL_VIEW_DESC ;

typedef struct tagD_DXGI_OUTPUT_DESC
{
	WCHAR										DeviceName[ 32 ] ;
	RECT										DesktopCoordinates ;
	BOOL										AttachedToDesktop ;
	D_DXGI_MODE_ROTATION						Rotation ;
	D_HMONITOR									Monitor ;
} D_DXGI_OUTPUT_DESC ;

typedef struct tagD_DXGI_RGB
{
	float										Red ;
	float										Green ;
	float										Blue ;
} D_DXGI_RGB;

typedef struct tagD_DXGI_GAMMA_CONTROL
{
	D_DXGI_RGB									Scale ;
	D_DXGI_RGB									Offset ;
	D_DXGI_RGB									GammaCurve[ 1025 ] ;
} D_DXGI_GAMMA_CONTROL ;

typedef struct tagD_DXGI_GAMMA_CONTROL_CAPABILITIES
{
	BOOL										ScaleAndOffsetSupported ;
	float										MaxConvertedValue ;
	float										MinConvertedValue ;
	UINT										NumGammaControlPoints ;
	float										ControlPointPositions[1025] ;
} D_DXGI_GAMMA_CONTROL_CAPABILITIES;

typedef struct tagD_DXGI_FRAME_STATISTICS
{
	UINT										PresentCount ;
	UINT										PresentRefreshCount ;
	UINT										SyncRefreshCount ;
	LARGE_INTEGER								SyncQPCTime ;
	LARGE_INTEGER								SyncGPUTime ;
} D_DXGI_FRAME_STATISTICS ;

typedef struct tagD_DXGI_MAPPED_RECT
{
	INT											Pitch ;
	BYTE *										pBits ;
} D_DXGI_MAPPED_RECT ;

typedef struct tagD_DXGI_SAMPLE_DESC
{
	UINT										Count ;
	UINT										Quality ;
} D_DXGI_SAMPLE_DESC ;

typedef struct tagD_DXGI_MODE_DESC
{
	UINT										Width ;
	UINT										Height ;
	D_DXGI_RATIONAL								RefreshRate ;
	D_DXGI_FORMAT								Format ;
	D_DXGI_MODE_SCANLINE_ORDER					ScanlineOrdering ;
	D_DXGI_MODE_SCALING							Scaling ;
} D_DXGI_MODE_DESC ;

typedef struct tagD_DXGI_SWAP_CHAIN_DESC
{
	D_DXGI_MODE_DESC							BufferDesc ;
	D_DXGI_SAMPLE_DESC							SampleDesc ;
	D_DXGI_USAGE								BufferUsage ;
	UINT										BufferCount ;
	HWND										OutputWindow ;
	BOOL										Windowed ;
	D_DXGI_SWAP_EFFECT							SwapEffect ;
	UINT										Flags ;
} D_DXGI_SWAP_CHAIN_DESC ;

typedef struct tagD_DXGI_SURFACE_DESC
{
	UINT										Width ;
	UINT										Height ;
	D_DXGI_FORMAT								Format ;
	D_DXGI_SAMPLE_DESC							SampleDesc ;
} D_DXGI_SURFACE_DESC ;

typedef struct tagD_DXGI_SHARED_RESOURCE
{
	HANDLE										Handle;
} D_DXGI_SHARED_RESOURCE;

typedef struct tagD_D3D11_TEXTURE1D_DESC
{
	UINT										Width ;
	UINT										MipLevels ;
	UINT										ArraySize ;
	D_DXGI_FORMAT								Format ;
	D_D3D11_USAGE								Usage ;
	UINT										BindFlags ;
	UINT										CPUAccessFlags ;
	UINT										MiscFlags ;
} D_D3D11_TEXTURE1D_DESC ;

typedef struct tagD_D3D11_TEXTURE2D_DESC
{
	UINT										Width ;
	UINT										Height ;
	UINT										MipLevels ;
	UINT										ArraySize ;
	D_DXGI_FORMAT								Format ;
	D_DXGI_SAMPLE_DESC							SampleDesc ;
	D_D3D11_USAGE								Usage ;
	UINT										BindFlags ;
	UINT										CPUAccessFlags ;
	UINT										MiscFlags ;
} D_D3D11_TEXTURE2D_DESC ;

typedef struct tagD_D3D11_TEXTURE3D_DESC
{
	UINT										Width ;
	UINT										Height ;
	UINT										Depth ;
	UINT										MipLevels ;
	D_DXGI_FORMAT								Format ;
	D_D3D11_USAGE								Usage ;
	UINT										BindFlags ;
	UINT										CPUAccessFlags ;
	UINT										MiscFlags ;
} D_D3D11_TEXTURE3D_DESC ;

typedef struct tagD_D3D11_BUFFER_SRV
{
	union 
	{
		UINT									FirstElement ;
		UINT									ElementOffset ;
	} ;

	union 
	{
		UINT									NumElements ;
		UINT									ElementWidth ;
	} ;
} D_D3D11_BUFFER_SRV;

typedef enum tagD_D3D11_BUFFEREX_SRV_FLAG
{
	D_D3D11_BUFFEREX_SRV_FLAG_RAW	= 0x1
} D_D3D11_BUFFEREX_SRV_FLAG;

typedef struct tagD_D3D11_BUFFEREX_SRV
{
	UINT										FirstElement ;
	UINT										NumElements ;
	UINT										Flags ;
} D_D3D11_BUFFEREX_SRV;

typedef struct tagD_D3D11_TEX1D_SRV
{
	UINT										MostDetailedMip ;
	UINT										MipLevels ;
} D_D3D11_TEX1D_SRV;

typedef struct tagD_D3D11_TEX1D_ARRAY_SRV
{
	UINT										MostDetailedMip ;
	UINT										MipLevels ;
	UINT										FirstArraySlice ;
	UINT										ArraySize ;
} D_D3D11_TEX1D_ARRAY_SRV;

typedef struct tagD_D3D11_TEX2D_SRV
{
	UINT										MostDetailedMip ;
	UINT										MipLevels ;
} D_D3D11_TEX2D_SRV;

typedef struct tagD_D3D11_TEX2D_ARRAY_SRV
{
	UINT										MostDetailedMip ;
	UINT										MipLevels ;
	UINT										FirstArraySlice ;
	UINT										ArraySize ;
} D_D3D11_TEX2D_ARRAY_SRV;

typedef struct tagD_D3D11_TEX3D_SRV
{
	UINT										MostDetailedMip ;
	UINT										MipLevels ;
} D_D3D11_TEX3D_SRV;

typedef struct tagD_D3D11_TEXCUBE_SRV
{
	UINT										MostDetailedMip ;
	UINT										MipLevels ;
} D_D3D11_TEXCUBE_SRV;

typedef struct tagD_D3D11_TEXCUBE_ARRAY_SRV
{
	UINT										MostDetailedMip ;
	UINT										MipLevels ;
	UINT										First2DArrayFace ;
	UINT										NumCubes ;
} D_D3D11_TEXCUBE_ARRAY_SRV;

typedef struct tagD_D3D11_TEX2DMS_SRV
{
	UINT										UnusedField_NothingToDefine ;
} D_D3D11_TEX2DMS_SRV;

typedef struct tagD_D3D11_TEX2DMS_ARRAY_SRV
{
	UINT										FirstArraySlice ;
	UINT										ArraySize ;
} D_D3D11_TEX2DMS_ARRAY_SRV;

typedef struct tagD_D3D11_SHADER_RESOURCE_VIEW_DESC
{
	D_DXGI_FORMAT								Format ;
	D_D3D11_SRV_DIMENSION						ViewDimension ;
	union 
	{
		D_D3D11_BUFFER_SRV						Buffer ;
		D_D3D11_TEX1D_SRV						Texture1D ;
		D_D3D11_TEX1D_ARRAY_SRV					Texture1DArray ;
		D_D3D11_TEX2D_SRV						Texture2D ;
		D_D3D11_TEX2D_ARRAY_SRV					Texture2DArray ;
		D_D3D11_TEX2DMS_SRV						Texture2DMS ;
		D_D3D11_TEX2DMS_ARRAY_SRV				Texture2DMSArray ;
		D_D3D11_TEX3D_SRV						Texture3D ;
		D_D3D11_TEXCUBE_SRV						TextureCube ;
		D_D3D11_TEXCUBE_ARRAY_SRV				TextureCubeArray ;
		D_D3D11_BUFFEREX_SRV					BufferEx ;
	} ;
} D_D3D11_SHADER_RESOURCE_VIEW_DESC;

typedef enum tagD_D3D11_BUFFER_UAV_FLAG
{
	D_D3D11_BUFFER_UAV_FLAG_RAW					= 0x1,
	D_D3D11_BUFFER_UAV_FLAG_APPEND				= 0x2,
	D_D3D11_BUFFER_UAV_FLAG_COUNTER				= 0x4
} D_D3D11_BUFFER_UAV_FLAG;

typedef struct tagD_D3D11_BUFFER_UAV
{
	UINT										FirstElement ;
	UINT										NumElements ;
	UINT										Flags ;
} D_D3D11_BUFFER_UAV ;

typedef struct tagD_D3D11_TEX1D_UAV
{
	UINT										MipSlice ;
} D_D3D11_TEX1D_UAV ;

typedef struct tagD_D3D11_TEX1D_ARRAY_UAV
{
	UINT										MipSlice ;
	UINT										FirstArraySlice ;
	UINT										ArraySize ;
} D_D3D11_TEX1D_ARRAY_UAV ;

typedef struct tagD_D3D11_TEX2D_UAV
{
	UINT										MipSlice ;
} D_D3D11_TEX2D_UAV ;

typedef struct tagD_D3D11_TEX2D_ARRAY_UAV
{
	UINT										MipSlice ;
	UINT										FirstArraySlice ;
	UINT										ArraySize ;
} D_D3D11_TEX2D_ARRAY_UAV ;

typedef struct tagD_D3D11_TEX3D_UAV
{
	UINT										MipSlice ;
	UINT										FirstWSlice ;
	UINT										WSize ;
} D_D3D11_TEX3D_UAV ;

typedef struct tagD_D3D11_UNORDERED_ACCESS_VIEW_DESC
{
	D_DXGI_FORMAT								Format ;
	D_D3D11_UAV_DIMENSION						ViewDimension ;
	union 
	{
		D_D3D11_BUFFER_UAV						Buffer ;
		D_D3D11_TEX1D_UAV						Texture1D ;
		D_D3D11_TEX1D_ARRAY_UAV					Texture1DArray ;
		D_D3D11_TEX2D_UAV						Texture2D ;
		D_D3D11_TEX2D_ARRAY_UAV					Texture2DArray ;
		D_D3D11_TEX3D_UAV						Texture3D ;
	}  ;
} D_D3D11_UNORDERED_ACCESS_VIEW_DESC ;

typedef struct tagD_D3D11_BUFFER_RTV
{
	union 
	{
		UINT									FirstElement ;
		UINT									ElementOffset ;
	} ;
	union 
	{
		UINT									NumElements ;
		UINT									ElementWidth ;
	} ;
} D_D3D11_BUFFER_RTV ;

typedef struct tagD_D3D11_TEX1D_RTV
{
	UINT										MipSlice ;
} D_D3D11_TEX1D_RTV ;

typedef struct tagD_D3D11_TEX1D_ARRAY_RTV
{
	UINT										MipSlice ;
	UINT										FirstArraySlice ;
	UINT										ArraySize ;
} D_D3D11_TEX1D_ARRAY_RTV ;

typedef struct tagD_D3D11_TEX2D_RTV
{
	UINT										MipSlice ;
} D_D3D11_TEX2D_RTV ;

typedef struct tagD_D3D11_TEX2DMS_RTV
{
	UINT										UnusedField_NothingToDefine ;
} D_D3D11_TEX2DMS_RTV ;

typedef struct tagD_D3D11_TEX2D_ARRAY_RTV
{
	UINT										MipSlice ;
	UINT										FirstArraySlice ;
	UINT										ArraySize ;
} D_D3D11_TEX2D_ARRAY_RTV ;

typedef struct tagD_D3D11_TEX2DMS_ARRAY_RTV
{
	UINT										FirstArraySlice ;
	UINT										ArraySize ;
} D_D3D11_TEX2DMS_ARRAY_RTV ;

typedef struct tagD_D3D11_TEX3D_RTV
{
	UINT										MipSlice ;
	UINT										FirstWSlice ;
	UINT										WSize ;
} D_D3D11_TEX3D_RTV ;

typedef struct tagD_D3D11_RENDER_TARGET_VIEW_DESC
{
	D_DXGI_FORMAT								Format ;
	D_D3D11_RTV_DIMENSION						ViewDimension ;
	union 
	{
		D_D3D11_BUFFER_RTV						Buffer ;
		D_D3D11_TEX1D_RTV						Texture1D ;
		D_D3D11_TEX1D_ARRAY_RTV					Texture1DArray ;
		D_D3D11_TEX2D_RTV						Texture2D ;
		D_D3D11_TEX2D_ARRAY_RTV					Texture2DArray ;
		D_D3D11_TEX2DMS_RTV						Texture2DMS ;
		D_D3D11_TEX2DMS_ARRAY_RTV				Texture2DMSArray ;
		D_D3D11_TEX3D_RTV						Texture3D ;
	} ;
} D_D3D11_RENDER_TARGET_VIEW_DESC;

typedef struct tagD_D3D11_SUBRESOURCE_DATA
{
	const void *								pSysMem ;
	UINT										SysMemPitch ;
	UINT										SysMemSlicePitch ;
} D_D3D11_SUBRESOURCE_DATA;

typedef struct tagD_D3D11_BUFFER_DESC
{
	UINT										ByteWidth ;
	D_D3D11_USAGE								Usage ;
	UINT										BindFlags ;
	UINT										CPUAccessFlags ;
	UINT										MiscFlags ;
	UINT										StructureByteStride ;
} D_D3D11_BUFFER_DESC ;

typedef struct D_D3D11_INPUT_ELEMENT_DESC
{
	LPCSTR										SemanticName ;
	UINT										SemanticIndex ;
	D_DXGI_FORMAT								Format ;
	UINT										InputSlot ;
	UINT										AlignedByteOffset ;
	D_D3D11_INPUT_CLASSIFICATION				InputSlotClass ;
	UINT										InstanceDataStepRate ;
} D_D3D11_INPUT_ELEMENT_DESC ;

typedef struct D_D3D11_CLASS_INSTANCE_DESC
{
	UINT										InstanceId ;
	UINT										InstanceIndex ;
	UINT										TypeId ;
	UINT										ConstantBuffer ;
	UINT										BaseConstantBufferOffset ;
	UINT										BaseTexture ;
	UINT										BaseSampler ;
	BOOL										Created ;
} D_D3D11_CLASS_INSTANCE_DESC ;

typedef struct tagD_D3D11_SO_DECLARATION_ENTRY
{
	UINT										Stream ;
	LPCSTR										SemanticName ;
	UINT										SemanticIndex ;
	BYTE										StartComponent ;
	BYTE										ComponentCount ;
	BYTE										OutputSlot ;
} D_D3D11_SO_DECLARATION_ENTRY ;

typedef enum tagD_D3D11_COLOR_WRITE_ENABLE
{
	D_D3D11_COLOR_WRITE_ENABLE_RED				= 1,
	D_D3D11_COLOR_WRITE_ENABLE_GREEN			= 2,
	D_D3D11_COLOR_WRITE_ENABLE_BLUE				= 4,
	D_D3D11_COLOR_WRITE_ENABLE_ALPHA			= 8,
	D_D3D11_COLOR_WRITE_ENABLE_ALL				= ( ( ( D_D3D11_COLOR_WRITE_ENABLE_RED | D_D3D11_COLOR_WRITE_ENABLE_GREEN )  | D_D3D11_COLOR_WRITE_ENABLE_BLUE )  | D_D3D11_COLOR_WRITE_ENABLE_ALPHA ) 
} D_D3D11_COLOR_WRITE_ENABLE ;

typedef struct D_D3D11_RENDER_TARGET_BLEND_DESC
{
	BOOL										BlendEnable ;
	D_D3D11_BLEND								SrcBlend ;
	D_D3D11_BLEND								DestBlend ;
	D_D3D11_BLEND_OP							BlendOp ;
	D_D3D11_BLEND								SrcBlendAlpha ;
	D_D3D11_BLEND								DestBlendAlpha ;
	D_D3D11_BLEND_OP							BlendOpAlpha ;
	D_UINT8										RenderTargetWriteMask ;
} D_D3D11_RENDER_TARGET_BLEND_DESC ;

typedef struct D_D3D11_BLEND_DESC
{
	BOOL										AlphaToCoverageEnable ;
	BOOL										IndependentBlendEnable ;
	D_D3D11_RENDER_TARGET_BLEND_DESC			RenderTarget[ 8 ] ;
} D_D3D11_BLEND_DESC ;

typedef struct tagD_D3D11_DEPTH_STENCILOP_DESC
{
	D_D3D11_STENCIL_OP							StencilFailOp ;
	D_D3D11_STENCIL_OP							StencilDepthFailOp ;
	D_D3D11_STENCIL_OP							StencilPassOp ;
	D_D3D11_COMPARISON_FUNC						StencilFunc ;
} D_D3D11_DEPTH_STENCILOP_DESC ;

typedef struct tagD_D3D11_DEPTH_STENCIL_DESC
{
	BOOL										DepthEnable ;
	D_D3D11_DEPTH_WRITE_MASK					DepthWriteMask ;
	D_D3D11_COMPARISON_FUNC						DepthFunc ;
	BOOL										StencilEnable ;
	D_UINT8										StencilReadMask ;
	D_UINT8										StencilWriteMask ;
	D_D3D11_DEPTH_STENCILOP_DESC				FrontFace ;
	D_D3D11_DEPTH_STENCILOP_DESC				BackFace ;
} D_D3D11_DEPTH_STENCIL_DESC ;

typedef struct tagD_D3D11_RASTERIZER_DESC
{
	D_D3D11_FILL_MODE							FillMode ;
	D_D3D11_CULL_MODE							CullMode ;
	BOOL										FrontCounterClockwise ;
	INT											DepthBias ;
	FLOAT										DepthBiasClamp ;
	FLOAT										SlopeScaledDepthBias ;
	BOOL										DepthClipEnable ;
	BOOL										ScissorEnable ;
	BOOL										MultisampleEnable ;
	BOOL										AntialiasedLineEnable ;
} D_D3D11_RASTERIZER_DESC ;

typedef struct tagD_D3D11_SAMPLER_DESC
{
	D_D3D11_FILTER								Filter;
	D_D3D11_TEXTURE_ADDRESS_MODE				AddressU;
	D_D3D11_TEXTURE_ADDRESS_MODE				AddressV;
	D_D3D11_TEXTURE_ADDRESS_MODE				AddressW;
	FLOAT										MipLODBias;
	UINT										MaxAnisotropy;
	D_D3D11_COMPARISON_FUNC						ComparisonFunc;
	FLOAT										BorderColor[ 4 ];
	FLOAT										MinLOD;
	FLOAT										MaxLOD;
} D_D3D11_SAMPLER_DESC ;

typedef struct tagD_D3D11_QUERY_DESC
{
	D_D3D11_QUERY								Query ;
	UINT										MiscFlags ;
} D_D3D11_QUERY_DESC ;

typedef struct tagD_D3D11_COUNTER_DESC
{
	D_D3D11_COUNTER								Counter ;
	UINT										MiscFlags ;
}  D_D3D11_COUNTER_DESC ;

typedef struct tagD_D3D11_MAPPED_SUBRESOURCE
{
	void *										pData ;
	UINT										RowPitch ;
	UINT										DepthPitch ;
} D_D3D11_MAPPED_SUBRESOURCE ;

typedef struct tagD_D3D11_VIEWPORT
{
	FLOAT										TopLeftX ;
	FLOAT										TopLeftY ;
	FLOAT										Width ;
	FLOAT										Height ;
	FLOAT										MinDepth ;
	FLOAT										MaxDepth ;
} D_D3D11_VIEWPORT ;

typedef RECT D_D3D11_RECT ;

typedef struct tagD_D3D11_BOX
{
	UINT										left ;
	UINT										top ;
	UINT										front ;
	UINT										right ;
	UINT										bottom ;
	UINT										back ;
} D_D3D11_BOX ;

typedef struct D_D3D11_COUNTER_INFO
{
	D_D3D11_COUNTER								LastDeviceDependentCounter ;
	UINT										NumSimultaneousCounters ;
	D_UINT8										NumDetectableParallelUnits ;
} D_D3D11_COUNTER_INFO;







class D_ID3DBlob : public D_IUnknown
{
public:
    virtual LPVOID	__stdcall GetBufferPointer			( void ) = 0 ;
    virtual SIZE_T	__stdcall GetBufferSize				( void ) = 0 ;
};

class D_ID3DInclude
{
public:
	virtual HRESULT	__stdcall Open						( D_D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes ) = 0 ;
	virtual HRESULT	__stdcall Close						( LPCVOID pData ) = 0 ;
};

class D_ID3D11Device ;
class D_ID3D11ClassLinkage ;
class D_IDXGIAdapter ;
class D_IDXGISurface ;

class D_IDXGIObject : public D_IUnknown
{
public:
	virtual HRESULT __stdcall SetPrivateData			( REFGUID Name, UINT DataSize, const void *pData ) = 0 ;
	virtual HRESULT __stdcall SetPrivateDataInterface	( REFGUID Name, const D_IUnknown *pUnknown ) = 0 ;
	virtual HRESULT __stdcall GetPrivateData			( REFGUID Name, UINT *pDataSize, void *pData ) = 0 ;
	virtual HRESULT __stdcall GetParent					( REFIID riid, void **ppParent ) = 0 ;
};

class D_IDXGIDevice : public D_IDXGIObject
{
public:
	virtual HRESULT __stdcall GetAdapter				( D_IDXGIAdapter **pAdapter) = 0 ;
	virtual HRESULT __stdcall CreateSurface				( const D_DXGI_SURFACE_DESC *pDesc, UINT NumSurfaces, D_DXGI_USAGE Usage, const D_DXGI_SHARED_RESOURCE *pSharedResource, D_IDXGISurface **ppSurface ) = 0 ;
	virtual HRESULT __stdcall QueryResourceResidency	( D_IUnknown *const *ppResources, D_DXGI_RESIDENCY *pResidencyStatus, UINT NumResources ) = 0 ;
	virtual HRESULT __stdcall SetGPUThreadPriority		( INT Priority ) = 0 ;
	virtual HRESULT __stdcall GetGPUThreadPriority		( INT *pPriority ) = 0 ;
};

class D_IDXGIDevice1 : public D_IDXGIDevice
{
public:
	virtual HRESULT __stdcall SetMaximumFrameLatency	( UINT MaxLatency ) = 0 ;
	virtual HRESULT __stdcall GetMaximumFrameLatency	( UINT *pMaxLatency ) = 0 ;
};

class D_IDXGIDeviceSubObject : public D_IDXGIObject
{
public:
	virtual HRESULT __stdcall GetDevice					( REFIID riid, void **ppDevice ) = 0 ;
};

class D_IDXGISurface : public D_IDXGIDeviceSubObject
{
public:
	virtual HRESULT __stdcall GetDesc					( D_DXGI_SURFACE_DESC *pDesc ) = 0 ;
	virtual HRESULT __stdcall Map						( D_DXGI_MAPPED_RECT *pLockedRect, UINT MapFlags ) = 0 ;
	virtual HRESULT __stdcall Unmap						( void ) = 0 ;
};

class D_IDXGIOutput : public D_IDXGIObject
{
public:
	virtual HRESULT __stdcall GetDesc					( D_DXGI_OUTPUT_DESC *pDesc ) = 0 ;
	virtual HRESULT __stdcall GetDisplayModeList		( D_DXGI_FORMAT EnumFormat, UINT Flags, UINT *pNumModes, D_DXGI_MODE_DESC *pDesc ) = 0 ;
	virtual HRESULT __stdcall FindClosestMatchingMode	( const D_DXGI_MODE_DESC *pModeToMatch, D_DXGI_MODE_DESC *pClosestMatch, D_IUnknown *pConcernedDevice ) = 0 ;
	virtual HRESULT __stdcall WaitForVBlank				( void ) = 0 ;
	virtual HRESULT __stdcall TakeOwnership				( D_IUnknown *pDevice, BOOL Exclusive ) = 0 ;
	virtual void	__stdcall ReleaseOwnership			( void ) = 0 ;
	virtual HRESULT __stdcall GetGammaControlCapabilities( D_DXGI_GAMMA_CONTROL_CAPABILITIES *pGammaCaps ) = 0 ;
	virtual HRESULT __stdcall SetGammaControl			( const D_DXGI_GAMMA_CONTROL *pArray ) = 0 ;
	virtual HRESULT __stdcall GetGammaControl			( D_DXGI_GAMMA_CONTROL *pArray ) = 0 ;
	virtual HRESULT __stdcall SetDisplaySurface			( D_IDXGISurface *pScanoutSurface ) = 0 ;
	virtual HRESULT __stdcall GetDisplaySurfaceData		( D_IDXGISurface *pDestination ) = 0 ;
	virtual HRESULT __stdcall GetFrameStatistics		( D_DXGI_FRAME_STATISTICS *pStats ) = 0 ;
};

class D_IDXGIAdapter : public D_IDXGIObject
{
public:
	virtual HRESULT __stdcall EnumOutputs				( UINT Output, D_IDXGIOutput **ppOutput ) = 0 ;
	virtual HRESULT __stdcall GetDesc					( D_DXGI_ADAPTER_DESC *pDesc ) = 0 ;
	virtual HRESULT __stdcall CheckInterfaceSupport		( REFGUID InterfaceName, LARGE_INTEGER *pUMDVersion ) = 0 ;
};

class D_IDXGIAdapter1 : public D_IDXGIAdapter
{
public:
	virtual HRESULT __stdcall GetDesc1					( D_DXGI_ADAPTER_DESC1 *pDesc ) = 0 ;
} ;

class D_IDXGISwapChain : public D_IDXGIDeviceSubObject
{
public:
	virtual HRESULT __stdcall Present					( UINT SyncInterval, UINT Flags ) = 0 ;
	virtual HRESULT __stdcall GetBuffer					( UINT Buffer, REFIID riid, void **ppSurface ) = 0 ;
	virtual HRESULT __stdcall SetFullscreenState		( BOOL Fullscreen, D_IDXGIOutput *pTarget ) = 0 ;
	virtual HRESULT __stdcall GetFullscreenState		( BOOL *pFullscreen, D_IDXGIOutput **ppTarget ) = 0 ;
	virtual HRESULT __stdcall GetDesc					( D_DXGI_SWAP_CHAIN_DESC *pDesc ) = 0 ;
	virtual HRESULT __stdcall ResizeBuffers				( UINT BufferCount, UINT Width, UINT Height, D_DXGI_FORMAT NewFormat, UINT SwapChainFlags ) = 0 ;
	virtual HRESULT __stdcall ResizeTarget				( const D_DXGI_MODE_DESC *pNewTargetParameters ) = 0 ;
	virtual HRESULT __stdcall GetContainingOutput		( D_IDXGIOutput **ppOutput ) = 0 ;
	virtual HRESULT __stdcall GetFrameStatistics		( D_DXGI_FRAME_STATISTICS *pStats ) = 0 ;
	virtual HRESULT __stdcall GetLastPresentCount		( UINT *pLastPresentCount ) = 0 ;
};

class D_IDXGIFactory : public D_IDXGIObject
{
public:
	virtual HRESULT __stdcall EnumAdapters				( UINT Adapter, D_IDXGIAdapter **ppAdapter ) = 0 ;
	virtual HRESULT __stdcall MakeWindowAssociation		( HWND WindowHandle, UINT Flags ) = 0 ;
	virtual HRESULT __stdcall GetWindowAssociation		( HWND *pWindowHandle ) = 0 ;
	virtual HRESULT __stdcall CreateSwapChain			( D_IUnknown *pDevice, D_DXGI_SWAP_CHAIN_DESC *pDesc, D_IDXGISwapChain **ppSwapChain ) = 0 ;
	virtual HRESULT __stdcall CreateSoftwareAdapter		( HMODULE Module, D_IDXGIAdapter **ppAdapter ) = 0 ;
};

class D_IDXGIFactory1 : public D_IDXGIFactory
{
public:
	virtual	HRESULT	__stdcall EnumAdapters1				( UINT Adapter, D_IDXGIAdapter1 **ppAdapter ) = 0 ;
	virtual BOOL    __stdcall IsCurrent					( void ) = 0 ;
} ;

class D_ID3D11DeviceChild : public D_IUnknown
{
public:
	virtual void	__stdcall GetDevice					( D_ID3D11Device **ppDevice ) = 0 ;
	virtual HRESULT __stdcall GetPrivateData			( REFGUID guid, UINT *pDataSize, void *pData ) = 0 ;
	virtual HRESULT __stdcall SetPrivateData			( REFGUID guid, UINT DataSize, const void *pData ) = 0 ;
	virtual HRESULT __stdcall SetPrivateDataInterface	( REFGUID guid, const D_IUnknown *pData ) = 0 ;
};

class D_ID3D11Resource : public D_ID3D11DeviceChild
{
public:
	virtual void	__stdcall GetType					( D_D3D11_RESOURCE_DIMENSION *pResourceDimension ) = 0 ;
	virtual void	__stdcall SetEvictionPriority		( UINT EvictionPriority ) = 0 ;
	virtual UINT	__stdcall GetEvictionPriority		( void ) = 0 ;
};

class D_ID3D11Buffer : public D_ID3D11Resource
{
public:
	virtual void	__stdcall GetDesc					( D_D3D11_BUFFER_DESC *pDesc ) = 0 ;
};

class D_ID3D11Texture1D : public D_ID3D11Resource
{
public:
	virtual void	__stdcall GetDesc					( D_D3D11_TEXTURE1D_DESC *pDesc ) = 0 ;
};

class D_ID3D11Texture2D : public D_ID3D11Resource
{
public:
	virtual void	__stdcall GetDesc					( D_D3D11_TEXTURE2D_DESC *pDesc ) = 0 ;
};

class D_ID3D11Texture3D : public D_ID3D11Resource
{
public:
	virtual void	__stdcall GetDesc					( D_D3D11_TEXTURE3D_DESC *pDesc ) = 0 ;
};

class D_ID3D11View : public D_ID3D11DeviceChild
{
public:
	virtual void	__stdcall GetResource				( D_ID3D11Resource **ppResource ) = 0 ;
};

class D_ID3D11InputLayout : public D_ID3D11DeviceChild
{
public:
};

class D_ID3D11ShaderResourceView : public D_ID3D11View
{
public:
	virtual void	__stdcall GetDesc					( D_D3D11_SHADER_RESOURCE_VIEW_DESC *pDesc ) = 0 ;

};

class D_ID3D11UnorderedAccessView : public D_ID3D11View
{
public:
	virtual void	__stdcall GetDesc					( D_D3D11_UNORDERED_ACCESS_VIEW_DESC *pDesc ) = 0 ;
};

class D_ID3D11RenderTargetView : public D_ID3D11View
{
public:
	virtual void	__stdcall GetDesc					( D_D3D11_RENDER_TARGET_VIEW_DESC *pDesc ) = 0 ;
};

class D_ID3D11DepthStencilView : public D_ID3D11View
{
public:
	virtual void	__stdcall GetDesc					( D_D3D11_DEPTH_STENCIL_VIEW_DESC *pDesc ) = 0 ;
};

class D_ID3D11ClassInstance : public D_ID3D11DeviceChild
{
public:
	virtual void	__stdcall GetClassLinkage			( D_ID3D11ClassLinkage **ppLinkage ) = 0 ;
	virtual void	__stdcall GetDesc					( D_D3D11_CLASS_INSTANCE_DESC *pDesc ) = 0 ;
	virtual void	__stdcall GetInstanceName			( LPSTR pInstanceName, SIZE_T *pBufferLength ) = 0 ;
	virtual void	__stdcall GetTypeName				( LPSTR pTypeName, SIZE_T *pBufferLength ) = 0 ;
};

class D_ID3D11ClassLinkage : public D_ID3D11DeviceChild
{
public:
	virtual HRESULT __stdcall GetClassInstance			( LPCSTR pClassInstanceName, UINT InstanceIndex, D_ID3D11ClassInstance **ppInstance ) = 0 ;
	virtual HRESULT __stdcall CreateClassInstance		( LPCSTR pClassTypeName, UINT ConstantBufferOffset, UINT ConstantVectorOffset, UINT TextureOffset, UINT SamplerOffset, D_ID3D11ClassInstance **ppInstance ) = 0 ;
};

class D_ID3D11VertexShader : public D_ID3D11DeviceChild
{
public:
};

class D_ID3D11GeometryShader : public D_ID3D11DeviceChild
{
public:
};

class D_ID3D11PixelShader : public D_ID3D11DeviceChild
{
public:
};

class D_ID3D11HullShader : public D_ID3D11DeviceChild
{
public:
};

class D_ID3D11DomainShader : public D_ID3D11DeviceChild
{
public:
};

class D_ID3D11ComputeShader : public D_ID3D11DeviceChild
{
public:
};

class D_ID3D11BlendState : public D_ID3D11DeviceChild
{
public:
	virtual void	__stdcall GetDesc					( D_D3D11_BLEND_DESC *pDesc ) = 0 ;
};

class D_ID3D11DepthStencilState : public D_ID3D11DeviceChild
{
public:
	virtual void	__stdcall GetDesc					( D_D3D11_DEPTH_STENCIL_DESC *pDesc ) = 0 ;
};

class D_ID3D11RasterizerState : public D_ID3D11DeviceChild
{
public:
	virtual void	__stdcall GetDesc					( D_D3D11_RASTERIZER_DESC *pDesc ) = 0 ;
};

class D_ID3D11SamplerState : public D_ID3D11DeviceChild
{
public:
	virtual void	__stdcall GetDesc					( D_D3D11_SAMPLER_DESC *pDesc ) = 0 ;
};

class D_ID3D11Asynchronous : public D_ID3D11DeviceChild
{
public:
	virtual UINT	__stdcall GetDataSize				( void ) = 0 ;
};

class D_ID3D11Query : public D_ID3D11Asynchronous
{
public:
	virtual void	__stdcall GetDesc					( D_D3D11_QUERY_DESC *pDesc ) = 0 ;
};

class D_ID3D11Predicate : public D_ID3D11Query
{
public:
};

class D_ID3D11Counter : public D_ID3D11Asynchronous
{
public:
	virtual void	__stdcall GetDesc					( D_D3D11_COUNTER_DESC *pDesc ) = 0 ;
    
};

class D_ID3D11CommandList : public D_ID3D11DeviceChild
{
public:
	virtual UINT	__stdcall GetContextFlags			( void ) = 0 ;
};

class D_ID3D11DeviceContext : public D_ID3D11DeviceChild
{
public:
	virtual void	__stdcall VSSetConstantBuffers		( UINT StartSlot, UINT NumBuffers, D_ID3D11Buffer *const *ppConstantBuffers ) = 0 ;
	virtual void	__stdcall PSSetShaderResources		( UINT StartSlot, UINT NumViews,   D_ID3D11ShaderResourceView *const *ppShaderResourceViews ) = 0 ;
	virtual void	__stdcall PSSetShader				( D_ID3D11PixelShader *pPixelShader, D_ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances ) = 0 ;
	virtual void	__stdcall PSSetSamplers				( UINT StartSlot, UINT NumSamplers, D_ID3D11SamplerState *const *ppSamplers ) = 0 ;
	virtual void	__stdcall VSSetShader				( D_ID3D11VertexShader *pVertexShader, D_ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances ) = 0 ;
	virtual void	__stdcall DrawIndexed				( UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation ) = 0 ;
	virtual void	__stdcall Draw						( UINT VertexCount, UINT StartVertexLocation ) = 0 ;
	virtual HRESULT __stdcall Map						( D_ID3D11Resource *pResource, UINT Subresource, D_D3D11_MAP MapType, UINT MapFlags, D_D3D11_MAPPED_SUBRESOURCE *pMappedResource ) = 0 ;
	virtual void	__stdcall Unmap						( D_ID3D11Resource *pResource, UINT Subresource ) = 0 ;
	virtual void	__stdcall PSSetConstantBuffers		( UINT StartSlot, UINT NumBuffers, D_ID3D11Buffer *const *ppConstantBuffers ) = 0 ;
	virtual void	__stdcall IASetInputLayout			( D_ID3D11InputLayout *pInputLayout ) = 0 ;
	virtual void	__stdcall IASetVertexBuffers		( UINT StartSlot, UINT NumBuffers, D_ID3D11Buffer *const *ppVertexBuffers, const UINT *pStrides, const UINT *pOffsets ) = 0 ;
	virtual void	__stdcall IASetIndexBuffer			( D_ID3D11Buffer *pIndexBuffer, D_DXGI_FORMAT Format, UINT Offset ) = 0 ;
	virtual void	__stdcall DrawIndexedInstanced		( UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation ) = 0 ;
	virtual void	__stdcall DrawInstanced				( UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation ) = 0 ; 
	virtual void	__stdcall GSSetConstantBuffers		( UINT StartSlot, UINT NumBuffers, D_ID3D11Buffer *const *ppConstantBuffers ) = 0 ; 
	virtual void	__stdcall GSSetShader				( D_ID3D11GeometryShader *pShader, D_ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances ) = 0 ;
	virtual void	__stdcall IASetPrimitiveTopology	( D_D3D11_PRIMITIVE_TOPOLOGY Topology ) = 0 ;
	virtual void	__stdcall VSSetShaderResources		( UINT StartSlot, UINT NumViews, D_ID3D11ShaderResourceView *const *ppShaderResourceViews ) = 0 ;
	virtual void	__stdcall VSSetSamplers				( UINT StartSlot, UINT NumSamplers, D_ID3D11SamplerState *const *ppSamplers ) = 0 ;
	virtual void	__stdcall Begin						( D_ID3D11Asynchronous *pAsync ) = 0 ;
	virtual void	__stdcall End						( D_ID3D11Asynchronous *pAsync ) = 0 ;
	virtual HRESULT __stdcall GetData					( D_ID3D11Asynchronous *pAsync, void *pData,UINT DataSize, UINT GetDataFlags ) = 0 ;
	virtual void	__stdcall SetPredication			( D_ID3D11Predicate *pPredicate, BOOL PredicateValue ) = 0 ;
	virtual void	__stdcall GSSetShaderResources		( UINT StartSlot, UINT NumViews, D_ID3D11ShaderResourceView *const *ppShaderResourceViews ) = 0 ;
	virtual void	__stdcall GSSetSamplers				( UINT StartSlot, UINT NumSamplers, D_ID3D11SamplerState *const *ppSamplers ) = 0 ;
	virtual void	__stdcall OMSetRenderTargets		( UINT NumViews, D_ID3D11RenderTargetView *const *ppRenderTargetViews, D_ID3D11DepthStencilView *pDepthStencilView ) = 0 ;
	virtual void	__stdcall OMSetRenderTargetsAndUnorderedAccessViews( UINT NumRTVs, D_ID3D11RenderTargetView *const *ppRenderTargetViews, D_ID3D11DepthStencilView *pDepthStencilView, UINT UAVStartSlot, UINT NumUAVs, D_ID3D11UnorderedAccessView *const *ppUnorderedAccessViews, const UINT *pUAVInitialCounts ) = 0 ;
	virtual void	__stdcall OMSetBlendState			( D_ID3D11BlendState *pBlendState, const FLOAT BlendFactor[ 4 ], UINT SampleMask ) = 0 ;
	virtual void	__stdcall OMSetDepthStencilState	( D_ID3D11DepthStencilState *pDepthStencilState, UINT StencilRef ) = 0 ;
	virtual void	__stdcall SOSetTargets				( UINT NumBuffers, D_ID3D11Buffer *const *ppSOTargets, const UINT *pOffsets ) = 0 ;
	virtual void	__stdcall DrawAuto					( void ) = 0 ;
	virtual void	__stdcall DrawIndexedInstancedIndirect( D_ID3D11Buffer *pBufferForArgs, UINT AlignedByteOffsetForArgs ) = 0 ;
	virtual void	__stdcall DrawInstancedIndirect		( D_ID3D11Buffer *pBufferForArgs, UINT AlignedByteOffsetForArgs ) = 0 ;
	virtual void	__stdcall Dispatch					( UINT ThreadGroupCountX, UINT ThreadGroupCountY, UINT ThreadGroupCountZ ) = 0 ;
	virtual void	__stdcall DispatchIndirect			( D_ID3D11Buffer *pBufferForArgs, UINT AlignedByteOffsetForArgs ) = 0 ;
	virtual void	__stdcall RSSetState				( D_ID3D11RasterizerState *pRasterizerState ) = 0 ;
	virtual void	__stdcall RSSetViewports			( UINT NumViewports, const D_D3D11_VIEWPORT *pViewports ) = 0 ;
	virtual void	__stdcall RSSetScissorRects			( UINT NumRects, const D_D3D11_RECT *pRects ) = 0 ;
	virtual void	__stdcall CopySubresourceRegion		( D_ID3D11Resource *pDstResource, UINT DstSubresource, UINT DstX, UINT DstY, UINT DstZ, D_ID3D11Resource *pSrcResource, UINT SrcSubresource, const D_D3D11_BOX *pSrcBox ) = 0 ;
	virtual void	__stdcall CopyResource				( D_ID3D11Resource *pDstResource, D_ID3D11Resource *pSrcResource ) = 0 ;
	virtual void	__stdcall UpdateSubresource			( D_ID3D11Resource *pDstResource, UINT DstSubresource, const D_D3D11_BOX *pDstBox, const void *pSrcData, UINT SrcRowPitch, UINT SrcDepthPitch ) = 0 ;
	virtual void	__stdcall CopyStructureCount		( D_ID3D11Buffer *pDstBuffer, UINT DstAlignedByteOffset, D_ID3D11UnorderedAccessView *pSrcView ) = 0 ;
	virtual void	__stdcall ClearRenderTargetView		( D_ID3D11RenderTargetView *pRenderTargetView, const FLOAT ColorRGBA[ 4 ] ) = 0 ;
	virtual void	__stdcall ClearUnorderedAccessViewUint( D_ID3D11UnorderedAccessView *pUnorderedAccessView, const UINT Values[ 4 ] ) = 0 ;
	virtual void	__stdcall ClearUnorderedAccessViewFloat( D_ID3D11UnorderedAccessView *pUnorderedAccessView, const FLOAT Values[ 4 ] ) = 0 ;
	virtual void	__stdcall ClearDepthStencilView		( D_ID3D11DepthStencilView *pDepthStencilView, UINT ClearFlags, FLOAT Depth, D_UINT8 Stencil ) = 0 ;
	virtual void	__stdcall GenerateMips				( D_ID3D11ShaderResourceView *pShaderResourceView ) = 0 ;
	virtual void	__stdcall SetResourceMinLOD			( D_ID3D11Resource *pResource, FLOAT MinLOD ) = 0 ;
	virtual FLOAT	__stdcall GetResourceMinLOD			( D_ID3D11Resource *pResource ) = 0 ;
	virtual void	__stdcall ResolveSubresource		( D_ID3D11Resource *pDstResource, UINT DstSubresource, D_ID3D11Resource *pSrcResource, UINT SrcSubresource, D_DXGI_FORMAT Format ) = 0 ;
	virtual void	__stdcall ExecuteCommandList		( D_ID3D11CommandList *pCommandList, BOOL RestoreContextState ) = 0 ;
	virtual void	__stdcall HSSetShaderResources		( UINT StartSlot, UINT NumViews, D_ID3D11ShaderResourceView *const *ppShaderResourceViews ) = 0 ;
	virtual void	__stdcall HSSetShader				( D_ID3D11HullShader *pHullShader, D_ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances ) = 0 ;
	virtual void	__stdcall HSSetSamplers				( UINT StartSlot, UINT NumSamplers, D_ID3D11SamplerState *const *ppSamplers ) = 0 ;
	virtual void	__stdcall HSSetConstantBuffers		( UINT StartSlot, UINT NumBuffers, D_ID3D11Buffer *const *ppConstantBuffers ) = 0 ;
	virtual void	__stdcall DSSetShaderResources		( UINT StartSlot, UINT NumViews, D_ID3D11ShaderResourceView *const *ppShaderResourceViews ) = 0 ;
	virtual void	__stdcall DSSetShader				( D_ID3D11DomainShader *pDomainShader, D_ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances ) = 0 ;
	virtual void	__stdcall DSSetSamplers				( UINT StartSlot, UINT NumSamplers, D_ID3D11SamplerState *const *ppSamplers ) = 0 ;
	virtual void	__stdcall DSSetConstantBuffers		( UINT StartSlot, UINT NumBuffers, D_ID3D11Buffer *const *ppConstantBuffers ) = 0 ;
	virtual void	__stdcall CSSetShaderResources		( UINT StartSlot, UINT NumViews, D_ID3D11ShaderResourceView *const *ppShaderResourceViews ) = 0 ;
	virtual void	__stdcall CSSetUnorderedAccessViews	( UINT StartSlot, UINT NumUAVs, D_ID3D11UnorderedAccessView *const *ppUnorderedAccessViews, const UINT *pUAVInitialCounts ) = 0 ;
	virtual void	__stdcall CSSetShader				( D_ID3D11ComputeShader *pComputeShader, D_ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances ) = 0 ;
	virtual void	__stdcall CSSetSamplers				( UINT StartSlot, UINT NumSamplers, D_ID3D11SamplerState *const *ppSamplers ) = 0 ;
	virtual void	__stdcall CSSetConstantBuffers		( UINT StartSlot, UINT NumBuffers, D_ID3D11Buffer *const *ppConstantBuffers ) = 0 ;
	virtual void	__stdcall VSGetConstantBuffers		( UINT StartSlot, UINT NumBuffers, D_ID3D11Buffer **ppConstantBuffers ) = 0 ;
	virtual void	__stdcall PSGetShaderResources		( UINT StartSlot, UINT NumViews, D_ID3D11ShaderResourceView **ppShaderResourceViews ) = 0 ;
	virtual void	__stdcall PSGetShader				( D_ID3D11PixelShader **ppPixelShader, D_ID3D11ClassInstance **ppClassInstances, UINT *pNumClassInstances ) = 0 ;
	virtual void	__stdcall PSGetSamplers				( UINT StartSlot, UINT NumSamplers, D_ID3D11SamplerState **ppSamplers ) = 0 ;
	virtual void	__stdcall VSGetShader				(  D_ID3D11VertexShader **ppVertexShader, D_ID3D11ClassInstance **ppClassInstances, UINT *pNumClassInstances ) = 0 ;
	virtual void	__stdcall PSGetConstantBuffers		( UINT StartSlot, UINT NumBuffers, D_ID3D11Buffer **ppConstantBuffers ) = 0 ;
	virtual void	__stdcall IAGetInputLayout			( D_ID3D11InputLayout **ppInputLayout ) = 0 ;
	virtual void	__stdcall IAGetVertexBuffers		( UINT StartSlot, UINT NumBuffers, D_ID3D11Buffer **ppVertexBuffers, UINT *pStrides, UINT *pOffsets ) = 0 ;
	virtual void	__stdcall IAGetIndexBuffer			( D_ID3D11Buffer **pIndexBuffer, D_DXGI_FORMAT *Format, UINT *Offset ) = 0 ;
	virtual void	__stdcall GSGetConstantBuffers		( UINT StartSlot, UINT NumBuffers, D_ID3D11Buffer **ppConstantBuffers ) = 0 ;
	virtual void	__stdcall GSGetShader				( D_ID3D11GeometryShader **ppGeometryShader, D_ID3D11ClassInstance **ppClassInstances, UINT *pNumClassInstances ) = 0 ;
	virtual void	__stdcall IAGetPrimitiveTopology	( D_D3D11_PRIMITIVE_TOPOLOGY *pTopology ) = 0 ;
	virtual void	__stdcall VSGetShaderResources		( UINT StartSlot, UINT NumViews, D_ID3D11ShaderResourceView **ppShaderResourceViews ) = 0 ;
	virtual void	__stdcall VSGetSamplers				( UINT StartSlot, UINT NumSamplers, D_ID3D11SamplerState **ppSamplers ) = 0 ;
	virtual void	__stdcall GetPredication			( D_ID3D11Predicate **ppPredicate, BOOL *pPredicateValue ) = 0 ;
	virtual void	__stdcall GSGetShaderResources		( UINT StartSlot, UINT NumViews, D_ID3D11ShaderResourceView **ppShaderResourceViews ) = 0 ;
	virtual void	__stdcall GSGetSamplers				( UINT StartSlot, UINT NumSamplers, D_ID3D11SamplerState **ppSamplers ) = 0 ;
	virtual void	__stdcall OMGetRenderTargets		( UINT NumViews, D_ID3D11RenderTargetView **ppRenderTargetViews, D_ID3D11DepthStencilView **ppDepthStencilView ) = 0 ;
	virtual void	__stdcall OMGetRenderTargetsAndUnorderedAccessViews( UINT NumRTVs, D_ID3D11RenderTargetView **ppRenderTargetViews, D_ID3D11DepthStencilView **ppDepthStencilView, UINT UAVStartSlot, UINT NumUAVs, D_ID3D11UnorderedAccessView **ppUnorderedAccessViews ) = 0 ;
	virtual void	__stdcall OMGetBlendState			( D_ID3D11BlendState **ppBlendState, FLOAT BlendFactor[ 4 ], UINT *pSampleMask ) = 0 ;
	virtual void	__stdcall OMGetDepthStencilState	( D_ID3D11DepthStencilState **ppDepthStencilState, UINT *pStencilRef ) = 0 ;
	virtual void	__stdcall SOGetTargets				( UINT NumBuffers, D_ID3D11Buffer **ppSOTargets ) = 0 ;
	virtual void	__stdcall RSGetState				( D_ID3D11RasterizerState **ppRasterizerState ) = 0 ;
	virtual void	__stdcall RSGetViewports			( UINT *pNumViewports, D_D3D11_VIEWPORT *pViewports ) = 0 ;
	virtual void	__stdcall RSGetScissorRects			( UINT *pNumRects, D_D3D11_RECT *pRects ) = 0 ;
	virtual void	__stdcall HSGetShaderResources		( UINT StartSlot, UINT NumViews,    D_ID3D11ShaderResourceView **ppShaderResourceViews ) = 0 ;
	virtual void	__stdcall HSGetShader				( D_ID3D11HullShader **ppHullShader, D_ID3D11ClassInstance **ppClassInstances, UINT *pNumClassInstances ) = 0 ;
	virtual void	__stdcall HSGetSamplers				( UINT StartSlot, UINT NumSamplers, D_ID3D11SamplerState **ppSamplers ) = 0 ;
	virtual void	__stdcall HSGetConstantBuffers		( UINT StartSlot, UINT NumBuffers,  D_ID3D11Buffer **ppConstantBuffers ) = 0 ;
	virtual void	__stdcall DSGetShaderResources		( UINT StartSlot, UINT NumViews,    D_ID3D11ShaderResourceView **ppShaderResourceViews ) = 0 ;
	virtual void	__stdcall DSGetShader				( D_ID3D11DomainShader **ppDomainShader, D_ID3D11ClassInstance **ppClassInstances, UINT *pNumClassInstances ) = 0 ;
	virtual void	__stdcall DSGetSamplers				( UINT StartSlot, UINT NumSamplers, D_ID3D11SamplerState **ppSamplers ) = 0 ;
	virtual void	__stdcall DSGetConstantBuffers		( UINT StartSlot, UINT NumBuffers,  D_ID3D11Buffer **ppConstantBuffers ) = 0 ;
	virtual void	__stdcall CSGetShaderResources		( UINT StartSlot, UINT NumViews,    D_ID3D11ShaderResourceView **ppShaderResourceViews ) = 0 ;
	virtual void	__stdcall CSGetUnorderedAccessViews	( UINT StartSlot, UINT NumUAVs,     D_ID3D11UnorderedAccessView **ppUnorderedAccessViews ) = 0 ;
	virtual void	__stdcall CSGetShader				( D_ID3D11ComputeShader **ppComputeShader, D_ID3D11ClassInstance **ppClassInstances, UINT *pNumClassInstances ) = 0 ;
	virtual void	__stdcall CSGetSamplers				( UINT StartSlot, UINT NumSamplers, D_ID3D11SamplerState **ppSamplers ) = 0 ;
	virtual void	__stdcall CSGetConstantBuffers		( UINT StartSlot, UINT NumBuffers,  D_ID3D11Buffer **ppConstantBuffers ) = 0 ;
	virtual void	__stdcall ClearState				( void ) = 0 ;
	virtual void	__stdcall Flush						( void ) = 0 ;
	virtual D_D3D11_DEVICE_CONTEXT_TYPE __stdcall GetType( void ) = 0 ;
	virtual UINT	__stdcall GetContextFlags			( void ) = 0 ;
	virtual HRESULT __stdcall FinishCommandList			( BOOL RestoreDeferredContextState, D_ID3D11CommandList **ppCommandList ) = 0 ;
};

class D_ID3D11Device : public D_IUnknown
{
public:
	virtual HRESULT __stdcall CreateBuffer				( const D_D3D11_BUFFER_DESC *pDesc, const D_D3D11_SUBRESOURCE_DATA *pInitialData, D_ID3D11Buffer **ppBuffer ) = 0 ;
	virtual HRESULT __stdcall CreateTexture1D			( const D_D3D11_TEXTURE1D_DESC *pDesc, const D_D3D11_SUBRESOURCE_DATA *pInitialData, D_ID3D11Texture1D **ppTexture1D ) = 0 ;
	virtual HRESULT __stdcall CreateTexture2D			( const D_D3D11_TEXTURE2D_DESC *pDesc, const D_D3D11_SUBRESOURCE_DATA *pInitialData, D_ID3D11Texture2D **ppTexture2D ) = 0 ;
	virtual HRESULT __stdcall CreateTexture3D			( const D_D3D11_TEXTURE3D_DESC *pDesc, const D_D3D11_SUBRESOURCE_DATA *pInitialData, D_ID3D11Texture3D **ppTexture3D ) = 0 ;
	virtual HRESULT __stdcall CreateShaderResourceView	( D_ID3D11Resource *pResource, const D_D3D11_SHADER_RESOURCE_VIEW_DESC *pDesc, D_ID3D11ShaderResourceView **ppSRView ) = 0 ;
	virtual HRESULT __stdcall CreateUnorderedAccessView	( D_ID3D11Resource *pResource, const D_D3D11_UNORDERED_ACCESS_VIEW_DESC *pDesc, D_ID3D11UnorderedAccessView **ppUAView ) = 0 ;
	virtual HRESULT __stdcall CreateRenderTargetView	( D_ID3D11Resource *pResource, const D_D3D11_RENDER_TARGET_VIEW_DESC *pDesc, D_ID3D11RenderTargetView **ppRTView ) = 0 ;
	virtual HRESULT __stdcall CreateDepthStencilView	( D_ID3D11Resource *pResource, const D_D3D11_DEPTH_STENCIL_VIEW_DESC *pDesc, D_ID3D11DepthStencilView **ppDepthStencilView ) = 0 ;
	virtual HRESULT __stdcall CreateInputLayout			( const D_D3D11_INPUT_ELEMENT_DESC *pInputElementDescs, UINT NumElements, const void *pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength, D_ID3D11InputLayout **ppInputLayout ) = 0 ;
	virtual HRESULT __stdcall CreateVertexShader		( const void *pShaderBytecode, SIZE_T BytecodeLength, D_ID3D11ClassLinkage *pClassLinkage, D_ID3D11VertexShader **ppVertexShader ) = 0 ;
	virtual HRESULT __stdcall CreateGeometryShader		( const void *pShaderBytecode, SIZE_T BytecodeLength, D_ID3D11ClassLinkage *pClassLinkage, D_ID3D11GeometryShader **ppGeometryShader ) = 0 ;
	virtual HRESULT __stdcall CreateGeometryShaderWithStreamOutput	( const void *pShaderBytecode, SIZE_T BytecodeLength, const D_D3D11_SO_DECLARATION_ENTRY *pSODeclaration, UINT NumEntries, const UINT *pBufferStrides, UINT NumStrides, UINT RasterizedStream, D_ID3D11ClassLinkage *pClassLinkage, D_ID3D11GeometryShader **ppGeometryShader ) = 0 ;
	virtual HRESULT __stdcall CreatePixelShader			( const void *pShaderBytecode, SIZE_T BytecodeLength, D_ID3D11ClassLinkage *pClassLinkage, D_ID3D11PixelShader **ppPixelShader ) = 0 ;
	virtual HRESULT __stdcall CreateHullShader			( const void *pShaderBytecode, SIZE_T BytecodeLength, D_ID3D11ClassLinkage *pClassLinkage, D_ID3D11HullShader **ppHullShader ) = 0 ;
	virtual HRESULT __stdcall CreateDomainShader		( const void *pShaderBytecode, SIZE_T BytecodeLength, D_ID3D11ClassLinkage *pClassLinkage, D_ID3D11DomainShader **ppDomainShader ) = 0 ;
	virtual HRESULT __stdcall CreateComputeShader		( const void *pShaderBytecode, SIZE_T BytecodeLength, D_ID3D11ClassLinkage *pClassLinkage, D_ID3D11ComputeShader **ppComputeShader ) = 0 ;
	virtual HRESULT __stdcall CreateClassLinkage		( D_ID3D11ClassLinkage **ppLinkage ) = 0 ;
	virtual HRESULT __stdcall CreateBlendState			( const D_D3D11_BLEND_DESC         *pBlendStateDesc,   D_ID3D11BlendState        **ppBlendState        ) = 0 ;
	virtual HRESULT __stdcall CreateDepthStencilState	( const D_D3D11_DEPTH_STENCIL_DESC *pDepthStencilDesc, D_ID3D11DepthStencilState **ppDepthStencilState ) = 0 ;
	virtual HRESULT __stdcall CreateRasterizerState		( const D_D3D11_RASTERIZER_DESC    *pRasterizerDesc,   D_ID3D11RasterizerState   **ppRasterizerState   ) = 0 ;
	virtual HRESULT __stdcall CreateSamplerState		( const D_D3D11_SAMPLER_DESC       *pSamplerDesc,      D_ID3D11SamplerState      **ppSamplerState      ) = 0 ;
	virtual HRESULT __stdcall CreateQuery				( const D_D3D11_QUERY_DESC         *pQueryDesc,        D_ID3D11Query             **ppQuery             ) = 0 ;
	virtual HRESULT __stdcall CreatePredicate			( const D_D3D11_QUERY_DESC         *pPredicateDesc,    D_ID3D11Predicate         **ppPredicate         ) = 0 ;
	virtual HRESULT __stdcall CreateCounter				( const D_D3D11_COUNTER_DESC       *pCounterDesc,      D_ID3D11Counter           **ppCounter           ) = 0 ;
	virtual HRESULT __stdcall CreateDeferredContext		( UINT ContextFlags, D_ID3D11DeviceContext **ppDeferredContext ) = 0 ;
	virtual HRESULT __stdcall OpenSharedResource		( HANDLE hResource, REFIID ReturnedInterface, void **ppResource ) = 0 ;
	virtual HRESULT __stdcall CheckFormatSupport		( D_DXGI_FORMAT Format, UINT *pFormatSupport ) = 0 ;
	virtual HRESULT __stdcall CheckMultisampleQualityLevels	( D_DXGI_FORMAT Format, UINT SampleCount, UINT *pNumQualityLevels ) = 0 ;
	virtual void	__stdcall CheckCounterInfo			( D_D3D11_COUNTER_INFO *pCounterInfo ) = 0 ;
	virtual HRESULT __stdcall CheckCounter				( const D_D3D11_COUNTER_DESC *pDesc, D_D3D11_COUNTER_TYPE *pType, UINT *pActiveCounters, LPSTR szName, UINT *pNameLength, LPSTR szUnits, UINT *pUnitsLength, LPSTR szDescription, UINT *pDescriptionLength ) = 0 ;
	virtual HRESULT __stdcall CheckFeatureSupport		( D_D3D11_FEATURE Feature, void *pFeatureSupportData, UINT FeatureSupportDataSize ) = 0 ;
	virtual HRESULT __stdcall GetPrivateData			( REFGUID guid, UINT *pDataSize, void *pData ) = 0 ;
	virtual HRESULT __stdcall SetPrivateData			( REFGUID guid, UINT DataSize, const void *pData ) = 0 ;
	virtual HRESULT __stdcall SetPrivateDataInterface	( REFGUID guid, const D_IUnknown *pData ) = 0 ;
	virtual D_D3D_FEATURE_LEVEL __stdcall GetFeatureLevel( void ) = 0 ;
	virtual UINT	__stdcall GetCreationFlags			( void ) = 0 ;
	virtual HRESULT __stdcall GetDeviceRemovedReason	( void ) = 0 ;
	virtual void	__stdcall GetImmediateContext		( D_ID3D11DeviceContext **ppImmediateContext ) = 0 ;
	virtual HRESULT __stdcall SetExceptionMode			( UINT RaiseFlags ) = 0 ;
	virtual UINT	__stdcall GetExceptionMode			( void ) = 0 ;
};

typedef HRESULT ( WINAPI * LPD_CREATEDXGIFACTORY  )( REFIID riid, void **ppFactory ) ;
typedef HRESULT ( WINAPI * LPD_CREATEDXGIFACTORY1 )( REFIID riid, void **ppFactory ) ;

typedef HRESULT ( WINAPI * LPD_D3D11CREATEDEVICE )(
	D_IDXGIAdapter                *  pAdapter,
	D_D3D_DRIVER_TYPE                DriverType,
	HMODULE                          Software,
	UINT                             Flags,
	CONST D_D3D_FEATURE_LEVEL     *  pFeatureLevels,
	UINT                             FeatureLevels,
	UINT                             SDKVersion,
	D_ID3D11Device                ** ppDevice,
	D_D3D_FEATURE_LEVEL           *  pFeatureLevel,
	D_ID3D11DeviceContext         ** ppImmediateContext
) ;

typedef HRESULT ( CALLBACK *LPD_D3D11CREATEDEVICEANDSWAPCHAIN )( 
	D_IDXGIAdapter                *  pAdapter,
	D_D3D_DRIVER_TYPE                DriverType,
	HMODULE                          Software,
	UINT                             Flags,
	CONST D_D3D_FEATURE_LEVEL     *  pFeatureLevels,
	UINT                             FeatureLevels,
	UINT                             SDKVersion,
	const D_DXGI_SWAP_CHAIN_DESC  *  pSwapChainDesc,
	D_IDXGISwapChain              ** ppSwapChain,
	D_ID3D11Device                ** ppDevice,
	D_D3D_FEATURE_LEVEL           *  pFeatureLevel,
	D_ID3D11DeviceContext         ** ppImmediateContext
) ;



// Ｄｉｒｅｃｔ３Ｄ９以前 -----------------------------------------------------

#define D_D3D_OK								(D_DD_OK)

#define D_D3DENUMRET_CANCEL						D_DDENUMRET_CANCEL
#define D_D3DENUMRET_OK							D_DDENUMRET_OK

#define D_D3DPTEXTURECAPS_PERSPECTIVE			(0x00000001L)
#define D_D3DPTEXTURECAPS_POW2					(0x00000002L)
#define D_D3DPTEXTURECAPS_ALPHA					(0x00000004L)
#define D_D3DPTEXTURECAPS_SQUAREONLY			(0x00000020L)
#define D_D3DPTEXTURECAPS_TEXREPEATNOTSCALEDBYSIZE (0x00000040L)
#define D_D3DPTEXTURECAPS_ALPHAPALETTE			(0x00000080L)
#define D_D3DPTEXTURECAPS_NONPOW2CONDITIONAL	(0x00000100L)
#define D_D3DPTEXTURECAPS_PROJECTED				(0x00000400L)
#define D_D3DPTEXTURECAPS_CUBEMAP				(0x00000800L)
#define D_D3DPTEXTURECAPS_VOLUMEMAP				(0x00002000L)
#define D_D3DPTEXTURECAPS_MIPMAP				(0x00004000L)
#define D_D3DPTEXTURECAPS_MIPVOLUMEMAP			(0x00008000L)
#define D_D3DPTEXTURECAPS_MIPCUBEMAP			(0x00010000L)
#define D_D3DPTEXTURECAPS_CUBEMAP_POW2			(0x00020000L)
#define D_D3DPTEXTURECAPS_VOLUMEMAP_POW2		(0x00040000L)
#define D_D3DPTEXTURECAPS_NOPROJECTEDBUMPENV	(0x00200000L)

#define D_D3DPTFILTERCAPS_MINFPOINT				(0x00000100L)
#define D_D3DPTFILTERCAPS_MINFLINEAR			(0x00000200L)
#define D_D3DPTFILTERCAPS_MINFANISOTROPIC		(0x00000400L)
#define D_D3DPTFILTERCAPS_MINFPYRAMIDALQUAD		(0x00000800L)
#define D_D3DPTFILTERCAPS_MINFGAUSSIANQUAD		(0x00001000L)
#define D_D3DPTFILTERCAPS_MIPFPOINT				(0x00010000L)
#define D_D3DPTFILTERCAPS_MIPFLINEAR			(0x00020000L)
#define D_D3DPTFILTERCAPS_CONVOLUTIONMONO		(0x00040000L)
#define D_D3DPTFILTERCAPS_MAGFPOINT				(0x01000000L)
#define D_D3DPTFILTERCAPS_MAGFLINEAR			(0x02000000L)
#define D_D3DPTFILTERCAPS_MAGFANISOTROPIC		(0x04000000L)
#define D_D3DPTFILTERCAPS_MAGFPYRAMIDALQUAD		(0x08000000L)
#define D_D3DPTFILTERCAPS_MAGFGAUSSIANQUAD		(0x10000000L)


#define D_D3DTA_DIFFUSE							(0x00000000)
#define D_D3DTA_CURRENT							(0x00000001)
#define D_D3DTA_TEXTURE							(0x00000002)
#define D_D3DTA_TFACTOR							(0x00000003)
#define D_D3DTA_SPECULAR						(0x00000004)
#define D_D3DTA_TEMP							(0x00000005)
#define D_D3DTA_COMPLEMENT						(0x00000010)
#define D_D3DTA_ALPHAREPLICATE					(0x00000020)

#define D_D3DFVF_XYZ							(0x002)
#define D_D3DFVF_XYZRHW							(0x004)
#define D_D3DFVF_NORMAL							(0x010)
#define D_D3DFVF_DIFFUSE						(0x040)
#define D_D3DFVF_SPECULAR						(0x080)
#define D_D3DFVF_TEX0							(0x000)
#define D_D3DFVF_TEX1							(0x100)
#define D_D3DFVF_TEX2							(0x200)
#define D_D3DFVF_TEX3							(0x300)
#define D_D3DFVF_TEX4							(0x400)
#define D_D3DFVF_TEX5							(0x500)
#define D_D3DFVF_TEX6							(0x600)
#define D_D3DFVF_TEX7							(0x700)
#define D_D3DFVF_TEX8							(0x800)
#define D_D3DFVF_XYZB1							(0x006)
#define D_D3DFVF_XYZB2							(0x008)
#define D_D3DFVF_XYZB3							(0x00a)
#define D_D3DFVF_XYZB4							(0x00c)
#define D_D3DFVF_XYZB5							(0x00e)

#define D_D3DVBCAPS_SYSTEMMEMORY				(0x00000800l)
#define D_D3DVBCAPS_WRITEONLY					(0x00010000l)
#define D_D3DVBCAPS_OPTIMIZED					(0x80000000l)
#define D_D3DVBCAPS_DONOTCLIP					(0x00000001l)

#define D_RGBA_MAKE(r, g, b, a)					((D_D3DCOLOR) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b)))

#define D_D3DPRESENTFLAG_LOCKABLE_BACKBUFFER	(0x00000001)
#define D_D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL	(0x00000002)
#define D_D3DPRESENTFLAG_DEVICECLIP				(0x00000004)
#define D_D3DPRESENTFLAG_VIDEO					(0x00000010)

#define D_D3DPMISCCAPS_MASKZ					(0x00000002L)
#define D_D3DPMISCCAPS_CULLNONE					(0x00000010L)
#define D_D3DPMISCCAPS_CULLCW					(0x00000020L)
#define D_D3DPMISCCAPS_CULLCCW					(0x00000040L)
#define D_D3DPMISCCAPS_COLORWRITEENABLE			(0x00000080L)
#define D_D3DPMISCCAPS_CLIPPLANESCALEDPOINTS	(0x00000100L)
#define D_D3DPMISCCAPS_CLIPTLVERTS				(0x00000200L)
#define D_D3DPMISCCAPS_TSSARGTEMP				(0x00000400L)
#define D_D3DPMISCCAPS_BLENDOP					(0x00000800L)
#define D_D3DPMISCCAPS_NULLREFERENCE			(0x00001000L)
#define D_D3DPMISCCAPS_INDEPENDENTWRITEMASKS	(0x00004000L)
#define D_D3DPMISCCAPS_PERSTAGECONSTANT			(0x00008000L)
#define D_D3DPMISCCAPS_FOGANDSPECULARALPHA		(0x00010000L)
#define D_D3DPMISCCAPS_SEPARATEALPHABLEND		(0x00020000L)
#define D_D3DPMISCCAPS_MRTINDEPENDENTBITDEPTHS	(0x00040000L)
#define D_D3DPMISCCAPS_MRTPOSTPIXELSHADERBLENDING	(0x00080000L)
#define D_D3DPMISCCAPS_FOGVERTEXCLAMPED			(0x00100000L)

#define D_D3DFVF_TEXTUREFORMAT2					(( DWORD )0)
#define D_D3DFVF_TEXTUREFORMAT1					(( DWORD )3)
#define D_D3DFVF_TEXTUREFORMAT3					(( DWORD )1)
#define D_D3DFVF_TEXTUREFORMAT4					(( DWORD )2)

#define D_D3DFVF_TEXCOORDSIZE2( CoordIndex )	( D_D3DFVF_TEXTUREFORMAT2 << ( DWORD )( CoordIndex * 2 + 16 ) )
#define D_D3DFVF_TEXCOORDSIZE1( CoordIndex )	( D_D3DFVF_TEXTUREFORMAT1 << ( DWORD )( CoordIndex * 2 + 16 ) )
#define D_D3DFVF_TEXCOORDSIZE3( CoordIndex )	( D_D3DFVF_TEXTUREFORMAT3 << ( DWORD )( CoordIndex * 2 + 16 ) )
#define D_D3DFVF_TEXCOORDSIZE4( CoordIndex )	( D_D3DFVF_TEXTUREFORMAT4 << ( DWORD )( CoordIndex * 2 + 16 ) )

typedef enum tagD_D3DVERTEXBLENDFLAGS
{
	D3DVBLEND_DISABLE							= 0,
	D3DVBLEND_1WEIGHT							= 1,
	D3DVBLEND_2WEIGHTS							= 2,
	D3DVBLEND_3WEIGHTS							= 3,
} D_D3DVERTEXBLENDFLAGS;

typedef enum tagD_D3DCULL
{
	D_D3DCULL_NONE								= 1,
	D_D3DCULL_CW								= 2,
	D_D3DCULL_CCW								= 3,
	D_D3DCULL_FORCE_DWORD						= 0x7fffffff,
} D_D3DCULL;

typedef enum tagD_D3DZBUFFERTYPE
{
	D_D3DZB_FALSE								= 0,
	D_D3DZB_TRUE								= 1,
	D_D3DZB_USEW								= 2,
	D_D3DZB_FORCE_DWORD							= 0x7fffffff,
} D_D3DZBUFFERTYPE;

typedef enum tagD_D3DBLEND
{
	D_D3DBLEND_ZERO								= 1,
	D_D3DBLEND_ONE								= 2,
	D_D3DBLEND_SRCCOLOR							= 3,
	D_D3DBLEND_INVSRCCOLOR						= 4,
	D_D3DBLEND_SRCALPHA							= 5,
	D_D3DBLEND_INVSRCALPHA						= 6,
	D_D3DBLEND_DESTALPHA						= 7,
	D_D3DBLEND_INVDESTALPHA						= 8,
	D_D3DBLEND_DESTCOLOR						= 9,
	D_D3DBLEND_INVDESTCOLOR						= 10,
	D_D3DBLEND_SRCALPHASAT						= 11,
	D_D3DBLEND_BOTHSRCALPHA						= 12,
	D_D3DBLEND_BOTHINVSRCALPHA					= 13,
	D_D3DBLEND_FORCE_DWORD						= 0x7fffffff,
} D_D3DBLEND;

typedef enum tagD_D3DMATERIALCOLORSOURCE
{
	D_D3DMCS_MATERIAL							= 0,
	D_D3DMCS_COLOR1								= 1,
	D_D3DMCS_COLOR2								= 2,
	D_D3DMCS_FORCE_DWORD						= 0x7fffffff,
} D_D3DMATERIALCOLORSOURCE;

typedef enum tagD_D3DTEXTUREOP
{
	D_D3DTOP_DISABLE							= 1,
	D_D3DTOP_SELECTARG1							= 2,
	D_D3DTOP_SELECTARG2							= 3,

	D_D3DTOP_MODULATE							= 4,
	D_D3DTOP_MODULATE2X							= 5,
	D_D3DTOP_MODULATE4X							= 6,

	D_D3DTOP_ADD								= 7,
	D_D3DTOP_ADDSIGNED							= 8,
	D_D3DTOP_ADDSIGNED2X						= 9,
	D_D3DTOP_SUBTRACT							= 10,
	D_D3DTOP_ADDSMOOTH							= 11,

	D_D3DTOP_BLENDDIFFUSEALPHA					= 12,
	D_D3DTOP_BLENDTEXTUREALPHA					= 13,
	D_D3DTOP_BLENDFACTORALPHA					= 14,

	D_D3DTOP_BLENDTEXTUREALPHAPM				= 15,
	D_D3DTOP_BLENDCURRENTALPHA					= 16,

	D_D3DTOP_PREMODULATE						= 17,
	D_D3DTOP_MODULATEALPHA_ADDCOLOR				= 18,
	D_D3DTOP_MODULATECOLOR_ADDALPHA				= 19,
	D_D3DTOP_MODULATEINVALPHA_ADDCOLOR			= 20,
	D_D3DTOP_MODULATEINVCOLOR_ADDALPHA			= 21,

	D_D3DTOP_BUMPENVMAP							= 22,
	D_D3DTOP_BUMPENVMAPLUMINANCE				= 23,
	D_D3DTOP_DOTPRODUCT3						= 24,

	D_D3DTOP_FORCE_DWORD						= 0x7fffffff,
} D_D3DTEXTUREOP;

typedef enum tagD_D3DSHADEMODE
{
	D_D3DSHADE_FLAT								= 1,
	D_D3DSHADE_GOURAUD							= 2,
	D_D3DSHADE_PHONG							= 3,
	D_D3DSHADE_FORCE_DWORD						= 0x7fffffff,
} D_D3DSHADEMODE;

typedef enum tagD_D3DFOGMODE {
	D_D3DFOG_NONE								= 0,
	D_D3DFOG_EXP								= 1,
	D_D3DFOG_EXP2								= 2,
	D_D3DFOG_LINEAR								= 3,
	D_D3DFOG_FORCE_DWORD						= 0x7fffffff,
} D_D3DFOGMODE;

typedef enum tagD_D3DTEXTUREMAGFILTER
{
	D_D3DTFG_POINT								= 1,
	D_D3DTFG_LINEAR								= 2,
	D_D3DTFG_FLATCUBIC							= 3,
	D_D3DTFG_GAUSSIANCUBIC						= 4,
	D_D3DTFG_ANISOTROPIC						= 5,
	D_D3DTFG_FORCE_DWORD						= 0x7fffffff,
} D_D3DTEXTUREMAGFILTER;

typedef enum tagD_D3DTEXTUREMINFILTER
{
	D_D3DTFN_POINT								= 1,
	D_D3DTFN_LINEAR								= 2,
	D_D3DTFN_ANISOTROPIC						= 3,
	D_D3DTFN_FORCE_DWORD						= 0x7fffffff,
} D_D3DTEXTUREMINFILTER;

typedef enum tagD_D3DTEXTUREMIPFILTER
{
	D_D3DTFP_NONE								= 1,
	D_D3DTFP_POINT								= 2,
	D_D3DTFP_LINEAR								= 3,
	D_D3DTFP_FORCE_DWORD						= 0x7fffffff,
} D_D3DTEXTUREMIPFILTER;

typedef enum tagD_D3DTEXTUREADDRESS
{
	D_D3DTADDRESS_WRAP							= 1,
	D_D3DTADDRESS_MIRROR						= 2,
	D_D3DTADDRESS_CLAMP							= 3,
	D_D3DTADDRESS_BORDER						= 4,
	D_D3DTADDRESS_MIRRORONCE					= 5,
	D_D3DTADDRESS_FORCE_DWORD					= 0x7fffffff,
} D_D3DTEXTUREADDRESS;

typedef enum tagD_D3DPRIMITIVETYPE
{
	D_D3DPT_POINTLIST							= 1,
	D_D3DPT_LINELIST							= 2,
	D_D3DPT_LINESTRIP							= 3,
	D_D3DPT_TRIANGLELIST						= 4,
	D_D3DPT_TRIANGLESTRIP						= 5,
	D_D3DPT_TRIANGLEFAN							= 6,
	D_D3DPT_FORCE_DWORD							= 0x7fffffff,
} D_D3DPRIMITIVETYPE;

typedef enum tagD_D3DCMPFUNC
{
	D_D3DCMP_NEVER								= 1, 
	D_D3DCMP_LESS								= 2, 
	D_D3DCMP_EQUAL								= 3, 
	D_D3DCMP_LESSEQUAL							= 4, 
	D_D3DCMP_GREATER							= 5, 
	D_D3DCMP_NOTEQUAL							= 6, 
	D_D3DCMP_GREATEREQUAL						= 7, 
	D_D3DCMP_ALWAYS								= 8, 
	D_D3DCMP_FORCE_DWORD						= 0x7fffffff, 
} D_D3DCMPFUNC ;

typedef enum tagD_D3DANTIALIASMODE
{
	D_D3DANTIALIAS_NONE							= 0,
	D_D3DANTIALIAS_SORTDEPENDENT				= 1,
	D_D3DANTIALIAS_SORTINDEPENDENT				= 2,
	D_D3DANTIALIAS_FORCE_DWORD					= 0x7fffffff,
} D_D3DANTIALIASMODE;

typedef enum tagD_D3DRENDERSTATETYPE
{
	D_D3DRENDERSTATE_ANTIALIAS					= 2,
	D_D3DRENDERSTATE_TEXTUREPERSPECTIVE			= 4,
	D_D3DRENDERSTATE_ZENABLE					= 7,
	D_D3DRENDERSTATE_FILLMODE					= 8,
	D_D3DRENDERSTATE_SHADEMODE					= 9,
	D_D3DRENDERSTATE_LINEPATTERN				= 10,
	D_D3DRENDERSTATE_ZWRITEENABLE				= 14,
	D_D3DRENDERSTATE_ALPHATESTENABLE			= 15,
	D_D3DRENDERSTATE_LASTPIXEL					= 16,
	D_D3DRENDERSTATE_SRCBLEND					= 19,
	D_D3DRENDERSTATE_DESTBLEND					= 20,
	D_D3DRENDERSTATE_CULLMODE					= 22,
	D_D3DRENDERSTATE_ZFUNC						= 23,
	D_D3DRENDERSTATE_ALPHAREF					= 24,
	D_D3DRENDERSTATE_ALPHAFUNC					= 25,
	D_D3DRENDERSTATE_DITHERENABLE				= 26,
	D_D3DRENDERSTATE_ALPHABLENDENABLE			= 27,
	D_D3DRENDERSTATE_FOGENABLE					= 28,
	D_D3DRENDERSTATE_SPECULARENABLE				= 29,
	D_D3DRENDERSTATE_ZVISIBLE					= 30,
	D_D3DRENDERSTATE_STIPPLEDALPHA				= 33,
	D_D3DRENDERSTATE_FOGCOLOR					= 34,
	D_D3DRENDERSTATE_FOGTABLEMODE				= 35,
	D_D3DRENDERSTATE_FOGSTART					= 36,
	D_D3DRENDERSTATE_FOGEND						= 37,
	D_D3DRENDERSTATE_FOGDENSITY					= 38,
	D_D3DRENDERSTATE_EDGEANTIALIAS				= 40,
	D_D3DRENDERSTATE_COLORKEYENABLE				= 41,
	D_D3DRENDERSTATE_ZBIAS						= 47,
	D_D3DRENDERSTATE_RANGEFOGENABLE				= 48,
	D_D3DRENDERSTATE_STENCILENABLE				= 52,
	D_D3DRENDERSTATE_STENCILFAIL				= 53,
	D_D3DRENDERSTATE_STENCILZFAIL				= 54,
	D_D3DRENDERSTATE_STENCILPASS				= 55,
	D_D3DRENDERSTATE_STENCILFUNC				= 56,
	D_D3DRENDERSTATE_STENCILREF					= 57,
	D_D3DRENDERSTATE_STENCILMASK				= 58,
	D_D3DRENDERSTATE_STENCILWRITEMASK			= 59,
	D_D3DRENDERSTATE_TEXTUREFACTOR				= 60,
	D_D3DRENDERSTATE_WRAP0						= 128,
	D_D3DRENDERSTATE_WRAP1						= 129,
	D_D3DRENDERSTATE_WRAP2						= 130,
	D_D3DRENDERSTATE_WRAP3						= 131,
	D_D3DRENDERSTATE_WRAP4						= 132,
	D_D3DRENDERSTATE_WRAP5						= 133,
	D_D3DRENDERSTATE_WRAP6						= 134,
	D_D3DRENDERSTATE_WRAP7						= 135,
	D_D3DRENDERSTATE_CLIPPING					= 136,
	D_D3DRENDERSTATE_LIGHTING					= 137,
	D_D3DRENDERSTATE_EXTENTS					= 138,
	D_D3DRENDERSTATE_AMBIENT					= 139,
	D_D3DRENDERSTATE_FOGVERTEXMODE				= 140,
	D_D3DRENDERSTATE_COLORVERTEX				= 141,
	D_D3DRENDERSTATE_LOCALVIEWER				= 142,
	D_D3DRENDERSTATE_NORMALIZENORMALS			= 143,
	D_D3DRENDERSTATE_COLORKEYBLENDENABLE		= 144,
	D_D3DRENDERSTATE_DIFFUSEMATERIALSOURCE		= 145,
	D_D3DRENDERSTATE_SPECULARMATERIALSOURCE		= 146,
	D_D3DRENDERSTATE_AMBIENTMATERIALSOURCE		= 147,
	D_D3DRENDERSTATE_EMISSIVEMATERIALSOURCE		= 148,
	D_D3DRENDERSTATE_VERTEXBLEND				= 151,
	D_D3DRENDERSTATE_CLIPPLANEENABLE			= 152,

	D_D3DRENDERSTATE_TEXTUREHANDLE				= 1,
	D_D3DRENDERSTATE_TEXTUREADDRESS				= 3,
	D_D3DRENDERSTATE_WRAPU						= 5,
	D_D3DRENDERSTATE_WRAPV						= 6,
	D_D3DRENDERSTATE_MONOENABLE					= 11,
	D_D3DRENDERSTATE_ROP2						= 12,
	D_D3DRENDERSTATE_PLANEMASK					= 13,
	D_D3DRENDERSTATE_TEXTUREMAG					= 17,
	D_D3DRENDERSTATE_TEXTUREMIN					= 18,
	D_D3DRENDERSTATE_TEXTUREMAPBLEND			= 21,
	D_D3DRENDERSTATE_SUBPIXEL					= 31,
	D_D3DRENDERSTATE_SUBPIXELX					= 32,
	D_D3DRENDERSTATE_STIPPLEENABLE				= 39,
	D_D3DRENDERSTATE_BORDERCOLOR				= 43,
	D_D3DRENDERSTATE_TEXTUREADDRESSU			= 44,
	D_D3DRENDERSTATE_TEXTUREADDRESSV			= 45,
	D_D3DRENDERSTATE_MIPMAPLODBIAS				= 46,
	D_D3DRENDERSTATE_ANISOTROPY					= 49,
	D_D3DRENDERSTATE_FLUSHBATCH					= 50,
	D_D3DRENDERSTATE_TRANSLUCENTSORTINDEPENDENT	= 51,
	D_D3DRENDERSTATE_STIPPLEPATTERN00			= 64,
	D_D3DRENDERSTATE_STIPPLEPATTERN01			= 65,
	D_D3DRENDERSTATE_STIPPLEPATTERN02			= 66,
	D_D3DRENDERSTATE_STIPPLEPATTERN03			= 67,
	D_D3DRENDERSTATE_STIPPLEPATTERN04			= 68,
	D_D3DRENDERSTATE_STIPPLEPATTERN05			= 69,
	D_D3DRENDERSTATE_STIPPLEPATTERN06			= 70,
	D_D3DRENDERSTATE_STIPPLEPATTERN07			= 71,
	D_D3DRENDERSTATE_STIPPLEPATTERN08			= 72,
	D_D3DRENDERSTATE_STIPPLEPATTERN09			= 73,
	D_D3DRENDERSTATE_STIPPLEPATTERN10			= 74,
	D_D3DRENDERSTATE_STIPPLEPATTERN11			= 75,
	D_D3DRENDERSTATE_STIPPLEPATTERN12			= 76,
	D_D3DRENDERSTATE_STIPPLEPATTERN13			= 77,
	D_D3DRENDERSTATE_STIPPLEPATTERN14			= 78,
	D_D3DRENDERSTATE_STIPPLEPATTERN15			= 79,
	D_D3DRENDERSTATE_STIPPLEPATTERN16			= 80,
	D_D3DRENDERSTATE_STIPPLEPATTERN17			= 81,
	D_D3DRENDERSTATE_STIPPLEPATTERN18			= 82,
	D_D3DRENDERSTATE_STIPPLEPATTERN19			= 83,
	D_D3DRENDERSTATE_STIPPLEPATTERN20			= 84,
	D_D3DRENDERSTATE_STIPPLEPATTERN21			= 85,
	D_D3DRENDERSTATE_STIPPLEPATTERN22			= 86,
	D_D3DRENDERSTATE_STIPPLEPATTERN23			= 87,
	D_D3DRENDERSTATE_STIPPLEPATTERN24			= 88,
	D_D3DRENDERSTATE_STIPPLEPATTERN25			= 89,
	D_D3DRENDERSTATE_STIPPLEPATTERN26			= 90,
	D_D3DRENDERSTATE_STIPPLEPATTERN27			= 91,
	D_D3DRENDERSTATE_STIPPLEPATTERN28			= 92,
	D_D3DRENDERSTATE_STIPPLEPATTERN29			= 93,
	D_D3DRENDERSTATE_STIPPLEPATTERN30			= 94,
	D_D3DRENDERSTATE_STIPPLEPATTERN31			= 95,

	D_D3DRENDERSTATE_FOGTABLESTART				= 36,
	D_D3DRENDERSTATE_FOGTABLEEND				= 37,
	D_D3DRENDERSTATE_FOGTABLEDENSITY			= 38,

	D_D3DRENDERSTATE_FORCE_DWORD				= 0x7fffffff,

	D_D3DRS_ZENABLE								= 7,
	D_D3DRS_FILLMODE							= 8,
	D_D3DRS_SHADEMODE							= 9,
	D_D3DRS_ZWRITEENABLE						= 14,
	D_D3DRS_ALPHATESTENABLE						= 15,
	D_D3DRS_LASTPIXEL							= 16,
	D_D3DRS_SRCBLEND							= 19,
	D_D3DRS_DESTBLEND							= 20,
	D_D3DRS_CULLMODE							= 22,
	D_D3DRS_ZFUNC								= 23,
	D_D3DRS_ALPHAREF							= 24,
	D_D3DRS_ALPHAFUNC							= 25,
	D_D3DRS_DITHERENABLE						= 26,
	D_D3DRS_ALPHABLENDENABLE					= 27,
	D_D3DRS_FOGENABLE							= 28,
	D_D3DRS_SPECULARENABLE						= 29,
	D_D3DRS_FOGCOLOR							= 34,
	D_D3DRS_FOGTABLEMODE						= 35,
	D_D3DRS_FOGSTART							= 36,
	D_D3DRS_FOGEND								= 37,
	D_D3DRS_FOGDENSITY							= 38,
	D_D3DRS_RANGEFOGENABLE						= 48,
	D_D3DRS_STENCILENABLE						= 52,
	D_D3DRS_STENCILFAIL							= 53,
	D_D3DRS_STENCILZFAIL						= 54,
	D_D3DRS_STENCILPASS							= 55,
	D_D3DRS_STENCILFUNC							= 56,
	D_D3DRS_STENCILREF							= 57,
	D_D3DRS_STENCILMASK							= 58,
	D_D3DRS_STENCILWRITEMASK					= 59,
	D_D3DRS_TEXTUREFACTOR						= 60,
	D_D3DRS_WRAP0								= 128,
	D_D3DRS_WRAP1								= 129,
	D_D3DRS_WRAP2								= 130,
	D_D3DRS_WRAP3								= 131,
	D_D3DRS_WRAP4								= 132,
	D_D3DRS_WRAP5								= 133,
	D_D3DRS_WRAP6								= 134,
	D_D3DRS_WRAP7								= 135,
	D_D3DRS_CLIPPING							= 136,
	D_D3DRS_LIGHTING							= 137,
	D_D3DRS_AMBIENT								= 139,
	D_D3DRS_FOGVERTEXMODE						= 140,
	D_D3DRS_COLORVERTEX							= 141,
	D_D3DRS_LOCALVIEWER							= 142,
	D_D3DRS_NORMALIZENORMALS					= 143,
	D_D3DRS_DIFFUSEMATERIALSOURCE				= 145,
	D_D3DRS_SPECULARMATERIALSOURCE				= 146,
	D_D3DRS_AMBIENTMATERIALSOURCE				= 147,
	D_D3DRS_EMISSIVEMATERIALSOURCE				= 148,
	D_D3DRS_VERTEXBLEND							= 151,
	D_D3DRS_CLIPPLANEENABLE						= 152,
	D_D3DRS_POINTSIZE							= 154,
	D_D3DRS_POINTSIZE_MIN						= 155,
	D_D3DRS_POINTSPRITEENABLE					= 156,
	D_D3DRS_POINTSCALEENABLE					= 157,
	D_D3DRS_POINTSCALE_A						= 158,
	D_D3DRS_POINTSCALE_B						= 159,
	D_D3DRS_POINTSCALE_C						= 160,
	D_D3DRS_MULTISAMPLEANTIALIAS				= 161,
	D_D3DRS_MULTISAMPLEMASK						= 162,
	D_D3DRS_PATCHEDGESTYLE						= 163,
	D_D3DRS_DEBUGMONITORTOKEN					= 165,
	D_D3DRS_POINTSIZE_MAX						= 166,
	D_D3DRS_INDEXEDVERTEXBLENDENABLE			= 167,
	D_D3DRS_COLORWRITEENABLE					= 168,
	D_D3DRS_TWEENFACTOR							= 170,
	D_D3DRS_BLENDOP								= 171,
	D_D3DRS_POSITIONDEGREE						= 172,
	D_D3DRS_NORMALDEGREE						= 173,
	D_D3DRS_SCISSORTESTENABLE					= 174,
	D_D3DRS_SLOPESCALEDEPTHBIAS					= 175,
	D_D3DRS_ANTIALIASEDLINEENABLE				= 176,
	D_D3DRS_MINTESSELLATIONLEVEL				= 178,
	D_D3DRS_MAXTESSELLATIONLEVEL				= 179,
	D_D3DRS_ADAPTIVETESS_X						= 180,
	D_D3DRS_ADAPTIVETESS_Y						= 181,
	D_D3DRS_ADAPTIVETESS_Z						= 182,
	D_D3DRS_ADAPTIVETESS_W						= 183,
	D_D3DRS_ENABLEADAPTIVETESSELLATION			= 184,
	D_D3DRS_TWOSIDEDSTENCILMODE					= 185,
	D_D3DRS_CCW_STENCILFAIL						= 186,
	D_D3DRS_CCW_STENCILZFAIL					= 187,
	D_D3DRS_CCW_STENCILPASS						= 188,
	D_D3DRS_CCW_STENCILFUNC						= 189,
	D_D3DRS_COLORWRITEENABLE1					= 190,
	D_D3DRS_COLORWRITEENABLE2					= 191,
	D_D3DRS_COLORWRITEENABLE3					= 192,
	D_D3DRS_BLENDFACTOR							= 193,
	D_D3DRS_SRGBWRITEENABLE						= 194,
	D_D3DRS_DEPTHBIAS							= 195,
	D_D3DRS_WRAP8								= 198,
	D_D3DRS_WRAP9								= 199,
	D_D3DRS_WRAP10								= 200,
	D_D3DRS_WRAP11								= 201,
	D_D3DRS_WRAP12								= 202,
	D_D3DRS_WRAP13								= 203,
	D_D3DRS_WRAP14								= 204,
	D_D3DRS_WRAP15								= 205,
	D_D3DRS_SEPARATEALPHABLENDENABLE			= 206,
	D_D3DRS_SRCBLENDALPHA						= 207,
	D_D3DRS_DESTBLENDALPHA						= 208,
	D_D3DRS_BLENDOPALPHA						= 209,

	D_D3DRS_FORCE_DWORD							= 0x7fffffff,
} D_D3DRENDERSTATETYPE;

typedef enum tagD_D3DSAMPLERSTATETYPE
{
	D_D3DSAMP_ADDRESSU							= 1,
	D_D3DSAMP_ADDRESSV							= 2,
	D_D3DSAMP_ADDRESSW							= 3,
	D_D3DSAMP_BORDERCOLOR						= 4,
	D_D3DSAMP_MAGFILTER							= 5,
	D_D3DSAMP_MINFILTER							= 6,
	D_D3DSAMP_MIPFILTER							= 7,
	D_D3DSAMP_MIPMAPLODBIAS						= 8,
	D_D3DSAMP_MAXMIPLEVEL						= 9,
	D_D3DSAMP_MAXANISOTROPY						= 10,
	D_D3DSAMP_SRGBTEXTURE						= 11,
	D_D3DSAMP_ELEMENTINDEX						= 12,
	D_D3DSAMP_DMAPOFFSET						= 13,
	D_D3DSAMP_FORCE_DWORD						= 0x7fffffff,
} D_D3DSAMPLERSTATETYPE;

typedef enum tagD_D3DSTATEBLOCKTYPE
{
	D_D3DSBT_ALL								= 1,
	D_D3DSBT_PIXELSTATE							= 2,
	D_D3DSBT_VERTEXSTATE						= 3,
	D_D3DSBT_FORCE_DWORD						= 0x7fffffff,
} D_D3DSTATEBLOCKTYPE;

typedef enum tagD_D3DTEXTURESTAGESTATETYPE
{
	D_D3DTSS_COLOROP							= 1,
	D_D3DTSS_COLORARG1							= 2,
	D_D3DTSS_COLORARG2							= 3,
	D_D3DTSS_ALPHAOP							= 4,
	D_D3DTSS_ALPHAARG1							= 5,
	D_D3DTSS_ALPHAARG2							= 6,
	D_D3DTSS_BUMPENVMAT00						= 7,
	D_D3DTSS_BUMPENVMAT01						= 8,
	D_D3DTSS_BUMPENVMAT10						= 9,
	D_D3DTSS_BUMPENVMAT11						= 10,
	D_D3DTSS_TEXCOORDINDEX						= 11,
	D_D3DTSS_ADDRESS							= 12,
	D_D3DTSS_ADDRESSU							= 13,
	D_D3DTSS_ADDRESSV							= 14,
	D_D3DTSS_BORDERCOLOR						= 15,
	D_D3DTSS_MAGFILTER							= 16,
	D_D3DTSS_MINFILTER							= 17,
	D_D3DTSS_MIPFILTER							= 18,
	D_D3DTSS_MIPMAPLODBIAS						= 19,
	D_D3DTSS_MAXMIPLEVEL						= 20,
	D_D3DTSS_MAXANISOTROPY						= 21,
	D_D3DTSS_BUMPENVLSCALE						= 22,
	D_D3DTSS_BUMPENVLOFFSET						= 23,
	D_D3DTSS_TEXTURETRANSFORMFLAGS				= 24,
	D_D3DTSS_COLORARG0							= 26,
	D_D3DTSS_ALPHAARG0							= 27,
	D_D3DTSS_RESULTARG							= 28,
	D_D3DTSS_CONSTANT							= 32,
	D_D3DTSS_FORCE_DWORD						= 0x7fffffff,
} D_D3DTEXTURESTAGESTATETYPE ;

typedef enum tagD_D3DTEXTURETRANSFORMFLAGS
{
	D_D3DTTFF_DISABLE							= 0,
	D_D3DTTFF_COUNT1							= 1,
	D_D3DTTFF_COUNT2							= 2,
	D_D3DTTFF_COUNT3							= 3,
	D_D3DTTFF_COUNT4							= 4,
	D_D3DTTFF_PROJECTED							= 256,
	D_D3DTTFF_FORCE_DWORD						= 0x7fffffff,
} D_D3DTEXTURETRANSFORMFLAGS;

typedef enum tagD_D3DTRANSFORMSTATETYPE
{
	D_D3DTRANSFORMSTATE_WORLD					= 1,
	D_D3DTRANSFORMSTATE_VIEW					= 2,
	D_D3DTRANSFORMSTATE_PROJECTION				= 3,
	D_D3DTRANSFORMSTATE_WORLD1					= 4,
	D_D3DTRANSFORMSTATE_WORLD2					= 5,
	D_D3DTRANSFORMSTATE_WORLD3					= 6,
	D_D3DTRANSFORMSTATE_TEXTURE0				= 16,
	D_D3DTRANSFORMSTATE_TEXTURE1				= 17,
	D_D3DTRANSFORMSTATE_TEXTURE2				= 18,
	D_D3DTRANSFORMSTATE_TEXTURE3				= 19,
	D_D3DTRANSFORMSTATE_TEXTURE4				= 20,
	D_D3DTRANSFORMSTATE_TEXTURE5				= 21,
	D_D3DTRANSFORMSTATE_TEXTURE6				= 22,
	D_D3DTRANSFORMSTATE_TEXTURE7				= 23,
	D_D3DTRANSFORMSTATE_FORCE_DWORD				= 0x7fffffff,

	D_D3DTS_VIEW								= 2,
	D_D3DTS_PROJECTION							= 3,
	D_D3DTS_TEXTURE0							= 16,
	D_D3DTS_TEXTURE1							= 17,
	D_D3DTS_TEXTURE2							= 18,
	D_D3DTS_TEXTURE3							= 19,
	D_D3DTS_TEXTURE4							= 20,
	D_D3DTS_TEXTURE5							= 21,
	D_D3DTS_TEXTURE6							= 22,
	D_D3DTS_TEXTURE7							= 23,
	D_D3DTS_FORCE_DWORD							= 0x7fffffff,
} D_D3DTRANSFORMSTATETYPE ;

typedef enum tagD_D3DLIGHTTYPE
{
	D_D3DLIGHT_POINT							= 1,
	D_D3DLIGHT_SPOT								= 2,
	D_D3DLIGHT_DIRECTIONAL						= 3,
	D_D3DLIGHT_PARALLELPOINT					= 4,
	D_D3DLIGHT_GLSPOT							= 5,
	D_D3DLIGHT_FORCE_DWORD						= 0x7fffffff,
} D_D3DLIGHTTYPE ;

typedef enum D_D3DFORMAT
{
	D_D3DFMT_UNKNOWN							= 0,

	D_D3DFMT_R8G8B8								= 20,
	D_D3DFMT_A8R8G8B8							= 21,
	D_D3DFMT_X8R8G8B8							= 22,
	D_D3DFMT_R5G6B5								= 23,
	D_D3DFMT_X1R5G5B5							= 24,
	D_D3DFMT_A1R5G5B5							= 25,
	D_D3DFMT_A4R4G4B4							= 26,
	D_D3DFMT_R3G3B2								= 27,
	D_D3DFMT_A8									= 28,
	D_D3DFMT_A8R3G3B2							= 29,
	D_D3DFMT_X4R4G4B4							= 30,
	D_D3DFMT_A2B10G10R10						= 31,
	D_D3DFMT_A8B8G8R8							= 32,
	D_D3DFMT_X8B8G8R8							= 33,
	D_D3DFMT_G16R16								= 34,
	D_D3DFMT_A2R10G10B10						= 35,
	D_D3DFMT_A16B16G16R16						= 36,

	D_D3DFMT_A8P8								= 40,
	D_D3DFMT_P8									= 41,

	D_D3DFMT_L8									= 50,
	D_D3DFMT_A8L8								= 51,
	D_D3DFMT_A4L4								= 52,

	D_D3DFMT_V8U8								= 60,
	D_D3DFMT_L6V5U5								= 61,
	D_D3DFMT_X8L8V8U8							= 62,
	D_D3DFMT_Q8W8V8U8							= 63,
	D_D3DFMT_V16U16								= 64,
	D_D3DFMT_A2W10V10U10						= 67,

	D_D3DFMT_UYVY								= ((DWORD)(BYTE)('U') | ((DWORD)(BYTE)('Y') << 8) | ((DWORD)(BYTE)('V') << 16) | ((DWORD)(BYTE)('Y') << 24 )),
	D_D3DFMT_R8G8_B8G8							= ((DWORD)(BYTE)('R') | ((DWORD)(BYTE)('G') << 8) | ((DWORD)(BYTE)('B') << 16) | ((DWORD)(BYTE)('G') << 24 )),
	D_D3DFMT_YUY2								= ((DWORD)(BYTE)('Y') | ((DWORD)(BYTE)('U') << 8) | ((DWORD)(BYTE)('Y') << 16) | ((DWORD)(BYTE)('2') << 24 )),
	D_D3DFMT_G8R8_G8B8							= ((DWORD)(BYTE)('G') | ((DWORD)(BYTE)('R') << 8) | ((DWORD)(BYTE)('G') << 16) | ((DWORD)(BYTE)('B') << 24 )),
	D_D3DFMT_DXT1								= ((DWORD)(BYTE)('D') | ((DWORD)(BYTE)('X') << 8) | ((DWORD)(BYTE)('T') << 16) | ((DWORD)(BYTE)('1') << 24 )),
	D_D3DFMT_DXT2								= ((DWORD)(BYTE)('D') | ((DWORD)(BYTE)('X') << 8) | ((DWORD)(BYTE)('T') << 16) | ((DWORD)(BYTE)('2') << 24 )),
	D_D3DFMT_DXT3								= ((DWORD)(BYTE)('D') | ((DWORD)(BYTE)('X') << 8) | ((DWORD)(BYTE)('T') << 16) | ((DWORD)(BYTE)('3') << 24 )),
	D_D3DFMT_DXT4								= ((DWORD)(BYTE)('D') | ((DWORD)(BYTE)('X') << 8) | ((DWORD)(BYTE)('T') << 16) | ((DWORD)(BYTE)('4') << 24 )),
	D_D3DFMT_DXT5								= ((DWORD)(BYTE)('D') | ((DWORD)(BYTE)('X') << 8) | ((DWORD)(BYTE)('T') << 16) | ((DWORD)(BYTE)('5') << 24 )),

	D_D3DFMT_D16_LOCKABLE						= 70,
	D_D3DFMT_D32								= 71,
	D_D3DFMT_D15S1								= 73,
	D_D3DFMT_D24S8								= 75,
	D_D3DFMT_D24X8								= 77,
	D_D3DFMT_D24X4S4							= 79,
	D_D3DFMT_D16								= 80,

	D_D3DFMT_D32F_LOCKABLE						= 82,
	D_D3DFMT_D24FS8								= 83,


	D_D3DFMT_L16								= 81,

	D_D3DFMT_VERTEXDATA							= 100,
	D_D3DFMT_INDEX16							= 101,
	D_D3DFMT_INDEX32							= 102,

	D_D3DFMT_Q16W16V16U16						= 110,

	D_D3DFMT_MULTI2_ARGB8						= ((DWORD)(BYTE)('M') | ((DWORD)(BYTE)('E') << 8) | ((DWORD)(BYTE)('T') << 16) | ((DWORD)(BYTE)('1') << 24 )),

	D_D3DFMT_R16F								= 111,
	D_D3DFMT_G16R16F							= 112,
	D_D3DFMT_A16B16G16R16F						= 113,

	D_D3DFMT_R32F								= 114,
	D_D3DFMT_G32R32F							= 115,
	D_D3DFMT_A32B32G32R32F						= 116,

	D_D3DFMT_CxV8U8								= 117,

	D_D3DFMT_FORCE_DWORD						= 0x7fffffff
} D_D3DFORMAT ;

typedef enum tagD_D3DQUERYTYPE
{
	D_D3DQUERYTYPE_VCACHE						= 4,
	D_D3DQUERYTYPE_RESOURCEMANAGER				= 5,
	D_D3DQUERYTYPE_VERTEXSTATS					= 6,
	D_D3DQUERYTYPE_EVENT						= 8,
	D_D3DQUERYTYPE_OCCLUSION					= 9,
} D_D3DQUERYTYPE;

typedef enum tagD_D3DTEXTUREFILTERTYPE
{
	D_D3DTEXF_NONE								= 0,
	D_D3DTEXF_POINT								= 1,
	D_D3DTEXF_LINEAR							= 2,
	D_D3DTEXF_ANISOTROPIC						= 3,
	D_D3DTEXF_PYRAMIDALQUAD						= 6,
	D_D3DTEXF_GAUSSIANQUAD						= 7,
	D_D3DTEXF_FORCE_DWORD						= 0x7fffffff,
} D_D3DTEXTUREFILTERTYPE;

typedef enum tagD_D3DBASISTYPE
{
	D_D3DBASIS_BEZIER							= 0,
	D_D3DBASIS_BSPLINE							= 1,
	D_D3DBASIS_CATMULL_ROM						= 2,
	D_D3DBASIS_FORCE_DWORD						= 0x7fffffff,
} D_D3DBASISTYPE;

typedef enum tagD_D3DDEGREETYPE
{
	D_D3DDEGREE_LINEAR							= 1,
	D_D3DDEGREE_QUADRATIC						= 2,
	D_D3DDEGREE_CUBIC							= 3,
	D_D3DDEGREE_QUINTIC							= 5,
	D_D3DDEGREE_FORCE_DWORD						= 0x7fffffff,
} D_D3DDEGREETYPE;

typedef enum tagD_D3DBLENDOP
{
	D_D3DBLENDOP_ADD							= 1,
	D_D3DBLENDOP_SUBTRACT						= 2,
	D_D3DBLENDOP_REVSUBTRACT					= 3,
	D_D3DBLENDOP_MIN							= 4,
	D_D3DBLENDOP_MAX							= 5,
	D_D3DBLENDOP_FORCE_DWORD					= 0x7fffffff,
} D_D3DBLENDOP;

typedef enum tagD_D3DCUBEMAP_FACES
{
	D_D3DCUBEMAP_FACE_POSITIVE_X				= 0,
	D_D3DCUBEMAP_FACE_NEGATIVE_X				= 1,
	D_D3DCUBEMAP_FACE_POSITIVE_Y				= 2,
	D_D3DCUBEMAP_FACE_NEGATIVE_Y				= 3,
	D_D3DCUBEMAP_FACE_POSITIVE_Z				= 4,
	D_D3DCUBEMAP_FACE_NEGATIVE_Z				= 5,

	D_D3DCUBEMAP_FACE_FORCE_DWORD				= 0x7fffffff
} D_D3DCUBEMAP_FACES;

typedef enum tagD_D3DDEVTYPE
{
	D_D3DDEVTYPE_HAL							= 1,
	D_D3DDEVTYPE_REF							= 2,
	D_D3DDEVTYPE_SW								= 3,

	D_D3DDEVTYPE_FORCE_DWORD					= 0x7fffffff
} D_D3DDEVTYPE;

typedef enum tagD_D3DPOOL
{
	D_D3DPOOL_DEFAULT							= 0,
	D_D3DPOOL_MANAGED							= 1,
	D_D3DPOOL_SYSTEMMEM							= 2,
	D_D3DPOOL_SCRATCH							= 3,

	D_D3DPOOL_FORCE_DWORD						= 0x7fffffff
} D_D3DPOOL;

typedef enum tagD_D39DBACKBUFFER_TYPE
{
	D_D3DBACKBUFFER_TYPE_MONO					= 0,
	D_D3DBACKBUFFER_TYPE_LEFT					= 1,
	D_D3DBACKBUFFER_TYPE_RIGHT					= 2,

	D_D3DBACKBUFFER_TYPE_FORCE_DWORD			= 0x7fffffff
} D_D3DBACKBUFFER_TYPE;

typedef enum tagD_D3DSWAPEFFECT
{
	D_D3DSWAPEFFECT_DISCARD						= 1,
	D_D3DSWAPEFFECT_FLIP						= 2,
	D_D3DSWAPEFFECT_COPY						= 3,

	D_D3DSWAPEFFECT_FORCE_DWORD					= 0x7fffffff
} D_D3DSWAPEFFECT;

typedef enum tagD_D3DMULTISAMPLE_TYPE
{
	D_D3DMULTISAMPLE_NONE						=  0,
	D_D3DMULTISAMPLE_NONMASKABLE				=  1,
	D_D3DMULTISAMPLE_2_SAMPLES					=  2,
	D_D3DMULTISAMPLE_3_SAMPLES					=  3,
	D_D3DMULTISAMPLE_4_SAMPLES					=  4,
	D_D3DMULTISAMPLE_5_SAMPLES					=  5,
	D_D3DMULTISAMPLE_6_SAMPLES					=  6,
	D_D3DMULTISAMPLE_7_SAMPLES					=  7,
	D_D3DMULTISAMPLE_8_SAMPLES					=  8,
	D_D3DMULTISAMPLE_9_SAMPLES					=  9,
	D_D3DMULTISAMPLE_10_SAMPLES					= 10,
	D_D3DMULTISAMPLE_11_SAMPLES					= 11,
	D_D3DMULTISAMPLE_12_SAMPLES					= 12,
	D_D3DMULTISAMPLE_13_SAMPLES					= 13,
	D_D3DMULTISAMPLE_14_SAMPLES					= 14,
	D_D3DMULTISAMPLE_15_SAMPLES					= 15,
	D_D3DMULTISAMPLE_16_SAMPLES					= 16,

	D_D3DMULTISAMPLE_FORCE_DWORD				= 0x7fffffff
} D_D3DMULTISAMPLE_TYPE;

typedef enum D_D3DRESOURCETYPE
{
	D_D3DRTYPE_SURFACE							= 1,
	D_D3DRTYPE_VOLUME							= 2,
	D_D3DRTYPE_TEXTURE							= 3,
	D_D3DRTYPE_VOLUMETEXTURE					= 4,
	D_D3DRTYPE_CUBETEXTURE						= 5,
	D_D3DRTYPE_VERTEXBUFFER						= 6,
	D_D3DRTYPE_INDEXBUFFER						= 7,

	D_D3DRTYPE_FORCE_DWORD						= 0x7fffffff
} D_D3DRESOURCETYPE;

typedef enum D_D3DDECLUSAGE
{
	D_D3DDECLUSAGE_POSITION						= 0,
	D_D3DDECLUSAGE_BLENDWEIGHT					= 1,
	D_D3DDECLUSAGE_BLENDINDICES					= 2,
	D_D3DDECLUSAGE_NORMAL						= 3,
	D_D3DDECLUSAGE_PSIZE						= 4,
	D_D3DDECLUSAGE_TEXCOORD						= 5,
	D_D3DDECLUSAGE_TANGENT						= 6,
	D_D3DDECLUSAGE_BINORMAL						= 7,
	D_D3DDECLUSAGE_TESSFACTOR					= 8,
	D_D3DDECLUSAGE_POSITIONT					= 9,
	D_D3DDECLUSAGE_COLOR						= 10,
	D_D3DDECLUSAGE_FOG							= 11,
	D_D3DDECLUSAGE_DEPTH						= 12,
	D_D3DDECLUSAGE_SAMPLE						= 13,
} D_D3DDECLUSAGE;

#define D_MAXD3DDECLUSAGE						D_D3DDECLUSAGE_SAMPLE
#define D_MAXD3DDECLUSAGEINDEX					15
#define D_MAXD3DDECLLENGTH						64

typedef enum D_D3DDECLMETHOD
{
	D_D3DDECLMETHOD_DEFAULT						= 0,
	D_D3DDECLMETHOD_PARTIALU					= 1,
	D_D3DDECLMETHOD_PARTIALV					= 2,
	D_D3DDECLMETHOD_CROSSUV						= 3,
	D_D3DDECLMETHOD_UV							= 4,
	D_D3DDECLMETHOD_LOOKUP						= 5,
	D_D3DDECLMETHOD_LOOKUPPRESAMPLED			= 6,
} D_D3DDECLMETHOD;

#define D_MAXD3DDECLMETHOD						D_D3DDECLMETHOD_LOOKUPPRESAMPLED

typedef enum tagD_D3DDECLTYPE
{
	D_D3DDECLTYPE_FLOAT1						=  0,
	D_D3DDECLTYPE_FLOAT2						=  1,
	D_D3DDECLTYPE_FLOAT3						=  2,
	D_D3DDECLTYPE_FLOAT4						=  3,
	D_D3DDECLTYPE_D3DCOLOR						=  4,
	D_D3DDECLTYPE_UBYTE4						=  5,
	D_D3DDECLTYPE_SHORT2						=  6,
	D_D3DDECLTYPE_SHORT4						=  7,
	D_D3DDECLTYPE_UBYTE4N						=  8,
	D_D3DDECLTYPE_SHORT2N						=  9,
	D_D3DDECLTYPE_SHORT4N						= 10,
	D_D3DDECLTYPE_USHORT2N						= 11,
	D_D3DDECLTYPE_USHORT4N						= 12,
	D_D3DDECLTYPE_UDEC3							= 13,
	D_D3DDECLTYPE_DEC3N							= 14,
	D_D3DDECLTYPE_FLOAT16_2						= 15,
	D_D3DDECLTYPE_FLOAT16_4						= 16,
	D_D3DDECLTYPE_UNUSED						= 17,
} D_D3DDECLTYPE;

typedef enum tagD_D3DXREGISTER_SET
{
	D_D3DXRS_BOOL								= 0,
	D_D3DXRS_INT4								= 1,
	D_D3DXRS_FLOAT4								= 2,
	D_D3DXRS_SAMPLER							= 3,

	D_D3DXRS_FORCE_DWORD						= 0x7fffffff
} D_D3DXREGISTER_SET ;

typedef enum tagD_D3DXPARAMETER_CLASS
{
	D_D3DXPC_SCALAR								= 0,
	D_D3DXPC_VECTOR								= 1,
	D_D3DXPC_MATRIX_ROWS						= 2,
	D_D3DXPC_MATRIX_COLUMNS						= 3,
	D_D3DXPC_OBJECT								= 4,
	D_D3DXPC_STRUCT								= 5,

	D_D3DXPC_FORCE_DWORD						= 0x7fffffff
} D_D3DXPARAMETER_CLASS ;

typedef enum tagD_D3DXPARAMETER_TYPE
{
	D_D3DXPT_VOID								= 0,
	D_D3DXPT_BOOL								= 1,
	D_D3DXPT_INT								= 2,
	D_D3DXPT_FLOAT								= 3,
	D_D3DXPT_STRING								= 4,
	D_D3DXPT_TEXTURE							= 5,
	D_D3DXPT_TEXTURE1D							= 6,
	D_D3DXPT_TEXTURE2D							= 7,
	D_D3DXPT_TEXTURE3D							= 8,
	D_D3DXPT_TEXTURECUBE						= 9,
	D_D3DXPT_SAMPLER							= 10,
	D_D3DXPT_SAMPLER1D							= 11,
	D_D3DXPT_SAMPLER2D							= 12,
	D_D3DXPT_SAMPLER3D							= 13,
	D_D3DXPT_SAMPLERCUBE						= 14,
	D_D3DXPT_PIXELSHADER						= 15,
	D_D3DXPT_VERTEXSHADER						= 16,
	D_D3DXPT_PIXELFRAGMENT						= 17,
	D_D3DXPT_VERTEXFRAGMENT						= 18,
	D_D3DXPT_UNSUPPORTED						= 19,

	D_D3DXPT_FORCE_DWORD						= 0x7fffffff
} D_D3DXPARAMETER_TYPE ;

typedef enum D_D3DSCANLINEORDERING
{
	D_D3DSCANLINEORDERING_UNKNOWN				= 0, 
	D_D3DSCANLINEORDERING_PROGRESSIVE			= 1,
	D_D3DSCANLINEORDERING_INTERLACED			= 2,
} D_D3DSCANLINEORDERING ;

typedef enum D_D3DCOMPOSERECTSOP
{
	D_D3DCOMPOSERECTS_COPY						= 1,
	D_D3DCOMPOSERECTS_OR						= 2,
	D_D3DCOMPOSERECTS_AND						= 3,
	D_D3DCOMPOSERECTS_NEG						= 4,

	D_D3DCOMPOSERECTS_FORCE_DWORD				= 0x7fffffff,
} D_D3DCOMPOSERECTSOP ;

typedef enum D_D3DDISPLAYROTATION
{
	D_D3DDISPLAYROTATION_IDENTITY				= 1,
	D_D3DDISPLAYROTATION_90						= 2,
	D_D3DDISPLAYROTATION_180					= 3,
	D_D3DDISPLAYROTATION_270					= 4 
} D_D3DDISPLAYROTATION ;

typedef enum D_D3DFILLMODE
{
	D_D3DFILL_POINT								= 1,
	D_D3DFILL_WIREFRAME							= 2,
	D_D3DFILL_SOLID								= 3,

	D_D3DFILL_FORCE_DWORD						= 0x7fffffff,
} D_D3DFILLMODE ;

#define D_D3DCREATE_FPU_PRESERVE				(0x00000002L)
#define D_D3DCREATE_MULTITHREADED				(0x00000004L)

#define D_D3DCREATE_PUREDEVICE					(0x00000010L)
#define D_D3DCREATE_SOFTWARE_VERTEXPROCESSING	(0x00000020L)
#define D_D3DCREATE_HARDWARE_VERTEXPROCESSING	(0x00000040L)
#define D_D3DCREATE_MIXED_VERTEXPROCESSING		(0x00000080L)

#define D_D3DCREATE_DISABLE_DRIVER_MANAGEMENT	(0x00000100L)

#define D_D3DADAPTER_DEFAULT					(0)

#define D_D3DPRESENT_INTERVAL_DEFAULT			(0x00000000L)
#define D_D3DPRESENT_INTERVAL_ONE				(0x00000001L)
#define D_D3DPRESENT_INTERVAL_TWO				(0x00000002L)
#define D_D3DPRESENT_INTERVAL_THREE				(0x00000004L)
#define D_D3DPRESENT_INTERVAL_FOUR				(0x00000008L)
#define D_D3DPRESENT_INTERVAL_IMMEDIATE			(0x80000000L)

#define D_D3DPRESENT_DONOTWAIT					(0x00000001L)
#define D_D3DPRESENT_LINEAR_CONTENT				(0x00000002L)

#define D_D3DUSAGE_RENDERTARGET					(0x00000001L)
#define D_D3DUSAGE_DEPTHSTENCIL					(0x00000002L)
#define D_D3DUSAGE_WRITEONLY					(0x00000008L)
#define D_D3DUSAGE_SOFTWAREPROCESSING			(0x00000010L)
#define D_D3DUSAGE_DONOTCLIP					(0x00000020L)
#define D_D3DUSAGE_POINTS						(0x00000040L)
#define D_D3DUSAGE_RTPATCHES					(0x00000080L)
#define D_D3DUSAGE_NPATCHES						(0x00000100L)
#define D_D3DUSAGE_DYNAMIC						(0x00000200L)
#define D_D3DUSAGE_AUTOGENMIPMAP				(0x00000400L)
#define D_D3DUSAGE_DMAP							(0x00004000L)
#define D_D3DUSAGE_QUERY_LEGACYBUMPMAP			(0x00008000L)
#define D_D3DUSAGE_QUERY_SRGBREAD				(0x00010000L)
#define D_D3DUSAGE_QUERY_FILTER					(0x00020000L)
#define D_D3DUSAGE_QUERY_SRGBWRITE				(0x00040000L)
#define D_D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING	(0x00080000L)
#define D_D3DUSAGE_QUERY_VERTEXTEXTURE			(0x00100000L)

#define D_D3DLOCK_READONLY						(0x00000010L)
#define D_D3DLOCK_DISCARD						(0x00002000L)
#define D_D3DLOCK_NOOVERWRITE					(0x00001000L)
#define D_D3DLOCK_NOSYSLOCK						(0x00000800L)
#define D_D3DLOCK_DONOTWAIT						(0x00004000L)

#define D_D3DCLEAR_TARGET						(0x00000001l)
#define D_D3DCLEAR_ZBUFFER						(0x00000002l)
#define D_D3DCLEAR_STENCIL						(0x00000004l)

#define D_D3DDMAPSAMPLER						256

#define D_D3DVERTEXTEXTURESAMPLER0				(D3DDMAPSAMPLER + 1)
#define D_D3DVERTEXTEXTURESAMPLER1				(D3DDMAPSAMPLER + 2)
#define D_D3DVERTEXTEXTURESAMPLER2				(D3DDMAPSAMPLER + 3)
#define D_D3DVERTEXTEXTURESAMPLER3				(D3DDMAPSAMPLER + 4)

#define D_D3DDECL_END()							{ 0xFF, 0, D_D3DDECLTYPE_UNUSED, 0, 0, 0 }
#define D_D3DTS_WORLDMATRIX( index )			( D_D3DTRANSFORMSTATETYPE )( index + 256 )

typedef float									D_D3DVALUE ;
typedef DWORD									D_D3DCOLOR ;

typedef struct tagD_D3DVIEWPORT7
{
	DWORD										dwX;
	DWORD										dwY;
	DWORD										dwWidth;
	DWORD										dwHeight;
	D_D3DVALUE									dvMinZ;
	D_D3DVALUE									dvMaxZ;
} D_D3DVIEWPORT7 ;

typedef struct tagD_D3DPRIMCAPS
{
	DWORD										dwSize;
	DWORD										dwMiscCaps;
	DWORD										dwRasterCaps;
	DWORD										dwZCmpCaps;
	DWORD										dwSrcBlendCaps;
	DWORD										dwDestBlendCaps;
	DWORD										dwAlphaCmpCaps;
	DWORD										dwShadeCaps;
	DWORD										dwTextureCaps;
	DWORD										dwTextureFilterCaps;
	DWORD										dwTextureBlendCaps;
	DWORD										dwTextureAddressCaps;
	DWORD										dwStippleWidth;
	DWORD										dwStippleHeight;
} D_D3DPRIMCAPS ;

typedef struct tagD_D3DDEVICEDESC7
{
	DWORD										dwDevCaps;
	D_D3DPRIMCAPS								dpcLineCaps;
	D_D3DPRIMCAPS								dpcTriCaps;
	DWORD										dwDeviceRenderBitDepth;
	DWORD										dwDeviceZBufferBitDepth;

	DWORD										dwMinTextureWidth, dwMinTextureHeight;
	DWORD										dwMaxTextureWidth, dwMaxTextureHeight;

	DWORD										dwMaxTextureRepeat;
	DWORD										dwMaxTextureAspectRatio;
	DWORD										dwMaxAnisotropy;

	D_D3DVALUE									dvGuardBandLeft;
	D_D3DVALUE									dvGuardBandTop;
	D_D3DVALUE									dvGuardBandRight;
	D_D3DVALUE									dvGuardBandBottom;

	D_D3DVALUE									dvExtentsAdjust;
	DWORD										dwStencilCaps;

	DWORD										dwFVFCaps;
	DWORD										dwTextureOpCaps;
	WORD										wMaxTextureBlendStages;
	WORD										wMaxSimultaneousTextures;

	DWORD										dwMaxActiveLights;
	D_D3DVALUE									dvMaxVertexW;
	GUID										deviceGUID;

	WORD										wMaxUserClipPlanes;
	WORD										wMaxVertexBlendMatrices;

	DWORD										dwVertexProcessingCaps;

	DWORD										dwReserved1;
	DWORD										dwReserved2;
	DWORD										dwReserved3;
	DWORD										dwReserved4;
} D_D3DDEVICEDESC7 ;

typedef struct tagD_D3DRECT
{
	union
	{
		LONG 									x1;
		LONG 									lX1;
	} ;
	union
	{
		LONG 									y1;
		LONG 									lY1;
	} ;
	union
	{
		LONG 									x2;
		LONG 									lX2;
	} ;
	union
	{
		LONG 									y2;
		LONG 									lY2;
	} ;
} D_D3DRECT ;

typedef struct tagD_D3DMATRIX
{
	D_D3DVALUE  								m[4][4];
} D_D3DMATRIX ;

typedef struct tagD_D3DCOLORVALUE
{
	union
	{
		D_D3DVALUE  							r;
		D_D3DVALUE  							dvR;
	} ;
	union
	{
		D_D3DVALUE  							g;
		D_D3DVALUE  							dvG;
	} ;
	union
	{
		D_D3DVALUE  							b;
		D_D3DVALUE  							dvB;
	} ;
	union
	{
		D_D3DVALUE  							a;
		D_D3DVALUE  							dvA;
	} ;
} D_D3DCOLORVALUE;

typedef struct tagD_D3DVECTOR
{
	union
	{
		D_D3DVALUE  							x;
		D_D3DVALUE  							dvX;
	} ;
	union
	{
		D_D3DVALUE  							y;
		D_D3DVALUE  							dvY;
	} ;
	union
	{
		D_D3DVALUE  							z;
		D_D3DVALUE  							dvZ;
	} ;
} D_D3DVECTOR ;

typedef struct tagD_D3DMATERIAL7
{
	union
	{
		D_D3DCOLORVALUE							diffuse;
		D_D3DCOLORVALUE							dcvDiffuse;
	} ;
	union
	{
		D_D3DCOLORVALUE							ambient;
		D_D3DCOLORVALUE							dcvAmbient;
	} ;
	union
	{
		D_D3DCOLORVALUE							specular;
		D_D3DCOLORVALUE							dcvSpecular;
	} ;
	union
	{
		D_D3DCOLORVALUE							emissive;
		D_D3DCOLORVALUE							dcvEmissive;
	} ;
	union
	{
		D_D3DVALUE								power;
		D_D3DVALUE								dvPower;
	} ;
} D_D3DMATERIAL7 ;

typedef struct tagD_D3DLIGHT7
{
	D_D3DLIGHTTYPE								dltType;
	D_D3DCOLORVALUE								dcvDiffuse;
	D_D3DCOLORVALUE								dcvSpecular;
	D_D3DCOLORVALUE								dcvAmbient;
	D_D3DVECTOR									dvPosition;
	D_D3DVECTOR									dvDirection;
	D_D3DVALUE									dvRange;
	D_D3DVALUE									dvFalloff;
	D_D3DVALUE									dvAttenuation0;
	D_D3DVALUE									dvAttenuation1;
	D_D3DVALUE									dvAttenuation2;
	D_D3DVALUE									dvTheta;
	D_D3DVALUE									dvPhi;
} D_D3DLIGHT7 ;

typedef struct tagD_D3DVERTEXBUFFERDESC
{
	DWORD										dwSize;
	DWORD										dwCaps;
	DWORD										dwFVF;
	DWORD										dwNumVertices;
} D_D3DVERTEXBUFFERDESC ;

typedef struct D_D3DVSHADERCAPS2_0
{
	DWORD										Caps;
	int											DynamicFlowControlDepth;
	int											NumTemps;
	int											StaticFlowControlDepth;
} D_D3DVSHADERCAPS2_0;

typedef struct tagD_D3DPSHADERCAPS2_0
{
	DWORD										Caps;
	int											DynamicFlowControlDepth;
	int											NumTemps;
	int											StaticFlowControlDepth;
	int											NumInstructionSlots;
} D_D3DPSHADERCAPS2_0;

typedef struct tagD_D3DDISPLAYMODE
{
	UINT										Width;
	UINT										Height;
	UINT										RefreshRate;
	D_D3DFORMAT									Format;
} D_D3DDISPLAYMODE;

typedef struct tagD_D3DPRESENT_PARAMETERS
{
	UINT										BackBufferWidth;
	UINT										BackBufferHeight;
	D_D3DFORMAT									BackBufferFormat;
	UINT										BackBufferCount;

	D_D3DMULTISAMPLE_TYPE						MultiSampleType;
	DWORD										MultiSampleQuality;

	D_D3DSWAPEFFECT								SwapEffect;
	HWND										hDeviceWindow;
	BOOL										Windowed;
	BOOL										EnableAutoDepthStencil;
	D_D3DFORMAT									AutoDepthStencilFormat;
	DWORD										Flags;

	UINT										FullScreen_RefreshRateInHz;
	UINT										PresentationInterval;
} D_D3DPRESENT_PARAMETERS;

typedef struct tagD_D3DDEVICE_CREATION_PARAMETERS
{
	UINT										AdapterOrdinal;
	D_D3DDEVTYPE								DeviceType;
	HWND										hFocusWindow;
	DWORD										BehaviorFlags;
} D_D3DDEVICE_CREATION_PARAMETERS;

typedef struct tagD_D3DRASTER_STATUS
{
	BOOL										InVBlank;
	UINT										ScanLine;
} D_D3DRASTER_STATUS;

typedef struct tagD_D3DGAMMARAMP
{
	WORD										red  [256];
	WORD										green[256];
	WORD										blue [256];
} D_D3DGAMMARAMP;

typedef struct tagD_D3DVIEWPORT9
{
	DWORD										X;
	DWORD										Y;
	DWORD										Width;
	DWORD										Height;
	float										MinZ;
	float										MaxZ;
} D_D3DVIEWPORT9;

typedef struct tagD_D3DCLIPSTATUS9
{
	DWORD										ClipUnion;
	DWORD										ClipIntersection;
} D_D3DCLIPSTATUS9;

typedef struct tagD_D3DMATERIAL9
{
	D_D3DCOLORVALUE								Diffuse;
	D_D3DCOLORVALUE								Ambient;
	D_D3DCOLORVALUE								Specular;
	D_D3DCOLORVALUE								Emissive;
	float										Power;
} D_D3DMATERIAL9;

typedef struct tagD_D3DLIGHT9
{
	D_D3DLIGHTTYPE								Type;
	D_D3DCOLORVALUE								Diffuse;
	D_D3DCOLORVALUE								Specular;
	D_D3DCOLORVALUE								Ambient;
	D_D3DVECTOR									Position;
	D_D3DVECTOR									Direction;
	float										Range;
	float										Falloff;
	float										Attenuation0;
	float										Attenuation1;
	float										Attenuation2;
	float										Theta;
	float										Phi;
} D_D3DLIGHT9;

typedef struct tagD_D3DVERTEXELEMENT9
{
	WORD										Stream;
	WORD										Offset;
	BYTE										Type;
	BYTE										Method;
	BYTE										Usage;
	BYTE										UsageIndex;
} D_D3DVERTEXELEMENT9 ;

typedef struct tagD_D3DRECTPATCH_INFO
{
	UINT										StartVertexOffsetWidth;
	UINT										StartVertexOffsetHeight;
	UINT										Width;
	UINT										Height;
	UINT										Stride;
	D_D3DBASISTYPE								Basis;
	D_D3DDEGREETYPE								Degree;
} D_D3DRECTPATCH_INFO;

typedef struct tagD_D3DTRIPATCH_INFO
{
	UINT										StartVertexOffset;
	UINT										NumVertices;
	D_D3DBASISTYPE								Basis;
	D_D3DDEGREETYPE								Degree;
} D_D3DTRIPATCH_INFO;

typedef struct tagD_D3DSURFACE_DESC
{
	D_D3DFORMAT									Format;
	D_D3DRESOURCETYPE							Type;
	DWORD										Usage;
	D_D3DPOOL									Pool;
	UINT										Size;

	D_D3DMULTISAMPLE_TYPE						MultiSampleType;
	UINT										Width;
	UINT										Height;
} D_D3DSURFACE_DESC;

typedef struct tagD_D3DLOCKED_BOX
{
	INT											RowPitch;
	INT											SlicePitch;
	void*										pBits;
} D_D3DLOCKED_BOX;

typedef struct tagD_D3DVOLUME_DESC
{
	D_D3DFORMAT									Format;
	D_D3DRESOURCETYPE							Type;
	DWORD										Usage;
	D_D3DPOOL									Pool;
	UINT										Size;

	UINT										Width;
	UINT										Height;
	UINT										Depth;
} D_D3DVOLUME_DESC;

typedef struct tagD_D3DBOX
{
	UINT										Left;
	UINT										Top;
	UINT										Right;
	UINT										Bottom;
	UINT										Front;
	UINT										Back;
} D_D3DBOX;

typedef struct tagD_D3DLOCKED_RECT
{
	INT											Pitch;
	void*										pBits;
} D_D3DLOCKED_RECT;

typedef struct tagD_D3DVERTEXBUFFER_DESC
{
	D_D3DFORMAT									Format;
	D_D3DRESOURCETYPE							Type;
	DWORD										Usage;
	D_D3DPOOL									Pool;
	UINT										Size;

	DWORD										FVF;
} D_D3DVERTEXBUFFER_DESC;

typedef struct tagD_D3DINDEXBUFFER_DESC
{
	D_D3DFORMAT									Format;
	D_D3DRESOURCETYPE							Type;
	DWORD										Usage;
	D_D3DPOOL									Pool;
	UINT										Size;
} D_D3DINDEXBUFFER_DESC;

typedef struct tagD_D3DCAPS9
{
	D_D3DDEVTYPE								DeviceType;
	UINT										AdapterOrdinal;

	DWORD										Caps;
	DWORD										Caps2;
	DWORD										Caps3;
	DWORD										PresentationIntervals;

	DWORD										CursorCaps;

	DWORD										DevCaps;

	DWORD										PrimitiveMiscCaps;
	DWORD										RasterCaps;
	DWORD										ZCmpCaps;
	DWORD										SrcBlendCaps;
	DWORD										DestBlendCaps;
	DWORD										AlphaCmpCaps;
	DWORD										ShadeCaps;
	DWORD										TextureCaps;
	DWORD										TextureFilterCaps;
	DWORD										CubeTextureFilterCaps;
	DWORD										VolumeTextureFilterCaps;
	DWORD										TextureAddressCaps;
	DWORD										VolumeTextureAddressCaps;

	DWORD										LineCaps;

	DWORD										MaxTextureWidth, MaxTextureHeight;
	DWORD										MaxVolumeExtent;

	DWORD										MaxTextureRepeat;
	DWORD										MaxTextureAspectRatio;
	DWORD										MaxAnisotropy;
	float										MaxVertexW;

	float										GuardBandLeft;
	float										GuardBandTop;
	float										GuardBandRight;
	float										GuardBandBottom;

	float										ExtentsAdjust;
	DWORD										StencilCaps;

	DWORD										FVFCaps;
	DWORD										TextureOpCaps;
	DWORD										MaxTextureBlendStages;
	DWORD										MaxSimultaneousTextures;

	DWORD										VertexProcessingCaps;
	DWORD										MaxActiveLights;
	DWORD										MaxUserClipPlanes;
	DWORD										MaxVertexBlendMatrices;
	DWORD										MaxVertexBlendMatrixIndex;

	float										MaxPointSize;

	DWORD										MaxPrimitiveCount;
	DWORD										MaxVertexIndex;
	DWORD										MaxStreams;
	DWORD										MaxStreamStride;

	DWORD										VertexShaderVersion;
	DWORD										MaxVertexShaderConst;

	DWORD										PixelShaderVersion;
	float										PixelShader1xMaxValue;

	DWORD										DevCaps2;

	float										MaxNpatchTessellationLevel;
	DWORD										Reserved5;

	UINT										MasterAdapterOrdinal;
	UINT										AdapterOrdinalInGroup;
	UINT										NumberOfAdaptersInGroup;
	DWORD										DeclTypes;
	DWORD										NumSimultaneousRTs;
	DWORD										StretchRectFilterCaps;
	D_D3DVSHADERCAPS2_0							VS20Caps;
	D_D3DPSHADERCAPS2_0							PS20Caps;
	DWORD										VertexTextureFilterCaps;
	DWORD										MaxVShaderInstructionsExecuted;
	DWORD										MaxPShaderInstructionsExecuted;
	DWORD										MaxVertexShader30InstructionSlots; 
	DWORD										MaxPixelShader30InstructionSlots;
} D_D3DCAPS9;

#define MAX_DEVICE_IDENTIFIER_STRING        512
typedef struct tagD_D3DADAPTER_IDENTIFIER9
{
	char										Driver[MAX_DEVICE_IDENTIFIER_STRING];
	char										Description[MAX_DEVICE_IDENTIFIER_STRING];
	char										DeviceName[32];

#ifdef _WIN32
	LARGE_INTEGER								DriverVersion;
#else
	DWORD										DriverVersionLowPart;
	DWORD										DriverVersionHighPart;
#endif

	DWORD										VendorId;
	DWORD										DeviceId;
	DWORD										SubSysId;
	DWORD										Revision;

	GUID										DeviceIdentifier;

	DWORD										WHQLLevel;

} D_D3DADAPTER_IDENTIFIER9;

typedef struct tagD_D3DXMACRO
{
	const CHAR									*Name ;
	const CHAR									*Definition ;
} D_D3DXMACRO ;

typedef struct tagD_D3DXCONSTANTTABLE_DESC
{
	const CHAR									*Creator ;
	DWORD										Version ;
	UINT										Constants ;
} D_D3DXCONSTANTTABLE_DESC ;

typedef struct tagD_D3DXCONSTANT_DESC
{
	const CHAR									*Name ;

	D_D3DXREGISTER_SET							RegisterSet ;
	UINT										RegisterIndex ;
	UINT										RegisterCount ;

	D_D3DXPARAMETER_CLASS						Class ;
	D_D3DXPARAMETER_TYPE						Type ;

	UINT										Rows ;
	UINT										Columns ;
	UINT										Elements ;
	UINT										StructMembers ;

	UINT										Bytes ;
	const void									*DefaultValue ;
} D_D3DXCONSTANT_DESC ;

typedef struct tagD_D3DXSHADER_CONSTANTINFO
{
	DWORD										Name ;
	WORD										RegisterSet ;
	WORD										RegisterIndex ;
	WORD										RegisterCount ;
	WORD										Reserved ;
	DWORD										TypeInfo ;
	DWORD										DefaultValue ;
} D_D3DXSHADER_CONSTANTINFO ;

typedef struct tagD_D3DDISPLAYMODEEX
{
	UINT										Size ;
	UINT										Width ;
	UINT										Height ;
	UINT										RefreshRate ;
	D_D3DFORMAT									Format ;
	D_D3DSCANLINEORDERING						ScanLineOrdering ;
} D_D3DDISPLAYMODEEX ;

typedef struct tagD_D3DDISPLAYMODEFILTER
{
	UINT										Size ;
	D_D3DFORMAT									Format ;
	D_D3DSCANLINEORDERING						ScanLineOrdering ;
} D_D3DDISPLAYMODEFILTER ;

typedef HRESULT ( CALLBACK * LPD_D3DENUMDEVICESCALLBACK7 )		( LPSTR lpDeviceDescription, LPSTR lpDeviceName, class D_IDirect3DDevice7 *, LPVOID);
typedef HRESULT ( CALLBACK * LPD_D3DENUMPIXELFORMATSCALLBACK )	( D_DDPIXELFORMAT *lpDDPixFmt, LPVOID lpContext);
typedef HRESULT ( CALLBACK * LPD_D3DENUMPIXELFORMATSCALLBACK )	( D_DDPIXELFORMAT *lpDDPixFmt, LPVOID lpContext);

class D_IDirect3DResource9 : public D_IUnknown
{
public :
	virtual HRESULT __stdcall GetDevice					( class D_IDirect3DDevice9** ppDevice) = 0 ;
	virtual HRESULT __stdcall SetPrivateData			( REFGUID refguid, CONST void* pData, DWORD SizeOfData, DWORD Flags) = 0 ;
	virtual HRESULT __stdcall GetPrivateData			( REFGUID refguid, void* pData, DWORD* pSizeOfData) = 0 ;
	virtual HRESULT __stdcall FreePrivateData			( REFGUID refguid) = 0 ;
	virtual DWORD   __stdcall SetPriority				( DWORD PriorityNew) = 0 ;
	virtual DWORD   __stdcall GetPriority				( void ) = 0 ;
	virtual void    __stdcall PreLoad					( void ) = 0 ;
	virtual D_D3DRESOURCETYPE __stdcall GetType			( void ) = 0 ;
};

class D_IDirect3DSurface9 : public D_IDirect3DResource9
{
public :
	virtual HRESULT __stdcall GetContainer				( REFIID riid, void** ppContainer) = 0 ;
	virtual HRESULT __stdcall GetDesc					( D_D3DSURFACE_DESC *pDesc) = 0 ;
	virtual HRESULT __stdcall LockRect					( D_D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags) = 0 ;
	virtual HRESULT __stdcall UnlockRect				( void ) = 0 ;
	virtual HRESULT __stdcall GetDC						( HDC *phdc) = 0 ;
	virtual HRESULT __stdcall ReleaseDC					( HDC hdc) = 0 ;
};

class D_IDirect3DBaseTexture9 : public D_IDirect3DResource9
{
public :
	virtual DWORD   __stdcall SetLOD					( DWORD LODNew) = 0 ;
	virtual DWORD   __stdcall GetLOD					( void ) = 0 ;
	virtual DWORD   __stdcall GetLevelCount				( void ) = 0 ;
	virtual HRESULT __stdcall SetAutoGenFilterType		( D_D3DTEXTUREFILTERTYPE FilterType) = 0 ;
	virtual D_D3DTEXTUREFILTERTYPE __stdcall GetAutoGenFilterType( void ) = 0 ;
	virtual void    __stdcall GenerateMipSubLevels		( void ) = 0 ;
};

class D_IDirect3DTexture9 : public D_IDirect3DBaseTexture9
{
public :
	virtual HRESULT __stdcall GetLevelDesc				( UINT Level, D_D3DSURFACE_DESC *pDesc) = 0 ;
	virtual HRESULT __stdcall GetSurfaceLevel			( UINT Level,  class D_IDirect3DSurface9** ppSurfaceLevel) = 0 ;
	virtual HRESULT __stdcall LockRect					( UINT Level, D_D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags) = 0 ;
	virtual HRESULT __stdcall UnlockRect				( UINT Level) = 0 ;
	virtual HRESULT __stdcall AddDirtyRect				( CONST RECT* pDirtyRect) = 0 ;
};

class D_IDirect3DVolume9 : public D_IUnknown
{
	virtual HRESULT __stdcall GetDevice					( class D_IDirect3DDevice9** ppDevice)  = 0 ;
	virtual HRESULT __stdcall SetPrivateData			( REFGUID refguid, CONST void* pData, DWORD SizeOfData, DWORD Flags)  = 0 ;
	virtual HRESULT __stdcall GetPrivateData			( REFGUID refguid, void* pData, DWORD* pSizeOfData)  = 0 ;
	virtual HRESULT __stdcall FreePrivateData			( REFGUID refguid)  = 0 ;
	virtual HRESULT __stdcall GetContainer				( REFIID riid, void** ppContainer)  = 0 ;
	virtual HRESULT __stdcall GetDesc					( D_D3DVOLUME_DESC *pDesc)  = 0 ;
	virtual HRESULT __stdcall LockBox					( D_D3DLOCKED_BOX * pLockedVolume, CONST D_D3DBOX* pBox, DWORD Flags)  = 0 ;
	virtual HRESULT __stdcall UnlockBox					( void )  = 0 ;
};

class D_IDirect3DVolumeTexture9 : public D_IDirect3DBaseTexture9
{
public :
	virtual HRESULT __stdcall GetLevelDesc				( UINT Level, D_D3DVOLUME_DESC *pDesc) = 0 ;
	virtual HRESULT __stdcall GetVolumeLevel			( UINT Level, class D_IDirect3DVolume9** ppVolumeLevel) = 0 ;
	virtual HRESULT __stdcall LockBox					( UINT Level, D_D3DLOCKED_BOX* pLockedVolume, CONST D_D3DBOX* pBox, DWORD Flags) = 0 ;
	virtual HRESULT __stdcall UnlockBox					( UINT Level) = 0 ;
	virtual HRESULT __stdcall AddDirtyBox				( CONST D_D3DBOX* pDirtyBox) = 0 ;
};

class D_IDirect3DCubeTexture9 : public D_IDirect3DBaseTexture9
{
public :
	virtual HRESULT __stdcall GetLevelDesc				( UINT Level, D_D3DSURFACE_DESC *pDesc)  = 0 ;
	virtual HRESULT __stdcall GetCubeMapSurface			( D_D3DCUBEMAP_FACES FaceType, UINT Level, class D_IDirect3DSurface9** ppCubeMapSurface)  = 0 ;
	virtual HRESULT __stdcall LockRect					( D_D3DCUBEMAP_FACES FaceType, UINT Level, D_D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags)  = 0 ;
	virtual HRESULT __stdcall UnlockRect				( D_D3DCUBEMAP_FACES FaceType, UINT Level)  = 0 ;
	virtual HRESULT __stdcall AddDirtyRect				( D_D3DCUBEMAP_FACES FaceType, CONST RECT* pDirtyRect)  = 0 ;
};

class D_IDirect3DVertexBuffer9 : public D_IDirect3DResource9
{
public :
	virtual HRESULT __stdcall Lock						( UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags)  = 0 ;
	virtual HRESULT __stdcall Unlock					( void )  = 0 ;
	virtual HRESULT __stdcall GetDesc					( D_D3DVERTEXBUFFER_DESC *pDesc)  = 0 ;
};

class D_IDirect3DIndexBuffer9 : public D_IDirect3DResource9
{
public :
	virtual HRESULT __stdcall Lock						( UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags ) = 0 ;
	virtual HRESULT __stdcall Unlock					( void ) = 0 ;
	virtual HRESULT __stdcall GetDesc					( D_D3DINDEXBUFFER_DESC *pDesc) = 0 ;
};

class D_IDirect3DSwapChain9 : public D_IUnknown
{
public :
	virtual HRESULT __stdcall Present					( CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion, DWORD dwFlags) = 0 ;
	virtual HRESULT __stdcall GetFrontBufferData		( class D_IDirect3DSurface9* pDestSurface) = 0 ;
	virtual HRESULT __stdcall GetBackBuffer				( UINT iBackBuffer, D_D3DBACKBUFFER_TYPE Type, class D_IDirect3DSurface9** ppBackBuffer) = 0 ;
	virtual HRESULT __stdcall GetRasterStatus			( D_D3DRASTER_STATUS* pRasterStatus) = 0 ;
	virtual HRESULT __stdcall GetDisplayMode			( D_D3DDISPLAYMODE* pMode) = 0 ;
	virtual HRESULT __stdcall GetDevice					( class D_IDirect3DDevice9** ppDevice) = 0 ;
	virtual HRESULT __stdcall GetPresentParameters		( D_D3DPRESENT_PARAMETERS* pPresentationParameters) = 0 ;
};

class D_IDirect3DQuery9 : public D_IUnknown
{
public :
	virtual HRESULT __stdcall GetDevice					( class D_IDirect3DDevice9** ppDevice) = 0 ;
	virtual D_D3DQUERYTYPE __stdcall GetType			( void ) = 0 ;
	virtual DWORD   __stdcall GetDataSize				( void ) = 0 ;
	virtual HRESULT __stdcall Issue						( DWORD dwIssueFlags) = 0 ;
	virtual HRESULT __stdcall GetData					( void* pData, DWORD dwSize, DWORD dwGetDataFlags) = 0 ;
};

class D_IDirect3DStateBlock9 : public D_IUnknown
{
public :
	virtual HRESULT __stdcall GetDevice					( class D_IDirect3DDevice9** ppDevice) = 0 ;
	virtual HRESULT __stdcall Capture					( void ) = 0 ;
	virtual HRESULT __stdcall Apply						( void ) = 0 ;
};

class D_IDirect3DVertexDeclaration9 : public D_IUnknown
{
public :
	virtual HRESULT __stdcall GetDevice					( class D_IDirect3DDevice9** ppDevice) = 0 ;
	virtual HRESULT __stdcall GetDeclaration			( D_D3DVERTEXELEMENT9*, UINT* pNumElements) = 0 ;
};

class D_IDirect3DVertexShader9 : public D_IUnknown
{
public :
	virtual HRESULT __stdcall GetDevice					( class D_IDirect3DDevice9** ppDevice ) = 0 ;
	virtual HRESULT __stdcall GetFunction				( void*, UINT* pSizeOfData ) = 0 ;
};

class D_IDirect3DPixelShader9 : public D_IUnknown
{
public :
	virtual HRESULT __stdcall GetDevice					( class D_IDirect3DDevice9** ppDevice ) = 0 ;
	virtual HRESULT __stdcall GetFunction				( void*, UINT* pSizeOfData ) = 0 ;
};

class D_IDirect3DDevice9 : public D_IUnknown
{
public :
	virtual HRESULT __stdcall TestCooperativeLevel		( void ) = 0 ;
	virtual UINT    __stdcall GetAvailableTextureMem	( void ) = 0 ;
	virtual HRESULT __stdcall EvictManagedResources		( void ) = 0 ;
	virtual HRESULT __stdcall GetDirect3D				( class D_IDirect3D9** ppD3D9) = 0 ;
	virtual HRESULT __stdcall GetDeviceCaps				( D_D3DCAPS9* pCaps) = 0 ;
	virtual HRESULT __stdcall GetDisplayMode			( UINT iSwapChain, D_D3DDISPLAYMODE* pMode) = 0 ;
	virtual HRESULT __stdcall GetCreationParameters		( D_D3DDEVICE_CREATION_PARAMETERS *pParameters) = 0 ;
	virtual HRESULT __stdcall SetCursorProperties		( UINT XHotSpot, UINT YHotSpot, class D_IDirect3DSurface9* pCursorBitmap) = 0 ;
	virtual void    __stdcall SetCursorPosition			( int X, int Y, DWORD Flags) = 0 ;
	virtual BOOL    __stdcall ShowCursor				( BOOL bShow) = 0 ;
	virtual HRESULT __stdcall CreateAdditionalSwapChain	( D_D3DPRESENT_PARAMETERS* pPresentationParameters, class D_IDirect3DSwapChain9** pSwapChain) = 0 ;
	virtual HRESULT __stdcall GetSwapChain				( UINT iSwapChain, class D_IDirect3DSwapChain9** pSwapChain) = 0 ;
	virtual UINT    __stdcall GetNumberOfSwapChains		( void ) = 0 ;
	virtual HRESULT __stdcall Reset						( D_D3DPRESENT_PARAMETERS* pPresentationParameters) = 0 ;
	virtual HRESULT __stdcall Present					( CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion) = 0 ;
	virtual HRESULT __stdcall GetBackBuffer				( UINT iSwapChain, UINT iBackBuffer, D_D3DBACKBUFFER_TYPE Type, class D_IDirect3DSurface9** ppBackBuffer) = 0 ;
	virtual HRESULT __stdcall GetRasterStatus			( UINT iSwapChain, D_D3DRASTER_STATUS* pRasterStatus) = 0 ;
	virtual HRESULT __stdcall SetDialogBoxMode			( BOOL bEnableDialogs) = 0 ;
	virtual void    __stdcall SetGammaRamp				( UINT iSwapChain, DWORD Flags, CONST D_D3DGAMMARAMP* pRamp) = 0 ;
	virtual void    __stdcall GetGammaRamp				( UINT iSwapChain, D_D3DGAMMARAMP* pRamp) = 0 ;
	virtual HRESULT __stdcall CreateTexture				( UINT Width, UINT Height, UINT Levels, DWORD Usage, D_D3DFORMAT Format, D_D3DPOOL Pool, class D_IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle) = 0 ;
	virtual HRESULT __stdcall CreateVolumeTexture		( UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D_D3DFORMAT Format, D_D3DPOOL Pool, class D_IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle) = 0 ;
	virtual HRESULT __stdcall CreateCubeTexture			( UINT EdgeLength, UINT Levels, DWORD Usage, D_D3DFORMAT Format, D_D3DPOOL Pool, class D_IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle) = 0 ;
	virtual HRESULT __stdcall CreateVertexBuffer		( UINT Length, DWORD Usage, DWORD FVF, D_D3DPOOL Pool, class D_IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle) = 0 ;
	virtual HRESULT __stdcall CreateIndexBuffer			( UINT Length, DWORD Usage, D_D3DFORMAT Format, D_D3DPOOL Pool, class D_IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle) = 0 ;
	virtual HRESULT __stdcall CreateRenderTarget		( UINT Width, UINT Height, D_D3DFORMAT Format, D_D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, class D_IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) = 0 ;
	virtual HRESULT __stdcall CreateDepthStencilSurface	( UINT Width, UINT Height, D_D3DFORMAT Format, D_D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, class D_IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) = 0 ;
	virtual HRESULT __stdcall UpdateSurface				( class D_IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, class D_IDirect3DSurface9* pDestinationSurface, CONST POINT* pDestPoint) = 0 ;
	virtual HRESULT __stdcall UpdateTexture				( class D_IDirect3DBaseTexture9* pSourceTexture, class D_IDirect3DBaseTexture9* pDestinationTexture) = 0 ;
	virtual HRESULT __stdcall GetRenderTargetData		( class D_IDirect3DSurface9* pRenderTarget, class D_IDirect3DSurface9* pDestSurface) = 0 ;
	virtual HRESULT __stdcall GetFrontBufferData		( UINT iSwapChain, class D_IDirect3DSurface9* pDestSurface) = 0 ;
	virtual HRESULT __stdcall StretchRect				( class D_IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, class D_IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect, D_D3DTEXTUREFILTERTYPE Filter) = 0 ;
	virtual HRESULT __stdcall ColorFill					( class D_IDirect3DSurface9* pSurface, CONST RECT* pRect, D_D3DCOLOR color) = 0 ;
	virtual HRESULT __stdcall CreateOffscreenPlainSurface( UINT Width, UINT Height, D_D3DFORMAT Format, D_D3DPOOL Pool, class D_IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) = 0 ;
	virtual HRESULT __stdcall SetRenderTarget			( DWORD RenderTargetIndex, class D_IDirect3DSurface9* pRenderTarget) = 0 ;
	virtual HRESULT __stdcall GetRenderTarget			( DWORD RenderTargetIndex, class D_IDirect3DSurface9** ppRenderTarget) = 0 ;
	virtual HRESULT __stdcall SetDepthStencilSurface	( class D_IDirect3DSurface9* pNewZStencil) = 0 ;
	virtual HRESULT __stdcall GetDepthStencilSurface	( class D_IDirect3DSurface9** ppZStencilSurface) = 0 ;
	virtual HRESULT __stdcall BeginScene				( void ) = 0 ;
	virtual HRESULT __stdcall EndScene					( void ) = 0 ;
	virtual HRESULT __stdcall Clear						( DWORD Count, CONST D_D3DRECT* pRects, DWORD Flags, D_D3DCOLOR Color, float Z, DWORD Stencil) = 0 ;
	virtual HRESULT __stdcall SetTransform				( D_D3DTRANSFORMSTATETYPE State, CONST D_D3DMATRIX* pMatrix) = 0 ;
	virtual HRESULT __stdcall GetTransform				( D_D3DTRANSFORMSTATETYPE State, D_D3DMATRIX* pMatrix) = 0 ;
	virtual HRESULT __stdcall MultiplyTransform			( D_D3DTRANSFORMSTATETYPE, CONST D_D3DMATRIX*) = 0 ;
	virtual HRESULT __stdcall SetViewport				( CONST D_D3DVIEWPORT9* pViewport) = 0 ;
	virtual HRESULT __stdcall GetViewport				( D_D3DVIEWPORT9* pViewport) = 0 ;
	virtual HRESULT __stdcall SetMaterial				( CONST D_D3DMATERIAL9* pMaterial) = 0 ;
	virtual HRESULT __stdcall GetMaterial				( D_D3DMATERIAL9* pMaterial) = 0 ;
	virtual HRESULT __stdcall SetLight					( DWORD Index, CONST D_D3DLIGHT9*) = 0 ;
	virtual HRESULT __stdcall GetLight					( DWORD Index, D_D3DLIGHT9*) = 0 ;
	virtual HRESULT __stdcall LightEnable				( DWORD Index, BOOL Enable) = 0 ;
	virtual HRESULT __stdcall GetLightEnable			( DWORD Index, BOOL* pEnable) = 0 ;
	virtual HRESULT __stdcall SetClipPlane				( DWORD Index, CONST float* pPlane) = 0 ;
	virtual HRESULT __stdcall GetClipPlane				( DWORD Index, float* pPlane) = 0 ;
	virtual HRESULT __stdcall SetRenderState			( D_D3DRENDERSTATETYPE State, DWORD Value) = 0 ;
	virtual HRESULT __stdcall GetRenderState			( D_D3DRENDERSTATETYPE State, DWORD* pValue) = 0 ;
	virtual HRESULT __stdcall CreateStateBlock			( D_D3DSTATEBLOCKTYPE Type, class D_IDirect3DStateBlock9** ppSB) = 0 ;
	virtual HRESULT __stdcall BeginStateBlock			( void ) = 0 ;
	virtual HRESULT __stdcall EndStateBlock				( class D_IDirect3DStateBlock9** ppSB) = 0 ;
	virtual HRESULT __stdcall SetClipStatus				( CONST D_D3DCLIPSTATUS9* pClipStatus) = 0 ;
	virtual HRESULT __stdcall GetClipStatus				( D_D3DCLIPSTATUS9* pClipStatus) = 0 ;
	virtual HRESULT __stdcall GetTexture				( DWORD Stage, class D_IDirect3DBaseTexture9** ppTexture) = 0 ;
	virtual HRESULT __stdcall SetTexture				( DWORD Stage, class D_IDirect3DBaseTexture9* pTexture) = 0 ;
	virtual HRESULT __stdcall GetTextureStageState		( DWORD Stage, D_D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue) = 0 ;
	virtual HRESULT __stdcall SetTextureStageState		( DWORD Stage, D_D3DTEXTURESTAGESTATETYPE Type, DWORD Value) = 0 ;
	virtual HRESULT __stdcall GetSamplerState			( DWORD Sampler, D_D3DSAMPLERSTATETYPE Type, DWORD* pValue) = 0 ;
	virtual HRESULT __stdcall SetSamplerState			( DWORD Sampler, D_D3DSAMPLERSTATETYPE Type, DWORD Value) = 0 ;
	virtual HRESULT __stdcall ValidateDevice			( DWORD* pNumPasses) = 0 ;
	virtual HRESULT __stdcall SetPaletteEntries			( UINT PaletteNumber, CONST PALETTEENTRY* pEntries) = 0 ;
	virtual HRESULT __stdcall GetPaletteEntries			( UINT PaletteNumber, PALETTEENTRY* pEntries) = 0 ;
	virtual HRESULT __stdcall SetCurrentTexturePalette	( UINT PaletteNumber) = 0 ;
	virtual HRESULT __stdcall GetCurrentTexturePalette	( UINT *PaletteNumber) = 0 ;
	virtual HRESULT __stdcall SetScissorRect			( CONST RECT* pRect) = 0 ;
	virtual HRESULT __stdcall GetScissorRect			( RECT* pRect) = 0 ;
	virtual HRESULT __stdcall SetSoftwareVertexProcessing( BOOL bSoftware) = 0 ;
	virtual BOOL    __stdcall GetSoftwareVertexProcessing( void ) = 0 ;
	virtual HRESULT __stdcall SetNPatchMode				( float nSegments) = 0 ;
	virtual float   __stdcall GetNPatchMode				( void ) = 0 ;
	virtual HRESULT __stdcall DrawPrimitive				( D_D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) = 0 ;
	virtual HRESULT __stdcall DrawIndexedPrimitive		( D_D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount) = 0 ;
	virtual HRESULT __stdcall DrawPrimitiveUP			( D_D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) = 0 ;
	virtual HRESULT __stdcall DrawIndexedPrimitiveUP	( D_D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST void* pIndexData, D_D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) = 0 ;
	virtual HRESULT __stdcall ProcessVertices			( UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, class D_IDirect3DVertexBuffer9* pDestBuffer, class D_IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags) = 0 ;
	virtual HRESULT __stdcall CreateVertexDeclaration	( CONST D_D3DVERTEXELEMENT9* pVertexElements, class D_IDirect3DVertexDeclaration9** ppDecl) = 0 ;
	virtual HRESULT __stdcall SetVertexDeclaration		( class D_IDirect3DVertexDeclaration9* pDecl) = 0 ;
	virtual HRESULT __stdcall GetVertexDeclaration		( class D_IDirect3DVertexDeclaration9** ppDecl) = 0 ;
	virtual HRESULT __stdcall SetFVF					( DWORD FVF) = 0 ;
	virtual HRESULT __stdcall GetFVF					( DWORD* pFVF) = 0 ;
	virtual HRESULT __stdcall CreateVertexShader		( CONST DWORD* pFunction, class D_IDirect3DVertexShader9** ppShader) = 0 ;
	virtual HRESULT __stdcall SetVertexShader			( class D_IDirect3DVertexShader9* pShader) = 0 ;
	virtual HRESULT __stdcall GetVertexShader			( class D_IDirect3DVertexShader9** ppShader) = 0 ;
	virtual HRESULT __stdcall SetVertexShaderConstantF	( UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) = 0 ;
	virtual HRESULT __stdcall GetVertexShaderConstantF	( UINT StartRegister, float* pConstantData, UINT Vector4fCount) = 0 ;
	virtual HRESULT __stdcall SetVertexShaderConstantI	( UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount) = 0 ;
	virtual HRESULT __stdcall GetVertexShaderConstantI	( UINT StartRegister, int* pConstantData, UINT Vector4iCount) = 0 ;
	virtual HRESULT __stdcall SetVertexShaderConstantB	( UINT StartRegister, CONST BOOL* pConstantData, UINT  BoolCount) = 0 ;
	virtual HRESULT __stdcall GetVertexShaderConstantB	( UINT StartRegister, BOOL* pConstantData, UINT BoolCount) = 0 ;
	virtual HRESULT __stdcall SetStreamSource			( UINT StreamNumber, class D_IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride) = 0 ;
	virtual HRESULT __stdcall GetStreamSource			( UINT StreamNumber, class D_IDirect3DVertexBuffer9** ppStreamData, UINT* OffsetInBytes, UINT* pStride) = 0 ;
	virtual HRESULT __stdcall SetStreamSourceFreq		( UINT StreamNumber, UINT Divider) = 0 ;
	virtual HRESULT __stdcall GetStreamSourceFreq		( UINT StreamNumber, UINT* Divider) = 0 ;
	virtual HRESULT __stdcall SetIndices				( class D_IDirect3DIndexBuffer9* pIndexData) = 0 ;
	virtual HRESULT __stdcall GetIndices				( class D_IDirect3DIndexBuffer9** ppIndexData) = 0 ;
	virtual HRESULT __stdcall CreatePixelShader			( CONST DWORD* pFunction, class D_IDirect3DPixelShader9** ppShader) = 0 ;
	virtual HRESULT __stdcall SetPixelShader			( class D_IDirect3DPixelShader9* pShader) = 0 ;
	virtual HRESULT __stdcall GetPixelShader			( class D_IDirect3DPixelShader9** ppShader) = 0 ;
	virtual HRESULT __stdcall SetPixelShaderConstantF	( UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) = 0 ;
	virtual HRESULT __stdcall GetPixelShaderConstantF	( UINT StartRegister, float* pConstantData, UINT Vector4fCount) = 0 ;
	virtual HRESULT __stdcall SetPixelShaderConstantI	( UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount) = 0 ;
	virtual HRESULT __stdcall GetPixelShaderConstantI	( UINT StartRegister, int* pConstantData, UINT Vector4iCount) = 0 ;
	virtual HRESULT __stdcall SetPixelShaderConstantB	( UINT StartRegister, CONST BOOL* pConstantData, UINT  BoolCount) = 0 ;
	virtual HRESULT __stdcall GetPixelShaderConstantB	( UINT StartRegister, BOOL* pConstantData, UINT BoolCount) = 0 ;
	virtual HRESULT __stdcall DrawRectPatch				( UINT Handle, CONST float* pNumSegs, CONST D_D3DRECTPATCH_INFO* pRectPatchInfo) = 0 ;
	virtual HRESULT __stdcall DrawTriPatch				( UINT Handle, CONST float* pNumSegs, CONST D_D3DTRIPATCH_INFO* pTriPatchInfo) = 0 ;
	virtual HRESULT __stdcall DeletePatch				( UINT Handle) = 0 ;
	virtual HRESULT __stdcall CreateQuery				( D_D3DQUERYTYPE Type, class D_IDirect3DQuery9** ppQuery) = 0 ;
};

class D_IDirect3DDevice9Ex : public D_IDirect3DDevice9
{
public :
	virtual HRESULT __stdcall SetConvolutionMonoKernel	( UINT width,UINT height,float* rows,float* columns) = 0 ;
	virtual HRESULT __stdcall ComposeRects				( D_IDirect3DSurface9* pSrc, D_IDirect3DSurface9* pDst, D_IDirect3DVertexBuffer9* pSrcRectDescs,UINT NumRects,D_IDirect3DVertexBuffer9* pDstRectDescs,D_D3DCOMPOSERECTSOP Operation,int Xoffset,int Yoffset) = 0 ;
	virtual HRESULT __stdcall PresentEx					( CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion,DWORD dwFlags) = 0 ;
	virtual HRESULT __stdcall GetGPUThreadPriority		( INT* pPriority) = 0 ;
	virtual HRESULT __stdcall SetGPUThreadPriority		( INT Priority) = 0 ;
	virtual HRESULT __stdcall WaitForVBlank				( UINT iSwapChain) = 0 ;
	virtual HRESULT __stdcall CheckResourceResidency	( D_IDirect3DResource9** pResourceArray,DWORD NumResources) = 0 ;
	virtual HRESULT __stdcall SetMaximumFrameLatency	( UINT MaxLatency) = 0 ;
	virtual HRESULT __stdcall GetMaximumFrameLatency	( UINT* pMaxLatency) = 0 ;
	virtual HRESULT __stdcall CheckDeviceState			( HWND hDestinationWindow) = 0 ;
	virtual HRESULT __stdcall CreateRenderTargetEx		( UINT Width,UINT Height, D_D3DFORMAT Format, D_D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, D_IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle, DWORD Usage) = 0 ;
	virtual HRESULT __stdcall CreateOffscreenPlainSurfaceEx( UINT Width,UINT Height, D_D3DFORMAT Format, D_D3DPOOL Pool, D_IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle,DWORD Usage) = 0 ;
	virtual HRESULT __stdcall CreateDepthStencilSurfaceEx( UINT Width,UINT Height, D_D3DFORMAT Format, D_D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, D_IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle, DWORD Usage) = 0 ;
	virtual HRESULT __stdcall ResetEx					( D_D3DPRESENT_PARAMETERS* pPresentationParameters, D_D3DDISPLAYMODEEX *pFullscreenDisplayMode ) = 0 ;
	virtual HRESULT __stdcall GetDisplayModeEx			( UINT iSwapChain, D_D3DDISPLAYMODEEX* pMode, D_D3DDISPLAYROTATION* pRotation) = 0 ;
};

class D_IDirect3D9 : public D_IUnknown
{
public :
	virtual HRESULT __stdcall RegisterSoftwareDevice	( void* pInitializeFunction) = 0 ;
	virtual UINT    __stdcall GetAdapterCount			( void ) = 0 ;
	virtual HRESULT __stdcall GetAdapterIdentifier		( DWORD Adapter, DWORD Flags, D_D3DADAPTER_IDENTIFIER9* pIdentifier) = 0 ;
	virtual UINT    __stdcall GetAdapterModeCount		( DWORD Adapter, D_D3DFORMAT Format) = 0 ;
	virtual HRESULT __stdcall EnumAdapterModes			( DWORD Adapter, D_D3DFORMAT Format, DWORD Mode,D_D3DDISPLAYMODE* pMode) = 0 ;
	virtual HRESULT __stdcall GetAdapterDisplayMode		( DWORD Adapter, D_D3DDISPLAYMODE* pMode) = 0 ;
	virtual HRESULT __stdcall CheckDeviceType			( DWORD Adapter, D_D3DDEVTYPE DevType, D_D3DFORMAT DisplayFormat, D_D3DFORMAT BackBufferFormat, BOOL bWindowed) = 0 ;
	virtual HRESULT __stdcall CheckDeviceFormat			( DWORD Adapter, D_D3DDEVTYPE DeviceType, D_D3DFORMAT AdapterFormat, DWORD Usage, D_D3DRESOURCETYPE RType, D_D3DFORMAT CheckFormat) = 0 ;
	virtual HRESULT __stdcall CheckDeviceMultiSampleType( DWORD Adapter, D_D3DDEVTYPE DeviceType, D_D3DFORMAT SurfaceFormat, BOOL Windowed, D_D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels) = 0 ;
	virtual HRESULT __stdcall CheckDepthStencilMatch	( DWORD Adapter, D_D3DDEVTYPE DeviceType, D_D3DFORMAT AdapterFormat, D_D3DFORMAT RenderTargetFormat, D_D3DFORMAT DepthStencilFormat) = 0 ;
	virtual HRESULT __stdcall CheckDeviceFormatConversion( DWORD Adapter, D_D3DDEVTYPE DeviceType, D_D3DFORMAT SourceFormat, D_D3DFORMAT TargetFormat) = 0 ;
	virtual HRESULT __stdcall GetDeviceCaps				( DWORD Adapter, D_D3DDEVTYPE DeviceType, D_D3DCAPS9* pCaps) = 0 ;
	virtual HANDLE	__stdcall GetAdapterMonitor			( DWORD Adapter ) = 0 ;
	virtual HRESULT __stdcall CreateDevice				( DWORD Adapter, D_D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D_D3DPRESENT_PARAMETERS* pPresentationParameters, D_IDirect3DDevice9** ppReturnedDeviceInterface ) = 0 ;
} ;

class D_IDirect3D9Ex : public D_IDirect3D9
{
public :
	virtual UINT    __stdcall GetAdapterModeCountEx		( UINT Adapter,CONST D_D3DDISPLAYMODEFILTER* pFilter ) = 0 ;
	virtual HRESULT __stdcall EnumAdapterModesEx		( UINT Adapter,CONST D_D3DDISPLAYMODEFILTER* pFilter,UINT Mode,D_D3DDISPLAYMODEEX* pMode) = 0 ;
	virtual HRESULT __stdcall GetAdapterDisplayModeEx	( UINT Adapter,D_D3DDISPLAYMODEEX* pMode,D_D3DDISPLAYROTATION* pRotation) = 0 ;
	virtual HRESULT __stdcall CreateDeviceEx			( UINT Adapter, D_D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D_D3DPRESENT_PARAMETERS* pPresentationParameters, D_D3DDISPLAYMODEEX* pFullscreenDisplayMode, D_IDirect3DDevice9Ex** ppReturnedDeviceInterface ) = 0 ;
	virtual HRESULT __stdcall GetAdapterLUID			( UINT Adapter,LUID * pLUID) = 0 ;
};

class D_IDirect3D7 : public D_IUnknown
{
public :
	virtual HRESULT __stdcall EnumDevices				( LPD_D3DENUMDEVICESCALLBACK7, LPVOID ) = 0 ;
	virtual HRESULT __stdcall CreateDevice				( REFCLSID, D_IDirectDrawSurface7 *, class D_IDirect3DDevice7 ** ) = 0 ;
	virtual HRESULT __stdcall CreateVertexBuffer		( D_D3DVERTEXBUFFERDESC *, class D_IDirect3DVertexBuffer7 **, DWORD ) = 0 ;
	virtual HRESULT __stdcall EnumZBufferFormats		( REFCLSID, LPD_D3DENUMPIXELFORMATSCALLBACK, LPVOID ) = 0 ;
	virtual HRESULT __stdcall NonUse02					( void ) = 0 ;				// EvictManagedTextures( void ) = 0 ;
} ;

class D_IDirect3DDevice7 : public D_IUnknown
{
public :
	virtual HRESULT __stdcall GetCaps					( D_D3DDEVICEDESC7 * ) = 0 ;
	virtual HRESULT __stdcall EnumTextureFormats		( LPD_D3DENUMPIXELFORMATSCALLBACK, LPVOID ) = 0 ;
	virtual HRESULT __stdcall BeginScene				( void ) = 0 ;
	virtual HRESULT __stdcall EndScene					( void ) = 0 ;
	virtual HRESULT __stdcall NonUse00					( void ) = 0 ;				// GetDirect3D( LPDIRECT3D7* ) = 0 ;
	virtual HRESULT __stdcall SetRenderTarget			( D_IDirectDrawSurface7 *,DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse01					( void ) = 0 ;				// GetRenderTarget( D_IDirectDrawSurface7 * * ) = 0 ;
	virtual HRESULT __stdcall Clear						( DWORD, D_D3DRECT *, DWORD, D_D3DCOLOR, D_D3DVALUE, DWORD ) = 0 ;
	virtual HRESULT __stdcall SetTransform				( D_D3DTRANSFORMSTATETYPE, D_D3DMATRIX * ) = 0 ;
	virtual HRESULT __stdcall NonUse02					( void ) = 0 ;				// GetTransform( D3DTRANSFORMSTATETYPE,LPD3DMATRIX ) = 0 ;
	virtual HRESULT __stdcall SetViewport				( D_D3DVIEWPORT7 * ) = 0 ;
	virtual HRESULT __stdcall NonUse03					( void ) = 0 ;				// MultiplyTransform( D3DTRANSFORMSTATETYPE,LPD3DMATRIX ) = 0 ;
	virtual HRESULT __stdcall NonUse04					( void ) = 0 ;				// GetViewport( LPD3DVIEWPORT7 ) = 0 ;
	virtual HRESULT __stdcall SetMaterial				( D_D3DMATERIAL7 * ) = 0 ;
	virtual HRESULT __stdcall NonUse05					( void ) = 0 ;				// GetMaterial( LPD3DMATERIAL7 ) = 0 ;
	virtual HRESULT __stdcall SetLight					( DWORD, D_D3DLIGHT7 * ) = 0 ;
	virtual HRESULT __stdcall NonUse06					( void ) = 0 ;				// GetLight( DWORD,LPD3DLIGHT7 ) = 0 ;
	virtual HRESULT __stdcall SetRenderState			( D_D3DRENDERSTATETYPE, DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse07					( void ) = 0 ;				// GetRenderState( D3DRENDERSTATETYPE,LPDWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse08					( void ) = 0 ;				// BeginStateBlock( void	) = 0 ;
	virtual HRESULT __stdcall NonUse09					( void ) = 0 ;				// EndStateBlock( LPDWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse10					( void ) = 0 ;				// PreLoad( D_IDirectDrawSurface7 * ) = 0 ;
	virtual HRESULT __stdcall DrawPrimitive				( D_D3DPRIMITIVETYPE, DWORD, LPVOID, DWORD, DWORD ) = 0 ;
	virtual HRESULT __stdcall DrawIndexedPrimitive		( D_D3DPRIMITIVETYPE, DWORD, LPVOID, DWORD, LPWORD, DWORD, DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse11					( void ) = 0 ;				// SetClipStatus( void ) = 0 ;					// SetClipStatus( LPD3DCLIPSTATUS ) = 0 ;
	virtual HRESULT __stdcall NonUse12					( void ) = 0 ;				// GetClipStatus( LPD3DCLIPSTATUS ) = 0 ;
	virtual HRESULT __stdcall NonUse13					( void ) = 0 ;				// DrawPrimitiveStrided( D_D3DPRIMITIVETYPE,DWORD,LPD3DDRAWPRIMITIVESTRIDEDDATA,DWORD,DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse14					( void ) = 0 ;				// DrawIndexedPrimitiveStrided( D_D3DPRIMITIVETYPE,DWORD,LPD3DDRAWPRIMITIVESTRIDEDDATA,DWORD,LPWORD,DWORD,DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse15					( void ) = 0 ;				// DrawPrimitiveVB( D_D3DPRIMITIVETYPE,LPDIRECT3DVERTEXBUFFER7,DWORD,DWORD,DWORD ) = 0 ;
	virtual HRESULT __stdcall DrawIndexedPrimitiveVB	( D_D3DPRIMITIVETYPE, D_IDirect3DVertexBuffer7 *, DWORD, DWORD, LPWORD, DWORD, DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse17					( void ) = 0 ;				// ComputeSphereVisibility( LPD3DVECTOR,LPD3DVALUE,DWORD,DWORD,LPDWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse18					( void ) = 0 ;				// GetTexture( DWORD,D_IDirectDrawSurface7 * * ) = 0 ;
	virtual HRESULT __stdcall SetTexture				( DWORD, D_IDirectDrawSurface7 * ) = 0 ;
	virtual HRESULT __stdcall NonUse19					( void ) = 0 ;				// GetTextureStageState( DWORD,D_D3DTEXTURESTAGESTATETYPE,LPDWORD ) = 0 ;
	virtual HRESULT __stdcall SetTextureStageState		( DWORD, D_D3DTEXTURESTAGESTATETYPE, DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse20					( void ) = 0 ;				// ValidateDevice( LPDWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse21					( void ) = 0 ;				// ApplyStateBlock( DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse22					( void ) = 0 ;				// CaptureStateBlock( DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse23					( void ) = 0 ;				// DeleteStateBlock( DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse24					( void ) = 0 ;				// CreateStateBlock( D_D3DSTATEBLOCKTYPE,LPDWORD ) = 0 ;
	virtual HRESULT __stdcall Load						( D_IDirectDrawSurface7 *, LPPOINT, D_IDirectDrawSurface7 *, LPRECT, DWORD ) = 0 ;
	virtual HRESULT __stdcall LightEnable				( DWORD, BOOL ) = 0 ;
	virtual HRESULT __stdcall NonUse25					( void ) = 0 ;				// GetLightEnable( DWORD,BOOL* ) = 0 ;
	virtual HRESULT __stdcall NonUse26					( void ) = 0 ;				// SetClipPlane( DWORD,D3DVALUE* ) = 0 ;
	virtual HRESULT __stdcall NonUse27					( void ) = 0 ;				// GetClipPlane( DWORD,D3DVALUE* ) = 0 ;
	virtual HRESULT __stdcall NonUse28					( void ) = 0 ;				// GetInfo( DWORD,LPVOID,DWORD ) = 0 ;
} ;

class D_IDirect3DVertexBuffer7 : public D_IUnknown
{
public :
	virtual HRESULT __stdcall Lock						( DWORD, LPVOID * , LPDWORD ) = 0 ;
	virtual HRESULT __stdcall Unlock					( void ) = 0 ;
	virtual HRESULT __stdcall ProcessVertices			( DWORD, DWORD, DWORD, D_IDirect3DVertexBuffer7 *, DWORD, D_IDirect3DDevice7 *, DWORD ) = 0 ;
	virtual HRESULT __stdcall GetVertexBufferDesc		( D_D3DVERTEXBUFFERDESC * ) = 0 ;
	virtual HRESULT __stdcall Optimize					( D_IDirect3DDevice7 *, DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse1					( void ) = 0 ;				// ProcessVerticesStrided	( DWORD, DWORD, DWORD, LPD3DDRAWPRIMITIVESTRIDEDDATA, DWORD, LPDIRECT3DDEVICE7, DWORD ) = 0 ;
};

class D_ID3DXBuffer : public D_IUnknown
{
public :
	virtual LPVOID	__stdcall GetBufferPointer			( void ) = 0 ;
	virtual DWORD	__stdcall GetBufferSize				( void ) = 0 ;
} ;

class D_ID3DXConstantTable : public D_ID3DXBuffer
{
public :
	virtual HRESULT __stdcall GetDesc					( D_D3DXCONSTANTTABLE_DESC *pDesc ) = 0 ;
	virtual HRESULT __stdcall GetConstantDesc			( UINT_PTR hConstant, D_D3DXCONSTANT_DESC *pConstantDesc, UINT *pCount ) = 0 ;
	virtual UINT	__stdcall GetSamplerIndex			( UINT_PTR hConstant ) = 0 ;

	virtual UINT_PTR __stdcall GetConstant				( UINT_PTR hConstant, UINT Index ) = 0 ;
	virtual UINT_PTR __stdcall GetConstantByName		( UINT_PTR hConstant, LPCSTR pName ) = 0 ;
	virtual UINT_PTR __stdcall GetConstantElement		( UINT_PTR hConstant, UINT Index ) = 0 ;

	virtual HRESULT __stdcall SetDefaults				( D_IDirect3DDevice9 *pDevice ) = 0 ;
	virtual HRESULT __stdcall SetValue					( D_IDirect3DDevice9 *pDevice, UINT_PTR hConstant, LPCVOID pData, UINT Bytes ) = 0 ;
	virtual HRESULT __stdcall SetBool					( D_IDirect3DDevice9 *pDevice, UINT_PTR hConstant, BOOL b ) = 0 ;
	virtual HRESULT __stdcall SetBoolArray				( D_IDirect3DDevice9 *pDevice, UINT_PTR hConstant, CONST BOOL* pb, UINT Count ) = 0 ;
	virtual HRESULT __stdcall SetInt					( D_IDirect3DDevice9 *pDevice, UINT_PTR hConstant, INT n ) = 0 ;
	virtual HRESULT __stdcall SetIntArray				( D_IDirect3DDevice9 *pDevice, UINT_PTR hConstant, CONST INT* pn, UINT Count ) = 0 ;
	virtual HRESULT __stdcall SetFloat					( D_IDirect3DDevice9 *pDevice, UINT_PTR hConstant, FLOAT f ) = 0 ;
	virtual HRESULT __stdcall SetFloatArray				( D_IDirect3DDevice9 *pDevice, UINT_PTR hConstant, CONST FLOAT* pf, UINT Count ) = 0 ;
	virtual HRESULT __stdcall SetVector					( D_IDirect3DDevice9 *pDevice, UINT_PTR hConstant, CONST FLOAT /* D3DXVECTOR4 */* pVector ) = 0 ;
	virtual HRESULT __stdcall SetVectorArray			( D_IDirect3DDevice9 *pDevice, UINT_PTR hConstant, CONST FLOAT /* D3DXVECTOR4 */* pVector, UINT Count ) = 0 ;
	virtual HRESULT __stdcall SetMatrix					( D_IDirect3DDevice9 *pDevice, UINT_PTR hConstant, CONST FLOAT /* D3DXMATRIX */* pMatrix ) = 0 ;
	virtual HRESULT __stdcall SetMatrixArray			( D_IDirect3DDevice9 *pDevice, UINT_PTR hConstant, CONST FLOAT /* D3DXMATRIX */* pMatrix, UINT Count ) = 0 ;
	virtual HRESULT __stdcall SetMatrixPointerArray		( D_IDirect3DDevice9 *pDevice, UINT_PTR hConstant, CONST FLOAT /* D3DXMATRIX */** ppMatrix, UINT Count ) = 0 ;
	virtual HRESULT __stdcall SetMatrixTranspose		( D_IDirect3DDevice9 *pDevice, UINT_PTR hConstant, CONST FLOAT /* D3DXMATRIX */* pMatrix ) = 0 ;
	virtual HRESULT __stdcall SetMatrixTransposeArray	( D_IDirect3DDevice9 *pDevice, UINT_PTR hConstant, CONST FLOAT /* D3DXMATRIX */* pMatrix, UINT Count ) = 0 ;
	virtual HRESULT __stdcall SetMatrixTransposePointerArray( D_IDirect3DDevice9 *pDevice, UINT_PTR hConstant, CONST FLOAT /* D3DXMATRIX */** ppMatrix, UINT Count ) = 0 ;
} ;

// ＸＩｎｐｕｔ ---------------------------------------------------------------

#define D_XINPUT_GAMEPAD_DPAD_UP				(0x00000001)
#define D_XINPUT_GAMEPAD_DPAD_DOWN				(0x00000002)
#define D_XINPUT_GAMEPAD_DPAD_LEFT				(0x00000004)
#define D_XINPUT_GAMEPAD_DPAD_RIGHT				(0x00000008)
#define D_XINPUT_GAMEPAD_START					(0x00000010)
#define D_XINPUT_GAMEPAD_BACK					(0x00000020)
#define D_XINPUT_GAMEPAD_LEFT_THUMB				(0x00000040)
#define D_XINPUT_GAMEPAD_RIGHT_THUMB			(0x00000080)
#define D_XINPUT_GAMEPAD_LEFT_SHOULDER			(0x00000100)
#define D_XINPUT_GAMEPAD_RIGHT_SHOULDER			(0x00000200)
#define D_XINPUT_GAMEPAD_A						(0x00001000)
#define D_XINPUT_GAMEPAD_B						(0x00002000)
#define D_XINPUT_GAMEPAD_X						(0x00004000)
#define D_XINPUT_GAMEPAD_Y						(0x00008000)

typedef struct tagD_XINPUT_GAMEPAD
{
    WORD										wButtons;
    BYTE										bLeftTrigger;
    BYTE										bRightTrigger;
    SHORT										sThumbLX;
    SHORT										sThumbLY;
    SHORT										sThumbRX;
    SHORT										sThumbRY;
} D_XINPUT_GAMEPAD ;

typedef struct tagD_XINPUT_STATE
{
    DWORD										dwPacketNumber;
    D_XINPUT_GAMEPAD							Gamepad;
} D_XINPUT_STATE ;

typedef struct tagD_XINPUT_VIBRATION
{
    WORD										wLeftMotorSpeed;
    WORD										wRightMotorSpeed;
} D_XINPUT_VIBRATION ;

// ＤｉｒｅｃｔＩｎｐｕｔ -----------------------------------------------------

#define D_DI_OK									(S_OK)
#define D_DIDEVTYPE_KEYBOARD					(3)
#define D_DIDEVTYPE_JOYSTICK					(4)
#define D_DIEDFL_ATTACHEDONLY					(0x00000001)

#define D_DIENUM_STOP							(0)
#define D_DIENUM_CONTINUE						(1)

#define D_DIPROP_BUFFERSIZE						(*(const GUID *)(1))
#define D_DIPROP_RANGE							(*(const GUID *)(4))
#define D_DIPROP_DEADZONE						(*(const GUID *)(5))

#define D_DI_POLLEDDEVICE						((HRESULT)0x00000002L)

#define D_DISCL_EXCLUSIVE						(0x00000001)
#define D_DISCL_NONEXCLUSIVE					(0x00000002)
#define D_DISCL_FOREGROUND						(0x00000004)
#define D_DISCL_BACKGROUND						(0x00000008)

#define D_DIJOFS_X								FIELD_OFFSET(D_DIJOYSTATE, lX)
#define D_DIJOFS_Y								FIELD_OFFSET(D_DIJOYSTATE, lY)
#define D_DIJOFS_Z								FIELD_OFFSET(D_DIJOYSTATE, lZ)
#define D_DIJOFS_RX								FIELD_OFFSET(D_DIJOYSTATE, lRx)
#define D_DIJOFS_RY								FIELD_OFFSET(D_DIJOYSTATE, lRy)
#define D_DIJOFS_RZ								FIELD_OFFSET(D_DIJOYSTATE, lRz)

#define D_DIPH_DEVICE							(0)
#define D_DIPH_BYOFFSET							(1)
#define D_DIDFT_ENUMCOLLECTION(n)				((WORD)(n) << 8)

#define D_DIK_ESCAPE							(0x01)
#define D_DIK_1									(0x02)
#define D_DIK_2									(0x03)
#define D_DIK_3									(0x04)
#define D_DIK_4									(0x05)
#define D_DIK_5									(0x06)
#define D_DIK_6									(0x07)
#define D_DIK_7									(0x08)
#define D_DIK_8									(0x09)
#define D_DIK_9									(0x0A)
#define D_DIK_0									(0x0B)
#define D_DIK_MINUS								(0x0C)
#define D_DIK_EQUALS							(0x0D)
#define D_DIK_BACK								(0x0E)
#define D_DIK_TAB								(0x0F)
#define D_DIK_Q									(0x10)
#define D_DIK_W									(0x11)
#define D_DIK_E									(0x12)
#define D_DIK_R									(0x13)
#define D_DIK_T									(0x14)
#define D_DIK_Y									(0x15)
#define D_DIK_U									(0x16)
#define D_DIK_I									(0x17)
#define D_DIK_O									(0x18)
#define D_DIK_P									(0x19)
#define D_DIK_LBRACKET							(0x1A)
#define D_DIK_RBRACKET							(0x1B)
#define D_DIK_RETURN							(0x1C)
#define D_DIK_LCONTROL							(0x1D)
#define D_DIK_A									(0x1E)
#define D_DIK_S									(0x1F)
#define D_DIK_D									(0x20)
#define D_DIK_F									(0x21)
#define D_DIK_G									(0x22)
#define D_DIK_H									(0x23)
#define D_DIK_J									(0x24)
#define D_DIK_K									(0x25)
#define D_DIK_L									(0x26)
#define D_DIK_SEMICOLON							(0x27)
#define D_DIK_APOSTROPHE						(0x28)
#define D_DIK_GRAVE								(0x29)
#define D_DIK_LSHIFT							(0x2A)
#define D_DIK_BACKSLASH							(0x2B)
#define D_DIK_Z									(0x2C)
#define D_DIK_X									(0x2D)
#define D_DIK_C									(0x2E)
#define D_DIK_V									(0x2F)
#define D_DIK_B									(0x30)
#define D_DIK_N									(0x31)
#define D_DIK_M									(0x32)
#define D_DIK_COMMA								(0x33)
#define D_DIK_PERIOD							(0x34)
#define D_DIK_SLASH								(0x35)
#define D_DIK_RSHIFT							(0x36)
#define D_DIK_MULTIPLY							(0x37)
#define D_DIK_LMENU								(0x38)
#define D_DIK_SPACE								(0x39)
#define D_DIK_CAPITAL							(0x3A)
#define D_DIK_F1								(0x3B)
#define D_DIK_F2								(0x3C)
#define D_DIK_F3								(0x3D)
#define D_DIK_F4								(0x3E)
#define D_DIK_F5								(0x3F)
#define D_DIK_F6								(0x40)
#define D_DIK_F7								(0x41)
#define D_DIK_F8								(0x42)
#define D_DIK_F9								(0x43)
#define D_DIK_F10								(0x44)
#define D_DIK_NUMLOCK							(0x45)
#define D_DIK_SCROLL							(0x46)
#define D_DIK_NUMPAD7							(0x47)
#define D_DIK_NUMPAD8							(0x48)
#define D_DIK_NUMPAD9							(0x49)
#define D_DIK_SUBTRACT							(0x4A)
#define D_DIK_NUMPAD4							(0x4B)
#define D_DIK_NUMPAD5							(0x4C)
#define D_DIK_NUMPAD6							(0x4D)
#define D_DIK_ADD								(0x4E)
#define D_DIK_NUMPAD1							(0x4F)
#define D_DIK_NUMPAD2							(0x50)
#define D_DIK_NUMPAD3							(0x51)
#define D_DIK_NUMPAD0							(0x52)
#define D_DIK_DECIMAL							(0x53)
#define D_DIK_OEM_102							(0x56)
#define D_DIK_F11								(0x57)
#define D_DIK_F12								(0x58)
#define D_DIK_F13								(0x64)
#define D_DIK_F14								(0x65)
#define D_DIK_F15								(0x66)
#define D_DIK_KANA								(0x70)
#define D_DIK_ABNT_C1							(0x73)
#define D_DIK_CONVERT							(0x79)
#define D_DIK_NOCONVERT							(0x7B)
#define D_DIK_YEN								(0x7D)
#define D_DIK_ABNT_C2							(0x7E)
#define D_DIK_NUMPADEQUALS						(0x8D)
#define D_DIK_PREVTRACK							(0x90)
#define D_DIK_AT								(0x91)
#define D_DIK_COLON								(0x92)
#define D_DIK_UNDERLINE							(0x93)
#define D_DIK_KANJI								(0x94)
#define D_DIK_STOP								(0x95)
#define D_DIK_AX								(0x96)
#define D_DIK_UNLABELED							(0x97)
#define D_DIK_NEXTTRACK							(0x99)
#define D_DIK_NUMPADENTER						(0x9C)
#define D_DIK_RCONTROL							(0x9D)
#define D_DIK_MUTE								(0xA0)
#define D_DIK_CALCULATOR						(0xA1)
#define D_DIK_PLAYPAUSE							(0xA2)
#define D_DIK_MEDIASTOP							(0xA4)
#define D_DIK_VOLUMEDOWN						(0xAE)
#define D_DIK_VOLUMEUP							(0xB0)
#define D_DIK_WEBHOME							(0xB2)
#define D_DIK_NUMPADCOMMA						(0xB3)
#define D_DIK_DIVIDE							(0xB5)
#define D_DIK_SYSRQ								(0xB7)
#define D_DIK_RMENU								(0xB8)
#define D_DIK_PAUSE								(0xC5)
#define D_DIK_HOME								(0xC7)
#define D_DIK_UP								(0xC8)
#define D_DIK_PRIOR								(0xC9)
#define D_DIK_LEFT								(0xCB)
#define D_DIK_RIGHT								(0xCD)
#define D_DIK_END								(0xCF)
#define D_DIK_DOWN								(0xD0)
#define D_DIK_NEXT								(0xD1)
#define D_DIK_INSERT							(0xD2)
#define D_DIK_DELETE							(0xD3)
#define D_DIK_LWIN								(0xDB)
#define D_DIK_RWIN								(0xDC)
#define D_DIK_APPS								(0xDD)
#define D_DIK_POWER								(0xDE)
#define D_DIK_SLEEP								(0xDF)
#define D_DIK_WAKE								(0xE3)
#define D_DIK_WEBSEARCH							(0xE5)
#define D_DIK_WEBFAVORITES						(0xE6)
#define D_DIK_WEBREFRESH						(0xE7)
#define D_DIK_WEBSTOP							(0xE8)
#define D_DIK_WEBFORWARD						(0xE9)
#define D_DIK_WEBBACK							(0xEA)
#define D_DIK_MYCOMPUTER						(0xEB)
#define D_DIK_MAIL								(0xEC)
#define D_DIK_MEDIASELECT						(0xED)

#define D_DIK_BACKSPACE							D_DIK_BACK
#define D_DIK_NUMPADSTAR						D_DIK_MULTIPLY
#define D_DIK_LALT								D_DIK_LMENU
#define D_DIK_CAPSLOCK							D_DIK_CAPITAL
#define D_DIK_NUMPADMINUS						D_DIK_SUBTRACT
#define D_DIK_NUMPADPLUS						D_DIK_ADD
#define D_DIK_NUMPADPERIOD						D_DIK_DECIMAL
#define D_DIK_NUMPADSLASH						D_DIK_DIVIDE
#define D_DIK_RALT								D_DIK_RMENU
#define D_DIK_UPARROW							D_DIK_UP
#define D_DIK_PGUP								D_DIK_PRIOR
#define D_DIK_LEFTARROW							D_DIK_LEFT
#define D_DIK_RIGHTARROW						D_DIK_RIGHT
#define D_DIK_DOWNARROW							D_DIK_DOWN
#define D_DIK_PGDN								D_DIK_NEXT

#define D_DI_DEGREES							(100)
#define D_DI_FFNOMINALMAX						(10000)
#define D_DI_SECONDS							(1000000)

#define D_DIEFF_OBJECTIDS						(0x00000001)
#define D_DIEFF_OBJECTOFFSETS					(0x00000002)
#define D_DIEFF_CARTESIAN						(0x00000010)
#define D_DIEFF_POLAR							(0x00000020)
#define D_DIEFF_SPHERICAL						(0x00000040)

#define D_DIJOFS_BUTTON(n)						(FIELD_OFFSET(D_DIJOYSTATE, rgbButtons) + (n))

#define D_DIEP_DURATION							(0x00000001)
#define D_DIEP_SAMPLEPERIOD						(0x00000002)
#define D_DIEP_GAIN								(0x00000004)
#define D_DIEP_TRIGGERBUTTON					(0x00000008)
#define D_DIEP_TRIGGERREPEATINTERVAL			(0x00000010)
#define D_DIEP_AXES								(0x00000020)
#define D_DIEP_DIRECTION						(0x00000040)
#define D_DIEP_ENVELOPE							(0x00000080)
#define D_DIEP_TYPESPECIFICPARAMS				(0x00000100)
#define D_DIEP_START							(0x20000000)
#define D_DIEP_NORESTART						(0x40000000)
#define D_DIEP_NODOWNLOAD						(0x80000000)
#define D_DIEB_NOTRIGGER						(0xFFFFFFFF)

#define D_DIEFT_ALL								(0x00000000)
#define D_DIEFT_CONSTANTFORCE					(0x00000001)
#define D_DIEFT_RAMPFORCE						(0x00000002)
#define D_DIEFT_PERIODIC						(0x00000003)
#define D_DIEFT_CONDITION						(0x00000004)
#define D_DIEFT_CUSTOMFORCE						(0x00000005)
#define D_DIEFT_HARDWARE						(0x000000FF)
#define D_DIEFT_FFATTACK						(0x00000200)
#define D_DIEFT_FFFADE							(0x00000400)
#define D_DIEFT_SATURATION						(0x00000800)
#define D_DIEFT_POSNEGCOEFFICIENTS				(0x00001000)
#define D_DIEFT_POSNEGSATURATION				(0x00002000)
#define D_DIEFT_DEADBAND						(0x00004000)
#define D_DIEFT_STARTDELAY						(0x00008000)

typedef struct tagD_DIDEVICEOBJECTINSTANCEA
{
	DWORD										dwSize;
	GUID										guidType;
	DWORD										dwOfs;
	DWORD										dwType;
	DWORD										dwFlags;
	CHAR										tszName[MAX_PATH];
//		#if(DIRECTINPUT_VERSION >= 0x0500)
	DWORD										dwFFMaxForce;
	DWORD										dwFFForceResolution;
	WORD										wCollectionNumber;
	WORD										wDesignatorIndex;
	WORD										wUsagePage;
	WORD										wUsage;
	DWORD										dwDimension;
	WORD										wExponent;
	WORD										wReportId;
//		#endif
} D_DIDEVICEOBJECTINSTANCEA;

typedef struct tagD_DIDEVICEOBJECTINSTANCEW
{
	DWORD										dwSize;
	GUID										guidType;
	DWORD										dwOfs;
	DWORD										dwType;
	DWORD										dwFlags;
	WCHAR										tszName[MAX_PATH];
//		#if(DIRECTINPUT_VERSION >= 0x0500)
	DWORD										dwFFMaxForce;
	DWORD										dwFFForceResolution;
	WORD										wCollectionNumber;
	WORD										wDesignatorIndex;
	WORD										wUsagePage;
	WORD										wUsage;
	DWORD										dwDimension;
	WORD										wExponent;
	WORD										wReportId;
//		#endif
} D_DIDEVICEOBJECTINSTANCEW;

typedef struct tagD_DIDEVICEINSTANCEA
{
	DWORD										dwSize;
	GUID										guidInstance;
	GUID										guidProduct;
	DWORD										dwDevType;
	CHAR										tszInstanceName[MAX_PATH];
	CHAR										tszProductName[MAX_PATH];
//		#if (DIRECTINPUT_VERSION >= 0x0500)
	GUID										guidFFDriver;
	WORD										wUsagePage;
	WORD										wUsage;
//		#endif
} D_DIDEVICEINSTANCEA ;

typedef struct tagD_DIDEVICEINSTANCEW
{
	DWORD										dwSize;
	GUID										guidInstance;
	GUID										guidProduct;
	DWORD										dwDevType;
	WCHAR										tszInstanceName[MAX_PATH];
	WCHAR										tszProductName[MAX_PATH];
//		#if (DIRECTINPUT_VERSION >= 0x0500)
	GUID										guidFFDriver;
	WORD										wUsagePage;
	WORD										wUsage;
//		#endif
} D_DIDEVICEINSTANCEW ;

typedef struct tagD_DIPROPHEADER
{
	DWORD										dwSize;
	DWORD										dwHeaderSize;
	DWORD										dwObj;
	DWORD										dwHow;
} D_DIPROPHEADER ;

typedef struct tagD_DIPROPRANGE
{
	D_DIPROPHEADER 								diph;
	LONG										lMin;
	LONG										lMax;
} D_DIPROPRANGE ;

typedef struct tagD_DIPROPDWORD
{
	D_DIPROPHEADER 								diph;
	DWORD										dwData;
} D_DIPROPDWORD ;

typedef struct tagD_DIDEVICEOBJECTDATA_DX8
{
	DWORD										dwOfs;
	DWORD										dwData;
	DWORD										dwTimeStamp;
	DWORD										dwSequence;
	UINT_PTR									uAppData;
} D_DIDEVICEOBJECTDATA_DX8 ;

typedef struct tagD_DIDEVICEOBJECTDATA_DX7
{
	DWORD										dwOfs;
	DWORD										dwData;
	DWORD										dwTimeStamp;
	DWORD										dwSequence;
} D_DIDEVICEOBJECTDATA_DX7 ;

typedef struct tagD_DIJOYSTATE
{
	LONG										lX;
	LONG										lY;
	LONG										lZ;
	LONG										lRx;
	LONG										lRy;
	LONG										lRz;
	LONG										rglSlider[2];
	DWORD										rgdwPOV[4];
	BYTE										rgbButtons[32];
} D_DIJOYSTATE ;

typedef struct tagD_DIJOYSTATE2
{
	LONG										lX;
	LONG										lY;
	LONG										lZ;
	LONG										lRx;
	LONG										lRy;
	LONG										lRz;
	LONG										rglSlider[2];
	DWORD										rgdwPOV[4];
	BYTE										rgbButtons[128];
	LONG										lVX;
	LONG										lVY;
	LONG										lVZ;
	LONG										lVRx;
	LONG										lVRy;
	LONG										lVRz;
	LONG										rglVSlider[2];
	LONG										lAX;
	LONG										lAY;
	LONG										lAZ;
	LONG										lARx;
	LONG										lARy;
	LONG										lARz;
	LONG										rglASlider[2];
	LONG										lFX;
	LONG										lFY;
	LONG										lFZ;
	LONG										lFRx;
	LONG										lFRy;
	LONG										lFRz;
	LONG										rglFSlider[2];
} D_DIJOYSTATE2 ;

typedef struct tagD_DIMOUSESTATE
{
	LONG										lX;
	LONG										lY;
	LONG										lZ;
	BYTE										rgbButtons[4];
} D_DIMOUSESTATE ;

typedef struct tagD_DIMOUSESTATE2
{
	LONG										lX;
	LONG										lY;
	LONG										lZ;
	BYTE										rgbButtons[8];
} D_DIMOUSESTATE2 ;

typedef struct tagD_DIOBJECTDATAFORMAT
{
	const GUID *pguid;
	DWORD										dwOfs;
	DWORD										dwType;
	DWORD										dwFlags;
} D_DIOBJECTDATAFORMAT ;

typedef struct tagD_DIDATAFORMAT
{
	DWORD										dwSize;
	DWORD										dwObjSize;
	DWORD										dwFlags;
	DWORD										dwDataSize;
	DWORD										dwNumObjs;
	D_DIOBJECTDATAFORMAT						*rgodf;
} D_DIDATAFORMAT ;

typedef struct tagD_DIDEVCAPS
{
	DWORD										dwSize;
	DWORD										dwFlags;
	DWORD										dwDevType;
	DWORD										dwAxes;
	DWORD										dwButtons;
	DWORD										dwPOVs;
//	#if(DIRECTINPUT_VERSION >= 0x0500)
	DWORD										dwFFSamplePeriod;
	DWORD										dwFFMinTimeResolution;
	DWORD										dwFirmwareRevision;
	DWORD										dwHardwareRevision;
	DWORD										dwFFDriverVersion;
//	#endif
} D_DIDEVCAPS ;

typedef struct tagD_DIPERIODIC
{
	DWORD										dwMagnitude;
	LONG										lOffset;
	DWORD										dwPhase;
	DWORD										dwPeriod;
} D_DIPERIODIC ;

typedef struct tagD_DIENVELOPE
{
	DWORD										dwSize;
	DWORD										dwAttackLevel;
	DWORD										dwAttackTime;
	DWORD										dwFadeLevel;
	DWORD										dwFadeTime;
} D_DIENVELOPE;

typedef struct tagD_DIEFFECT
{
	DWORD										dwSize;
	DWORD										dwFlags;
	DWORD										dwDuration;
	DWORD										dwSamplePeriod;
	DWORD										dwGain;
	DWORD										dwTriggerButton;
	DWORD										dwTriggerRepeatInterval;
	DWORD										cAxes;
	DWORD										*rgdwAxes;
	LONG										*rglDirection;
	D_DIENVELOPE								*lpEnvelope;
	DWORD										cbTypeSpecificParams;
	LPVOID										lpvTypeSpecificParams;
//	#if(DIRECTINPUT_VERSION >= 0x0600)
	DWORD										dwStartDelay;
//	#endif
} D_DIEFFECT ;

typedef struct tagD_DIEFFECTINFOA
{
	DWORD										dwSize;
	GUID										guid;
	DWORD										dwEffType;
	DWORD										dwStaticParams;
	DWORD										dwDynamicParams;
	CHAR										tszName[MAX_PATH];
} D_DIEFFECTINFOA ;

typedef struct tagD_DIEFFECTINFOW
{
	DWORD										dwSize;
	GUID										guid;
	DWORD										dwEffType;
	DWORD										dwStaticParams;
	DWORD										dwDynamicParams;
	WCHAR										tszName[MAX_PATH];
} D_DIEFFECTINFOW ;

typedef struct tagD_DICONSTANTFORCE
{
    LONG										lMagnitude;
} D_DICONSTANTFORCE ;

typedef BOOL ( FAR PASCAL * LPD_DIENUMDEVICESCALLBACKA )( const D_DIDEVICEINSTANCEA *, LPVOID ) ;
typedef BOOL ( FAR PASCAL * LPD_DIENUMDEVICESCALLBACKW )( const D_DIDEVICEINSTANCEW *, LPVOID ) ;
typedef BOOL ( FAR PASCAL * LPD_DIENUMEFFECTSCALLBACKA)( const D_DIEFFECTINFOA *, LPVOID);
typedef BOOL ( FAR PASCAL * LPD_DIENUMEFFECTSCALLBACKW)( const D_DIEFFECTINFOW *, LPVOID);

class D_IDirectInputDevice : public D_IUnknown
{
public :
	virtual HRESULT __stdcall NonUse00					( void ) = 0 ;				// GetCapabilities( LPDIDEVCAPS ) = 0 ;
	virtual HRESULT __stdcall NonUse01					( void ) = 0 ;				// EnumObjects( LPDIENUMDEVICEOBJECTSCALLBACK, LPVOID,DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse02					( void ) = 0 ;				// GetProperty( REFGUID, LPDIPROPHEADER ) = 0 ;
	virtual HRESULT __stdcall SetProperty				( REFGUID, const D_DIPROPHEADER * ) = 0 ;
	virtual HRESULT __stdcall Acquire					( void ) = 0 ;
	virtual HRESULT __stdcall Unacquire					( void ) = 0 ;
	virtual HRESULT __stdcall GetDeviceState			( DWORD, LPVOID ) = 0 ;
	virtual HRESULT __stdcall GetDeviceData				( DWORD, void * /*D_DIDEVICEOBJECTDATA * */, LPDWORD, DWORD ) = 0 ;
	virtual HRESULT __stdcall SetDataFormat				( const D_DIDATAFORMAT * ) = 0 ;
	virtual HRESULT __stdcall SetEventNotification		( HANDLE ) = 0 ;
	virtual HRESULT __stdcall SetCooperativeLevel		( HWND, DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse03					( void ) = 0 ;				// GetObjectInfo( LPDIDEVICEOBJECTINSTANCE, DWORD, DWORD ) = 0 ;
	virtual HRESULT __stdcall GetDeviceInfo				( D_DIDEVICEINSTANCEW * ) = 0 ;
	virtual HRESULT __stdcall NonUse04					( void ) = 0 ;				// RunControlPanel( HWND, DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse05					( void ) = 0 ;				// Initialize( HINSTANCE, DWORD, REFGUID ) = 0 ;
};

class D_IDirectInputDevice2 : public D_IDirectInputDevice
{
public :
	virtual HRESULT __stdcall CreateEffect				( REFGUID, const D_DIEFFECT *, class D_IDirectInputEffect **, D_IUnknown * ) = 0 ;
	virtual HRESULT __stdcall EnumEffects				( LPD_DIENUMEFFECTSCALLBACKW, LPVOID,DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse08					( void ) = 0 ;				// GetEffectInfo( LPDIEFFECTINFOA,REFGUID ) = 0 ;
	virtual HRESULT __stdcall NonUse09					( void ) = 0 ;				// GetForceFeedbackState( LPDWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse10					( void ) = 0 ;				// SendForceFeedbackCommand( DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse11					( void ) = 0 ;				// EnumCreatedEffectObjects( LPDIENUMCREATEDEFFECTOBJECTSCALLBACK,LPVOID,DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse12					( void ) = 0 ;				// Escape( LPDIEFFESCAPE ) = 0 ;
	virtual HRESULT __stdcall Poll						( void ) = 0 ;
	virtual HRESULT __stdcall NonUse13					( void ) = 0 ;				// SendDeviceData( DWORD,LPCDIDEVICEOBJECTDATA,LPDWORD,DWORD ) = 0 ;
} ;

class D_IDirectInputDevice7 : public D_IDirectInputDevice2
{
public :
	virtual HRESULT __stdcall NonUse14					( void ) = 0 ;				// EnumEffectsInFile( LPCSTR,LPDIENUMEFFECTSINFILECALLBACK,LPVOID,DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse15					( void ) = 0 ;				// WriteEffectToFile( LPCSTR,DWORD,LPDIFILEEFFECT,DWORD ) = 0 ;
} ;

class D_IDirectInputDevice8 : public D_IDirectInputDevice7
{
public :
	virtual HRESULT __stdcall NonUse16					( void ) = 0 ;				// BuildActionMap( LPDIACTIONFORMATW,LPCWSTR,DWORD) = 0 ;
	virtual HRESULT __stdcall NonUse17					( void ) = 0 ;				// SetActionMap( LPDIACTIONFORMATW,LPCWSTR,DWORD) = 0 ;
	virtual HRESULT __stdcall NonUse18					( void ) = 0 ;				// GetImageInfo( LPDIDEVICEIMAGEINFOHEADERW) = 0 ;
};

class D_IDirectInput8 : public D_IUnknown
{
public :
	virtual HRESULT __stdcall CreateDevice				( REFGUID, D_IDirectInputDevice8 **, D_IUnknown * ) = 0 ;
	virtual HRESULT __stdcall EnumDevices				( DWORD, LPD_DIENUMDEVICESCALLBACKW, LPVOID, DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse01					( void ) = 0 ;				// GetDeviceStatus( REFGUID) = 0 ;
	virtual HRESULT __stdcall NonUse02					( void ) = 0 ;				// RunControlPanel( HWND,DWORD) = 0 ;
	virtual HRESULT __stdcall Initialize				( HINSTANCE, DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse03					( void ) = 0 ;				// FindDevice( REFGUID,LPCWSTR,LPGUID) = 0 ;
	virtual HRESULT __stdcall NonUse04					( void ) = 0 ;				// EnumDevicesBySemantics( LPCWSTR,LPDIACTIONFORMATW,LPDIENUMDEVICESBYSEMANTICSCBW,LPVOID,DWORD) = 0 ;
	virtual HRESULT __stdcall NonUse05					( void ) = 0 ;				// ConfigureDevices( LPDICONFIGUREDEVICESCALLBACK,LPDICONFIGUREDEVICESPARAMSW,DWORD,LPVOID) = 0 ;
};

class D_IDirectInput : public D_IUnknown
{
public :
	virtual HRESULT __stdcall CreateDevice				( REFGUID, D_IDirectInputDevice **, D_IUnknown * ) = 0 ;
	virtual HRESULT __stdcall EnumDevices				( DWORD, LPD_DIENUMDEVICESCALLBACKW, LPVOID, DWORD ) = 0 ;
	virtual HRESULT __stdcall NonUse01					( void ) = 0 ;				// GetDeviceStatus( REFGUID ) = 0 ;
	virtual HRESULT __stdcall NonUse02					( void ) = 0 ;				// RunControlPanel( HWND, DWORD ) = 0 ;
	virtual HRESULT __stdcall Initialize				( HINSTANCE, DWORD ) = 0 ;
};

class D_IDirectInput2 : public D_IDirectInput
{
public :
	virtual HRESULT __stdcall NonUse03					( void ) = 0 ;				// FindDevice( REFGUID, LPCSTR, LPGUID ) = 0 ;
} ;

class D_IDirectInput7 : public D_IDirectInput2
{
public :
	virtual HRESULT __stdcall CreateDeviceEx			( REFGUID, REFIID, LPVOID *, D_IUnknown * ) = 0 ;
} ;

class D_IDirectInputEffect : public D_IUnknown
{
public :
	virtual HRESULT __stdcall NonUse00					( void ) = 0 ;				// Initialize( HINSTANCE,DWORD,REFGUID ) = 0 ;
	virtual HRESULT __stdcall NonUse01					( void ) = 0 ;				// GetEffectGuid( LPGUID ) = 0 ;
	virtual HRESULT __stdcall NonUse02					( void ) = 0 ;				// GetParameters( LPDIEFFECT,DWORD ) = 0 ;
	virtual HRESULT __stdcall SetParameters				( const D_DIEFFECT *, DWORD ) = 0 ;
	virtual HRESULT __stdcall Start						( DWORD,DWORD ) = 0 ;
	virtual HRESULT __stdcall Stop						( void ) = 0 ;
	virtual HRESULT __stdcall GetEffectStatus			( LPDWORD ) = 0 ;
	virtual HRESULT __stdcall Download					( void ) = 0 ;
	virtual HRESULT __stdcall Unload					( void ) = 0 ;
	virtual HRESULT __stdcall NonUse03					( void ) = 0 ;				// Escape( LPDIEFFESCAPE ) = 0 ;
} ;

// Text Services Framework ( TSF ) -----------------------------------------------------

#define D_TfClientId		DWORD

#define D_TF_TMAE_NOACTIVATETIP					(0x00000001)
#define D_TF_TMAE_SECUREMODE					(0x00000002)
#define D_TF_TMAE_UIELEMENTENABLEDONLY			(0x00000004)
#define D_TF_TMAE_COMLESS						(0x00000008)
#define D_TF_TMAE_WOW16							(0x00000010)
#define D_TF_TMAE_NOACTIVATEKEYBOARDLAYOUT		(0x00000020)
#define D_TF_TMAE_CONSOLE						(0x00000040)

class D_ITfSource : public D_IUnknown
{
public:
	virtual HRESULT __stdcall AdviseSink				( REFIID riid, D_IUnknown *punk, DWORD *pdwCookie ) = 0 ;
	virtual HRESULT __stdcall UnadviseSink				( DWORD dwCookie ) = 0 ;
} ;

class D_ITfSourceSingle : public D_IUnknown
{
public:
	virtual HRESULT __stdcall AdviseSingleSink			( D_TfClientId tid, REFIID riid, D_IUnknown *punk ) = 0 ;
	virtual HRESULT __stdcall UnadviseSingleSink		( D_TfClientId tid, REFIID riid ) = 0 ;
} ;

class D_ITfUIElementSink : public D_IUnknown
{
public:
	virtual HRESULT __stdcall BeginUIElement			( DWORD dwUIElementId, BOOL *pbShow ) = 0 ;
	virtual HRESULT __stdcall UpdateUIElement			( DWORD dwUIElementId ) = 0 ;
	virtual HRESULT __stdcall EndUIElement				( DWORD dwUIElementId ) = 0 ;
} ;

class D_ITfInputProcessorProfileActivationSink : public D_IUnknown
{
public:
	virtual HRESULT __stdcall OnActivated				( DWORD dwProfileType, LANGID langid, REFCLSID clsid, REFGUID catid, REFGUID guidProfile, HKL hkl, DWORD dwFlags ) = 0 ;
} ;

class D_ITfCompartmentEventSink : public D_IUnknown
{
public:
	virtual HRESULT __stdcall OnChange					( REFGUID rguid ) = 0 ;
} ;

class D_ITfUIElement : public D_IUnknown
{
public:
	virtual HRESULT __stdcall GetDescription			( BSTR *pbstrDescription ) = 0 ;
	virtual HRESULT __stdcall GetGUID					( GUID *pguid ) = 0 ;
	virtual HRESULT __stdcall Show						( BOOL bShow ) = 0 ;
	virtual HRESULT __stdcall IsShown					( BOOL *pbShow ) = 0 ;
} ;

class D_ITfReadingInformationUIElement : public D_ITfUIElement
{
public:
	virtual HRESULT __stdcall GetUpdatedFlags			( DWORD *pdwFlags ) = 0 ;
	virtual HRESULT __stdcall NonUse00					( void ) = 0 ;				// GetContext( ITfContext **ppic) = 0;
	virtual HRESULT __stdcall GetString					( BSTR *pstr ) = 0 ;
	virtual HRESULT __stdcall GetMaxReadingStringLength( UINT *pcchMax ) = 0 ;
	virtual HRESULT __stdcall GetErrorIndex				( UINT *pErrorIndex ) = 0 ;
	virtual HRESULT __stdcall IsVerticalOrderPreferred	( BOOL *pfVertical ) = 0 ;
} ;

class D_ITfCandidateListUIElement : public D_ITfUIElement
{
public:
	virtual HRESULT __stdcall GetUpdatedFlags			( DWORD *pdwFlags ) = 0 ;
	virtual HRESULT __stdcall GetDocumentMgr			( class D_ITfDocumentMgr **ppdim ) = 0 ;
	virtual HRESULT __stdcall GetCount					( UINT *puCount ) = 0 ;
	virtual HRESULT __stdcall GetSelection				( UINT *puIndex ) = 0 ;
	virtual HRESULT __stdcall GetString					( UINT uIndex, BSTR *pstr ) = 0 ;
	virtual HRESULT __stdcall GetPageIndex				( UINT *pIndex, UINT uSize, UINT *puPageCnt ) = 0 ;
	virtual HRESULT __stdcall SetPageIndex				( UINT *pIndex, UINT uPageCnt ) = 0 ;
	virtual HRESULT __stdcall GetCurrentPage			( UINT *puPage ) = 0 ;
} ;

class D_ITfUIElementMgr : public D_IUnknown
{
public:
	virtual HRESULT __stdcall BeginUIElement			( D_ITfUIElement *pElement, BOOL *pbShow, DWORD *pdwUIElementId ) = 0 ;
	virtual HRESULT __stdcall UpdateUIElement			( DWORD dwUIElementId ) = 0 ;
	virtual HRESULT __stdcall EndUIElement				( DWORD dwUIElementId ) = 0 ;
	virtual HRESULT __stdcall GetUIElement				( DWORD dwUIELementId, D_ITfUIElement **ppElement ) = 0 ;
	virtual HRESULT __stdcall NonUse00					( void ) = 0 ;				// EnumUIElements( IEnumTfUIElements **ppEnum ) = 0 ;
} ;

class D_ITfDocumentMgr : public D_IUnknown
{
public:
	virtual HRESULT __stdcall NonUse00					( void ) = 0 ;				// CreateContext( D_TfClientId tidOwner, DWORD dwFlags, D_IUnknown *punk, ITfContext **ppic, TfEditCookie *pecTextStore ) = 0 ;
	virtual HRESULT __stdcall NonUse01					( void ) = 0 ;				// Push( ITfContext *pic ) = 0 ;
	virtual HRESULT __stdcall NonUse02					( void ) = 0 ;				// Pop( DWORD dwFlags ) = 0 ;
	virtual HRESULT __stdcall NonUse03					( void ) = 0 ;				// GetTop( ITfContext **ppic ) = 0 ;
	virtual HRESULT __stdcall NonUse04					( void ) = 0 ;				// GetBase( ITfContext **ppic ) = 0 ;
	virtual HRESULT __stdcall NonUse05					( void ) = 0 ;				// EnumContexts( IEnumTfContexts **ppEnum ) = 0 ;
} ;

class D_ITfCompartment : public D_IUnknown
{
public:
	virtual HRESULT __stdcall SetValue					( D_TfClientId tid, const VARIANT *pvarValue ) = 0 ;
	virtual HRESULT __stdcall GetValue					( VARIANT *pvarValue ) = 0 ;
} ;

class D_ITfCompartmentMgr : public D_IUnknown
{
public:
	virtual HRESULT __stdcall GetCompartment			( REFGUID rguid, D_ITfCompartment **ppcomp ) = 0 ;
	virtual HRESULT __stdcall ClearCompartment			( D_TfClientId tid, REFGUID rguid ) = 0 ;
	virtual HRESULT __stdcall NonUse00					( void ) = 0 ;				// EnumCompartments( IEnumGUID **ppEnum ) = 0 ;
} ;

class D_ITfThreadMgr : public D_IUnknown
{
public:
	virtual HRESULT __stdcall Activate					( D_TfClientId *ptid ) = 0 ;
	virtual HRESULT __stdcall Deactivate				( void ) = 0 ;
	virtual HRESULT __stdcall CreateDocumentMgr			( D_ITfDocumentMgr **ppdim ) = 0 ;
	virtual HRESULT __stdcall NonUse00					( void ) = 0 ;				// EnumDocumentMgrs( IEnumTfDocumentMgrs **ppEnum ) = 0 ;
	virtual HRESULT __stdcall GetFocus					( D_ITfDocumentMgr **ppdimFocus ) = 0 ;
	virtual HRESULT __stdcall SetFocus					( D_ITfDocumentMgr *pdimFocus ) = 0 ;
	virtual HRESULT __stdcall AssociateFocus			( HWND hwnd, D_ITfDocumentMgr *pdimNew, D_ITfDocumentMgr **ppdimPrev ) = 0 ;
	virtual HRESULT __stdcall IsThreadFocus				( BOOL *pfThreadFocus ) = 0 ;
	virtual HRESULT __stdcall NonUse01					( void ) = 0 ;				// GetFunctionProvider( REFCLSID clsid, ITfFunctionProvider **ppFuncProv ) = 0 ;
	virtual HRESULT __stdcall NonUse02					( void ) = 0 ;				// EnumFunctionProviders( IEnumTfFunctionProviders **ppEnum ) = 0 ;
	virtual HRESULT __stdcall GetGlobalCompartment		( D_ITfCompartmentMgr **ppCompMgr ) = 0 ;
} ;

class D_ITfThreadMgrEx : public D_ITfThreadMgr
{
public:
	virtual HRESULT __stdcall ActivateEx				( D_TfClientId *ptid, DWORD dwFlags ) = 0 ;
	virtual HRESULT __stdcall GetActiveFlags			( DWORD *lpdwFlags ) = 0 ;
} ;

// Media Foundation -----------------------------------------------------

#define D_MF_SDK_VERSION						(0x0002)
#define D_MF_API_VERSION						(0x0070)
#define D_MF_VERSION							( D_MF_SDK_VERSION << 16 | D_MF_API_VERSION )

#define D_MFSTARTUP_NOSOCKET					(0x1)
#define D_MFSTARTUP_LITE						(D_MFSTARTUP_NOSOCKET)
#define D_MFSTARTUP_FULL						(0)

#define D_MFBYTESTREAM_IS_READABLE				(0x00000001)
#define D_MFBYTESTREAM_IS_WRITABLE				(0x00000002)
#define D_MFBYTESTREAM_IS_SEEKABLE				(0x00000004)
#define D_MFBYTESTREAM_IS_REMOTE				(0x00000008)
#define D_MFBYTESTREAM_IS_DIRECTORY				(0x00000080)
#define D_MFBYTESTREAM_HAS_SLOW_SEEK			(0x00000100)
#define D_MFBYTESTREAM_IS_PARTIALLY_DOWNLOADED	(0x00000200)

enum D_VARENUM
{
	D_VT_EMPTY									= 0,
	D_VT_NULL									= 1,
	D_VT_I2										= 2,
	D_VT_I4										= 3,
	D_VT_R4										= 4,
	D_VT_R8										= 5,
	D_VT_CY										= 6,
	D_VT_DATE									= 7,
	D_VT_BSTR									= 8,
	D_VT_DISPATCH								= 9,
	D_VT_ERROR									= 10,
	D_VT_BOOL									= 11,
	D_VT_VARIANT								= 12,
	D_VT_UNKNOWN								= 13,
	D_VT_DECIMAL								= 14,
	D_VT_I1										= 16,
	D_VT_UI1									= 17,
	D_VT_UI2									= 18,
	D_VT_UI4									= 19,
	D_VT_I8										= 20,
	D_VT_UI8									= 21,
	D_VT_INT									= 22,
	D_VT_UINT									= 23,
	D_VT_VOID									= 24,
	D_VT_HRESULT								= 25,
	D_VT_PTR									= 26,
	D_VT_SAFEARRAY								= 27,
	D_VT_CARRAY									= 28,
	D_VT_USERDEFINED							= 29,
	D_VT_LPSTR									= 30,
	D_VT_LPWSTR									= 31,
	D_VT_RECORD									= 36,
	D_VT_INT_PTR								= 37,
	D_VT_UINT_PTR								= 38,
	D_VT_FILETIME								= 64,
	D_VT_BLOB									= 65,
	D_VT_STREAM									= 66,
	D_VT_STORAGE								= 67,
	D_VT_STREAMED_OBJECT						= 68,
	D_VT_STORED_OBJECT							= 69,
	D_VT_BLOB_OBJECT							= 70,
	D_VT_CF										= 71,
	D_VT_CLSID									= 72,
	D_VT_VERSIONED_STREAM						= 73,
	D_VT_BSTR_BLOB								= 0xfff,
	D_VT_VECTOR									= 0x1000,
	D_VT_ARRAY									= 0x2000,
	D_VT_BYREF									= 0x4000,
	D_VT_RESERVED								= 0x8000,
	D_VT_ILLEGAL								= 0xffff,
	D_VT_ILLEGALMASKED							= 0xfff,
	D_VT_TYPEMASK								= 0xfff
} ;

enum D_MF_ATTRIBUTE_TYPE
{
	D_MF_ATTRIBUTE_UINT32						= D_VT_UI4,
	D_MF_ATTRIBUTE_UINT64						= D_VT_UI8,
	D_MF_ATTRIBUTE_DOUBLE						= D_VT_R8,
	D_MF_ATTRIBUTE_GUID							= D_VT_CLSID,
	D_MF_ATTRIBUTE_STRING						= D_VT_LPWSTR,
	D_MF_ATTRIBUTE_BLOB							= ( D_VT_VECTOR | D_VT_UI1 ) ,
	D_MF_ATTRIBUTE_IUNKNOWN						= D_VT_UNKNOWN
} ;

enum D_MF_ATTRIBUTES_MATCH_TYPE
{
	D_MF_ATTRIBUTES_MATCH_OUR_ITEMS				= 0,
	D_MF_ATTRIBUTES_MATCH_THEIR_ITEMS			= 1,
	D_MF_ATTRIBUTES_MATCH_ALL_ITEMS				= 2,
	D_MF_ATTRIBUTES_MATCH_INTERSECTION			= 3,
	D_MF_ATTRIBUTES_MATCH_SMALLER				= 4
} ;

enum D__MFVideoInterlaceMode
{
	D_MFVideoInterlace_Unknown						= 0,
	D_MFVideoInterlace_Progressive					= 2,
	D_MFVideoInterlace_FieldInterleavedUpperFirst	= 3,
	D_MFVideoInterlace_FieldInterleavedLowerFirst	= 4,
	D_MFVideoInterlace_FieldSingleUpper				= 5,
	D_MFVideoInterlace_FieldSingleLower				= 6,
	D_MFVideoInterlace_MixedInterlaceOrProgressive	= 7,
	D_MFVideoInterlace_Last							= ( D_MFVideoInterlace_MixedInterlaceOrProgressive + 1 ) ,
	D_MFVideoInterlace_ForceDWORD					= 0x7fffffff
} ;

enum D_MFWaveFormatExConvertFlags
{
	D_MFWaveFormatExConvertFlag_Normal			= 0,
	D_MFWaveFormatExConvertFlag_ForceExtensible	= 1
} ;

enum D_MF_SOURCE_READER_FLAG
{
	D_MF_SOURCE_READERF_ERROR					= 0x1,
	D_MF_SOURCE_READERF_ENDOFSTREAM				= 0x2,
	D_MF_SOURCE_READERF_NEWSTREAM				= 0x4,
	D_MF_SOURCE_READERF_NATIVEMEDIATYPECHANGED	= 0x10,
	D_MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED	= 0x20,
	D_MF_SOURCE_READERF_STREAMTICK				= 0x100
} ;

enum D_MF_SOURCE_READER_CONTROL_FLAG
{
	D_MF_SOURCE_READER_CONTROLF_DRAIN			= 0x1
} ;

enum D___MIDL___MIDL_itf_mfreadwrite_0000_0001_0001
{
	D_MF_SOURCE_READER_INVALID_STREAM_INDEX		= 0xffffffff,
	D_MF_SOURCE_READER_ALL_STREAMS				= 0xfffffffe,
	D_MF_SOURCE_READER_ANY_STREAM				= 0xfffffffe,
	D_MF_SOURCE_READER_FIRST_AUDIO_STREAM		= 0xfffffffd,
	D_MF_SOURCE_READER_FIRST_VIDEO_STREAM		= 0xfffffffc,
	D_MF_SOURCE_READER_MEDIASOURCE				= 0xffffffff
} ;

enum D_MFT_MESSAGE_TYPE
{
	D_MFT_MESSAGE_COMMAND_FLUSH					= 0,
	D_MFT_MESSAGE_COMMAND_DRAIN					= 0x1,
	D_MFT_MESSAGE_SET_D3D_MANAGER				= 0x2,
	D_MFT_MESSAGE_DROP_SAMPLES					= 0x3,
	D_MFT_MESSAGE_NOTIFY_BEGIN_STREAMING		= 0x10000000,
	D_MFT_MESSAGE_NOTIFY_END_STREAMING			= 0x10000001,
	D_MFT_MESSAGE_NOTIFY_END_OF_STREAM			= 0x10000002,
	D_MFT_MESSAGE_NOTIFY_START_OF_STREAM		= 0x10000003,
	D_MFT_MESSAGE_COMMAND_MARKER				= 0x20000000
} ;

enum D_MFVIDEOFORMAT_TYPE
{
	D_MFVIDEOFORMAT_TYPE_BASE 					= 0,
	D_MFVIDEOFORMAT_TYPE_RGB32 					= 1,
	D_MFVIDEOFORMAT_TYPE_ARGB32 				= 2,
	D_MFVIDEOFORMAT_TYPE_RGB24 					= 3,
	D_MFVIDEOFORMAT_TYPE_RGB555 				= 4,
	D_MFVIDEOFORMAT_TYPE_RGB565 				= 5,
	D_MFVIDEOFORMAT_TYPE_RGB8 					= 6,
	D_MFVIDEOFORMAT_TYPE_AI44 					= 7,
	D_MFVIDEOFORMAT_TYPE_AYUV 					= 8,
	D_MFVIDEOFORMAT_TYPE_YUY2 					= 9,
	D_MFVIDEOFORMAT_TYPE_YVYU 					= 10,
	D_MFVIDEOFORMAT_TYPE_YVU9 					= 11,
	D_MFVIDEOFORMAT_TYPE_UYVY 					= 12,
	D_MFVIDEOFORMAT_TYPE_NV11 					= 13,
	D_MFVIDEOFORMAT_TYPE_NV12 					= 14,
	D_MFVIDEOFORMAT_TYPE_YV12 					= 15,
	D_MFVIDEOFORMAT_TYPE_I420 					= 16,
	D_MFVIDEOFORMAT_TYPE_IYUV 					= 17,
	D_MFVIDEOFORMAT_TYPE_Y210 					= 18,
	D_MFVIDEOFORMAT_TYPE_Y216 					= 19,
	D_MFVIDEOFORMAT_TYPE_Y410 					= 20,
	D_MFVIDEOFORMAT_TYPE_Y416 					= 21,
	D_MFVIDEOFORMAT_TYPE_Y41P 					= 22,
	D_MFVIDEOFORMAT_TYPE_Y41T 					= 23,
	D_MFVIDEOFORMAT_TYPE_Y42T 					= 24,
	D_MFVIDEOFORMAT_TYPE_P210 					= 25,
	D_MFVIDEOFORMAT_TYPE_P216 					= 26,
	D_MFVIDEOFORMAT_TYPE_P010 					= 27,
	D_MFVIDEOFORMAT_TYPE_P016 					= 28,
	D_MFVIDEOFORMAT_TYPE_v210 					= 29,
	D_MFVIDEOFORMAT_TYPE_v216 					= 30,
	D_MFVIDEOFORMAT_TYPE_v410 					= 31,
	D_MFVIDEOFORMAT_TYPE_MP43 					= 32,
	D_MFVIDEOFORMAT_TYPE_MP4S 					= 33,
	D_MFVIDEOFORMAT_TYPE_M4S2 					= 34,
	D_MFVIDEOFORMAT_TYPE_MP4V 					= 35,
	D_MFVIDEOFORMAT_TYPE_WMV1 					= 36,
	D_MFVIDEOFORMAT_TYPE_WMV2 					= 37,
	D_MFVIDEOFORMAT_TYPE_WMV3 					= 38,
	D_MFVIDEOFORMAT_TYPE_WVC1 					= 39,
	D_MFVIDEOFORMAT_TYPE_MSS1 					= 40,
	D_MFVIDEOFORMAT_TYPE_MSS2 					= 41,
	D_MFVIDEOFORMAT_TYPE_MPG1 					= 42,
	D_MFVIDEOFORMAT_TYPE_DVSL 					= 43,
	D_MFVIDEOFORMAT_TYPE_DVSD 					= 44,
	D_MFVIDEOFORMAT_TYPE_DVHD 					= 45,
	D_MFVIDEOFORMAT_TYPE_DV25 					= 46,
	D_MFVIDEOFORMAT_TYPE_DV50 					= 47,
	D_MFVIDEOFORMAT_TYPE_DVH1 					= 48,
	D_MFVIDEOFORMAT_TYPE_DVC					= 49,
	D_MFVIDEOFORMAT_TYPE_H264 					= 50,
	D_MFVIDEOFORMAT_TYPE_MJPG 					= 51,

	D_MFVIDEOFORMAT_TYPE_UNKNOWN				= 52,
} ;

enum D_MFBYTESTREAM_SEEK_ORIGIN
{
	D_msoBegin									= 0,
	D_msoCurrent								= ( D_msoBegin + 1 )
} ;

struct D_MFT_INPUT_STREAM_INFO
{
	LONGLONG									hnsMaxLatency ;
	DWORD										dwFlags ;
	DWORD										cbSize ;
	DWORD										cbMaxLookahead ;
	DWORD										cbAlignment ;
} ;

struct D_MFT_OUTPUT_STREAM_INFO
{
	DWORD										dwFlags ;
	DWORD										cbSize ;
	DWORD										cbAlignment ;
} ;

struct D_MFT_OUTPUT_DATA_BUFFER
{
	DWORD										dwStreamID ;
	class D_IMFSample							*pSample ;
	DWORD										dwStatus ;
	class D_IMFCollection						*pEvents ;
} ;

typedef DWORD D_MediaEventType ;

class D_IMFAttributes : public D_IUnknown
{
public:
	virtual HRESULT __stdcall GetItem					( REFGUID guidKey, D_PROPVARIANT *pValue ) = 0 ;
	virtual HRESULT __stdcall GetItemType				( REFGUID guidKey, D_MF_ATTRIBUTE_TYPE *pType ) = 0 ;
	virtual HRESULT __stdcall CompareItem				( REFGUID guidKey, const D_PROPVARIANT &Value, BOOL *pbResult ) = 0 ;
	virtual HRESULT __stdcall Compare					( D_IMFAttributes *pTheirs, D_MF_ATTRIBUTES_MATCH_TYPE MatchType, BOOL *pbResult ) = 0 ;
	virtual HRESULT __stdcall GetUINT32					( REFGUID guidKey, UINT32 *punValue ) = 0 ;
	virtual HRESULT __stdcall GetUINT64					( REFGUID guidKey, UINT64 *punValue ) = 0 ;
	virtual HRESULT __stdcall GetDouble					( REFGUID guidKey, double *pfValue ) = 0 ;
	virtual HRESULT __stdcall GetGUID					( REFGUID guidKey, GUID *pguidValue ) = 0 ;
	virtual HRESULT __stdcall GetStringLength			( REFGUID guidKey, UINT32 *pcchLength ) = 0 ;
	virtual HRESULT __stdcall GetString					( REFGUID guidKey, LPWSTR pwszValue, UINT32 cchBufSize, UINT32 *pcchLength ) = 0 ;
	virtual HRESULT __stdcall GetAllocatedString		( REFGUID guidKey, LPWSTR *ppwszValue, UINT32 *pcchLength ) = 0 ;
	virtual HRESULT __stdcall GetBlobSize				( REFGUID guidKey, UINT32 *pcbBlobSize ) = 0 ;
	virtual HRESULT __stdcall GetBlob					( REFGUID guidKey, D_UINT8 *pBuf, UINT32 cbBufSize, UINT32 *pcbBlobSize ) = 0 ;
	virtual HRESULT __stdcall GetAllocatedBlob			( REFGUID guidKey, D_UINT8 **ppBuf, UINT32 *pcbSize ) = 0 ;
	virtual HRESULT __stdcall GetUnknown				( REFGUID guidKey, REFIID riid, LPVOID *ppv ) = 0 ;
	virtual HRESULT __stdcall SetItem					( REFGUID guidKey, const D_PROPVARIANT &Value ) = 0 ;
	virtual HRESULT __stdcall DeleteItem				( REFGUID guidKey ) = 0 ;
	virtual HRESULT __stdcall DeleteAllItems			( void ) = 0 ;
	virtual HRESULT __stdcall SetUINT32					( REFGUID guidKey, UINT32 unValue ) = 0 ;
	virtual HRESULT __stdcall SetUINT64					( REFGUID guidKey, UINT64 unValue ) = 0 ;
	virtual HRESULT __stdcall SetDouble					( REFGUID guidKey, double fValue ) = 0 ;
	virtual HRESULT __stdcall SetGUID					( REFGUID guidKey, REFGUID guidValue ) = 0 ;
	virtual HRESULT __stdcall SetString					( REFGUID guidKey, LPCWSTR wszValue ) = 0 ;
	virtual HRESULT __stdcall SetBlob					( REFGUID guidKey, const D_UINT8 *pBuf, UINT32 cbBufSize ) = 0 ;
	virtual HRESULT __stdcall SetUnknown				( REFGUID guidKey, D_IUnknown *pUnknown ) = 0 ;
	virtual HRESULT __stdcall LockStore					( void ) = 0 ;
	virtual HRESULT __stdcall UnlockStore				( void ) = 0 ;
	virtual HRESULT __stdcall GetCount					( UINT32 *pcItems ) = 0 ;
	virtual HRESULT __stdcall GetItemByIndex			( UINT32 unIndex, GUID *pguidKey, D_PROPVARIANT *pValue ) = 0 ;
	virtual HRESULT __stdcall CopyAllItems				( D_IMFAttributes *pDest ) = 0 ;
} ;

class D_IMFMediaType : public D_IMFAttributes
{
public:
	virtual HRESULT __stdcall GetMajorType				( GUID *pguidMajorType ) = 0 ;
	virtual HRESULT __stdcall IsCompressedFormat		( BOOL *pfCompressed ) = 0 ;
	virtual HRESULT __stdcall IsEqual					( D_IMFMediaType *pIMediaType, DWORD *pdwFlags ) = 0 ;
	virtual HRESULT __stdcall GetRepresentation			( GUID guidRepresentation, LPVOID *ppvRepresentation ) = 0 ;
	virtual HRESULT __stdcall FreeRepresentation		( GUID guidRepresentation, LPVOID pvRepresentation ) = 0 ;
} ;

class D_IMFMediaBuffer : public D_IUnknown
{
public:
	virtual HRESULT __stdcall Lock						( BYTE **ppbBuffer, DWORD *pcbMaxLength, DWORD *pcbCurrentLength ) = 0 ;
	virtual HRESULT __stdcall Unlock					( void ) = 0 ;
	virtual HRESULT __stdcall GetCurrentLength			( DWORD *pcbCurrentLength ) = 0 ;
	virtual HRESULT __stdcall SetCurrentLength			( DWORD cbCurrentLength ) = 0 ;
	virtual HRESULT __stdcall GetMaxLength				( DWORD *pcbMaxLength ) = 0 ;
} ;

class D_IMFSample : public D_IMFAttributes
{
public:
	virtual HRESULT __stdcall GetSampleFlags			( DWORD *pdwSampleFlags ) = 0 ;
	virtual HRESULT __stdcall SetSampleFlags			( DWORD dwSampleFlags ) = 0 ;
	virtual HRESULT __stdcall GetSampleTime				( LONGLONG *phnsSampleTime ) = 0 ;
	virtual HRESULT __stdcall SetSampleTime				( LONGLONG hnsSampleTime ) = 0 ;
	virtual HRESULT __stdcall GetSampleDuration			( LONGLONG *phnsSampleDuration ) = 0 ;
	virtual HRESULT __stdcall SetSampleDuration			( LONGLONG hnsSampleDuration ) = 0 ;
	virtual HRESULT __stdcall GetBufferCount			( DWORD *pdwBufferCount ) = 0 ;
	virtual HRESULT __stdcall GetBufferByIndex			( DWORD dwIndex, D_IMFMediaBuffer **ppBuffer ) = 0 ;
	virtual HRESULT __stdcall ConvertToContiguousBuffer	( D_IMFMediaBuffer **ppBuffer ) = 0 ;
	virtual HRESULT __stdcall AddBuffer					( D_IMFMediaBuffer *pBuffer ) = 0 ;
	virtual HRESULT __stdcall RemoveBufferByIndex		( DWORD dwIndex ) = 0 ;
	virtual HRESULT __stdcall RemoveAllBuffers			( void ) = 0 ;
	virtual HRESULT __stdcall GetTotalLength			( DWORD *pcbTotalLength ) = 0 ;
	virtual HRESULT __stdcall CopyToBuffer				( D_IMFMediaBuffer *pBuffer ) = 0 ;
} ;

class D_IMFMediaEvent : public D_IMFAttributes
{
public:
	virtual HRESULT __stdcall GetType					( D_MediaEventType *pmet ) = 0 ;
	virtual HRESULT __stdcall GetExtendedType			( GUID *pguidExtendedType ) = 0 ;
	virtual HRESULT __stdcall GetStatus					( HRESULT *phrStatus ) = 0 ;
	virtual HRESULT __stdcall GetValue					( D_PROPVARIANT *pvValue ) = 0 ;
} ;

class D_IMFSourceReader : public D_IUnknown
{
public:
	virtual HRESULT __stdcall GetStreamSelection		( DWORD dwStreamIndex, BOOL *pfSelected ) = 0 ;
	virtual HRESULT __stdcall SetStreamSelection		( DWORD dwStreamIndex, BOOL fSelected ) = 0 ;
	virtual HRESULT __stdcall GetNativeMediaType		( DWORD dwStreamIndex, DWORD dwMediaTypeIndex, D_IMFMediaType **ppMediaType ) = 0 ;
	virtual HRESULT __stdcall GetCurrentMediaType		( DWORD dwStreamIndex, D_IMFMediaType **ppMediaType ) = 0 ;
	virtual HRESULT __stdcall SetCurrentMediaType		( DWORD dwStreamIndex, DWORD *pdwReserved, D_IMFMediaType *pMediaType ) = 0 ;
	virtual HRESULT __stdcall SetCurrentPosition		( REFGUID guidTimeFormat, const D_PROPVARIANT &varPosition ) = 0 ;
	virtual HRESULT __stdcall ReadSample				( DWORD dwStreamIndex, DWORD dwControlFlags, DWORD *pdwActualStreamIndex, DWORD *pdwStreamFlags, LONGLONG *pllTimestamp, D_IMFSample **ppSample ) = 0 ;
	virtual HRESULT __stdcall Flush						( DWORD dwStreamIndex ) = 0 ;
	virtual HRESULT __stdcall GetServiceForStream		( DWORD dwStreamIndex, REFGUID guidService, REFIID riid, LPVOID *ppvObject ) = 0 ;
	virtual HRESULT __stdcall GetPresentationAttribute	( DWORD dwStreamIndex, REFGUID guidAttribute, D_PROPVARIANT *pvarAttribute ) = 0 ;
} ;

class D_IMFCollection : public D_IUnknown
{
public:
	virtual HRESULT __stdcall GetElementCount			( DWORD *pcElements ) = 0 ;
	virtual HRESULT __stdcall GetElement				( DWORD dwElementIndex, D_IUnknown **ppUnkElement ) = 0 ;
	virtual HRESULT __stdcall AddElement				( D_IUnknown *pUnkElement ) = 0 ;
	virtual HRESULT __stdcall RemoveElement				( DWORD dwElementIndex, D_IUnknown **ppUnkElement ) = 0 ;
	virtual HRESULT __stdcall InsertElementAt			( DWORD dwIndex, D_IUnknown *pUnknown ) = 0 ;
	virtual HRESULT __stdcall RemoveAllElements			( void ) = 0 ;
} ;

class D_IMFTransform : public D_IUnknown
{
public:
	virtual HRESULT __stdcall GetStreamLimits			( DWORD *pdwInputMinimum, DWORD *pdwInputMaximum, DWORD *pdwOutputMinimum, DWORD *pdwOutputMaximum ) = 0 ;
	virtual HRESULT __stdcall GetStreamCount			( DWORD *pcInputStreams, DWORD *pcOutputStreams ) = 0 ;
	virtual HRESULT __stdcall GetStreamIDs				( DWORD dwInputIDArraySize, DWORD *pdwInputIDs, DWORD dwOutputIDArraySize, DWORD *pdwOutputIDs ) = 0 ;
	virtual HRESULT __stdcall GetInputStreamInfo		( DWORD dwInputStreamID, D_MFT_INPUT_STREAM_INFO *pStreamInfo ) = 0 ;
	virtual HRESULT __stdcall GetOutputStreamInfo		( DWORD dwOutputStreamID, D_MFT_OUTPUT_STREAM_INFO *pStreamInfo ) = 0 ;
	virtual HRESULT __stdcall GetAttributes				( D_IMFAttributes **pAttributes ) = 0 ;
	virtual HRESULT __stdcall GetInputStreamAttributes	( DWORD dwInputStreamID, D_IMFAttributes **pAttributes ) = 0 ;
	virtual HRESULT __stdcall GetOutputStreamAttributes	( DWORD dwOutputStreamID, D_IMFAttributes **pAttributes ) = 0 ;
	virtual HRESULT __stdcall DeleteInputStream			( DWORD dwStreamID ) = 0 ;
	virtual HRESULT __stdcall AddInputStreams			( DWORD cStreams, DWORD *adwStreamIDs ) = 0 ;
	virtual HRESULT __stdcall GetInputAvailableType		( DWORD dwInputStreamID, DWORD dwTypeIndex, D_IMFMediaType **ppType ) = 0 ;
	virtual HRESULT __stdcall GetOutputAvailableType	( DWORD dwOutputStreamID, DWORD dwTypeIndex, D_IMFMediaType **ppType ) = 0 ;
	virtual HRESULT __stdcall SetInputType				( DWORD dwInputStreamID, D_IMFMediaType *pType, DWORD dwFlags ) = 0 ;
	virtual HRESULT __stdcall SetOutputType				( DWORD dwOutputStreamID, D_IMFMediaType *pType, DWORD dwFlags ) = 0 ;
	virtual HRESULT __stdcall GetInputCurrentType		( DWORD dwInputStreamID, D_IMFMediaType **ppType ) = 0 ;
	virtual HRESULT __stdcall GetOutputCurrentType		( DWORD dwOutputStreamID, D_IMFMediaType **ppType ) = 0 ;
	virtual HRESULT __stdcall GetInputStatus			( DWORD dwInputStreamID, DWORD *pdwFlags ) = 0 ;
	virtual HRESULT __stdcall GetOutputStatus			( DWORD *pdwFlags ) = 0 ;
	virtual HRESULT __stdcall SetOutputBounds			( LONGLONG hnsLowerBound, LONGLONG hnsUpperBound ) = 0 ;
	virtual HRESULT __stdcall ProcessEvent				( DWORD dwInputStreamID, D_IMFMediaEvent *pEvent ) = 0 ;
	virtual HRESULT __stdcall ProcessMessage			( D_MFT_MESSAGE_TYPE eMessage, DWORD_PTR ulParam ) = 0 ;
	virtual HRESULT __stdcall ProcessInput				( DWORD dwInputStreamID, D_IMFSample *pSample, DWORD dwFlags ) = 0 ;
	virtual HRESULT __stdcall ProcessOutput				( DWORD dwFlags, DWORD cOutputBufferCount, D_MFT_OUTPUT_DATA_BUFFER *pOutputSamples, DWORD *pdwStatus ) = 0 ;
} ;

class D_IMF2DBuffer : public D_IUnknown
{
public:
	virtual HRESULT __stdcall Lock2D					( BYTE **pbScanline0, LONG *plPitch ) = 0 ;
	virtual HRESULT __stdcall Unlock2D					( void ) = 0 ;
	virtual HRESULT __stdcall GetScanline0AndPitch		( BYTE **pbScanline0, LONG *plPitch ) = 0 ;
	virtual HRESULT __stdcall IsContiguousFormat		( BOOL *pfIsContiguous ) = 0 ;
	virtual HRESULT __stdcall GetContiguousLength		( DWORD *pcbLength ) = 0 ;
	virtual HRESULT __stdcall ContiguousCopyTo			( BYTE *pbDestBuffer, DWORD cbDestBuffer ) = 0 ;
	virtual HRESULT __stdcall ContiguousCopyFrom		( const BYTE *pbSrcBuffer, DWORD cbSrcBuffer ) = 0 ;
} ;

class D_IMFAsyncResult : public D_IUnknown
{
public:
	virtual HRESULT __stdcall GetState					( D_IUnknown **ppunkState ) = 0 ;
	virtual HRESULT __stdcall GetStatus					( void ) = 0 ;
	virtual HRESULT __stdcall SetStatus					( HRESULT hrStatus ) = 0 ;
	virtual HRESULT __stdcall GetObject					( D_IUnknown **ppObject ) = 0 ;
	virtual D_IUnknown *__stdcall GetStateNoAddRef		( void ) = 0 ;
} ;

class D_IMFAsyncCallback : public D_IUnknown
{
public:
	virtual HRESULT __stdcall GetParameters				( DWORD *pdwFlags, DWORD *pdwQueue ) = 0 ;
	virtual HRESULT __stdcall Invoke					( D_IMFAsyncResult *pAsyncResult ) = 0 ;
} ;

class D_IMFByteStream : public D_IUnknown
{
public:
	virtual HRESULT __stdcall GetCapabilities			( DWORD *pdwCapabilities ) = 0 ;
	virtual HRESULT __stdcall GetLength					( ULONGLONG *pqwLength ) = 0 ;
	virtual HRESULT __stdcall SetLength					( ULONGLONG qwLength ) = 0 ;
	virtual HRESULT __stdcall GetCurrentPosition		( ULONGLONG *pqwPosition ) = 0 ;
	virtual HRESULT __stdcall SetCurrentPosition		( ULONGLONG qwPosition ) = 0 ;
	virtual HRESULT __stdcall IsEndOfStream				( BOOL *pfEndOfStream ) = 0 ;
	virtual HRESULT __stdcall Read						( BYTE *pb, ULONG cb, ULONG *pcbRead ) = 0 ;
	virtual HRESULT __stdcall BeginRead					( BYTE *pb, ULONG cb, D_IMFAsyncCallback *pCallback, D_IUnknown *punkState ) = 0 ;
	virtual HRESULT __stdcall EndRead					( D_IMFAsyncResult *pResult, LONG *pcbRead ) = 0 ;
	virtual HRESULT __stdcall Write						( const BYTE *pb, ULONG cb, ULONG *pcbWritten ) = 0 ;
	virtual HRESULT __stdcall BeginWrite				( const BYTE *pb, ULONG cb, D_IMFAsyncCallback *pCallback, D_IUnknown *punkState ) = 0 ;
	virtual HRESULT __stdcall EndWrite					( D_IMFAsyncResult *pResult, ULONG *pcbWritten ) = 0 ;
	virtual HRESULT __stdcall Seek						( D_MFBYTESTREAM_SEEK_ORIGIN SeekOrigin, LONGLONG llSeekOffset, DWORD dwSeekFlags, ULONGLONG *pqwCurrentPosition ) = 0 ;
	virtual HRESULT __stdcall Flush						( void ) = 0 ;
	virtual HRESULT __stdcall Close						( void ) = 0 ;
} ;

inline UINT32 D_HI32( UINT64 unPacked )
{
    return ( UINT32 )( unPacked >> 32 ) ;
}

inline UINT32 D_LO32( UINT64 unPacked )
{
    return ( UINT32 )unPacked ;
}

inline UINT64 D_Pack2UINT32AsUINT64( UINT32 unHigh, UINT32 unLow )
{
    return ( ( UINT64 )unHigh << 32 ) | unLow ;
}

inline void D_Unpack2UINT32AsUINT64( UINT64 unPacked, UINT32* punHigh, UINT32* punLow )
{
	*punHigh = D_HI32( unPacked ) ;
	*punLow = D_LO32( unPacked ) ;
}

inline HRESULT D_MFGetAttribute2UINT32asUINT64( D_IMFAttributes *pAttributes, REFGUID guidKey, UINT32 *punHigh32, UINT32 *punLow32 )
{
	UINT64 unPacked;
	HRESULT hr = S_OK;

	hr = pAttributes->GetUINT64( guidKey, &unPacked ) ;
	if( FAILED( hr ) )
		return hr;
	D_Unpack2UINT32AsUINT64( unPacked, punHigh32, punLow32 ) ;

	return hr;
}

inline HRESULT D_MFSetAttribute2UINT32asUINT64( D_IMFAttributes* pAttributes, REFGUID guidKey, UINT32 unHigh32, UINT32 unLow32 )
{
    return pAttributes->SetUINT64( guidKey, D_Pack2UINT32AsUINT64( unHigh32, unLow32 ) ) ;
}

inline HRESULT D_MFGetAttributeSize( D_IMFAttributes* pAttributes, REFGUID guidKey, UINT32* punWidth, UINT32* punHeight	)
{
    return D_MFGetAttribute2UINT32asUINT64( pAttributes, guidKey, punWidth, punHeight ) ;
}

inline HRESULT D_MFGetAttributeRatio( D_IMFAttributes* pAttributes, REFGUID guidKey, UINT32* punNumerator, UINT32* punDenominator )
{
    return D_MFGetAttribute2UINT32asUINT64( pAttributes, guidKey, punNumerator, punDenominator ) ;
}

inline HRESULT D_MFSetAttributeSize( D_IMFAttributes* pAttributes, REFGUID guidKey, UINT32 unWidth, UINT32 unHeight )
{
    return D_MFSetAttribute2UINT32asUINT64( pAttributes, guidKey, unWidth, unHeight ) ;
}

inline HRESULT D_MFSetAttributeRatio( D_IMFAttributes* pAttributes, REFGUID guidKey, UINT32 unNumerator, UINT32 unDenominator )
{
    return D_MFSetAttribute2UINT32asUINT64( pAttributes, guidKey, unNumerator, unDenominator ) ;
}

#ifndef DX_NON_NAMESPACE

//}

#endif // DX_NON_NAMESPACE

#endif	/* __DXDIRECTX_H__ */


