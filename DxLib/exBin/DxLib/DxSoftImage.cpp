// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		ソフトウェアで扱う画像プログラム
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

// インクルード ------------------------------------------------------------------
#include "DxSoftImage.h"

#ifndef DX_NON_SOFTIMAGE

#include "DxStatic.h"
#include "DxLib.h"
#include "DxASyncLoad.h"
#include "DxBaseFunc.h"
#include "DxMemory.h"
#include "DxFile.h"
#include "DxFont.h"
#include "DxSystem.h"
#include "DxLog.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

// 構造体定義 --------------------------------------------------------------------

// 内部大域変数宣言 --------------------------------------------------------------

SOFTIMAGEMANAGE SoftImageManage ;

// 関数プロトタイプ宣言-----------------------------------------------------------

// プログラム --------------------------------------------------------------------

// 初期化、後始末

// 基本イメージ管理情報の初期化
extern int InitializeSoftImageManage( void )
{
	// 既に初期化されていたら何もしない
	if( SoftImageManage.InitializeFlag == TRUE )
		return -1 ;

	// ソフトイメージハンドル管理情報の初期化
	InitializeHandleManage( DX_HANDLETYPE_SOFTIMAGE, sizeof( SOFTIMAGE ), MAX_SOFTIMAGE_NUM, InitializeSoftImageHandle, TerminateSoftImageHandle, L"SoftImage" ) ;

	// 初期化フラグを立てる
	SoftImageManage.InitializeFlag = TRUE ;

	// 終了
	return 0 ;
}

// 基本イメージ管理情報の後始末
extern int TerminateSoftImageManage( void )
{
	// 既に後始末されていたら何もしない
	if( SoftImageManage.InitializeFlag == FALSE )
		return -1 ;

	// 初期化フラグを倒す
	SoftImageManage.InitializeFlag = FALSE ;

	// ソフトイメージハンドル管理情報の後始末
	TerminateHandleManage( DX_HANDLETYPE_SOFTIMAGE ) ;

	// 終了
	return 0 ;
}










// ソフトウエアイメージハンドルの初期化
extern int InitializeSoftImageHandle( HANDLEINFO * )
{
	// 特に何もしない
	return 0 ;
}

// ソフトウエアイメージハンドルの後始末
extern int TerminateSoftImageHandle( HANDLEINFO *HandleInfo )
{
	SOFTIMAGE *SoftImg = ( SOFTIMAGE * )HandleInfo ;

	// 画像データの解放
	NS_ReleaseBaseImage( &SoftImg->BaseImage ) ;

	// 終了
	return 0 ;
}

// ソフトウエアで扱うイメージを全て解放する
extern int NS_InitSoftImage( void )
{
	return AllHandleSub( DX_HANDLETYPE_SOFTIMAGE ) ;
}

// LoadSoftImage の実処理関数
static int LoadSoftImage_Static(
	int SIHandle,
	const wchar_t *FileName,
	int ASyncThread
)
{
	SOFTIMAGE *SoftImg ;

	// 新しいソフトイメージハンドルの取得
	if( ASyncThread )
	{
		if( SFTIMGCHK_ASYNC( SIHandle, SoftImg ) )
			return -1 ;
	}
	else
	{
		if( SFTIMGCHK( SIHandle, SoftImg ) )
			return -1 ;
	}

	// CreateBaseImageToFile でファイルから読み込み
	if( CreateBaseImageToFile_WCHAR_T( FileName, &SoftImg->BaseImage, FALSE ) == -1 )
		return -1 ;

	// 正常終了
	return 0 ;
}

#ifndef DX_NON_ASYNCLOAD

// LoadSoftImage の非同期読み込みスレッドから呼ばれる関数
static void LoadSoftImage_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	int SIHandle ;
	const wchar_t *FileName ;
	int Addr ;
	int Result ;

	Addr = 0 ;
	SIHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	FileName = GetASyncLoadParamString( AParam->Data, &Addr ) ;

	Result = LoadSoftImage_Static( SIHandle, FileName, TRUE ) ;

	DecASyncLoadCount( SIHandle ) ;
	if( Result < 0 )
	{
		NS_DeleteSoftImage( SIHandle ) ;
	}
}

#endif // DX_NON_ASYNCLOAD

// LoadSoftImage のグローバル変数にアクセスしないバージョン
extern int LoadSoftImage_UseGParam(
	const wchar_t *FileName,
	int ASyncLoadFlag
)
{
	int SIHandle ;

	CheckActiveState() ;

	// ハンドルの作成
	SIHandle = AddHandle( DX_HANDLETYPE_SOFTIMAGE, FALSE, -1 ) ;
	if( SIHandle == -1 )
	{
		return -1 ;
	}

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		wchar_t FullPath[ 1024 ] ;
		int Addr ;

		ConvertFullPathW_( FileName, FullPath, sizeof( FullPath ) ) ;

		// パラメータに必要なメモリのサイズを算出
		Addr = 0 ;
		AddASyncLoadParamInt( NULL, &Addr, SIHandle ) ;
		AddASyncLoadParamString( NULL, &Addr, FullPath ) ;

		// メモリの確保
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// 処理に必要な情報をセット
		AParam->ProcessFunction = LoadSoftImage_ASync ;
		Addr = 0 ;
		AddASyncLoadParamInt( AParam->Data, &Addr, SIHandle ) ;
		AddASyncLoadParamString( AParam->Data, &Addr, FullPath ) ;

		// データを追加
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto ERR ;
		}

		// 非同期読み込みカウントをインクリメント
		IncASyncLoadCount( SIHandle, AParam->Index ) ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		if( LoadSoftImage_Static( SIHandle, FileName, FALSE ) < 0 )
			goto ERR ;
	}

	// 終了
	return SIHandle ;

ERR :
	NS_DeleteSoftImage( SIHandle ) ;

	return -1 ;
}

// ソフトウエアで扱うイメージの読み込み( -1:エラー  -1以外:イメージハンドル )
extern	int		NS_LoadSoftImage( const TCHAR *FileName )
{
#ifdef UNICODE
	return LoadSoftImage_WCHAR_T(
		FileName
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, return -1 )

	Result = LoadSoftImage_WCHAR_T(
		UseFileNameBuffer
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( FileName )

	return Result ;
#endif
}

// 画像ファイルを読み込みソフトウエアイメージハンドルを作成する( -1:エラー  -1以外:イメージハンドル )
extern int NS_LoadSoftImageWithStrLen( const TCHAR *FileName, size_t FileNameLength )
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = LoadSoftImage_WCHAR_T( UseFileNameBuffer ) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = LoadSoftImage_WCHAR_T( UseFileNameBuffer ) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#endif
	return Result ;
}

// ソフトウエアで扱うイメージの読み込み( -1:エラー  -1以外:イメージハンドル )
extern	int		LoadSoftImage_WCHAR_T( const wchar_t *FileName )
{
	return LoadSoftImage_UseGParam( FileName, GetASyncLoadFlag() ) ;
}






// LoadARGB8ColorSoftImage の実処理関数
static int LoadARGB8ColorSoftImage_Static(
	int SIHandle,
	const wchar_t *FileName,
	int ASyncThread
)
{
	SOFTIMAGE *SoftImg ;

	// 新しいソフトイメージハンドルの取得
	if( ASyncThread )
	{
		if( SFTIMGCHK_ASYNC( SIHandle, SoftImg ) )
			return -1 ;
	}
	else
	{
		if( SFTIMGCHK( SIHandle, SoftImg ) )
			return -1 ;
	}

	// CreateBaseImageToFile でファイルから読み込み
	if( CreateBaseImageToFile_WCHAR_T( FileName, &SoftImg->BaseImage, FALSE ) == -1 )
		return -1 ;

	// 読み込んだ画像の形式が ARGB8 ではなかったら ARGB8 形式に変換
	if( SoftImg->BaseImage.ColorData.Format				!= DX_BASEIMAGE_FORMAT_NORMAL ||
		SoftImg->BaseImage.ColorData.ChannelNum			!= 0 ||
		SoftImg->BaseImage.ColorData.ChannelBitDepth	!= 0 ||
		SoftImg->BaseImage.ColorData.FloatTypeFlag		!= 0 ||
		SoftImg->BaseImage.ColorData.ColorBitDepth		!= 32 ||
		SoftImg->BaseImage.ColorData.PixelByte			!= 4 ||
		SoftImg->BaseImage.ColorData.MaxPaletteNo		!= 0 ||
		SoftImg->BaseImage.ColorData.NoneMask			!= 0x00000000 ||
		SoftImg->BaseImage.ColorData.AlphaMask			!= 0xff000000 ||
		SoftImg->BaseImage.ColorData.RedMask			!= 0x00ff0000 ||
		SoftImg->BaseImage.ColorData.GreenMask			!= 0x0000ff00 ||
		SoftImg->BaseImage.ColorData.BlueMask			!= 0x000000ff )
	{
		BASEIMAGE TempBaseImage ;

		// 同じサイズの ARGB8形式の基本イメージを作成
		TempBaseImage = SoftImg->BaseImage ;
		if( NS_CreateARGB8ColorBaseImage( SoftImg->BaseImage.Width, SoftImg->BaseImage.Height, &SoftImg->BaseImage ) == -1 )
		{
			NS_ReleaseBaseImage( &TempBaseImage ) ;
			return -1 ;
		}

		// ARGB8 形式に変換
		NS_BltBaseImage( 0, 0, &TempBaseImage, &SoftImg->BaseImage ) ;

		// 読み込んだ基本イメージは解放
		NS_ReleaseBaseImage( &TempBaseImage ) ;
	}

	// 正常終了
	return 0 ;
}

