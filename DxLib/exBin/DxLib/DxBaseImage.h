// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		ＢａｓｅＩｍａｇｅプログラムヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXBASEIMAGE_H__
#define __DXBASEIMAGE_H__

// インクルード ------------------------------------------------------------------
#include "DxCompileConfig.h"
#include "DxLib.h"
#include "DxStatic.h"
#include "DxThread.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

// キューブマップの面の数
#define CUBEMAP_SURFACE_NUM					(6)

// YUV関連の定義
#define YUV_RV								(0)
#define YUV_GU								(1)
#define YUV_GV								(2)
#define YUV_BU								(3)
#define YUV_Y								(4)

#define BASEIM								BaseImageManage

// 構造体定義 --------------------------------------------------------------------

// 汎用画像読み込みに必要なグローバルデータを集めた構造体
struct CREATEBASEIMAGETYPE2_GPARAM
{
	int						( *UserImageLoadFunc4[ MAX_USERIMAGEREAD_FUNCNUM ] )( STREAMDATA *Src, BASEIMAGE *BaseImage ) ;	// ユーザー画像読み込み関数Ver4
	int						UserImageLoadFuncNum4 ;																		// ユーザー画像読み込み関数Ver4の数
	int						GetFormatOnly ;																				// フォーマットの取得のみを行うかどうか( TRUE:フォーマットの取得のみ行う  FALSE:データも読み込む )
} ;

// 画像の読み込みに必要なグローバルデータを纏めた構造体
struct LOADBASEIMAGE_GPARAM
{
	CREATEBASEIMAGETYPE2_GPARAM CreateGraphImageType2GParam ;		// CreateGraphImageType2_UseGParam で使用するデータ

	STREAMDATASHREDTYPE2W	StreamDataShred2 ;						// ストリームデータアクセス用関数２
	STREAMDATASHRED			FileStreamDataShred ;					// ファイルデータアクセス用関数
	STREAMDATASHRED			MemStreamDataShred ;					// メモリデータアクセス用関数

	int						ConvertPremultipliedAlpha ;				// 読み込み処理後に乗算済みアルファの画像に変換するかどうかのフラグ( TRUE:変換処理を行う  FALSE:変換処理を行わない )
	int						AlphaTestImageCreateFlag ;				// アルファテストを使用するグラフィックを作成するかどうかのフラグ( TRUE:αテストを使用する  FALSE:αテストは使用しない )
	DWORD					TransColor ;							// 透過色( AlphaTestImageCreateFlag が TRUE の場合用 )
} ;

// BASEIMAGE + DIB 関係の情報構造体
struct BASEIMAGEMANAGE
{
	int						( *UserImageLoadFunc4[ MAX_USERIMAGEREAD_FUNCNUM ] )( STREAMDATA *Src, BASEIMAGE *BaseImage ) ;																									// ユーザー画像読み込み関数Ver4
	int						UserImageLoadFuncNum4 ;																																										// ユーザー画像読み込み関数Ver4の数
/*
	int						( *UserImageLoadFunc3[ MAX_USERIMAGEREAD_FUNCNUM ] )( void *DataImage, int DataImageSize, int DataImageType, int BmpFlag, BASEIMAGE *BaseImage, BITMAPINFO **BmpInfo, void **GraphData ) ;		// ユーザー画像読み込み関数Ver3
	int						UserImageLoadFuncNum3 ;																																										// ユーザー画像読み込み関数Ver3の数

	int						( *UserImageLoadFunc2[ MAX_USERIMAGEREAD_FUNCNUM ] )( void *Image, int ImageSize, int ImageType, BITMAPINFO **BmpInfo, void **GraphData ) ;													// ユーザー画像読み込み関数Ver2
	int						UserImageLoadFuncNum2 ;																																										// ユーザー画像読み込み関数Ver2の数

	int						( *UserImageLoadFunc[ MAX_USERIMAGEREAD_FUNCNUM ] )( FILE *fp, BITMAPINFO **BmpInfo, void **GraphData ) ;																					// ユーザー画像読み込み関数
	int						UserImageLoadFuncNum ;																																										// ユーザー画像読み込み関数の数
*/

	DX_CRITICAL_SECTION		CriticalProcessHandle ;					// データ処理衝突回避用クリティカルセクション
	int						InitializeFlag ;						// 初期化フラグ

