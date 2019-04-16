//-----------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		WindowsOS用動画プログラム
// 
//  	Ver 3.20c
// 
//-----------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

#include "../DxCompileConfig.h"

#ifndef DX_NON_MOVIE

// インクルード----------------------------------------------------------------
#include "DxMovieWin.h"
#include "DxWinAPI.h"
#include "DxFileWin.h"
#include "../DxGraphics.h"
#include "../DxLog.h"
#include "../DxMovie.h"
#include "../DxSound.h"
#include "../DxSystem.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義------------------------------------------------------------------

// 型定義----------------------------------------------------------------------

// データ宣言------------------------------------------------------------------

#ifndef DX_NON_MEDIA_FOUNDATION
static GUID *g_MFVideoFormatGUIDTable[ D_MFVIDEOFORMAT_TYPE_UNKNOWN ] =
{
	&D_MFVIDEOFORMAT_BASE,
	&D_MFVIDEOFORMAT_RGB32,
	&D_MFVIDEOFORMAT_ARGB32,
	&D_MFVIDEOFORMAT_RGB24,
	&D_MFVIDEOFORMAT_RGB555,
	&D_MFVIDEOFORMAT_RGB565,
	&D_MFVIDEOFORMAT_RGB8,
	&D_MFVIDEOFORMAT_AI44,
	&D_MFVIDEOFORMAT_AYUV,
	&D_MFVIDEOFORMAT_YUY2,
	&D_MFVIDEOFORMAT_YVYU,
	&D_MFVIDEOFORMAT_YVU9,
	&D_MFVIDEOFORMAT_UYVY,
	&D_MFVIDEOFORMAT_NV11,
	&D_MFVIDEOFORMAT_NV12,
	&D_MFVIDEOFORMAT_YV12,
	&D_MFVIDEOFORMAT_I420,
	&D_MFVIDEOFORMAT_IYUV,
	&D_MFVIDEOFORMAT_Y210,
	&D_MFVIDEOFORMAT_Y216,
	&D_MFVIDEOFORMAT_Y410,
	&D_MFVIDEOFORMAT_Y416,
	&D_MFVIDEOFORMAT_Y41P,
	&D_MFVIDEOFORMAT_Y41T,
	&D_MFVIDEOFORMAT_Y42T,
	&D_MFVIDEOFORMAT_P210,
	&D_MFVIDEOFORMAT_P216,
	&D_MFVIDEOFORMAT_P010,
	&D_MFVIDEOFORMAT_P016,
	&D_MFVIDEOFORMAT_v210,
	&D_MFVIDEOFORMAT_v216,
	&D_MFVIDEOFORMAT_v410,
	&D_MFVIDEOFORMAT_MP43,
	&D_MFVIDEOFORMAT_MP4S,
	&D_MFVIDEOFORMAT_M4S2,
	&D_MFVIDEOFORMAT_MP4V,
	&D_MFVIDEOFORMAT_WMV1,
	&D_MFVIDEOFORMAT_WMV2,
	&D_MFVIDEOFORMAT_WMV3,
	&D_MFVIDEOFORMAT_WVC1,
	&D_MFVIDEOFORMAT_MSS1,
	&D_MFVIDEOFORMAT_MSS2,
	&D_MFVIDEOFORMAT_MPG1,
	&D_MFVIDEOFORMAT_DVSL,
	&D_MFVIDEOFORMAT_DVSD,
	&D_MFVIDEOFORMAT_DVHD,
	&D_MFVIDEOFORMAT_DV25,
	&D_MFVIDEOFORMAT_DV50,
	&D_MFVIDEOFORMAT_DVH1,
	&D_MFVIDEOFORMAT_DVC,
	&D_MFVIDEOFORMAT_H264,
	&D_MFVIDEOFORMAT_MJPG,
} ;

#endif // DX_NON_MEDIA_FOUNDATION

// 関数プロトタイプ宣言 -------------------------------------------------------

#ifndef DX_NON_MEDIA_FOUNDATION
// Media Foundation を使用した動画ファイルのオープンを試みる
static int MediaFoundationOpenMovieFile(
	MOVIEGRAPH * Movie,
	const wchar_t *FileName,
	int *Width,
	int *Height,
	int ImageSizeGetOnly,
	int ASyncThread
) ;

// カレントフレームのRGB画像を作成する( 戻り値  1:作成された  0:されなかった )
static int MediaFoundationMovie_SetupImage(
	MOVIEGRAPH * Movie,
	int BaseImage,
	int YUVGrHandle,
	int ASyncThread
) ;

// Media Foundation を使用した動画ファイルのフレームを進める
static int MediaFoundationMovie_IncToFrame(
	MOVIEGRAPH * Movie,
	int AddFrame,
	int ASyncThread
) ;
#endif

#ifndef DX_NON_DSHOW_MOVIE
// DirectShow を使用した動画ファイルのオープンを試みる
extern int DirectShowOpenMovieFile(
	MOVIEGRAPH * Movie,
	const wchar_t *FileName,
	int *Width,
	int *Height,
	int SurfaceMode,
	int ImageSizeGetOnly,
	int ASyncThread
) ;
#endif

// プログラム------------------------------------------------------------------

// ムービー関連の管理処理の初期化の環境依存処理
extern int InitializeMovieManage_PF( void )
{
	return 0 ;
}

// ムービー関連の管理処理の後始末の環境依存処理
extern int TerminateMovieManage_PF( void )
{
#ifndef DX_NON_MEDIA_FOUNDATION
	SETUP_WIN_API

	// MFStartup が呼ばれていたら MFShutdown を呼ぶ
	if( MovieGraphManageData.PF.MFStartupRunFlag )
	{
		MovieGraphManageData.PF.MFStartupRunFlag = FALSE ;

		if( WinAPIData.Win32Func.MFShutdownFunc != NULL )
		{
			WinAPIData.Win32Func.MFShutdownFunc() ;
		}
	}

#endif // DX_NON_MEDIA_FOUNDATION

	return 0 ;
}

#ifndef DX_NON_MEDIA_FOUNDATION

