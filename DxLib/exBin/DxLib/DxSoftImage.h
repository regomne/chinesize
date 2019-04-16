// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		ソフトウェアで扱う画像プログラムヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXSOFTIMAGE_H__
#define __DXSOFTIMAGE_H__

// インクルード ------------------------------------------------------------------
#include "DxCompileConfig.h"

#ifndef DX_NON_SOFTIMAGE
#include "DxHandle.h"
#include "DxBaseImage.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

// ソフトイメージハンドルの有効性チェック
#define SFTIMGCHK( HAND, SPOINT )			HANDLECHK(       DX_HANDLETYPE_SOFTIMAGE, HAND, *( ( HANDLEINFO ** )&SPOINT ) )
#define SFTIMGCHK_ASYNC( HAND, SPOINT )		HANDLECHK_ASYNC( DX_HANDLETYPE_SOFTIMAGE, HAND, *( ( HANDLEINFO ** )&SPOINT ) )

// 構造体定義 --------------------------------------------------------------------

// ソフトイメージの情報構造体
struct SOFTIMAGE
{
	HANDLEINFO				HandleInfo ;			// ハンドル共通データ
	BASEIMAGE				BaseImage ;				// 基本イメージ構造体
} ;

// ソフトウエアで扱う画像の情報構造体
struct SOFTIMAGEMANAGE
{
	int						InitializeFlag ;			// 初期化フラグ
} ;

// 内部大域変数宣言 --------------------------------------------------------------

extern SOFTIMAGEMANAGE SoftImageManage ;

// 関数プロトタイプ宣言-----------------------------------------------------------

// 初期化、後始末
extern	int		InitializeSoftImageManage( void ) ;										// ソフトイメージ管理情報の初期化
extern	int		TerminateSoftImageManage( void ) ;										// ソフトイメージ管理情報の後始末

// 読み込み関連
extern	int		InitializeSoftImageHandle( HANDLEINFO *HandleInfo ) ;															// ソフトウエアイメージハンドルの初期化
extern	int		TerminateSoftImageHandle( HANDLEINFO *HandleInfo ) ;															// ソフトウエアイメージハンドルの後始末
extern	int		LoadSoftImage_UseGParam( const wchar_t *FileName, int ASyncLoadFlag = FALSE ) ;									// LoadSoftImage のグローバル変数にアクセスしないバージョン
extern	int		LoadARGB8ColorSoftImage_UseGParam( const wchar_t *FileName, int ASyncLoadFlag = FALSE ) ;						// LoadARGB8ColorSoftImage のグローバル変数にアクセスしないバージョン
extern	int		LoadXRGB8ColorSoftImage_UseGParam( const wchar_t *FileName, int ASyncLoadFlag = FALSE ) ;						// LoadXRGB8ColorSoftImage のグローバル変数にアクセスしないバージョン
extern	int		LoadSoftImageToMem_UseGParam( void *FileImage, int FileImageSize, int ASyncLoadFlag = FALSE ) ;					// LoadSoftImageToMem のグローバル変数にアクセスしないバージョン
extern	int		LoadARGB8ColorSoftImageToMem_UseGParam( void *FileImage, int FileImageSize, int ASyncLoadFlag = FALSE ) ;		// LoadARGB8ColorSoftImageToMem のグローバル変数にアクセスしないバージョン
extern	int		LoadXRGB8ColorSoftImageToMem_UseGParam( void *FileImage, int FileImageSize, int ASyncLoadFlag = FALSE ) ;		// LoadXRGB8ColorSoftImageToMem のグローバル変数にアクセスしないバージョン


// wchar_t版関数
extern	int		LoadSoftImage_WCHAR_T(              const wchar_t *FileName ) ;
extern	int		LoadARGB8ColorSoftImage_WCHAR_T(    const wchar_t *FileName ) ;
extern	int		LoadXRGB8ColorSoftImage_WCHAR_T(    const wchar_t *FileName ) ;
#ifndef DX_NON_FONT
extern	int		BltStringSoftImage_WCHAR_T(         int x, int y, const wchar_t *StrData, int DestSIHandle, int DestEdgeSIHandle = -1 ,                                        int VerticalFlag = FALSE ) ;
extern	int		BltStringSoftImageToHandle_WCHAR_T(	int x, int y, const wchar_t *StrData, int DestSIHandle, int DestEdgeSIHandle /* 縁が必要ない場合は -1 */ , int FontHandle, int VerticalFlag = FALSE ) ;
#endif // DX_NON_FONT

#ifndef DX_NON_SAVEFUNCTION
extern	int		SaveSoftImageToBmp_WCHAR_T(         const wchar_t *FilePath, int SIHandle ) ;
#ifndef DX_NON_PNGREAD
extern	int		SaveSoftImageToPng_WCHAR_T(         const wchar_t *FilePath, int SIHandle, int CompressionLevel ) ;
#endif // DX_NON_PNGREAD
#ifndef DX_NON_JPEGREAD
extern	int		SaveSoftImageToJpeg_WCHAR_T(        const wchar_t *FilePath, int SIHandle, int Quality, int Sample2x1 ) ;
#endif // DX_NON_JPEGREAD
#endif // DX_NON_SAVEFUNCTION



#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_NON_SOFTIMAGE

#endif // __DXSOFTIMAGE_H__

