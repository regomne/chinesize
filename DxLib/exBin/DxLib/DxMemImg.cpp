// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		メモリイメージ制御用プログラム
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

// インクルード----------------------------------------------------------------
#include "DxMemImg.h"
#include "DxLib.h"
#include "DxStatic.h"
#include "DxBaseFunc.h"
#include "DxBaseImage.h"
#include "DxMemory.h"
#include "DxMath.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義------------------------------------------------------------------

// データ宣言------------------------------------------------------------------

MEMIMGMANAGE MemImgManage ;

// 関数プロトタイプ宣言--------------------------------------------------------

// コード----------------------------------------------------------------------

// MEMIMG 管理データの初期化
extern int InitializeMemImgManage( void )
{
	// 乗算テーブル、割合テーブルの作成
	{
		int i, j, k ;

		for( i = 0 ; i < 256 ; i ++ )
		{
			for( j = -255 ; j < 256 ; j ++ )
			{
				MemImgManage.RateTable2[i][j+256] = i * j / 255 ;
			}
		}

		for( i = 0 ; i < 256 ; i ++ )
		{
			k = 0 ;
			for( j = 0 ; j < 256 ; j ++, k += i )
			{
				MemImgManage.RateTable[i][j] = ( DWORD )( k / 255 ) ;
			}
		}

		for( i = 0 ; i < 64 ; i ++ )
		{
			for( j = 0 ; j < 64 ; j ++ )
			{
				for( k = 0 ; k < 64 ; k ++ )
				{
					MemImgManage.RateTable16[i][j][k] = ( BYTE )( ( j * i + k * ( 63 - i ) ) / 63 ) ;
				}
			}
		}
	}

	// 終了
	return 0 ;
}

// デフォルトのカラータイプをセットする
extern	int		SetMemImgDefaultColorType( int ColorType )
{
	MemImgManage.DefaultColorType = ColorType ;

	return 0 ;
}

// MEMIMG用の描画範囲を設定する
extern	int		SetMemImgDrawArea( const RECT *DrawArea )
{
	MemImgManage.DrawArea = *DrawArea ;

	return 0 ;
}

// MEMIMG用のブレンドモードとブレンドパラメータの設定を行う
extern	int		SetMemImgDrawBlendMode( int BlendMode, int BlendParam )
{
	MemImgManage.BlendMode = BlendMode ;
	MemImgManage.BlendParam = BlendParam ;

	return 0 ;
}

// MEMIMG用の描画輝度
extern	int		SetMemImgDrawBright( DWORD Bright )
{
	MemImgManage.bDrawBright = Bright ;

	return 0 ;
}

// ブレンドグラフィック処理に必要なテーブルのセットアップを行う
extern	void	SetBlendGraphParamMemImg( int BorderParam, int BorderRange )
{
	int i, p, Length, Adjust ;
	
	BorderRange += 1 ;

	// ブレンド画像を適応する際に使用するブレンド率を予め計算しておく
	{
		Length = 256 * 256 / BorderRange ;
		p = ( Length + 256 ) - ( Length + 256 ) * BorderParam / 256 ;
		Adjust = -Length + p ;
		if( Adjust > 0 )
		{
			for( i = 0 ; i < 256 ; i ++ )
			{
				p = i * 256 / BorderRange + Adjust ;
				if( p > 255 ) p = 255 ;
				MemImgManage.BlendGraphTable[i] = (BYTE)p ;
			}
		}
		else
		{
			for( i = 0 ; i < 256 ; i ++ )
			{
				p = i * 256 / BorderRange + Adjust ;
				if( p > 255 ) p = 255 ;
				else
				if( p < 0   ) p = 0 ;
				MemImgManage.BlendGraphTable[i] = (BYTE)p ;
			}
		}
	}
}

// メモリ画像データを処理する関数

// (補助関数)MEMIMG 構造体に画像を読み込む( これを呼んだ場合 InitializeMemImg は必要なし )( 戻り値: -1=失敗  0=成功 )
extern int LoadImageToMemImg( const wchar_t *FilePath, MEMIMG *MemImg, DWORD TransColor, int ColorType, int UsePaletteFormat )
{
	BASEIMAGE BaseImage ;
	int Res, UseAlpha, UsePalette ;
	DWORD lTransColor ;

	// ファイルの読み込み
	Res = CreateGraphImage_plus_Alpha_WCHAR_T( FilePath, NULL, 0, LOADIMAGE_TYPE_FILE,
										 NULL, 0, LOADIMAGE_TYPE_FILE,
										 &BaseImage, NULL, FALSE, FALSE ) ;
	if( Res == -1 ) return -1 ;
	
	// フォーマットの判定
	if( ColorType == -1 )
	{
		ColorType  = MemImgManage.DefaultColorType ;
	}
	UseAlpha   = BaseImage.ColorData.AlphaWidth != 0  ? 1 : 0 ;
	UsePalette = BaseImage.ColorData.PixelByte  == 1  ? 1 : 0 ;
	if( UsePaletteFormat == 0 ) UsePalette = 0 ;

	// 透過色の算出
	if( UsePalette == 1 )
	{
		lTransColor = 0 ;
	}
	else
	{
		lTransColor = NS_GetColor3( GetMemImgColorData( ColorType, UseAlpha, UsePalette ),
									( int )( ( TransColor >> 16 ) & 0xff ),
									( int )( ( TransColor >> 8  ) & 0xff ),
									( int )( ( TransColor >> 0  ) & 0xff ) ) ;
	}

	// メモリイメージデータの初期化
	_MEMSET( MemImg, 0, sizeof( MEMIMG ) ) ;
	if( InitializeMemImg( MemImg, BaseImage.Width, BaseImage.Height, -1,
							lTransColor, ColorType, UsePalette, UseAlpha ) == -1 )
	{
		NS_ReleaseGraphImage( &BaseImage ) ;
		return -1 ;
	}
	
	// メモリイメージへデータを転送
	BltBaseImageToMemImg( &BaseImage, NULL, MemImg,
							0, 0,
							BaseImage.Width, BaseImage.Height,
							0, 0, TRUE ) ;
							
	// 元画像を解放
	NS_ReleaseGraphImage( &BaseImage ) ;

	// 終了
	return 0 ;
}


