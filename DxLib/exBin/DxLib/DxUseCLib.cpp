// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		標準Ｃライブラリ使用コード
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

// インクルード ------------------------------------------------------------------
#include "DxCompileConfig.h"
#include "DxUseCLib.h"
#include "DxLib.h"
#include "DxStatic.h"
#include "DxFile.h"
#include "DxChar.h"
#include "DxBaseFunc.h"
#include "DxSystem.h"
#include "DxMemory.h"
#include "DxLog.h"

#ifndef DX_NON_MOVIE
#ifndef DX_NON_DSHOW_MP3
#ifndef DX_NON_DSHOW_MOVIE
#include "Windows/DxWinAPI.h"
#include "Windows/DxGuid.h"
#endif // DX_NON_MOVIE
#endif // DX_NON_DSHOW_MP3
#endif // DX_NON_DSHOW_MOVIE

#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>

#ifdef __ANDROID__
#include <wchar.h>
#endif

#ifndef DX_NON_BULLET_PHYSICS
	#include "btBulletDynamicsCommon.h"
#endif

#ifndef DX_NON_TIFFREAD
#include "tiff.h"
#include "tiffio.h"
#endif

#ifndef DX_NON_PNGREAD
	#include "png.h"
//  #include "pngpriv.h"
#endif

#ifndef DX_NON_JPEGREAD
	#ifdef DX_GCC_COMPILE
	typedef unsigned char boolean;
	#endif	
	#ifdef __BCC
	typedef unsigned char boolean;
	#endif	
	extern "C"
	{
	#include "jpeglib.h"
	}
#endif

//namespace DxLib
//{

// 構造体型宣言 ------------------------------------------------------------------

// 関数プロトタイプ宣言 ----------------------------------------------------------

// extern int LoadPngImage( STREAMDATA *Src, BASEIMAGE *BaseImage ) ;
// extern int LoadJpegImage( STREAMDATA *Src, BASEIMAGE *BaseImage ) ;
// extern int NS_SaveBaseImageToPng( const char *pFilePath, BASEIMAGE *BaseImage, int CompressionLevel ) ;
// extern int NS_SaveBaseImageToJpeg( const char *pFilePath, BASEIMAGE *BaseImage, int Quality ) ;
// extern int NS_SRand( int Seed ) ;
// extern int NS_GetRand( int RandMax ) ;

// プログラム --------------------------------------------------------------------








// 汎用データ読み込み処理からのＴＩＦＦ画像読み込みのためのプログラム

#ifndef DX_NON_TIFFREAD

// 汎用データ読み込み処理からの読み込みをするためのデータ型
typedef struct tagTIFFHANDLE
{
	STREAMDATA *Data ;
	LONGLONG DataSize ;
} TIFFHANDLE ;

// 汎用データ読み取り処理からデータを読み込むコールバック関数
tmsize_t TIFFReadProc( thandle_t fd, void *buf, tmsize_t size )
{
	TIFFHANDLE *THnd ;
	tmsize_t Result ;

	THnd = ( TIFFHANDLE * )fd ;

	// サイズの調整
	if( ( tmsize_t )( THnd->DataSize - STTELL( THnd->Data ) ) < size )
	{
		size = ( tmsize_t )( THnd->DataSize - STTELL( THnd->Data ) ) ;
	}
	if( size == 0 )
	{
		return 0 ;
	}

	Result = ( tmsize_t )STREAD( buf, 1, ( size_t )size, THnd->Data ) ;

	return Result ;
}

// 汎用データ読み取り処理からの書き込みには非対応
tmsize_t TIFFWriteProc( thandle_t /* fd */, void * /* buf */, tmsize_t size )
{
	return size ;
}

// 汎用データ読み取り処理のシークを行うコールバック関数
toff_t   TIFFSeekProc_     ( thandle_t fd, toff_t offset, int whence )
{
	TIFFHANDLE *THnd ;

	THnd = ( TIFFHANDLE * )fd ;

	switch( whence )
	{
	case SEEK_CUR :
		STSEEK( THnd->Data, ( LONGLONG )offset, SEEK_CUR ) ;
		break ;

	case SEEK_END:
		STSEEK( THnd->Data, ( LONGLONG )offset, SEEK_END ) ;
		break ;

	case SEEK_SET:
	default:
		STSEEK( THnd->Data, ( LONGLONG )offset, SEEK_SET ) ;
		break ;
	}

	return ( toff_t )STTELL( THnd->Data ) ;
}

// 汎用データ読み込み処理のファイルを閉じるコールバック関数
int      TIFFCloseProc_    ( thandle_t /*fd*/ )
{
	// ファイルを閉じる処理はＴＩＦＦ読み込み外で行うので何もしない
	return 0 ;
}


// 汎用データ読み込み処理のデータサイズを取得するコールバック関数
toff_t   TIFFSizeProc_     ( thandle_t fd )
{
	TIFFHANDLE *THnd ;

	THnd = ( TIFFHANDLE * )fd ;

	return ( toff_t )THnd->DataSize ;
}

int      TIFFMapFileProc_  ( thandle_t /*fd*/, void ** /*base*/, toff_t* /*size*/)
{
	return 0 ;
}

void     TIFFUnmapFileProc_( thandle_t /*fd*/, void* /*base*/, toff_t /*size*/)
{
	return ;
}


// ＴＩＦＦ画像の読みこみ
extern int LoadTiffImage( STREAMDATA *Src, BASEIMAGE *BaseImage, int GetFormatOnly )
{
	TIFFHANDLE    THnd ;
	LONGLONG      FilePoint ;
	TIFF         *Conv ;
    uint16        bits_per_sample ;
    uint16        samples_per_pixel ;
    uint16        photometric ;
    uint32        width      = 0 ;
    uint32        height     = 0 ;
    uint32        rows_strip = 0 ;
    uint32       *strip      = NULL ;
    uint32       *src ;
	BYTE          *dest ;
	int           x ;
	int           y ;
	int           dest_y ;
	int           line_no ;
	int           valid_line ;
	uint32        pixel;
	unsigned char checkhead[ 2 ] ;

	// 汎用データ読み取り処理からデータを読み込むための準備
	THnd.Data = Src ;
	FilePoint = STTELL( Src ) ;
	STSEEK( Src, 0, SEEK_END ) ;
	THnd.DataSize = ( LONGLONG )STTELL( Src ) ;
	STSEEK( Src, FilePoint, SEEK_SET ) ;

	// ファイルフォーマットの簡易チェック
	STREAD( checkhead, 2, 1, Src ) ;
	if( ( checkhead[ 0 ] != 0x49 || checkhead[ 1 ] != 0x49 ) &&
		( checkhead[ 0 ] != 0x4d || checkhead[ 1 ] != 0x4d ) )
	{
		return -1 ;
	}
	STSEEK( Src, -2, SEEK_CUR ) ;

	// TIFFファイルのオープン
	Conv = TIFFClientOpen(
		"Read",
		"r",
		&THnd,
		TIFFReadProc,
		TIFFWriteProc,
		TIFFSeekProc_,
		TIFFCloseProc_,
		TIFFSizeProc_,
		TIFFMapFileProc_,
		TIFFUnmapFileProc_
	) ;

	// オープンに失敗したらここで終了
	if( Conv == NULL )
	{
		return -1 ;
	}

	// 画像の情報を取得
	TIFFGetField( Conv, TIFFTAG_IMAGELENGTH,     &height ) ;
	TIFFGetField( Conv, TIFFTAG_IMAGEWIDTH,      &width ) ;
	TIFFGetField( Conv, TIFFTAG_ROWSPERSTRIP,    &rows_strip ) ;
	TIFFGetField( Conv, TIFFTAG_BITSPERSAMPLE,   &bits_per_sample ) ;
	TIFFGetField( Conv, TIFFTAG_SAMPLESPERPIXEL, &samples_per_pixel ) ;
	TIFFGetField( Conv, TIFFTAG_PHOTOMETRIC,     &photometric ) ;

	if( GetFormatOnly == FALSE )
	{
		// １ストリップ分の画像データを格納するメモリ領域の確保
		strip = ( uint32 * )DXALLOC( sizeof( uint32 ) * ( width * rows_strip ) ) ;

		// BASEIMAGE 側の画像データを格納するメモリ領域の確保
		BaseImage->GraphData = DXALLOC( width * height * 4 ) ;
		if( BaseImage->GraphData == NULL )
		{
			goto ERR ;
		}
	}
	else
	{
		BaseImage->GraphData = NULL ;
	}

	// 画像の幅と高さを保存
	BaseImage->Width  = ( int )width ;
	BaseImage->Height = ( int )height ;

	// ピッチの保存
	BaseImage->Pitch  = ( int )width * 4 ;

	// 現状すべての RGBA8 フォーマットとして読み込み
	NS_CreateARGB8ColorData( &BaseImage->ColorData ) ;

	if( GetFormatOnly == FALSE )
	{
		// 画像の読み込み
		for( line_no = 0; line_no < ( int )height; line_no += rows_strip )
		{
			// １ストリップの情報を読み込み
			if( !TIFFReadRGBAStrip( Conv, ( uint32 )line_no, strip ) )
			{
				goto ERR ;
			}

			// 有効なラインを取得
			valid_line = ( int )rows_strip ;
			if( line_no + rows_strip > height )
			{
				valid_line = ( int )( height - line_no ) ;
			}

			// 画像情報を BASEIMAGE 側の画像格納領域に転送
			for( y = 0; y < valid_line ; y++ )
			{
				// 出力先の画像でのＹ座標を算出
				dest_y = line_no + ( ( valid_line - y ) - 1 ) ;

				// 転送元アドレスの算出
				src    = strip + ( width * y ) ;

				// 転送先アドレスの算出
				dest   = ( BYTE * )BaseImage->GraphData + 4 * width * dest_y ;

				// １ライン分のピクセル情報を BASEIMAGE 側に転送
				for( x = 0; x < ( int )width; x++, dest += 4 )
				{
					pixel = src[ x ] ;
					dest[ 0 ] = ( BYTE )TIFFGetB( pixel ) ;
					dest[ 1 ] = ( BYTE )TIFFGetG( pixel ) ;
					dest[ 2 ] = ( BYTE )TIFFGetR( pixel ) ;
					dest[ 3 ] = ( BYTE )TIFFGetA( pixel ) ;
				}
			}
		}
	}

	// TIFFファイルのクローズ
	TIFFClose( Conv ) ;

	// １ストリップ分の画像データを格納するメモリ領域の解放
	DXFREE( strip );

	// 終了
	return 0 ;

ERR :
	if( BaseImage->GraphData )
	{
		DXFREE( BaseImage->GraphData ) ;
		BaseImage->GraphData = NULL ;
	}

	if( strip )
	{
		DXFREE( strip ) ;
		strip = NULL ;
	}

	TIFFClose( Conv ) ;
	Conv = NULL ;

	// エラー終了
	return -1 ;
}

#endif // DX_NON_TIFFREAD










// 汎用データ読み込み処理からのＰＮＧ画像読み込みのためのプログラム

#ifndef DX_NON_PNGREAD

// 汎用データ読み込み処理からの読み込みをするためのデータ型
typedef struct tagPNGGENERAL
{
	STREAMDATA *Data ;
	unsigned int DataSize ;
} PNGGENERAL ;

// 汎用データ読み込み処理からデータを読み込むコールバック関数
static void png_general_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
	PNGGENERAL *PGen ;

//	PGen = (PNGGENERAL *)/*CVT_PTR*/(png_ptr->io_ptr) ;
	PGen = (PNGGENERAL *)png_get_io_ptr(png_ptr) ;

	// 残りのサイズが足りなかったらエラー
	if( PGen->DataSize - ( int )STTELL( PGen->Data ) < length )
	{
		png_error(png_ptr, "Read Error");
	}
	else
	{
		STREAD( data, length, 1, PGen->Data ) ;
	}
}