#ifndef DX_NON_ASYNCLOAD

// LoadARGB8ColorSoftImage の非同期読み込みスレッドから呼ばれる関数
static void LoadARGB8ColorSoftImage_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	int SIHandle ;
	const wchar_t *FileName ;
	int Addr ;
	int Result ;

	Addr = 0 ;
	SIHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	FileName = GetASyncLoadParamString( AParam->Data, &Addr ) ;

	Result = LoadARGB8ColorSoftImage_Static( SIHandle, FileName, TRUE ) ;

	DecASyncLoadCount( SIHandle ) ;
	if( Result < 0 )
	{
		NS_DeleteSoftImage( SIHandle ) ;
	}
}

#endif // DX_NON_ASYNCLOAD

// LoadARGB8ColorSoftImage のグローバル変数にアクセスしないバージョン
extern int LoadARGB8ColorSoftImage_UseGParam(
	const wchar_t *FileName,
	int ASyncLoadFlag
)
{
	int SIHandle ;

	CheckActiveState() ;

	// ハンドルの作成
	SIHandle = AddHandle( DX_HANDLETYPE_SOFTIMAGE, FALSE, -1 ) ;
	if( SIHandle == -1 )
	{
		return -1 ;
	}

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		wchar_t FullPath[ 1024 ] ;
		int Addr ;

		ConvertFullPathW_( FileName, FullPath, sizeof( FullPath ) ) ;

		// パラメータに必要なメモリのサイズを算出
		Addr = 0 ;
		AddASyncLoadParamInt( NULL, &Addr, SIHandle ) ;
		AddASyncLoadParamString( NULL, &Addr, FullPath ) ;

		// メモリの確保
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// 処理に必要な情報をセット
		AParam->ProcessFunction = LoadARGB8ColorSoftImage_ASync ;
		Addr = 0 ;
		AddASyncLoadParamInt( AParam->Data, &Addr, SIHandle ) ;
		AddASyncLoadParamString( AParam->Data, &Addr, FullPath ) ;

		// データを追加
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto ERR ;
		}

		// 非同期読み込みカウントをインクリメント
		IncASyncLoadCount( SIHandle, AParam->Index ) ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		if( LoadARGB8ColorSoftImage_Static( SIHandle, FileName, FALSE ) < 0 )
			goto ERR ;
	}

	// 終了
	return SIHandle ;

ERR :
	NS_DeleteSoftImage( SIHandle ) ;

	return -1 ;
}

// ソフトウエアで扱うイメージの読み込み( -1:エラー  -1以外:イメージハンドル )( 読み込んだ画像が RGBA8 以外のフォーマットだった場合は RGBA8 カラーに変換 )
extern	int		NS_LoadARGB8ColorSoftImage( const TCHAR *FileName )
{
#ifdef UNICODE
	return LoadARGB8ColorSoftImage_WCHAR_T(
		FileName
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, return -1 )

	Result = LoadARGB8ColorSoftImage_WCHAR_T(
		UseFileNameBuffer
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( FileName )

	return Result ;
#endif
}

// 画像ファイルを読み込みソフトウエアイメージハンドルを作成する( -1:エラー  -1以外:イメージハンドル )( 読み込んだ画像が RGBA8 以外のフォーマットだった場合は RGBA8 カラーに変換 )
extern int NS_LoadARGB8ColorSoftImageWithStrLen( const TCHAR *FileName, size_t FileNameLength )
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = LoadARGB8ColorSoftImage_WCHAR_T( UseFileNameBuffer ) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = LoadARGB8ColorSoftImage_WCHAR_T( UseFileNameBuffer ) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#endif
	return Result ;
}

// ソフトウエアで扱うイメージの読み込み( -1:エラー  -1以外:イメージハンドル )( 読み込んだ画像が RGBA8 以外のフォーマットだった場合は RGBA8 カラーに変換 )
extern	int		LoadARGB8ColorSoftImage_WCHAR_T( const wchar_t *FileName )
{
	return LoadARGB8ColorSoftImage_UseGParam( FileName, GetASyncLoadFlag() ) ;
}



















// LoadXRGB8ColorSoftImage の実処理関数
static int LoadXRGB8ColorSoftImage_Static(
	int SIHandle,
	const wchar_t *FileName,
	int ASyncThread
)
{
	SOFTIMAGE *SoftImg ;

	// 新しいソフトイメージハンドルの取得
	if( ASyncThread )
	{
		if( SFTIMGCHK_ASYNC( SIHandle, SoftImg ) )
			return -1 ;
	}
	else
	{
		if( SFTIMGCHK( SIHandle, SoftImg ) )
			return -1 ;
	}

	// CreateBaseImageToFile でファイルから読み込み
	if( CreateBaseImageToFile_WCHAR_T( FileName, &SoftImg->BaseImage, FALSE ) == -1 )
		return -1 ;

	// 読み込んだ画像の形式が XRGB8 ではなかったら XRGB8 形式に変換
	if( SoftImg->BaseImage.ColorData.Format				!= DX_BASEIMAGE_FORMAT_NORMAL ||
		SoftImg->BaseImage.ColorData.ChannelNum			!= 0 ||
		SoftImg->BaseImage.ColorData.ChannelBitDepth	!= 0 ||
		SoftImg->BaseImage.ColorData.FloatTypeFlag		!= 0 ||
		SoftImg->BaseImage.ColorData.ColorBitDepth		!= 32 ||
		SoftImg->BaseImage.ColorData.PixelByte			!= 4 ||
		SoftImg->BaseImage.ColorData.MaxPaletteNo		!= 0 ||
		SoftImg->BaseImage.ColorData.NoneMask			!= 0xff000000 ||
		SoftImg->BaseImage.ColorData.AlphaMask			!= 0x00000000 ||
		SoftImg->BaseImage.ColorData.RedMask			!= 0x00ff0000 ||
		SoftImg->BaseImage.ColorData.GreenMask			!= 0x0000ff00 ||
		SoftImg->BaseImage.ColorData.BlueMask			!= 0x000000ff )
	{
		BASEIMAGE TempBaseImage ;

		// 同じサイズの XRGB8形式の基本イメージを作成
		TempBaseImage = SoftImg->BaseImage ;
		if( NS_CreateXRGB8ColorBaseImage( SoftImg->BaseImage.Width, SoftImg->BaseImage.Height, &SoftImg->BaseImage ) == -1 )
		{
			NS_ReleaseBaseImage( &TempBaseImage ) ;
			return -1 ;
		}

		// XRGB8 形式に変換
		NS_BltBaseImage( 0, 0, &TempBaseImage, &SoftImg->BaseImage ) ;

		// 読み込んだ基本イメージは解放
		NS_ReleaseBaseImage( &TempBaseImage ) ;
	}

	// 正常終了
	return 0 ;
}

#ifndef DX_NON_ASYNCLOAD

// LoadXRGB8ColorSoftImage の非同期読み込みスレッドから呼ばれる関数
static void LoadXRGB8ColorSoftImage_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	int SIHandle ;
	const wchar_t *FileName ;
	int Addr ;
	int Result ;

	Addr = 0 ;
	SIHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	FileName = GetASyncLoadParamString( AParam->Data, &Addr ) ;

	Result = LoadXRGB8ColorSoftImage_Static( SIHandle, FileName, TRUE ) ;

	DecASyncLoadCount( SIHandle ) ;
	if( Result < 0 )
	{
		NS_DeleteSoftImage( SIHandle ) ;
	}
}

#endif // DX_NON_ASYNCLOAD

// LoadXRGB8ColorSoftImage のグローバル変数にアクセスしないバージョン
extern int LoadXRGB8ColorSoftImage_UseGParam(
	const wchar_t *FileName,
	int ASyncLoadFlag
)
{
	int SIHandle ;

	CheckActiveState() ;

	// ハンドルの作成
	SIHandle = AddHandle( DX_HANDLETYPE_SOFTIMAGE, FALSE, -1 ) ;
	if( SIHandle == -1 )
	{
		return -1 ;
	}

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		wchar_t FullPath[ 1024 ] ;
		int Addr ;

		ConvertFullPathW_( FileName, FullPath, sizeof( FullPath ) ) ;

		// パラメータに必要なメモリのサイズを算出
		Addr = 0 ;
		AddASyncLoadParamInt( NULL, &Addr, SIHandle ) ;
		AddASyncLoadParamString( NULL, &Addr, FullPath ) ;

		// メモリの確保
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// 処理に必要な情報をセット
		AParam->ProcessFunction = LoadXRGB8ColorSoftImage_ASync ;
		Addr = 0 ;
		AddASyncLoadParamInt( AParam->Data, &Addr, SIHandle ) ;
		AddASyncLoadParamString( AParam->Data, &Addr, FullPath ) ;

		// データを追加
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto ERR ;
		}

		// 非同期読み込みカウントをインクリメント
		IncASyncLoadCount( SIHandle, AParam->Index ) ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		if( LoadXRGB8ColorSoftImage_Static( SIHandle, FileName, FALSE ) < 0 )
			goto ERR ;
	}

	// 終了
	return SIHandle ;