// (補助関数)描画対象用のメモリ画像を作成する( InitializeMemImg を簡略化しただけのもの、MemImgは０初期化しておく必要は無い )
extern int MakeMemImgScreen( MEMIMG *Img, int Width, int Height, int ColorType )
{
	_MEMSET( Img, 0, sizeof( MEMIMG ) ) ;
	if( ColorType == -1 ) ColorType = MemImgManage.DefaultColorType ; 
	return InitializeMemImg( Img, Width, Height, -1, 0, ColorType, 0, 0, 0 ) ;
}

// (補助関数)Ｚバッファのメモリイメージを作成する( InitializeMemImg を簡略化しただけのもの、MemImgは０クリアしておく必要は無い )
extern int MakeMemImgZBuffer( MEMIMG *Img, int Width, int Height )
{
	_MEMSET( Img, 0, sizeof( MEMIMG ) ) ;
	return InitializeMemImg( Img, Width, Height, -1, 0, 3, 0, 0, 0 ) ;
}

// (補助関数)MEMIMG 間でデータの転送を行う
extern void BltMemImg( MEMIMG *DestImg, const MEMIMG *SrcImg, const RECT *SrcRect, int DestX, int DestY )
{
	POINT DestPos ;
	RECT srcrect ;

	if( SrcRect == NULL )
	{
		SrcRect = &srcrect ;
		srcrect.left   = 0 ;
		srcrect.top    = 0 ;
		srcrect.right  = ( LONG )SrcImg->Width ;
		srcrect.bottom = ( LONG )SrcImg->Height ;
	}
	
	DestPos.x = DestX ;
	DestPos.y = DestY ;

	NS_GraphColorMatchBltVer2( DestImg->UseImage, ( int )DestImg->Base->Pitch, DestImg->Base->ColorDataP,
								SrcImg->UseImage, ( int )SrcImg->Base->Pitch,  SrcImg->Base->ColorDataP,
								NULL, 0, NULL,
								DestPos, SrcRect, FALSE,
								FALSE, 0,
								0, FALSE, FALSE ) ;
}

// ある MEMIMG の一部を使用する MEMIMG の情報を作成する(派生元が無効になったら派生 MEMIMG も使用不可になる)
extern void DerivationMemImg( MEMIMG *DestImg, MEMIMG *SrcImg, int SrcX, int SrcY, int Width, int Height )
{
	unsigned int Addr ;

	// 範囲から外れていたらエラー
	if( SrcX < 0 ||
		SrcY < 0 ||
		( int )SrcImg->Width  < SrcX + Width ||
		( int )SrcImg->Height < SrcY + Height ) return ;

	// 初期化フラグを立てる
	DestImg->InitializeCheck = MEMIMG_INITIALIZECODE ;

	// サイズをセット
	DestImg->Width  = ( unsigned int )Width ;
	DestImg->Height = ( unsigned int )Height ;

	// 使用するイメージのアドレスをセット
	Addr = SrcX * SrcImg->Base->ColorDataP->PixelByte + SrcY * SrcImg->Base->Pitch ;
	DestImg->UseImage = SrcImg->UseImage + Addr ;

	// アルファイメージがある場合はそれも
	if( SrcImg->Base->AlphaImage )
	{
		DestImg->UseAlphaImage = SrcImg->UseAlphaImage + Addr ;
	}
	else
	{
		DestImg->UseAlphaImage = NULL ;
	}

	// 基本情報のアドレスをセット
	DestImg->Base = SrcImg->Base ;

	// 参照数を増やす
	SrcImg->Base->RefCount ++ ;
}

