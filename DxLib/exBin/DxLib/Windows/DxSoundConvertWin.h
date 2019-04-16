// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		WindowsOS用サウンドデータ変換プログラムヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXSOUNDCONVERTWIN_H__
#define __DXSOUNDCONVERTWIN_H__

#include "../DxCompileConfig.h"

// インクルード ------------------------------------------------------------------
#include "../DxLib.h"
#include "../DxThread.h"

#include "DxUseCStrmBaseFilter.h"

#ifndef DX_NON_ACM
	#if defined( DX_GCC_COMPILE ) || defined( DX_GCC_COMPILE_4_9_2 ) || defined( DX_GCC_COMPILE_5_3_0 )
		#include <mmreg.h>
	#endif // defined( DX_GCC_COMPILE ) || defined( DX_GCC_COMPILE_4_9_2 ) || defined( DX_GCC_COMPILE_5_3_0 )
	#include <msacm.h>
#endif // DX_NON_ACM

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

#define SOUND_METHODTYPE_ACM				(SOUND_METHODTYPE_DEFAULT_NUM + 0)		// ＡＣＭを使用
#define SOUND_METHODTYPE_DSMP3				(SOUND_METHODTYPE_DEFAULT_NUM + 1)		// ＤｉｒｅｃｔＳｈｏｗを使用したＭＰ３再生
#define SOUND_METHODTYPE_MF					(SOUND_METHODTYPE_DEFAULT_NUM + 2)		// Ｍｅｄｉａ　Ｆｏｕｎｄａｔｉｏｎを使用

// 構造体定義 --------------------------------------------------------------------

#ifndef DX_NON_ACM

// #ifdef DX_GCC_COMPILE
struct D_HACMSTREAM__
{
	int unused ;
} ;

typedef struct D_HACMSTREAM__ *D_HACMSTREAM ;
typedef        D_HACMSTREAM   *D_LPHACMSTREAM ;

struct D_HACMOBJ__
{
	int unused ;
} ;
typedef struct D_HACMOBJ__     *D_HACMOBJ ;

typedef struct D_wavefilter_tag
{
	DWORD   cbStruct;           /* Size of the filter in bytes */
	DWORD   dwFilterTag;        /* filter type */
	DWORD   fdwFilter;          /* Flags for the filter (Universal Dfns) */
	DWORD   dwReserved[5];      /* Reserved for system use */
} D_WAVEFILTER;
typedef D_WAVEFILTER FAR  *D_LPWAVEFILTER;

#define D_MPEGLAYER3_WFX_EXTRA_BYTES			12
#define D_WAVE_FORMAT_MPEGLAYER3				0x0055 /* ISO/MPEG Layer3 Format Tag */

#define D_MPEGLAYER3_ID_UNKNOWN					0
#define D_MPEGLAYER3_ID_MPEG					1
#define D_MPEGLAYER3_ID_CONSTANTFRAMESIZE		2

typedef struct D_mpeglayer3waveformat_tag
{
	WAVEFORMATEX  wfx;
	WORD          wID;
	DWORD         fdwFlags;
	WORD          nBlockSize;
	WORD          nFramesPerBlock;
	WORD          nCodecDelay;
} D_MPEGLAYER3WAVEFORMAT ;

#define D_ACM_STREAMSIZEF_SOURCE				0x00000000L
#define D_ACM_STREAMSIZEF_DESTINATION			0x00000001L
#define D_ACM_STREAMSIZEF_QUERYMASK				0x0000000FL

#define D_ACM_METRIC_MAX_SIZE_FORMAT			50

#define D_ACM_STREAMCONVERTF_BLOCKALIGN			0x00000004
#define D_ACM_STREAMCONVERTF_START				0x00000010
#define D_ACM_STREAMCONVERTF_END				0x00000020

#ifdef  _WIN64
#define D__DRVRESERVED    15
#else
#define D__DRVRESERVED    10
#endif  // _WIN64

typedef struct D_tACMSTREAMHEADER
{
    DWORD           cbStruct;               // sizeof(D_ACMSTREAMHEADER)
    DWORD           fdwStatus;              // ACMSTREAMHEADER_STATUSF_*
    DWORD_PTR       dwUser;                 // user instance data for hdr
    LPBYTE          pbSrc;
    DWORD           cbSrcLength;
    DWORD           cbSrcLengthUsed;
    DWORD_PTR       dwSrcUser;              // user instance data for src
    LPBYTE          pbDst;
    DWORD           cbDstLength;
    DWORD           cbDstLengthUsed;
    DWORD_PTR       dwDstUser;              // user instance data for dst
    DWORD           dwReservedDriver[D__DRVRESERVED];   // driver reserved work space

} D_ACMSTREAMHEADER, *D_PACMSTREAMHEADER, FAR *D_LPACMSTREAMHEADER;
// #endif