ERR :
	NS_DeleteSoftImage( SIHandle ) ;

	return -1 ;
}

// ソフトウエアで扱うイメージの読み込み( -1:エラー  -1以外:イメージハンドル )( 読み込んだ画像が RGBA8 以外のフォーマットだった場合は RGBA8 カラーに変換 )
extern	int		NS_LoadXRGB8ColorSoftImage( const TCHAR *FileName )
{
#ifdef UNICODE
	return LoadXRGB8ColorSoftImage_WCHAR_T(
		FileName
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, return -1 )

	Result = LoadXRGB8ColorSoftImage_WCHAR_T(
		UseFileNameBuffer
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( FileName )

	return Result ;
#endif
}

// 画像ファイルを読み込みソフトウエアイメージハンドルを作成する( -1:エラー  -1以外:イメージハンドル )( 読み込んだ画像が XGBA8 以外のフォーマットだった場合は XGBA8 カラーに変換 )
extern int NS_LoadXRGB8ColorSoftImageWithStrLen( const TCHAR *FileName, size_t FileNameLength )
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = LoadXRGB8ColorSoftImage_WCHAR_T( UseFileNameBuffer ) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = LoadXRGB8ColorSoftImage_WCHAR_T( UseFileNameBuffer ) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#endif
	return Result ;
}

// ソフトウエアで扱うイメージの読み込み( -1:エラー  -1以外:イメージハンドル )( 読み込んだ画像が RGBA8 以外のフォーマットだった場合は RGBA8 カラーに変換 )
extern	int		LoadXRGB8ColorSoftImage_WCHAR_T( const wchar_t *FileName )
{
	return LoadXRGB8ColorSoftImage_UseGParam( FileName, GetASyncLoadFlag() ) ;
}









// LoadSoftImageToMem の実処理関数
static int LoadSoftImageToMem_Static(
	int SIHandle,
	const void *FileImage,
	int FileImageSize,
	int ASyncThread
)
{
	SOFTIMAGE *SoftImg ;

	// 新しいソフトイメージハンドルの取得
	if( ASyncThread )
	{
		if( SFTIMGCHK_ASYNC( SIHandle, SoftImg ) )
			return -1 ;
	}
	else
	{
		if( SFTIMGCHK( SIHandle, SoftImg ) )
			return -1 ;
	}

	// CreateBaseImageToMem でメモリから読み込み
	if( NS_CreateBaseImageToMem( FileImage, FileImageSize, &SoftImg->BaseImage, FALSE ) == -1 )
		return -1 ;

	// 正常終了
	return 0 ;
}

#ifndef DX_NON_ASYNCLOAD

// LoadSoftImageToMem の非同期読み込みスレッドから呼ばれる関数
static void LoadSoftImageToMem_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	int SIHandle ;
	const void *FileImage ;
	int FileImageSize ;
	int Addr ;
	int Result ;

	Addr = 0 ;
	SIHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	FileImage = GetASyncLoadParamVoidP( AParam->Data, &Addr ) ;
	FileImageSize = GetASyncLoadParamInt( AParam->Data, &Addr ) ;

	Result = LoadSoftImageToMem_Static( SIHandle, FileImage, FileImageSize, TRUE ) ;

	DecASyncLoadCount( SIHandle ) ;
	if( Result < 0 )
	{
		NS_DeleteSoftImage( SIHandle ) ;
	}
}

#endif // DX_NON_ASYNCLOAD


// LoadSoftImageToMem のグローバル変数にアクセスしないバージョン
extern int LoadSoftImageToMem_UseGParam(
	const void *FileImage,
	int FileImageSize,
	int ASyncLoadFlag
)
{
	int SIHandle ;

	CheckActiveState() ;

	// ハンドルの作成
	SIHandle = AddHandle( DX_HANDLETYPE_SOFTIMAGE, FALSE, -1 ) ;
	if( SIHandle == -1 )
	{
		return -1 ;
	}

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;

		// パラメータに必要なメモリのサイズを算出
		Addr = 0 ;
		AddASyncLoadParamInt( NULL, &Addr, SIHandle ) ;
		AddASyncLoadParamConstVoidP( NULL, &Addr, FileImage ) ;
		AddASyncLoadParamInt( NULL, &Addr, FileImageSize ) ;

		// メモリの確保
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// 処理に必要な情報をセット
		AParam->ProcessFunction = LoadSoftImageToMem_ASync ;
		Addr = 0 ;
		AddASyncLoadParamInt( AParam->Data, &Addr, SIHandle ) ;
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
		IncASyncLoadCount( SIHandle, AParam->Index ) ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		if( LoadSoftImageToMem_Static( SIHandle, FileImage, FileImageSize, FALSE ) < 0 )
			goto ERR ;
	}

	// 終了
	return SIHandle ;

ERR :
	NS_DeleteSoftImage( SIHandle ) ;

	return -1 ;
}

// ソフトウエアで扱うイメージのメモリからの読み込み( -1:エラー  -1以外:イメージハンドル )
extern	int		NS_LoadSoftImageToMem( const void *FileImage, int FileImageSize )
{
	return LoadSoftImageToMem_UseGParam( FileImage, FileImageSize, GetASyncLoadFlag() ) ;
}









// LoadARGB8ColorSoftImageToMem の実処理関数
static int LoadARGB8ColorSoftImageToMem_Static(
	int SIHandle,
	const void *FileImage,
	int FileImageSize,
	int ASyncThread
)
{
	SOFTIMAGE *SoftImg ;

	// 新しいソフトイメージハンドルの取得
	if( ASyncThread )
	{
		if( SFTIMGCHK_ASYNC( SIHandle, SoftImg ) )
			return -1 ;
	}
	else
	{
		if( SFTIMGCHK( SIHandle, SoftImg ) )
			return -1 ;
	}

	// CreateBaseImageToMem でメモリから読み込み
	if( NS_CreateBaseImageToMem( FileImage, FileImageSize, &SoftImg->BaseImage, FALSE ) == -1 )
		return -1 ;

	// 読み込んだ画像の形式が ARGB8 ではなかったら ARGB8 形式に変換
	if( SoftImg->BaseImage.ColorData.Format				!= DX_BASEIMAGE_FORMAT_NORMAL ||
		SoftImg->BaseImage.ColorData.ChannelNum			!= 0 ||
		SoftImg->BaseImage.ColorData.ChannelBitDepth	!= 0 ||
		SoftImg->BaseImage.ColorData.FloatTypeFlag		!= 0 ||
		SoftImg->BaseImage.ColorData.ColorBitDepth		!= 32 ||
		SoftImg->BaseImage.ColorData.PixelByte			!= 4 ||
		SoftImg->BaseImage.ColorData.MaxPaletteNo		!= 0 ||
		SoftImg->BaseImage.ColorData.NoneMask			!= 0x00000000 ||
		SoftImg->BaseImage.ColorData.AlphaMask			!= 0xff000000 ||
		SoftImg->BaseImage.ColorData.RedMask			!= 0x00ff0000 ||
		SoftImg->BaseImage.ColorData.GreenMask			!= 0x0000ff00 ||
		SoftImg->BaseImage.ColorData.BlueMask			!= 0x000000ff )
	{
		BASEIMAGE TempBaseImage ;

		// 同じサイズの ARGB8形式の基本イメージを作成
		TempBaseImage = SoftImg->BaseImage ;
		if( NS_CreateARGB8ColorBaseImage( SoftImg->BaseImage.Width, SoftImg->BaseImage.Height, &SoftImg->BaseImage ) == -1 )
		{
			NS_ReleaseBaseImage( &TempBaseImage ) ;
			return -1 ;
		}

		// ARGB8 形式に変換
		NS_BltBaseImage( 0, 0, &TempBaseImage, &SoftImg->BaseImage ) ;

		// 読み込んだ基本イメージは解放
		NS_ReleaseBaseImage( &TempBaseImage ) ;
	}

	// 正常終了
	return 0 ;
}

#ifndef DX_NON_ASYNCLOAD

// LoadARGB8ColorSoftImageToMem の非同期読み込みスレッドから呼ばれる関数
static void LoadARGB8ColorSoftImageToMem_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	int SIHandle ;
	const void *FileImage ;
	int FileImageSize ;
	int Addr ;
	int Result ;

	Addr = 0 ;
	SIHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	FileImage = GetASyncLoadParamVoidP( AParam->Data, &Addr ) ;
	FileImageSize = GetASyncLoadParamInt( AParam->Data, &Addr ) ;

	Result = LoadARGB8ColorSoftImageToMem_Static( SIHandle, FileImage, FileImageSize, TRUE ) ;

	DecASyncLoadCount( SIHandle ) ;
	if( Result < 0 )
	{
		NS_DeleteSoftImage( SIHandle ) ;
	}
}

