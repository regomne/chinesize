// ----------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		ＢａｓｅＩｍａｇｅプログラム
// 
// 				Ver 3.20c
// 
// ----------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

// インクルード----------------------------------------------------------------
#include "DxBaseImage.h"
#include "DxMemory.h"
#include "DxBaseFunc.h"
#include "DxGraphics.h"
#include "DxFile.h"
#include "DxLog.h"
#include "DxChar.h"
#include "DxUseCLib.h"
#include "DxASyncLoad.h"
#include "DxSystem.h"
#include "DxMath.h"

#ifdef __WINDOWS__
#include "Windows/DxBaseImageWin.h"
#include "Windows/DxWinAPI.h"
#endif // __WINDOWS__

#ifdef __ANDROID__
#include "Android/DxBaseImageAndroid.h"
#endif // __ANDROID__

#ifdef __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE
		#include "iOS/DxBaseImageiOS.h"
    #endif // TARGET_OS_IPHONE
#endif // __APPLE__





#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義------------------------------------------------------------------

#define BI_DDPF_ALPHAPIXELS						(0x00000001l)
#define BI_DDPF_ALPHA							(0x00000002l)
#define BI_DDPF_FOURCC							(0x00000004l)
#define BI_DDPF_RGB								(0x00000040l)
#define BI_DDPF_ZBUFFER							(0x00000400l)
#define BI_DDPF_LUMINANCE						(0x00020000l)
#define BI_DDPF_BUMPLUMINANCE					(0x00040000l)
#define BI_DDPF_BUMPDUDV						(0x00080000l)
#define BI_DDPF_RGBTOYUV						(0x000001001)
#define BI_DDPF_YUV								(0x000002001)

#define BI_DDSD_CAPS							(0x00000001l)
#define BI_DDSD_HEIGHT							(0x00000002l)
#define BI_DDSD_WIDTH							(0x00000004l)
#define BI_DDSD_PITCH							(0x00000008l)
#define BI_DDSD_BACKBUFFERCOUNT					(0x00000020l)
#define BI_DDSD_ZBUFFERBITDEPTH					(0x00000040l)
#define BI_DDSD_ALPHABITDEPTH					(0x00000080l)
#define BI_DDSD_LPSURFACE						(0x00000800l)
#define BI_DDSD_PIXELFORMAT						(0x00001000l)
#define BI_DDSD_CKDESTOVERLAY					(0x00002000l)
#define BI_DDSD_CKDESTBLT						(0x00004000l)
#define BI_DDSD_CKSRCOVERLAY					(0x00008000l)
#define BI_DDSD_CKSRCBLT						(0x00010000l)
#define BI_DDSD_MIPMAPCOUNT						(0x00020000l)
#define BI_DDSD_REFRESHRATE						(0x00040000l)
#define BI_DDSD_LINEARSIZE						(0x00080000l)
#define BI_DDSD_TEXTURESTAGE					(0x00100000l)
#define BI_DDSD_FVF								(0x00200000l)
#define BI_DDSD_SRCVBHANDLE						(0x00400000l)
#define BI_DDSD_DEPTH							(0x00800000l)

#define BI_DDSCAPS_RESERVED1					(0x00000001l)
#define BI_DDSCAPS_ALPHA						(0x00000002l)
#define BI_DDSCAPS_BACKBUFFER					(0x00000004l)
#define BI_DDSCAPS_COMPLEX						(0x00000008l)
#define BI_DDSCAPS_FLIP							(0x00000010l)
#define BI_DDSCAPS_OFFSCREENPLAIN				(0x00000040l)
#define BI_DDSCAPS_OVERLAY						(0x00000080l)
#define BI_DDSCAPS_PRIMARYSURFACE				(0x00000200l)
#define BI_DDSCAPS_SYSTEMMEMORY					(0x00000800l)
#define BI_DDSCAPS_TEXTURE						(0x00001000l)
#define BI_DDSCAPS_3DDEVICE						(0x00002000l)
#define BI_DDSCAPS_VIDEOMEMORY					(0x00004000l)
#define BI_DDSCAPS_ZBUFFER						(0x00020000l)
#define BI_DDSCAPS_MIPMAP						(0x00400000l)
#define BI_DDSCAPS_LOCALVIDMEM					(0x10000000l)

#define BI_DDSCAPS2_RESERVED4					(0x00000002L)
#define BI_DDSCAPS2_HARDWAREDEINTERLACE			(0x00000000L)
#define BI_DDSCAPS2_HINTDYNAMIC					(0x00000004L)
#define BI_DDSCAPS2_HINTSTATIC					(0x00000008L)
#define BI_DDSCAPS2_TEXTUREMANAGE				(0x00000010L)
#define BI_DDSCAPS2_RESERVED1					(0x00000020L)
#define BI_DDSCAPS2_RESERVED2					(0x00000040L)
#define BI_DDSCAPS2_OPAQUE						(0x00000080L)
#define BI_DDSCAPS2_HINTANTIALIASING			(0x00000100L)
#define BI_DDSCAPS2_CUBEMAP						(0x00000200L)
#define BI_DDSCAPS2_CUBEMAP_POSITIVEX			(0x00000400L)
#define BI_DDSCAPS2_CUBEMAP_NEGATIVEX			(0x00000800L)
#define BI_DDSCAPS2_CUBEMAP_POSITIVEY			(0x00001000L)
#define BI_DDSCAPS2_CUBEMAP_NEGATIVEY			(0x00002000L)
#define BI_DDSCAPS2_CUBEMAP_POSITIVEZ			(0x00004000L)
#define BI_DDSCAPS2_CUBEMAP_NEGATIVEZ			(0x00008000L)
#define BI_DDSCAPS2_CUBEMAP_ALLFACES			(	BI_DDSCAPS2_CUBEMAP_POSITIVEX |\
													BI_DDSCAPS2_CUBEMAP_NEGATIVEX |\
													BI_DDSCAPS2_CUBEMAP_POSITIVEY |\
													BI_DDSCAPS2_CUBEMAP_NEGATIVEY |\
													BI_DDSCAPS2_CUBEMAP_POSITIVEZ |\
													BI_DDSCAPS2_CUBEMAP_NEGATIVEZ )
#define BI_DDSCAPS2_MIPMAPSUBLEVEL				(0x00010000L)
#define BI_DDSCAPS2_D3DTEXTUREMANAGE			(0x00020000L)
#define BI_DDSCAPS2_DONOTPERSIST				(0x00040000L)
#define BI_DDSCAPS2_STEREOSURFACELEFT			(0x00080000L)
#define BI_DDSCAPS2_VOLUME						(0x00200000L)
#define BI_DDSCAPS2_NOTUSERLOCKABLE				(0x00400000L)
#define BI_DDSCAPS2_POINTS						(0x00800000L)
#define BI_DDSCAPS2_RTPATCHES					(0x01000000L)
#define BI_DDSCAPS2_NPATCHES					(0x02000000L)
#define BI_DDSCAPS2_RESERVED3					(0x04000000L)
#define BI_DDSCAPS2_DISCARDBACKBUFFER			(0x10000000L)
#define BI_DDSCAPS2_ENABLEALPHACHANNEL			(0x20000000L)
#define BI_DDSCAPS2_EXTENDEDFORMATPRIMARY		(0x40000000L)
#define BI_DDSCAPS2_ADDITIONALPRIMARY			(0x80000000L)

// 構造体宣言------------------------------------------------------------------

// DDSファイルヘッダ
struct DDSFILEHEADER
{
	DWORD	dwMagic ;					// "DDS"
	DWORD	dwSize ;					// ここから数えるヘッダのサイズ( 124 )
	DWORD	dwFlags ;					// D_DDSURFACEDESC2.dwFlags と同じもの
	DWORD	dwHeight ;					// 画像の高さ
	DWORD	dwWidth ;					// 画像の幅
	DWORD	dwPitchOrLinearSize ;		// 横1 line の byte 数 (pitch)
										// または 1面分の byte 数 (linearsize)
	DWORD	dwDepth ;					// 画像の奥行き z size (Volume Texture 用)
	DWORD	dwMipMapCount ;				// 含まれている mipmap レベル数
	DWORD	dwReserved1[ 11 ] ;			// 予約１
	DWORD	dwPfSize ;					// ピクセルフォーマットサイズ( 32 )
	DWORD	dwPfFlags ;					// D_DDPIXELFORMAT.dwFlags と同じもの
	DWORD	dwFourCC ;					// FourCC
	DWORD	dwRGBBitCount ;				// カラービット深度
	DWORD	dwRBitMask ;				// 赤マスク
	DWORD	dwGBitMask ;				// 緑マスク
	DWORD	dwBBitMask ;				// 青マスク
	DWORD	dwRGBAlphaBitMask ;			// αマスク
	DWORD	dwCaps ;					// BI_DDSCAPS2.dwCaps と同じもの
	DWORD	dwCaps2 ;					// BI_DDSCAPS2.dwCaps2 と同じもの
	DWORD	dwReservedCaps[ 2 ] ;		// 予約２
	DWORD	dwReserved2 ;				// 予約３
} ;

// カラーフォーマットテーブル構造体
struct DDSCOLORTABLE
{
	DWORD Type ;
	BYTE IsFloat ;
	BYTE ChannelNum ;
	BYTE BitDepth ;
	DWORD RedMask, GreenMask, BlueMask, AlphaMask ;
} ;

// 関数宣言--------------------------------------------------------------------

// 画像読み込み関数
static	int LoadBmpImage( STREAMDATA *Stream, BASEIMAGE *BaseImage, int GetFormatOnly ) ;					// ＢＭＰ画像の読みこみ
static	int LoadArgbImage( STREAMDATA *Src, BASEIMAGE *BaseImage, int GetFormatOnly ) ;						// ＡＲＧＢ画像の読みこみ

#ifndef DX_NON_TGA
static	int LoadTargaImage( STREAMDATA *Src, BASEIMAGE *BaseImage, int GetFormatOnly ) ;					// ＴＧＡ画像の読みこみ
#endif

static	int LoadDDSImage( STREAMDATA *Src, BASEIMAGE *BaseImage, int GetFormatOnly ) ;						// ＤＤＳ画像の読みこみ

// アルファマスク用のファイルパスを作成する
static int CreateAlphaMaskFilePath(  const wchar_t *Path, wchar_t *Dest, size_t BufferBytes ) ;
static int CreateAlphaMaskFilePathT( const TCHAR   *Path, TCHAR   *Dest, size_t BufferBytes ) ;

// データ宣言------------------------------------------------------------------

BYTE YUVLimitTable[ 512 * 2 + 256 ] ;
int YUVTable[ 5 ][ 256 ] ;		// 0:rv  1:gu   2:gv   3:bu   4:y

// 画像読み込み関数配列
int ( *DefaultImageLoadFunc[] )( STREAMDATA *Src, BASEIMAGE *BaseImage, int GetFormatOnly ) =
{
	LoadBmpImage ,
#ifndef DX_NON_JPEGREAD
	LoadJpegImage ,
#endif
#ifndef DX_NON_PNGREAD
	LoadPngImage ,
#endif
#ifndef DX_NON_TGA
	LoadTargaImage ,
#endif
	LoadDDSImage ,
#ifndef DX_NON_TIFFREAD
	LoadTiffImage ,
#endif
	LoadArgbImage ,
	NULL 
} ;

BASEIMAGEMANAGE BaseImageManage ;

// プログラムコード------------------------------------------------------------

// CREATEBASEIMAGETYPE2_GPARAM のデータをセットする
extern void InitCreateBaseImageType2GParam( CREATEBASEIMAGETYPE2_GPARAM *GParam, int GetFormatOnly )
{
	int i ;

	for( i = 0 ; i < BASEIM.UserImageLoadFuncNum4 ; i ++ )
	{
		GParam->UserImageLoadFunc4[ i ] = BASEIM.UserImageLoadFunc4[ i ] ;
	}

	GParam->UserImageLoadFuncNum4 = BASEIM.UserImageLoadFuncNum4 ;
	GParam->GetFormatOnly         = GetFormatOnly ;
}

// LOADBASEIMAGE_GPARAM のデータをセットする
extern void InitLoadBaseImageGParam( LOADBASEIMAGE_GPARAM *GParam, int GetFormatOnly )
{
	InitCreateBaseImageType2GParam( &GParam->CreateGraphImageType2GParam, GetFormatOnly ) ;

	GParam->StreamDataShred2			= *StreamGetStruct() ;
	GParam->FileStreamDataShred			= *GetFileStreamDataShredStruct() ;
	GParam->MemStreamDataShred			= *GetMemStreamDataShredStruct() ;
	GParam->ConvertPremultipliedAlpha	= BASEIM.ConvertPremultipliedAlpha ;
	GParam->AlphaTestImageCreateFlag	= BASEIM.AlphaTestImageCreateFlag ;
	GParam->TransColor					= BASEIM.TransColor ;
}

#ifndef DX_NON_JPEGREAD

// JPEGファイルの Exif情報を取得する、ExifBuffer を NULL に渡すと、戻り値の情報のサイズのみ取得できます( 戻り値  -1:エラー  -1以外：Exif情報のサイズ )
extern int NS_ReadJpegExif( const TCHAR *JpegFilePath, BYTE *ExifBuffer, size_t ExifBufferSize )
{
#ifdef UNICODE
	return ReadJpegExif_WCHAR_T(
		JpegFilePath, NULL, 0, ExifBuffer, ExifBufferSize
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( JpegFilePath, return -1 )

	Result = ReadJpegExif_WCHAR_T(
		UseJpegFilePathBuffer, NULL, 0, ExifBuffer, ExifBufferSize
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( JpegFilePath )

	return Result ;
#endif
}

// JPEGファイルの Exif情報を取得する、ExifBuffer を NULL に渡すと、戻り値の情報のサイズのみ取得できます( 戻り値  -1:エラー  -1以外：Exif情報のサイズ )
extern int NS_ReadJpegExifWithStrLen( const TCHAR *JpegFilePath, size_t JpegFilePathLength, BYTE *ExifBuffer, size_t ExifBufferSize )
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( JpegFilePath, JpegFilePathLength, return -1 )
	Result = ReadJpegExif_WCHAR_T( UseJpegFilePathBuffer, NULL, 0, ExifBuffer, ExifBufferSize ) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( JpegFilePath )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( JpegFilePath, JpegFilePathLength, return -1 )
	Result = ReadJpegExif_WCHAR_T( UseJpegFilePathBuffer, NULL, 0, ExifBuffer, ExifBufferSize ) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( JpegFilePath )
#endif
	return Result ;
}

extern int ReadJpegExif_WCHAR_T( const wchar_t *FilePath, const void *FileImage, size_t FileImageSize, BYTE *ExifBuffer, size_t ExifBufferSize )
{
	LOADBASEIMAGE_GPARAM GParam ;
	DWORD_PTR fp = 0 ;
	STREAMDATA Src ;
	int Result ;

	InitLoadBaseImageGParam( &GParam, FALSE ) ;

	// ファイルを開く
	if( FilePath != NULL )
	{
		fp = GParam.StreamDataShred2.Open( FilePath, FALSE, TRUE, FALSE ) ;
		if( fp == 0 )
		{
			return -1 ;
		}

		Src.DataPoint = fp ;
		Src.ReadShred = GParam.FileStreamDataShred ;
	}
	else
	if( FileImage != NULL )
	{
		Src.DataPoint = MemStreamOpen( FileImage, FileImageSize ) ;
		if( Src.DataPoint == NULL )
		{
			return -1 ;
		}
		Src.ReadShred = GParam.MemStreamDataShred ;
	}
	else
	{
		return -1 ;
	}

	Result = LoadJpegImageBase( &Src, NULL, FALSE, TRUE, ExifBuffer, ExifBufferSize ) ;

	if( FilePath != NULL )
	{
		STCLOSE( &Src ) ;
	}

	return Result ;
}

#endif // DX_NON_JPEGREAD

// 基本イメージデータのロード＋ＤＩＢ関係

#ifndef DX_NON_SAVEFUNCTION

// 基本画像データをＢＭＰ画像として保存する
extern int NS_SaveBaseImageToBmp( const TCHAR *FilePath, const BASEIMAGE *BaseImage )
{
#ifdef UNICODE
	return SaveBaseImageToBmp_WCHAR_T(
		FilePath, BaseImage
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( FilePath, return -1 )

	Result = SaveBaseImageToBmp_WCHAR_T(
		UseFilePathBuffer, BaseImage
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( FilePath )

	return Result ;
#endif
}

// 基本イメージデータをＢＭＰ画像として保存する
extern int NS_SaveBaseImageToBmpWithStrLen( const TCHAR *FilePath, size_t FilePathLength, const BASEIMAGE *BaseImage )
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FilePath, FilePathLength, return -1 )
	Result = SaveBaseImageToBmp_WCHAR_T( UseFilePathBuffer, BaseImage ) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FilePath )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FilePath, FilePathLength, return -1 )
	Result = SaveBaseImageToBmp_WCHAR_T( UseFilePathBuffer, BaseImage ) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FilePath )
#endif
	return Result ;
}

// 基本画像データをＢＭＰ画像として保存する
extern int SaveBaseImageToBmp_WCHAR_T( const wchar_t *FilePath, const BASEIMAGE *BaseImage )
{
	BITMAPFILEHEADER BmpHead ;
	BITMAPINFO *BmpInfo = 0 ;
	BYTE *DIBBuf = 0 ;
	BYTE *Dest, *Src ;
	int Ret = -1 ;
	int x, y, i ;
	int WidthByte ;
	DWORD_PTR fp ;

	// ファイルヘッダに基本データをセット
	_MEMSET( &BmpHead, 0, sizeof( BmpHead ) ) ;
	( ( BYTE * )&BmpHead.bfType )[ 0 ] = 'B' ;
	( ( BYTE * )&BmpHead.bfType )[ 1 ] = 'M' ;
	BmpHead.bfOffBits	= sizeof( BmpHead ) + sizeof( BITMAPINFOHEADER ) ;
	BmpHead.bfSize		= BmpHead.bfOffBits ;

	// 色数ごとに処理を変更する
	switch( BaseImage->ColorData.ColorBitDepth )
	{
	case 8:
		// パレット保存用メモリを含めたサイズのメモリを確保する
		BmpInfo = ( BITMAPINFO * )DXCALLOC( sizeof( BITMAPINFO ) + sizeof( RGBQUAD ) * 256 ) ;
		if( BmpInfo == NULL )
		{
			DXST_LOGFILE_ADDUTF16LE( "\x42\x00\x49\x00\x54\x00\x4d\x00\x41\x00\x50\x00\x49\x00\x4e\x00\x46\x00\x4f\x00\xcb\x69\x20\x90\x53\x4f\x28\x75\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"BITMAPINFO構造体用メモリの確保に失敗しました\n" @*/ ) ; goto END ;
		}

		// １ライン当たりのデータ量を算出
		WidthByte = ( BaseImage->Width + 3 ) / 4 * 4 ; 

		// グラフィック保存メモリの確保
		DIBBuf = ( BYTE * )DXCALLOC( ( size_t )( WidthByte * BaseImage->Height ) ) ;
		if( DIBBuf == NULL )
		{
			DXST_LOGFILE_ADDUTF16LE( "\xb0\x30\xe9\x30\xd5\x30\xa3\x30\xc3\x30\xaf\x30\xdd\x4f\x58\x5b\x28\x75\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"グラフィック保存用メモリの確保に失敗しました\n" @*/ ) ; goto END ;
		}

		// パレットの保存
		{
			RGBQUAD *Bgb = BmpInfo->bmiColors ;
			const COLORPALETTEDATA *Pal = BaseImage->ColorData.Palette ;

			for( i = 0 ; i < 256 ; i ++, Bgb ++, Pal ++ )
			{
				Bgb->rgbBlue     = Pal->Blue ;
				Bgb->rgbGreen    = Pal->Green ;
				Bgb->rgbRed      = Pal->Red ;
				Bgb->rgbReserved = 0 ;
			}
		}

		// イメージデータの先頭位置を変更
		BmpHead.bfOffBits += sizeof( RGBQUAD ) * 256 ;

		// ファイルの総サイズをセット
		BmpHead.bfSize += sizeof( RGBQUAD ) * 256 + WidthByte * BaseImage->Height ;

		// BITMAPINFOにデータを書き込む
		BmpInfo->bmiHeader.biBitCount = 8 ;
		BmpInfo->bmiHeader.biSize = sizeof( BmpInfo->bmiHeader ) ;
		BmpInfo->bmiHeader.biWidth = BaseImage->Width ;
		BmpInfo->bmiHeader.biHeight = BaseImage->Height ;
		BmpInfo->bmiHeader.biPlanes = 1 ;
		BmpInfo->bmiHeader.biCompression = 0L ;
		
		// イメージをバッファに転送
		for( i = 0, y = BaseImage->Height - 1 ; y > -1 ; y --, i ++ )
		{
			_MEMCPY( DIBBuf + WidthByte * i, ( BYTE * )BaseImage->GraphData + BaseImage->Pitch * y, ( size_t )BaseImage->Width ) ;
		}

		// ファイルに書き出し
		{
			WriteOnlyFileAccessDelete( FilePath ) ;
			fp = WriteOnlyFileAccessOpen( FilePath ) ;
			if( fp == 0 ) 
			{
				DXST_LOGFILE_ADDUTF16LE( "\x22\xff\x2d\xff\x30\xff\xbb\x30\xfc\x30\xd6\x30\x28\x75\xd5\x30\xa1\x30\xa4\x30\xeb\x30\x6e\x30\xaa\x30\xfc\x30\xd7\x30\xf3\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ＢＭＰセーブ用ファイルのオープンに失敗しました\n" @*/ ) ; goto END ;
			}

			// ファイルヘッダの書き出し
			WriteOnlyFileAccessWrite( fp, &BmpHead, sizeof( BmpHead ) ) ;
			
			// インフォヘッダの書き出し
			WriteOnlyFileAccessWrite( fp, BmpInfo, sizeof( BmpInfo->bmiHeader ) + sizeof( RGBQUAD ) * 256 ) ;

			// イメージの書き出し
			WriteOnlyFileAccessWrite( fp, DIBBuf, ( DWORD )( WidthByte * BaseImage->Height ) ) ;

			// 終了
			WriteOnlyFileAccessClose( fp ) ;
		}
		break ;

	case 32 :
	case 24 :
	case 16 :
		{
			// パレット保存用メモリを含めたサイズのメモリを確保する
			BmpInfo = ( BITMAPINFO * )DXCALLOC( sizeof( BITMAPINFO ) ) ;
			if( BmpInfo == NULL )
			{
				DXST_LOGFILE_ADDUTF16LE( "\x42\x00\x49\x00\x54\x00\x4d\x00\x41\x00\x50\x00\x49\x00\x4e\x00\x46\x00\x4f\x00\xcb\x69\x20\x90\x53\x4f\x28\x75\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"BITMAPINFO構造体用メモリの確保に失敗しました\n" @*/ ) ; goto END ;
			}

			// １ライン当たりのデータ量を算出
			WidthByte = ( BaseImage->Width * 3 + 3 ) / 4 * 4 ; 

			// グラフィック保存メモリの確保
			DIBBuf = ( BYTE * )DXCALLOC( ( size_t )( WidthByte * BaseImage->Height ) ) ;
			if( DIBBuf == NULL ) 
			{
				DXST_LOGFILE_ADDUTF16LE( "\xb0\x30\xe9\x30\xd5\x30\xa3\x30\xc3\x30\xaf\x30\xdd\x4f\x58\x5b\x28\x75\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"グラフィック保存用メモリの確保に失敗しました\n" @*/ ) ; goto END ;
			}

			// ファイルの総サイズをセット
			BmpHead.bfSize += WidthByte * BaseImage->Height ;

			// BITMAPINFOにデータを書き込む
			BmpInfo->bmiHeader.biBitCount    = 24 ;
			BmpInfo->bmiHeader.biSize        = sizeof( BmpInfo->bmiHeader ) ;
			BmpInfo->bmiHeader.biWidth       = BaseImage->Width ;
			BmpInfo->bmiHeader.biHeight      = BaseImage->Height ;
			BmpInfo->bmiHeader.biPlanes      = 1 ;
			BmpInfo->bmiHeader.biCompression = 0L ;
			
			// イメージをバッファに転送
			if( BaseImage->ColorData.BlueMask  == 0x00ff0000 &&
				BaseImage->ColorData.GreenMask == 0x0000ff00 &&
				BaseImage->ColorData.RedMask   == 0x000000ff &&
				BaseImage->ColorData.PixelByte == 4 )
			{
				for( i = 0, y = BaseImage->Height - 1 ; y > -1 ; y --, i ++ )
				{
					Dest = DIBBuf + WidthByte * i ;
					Src = ( BYTE * )BaseImage->GraphData + BaseImage->Pitch * y ;

					for( x = 0 ; x < BaseImage->Width ; x ++ )
					{
						Dest[ 0 ] = Src[ 2 ] ;
						Dest[ 1 ] = Src[ 1 ] ;
						Dest[ 2 ] = Src[ 0 ] ;
						Dest += 3 ;
						Src  += 4 ;
					}
				}
			}
			else
			if( BaseImage->ColorData.BlueMask  == 0x000000ff &&
				BaseImage->ColorData.GreenMask == 0x0000ff00 &&
				BaseImage->ColorData.RedMask   == 0x00ff0000 &&
				BaseImage->ColorData.PixelByte == 4 )
			{
				for( i = 0, y = BaseImage->Height - 1 ; y > -1 ; y --, i ++ )
				{
					Dest = DIBBuf + WidthByte * i ;
					Src = ( BYTE * )BaseImage->GraphData + BaseImage->Pitch * y ;

					for( x = 0 ; x < BaseImage->Width ; x ++ )
					{
						*Dest = *Src ; Src ++ ;   Dest ++ ; 
						*Dest = *Src ; Src ++ ;   Dest ++ ; 
						*Dest = *Src ; Src += 2 ; Dest ++ ;
					}
				}
			}
			else
			if( BaseImage->ColorData.BlueMask  == 0x000000ff &&
				BaseImage->ColorData.GreenMask == 0x0000ff00 &&
				BaseImage->ColorData.RedMask   == 0x00ff0000 &&
				BaseImage->ColorData.PixelByte == 3 )
			{
				for( i = 0, y = BaseImage->Height - 1 ; y > -1 ; y --, i ++ )
				{
					Dest = DIBBuf + WidthByte * i ;
					Src = ( BYTE * )BaseImage->GraphData + BaseImage->Pitch * y ;

					for( x = 0 ; x < BaseImage->Width ; x ++ )
					{
						*Dest = *Src ; Src ++ ; Dest ++ ; 
						*Dest = *Src ; Src ++ ; Dest ++ ; 
						*Dest = *Src ; Src ++ ; Dest ++ ;
					}
				}
			}
			else
			if( BaseImage->ColorData.PixelByte == 2 )
			{
				DWORD RedMask,  GreenMask, BlueMask ;
				DWORD RedLoc,   GreenLoc,  BlueLoc ;
				DWORD RedLoc2,  GreenLoc2, BlueLoc2 ;
				DWORD DestData, SrcData ;

				RedMask   = BaseImage->ColorData.RedMask ;
				GreenMask = BaseImage->ColorData.GreenMask ;
				BlueMask  = BaseImage->ColorData.BlueMask ;

				RedLoc    = BaseImage->ColorData.RedLoc ; 
				GreenLoc  = BaseImage->ColorData.GreenLoc ; 
				BlueLoc   = BaseImage->ColorData.BlueLoc ; 

				RedLoc2   = ( DWORD )( 8  - BaseImage->ColorData.RedWidth   ) ; 
				GreenLoc2 = ( DWORD )( 16 - BaseImage->ColorData.GreenWidth ) ; 
				BlueLoc2  = ( DWORD )( 24 - BaseImage->ColorData.BlueWidth  ) ;

				for( i = 0, y = BaseImage->Height - 1 ; y > -1 ; y --, i ++ )
				{
					Dest = DIBBuf + WidthByte * i ;
					Src = ( BYTE * )BaseImage->GraphData + BaseImage->Pitch * y ;

					for( x = 0 ; x < BaseImage->Width ; x ++ )
					{
						SrcData = ( DWORD )*( ( WORD * )Src ) ;
						DestData =
									( ( ( SrcData & RedMask   ) >> RedLoc   ) << RedLoc2   ) |
									( ( ( SrcData & GreenMask ) >> GreenLoc ) << GreenLoc2 ) |
									( ( ( SrcData & BlueMask  ) >> BlueLoc  ) << BlueLoc2  ) ;

						*Dest = ( BYTE )( DestData >> 16 ) ; Dest ++ ;
						*Dest = ( BYTE )( DestData >> 8  ) ; Dest ++ ;
						*Dest = ( BYTE )( DestData       ) ; Dest ++ ;

						Src += 2 ;
					}
				}
			}
			else
			{
				int r, g, b, a ;

				for( i = 0, y = BaseImage->Height - 1 ; y > -1 ; y --, i ++ )
				{
					Dest = DIBBuf + WidthByte * i ;

					for( x = 0 ; x < BaseImage->Width ; x ++ )
					{
						NS_GetPixelBaseImage( BaseImage, x, y, &r, &g, &b, &a ) ;

						Dest[ 0 ] = ( BYTE )b ;
						Dest[ 1 ] = ( BYTE )g ;
						Dest[ 2 ] = ( BYTE )r ;
						Dest += 3 ;
					}
				}
			}
		}

		// ファイルに書き出し
		{
			WriteOnlyFileAccessDelete( FilePath ) ;
			fp = WriteOnlyFileAccessOpen( FilePath ) ;
			if( fp == 0 ) 
			{
				DXST_LOGFILE_ADDUTF16LE( "\x22\xff\x2d\xff\x30\xff\xbb\x30\xfc\x30\xd6\x30\x28\x75\xd5\x30\xa1\x30\xa4\x30\xeb\x30\x6e\x30\xaa\x30\xfc\x30\xd7\x30\xf3\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ＢＭＰセーブ用ファイルのオープンに失敗しました\n" @*/ ) ; goto END ;
			}

			// ファイルヘッダの書き出し
			WriteOnlyFileAccessWrite( fp, &BmpHead, sizeof( BmpHead ) ) ;
			
			// インフォヘッダの書き出し
			WriteOnlyFileAccessWrite( fp, BmpInfo, sizeof( BmpInfo->bmiHeader ) ) ;

			// イメージの書き出し
			WriteOnlyFileAccessWrite( fp, DIBBuf, ( DWORD )( WidthByte * BaseImage->Height ) ) ;

			// 終了
			WriteOnlyFileAccessClose( fp ) ;
		}
		break ;
	}

	Ret = 0 ;
END:

	// メモリの解放
	if( BmpInfo ) DXFREE( BmpInfo ) ;
	if( DIBBuf  ) DXFREE( DIBBuf ) ;

	// 正常終了
	return Ret ;
}

// 基本画像データをＤＤＳ画像として保存する
extern int NS_SaveBaseImageToDds( const TCHAR *FilePath, const BASEIMAGE *BaseImage )
{
#ifdef UNICODE
	return SaveBaseImageToDds_WCHAR_T(
		FilePath, BaseImage
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( FilePath, return -1 )

	Result = SaveBaseImageToDds_WCHAR_T(
		UseFilePathBuffer, BaseImage
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( FilePath )

	return Result ;
#endif
}

// 基本イメージデータをＤＤＳ画像として保存する
extern int NS_SaveBaseImageToDdsWithStrLen( const TCHAR *FilePath, size_t FilePathLength, const BASEIMAGE *BaseImage )
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FilePath, FilePathLength, return -1 )
	Result = SaveBaseImageToDds_WCHAR_T( UseFilePathBuffer, BaseImage ) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FilePath )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FilePath, FilePathLength, return -1 )
	Result = SaveBaseImageToDds_WCHAR_T( UseFilePathBuffer, BaseImage ) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FilePath )
#endif
	return Result ;
}

// 基本画像データをＤＤＳ画像として保存する
extern int SaveBaseImageToDds_WCHAR_T( const wchar_t *FilePath, const BASEIMAGE *BaseImage, int CubeMapFlag , int MipMapCount )
{
	int Ret = -1 ;
	DWORD_PTR fp ;
	DDSFILEHEADER DDSHead ;

	_MEMSET( &DDSHead, 0, sizeof( DDSHead ) ) ;

	// マジックナンバー『DDS 』のセット
	( ( BYTE * )&DDSHead.dwMagic )[ 0 ] = 'D' ;
	( ( BYTE * )&DDSHead.dwMagic )[ 1 ] = 'D' ;
	( ( BYTE * )&DDSHead.dwMagic )[ 2 ] = 'S' ;
	( ( BYTE * )&DDSHead.dwMagic )[ 3 ] = ' ' ;

	// ヘッダのサイズをセット
	DDSHead.dwSize = 124 ;

	// フラグをセット
	DDSHead.dwFlags  = BI_DDSD_CAPS | BI_DDSD_HEIGHT | BI_DDSD_WIDTH | BI_DDSD_PITCH | BI_DDSD_PIXELFORMAT ;
	DDSHead.dwCaps = BI_DDSCAPS_TEXTURE ;

	// ミップマップの情報をセット
	if( MipMapCount > 1 )
	{
		DDSHead.dwCaps |= BI_DDSCAPS_MIPMAP | BI_DDSCAPS_COMPLEX ;
		DDSHead.dwFlags |= BI_DDSD_MIPMAPCOUNT ;
		DDSHead.dwMipMapCount = MipMapCount ;
	}

	// キューブマップの情報をセット
	if( CubeMapFlag )
	{
		DDSHead.dwCaps2 |= BI_DDSCAPS2_CUBEMAP | BI_DDSCAPS2_CUBEMAP_ALLFACES ;
	}

	// 幅と高さをセット
	DDSHead.dwWidth  = ( DWORD )BaseImage->Width ;
	DDSHead.dwHeight = ( DWORD )BaseImage->Height ;

	// ピッチをセット
	DDSHead.dwPitchOrLinearSize = ( DWORD )( BaseImage->Width * BaseImage->ColorData.PixelByte ) ;

	// ピクセルフォーマットのサイズをセット
	DDSHead.dwPfSize = 32 ;

	// Floatタイプかどうかで処理を分岐
	if( BaseImage->ColorData.FloatTypeFlag )
	{
		// Floatタイプの場合

		// フラグをセット
		DDSHead.dwPfFlags = BI_DDPF_FOURCC ;

		// タイプにあった FourCC をセットする
		switch( BaseImage->ColorData.ChannelBitDepth )
		{
		case 16 :
			switch( BaseImage->ColorData.ChannelNum )
			{
			case 1 :
				DDSHead.dwFourCC = 111 ;
				break ;

			case 2 :
				DDSHead.dwFourCC = 112 ;
				break ;

			case 4 :
				DDSHead.dwFourCC = 113 ;
				break ;

			default :
				DXST_LOGFILE_ADDUTF16LE( "\x24\xff\x24\xff\x33\xff\xbb\x30\xfc\x30\xd6\x30\xa8\x30\xe9\x30\xfc\x30\x1a\xff\x5e\x97\xfe\x5b\xdc\x5f\x6e\x30\x66\x00\x6c\x00\x6f\x00\x61\x00\x74\x00\x8b\x57\xc1\x30\xe3\x30\xf3\x30\xcd\x30\xeb\x30\x70\x65\x67\x30\x59\x30\x0a\x00\x00"/*@ L"ＤＤＳセーブエラー：非対応のfloat型チャンネル数です\n" @*/ ) ; goto END ;
				break ;
			}
			break ;

		case 32 :
			switch( BaseImage->ColorData.ChannelNum )
			{
			case 1 :
				DDSHead.dwFourCC = 114 ;
				break ;

			case 2 :
				DDSHead.dwFourCC = 115 ;
				break ;

			case 4 :
				DDSHead.dwFourCC = 116 ;
				break ;

			default :
				DXST_LOGFILE_ADDUTF16LE( "\x24\xff\x24\xff\x33\xff\xbb\x30\xfc\x30\xd6\x30\xa8\x30\xe9\x30\xfc\x30\x1a\xff\x5e\x97\xfe\x5b\xdc\x5f\x6e\x30\x66\x00\x6c\x00\x6f\x00\x61\x00\x74\x00\x8b\x57\xc1\x30\xe3\x30\xf3\x30\xcd\x30\xeb\x30\x70\x65\x67\x30\x59\x30\x0a\x00\x00"/*@ L"ＤＤＳセーブエラー：非対応のfloat型チャンネル数です\n" @*/ ) ; goto END ;
				break ;
			}
			break ;

		default :
			DXST_LOGFILE_ADDUTF16LE( "\x24\xff\x24\xff\x33\xff\xbb\x30\xfc\x30\xd6\x30\xa8\x30\xe9\x30\xfc\x30\x1a\xff\x5e\x97\xfe\x5b\xdc\x5f\x6e\x30\x66\x00\x6c\x00\x6f\x00\x61\x00\x74\x00\x8b\x57\xd3\x30\xc3\x30\xc8\x30\x70\x65\x67\x30\x59\x30\x0a\x00\x00"/*@ L"ＤＤＳセーブエラー：非対応のfloat型ビット数です\n" @*/ ) ; goto END ;
			break ;
		}
	}
	else
	// 整数型で1チャンネルのビット数が16の場合は処理を分岐
	if( BaseImage->ColorData.ChannelBitDepth == 16 )
	{
		// フラグをセット
		DDSHead.dwPfFlags = BI_DDPF_FOURCC ;

		// タイプにあった FourCC をセットする
		switch( BaseImage->ColorData.ChannelNum )
		{
		case 1 :
			DDSHead.dwFourCC = 81 ;
			break ;

		case 2 :
			DDSHead.dwFourCC = 34 ;
			break ;

		case 4 :
			DDSHead.dwFourCC = 36 ;
			break ;

		default :
			DXST_LOGFILE_ADDUTF16LE( "\x24\xff\x24\xff\x33\xff\xbb\x30\xfc\x30\xd6\x30\xa8\x30\xe9\x30\xfc\x30\x1a\xff\x5e\x97\xfe\x5b\xdc\x5f\x6e\x30\x74\x65\x70\x65\x8b\x57\x31\x00\xc1\x30\xe3\x30\xf3\x30\xcd\x30\xeb\x30\x31\x00\x36\x00\xd3\x30\xc3\x30\xc8\x30\x6e\x30\x8b\x57\xc1\x30\xe3\x30\xf3\x30\xcd\x30\xeb\x30\x70\x65\x67\x30\x59\x30\x0a\x00\x00"/*@ L"ＤＤＳセーブエラー：非対応の整数型1チャンネル16ビットの型チャンネル数です\n" @*/ ) ; goto END ;
			break ;
		}
	}
	else
	{
		// 通常型( 整数型タイプの場合 )

		// フラグをセット
		DDSHead.dwPfFlags = BI_DDPF_RGB ;

		// アルファビットが存在する場合は BI_DDPF_ALPHAPIXELS と BI_DDSCAPS_ALPHA も追加
		if( BaseImage->ColorData.AlphaWidth != 0 )
		{
			DDSHead.dwCaps    |= BI_DDSCAPS_ALPHA ;
			DDSHead.dwPfFlags |= BI_DDPF_ALPHAPIXELS ;
		}

		// ビット数をセット
		DDSHead.dwRGBBitCount = BaseImage->ColorData.ColorBitDepth ;

		// RGBAのマスクをセット
		DDSHead.dwRBitMask        = BaseImage->ColorData.RedMask ;
		DDSHead.dwGBitMask        = BaseImage->ColorData.GreenMask ;
		DDSHead.dwBBitMask        = BaseImage->ColorData.BlueMask ;
		DDSHead.dwRGBAlphaBitMask = BaseImage->ColorData.AlphaMask ;
	}

	// ファイルを開く
	WriteOnlyFileAccessDelete( FilePath ) ;
	fp = WriteOnlyFileAccessOpen( FilePath ) ;
	if( fp == 0 ) 
	{
		DXST_LOGFILE_ADDUTF16LE( "\x24\xff\x24\xff\x33\xff\xbb\x30\xfc\x30\xd6\x30\x28\x75\xd5\x30\xa1\x30\xa4\x30\xeb\x30\x6e\x30\xaa\x30\xfc\x30\xd7\x30\xf3\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ＤＤＳセーブ用ファイルのオープンに失敗しました\n" @*/ ) ; goto END ;
	}

	// ヘッダの書き出し
	WriteOnlyFileAccessWrite( fp, &DDSHead, 128 ) ;

	// イメージの書き出し、ミップマップありかどうかで処理を分岐
	{
		int i ;
		int j ;
		BYTE *Image ;
		int ImageNum ;
		ImageNum = CubeMapFlag ? CUBEMAP_SURFACE_NUM : 1 ;
		DWORD LineSize ;
		const BASEIMAGE *pBaseImage ;

		pBaseImage = BaseImage ;
		if( MipMapCount > 1 )
		{
			int k ;
			BYTE Padding[ 8 ] = { 0 } ;

			for( i = 0 ; i < ImageNum ; i ++ )
			{
				for( j = 0 ; j < MipMapCount ; j ++, pBaseImage ++ )
				{
					Image = ( BYTE * )pBaseImage->GraphData ;
					LineSize = ( DWORD )( pBaseImage->Width * pBaseImage->ColorData.PixelByte ) ;
					for( k = 0 ; k < pBaseImage->Height ; k ++, Image += pBaseImage->Pitch )
					{
						WriteOnlyFileAccessWrite( fp, Image, LineSize ) ;
					}

					if( LineSize * pBaseImage->Height < 8 )
					{
						WriteOnlyFileAccessWrite( fp, Padding, 8 - LineSize * pBaseImage->Height, NULL ) ;
					}
				}
			}
		}
		else
		{
			for( i = 0 ; i < ImageNum ; i ++, pBaseImage ++ )
			{
				Image = ( BYTE * )pBaseImage->GraphData ;
				LineSize = ( DWORD )( pBaseImage->Width * pBaseImage->ColorData.PixelByte ) ;
				for( j = 0 ; j < pBaseImage->Height ; j ++, Image += pBaseImage->Pitch )
				{
					WriteOnlyFileAccessWrite( fp, Image, LineSize ) ;
				}
			}
		}
	}

	// 終了
	WriteOnlyFileAccessClose( fp ) ;

	Ret = 0 ;
END:

	// 正常終了
	return Ret ;
}

extern int NS_SaveBaseImageToJpeg( const TCHAR *pFilePath, BASEIMAGE *BaseImage, int Quality, int Sample2x1 )
{
#ifndef DX_NON_JPEGREAD
#ifdef UNICODE
	return SaveBaseImageToJpegBase( ( const char * )pFilePath, NULL, BaseImage, Quality, Sample2x1 ) ;
#else
	return SaveBaseImageToJpegBase( NULL, pFilePath, BaseImage, Quality, Sample2x1 ) ;
#endif
#else
	return -1 ;
#endif
}

// 基本イメージデータをＪＰＥＧ画像として保存する
extern int NS_SaveBaseImageToJpegWithStrLen( const TCHAR *FilePath, size_t FilePathLength, BASEIMAGE *BaseImage, int Quality, int Sample2x1 )
{
#ifndef DX_NON_JPEGREAD
	int Result ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_ONE_BEGIN( FilePath, FilePathLength, return -1 )
#ifdef UNICODE
	Result = SaveBaseImageToJpegBase( ( const char * )UseFilePathBuffer, NULL, BaseImage, Quality, Sample2x1 ) ;
#else
	Result = SaveBaseImageToJpegBase( NULL, UseFilePathBuffer, BaseImage, Quality, Sample2x1 ) ;
#endif
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( FilePath )
	return Result ;
#else
	return -1 ;
#endif
}

extern int SaveBaseImageToJpeg_WCHAR_T( const wchar_t *pFilePath, BASEIMAGE *BaseImage, int Quality, int Sample2x1 )
{
#ifndef DX_NON_JPEGREAD
	return SaveBaseImageToJpegBase( ( const char * )pFilePath, NULL, BaseImage, Quality, Sample2x1 ) ;
#else
	return -1 ;
#endif
}

extern int NS_SaveBaseImageToPng( const TCHAR *pFilePath, BASEIMAGE *BaseImage, int CompressionLevel )
{
#ifndef DX_NON_PNGREAD
#ifdef UNICODE
	return SaveBaseImageToPngBase( ( const char * )pFilePath, NULL, BaseImage, CompressionLevel ) ;
#else
	return SaveBaseImageToPngBase( NULL, pFilePath, BaseImage, CompressionLevel ) ;
#endif
#else
	return -1 ;
#endif
}

// 基本イメージデータをＰＮＧ画像として保存する
extern int NS_SaveBaseImageToPngWithStrLen( const TCHAR *FilePath, size_t FilePathLength, BASEIMAGE *BaseImage, int CompressionLevel )
{
#ifndef DX_NON_PNGREAD
	int Result ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_ONE_BEGIN( FilePath, FilePathLength, return -1 )
#ifdef UNICODE
	Result = SaveBaseImageToPngBase( ( const char * )UseFilePathBuffer, NULL, BaseImage, CompressionLevel ) ;
#else
	Result = SaveBaseImageToPngBase( NULL, UseFilePathBuffer, BaseImage, CompressionLevel ) ;
#endif
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( FilePath )
	return Result ;
#else
	return -1 ;
#endif
}

extern int SaveBaseImageToPng_WCHAR_T( const wchar_t *pFilePath, BASEIMAGE *BaseImage, int CompressionLevel )
{
#ifndef DX_NON_PNGREAD
	return SaveBaseImageToPngBase( ( const char * )pFilePath, NULL, BaseImage, CompressionLevel ) ;
#else
	return -1 ;
#endif
}

#endif // DX_NON_SAVEFUNCTION

// 登録されている各種グラフィックローダ関数から、ＢＭＰデータもしくは GraphImageデータを構築する
extern int CreateGraphImageOrDIBGraph_UseGParam(
	LOADBASEIMAGE_GPARAM *GParam,
	const wchar_t *FileName,
	const void *DataImage,
	int DataImageSize,
	int DataImageType,
	int BmpFlag,
	int ReverseFlag,
	int NotUseTransColor,
	BASEIMAGE *BaseImage,
	BITMAPINFO **BmpInfo,
	void **GraphData
)
{
	DWORD_PTR fp = 0 ;
	int hr ;
	BASEIMAGE GraphI ;
	STREAMDATA Src ;

	// バージョン４の読み込み関数に掛ける
	{
		if( DataImageType == LOADIMAGE_TYPE_FILE )
		{
			fp = GParam->StreamDataShred2.Open( FileName, FALSE, TRUE, FALSE ) ;
			if( fp == 0 )
			{
				return -1 ;
			}

			Src.DataPoint = fp ;

			// 何故か直接代入すると Android でクラッシュする…
//			Src.ReadShred = GParam->FileStreamDataShred ;
			_MEMCPY( &Src.ReadShred, &GParam->FileStreamDataShred, sizeof( GParam->FileStreamDataShred ) ) ;
		}
		else
		{
			Src.DataPoint = MemStreamOpen( DataImage, DataImageSize ) ;
			if( Src.DataPoint == NULL )
			{
				return -1 ;
			}
			// 何故か直接代入すると Android でクラッシュする…
//			Src.ReadShred = GParam->MemStreamDataShred ;
			_MEMCPY( &Src.ReadShred, &GParam->MemStreamDataShred, sizeof( GParam->MemStreamDataShred ) ) ;
		}

		hr = CreateGraphImageType2_UseGParam( &GParam->CreateGraphImageType2GParam, &Src, &GraphI ) ;
		if( hr == 0 )
		{
			if( DataImageType != LOADIMAGE_TYPE_FILE )
				STCLOSE( &Src ) ;
			goto END ;
		}
	}
	// ここにきたら読み込めなかったということなのでエラー
	goto ERR ;

END :
	if( GParam->CreateGraphImageType2GParam.GetFormatOnly )
	{
		// フォーマットを取得するだけの場合はデータをコピーして終了
		*BaseImage = GraphI ;
	}
	else
	{
		// 反転フラグが立っていたらグラフィックを反転させる
		if( ReverseFlag == TRUE )
		{
			// 画像フォーマットが DX_BASEIMAGE_FORMAT_NORMAL 以外だった場合は DX_BASEIMAGE_FORMAT_NORMAL に変換する
			NS_ConvertNormalFormatBaseImage( &GraphI ) ;

			// 反転
			NS_ReverseGraphImage( &GraphI ) ;
		}

		// もし乗算済みアルファ画像への変換のフラグが立っていたら変換する
		if( GParam->ConvertPremultipliedAlpha )
		{
			// αチャンネルが無く、且つアルファテストが有効な場合は透過色部分を真っ黒にする
			if( GraphI.ColorData.AlphaWidth == 0 && GParam->AlphaTestImageCreateFlag == TRUE && NotUseTransColor == FALSE )
			{
				DWORD i ;
				DWORD j ;
				DWORD *p ;
				DWORD TransColor ;

				// フォーマットが標準フォーマットではなかったら標準フォーマットに変換する
				if( GraphI.ColorData.Format != DX_BASEIMAGE_FORMAT_NORMAL )
				{
					if( NS_ConvertNormalFormatBaseImage( &GraphI ) < 0 )
						return -1 ;
				}

				// ARGB8 以外の場合も ARGB8 に変換する
				if( GraphI.ColorData.ColorBitDepth != 32 ||
					GraphI.ColorData.PixelByte != 4 ||
					GraphI.ColorData.FloatTypeFlag != FALSE ||
					GraphI.ColorData.NoneMask  != 0x00000000 ||
					GraphI.ColorData.AlphaMask != 0xff000000 ||
					GraphI.ColorData.RedMask   != 0x00ff0000 ||
					GraphI.ColorData.GreenMask != 0x0000ff00 ||
					GraphI.ColorData.BlueMask  != 0x000000ff )
				{
					BASEIMAGE TempBaseImage ;
					TempBaseImage = GraphI ;
					NS_CreateARGB8ColorBaseImage( GraphI.Width, GraphI.Height, &GraphI ) ;
					NS_BltBaseImage( 0, 0, &TempBaseImage, &GraphI ) ;
					NS_ReleaseBaseImage( &TempBaseImage ) ;
				}

				TransColor = GParam->TransColor | 0xff000000 ;
				for( i = 0 ; i < ( DWORD )GraphI.Height ; i ++ )
				{
					p = ( DWORD * )( ( BYTE * )GraphI.GraphData + i * GraphI.Pitch ) ;
					for( j = 0 ; j < ( DWORD )GraphI.Width ; j ++, p ++ )
					{
						if( *p == TransColor )
						{
							*p = 0 ;
						}
					}
				}
			}
			else
			{
				// αチャンネルが普通に在る場合は普通にコンバート
				NS_ConvertPremulAlphaBaseImage( &GraphI ) ; 
			}
		}

		// ロードしたデータのタイプが指定と違っていたら変換を掛ける
		if( BmpFlag != FALSE )
		{
#ifndef __WINDOWS__
			goto ERR ;
#else
			// ＢＭＰデータ を欲しかった場合は GraphImage データを ＢＭＰ データに変換する

			// 画像フォーマットが DX_BASEIMAGE_FORMAT_NORMAL 以外だった場合は DX_BASEIMAGE_FORMAT_NORMAL に変換する
			NS_ConvertNormalFormatBaseImage( &GraphI ) ;

			// BITMAPINFO 構造体の情報を格納するメモリ領域の確保
			if( ( *BmpInfo = ( BITMAPINFO * )DXALLOC( sizeof( BITMAPINFO ) + sizeof( RGBQUAD ) * 256 ) ) == NULL )
			{
				DXFREE( GraphI.GraphData ) ;
				goto ERR ;
			}

			// 変換
			if( ( hr = NS_ConvGraphImageToBitmap( &GraphI, *BmpInfo, GraphData, FALSE, FALSE ) ) == -1 )
			{
				DXFREE( GraphI.GraphData ) ;
				DXFREE( *BmpInfo ) ;
			}

			// コピーが行われた場合はもとのグラフィックデータを解放する
			if( hr == 1 )
			{
				DXFREE( GraphI.GraphData ) ;
			}
#endif // __WINDOWS__
		}
		else
		{
			// 指定通りのデータを読み込めていた場合はデータをコピーして終了
			*BaseImage = GraphI ;
		}
	}

	// もしファイルイメージの場合はファイルを閉じる
	if( DataImageType == LOADIMAGE_TYPE_FILE ) GParam->StreamDataShred2.Close( fp ) ;

	// 終了
	return 0 ;

ERR:
	// もしファイルイメージの場合はファイルを閉じる
	if( DataImageType == LOADIMAGE_TYPE_FILE )
	{
		GParam->StreamDataShred2.Close( fp ) ;
	}
	else
	{
		STCLOSE( &Src ) ;
	}

	return -1 ;
}

// 登録されている各種グラフィックローダ関数から、ＢＭＰデータもしくは GraphImageデータを構築する
extern int NS_CreateGraphImageOrDIBGraph(
	const TCHAR *FileName,
	const void *DataImage, int DataImageSize, int DataImageType,
	int BmpFlag, int ReverseFlag,
	 BASEIMAGE *BaseImage, BITMAPINFO **BmpInfo, void **GraphData
)
{
#ifdef UNICODE
	return CreateGraphImageOrDIBGraph_WCHAR_T(
		FileName,
		DataImage, DataImageSize, DataImageType,
		BmpFlag, ReverseFlag, FALSE,
		BaseImage, BmpInfo, GraphData
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, return -1 )

	Result = CreateGraphImageOrDIBGraph_WCHAR_T(
		UseFileNameBuffer,
		DataImage, DataImageSize, DataImageType,
		BmpFlag, ReverseFlag, FALSE,
		BaseImage, BmpInfo, GraphData
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( FileName )

	return Result ;
#endif
}

// 画像ファイル若しくはメモリ上に展開された画像ファイルイメージを読み込み、基本イメージデータ若しくはＢＭＰデータを構築する
extern int NS_CreateGraphImageOrDIBGraphWithStrLen( const TCHAR *FileName, size_t FileNameLength, const void *DataImage, int DataImageSize, int DataImageType /* LOADIMAGE_TYPE_FILE 等 */ , int BmpFlag, int ReverseFlag, BASEIMAGE *BaseImage, BITMAPINFO **BmpInfo, void **GraphData )
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = CreateGraphImageOrDIBGraph_WCHAR_T(
		UseFileNameBuffer,
		DataImage, DataImageSize, DataImageType,
		BmpFlag, ReverseFlag, FALSE,
		BaseImage, BmpInfo, GraphData
	) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = CreateGraphImageOrDIBGraph_WCHAR_T(
		UseFileNameBuffer,
		DataImage, DataImageSize, DataImageType,
		BmpFlag, ReverseFlag, FALSE,
		BaseImage, BmpInfo, GraphData
	) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#endif
	return Result ;
}

// 登録されている各種グラフィックローダ関数から、ＢＭＰデータもしくは GraphImageデータを構築する
extern int CreateGraphImageOrDIBGraph_WCHAR_T(
	const wchar_t *FileName,
	const void *DataImage, int DataImageSize, int DataImageType,
	int BmpFlag, int ReverseFlag, int NotUseTransColor,
	 BASEIMAGE *BaseImage, BITMAPINFO **BmpInfo, void **GraphData
)
{
	LOADBASEIMAGE_GPARAM GParam ;

	InitLoadBaseImageGParam( &GParam, FALSE ) ;
	return CreateGraphImageOrDIBGraph_UseGParam(
		&GParam,
		FileName,
		DataImage, DataImageSize, DataImageType,
		BmpFlag, ReverseFlag, NotUseTransColor,
		BaseImage, BmpInfo, GraphData
	) ;
}

// 画像ファイルの横ピクセル数と縦ピクセル数を取得する
extern int NS_GetImageSize_File( const TCHAR *FileName, int *SizeX, int *SizeY )
{
#ifdef UNICODE
	return GetImageSize_File_WCHAR_T( FileName, SizeX, SizeY ) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, return -1 )

	Result = GetImageSize_File_WCHAR_T( UseFileNameBuffer, SizeX, SizeY ) ;

	TCHAR_TO_WCHAR_T_STRING_END( FileName )

	return Result ;
#endif
}

// 画像ファイルの横ピクセル数と縦ピクセル数を取得する
extern int NS_GetImageSize_FileWithStrLen( const TCHAR *FileName, size_t FileNameLength, int *SizeX, int *SizeY )
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = GetImageSize_File_WCHAR_T( UseFileNameBuffer, SizeX, SizeY ) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = GetImageSize_File_WCHAR_T( UseFileNameBuffer, SizeX, SizeY ) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#endif
	return Result ;
}

// 画像ファイルの横ピクセル数と縦ピクセル数を取得する
extern int GetImageSize_File_WCHAR_T( const wchar_t *FileName, int *SizeX, int *SizeY )
{
	LOADBASEIMAGE_GPARAM GParam ;
	BASEIMAGE BaseImage ;
	int Result ;

	InitLoadBaseImageGParam( &GParam, TRUE ) ;
	Result = CreateGraphImageOrDIBGraph_UseGParam(
		&GParam,
		FileName,
		NULL, 0, LOADIMAGE_TYPE_FILE,
		FALSE, FALSE, FALSE,
		&BaseImage, NULL, NULL
	) ;
	if( Result < 0 )
	{
#ifndef DX_NON_MOVIE

		// ムービーグラフィックとして扱おうとしてみる
		return GetMovieImageSize_File_WCHAR_T( FileName, SizeX, SizeY ) ;

#else // DX_NON_MOVIE

		return -1 ;

#endif // DX_NON_MOVIE
	}

	if( SizeX )
	{
		*SizeX = BaseImage.Width ;
	}

	if( SizeY )
	{
		*SizeY = BaseImage.Height ;
	}

	return 0 ;
}

// メモリ上に展開された画像ファイルの横ピクセル数と縦ピクセル数を取得する
extern int NS_GetImageSize_Mem( const void *FileImage, int FileImageSize, int *SizeX, int *SizeY )
{
	LOADBASEIMAGE_GPARAM GParam ;
	BASEIMAGE BaseImage ;
	int Result ;

	InitLoadBaseImageGParam( &GParam, TRUE ) ;
	Result = CreateGraphImageOrDIBGraph_UseGParam(
		&GParam,
		NULL,
		FileImage, FileImageSize, LOADIMAGE_TYPE_MEM,
		FALSE, FALSE, FALSE,
		&BaseImage, NULL, NULL
	) ;
	if( Result < 0 )
	{
#ifndef DX_NON_MOVIE

		// ムービーグラフィックとして扱おうとしてみる
		return NS_GetMovieImageSize_Mem( FileImage, FileImageSize, SizeX, SizeY ) ;

#else // DX_NON_MOVIE

		return -1 ;

#endif // DX_NON_MOVIE
	}

	if( SizeX )
	{
		*SizeX = BaseImage.Width ;
	}

	if( SizeY )
	{
		*SizeY = BaseImage.Height ;
	}

	return 0 ;
}

// CreateGraphImageType2 のグローバル変数にアクセスしないバージョン
extern int CreateGraphImageType2_UseGParam( CREATEBASEIMAGETYPE2_GPARAM *GParam, STREAMDATA *Src, BASEIMAGE *Dest )
{
	int i ;

	// 出力先を零初期化
	_MEMSET( Dest, 0, sizeof( BASEIMAGE ) ) ;

	// 先ずユーザー定義関数に噛ませる
	for( i = 0 ; i < GParam->UserImageLoadFuncNum4 ; i ++ )
	{
		STSEEK( Src, 0, STREAM_SEEKTYPE_SET ) ;
		_MEMSET( Dest, 0, sizeof( BASEIMAGE ) ) ;
		if( GParam->UserImageLoadFunc4[i]( Src, Dest ) == 0 ) break ;
	}
	// 読み込めていたら終了
	if( i != GParam->UserImageLoadFuncNum4 )
	{
		return 0 ;
	}

	// 環境依存のデフォルト関数を噛ませる
	for( i = 0 ; DefaultImageLoadFunc_PF[ i ] != NULL ; i ++ )
	{
		STSEEK( Src, 0, STREAM_SEEKTYPE_SET ) ;
		_MEMSET( Dest, 0, sizeof( BASEIMAGE ) ) ;
		if( DefaultImageLoadFunc_PF[ i ]( Src, Dest, GParam->GetFormatOnly ) == 0 ) break ;
	}
	// 読み込めていたら終了
	if( DefaultImageLoadFunc_PF[ i ] != NULL )
	{
		return 0 ;
	}

	// 次にデフォルト関数を噛ませる
	for( i = 0 ; DefaultImageLoadFunc[i] != NULL ; i ++ )
	{
		STSEEK( Src, 0, STREAM_SEEKTYPE_SET ) ;
		_MEMSET( Dest, 0, sizeof( BASEIMAGE ) ) ;
		if( DefaultImageLoadFunc[i]( Src, Dest, GParam->GetFormatOnly ) == 0 ) break ;
	}

	// 終了
	return DefaultImageLoadFunc[i] == NULL ? -1 : 0 ;
}

// 汎用読み込み処理によるグラフィックイメージ構築関数( 0:成功  -1:失敗 )
extern int NS_CreateGraphImageType2( STREAMDATA *Src, BASEIMAGE *Dest )
{
	CREATEBASEIMAGETYPE2_GPARAM GParam ;

	InitCreateBaseImageType2GParam( &GParam, FALSE ) ;

	return CreateGraphImageType2_UseGParam( &GParam, Src, Dest ) ;
}

#ifdef __WINDOWS__

// ＤＩＢグラフィックを作成する
extern HBITMAP NS_CreateDIBGraph( const TCHAR *FileName, int ReverseFlag, COLORDATA *SrcColor )
{
	// バージョン２に渡す
	return NS_CreateDIBGraphVer2( FileName, NULL, 0, LOADIMAGE_TYPE_FILE, ReverseFlag, SrcColor ) ;
}

// 画像ファイルからＤＩＢデータを作成する
extern HBITMAP NS_CreateDIBGraphWithStrLen( const TCHAR *FileName, size_t FileNameLength, int ReverseFlag, COLORDATA *SrcColor )
{
	HBITMAP Result ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_ONE_BEGIN( FileName, FileNameLength, return NULL )
	Result = NS_CreateDIBGraphVer2( UseFileNameBuffer, NULL, 0, LOADIMAGE_TYPE_FILE, ReverseFlag, SrcColor ) ; ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( FileName )
	return Result ;
}

// ＤＩＢグラフィックを作成する
extern HBITMAP CreateDIBGraph_WCHAR_T( const wchar_t *FileName, int ReverseFlag, COLORDATA *SrcColor )
{
	// バージョン２に渡す
	return CreateDIBGraphVer2_WCHAR_T( FileName, NULL, 0, LOADIMAGE_TYPE_FILE, ReverseFlag, SrcColor ) ;
}

// ＤＩＢグラフィックを作成する(バージョン２)
extern HBITMAP NS_CreateDIBGraphVer2( const TCHAR *FileName, const void *MemImage, int MemImageSize, int ImageType, int ReverseFlag, COLORDATA *SrcColor )
{
#ifdef UNICODE
	return CreateDIBGraphVer2_WCHAR_T(
		FileName, MemImage, MemImageSize, ImageType, ReverseFlag, SrcColor
	) ;
#else
	HBITMAP Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, return NULL )

	Result = CreateDIBGraphVer2_WCHAR_T(
		UseFileNameBuffer, MemImage, MemImageSize, ImageType, ReverseFlag, SrcColor
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( FileName )

	return Result ;
#endif
}

// 画像ファイル若しくはメモリ上に展開された画像ファイルイメージからＤＩＢデータを作成する
extern HBITMAP NS_CreateDIBGraphVer2WithStrLen( const TCHAR *FileName, size_t FileNameLength, const void *MemImage, int MemImageSize, int ImageType, int ReverseFlag, COLORDATA *SrcColor )
{
	HBITMAP Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return NULL )
	Result = CreateDIBGraphVer2_WCHAR_T( UseFileNameBuffer, MemImage, MemImageSize, ImageType, ReverseFlag, SrcColor ) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return NULL )
	Result = CreateDIBGraphVer2_WCHAR_T( UseFileNameBuffer, MemImage, MemImageSize, ImageType, ReverseFlag, SrcColor ) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#endif
	return Result ;
}

// ＤＩＢグラフィックを作成する(バージョン２)
extern HBITMAP CreateDIBGraphVer2_WCHAR_T( const wchar_t *FileName, const void *MemImage, int MemImageSize, int ImageType, int ReverseFlag, COLORDATA *SrcColor )
{
	void *GraphData = NULL ;
	BITMAPINFO *BmpInfo = NULL ;
	HBITMAP Bmp ;

	// ファイルの読み込み
	if( CreateGraphImageOrDIBGraph_WCHAR_T( FileName, MemImage, MemImageSize, ImageType, TRUE, FALSE, FALSE, NULL, &BmpInfo, &GraphData ) == -1 )
	{
		return NULL ;
	}

	// HBITMAP の作成
	Bmp = NS_CreateDIBGraphToMem( BmpInfo, GraphData, ReverseFlag, SrcColor ) ;

	// データの解放
	DXFREE( GraphData ) ;
	DXFREE( BmpInfo ) ;

	// ハンドルを返す
	return Bmp ;
}

#endif // __WINDOWS__

// フルカラー形式のBITMAPINFO構造体を作成する
extern int NS_CreateBmpInfo( BITMAPINFO *BmpInfo, int Width, int Height, int Pitch, const void *SrcGrData, void **DestGrData )
{
	BYTE *DestData, *SrcData ;
	int i ;
	int DPitch, SPitch ;

	// ピッチ作成
	SPitch = ( Pitch == 0 ) ? Width * 3 : Pitch ;
	DPitch = Width * 3 ;
	DPitch = ( ( DPitch + 3 ) / 4 ) * 4 ;

	// キャスト
	SrcData = ( BYTE * )SrcGrData ;

	// グラフィックデータを複製
	if( ( *DestGrData = ( BYTE *)DXCALLOC( ( size_t )( Height * DPitch ) ) ) == NULL ) return -1 ;
	DestData = ( BYTE * )*DestGrData ;
	if( DPitch == SPitch )
	{
		_MEMCPY( DestData, SrcData, ( size_t )( Height * DPitch ) ) ;
	}
	else
	{
		for( i = 0 ; i < Height ; i ++, SrcData += SPitch, DestData += DPitch )
			_MEMCPY( DestData, SrcData, ( size_t )SPitch ) ;
	}

	// BITMAPINFO構造体のセット
	_MEMSET( BmpInfo, 0, sizeof( *BmpInfo ) ) ;
	BmpInfo->bmiHeader.biSize	= sizeof( BITMAPINFOHEADER ) ;
	BmpInfo->bmiHeader.biWidth	= Width ;
	BmpInfo->bmiHeader.biHeight	= -Height ;
	BmpInfo->bmiHeader.biPlanes	= 1 ;
	BmpInfo->bmiHeader.biBitCount= 24 ;

	// 終了
	return 0 ;
}

// BASEIMAGE 構造体の画像情報から指定の座標のフルカラーコードを取得する
extern unsigned int NS_GetGraphImageFullColorCode( const BASEIMAGE *GraphImage, int x, int y )
{
	BYTE *gp ;
	const COLORDATA * cr ;
	unsigned int code, src = 0 ;

	if( GraphImage->GraphData == NULL ) return 0 ;
	
	cr = &GraphImage->ColorData ;
	gp = (BYTE *)GraphImage->GraphData + GraphImage->Pitch * y + cr->PixelByte * x ;

	switch( cr->PixelByte )
	{
	case 2 : src = *((WORD *)gp) ; break ;
	case 3 : src = *((DWORD *)gp) & 0xffffff ; break ;
	case 4 : src = *((DWORD *)gp) ; break ;
	}

	// フルカラーの色コードを作成	
	code  = ( ( src & cr->RedMask   ) >> cr->RedLoc   ) << ( 16 + ( 8 - cr->RedWidth   ) ) ;
	code |= ( ( src & cr->GreenMask ) >> cr->GreenLoc ) << (  8 + ( 8 - cr->GreenWidth ) ) ;
	code |= ( ( src & cr->BlueMask  ) >> cr->BlueLoc  ) << (  0 + ( 8 - cr->BlueWidth  ) ) ;
	if( cr->AlphaWidth != 0 ) code |= ( ( src & cr->AlphaMask ) >> cr->AlphaLoc ) << ( 24 + ( 8 - cr->AlphaWidth ) ) ;

	// 色コードを返す
	return code ;
}

// アルファマスク用のファイルパスを作成する
static int CreateAlphaMaskFilePath( const wchar_t *Path, wchar_t *Dest, size_t BufferBytes )
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

	// アルファマスク用ファイルネームの作成
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
	j += PutCharCode( '_', WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )Dest )[ j ], BufferBytes - j ) ;
	j += PutCharCode( 'a', WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )Dest )[ j ], BufferBytes - j ) ;
	_WCSCPY_S( ( wchar_t * )&( ( BYTE * )Dest )[ j ], BufferBytes - j, ( const wchar_t * )&( ( BYTE * )Path )[ i ] ) ;

	return 0 ;
}

static int CreateAlphaMaskFilePathT( const TCHAR *Path, TCHAR *Dest, size_t BufferBytes )
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
		CharCode = GetCharCode( ( const char * )&( ( BYTE * )Path )[ i ], _TCHARCODEFORMAT, &CharBytes ) ;
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

	// アルファマスク用ファイルネームの作成
	i = 0 ;
	for(;;)
	{
		CharCode = GetCharCode( ( const char * )&( ( BYTE * )Path )[ i ], _TCHARCODEFORMAT, &CharBytes ) ;
		if( CharCode == '\0' || i == LastPoint )
		{
			break ;
		}

		PutCharCode( CharCode, _TCHARCODEFORMAT, ( char * )&( ( BYTE * )Dest )[ i ], BufferBytes - i ) ;
		i += CharBytes ;
	}

	j = i ;
	j += PutCharCode( '_', _TCHARCODEFORMAT, ( char * )&( ( BYTE * )Dest )[ j ], BufferBytes - j ) ;
	j += PutCharCode( 'a', _TCHARCODEFORMAT, ( char * )&( ( BYTE * )Dest )[ j ], BufferBytes - j ) ;
	_TSTRCPY_S( ( TCHAR * )&( ( BYTE * )Dest )[ j ], BufferBytes - j, ( const TCHAR * )&( ( BYTE * )Path )[ i ] ) ;

	return 0 ;
}

// CreateGraphImage_plus_Alpha のグローバル変数にアクセスしないバージョン
extern int CreateGraphImage_plus_Alpha_UseGParam(
	LOADBASEIMAGE_GPARAM *GParam,
	const wchar_t *FileName,
	const void *RgbImage,
	int RgbImageSize,
	int RgbImageType,
	const void *AlphaImage,
	int AlphaImageSize,
	int AlphaImageType,
	BASEIMAGE *RgbGraphImage,
	BASEIMAGE *AlphaGraphImage,
	int ReverseFlag,
	int NotUseTransColor
)
{
	int Result ;

	// ＲＧＢイメージの読み込み
	if( CreateGraphImageOrDIBGraph_UseGParam( GParam, FileName, RgbImage, RgbImageSize, RgbImageType, FALSE, ReverseFlag, NotUseTransColor, RgbGraphImage, NULL, NULL ) == -1 )
		return -1 ;

	if( AlphaGraphImage != NULL )
	{
		switch( AlphaImageType )
		{
		case LOADIMAGE_TYPE_MEM :
			// メモリ上のアルファマップイメージの読み込み

			// 読み込み。失敗したら RGB だけ取得できたということ、成功したら Alpha も取得できたということ
			Result = CreateGraphImageOrDIBGraph_UseGParam( GParam, NULL, AlphaImage, AlphaImageSize, AlphaImageType, FALSE, ReverseFlag, FALSE, AlphaGraphImage, NULL, NULL ) == -1 ? 1 : 0 ;
			return Result ;

		case LOADIMAGE_TYPE_FILE :
			// ファイル上のアルファマップイメージの読み込み
			wchar_t AlphaName[FILEPATH_MAX] ;
			CreateAlphaMaskFilePath( FileName, AlphaName, sizeof( AlphaName ) ) ;

			// 読み込み。失敗したら RGB だけ取得できたということ、成功したら Alpha も取得できたということ
			Result = CreateGraphImageOrDIBGraph_UseGParam( GParam, AlphaName, NULL, 0, AlphaImageType, FALSE, ReverseFlag, FALSE, AlphaGraphImage, NULL, NULL ) == -1 ? 1 : 0 ;
			return Result ;
		}
	}

	// ここへ来たということは RGB だけ取得できたということ
	return 1 ;
}

// 各種グラフィックデータからグラフィックイメージデータとアルファマップ用イメージデータを構築する
// Ret:  0:RGBとAlphaどちらも取得  1:RGBのみ取得  -1:どちらも取得できなかった
extern int NS_CreateGraphImage_plus_Alpha(
	const TCHAR *FileName,
	const void *RgbImage,
	int RgbImageSize,
	int RgbImageType,
	const void *AlphaImage,
	int AlphaImageSize,
	int AlphaImageType,
	BASEIMAGE *RgbGraphImage,
	BASEIMAGE *AlphaGraphImage,
	int ReverseFlag
)
{
#ifdef UNICODE
	return CreateGraphImage_plus_Alpha_WCHAR_T(
		FileName,
		RgbImage,
		RgbImageSize,
		RgbImageType,
		AlphaImage,
		AlphaImageSize,
		AlphaImageType,
		RgbGraphImage,
		AlphaGraphImage,
		ReverseFlag,
		FALSE
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, return -1 )

	Result = CreateGraphImage_plus_Alpha_WCHAR_T(
		UseFileNameBuffer,
		RgbImage,
		RgbImageSize,
		RgbImageType,
		AlphaImage,
		AlphaImageSize,
		AlphaImageType,
		RgbGraphImage,
		AlphaGraphImage,
		ReverseFlag,
		FALSE
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( FileName )

	return Result ;
#endif
}

// 画像ファイル若しくはメモリ上に展開された画像ファイルイメージを読み込み、基本イメージデータを構築する
extern int NS_CreateGraphImage_plus_AlphaWithStrLen( const TCHAR *FileName, size_t FileNameLength, const void *RgbImage, int RgbImageSize, int RgbImageType, const void *AlphaImage, int AlphaImageSize, int AlphaImageType, BASEIMAGE *RgbGraphImage, BASEIMAGE *AlphaGraphImage, int ReverseFlag )
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = CreateGraphImage_plus_Alpha_WCHAR_T(
		UseFileNameBuffer,
		RgbImage,
		RgbImageSize,
		RgbImageType,
		AlphaImage,
		AlphaImageSize,
		AlphaImageType,
		RgbGraphImage,
		AlphaGraphImage,
		ReverseFlag,
		FALSE
	) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = CreateGraphImage_plus_Alpha_WCHAR_T(
		UseFileNameBuffer,
		RgbImage,
		RgbImageSize,
		RgbImageType,
		AlphaImage,
		AlphaImageSize,
		AlphaImageType,
		RgbGraphImage,
		AlphaGraphImage,
		ReverseFlag,
		FALSE
	) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#endif
	return Result ;
}

// 各種グラフィックデータからグラフィックイメージデータとアルファマップ用イメージデータを構築する
// Ret:  0:RGBとAlphaどちらも取得  1:RGBのみ取得  -1:どちらも取得できなかった
extern int CreateGraphImage_plus_Alpha_WCHAR_T(
	const wchar_t *FileName,
	const void *RgbImage,
	int RgbImageSize,
	int RgbImageType,
	const void *AlphaImage,
	int AlphaImageSize,
	int AlphaImageType,
	BASEIMAGE *RgbGraphImage,
	BASEIMAGE *AlphaGraphImage,
	int ReverseFlag,
	int NotUseTransColor
)
{
	LOADBASEIMAGE_GPARAM GParam ;

	InitLoadBaseImageGParam( &GParam, FALSE ) ;
	return CreateGraphImage_plus_Alpha_UseGParam(
		&GParam,
		FileName,
		RgbImage, RgbImageSize, RgbImageType,
		AlphaImage, AlphaImageSize, AlphaImageType,
		RgbGraphImage, AlphaGraphImage,
		ReverseFlag, NotUseTransColor
	) ;
}








// 指定の GraphImage を左右反転する
extern int NS_ReverseGraphImage( BASEIMAGE *GraphImage )
{
	int height, width, pitch, i ;
	DWORD tmp ;
	
	if( GraphImage->Width == 0 || GraphImage->Height == 0 ) return -1 ;
	
	switch( GraphImage->ColorData.PixelByte )
	{
	case 1 :
		{
			BYTE *bufL, *bufR ;
			
			bufL = (BYTE *)GraphImage->GraphData ;
			bufR = (BYTE *)GraphImage->GraphData + GraphImage->Width - 1 ;
			height = GraphImage->Height ;
			width = GraphImage->Width / 2 ;
			pitch = GraphImage->Pitch ;
			do
			{
				for( i = 0 ; i < width ; i ++ )
				{
					tmp = bufL[i] ;
					bufL[i] = bufR[-i] ;
					bufR[-i] = (BYTE)tmp ;
				}
				bufL += pitch ;
				bufR += pitch ;
			}while( -- height ) ;
		}
		break ;
		
	case 2 :
		{
			WORD *bufL, *bufR ;
			
			bufL = (WORD *)GraphImage->GraphData ;
			bufR = (WORD *)GraphImage->GraphData + GraphImage->Width - 1 ;
			height = GraphImage->Height ;
			width = GraphImage->Width / 2 ;
			pitch = GraphImage->Pitch ;
			do
			{
				for( i = 0 ; i < width ; i ++ )
				{
					tmp = bufL[i] ;
					bufL[i] = bufR[-i] ;
					bufR[-i] = (WORD)tmp ;
				}
				bufL = (WORD *)( (BYTE *)bufL + pitch ) ;
				bufR = (WORD *)( (BYTE *)bufR + pitch ) ;
			}while( -- height ) ;
		}
		break ;

	case 3 :
		{
			BYTE *bufL, *bufR ;
			
			bufL = (BYTE *)GraphImage->GraphData ;
			bufR = (BYTE *)GraphImage->GraphData + ( GraphImage->Width - 1 ) * 3 ;
			height = GraphImage->Height ;
			width = ( GraphImage->Width / 2 ) * 3 ;
			pitch = GraphImage->Pitch ;
			do
			{
				for( i = 0 ; i < width ; i += 3 )
				{
					tmp = *((WORD *)(&bufL[i])) ;
					*((WORD *)(&bufL[i])) = *((WORD *)(&bufR[-i])) ;
					*((WORD *)(&bufR[-i])) = (WORD)tmp ;
					tmp = *((BYTE *)(&bufL[i+2])) ;
					*((BYTE *)(&bufL[i+2])) = *((BYTE *)(&bufR[-i+2])) ;
					*((BYTE *)(&bufR[-i+2])) = (BYTE)tmp ;
				}
				bufL += pitch ;
				bufR += pitch ;
			}while( -- height ) ;
		}
		break ;

	case 4 :
		{
			DWORD *bufL, *bufR ;
			
			bufL = (DWORD *)GraphImage->GraphData ;
			bufR = (DWORD *)GraphImage->GraphData + GraphImage->Width - 1 ;
			height = GraphImage->Height ;
			width = GraphImage->Width / 2 ;
			pitch = GraphImage->Pitch ;
			do
			{
				for( i = 0 ; i < width ; i ++ )
				{
					tmp = bufL[i] ;
					bufL[i] = bufR[-i] ;
					bufR[-i] = tmp ;
				}
				bufL = (DWORD *)( (BYTE *)bufL + pitch ) ;
				bufR = (DWORD *)( (BYTE *)bufR + pitch ) ;
			}while( -- height ) ;
		}
		break ;
	}

	// 終了
	return 0 ;	
}





#ifdef __WINDOWS__

// ＤＩＢグラフィックを作成する
extern int NS_CreateDIBGraphVer2_plus_Alpha( const TCHAR *FileName, const void *MemImage, int MemImageSize, const void *AlphaImage, int AlphaImageSize, int ImageType, HBITMAP *RGBBmp, HBITMAP *AlphaBmp, int ReverseFlag, COLORDATA *SrcColor )
{
#ifdef UNICODE
	return CreateDIBGraphVer2_plus_Alpha_WCHAR_T(
		FileName, MemImage, MemImageSize, AlphaImage, AlphaImageSize, ImageType, RGBBmp, AlphaBmp, ReverseFlag, SrcColor
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, return -1 )

	Result = CreateDIBGraphVer2_plus_Alpha_WCHAR_T(
		UseFileNameBuffer, MemImage, MemImageSize, AlphaImage, AlphaImageSize, ImageType, RGBBmp, AlphaBmp, ReverseFlag, SrcColor
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( FileName )

	return Result ;
#endif
}

// 画像ファイル若しくはメモリ上に展開された画像ファイルイメージからＤＩＢデータとマスク用ＤＩＢデータを作成する
extern int NS_CreateDIBGraphVer2_plus_AlphaWithStrLen( const TCHAR *FileName, size_t FileNameLength, const void *MemImage, int MemImageSize, const void *AlphaImage, int AlphaImageSize, int ImageType, HBITMAP *RGBBmp, HBITMAP *AlphaBmp, int ReverseFlag, COLORDATA *SrcColor )
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = CreateDIBGraphVer2_plus_Alpha_WCHAR_T( UseFileNameBuffer, MemImage, MemImageSize, AlphaImage, AlphaImageSize, ImageType, RGBBmp, AlphaBmp, ReverseFlag, SrcColor ) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = CreateDIBGraphVer2_plus_Alpha_WCHAR_T( UseFileNameBuffer, MemImage, MemImageSize, AlphaImage, AlphaImageSize, ImageType, RGBBmp, AlphaBmp, ReverseFlag, SrcColor ) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#endif
	return Result ;
}

// ＤＩＢグラフィックを作成する
extern int CreateDIBGraphVer2_plus_Alpha_WCHAR_T( const wchar_t *FileName, const void *MemImage, int MemImageSize, const void *AlphaImage, int AlphaImageSize, int ImageType, HBITMAP *RGBBmp, HBITMAP *AlphaBmp, int ReverseFlag, COLORDATA *SrcColor )
{
	int Result = -1 ;

	// イメージタイプによって処理を分岐
	switch( ImageType )
	{
	case LOADIMAGE_TYPE_FILE :		// ファイルイメージの場合
		Result = CreateDIBGraph_plus_Alpha_WCHAR_T( FileName, RGBBmp, AlphaBmp, ReverseFlag, SrcColor ) ;		// バージョン１に投げる
		break ;

	case LOADIMAGE_TYPE_MEM :		// メモリイメージの場合
		*RGBBmp = CreateDIBGraphVer2_WCHAR_T( NULL, MemImage, MemImageSize, LOADIMAGE_TYPE_MEM, ReverseFlag, SrcColor ) ;
		if( *RGBBmp == NULL )
		{
			return -1 ;
		}
		if( AlphaBmp != NULL && AlphaImage != NULL && AlphaImageSize != 0 )
		{
			*AlphaBmp = CreateDIBGraphVer2_WCHAR_T( NULL, AlphaImage, AlphaImageSize, LOADIMAGE_TYPE_MEM, ReverseFlag, SrcColor ) ;
		}
		Result = 0 ;
		break ;
	}

	// 終了
	return Result ;
}

// ＤＩＢグラフィックをメモリイメージから作成する
extern HBITMAP NS_CreateDIBGraphToMem( const BITMAPINFO *BmpInfo, const void *GraphData, int ReverseFlag, COLORDATA *SrcColor )
{
	HDC hdc ;
	HBITMAP Bmp ; 
	BITMAP bm ;
	void *Data ;
	int SrcWidthByte ;
//	HRESULT hr ;
//	void *GData ;

	SETUP_WIN_API

	// ピッチの算出
	{
		int Byte ; 
		
		Byte = BmpInfo->bmiHeader.biWidth * BmpInfo->bmiHeader.biBitCount / 8 ;

		SrcWidthByte = Byte + ( Byte % 4 != 0 ? ( 4 - Byte % 4 ) : 0 ) ;
	}

	// ＤＩＢグラフィックの作成
	{
		hdc = WinAPIData.Win32Func.CreateCompatibleDCFunc( NULL ) ;

		// ＤＩＢデータを作成する
		{
			BITMAPINFO *BmpInfo2 ;
			int PlusByte = 0 ;
//			PlusByte = BmpInfo->bmiHeader.biBitCount <= 8 ? sizeof( RGBQUAD ) * ( 1 << BmpInfo->bmiHeader.biBitCount ) : 
//						( BmpInfo->bmiHeader.biBitCount == 16 ? 4 * 3 : 0 ) ;
			if( SrcColor != NULL &&
				( SrcColor->ColorBitDepth == 16 || ( SrcColor->ColorBitDepth > 24 && BmpInfo->bmiHeader.biBitCount != 24 ) ) )
			{
				PlusByte = 3 * 4 ;
			}
			else
			if( BmpInfo->bmiHeader.biBitCount <= 8 )
			{
				PlusByte = ( 1 << BmpInfo->bmiHeader.biBitCount ) * ( int )sizeof( RGBQUAD ) ;
			}
			else
			{
				PlusByte = 0 ;
			}

			if( ( BmpInfo2 = ( BITMAPINFO * )DXALLOC( sizeof( BITMAPINFOHEADER ) + PlusByte ) ) == NULL )
			{
				DXST_LOGFILE_ADDUTF16LE( "\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x69\x00\x6e\x00\x20\x00\x43\x00\x72\x00\x65\x00\x61\x00\x74\x00\x65\x00\x44\x00\x49\x00\x42\x00\x47\x00\x72\x00\x61\x00\x70\x00\x68\x00\x20\x00\x0a\x00\x00"/*@ L"メモリの確保に失敗しました in CreateDIBGraph \n" @*/ ) ;	

				// メモリの解放
				WinAPIData.Win32Func.DeleteDCFunc( hdc ) ;
				return NULL ;
			}

			// BITMAPINFOHEADER を設定
			_MEMSET( BmpInfo2, 0, sizeof( BITMAPINFOHEADER ) ) ;
			BmpInfo2->bmiHeader.biSize	= sizeof( BITMAPINFOHEADER ) ;
			BmpInfo2->bmiHeader.biWidth  = BmpInfo->bmiHeader.biWidth ;
			BmpInfo2->bmiHeader.biHeight = -_ABS(BmpInfo->bmiHeader.biHeight) ;
			BmpInfo2->bmiHeader.biPlanes = 1 ;

			if( SrcColor == NULL || ( SrcColor->ColorBitDepth == 16 && ( BASEIM.LowLoadFlag == TRUE || NS_GetGraphDataShavedMode() != DX_SHAVEDMODE_NONE ) ) )
			{
				if( BmpInfo->bmiHeader.biBitCount == 8 )
				{
					BmpInfo2->bmiHeader.biBitCount    = BmpInfo->bmiHeader.biBitCount ;
					BmpInfo2->bmiHeader.biCompression = BmpInfo->bmiHeader.biCompression ;
					_MEMCPY( BmpInfo2->bmiColors, BmpInfo->bmiColors, sizeof( RGBQUAD ) * 256 ) ;
				}
				else
				{
					BmpInfo2->bmiHeader.biBitCount = 24 ;
					BmpInfo2->bmiHeader.biCompression = BI_RGB ;
					if( SrcColor != NULL ) NS_CreateFullColorData( SrcColor ) ;
				}
			}
			else
			{
				if( SrcColor != NULL )
				{
					DWORD *Mask ;

	/*				if( SrcColor->ColorBitDepth != 24 )
					{
						Mask = ( DWORD * )( ( BYTE * )BmpInfo2 + sizeof( BITMAPINFOHEADER ) ) ;
						Mask[0] = SrcColor->RedMask ;
						Mask[1] = SrcColor->GreenMask ;
						Mask[2] = SrcColor->BlueMask ;
						BmpInfo2->bmiHeader.biCompression = BI_BITFIELDS ;
					}
					else
					{
						BmpInfo2->bmiHeader.biCompression = BI_RGB ;
					}
	*/
					if( SrcColor->ColorBitDepth == 16 ||
						( SrcColor->ColorBitDepth > 24 && BmpInfo->bmiHeader.biBitCount != 24 ) )
					{
						Mask = ( DWORD * )( ( BYTE * )BmpInfo2 + sizeof( BITMAPINFOHEADER ) ) ;
						Mask[0] = SrcColor->RedMask ;
						Mask[1] = SrcColor->GreenMask ;
						Mask[2] = SrcColor->BlueMask ;
						BmpInfo2->bmiHeader.biCompression = BI_BITFIELDS ;
					}
					else
					{
						if( SrcColor->ColorBitDepth == 32 )
						{
							BmpInfo2->bmiHeader.biBitCount = 24 ;
							NS_CreateFullColorData( SrcColor ) ;
						}

						BmpInfo2->bmiHeader.biCompression = BI_RGB ;
					}

					BmpInfo2->bmiHeader.biBitCount = SrcColor->ColorBitDepth ;
				}
				else
				{
					BmpInfo2->bmiHeader.biBitCount = 24 ;
					BmpInfo2->bmiHeader.biCompression = BI_RGB ;
				}
			}

//			_MEMCPY( BmpInfo2, BmpInfo, sizeof( BITMAPINFOHEADER ) + PlusByte ) ;
			if( ( Bmp = WinAPIData.Win32Func.CreateDIBSectionFunc( hdc, BmpInfo2, DIB_RGB_COLORS, &Data, NULL, 0 ) ) == NULL )
			{
				DXST_LOGFILE_ADDUTF16LE( "\x24\xff\x29\xff\x22\xff\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x33\x00\x0a\x00\x00"/*@ L"ＤＩＢの作成に失敗しました3\n" @*/ ) ;	

				// メモリの解放
				DXFREE( BmpInfo2 ) ;
				WinAPIData.Win32Func.DeleteDCFunc( hdc ) ;
				return NULL ;
			}

			DXFREE( BmpInfo2 ) ;
		}

		// ビットマップの情報を取得
		WinAPIData.Win32Func.GetObjectAFunc( Bmp, sizeof( bm ), &bm ) ;
		bm.bmWidthBytes = ( ( bm.bmWidthBytes + 3 ) / 4 ) * 4 ;

		// グラフィックデータをコピーする
		{
			if( Data == NULL )
			{
				DXST_LOGFILE_ADDUTF16LE( "\x24\xff\x29\xff\x22\xff\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x32\x00\x0a\x00\x00"/*@ L"ＤＩＢの作成に失敗しました2\n" @*/ ) ;
				return NULL ;
			}
			
			{
				HBITMAP OldBmp ;

				OldBmp = ( HBITMAP )WinAPIData.Win32Func.SelectObjectFunc( hdc, ( HGDIOBJ )Bmp ) ;

				if( ReverseFlag )
				{
					WinAPIData.Win32Func.StretchDIBitsFunc( hdc,
									                             0,                                  0,
									    BmpInfo->bmiHeader.biWidth,        BmpInfo->bmiHeader.biHeight, 
									BmpInfo->bmiHeader.biWidth - 1,                                  0,
									   -BmpInfo->bmiHeader.biWidth, _ABS(BmpInfo->bmiHeader.biHeight) ,
									GraphData, BmpInfo, DIB_RGB_COLORS, SRCCOPY ) ;
				}
				else
				{
					WinAPIData.Win32Func.SetDIBitsToDeviceFunc( hdc, 0, 0, ( DWORD )BmpInfo->bmiHeader.biWidth, ( DWORD )_ABS(BmpInfo->bmiHeader.biHeight) ,
											0 , 0 ,
											0, ( UINT )_ABS(BmpInfo->bmiHeader.biHeight), GraphData, BmpInfo, DIB_RGB_COLORS ) ;
				}

				WinAPIData.Win32Func.SelectObjectFunc( hdc, ( HGDIOBJ )OldBmp ) ;

			}
		}

		WinAPIData.Win32Func.DeleteDCFunc( hdc ) ;
	}

	// ＢＭＰハンドルを返す
	return Bmp ;
}

// ファイルからＤＩＢグラフィックとマスクグラフィックを作成する
extern int NS_CreateDIBGraph_plus_Alpha( const TCHAR *FileName, HBITMAP *RGBBmp, HBITMAP *AlphaBmp, int ReverseFlag, COLORDATA *SrcColor )
{
#ifdef UNICODE
	return CreateDIBGraph_plus_Alpha_WCHAR_T(
		FileName, RGBBmp, AlphaBmp, ReverseFlag, SrcColor
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, return -1 )

	Result = CreateDIBGraph_plus_Alpha_WCHAR_T(
		UseFileNameBuffer, RGBBmp, AlphaBmp, ReverseFlag, SrcColor
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( FileName )

	return Result ;
#endif
}

// 画像ファイルからＤＩＢデータとマスク用ＤＩＢデータを作成する
extern int NS_CreateDIBGraph_plus_AlphaWithStrLen( const TCHAR *FileName, size_t FileNameLength, HBITMAP *RGBBmp, HBITMAP *AlphaBmp, int ReverseFlag , COLORDATA *SrcColor)
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = CreateDIBGraph_plus_Alpha_WCHAR_T( UseFileNameBuffer, RGBBmp, AlphaBmp, ReverseFlag, SrcColor ) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = CreateDIBGraph_plus_Alpha_WCHAR_T( UseFileNameBuffer, RGBBmp, AlphaBmp, ReverseFlag, SrcColor ) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#endif
	return Result ;
}

// ファイルからＤＩＢグラフィックとマスクグラフィックを作成する
extern int CreateDIBGraph_plus_Alpha_WCHAR_T( const wchar_t *FileName, HBITMAP *RGBBmp, HBITMAP *AlphaBmp, int ReverseFlag, COLORDATA *SrcColor )
{
	wchar_t AlphaFileName[ 256 ] ;

	// グラフィックの読みこみ
	*RGBBmp = CreateDIBGraph_WCHAR_T( FileName, ReverseFlag, SrcColor ) ;
	if( *RGBBmp == NULL )
	{
		return -1 ;
	}

	// アルファマスクの読みこみ
	if( AlphaBmp )
	{
		CreateAlphaMaskFilePath( FileName, AlphaFileName, sizeof( AlphaFileName ) ) ;
		*AlphaBmp = CreateDIBGraph_WCHAR_T( AlphaFileName, ReverseFlag, SrcColor ) ;
	}

	// 終了
	return 0 ;
}

// ＢＭＰ を GraphImage に変換する( Ret 0:正常終了  1:コピーを行った  -1:エラー )
extern int NS_ConvBitmapToGraphImage( const BITMAPINFO *BmpInfo, void *GraphData, BASEIMAGE *GraphImage, int CopyFlag )
{
	const BITMAPINFOHEADER *BInfo = &BmpInfo->bmiHeader ;

	// 出力先を零初期化
	_MEMSET( GraphImage, 0, sizeof( BASEIMAGE ) ) ;

	// ヘッダ情報をセットする
	GraphImage->Width = BInfo->biWidth ;
	GraphImage->Height = _ABS( BInfo->biHeight ) ;

	// パレットカラーだったり、圧縮されていたり反転ビットマップだったりする場合は２４ビットカラーへの変換処理を行う
	if( BmpInfo->bmiHeader.biHeight >= 0 || 
		BInfo->biBitCount <= 8 || ( BInfo->biBitCount != 16 && BInfo->biBitCount != 24 && BInfo->biBitCount != 32 ) ||
		( BInfo->biCompression != BI_RGB && BInfo->biCompression != BI_BITFIELDS ) )
	{
		HDC hdc ;
		HBITMAP Bmp ;
		BITMAP bm ;
		int Height ;
		void *Data ;

		SETUP_WIN_API

		// 変換処理に使用するＤＣの作成
		hdc = WinAPIData.Win32Func.CreateCompatibleDCFunc( NULL ) ;

		// ２４ビットカラーＤＩＢデータを作成する
		{
			BITMAPINFO BmpInfo2 ;

			// ビットマップインフォヘッダを設定
			_MEMSET( &BmpInfo2, 0, sizeof( BITMAPINFOHEADER ) ) ;
			BmpInfo2.bmiHeader.biSize	= sizeof( BITMAPINFOHEADER ) ;
			BmpInfo2.bmiHeader.biWidth  = BmpInfo->bmiHeader.biWidth ;
			BmpInfo2.bmiHeader.biHeight = -_ABS(BmpInfo->bmiHeader.biHeight) ;
			BmpInfo2.bmiHeader.biPlanes = 1 ;
			BmpInfo2.bmiHeader.biBitCount = 24 ;
			BmpInfo2.bmiHeader.biCompression = BI_RGB ;

			Bmp = WinAPIData.Win32Func.CreateDIBSectionFunc( hdc, &BmpInfo2, DIB_RGB_COLORS, &Data, NULL, 0 ) ;
			if( Bmp == NULL || Data == NULL )
			{
				DXST_LOGFILE_ADDUTF16LE( "\x24\xff\x29\xff\x22\xff\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x33\x00\x0a\x00\x00"/*@ L"ＤＩＢの作成に失敗しました3\n" @*/ ) ;	
				WinAPIData.Win32Func.DeleteDCFunc( hdc ) ;
				return -1 ;
			}
		}

		// ビットマップの情報を取得
		WinAPIData.Win32Func.GetObjectAFunc( Bmp, sizeof( bm ), &bm ) ;
		bm.bmWidthBytes += bm.bmWidthBytes % 4 ? 4 - bm.bmWidthBytes % 4 : 0 ;

		// グラフィックデータ２４ビットに変換しつつコピーする
		Height = _ABS(bm.bmHeight) ;
		{
			HBITMAP OldBmp ;

			OldBmp = ( HBITMAP )WinAPIData.Win32Func.SelectObjectFunc( hdc, ( HGDIOBJ )Bmp ) ;
			WinAPIData.Win32Func.SetDIBitsToDeviceFunc( hdc, 0, 0, ( DWORD )BmpInfo->bmiHeader.biWidth, ( DWORD )_ABS(BmpInfo->bmiHeader.biHeight) ,
									0 , 0 ,
									0, ( UINT )_ABS(BmpInfo->bmiHeader.biHeight), GraphData, BmpInfo, DIB_RGB_COLORS ) ;
			WinAPIData.Win32Func.SelectObjectFunc( hdc, ( HGDIOBJ )OldBmp ) ;
		}

		// 不要になったＤＣの削除
		WinAPIData.Win32Func.DeleteDCFunc( hdc ) ;

		// ２４ビットカラー化したグラフィックを格納するメモリ領域の確保
		GraphImage->GraphData = DXALLOC( ( size_t )( Height * bm.bmWidthBytes ) ) ;
		if( GraphImage->GraphData == NULL )
		{
			DXST_LOGFILE_ADDUTF16LE( "\x12\xff\x14\xff\xd3\x30\xc3\x30\xc8\x30\xab\x30\xe9\x30\xfc\x30\x16\x53\x57\x30\x5f\x30\xb0\x30\xe9\x30\xd5\x30\xa3\x30\xc3\x30\xaf\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"２４ビットカラー化したグラフィックを格納するメモリ領域の確保に失敗しました\n" @*/ ) ;	
			return -1 ;
		}
		
		// ２４ビットカラー化したグラフィックのデータを専用の領域に転送
		_MEMCPY( GraphImage->GraphData, bm.bmBits, ( size_t )( Height * bm.bmWidthBytes ) ) ;

		// 変換用に作成したＤＩＢの削除
		WinAPIData.Win32Func.DeleteObjectFunc( Bmp ) ;
		
		// フルカラー用のカラー情報をセットする
		NS_CreateFullColorData( &GraphImage->ColorData ) ;
		
		// ピッチをセット
		GraphImage->Pitch = bm.bmWidthBytes ;
		
		// コピーフラグを立てる
		CopyFlag = TRUE ;
	}
	else
	// 圧縮タイプがビットフィールドだった場合は専用の処理を行う
	if( BInfo->biCompression == BI_BITFIELDS ) 
	{
		unsigned int *Mask ;
		COLORDATA * Color ;
		
		Mask = (unsigned int *)BmpInfo->bmiColors ;
		Color = &GraphImage->ColorData ;

		// カラービット深度をセット
		Color->ColorBitDepth = ( unsigned char )BInfo->biBitCount ;
		Color->PixelByte     = ( unsigned char )( Color->ColorBitDepth / 8 ) ;

		// ピッチを算出
		GraphImage->Pitch = GraphImage->Width * Color->PixelByte ;
		GraphImage->Pitch = GraphImage->Pitch + ( GraphImage->Pitch % 4 != 0 ? ( 4 - GraphImage->Pitch % 4 ) : 0 ) ;
		
		// ビットフィールドを調べて、ＲＧＢ各色のビットマスクを取得する
		{
			int i, j, w ;
		
			// Ｒ成分を調べる
			j = 1 ;
			for( i = 0 ; i < 32 && ( *Mask & j ) == 0 ; i ++, j <<= 1 ){}
			Color->RedLoc = ( unsigned char )i ;
			for( w = 0 ; i < 32 && ( *Mask & j ) != 0 ; i ++, w ++, j <<= 1 ){}
			Color->RedWidth = ( unsigned char )w ;
			Color->RedMask = *Mask ;
			
			Mask ++ ;
			
			// Ｇ成分を調べる
			j = 1 ;
			for( i = 0 ; i < 32 && ( *Mask & j ) == 0 ; i ++, j <<= 1 ){}
			Color->GreenLoc = ( unsigned char )i ;
			for( w = 0 ; i < 32 && ( *Mask & j ) != 0 ; i ++, w ++, j <<= 1 ){}
			Color->GreenWidth = ( unsigned char )w ;
			Color->GreenMask = *Mask ;
			
			Mask ++ ;
			
			// Ｂ成分を調べる
			j = 1 ;
			for( i = 0 ; i < 32 && ( *Mask & j ) == 0 ; i ++, j <<= 1 ){}
			Color->BlueLoc = ( unsigned char )i ;
			for( w = 0 ; i < 32 && ( *Mask & j ) != 0 ; i ++, w ++, j <<= 1 ){}
			Color->BlueWidth = ( unsigned char )w ;
			Color->BlueMask = *Mask ;
			
			// アルファ成分はなし
			Color->AlphaWidth = 0 ;
			Color->AlphaLoc = 0 ;
			Color->AlphaMask = 0 ;

			// NoneMask をセットする
			NS_SetColorDataNoneMask( Color ) ;
		}
		
		// コピーフラグが立っていたらデータをコピーする
		if( CopyFlag == TRUE )
		{
			// コピーしたデータを格納するメモリ領域の確保
			GraphImage->GraphData = DXALLOC( ( size_t )( GraphImage->Pitch * GraphImage->Height ) ) ;
			if( GraphImage->GraphData == NULL ) return -1 ;
			
			// データのコピー
			_MEMCPY( GraphImage->GraphData, GraphData, ( size_t )( GraphImage->Pitch * GraphImage->Height ) ) ;
		}
		else
		{
			GraphImage->GraphData = GraphData ;
		}
	}
	else
	// 圧縮タイプが無圧縮だった場合は通常の処理を行う
	if( BInfo->biCompression == BI_RGB && ( BInfo->biBitCount == 16 || BInfo->biBitCount == 24 ) ) 
	{
		COLORDATA * Color ;

		Color = &GraphImage->ColorData ;

		// カラービット深度をセット
		Color->ColorBitDepth = ( unsigned char )BInfo->biBitCount ;
		Color->PixelByte     = ( unsigned char )( Color->ColorBitDepth / 8 ) ;

		// ピッチを算出
		GraphImage->Pitch = GraphImage->Width * Color->PixelByte ;
		GraphImage->Pitch = GraphImage->Pitch + ( GraphImage->Pitch % 4 != 0 ? ( 4 - GraphImage->Pitch % 4 ) : 0 ) ;
		
		// カラービット深度ごとにカラーマスクを埋める
		switch( BInfo->biBitCount )
		{
		case 16 :		// １６ビットカラーだった場合はＲ５Ｇ５Ｂ５
			Color->RedWidth = 5 ; Color->RedLoc = 10 ; Color->RedMask = 0x7c00 ;
			Color->GreenWidth = 5 ; Color->GreenLoc = 5 ; Color->GreenMask = 0x3e0 ;
			Color->BlueWidth = 5 ; Color->BlueLoc = 0 ; Color->BlueMask = 0x1f ;
			break ;
			
		case 24 :		// ２４ビットカラーだった場合はＲ８Ｇ８Ｂ８
			Color->RedWidth = 8 ; Color->RedLoc = 16 ; Color->RedMask = 0xff0000 ;
			Color->GreenWidth = 8 ; Color->GreenLoc = 8 ; Color->GreenMask = 0xff00 ;
			Color->BlueWidth = 8 ; Color->BlueLoc = 0 ; Color->BlueMask = 0xff ;
			break ;
		}

		// アルファは無し		
		Color->AlphaWidth = 0 ; Color->AlphaLoc = 0 ; Color->AlphaMask = 0 ;

		// NoneMask をセットする
		NS_SetColorDataNoneMask( Color ) ;
		
		// コピーフラグが立っていたらデータをコピーする
		if( CopyFlag == TRUE )
		{
			// コピーしたデータを格納するメモリ領域の確保
			GraphImage->GraphData = DXALLOC( ( size_t )( GraphImage->Pitch * GraphImage->Height ) ) ;
			if( GraphImage->GraphData == NULL ) return -1 ;
			
			// データのコピー
			_MEMCPY( GraphImage->GraphData, GraphData, ( size_t )( GraphImage->Pitch * GraphImage->Height ) ) ;
		}
		else
		{
			GraphImage->GraphData = GraphData ;
		}
	}
	else
	// 今までの処理に該当しなかったらエラー
	return -1 ;	
		
	// コピーしたかどうかを返す
	return CopyFlag ;
}

// GraphImage を ＢＭＰ に変換する(アルファデータはあっても無視される)( Ret 0:正常終了  1:コピーを行った  -1:エラー )
extern int NS_ConvGraphImageToBitmap( const BASEIMAGE *GraphImage, BITMAPINFO *BmpInfo, void **GraphData, int CopyFlag, int FullColorConv )
{
	const COLORDATA *SrcColor ;
	int Pitch, i ;

	// ８ビットカラーの場合
	if( GraphImage->ColorData.ColorBitDepth == 8 && FullColorConv == FALSE )
	{
		// ８ビットカラーの BITMAPINFOHEADER の情報を詰める
		_MEMSET( BmpInfo, 0, sizeof( *BmpInfo ) ) ;
		BmpInfo->bmiHeader.biSize	= sizeof( BITMAPINFOHEADER ) ;
		BmpInfo->bmiHeader.biWidth	= GraphImage->Width ;
		BmpInfo->bmiHeader.biHeight	= -GraphImage->Height ;
		BmpInfo->bmiHeader.biPlanes	= 1 ;
		BmpInfo->bmiHeader.biBitCount= 8 ;
		BmpInfo->bmiHeader.biCompression = BI_RGB ;

		// パレットのコピー
		_MEMCPY( BmpInfo->bmiColors, GraphImage->ColorData.Palette, sizeof( DWORD ) * 256 ) ;

		// ピッチの算出
		Pitch = ( ( GraphImage->Width + 3 ) / 4 ) * 4 ;

		// コピーフラグが立っているか、ピッチが４の倍数ではない場合は内容をコピーする
		if( CopyFlag == TRUE || Pitch != GraphImage->Pitch )
		{
			BYTE *Src, *Dest ;

			// 変換したグラフィックデータを格納するバッファの確保
			*GraphData = DXALLOC( ( size_t )( GraphImage->Height * Pitch ) ) ;
			if( *GraphData == NULL ) return -1 ;

			// データのコピー
			Src  = (BYTE *)GraphImage->GraphData ;
			Dest = (BYTE *)*GraphData ; 
			for( i = 0 ; i < GraphImage->Height ; i ++, Src += GraphImage->Pitch, Dest += Pitch )
			{
				_MEMCPY( Dest, Src, ( size_t )GraphImage->Width ) ;
			}
		 							
			// コピーフラグを立てる
			CopyFlag = TRUE ;
		}
		else
		// それ以外の場合はアドレスをそのままセットする
		{
			// コピーをしない場合は、グラフィックデータのポインタを GraphData に代入して終わり
			*GraphData = GraphImage->GraphData ;
		}
	}
	else
	// それ以外の場合
	{
		// ２４ビットカラーの BITMAPINFOHEADER の情報を詰める
		_MEMSET( BmpInfo, 0, sizeof( *BmpInfo ) ) ;
		BmpInfo->bmiHeader.biSize	= sizeof( BITMAPINFOHEADER ) ;
		BmpInfo->bmiHeader.biWidth	= GraphImage->Width ;
		BmpInfo->bmiHeader.biHeight	= -GraphImage->Height ;
		BmpInfo->bmiHeader.biPlanes	= 1 ;
		BmpInfo->bmiHeader.biBitCount= 24 ;

		// 変換後のピッチの算出
		SrcColor = &GraphImage->ColorData ;
		Pitch = GraphImage->Width * 3 ;
		Pitch = ( ( Pitch + 3 ) / 4 ) * 4 ;

		// カラーフォーマットが２４ビットカラー以外だった場合か、コピーフラグが立っている場合は２４ビットカラーに変換する
		SrcColor = &GraphImage->ColorData ;
		if( SrcColor->ColorBitDepth != 24 || CopyFlag == TRUE || 
			SrcColor->RedMask != 0xff0000 || SrcColor->GreenMask != 0xff00 || SrcColor->BlueMask != 0xff )
		{
			COLORDATA SrcColor_ ;
			POINT DestPoint ;
			RECT SrcRect ;
		
			// 変換したグラフィックデータを格納するバッファの確保
			*GraphData = DXALLOC( ( size_t )( GraphImage->Height * Pitch ) ) ;
			if( *GraphData == NULL ) return -1 ;
			
			// 変換先のフォーマットをセット
			NS_CreateFullColorData( &SrcColor_ ) ;
			
			// 変換処理を行う
			DestPoint.y = DestPoint.x = 0 ;
			SrcRect.left = SrcRect.top = 0 ;
			SrcRect.right = GraphImage->Width ; SrcRect.bottom = GraphImage->Height ;
			NS_GraphColorMatchBltVer2( *GraphData, Pitch, &SrcColor_, 
										GraphImage->GraphData, GraphImage->Pitch, &GraphImage->ColorData,
		 								NULL, 0, NULL,
		 								DestPoint, &SrcRect, FALSE,
		 								FALSE, 0,
		 								DX_SHAVEDMODE_NONE ) ;
		 							
			// コピーフラグを立てる
			CopyFlag = TRUE ;
		}
		else
		// コピーをしない場合は、ピッチがあっていない場合のみ簡易転送処理を行う
		if( Pitch != GraphImage->Pitch )
		{
			// コピーフラグを立てる
			CopyFlag = TRUE ;
			
			// 変換したグラフィックデータを格納するバッファの確保
			*GraphData = DXALLOC( ( size_t )( GraphImage->Height * Pitch ) ) ;
			if( *GraphData == NULL ) return -1 ;
			
			// 転送処理
			{
				int j ;
				BYTE *Src, *Dest ;
				
				Dest = ( BYTE * )*GraphData ;
				Src = ( BYTE * )GraphImage->GraphData ;
				for( j = 0 ; j < GraphImage->Height ; j ++, Dest += Pitch, Src += GraphImage->Pitch )
				{
					_MEMCPY( Dest, Src, ( size_t )GraphImage->Pitch ) ;
				}
			}
		}
		else
		// ピッチもあっている場合は変換処理を行う必要は無い
		{
			// コピーをしない場合は、グラフィックデータのポインタを GraphData に代入して終わり
			*GraphData = GraphImage->GraphData ;
		}
	}

	// 終了
	return CopyFlag ;
}

#endif // __WINDOWS__










// ユーザー定義のグラフィックロード関数Ver4を登録する
extern int NS_AddUserGraphLoadFunction4( int ( *UserLoadFunc )( STREAMDATA *Src, BASEIMAGE *BaseImage ) )
{
	int i ;

	// 同じ関数が登録されていないか調べる
	for( i = 0 ; i < MAX_USERIMAGEREAD_FUNCNUM ; i ++ )
	{
		if( BASEIM.UserImageLoadFunc4[ i ] == UserLoadFunc ) break ; 
	}
	if( i != MAX_USERIMAGEREAD_FUNCNUM ) return -1 ;

	// 使われていないポインタ変数を探す
	for( i = 0 ; i < MAX_USERIMAGEREAD_FUNCNUM ; i ++ )
	{
		if( BASEIM.UserImageLoadFunc4[ i ] == NULL ) break ; 
	}
	if( i == MAX_USERIMAGEREAD_FUNCNUM ) return -1 ;

	// 登録
	BASEIM.UserImageLoadFunc4[ i ] = UserLoadFunc ;

	// ローダーの数を増やす
	BASEIM.UserImageLoadFuncNum4 ++ ;

	// 終了
	return 0 ;
}

// ユーザー定義のグラフィックロード関数Ver4を登録から抹消する
extern int NS_SubUserGraphLoadFunction4( int ( *UserLoadFunc )( STREAMDATA *Src, BASEIMAGE *BaseImage ) )
{
	int i ;

	// 指定のグラフィックロード関数を探す
	for( i = 0 ; i < MAX_USERIMAGEREAD_FUNCNUM ; i ++ )
	{
		if( BASEIM.UserImageLoadFunc4[ i ] == UserLoadFunc ) break ;
	}
	if( i == MAX_USERIMAGEREAD_FUNCNUM ) return -1 ;

	// データを抹消
	BASEIM.UserImageLoadFunc4[ i ] = NULL ;
	
	// データの数を減らす
	BASEIM.UserImageLoadFuncNum4 -- ;

	// 終了
	return 0 ;
}

// 32bit bmp 画像のＡ成分がすべて０だったらＡ成分を無視するかどうかのフラグをセットする
extern int SetBmp32AllZeroAlphaToXRGB8( int Flag )
{
	// フラグを保存する
	BASEIM.Bmp32AllZeroAlphaToXRGB8 = Flag ;

	// 終了
	return 0 ;
}










// 標準画像読み込み関数

// ＢＭＰ画像の読みこみ
static int LoadBmpImage( STREAMDATA *Stream, BASEIMAGE *BaseImage, int GetFormatOnly )
{
	DWORD_PTR			sp ;
	BYTE				*GData = NULL ;
	BYTE				*Buf = NULL, *DecodeBuf = NULL ;
	STREAMDATASHRED		*sstr ;
	int					Pitch = 0, DataSize ;
	BITMAPFILEHEADER	BmpFileHeader ;
	BITMAPINFO			BmpInfoT, *BmpInfo2 = NULL ;
#ifdef __WINDOWS__
	HDC					hdc = NULL ;
	HBITMAP				Bmp = NULL ;
#endif // __WINDOWS__
//	bool				Palette ;
	BYTE 				*Dest, *DestT, *Src ;
	WORD				Num ;
	int					DestAddPitch, i, Height ;
	int					Loop, Loop2 ;
	size_t				FileSize, ImageSize ;

	sstr = &Stream->ReadShred ;
	sp   = Stream->DataPoint ;

	// ファイルサイズを得る
	sstr->Seek( sp, 0, STREAM_SEEKTYPE_END ) ;
	FileSize = ( size_t )sstr->Tell( sp ) ;
	sstr->Seek( sp, 0, STREAM_SEEKTYPE_SET ) ;
	
	if( sstr->Read( &BmpFileHeader, sizeof( BmpFileHeader ), 1, sp ) <= 0 ) goto ERR ;		// ファイルヘッダー部分を読みこむ
	if( _MEMCMP( &BmpFileHeader.bfType, "BM", 2 ) ) goto ERR ;								// ＩＤ検査
	if( sstr->Read( &BmpInfoT, sizeof( BITMAPINFO ), 1, sp ) <= 0 ) goto ERR ;				// BITMAPINFOを読みこむ

	// カラービット数が８以下の時はパレットを読む
	if( BmpInfoT.bmiHeader.biBitCount <= 8 )
	{
		// ＢＭＰＩＮＦＯ構造体の格納用メモリを確保
		if( ( BmpInfo2 = ( BITMAPINFO * )DXCALLOC( sizeof( BITMAPINFO ) + 256 * sizeof( RGBQUAD ) ) ) == NULL ) goto ERR ;

		// ＢＭＰＩＮＦＯ構造体の内容をコピー
		_MEMCPY( BmpInfo2, &BmpInfoT, sizeof( BITMAPINFO ) ) ;

		// 残りのカラーパレットの読みこみ
		if( ( sstr->Read( ( ( BYTE * )BmpInfo2 ) + sizeof( BITMAPINFO ), 1, ( ( 1 << BmpInfoT.bmiHeader.biBitCount ) - 1 ) * sizeof( RGBQUAD ), sp ) ) <= 0 ) goto ERR ;
	}
	else
	// カラービット数が３２か１６でカラーマスク使用時の処理
	if( BmpInfoT.bmiHeader.biBitCount == 16 || BmpInfoT.bmiHeader.biBitCount == 32 || BmpInfoT.bmiHeader.biCompression == BI_BITFIELDS )
	{
		// ＢＭＰＩＮＦＯ構造体の格納用メモリを確保
		if( ( BmpInfo2 = ( BITMAPINFO * )DXCALLOC( sizeof( BITMAPINFOHEADER ) + sizeof( RGBQUAD ) * 3 ) ) == NULL ) goto ERR ;

		// ＢＭＰＩＮＦＯＨＥＡＤＥＲ構造体の内容をコピー
		_MEMCPY( BmpInfo2, &BmpInfoT, sizeof( BITMAPINFO ) ) ;

		// カラーマスクの読みこみ
		if( BmpInfoT.bmiHeader.biCompression == BI_BITFIELDS )
		{
			if( ( sstr->Read( ( ( BYTE * )BmpInfo2 ) + sizeof( BITMAPINFO ), sizeof( RGBQUAD ) * 2, 1, sp ) ) <= 0 ) goto ERR ;
		}
	}
	else
	// それ以外の場合の処理
	{
		// ＢＭＰＩＮＦＯ構造体の格納用メモリを確保
		if( ( BmpInfo2 = ( BITMAPINFO * )DXCALLOC( sizeof( BITMAPINFO ) ) ) == NULL ) goto ERR ;

		// ＢＭＰＩＮＦＯＨＥＡＤＥＲ構造体の内容をコピー
		_MEMCPY( BmpInfo2, &BmpInfoT, sizeof( BITMAPINFO ) ) ;
	}

	// ファイルポインタを操作
	sstr->Seek( sp, BmpFileHeader.bfOffBits, STREAM_SEEKTYPE_SET ) ;

	// イメージサイズを計算する
	{
		Pitch = BmpInfoT.bmiHeader.biWidth * BmpInfoT.bmiHeader.biBitCount / 8 ;
		if( ( BmpInfoT.bmiHeader.biWidth * BmpInfoT.bmiHeader.biBitCount ) % 8 != 0 ) Pitch ++ ;
		Pitch = ( Pitch + 3 ) / 4 * 4 ;
		DataSize = Pitch * _ABS( BmpInfoT.bmiHeader.biHeight ) ;
	}

	// ファイルの残りの部分をメモリに読み込む
	if( GetFormatOnly == FALSE )
	{
		ImageSize = ( size_t )( FileSize - sstr->Tell( sp ) ) ;
		
		// 残りの部分を格納するメモリ領域の確保
		GData = (BYTE *)DXALLOC( ImageSize ) ;
		if( GData == NULL ) goto ERR ;

		// 読み込み
		if( sstr->Read( GData, 1, ImageSize, sp ) <= 0 ) goto ERR ;
	}

	// 形式によって処理を分岐
	if( BmpInfo2->bmiHeader.biCompression == BI_RLE8 )
	{
		// ８ビットパレット圧縮形式

		if( GetFormatOnly == FALSE )
		{
			// デコード後のデータを格納するメモリ領域の確保
			DecodeBuf = (BYTE *)DXALLOC( ( size_t )DataSize ) ;	
			if( DecodeBuf == NULL ) goto ERR ;
			_MEMSET( DecodeBuf, 0, ( size_t )DataSize ) ;
			
			// 解凍準備
			if( BmpInfo2->bmiHeader.biHeight < 0 )
			{
				Dest         = DecodeBuf ;
				DestAddPitch = Pitch ;
			}
			else
			{
				Dest         = DecodeBuf + Pitch * ( BmpInfo2->bmiHeader.biHeight - 1 ) ;
				DestAddPitch = -Pitch ;
			}
			Src = GData ;
			
			// 解凍開始
			DestT = Dest ;
			for(;;)
			{
				// １バイト目のコードによって処理を分岐
				if( Src[0] != 0 )
				{
					// 連続展開モード

					// ２バイト目の値が１バイト目の数値分だけ展開される
					_MEMSET( DestT, Src[1], Src[0] ) ;

					// 座標をずらす
					DestT += Src[0] ;
				}
				else
				{
					// コード化モード

					// エスケープコードによって処理を分岐
					switch( Src[1] )
					{
					case 0 :	// ０だったら行の終端、アドレスを変更する
						Dest += DestAddPitch ;
						DestT = Dest ;
						break ;

					case 1 :	// １だったらイメージデータの終端
						goto RLE8END ;

					case 2 :	// ２だったら現在のアドレスを変更する
						Dest  += *((char *)&Src[3]) * DestAddPitch ;
						DestT  = Dest + *((char *)&Src[2]) ;
						Src   += 2 ;
						break ;

					default :
						// それ以外の場合は非圧縮展開
						_MEMCPY( DestT, Src + 2, Src[1] ) ;

						// アドレスを変更する
						DestT += Src[1] ;
						Src   += Src[1] ;
						Src    = (BYTE *)( ( (DWORD_PTR)Src + 1 ) / 2 * 2 ) ;	// Src のアドレスは必ず２の倍数
						break ;
					}
				}
				
				Src += 2 ;
			}
	RLE8END:;

			DXFREE( GData ) ;
			GData = NULL ;
		}
		
		// BASEIMAGE のヘッダを構築
		{
			BaseImage->GraphData = GetFormatOnly == FALSE ? DecodeBuf : NULL ;
			BaseImage->Width     = BmpInfo2->bmiHeader.biWidth ;
			BaseImage->Height    = _ABS( BmpInfo2->bmiHeader.biHeight ) ;
			BaseImage->Pitch     = Pitch ;

			// カラー情報のセット
			NS_CreatePaletteColorData( &BaseImage->ColorData ) ;
			_MEMCPY( BaseImage->ColorData.Palette, BmpInfo2->bmiColors, 256 * sizeof( RGBQUAD ) ) ;
			for( i = 0 ; i < 256 ; i ++ )
				BaseImage->ColorData.Palette[i].Alpha = 0 ;
		}
	}
	else
	if( BmpInfo2->bmiHeader.biCompression == BI_RLE4 )
	{
		// ４ビットパレット圧縮形式
		// ８ビットパレット形式に変換しながら解凍

		Pitch    = ( BmpInfo2->bmiHeader.biWidth + 3 ) / 4 * 4 ;
		DataSize = Pitch * _ABS( BmpInfo2->bmiHeader.biHeight ) ;

		if( GetFormatOnly == FALSE )
		{
			// デコード後のデータを格納するメモリ領域の確保
			DecodeBuf = (BYTE *)DXALLOC( ( size_t )DataSize ) ;	
			if( DecodeBuf == NULL ) goto ERR ;
			_MEMSET( DecodeBuf, 0, ( size_t )DataSize ) ;

			// 解凍準備
			if( BmpInfo2->bmiHeader.biHeight < 0 )
			{
				Dest         = DecodeBuf ;
				DestAddPitch = Pitch ;
			}
			else
			{
				Dest         = DecodeBuf + Pitch * ( BmpInfo2->bmiHeader.biHeight - 1 ) ;
				DestAddPitch = -Pitch ;
			}
			Src = GData ;
			
			// 解凍開始
			DestT = Dest ;
			for(;;)
			{
				// １バイト目のコードによって処理を分岐
				if( Src[0] != 0 )
				{
					// 連続展開モード

					// ２バイト目の値(２ピクセル分(上位4bitが１ピクセル目))が
					// １バイト目の数分だけ展開される(ピクセル単位)
					// つまり 07 13 とあったら 01 03 01 03 01 03 01 と展開される 
					Loop = Src[0] ;
					Num  = ( WORD )( ( ( Src[1] & 0xf0 ) >> 4 ) | ( ( Src[1] & 0x0f ) << 8 ) ) ;
					
					Loop2 = Loop / 2 ;
					if( Loop2 != 0 )
					{
						do{
							*((WORD *)DestT) = Num ;
							DestT += 2 ;
						}while( -- Loop2 ) ;
					}
					
					if( Loop % 2 != 0 )
					{
						DestT[0]   = (BYTE)( Num & 0xff ) ;
						DestT ++ ;
					}
				}
				else
				{
					// コード化モード

					// エスケープコードによって処理を分岐
					switch( Src[1] )
					{
					case 0 :
						// ０だったら行の終端、アドレスを変更する
						Dest  += DestAddPitch ;
						DestT  = Dest ;
						break ;

					case 1 :
						// １だったらイメージデータの終端
						goto RLE4END ;

					case 2 :
						// ２だったら現在のアドレスを変更する
						Dest  += *((char *)&Src[3]) * DestAddPitch ;
						DestT  = Dest + *((char *)&Src[2]) ;
						Src   += 2 ;
						break ;

					default :
						// それ以外の場合は非圧縮展開
						Loop = Src[1] ;
						Src += 2 ;
						Loop2 = Loop / 2 ;
						for( i = 0 ; i < Loop2 ; i ++, DestT += 2, Src ++ )
						{
							DestT[0] = ( BYTE )( ( *Src & 0xf0 ) >> 4 ) ;
							DestT[1] = ( BYTE )( *Src & 0x0f ) ;
						}
						
						if( Loop % 2 != 0 )
						{
							*DestT = ( BYTE )( ( *Src & 0xf0 ) >> 4 ) ;
							Src ++ ;
							DestT ++ ;
						}

						Src = (BYTE *)( ( (DWORD_PTR)Src + 1 ) / 2 * 2 ) ;
						Src -= 2 ;
						break ;
					}
				}
				Src += 2 ;
			}
	RLE4END :;

			DXFREE( GData ) ;
			GData = NULL ;
		}

		// BASEIMAGE のヘッダを構築
		{
			BaseImage->GraphData = DecodeBuf ;
			BaseImage->Width     = BmpInfo2->bmiHeader.biWidth ;
			BaseImage->Height    = _ABS( BmpInfo2->bmiHeader.biHeight ) ;
			BaseImage->Pitch     = Pitch ;

			// カラー情報のセット
			NS_CreatePaletteColorData( &BaseImage->ColorData ) ;
			_MEMCPY( BaseImage->ColorData.Palette, BmpInfo2->bmiColors, 16 * sizeof( RGBQUAD ) ) ;
			for( i = 0 ; i < 16 ; i ++ )
				BaseImage->ColorData.Palette[i].Alpha = 0 ;
		}
	}
	else
	if( BmpInfo2->bmiHeader.biCompression == BI_RGB && BmpInfo2->bmiHeader.biBitCount == 1 )
	{
		// ２色カラーの場合は２５６色カラーに変換する
		int nt, qt, NokoriNum, MultiNum, Width, DestPitch ;
		int SrcAddPitch ;

		Width     = BmpInfo2->bmiHeader.biWidth ;
		Height    = _ABS( BmpInfo2->bmiHeader.biHeight ) ;
		DestPitch = ( Width + 3 ) / 4 * 4 ;

		if( GetFormatOnly == FALSE )
		{
			Buf = (BYTE *)DXALLOC( ( size_t )( DestPitch * Height ) ) ;
			if( Buf == NULL ) goto ERR ;

			MultiNum  = Width / 8 ;
			NokoriNum = Width - MultiNum * 8 ;

			// 上下反転が必要な場合はその処理もここでする
			if( BmpInfo2->bmiHeader.biHeight > 0 )
			{
				// 上下反転する場合
				Src         = (BYTE *)GData + Pitch * ( Height - 1 ) ;
				SrcAddPitch = ( Pitch - Width / 8 ) - Pitch * 2 ;
			}
			else
			{
				// 上下反転しない場合
				SrcAddPitch = Pitch - Width / 8 ;
				Src         = (BYTE *)GData ;
			}
			DestAddPitch = DestPitch - Width ;
			Dest         = Buf ;

			do
			{
				if( MultiNum )
				{
					qt = MultiNum ;
					do
					{
						Dest[ 0 ] = ( BYTE )( ( *Src >> 7 ) & 0x01 ) ;
						Dest[ 1 ] = ( BYTE )( ( *Src >> 6 ) & 0x01 ) ;
						Dest[ 2 ] = ( BYTE )( ( *Src >> 5 ) & 0x01 ) ;
						Dest[ 3 ] = ( BYTE )( ( *Src >> 4 ) & 0x01 ) ;
						Dest[ 4 ] = ( BYTE )( ( *Src >> 3 ) & 0x01 ) ;
						Dest[ 5 ] = ( BYTE )( ( *Src >> 2 ) & 0x01 ) ;
						Dest[ 6 ] = ( BYTE )( ( *Src >> 1 ) & 0x01 ) ;
						Dest[ 7 ] = ( BYTE )( ( *Src      ) & 0x01 ) ;

						Dest += 8 ;
						Src  ++ ;
					}while( --qt ) ;
				}

				if( NokoriNum )
				{
					nt = NokoriNum ;
					for(;;)
					{
						*Dest = ( BYTE )( ( *Src >> 7 ) & 0x01 ) ;
						Dest ++ ;
						if( --nt == 0 ) break ;

						*Dest = ( BYTE )( ( *Src >> 6 ) & 0x01 ) ;
						Dest ++ ;
						if( --nt == 0 ) break ;

						*Dest = ( BYTE )( ( *Src >> 5 ) & 0x01 ) ;
						Dest ++ ;
						if( --nt == 0 ) break ;

						*Dest = ( BYTE )( ( *Src >> 4 ) & 0x01 ) ;
						Dest ++ ;
						if( --nt == 0 ) break ;

						*Dest = ( BYTE )( ( *Src >> 3 ) & 0x01 ) ;
						Dest ++ ;
						if( --nt == 0 ) break ;

						*Dest = ( BYTE )( ( *Src >> 2 ) & 0x01 ) ;
						Dest ++ ;
						if( --nt == 0 ) break ;

						*Dest = ( BYTE )( ( *Src >> 1 ) & 0x01 ) ;
						Dest ++ ;
						if( --nt == 0 ) break ;

						*Dest = ( BYTE )(   *Src        & 0x01 ) ;
						Dest ++ ;
						Src  ++ ;
						if( --nt == 0 ) break ;
					}
				}

				Dest += DestAddPitch ;
				Src  += SrcAddPitch ;
			}while( --Height ) ;

			DXFREE( GData ) ;
			GData = Buf ;
			Buf   = NULL ;
		}

		// BASEIMAGE のヘッダを構築
		{
			BaseImage->GraphData = GData ;
			BaseImage->Width     = BmpInfo2->bmiHeader.biWidth ;
			BaseImage->Height    = _ABS( BmpInfo2->bmiHeader.biHeight ) ;
			BaseImage->Pitch     = DestPitch ;

			// カラー情報のセット
			NS_CreatePaletteColorData( &BaseImage->ColorData ) ;
			BaseImage->ColorData.MaxPaletteNo = 15 ;
			_MEMCPY( BaseImage->ColorData.Palette, BmpInfo2->bmiColors, 2 * sizeof( RGBQUAD ) ) ;
			BaseImage->ColorData.Palette[ 0 ].Alpha = 0 ;
			BaseImage->ColorData.Palette[ 1 ].Alpha = 0 ;
		}
	}
	else
	if( BmpInfo2->bmiHeader.biCompression == BI_RGB && BmpInfo2->bmiHeader.biBitCount == 4 )
	{
		// １６色カラーの場合は２５６色カラーに変換する
	
		int nt, qt, NokoriNum, QwordNum, Width, DestPitch ;
		int SrcAddPitch ;
		unsigned char Code[4] ;

		Width     = BmpInfo2->bmiHeader.biWidth ;
		Height    = _ABS( BmpInfo2->bmiHeader.biHeight ) ;
		DestPitch = ( Width + 3 ) / 4 * 4 ;

		if( GetFormatOnly == FALSE )
		{
			Buf = (BYTE *)DXALLOC( ( size_t )( DestPitch * Height ) ) ;
			if( Buf == NULL ) goto ERR ;

			QwordNum  = Width / 8 ;
			NokoriNum = Width - QwordNum * 8 ;

			// 上下反転が必要な場合はその処理もここでする
			if( BmpInfo2->bmiHeader.biHeight > 0 )
			{
				// 上下反転する場合
				Src         = (BYTE *)GData + Pitch * ( Height - 1 ) ;
				SrcAddPitch = ( Pitch - Width / 2 ) - Pitch * 2 ;
			}
			else
			{
				// 上下反転しない場合
				SrcAddPitch = Pitch - Width / 2 ;
				Src         = (BYTE *)GData ;
			}
			DestAddPitch = DestPitch - Width ;
			Dest         = Buf ;

			do
			{
				if( QwordNum )
				{
					qt = QwordNum ;
					do
					{
						*((DWORD *)&Code) = ( *((DWORD *)Src) & 0xf0f0f0f0 ) >> 4 ;
						Dest[0] = Code[0] ;
						Dest[2] = Code[1] ;
						Dest[4] = Code[2] ;
						Dest[6] = Code[3] ;

						*((DWORD *)&Code) = *((DWORD *)Src) & 0x0f0f0f0f ;
						Dest[1] = Code[0] ;
						Dest[3] = Code[1] ;
						Dest[5] = Code[2] ;
						Dest[7] = Code[3] ;

						Dest += 8 ;
						Src  += 4 ;
					}while( --qt ) ;
				}

				if( NokoriNum )
				{
					nt = NokoriNum ;
					for(;;)
					{
						*Dest = ( BYTE )( ( *Src & 0xf0 ) >> 4 ) ;
						Dest ++ ;
						if( --nt == 0 ) break ;

						*Dest = ( BYTE )( *Src & 0xf ) ;
						Dest ++ ;
						Src  ++ ;
						if( --nt == 0 ) break ;
					}
				}

				Dest += DestAddPitch ;
				Src += SrcAddPitch ;
			}while( --Height ) ;

			DXFREE( GData ) ;
			GData = Buf ;
			Buf   = NULL ;
		}

		// BASEIMAGE のヘッダを構築
		{
			BaseImage->GraphData = GData ;
			BaseImage->Width     = BmpInfo2->bmiHeader.biWidth ;
			BaseImage->Height    = _ABS( BmpInfo2->bmiHeader.biHeight ) ;
			BaseImage->Pitch     = DestPitch ;

			// カラー情報のセット
			NS_CreatePaletteColorData( &BaseImage->ColorData ) ;
			BaseImage->ColorData.MaxPaletteNo = 15 ;
			_MEMCPY( BaseImage->ColorData.Palette, BmpInfo2->bmiColors, 16 * sizeof( RGBQUAD ) ) ;
			for( i = 0 ; i < 16 ; i ++ )
				BaseImage->ColorData.Palette[i].Alpha = 0 ;
		}
	}
	else
	if( BmpInfo2->bmiHeader.biCompression == BI_RGB &&
		( BmpInfo2->bmiHeader.biBitCount == 32 ||  BmpInfo2->bmiHeader.biBitCount == 24 || BmpInfo2->bmiHeader.biBitCount == 8 ) )
	{
		// ３２ビットカラーＢＭＰか２４ビットフルカラーＢＭＰか８ビットパレットカラーの場合

		if( GetFormatOnly == FALSE )
		{
			// 上下反転している場合は上下反転を行う
			if( BmpInfo2->bmiHeader.biHeight > 0 )
			{
				Height = BmpInfo2->bmiHeader.biHeight ;
				Buf    = (BYTE *)DXALLOC( ( size_t )( Pitch * Height ) ) ;
				if( Buf == NULL ) goto ERR ;

				Dest = Buf ;
				Src  = (BYTE *)GData + Pitch * ( Height - 1 ) ;
				for( i = 0 ; i < Height ; i ++, Src -= Pitch, Dest += Pitch )
				{
					_MEMCPY( Dest, Src, ( size_t )Pitch ) ;
				}

				DXFREE( GData ) ;
				GData = Buf ;
				Buf   = NULL ;
			}
			else
			{
				Height = -BmpInfo2->bmiHeader.biHeight ;
			}
		}
		
		// BASEIMAGE のヘッダを構築
		{
			BaseImage->GraphData = GData ;
			BaseImage->Width     = BmpInfo2->bmiHeader.biWidth ;
			BaseImage->Height    = _ABS(BmpInfo2->bmiHeader.biHeight) ;
			BaseImage->Pitch     = Pitch ;
			
			if( BmpInfo2->bmiHeader.biBitCount == 8 )
			{
				// ８ビットカラーの場合
				
				// カラー情報のセット
				NS_CreatePaletteColorData( &BaseImage->ColorData ) ;
				_MEMCPY( BaseImage->ColorData.Palette, BmpInfo2->bmiColors, ( BmpInfo2->bmiHeader.biBitCount == 8 ? 256 : 16 ) * sizeof( RGBQUAD ) ) ;
				for( i = 0 ; i < 256 ; i ++ )
					BaseImage->ColorData.Palette[i].Alpha = 0 ;
			}
			else
			if( BmpInfo2->bmiHeader.biBitCount == 24 )
			{
				// ２４ビットカラーの場合
				
				// フルカラー用のカラー情報をセットする
				NS_CreateFullColorData( &BaseImage->ColorData ) ;
			}
			else
			{
				// ３２ビットカラーの場合
				
				// ３２ビットカラー用のカラー情報をセットする
				if( BASEIM.Bmp32AllZeroAlphaToXRGB8 )
				{
					// すべてのアルファ値が０だったらアルファなしと判断する
					DWORD PixelNum ;
					PixelNum = ( DWORD )( BmpInfo2->bmiHeader.biWidth * BaseImage->Height ) ;
					for( i = 0 ; ( DWORD )i < PixelNum ; i ++ )
						if( GData[ i * 4 + 3 ] != 0 ) break ;
					if( ( DWORD )i == PixelNum )
					{
						NS_CreateXRGB8ColorData( &BaseImage->ColorData ) ;
					}
					else
					{
						NS_CreateARGB8ColorData( &BaseImage->ColorData ) ;
					}
				}
				else
				{
					NS_CreateARGB8ColorData( &BaseImage->ColorData ) ;
				}
			}
		}
	}
	else
	{
		int Process = -1 ;

		// それ以外のフォーマットの場合は一部のカラーだけ対応
		if( ( ( BmpInfoT.bmiHeader.biBitCount == 32 || BmpInfoT.bmiHeader.biBitCount == 16 ) && BmpInfoT.bmiHeader.biCompression == BI_BITFIELDS ) ||
			( BmpInfoT.bmiHeader.biBitCount == 16 && BmpInfoT.bmiHeader.biCompression == BI_RGB ) )
		{
			int Format = -1 ;

			// R8G8B8X8 又は R5G6B5 又は X1R5G5B5
			if( BmpInfoT.bmiHeader.biBitCount == 16 && BmpInfoT.bmiHeader.biCompression == BI_RGB )
			{
				Format = 555 ;
			}
			else
			if( ( ( DWORD * )BmpInfo2->bmiColors )[ 0 ] == 0xff000000 &&
				( ( DWORD * )BmpInfo2->bmiColors )[ 1 ] == 0x00ff0000 &&
				( ( DWORD * )BmpInfo2->bmiColors )[ 2 ] == 0x0000ff00 )
			{
				Format = 888 ;
			}
			else
			if( ( ( DWORD * )BmpInfo2->bmiColors )[ 0 ] == 0x0000f800 &&
				( ( DWORD * )BmpInfo2->bmiColors )[ 1 ] == 0x000007e0 &&
				( ( DWORD * )BmpInfo2->bmiColors )[ 2 ] == 0x0000001f )
			{
				Format = 565 ;
			}
			else
			if( ( ( DWORD * )BmpInfo2->bmiColors )[ 0 ] == 0x000007c0 &&
				( ( DWORD * )BmpInfo2->bmiColors )[ 1 ] == 0x000003e0 &&
				( ( DWORD * )BmpInfo2->bmiColors )[ 2 ] == 0x0000001f )
			{
				Format = 555 ;
			}
			else
			{
				Format = 555 ;
			}

			if( Format != -1 )
			{
				if( GetFormatOnly == FALSE )
				{
					// 上下反転している場合は上下反転を行う
					if( BmpInfo2->bmiHeader.biHeight > 0 )
					{
						Height = BmpInfo2->bmiHeader.biHeight ;
						Buf    = (BYTE *)DXALLOC( ( size_t )( Pitch * Height ) ) ;
						if( Buf == NULL ) goto ERR ;

						Dest = Buf ;
						Src  = (BYTE *)GData + Pitch * ( Height - 1 ) ;
						for( i = 0 ; i < Height ; i ++, Src -= Pitch, Dest += Pitch )
						{
							_MEMCPY( Dest, Src, ( size_t )Pitch ) ;
						}

						DXFREE( GData ) ;
						GData = Buf ;
						Buf   = NULL ;
					}
					else
					{
						Height = -BmpInfo2->bmiHeader.biHeight ;
					}
				}

				// BASEIMAGE のヘッダを構築
				{
					BaseImage->GraphData = GData ;
					BaseImage->Width     = BmpInfo2->bmiHeader.biWidth ;
					BaseImage->Height    = _ABS(BmpInfo2->bmiHeader.biHeight) ;
					BaseImage->Pitch     = Pitch ;
			
					// カラー情報をセットする
					NS_CreateXRGB8ColorData( &BaseImage->ColorData ) ;
					switch( Format )
					{
					case 888 :
						BaseImage->ColorData.ColorBitDepth = 32 ;
						BaseImage->ColorData.PixelByte = 4 ;

						BaseImage->ColorData.NoneLoc = 0 ;
						BaseImage->ColorData.NoneMask = 0x000000ff ;
						BaseImage->ColorData.NoneWidth = 8 ;

						BaseImage->ColorData.RedLoc = 24 ;
						BaseImage->ColorData.RedMask = 0xff000000 ;
						BaseImage->ColorData.RedWidth = 8 ;

						BaseImage->ColorData.GreenLoc = 16 ;
						BaseImage->ColorData.GreenMask = 0x00ff0000 ;
						BaseImage->ColorData.GreenWidth = 8 ;

						BaseImage->ColorData.BlueLoc = 8  ;
						BaseImage->ColorData.BlueMask = 0x0000ff00 ;
						BaseImage->ColorData.BlueWidth = 8 ;
						break ;

					case 565 :
						BaseImage->ColorData.ColorBitDepth = 16 ;
						BaseImage->ColorData.PixelByte = 2 ;

						BaseImage->ColorData.NoneLoc = 0 ;
						BaseImage->ColorData.NoneMask = 0 ;
						BaseImage->ColorData.NoneWidth = 0 ;

						BaseImage->ColorData.RedLoc = 11 ;
						BaseImage->ColorData.RedMask = 0x0000f800 ;
						BaseImage->ColorData.RedWidth = 5 ;

						BaseImage->ColorData.GreenLoc = 5 ;
						BaseImage->ColorData.GreenMask = 0x000007e0 ;
						BaseImage->ColorData.GreenWidth = 6 ;

						BaseImage->ColorData.BlueLoc = 0  ;
						BaseImage->ColorData.BlueMask = 0x0000001f ;
						BaseImage->ColorData.BlueWidth = 5 ;
						break ;

					case 555 :
						BaseImage->ColorData.ColorBitDepth = 16 ;
						BaseImage->ColorData.PixelByte = 2 ;

						BaseImage->ColorData.NoneLoc = 15 ;
						BaseImage->ColorData.NoneMask = 0x00008000 ;
						BaseImage->ColorData.NoneWidth = 1 ;

						BaseImage->ColorData.RedLoc = 10 ;
						BaseImage->ColorData.RedMask = 0x00007c00 ;
						BaseImage->ColorData.RedWidth = 5 ;

						BaseImage->ColorData.GreenLoc = 5 ;
						BaseImage->ColorData.GreenMask = 0x000003e0 ;
						BaseImage->ColorData.GreenWidth = 5 ;

						BaseImage->ColorData.BlueLoc = 0  ;
						BaseImage->ColorData.BlueMask = 0x0000001f ;
						BaseImage->ColorData.BlueWidth = 5 ;
						break ;
					}
				}

				Process = 1 ;
			}
		}

		if( Process < 0 )
		{
#ifndef __WINDOWS__
			goto ERR ;

#else // __WINDOWS__

			// それ以外のフォーマットの場合は２４ビットカラーＢＭＰに変換する

			BITMAP bm ;
			HBITMAP OldBmp ;
			void *Data ;

			if( GetFormatOnly == FALSE )
			{
				SETUP_WIN_API

				// 変換処理に使用するＤＣの作成
				hdc = WinAPIData.Win32Func.CreateCompatibleDCFunc( NULL ) ;

				// ＤＩＢデータを作成する
				{
					BITMAPINFO BmpInfo3 ;

					// 変換後のビットマップインフォヘッダを設定
					_MEMSET( &BmpInfo3, 0, sizeof( BITMAPINFOHEADER ) ) ;
					BmpInfo3.bmiHeader.biSize        = sizeof( BITMAPINFOHEADER ) ;
					BmpInfo3.bmiHeader.biWidth       = BmpInfo2->bmiHeader.biWidth ;
					BmpInfo3.bmiHeader.biHeight      = -_ABS(BmpInfo2->bmiHeader.biHeight) ;
					BmpInfo3.bmiHeader.biPlanes      = 1 ;
					BmpInfo3.bmiHeader.biBitCount    = 24 ;
					BmpInfo3.bmiHeader.biCompression = BI_RGB ;

					Bmp = WinAPIData.Win32Func.CreateDIBSectionFunc( hdc, &BmpInfo3, DIB_RGB_COLORS, &Data, NULL, 0 ) ;
					if( Bmp == NULL || Data == NULL )
					{
						DXST_LOGFILE_ADDUTF16LE( "\x24\xff\x29\xff\x22\xff\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x33\x00\x0a\x00\x00"/*@ L"ＤＩＢの作成に失敗しました3\n" @*/ ) ;
						goto ERR ;
					}
				}

				// ビットマップの情報を取得
				WinAPIData.Win32Func.GetObjectAFunc( Bmp, sizeof( bm ), &bm ) ;
				bm.bmWidthBytes = ( bm.bmWidthBytes + 3 ) / 4 * 4 ;

				// グラフィックデータ２４ビットに変換しつつコピーする
				Height = _ABS( bm.bmHeight ) ;
				OldBmp = ( HBITMAP )WinAPIData.Win32Func.SelectObjectFunc( hdc, ( HGDIOBJ )Bmp ) ;
				WinAPIData.Win32Func.SetDIBitsToDeviceFunc(
					hdc,
					0, 0,  ( DWORD )BmpInfo2->bmiHeader.biWidth, ( DWORD )_ABS(BmpInfo2->bmiHeader.biHeight),
					0, 0,                                     0, ( UINT ) _ABS(BmpInfo2->bmiHeader.biHeight),
					GData, BmpInfo2, DIB_RGB_COLORS ) ;
				WinAPIData.Win32Func.SelectObjectFunc( hdc, ( HGDIOBJ )OldBmp ) ;

				// 不要になったＤＣの削除
				WinAPIData.Win32Func.DeleteDCFunc( hdc ) ;
				hdc = NULL ;
			
				// 変換前のデータを解放
				DXFREE( GData ) ;
				GData = NULL ;
			
				// ２４ビットカラー化したグラフィックを格納するメモリ領域の確保
				GData = (BYTE *)DXALLOC( ( size_t )( Height * bm.bmWidthBytes ) ) ;
				if( GData == NULL )
				{
					DXST_LOGFILE_ADDUTF16LE( "\x12\xff\x14\xff\xd3\x30\xc3\x30\xc8\x30\xab\x30\xe9\x30\xfc\x30\x16\x53\x57\x30\x5f\x30\xb0\x30\xe9\x30\xd5\x30\xa3\x30\xc3\x30\xaf\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"２４ビットカラー化したグラフィックを格納するメモリ領域の確保に失敗しました\n" @*/ ) ;	
					goto ERR ;
				}
			
				// ２４ビットカラー化したグラフィックのデータを専用の領域に転送
				_MEMCPY( GData, bm.bmBits, ( size_t )( Height * bm.bmWidthBytes ) ) ;
			
				// 変換用に作成したＤＩＢの削除
				WinAPIData.Win32Func.DeleteObjectFunc( Bmp ) ;
				Bmp = NULL ;

				BaseImage->Pitch     = bm.bmWidthBytes ;
			}
			else
			{
				BaseImage->Pitch     = 0 ;
			}
	
			// BASEIMAGE のヘッダを構築
			{
				// ヘッダ情報をセットする
				BaseImage->GraphData = GData ;
				BaseImage->Width     = BmpInfo2->bmiHeader.biWidth ;
				BaseImage->Height    = _ABS(BmpInfo2->bmiHeader.biHeight) ;

				// フルカラー用のカラー情報をセットする
				NS_CreateFullColorData( &BaseImage->ColorData ) ;
			}
#endif // __WINDOWS__
		}
	}

	// BITMAPINFOHEADER 情報を格納していたメモリを解放
	DXFREE( BmpInfo2 ) ;
	BmpInfo2 = NULL ;

	// 終了
	return 0 ;

ERR :
	if( BmpInfo2 != NULL ) DXFREE( BmpInfo2 ) ;
	if( GData != NULL ) DXFREE( GData ) ;
	if( Buf != NULL ) DXFREE( Buf ) ;
#ifdef __WINDOWS__
	SETUP_WIN_API

	if( Bmp != NULL ) WinAPIData.Win32Func.DeleteObjectFunc( Bmp ) ;
	if( hdc != NULL ) WinAPIData.Win32Func.DeleteDCFunc( hdc ) ;
#endif // __WINDOWS__

	// エラー終了
	return -1 ; 
}



// ピクセルの並び方を強く主張
typedef struct
{
    union
	{
        BYTE abyIdentifier[4] ;		// "BGRA"
        DWORD dwIdentifier ;		// 0x41524742
    };

	// 0x08080808(32bitColor) or 0x10101010(64bitColor)
    DWORD dwPixelFormat ;
    DWORD dwWidth ;
    DWORD dwHeight ;
} ArgbHeader ;


// ＡＲＧＢ画像の読みこみ
static int LoadArgbImage( STREAMDATA *Src, BASEIMAGE *BaseImage, int GetFormatOnly )
{
	ArgbHeader Head ;
	DWORD_PTR sp ;
	STREAMDATASHRED *sstr ;
	int P64Flag, PixelByte ;
	int Width, Height, Pitch ;
	void *GraphPoint = NULL ;
	BYTE *TempBuf = NULL, *SrcP ;

	sstr = &Src->ReadShred ;
	sp = Src->DataPoint ;

	if( sstr->Read( &Head, sizeof( Head ), 1, sp ) <= 0 ) return -1	;						// ヘッダの読み込み
	if( _MEMCMP( Head.abyIdentifier, "BGRA", 4 ) != 0 ) return -1 ;							// ＩＤ検査
	if( Head.dwPixelFormat != 0x10101010 && Head.dwPixelFormat != 0x08080808 ) return -1 ;	// ピクセルフォーマットの検査
	P64Flag = Head.dwPixelFormat == 0x10101010 ;

	// サイズの取得
	Width		= ( int )Head.dwWidth ;
	Height		= ( int )Head.dwHeight ;
	PixelByte	= P64Flag ? 8 : 4 ;
	Pitch		= Width * PixelByte ;

	if( GetFormatOnly == FALSE )
	{
		// グラフィックを格納するメモリ領域の確保
		GraphPoint = DXALLOC( ( size_t )( Width * Height * 4 ) ) ;
		if( GraphPoint == NULL )
		{
			goto ERR ;
		}
		
		// グラフィックの読み込み
		if( P64Flag == TRUE )
		{
			unsigned int *GraphP ;
			int i, j ;
		
			// ３２ビットカラーに変換しながら読み込み
			TempBuf = ( BYTE * )DXALLOC( ( size_t )Pitch ) ;
			if( TempBuf == NULL ) goto ERR ;
			
			GraphP = ( unsigned int * )GraphPoint ;
			for( i = 0 ; i < Height ; i ++ )
			{
				if( sstr->Read( TempBuf, ( size_t )Pitch, 1, sp ) <= 0 ) goto ERR ;

				SrcP = TempBuf ;
				for( j = 0 ; j < Width ; j ++, GraphP ++, SrcP += 8 )
				{
					*GraphP = ( unsigned int )( SrcP[1] | ( SrcP[3] << 8 ) | ( SrcP[5] << 16 ) | ( SrcP[7] << 24 ) ) ;
	//				*GraphP = ( unsigned int )(
	//							( ( *SrcP & 0xff00000000000000 ) >> 40 ) |
	//							( ( *SrcP & 0xff0000000000 ) >> 24 ) |
	//							( ( *SrcP & 0xff000000 ) >> 16 ) |
	//							( ( *SrcP & 0xff00 ) >> 8 ) ) ;  
				}
			}
			
			// テンポラリバッファを解放
			DXFREE( TempBuf ) ;
			TempBuf = NULL ;
		}
		else
		{
			// そのままメモリ領域に読み込み
			if( sstr->Read( GraphPoint, ( size_t )( Width * Height * 4 ), 1, sp ) <= 0 ) goto ERR ;
		}
	}

	// BASEIMAGE 構造体の情報を埋める
	{
		BaseImage->GraphData = GraphPoint ;
		BaseImage->Width = Width ;
		BaseImage->Height = Height ;
		BaseImage->Pitch = Width * 4 ;

		// カラー情報をセットする
		NS_CreateFullColorData( &BaseImage->ColorData ) ;
		BaseImage->ColorData.ColorBitDepth = 32 ;
		BaseImage->ColorData.PixelByte = 4 ;
		BaseImage->ColorData.AlphaLoc = 24 ;
		BaseImage->ColorData.AlphaMask = 0xff000000 ;
		BaseImage->ColorData.AlphaWidth = 8 ;
	}

	// 終了
	return 0 ;

ERR :
	if( GraphPoint != NULL ) DXFREE( GraphPoint ) ;
	if( TempBuf != NULL ) DXFREE( TempBuf ) ;

	return -1 ;
}







#ifndef DX_NON_TGA

// #ifndef DX_GCC_COMPILE
// #pragma pack(push)
// #pragma pack(1)
// #else
// #pragma pack(push,1)
// #endif
// 
// // ＴＧＡ画像のヘッダ
// typedef struct _tagTGAHEADER
// {
// 	BYTE ImageIDLength ;	// 0:イメージＩＤの長さ(バイト)
// 	BYTE UseColorMap ;		// 1:カラーマップの有無　0:無し  1:あり
// 	BYTE ImageType ;		// 2:画像形式 
// 							// 	  0:イメージ無し
// 							//    1:インデックスカラー( 256色 ) 非圧縮
// 							//	  2:ＲＧＢカラー 非圧縮
// 							//	  3:２階調カラー 非圧縮
// 							//	  9:インデックスカラー RLE圧縮
// 							//	  10:ＲＧＢカラー RLE圧縮
// 							//	  11:２階調カラー RLE圧縮
// 	WORD PaletteOrigin ;	// 3:カラーパレットの開始番号
// 	WORD PaletteLength ;	// 5:カラーパレットの個数
// 	BYTE PaletteBitDepth ;	// 7:カラーパレット一つのビット数( 16:16ビット  24:24ビット  32:32ビット )
// 	WORD XOrigin ;			// 8:画像の配置左上Ｘ座標( ほぼ0 ? )
// 	WORD YOrigin ;			// 10:画像の配置左上Ｙ座標( ほぼ0 ? )
// 	WORD Width ;			// 12:画像の幅
// 	WORD Height ;			// 14:画像の高さ
// 	BYTE ColorBitDepth ;	// 16:色深度( 16:16ビット  24:24ビット  32:32ビット )
// 	BYTE Descriptor ;		// 17:その他の情報
// 							//     bit 0〜3 属性( 16bit画像なら 0 又は 1, 24bit画像なら 0, 32bit画像なら 8 )
// 							//     bit 4 格納方向( 0:左から右  1:右から左 )
// 							//     bit 5 格納方向( 0:下から上  1:上から下 )
// 							//     bit 6, 7  未使用
// } _TGAHEADER ;
// 
// // ＴＧＡ画像のフッタ
// typedef struct _tagTGAFOOTER
// {
// 	DWORD ExtensionOffset ;		// 拡張情報のアドレス( 0:無い )
// 	DWORD DeveloperOffset ;		// 開発者領域(?)のアドレス( 0:無い )
// 	char Signature[18] ;		// フッタかどうかを調べるためのＩＤ( "TRUEVISION-XFILE" )
// } _TGAFOOTER ;
// 
// // ＴＧＡ画像のタグ
// typedef struct _tagTGATAG
// {
// 	WORD  TagNumber ;	// タグ番号
// 	DWORD DataOffset ;	// ＴＧＡの実データのあるアドレス
// 	DWORD DataSize ;	// ＴＧＡの実データのサイズ
// } _TGATAG ;
// 
// // ＴＧＡ画像の拡張情報
// typedef struct _tagTGAEXTENSION
// {
// 	WORD 		Size ;				// フッタ( 495固定 )
// 	BYTE 		Creator[41] ;		// 作者名( 未使用時は 0 で埋める )
// 	BYTE 		Comment[4][81] ;	// 作者名や時刻、そのほかの情報
// 	WORD		CreationDateMonth ;	// 作成日  月 1〜12
// 	WORD		CreationDateDay ;	// 作成日  日 1〜31
// 	WORD		CreationDateYear ;	// 作成日  年 4桁( 2004等 )
// 	WORD		CreationDateHour ;	// 作成日  時 0〜23
// 	WORD		CreationDateMinute ;// 作成日  分 0〜59
// 	WORD		CreationDateSecond ;// 作成日  秒 0〜59
// 	BYTE 		JobName[41] ;		// 仕事名( 未使用時は 0 で埋める )
// 	WORD		CreationTimeHours ;	// 作成時刻  時 0〜65535
// 	WORD		CreationTimeMinute ;// 作成時刻  分 0〜59
// 	WORD		CreationTimeSecond ;// 作成時刻  秒 0〜59
// 	BYTE		SoftID[41] ;		// ソフトウエアのＩＤ( 未使用時は 0 で埋める )
// 	WORD		VersionNumber ;		// バージョンナンバー( 1.01 だったら 101 )
// 	BYTE		VersionRot ;		// ロット( アスキーコード )
// 	DWORD		ColorKey ;			// カラーキー
// 	WORD		AcpectX ;			// ドットのアスペクト比 X (0:無効)
// 	WORD		AcpectY ;			// ドットのアスペクト比 Y (0:無効)
// 	WORD		GammaN ;			// ガンマ値 0.0 〜 10.0
// 	WORD		GammaD ;			// ガンマ値 ( 0:ガンマ指定無し )
// 	DWORD		ColorTableOffset ;	// カラーコレクションテーブルの位置 (0:省略)
// 	DWORD		StampTableOffset ;	// スタンプテーブルの位置 (0:省略)
// 	DWORD		ScanTableOffset ;	// スキャンテーブルの位置 (0:省略)
// 	BYTE		AttributeType ;		// アトリビュートタイプ
// 									//	0:αデータ無し
// 									//	1,2:αデータ無効
// 									//	3:α有効
// 									//	4:pre-multiplied なαデータ
// } _TGAEXTENSION ;
// 
// #ifndef DX_GCC_COMPILE
// #pragma pack(pop)
// #pragma pack()
// #else
// #pragma pack(pop)
// #endif

#define TGAHEADER_SIZE			(18)
#define TGAFOOTER_SIZE			(26)

// TGA Footer のシグネチャ "TRUEVISION-XFILE."
BYTE TGA_Footer_Signature[ 18 ] =
{
	0x54, 0x52, 0x55, 0x45, 0x56, 0x49, 0x53, 0x49,
	0x4F, 0x4E, 0x2D, 0x58, 0x46, 0x49, 0x4C, 0x45,
	0x2E, 0x00
} ;

// ＴＧＡ画像の読みこみ
static int LoadTargaImage( STREAMDATA *Src, BASEIMAGE *BaseImage, int GetFormatOnly )
{
//	_TGAHEADER head ;
//	_TGAEXTENSION ext ;
//	_TGAFOOTER foot ;
//	_TGATAG tag ;
	DWORD_PTR sp ;
	BYTE *data, *buf ;
	BYTE *temp, *palette ;
	STREAMDATASHRED *sstr ;
	int datasize, pixelbyte, pitch, usealpha, validfooter ;
	int rightleft, bottomtop, rle, pal, colordepth ;
	BYTE HeaderBuffer[ TGAHEADER_SIZE ] ;
	BYTE FooterBuffer[ TGAFOOTER_SIZE ] ;

	// TGAHEADER
 	BYTE ImageIDLength ;		// 0:イメージＩＤの長さ(バイト)
 	BYTE UseColorMap ;			// 1:カラーマップの有無　0:無し  1:あり
 	BYTE ImageType ;			// 2:画像形式 
 								// 	  0:イメージ無し
 								//    1:インデックスカラー( 256色 ) 非圧縮
 								//	  2:ＲＧＢカラー 非圧縮
 								//	  3:２階調カラー 非圧縮
 								//	  9:インデックスカラー RLE圧縮
 								//	  10:ＲＧＢカラー RLE圧縮
 								//	  11:２階調カラー RLE圧縮
 	WORD PaletteOrigin ;		// 3:カラーパレットの開始番号
 	WORD PaletteLength ;		// 5:カラーパレットの個数
 	BYTE PaletteBitDepth ;		// 7:カラーパレット一つのビット数( 16:16ビット  24:24ビット  32:32ビット )
 	WORD XOrigin ;				// 8:画像の配置左上Ｘ座標( ほぼ0 ? )
 	WORD YOrigin ;				// 10:画像の配置左上Ｙ座標( ほぼ0 ? )
 	WORD Width ;				// 12:画像の幅
 	WORD Height ;				// 14:画像の高さ
 	BYTE ColorBitDepth ;		// 16:色深度( 16:16ビット  24:24ビット  32:32ビット )
 	BYTE Descriptor ;			// 17:その他の情報
 								//     bit 0〜3 属性( 16bit画像なら 0 又は 1, 24bit画像なら 0, 32bit画像なら 8 )
 								//     bit 4 格納方向( 0:左から右  1:右から左 )
 								//     bit 5 格納方向( 0:下から上  1:上から下 )
 								//     bit 6, 7  未使用

	// TGAFOOTER
 	DWORD ExtensionOffset ;		// 拡張情報のアドレス( 0:無い )
 	DWORD DeveloperOffset ;		// 開発者領域(?)のアドレス( 0:無い )
// 	char Signature[ 18 ] ;		// フッタかどうかを調べるためのＩＤ( "TRUEVISION-XFILE" )
	
	usealpha = 1 ;
	data = NULL ;
	buf = NULL ;
	temp = NULL ;
	palette = NULL ;

	sstr = &Src->ReadShred ;
	sp = Src->DataPoint ;
	
	// フッタを読み込む
	{
		int FailedFlag = FALSE ;

		sstr->Seek( sp, -TGAFOOTER_SIZE, SEEK_END ) ;
		if( sstr->Read( FooterBuffer, TGAFOOTER_SIZE, 1, sp ) != 1 )
		{
			FailedFlag = TRUE ;
		}
		else
		if( _MEMCMP( TGA_Footer_Signature, &FooterBuffer[ 8 ], 18 ) != 0 )
		{
			FailedFlag = TRUE ;
		}
		else
		{
			READ_MEM_4BYTE( &ExtensionOffset, &FooterBuffer[ 0 ] ) ;
			READ_MEM_4BYTE( &DeveloperOffset, &FooterBuffer[ 4 ] ) ;
			if( ExtensionOffset != 0 )
			{
				FailedFlag = TRUE ;
			}
		}

		if( FailedFlag )
		{
			// フッターなし		
			validfooter = 0 ;
		
			// ファイルの先頭から読み込む
			sstr->Seek( sp, 0, SEEK_SET ) ;
	/*	
			// αの有無だけ見る
			sstr->Seek( sp, foot.ExtensionOffset, SEEK_SET ) ;
			if( sstr->Read( &ext, sizeof( ext ), 1, sp ) <= 0 ) return -1 ;
			usealpha = ext.AttributeType == 3 ? 3 : 0 ;
	
			// データのある位置を取得
			sstr->Seek( sp, foot.DeveloperOffset, SEEK_SET ) ;
			if( sstr->Read( &tag, sizeof( tag ), 1, sp ) <= 0 ) return -1 ;
		
			// 実データのある位置へ移動
			sstr->Seek( sp, tag.DataOffset, SEEK_SET ) ;
	*/
		}
		else
		{
			// フッターあり
			validfooter = 1 ;
		
			// ファイルの先頭から読み込む
			sstr->Seek( sp, 0, SEEK_SET ) ;
		}
	}

	// ヘッダを読み込む
	if( sstr->Read( HeaderBuffer, TGAHEADER_SIZE, 1, sp ) != 1 )
	{
		return -1 ;
	}
	READ_MEM_1BYTE( &ImageIDLength	, &HeaderBuffer[  0 ] ) ;
	READ_MEM_1BYTE( &UseColorMap	, &HeaderBuffer[  1 ] ) ;
	READ_MEM_1BYTE( &ImageType		, &HeaderBuffer[  2 ] ) ;
	READ_MEM_2BYTE( &PaletteOrigin	, &HeaderBuffer[  3 ] ) ;
	READ_MEM_2BYTE( &PaletteLength	, &HeaderBuffer[  5 ] ) ;
	READ_MEM_1BYTE( &PaletteBitDepth, &HeaderBuffer[  7 ] ) ;
	READ_MEM_2BYTE( &XOrigin		, &HeaderBuffer[  8 ] ) ;
	READ_MEM_2BYTE( &YOrigin		, &HeaderBuffer[ 10 ] ) ;
	READ_MEM_2BYTE( &Width			, &HeaderBuffer[ 12 ] ) ;
	READ_MEM_2BYTE( &Height			, &HeaderBuffer[ 14 ] ) ;
	READ_MEM_1BYTE( &ColorBitDepth	, &HeaderBuffer[ 16 ] ) ;
	READ_MEM_1BYTE( &Descriptor		, &HeaderBuffer[ 17 ] ) ;

	// IDのサイズ分だけ読み込み位置を変更する
	if( ImageIDLength != 0 )
	{
		sstr->Seek( sp, ImageIDLength, SEEK_CUR ) ;
	}
	
//	DXST_LOGFILEFMT_ADDW(( L"ImageIDLength:%d",		head.ImageIDLength		)) ;
//	DXST_LOGFILEFMT_ADDW(( L"ImageType:%d",			head.ImageType			)) ;
//	DXST_LOGFILEFMT_ADDW(( L"PaletteOrigin:%d",		head.PaletteOrigin		)) ;
//	DXST_LOGFILEFMT_ADDW(( L"PaletteLength:%d",		head.PaletteLength		)) ;
//	DXST_LOGFILEFMT_ADDW(( L"PaletteBitDepth:%d",	head.PaletteBitDepth	)) ;
//	DXST_LOGFILEFMT_ADDW(( L"XOrigin:%d",			head.XOrigin			)) ;
//	DXST_LOGFILEFMT_ADDW(( L"YOrigin:%d",			head.YOrigin			)) ;
//	DXST_LOGFILEFMT_ADDW(( L"Width:%d",				head.Width				)) ;
//	DXST_LOGFILEFMT_ADDW(( L"Height:%d",			head.Height				)) ;
//	DXST_LOGFILEFMT_ADDW(( L"ColorBitDepth:%d",		head.ColorBitDepth		)) ;
//	DXST_LOGFILEFMT_ADDW(( L"Descriptor:%x",		head.Descriptor			)) ;
//	DXST_LOGFILEFMT_ADDW(( L"ValidFooter:%d",		validfooter				)) ;

	// カラーパレットグラフィックを使用しているかどうか調べる
	pal = ImageType == 1 || ImageType == 9 ? 1 : 0 ;

	// 色深度を得る
	colordepth = pal == 1 ? PaletteBitDepth : ColorBitDepth ;
	
	// データがＴＧＡか調べる
	if( /*ImageIDLength != 0 ||*/
		( ImageType >= 4 && ImageType <= 8 ) ||
		ImageType	> 11 ||
		XOrigin		!= 0 ||
		YOrigin		!= 0 ||
		Width		> 32768 ||
		Height		> 32768 ||
		( colordepth != 16 && colordepth != 24 && colordepth != 32 ) )
	{
		return -1 ;
	}

	// 対応していない形式は弾く( ２階調形式と未知の形式 )
	if(	ImageType == 0 ||
		ImageType == 3 ||
		ImageType == 11 ||
		ImageType > 11 )
	{
		return -1 ;
	}

	// ランレングス圧縮データかどうか調べる
	rle = 0 ;
	switch( ImageType )
	{
	case 0 : rle = 0 ; break ;
	case 1 : rle = 0 ; break ;
	case 2 : rle = 0 ; break ;
	case 3 : rle = 0 ; break ;
	case 9 : rle = 1 ; break ;
	case 10 : rle = 1 ; break ;
	case 11 : rle = 1 ; break ;
	}
	
	// イメージの格納方向をセットする
	rightleft = ( Descriptor >> 4 ) & 1 ? 1 : 0 ;
	bottomtop = ( Descriptor >> 5 ) & 1 ? 0 : 1 ;

	// イメージのデータサイズを得る
	pixelbyte = colordepth / 8 ;
	pitch     = pixelbyte * Width ;
	datasize  = pitch * Height ;

	// α付きで 32 ビット以外だったらお手上げ
	if( ( ( usealpha & 1 ) == 1 && colordepth != 32 ) && 
		( ( usealpha & 2 ) == 2 && colordepth != 16 ) ) goto ERR ;

	// BASEIMAGE 構造体の情報を埋める
	{
		COLORDATA *cr ;
	
		BaseImage->Width = Width ;
		BaseImage->Height = Height ;
		BaseImage->Pitch = pitch ;
		
		cr                = &BaseImage->ColorData ;
		cr->ColorBitDepth = ( unsigned char )colordepth ;
		cr->PixelByte     = ( unsigned char )pixelbyte ;
		
		switch( colordepth )
		{
		case 16 :
			cr->RedWidth 	= 5 ;	cr->RedLoc   = 10 ;	cr->RedMask   = 0x7c00 ;
			cr->GreenWidth 	= 5 ;	cr->GreenLoc = 5  ;	cr->GreenMask = 0x03e0 ;
			cr->BlueWidth 	= 5 ;	cr->BlueLoc  = 0  ;	cr->BlueMask  = 0x001f ;

			if( ( usealpha & 2 ) == 2 )
			{
				cr->AlphaWidth 	= 1 ;	cr->AlphaLoc = 15 ;	cr->AlphaMask = 0x8000 ;
				cr->NoneMask	= 0 ;
			}
			else
			{
				cr->AlphaWidth 	= 0 ;	cr->AlphaLoc = 0 ;	cr->AlphaMask = 0 ;
				cr->NoneMask	= 0x8000 ;
			}
			break ;
			
		case 24 :
			cr->RedWidth 	= 8 ;	cr->RedLoc   = 16 ;	cr->RedMask   = 0xff0000 ;
			cr->GreenWidth 	= 8 ;	cr->GreenLoc = 8  ;	cr->GreenMask = 0x00ff00 ;
			cr->BlueWidth 	= 8 ;	cr->BlueLoc  = 0  ;	cr->BlueMask  = 0x0000ff ;
			cr->AlphaWidth 	= 0 ;	cr->AlphaLoc = 0  ;	cr->AlphaMask = 0x000000 ;
			cr->NoneMask	= 0 ;
			break ;
			
		case 32 :
			cr->RedWidth 	= 8 ;	cr->RedLoc   = 16 ;	cr->RedMask   = 0x00ff0000 ;
			cr->GreenWidth 	= 8 ;	cr->GreenLoc = 8  ;	cr->GreenMask = 0x0000ff00 ;
			cr->BlueWidth 	= 8 ;	cr->BlueLoc  = 0  ;	cr->BlueMask  = 0x000000ff ;

			if( ( usealpha & 1 ) == 1 )
			{
				cr->AlphaWidth 	= 8 ;	cr->AlphaLoc = 24 ;	cr->AlphaMask = 0xff000000 ;
				cr->NoneMask	= 0 ;
			}
			else
			{
				cr->AlphaWidth 	= 0 ;	cr->AlphaLoc = 0 ;	cr->AlphaMask = 0 ;
				cr->NoneMask	= 0xff000000 ;
			}
			break ;
		}
	}
	
	if( GetFormatOnly == FALSE )
	{
		// 画像データを格納するメモリ領域を確保
		data = ( BYTE * )DXALLOC( ( size_t )datasize ) ;
		if( data == NULL ) goto ERR ;
		
		// パレットグラフィックかどうかで処理を分岐
		if( pal == 1 )
		{
			// パレットカラーである場合
			
			// パレットを読み込む
			palette = ( BYTE * )DXALLOC( ( size_t )( 256 * pixelbyte ) ) ;
			if( palette == NULL ) goto ERR ;
			_MEMSET( palette, 0, ( size_t )( 256 * pixelbyte ) ) ;
			if( sstr->Read( palette + PaletteOrigin * pixelbyte, ( size_t )( PaletteLength * pixelbyte ), 1, sp ) <= 0 ) goto ERR ;
			
			// 画像が圧縮されているかどうかで処理を分岐
			if( rle == 0 )
			{
				// 非圧縮
				
				unsigned int destpixel ;
				BYTE *src ;

				// 全ピクセル数を得る
				destpixel = ( unsigned int )( Width * Height ) ;
				
				// データを読み込むメモリを確保する
				temp = ( BYTE * )DXALLOC( destpixel ) ;
				if( temp == NULL ) goto ERR ;
				
				// データを読み込む
				if( sstr->Read( temp, destpixel, 1, sp ) <= 0 ) goto ERR ;
				
				// パレットデータを元にデータを展開する
				src = temp ;
				switch( colordepth )
				{
				case 16 :
					{
						WORD *palW, *dest ;

						palW = ( WORD * )palette ;	
						dest = ( WORD * )data ;
						do
						{
							*dest++ = palW[*src++] ;
						}while( --destpixel ) ;
					}
					break ;

				case 24 :
					{
						BYTE *dest ;

						dest = data ;
						do
						{
							dest[0] = palette[( *src * 3 ) + 0] ;
							dest[1] = palette[( *src * 3 ) + 1] ;
							dest[2] = palette[( *src * 3 ) + 2] ;
							dest += 3 ;
							src ++ ;
						}while( --destpixel ) ;
					}
					break ;

				case 32 :
					{
						DWORD *palD, *dest ;

						palD = ( DWORD * )palette ;	
						dest = ( DWORD * )data ;
						do
						{
							*dest++ = palD[*src++] ;
						}while( --destpixel ) ;
					}
					break ;
				}
			}
			else
			{
				// 圧縮
				
				DWORD pos, size, destpixel ;
				int loopnum ;
				BYTE *src ;
				
				// 全ピクセル数を得る
				destpixel = ( DWORD )( Width * Height ) ;
				
				// 圧縮データを含むデータを節操無く読み込む
				pos = ( DWORD )sstr->Tell( sp ) ;
				sstr->Seek( sp, -TGAFOOTER_SIZE, SEEK_END ) ;
				size = ( DWORD )( sstr->Tell( sp ) - pos ) ;
				sstr->Seek( sp, pos, SEEK_SET ) ;
				temp = ( BYTE * )DXALLOC( size ) ;
				if( temp == NULL ) goto ERR ;
				if( sstr->Read( temp, size, 1, sp ) <= 0 ) goto ERR ;
				
				// パレットと圧縮データを元に展開する
				src = temp ;
				switch( colordepth )
				{
				case 16 :
					{
						WORD *palW, *dest ;
						
						palW = (WORD *)palette ;
						dest = (WORD *)data ;
						do
						{
							loopnum = ( *src & 0x7f ) + 1 ;	// ループ回数を得る( 下位 7bit がループ回数 - 1 )
							destpixel -= loopnum ;	// 処理するピクセル数を減算
							if( *src++ & 0x80 ) 	// 同じ色が続いているかどうかで処理を分岐( 8bit  1:同コード展開  0:通常展開 )
							{
								// 色コードを取得・展開
								WORD cr = palW[*src++] ;	
								do	*dest++ = cr ;	while( --loopnum ) ;
							}
							else
							{
								// 色コードを取得・展開
								do	*dest++ = palW[*src++] ;	while( --loopnum ) ;
							}
						}while( destpixel ) ;
					}
					break ;


				case 24 :
					{
						BYTE *dest ;
						
						dest = data ;
						do
						{
							loopnum = ( *src & 0x7f ) + 1 ;	// ループ回数を得る( 下位 7bit がループ回数 - 1 )
							destpixel -= loopnum ;	// 処理するピクセル数を減算
							if( *src++ & 0x80 ) 	// 同じ色が続いているかどうかで処理を分岐( 8bit  1:同コード展開  0:通常展開 )
							{
								// 色コードを取得・展開
								BYTE cr[ 3 ] ;
								cr[ 0 ] = palette[( *src * 3 ) + 0] ;
								cr[ 1 ] = palette[( *src * 3 ) + 1] ;
								cr[ 2 ] = palette[( *src * 3 ) + 2] ;
								do
								{
									dest[ 0 ] = cr[ 0 ] ;
									dest[ 1 ] = cr[ 1 ] ;
									dest[ 2 ] = cr[ 2 ] ;
									dest += 3 ;
								}while( --loopnum ) ;
								src ++ ;
							}
							else
							{
								// 色コードを取得・展開
								do
								{
									dest[0] = palette[( *src * 3 ) + 0] ;
									dest[1] = palette[( *src * 3 ) + 1] ;
									dest[2] = palette[( *src * 3 ) + 2] ;
									dest += 3 ;
									src ++ ;
								}while( --loopnum ) ;
							}
						}while( destpixel ) ;
					}
					break ;


				case 32 :
					{
						DWORD *palD, *dest ;
						
						palD = (DWORD *)palette ;
						dest = (DWORD *)data ;
						do
						{
							loopnum = ( *src & 0x7f ) + 1 ;	// ループ回数を得る( 下位 7bit がループ回数 - 1 )
							destpixel -= loopnum ;	// 処理するピクセル数を減算
							if( *src++ & 0x80 ) 	// 同じ色が続いているかどうかで処理を分岐( 8bit  1:同コード展開  0:通常展開 )
							{
								// 色コードを取得・展開
								DWORD cr = palD[*src++] ;	
								do	*dest++ = cr ;	while( --loopnum ) ;
							}
							else
							{
								// 色コードを取得・展開
								do	*dest++ = palD[*src++] ; while( --loopnum ) ;
							}
						}while( destpixel ) ;
					}
					break ;
				}
			}
			
			// パレット画像とパレット情報を解放する
			DXFREE( temp ) ;
			DXFREE( palette ) ;
			temp = NULL ;
			palette = NULL ;
		}
		else
		{
			// パレットカラーではない場合
			
			// 圧縮データかどうかによって処理を分岐
			if( rle == 0 )
			{
				// 非圧縮

				// 普通にデータを読み込む
				if( sstr->Read( data, ( size_t )datasize, 1, sp ) <= 0 ) goto ERR ;
			}
			else
			{
				// 圧縮
			
				BYTE *src, *dest ;
				int size, pos, destpixel, loopnum ;
				
				// 圧縮データを含むデータを節操無く読み込む
				pos = ( int )sstr->Tell( sp ) ;
				sstr->Seek( sp, validfooter ? -TGAFOOTER_SIZE : 0, SEEK_END ) ;
				size = ( int )( sstr->Tell( sp ) - pos ) ;
				sstr->Seek( sp, pos, SEEK_SET ) ;
				temp = ( BYTE * )DXALLOC( ( size_t )size ) ;
				if( temp == NULL ) goto ERR ;
				if( sstr->Read( temp, ( size_t )size, 1, sp ) <= 0 ) goto ERR ;
				
				// 展開処理
				dest      = data ;
				src       = temp ;
				destpixel = Width * Height ;
				
				// ビット深度によって処理を分岐
				switch( ColorBitDepth )
				{
				case 16 :
					do
					{
						loopnum    = ( GET_MEM_BYTE( src ) & 0x7f ) + 1 ;	// ループ回数を得る( 下位 7bit がループ回数 - 1 )
						destpixel -= loopnum ;	// 処理ピクセル数分減らす
						if( GET_MEM_BYTE( src ) & 0x80 )		// 同じ色が続いているかどうかで処理を分岐( 8bit  1:同コード展開  0:通常展開 )
						{
							src ++ ;
							do	// 色コードを取得・展開
							{
								READ_MEM_2BYTE( dest, src ) ;
								dest += 2 ;
							}while( --loopnum ) ;
							src += 2 ;
						}
						else
						{
							src ++ ;
							do	// 色コードを取得・展開
							{
								READ_MEM_2BYTE( dest, src ) ;
								dest += 2 ;
								src += 2 ;
							}while( --loopnum ) ;
						}
					}while( destpixel ) ;
					break ;
					
				case 24 :
					do
					{
						loopnum = ( GET_MEM_BYTE( src ) & 0x7f ) + 1 ;	// ループ回数を得る( 下位 7bit がループ回数 - 1 )
						destpixel -= loopnum ;	// 処理ピクセル数分減らす
						if( GET_MEM_BYTE( src ) & 0x80 )		// 同じ色が続いているかどうかで処理を分岐( 8bit  1:同コード展開  0:通常展開 )
						{
							src ++ ;
							do	// 色コードを取得・展開
							{
								READ_MEM_3BYTE( dest, src ) ;
								dest += 3 ;
							}while( --loopnum ) ;
							src += 3 ;
						}
						else
						{
							src ++ ;
							do	// 色コードを取得・展開
							{
								READ_MEM_3BYTE( dest, src ) ;
								dest += 3 ;
								src += 3 ;
							}while( --loopnum ) ;
						}
					}while( destpixel ) ;
					break ;
					
				case 32 :
					do
					{
						loopnum = ( GET_MEM_BYTE( src ) & 0x7f ) + 1 ;	// ループ回数を得る( 下位 7bit がループ回数 - 1 )
						destpixel -= loopnum ;	// 処理ピクセル数分減らす
						if( GET_MEM_BYTE( src ) & 0x80 )		// 同じ色が続いているかどうかで処理を分岐( 8bit  1:同コード展開  0:通常展開 )
						{
							src ++ ;
							do	// 色コードを取得・展開
							{
								READ_MEM_4BYTE( dest, src ) ;
								dest += 4 ;
							}while( --loopnum ) ;
							src += 4 ;
						}
						else
						{
							src ++ ;
							do	// 色コードを取得・展開
							{
								READ_MEM_4BYTE( dest, src ) ;
								dest += 4 ;
								src += 4 ;
							}while( --loopnum ) ;
						}
					}while( destpixel ) ;
					break ;
				}
				
				// 圧縮データを解放
				DXFREE( temp ) ;
				temp = NULL ;
			}
		}

		// データの格納方向がＤＸライブラリが望むものと同じ場合は此処で終了
		if( rightleft == 0 && bottomtop == 0 )
		{
			BaseImage->GraphData = data ;
		}
		else
		{
			// データの格納方向がＤＸライブラリが望むものと違う場合は置き換え処理を行う
			
			BYTE *dest, *src ;
			int i, j, addx, addy ;

			buf = ( BYTE * )DXALLOC( ( size_t )datasize ) ;
			if( buf == NULL ) goto ERR ;
			
			dest = buf ;
			if( rightleft )
			{
				dest += ( Width - 1 ) * pixelbyte ;
				addx = -pixelbyte ;
			}
			else
			{
				addx = pixelbyte ;
			}
			
			if( bottomtop )
			{
				dest += ( Height - 1 ) * pitch ;
				addy = rightleft ? 0 : -pitch * 2 ;
			}
			else
			{
				addy = rightleft ? 0 : pitch * 2 ;
			}
			
			src = data ;
			switch( colordepth )
			{
			case 16 :
				for( i = 0 ; i < Height ; i ++, dest += addy )
					for( j = 0 ; j < Width ; j ++, src += pixelbyte, dest += addx )
					{
						READ_MEM_2BYTE( dest, src ) ;
					}
				break ;

			case 24 :
				for( i = 0 ; i < Height ; i ++, dest += addy )
					for( j = 0 ; j < Width ; j ++, src += pixelbyte, dest += addx )
					{
						READ_MEM_3BYTE( dest, src ) ;
					}
				break ;
				
			case 32 :
				for( i = 0 ; i < Height ; i ++, dest += addy )
					for( j = 0 ; j < Width ; j ++, src += pixelbyte, dest += addx )
					{
						READ_MEM_4BYTE( dest, src ) ;
					}
				break ;
			}

			// イメージのポインタを返して終了
			BaseImage->GraphData = buf ;
			DXFREE( data ) ;
			data = NULL ;
		}
	}
	else
	{
		BaseImage->GraphData = NULL ;
	}
	
	return 0 ;
	
ERR :
	if( data    ) DXFREE( data ) ;
	if( buf     ) DXFREE( buf ) ;
	if( palette ) DXFREE( palette ) ;
	if( temp    ) DXFREE( temp ) ;

	return -1 ;
}

#endif






// カラーフォーマットテーブル
static DDSCOLORTABLE _DDSColorTable[] =
{
	{
		20,		// D3DFMT_R8G8B8
		FALSE,
		0,
		24,
		0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000
	} ,
	{
		21,		// D3DFMT_A8R8G8B8
		FALSE,
		0,
		32,
		0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000
	} ,
	{
		22,		// D3DFMT_X8R8G8B8
		FALSE,
		0,
		32,
		0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000
	} ,
	{
		23,		// D3DFMT_R5G6B5
		FALSE,
		0,
		16,
		0xf800, 0x07c0, 0x003f, 0x0000
	} ,
	{
		24,		// D3DFMT_X1R5G5B5
		FALSE,
		0,
		16,
		0x7c00, 0x03e0, 0x001f, 0x0000
	} ,
	{
		25,		// D3DFMT_A1R5G5B5
		FALSE,
		0,
		16,
		0x7c00, 0x03e0, 0x001f, 0x8000
	} ,
	{
		26,		// D3DFMT_A4R4G4B4
		FALSE,
		0,
		16,
		0x0f00, 0x00f0, 0x000f, 0xf000
	} ,
	{
		27,		// D3DFMT_R3G3B2
		FALSE,
		0,
		8,
		0xe0, 0x1c, 0x03, 0x00
	} ,
	{
		29,		// D3DFMT_A8R3G3B2
		FALSE,
		0,
		16,
		0x00e0, 0x001c, 0x0003, 0xff00
	} ,
	{
		30,		// D3DFMT_X4R4G4B4
		FALSE,
		0,
		16,
		0x0f00, 0x00f0, 0x000f, 0x0000
	} ,
	{
		31,		// D3DFMT_A2B10G10R10
		FALSE,
		0,
		32,
		0x000003ff, 0x000ffc00, 0x3ff00000, 0xc0000000
	} ,
	{
		32,		// D3DFMT_A8B8G8R8
		FALSE,
		0,
		32,
		0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000
	} ,
	{
		33,		// D3DFMT_X8B8G8R8
		FALSE,
		0,
		32,
		0x000000ff, 0x0000ff00, 0x00ff0000, 0x00000000
	} ,
	{
		34,		// D3DFMT_G16R16
		FALSE,
		2,
		16,
		0x00000000, 0x00000000, 0x00000000, 0x00000000
	} ,
	{
		35,		// D3DFMT_A2R10G10B10
		FALSE,
		0,
		32,
		0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000
	} ,
	{
		36,		// D3DFMT_A16B16G16R16
		FALSE,
		4,
		16,
		0x00000000, 0x00000000, 0x00000000, 0x00000000
	} ,
	{
		81,		// D3DFMT_L16
		FALSE,
		1,
		16,
		0x00000000, 0x00000000, 0x00000000, 0x00000000
	} ,

	{
		111,	// DXGI_FORMAT_R16_FLOAT
		TRUE,
		1,
		16,
		0x00000000, 0x00000000, 0x00000000, 0x00000000
	} ,
	{
		112,	// DXGI_FORMAT_R16G16_FLOAT
		TRUE,
		2,
		16,
		0x00000000, 0x00000000, 0x00000000, 0x00000000
	} ,
	{
		113,	// DXGI_FORMAT_R16G16B16A16_FLOAT
		TRUE,
		4,
		16,
		0x00000000, 0x00000000, 0x00000000, 0x00000000
	} ,

	{
		114,	// DXGI_FORMAT_R32_FLOAT
		TRUE,
		1,
		32,
		0x00000000, 0x00000000, 0x00000000, 0x00000000
	} ,
	{
		115,	// DXGI_FORMAT_R32G32_FLOAT
		TRUE,
		2,
		32,
		0x00000000, 0x00000000, 0x00000000, 0x00000000
	} ,
	{
		116,	// DXGI_FORMAT_R32G32B32A32_FLOAT
		TRUE,
		4,
		32,
		0x00000000, 0x00000000, 0x00000000, 0x00000000
	} ,

	{
		0xffffffff,		// 終端
		0xff,
		0xff,
		0xff,
		0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
	} ,
} ;

// ＤＤＳ画像の読みこみ
static int LoadDDSImage( STREAMDATA *Src, BASEIMAGE *BaseImage, int GetFormatOnly )
{
	DWORD_PTR       sp ;
	STREAMDATASHRED *sstr ;
	DDSFILEHEADER   head ;
	void            *data = NULL ;
	int             size ;
	int             pitch = 0 ;
	int             addsize ;
	int             i ;
	DWORD           headsize ;
	char            magic[ 4 ] ;
	int             dxtformat ;
	int             dxtsize ;
	int             dxtpacketsize ;
	int             mipcount ;
	int             imagenum ;

	sstr = &Src->ReadShred ;
	sp = Src->DataPoint ;
	data = NULL ;
	dxtformat = 0 ;

	// ＤＤＳ識別文字列のチェック
	if( sstr->Read( magic, 4, 1, sp ) <= 0 ) return -1 ;
	if( _MEMCMP( magic, "DDS", ( int )_STRLEN( "DDS" ) ) != 0 ) return -1 ;

	// ヘッダサイズを取得
	if( sstr->Read( &headsize, 4, 1, sp ) <= 0 ) return -1 ;

	// フォーマットの取得
	if( sstr->Read( &head.dwFlags, headsize - 4, 1, sp ) <= 0 ) return -1 ;

	// フォーマットの取得
	{
		int formatget ;
		
		formatget = 0 ;
	
		// DirectX8 以降のフォーマット定義を調べる
		if( head.dwPfFlags == BI_DDPF_FOURCC )
		{
			DDSCOLORTABLE *table ;
			
			for( 	table = _DDSColorTable ;
					table->Type != 0xffffffff && table->Type != head.dwFourCC ;
					table ++ ){}
			// 見つけたらフォーマットをセットする
			if( table->Type != 0xffffffff )
			{
				// Float タイプかどうかで処理を分岐
				if( table->IsFloat )
				{
					// Floatタイプ
					_MEMSET( &BaseImage->ColorData, 0, sizeof( BaseImage->ColorData ) ) ;
					BaseImage->ColorData.Format				= DX_BASEIMAGE_FORMAT_NORMAL ;
					BaseImage->ColorData.ChannelBitDepth	= ( unsigned char )table->BitDepth ;
					BaseImage->ColorData.ChannelNum			= ( unsigned char )table->ChannelNum ;
					BaseImage->ColorData.FloatTypeFlag		= ( unsigned char )TRUE ;
					BaseImage->ColorData.PixelByte			= ( unsigned char )( BaseImage->ColorData.ChannelBitDepth * BaseImage->ColorData.ChannelNum / 8 ) ;
				}
				else
				if( table->BitDepth == 16 && table->ChannelNum > 0 )
				{
					// 整数型の1チャンネル16ビットタイプ

					_MEMSET( &BaseImage->ColorData, 0, sizeof( BaseImage->ColorData ) ) ;
					BaseImage->ColorData.Format				= DX_BASEIMAGE_FORMAT_NORMAL ;
					BaseImage->ColorData.ChannelBitDepth	= ( unsigned char )table->BitDepth ;
					BaseImage->ColorData.ChannelNum			= ( unsigned char )table->ChannelNum ;
					BaseImage->ColorData.FloatTypeFlag		= ( unsigned char )FALSE ;
					BaseImage->ColorData.PixelByte			= ( unsigned char )( BaseImage->ColorData.ChannelBitDepth * BaseImage->ColorData.ChannelNum / 8 ) ;
				}
				else
				{
					// 整数値タイプ
					NS_CreateColorData( &BaseImage->ColorData, table->BitDepth,
										table->RedMask, table->GreenMask, table->BlueMask, table->AlphaMask ) ;
				}
				formatget = 1 ;
			}
		}
	
		// DXTフォーマットかどうかを調べる
		dxtpacketsize = 0 ;
		if( formatget == 0 && ( head.dwPfFlags & BI_DDPF_FOURCC ) != 0 )
		{
			_MEMSET( &BaseImage->ColorData, 0, sizeof( COLORDATA ) ) ;
			if( head.dwFourCC == MAKEFOURCC( 'D', 'X', 'T', '1' ) )
			{
				dxtformat = DX_BASEIMAGE_FORMAT_DXT1 ;
				dxtpacketsize = 8 ;
			}

			if( head.dwFourCC == MAKEFOURCC( 'D', 'X', 'T', '2' ) )
			{
				dxtformat = DX_BASEIMAGE_FORMAT_DXT2 ;
				dxtpacketsize = 16 ;
			}

			if( head.dwFourCC == MAKEFOURCC( 'D', 'X', 'T', '3' ) )
			{
				dxtformat = DX_BASEIMAGE_FORMAT_DXT3 ;
				dxtpacketsize = 16 ;
			}

			if( head.dwFourCC == MAKEFOURCC( 'D', 'X', 'T', '4' ) )
			{
				dxtformat = DX_BASEIMAGE_FORMAT_DXT4 ;
				dxtpacketsize = 16 ;
			}

			if( head.dwFourCC == MAKEFOURCC( 'D', 'X', 'T', '5' ) )
			{
				dxtformat = DX_BASEIMAGE_FORMAT_DXT5 ;
				dxtpacketsize = 16 ;
			}

			if( dxtformat != 0 )
			{
				formatget = 1 ;
				NS_CreateARGB8ColorData( &BaseImage->ColorData ) ; 
			}
		}

		if( formatget == 0 )
		{
			// ＲＧＢ以外のフォーマット、ＲＧＢ３つ揃ってないフォーマット、
			// 及びパレット画像は受け付けない
			if( ( ( head.dwPfFlags & ( BI_DDPF_RGB | BI_DDPF_ALPHAPIXELS ) ) == ( BI_DDPF_RGB | BI_DDPF_ALPHAPIXELS ) ) || 
				( ( head.dwPfFlags & BI_DDPF_RGB ) == BI_DDPF_RGB ) )
			{
				// カラーフォーマットのセット
				NS_CreateColorData( &BaseImage->ColorData, ( int )head.dwRGBBitCount,
									head.dwRBitMask, head.dwGBitMask, head.dwBBitMask, head.dwRGBAlphaBitMask ) ;
									
				formatget = 1 ;
			}
		}
		
		// フォーマットを見つけていなかったらここで終了
		if( formatget == 0 ) return -1 ;
	}

	// ミップマップの情報を取得する
	if( ( head.dwFlags & BI_DDSD_MIPMAPCOUNT ) && ( head.dwCaps & ( BI_DDSCAPS_MIPMAP | BI_DDSCAPS_COMPLEX ) ) == ( BI_DDSCAPS_MIPMAP | BI_DDSCAPS_COMPLEX ) )
	{
		mipcount = ( int )head.dwMipMapCount ;
	}
	else
	{
		mipcount = 0 ;
	}

	// キューブマップの情報を取得する
	if( ( head.dwCaps2 & BI_DDSCAPS2_CUBEMAP ) != 0 && ( head.dwCaps2 & BI_DDSCAPS2_CUBEMAP_ALLFACES ) == BI_DDSCAPS2_CUBEMAP_ALLFACES )
	{
		imagenum = CUBEMAP_SURFACE_NUM ;
	}
	else
	{
		imagenum = 1 ;
	}

	// ＤＸＴかどうかで処理を分岐
	if( dxtformat != 0 )
	{
		// 共通のカラーフォーマットをセット
		NS_CreateColorData( &BaseImage->ColorData, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000 ) ;

		// データの読み込み
		if( GetFormatOnly == FALSE )
		{
			DWORD TempWidth, TempHeight ;

			dxtsize = ( int )( ( ( head.dwWidth + 3 ) / 4 ) * ( ( head.dwHeight + 3 ) / 4 ) * dxtpacketsize ) ;
			if( mipcount > 1 )
			{
				TempWidth  = ( head.dwWidth  + 3 ) / 4 * 4 ;
				TempHeight = ( head.dwHeight + 3 ) / 4 * 4 ;
				dxtsize = 0 ;
				for( i = 0 ; i < mipcount ; i ++ )
				{
					addsize = ( int )( ( ( TempWidth + 3 ) / 4 ) * ( ( TempHeight + 3 ) / 4 ) * dxtpacketsize ) ;
					dxtsize += addsize ;

					TempWidth  /= 2 ;
					if( TempWidth == 0 )
					{
						TempWidth = 1 ;
					}
					TempHeight /= 2 ;
					if( TempHeight == 0 )
					{
						TempHeight = 1 ;
					}
				}
			}

			data = ( BYTE * )DXALLOC( ( size_t )( dxtsize * imagenum ) ) ;
			if( data == NULL ) return -1 ;
			if( sstr->Read( data, ( size_t )( dxtsize * imagenum ), 1, sp ) <= 0 )
			{
				DXFREE( data ) ;
				return -1 ;
			}
		}

		// 色ビット深度は１ピクセル辺りに必要なビット数
		BaseImage->ColorData.ColorBitDepth   = ( unsigned char )( dxtformat == DX_BASEIMAGE_FORMAT_DXT1 ? 4 : 8 ) ;
		BaseImage->ColorData.Format          = ( unsigned char )dxtformat ;
		BaseImage->ColorData.ChannelNum      = 0 ;
		BaseImage->ColorData.ChannelBitDepth = 0 ;
		BaseImage->ColorData.FloatTypeFlag   = FALSE ;
		pitch = 0 ;
	}
	else
	{
		// 通常の画像の場合

		if( GetFormatOnly == FALSE )
		{
			// ピッチの算出
			if( head.dwFlags & BI_DDSD_PITCH )
			{
				pitch = ( int )head.dwPitchOrLinearSize ;
			}
			else
			{
		//		DXST_LOGFILEFMT_ADDW(( L"bitcount:%d", pform->dwRGBBitCount )) ;
				pitch = ( int )( head.dwWidth * BaseImage->ColorData.PixelByte ) ;
			}
			
			// データの読み込み
			size = ( int )( head.dwHeight * pitch ) ;
			if( mipcount > 1 )
			{
				addsize = size ;
				size = 0 ;
				for( i = 0 ; i < mipcount ; i ++ )
				{
					size += addsize ;
					addsize /= 4 ;
				}
			}

			data = DXALLOC( ( size_t )( size * imagenum ) ) ;
			if( data == NULL )
			{
				return -1 ;
			}

			if( mipcount > 1 )
			{
				BYTE *datap ;
				DWORD Width, Height ;

				datap = ( BYTE * )data ;
				Width  = head.dwWidth ;
				Height = head.dwHeight ;
				for( i = 0 ; i < mipcount ; i ++ )
				{
					addsize = Width * Height * BaseImage->ColorData.PixelByte ;
					if( sstr->Read( datap, ( size_t )( addsize * imagenum ), 1, sp ) <= 0 )
					{
						if( i == 0 )
						{
							DXFREE( data ) ;
							return -1 ;
						}
						break ;
					}
					datap += addsize * imagenum ;

					Width  /= 2 ;
					if( Width == 0 )
					{
						Width = 1 ;
					}
					Height /= 2 ;
					if( Height == 0 )
					{
						Height = 1 ;
					}
				}
				mipcount = i ;
			}
			else
			{
				if( sstr->Read( data, ( size_t )( size * imagenum ), 1, sp ) <= 0 )
				{
					DXFREE( data ) ;
					return -1 ;
				}
			}
		}
	}

	// BASEIMAGE 構造体の情報を埋める
	{
		BaseImage->GraphData      = data ;
		BaseImage->Width          = ( int )head.dwWidth ;
		BaseImage->Height         = ( int )head.dwHeight ;
		BaseImage->Pitch          = pitch ;
		BaseImage->MipMapCount    = mipcount ;
		BaseImage->GraphDataCount = imagenum == 1 ? 0 : imagenum ; 
	}

	// 終了
	return 0 ;
}






















// 基本イメージデータ機能公開用関数

// 初期化、後始末

// 基本イメージ管理情報の初期化
extern int InitializeBaseImageManage( void )
{
	// 既に初期化されていたら何もしない
	if( BASEIM.InitializeFlag == TRUE )
	{
		return -1 ;
	}

	// 環境依存の初期化処理を行う
	if( InitializeBaseImageManage_PF() != 0 )
	{
		return -1 ;
	}

	// YUVをRGB に変換する処理で使用するテーブルを初期化
	{
		int i , Num ;

		// リミットテーブルの構築
		for( i = 0 ; i < 512 * 2 + 256 ; i ++ )
		{
			Num = i - 512 ;
			YUVLimitTable[ i ] = ( BYTE )( Num < 0 ? 0 : ( Num > 255 ? 255 : Num ) ) ;
		}

		// YUV変換テーブルの構築
		for( i = 0 ; i < 256 ; i ++ )
		{
			YUVTable[ YUV_RV ][ i ] =   ( int )( 1.596f * 16384 ) * ( i - 128 ) ;
			YUVTable[ YUV_GU ][ i ] = - ( int )( 0.391f * 16384 ) * ( i - 128 ) ;
			YUVTable[ YUV_GV ][ i ] = - ( int )( 0.813f * 16384 ) * ( i - 128 ) ;
			YUVTable[ YUV_BU ][ i ] =   ( int )( 2.018f * 16384 ) * ( i - 128 ) ;
			YUVTable[ YUV_Y  ][ i ] =   ( int )( 1.164f * 16384 ) * ( i - 16  ) ;
		}
	}

	// RGB値をPMA値に変換する処理用のテーブルとPMA値をRGB値に変換する処理用のテーブルを作成
	{
		DWORD i, j ;
		DWORD tmp ;

		for( i = 0 ; i < 256 ; i ++ )
		{
			for( j = 0 ; j < 256 ; j ++ )
			{
				BASEIM.RgbToPmaTable[ i ][ j ] = ( BYTE )( i * j / 255 ) ;

				if( j == 0 )
				{
					BASEIM.PmaToRgbTable[ i ][ j ] = 0 ;
				}
				else
				{
					tmp = i * 255 / j ;
					BASEIM.PmaToRgbTable[ i ][ j ] = ( BYTE )( tmp > 255 ? 255 : tmp ) ;
				}
			}
		}
	}

	// 初期化フラグを立てる
	BASEIM.InitializeFlag = TRUE ;

	// 終了
	return 0 ;
}

// 基本イメージ管理情報の後始末
extern int TerminateBaseImageManage( void )
{
	// 既に後始末されていたら何もしない
	if( BASEIM.InitializeFlag == FALSE )
	{
		return -1 ;
	}

	// 環境依存の後始末処理を行う
	if( TerminateBaseImageManage_PF() != 0 )
	{
		return -1 ;
	}

	// 今のところ何もすることなし

	// 初期化フラグを倒す
	BASEIM.InitializeFlag = FALSE ;

	// 終了
	return 0 ;
}

















//基本イメージデータ構造体関係

// 各種グラフィックデータから基本イメージデータを構築する
extern int NS_CreateBaseImage(  const TCHAR *FileName, const void *FileImage, int FileImageSize, int DataType, BASEIMAGE *BaseImage, int ReverseFlag )
{
	return NS_CreateGraphImageOrDIBGraph( FileName, FileImage, FileImageSize, DataType, FALSE, ReverseFlag, BaseImage, NULL, NULL ) ;
}

// 画像ファイル若しくはメモリ上に展開された画像ファイルイメージから基本イメージデータを構築する
extern int NS_CreateBaseImageWithStrLen( const TCHAR *FileName, size_t FileNameLength, const void *FileImage, int FileImageSize, int DataType /*=LOADIMAGE_TYPE_FILE*/ , BASEIMAGE *BaseImage,  int ReverseFlag )
{
	int Result ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = NS_CreateGraphImageOrDIBGraph( UseFileNameBuffer, FileImage, FileImageSize, DataType, FALSE, ReverseFlag, BaseImage, NULL, NULL ) ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( FileName )
	return Result ;
}

// 各種グラフィックデータから基本イメージデータを構築する
extern int CreateBaseImage_WCHAR_T(  const wchar_t *FileName, const void *FileImage, int FileImageSize, int DataType, BASEIMAGE *BaseImage, int ReverseFlag )
{
	return CreateGraphImageOrDIBGraph_WCHAR_T( FileName, FileImage, FileImageSize, DataType, FALSE, ReverseFlag, FALSE, BaseImage, NULL, NULL ) ;
}

// CreateBaseImage の旧名称
extern int NS_CreateGraphImage( const TCHAR *FileName, const void *DataImage, int DataImageSize, int DataImageType, BASEIMAGE *GraphImage, int ReverseFlag )
{
	return NS_CreateGraphImageOrDIBGraph( FileName, DataImage, DataImageSize, DataImageType, FALSE, ReverseFlag, GraphImage, NULL, NULL ) ;
}

// 画像ファイルから基本イメージデータを構築する
extern int NS_CreateBaseImageToFile( const TCHAR *FileName,               BASEIMAGE *BaseImage, int ReverseFlag )
{
	return NS_CreateBaseImage( FileName, NULL, 0, LOADIMAGE_TYPE_FILE, BaseImage, ReverseFlag ) ;
}

// 画像ファイルから基本イメージデータを構築する
extern int NS_CreateBaseImageToFileWithStrLen( const TCHAR *FileName, size_t FileNameLength, BASEIMAGE *BaseImage, int ReverseFlag )
{
	int Result ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = NS_CreateBaseImage( UseFileNameBuffer, NULL, 0, LOADIMAGE_TYPE_FILE, BaseImage, ReverseFlag ) ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( FileName )
	return Result ;
}

// 画像ファイルから基本イメージデータを構築する
extern int CreateBaseImageToFile_WCHAR_T( const wchar_t *FileName,               BASEIMAGE *BaseImage, int ReverseFlag )
{
	return CreateBaseImage_WCHAR_T( FileName, NULL, 0, LOADIMAGE_TYPE_FILE, BaseImage, ReverseFlag ) ;
}

// メモリ上に展開された画像ファイルから基本イメージデータを構築する
extern int NS_CreateBaseImageToMem(  const void *FileImage, int FileImageSize, BASEIMAGE *BaseImage, int ReverseFlag )
{
	return NS_CreateBaseImage( NULL, FileImage, FileImageSize, LOADIMAGE_TYPE_MEM, BaseImage, ReverseFlag ) ;
}

// ＡＲＧＢ各チャンネル 32bit 浮動小数点型 カラーの基本イメージデータを作成する
extern int NS_CreateARGBF32ColorBaseImage( int SizeX, int SizeY, BASEIMAGE *BaseImage )
{
	NS_CreateARGBF32ColorData( &BaseImage->ColorData );
	BaseImage->Width          = SizeX ;
	BaseImage->Height         = SizeY ;
	BaseImage->Pitch          = SizeX * BaseImage->ColorData.PixelByte ;
	BaseImage->Pitch          = ( BaseImage->Pitch + 3 ) / 4 * 4 ;
	BaseImage->GraphData      = DXALLOC( ( size_t )( BaseImage->Pitch * BaseImage->Height ) );
	if( BaseImage->GraphData == NULL ) return -1;
	BaseImage->MipMapCount    = 0 ;
	BaseImage->GraphDataCount = 0 ;

	// 終了
	return 0;
}

// ＡＲＧＢ各チャンネル 16bit 浮動小数点型 カラーの基本イメージデータを作成する
extern int NS_CreateARGBF16ColorBaseImage( int SizeX, int SizeY, BASEIMAGE *BaseImage )
{
	NS_CreateARGBF16ColorData( &BaseImage->ColorData );
	BaseImage->Width          = SizeX ;
	BaseImage->Height         = SizeY ;
	BaseImage->Pitch          = SizeX * BaseImage->ColorData.PixelByte ;
	BaseImage->Pitch          = ( BaseImage->Pitch + 3 ) / 4 * 4 ;
	BaseImage->GraphData      = DXALLOC( ( size_t )( BaseImage->Pitch * BaseImage->Height ) );
	if( BaseImage->GraphData == NULL ) return -1;
	BaseImage->MipMapCount    = 0 ;
	BaseImage->GraphDataCount = 0 ;

	// 終了
	return 0;
}

// ＡＲＧＢ８カラーの空の基本イメージデータを作成する
extern int NS_CreateARGB8ColorBaseImage( int SizeX, int SizeY, BASEIMAGE *BaseImage )
{
	NS_CreateARGB8ColorData( &BaseImage->ColorData );
	BaseImage->Width          = SizeX ;
	BaseImage->Height         = SizeY ;
	BaseImage->Pitch          = SizeX * BaseImage->ColorData.PixelByte ;
	BaseImage->Pitch          = ( BaseImage->Pitch + 3 ) / 4 * 4 ;
	BaseImage->GraphData      = DXALLOC( ( size_t )( BaseImage->Pitch * BaseImage->Height ) );
	if( BaseImage->GraphData == NULL ) return -1;
	BaseImage->MipMapCount    = 0 ;
	BaseImage->GraphDataCount = 0 ;

	// 終了
	return 0;
}

// ＸＲＧＢ８カラーの空の基本イメージデータを作成する
extern int NS_CreateXRGB8ColorBaseImage( int SizeX, int SizeY, BASEIMAGE *BaseImage )
{
	NS_CreateXRGB8ColorData( &BaseImage->ColorData );
	BaseImage->Width  = SizeX;
	BaseImage->Height = SizeY;
	BaseImage->Pitch  = SizeX * BaseImage->ColorData.PixelByte;
	BaseImage->Pitch  = ( BaseImage->Pitch + 3 ) / 4 * 4;
	BaseImage->GraphData = DXALLOC( ( size_t )( BaseImage->Pitch * BaseImage->Height ) );
	if( BaseImage->GraphData == NULL ) return -1;
	BaseImage->MipMapCount = 0 ;
	BaseImage->GraphDataCount = 0 ;

	// 終了
	return 0;
}

// ＡＲＧＢ４カラーの空の基本イメージデータを作成する
extern int NS_CreateARGB4ColorBaseImage( int SizeX, int SizeY, BASEIMAGE *BaseImage )
{
	NS_CreateARGB4ColorData( &BaseImage->ColorData );
	BaseImage->Width  = SizeX;
	BaseImage->Height = SizeY;
	BaseImage->Pitch  = SizeX * BaseImage->ColorData.PixelByte;
	BaseImage->Pitch  = ( BaseImage->Pitch + 3 ) / 4 * 4;
	BaseImage->GraphData = DXALLOC( ( size_t )( BaseImage->Pitch * BaseImage->Height ) );
	if( BaseImage->GraphData == NULL ) return -1;
	BaseImage->MipMapCount = 0 ;
	BaseImage->GraphDataCount = 0 ;

	// 終了
	return 0;
}

// Ａ１Ｒ５Ｇ５Ｂ５カラーの基本イメージデータを作成する
extern int NS_CreateA1R5G5B5ColorBaseImage( int SizeX, int SizeY, BASEIMAGE *BaseImage )
{
	NS_CreateA1R5G5B5ColorData( &BaseImage->ColorData );
	BaseImage->Width  = SizeX;
	BaseImage->Height = SizeY;
	BaseImage->Pitch  = SizeX * BaseImage->ColorData.PixelByte;
	BaseImage->Pitch  = ( BaseImage->Pitch + 3 ) / 4 * 4;
	BaseImage->GraphData = DXALLOC( ( size_t )( BaseImage->Pitch * BaseImage->Height ) );
	if( BaseImage->GraphData == NULL ) return -1;
	BaseImage->MipMapCount = 0 ;
	BaseImage->GraphDataCount = 0 ;

	// 終了
	return 0;
}

// Ｘ１Ｒ５Ｇ５Ｂ５カラーの基本イメージデータを作成する
extern int NS_CreateX1R5G5B5ColorBaseImage( int SizeX, int SizeY, BASEIMAGE *BaseImage )
{
	NS_CreateX1R5G5B5ColorData( &BaseImage->ColorData );
	BaseImage->Width  = SizeX;
	BaseImage->Height = SizeY;
	BaseImage->Pitch  = SizeX * BaseImage->ColorData.PixelByte;
	BaseImage->Pitch  = ( BaseImage->Pitch + 3 ) / 4 * 4;
	BaseImage->GraphData = DXALLOC( ( size_t )( BaseImage->Pitch * BaseImage->Height ) );
	if( BaseImage->GraphData == NULL ) return -1;
	BaseImage->MipMapCount = 0 ;
	BaseImage->GraphDataCount = 0 ;

	// 終了
	return 0;
}

// Ｒ５Ｇ５Ｂ５Ａ１カラーの基本イメージデータを作成する
extern int NS_CreateR5G5B5A1ColorBaseImage( int SizeX, int SizeY, BASEIMAGE *BaseImage )
{
	NS_CreateR5G5B5A1ColorData( &BaseImage->ColorData );
	BaseImage->Width  = SizeX;
	BaseImage->Height = SizeY;
	BaseImage->Pitch  = SizeX * BaseImage->ColorData.PixelByte;
	BaseImage->Pitch  = ( BaseImage->Pitch + 3 ) / 4 * 4;
	BaseImage->GraphData = DXALLOC( ( size_t )( BaseImage->Pitch * BaseImage->Height ) );
	if( BaseImage->GraphData == NULL ) return -1;
	BaseImage->MipMapCount = 0 ;
	BaseImage->GraphDataCount = 0 ;

	// 終了
	return 0;
}

// Ｒ５Ｇ６Ｂ５カラーの基本イメージデータを作成する
extern int NS_CreateR5G6B5ColorBaseImage( int SizeX, int SizeY, BASEIMAGE *BaseImage )
{
	NS_CreateR5G6B5ColorData( &BaseImage->ColorData );
	BaseImage->Width  = SizeX;
	BaseImage->Height = SizeY;
	BaseImage->Pitch  = SizeX * BaseImage->ColorData.PixelByte;
	BaseImage->Pitch  = ( BaseImage->Pitch + 3 ) / 4 * 4;
	BaseImage->GraphData = DXALLOC( ( size_t )( BaseImage->Pitch * BaseImage->Height ) );
	if( BaseImage->GraphData == NULL ) return -1;
	BaseImage->MipMapCount = 0 ;
	BaseImage->GraphDataCount = 0 ;

	// 終了
	return 0;
}

// ＲＧＢ８カラーの空の基本イメージデータを作成する
extern int NS_CreateRGB8ColorBaseImage( int SizeX, int SizeY, BASEIMAGE *BaseImage )
{
	NS_CreateFullColorData( &BaseImage->ColorData );
	BaseImage->Width  = SizeX;
	BaseImage->Height = SizeY;
	BaseImage->Pitch  = SizeX * BaseImage->ColorData.PixelByte;
	BaseImage->Pitch  = ( BaseImage->Pitch + 3 ) / 4 * 4;
	BaseImage->GraphData = DXALLOC( ( size_t )( BaseImage->Pitch * BaseImage->Height ) );
	if( BaseImage->GraphData == NULL ) return -1;
	BaseImage->MipMapCount = 0 ;
	BaseImage->GraphDataCount = 0 ;

	// 終了
	return 0;
}

// パレット８ビットカラーの空の基本イメージデータを作成する
extern int NS_CreatePAL8ColorBaseImage( int SizeX, int SizeY, BASEIMAGE *BaseImage, int UseAlpha )
{
	NS_CreatePal8ColorData( &BaseImage->ColorData, UseAlpha );
	BaseImage->Width  = SizeX;
	BaseImage->Height = SizeY;
	BaseImage->Pitch  = SizeX * BaseImage->ColorData.PixelByte;
	BaseImage->Pitch  = ( BaseImage->Pitch + 3 ) / 4 * 4;
	BaseImage->GraphData = DXALLOC( ( size_t )( BaseImage->Pitch * BaseImage->Height ) );
	if( BaseImage->GraphData == NULL ) return -1;
	BaseImage->MipMapCount = 0 ;
	BaseImage->GraphDataCount = 0 ;

	// 終了
	return 0;
}

// 指定のカラーフォーマットの殻の基本イメージデータを作成する
extern int NS_CreateColorDataBaseImage( int SizeX, int SizeY, const COLORDATA *ColorData, BASEIMAGE *BaseImage )
{
	BaseImage->ColorData = *ColorData ;
	BaseImage->Width  = SizeX;
	BaseImage->Height = SizeY;
	BaseImage->Pitch  = SizeX * BaseImage->ColorData.PixelByte;
	BaseImage->Pitch  = ( BaseImage->Pitch + 3 ) / 4 * 4;
	BaseImage->GraphData = DXALLOC( ( size_t )( BaseImage->Pitch * BaseImage->Height ) );
	if( BaseImage->GraphData == NULL ) return -1;
	BaseImage->MipMapCount = 0 ;
	BaseImage->GraphDataCount = 0 ;

	// 終了
	return 0;
}

// 基本イメージデータのイメージサイズを取得する( byte単位 )
extern int NS_GetBaseImageGraphDataSize( const BASEIMAGE *BaseImage )
{
	int BaseImageSize ;
	int TotalSize ;
	int PacketSize ;
	int i ;

	if( BaseImage->ColorData.Format == DX_BASEIMAGE_FORMAT_NORMAL )
	{
		BaseImageSize = BaseImage->Pitch * BaseImage->Height ;
	}
	else
	{
		switch( BaseImage->ColorData.Format )
		{
		case DX_BASEIMAGE_FORMAT_DXT1 :
			PacketSize = 8 ;
			break ;

		case DX_BASEIMAGE_FORMAT_DXT2 :
			PacketSize = 16 ;
			break ;

		case DX_BASEIMAGE_FORMAT_DXT3 :
			PacketSize = 16 ;
			break ;

		case DX_BASEIMAGE_FORMAT_DXT4 :
			PacketSize = 16 ;
			break ;

		case DX_BASEIMAGE_FORMAT_DXT5 :
			PacketSize = 16 ;
			break ;

		default :
			return -1 ;
		}

		BaseImageSize = ( BaseImage->Width / 4 ) * ( BaseImage->Height / 4 ) * PacketSize ;
	}

	if( BaseImage->MipMapCount > 1 )
	{
		TotalSize = 0 ;
		for( i = 0 ; i < BaseImage->MipMapCount ; i ++ )
		{
			TotalSize += BaseImageSize ;
			BaseImageSize /= 4 ;
			if( BaseImageSize < 8 )
			{
				BaseImageSize = 8 ;
			}
		}
	}
	else
	{
		TotalSize = BaseImageSize ;
	}

	return TotalSize ;
}

// 基本イメージデータの指定の部分だけを使う基本イメージデータの情報を作成する
extern int NS_DerivationBaseImage( const BASEIMAGE *BaseImage, int x1, int y1, int x2, int y2, BASEIMAGE *NewBaseImage )
{
	// 座標エラーチェック
	if( x1 < 0 || y1 < 0 || x1 >= x2 || y1 >= y2 ||
		x2 > BaseImage->Width || y2 > BaseImage->Height )
	{
		return -1 ;
	}

	// 標準フォーマット＋MipMap非使用のみ対応
	if( BaseImage->ColorData.Format != DX_BASEIMAGE_FORMAT_NORMAL ||
		BaseImage->MipMapCount != 1 )
	{
		return -1 ;
	}

	// 情報をセット
	*NewBaseImage = *BaseImage ;
	NewBaseImage->Width  = x2 - x1 ;
	NewBaseImage->Height = y2 - y1 ;
	NewBaseImage->GraphData = ( BYTE * )BaseImage->GraphData + x1 * BaseImage->ColorData.PixelByte + y1 * BaseImage->Pitch ;

	// 終了
	return 0 ;
}

// 基本イメージデータの後始末を行う
extern int NS_ReleaseBaseImage(  BASEIMAGE *GraphImage )
{
	if( GraphImage->GraphData != NULL )
	{
		DXFREE( GraphImage->GraphData ) ;
		GraphImage->GraphData = NULL ;
	}
	_MEMSET( GraphImage, 0, sizeof( BASEIMAGE ) ) ;

	// 終了
	return 0 ;
}

// ReleaseBaseImage の旧名称
extern int NS_ReleaseGraphImage( BASEIMAGE *GraphImage )
{
	return NS_ReleaseBaseImage( GraphImage ) ;
}

// DX_BASEIMAGE_FORMAT_NORMAL 以外の形式のイメージデータを DX_BASEIMAGE_FORMAT_NORMAL 形式のイメージに変換する
extern int NS_ConvertNormalFormatBaseImage( BASEIMAGE *BaseImage, int ReleaseOrigGraphData )
{
	void *NewBuffer ;
	DWORD NewPixelByte, NewPitch, NewImageSize ;
	DWORD code, *dstd, pw, ph, i, j, k ;
	BYTE *dstb, *srcd, *bdstb ;
	BYTE C[ 4 ][ 4 ], A[ 8 ] ;
	DWORD GraphNum ;

	// 最初から形式が DX_BASEIMAGE_FORMAT_NORMAL だったら何もせずに終了
	if( BaseImage->ColorData.Format == DX_BASEIMAGE_FORMAT_NORMAL )
		return 0 ;

	// データの数をセット
	GraphNum = ( DWORD )( BaseImage->GraphDataCount == 0 ? 1 : 6 ) ;

	// 新しいデータを格納するメモリ領域の確保
	NewPixelByte = 4 ;
	NewPitch = BaseImage->Width * NewPixelByte ;
	NewImageSize = NewPitch * BaseImage->Height ;
	NewBuffer = DXALLOC( NewImageSize * GraphNum ) ;
	if( NewBuffer == NULL )
	{
		// メモリが確保できなかったらエラー
		return -1 ;
	}

	// フォーマットによって処理を分岐
	pw = ( DWORD )( BaseImage->Width  / 4 ) ;
	ph = ( DWORD )( BaseImage->Height / 4 ) ;
	for( k = 0 ; k < GraphNum ; k ++ )
	{
		bdstb = ( BYTE * )NewBuffer + NewImageSize * k ;
		srcd = ( BYTE * )BaseImage->GraphData ;
		switch( BaseImage->ColorData.Format )
		{
		case DX_BASEIMAGE_FORMAT_DXT1 :
			for( i = 0 ; i < ph ; i ++ )
			{
				for( j = 0 ; j < pw ; j ++, srcd += 8 )
				{
					dstd = ( DWORD * )( bdstb + ( j * 4 ) * NewPixelByte + ( i * 4 ) * NewPitch ) ;

					C[ 0 ][ 0 ] = ( BYTE )( ( ( ( ( WORD * )srcd )[ 0 ] >>  0 ) & 31 ) * 255 / 31 ) ;
					C[ 0 ][ 1 ] = ( BYTE )( ( ( ( ( WORD * )srcd )[ 0 ] >>  5 ) & 63 ) * 255 / 63 ) ;
					C[ 0 ][ 2 ] = ( BYTE )( ( ( ( ( WORD * )srcd )[ 0 ] >> 11 ) & 31 ) * 255 / 31 ) ;
					C[ 0 ][ 3 ] = 255 ;

					C[ 1 ][ 0 ] = ( BYTE )( ( ( ( ( WORD * )srcd )[ 1 ] >>  0 ) & 31 ) * 255 / 31 ) ;
					C[ 1 ][ 1 ] = ( BYTE )( ( ( ( ( WORD * )srcd )[ 1 ] >>  5 ) & 63 ) * 255 / 63 ) ;
					C[ 1 ][ 2 ] = ( BYTE )( ( ( ( ( WORD * )srcd )[ 1 ] >> 11 ) & 31 ) * 255 / 31 ) ;
					C[ 1 ][ 3 ] = 255 ;

					if( ( ( WORD * )srcd )[ 0 ] > ( ( WORD * )srcd )[ 1 ] )
					{
						// 透過色なし
						C[ 2 ][ 0 ] = ( BYTE )( ( C[ 0 ][ 0 ] * 2 + C[ 1 ][ 0 ] ) / 3 ) ;
						C[ 2 ][ 1 ] = ( BYTE )( ( C[ 0 ][ 1 ] * 2 + C[ 1 ][ 1 ] ) / 3 ) ;
						C[ 2 ][ 2 ] = ( BYTE )( ( C[ 0 ][ 2 ] * 2 + C[ 1 ][ 2 ] ) / 3 ) ;
						C[ 2 ][ 3 ] = 255 ;

						C[ 3 ][ 0 ] = ( BYTE )( ( C[ 0 ][ 0 ] + C[ 1 ][ 0 ] * 2 ) / 3 ) ;
						C[ 3 ][ 1 ] = ( BYTE )( ( C[ 0 ][ 1 ] + C[ 1 ][ 1 ] * 2 ) / 3 ) ;
						C[ 3 ][ 2 ] = ( BYTE )( ( C[ 0 ][ 2 ] + C[ 1 ][ 2 ] * 2 ) / 3 ) ;
						C[ 3 ][ 3 ] = 255 ;
					}
					else
					{
						// 透過色あり
						C[ 2 ][ 0 ] = ( BYTE )( ( C[ 0 ][ 0 ] + C[ 1 ][ 0 ] ) / 2 ) ;
						C[ 2 ][ 1 ] = ( BYTE )( ( C[ 0 ][ 1 ] + C[ 1 ][ 1 ] ) / 2 ) ;
						C[ 2 ][ 2 ] = ( BYTE )( ( C[ 0 ][ 2 ] + C[ 1 ][ 2 ] ) / 2 ) ;
						C[ 2 ][ 3 ] = 255 ;

						C[ 3 ][ 0 ] = ( BYTE )( ( C[ 0 ][ 0 ] + C[ 1 ][ 0 ] ) / 2 ) ;
						C[ 3 ][ 1 ] = ( BYTE )( ( C[ 0 ][ 1 ] + C[ 1 ][ 1 ] ) / 2 ) ;
						C[ 3 ][ 2 ] = ( BYTE )( ( C[ 0 ][ 2 ] + C[ 1 ][ 2 ] ) / 2 ) ;
						C[ 3 ][ 3 ] = 0 ;
					}

					code = *( ( DWORD * )( srcd + 4 ) ) ;
					dstd[ 0 ] = *( ( DWORD * )C[ ( code >>  0 ) & 3 ] ) ;
					dstd[ 1 ] = *( ( DWORD * )C[ ( code >>  2 ) & 3 ] ) ;
					dstd[ 2 ] = *( ( DWORD * )C[ ( code >>  4 ) & 3 ] ) ;
					dstd[ 3 ] = *( ( DWORD * )C[ ( code >>  6 ) & 3 ] ) ;
					dstd = ( DWORD * )( ( BYTE * )dstd + NewPitch ) ;

					dstd[ 0 ] = *( ( DWORD * )C[ ( code >>  8 ) & 3 ] ) ;
					dstd[ 1 ] = *( ( DWORD * )C[ ( code >> 10 ) & 3 ] ) ;
					dstd[ 2 ] = *( ( DWORD * )C[ ( code >> 12 ) & 3 ] ) ;
					dstd[ 3 ] = *( ( DWORD * )C[ ( code >> 14 ) & 3 ] ) ;
					dstd = ( DWORD * )( ( BYTE * )dstd + NewPitch ) ;

					dstd[ 0 ] = *( ( DWORD * )C[ ( code >> 16 ) & 3 ] ) ;
					dstd[ 1 ] = *( ( DWORD * )C[ ( code >> 18 ) & 3 ] ) ;
					dstd[ 2 ] = *( ( DWORD * )C[ ( code >> 20 ) & 3 ] ) ;
					dstd[ 3 ] = *( ( DWORD * )C[ ( code >> 22 ) & 3 ] ) ;
					dstd = ( DWORD * )( ( BYTE * )dstd + NewPitch ) ;

					dstd[ 0 ] = *( ( DWORD * )C[ ( code >> 24 ) & 3 ] ) ;
					dstd[ 1 ] = *( ( DWORD * )C[ ( code >> 26 ) & 3 ] ) ;
					dstd[ 2 ] = *( ( DWORD * )C[ ( code >> 28 ) & 3 ] ) ;
					dstd[ 3 ] = *( ( DWORD * )C[ ( code >> 30 ) & 3 ] ) ;
				}
			}
			break ;

		case DX_BASEIMAGE_FORMAT_DXT2 :
			for( i = 0 ; i < ph ; i ++ )
			{
				for( j = 0 ; j < pw ; j ++ )
				{
					// カラーの処理
					{
						dstb = bdstb + ( j * 4 ) * NewPixelByte + ( i * 4 ) * NewPitch ;
						dstd = ( DWORD * )dstb ;
						C[ 0 ][ 0 ] = ( BYTE )( ( ( ( ( WORD * )srcd )[ 0 ] >>  0 ) & 31 ) * 255 / 31 ) ; 
						C[ 0 ][ 1 ] = ( BYTE )( ( ( ( ( WORD * )srcd )[ 0 ] >>  5 ) & 63 ) * 255 / 63 ) ; 
						C[ 0 ][ 2 ] = ( BYTE )( ( ( ( ( WORD * )srcd )[ 0 ] >> 11 ) & 31 ) * 255 / 31 ) ;
						C[ 0 ][ 3 ] = 255 ;

						C[ 1 ][ 0 ] = ( BYTE )( ( ( ( ( WORD * )srcd )[ 1 ] >>  0 ) & 31 ) * 255 / 31 ) ;
						C[ 1 ][ 1 ] = ( BYTE )( ( ( ( ( WORD * )srcd )[ 1 ] >>  5 ) & 63 ) * 255 / 63 ) ;
						C[ 1 ][ 2 ] = ( BYTE )( ( ( ( ( WORD * )srcd )[ 1 ] >> 11 ) & 31 ) * 255 / 31 ) ;
						C[ 1 ][ 3 ] = 255 ;

						C[ 2 ][ 0 ] = ( BYTE )( ( C[ 0 ][ 0 ] * 2 + C[ 1 ][ 0 ] ) / 3 ) ;
						C[ 2 ][ 1 ] = ( BYTE )( ( C[ 0 ][ 1 ] * 2 + C[ 1 ][ 1 ] ) / 3 ) ;
						C[ 2 ][ 2 ] = ( BYTE )( ( C[ 0 ][ 2 ] * 2 + C[ 1 ][ 2 ] ) / 3 ) ;
						C[ 2 ][ 3 ] = 255 ;

						C[ 3 ][ 0 ] = ( BYTE )( ( C[ 0 ][ 0 ] + C[ 1 ][ 0 ] * 2 ) / 3 ) ;
						C[ 3 ][ 1 ] = ( BYTE )( ( C[ 0 ][ 1 ] + C[ 1 ][ 1 ] * 2 ) / 3 ) ;
						C[ 3 ][ 2 ] = ( BYTE )( ( C[ 0 ][ 2 ] + C[ 1 ][ 2 ] * 2 ) / 3 ) ;
						C[ 3 ][ 3 ] = 255 ;

						code = *( ( DWORD * )( srcd + 4 ) ) ;
						dstd[ 0 ] = *( ( DWORD * )C[ ( code >>  0 ) & 3 ] ) ;
						dstd[ 1 ] = *( ( DWORD * )C[ ( code >>  2 ) & 3 ] ) ;
						dstd[ 2 ] = *( ( DWORD * )C[ ( code >>  4 ) & 3 ] ) ;
						dstd[ 3 ] = *( ( DWORD * )C[ ( code >>  6 ) & 3 ] ) ;
						dstd = ( DWORD * )( ( BYTE * )dstd + NewPitch ) ;

						dstd[ 0 ] = *( ( DWORD * )C[ ( code >>  8 ) & 3 ] ) ;
						dstd[ 1 ] = *( ( DWORD * )C[ ( code >> 10 ) & 3 ] ) ;
						dstd[ 2 ] = *( ( DWORD * )C[ ( code >> 12 ) & 3 ] ) ;
						dstd[ 3 ] = *( ( DWORD * )C[ ( code >> 14 ) & 3 ] ) ;
						dstd = ( DWORD * )( ( BYTE * )dstd + NewPitch ) ;

						dstd[ 0 ] = *( ( DWORD * )C[ ( code >> 16 ) & 3 ] ) ;
						dstd[ 1 ] = *( ( DWORD * )C[ ( code >> 18 ) & 3 ] ) ;
						dstd[ 2 ] = *( ( DWORD * )C[ ( code >> 20 ) & 3 ] ) ;
						dstd[ 3 ] = *( ( DWORD * )C[ ( code >> 22 ) & 3 ] ) ;
						dstd = ( DWORD * )( ( BYTE * )dstd + NewPitch ) ;

						dstd[ 0 ] = *( ( DWORD * )C[ ( code >> 24 ) & 3 ] ) ;
						dstd[ 1 ] = *( ( DWORD * )C[ ( code >> 26 ) & 3 ] ) ;
						dstd[ 2 ] = *( ( DWORD * )C[ ( code >> 28 ) & 3 ] ) ;
						dstd[ 3 ] = *( ( DWORD * )C[ ( code >> 30 ) & 3 ] ) ;
					}
					srcd += 8 ;

					// アルファチャンネルのセット
					{
						code = *( ( DWORD * )srcd ) ;
						dstb[  3 ] = ( BYTE )( ( ( code >>  0 ) & 15 ) * 255 / 15 ) ;
						dstb[  7 ] = ( BYTE )( ( ( code >>  4 ) & 15 ) * 255 / 15 ) ;
						dstb[ 11 ] = ( BYTE )( ( ( code >>  8 ) & 15 ) * 255 / 15 ) ;
						dstb[ 15 ] = ( BYTE )( ( ( code >> 12 ) & 15 ) * 255 / 15 ) ;
						dstb = dstb + NewPitch ;

						dstb[  3 ] = ( BYTE )( ( ( code >> 16 ) & 15 ) * 255 / 15 ) ;
						dstb[  7 ] = ( BYTE )( ( ( code >> 20 ) & 15 ) * 255 / 15 ) ;
						dstb[ 11 ] = ( BYTE )( ( ( code >> 24 ) & 15 ) * 255 / 15 ) ;
						dstb[ 15 ] = ( BYTE )( ( ( code >> 28 ) & 15 ) * 255 / 15 ) ;
						dstb = dstb + NewPitch ;

						code = *( ( DWORD * )( srcd + 4 ) ) ;
						dstb[  3 ] = ( BYTE )( ( ( code >>  0 ) & 15 ) * 255 / 15 ) ;
						dstb[  7 ] = ( BYTE )( ( ( code >>  4 ) & 15 ) * 255 / 15 ) ;
						dstb[ 11 ] = ( BYTE )( ( ( code >>  8 ) & 15 ) * 255 / 15 ) ;
						dstb[ 15 ] = ( BYTE )( ( ( code >> 12 ) & 15 ) * 255 / 15 ) ;
						dstb = dstb + NewPitch ;

						dstb[  3 ] = ( BYTE )( ( ( code >> 16 ) & 15 ) * 255 / 15 ) ;
						dstb[  7 ] = ( BYTE )( ( ( code >> 20 ) & 15 ) * 255 / 15 ) ;
						dstb[ 11 ] = ( BYTE )( ( ( code >> 24 ) & 15 ) * 255 / 15 ) ;
						dstb[ 15 ] = ( BYTE )( ( ( code >> 28 ) & 15 ) * 255 / 15 ) ;
					}
					srcd += 8 ;

					// アルファチャンネルの反適応
					{
						dstb -= NewPitch * 3 ;
						dstb[  0 ] = ( BYTE )( dstb[  0 ] * 255 / dstb[  3 ] ) ;
						dstb[  1 ] = ( BYTE )( dstb[  1 ] * 255 / dstb[  3 ] ) ;
						dstb[  2 ] = ( BYTE )( dstb[  2 ] * 255 / dstb[  3 ] ) ;

						dstb[  4 ] = ( BYTE )( dstb[  4 ] * 255 / dstb[  7 ] ) ;
						dstb[  5 ] = ( BYTE )( dstb[  5 ] * 255 / dstb[  7 ] ) ;
						dstb[  6 ] = ( BYTE )( dstb[  6 ] * 255 / dstb[  7 ] ) ;

						dstb[  8 ] = ( BYTE )( dstb[  8 ] * 255 / dstb[ 11 ] ) ;
						dstb[  9 ] = ( BYTE )( dstb[  9 ] * 255 / dstb[ 11 ] ) ;
						dstb[ 10 ] = ( BYTE )( dstb[ 10 ] * 255 / dstb[ 11 ] ) ;

						dstb[ 12 ] = ( BYTE )( dstb[ 12 ] * 255 / dstb[ 15 ] ) ;
						dstb[ 13 ] = ( BYTE )( dstb[ 13 ] * 255 / dstb[ 15 ] ) ;
						dstb[ 14 ] = ( BYTE )( dstb[ 14 ] * 255 / dstb[ 15 ] ) ;
						dstb = dstb + NewPitch ;

						dstb[  0 ] = ( BYTE )( dstb[  0 ] * 255 / dstb[  3 ] ) ;
						dstb[  1 ] = ( BYTE )( dstb[  1 ] * 255 / dstb[  3 ] ) ;
						dstb[  2 ] = ( BYTE )( dstb[  2 ] * 255 / dstb[  3 ] ) ;

						dstb[  4 ] = ( BYTE )( dstb[  4 ] * 255 / dstb[  7 ] ) ;
						dstb[  5 ] = ( BYTE )( dstb[  5 ] * 255 / dstb[  7 ] ) ;
						dstb[  6 ] = ( BYTE )( dstb[  6 ] * 255 / dstb[  7 ] ) ;

						dstb[  8 ] = ( BYTE )( dstb[  8 ] * 255 / dstb[ 11 ] ) ;
						dstb[  9 ] = ( BYTE )( dstb[  9 ] * 255 / dstb[ 11 ] ) ;
						dstb[ 10 ] = ( BYTE )( dstb[ 10 ] * 255 / dstb[ 11 ] ) ;

						dstb[ 12 ] = ( BYTE )( dstb[ 12 ] * 255 / dstb[ 15 ] ) ;
						dstb[ 13 ] = ( BYTE )( dstb[ 13 ] * 255 / dstb[ 15 ] ) ;
						dstb[ 14 ] = ( BYTE )( dstb[ 14 ] * 255 / dstb[ 15 ] ) ;
						dstb = dstb + NewPitch ;

						dstb[  0 ] = ( BYTE )( dstb[  0 ] * 255 / dstb[  3 ] ) ;
						dstb[  1 ] = ( BYTE )( dstb[  1 ] * 255 / dstb[  3 ] ) ;
						dstb[  2 ] = ( BYTE )( dstb[  2 ] * 255 / dstb[  3 ] ) ;

						dstb[  4 ] = ( BYTE )( dstb[  4 ] * 255 / dstb[  7 ] ) ;
						dstb[  5 ] = ( BYTE )( dstb[  5 ] * 255 / dstb[  7 ] ) ;
						dstb[  6 ] = ( BYTE )( dstb[  6 ] * 255 / dstb[  7 ] ) ;

						dstb[  8 ] = ( BYTE )( dstb[  8 ] * 255 / dstb[ 11 ] ) ;
						dstb[  9 ] = ( BYTE )( dstb[  9 ] * 255 / dstb[ 11 ] ) ;
						dstb[ 10 ] = ( BYTE )( dstb[ 10 ] * 255 / dstb[ 11 ] ) ;

						dstb[ 12 ] = ( BYTE )( dstb[ 12 ] * 255 / dstb[ 15 ] ) ;
						dstb[ 13 ] = ( BYTE )( dstb[ 13 ] * 255 / dstb[ 15 ] ) ;
						dstb[ 14 ] = ( BYTE )( dstb[ 14 ] * 255 / dstb[ 15 ] ) ;
						dstb = dstb + NewPitch ;

						dstb[  0 ] = ( BYTE )( dstb[  0 ] * 255 / dstb[  3 ] ) ;
						dstb[  1 ] = ( BYTE )( dstb[  1 ] * 255 / dstb[  3 ] ) ;
						dstb[  2 ] = ( BYTE )( dstb[  2 ] * 255 / dstb[  3 ] ) ;

						dstb[  4 ] = ( BYTE )( dstb[  4 ] * 255 / dstb[  7 ] ) ;
						dstb[  5 ] = ( BYTE )( dstb[  5 ] * 255 / dstb[  7 ] ) ;
						dstb[  6 ] = ( BYTE )( dstb[  6 ] * 255 / dstb[  7 ] ) ;

						dstb[  8 ] = ( BYTE )( dstb[  8 ] * 255 / dstb[ 11 ] ) ;
						dstb[  9 ] = ( BYTE )( dstb[  9 ] * 255 / dstb[ 11 ] ) ;
						dstb[ 10 ] = ( BYTE )( dstb[ 10 ] * 255 / dstb[ 11 ] ) ;

						dstb[ 12 ] = ( BYTE )( dstb[ 12 ] * 255 / dstb[ 15 ] ) ;
						dstb[ 13 ] = ( BYTE )( dstb[ 13 ] * 255 / dstb[ 15 ] ) ;
						dstb[ 14 ] = ( BYTE )( dstb[ 14 ] * 255 / dstb[ 15 ] ) ;
					}
				}
			}
			break ;

		case DX_BASEIMAGE_FORMAT_DXT3 :
			for( i = 0 ; i < ph ; i ++ )
			{
				for( j = 0 ; j < pw ; j ++ )
				{
					// カラーの処理
					{
						dstb = bdstb + ( j * 4 ) * NewPixelByte + ( i * 4 ) * NewPitch ;
						dstd = ( DWORD * )dstb ;
						C[ 0 ][ 0 ] = ( BYTE )( ( ( ( ( WORD * )srcd )[ 0 ] >>  0 ) & 31 ) * 255 / 31 ) ; 
						C[ 0 ][ 1 ] = ( BYTE )( ( ( ( ( WORD * )srcd )[ 0 ] >>  5 ) & 63 ) * 255 / 63 ) ; 
						C[ 0 ][ 2 ] = ( BYTE )( ( ( ( ( WORD * )srcd )[ 0 ] >> 11 ) & 31 ) * 255 / 31 ) ; 
						C[ 0 ][ 3 ] = 255 ;

						C[ 1 ][ 0 ] = ( BYTE )( ( ( ( ( WORD * )srcd )[ 1 ] >>  0 ) & 31 ) * 255 / 31 ) ; 
						C[ 1 ][ 1 ] = ( BYTE )( ( ( ( ( WORD * )srcd )[ 1 ] >>  5 ) & 63 ) * 255 / 63 ) ; 
						C[ 1 ][ 2 ] = ( BYTE )( ( ( ( ( WORD * )srcd )[ 1 ] >> 11 ) & 31 ) * 255 / 31 ) ; 
						C[ 1 ][ 3 ] = 255 ;

						C[ 2 ][ 0 ] = ( BYTE )( ( C[ 0 ][ 0 ] * 2 + C[ 1 ][ 0 ] ) / 3 ) ;
						C[ 2 ][ 1 ] = ( BYTE )( ( C[ 0 ][ 1 ] * 2 + C[ 1 ][ 1 ] ) / 3 ) ;
						C[ 2 ][ 2 ] = ( BYTE )( ( C[ 0 ][ 2 ] * 2 + C[ 1 ][ 2 ] ) / 3 ) ;
						C[ 2 ][ 3 ] = 255 ;

						C[ 3 ][ 0 ] = ( BYTE )( ( C[ 0 ][ 0 ] + C[ 1 ][ 0 ] * 2 ) / 3 ) ;
						C[ 3 ][ 1 ] = ( BYTE )( ( C[ 0 ][ 1 ] + C[ 1 ][ 1 ] * 2 ) / 3 ) ;
						C[ 3 ][ 2 ] = ( BYTE )( ( C[ 0 ][ 2 ] + C[ 1 ][ 2 ] * 2 ) / 3 ) ;
						C[ 3 ][ 3 ] = 255 ;

						code = *( ( DWORD * )( srcd + 4 ) ) ;
						dstd[ 0 ] = *( ( DWORD * )C[ ( code >>  0 ) & 3 ] ) ;
						dstd[ 1 ] = *( ( DWORD * )C[ ( code >>  2 ) & 3 ] ) ;
						dstd[ 2 ] = *( ( DWORD * )C[ ( code >>  4 ) & 3 ] ) ;
						dstd[ 3 ] = *( ( DWORD * )C[ ( code >>  6 ) & 3 ] ) ;
						dstd = ( DWORD * )( ( BYTE * )dstd + NewPitch ) ;

						dstd[ 0 ] = *( ( DWORD * )C[ ( code >>  8 ) & 3 ] ) ;
						dstd[ 1 ] = *( ( DWORD * )C[ ( code >> 10 ) & 3 ] ) ;
						dstd[ 2 ] = *( ( DWORD * )C[ ( code >> 12 ) & 3 ] ) ;
						dstd[ 3 ] = *( ( DWORD * )C[ ( code >> 14 ) & 3 ] ) ;
						dstd = ( DWORD * )( ( BYTE * )dstd + NewPitch ) ;

						dstd[ 0 ] = *( ( DWORD * )C[ ( code >> 16 ) & 3 ] ) ;
						dstd[ 1 ] = *( ( DWORD * )C[ ( code >> 18 ) & 3 ] ) ;
						dstd[ 2 ] = *( ( DWORD * )C[ ( code >> 20 ) & 3 ] ) ;
						dstd[ 3 ] = *( ( DWORD * )C[ ( code >> 22 ) & 3 ] ) ;
						dstd = ( DWORD * )( ( BYTE * )dstd + NewPitch ) ;

						dstd[ 0 ] = *( ( DWORD * )C[ ( code >> 24 ) & 3 ] ) ;
						dstd[ 1 ] = *( ( DWORD * )C[ ( code >> 26 ) & 3 ] ) ;
						dstd[ 2 ] = *( ( DWORD * )C[ ( code >> 28 ) & 3 ] ) ;
						dstd[ 3 ] = *( ( DWORD * )C[ ( code >> 30 ) & 3 ] ) ;
					}
					srcd += 8 ;

					// アルファチャンネルのセット
					{
						code = *( ( DWORD * )srcd ) ;
						dstb[  3 ] = ( BYTE )( ( ( code >>  0 ) & 15 ) * 255 / 15 ) ;
						dstb[  7 ] = ( BYTE )( ( ( code >>  4 ) & 15 ) * 255 / 15 ) ;
						dstb[ 11 ] = ( BYTE )( ( ( code >>  8 ) & 15 ) * 255 / 15 ) ;
						dstb[ 15 ] = ( BYTE )( ( ( code >> 12 ) & 15 ) * 255 / 15 ) ;
						dstb = dstb + NewPitch ;

						dstb[  3 ] = ( BYTE )( ( ( code >> 16 ) & 15 ) * 255 / 15 ) ;
						dstb[  7 ] = ( BYTE )( ( ( code >> 20 ) & 15 ) * 255 / 15 ) ;
						dstb[ 11 ] = ( BYTE )( ( ( code >> 24 ) & 15 ) * 255 / 15 ) ;
						dstb[ 15 ] = ( BYTE )( ( ( code >> 28 ) & 15 ) * 255 / 15 ) ;
						dstb = dstb + NewPitch ;

						code = *( ( DWORD * )( srcd + 4 ) ) ;
						dstb[  3 ] = ( BYTE )( ( ( code >>  0 ) & 15 ) * 255 / 15 ) ;
						dstb[  7 ] = ( BYTE )( ( ( code >>  4 ) & 15 ) * 255 / 15 ) ;
						dstb[ 11 ] = ( BYTE )( ( ( code >>  8 ) & 15 ) * 255 / 15 ) ;
						dstb[ 15 ] = ( BYTE )( ( ( code >> 12 ) & 15 ) * 255 / 15 ) ;
						dstb = dstb + NewPitch ;

						dstb[  3 ] = ( BYTE )( ( ( code >> 16 ) & 15 ) * 255 / 15 ) ;
						dstb[  7 ] = ( BYTE )( ( ( code >> 20 ) & 15 ) * 255 / 15 ) ;
						dstb[ 11 ] = ( BYTE )( ( ( code >> 24 ) & 15 ) * 255 / 15 ) ;
						dstb[ 15 ] = ( BYTE )( ( ( code >> 28 ) & 15 ) * 255 / 15 ) ;
					}
					srcd += 8 ;
				}
			}
			break ;

		case DX_BASEIMAGE_FORMAT_DXT4 :
			for( i = 0 ; i < ph ; i ++ )
			{
				for( j = 0 ; j < pw ; j ++ )
				{
					// カラーの処理
					{
						dstb = bdstb + ( j * 4 ) * NewPixelByte + ( i * 4 ) * NewPitch ;
						dstd = ( DWORD * )dstb ;
						C[ 0 ][ 0 ] = ( BYTE )( ( ( ( ( WORD * )srcd )[ 0 ] >>  0 ) & 31 ) * 255 / 31 ) ;
						C[ 0 ][ 1 ] = ( BYTE )( ( ( ( ( WORD * )srcd )[ 0 ] >>  5 ) & 63 ) * 255 / 63 ) ;
						C[ 0 ][ 2 ] = ( BYTE )( ( ( ( ( WORD * )srcd )[ 0 ] >> 11 ) & 31 ) * 255 / 31 ) ;
						C[ 0 ][ 3 ] = 255 ;

						C[ 1 ][ 0 ] = ( BYTE )( ( ( ( ( WORD * )srcd )[ 1 ] >>  0 ) & 31 ) * 255 / 31 ) ;
						C[ 1 ][ 1 ] = ( BYTE )( ( ( ( ( WORD * )srcd )[ 1 ] >>  5 ) & 63 ) * 255 / 63 ) ;
						C[ 1 ][ 2 ] = ( BYTE )( ( ( ( ( WORD * )srcd )[ 1 ] >> 11 ) & 31 ) * 255 / 31 ) ;
						C[ 1 ][ 3 ] = 255 ;

						C[ 2 ][ 0 ] = ( BYTE )( ( C[ 0 ][ 0 ] * 2 + C[ 1 ][ 0 ] ) / 3 ) ;
						C[ 2 ][ 1 ] = ( BYTE )( ( C[ 0 ][ 1 ] * 2 + C[ 1 ][ 1 ] ) / 3 ) ;
						C[ 2 ][ 2 ] = ( BYTE )( ( C[ 0 ][ 2 ] * 2 + C[ 1 ][ 2 ] ) / 3 ) ;
						C[ 2 ][ 3 ] = 255 ;

						C[ 3 ][ 0 ] = ( BYTE )( ( C[ 0 ][ 0 ] + C[ 1 ][ 0 ] * 2 ) / 3 ) ;
						C[ 3 ][ 1 ] = ( BYTE )( ( C[ 0 ][ 1 ] + C[ 1 ][ 1 ] * 2 ) / 3 ) ;
						C[ 3 ][ 2 ] = ( BYTE )( ( C[ 0 ][ 2 ] + C[ 1 ][ 2 ] * 2 ) / 3 ) ;
						C[ 3 ][ 3 ] = 255 ;

						code = *( ( DWORD * )( srcd + 4 ) ) ;
						dstd[ 0 ] = *( ( DWORD * )C[ ( code >>  0 ) & 3 ] ) ;
						dstd[ 1 ] = *( ( DWORD * )C[ ( code >>  2 ) & 3 ] ) ;
						dstd[ 2 ] = *( ( DWORD * )C[ ( code >>  4 ) & 3 ] ) ;
						dstd[ 3 ] = *( ( DWORD * )C[ ( code >>  6 ) & 3 ] ) ;
						dstd = ( DWORD * )( ( BYTE * )dstd + NewPitch ) ;

						dstd[ 0 ] = *( ( DWORD * )C[ ( code >>  8 ) & 3 ] ) ;
						dstd[ 1 ] = *( ( DWORD * )C[ ( code >> 10 ) & 3 ] ) ;
						dstd[ 2 ] = *( ( DWORD * )C[ ( code >> 12 ) & 3 ] ) ;
						dstd[ 3 ] = *( ( DWORD * )C[ ( code >> 14 ) & 3 ] ) ;
						dstd = ( DWORD * )( ( BYTE * )dstd + NewPitch ) ;

						dstd[ 0 ] = *( ( DWORD * )C[ ( code >> 16 ) & 3 ] ) ;
						dstd[ 1 ] = *( ( DWORD * )C[ ( code >> 18 ) & 3 ] ) ;
						dstd[ 2 ] = *( ( DWORD * )C[ ( code >> 20 ) & 3 ] ) ;
						dstd[ 3 ] = *( ( DWORD * )C[ ( code >> 22 ) & 3 ] ) ;
						dstd = ( DWORD * )( ( BYTE * )dstd + NewPitch ) ;

						dstd[ 0 ] = *( ( DWORD * )C[ ( code >> 24 ) & 3 ] ) ;
						dstd[ 1 ] = *( ( DWORD * )C[ ( code >> 26 ) & 3 ] ) ;
						dstd[ 2 ] = *( ( DWORD * )C[ ( code >> 28 ) & 3 ] ) ;
						dstd[ 3 ] = *( ( DWORD * )C[ ( code >> 30 ) & 3 ] ) ;
					}
					srcd += 8 ;

					// アルファチャンネルのセット
					{
						A[ 0 ] = srcd[ 0 ] ;
						A[ 1 ] = srcd[ 1 ] ;
						if( A[ 0 ] > A[ 1 ] )
						{
							A[ 2 ] = ( BYTE )( ( A[ 0 ] * 6 + A[ 1 ] * 1 ) / 7 ) ;
							A[ 3 ] = ( BYTE )( ( A[ 0 ] * 5 + A[ 1 ] * 2 ) / 7 ) ;
							A[ 4 ] = ( BYTE )( ( A[ 0 ] * 4 + A[ 1 ] * 3 ) / 7 ) ;
							A[ 5 ] = ( BYTE )( ( A[ 0 ] * 3 + A[ 1 ] * 4 ) / 7 ) ;
							A[ 6 ] = ( BYTE )( ( A[ 0 ] * 2 + A[ 1 ] * 5 ) / 7 ) ;
							A[ 7 ] = ( BYTE )( ( A[ 0 ] * 1 + A[ 1 ] * 6 ) / 7 ) ;
						}
						else
						{
							A[ 2 ] = ( BYTE )( ( A[ 0 ] * 4 + A[ 1 ] * 1 ) / 5 ) ;
							A[ 3 ] = ( BYTE )( ( A[ 0 ] * 3 + A[ 1 ] * 2 ) / 5 ) ;
							A[ 4 ] = ( BYTE )( ( A[ 0 ] * 2 + A[ 1 ] * 3 ) / 5 ) ;
							A[ 5 ] = ( BYTE )( ( A[ 0 ] * 1 + A[ 1 ] * 4 ) / 5 ) ;
							A[ 6 ] = 0 ;
							A[ 7 ] = 255 ;
						}

						code = ( DWORD )( srcd[ 2 ] | ( srcd[ 3 ] << 8 ) | ( srcd[ 4 ] << 16 ) ) ;
						dstb[  3 ] = A[ ( code >>  0 ) & 7 ] ;
						dstb[  7 ] = A[ ( code >>  3 ) & 7 ] ;
						dstb[ 11 ] = A[ ( code >>  6 ) & 7 ] ;
						dstb[ 15 ] = A[ ( code >>  9 ) & 7 ] ;
						dstb = dstb + NewPitch ;

						dstb[  3 ] = A[ ( code >> 12 ) & 7 ] ;
						dstb[  7 ] = A[ ( code >> 15 ) & 7 ] ;
						dstb[ 11 ] = A[ ( code >> 18 ) & 7 ] ;
						dstb[ 15 ] = A[ ( code >> 21 ) & 7 ] ;
						dstb = dstb + NewPitch ;

						code = ( DWORD )( srcd[ 5 ] | ( srcd[ 6 ] << 8 ) | ( srcd[ 7 ] << 16 ) ) ;
						dstb[  3 ] = A[ ( code >>  0 ) & 7 ] ;
						dstb[  7 ] = A[ ( code >>  3 ) & 7 ] ;
						dstb[ 11 ] = A[ ( code >>  6 ) & 7 ] ;
						dstb[ 15 ] = A[ ( code >>  9 ) & 7 ] ;
						dstb = dstb + NewPitch ;

						dstb[  3 ] = A[ ( code >> 12 ) & 7 ] ;
						dstb[  7 ] = A[ ( code >> 15 ) & 7 ] ;
						dstb[ 11 ] = A[ ( code >> 18 ) & 7 ] ;
						dstb[ 15 ] = A[ ( code >> 21 ) & 7 ] ;
					}
					srcd += 8 ;

					// アルファチャンネルの反適応
					{
						dstb -= NewPitch * 3 ;
						dstb[  0 ] = ( BYTE )( dstb[  0 ] * 255 / dstb[  3 ] ) ;
						dstb[  1 ] = ( BYTE )( dstb[  1 ] * 255 / dstb[  3 ] ) ;
						dstb[  2 ] = ( BYTE )( dstb[  2 ] * 255 / dstb[  3 ] ) ;

						dstb[  4 ] = ( BYTE )( dstb[  4 ] * 255 / dstb[  7 ] ) ;
						dstb[  5 ] = ( BYTE )( dstb[  5 ] * 255 / dstb[  7 ] ) ;
						dstb[  6 ] = ( BYTE )( dstb[  6 ] * 255 / dstb[  7 ] ) ;

						dstb[  8 ] = ( BYTE )( dstb[  8 ] * 255 / dstb[ 11 ] ) ;
						dstb[  9 ] = ( BYTE )( dstb[  9 ] * 255 / dstb[ 11 ] ) ;
						dstb[ 10 ] = ( BYTE )( dstb[ 10 ] * 255 / dstb[ 11 ] ) ;

						dstb[ 12 ] = ( BYTE )( dstb[ 12 ] * 255 / dstb[ 15 ] ) ;
						dstb[ 13 ] = ( BYTE )( dstb[ 13 ] * 255 / dstb[ 15 ] ) ;
						dstb[ 14 ] = ( BYTE )( dstb[ 14 ] * 255 / dstb[ 15 ] ) ;
						dstb = dstb + NewPitch ;

						dstb[  0 ] = ( BYTE )( dstb[  0 ] * 255 / dstb[  3 ] ) ;
						dstb[  1 ] = ( BYTE )( dstb[  1 ] * 255 / dstb[  3 ] ) ;
						dstb[  2 ] = ( BYTE )( dstb[  2 ] * 255 / dstb[  3 ] ) ;

						dstb[  4 ] = ( BYTE )( dstb[  4 ] * 255 / dstb[  7 ] ) ;
						dstb[  5 ] = ( BYTE )( dstb[  5 ] * 255 / dstb[  7 ] ) ;
						dstb[  6 ] = ( BYTE )( dstb[  6 ] * 255 / dstb[  7 ] ) ;

						dstb[  8 ] = ( BYTE )( dstb[  8 ] * 255 / dstb[ 11 ] ) ;
						dstb[  9 ] = ( BYTE )( dstb[  9 ] * 255 / dstb[ 11 ] ) ;
						dstb[ 10 ] = ( BYTE )( dstb[ 10 ] * 255 / dstb[ 11 ] ) ;

						dstb[ 12 ] = ( BYTE )( dstb[ 12 ] * 255 / dstb[ 15 ] ) ;
						dstb[ 13 ] = ( BYTE )( dstb[ 13 ] * 255 / dstb[ 15 ] ) ;
						dstb[ 14 ] = ( BYTE )( dstb[ 14 ] * 255 / dstb[ 15 ] ) ;
						dstb = dstb + NewPitch ;

						dstb[  0 ] = ( BYTE )( dstb[  0 ] * 255 / dstb[  3 ] ) ;
						dstb[  1 ] = ( BYTE )( dstb[  1 ] * 255 / dstb[  3 ] ) ;
						dstb[  2 ] = ( BYTE )( dstb[  2 ] * 255 / dstb[  3 ] ) ;

						dstb[  4 ] = ( BYTE )( dstb[  4 ] * 255 / dstb[  7 ] ) ;
						dstb[  5 ] = ( BYTE )( dstb[  5 ] * 255 / dstb[  7 ] ) ;
						dstb[  6 ] = ( BYTE )( dstb[  6 ] * 255 / dstb[  7 ] ) ;

						dstb[  8 ] = ( BYTE )( dstb[  8 ] * 255 / dstb[ 11 ] ) ;
						dstb[  9 ] = ( BYTE )( dstb[  9 ] * 255 / dstb[ 11 ] ) ;
						dstb[ 10 ] = ( BYTE )( dstb[ 10 ] * 255 / dstb[ 11 ] ) ;

						dstb[ 12 ] = ( BYTE )( dstb[ 12 ] * 255 / dstb[ 15 ] ) ;
						dstb[ 13 ] = ( BYTE )( dstb[ 13 ] * 255 / dstb[ 15 ] ) ;
						dstb[ 14 ] = ( BYTE )( dstb[ 14 ] * 255 / dstb[ 15 ] ) ;
						dstb = dstb + NewPitch ;

						dstb[  0 ] = ( BYTE )( dstb[  0 ] * 255 / dstb[  3 ] ) ;
						dstb[  1 ] = ( BYTE )( dstb[  1 ] * 255 / dstb[  3 ] ) ;
						dstb[  2 ] = ( BYTE )( dstb[  2 ] * 255 / dstb[  3 ] ) ;

						dstb[  4 ] = ( BYTE )( dstb[  4 ] * 255 / dstb[  7 ] ) ;
						dstb[  5 ] = ( BYTE )( dstb[  5 ] * 255 / dstb[  7 ] ) ;
						dstb[  6 ] = ( BYTE )( dstb[  6 ] * 255 / dstb[  7 ] ) ;

						dstb[  8 ] = ( BYTE )( dstb[  8 ] * 255 / dstb[ 11 ] ) ;
						dstb[  9 ] = ( BYTE )( dstb[  9 ] * 255 / dstb[ 11 ] ) ;
						dstb[ 10 ] = ( BYTE )( dstb[ 10 ] * 255 / dstb[ 11 ] ) ;

						dstb[ 12 ] = ( BYTE )( dstb[ 12 ] * 255 / dstb[ 15 ] ) ;
						dstb[ 13 ] = ( BYTE )( dstb[ 13 ] * 255 / dstb[ 15 ] ) ;
						dstb[ 14 ] = ( BYTE )( dstb[ 14 ] * 255 / dstb[ 15 ] ) ;
					}
				}
			}
			break ;

		case DX_BASEIMAGE_FORMAT_DXT5 :
			for( i = 0 ; i < ph ; i ++ )
			{
				for( j = 0 ; j < pw ; j ++ )
				{
					// カラーの処理
					{
						dstb = bdstb + ( j * 4 ) * NewPixelByte + ( i * 4 ) * NewPitch ;
						dstd = ( DWORD * )dstb ;
						C[ 0 ][ 0 ] = ( BYTE )( ( ( ( ( WORD * )srcd )[ 0 ] >>  0 ) & 31 ) * 255 / 31 ) ;
						C[ 0 ][ 1 ] = ( BYTE )( ( ( ( ( WORD * )srcd )[ 0 ] >>  5 ) & 63 ) * 255 / 63 ) ;
						C[ 0 ][ 2 ] = ( BYTE )( ( ( ( ( WORD * )srcd )[ 0 ] >> 11 ) & 31 ) * 255 / 31 ) ;
						C[ 0 ][ 3 ] = 255 ;

						C[ 1 ][ 0 ] = ( BYTE )( ( ( ( ( WORD * )srcd )[ 1 ] >>  0 ) & 31 ) * 255 / 31 ) ;
						C[ 1 ][ 1 ] = ( BYTE )( ( ( ( ( WORD * )srcd )[ 1 ] >>  5 ) & 63 ) * 255 / 63 ) ;
						C[ 1 ][ 2 ] = ( BYTE )( ( ( ( ( WORD * )srcd )[ 1 ] >> 11 ) & 31 ) * 255 / 31 ) ;
						C[ 1 ][ 3 ] = 255 ;

						C[ 2 ][ 0 ] = ( BYTE )( ( C[ 0 ][ 0 ] * 2 + C[ 1 ][ 0 ] ) / 3 ) ;
						C[ 2 ][ 1 ] = ( BYTE )( ( C[ 0 ][ 1 ] * 2 + C[ 1 ][ 1 ] ) / 3 ) ;
						C[ 2 ][ 2 ] = ( BYTE )( ( C[ 0 ][ 2 ] * 2 + C[ 1 ][ 2 ] ) / 3 ) ;
						C[ 2 ][ 3 ] = 255 ;

						C[ 3 ][ 0 ] = ( BYTE )( ( C[ 0 ][ 0 ] + C[ 1 ][ 0 ] * 2 ) / 3 ) ;
						C[ 3 ][ 1 ] = ( BYTE )( ( C[ 0 ][ 1 ] + C[ 1 ][ 1 ] * 2 ) / 3 ) ;
						C[ 3 ][ 2 ] = ( BYTE )( ( C[ 0 ][ 2 ] + C[ 1 ][ 2 ] * 2 ) / 3 ) ;
						C[ 3 ][ 3 ] = 255 ;

						code = *( ( DWORD * )( srcd + 4 ) ) ;
						dstd[ 0 ] = *( ( DWORD * )C[ ( code >>  0 ) & 3 ] ) ;
						dstd[ 1 ] = *( ( DWORD * )C[ ( code >>  2 ) & 3 ] ) ;
						dstd[ 2 ] = *( ( DWORD * )C[ ( code >>  4 ) & 3 ] ) ;
						dstd[ 3 ] = *( ( DWORD * )C[ ( code >>  6 ) & 3 ] ) ;
						dstd = ( DWORD * )( ( BYTE * )dstd + NewPitch ) ;

						dstd[ 0 ] = *( ( DWORD * )C[ ( code >>  8 ) & 3 ] ) ;
						dstd[ 1 ] = *( ( DWORD * )C[ ( code >> 10 ) & 3 ] ) ;
						dstd[ 2 ] = *( ( DWORD * )C[ ( code >> 12 ) & 3 ] ) ;
						dstd[ 3 ] = *( ( DWORD * )C[ ( code >> 14 ) & 3 ] ) ;
						dstd = ( DWORD * )( ( BYTE * )dstd + NewPitch ) ;

						dstd[ 0 ] = *( ( DWORD * )C[ ( code >> 16 ) & 3 ] ) ;
						dstd[ 1 ] = *( ( DWORD * )C[ ( code >> 18 ) & 3 ] ) ;
						dstd[ 2 ] = *( ( DWORD * )C[ ( code >> 20 ) & 3 ] ) ;
						dstd[ 3 ] = *( ( DWORD * )C[ ( code >> 22 ) & 3 ] ) ;
						dstd = ( DWORD * )( ( BYTE * )dstd + NewPitch ) ;

						dstd[ 0 ] = *( ( DWORD * )C[ ( code >> 24 ) & 3 ] ) ;
						dstd[ 1 ] = *( ( DWORD * )C[ ( code >> 26 ) & 3 ] ) ;
						dstd[ 2 ] = *( ( DWORD * )C[ ( code >> 28 ) & 3 ] ) ;
						dstd[ 3 ] = *( ( DWORD * )C[ ( code >> 30 ) & 3 ] ) ;
					}
					srcd += 8 ;

					// アルファチャンネルのセット
					{
						A[ 0 ] = srcd[ 0 ] ;
						A[ 1 ] = srcd[ 1 ] ;
						if( A[ 0 ] > A[ 1 ] )
						{
							A[ 2 ] = ( BYTE )( ( A[ 0 ] * 6 + A[ 1 ] * 1 ) / 7 ) ;
							A[ 3 ] = ( BYTE )( ( A[ 0 ] * 5 + A[ 1 ] * 2 ) / 7 ) ;
							A[ 4 ] = ( BYTE )( ( A[ 0 ] * 4 + A[ 1 ] * 3 ) / 7 ) ;
							A[ 5 ] = ( BYTE )( ( A[ 0 ] * 3 + A[ 1 ] * 4 ) / 7 ) ;
							A[ 6 ] = ( BYTE )( ( A[ 0 ] * 2 + A[ 1 ] * 5 ) / 7 ) ;
							A[ 7 ] = ( BYTE )( ( A[ 0 ] * 1 + A[ 1 ] * 6 ) / 7 ) ;
						}
						else
						{
							A[ 2 ] = ( BYTE )( ( A[ 0 ] * 4 + A[ 1 ] * 1 ) / 5 ) ;
							A[ 3 ] = ( BYTE )( ( A[ 0 ] * 3 + A[ 1 ] * 2 ) / 5 ) ;
							A[ 4 ] = ( BYTE )( ( A[ 0 ] * 2 + A[ 1 ] * 3 ) / 5 ) ;
							A[ 5 ] = ( BYTE )( ( A[ 0 ] * 1 + A[ 1 ] * 4 ) / 5 ) ;
							A[ 6 ] = 0 ;
							A[ 7 ] = 255 ;
						}

						code = ( DWORD )( srcd[ 2 ] | ( srcd[ 3 ] << 8 ) | ( srcd[ 4 ] << 16 ) ) ;
						dstb[  3 ] = A[ ( code >>  0 ) & 7 ] ;
						dstb[  7 ] = A[ ( code >>  3 ) & 7 ] ;
						dstb[ 11 ] = A[ ( code >>  6 ) & 7 ] ;
						dstb[ 15 ] = A[ ( code >>  9 ) & 7 ] ;
						dstb = dstb + NewPitch ;

						dstb[  3 ] = A[ ( code >> 12 ) & 7 ] ;
						dstb[  7 ] = A[ ( code >> 15 ) & 7 ] ;
						dstb[ 11 ] = A[ ( code >> 18 ) & 7 ] ;
						dstb[ 15 ] = A[ ( code >> 21 ) & 7 ] ;
						dstb = dstb + NewPitch ;

						code = ( DWORD )( srcd[ 5 ] | ( srcd[ 6 ] << 8 ) | ( srcd[ 7 ] << 16 ) ) ;
						dstb[  3 ] = A[ ( code >>  0 ) & 7 ] ;
						dstb[  7 ] = A[ ( code >>  3 ) & 7 ] ;
						dstb[ 11 ] = A[ ( code >>  6 ) & 7 ] ;
						dstb[ 15 ] = A[ ( code >>  9 ) & 7 ] ;
						dstb = dstb + NewPitch ;

						dstb[  3 ] = A[ ( code >> 12 ) & 7 ] ;
						dstb[  7 ] = A[ ( code >> 15 ) & 7 ] ;
						dstb[ 11 ] = A[ ( code >> 18 ) & 7 ] ;
						dstb[ 15 ] = A[ ( code >> 21 ) & 7 ] ;
					}
					srcd += 8 ;
				}
			}
			break ;
		}
	}

	// 今までのデータを解放して新しいデータのポインタをセット
	if( ReleaseOrigGraphData )
	{
		DXFREE( BaseImage->GraphData ) ;
	}
	BaseImage->GraphData = NewBuffer ;

	// カラーデータは ARGB8 にする
	NS_CreateARGB8ColorData( &BaseImage->ColorData ) ;

	// ピッチも新しい画像でのピッチをセットする
	BaseImage->Pitch = ( int )NewPitch ;

	// 正常終了
	return 0 ;
}

// 通常のαチャンネル付き画像を乗算済みαチャンネル付き画像に変換する( ピクセルフォーマットが ARGB8 以外の場合は ARGB8 に変換されます )
extern int NS_ConvertPremulAlphaBaseImage( BASEIMAGE *BaseImage )
{
	DWORD i ;
	DWORD j ;
	BYTE *p ;

	// αチャンネルの無い画像フォーマットの場合は何もしない
	if( BaseImage->ColorData.AlphaWidth == 0 )
		return 0 ;

	// フォーマットが標準フォーマットではなかったら標準フォーマットに変換する
	if( BaseImage->ColorData.Format != DX_BASEIMAGE_FORMAT_NORMAL )
	{
		if( NS_ConvertNormalFormatBaseImage( BaseImage ) < 0 )
			return -1 ;
	}

	// ARGB8 以外の場合も ARGB8 に変換する
	if( BaseImage->ColorData.ColorBitDepth != 32 ||
		BaseImage->ColorData.PixelByte != 4 ||
		BaseImage->ColorData.FloatTypeFlag != FALSE ||
		BaseImage->ColorData.NoneMask  != 0x00000000 ||
		BaseImage->ColorData.AlphaMask != 0xff000000 ||
		BaseImage->ColorData.RedMask   != 0x00ff0000 ||
		BaseImage->ColorData.GreenMask != 0x0000ff00 ||
		BaseImage->ColorData.BlueMask  != 0x000000ff )
	{
		BASEIMAGE TempBaseImage ;
		TempBaseImage = *BaseImage ;
		NS_CreateARGB8ColorBaseImage( BaseImage->Width, BaseImage->Height, BaseImage ) ;
		NS_BltBaseImage( 0, 0, &TempBaseImage, BaseImage ) ;
		NS_ReleaseBaseImage( &TempBaseImage ) ;
	}

	for( i = 0 ; i < ( DWORD )BaseImage->Height ; i ++ )
	{
		p = ( BYTE * )BaseImage->GraphData + i * BaseImage->Pitch ;
		for( j = 0 ; j < ( DWORD )BaseImage->Width ; j ++, p += 4 )
		{
			p[ 0 ] = BASEIM.RgbToPmaTable[ p[ 0 ] ][ p[ 3 ] ] ;
			p[ 1 ] = BASEIM.RgbToPmaTable[ p[ 1 ] ][ p[ 3 ] ] ;
			p[ 2 ] = BASEIM.RgbToPmaTable[ p[ 2 ] ][ p[ 3 ] ] ;
//			p[ 0 ] = ( BYTE )( p[ 0 ] * p[ 3 ] / 255 ) ;
//			p[ 1 ] = ( BYTE )( p[ 1 ] * p[ 3 ] / 255 ) ;
//			p[ 2 ] = ( BYTE )( p[ 2 ] * p[ 3 ] / 255 ) ;
		}
	}

	// 正常終了
	return 0 ;
}

// 乗算済みαチャンネル付き画像を通常のαチャンネル付き画像に変換する( ピクセルフォーマットが ARGB8 以外の場合は ARGB8 に変換されます )
extern int NS_ConvertInterpAlphaBaseImage( BASEIMAGE *BaseImage )
{
	DWORD i ;
	DWORD j ;
//	DWORD tmp ;
	BYTE *p ;

	// αチャンネルの無い画像フォーマットの場合は何もしない
	if( BaseImage->ColorData.AlphaWidth == 0 )
		return 0 ;

	// フォーマットが標準フォーマットではなかったら標準フォーマットに変換する
	if( BaseImage->ColorData.Format != DX_BASEIMAGE_FORMAT_NORMAL )
	{
		if( NS_ConvertNormalFormatBaseImage( BaseImage ) < 0 )
			return -1 ;
	}
	else
	{
		// ARGB8 以外の場合も ARGB8 に変換する
		if( BaseImage->ColorData.ColorBitDepth != 32 ||
			BaseImage->ColorData.PixelByte != 4 ||
			BaseImage->ColorData.FloatTypeFlag != FALSE ||
			BaseImage->ColorData.NoneMask  != 0x00000000 ||
			BaseImage->ColorData.AlphaMask != 0xff000000 ||
			BaseImage->ColorData.RedMask   != 0x00ff0000 ||
			BaseImage->ColorData.GreenMask != 0x0000ff00 ||
			BaseImage->ColorData.BlueMask  != 0x000000ff )
		{
			BASEIMAGE TempBaseImage ;
			TempBaseImage = *BaseImage ;
			NS_CreateARGB8ColorBaseImage( BaseImage->Width, BaseImage->Height, BaseImage ) ;
			NS_BltBaseImage( 0, 0, &TempBaseImage, BaseImage ) ;
			NS_ReleaseBaseImage( &TempBaseImage ) ;
		}
	}

	for( i = 0 ; i < ( DWORD )BaseImage->Height ; i ++ )
	{
		p = ( BYTE * )BaseImage->GraphData + i * BaseImage->Pitch ;
		for( j = 0 ; j < ( DWORD )BaseImage->Width ; j ++, p += 4 )
		{
			p[ 0 ] = BASEIM.PmaToRgbTable[ p[ 0 ] ][ p[ 3 ] ] ;
			p[ 1 ] = BASEIM.PmaToRgbTable[ p[ 1 ] ][ p[ 3 ] ] ;
			p[ 2 ] = BASEIM.PmaToRgbTable[ p[ 2 ] ][ p[ 3 ] ] ;
//			if( p[ 3 ] == 0 )
//			{
//				p[ 0 ] = 0 ;
//				p[ 1 ] = 0 ;
//				p[ 2 ] = 0 ;
//			}
//			else
//			{
//				tmp = ( DWORD )( p[ 0 ] * 255 / p[ 3 ] ) ; p[ 0 ] = ( BYTE )( tmp > 255 ? 255 : tmp ) ;
//				tmp = ( DWORD )( p[ 1 ] * 255 / p[ 3 ] ) ; p[ 1 ] = ( BYTE )( tmp > 255 ? 255 : tmp ) ;
//				tmp = ( DWORD )( p[ 2 ] * 255 / p[ 3 ] ) ; p[ 2 ] = ( BYTE )( tmp > 255 ? 255 : tmp ) ;
//			}
		}
	}

	// 正常終了
	return 0 ;
}

#ifdef __WINDOWS__

// 基本イメージを使用して UpdateLayerdWindow を行う
extern int NS_UpdateLayerdWindowForBaseImage( const BASEIMAGE *BaseImage )
{
	return UpdateBackBufferTransColorWindow( BaseImage, NULL, NULL, TRUE, FALSE ) ;
}

// 基本イメージデータの指定の範囲を使用して UpdateLayerdWindow を行う
extern int NS_UpdateLayerdWindowForBaseImageRect( const BASEIMAGE *BaseImage, int x1, int y1, int x2, int y2 )
{
	RECT SrcRect ;

	SrcRect.left   = x1 ;
	SrcRect.top    = y1 ;
	SrcRect.right  = x2 ;
	SrcRect.bottom = y2 ;

	return UpdateBackBufferTransColorWindow( BaseImage, &SrcRect, NULL, TRUE, FALSE ) ;
}

// 乗算済みアルファの基本イメージを使用して UpdateLayerdWindow を行う
extern int NS_UpdateLayerdWindowForPremultipliedAlphaBaseImage( const BASEIMAGE *BaseImage )
{
	return UpdateBackBufferTransColorWindow( BaseImage, NULL, NULL, TRUE, TRUE ) ;
}

// デスクトップキャプチャ

// デスクトップの指定の領域を基本イメージデータに転送する
extern int NS_GetDesktopScreenBaseImage( int x1, int y1, int x2, int y2, BASEIMAGE *BaseImage, int DestX, int DestY )
{
	HWND DesktopHWND ;
	RECT DesktopRect ;
	int DesktopW, DesktopH ;
	int CaptureW, CaptureH ;
	HDC hdc ;
	BITMAPINFO BitmapInfo ;
	void *ImageBuffer = NULL ;
	HBITMAP HBitmap ;
	HDC HMemDC ;
	BASEIMAGE CaptureImage ;

	// 引数のチェック
	if( x1 >= x2 || y1 >= y2 )
	{
		return -1 ;
	}

	SETUP_WIN_API

	// デスクトップウインドウハンドルを取得
	DesktopHWND = WinAPIData.Win32Func.GetDesktopWindowFunc() ;

	// デスクトップの矩形を取得
	WinAPIData.Win32Func.GetWindowRectFunc( DesktopHWND, &DesktopRect ) ;

	// デスクトップのサイズを取得
	DesktopW = DesktopRect.right  - DesktopRect.left ;
	DesktopH = DesktopRect.bottom - DesktopRect.top ;

	// 指定の矩形の補正
	if( x1 < DesktopRect.left )
	{
		DestX += DesktopRect.left - x1 ;
		x1     = DesktopRect.left ;
	}
	if( y1 < DesktopRect.top )
	{
		DestY += DesktopRect.top - y1 ;
		y1     = DesktopRect.top ;
	}
	if( x2 > DesktopRect.right )
	{
		x2 = DesktopRect.right ;
	}
	if( y2 > DesktopRect.bottom )
	{
		y2 = DesktopRect.bottom ;
	}

	// 取り込みサイズの算出
	CaptureW = x2 - x1 ;
	CaptureH = y2 - y1 ;

	// 取り込みサイズチェック
	if( CaptureW <= 0 || CaptureH <= 0 )
	{
		return -1 ;
	}

	// 取り込みサイズ補正
	if( DestX + CaptureW > BaseImage->Width )
	{
		CaptureW = BaseImage->Width - DestX ;
		x2       = x1 + CaptureW ;
	}
	if( DestY + CaptureH > BaseImage->Height )
	{
		CaptureH = BaseImage->Height - DestY ;
		y2       = y1 + CaptureH ;
	}

	// 再度取り込みサイズチェック
	if( CaptureW <= 0 || CaptureH <= 0 )
	{
		return -1 ;
	}

	// DIBの情報を設定する
	_MEMSET( &BitmapInfo, 0, sizeof( BitmapInfo ) ) ;
	BitmapInfo.bmiHeader.biSize			= sizeof( BITMAPINFOHEADER ) ;
	BitmapInfo.bmiHeader.biWidth		=  CaptureW ;
	BitmapInfo.bmiHeader.biHeight		= -CaptureH ;
	BitmapInfo.bmiHeader.biPlanes		= 1 ;
	BitmapInfo.bmiHeader.biBitCount		= 32 ;
	BitmapInfo.bmiHeader.biCompression	= BI_RGB ;

	// DIBの作成
	hdc			= WinAPIData.Win32Func.GetDCFunc( GetMainWindowHandle() ) ;
	HBitmap		= WinAPIData.Win32Func.CreateDIBSectionFunc( hdc, &BitmapInfo, DIB_RGB_COLORS, ( void ** )&ImageBuffer, NULL, 0 ) ;
	HMemDC		= WinAPIData.Win32Func.CreateCompatibleDCFunc( hdc ) ;
	WinAPIData.Win32Func.SelectObjectFunc( HMemDC, HBitmap ) ;
	WinAPIData.Win32Func.ReleaseDCFunc( GetMainWindowHandle(), hdc ) ;

	// デスクトップイメージををDIBにコピー
	hdc = WinAPIData.Win32Func.GetDCFunc( DesktopHWND ) ;
	WinAPIData.Win32Func.BitBltFunc( HMemDC, 0, 0, CaptureW, CaptureH, hdc, x1, y1, SRCCOPY ) ;
	WinAPIData.Win32Func.ReleaseDCFunc( DesktopHWND, hdc ) ;

	// デスクトップイメージを基本イメージデータに転送
	_MEMSET( &CaptureImage, 0, sizeof( CaptureImage ) ) ;
	NS_CreateXRGB8ColorData( &CaptureImage.ColorData ) ;
	CaptureImage.Width		= CaptureW ;
	CaptureImage.Height		= CaptureH ;
	CaptureImage.Pitch		= CaptureW * 4 ;
	CaptureImage.GraphData	= ( void * )ImageBuffer ;
	NS_BltBaseImage( DestX, DestY, &CaptureImage, BaseImage ) ;

	// DIBの削除
	WinAPIData.Win32Func.DeleteDCFunc( HMemDC ) ;
	WinAPIData.Win32Func.DeleteObjectFunc( HBitmap ) ;

	// 終了
	return 0 ;
}

// 乗算済みアルファの基本イメージデータの指定の範囲を使用して UpdateLayerdWindow を行う
extern int NS_UpdateLayerdWindowForPremultipliedAlphaBaseImageRect( const BASEIMAGE *BaseImage, int x1, int y1, int x2, int y2 )
{
	RECT SrcRect ;

	SrcRect.left   = x1 ;
	SrcRect.top    = y1 ;
	SrcRect.right  = x2 ;
	SrcRect.bottom = y2 ;

	return UpdateBackBufferTransColorWindow( BaseImage, &SrcRect, NULL, TRUE, TRUE ) ;
}

#endif // __WINDOWS__

// 基本イメージデータを指定の色で塗りつぶす
extern int NS_FillBaseImage( BASEIMAGE *BaseImage, int r, int g, int b, int a )
{
	unsigned int Color;
	int i, j, w, h, addpitch;
	BYTE *p;

	Color = NS_GetColor3( &BaseImage->ColorData, r, g, b, a );
	w = BaseImage->Width;
	h = BaseImage->Height;
	p = (BYTE *)BaseImage->GraphData;
	addpitch = BaseImage->Pitch - w * BaseImage->ColorData.PixelByte;
	switch( BaseImage->ColorData.PixelByte )
	{
	case 1 :
		for( i = 0; i < h; i ++, p += addpitch )
		{
			for( j = 0; j < w; j ++, p ++ )
			{
				*((BYTE *)p) = (BYTE)r;
			}
		}
		break ;

	case 2:
		for( i = 0; i < h; i ++, p += addpitch )
		{
			for( j = 0; j < w; j ++, p += 2 )
			{
				*((WORD *)p) = (WORD)Color;
			}
		}
		break;

	case 3:
		for( i = 0; i < h; i ++, p += addpitch )
		{
			for( j = 0; j < w; j ++, p += 3 )
			{
				((WORD *)p)[0] = ((WORD *)&Color)[0];
				((BYTE *)p)[2] = ((BYTE *)&Color)[2];
			}
		}
		break;

	case 4:
		for( i = 0; i < h; i ++, p += addpitch )
		{
			for( j = 0; j < w; j ++, p += 4 )
			{
				*((DWORD *)p) = (DWORD)Color;
			}
		}
		break;
	}

	// 終了
	return 0;
}

// 基本イメージデータの指定の領域を指定の色で塗りつぶす
extern int NS_FillRectBaseImage( BASEIMAGE *BaseImage, int x, int y, int w, int h, int r, int g, int b, int a )
{
	DWORD	Color;
	int		i ;
	int		j ;
	int		addpitch;
	BYTE *	p;

	if( x >= BaseImage->Width  || w <= 0 ||
		y >= BaseImage->Height || h <= 0 )
	{
		return -1 ;
	}

	if( x < 0 )
	{
		w += x ;
		x = 0 ;
	}
	else
	if( x + w > BaseImage->Width )
	{
		w = BaseImage->Width - x ;
	}

	if( y < 0 )
	{
		h += y ;
		y = 0 ;
	}
	else
	if( y + h > BaseImage->Height )
	{
		h = BaseImage->Height - y ;
	}

	Color    = BaseImage->ColorData.PixelByte == 1 ? 0 : NS_GetColor3( &BaseImage->ColorData, r, g, b, a ) ;
	p        = ( BYTE * )BaseImage->GraphData + x * BaseImage->ColorData.PixelByte + y * BaseImage->Pitch ;
	addpitch = BaseImage->Pitch - w * BaseImage->ColorData.PixelByte ;
	switch( BaseImage->ColorData.PixelByte )
	{
	case 1:
		for( i = 0; i < h; i ++, p += addpitch )
		{
			for( j = 0; j < w; j ++, p ++ )
			{
				*p = ( BYTE )r ;
			}
		}
		break;

	case 2:
		for( i = 0; i < h; i ++, p += addpitch )
		{
			for( j = 0; j < w; j ++, p += 2 )
			{
				*( ( WORD * )p ) = ( WORD )Color ;
			}
		}
		break;

	case 3:
		for( i = 0; i < h; i ++, p += addpitch )
		{
			for( j = 0; j < w; j ++, p += 3 )
			{
				( ( WORD * )p )[ 0 ] = ( ( WORD * )&Color )[ 0 ] ;
				( ( BYTE * )p )[ 2 ] = ( ( BYTE * )&Color )[ 2 ] ;
			}
		}
		break;

	case 4:
		for( i = 0; i < h; i ++, p += addpitch )
		{
			for( j = 0; j < w; j ++, p += 4 )
			{
				*( ( DWORD * )p ) = ( DWORD )Color ;
			}
		}
		break;
	}

	// 終了
	return 0;
}

// 基本イメージデータの指定の領域を０クリアする
extern int NS_ClearRectBaseImage( BASEIMAGE *BaseImage, int x, int y, int w, int h )
{
	int i, FillByte, Pitch ;
	BYTE *Dest ;

	if( x >= BaseImage->Width  || w <= 0 ||
		y >= BaseImage->Height || h <= 0 )
	{
		return -1 ;
	}

	if( x < 0 )
	{
		w += x ;
		x = 0 ;
	}
	else
	if( x + w > BaseImage->Width )
	{
		w = BaseImage->Width - x ;
	}

	if( y < 0 )
	{
		h += y ;
		y = 0 ;
	}
	else
	if( y + h > BaseImage->Height )
	{
		h = BaseImage->Height - y ;
	}

	Pitch = BaseImage->Pitch ;
	FillByte = w * BaseImage->ColorData.PixelByte ;
	Dest = ( BYTE * )BaseImage->GraphData + x * BaseImage->ColorData.PixelByte + y * Pitch ;
	for( i = h ; i > 0 ; i --, Dest += Pitch )
	{
		_MEMSET( Dest, 0, ( size_t )FillByte ) ;
	}

	// 正常終了
	return 0 ;
}

// 基本イメージデータのパレットを取得する
extern int NS_GetPaletteBaseImage( const BASEIMAGE *BaseImage, int PaletteNo, int *r, int *g, int *b, int *a )
{
	const COLORPALETTEDATA *Palette ;
	
	if( PaletteNo < 0 || PaletteNo > 255 ) return -1 ;

	Palette = &BaseImage->ColorData.Palette[ PaletteNo ] ;
	if( r ) *r = Palette->Red ;
	if( g ) *g = Palette->Green ;
	if( b ) *b = Palette->Blue ;
	if( a ) *a = Palette->Alpha ;

	// 終了
	return 0 ;
}

// 基本イメージデータのパレットをセットする
extern int NS_SetPaletteBaseImage( BASEIMAGE *BaseImage, int PaletteNo, int  r, int  g, int  b, int  a )
{
	COLORPALETTEDATA *Palette ;
	
	if( PaletteNo < 0 || PaletteNo > 255 ) return -1 ;

	Palette = &BaseImage->ColorData.Palette[ PaletteNo ] ;
	Palette->Red   = ( unsigned char )r ;
	Palette->Green = ( unsigned char )g ;
	Palette->Blue  = ( unsigned char )b ;
	Palette->Alpha = ( unsigned char )a ;

	// 終了
	return 0 ;
}

// 基本イメージデータの指定の座標の色コードを変更する(パレット画像用)
extern int NS_SetPixelPalCodeBaseImage( BASEIMAGE *BaseImage, int x, int y, int palNo )
{
	unsigned int Color;
	BYTE *p;

	// フォーマットが標準フォーマットではないか、パレット形式ではなかったらエラー
	if( BaseImage->ColorData.Format != DX_BASEIMAGE_FORMAT_NORMAL ||
		BaseImage->ColorData.PixelByte != 1 )
		return -1 ;

	// はみ出し判定
	if( ( DWORD )BaseImage->Width <= ( DWORD )x || ( DWORD )BaseImage->Height <= ( DWORD )y )
		return -1 ;

	Color = ( DWORD )palNo ;
	switch( BaseImage->ColorData.PixelByte )
	{
	case 1:
		*( ( BYTE * )BaseImage->GraphData + BaseImage->Pitch * y + x ) = ( BYTE )Color ;
		break;

	case 2:
		*((WORD *)((BYTE *)BaseImage->GraphData + BaseImage->Pitch * y + x * 2)) =  (WORD)Color;
		break;

	case 3:
		p = (BYTE *)BaseImage->GraphData + BaseImage->Pitch * y + x * 3;
		((WORD *)p)[0] = ((WORD *)&Color)[0];
		((BYTE *)p)[2] = ((BYTE *)&Color)[2];
		break;

	case 4:
		*((DWORD *)((BYTE *)BaseImage->GraphData + BaseImage->Pitch * y + x * 4)) = (DWORD)Color;
		break;
	}

	// 終了
	return 0;
}

// 基本イメージデータの指定の座標の色コードを取得する(パレット画像用)
extern int NS_GetPixelPalCodeBaseImage( const BASEIMAGE *BaseImage, int x, int y )
{
	BYTE *p;

	// フォーマットが標準フォーマットではないか、パレット形式ではなかったらエラー
	if( BaseImage->ColorData.Format != DX_BASEIMAGE_FORMAT_NORMAL ||
		BaseImage->ColorData.PixelByte != 1 )
		return 0xffffffff ;

	// はみ出し判定
	if( ( DWORD )BaseImage->Width <= ( DWORD )x || ( DWORD )BaseImage->Height <= ( DWORD )y )
		return 0xffffffff ;

	p = (BYTE *)BaseImage->GraphData + BaseImage->Pitch * y;
	switch( BaseImage->ColorData.PixelByte )
	{
	case 1:
		return *( p + x ) ;

	case 2:
		return *( ( WORD * )p + x ) ;

	case 3:
		p += x * 3;
		return *( ( WORD * )p ) | ( p[ 2 ] << 16 ) ;

	case 4:
		return ( int )( *( ( DWORD * )p + x ) ) ;
	}

	// 終了
	return 0xffffffff;
}

// 基本イメージデータの指定の座標の色を変更する(各色要素は０〜２５５)
extern int NS_SetPixelBaseImage( BASEIMAGE *BaseImage, int x, int y, int  r, int  g, int  b, int  a )
{
	unsigned int Color;
	BYTE *p;

	// フォーマットが標準フォーマットではなかったら標準フォーマットに変換する
	if( BaseImage->ColorData.Format != DX_BASEIMAGE_FORMAT_NORMAL )
	{
		if( NS_ConvertNormalFormatBaseImage( BaseImage ) < 0 )
			return -1 ;
	}

	// はみ出し判定
	if( ( DWORD )BaseImage->Width <= ( DWORD )x || ( DWORD )BaseImage->Height <= ( DWORD )y )
		return -1 ;

	if( BaseImage->ColorData.FloatTypeFlag )
	{
		switch( BaseImage->ColorData.ChannelBitDepth )
		{
		case 16 :
			p = ( BYTE * )BaseImage->GraphData + BaseImage->Pitch * y + x * 8 ;
			if( BaseImage->ColorData.ChannelNum > 0 )
			{
				( ( WORD * )p )[ 0 ] = Float32ToFloat16( r / 255.0f ) ;
			}
			if( BaseImage->ColorData.ChannelNum > 1 )
			{
				( ( WORD * )p )[ 1 ] = Float32ToFloat16( g / 255.0f ) ;
			}
			if( BaseImage->ColorData.ChannelNum > 2 )
			{
				( ( WORD * )p )[ 2 ] = Float32ToFloat16( b / 255.0f ) ;
			}
			if( BaseImage->ColorData.ChannelNum > 3 )
			{
				( ( WORD * )p )[ 3 ] = Float32ToFloat16( a / 255.0f ) ;
			}
			break ;

		case 32 :
			p = ( BYTE * )BaseImage->GraphData + BaseImage->Pitch * y + x * 16 ;
			if( BaseImage->ColorData.ChannelNum > 0 )
			{
				( ( float * )p )[ 0 ] = r / 255.0f ;
			}
			if( BaseImage->ColorData.ChannelNum > 1 )
			{
				( ( float * )p )[ 1 ] = g / 255.0f ;
			}
			if( BaseImage->ColorData.ChannelNum > 2 )
			{
				( ( float * )p )[ 2 ] = b / 255.0f ;
			}
			if( BaseImage->ColorData.ChannelNum > 3 )
			{
				( ( float * )p )[ 3 ] = a / 255.0f ;
			}
			break ;

		default :
			return -1 ;
		}
	}
	else
	{
		Color = NS_GetColor3( &BaseImage->ColorData, r, g, b, a );
		switch( BaseImage->ColorData.PixelByte )
		{
		case 1:
			*( ( BYTE * )BaseImage->GraphData + BaseImage->Pitch * y + x ) = ( BYTE )Color ;
			break;

		case 2:
			*((WORD *)((BYTE *)BaseImage->GraphData + BaseImage->Pitch * y + x * 2)) =  (WORD)Color;
			break;

		case 3:
			p = (BYTE *)BaseImage->GraphData + BaseImage->Pitch * y + x * 3;
			((WORD *)p)[0] = ((WORD *)&Color)[0];
			((BYTE *)p)[2] = ((BYTE *)&Color)[2];
			break;

		case 4: *((DWORD *)((BYTE *)BaseImage->GraphData + BaseImage->Pitch * y + x * 4)) = (DWORD)Color; break;
		}
	}

	// 終了
	return 0;
}

// 基本イメージデータの指定の座標の色を変更する(各色要素は浮動小数点数)
extern int NS_SetPixelBaseImageF( BASEIMAGE *BaseImage, int x, int y, float  r, float  g, float  b, float  a )
{
	unsigned int Color ;
	BYTE *p ;

	// フォーマットが標準フォーマットではなかったら標準フォーマットに変換する
	if( BaseImage->ColorData.Format != DX_BASEIMAGE_FORMAT_NORMAL )
	{
		if( NS_ConvertNormalFormatBaseImage( BaseImage ) < 0 )
			return -1 ;
	}

	// はみ出し判定
	if( ( DWORD )BaseImage->Width <= ( DWORD )x || ( DWORD )BaseImage->Height <= ( DWORD )y )
		return -1 ;

	if( BaseImage->ColorData.FloatTypeFlag )
	{
		switch( BaseImage->ColorData.ChannelBitDepth )
		{
		case 16 :
			p = ( BYTE * )BaseImage->GraphData + BaseImage->Pitch * y + x * 8 ;
			if( BaseImage->ColorData.ChannelNum > 0 )
			{
				( ( WORD * )p )[ 0 ] = Float32ToFloat16( r ) ;
			}
			if( BaseImage->ColorData.ChannelNum > 1 )
			{
				( ( WORD * )p )[ 1 ] = Float32ToFloat16( g ) ;
			}
			if( BaseImage->ColorData.ChannelNum > 2 )
			{
				( ( WORD * )p )[ 2 ] = Float32ToFloat16( b ) ;
			}
			if( BaseImage->ColorData.ChannelNum > 3 )
			{
				( ( WORD * )p )[ 3 ] = Float32ToFloat16( a ) ;
			}
			break ;

		case 32 :
			p = ( BYTE * )BaseImage->GraphData + BaseImage->Pitch * y + x * 16 ;
			if( BaseImage->ColorData.ChannelNum > 0 )
			{
				( ( float * )p )[ 0 ] = r ;
			}
			if( BaseImage->ColorData.ChannelNum > 1 )
			{
				( ( float * )p )[ 1 ] = g ;
			}
			if( BaseImage->ColorData.ChannelNum > 2 )
			{
				( ( float * )p )[ 2 ] = b ;
			}
			if( BaseImage->ColorData.ChannelNum > 3 )
			{
				( ( float * )p )[ 3 ] = a ;
			}
			break ;

		default :
			return -1 ;
		}
	}
	else
	{
		Color = NS_GetColor3( &BaseImage->ColorData,
			r < 0.0f ? 0 : ( r > 1.0f ? 255 : _FTOL( r * 255.0f ) ),
			g < 0.0f ? 0 : ( g > 1.0f ? 255 : _FTOL( g * 255.0f ) ),
			b < 0.0f ? 0 : ( b > 1.0f ? 255 : _FTOL( b * 255.0f ) ),
			a < 0.0f ? 0 : ( a > 1.0f ? 255 : _FTOL( a * 255.0f ) )
		) ;
		switch( BaseImage->ColorData.PixelByte )
		{
		case 1:
			*( ( BYTE * )BaseImage->GraphData + BaseImage->Pitch * y + x ) = ( BYTE )Color ;
			break;

		case 2:
			*((WORD *)((BYTE *)BaseImage->GraphData + BaseImage->Pitch * y + x * 2)) =  (WORD)Color;
			break;

		case 3:
			p = (BYTE *)BaseImage->GraphData + BaseImage->Pitch * y + x * 3;
			((WORD *)p)[0] = ((WORD *)&Color)[0];
			((BYTE *)p)[2] = ((BYTE *)&Color)[2];
			break;

		case 4: *((DWORD *)((BYTE *)BaseImage->GraphData + BaseImage->Pitch * y + x * 4)) = (DWORD)Color; break;
		}
	}

	// 終了
	return 0;
}

// 基本イメージデータの指定の座標の色を取得する(各色要素は０〜２５５)
extern int NS_GetPixelBaseImage( const BASEIMAGE *BaseImage, int x, int y, int *r, int *g, int *b, int *a )
{
	unsigned int Color;
	const BYTE *p;
	const COLORPALETTEDATA *Pal ;

	// フォーマットが標準フォーマットではなかったら標準フォーマットに変換する
	if( BaseImage->ColorData.Format != DX_BASEIMAGE_FORMAT_NORMAL )
	{
		if( NS_ConvertNormalFormatBaseImage( ( BASEIMAGE * )BaseImage ) < 0 )
			return -1 ;
	}

	// はみ出し判定
	if( ( DWORD )BaseImage->Width <= ( DWORD )x || ( DWORD )BaseImage->Height <= ( DWORD )y )
		return -1 ;

	p = (BYTE *)BaseImage->GraphData + BaseImage->Pitch * y;
	if( BaseImage->ColorData.FloatTypeFlag )
	{
		switch( BaseImage->ColorData.ChannelBitDepth )
		{
		case 16 :
			p += x * 8 ;
			if( BaseImage->ColorData.ChannelNum > 0 )
			{
				if( r ) *r = _FTOL( Float16ToFloat32( ( ( WORD * )p )[ 0 ] ) * 255.0f ) ;
			}
			if( BaseImage->ColorData.ChannelNum > 1 )
			{
				if( g ) *g = _FTOL( Float16ToFloat32( ( ( WORD * )p )[ 1 ] ) * 255.0f ) ;
			}
			if( BaseImage->ColorData.ChannelNum > 2 )
			{
				if( b ) *b = _FTOL( Float16ToFloat32( ( ( WORD * )p )[ 2 ] ) * 255.0f ) ;
			}
			if( BaseImage->ColorData.ChannelNum > 3 )
			{
				if( a ) *a = _FTOL( Float16ToFloat32( ( ( WORD * )p )[ 3 ] ) * 255.0f ) ;
			}
			break ;

		case 32 :
			p += x * 16 ;
			if( BaseImage->ColorData.ChannelNum > 0 )
			{
				if( r ) *r = _FTOL( ( ( float * )p )[ 0 ] * 255.0f ) ;
			}
			if( BaseImage->ColorData.ChannelNum > 1 )
			{
				if( g ) *g = _FTOL( ( ( float * )p )[ 1 ] * 255.0f ) ;
			}
			if( BaseImage->ColorData.ChannelNum > 2 )
			{
				if( b ) *b = _FTOL( ( ( float * )p )[ 2 ] * 255.0f ) ;
			}
			if( BaseImage->ColorData.ChannelNum > 3 )
			{
				if( a ) *a = _FTOL( ( ( float * )p )[ 3 ] * 255.0f ) ;
			}
			break ;

		default :
			return -1 ;
		}
	}
	else
	{
		switch( BaseImage->ColorData.PixelByte )
		{
		case 1:
				p += x;
				Pal = &BaseImage->ColorData.Palette[*p] ;
				if( r ) *r = Pal->Red ;
				if( g ) *g = Pal->Green ;
				if( b ) *b = Pal->Blue ;
				if( a ) *a = Pal->Alpha ;

				// パレットカラーはここで終了
				return 0 ;

		case 2: Color = *((WORD *)(p + x * 2)); break;
		case 3:
				p += x * 3;
				Color = 0;
				((WORD *)&Color)[0] = ((WORD *)p)[0];
				((BYTE *)&Color)[2] = ((BYTE *)p)[2];
				break;
		case 4: Color = *((DWORD *)(p + x * 4)); break;

		default :
			return -1 ;
		}

		NS_GetColor5( &BaseImage->ColorData, Color, r, g, b, a );
	}

	// 終了
	return 0;
}

// 基本イメージデータの指定の座標の色を取得する(各色要素は浮動小数点数)
extern int NS_GetPixelBaseImageF( const BASEIMAGE *BaseImage, int x, int y, float *r, float *g, float *b, float *a )
{
	unsigned int Color;
	const BYTE *p;
	const COLORPALETTEDATA *Pal ;
	int ir, ig, ib, ia ;

	// フォーマットが標準フォーマットではなかったら標準フォーマットに変換する
	if( BaseImage->ColorData.Format != DX_BASEIMAGE_FORMAT_NORMAL )
	{
		if( NS_ConvertNormalFormatBaseImage( ( BASEIMAGE * )BaseImage ) < 0 )
			return -1 ;
	}

	// はみ出し判定
	if( ( DWORD )BaseImage->Width <= ( DWORD )x || ( DWORD )BaseImage->Height <= ( DWORD )y )
		return -1 ;

	p = (BYTE *)BaseImage->GraphData + BaseImage->Pitch * y;
	if( BaseImage->ColorData.FloatTypeFlag )
	{
		switch( BaseImage->ColorData.ChannelBitDepth )
		{
		case 16 :
			p += x * 8 ;
			if( BaseImage->ColorData.ChannelNum > 0 )
			{
				if( r ) *r = Float16ToFloat32( ( ( WORD * )p )[ 0 ] ) ;
			}
			if( BaseImage->ColorData.ChannelNum > 1 )
			{
				if( g ) *g = Float16ToFloat32( ( ( WORD * )p )[ 1 ] ) ;
			}
			if( BaseImage->ColorData.ChannelNum > 2 )
			{
				if( b ) *b = Float16ToFloat32( ( ( WORD * )p )[ 2 ] ) ;
			}
			if( BaseImage->ColorData.ChannelNum > 3 )
			{
				if( a ) *a = Float16ToFloat32( ( ( WORD * )p )[ 3 ] ) ;
			}
			break ;

		case 32 :
			p += x * 16 ;
			if( BaseImage->ColorData.ChannelNum > 0 )
			{
				if( r ) *r = ( ( float * )p )[ 0 ] ;
			}
			if( BaseImage->ColorData.ChannelNum > 1 )
			{
				if( g ) *g = ( ( float * )p )[ 1 ] ;
			}
			if( BaseImage->ColorData.ChannelNum > 2 )
			{
				if( b ) *b = ( ( float * )p )[ 2 ] ;
			}
			if( BaseImage->ColorData.ChannelNum > 3 )
			{
				if( a ) *a = ( ( float * )p )[ 3 ] ;
			}
			break ;

		default :
			return -1 ;
		}
	}
	else
	{
		switch( BaseImage->ColorData.PixelByte )
		{
		case 1:
				p += x;
				Pal = &BaseImage->ColorData.Palette[*p] ;
				if( r ) *r = Pal->Red   / 255.0f ;
				if( g ) *g = Pal->Green / 255.0f ;
				if( b ) *b = Pal->Blue  / 255.0f ;
				if( a ) *a = Pal->Alpha / 255.0f ;

				// パレットカラーはここで終了
				return 0 ;

		case 2: Color = *((WORD *)(p + x * 2)); break;
		case 3:
				p += x * 3;
				Color = 0;
				((WORD *)&Color)[0] = ((WORD *)p)[0];
				((BYTE *)&Color)[2] = ((BYTE *)p)[2];
				break;
		case 4: Color = *((DWORD *)(p + x * 4)); break;

		default :
			return -1 ;
		}

		NS_GetColor5( &BaseImage->ColorData, Color, &ir, &ig, &ib, &ia );
		if( r ) *r = ir / 255.0f ;
		if( g ) *g = ig / 255.0f ;
		if( b ) *b = ib / 255.0f ;
		if( a ) *a = ia / 255.0f ;
	}

	// 終了
	return 0;
}

// 基本イメージデータの指定の座標に線を描画する(各色要素は０〜２５５)
extern int NS_DrawLineBaseImage( BASEIMAGE *BaseImage, int x1, int y1, int x2, int y2, int r, int g, int b, int a )
{
	unsigned int Color;
	BYTE *p;
	int c, w, h, adx, ady, x, y ;

	// フォーマットが標準フォーマットではなかったら標準フォーマットに変換する
	if( BaseImage->ColorData.Format != DX_BASEIMAGE_FORMAT_NORMAL )
	{
		if( NS_ConvertNormalFormatBaseImage( BaseImage ) < 0 )
			return -1 ;
	}

	if( x1 == x2 && y1 == y2 ) return 0 ;

	w = x2 - x1 ;
	h = y2 - y1 ;
	if( w < 0 )
	{
		adx = -1 ;
		w = -w ;
	}
	else
	{
		adx = 1 ;
	}

	if( h < 0 )
	{
		ady = -1 ;
		h = -h ;
	}
	else
	{
		ady = 1 ;
	}

	Color = NS_GetColor3( &BaseImage->ColorData, r, g, b, a );
	x = x1 ;
	y = y1 ;
	switch( BaseImage->ColorData.PixelByte )
	{
	case 1:
		if( w == 0 )
		{
			if( ( DWORD )BaseImage->Width > ( DWORD )x )
			{
				while( y2 != y )
				{
					if( ( DWORD )BaseImage->Height > ( DWORD )y )
						*( ( BYTE * )BaseImage->GraphData + BaseImage->Pitch * y + x ) = ( BYTE )Color ;
					y += ady ;
				}
			}
		}
		else
		if( h == 0 )
		{
			if( ( DWORD )BaseImage->Height > ( DWORD )y )
			{
				while( x2 != x )
				{
					if( ( DWORD )BaseImage->Width > ( DWORD )x )
						*( ( BYTE * )BaseImage->GraphData + BaseImage->Pitch * y + x ) = ( BYTE )Color ;
					x += adx ;
				}
			}
		}
		if( h > w )
		{
			c = 0 ;
			while( x2 != x || y2 != y )
			{
				if( ( DWORD )BaseImage->Width > ( DWORD )x && ( DWORD )BaseImage->Height > ( DWORD )y )
					*( ( BYTE * )BaseImage->GraphData + BaseImage->Pitch * y + x ) = ( BYTE )Color ;
				c += w ;
				if( c >= h )
				{
					c -= h ;
					x += adx ;
				}
				y += ady ;
			}
		}
		else
		{
			c = 0 ;
			while( x2 != x || y2 != y )
			{
				if( ( DWORD )BaseImage->Width > ( DWORD )x && ( DWORD )BaseImage->Height > ( DWORD )y )
					*( ( BYTE * )BaseImage->GraphData + BaseImage->Pitch * y + x ) = ( BYTE )Color ;
				c += h ;
				if( c >= w )
				{
					c -= w ;
					y += ady ;
				}
				x += adx ;
			}
		}
		break;

	case 2:
		if( w == 0 )
		{
			if( ( DWORD )BaseImage->Width > ( DWORD )x )
			{
				while( y2 != y )
				{
					if( ( DWORD )BaseImage->Height > ( DWORD )y )
						*((WORD *)((BYTE *)BaseImage->GraphData + BaseImage->Pitch * y + x * 2)) =  (WORD)Color;
					y += ady ;
				}
			}
		}
		else
		if( h == 0 )
		{
			if( ( DWORD )BaseImage->Height > ( DWORD )y )
			{
				while( x2 != x )
				{
					if( ( DWORD )BaseImage->Width > ( DWORD )x )
						*((WORD *)((BYTE *)BaseImage->GraphData + BaseImage->Pitch * y + x * 2)) =  (WORD)Color;
					x += adx ;
				}
			}
		}
		if( h > w )
		{
			c = 0 ;
			while( x2 != x || y2 != y )
			{
				if( ( DWORD )BaseImage->Width > ( DWORD )x && ( DWORD )BaseImage->Height > ( DWORD )y )
					*((WORD *)((BYTE *)BaseImage->GraphData + BaseImage->Pitch * y + x * 2)) =  (WORD)Color;
				c += w ;
				if( c >= h )
				{
					c -= h ;
					x += adx ;
				}
				y += ady ;
			}
		}
		else
		{
			c = 0 ;
			while( x2 != x || y2 != y )
			{
				if( ( DWORD )BaseImage->Width > ( DWORD )x && ( DWORD )BaseImage->Height > ( DWORD )y )
					*((WORD *)((BYTE *)BaseImage->GraphData + BaseImage->Pitch * y + x * 2)) =  (WORD)Color;
				c += h ;
				if( c >= w )
				{
					c -= w ;
					y += ady ;
				}
				x += adx ;
			}
		}
		break;

	case 3:
		if( w == 0 )
		{
			if( ( DWORD )BaseImage->Width > ( DWORD )x )
			{
				while( y2 != y )
				{
					if( ( DWORD )BaseImage->Height > ( DWORD )y )
					{
						p = (BYTE *)BaseImage->GraphData + BaseImage->Pitch * y + x * 3;
						((WORD *)p)[0] = ((WORD *)&Color)[0];
						((BYTE *)p)[2] = ((BYTE *)&Color)[2];
					}
					y += ady ;
				}
			}
		}
		else
		if( h == 0 )
		{
			if( ( DWORD )BaseImage->Height > ( DWORD )y )
			{
				while( x2 != x )
				{
					if( ( DWORD )BaseImage->Width > ( DWORD )x )
					{
						p = (BYTE *)BaseImage->GraphData + BaseImage->Pitch * y + x * 3;
						((WORD *)p)[0] = ((WORD *)&Color)[0];
						((BYTE *)p)[2] = ((BYTE *)&Color)[2];
					}
					x += adx ;
				}
			}
		}
		if( h > w )
		{
			c = 0 ;
			while( x2 != x || y2 != y )
			{
				if( ( DWORD )BaseImage->Width > ( DWORD )x && ( DWORD )BaseImage->Height > ( DWORD )y )
				{
					p = (BYTE *)BaseImage->GraphData + BaseImage->Pitch * y + x * 3;
					((WORD *)p)[0] = ((WORD *)&Color)[0];
					((BYTE *)p)[2] = ((BYTE *)&Color)[2];
				}
				c += w ;
				if( c >= h )
				{
					c -= h ;
					x += adx ;
				}
				y += ady ;
			}
		}
		else
		{
			c = 0 ;
			while( x2 != x || y2 != y )
			{
				if( ( DWORD )BaseImage->Width > ( DWORD )x && ( DWORD )BaseImage->Height > ( DWORD )y )
				{
					p = (BYTE *)BaseImage->GraphData + BaseImage->Pitch * y + x * 3;
					((WORD *)p)[0] = ((WORD *)&Color)[0];
					((BYTE *)p)[2] = ((BYTE *)&Color)[2];
				}
				c += h ;
				if( c >= w )
				{
					c -= w ;
					y += ady ;
				}
				x += adx ;
			}
		}
		break;

	case 4:
		if( w == 0 )
		{
			if( ( DWORD )BaseImage->Width > ( DWORD )x )
			{
				while( y2 != y )
				{
					if( ( DWORD )BaseImage->Height > ( DWORD )y )
						*((DWORD *)((BYTE *)BaseImage->GraphData + BaseImage->Pitch * y + x * 4)) = (DWORD)Color;
					y += ady ;
				}
			}
		}
		else
		if( h == 0 )
		{
			if( ( DWORD )BaseImage->Height > ( DWORD )y )
			{
				while( x2 != x )
				{
					if( ( DWORD )BaseImage->Width > ( DWORD )x )
						*((DWORD *)((BYTE *)BaseImage->GraphData + BaseImage->Pitch * y + x * 4)) = (DWORD)Color;
					x += adx ;
				}
			}
		}
		if( h > w )
		{
			c = 0 ;
			while( x2 != x || y2 != y )
			{
				if( ( DWORD )BaseImage->Width > ( DWORD )x && ( DWORD )BaseImage->Height > ( DWORD )y )
					*((DWORD *)((BYTE *)BaseImage->GraphData + BaseImage->Pitch * y + x * 4)) = (DWORD)Color;
				c += w ;
				if( c >= h )
				{
					c -= h ;
					x += adx ;
				}
				y += ady ;
			}
		}
		else
		{
			c = 0 ;
			while( x2 != x || y2 != y )
			{
				if( ( DWORD )BaseImage->Width > ( DWORD )x && ( DWORD )BaseImage->Height > ( DWORD )y )
					*((DWORD *)((BYTE *)BaseImage->GraphData + BaseImage->Pitch * y + x * 4)) = (DWORD)Color;
				c += h ;
				if( c >= w )
				{
					c -= w ;
					y += ady ;
				}
				x += adx ;
			}
		}
		break;
	}

	// 終了
	return 0;
}










#define DRAWCIRCLEMEMIMG_FILL_SHRED_ND( DRAW, DESTADDNUM )	\
	if( !( ( x2 < DrawArea.left ) | ( x1 > DrawArea.right  ) |\
		   ( y1 < DrawArea.top  ) | ( y1 > DrawArea.bottom ) | LineDrawBuf[ y1 ] ) )\
	{\
		if( x1 < DrawArea.left  ) x1 = DrawArea.left  ;\
		if( x2 > DrawArea.right ) x2 = DrawArea.right ;\
\
		LineDrawBuf[ y1 ] = 1 ;\
\
		C = x2 - x1 + 1 ;\
		DrawBP = DestBP + DestPitch * y1 + x1 * (DESTADDNUM) ;\
		do{\
			DRAW\
			DrawBP += (DESTADDNUM) ;\
		}while( -- C ) ;\
	}


#define DRAWCIRCLEMEMIMG_FILL_ND( DRAW, DESTADDNUM )	\
{\
	x2 = Dx + x ; x1 = -Dx + x ; y1 = Dy + y ;\
	DRAWCIRCLEMEMIMG_FILL_SHRED_ND( DRAW, DESTADDNUM )\
\
	x2 = Dy + x ; x1 = -Dy + x ; y1 = Dx + y ;\
	DRAWCIRCLEMEMIMG_FILL_SHRED_ND( DRAW, DESTADDNUM )\
\
	x2 = Dy + x ; x1 = -Dy + x ; y1 = -Dx + y;\
	DRAWCIRCLEMEMIMG_FILL_SHRED_ND( DRAW, DESTADDNUM )\
\
	if( F >= 0 )\
	{\
		x2 = Dy + x ; x1 = -Dy + x ; y1 = Dx + y ;\
		DRAWCIRCLEMEMIMG_FILL_SHRED_ND( DRAW, DESTADDNUM )\
\
		x2 = Dy + x ; x1 = -Dy + x ; y1 = -Dx + y;\
		DRAWCIRCLEMEMIMG_FILL_SHRED_ND( DRAW, DESTADDNUM )\
\
		Dx -- ;\
		F -= 4 * Dx ;\
	}\
\
	Dy ++ ;\
	F += 4 * Dy + 2 ;\
\
	while( Dx >= Dy )\
	{\
		x2 = Dx + x ; x1 = -Dx + x ; y1 = Dy + y ;\
		DRAWCIRCLEMEMIMG_FILL_SHRED_ND( DRAW, DESTADDNUM )\
\
		x2 = Dx + x ; x1 = -Dx + x ; y1 = -Dy + y;\
		DRAWCIRCLEMEMIMG_FILL_SHRED_ND( DRAW, DESTADDNUM )\
\
		if( F >= 0 )\
		{\
			x2 = Dy + x ; x1 = -Dy + x ; y1 = Dx + y ;\
			DRAWCIRCLEMEMIMG_FILL_SHRED_ND( DRAW, DESTADDNUM )\
\
			x2 = Dy + x ; x1 = -Dy + x ; y1 = -Dx + y;\
			DRAWCIRCLEMEMIMG_FILL_SHRED_ND( DRAW, DESTADDNUM )\
\
			Dx -- ;\
			F -= 4 * Dx ;\
		}\
\
		Dy ++ ;\
		F += 4 * Dy + 2 ;\
	}\
}



#define DRAWCIRCLEMEMIMG_PSET_SHRED_ND( DRAW, DESTADDNUM )	\
	if( ( ( (DWORD)( x1 - DrawArea.left ) <= (DWORD)( DrawArea.right  - DrawArea.left ) ) &&\
		  ( (DWORD)( y1 - DrawArea.top  ) <= (DWORD)( DrawArea.bottom - DrawArea.top  ) ) ) )\
	{\
		DrawBP = DestBP + DestPitch * y1 + x1 * (DESTADDNUM) ;\
		DRAW\
	}

#define DRAWCIRCLEMEMIMG_PSET_ND( DRAW, DESTADDNUM )	\
{\
	x1 = -Dx + x ; y1 = Dy + y ;\
	DRAWCIRCLEMEMIMG_PSET_SHRED_ND( DRAW, DESTADDNUM )\
\
	x1 = Dx + x ;\
	DRAWCIRCLEMEMIMG_PSET_SHRED_ND( DRAW, DESTADDNUM )\
\
	x1 = Dy + x ; y1 = Dx + y ;\
	DRAWCIRCLEMEMIMG_PSET_SHRED_ND( DRAW, DESTADDNUM )\
\
	x1 = -Dy + x ; y1 = -Dx + y;\
	DRAWCIRCLEMEMIMG_PSET_SHRED_ND( DRAW, DESTADDNUM )\
\
	if( F >= 0 )\
	{\
		Dx -- ;\
		F -= 4 * Dx ;\
	}\
\
	Dy ++ ;\
	F += 4 * Dy + 2 ;\
\
	while( Dx >= Dy )\
	{\
		x1 = -Dx + x ; y1 = Dy + y ;\
		DRAWCIRCLEMEMIMG_PSET_SHRED_ND( DRAW, DESTADDNUM )\
\
		x1 = Dx + x ;\
		DRAWCIRCLEMEMIMG_PSET_SHRED_ND( DRAW, DESTADDNUM )\
\
		x1 = -Dx + x ; y1 = -Dy + y;\
		DRAWCIRCLEMEMIMG_PSET_SHRED_ND( DRAW, DESTADDNUM )\
\
		x1 = Dx + x ;\
		DRAWCIRCLEMEMIMG_PSET_SHRED_ND( DRAW, DESTADDNUM )\
\
\
		x1 = -Dy + x ; y1 = Dx + y ;\
		DRAWCIRCLEMEMIMG_PSET_SHRED_ND( DRAW, DESTADDNUM )\
\
		x1 = Dy + x ;\
		DRAWCIRCLEMEMIMG_PSET_SHRED_ND( DRAW, DESTADDNUM )\
\
		x1 = -Dy + x ; y1 = -Dx + y;\
		DRAWCIRCLEMEMIMG_PSET_SHRED_ND( DRAW, DESTADDNUM )\
\
		x1 = Dy + x ;\
		DRAWCIRCLEMEMIMG_PSET_SHRED_ND( DRAW, DESTADDNUM )\
\
		if( F >= 0 )\
		{\
			Dx -- ;\
			F -= 4 * Dx ;\
		}\
\
		Dy ++ ;\
		F += 4 * Dy + 2 ;\
	}\
}

#define DRAWCIRCLEMEMIMG_DRAW_BYTE		*( ( BYTE  * )DrawBP ) = ( BYTE  )Color ;
#define DRAWCIRCLEMEMIMG_DRAW_WORD		*( ( WORD  * )DrawBP ) = ( WORD  )Color ;
#define DRAWCIRCLEMEMIMG_DRAW_3BYTE		*( ( WORD  * )DrawBP ) = ( WORD  )Color ; ( ( BYTE * )DrawBP )[ 2 ] = ( BYTE )( Color >> 16 ) ;
#define DRAWCIRCLEMEMIMG_DRAW_DWORD		*( ( DWORD * )DrawBP ) = ( DWORD )Color ;


// 基本イメージデータの指定の座標に円を描画する(各色要素は０〜２５５)
extern int NS_DrawCircleBaseImage( BASEIMAGE *BaseImage, int x, int y, int radius, int r, int g, int b, int a, int FillFlag )
{
	unsigned int Color ;
	BYTE *LineDrawBuf ;
	RECT DrawArea ;
	DWORD DestPitch ;
	int Dx, Dy, F ;
	int x1, x2, y1, C ;
	BYTE  *DrawBP ;
	BYTE  *DestBP ;

	// 描画領域のセット
	DrawArea.left   = 0 ;
	DrawArea.top    = 0 ;
	DrawArea.right  = BaseImage->Width  - 1 ;
	DrawArea.bottom = BaseImage->Height - 1 ;

	// 転送先のアドレスをセット
	DestBP    = ( BYTE *)BaseImage->GraphData ;
	DestPitch = BaseImage->Pitch ;

	// 初期値セット
	Dx = radius ;
	Dy = 0 ;
	F = -2 * radius + 3 ;

	Color = NS_GetColor3( &BaseImage->ColorData, r, g, b, a ) ;

	// 塗りつぶすかどうかで処理を分岐
	if( FillFlag )
	{
		LineDrawBuf = (BYTE *)DXCALLOC( ( size_t )( DrawArea.bottom + 1 ) ) ;
		if( LineDrawBuf == NULL )
			return -1 ;

		switch( BaseImage->ColorData.PixelByte )
		{
		case 1 :
			DRAWCIRCLEMEMIMG_FILL_ND( DRAWCIRCLEMEMIMG_DRAW_BYTE, 1 )
			break ;

		case 2 :
			DRAWCIRCLEMEMIMG_FILL_ND( DRAWCIRCLEMEMIMG_DRAW_WORD, 2 )
			break ;

		case 3 :
			DRAWCIRCLEMEMIMG_FILL_ND( DRAWCIRCLEMEMIMG_DRAW_WORD, 3 )
			break ;

		case 4 :
			DRAWCIRCLEMEMIMG_FILL_ND( DRAWCIRCLEMEMIMG_DRAW_DWORD, 4 )
			break ;
		}

		DXFREE( LineDrawBuf ) ;
	}
	else
	{
		switch( BaseImage->ColorData.PixelByte )
		{
		case 1 :
			DRAWCIRCLEMEMIMG_PSET_ND( DRAWCIRCLEMEMIMG_DRAW_BYTE, 1 )
			break ;

		case 2 :
			DRAWCIRCLEMEMIMG_PSET_ND( DRAWCIRCLEMEMIMG_DRAW_WORD, 2 )
			break ;

		case 3 :
			DRAWCIRCLEMEMIMG_PSET_ND( DRAWCIRCLEMEMIMG_DRAW_3BYTE, 3 )
			break ;

		case 4 :
			DRAWCIRCLEMEMIMG_PSET_ND( DRAWCIRCLEMEMIMG_DRAW_DWORD, 4 )
			break ;
		}
	}

	// 終了
	return 0 ;
}

// 基本イメージデータを転送する
extern int NS_BltBaseImage( int DestX, int DestY, BASEIMAGE *SrcBaseImage, BASEIMAGE *DestBaseImage )
{
	return NS_BltBaseImage( 0, 0, SrcBaseImage->Width, SrcBaseImage->Height, DestX, DestY, SrcBaseImage, DestBaseImage ) ;
}

// 基本イメージデータを拡大転送する
extern int ScalingBltBaseImage(
				int SrcX1,  int SrcY1,  int SrcX2,  int SrcY2,  BASEIMAGE *SrcBaseImage,
				int DestX1, int DestY1, int DestX2, int DestY2, BASEIMAGE *DestBaseImage, int Bilinear )
{
	DWORD DestWidth, DestHeight ;
	DWORD SrcWidth, SrcHeight ;
	DWORD i, j ;
	DWORD SrcX, SrcY ;
	DWORD AddSrcX, AddSrcY ;
	DWORD NowX, NowY ;
	DWORD RateX, RateY, RateXR, RateYR ;
	DWORD Rate11, Rate12, Rate21, Rate22 ;
	DWORD r11, r12, r21, r22 ;
	DWORD g11, g12, g21, g22 ;
	DWORD b11, b12, b21, b22 ;
	DWORD a11, a12, a21, a22 ;
	DWORD dr, dg, db, da ;

	// 転送元座標と転送先座標の検査
	if( SrcX2 <= SrcX1 || SrcY2 <= SrcY1 ||
		SrcX1 < 0 || SrcX1 >= SrcBaseImage->Width  ||
		SrcY1 < 0 || SrcY1 >= SrcBaseImage->Height ||
		SrcX2 <= 0 || SrcX2 > SrcBaseImage->Width  ||
		SrcY2 <= 0 || SrcY2 > SrcBaseImage->Height ||
		DestX2 <= DestX1 || DestY2 <= DestY1 ||
		DestX1 < 0 || DestX1 >= DestBaseImage->Width  ||
		DestY1 < 0 || DestY1 >= DestBaseImage->Height ||
		DestX2 <= 0 || DestX2 > DestBaseImage->Width  ||
		DestY2 <= 0 || DestY2 > DestBaseImage->Height )
		return -1 ;

	// 転送サイズを算出
	SrcWidth   = ( DWORD )( SrcX2  - SrcX1  ) ;
	SrcHeight  = ( DWORD )( SrcY2  - SrcY1  ) ;
	DestWidth  = ( DWORD )( DestX2 - DestX1 ) ;
	DestHeight = ( DWORD )( DestY2 - DestY1 ) ;

	// 転送元と転送先のサイズが等しい場合は通常の転送を行う
	if( SrcWidth == DestWidth && SrcHeight == DestHeight )
	{
		return NS_BltBaseImage( SrcX1, SrcY1, ( int )SrcWidth, ( int )SrcHeight, DestX1, DestY1, SrcBaseImage, DestBaseImage ) ;
	}

	// １ドット辺りの変化値の割り出し
	AddSrcX = ( SrcWidth  << 16 ) / DestWidth  ;
	AddSrcY = ( SrcHeight << 16 ) / DestHeight ;

	// バイリニア補間かどうかで処理を分岐
	if( Bilinear == TRUE )
	{
		// バイリニア補間の場合
		SrcY = ( DWORD )( SrcY1 << 16 ) ;
		for( i = 0 ; i < DestHeight ; i ++, SrcY += AddSrcY )
		{
			SrcX = ( DWORD )( SrcX1 << 16 ) ;
			for( j = 0 ; j < DestWidth ; j ++, SrcX += AddSrcX )
			{
				NowX = SrcX >> 16 ;
				NowY = SrcY >> 16 ;
				NS_GetPixelBaseImage( SrcBaseImage, ( int )( NowX     ), ( int )( NowY     ), ( int * )&r11, ( int * )&g11, ( int * )&b11, ( int * )&a11 ) ; 
				NS_GetPixelBaseImage( SrcBaseImage, ( int )( NowX + 1 ), ( int )( NowY     ), ( int * )&r12, ( int * )&g12, ( int * )&b12, ( int * )&a12 ) ; 
				NS_GetPixelBaseImage( SrcBaseImage, ( int )( NowX     ), ( int )( NowY + 1 ), ( int * )&r21, ( int * )&g21, ( int * )&b21, ( int * )&a21 ) ; 
				NS_GetPixelBaseImage( SrcBaseImage, ( int )( NowX + 1 ), ( int )( NowY + 1 ), ( int * )&r22, ( int * )&g22, ( int * )&b22, ( int * )&a22 ) ; 

				RateX = SrcX & ( ( 1 << 16 ) - 1 ) ;
				RateY = SrcY & ( ( 1 << 16 ) - 1 ) ;
				RateXR = ( ( 1 << 16 ) - 1 ) - RateX ;
				RateYR = ( ( 1 << 16 ) - 1 ) - RateY ;
				Rate11 = ( RateXR * RateYR ) >> 16 ;
				Rate12 = ( RateX  * RateYR ) >> 16 ;
				Rate21 = ( RateXR * RateY  ) >> 16 ;
				Rate22 = ( RateX  * RateY  ) >> 16 ;

				dr = ( r11 * Rate11 + r12 * Rate12 + r21 * Rate21 + r22 * Rate22 ) >> 16 ; 
				dg = ( g11 * Rate11 + g12 * Rate12 + g21 * Rate21 + g22 * Rate22 ) >> 16 ; 
				db = ( b11 * Rate11 + b12 * Rate12 + b21 * Rate21 + b22 * Rate22 ) >> 16 ; 
				da = ( a11 * Rate11 + a12 * Rate12 + a21 * Rate21 + a22 * Rate22 ) >> 16 ; 

				NS_SetPixelBaseImage( DestBaseImage, ( int )( DestX1 + j ), ( int )( DestY1 + i ), ( int )dr, ( int )dg, ( int )db, ( int )da ) ;
			}
		}
	}
	else
	{
		// バイリニア補間じゃない場合
		SrcY = ( DWORD )( SrcY1 << 16 ) ;
		for( i = 0 ; i < DestHeight ; i ++, SrcY += AddSrcY )
		{
			SrcX = ( DWORD )( SrcX1 << 16 ) ;
			for( j = 0 ; j < DestWidth ; j ++, SrcX += AddSrcX )
			{
				NowX = SrcX >> 16 ;
				NowY = SrcY >> 16 ;
				NS_GetPixelBaseImage( SrcBaseImage,  ( int )NowX,           ( int )NowY,            ( int * )&dr, ( int * )&dg, ( int * )&db, ( int * )&da ) ; 
				NS_SetPixelBaseImage( DestBaseImage, ( int )( DestX1 + j ), ( int )( DestY1 + i ),  ( int )dr,    ( int )dg,    ( int )db,    ( int )da    ) ;
			}
		}
	}

	// 正常終了
	return 0 ;
}

// 基本イメージデータを転送する
extern int NS_BltBaseImage( int SrcX, int SrcY, int SrcSizeX, int SrcSizeY, int DestX, int DestY, BASEIMAGE *SrcBaseImage, BASEIMAGE *DestBaseImage )
{
	POINT DestPoint;
	RECT SrcRect;

	// フォーマットが標準フォーマットではなかったら標準フォーマットに変換する
	if( SrcBaseImage->ColorData.Format != DX_BASEIMAGE_FORMAT_NORMAL )
	{
		if( NS_ConvertNormalFormatBaseImage( SrcBaseImage ) < 0 )
			return -1 ;
	}

	// フォーマットが標準フォーマットではなかったら標準フォーマットに変換する
	if( DestBaseImage->ColorData.Format != DX_BASEIMAGE_FORMAT_NORMAL )
	{
		if( NS_ConvertNormalFormatBaseImage( DestBaseImage ) < 0 )
			return -1 ;
	}

	// サイズ補正
	if( SrcX  < 0 ){ SrcSizeX += SrcX  ; DestX -= SrcX;  SrcX  = 0 ; }
	if( SrcY  < 0 ){ SrcSizeY += SrcY  ; DestY -= SrcY;  SrcY  = 0 ; }
	if( DestX < 0 ){ SrcSizeX += DestX ; SrcX  -= DestX; DestX = 0 ; }
	if( DestY < 0 ){ SrcSizeY += DestY ; SrcY  -= DestY; DestY = 0 ; }
	if( DestBaseImage->Width  < DestX + SrcSizeX ) SrcSizeX = DestBaseImage->Width  - DestX;
	if( DestBaseImage->Height < DestY + SrcSizeY ) SrcSizeY = DestBaseImage->Height - DestY;
	if( SrcBaseImage->Width   < SrcX  + SrcSizeX ) SrcSizeX = SrcBaseImage->Width   - SrcX;
	if( SrcBaseImage->Height  < SrcY  + SrcSizeY ) SrcSizeY = SrcBaseImage->Height  - SrcY;
	if( SrcSizeX <= 0 || SrcSizeY <= 0 ) return 0;

	// 転送
	DestPoint.x    = DestX;
	DestPoint.y    = DestY;
	SrcRect.left   = SrcX;
	SrcRect.top    = SrcY;
	SrcRect.right  = SrcX + SrcSizeX;
	SrcRect.bottom = SrcY + SrcSizeY;
	NS_GraphColorMatchBltVer2(
		DestBaseImage->GraphData, DestBaseImage->Pitch, &DestBaseImage->ColorData, 
		 SrcBaseImage->GraphData,  SrcBaseImage->Pitch,  &SrcBaseImage->ColorData, 
		                    NULL,                    0,                      NULL,
		DestPoint, &SrcRect, FALSE,
		FALSE, 0,
		DX_SHAVEDMODE_NONE, FALSE, FALSE, FALSE, TRUE ) ;

	// 終了
	return 0;
}

// 基本イメージデータを透過色処理付きで転送する
extern	int		NS_BltBaseImageWithTransColor( int SrcX, int SrcY, int SrcSizeX, int SrcSizeY, int DestX, int DestY, BASEIMAGE *SrcBaseImage, BASEIMAGE *DestBaseImage, int Tr, int Tg, int Tb, int Ta )
{
	POINT DestPoint;
	RECT SrcRect;

	// フォーマットが標準フォーマットではなかったら標準フォーマットに変換する
	if( SrcBaseImage->ColorData.Format != DX_BASEIMAGE_FORMAT_NORMAL )
	{
		if( NS_ConvertNormalFormatBaseImage( SrcBaseImage ) < 0 )
			return -1 ;
	}

	// フォーマットが標準フォーマットではなかったら標準フォーマットに変換する
	if( DestBaseImage->ColorData.Format != DX_BASEIMAGE_FORMAT_NORMAL )
	{
		if( NS_ConvertNormalFormatBaseImage( DestBaseImage ) < 0 )
			return -1 ;
	}

	// サイズ補正
	if( SrcX  < 0 ){ SrcSizeX += SrcX  ; DestX -= SrcX;  SrcX  = 0 ; }
	if( SrcY  < 0 ){ SrcSizeY += SrcY  ; DestY -= SrcY;  SrcY  = 0 ; }
	if( DestX < 0 ){ SrcSizeX += DestX ; SrcX  -= DestX; DestX = 0 ; }
	if( DestY < 0 ){ SrcSizeY += DestY ; SrcY  -= DestY; DestY = 0 ; }
	if( DestBaseImage->Width  < DestX + SrcSizeX ) SrcSizeX = DestBaseImage->Width  - DestX;
	if( DestBaseImage->Height < DestY + SrcSizeY ) SrcSizeY = DestBaseImage->Height - DestY;
	if( SrcBaseImage->Width   < SrcX  + SrcSizeX ) SrcSizeX = SrcBaseImage->Width   - SrcX;
	if( SrcBaseImage->Height  < SrcY  + SrcSizeY ) SrcSizeY = SrcBaseImage->Height  - SrcY;
	if( SrcSizeX <= 0 || SrcSizeY <= 0 ) return 0;

	// 透過色値補正
	if( Tr < 0 ) Tr = 0 ; else if( Tr > 255 ) Tr = 255 ;
	if( Tg < 0 ) Tg = 0 ; else if( Tg > 255 ) Tg = 255 ;
	if( Tb < 0 ) Tb = 0 ; else if( Tb > 255 ) Tb = 255 ;
	if( Ta < 0 ) Ta = 0 ; else if( Ta > 255 ) Ta = 255 ;

	// 転送
	DestPoint.x    = DestX;
	DestPoint.y    = DestY;
	SrcRect.left   = SrcX;
	SrcRect.top    = SrcY;
	SrcRect.right  = SrcX + SrcSizeX;
	SrcRect.bottom = SrcY + SrcSizeY;
	NS_GraphColorMatchBltVer2(
		DestBaseImage->GraphData, DestBaseImage->Pitch, &DestBaseImage->ColorData, 
		 SrcBaseImage->GraphData,  SrcBaseImage->Pitch,  &SrcBaseImage->ColorData, 
		                    NULL,                    0,                      NULL,
		DestPoint, &SrcRect, FALSE,
		FALSE, ( ( DWORD )Ta << 24 ) | ( ( DWORD )Tr << 16 ) | ( ( DWORD )Tg << 8 ) | ( ( DWORD )Tb ),
		DX_SHAVEDMODE_NONE, FALSE, FALSE, TRUE, TRUE ) ;

	// 終了
	return 0;
}

// 基本イメージデータをアルファ値のブレンドを考慮した上で転送する( 出力先が ARGB8 形式以外の場合はエラーになります )
extern int NS_BltBaseImageWithAlphaBlend( int SrcX, int SrcY, int SrcSizeX, int SrcSizeY, int DestX, int DestY, BASEIMAGE *SrcBaseImage, BASEIMAGE *DestBaseImage, int Opacity )
{
	DWORD Width, Height ;
	DWORD i, j, OpSrcAlpha, SrcAlpha, ResultAlpha, SrcAddPitch, DestAddPitch ;
	BYTE *Dest, *Src ;
	BASEIMAGE TempBaseImage, *UseSrcBaseImage ;

	// フォーマットが標準フォーマットではなかったら標準フォーマットに変換する
	if( SrcBaseImage->ColorData.Format != DX_BASEIMAGE_FORMAT_NORMAL )
	{
		if( NS_ConvertNormalFormatBaseImage( SrcBaseImage ) < 0 )
			return -1 ;
	}

	// フォーマットが標準フォーマットではなかったら標準フォーマットに変換する
	if( DestBaseImage->ColorData.Format != DX_BASEIMAGE_FORMAT_NORMAL )
	{
		if( NS_ConvertNormalFormatBaseImage( DestBaseImage ) < 0 )
			return -1 ;
	}

	// 出力先が ARGB8 以外の場合はエラー
	if( DestBaseImage->ColorData.AlphaMask != 0xff000000 ||
		DestBaseImage->ColorData.RedMask   != 0x00ff0000 ||
		DestBaseImage->ColorData.GreenMask != 0x0000ff00 ||
		DestBaseImage->ColorData.BlueMask  != 0x000000ff )
	{
		return -1 ;
	}

	// 透明度が最大のときは何もせずに終了
	if( Opacity == 0 )
		return 0 ;

	// 転送元が３２ビットフォーマット以外の場合は３２ビットフォーマットに変更する
	if( SrcBaseImage->ColorData.PixelByte != 4 )
	{
		if( SrcBaseImage->ColorData.AlphaWidth != 0 )
		{
			NS_CreateARGB8ColorBaseImage( SrcBaseImage->Width, SrcBaseImage->Height, &TempBaseImage ) ;
		}
		else
		{
			NS_CreateXRGB8ColorBaseImage( SrcBaseImage->Width, SrcBaseImage->Height, &TempBaseImage ) ;
		}
		NS_BltBaseImage( 0, 0, SrcBaseImage, &TempBaseImage ) ; 
		UseSrcBaseImage = &TempBaseImage ;
	}
	else
	{
		UseSrcBaseImage = SrcBaseImage ;
	}

	// サイズ補正
	if( SrcX  < 0 ){ SrcSizeX += SrcX  ; DestX -= SrcX;  SrcX  = 0 ; }
	if( SrcY  < 0 ){ SrcSizeY += SrcY  ; DestY -= SrcY;  SrcY  = 0 ; }
	if( DestX < 0 ){ SrcSizeX += DestX ; SrcX  -= DestX; DestX = 0 ; }
	if( DestY < 0 ){ SrcSizeY += DestY ; SrcY  -= DestY; DestY = 0 ; }
	if( DestBaseImage->Width  < DestX + SrcSizeX ) SrcSizeX = DestBaseImage->Width  - DestX;
	if( DestBaseImage->Height < DestY + SrcSizeY ) SrcSizeY = DestBaseImage->Height - DestY;
	if( SrcBaseImage->Width   < SrcX  + SrcSizeX ) SrcSizeX = SrcBaseImage->Width   - SrcX;
	if( SrcBaseImage->Height  < SrcY  + SrcSizeY ) SrcSizeY = SrcBaseImage->Height  - SrcY;
	if( SrcSizeX <= 0 || SrcSizeY <= 0 ) return 0;

	Width  = ( DWORD )SrcSizeX ;
	Height = ( DWORD )SrcSizeY ;

	SrcAddPitch  = UseSrcBaseImage->Pitch  - Width * UseSrcBaseImage->ColorData.PixelByte ;
	DestAddPitch = DestBaseImage->Pitch - Width * DestBaseImage->ColorData.PixelByte ;
	Src  = ( BYTE * )UseSrcBaseImage->GraphData + SrcX * UseSrcBaseImage->ColorData.PixelByte + SrcY * UseSrcBaseImage->Pitch ;
	Dest = ( BYTE * )DestBaseImage->GraphData + DestX * DestBaseImage->ColorData.PixelByte + DestY * DestBaseImage->Pitch ;

	// 転送元のフォーマットによって処理を分岐
	if( SrcBaseImage->ColorData.AlphaMask == 0xff000000 &&
		SrcBaseImage->ColorData.RedMask   == 0x00ff0000 &&
		SrcBaseImage->ColorData.GreenMask == 0x0000ff00 &&
		SrcBaseImage->ColorData.BlueMask  == 0x000000ff )
	{
		if( Opacity == 255 )
		{
			for( i = Height ; i ; i --, Src += SrcAddPitch, Dest += DestAddPitch )
			{
				for( j = Width ; j ; j --, Src += 4, Dest += 4 )
				{
					if( Src[ 3 ] == 0 ) continue ;
					if( Src[ 3 ] == 255 )
					{
						*( ( DWORD * )Dest ) = *( ( DWORD * )Src ) ;
					}
					else
					{
						ResultAlpha = ( DWORD )( ( 256 - Dest[ 3 ] ) * Src[ 3 ] / 256 + Dest[ 3 ] ) ;
						SrcAlpha = Src[ 3 ] * 256 / ResultAlpha ;
						Dest[ 0 ] = ( BYTE )( SrcAlpha * ( Src[ 0 ] - Dest[ 0 ] ) / 256 + Dest[ 0 ] ) ;
						Dest[ 1 ] = ( BYTE )( SrcAlpha * ( Src[ 1 ] - Dest[ 1 ] ) / 256 + Dest[ 1 ] ) ;
						Dest[ 2 ] = ( BYTE )( SrcAlpha * ( Src[ 2 ] - Dest[ 2 ] ) / 256 + Dest[ 2 ] ) ;
						Dest[ 3 ] = ( BYTE )( ResultAlpha ) ;
					}
				}
			}
		}
		else
		{
			for( i = Height ; i ; i --, Src += SrcAddPitch, Dest += DestAddPitch )
			{
				for( j = Width ; j ; j --, Src += 4, Dest += 4 )
				{
					OpSrcAlpha = ( DWORD )( Src[ 3 ] * Opacity / 256 ) ;
					if( OpSrcAlpha == 0 ) continue ;
					ResultAlpha = ( 256 - Dest[ 3 ] ) * OpSrcAlpha / 256 + Dest[ 3 ] ;
					SrcAlpha = OpSrcAlpha * 256 / ResultAlpha ;
					Dest[ 0 ] = ( BYTE )( SrcAlpha * ( Src[ 0 ] - Dest[ 0 ] ) / 256 + Dest[ 0 ] ) ;
					Dest[ 1 ] = ( BYTE )( SrcAlpha * ( Src[ 1 ] - Dest[ 1 ] ) / 256 + Dest[ 1 ] ) ;
					Dest[ 2 ] = ( BYTE )( SrcAlpha * ( Src[ 2 ] - Dest[ 2 ] ) / 256 + Dest[ 2 ] ) ;
					Dest[ 3 ] = ( BYTE )( ResultAlpha ) ;
				}
			}
		}
	}
	else
	if( SrcBaseImage->ColorData.AlphaMask == 0x00000000 &&
		SrcBaseImage->ColorData.RedMask   == 0x00ff0000 &&
		SrcBaseImage->ColorData.GreenMask == 0x0000ff00 &&
		SrcBaseImage->ColorData.BlueMask  == 0x000000ff )
	{
		if( Opacity == 255 )
		{
			for( i = Height ; i ; i --, Src += SrcAddPitch, Dest += DestAddPitch )
			{
				for( j = Width ; j ; j --, Src += 4, Dest += 4 )
				{
					Dest[ 0 ] = Src[ 0 ] ;
					Dest[ 1 ] = Src[ 1 ] ;
					Dest[ 2 ] = Src[ 2 ] ;
					Dest[ 3 ] = 255 ;
				}
			}
		}
		else
		{
			for( i = Height ; i ; i --, Src += SrcAddPitch, Dest += DestAddPitch )
			{
				for( j = Width ; j ; j --, Src += 4, Dest += 4 )
				{
					ResultAlpha = ( DWORD )( ( 256 - Dest[ 3 ] ) * Opacity / 256 + Dest[ 3 ] ) ;
					SrcAlpha = Opacity * 256 / ResultAlpha ;
					Dest[ 0 ] = ( BYTE )( SrcAlpha * ( Src[ 0 ] - Dest[ 0 ] ) / 256 + Dest[ 0 ] ) ;
					Dest[ 1 ] = ( BYTE )( SrcAlpha * ( Src[ 1 ] - Dest[ 1 ] ) / 256 + Dest[ 1 ] ) ;
					Dest[ 2 ] = ( BYTE )( SrcAlpha * ( Src[ 2 ] - Dest[ 2 ] ) / 256 + Dest[ 2 ] ) ;
					Dest[ 3 ] = ( BYTE )( ResultAlpha ) ;
				}
			}
		}
	}

	if( UseSrcBaseImage == &TempBaseImage )
	{
		NS_ReleaseBaseImage( &TempBaseImage ) ;
	}

	// 終了
	return 0 ;
}


// 基本イメージデータの左右を反転する
extern int NS_ReverseBaseImageH( BASEIMAGE *BaseImage )
{
	BYTE TempB ;
	WORD TempW ;
	DWORD TempD ;
	DWORD i, j, loopnum ;
	DWORD w ,h ;
	BYTE *Src, *Dest ;

	// 圧縮フォーマットは反転できない
	if( BaseImage->ColorData.Format != DX_BASEIMAGE_FORMAT_NORMAL )
		return -1 ;

	// 水平転送ループの数を算出
	loopnum = ( DWORD )( BaseImage->Width / 2 ) ;

	// 情報をローカル変数に代入
	w = ( DWORD )BaseImage->Width ;
	h = ( DWORD )BaseImage->Height ;

	// バイト数によって処理を分岐
	switch( BaseImage->ColorData.PixelByte )
	{
	case 1 :
		for( i = 0 ; i < h ; i ++ )
		{
			Src  = ( BYTE * )BaseImage->GraphData + i * BaseImage->Pitch ;
			Dest = Src + ( w - 1 ) * 1 ;
			for( j = 0 ; j < loopnum ; j ++, Src += 1, Dest -= 1 )
			{
				TempB = *( ( BYTE * )Src ) ;
				*( ( BYTE * )Src  ) = *( ( BYTE * )Dest ) ;
				*( ( BYTE * )Dest ) = TempB ;
			}
		}
		break ;

	case 2 :
		for( i = 0 ; i < h ; i ++ )
		{
			Src  = ( BYTE * )BaseImage->GraphData + i * BaseImage->Pitch ;
			Dest = Src + ( w - 1 ) * 2 ;
			for( j = 0 ; j < loopnum ; j ++, Src += 2, Dest -= 2 )
			{
				TempW = *( ( WORD * )Src ) ;
				*( ( WORD * )Src  ) = *( ( WORD * )Dest ) ;
				*( ( WORD * )Dest ) = TempW ;
			}
		}
		break ;

	case 3 :
		for( i = 0 ; i < h ; i ++ )
		{
			Src  = ( BYTE * )BaseImage->GraphData + i * BaseImage->Pitch ;
			Dest = Src + ( w - 1 ) * 3 ;
			for( j = 0 ; j < loopnum ; j ++, Src += 3, Dest -= 3 )
			{
				TempW = *( ( WORD * )Src ) ;
				TempB = Src[ 2 ] ;
				*( ( WORD * )Src  ) = *( ( WORD * )Dest ) ;
				Src[ 2 ] = Dest[ 2 ] ;
				*( ( WORD * )Dest ) = TempW ;
				Dest[ 2 ] = TempB ;
			}
		}
		break ;

	case 4 :
		for( i = 0 ; i < h ; i ++ )
		{
			Src  = ( BYTE * )BaseImage->GraphData + i * BaseImage->Pitch ;
			Dest = Src + ( w - 1 ) * 4 ;
			for( j = 0 ; j < loopnum ; j ++, Src += 4, Dest -= 4 )
			{
				TempD = *( ( DWORD * )Src ) ;
				*( ( DWORD * )Src  ) = *( ( DWORD * )Dest ) ;
				*( ( DWORD * )Dest ) = TempD ;
			}
		}
		break ;
	}

	// 終了
	return 0 ;
}

// 基本イメージデータの上下を反転する
extern int NS_ReverseBaseImageV( BASEIMAGE *BaseImage )
{
	BYTE TempB ;
	WORD TempW ;
	DWORD TempD ;
	DWORD i, j, loopnum ;
	DWORD w ,h ;
	BYTE *Src, *Dest ;

	// 圧縮フォーマットは反転できない
	if( BaseImage->ColorData.Format != DX_BASEIMAGE_FORMAT_NORMAL )
		return -1 ;

	// 垂直転送ループの数を算出
	loopnum = ( DWORD )( BaseImage->Height / 2 ) ;

	// 情報をローカル変数に代入
	w = ( DWORD )BaseImage->Width ;
	h = ( DWORD )BaseImage->Height ;

	// バイト数によって処理を分岐
	switch( BaseImage->ColorData.PixelByte )
	{
	case 1 :
		for( i = 0 ; i < loopnum ; i ++ )
		{
			Src  = ( BYTE * )BaseImage->GraphData + i * BaseImage->Pitch ;
			Dest = ( BYTE * )BaseImage->GraphData + ( h - i - 1 ) * BaseImage->Pitch ;
			for( j = 0 ; j < w ; j ++, Src += 1, Dest += 1 )
			{
				TempB = *( ( BYTE * )Src ) ;
				*( ( BYTE * )Src  ) = *( ( BYTE * )Dest ) ;
				*( ( BYTE * )Dest ) = TempB ;
			}
		}
		break ;

	case 2 :
		for( i = 0 ; i < loopnum ; i ++ )
		{
			Src  = ( BYTE * )BaseImage->GraphData + i * BaseImage->Pitch ;
			Dest = ( BYTE * )BaseImage->GraphData + ( h - i - 1 ) * BaseImage->Pitch ;
			for( j = 0 ; j < w ; j ++, Src += 2, Dest += 2 )
			{
				TempW = *( ( WORD * )Src ) ;
				*( ( WORD * )Src  ) = *( ( WORD * )Dest ) ;
				*( ( WORD * )Dest ) = TempW ;
			}
		}
		break ;

	case 3 :
		for( i = 0 ; i < loopnum ; i ++ )
		{
			Src  = ( BYTE * )BaseImage->GraphData + i * BaseImage->Pitch ;
			Dest = ( BYTE * )BaseImage->GraphData + ( h - i - 1 ) * BaseImage->Pitch ;
			for( j = 0 ; j < w ; j ++, Src += 3, Dest += 3 )
			{
				TempW = *( ( WORD * )Src ) ;
				TempB = Src[ 2 ] ;
				*( ( WORD * )Src  ) = *( ( WORD * )Dest ) ;
				Src[ 2 ] = Dest[ 2 ] ;
				*( ( WORD * )Dest ) = TempW ;
				Dest[ 2 ] = TempB ;
			}
		}
		break ;

	case 4 :
		for( i = 0 ; i < loopnum ; i ++ )
		{
			Src  = ( BYTE * )BaseImage->GraphData + i * BaseImage->Pitch ;
			Dest = ( BYTE * )BaseImage->GraphData + ( h - i - 1 ) * BaseImage->Pitch ;
			for( j = 0 ; j < w ; j ++, Src += 4, Dest += 4 )
			{
				TempD = *( ( DWORD * )Src ) ;
				*( ( DWORD * )Src  ) = *( ( DWORD * )Dest ) ;
				*( ( DWORD * )Dest ) = TempD ;
			}
		}
		break ;
	}

	// 終了
	return 0 ;
}

// 基本イメージデータの上下左右を反転する
extern int NS_ReverseBaseImage( BASEIMAGE *BaseImage )
{
	BYTE TempB ;
	WORD TempW ;
	DWORD TempD ;
	DWORD i, j, loopnum ;
	DWORD w ,h ;
	BYTE *Src, *Dest ;

	// 圧縮フォーマットは反転できない
	if( BaseImage->ColorData.Format != DX_BASEIMAGE_FORMAT_NORMAL )
		return -1 ;

	// 垂直転送ループの数を算出
	loopnum = ( DWORD )( BaseImage->Height / 2 ) ;

	// 情報をローカル変数に代入
	w = ( DWORD )BaseImage->Width ;
	h = ( DWORD )BaseImage->Height ;

	// バイト数によって処理を分岐
	switch( BaseImage->ColorData.PixelByte )
	{
	case 1 :
		for( i = 0 ; i < loopnum ; i ++ )
		{
			Src  = ( BYTE * )BaseImage->GraphData + i * BaseImage->Pitch ;
			Dest = ( BYTE * )BaseImage->GraphData + ( w - 1 ) * 1 + ( h - i - 1 ) * BaseImage->Pitch ;
			for( j = 0 ; j < w ; j ++, Src += 1, Dest -= 1 )
			{
				TempB = *( ( BYTE * )Src ) ;
				*( ( BYTE * )Src  ) = *( ( BYTE * )Dest ) ;
				*( ( BYTE * )Dest ) = TempB ;
			}
		}
		break ;

	case 2 :
		for( i = 0 ; i < loopnum ; i ++ )
		{
			Src  = ( BYTE * )BaseImage->GraphData + i * BaseImage->Pitch ;
			Dest = ( BYTE * )BaseImage->GraphData + ( w - 1 ) * 2 + ( h - i - 1 ) * BaseImage->Pitch ;
			for( j = 0 ; j < w ; j ++, Src += 2, Dest -= 2 )
			{
				TempW = *( ( WORD * )Src ) ;
				*( ( WORD * )Src  ) = *( ( WORD * )Dest ) ;
				*( ( WORD * )Dest ) = TempW ;
			}
		}
		break ;

	case 3 :
		for( i = 0 ; i < loopnum ; i ++ )
		{
			Src  = ( BYTE * )BaseImage->GraphData + i * BaseImage->Pitch ;
			Dest = ( BYTE * )BaseImage->GraphData + ( w - 1 ) * 3 + ( h - i - 1 ) * BaseImage->Pitch ;
			for( j = 0 ; j < w ; j ++, Src += 3, Dest -= 3 )
			{
				TempW = *( ( WORD * )Src ) ;
				TempB = Src[ 2 ] ;
				*( ( WORD * )Src  ) = *( ( WORD * )Dest ) ;
				Src[ 2 ] = Dest[ 2 ] ;
				*( ( WORD * )Dest ) = TempW ;
				Dest[ 2 ] = TempB ;
			}
		}
		break ;

	case 4 :
		for( i = 0 ; i < loopnum ; i ++ )
		{
			Src  = ( BYTE * )BaseImage->GraphData + i * BaseImage->Pitch ;
			Dest = ( BYTE * )BaseImage->GraphData + ( w - 1 ) * 4 + ( h - i - 1 ) * BaseImage->Pitch ;
			for( j = 0 ; j < w ; j ++, Src += 4, Dest -= 4 )
			{
				TempD = *( ( DWORD * )Src ) ;
				*( ( DWORD * )Src  ) = *( ( DWORD * )Dest ) ;
				*( ( DWORD * )Dest ) = TempD ;
			}
		}
		break ;
	}

	// 終了
	return 0 ;
}

// 基本イメージデータに含まれるピクセルのアルファ値をチェックする
// 戻り値
// -1:エラー
// 0:画像にアルファ成分が無い
// 1:画像にアルファ成分があり、すべて最大(255)値
// 2:画像にアルファ成分があり、存在するアルファ値は最小(0)と最大(255)もしくは最小(0)のみ
// 3:画像にアルファ成分があり、最小と最大以外の中間の値がある
extern int NS_CheckPixelAlphaBaseImage( const BASEIMAGE *BaseImage )
{
	int i, j ;
	int Width ;
	int Height ;
	DWORD AlphaMask, AlphaLoc, AlphaMax ;
	DWORD Pitch ;
	BYTE *p, *lp ;
	const COLORPALETTEDATA *Pal ;
	int Result ;
	DWORD A ;

	// フォーマットが標準フォーマットではなかったらエラー
	if( BaseImage->ColorData.Format != DX_BASEIMAGE_FORMAT_NORMAL )
		return -1 ;

	// 画像にアルファ成分がなければここで終了
	if( BaseImage->ColorData.AlphaWidth == 0 )
		return 0 ;

	AlphaMask = BaseImage->ColorData.AlphaMask ;
	AlphaLoc = BaseImage->ColorData.AlphaLoc ;
	AlphaMax = AlphaMask >> AlphaLoc ;
	Width = BaseImage->Width ;
	Height = BaseImage->Height ;
	Pitch = ( DWORD )BaseImage->Pitch ;
	lp = ( BYTE * )BaseImage->GraphData ;
	Result = 1 ;
	switch( BaseImage->ColorData.PixelByte )
	{
	case 1:
		Pal = BaseImage->ColorData.Palette ;
		for( i = 0 ; i < Height ; i ++, lp += Pitch )
		{
			p = lp ;
			for( j = 0 ; j < Width ; j ++, p ++ )
			{
				A = Pal[ *p ].Alpha ;
				if( A == 255 ) continue ;

				if( A == 0 )
				{
					if( Result == 1 ) Result = 2 ;
				}
				else return 3 ;
			}
		}
		break ;

	case 2:
		for( i = 0 ; i < Height ; i ++, lp += Pitch )
		{
			p = lp ;
			for( j = 0 ; j < Width ; j ++, p += 2 )
			{
				A = ( *(( WORD * )p ) & AlphaMask ) >> AlphaLoc ;
				if( A == AlphaMax ) continue ;

				if( A == 0 )
				{
					if( Result == 1 ) Result = 2 ;
				}
				else return 3 ;
			}
		}
		break ;

	case 3:
		for( i = 0 ; i < Height ; i ++, lp += Pitch )
		{
			p = lp ;
			for( j = 0 ; j < Width ; j ++, p += 3 )
			{
				A = ( ( p[0] | ( p [1] << 8 ) | ( p[2] << 16 ) ) & AlphaMask ) >> AlphaLoc ;
				if( A == AlphaMax ) continue ;

				if( A == 0 )
				{
					if( Result == 1 ) Result = 2 ;
				}
				else return 3 ;
			}
		}
		break ;

	case 4:
		for( i = 0 ; i < Height ; i ++, lp += Pitch )
		{
			p = lp ;
			for( j = 0 ; j < Width ; j ++, p += 4 )
			{
				A = ( *(( DWORD * )p ) & AlphaMask ) >> AlphaLoc ;
				if( A == AlphaMax ) continue ;

				if( A == 0 )
				{
					if( Result == 1 ) Result = 2 ;
				}
				else return 3 ;
			}
		}
		break ;
	}

	// 終了
	return Result ;
}

// 基本イメージデータで使用されているパレット番号の最大値を取得する( パレット画像では無い場合は -1 が返る )
extern int NS_GetBaseImageUseMaxPaletteNo( const BASEIMAGE *BaseImage )
{
	int i, j ;
	int Width ;
	int Height ;
	DWORD Pitch ;
	BYTE *p, *lp ;
	BYTE MaxPaletteNo ;

	// フォーマットが標準フォーマットではなかったらエラー
	if( BaseImage->ColorData.Format != DX_BASEIMAGE_FORMAT_NORMAL )
		return -1 ;

	// パレットカラーで無い場合もエラー
	if( BaseImage->ColorData.PixelByte != 1 )
		return -1 ;

	Width = BaseImage->Width ;
	Height = BaseImage->Height ;
	Pitch = ( DWORD )BaseImage->Pitch ;
	lp = ( BYTE * )BaseImage->GraphData ;
	MaxPaletteNo = 0 ;
	for( i = 0 ; i < Height ; i ++, lp += Pitch )
	{
		p = lp ;
		for( j = 0 ; j < Width ; j ++, p ++ )
		{
			// 最大値があったらそこで終了
			if( *p == 255 )
			{
				return 255 ;
			}
			
			if( MaxPaletteNo < *p )
			{
				MaxPaletteNo = *p ;
			}
		}
	}

	// 終了
	return ( int )MaxPaletteNo ;
}

// ２分の１スケーリングしながらグラフィックデータ間転送を行う、そのため奇数倍数の転送矩形は指定できない
extern int GraphHalfScaleBlt( const COLORDATA *ColorData,
								       void *DestGraphData, int DestPitch,
		 						 const void *SrcGraphData, int SrcPitch,
								 int DestX, int DestY, int SrcX, int SrcY, int SrcWidth, int SrcHeight )
{
	DWORD i, j ;
	DWORD pbyte ;
	DWORD code1, code2, code3, code4, dcode ;
	DWORD rm, gm, bm, am, rl, gl, bl, al, bitdepth, bltw, blth ;
	BYTE *src, *dst ;

	// サイズが１以外の奇数だったらエラー
	if( ( SrcWidth  != 1 && ( SrcWidth  & 1 ) ) ||
		( SrcHeight != 1 && ( SrcHeight & 1 ) ) )
		return -1 ;

	bltw = ( DWORD )( SrcWidth  / 2 ) ;
	blth = ( DWORD )( SrcHeight / 2 ) ;

	rm = ColorData->RedMask ;
	rl = ColorData->RedLoc ;
	gm = ColorData->GreenMask ;
	gl = ColorData->GreenLoc ;
	bm = ColorData->BlueMask ;
	bl = ColorData->BlueLoc ;
	am = ColorData->AlphaMask ;
	al = ColorData->AlphaLoc ;
	bitdepth = ColorData->ColorBitDepth ;
	pbyte = ColorData->PixelByte ;

	// 縦横どちらかのサイズが 1 かどうかで処理を分岐
	if( SrcWidth == 1 && SrcHeight == 1 )
	{
		// 縦も横も１の場合
		src = ( BYTE * )SrcGraphData + SrcX * pbyte + SrcY * SrcPitch ;
		dst = ( BYTE * )DestGraphData + DestX * pbyte + DestY * DestPitch ;
		switch( bitdepth )
		{
		case 16 :
			*( ( WORD * )dst ) = ( ( WORD * )src )[ 0 ] ;
			break ;

		case 32 :
			*( ( DWORD * )dst ) = ( ( DWORD * )src )[ 0 ] ;
			break ;
		}
	}
	else
	if( SrcWidth == 1 )
	{
		// 横が１の場合
		for( i = 0 ; i < blth ; i ++ )
		{
			src = ( BYTE * )SrcGraphData + SrcX * pbyte + ( SrcY + ( i << 1 ) ) * SrcPitch ;
			switch( bitdepth )
			{
			case 16 :
				code1 = ( ( WORD * )src )[ 0 ] ;
				code3 = ( ( WORD * )( src + SrcPitch ) )[ 0 ] ;
				break ;

			case 32 :
				code1 = ( ( DWORD * )src )[ 0 ] ;
				code3 = ( ( DWORD * )( src + SrcPitch ) )[ 0 ] ;
				break ;

			default :
				return -1 ;
			}

			dcode = 
				( ( ( ( ( code1 & rm ) >> rl ) + ( ( code3 & rm ) >> rl ) ) >> 1 ) << rl ) |
				( ( ( ( ( code1 & gm ) >> gl ) + ( ( code3 & gm ) >> gl ) ) >> 1 ) << gl ) |
				( ( ( ( ( code1 & bm ) >> bl ) + ( ( code3 & bm ) >> bl ) ) >> 1 ) << bl ) |
				( ( ( ( ( code1 & am ) >> al ) + ( ( code3 & am ) >> al ) ) >> 1 ) << al ) ;

			dst = ( BYTE * )DestGraphData + DestX * pbyte + ( SrcY + i ) * DestPitch ;
			switch( bitdepth )
			{
			case 16 :
				*( ( WORD * )dst ) = ( WORD )dcode ;
				break ;

			case 32 :
				*( ( DWORD * )dst ) = dcode ;
				break ;
			}
		}
	}
	else
	if( SrcHeight == 1 )
	{
		// 縦が１の場合
		for( j = 0 ; j < bltw ; j ++ )
		{
			src = ( BYTE * )SrcGraphData + ( SrcX + ( j << 1 ) ) * pbyte + SrcY * SrcPitch ;
			switch( bitdepth )
			{
			case 16 :
				code1 = ( ( WORD * )src )[ 0 ] ;
				code2 = ( ( WORD * )src )[ 1 ] ;
				break ;

			case 32 :
				code1 = ( ( DWORD * )src )[ 0 ] ;
				code2 = ( ( DWORD * )src )[ 1 ] ;
				break ;

			default :
				return -1 ;
			}

			dcode = 
				( ( ( ( ( code1 & rm ) >> rl ) + ( ( code2 & rm ) >> rl ) ) >> 1 ) << rl ) |
				( ( ( ( ( code1 & gm ) >> gl ) + ( ( code2 & gm ) >> gl ) ) >> 1 ) << gl ) |
				( ( ( ( ( code1 & bm ) >> bl ) + ( ( code2 & bm ) >> bl ) ) >> 1 ) << bl ) |
				( ( ( ( ( code1 & am ) >> al ) + ( ( code2 & am ) >> al ) ) >> 1 ) << al ) ;

			dst = ( BYTE * )DestGraphData + ( DestX + j ) * pbyte + SrcY * DestPitch ;
			switch( bitdepth )
			{
			case 16 :
				*( ( WORD * )dst ) = ( WORD )dcode ;
				break ;

			case 32 :
				*( ( DWORD * )dst ) = dcode ;
				break ;
			}
		}
	}
	else
	{
		// 縦横どちらも１ではない場合
		for( i = 0 ; i < blth ; i ++ )
		{
			for( j = 0 ; j < bltw ; j ++ )
			{
				src = ( BYTE * )SrcGraphData + ( SrcX + ( j << 1 ) ) * pbyte + ( SrcY + ( i << 1 ) ) * SrcPitch ;
				switch( bitdepth )
				{
				case 16 :
					code1 = ( ( WORD * )src )[ 0 ] ;
					code2 = ( ( WORD * )src )[ 1 ] ;
					code3 = ( ( WORD * )( src + SrcPitch ) )[ 0 ] ;
					code4 = ( ( WORD * )( src + SrcPitch ) )[ 1 ] ;
					break ;

				case 32 :
					code1 = ( ( DWORD * )src )[ 0 ] ;
					code2 = ( ( DWORD * )src )[ 1 ] ;
					code3 = ( ( DWORD * )( src + SrcPitch ) )[ 0 ] ;
					code4 = ( ( DWORD * )( src + SrcPitch ) )[ 1 ] ;
					break ;

				default :
					return -1 ;
				}

				dcode = 
					( ( ( ( ( code1 & rm ) >> rl ) + ( ( code2 & rm ) >> rl ) + ( ( code3 & rm ) >> rl ) + ( ( code4 & rm ) >> rl ) ) >> 2 ) << rl ) |
					( ( ( ( ( code1 & gm ) >> gl ) + ( ( code2 & gm ) >> gl ) + ( ( code3 & gm ) >> gl ) + ( ( code4 & gm ) >> gl ) ) >> 2 ) << gl ) |
					( ( ( ( ( code1 & bm ) >> bl ) + ( ( code2 & bm ) >> bl ) + ( ( code3 & bm ) >> bl ) + ( ( code4 & bm ) >> bl ) ) >> 2 ) << bl ) |
					( ( ( ( ( code1 & am ) >> al ) + ( ( code2 & am ) >> al ) + ( ( code3 & am ) >> al ) + ( ( code4 & am ) >> al ) ) >> 2 ) << al ) ;

				dst = ( BYTE * )DestGraphData + ( DestX + j ) * pbyte + ( SrcY + i ) * DestPitch ;
				switch( bitdepth )
				{
				case 16 :
					*( ( WORD * )dst ) = ( WORD )dcode ;
					break ;

				case 32 :
					*( ( DWORD * )dst ) = dcode ;
					break ;
				}
			}
		}
	}

	// 終了
	return 0 ;
}

// 使われていないビットを指定の値で埋める
extern int NoneMaskFill( RECT *Rect, void *ImageData, int Pitch, COLORDATA *ColorData, unsigned int Fill )
{
	DWORD Width, Height;
	DWORD AddPitch;
	BYTE *DestP;
	DWORD PixelByte, FillColor;

	// 使われていないビットがない場合は何もしない
	if( ColorData->NoneMask == 0 ) return 0;

	// 処理矩形のサイズを得る
	RectAdjust( Rect );
	GetRectSize( Rect, ( int * )&Width, ( int * )&Height );

	// 転送処理の準備
	PixelByte = ColorData->PixelByte;
	DestP     = ( BYTE * )ImageData + Rect->left * PixelByte + Rect->top * Pitch;
	AddPitch  = Pitch - Width * PixelByte;
	FillColor = Fill << ColorData->NoneLoc;

	// 転送
#ifdef DX_NON_INLINE_ASM
	int i ;
	switch( PixelByte )
	{
	case 4 :
		if( ColorData->NoneMask == 0xff000000 )
		{
			do
			{
				i = ( int )Width ;
				do
				{
					DestP[ 3 ] = ( BYTE )Fill ;
					DestP += 4 ;
				}while( -- i != 0 ) ;
				DestP += AddPitch ;
			}while( -- Height != 0 ) ;
		}
		else
		{
			do
			{
				i = ( int )Width ;
				do
				{
					*( ( DWORD * )DestP ) |= FillColor ;
					DestP += 4 ;
				}while( -- i != 0 ) ;
				DestP += AddPitch ;
			}while( -- Height != 0 ) ;
		}
		break ;

	case 2 :
		do
		{
			i = ( int )Width ;
			do
			{
				*( ( WORD * )DestP ) |= ( WORD )FillColor ;
				DestP += 2 ;
			}while( -- i != 0 ) ;
			DestP += AddPitch ;
		}while( -- Height != 0 ) ;
		break ;
	}
#else
	switch( PixelByte )
	{
	case 4:
		if( ColorData->NoneMask == 0xff000000 )
		{
			__asm
			{
				MOV EDI, DestP
				MOV EDX, Height
				MOV EBX, AddPitch
				MOV EAX, Fill
		LOOP1_32_1:
				MOV ECX, Width
		LOOP2_32_1:
				OR  [EDI+3], AL
				ADD EDI, 4
				DEC ECX
				JNZ LOOP2_32_1

				ADD EDI, EBX
				DEC EDX
				JNZ LOOP1_32_1
			}
		}
		else
		{
			__asm
			{
				MOV EDI, DestP
				MOV EDX, Height
				MOV EBX, AddPitch
				MOV EAX, FillColor
		LOOP1_32_2:
				MOV ECX, Width
		LOOP2_32_2:
				OR  [EDI], EAX
				ADD EDI, 4
				DEC ECX
				JNZ LOOP2_32_2

				ADD EDI, EBX
				DEC EDX
				JNZ LOOP1_32_2
			}
		}
		break;

	case 2:
		__asm
		{
			MOV EDI, DestP
			MOV EDX, Height
			MOV EBX, AddPitch
			MOV EAX, FillColor
	LOOP1_16:
			MOV ECX, Width
	LOOP2_16:
			OR  [EDI], AX
			ADD EDI, 2
			DEC ECX
			JNZ LOOP2_16

			ADD EDI, EBX
			DEC EDX
			JNZ LOOP1_16
		}
		break;
	}
#endif

	return 0;
}

// YV12 フォーマットのイメージを XRGB32 のビットマップイメージに変換する
extern int ConvertYV12ToXRGB32( void *YV12Image, int Width, int Height, BASEIMAGE *DestBaseImage )
{
	int i, j ;
	BYTE *SrcY ;
	BYTE *SrcU ;
	BYTE *SrcV ;
	BYTE *SrcYY ;
	BYTE *SrcUY ;
	BYTE *SrcVY ;
	BYTE *Dest ;
	BYTE *DestY ;
	DWORD DestPitch2 ;
	DWORD DestPitch ;
	DWORD SrcYPitch2 ;
	DWORD SrcYPitch ;
	DWORD SrcVUPitch ;
	int U, V, Y, C ;
	int UB, VUG, VR ;

	SrcYY = ( BYTE * )YV12Image ;
	SrcVY = ( BYTE * )SrcYY + Width * Height ;
	SrcUY = ( BYTE * )SrcVY + Width * Height / 4 ;

	SrcYPitch  = ( DWORD )Width ;
	SrcYPitch2 = ( DWORD )( SrcYPitch * 2 ) ;

	DestPitch  = ( DWORD )DestBaseImage->Pitch ;
	DestPitch2 = DestPitch * 2 ;
	SrcVUPitch = SrcYPitch / 2 ;
	DestY = ( BYTE * )DestBaseImage->GraphData ;

	Width /= 2 ;
	Height /= 2 ;
	for( i = 0 ; i < Height ; i ++, DestY += DestPitch2, SrcYY += SrcYPitch2, SrcVY += SrcVUPitch, SrcUY += SrcVUPitch )
	{
		SrcY = SrcYY ;
		SrcV = SrcVY ;
		SrcU = SrcUY ;
		Dest = DestY ;
		for( j = 0 ; j < Width ; j ++, Dest += 8 )
		{
			U = ( int )SrcU[ 0 ] - 128 ;
			V = ( int )SrcV[ 0 ] - 128 ;

			UB  = U *  58064 ;				//  1.772
			VUG = U * -11272 + V * -23396 ;	// -0.344 -0.714
			VR  = V *  45940 ;				//  1.402

			Y = SrcY[ 0 ] << 15 ;
			C = Y + UB  ; Dest[             0 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // B
			C = Y + VUG ; Dest[             1 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // G
			C = Y + VR  ; Dest[             2 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // R
			Dest[             3 ] = 255 ;

			Y = SrcY[ 1 ] << 15 ;
			C = Y + UB  ; Dest[             4 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // B
			C = Y + VUG ; Dest[             5 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // G
			C = Y + VR  ; Dest[             6 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // R
			Dest[             7 ] = 255 ;

			Y = SrcY[ SrcYPitch + 0 ] << 15 ;
			C = Y + UB  ; Dest[ DestPitch + 0 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // B
			C = Y + VUG ; Dest[ DestPitch + 1 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // G
			C = Y + VR  ; Dest[ DestPitch + 2 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // R
			Dest[ DestPitch + 3 ] = 255 ;

			Y = SrcY[ SrcYPitch + 1 ] << 15 ;
			C = Y + UB  ; Dest[ DestPitch + 4 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // B
			C = Y + VUG ; Dest[ DestPitch + 5 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // G
			C = Y + VR  ; Dest[ DestPitch + 6 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // R
			Dest[ DestPitch + 7 ] = 255 ;

			SrcY += 2 ;
			SrcV ++ ;
			SrcU ++ ;
		}
	}

	// 終了
	return 0 ;
}

// NV11 フォーマットのイメージを XRGB32 のビットマップイメージに変換する
extern int ConvertNV11ToXRGB32( void *NV11Image, int Width, int Height, BASEIMAGE *DestBaseImage )
{
	int i, j ;
	BYTE *SrcY ;
	BYTE *SrcUV ;
	BYTE *SrcYY ;
	BYTE *SrcUVY ;
	BYTE *Dest ;
	BYTE *DestY ;
	DWORD DestPitch ;
	DWORD SrcYPitch ;
	DWORD SrcVUPitch ;
	int U, V, Y ;
	int UB, VUG, VR, C ;

	SrcYY  = ( BYTE * )NV11Image ;
	SrcUVY = ( BYTE * )SrcYY + Width * Height ;

	SrcYPitch  = ( DWORD )Width ;

	DestPitch  = ( DWORD )DestBaseImage->Pitch ;
	SrcVUPitch = SrcYPitch / 2 ;
	DestY = ( BYTE * )DestBaseImage->GraphData ;

	Width /= 4 ;
	for( i = 0 ; i < Height ; i ++, DestY += DestPitch, SrcYY += SrcYPitch, SrcUVY += SrcVUPitch )
	{
		SrcY = SrcYY ;
		SrcUV = SrcUVY ;
		Dest = DestY ;
		for( j = 0 ; j < Width ; j ++, Dest += 16 )
		{
			U = ( int )SrcUV[ 0 ] - 128 ;
			V = ( int )SrcUV[ 1 ] - 128 ;

			UB  = U *  58064 ;				//  1.772
			VUG = U * -11272 + V * -23396 ;	// -0.344 -0.714
			VR  = V *  45940 ;				//  1.402

			Y = SrcY[ 0 ] << 15 ;
			C = Y + UB  ; Dest[  0 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // B
			C = Y + VUG ; Dest[  1 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // G
			C = Y + VR  ; Dest[  2 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // R
			Dest[ 3 ] = 255 ;

			Y = SrcY[ 1 ] << 15 ;
			C = Y + UB  ; Dest[  4 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // B
			C = Y + VUG ; Dest[  5 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // G
			C = Y + VR  ; Dest[  6 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // R
			Dest[ 7 ] = 255 ;

			Y = SrcY[ 2 ] << 15 ;
			C = Y + UB  ; Dest[  8 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // B
			C = Y + VUG ; Dest[  9 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // G
			C = Y + VR  ; Dest[ 10 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // R
			Dest[ 11 ] = 255 ;

			Y = SrcY[ 3 ] << 15 ;
			C = Y + UB  ; Dest[ 12 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // B
			C = Y + VUG ; Dest[ 13 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // G
			C = Y + VR  ; Dest[ 14 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // R
			Dest[ 15 ] = 255 ;

			SrcY += 4 ;
			SrcUV += 2 ;
		}
	}

	// 終了
	return 0 ;
}

// NV12 フォーマットのイメージを XRGB32 のビットマップイメージに変換する
extern int ConvertNV12ToXRGB32( void *NV12Image, int Width, int Height, BASEIMAGE *DestBaseImage )
{
	int i, j ;
	BYTE *SrcY ;
	BYTE *SrcUV ;
	BYTE *SrcYY ;
	BYTE *SrcUVY ;
	BYTE *Dest ;
	BYTE *DestY ;
	DWORD DestPitch2 ;
	DWORD DestPitch ;
	DWORD SrcYPitch2 ;
	DWORD SrcYPitch ;
	DWORD SrcVUPitch ;
	int U, V, Y ;
	int UB, VUG, VR, C ;

	SrcYY = ( BYTE * )NV12Image ;
	SrcUVY = ( BYTE * )SrcYY + Width * Height ;

	SrcYPitch  = ( DWORD )Width ;
	SrcYPitch2 = SrcYPitch * 2 ;

	DestPitch  = ( DWORD )DestBaseImage->Pitch ;
	DestPitch2 = DestPitch * 2 ;
	SrcVUPitch = SrcYPitch ;
	DestY = ( BYTE * )DestBaseImage->GraphData ;

	Width /= 2 ;
	Height /= 2 ;
	for( i = 0 ; i < Height ; i ++, DestY += DestPitch2, SrcYY += SrcYPitch2, SrcUVY += SrcVUPitch )
	{
		SrcY = SrcYY ;
		SrcUV = SrcUVY ;
		Dest = DestY ;
		for( j = 0 ; j < Width ; j ++, Dest += 8 )
		{
			U = ( int )SrcUV[ 0 ] - 128 ;
			V = ( int )SrcUV[ 1 ] - 128 ;

			UB  = U *  58064 ;				//  1.772
			VUG = U * -11272 + V * -23396 ;	// -0.344 -0.714
			VR  = V *  45940 ;				//  1.402

			Y = SrcY[ 0 ] << 15 ;
			C = Y + UB  ; Dest[             0 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // B
			C = Y + VUG ; Dest[             1 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // G
			C = Y + VR  ; Dest[             2 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // R
			Dest[             3 ] = 255 ;

			Y = SrcY[ 1 ] << 15 ;
			C = Y + UB  ; Dest[             4 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // B
			C = Y + VUG ; Dest[             5 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // G
			C = Y + VR  ; Dest[             6 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // R
			Dest[             7 ] = 255 ;

			Y = SrcY[ SrcYPitch + 0 ] << 15 ;
			C = Y + UB  ; Dest[ DestPitch + 0 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // B
			C = Y + VUG ; Dest[ DestPitch + 1 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // G
			C = Y + VR  ; Dest[ DestPitch + 2 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // R
			Dest[ DestPitch + 3 ] = 255 ;

			Y = SrcY[ SrcYPitch + 1 ] << 15 ;
			C = Y + UB  ; Dest[ DestPitch + 4 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // B
			C = Y + VUG ; Dest[ DestPitch + 5 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // G
			C = Y + VR  ; Dest[ DestPitch + 6 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // R
			Dest[ DestPitch + 7 ] = 255 ;

			SrcY += 2 ;
			SrcUV += 2 ;
		}
	}

	// 終了
	return 0 ;
}

// YUY2 フォーマットのイメージを XRGB32 のビットマップイメージに変換する
extern int ConvertYUY2ToXRGB32( void *YUY2Image, int Width, int Height, BASEIMAGE *DestBaseImage )
{
	int i, j ;
	BYTE *Src ;
	BYTE *SrcY ;
	BYTE *Dest ;
	BYTE *DestY ;
	DWORD DestPitch ;
	DWORD SrcPitch ;
	int U, V, Y ;
	int UB, VUG, VR, C ;

	SrcY = ( BYTE * )YUY2Image ;

	SrcPitch  = ( DWORD )( Width * 2 ) ;
	DestPitch = ( DWORD )DestBaseImage->Pitch ;
	DestY     = ( BYTE * )DestBaseImage->GraphData ;

	Width /= 2 ;
	for( i = 0 ; i < Height ; i ++, DestY += DestPitch, SrcY += SrcPitch )
	{
		Src = SrcY ;
		Dest = DestY ;
		for( j = 0 ; j < Width ; j ++, Dest += 8, Src += 4 )
		{
			U = ( int )Src[ 1 ] - 128 ;
			V = ( int )Src[ 3 ] - 128 ;

			UB  = U *  58064 ;				//  1.772
			VUG = U * -11272 + V * -23396 ;	// -0.344 -0.714
			VR  = V *  45940 ;				//  1.402

			Y = Src[ 0 ] << 15 ;
			C = Y + UB  ; Dest[ 0 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // B
			C = Y + VUG ; Dest[ 1 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // G
			C = Y + VR  ; Dest[ 2 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // R
			Dest[ 3 ] = 255 ;

			Y = Src[ 2 ] << 15 ;
			C = Y + UB  ; Dest[ 4 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // B
			C = Y + VUG ; Dest[ 5 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // G
			C = Y + VR  ; Dest[ 6 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // R
			Dest[ 7 ] = 255 ;
		}
	}

	// 終了
	return 0 ;
}

// UYVY フォーマットのイメージを XRGB32 のビットマップイメージに変換する
extern int ConvertUYVYToXRGB32( void *UYVYImage, int Width, int Height, BASEIMAGE *DestBaseImage )
{
	int i, j ;
	BYTE *Src ;
	BYTE *SrcY ;
	BYTE *Dest ;
	BYTE *DestY ;
	DWORD DestPitch ;
	DWORD SrcPitch ;
	int U, V, Y ;
	int UB, VUG, VR, C ;

	SrcY = ( BYTE * )UYVYImage ;

	SrcPitch  = ( DWORD )( Width * 2 ) ;
	DestPitch = ( DWORD )DestBaseImage->Pitch ;
	DestY     = ( BYTE * )DestBaseImage->GraphData ;

	Width /= 2 ;
	for( i = 0 ; i < Height ; i ++, DestY += DestPitch, SrcY += SrcPitch )
	{
		Src = SrcY ;
		Dest = DestY ;
		for( j = 0 ; j < Width ; j ++, Dest += 8, Src += 4 )
		{
			U = ( int )Src[ 0 ] - 128 ;
			V = ( int )Src[ 2 ] - 128 ;

			UB  = U *  58064 ;				//  1.772
			VUG = U * -11272 + V * -23396 ;	// -0.344 -0.714
			VR  = V *  45940 ;				//  1.402

			Y = Src[ 1 ] << 15 ;
			C = Y + UB  ; Dest[ 0 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // B
			C = Y + VUG ; Dest[ 1 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // G
			C = Y + VR  ; Dest[ 2 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // R
			Dest[ 3 ] = 255 ;

			Y = Src[ 3 ] << 15 ;
			C = Y + UB  ; Dest[ 4 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // B
			C = Y + VUG ; Dest[ 5 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // G
			C = Y + VR  ; Dest[ 6 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // R
			Dest[ 7 ] = 255 ;
		}
	}

	// 終了
	return 0 ;
}

// YVYU フォーマットのイメージを XRGB32 のビットマップイメージに変換する
extern int ConvertYVYUToXRGB32( void *YVYUImage, int Width, int Height, BASEIMAGE *DestBaseImage )
{
	int i, j ;
	BYTE *Src ;
	BYTE *SrcY ;
	BYTE *Dest ;
	BYTE *DestY ;
	DWORD DestPitch ;
	DWORD SrcPitch ;
	int U, V, Y ;
	int UB, VUG, VR, C ;

	SrcY = ( BYTE * )YVYUImage ;

	SrcPitch  = ( DWORD )( Width * 2 ) ;
	DestPitch = ( DWORD )DestBaseImage->Pitch ;
	DestY     = ( BYTE * )DestBaseImage->GraphData ;

	Width /= 2 ;
	for( i = 0 ; i < Height ; i ++, DestY += DestPitch, SrcY += SrcPitch )
	{
		Src = SrcY ;
		Dest = DestY ;
		for( j = 0 ; j < Width ; j ++, Dest += 8, Src += 4 )
		{
			U = ( int )Src[ 3 ] - 128 ;
			V = ( int )Src[ 1 ] - 128 ;

			UB  = U *  58064 ;				//  1.772
			VUG = U * -11272 + V * -23396 ;	// -0.344 -0.714
			VR  = V *  45940 ;				//  1.402

			Y = Src[ 0 ] << 15 ;
			C = Y + UB  ; Dest[ 0 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // B
			C = Y + VUG ; Dest[ 1 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // G
			C = Y + VR  ; Dest[ 2 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // R
			Dest[ 3 ] = 255 ;

			Y = Src[ 2 ] << 15 ;
			C = Y + UB  ; Dest[ 4 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // B
			C = Y + VUG ; Dest[ 5 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // G
			C = Y + VR  ; Dest[ 6 ] = ( BYTE )( C < 0 ? 0 : ( C > 0x7F8000 ? 255 : ( C >> 15 ) ) ) ; // R
			Dest[ 7 ] = 255 ;
		}
	}

	// 終了
	return 0 ;
}

// YV12 フォーマットのイメージを Y成分とUV成分のイメージに分離する
extern int ConvertYV12ToYPlane_UVPlane( void *YV12Image, int Width, int Height, void *YBuffer, void *UVBuffer )
{
	int i, j ;
	BYTE *SrcY ;
	BYTE *SrcU ;
	BYTE *SrcV ;
	BYTE *DestY ;
	BYTE *DestUV ;
	DWORD DestYPitch ;
	DWORD DestYPitch2 ;
	DWORD DestUVPitch ;
	DWORD SrcYPitch2 ;
	DWORD SrcYPitch ;
	DWORD SrcVUPitch ;

	SrcY = ( BYTE * )YV12Image ;
	SrcV = ( BYTE * )SrcY + Width * Height ;
	SrcU = ( BYTE * )SrcV + Width * Height / 4 ;

	SrcYPitch  = ( DWORD )Width ;
	SrcYPitch2 = ( DWORD )( SrcYPitch * 2 ) ;

	DestYPitch  = Width ;
	DestYPitch2 = DestYPitch * 2 ;
	DestUVPitch = Width ;
	SrcVUPitch = SrcYPitch / 2 ;
	DestY  = ( BYTE * )YBuffer ;
	DestUV = ( BYTE * )UVBuffer ;

	Width /= 2 ;
	Height /= 2 ;
	for( i = 0 ; i < Height ; i ++, DestY += DestYPitch2, DestUV += DestUVPitch, SrcY += SrcYPitch2, SrcV += SrcVUPitch, SrcU += SrcVUPitch )
	{
		BYTE *SrcYTemp ;
		BYTE *SrcVTemp ;
		BYTE *SrcUTemp ;
		BYTE *DestYTemp ;
		BYTE *DestUVTemp ;

		SrcYTemp = SrcY ;
		SrcVTemp = SrcV ;
		SrcUTemp = SrcU ;
		DestYTemp = DestY ;
		DestUVTemp = DestUV ;
		for( j = 0 ; j < Width ; j ++, DestYTemp += 2, DestUVTemp += 2 )
		{
			DestYTemp[              0 ] = SrcYTemp[             0 ] ;
			DestYTemp[              1 ] = SrcYTemp[             1 ] ;
			DestYTemp[ DestYPitch + 0 ] = SrcYTemp[ SrcYPitch + 0 ] ;
			DestYTemp[ DestYPitch + 1 ] = SrcYTemp[ SrcYPitch + 1 ] ;

			DestUVTemp[ 0 ] = SrcUTemp[ 0 ] ;
			DestUVTemp[ 1 ] = SrcVTemp[ 0 ] ;

			SrcYTemp += 2 ;
			SrcVTemp ++ ;
			SrcUTemp ++ ;
		}
	}

	// 終了
	return 0 ;
}

// NV11 フォーマットのイメージを Y成分とUV成分のイメージに分離する
extern int ConvertNV11ToYPlane_UVPlane( void *NV11Image, int Width, int Height, void *YBuffer, void *UVBuffer )
{
	int i, j ;
	BYTE *SrcY ;
	BYTE *SrcUV ;
	BYTE *DestY ;
	BYTE *DestUV ;
	DWORD DestYPitch ;
	DWORD DestUVPitch ;
	DWORD SrcYPitch ;
	DWORD SrcVUPitch ;

	SrcY  = ( BYTE * )NV11Image ;
	SrcUV = ( BYTE * )SrcY + Width * Height ;

	SrcYPitch  = ( DWORD )Width ;
	SrcVUPitch = SrcYPitch / 2 ;

	DestYPitch  = ( DWORD )Width ;
	DestUVPitch = ( DWORD )Width / 2 ; 

	DestY  = ( BYTE * )YBuffer ;
	DestUV = ( BYTE * )UVBuffer ;

	Width /= 4 ;
	for( i = 0 ; i < Height ; i ++, DestY += DestYPitch, DestUV += DestUVPitch, SrcY += SrcYPitch, SrcUV += SrcVUPitch )
	{
		BYTE *SrcYTemp ;
		BYTE *SrcUVTemp ;
		BYTE *DestYTemp ;
		BYTE *DestUVTemp ;

		SrcYTemp   = SrcY ;
		SrcUVTemp  = SrcUV ;
		DestYTemp  = DestY ;
		DestUVTemp = DestUV ;
		for( j = 0 ; j < Width ; j ++, DestYTemp += 4, DestUVTemp += 2 )
		{
			DestYTemp[ 0 ] = SrcYTemp[ 0 ] ;
			DestYTemp[ 1 ] = SrcYTemp[ 1 ] ;
			DestYTemp[ 2 ] = SrcYTemp[ 2 ] ;
			DestYTemp[ 3 ] = SrcYTemp[ 3 ] ;

			DestUVTemp[ 0 ] = SrcUVTemp[ 0 ] ;
			DestUVTemp[ 1 ] = SrcUVTemp[ 1 ] ;

			SrcYTemp  += 4 ;
			SrcUVTemp += 2 ;
		}
	}

	// 終了
	return 0 ;
}

// NV12 フォーマットのイメージを Y成分とUV成分のイメージに分離する
extern int ConvertNV12ToYPlane_UVPlane( void *NV12Image, int Width, int Height, void *YBuffer, void *UVBuffer )
{
	int i, j ;
	BYTE *SrcY ;
	BYTE *SrcUV ;
	BYTE *DestY ;
	BYTE *DestUV ;
	DWORD DestYPitch2 ;
	DWORD DestYPitch ;
	DWORD DestUVPitch ;
	DWORD SrcYPitch2 ;
	DWORD SrcYPitch ;
	DWORD SrcUVPitch ;

	SrcY  = ( BYTE * )NV12Image ;
	SrcUV = ( BYTE * )SrcY + Width * Height ;

	SrcYPitch  = ( DWORD )Width ;
	SrcYPitch2 = SrcYPitch * 2 ;
	SrcUVPitch = SrcYPitch ;

	DestYPitch  = Width ;
	DestYPitch2 = DestYPitch * 2 ;
	DestUVPitch = Width ;

	DestY  = ( BYTE * )YBuffer ;
	DestUV = ( BYTE * )UVBuffer ;

	Width  /= 2 ;
	Height /= 2 ;
	for( i = 0 ; i < Height ; i ++, DestY += DestYPitch2, DestUV += DestUVPitch, SrcY += SrcYPitch2, SrcUV += SrcUVPitch )
	{
		BYTE *SrcYTemp ;
		BYTE *SrcUVTemp ;
		BYTE *DestYTemp ;
		BYTE *DestUVTemp ;

		SrcYTemp   = SrcY ;
		SrcUVTemp  = SrcUV ;
		DestYTemp  = DestY ;
		DestUVTemp = DestUV ;
		for( j = 0 ; j < Width ; j ++, DestYTemp += 2, DestUVTemp += 2 )
		{
			DestYTemp[              0 ] = SrcYTemp[             0 ] ;
			DestYTemp[              1 ] = SrcYTemp[             1 ] ;
			DestYTemp[ DestYPitch + 0 ] = SrcYTemp[ SrcYPitch + 0 ] ;
			DestYTemp[ DestYPitch + 1 ] = SrcYTemp[ SrcYPitch + 1 ] ;

			DestUVTemp[ 0 ] = SrcUVTemp[ 0 ] ;
			DestUVTemp[ 1 ] = SrcUVTemp[ 1 ] ;

			SrcYTemp  += 2 ;
			SrcUVTemp += 2 ;
		}
	}

	// 終了
	return 0 ;
}

// YUY2 フォーマットのイメージを Y成分とUV成分のイメージに分離する
extern int ConvertYUY2ToYPlane_UVPlane( void *YUY2Image, int Width, int Height, void *YBuffer, void *UVBuffer )
{
	int i, j ;
	BYTE *Src ;
	BYTE *DestY ;
	BYTE *DestUV ;
	DWORD DestYPitch ;
	DWORD DestUVPitch ;
	DWORD SrcPitch ;

	Src = ( BYTE * )YUY2Image ;

	SrcPitch  = ( DWORD )( Width * 2 ) ;

	DestYPitch  = ( DWORD )Width ;
	DestUVPitch = DestYPitch ;

	DestY  = ( BYTE * )YBuffer ;
	DestUV = ( BYTE * )UVBuffer ;

	Width /= 2 ;
	for( i = 0 ; i < Height ; i ++, DestY += DestYPitch, DestUV += DestUVPitch, Src += SrcPitch )
	{
		BYTE *SrcTemp ;
		BYTE *DestYTemp ;
		BYTE *DestUVTemp ;

		SrcTemp = Src ;
		DestYTemp = DestY ;
		DestUVTemp = DestUV ;
		for( j = 0 ; j < Width ; j ++, DestYTemp += 2, DestUVTemp += 2 )
		{
			DestYTemp[ 0 ] = SrcTemp[ 0 ] ;
			DestYTemp[ 1 ] = SrcTemp[ 2 ] ;

			DestUVTemp[ 0 ] = SrcTemp[ 1 ] ;
			DestUVTemp[ 1 ] = SrcTemp[ 3 ] ;

			SrcTemp += 4 ;
		}
	}

	// 終了
	return 0 ;
}

// UYVY フォーマットのイメージを Y成分とUV成分のイメージに分離する
extern int ConvertUYVYToYPlane_UVPlane( void *UYVYImage, int Width, int Height, void *YBuffer, void *UVBuffer )
{
	int i, j ;
	BYTE *Src ;
	BYTE *DestY ;
	BYTE *DestUV ;
	DWORD DestYPitch ;
	DWORD DestUVPitch ;
	DWORD SrcPitch ;

	Src = ( BYTE * )UYVYImage ;

	SrcPitch    = ( DWORD )( Width * 2 ) ;

	DestYPitch  = ( DWORD )Width ;
	DestUVPitch = ( DWORD )Width ;

	DestY       = ( BYTE * )YBuffer ;
	DestUV      = ( BYTE * )UVBuffer ;

	Width /= 2 ;
	for( i = 0 ; i < Height ; i ++, DestY += DestYPitch, DestUV += DestUVPitch, Src += SrcPitch )
	{
		BYTE *SrcTemp ;
		BYTE *DestYTemp ;
		BYTE *DestUVTemp ;

		SrcTemp    = Src ;
		DestYTemp  = DestY ;
		DestUVTemp = DestUV ;
		for( j = 0 ; j < Width ; j ++, DestYTemp += 2, DestUVTemp += 2 )
		{
			DestYTemp[ 0 ] = SrcTemp[ 1 ] ;
			DestYTemp[ 1 ] = SrcTemp[ 3 ] ;

			DestUVTemp[ 0 ] = SrcTemp[ 0 ] ;
			DestUVTemp[ 1 ] = SrcTemp[ 2 ] ;

			SrcTemp += 4 ;
		}
	}

	// 終了
	return 0 ;
}

// YVYU フォーマットのイメージを Y成分とUV成分のイメージに分離する
extern int ConvertYVYUToYPlane_UVPlane( void *YVYUImage, int Width, int Height, void *YBuffer, void *UVBuffer )
{
	int i, j ;
	BYTE *Src ;
	BYTE *DestY ;
	BYTE *DestUV ;
	DWORD DestYPitch ;
	DWORD DestUVPitch ;
	DWORD SrcPitch ;

	Src         = ( BYTE * )YVYUImage ;

	SrcPitch    = ( DWORD )( Width * 2 ) ;

	DestYPitch  = ( DWORD )Width ;
	DestUVPitch = ( DWORD )Width ;

	DestY       = ( BYTE * )YBuffer ;
	DestUV      = ( BYTE * )UVBuffer ;

	Width /= 2 ;
	for( i = 0 ; i < Height ; i ++, DestY += DestYPitch, DestUV += DestUVPitch, Src += SrcPitch )
	{
		BYTE *SrcTemp ;
		BYTE *DestYTemp ;
		BYTE *DestUVTemp ;

		SrcTemp = Src ;
		DestYTemp = DestY ;
		DestUVTemp = DestUV ;
		for( j = 0 ; j < Width ; j ++, DestYTemp += 2, DestUVTemp += 2 )
		{
			DestYTemp[ 0 ] = SrcTemp[ 0 ] ;
			DestYTemp[ 1 ] = SrcTemp[ 2 ] ;

			DestUVTemp[ 0 ] = SrcTemp[ 3 ] ;
			DestUVTemp[ 1 ] = SrcTemp[ 1 ] ;

			SrcTemp += 4 ;
		}
	}

	// 終了
	return 0 ;
}

// 16階調ディザ行列のテーブル
static int DitherTable[16] = {
	 0,  8,  2, 10,
	12,  4, 14,  6,
     3, 11,  1,  9,
    15,  7, 13,  5
};
/*	0,   8, 2, 10,
	12,  4,  14,  6,
    3, 11,  1,  9,
    15,   7, 13, 5
*/

/*	1,   3, 15, 13,
	9,  11,  6,  8,
    16, 14,  2,  4,
    5,   7, 10, 12
*/

// カラーマッチングしながらグラフィックデータ間転送を行う Ver2
extern int NS_GraphColorMatchBltVer2( void *DestGraphData,       int DestPitch,  const COLORDATA *DestColorData,
									  const void *SrcGraphData,  int SrcPitch,   const COLORDATA *SrcColorData,
									  const void *AlphaMask,     int AlphaPitch, const COLORDATA *AlphaColorData,
								 	  POINT DestPoint, const RECT *SrcRect, int ReverseFlag,
									  int TransColorAlphaTestFlag, unsigned int TransColor,
									  int ImageShavedMode, int AlphaOnlyFlag,
									  int RedIsAlphaFlag,  int TransColorNoMoveFlag, int Pal8ColorMatch )
{
	int BltWidth, BltHeight ;
	int AlphaFlag, AlphaMaskFlag ;
	COLORDATA SColor, AColor, DColor ;
	int FloatTypeFlag ;

	SColor = *SrcColorData ;
	DColor = *DestColorData ;
	if( AlphaColorData != NULL ) AColor = *AlphaColorData ; else _MEMSET( &AColor, 0, sizeof( AColor ) ) ;

	// 転送元と転送先のどちらかが浮動小数点型かどうかをセット
	FloatTypeFlag = SColor.FloatTypeFlag != 0 || DColor.FloatTypeFlag != 0 ? TRUE : FALSE ;

	// アルファチャンネルを処理するかどうかのフラグをセットする
	AlphaFlag = DColor.AlphaWidth != 0 && ( AlphaMask != NULL || SColor.AlphaWidth != 0 ) ;
	
	// アルファチャンネルデータはアルファマスクから取るかソースグラフィック上から取るかのフラグをセット
	AlphaMaskFlag = DColor.AlphaWidth != 0 && AlphaMask != NULL && SColor.AlphaWidth == 0 ;

	// 透過色アルファテスト処理は、アルファマスク、またはアルファチャンネルがある場合、
	// もしくは転送先グラフィックのフォーマットがアルファなしだった場合は強制的にＯＦＦになる
	// 又、赤成分をα成分に変換する指定がある場合もＯＦＦになる
	if( AlphaFlag == TRUE || DColor.AlphaWidth == 0 || RedIsAlphaFlag == TRUE  ) TransColorAlphaTestFlag = FALSE ;

	// 反転フラグがある場合は透過色処理は無し
	if( ReverseFlag ) TransColorNoMoveFlag = FALSE ;

	// 転送サイズをセット
	BltWidth = SrcRect->right - SrcRect->left ;
	BltHeight = SrcRect->bottom - SrcRect->top ;
	if( BltWidth == 0 || BltHeight == 0 ) return -1 ;

	// 整数型で1チャンネル辺り16ビットのフォーマットの場合は分岐
	if( ( SColor.FloatTypeFlag == FALSE && SColor.ChannelBitDepth == 16 ) ||
		( DColor.FloatTypeFlag == FALSE && DColor.ChannelBitDepth == 16 ) )
	{
		// 転送元と転送先のフォーマットが全く同じで特殊な処理も無い場合は単純転送を行う
		if( TransColorAlphaTestFlag == FALSE &&
			AlphaMaskFlag           == FALSE &&
			AlphaOnlyFlag           == FALSE &&
			RedIsAlphaFlag          == FALSE &&
			DColor.FloatTypeFlag    == SColor.FloatTypeFlag &&
			DColor.ChannelBitDepth  == SColor.ChannelBitDepth &&
			DColor.ChannelNum       == SColor.ChannelNum &&
			DColor.PixelByte        == SColor.PixelByte )
		{
			BYTE *DestP, *SrcP ;
			DWORD SrcAddPitch, DestAddPitch ;
			DWORD DwMoveSetNum ;
			DWORD MoveLineByte ;

			// １ライン当たりの転送サイズをセット
			MoveLineByte = ( DWORD )( BltWidth * DColor.PixelByte ) ;

			// 転送元、先アドレスセット
			DestP = ( BYTE * )DestGraphData + DestPoint.y  * DestPitch + DestPoint.x   * DColor.PixelByte ;
			SrcP  = ( BYTE * )SrcGraphData  + SrcRect->top * SrcPitch  + SrcRect->left * SColor.PixelByte ;

			// １ライン転送後、次の転送元アドレスまでのバイト数計算
			DestAddPitch = ( DWORD )( DestPitch - MoveLineByte ) ;
			SrcAddPitch  = ( DWORD )( SrcPitch  - MoveLineByte ) ;

			// ４バイト転送何回分あるか算出
			DwMoveSetNum = MoveLineByte / 4 ;

			DWORD i ;
			do
			{
				i = DwMoveSetNum ;
				do
				{
					*( ( DWORD * )DestP ) = *( ( DWORD * )SrcP ) ;
					DestP += 4 ;
					SrcP  += 4 ;
				}while( -- i != 0 ) ;

				DestP += DestAddPitch ;
				SrcP  += SrcAddPitch ;
			}while( -- BltHeight != 0 ) ;
		}

		// 低速転送を行う
		goto NORMALMOVE ;
	}

	// 浮動小数点型かどうかで処理を分岐
	if( FloatTypeFlag )
	{
		// 転送元と転送先のフォーマットが全く同じで特殊な処理も無い場合は単純転送を行う
		if( TransColorAlphaTestFlag == FALSE &&
			AlphaMaskFlag           == FALSE &&
			AlphaOnlyFlag           == FALSE &&
			RedIsAlphaFlag          == FALSE &&
			DColor.FloatTypeFlag    == SColor.FloatTypeFlag &&
			DColor.ChannelBitDepth  == SColor.ChannelBitDepth &&
			DColor.ChannelNum       == SColor.ChannelNum &&
			DColor.PixelByte        == SColor.PixelByte )
		{
			BYTE *DestP, *SrcP ;
			DWORD SrcAddPitch, DestAddPitch ;
			DWORD DwMoveSetNum ;
			DWORD MoveLineByte ;

			// １ライン当たりの転送サイズをセット
			MoveLineByte = ( DWORD )( BltWidth * DColor.PixelByte ) ;

			// 転送元、先アドレスセット
			DestP = ( BYTE * )DestGraphData + DestPoint.y  * DestPitch + DestPoint.x   * DColor.PixelByte ;
			SrcP  = ( BYTE * )SrcGraphData  + SrcRect->top * SrcPitch  + SrcRect->left * SColor.PixelByte ;

			// １ライン転送後、次の転送元アドレスまでのバイト数計算
			DestAddPitch = ( DWORD )( DestPitch - MoveLineByte ) ;
			SrcAddPitch  = ( DWORD )( SrcPitch  - MoveLineByte ) ;

			// ４バイト転送何回分あるか算出
			DwMoveSetNum = MoveLineByte / 4 ;

			DWORD i ;
			do
			{
				i = DwMoveSetNum ;
				do
				{
					*( ( DWORD * )DestP ) = *( ( DWORD * )SrcP ) ;
					DestP += 4 ;
					SrcP  += 4 ;
				}while( -- i != 0 ) ;

				DestP += DestAddPitch ;
				SrcP  += SrcAddPitch ;
			}while( -- BltHeight != 0 ) ;
		}

		// 低速転送を行う
		goto NORMALMOVE ;
	}

	// 転送先が 4bit の場合
	if( DestColorData->ColorBitDepth == 4 )
	{
		// 転送元は 8bit にしか対応しない
		if( SrcColorData->ColorBitDepth != 8 )
		{
			return -1 ;
		}

		// x軸方向の転送位置が２の倍数ではない場合はエラー
		if( DestPoint.x % 2 != 0 )
		{
			return -1 ;
		}

		BYTE *DestP ;
		BYTE *SrcP ;
		int SrcAddPitch ;
		int DestAddPitch ;
		int SingleBltNum ;
		int DoubleBltNum ;
	
		DestP = (BYTE *)DestGraphData + DestPitch * DestPoint.y  + DestPoint.x / 2 ;
		SrcP  = (BYTE *)SrcGraphData  + SrcPitch  * SrcRect->top + SrcRect->left ;
		
		DestAddPitch = DestPitch - BltWidth / 2 ;
		SrcAddPitch  = SrcPitch  - BltWidth ;

		DoubleBltNum = BltWidth / 2 ;
		SingleBltNum = BltWidth - DoubleBltNum * 2 ;

		int DoubleBltNumTemp ;
		do
		{
			DoubleBltNumTemp = DoubleBltNum ;
			do
			{
				*DestP = SrcP[ 0 ] | ( SrcP[ 1 ] << 4 ) ;
				DestP ++ ;
				SrcP  += 2 ;
			}while( -- DoubleBltNumTemp != 0 ) ;

			if( SingleBltNum != 0 )
			{
				*DestP = SrcP[ 0 ] ;
				SrcP ++ ;
			}

			DestP += DestAddPitch ;
			SrcP  += SrcAddPitch ;
		}while( -- BltHeight != 0 ) ;

		return 0 ;
	}

	// ８ビットカラー同士のカラーマッチ指定の場合はパレットが一致していない場合は低速転送処理を行う
	if( Pal8ColorMatch && DestColorData->ColorBitDepth == 8 && SrcColorData->ColorBitDepth == 8 && ReverseFlag == FALSE )
	{
		if( _MEMCMP( DestColorData->Palette, SrcColorData->Palette, sizeof( SrcColorData->Palette ) ) != 0 )
		{
			int i, j ;
			BYTE *src, *dest ;
			const COLORPALETTEDATA *srcp ;

			for( i = 0 ; i < BltHeight ; i ++ )
			{
				src  = ( BYTE * )SrcGraphData  + SrcPitch  * ( SrcRect->top + i ) + SrcRect->left ;
				dest = ( BYTE * )DestGraphData + DestPitch * ( DestPoint.y + i )  + DestPoint.x ;
				for( j = 0 ; j < BltWidth ; j ++, src ++, dest ++ )
				{
					srcp = &SrcColorData->Palette[ *src ] ;
					*dest = ( BYTE )NS_GetColor3( DestColorData, srcp->Red, srcp->Green, srcp->Blue, 0 ) ;
				}
			}
			return 0 ;
		}
	}

	// 転送先が８ビットで、転送元が８ビット以外だった場合は低速転送
	if( DestColorData->ColorBitDepth == 8 && SrcColorData->ColorBitDepth != 8 && RedIsAlphaFlag == FALSE && ReverseFlag == FALSE )
	{
		int i, j ;
		BYTE *src, *dest ;
		unsigned int Color ;

		switch( SrcColorData->ColorBitDepth )
		{
		case 16 :
			for( i = 0 ; i < BltHeight ; i ++ )
			{
				src  = ( BYTE * )SrcGraphData  + SrcPitch  * ( SrcRect->top + i ) + SrcRect->left * 2 ;
				dest = ( BYTE * )DestGraphData + DestPitch * ( DestPoint.y + i )  + DestPoint.x ;
				for( j = 0 ; j < BltWidth ; j ++, src += 2, dest ++ )
				{
					*dest = ( BYTE )NS_GetColor3( DestColorData,
						( int )( ( *( ( WORD * )src ) & SrcColorData->RedMask   ) >> SrcColorData->RedLoc   ),
						( int )( ( *( ( WORD * )src ) & SrcColorData->GreenMask ) >> SrcColorData->GreenLoc ),
						( int )( ( *( ( WORD * )src ) & SrcColorData->BlueMask  ) >> SrcColorData->BlueLoc  ),
						( int )( ( *( ( WORD * )src ) & SrcColorData->AlphaMask ) >> SrcColorData->AlphaLoc ) ) ;
				}
			}
			break ;

		case 24 :
			for( i = 0 ; i < BltHeight ; i ++ )
			{
				src  = ( BYTE * )SrcGraphData  + SrcPitch  * ( SrcRect->top + i ) + SrcRect->left * 2 ;
				dest = ( BYTE * )DestGraphData + DestPitch * ( DestPoint.y + i )  + DestPoint.x ;
				for( j = 0 ; j < BltWidth ; j ++, src += 3, dest ++ )
				{
					Color = ( DWORD )( *( ( WORD * )src ) | ( src[ 2 ] << 16 ) ) ;
					*dest = ( BYTE )NS_GetColor3( DestColorData,
						( int )( ( Color & SrcColorData->RedMask   ) >> SrcColorData->RedLoc   ),
						( int )( ( Color & SrcColorData->GreenMask ) >> SrcColorData->GreenLoc ),
						( int )( ( Color & SrcColorData->BlueMask  ) >> SrcColorData->BlueLoc  ),
						( int )( ( Color & SrcColorData->AlphaMask ) >> SrcColorData->AlphaLoc ) ) ;
				}
			}
			break ;

		case 32 :
			for( i = 0 ; i < BltHeight ; i ++ )
			{
				src  = ( BYTE * )SrcGraphData  + SrcPitch  * ( SrcRect->top + i ) + SrcRect->left * 4 ;
				dest = ( BYTE * )DestGraphData + DestPitch * ( DestPoint.y + i )  + DestPoint.x ;
				for( j = 0 ; j < BltWidth ; j ++, src += 4, dest ++ )
				{
					*dest = ( BYTE )NS_GetColor3( DestColorData,
						( int )( ( *( ( DWORD * )src ) & SrcColorData->RedMask   ) >> SrcColorData->RedLoc   ),
						( int )( ( *( ( DWORD * )src ) & SrcColorData->GreenMask ) >> SrcColorData->GreenLoc ),
						( int )( ( *( ( DWORD * )src ) & SrcColorData->BlueMask  ) >> SrcColorData->BlueLoc  ),
						( int )( ( *( ( DWORD * )src ) & SrcColorData->AlphaMask ) >> SrcColorData->AlphaLoc ) ) ;
				}
			}
			break ;
		}
		return 0 ;
	}

	// 転送先が８ビットだったら処理を分岐
	if( DestColorData->ColorBitDepth == 8 && AlphaOnlyFlag == FALSE && TransColorNoMoveFlag == FALSE && RedIsAlphaFlag == FALSE && ReverseFlag == FALSE )
	{
		BYTE *DestP, *SrcP ;
		int SrcAddPitch, DestAddPitch ;
		DWORD MaxColorNumber ;

		// 転送元が８ビット以外だったらエラー
		if( SrcColorData->ColorBitDepth != 8 ) return -1 ;
	
		// カラーマッチングは行わず、ただ転送するだけ
		DestP = (BYTE *)DestGraphData + DestPitch * DestPoint.y + DestPoint.x ;
		SrcP = (BYTE *)SrcGraphData + SrcPitch * SrcRect->top + SrcRect->left ;
		
		SrcAddPitch = SrcPitch - BltWidth ;
		DestAddPitch = DestPitch - BltWidth ;
#ifdef DX_NON_INLINE_ASM
		int BltWidthTemp ;
		MaxColorNumber = 0 ;
		do
		{
			BltWidthTemp = BltWidth ;
			do
			{
				if( MaxColorNumber < *SrcP )
					MaxColorNumber = *SrcP ;
				*DestP ++ = *SrcP ++ ;
			}while( -- BltWidthTemp != 0 ) ;
			DestP += DestAddPitch ;
			SrcP += SrcAddPitch ;
		}while( -- BltHeight != 0 ) ;
#else
		_asm
		{
			PUSHF
			CLD
			MOV EDI, DestP
			MOV ESI, SrcP
			MOV EBX, BltHeight
			MOV EAX, 0
		LABEL_PAL_IMAGE_MOVE_1:
			MOV ECX, BltWidth
		LABEL_PAL_IMAGE_MOVE_2:
			MOVZX EDX, [ESI]
			MOV [EDI], DL
			CMP EDX, EAX
			JNA LABEL_PAL_IMAGE_MOVE_3
			MOV EAX, EDX
		LABEL_PAL_IMAGE_MOVE_3:
			INC ESI
			INC EDI
			DEC ECX
			JNZ LABEL_PAL_IMAGE_MOVE_2

			ADD EDI, DestAddPitch
			ADD ESI, SrcAddPitch
			DEC EBX
			JNZ LABEL_PAL_IMAGE_MOVE_1
			MOV MaxColorNumber, EAX
			POPF
		}
#endif
		// 使用している色数を返す
		return ( int )MaxColorNumber ;
	}

	// 透過色アルファテストフラグが倒れていてピクセルフォーマットが同じ
	// だった場合は高速転送プログラムを走らせる
	if( TransColorAlphaTestFlag == FALSE &&
		AlphaMaskFlag           == FALSE &&
		AlphaOnlyFlag           == FALSE &&
		RedIsAlphaFlag          == FALSE &&
		DColor.ChannelNum       == 0                    &&
		SColor.ChannelNum       == 0                    &&
		DColor.ColorBitDepth    == SColor.ColorBitDepth &&
		DColor.RedMask          == SColor.RedMask       &&
		DColor.GreenMask        == SColor.GreenMask     &&
		DColor.BlueMask         == SColor.BlueMask      &&
		DColor.AlphaMask        == SColor.AlphaMask )
	{
		// 高速転送処理プログラム

		BYTE *DestP, *SrcP ;
		int SrcAddPitch, DestAddPitch ;
		int DwMoveSetNum, NokoriMoveSetNum ;
		int MoveLineByte, ColorBitDepth ;

		// １ライン当たりの転送サイズをセット
		MoveLineByte = BltWidth * DColor.PixelByte ;
		
		// 反転転送かどうかで処理を分岐
		if( ReverseFlag == FALSE )
		{
			// 転送元、先アドレスセット
			DestP = ( BYTE * )DestGraphData + DestPoint.y  * DestPitch + DestPoint.x   * DColor.PixelByte ;
			SrcP  = ( BYTE * )SrcGraphData  + SrcRect->top * SrcPitch  + SrcRect->left * SColor.PixelByte ;

			// １ライン転送後、次の転送元アドレスまでのバイト数計算
			DestAddPitch = DestPitch - MoveLineByte ;
			SrcAddPitch  = SrcPitch  - MoveLineByte ;

			// 透過色処理を行うかどうかで処理を分岐
			if( TransColorNoMoveFlag == TRUE )
			{
				// 透過色処理を行う場合

				// 色ビット数をセット
				ColorBitDepth = DColor.ColorBitDepth ;

#ifdef DX_NON_INLINE_ASM
				int i ;
				switch( ColorBitDepth )
				{
				case 32 :
					TransColor &= 0xffff ;
					do
					{
						i = BltWidth ;
						do
						{
							if( *( ( DWORD * )SrcP ) != TransColor )
							{
								*( ( DWORD * )DestP ) = *( ( DWORD * )SrcP ) ;
							}
							DestP += 4 ;
							SrcP += 4 ;
						}while( -- i != 0 ) ;
						DestP += DestAddPitch ;
						SrcP += SrcAddPitch ;
					}while( -- BltHeight != 0 ) ;
					break ;

				case 24 :
					DWORD SrcC ;
					TransColor &= 0xffff ;
					do
					{
						i = BltWidth ;
						do
						{
							SrcC = ( DWORD )( *( ( WORD * )SrcP ) | ( SrcP[ 2 ] << 16 ) ) ;
							if( SrcC != TransColor )
							{
								*( ( WORD * )DestP ) = *( ( WORD * )SrcP ) ;
								DestP[ 2 ] = SrcP[ 2 ] ;
							}
							DestP += 3 ;
							SrcP += 3 ;
						}while( -- i != 0 ) ;
						DestP += DestAddPitch ;
						SrcP += SrcAddPitch ;
					}while( -- BltHeight != 0 ) ;
					break ;

				case 16 :
					TransColor &= 0xffff ;
					do
					{
						i = BltWidth ;
						do
						{
							if( *( ( WORD * )SrcP ) != TransColor )
							{
								*( ( WORD * )DestP ) = *( ( WORD * )SrcP ) ;
							}
							DestP += 2 ;
							SrcP += 2 ;
						}while( -- i != 0 ) ;
						DestP += DestAddPitch ;
						SrcP += SrcAddPitch ;
					}while( -- BltHeight != 0 ) ;
					break ;

				case 8 :
					TransColor &= 0xff ;
					do
					{
						i = BltWidth ;
						do
						{
							if( *SrcP != TransColor )
							{
								*DestP = *SrcP ;
							}
							DestP ++ ;
							SrcP ++ ;
						}while( -- i != 0 ) ;
						DestP += DestAddPitch ;
						SrcP += SrcAddPitch ;
					}while( -- BltHeight != 0 ) ;
					break ;
				}
#else
				// 転送処理
				_asm{
					PUSHA
					PUSHF
					CLD
					MOV		EDI, DestP
					MOV		ESI, SrcP
					MOV		EBX, TransColor
					MOV		EDX, BltHeight
					CMP		ColorBitDepth, 8
					JZ		CR8_a_TR
					CMP		ColorBitDepth, 16
					JZ		CR16_a_TR
					CMP		ColorBitDepth, 24
					JZ		CR24_a_TR


				CR32_a_TR:
					MOV		ECX, BltWidth
				CR32_b_TR:
					MOV		EAX, [ESI]
					CMP		EAX, EBX
					JE		CR32_c_TR
					MOV		[EDI], EAX
				CR32_c_TR:
					ADD		ESI, 4
					ADD		EDI, 4
					DEC		ECX
					JNZ		CR32_b_TR
					ADD		EDI, DestAddPitch
					ADD		ESI, SrcAddPitch
					DEC		EDX
					JNZ		CR32_a_TR
					JMP		END_a_TR
					

				CR24_a_TR:
					MOV		ECX, BltWidth
					AND		EBX, 0xffffff
				CR24_b_TR:
					XOR		EAX, EAX
					MOV		AL, [ESI+2]
					SHL		EAX, 16
					MOV		AX, [ESI]
					CMP		EAX, EBX
					JE		CR24_c_TR
					MOV		[EDI], AX
					SHR		EAX, 16
					MOV		[EDI+2], AL
				CR24_c_TR:
					ADD		ESI, 3
					ADD		EDI, 3
					DEC		ECX
					JNZ		CR24_b_TR
					ADD		EDI, DestAddPitch
					ADD		ESI, SrcAddPitch
					DEC		EDX
					JNZ		CR24_a_TR
					JMP		END_a_TR

					

				CR16_a_TR:
					MOV		ECX, BltWidth
				CR16_b_TR:
					MOV		AX, [ESI]
					CMP		AX, BX
					JE		CR16_c_TR
					MOV		[EDI], AX
				CR16_c_TR:
					ADD		ESI, 2
					ADD		EDI, 2
					DEC		ECX
					JNZ		CR16_b_TR
					ADD		EDI, DestAddPitch
					ADD		ESI, SrcAddPitch
					DEC		EDX
					JNZ		CR16_a_TR
					JMP		END_a_TR


				CR8_a_TR:
					MOV		ECX, BltWidth
				CR8_b_TR:
					MOV		AL, [ESI]
					CMP		AL, BL
					JE		CR8_c_TR
					MOV		[EDI], AL
				CR8_c_TR:
					INC		ESI
					INC		EDI
					DEC		ECX
					JNZ		CR8_b_TR
					ADD		EDI, DestAddPitch
					ADD		ESI, SrcAddPitch
					DEC		EDX
					JNZ		CR8_a_TR
					JMP		END_a_TR

				END_a_TR:
					POPF
					POPA
				}
#endif
			}
			else
			{
				// ４バイト転送何回、その後あまる転送分が何ピクセル分あるか算出
				DwMoveSetNum = MoveLineByte / 4 ;
				NokoriMoveSetNum = ( MoveLineByte - DwMoveSetNum * 4 ) / DColor.PixelByte ;

				// 色ビット数をセット
				ColorBitDepth = DColor.ColorBitDepth ;

#ifdef DX_NON_INLINE_ASM
				int i ;
				switch( ColorBitDepth )
				{
				case 32 :
					{
						DWORD Dw8MoveSetNum ;

						Dw8MoveSetNum  = DwMoveSetNum  / 16 ;
						DwMoveSetNum  -= Dw8MoveSetNum * 16 ;
						do
						{
							if( Dw8MoveSetNum > 0 )
							{
								i = Dw8MoveSetNum ;
								do
								{
									( ( ULONGLONG * )DestP )[ 0 ] = ( ( ULONGLONG * )SrcP )[ 0 ] ;
									( ( ULONGLONG * )DestP )[ 1 ] = ( ( ULONGLONG * )SrcP )[ 1 ] ;
									( ( ULONGLONG * )DestP )[ 2 ] = ( ( ULONGLONG * )SrcP )[ 2 ] ;
									( ( ULONGLONG * )DestP )[ 3 ] = ( ( ULONGLONG * )SrcP )[ 3 ] ;
									( ( ULONGLONG * )DestP )[ 4 ] = ( ( ULONGLONG * )SrcP )[ 4 ] ;
									( ( ULONGLONG * )DestP )[ 5 ] = ( ( ULONGLONG * )SrcP )[ 5 ] ;
									( ( ULONGLONG * )DestP )[ 6 ] = ( ( ULONGLONG * )SrcP )[ 6 ] ;
									( ( ULONGLONG * )DestP )[ 7 ] = ( ( ULONGLONG * )SrcP )[ 7 ] ;
									DestP += 64 ;
									SrcP  += 64 ;
								}while( -- i != 0 ) ;
							}

							if( DwMoveSetNum > 0 )
							{
								i = DwMoveSetNum ;
								do
								{
									*( ( DWORD * )DestP ) = *( ( DWORD * )SrcP ) ;
									DestP += 4 ;
									SrcP += 4 ;
								}while( -- i != 0 ) ;
							}

							DestP += DestAddPitch ;
							SrcP += SrcAddPitch ;
						}while( -- BltHeight != 0 ) ;
					}
					break ;

				case 24 :
					do
					{
						if( DwMoveSetNum != 0 )
						{
							i = DwMoveSetNum ;
							do
							{
								*( ( DWORD * )DestP ) = *( ( DWORD * )SrcP ) ;
								DestP += 4 ;
								SrcP += 4 ;
							}while( -- i != 0 ) ;
						}

						if( NokoriMoveSetNum != 0 )
						{
							i = NokoriMoveSetNum ;
							do
							{
								*( ( WORD * )DestP ) = *( ( WORD * )SrcP ) ;
								DestP[ 2 ] = SrcP[ 2 ] ;
								DestP += 3 ;
								SrcP += 3 ;
							}while( -- i != 0 ) ;
						}
						DestP += DestAddPitch ;
						SrcP += SrcAddPitch ;
					}while( -- BltHeight != 0 ) ;
					break ;

				case 16 :
					do
					{
						if( DwMoveSetNum != 0 )
						{
							i = DwMoveSetNum ;
							do
							{
								*( ( DWORD * )DestP ) = *( ( DWORD * )SrcP ) ;
								DestP += 4 ;
								SrcP += 4 ;
							}while( -- i != 0 ) ;
						}

						if( NokoriMoveSetNum != 0 )
						{
							i = NokoriMoveSetNum ;
							do
							{
								*( ( WORD * )DestP ) = *( ( WORD * )SrcP ) ;
								DestP += 2 ;
								SrcP += 2 ;
							}while( -- i != 0 ) ;
						}
						DestP += DestAddPitch ;
						SrcP += SrcAddPitch ;
					}while( -- BltHeight != 0 ) ;
					break ;

				case 8 :
					do
					{
						if( DwMoveSetNum != 0 )
						{
							i = DwMoveSetNum ;
							do
							{
								*( ( DWORD * )DestP ) = *( ( DWORD * )SrcP ) ;
								DestP += 4 ;
								SrcP += 4 ;
							}while( -- i != 0 ) ;
						}

						if( NokoriMoveSetNum != 0 )
						{
							i = NokoriMoveSetNum ;
							do
							{
								*DestP = *SrcP ;
								DestP ++ ;
								SrcP ++ ;
							}while( -- i != 0 ) ;
						}
						DestP += DestAddPitch ;
						SrcP += SrcAddPitch ;
					}while( -- BltHeight != 0 ) ;
					break ;
				}
#else
				// 転送処理
				_asm{
					PUSHA
					PUSHF
					CLD
					MOV		EDI, DestP
					MOV		ESI, SrcP
					MOV		EAX, DestAddPitch
					MOV		EBX, SrcAddPitch
					MOV		EDX, BltHeight
					CMP		ColorBitDepth, 8
					JZ		CR8_a
					CMP		ColorBitDepth, 16
					JZ		CR16_a
					CMP		ColorBitDepth, 24
					JZ		CR24_a


				CR32_a:
					MOV		ECX, DwMoveSetNum
					REP		MOVSD
					ADD		EDI, EAX
					ADD		ESI, EBX
					DEC		EDX
					JNZ		CR32_a
					JMP		END_a
					
					
				CR24_a:
					MOV		ECX, DwMoveSetNum
					TEST	ECX, 0xffffffff
					JZ		R241_a
					REP		MOVSD
				R241_a:
					MOV		ECX, NokoriMoveSetNum
					TEST	ECX, 0xffffffff
					JZ		R242_a
				R243_a:
					MOVSW
					MOVSB
					LOOP 	R243_a
				R242_a:
					ADD		EDI, EAX
					ADD		ESI, EBX
					DEC		EDX
					JNZ		CR24_a
					JMP		END_a

					

				CR16_a:
					MOV		ECX, DwMoveSetNum
					TEST	ECX, 0xffffffff
					JZ		R161_a
					REP		MOVSD
				R161_a:
					MOV		ECX, NokoriMoveSetNum
					TEST	ECX, 0xffffffff
					JZ		R162_a
					REP		MOVSW
				R162_a:
					ADD		EDI, EAX
					ADD		ESI, EBX
					DEC		EDX
					JNZ		CR16_a
					JMP		END_a


				CR8_a:
					MOV		ECX, DwMoveSetNum
					TEST	ECX, 0xffffffff
					JZ		R81_a
					REP		MOVSD
				R81_a:
					MOV		ECX, NokoriMoveSetNum
					TEST	ECX, 0xffffffff
					JZ		R82_a
					REP		MOVSB
				R82_a:
					ADD		EDI, EAX
					ADD		ESI, EBX
					DEC		EDX
					JNZ		CR8_a
					JMP		END_a

				END_a:
					POPF
					POPA
				}
#endif
			}
		}
		else
		{
			// 反転転送

			// 転送元、先アドレスセット
			DestP = ( BYTE * )DestGraphData + DestPoint.y  * DestPitch + ( ( BltWidth - 1 ) + DestPoint.x ) * DColor.PixelByte ;
			SrcP  = ( BYTE * )SrcGraphData  + SrcRect->top * SrcPitch  + SrcRect->left                      * SColor.PixelByte ;

			// １ライン転送後、次の転送元アドレスまでのバイト数計算
			DestAddPitch = DestPitch + MoveLineByte ;
			SrcAddPitch = SrcPitch - MoveLineByte ;

			// 色ビット数をセット
			ColorBitDepth = DColor.ColorBitDepth ;

#ifdef DX_NON_INLINE_ASM
			int i ;
			switch( ColorBitDepth )
			{
			case 32 :
				do
				{
					i = BltWidth ;
					do
					{
						*( ( DWORD * )DestP ) = *( ( DWORD * )SrcP ) ;
						DestP -= 4 ;
						SrcP += 4 ;
					}while( -- i != 0 ) ;
					DestP += DestAddPitch ;
					SrcP += SrcAddPitch ;
				}while( -- BltHeight != 0 ) ;
				break ;

			case 24 :
				do
				{
					i = BltWidth ;
					do
					{
						*( ( WORD * )DestP ) = *( ( WORD * )SrcP ) ;
						DestP[ 2 ] = SrcP[ 2 ] ;
						DestP -= 3 ;
						SrcP += 3 ;
					}while( -- i != 0 ) ;
					DestP += DestAddPitch ;
					SrcP += SrcAddPitch ;
				}while( -- BltHeight != 0 ) ;
				break ;

			case 16 :
				do
				{
					i = BltWidth ;
					do
					{
						*( ( WORD * )DestP ) = *( ( WORD * )SrcP ) ;
						DestP -= 2 ;
						SrcP += 2 ;
					}while( -- i != 0 ) ;
					DestP += DestAddPitch ;
					SrcP += SrcAddPitch ;
				}while( -- BltHeight != 0 ) ;
				break ;

			case 8 :
				for( i = 0 ; i < BltHeight ; i ++ )
				{
					int j ;
					for( j = 0 ; j < BltWidth ; j ++ )
					{
						*DestP = *SrcP ;
						DestP -- ;
						SrcP ++ ;
					}
					DestP += DestAddPitch ;
					SrcP += SrcAddPitch ;
				}
				break ;
			}
#else
			// 転送処理
			_asm{
				PUSHA
				MOV		EDI, DestP
				MOV		ESI, SrcP
				MOV		EAX, DestAddPitch
				MOV		EBX, SrcAddPitch
				MOV		EDX, BltHeight
				CMP		ColorBitDepth, 8
				JZ		CR8R
				CMP		ColorBitDepth, 16
				JZ		CR16R
				CMP		ColorBitDepth, 24
				JZ		CR24R

			CR32R:
				MOV		ECX, BltWidth
			R321R:
				MOVSD
				SUB		EDI, 8
				LOOP	R321R
				ADD		EDI, EAX
				ADD		ESI, EBX
				DEC		EDX
				JNZ		CR32R
				JMP		ENDR
				
				
			CR24R:
				MOV		ECX, BltWidth
			R241R:
				MOVSW
				MOVSB
				SUB		EDI, 6
				LOOP 	R241R
				ADD		EDI, EAX
				ADD		ESI, EBX
				DEC		EDX
				JNZ		CR24R
				JMP		ENDR
				

			CR16R:
				MOV		ECX, BltWidth
			R161R:
				MOVSW
				SUB		EDI, 4
				LOOP	R161R
				ADD		EDI, EAX
				ADD		ESI, EBX
				DEC		EDX
				JNZ		CR16R
				JMP		ENDR


			CR8R:
				MOV		ECX, BltWidth
			R81R:
				MOVSB
				SUB		EDI, 2
				LOOP	R81R
				ADD		EDI, EAX
				ADD		ESI, EBX
				DEC		EDX
				JNZ		CR8R
				JMP		ENDR;

			ENDR:
				POPA
			}
#endif
		}
		// 終了
		return 0 ;
	}
	else
	// 透過色処理を行う場合
	if( TransColorNoMoveFlag == TRUE )
	{
		// Srcが XRGB32 Dest が ARGB32 の場合はちょっと高速な転送
		if( AlphaOnlyFlag == FALSE &&
			AlphaMaskFlag == FALSE &&
			AlphaFlag == FALSE &&
			RedIsAlphaFlag == FALSE &&
			TransColorAlphaTestFlag == FALSE &&
			ReverseFlag == FALSE &&
			SColor.ColorBitDepth == 32 &&
			DColor.ColorBitDepth == 32 &&
			SColor.RedMask   == 0x00ff0000 &&
			SColor.GreenMask == 0x0000ff00 &&
			SColor.BlueMask  == 0x000000ff &&
			SColor.AlphaMask == 0x00000000 &&
			DColor.RedMask   == 0x00ff0000 &&
			DColor.GreenMask == 0x0000ff00 &&
			DColor.BlueMask  == 0x000000ff &&
			DColor.AlphaMask == 0xff000000 )
		{
			DWORD TColor ;
			BYTE *DBuf, *SBuf ;
			DWORD DestPitch2, SrcPitch2 ;
			DWORD i, j ;

			TColor = TransColor & 0x00ffffff;

			// 各バッファへのアドレスをセット
			DBuf = ( BYTE * )DestGraphData + DestPoint.y  * DestPitch + DestPoint.x   * DColor.PixelByte ;
			SBuf = ( BYTE * )SrcGraphData  + SrcRect->top * SrcPitch  + SrcRect->left * SColor.PixelByte ;

			// １ライン転送する度に加算する値セット
			DestPitch2 = ( DWORD )( DestPitch - BltWidth * DColor.PixelByte ) ;
			SrcPitch2  = ( DWORD )( SrcPitch  - BltWidth * SColor.PixelByte ) ;

			// 転送
			if( BltHeight != 0 && BltWidth != 0 )
			{
				i = ( DWORD )BltHeight ;
				do
				{
					j = ( DWORD )BltWidth ;
					do
					{
						if( ( *( ( DWORD * )SBuf ) & 0x00ffffff ) != TColor )
						{
							*( ( DWORD * )DBuf ) = *( ( DWORD * )SBuf ) | 0xff000000 ;
						}
						DBuf += 4 ;
						SBuf += 4 ;
					}while( -- j ) ;

					DBuf += DestPitch2 ;
					SBuf += SrcPitch2 ;
				}while( -- i ) ;
			}

			// 終了
			return 0 ;
		}

		goto NORMALMOVE ;
	}
	else
	// アルファチャンネルを透過色テストアルファから取り、さらにそれ以外のフォーマットが同じの場合は
	// 少し高速な転送処理を行う
	if( ( DColor.AlphaWidth != 0 && TransColorAlphaTestFlag == TRUE ) && 
		AlphaOnlyFlag == FALSE && RedIsAlphaFlag == FALSE &&
		DColor.RedMask       == SColor.RedMask &&
		DColor.GreenMask     == SColor.GreenMask &&
		DColor.BlueMask      == SColor.BlueMask &&
		DColor.ColorBitDepth == SColor.ColorBitDepth )
	{
		DWORD TColor, NRGBMask ;
		BYTE *DBuf, *SBuf ;
		DWORD DestPitch2, SrcPitch2 ;
		int DestAdd ;

		// 透過色コードを得る
		TColor = NS_GetColor3( &DColor,
									( int )( ( TransColor & 0xff0000 ) >> 16 ),
									( int )( ( TransColor & 0x00ff00 ) >>  8 ),
									( int )( ( TransColor & 0x0000ff )       ) ) ;

		// ＲＧＢの反転マスクを作成する
		NRGBMask = ~( DColor.RedMask | DColor.GreenMask | DColor.BlueMask ) ;

		// 反転転送かどうかで処理を分岐
		if( ReverseFlag == TRUE )
		{
			DestPitch2 = ( DWORD )( DestPitch + BltWidth * DColor.PixelByte ) ;
			DestAdd    = -DColor.PixelByte ;
			DBuf       = ( BYTE * )DestGraphData + DestPoint.y * DestPitch + DestPoint.x * DColor.PixelByte + ( BltWidth - 1 ) * DColor.PixelByte ;
		}
		else
		{
			DestPitch2 = ( DWORD )( DestPitch - BltWidth * DColor.PixelByte ) ;
			DestAdd    = DColor.PixelByte ;
			DBuf       = ( BYTE * )DestGraphData + DestPoint.y * DestPitch + DestPoint.x * DColor.PixelByte ;
		}

		// 各バッファへのアドレスをセット
		SBuf = ( BYTE * )SrcGraphData + SrcRect->top * SrcPitch + SrcRect->left * SColor.PixelByte ;

		// １ライン転送する度に加算する値セット
		SrcPitch2 = ( DWORD )( SrcPitch - BltWidth * SColor.PixelByte ) ;
		
		// カラービット数によって処理を分岐
		switch( DColor.ColorBitDepth )
		{
		case 8 :		// ８ビットカラー
			// 転送処理
			{
				BYTE Color, Trans ;
				int i, j ;

				Trans = ( BYTE )TColor ;

				for( i = 0 ; i < BltHeight ; i ++, DBuf += DestAdd, SBuf += SrcPitch2 )
				{
					for( j = 0 ; j < BltWidth ; j ++, DBuf += DestAdd, SBuf += SColor.PixelByte )
					{
						Color = *( ( BYTE * )SBuf ) ;
						if( Trans == Color )	Color &= ~DColor.AlphaMask ;
						else					Color |= DColor.AlphaMask ;

						*( ( BYTE * )DBuf ) = Color ; 
					}
				}
			}
			break ;

		case 16 :		// １６ビットカラー
			// 転送処理
			{
				WORD Color, Trans ;
				int i, j ;

				Trans = ( WORD )TColor ;

				for( i = 0 ; i < BltHeight ; i ++, DBuf += DestPitch2, SBuf += SrcPitch2 )
				{
					for( j = 0 ; j < BltWidth ; j ++, DBuf += DestAdd, SBuf += SColor.PixelByte )
					{
						Color = *( ( WORD * )SBuf ) ;
						if( Trans == Color )	Color &= ~DColor.AlphaMask ;
						else					Color |= DColor.AlphaMask ;

						*( ( WORD * )DBuf ) = Color ; 
					}
				}
			}
			break ;
			
		case 24 :		// ２４ビットカラー
			// 転送処理
			{
				unsigned int Color, Trans ;
				int i, j ;

				Trans = TColor ;

				for( i = 0 ; i < BltHeight ; i ++, DBuf += DestPitch2, SBuf += SrcPitch2 )
				{
					for( j = 0 ; j < BltWidth ; j ++, DBuf += DestAdd, SBuf += SColor.PixelByte )
					{
						Color = ( DWORD )( ( *( ( WORD * )SBuf ) + ( *( ( BYTE * )( SBuf + 2 ) ) << 16 ) ) ) ;
						if( Trans == Color )	Color &= ~DColor.AlphaMask ;
						else					Color |= DColor.AlphaMask ;

						*( ( WORD * )DBuf ) 		= ( WORD )( Color & 0xffff ) ;
						*( ( BYTE * )( DBuf + 2 ) )	= ( BYTE )( ( Color >> 16 ) & 0xff ) ; 
					}
				}
			}
			break ;

		case 32 :		// ３２ビットカラー
			// 転送処理
			{
				unsigned int Color, Trans, NRgbMask ;
				int i, j ;

				Trans = TColor ;
				NRgbMask = NRGBMask ;

				for( i = 0 ; i < BltHeight ; i ++, DBuf += DestPitch2, SBuf += SrcPitch2 )
				{
					for( j = 0 ; j < BltWidth ; j ++, DBuf += DestAdd, SBuf += SColor.PixelByte )
					{
						Color = *( ( DWORD * )SBuf ) | NRgbMask ;
//						Color = *( ( DWORD * )SBuf ) ;
						if( Trans == Color )	Color &= ~DColor.AlphaMask ;
						else					Color |= DColor.AlphaMask ;

						*( ( DWORD * )DBuf ) = Color ; 
					}
				}
			}
			break ;
		}

		// 終了
		return 0 ;
	}

	// 良くある条件でのみ少し高速な転送処理を行う
	if( 
		ImageShavedMode == DX_SHAVEDMODE_NONE &&
		AlphaOnlyFlag   == FALSE &&
		RedIsAlphaFlag  == FALSE &&
		ReverseFlag     == FALSE &&
		AlphaMask       == NULL &&
		ReverseFlag     == FALSE )
	{
		if( TransColorAlphaTestFlag == FALSE &&
			SColor.ChannelNum      == 1 &&
			SColor.ChannelBitDepth == 8 )
		{
			BYTE *Dest ;
			BYTE *Src ;
			DWORD BlockNum ;
			DWORD OneNum ;
			DWORD w ;
			int DestAddPitch, SrcAddPitch ;

			DestAddPitch = DestPitch - BltWidth * DColor.PixelByte ;
			SrcAddPitch  = SrcPitch  - BltWidth * SColor.PixelByte ;

			Dest = ( BYTE * )DestGraphData + DestPoint.y  * DestPitch + DestPoint.x   * DColor.PixelByte ;
			Src  = ( BYTE * )SrcGraphData  + SrcRect->top * SrcPitch  + SrcRect->left * SColor.PixelByte ;

			if( DColor.ChannelNum      == 1 &&
				DColor.ChannelBitDepth == 8 )
			{
				BlockNum = BltWidth / 8 ;
				OneNum   = BltWidth - BlockNum * 8 ;

				do
				{
					if( BlockNum > 0 )
					{
						w = BlockNum ;
						do
						{
							( ( DWORD * )Dest )[ 0 ] = ( ( DWORD * )Src )[ 0 ] ;
							( ( DWORD * )Dest )[ 1 ] = ( ( DWORD * )Src )[ 1 ] ;
							Dest += 8 ;
							Src  += 8 ;
						}while( -- w ) ;
					}
					if( OneNum > 0 )
					{
						w = OneNum ;
						do
						{
							*Dest = *Src ;
							Dest ++ ;
							Src  ++ ;
						}while( -- w ) ;
					}
					Dest += DestAddPitch ;
					Src  += SrcAddPitch ;
				}while( -- BltHeight ) ;

				return 0 ;
			}
			else
			if( DColor.ChannelNum      == 2 &&
				DColor.ChannelBitDepth == 8 )
			{
				BlockNum = BltWidth / 8 ;
				OneNum   = BltWidth - BlockNum * 8 ;

				do
				{
					if( BlockNum > 0 )
					{
						w = BlockNum ;
						do
						{
							Dest[  0 ] = Src[ 0 ] ; Dest[  1 ] = Src[ 0 ] ;
							Dest[  2 ] = Src[ 1 ] ; Dest[  3 ] = Src[ 1 ] ;
							Dest[  4 ] = Src[ 2 ] ; Dest[  5 ] = Src[ 2 ] ;
							Dest[  6 ] = Src[ 3 ] ; Dest[  7 ] = Src[ 3 ] ;
							Dest[  8 ] = Src[ 4 ] ; Dest[  9 ] = Src[ 4 ] ;
							Dest[ 10 ] = Src[ 5 ] ; Dest[ 11 ] = Src[ 5 ] ;
							Dest[ 12 ] = Src[ 6 ] ; Dest[ 13 ] = Src[ 6 ] ;
							Dest[ 14 ] = Src[ 7 ] ; Dest[ 15 ] = Src[ 7 ] ;
							Dest += 16 ;
							Src  += 8 ;
						}while( -- w ) ;
					}
					if( OneNum > 0 )
					{
						w = OneNum ;
						do
						{
							Dest[ 0 ] = Src[ 0 ] ;
							Dest[ 1 ] = 0 ;
							Dest += 2 ;
							Src  ++ ;
						}while( -- w ) ;
					}
					Dest += DestAddPitch ;
					Src  += SrcAddPitch ;
				}while( -- BltHeight ) ;

				return 0 ;
			}
			else
			if( ( DColor.ColorBitDepth == 32 &&
				  // DColor.AlphaMask	== 0xff000000 &&
				  DColor.RedMask 	== 0x00ff0000 &&
				  DColor.GreenMask 	== 0x0000ff00 &&
				  DColor.BlueMask 	== 0x000000ff ) ||
				( DColor.ColorBitDepth == 32 &&
				  // DColor.AlphaMask	== 0xff000000 &&
				  DColor.RedMask 	== 0x000000ff &&
				  DColor.GreenMask 	== 0x0000ff00 &&
				  DColor.BlueMask 	== 0x00ff0000 ) )
			{
				BlockNum = BltWidth / 8 ;
				OneNum   = BltWidth - BlockNum * 8 ;

				do
				{
					if( BlockNum > 0 )
					{
						w = BlockNum ;
						do
						{
							Dest[  0 ] = Src[ 0 ] ; Dest[  1 ] = Src[ 0 ] ; Dest[  2 ] = Src[ 0 ] ; Dest[  3 ] = Src[ 0 ] ;
							Dest[  4 ] = Src[ 1 ] ; Dest[  5 ] = Src[ 1 ] ; Dest[  6 ] = Src[ 1 ] ; Dest[  7 ] = Src[ 1 ] ;
							Dest[  8 ] = Src[ 2 ] ; Dest[  9 ] = Src[ 2 ] ; Dest[ 10 ] = Src[ 2 ] ; Dest[ 11 ] = Src[ 2 ] ;
							Dest[ 12 ] = Src[ 3 ] ; Dest[ 13 ] = Src[ 3 ] ; Dest[ 14 ] = Src[ 3 ] ; Dest[ 15 ] = Src[ 3 ] ;
							Dest[ 16 ] = Src[ 4 ] ; Dest[ 17 ] = Src[ 4 ] ; Dest[ 18 ] = Src[ 4 ] ; Dest[ 19 ] = Src[ 4 ] ;
							Dest[ 20 ] = Src[ 5 ] ; Dest[ 21 ] = Src[ 5 ] ; Dest[ 22 ] = Src[ 5 ] ; Dest[ 23 ] = Src[ 5 ] ;
							Dest[ 24 ] = Src[ 6 ] ; Dest[ 25 ] = Src[ 6 ] ; Dest[ 26 ] = Src[ 6 ] ; Dest[ 27 ] = Src[ 6 ] ;
							Dest[ 28 ] = Src[ 7 ] ; Dest[ 29 ] = Src[ 7 ] ; Dest[ 30 ] = Src[ 7 ] ; Dest[ 31 ] = Src[ 7 ] ;
							Dest += 32 ;
							Src  += 8 ;
						}while( -- w ) ;
					}
					if( OneNum > 0 )
					{
						w = OneNum ;
						do
						{
							Dest[  0 ] = Src[ 0 ] ;
							Dest[  1 ] = Src[ 0 ] ;
							Dest[  2 ] = Src[ 0 ] ;
							Dest[  3 ] = Src[ 0 ] ;
							Dest += 4 ;
							Src  ++ ;
						}while( -- w ) ;
					}
					Dest += DestAddPitch ;
					Src  += SrcAddPitch ;
				}while( -- BltHeight ) ;

				return 0 ;
			}
		}
		else
		if( TransColorAlphaTestFlag == FALSE &&
			SColor.ChannelNum      == 2 &&
			SColor.ChannelBitDepth == 8 )
		{
			BYTE *Dest ;
			BYTE *Src ;
			DWORD BlockNum ;
			DWORD OneNum ;
			DWORD w ;
			int DestAddPitch, SrcAddPitch ;

			DestAddPitch = DestPitch - BltWidth * DColor.PixelByte ;
			SrcAddPitch  = SrcPitch  - BltWidth * SColor.PixelByte ;

			Dest = ( BYTE * )DestGraphData + DestPoint.y  * DestPitch + DestPoint.x   * DColor.PixelByte ;
			Src  = ( BYTE * )SrcGraphData  + SrcRect->top * SrcPitch  + SrcRect->left * SColor.PixelByte ;

			if( DColor.ChannelNum      == 2 &&
				DColor.ChannelBitDepth == 8 )
			{
				BlockNum = BltWidth / 8 ;
				OneNum   = BltWidth - BlockNum * 8 ;

				do
				{
					if( BlockNum > 0 )
					{
						w = BlockNum ;
						do
						{
							( ( DWORD * )Dest )[ 0 ] = ( ( DWORD * )Src )[ 0 ] ;
							( ( DWORD * )Dest )[ 1 ] = ( ( DWORD * )Src )[ 1 ] ;
							( ( DWORD * )Dest )[ 2 ] = ( ( DWORD * )Src )[ 2 ] ;
							( ( DWORD * )Dest )[ 3 ] = ( ( DWORD * )Src )[ 3 ] ;
							Dest += 16 ;
							Src  += 16 ;
						}while( -- w ) ;
					}

					if( OneNum > 0 )
					{
						w = OneNum ;
						do
						{
							*( ( WORD * )Dest ) = *( ( WORD * )Src ) ;
							Dest += 2 ;
							Src  += 2 ;
						}while( -- w ) ;
					}

					Dest += DestAddPitch ;
					Src  += SrcAddPitch ;
				}while( -- BltHeight ) ;

				return 0 ;
			}
			else
			if( ( DColor.ColorBitDepth == 32 &&
				  // DColor.AlphaMask	== 0xff000000 &&
				  DColor.RedMask 	== 0x00ff0000 &&
				  DColor.GreenMask 	== 0x0000ff00 &&
				  DColor.BlueMask 	== 0x000000ff ) ||
				( DColor.ColorBitDepth == 32 &&
				  // DColor.AlphaMask	== 0xff000000 &&
				  DColor.RedMask 	== 0x000000ff &&
				  DColor.GreenMask 	== 0x0000ff00 &&
				  DColor.BlueMask 	== 0x00ff0000 ) )
			{
				BlockNum = BltWidth / 8 ;
				OneNum   = BltWidth - BlockNum * 8 ;

				do
				{
					if( BlockNum > 0 )
					{
						w = BlockNum ;
						do
						{
							Dest[  0 ] = Src[  0 ] ; Dest[  1 ] = Src[  1 ] ; Dest[  2 ] = Src[  0 ] ; Dest[  3 ] = Src[  1 ] ;
							Dest[  4 ] = Src[  2 ] ; Dest[  5 ] = Src[  3 ] ; Dest[  6 ] = Src[  2 ] ; Dest[  7 ] = Src[  3 ] ;
							Dest[  8 ] = Src[  4 ] ; Dest[  9 ] = Src[  5 ] ; Dest[ 10 ] = Src[  4 ] ; Dest[ 11 ] = Src[  5 ] ;
							Dest[ 12 ] = Src[  6 ] ; Dest[ 13 ] = Src[  7 ] ; Dest[ 14 ] = Src[  6 ] ; Dest[ 15 ] = Src[  7 ] ;
							Dest[ 16 ] = Src[  8 ] ; Dest[ 17 ] = Src[  9 ] ; Dest[ 18 ] = Src[  8 ] ; Dest[ 19 ] = Src[  9 ] ;
							Dest[ 20 ] = Src[ 10 ] ; Dest[ 21 ] = Src[ 11 ] ; Dest[ 22 ] = Src[ 10 ] ; Dest[ 23 ] = Src[ 11 ] ;
							Dest[ 24 ] = Src[ 12 ] ; Dest[ 25 ] = Src[ 13 ] ; Dest[ 26 ] = Src[ 12 ] ; Dest[ 27 ] = Src[ 13 ] ;
							Dest[ 28 ] = Src[ 14 ] ; Dest[ 29 ] = Src[ 15 ] ; Dest[ 30 ] = Src[ 14 ] ; Dest[ 31 ] = Src[ 15 ] ;
							Dest += 32 ;
							Src  += 16 ;
						}while( -- w ) ;
					}
					if( OneNum > 0 )
					{
						w = OneNum ;
						do
						{
							Dest[  0 ] = Src[ 0 ] ;
							Dest[  1 ] = Src[ 1 ] ;
							Dest[  2 ] = Src[ 0 ] ;
							Dest[  3 ] = Src[ 1 ] ;
							Dest += 4 ;
							Src  += 2 ;
						}while( -- w ) ;
					}
					Dest += DestAddPitch ;
					Src  += SrcAddPitch ;
				}while( -- BltHeight ) ;

				return 0 ;
			}
		}
		else
		if( TransColorAlphaTestFlag == FALSE &&
			SColor.ColorBitDepth == 8 )
		{
			BYTE *Dest ;
			BYTE *Src ;
#ifdef DX_NON_INLINE_ASM
			DWORD *Palette ;
			BYTE Code[4] ;
			int w ;
#endif
			int DestAddPitch, SrcAddPitch ;

			DestAddPitch = DestPitch - BltWidth * DColor.PixelByte ;
			SrcAddPitch = SrcPitch - BltWidth * SColor.PixelByte ;

#ifdef DX_NON_INLINE_ASM
			Palette = (DWORD *)SColor.Palette ;
#endif
			Dest = ( BYTE * )DestGraphData + DestPoint.y  * DestPitch + DestPoint.x   * DColor.PixelByte ;
			Src  = ( BYTE * )SrcGraphData  + SrcRect->top * SrcPitch  + SrcRect->left * SColor.PixelByte ;

			if( DColor.ColorBitDepth == 16 &&
				DColor.AlphaMask	== 0x00000000 &&
				DColor.RedMask 		== 0x0000f800 &&
				DColor.GreenMask 	== 0x000007e0 &&
				DColor.BlueMask 	== 0x0000001f )
			{
#ifdef DX_NON_INLINE_ASM
				do
				{
					w = BltWidth ;
					do
					{
						*((DWORD *)Code) = Palette[*Src] ;
						*((WORD  *)Dest) = ( WORD )( 
											((Code[2] >> 3) << 11) |
											((Code[1] >> 2) <<  5) |
											((Code[0] >> 3)      ) ) ;
						Dest += 2 ;
						Src ++ ;
					}while( -- w ) ;
					Dest += DestAddPitch ;
					Src += SrcAddPitch ;
				}while( -- BltHeight ) ;
#else
				__asm
				{
					MOV EDI, Dest
					MOV ESI, Src
	LOCK6_LOOP1_8:
					MOV	ECX, BltWidth
	LOCK6_LOOP2_8:
					MOVZX EAX, BYTE PTR [ESI]
					MOV	EAX, DWORD PTR SColor[EAX*4+40]
					MOV	EDX, EAX
					SHR	EDX, 5
					AND	EDX, 0x07e0
					MOV	EBX, EAX
					SHR	EBX, 3
					AND	EBX, 0x001f
					OR	EDX, EBX
					SHR	EAX, 8
					AND	EAX, 0xf800
					OR	EDX, EAX

					MOV	[EDI], DX
					ADD	EDI, 2
					INC	ESI
					DEC	ECX
					JNE	LOCK6_LOOP2_8
					ADD	EDI, DestAddPitch
					ADD	ESI, SrcAddPitch
					DEC	BltHeight
					JNE	LOCK6_LOOP1_8
				} ;
#endif
				return 0 ;
			}
			else
			if( DColor.ColorBitDepth == 16 &&
				DColor.AlphaMask	== 0x00000000 &&
				DColor.RedMask 		== 0x00007c00 &&
				DColor.GreenMask 	== 0x000003e0 &&
				DColor.BlueMask 	== 0x0000001f )
			{
#ifdef DX_NON_INLINE_ASM
				do
				{
					w = BltWidth ;
					do
					{
						*((DWORD *)Code) = Palette[*Src] ;
						*((WORD  *)Dest) = ( WORD )( 
											((Code[2] >> 3) << 10) |
											((Code[1] >> 3) <<  5) |
											((Code[0] >> 3)      ) ) ;
						Dest += 2 ;
						Src ++ ;
					}while( -- w ) ;
					Dest += DestAddPitch ;
					Src += SrcAddPitch ;
				}while( -- BltHeight ) ;
#else
				__asm
				{
					MOV EDI, Dest
					MOV ESI, Src
	LOCK7_LOOP1_8:
					MOV	ECX, BltWidth
	LOCK7_LOOP2_8:
					MOVZX EAX, BYTE PTR [ESI]
					MOV	EAX, DWORD PTR SColor[EAX*4+40]
					MOV	EDX, EAX
					SHR	EDX, 6
					AND	EDX, 0x03e0
					MOV	EBX, EAX
					SHR	EBX, 3
					AND	EBX, 0x001f
					OR	EDX, EBX
					SHR	EAX, 9
					AND	EAX, 0x7c00
					OR	EDX, EAX
					OR  EDX, 0x8000

					MOV	[EDI], DX
					ADD	EDI, 2
					INC	ESI
					DEC	ECX
					JNE	LOCK7_LOOP2_8
					ADD	EDI, DestAddPitch
					ADD	ESI, SrcAddPitch
					DEC	BltHeight
					JNE	LOCK7_LOOP1_8
				} ;
#endif
				return 0 ;
			}
			else
			if( DColor.ColorBitDepth == 32 &&
				DColor.AlphaMask	== 0x00000000 &&
				DColor.RedMask 		== 0x00ff0000 &&
				DColor.GreenMask 	== 0x0000ff00 &&
				DColor.BlueMask 	== 0x000000ff )
			{
#ifdef DX_NON_INLINE_ASM
				do
				{
					w = BltWidth ;
					do
					{
						*((DWORD *)Dest) = Palette[*Src] | 0xff000000 ;
						Dest += 4 ;
						Src ++ ;
					}while( -- w ) ;
					Dest += DestAddPitch ;
					Src += SrcAddPitch ;
				}while( -- BltHeight ) ;
#else
				__asm
				{
					MOV EDI, Dest
					MOV ESI, Src
					MOV EDX, TransColor
					MOV EBX, BltHeight
	LOCK8_LOOP1_8:
					MOV ECX, BltWidth
	LOCK8_LOOP2_8:
					XOR EAX, EAX
					MOV AL, [ESI]
					MOV EAX, DWORD PTR SColor[EAX*4+40]
					OR EAX, 0xFF000000
					MOV [EDI], EAX
					INC ESI
					ADD EDI, 4
					DEC ECX
					JNZ LOCK8_LOOP2_8
					
					ADD EDI, DestAddPitch
					ADD ESI, SrcAddPitch
					DEC EBX
					JNZ LOCK8_LOOP1_8
				} ;
#endif
				return 0 ;
			}
		}
		else
		if( TransColorAlphaTestFlag == TRUE &&
			SColor.ColorBitDepth == 8 )
		{
			BYTE *Dest ;
			BYTE *Src ;
			int DestAddPitch, SrcAddPitch ;
			int w ;
			DWORD *Palette ;
			BYTE Code[4] ;

			Palette = (DWORD *)SColor.Palette ;
			DestAddPitch = DestPitch - BltWidth * DColor.PixelByte ;
			SrcAddPitch = SrcPitch - BltWidth * SColor.PixelByte ;

			Dest = ( BYTE * )DestGraphData + DestPoint.y  * DestPitch + DestPoint.x   * DColor.PixelByte ;
			Src  = ( BYTE * )SrcGraphData  + SrcRect->top * SrcPitch  + SrcRect->left * SColor.PixelByte ;

			if( DColor.ColorBitDepth == 16 &&
				DColor.AlphaMask	== 0x00008000 &&
				DColor.RedMask 		== 0x00007c00 &&
				DColor.GreenMask 	== 0x000003e0 &&
				DColor.BlueMask 	== 0x0000001f )
			{
#ifdef DX_NON_INLINE_ASM
				WORD Alpha ;
				do
				{
					w = BltWidth ;
					do
					{
						*((DWORD *)Code) = Palette[*Src] ;
						if( *((DWORD *)Code) == TransColor )	Alpha = 0 ;
						else									Alpha = 0x8000 ;
						*((WORD *)Dest) =	(WORD)(
											( ( *((DWORD *)Code) & 0xf80000 ) >> 9 ) |
											( ( *((DWORD *)Code) & 0xf800   ) >> 6 ) |
											( ( *((DWORD *)Code) & 0xf8     ) >> 3 ) | Alpha ) ;
						Dest += 2 ;
						Src ++ ;
					}while( -- w ) ;
					Dest += DestAddPitch ;
					Src += SrcAddPitch ;
				}while( -- BltHeight ) ;
#else
				__asm
				{
					MOV EDI, Dest
					MOV ESI, Src
	LOCK5_LOOP1_8:
					MOV	ECX, BltWidth
	LOCK5_LOOP2_8:
					MOVZX EAX, BYTE PTR [ESI]
					MOV	EAX, DWORD PTR SColor[EAX*4+40]
					CMP EAX, TransColor
					JE LOCK5_RABEL1_8
					OR EAX, 0x1000000
	LOCK5_RABEL1_8:
					MOV	EDX, EAX
					SHR	EDX, 6
					AND	EDX, 0x03e0
					MOV	EBX, EAX
					SHR	EBX, 3
					AND	EBX, 0x001f
					OR	EDX, EBX
					SHR	EAX, 9
					AND	EAX, 0xfc00
					OR	EDX, EAX

					MOV	[EDI], DX
					ADD	EDI, 2
					INC	ESI
					DEC	ECX
					JNE	LOCK5_LOOP2_8
					ADD	EDI, DestAddPitch
					ADD	ESI, SrcAddPitch
					DEC	BltHeight
					JNE	LOCK5_LOOP1_8
				} ;
#endif
				return 0 ;
			}
			else
			if( DColor.ColorBitDepth == 32 &&
				DColor.AlphaMask	== 0xff000000 &&
				DColor.RedMask 		== 0x00ff0000 &&
				DColor.GreenMask 	== 0x0000ff00 &&
				DColor.BlueMask 	== 0x000000ff )
			{
#ifdef DX_NON_INLINE_ASM
				do
				{
					w = BltWidth ;
					do
					{
						*((DWORD *)Code) = Palette[*Src] ;
						if( *((DWORD *)Code) == TransColor )	Code[3] = 0 ;
						else									Code[3] = 0xff ;
						*((DWORD *)Dest) = *((DWORD *)Code) ;
						Dest += 4 ;
						Src ++ ;
					}while( -- w ) ;
					Dest += DestAddPitch ;
					Src += SrcAddPitch ;
				}while( -- BltHeight ) ;
#else
				__asm
				{
					MOV EDI, Dest
					MOV ESI, Src
					MOV EDX, TransColor
					MOV EBX, BltHeight
	LOCK4_LOOP1_8:
					MOV ECX, BltWidth
	LOCK4_LOOP2_8:
					MOVZX EAX, BYTE PTR [ESI]
					MOV EAX, DWORD PTR SColor[EAX*4+40]
					CMP EAX, EDX
					JE LOCK4_RABEL1_8
					OR EAX, 0xFF000000
	LOCK4_RABEL1_8:
					MOV [EDI], EAX
					INC ESI
					ADD EDI, 4
					DEC ECX
					JNZ LOCK4_LOOP2_8
					
					ADD EDI, DestAddPitch
					ADD ESI, SrcAddPitch
					DEC EBX
					JNZ LOCK4_LOOP1_8
				} ;
#endif
				return 0 ;
			}
			else
			if( DColor.ColorBitDepth == 32 &&
				DColor.AlphaMask	== 0xff000000 &&
				DColor.RedMask 		== 0x000000ff &&
				DColor.GreenMask 	== 0x0000ff00 &&
				DColor.BlueMask 	== 0x00ff0000 )
			{
				DWORD TransColorRev ;

				TransColorRev = ( ( TransColor & 0x00ff0000 ) >> 16 ) |
								( ( TransColor & 0x000000ff ) << 16 ) |
								( ( TransColor & 0xff00ff00 )       ) ;

				do
				{
					w = BltWidth ;
					do
					{
						Code[ 0 ] = ( ( BYTE * )&Palette[ *Src ] )[ 2 ] ;
						Code[ 1 ] = ( ( BYTE * )&Palette[ *Src ] )[ 1 ] ;
						Code[ 2 ] = ( ( BYTE * )&Palette[ *Src ] )[ 0 ] ;
						Code[ 3 ] = ( ( BYTE * )&Palette[ *Src ] )[ 3 ] ;
						if( *((DWORD *)Code) == TransColorRev )	Code[3] = 0 ;
						else									Code[3] = 0xff ;
						*((DWORD *)Dest) = *((DWORD *)Code) ;
						Dest += 4 ;
						Src ++ ;
					}while( -- w ) ;
					Dest += DestAddPitch ;
					Src += SrcAddPitch ;
				}while( -- BltHeight ) ;
				return 0 ;
			}
		}
		else
		if( TransColorAlphaTestFlag == FALSE &&
			SColor.ColorBitDepth	== 32 &&
			SColor.AlphaMask		== 0xff000000 &&
			SColor.RedMask 			== 0x00ff0000 &&
			SColor.GreenMask 		== 0x0000ff00 &&
			SColor.BlueMask 		== 0x000000ff )
		{
			BYTE *Dest ;
			BYTE *Src ;
			int DestAddPitch, SrcAddPitch ;
			int w ;

			DestAddPitch = DestPitch - BltWidth * DColor.PixelByte ;
			SrcAddPitch = SrcPitch - BltWidth * SColor.PixelByte ;
			
			Dest = ( BYTE * )DestGraphData + DestPoint.y  * DestPitch + DestPoint.x   * DColor.PixelByte ;
			Src  = ( BYTE * )SrcGraphData  + SrcRect->top * SrcPitch  + SrcRect->left * SColor.PixelByte ;

			if( DColor.ColorBitDepth	== 32 &&
				DColor.AlphaMask		== 0xff000000 &&
				DColor.RedMask			== 0x000000ff &&
				DColor.GreenMask	 	== 0x0000ff00 &&
				DColor.BlueMask			== 0x00ff0000 )
			{
				do
				{
					w = BltWidth ;
					do
					{
						*( (DWORD *) Dest ) = ( DWORD )( 
												( Src[ 0 ] << 16 ) |
												( Src[ 1 ] <<  8 ) |
												( Src[ 2 ]       ) |
												( Src[ 3 ] << 24 ) ) ;
						Dest += 4 ;
						Src  += 4 ;
					}while( -- w ) ;
					Dest += DestAddPitch ;
					Src  += SrcAddPitch ;
				}while( -- BltHeight ) ;

				return 0 ;
			}
			else
			if( DColor.ColorBitDepth	== 32 &&
				DColor.AlphaMask		== 0x00000000 &&
				DColor.RedMask			== 0x00ff0000 &&
				DColor.GreenMask	 	== 0x0000ff00 &&
				DColor.BlueMask			== 0x000000ff )
			{
				do
				{
					w = BltWidth ;
					do
					{
						*( (DWORD *) Dest ) = *( ( DWORD *) Src ) ;
						Dest += 4 ;
						Src  += 4 ;
					}while( -- w ) ;
					Dest += DestAddPitch ;
					Src  += SrcAddPitch ;
				}while( -- BltHeight ) ;

				return 0 ;
			}
		}
		else
		if( TransColorAlphaTestFlag == FALSE &&
			SColor.ColorBitDepth == 24 &&
			SColor.RedMask 		== 0xff0000 &&
			SColor.GreenMask 	== 0x00ff00 &&
			SColor.BlueMask 	== 0x0000ff )
		{
			BYTE *Dest ;
			BYTE *Src ;
			int DestAddPitch, SrcAddPitch ;
			int w ;

			DestAddPitch = DestPitch - BltWidth * DColor.PixelByte ;
			SrcAddPitch = SrcPitch - BltWidth * SColor.PixelByte ;
			
			Dest = ( BYTE * )DestGraphData + DestPoint.y  * DestPitch + DestPoint.x   * DColor.PixelByte ;
			Src  = ( BYTE * )SrcGraphData  + SrcRect->top * SrcPitch  + SrcRect->left * SColor.PixelByte ;

			if( DColor.ColorBitDepth == 16 &&
				DColor.AlphaMask	== 0x00000000 &&
				DColor.RedMask 		== 0x0000f800 &&
				DColor.GreenMask 	== 0x000007e0 &&
				DColor.BlueMask 	== 0x0000001f )
			{
#ifdef DX_NON_INLINE_ASM
				do
				{
					w = BltWidth ;
					do
					{
						*((WORD *)Dest) = ( WORD )(
											((Src[2] >> 3) << 11) |
											((Src[1] >> 2) <<  5) |
											((Src[0] >> 3)      ) ) ;
						Dest += 2 ;
						Src += 3 ;
					}while( -- w ) ;
					Dest += DestAddPitch ;
					Src += SrcAddPitch ;
				}while( -- BltHeight ) ;
#else
				__asm
				{
					MOV EDI, Dest
					MOV ESI, Src
					MOV EBX, BltWidth
					MOV EDX, DestAddPitch
	LOCK3_LOOP1:
					MOV ECX, EBX // BltWidth
	LOCK3_LOOP2:
					XOR EAX, EAX
					MOV AL, [ESI+2]
					SHR EAX, 3
					SHL EAX, 11
					MOV EDX, EAX
					XOR EAX, EAX
					MOV AL, [ESI+1]
					SHR EAX, 2
					SHL EAX, 5
					OR EDX, EAX
					XOR EAX, EAX
					MOV AL, [ESI]
					SHR EAX, 3
					OR EDX, EAX
					MOV [EDI], DX
					ADD EDI, 2
					ADD ESI, 3
					DEC ECX
					JNZ LOCK3_LOOP2
					
					ADD EDI, DestAddPitch
					ADD ESI, SrcAddPitch
					DEC BltHeight
					JNZ LOCK3_LOOP1
				} ;
#endif
				return 0 ;
			}
			else
			if( DColor.ColorBitDepth == 16 &&
				DColor.AlphaMask	== 0x00000000 &&
				DColor.RedMask 		== 0x00007c00 &&
				DColor.GreenMask 	== 0x000003e0 &&
				DColor.BlueMask 	== 0x0000001f )
			{
#ifdef DX_NON_INLINE_ASM
				do
				{
					w = BltWidth ;
					do
					{
						*((WORD *)Dest) = ( WORD )(
											((Src[2] >> 3) << 10) |
											((Src[1] >> 3) <<  5) |
											((Src[0] >> 3)      ) ) ;
						Dest += 2 ;
						Src += 3 ;
					}while( -- w ) ;
					Dest += DestAddPitch ;
					Src += SrcAddPitch ;
				}while( -- BltHeight ) ;
#else
				__asm
				{
					MOV EDI, Dest
					MOV ESI, Src
					MOV EBX, BltWidth
					MOV EDX, DestAddPitch
	LOCK2_LOOP1:
					MOV ECX, EBX // BltWidth
	LOCK2_LOOP2:
					XOR EAX, EAX
					MOV AL, [ESI+2]
					SHR EAX, 3
					SHL EAX, 10
					MOV EDX, EAX
					XOR EAX, EAX
					MOV AL, [ESI+1]
					SHR EAX, 3
					SHL EAX, 5
					OR EDX, EAX
					XOR EAX, EAX
					MOV AL, [ESI]
					SHR EAX, 3
					OR EAX, 0x8000
					OR EDX, EAX
					MOV [EDI], DX
					ADD EDI, 2
					ADD ESI, 3
					DEC ECX
					JNZ LOCK2_LOOP2
					
					ADD EDI, DestAddPitch
					ADD ESI, SrcAddPitch
					DEC BltHeight
					JNZ LOCK2_LOOP1
				} ;
#endif
				return 0 ;
			}
			else
			if( DColor.ColorBitDepth == 32 &&
//				DColor.AlphaMask	== 0x00000000 &&
				DColor.RedMask 		== 0x00ff0000 &&
				DColor.GreenMask 	== 0x0000ff00 &&
				DColor.BlueMask 	== 0x000000ff )
			{
#ifdef DX_NON_INLINE_ASM
				do
				{
					w = BltWidth ;
					do
					{
						*((DWORD *)Dest) = *((WORD *)Src) | ( Src[2] << 16 ) | 0xff000000 ;
						Dest += 4 ;
						Src += 3 ;
					}while( -- w ) ;
					Dest += DestAddPitch ;
					Src += SrcAddPitch ;
				}while( -- BltHeight ) ;
#else
				__asm
				{
					PUSH EBP
					MOV EDI, Dest
					MOV ESI, Src
					MOV EAX, SrcAddPitch
					PUSH EAX
					MOV EBX, BltWidth
					MOV EAX, BltHeight
					MOV EDX, DestAddPitch
					POP EBP // SrcAddPitch
	LOCK1_LOOP1:
					MOV ECX, EBX // BltWidth
	LOCK1_LOOP2:
					MOVSW
					MOVSB
					MOV byte ptr [EDI], 0xFF
					INC EDI
					DEC ECX
					JNZ LOCK1_LOOP2
					
					ADD EDI, EDX // DestAddPitch
					ADD ESI, EBP // SrcAddPitch
					DEC EAX // BltHeight
					JNZ LOCK1_LOOP1
					POP EBP
				} ;
#endif
				return 0 ;
			}
			else
			if( DColor.ColorBitDepth == 32 &&
//				DColor.AlphaMask	== 0x00000000 &&
				DColor.RedMask 		== 0x000000ff &&
				DColor.GreenMask 	== 0x0000ff00 &&
				DColor.BlueMask 	== 0x00ff0000 )
			{
				do
				{
					w = BltWidth ;
					do
					{
						*((DWORD *)Dest) = ( Src[ 0 ] << 16 ) | ( Src[ 1 ] << 8 ) | Src[ 2 ] | 0xff000000 ;
						Dest += 4 ;
						Src += 3 ;
					}while( -- w ) ;
					Dest += DestAddPitch ;
					Src += SrcAddPitch ;
				}while( -- BltHeight ) ;
				return 0 ;
			}
		}
		else
		if( TransColorAlphaTestFlag == TRUE &&
			SColor.ColorBitDepth == 24 &&
			SColor.RedMask 		== 0xff0000 &&
			SColor.GreenMask 	== 0x00ff00 &&
			SColor.BlueMask 	== 0x0000ff )
		{
			BYTE *Dest ;
			BYTE *Src ;
			int DestAddPitch, SrcAddPitch ;
			int w, Alpha ;
			union
			{
				unsigned int dword ;
				unsigned char byte[4] ;
			} SrcColor ;

			DestAddPitch = DestPitch - BltWidth * DColor.PixelByte ;
			SrcAddPitch = SrcPitch - BltWidth * SColor.PixelByte ;
			
			Dest = ( BYTE * )DestGraphData + DestPoint.y  * DestPitch + DestPoint.x   * DColor.PixelByte ;
			Src  = ( BYTE * )SrcGraphData  + SrcRect->top * SrcPitch  + SrcRect->left * SColor.PixelByte ;

			if( DColor.ColorBitDepth == 16 &&
				DColor.AlphaMask	== 0x00008000 &&
				DColor.RedMask 		== 0x00007c00 &&
				DColor.GreenMask 	== 0x000003e0 &&
				DColor.BlueMask 	== 0x0000001f )
			{
#ifdef DX_NON_INLINE_ASM
				do
				{
					w = BltWidth ;
					do
					{
						SrcColor.dword = ( unsigned int )( *((WORD *)Src) + (Src[2] << 16) ) ;
						Alpha          = 0x8000 ;
						if( SrcColor.dword == TransColor ) Alpha = 0 ;
						*((WORD *)Dest) = ( WORD )(
											((SrcColor.byte[2] >> 3) << 10) |
											((SrcColor.byte[1] >> 3) <<  5) |
											((SrcColor.byte[0] >> 3)      ) |
											Alpha ) ;
						Dest += 2 ;
						Src += 3 ;
					}while( -- w ) ;
					Dest += DestAddPitch ;
					Src += SrcAddPitch ;
				}while( -- BltHeight ) ;
#else
//				DWORD TransCode ;
//				TransCode = (   ((BYTE *)&TransColor)[0] >> 3) |
//							( ( ((BYTE *)&TransColor)[1] >> 3) << 5) |
//							( ( ((BYTE *)&TransColor)[2] >> 3) << 10) ;
				__asm
				{
					MOV EDI, Dest
					MOV ESI, Src
					MOV EBX, TransColor
	LOCK5_LOOP1:
					MOV ECX, BltWidth
	LOCK5_LOOP2:
					MOV EAX, [ESI]
					AND EAX, 0x00ffffff
					CMP EAX, EBX
					JE LOCK5_RABEL1
					XOR EDX, EDX
					AND EAX, 0xF8F8F8
					SHR EAX, 3
					MOV DL, AL
					SHR	AX, 8
					SHL AX, 5
					OR DX, AX
					SHR EAX, 16
					SHL EAX, 10
					OR EDX, EAX
					OR EDX, 0x8000
					MOV [EDI], DX
					ADD EDI, 2
					ADD ESI, 3
					DEC ECX
					JNZ LOCK5_LOOP2
					JMP LOCK5_RABEL2
	LOCK5_RABEL1:
					XOR EDX, EDX
					AND EAX, 0xF8F8F8
					SHR EAX, 3
					MOV DL, AL
					SHR	AX, 8
					SHL AX, 5
					OR DX, AX
					SHR EAX, 16
					SHL EAX, 10
					OR EDX, EAX
					MOV [EDI], DX
					ADD EDI, 2
					ADD ESI, 3
					DEC ECX
					JNZ LOCK5_LOOP2
	LOCK5_RABEL2:
					
					ADD EDI, DestAddPitch
					ADD ESI, SrcAddPitch
					DEC BltHeight
					JNZ LOCK5_LOOP1
				} ;
#endif
				return 0 ;
			}
			else
			if( DColor.ColorBitDepth == 32 &&
				DColor.AlphaMask	== 0xff000000 &&
				DColor.RedMask 		== 0x00ff0000 &&
				DColor.GreenMask 	== 0x0000ff00 &&
				DColor.BlueMask 	== 0x000000ff )
			{
#ifdef DX_NON_INLINE_ASM
				do
				{
					w = BltWidth ;
					do
					{
						SrcColor.dword = ( unsigned int )( *((WORD *)Src) + (Src[2] << 16) ) ;
						Alpha = 0xff000000 ;
						if( SrcColor.dword == TransColor ) Alpha = 0 ;
						*((DWORD *)Dest) = SrcColor.dword | Alpha ;
						Dest += 4 ;
						Src += 3 ;
					}while( -- w ) ;
					Dest += DestAddPitch ;
					Src += SrcAddPitch ;
				}while( -- BltHeight ) ;
#else
				__asm
				{
					MOV EDI, Dest
					MOV ESI, Src
					MOV EDX, TransColor
					MOV EBX, BltHeight
	LOCK4_LOOP1:
					MOV ECX, BltWidth
	LOCK4_LOOP2:
					XOR EAX, EAX
					MOV AL, [ESI+2]
					SHL EAX, 16
					MOV AX, [ESI]
					CMP EAX, EDX
					JE LOCK4_RABEL1
					OR EAX, 0xFF000000
	LOCK4_RABEL1:
					MOV [EDI], EAX
					ADD ESI, 3
					ADD EDI, 4
					DEC ECX
					JNZ LOCK4_LOOP2
					
					ADD EDI, DestAddPitch
					ADD ESI, SrcAddPitch
					DEC EBX
					JNZ LOCK4_LOOP1
				} ;
#endif
				return 0 ;
			}
			else
			if( DColor.ColorBitDepth == 32 &&
				DColor.AlphaMask	== 0xff000000 &&
				DColor.RedMask 		== 0x000000ff &&
				DColor.GreenMask 	== 0x0000ff00 &&
				DColor.BlueMask 	== 0x00ff0000 )
			{
				do
				{
					w = BltWidth ;
					do
					{
						SrcColor.dword = ( unsigned int )( *((WORD *)Src) + (Src[2] << 16) ) ;
						Alpha = 0xff000000 ;
						if( SrcColor.dword == TransColor ) Alpha = 0 ;
						*((DWORD *)Dest) = ( DWORD )( ( Src[ 0 ] << 16 ) | ( Src[ 1 ] << 8 ) | Src[ 2 ] | Alpha ) ;
						Dest += 4 ;
						Src  += 3 ;
					}while( -- w ) ;
					Dest += DestAddPitch ;
					Src  += SrcAddPitch ;
				}while( -- BltHeight ) ;

				return 0 ;
			}
		}
		else
		if( TransColorAlphaTestFlag == FALSE &&
			AlphaFlag               == FALSE &&
			SColor.ColorBitDepth    == 32 &&
			SColor.AlphaMask	    == 0x00000000 &&
			SColor.RedMask 	   	    == 0x00ff0000 &&
			SColor.GreenMask 	    == 0x0000ff00 &&
			SColor.BlueMask 	    == 0x000000ff )
		{
			BYTE *Dest ;
			BYTE *Src ;
			int DestAddPitch, SrcAddPitch ;

			DestAddPitch = DestPitch - BltWidth * DColor.PixelByte ;
			SrcAddPitch  = SrcPitch  - BltWidth * SColor.PixelByte ;

			Dest = ( BYTE * )DestGraphData + DestPoint.y  * DestPitch + DestPoint.x   * DColor.PixelByte ;
			Src  = ( BYTE * )SrcGraphData  + SrcRect->top * SrcPitch  + SrcRect->left * SColor.PixelByte ;

			if( DColor.ColorBitDepth == 16 &&
				DColor.AlphaMask	 == 0x00000000 &&
				DColor.RedMask 		 == 0x0000f800 &&
				DColor.GreenMask 	 == 0x000007e0 &&
				DColor.BlueMask 	 == 0x0000001f )
			{
#ifdef DX_NON_INLINE_ASM
				int i ;
				DWORD SrcC ;
				do
				{
					i = BltWidth ;
					do
					{
						SrcC = *( ( DWORD * )Src ) ;
						*( ( WORD * )Dest ) = ( WORD )
							( ( ( SrcC & 0x00f80000 ) >> 8 ) |
							  ( ( SrcC & 0x0000fc00 ) >> 5 ) |
							  ( ( SrcC & 0x000000f8 ) >> 3 ) ) ;
						Src += 4 ;
						Dest += 2 ;
					}while( -- i != 0 ) ;

					Dest += DestAddPitch ;
					Src += SrcAddPitch ;

				}while( -- BltHeight != 0 ) ;
#else
				__asm
				{
					MOV EDI, Dest
					MOV ESI, Src
	LOCK6_LOOP1:
					MOV ECX, BltWidth
	LOCK6_LOOP2:
					MOV EAX, [ESI]
					MOV EBX, EAX
					MOV EDX, EAX
					AND EBX, 0x00f80000
					AND EDX, 0x0000fc00
					AND EAX, 0x000000f8
					SHR EBX, 8
					SHR EDX, 5
					SHR EAX, 3
					OR EDX, EBX
					OR EAX, EDX
					MOV [EDI], AX
					ADD EDI, 2
					ADD ESI, 4
					DEC ECX
					JNZ LOCK6_LOOP2
					
					ADD EDI, DestAddPitch
					ADD ESI, SrcAddPitch
					DEC BltHeight
					JNZ LOCK6_LOOP1
				} ;
#endif
				return 0 ;
			}
		}
		else
		if( TransColorAlphaTestFlag == TRUE &&
			AlphaFlag               == FALSE &&
			SColor.ColorBitDepth    == 32 &&
			SColor.AlphaMask	    == 0x00000000 &&
			SColor.RedMask 	   	    == 0x00ff0000 &&
			SColor.GreenMask 	    == 0x0000ff00 &&
			SColor.BlueMask 	    == 0x000000ff )
		{
			BYTE *Dest ;
			BYTE *Src ;
			int DestAddPitch, SrcAddPitch ;
			DWORD TransCode ;

			DestAddPitch = DestPitch - BltWidth * DColor.PixelByte ;
			SrcAddPitch  = SrcPitch  - BltWidth * SColor.PixelByte ;

			Dest = ( BYTE * )DestGraphData + DestPoint.y  * DestPitch + DestPoint.x   * DColor.PixelByte ;
			Src  = ( BYTE * )SrcGraphData  + SrcRect->top * SrcPitch  + SrcRect->left * SColor.PixelByte ;

			if( DColor.ColorBitDepth == 32 &&
				DColor.AlphaMask	 == 0xff000000 &&
				DColor.RedMask 		 == 0x000000ff &&
				DColor.GreenMask 	 == 0x0000ff00 &&
				DColor.BlueMask 	 == 0x00ff0000 )
			{
				int i ;
				DWORD SrcC ;

				TransCode = ( DWORD )( 
							( ((BYTE *)&TransColor)[0] << 16) |
							( ((BYTE *)&TransColor)[1] <<  8) |
							( ((BYTE *)&TransColor)[2]      ) ) ;

				do
				{
					i = BltWidth ;
					do
					{
						SrcC = ( ( DWORD )Src[ 0 ] << 16 ) | ( ( DWORD )Src[ 1 ] << 8 ) | ( DWORD )Src[ 2 ];
						if( TransCode != SrcC )
						{
							*( ( DWORD * )Dest ) = ( DWORD )( SrcC | 0xff000000 ) ;
						}
						else
						{
							*( ( DWORD * )Dest ) = ( DWORD )SrcC  ;
						}
						Src += 4 ;
						Dest += 4 ;
					}while( -- i != 0 ) ;

					Dest += DestAddPitch ;
					Src += SrcAddPitch ;

				}while( -- BltHeight != 0 ) ;

				return 0 ;
			}
		}
		else
		if( TransColorAlphaTestFlag == FALSE &&
			SColor.ColorBitDepth    == 16 &&
			SColor.AlphaMask	    == 0x00008000 &&
			SColor.RedMask 			== 0x00007c00 &&
			SColor.GreenMask 		== 0x000003e0 &&
			SColor.BlueMask 		== 0x0000001f )
		{
			BYTE *Dest ;
			BYTE *Src ;
			int DestAddPitch, SrcAddPitch ;

			DestAddPitch = DestPitch - BltWidth * DColor.PixelByte ;
			SrcAddPitch  = SrcPitch  - BltWidth * SColor.PixelByte ;

			Dest = ( BYTE * )DestGraphData + DestPoint.y  * DestPitch + DestPoint.x   * DColor.PixelByte ;
			Src  = ( BYTE * )SrcGraphData  + SrcRect->top * SrcPitch  + SrcRect->left * SColor.PixelByte ;

			if( DColor.ColorBitDepth == 32 &&
				DColor.AlphaMask	 == 0xff000000 &&
				DColor.RedMask 		 == 0x000000ff &&
				DColor.GreenMask 	 == 0x0000ff00 &&
				DColor.BlueMask 	 == 0x00ff0000 )
			{
				int i ;
				DWORD SrcC ;
				do
				{
					i = BltWidth ;
					do
					{
						SrcC  = *( ( WORD * )Src ) ;
						if( SrcC & 0x8000 )
						{
							*( ( DWORD * )Dest ) =
								( ( SrcC & 0x7c00 ) >>  7 ) |
								( ( SrcC & 0x03e0 ) <<  6 ) |
								( ( SrcC & 0x001f ) << 19 ) | 0xff000000 ;
						}
						else
						{
							*( ( DWORD * )Dest ) =
								( ( SrcC & 0x7c00 ) >>  7 ) |
								( ( SrcC & 0x03e0 ) <<  6 ) |
								( ( SrcC & 0x001f ) << 19 ) ;
						}

						Src  += 2 ;
						Dest += 4 ;
					}while( -- i != 0 ) ;

					Dest += DestAddPitch ;
					Src += SrcAddPitch ;

				}while( -- BltHeight != 0 ) ;

				return 0 ;
			}
		}
		else
		if( TransColorAlphaTestFlag == TRUE &&
			SColor.ColorBitDepth    == 16 &&
			SColor.AlphaMask	    == 0x00000000 &&
			SColor.RedMask 	   	    == 0x0000f800 &&
			SColor.GreenMask 	    == 0x000007e0 &&
			SColor.BlueMask 	    == 0x0000001f )
		{
			BYTE *Dest ;
			BYTE *Src ;
			int DestAddPitch, SrcAddPitch ;
			DWORD TransCode ;

			DestAddPitch = DestPitch - BltWidth * DColor.PixelByte ;
			SrcAddPitch  = SrcPitch  - BltWidth * SColor.PixelByte ;

			Dest = ( BYTE * )DestGraphData + DestPoint.y  * DestPitch + DestPoint.x   * DColor.PixelByte ;
			Src  = ( BYTE * )SrcGraphData  + SrcRect->top * SrcPitch  + SrcRect->left * SColor.PixelByte ;

			if( DColor.ColorBitDepth == 16 &&
				DColor.AlphaMask	 == 0x00000001 &&
				DColor.RedMask 		 == 0x0000f800 &&
				DColor.GreenMask 	 == 0x000007c0 &&
				DColor.BlueMask 	 == 0x0000003e )
			{
				TransCode = ( DWORD )( 
							( ( ((BYTE *)&TransColor)[0] >> 3) <<  1) |
							( ( ((BYTE *)&TransColor)[1] >> 3) <<  6) |
							( ( ((BYTE *)&TransColor)[2] >> 3) << 11) ) ;

				int i ;
				DWORD SrcC ;
				do
				{
					i = BltWidth ;
					if( i != 0 )
					{
						do
						{
							SrcC = ( DWORD )( ( ( *( ( WORD * )Src ) & 0x1f ) << 1 ) | ( *( ( WORD * )Src ) & 0xffc0 ) ) ;
							if( TransCode != SrcC )
							{
								*( ( WORD * )Dest ) = ( WORD )( SrcC | 0x0001 ) ;
							}
							else
							{
								*( ( WORD * )Dest ) = ( WORD )SrcC  ;
							}
							Src += 2 ;
							Dest += 2 ;
						}while( -- i != 0 ) ;
					}

					Dest += DestAddPitch ;
					Src += SrcAddPitch ;

				}while( -- BltHeight != 0 ) ;
				return 0 ;
			}
			else
			if( DColor.ColorBitDepth == 16 &&
				DColor.AlphaMask	 == 0x00008000 &&
				DColor.RedMask 		 == 0x00007c00 &&
				DColor.GreenMask 	 == 0x000003e0 &&
				DColor.BlueMask 	 == 0x0000001f )
			{
				TransCode = ( DWORD )( 
							(   ((BYTE *)&TransColor)[0] >> 3)        |
							( ( ((BYTE *)&TransColor)[1] >> 3) <<  5) |
							( ( ((BYTE *)&TransColor)[2] >> 3) << 10) ) ;

#ifdef DX_NON_INLINE_ASM
				int i ;
				DWORD SrcC ;
				do
				{
					i = BltWidth ;
					do
					{
						SrcC = ( DWORD )( ( *( ( WORD * )Src ) & 0x1f ) | ( ( *( ( WORD * )Src ) & 0xffc0 ) >> 1 ) ) ;
						if( SrcC != TransCode )
						{
							*( ( WORD * )Dest ) = ( WORD )( SrcC | 0x8000 ) ;
						}
						else
						{
							*( ( WORD * )Dest ) = ( WORD )( SrcC ) ;
						}
						Src += 2 ;
						Dest += 2 ;
					}while( -- i != 0 ) ;

					Dest += DestAddPitch ;
					Src += SrcAddPitch ;

				}while( -- BltHeight != 0 ) ;
#else
				__asm
				{
					MOV EDI, Dest
					MOV ESI, Src
					MOV EBX, TransCode
	LOCK11_LOOP1:
					MOV ECX, BltWidth
	LOCK11_LOOP2:
					XOR EAX, EAX
					MOV AX, [ESI]
					MOV EDX, EAX
					AND EDX, 0x1f
					AND EAX, 0xffc0
					SHR EAX, 1
					OR  EAX, EDX
					CMP EAX, EBX
					JE LOCK11_RABEL1
					OR EAX, 0x8000
	LOCK11_RABEL1:
					MOV [EDI], AX
					ADD EDI, 2
					ADD ESI, 2
					DEC ECX
					JNZ LOCK11_LOOP2
					
					ADD EDI, DestAddPitch
					ADD ESI, SrcAddPitch
					DEC BltHeight
					JNZ LOCK11_LOOP1
				} ;
#endif
				return 0 ;
			}
			else
			if( DColor.ColorBitDepth == 32 &&
				DColor.AlphaMask	 == 0xff000000 &&
				DColor.RedMask 		 == 0x00ff0000 &&
				DColor.GreenMask 	 == 0x0000ff00 &&
				DColor.BlueMask 	 == 0x000000ff )
			{
				TransCode = ( DWORD )( 
							(   ((BYTE *)&TransColor)[0] >> 3)        |
							( ( ((BYTE *)&TransColor)[1] >> 2) <<  5) |
							( ( ((BYTE *)&TransColor)[2] >> 3) << 11) ) ;
#ifdef DX_NON_INLINE_ASM
				int i ;
				DWORD SrcC ;
				do
				{
					i = BltWidth ;
					do
					{
						SrcC = *( ( WORD * )Src ) ;
						if( SrcC != TransCode )
						{
							*( ( DWORD * )Dest ) = 
								( ( SrcC & 0x001f ) << 3 ) |
								( ( SrcC & 0x07e0 ) << 5 ) |
								( ( SrcC & 0xf800 ) << 8 ) |
								0xff000000 ;
						}
						else
						{
							*( ( DWORD * )Dest ) = 
								( ( SrcC & 0x001f ) << 3 ) |
								( ( SrcC & 0x07e0 ) << 5 ) |
								( ( SrcC & 0xf800 ) << 8 ) ;
						}
						Src += 2 ;
						Dest += 4 ;
					}while( -- i != 0 ) ;

					Dest += DestAddPitch ;
					Src += SrcAddPitch ;

				}while( -- BltHeight != 0 ) ;
#else
				__asm
				{
					MOV EDI, Dest
					MOV ESI, Src
	LOCK15_LOOP1:
					MOV ECX, BltWidth
	LOCK15_LOOP2:
					XOR EAX, EAX
					MOV AX, [ESI]
					MOV EDX, EAX
					MOV EBX, EAX
					AND EDX, 0x001f
					AND EAX, 0x07e0
					AND EBX, 0xf800
					SHL EDX, 3
					SHL EAX, 5
					SHL EBX, 8
					OR  EAX, EDX
					OR  EAX, EBX
					XOR EDX, EDX
					MOV DX, [ESI]
					CMP EDX, TransCode
					JE  LOCK15_RABEL1
					OR  EAX, 0xff000000
	LOCK15_RABEL1:
					MOV [EDI], EAX
					ADD EDI, 4
					ADD ESI, 2
					DEC ECX
					JNZ LOCK15_LOOP2
					
					ADD EDI, DestAddPitch
					ADD ESI, SrcAddPitch
					DEC BltHeight
					JNZ LOCK15_LOOP1
				} ;
#endif
				return 0 ;
			}
			else
			if( DColor.ColorBitDepth == 32 &&
				DColor.AlphaMask	 == 0xff000000 &&
				DColor.RedMask 		 == 0x000000ff &&
				DColor.GreenMask 	 == 0x0000ff00 &&
				DColor.BlueMask 	 == 0x00ff0000 )
			{
				TransCode = ( DWORD )( 
							(   ((BYTE *)&TransColor)[0] >> 3)        |
							( ( ((BYTE *)&TransColor)[1] >> 2) <<  5) |
							( ( ((BYTE *)&TransColor)[2] >> 3) << 11) ) ;

				int i ;
				DWORD SrcC ;
				do
				{
					i = BltWidth ;
					do
					{
						SrcC = *( ( WORD * )Src ) ;
						if( SrcC != TransCode )
						{
							*( ( DWORD * )Dest ) = 
								( ( SrcC & 0x001f ) << 19 ) |
								( ( SrcC & 0x07e0 ) << 5 ) |
								( ( SrcC & 0xf800 ) >> 8 ) |
								0xff000000 ;
						}
						else
						{
							*( ( DWORD * )Dest ) = 
								( ( SrcC & 0x001f ) << 19 ) |
								( ( SrcC & 0x07e0 ) << 5 ) |
								( ( SrcC & 0xf800 ) >> 8 ) ;
						}
						Src += 2 ;
						Dest += 4 ;
					}while( -- i != 0 ) ;

					Dest += DestAddPitch ;
					Src += SrcAddPitch ;

				}while( -- BltHeight != 0 ) ;
				return 0 ;
			}
		}
		else
		if( TransColorAlphaTestFlag == FALSE &&
			AlphaFlag               == FALSE &&
			SColor.ColorBitDepth    == 16 &&
			SColor.AlphaMask	    == 0x00000000 &&
			SColor.RedMask 	   	    == 0x0000f800 &&
			SColor.GreenMask 	    == 0x000007e0 &&
			SColor.BlueMask 	    == 0x0000001f )
		{
			BYTE *Dest ;
			BYTE *Src ;
			int DestAddPitch, SrcAddPitch ;

			DestAddPitch = DestPitch - BltWidth * DColor.PixelByte ;
			SrcAddPitch  = SrcPitch  - BltWidth * SColor.PixelByte ;

			Dest = ( BYTE * )DestGraphData + DestPoint.y  * DestPitch + DestPoint.x   * DColor.PixelByte ;
			Src  = ( BYTE * )SrcGraphData  + SrcRect->top * SrcPitch  + SrcRect->left * SColor.PixelByte ;

			if( DColor.ColorBitDepth == 24 &&
				DColor.AlphaMask	 == 0x000000 &&
				DColor.RedMask 		 == 0xff0000 &&
				DColor.GreenMask 	 == 0x00ff00 &&
				DColor.BlueMask 	 == 0x0000ff )
			{
#ifdef DX_NON_INLINE_ASM
				int i ;
				DWORD SrcC ;
				do
				{
					i = BltWidth ;
					do
					{
						SrcC = ( WORD )
							( ( ( *( ( WORD * )Src ) & 0x001f ) << 3 ) |
							  ( ( *( ( WORD * )Src ) & 0x07e0 ) << 5 ) |
							  ( ( *( ( WORD * )Src ) & 0xf800 ) << 8 ) ) ;
						*( ( WORD * )Dest ) = ( WORD )SrcC ;
						Dest[ 2 ] = ( BYTE )( SrcC >> 16 ) ;
						Src += 2 ;
						Dest += 3 ;
					}while( -- i != 0 ) ;

					Dest += DestAddPitch ;
					Src += SrcAddPitch ;

				}while( -- BltHeight != 0 ) ;
#else
				__asm
				{
					MOV EDI, Dest
					MOV ESI, Src
	LOCK14_LOOP1:
					MOV ECX, BltWidth
	LOCK14_LOOP2:
					MOV AX, [ESI]
					MOV EBX, EAX
					MOV EDX, EAX
					AND EBX, 0x0000f800
					AND EDX, 0x000007e0
					AND EAX, 0x0000001f
					SHL EBX, 8
					SHL EDX, 5
					SHL EAX, 3
					OR EDX, EBX
					OR EAX, EDX
					MOV [EDI], AX
					SHR EAX, 16
					MOV [EDI+2], AL
					ADD EDI, 3
					ADD ESI, 2
					DEC ECX
					JNZ LOCK14_LOOP2
					
					ADD EDI, DestAddPitch
					ADD ESI, SrcAddPitch
					DEC BltHeight
					JNZ LOCK14_LOOP1
				} ;
#endif
				return 0 ;
			}
			else
			if( DColor.ColorBitDepth == 32 &&
				DColor.AlphaMask	 == 0x00000000 &&
				DColor.RedMask 		 == 0x00ff0000 &&
				DColor.GreenMask 	 == 0x0000ff00 &&
				DColor.BlueMask 	 == 0x000000ff )
			{
#ifdef DX_NON_INLINE_ASM
				int i ;
				do
				{
					i = BltWidth ;
					do
					{
						*( ( DWORD * )Dest ) = ( DWORD )( 
							( ( *( ( WORD * )Src ) & 0x001f ) << 3 ) |
							( ( *( ( WORD * )Src ) & 0x07e0 ) << 5 ) |
							( ( *( ( WORD * )Src ) & 0xf800 ) << 8 ) ) ;
						Src += 2 ;
						Dest += 4 ;
					}while( -- i != 0 ) ;

					Dest += DestAddPitch ;
					Src += SrcAddPitch ;

				}while( -- BltHeight != 0 ) ;
#else
				__asm
				{
					MOV EDI, Dest
					MOV ESI, Src
	LOCK8_LOOP1:
					MOV ECX, BltWidth
	LOCK8_LOOP2:
					MOV AX, [ESI]
					MOV EBX, EAX
					MOV EDX, EAX
					AND EBX, 0x0000f800
					AND EDX, 0x000007e0
					AND EAX, 0x0000001f
					SHL EBX, 8
					SHL EDX, 5
					SHL EAX, 3
					OR EDX, EBX
					OR EAX, EDX
					MOV [EDI], EAX
					ADD EDI, 4
					ADD ESI, 2
					DEC ECX
					JNZ LOCK8_LOOP2
					
					ADD EDI, DestAddPitch
					ADD ESI, SrcAddPitch
					DEC BltHeight
					JNZ LOCK8_LOOP1
				} ;
#endif
				return 0 ;
			}
			else
			if( DColor.ColorBitDepth == 16 &&
				DColor.AlphaMask	 == 0x00000000 &&
				DColor.RedMask 		 == 0x00007c00 &&
				DColor.GreenMask 	 == 0x000003e0 &&
				DColor.BlueMask 	 == 0x0000001f )
			{
#ifdef DX_NON_INLINE_ASM
				int i ;
				do
				{
					i = BltWidth >> 1 ;
					if( i != 0 )
					{
						do
						{
							*( ( DWORD * )Dest ) =  
								( ( *( ( DWORD * )Src ) & 0xffc0ffc0 ) >> 1 ) |
								( ( *( ( DWORD * )Src ) & 0x001f001f ) ) ;
							Src += 4 ;
							Dest += 4 ;
						}while( -- i != 0 ) ;
					}

					if( ( BltWidth & 1 ) != 0 )
					{
						*( ( DWORD * )Dest ) =  
							( ( *( ( DWORD * )Src ) & 0xffc0 ) >> 1 ) |
							( ( *( ( DWORD * )Src ) & 0x001f ) ) ;
						Src += 2 ;
						Dest += 2 ;
					}

					Dest += DestAddPitch ;
					Src += SrcAddPitch ;

				}while( -- BltHeight != 0 ) ;
#else
				if( NS_GetUseMMXFlag() == TRUE )
				{
					__asm
					{
						MOV   EAX, 0xffc0ffc0
						MOV   EBX, 0x001f001f
						MOVD  MM0, EAX
						MOVD  MM1, EBX
						MOVQ  MM2, MM0
						MOVQ  MM3, MM1
						PSLLQ MM2, 32
						PSLLQ MM3, 32
						POR   MM0, MM2
						POR   MM1, MM3
						MOV EDI, Dest
						MOV ESI, Src
		LOCK9_MMX_LOOP1:
						MOV ECX, BltWidth
						SHR ECX, 2
						JZ LOCK9_MMX_LABEL1
		LOCK9_MMX_LOOP2:
						MOVQ  MM2, [ESI]
						MOVQ  MM3, MM2
						PAND  MM2, MM0
						PAND  MM3, MM1
						PSRLQ MM2, 1
						POR   MM2, MM3
						MOVQ  [EDI], MM2
						ADD EDI, 8
						ADD ESI, 8
						DEC ECX
						JNZ LOCK9_MMX_LOOP2

		LOCK9_MMX_LABEL1:
						MOV ECX, BltWidth
						AND ECX, 3
						JZ LOCK9_MMX_LABEL2
		LOCK9_MMX_LOOP3:
						MOV AX, [ESI]
						MOV EBX, EAX
						AND EBX, 0xffc0ffc0
						AND EAX, 0x001f001f
						SHR EBX, 1
						OR EAX, EBX
						MOV [EDI], AX
						ADD EDI, 2
						ADD ESI, 2
						DEC ECX
						JNZ LOCK9_MMX_LOOP3
						
		LOCK9_MMX_LABEL2:
						ADD EDI, DestAddPitch
						ADD ESI, SrcAddPitch
						DEC BltHeight
						JNZ LOCK9_MMX_LOOP1
						
						EMMS
					}
				}
				else
				{
					__asm
					{
						MOV EDI, Dest
						MOV ESI, Src
		LOCK9_LOOP1:
						MOV ECX, BltWidth
						SHR ECX, 1
						JZ LOCK9_LABEL1
		LOCK9_LOOP2:
						MOV EAX, [ESI]
						MOV EBX, EAX
						AND EBX, 0xffc0ffc0
						AND EAX, 0x001f001f
						SHR EBX, 1
						OR EAX, EBX
						MOV [EDI], EAX
						ADD EDI, 4
						ADD ESI, 4
						DEC ECX
						JNZ LOCK9_LOOP2

		LOCK9_LABEL1:
						TEST BltWidth, 1
						JZ LOCK9_LABEL2
						MOV AX, [ESI]
						MOV EBX, EAX
						AND EBX, 0xffc0ffc0
						AND EAX, 0x001f001f
						SHR EBX, 1
						OR EAX, EBX
						MOV [EDI], AX
						ADD EDI, 2
						ADD ESI, 2
						
		LOCK9_LABEL2:
						ADD EDI, DestAddPitch
						ADD ESI, SrcAddPitch
						DEC BltHeight
						JNZ LOCK9_LOOP1
					}
				}
#endif
				return 0 ;
			}
		}
		else
		if( TransColorAlphaTestFlag == TRUE &&
			SColor.ColorBitDepth    == 16 &&
			SColor.AlphaMask	    == 0x00000000 &&
			SColor.RedMask 	   	    == 0x00007c00 &&
			SColor.GreenMask 	    == 0x000003e0 &&
			SColor.BlueMask 	    == 0x0000001f )
		{
			BYTE *Dest ;
			BYTE *Src ;
			int DestAddPitch, SrcAddPitch ;
			DWORD TransCode ;

			DestAddPitch = DestPitch - BltWidth * DColor.PixelByte ;
			SrcAddPitch  = SrcPitch  - BltWidth * SColor.PixelByte ;

			Dest = ( BYTE * )DestGraphData + DestPoint.y  * DestPitch + DestPoint.x   * DColor.PixelByte ;
			Src  = ( BYTE * )SrcGraphData  + SrcRect->top * SrcPitch  + SrcRect->left * SColor.PixelByte ;

			if( DColor.ColorBitDepth == 16 &&
				DColor.AlphaMask	 == 0x00000001 &&
				DColor.RedMask 		 == 0x0000f800 &&
				DColor.GreenMask 	 == 0x000007c0 &&
				DColor.BlueMask 	 == 0x0000003e )
			{
				TransCode = ( DWORD )( 
							( ( ((BYTE *)&TransColor)[0] >> 3) <<  1) |
							( ( ((BYTE *)&TransColor)[1] >> 3) <<  6) |
							( ( ((BYTE *)&TransColor)[2] >> 3) << 11) ) ;

				int i ;
				DWORD SrcC ;
				do
				{
					i = BltWidth ;
					if( i != 0 )
					{
						do
						{
							SrcC = ( DWORD )( ( *( ( WORD * )Src ) << 1 ) & 0xfffe ) ;
							if( TransCode != SrcC )
							{
								*( ( WORD * )Dest ) = ( WORD )( SrcC | 0x0001 ) ;
							}
							else
							{
								*( ( WORD * )Dest ) = ( WORD )SrcC  ;
							}
							Src += 2 ;
							Dest += 2 ;
						}while( -- i != 0 ) ;
					}

					Dest += DestAddPitch ;
					Src += SrcAddPitch ;

				}while( -- BltHeight != 0 ) ;
				return 0 ;
			}
			else
			if( DColor.ColorBitDepth == 16 &&
				DColor.AlphaMask	 == 0x00008000 &&
				DColor.RedMask 		 == 0x00007c00 &&
				DColor.GreenMask 	 == 0x000003e0 &&
				DColor.BlueMask 	 == 0x0000001f )
			{
				TransCode = ( DWORD )( 
							(   ((BYTE *)&TransColor)[0] >> 3)        |
							( ( ((BYTE *)&TransColor)[1] >> 3) <<  5) |
							( ( ((BYTE *)&TransColor)[2] >> 3) << 10) ) ;

				int i ;
				DWORD SrcC ;
				do
				{
					i = BltWidth ;
					if( i != 0 )
					{
						do
						{
							SrcC = ( DWORD )( *( ( WORD * )Src ) & 0x7fff ) ;
							if( TransCode != SrcC )
							{
								*( ( WORD * )Dest ) = ( WORD )( SrcC | 0x8000 ) ;
							}
							else
							{
								*( ( WORD * )Dest ) = ( WORD )SrcC ;
							}
							Src += 2 ;
							Dest += 2 ;
						}while( -- i != 0 ) ;
					}

					Dest += DestAddPitch ;
					Src += SrcAddPitch ;

				}while( -- BltHeight != 0 ) ;
				return 0 ;
			}
			else
			if( DColor.ColorBitDepth == 16 &&
				DColor.AlphaMask	 == 0x00000000 &&
				DColor.RedMask 		 == 0x0000f800 &&
				DColor.GreenMask 	 == 0x000007e0 &&
				DColor.BlueMask 	 == 0x0000001f )
			{
				TransCode = ( DWORD )( 
							(   ((BYTE *)&TransColor)[0] >> 3)        |
							( ( ((BYTE *)&TransColor)[1] >> 2) <<  5) |
							( ( ((BYTE *)&TransColor)[2] >> 3) << 11) ) ;

				int i ;
				DWORD SrcC ;
				do
				{
					i = BltWidth ;
					if( i != 0 )
					{
						do
						{
							SrcC = ( DWORD )( ( *( ( WORD * )Src ) & 0x1f ) | ( ( *( ( WORD * )Src ) & 0x7fe0 ) << 1 ) ) ;
							*( ( DWORD * )Dest ) = ( WORD )SrcC ;
							Src += 2 ;
							Dest += 2 ;
						}while( -- i != 0 ) ;
					}

					Dest += DestAddPitch ;
					Src += SrcAddPitch ;

				}while( -- BltHeight != 0 ) ;
				return 0 ;
			}
			else
			if( DColor.ColorBitDepth == 32 &&
				DColor.AlphaMask	 == 0xff000000 &&
				DColor.RedMask 		 == 0x00ff0000 &&
				DColor.GreenMask 	 == 0x0000ff00 &&
				DColor.BlueMask 	 == 0x000000ff )
			{
				TransCode = ( DWORD )( 
							(   ((BYTE *)&TransColor)[0] >> 3)        |
							( ( ((BYTE *)&TransColor)[1] >> 3) <<  5) |
							( ( ((BYTE *)&TransColor)[2] >> 3) << 10) ) ;

				int i ;
				DWORD SrcC ;
				do
				{
					i = BltWidth ;
					if( i != 0 )
					{
						do
						{
							SrcC = *( ( WORD * )Src ) ;
							if( SrcC != TransCode )
							{
								*( ( DWORD * )Dest ) = 
									( ( SrcC & 0x001f ) << 3 ) |
									( ( SrcC & 0x03e0 ) << 6 ) |
									( ( SrcC & 0x7c00 ) << 9 ) |
									0xff000000 ;
							}
							else
							{
								*( ( DWORD * )Dest ) = 
									( ( SrcC & 0x001f ) << 3 ) |
									( ( SrcC & 0x03e0 ) << 6 ) |
									( ( SrcC & 0x7c00 ) << 9 ) ;
							}
							Src += 2 ;
							Dest += 4 ;
						}while( -- i != 0 ) ;
					}

					Dest += DestAddPitch ;
					Src += SrcAddPitch ;

				}while( -- BltHeight != 0 ) ;
				return 0 ;
			}
			else
			if( DColor.ColorBitDepth == 32 &&
				DColor.AlphaMask	 == 0xff000000 &&
				DColor.RedMask 		 == 0x000000ff &&
				DColor.GreenMask 	 == 0x0000ff00 &&
				DColor.BlueMask 	 == 0x00ff0000 )
			{
				TransCode = ( DWORD )( 
							(   ((BYTE *)&TransColor)[0] >> 3)        |
							( ( ((BYTE *)&TransColor)[1] >> 3) <<  5) |
							( ( ((BYTE *)&TransColor)[2] >> 3) << 10) ) ;

				int i ;
				DWORD SrcC ;
				do
				{
					i = BltWidth ;
					if( i != 0 )
					{
						do
						{
							SrcC = *( ( WORD * )Src ) ;
							if( SrcC != TransCode )
							{
								*( ( DWORD * )Dest ) = 
									( ( SrcC & 0x001f ) << 19 ) |
									( ( SrcC & 0x03e0 ) << 6 ) |
									( ( SrcC & 0x7c00 ) >> 7 ) |
									0xff000000 ;
							}
							else
							{
								*( ( DWORD * )Dest ) = 
									( ( SrcC & 0x001f ) << 19 ) |
									( ( SrcC & 0x03e0 ) << 6 ) |
									( ( SrcC & 0x7c00 ) >> 7 ) ;
							}
							Src += 2 ;
							Dest += 4 ;
						}while( -- i != 0 ) ;
					}

					Dest += DestAddPitch ;
					Src += SrcAddPitch ;

				}while( -- BltHeight != 0 ) ;
				return 0 ;
			}
		}
		else
		if( TransColorAlphaTestFlag == FALSE &&
			AlphaFlag               == FALSE &&
			SColor.ColorBitDepth    == 16 &&
			SColor.AlphaMask	    == 0x00000000 &&
			SColor.RedMask 	   	    == 0x00007c00 &&
			SColor.GreenMask 	    == 0x000003e0 &&
			SColor.BlueMask 	    == 0x0000001f )
		{
			BYTE *Dest ;
			BYTE *Src ;
			int DestAddPitch, SrcAddPitch ;

			DestAddPitch = DestPitch - BltWidth * DColor.PixelByte ;
			SrcAddPitch  = SrcPitch  - BltWidth * SColor.PixelByte ;

			Dest = ( BYTE * )DestGraphData + DestPoint.y  * DestPitch + DestPoint.x   * DColor.PixelByte ;
			Src  = ( BYTE * )SrcGraphData  + SrcRect->top * SrcPitch  + SrcRect->left * SColor.PixelByte ;

			if( DColor.ColorBitDepth == 16 &&
				DColor.AlphaMask	 == 0x00000000 &&
				DColor.RedMask 		 == 0x0000f800 &&
				DColor.GreenMask 	 == 0x000007e0 &&
				DColor.BlueMask 	 == 0x0000001f )
			{
#ifdef DX_NON_INLINE_ASM
				int i ;
				do
				{
					i = BltWidth >> 1 ;
					if( i != 0 )
					{
						do
						{
							*( ( DWORD * )Dest ) =  
								( ( *( ( DWORD * )Src ) & 0x7fe07fe0 ) << 1 ) |
								( ( *( ( DWORD * )Src ) & 0x003f003f ) ) ;
							Src += 4 ;
							Dest += 4 ;
						}while( -- i != 0 ) ;
					}

					if( ( BltWidth & 1 ) != 0 )
					{
						*( ( DWORD * )Dest ) =  
							( ( *( ( DWORD * )Src ) & 0x7fe0 ) << 1 ) |
							( ( *( ( DWORD * )Src ) & 0x003f ) ) ;
						Src += 2 ;
						Dest += 2 ;
					}

					Dest += DestAddPitch ;
					Src += SrcAddPitch ;

				}while( -- BltHeight != 0 ) ;
#else
				if( NS_GetUseMMXFlag() == TRUE )
				{
					__asm
					{
						MOV   EAX, 0x7fe07fe0
						MOV   EBX, 0x003f003f
						MOVD  MM0, EAX
						MOVD  MM1, EBX
						MOVQ  MM2, MM0
						MOVQ  MM3, MM1
						PSLLQ MM2, 32
						PSLLQ MM3, 32
						POR   MM0, MM2
						POR   MM1, MM3
						MOV EDI, Dest
						MOV ESI, Src
		LOCK10_MMX_LOOP1:
						MOV ECX, BltWidth
						SHR ECX, 2
						JZ LOCK10_MMX_LABEL1
		LOCK10_MMX_LOOP2:
						MOVQ  MM2, [ESI]
						MOVQ  MM3, MM2
						PAND  MM2, MM0
						PAND  MM3, MM1
						PSLLQ MM2, 1
						POR   MM2, MM3
						MOVQ  [EDI], MM2
						ADD EDI, 8
						ADD ESI, 8
						DEC ECX
						JNZ LOCK10_MMX_LOOP2

		LOCK10_MMX_LABEL1:
						MOV ECX, BltWidth
						AND ECX, 3
						JZ LOCK10_MMX_LABEL2
		LOCK10_MMX_LOOP3:
						MOV AX, [ESI]
						MOV EBX, EAX
						AND EBX, 0x7fe07fe0
						AND EAX, 0x003f003f
						SHL EBX, 1
						OR EAX, EBX
						MOV [EDI], AX
						ADD EDI, 2
						ADD ESI, 2
						DEC ECX
						JNZ LOCK10_MMX_LOOP3
						
		LOCK10_MMX_LABEL2:
						ADD EDI, DestAddPitch
						ADD ESI, SrcAddPitch
						DEC BltHeight
						JNZ LOCK10_MMX_LOOP1
						
						EMMS
					}
				}
				else
				{
					__asm
					{
						MOV EDI, Dest
						MOV ESI, Src
		LOCK10_LOOP1:
						MOV ECX, BltWidth
						SHR ECX, 1
						JZ LOCK10_LABEL1
		LOCK10_LOOP2:
						MOV EAX, [ESI]
						MOV EBX, EAX
						AND EBX, 0x7fe07fe0
						AND EAX, 0x003f003f
						SHL EBX, 1
						OR EAX, EBX
						MOV [EDI], EAX
						ADD EDI, 4
						ADD ESI, 4
						DEC ECX
						JNZ LOCK10_LOOP2

		LOCK10_LABEL1:
						TEST BltWidth, 1
						JZ LOCK10_LABEL2

						MOV AX, [ESI]
						MOV EBX, EAX
						AND EBX, 0x7fe07fe0
						AND EAX, 0x003f003f
						SHL EBX, 1
						OR EAX, EBX
						MOV [EDI], AX
						ADD EDI, 2
						ADD ESI, 2
						
		LOCK10_LABEL2:
						ADD EDI, DestAddPitch
						ADD ESI, SrcAddPitch
						DEC BltHeight
						JNZ LOCK10_LOOP1
					}
				}
#endif
				return 0 ;
			}
		}
	}

NORMALMOVE:

	// いずれにも対応していない場合は普通の転送処理を行う
	{
		DWORD DitherTableRed[16], DitherTableBlue[16], DitherTableGreen[16], DitherTableAlpha[16] ;
		DWORD DiffMaskRed = 0, DiffMaskGreen = 0, DiffMaskBlue = 0, DiffMaskAlpha = 0 ;

		DWORD SrcPitch2, DestPitch2, AlphaPitch2 ;
		DWORD ARGBMask = 0 ;
		DWORD GRed = 0, GGreen = 0 ,GBlue = 0, GAlpha = 0 ;
		DWORD TRed = 0, TGreen = 0, TBlue = 0 ;
		BYTE GraphShavedFlag, DestColorBitDepth, SrcColorBitDepth, AlphaColorBitDepth ;
		BYTE  GWRed    = 0, GWGreen    = 0, GWBlue    = 0, GWAlpha    = 0 ;
		DWORD GRedMask = 0, GGreenMask = 0, GBlueMask = 0, GAlphaMask = 0 ;
		DWORD RedMask  = 0, GreenMask  = 0, BlueMask  = 0, AMask      = 0 ;
		DWORD *SrcPalette, *AlphaPalette ;
		DWORD SrcValidMask ;

		// 浮動小数点フォーマットの場合は 32bit ARGBとする
		// 1チャンネルが16ビットの整数型だった場合も仮に 32bit ARGB とする
		if( SColor.FloatTypeFlag != 0 ||
			( SColor.FloatTypeFlag == 0 && SColor.ChannelBitDepth == 16 ) )
		{
			SColor.ColorBitDepth = 32 ;

			SColor.RedMask   = 0x00ff0000 ; SColor.RedLoc   = 16 ; SColor.RedWidth   = 8 ;
			SColor.GreenMask = 0x0000ff00 ; SColor.GreenLoc =  8 ; SColor.GreenWidth = 8 ;
			SColor.BlueMask  = 0x000000ff ; SColor.BlueLoc  =  0 ; SColor.BlueWidth  = 8 ;
			SColor.AlphaMask = 0xff000000 ; SColor.AlphaLoc = 24 ; SColor.AlphaWidth = 8 ;
		}
		if( DColor.FloatTypeFlag != 0 ||
			( DColor.FloatTypeFlag == 0 && DColor.ChannelBitDepth == 16 ) )
		{
			DColor.ColorBitDepth = 32 ;

			DColor.RedMask   = 0x00ff0000 ; DColor.RedLoc   = 16 ; DColor.RedWidth   = 8 ;
			DColor.GreenMask = 0x0000ff00 ; DColor.GreenLoc =  8 ; DColor.GreenWidth = 8 ;
			DColor.BlueMask  = 0x000000ff ; DColor.BlueLoc  =  0 ; DColor.BlueWidth  = 8 ;
			DColor.AlphaMask = 0xff000000 ; DColor.AlphaLoc = 24 ; DColor.AlphaWidth = 8 ;
		}

		// グラフィック劣化緩和処理モードをローカル変数に落とす
		GraphShavedFlag = ( BYTE )(
							( SColor.RedWidth   != DColor.RedWidth   ) || 
							( SColor.GreenWidth != DColor.GreenWidth ) || 
							( SColor.RedWidth   != DColor.RedWidth   ) ) ; 
		if( AlphaMaskFlag && AColor.RedWidth != DColor.AlphaWidth ) GraphShavedFlag = TRUE;

		// 転送先が浮動小数点型の場合か1チャンネルが16ビットの整数型の場合はグラフィック劣化緩和処理は使用できない
		if( DColor.FloatTypeFlag != 0 || 
			( DColor.FloatTypeFlag == 0 && DColor.ChannelBitDepth == 16 ) )
		{
			GraphShavedFlag = FALSE ;
		}

		// 透過色やアルファテストやアルファマスクは浮動小数点画像や1チャンネルが16ビットの整数型の場合は使用できない
		if( SColor.FloatTypeFlag != 0 || DColor.FloatTypeFlag != 0 ||
			( SColor.FloatTypeFlag == 0 && SColor.ChannelBitDepth == 16 ) ||
			( DColor.FloatTypeFlag == 0 && DColor.ChannelBitDepth == 16 ) )
		{
			TransColorNoMoveFlag = FALSE ;
			TransColorAlphaTestFlag = FALSE ;
			AlphaMaskFlag = FALSE ;
		}

		// パレットセット
		SrcPalette = (DWORD *)SColor.Palette ;
		AlphaPalette = (DWORD *)AColor.Palette ;

		// 色数をセット
		SrcColorBitDepth = 0 ;
		if( SColor.FloatTypeFlag != 0 )
		{
			switch( SColor.ChannelBitDepth )
			{
			case 32 : SrcColorBitDepth = 4 ; break ;
			case 16 : SrcColorBitDepth = 5 ; break ;
			}
		}
		else
		if( SColor.ChannelBitDepth == 16 )
		{
			switch( SColor.ChannelBitDepth )
			{
			case 16 : SrcColorBitDepth = 6 ; break ;
			}
		}
		else
		{
			switch( SColor.ColorBitDepth )
			{
			case 16 : SrcColorBitDepth = 0 ; break ;
			case 24 : SrcColorBitDepth = 1 ; break ;
			case 32 : SrcColorBitDepth = 2 ; break ;
			case 8  : SrcColorBitDepth = 3 ; break ;
			}
		}

		// 転送元の有効ビットのセット
		SrcValidMask = SColor.RedMask | SColor.GreenMask | SColor.BlueMask | SColor.AlphaMask ;
		if( TransColorAlphaTestFlag == FALSE )
		{
			switch( SColor.ColorBitDepth )
			{
			case 16 : TransColor &= 0xffff     ; break ;
			case 24 : TransColor &= 0xffffff   ; break ;
			case 32 : TransColor &= 0xffffffff ; break ;
			case 8  : TransColor &= 0xff       ; break ;
			}

			// 透過色のセッティング
			if( SColor.ColorBitDepth != 8 )
			{
				TransColor &= SrcValidMask ;
			}
		}

		DestColorBitDepth = 0 ;
		if( DColor.FloatTypeFlag != 0 )
		{
			switch( DColor.ChannelBitDepth )
			{
			case 32 : DestColorBitDepth = 4 ; break ;
			case 16 : DestColorBitDepth = 5 ; break ;
			}
		}
		else
		if( DColor.ChannelBitDepth == 16 )
		{
			switch( DColor.ChannelBitDepth )
			{
			case 16 : DestColorBitDepth = 6 ; break ;
			}
		}
		else
		{
			switch( DColor.ColorBitDepth )
			{
			case 16 : DestColorBitDepth = 0 ; break ;
			case 24 : DestColorBitDepth = 1 ; break ;
			case 32 : DestColorBitDepth = 2 ; break ;
			case  8 : DestColorBitDepth = 3 ; break ;
			}
		}
		
		AlphaColorBitDepth = 0 ;
		switch( AColor.ColorBitDepth )
		{
		case 16 : AlphaColorBitDepth = 0 ; break ;
		case 24 : AlphaColorBitDepth = 1 ; break ;
		case 32 : AlphaColorBitDepth = 2 ; break ;
		case  8 : AlphaColorBitDepth = 3 ; break ;
		}

		// 色幅が違った場合は減色緩和レンダリング処理を行う
		if( GraphShavedFlag == TRUE )
		{
			DWORD DiffRed, DiffGreen, DiffBlue, DiffAlpha ;
			int i ;

			GWRed	= ( BYTE )( SColor.RedWidth   - DColor.RedWidth   ) ;
			GWGreen	= ( BYTE )( SColor.GreenWidth - DColor.GreenWidth ) ;
			GWBlue	= ( BYTE )( SColor.BlueWidth  - DColor.BlueWidth  ) ;
			GWAlpha	= ( BYTE )( ( AlphaMaskFlag ? AColor.RedWidth : SColor.AlphaWidth	) - DColor.AlphaWidth ) ;

			DiffRed   = ( DWORD )( 1 << GWRed   ) ;
			DiffGreen = ( DWORD )( 1 << GWGreen ) ;
			DiffBlue  = ( DWORD )( 1 << GWBlue  ) ;
			DiffAlpha = ( DWORD )( 1 << GWAlpha ) ;

			// 減色緩和モードがディザリングだった場合はテーブルを埋める
			if( ImageShavedMode == DX_SHAVEDMODE_DITHER )
			{
				for( i = 0 ; i < 16 ; i ++ )
				{
					DitherTableRed  [ i ] = ( DiffRed   * DitherTable[ i ] / 16 ) << SColor.RedLoc ;
					DitherTableGreen[ i ] = ( DiffGreen * DitherTable[ i ] / 16 ) << SColor.GreenLoc ;
					DitherTableBlue [ i ] = ( DiffBlue  * DitherTable[ i ] / 16 ) << SColor.BlueLoc ;
					DitherTableAlpha[ i ] = ( DiffAlpha * DitherTable[ i ] / 16 ) << ( AlphaMaskFlag ? AColor.RedLoc : SColor.AlphaLoc ) ;
				}
			}

			DiffMaskRed   = ( DiffRed   - 1 ) << SColor.RedLoc   ;
			DiffMaskGreen = ( DiffGreen - 1 ) << SColor.GreenLoc ;
			DiffMaskBlue  = ( DiffBlue  - 1 ) << SColor.BlueLoc  ;
			DiffMaskAlpha = ( DiffAlpha - 1 ) << ( AlphaMaskFlag ? AColor.RedLoc : SColor.AlphaLoc ) ;

			ARGBMask  = ~( DColor.RedMask | DColor.GreenMask | DColor.BlueMask | DColor.AlphaMask ) ;
			
			RedMask   = SColor.RedMask   >> SColor.RedLoc   ;
			GreenMask = SColor.GreenMask >> SColor.GreenLoc ;
			BlueMask  = SColor.BlueMask  >> SColor.BlueLoc  ;
			AMask     = ( AlphaMaskFlag ? ( AColor.RedMask >> AColor.RedLoc ) : ( SColor.AlphaMask >> SColor.AlphaLoc ) ) ;
			
			GRedMask   = ( DWORD )( ( 1 << ( DColor.RedWidth   - SColor.RedWidth   ) ) - 1 ) ;
			GGreenMask = ( DWORD )( ( 1 << ( DColor.GreenWidth - SColor.GreenWidth ) ) - 1 ) ;
			GBlueMask  = ( DWORD )( ( 1 << ( DColor.BlueWidth  - SColor.BlueWidth  ) ) - 1 ) ;
			GAlphaMask = ( DWORD )( ( 1 << ( DColor.AlphaWidth - ( AlphaMaskFlag ? AColor.RedWidth : SColor.AlphaWidth	) ) )  - 1 ) ;

			GAlpha = GRed = GBlue = GGreen = 0 ;
		}
		
//		DXST_LOGFILEFMT_ADDW(( L"GWAlpha:%d   GAlphaMask:%x   AlphaMask:%x", GWAlpha, GAlphaMask, AlphaMask )) ;

		// カラーキーを取得
		if( TransColorAlphaTestFlag )
		{
			unsigned int Color ;

			TRed   = ( ( TransColor >> 16 ) & 0xff )  ;
			TGreen = ( ( TransColor >>  8 ) & 0xff )  ;
			TBlue  = ( ( TransColor       ) & 0xff )  ;
			Color  = NS_GetColor3( &SColor, ( int )TRed, ( int )TGreen, ( int )TBlue ) ;
			if( SColor.ColorBitDepth == 8 )
			{
				TRed   = SColor.Palette[ Color ].Red ;
				TGreen = SColor.Palette[ Color ].Green ;
				TBlue  = SColor.Palette[ Color ].Blue ;
			}
			else
			{
				TRed   = ( Color & SColor.RedMask   ) >> SColor.RedLoc ;
				TGreen = ( Color & SColor.GreenMask ) >> SColor.GreenLoc ;
				TBlue  = ( Color & SColor.BlueMask  ) >> SColor.BlueLoc ;
			}
		}

		// １ライン転送する度に加算する値セット
		if( ReverseFlag == FALSE )
		{
			DestPitch2  = ( DWORD )( DestPitch  - BltWidth * DColor.PixelByte ) ;
			SrcPitch2   = ( DWORD )( SrcPitch   - BltWidth * SColor.PixelByte ) ;
			AlphaPitch2 = ( DWORD )( AlphaPitch - BltWidth * AColor.PixelByte ) ;
		}
		else
		{
			DestPitch2  = ( DWORD )( DestPitch  + BltWidth * DColor.PixelByte ) ;
			SrcPitch2   = ( DWORD )( SrcPitch   - BltWidth * SColor.PixelByte ) ;
			AlphaPitch2 = ( DWORD )( AlphaPitch - BltWidth * AColor.PixelByte ) ;
		}

		// 転送処理
		{
			BYTE *DBuf, *SBuf, *ABuf ;
			DWORD DestColor = 0, SrcColor  = 0, AlphaColor = 0, NoneMask = 0 ;
			WORD SrcColorR_I = 0, SrcColorG_I = 0, SrcColorB_I = 0, SrcColorA_I = 0 ;
			float SrcColorR_F = 0, SrcColorG_F = 0, SrcColorB_F = 0, SrcColorA_F = 0 ;
			int i, j, DestAdd ;

			// 無効ビットをセット
			NoneMask = DColor.NoneMask ;
			
			// 反転転送かどうかで、転送先の初期アドレスなどを変更
			if( ReverseFlag == TRUE )
			{
				DBuf = ( BYTE * )DestGraphData + DestPoint.y * DestPitch + DestPoint.x * DColor.PixelByte + ( BltWidth - 1 ) * DColor.PixelByte ;
				DestAdd = -DColor.PixelByte ;
			}
			else
			{
				DBuf = ( BYTE * )DestGraphData + DestPoint.y * DestPitch + DestPoint.x * DColor.PixelByte ;
				DestAdd = DColor.PixelByte ;
			}
			
			// 転送先以外のデータは通常通り
			SBuf = ( BYTE * )SrcGraphData + SrcRect->top * SrcPitch   + SrcRect->left * SColor.PixelByte ;
			ABuf = ( BYTE * )AlphaMask    + SrcRect->top * AlphaPitch + SrcRect->left * AColor.PixelByte ;

			for( i = 0 ; i < BltHeight ; i ++, DBuf += DestPitch2, SBuf += SrcPitch2, ABuf += AlphaPitch2 )
			{
				for( j = 0 ; j < BltWidth ; j ++, DBuf += DestAdd, SBuf += SColor.PixelByte, ABuf += AColor.PixelByte )
				{
					// カラーを得る
					switch( SrcColorBitDepth )
					{
					case 0 :	SrcColor = ( DWORD )( *( ( WORD  * )SBuf ) ) ; break ; 
					case 1 :	SrcColor = ( DWORD )( *( ( WORD  * )SBuf ) ) | ( ( DWORD )( SBuf[ 2 ] ) << 16 ) ; break ; 
					case 2 :	SrcColor = ( DWORD )( *( ( DWORD * )SBuf ) ) ; break ;
					case 3 :	SrcColor = ( DWORD )( SrcPalette[*( ( BYTE * )SBuf )] ) ; break ;

					case 4 :
						SrcColorR_F =                         ( ( float * )SBuf )[ 0 ] ;
						SrcColorG_F = SColor.ChannelNum > 1 ? ( ( float * )SBuf )[ 1 ] : 1.0f ;
						SrcColorB_F = SColor.ChannelNum > 2 ? ( ( float * )SBuf )[ 2 ] : 1.0f ;
						SrcColorA_F = SColor.ChannelNum > 3 ? ( ( float * )SBuf )[ 3 ] : 1.0f ;
						break ;

					case 5 :
						SrcColorR_F =                         Float16ToFloat32( ( ( WORD * )SBuf )[ 0 ] ) ;
						SrcColorG_F = SColor.ChannelNum > 1 ? Float16ToFloat32( ( ( WORD * )SBuf )[ 1 ] ) : 1.0f ;
						SrcColorB_F = SColor.ChannelNum > 2 ? Float16ToFloat32( ( ( WORD * )SBuf )[ 2 ] ) : 1.0f ;
						SrcColorA_F = SColor.ChannelNum > 3 ? Float16ToFloat32( ( ( WORD * )SBuf )[ 3 ] ) : 1.0f ;
						break ;

					case 6 :
						SrcColorR_I =                         ( ( WORD * )SBuf )[ 0 ] ;
						SrcColorG_I = SColor.ChannelNum > 1 ? ( ( WORD * )SBuf )[ 1 ] : 0xffff ;
						SrcColorB_I = SColor.ChannelNum > 2 ? ( ( WORD * )SBuf )[ 2 ] : 0xffff ;
						SrcColorA_I = SColor.ChannelNum > 3 ? ( ( WORD * )SBuf )[ 3 ] : 0xffff ;
						break ;
					}

					if( SColor.FloatTypeFlag != 0 )
					{
						if( DColor.FloatTypeFlag == 0 )
						{
							if( DColor.ChannelBitDepth == 16 )
							{
								SrcColorR_I = 0 ;
								SrcColorG_I = 0 ;
								SrcColorB_I = 0 ;
								SrcColorA_I = 0 ;

									 if( SrcColorR_F > 1.0f ) SrcColorR_I = 0xffff ;
								else if( SrcColorR_F > 0.0f ) SrcColorR_I = ( WORD )_FTOL( SrcColorR_F * 0xffff ) ;

									 if( SrcColorG_F > 1.0f ) SrcColorG_I = 0xffff ;
								else if( SrcColorG_F > 0.0f ) SrcColorG_I = ( WORD )_FTOL( SrcColorG_F * 0xffff ) ;

									 if( SrcColorB_F > 1.0f ) SrcColorB_I = 0xffff ;
								else if( SrcColorB_F > 0.0f ) SrcColorB_I = ( WORD )_FTOL( SrcColorB_F * 0xffff ) ;

									 if( SrcColorA_F > 1.0f ) SrcColorA_I = 0xffff ;
								else if( SrcColorA_F > 0.0f ) SrcColorA_I = ( WORD )_FTOL( SrcColorA_F * 0xffff ) ;
							}
							else
							{
								SrcColor = 0 ;

									 if( SrcColorR_F > 1.0f ) SrcColor |= ( DWORD )0xff << SColor.RedLoc ;
								else if( SrcColorR_F > 0.0f ) SrcColor |= ( DWORD )( _FTOL( SrcColorR_F * 255.0f ) << SColor.RedLoc ) ;

									 if( SrcColorG_F > 1.0f ) SrcColor |= ( DWORD )0xff << SColor.GreenLoc ;
								else if( SrcColorG_F > 0.0f ) SrcColor |= ( DWORD )( _FTOL( SrcColorG_F * 255.0f ) << SColor.GreenLoc ) ;

									 if( SrcColorB_F > 1.0f ) SrcColor |= ( DWORD )0xff << SColor.BlueLoc ;
								else if( SrcColorB_F > 0.0f ) SrcColor |= ( DWORD )( _FTOL( SrcColorB_F * 255.0f ) << SColor.BlueLoc ) ;

									 if( SrcColorA_F > 1.0f ) SrcColor |= ( DWORD )0xff << SColor.AlphaLoc ;
								else if( SrcColorA_F > 0.0f ) SrcColor |= ( DWORD )( _FTOL( SrcColorA_F * 255.0f ) << SColor.AlphaLoc ) ;
							}
						}
					}
					else
					if( SColor.ChannelBitDepth == 16 )
					{
						if( DColor.FloatTypeFlag != 0 )
						{
							SrcColorR_F = SrcColorR_I / 65535.0f ;
							SrcColorG_F = SrcColorG_I / 65535.0f ;
							SrcColorB_F = SrcColorB_I / 65535.0f ;
							SrcColorA_F = SrcColorA_I / 65535.0f ;
						}
						else
						if( DColor.ChannelBitDepth != 16 )
						{
							SrcColor = 0 ;

							SrcColor |= ( DWORD )( ( SrcColorR_I >> 8 ) << SColor.RedLoc ) ;
							SrcColor |= ( DWORD )( ( SrcColorG_I >> 8 ) << SColor.GreenLoc ) ;
							SrcColor |= ( DWORD )( ( SrcColorB_I >> 8 ) << SColor.BlueLoc ) ;
							SrcColor |= ( DWORD )( ( SrcColorA_I >> 8 ) << SColor.AlphaLoc ) ;
						}
					}
					else
					{
						if( DColor.FloatTypeFlag != 0 )
						{
							SrcColorR_F = SColor.RedWidth   == 0 ? 1.0f : ( float )( ( SrcColor & SColor.RedMask   ) >> SColor.RedLoc   ) / ( SColor.RedMask   >> SColor.RedLoc   ) ;
							SrcColorG_F = SColor.GreenWidth == 0 ? 1.0f : ( float )( ( SrcColor & SColor.GreenMask ) >> SColor.GreenLoc ) / ( SColor.GreenMask >> SColor.GreenLoc ) ;
							SrcColorB_F = SColor.BlueWidth  == 0 ? 1.0f : ( float )( ( SrcColor & SColor.BlueMask  ) >> SColor.BlueLoc  ) / ( SColor.BlueMask  >> SColor.BlueLoc  ) ;
							SrcColorA_F = SColor.AlphaWidth == 0 ? 1.0f : ( float )( ( SrcColor & SColor.AlphaMask ) >> SColor.AlphaLoc ) / ( SColor.AlphaMask >> SColor.AlphaLoc ) ;
						}
						else
						if( DColor.ChannelBitDepth > 8 )
						{
							SrcColorR_I = SColor.RedWidth   == 0 ? 0xffff : ( WORD )( ( ( SrcColor & SColor.RedMask   ) >> SColor.RedLoc   ) * 0xffff / ( SColor.RedMask   >> SColor.RedLoc   ) ) ;
							SrcColorG_I = SColor.GreenWidth == 0 ? 0xffff : ( WORD )( ( ( SrcColor & SColor.GreenMask ) >> SColor.GreenLoc ) * 0xffff / ( SColor.GreenMask >> SColor.GreenLoc ) ) ;
							SrcColorB_I = SColor.BlueWidth  == 0 ? 0xffff : ( WORD )( ( ( SrcColor & SColor.BlueMask  ) >> SColor.BlueLoc  ) * 0xffff / ( SColor.BlueMask  >> SColor.BlueLoc  ) ) ;
							SrcColorA_I = SColor.AlphaWidth == 0 ? 0xffff : ( WORD )( ( ( SrcColor & SColor.AlphaMask ) >> SColor.AlphaLoc ) * 0xffff / ( SColor.AlphaMask >> SColor.AlphaLoc ) ) ;
						}
					}

					// 透過色処理
					if( TransColorNoMoveFlag )
					{
						if( SrcColorBitDepth != 3 )
						{
							if( ( SrcColor & SrcValidMask ) == TransColor )
								continue ;
						}
						else
						if( SrcColor == TransColor ) continue ;
					}

					// アルファマスクからアルファ値を取る場合はアルファ値も
					if( AlphaMaskFlag == TRUE )
					{
						switch( AlphaColorBitDepth )
						{
						case 0 :	AlphaColor = ( DWORD )( *( ( WORD * )ABuf ) ) ; break ; 
						case 1 :	AlphaColor = ( DWORD )( *( ( WORD * )ABuf ) ) | ( ( DWORD )( ABuf[ 2 ] ) << 16 ) ; break ; 
						case 2 :	AlphaColor = ( DWORD )( *( ( DWORD * )ABuf ) ) ; break ;
						case 3 :	AlphaColor = ( DWORD )( AlphaPalette[*( ( BYTE * )ABuf )] ) ; break ;
						}
					}

					// 減色処理
					if( DColor.FloatTypeFlag == FALSE )
					{
						DWORD Red, Blue, Green, Alpha ;

						Red   = ( SrcColor & SColor.RedMask   ) >> SColor.RedLoc ;
						Green = ( SrcColor & SColor.GreenMask ) >> SColor.GreenLoc ;
						Blue  = ( SrcColor & SColor.BlueMask  ) >> SColor.BlueLoc ;

						if( DColor.AlphaWidth != 0 && TransColorAlphaTestFlag == TRUE )
						{
							if( Red == TRed && Green == TGreen && Blue == TBlue )
								Alpha = 0 ;
							else
								Alpha = DColor.AlphaMask >> DColor.AlphaLoc ;
						}
						else
						{
							if( AlphaMaskFlag == TRUE )
							{
								Alpha = ( AlphaColor & AColor.RedMask ) >> AColor.RedLoc ;
								if( GWAlpha ){ if( GWAlpha & 0x80 ) Alpha = ( Alpha << ( ~GWAlpha + 1 ) ) + ( Alpha * GAlphaMask / AMask ) ; else Alpha >>= GWAlpha ; }
							}
							else
							{
								if( SColor.AlphaWidth == 0 )
								{
									Alpha = DColor.AlphaMask >> DColor.AlphaLoc ;
								}
								else
								if( DColor.AlphaMask == 0 )
								{
									Alpha = 0 ;
								}
								else
								{
									Alpha = ( SrcColor & SColor.AlphaMask ) >> SColor.AlphaLoc ;
									if( GWAlpha ){ if( GWAlpha & 0x80 ) Alpha = ( Alpha << ( ~GWAlpha + 1 ) ) + ( Alpha * GAlphaMask / AMask ) ; else Alpha >>= GWAlpha ; }
								}
							}

							if( RedIsAlphaFlag ) Alpha = Red ;
						}

						if( GWRed   && RedMask   != 0 ){ if( GWRed   & 0x80 ) Red   = ( Red   << ( ~GWRed   + 1 ) ) + ( Red   * GRedMask   / RedMask   ) ; else Red   >>= GWRed   ; }
						if( GWGreen && GreenMask != 0 ){ if( GWGreen & 0x80 ) Green = ( Green << ( ~GWGreen + 1 ) ) + ( Green * GGreenMask / GreenMask ) ; else Green >>= GWGreen ; }
						if( GWBlue  && BlueMask  != 0 ){ if( GWBlue  & 0x80 ) Blue  = ( Blue  << ( ~GWBlue  + 1 ) ) + ( Blue  * GBlueMask  / BlueMask  ) ; else Blue  >>= GWBlue  ; }

						if( AlphaOnlyFlag ) Green = Blue = Red = 0 ;
						DestColor = ( Red   << DColor.RedLoc   ) |
									( Blue  << DColor.BlueLoc  ) |
									( Green << DColor.GreenLoc ) |
									( Alpha << DColor.AlphaLoc ) ;
					}

					// 画像劣化緩和処理
					if( GraphShavedFlag )
					{
						DWORD Red, Green, Blue, Alpha ;

						Red   = DestColor & DColor.RedMask ;
						Green = DestColor & DColor.GreenMask ;
						Blue  = DestColor & DColor.BlueMask ;
						Alpha = DestColor & DColor.AlphaMask ;

						switch( ImageShavedMode )
						{
						case DX_SHAVEDMODE_DITHER :		// ディザリングによる緩和処理
							Red   += ( ( SrcColor & DiffMaskRed   ) > DitherTableRed  [ ( ( i & 3 ) << 2 ) + ( j & 3 ) ] ) << DColor.RedLoc ;
							Green += ( ( SrcColor & DiffMaskGreen ) > DitherTableGreen[ ( ( i & 3 ) << 2 ) + ( j & 3 ) ] ) << DColor.GreenLoc ;
							Blue  += ( ( SrcColor & DiffMaskBlue  ) > DitherTableBlue [ ( ( i & 3 ) << 2 ) + ( j & 3 ) ] ) << DColor.BlueLoc ;
							Alpha += ( ( SrcColor & DiffMaskAlpha ) > DitherTableAlpha[ ( ( i & 3 ) << 2 ) + ( j & 3 ) ] ) << DColor.AlphaLoc ;
							break ;

						case DX_SHAVEDMODE_DIFFUS :		// 誤差拡散による緩和処理
							GRed   += SrcColor & DiffMaskRed ;
							GGreen += SrcColor & DiffMaskGreen ;
							GBlue  += SrcColor & DiffMaskBlue ;
							GAlpha += SrcColor & DiffMaskAlpha ;

							Red   += ( GRed   > DiffMaskRed   ) << DColor.RedLoc   ;
							Green += ( GGreen > DiffMaskGreen ) << DColor.GreenLoc ;
							Blue  += ( GBlue  > DiffMaskBlue  ) << DColor.BlueLoc  ;
							Alpha += ( GAlpha > DiffMaskAlpha ) << DColor.AlphaLoc ;

							GRed   &= DiffMaskRed ;
							GGreen &= DiffMaskGreen ;
							GBlue  &= DiffMaskBlue ;
							GAlpha &= DiffMaskAlpha ;
							break ;
						}

						if( Red   > ( DWORD )DColor.RedMask   ) Red   = ( DWORD )DColor.RedMask ;
						if( Green > ( DWORD )DColor.GreenMask ) Green = ( DWORD )DColor.GreenMask ;
						if( Blue  > ( DWORD )DColor.BlueMask  ) Blue  = ( DWORD )DColor.BlueMask ;
						if( Alpha > ( DWORD )DColor.AlphaMask ) Alpha = ( DWORD )DColor.AlphaMask ;

						DestColor = ( DestColor & ARGBMask ) | Red | Blue | Green | Alpha ;
					}

					// 転送
					switch( DestColorBitDepth )
					{
					case 0 :	*( ( WORD  * )DBuf ) = ( WORD )( DestColor | NoneMask ) ; break ; 
					case 1 :	*( ( WORD  * )DBuf ) = ( WORD )DestColor  ; *( ( BYTE * )( DBuf + 2 ) ) = ( BYTE )( DestColor >> 16 ) ; break ; 
					case 2 :	*( ( DWORD * )DBuf ) = ( DWORD )( DestColor | NoneMask ) ; break ;
					case 3 :	*( ( BYTE  * )DBuf ) = ( BYTE )DestColor ; break ;

					case 4 :
						                            ( ( float * )DBuf )[ 0 ] = SrcColorR_F ;
						if( DColor.ChannelNum > 1 ) ( ( float * )DBuf )[ 1 ] = SrcColorG_F ;
						if( DColor.ChannelNum > 2 ) ( ( float * )DBuf )[ 2 ] = SrcColorB_F ;
						if( DColor.ChannelNum > 3 ) ( ( float * )DBuf )[ 3 ] = SrcColorA_F ;
						break ;

					case 5 :
						                            ( ( WORD * )DBuf )[ 0 ] = Float32ToFloat16( SrcColorR_F ) ;
						if( DColor.ChannelNum > 1 ) ( ( WORD * )DBuf )[ 1 ] = Float32ToFloat16( SrcColorG_F ) ;
						if( DColor.ChannelNum > 2 ) ( ( WORD * )DBuf )[ 2 ] = Float32ToFloat16( SrcColorB_F ) ;
						if( DColor.ChannelNum > 3 ) ( ( WORD * )DBuf )[ 3 ] = Float32ToFloat16( SrcColorA_F ) ;
						break ;

					case 6 :
						                            ( ( WORD * )DBuf )[ 0 ] = SrcColorR_I ;
						if( DColor.ChannelNum > 1 ) ( ( WORD * )DBuf )[ 1 ] = SrcColorG_I ;
						if( DColor.ChannelNum > 2 ) ( ( WORD * )DBuf )[ 2 ] = SrcColorB_I ;
						if( DColor.ChannelNum > 3 ) ( ( WORD * )DBuf )[ 3 ] = SrcColorA_I ;
						break ;
					}
				}
			}
		}
	}

	// 終了
	return 0 ;
}











// 色情報取得関係
extern COLOR_F NS_GetColorF( float Red, float Green, float Blue, float Alpha )
{
	COLOR_F Ret = { Red,  Green,  Blue,  Alpha } ;
	return Ret ;
}

extern COLOR_U8 NS_GetColorU8( int Red, int Green, int Blue, int Alpha )
{
	COLOR_U8 Ret = { ( BYTE )Blue, ( BYTE )Green, ( BYTE )Red, ( BYTE )Alpha } ;
	return Ret ;
}

// DrawPixel 等の描画関数で使用するカラー値を取得する
extern	unsigned int	NS_GetColor( int Red, int Green, int Blue )
{
#ifndef DX_NON_GRAPHICS
	const COLORDATA *ColorData ;

	// 色情報を返す
	if( GSYS.Setting.ValidHardware == TRUE )
	{
//		ColorData = Graphics_Hardware_GetMainColorData_PF() ;
		return 0xff000000 | ( ( ( unsigned int )Red ) << 16 ) | ( ( ( unsigned int )Green ) << 8 ) | ( ( unsigned int )Blue ) ;
	}
	else
	{
		ColorData = GetMemImgColorData( GSYS.Screen.MainScreenColorBitDepth == 16 ? 0 : 1, FALSE, FALSE ) ;
	}
	
	return ColorData->NoneMask +
				( ( ( BYTE )Red   >> ( 8 - ColorData->RedWidth   ) ) << ColorData->RedLoc   ) +
				( ( ( BYTE )Green >> ( 8 - ColorData->GreenWidth ) ) << ColorData->GreenLoc ) +
				( ( ( BYTE )Blue  >> ( 8 - ColorData->BlueWidth  ) ) << ColorData->BlueLoc  ) ;
#else // DX_NON_GRAPHICS
	return 0 ;
#endif // DX_NON_GRAPHICS
}

// カラー値から赤、緑、青、アルファの値を取得する
extern	int		NS_GetColor2( unsigned int Color, int *Red, int *Green, int *Blue )
{
#ifndef DX_NON_GRAPHICS
	const COLORDATA *ColorData ;
	int MaxRed ;
	int MaxGreen ;
	int MaxBlue ;

	// 色情報を返す
	if( GSYS.Setting.ValidHardware == TRUE )
	{
//		ColorData = Graphics_Hardware_GetMainColorData_PF() ;
		*Red   = ( int )( ( Color >> 16 ) & 0xff ) ;
		*Green = ( int )( ( Color >>  8 ) & 0xff ) ;
		*Blue  = ( int )( ( Color       ) & 0xff ) ;

		return 0 ;
	}
	else
	{
		ColorData = GetMemImgColorData( GSYS.Screen.MainScreenColorBitDepth == 16 ? 0 : 1, FALSE, FALSE ) ;
	}

	MaxRed		= ( 1 << ColorData->RedWidth   ) - 1 ; 
	MaxGreen	= ( 1 << ColorData->GreenWidth ) - 1 ;
	MaxBlue		= ( 1 << ColorData->BlueWidth  ) - 1 ; 

	// 色情報を格納する
	*Red	= ( int )( ( ( Color & ColorData->RedMask   ) >> ColorData->RedLoc   ) * 255 / MaxRed   ) ;
	*Green	= ( int )( ( ( Color & ColorData->GreenMask ) >> ColorData->GreenLoc ) * 255 / MaxGreen ) ;
	*Blue	= ( int )( ( ( Color & ColorData->BlueMask  ) >> ColorData->BlueLoc  ) * 255 / MaxBlue  ) ;

	// 終了
	return 0 ;
#else // DX_NON_GRAPHICS
	return -1 ;
#endif // DX_NON_GRAPHICS
}

// 指定のピクセルフォーマットに対応したカラー値を得る
extern unsigned int NS_GetColor3( const COLORDATA * ColorData, int Red, int Green, int Blue, int Alpha )
{
	// 8ビットカラーの場合はパレットから一番近い色を探す
	if( ColorData->PixelByte == 1 )
	{
		int RedData, BlueData, GreenData ;
		unsigned int ColorCode ;
		unsigned int MinColor = 0xffffff;
		int MinIndex = 0 ;
		int i ;
		const COLORPALETTEDATA *PalData ;

		// 一番近い色を返す
		PalData = ColorData->Palette ;
		for( i = 0 ; i < 256 ; i ++, PalData ++ )
		{
			RedData   = ( int )PalData->Red   - Red ;
			GreenData = ( int )PalData->Green - Green ;
			BlueData  = ( int )PalData->Blue  - Blue ;

			ColorCode = ( unsigned int )( RedData * RedData + GreenData * GreenData + BlueData * BlueData ) ;
			if( ColorCode < MinColor ) 
			{
				MinIndex = i ;
				MinColor = ColorCode ;
			}
		}
		
		return ( unsigned int )MinIndex ;
	}
	else
	{
		// 色情報を返す
		return ColorData->NoneMask +
				( ( ( BYTE )Red   >> ( 8 - ColorData->RedWidth   ) ) << ColorData->RedLoc   ) +
				( ( ( BYTE )Green >> ( 8 - ColorData->GreenWidth ) ) << ColorData->GreenLoc ) +
				( ( ( BYTE )Blue  >> ( 8 - ColorData->BlueWidth  ) ) << ColorData->BlueLoc  ) +
				( ColorData->AlphaWidth != 0 ? ( ( ( BYTE )Alpha >> ( 8 - ColorData->AlphaWidth ) ) << ColorData->AlphaLoc ) : 0 ) ;
	}
}

// 指定のカラーフォーマットのカラー値を別のカラーフォーマットのカラー値に変換する
extern unsigned int NS_GetColor4( const COLORDATA * DestColorData, const COLORDATA * SrcColorData, unsigned int SrcColor )
{
	unsigned int Red, Blue, Green, Alpha ;
	unsigned int i ;

	// フォーマットが同じ場合は何もせずに返す
	if( SrcColorData->ColorBitDepth == DestColorData->ColorBitDepth &&
		SrcColorData->RedMask       == DestColorData->RedMask       &&
		SrcColorData->GreenMask     == DestColorData->GreenMask     &&
		SrcColorData->BlueMask      == DestColorData->BlueMask      &&
		SrcColorData->AlphaMask     == DestColorData->AlphaMask )
	{
		return DestColorData->NoneMask | SrcColor ;
	}
	else
	{
		Red = ( SrcColor & SrcColorData->RedMask ) >> SrcColorData->RedLoc ;
		i = ( DWORD )( SrcColorData->RedWidth - DestColorData->RedWidth ) ;
		if( i != 0 )
		{
			if( i & 0x80000000 )
			{
				Red <<= ~i + 1 ;
			}
			else
			{
				Red >>= i ;
			}
		}

		Green = ( SrcColor & SrcColorData->GreenMask ) >> SrcColorData->GreenLoc ;
		i = ( DWORD )( SrcColorData->GreenWidth - DestColorData->GreenWidth ) ;
		if( i != 0 )
		{
			if( i & 0x80000000 )
			{
				Green <<= ~i + 1 ;
			}
			else
			{
				Green >>= i ;
			}
		}

		Blue = ( SrcColor & SrcColorData->BlueMask ) >> SrcColorData->BlueLoc ;
		i = ( DWORD )( SrcColorData->BlueWidth - DestColorData->BlueWidth ) ;
		if( i != 0 )
		{
			if( i & 0x80000000 )
			{
				Blue <<= ~i + 1 ;
			}
			else
			{
				Blue >>= i ;
			}
		}

		Alpha = ( SrcColor & SrcColorData->AlphaMask ) >> SrcColorData->AlphaLoc ;
		i = ( DWORD )( SrcColorData->AlphaWidth - DestColorData->AlphaWidth ) ;
		if( i != 0 )
		{
			if( i & 0x80000000 )
			{
				Alpha <<= ~i + 1 ;
			}
			else
			{
				Alpha >>= i ;
			}
		}

		if( SrcColorData->AlphaWidth == 0 )
		{
			Alpha = DestColorData->AlphaMask >> DestColorData->AlphaLoc ;
		}

		// 値を返す
		return DestColorData->NoneMask |
					( Red   << DestColorData->RedLoc ) |
					( Blue  << DestColorData->BlueLoc ) |
					( Green << DestColorData->GreenLoc ) |
					( Alpha << DestColorData->AlphaLoc ) ;
	}
}

// 指定のカラーフォーマットのカラー値を赤、緑、青、アルファの値を取得する
extern int NS_GetColor5( const COLORDATA * ColorData, unsigned int Color, int *Red, int *Green, int *Blue, int *Alpha )
{
	DWORD MaxRed, MaxGreen, MaxBlue, MaxAlpha ;

	// 各ビットが８ビットだったら簡略処理
	if( ColorData->RedWidth   == 8 &&
		ColorData->GreenWidth == 8 &&
		ColorData->BlueWidth  == 8 )
	{
		// 色情報を格納する
		if( Red   != NULL )	*Red	= ( int )( ( Color & ColorData->RedMask   ) >> ColorData->RedLoc   ) ;
		if( Green != NULL )	*Green	= ( int )( ( Color & ColorData->GreenMask ) >> ColorData->GreenLoc ) ;
		if( Blue  != NULL )	*Blue	= ( int )( ( Color & ColorData->BlueMask  ) >> ColorData->BlueLoc  ) ;

		if( ColorData->AlphaWidth != 0 )
		{
			if( ColorData->AlphaWidth == 8 )
			{
				if( Alpha != NULL )	*Alpha = ( int )( ( Color & ColorData->AlphaMask ) >> ColorData->AlphaLoc ) ;
			}
			else
			{
				MaxAlpha = ( DWORD )( ( 1 << ColorData->AlphaWidth	) - 1 ) ;
				if( Alpha != NULL ) *Alpha = ( int )( ColorData->AlphaWidth != 0 ? ( ( Color & ColorData->AlphaMask	) >> ColorData->AlphaLoc ) * 255 / MaxAlpha : 0 ) ;
			}
		}
	}
	else
	{
		// ８ビット以上だった場合の処理
		MaxRed		= ( DWORD )( ( 1 << ColorData->RedWidth		) - 1 ) ;
		MaxGreen	= ( DWORD )( ( 1 << ColorData->GreenWidth	) - 1 ) ;
		MaxBlue		= ( DWORD )( ( 1 << ColorData->BlueWidth	) - 1 ) ;
		MaxAlpha	= ( DWORD )( ( 1 << ColorData->AlphaWidth	) - 1 ) ;

		// 色情報を格納する
		if( Red   != NULL )	*Red	= ( int )( ( ( Color & ColorData->RedMask	) >> ColorData->RedLoc		) * 255 / MaxRed   ) ;
		if( Green != NULL )	*Green	= ( int )( ( ( Color & ColorData->GreenMask	) >> ColorData->GreenLoc	) * 255 / MaxGreen ) ;
		if( Blue  != NULL )	*Blue	= ( int )( ( ( Color & ColorData->BlueMask	) >> ColorData->BlueLoc		) * 255 / MaxBlue  ) ;
		if( Alpha != NULL ) *Alpha	= ( int )( ColorData->AlphaWidth != 0 ? ( ( Color & ColorData->AlphaMask	) >> ColorData->AlphaLoc ) * 255 / MaxAlpha : 0 ) ;
	}

	// 終了
	return 0 ;
}

// パレットカラーのカラー情報を構築する
extern int NS_CreatePaletteColorData( COLORDATA * ColorDataBuf )
{
	ColorDataBuf->Format = DX_BASEIMAGE_FORMAT_NORMAL ;
	ColorDataBuf->ChannelNum = 0 ;
	ColorDataBuf->ChannelBitDepth = 0 ;
	ColorDataBuf->FloatTypeFlag = FALSE ;
	ColorDataBuf->ColorBitDepth = 8 ;
	ColorDataBuf->PixelByte = 1 ;
	ColorDataBuf->MaxPaletteNo = 255 ;

	ColorDataBuf->NoneMask = 0xff000000 ;

	ColorDataBuf->AlphaLoc = 0 ;
	ColorDataBuf->AlphaMask = 0 ;
	ColorDataBuf->AlphaWidth = 0 ;

	ColorDataBuf->RedLoc = 16 ;
	ColorDataBuf->RedMask = 0x00ff0000 ;
	ColorDataBuf->RedWidth = 8 ;

	ColorDataBuf->GreenLoc = 8 ;
	ColorDataBuf->GreenMask = 0x0000ff00 ;
	ColorDataBuf->GreenWidth = 8 ;

	ColorDataBuf->BlueLoc = 0  ;
	ColorDataBuf->BlueMask = 0x000000ff ;
	ColorDataBuf->BlueWidth = 8 ;

	// 終了
	return 0 ;
}

// ＡＲＧＢ各チャンネル 32bit 浮動小数点型カラーのカラーフォーマットを構築する
extern int NS_CreateARGBF32ColorData( COLORDATA *ColorDataBuf )
{
	_MEMSET( ColorDataBuf, 0, sizeof( COLORDATA ) ) ;

	// データのセット
	ColorDataBuf->Format = DX_BASEIMAGE_FORMAT_NORMAL ;

	ColorDataBuf->ChannelBitDepth = ( unsigned char )32 ;
	ColorDataBuf->ChannelNum      = ( unsigned char )4 ;
	ColorDataBuf->FloatTypeFlag   = ( unsigned char )TRUE ;
	ColorDataBuf->PixelByte       = ( unsigned char )( ColorDataBuf->ChannelBitDepth * ColorDataBuf->ChannelNum / 8 ) ;

	// 終了
	return 0 ;
}

// ＡＲＧＢ各チャンネル 16bit 浮動小数点型カラーのカラーフォーマットを構築する
extern int NS_CreateARGBF16ColorData( COLORDATA *ColorDataBuf )
{
	_MEMSET( ColorDataBuf, 0, sizeof( COLORDATA ) ) ;

	// データのセット
	ColorDataBuf->Format = DX_BASEIMAGE_FORMAT_NORMAL ;

	ColorDataBuf->ChannelBitDepth = ( unsigned char )16 ;
	ColorDataBuf->ChannelNum      = ( unsigned char )4 ;
	ColorDataBuf->FloatTypeFlag   = ( unsigned char )TRUE ;
	ColorDataBuf->PixelByte       = ( unsigned char )( ColorDataBuf->ChannelBitDepth * ColorDataBuf->ChannelNum / 8 ) ;

	// 終了
	return 0 ;
}

// ＸＲＧＢ８カラーのカラー情報を構築する
extern	int NS_CreateXRGB8ColorData( COLORDATA * ColorDataBuf )
{
	ColorDataBuf->Format = DX_BASEIMAGE_FORMAT_NORMAL ;
	ColorDataBuf->ChannelNum = 0 ;
	ColorDataBuf->ChannelBitDepth = 0 ;
	ColorDataBuf->FloatTypeFlag = FALSE ;
	ColorDataBuf->ColorBitDepth = 32 ;
	ColorDataBuf->PixelByte = 4 ;
	ColorDataBuf->MaxPaletteNo = 0 ;

	ColorDataBuf->NoneLoc = 24 ;
	ColorDataBuf->NoneMask = 0xff000000 ;
	ColorDataBuf->NoneWidth = 8 ;

	ColorDataBuf->AlphaLoc = 0 ;
	ColorDataBuf->AlphaMask = 0 ;
	ColorDataBuf->AlphaWidth = 0 ;

	ColorDataBuf->RedLoc = 16 ;
	ColorDataBuf->RedMask = 0x00ff0000 ;
	ColorDataBuf->RedWidth = 8 ;

	ColorDataBuf->GreenLoc = 8 ;
	ColorDataBuf->GreenMask = 0x0000ff00 ;
	ColorDataBuf->GreenWidth = 8 ;

	ColorDataBuf->BlueLoc = 0  ;
	ColorDataBuf->BlueMask = 0x000000ff ;
	ColorDataBuf->BlueWidth = 8 ;

	// 終了
	return 0 ;
}

// ＡＲＧＢ８カラーのカラー情報を構築する
extern	int NS_CreateARGB8ColorData( COLORDATA * ColorDataBuf )
{
	ColorDataBuf->Format = DX_BASEIMAGE_FORMAT_NORMAL ;
	ColorDataBuf->ChannelNum = 0 ;
	ColorDataBuf->ChannelBitDepth = 0 ;
	ColorDataBuf->FloatTypeFlag = FALSE ;
	ColorDataBuf->ColorBitDepth = 32 ;
	ColorDataBuf->PixelByte = 4 ;
	ColorDataBuf->MaxPaletteNo = 0 ;

	ColorDataBuf->NoneLoc = 0 ;
	ColorDataBuf->NoneMask = 0x00000000 ;
	ColorDataBuf->NoneWidth = 0 ;

	ColorDataBuf->AlphaLoc = 24 ;
	ColorDataBuf->AlphaMask = 0xff000000 ;
	ColorDataBuf->AlphaWidth = 8 ;

	ColorDataBuf->RedLoc = 16 ;
	ColorDataBuf->RedMask = 0x00ff0000 ;
	ColorDataBuf->RedWidth = 8 ;

	ColorDataBuf->GreenLoc = 8 ;
	ColorDataBuf->GreenMask = 0x0000ff00 ;
	ColorDataBuf->GreenWidth = 8 ;

	ColorDataBuf->BlueLoc = 0  ;
	ColorDataBuf->BlueMask = 0x000000ff ;
	ColorDataBuf->BlueWidth = 8 ;

	// 終了
	return 0 ;
}

// ＡＲＧＢ４カラーのカラー情報を構築する
extern	int NS_CreateARGB4ColorData( COLORDATA * ColorDataBuf )
{
	ColorDataBuf->Format = DX_BASEIMAGE_FORMAT_NORMAL ;
	ColorDataBuf->ChannelNum = 0 ;
	ColorDataBuf->ChannelBitDepth = 0 ;
	ColorDataBuf->FloatTypeFlag = FALSE ;
	ColorDataBuf->ColorBitDepth = 16 ;
	ColorDataBuf->PixelByte = 2 ;
	ColorDataBuf->MaxPaletteNo = 0 ;

	ColorDataBuf->NoneMask = 0x00000000 ;

	ColorDataBuf->AlphaLoc = 12 ;
	ColorDataBuf->AlphaMask = 0x0000f000 ;
	ColorDataBuf->AlphaWidth = 4 ;

	ColorDataBuf->RedLoc = 8 ;
	ColorDataBuf->RedMask = 0x00000f00 ;
	ColorDataBuf->RedWidth = 4 ;

	ColorDataBuf->GreenLoc = 4 ;
	ColorDataBuf->GreenMask = 0x000000f0 ;
	ColorDataBuf->GreenWidth = 4 ;

	ColorDataBuf->BlueLoc = 0  ;
	ColorDataBuf->BlueMask = 0x0000000f ;
	ColorDataBuf->BlueWidth = 4 ;

	// 終了
	return 0 ;
}

// Ａ１Ｒ５Ｇ５Ｂ５カラーのカラーフォーマットを構築する
extern int NS_CreateA1R5G5B5ColorData( COLORDATA *ColorDataBuf )
{
	ColorDataBuf->Format = DX_BASEIMAGE_FORMAT_NORMAL ;
	ColorDataBuf->ChannelNum = 0 ;
	ColorDataBuf->ChannelBitDepth = 0 ;
	ColorDataBuf->FloatTypeFlag = FALSE ;
	ColorDataBuf->ColorBitDepth = 16 ;
	ColorDataBuf->PixelByte = 2 ;
	ColorDataBuf->MaxPaletteNo = 0 ;

	ColorDataBuf->NoneMask = 0x00000000 ;

	ColorDataBuf->AlphaLoc = 15 ;
	ColorDataBuf->AlphaMask = 0x00008000 ;
	ColorDataBuf->AlphaWidth = 1 ;

	ColorDataBuf->RedLoc = 10 ;
	ColorDataBuf->RedMask = 0x00007c00 ;
	ColorDataBuf->RedWidth = 5 ;

	ColorDataBuf->GreenLoc = 5 ;
	ColorDataBuf->GreenMask = 0x000003e0 ;
	ColorDataBuf->GreenWidth = 5 ;

	ColorDataBuf->BlueLoc = 0 ;
	ColorDataBuf->BlueMask = 0x0000001f ;
	ColorDataBuf->BlueWidth = 5 ;

	// 終了
	return 0 ;
}

// Ｘ１Ｒ５Ｇ５Ｂ５カラーのカラーフォーマットを構築する
extern int NS_CreateX1R5G5B5ColorData( COLORDATA *ColorDataBuf )
{
	ColorDataBuf->Format = DX_BASEIMAGE_FORMAT_NORMAL ;
	ColorDataBuf->ChannelNum = 0 ;
	ColorDataBuf->ChannelBitDepth = 0 ;
	ColorDataBuf->FloatTypeFlag = FALSE ;
	ColorDataBuf->ColorBitDepth = 16 ;
	ColorDataBuf->PixelByte = 2 ;
	ColorDataBuf->MaxPaletteNo = 0 ;

	ColorDataBuf->NoneMask = 0x00008000 ;

	ColorDataBuf->AlphaLoc = 0 ;
	ColorDataBuf->AlphaMask = 0x00000000 ;
	ColorDataBuf->AlphaWidth = 0 ;

	ColorDataBuf->RedLoc = 10 ;
	ColorDataBuf->RedMask = 0x00007c00 ;
	ColorDataBuf->RedWidth = 5 ;

	ColorDataBuf->GreenLoc = 5 ;
	ColorDataBuf->GreenMask = 0x000003e0 ;
	ColorDataBuf->GreenWidth = 5 ;

	ColorDataBuf->BlueLoc = 0 ;
	ColorDataBuf->BlueMask = 0x0000001f ;
	ColorDataBuf->BlueWidth = 5 ;

	// 終了
	return 0 ;
}

// Ｒ５Ｇ５Ｂ５Ａ１カラーのカラーフォーマットを構築する
extern int NS_CreateR5G5B5A1ColorData( COLORDATA *ColorDataBuf )
{
	ColorDataBuf->Format = DX_BASEIMAGE_FORMAT_NORMAL ;
	ColorDataBuf->ChannelNum = 0 ;
	ColorDataBuf->ChannelBitDepth = 0 ;
	ColorDataBuf->FloatTypeFlag = FALSE ;
	ColorDataBuf->ColorBitDepth = 16 ;
	ColorDataBuf->PixelByte = 2 ;
	ColorDataBuf->MaxPaletteNo = 0 ;

	ColorDataBuf->NoneMask = 0x00000000 ;

	ColorDataBuf->AlphaLoc = 0 ;
	ColorDataBuf->AlphaMask = 0x00000001 ;
	ColorDataBuf->AlphaWidth = 1 ;

	ColorDataBuf->RedLoc = 11 ;
	ColorDataBuf->RedMask = 0x0000f800 ;
	ColorDataBuf->RedWidth = 5 ;

	ColorDataBuf->GreenLoc = 6 ;
	ColorDataBuf->GreenMask = 0x000007c0 ;
	ColorDataBuf->GreenWidth = 5 ;

	ColorDataBuf->BlueLoc = 1 ;
	ColorDataBuf->BlueMask = 0x0000003e ;
	ColorDataBuf->BlueWidth = 5 ;

	// 終了
	return 0 ;
}

// Ｒ５Ｇ６Ｂ５カラーのカラーフォーマットを構築する
extern int NS_CreateR5G6B5ColorData( COLORDATA *ColorDataBuf )
{
	ColorDataBuf->Format = DX_BASEIMAGE_FORMAT_NORMAL ;
	ColorDataBuf->ChannelNum = 0 ;
	ColorDataBuf->ChannelBitDepth = 0 ;
	ColorDataBuf->FloatTypeFlag = FALSE ;
	ColorDataBuf->ColorBitDepth = 16 ;
	ColorDataBuf->PixelByte = 2 ;
	ColorDataBuf->MaxPaletteNo = 0 ;

	ColorDataBuf->NoneMask = 0x00000000 ;

	ColorDataBuf->AlphaLoc = 0 ;
	ColorDataBuf->AlphaMask = 0x00000000 ;
	ColorDataBuf->AlphaWidth = 0 ;

	ColorDataBuf->RedLoc = 11 ;
	ColorDataBuf->RedMask = 0x0000f800 ;
	ColorDataBuf->RedWidth = 5 ;

	ColorDataBuf->GreenLoc = 5 ;
	ColorDataBuf->GreenMask = 0x000007e0 ;
	ColorDataBuf->GreenWidth = 6 ;

	ColorDataBuf->BlueLoc = 0 ;
	ColorDataBuf->BlueMask = 0x0000001f ;
	ColorDataBuf->BlueWidth = 5 ;

	// 終了
	return 0 ;
}

// フルカラーＤＩＢのカラー情報を構築する
extern	int	NS_CreateFullColorData( COLORDATA * ColorDataBuf )
{
	ColorDataBuf->Format = DX_BASEIMAGE_FORMAT_NORMAL ;
	ColorDataBuf->ChannelNum = 0 ;
	ColorDataBuf->ChannelBitDepth = 0 ;
	ColorDataBuf->FloatTypeFlag = FALSE ;
	ColorDataBuf->ColorBitDepth = 24 ;
	ColorDataBuf->PixelByte = 3 ;
	ColorDataBuf->MaxPaletteNo = 0 ;

	ColorDataBuf->NoneMask = 0x00000000 ;

	ColorDataBuf->AlphaLoc = 0 ;
	ColorDataBuf->AlphaMask = 0 ;
	ColorDataBuf->AlphaWidth = 0 ;

	ColorDataBuf->RedLoc = 16 ;
	ColorDataBuf->RedMask = 0xff0000 ;
	ColorDataBuf->RedWidth = 8 ;

	ColorDataBuf->GreenLoc = 8 ;
	ColorDataBuf->GreenMask = 0x00ff00 ;
	ColorDataBuf->GreenWidth = 8 ;

	ColorDataBuf->BlueLoc = 0  ;
	ColorDataBuf->BlueMask = 0x0000ff ;
	ColorDataBuf->BlueWidth = 8 ;

	// 終了
	return 0 ;
}

// グレースケールのカラー情報を構築する
extern int NS_CreateGrayColorData( COLORDATA * ColorDataBuf )
{
	int i ;

	ColorDataBuf->Format = DX_BASEIMAGE_FORMAT_NORMAL ;
	ColorDataBuf->ChannelNum = 0 ;
	ColorDataBuf->ChannelBitDepth = 0 ;
	ColorDataBuf->FloatTypeFlag = FALSE ;
	ColorDataBuf->ColorBitDepth = 8 ;
	ColorDataBuf->PixelByte = 1 ;
	ColorDataBuf->MaxPaletteNo = 255 ;

	ColorDataBuf->NoneMask = 0xff000000 ;

	ColorDataBuf->AlphaLoc = 0 ;
	ColorDataBuf->AlphaMask = 0 ;
	ColorDataBuf->AlphaWidth = 0 ;

	ColorDataBuf->RedLoc = 16 ;
	ColorDataBuf->RedMask = 0x00ff0000 ;
	ColorDataBuf->RedWidth = 8 ;

	ColorDataBuf->GreenLoc = 8 ;
	ColorDataBuf->GreenMask = 0x0000ff00 ;
	ColorDataBuf->GreenWidth = 8 ;

	ColorDataBuf->BlueLoc = 0  ;
	ColorDataBuf->BlueMask = 0x000000ff ;
	ColorDataBuf->BlueWidth = 8 ;

	for( i = 0 ; i < 256 ; i ++ )
	{
		ColorDataBuf->Palette[i].Red = ( unsigned char )i ;
		ColorDataBuf->Palette[i].Green = ( unsigned char )i ;
		ColorDataBuf->Palette[i].Blue = ( unsigned char )i ;
		ColorDataBuf->Palette[i].Alpha = 0 ;
	}

	// 終了
	return 0 ;
}

// パレット２５６色のカラー情報を構築する
extern int NS_CreatePal8ColorData( COLORDATA * ColorDataBuf, int UseAlpha )
{
	int i ;

	ColorDataBuf->Format = DX_BASEIMAGE_FORMAT_NORMAL ;
	ColorDataBuf->ChannelNum = 0 ;
	ColorDataBuf->ChannelBitDepth = 0 ;
	ColorDataBuf->FloatTypeFlag = FALSE ;
	ColorDataBuf->ColorBitDepth = 8 ;
	ColorDataBuf->PixelByte = 1 ;
	ColorDataBuf->MaxPaletteNo = 255 ;

	if( UseAlpha == FALSE )
	{
		ColorDataBuf->NoneMask = 0xff000000 ;

		ColorDataBuf->AlphaLoc = 0 ;
		ColorDataBuf->AlphaMask = 0 ;
		ColorDataBuf->AlphaWidth = 0 ;
	}
	else
	{
		ColorDataBuf->NoneMask = 0x00000000 ;

		ColorDataBuf->AlphaLoc = 24 ;
		ColorDataBuf->AlphaMask = 0xff000000 ;
		ColorDataBuf->AlphaWidth = 8 ;
	}

	ColorDataBuf->RedLoc = 16 ;
	ColorDataBuf->RedMask = 0x00ff0000 ;
	ColorDataBuf->RedWidth = 8 ;

	ColorDataBuf->GreenLoc = 8 ;
	ColorDataBuf->GreenMask = 0x0000ff00 ;
	ColorDataBuf->GreenWidth = 8 ;

	ColorDataBuf->BlueLoc = 0  ;
	ColorDataBuf->BlueMask = 0x000000ff ;
	ColorDataBuf->BlueWidth = 8 ;

	for( i = 0 ; i < 256 ; i ++ )
	{
		ColorDataBuf->Palette[i].Red   = ( BYTE )( ( ( i >> 0 ) & 7 ) * 255 / 7 ) ;
		ColorDataBuf->Palette[i].Green = ( BYTE )( ( ( i >> 3 ) & 7 ) * 255 / 7 ) ;
		ColorDataBuf->Palette[i].Blue  = ( BYTE )( ( ( i >> 5 ) & 3 ) * 255 / 3 ) ;
		ColorDataBuf->Palette[i].Alpha = 0 ;
	}

	// 終了
	return 0 ;
}

// カラーデータを作成する	
extern int NS_CreateColorData( COLORDATA * ColorDataBuf, int ColorBitDepth,
							 DWORD RedMask, DWORD GreenMask, DWORD BlueMask, DWORD AlphaMask,
							 int ChannelNum, int ChannelBitDepth, int FloatTypeFlag )
{
	int i, j ;
	DWORD NoneMask;

	_MEMSET( ColorDataBuf, 0, sizeof( COLORDATA ) ) ;

	// データのセット
	ColorDataBuf->Format = DX_BASEIMAGE_FORMAT_NORMAL ;

	if( ChannelNum != 0 && ChannelBitDepth != 0 )
	{
		ColorDataBuf->ChannelBitDepth = ( unsigned char )ChannelBitDepth ;
		ColorDataBuf->ChannelNum      = ( unsigned char )ChannelNum ;
		ColorDataBuf->FloatTypeFlag   = ( unsigned char )FloatTypeFlag ;
		ColorDataBuf->PixelByte       = ( unsigned char )( ChannelBitDepth * ChannelNum / 8 ) ;
	}
	else
	{
		ColorDataBuf->ChannelBitDepth = 0 ;
		ColorDataBuf->ChannelNum      = 0 ;
		ColorDataBuf->FloatTypeFlag   = FALSE ;
		ColorDataBuf->ColorBitDepth   = ( unsigned char )ColorBitDepth ;
		ColorDataBuf->PixelByte       = ( unsigned char )( ColorBitDepth / 8 ) ;
		ColorDataBuf->RedMask         = RedMask ;
		ColorDataBuf->GreenMask       = GreenMask ;
		ColorDataBuf->BlueMask        = BlueMask ;
		ColorDataBuf->AlphaMask       = AlphaMask ;
		if( ColorBitDepth == 32 ) ColorDataBuf->NoneMask = 0xffffffff;
		else                      ColorDataBuf->NoneMask = ( DWORD )( ( 1 << ColorBitDepth ) - 1 ) ;
		ColorDataBuf->NoneMask     &= ~( RedMask | GreenMask | BlueMask | AlphaMask );
		NoneMask = ColorDataBuf->NoneMask;
		
		// 赤色マスクの解析
		if( RedMask == 0 )
		{
			ColorDataBuf->RedLoc = ColorDataBuf->RedWidth = 0 ;
			ColorDataBuf->RedMask = 0 ;
		}
		else
		{
			j = 1 ;
			for( i = 0 ; ( RedMask & j ) == 0 ; j <<= 1, i ++ ){} ColorDataBuf->RedLoc   = ( unsigned char )i ;
			for( i = 0 ; ( RedMask & j ) != 0 ; j <<= 1, i ++ ){} ColorDataBuf->RedWidth = ( unsigned char )i ;
		}

		// 緑色マスクの解析
		if( GreenMask == 0 )
		{
			ColorDataBuf->GreenLoc = ColorDataBuf->GreenWidth = 0 ;
			ColorDataBuf->GreenMask = 0 ;
		}
		else
		{
			j = 1 ;
			for( i = 0 ; ( GreenMask & j ) == 0 ; j <<= 1, i ++ ){} ColorDataBuf->GreenLoc   = ( unsigned char )i ;
			for( i = 0 ; ( GreenMask & j ) != 0 ; j <<= 1, i ++ ){} ColorDataBuf->GreenWidth = ( unsigned char )i ;
		}

		// 青色マスクの解析
		if( BlueMask == 0 )
		{
			ColorDataBuf->BlueLoc = ColorDataBuf->BlueWidth = 0 ;
			ColorDataBuf->BlueMask = 0 ;
		}
		else
		{
			j = 1 ;
			for( i = 0 ; ( BlueMask & j ) == 0 ; j <<= 1, i ++ ){} ColorDataBuf->BlueLoc   = ( unsigned char )i ;
			for( i = 0 ; ( BlueMask & j ) != 0 ; j <<= 1, i ++ ){} ColorDataBuf->BlueWidth = ( unsigned char )i ;
		}

		// アルファマスクの解析
		if( AlphaMask == 0 )
		{
			ColorDataBuf->AlphaLoc = ColorDataBuf->AlphaWidth = 0 ;
			ColorDataBuf->AlphaMask = 0 ;
		}
		else
		{
			j = 1 ;
			for( i = 0 ; ( AlphaMask & j ) == 0 ; j <<= 1, i ++ ){} ColorDataBuf->AlphaLoc   = ( unsigned char )i ;
			for( i = 0 ; ( AlphaMask & j ) != 0 ; j <<= 1, i ++ ){} ColorDataBuf->AlphaWidth = ( unsigned char )i ;
		}

		// 使われていないマスクの解析
		if( NoneMask == 0 )
		{
			ColorDataBuf->NoneLoc = ColorDataBuf->NoneWidth = 0 ;
			ColorDataBuf->NoneMask = 0 ;
		}
		else
		{
			j = 1 ;
			for( i = 0 ; ( NoneMask & j ) == 0 ; j <<= 1, i ++ ){} ColorDataBuf->NoneLoc   = ( unsigned char )i ;
			for( i = 0 ; ( NoneMask & j ) != 0 ; j <<= 1, i ++ ){} ColorDataBuf->NoneWidth = ( unsigned char )i ;
		}

		// NoneMask のセット
//		NS_SetColorDataNoneMask( ColorDataBuf ) ;
	}
	
	// 終了
	return 0 ;
}

// NoneMask 以外の要素を埋めた COLORDATA 構造体の情報を元に NoneMask をセットする
extern void NS_SetColorDataNoneMask( COLORDATA * ColorData )
{
	if( ColorData->AlphaWidth + ColorData->RedWidth + ColorData->GreenWidth + ColorData->BlueWidth != ColorData->ColorBitDepth )
	{
		int i ;

		ColorData->NoneMask = 0 ;
		for( i = 0 ; i < ColorData->ColorBitDepth ; i ++ )
		{
			ColorData->NoneMask <<= 1 ;
			ColorData->NoneMask |= 1 ;
		}
		ColorData->NoneMask &= ~ColorData->RedMask & ~ColorData->GreenMask & ~ColorData->BlueMask & ~ColorData->AlphaMask ;
	}
	else
	{
		ColorData->NoneMask = 0 ;
	}
}

// 二つのカラーデータが等しいかどうか調べる( TRUE:等しい  FALSE:等しくない )
extern int NS_CmpColorData( const COLORDATA * ColorData1, const COLORDATA * ColorData2 )
{
	if( ColorData1->Format != ColorData2->Format ) return FALSE ;

	if( ColorData1->ChannelNum != 0 || ColorData2->ChannelNum != 0 )
	{
		return	ColorData1->ChannelNum      == ColorData2->ChannelNum &&
				ColorData1->ChannelBitDepth == ColorData2->ChannelBitDepth &&
				ColorData1->FloatTypeFlag   == ColorData2->FloatTypeFlag ? TRUE : FALSE ;
	}

	return 	ColorData1->ColorBitDepth 	== ColorData2->ColorBitDepth 	&&
			ColorData1->RedMask 		== ColorData2->RedMask 			&&
			ColorData1->GreenMask 		== ColorData2->GreenMask 		&&
			ColorData1->BlueMask 		== ColorData2->BlueMask 		&&
			ColorData1->AlphaMask 		== ColorData2->AlphaMask ? TRUE : FALSE ;
}

// 高速読み込みルーチンを使うか否かのフラグをセットする
extern int NS_SetUseFastLoadFlag( int Flag )
{
	// フラグをセットする
	BASEIM.LowLoadFlag = Flag == TRUE ? FALSE : TRUE ;

	// 終了
	return 0 ;
}

// グラフィック減色時の画像劣化緩和処理モードの取得
extern int NS_GetGraphDataShavedMode( void )
{
	return BASEIM.ImageShavedMode ;
}

// グラフィック減色時の画像劣化緩和処理モードの変更
extern int NS_SetGraphDataShavedMode( int ShavedMode )
{
	BASEIM.ImageShavedMode = ShavedMode ;

	// 終了
	return 0 ;
}

// 画像ファイル読み込み時に乗算済みアルファ画像に変換するかどうかを設定する( TRUE:変換処理を行う  FALSE:変換処理を行わない( デフォルト ) )
extern int NS_SetUsePremulAlphaConvertLoad( int UseFlag )
{
	BASEIM.ConvertPremultipliedAlpha = UseFlag ;

	// 終了
	return 0 ;
}

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE
