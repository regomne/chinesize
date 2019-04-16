// ----------------------------------------------------------------------------
//
//		ＤＸライブラリ　ムービー再生処理用プログラム
//
//				Ver 3.20c
//
// ----------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

#include "DxMovie.h"

#ifndef DX_NON_MOVIE

// インクルード----------------------------------------------------------------
#include "DxLib.h"
#include "DxStatic.h"
#include "DxBaseFunc.h"
#include "DxMemory.h"
#include "DxSound.h"
#include "DxUseCLib.h"
#include "DxLog.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義------------------------------------------------------------------

// ムービーを更新する間隔
#define MOVIEREFRESHWAIT		(17)

// ムービーハンドルの有効性チェック
#define MOVIEHCHK( HAND, MPOINT )		HANDLECHK(       DX_HANDLETYPE_MOVIE, HAND, *( ( HANDLEINFO ** )&MPOINT ) )
#define MOVIEHCHK_ASYNC( HAND, MPOINT )	HANDLECHK_ASYNC( DX_HANDLETYPE_MOVIE, HAND, *( ( HANDLEINFO ** )&MPOINT ) )

#define VFW_E_NOT_FOUND                  ((HRESULT)0x80040216L)

#define MOVIE MovieGraphManageData

// 構造体型宣言----------------------------------------------------------------

// データ宣言------------------------------------------------------------------

MOVIEGRAPHMANAGE MovieGraphManageData ;								// 動画関連データ

// クラス宣言 -----------------------------------------------------------------

// 関数プロトタイプ宣言--------------------------------------------------------

// プログラムコード------------------------------------------------------------

// ムービー関連の管理処理の初期化
extern int InitializeMovieManage( void )
{
	if( MOVIE.InitializeFlag == TRUE )
		return -1 ;

	// ハンドル管理情報の初期化
	InitializeHandleManage( DX_HANDLETYPE_MOVIE, sizeof( MOVIEGRAPH ), MAX_MOVIE_NUM, InitializeMovieHandle, TerminateMovieHandle, L"Movie" ) ;

	// 環境依存の初期化処理を行う
	if( InitializeMovieManage_PF() < 0 )
	{
		return -1 ;
	}

	// 初期化フラグ立てる
	MOVIE.InitializeFlag = TRUE ;

	// 終了
	return 0 ;
}

// ムービー関連の管理処理の後始末
extern int TerminateMovieManage( void )
{
	if( MOVIE.InitializeFlag == FALSE )
		return -1 ;

	// 環境依存の後始末処理を行う
	if( TerminateMovieManage_PF() < 0 )
	{
		return -1 ;
	}

	// ハンドル管理情報の後始末
	TerminateHandleManage( DX_HANDLETYPE_MOVIE ) ;

	// 初期化フラグ倒す
	MOVIE.InitializeFlag = FALSE ;

	// 終了
	return 0 ;
}

// OPENMOVIE_GPARAM のデータをセットする
extern void Graphics_Image_InitOpenMovieGParam( OPENMOVIE_GPARAM *GParam )
{
	GParam->RightAlphaFlag = MOVIE.RightAlphaFlag ;
	GParam->A8R8G8B8Flag = MOVIE.A8R8G8B8Flag ;
	GParam->NotUseYUVFormatSurfaceFlag = MOVIE.NotUseYUVFormatSurfaceFlag ;
}

// ムービーファイルをオープンする
extern int OpenMovie( const wchar_t *FileName, int *Width, int *Height, int SurfaceMode )
{
	OPENMOVIE_GPARAM GParam ;

	Graphics_Image_InitOpenMovieGParam( &GParam ) ;

	return OpenMovie_UseGParam( -1, &GParam, FileName, NULL, 0, Width, Height, FALSE, SurfaceMode ) ;
}

// ムービーハンドルを初期化をする関数
extern int InitializeMovieHandle( HANDLEINFO *HandleInfo )
{
	MOVIEGRAPH *Movie = ( MOVIEGRAPH * )HandleInfo ;

#ifndef DX_NON_FILTER
	Movie->YGrHandle  = -1 ;
	Movie->UVGrHandle = -1 ;
#endif // DX_NON_FILTER

	// 終了
	return 0 ;
}

// ムービーハンドルの後始末を行う関数
extern int TerminateMovieHandle( HANDLEINFO *HandleInfo )
{
	MOVIEGRAPH *Movie = ( MOVIEGRAPH * )HandleInfo ;

#ifndef DX_NON_FILTER
	if( Movie->YGrHandle >= 0 )
	{
		NS_DeleteGraph( Movie->YGrHandle ) ;
		Movie->YGrHandle = -1 ;
	}

	if( Movie->UVGrHandle >= 0 )
	{
		NS_DeleteGraph( Movie->UVGrHandle ) ;
		Movie->UVGrHandle = -1 ;
	}
#endif // DX_NON_FILTER

	// 環境依存処理
	TerminateMovieHandle_PF( HandleInfo ) ;

#ifndef DX_NON_OGGTHEORA
	// もし Theora を使用していた場合はその開放処理を行う
	if( Movie->TheoraFlag )
	{
		TheoraDecode_Terminate( Movie->TheoraHandle ) ;

		if( Movie->TheoraStreamData )
		{
			if( Movie->TheoraStreamDataIsFileImage == FALSE )
			{
				DX_FCLOSE( Movie->TheoraStreamData ) ;
			}
			else
			{
				MemStreamClose( Movie->TheoraStreamData ) ;
			}
			Movie->TheoraStreamData = 0 ;
		}

#ifndef DX_NON_SOUND
		NS_DeleteSoundMem( Movie->TheoraVorbisHandle ) ;
		Movie->TheoraVorbisHandle = 0 ;
#endif // DX_NON_SOUND
		Movie->TheoraHandle = 0 ;
		Movie->TheoraFlag = 0 ;
	}
#endif // DX_NON_OGGTHEORA

	if( Movie->NowImage.GraphData != NULL )
	{
		if( Movie->NowImageGraphOutAlloc == FALSE )
		{
			DXFREE( Movie->NowImage.GraphData ) ;
		}
		Movie->NowImage.GraphData = NULL ;
	}

	// 正常終了
	return 0 ;
}