#endif // DX_NON_ASYNCLOAD


// LoadARGB8ColorSoftImageToMem のグローバル変数にアクセスしないバージョン
extern int LoadARGB8ColorSoftImageToMem_UseGParam(
	const void *FileImage,
	int FileImageSize,
	int ASyncLoadFlag
)
{
	int SIHandle ;

	CheckActiveState() ;

	// ハンドルの作成
	SIHandle = AddHandle( DX_HANDLETYPE_SOFTIMAGE, FALSE, -1 ) ;
	if( SIHandle == -1 )
	{
		return -1 ;
	}

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;

		// パラメータに必要なメモリのサイズを算出
		Addr = 0 ;
		AddASyncLoadParamInt( NULL, &Addr, SIHandle ) ;
		AddASyncLoadParamConstVoidP( NULL, &Addr, FileImage ) ;
		AddASyncLoadParamInt( NULL, &Addr, FileImageSize ) ;

		// メモリの確保
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// 処理に必要な情報をセット
		AParam->ProcessFunction = LoadARGB8ColorSoftImageToMem_ASync ;
		Addr = 0 ;
		AddASyncLoadParamInt( AParam->Data, &Addr, SIHandle ) ;
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
		IncASyncLoadCount( SIHandle, AParam->Index ) ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		if( LoadARGB8ColorSoftImageToMem_Static( SIHandle, FileImage, FileImageSize, FALSE ) < 0 )
			goto ERR ;
	}

	// 終了
	return SIHandle ;

ERR :
	NS_DeleteSoftImage( SIHandle ) ;

	return -1 ;
}

// ソフトウエアで扱うイメージのメモリからの読み込み( -1:エラー  -1以外:イメージハンドル )( 読み込んだ画像が RGBA8 以外のフォーマットだった場合は RGBA8 カラーに変換 )
extern	int		NS_LoadARGB8ColorSoftImageToMem( const void *FileImage, int FileImageSize )
{
	return LoadARGB8ColorSoftImageToMem_UseGParam( FileImage, FileImageSize, GetASyncLoadFlag() ) ;
}









// LoadXRGB8ColorSoftImageToMem の実処理関数
static int LoadXRGB8ColorSoftImageToMem_Static(
	int SIHandle,
	const void *FileImage,
	int FileImageSize,
	int ASyncThread
)
{
	SOFTIMAGE *SoftImg ;

	// 新しいソフトイメージハンドルの取得
	if( ASyncThread )
	{
		if( SFTIMGCHK_ASYNC( SIHandle, SoftImg ) )
			return -1 ;
	}
	else
	{
		if( SFTIMGCHK( SIHandle, SoftImg ) )
			return -1 ;
	}

	// CreateBaseImageToMem でメモリから読み込み
	if( NS_CreateBaseImageToMem( FileImage, FileImageSize, &SoftImg->BaseImage, FALSE ) == -1 )
		return -1 ;

	// 読み込んだ画像の形式が XRGB8 ではなかったら XRGB8 形式に変換
	if( SoftImg->BaseImage.ColorData.Format				!= DX_BASEIMAGE_FORMAT_NORMAL ||
		SoftImg->BaseImage.ColorData.ChannelNum			!= 0 ||
		SoftImg->BaseImage.ColorData.ChannelBitDepth	!= 0 ||
		SoftImg->BaseImage.ColorData.FloatTypeFlag		!= 0 ||
		SoftImg->BaseImage.ColorData.ColorBitDepth		!= 32 ||
		SoftImg->BaseImage.ColorData.PixelByte			!= 4 ||
		SoftImg->BaseImage.ColorData.MaxPaletteNo		!= 0 ||
		SoftImg->BaseImage.ColorData.NoneMask			!= 0xff000000 ||
		SoftImg->BaseImage.ColorData.AlphaMask			!= 0x00000000 ||
		SoftImg->BaseImage.ColorData.RedMask			!= 0x00ff0000 ||
		SoftImg->BaseImage.ColorData.GreenMask			!= 0x0000ff00 ||
		SoftImg->BaseImage.ColorData.BlueMask			!= 0x000000ff )
	{
		BASEIMAGE TempBaseImage ;

		// 同じサイズの XRGB8形式の基本イメージを作成
		TempBaseImage = SoftImg->BaseImage ;
		if( NS_CreateXRGB8ColorBaseImage( SoftImg->BaseImage.Width, SoftImg->BaseImage.Height, &SoftImg->BaseImage ) == -1 )
		{
			NS_ReleaseBaseImage( &TempBaseImage ) ;
			return -1 ;
		}

		// XRGB8 形式に変換
		NS_BltBaseImage( 0, 0, &TempBaseImage, &SoftImg->BaseImage ) ;

		// 読み込んだ基本イメージは解放
		NS_ReleaseBaseImage( &TempBaseImage ) ;
	}

	// 正常終了
	return 0 ;
}

#ifndef DX_NON_ASYNCLOAD

// LoadXRGB8ColorSoftImageToMem の非同期読み込みスレッドから呼ばれる関数
static void LoadXRGB8ColorSoftImageToMem_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	int SIHandle ;
	const void *FileImage ;
	int FileImageSize ;
	int Addr ;
	int Result ;

	Addr = 0 ;
	SIHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	FileImage = GetASyncLoadParamVoidP( AParam->Data, &Addr ) ;
	FileImageSize = GetASyncLoadParamInt( AParam->Data, &Addr ) ;

	Result = LoadXRGB8ColorSoftImageToMem_Static( SIHandle, FileImage, FileImageSize, TRUE ) ;

	DecASyncLoadCount( SIHandle ) ;
	if( Result < 0 )
	{
		NS_DeleteSoftImage( SIHandle ) ;
	}
}

#endif // DX_NON_ASYNCLOAD


// LoadXRGB8ColorSoftImageToMem のグローバル変数にアクセスしないバージョン
extern int LoadXRGB8ColorSoftImageToMem_UseGParam(
	const void *FileImage,
	int FileImageSize,
	int ASyncLoadFlag
)
{
	int SIHandle ;

	CheckActiveState() ;

	// ハンドルの作成
	SIHandle = AddHandle( DX_HANDLETYPE_SOFTIMAGE, FALSE, -1 ) ;
	if( SIHandle == -1 )
	{
		return -1 ;
	}

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;

		// パラメータに必要なメモリのサイズを算出
		Addr = 0 ;
		AddASyncLoadParamInt( NULL, &Addr, SIHandle ) ;
		AddASyncLoadParamConstVoidP( NULL, &Addr, FileImage ) ;
		AddASyncLoadParamInt( NULL, &Addr, FileImageSize ) ;

		// メモリの確保
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// 処理に必要な情報をセット
		AParam->ProcessFunction = LoadXRGB8ColorSoftImageToMem_ASync ;
		Addr = 0 ;
		AddASyncLoadParamInt( AParam->Data, &Addr, SIHandle ) ;
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
		IncASyncLoadCount( SIHandle, AParam->Index ) ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		if( LoadXRGB8ColorSoftImageToMem_Static( SIHandle, FileImage, FileImageSize, FALSE ) < 0 )
			goto ERR ;
	}

	// 終了
	return SIHandle ;

ERR :
	NS_DeleteSoftImage( SIHandle ) ;

	return -1 ;
}

// ソフトウエアで扱うイメージのメモリからの読み込み( -1:エラー  -1以外:イメージハンドル )( 読み込んだ画像が RGBA8 以外のフォーマットだった場合は RGBA8 カラーに変換 )
extern	int		NS_LoadXRGB8ColorSoftImageToMem( const void *FileImage, int FileImageSize )
{
	return LoadXRGB8ColorSoftImageToMem_UseGParam( FileImage, FileImageSize, GetASyncLoadFlag() ) ;
}












// ソフトウエアで扱うイメージの作成( -1:エラー  -1以外:イメージハンドル )
extern	int		NS_MakeSoftImage( int SizeX, int SizeY )
{
	SOFTIMAGE *SoftImg ;
	int NewHandle ;
	int Result ;

	// 新しいソフトイメージハンドルの取得
	NewHandle = AddHandle( DX_HANDLETYPE_SOFTIMAGE, FALSE, -1 ) ;
	if( NewHandle == -1 )
	{
		return -1 ;
	}
	if( SFTIMGCHK_ASYNC( NewHandle, SoftImg ) )
	{
		return -1 ;
	}

	// CreateARGB8ColorBaseImage を使用
	Result = NS_CreateARGB8ColorBaseImage( SizeX, SizeY, &SoftImg->BaseImage ) ;
	if( Result == -1 )
	{
		SubHandle( NewHandle ) ;
		return -1 ;
	}

	// 成功したらハンドルを返す
	return NewHandle ;
}