	int						Bmp32AllZeroAlphaToXRGB8 ;				// 32bit bmp 画像のＡ成分がすべて０だったらＡ成分を無視するかどうか
	int						LowLoadFlag ;							// グラフィックの低速読み込みを行うかフラグ
	int						ImageShavedMode ;						// 減色時の拡散パターン
	int						ConvertPremultipliedAlpha ;				// 読み込み処理後に乗算済みアルファの画像に変換するかどうかのフラグ( TRUE:変換処理を行う  FALSE:変換処理を行わない )

	int						AlphaTestImageCreateFlag ;				// アルファテストを使用するグラフィックを作成するかどうかのフラグ( TRUE:αテストを使用する  FALSE:αテストは使用しない )
	DWORD					TransColor ;							// 透過色( AlphaTestImageCreateFlag が TRUE の場合用 )

	BYTE					RgbToPmaTable[ 256 ][ 256 ] ;			// RGB値をPMA値に変換する際に使用するテーブル[ RGB値 ][ A値 ]
	BYTE					PmaToRgbTable[ 256 ][ 256 ] ;			// PMA値をRGB値に変換する際に使用するテーブル[ RGB値 ][ A値 ]
} ;


// テーブル-----------------------------------------------------------------------

extern BYTE YUVLimitTable[ 512 * 2 + 256 ] ;
extern int YUVTable[ 5 ][ 256 ] ;		// 0:rv  1:gu   2:gv   3:bu   4:y

// 内部大域変数宣言 --------------------------------------------------------------

// 環境依存の画像読み込み関数配列
extern int ( *DefaultImageLoadFunc_PF[] )( STREAMDATA *Src, BASEIMAGE *BaseImage, int GetFormatOnly ) ;

// 基本イメージ管理用データ
extern BASEIMAGEMANAGE BaseImageManage ;

// 関数プロトタイプ宣言-----------------------------------------------------------

// 初期化、後始末
extern	int		InitializeBaseImageManage( void ) ;															// 基本イメージ管理情報の初期化
extern	int		TerminateBaseImageManage( void ) ;															// 基本イメージ管理情報の後始末

// 画像読み込み関数
extern	int		SetBmp32AllZeroAlphaToXRGB8( int Flag ) ;															// 32bit bmp 画像のＡ成分がすべて０だったらＡ成分を無視するかどうかのフラグをセットする

extern	int		ScalingBltBaseImage(
					int SrcX1,  int SrcY1,  int SrcX2,  int SrcY2,  BASEIMAGE *SrcBaseImage,
					int DestX1, int DestY1, int DestX2, int DestY2, BASEIMAGE *DestBaseImage, int Bilinear = TRUE ) ;		// 基本イメージデータを拡大転送する
extern	int		GraphHalfScaleBlt(	const COLORDATA *ColorData,
										      void *DestGraphData, int DestPitch,
										const void *SrcGraphData,  int SrcPitch,
										int DestX, int DestY, int SrcX, int SrcY, int SrcWidth, int SrcHeight ) ;	// ２分の１スケーリングしながらグラフィックデータ間転送を行う、そのため奇数倍数の転送矩形は指定できない
extern	int		NoneMaskFill( RECT *Rect, void *ImageData, int Pitch, COLORDATA *ColorData, unsigned int Fill = 0xff ) ;		// 使われていないビットを指定の値で埋める


extern	int		ConvertYV12ToXRGB32( void *YV12Image, int Width, int Height, BASEIMAGE *DestBaseImage ) ;				// YV12 フォーマットのイメージを XRGB32 のビットマップイメージに変換する
extern	int		ConvertNV11ToXRGB32( void *NV11Image, int Width, int Height, BASEIMAGE *DestBaseImage ) ;				// NV11 フォーマットのイメージを XRGB32 のビットマップイメージに変換する
extern	int		ConvertNV12ToXRGB32( void *NV12Image, int Width, int Height, BASEIMAGE *DestBaseImage ) ;				// NV12 フォーマットのイメージを XRGB32 のビットマップイメージに変換する
extern	int		ConvertYUY2ToXRGB32( void *YUY2Image, int Width, int Height, BASEIMAGE *DestBaseImage ) ;				// YUY2 フォーマットのイメージを XRGB32 のビットマップイメージに変換する
extern	int		ConvertUYVYToXRGB32( void *UYVYImage, int Width, int Height, BASEIMAGE *DestBaseImage ) ;				// UYVY フォーマットのイメージを XRGB32 のビットマップイメージに変換する
extern	int		ConvertYVYUToXRGB32( void *YVYUImage, int Width, int Height, BASEIMAGE *DestBaseImage ) ;				// YVYU フォーマットのイメージを XRGB32 のビットマップイメージに変換する