// 汎用データ読み込み処理からの読み込みを設定する関数
int png_general_read_set( png_structp png_ptr, PNGGENERAL *PGen, STREAMDATA *Data )
{
	PGen->Data = Data ;

	// 現在のファイルポインタから終端までのサイズを取得する
	{
		long pos ;
		pos = ( long )STTELL( PGen->Data ) ;
		STSEEK( PGen->Data, 0, STREAM_SEEKTYPE_END ) ;
		PGen->DataSize = ( unsigned int )STTELL( PGen->Data ) - pos ;
		STSEEK( PGen->Data, pos, STREAM_SEEKTYPE_SET ) ;
	}

	// コールバック関数のセット
	png_set_read_fn( png_ptr, PGen, png_general_read_data ) ;

	// 終了
	return 0 ;
}

// ＰＮＧ画像の読みこみ
extern int LoadPngImage( STREAMDATA *Src, BASEIMAGE *BaseImage, int GetFormatOnly )
{
	png_bytep *row_pointers;
	size_t row, rowbytes ;
	BYTE *GData ;
	PNGGENERAL PGen ;
	png_structp png_ptr;
	png_infop info_ptr;
	unsigned int sig_read = 0;
	png_uint_32 width, height;
	int bit_depth, color_type, interlace_type;
	void *ImageData = NULL ;
//	int i ;
	png_bytep BufPoint ;
	BYTE Check[ 8 ] ;

	// 最初の８バイトでPNGファイルかどうかをチェック
	STREAD( Check, 8, 1, Src ) ;
	STSEEK( Src, 0, STREAM_SEEKTYPE_SET ) ;
	if( Check[ 0 ] != 0x89 || Check[ 1 ] != 0x50 || Check[ 2 ] != 0x4E || Check[ 3 ] != 0x47 ||
		Check[ 4 ] != 0x0D || Check[ 5 ] != 0x0A || Check[ 6 ] != 0x1A || Check[ 7 ] != 0x0A )
	{
		return -1 ;
	}

	// ＰＮＧ管理情報の作成
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL ) ;
	if( png_ptr == NULL ) return -1 ;

	// 画像情報構造体の作成
	if( ( info_ptr = png_create_info_struct( png_ptr ) ) == NULL ) 
	{
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		return -1 ;
	}

	// エラー時の処理セットアップおよび処理
	if( setjmp( png_jmpbuf( png_ptr ) ) )
	{
		png_destroy_read_struct( &png_ptr, &info_ptr, (png_infopp)NULL );
		return -1 ;
	}

	// 汎用データ読み込み処理から読み込む場合の設定を行う
	png_general_read_set( png_ptr, &PGen, Src ) ;

	// 設定処理郡
	png_set_sig_bytes(		png_ptr, ( int )sig_read ) ;										// よくわから無い処理(汗)
	png_read_info(			png_ptr, info_ptr );												// 画像情報を得る
	png_get_IHDR(			png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,		// 画像の基本ステータスを取得する
							&interlace_type, NULL, NULL ) ;
	png_set_strip_16(		png_ptr ) ;															// １６ビットの画像でも８ビットで収納するように設定
//	if( BmpFlag == TRUE )										png_set_strip_alpha( png_ptr ) ;// アルファチャンネル無視
	png_set_packing(		png_ptr ) ;															// １バイト以下のパレット画像をバイト単位で展開するよう指定
	png_set_packswap(		png_ptr ) ;															// よくわから無い処理

//	if( color_type == PNG_COLOR_TYPE_PALETTE )					png_set_expand( png_ptr ) ;		// パレット使用画像データの自動展開指定

	png_set_bgr(			png_ptr ) ;															// カラー配列をＲＧＢからＢＧＲに反転

	// 変更した設定を反映させる
	png_read_update_info(	png_ptr, info_ptr ) ;

	// １ラインあたりに必要なデータ量を得る
	rowbytes = png_get_rowbytes( png_ptr, info_ptr ) ;

	if( GetFormatOnly == FALSE )
	{
		// グラフィックデータを格納するメモリ領域を作成する
		{
			png_bytep BufP ;

			row_pointers = ( png_bytep * )DXALLOC( height * sizeof( png_bytep * ) ) ;
			if( row_pointers == NULL )
			{
				png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
				return -1 ;
			}

			BufPoint = ( png_bytep )png_malloc( png_ptr, rowbytes * height ) ;
			if( BufPoint == NULL )
			{
				DXFREE( row_pointers ) ;
				png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
				return -1 ;
			}

			BufP = BufPoint ;
			for( row = 0; row < height; row++, BufP += rowbytes )
			{
				row_pointers[ row ] = BufP ;
			}
		}

		// 画像データの読み込み
		png_read_image( png_ptr, row_pointers ) ;

		// 実際に使うグラフィックデータ領域の作成・転送
		ImageData = DXALLOC( rowbytes * height ) ;
		if( ImageData == NULL )
		{
			png_free( png_ptr, BufPoint ) ;
			DXFREE( row_pointers ) ;
			png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
			return -1 ;
		}
		GData = ( BYTE * )ImageData ;
		for (row = 0; row < height; row++, GData += rowbytes )
			_MEMCPY( GData, row_pointers[row], rowbytes ) ;

		// メモリの解放
		png_free( png_ptr, BufPoint ) ;
		DXFREE( row_pointers ) ;

		// 読み込み処理の終了
		png_read_end( png_ptr, info_ptr ) ;
	}

	// BASEIMAGE データの情報をセットする
	{
		BaseImage->Width		= ( int )width ;
		BaseImage->Height		= ( int )height ;
		BaseImage->Pitch		= ( int )rowbytes ;
		BaseImage->GraphData	= ImageData ;

		// カラー情報をセットする
		if( color_type == PNG_COLOR_TYPE_PALETTE )
		{
			png_colorp SrcPalette ;
			int PaletteNum ;
			int i ;
			COLORPALETTEDATA *Palette ;
			
			// パレットカラーの場合
			NS_CreatePaletteColorData( &BaseImage->ColorData ) ;

			// パレットを取得
			png_get_PLTE( png_ptr, info_ptr, &SrcPalette, &PaletteNum ) ;

			// パレットの数が２５６以上だった場合は２５６に補正
			if( PaletteNum > 256 ) PaletteNum = 256 ;

			// パレットのコピー
			Palette = BaseImage->ColorData.Palette ;
			for( i = 0 ; i < PaletteNum ; i ++, Palette ++, SrcPalette ++ )
			{
				Palette->Blue  = SrcPalette->blue ;
				Palette->Green = SrcPalette->green ;
				Palette->Red   = SrcPalette->red ;
				Palette->Alpha = 0 ;
			}

			// アルファチャンネルパレットがある場合はデータを読み込む
			if( png_get_valid( png_ptr, info_ptr, PNG_INFO_tRNS ) )
			{
				png_bytep trans ;
				png_color_16p trans_values ;
				int num_trans ;

				png_get_tRNS( png_ptr, info_ptr, &trans, &num_trans, &trans_values ) ;

				Palette = BaseImage->ColorData.Palette ;
				for( i = 0 ; i < num_trans ; i ++ )
				{
					Palette[ i ].Alpha = trans[ i ] ;
				}
				if( num_trans < PaletteNum )
				{
					for( i = num_trans ; i < PaletteNum ; i ++ )
					{
						Palette[ i ].Alpha = 255 ;
					}
				}

				// アルファチャンネルありパレットにする
				BaseImage->ColorData.AlphaLoc   = 24 ;
				BaseImage->ColorData.AlphaMask  = 0xff000000 ;
				BaseImage->ColorData.AlphaWidth = 8 ;
			}
		}
		else
		if( color_type == PNG_COLOR_TYPE_GRAY )
		{
			// 透過色情報がある場合はアルファチャンネルつき画像にする
			if( png_get_valid( png_ptr, info_ptr, PNG_INFO_tRNS ) )
			{
				png_bytep trans ;
				png_color_16p trans_values ;
				int num_trans ;
				int i, j ;

				// 透過色を取得
				png_get_tRNS( png_ptr, info_ptr, &trans, &num_trans, &trans_values ) ;

				// アルファチャンネルありにする
				NS_CreateFullColorData( &BaseImage->ColorData ) ;
				BaseImage->ColorData.ColorBitDepth	= 32 ;
				BaseImage->ColorData.PixelByte		= 4 ;
				BaseImage->ColorData.AlphaLoc		= 24 ;
				BaseImage->ColorData.AlphaWidth		= 8 ;
				BaseImage->ColorData.AlphaMask		= 0xff000000 ;

				// グレースケール画像を 32ビットアルファチャンネルつき画像に変換
				BYTE *OldGraphData = ( BYTE * )BaseImage->GraphData ;
				BYTE *NewGraphData ;
				BaseImage->Pitch		= BaseImage->Width * 4 ;
				BaseImage->GraphData	= DXALLOC( BaseImage->Pitch * BaseImage->Height ) ;
				if( BaseImage->GraphData == NULL )
				{
					png_destroy_read_struct( &png_ptr, &info_ptr, ( png_infopp )NULL ) ;
					DXFREE( OldGraphData ) ;
					return -1 ;
				}
				NewGraphData = ( BYTE * )BaseImage->GraphData ;

				BYTE *SrcBase  = OldGraphData ;
				BYTE *DestBase = NewGraphData ;

				// もしグレースケールなのに１ピクセル２バイトだったら上位８ビットだけを使用する
				if( rowbytes / 2 >= width )
				{
					for( i = 0 ; i < BaseImage->Height; i ++, SrcBase += rowbytes, DestBase += BaseImage->Pitch )
					{
						BYTE *SrcB = SrcBase ;
						BYTE *Dest = DestBase ;
						for( j = 0 ; j < BaseImage->Width ; j ++, SrcB += 2, Dest += 4 )
						{
							Dest[ 0 ] = SrcB[ 1 ] ;
							Dest[ 1 ] = SrcB[ 1 ] ;
							Dest[ 2 ] = SrcB[ 1 ] ;

							if( SrcB[ 1 ] == trans_values->blue &&
								SrcB[ 1 ] == trans_values->green &&
								SrcB[ 1 ] == trans_values->red )
							{
								Dest[ 3 ] = 0 ;
							}
							else
							{
								Dest[ 3 ] = 255 ;
							}
						}
					}
				}
				else
				{
					for( i = 0 ; i < BaseImage->Height; i ++, SrcBase += rowbytes, DestBase += BaseImage->Pitch )
					{
						BYTE *SrcB = SrcBase ;
						BYTE *Dest = DestBase ;
						for( j = 0 ; j < BaseImage->Width ; j ++, SrcB += 1, Dest += 4 )
						{
							Dest[ 0 ] = SrcB[ 0 ] ;
							Dest[ 1 ] = SrcB[ 0 ] ;
							Dest[ 2 ] = SrcB[ 0 ] ;

							if( SrcB[ 0 ] == trans_values->blue &&
								SrcB[ 0 ] == trans_values->green &&
								SrcB[ 0 ] == trans_values->red )
							{
								Dest[ 3 ] = 0 ;
							}
							else
							{
								Dest[ 3 ] = 255 ;
							}
						}
					}
				}

				DXFREE( OldGraphData ) ;
				OldGraphData = NULL ;
			}
			else
			{
				// もしグレースケールなのに１ピクセル２バイトだったら上位８ビットだけを使用する
				if( rowbytes / 2 >= width )
				{
					BaseImage->ColorData.ColorBitDepth	= 16 ;
					BaseImage->ColorData.PixelByte		= 2 ;

					BaseImage->ColorData.NoneMask		= 0x000000ff ;

					BaseImage->ColorData.AlphaLoc		= 0 ;
					BaseImage->ColorData.AlphaMask		= 0 ;
					BaseImage->ColorData.AlphaWidth		= 0 ;

					BaseImage->ColorData.RedLoc			= 8 ;
					BaseImage->ColorData.RedMask		= 0x0000ff00 ;
					BaseImage->ColorData.RedWidth		= 8 ;

					BaseImage->ColorData.GreenLoc		= 8 ;
					BaseImage->ColorData.GreenMask		= 0x0000ff00 ;
					BaseImage->ColorData.GreenWidth		= 8 ;

					BaseImage->ColorData.BlueLoc		= 8  ;
					BaseImage->ColorData.BlueMask		= 0x0000ff00 ;
					BaseImage->ColorData.BlueWidth		= 8 ;
				}
				else
				{
					NS_CreateGrayColorData( &BaseImage->ColorData ) ;
				}
			}
		}
		else
		if( color_type == PNG_COLOR_TYPE_GRAY_ALPHA )
		{
			BYTE *NewBuffer ;
			BYTE *SrcB ;
			BYTE *Dest ;
			DWORD Pitch ;
			DWORD i ;
			DWORD j ;

			Pitch            = 4 * width ;
			BaseImage->Pitch = ( int )Pitch ;
			NS_CreateARGB8ColorData( &BaseImage->ColorData ) ;

			if( GetFormatOnly == FALSE )
			{
				NewBuffer = ( BYTE * )DXALLOC( Pitch * height ) ;
				if( NewBuffer == NULL )
				{
					png_destroy_read_struct( &png_ptr, &info_ptr, ( png_infopp )NULL ) ;
					DXFREE( ImageData ) ;
					return -1 ;
				}
				BaseImage->GraphData	= NewBuffer ;

				SrcB = ( BYTE * )ImageData ;
				Dest = NewBuffer ;

				// １ピクセル２バイトの場合は１ピクセル目は輝度、２バイト目はアルファ値
				if( rowbytes / 2 >= width )
				{
					for( i = 0 ; i < height ; i ++ )
					{
						for( j = 0 ; j < width ; j ++ )
						{
							Dest[ 0 ] = SrcB[ 0 ] ;
							Dest[ 1 ] = SrcB[ 0 ] ;
							Dest[ 2 ] = SrcB[ 0 ] ;
							Dest[ 3 ] = SrcB[ 1 ] ;
							Dest += 4 ;
							SrcB += 2 ;
						}

						SrcB += rowbytes - width * 2 ;
					}
				}
				else
				{
					for( i = 0 ; i < height ; i ++ )
					{
						for( j = 0 ; j < width ; j ++ )
						{
							Dest[ 0 ] = 255 ;
							Dest[ 1 ] = 255 ;
							Dest[ 2 ] = 255 ;
							Dest[ 3 ] = *SrcB ;
							Dest += 4 ;
							SrcB ++ ;
						}

						SrcB += rowbytes - width ;
					}
				}

				DXFREE( ImageData ) ;
			}
		}
		else
		{
//			if( info_ptr->channels == 4 )
			if( png_get_channels( png_ptr, info_ptr ) == 4 )
			{
				NS_CreateFullColorData( &BaseImage->ColorData ) ;
				BaseImage->ColorData.ColorBitDepth	= 32 ;
				BaseImage->ColorData.PixelByte		= 4 ;
				BaseImage->ColorData.AlphaLoc		= 24 ;
				BaseImage->ColorData.AlphaWidth		= 8 ;
				BaseImage->ColorData.AlphaMask		= 0xff000000 ;
			}
			else
			// 透過色情報がある場合はアルファチャンネルつき画像にする
			if( png_get_valid( png_ptr, info_ptr, PNG_INFO_tRNS ) )
			{
				png_bytep trans ;
				png_color_16p trans_values ;
				int num_trans ;
				int i, j ;

				// 透過色を取得
				png_get_tRNS( png_ptr, info_ptr, &trans, &num_trans, &trans_values ) ;

				// アルファチャンネルありにする
				NS_CreateFullColorData( &BaseImage->ColorData ) ;
				BaseImage->ColorData.ColorBitDepth	= 32 ;
				BaseImage->ColorData.PixelByte		= 4 ;
				BaseImage->ColorData.AlphaLoc		= 24 ;
				BaseImage->ColorData.AlphaWidth		= 8 ;
				BaseImage->ColorData.AlphaMask		= 0xff000000 ;

				// 24ビットカラー画像を 32ビットアルファチャンネルつき画像に変換
				BYTE *OldGraphData = ( BYTE * )BaseImage->GraphData ;
				BYTE *NewGraphData ;
				BaseImage->Pitch		= BaseImage->Width * 4 ;
				BaseImage->GraphData	= DXALLOC( BaseImage->Pitch * BaseImage->Height ) ;
				if( BaseImage->GraphData == NULL )
				{
					png_destroy_read_struct( &png_ptr, &info_ptr, ( png_infopp )NULL ) ;
					DXFREE( OldGraphData ) ;
					return -1 ;
				}
				NewGraphData = ( BYTE * )BaseImage->GraphData ;

				BYTE *SrcBase  = OldGraphData ;
				BYTE *DestBase = NewGraphData ;
				for( i = 0 ; i < BaseImage->Height; i ++, SrcBase += rowbytes, DestBase += BaseImage->Pitch )
				{
					BYTE *SrcB = SrcBase ;
					BYTE *Dest = DestBase ;
					for( j = 0 ; j < BaseImage->Width ; j ++, SrcB += 3, Dest += 4 )
					{
						Dest[ 0 ] = SrcB[ 0 ] ;
						Dest[ 1 ] = SrcB[ 1 ] ;
						Dest[ 2 ] = SrcB[ 2 ] ;

						if( SrcB[ 0 ] == trans_values->blue &&
							SrcB[ 1 ] == trans_values->green &&
							SrcB[ 2 ] == trans_values->red )
						{
							Dest[ 3 ] = 0 ;
						}
						else
						{
							Dest[ 3 ] = 255 ;
						}
					}
				}

				DXFREE( OldGraphData ) ;
				OldGraphData = NULL ;
			}
			else
			// それ以外の場合は通常のフルカラー画像
			{
				NS_CreateFullColorData( &BaseImage->ColorData ) ;
			}
		}
	}

	// 読み込み処理用構造体の破棄
	png_destroy_read_struct( &png_ptr, &info_ptr, ( png_infopp )NULL ) ;

	// 終了
	return 0;
}