// Media Foundation を使用した動画ファイルのオープンを試みる
static int MediaFoundationOpenMovieFile(
	MOVIEGRAPH * Movie,
	const wchar_t *FileName,
	int *Width,
	int *Height,
	int ImageSizeGetOnly,
	int ASyncThread
)
{
	HRESULT hr ;
	wchar_t ErStr[ 256 ] ;

	SETUP_WIN_API

	Movie->NowImage.GraphData = NULL;
	Movie->YUVFlag = FALSE ;
	Movie->OverlayDestX = 0 ;
	Movie->OverlayDestY = 0 ;
	Movie->OverlayDestExRate = 0 ;
	Movie->OverlayDispFlag = 0 ;
	Movie->FirstUpdateFlag = FALSE ;
	_MEMSET( &Movie->OverlaySrcRect, 0, sizeof( RECT ) ) ;
	_MEMSET( &Movie->OverlayDestRect, 0, sizeof( RECT ) ) ;

	Movie->PF.MFImageBuffer = NULL ;
	Movie->PF.MFYBuffer = NULL ;
	Movie->PF.MFUVBuffer = NULL ;

	// 最後の ReadSample の時間、フレームをリセット
	Movie->PF.MFLastReadSampleTimeStamp = -1 ;
	Movie->PF.MFLastReadSampleFrame = -1 ;

	// Media Foundation 関連の DLL が無かったらエラー
	if( WinAPIData.Win32Func.MFPLATDLL == NULL ||
		WinAPIData.Win32Func.MFREADWRITEDLL == NULL ||
		WinAPIData.Win32Func.MFStartupFunc == NULL ||
		WinAPIData.Win32Func.MFCreateSourceReaderFromURLFunc == NULL ||
		WinAPIData.Win32Func.MFCreateMediaTypeFunc == NULL /*||
		WinAPIData.Win32Func.PropVariantToInt64Func == NULL*/ )
	{
		return -1 ;
	}

	// まだ MFStartup を呼んでいなかったら呼ぶ
	if( MovieGraphManageData.PF.MFStartupRunFlag == FALSE )
	{
		MovieGraphManageData.PF.MFStartupRunFlag = TRUE ;

		hr = WinAPIData.Win32Func.MFStartupFunc( D_MF_VERSION, D_MFSTARTUP_FULL ) ;
		if( hr != S_OK )
		{
			_WCSCPY_S( ErStr, sizeof( ErStr ), L"MFStartup Error\n" ) ;
			goto ERR ;
		}
	}

   	// ファイル名保存
	_WCSCPY_S( Movie->PF.FileName, sizeof( Movie->PF.FileName ), FileName ) ;

	// ディレクトリの区切りが / になっている箇所を \ に置き換える
	{
		wchar_t *wp ;

		for( wp = Movie->PF.FileName; *wp; wp++ )
		{
			if( *wp == L'/' )
			{
				*wp = '\\' ;
			}
		}
	}

	// ファイルを開く
	if( WinAPIData.Win32Func.MFCreateSourceReaderFromURLFunc( Movie->PF.FileName, NULL, &Movie->PF.pMFReader ) != S_OK )
	{
		DWORD_PTR fp ;
		HANDLE FileHandle ;
		void *TempBuffer ;
		size_t CompSize, MoveSize ;
		DWORD WriteSize ;
		size_t FileSize ;
		const DWORD BufferSize = 0x100000 ;

		// 既にテンポラリファイルを作成している場合は、
		// 更にテンポラリファイルを作成することはしない
		if( Movie->PF.UseTemporaryFile == TRUE )
			goto ERR ;

		// ファイルが開けなかったらアーカイブされている可能性がある
#ifdef UNICODE
		fp = DX_FOPEN( Movie->PF.FileName ) ;
#else
		fp = DX_FOPEN( FileName ) ;
#endif
		if( fp == 0 ) goto ERR ;
		
		// 開けた場合はテンポラリファイルに書き出す
		{
			// ファイルサイズの取得
			DX_FSEEK( fp, 0L, SEEK_END ) ;
			FileSize = ( size_t )DX_FTELL( fp ) ;
			DX_FSEEK( fp, 0L, SEEK_SET ) ;

			// 一時的にデータを格納するバッファを確保
			TempBuffer = DXALLOC( BufferSize ) ;
			if( TempBuffer == NULL )
			{
				DX_FCLOSE( fp ) ;
				goto ERR ;
			}

			// テンポラリファイルの作成
			FileHandle = CreateTemporaryFile( Movie->PF.FileName, sizeof( Movie->PF.FileName ) ) ;

			if( FileHandle == NULL )
			{
				DX_FCLOSE( fp ) ;
				DXFREE( TempBuffer ) ;
				goto ERR ;
			}
			Movie->PF.UseTemporaryFile = TRUE ;

			// テンポラリファイルにデータを書き出す
			CompSize = 0 ;
			while( CompSize < FileSize )
			{
				MoveSize = CompSize - FileSize ;
				if( MoveSize > BufferSize ) MoveSize = BufferSize ;

				DX_FREAD( TempBuffer, MoveSize, 1, fp ) ;
				WinAPIData.Win32Func.WriteFileFunc( FileHandle, TempBuffer, ( DWORD )MoveSize, &WriteSize, NULL ) ;

				if( MoveSize != WriteSize ) break ;
				CompSize += MoveSize ;
			}

			// ファイルを閉じ、メモリを解放する
			DX_FCLOSE( fp ) ;
			WinAPIData.Win32Func.CloseHandleFunc( FileHandle ) ;
			DXFREE( TempBuffer ) ;
		}

		// 改めてファイルを開く
		if( WinAPIData.Win32Func.MFCreateSourceReaderFromURLFunc( Movie->PF.FileName, NULL, &Movie->PF.pMFReader ) != S_OK )
		{
			// テンポラリファイルを削除
			WinAPIData.Win32Func.DeleteFileWFunc( Movie->PF.FileName ) ;
			goto ERR ;
		}
	}

	// ファイルの情報を取得する
	if( Movie->PF.pMFReader->GetCurrentMediaType(
			( DWORD )D_MF_SOURCE_READER_FIRST_VIDEO_STREAM, 
			&Movie->PF.pMFMediaTypeVideoStream ) != S_OK )
		goto ERR ;

	// サイズの取得
	if( D_MFGetAttributeSize( Movie->PF.pMFMediaTypeVideoStream, D_MF_MT_FRAME_SIZE, &Movie->PF.MFFrameSizeX, &Movie->PF.MFFrameSizeY ) != S_OK )
		goto ERR ;

	// サイズの取得のみの場合はここで終了
	if( ImageSizeGetOnly )
	{
		if( Width  ) *Width  = ( int )Movie->PF.MFFrameSizeX ;
		if( Height ) *Height = ( int )Movie->PF.MFFrameSizeY ;

		return 0 ;
	}

	// フレームレートの取得
	if( D_MFGetAttributeRatio( Movie->PF.pMFMediaTypeVideoStream, D_MF_MT_FRAME_RATE, &Movie->PF.MFFrameRateNumerator, &Movie->PF.MFFrameRateDenominator ) != S_OK )
		goto ERR ;

	// アスペクト比の取得
	if( D_MFGetAttributeRatio( Movie->PF.pMFMediaTypeVideoStream, D_MF_MT_PIXEL_ASPECT_RATIO, &Movie->PF.MFAspectRatioX, &Movie->PF.MFAspectRatioY ) != S_OK )
		goto ERR ;

	// 再生時間を取得
	if( Movie->PF.pMFReader->GetPresentationAttribute( ( DWORD )D_MF_SOURCE_READER_MEDIASOURCE, D_MF_PD_DURATION, &Movie->PF.MFDuration ) != S_OK )
		goto ERR ;
	Movie->StopTime = Movie->PF.MFDuration.hVal.QuadPart ;

	// 総フレーム数を算出
	{
		LONGLONG Temp64_1, Temp64_2 ;
		DWORD Temp128[ 4 ] ;

		Temp64_1 = Movie->PF.MFFrameRateNumerator ;
		_MUL128_1( ( DWORD * )&Movie->PF.MFDuration.hVal.QuadPart, ( DWORD * )&Temp64_1, Temp128 ) ;
		Temp64_1 = ( LONGLONG )10000000 * ( LONGLONG )Movie->PF.MFFrameRateDenominator ;
		_DIV128_1( Temp128, ( DWORD * )&Temp64_1, ( DWORD * )&Temp64_2 ) ;
		Movie->PF.MFTotalFrame = ( int )Temp64_2 ;
	}

	// 出力形式を作成
	WinAPIData.Win32Func.MFCreateMediaTypeFunc( &Movie->PF.pMFMediaTypeOutputVideoStream ) ;
	Movie->PF.pMFMediaTypeOutputVideoStream->SetGUID( D_MF_MT_MAJOR_TYPE, D_MFMEDIATYPE_VIDEO ) ;
	Movie->PF.pMFMediaTypeOutputVideoStream->SetGUID( D_MF_MT_SUBTYPE, D_MFVIDEOFORMAT_IYUV ) ;
	Movie->PF.pMFMediaTypeOutputVideoStream->SetUINT32( D_MF_MT_DEFAULT_STRIDE, Movie->PF.MFFrameSizeX ) ;
	D_MFSetAttributeRatio( ( D_IMFAttributes * )Movie->PF.pMFMediaTypeOutputVideoStream, D_MF_MT_FRAME_RATE, Movie->PF.MFFrameRateNumerator, Movie->PF.MFFrameRateDenominator ) ;
	D_MFSetAttributeSize( ( D_IMFAttributes * )Movie->PF.pMFMediaTypeOutputVideoStream, D_MF_MT_FRAME_SIZE, Movie->PF.MFFrameSizeX, Movie->PF.MFFrameSizeY ) ;
//	Movie->PF.pMFMediaTypeOutputVideoStream->SetUINT32( D_MF_MT_INTERLACE_MODE, D_MFVideoInterlace_Progressive ) ;
	Movie->PF.pMFMediaTypeOutputVideoStream->SetUINT32( D_MF_MT_ALL_SAMPLES_INDEPENDENT, TRUE ) ;
	D_MFSetAttributeRatio( ( D_IMFAttributes * )Movie->PF.pMFMediaTypeOutputVideoStream, D_MF_MT_PIXEL_ASPECT_RATIO, Movie->PF.MFAspectRatioX, Movie->PF.MFAspectRatioY ) ;

	// 出力形式をセット
	hr = Movie->PF.pMFReader->SetCurrentMediaType(
			( DWORD )D_MF_SOURCE_READER_FIRST_VIDEO_STREAM,
			NULL,
			Movie->PF.pMFMediaTypeOutputVideoStream ) ;
	if( hr != S_OK )
		goto ERR ;

	// １フレーム辺りの時間をセット
	if( Movie->PF.MFFrameRateNumerator == 0 )
	{
		Movie->PF.FrameTime = 10000000 / 60 ;
	}
	else
	{
		LONGLONG OneSec = ( D_STREAM_TIME )10000000 ;
		Movie->PF.FrameTime = OneSec * ( D_STREAM_TIME )Movie->PF.MFFrameRateDenominator / Movie->PF.MFFrameRateNumerator ;
	}

	// 再生速度の初期化
	Movie->PF.MFPlaySpeedRate = 1.0 ;

	// サーフェスモードはノーマル
	Movie->SurfaceMode = DX_MOVIESURFACE_NORMAL ;

	// 画像構築の後、グラフィックハンドルのセットアップを終えているかのフラグを倒しておく
	Movie->PF.MFSetupGraphHandleImage = FALSE ;

	// 画像イメージの情報をセットする
	Movie->NowImage.Width        = ( int )Movie->PF.MFFrameSizeX ;
	Movie->NowImage.Height       = ( int )Movie->PF.MFFrameSizeY ;
	Movie->NowImage.Pitch        = ( int )( ( Movie->PF.MFFrameSizeX * 4 + 15 ) / 16 * 16 ) ;
	Movie->PF.MFImageBuffer      = DXALLOC( Movie->NowImage.Pitch * Movie->NowImage.Height ) ;
	if( Movie->PF.MFImageBuffer == NULL )
		goto ERR ;
	_MEMSET( Movie->PF.MFImageBuffer, 0, Movie->NowImage.Pitch * Movie->NowImage.Height ) ;
	Movie->NowImage.GraphData    = Movie->PF.MFImageBuffer ;
	Movie->NowImageGraphOutAlloc = TRUE ;
	NS_CreateXRGB8ColorData( &Movie->NowImage.ColorData ) ;
	Movie->UseNowImage = &Movie->NowImage ;

	// YバッファとUVバッファの確保
	Movie->PF.MFYWidth  = Movie->PF.MFFrameSizeX ;
	Movie->PF.MFYHeight = Movie->PF.MFFrameSizeY ;
	Movie->PF.MFYStride = ( Movie->PF.MFFrameSizeX + 15 ) / 16 * 16 ;
	Movie->PF.MFYBuffer = DXALLOC( Movie->PF.MFYStride * Movie->PF.MFYHeight ) ;
	if( Movie->PF.MFYBuffer == NULL )
		goto ERR ;
	Movie->PF.MFUVWidth  = Movie->PF.MFFrameSizeX / 2 ;
	Movie->PF.MFUVHeight = Movie->PF.MFFrameSizeY / 2 ;
	Movie->PF.MFUVStride = ( Movie->PF.MFFrameSizeX + 15 ) / 16 * 16 ;
	Movie->PF.MFUVBuffer = DXALLOC( Movie->PF.MFUVStride * Movie->PF.MFUVHeight ) ;
	if( Movie->PF.MFUVBuffer == NULL )
		goto ERR ;

	if( Width  ) *Width  = ( int )Movie->PF.MFFrameSizeX ;
	if( Height ) *Height = ( int )Movie->PF.MFFrameSizeY ;

#ifndef DX_NON_SOUND
	LOADSOUND_GPARAM GParam ;

	InitLoadSoundGParam( &GParam ) ;

	// 開いたら Thera 用のセットアップを行う

	// サウンド再生用にサウンドデータとしても読み込み
	GParam.NotInitSoundMemDelete = TRUE ;
#ifndef DX_NON_OGGVORBIS
	GParam.OggVorbisFromTheoraFile = TRUE ;
#endif // DX_NON_OGGVORBIS
	GParam.DisableReadSoundFunctionMask = ~DX_READSOUNDFUNCTION_MF ;
//	if( FileName != NULL )
//	{
		GParam.CreateSoundDataType = DX_SOUNDDATATYPE_FILE ;
		Movie->PF.MFSoundHandle = LoadSoundMemBase_UseGParam( &GParam, Movie->PF.FileName, 1, -1, FALSE, ASyncThread ) ;
//	}
//	else
//	{
//		GParam.CreateSoundDataType = DX_SOUNDDATATYPE_MEMPRESS ;
//		Movie->PF.MFSoundHandle = LoadSoundMemByMemImageBase_UseGParam( &GParam, TRUE, -1, FileImage, ( int )FileImageSize, 1, -1, FALSE, ASyncThread ) ;
//	}
	Movie->PF.MFSoundTotalTime = NS_GetSoundTotalTime( Movie->PF.MFSoundHandle ) ;
	Movie->PF.MFSoundFrequency = NS_GetFrequencySoundMem( Movie->PF.MFSoundHandle ) ;

	// ループタイプの決定( 長いほうを基準にする )
//	Movie->PF.MFLoopType = Movie->PF.MFSoundTotalTime > Movie->StopTime / 10000 ? 1 : 0 ;
	Movie->PF.MFLoopType = 0 ;

#else // DX_NON_SOUND
	// ループタイプは動画データ合わせ
	Movie->PF.MFLoopType = 0 ;
#endif // DX_NON_SOUND

	// 最初のフレームをセットアップする
	{
		// イメージがセットアップされている、フラグを倒す
		Movie->PF.MFBaseImageSetup = 0 ;
#ifndef DX_NON_FILTER
		Movie->PF.MFYUVGrHandleSetup = 0 ;

		if( GSYS.HardInfo.UseShader )
		{
			MediaFoundationMovie_SetupImage( Movie, 0, 1, ASyncThread ) ;
		}
		else
#endif // DX_NON_FILTER
		{
			MediaFoundationMovie_SetupImage( Movie, 1, 0, ASyncThread ) ;
		}
	}

	// 正常終了
	return 0 ;

ERR :
	if( Movie->PF.pMFReader != NULL						){		Movie->PF.pMFReader->Release() ;						Movie->PF.pMFReader = NULL ; }
	if( Movie->PF.pMFMediaTypeVideoStream != NULL		){		Movie->PF.pMFMediaTypeVideoStream->Release() ;			Movie->PF.pMFMediaTypeVideoStream = NULL ; }
	if( Movie->PF.pMFMediaTypeOutputVideoStream != NULL	){		Movie->PF.pMFMediaTypeOutputVideoStream->Release() ;	Movie->PF.pMFMediaTypeOutputVideoStream = NULL ; }
	if( Movie->PF.MFImageBuffer != NULL					){		DXFREE( Movie->PF.MFImageBuffer ) ;						Movie->PF.MFImageBuffer = NULL ; }
	if( Movie->PF.MFYBuffer != NULL						){		DXFREE( Movie->PF.MFYBuffer ) ;							Movie->PF.MFYBuffer = NULL ; }
	if( Movie->PF.MFUVBuffer != NULL					){		DXFREE( Movie->PF.MFUVBuffer ) ;						Movie->PF.MFUVBuffer = NULL ; }

	return -1 ;
}

