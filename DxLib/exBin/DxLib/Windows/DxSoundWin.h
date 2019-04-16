// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		WindowsOS用サウンドプログラムヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXSOUNDWIN_H__
#define __DXSOUNDWIN_H__

#include "../DxCompileConfig.h"

#ifndef DX_NON_SOUND

// インクルード ------------------------------------------------------------------
#include "../DxLib.h"
#include "DxDirectX.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

// ストリーム関係の定義( Windows環境はHDDがスリープするので、ファイルから直接再生する場合はバッファを多めに確保 )
#define STS_BUFSEC_FILE						(256 * 13)			// ストリーム風サウンド再生時に確保するサウンドバッファの時間( 単位は STS_DIVNUM分の1秒 )( ファイルからのストリーム再生用 )
#define STS_ADVANCECOPYSEC_FILE				(256 * 12 + 192)	// ストリーム風サウンド再生時に音声データを先行展開しておく最大時間( 単位は STS_DIVNUM分の1秒 )( ファイルからのストリーム再生用 )
#define STS_MINADVANCECOPYSEC_FILE			(256 * 12)			// ストリーム風サウンド再生時に音声データを先行展開しておく最小時間( 単位は STS_DIVNUM分の1秒 )( ファイルからのストリーム再生用 )

#define D_X3DAUDIO_INPUTCHANNELS		(8)				// X3DAudio の計算で使用する最大入力チャンネル数

// 構造体定義 --------------------------------------------------------------------

// XAudio2用コールバック処理用構造体
struct SOUNDBUFFER_CALLBACK : public D_IXAudio2VoiceCallback
{
	virtual void    __stdcall OnVoiceProcessingPassStart( DWORD BytesRequired ) ;
	virtual void    __stdcall OnVoiceProcessingPassEnd	() ;
	virtual void    __stdcall OnStreamEnd				() ;
	virtual void    __stdcall OnBufferStart				( void* pBufferContext ) ;
	virtual void    __stdcall OnBufferEnd				( void* pBufferContext ) ;
	virtual void    __stdcall OnLoopEnd					( void* pBufferContext ) ;
	virtual void    __stdcall OnVoiceError				( void* pBufferContext, HRESULT Error ) ;

	struct SOUNDBUFFER			*Buffer ;
} ;

// サウンドバッファ環境依存情報構造体
struct SOUNDBUFFER_PF
{
	union
	{
		D_IDirectSoundBuffer 		*DSBuffer ;				// DirectSoundBuffer
		D_IXAudio2SourceVoice		*XA2SourceVoice ;		// XAudio2SourceVoice
		D_IXAudio2_8SourceVoice		*XA2_8SourceVoice ;		// XAudio2_8SourceVoice
	} ;
	union
	{
		D_IXAudio2SubmixVoice		*XA2SubmixVoice ;		// XAudio2SubmixVoice
		D_IXAudio2_8SubmixVoice		*XA2_8SubmixVoice ;		// XAudio2_8SubmixVoice
	} ;
	D_IUnknown					*XA2ReverbEffect ;		// リバーブエフェクト
	SOUNDBUFFER_CALLBACK		*XA2Callback ;			// XAudio2用のコールバック

	union
	{
		D_XAUDIO2FX_REVERB_PARAMETERS    XAudio2ReverbParameter ;	// リバーブ計算用パラメータ
		D_XAUDIO2FX_REVERB_PARAMETERS2_8 XAudio2_8ReverbParameter ;	// リバーブ計算用パラメータXAudio2.8用
	} ;

	int							XAudioChannels ;		// XAudio 側でのチャンネル数

	// ３Ｄサウンド再生用データ
	D_X3DAUDIO_EMITTER			X3DAudioEmitterData ;									// ３Ｄサウンド用音源情報
	D_X3DAUDIO_CONE				X3DAudioEmitterConeData ;								// ３Ｄサウンド用音源情報で使用するコーン情報
	float						X3DAudioEmitterChannelAzimuths[ D_X3DAUDIO_INPUTCHANNELS ] ;	// ３Ｄサウンド用音源情報で使用するチャンネル位置テーブル
} ;

// サウンドシステム用環境依存データ構造体
struct SOUNDSYSTEMDATA_PF
{
	D_IDirectMusicLoader8		*DirectMusicLoaderObject ;		// DirectMusicLoader8 オブジェクト
	D_IDirectMusicPerformance8	*DirectMusicPerformanceObject ;	// DirectMusicPerformance8 オブジェクト