// メモリ画像を初期化する、ゼロ初期化されている必要がある( 戻り値: -1=失敗  0=成功 )
extern int InitializeMemImg( MEMIMG *Img, int Width, int Height, int Pitch, DWORD TransColor, int ColorType, int UsePalette, int UseAlpha, int AnalysisFlag, const void *UserImage )
{
	int PixelByte, Pow2n, ShftNum, DefPitch ;
	MEMIMGBASE *Base ;

	// メモリ画像が既に初期化されていたら何もしない
	if( Img->InitializeCheck == MEMIMG_INITIALIZECODE ) return -1 ;

	// 共通情報格納用メモリの確保
	if( Img->Base == NULL )
	{
		Img->Base = ( MEMIMGBASE * )DXCALLOC( sizeof( MEMIMGBASE ) ) ;
		if( Img->Base == NULL ) return -1 ;
	}
	Base = Img->Base ;

	// パレットが有効だったらαチャンネルは無理
	if( UsePalette == 1 ) UseAlpha = 0 ;

	// Ｚバッファかグレースケールタイプだったらパレットもαチャンネルも無理
	if( ColorType == 2 || ColorType == 3 )
	{
		UsePalette = 0 ;
		UseAlpha = 0 ;
	}
	
	// 基本データをセット
	Base->RefCount          = 1 ;
	Base->BaseWidth         = Width ;
	Base->BaseHeight        = Height ;
	Base->UseTransColor		= 1 ;
	Base->TransColor        = TransColor ;
//	Base->TransPalette      = 0 ;
	Base->ColorType         = ColorType ;
	Base->UsePalette        = UsePalette ;
	Base->UseAlpha          = UseAlpha ;
	Base->AlphaImageValid   = 0 ;
	Base->AnalysisDataValid = 0 ;

	// パレット情報を格納するメモリ領域の確保
	if( UsePalette == 1 )
	{
		Base->Palette         = ( unsigned int * )DXCALLOC( sizeof( unsigned int ) * 256 * 2 ) ;
		Base->OriginalPalette = Base->Palette + 256 ;
	}
	else
	{
		Base->Palette         = NULL ;
		Base->OriginalPalette = NULL ;
	}

	// ピッチの算出
	{
		const int PixelByteTable[] = { 2, 4, 1, 2 } ;

		// １ピクセル辺りのバイト数をセット
		PixelByte = PixelByteTable[ColorType] ;
		
		// パレットが有効な場合は１バイト
		if( UsePalette == 1 ) PixelByte = 1 ;
		
		// １６ビット画像でもαチャンネルがある場合は４バイト
		if( UseAlpha == 1 && ColorType == 0 ) PixelByte = 4 ;
		
		// ピッチの算出
		DefPitch = Pitch == -1 ? PixelByte * Width : Pitch ;
		
		// ２のｎ乗のピッチを算出
		for( ShftNum = 0, Pow2n = 1 ; Pow2n < DefPitch ; Pow2n += Pow2n, ShftNum ++ ){}

		// ピッチのセット(ピッチは指定が無ければ１６の倍数にする)
		if( Pitch != -1 )
		{
			Base->Pitch = ( unsigned int )Pitch ;
		}
		else
		{
//			Base->Pitch = Pitch == -1 ? Pow2n : Pitch ;
			Base->Pitch = ( unsigned int )( PixelByte * Width ) ;
			Base->Pitch = ( Base->Pitch + 15 ) / 16 * 16 ;
		}

		// ピッチが２のｎ乗かどうかを判定
		Base->PitchPow2n = Base->Pitch == (DWORD)Pow2n ? ShftNum : -1 ;
	}

	// メモリイメージのカラーデータを構築する
	Base->ColorDataP = GetMemImgColorData( Base->ColorType, Base->UseAlpha, Base->UsePalette ) ;
/*	switch( Base->ColorType )
	{
	case 0 :
		if( Base->UseAlpha == 1 ) CreateColorData( &Base->ColorData, UsePalette == 1 ? 8 : 32, MEMIMG_X8A8R5G6B5_R, MEMIMG_X8A8R5G6B5_G, MEMIMG_X8A8R5G6B5_B, MEMIMG_X8A8R5G6B5_A ) ;
		else					 CreateColorData( &Base->ColorData, UsePalette == 1 ? 8 : 16, MEMIMG_R5G6B5_R, MEMIMG_R5G6B5_G, MEMIMG_R5G6B5_B, MEMIMG_R5G6B5_A ) ;
		break ;

	case 1 :
		if( Base->UseAlpha == 1 ) CreateColorData( &Base->ColorData, UsePalette == 1 ? 8 : 32, MEMIMG_ARGB8_R, MEMIMG_ARGB8_G, MEMIMG_ARGB8_B, MEMIMG_ARGB8_A ) ;
		else					 CreateColorData( &Base->ColorData, UsePalette == 1 ? 8 : 32, MEMIMG_XRGB8_R, MEMIMG_XRGB8_G, MEMIMG_XRGB8_B, MEMIMG_XRGB8_A ) ;
		break ;

	case 2 :
		Base->ColorData.ColorBitDepth = 8 ;
		Base->ColorData.PixelByte = 1 ;

		Base->ColorData.NoneMask = 0x00 ;

		Base->ColorData.AlphaLoc = 0 ;		Base->ColorData.AlphaMask = 0xff ;		Base->ColorData.AlphaWidth = 8 ;
		Base->ColorData.RedLoc   = 0 ;		Base->ColorData.RedMask   = 0xff ;		Base->ColorData.RedWidth   = 8 ;
		Base->ColorData.GreenLoc = 0 ;		Base->ColorData.GreenMask = 0xff ;		Base->ColorData.GreenWidth = 8 ;
		Base->ColorData.BlueLoc  = 0 ;		Base->ColorData.BlueMask  = 0xff ;		Base->ColorData.BlueWidth  = 8 ;
		break ;

	case 3 :
		Base->ColorData.ColorBitDepth = 16 ;
		Base->ColorData.PixelByte = 2 ;

		Base->ColorData.NoneMask = 0x00 ;

		Base->ColorData.AlphaLoc = 0 ;		Base->ColorData.AlphaMask = 0xffff ;		Base->ColorData.AlphaWidth = 16 ;
		Base->ColorData.RedLoc   = 0 ;		Base->ColorData.RedMask   = 0xffff ;		Base->ColorData.RedWidth   = 16 ;
		Base->ColorData.GreenLoc = 0 ;		Base->ColorData.GreenMask = 0xffff ;		Base->ColorData.GreenWidth = 16 ;
		Base->ColorData.BlueLoc  = 0 ;		Base->ColorData.BlueMask  = 0xffff ;		Base->ColorData.BlueWidth  = 16 ;
		break ;
	}
*/
	// イメージを格納するメモリ領域の確保
	if( UserImage == NULL )
	{
		Base->Image = ( unsigned char *)DXALLOC( Base->BaseHeight * Base->Pitch ) ;
		if( Base->Image == NULL ) return -1 ;
		
		Base->UserImageFlag = FALSE ;
	}
	else
	{
		Base->Image = ( BYTE * )UserImage ;
		Base->UserImageFlag = TRUE ;
	}

	// αチャンネルを適応したイメージを格納するメモリ領域の確保
	if( Base->UseAlpha != 0 && Base->AlphaImage == NULL && AnalysisFlag != FALSE )
	{
		Base->AlphaImage = ( unsigned char *)DXALLOC( Base->BaseHeight * Base->Pitch ) ;
		if( Base->AlphaImage == NULL ) return -1 ;
	}

	// 解析データを格納するメモリ領域の確保
	if( Base->ColorType != 2 && Base->AnalysisData == NULL && AnalysisFlag != FALSE )
	{
		// 最大のデータ量が収まるサイズを確保する
		Base->AnalysisData = ( unsigned char * )DXALLOC( ( size_t )( Base->BaseHeight * ( Base->BaseWidth + 5 ) ) ) ;
		if( Base->AnalysisData == NULL ) return -1 ;
	}

	// 派生データをセット
	Img->InitializeCheck = MEMIMG_INITIALIZECODE ;
	Img->Width           = ( unsigned int )Width ;
	Img->Height          = ( unsigned int )Height ;
	Img->UseImage        = Base->Image ;
	Img->UseAlphaImage   = Base->AlphaImage ;

	return 0 ;
}