// カレントフレームのRGB画像を作成する( 戻り値  1:作成された  0:されなかった )
static int MediaFoundationMovie_SetupImage(
	MOVIEGRAPH * Movie,
	int BaseImage,
	int YUVGrHandle,
	int ASyncThread
)
{
	unsigned char *d, *ys, *uvs ;
	int i, j, r, g, b, ysadd, dadd, uvadd, w, h, dpitch, uvr, uvg, uvb, y, y2, y3, y4;
	HRESULT hr ;
	DWORD dwFlags = 0 ;
	D_IMFSample *pSample = NULL;
	D_IMFMediaBuffer *pBuffer = NULL;
	D_IMF2DBuffer *p2DBuffer = NULL ;
//	BYTE *pScanline ;
//	LONG Pitch ;
	BYTE *Buffer ;
	DWORD MaxLength ;
	DWORD CurrentLength ;
	LONGLONG llTimestamp ;

	// 現在のフレームの画像がセットアップされていれば何もせず終了
	if( 
#ifndef DX_NON_FILTER
		( ( YUVGrHandle && Movie->PF.MFYUVGrHandleSetup ) || YUVGrHandle == 0 || Movie->PF.MFNotUseYUVGrHandle == TRUE  ) &&
#endif // DX_NON_FILTER
		( ( BaseImage   && Movie->PF.MFBaseImageSetup   ) || BaseImage   == 0 ) )
		return 0 ;

	// イメージデータの読み込み
	if( Movie->PF.MFLastReadSampleFrame < Movie->PF.MFCurrentFrame )
	{
		for(;;)
		{
			int CurFrame ;

			if( pSample != NULL )
			{
				pSample->Release() ;
				pSample = NULL ;
			}

			hr = Movie->PF.pMFReader->ReadSample(
				( DWORD )D_MF_SOURCE_READER_FIRST_VIDEO_STREAM,
				0,
				NULL,
				&dwFlags,
				&llTimestamp,
				&pSample
			) ;
			if( hr != S_OK )
			{
				goto ERR ;
			}

			if( dwFlags & D_MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED )
			{
				continue ;
			}

			if( dwFlags & D_MF_SOURCE_READERF_ENDOFSTREAM )
			{
				goto ERR ;
			}

			if( pSample == NULL )
			{
				goto ERR ;
			}

			// フレームを算出
			{
				LONGLONG Temp64_1, Temp64_2 ;
				DWORD Temp128[ 4 ] ;

				Temp64_1 = llTimestamp + Movie->PF.FrameTime / 4 ;
				Temp64_2 = ( LONGLONG )Movie->PF.MFFrameRateNumerator * 0x1000 / Movie->PF.MFFrameRateDenominator ;
				_MUL128_1( ( DWORD * )&Temp64_1, ( DWORD * )&Temp64_2, Temp128 ) ;

				Temp64_2 = LL_NUM( 40960000000 ) /* 10000000 * 0x1000 */ ;
				_DIV128_1( Temp128, ( DWORD * )&Temp64_2, ( DWORD * )&Temp64_1 ) ;

				CurFrame = ( int )Temp64_1 ;
			}

			Movie->PF.MFLastReadSampleTimeStamp = llTimestamp ;
			Movie->PF.MFLastReadSampleFrame     = CurFrame ;

			if( CurFrame >= Movie->PF.MFCurrentFrame )
			{
				break ;
			}
			else
			{
				CurFrame = CurFrame ;
			}
		}

		hr = pSample->GetBufferByIndex( 0, &pBuffer ) ;
		if( hr != S_OK )
		{
			goto ERR ;
		}

	//	hr = pBuffer->QueryInterface( IID_IMF2DBUFFER, ( void ** )&p2DBuffer ) ;
	//	if( hr != S_OK )
	//	{
	//		goto ERR ;
	//	}
	//
	//	hr = p2DBuffer->Lock2D( &pScanline, &Pitch ) ;
	//	if( hr != S_OK )
	//	{
	//		goto ERR ;
	//	}

		hr = pBuffer->Lock( &Buffer, &MaxLength, &CurrentLength ) ;
		if( hr != S_OK )
		{
			goto ERR ;
		}

		// バッファにデータをコピーする
		{
			BYTE *yimage ;
			BYTE *uimage ;
			BYTE *vimage ;
			DWORD uvstride ;

			uvstride = ( Movie->PF.MFFrameSizeX / 2 + 7 ) / 8 * 8 ;

			yimage = ( BYTE * )Buffer ;
			uimage = yimage + Movie->PF.MFYStride  * ( ( Movie->PF.MFFrameSizeY + 15 ) / 16 * 16 ) ;
			vimage = uimage + uvstride * ( ( Movie->PF.MFFrameSizeY / 2 + 7 ) / 8 * 8 ) ;
			_MEMCPY( Movie->PF.MFYBuffer, yimage, ( size_t )( Movie->PF.MFYStride * Movie->PF.MFFrameSizeY ) ) ;
			{
				DWORD n, m ;
				DWORD bw ;
				DWORD ow ;
				BYTE *dst ;
				BYTE *u, *v ;

				bw = Movie->PF.MFUVWidth / 8 ;
				ow = Movie->PF.MFUVWidth % 8 ;

				dst = ( BYTE * )Movie->PF.MFUVBuffer ;
				u = uimage ;
				v = vimage ;
				for( n = 0 ; n < ( DWORD )Movie->PF.MFUVHeight ; n ++, dst += Movie->PF.MFUVStride, u += uvstride, v += uvstride )
				{
					BYTE *u_t, *v_t ;
					BYTE *dst_t ;

					dst_t = dst ;
					u_t = u ;
					v_t = v ;
					for( m = 0 ; m < bw ; m ++, dst_t += 2 * 8, u_t += 8, v_t += 8 )
					{
						dst_t[  0 ] = u_t[ 0 ] ;		dst_t[  1 ] = v_t[ 0 ] ;
						dst_t[  2 ] = u_t[ 1 ] ;		dst_t[  3 ] = v_t[ 1 ] ;
						dst_t[  4 ] = u_t[ 2 ] ;		dst_t[  5 ] = v_t[ 2 ] ;
						dst_t[  6 ] = u_t[ 3 ] ;		dst_t[  7 ] = v_t[ 3 ] ;
						dst_t[  8 ] = u_t[ 4 ] ;		dst_t[  9 ] = v_t[ 4 ] ;
						dst_t[ 10 ] = u_t[ 5 ] ;		dst_t[ 11 ] = v_t[ 5 ] ;
						dst_t[ 12 ] = u_t[ 6 ] ;		dst_t[ 13 ] = v_t[ 6 ] ;
						dst_t[ 14 ] = u_t[ 7 ] ;		dst_t[ 15 ] = v_t[ 7 ] ;
					}
					for( m = 0 ; m < ow ; m ++, dst_t += 2, u_t ++, v_t ++ )
					{
						dst_t[ 0 ] = *u_t ;
						dst_t[ 1 ] = *v_t ;
					}
				}
			}
		}

		if( pBuffer->Unlock() != S_OK )
		{
			goto ERR ;
		}
	}

#ifndef DX_NON_FILTER
	// シェーダーが使用できる場合はYUVカラーのグラフィックハンドルを使用する
	if( YUVGrHandle && Movie->PF.MFNotUseYUVGrHandle == FALSE && GetValidShaderVersion() >= 200 )
	{
		SETUP_GRAPHHANDLE_GPARAM GParam ;
		BASEIMAGE BaseImage_ ;
		RECT SrcRect ;

		// グラフィックハンドルがまだ作成されていなかったら作成する
		if( Movie->YGrHandle == -1 )
		{
			Graphics_Image_InitSetupGraphHandleGParam( &GParam ) ;

			Graphics_Image_InitSetupGraphHandleGParam_Normal_NonDrawValid( &GParam, 32, FALSE ) ;
			GParam.CreateImageChannelNum      = 1 ;
			GParam.CreateImageChannelBitDepth = 8 ;
			Movie->YGrHandle = Graphics_Image_MakeGraph_UseGParam( &GParam, Movie->PF.MFYWidth, Movie->PF.MFYHeight, FALSE, FALSE, 0, FALSE, ASyncThread ) ;
			NS_SetDeleteHandleFlag( Movie->YGrHandle, ( int * )&Movie->YGrHandle ) ;
		}
		if( Movie->UVGrHandle == -1 )
		{
			Graphics_Image_InitSetupGraphHandleGParam( &GParam ) ;

			Graphics_Image_InitSetupGraphHandleGParam_Normal_NonDrawValid( &GParam, 32, FALSE ) ;
			GParam.CreateImageChannelNum      = 2 ;
			GParam.CreateImageChannelBitDepth = 8 ;
			Movie->UVGrHandle     = Graphics_Image_MakeGraph_UseGParam( &GParam, Movie->PF.MFUVWidth, Movie->PF.MFUVHeight, FALSE, FALSE, 0, FALSE, ASyncThread ) ;
			NS_SetDeleteHandleFlag( Movie->UVGrHandle, ( int * )&Movie->UVGrHandle ) ;
		}

		_MEMSET( &BaseImage_, 0, sizeof( BaseImage_ ) ) ;
		BaseImage_.Width                     = Movie->PF.MFYWidth ;
		BaseImage_.Height                    = Movie->PF.MFYHeight ;
		BaseImage_.Pitch                     = Movie->PF.MFYStride ;
		BaseImage_.GraphData                 = Movie->PF.MFYBuffer ;
		BaseImage_.ColorData.Format          = DX_BASEIMAGE_FORMAT_NORMAL ;
		BaseImage_.ColorData.PixelByte       = 1 ;
		BaseImage_.ColorData.ChannelNum      = 1 ;
		BaseImage_.ColorData.ChannelBitDepth = 8 ;
		SETRECT( SrcRect, 0, 0, BaseImage_.Width, BaseImage_.Height ) ;
		Graphics_Image_BltBmpOrBaseImageToGraph3(
			&SrcRect,
			0,
			0,
			Movie->YGrHandle,
			&BaseImage_,
			NULL,
			FALSE,
			FALSE,
			FALSE,
			ASyncThread
		) ;

		BaseImage_.Width                     = Movie->PF.MFUVWidth ;
		BaseImage_.Height                    = Movie->PF.MFUVHeight ;
		BaseImage_.Pitch                     = Movie->PF.MFUVStride ;
		BaseImage_.GraphData                 = Movie->PF.MFUVBuffer ;
		BaseImage_.ColorData.Format          = DX_BASEIMAGE_FORMAT_NORMAL ;
		BaseImage_.ColorData.PixelByte       = 2 ;
		BaseImage_.ColorData.ChannelNum      = 2 ;
		BaseImage_.ColorData.ChannelBitDepth = 8 ;
		SETRECT( SrcRect, 0, 0, BaseImage_.Width, BaseImage_.Height ) ;
		Graphics_Image_BltBmpOrBaseImageToGraph3(
			&SrcRect,
			0,
			0,
			Movie->UVGrHandle,
			&BaseImage_,
			NULL,
			FALSE,
			FALSE,
			FALSE,
			ASyncThread
		) ;

		// セットアップフラグを立てる
		Movie->PF.MFYUVGrHandleSetup = 1 ;
	}
#endif // DX_NON_FILTER

	// ＲＧＢイメージのセットアップ指定があって、まだ作成されていなかったら作成する
	if( BaseImage && Movie->PF.MFBaseImageSetup == 0 )
	{
		// BASEIMAGE の場合
		d  = ( unsigned char * )Movie->NowImage.GraphData ;
		ys = ( unsigned char * )Movie->PF.MFYBuffer ;
		uvs = ( unsigned char * )Movie->PF.MFUVBuffer ;

		// yuv 情報を rgb データに変換
		if( Movie->PF.MFYWidth  == Movie->PF.MFUVWidth  * 2 &&
			Movie->PF.MFYHeight == Movie->PF.MFUVHeight * 2 )
		{
			ysadd  = Movie->PF.MFYStride  * 2 - Movie->PF.MFYWidth ;
			uvadd  = Movie->PF.MFUVStride     - Movie->PF.MFUVWidth * 2 ;
			dadd   = Movie->NowImage.Pitch * 2 - Movie->PF.MFYWidth * 4 ;
			dpitch = Movie->NowImage.Pitch ;
			w = Movie->PF.MFYWidth  / 2 ;
			h = Movie->PF.MFYHeight / 2 ;
			for( i = 0; i < h; i++, d += dadd, ys += ysadd, uvs += uvadd )
			{
				for( j = 0; j < w; j ++, d += 8, ys += 2, uvs += 2 )
				{
					uvr =                              YUVTable[ YUV_RV ][ uvs[ 1 ] ] ;
					uvg = YUVTable[ YUV_GU ][ uvs[ 0 ] ] + YUVTable[ YUV_GV ][ uvs[ 1 ] ] ;
					uvb = YUVTable[ YUV_BU ][ uvs[ 0 ] ]                              ;

					y  = YUVTable[ YUV_Y ][ ys[ 0 ] ] ;
					y2 = YUVTable[ YUV_Y ][ ys[ 1 ] ] ;
					y3 = YUVTable[ YUV_Y ][ ys[ Movie->PF.MFYStride ] ] ;
					y4 = YUVTable[ YUV_Y ][ ys[ Movie->PF.MFYStride + 1 ] ] ;

					d[2]              = YUVLimitTable[ ( ( y  + uvr ) >> 14 ) + 512 ] ;
					d[1]              = YUVLimitTable[ ( ( y  + uvg ) >> 14 ) + 512 ] ;
					d[0]              = YUVLimitTable[ ( ( y  + uvb ) >> 14 ) + 512 ] ;
					d[3]              = 255;

					d[2 + 4]          = YUVLimitTable[ ( ( y2 + uvr ) >> 14 ) + 512 ] ;
					d[1 + 4]          = YUVLimitTable[ ( ( y2 + uvg ) >> 14 ) + 512 ] ;
					d[0 + 4]          = YUVLimitTable[ ( ( y2 + uvb ) >> 14 ) + 512 ] ;
					d[3 + 4]          = 255;

					d[2 + dpitch]     = YUVLimitTable[ ( ( y3 + uvr ) >> 14 ) + 512 ] ;
					d[1 + dpitch]     = YUVLimitTable[ ( ( y3 + uvg ) >> 14 ) + 512 ] ;
					d[0 + dpitch]     = YUVLimitTable[ ( ( y3 + uvb ) >> 14 ) + 512 ] ;
					d[3 + dpitch]     = 255;

					d[2 + dpitch + 4] = YUVLimitTable[ ( ( y4 + uvr ) >> 14 ) + 512 ] ;
					d[1 + dpitch + 4] = YUVLimitTable[ ( ( y4 + uvg ) >> 14 ) + 512 ] ;
					d[0 + dpitch + 4] = YUVLimitTable[ ( ( y4 + uvb ) >> 14 ) + 512 ] ;
					d[3 + dpitch + 4] = 255;
				}
			}
		}
		else
		{
			ysadd = Movie->PF.MFYStride - Movie->PF.MFYWidth;
			dadd  = Movie->NowImage.Pitch - Movie->PF.MFYWidth * 4;
			for( i = 0; ( DWORD )i < Movie->PF.MFYHeight; i++, d += dadd, ys += ysadd )
			{
				for( j = 0; ( DWORD )j < Movie->PF.MFYWidth; j ++, d += 4, ys++ )
				{
					uvs = ( BYTE * )Movie->PF.MFUVBuffer + ( i / 2 ) * Movie->PF.MFUVStride + j / 2 * 2 ;

					r = _FTOL(1.164f * (*ys-16)                         + 1.596f * (uvs[1]-128));
					g = _FTOL(1.164f * (*ys-16) - 0.391f * (uvs[0]-128) - 0.813f * (uvs[1]-128));
					b = _FTOL(1.164f * (*ys-16) + 2.018f * (uvs[0]-128));
					if( r < 0 ) r = 0; else if( r > 255 ) r = 255;
					if( g < 0 ) g = 0; else if( g > 255 ) g = 255;
					if( b < 0 ) b = 0; else if( b > 255 ) b = 255;
					d[2] = ( unsigned char )r;
					d[1] = ( unsigned char )g;
					d[0] = ( unsigned char )b;
					d[3] = 255;
				}
			}
		}

		// セットアップフラグを立てる
		Movie->PF.MFBaseImageSetup = 1 ;
	}

	if( pSample != NULL )
	{
		pSample->Release() ;
		pSample = NULL ;
	}

	if( p2DBuffer != NULL )
	{
		p2DBuffer->Release() ;
		p2DBuffer = NULL ;
	}

	if( pBuffer != NULL )
	{
		pBuffer->Release() ;
		pBuffer = NULL ;
	}

	// 正常終了
	return 0 ;

ERR :
	if( pSample != NULL )
	{
		pSample->Release() ;
		pSample = NULL ;
	}

	if( p2DBuffer != NULL )
	{
		p2DBuffer->Release() ;
		p2DBuffer = NULL ;
	}

	if( pBuffer != NULL )
	{
		pBuffer->Release() ;
		pBuffer = NULL ;
	}

	return -1 ;
}