// ＡＣＭデータ変換用構造体
struct SOUNDCONV_ACM
{
	WAVEFORMATEX				*Format ;				// 変換元のフォーマット
	D_HACMSTREAM				AcmStreamHandle[2]	;	// 圧縮されたデータを扱う際に使うＡＣＭのハンドル

	void						*SrcData ;				// 変換元のデータを一時的に保存しておくメモリ領域
	int							SrcDataSize ;			// 変換元のデータを一時的に保存しておくメモリ領域のサイズ
//	int							SrcDataValidSize ;		// 変換元のデータの有効なサイズ
	int							SrcDataPosition ;		// 変換元のデータの変換が完了したサイズ
	int							DestDataSampleNum ;		// 変換後のデータのサンプル数( -1:以外の場合のみ有効 )

	int							BeginSeekCompSrcSize ;	// シーク予定位置までサブACMハンドルで変換しておく処理で、既に変換が完了している圧縮データのサイズ
	int							BeginSeekPosition ;		// シーク予定位置までサブACMハンドルで変換しておく処理で、既に変換が完了している展開後データのサイズ(シーク予定位置は SOUNDCONV 関数の SeekLockPosition メンバー)
	void						*BeginSeekBuffer ;		// シーク予定位置までサブACMハンドルで変換しておく処理で使用するバッファ
} ;

#endif // DX_NON_ACM

#ifndef DX_NON_MOVIE
#ifndef DX_NON_DSHOW_MOVIE

// ＤｉｒｅｃｔＳｈｏｗを使ったＭＰ３データ変換処理で使用するバッファクラス
class SOUNDCONV_DSMP3_BUF : public D_ISampleGrabberCB
{
public :
	struct SOUNDCONV			*SoundConvData ;		// 音声データ変換処理用構造体へのポインタ

	ULONG __stdcall				AddRef() ;
	ULONG __stdcall				Release() ;

	HRESULT __stdcall			QueryInterface( REFIID riid, void ** ppv ) ;
	HRESULT __stdcall			SampleCB( double SampleTime, D_IMediaSample *pSample ) ;
	HRESULT __stdcall			BufferCB( double SampleTime, BYTE *pBuffer, long BufferLen ) ;
} ;

// ＤｉｒｅｃｔＳｈｏｗを使ったＭＰ３データ変換用構造体
struct SOUNDCONV_DSMP3
{
	void						*PCMBuffer ;			// ＰＣＭに変換されたデータを格納するメモリ領域
	DWORD						PCMBufferSize ;			// ＰＣＭバッファのサイズ
	DWORD						PCMValidDataSize ;		// 有効なＰＣＭのサイズ
	DWORD						PCMDestCopySize ;		// 本バッファへのコピーが完了したサイズ
} ;

#endif  // DX_NON_DSHOW_MOVIE
#endif	// DX_NON_MOVIE

// サウンド変換処理全体で使用するデータ構造体で Windows に依存している情報の構造体
struct SOUNDCONVERTDATA_WIN
{
	int							Dummy ;
#ifndef DX_NON_ACM
	HMODULE						msacm32DLL ;					// msacm32DLL
	MMRESULT					( WINAPI *acmStreamOpenFunc            )( D_LPHACMSTREAM phas, HACMDRIVER had, LPWAVEFORMATEX pwfxSrc, LPWAVEFORMATEX pwfxDst, D_LPWAVEFILTER pwfltr, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen );
	MMRESULT					( WINAPI *acmFormatSuggestFunc         )( HACMDRIVER had, LPWAVEFORMATEX pwfxSrc, LPWAVEFORMATEX pwfxDst, DWORD cbwfxDst, DWORD fdwSuggest );
	MMRESULT					( WINAPI *acmStreamCloseFunc           )( D_HACMSTREAM has, DWORD fdwClose );
	MMRESULT					( WINAPI *acmMetricsFunc               )( D_HACMOBJ hao, UINT uMetric, LPVOID pMetric );
	MMRESULT					( WINAPI *acmStreamPrepareHeaderFunc   )( D_HACMSTREAM has, D_LPACMSTREAMHEADER pash, DWORD fdwPrepare );
	MMRESULT					( WINAPI *acmStreamConvertFunc         )( D_HACMSTREAM has, D_LPACMSTREAMHEADER pash, DWORD fdwConvert );
	MMRESULT					( WINAPI *acmStreamUnprepareHeaderFunc )( D_HACMSTREAM has, D_LPACMSTREAMHEADER pash, DWORD fdwUnprepare );
	MMRESULT					( WINAPI *acmStreamSizeFunc            )( D_HACMSTREAM has, DWORD cbInput, LPDWORD pdwOutputBytes, DWORD fdwSize );
#endif
} ;