// MEMIMG が有効かどうかを取得する
extern int CheckValidMemImg( const MEMIMG *Img )
{
	return Img->InitializeCheck == MEMIMG_INITIALIZECODE && Img->Base != NULL ;
}

// メモリ画像の後始末をする
extern void TerminateMemImg( MEMIMG *Img )
{
	// 後始末が終わっていたら何もいない
	if( Img->InitializeCheck != MEMIMG_INITIALIZECODE ) return ;

	// 基本データの参照数をデクリメントする
	Img->Base->RefCount -- ;

	// ０だったら基本データの解放
	if( Img->Base->RefCount == 0 )
	{
		MEMIMGBASE *Base ;

		Base = Img->Base ;

		// イメージ領域の開放
		if( Base->UserImageFlag == FALSE && Base->Image != NULL ) DXFREE( Base->Image ) ;
		Base->Image = NULL ;

		if( Base->AlphaImage != NULL ) DXFREE( Base->AlphaImage ) ;
		Base->AlphaImage = NULL ;

		if( Base->AnalysisData != NULL ) DXFREE( Base->AnalysisData ) ;
		Base->AnalysisData = NULL ;

		// パレット用メモリ領域の解放
		if( Base->Palette != NULL ) DXFREE( Base->Palette ) ;
		Base->Palette = NULL ;
		Base->OriginalPalette = NULL ;

		// 基本データ自体のメモリ領域も解放
		DXFREE( Base ) ;
	}
	Img->Base = NULL ;
	
	// 初期化チェックデータを無効にする
	Img->InitializeCheck = 0 ;
}