#ifndef DX_NON_SAVEFUNCTION

static void png_general_flush( png_structp /* png_ptr */ )
{
}

static void png_general_write_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
	DWORD_PTR fp ;

	fp = ( DWORD_PTR )png_get_io_ptr( png_ptr ) ;

	WriteOnlyFileAccessWrite( fp, data, length ) ;
}

int png_general_write_set( png_structp png_ptr, DWORD_PTR fp )
{
	// コールバック関数のセット
	png_set_write_fn( png_ptr, ( void * )fp, png_general_write_data, png_general_flush ) ;

	// 終了
	return 0 ;
}

extern int SaveBaseImageToPngBase( const char *pFilePathW, const char *pFilePathA, BASEIMAGE *BaseImage, int CompressionLevel )
{
	DWORD_PTR fp ;
	png_structp png_ptr;
	png_infop   info_ptr;
	png_bytepp  buffer;
	png_bytep   sample;
	int r, g, b, a, i, j;
	
	// 保存用のファイルを開く
	if( pFilePathW )
	{
		fp = WriteOnlyFileAccessOpenWCHAR( pFilePathW ) ;
	}
	else
	{
		char TempBuffer[ 1024 ] ;

		ConvString( pFilePathA, -1, CHAR_CHARCODEFORMAT, TempBuffer, sizeof( TempBuffer ), WCHAR_T_CHARCODEFORMAT ) ;
		fp = WriteOnlyFileAccessOpenWCHAR( TempBuffer ) ;
	}
	if( fp == 0 ) return -1;

	// ＰＮＧ管理情報の作成
	png_ptr = png_create_write_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if( png_ptr == NULL ) return -1;

	// 画像情報構造体の作成
	info_ptr = png_create_info_struct(png_ptr);             // info_ptr構造体を確保・初期化します
	if( info_ptr == NULL )
	{
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		return -1 ;
	}

	// エラー時の処理セットアップおよび処理
	if( setjmp( png_jmpbuf( png_ptr ) ) )
	{
ERR:
		if( buffer )
		{
			for( i = 0; i < BaseImage->Height; i++ )
				if( buffer[i] ) DXFREE( buffer[i] );
			DXFREE( buffer );
		}
		if( fp ) WriteOnlyFileAccessClose( fp );
		png_destroy_write_struct( &png_ptr, &info_ptr );
		return -1 ;
	}

	png_general_write_set( png_ptr, fp ) ;

	// 開いたファイルポインタをセット
//	png_init_io( png_ptr, fp );

	// 使用するフィルタのセット
	png_set_filter( png_ptr, 0, PNG_ALL_FILTERS );

	// 圧縮レベルのセット
	png_set_compression_level( png_ptr, CompressionLevel );

	// IHDRチャンク情報の設定
	png_set_IHDR(
		png_ptr,
		info_ptr,
		( png_uint_32 )BaseImage->Width,
		( png_uint_32 )BaseImage->Height,
		8,
		BaseImage->ColorData.AlphaWidth ? PNG_COLOR_TYPE_RGBA : PNG_COLOR_TYPE_RGB,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT
	) ;

	// ヘッダ部分の書き出し
	png_write_info( png_ptr, info_ptr );

	// 圧縮用データの用意
	{
		// バッファの確保
		buffer = (png_bytepp)DXALLOC( sizeof( png_bytep ) * BaseImage->Height );
		if( buffer == NULL ) goto ERR;
		_MEMSET( buffer, 0, sizeof( png_bytep ) * BaseImage->Height );
		for( i = 0; i < BaseImage->Height; i++ )
		{
			buffer[i] = (png_bytep)DXALLOC( sizeof( png_byte ) * ( BaseImage->ColorData.AlphaWidth ? 4 : 3 ) * BaseImage->Width );
			if( buffer[i] == NULL ) goto ERR;
			sample = buffer[i];
			if( BaseImage->ColorData.AlphaWidth )
			{
				for( j = 0; j < BaseImage->Width; j ++, sample += 4 )
				{
					NS_GetPixelBaseImage( BaseImage, j, i, &r, &g, &b, &a );
					sample[0] = (png_byte)r;
					sample[1] = (png_byte)g;
					sample[2] = (png_byte)b;
					sample[3] = (png_byte)a;
				}
			}
			else
			{
				for( j = 0; j < BaseImage->Width; j ++, sample += 3 )
				{
					NS_GetPixelBaseImage( BaseImage, j, i, &r, &g, &b, &a );
					sample[0] = (png_byte)r;
					sample[1] = (png_byte)g;
					sample[2] = (png_byte)b;
				}
			}
		}
	}

	// イメージの書き出し
	png_write_image( png_ptr, buffer );

	// フッダ部分の書き出し
	png_write_end( png_ptr, info_ptr );

	// 構造体の後始末
	png_destroy_write_struct( &png_ptr, &info_ptr );

	// ファイルを閉じる
	WriteOnlyFileAccessClose( fp ) ;

	// メモリの解放
	for( i = 0; i < BaseImage->Height; i++ )
		if( buffer[i] ) DXFREE( buffer[i] );
	DXFREE( buffer );

	// 正常終了
	return 0 ;
}

#endif // DX_NON_SAVEFUNCTION
#endif // DX_NON_PNGREAD








// ＪＰＥＧをメモリから読み込むためのプログラム
#ifndef DX_NON_JPEGREAD