// OpenMovie のグローバル変数にアクセスしないバージョン
extern int OpenMovie_UseGParam(
	int GrHandle,
	OPENMOVIE_GPARAM *GParam,
	const wchar_t *FileName,
	const void *FileImage,
	size_t FileImageSize,
	int *Width,
	int *Height,
	int SurfaceMode,
	int ImageSizeGetOnly,
	int ASyncThread
)
{
	int NewHandle ;
	MOVIEGRAPH * Movie ;

	// ハンドルの作成
	NewHandle = AddHandle( DX_HANDLETYPE_MOVIE, ASyncThread, -1 ) ;
	if( NewHandle < 0 )
	{
		return -1 ;
	}

	if( MOVIEHCHK_ASYNC( NewHandle, Movie ) )
	{
		return -1 ;
	}

	// 使用しているグラフィックハンドルをセット
	Movie->ParentGrHandle = GrHandle ;

	// 右側をアルファとして扱うかフラグをセットする
	Movie->RightAlpha = GParam->RightAlphaFlag ;

	// 32bitカラーフォーマットの動画を A8R8G8B8 形式として扱うかどうかのフラグをセットする
	Movie->A8R8G8B8Flag = GParam->A8R8G8B8Flag ;

	// YUVフォーマットのサーフェスを使用しないかどうかのフラグをセットする
	Movie->NotUseYUVFormatSurfaceFlag = GParam->NotUseYUVFormatSurfaceFlag ;

#ifndef DX_NON_OGGTHEORA
	STREAMDATASHRED *UseStreamDataShred ;
	// Ogg Theora としてオープンしようとしてみる
	if( FileName != NULL )
	{
		Movie->TheoraStreamData = DX_FOPEN( FileName ) ;
		UseStreamDataShred = GetFileStreamDataShredStruct() ;
		Movie->TheoraStreamDataIsFileImage = FALSE ;
	}
	else
	{
		Movie->TheoraStreamData = MemStreamOpen( ( void *)FileImage, FileImageSize ) ;
		UseStreamDataShred = GetMemStreamDataShredStruct() ;
		Movie->TheoraStreamDataIsFileImage = TRUE ;
	}
	Movie->TheoraHandle = TheoraDecode_InitializeStream(
		UseStreamDataShred,
		Movie->TheoraStreamData,
		20,
		Movie->NotUseYUVFormatSurfaceFlag,
		Movie->NotUseYUVFormatSurfaceFlag,
		ASyncThread
	) ;
	if( Movie->TheoraHandle == 0 )
	{
		Movie->TheoraSetupGraphHandleImage = FALSE ;
		if( Movie->TheoraStreamDataIsFileImage == FALSE )
		{
			DX_FCLOSE( Movie->TheoraStreamData ) ;
		}
		else
		{
			MemStreamClose( Movie->TheoraStreamData ) ;
		}
		Movie->TheoraStreamData = 0 ;
	}
	if( Movie->TheoraHandle != 0 )
	{
		THEORADECODE_INFO Info ;

		// 情報を埋める
		TheoraDecode_GetInfo( Movie->TheoraHandle, &Info ) ;
		Movie->Width  = Info.Width ;
		Movie->Height = Info.Height ;
		if( Width  ) *Width  = Info.Width ;
		if( Height ) *Height = Info.Height ;
		Movie->TheoraFrameRate = Info.FrameRate ;
		Movie->TheoraTotalPlayTime = _DTOL( 1000.0 / Info.FrameRate * Info.TotalFrame ) ;

#ifndef DX_NON_SOUND
		if( ImageSizeGetOnly == FALSE )
		{
			LOADSOUND_GPARAM LoadSoundGParam ;

			InitLoadSoundGParam( &LoadSoundGParam ) ;

			// 開いたら Thera 用のセットアップを行う

			// サウンド再生用にサウンドデータとしても読み込み
			LoadSoundGParam.NotInitSoundMemDelete = TRUE ;
			LoadSoundGParam.OggVorbisFromTheoraFile = TRUE ;
			LoadSoundGParam.DisableReadSoundFunctionMask = ~DX_READSOUNDFUNCTION_OGG ;
			if( FileName != NULL )
			{
				LoadSoundGParam.CreateSoundDataType = DX_SOUNDDATATYPE_FILE ;
				Movie->TheoraVorbisHandle = LoadSoundMemBase_UseGParam( &LoadSoundGParam, FileName, 1, -1, FALSE, ASyncThread ) ;
			}
			else
			{
				LoadSoundGParam.CreateSoundDataType = DX_SOUNDDATATYPE_MEMPRESS ;
				Movie->TheoraVorbisHandle = LoadSoundMemByMemImageBase_UseGParam( &LoadSoundGParam, TRUE, -1, FileImage, ( int )FileImageSize, 1, -1, FALSE, ASyncThread ) ;
			}
			Movie->TheoraVorbisTotalTime = NS_GetSoundTotalTime( Movie->TheoraVorbisHandle ) ;
			Movie->TheoraVorbisFrequency = NS_GetFrequencySoundMem( Movie->TheoraVorbisHandle ) ;

			// ループタイプの決定( 長いほうを基準にする )
			Movie->TheoraLoopType = Movie->TheoraVorbisTotalTime > Movie->TheoraTotalPlayTime ? 1 : 0 ;
		}
		else
		{
			Movie->TheoraVorbisHandle = -1 ;
		}

#else // DX_NON_SOUND
		// ループタイプは動画データ合わせ
		Movie->TheoraLoopType = 0 ;
#endif // DX_NON_SOUND

		// 再生速度の初期化
		Movie->TheoraPlaySpeedRate = 1.0 ;

		// Theora を使用しているフラグを立てる
		Movie->TheoraFlag = 1 ;

		// 使用すべき BASEIMAGE をセット
		Movie->UseNowImage = ( BASEIMAGE * )TheoraDecode_GetBaseImage( Movie->TheoraHandle ) ;

		// サーフェスモードはノーマルということで
		Movie->SurfaceMode = DX_MOVIESURFACE_NORMAL ;
	}
	else
#endif // DX_NON_OGGTHEORA
	{
		// Ogg Theora 以外の動画はメモリからの再生はできない
		if( FileName == NULL )
		{
			DXST_LOGFILE_ADDUTF16LE( "\xe0\x30\xfc\x30\xd3\x30\xfc\x30\xd5\x30\xa1\x30\xa4\x30\xeb\x30\xaa\x30\xfc\x30\xd7\x30\xf3\x30\xe6\x51\x06\x74\x42\x66\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"ムービーファイルオープン処理時に失敗しました" @*/ ) ;
			goto ERR ;
		}

		// オープンできなかったら環境依存の動画ファイルオープンを試みる
		if( OpenMovie_UseGParam_PF( Movie, GParam, FileName, Width, Height, SurfaceMode, ImageSizeGetOnly, ASyncThread ) == -1 )
		{
			DXST_LOGFILE_ADDUTF16LE( "\xe0\x30\xfc\x30\xd3\x30\xfc\x30\xd5\x30\xa1\x30\xa4\x30\xeb\x30\xaa\x30\xfc\x30\xd7\x30\xf3\x30\xe6\x51\x06\x74\x42\x66\x6b\x30\xa8\x30\xe9\x30\xfc\x30\x4c\x30\x7a\x76\x1f\x75\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"ムービーファイルオープン処理時にエラーが発生しました" @*/ ) ;
			goto ERR ;
		}
		Movie->Width  = Movie->NowImage.Width  ;
		Movie->Height = Movie->NowImage.Height ;

		// 使用すべき BASEIMAGE をセット
		Movie->UseNowImage = &Movie->NowImage ;
	}

	// 画像が更新されたフラグを倒す
	Movie->NowImageUpdateFlag = FALSE ;

	// 内部一時停止フラグを立てる
	Movie->SysPauseFlag = 1 ;

	// 再生中フラグを倒す
	Movie->PlayFlag = FALSE ;

	// 再生タイプはバックグラウンドにしておく
	Movie->PlayType = DX_PLAYTYPE_BACK ;

#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		DecASyncLoadCount( NewHandle ) ;
	}
#endif // DX_NON_ASYNCLOAD

	// ハンドルを返す
	return NewHandle ;

ERR :
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		DecASyncLoadCount( NewHandle ) ;
	}
#endif // DX_NON_ASYNCLOAD

	CloseMovie( NewHandle ) ;

	// エラー終了
	return -1 ;
}