extern	int		ConvertYV12ToYPlane_UVPlane( void *YV12Image, int Width, int Height, void *YBuffer, void *UVBuffer ) ;	// YV12 フォーマットのイメージを Y成分とUV成分のイメージに分離する
extern	int		ConvertNV11ToYPlane_UVPlane( void *NV11Image, int Width, int Height, void *YBuffer, void *UVBuffer ) ;	// NV11 フォーマットのイメージを Y成分とUV成分のイメージに分離する
extern	int		ConvertNV12ToYPlane_UVPlane( void *NV12Image, int Width, int Height, void *YBuffer, void *UVBuffer ) ;	// NV12 フォーマットのイメージを Y成分とUV成分のイメージに分離する
extern	int		ConvertYUY2ToYPlane_UVPlane( void *YUY2Image, int Width, int Height, void *YBuffer, void *UVBuffer ) ;	// YUY2 フォーマットのイメージを Y成分とUV成分のイメージに分離する
extern	int		ConvertUYVYToYPlane_UVPlane( void *UYVYImage, int Width, int Height, void *YBuffer, void *UVBuffer ) ;	// UYVY フォーマットのイメージを Y成分とUV成分のイメージに分離する
extern	int		ConvertYVYUToYPlane_UVPlane( void *YVYUImage, int Width, int Height, void *YBuffer, void *UVBuffer ) ;	// YVYU フォーマットのイメージを Y成分とUV成分のイメージに分離する

extern	void	InitCreateBaseImageType2GParam( CREATEBASEIMAGETYPE2_GPARAM *GParam, int GetFormatOnly ) ;				// CREATEBASEIMAGETYPE2_GPARAM のデータをセットする
extern	void	InitLoadBaseImageGParam( LOADBASEIMAGE_GPARAM *GParam, int GetFormatOnly ) ;							// LOADBASEIMAGE_GPARAM のデータをセットする

// CreateGraphImageType2 のグローバル変数にアクセスしないバージョン
extern	int		CreateGraphImageType2_UseGParam( CREATEBASEIMAGETYPE2_GPARAM *GParam, STREAMDATA *Src, BASEIMAGE *Dest ) ;

// CreateGraphImageOrDIBGraph のグローバル変数にアクセスしないバージョン
extern	int		CreateGraphImageOrDIBGraph_UseGParam(
					LOADBASEIMAGE_GPARAM *GParam,
					const wchar_t *FileName,
					const void *DataImage, int DataImageSize, int DataImageType,
					int BmpFlag, int ReverseFlag, int NotUseTransColor,
					BASEIMAGE *BaseImage, BITMAPINFO **BmpInfo, void **GraphData
				) ;

// CreateGraphImage_plus_Alpha のグローバル変数にアクセスしないバージョン
extern	int		CreateGraphImage_plus_Alpha_UseGParam(
					LOADBASEIMAGE_GPARAM *GParam,
					const wchar_t *FileName,
					const void *RgbImage, int RgbImageSize, int RgbImageType,
					const void *AlphaImage, int AlphaImageSize, int AlphaImageType,
					BASEIMAGE *RgbGraphImage, BASEIMAGE *AlphaGraphImage,
					int ReverseFlag, int NotUseTransColor
				) ;