// (殆ど jdatasrc.c の流用)
#include "jinclude.h"
#include "jpeglib.h"
#include "jerror.h"

// ＪＰＥＧ読みこみエラー時処理ルーチン用構造体
typedef struct my_error_mgr {
	struct jpeg_error_mgr pub;	// 標準エラーデータ

	jmp_buf setjmp_buffer;		// ロングジャンプ用構造体
} *my_error_ptr ;

// エラー時に呼ばれる関数
void my_error_exit( j_common_ptr cinfo )
{
	// cinfo->errが示す標準エラーデータの先頭アドレスをmy_error_mgr構造体の先頭アドレスに変換
	my_error_ptr myerr = (my_error_ptr) cinfo->err;

	// すいませんよくわかりません、とりあえずエラーメッセージ標準関数？
	(*cinfo->err->output_message) (cinfo);

	// ユーザーが決めたエラー処理コードの位置まで飛ぶ
	longjmp( myerr->setjmp_buffer, 1 ) ;
}


// 汎用データ形式の転送用構造体
typedef struct
{
	struct jpeg_source_mgr pub;		/* public fields */

	JOCTET *buffer;					/* start of buffer */
	boolean start_of_file;			/* have we gotten any data yet? */

	STREAMDATA *Data ;				// 汎用データ形式読み込み処理用構造体
	int DataSize ;					// データのサイズ
} my_source_general_mgr;

typedef my_source_general_mgr	*my_src_general_ptr;

#define INPUT_BUF_SIZE		(4096)	// 作業用バッファのサイズ

// 読み込むソースを初期化する関数
METHODDEF(void)
init_source_general( j_decompress_ptr cinfo )
{
	my_src_general_ptr src = (my_src_general_ptr) cinfo->src;

	// ファイル読み込み開始のフラグを立てる
	src->start_of_file = TRUE;
}

// データバッファにデータを転送する
METHODDEF(boolean)
fill_input_buffer_general (j_decompress_ptr cinfo)
{
	my_src_general_ptr src = (my_src_general_ptr) cinfo->src;
	size_t nbytes;

	// 転送するデータの量をコピーする
	nbytes = ( size_t )( ( src->DataSize - STTELL( src->Data ) < INPUT_BUF_SIZE ) ?
			 src->DataSize - STTELL( src->Data ) : INPUT_BUF_SIZE ) ;
	if( nbytes != 0 ) STREAD( src->buffer, nbytes, 1, src->Data ) ;

	// 読み込みに失敗したらエラー
	if( nbytes <= 0 )
	{
		if (src->start_of_file)	/* Treat empty input file as fatal error */
			ERREXIT(cinfo, JERR_INPUT_EMPTY);
		WARNMS(cinfo, JWRN_JPEG_EOF);

		/* Insert a fake EOI marker */
	    src->buffer[0] = (JOCTET) 0xFF;
		src->buffer[1] = (JOCTET) JPEG_EOI;
		nbytes = 2;
	}

	// その他の処理
	src->pub.next_input_byte = src->buffer;
	src->pub.bytes_in_buffer = nbytes;
	src->start_of_file = FALSE;

	return TRUE;
}

// 指定されたサイズのデータをスキップする処理
METHODDEF(void)
skip_input_data_general( j_decompress_ptr cinfo, long num_bytes)
{
	my_src_general_ptr src = (my_src_general_ptr) cinfo->src;

	// データスキップ処理
	if( num_bytes > 0 )
	{
		while( num_bytes > (long) src->pub.bytes_in_buffer )
		{
			num_bytes -= (long) src->pub.bytes_in_buffer;
			(void) fill_input_buffer_general( cinfo ) ;
		}
		src->pub.next_input_byte += (size_t) num_bytes;
		src->pub.bytes_in_buffer -= (size_t) num_bytes;
	}
}

// データを閉じるときの処理
METHODDEF(void)
term_source_general( j_decompress_ptr /*cinfo*/ )
{
  /* no work necessary here */
}

// 汎用データ読み込み処理からデータを読み込むようにする実際の設定を行う関数
GLOBAL(void)
jpeg_general_src (j_decompress_ptr cinfo, STREAMDATA *Data )
{
	my_src_general_ptr src;

	// まだＪＰＥＧデータを一時的に格納するバッファを確保していなかったら確保する
	if (cinfo->src == NULL)
	{
		/* first time for this JPEG object? */
		cinfo->src = (struct jpeg_source_mgr *)
						(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
						SIZEOF(my_source_general_mgr));
		src = (my_src_general_ptr) cinfo->src;
		src->buffer = (JOCTET *)
					(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
					INPUT_BUF_SIZE * SIZEOF(JOCTET));
	}

	// 関数ポインタなどをセットする
	src = (my_src_general_ptr) cinfo->src;
	src->pub.init_source			= init_source_general ;
	src->pub.fill_input_buffer		= fill_input_buffer_general ;
	src->pub.skip_input_data		= skip_input_data_general ;
	src->pub.resync_to_restart		= jpeg_resync_to_restart ; /* use default method */
	src->pub.term_source			= term_source_general ;

	src->Data = Data ;

	// 現在のファイルポインタから終端までのサイズを取得する
	{
		long pos ;
		pos = ( long )STTELL( src->Data ) ;
		STSEEK( src->Data, 0, STREAM_SEEKTYPE_END ) ;
		src->DataSize = ( int )STTELL( src->Data ) - pos ;
		STSEEK( src->Data, pos, STREAM_SEEKTYPE_SET ) ;
	}

	src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
	src->pub.next_input_byte = NULL; /* until buffer loaded */
}

#ifdef DX_TEST	// テスト用
extern LONGLONG time2;
#endif

// ＪＰＥＧ画像の読みこみ
extern int LoadJpegImage( STREAMDATA *Src, BASEIMAGE *BaseImage, int GetFormatOnly )
{
	return LoadJpegImageBase( Src, BaseImage, GetFormatOnly, FALSE, NULL, 0 ) ;
}
extern int LoadJpegImageBase( STREAMDATA *Src, BASEIMAGE *BaseImage, int GetFormatOnly, int GetExifOnly, void *ExifBuffer, size_t ExifBufferSize )
{
	struct jpeg_decompress_struct cinfo ;
	struct my_error_mgr jerr ;
	JSAMPARRAY buffer ;
	void *ImageData = NULL ;
	unsigned int InPitch ;
	unsigned int i ;
	unsigned int imgSize ;
	unsigned char *pImg ;
	unsigned int OutPitch ;
	unsigned char Check ;
	unsigned int PixelByte ;

	// 先頭の１バイトが 0xFF ではなかったらJPEGファイルではない
	STREAD( &Check, 1, 1, Src ) ;
	STSEEK( Src, 0, STREAM_SEEKTYPE_SET ) ;
	if( Check != 0xff )
	{
		return -1 ;
	}
	
	// 通常ＪＰＥＧエラールーチンのセットアップ
	_MEMSET( &cinfo, 0, sizeof( cinfo ) );
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	if( setjmp( jerr.setjmp_buffer ) )
	{
		jpeg_destroy_decompress( &cinfo );
		return -1;
	}

#ifdef DX_TEST	// テスト用
	time2 = NS_GetNowHiPerformanceCount();
#endif

	// cinfo情報のアロケートと初期化を行う
	jpeg_create_decompress(&cinfo);

	// 汎用データ読み込み処理からデータを読み込む設定を行う
	jpeg_general_src( &cinfo, Src ) ;

	// Exif 情報を取得するための設定を行う
	if( GetExifOnly == TRUE )
	{
		jpeg_save_markers( &cinfo, JPEG_APP0 + 1, 0xFFFF ) ;
	}

	// ＪＰＥＧファイルのパラメータ情報の読みこみ
	(void)jpeg_read_header(&cinfo, TRUE);

	// Exif 情報を取得する場合は処理を分岐
	if( GetExifOnly == TRUE )
	{
		int data_length ;

		// Exif情報がなかったらエラー
		if( cinfo.marker_list == NULL ||
			cinfo.marker_list->marker != JPEG_APP0 + 1 ||
			cinfo.marker_list->data_length < 6 ||
			GETJOCTET( cinfo.marker_list->data[ 0 ] ) != 0x45 ||
			GETJOCTET( cinfo.marker_list->data[ 1 ] ) != 0x78 ||
			GETJOCTET( cinfo.marker_list->data[ 2 ] ) != 0x69 ||
			GETJOCTET( cinfo.marker_list->data[ 3 ] ) != 0x66 ||
			GETJOCTET( cinfo.marker_list->data[ 4 ] ) != 0 ||
			GETJOCTET( cinfo.marker_list->data[ 5 ] ) != 0 )
		{
			jpeg_destroy_decompress( &cinfo );
			return -1 ;
		}

		// ExifBuffer が NULL の場合はサイズのみ返す
		data_length = ( int )cinfo.marker_list->data_length ;
		if( ExifBuffer == NULL )
		{
			jpeg_destroy_decompress( &cinfo );
			return data_length ;
		}

		// ExifBufferSize のサイズが足りない場合はエラー
		if( cinfo.marker_list->data_length > ExifBufferSize )
		{
			jpeg_destroy_decompress( &cinfo );
			return -1 ;
		}

		// Exif 情報をコピー
		_MEMCPY( ExifBuffer, cinfo.marker_list->data, cinfo.marker_list->data_length ) ;

		// Exif 情報のサイズを返す
		jpeg_destroy_decompress( &cinfo );
		return data_length ;
	}

	if( GetFormatOnly == FALSE )
	{
		// ＪＰＥＧファイルの解凍の開始
		(void)jpeg_start_decompress(&cinfo);

		// １ライン当たりのデータバイト数を計算
		PixelByte = ( unsigned int )( cinfo.output_components == 4 ? 3 : cinfo.output_components ) ;
		InPitch   = cinfo.output_width * cinfo.output_components ;
		OutPitch  = cinfo.output_width * PixelByte ;

		// データバッファの確保
		buffer = (*cinfo.mem->alloc_sarray)
					((j_common_ptr) &cinfo, JPOOL_IMAGE, InPitch, 1 );

		// 出力イメージデータサイズのセット、及びデータ領域の確保
		imgSize = cinfo.output_height * OutPitch ;
		if( ( ImageData = ( unsigned char * ) DXALLOC( imgSize ) ) == NULL )
		{
			jpeg_destroy_decompress( &cinfo );
			return -1 ;
		}
		
		// 画像データの読みこみ
		pImg = ( unsigned char * )ImageData ; 
		while( cinfo.output_scanline < cinfo.output_height )
		{
			(void) jpeg_read_scanlines(&cinfo, buffer, 1);

			// データを出力データに変換して、またはそのまま転送
			for( i = 0 ; i < InPitch ; )
			{
				switch( cinfo.output_components )
				{
				case 1 :	// グレースケール
					*pImg = *( buffer[0] + i ) ; pImg ++ ;
					i ++ ;
					break ;

				case 3 :	// RGB
					*pImg = *( buffer[0] + i + 2 ) ; pImg ++ ;
					*pImg = *( buffer[0] + i + 1 ) ; pImg ++ ;
					*pImg = *( buffer[0] + i     ) ; pImg ++ ;
					i += 3 ;
					break ;

				case 4 :	// CMYK?
					pImg[ 0 ] = ( unsigned char )( *( buffer[0] + i + 2 ) * *( buffer[0] + i + 3 ) / 255 ) ;
					pImg[ 1 ] = ( unsigned char )( *( buffer[0] + i + 1 ) * *( buffer[0] + i + 3 ) / 255 ) ;
					pImg[ 2 ] = ( unsigned char )( *( buffer[0] + i + 0 ) * *( buffer[0] + i + 3 ) / 255 ) ;
					pImg += 3 ;
					i += 4 ;
					break ;
				}
			}
		}

		// 解凍処理の終了
		(void) jpeg_finish_decompress(&cinfo);

		BaseImage->Width     = ( int )cinfo.output_width ;
		BaseImage->Height    = ( int )cinfo.output_height ;
		BaseImage->Pitch     = ( int )OutPitch ;
	}
	else
	{
		BaseImage->Width     = ( int )cinfo.image_width ;
		BaseImage->Height    = ( int )cinfo.image_height ;
		BaseImage->Pitch     = 0 ;
	}

	// BASEIMAGE 構造体のデータを詰める
	{
		BaseImage->GraphData = ImageData ;

		// カラー情報をセットする
		switch( PixelByte )
		{
		case 1 :
			NS_CreateGrayColorData( &BaseImage->ColorData ) ;
			break ;

		case 3 :
			NS_CreateFullColorData( &BaseImage->ColorData ) ;
			break ;
		}
	}

	// cinfo構造体の解放
	jpeg_destroy_decompress(&cinfo);

#ifdef DX_TEST	// テスト用
	time2 = NS_GetNowHiPerformanceCount() - time2 ;
#endif

	// 終了
	return 0 ;
}