// Media Foundation を使用した動画ファイルのフレームを進める
static int MediaFoundationMovie_IncToFrame(
	MOVIEGRAPH * Movie,
	int AddFrame
)
{
	// 進めるフレーム数が 0 の場合は何もせず終了
	if( AddFrame <= 0 )
	{
		return 0 ;
	}

	// カレントフレームを変更
	Movie->PF.MFCurrentFrame += AddFrame ;

	// 要求されるフレームがデコード済みのフレームより番号が若い場合はセットアップフラグを倒さない
	if( Movie->PF.MFLastReadSampleFrame < Movie->PF.MFCurrentFrame )
	{
		// イメージがセットアップされている、フラグを倒す
		Movie->PF.MFBaseImageSetup = 0 ;
#ifndef DX_NON_FILTER
		Movie->PF.MFYUVGrHandleSetup = 0 ;

		if( GSYS.HardInfo.UseShader )
		{
			MediaFoundationMovie_SetupImage( Movie, 0, 1, FALSE ) ;
		}
		else
#endif // DX_NON_FILTER
		{
			MediaFoundationMovie_SetupImage( Movie, 1, 0, FALSE ) ;
		}
	}

	// 正常終了
	return 0 ;
}

// Media Foundation を使用するかどうかを設定する( TRUE:使用する( デフォルト )  FALSE:使用しない )
extern int NS_SetUseMediaFoundationFlag( int Flag )
{
	// フラグを保存
	MovieGraphManageData.PF.DisableMediaFoundation = Flag ? FALSE : TRUE ;

	// 正常終了
	return 0 ;
}

#endif // DX_NON_MEDIA_FOUNDATION

#ifndef DX_NON_DSHOW_MOVIE