// wchar_t版関数
extern	int		CreateGraphImageOrDIBGraph_WCHAR_T(    const wchar_t *FileName, const void *DataImage, int DataImageSize, int DataImageType /* LOADIMAGE_TYPE_FILE 等 */ , int BmpFlag, int ReverseFlag, int NotUseTransColor, BASEIMAGE *BaseImage, BITMAPINFO **BmpInfo, void **GraphData ) ;
extern	int		GetImageSize_File_WCHAR_T(             const wchar_t *FileName, int *SizeX, int *SizeY ) ;
extern	int		CreateGraphImage_plus_Alpha_WCHAR_T(   const wchar_t *FileName, const void *RgbBaseImage, int RgbImageSize, int RgbImageType, const void *AlphaImage, int AlphaImageSize, int AlphaImageType, BASEIMAGE *RgbGraphImage, BASEIMAGE *AlphaGraphImage, int ReverseFlag, int NotUseTransColor ) ;
#ifdef __WINDOWS__
extern	HBITMAP	CreateDIBGraph_WCHAR_T(                const wchar_t *FileName,                                                                                                                                        int ReverseFlag,          COLORDATA *SrcColor ) ;
extern	int		CreateDIBGraph_plus_Alpha_WCHAR_T(     const wchar_t *FileName, HBITMAP *RGBBmp, HBITMAP *AlphaBmp,                                                                                                    int ReverseFlag = FALSE , COLORDATA *SrcColor = NULL ) ;
extern	HBITMAP	CreateDIBGraphVer2_WCHAR_T(            const wchar_t *FileName, const void *MemImage, int MemImageSize,                                             int ImageType,                                     int ReverseFlag,          COLORDATA *SrcColor ) ;
extern	int		CreateDIBGraphVer2_plus_Alpha_WCHAR_T( const wchar_t *FileName, const void *MemImage, int MemImageSize, const void *AlphaImage, int AlphaImageSize, int ImageType, HBITMAP *RGBBmp, HBITMAP *AlphaBmp, int ReverseFlag,          COLORDATA *SrcColor ) ;
#endif // __WINDOWS__
extern	int		CreateBaseImage_WCHAR_T(               const wchar_t *FileName, const void *FileImage, int FileImageSize, int DataType /*=LOADIMAGE_TYPE_FILE*/ , BASEIMAGE *BaseImage,  int ReverseFlag ) ;
extern	int		CreateBaseImageToFile_WCHAR_T(         const wchar_t *FileName,                                                                                   BASEIMAGE *BaseImage,  int ReverseFlag = FALSE ) ;

#ifndef DX_NON_JPEGREAD
extern	int		ReadJpegExif_WCHAR_T(                  const wchar_t *FilePath, const void *FileImage, size_t FileImageSize, BYTE *ExifBuffer, size_t ExifBufferSize ) ;
#endif // DX_NON_JPEGREAD

#ifndef DX_NON_SAVEFUNCTION

extern	int		SaveBaseImageToBmp_WCHAR_T(            const wchar_t *FilePath, const BASEIMAGE *BaseImage ) ;
extern	int		SaveBaseImageToDds_WCHAR_T(            const wchar_t *FilePath, const BASEIMAGE *BaseImage, int CubeMapFlag = FALSE , int MipMapCount = 1 ) ;
#ifndef DX_NON_PNGREAD
extern	int		SaveBaseImageToPng_WCHAR_T(            const wchar_t *FilePath,       BASEIMAGE *BaseImage, int CompressionLevel ) ;
#endif // DX_NON_PNGREAD
#ifndef DX_NON_JPEGREAD
extern	int		SaveBaseImageToJpeg_WCHAR_T(           const wchar_t *FilePath,       BASEIMAGE *BaseImage, int Quality, int Sample2x1 ) ;
#endif // DX_NON_JPEGREAD

#endif // DX_NON_SAVEFUNCTION



// 環境依存初期化・終了関数
extern	int		InitializeBaseImageManage_PF( void ) ;													// 基本イメージ管理情報の環境依存処理の初期化
extern	int		TerminateBaseImageManage_PF( void ) ;													// 基本イメージ管理情報の環境依存処理の後始末


// インライン関数・マクロ---------------------------------------------------------

// 半精度浮動小数点を単精度浮動小数点に変換する
__inline float Float16ToFloat32( WORD Float16 )
{
	unsigned int s ;
	int e ;
	unsigned int f ;
	unsigned int dest ;

	if( Float16 == 0 )
	{
		return	0.0f ;
	}

	s =     Float16 & 0x8000 ;
	e = ( ( Float16 & 0x7c00 ) >> 10 ) - 15 + 127 ;
	f =     Float16 & 0x03ff ;
	dest = ( s << 16 ) | ( ( e << 23 ) & 0x7f800000 ) | ( f << 13 ) ;
	return	*( ( float * )&dest ) ;
}