// ソフトウエアイメージハンドルの作成( RGBA 各チャンネル 32bit 浮動小数点型 カラー )
extern	int		NS_MakeARGBF32ColorSoftImage( int SizeX, int SizeY )
{
	SOFTIMAGE *SoftImg ;
	int NewHandle ;
	int Result ;

	// 新しいソフトイメージハンドルの取得
	NewHandle = AddHandle( DX_HANDLETYPE_SOFTIMAGE, FALSE, -1 ) ;
	if( NewHandle == -1 )
	{
		return -1 ;
	}
	if( SFTIMGCHK_ASYNC( NewHandle, SoftImg ) )
	{
		return -1 ;
	}

	// CreateARGBF32ColorBaseImage を使用
	Result = NS_CreateARGBF32ColorBaseImage( SizeX, SizeY, &SoftImg->BaseImage ) ;
	if( Result == -1 )
	{
		SubHandle( NewHandle ) ;
		return -1 ;
	}

	// 成功したらハンドルを返す
	return NewHandle ;
}

// ソフトウエアイメージハンドルの作成( RGBA 各チャンネル 16bit 浮動小数点型 カラー )
extern	int		NS_MakeARGBF16ColorSoftImage( int SizeX, int SizeY )
{
	SOFTIMAGE *SoftImg ;
	int NewHandle ;
	int Result ;

	// 新しいソフトイメージハンドルの取得
	NewHandle = AddHandle( DX_HANDLETYPE_SOFTIMAGE, FALSE, -1 ) ;
	if( NewHandle == -1 )
	{
		return -1 ;
	}
	if( SFTIMGCHK_ASYNC( NewHandle, SoftImg ) )
	{
		return -1 ;
	}

	// CreateARGBF16ColorBaseImage を使用
	Result = NS_CreateARGBF16ColorBaseImage( SizeX, SizeY, &SoftImg->BaseImage ) ;
	if( Result == -1 )
	{
		SubHandle( NewHandle ) ;
		return -1 ;
	}

	// 成功したらハンドルを返す
	return NewHandle ;
}

// ソフトウエアで扱うイメージの作成( RGBA8 カラー )
extern	int		NS_MakeARGB8ColorSoftImage( int SizeX, int SizeY )
{
	// NS_MakeSoftImage と同じ
	return NS_MakeSoftImage( SizeX, SizeY ) ;
}

// ソフトウエアで扱うイメージの作成( XRGB8 カラー )
extern	int		NS_MakeXRGB8ColorSoftImage( int SizeX, int SizeY )
{
	SOFTIMAGE *SoftImg ;
	int NewHandle ;
	int Result ;

	// 新しいソフトイメージハンドルの取得
	NewHandle = AddHandle( DX_HANDLETYPE_SOFTIMAGE, FALSE, -1 ) ;
	if( NewHandle == -1 )
	{
		return -1 ;
	}
	if( SFTIMGCHK_ASYNC( NewHandle, SoftImg ) )
	{
		return -1 ;
	}

	// CreateXRGB8ColorBaseImage を使用
	Result = NS_CreateXRGB8ColorBaseImage( SizeX, SizeY, &SoftImg->BaseImage ) ;
	if( Result == -1 )
	{
		SubHandle( NewHandle ) ;
		return -1 ;
	}

	// 成功したらハンドルを返す
	return NewHandle ;
}

// ソフトウエアで扱うイメージの作成( ARGB4 カラー )
extern	int		NS_MakeARGB4ColorSoftImage( int SizeX, int SizeY )
{
	SOFTIMAGE *SoftImg ;
	int NewHandle ;
	int Result ;

	// 新しいソフトイメージハンドルの取得
	NewHandle = AddHandle( DX_HANDLETYPE_SOFTIMAGE, FALSE, -1 ) ;
	if( NewHandle == -1 )
	{
		return -1 ;
	}
	if( SFTIMGCHK_ASYNC( NewHandle, SoftImg ) )
	{
		return -1 ;
	}

	// CreateARGB4ColorBaseImage を使用
	Result = NS_CreateARGB4ColorBaseImage( SizeX, SizeY, &SoftImg->BaseImage ) ;
	if( Result == -1 )
	{
		SubHandle( NewHandle ) ;
		return -1 ;
	}

	// 成功したらハンドルを返す
	return NewHandle ;
}

// ソフトウエアイメージハンドルの作成( A1R5G5B5 カラー )
extern	int	NS_MakeA1R5G5B5ColorSoftImage( int SizeX, int SizeY )
{
	SOFTIMAGE *SoftImg ;
	int NewHandle ;
	int Result ;

	// 新しいソフトイメージハンドルの取得
	NewHandle = AddHandle( DX_HANDLETYPE_SOFTIMAGE, FALSE, -1 ) ;
	if( NewHandle == -1 )
	{
		return -1 ;
	}
	if( SFTIMGCHK_ASYNC( NewHandle, SoftImg ) )
	{
		return -1 ;
	}

	// CreateA1R5G5B5ColorBaseImage を使用
	Result = NS_CreateA1R5G5B5ColorBaseImage( SizeX, SizeY, &SoftImg->BaseImage ) ;
	if( Result == -1 )
	{
		SubHandle( NewHandle ) ;
		return -1 ;
	}

	// 成功したらハンドルを返す
	return NewHandle ;
}

// ソフトウエアイメージハンドルの作成( X1R5G5B5 カラー )
extern	int	NS_MakeX1R5G5B5ColorSoftImage( int SizeX, int SizeY )
{
	SOFTIMAGE *SoftImg ;
	int NewHandle ;
	int Result ;

	// 新しいソフトイメージハンドルの取得
	NewHandle = AddHandle( DX_HANDLETYPE_SOFTIMAGE, FALSE, -1 ) ;
	if( NewHandle == -1 )
	{
		return -1 ;
	}
	if( SFTIMGCHK_ASYNC( NewHandle, SoftImg ) )
	{
		return -1 ;
	}

	// CreateX1R5G5B5ColorBaseImage を使用
	Result = NS_CreateX1R5G5B5ColorBaseImage( SizeX, SizeY, &SoftImg->BaseImage ) ;
	if( Result == -1 )
	{
		SubHandle( NewHandle ) ;
		return -1 ;
	}

	// 成功したらハンドルを返す
	return NewHandle ;
}

// ソフトウエアイメージハンドルの作成( R5G5B5A1 カラー )
extern	int	NS_MakeR5G5B5A1ColorSoftImage( int SizeX, int SizeY )
{
	SOFTIMAGE *SoftImg ;
	int NewHandle ;
	int Result ;

	// 新しいソフトイメージハンドルの取得
	NewHandle = AddHandle( DX_HANDLETYPE_SOFTIMAGE, FALSE, -1 ) ;
	if( NewHandle == -1 )
	{
		return -1 ;
	}
	if( SFTIMGCHK_ASYNC( NewHandle, SoftImg ) )
	{
		return -1 ;
	}

	// CreateR5G5B5A1ColorBaseImage を使用
	Result = NS_CreateR5G5B5A1ColorBaseImage( SizeX, SizeY, &SoftImg->BaseImage ) ;
	if( Result == -1 )
	{
		SubHandle( NewHandle ) ;
		return -1 ;
	}

	// 成功したらハンドルを返す
	return NewHandle ;
}

// ソフトウエアイメージハンドルの作成( R5G6B5 カラー )
extern	int	NS_MakeR5G6B5ColorSoftImage( int SizeX, int SizeY )
{
	SOFTIMAGE *SoftImg ;
	int NewHandle ;
	int Result ;

	// 新しいソフトイメージハンドルの取得
	NewHandle = AddHandle( DX_HANDLETYPE_SOFTIMAGE, FALSE, -1 ) ;
	if( NewHandle == -1 )
	{
		return -1 ;
	}
	if( SFTIMGCHK_ASYNC( NewHandle, SoftImg ) )
	{
		return -1 ;
	}

	// CreateR5G6B5ColorBaseImage を使用
	Result = NS_CreateR5G6B5ColorBaseImage( SizeX, SizeY, &SoftImg->BaseImage ) ;
	if( Result == -1 )
	{
		SubHandle( NewHandle ) ;
		return -1 ;
	}

	// 成功したらハンドルを返す
	return NewHandle ;
}

// ソフトウエアで扱うイメージの作成( RGB8 カラー )
extern	int		NS_MakeRGB8ColorSoftImage( int SizeX, int SizeY )
{
	SOFTIMAGE *SoftImg ;
	int NewHandle ;
	int Result ;

	// 新しいソフトイメージハンドルの取得
	NewHandle = AddHandle( DX_HANDLETYPE_SOFTIMAGE, FALSE, -1 ) ;
	if( NewHandle == -1 )
	{
		return -1 ;
	}
	if( SFTIMGCHK_ASYNC( NewHandle, SoftImg ) )
	{
		return -1 ;
	}

	// CreateRGB8ColorBaseImage を使用
	Result = NS_CreateRGB8ColorBaseImage( SizeX, SizeY, &SoftImg->BaseImage ) ;
	if( Result == -1 )
	{
		SubHandle( NewHandle ) ;
		return -1 ;
	}

	// 成功したらハンドルを返す
	return NewHandle ;
}