// ムービーグラフィックを終了する
extern int CloseMovie( int MovieHandle )
{
	return SubHandle( MovieHandle ) ;
}

// ムービーの再生を開始する
extern int PlayMovie_( int MovieHandle, int PlayType, int SysPlay )
{
	MOVIEGRAPH * Movie ;

	// ムービーデータハンドルを取得
	if( MOVIEHCHK( MovieHandle, Movie ) )
		return -1 ;

	// PlayType が -1 の場合は変更しない
	if( PlayType == -1 ) PlayType = Movie->PlayType ;

	// PlayType に DX_PLAYTYPE_NORMAL を指定した場合は DX_PLAYTYPE_BACK になる
	if( PlayType == DX_PLAYTYPE_NORMAL ) PlayType = DX_PLAYTYPE_BACK ;

	// 内部一時停止フラグが倒れていたら何もしない
	if( Movie->SysPauseFlag == 0 ) return 0 ;

	if( SysPlay == 1 && Movie->PlayFlag == FALSE ) return 0 ;

	// 画像が更新されたフラグを倒す
	Movie->NowImageUpdateFlag = FALSE ;

	// 内部一時停止フラグを倒す
	Movie->SysPauseFlag = 0 ;

	// 再生タイプを保存する
	Movie->PlayType = PlayType ;

	// 再生中フラグを立てる
	Movie->PlayFlag = TRUE ;

#ifndef DX_NON_OGGTHEORA
	// Theora を使用しているかどうかで処理を分岐
	if( Movie->TheoraFlag )
	{
		int CurFrame ;
		THEORADECODE_INFO Info ;

		// 再生開始時の時間を取得
//		Movie->TheoraPlayTime = NS_GetNowHiPerformanceCount() ;

		// 再生開始時の時間を取得
		Movie->TheoraPrevTimeCount = NS_GetNowHiPerformanceCount() ;

		// 再生時間をセット
		TheoraDecode_GetInfo( Movie->TheoraHandle, &Info ) ;
		CurFrame = TheoraDecode_GetCurrentFrame( Movie->TheoraHandle ) ;
		Movie->TheoraPlayNowTime = _DTOL( 1000000.0 / Movie->TheoraFrameRate * CurFrame ) ;

		// 既に再生済みのフレーム数分だけ前に再生したことにする
//		TheoraDecode_GetInfo( Movie->TheoraHandle, &Info ) ;
//		CurFrame = TheoraDecode_GetCurrentFrame( Movie->TheoraHandle ) ;
//		Movie->TheoraPlayTime -= _DTOL( 1000000.0 / Movie->TheoraFrameRate * CurFrame ) ;

		// Vorbis の再生も開始する
#ifndef DX_NON_SOUND
//		NS_SetSoundCurrentTime( _DTOL( 1000.0 / Movie->TheoraFrameRate * CurFrame ), Movie->TheoraVorbisHandle ) ; 
		NS_PlaySoundMem( Movie->TheoraVorbisHandle, Movie->TheoraLoopType == 1 ? PlayType : DX_PLAYTYPE_BACK, FALSE ) ;
#endif // DX_NON_SOUND
	}
	else
#endif // DX_NON_OGGTHEORA
	{
		// 環境依存の再生処理を行う
		PlayMovie__PF( Movie, PlayType, SysPlay ) ;
	}

	// 終了
	return 0 ;
}