// 単精度浮動小数点を半精度浮動小数点に変換する
__inline WORD Float32ToFloat16( float Float32 )
{
	unsigned int src = *( ( DWORD * )&Float32 ) ;
	int e ;
	unsigned int s ;
	unsigned int f ;

	if( src == 0 )
	{
		return 0 ;
	}

	e = ( ( src & 0x7f800000 ) >> 23 ) - 127 + 15 ;
	if( e < 0 )
	{
		return 0 ;
	}
	else
	if( e > 31 )
	{
		e = 31 ;
	}

	s = src & 0x80000000 ;
	f = src & 0x007fffff ;
	return ( ( s >> 16 ) & 0x8000 ) | ( ( e << 10 ) & 0x7c00 ) | ( ( f >> 13 ) & 0x03ff ) ;
}

// DrawPixel 等の描画関数で使用するカラー値を取得する
#ifndef DX_NON_GRAPHICS
	#define GetColor_Macro( /*int*/ Red, /*int*/ Green, /*int*/ Blue, /*int*/ Dest )\
	{\
		const COLORDATA *ColorData ;\
	\
		/* 色情報を返す */\
		if( GSYS.Setting.ValidHardware == TRUE )\
		{\
			Dest = 0xff000000 | ( ( ( unsigned int )( Red ) ) << 16 ) | ( ( ( unsigned int )( Green ) ) << 8 ) | ( ( unsigned int )( Blue ) ) ;\
		}\
		else\
		{\
			ColorData = GetMemImgColorData( GSYS.Screen.MainScreenColorBitDepth == 16 ? 0 : 1, FALSE, FALSE ) ;\
			Dest = ColorData->NoneMask +\
						( ( ( BYTE )( Red   ) >> ( 8 - ColorData->RedWidth   ) ) << ColorData->RedLoc   ) +\
						( ( ( BYTE )( Green ) >> ( 8 - ColorData->GreenWidth ) ) << ColorData->GreenLoc ) +\
						( ( ( BYTE )( Blue  ) >> ( 8 - ColorData->BlueWidth  ) ) << ColorData->BlueLoc  ) ;\
		}\
	}
#else // DX_NON_GRAPHICS
	#define GetColor_Macro( /*int*/ Red, /*int*/ Green, /*int*/ Blue, /*int*/ Dest )	0
#endif // DX_NON_GRAPHICS

// カラー値から赤、緑、青、アルファの値を取得する
#ifndef DX_NON_GRAPHICS
	#define GetColor2_Macro( /* unsigned int */ Color, /* int */Red, /* int */Green, /* int */Blue )\
	{\
		/* 色情報を返す */\
		if( GSYS.Setting.ValidHardware == TRUE )\
		{\
			Red   = ( int )( ( Color >> 16 ) & 0xff ) ;\
			Green = ( int )( ( Color >>  8 ) & 0xff ) ;\
			Blue  = ( int )( ( Color       ) & 0xff ) ;\
		}\
		else\
		{\
			const COLORDATA *ColorData ;\
			int MaxRed ;\
			int MaxGreen ;\
			int MaxBlue ;\
\
			ColorData = GetMemImgColorData( GSYS.Screen.MainScreenColorBitDepth == 16 ? 0 : 1, FALSE, FALSE ) ;\
\
			MaxRed		= ( 1 << ColorData->RedWidth   ) - 1 ;\
			MaxGreen	= ( 1 << ColorData->GreenWidth ) - 1 ;\
			MaxBlue		= ( 1 << ColorData->BlueWidth  ) - 1 ;\
\
			/* 色情報を格納する */\
			Red		= ( int )( ( ( Color & ColorData->RedMask   ) >> ColorData->RedLoc   ) * 255 / MaxRed   ) ;\
			Green	= ( int )( ( ( Color & ColorData->GreenMask ) >> ColorData->GreenLoc ) * 255 / MaxGreen ) ;\
			Blue	= ( int )( ( ( Color & ColorData->BlueMask  ) >> ColorData->BlueLoc  ) * 255 / MaxBlue  ) ;\
		}\
	}
#else // DX_NON_GRAPHICS
	#define GetColor2_Macro( /* unsigned int */ Color, /* int */ Red, /* int */ Green, /* int */ Blue )
#endif // DX_NON_GRAPHICS


#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // __DXBASEIMAGE_H__