// 内部大域変数宣言 --------------------------------------------------------------

// 関数プロトタイプ宣言-----------------------------------------------------------

#ifndef DX_NON_ACM
extern	int SetupSoundConvert_ACM(            struct SOUNDCONV *SoundConv ) ;								// ＡＣＭを使用したファイルのセットアップ処理を行う( [戻] -1:エラー )
extern	int SetupSoundConvert_MP3(            struct SOUNDCONV *SoundConv ) ;								// ＭＰ３ファイルのセットアップ処理を行う( [戻] -1:エラー )
extern	int TerminateSoundConvert_ACM(        struct SOUNDCONV *SoundConv ) ;								// ＡＣＭを使用したファイルの後始末処理を行う
extern	int ConvertProcessSoundConvert_ACM(   struct SOUNDCONV *SoundConv ) ;								// 変換後のバッファにデータを補充する
//extern	int SetTimeSoundConvert_ACM(      struct SOUNDCONV *SoundConv, int Time ) ;						// 変換処理を位置を変更する( ミリ秒単位 )
extern	int SetSampleTimeSoundConvert_ACM(    struct SOUNDCONV *SoundConv, int SampleTime ) ;				// 変換処理の位置を変更する( サンプル単位 )
extern	int GetSoundConvertDestSize_Fast_ACM( struct SOUNDCONV *SoundConv ) ;								// 変換後の大凡のデータサイズを得る
#endif

#ifndef DX_NON_MOVIE
#ifndef DX_NON_DSHOW_MOVIE
#ifndef DX_NON_DSHOW_MP3
extern	int TerminateSoundConvert_DSMP3(        struct SOUNDCONV *SoundConv ) ;								// MP3を使用したファイルの後始末処理を行う
extern	int ConvertProcessSoundConvert_DSMP3(   struct SOUNDCONV *SoundConv ) ;								// 変換後のバッファにデータを補充する
//extern	int SetTimeSoundConvert_DSMP3(      struct SOUNDCONV *SoundConv, int Time ) ;					// 変換処理を位置を変更する( ミリ秒単位 )
extern	int SetSampleTimeSoundConvert_DSMP3(    struct SOUNDCONV *SoundConv, int SampleTime ) ;				// 変換処理の位置を変更する( サンプル単位 )
extern	int GetSoundConvertDestSize_Fast_DSMP3( struct SOUNDCONV *SoundConv ) ;								// 変換後の大凡のデータサイズを得る
#endif
#endif
#endif

#ifndef DX_NON_MEDIA_FOUNDATION
extern	int SetupSoundConvert_MF(            SOUNDCONV *SoundConv ) ;										// Media Foundation を使用したファイルのセットアップ処理を行う( [戻] -1:エラー )
extern	int TerminateSoundConvert_MF(        SOUNDCONV *SoundConv ) ;										// Media Foundation を使用したファイルの後始末処理を行う
extern	int ConvertProcessSoundConvert_MF(   SOUNDCONV *SoundConv ) ;										// 変換後のバッファにデータを補充する
//extern	int SetTimeSoundConvert_MF(      SOUNDCONV *SoundConv, int Time ) ;								// 変換処理を位置を変更する( ミリ秒単位 )
extern	int SetSampleTimeSoundConvert_MF(    SOUNDCONV *SoundConv, int SampleTime ) ;						// 変換処理の位置を変更する( サンプル単位 )
extern	int GetSoundConvertDestSize_Fast_MF( SOUNDCONV *SoundConv ) ;										// 変換後の大凡のデータサイズを得る
#endif // DX_NON_MEDIA_FOUNDATION

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // __DXSOUNDCONVERTWIN_H__