#ifndef DX_NON_SAVEFUNCTION

#define OUTPUT_BUF_SIZE  4096	/* choose an efficiently fwrite'able size */

/* Expanded data destination object for stdio output */

typedef struct {
  struct jpeg_destination_mgr pub; /* public fields */

  DWORD_PTR outfile;	/* target stream */
  JOCTET * buffer;		/* start of buffer */
} my_destination_mgr;

typedef my_destination_mgr * my_dest_ptr;

METHODDEF(void)
init_destination_general (j_compress_ptr cinfo)
{
  my_dest_ptr dest = (my_dest_ptr) cinfo->dest;

  /* Allocate the output buffer --- it will be released when done with image */
  dest->buffer = (JOCTET *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				  OUTPUT_BUF_SIZE * SIZEOF(JOCTET));

  dest->pub.next_output_byte = dest->buffer;
  dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;
}

METHODDEF(boolean)
empty_output_buffer_general (j_compress_ptr cinfo)
{
  my_dest_ptr dest = (my_dest_ptr) cinfo->dest;

  WriteOnlyFileAccessWrite(dest->outfile, dest->buffer, OUTPUT_BUF_SIZE);

  dest->pub.next_output_byte = dest->buffer;
  dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;

  return TRUE;
}

METHODDEF(void)
term_destination_general (j_compress_ptr cinfo)
{
  my_dest_ptr dest = (my_dest_ptr) cinfo->dest;
  size_t datacount = OUTPUT_BUF_SIZE - dest->pub.free_in_buffer;

  /* Write any data remaining in the buffer */
  if (datacount > 0) {
    WriteOnlyFileAccessWrite(dest->outfile, dest->buffer, datacount);
  }
}

GLOBAL(void)
jpeg_general_dest (j_compress_ptr cinfo, DWORD_PTR outfile)
{
   my_dest_ptr dest;

  /* The destination object is made permanent so that multiple JPEG images
   * can be written to the same file without re-executing jpeg_stdio_dest.
   * This makes it dangerous to use this manager and a different destination
   * manager serially with the same JPEG object, because their private object
   * sizes may be different.  Caveat programmer.
   */
  if (cinfo->dest == NULL) {	/* first time for this JPEG object? */
    cinfo->dest = (struct jpeg_destination_mgr *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
				  SIZEOF(my_destination_mgr));
  }

  dest = (my_dest_ptr) cinfo->dest;
  dest->pub.init_destination = init_destination_general;
  dest->pub.empty_output_buffer = empty_output_buffer_general;
  dest->pub.term_destination = term_destination_general;
  dest->outfile = outfile;
}

extern int SaveBaseImageToJpegBase( const char *pFilePathW, const char *pFilePathA, BASEIMAGE *BaseImage, int Quality, int /*Sample2x1*/ )
{
	struct jpeg_compress_struct cinfo ;
	struct my_error_mgr jerr ;
	DWORD_PTR fp = 0;
	JSAMPARRAY buffer = 0;
	JSAMPROW sample;
	int i, j, r, g, b, a;

	// 保存用のファイルを開く
	if( pFilePathW )
	{
		fp = WriteOnlyFileAccessOpenWCHAR( pFilePathW ) ;
	}
	else
	{
		char TempBuffer[ 1024 ] ;

		ConvString( pFilePathA, -1, CHAR_CHARCODEFORMAT, TempBuffer, sizeof( TempBuffer ), WCHAR_T_CHARCODEFORMAT ) ;
		fp = WriteOnlyFileAccessOpenWCHAR( TempBuffer ) ;
	}
	if( fp == 0 ) return -1;

#ifdef DX_TEST	// テスト用
	time2 = NS_GetNowHiPerformanceCount();
#endif

	// 通常ＪＰＥＧエラールーチンのセットアップ
	_MEMSET( &cinfo, 0, sizeof( cinfo ) );
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	if( setjmp( jerr.setjmp_buffer ) )
	{
ERR:
		if( buffer )
		{
			for( i = 0; i < BaseImage->Height; i++ )
				if( buffer[i] ) DXFREE( buffer[i] );
			DXFREE( buffer );
		}

		if( fp ) WriteOnlyFileAccessClose( fp );
		jpeg_destroy_compress( &cinfo );
		return -1;
	}

	// cinfo情報のアロケートと初期化を行う
	jpeg_create_compress( &cinfo );

	// 出力ファイルのポインタをセット
	jpeg_general_dest( &cinfo, fp ) ;
//	jpeg_stdio_dest( &cinfo, fp );

	// 出力画像の情報を設定する
	cinfo.image_width      = ( JDIMENSION )BaseImage->Width;
	cinfo.image_height     = ( JDIMENSION )BaseImage->Height;
	cinfo.input_components = 3;
	cinfo.in_color_space   = JCS_RGB;
	jpeg_set_defaults( &cinfo );

	// 画質を設定
	jpeg_set_quality( &cinfo, Quality, TRUE );

	// 圧縮処理開始
	jpeg_start_compress( &cinfo, TRUE );

	// 圧縮用データの用意
	{
		// バッファの確保
		buffer = (JSAMPARRAY)DXALLOC( sizeof( JSAMPROW ) * BaseImage->Height );
		if( buffer == NULL ) goto ERR;
		_MEMSET( buffer, 0, sizeof( JSAMPROW ) * BaseImage->Height );
		for( i = 0; i < BaseImage->Height; i++ )
		{
			buffer[i] = (JSAMPROW)DXALLOC( sizeof( JSAMPLE ) * 3 * BaseImage->Width );
			if( buffer[i] == NULL ) goto ERR;
			sample = buffer[i];
			for( j = 0; j < BaseImage->Width; j ++, sample += 3 )
			{
				NS_GetPixelBaseImage( BaseImage, j, i, &r, &g, &b, &a );
				sample[0] = (JSAMPLE)r;
				sample[1] = (JSAMPLE)g;
				sample[2] = (JSAMPLE)b;
			}
		}
	}

	// 圧縮
	jpeg_write_scanlines( &cinfo, buffer, ( JDIMENSION )BaseImage->Height );

	// 圧縮終了
	jpeg_finish_compress( &cinfo );

	// 後始末
	jpeg_destroy_compress( &cinfo );

#ifdef DX_TEST	// テスト用
	time2 = NS_GetNowHiPerformanceCount() - time2 ;
#endif

	// ファイルを閉じる
	WriteOnlyFileAccessClose( fp );

	// データの解放
	for( i = 0; i < BaseImage->Height; i++ )
		DXFREE( buffer[i] );
	DXFREE( buffer );

	// 正常終了
	return 0;
}

#endif // DX_NON_SAVEFUNCTION

#endif // DX_NON_JPEGREAD






#ifndef DX_NON_MERSENNE_TWISTER

//=====================================================================================
//   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
// 
// 下記のプログラムは松本様と西村様が作成されたプログラムを元に SYN様 が
// ＭＭＸを使用する改良を行われた Mersenne Twister 法疑似乱数発生プログラムを
// 更に BorlandC++ Compiler でもコンパイル出来るようにしたバージョン

/* Period parameters */
#define N 624
#define M 397
#define MATRIX_A 0x9908b0df   /* constant vector a */
#define UPPER_MASK 0x80000000 /* most significant w-r bits */
#define LOWER_MASK 0x7fffffff /* least significant r bits */

/* Tempering parameters */
#define TEMPERING_MASK_B 0x9d2c5680
#define TEMPERING_MASK_C 0xefc60000
#define TEMPERING_SHIFT_U(y)  (y >> 11)
#define TEMPERING_SHIFT_S(y)  (y << 7)
#define TEMPERING_SHIFT_T(y)  (y << 15)
#define TEMPERING_SHIFT_L(y)  (y >> 18)

/* Static member */
int mti;                   /* index number */
unsigned long mt[N + 1];   /* the array for the state vector */
unsigned long mtr[N];      /* the array for the random number */
unsigned long bInitialized = 0;
unsigned long bMMX = 0;

/* Prototype */
//#ifndef DX_NON_INLINE_ASM
//unsigned long CheckMMX(void);
//#endif
//void srandMT(unsigned long seed);
#ifdef DX_NON_INLINE_ASM
void generateMT_C(void);
#endif
//unsigned long randMT(void);


void srandMT(unsigned int seed)
{
    int i;

    for(i = 0; i < N; i++){
         mt[i] = seed & 0xffff0000;
         seed = 69069 * seed + 1;
         mt[i] |= (seed & 0xffff0000) >> 16;
         seed = 69069 * seed + 1;
    }

#ifndef DX_NON_INLINE_ASM
    bMMX = CheckMMX();
#else
	bMMX = 0;
#endif
    bInitialized = 1;
#ifndef DX_NON_INLINE_ASM
    generateMT();
#else
    generateMT_C();
#endif
}

#ifndef DX_NON_INLINE_ASM

#ifdef __BCC
	#define cpuid __emit__(0xf) ; __emit__(0xa2) ;
#else
	#define cpuid __asm __emit 0fh __asm __emit 0a2h
#endif

unsigned long CheckMMX(void)
{
    unsigned long flag = 0;
	
    _asm{
        push    edx
        push    ecx
        pushfd
        pop     eax
        xor     eax, 00200000h
        push    eax
        popfd
        pushfd
        pop     ebx
        cmp     eax, ebx
        jnz     non_mmx
        mov     eax, 0
	}
//		db 0fh
//		db 0a2h
        cpuid
	__asm{
        cmp     eax, 0
        jz      non_mmx
        mov     eax, 1
	}
//		db 0fh
//		db 0a2h
	    cpuid
    __asm{
        and     edx, 00800000h
        jz      non_mmx
        mov     flag, 1
        non_mmx:
        pop     ecx
        pop     edx
    }

    return(flag);
}