// ソフトウエアで扱うイメージの作成( パレット２５６色 カラー )
extern	int		NS_MakePAL8ColorSoftImage( int SizeX, int SizeY, int UseAlpha )
{
	SOFTIMAGE *SoftImg ;
	int NewHandle ;
	int Result ;

	// 新しいソフトイメージハンドルの取得
	NewHandle = AddHandle( DX_HANDLETYPE_SOFTIMAGE, FALSE, -1 ) ;
	if( NewHandle == -1 )
	{
		return -1 ;
	}
	if( SFTIMGCHK_ASYNC( NewHandle, SoftImg ) )
	{
		return -1 ;
	}

	// CreatePAL8ColorBaseImage を使用
	Result = NS_CreatePAL8ColorBaseImage( SizeX, SizeY, &SoftImg->BaseImage, UseAlpha ) ;
	if( Result == -1 )
	{
		SubHandle( NewHandle ) ;
		return -1 ;
	}

	// 成功したらハンドルを返す
	return NewHandle ;
}

// ソフトウエアで扱うイメージの解放
extern	int		NS_DeleteSoftImage( int SIHandle )
{
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;
	
	// ハンドルの解放
	SubHandle( SIHandle ) ; 

	// 終了
	return 0 ;
}

// ソフトウエアで扱うイメージのサイズを取得する
extern	int		NS_GetSoftImageSize( int SIHandle, int *Width, int *Height )
{
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	if( Width  ) *Width  = SoftImg->BaseImage.Width ;
	if( Height ) *Height = SoftImg->BaseImage.Height ;

	// 終了
	return 0 ;
}

// ソフトウエアで扱うイメージがパレット画像かどうかを取得する( TRUE:パレット画像  FALSE:パレット画像じゃない )
extern	int		NS_CheckPaletteSoftImage( int SIHandle )
{
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	return SoftImg->BaseImage.ColorData.PixelByte == 1 ? TRUE : FALSE ;
}

// ソフトウエアで扱うイメージのフォーマットにα要素があるかどうかを取得する( TRUE:ある  FALSE:ない )
extern	int		NS_CheckAlphaSoftImage( int SIHandle )
{
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	return SoftImg->BaseImage.ColorData.AlphaMask != 0 && SoftImg->BaseImage.ColorData.PixelByte >= 2 ? TRUE : FALSE ;
}

// 描画対象の画面から指定領域のグラフィックをソフトウエアで扱うイメージに転送する
extern	int		NS_GetDrawScreenSoftImage( int x1, int y1, int x2, int y2, int SIHandle )
{
#ifndef DX_NON_GRAPHICS
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	return NS_GetDrawScreenBaseImage( x1, y1, x2, y2, &SoftImg->BaseImage ) ;
#else // DX_NON_GRAPHICS
	return -1 ;
#endif // DX_NON_GRAPHICS
}

// 描画対象の画面から指定領域をソフトウエアイメージハンドルに転送する( 転送先座標指定版 )
extern	int		NS_GetDrawScreenSoftImageDestPos( int x1, int y1, int x2, int y2, int SIHandle, int DestX, int DestY )
{
#ifndef DX_NON_GRAPHICS
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	return NS_GetDrawScreenBaseImageDestPos( x1, y1, x2, y2, &SoftImg->BaseImage, DestX, DestY ) ;
#else // DX_NON_GRAPHICS
	return -1 ;
#endif // DX_NON_GRAPHICS
}

#ifdef __WINDOWS__

// ソフトウエアで扱うイメージを使用して UpdateLayerdWindow を行う
extern	int		NS_UpdateLayerdWindowForSoftImage( int SIHandle )
{
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	return NS_UpdateLayerdWindowForBaseImage( &SoftImg->BaseImage ) ;
}

// ソフトウエアイメージハンドルの指定の範囲を使用して UpdateLayerdWindow を行う
extern	int		NS_UpdateLayerdWindowForSoftImageRect( int SIHandle, int x1, int y1, int x2, int y2 )
{
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	return NS_UpdateLayerdWindowForBaseImageRect( &SoftImg->BaseImage, x1, y1, x2, y2 ) ;
}

// 乗算済みアルファのソフトウエアで扱うイメージを使用して UpdateLayerdWindow を行う
extern	int		NS_UpdateLayerdWindowForPremultipliedAlphaSoftImage( int SIHandle )
{
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	return NS_UpdateLayerdWindowForPremultipliedAlphaBaseImage( &SoftImg->BaseImage ) ;
}

// 乗算済みアルファのソフトウエアイメージハンドルの指定の範囲を使用して UpdateLayerdWindow を行う
extern	int		NS_UpdateLayerdWindowForPremultipliedAlphaSoftImageRect( int SIHandle, int x1, int y1, int x2, int y2 )
{
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	return NS_UpdateLayerdWindowForPremultipliedAlphaBaseImageRect( &SoftImg->BaseImage, x1, y1, x2, y2 ) ;
}


// デスクトップキャプチャ

// デスクトップの指定の領域をソフトウエアイメージハンドルに転送する
extern	int		NS_GetDesktopScreenSoftImage( int x1, int y1, int x2, int y2, int SIHandle, int DestX, int DestY )
{
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	return NS_GetDesktopScreenBaseImage( x1, y1, x2, y2, &SoftImg->BaseImage, DestX, DestY ) ;
}

#endif // __WINDOWS__

// ソフトウエアで扱うイメージを指定色で塗りつぶす
extern	int		NS_FillSoftImage( int SIHandle, int r, int g, int b, int a )
{
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	return NS_FillBaseImage( &SoftImg->BaseImage, r, g, b, a ) ;
}

// ソフトウエアで扱うイメージの指定の領域を０クリアする
extern	int		NS_ClearRectSoftImage( int SIHandle, int x, int y, int w, int h )
{
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	return NS_ClearRectBaseImage( &SoftImg->BaseImage, x, y, w, h ) ;
}

// ソフトウエアで扱うイメージのパレットを取得する
extern	int		NS_GetPaletteSoftImage( int SIHandle, int PaletteNo, int *r, int *g, int *b, int *a )
{
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	return NS_GetPaletteBaseImage( &SoftImg->BaseImage, PaletteNo, r, g, b, a ) ;
}

// ソフトウエアで扱うイメージのパレットをセットする
extern	int		NS_SetPaletteSoftImage( int SIHandle, int PaletteNo, int  r, int  g, int  b, int  a )
{
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	return NS_SetPaletteBaseImage( &SoftImg->BaseImage, PaletteNo, r, g, b, a ) ;
}

// ソフトウエアで扱うイメージの指定座標にドットを描画する
extern	int		NS_DrawPixelSoftImage( int SIHandle, int x, int y, int  r, int  g, int  b, int  a )
{
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	return NS_SetPixelBaseImage( &SoftImg->BaseImage, x, y, r, g, b, a ) ;
}

// ソフトウエアイメージハンドルの指定座標にドットを描画する(各色要素は浮動小数点数)
extern	int		NS_DrawPixelSoftImageF( int SIHandle, int x, int y, float  r, float  g, float  b, float  a )
{
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	return NS_SetPixelBaseImageF( &SoftImg->BaseImage, x, y, r, g, b, a ) ;
}

// ソフトウエアで扱うイメージの指定座標にドットを描画する
extern	void	NS_DrawPixelSoftImage_Unsafe_XRGB8( int SIHandle, int x, int y, int  r, int  g, int  b )
{
	BASEIMAGE *BaseImage = &( ( SOFTIMAGE * )HandleManageArray[ DX_HANDLETYPE_SOFTIMAGE ].Handle[ SIHandle & DX_HANDLEINDEX_MASK ] )->BaseImage ;

	*((DWORD *)((BYTE *)BaseImage->GraphData + BaseImage->Pitch * y + x * 4)) = ( DWORD )( ( r << 16 ) | ( g << 8 ) | b ) ;
}

// ソフトウエアで扱うイメージの指定座標にドットを描画する
extern	void	NS_DrawPixelSoftImage_Unsafe_ARGB8( int SIHandle, int x, int y, int  r, int  g, int  b, int a )
{
	BASEIMAGE *BaseImage = &( ( SOFTIMAGE * )HandleManageArray[ DX_HANDLETYPE_SOFTIMAGE ].Handle[ SIHandle & DX_HANDLEINDEX_MASK ] )->BaseImage ;

	*((DWORD *)((BYTE *)BaseImage->GraphData + BaseImage->Pitch * y + x * 4)) = ( DWORD )( ( a << 24 ) | ( r << 16 ) | ( g << 8 ) | b ) ;
}

// ソフトウエアで扱うイメージの指定座標にドットを描画する(パレット画像用、有効値は０〜２５５)
extern	int		NS_DrawPixelPalCodeSoftImage( int SIHandle, int x, int y, int palNo )
{
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	return NS_SetPixelPalCodeBaseImage( &SoftImg->BaseImage,  x,  y,  palNo ) ;
}

// ソフトウエアで扱うイメージの指定座標の色コードを取得する(パレット画像用、戻り値は０〜２５５)
extern	int		NS_GetPixelPalCodeSoftImage( int SIHandle, int x, int y )
{
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	return NS_GetPixelPalCodeBaseImage( &SoftImg->BaseImage,  x,  y ) ;
}