// BASEIMAGE イメージから MEMIMG イメージに画像データを転送する
extern void BltBaseImageToMemImg( const BASEIMAGE *RgbBaseImage, const BASEIMAGE *AlphaBaseImage, MEMIMG *MemImg,
								 int SrcX, int SrcY,
								 int Width, int Height,
								 int DestX, int DestY, int UseTransColorConvAlpha )
{
	POINT DestPos ;
	RECT SrcRect ;
	int i, j, AddPitch ;
	union
	{
		BYTE *DestBP ;
		WORD *DestWP ;
		DWORD *DestDP ;
	} ;
	union
	{
		BYTE *SrcBP ;
		WORD *SrcWP ;
		DWORD *SrcDP ;
	} ;
	DWORD ContNum, TransColor ;

	// 情報をセット	
	DestPos.x = DestX ;
	DestPos.y = DestY ;
	SrcRect.left = SrcX ;
	SrcRect.top = SrcY ;
	SrcRect.right = SrcX + Width ;
	SrcRect.bottom = SrcY + Height ;

	// 転送
	
	if( MemImg->Base->ColorType == 3 || MemImg->Base->ColorType == 2 )
	// Ｚバッファかブレンド用画像の場合
	{
		NS_GraphColorMatchBltVer2(  MemImg->UseImage,        ( int )MemImg->Base->Pitch, MemImg->Base->ColorDataP,
									RgbBaseImage->GraphData, ( int )RgbBaseImage->Pitch, &RgbBaseImage->ColorData,
									NULL, 0, NULL,
									DestPos, &SrcRect, FALSE,
									FALSE, 0,
									0, TRUE, TRUE ) ;
	}
	else
	// それ以外の場合
	{
		// αチャンネルのデータが別になっている場合は処理を分岐
		if( AlphaBaseImage != NULL )
		{
			if( NS_GraphColorMatchBltVer2(  MemImg->UseImage,        ( int )MemImg->Base->Pitch, MemImg->Base->ColorDataP,
											RgbBaseImage->GraphData, ( int )RgbBaseImage->Pitch, &RgbBaseImage->ColorData,
											AlphaBaseImage->GraphData, AlphaBaseImage->Pitch, &AlphaBaseImage->ColorData,
											DestPos, &SrcRect, FALSE,
											UseTransColorConvAlpha, MemImg->Base->TransColor,
											0, FALSE, FALSE ) < 0 ) return ;
		}
		else
		{
			int Result ;

			Result = NS_GraphColorMatchBltVer2( MemImg->UseImage,        ( int )MemImg->Base->Pitch, MemImg->Base->ColorDataP,
												RgbBaseImage->GraphData, ( int )RgbBaseImage->Pitch, &RgbBaseImage->ColorData,
												NULL, 0, NULL,
												DestPos, &SrcRect, FALSE,
												UseTransColorConvAlpha, MemImg->Base->TransColor,
												0, FALSE, FALSE ) ;
			if( Result < 0 ) return ;

			// パレットモードの場合はパレットの数を保存
			if( MemImg->Base->UsePalette == 1 )
				MemImg->Base->ColorNum = Result + 1 ;
		}
	}

	// パレットがある場合はパレットをコピーする
	if( MemImg->Base->UsePalette == 1 )
	{
		const COLORPALETTEDATA *SrcColor = RgbBaseImage->ColorData.Palette ;
		DestBP = (BYTE *)MemImg->Base->Palette ;

		if( MemImg->Base->ColorType == 0 )
		// 16ビットカラーの場合
		{
			for( i = 0 ; i < 256 ; i ++, SrcColor ++ )
			{
				*( DestWP ++ ) = ( WORD )( ( ( SrcColor->Red   >> 3 ) << 11 ) |
								           ( ( SrcColor->Green >> 2 ) << 5  ) |
								           (   SrcColor->Blue  >> 3 )         );
			}
			_MEMCPY( MemImg->Base->OriginalPalette, MemImg->Base->Palette, 256 * 2 );
		}
		else
		// 32ビットカラーの場合
		{
			for( i = 0 ; i < 256 ; i ++, SrcColor ++ )
			{
				*( DestDP ++ ) = ( DWORD )( ( SrcColor->Red   << 16 ) |
								            ( SrcColor->Green << 8  ) |
								              SrcColor->Blue          ) ;
			}
			_MEMCPY( MemImg->Base->OriginalPalette, MemImg->Base->Palette, 256 * 4 );
		}
	}

	// 透過色を使用するかどうかを保存
	if( ( MemImg->Base->ColorType == 0 || MemImg->Base->ColorType == 1 ) && MemImg->Base->UseAlpha == 0 )
	{
		MemImg->Base->UseTransColor = UseTransColorConvAlpha ? 1 : 0 ;
	}

	// 標準的なカラータイプで転送サイズが画像サイズと同じ場合のみ解析を行う
	MemImg->Base->AlphaImageValid = 0 ;
	MemImg->Base->AnalysisDataValid = 0 ;
	if( (DWORD)Width == MemImg->Width && (DWORD)Height == MemImg->Height &&
		( MemImg->Base->ColorType == 0 || MemImg->Base->ColorType == 1 ) && MemImg->Base->AnalysisData != NULL )
	{
		MemImg->Base->AnalysisDataValid = 1 ;

		TransColor = MemImg->Base->TransColor ;

		if( MemImg->Base->UseAlpha == 1 && MemImg->Base->AlphaImage != NULL )
		// αチャンネルを使用している場合はαチャンネルを適応した後のイメージデータを作成する
		{
			MemImg->Base->AlphaImageValid = 1 ;

			DestBP   = MemImg->UseAlphaImage ;
			SrcBP    = MemImg->UseImage ;
			AddPitch = ( int )( MemImg->Base->Pitch - MemImg->Width * 4 ) ;

			if( MemImg->Base->ColorType == 0 )
			// 16bit タイプの場合
			{
				i = ( int )MemImg->Height ;
				do{
					j = Width ;
					do{
						*DestDP = ( DWORD )( (((( *SrcWP & 0xf800 ) * SrcBP[2]) >> 8) & 0xf800) |
								             (((( *SrcWP & 0x7e0  ) * SrcBP[2]) >> 8) & 0x7e0 ) |
								             (((( *SrcWP & 0x1f   ) * SrcBP[2]) >> 8) & 0x1f  ) ) ;
						DestBP[2] = SrcBP[2] ;

						SrcDP ++ ;
						DestDP ++ ;
					}while( --j );
					SrcBP += AddPitch ;
					DestBP += AddPitch ;
				}while( -- i );
			}
			else
			// 32bit タイプの場合
			{
				i = ( int )MemImg->Height ;
				do{
					j = Width ;
					do{
						DestBP[0] = ( BYTE )( ( SrcBP[0] * SrcBP[3] ) >> 8 ) ;
						DestBP[1] = ( BYTE )( ( SrcBP[1] * SrcBP[3] ) >> 8 ) ;
						DestBP[2] = ( BYTE )( ( SrcBP[2] * SrcBP[3] ) >> 8 ) ;
						DestBP[3] = SrcBP[2] ;

						SrcDP ++ ;
						DestDP ++ ;
					}while( --j );
					SrcBP += AddPitch ;
					DestBP += AddPitch ;
				}while( -- i );
			}
		}

		DestBP   = MemImg->Base->AnalysisData ;
		SrcBP    = MemImg->UseImage ;
		AddPitch = ( int )( MemImg->Base->Pitch - MemImg->Base->ColorDataP->PixelByte * MemImg->Width ) ;

		if( MemImg->Base->UsePalette == 1 )
		// パレットを使用している場合の解析
		{
			i = ( int )MemImg->Height ;
			do{
				j = Width ;
				do{
					// 透過色か、非透過色かを判断した後、何ドット続いているかを調べる
					ContNum = 0 ;
					if( *( SrcBP ++ ) == TransColor ){
						while( ( ( (-- j) != 0 ) & ( (++ ContNum) < 64 ) ) && *SrcBP == TransColor ) SrcBP ++ ;
						*( DestBP ++ ) = (BYTE)( ( 0 << 6 ) | ( ContNum - 1 ) ) ;
					}else{
						while( ( ( (-- j) != 0 ) & ( (++ ContNum) < 64 ) ) && *SrcBP != TransColor ) SrcBP ++ ;
						*( DestBP ++ ) = (BYTE)( ( 1 << 6 ) | ( ContNum - 1 ) ) ;
					}
				}while( j != 0 );

				// 行の終端をセット
				*( DestBP ++ ) = 3 << 6 ;

				SrcBP += AddPitch ;
			}while( -- i );
		}
		else
		{
			if( MemImg->Base->UseAlpha == 0 )
			// αチャンネルが無い画像の場合
			{
				if( MemImg->Base->ColorType == 0 )
				// 16ビットの場合
				{
					i = ( int )MemImg->Height ;
					do{
						j = Width ;
						do{
							// 透過色か、非透過色かを判断した後、何ドット続いているかを調べる
							ContNum = 0 ;
							if( *( SrcWP ++ ) == TransColor ){
								while( ( ( (-- j) != 0 ) & ( (++ ContNum) < 64 ) ) && *SrcWP == TransColor ) SrcWP ++ ;
								*( DestBP ++ ) = (BYTE)( ( 0 << 6 ) | ( ContNum - 1 ) ) ;
							}else{
								while( ( ( (-- j) != 0 ) & ( (++ ContNum) < 64 ) ) && *SrcWP != TransColor ) SrcWP ++ ;
								*( DestBP ++ ) = (BYTE)( ( 1 << 6 ) | ( ContNum - 1 ) ) ;
							}
						}while( j != 0 );

						// 行の終端をセット
						*( DestBP ++ ) = 3 << 6 ;

						SrcBP += AddPitch ;
					}while( -- i );
				}
				else
				// 32ビットの場合
				{
					i = ( int )MemImg->Height ;
					do{
						j = Width ;
						do{
							// 透過色か、非透過色かを判断した後、何ドット続いているかを調べる
							ContNum = 0 ;
							if( *( SrcDP ++ ) == TransColor ){
								while( ( ( (-- j) != 0 ) & ( (++ ContNum) < 64 ) ) && *SrcDP == TransColor ) SrcDP ++ ;
								*( DestBP ++ ) = (BYTE)( ( 0 << 6 ) | ( ContNum - 1 ) ) ;
							}else{
								while( ( ( (-- j) != 0 ) & ( (++ ContNum) < 64 ) ) && *SrcDP != TransColor ) SrcDP ++ ;
								*( DestBP ++ ) = (BYTE)( ( 1 << 6 ) | ( ContNum - 1 ) ) ;
							}
						}while( j != 0 );

						// 行の終端をセット
						*( DestBP ++ ) = 3 << 6 ;

						SrcBP += AddPitch ;
					}while( -- i );
				}
			}
			else
			// αチャンネルが有る画像の場合
			{
				int Alpha ;

				// α値として参照するアドレスをセット
				Alpha = MemImg->Base->ColorType == 0 ? 2 : 3 ;

				i = ( int )MemImg->Height ;
				do{
					j = Width ;
					do{
						// 完全透明か、半透明か、透過色かを判別した後、何ドット続いているかを調べる
						ContNum = 0 ;
						switch( SrcDP ++, SrcBP[Alpha-4] )
						{
						case 0 :
							// 完全透明の場合
							while( ( ( (-- j) != 0 ) & ( (++ ContNum) < 64 ) ) && SrcBP[Alpha] == 0 ) SrcDP ++ ;
							*( DestBP ++ ) = (BYTE)( ( 0 << 6 ) | ( ContNum - 1 ) ) ;
							break ;

						case 0xff :
							// 完全不透明の場合
							while( ( ( (-- j) != 0 ) & ( (++ ContNum) < 64 ) ) && SrcBP[Alpha] == 0xff ) SrcDP ++ ;
							*( DestBP ++ ) = (BYTE)( ( 1 << 6 ) | ( ContNum - 1 ) ) ;
							break ;

						default :
							// 透明の場合
							while( ( ( (-- j) != 0 ) & ( (++ ContNum) < 64 ) ) && ( SrcBP[Alpha] != 0x00 && SrcBP[Alpha] != 0xff ) ) SrcDP ++ ;
							*( DestBP ++ ) = (BYTE)( ( 2 << 6 ) | ( ContNum - 1 ) ) ;
							break ;
						}
					}while( j != 0 );

					// 行の終端をセット
					*( DestBP ++ ) = 3 << 6 ;

					SrcBP += AddPitch ;
				}while( -- i );
			}
		}
	}
}