// DirectShow を使用した動画ファイルのオープンを試みる
extern int DirectShowOpenMovieFile(
	MOVIEGRAPH * Movie,
	const wchar_t *FileName,
	int *Width,
	int *Height,
	int SurfaceMode,
	int /*ImageSizeGetOnly*/,
	int ASyncThread
)
{
	wchar_t ErStr[256] ;
	D_IAMMultiMediaStream *pAMStream = NULL;
	HRESULT hr ;

	SETUP_WIN_API

	Movie->NowImage.GraphData = NULL;
	Movie->YUVFlag = FALSE ;
	Movie->OverlayDestX = 0 ;
	Movie->OverlayDestY = 0 ;
	Movie->OverlayDestExRate = 0 ;
	Movie->OverlayDispFlag = 0 ;
	Movie->FirstUpdateFlag = FALSE ;
	_MEMSET( &Movie->OverlaySrcRect, 0, sizeof( RECT ) ) ;
	_MEMSET( &Movie->OverlayDestRect, 0, sizeof( RECT ) ) ;

	// もしオーバーレイが使えない場合はフルカラーにする
	if( SurfaceMode == DX_MOVIESURFACE_OVERLAY  )
	{
		SurfaceMode = DX_MOVIESURFACE_FULLCOLOR ;
	}

	// もし画面が３２ビットカラーモードでフルカラーモードを指定してきた場合はノーマルにする
	if( SurfaceMode == DX_MOVIESURFACE_FULLCOLOR && NS_GetColorBitDepth() == 32 ) SurfaceMode = DX_MOVIESURFACE_NORMAL ;

	SurfaceMode = DX_MOVIESURFACE_FULLCOLOR ;

	// グラフィックビルダーオブジェクトの作成
	if( ( FAILED( WinAPI_CoCreateInstance_ASync(CLSID_FILTERGRAPH, NULL, CLSCTX_INPROC, IID_IGRAPHBUILDER, (void **)&Movie->PF.pGraph, ASyncThread ) ) ) )
	{
		_WCSCPY_S( ErStr, sizeof( ErStr ), L"CoCreateInstance Error : CLSID_FilterGraph\n" ) ;
		goto ERROR_R ;
	}

	// Create the Texture Renderer object
	Movie->PF.pMovieImage = New_D_CMovieRender( NULL, &hr ) ;
    
    // Get a pointer to the IBaseFilter on the TextureRenderer, add it to graph
	hr = Movie->PF.pGraph->AddFilter( Movie->PF.pMovieImage, L"MovieRenderer" ) ;
    if( FAILED( hr ) )
    {
        _WCSCPY_S( ErStr, sizeof( ErStr ), L"Could not add renderer filter to graph!\n" ) ;
        return hr;
    }

	// BasicAudio インターフェイスを得る
	if( FAILED( Movie->PF.pGraph->QueryInterface( IID_IBASICAUDIO, ( void ** )&Movie->PF.pBasicAudio ) ) )
	{
		_WCSCPY_S( ErStr, sizeof( ErStr ), L"QueryInterface Error : IID_IBasicAudio\n" ) ;
		goto ERROR_R ;
	}

	// メディアコントローラオブジェクトを取得する
	if( FAILED( Movie->PF.pGraph->QueryInterface( IID_IMEDIACONTROL, ( void ** )&Movie->PF.pMediaControl ) ) )
	{
		_WCSCPY_S( ErStr, sizeof( ErStr ), L"QueryInterface Error : IID_IMediaControl\n" ) ;
		goto ERROR_R ;
	}

	// メディアシーキングオブジェクトを取得する
	if( FAILED( Movie->PF.pGraph->QueryInterface( IID_IMEDIASEEKING, ( void ** )&Movie->PF.pMediaSeeking ) ) )
	{
		_WCSCPY_S( ErStr, sizeof( ErStr ), L"QueryInterface Error : IID_IMediaSeeking\n" ) ;
		goto ERROR_R ;
	}

   	// ファイル名保存
	_WCSCPY_S( Movie->PF.FileName, sizeof( Movie->PF.FileName ), FileName ) ;

	// ディレクトリの区切りが / になっている箇所を \ に置き換える
	{
		wchar_t *wp ;

		for( wp = Movie->PF.FileName; *wp; wp++ )
		{
			if( *wp == L'/' )
			{
				*wp = '\\' ;
			}
		}
	}

	hr = Movie->PF.pGraph->RenderFile( Movie->PF.FileName, NULL ) ;
	if( FAILED( hr ) )
	{
		_WCSCPY_S( ErStr, sizeof( ErStr ), L"RenderFile faired!\n" ) ;

		DWORD_PTR fp ;
		HANDLE FileHandle ;
		void *TempBuffer ;
		size_t CompSize, MoveSize ;
		DWORD WriteSize ;
		size_t FileSize ;
		const DWORD BufferSize = 0x100000 ;

		_WCSCPY_S( ErStr, sizeof( ErStr ), L"Movie File Open Error : " ) ;
		_WCSCAT_S( ErStr, sizeof( ErStr ), FileName ) ;
		_WCSCAT_S( ErStr, sizeof( ErStr ), L"\n" ) ;

		// 既にテンポラリファイルを作成している場合は、
		// 更にテンポラリファイルを作成することはしない
		if( Movie->PF.UseTemporaryFile == TRUE )
			goto ERROR_R ;

		// ファイルが開けなかったらアーカイブされている可能性がある
#ifdef UNICODE
		fp = DX_FOPEN( Movie->PF.FileName ) ;
#else
		fp = DX_FOPEN( FileName ) ;
#endif
		if( fp == 0 ) goto ERROR_R ;
		
		// 開けた場合はテンポラリファイルに書き出す
		{
			// ファイルサイズの取得
			DX_FSEEK( fp, 0L, SEEK_END ) ;
			FileSize = ( size_t )DX_FTELL( fp ) ;
			DX_FSEEK( fp, 0L, SEEK_SET ) ;

			// 一時的にデータを格納するバッファを確保
			TempBuffer = DXALLOC( BufferSize ) ;
			if( TempBuffer == NULL )
			{
				DX_FCLOSE( fp ) ;
				goto ERROR_R ;
			}

			// テンポラリファイルの作成
			FileHandle = CreateTemporaryFile( Movie->PF.FileName, sizeof( Movie->PF.FileName ) ) ;

			if( FileHandle == NULL )
			{
				DX_FCLOSE( fp ) ;
				DXFREE( TempBuffer ) ;
				goto ERROR_R ;
			}
			Movie->PF.UseTemporaryFile = TRUE ;

			// テンポラリファイルにデータを書き出す
			CompSize = 0 ;
			while( CompSize < FileSize )
			{
				MoveSize = CompSize - FileSize ;
				if( MoveSize > BufferSize ) MoveSize = BufferSize ;

				DX_FREAD( TempBuffer, MoveSize, 1, fp ) ;
				WinAPIData.Win32Func.WriteFileFunc( FileHandle, TempBuffer, ( DWORD )MoveSize, &WriteSize, NULL ) ;

				if( MoveSize != WriteSize ) break ;
				CompSize += MoveSize ;
			}

			// ファイルを閉じ、メモリを解放する
			DX_FCLOSE( fp ) ;
			WinAPIData.Win32Func.CloseHandleFunc( FileHandle ) ;
			DXFREE( TempBuffer ) ;
		}

		// 改めてファイルを開く
		hr = Movie->PF.pGraph->RenderFile( Movie->PF.FileName, NULL ) ;
		if( FAILED( hr ) )
		{
			// テンポラリファイルを削除
			WinAPIData.Win32Func.DeleteFileWFunc( Movie->PF.FileName ) ;
			goto ERROR_R ;
		}
	}

	// １フレームあたりの時間を得る
	Movie->PF.pMediaSeeking->GetDuration( &Movie->PF.FrameTime ) ;
	if( Movie->PF.FrameTime == 0 )
	{
		Movie->PF.FrameTime = 10000000 / 60 ;
	}

	// 終了時間を取得する
	Movie->PF.pMediaSeeking->GetStopPosition( &Movie->StopTime ) ;

	// 画像イメージの情報をセットする
	Movie->NowImage.Width        = ( int )Movie->PF.pMovieImage->Width ;
	Movie->NowImage.Height       = ( int )Movie->PF.pMovieImage->Height ;
	Movie->NowImage.Pitch        = ( int )Movie->PF.pMovieImage->Pitch ;
	Movie->NowImage.GraphData    = Movie->PF.pMovieImage->ImageBuffer ;
	Movie->NowImageGraphOutAlloc = TRUE ;
	if( Movie->PF.pMovieImage->ImageType == 0 )
	{
		NS_CreateFullColorData( &Movie->NowImage.ColorData ) ;
	}
	else
	if( Movie->PF.pMovieImage->ImageType == 1 && Movie->A8R8G8B8Flag )
	{
		NS_CreateARGB8ColorData( &Movie->NowImage.ColorData ) ;
	}
	else
	{
		NS_CreateXRGB8ColorData( &Movie->NowImage.ColorData ) ;
	}
	Movie->UseNowImage = &Movie->NowImage ;

	if( Width  ) *Width  = ( int )Movie->PF.pMovieImage->Width ;
	if( Height ) *Height = ( int )Movie->PF.pMovieImage->Height ;

	// 終了
	return 0 ;


ERROR_R:

	// 各種ＣＯＭオブジェクトを終了する
	if( pAMStream					){ pAMStream->Release()					; pAMStream = NULL ; }

	if( Movie->PF.pGraph			){ Movie->PF.pGraph->Release()			; Movie->PF.pGraph = NULL ; }
	if( Movie->PF.pMediaControl		){ Movie->PF.pMediaControl->Release()	; Movie->PF.pMediaControl = NULL ; }
	if( Movie->PF.pMediaSeeking		){ Movie->PF.pMediaSeeking->Release()	; Movie->PF.pMediaSeeking = NULL ; }
	if( Movie->PF.pBasicAudio		){ Movie->PF.pBasicAudio->Release()		; Movie->PF.pBasicAudio = NULL ; }

	return DXST_LOGFILE_ADDW( ErStr ) ;
}

#endif // DX_NON_DSHOW_MOVIE


// ムービーハンドルの後始末を行う
extern int TerminateMovieHandle_PF( HANDLEINFO *HandleInfo )
{
	MOVIEGRAPH *Movie = ( MOVIEGRAPH * )HandleInfo ;

#if !defined( DX_NON_MEDIA_FOUNDATION ) || !defined( DX_NON_DSHOW_MOVIE )
	SETUP_WIN_API

	// もしテンポラリファイルを使用していた場合はテンポラリファイルを削除する
	if( Movie->PF.UseTemporaryFile == TRUE )
	{
		WinAPIData.Win32Func.DeleteFileWFunc( Movie->PF.FileName ) ;
		Movie->PF.UseTemporaryFile = FALSE ;
	}
#endif // !defined( DX_NON_MEDIA_FOUNDATION ) || !defined( DX_NON_DSHOW_MOVIE )

#ifndef DX_NON_MEDIA_FOUNDATION
#ifndef DX_NON_SOUND
	if( Movie->PF.MFSoundHandle > 0 )
	{
		NS_DeleteSoundMem( Movie->PF.MFSoundHandle ) ;
		Movie->PF.MFSoundHandle = 0 ;
	}
#endif // DX_NON_SOUND

	if( Movie->PF.pMFReader != NULL						){		Movie->PF.pMFReader->Release() ;						Movie->PF.pMFReader = NULL ; }
	if( Movie->PF.pMFMediaTypeVideoStream != NULL		){		Movie->PF.pMFMediaTypeVideoStream->Release() ;			Movie->PF.pMFMediaTypeVideoStream = NULL ; }
	if( Movie->PF.pMFMediaTypeOutputVideoStream != NULL	){		Movie->PF.pMFMediaTypeOutputVideoStream->Release() ;	Movie->PF.pMFMediaTypeOutputVideoStream = NULL ; }
	if( Movie->PF.MFImageBuffer != NULL					){		DXFREE( Movie->PF.MFImageBuffer ) ;						Movie->PF.MFImageBuffer = NULL ; }
	if( Movie->PF.MFYBuffer != NULL						){		DXFREE( Movie->PF.MFYBuffer ) ;							Movie->PF.MFYBuffer = NULL ; }
	if( Movie->PF.MFUVBuffer != NULL					){		DXFREE( Movie->PF.MFUVBuffer ) ;						Movie->PF.MFUVBuffer = NULL ; }
#endif // DX_NON_MEDIA_FOUNDATION

#ifndef DX_NON_DSHOW_MOVIE
	if( Movie->PF.pBasicAudio )		{ Movie->PF.pBasicAudio->Release() ; 		Movie->PF.pBasicAudio = NULL ; }
	if( Movie->PF.pMediaSeeking )	{ Movie->PF.pMediaSeeking->Release() ; 		Movie->PF.pMediaSeeking = NULL ; }
	if( Movie->PF.pMediaControl )	{ Movie->PF.pMediaControl->Release(); 		Movie->PF.pMediaControl = NULL ; }
	if( Movie->PF.pGraph )			{ Movie->PF.pGraph->Release(); 				Movie->PF.pGraph = NULL ; }
	if( Movie->PF.pMovieImage )		{ delete Movie->PF.pMovieImage ;			Movie->PF.pMovieImage = NULL ; }
#endif // DX_NON_DSHOW_MOVIE

	// 正常終了
	return 0 ;
}