/* MMX version */
void MMX_generateMT(void)
{
    _asm{
#ifdef __BCC
//PMMX // Borland C++ 5.6.4 for Win32 Copyright (c) 1993, 2002 Borland	では要らなくなったようです
#endif
        mov         eax, MATRIX_A
        movd        mm4, eax
        punpckldq   mm4, mm4
        mov         eax, 1
        movd        mm5, eax
        punpckldq   mm5, mm5
        movq        mm6, mm5
        psllq       mm6, 31     /* UPPER_MASK */
        movq        mm7, mm6
        psubd       mm7, mm5    /* LOWER_MASK */

        lea         esi, mt
        add         esi, ((N-M)/2)*8
        mov         edi, esi
        add         edi, M*4
        
        mov         ecx, -((N-M)/2)
        /* 8clocks */
        movq        mm0, [esi+ecx*8]
        pand        mm0, mm6
        movq        mm1, [esi+ecx*8+4]
        movq        mm2, [edi+ecx*8]
        pand        mm1, mm7
    loop0:
        por         mm0, mm1
        movq        mm3, mm1
        psrld       mm0, 1
        pand        mm3, mm5
        pxor        mm2, mm0
        psubd       mm3, mm5
        movq        mm0, [esi+ecx*8+8]
        pandn       mm3, mm4
        movq        mm1, [esi+ecx*8+12]
        pxor        mm3, mm2
        movq        mm2, [edi+ecx*8+8]
        pand        mm0, mm6
        movq        [esi+ecx*8], mm3
        pand        mm1, mm7
        inc         ecx
        jnz         loop0
        /**/
        mov         eax, [esi]
        mov         ebx, [esi+4]
        and         eax, UPPER_MASK
        and         ebx, LOWER_MASK
        mov         edx, [edi]
        or          eax, ebx
        shr         eax, 1
        or          ebx, 0xfffffffe
        xor         eax, edx
        add         ebx, 1
        xor         eax, MATRIX_A
        and         ebx, MATRIX_A
        xor         ebx, eax
        mov         [esi], ebx
        /**/

        lea         esi, mt
        mov         eax, [esi] /* mt[N] = mt[0]; */
        add         esi, N*4
        mov         [esi], eax /* mt[N] = mt[0]; */

        mov         edi, esi
        sub         edi, (N-M)*4
        add         esi, 4
        add         edi, 4
        
        mov         ecx, -((M+1)/2) /* overrun */
        /* 8clocks */
        movq        mm0, [esi+ecx*8]
        pand        mm0, mm6
        movq        mm1, [esi+ecx*8+4]
        movq        mm2, [edi+ecx*8]
        pand        mm1, mm7
    loop1:
        por         mm0, mm1
        movq        mm3, mm1
        psrld       mm0, 1
        pand        mm3, mm5
        pxor        mm2, mm0
        psubd       mm3, mm5
        movq        mm0, [esi+ecx*8+8]
        pandn       mm3, mm4
        movq        mm1, [esi+ecx*8+12]
        pxor        mm3, mm2
        movq        mm2, [edi+ecx*8+8]
        pand        mm0, mm6
        movq        [esi+ecx*8], mm3
        pand        mm1, mm7
        inc         ecx
        jnz         loop1
        /**/

        push        ebp

        mov         eax, TEMPERING_MASK_B
        movd        mm6, eax
        punpckldq   mm6, mm6
        mov         eax, TEMPERING_MASK_C
        movd        mm7, eax
        punpckldq   mm7, mm7

        lea         esi, mt
        lea         edi, mtr
        add         esi, N*4
        add         edi, N*4
        mov         ecx, -N/2
        /* 17clocks */
        movq        mm0, [esi+ecx*8]
        movq        mm1, mm0
        psrld       mm0, 11
        pxor        mm0, mm1
        movq        mm2, [esi+ecx*8+8]
    loop2:
        movq        mm1, mm0
        pslld       mm0, 7
        movq        mm3, mm2
        psrld       mm2, 11
        pand        mm0, mm6
        pxor        mm2, mm3
        pxor        mm0, mm1
        movq        mm3, mm2
        pslld       mm2, 7
        movq        mm1, mm0
        pslld       mm0, 15
        pand        mm2, mm6
        pand        mm0, mm7
        pxor        mm2, mm3
        pxor        mm0, mm1
        movq        mm3, mm2
        pslld       mm2, 15
        movq        mm1, mm0
        pand        mm2, mm7
        psrld       mm0, 18
        pxor        mm2, mm3
        pxor        mm1, mm0
        movq        mm0, [esi+ecx*8+16]
        movq        mm3, mm2
        movq        [edi+ecx*8], mm1
        psrld       mm2, 18
        movq        mm1, mm0
        pxor        mm3, mm2
        movq        mm2, [esi+ecx*8+24]
        psrld       mm0, 11
        movq        [edi+ecx*8+8], mm3
        pxor        mm0, mm1
        add         ecx, 2
        jnz         loop2
        /**/

        pop         ebp
        emms
#ifdef __BCC
//PNOMMX // Borland C++ 5.6.4 for Win32 Copyright (c) 1993, 2002 Borland	では要らなくなったようです
#endif
    }
    mti = 0;
}

/* Non MMX version */
void generateMT(void)
{
    if(bMMX){
        MMX_generateMT();
        return;
    }

    _asm{
        lea     esi, mt
        add     esi, (N-M)*4
        mov     edi, esi
        add     edi, M*4
        
        mov     ecx, -(N-M)
        /* 8clocks */
        mov     eax, [esi+ecx*4]
        mov     ebx, [esi+ecx*4+4]
    loop0_a:
        and     eax, UPPER_MASK
        and     ebx, LOWER_MASK
        mov     edx, [edi+ecx*4]
        or      eax, ebx
        shr     eax, 1
        or      ebx, 0xfffffffe
        xor     eax, edx
        add     ebx, 1
        xor     eax, MATRIX_A
        and     ebx, MATRIX_A
        xor     ebx, eax
        mov     eax, [esi+ecx*4+4]
        mov     [esi+ecx*4], ebx
        mov     ebx, [esi+ecx*4+8]
        inc     ecx
        jnz     loop0_a
        /**/

        lea     esi, mt
        mov     eax, [esi] /* mt[N] = mt[0]; */
        add     esi, N*4
        mov     [esi], eax /* mt[N] = mt[0]; */
        mov     edi, esi
        sub     edi, (N-M)*4
        
        mov     ecx, -M
        /* 8clocks */
        mov     eax, [esi+ecx*4]
        mov     ebx, [esi+ecx*4+4]
    loop1_a:
        and     eax, UPPER_MASK
        and     ebx, LOWER_MASK
        mov     edx, [edi+ecx*4]
        or      eax, ebx
        shr     eax, 1
        or      ebx, 0xfffffffe
        xor     eax, edx
        add     ebx, 1
        xor     eax, MATRIX_A
        and     ebx, MATRIX_A
        xor     ebx, eax
        mov     eax, [esi+ecx*4+4]
        mov     [esi+ecx*4], ebx
        mov     ebx, [esi+ecx*4+8]
        inc     ecx
        jnz     loop1_a
        /**/

        push    ebp

        lea     esi, mt
        lea     edi, mtr
        add     esi, N*4
        add     edi, N*4
        mov     ecx, -N/2
        /* 17clocks */
        mov     eax, [esi+ecx*8]
    loop2_a:
        mov     edx, [esi+ecx*8+4]
        mov     ebx, eax
        shr     eax, 11
        mov     ebp, edx
        shr     edx, 11
        xor     eax, ebx
        xor     edx, ebp
        mov     ebx, eax
        shl     eax, 7
        mov     ebp, edx
        shl     edx, 7
        and     eax, TEMPERING_MASK_B
        and     edx, TEMPERING_MASK_B
        xor     eax, ebx
        xor     edx, ebp
        mov     ebx, eax
        shl     eax, 15
        mov     ebp, edx
        shl     edx, 15
        and     eax, TEMPERING_MASK_C
        and     edx, TEMPERING_MASK_C
        xor     eax, ebx
        xor     edx, ebp
        mov     ebx, eax
        shr     eax, 18
        mov     ebp, edx
        shr     edx, 18
        xor     eax, ebx
        xor     edx, ebp
        mov     [edi+ecx*8],   eax
        mov     [edi+ecx*8+4], edx
        mov     eax, [esi+ecx*8+8]
        inc     ecx
        jnz     loop2_a
        /**/

        pop     ebp
    }
    mti = 0;
}

#else

/* C version */
void generateMT_C(void)
{
    int kk;
    unsigned long y;
    static unsigned long mag01[2] = {0x0, MATRIX_A}; /* mag01[x] = x * MATRIX_A  for x=0,1 */
    
    for(kk = 0; kk < N - M; kk++){
        y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
        mt[kk] = mt[kk + M] ^ (y >> 1) ^ mag01[y & 0x1];
    }

    mt[N] = mt[0];

    for(; kk < N; kk++){
        y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
        mt[kk] = mt[kk + (M - N)] ^ (y >> 1) ^ mag01[y & 0x1];
    }

    for(kk = 0; kk < N; kk++){
        y = mt[kk];
        y ^= TEMPERING_SHIFT_U(y);
        y ^= TEMPERING_SHIFT_S(y) & TEMPERING_MASK_B;
        y ^= TEMPERING_SHIFT_T(y) & TEMPERING_MASK_C;
        y ^= TEMPERING_SHIFT_L(y);
        mtr[kk] = y;
    }
    mti = 0;
}

#endif


unsigned long randMT(void)
{
    if(mti >= N){
        if(!bInitialized) srandMT(4357);
#ifndef DX_NON_INLINE_ASM
        generateMT();
#else
		generateMT_C();
#endif
    }
    return mtr[mti++]; 
}
//=============================================================================

#endif // DX_NON_MERSENNE_TWISTER












extern int INT64DIV( const BYTE *Int64, int DivNum )
{
	return ( int )( *( ( LONGLONG * )Int64 ) / DivNum ) ;
}

extern DWORD UINT64DIV( const BYTE *UInt64, DWORD DivNum )
{
	return ( DWORD )( *( ( ULONGLONG * )UInt64 ) / DivNum ) ;
}

extern int INT64MOD( const BYTE *Int64, int ModNum )
{
	return ( int )( *( ( LONGLONG * )Int64 ) % ModNum ) ;
}

extern DWORD UINT64MOD( const BYTE *UInt64, DWORD ModNum )
{
	return ( DWORD )( *( ( ULONGLONG * )UInt64 ) % ModNum ) ;
}






















#ifndef DX_NON_MOVIE
#ifndef DX_NON_DSHOW_MP3
#ifndef DX_NON_DSHOW_MOVIE

HRESULT SoundCallback_DSMP3(  D_IMediaSample * pSample, D_REFERENCE_TIME * /*StartTime*/, D_REFERENCE_TIME * /*StopTime*/, BOOL /*TypeChanged*/, void *CallbackData )
{
	SOUNDCONV_DSMP3 *dsmp3 = ( SOUNDCONV_DSMP3 * )( ( ( SOUNDCONV * ) CallbackData )->ConvFunctionBuffer ) ;
	BYTE *pBuffer ;
	long BufferLen ;

	// バッファが解放されていたら何もせずに終了
	if( dsmp3->PCMBuffer == NULL )
		return 0 ;

	// サンプルのデータサイズを取得する
	pSample->GetPointer( &pBuffer ) ;
	BufferLen = pSample->GetActualDataLength() ;

	// サンプルがバッファに収まらない場合はバッファのサイズを大きくする
	if( dsmp3->PCMValidDataSize + BufferLen > dsmp3->PCMBufferSize )
	{
		void *OldBuffer ;

		// 今までのバッファのサイズを保存
		OldBuffer = dsmp3->PCMBuffer ;

		// バッファのサイズを１ＭＢ増やす
		dsmp3->PCMBufferSize += 1 * 1024 * 1024 ;
		dsmp3->PCMBuffer = DXALLOC( dsmp3->PCMBufferSize ) ;

		// 確保に失敗したらエラー
		if( dsmp3->PCMBuffer == NULL )
		{
			DXFREE( OldBuffer ) ;
			return 0 ;
		}

		// 今までのデータを転送
		_MEMCPY( dsmp3->PCMBuffer, OldBuffer, dsmp3->PCMValidDataSize ) ;

		// 今までのバッファを解放
		DXFREE( OldBuffer ) ;
	}

	_MEMCPY( ( BYTE * )dsmp3->PCMBuffer + dsmp3->PCMValidDataSize, pBuffer, ( size_t )BufferLen ) ;
	dsmp3->PCMValidDataSize += BufferLen ;
	return 0 ;
}