// 指定のフォーマットのカラーデータを得る
extern COLORDATA *GetMemImgColorData( int ColorType, int UseAlpha, int UsePalette )
{
	// 各フォーマットのカラーデータを初期化していなかったら初期化して、そのアドレスを返す
	switch( ColorType )
	{
	case 0 :
		if( UseAlpha ){
			if( UsePalette ){
				static COLORDATA ColorData ;
				static int Init = 0 ;
				
				if( Init == 0 ){
					Init = 1 ;
					CreateColorData( &ColorData, 8, MEMIMG_X8A8R5G6B5_R, MEMIMG_X8A8R5G6B5_G, MEMIMG_X8A8R5G6B5_B, MEMIMG_X8A8R5G6B5_A ) ;
				}
				return &ColorData ;
			}else{
				static COLORDATA ColorData ;
				static int Init = 0 ;
				
				if( Init == 0 ){
					Init = 1 ;
					CreateColorData( &ColorData, 32, MEMIMG_X8A8R5G6B5_R, MEMIMG_X8A8R5G6B5_G, MEMIMG_X8A8R5G6B5_B, MEMIMG_X8A8R5G6B5_A ) ;
				}
				return &ColorData ;
			}
		}else{
			if( UsePalette )
			{
				static COLORDATA ColorData ;
				static int Init = 0 ;
				
				if( Init == 0 ){
					Init = 1 ;
					 CreateColorData( &ColorData, 8, MEMIMG_R5G6B5_R, MEMIMG_R5G6B5_G, MEMIMG_R5G6B5_B, MEMIMG_R5G6B5_A ) ;
				}
				return &ColorData ;
			}else{
				static COLORDATA ColorData ;
				static int Init = 0 ;
				
				if( Init == 0 ){
					Init = 1 ;
					 CreateColorData( &ColorData, UsePalette == 1 ? 8 : 16, MEMIMG_R5G6B5_R, MEMIMG_R5G6B5_G, MEMIMG_R5G6B5_B, MEMIMG_R5G6B5_A ) ;
				}
				return &ColorData ;
			}
		}

	case 1 :
		if( UseAlpha ){
			if( UsePalette ){
				static COLORDATA ColorData ;
				static int Init = 0 ;
				
				if( Init == 0 ){
					Init = 1 ;
					 CreateColorData( &ColorData, 8, MEMIMG_ARGB8_R, MEMIMG_ARGB8_G, MEMIMG_ARGB8_B, MEMIMG_ARGB8_A ) ;
				}
				return &ColorData ;
			}else{
				static COLORDATA ColorData ;
				static int Init = 0 ;
				
				if( Init == 0 ){
					Init = 1 ;
					CreateColorData( &ColorData, 32, MEMIMG_ARGB8_R, MEMIMG_ARGB8_G, MEMIMG_ARGB8_B, MEMIMG_ARGB8_A ) ;
				}
				return &ColorData ;
			}
		}else{
			if( UsePalette ){
				static COLORDATA ColorData ;
				static int Init = 0 ;
				
				if( Init == 0 ){
					Init = 1 ;
					CreateColorData( &ColorData, 8, MEMIMG_XRGB8_R, MEMIMG_XRGB8_G, MEMIMG_XRGB8_B, MEMIMG_XRGB8_A ) ;
				}
				return &ColorData ;
			}else{
				static COLORDATA ColorData ;
				static int Init = 0 ;
				
				if( Init == 0 ){
					Init = 1 ;
					CreateColorData( &ColorData, 32, MEMIMG_XRGB8_R, MEMIMG_XRGB8_G, MEMIMG_XRGB8_B, MEMIMG_XRGB8_A ) ;
				}
				return &ColorData ;
			}
		}

	case 2 :
		{
			static COLORDATA ColorData ;
			static int Init = 0 ;
			
			if( Init == 0 )
			{
				Init = 1 ;
				ColorData.ColorBitDepth = 8 ;
				ColorData.PixelByte = 1 ;

				ColorData.NoneMask = 0x00 ;

				ColorData.AlphaLoc = 0 ;		ColorData.AlphaMask = 0xff ;		ColorData.AlphaWidth = 8 ;
				ColorData.RedLoc   = 0 ;		ColorData.RedMask   = 0xff ;		ColorData.RedWidth   = 8 ;
				ColorData.GreenLoc = 0 ;		ColorData.GreenMask = 0xff ;		ColorData.GreenWidth = 8 ;
				ColorData.BlueLoc  = 0 ;		ColorData.BlueMask  = 0xff ;		ColorData.BlueWidth  = 8 ;
			}
			return &ColorData ;
		}

	case 3 :
		{
			static COLORDATA ColorData ;
			static int Init = 0 ;
			
			if( Init == 0 )
			{
				Init = 1 ;
				ColorData.ColorBitDepth = 16 ;
				ColorData.PixelByte = 2 ;

				ColorData.NoneMask = 0x00 ;

				ColorData.AlphaLoc = 0 ;		ColorData.AlphaMask = 0xffff ;		ColorData.AlphaWidth = 16 ;
				ColorData.RedLoc   = 0 ;		ColorData.RedMask   = 0xffff ;		ColorData.RedWidth   = 16 ;
				ColorData.GreenLoc = 0 ;		ColorData.GreenMask = 0xffff ;		ColorData.GreenWidth = 16 ;
				ColorData.BlueLoc  = 0 ;		ColorData.BlueMask  = 0xffff ;		ColorData.BlueWidth  = 16 ;
			}
			return &ColorData ;
		}
	}
	
	// ここに来たら対応していないカラーフォーマットということ
	return NULL ;
}