// OpenMovie のグローバル変数にアクセスしないバージョンの環境依存処理
extern int OpenMovie_UseGParam_PF( MOVIEGRAPH * Movie, OPENMOVIE_GPARAM * /*GParam*/, const wchar_t *FileName, int *Width, int *Height, int SurfaceMode, int ImageSizeGetOnly, int ASyncThread )
{
	int Flag = FALSE ;

#ifndef DX_NON_MEDIA_FOUNDATION
	// Media Foundation によるファイルのオープンを試みる
	if( Flag == FALSE && MovieGraphManageData.PF.DisableMediaFoundation == FALSE )
	{
		Movie->PF.UseTemporaryFile = FALSE ;
		if( MediaFoundationOpenMovieFile( Movie, FileName, Width, Height, ImageSizeGetOnly, ASyncThread ) != -1 )
		{
			Flag = TRUE ;
		}
	}
#endif // DX_NON_MEDIA_FOUNDATION

#ifndef DX_NON_DSHOW_MOVIE
	// DirectShow によるファイルのオープンを試みる
	if( Flag == FALSE )
	{
		Movie->PF.UseTemporaryFile = FALSE ;
		if( DirectShowOpenMovieFile( Movie, FileName, Width, Height,SurfaceMode, ImageSizeGetOnly, ASyncThread ) != -1 )
		{
			Flag = TRUE ;
		}
	}
#endif // DX_NON_DSHOW_MOVIE

	if( Flag == FALSE )
	{
		return -1 ;
	}

	// 成功
	return 0 ;
}

// ムービー用のグラフィックハンドル作成用のパラメータをカスタマイズする
extern int OpenMovie_CustomGraphHandleGParam_PF( MOVIEGRAPH * /*Movie*/, struct SETUP_GRAPHHANDLE_GPARAM * /*InitGraphHandleGParam*/ )
{
	return 0 ;
}

// ムービーの再生を開始する処理の環境依存処理
extern int PlayMovie__PF( MOVIEGRAPH *Movie, int PlayType, int /*SysPlay*/ )
{
#ifndef DX_NON_MEDIA_FOUNDATION
	if( Movie->PF.pMFReader != NULL )
	{
		// １フレーム目をセットアップ
#ifndef DX_NON_FILTER
		if( GSYS.HardInfo.UseShader )
		{
			MediaFoundationMovie_SetupImage( Movie, 0, 1, FALSE ) ;
		}
		else
#endif // DX_NON_FILTER
		{
			MediaFoundationMovie_SetupImage( Movie, 1, 0, FALSE ) ;
		}

		// サウンドの再生も開始する
#ifndef DX_NON_SOUND
		NS_PlaySoundMem( Movie->PF.MFSoundHandle, Movie->PF.MFLoopType == 1 ? PlayType : DX_PLAYTYPE_BACK, FALSE ) ;
#endif // DX_NON_SOUND

		// 再生開始時の時間を取得
		Movie->PF.MFPrevTimeCount = NS_GetNowHiPerformanceCount() ;

		// 再生時間をセット
		Movie->PF.MFPlayNowTime = Movie->PF.MFFrameRateDenominator * ( LONGLONG )Movie->PF.MFCurrentFrame * ( LONGLONG )1000000 / Movie->PF.MFFrameRateNumerator ;

		// 正常終了
		return 0 ;
	}
#endif // DX_NON_MEDIA_FOUNDATION

#ifndef DX_NON_DSHOW_MOVIE
	// DirectShow による再生を開始
	if( Movie->PF.pMediaControl != NULL )
	{
		Movie->PF.pMediaControl->Run() ;
	}
#endif // DX_NON_DSHOW_MOVIE

	// 正常終了
	return 0 ;
}

// ムービーの再生をストップする処理の環境依存処理
extern 	int PauseMovie_PF( MOVIEGRAPH * Movie, int /*SysPause*/ )
{
#ifndef DX_NON_MEDIA_FOUNDATION
	if( Movie->PF.pMFReader != NULL )
	{
		// サウンドの再生を停止する
#ifndef DX_NON_SOUND
		NS_StopSoundMem( Movie->PF.MFSoundHandle ) ;
#endif // DX_NON_SOUND

		// 現在の再生時間分までフレームを進めておく
		UpdateMovie( Movie->HandleInfo.Handle ) ;

		return 0 ;
	}
#endif // DX_NON_MEDIA_FOUNDATION


#ifndef DX_NON_DSHOW_MOVIE
	if( Movie->PF.pMediaControl != NULL )
	{
		// 停止
		Movie->PF.pMediaControl->Pause() ;

		// 正常終了
		return 0 ;
	}
#endif // DX_NON_DSHOW_MOVIE

	return -1 ; 
}

// ムービーの再生位置を設定する(ミリ秒単位)処理の環境依存処理
extern int SeekMovie_PF( MOVIEGRAPH * Movie, int Time )
{
#ifndef DX_NON_MEDIA_FOUNDATION
	if( Movie->PF.pMFReader != NULL )
	{
		D_PROPVARIANT variant ;

		_MEMSET( &variant, 0, sizeof( variant ) ) ;
		variant.vt = D_VT_I8 ;
		variant.hVal.QuadPart = ( ULONGLONG )Time * 10000 ;
		Movie->PF.pMFReader->SetCurrentPosition( _GUID_NULL, variant ) ;

		// 再生フレームを算出
		{
			LONGLONG Temp64_1, Temp64_2 ;
			DWORD Temp128[ 4 ] ;

			Temp64_1 = ( LONGLONG )Time * 1000 ;
			Temp64_2 = ( LONGLONG )Movie->PF.MFFrameRateNumerator * 0x10000 / Movie->PF.MFFrameRateDenominator ;
			_MUL128_1( ( DWORD * )&Temp64_1, ( DWORD * )&Temp64_2, Temp128 ) ;

			Temp64_2 = LL_NUM( 65536000000 ) /* 1000000 * 0x10000 */ ;
			_DIV128_1( Temp128, ( DWORD * )&Temp64_2, ( DWORD * )&Temp64_1 ) ;

			Movie->PF.MFCurrentFrame = ( int )Temp64_1 ;
		}

		// 再生開始タイムを変更する
		Movie->PF.MFPlayNowTime = Movie->PF.MFFrameRateDenominator * ( LONGLONG )Movie->PF.MFCurrentFrame * ( LONGLONG )1000000 / Movie->PF.MFFrameRateNumerator ;

		// 最後の ReadSample の時間、フレームをリセット
		Movie->PF.MFLastReadSampleTimeStamp = -1 ;
		Movie->PF.MFLastReadSampleFrame = -1 ;

		// イメージがセットアップされている、フラグを倒す
		Movie->PF.MFBaseImageSetup = 0 ;
#ifndef DX_NON_FILTER
		Movie->PF.MFYUVGrHandleSetup = 0 ;
#endif // DX_NON_FILTER
	
		// 再生位置を変更する
#ifndef DX_NON_SOUND
		NS_SetSoundCurrentTime( ( int )( Movie->PF.MFPlayNowTime / 1000 ), Movie->PF.MFSoundHandle ) ;
#endif // DX_NON_SOUND

		// シーク後の１フレーム目をセットアップ
#ifndef DX_NON_FILTER
		if( GSYS.HardInfo.UseShader )
		{
			MediaFoundationMovie_SetupImage( Movie, 0, 1, FALSE ) ;
		}
		else
#endif // DX_NON_FILTER
		{
			MediaFoundationMovie_SetupImage( Movie, 1, 0, FALSE ) ;
		}

		return 0 ;
	}
#endif // DX_NON_MEDIA_FOUNDATION


#ifndef DX_NON_DSHOW_MOVIE
	if( Movie->PF.pMediaSeeking != NULL )
	{
		LONGLONG Now, Stop ;

		Now = ( LONGLONG )Time * 10000 ;
		Stop = 0 ;
		Movie->PF.pMediaSeeking->SetPositions( &Now, D_AM_SEEKING_AbsolutePositioning, &Stop, D_AM_SEEKING_NoPositioning ) ;

		// 正常終了
		return 0 ;
	}

#endif // DX_NON_DSHOW_MOVIE

	return -1 ;
}

// ムービーの再生速度を設定する( 1.0 = 等倍速  2.0 = ２倍速 )処理の環境依存処理
extern int SetPlaySpeedRateMovie_PF( MOVIEGRAPH * Movie, double SpeedRate )
{
#ifndef DX_NON_MEDIA_FOUNDATION
	if( Movie->PF.pMFReader != NULL )
	{
		Movie->PF.MFPlaySpeedRate = SpeedRate ;
#ifndef DX_NON_SOUND
		if( Movie->PF.MFSoundHandle != -1 )
		{
			NS_SetFrequencySoundMem( _DTOL( Movie->PF.MFSoundFrequency * Movie->PF.MFPlaySpeedRate ), Movie->PF.MFSoundHandle ) ;
		}
		else
#endif // DX_NON_SOUND
		{
		}

		return 0 ;
	}
#endif // DX_NON_MEDIA_FOUNDATION

#ifndef DX_NON_DSHOW_MOVIE
	if( Movie->PF.pMediaSeeking != NULL )
	{
		Movie->PF.pMediaSeeking->SetRate( SpeedRate ) ;

		// 正常終了
		return 0 ;
	}

#endif // DX_NON_DSHOW_MOVIE

	return -1 ;
}

// ムービーの再生状態を得る処理の環境依存処理
extern int GetMovieState_PF( MOVIEGRAPH * Movie )
{
#ifndef DX_NON_MEDIA_FOUNDATION
	if( Movie->PF.pMFReader != NULL )
	{
		return Movie->PlayFlag ;
	}
#endif // DX_NON_MEDIA_FOUNDATION

#ifndef DX_NON_DSHOW_MOVIE
	D_OAFilterState state ;
	LONGLONG Current ;

	if( Movie->PF.pMediaSeeking == NULL )
	{
		return Movie->PlayFlag ; 
	}

	if( Movie->PF.pMediaControl->GetState( 1000, &state ) != S_OK )
	{
		return Movie->PlayFlag ;
	}

	Movie->PF.pMediaSeeking->GetCurrentPosition( &Current ) ;
	if( ( Movie->PlayType & DX_PLAYTYPE_LOOPBIT ) == 0 && Current >= Movie->StopTime && state == D_State_Stopped )
	{
		Movie->PlayFlag = FALSE ;

		// 内部一時停止フラグを立てる
		Movie->SysPauseFlag = 1 ;
	}
#endif // DX_NON_DSHOW_MOVIE

	return Movie->PlayFlag ;
}