// ムービーの再生をストップする
extern int PauseMovie( int MovieHandle, int SysPause )
{
	MOVIEGRAPH * Movie ;

	// ムービーデータハンドルを取得
	if( MOVIEHCHK( MovieHandle, Movie ) )
		return -1 ;

	// 内部一時停止フラグが立っていたらなにもしない
	if( Movie->SysPauseFlag == 1 )
		return 0 ;

#ifndef DX_NON_OGGTHEORA
	// Theora を使用しているかどうかで処理を分岐
	if( Movie->TheoraFlag )
	{
		// Theora を使用している場合

		// Vorbis の再生を停止する
#ifndef DX_NON_SOUND
		NS_StopSoundMem( Movie->TheoraVorbisHandle ) ;
#endif // DX_NON_SOUND

		// 現在の再生時間分までフレームを進めておく
		UpdateMovie( MovieHandle ) ;
	}
	else
#endif // DX_NON_OGGTHEORA
	{
		// 環境依存の再生停止処理
		if( PauseMovie_PF( Movie, SysPause ) < 0 )
		{
			return 0 ;
		}
	}

	// 内部一時停止フラグを立てる
	Movie->SysPauseFlag = 1 ;

	// 再生中フラグを倒す
	if( SysPause == 0 )
	{
		Movie->PlayFlag = FALSE ;
	}

	// 終了
	return 0 ;
}

// ムービーのフレームを進める、戻すことは出来ない( ムービーが停止状態で、且つ Ogg Theora のみ有効 )
extern int AddMovieFrame( int MovieHandle, unsigned int FrameNum )
{
	MOVIEGRAPH * Movie ;

	// ムービーデータハンドルを取得
	if( MOVIEHCHK( MovieHandle, Movie ) )
		return -1 ;

	// 画像が更新されたフラグを倒す
	Movie->NowImageUpdateFlag = FALSE ;

#ifndef DX_NON_OGGTHEORA
	// Ogg Theora ではなかったら変更できない
	if( Movie->TheoraFlag == FALSE )
		return -1 ;

	// 再生中だったら変更できない
	if( GetMovieState( MovieHandle ) == TRUE )
		return -1 ;

	// 再生位置を変更
	TheoraDecode_IncToFrame( Movie->TheoraHandle, ( int )FrameNum ) ;

	// 終了
	return 0 ;
#else // DX_NON_OGGTHEORA
	return -1 ;
#endif // DX_NON_OGGTHEORA
}

// ムービーの再生位置を設定する(ミリ秒単位)
extern int SeekMovie( int MovieHandle, int Time )
{
	MOVIEGRAPH * Movie ;

	// ムービーデータハンドルを取得
	if( MOVIEHCHK( MovieHandle, Movie ) )
		return -1 ;

	// 再生を止める
	PauseMovie( MovieHandle ) ;

	// 画像が更新されたフラグを倒す
	Movie->NowImageUpdateFlag = FALSE ;

#ifndef DX_NON_OGGTHEORA
	// Theora を使用しているかどうかで処理を分岐
	if( Movie->TheoraFlag )
	{
		int CurFrame ;
		THEORADECODE_INFO Info ;

		// コンバート位置を変更する
		TheoraDecode_SeekToTime( Movie->TheoraHandle, Time * 1000 ) ;

		// 再生開始タイムを変更する
//		Movie->TheoraPlayTime = NS_GetNowHiPerformanceCount() ;
		TheoraDecode_GetInfo( Movie->TheoraHandle, &Info ) ;
		CurFrame = TheoraDecode_GetCurrentFrame( Movie->TheoraHandle ) ;
		Movie->TheoraPlayNowTime = _DTOL( 1000000.0 / Movie->TheoraFrameRate * CurFrame ) ;
//		Movie->TheoraPlayTime -= _DTOL( 1000000.0 / Movie->TheoraFrameRate * CurFrame ) ;
	
		// 再生位置を変更する
#ifndef DX_NON_SOUND
		NS_SetSoundCurrentTime( Time, Movie->TheoraVorbisHandle ) ;
#endif // DX_NON_SOUND
	}
	else
#endif // DX_NON_OGGTHEORA
	{
		// 環境依存処理
		if( SeekMovie_PF( Movie, Time ) < 0 )
		{
			return 0 ;
		}
	}

	// 終了
	return 0 ;
}

// ムービーの再生速度を設定する( 1.0 = 等倍速  2.0 = ２倍速 )、一部のファイルフォーマットのみで有効な機能です
extern int SetPlaySpeedRateMovie( int MovieHandle, double SpeedRate )
{
	MOVIEGRAPH * Movie ;

	// ムービーデータハンドルを取得
	if( MOVIEHCHK( MovieHandle, Movie ) )
		return -1 ;

#ifndef DX_NON_OGGTHEORA
	// Theora を使用しているかどうかで処理を分岐
	if( Movie->TheoraFlag )
	{
		Movie->TheoraPlaySpeedRate = SpeedRate ;
#ifndef DX_NON_SOUND
		if( Movie->TheoraVorbisHandle != -1 )
		{
			NS_SetFrequencySoundMem( _DTOL( Movie->TheoraVorbisFrequency * Movie->TheoraPlaySpeedRate ), Movie->TheoraVorbisHandle ) ;
		}
		else
#endif // DX_NON_SOUND
		{
		}
	}
	else
#endif // DX_NON_OGGTHEORA
	{
		// 環境依存処理
		if( SetPlaySpeedRateMovie_PF( Movie, SpeedRate ) < 0 )
		{
			return 0 ;
		}
	}

	// 終了
	return 0 ;
}

// ムービーの再生状態を得る
extern int GetMovieState( int MovieHandle )
{
	MOVIEGRAPH * Movie ;

	// ムービーデータハンドルを取得
	if( MOVIEHCHK( MovieHandle, Movie ) )
	{
		return -1 ;
	}

#ifndef DX_NON_OGGTHEORA
	// Theoraを使用しているかどうかで処理を分岐
	if( Movie->TheoraFlag )
	{
		return Movie->PlayFlag ;
	}
	else
#endif // DX_NON_OGGTHEORA
	{
		// 環境依存処理
		return GetMovieState_PF( Movie ) ;
	}
}