/*
#ifdef __WINDOWS__

// DDPIXELFORMATデータを作成する
extern int CreatePixelFormat( D_DDPIXELFORMAT *PixelFormatBuf, int ColorBitDepth,
								 DWORD RedMask, DWORD GreenMask, DWORD BlueMask, DWORD AlphaMask )
{
	_MEMSET( PixelFormatBuf, 0, sizeof( D_DDPIXELFORMAT ) ) ;
	PixelFormatBuf->dwFlags				= D_DDPF_RGB ;
	PixelFormatBuf->dwSize				= sizeof( D_DDPIXELFORMAT ) ;
	PixelFormatBuf->dwRGBBitCount		= ( DWORD )ColorBitDepth ;
	PixelFormatBuf->dwRBitMask			= RedMask ;
	PixelFormatBuf->dwGBitMask			= GreenMask ;
	PixelFormatBuf->dwBBitMask			= BlueMask ;
	PixelFormatBuf->dwRGBAlphaBitMask = AlphaMask ;

	// 終了
	return 0 ;
}

// 指定のフォーマットのカラーデータを得る
extern D_DDPIXELFORMAT *GetMemImgPixelFormat( int ColorType, int UseAlpha, int UsePalette )
{
	// 各フォーマットのカラーデータを初期化していなかったら初期化して、そのアドレスを返す
	switch( ColorType )
	{
	case 0 :
		if( UseAlpha ){
			if( UsePalette ){
				static D_DDPIXELFORMAT PixelFormat ;
				static int Init = 0 ;
				
				if( Init == 0 ){
					Init = 1 ;
					CreatePixelFormat( &PixelFormat, 8, MEMIMG_X8A8R5G6B5_R, MEMIMG_X8A8R5G6B5_G, MEMIMG_X8A8R5G6B5_B, MEMIMG_X8A8R5G6B5_A ) ;
				}
				return &PixelFormat ;
			}else{
				static D_DDPIXELFORMAT PixelFormat ;
				static int Init = 0 ;
				
				if( Init == 0 ){
					Init = 1 ;
					CreatePixelFormat( &PixelFormat, 32, MEMIMG_X8A8R5G6B5_R, MEMIMG_X8A8R5G6B5_G, MEMIMG_X8A8R5G6B5_B, MEMIMG_X8A8R5G6B5_A ) ;
				}
				return &PixelFormat ;
			}
		}else{
			if( UsePalette )
			{
				static D_DDPIXELFORMAT PixelFormat ;
				static int Init = 0 ;
				
				if( Init == 0 ){
					Init = 1 ;
					 CreatePixelFormat( &PixelFormat, 8, MEMIMG_R5G6B5_R, MEMIMG_R5G6B5_G, MEMIMG_R5G6B5_B, MEMIMG_R5G6B5_A ) ;
				}
				return &PixelFormat ;
			}else{
				static D_DDPIXELFORMAT PixelFormat ;
				static int Init = 0 ;
				
				if( Init == 0 ){
					Init = 1 ;
					 CreatePixelFormat( &PixelFormat, UsePalette == 1 ? 8 : 16, MEMIMG_R5G6B5_R, MEMIMG_R5G6B5_G, MEMIMG_R5G6B5_B, MEMIMG_R5G6B5_A ) ;
				}
				return &PixelFormat ;
			}
		}
		break ;

	case 1 :
		if( UseAlpha ){
			if( UsePalette ){
				static D_DDPIXELFORMAT PixelFormat ;
				static int Init = 0 ;
				
				if( Init == 0 ){
					Init = 1 ;
					 CreatePixelFormat( &PixelFormat, 8, MEMIMG_ARGB8_R, MEMIMG_ARGB8_G, MEMIMG_ARGB8_B, MEMIMG_ARGB8_A ) ;
				}
				return &PixelFormat ;
			}else{
				static D_DDPIXELFORMAT PixelFormat ;
				static int Init = 0 ;
				
				if( Init == 0 ){
					Init = 1 ;
					CreatePixelFormat( &PixelFormat, 32, MEMIMG_ARGB8_R, MEMIMG_ARGB8_G, MEMIMG_ARGB8_B, MEMIMG_ARGB8_A ) ;
				}
				return &PixelFormat ;
			}
		}else{
			if( UsePalette ){
				static D_DDPIXELFORMAT PixelFormat ;
				static int Init = 0 ;
				
				if( Init == 0 ){
					Init = 1 ;
					CreatePixelFormat( &PixelFormat, 8, MEMIMG_XRGB8_R, MEMIMG_XRGB8_G, MEMIMG_XRGB8_B, MEMIMG_XRGB8_A ) ;
				}
				return &PixelFormat ;
			}else{
				static D_DDPIXELFORMAT PixelFormat ;
				static int Init = 0 ;
				
				if( Init == 0 ){
					Init = 1 ;
					CreatePixelFormat( &PixelFormat, 32, MEMIMG_XRGB8_R, MEMIMG_XRGB8_G, MEMIMG_XRGB8_B, MEMIMG_XRGB8_A ) ;
				}
				return &PixelFormat ;
			}
		}
		break ;

	case 2 :
		{
			static D_DDPIXELFORMAT PixelFormat ;
			static int Init = 0 ;
			
			if( Init == 0 )
			{
				Init = 1 ;
				PixelFormat.dwRGBBitCount = 8 ;

				PixelFormat.dwRBitMask         = 0xff ;
				PixelFormat.dwGBitMask         = 0xff ;
				PixelFormat.dwBBitMask         = 0xff ;
				PixelFormat.dwRGBAlphaBitMask  = 0xff ;
			}
			return &PixelFormat ;
		}
		break ;

	case 3 :
		{
			static D_DDPIXELFORMAT PixelFormat ;
			static int Init = 0 ;
			
			if( Init == 0 )
			{
				Init = 1 ;
				PixelFormat.dwRGBBitCount = 16 ;

				PixelFormat.dwRBitMask         = 0xffff ;
				PixelFormat.dwGBitMask         = 0xffff ;
				PixelFormat.dwBBitMask         = 0xffff ;
				PixelFormat.dwRGBAlphaBitMask  = 0xffff ;
			}
			return &PixelFormat ;
		}
		break ;
	}
	
	// ここに来たら対応していないカラーフォーマットということ
	return NULL ;
}

#endif // __WINDOWS__
*/