// ムービーのボリュームをセットする(0〜10000)処理の環境依存処理
extern int SetMovieVolume_PF( MOVIEGRAPH * Movie, int Volume )
{
	// 音量補正
	if( Volume > 10000 )
	{
		Volume = 10000 ;
	}

	if( Volume < 0     )
	{
		Volume = 0 ;
	}

#ifndef DX_NON_MEDIA_FOUNDATION
	if( Movie->PF.pMFReader != NULL )
	{
		// サウンドの音量をセット
#ifndef DX_NON_SOUND
		NS_SetVolumeSoundMem( Volume, Movie->PF.MFSoundHandle ) ;
#endif // DX_NON_SOUND

		return 0 ;
	}
#endif // DX_NON_MEDIA_FOUNDATION

#ifndef DX_NON_DSHOW_MOVIE
	if( Movie->PF.pBasicAudio != NULL )
	{
		Movie->PF.pBasicAudio->put_Volume( -10000 + Volume ) ;

		// 正常終了
		return 0 ;
	}
#endif // DX_NON_DSHOW_MOVIE

	return -1 ;
}

// ムービーの基本イメージデータを取得する処理の環境依存処理
extern	BASEIMAGE *GetMovieBaseImage_PF( MOVIEGRAPH * Movie, int *ImageUpdateFlag, int ImageUpdateFlagSetOnly )
{
#ifndef DX_NON_MEDIA_FOUNDATION
	if( Movie->PF.pMFReader != NULL )
	{
		// ムービーのフレームを更新
		UpdateMovie( Movie->HandleInfo.Handle ) ;

		// NowImage の内容が更新されたかどうかのフラグを代入する
		if( ImageUpdateFlag != NULL )
		{
			*ImageUpdateFlag = Movie->NowImageUpdateFlag ;
		}
		Movie->NowImageUpdateFlag = FALSE ;

		// ImageUpdateFlagSetOnly が TRUE の場合はここで終了
		if( ImageUpdateFlagSetOnly )
		{
			return NULL ;
		}

		// BaseImage がセットアップされていなかったらセットアップする
		if( Movie->PF.MFBaseImageSetup == 0 )
		{
			MediaFoundationMovie_SetupImage( Movie, 1, 0, FALSE ) ;
		}

		// アドレスを返す
		return &Movie->NowImage ;
	}
#endif // DX_NON_MEDIA_FOUNDATION

#ifndef DX_NON_DSHOW_MOVIE
	if( Movie->PF.pMovieImage != NULL )
	{
		// ムービーのフレームを更新
		if( GetMovieState( Movie->HandleInfo.Handle ) == FALSE )
		{
			int Time ;

			Time = TellMovie( Movie->HandleInfo.Handle ) ;
			PlayMovie_(  Movie->HandleInfo.Handle ) ;
			UpdateMovie( Movie->HandleInfo.Handle, TRUE ) ;
			PauseMovie(  Movie->HandleInfo.Handle ) ;
			SeekMovie(   Movie->HandleInfo.Handle, Time ) ;
		}
		else
		{
			if( ImageUpdateFlagSetOnly == FALSE && Movie->PF.pMovieImage->AlwaysBaseImage == 0 )
			{
				Movie->PF.pMovieImage->AlwaysBaseImage = 1 ;
			}

			UpdateMovie( Movie->HandleInfo.Handle ) ;
		}

		// NowImage の内容が更新されたかどうかのフラグを代入する
		if( ImageUpdateFlag != NULL )
		{
			*ImageUpdateFlag = Movie->NowImageUpdateFlag ;
		}
		Movie->NowImageUpdateFlag = FALSE ;

		return &Movie->NowImage ;
	}
#endif // DX_NON_DSHOW_MOVIE

	return NULL ;
}

// ムービーの総フレーム数を得る
extern int GetMovieTotalFrame_PF( MOVIEGRAPH * Movie )
{
#ifndef DX_NON_MEDIA_FOUNDATION
	if( Movie->PF.pMFReader != NULL )
	{
		// 総フレーム数を返す
		return Movie->PF.MFTotalFrame ;
	}
#endif // DX_NON_MEDIA_FOUNDATION

	return -1 ;
}

// ムービーの再生位置を取得する(ミリ秒単位)処理の環境依存処理
extern int TellMovie_PF( MOVIEGRAPH * Movie )
{
#ifndef DX_NON_MEDIA_FOUNDATION
	if( Movie->PF.pMFReader != NULL )
	{
		// フレームを更新
		UpdateMovie( Movie->HandleInfo.Handle ) ;

		// フレームから再生時間を割り出す
		return ( int )( Movie->PF.MFFrameRateDenominator * Movie->PF.MFCurrentFrame * 1000 / Movie->PF.MFFrameRateNumerator ) ;
	}
#endif // DX_NON_MEDIA_FOUNDATION

#ifndef DX_NON_DSHOW_MOVIE
	if( Movie->PF.pMediaSeeking != NULL )
	{
		D_STREAM_TIME NowTime ;

		// 時間取得
		if( Movie->PF.pMediaSeeking->GetCurrentPosition( &NowTime ) != S_OK )
		{
			return -1 ;
		}

		// 時間を返す
		return _DTOL( (double)NowTime / 10000 ) ;
	}
#endif // DX_NON_DSHOW_MOVIE

	return -1 ;
}

// ムービーの再生位置を取得する(フレーム単位)処理の環境依存処理
extern int TellMovieToFrame_PF( MOVIEGRAPH * Movie )
{
#ifndef DX_NON_MEDIA_FOUNDATION
	if( Movie->PF.pMFReader != NULL )
	{
		// フレームを更新
		UpdateMovie( Movie->HandleInfo.Handle ) ;

		// フレームを返す
		return Movie->PF.MFCurrentFrame ;
	}
#endif // DX_NON_MEDIA_FOUNDATION

#ifndef DX_NON_DSHOW_MOVIE
	if( Movie->PF.pMediaSeeking != NULL )
	{
		D_STREAM_TIME NowTime ;

		// 時間取得
		if( Movie->PF.pMediaSeeking->GetCurrentPosition( &NowTime ) != S_OK )
		{
			return -1 ;
		}

		// 時間を返す
		return _DTOL( (double)NowTime / Movie->PF.FrameTime ) ;
	}
#endif // DX_NON_DSHOW_MOVIE

	return -1 ;
}

// ムービーの再生位置を設定する(フレーム単位)処理の環境依存処理
extern int SeekMovieToFrame_PF( MOVIEGRAPH * Movie, int Frame )
{
#ifndef DX_NON_MEDIA_FOUNDATION
	if( Movie->PF.pMFReader != NULL )
	{
		D_PROPVARIANT variant ;

		_MEMSET( &variant, 0, sizeof( variant ) ) ;
		variant.vt = D_VT_I8 ;
		variant.hVal.QuadPart = ( ULONGLONG )Movie->PF.MFFrameRateDenominator * ( ULONGLONG )Frame * ( ULONGLONG )10000000 / Movie->PF.MFFrameRateNumerator ;
		Movie->PF.pMFReader->SetCurrentPosition( _GUID_NULL, variant ) ;

		// 再生フレームをセット
		Movie->PF.MFCurrentFrame = Frame ;

		// 再生開始タイムを変更する
		Movie->PF.MFPlayNowTime = Movie->PF.MFFrameRateDenominator * ( LONGLONG )Movie->PF.MFCurrentFrame * ( LONGLONG )1000000 / Movie->PF.MFFrameRateNumerator ;

		// 最後の ReadSample の時間、フレームをリセット
		Movie->PF.MFLastReadSampleTimeStamp = -1 ;
		Movie->PF.MFLastReadSampleFrame = -1 ;

		// イメージがセットアップされている、フラグを倒す
		Movie->PF.MFBaseImageSetup = 0 ;
#ifndef DX_NON_FILTER
		Movie->PF.MFYUVGrHandleSetup = 0 ;
#endif // DX_NON_FILTER

		// 再生位置を変更する
#ifndef DX_NON_SOUND
		NS_SetSoundCurrentTime( ( int )( Movie->PF.MFPlayNowTime / 1000 ), Movie->PF.MFSoundHandle ) ;
#endif // DX_NON_SOUND

		return 0 ;
	}
#endif // DX_NON_MEDIA_FOUNDATION

#ifndef DX_NON_DSHOW_MOVIE
	if( Movie->PF.pMediaSeeking != NULL )
	{
		LONGLONG Now, Stop ;

		if( Movie->PF.pMediaSeeking == NULL ) return 0 ;

		Now = ( D_STREAM_TIME )_DTOL64( (double)Frame * Movie->PF.FrameTime ) ;
		Stop = 0 ;
		Movie->PF.pMediaSeeking->SetPositions( &Now, D_AM_SEEKING_AbsolutePositioning, &Stop, D_AM_SEEKING_NoPositioning ) ;

		// 正常終了
		return 0 ;
	}
#endif // DX_NON_DSHOW_MOVIE

	return -1 ;
}

// ムービーの１フレームあたりの時間を得る処理の環境依存処理
extern LONGLONG GetOneFrameTimeMovie_PF( MOVIEGRAPH * Movie )
{
#if !defined( DX_NON_DSHOW_MOVIE ) || !defined( DX_NON_MEDIA_FOUNDATION )

	return Movie->PF.FrameTime ;

#else // !defined( DX_NON_DSHOW_MOVIE ) || !defined( DX_NON_MEDIA_FOUNDATION )

	return -1 ;

#endif // !defined( DX_NON_DSHOW_MOVIE ) || !defined( DX_NON_MEDIA_FOUNDATION )
}