// ムービーのボリュームをセットする(0〜10000)
extern int SetMovieVolume( int Volume, int MovieHandle )
{
	MOVIEGRAPH * Movie ;

	// ムービーデータハンドルを取得
	if( MOVIEHCHK( MovieHandle, Movie ) )
		return -1 ;

#ifndef DX_NON_OGGTHEORA
	// Theora を使用しているかどうかで処理を分岐
	if( Movie->TheoraFlag )
	{
		// Vorbis サウンドの音量をセット
#ifndef DX_NON_SOUND
		NS_SetVolumeSoundMem( Volume, Movie->TheoraVorbisHandle ) ;
#endif // DX_NON_SOUND
	}
	else
#endif // DX_NON_OGGTHEORA
	{
		// 環境依存処理
		if( SetMovieVolume_PF( Movie, Volume ) < 0 )
		{
			return 0 ;
		}
	}

	// 終了
	return 0 ;
}

// ムービーの基本イメージデータを取得する
extern BASEIMAGE *GetMovieBaseImage( int MovieHandle, int *ImageUpdateFlag, int ImageUpdateFlagSetOnly )
{
	MOVIEGRAPH * Movie ;

	// ムービーデータハンドルを取得
	if( MOVIEHCHK( MovieHandle, Movie ) )
		return NULL ;

#ifndef DX_NON_OGGTHEORA
	// Theora を使用しているかどうかで処理を分岐
	if( Movie->TheoraFlag )
	{
		// ムービーのフレームを更新
		UpdateMovie( MovieHandle ) ;

		// NowImage の内容が更新されたかどうかのフラグを代入する
		if( ImageUpdateFlag != NULL )
		{
			*ImageUpdateFlag = Movie->NowImageUpdateFlag ;
		}
		Movie->NowImageUpdateFlag = FALSE ;

		// Theora のイメージを返す
		return ImageUpdateFlagSetOnly ? NULL : ( BASEIMAGE * )TheoraDecode_GetBaseImage( Movie->TheoraHandle ) ;
	}
#endif // DX_NON_OGGTHEORA

	// 使用していない場合は環境依存の処理を行う
	return GetMovieBaseImage_PF( Movie, ImageUpdateFlag, ImageUpdateFlagSetOnly ) ;
}

// ムービーの総フレーム数を得る( Ogg Theora でのみ有効 )
extern int GetMovieTotalFrame( int MovieHandle )
{
	MOVIEGRAPH * Movie ;

	// ムービーデータハンドルを取得
	if( MOVIEHCHK( MovieHandle, Movie ) )
		return -1 ;

#ifndef DX_NON_OGGTHEORA
	// Theora を使用しているかどうかで処理を分岐
	if( Movie->TheoraFlag )
	{
		THEORADECODE_INFO info ;

		// 情報の取得
		TheoraDecode_GetInfo( Movie->TheoraHandle, &info ) ;

		// 総フレーム数を返す
		return info.TotalFrame ;
	}
#endif // DX_NON_OGGTHEORA

	// 使用していない場合は環境依存の処理を行う
	return GetMovieTotalFrame_PF( Movie ) ;
}

// ムービーの再生位置を取得する(ミリ秒単位)
extern int TellMovie( int MovieHandle )
{
	MOVIEGRAPH * Movie ;
	
	// ムービーデータハンドルを取得
	if( MOVIEHCHK( MovieHandle, Movie ) )
		return -1 ;

#ifndef DX_NON_OGGTHEORA
	// Theora を使用しているかどうかで処理を分岐
	if( Movie->TheoraFlag )
	{
		THEORADECODE_INFO Info ;

		TheoraDecode_GetInfo( Movie->TheoraHandle, &Info ) ;

		// フレームを更新
		UpdateMovie( MovieHandle ) ;

		// フレームから再生時間を割り出す
		return _DTOL( TheoraDecode_GetCurrentFrame( Movie->TheoraHandle ) * 1000 / Movie->TheoraFrameRate ) ;
	}
#endif // DX_NON_OGGTHEORA

	// 使用していない場合は環境依存の処理を行う
	return TellMovie_PF( Movie ) ;
}
 
// ムービーの再生位置を取得する(フレーム単位)
extern int TellMovieToFrame( int MovieHandle )
{
	MOVIEGRAPH * Movie ;
	
	// ムービーデータハンドルを取得
	if( MOVIEHCHK( MovieHandle, Movie ) )
		return -1 ;

#ifndef DX_NON_OGGTHEORA
	// Theora かどうかで処理を分岐
	if( Movie->TheoraFlag )
	{
		THEORADECODE_INFO Info ;

		TheoraDecode_GetInfo( Movie->TheoraHandle, &Info ) ;

		// フレームを更新
		UpdateMovie( MovieHandle ) ;

		// 現在のフレームを返す
		return TheoraDecode_GetCurrentFrame( Movie->TheoraHandle ) ;
	}
#endif // DX_NON_OGGTHEORA

	// 使用していない場合は環境依存の処理を行う
	return TellMovieToFrame_PF( Movie ) ;
}