// ソフトウエアで扱うイメージの実イメージが格納されているメモリ領域の先頭アドレスを取得する
extern void *NS_GetImageAddressSoftImage( int SIHandle )
{
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return NULL ;

	return SoftImg->BaseImage.GraphData ;
}

// ソフトウエアイメージハンドルのメモリに格納されている画像データの1ライン辺りのバイト数を取得する
extern int NS_GetPitchSoftImage( int SIHandle )
{
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	return SoftImg->BaseImage.Pitch ;
}

// ソフトウエアで扱うイメージの指定座標の色を取得する
extern	int		NS_GetPixelSoftImage(  int SIHandle, int x, int y, int *r, int *g, int *b, int *a )
{
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	return NS_GetPixelBaseImage( &SoftImg->BaseImage, x, y, r, g, b, a ) ;
}

// ソフトウエアイメージハンドルの指定座標の色を取得する(各色要素は浮動小数点数)
extern	int		NS_GetPixelSoftImageF( int SIHandle, int x, int y, float *r, float *g, float *b, float *a )
{
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	return NS_GetPixelBaseImageF( &SoftImg->BaseImage, x, y, r, g, b, a ) ;
}

// ソフトウエアで扱うイメージの指定座標の色を取得する
extern	void		NS_GetPixelSoftImage_Unsafe_XRGB8(  int SIHandle, int x, int y, int *r, int *g, int *b )
{
	BASEIMAGE *BaseImage = &( ( SOFTIMAGE * )HandleManageArray[ DX_HANDLETYPE_SOFTIMAGE ].Handle[ SIHandle & DX_HANDLEINDEX_MASK ] )->BaseImage ;
	unsigned int Color ;

	Color = *((DWORD *)((BYTE *)BaseImage->GraphData + BaseImage->Pitch * y + x * 4))  ;
	*r = ( int )( ( Color >> 16 ) & 0xff ) ;
	*g = ( int )( ( Color >>  8 ) & 0xff ) ;
	*b = ( int )(   Color         & 0xff ) ;
}

// ソフトウエアで扱うイメージの指定座標の色を取得する
extern	void		NS_GetPixelSoftImage_Unsafe_ARGB8(  int SIHandle, int x, int y, int *r, int *g, int *b, int *a )
{
	BASEIMAGE *BaseImage = &( ( SOFTIMAGE * )HandleManageArray[ DX_HANDLETYPE_SOFTIMAGE ].Handle[ SIHandle & DX_HANDLEINDEX_MASK ] )->BaseImage ;
	unsigned int Color ;

	Color = *((DWORD *)((BYTE *)BaseImage->GraphData + BaseImage->Pitch * y + x * 4))  ;
	*a = ( int )( ( Color >> 24 ) & 0xff ) ;
	*r = ( int )( ( Color >> 16 ) & 0xff ) ;
	*g = ( int )( ( Color >>  8 ) & 0xff ) ;
	*b = ( int )(   Color         & 0xff ) ;
}

// ソフトウエアで扱うイメージの指定座標に線を描画する(各色要素は０〜２５５)
extern int NS_DrawLineSoftImage(  int SIHandle, int x1, int y1, int x2, int y2, int r, int g, int b, int a )
{
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	return NS_DrawLineBaseImage( &SoftImg->BaseImage, x1, y1, x2, y2, r, g, b, a ) ;
}

// ソフトウエアイメージハンドルの指定座標に円を描画する(各色要素は０〜２５５)
extern int NS_DrawCircleSoftImage( int SIHandle, int x, int y, int radius, int r, int g, int b, int a, int FillFlag )
{
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	return NS_DrawCircleBaseImage( &SoftImg->BaseImage, x, y, radius, r, g, b, a, FillFlag ) ;
}

// ソフトウエアで扱うイメージを別のイメージ上に転送する
extern	int		NS_BltSoftImage( int SrcX, int SrcY, int SrcSizeX, int SrcSizeY, int SrcSIHandle, int DestX, int DestY, int DestSIHandle )
{
	SOFTIMAGE *SrcSoftImg, *DestSoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SrcSIHandle,  SrcSoftImg ) ) 
		return -1 ;
	if( SFTIMGCHK( DestSIHandle, DestSoftImg ) )
		return -1 ;

	return NS_BltBaseImage( SrcX, SrcY, SrcSizeX, SrcSizeY, DestX, DestY, &SrcSoftImg->BaseImage, &DestSoftImg->BaseImage ) ;
}

// ソフトウエアで扱うイメージを透過色処理付きで転送する
extern	int		NS_BltSoftImageWithTransColor( int SrcX, int SrcY, int SrcSizeX, int SrcSizeY, int SrcSIHandle, int DestX, int DestY, int DestSIHandle, int Tr, int Tg, int Tb, int Ta )
{
	SOFTIMAGE *SrcSoftImg, *DestSoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SrcSIHandle,  SrcSoftImg ) ) 
		return -1 ;
	if( SFTIMGCHK( DestSIHandle, DestSoftImg ) )
		return -1 ;

	return NS_BltBaseImageWithTransColor( SrcX, SrcY, SrcSizeX, SrcSizeY, DestX, DestY, &SrcSoftImg->BaseImage, &DestSoftImg->BaseImage, Tr, Tg, Tb, Ta ) ;
}

// ソフトウエアで扱うイメージをアルファ値のブレンドを考慮した上で転送する( Opacity は透明度 : 0( 完全透明 ) 〜 255( 完全不透明 ) )( 出力先が ARGB8 形式以外の場合はエラーになります )
extern	int		NS_BltSoftImageWithAlphaBlend( int SrcX, int SrcY, int SrcSizeX, int SrcSizeY, int SrcSIHandle, int DestX, int DestY, int DestSIHandle, int Opacity )
{
	SOFTIMAGE *SrcSoftImg, *DestSoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SrcSIHandle,  SrcSoftImg ) ) 
		return -1 ;
	if( SFTIMGCHK( DestSIHandle, DestSoftImg ) )
		return -1 ;

	return NS_BltBaseImageWithAlphaBlend( SrcX, SrcY, SrcSizeX, SrcSizeY, DestX, DestY, &SrcSoftImg->BaseImage, &DestSoftImg->BaseImage, Opacity ) ;
}

// ソフトウエアで扱うイメージの左右を反転する
extern	int		NS_ReverseSoftImageH( int SIHandle )
{
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	return NS_ReverseBaseImageH( &SoftImg->BaseImage ) ;
}

// ソフトウエアで扱うイメージの上下を反転する
extern	int		NS_ReverseSoftImageV( int SIHandle )
{
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	return NS_ReverseBaseImageV( &SoftImg->BaseImage ) ;
}

// ソフトウエアで扱うイメージの上下左右を反転する
extern	int		NS_ReverseSoftImage( int SIHandle )
{
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	return NS_ReverseBaseImage( &SoftImg->BaseImage ) ;
}

// ソフトウエアで扱うイメージに含まれるピクセルのアルファ値をチェックする( 戻り値   0:画像にアルファ成分が無い  1:画像にアルファ成分があり、すべて最大(255)値  2:画像にアルファ成分があり、存在するアルファ値は最小(0)と最大(255)のみ　3:画像にアルファ成分があり、最小と最大以外の中間の値がある )  
extern	int		NS_CheckPixelAlphaSoftImage( int SIHandle )
{
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	return NS_CheckPixelAlphaBaseImage( &SoftImg->BaseImage ) ;
}

#ifndef DX_NON_FONT