// ムービーの更新を行う処理の環境依存処理
extern int UpdateMovie_PF( MOVIEGRAPH * Movie, int /*AlwaysFlag*/ )
{
#ifndef DX_NON_MEDIA_FOUNDATION
	if( Movie->PF.pMFReader != NULL )
	{
		int NowFrame, AddFrame ;
		LONGLONG NowTime ;

		// 今の再生時間分フレームを進める
		if( Movie->SysPauseFlag == 0 )
		{
			LONGLONG Temp64_1, Temp64_2 ;
			DWORD Temp128[ 4 ] ;

			NowTime = NS_GetNowHiPerformanceCount() ;

			// 再生済み時間を進める
			if( Movie->PF.MFPlaySpeedRate < 0.999999999 || Movie->PF.MFPlaySpeedRate > 1.0000000001 )
			{
				Temp64_1 = NowTime - Movie->PF.MFPrevTimeCount ;
				Temp64_2 = _DTOL( Movie->PF.MFPlaySpeedRate * 0x10000 ) ;
				_MUL128_1( ( DWORD * )&Temp64_1, ( DWORD * )&Temp64_2, Temp128 ) ;

				Temp64_2 = 0x10000 ;
				_DIV128_1( Temp128, ( DWORD * )&Temp64_2, ( DWORD * )&Temp64_1 ) ;

				Movie->PF.MFPlayNowTime += Temp64_1 ;
			}
			else
			{
				Movie->PF.MFPlayNowTime += NowTime - Movie->PF.MFPrevTimeCount ;
			}

			Movie->PF.MFPrevTimeCount = NowTime ;

			// 現在のフレームの算出
			{
				Temp64_1 = Movie->PF.MFPlayNowTime ;
				Temp64_2 = ( LONGLONG )Movie->PF.MFFrameRateNumerator * 0x10000 / Movie->PF.MFFrameRateDenominator ;
				_MUL128_1( ( DWORD * )&Temp64_1, ( DWORD * )&Temp64_2, Temp128 ) ;

				Temp64_2 = LL_NUM( 65536000000 ) /* 1000000 * 0x10000 */ ;
				_DIV128_1( Temp128, ( DWORD * )&Temp64_2, ( DWORD * )&Temp64_1 ) ;

				NowFrame = ( int )Temp64_1 ;
			}

			// ループ指定があるかどうかで総フレーム数を超えている場合の処理を分岐する
			if( Movie->PF.MFTotalFrame <= NowFrame )
			{
				if( Movie->PlayType & DX_PLAYTYPE_LOOPBIT )
				{
					// ループする場合は総フレーム数で割った余りを出す
					NowFrame %= Movie->PF.MFTotalFrame ;
				}
				else
				{
					// ループしない場合は最終フレームで止まる
					NowFrame = Movie->PF.MFTotalFrame - 1 ;
				}
			}

			// 希望のフレームが現在バッファに格納されているフレームと違う場合はバッファを更新する
			if( Movie->PF.MFCurrentFrame != NowFrame )
			{
				Movie->NowImageUpdateFlag = TRUE ;

				// 加算するフレーム数を算出
				if( NowFrame < Movie->PF.MFCurrentFrame )
				{
					// ループする場合
					AddFrame = Movie->PF.MFTotalFrame - Movie->PF.MFCurrentFrame + NowFrame ;

					// 再生位置を変更
					SeekMovieToFrame_PF( Movie, NowFrame ) ;

#ifndef DX_NON_SOUND
					// ループする場合で、再生タイプが動画基準の場合はサウンドも再度再生を開始する
					if( Movie->PF.MFLoopType == 0 )
					{
						NS_PlaySoundMem( Movie->PF.MFSoundHandle, DX_PLAYTYPE_BACK ) ;
					}
#endif // DX_NON_SOUND
				}
				else
				{
					AddFrame = NowFrame - Movie->PF.MFCurrentFrame ;

					// 最終更新時間から 2フレーム分以上時間が経過していないのに 2フレーム進むことになった場合は 1フレームに補正する
					if( AddFrame == 2 && ( Movie->RefreshTime - NowTime + ( Movie->PF.FrameTime / 10 ) / 10 ) / ( Movie->PF.FrameTime / 10 ) < 2 )
					{
						AddFrame = 1 ;
					}

					// フレームを進める
					MediaFoundationMovie_IncToFrame( Movie, AddFrame ) ;
				}

				// 最終更新時間を保存
				Movie->RefreshTime = NowTime ;
			}
			else
			{
				// 希望のフレームも現フレームも終端に達していて且つループ指定でも無くサウンドも無い場合はここで再生終了
				if( NowFrame >= Movie->PF.MFTotalFrame - 1 &&
					( Movie->PlayType & DX_PLAYTYPE_LOOPBIT ) == 0
#ifndef DX_NON_SOUND
					&& ( Movie->PF.MFSoundHandle == -1 || NS_CheckSoundMem( Movie->PF.MFSoundHandle ) != 1 )
#endif // DX_NON_SOUND
					)
				{
					Movie->PlayFlag = FALSE ;

					// 内部一時停止フラグを立てる
					Movie->SysPauseFlag = 1 ;
				}
			}
		}
		else
		{
#ifndef DX_NON_FILTER
			if( GSYS.HardInfo.UseShader )
			{
				if( Movie->PF.MFYUVGrHandleSetup == 0 )
				{
					MediaFoundationMovie_SetupImage( Movie, 0, 1, FALSE ) ;
				}
			}
			else
#endif // DX_NON_FILTER
			{
				if( Movie->PF.MFBaseImageSetup == 0 )
				{
					MediaFoundationMovie_SetupImage( Movie, 1, 0, FALSE ) ;
				}
			}
		}

		// イメージの構築
		if( Movie->UpdateFunction )
		{
			Movie->UpdateFunction( Movie, Movie->UpdateFunctionData ) ;

			// 最初の更新が行われたフラグをセットする
			Movie->FirstUpdateFlag = TRUE ;
		}
	}
#endif // DX_NON_MEDIA_FOUNDATION

#ifndef DX_NON_DSHOW_MOVIE
	if( Movie->PF.pGraph != NULL )
	{
		LONGLONG Now, Stop ;

		if( Movie->PF.pMovieImage->NewImageSet )
		{
			Movie->NowImageUpdateFlag = TRUE ;

			Movie->PF.pMovieImage->NewImageSet = 0 ;
			if( Movie->UpdateFunction )
			{
				if( Movie->PF.pMovieImage->ImageBuffer &&
					( Movie->PF.pMovieImage->YImageBuffer == NULL || Movie->PF.pMovieImage->AlwaysBaseImage ) )
				{
					Movie->NowImage.Width        = ( int )Movie->PF.pMovieImage->Width ;
					Movie->NowImage.Height       = ( int )Movie->PF.pMovieImage->Height ;
					Movie->NowImage.Pitch        = ( int )Movie->PF.pMovieImage->Pitch ;
					Movie->NowImage.GraphData    = Movie->PF.pMovieImage->ImageBuffer ;
					Movie->NowImageGraphOutAlloc = TRUE ;
					if( Movie->PF.pMovieImage->ImageType == 0 )
					{
						NS_CreateFullColorData( &Movie->NowImage.ColorData ) ;
					}
					else
					if( Movie->PF.pMovieImage->ImageType == 1 && Movie->A8R8G8B8Flag )
					{
						NS_CreateARGB8ColorData( &Movie->NowImage.ColorData ) ;
					}
					else
					{
						NS_CreateXRGB8ColorData( &Movie->NowImage.ColorData ) ;
					}
					Movie->UseNowImage = &Movie->NowImage ;
					Movie->UpdateFunction( Movie, Movie->UpdateFunctionData ) ;
				}

				if( Movie->PF.pMovieImage->YImageBuffer )
				{
#ifndef DX_NON_FILTER
					SETUP_GRAPHHANDLE_GPARAM GParam ;
					BASEIMAGE BaseImage ;
					RECT SrcRect ;

					// グラフィックハンドルがまだ作成されていなかったら作成する
					if( Movie->YGrHandle == -1 )
					{
						Graphics_Image_InitSetupGraphHandleGParam( &GParam ) ;

						Graphics_Image_InitSetupGraphHandleGParam_Normal_NonDrawValid( &GParam, 32, FALSE ) ;
						GParam.CreateImageChannelNum      = 1 ;
						GParam.CreateImageChannelBitDepth = 8 ;
						Movie->YGrHandle = Graphics_Image_MakeGraph_UseGParam( &GParam, Movie->PF.pMovieImage->YWidth, Movie->PF.pMovieImage->YHeight, FALSE, FALSE, 0, FALSE, FALSE ) ;
						NS_SetDeleteHandleFlag( Movie->YGrHandle, ( int * )&Movie->YGrHandle ) ;
					}
					if( Movie->UVGrHandle == -1 )
					{
						Graphics_Image_InitSetupGraphHandleGParam( &GParam ) ;

						Graphics_Image_InitSetupGraphHandleGParam_Normal_NonDrawValid( &GParam, 32, FALSE ) ;
						GParam.CreateImageChannelNum      = 2 ;
						GParam.CreateImageChannelBitDepth = 8 ;
						Movie->UVGrHandle     = Graphics_Image_MakeGraph_UseGParam( &GParam, Movie->PF.pMovieImage->UVWidth, Movie->PF.pMovieImage->UVHeight, FALSE, FALSE, 0, FALSE, FALSE ) ;
						NS_SetDeleteHandleFlag( Movie->UVGrHandle, ( int * )&Movie->UVGrHandle ) ;
					}

					_MEMSET( &BaseImage, 0, sizeof( BaseImage ) ) ;
					BaseImage.Width                     = Movie->PF.pMovieImage->YWidth ;
					BaseImage.Height                    = Movie->PF.pMovieImage->YHeight ;
					BaseImage.Pitch                     = Movie->PF.pMovieImage->YPitch ;
					BaseImage.GraphData                 = Movie->PF.pMovieImage->YImageBuffer ;
					BaseImage.ColorData.Format          = DX_BASEIMAGE_FORMAT_NORMAL ;
					BaseImage.ColorData.PixelByte       = 1 ;
					BaseImage.ColorData.ChannelNum      = 1 ;
					BaseImage.ColorData.ChannelBitDepth = 8 ;
					SETRECT( SrcRect, 0, 0, BaseImage.Width, BaseImage.Height ) ;
					Graphics_Image_BltBmpOrBaseImageToGraph3(
						&SrcRect,
						0,
						0,
						Movie->YGrHandle,
						&BaseImage,
						NULL,
						FALSE,
						FALSE,
						FALSE,
						FALSE
					) ;

					BaseImage.Width                     = Movie->PF.pMovieImage->UVWidth ;
					BaseImage.Height                    = Movie->PF.pMovieImage->UVHeight ;
					BaseImage.Pitch                     = Movie->PF.pMovieImage->UVPitch ;
					BaseImage.GraphData                 = Movie->PF.pMovieImage->UVImageBuffer ;
					BaseImage.ColorData.Format          = DX_BASEIMAGE_FORMAT_NORMAL ;
					BaseImage.ColorData.PixelByte       = 2 ;
					BaseImage.ColorData.ChannelNum      = 2 ;
					BaseImage.ColorData.ChannelBitDepth = 8 ;
					SETRECT( SrcRect, 0, 0, BaseImage.Width, BaseImage.Height ) ;
					Graphics_Image_BltBmpOrBaseImageToGraph3(
						&SrcRect,
						0,
						0,
						Movie->UVGrHandle,
						&BaseImage,
						NULL,
						FALSE,
						FALSE,
						FALSE,
						FALSE
					) ;
#endif // DX_NON_FILTER

					Movie->UpdateFunction( Movie, Movie->UpdateFunctionData ) ;
				}
			}
		}

		if( Movie->PF.pMediaSeeking && Movie->PF.pMediaControl )
		{
			Movie->PF.pMediaSeeking->GetCurrentPosition( &Now ) ;
			if( Now >= Movie->StopTime )
			{
				if( Movie->PlayType & DX_PLAYTYPE_LOOPBIT )
				{
					Now = 0 ;
					Stop = 0 ;
					Movie->PF.pMediaSeeking->SetPositions( &Now, D_AM_SEEKING_AbsolutePositioning, &Stop, D_AM_SEEKING_NoPositioning ) ;
					Movie->PF.pMediaControl->Run() ;
				}
				else
				{
					// 停止
					Movie->PF.pMediaControl->Pause() ;

					Movie->PlayFlag = FALSE ;

					// 内部一時停止フラグを立てる
					Movie->SysPauseFlag = 1 ;
				}
			}
		}
	}
#endif // DX_NON_DSHOW_MOVIE

	// 正常終了
	return 0 ;
}





#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_NON_MOVIE