	HMODULE						XAudio2_8DLL ;					// XAudio2_8.dll
	HRESULT						( WINAPI *XAudio2CreateFunc )( D_IXAudio2_8 ** ppXAudio2, DWORD Flags, D_XAUDIO2_PROCESSOR XAudio2Processor );
	HRESULT						( WINAPI *CreateAudioVolumeMeterFunc )( D_IUnknown** ppApo ) ;
	HRESULT						( WINAPI *CreateAudioReverbFunc )( D_IUnknown** ppApo ) ;

	HMODULE						X3DAudioDLL ;					// X3DAudio.dll
	void						( __cdecl *X3DAudioInitializeFunc )( DWORD SpeakerChannelMask, float SpeedOfSound, D_X3DAUDIO_HANDLE Instance ) ;
	void						( __cdecl *X3DAudioCalculateFunc )( const D_X3DAUDIO_HANDLE Instance, const D_X3DAUDIO_LISTENER* pListener, const D_X3DAUDIO_EMITTER* pEmitter, DWORD Flags, D_X3DAUDIO_DSP_SETTINGS* pDSPSettings ) ;

	union
	{
		D_IXAudio2				*XAudio2Object ;				// XAudio2オブジェクト
		D_IXAudio2_8			*XAudio2_8Object ;				// XAudio2_8オブジェクト
		D_IDirectSound			*DirectSoundObject ;			// ＤｉｒｅｃｔＳｏｕｎｄオブジェクト
	} ;

	D_XAUDIO2FX_REVERB_PARAMETERS    XAudio2ReverbParameters[ D_XAUDIO2FX_PRESET_NUM ] ;	// リバーブ計算用パラメータ
	D_XAUDIO2FX_REVERB_PARAMETERS2_8 XAudio2_8ReverbParameters[ D_XAUDIO2FX_PRESET_NUM ] ;	// リバーブ計算用パラメータXAudio2.8用
	D_IXAudio2MasteringVoice	*XAudio2MasteringVoiceObject ;	// XAudio2MasteringVoiceオブジェクト
	D_IXAudio2_8MasteringVoice	*XAudio2_8MasteringVoiceObject ;// XAudio2_8MasteringVoiceオブジェクト
	D_XAUDIO2_EFFECT_CHAIN		XAudio2MasteringEffectChain ;	// マスターボイス用のエフェクトチェイン

	DWORD						XAudio2OutputChannelMask ;		// 出力チャンネルマスク

	D_X3DAUDIO_HANDLE			X3DAudioInstance ;				// ３Ｄサウンド計算用インスタンスハンドル
	D_X3DAUDIO_CONE				X3DAudioListenerConeData ;		// ３Ｄサウンドのリスナー情報に使用するコーン情報
	D_X3DAUDIO_LISTENER			X3DAudioListenerData ;			// ３Ｄサウンドのリスナー情報

	D_IDirectSoundBuffer		*PrimarySoundBuffer ;			// プライマリサウンドバッファ

	D_IDirectSoundBuffer		*NoSoundBuffer ;				// 無音バッファ

	int							EnableXAudioFlag ;				// XAudioを使用するかどうかのフラグ( TRUE:使用する  FALSE:使用しない ) 
	int							UseSoftwareMixing ;				// ソフトウエアミキシングを行うかどうかのフラグ( TRUE:ソフトウエア  FALSE:ハードウエア )

	HANDLE						StreamSoundThreadHandle ;		// ストリームサウンドを再生する処理を行うスレッドのハンドル
	DWORD						StreamSoundThreadID ;			// ストリームサウンドを再生する処理を行うスレッドのＩＤ
	DWORD						StreamSoundThreadEndFlag ;		// ストリームサウンドを再生する処理を終了するかどうかのフラグ
} ;

// ＭＩＤＩデータ環境依存情報
struct MIDIHANDLEDATA_PF
{
	D_IDirectMusicSegment8		*DirectMusicSegmentObject ;		// DirectMusicSegment8 オブジェクト
} ;

// ＭＩＤＩシステム用環境依存データ構造体
struct MIDISYSTEMDATA_PF
{
	UINT						MidiDeviceID ;					// ＭＩＤＩ演奏時のデバイスＩＤ
} ;

// 内部大域変数宣言 --------------------------------------------------------------

// 関数プロトタイプ宣言-----------------------------------------------------------

extern	int		MidiCallBackProcess( void ) ;																			// ＭＩＤＩ演奏終了時呼ばれるコールバック関数


#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_NON_SOUND

#endif // __DXSOUNDWIN_H__