// フィルターの指定方向の最初のピンを取得する
D_IPin *_GetFilterPin( D_IBaseFilter *Filter, D_PIN_DIRECTION Direction )
{
	D_IEnumPins *EnumPin ;
	D_IPin *Pin ;
	D_PIN_DIRECTION PinDir ;

	Filter->EnumPins( &EnumPin ) ;
	while( EnumPin->Next( 1, &Pin, NULL ) == S_OK )
	{
		Pin->QueryDirection( &PinDir ) ;
		if( PinDir == Direction )
		{
			EnumPin->Release() ;
			return Pin ;
		}
		Pin->Release() ;
	}
	EnumPin->Release() ;
	return NULL ;
}

// DirectShowを使ってMP3ファイルをコンバートするセットアップ処理を行う( [戻] -1:エラー )
extern int SetupSoundConvert_DSMP3( SOUNDCONV *SoundConv )
{
	SETUP_WIN_API

	D_CMediaType ccmt, cmt, csmpmt;
	D_CMemStream *MemStream = NULL ;
	D_CMemReader *MemReader = NULL ;
	D_IBaseFilter *NullRenderer = NULL ;
	D_CSampleGrabber *SampleGrabber = NULL ;
	D_IGraphBuilder *GraphBuilder = NULL ;
	D_IMediaControl *MediaControl = NULL ;
	D_IMediaFilter *MediaFilter = NULL ;
	D_IMediaEvent *MediaEvent = NULL ;

	SOUNDCONV_DSMP3 *dsmp3 = ( SOUNDCONV_DSMP3 * )SoundConv->ConvFunctionBuffer ;
	STREAMDATA *Stream = &SoundConv->Stream ;
	void *TempBuffer ;
	HRESULT hr ;
	WAVEFORMATEX *smpfmt;
	DWORD TempSize ;

	// メンバ変数初期化
	_MEMSET( dsmp3, 0, sizeof( SOUNDCONV_DSMP3 ) ) ;

	// ファイルのサイズを取得する
	Stream->ReadShred.Seek( Stream->DataPoint, 0, SEEK_END ) ;
	TempSize = ( DWORD )Stream->ReadShred.Tell( Stream->DataPoint ) ;
	Stream->ReadShred.Seek( Stream->DataPoint, 0, SEEK_SET ) ;

	// ファイルを丸ごと格納できるメモリ領域を取得してそこにファイルを丸ごと読み込む
	TempBuffer = DXALLOC( TempSize ) ;
	if( TempBuffer == NULL )
	{
		goto ERR ;
	}
	Stream->ReadShred.Read( TempBuffer, TempSize, 1, Stream->DataPoint ) ;

	// メモリソースフィルタの準備
	MemStream = ( D_CMemStream * )D_CMemStream::CreateInstance( ( BYTE * )TempBuffer, TempSize ) ;
	if( MemStream == NULL )
	{
		DXST_LOGFILE_ADDA( "MP3\x95\xcf\x8a\xb7\x97\x70 D_CMemStream \x82\xcc\x8d\xec\x90\xac\x82\xc9\x8e\xb8\x94\x73\x82\xb5\x82\xdc\x82\xb5\x82\xbd"/*@ "MP3変換用 D_CMemStream の作成に失敗しました" @*/ );
		goto ERR ;
	}
	hr = S_OK ;
	ccmt.majortype = MEDIATYPE_STREAM ;
	ccmt.subtype = D_MEDIASUBTYPE_MPEG1AUDIO ;
	MemReader = ( D_CMemReader * )D_CMemReader::CreateInstance( MemStream, &ccmt, &hr ) ;
	if( MemReader == NULL )
	{
		DXST_LOGFILE_ADDA( "MP3\x95\xcf\x8a\xb7\x97\x70 D_CMemReader \x82\xcc\x8d\xec\x90\xac\x82\xc9\x8e\xb8\x94\x73\x82\xb5\x82\xdc\x82\xb5\x82\xbd"/*@ "MP3変換用 D_CMemReader の作成に失敗しました" @*/ );
		goto ERR ;
	}
	MemReader->AddRef() ;

	// Nullレンダラーフィルタの作成
	if( ( FAILED( WinAPIData.Win32Func.CoCreateInstanceFunc( CLSID_NULLRENDERER, 0, CLSCTX_INPROC_SERVER, IID_IBASEFILTER, ( void ** )&NullRenderer ) ) ) )
	{
		DXST_LOGFILE_ADDA( "MP3\x95\xcf\x8a\xb7\x97\x70 NullRender \x82\xcc\x8d\xec\x90\xac\x82\xc9\x8e\xb8\x94\x73\x82\xb5\x82\xdc\x82\xb5\x82\xbd"/*@ "MP3変換用 NullRender の作成に失敗しました" @*/ );
		goto ERR ;
	}

	// グラフィックビルダーオブジェクトの作成
	if( ( FAILED( WinAPIData.Win32Func.CoCreateInstanceFunc( CLSID_FILTERGRAPH, NULL, CLSCTX_INPROC, IID_IGRAPHBUILDER, ( void ** )&GraphBuilder ) ) ) )
	{
		DXST_LOGFILE_ADDA( "MP3\x95\xcf\x8a\xb7\x97\x70 IGraphBuilder \x82\xcc\x8d\xec\x90\xac\x82\xc9\x8e\xb8\x94\x73\x82\xb5\x82\xdc\x82\xb5\x82\xbd"/*@ "MP3変換用 IGraphBuilder の作成に失敗しました" @*/ );
		goto ERR ;
	}

	// サンプルグラバフィルタ生成
	hr = TRUE ;
	SampleGrabber = ( D_CSampleGrabber * )D_CSampleGrabber::CreateInstance( NULL, &hr ) ;
	if( SampleGrabber == NULL )
	{
		DXST_LOGFILE_ADDA( "MP3\x95\xcf\x8a\xb7\x97\x70 D_CSampleGrabber \x82\xcc\x8d\xec\x90\xac\x82\xc9\x8e\xb8\x94\x73\x82\xb5\x82\xdc\x82\xb5\x82\xbd"/*@ "MP3変換用 D_CSampleGrabber の作成に失敗しました" @*/ );
		goto ERR ;
	}

	// サンプルグラバフィルタの設定
	cmt.majortype = MEDIATYPE_AUDIO ;
	cmt.subtype = D_MEDIASUBTYPE_PCM ;
	hr = SampleGrabber->SetAcceptedMediaType( &cmt ) ;
	if( hr != S_OK )
	{
		DXST_LOGFILE_ADDA( "\x83\x54\x83\x93\x83\x76\x83\x8b\x83\x4f\x83\x89\x83\x6f\x83\x74\x83\x42\x83\x8b\x83\x5e\x82\xcc\x83\x81\x83\x66\x83\x42\x83\x41\x83\x5e\x83\x43\x83\x76\x82\xcc\x90\xdd\x92\xe8\x82\xc9\x8e\xb8\x94\x73\x82\xb5\x82\xdc\x82\xb5\x82\xbd"/*@ "サンプルグラバフィルタのメディアタイプの設定に失敗しました" @*/ );
		goto ERR ;
	}

	// サンプルグラバフィルタをグラフに追加
	hr = GraphBuilder->AddFilter( SampleGrabber, L"SampleGrabber") ;
	if( hr != S_OK )
	{
		DXST_LOGFILE_ADDA( "\x83\x54\x83\x93\x83\x76\x83\x8b\x83\x4f\x83\x89\x83\x6f\x83\x74\x83\x42\x83\x8b\x83\x5e\x82\xcc\x83\x4f\x83\x89\x83\x74\x82\xd6\x82\xcc\x92\xc7\x89\xc1\x82\xc9\x8e\xb8\x94\x73\x82\xb5\x82\xdc\x82\xb5\x82\xbd"/*@ "サンプルグラバフィルタのグラフへの追加に失敗しました" @*/ );
		goto ERR ;
	}

	// メモリソースフィルタをグラフに追加
	hr = GraphBuilder->AddFilter( MemReader, L"MemFile" ) ;
	if( hr != S_OK )
	{
		DXST_LOGFILE_ADDA( "\x83\x81\x83\x82\x83\x8a\x83\x74\x83\x40\x83\x43\x83\x8b\x83\x74\x83\x42\x83\x8b\x83\x5e\x82\xcc\x83\x4f\x83\x89\x83\x74\x82\xd6\x82\xcc\x92\xc7\x89\xc1\x82\xc9\x8e\xb8\x94\x73\x82\xb5\x82\xdc\x82\xb5\x82\xbd"/*@ "メモリファイルフィルタのグラフへの追加に失敗しました" @*/ );
		goto ERR ;
	}

	// フィルタ連結
	hr = GraphBuilder->Render( MemReader->GetPin( 0 ) ) ;
	if( hr != S_OK )
	{
		DXST_LOGFILE_ADDA( "\x83\x4f\x83\x89\x83\x74\x82\xcc\x8d\x5c\x92\x7a\x82\xc9\x8e\xb8\x94\x73\x82\xb5\x82\xdc\x82\xb5\x82\xbd"/*@ "グラフの構築に失敗しました" @*/ );
		goto ERR ;
	}

	// ヌルレンダラーフィルタをグラフに追加
	hr = GraphBuilder->AddFilter( NullRenderer, L"File Renderer" ) ;
	if( hr != S_OK )
	{
		DXST_LOGFILE_ADDA( "\x83\x6b\x83\x8b\x83\x8c\x83\x93\x83\x5f\x83\x89\x81\x5b\x83\x74\x83\x42\x83\x8b\x83\x5e\x82\xcc\x83\x4f\x83\x89\x83\x74\x82\xd6\x82\xcc\x92\xc7\x89\xc1\x82\xc9\x8e\xb8\x94\x73\x82\xb5\x82\xdc\x82\xb5\x82\xbd"/*@ "ヌルレンダラーフィルタのグラフへの追加に失敗しました" @*/ );
		goto ERR ;
	}

	// ヌルレンダラーを元のレンダラーと交換する
	{
		D_IEnumFilters *EnumFilter ;
		D_IPin         *LastOutput = NULL ;
		D_IPin         *LastInput  = NULL ;
		D_IPin         *InputPin ;
		D_IPin         *OutputPin ;
		D_IBaseFilter  *TmpFilter ;

		// フィルターの列挙
		GraphBuilder->EnumFilters( &EnumFilter ) ;

		// フィルター群の中からレンダラーフィルタを捜す
		while( EnumFilter->Next( 1, &TmpFilter, NULL ) == S_OK )
		{
			InputPin  = _GetFilterPin( TmpFilter, D_PINDIR_INPUT  ) ;
			OutputPin = _GetFilterPin( TmpFilter, D_PINDIR_OUTPUT ) ;

			// 出力のないフィルターはレンダラーフィルタという判断
			if( InputPin != NULL && OutputPin == NULL )
			{
				// 入力ピンに繋がれている出力ピンを取得する
				InputPin->ConnectedTo( &LastOutput ) ;
				if( LastOutput != NULL )
				{
					// 現在のレンダラーフィルタと出力ピンの接続を解除
					GraphBuilder->Disconnect( LastOutput ) ;
					GraphBuilder->Disconnect( InputPin ) ;

					// 列挙用に取得したオブジェクトを解放
					InputPin->Release() ;
					TmpFilter->Release() ;
					break ;
				}
			}

			// 列挙用に取得したオブジェクトを解放
			if( InputPin  ) InputPin->Release() ;
			if( OutputPin ) OutputPin->Release() ; 
			TmpFilter->Release() ;
		}

		// 列挙用インターフェイスの解放
		EnumFilter->Release() ;

		// ヌルレンダラーの入力ピンを取得
		LastInput = _GetFilterPin( NullRenderer, D_PINDIR_INPUT ) ;

		// ヌルレンダラーの入力ピンと最初にレンダラーと接続していた出力ピンを接続
		hr = GraphBuilder->Connect( LastOutput, LastInput ) ;

		// 一時的なインターフェイスを解放
		LastOutput->Release() ;
		LastInput->Release() ;

		// エラー判定
		if( hr != S_OK )
		{
			DXST_LOGFILE_ADDA( "\x8a\xf9\x91\xb6\x83\x8c\x83\x93\x83\x5f\x83\x89\x81\x5b\x82\xf0Null\x83\x8c\x83\x93\x83\x5f\x83\x89\x81\x5b\x82\xc6\x8c\xf0\x8a\xb7\x82\xb7\x82\xe9\x8f\x88\x97\x9d\x82\xaa\x8e\xb8\x94\x73\x82\xb5\x82\xdc\x82\xb5\x82\xbd"/*@ "既存レンダラーをNullレンダラーと交換する処理が失敗しました" @*/ );
			goto ERR ;
		}
	}

	// オーディオフォーマットの取得
	_MEMSET( &csmpmt, 0, sizeof( csmpmt ) ) ;
	hr = SampleGrabber->GetConnectedMediaType( &csmpmt ) ;
	if( hr != S_OK )
	{
		DXST_LOGFILE_ADDA( "\x89\xb9\x90\xba\x82\xaa\x8a\xdc\x82\xdc\x82\xea\x82\xc4\x82\xa2\x82\xc8\x82\xa2\x82\xa9\x81\x41\x83\x49\x81\x5b\x83\x66\x83\x42\x83\x49\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xcc\x8e\xe6\x93\xbe\x82\xc9\x8e\xb8\x94\x73\x82\xb5\x82\xdc\x82\xb5\x82\xbd\x81\x42"/*@ "音声が含まれていないか、オーディオフォーマットの取得に失敗しました。" @*/ );
		goto ERR ;
	}
	if( csmpmt.formattype != FORMAT_WAVEFORMATEX )
	{
		DXST_LOGFILE_ADDA( "\x83\x54\x83\x7c\x81\x5b\x83\x67\x8a\x4f\x82\xcc\x83\x58\x83\x67\x83\x8a\x81\x5b\x83\x80\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xc5\x82\xb7\x81\x42"/*@ "サポート外のストリームフォーマットです。" @*/ );
		goto ERR ;;
	}
	smpfmt = ( WAVEFORMATEX * )csmpmt.pbFormat ;
	if( smpfmt->wFormatTag != WAVE_FORMAT_PCM )
	{
		DXST_LOGFILE_ADDA( "\x83\x54\x83\x7c\x81\x5b\x83\x67\x8a\x4f\x82\xcc\x83\x49\x81\x5b\x83\x66\x83\x42\x83\x49\x83\x74\x83\x48\x81\x5b\x83\x7d\x83\x62\x83\x67\x82\xc5\x82\xb7\x81\x42"/*@ "サポート外のオーディオフォーマットです。" @*/ );
		goto ERR ;
	}
	SoundConv->OutFormat.cbSize = 0 ;
	SoundConv->OutFormat.wFormatTag			= smpfmt->wFormatTag ;
	SoundConv->OutFormat.nChannels			= smpfmt->nChannels ;
	SoundConv->OutFormat.nBlockAlign		= smpfmt->nBlockAlign ;
	SoundConv->OutFormat.nSamplesPerSec		= smpfmt->nSamplesPerSec ;
	SoundConv->OutFormat.nAvgBytesPerSec	= SoundConv->OutFormat.nBlockAlign * smpfmt->nSamplesPerSec ;
	SoundConv->OutFormat.wBitsPerSample		= ( WORD )( smpfmt->nBlockAlign * 8 / smpfmt->nChannels ) ;

	// サンプリング準備
	hr = SampleGrabber->SetCallback( SoundCallback_DSMP3, SoundConv ) ;
	if( hr != S_OK )
	{
		DXST_LOGFILE_ADDA( "\x83\x52\x81\x5b\x83\x8b\x83\x6f\x83\x62\x83\x4e\x8a\xd6\x90\x94\x82\xf0\x90\xdd\x92\xe8\x82\xc5\x82\xab\x82\xdc\x82\xb9\x82\xf1"/*@ "コールバック関数を設定できません" @*/ );
		goto ERR ;;
	}

	// とりあえず元データの１０倍サイズのメモリを確保する
	dsmp3->PCMBufferSize = TempSize * 10 ;
	dsmp3->PCMBuffer = DXALLOC( dsmp3->PCMBufferSize ) ;
	if( dsmp3->PCMBuffer == NULL )
	{
		DXST_LOGFILE_ADDA( "PCM\x83\x66\x81\x5b\x83\x5e\x82\xf0\x8a\x69\x94\x5b\x82\xb7\x82\xe9\x83\x81\x83\x82\x83\x8a\x97\xcc\x88\xe6\x82\xcc\x8a\x6d\x95\xdb\x82\xc9\x8e\xb8\x94\x73\x82\xb5\x82\xdc\x82\xb5\x82\xbd"/*@ "PCMデータを格納するメモリ領域の確保に失敗しました" @*/ );
		goto ERR ;
	}

	// バッファ中の有効なデータのサイズを初期化
	dsmp3->PCMValidDataSize = 0 ;

	// メディアフィルターの取得
	hr = GraphBuilder->QueryInterface( IID_IMEDIAFILTER, ( void ** )&MediaFilter ) ;
	if( hr != S_OK )
	{
		DXST_LOGFILE_ADDA( "IMediaFilter \x82\xcc\x8e\xe6\x93\xbe\x82\xc9\x8e\xb8\x94\x73\x82\xb5\x82\xdc\x82\xb5\x82\xbd"/*@ "IMediaFilter の取得に失敗しました" @*/ );
		goto ERR ;
	}

	// 同期ソースを無くす
	MediaFilter->SetSyncSource( NULL ) ;

	// メディアコントロールの取得
	hr = GraphBuilder->QueryInterface( IID_IMEDIACONTROL, ( void ** )&MediaControl ) ;
	if( hr != S_OK )
	{
		DXST_LOGFILE_ADDA( "IMediaControl \x82\xcc\x8e\xe6\x93\xbe\x82\xc9\x8e\xb8\x94\x73\x82\xb5\x82\xdc\x82\xb5\x82\xbd"/*@ "IMediaControl の取得に失敗しました" @*/ );
		goto ERR ;
	}

	// メディアイベントの取得
	hr = GraphBuilder->QueryInterface( IID_IMEDIAEVENT, ( void ** )&MediaEvent ) ;
	if( hr != S_OK )
	{
		DXST_LOGFILE_ADDA( "IMediaEvent \x82\xcc\x8e\xe6\x93\xbe\x82\xc9\x8e\xb8\x94\x73\x82\xb5\x82\xdc\x82\xb5\x82\xbd"/*@ "IMediaEvent の取得に失敗しました" @*/ );
		goto ERR ;
	}

	// 再生開始
	MediaControl->Run() ;

	// 再生が終了するのを待つ
	for(;;)
	{
		long EvCode ;

		if( NS_ProcessMessage() != 0 )
		{
			DXST_LOGFILE_ADDA( "MP3\x95\xcf\x8a\xb7\x82\xcc\x93\x72\x92\x86\x82\xc5\x83\x5c\x83\x74\x83\x67\x82\xaa\x8f\x49\x97\xb9\x82\xb5\x82\xdc\x82\xb5\x82\xbd"/*@ "MP3変換の途中でソフトが終了しました" @*/ );
			goto ERR ;
		}

		// メモリ転送が完了したらループを抜ける
		if( MediaEvent->WaitForCompletion( 100, &EvCode ) == S_OK )
		{
			if( EvCode == D_EC_ERRORABORT )
			{
				DXST_LOGFILE_ADDA( "MP3\x95\xcf\x8a\xb7\x82\xcc\x93\x72\x92\x86\x82\xc5\x83\x47\x83\x89\x81\x5b\x82\xaa\x94\xad\x90\xb6\x82\xb5\x82\xdc\x82\xb5\x82\xbd"/*@ "MP3変換の途中でエラーが発生しました" @*/ );
				goto ERR ;
			}
			if( EvCode == D_EC_COMPLETE )
			{
				break ;
			}
		}
	}

	// バッファが解放されていたらエラー
	if( dsmp3->PCMBuffer == NULL )
	{
		DXST_LOGFILE_ADDA( "PCM\x83\x66\x81\x5b\x83\x5e\x82\xf0\x8a\x69\x94\x5b\x82\xb7\x82\xe9\x83\x81\x83\x82\x83\x8a\x97\xcc\x88\xe6\x82\xcc\x8d\xc4\x8a\x6d\x95\xdb\x82\xc9\x8e\xb8\x94\x73\x82\xb5\x82\xdc\x82\xb5\x82\xbd"/*@ "PCMデータを格納するメモリ領域の再確保に失敗しました" @*/ );
		goto ERR ;
	}

	// DirectShow を使用したＭＰ３再生の印を保存
	SoundConv->MethodType = SOUND_METHODTYPE_DSMP3 ;

	// 変換後のＰＣＭデータを一時的に保存するメモリ領域のサイズは１秒
	SoundConv->DestDataSize = ( int )SoundConv->OutFormat.nAvgBytesPerSec ;

	// 各種インターフェイスとメモリの解放
	if( MediaEvent   ){ MediaEvent->Release() ; MediaEvent = NULL ; }
	if( MediaFilter  ){ MediaFilter->Release() ; MediaFilter = NULL ; }
	if( MediaControl ){ MediaControl->Release() ; MediaControl = NULL ; }
	if( GraphBuilder ){ GraphBuilder->Release() ; GraphBuilder = NULL ; }
	if( SampleGrabber ){ SampleGrabber->DeleteInstance() ; SampleGrabber = NULL ; }
	if( NullRenderer ){ NullRenderer->Release() ; NullRenderer = NULL ; }
	if( MemStream ){ MemStream->DeleteInstance() ; MemStream = NULL ; }
	if( MemReader ){ MemReader->DeleteInstance() ; MemReader = NULL ; }
	if( TempBuffer ){ DXFREE( TempBuffer ) ; TempBuffer = NULL ; }

	// 正常終了
	return 0 ;

ERR :
	if( MediaEvent   ){ MediaEvent->Release() ; MediaEvent = NULL ; }
	if( MediaFilter ){ MediaFilter->Release() ; MediaFilter = NULL ; }
	if( MediaControl ){ MediaControl->Release() ; MediaControl = NULL ; }
	if( GraphBuilder ){ GraphBuilder->Release() ; GraphBuilder = NULL ; }
	if( SampleGrabber ){ SampleGrabber->DeleteInstance() ; SampleGrabber = NULL ; }
	if( NullRenderer ){ NullRenderer->Release() ; NullRenderer = NULL ; }
	if( MemStream ){ MemStream->DeleteInstance() ; MemStream = NULL ; }
	if( MemReader ){ MemReader->DeleteInstance() ; MemReader = NULL ; }
	if( TempBuffer ){ DXFREE( TempBuffer ) ; TempBuffer = NULL ; }
	if( dsmp3->PCMBuffer ){ DXFREE( dsmp3->PCMBuffer ) ; dsmp3->PCMBuffer = NULL ; }

	// エラー終了
	return -1 ;
}

#endif // DX_NON_MOVIE
#endif // DX_NON_DSHOW_MP3
#endif // DX_NON_DSHOW_MOVIE




//}