// ソフトウエアで扱うイメージに文字列イメージを転送する
extern	int		NS_BltStringSoftImage( 
	int x, int y, const TCHAR *StrData,
	int DestSIHandle, int DestEdgeSIHandle,
	int VerticalFlag
)
{
#ifdef UNICODE
	return BltStringSoftImage_WCHAR_T(
		x, y, StrData,
		DestSIHandle, DestEdgeSIHandle,
		VerticalFlag
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( StrData, return -1 )

	Result = BltStringSoftImage_WCHAR_T(
		x, y, UseStrDataBuffer,
		DestSIHandle, DestEdgeSIHandle,
		VerticalFlag
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( StrData )

	return Result ;
#endif
}

// ソフトウエアイメージハンドルに文字列を描画する( デフォルトフォントハンドルを使用する )
extern int NS_BltStringSoftImageWithStrLen( int x, int y, const TCHAR *StrData, size_t StrDataLength, int DestSIHandle, int DestEdgeSIHandle, int VerticalFlag )
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( StrData, StrDataLength, return -1 )
	Result = BltStringSoftImage_WCHAR_T(
		x, y, UseStrDataBuffer,
		DestSIHandle, DestEdgeSIHandle,
		VerticalFlag
	) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( StrData )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( StrData, StrDataLength, return -1 )
	Result = BltStringSoftImage_WCHAR_T(
		x, y, UseStrDataBuffer,
		DestSIHandle, DestEdgeSIHandle,
		VerticalFlag
	) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( StrData )
#endif
	return Result ;
}

// ソフトウエアで扱うイメージに文字列イメージを転送する
extern	int		BltStringSoftImage_WCHAR_T( 
	int x, int y, const wchar_t *StrData,
	int DestSIHandle, int DestEdgeSIHandle,
	int VerticalFlag
)
{
	SOFTIMAGE *SoftImg, *EdgeSoftImg = NULL ;

	// アドレスの取得
	if( SFTIMGCHK( DestSIHandle, SoftImg ) )
	{
		return -1 ;
	}

	if( DestEdgeSIHandle != -1 )
	{
		if( SFTIMGCHK( DestEdgeSIHandle, EdgeSoftImg ) )
		{
			return -1 ;
		}
	}

	return FontBaseImageBlt_WCHAR_T( x, y, StrData, -1, &SoftImg->BaseImage, DestEdgeSIHandle != -1 ? &EdgeSoftImg->BaseImage : NULL, VerticalFlag ) ;
}

// ソフトウエアで扱うイメージに文字列イメージを転送する( フォントハンドル使用版 )
extern int NS_BltStringSoftImageToHandle( 
	int x, int y, const TCHAR *StrData,
	int DestSIHandle, int DestEdgeSIHandle,
	int FontHandle, int VerticalFlag
)
{
#ifdef UNICODE
	return BltStringSoftImageToHandle_WCHAR_T(
		x, y, StrData,
		DestSIHandle, DestEdgeSIHandle,
		FontHandle, VerticalFlag
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( StrData, return -1 )

	Result = BltStringSoftImageToHandle_WCHAR_T(
		x, y, UseStrDataBuffer,
		DestSIHandle, DestEdgeSIHandle,
		FontHandle, VerticalFlag
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( StrData )

	return Result ;
#endif
}

// ソフトウエアイメージハンドルに文字列を描画する( フォントハンドル使用版 )
extern int NS_BltStringSoftImageToHandleWithStrLen( int x, int y, const TCHAR *StrData, size_t StrDataLength, int DestSIHandle, int DestEdgeSIHandle, int FontHandle, int VerticalFlag )
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( StrData, StrDataLength, return -1 )
	Result = BltStringSoftImageToHandle_WCHAR_T(
		x, y, UseStrDataBuffer,
		DestSIHandle, DestEdgeSIHandle,
		FontHandle, VerticalFlag
	) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( StrData )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( StrData, StrDataLength, return -1 )
	Result = BltStringSoftImageToHandle_WCHAR_T(
		x, y, UseStrDataBuffer,
		DestSIHandle, DestEdgeSIHandle,
		FontHandle, VerticalFlag
	) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( StrData )
#endif
	return Result ;
}

// ソフトウエアで扱うイメージに文字列イメージを転送する( フォントハンドル使用版 )
extern int BltStringSoftImageToHandle_WCHAR_T(
	int x, int y, const wchar_t *StrData,
	int DestSIHandle, int DestEdgeSIHandle,
	int FontHandle, int VerticalFlag
)
{
	SOFTIMAGE *SoftImg, *EdgeSoftImg = NULL ;

	// アドレスの取得
	if( SFTIMGCHK( DestSIHandle, SoftImg ) )
	{
		return -1 ;
	}

	if( DestEdgeSIHandle != -1 )
	{
		if( SFTIMGCHK( DestEdgeSIHandle, EdgeSoftImg ) )
		{
			return -1 ;
		}
	}

	return FontBaseImageBltToHandle_WCHAR_T( x, y, StrData, -1, &SoftImg->BaseImage, DestEdgeSIHandle != -1 ? &EdgeSoftImg->BaseImage : NULL, FontHandle, VerticalFlag ) ;
}

#endif // DX_NON_FONT


// ソフトウエアで扱うイメージを画面に描画する
extern	int		NS_DrawSoftImage( int x, int y, int SIHandle )
{
#ifndef DX_NON_GRAPHICS
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	return NS_DrawBaseImage( x, y, &SoftImg->BaseImage ) ;
#else // DX_NON_GRAPHICS
	return -1 ;
#endif // DX_NON_GRAPHICS
}

#ifndef DX_NON_SAVEFUNCTION

// ソフトウエアで扱うイメージをＢＭＰ画像ファイルとして保存する
extern	int		NS_SaveSoftImageToBmp( const TCHAR *FilePath, int SIHandle )
{
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	return NS_SaveBaseImageToBmp( FilePath, &SoftImg->BaseImage ) ;
}

// ソフトウエアイメージハンドルをＢＭＰ画像ファイルとして保存する
extern int NS_SaveSoftImageToBmpWithStrLen( const TCHAR *FilePath, size_t FilePathLength, int SIHandle )
{
	int Result ;
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_ONE_BEGIN( FilePath, FilePathLength, return -1 )
	Result = NS_SaveBaseImageToBmp( UseFilePathBuffer, &SoftImg->BaseImage ) ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( FilePath )
	return Result ;
}

// ソフトウエアで扱うイメージをＢＭＰ画像ファイルとして保存する
extern	int		SaveSoftImageToBmp_WCHAR_T( const wchar_t *FilePath, int SIHandle )
{
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	return SaveBaseImageToBmp_WCHAR_T( FilePath, &SoftImg->BaseImage ) ;
}

// ソフトウエアで扱うイメージをＤＤＳ画像ファイルとして保存する
extern	int		NS_SaveSoftImageToDds( const TCHAR *FilePath, int SIHandle )
{
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	return NS_SaveBaseImageToDds( FilePath, &SoftImg->BaseImage ) ;
}

// ソフトウエアイメージハンドルをＤＤＳ画像ファイルとして保存する
extern int NS_SaveSoftImageToDdsWithStrLen( const TCHAR *FilePath, size_t FilePathLength, int SIHandle )
{
	int Result ;
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_ONE_BEGIN( FilePath, FilePathLength, return -1 )
	Result = NS_SaveBaseImageToDds( UseFilePathBuffer, &SoftImg->BaseImage ) ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( FilePath )
	return Result ;
}

// ソフトウエアで扱うイメージをＤＤＳ画像ファイルとして保存する
extern	int		SaveSoftImageToDds_WCHAR_T( const wchar_t *FilePath, int SIHandle )
{
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	return SaveBaseImageToDds_WCHAR_T( FilePath, &SoftImg->BaseImage ) ;
}

#ifndef DX_NON_PNGREAD
// ソフトウエアで扱うイメージをＰＮＧ画像ファイルとして保存する
extern	int		NS_SaveSoftImageToPng( const TCHAR *FilePath, int SIHandle, int CompressionLevel )
{
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	return NS_SaveBaseImageToPng( FilePath, &SoftImg->BaseImage, CompressionLevel ) ;
}

// ソフトウエアイメージハンドルをＰＮＧ画像ファイルとして保存する CompressionLevel = 圧縮率、値が大きいほど高圧縮率高負荷、０は無圧縮,0〜9
extern int NS_SaveSoftImageToPngWithStrLen( const TCHAR *FilePath, size_t FilePathLength, int SIHandle, int CompressionLevel )
{
	int Result ;
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_ONE_BEGIN( FilePath, FilePathLength, return -1 )
	Result = NS_SaveBaseImageToPng( UseFilePathBuffer, &SoftImg->BaseImage, CompressionLevel ) ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( FilePath )
	return Result ;
}

// ソフトウエアで扱うイメージをＰＮＧ画像ファイルとして保存する
extern	int		SaveSoftImageToPng_WCHAR_T( const wchar_t *FilePath, int SIHandle, int CompressionLevel )
{
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	return SaveBaseImageToPng_WCHAR_T( FilePath, &SoftImg->BaseImage, CompressionLevel ) ;
}
#endif

#ifndef DX_NON_JPEGREAD
// ソフトウエアで扱うイメージをＪＰＥＧ画像ファイルとして保存する
extern	int		NS_SaveSoftImageToJpeg( const TCHAR *FilePath, int SIHandle, int Quality, int Sample2x1 )
{
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	return NS_SaveBaseImageToJpeg( FilePath, &SoftImg->BaseImage, Quality, Sample2x1 ) ;
}

// ソフトウエアイメージハンドルをＪＰＥＧ画像ファイルとして保存する Quality = 画質、値が大きいほど低圧縮高画質,0〜100 
extern int NS_SaveSoftImageToJpegWithStrLen( const TCHAR *FilePath, size_t FilePathLength, int SIHandle, int Quality, int Sample2x1 )
{
	int Result ;
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_ONE_BEGIN( FilePath, FilePathLength, return -1 )
	Result = NS_SaveBaseImageToJpeg( UseFilePathBuffer, &SoftImg->BaseImage, Quality, Sample2x1 ) ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( FilePath )
	return Result ;
}

// ソフトウエアで扱うイメージをＪＰＥＧ画像ファイルとして保存する
extern	int		SaveSoftImageToJpeg_WCHAR_T( const wchar_t *FilePath, int SIHandle, int Quality, int Sample2x1 )
{
	SOFTIMAGE *SoftImg ;

	// アドレスの取得
	if( SFTIMGCHK( SIHandle, SoftImg ) )
		return -1 ;

	return SaveBaseImageToJpeg_WCHAR_T( FilePath, &SoftImg->BaseImage, Quality, Sample2x1 ) ;
}
#endif

#endif // DX_NON_SAVEFUNCTION


#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_NON_SOFTIMAGE