// MEMIMG イメージから BASEIMAGE イメージに画像データを転送する
extern void BltMemImgToBaseImage( BASEIMAGE *BaseImage, const MEMIMG *MemImg, int SrcX, int SrcY, int Width, int Height, int DestX, int DestY, DWORD TransColor, int TransFlag )
{
	POINT DestPos ;
	RECT SrcRect ;

	// 標準的なカラータイプ以外は転送できない
	if( MemImg->Base->ColorType != 0 && MemImg->Base->ColorType != 1 ) return ;

	// αチャンネル付きイメージやパレット付きイメージを転送することは出来ない
	if( MemImg->Base->UsePalette == 1 || MemImg->Base->UseAlpha == 1 ) return ;
	
	// 情報をセット	
	DestPos.x      = DestX ;
	DestPos.y      = DestY ;
	SrcRect.left   = SrcX ;
	SrcRect.top    = SrcY ;
	SrcRect.right  = SrcX + Width ;
	SrcRect.bottom = SrcY + Height ;
	
	// 転送
	NS_GraphColorMatchBltVer2( 	BaseImage->GraphData, BaseImage->Pitch,           &BaseImage->ColorData,
								MemImg->UseImage,     ( int )MemImg->Base->Pitch, MemImg->Base->ColorDataP,
								NULL, 0, NULL,
								DestPos, &SrcRect, FALSE,
								FALSE, TransColor,
								0, FALSE,
								FALSE, TransFlag ) ;
}

// イメージを初期化する
extern void ClearMemImg( MEMIMG *MemImg, const RECT *FillArea, unsigned int Color )
{
	int wtemp, Width, Height, AddPitch ;
	RECT ClipRect, FArea ;
	union
	{
		BYTE *DestBP ;
		WORD *DestWP ;
		DWORD *DestDP ;
	} ;

	if( FillArea == NULL )
	{
		FArea.left   = 0 ;
		FArea.top    = 0 ;
		FArea.right  = ( LONG )MemImg->Width ;
		FArea.bottom = ( LONG )MemImg->Height ;
	}
	else
	{
		ClipRect.left   = 0 ;
		ClipRect.top    = 0 ;
		ClipRect.right  = ( LONG )MemImg->Width ;
		ClipRect.bottom = ( LONG )MemImg->Height ;
		FArea = *FillArea ;
		RectClipping_Inline( &FArea, &ClipRect ) ;
	}

	Width    = FArea.right  - FArea.left ;
	Height   = FArea.bottom - FArea.top  ;
	DestBP   = MemImg->UseImage + FArea.left * MemImg->Base->ColorDataP->PixelByte + FArea.top * MemImg->Base->Pitch ;
	AddPitch =( int )(  MemImg->Base->Pitch - Width   * MemImg->Base->ColorDataP->PixelByte ) ;

	if( Width == 0 || Height == 0 )
	{
		return ;
	}

	switch( MemImg->Base->ColorDataP->PixelByte )
	{
	case 1 :
		do{
			wtemp = Width ;
			do{
				*( DestBP ++ ) = (BYTE)Color ;
			}while( -- wtemp ) ;
			DestBP += AddPitch ;
		}while( -- Height ) ;
		break ;

	case 2 :
		do{
			wtemp = Width ;
			do{
				*( DestWP ++ ) = (WORD)Color ;
			}while( -- wtemp ) ;
			DestBP += AddPitch ;
		}while( -- Height ) ;
		break ;

	case 4 :
		do{
			wtemp = Width ;
			do{
				*( DestDP ++ ) = Color ;
			}while( -- wtemp ) ;
			DestBP += AddPitch ;
		}while( -- Height ) ;
		break ;
	}
}

// イメージ中の指定座標の色を取得
extern DWORD GetPixelColorMemImg( const MEMIMG *MemImg, int x, int y )
{
	switch( MemImg->Base->ColorDataP->PixelByte )
	{
	case 1 : return *(           (BYTE *)MemImg->UseImage +   x        + y * MemImg->Base->Pitch )  ;
	case 2 : return *((WORD  *)( (BYTE *)MemImg->UseImage + ( x << 1 ) + y * MemImg->Base->Pitch )) ;
	case 4 : return *((DWORD *)( (BYTE *)MemImg->UseImage + ( x << 2 ) + y * MemImg->Base->Pitch )) ;
	}
	return 0 ;
}

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