// ムービーの再生位置を設定する(フレーム単位)
extern int SeekMovieToFrame( int MovieHandle, int Frame )
{
	MOVIEGRAPH * Movie ;
//	D_OAFilterState State ;
	
	// ムービーデータハンドルを取得
	if( MOVIEHCHK( MovieHandle, Movie ) )
		return -1 ;

	// 画像が更新されたフラグを倒す
	Movie->NowImageUpdateFlag = FALSE ;

#ifndef DX_NON_OGGTHEORA
	// Theora かどうかで処理を分岐
	if( Movie->TheoraFlag )
	{
		int CurFrame ;
		THEORADECODE_INFO Info ;

		// コンバート位置を変更する
		TheoraDecode_SeekToFrame( Movie->TheoraHandle, Frame ) ;

		// 再生開始タイムを変更する
//		Movie->TheoraPlayTime = NS_GetNowHiPerformanceCount() ;
		TheoraDecode_GetInfo( Movie->TheoraHandle, &Info ) ;
		CurFrame = TheoraDecode_GetCurrentFrame( Movie->TheoraHandle ) ;
		Movie->TheoraPlayNowTime = _DTOL( 1000000.0 / Movie->TheoraFrameRate * CurFrame ) ;
//		Movie->TheoraPlayTime -= _DTOL( 1000000.0 / Movie->TheoraFrameRate * CurFrame ) ;
	
		// 再生位置を変更する
#ifndef DX_NON_SOUND
		NS_SetSoundCurrentTime( _DTOL( Frame * 1000 / Movie->TheoraFrameRate ), Movie->TheoraVorbisHandle ) ;
#endif // DX_NON_SOUND
	}
	else
#endif // DX_NON_OGGTHEORA
	{
		// 環境依存の処理を行う
		SeekMovieToFrame_PF( Movie, Frame ) ;
	}

	// 終了
	return 0 ;
}

// ムービーの１フレームあたりの時間を得る
extern LONGLONG GetOneFrameTimeMovie( int MovieHandle )
{
	MOVIEGRAPH * Movie ;
	
	// ムービーデータハンドルを取得
	if( MOVIEHCHK( MovieHandle, Movie ) )
		return -1 ;

#ifndef DX_NON_OGGTHEORA
	// Theora かどうかで処理を分岐
	if( Movie->TheoraFlag )
	{
		THEORADECODE_INFO Info ;

		TheoraDecode_GetInfo( Movie->TheoraHandle, &Info ) ;
		return _DTOL( 1000000 / Movie->TheoraFrameRate ) ;
	}
#endif // DX_NON_OGGTHEORA

	// 環境依存の処理を行う
	return GetOneFrameTimeMovie_PF( Movie ) ;
}

// ムービーグラフィックのデータを取得する
extern MOVIEGRAPH * GetMovieData( int MovieHandle )
{
	MOVIEGRAPH * Movie ;
	
	if( MOVIEHCHK( MovieHandle, Movie ) )
		return NULL ;

	return Movie ;
}


// ムービーで使用しているサーフェスの解放を行う
extern int ReleaseMovieSurface( int MovieHandle )
{
	MOVIEGRAPH * Movie ;
	
	if( MOVIEHCHK( MovieHandle, Movie ) )
		return -1 ;

#ifndef DX_NON_OGGTHEORA
	if( Movie->TheoraFlag )
	{
		TheoraDecode_SurfaceTerminate( Movie->TheoraHandle ) ;
	}
	else
#endif
	{
	}

	// 終了
	return 0 ;
}

// ムービーの更新を行う
extern int UpdateMovie( int MovieHandle, int AlwaysFlag )
{
	MOVIEGRAPH * Movie ;
//	LONGLONG NowFrame, OldFrame ;
	
	// ムービーデータハンドルを取得
	if( MOVIEHCHK( MovieHandle, Movie ) )
		return -1 ;

#ifndef DX_NON_OGGTHEORA
	// Theora を使用しているかどうかで処理を分岐
	if( Movie->TheoraFlag )
	{
		// Theora を使用している場合
		int NowFrame, CurFrame, AddFrame ;
		LONGLONG NowTime ;
		THEORADECODE_INFO Info ;

		// Theora の情報を取得
		TheoraDecode_GetInfo( Movie->TheoraHandle, &Info ) ;

		// 今の再生時間分フレームを進める
		if( Movie->SysPauseFlag == 0 )
		{
			// 再生時間は音声が有効な場合は音声から取得する
#ifndef DX_NON_SOUND
/*			if( Movie->TheoraVorbisHandle != -1 && NS_CheckSoundMem( Movie->TheoraVorbisHandle ) == 1 )
			{
				NowTime = NS_GetSoundCurrentTime( Movie->TheoraVorbisHandle ) * 1000 ;
				NowFrame = _DTOL( ( double )( NowTime * Movie->TheoraFrameRate ) / 1000000 ) ;

				// 音声から音を取得した場合は、映像の最大フレームを超えてしまった場合は現在フレームを映像の最大フレームにする
				if( NowFrame >= Info.TotalFrame )
				{
					NowFrame = Info.TotalFrame - 1 ;
				}
			}
			else*/
#endif // DX_NON_SOUND
			{
				LONGLONG Temp64_1, Temp64_2 ;
				DWORD Temp128[ 4 ] ;

				NowTime = NS_GetNowHiPerformanceCount() ;

				// 再生済み時間を進める
				if( Movie->TheoraPlaySpeedRate < 0.999999999 || Movie->TheoraPlaySpeedRate > 1.0000000001 )
				{
					Temp64_1 = NowTime - Movie->TheoraPrevTimeCount ;
					Temp64_2 = _DTOL( Movie->TheoraPlaySpeedRate * 0x10000 ) ;
					_MUL128_1( ( DWORD * )&Temp64_1, ( DWORD * )&Temp64_2, Temp128 ) ;

					Temp64_2 = 0x10000 ;
					_DIV128_1( Temp128, ( DWORD * )&Temp64_2, ( DWORD * )&Temp64_1 ) ;

					Movie->TheoraPlayNowTime += Temp64_1 ;
				}
				else
				{
					Movie->TheoraPlayNowTime += NowTime - Movie->TheoraPrevTimeCount ;
				}

				Movie->TheoraPrevTimeCount = NowTime ;

				// 現在のフレームの算出
				{
					Temp64_1 = Movie->TheoraPlayNowTime ;
					Temp64_2 = _DTOL( Movie->TheoraFrameRate * 0x10000 ) ;
					_MUL128_1( ( DWORD * )&Temp64_1, ( DWORD * )&Temp64_2, Temp128 ) ;

					Temp64_2 = LL_NUM( 65536000000 ) /* 1000000 * 0x10000 */ ;
					_DIV128_1( Temp128, ( DWORD * )&Temp64_2, ( DWORD * )&Temp64_1 ) ;

					NowFrame = ( int )Temp64_1 ;
//					NowFrame = _DTOL( ( double )( ( NowTime - Movie->TheoraPlayTime ) * Movie->TheoraFrameRate ) / 1000000 ) ;
				}

				// ループ指定があるかどうかで総フレーム数を超えている場合の処理を分岐する
				if( Info.TotalFrame <= NowFrame )
				{
					if( Movie->PlayType & DX_PLAYTYPE_LOOPBIT )
					{
						// ループする場合は総フレーム数で割った余りを出す
						NowFrame %= Info.TotalFrame ;
					}
					else
					{
						// ループしない場合は最終フレームで止まる
						NowFrame = Info.TotalFrame - 1 ;
					}
				}
			}

			// ムービーの再生フレームを取得する
			CurFrame = TheoraDecode_GetCurrentFrame( Movie->TheoraHandle ) ;

			// 希望のフレームが現在バッファに格納されているフレームと違う場合はバッファを更新する
			if( CurFrame != NowFrame )
			{
				Movie->NowImageUpdateFlag = TRUE ;

				// 加算するフレーム数を算出
				if( NowFrame < CurFrame )
				{
					// ループする場合
					AddFrame = Info.TotalFrame - CurFrame + NowFrame ;

#ifndef DX_NON_SOUND
					// ループする場合で、再生タイプが動画基準の場合はサウンドも再度再生を開始する
					if( Movie->TheoraLoopType == 0 )
					{
						NS_PlaySoundMem( Movie->TheoraVorbisHandle, DX_PLAYTYPE_BACK ) ;
					}
#endif // DX_NON_SOUND
				}
				else
				{
					AddFrame = NowFrame - CurFrame ;
				}

				// フレームを進める
				TheoraDecode_IncToFrame( Movie->TheoraHandle, AddFrame ) ;

				// 最終更新時間を保存
				Movie->RefreshTime = NowTime ;
			}
			else
			{
				// 希望のフレームも現フレームも終端に達していて且つループ指定でも無くサウンドも無い場合はここで再生終了
				if( NowFrame == Info.TotalFrame - 1 &&
					( Movie->PlayType & DX_PLAYTYPE_LOOPBIT ) == 0
#ifndef DX_NON_SOUND
					&& ( Movie->TheoraVorbisHandle == -1 || NS_CheckSoundMem( Movie->TheoraVorbisHandle ) != 1 )
#endif // DX_NON_SOUND
					)
				{
					Movie->PlayFlag = FALSE ;

					// 内部一時停止フラグを立てる
					Movie->SysPauseFlag = 1 ;
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
	else
#endif
	{
		// 環境依存処理を行う
		UpdateMovie_PF( Movie, AlwaysFlag ) ;
	}

	// 終了
	return 0 ;
}

// ムービーの更新時に呼び出すコールバック関数を登録する
extern int SetCallbackMovie( int MovieHandle, void ( *Callback )( MOVIEGRAPH *Movie, void *Data ), void *Data )
{
	MOVIEGRAPH *Movie ;

	// ムービーデータハンドルを取得
	if( MOVIEHCHK( MovieHandle, Movie ) )
		return -1 ;

	// アドレスをセット
	Movie->UpdateFunction = Callback ;
	Movie->UpdateFunctionData = Data ;

	// 終了
	return 0 ;
}

// ムービーの再生状態を停止する
extern int DisableMovieAll( void )
{
	// 現バージョンでは特に何もしない
	return 0 ;

#if 0
	MOVIEGRAPH *Movie ;
	int i ;

	if( MOVIE.InitializeFlag == FALSE )
		return -1 ;

	// すべてのムービーについて処理を行う
	for( i = HandleManageArray[ DX_HANDLETYPE_MOVIE ].AreaMin ; i <= HandleManageArray[ DX_HANDLETYPE_MOVIE ].AreaMax ; i ++ )
	{
		Movie = ( MOVIEGRAPH * )HandleManageArray[ DX_HANDLETYPE_MOVIE ].Handle[ i ] ;
		if( Movie == NULL ) continue ;

#ifndef DX_NON_OGGTHEORA
		// Theora を使用しているかどうかで処理を分岐
		if( Movie->TheoraFlag )
		{
			// 何もしない
/*
			// 内部一時停止フラグが倒れていたら処理
			if( Movie->SysPauseFlag == 0 )
			{
				// Vorbis の再生を停止する
				NS_StopSoundMem( Movie->TheoraVorbisHandle ) ;

				// 現在の再生時間分までフレームを進めておく
				{
					int NowFrame, CurFrame ;
					LONGLONG NowTime ;
					THEORADECODE_INFO Info ;

					// Theora の情報を取得
					TheoraDecode_GetInfo( Movie->TheoraHandle, &Info ) ;

					// 今の再生時間分フレームを進める
					NowTime = NS_GetNowHiPerformanceCount() ;
					NowFrame = _DTOL( ( double )( ( NowTime - Movie->TheoraPlayTime ) * Movie->TheoraFrameRate ) / 1000000 ) ;
					CurFrame = TheoraDecode_GetCurrentFrame( Movie->TheoraHandle ) ;
					if( CurFrame != NowFrame )
					{
						if( NowFrame < CurFrame )
						{
							TheoraDecode_SeekToFrame( Movie->TheoraHandle, NowFrame ) ;
						}
						else
						{
							TheoraDecode_IncToFrame( Movie->TheoraHandle, NowFrame - CurFrame ) ;
						}
					}

					// イメージの構築
					TheoraDecode_SetupImage( Movie->TheoraHandle ) ;
				}

				// 内部一時停止フラグを立てる
				Movie->SysPauseFlag = 1 ;
			}
*/		}
		else
#endif
#ifndef DX_NON_DSHOW_MOVIE
		{
/*			if( Movie )
			{
				// 現在の再生時間を保存
				Movie->pMediaSeeking->GetCurrentPosition( &Movie->BackUpTime ) ;

				// 各種ＣＯＭオブジェクトを終了する

				if( Movie->pMediaControl		){ Movie->pMediaControl->Release() ;		Movie->pMediaControl = NULL ;		}
				if( Movie->pMediaSeeking		){ Movie->pMediaSeeking->Release() ;		Movie->pMediaSeeking = NULL ;		}
				if( Movie->pBasicAudio			){ Movie->pBasicAudio->Release() ;			Movie->pBasicAudio = NULL ;			}
				if( Movie->pMovieImage			){ Movie->pMovieImage->Release() ;			Movie->pMovieImage = NULL ;			}
				if( Movie->pFilter				){ Movie->pFilter->Release() ;				Movie->pFilter = NULL ;				}
				if( Movie->pGraph				){ Movie->pGraph->Release() ;				Movie->pGraph = NULL ;				}
			}
*/		}
#else  // DX_NON_DSHOW_MOVIE
		{
		}
#endif
	}

	// 終了
	return 0 ;
#endif
}


// ムービーの再生状態を復元する
extern int RestoreMovieAll( void )
{
	// 現バージョンでは特に何もしない
	return 0 ;

#if 0

	MOVIEGRAPH *Movie ;
	int i ;

	if( MOVIE.InitializeFlag == FALSE )
		return -1 ;

	// すべてのムービーについて処理を行う
	for( i = HandleManageArray[ DX_HANDLETYPE_MOVIE ].AreaMin ; i <= HandleManageArray[ DX_HANDLETYPE_MOVIE ].AreaMax ; i ++ )
	{
		Movie = ( MOVIEGRAPH * )HandleManageArray[ DX_HANDLETYPE_MOVIE ].Handle[ i ] ;
		if( Movie == NULL ) continue ;

#ifndef DX_NON_OGGTHEORA
		// Theora かどうかで処理を分岐
		if( Movie->TheoraFlag )
		{
			// 何もしない
/*
			// 再生中フラグが立っている場合のみ処理
			if( Movie->PlayFlag )
			{
				// 内部一時停止フラグが立っていたら処理
				if( Movie->SysPauseFlag == 1 )
				{
					int CurFrame ;
					THEORADECODE_INFO Info ;

					// 再生開始時の時間を取得
					Movie->TheoraPlayTime = NS_GetNowHiPerformanceCount() ;

					// 既に再生済みのフレーム数分だけ前に再生したことにする
					TheoraDecode_GetInfo( Movie->TheoraHandle, &Info ) ;
					CurFrame = TheoraDecode_GetCurrentFrame( Movie->TheoraHandle ) ;
					Movie->TheoraPlayTime -= _DTOL( 1000000.0 / Movie->TheoraFrameRate * CurFrame ) ;

					// Vorbis の再生も開始する
					NS_PlaySoundMem( Movie->TheoraVorbisHandle, DX_PLAYTYPE_BACK, FALSE ) ;

					// 内部一時停止フラグを倒す
					Movie->SysPauseFlag = 0 ;
				}
			}
*/
		}
		else
#endif
#ifndef DX_NON_DSHOW_MOVIE
		{
		}
#else // DX_NON_DSHOW_MOVIE
		{
		}
#endif
	}

	// 終了
	return 0 ;
#endif  
}

// すべてのムービーグラフィックをスタート
extern int PlayMovieAll( void )
{
	MOVIEGRAPH *Movie ;
	int i ;

	if( MOVIE.InitializeFlag == FALSE )
		return -1 ;

	for( i = HandleManageArray[ DX_HANDLETYPE_MOVIE ].AreaMin ; i <= HandleManageArray[ DX_HANDLETYPE_MOVIE ].AreaMax ; i ++ )
	{
		Movie = ( MOVIEGRAPH * )HandleManageArray[ DX_HANDLETYPE_MOVIE ].Handle[ i ] ;
		if( Movie == NULL ) continue ; 

		PlayMovie_( Movie->HandleInfo.Handle, -1, 1 ) ;
	}

	// 終了
	return 0 ;
}

// すべてのムービーグラフィックをストップ
extern int PauseMovieAll( void )
{
	MOVIEGRAPH *Movie ;
	int i ;

	if( MOVIE.InitializeFlag == FALSE )
		return -1 ;

	for( i = HandleManageArray[ DX_HANDLETYPE_MOVIE ].AreaMin ; i <= HandleManageArray[ DX_HANDLETYPE_MOVIE ].AreaMax ; i ++ )
	{
		Movie = ( MOVIEGRAPH * )HandleManageArray[ DX_HANDLETYPE_MOVIE ].Handle[ i ] ;
		if( Movie == NULL ) continue ; 

		PauseMovie( Movie->HandleInfo.Handle, 1 ) ;
	}

	// 終了
	return 0 ;
}

// 動画ファイルの右半分をアルファ情報として扱うかどうかをセットする( TRUE:アルファ情報として扱う )
extern int NS_SetMovieRightImageAlphaFlag( int Flag )
{
	MOVIE.RightAlphaFlag = Flag;
	return TRUE;
}

// 読み込む動画ファイルが32bitカラーだった場合、A8R8G8B8 形式として扱うかどうかをセットする、
// 32bitカラーではない動画ファイルに対しては無効( Flag  TRUE:A8R8G8B8として扱う  FALSE:X8R8G8B8として扱う( デフォルト ) )
extern int NS_SetMovieColorA8R8G8B8Flag( int Flag )
{
	MOVIE.A8R8G8B8Flag = Flag;
	return TRUE;
}

// ＹＵＶフォーマットのサーフェスが使用できる場合はＹＵＶフォーマットのサーフェスを使用するかどうかを設定する( TRUE:使用する( デフォルト ) FALSE:ＲＧＢフォーマットのサーフェスを使用する )
extern int NS_SetMovieUseYUVFormatSurfaceFlag( int Flag )
{
	MOVIE.NotUseYUVFormatSurfaceFlag = Flag ? FALSE : TRUE ;
	return TRUE;
}


#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif
