// ----------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		文字コード関係プログラム
// 
// 				Ver 3.20c
// 
// ----------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

// インクルード----------------------------------------------------------------
#include "DxMemory.h"
#include "DxBaseFunc.h"
#include "DxFile.h"
#include "DxArchive_.h"
#include "DxLog.h"
#include "DxUseCLib.h"
#include "DxChar.h"
#include <float.h>
#include <math.h>
//#include "DxSystem.h"


#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義------------------------------------------------------------------

// 書式文字列のサイズ指定プレフィックス
#define PRINTF_SIZE_PREFIX_h		(0)
#define PRINTF_SIZE_PREFIX_l		(1)
#define PRINTF_SIZE_PREFIX_ll		(2)
#define PRINTF_SIZE_PREFIX_w		(3)
#define PRINTF_SIZE_PREFIX_I		(4)
#define PRINTF_SIZE_PREFIX_I32		(5)
#define PRINTF_SIZE_PREFIX_I64		(6)

// 書式文字列の型指定
#define PRINTF_TYPE_c				(0)
#define PRINTF_TYPE_C				(1)
#define PRINTF_TYPE_d				(2)
#define PRINTF_TYPE_i				(3)
#define PRINTF_TYPE_o				(4)
#define PRINTF_TYPE_u				(5)
#define PRINTF_TYPE_x				(6)
#define PRINTF_TYPE_X				(7)
#define PRINTF_TYPE_e				(8)
#define PRINTF_TYPE_E				(9)
#define PRINTF_TYPE_f				(10)
#define PRINTF_TYPE_g				(11)
#define PRINTF_TYPE_G				(12)
#define PRINTF_TYPE_a				(13)
#define PRINTF_TYPE_A				(14)
#define PRINTF_TYPE_n				(15)
#define PRINTF_TYPE_p				(16)
#define PRINTF_TYPE_s				(17)
#define PRINTF_TYPE_S				(18)
#define PRINTF_TYPE_Z				(19)
#define PRINTF_TYPE_NUM				(20)

// 構造体宣言------------------------------------------------------------------

// 関数宣言--------------------------------------------------------------------

// UTF-16と各文字コードの対応表のセットアップを行う
static void SetupCharCodeTableInfo( CHARCODETABLEINFO *TableInfo, BYTE *PressTable, int *ConvertFlag, int IsSingleCharType = FALSE ) ;

// データ宣言------------------------------------------------------------------

static BYTE NumberToCharTable[ 2 ][ 16 ] =
{
	{
		'0', '1', '2', '3', '4', '5', '6', '7',
		'8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
	},

	{
		'0', '1', '2', '3', '4', '5', '6', '7',
		'8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
	}
} ;

static BYTE FloatErrorStr_QNAN[ 7 ][ 7 ] =
{
	{ '1',   0,   0,   0,   0,   0,   0 },
	{ '1', '.',   0,   0,   0,   0,   0 },
	{ '1', '.', '$',   0,   0,   0,   0 },
	{ '1', '.', '#', 'R',   0,   0,   0 },
	{ '1', '.', '#', 'Q', 'O',   0,   0 },
	{ '1', '.', '#', 'Q', 'N', 'B',   0 },
	{ '1', '.', '#', 'Q', 'N', 'A', 'N' },
} ;

static BYTE FloatErrorStr_INF[ 6 ][ 7 ] =
{
	{ '1',   0,   0,   0,   0,   0,   0 },
	{ '1', '.',   0,   0,   0,   0,   0 },
	{ '1', '.', '$',   0,   0,   0,   0 },
	{ '1', '.', '#', 'J',   0,   0,   0 },
	{ '1', '.', '#', 'I', 'O',   0,   0 },
	{ '1', '.', '#', 'I', 'N', 'F',   0 },
} ;

// static BYTE FloatErrorStr_IND[ 6 ][ 7 ] =
// {
// 	{ '1',   0,   0,   0,   0,   0,   0 },
// 	{ '1', '.',   0,   0,   0,   0,   0 },
// 	{ '1', '.', '$',   0,   0,   0,   0 },
// 	{ '1', '.', '#', 'J',   0,   0,   0 },
// 	{ '1', '.', '#', 'I', 'O',   0,   0 },
// 	{ '1', '.', '#', 'I', 'N', 'D',   0 },
// } ;

extern int  CP932ToUTF16TableConvert ;
extern BYTE CP932ToUTF16Table[] ;
extern int  CP936ToUTF16TableConvert ;
extern BYTE CP936ToUTF16Table[] ;
extern int  CP949ToUTF16TableConvert ;
extern BYTE CP949ToUTF16Table[] ;
extern int  CP950ToUTF16TableConvert ;
extern BYTE CP950ToUTF16Table[] ;
extern int  CP1252ToUTF16TableConvert ;
extern BYTE CP1252ToUTF16Table[] ;
extern int  ISO_IEC_8859_15ToUTF16TableConvert ;
extern BYTE ISO_IEC_8859_15ToUTF16Table[] ;

CHARCODESYSTEM g_CharCodeSystem ;

// プログラムコード------------------------------------------------------------

// UTF-16と各文字コードの対応表のセットアップを行う
static void SetupCharCodeTableInfo( CHARCODETABLEINFO *TableInfo, BYTE *PressTable, int *ConvertFlag, int IsSingleCharType )
{
	DWORD i ;

	if( *ConvertFlag == 0 )
	{
		Char128ToBin( PressTable, PressTable ) ;
		*ConvertFlag = 1 ;
	}
	DXA_Decode( PressTable, TableInfo->MultiByteToUTF16 ) ;

	if( IsSingleCharType )
	{
		for( i = 0 ; i < 0x100 ; i ++ )
		{
			TableInfo->UTF16ToMultiByte[ TableInfo->MultiByteToUTF16[ i ] ] = ( WORD )i ;
		}
	}
	else
	{
		for( i = 0 ; i < 0x10000 ; i ++ )
		{
			TableInfo->UTF16ToMultiByte[ TableInfo->MultiByteToUTF16[ i ] ] = ( WORD )i ;
		}
	}
}

extern int InitCharCode( void )
{
	// 既に初期化済みの場合は何もせず終了
	if( g_CharCodeSystem.InitializeFlag )
	{
		return 0 ;
	}

	// キャラクタコード対応表セットアップ
	SetupCharCodeTableInfo( &g_CharCodeSystem.CharCodeCP932Info,           CP932ToUTF16Table,			&CP932ToUTF16TableConvert ) ;
	SetupCharCodeTableInfo( &g_CharCodeSystem.CharCodeCP936Info,           CP936ToUTF16Table,			&CP936ToUTF16TableConvert ) ;
	SetupCharCodeTableInfo( &g_CharCodeSystem.CharCodeCP949Info,           CP949ToUTF16Table,			&CP949ToUTF16TableConvert ) ;
	SetupCharCodeTableInfo( &g_CharCodeSystem.CharCodeCP950Info,           CP950ToUTF16Table,			&CP950ToUTF16TableConvert ) ;
	SetupCharCodeTableInfo( &g_CharCodeSystem.CharCodeCP1252Info,          CP1252ToUTF16Table,			&CP1252ToUTF16TableConvert, TRUE ) ;
	SetupCharCodeTableInfo( &g_CharCodeSystem.CharCodeISO_IEC_8859_15Info, ISO_IEC_8859_15ToUTF16Table, &ISO_IEC_8859_15ToUTF16TableConvert, TRUE ) ;

	// 初期化フラグを立てる
	g_CharCodeSystem.InitializeFlag = TRUE ;

	// 正常終了
	return 0 ;
}



// 指定の文字コード形式の情報最少サイズを取得する( 戻り値：バイト数 )
__inline int GetCharCodeFormatUnitSize_inline( int CharCodeFormat )
{
	// 対応していない文字コード形式の場合は何もせず終了
	switch( CharCodeFormat )
	{
	case DX_CHARCODEFORMAT_SHIFTJIS :
	case DX_CHARCODEFORMAT_GB2312 :
	case DX_CHARCODEFORMAT_WINDOWS_1252 :
	case DX_CHARCODEFORMAT_ISO_IEC_8859_15 :
	case DX_CHARCODEFORMAT_UHC :
	case DX_CHARCODEFORMAT_BIG5 :
	case DX_CHARCODEFORMAT_ASCII :
		return 1 ;

	case DX_CHARCODEFORMAT_UTF16LE :
	case DX_CHARCODEFORMAT_UTF16BE :
		return 2 ;

	case DX_CHARCODEFORMAT_UTF8 :
		return 1 ;

	case DX_CHARCODEFORMAT_UTF32LE :
	case DX_CHARCODEFORMAT_UTF32BE :
		return 4 ;

	default :
		return -1 ;
	}
}

// 指定の文字コード形式の情報最少サイズを取得する( 戻り値：バイト数 )
extern int GetCharCodeFormatUnitSize( int CharCodeFormat )
{
	return GetCharCodeFormatUnitSize_inline( CharCodeFormat ) ;
}

// １文字のバイト数を取得する( 戻り値：１文字のバイト数 )
__inline int GetCharBytes_inline( const char *CharCode, int CharCodeFormat )
{
	switch( CharCodeFormat )
	{
	case DX_CHARCODEFORMAT_SHIFTJIS :
		return CHECK_SHIFTJIS_2BYTE( ( ( BYTE * )CharCode )[ 0 ] ) ? 2 : 1 ;
	//	return ( ( ( BYTE * )CharCode )[ 0 ] >= 0x81 && ( ( BYTE * )CharCode )[ 0 ] <= 0x9F ) || ( ( ( BYTE * )CharCode )[ 0 ] >= 0xE0 && ( ( BYTE * )CharCode )[ 0 ] <= 0xFC ) ;

	case DX_CHARCODEFORMAT_ASCII :
	case DX_CHARCODEFORMAT_WINDOWS_1252 :
	case DX_CHARCODEFORMAT_ISO_IEC_8859_15 :
		return 1 ;

	case DX_CHARCODEFORMAT_GB2312 :
	case DX_CHARCODEFORMAT_UHC :
	case DX_CHARCODEFORMAT_BIG5 :
		return ( ( ( ( BYTE * )CharCode )[ 0 ] & 0x80 ) != 0 ) ? 2 : 1 ;

	case DX_CHARCODEFORMAT_UTF16LE :
		return ( ( ( ( BYTE * )CharCode )[ 0 ] | ( ( ( BYTE * )CharCode )[ 1 ] << 8 ) ) & 0xfc00 ) == 0xd800 ? 4 : 2 ;

	case DX_CHARCODEFORMAT_UTF16BE :
		return ( ( ( ( ( BYTE * )CharCode )[ 0 ] << 8 ) | ( ( BYTE * )CharCode )[ 1 ] ) & 0xfc00 ) == 0xd800 ? 4 : 2 ;

	case DX_CHARCODEFORMAT_UTF8 :
		if( ( ( ( BYTE * )CharCode )[ 0 ] & 0x80 ) == 0x00 )
		{
			return 1 ;
		}

		if( ( ( ( BYTE * )CharCode )[ 0 ] & 0xe0 ) == 0xc0 )
		{
			return 2 ;
		}

		if( ( ( ( BYTE * )CharCode )[ 0 ] & 0xf0 ) == 0xe0 )
		{
			return 3 ;
		}

		if( ( ( ( BYTE * )CharCode )[ 0 ] & 0xf8 ) == 0xf0 )
		{
			return 4 ;
		}

		if( ( ( ( BYTE * )CharCode )[ 0 ] & 0xfc ) == 0xf8 )
		{
			return 5 ;
		}

		if( ( ( ( BYTE * )CharCode )[ 0 ] & 0xfe ) == 0xfc )
		{
			return 6 ;
		}
		break ;

	case DX_CHARCODEFORMAT_UTF32LE :
	case DX_CHARCODEFORMAT_UTF32BE :
		return 4 ;
	}

	return -1 ;
}

// １文字のバイト数を取得する( 戻り値：１文字のバイト数 )
extern int GetCharBytes_( const char *CharCode, int CharCodeFormat )
{
	return GetCharBytes_inline( CharCode, CharCodeFormat ) ;
}

// １文字の文字コードと文字のバイト数を取得する
__inline DWORD GetCharCode_inline( const char *CharCode, int CharCodeFormat, int *CharBytes, size_t &i )
{
	int UseSrcSize ;
	DWORD DestCode = 0 ;

	switch( GetCharCodeFormatUnitSize_inline( CharCodeFormat ) )
	{
	case 1 :
		if( ( ( BYTE * )CharCode )[ 0 ] == 0 )
		{
			if( CharBytes != NULL )
			{
				*CharBytes = 1 ;
			}
			return 0 ;
		}
		break ;

	case 2 :
		if( *( ( WORD * )CharCode ) == 0 )
		{
			if( CharBytes != NULL )
			{
				*CharBytes = 2 ;
			}
			return 0 ;
		}
		break ;

	case 4 :
		if( *( ( DWORD * )CharCode ) == 0 )
		{
			if( CharBytes != NULL )
			{
				*CharBytes = 4 ;
			}
			return 0 ;
		}
		break ;
	}

	UseSrcSize = GetCharBytes_inline( CharCode, CharCodeFormat ) ;
	if( UseSrcSize < 0 )
	{
		if( CharBytes != NULL )
		{
			*CharBytes = 0 ;
		}
		return 0 ;
	}

	switch( CharCodeFormat )
	{
	case DX_CHARCODEFORMAT_SHIFTJIS :
	case DX_CHARCODEFORMAT_GB2312 :
	case DX_CHARCODEFORMAT_UHC :
	case DX_CHARCODEFORMAT_BIG5 :
		if( UseSrcSize == 2 )
		{
			DestCode = ( ( ( BYTE * )CharCode )[ 0 ] << 8 ) | ( ( BYTE * )CharCode )[ 1 ] ;
			i += 2 ;
		}
		else
		{
			DestCode = ( ( BYTE * )CharCode )[ 0 ] ;
			i ++ ;
		}
		break ;

	case DX_CHARCODEFORMAT_ASCII :
	case DX_CHARCODEFORMAT_WINDOWS_1252 :
	case DX_CHARCODEFORMAT_ISO_IEC_8859_15 :
		DestCode = ( ( BYTE * )CharCode )[ 0 ] ;
		i ++ ;
		break ;

	case DX_CHARCODEFORMAT_UTF16LE :
	case DX_CHARCODEFORMAT_UTF16BE :
		{
			DWORD SrcCode1 ;
			DWORD SrcCode2 ;

			if( CharCodeFormat == DX_CHARCODEFORMAT_UTF16BE )
			{
				SrcCode1 = ( ( ( BYTE * )CharCode )[ 0 ] << 8 ) | ( ( BYTE * )CharCode )[ 1 ] ;
			}
			else
			{
				SrcCode1 = ( ( BYTE * )CharCode )[ 0 ] | ( ( ( BYTE * )CharCode )[ 1 ] << 8 ) ;
			}

			if( UseSrcSize == 4 )
			{
				if( CharCodeFormat == DX_CHARCODEFORMAT_UTF16BE )
				{
					SrcCode2 = ( ( ( BYTE * )CharCode )[ 2 ] << 8 ) | ( ( BYTE * )CharCode )[ 3 ] ;
				}
				else
				{
					SrcCode2 = ( ( BYTE * )CharCode )[ 2 ] | ( ( ( BYTE * )CharCode )[ 3 ] << 8 ) ;
				}

				DestCode = ( ( ( SrcCode1 & 0x3ff ) << 10 ) | ( SrcCode2 & 0x3ff ) ) + 0x10000 ;
				i += 2 ;
			}
			else
			{
				DestCode = SrcCode1 ;
				i ++ ;
			}
		}
		break ;

	case DX_CHARCODEFORMAT_UTF8 :
		switch( UseSrcSize )
		{
		case 1 :
			DestCode = ( ( BYTE * )CharCode )[ 0 ] ;
			i += 1 ;
			break ;

		case 2 :
			DestCode = ( ( ( ( BYTE * )CharCode )[ 0 ] & 0x1f ) << 6 ) | ( ( ( BYTE * )CharCode )[ 1 ] & 0x3f ) ;
			i += 2 ;
			break ;

		case 3 :
			DestCode = ( ( ( ( BYTE * )CharCode )[ 0 ] & 0x0f ) << 12 ) | ( ( ( ( BYTE * )CharCode )[ 1 ] & 0x3f ) << 6 ) | ( ( ( BYTE * )CharCode )[ 2 ] & 0x3f ) ;
			i += 3 ;
			break ;

		case 4 :
			DestCode = ( ( ( ( BYTE * )CharCode )[ 0 ] & 0x07 ) << 18 ) | ( ( ( ( BYTE * )CharCode )[ 1 ] & 0x3f ) << 12 ) | ( ( ( ( BYTE * )CharCode )[ 2 ] & 0x3f ) << 6 ) | ( ( ( BYTE * )CharCode )[ 3 ] & 0x3f ) ;
			i += 4 ;
			break ;

		case 5 :
			DestCode = ( ( ( ( BYTE * )CharCode )[ 0 ] & 0x03 ) << 24 ) | ( ( ( ( BYTE * )CharCode )[ 1 ] & 0x3f ) << 18 ) | ( ( ( ( BYTE * )CharCode )[ 2 ] & 0x3f ) << 12 ) | ( ( ( ( BYTE * )CharCode )[ 3 ] & 0x3f ) << 6 ) | ( ( ( BYTE * )CharCode )[ 4 ] & 0x3f ) ;
			i += 5 ;
			break ;

		case 6 :
			DestCode = ( ( ( ( BYTE * )CharCode )[ 0 ] & 0x01 ) << 30 ) | ( ( ( ( BYTE * )CharCode )[ 1 ] & 0x3f ) << 24 ) | ( ( ( ( BYTE * )CharCode )[ 2 ] & 0x3f ) << 18 ) | ( ( ( ( BYTE * )CharCode )[ 3 ] & 0x3f ) << 12 ) | ( ( ( ( BYTE * )CharCode )[ 4 ] & 0x3f ) << 6 ) | ( ( ( BYTE * )CharCode )[ 5 ] & 0x3f ) ;
			i += 6 ;
			break ;
		}
		break ;

	case DX_CHARCODEFORMAT_UTF32LE :
		DestCode = ( ( BYTE * )CharCode )[ 0 ] | ( ( ( BYTE * )CharCode )[ 1 ] << 8 ) | ( ( ( BYTE * )CharCode )[ 2 ] << 16 ) | ( ( ( BYTE * )CharCode )[ 3 ] << 24 ) ;
		i ++ ;
		break ;

	case DX_CHARCODEFORMAT_UTF32BE :
		DestCode = ( ( ( BYTE * )CharCode )[ 0 ] << 24 ) | ( ( ( BYTE * )CharCode )[ 1 ] << 16 ) | ( ( ( BYTE * )CharCode )[ 2 ] << 8 ) | ( ( BYTE * )CharCode )[ 3 ] ;
		i ++ ;
		break ;

	default :
		return 0 ;
	}

	if( CharBytes != NULL )
	{
		*CharBytes = UseSrcSize ;
	}

	return DestCode ;
}

// １文字の文字コードと文字のバイト数を取得する
extern DWORD GetCharCode( const char *CharCode, int CharCodeFormat, int *CharBytes )
{
	size_t i = 0 ;
	return GetCharCode_inline( CharCode, CharCodeFormat, CharBytes, i ) ;
}

// 文字コードを通常の文字列に変換する、終端にヌル文字は書き込まない( 戻り値：書き込んだバイト数 )
__inline int PutCharCode_inline( DWORD CharCode, int CharCodeFormat, char *Dest )
{
	switch( CharCodeFormat )
	{
	case DX_CHARCODEFORMAT_SHIFTJIS :
	case DX_CHARCODEFORMAT_GB2312 :
	case DX_CHARCODEFORMAT_UHC :
	case DX_CHARCODEFORMAT_BIG5 :
		if( CharCode >= 0x100 )
		{
			if( Dest != NULL )
			{
				( ( BYTE * )Dest )[ 0 ] = ( BYTE )( CharCode >> 8 ) ;
				( ( BYTE * )Dest )[ 1 ] = ( BYTE )( CharCode ) ;
			}
			return 2;
		}
		else
		{
			if( Dest != NULL )
			{
				( ( BYTE * )Dest )[ 0 ] = ( BYTE )CharCode ;
			}
			return 1 ;
		}

	case DX_CHARCODEFORMAT_ASCII :
	case DX_CHARCODEFORMAT_WINDOWS_1252 :
	case DX_CHARCODEFORMAT_ISO_IEC_8859_15 :
		if( Dest != NULL )
		{
			( ( BYTE * )Dest )[ 0 ] = ( BYTE )CharCode ;
		}
		return 1 ;

	case DX_CHARCODEFORMAT_UTF16LE :
	case DX_CHARCODEFORMAT_UTF16BE :
		{
			DWORD DestCode1 ;
			DWORD DestCode2 ;
			DWORD DestSize ;

			// UTF-16 で表現できない値の場合はキャンセル
			if( CharCode > 0x10ffff )
			{
				return 0 ;
			}

			if( CharCode > 0xffff )
			{
				DestCode1 = 0xd800 | ( ( ( CharCode - 0x10000 ) >> 10 ) & 0x3ff ) ;
				DestCode2 = 0xdc00 | (   ( CharCode - 0x10000 )         & 0x3ff ) ;

				DestSize = 4 ;
			}
			else
			{
				DestCode1 = CharCode ;
				DestCode2 = 0 ;

				DestSize = 2 ;
			}

			if( Dest != NULL )
			{
				if( CharCodeFormat == DX_CHARCODEFORMAT_UTF16BE )
				{
					( ( BYTE * )Dest )[ 0 ] = ( BYTE )( DestCode1 >> 8 ) ;
					( ( BYTE * )Dest )[ 1 ] = ( BYTE )( DestCode1 ) ;

					if( DestCode2 != 0 )
					{
						( ( BYTE * )Dest )[ 2 ] = ( BYTE )( DestCode2 >> 8 ) ;
						( ( BYTE * )Dest )[ 3 ] = ( BYTE )( DestCode2 ) ;
					}
				}
				else
				{
					( ( BYTE * )Dest )[ 0 ] = ( BYTE )( DestCode1 ) ;
					( ( BYTE * )Dest )[ 1 ] = ( BYTE )( DestCode1 >> 8 ) ;

					if( DestCode2 != 0 )
					{
						( ( BYTE * )Dest )[ 2 ] = ( BYTE )( DestCode2 ) ;
						( ( BYTE * )Dest )[ 3 ] = ( BYTE )( DestCode2 >> 8 ) ;
					}
				}
			}

			return DestSize ;
		}

	case DX_CHARCODEFORMAT_UTF8 :
		if( CharCode <= 0x7f )
		{
			if( Dest != NULL )
			{
				( ( BYTE * )Dest )[ 0 ] = ( BYTE )( CharCode ) ;
			}

			return 1 ;
		}
		else
		if( CharCode <= 0x7ff )
		{
			if( Dest != NULL )
			{
				( ( BYTE * )Dest )[ 0 ] = ( BYTE )( 0xc0 | ( ( CharCode >> 6 ) & 0x1f ) ) ;
				( ( BYTE * )Dest )[ 1 ] = ( BYTE )( 0x80 | ( ( CharCode      ) & 0x3f ) ) ;
			}

			return 2 ;
		}
		else
		if( CharCode <= 0xffff )
		{
			if( Dest != NULL )
			{
				( ( BYTE * )Dest )[ 0 ] = ( BYTE )( 0xe0 | ( ( CharCode >> 12 ) & 0x0f ) ) ;
				( ( BYTE * )Dest )[ 1 ] = ( BYTE )( 0x80 | ( ( CharCode >>  6 ) & 0x3f ) ) ;
				( ( BYTE * )Dest )[ 2 ] = ( BYTE )( 0x80 | ( ( CharCode       ) & 0x3f ) ) ;
			}

			return 3 ;
		}
		else
		if( CharCode <= 0x1fffff )
		{
			if( Dest != NULL )
			{
				( ( BYTE * )Dest )[ 0 ] = ( BYTE )( 0xf0 | ( ( CharCode >> 18 ) & 0x07 ) ) ;
				( ( BYTE * )Dest )[ 1 ] = ( BYTE )( 0x80 | ( ( CharCode >> 12 ) & 0x3f ) ) ;
				( ( BYTE * )Dest )[ 2 ] = ( BYTE )( 0x80 | ( ( CharCode >>  6 ) & 0x3f ) ) ;
				( ( BYTE * )Dest )[ 3 ] = ( BYTE )( 0x80 | ( ( CharCode       ) & 0x3f ) ) ;
			}

			return 4 ;
		}
		else
		if( CharCode <= 0x3ffffff )
		{
			if( Dest != NULL )
			{
				( ( BYTE * )Dest )[ 0 ] = ( BYTE )( 0xf8 | ( ( CharCode >> 24 ) & 0x03 ) ) ;
				( ( BYTE * )Dest )[ 1 ] = ( BYTE )( 0x80 | ( ( CharCode >> 18 ) & 0x3f ) ) ;
				( ( BYTE * )Dest )[ 2 ] = ( BYTE )( 0x80 | ( ( CharCode >> 12 ) & 0x3f ) ) ;
				( ( BYTE * )Dest )[ 3 ] = ( BYTE )( 0x80 | ( ( CharCode >>  6 ) & 0x3f ) ) ;
				( ( BYTE * )Dest )[ 4 ] = ( BYTE )( 0x80 | ( ( CharCode       ) & 0x3f ) ) ;
			}

			return 5 ;
		}
		else
		if( CharCode <= 0x7fffffff )
		{
			if( Dest != NULL )
			{
				( ( BYTE * )Dest )[ 0 ] = ( BYTE )( 0xfc | ( ( CharCode >> 30 ) & 0x01 ) ) ;
				( ( BYTE * )Dest )[ 1 ] = ( BYTE )( 0x80 | ( ( CharCode >> 24 ) & 0x3f ) ) ;
				( ( BYTE * )Dest )[ 2 ] = ( BYTE )( 0x80 | ( ( CharCode >> 18 ) & 0x3f ) ) ;
				( ( BYTE * )Dest )[ 3 ] = ( BYTE )( 0x80 | ( ( CharCode >> 12 ) & 0x3f ) ) ;
				( ( BYTE * )Dest )[ 4 ] = ( BYTE )( 0x80 | ( ( CharCode >>  6 ) & 0x3f ) ) ;
				( ( BYTE * )Dest )[ 5 ] = ( BYTE )( 0x80 | ( ( CharCode       ) & 0x3f ) ) ;
			}

			return 6 ;
		}
		else
		{
			return 0 ;
		}

	case DX_CHARCODEFORMAT_UTF32LE :
		if( Dest != NULL )
		{
			( ( BYTE * )Dest )[ 0 ] = ( BYTE )( CharCode       ) ;
			( ( BYTE * )Dest )[ 1 ] = ( BYTE )( CharCode >>  8 ) ;
			( ( BYTE * )Dest )[ 2 ] = ( BYTE )( CharCode >> 16 ) ;
			( ( BYTE * )Dest )[ 3 ] = ( BYTE )( CharCode >> 24 ) ;
		}
		return 4 ;

	case DX_CHARCODEFORMAT_UTF32BE :
		if( Dest != NULL )
		{
			( ( BYTE * )Dest )[ 0 ] = ( BYTE )( CharCode >> 24 ) ;
			( ( BYTE * )Dest )[ 1 ] = ( BYTE )( CharCode >> 16 ) ;
			( ( BYTE * )Dest )[ 2 ] = ( BYTE )( CharCode >>  8 ) ;
			( ( BYTE * )Dest )[ 3 ] = ( BYTE )( CharCode       ) ;
		}
		return 4 ;

	default :
		return 0 ;
	}
}

// 文字コードを通常の文字列に変換する( バッファーサイズ指定付き )、終端にヌル文字は書き込まない( 戻り値：書き込んだバイト数 )
__inline size_t PutCharCode_BufferBytes_inline( DWORD CharCode, int CharCodeFormat, char *Dest, size_t DestBufferBytes )
{
	switch( CharCodeFormat )
	{
	case DX_CHARCODEFORMAT_SHIFTJIS :
	case DX_CHARCODEFORMAT_GB2312 :
	case DX_CHARCODEFORMAT_UHC :
	case DX_CHARCODEFORMAT_BIG5 :
		if( CharCode >= 0x100 )
		{
			if( DestBufferBytes < 2 )
			{
				return 0 ;
			}
			else
			{
				if( Dest != NULL )
				{
					( ( BYTE * )Dest )[ 0 ] = ( BYTE )( CharCode >> 8 ) ;
					( ( BYTE * )Dest )[ 1 ] = ( BYTE )( CharCode ) ;
				}
				return 2;
			}
		}
		else
		{
			if( DestBufferBytes < 1 )
			{
				return 0 ;
			}
			else
			{
				if( Dest != NULL )
				{
					( ( BYTE * )Dest )[ 0 ] = ( BYTE )CharCode ;
				}
				return 1 ;
			}
		}

	case DX_CHARCODEFORMAT_ASCII :
	case DX_CHARCODEFORMAT_WINDOWS_1252 :
	case DX_CHARCODEFORMAT_ISO_IEC_8859_15 :
		if( DestBufferBytes < 1 )
		{
			return 0 ;
		}
		else
		{
			if( Dest != NULL )
			{
				( ( BYTE * )Dest )[ 0 ] = ( BYTE )CharCode ;
			}
			return 1 ;
		}

	case DX_CHARCODEFORMAT_UTF16LE :
	case DX_CHARCODEFORMAT_UTF16BE :
		{
			DWORD DestCode1 ;
			DWORD DestCode2 ;
			size_t DestSize ;

			// UTF-16 で表現できない値の場合はキャンセル
			if( CharCode > 0x10ffff )
			{
				return 0 ;
			}

			if( CharCode > 0xffff )
			{
				DestCode1 = 0xd800 | ( ( ( CharCode - 0x10000 ) >> 10 ) & 0x3ff ) ;
				DestCode2 = 0xdc00 | (   ( CharCode - 0x10000 )         & 0x3ff ) ;

				DestSize = 4 ;
			}
			else
			{
				DestCode1 = CharCode ;
				DestCode2 = 0 ;

				DestSize = 2 ;
			}

			if( DestBufferBytes < DestSize )
			{
				return 0 ;
			}
			else
			{
				if( Dest != NULL )
				{
					if( CharCodeFormat == DX_CHARCODEFORMAT_UTF16BE )
					{
						( ( BYTE * )Dest )[ 0 ] = ( BYTE )( DestCode1 >> 8 ) ;
						( ( BYTE * )Dest )[ 1 ] = ( BYTE )( DestCode1 ) ;

						if( DestCode2 != 0 )
						{
							( ( BYTE * )Dest )[ 2 ] = ( BYTE )( DestCode2 >> 8 ) ;
							( ( BYTE * )Dest )[ 3 ] = ( BYTE )( DestCode2 ) ;
						}
					}
					else
					{
						( ( BYTE * )Dest )[ 0 ] = ( BYTE )( DestCode1 ) ;
						( ( BYTE * )Dest )[ 1 ] = ( BYTE )( DestCode1 >> 8 ) ;

						if( DestCode2 != 0 )
						{
							( ( BYTE * )Dest )[ 2 ] = ( BYTE )( DestCode2 ) ;
							( ( BYTE * )Dest )[ 3 ] = ( BYTE )( DestCode2 >> 8 ) ;
						}
					}
				}

				return DestSize ;
			}
		}

	case DX_CHARCODEFORMAT_UTF8 :
		if( CharCode <= 0x7f )
		{
			if( DestBufferBytes < 1 )
			{
				return 0 ;
			}
			else
			{
				if( Dest != NULL )
				{
					( ( BYTE * )Dest )[ 0 ] = ( BYTE )( CharCode ) ;
				}

				return 1 ;
			}
		}
		else
		if( CharCode <= 0x7ff )
		{
			if( DestBufferBytes < 2 )
			{
				return 0 ;
			}
			else
			{
				if( Dest != NULL )
				{
					( ( BYTE * )Dest )[ 0 ] = ( BYTE )( 0xc0 | ( ( CharCode >> 6 ) & 0x1f ) ) ;
					( ( BYTE * )Dest )[ 1 ] = ( BYTE )( 0x80 | ( ( CharCode      ) & 0x3f ) ) ;
				}

				return 2 ;
			}
		}
		else
		if( CharCode <= 0xffff )
		{
			if( DestBufferBytes < 3 )
			{
				return 0 ;
			}
			else
			{
				if( Dest != NULL )
				{
					( ( BYTE * )Dest )[ 0 ] = ( BYTE )( 0xe0 | ( ( CharCode >> 12 ) & 0x0f ) ) ;
					( ( BYTE * )Dest )[ 1 ] = ( BYTE )( 0x80 | ( ( CharCode >>  6 ) & 0x3f ) ) ;
					( ( BYTE * )Dest )[ 2 ] = ( BYTE )( 0x80 | ( ( CharCode       ) & 0x3f ) ) ;
				}

				return 3 ;
			}
		}
		else
		if( CharCode <= 0x1fffff )
		{
			if( DestBufferBytes < 4 )
			{
				return 0 ;
			}
			else
			{
				if( Dest != NULL )
				{
					( ( BYTE * )Dest )[ 0 ] = ( BYTE )( 0xf0 | ( ( CharCode >> 18 ) & 0x07 ) ) ;
					( ( BYTE * )Dest )[ 1 ] = ( BYTE )( 0x80 | ( ( CharCode >> 12 ) & 0x3f ) ) ;
					( ( BYTE * )Dest )[ 2 ] = ( BYTE )( 0x80 | ( ( CharCode >>  6 ) & 0x3f ) ) ;
					( ( BYTE * )Dest )[ 3 ] = ( BYTE )( 0x80 | ( ( CharCode       ) & 0x3f ) ) ;
				}

				return 4 ;
			}
		}
		else
		if( CharCode <= 0x3ffffff )
		{
			if( DestBufferBytes < 5 )
			{
				return 0 ;
			}
			else
			{
				if( Dest != NULL )
				{
					( ( BYTE * )Dest )[ 0 ] = ( BYTE )( 0xf8 | ( ( CharCode >> 24 ) & 0x03 ) ) ;
					( ( BYTE * )Dest )[ 1 ] = ( BYTE )( 0x80 | ( ( CharCode >> 18 ) & 0x3f ) ) ;
					( ( BYTE * )Dest )[ 2 ] = ( BYTE )( 0x80 | ( ( CharCode >> 12 ) & 0x3f ) ) ;
					( ( BYTE * )Dest )[ 3 ] = ( BYTE )( 0x80 | ( ( CharCode >>  6 ) & 0x3f ) ) ;
					( ( BYTE * )Dest )[ 4 ] = ( BYTE )( 0x80 | ( ( CharCode       ) & 0x3f ) ) ;
				}

				return 5 ;
			}
		}
		else
		if( CharCode <= 0x7fffffff )
		{
			if( DestBufferBytes < 6 )
			{
				return 0 ;
			}
			else
			{
				if( Dest != NULL )
				{
					( ( BYTE * )Dest )[ 0 ] = ( BYTE )( 0xfc | ( ( CharCode >> 30 ) & 0x01 ) ) ;
					( ( BYTE * )Dest )[ 1 ] = ( BYTE )( 0x80 | ( ( CharCode >> 24 ) & 0x3f ) ) ;
					( ( BYTE * )Dest )[ 2 ] = ( BYTE )( 0x80 | ( ( CharCode >> 18 ) & 0x3f ) ) ;
					( ( BYTE * )Dest )[ 3 ] = ( BYTE )( 0x80 | ( ( CharCode >> 12 ) & 0x3f ) ) ;
					( ( BYTE * )Dest )[ 4 ] = ( BYTE )( 0x80 | ( ( CharCode >>  6 ) & 0x3f ) ) ;
					( ( BYTE * )Dest )[ 5 ] = ( BYTE )( 0x80 | ( ( CharCode       ) & 0x3f ) ) ;
				}

				return 6 ;
			}
		}
		else
		{
			return 0 ;
		}

	case DX_CHARCODEFORMAT_UTF32LE :
		if( DestBufferBytes < 4 )
		{
			return 0 ;
		}
		else
		{
			if( Dest != NULL )
			{
				( ( BYTE * )Dest )[ 0 ] = ( BYTE )( CharCode       ) ;
				( ( BYTE * )Dest )[ 1 ] = ( BYTE )( CharCode >>  8 ) ;
				( ( BYTE * )Dest )[ 2 ] = ( BYTE )( CharCode >> 16 ) ;
				( ( BYTE * )Dest )[ 3 ] = ( BYTE )( CharCode >> 24 ) ;
			}
			return 4 ;
		}

	case DX_CHARCODEFORMAT_UTF32BE :
		if( DestBufferBytes < 4 )
		{
			return 0 ;
		}
		else
		{
			if( Dest != NULL )
			{
				( ( BYTE * )Dest )[ 0 ] = ( BYTE )( CharCode >> 24 ) ;
				( ( BYTE * )Dest )[ 1 ] = ( BYTE )( CharCode >> 16 ) ;
				( ( BYTE * )Dest )[ 2 ] = ( BYTE )( CharCode >>  8 ) ;
				( ( BYTE * )Dest )[ 3 ] = ( BYTE )( CharCode       ) ;
			}
			return 4 ;
		}

	default :
		return 0 ;
	}
}

// 文字コードを通常の文字列に変換する、終端にヌル文字は書き込まない( 戻り値：書き込んだバイト数 )
extern int PutCharCode( DWORD CharCode, int CharCodeFormat, char *Dest, size_t BufferBytes )
{
	return ( int )PutCharCode_BufferBytes_inline( CharCode, CharCodeFormat, Dest, BufferBytes ) ;
}

// 文字コードを指定の文字コード形式の文字に変換する
__inline DWORD ConvCharCode_inline( DWORD SrcCharCode, int SrcCharCodeFormat, int DestCharCodeFormat )
{
	// キャラクターコードテーブルが初期化されていなかったら初期化
	if( g_CharCodeSystem.InitializeFlag == FALSE )
	{
		InitCharCode() ;
	}

	if( SrcCharCodeFormat == DestCharCodeFormat )
	{
		return SrcCharCode ;
	}

	switch( SrcCharCodeFormat )
	{
	case DX_CHARCODEFORMAT_SHIFTJIS :
	case DX_CHARCODEFORMAT_GB2312 :
	case DX_CHARCODEFORMAT_WINDOWS_1252 :
	case DX_CHARCODEFORMAT_ISO_IEC_8859_15 :
	case DX_CHARCODEFORMAT_UHC :
	case DX_CHARCODEFORMAT_BIG5 :
		{
			DWORD Unicode ;

			if( DestCharCodeFormat == DX_CHARCODEFORMAT_ASCII )
			{
				if( SrcCharCode > 0xff )
				{
					return 0 ;
				}
				return SrcCharCode ;
			}

			switch( SrcCharCodeFormat )
			{
			case DX_CHARCODEFORMAT_SHIFTJIS :
				Unicode = g_CharCodeSystem.CharCodeCP932Info.MultiByteToUTF16[ SrcCharCode ] ;
				break ;

			case DX_CHARCODEFORMAT_GB2312 :
				Unicode = g_CharCodeSystem.CharCodeCP936Info.MultiByteToUTF16[ SrcCharCode ] ;
				break ;

			case DX_CHARCODEFORMAT_UHC :
				Unicode = g_CharCodeSystem.CharCodeCP949Info.MultiByteToUTF16[ SrcCharCode ] ;
				break ;

			case DX_CHARCODEFORMAT_BIG5 :
				Unicode = g_CharCodeSystem.CharCodeCP950Info.MultiByteToUTF16[ SrcCharCode ] ;
				break ;

			case DX_CHARCODEFORMAT_WINDOWS_1252 :
				Unicode = g_CharCodeSystem.CharCodeCP1252Info.MultiByteToUTF16[ SrcCharCode ] ;
				break ;

			case DX_CHARCODEFORMAT_ISO_IEC_8859_15 :
				Unicode = g_CharCodeSystem.CharCodeISO_IEC_8859_15Info.MultiByteToUTF16[ SrcCharCode ] ;
				break ;

			default :
				return 0 ;
			}

			switch( DestCharCodeFormat )
			{
			case DX_CHARCODEFORMAT_SHIFTJIS :
				return g_CharCodeSystem.CharCodeCP932Info.UTF16ToMultiByte[ Unicode ] ;

			case DX_CHARCODEFORMAT_GB2312 :
				return g_CharCodeSystem.CharCodeCP936Info.UTF16ToMultiByte[ Unicode ] ;

			case DX_CHARCODEFORMAT_UHC :
				return g_CharCodeSystem.CharCodeCP949Info.UTF16ToMultiByte[ Unicode ] ;

			case DX_CHARCODEFORMAT_BIG5 :
				return g_CharCodeSystem.CharCodeCP950Info.UTF16ToMultiByte[ Unicode ] ;

			case DX_CHARCODEFORMAT_WINDOWS_1252 :
				return g_CharCodeSystem.CharCodeCP1252Info.UTF16ToMultiByte[ Unicode ] ;

			case DX_CHARCODEFORMAT_ISO_IEC_8859_15 :
				return g_CharCodeSystem.CharCodeISO_IEC_8859_15Info.UTF16ToMultiByte[ Unicode ] ;

			case DX_CHARCODEFORMAT_UTF16LE :
			case DX_CHARCODEFORMAT_UTF16BE :
			case DX_CHARCODEFORMAT_UTF8 :
			case DX_CHARCODEFORMAT_UTF32LE :
			case DX_CHARCODEFORMAT_UTF32BE :
				return Unicode ;

			default :
				return 0 ;
			}
		}

	case DX_CHARCODEFORMAT_ASCII :
		return SrcCharCode ;

	case DX_CHARCODEFORMAT_UTF16LE :
	case DX_CHARCODEFORMAT_UTF16BE :
	case DX_CHARCODEFORMAT_UTF8 :
	case DX_CHARCODEFORMAT_UTF32LE :
	case DX_CHARCODEFORMAT_UTF32BE :
		switch( DestCharCodeFormat )
		{
		case DX_CHARCODEFORMAT_ASCII :
			if( SrcCharCode > 0xff )
			{
				return 0 ;
			}
			return SrcCharCode ;

		case DX_CHARCODEFORMAT_SHIFTJIS :
			if( SrcCharCode > 0xffff )
			{
				return 0 ;
			}
			return g_CharCodeSystem.CharCodeCP932Info.UTF16ToMultiByte[ SrcCharCode ] ;

		case DX_CHARCODEFORMAT_GB2312 :
			if( SrcCharCode > 0xffff )
			{
				return 0 ;
			}
			return g_CharCodeSystem.CharCodeCP936Info.UTF16ToMultiByte[ SrcCharCode ] ;

		case DX_CHARCODEFORMAT_UHC :
			if( SrcCharCode > 0xffff )
			{
				return 0 ;
			}
			return g_CharCodeSystem.CharCodeCP949Info.UTF16ToMultiByte[ SrcCharCode ] ;

		case DX_CHARCODEFORMAT_BIG5 :
			if( SrcCharCode > 0xffff )
			{
				return 0 ;
			}
			return g_CharCodeSystem.CharCodeCP950Info.UTF16ToMultiByte[ SrcCharCode ] ;

		case DX_CHARCODEFORMAT_WINDOWS_1252 :
			if( SrcCharCode > 0xffff )
			{
				return 0 ;
			}
			return g_CharCodeSystem.CharCodeCP1252Info.UTF16ToMultiByte[ SrcCharCode ] ;

		case DX_CHARCODEFORMAT_ISO_IEC_8859_15 :
			if( SrcCharCode > 0xffff )
			{
				return 0 ;
			}
			return g_CharCodeSystem.CharCodeISO_IEC_8859_15Info.UTF16ToMultiByte[ SrcCharCode ] ;

		case DX_CHARCODEFORMAT_UTF16LE :
		case DX_CHARCODEFORMAT_UTF16BE :
		case DX_CHARCODEFORMAT_UTF8 :
		case DX_CHARCODEFORMAT_UTF32LE :
		case DX_CHARCODEFORMAT_UTF32BE :
			return SrcCharCode ;

		default :
			return 0 ;
		}

	default :
		return 0 ;
	}
}

// 文字コードを指定の文字コード形式の文字に変換する
extern DWORD ConvCharCode( DWORD SrcCharCode, int SrcCharCodeFormat, int DestCharCodeFormat )
{
	return ConvCharCode_inline( SrcCharCode, SrcCharCodeFormat, DestCharCodeFormat ) ;
}

// １文字４バイトの配列を、別文字コード形式の１文字４バイトの配列に変換する( 戻り値：変換後のサイズ、ヌル文字含む( 単位：バイト ) )
extern int ConvCharCodeString( const DWORD *Src, int SrcCharCodeFormat, DWORD *Dest, size_t BufferBytes, int DestCharCodeFormat )
{
	size_t DestSize ;
	DWORD DestCode ;

	if( BufferBytes < sizeof( DWORD ) )
	{
		return 0 ;
	}

	DestSize = 0 ;
	for(;;)
	{
		if( BufferBytes - DestSize < sizeof( DWORD ) * 2 )
		{
			break ;
		}

		if( *Src == 0 )
		{
			break ;
		}

		DestCode = ConvCharCode_inline( *Src, SrcCharCodeFormat, DestCharCodeFormat ) ;
		Src ++ ;

		if( DestCode != 0 )
		{
			if( Dest != NULL )
			{
				*Dest = DestCode ;
				Dest ++ ;
			}

			DestSize += 4 ;
		}
	}

	if( Dest != NULL )
	{
		*Dest = 0 ;
		Dest ++ ;
	}
	DestSize += 4 ;

	return ( int )DestSize ;
}

// 文字列を１文字４バイトの配列に変換する( 戻り値：変換後のサイズ、ヌル文字含む( 単位：バイト ) )
__inline int StringToCharCodeString_inline( const char *Src, int SrcCharCodeFormat, DWORD *Dest )
{
	int DestSize ;
	DWORD SrcCode ;
	const BYTE *SrcStr ;
	int UseSrcSize ;
	size_t i = 0 ;

	SrcStr = ( BYTE * )Src ;
	DestSize = 0 ;
	for(;;)
	{
		SrcCode = GetCharCode_inline( ( const char * )SrcStr, SrcCharCodeFormat, &UseSrcSize, i ) ;
		SrcStr += UseSrcSize ;
		if( SrcCode == 0 )
		{
			break ;
		}

		if( Dest != NULL )
		{
			*Dest = SrcCode ;
			Dest ++ ;
		}
		DestSize += 4 ;
	}

	if( Dest != NULL )
	{
		*Dest = 0 ;
		Dest ++ ;
	}
	DestSize += 4 ;

	return DestSize ;
}

// 文字列を１文字４バイトの配列に変換する( 戻り値：変換後のサイズ、ヌル文字含む( 単位：バイト ) )
__inline int StringToCharCodeString_BufferBytes_inline( const char *Src, int SrcCharCodeFormat, DWORD *Dest, size_t BufferBytes )
{
	size_t DestSize ;
	DWORD SrcCode ;
	const BYTE *SrcStr ;
	int UseSrcSize ;
	size_t i = 0 ;

	if( BufferBytes < 4 )
	{
		return 0 ;
	}

	SrcStr = ( BYTE * )Src ;
	DestSize = 0 ;
	for(;;)
	{
		if( BufferBytes < sizeof( DWORD ) * 2 )
		{
			break ;
		}

		SrcCode = GetCharCode_inline( ( const char * )SrcStr, SrcCharCodeFormat, &UseSrcSize, i ) ;
		SrcStr += UseSrcSize ;
		if( SrcCode == 0 )
		{
			break ;
		}

		if( Dest != NULL )
		{
			*Dest = SrcCode ;
			Dest ++ ;
		}
		DestSize += 4 ;
	}

	if( Dest != NULL )
	{
		*Dest = 0 ;
		Dest ++ ;
	}
	DestSize += 4 ;

	return ( int )DestSize ;
}

// 文字列を１文字４バイトの配列に変換する( 戻り値：変換後のサイズ、ヌル文字含む( 単位：バイト ) )
extern int StringToCharCodeString( const char *Src, int SrcCharCodeFormat, DWORD *Dest, size_t BufferBytes )
{
	return StringToCharCodeString_BufferBytes_inline( Src, SrcCharCodeFormat, Dest, BufferBytes ) ;
}

// １文字４バイトの配列を文字列に変換する( 戻り値：変換後のサイズ、ヌル文字含む( 単位：バイト ) )
extern int CharCodeStringToString( const DWORD *Src, char *Dest, size_t BufferBytes, int DestCharCodeFormat )
{
	size_t DestSize ;
	size_t NulWriteBytes ;
	size_t WriteBytes ;
	BYTE *DestStr ;

	DestStr = ( BYTE * )Dest ;
	DestSize = 0 ;

	NulWriteBytes = PutCharCode_BufferBytes_inline( 0x00, DestCharCodeFormat, ( char * )DestStr, BufferBytes - DestSize ) ;
	if( NulWriteBytes == 0 )
	{
		return 0 ;
	}

	for(;;)
	{
		if( *Src == 0 )
		{
			break ;
		}

		WriteBytes = PutCharCode_BufferBytes_inline( *Src, DestCharCodeFormat, ( char * )DestStr, BufferBytes - DestSize ) ;
		if( WriteBytes == 0 || BufferBytes - DestSize < WriteBytes + NulWriteBytes )
		{
			break ;
		}
		if( DestStr != NULL )
		{
			DestStr += WriteBytes ;
		}
		DestSize += WriteBytes ;
		Src ++ ;
		if( BufferBytes - DestSize <= NulWriteBytes )
		{
			break ;
		}
	}

	// 終端文字をセット
	DestSize += PutCharCode_BufferBytes_inline( 0, DestCharCodeFormat, ( char * )DestStr, BufferBytes - DestSize ) ;

	return ( int )DestSize ;
}






// ConvString の冒頭部分のマクロ
#define CONVSTRING_BEGIN				\
	size_t			DestSize = 0 ;		\
	DWORD			Unicode ;			\
	const BYTE *	SrcStr ;			\
	BYTE *			DestStr ;			\
	size_t			i = 0 ;				\
										\
	SrcStr   = ( BYTE * )Src ;			\
	DestStr  = ( BYTE * )Dest ;


// ConvString の ShiftJISからUnicodeを取得するまでの処理
__inline bool ConvString_SrcCode_SHIFTJIS( const BYTE *( &SrcStr ), DWORD &Unicode, size_t &i )
{
	DWORD SrcCode ;

	if( CHECK_SHIFTJIS_2BYTE( ( ( BYTE * )SrcStr )[ 0 ] ) )
	{
		SrcCode = ( ( ( BYTE * )SrcStr )[ 0 ] << 8 ) | ( ( BYTE * )SrcStr )[ 1 ] ;
		SrcStr += 2 ;
		i += 2 ;
	}
	else
	{
		SrcCode = ( ( BYTE * )SrcStr )[ 0 ] ;
		if( SrcCode == 0 )
		{
			return false;
		}
		SrcStr ++ ;
		i ++ ;
	}

	Unicode = g_CharCodeSystem.CharCodeCP932Info.MultiByteToUTF16[ SrcCode ] ;

	return true;
}

// ConvString の UTF16LEからUnicodeを取得するまでの処理
__inline bool ConvString_SrcCode_UTF16LE( const BYTE *( &SrcStr ), DWORD &Unicode, size_t &i )
{
	if( ( ( ( ( BYTE * )SrcStr )[ 0 ] | ( ( ( BYTE * )SrcStr )[ 1 ] << 8 ) ) & 0xfc00 ) == 0xd800 )
	{
		DWORD SrcCode1 ;
		DWORD SrcCode2 ;

		SrcCode1 = ( ( BYTE * )SrcStr )[ 0 ] | ( ( ( BYTE * )SrcStr )[ 1 ] << 8 ) ;
		SrcCode2 = ( ( BYTE * )SrcStr )[ 2 ] | ( ( ( BYTE * )SrcStr )[ 3 ] << 8 ) ;

		Unicode = ( ( ( SrcCode1 & 0x3ff ) << 10 ) | ( SrcCode2 & 0x3ff ) ) + 0x10000 ;
		SrcStr += 4 ;
		i += 2 ;
	}
	else
	{
		Unicode = ( ( BYTE * )SrcStr )[ 0 ] | ( ( ( BYTE * )SrcStr )[ 1 ] << 8 ) ;
		if( Unicode == 0 )
		{
			return false ;
		}
		SrcStr += 2 ;
		i ++ ;
	}

	return true ;
}

// ConvString の UTF16BEからUnicodeを取得するまでの処理
__inline bool ConvString_SrcCode_UTF16BE( const BYTE *( &SrcStr ), DWORD &Unicode, size_t &i )
{
	if( ( ( ( ( ( BYTE * )SrcStr )[ 0 ] << 8 ) | ( ( BYTE * )SrcStr )[ 1 ] ) & 0xfc00 ) == 0xd800 )
	{
		DWORD SrcCode1 ;
		DWORD SrcCode2 ;

		SrcCode1 = ( ( ( BYTE * )SrcStr )[ 0 ] << 8 ) | ( ( BYTE * )SrcStr )[ 1 ] ;
		SrcCode2 = ( ( ( BYTE * )SrcStr )[ 2 ] << 8 ) | ( ( BYTE * )SrcStr )[ 3 ] ;

		Unicode = ( ( ( SrcCode1 & 0x3ff ) << 10 ) | ( SrcCode2 & 0x3ff ) ) + 0x10000 ;
		SrcStr += 4 ;
		i += 2 ;
	}
	else
	{
		Unicode = ( ( ( BYTE * )SrcStr )[ 0 ] << 8 ) | ( ( BYTE * )SrcStr )[ 1 ] ;
		if( Unicode == 0 )
		{
			return false ;
		}
		SrcStr += 2 ;
		i ++ ;
	}

	return true ;
}

// ConvString の UTF8からUnicodeを取得するまでの処理
__inline bool ConvString_SrcCode_UTF8( const BYTE *( &SrcStr ), DWORD &Unicode, size_t &i )
{
	if( ( ( ( BYTE * )SrcStr )[ 0 ] & 0x80 ) == 0x00 )
	{
		Unicode = ( ( BYTE * )SrcStr )[ 0 ] ;
		if( Unicode == 0 )
		{
			return false ;
		}
		SrcStr += 1 ;
		i += 1 ;
	}
	else
	if( ( ( ( BYTE * )SrcStr )[ 0 ] & 0xe0 ) == 0xc0 )
	{
		Unicode = ( ( ( ( BYTE * )SrcStr )[ 0 ] & 0x1f ) << 6 ) | ( ( ( BYTE * )SrcStr )[ 1 ] & 0x3f ) ;
		SrcStr += 2 ;
		i += 2 ;
	}
	else
	if( ( ( ( BYTE * )SrcStr )[ 0 ] & 0xf0 ) == 0xe0 )
	{
		Unicode = ( ( ( ( BYTE * )SrcStr )[ 0 ] & 0x0f ) << 12 ) | ( ( ( ( BYTE * )SrcStr )[ 1 ] & 0x3f ) << 6 ) | ( ( ( BYTE * )SrcStr )[ 2 ] & 0x3f ) ;
		SrcStr += 3 ;
		i += 3 ;
	}
	else
	if( ( ( ( BYTE * )SrcStr )[ 0 ] & 0xf8 ) == 0xf0 )
	{
		Unicode = ( ( ( ( BYTE * )SrcStr )[ 0 ] & 0x07 ) << 18 ) | ( ( ( ( BYTE * )SrcStr )[ 1 ] & 0x3f ) << 12 ) | ( ( ( ( BYTE * )SrcStr )[ 2 ] & 0x3f ) << 6 ) | ( ( ( BYTE * )SrcStr )[ 3 ] & 0x3f ) ;
		SrcStr += 4 ;
		i += 4 ;
	}
	else
	if( ( ( ( BYTE * )SrcStr )[ 0 ] & 0xfc ) == 0xf8 )
	{
		Unicode = ( ( ( ( BYTE * )SrcStr )[ 0 ] & 0x03 ) << 24 ) | ( ( ( ( BYTE * )SrcStr )[ 1 ] & 0x3f ) << 18 ) | ( ( ( ( BYTE * )SrcStr )[ 2 ] & 0x3f ) << 12 ) | ( ( ( ( BYTE * )SrcStr )[ 3 ] & 0x3f ) << 6 ) | ( ( ( BYTE * )SrcStr )[ 4 ] & 0x3f ) ;
		SrcStr += 5 ;
		i += 5 ;
	}
	else
	if( ( ( ( BYTE * )SrcStr )[ 0 ] & 0xfe ) == 0xfc )
	{
		Unicode = ( ( ( ( BYTE * )SrcStr )[ 0 ] & 0x01 ) << 30 ) | ( ( ( ( BYTE * )SrcStr )[ 1 ] & 0x3f ) << 24 ) | ( ( ( ( BYTE * )SrcStr )[ 2 ] & 0x3f ) << 18 ) | ( ( ( ( BYTE * )SrcStr )[ 3 ] & 0x3f ) << 12 ) | ( ( ( ( BYTE * )SrcStr )[ 4 ] & 0x3f ) << 6 ) | ( ( ( BYTE * )SrcStr )[ 5 ] & 0x3f ) ;
		SrcStr += 6 ;
		i += 6 ;
	}
	else
	{
		return false ;
	}

	return true ;
}

// ConvString の UTF32LEからUnicodeを取得するまでの処理
__inline bool ConvString_SrcCode_UTF32LE( const BYTE *( &SrcStr ), DWORD &Unicode, size_t &i )
{
	Unicode = ( ( BYTE * )SrcStr )[ 0 ] | ( ( ( BYTE * )SrcStr )[ 1 ] << 8 ) | ( ( ( BYTE * )SrcStr )[ 2 ] << 16 ) | ( ( ( BYTE * )SrcStr )[ 3 ] << 24 ) ;
	SrcStr += 4 ;
	i ++ ;
	return Unicode != 0 ;
}

// ConvString の UTF32BEからUnicodeを取得するまでの処理
__inline bool ConvString_SrcCode_UTF32BE( const BYTE *( &SrcStr ), DWORD &Unicode, size_t &i )
{
	Unicode = ( ( ( BYTE * )SrcStr )[ 0 ] << 24 ) | ( ( ( BYTE * )SrcStr )[ 1 ] << 16 ) | ( ( ( BYTE * )SrcStr )[ 2 ] << 8 ) | ( ( BYTE * )SrcStr )[ 3 ] ;
	SrcStr += 4 ;
	i ++ ;
	return Unicode != 0 ;
}

// ConvString の Unicodeから ASCIIとして書き込むまでの処理
__inline void ConvString_DestCode_ASCII( BYTE *&DestStr, DWORD &DestCode, size_t &DestSize, size_t BufferBytes )
{
	DWORD CharCode ;

	// １バイトで表現できない値の場合はキャンセル
	if( DestCode > 0xff )
	{
		return ;
	}

	// 1文字 + ヌル文字を書き込むバッファサイズがない場合はキャンセル
	if( BufferBytes - DestSize < 1 + 1 )
	{
		return ;
	}

	CharCode = DestCode ;

	if( DestStr != NULL )
	{
		( ( BYTE * )DestStr )[ 0 ] = ( BYTE )CharCode ;
		DestStr += 1 ;
	}
	DestSize += 1 ;
}

// ConvString の Unicodeから ShiftJISとして書き込むまでの処理
__inline void ConvString_DestCode_SHIFTJIS( BYTE *&DestStr, DWORD &DestCode, size_t &DestSize, size_t BufferBytes )
{
	DWORD CharCode ;

	// テーブル値 で表現できない値の場合はキャンセル
	if( DestCode > 0xffff )
	{
		return ;
	}

	CharCode = g_CharCodeSystem.CharCodeCP932Info.UTF16ToMultiByte[ DestCode ] ;

	if( CharCode >= 0x100 )
	{
		// 1文字 + ヌル文字を書き込むバッファサイズがない場合はキャンセル
		if( BufferBytes - DestSize < 2 + 1 )
		{
			return ;
		}

		if( DestStr != NULL )
		{
			( ( BYTE * )DestStr )[ 0 ] = ( BYTE )( CharCode >> 8 ) ;
			( ( BYTE * )DestStr )[ 1 ] = ( BYTE )( CharCode ) ;
			DestStr += 2 ;
		}
		DestSize += 2 ;
	}
	else
	{
		// 1文字 + ヌル文字を書き込むバッファサイズがない場合はキャンセル
		if( BufferBytes - DestSize < 1 + 1 )
		{
			return ;
		}

		if( DestStr != NULL )
		{
			( ( BYTE * )DestStr )[ 0 ] = ( BYTE )CharCode ;
			DestStr += 1 ;
		}
		DestSize += 1 ;
	}
}

// ConvString の UnicodeからUTF16LEとして書き込むまでの処理
__inline void ConvString_DestCode_UTF16LE( BYTE *&DestStr, DWORD &DestCode, size_t &DestSize, size_t BufferBytes )
{
	// UTF-16 で表現できない値の場合はキャンセル
	if( DestCode > 0x10ffff )
	{
		return ;
	}

	if( DestCode > 0xffff )
	{
		// 1文字 + ヌル文字を書き込むバッファサイズがない場合はキャンセル
		if( BufferBytes - DestSize < 4 + 2 )
		{
			return ;
		}

		if( DestStr != NULL )
		{
			DWORD DestCode1 ;
			DWORD DestCode2 ;

			DestCode1 = 0xd800 | ( ( ( DestCode - 0x10000 ) >> 10 ) & 0x3ff ) ;
			DestCode2 = 0xdc00 | (   ( DestCode - 0x10000 )         & 0x3ff ) ;

			( ( BYTE * )DestStr )[ 0 ] = ( BYTE )( DestCode1      ) ;
			( ( BYTE * )DestStr )[ 1 ] = ( BYTE )( DestCode1 >> 8 ) ;

			( ( BYTE * )DestStr )[ 2 ] = ( BYTE )( DestCode2      ) ;
			( ( BYTE * )DestStr )[ 3 ] = ( BYTE )( DestCode2 >> 8 ) ;

			DestStr += 4 ;
		}

		DestSize += 4 ;
	}
	else
	{
		// 1文字 + ヌル文字を書き込むバッファサイズがない場合はキャンセル
		if( BufferBytes - DestSize < 2 + 2 )
		{
			return ;
		}

		if( DestStr != NULL )
		{
			( ( BYTE * )DestStr )[ 0 ] = ( BYTE )( DestCode      ) ;
			( ( BYTE * )DestStr )[ 1 ] = ( BYTE )( DestCode >> 8 ) ;

			DestStr += 2 ;
		}

		DestSize += 2 ;
	}
}

// ConvString の UnicodeからUTF16BEとして書き込むまでの処理
__inline void ConvString_DestCode_UTF16BE( BYTE *&DestStr, DWORD &DestCode, size_t &DestSize, size_t BufferBytes )
{
	// UTF-16 で表現できない値の場合はキャンセル
	if( DestCode > 0x10ffff )
	{
		return ;
	}

	if( DestCode > 0xffff )
	{
		// 1文字 + ヌル文字を書き込むバッファサイズがない場合はキャンセル
		if( BufferBytes - DestSize < 4 + 2 )
		{
			return ;
		}

		if( DestStr != NULL )
		{
			DWORD DestCode1 ;
			DWORD DestCode2 ;

			DestCode1 = 0xd800 | ( ( ( DestCode - 0x10000 ) >> 10 ) & 0x3ff ) ;
			DestCode2 = 0xdc00 | (   ( DestCode - 0x10000 )         & 0x3ff ) ;

			( ( BYTE * )DestStr )[ 0 ] = ( BYTE )( DestCode1 >> 8 ) ;
			( ( BYTE * )DestStr )[ 1 ] = ( BYTE )( DestCode1      ) ;

			( ( BYTE * )DestStr )[ 2 ] = ( BYTE )( DestCode2 >> 8 ) ;
			( ( BYTE * )DestStr )[ 3 ] = ( BYTE )( DestCode2      ) ;

			DestStr += 4 ;
		}

		DestSize += 4 ;
	}
	else
	{
		// 1文字 + ヌル文字を書き込むバッファサイズがない場合はキャンセル
		if( BufferBytes - DestSize < 2 + 2 )
		{
			return ;
		}

		if( DestStr != NULL )
		{
			( ( BYTE * )DestStr )[ 0 ] = ( BYTE )( DestCode >> 8 ) ;
			( ( BYTE * )DestStr )[ 1 ] = ( BYTE )( DestCode      ) ;

			DestStr += 2 ;
		}

		DestSize += 2 ;
	}
}

// ConvString の UnicodeからUTF8として書き込むまでの処理
__inline void ConvString_DestCode_UTF8( BYTE *&DestStr, DWORD &DestCode, size_t &DestSize, size_t BufferBytes )
{
	if( DestCode <= 0x7f )
	{
		// 1文字 + ヌル文字を書き込むバッファサイズがない場合はキャンセル
		if( BufferBytes - DestSize < 1 + 1 )
		{
			return ;
		}

		if( DestStr != NULL )
		{
			( ( BYTE * )DestStr )[ 0 ] = ( BYTE )( DestCode ) ;
			DestStr ++ ;
		}

		DestSize ++ ;
	}
	else
	if( DestCode <= 0x7ff )
	{
		// 1文字 + ヌル文字を書き込むバッファサイズがない場合はキャンセル
		if( BufferBytes - DestSize < 2 + 1 )
		{
			return ;
		}

		if( DestStr != NULL )
		{
			( ( BYTE * )DestStr )[ 0 ] = ( BYTE )( 0xc0 | ( ( DestCode >> 6 ) & 0x1f ) ) ;
			( ( BYTE * )DestStr )[ 1 ] = ( BYTE )( 0x80 | ( ( DestCode      ) & 0x3f ) ) ;
			DestStr += 2 ;
		}

		DestSize += 2 ;
	}
	else
	if( DestCode <= 0xffff )
	{
		// 1文字 + ヌル文字を書き込むバッファサイズがない場合はキャンセル
		if( BufferBytes - DestSize < 3 + 1 )
		{
			return ;
		}

		if( DestStr != NULL )
		{
			( ( BYTE * )DestStr )[ 0 ] = ( BYTE )( 0xe0 | ( ( DestCode >> 12 ) & 0x0f ) ) ;
			( ( BYTE * )DestStr )[ 1 ] = ( BYTE )( 0x80 | ( ( DestCode >>  6 ) & 0x3f ) ) ;
			( ( BYTE * )DestStr )[ 2 ] = ( BYTE )( 0x80 | ( ( DestCode       ) & 0x3f ) ) ;
			DestStr += 3 ;
		}

		DestSize += 3 ;
	}
	else
	if( DestCode <= 0x1fffff )
	{
		// 1文字 + ヌル文字を書き込むバッファサイズがない場合はキャンセル
		if( BufferBytes - DestSize < 4 + 1 )
		{
			return ;
		}

		if( DestStr != NULL )
		{
			( ( BYTE * )DestStr )[ 0 ] = ( BYTE )( 0xf0 | ( ( DestCode >> 18 ) & 0x07 ) ) ;
			( ( BYTE * )DestStr )[ 1 ] = ( BYTE )( 0x80 | ( ( DestCode >> 12 ) & 0x3f ) ) ;
			( ( BYTE * )DestStr )[ 2 ] = ( BYTE )( 0x80 | ( ( DestCode >>  6 ) & 0x3f ) ) ;
			( ( BYTE * )DestStr )[ 3 ] = ( BYTE )( 0x80 | ( ( DestCode       ) & 0x3f ) ) ;
			DestStr += 4 ;
		}

		DestSize += 4 ;
	}
	else
	if( DestCode <= 0x3ffffff )
	{
		// 1文字 + ヌル文字を書き込むバッファサイズがない場合はキャンセル
		if( BufferBytes - DestSize < 5 + 1 )
		{
			return ;
		}

		if( DestStr != NULL )
		{
			( ( BYTE * )DestStr )[ 0 ] = ( BYTE )( 0xf8 | ( ( DestCode >> 24 ) & 0x03 ) ) ;
			( ( BYTE * )DestStr )[ 1 ] = ( BYTE )( 0x80 | ( ( DestCode >> 18 ) & 0x3f ) ) ;
			( ( BYTE * )DestStr )[ 2 ] = ( BYTE )( 0x80 | ( ( DestCode >> 12 ) & 0x3f ) ) ;
			( ( BYTE * )DestStr )[ 3 ] = ( BYTE )( 0x80 | ( ( DestCode >>  6 ) & 0x3f ) ) ;
			( ( BYTE * )DestStr )[ 4 ] = ( BYTE )( 0x80 | ( ( DestCode       ) & 0x3f ) ) ;
			DestStr += 5 ;
		}

		DestSize += 5 ;
	}
	else
	if( DestCode <= 0x7fffffff )
	{
		// 1文字 + ヌル文字を書き込むバッファサイズがない場合はキャンセル
		if( BufferBytes - DestSize < 6 + 1 )
		{
			return ;
		}

		if( DestStr != NULL )
		{
			( ( BYTE * )DestStr )[ 0 ] = ( BYTE )( 0xfc | ( ( DestCode >> 30 ) & 0x01 ) ) ;
			( ( BYTE * )DestStr )[ 1 ] = ( BYTE )( 0x80 | ( ( DestCode >> 24 ) & 0x3f ) ) ;
			( ( BYTE * )DestStr )[ 2 ] = ( BYTE )( 0x80 | ( ( DestCode >> 18 ) & 0x3f ) ) ;
			( ( BYTE * )DestStr )[ 3 ] = ( BYTE )( 0x80 | ( ( DestCode >> 12 ) & 0x3f ) ) ;
			( ( BYTE * )DestStr )[ 4 ] = ( BYTE )( 0x80 | ( ( DestCode >>  6 ) & 0x3f ) ) ;
			( ( BYTE * )DestStr )[ 5 ] = ( BYTE )( 0x80 | ( ( DestCode       ) & 0x3f ) ) ;
			DestStr += 6 ;
		}

		DestSize += 6 ;
	}
	else
	{
		DestSize += 0 ;
	}
}

// ConvString の UnicodeからUTF32LEとして書き込むまでの処理
__inline void ConvString_DestCode_UTF32LE( BYTE *&DestStr, DWORD &DestCode, size_t &DestSize, size_t BufferBytes )
{
	// 1文字 + ヌル文字を書き込むバッファサイズがない場合はキャンセル
	if( BufferBytes - DestSize < 4 + 4 )
	{
		return ;
	}

	if( DestStr != NULL )
	{
		( ( BYTE * )DestStr )[ 0 ] = ( BYTE )( DestCode       ) ;
		( ( BYTE * )DestStr )[ 1 ] = ( BYTE )( DestCode >>  8 ) ;
		( ( BYTE * )DestStr )[ 2 ] = ( BYTE )( DestCode >> 16 ) ;
		( ( BYTE * )DestStr )[ 3 ] = ( BYTE )( DestCode >> 24 ) ;
		DestStr += 4 ;
	}

	DestSize += 4 ;
}

// ConvString の UnicodeからUTF32BEとして書き込むまでの処理
__inline void ConvString_DestCode_UTF32BE( BYTE *&DestStr, DWORD &DestCode, size_t &DestSize, size_t BufferBytes )
{
	// 1文字 + ヌル文字を書き込むバッファサイズがない場合はキャンセル
	if( BufferBytes - DestSize < 4 + 4 )
	{
		return ;
	}

	if( DestStr != NULL )
	{
		( ( BYTE * )DestStr )[ 0 ] = ( BYTE )( DestCode >> 24 ) ;
		( ( BYTE * )DestStr )[ 1 ] = ( BYTE )( DestCode >> 16 ) ;
		( ( BYTE * )DestStr )[ 2 ] = ( BYTE )( DestCode >>  8 ) ;
		( ( BYTE * )DestStr )[ 3 ] = ( BYTE )( DestCode       ) ;
		DestStr += 4 ;
	}

	DestSize += 4 ;
}

// ConvString の 1バイト単位コードの終端文字を書き込む処理
__inline void ConvString_1BYTE_NULL_CHAR( BYTE *&DestStr, size_t &DestSize )
{
	if( DestStr != NULL )
	{
		( ( BYTE * )DestStr )[ 0 ] = 0 ;
		DestStr += 1 ;
	}
	DestSize += 1 ;
}

// ConvString の 2バイト単位コードの終端文字を書き込む処理
__inline void ConvString_2BYTE_NULL_CHAR( BYTE *&DestStr, size_t &DestSize )
{
	if( DestStr != NULL )
	{
		( ( WORD * )DestStr )[ 0 ] = 0 ;
		DestStr += 2 ;
	}
	DestSize += 2 ;
}

// ConvString の 4バイト単位コードの終端文字を書き込む処理
__inline void ConvString_4BYTE_NULL_CHAR( BYTE *&DestStr, size_t &DestSize )
{
	if( DestStr != NULL )
	{
		( ( DWORD * )DestStr )[ 0 ] = 0 ;
		DestStr += 4 ;
	}
	DestSize += 4 ;
}


// ConvString の UTF16LE → UTF16BE 高速処理用
__inline int ConvString_UTF16LE_TO_UTF16BE( const char *Src, int SrcStrLength, char *Dest, size_t BufferBytes )
{
	CONVSTRING_BEGIN
	if( BufferBytes < 2 )
		return 0 ;
	if( SrcStrLength < 0 )
	{
		while( ConvString_SrcCode_UTF16LE( SrcStr, Unicode, i ) )
			ConvString_DestCode_UTF16BE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	else
	{
		for( i = 0 ; i < ( size_t )SrcStrLength && ConvString_SrcCode_UTF16LE( SrcStr, Unicode, i ); )
			ConvString_DestCode_UTF16BE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	ConvString_2BYTE_NULL_CHAR( DestStr, DestSize ) ;
	return ( int )DestSize ;
}

// ConvString の UTF16LE → UTF8 高速処理用
__inline int ConvString_UTF16LE_TO_UTF8( const char *Src, int SrcStrLength, char *Dest, size_t BufferBytes )
{
	CONVSTRING_BEGIN
	if( BufferBytes < 1 )
		return 0 ;
	if( SrcStrLength < 0 )
	{
		while( ConvString_SrcCode_UTF16LE( SrcStr, Unicode, i ) )
			ConvString_DestCode_UTF8( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	else
	{
		for( i = 0 ; i < ( size_t )SrcStrLength && ConvString_SrcCode_UTF16LE( SrcStr, Unicode, i ); )
			ConvString_DestCode_UTF8( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	ConvString_1BYTE_NULL_CHAR( DestStr, DestSize ) ;
	return ( int )DestSize ;
}

// ConvString の UTF16LE → UTF32LE 高速処理用
__inline int ConvString_UTF16LE_TO_UTF32LE( const char *Src, int SrcStrLength, char *Dest, size_t BufferBytes )
{
	CONVSTRING_BEGIN
	if( BufferBytes < 4 )
		return 0 ;
	if( SrcStrLength < 0 )
	{
		while( ConvString_SrcCode_UTF16LE( SrcStr, Unicode, i ) )
			ConvString_DestCode_UTF32LE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	else
	{
		for( i = 0 ; i < ( size_t )SrcStrLength && ConvString_SrcCode_UTF16LE( SrcStr, Unicode, i ); )
			ConvString_DestCode_UTF32LE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	ConvString_4BYTE_NULL_CHAR( DestStr, DestSize ) ;
	return ( int )DestSize ;
}

// ConvString の UTF16LE → UTF32BE 高速処理用
__inline int ConvString_UTF16LE_TO_UTF32BE( const char *Src, int SrcStrLength, char *Dest, size_t BufferBytes )
{
	CONVSTRING_BEGIN
	if( BufferBytes < 4 )
		return 0 ;
	if( SrcStrLength < 0 )
	{
		while( ConvString_SrcCode_UTF16LE( SrcStr, Unicode, i ) )
			ConvString_DestCode_UTF32BE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	else
	{
		for( i = 0 ; i < ( size_t )SrcStrLength && ConvString_SrcCode_UTF16LE( SrcStr, Unicode, i ); )
			ConvString_DestCode_UTF32BE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	ConvString_4BYTE_NULL_CHAR( DestStr, DestSize ) ;
	return ( int )DestSize ;
}


// ConvString の UTF16BE → UTF16LE 高速処理用
__inline int ConvString_UTF16BE_TO_UTF16LE( const char *Src, int SrcStrLength, char *Dest, size_t BufferBytes )
{
	CONVSTRING_BEGIN
	if( BufferBytes < 2 )
		return 0 ;
	if( SrcStrLength < 0 )
	{
		while( ConvString_SrcCode_UTF16BE( SrcStr, Unicode, i ) )
			ConvString_DestCode_UTF16LE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	else
	{
		for( i = 0 ; i < ( size_t )SrcStrLength && ConvString_SrcCode_UTF16BE( SrcStr, Unicode, i ); )
			ConvString_DestCode_UTF16LE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	ConvString_2BYTE_NULL_CHAR( DestStr, DestSize ) ;
	return ( int )DestSize ;
}

// ConvString の UTF16BE → UTF8 高速処理用
__inline int ConvString_UTF16BE_TO_UTF8( const char *Src, int SrcStrLength, char *Dest, size_t BufferBytes )
{
	CONVSTRING_BEGIN
	if( BufferBytes < 1 )
		return 0 ;
	if( SrcStrLength < 0 )
	{
		while( ConvString_SrcCode_UTF16BE( SrcStr, Unicode, i ) )
			ConvString_DestCode_UTF8( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	else
	{
		for( i = 0 ; i < ( size_t )SrcStrLength && ConvString_SrcCode_UTF16BE( SrcStr, Unicode, i ); )
			ConvString_DestCode_UTF8( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	ConvString_1BYTE_NULL_CHAR( DestStr, DestSize ) ;
	return ( int )DestSize ;
}

// ConvString の UTF16BE → UTF32LE 高速処理用
__inline int ConvString_UTF16BE_TO_UTF32LE( const char *Src, int SrcStrLength, char *Dest, size_t BufferBytes )
{
	CONVSTRING_BEGIN
	if( BufferBytes < 4 )
		return 0 ;
	if( SrcStrLength < 0 )
	{
		while( ConvString_SrcCode_UTF16BE( SrcStr, Unicode, i ) )
			ConvString_DestCode_UTF32LE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	else
	{
		for( i = 0 ; i < ( size_t )SrcStrLength && ConvString_SrcCode_UTF16BE( SrcStr, Unicode, i ); )
			ConvString_DestCode_UTF32LE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	ConvString_4BYTE_NULL_CHAR( DestStr, DestSize ) ;
	return ( int )DestSize ;
}

// ConvString の UTF16BE → UTF32BE 高速処理用
__inline int ConvString_UTF16BE_TO_UTF32BE( const char *Src, int SrcStrLength, char *Dest, size_t BufferBytes )
{
	CONVSTRING_BEGIN
	if( BufferBytes < 4 )
		return 0 ;
	if( SrcStrLength < 0 )
	{
		while( ConvString_SrcCode_UTF16BE( SrcStr, Unicode, i ) )
			ConvString_DestCode_UTF32BE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	else
	{
		for( i = 0 ; i < ( size_t )SrcStrLength && ConvString_SrcCode_UTF16BE( SrcStr, Unicode, i ); )
			ConvString_DestCode_UTF32BE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	ConvString_4BYTE_NULL_CHAR( DestStr, DestSize ) ;
	return ( int )DestSize ;
}


// ConvString の UTF8 → UTF16LE 高速処理用
__inline int ConvString_UTF8_TO_UTF16LE( const char *Src, int SrcStrLength, char *Dest, size_t BufferBytes )
{
	CONVSTRING_BEGIN
	if( BufferBytes < 2 )
		return 0 ;
	if( SrcStrLength < 0 )
	{
		while( ConvString_SrcCode_UTF8( SrcStr, Unicode, i ) )
			ConvString_DestCode_UTF16LE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	else
	{
		for( i = 0 ; i < ( size_t )SrcStrLength && ConvString_SrcCode_UTF8( SrcStr, Unicode, i ); )
			ConvString_DestCode_UTF16LE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	ConvString_2BYTE_NULL_CHAR( DestStr, DestSize ) ;
	return ( int )DestSize ;
}

// ConvString の UTF8 → UTF16BE 高速処理用
__inline int ConvString_UTF8_TO_UTF16BE( const char *Src, int SrcStrLength, char *Dest, size_t BufferBytes )
{
	CONVSTRING_BEGIN
	if( BufferBytes < 2 )
		return 0 ;
	if( SrcStrLength < 0 )
	{
		while( ConvString_SrcCode_UTF8( SrcStr, Unicode, i ) )
			ConvString_DestCode_UTF16BE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	else
	{
		for( i = 0 ; i < ( size_t )SrcStrLength && ConvString_SrcCode_UTF8( SrcStr, Unicode, i ); )
			ConvString_DestCode_UTF16BE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	ConvString_2BYTE_NULL_CHAR( DestStr, DestSize ) ;
	return ( int )DestSize ;
}

// ConvString の UTF8 → UTF32LE 高速処理用
__inline int ConvString_UTF8_TO_UTF32LE( const char *Src, int SrcStrLength, char *Dest, size_t BufferBytes )
{
	CONVSTRING_BEGIN
	if( BufferBytes < 4 )
		return 0 ;
	if( SrcStrLength < 0 )
	{
		while( ConvString_SrcCode_UTF8( SrcStr, Unicode, i ) )
			ConvString_DestCode_UTF32LE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	else
	{
		for( i = 0 ; i < ( size_t )SrcStrLength && ConvString_SrcCode_UTF8( SrcStr, Unicode, i ); )
			ConvString_DestCode_UTF32LE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	ConvString_4BYTE_NULL_CHAR( DestStr, DestSize ) ;
	return ( int )DestSize ;
}

// ConvString の UTF8 → UTF32BE 高速処理用
__inline int ConvString_UTF8_TO_UTF32BE( const char *Src, int SrcStrLength, char *Dest, size_t BufferBytes )
{
	CONVSTRING_BEGIN
	if( BufferBytes < 4 )
		return 0 ;
	if( SrcStrLength < 0 )
	{
		while( ConvString_SrcCode_UTF8( SrcStr, Unicode, i ) )
			ConvString_DestCode_UTF32BE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	else
	{
		for( i = 0 ; i < ( size_t )SrcStrLength && ConvString_SrcCode_UTF8( SrcStr, Unicode, i ); )
			ConvString_DestCode_UTF32BE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	ConvString_4BYTE_NULL_CHAR( DestStr, DestSize ) ;
	return ( int )DestSize ;
}


// ConvString の UTF32LE → UTF16LE 高速処理用
__inline int ConvString_UTF32LE_TO_UTF16LE( const char *Src, int SrcStrLength, char *Dest, size_t BufferBytes )
{
	CONVSTRING_BEGIN
	if( BufferBytes < 2 )
		return 0 ;
	if( SrcStrLength < 0 )
	{
		while( ConvString_SrcCode_UTF32LE( SrcStr, Unicode, i ) )
			ConvString_DestCode_UTF16LE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	else
	{
		for( i = 0 ; i < ( size_t )SrcStrLength && ConvString_SrcCode_UTF32LE( SrcStr, Unicode, i ); )
			ConvString_DestCode_UTF16LE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	ConvString_2BYTE_NULL_CHAR( DestStr, DestSize ) ;
	return ( int )DestSize ;
}

// ConvString の UTF32LE → UTF16BE 高速処理用
__inline int ConvString_UTF32LE_TO_UTF16BE( const char *Src, int SrcStrLength, char *Dest, size_t BufferBytes )
{
	CONVSTRING_BEGIN
	if( BufferBytes < 2 )
		return 0 ;
	if( SrcStrLength < 0 )
	{
		while( ConvString_SrcCode_UTF32LE( SrcStr, Unicode, i ) )
			ConvString_DestCode_UTF16BE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	else
	{
		for( i = 0 ; i < ( size_t )SrcStrLength && ConvString_SrcCode_UTF32LE( SrcStr, Unicode, i ); )
			ConvString_DestCode_UTF16BE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	ConvString_2BYTE_NULL_CHAR( DestStr, DestSize ) ;
	return ( int )DestSize ;
}

// ConvString の UTF32LE → UTF8 高速処理用
__inline int ConvString_UTF32LE_TO_UTF8( const char *Src, int SrcStrLength, char *Dest, size_t BufferBytes )
{
	CONVSTRING_BEGIN
	if( BufferBytes < 1 )
		return 0 ;
	if( SrcStrLength < 0 )
	{
		while( ConvString_SrcCode_UTF32LE( SrcStr, Unicode, i ) )
			ConvString_DestCode_UTF8( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	else
	{
		for( i = 0 ; i < ( size_t )SrcStrLength && ConvString_SrcCode_UTF32LE( SrcStr, Unicode, i ); )
			ConvString_DestCode_UTF8( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	ConvString_1BYTE_NULL_CHAR( DestStr, DestSize ) ;
	return ( int )DestSize ;
}

// ConvString の UTF32LE → UTF32BE 高速処理用
__inline int ConvString_UTF32LE_TO_UTF32BE( const char *Src, int SrcStrLength, char *Dest, size_t BufferBytes )
{
	CONVSTRING_BEGIN
	if( BufferBytes < 4 )
		return 0 ;
	if( SrcStrLength < 0 )
	{
		while( ConvString_SrcCode_UTF32LE( SrcStr, Unicode, i ) )
			ConvString_DestCode_UTF32BE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	else
	{
		for( i = 0 ; i < ( size_t )SrcStrLength && ConvString_SrcCode_UTF32LE( SrcStr, Unicode, i ); )
			ConvString_DestCode_UTF32BE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	ConvString_4BYTE_NULL_CHAR( DestStr, DestSize ) ;
	return ( int )DestSize ;
}


// ConvString の UTF32BE → UTF16LE 高速処理用
__inline int ConvString_UTF32BE_TO_UTF16LE( const char *Src, int SrcStrLength, char *Dest, size_t BufferBytes )
{
	CONVSTRING_BEGIN
	if( BufferBytes < 2 )
		return 0 ;
	if( SrcStrLength < 0 )
	{
		while( ConvString_SrcCode_UTF32BE( SrcStr, Unicode, i ) )
			ConvString_DestCode_UTF16LE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	else
	{
		for( i = 0 ; i < ( size_t )SrcStrLength && ConvString_SrcCode_UTF32BE( SrcStr, Unicode, i ); )
			ConvString_DestCode_UTF16LE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	ConvString_2BYTE_NULL_CHAR( DestStr, DestSize ) ;
	return ( int )DestSize ;
}

// ConvString の UTF32BE → UTF16BE 高速処理用
__inline int ConvString_UTF32BE_TO_UTF16BE( const char *Src, int SrcStrLength, char *Dest, size_t BufferBytes )
{
	CONVSTRING_BEGIN
	if( BufferBytes < 2 )
		return 0 ;
	if( SrcStrLength < 0 )
	{
		while( ConvString_SrcCode_UTF32BE( SrcStr, Unicode, i ) )
			ConvString_DestCode_UTF16BE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	else
	{
		for( i = 0 ; i < ( size_t )SrcStrLength && ConvString_SrcCode_UTF32BE( SrcStr, Unicode, i ); )
			ConvString_DestCode_UTF16BE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	ConvString_2BYTE_NULL_CHAR( DestStr, DestSize ) ;
	return ( int )DestSize ;
}

// ConvString の UTF32BE → UTF8 高速処理用
__inline int ConvString_UTF32BE_TO_UTF8( const char *Src, int SrcStrLength, char *Dest, size_t BufferBytes )
{
	CONVSTRING_BEGIN
	if( BufferBytes < 1 )
		return 0 ;
	if( SrcStrLength < 0 )
	{
		while( ConvString_SrcCode_UTF32BE( SrcStr, Unicode, i ) )
			ConvString_DestCode_UTF8( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	else
	{
		for( i = 0 ; i < ( size_t )SrcStrLength && ConvString_SrcCode_UTF32BE( SrcStr, Unicode, i ); )
			ConvString_DestCode_UTF8( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	ConvString_1BYTE_NULL_CHAR( DestStr, DestSize ) ;
	return ( int )DestSize ;
}

// ConvString の UTF32BE → UTF32LE 高速処理用
__inline int ConvString_UTF32BE_TO_UTF32LE( const char *Src, int SrcStrLength, char *Dest, size_t BufferBytes )
{
	CONVSTRING_BEGIN
	if( BufferBytes < 4 )
		return 0 ;
	if( SrcStrLength < 0 )
	{
		while( ConvString_SrcCode_UTF32BE( SrcStr, Unicode, i ) )
			ConvString_DestCode_UTF32LE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	else
	{
		for( i = 0 ; i < ( size_t )SrcStrLength && ConvString_SrcCode_UTF32BE( SrcStr, Unicode, i ); )
			ConvString_DestCode_UTF32LE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	ConvString_4BYTE_NULL_CHAR( DestStr, DestSize ) ;
	return ( int )DestSize ;
}



// ConvString の ShiftJIS → ASCII 高速処理用
__inline int ConvString_SHIFTJIS_TO_ASCII( const char *Src, int SrcStrLength, char *Dest, size_t BufferBytes )
{
	CONVSTRING_BEGIN
	if( BufferBytes < 1 )
		return 0 ;
	if( SrcStrLength < 0 )
	{
		while( ConvString_SrcCode_SHIFTJIS( SrcStr, Unicode, i ) )
			ConvString_DestCode_ASCII( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	else
	{
		for( i = 0 ; i < ( size_t )SrcStrLength && ConvString_SrcCode_SHIFTJIS( SrcStr, Unicode, i ); )
			ConvString_DestCode_ASCII( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	ConvString_1BYTE_NULL_CHAR( DestStr, DestSize ) ;
	return ( int )DestSize ;
}

// ConvString の ShiftJIS → UTF16LE 高速処理用
__inline int ConvString_SHIFTJIS_TO_UTF16LE( const char *Src, int SrcStrLength, char *Dest, size_t BufferBytes )
{
	CONVSTRING_BEGIN
	if( BufferBytes < 2 )
		return 0 ;
	if( SrcStrLength < 0 )
	{
		while( ConvString_SrcCode_SHIFTJIS( SrcStr, Unicode, i ) )
			ConvString_DestCode_UTF16LE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	else
	{
		for( i = 0 ; i < ( size_t )SrcStrLength && ConvString_SrcCode_SHIFTJIS( SrcStr, Unicode, i ); )
			ConvString_DestCode_UTF16LE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	ConvString_2BYTE_NULL_CHAR( DestStr, DestSize ) ;
	return ( int )DestSize ;
}

// ConvString の ShiftJIS → UTF16BE 高速処理用
__inline int ConvString_SHIFTJIS_TO_UTF16BE( const char *Src, int SrcStrLength, char *Dest, size_t BufferBytes )
{
	CONVSTRING_BEGIN
	if( BufferBytes < 2 )
		return 0 ;
	if( SrcStrLength < 0 )
	{
		while( ConvString_SrcCode_SHIFTJIS( SrcStr, Unicode, i ) )
			ConvString_DestCode_UTF16BE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	else
	{
		for( i = 0 ; i < ( size_t )SrcStrLength && ConvString_SrcCode_SHIFTJIS( SrcStr, Unicode, i ); )
			ConvString_DestCode_UTF16BE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	ConvString_2BYTE_NULL_CHAR( DestStr, DestSize ) ;
	return ( int )DestSize ;
}

// ConvString の ShiftJIS → UTF8 高速処理用
__inline int ConvString_SHIFTJIS_TO_UTF8( const char *Src, int SrcStrLength, char *Dest, size_t BufferBytes )
{
	CONVSTRING_BEGIN
	if( BufferBytes < 1 )
		return 0 ;
	if( SrcStrLength < 0 )
	{
		while( ConvString_SrcCode_SHIFTJIS( SrcStr, Unicode, i ) )
			ConvString_DestCode_UTF8( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	else
	{
		for( i = 0 ; i < ( size_t )SrcStrLength && ConvString_SrcCode_SHIFTJIS( SrcStr, Unicode, i ); )
			ConvString_DestCode_UTF8( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	ConvString_1BYTE_NULL_CHAR( DestStr, DestSize ) ;
	return ( int )DestSize ;
}

// ConvString の ShiftJIS → UTF32LE 高速処理用
__inline int ConvString_SHIFTJIS_TO_UTF32LE( const char *Src, int SrcStrLength, char *Dest, size_t BufferBytes )
{
	CONVSTRING_BEGIN
	if( BufferBytes < 4 )
		return 0 ;
	if( SrcStrLength < 0 )
	{
		while( ConvString_SrcCode_SHIFTJIS( SrcStr, Unicode, i ) )
			ConvString_DestCode_UTF32LE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	else
	{
		for( i = 0 ; i < ( size_t )SrcStrLength && ConvString_SrcCode_SHIFTJIS( SrcStr, Unicode, i ); )
			ConvString_DestCode_UTF32LE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	ConvString_4BYTE_NULL_CHAR( DestStr, DestSize ) ;
	return ( int )DestSize ;
}

// ConvString の ShiftJIS → UTF32BE 高速処理用
__inline int ConvString_SHIFTJIS_TO_UTF32BE( const char *Src, int SrcStrLength, char *Dest, size_t BufferBytes )
{
	CONVSTRING_BEGIN
	if( BufferBytes < 4 )
		return 0 ;
	if( SrcStrLength < 0 )
	{
		while( ConvString_SrcCode_SHIFTJIS( SrcStr, Unicode, i ) )
			ConvString_DestCode_UTF32BE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	else
	{
		for( i = 0 ; i < ( size_t )SrcStrLength && ConvString_SrcCode_SHIFTJIS( SrcStr, Unicode, i ); )
			ConvString_DestCode_UTF32BE( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	ConvString_4BYTE_NULL_CHAR( DestStr, DestSize ) ;
	return ( int )DestSize ;
}


// ConvString の UTF16LE → ASCII 高速処理用
__inline int ConvString_UTF16LE_TO_ASCII( const char *Src, int SrcStrLength, char *Dest, size_t BufferBytes )
{
	CONVSTRING_BEGIN
	if( BufferBytes < 1 )
		return 0 ;
	if( SrcStrLength < 0 )
	{
		while( ConvString_SrcCode_UTF16LE( SrcStr, Unicode, i ) )
			ConvString_DestCode_ASCII( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	else
	{
		for( i = 0 ; i < ( size_t )SrcStrLength && ConvString_SrcCode_UTF16LE( SrcStr, Unicode, i ); )
			ConvString_DestCode_ASCII( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	ConvString_1BYTE_NULL_CHAR( DestStr, DestSize ) ;
	return ( int )DestSize ;
}

// ConvString の UTF16LE → ShiftJIS 高速処理用
__inline int ConvString_UTF16LE_TO_SHIFTJIS( const char *Src, int SrcStrLength, char *Dest, size_t BufferBytes )
{
	CONVSTRING_BEGIN
	if( BufferBytes < 1 )
		return 0 ;
	if( SrcStrLength < 0 )
	{
		while( ConvString_SrcCode_UTF16LE( SrcStr, Unicode, i ) )
			ConvString_DestCode_SHIFTJIS( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	else
	{
		for( i = 0 ; i < ( size_t )SrcStrLength && ConvString_SrcCode_UTF16LE( SrcStr, Unicode, i ); )
			ConvString_DestCode_SHIFTJIS( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	ConvString_1BYTE_NULL_CHAR( DestStr, DestSize ) ;
	return ( int )DestSize ;
}

// ConvString の UTF16BE → ASCII 高速処理用
__inline int ConvString_UTF16BE_TO_ASCII( const char *Src, int SrcStrLength, char *Dest, size_t BufferBytes )
{
	CONVSTRING_BEGIN
	if( BufferBytes < 1 )
		return 0 ;
	if( SrcStrLength < 0 )
	{
		while( ConvString_SrcCode_UTF16BE( SrcStr, Unicode, i ) )
			ConvString_DestCode_ASCII( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	else
	{
		for( i = 0 ; i < ( size_t )SrcStrLength && ConvString_SrcCode_UTF16BE( SrcStr, Unicode, i ); )
			ConvString_DestCode_ASCII( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	ConvString_1BYTE_NULL_CHAR( DestStr, DestSize ) ;
	return ( int )DestSize ;
}

// ConvString の UTF16BE → ShiftJIS 高速処理用
__inline int ConvString_UTF16BE_TO_SHIFTJIS( const char *Src, int SrcStrLength, char *Dest, size_t BufferBytes )
{
	CONVSTRING_BEGIN
	if( BufferBytes < 1 )
		return 0 ;
	if( SrcStrLength < 0 )
	{
		while( ConvString_SrcCode_UTF16BE( SrcStr, Unicode, i ) )
			ConvString_DestCode_SHIFTJIS( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	else
	{
		for( i = 0 ; i < ( size_t )SrcStrLength && ConvString_SrcCode_UTF16BE( SrcStr, Unicode, i ); )
			ConvString_DestCode_SHIFTJIS( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	ConvString_1BYTE_NULL_CHAR( DestStr, DestSize ) ;
	return ( int )DestSize ;
}

// ConvString の UTF8 → ASCII 高速処理用
__inline int ConvString_UTF8_TO_ASCII( const char *Src, int SrcStrLength, char *Dest, size_t BufferBytes )
{
	CONVSTRING_BEGIN
	if( BufferBytes < 1 )
		return 0 ;
	if( SrcStrLength < 0 )
	{
		while( ConvString_SrcCode_UTF8( SrcStr, Unicode, i ) )
			ConvString_DestCode_ASCII( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	else
	{
		for( i = 0 ; i < ( size_t )SrcStrLength && ConvString_SrcCode_UTF8( SrcStr, Unicode, i ); )
			ConvString_DestCode_ASCII( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	ConvString_1BYTE_NULL_CHAR( DestStr, DestSize ) ;
	return ( int )DestSize ;
}

// ConvString の UTF8 → ShiftJIS 高速処理用
__inline int ConvString_UTF8_TO_SHIFTJIS( const char *Src, int SrcStrLength, char *Dest, size_t BufferBytes )
{
	CONVSTRING_BEGIN
	if( BufferBytes < 1 )
		return 0 ;
	if( SrcStrLength < 0 )
	{
		while( ConvString_SrcCode_UTF8( SrcStr, Unicode, i ) )
			ConvString_DestCode_SHIFTJIS( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	else
	{
		for( i = 0 ; i < ( size_t )SrcStrLength && ConvString_SrcCode_UTF8( SrcStr, Unicode, i ); )
			ConvString_DestCode_SHIFTJIS( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	ConvString_1BYTE_NULL_CHAR( DestStr, DestSize ) ;
	return ( int )DestSize ;
}

// ConvString の UTF32LE → ASCII 高速処理用
__inline int ConvString_UTF32LE_TO_ASCII( const char *Src, int SrcStrLength, char *Dest, size_t BufferBytes )
{
	CONVSTRING_BEGIN
	if( BufferBytes < 1 )
		return 0 ;
	if( SrcStrLength < 0 )
	{
		while( ConvString_SrcCode_UTF32LE( SrcStr, Unicode, i ) )
			ConvString_DestCode_ASCII( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	else
	{
		for( i = 0 ; i < ( size_t )SrcStrLength && ConvString_SrcCode_UTF32LE( SrcStr, Unicode, i ); )
			ConvString_DestCode_ASCII( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	ConvString_1BYTE_NULL_CHAR( DestStr, DestSize ) ;
	return ( int )DestSize ;
}

// ConvString の UTF32LE → ShiftJIS 高速処理用
__inline int ConvString_UTF32LE_TO_SHIFTJIS( const char *Src, int SrcStrLength, char *Dest, size_t BufferBytes )
{
	CONVSTRING_BEGIN
	if( BufferBytes < 1 )
		return 0 ;
	if( SrcStrLength < 0 )
	{
		while( ConvString_SrcCode_UTF32LE( SrcStr, Unicode, i ) )
			ConvString_DestCode_SHIFTJIS( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	else
	{
		for( i = 0 ; i < ( size_t )SrcStrLength && ConvString_SrcCode_UTF32LE( SrcStr, Unicode, i ); )
			ConvString_DestCode_SHIFTJIS( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	ConvString_1BYTE_NULL_CHAR( DestStr, DestSize ) ;
	return ( int )DestSize ;
}

// ConvString の UTF32BE → ASCII 高速処理用
__inline int ConvString_UTF32BE_TO_ASCII( const char *Src, int SrcStrLength, char *Dest, size_t BufferBytes )
{
	CONVSTRING_BEGIN
	if( BufferBytes < 1 )
		return 0 ;
	if( SrcStrLength < 0 )
	{
		while( ConvString_SrcCode_UTF32BE( SrcStr, Unicode, i ) )
			ConvString_DestCode_ASCII( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	else
	{
		for( i = 0 ; i < ( size_t )SrcStrLength && ConvString_SrcCode_UTF32BE( SrcStr, Unicode, i ); )
			ConvString_DestCode_ASCII( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	ConvString_1BYTE_NULL_CHAR( DestStr, DestSize ) ;
	return ( int )DestSize ;
}

// ConvString の UTF32BE → ShiftJIS 高速処理用
__inline int ConvString_UTF32BE_TO_SHIFTJIS( const char *Src, int SrcStrLength, char *Dest, size_t BufferBytes )
{
	CONVSTRING_BEGIN
	if( BufferBytes < 1 )
		return 0 ;
	if( SrcStrLength < 0 )
	{
		while( ConvString_SrcCode_UTF32BE( SrcStr, Unicode, i ) )
			ConvString_DestCode_SHIFTJIS( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	else
	{
		for( i = 0 ; i < ( size_t )SrcStrLength && ConvString_SrcCode_UTF32BE( SrcStr, Unicode, i ); )
			ConvString_DestCode_SHIFTJIS( DestStr, Unicode, DestSize, BufferBytes ) ;
	}
	ConvString_1BYTE_NULL_CHAR( DestStr, DestSize ) ;
	return ( int )DestSize ;
}

// 文字列を指定の文字コード形式の文字列に変換する( 戻り値：変換後のサイズ、ヌル文字含む( 単位：バイト ) )
extern int ConvString( const char *Src, int SrcStrLength, int SrcCharCodeFormat, char *Dest, size_t BufferBytes, int DestCharCodeFormat )
{
	// キャラクターコードテーブルが初期化されていなかったら初期化
	if( g_CharCodeSystem.InitializeFlag == FALSE )
	{
		InitCharCode() ;
	}

	// 変換元と変換先の文字コードが一致している場合は単純な文字列コピーを行う
	if( SrcCharCodeFormat == DestCharCodeFormat )
	{
		size_t i ;

		switch( GetCharCodeFormatUnitSize_inline( SrcCharCodeFormat ) )
		{
		case 1 :
			if( BufferBytes < 1 )
			{
				return 0 ;
			}
			for( i = 0 ; i * 1 < BufferBytes - 1 && ( SrcStrLength < 0 || i < ( size_t )SrcStrLength ) && ( ( BYTE * )Src )[ i ] != 0 ; i ++ )
			{
				( ( BYTE * )Dest )[ i ] = ( ( BYTE * )Src )[ i ] ;
			}
			( ( BYTE * )Dest )[ i ] = 0 ;
			i++ ;
			return ( int )i ;

		case 2 :
			if( BufferBytes < 2 )
			{
				return 0 ;
			}
			for( i = 0 ; i * 2 < BufferBytes - 2 && ( SrcStrLength < 0 || i < ( size_t )SrcStrLength ) && ( ( WORD * )Src )[ i ] != 0 ; i ++ )
			{
				( ( WORD * )Dest )[ i ] = ( ( WORD * )Src )[ i ] ;
			}
			( ( WORD * )Dest )[ i ] = 0 ;
			i ++ ;
			return ( int )( i * 2 ) ;

		case 4 :
			if( BufferBytes < 4 )
			{
				return 0 ;
			}
			for( i = 0 ; i * 4 < BufferBytes - 4 && ( SrcStrLength < 0 || i < ( size_t )SrcStrLength ) && ( ( DWORD * )Src )[ i ] != 0 ; i ++ )
			{
				( ( DWORD * )Dest )[ i ] = ( ( DWORD * )Src )[ i ] ;
			}
			( ( DWORD * )Dest )[ i ] = 0 ;
			i ++ ;
			return ( int )( i * 4 ) ;
		}
		return -1 ;
	}

	// 高速処理用の関数がある場合はそちらを使用する
	switch( SrcCharCodeFormat )
	{
	case DX_CHARCODEFORMAT_SHIFTJIS :
		switch( DestCharCodeFormat )
		{
		case DX_CHARCODEFORMAT_ASCII   : return ConvString_SHIFTJIS_TO_ASCII(   Src, SrcStrLength, Dest, BufferBytes ) ;
		case DX_CHARCODEFORMAT_UTF16LE : return ConvString_SHIFTJIS_TO_UTF16LE( Src, SrcStrLength, Dest, BufferBytes ) ;
		case DX_CHARCODEFORMAT_UTF16BE : return ConvString_SHIFTJIS_TO_UTF16BE( Src, SrcStrLength, Dest, BufferBytes ) ;
		case DX_CHARCODEFORMAT_UTF8    : return ConvString_SHIFTJIS_TO_UTF8(    Src, SrcStrLength, Dest, BufferBytes ) ;
		case DX_CHARCODEFORMAT_UTF32LE : return ConvString_SHIFTJIS_TO_UTF32LE( Src, SrcStrLength, Dest, BufferBytes ) ;
		case DX_CHARCODEFORMAT_UTF32BE : return ConvString_SHIFTJIS_TO_UTF32BE( Src, SrcStrLength, Dest, BufferBytes ) ;
		}
		break ;

	case DX_CHARCODEFORMAT_UTF16LE :
		switch( DestCharCodeFormat )
		{
		case DX_CHARCODEFORMAT_ASCII    : return ConvString_UTF16LE_TO_ASCII(    Src, SrcStrLength, Dest, BufferBytes ) ;
		case DX_CHARCODEFORMAT_SHIFTJIS : return ConvString_UTF16LE_TO_SHIFTJIS( Src, SrcStrLength, Dest, BufferBytes ) ;
		case DX_CHARCODEFORMAT_UTF16BE  : return ConvString_UTF16LE_TO_UTF16BE(  Src, SrcStrLength, Dest, BufferBytes ) ;
		case DX_CHARCODEFORMAT_UTF8     : return ConvString_UTF16LE_TO_UTF8(     Src, SrcStrLength, Dest, BufferBytes ) ;
		case DX_CHARCODEFORMAT_UTF32LE  : return ConvString_UTF16LE_TO_UTF32LE(  Src, SrcStrLength, Dest, BufferBytes ) ;
		case DX_CHARCODEFORMAT_UTF32BE  : return ConvString_UTF16LE_TO_UTF32BE(  Src, SrcStrLength, Dest, BufferBytes ) ;
		}
		break ;

	case DX_CHARCODEFORMAT_UTF16BE :
		switch( DestCharCodeFormat )
		{
		case DX_CHARCODEFORMAT_ASCII    : return ConvString_UTF16BE_TO_ASCII(    Src, SrcStrLength, Dest, BufferBytes ) ;
		case DX_CHARCODEFORMAT_SHIFTJIS : return ConvString_UTF16BE_TO_SHIFTJIS( Src, SrcStrLength, Dest, BufferBytes ) ;
		case DX_CHARCODEFORMAT_UTF16LE  : return ConvString_UTF16BE_TO_UTF16LE(  Src, SrcStrLength, Dest, BufferBytes ) ;
		case DX_CHARCODEFORMAT_UTF8     : return ConvString_UTF16BE_TO_UTF8(     Src, SrcStrLength, Dest, BufferBytes ) ;
		case DX_CHARCODEFORMAT_UTF32LE  : return ConvString_UTF16BE_TO_UTF32LE(  Src, SrcStrLength, Dest, BufferBytes ) ;
		case DX_CHARCODEFORMAT_UTF32BE  : return ConvString_UTF16BE_TO_UTF32BE(  Src, SrcStrLength, Dest, BufferBytes ) ;
		}
		break ;

	case DX_CHARCODEFORMAT_UTF8 :
		switch( DestCharCodeFormat )
		{
		case DX_CHARCODEFORMAT_ASCII    : return ConvString_UTF8_TO_ASCII(    Src, SrcStrLength, Dest, BufferBytes ) ;
		case DX_CHARCODEFORMAT_SHIFTJIS : return ConvString_UTF8_TO_SHIFTJIS( Src, SrcStrLength, Dest, BufferBytes ) ;
		case DX_CHARCODEFORMAT_UTF16LE  : return ConvString_UTF8_TO_UTF16LE(  Src, SrcStrLength, Dest, BufferBytes ) ;
		case DX_CHARCODEFORMAT_UTF16BE  : return ConvString_UTF8_TO_UTF16BE(  Src, SrcStrLength, Dest, BufferBytes ) ;
		case DX_CHARCODEFORMAT_UTF32LE  : return ConvString_UTF8_TO_UTF32LE(  Src, SrcStrLength, Dest, BufferBytes ) ;
		case DX_CHARCODEFORMAT_UTF32BE  : return ConvString_UTF8_TO_UTF32BE(  Src, SrcStrLength, Dest, BufferBytes ) ;
		}
		break ;

	case DX_CHARCODEFORMAT_UTF32LE :
		switch( DestCharCodeFormat )
		{
		case DX_CHARCODEFORMAT_ASCII    : return ConvString_UTF32LE_TO_ASCII(    Src, SrcStrLength, Dest, BufferBytes ) ;
		case DX_CHARCODEFORMAT_SHIFTJIS : return ConvString_UTF32LE_TO_SHIFTJIS( Src, SrcStrLength, Dest, BufferBytes ) ;
		case DX_CHARCODEFORMAT_UTF16LE  : return ConvString_UTF32LE_TO_UTF16LE(  Src, SrcStrLength, Dest, BufferBytes ) ;
		case DX_CHARCODEFORMAT_UTF16BE  : return ConvString_UTF32LE_TO_UTF16BE(  Src, SrcStrLength, Dest, BufferBytes ) ;
		case DX_CHARCODEFORMAT_UTF8     : return ConvString_UTF32LE_TO_UTF8(     Src, SrcStrLength, Dest, BufferBytes ) ;
		case DX_CHARCODEFORMAT_UTF32BE  : return ConvString_UTF32LE_TO_UTF32BE(  Src, SrcStrLength, Dest, BufferBytes ) ;
		}
		break ;

	case DX_CHARCODEFORMAT_UTF32BE :
		switch( DestCharCodeFormat )
		{
		case DX_CHARCODEFORMAT_ASCII    : return ConvString_UTF32BE_TO_ASCII(    Src, SrcStrLength, Dest, BufferBytes ) ;
		case DX_CHARCODEFORMAT_SHIFTJIS : return ConvString_UTF32BE_TO_SHIFTJIS( Src, SrcStrLength, Dest, BufferBytes ) ;
		case DX_CHARCODEFORMAT_UTF16LE  : return ConvString_UTF32BE_TO_UTF16LE(  Src, SrcStrLength, Dest, BufferBytes ) ;
		case DX_CHARCODEFORMAT_UTF16BE  : return ConvString_UTF32BE_TO_UTF16BE(  Src, SrcStrLength, Dest, BufferBytes ) ;
		case DX_CHARCODEFORMAT_UTF8     : return ConvString_UTF32BE_TO_UTF8(     Src, SrcStrLength, Dest, BufferBytes ) ;
		case DX_CHARCODEFORMAT_UTF32LE  : return ConvString_UTF32BE_TO_UTF32LE(  Src, SrcStrLength, Dest, BufferBytes ) ;
		}
		break ;
	}

	// 無かった場合は共通の処理を行う
	{
		size_t DestSize ;
		DWORD SrcCode ;
		DWORD DestCode ;
		BYTE *DestStr ;
		const BYTE *SrcStr ;
		int UseSrcBytes ;
		size_t i = 0 ;
		size_t WriteBytes ;
		size_t NulWriteBytes ;

		SrcStr  = ( BYTE * )Src ;
		DestStr = ( BYTE * )Dest ;
		DestSize = 0 ;

		// 最初にヌル文字を書き込む
		NulWriteBytes = PutCharCode_BufferBytes_inline( 0x00, DestCharCodeFormat, ( char * )DestStr, BufferBytes - DestSize ) ;
		if( NulWriteBytes == 0 )
		{
			return 0 ;
		}

		for( i = 0; SrcStrLength < 0 || i < ( size_t )SrcStrLength; )
		{
			SrcCode = GetCharCode_inline( ( const char * )SrcStr, SrcCharCodeFormat, &UseSrcBytes, i ) ;
			if( SrcCode == 0 )
			{
				break ;
			}
			SrcStr += UseSrcBytes ;

			DestCode = ConvCharCode_inline( SrcCode, SrcCharCodeFormat, DestCharCodeFormat ) ;

			WriteBytes = PutCharCode_BufferBytes_inline( DestCode, DestCharCodeFormat, ( char * )DestStr, BufferBytes - DestSize ) ;
			if( WriteBytes == 0 || BufferBytes - DestSize < WriteBytes + NulWriteBytes )
			{
				goto END ;
			}
			if( DestStr != NULL )
			{
				DestStr += WriteBytes ;
			}
			DestSize += WriteBytes ;
			if( BufferBytes - DestSize <= NulWriteBytes )
			{
				goto END ;
			}
		}

END :

		// 終端文字をセット
		DestSize += PutCharCode_BufferBytes_inline( 0, DestCharCodeFormat, ( char * )DestStr, BufferBytes - DestSize ) ;

		return ( int )DestSize ;
	}
}

// 文字列に含まれる文字数を取得する
extern int GetStringCharNum( const char *String, int CharCodeFormat )
{
	DWORD CharCode ;
	int CharBytes ;
	int Count ;
	int Address ;
	size_t i = 0 ;

	Address = 0 ;
	Count = 0 ;
	for(;;)
	{
		CharCode = GetCharCode_inline( ( const char * )&( ( BYTE * )String )[ Address ], CharCodeFormat, &CharBytes, i ) ;
		if( CharCode == 0 )
		{
			break ;
		}

		Address += CharBytes ;
		Count ++ ;
	}

	return Count ;
}

// 指定番号の文字のアドレスを取得する
extern const char *GetStringCharAddress( const char *String, int CharCodeFormat, int Index )
{
	DWORD CharCode ;
	int CharBytes ;
	int Count ;
	int Address ;
	size_t i = 0 ;

	Address = 0 ;
	Count = 0 ;
	for(;;)
	{
		if( Count == Index )
		{
			return ( const char * )&( ( BYTE * )String )[ Address ] ;
		}

		CharCode = GetCharCode_inline( ( const char * )&( ( BYTE * )String )[ Address ], CharCodeFormat, &CharBytes, i ) ;
		if( CharCode == 0 )
		{
			break ;
		}

		Address += CharBytes ;
		Count ++ ;
	}

	return NULL ;
}

// 指定番号の文字のコードを取得する
extern DWORD GetStringCharCode( const char *String, int CharCodeFormat, int Index )
{
	DWORD CharCode ;
	int CharBytes ;
	int Count ;
	int Address ;
	size_t i = 0 ;

	Address = 0 ;
	Count = 0 ;
	for(;;)
	{
		CharCode = GetCharCode_inline( ( const char * )&( ( BYTE * )String )[ Address ], CharCodeFormat, &CharBytes, i ) ;
		if( CharCode == 0 || Count == Index )
		{
			break ;
		}

		Address += CharBytes ;
		Count ++ ;
	}

	return CharCode ;
}








extern void CL_strcpy( int CharCodeFormat, char *Dest, const char *Src )
{
	int i ;

	switch( GetCharCodeFormatUnitSize_inline( CharCodeFormat ) )
	{
	case 1 :
		for( i = 0 ; ( ( BYTE * )Src )[ i ] != 0 ; i ++ )
		{
			( ( BYTE * )Dest )[ i ] = ( ( BYTE * )Src )[ i ] ;
		}
		( ( BYTE * )Dest )[ i ] = 0 ;
		break ;

	case 2 :
		for( i = 0 ; ( ( WORD * )Src )[ i ] != 0 ; i ++ )
		{
			( ( WORD * )Dest )[ i ] = ( ( WORD * )Src )[ i ] ;
		}
		( ( WORD * )Dest )[ i ] = 0 ;
		break ;

	case 4 :
		for( i = 0 ; ( ( DWORD * )Src )[ i ] != 0 ; i ++ )
		{
			( ( DWORD * )Dest )[ i ] = ( ( DWORD * )Src )[ i ] ;
		}
		( ( DWORD * )Dest )[ i ] = 0 ;
		break ;
	}
}

extern void CL_strcpy_s( int CharCodeFormat, char *Dest, size_t BufferBytes, const char *Src )
{
	size_t i ;

	switch( GetCharCodeFormatUnitSize_inline( CharCodeFormat ) )
	{
	case 1 :
		if( BufferBytes < 1 )
		{
			return ;
		}
		for( i = 0 ; i * 1 < BufferBytes - 1 && ( ( BYTE * )Src )[ i ] != 0 ; i ++ )
		{
			( ( BYTE * )Dest )[ i ] = ( ( BYTE * )Src )[ i ] ;
		}
		( ( BYTE * )Dest )[ i ] = 0 ;
		break ;

	case 2 :
		if( BufferBytes < 2 )
		{
			return ;
		}
		for( i = 0 ; i * 2 < BufferBytes - 2 && ( ( WORD * )Src )[ i ] != 0 ; i ++ )
		{
			( ( WORD * )Dest )[ i ] = ( ( WORD * )Src )[ i ] ;
		}
		( ( WORD * )Dest )[ i ] = 0 ;
		break ;

	case 4 :
		if( BufferBytes < 4 )
		{
			return ;
		}
		for( i = 0 ; i * 4 < BufferBytes - 4 && ( ( DWORD * )Src )[ i ] != 0 ; i ++ )
		{
			( ( DWORD * )Dest )[ i ] = ( ( DWORD * )Src )[ i ] ;
		}
		( ( DWORD * )Dest )[ i ] = 0 ;
		break ;
	}
}

extern void CL_strpcpy( int CharCodeFormat, char *Dest, const char *Src, int Pos )
{
	int UnitSize ;
	size_t Size ;

	Size = CL_strlen( CharCodeFormat, Src ) ;
	if( Pos > ( int )Size )
	{
		return ;
	}

	UnitSize = GetCharCodeFormatUnitSize_inline( CharCodeFormat ) ;
	CL_strcpy( CharCodeFormat, Dest, ( const char * )( ( BYTE * )Src + UnitSize * Pos ) ) ;
}

extern void CL_strpcpy_s( int CharCodeFormat, char *Dest, size_t BufferBytes, const char *Src, int Pos )
{
	int UnitSize ;
	size_t Size ;

	Size = CL_strlen( CharCodeFormat, Src ) ;
	if( Pos > ( int )Size )
	{
		return ;
	}

	UnitSize = GetCharCodeFormatUnitSize_inline( CharCodeFormat ) ;
	CL_strcpy_s( CharCodeFormat, Dest, BufferBytes, ( const char * )( ( BYTE * )Src + UnitSize * Pos ) ) ;
}

extern void CL_strpcpy2( int CharCodeFormat, char *Dest, const char *Src, int Pos )
{
	const char *SrcT ;

	SrcT = CL_strpos2( CharCodeFormat, Src, Pos ) ;
	if( SrcT == NULL )
	{
		PutCharCode( 0, CharCodeFormat, Dest, BUFFERBYTES_CANCEL ) ;
		return ;
	}

	CL_strcpy( CharCodeFormat, Dest, SrcT ) ;
}

extern void CL_strpcpy2_s( int CharCodeFormat, char *Dest, size_t BufferBytes, const char *Src, int Pos )
{
	const char *SrcT ;

	SrcT = CL_strpos2( CharCodeFormat, Src, Pos ) ;
	if( SrcT == NULL )
	{
		PutCharCode( 0, CharCodeFormat, Dest, BufferBytes ) ;
		return ;
	}

	CL_strcpy_s( CharCodeFormat, Dest, BufferBytes, SrcT ) ;
}

extern void CL_strncpy( int CharCodeFormat, char *Dest, const char *Src, int Num )
{
	int i ;

	switch( GetCharCodeFormatUnitSize_inline( CharCodeFormat ) )
	{
	case 1 :
		for( i = 0 ; i < Num && ( ( BYTE * )Src )[ i ] != 0 ; i ++ )
		{
			( ( BYTE * )Dest )[ i ] = ( ( BYTE * )Src )[ i ] ;
		}
		for( ; i < Num ; i ++ )
		{
			( ( BYTE * )Dest )[ i ] = 0 ;
		}
		break ;

	case 2 :
		for( i = 0 ; i < Num && ( ( WORD * )Src )[ i ] != 0 ; i ++ )
		{
			( ( WORD * )Dest )[ i ] = ( ( WORD * )Src )[ i ] ;
		}
		for( ; i < Num ; i ++ )
		{
			( ( WORD * )Dest )[ i ] = 0 ;
		}
		break ;

	case 4 :
		for( i = 0 ; i < Num && ( ( DWORD * )Src )[ i ] != 0 ; i ++ )
		{
			( ( DWORD * )Dest )[ i ] = ( ( DWORD * )Src )[ i ] ;
		}
		for( ; i < Num ; i ++ )
		{
			( ( DWORD * )Dest )[ i ] = 0 ;
		}
		break ;
	}
}

extern void CL_strncpy_s( int CharCodeFormat, char *Dest, size_t BufferBytes, const char *Src, int Num )
{
	int i ;

	switch( GetCharCodeFormatUnitSize_inline( CharCodeFormat ) )
	{
	case 1 :
		if( BufferBytes < 1 )
		{
			return ;
		}
		if( Num > ( int )BufferBytes )
		{
			Num = ( int )BufferBytes ;
		}
		for( i = 0 ; i < Num && ( ( BYTE * )Src )[ i ] != 0 ; i ++ )
		{
			( ( BYTE * )Dest )[ i ] = ( ( BYTE * )Src )[ i ] ;
		}
		for( ; i < Num ; i ++ )
		{
			( ( BYTE * )Dest )[ i ] = 0 ;
		}
		break ;

	case 2 :
		if( BufferBytes < 2 )
		{
			return ;
		}
		if( Num > ( int )( BufferBytes / 2 ) )
		{
			Num = ( int )( BufferBytes / 2 ) ;
		}
		for( i = 0 ; i < Num && ( ( WORD * )Src )[ i ] != 0 ; i ++ )
		{
			( ( WORD * )Dest )[ i ] = ( ( WORD * )Src )[ i ] ;
		}
		for( ; i < Num ; i ++ )
		{
			( ( WORD * )Dest )[ i ] = 0 ;
		}
		break ;

	case 4 :
		if( BufferBytes < 4 )
		{
			return ;
		}
		if( Num > ( int )( BufferBytes / 4 ) )
		{
			Num = ( int )( BufferBytes / 4 ) ;
		}
		for( i = 0 ; i < Num && ( ( DWORD * )Src )[ i ] != 0 ; i ++ )
		{
			( ( DWORD * )Dest )[ i ] = ( ( DWORD * )Src )[ i ] ;
		}
		for( ; i < Num ; i ++ )
		{
			( ( DWORD * )Dest )[ i ] = 0 ;
		}
		break ;
	}
}

extern void CL_strncpy2( int CharCodeFormat, char *Dest, const char *Src, int Num )
{
	int i ;
	int j ;
	int Addr ;
	int Size ;

	Addr = 0 ;
	switch( GetCharCodeFormatUnitSize_inline( CharCodeFormat ) )
	{
	case 1 :
		for( i = 0 ; i < Num && ( ( BYTE * )Src )[ Addr ] != 0 ; i ++ )
		{
			Size = GetCharBytes_inline( ( const char * )( ( BYTE * )Src + Addr ), CharCodeFormat ) / 1 ;
			for( j = 0 ; j < Size ; j ++, Addr ++ )
			{
				( ( BYTE * )Dest )[ Addr ] = ( ( BYTE * )Src )[ Addr ] ;
			}
		}
		( ( BYTE * )Dest )[ Addr ] = 0 ;
		break ;

	case 2 :
		for( i = 0 ; i < Num && ( ( WORD * )Src )[ Addr ] != 0 ; i ++ )
		{
			Size = GetCharBytes_inline( ( const char * )( ( WORD * )Src + Addr ), CharCodeFormat ) >> 1 ;
			for( j = 0 ; j < Size ; j ++, Addr ++ )
			{
				( ( WORD * )Dest )[ Addr ] = ( ( WORD * )Src )[ Addr ] ;
			}
		}
		( ( WORD * )Dest )[ Addr ] = 0 ;
		break ;

	case 4 :
		for( i = 0 ; i < Num && ( ( DWORD * )Src )[ Addr ] != 0 ; i ++ )
		{
			Size = GetCharBytes_inline( ( const char * )( ( DWORD * )Src + Addr ), CharCodeFormat ) >> 2 ;
			for( j = 0 ; j < Size ; j ++, Addr ++ )
			{
				( ( DWORD * )Dest )[ Addr ] = ( ( DWORD * )Src )[ Addr ] ;
			}
		}
		( ( DWORD * )Dest )[ Addr ] = 0 ;
		break ;
	}
}

extern void CL_strncpy2_s( int CharCodeFormat, char *Dest, size_t BufferBytes, const char *Src, int Num )
{
	int i ;
	size_t j ;
	size_t Addr ;
	size_t Size ;

	Addr = 0 ;
	switch( GetCharCodeFormatUnitSize_inline( CharCodeFormat ) )
	{
	case 1 :
		if( BufferBytes < 1 )
		{
			return ;
		}
		for( i = 0 ; Addr < BufferBytes - 1 && i < Num && ( ( BYTE * )Src )[ Addr ] != 0 ; i ++ )
		{
			Size = ( size_t )GetCharBytes_inline( ( const char * )( ( BYTE * )Src + Addr ), CharCodeFormat ) / 1 ;
			if( Addr + Size > BufferBytes - 1 )
			{
				break ;
			}
			for( j = 0 ; j < Size ; j ++, Addr ++ )
			{
				( ( BYTE * )Dest )[ Addr ] = ( ( BYTE * )Src )[ Addr ] ;
			}
		}
		( ( BYTE * )Dest )[ Addr ] = 0 ;
		break ;

	case 2 :
		if( BufferBytes < 2 )
		{
			return ;
		}
		for( i = 0 ; Addr < BufferBytes - 2 && i < Num && ( ( WORD * )Src )[ Addr ] != 0 ; i ++ )
		{
			Size = GetCharBytes_inline( ( const char * )( ( WORD * )Src + Addr ), CharCodeFormat ) >> 1 ;
			if( Addr + Size > BufferBytes - 2 )
			{
				break ;
			}
			for( j = 0 ; j < Size ; j ++, Addr ++ )
			{
				( ( WORD * )Dest )[ Addr ] = ( ( WORD * )Src )[ Addr ] ;
			}
		}
		( ( WORD * )Dest )[ Addr ] = 0 ;
		break ;

	case 4 :
		if( BufferBytes < 4 )
		{
			return ;
		}
		for( i = 0 ; Addr < BufferBytes - 4 && i < Num && ( ( DWORD * )Src )[ Addr ] != 0 ; i ++ )
		{
			Size = GetCharBytes_inline( ( const char * )( ( DWORD * )Src + Addr ), CharCodeFormat ) >> 2 ;
			if( Addr + Size > BufferBytes - 4 )
			{
				break ;
			}
			for( j = 0 ; j < Size ; j ++, Addr ++ )
			{
				( ( DWORD * )Dest )[ Addr ] = ( ( DWORD * )Src )[ Addr ] ;
			}
		}
		( ( DWORD * )Dest )[ Addr ] = 0 ;
		break ;
	}
}

extern void CL_strrncpy( int CharCodeFormat, char *Dest, const char *Src, int Num )
{
	int Size ;
	int UnitSize ;

	UnitSize = GetCharCodeFormatUnitSize_inline( CharCodeFormat ) ;

	Size = ( int )CL_strlen( CharCodeFormat, Src ) ;
	if( Num > Size )
	{
		int i ;
		int Addr ;

		_MEMCPY( Dest, Src, Size * UnitSize ) ;

		Addr = Size * UnitSize ;
		for( i = 0 ; i < UnitSize ; i ++ )
		{
			( ( BYTE * )Dest )[ Addr + i ] = 0 ;
		}
	}
	else
	{
		_MEMCPY( Dest, ( BYTE * )Src + ( Size - Num ) * UnitSize, Num * UnitSize ) ;
	}
}

extern void CL_strrncpy_s( int CharCodeFormat, char *Dest, size_t BufferBytes, const char *Src, int Num )
{
	int Size ;
	int UnitSize ;

	UnitSize = GetCharCodeFormatUnitSize_inline( CharCodeFormat ) ;
	if( Num * UnitSize > ( int )BufferBytes )
	{
		Num = ( int )( BufferBytes / UnitSize ) ;
	}

	Size = ( int )CL_strlen( CharCodeFormat, Src ) ;
	if( Num > Size )
	{
		int i ;
		int Addr ;

		_MEMCPY( Dest, Src, Size * UnitSize ) ;

		Addr = Size * UnitSize ;
		for( i = 0 ; i < UnitSize ; i ++ )
		{
			( ( BYTE * )Dest )[ Addr + i ] = 0 ;
		}
	}
	else
	{
		_MEMCPY( Dest, ( BYTE * )Src + ( Size - Num ) * UnitSize, Num * UnitSize ) ;
	}
}

extern void CL_strrncpy2( int CharCodeFormat, char *Dest, const char *Src, int Num )
{
	int Size ;

	Size = ( int )CL_strlen2( CharCodeFormat, Src ) ;
	if( Num > Size )
	{
		CL_strcpy( CharCodeFormat, Dest, Src ) ;
	}
	else
	{
		const char *SrcT ;
		SrcT = CL_strpos2( CharCodeFormat, Src, Size - Num ) ;
		CL_strncpy2( CharCodeFormat, Dest, SrcT, Num ) ;
	}
}

extern void CL_strrncpy2_s( int CharCodeFormat, char *Dest, size_t BufferBytes, const char *Src, int Num )
{
	int Size ;

	Size = ( int )CL_strlen2( CharCodeFormat, Src ) ;
	if( Num > Size )
	{
		CL_strcpy_s( CharCodeFormat, Dest, BufferBytes, Src ) ;
	}
	else
	{
		const char *SrcT ;
		SrcT = CL_strpos2( CharCodeFormat, Src, Size - Num ) ;
		CL_strncpy2_s( CharCodeFormat, Dest, BufferBytes, SrcT, Num ) ;
	}
}

extern void CL_strpncpy( int CharCodeFormat, char *Dest, const char *Src, int Pos, int Num )
{
	int UnitSize ;
	int Size ;

	Size = ( int )CL_strlen( CharCodeFormat, Src ) ;
	if( Pos >= Size )
	{
		return ;
	}

	UnitSize = GetCharCodeFormatUnitSize_inline( CharCodeFormat ) ;
	CL_strncpy( CharCodeFormat, Dest, ( const char * )( ( BYTE * )Src + UnitSize * Pos ), Num ) ;
}

extern void CL_strpncpy_s( int CharCodeFormat, char *Dest, size_t BufferBytes, const char *Src, int Pos, int Num )
{
	int UnitSize ;
	int Size ;

	Size = ( int )CL_strlen( CharCodeFormat, Src ) ;
	if( Pos >= Size )
	{
		return ;
	}

	UnitSize = GetCharCodeFormatUnitSize_inline( CharCodeFormat ) ;
	CL_strncpy_s( CharCodeFormat, Dest, BufferBytes, ( const char * )( ( BYTE * )Src + UnitSize * Pos ), Num ) ;
}

extern void CL_strpncpy2( int CharCodeFormat, char *Dest, const char *Src, int Pos, int Num )
{
	const char *SrcT ;

	SrcT = CL_strpos2( CharCodeFormat, Src, Pos ) ;
	if( SrcT == NULL )
	{
		PutCharCode( 0, CharCodeFormat, Dest, BUFFERBYTES_CANCEL ) ;
		return ;
	}

	CL_strncpy2( CharCodeFormat, Dest, SrcT, Num ) ;
}

extern void CL_strpncpy2_s( int CharCodeFormat, char *Dest, size_t BufferBytes, const char *Src, int Pos, int Num )
{
	const char *SrcT ;

	SrcT = CL_strpos2( CharCodeFormat, Src, Pos ) ;
	if( SrcT == NULL )
	{
		PutCharCode( 0, CharCodeFormat, Dest, BufferBytes ) ;
		return ;
	}

	CL_strncpy2_s( CharCodeFormat, Dest, BufferBytes, SrcT, Num ) ;
}

extern void CL_strcat( int CharCodeFormat, char *Dest, const char *Src )
{
	int i ;

	switch( GetCharCodeFormatUnitSize_inline( CharCodeFormat ) )
	{
	case 1 :
		for( i = 0 ; ( ( BYTE * )Dest )[ i ] != 0 ; i ++ ){}
		CL_strcpy( CharCodeFormat, ( char * )&( ( BYTE * )Dest )[ i ], Src ) ;
		break ;

	case 2 :
		for( i = 0 ; ( ( WORD * )Dest )[ i ] != 0 ; i ++ ){}
		CL_strcpy( CharCodeFormat, ( char * )&( ( WORD * )Dest )[ i ], Src ) ;
		break ;

	case 4 :
		for( i = 0 ; ( ( DWORD * )Dest )[ i ] != 0 ; i ++ ){}
		CL_strcpy( CharCodeFormat, ( char * )&( ( DWORD * )Dest )[ i ], Src ) ;
		break ;
	}
}

extern void CL_strcat_s( int CharCodeFormat, char *Dest, size_t BufferBytes, const char *Src )
{
	size_t i ;

	switch( GetCharCodeFormatUnitSize_inline( CharCodeFormat ) )
	{
	case 1 :
		for( i = 0 ; ( ( BYTE * )Dest )[ i ] != 0 ; i ++ ){}
		if( i >= BufferBytes - 1 )
		{
			return ;
		}
		CL_strcpy_s( CharCodeFormat, ( char * )&( ( BYTE * )Dest )[ i ], BufferBytes - i, Src ) ;
		break ;

	case 2 :
		for( i = 0 ; ( ( WORD * )Dest )[ i ] != 0 ; i ++ ){}
		if( i * 2 >= BufferBytes - 2 )
		{
			return ;
		}
		CL_strcpy_s( CharCodeFormat, ( char * )&( ( WORD * )Dest )[ i ], BufferBytes - i * 2, Src ) ;
		break ;

	case 4 :
		for( i = 0 ; ( ( DWORD * )Dest )[ i ] != 0 ; i ++ ){}
		if( i * 4 >= BufferBytes - 4 )
		{
			return ;
		}
		CL_strcpy_s( CharCodeFormat, ( char * )&( ( DWORD * )Dest )[ i ], BufferBytes - i * 4, Src ) ;
		break ;
	}
}

extern size_t CL_strlen( int CharCodeFormat, const char *Str )
{
	size_t i ;

	switch( GetCharCodeFormatUnitSize_inline( CharCodeFormat ) )
	{
	case 1 :
		for( i = 0 ; ( ( BYTE  * )Str )[ i ] != 0 ; i ++ ){}
		return i ;

	case 2 :
		for( i = 0 ; ( ( WORD * )Str )[ i ] != 0 ; i ++ ){}
		return i ;

	case 4 :
		for( i = 0 ; ( ( DWORD * )Str )[ i ] != 0 ; i ++ ){}
		return i ;
	}

	return 0 ;
}

extern size_t CL_strlen2( int CharCodeFormat, const char *Str )
{
	return ( size_t )GetStringCharNum( Str, CharCodeFormat ) ;
}

extern int CL_strcmp( int CharCodeFormat, const char *Str1, const char *Str2 )
{
	int i ;

	switch( GetCharCodeFormatUnitSize_inline( CharCodeFormat ) )
	{
	case 1 :
		for( i = 0 ; ( ( BYTE * )Str1 )[ i ] != 0 &&
			         ( ( BYTE * )Str2 )[ i ] != 0 &&
					 ( ( BYTE * )Str1 )[ i ] == ( ( BYTE * )Str2 )[ i ] ; i ++ ){}
		if( ( ( BYTE * )Str1 )[ i ] == 0 &&
			( ( BYTE * )Str2 )[ i ] == 0 )
		{
			return 0 ;
		}
		return ( ( BYTE * )Str1 )[ i ] < ( ( BYTE * )Str2 )[ i ] ? -1 : 1 ;

	case 2 :
		for( i = 0 ; ( ( WORD * )Str1 )[ i ] != 0 &&
			         ( ( WORD * )Str2 )[ i ] != 0 &&
					 ( ( WORD * )Str1 )[ i ] == ( ( WORD * )Str2 )[ i ] ; i ++ ){}
		if( ( ( WORD * )Str1 )[ i ] == 0 &&
			( ( WORD * )Str2 )[ i ] == 0 )
		{
			return 0 ;
		}
		return ( ( WORD * )Str1 )[ i ] < ( ( WORD * )Str2 )[ i ] ? -1 : 1 ;

	case 4 :
		for( i = 0 ; ( ( DWORD * )Str1 )[ i ] != 0 &&
			         ( ( DWORD * )Str2 )[ i ] != 0 &&
					 ( ( DWORD * )Str1 )[ i ] == ( ( DWORD * )Str2 )[ i ] ; i ++ ){}
		if( ( ( DWORD * )Str1 )[ i ] == 0 &&
			( ( DWORD * )Str2 )[ i ] == 0 )
		{
			return 0 ;
		}
		return ( ( DWORD * )Str1 )[ i ] < ( ( DWORD * )Str2 )[ i ] ? -1 : 1 ;
	}

	return 0 ;
}

extern int CL_strcmp_str2_ascii( int CharCodeFormat, const char *Str1, const char *Str2 )
{
	int i ;

	switch( GetCharCodeFormatUnitSize_inline( CharCodeFormat ) )
	{
	case 1 :
		for( i = 0 ; ( ( BYTE * )Str1 )[ i ] != 0 &&
			         ( ( BYTE * )Str2 )[ i ] != 0 &&
					 ( ( BYTE * )Str1 )[ i ] == ( ( BYTE * )Str2 )[ i ] ; i ++ ){}
		if( ( ( BYTE * )Str1 )[ i ] == 0 &&
			( ( BYTE * )Str2 )[ i ] == 0 )
		{
			return 0 ;
		}
		return ( ( BYTE * )Str1 )[ i ] < ( ( BYTE * )Str2 )[ i ] ? -1 : 1 ;

	case 2 :
		for( i = 0 ; ( ( WORD * )Str1 )[ i ] != 0 &&
			         ( ( BYTE * )Str2 )[ i ] != 0 &&
					 ( ( WORD * )Str1 )[ i ] == ( ( BYTE * )Str2 )[ i ] ; i ++ ){}
		if( ( ( WORD * )Str1 )[ i ] == 0 &&
			( ( BYTE * )Str2 )[ i ] == 0 )
		{
			return 0 ;
		}
		return ( ( WORD * )Str1 )[ i ] < ( ( BYTE * )Str2 )[ i ] ? -1 : 1 ;

	case 4 :
		for( i = 0 ; ( ( DWORD * )Str1 )[ i ] != 0 &&
			         ( ( BYTE  * )Str2 )[ i ] != 0 &&
					 ( ( DWORD * )Str1 )[ i ] == ( ( BYTE * )Str2 )[ i ] ; i ++ ){}
		if( ( ( DWORD * )Str1 )[ i ] == 0 &&
			( ( BYTE  * )Str2 )[ i ] == 0 )
		{
			return 0 ;
		}
		return ( ( DWORD * )Str1 )[ i ] < ( ( BYTE * )Str2 )[ i ] ? -1 : 1 ;
	}

	return 0 ;
}

extern int CL_stricmp( int CharCodeFormat, const char *Str1, const char *Str2 )
{
	int i ;
	DWORD Str1Code ;
	DWORD Str2Code ;
	int Str1CodeBytes ;
	int Str2CodeBytes ;
	size_t i1 = 0, i2 = 0 ;

	for( i = 0 ;; i += Str1CodeBytes )
	{
		Str1Code = GetCharCode_inline( ( const char * )&( ( BYTE * )Str1 )[ i ], CharCodeFormat, &Str1CodeBytes, i1 ) ;
		Str2Code = GetCharCode_inline( ( const char * )&( ( BYTE * )Str2 )[ i ], CharCodeFormat, &Str2CodeBytes, i2 ) ;

		if( Str1Code >= 0x61 && Str1Code <= 0x7a )
		{
			Str1Code = Str1Code - 0x61 + 0x41 ;
		}

		if( Str2Code >= 0x61 && Str2Code <= 0x7a )
		{
			Str2Code = Str2Code - 0x61 + 0x41 ;
		}

		if( Str1Code      != Str2Code      ||
			Str1CodeBytes != Str2CodeBytes ||
			Str1Code == 0                  ||
			Str2Code == 0                  )
		{
			break ;
		}
	}

	if( Str1CodeBytes != Str2CodeBytes )
	{
		return Str1CodeBytes < Str2CodeBytes ? -1 : 1 ;
	}

	return Str1Code == Str2Code ? 0 : ( Str1Code < Str2Code ? -1 : 1 ) ;
}

extern int CL_strncmp( int CharCodeFormat, const char *Str1, const char *Str2, int Num )
{
	int i ;

	switch( GetCharCodeFormatUnitSize_inline( CharCodeFormat ) )
	{
	case 1 :
		for( i = 0 ; i < Num ; i ++ )
		{
			if( ( ( BYTE * )Str1 )[ i ] == 0 || ( ( BYTE * )Str2 )[ i ] == 0 )
			{
				if( ( ( BYTE * )Str1 )[ i ] == 0 && ( ( BYTE * )Str2 )[ i ] == 0 )
				{
					return 0 ;
				}
				break ;
			}

			if( ( ( BYTE * )Str1 )[ i ] != ( ( BYTE * )Str2 )[ i ] ) 
			{
				break ;
			}
		}
		return i == Num ? 0 : ( ( ( BYTE * )Str1 )[ i ] < ( ( BYTE * )Str2 )[ i ] ? -1 : 1 ) ;

	case 2 :
		for( i = 0 ; i < Num ; i ++ )
		{
			if( ( ( WORD * )Str1 )[ i ] == 0 || ( ( WORD * )Str2 )[ i ] == 0 )
			{
				if( ( ( WORD * )Str1 )[ i ] == 0 && ( ( WORD * )Str2 )[ i ] == 0 )
				{
					return 0 ;
				}
				break ;
			}

			if( ( ( WORD * )Str1 )[ i ] != ( ( WORD * )Str2 )[ i ] ) 
			{
				break ;
			}
		}
		return i == Num ? 0 : ( ( ( WORD * )Str1 )[ i ] < ( ( WORD * )Str2 )[ i ] ? -1 : 1 ) ;

	case 3 :
		for( i = 0 ; i < Num ; i ++ )
		{
			if( ( ( DWORD * )Str1 )[ i ] == 0 || ( ( DWORD * )Str2 )[ i ] == 0 )
			{
				if( ( ( DWORD * )Str1 )[ i ] == 0 && ( ( DWORD * )Str2 )[ i ] == 0 )
				{
					return 0 ;
				}
				break ;
			}

			if( ( ( DWORD * )Str1 )[ i ] != ( ( DWORD * )Str2 )[ i ] ) 
			{
				break ;
			}
		}
		return i == Num ? 0 : ( ( ( DWORD * )Str1 )[ i ] < ( ( DWORD * )Str2 )[ i ] ? -1 : 1 ) ;
	}

	return 0 ;
}

extern int CL_strncmp2( int CharCodeFormat, const char *Str1, const char *Str2, int Num )
{
	int i ;
	int Addr ;
	int Size1 ;
	int Size2 ;
	int Code1 = 0 ;
	int Code2 = 0 ;
	size_t i1 = 0, i2 = 0 ;

	Addr = 0 ;
	for( i = 0 ; i < Num ; i ++ )
	{
		Code1 = GetCharCode_inline( ( const char * )( ( BYTE * )Str1 + Addr ), CharCodeFormat, &Size1, i1 ) ;
		Code2 = GetCharCode_inline( ( const char * )( ( BYTE * )Str2 + Addr ), CharCodeFormat, &Size2, i2 ) ;

		if( Code1 == 0 || Code2 == 0 || Size1 != Size2 || Code1 != Code2 )
		{
			if( Code1 == 0 && Code2 == 0 )
			{
				return 0 ;
			}
			break ;
		}

		Addr += Size1 ;
	}
	return i == Num ? 0 : ( Code1 < Code2 ? -1 : 1 ) ;
}

extern int CL_strpncmp( int CharCodeFormat, const char *Str1, const char *Str2, int Pos, int Num )
{
	const char *StrT ;

	StrT = CL_strpos( CharCodeFormat, Str1, Pos ) ;
	if( StrT == NULL )
	{
		return -1 ;
	}

	return CL_strncmp( CharCodeFormat, StrT, Str2, Num ) ;
}

extern int CL_strpncmp2( int CharCodeFormat, const char *Str1, const char *Str2, int Pos, int Num )
{
	const char *StrT ;

	StrT = CL_strpos2( CharCodeFormat, Str1, Pos ) ;
	if( StrT == NULL )
	{
		return -1 ;
	}

	return CL_strncmp2( CharCodeFormat, StrT, Str2, Num ) ;
}

extern DWORD CL_strgetchr( int CharCodeFormat, const char *Str, int Pos, int *CharNums )
{
	const char *StrT ;
	int CharBytes ;
	DWORD Ret ;
	size_t i = 0 ;

	StrT = CL_strpos( CharCodeFormat, Str, Pos ) ;
	if( StrT == NULL )
	{
		if( CharNums != NULL )
		{
			*CharNums = 0 ;
		}
		return 0 ;
	}

	Ret = GetCharCode_inline( StrT, CharCodeFormat, &CharBytes, i ) ;

	if( CharNums != NULL )
	{
		*CharNums = CharBytes / GetCharCodeFormatUnitSize_inline( CharCodeFormat ) ;
	}

	return Ret ;
}

extern DWORD CL_strgetchr2( int CharCodeFormat, const char *Str, int Pos, int *CharNums )
{
	const char *StrT ;
	int CharBytes ;
	DWORD Ret ;
	size_t i = 0 ;

	StrT = CL_strpos2( CharCodeFormat, Str, Pos ) ;
	if( StrT == NULL )
	{
		if( CharNums != NULL )
		{
			*CharNums = 0 ;
		}
		return 0 ;
	}

	Ret = GetCharCode_inline( StrT, CharCodeFormat, &CharBytes, i ) ;

	if( CharNums != NULL )
	{
		*CharNums = CharBytes / GetCharCodeFormatUnitSize_inline( CharCodeFormat ) ;
	}

	return Ret ;
}

extern int CL_strputchr( int CharCodeFormat, char *Str, int Pos, DWORD CharCode )
{
	char *StrT ;
	int UnitSize ;
	int Bytes ;

	StrT = ( char * )CL_strpos( CharCodeFormat, Str, Pos ) ;
	if( StrT == NULL )
	{
		return -1 ;
	}

	UnitSize = GetCharCodeFormatUnitSize_inline( CharCodeFormat ) ;
	Bytes = PutCharCode( CharCode, CharCodeFormat, StrT, BUFFERBYTES_CANCEL ) ;

	return Bytes / UnitSize ;
}

extern int CL_strputchr2( int CharCodeFormat, char *Str, int Pos, DWORD CharCode )
{
	char *StrT ;
	int UnitSize ;
	int Bytes ;

	StrT = ( char * )CL_strpos2( CharCodeFormat, Str, Pos ) ;
	if( StrT == NULL )
	{
		return -1 ;
	}

	UnitSize = GetCharCodeFormatUnitSize_inline( CharCodeFormat ) ;
	Bytes = PutCharCode( CharCode, CharCodeFormat, StrT, BUFFERBYTES_CANCEL ) ;

	return Bytes / UnitSize ;
}

extern const char *CL_strpos( int CharCodeFormat, const char *Str, int Pos )
{
	int Size ;
	int UnitSize ;

	UnitSize = GetCharCodeFormatUnitSize_inline( CharCodeFormat ) ;

	Size = ( int )CL_strlen( CharCodeFormat, Str ) ;
	if( Pos > Size )
	{
		return NULL ;
	}

	return ( const char * )( ( BYTE * )Str + UnitSize * Pos ) ;
}

extern const char *CL_strpos2( int CharCodeFormat, const char *Str, int Pos )
{
	int i ;
	int Addr ;
	DWORD StrCharCode ;
	int CodeBytes ;
	size_t j = 0 ;

	Addr = 0 ;
	for( i = 0 ; i < Pos ; i ++ )
	{
		StrCharCode = GetCharCode_inline( ( const char * )( ( BYTE * )Str + Addr ), CharCodeFormat, &CodeBytes, j ) ;
		if( StrCharCode == 0 )
		{
			return NULL ;
		}

		Addr += CodeBytes ;
	}

	return ( const char * )( ( BYTE * )Str + Addr ) ;
}

extern const char *CL_strstr( int CharCodeFormat, const char *Str1, const char *Str2 )
{
	int i, j ;

	switch( GetCharCodeFormatUnitSize_inline( CharCodeFormat ) )
	{
	case 1 :
		for( i = 0 ; ( ( BYTE * )Str1 )[ i ] != 0 ; i ++ )
		{
			for( j = 0 ; ( ( BYTE * )Str2 )[ j     ] != 0 &&
				         ( ( BYTE * )Str1 )[ i + j ] != 0 &&
						 ( ( BYTE * )Str1 )[ i + j ] == ( ( BYTE * )Str2 )[ j ] ; j ++ ){}

			if( ( ( BYTE * )Str2 )[ j ] == 0 )
			{
				return ( const char * )&( ( BYTE * )Str1 )[ i ] ;
			}

			if( ( ( BYTE * )Str1 )[ i + j ] == 0 )
			{
				return NULL ;
			}
		}
		break ;

	case 2 :
		for( i = 0 ; ( ( WORD * )Str1 )[ i ] != 0 ; i ++ )
		{
			for( j = 0 ; ( ( WORD * )Str2 )[ j     ] != 0 &&
				         ( ( WORD * )Str1 )[ i + j ] != 0 &&
						 ( ( WORD * )Str1 )[ i + j ] == ( ( WORD * )Str2 )[ j ] ; j ++ ){}

			if( ( ( WORD * )Str2 )[ j ] == 0 )
			{
				return ( const char * )&( ( WORD * )Str1 )[ i ] ;
			}

			if( ( ( WORD * )Str1 )[ i + j ] == 0 )
			{
				return NULL ;
			}
		}
		break ;

	case 4 :
		for( i = 0 ; ( ( DWORD * )Str1 )[ i ] != 0 ; i ++ )
		{
			for( j = 0 ; ( ( DWORD * )Str2 )[ j     ] != 0 &&
				         ( ( DWORD * )Str1 )[ i + j ] != 0 &&
						 ( ( DWORD * )Str1 )[ i + j ] == ( ( DWORD * )Str2 )[ j ] ; j ++ ){}

			if( ( ( DWORD * )Str2 )[ j ] == 0 )
			{
				return ( const char * )&( ( DWORD * )Str1 )[ i ] ;
			}

			if( ( ( DWORD * )Str1 )[ i + j ] == 0 )
			{
				return NULL ;
			}
		}
		break ;
	}

	return NULL ;
}

extern int CL_strstr2( int CharCodeFormat, const char *Str1, const char *Str2 )
{
	int i, j ;
	int Pos ;
	int Size ;

	Pos = 0 ;
	switch( GetCharCodeFormatUnitSize_inline( CharCodeFormat ) )
	{
	case 1 :
		for( i = 0 ; ( ( BYTE * )Str1 )[ i ] != 0 ; Pos ++ )
		{
			for( j = 0 ; ( ( BYTE * )Str2 )[ j     ] != 0 &&
				         ( ( BYTE * )Str1 )[ i + j ] != 0 &&
						 ( ( BYTE * )Str1 )[ i + j ] == ( ( BYTE * )Str2 )[ j ] ; j ++ ){}

			if( ( ( BYTE * )Str2 )[ j ] == 0 )
			{
				return Pos ;
			}

			if( ( ( BYTE * )Str1 )[ i + j ] == 0 )
			{
				return -1 ;
			}

			Size = GetCharBytes_inline( ( const char * )( &( ( BYTE * )Str1 )[ i ] ), CharCodeFormat ) ;
			i += Size ;
		}
		break ;

	case 2 :
		for( i = 0 ; ( ( WORD * )Str1 )[ i ] != 0 ; Pos ++ )
		{
			for( j = 0 ; ( ( WORD * )Str2 )[ j     ] != 0 &&
				         ( ( WORD * )Str1 )[ i + j ] != 0 &&
						 ( ( WORD * )Str1 )[ i + j ] == ( ( WORD * )Str2 )[ j ] ; j ++ ){}

			if( ( ( WORD * )Str2 )[ j ] == 0 )
			{
				return Pos ;
			}

			if( ( ( WORD * )Str1 )[ i + j ] == 0 )
			{
				return -1 ;
			}

			Size = GetCharBytes_inline( ( const char * )( &( ( WORD * )Str1 )[ i ] ), CharCodeFormat ) ;
			i += Size >> 1 ;
		}
		break ;

	case 4 :
		for( i = 0 ; ( ( DWORD * )Str1 )[ i ] != 0 ; Pos ++ )
		{
			for( j = 0 ; ( ( DWORD * )Str2 )[ j     ] != 0 &&
				         ( ( DWORD * )Str1 )[ i + j ] != 0 &&
						 ( ( DWORD * )Str1 )[ i + j ] == ( ( DWORD * )Str2 )[ j ] ; j ++ ){}

			if( ( ( DWORD * )Str2 )[ j ] == 0 )
			{
				return Pos ;
			}

			if( ( ( DWORD * )Str1 )[ i + j ] == 0 )
			{
				return -1 ;
			}

			Size = GetCharBytes_inline( ( const char * )( &( ( DWORD * )Str1 )[ i ] ), CharCodeFormat ) ;
			i += Size >> 2 ;
		}
		break ;
	}

	return -1 ;
}

extern const char *CL_strrstr( int CharCodeFormat, const char *Str1, const char *Str2 )
{
	int i, j ;
	const char *lastp = NULL ;

	switch( GetCharCodeFormatUnitSize_inline( CharCodeFormat ) )
	{
	case 1 :
		for( i = 0 ; ( ( BYTE * )Str1 )[ i ] != 0 ; i ++ )
		{
			for( j = 0 ; ( ( BYTE * )Str2 )[ j     ] != 0 &&
				         ( ( BYTE * )Str1 )[ i + j ] != 0 &&
						 ( ( BYTE * )Str1 )[ i + j ] == ( ( BYTE * )Str2 )[ j ] ; j ++ ){}

			if( ( ( BYTE * )Str2 )[ j ] == 0 )
			{
				lastp = ( const char * )&( ( BYTE * )Str1 )[ i ] ;
			}

			if( ( ( BYTE * )Str1 )[ i + j ] == 0 )
			{
				break ;
			}
		}
		break ;

	case 2 :
		for( i = 0 ; ( ( WORD * )Str1 )[ i ] != 0 ; i ++ )
		{
			for( j = 0 ; ( ( WORD * )Str2 )[ j     ] != 0 &&
				         ( ( WORD * )Str1 )[ i + j ] != 0 &&
						 ( ( WORD * )Str1 )[ i + j ] == ( ( WORD * )Str2 )[ j ] ; j ++ ){}

			if( ( ( WORD * )Str2 )[ j ] == 0 )
			{
				lastp = ( const char * )&( ( WORD * )Str1 )[ i ] ;
			}

			if( ( ( WORD * )Str1 )[ i + j ] == 0 )
			{
				break ;
			}
		}
		break ;

	case 4 :
		for( i = 0 ; ( ( DWORD * )Str1 )[ i ] != 0 ; i ++ )
		{
			for( j = 0 ; ( ( DWORD * )Str2 )[ j     ] != 0 &&
				         ( ( DWORD * )Str1 )[ i + j ] != 0 &&
						 ( ( DWORD * )Str1 )[ i + j ] == ( ( DWORD * )Str2 )[ j ] ; j ++ ){}

			if( ( ( DWORD * )Str2 )[ j ] == 0 )
			{
				lastp = ( const char * )&( ( DWORD * )Str1 )[ i ] ;
			}

			if( ( ( DWORD * )Str1 )[ i + j ] == 0 )
			{
				break ;
			}
		}
		break ;
	}

	return lastp ;
}

extern int CL_strrstr2( int CharCodeFormat, const char *Str1, const char *Str2 )
{
	int i, j ;
	int Pos ;
	int lastp = -1 ;
	int Size ;

	Pos = 0 ;
	switch( GetCharCodeFormatUnitSize_inline( CharCodeFormat ) )
	{
	case 1 :
		for( i = 0 ; ( ( BYTE * )Str1 )[ i ] != 0 ; Pos ++ )
		{
			for( j = 0 ; ( ( BYTE * )Str2 )[ j     ] != 0 &&
				         ( ( BYTE * )Str1 )[ i + j ] != 0 &&
						 ( ( BYTE * )Str1 )[ i + j ] == ( ( BYTE * )Str2 )[ j ] ; j ++ ){}

			if( ( ( BYTE * )Str2 )[ j ] == 0 )
			{
				lastp = Pos ;
			}

			if( ( ( BYTE * )Str1 )[ i + j ] == 0 )
			{
				break ;
			}

			Size = GetCharBytes_inline( ( const char * )( &( ( BYTE * )Str1 )[ i ] ), CharCodeFormat ) ;
			i += Size ;
		}
		break ;

	case 2 :
		for( i = 0 ; ( ( WORD * )Str1 )[ i ] != 0 ; Pos ++ )
		{
			for( j = 0 ; ( ( WORD * )Str2 )[ j     ] != 0 &&
				         ( ( WORD * )Str1 )[ i + j ] != 0 &&
						 ( ( WORD * )Str1 )[ i + j ] == ( ( WORD * )Str2 )[ j ] ; j ++ ){}

			if( ( ( WORD * )Str2 )[ j ] == 0 )
			{
				lastp = Pos ;
			}

			if( ( ( WORD * )Str1 )[ i + j ] == 0 )
			{
				break ;
			}

			Size = GetCharBytes_inline( ( const char * )( &( ( WORD * )Str1 )[ i ] ), CharCodeFormat ) ;
			i += Size >> 1 ;
		}
		break ;

	case 4 :
		for( i = 0 ; ( ( DWORD * )Str1 )[ i ] != 0 ; Pos ++ )
		{
			for( j = 0 ; ( ( DWORD * )Str2 )[ j     ] != 0 &&
				         ( ( DWORD * )Str1 )[ i + j ] != 0 &&
						 ( ( DWORD * )Str1 )[ i + j ] == ( ( DWORD * )Str2 )[ j ] ; j ++ ){}

			if( ( ( DWORD * )Str2 )[ j ] == 0 )
			{
				lastp = Pos ;
			}

			if( ( ( DWORD * )Str1 )[ i + j ] == 0 )
			{
				break ;
			}

			Size = GetCharBytes_inline( ( const char * )( &( ( DWORD * )Str1 )[ i ] ), CharCodeFormat ) ;
			i += Size >> 2 ;
		}
		break ;
	}

	return lastp ;
}

extern const char *CL_strchr( int CharCodeFormat, const char *Str, DWORD CharCode )
{
	int i ;
	DWORD StrCharCode ;
	int CodeBytes ;
	size_t j = 0 ;

	switch( GetCharCodeFormatUnitSize_inline( CharCodeFormat ) )
	{
	case 1 :
		for( i = 0 ; ( ( BYTE * )Str )[ i ] != 0 ; i ++ )
		{
			StrCharCode = GetCharCode_inline( ( const char * )&( ( BYTE * )Str )[ i ], CharCodeFormat, &CodeBytes, j ) ;
			if( StrCharCode == CharCode )
			{
				return ( const char * )&( ( BYTE * )Str )[ i ] ;
			}

			if( CodeBytes > 1 )
			{
				i ++ ;
			}
		}
		return NULL ;

	case 2 :
		for( i = 0 ; ( ( WORD * )Str )[ i ] != 0 ; i ++ )
		{
			StrCharCode = GetCharCode_inline( ( const char * )&( ( WORD * )Str )[ i ], CharCodeFormat, &CodeBytes, j ) ;
			if( StrCharCode == CharCode )
			{
				return ( const char * )&( ( WORD * )Str )[ i ] ;
			}

			if( CodeBytes > 2 )
			{
				i ++ ;
			}
		}
		return NULL ;

	case 4 :
		for( i = 0 ; ( ( DWORD * )Str )[ i ] != 0 ; i ++ )
		{
			StrCharCode = GetCharCode_inline( ( const char * )&( ( DWORD * )Str )[ i ], CharCodeFormat, &CodeBytes, j ) ;
			if( StrCharCode == CharCode )
			{
				return ( const char * )&( ( DWORD * )Str )[ i ] ;
			}
		}
		return NULL ;
	}

	return NULL ;
}

extern int CL_strchr2( int CharCodeFormat, const char *Str, DWORD CharCode )
{
	int i ;
	DWORD StrCharCode ;
	int CodeBytes ;
	int Pos ;
	size_t j = 0 ;

	Pos = 0 ;
	switch( GetCharCodeFormatUnitSize_inline( CharCodeFormat ) )
	{
	case 1 :
		for( i = 0 ; ( ( BYTE * )Str )[ i ] != 0 ; Pos ++ )
		{
			StrCharCode = GetCharCode_inline( ( const char * )&( ( BYTE * )Str )[ i ], CharCodeFormat, &CodeBytes, j ) ;
			if( StrCharCode == CharCode )
			{
				return Pos ;
			}

			i += CodeBytes ;
		}
		return -1 ;

	case 2 :
		for( i = 0 ; ( ( WORD * )Str )[ i ] != 0 ; Pos ++ )
		{
			StrCharCode = GetCharCode_inline( ( const char * )&( ( WORD * )Str )[ i ], CharCodeFormat, &CodeBytes, j ) ;
			if( StrCharCode == CharCode )
			{
				return Pos ;
			}

			i += CodeBytes >> 1 ;
		}
		return -1 ;

	case 4 :
		for( i = 0 ; ( ( DWORD * )Str )[ i ] != 0 ; Pos ++ )
		{
			StrCharCode = GetCharCode_inline( ( const char * )&( ( DWORD * )Str )[ i ], CharCodeFormat, &CodeBytes, j ) ;
			if( StrCharCode == CharCode )
			{
				return Pos ;
			}

			i += CodeBytes >> 2 ;
		}
		return -1 ;
	}

	return -1 ;
}

extern const char *CL_strrchr( int CharCodeFormat, const char *Str, DWORD CharCode )
{
	const char *lastp ;
	DWORD StrCharCode ;
	int CodeBytes ;
	int i ;
	size_t j = 0 ;

	lastp = NULL;

	switch( GetCharCodeFormatUnitSize_inline( CharCodeFormat ) )
	{
	case 1 :
		for( i = 0 ; ( ( BYTE * )Str )[ i ] != 0 ; i ++ )
		{
			StrCharCode = GetCharCode_inline( ( const char * )&( ( BYTE * )Str )[ i ], CharCodeFormat, &CodeBytes, j ) ;
			if( StrCharCode == CharCode )
			{
				lastp = ( const char * )&( ( BYTE * )Str )[ i ] ;
			}

			if( CodeBytes > 1 )
			{
				i ++ ;
			}
		}
		break ;

	case 2 :
		for( i = 0 ; ( ( WORD * )Str )[ i ] != 0 ; i ++ )
		{
			StrCharCode = GetCharCode_inline( ( const char * )&( ( WORD * )Str )[ i ], CharCodeFormat, &CodeBytes, j ) ;
			if( StrCharCode == CharCode )
			{
				lastp = ( const char * )&( ( WORD * )Str )[ i ] ;
			}

			if( CodeBytes > 2 )
			{
				i ++ ;
			}
		}
		break ;

	case 4 :
		for( i = 0 ; ( ( DWORD * )Str )[ i ] != 0 ; i ++ )
		{
			StrCharCode = GetCharCode_inline( ( const char * )&( ( DWORD * )Str )[ i ], CharCodeFormat, &CodeBytes, j ) ;
			if( StrCharCode == CharCode )
			{
				lastp = ( const char * )&( ( DWORD * )Str )[ i ] ;
			}
		}
		break ;
	}

	return lastp ;
}

extern int CL_strrchr2( int CharCodeFormat, const char *Str, DWORD CharCode )
{
	int i ;
	int lastp ;
	DWORD StrCharCode ;
	int CodeBytes ;
	int Pos ;
	size_t j = 0 ;

	Pos = 0 ;
	lastp = -1 ;
	switch( GetCharCodeFormatUnitSize_inline( CharCodeFormat ) )
	{
	case 1 :
		for( i = 0 ; ( ( BYTE * )Str )[ i ] != 0 ; Pos ++ )
		{
			StrCharCode = GetCharCode_inline( ( const char * )&( ( BYTE * )Str )[ i ], CharCodeFormat, &CodeBytes, j ) ;
			if( StrCharCode == CharCode )
			{
				lastp = Pos ;
			}

			i += CodeBytes ;
		}
		break ;

	case 2 :
		for( i = 0 ; ( ( WORD * )Str )[ i ] != 0 ; Pos ++ )
		{
			StrCharCode = GetCharCode_inline( ( const char * )&( ( WORD * )Str )[ i ], CharCodeFormat, &CodeBytes, j ) ;
			if( StrCharCode == CharCode )
			{
				lastp = Pos ;
			}

			i += CodeBytes >> 1 ;
		}
		break ;

	case 4 :
		for( i = 0 ; ( ( DWORD * )Str )[ i ] != 0 ; Pos ++ )
		{
			StrCharCode = GetCharCode_inline( ( const char * )&( ( DWORD * )Str )[ i ], CharCodeFormat, &CodeBytes, j ) ;
			if( StrCharCode == CharCode )
			{
				lastp = Pos ;
			}

			i += CodeBytes >> 2 ;
		}
		break ;
	}

	return lastp ;
}

extern char * CL_strupr( int CharCodeFormat, char *Str )
{
	DWORD StrCharCode ;
	int CodeBytes ;
	int i ;
	size_t j = 0 ;

	switch( GetCharCodeFormatUnitSize_inline( CharCodeFormat ) )
	{
	case 1 :
		for( i = 0 ; ( ( BYTE * )Str )[ i ] != 0 ; i ++ )
		{
			StrCharCode = GetCharCode_inline( ( const char * )&( ( BYTE * )Str )[ i ], CharCodeFormat, &CodeBytes, j ) ;
			if( StrCharCode >= 0x61 && StrCharCode <= 0x7a )
			{
				StrCharCode = StrCharCode - 0x61 + 0x41 ;
				PutCharCode_inline( StrCharCode, CharCodeFormat, ( char * )&( ( BYTE * )Str )[ i ] ) ;
			}

			if( CodeBytes > 1 )
			{
				i ++ ;
			}
		}
		break ;

	case 2 :
		for( i = 0 ; ( ( WORD * )Str )[ i ] != 0 ; i ++ )
		{
			StrCharCode = GetCharCode_inline( ( const char * )&( ( WORD * )Str )[ i ], CharCodeFormat, &CodeBytes, j ) ;
			if( StrCharCode >= 0x61 && StrCharCode <= 0x7a )
			{
				StrCharCode = StrCharCode - 0x61 + 0x41 ;
				PutCharCode_inline( StrCharCode, CharCodeFormat, ( char * )&( ( WORD * )Str )[ i ] ) ;
			}

			if( CodeBytes > 2 )
			{
				i ++ ;
			}
		}
		break ;

	case 4 :
		for( i = 0 ; ( ( DWORD * )Str )[ i ] != 0 ; i ++ )
		{
			StrCharCode = GetCharCode_inline( ( const char * )&( ( DWORD * )Str )[ i ], CharCodeFormat, &CodeBytes, j ) ;
			if( StrCharCode >= 0x61 && StrCharCode <= 0x7a )
			{
				StrCharCode = StrCharCode - 0x61 + 0x41 ;
				PutCharCode_inline( StrCharCode, CharCodeFormat, ( char * )&( ( DWORD * )Str )[ i ] ) ;
			}
		}
		break ;
	}

	return Str ;
}

static DWORD CL_vsnprintf_help_getnumber( const DWORD *CharCode, int *UseCharNum )
{
	DWORD Result ;
	DWORD AddNum ;
	int i ;
	DWORD NumberNum ;
	BYTE NumberStr[ 1024 ] ;

	for( NumberNum = 0 ; NumberNum < sizeof( NumberStr ) && CharCode[ NumberNum ] >= '0' && CharCode[ NumberNum ] <= '9' ; NumberNum ++ )
	{
		NumberStr[ NumberNum ] = ( BYTE )CharCode[ NumberNum ] ;
	}

	Result = 0 ;
	AddNum = 1 ;
	for( i = ( int )( NumberNum - 1 ) ; i >= 0 ; i -- )
	{
		Result += AddNum * ( NumberStr[ i ] - '0' ) ;
		AddNum *= 10 ;
	}

	if( UseCharNum != NULL )
	{
		*UseCharNum = ( int )NumberNum ;
	}

	return Result ;
}

// 指定の文字を指定数書き込む、戻り値は書き込んだバイト数
static size_t CL_vsnprintf_help_set_char( ULONGLONG Num, DWORD CharCode, char *Dest, size_t BufferBytes, int DestCharCodeFormat )
{
	DWORD i ;
	size_t DestSize ;
	size_t NulWriteBytes ;
	size_t WriteBytes ;

	DestSize = 0 ;

	NulWriteBytes = PutCharCode_BufferBytes_inline( 0x00, DestCharCodeFormat, ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize ) ;
	if( NulWriteBytes == 0 )
	{
		return 0 ;
	}

	for( i = 0 ; i < Num ; i ++ )
	{
		WriteBytes = PutCharCode_BufferBytes_inline( CharCode, DestCharCodeFormat, ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize ) ;
		if( WriteBytes == 0 || BufferBytes - DestSize < WriteBytes + NulWriteBytes )
		{
			goto END ;
		}
		DestSize += WriteBytes ;
		if( BufferBytes - DestSize <= NulWriteBytes )
		{
			goto END ;
		}
	}

END :

	return DestSize ;
}

// f タイプ用の NaN 文字列化を行う、戻り値は文字数
static int CL_vsnprintf_help_ftoa_NaN_f(
	int FPClass,
	int Flag_Sharp,
	int ZeroAdd,
	int Precision,
	BYTE *NumberStrBuffer
)
{
	int DestSize ;

	// 精度がマイナスの場合は既定値をセット
	if( Precision < 0 )
	{
		Precision = 6 ;
	}

	DestSize = 0 ;
	// 精度が 0 の場合は Flag_Sharp があれば . を付ける
	if( Precision == 0 )
	{
		NumberStrBuffer[ DestSize ] = '1' ;
		DestSize ++ ;

		if( Flag_Sharp )
		{
			NumberStrBuffer[ DestSize ] = '.' ;
			DestSize ++ ;
		}

		Precision = 0 ;
	}
	else
	{
		int SetNum ;
		int i ;
		BYTE ( *Table )[ 7 ] = NULL ;
		int MaxLength = 0 ;

		// 使用するテーブルと最大使用精度長をセット
		switch( FPClass )
		{
		case _FPCLASS_SNAN :
		case _FPCLASS_QNAN :
			Table     = FloatErrorStr_QNAN ;
			MaxLength = 5 ;
			break ;

		case _FPCLASS_NINF :
		case _FPCLASS_PINF :
			Table     = FloatErrorStr_INF ;
			MaxLength = 4 ;
			break ;
		}

		// 精度が文字列の最大長以上の場合は文字列を全て転送
		if( Precision >= MaxLength )
		{
			SetNum = MaxLength + 2 ;
		}
		else
		// それ以外の場合は精度 + 2 の分だけ転送
		{
			SetNum = Precision + 2 ;
		}

		// 書き込み
		for( i = 0 ; i < SetNum ; i ++ )
		{
			NumberStrBuffer[ DestSize ] = Table[ SetNum - 1 ][ i ] ;
			DestSize ++ ;
		}

		// 使用した分の精度を引く
		Precision -= SetNum - 2 ;

		// 精度に残りがあったら 0 で埋める
		if( ZeroAdd )
		{
			for( i = 0 ; i < Precision ; i ++ )
			{
				NumberStrBuffer[ DestSize ] = '0' ;
				DestSize ++ ;
			}
		}
	}

	return DestSize ;
}

// e タイプ用の NaN 文字列化を行う、戻り値は文字数
static int CL_vsnprintf_help_ftoa_NaN_e(
	int FPClass,
	int Flag_Sharp,
	int Precision,
	int Big,
	BYTE *NumberStrBuffer
)
{
	int DestSize ;

	// 後に e+000 が付く以外は CL_vsnprintf_help_ftoa_NaN_f と処理は同じ
	DestSize = CL_vsnprintf_help_ftoa_NaN_f( FPClass, Flag_Sharp, 1, Precision, NumberStrBuffer ) ;

	// 後にe+000 を付ける
	NumberStrBuffer[ DestSize + 0 ] = Big ? 'E' : 'e' ;
	NumberStrBuffer[ DestSize + 1 ] = '+' ;
	NumberStrBuffer[ DestSize + 2 ] = '0' ;
	NumberStrBuffer[ DestSize + 3 ] = '0' ;
	NumberStrBuffer[ DestSize + 4 ] = '0' ;
	DestSize += 5 ;

	return DestSize ;
}

// g タイプ用の NaN 文字列化を行う、戻り値は文字数
static int CL_vsnprintf_help_ftoa_NaN_g(
	int FPClass,
	int Flag_Sharp,
	int Precision,
	BYTE *NumberStrBuffer
)
{
	// 精度がマイナスの場合は既定値の 6 をセット
	if( Precision < 0 )
	{
		Precision = 6 ;
	}

	// f 形式の扱いでは Precision が 1 少ない状態になる
	if( Precision > 0 )
	{
		Precision -- ;
	}

	// 精度の数値の扱いが 1 少ないのと、精度が余分にあっても 0 が付かない以外は CL_vsnprintf_help_ftoa_NaN_f と処理は同じ
	return CL_vsnprintf_help_ftoa_NaN_f( FPClass, Flag_Sharp, 0, Precision, NumberStrBuffer ) ;
}

// a タイプ用の NaN 文字列化を行う、戻り値は文字数
static int CL_vsnprintf_help_ftoa_NaN_a(
	int FPClass,
	int Flag_Sharp,
	int Precision,
	int Big,
	BYTE *NumberStrBuffer
)
{
	int DestSize = 0 ;

	// 先頭に 0x、後に p+0 が付く以外は CL_vsnprintf_help_ftoa_NaN_f と処理は同じ
	NumberStrBuffer[ DestSize + 0 ] = '0' ;
	NumberStrBuffer[ DestSize + 1 ] = '0' ;
	DestSize += 2 ;

	DestSize += CL_vsnprintf_help_ftoa_NaN_f( FPClass, Flag_Sharp, 1, Precision, &NumberStrBuffer[ DestSize ] ) ;

	NumberStrBuffer[ DestSize + 0 ] = Big ? 'P' : 'p' ;
	NumberStrBuffer[ DestSize + 1 ] = '+' ;
	NumberStrBuffer[ DestSize + 2 ] = '0' ;
	DestSize += 3 ;

	return DestSize ;
}

// a タイプの浮動小数点値の文字列化を行う、戻り値は書き込んだバイト数
static int CL_vsnprintf_help_ftoa_a(
	double Number,
	int Flag_Sharp,
	int Precision,
	int Big,
	BYTE *NumberStrBuffer
)
{
	double TopFigureNumber ;
	double TempNumber ;
	double TargetFigureOne ;
	int LoopFlag ;
	int TempNumberI ;
	BYTE NumberCharTemp ;
	int TopFigure ;
	int IntNumberNum ;
	int FloatNumberNum ;
	int i ;
	int DestSize = 0 ;
	BYTE NumberStrTemp[ 1024 + 32 ] ;
	BYTE *AllocNumberStrTempBuffer = NULL ;
	BYTE *UseNumberStrTempBuffer ;

	// デフォルトのサイズを超える場合は一時バッファを確保する
	if( Precision >= 0 && sizeof( NumberStrTemp ) - 32 <= ( size_t )Precision )
	{
		AllocNumberStrTempBuffer = ( BYTE * )DXALLOC( Precision + 32 ) ;

		// バッファが確保できなかった場合は文字列化する桁数を 1024 に変更する
		if( AllocNumberStrTempBuffer == NULL )
		{
			Precision = 1024 ;
			UseNumberStrTempBuffer = NumberStrTemp ;
		}
		else
		{
			UseNumberStrTempBuffer = AllocNumberStrTempBuffer ;
		}
	}
	else
	{
		UseNumberStrTempBuffer = NumberStrTemp ;
	}

	// 精度が決まっていない場合は既定値の 6 をセット
	if( Precision < 0 )
	{
		Precision = 6 ;
	}

	// マイナス値の場合はプラス値にする
	if( Number < 0.0 )
	{
		Number = -Number ;
	}

	// ループしたかどうかのフラグを倒す
	LoopFlag = 0 ;

	// 文字列化やり直しをする場合に飛ぶ位置
LOOPLABEL :
	DestSize = 0 ;

	// 16進数を表す 0x を出力する
	NumberStrBuffer[ DestSize + 0 ] = '0' ;
	NumberStrBuffer[ DestSize + 1 ] = 'x' ;
	DestSize += 2 ;

	// 最上位の桁を探す

	// 0 の場合は特別処理
	TempNumber = Number ;
	TargetFigureOne = 1.0 ;
	if( Number == 0.0 )
	{
		TopFigure = 0 ;
	}
	else
	if( Number < 1.0 )
	{
		for( TopFigure = 0 ; TempNumber <  1.0 ; TempNumber *= 2.0, TopFigure --, TargetFigureOne /= 2.0 ){}
	}
	else
	{
		for( TopFigure = 0 ; TempNumber >= 2.0 ; TempNumber /= 2.0, TopFigure ++, TargetFigureOne *= 2.0 ){}
	}
	TopFigureNumber = TempNumber ;

	// 最上位桁の出力
	NumberCharTemp = ( BYTE )TempNumber ;
	NumberStrBuffer[ DestSize ] = '0' + NumberCharTemp ;
	DestSize ++ ;

	// 小数部を列挙する
	TempNumber = fmod( TopFigureNumber, 1.0 ) ;
	FloatNumberNum = 0 ;
	for( i = 0 ; i < Precision ; i ++ )
	{
		TargetFigureOne /= 16.0 ;
		TempNumber *= 16.0 ;
		UseNumberStrTempBuffer[ FloatNumberNum ] = ( BYTE )fmod( TempNumber, 16.0 ) ;
		TempNumber -= ( double )UseNumberStrTempBuffer[ FloatNumberNum ] ;
		FloatNumberNum++ ;
	}

	// 次の桁の値が 0.8 を超える場合は最後の桁の値に１を足して文字列化処理を最初からやり直す
	if( LoopFlag == 0 )
	{
		TempNumber *= 16.0 ;
		if( fmod( TempNumber, 16.0 ) > 8.0 )
		{
			Number += TargetFigureOne ;
			LoopFlag = 1 ;
			goto LOOPLABEL ;
		}
	}

	// 精度が 0 でも Flag_Sharp が有効な場合は . をセットする
	if( Precision > 0 || Flag_Sharp )
	{
		NumberStrBuffer[ DestSize ] = '.' ;
		DestSize ++ ;
	}

	// 最上位桁以下を出力する
	for( i = 0 ; i < Precision ; i ++ )
	{
		NumberStrBuffer[ DestSize ] = NumberToCharTable[ Big ][ UseNumberStrTempBuffer[ i ] ] ;
		DestSize ++ ;
	}

	// 指数部の p 又は P をセットする
	NumberStrBuffer[ DestSize ] = Big ? 'P' : 'p' ;
	DestSize ++ ;

	// 指数部の + 又は - をセットする
	if( TopFigure < 0 )
	{
		TopFigure = -TopFigure ;
		NumberStrBuffer[ DestSize ] = '-' ;
	}
	else
	{
		NumberStrBuffer[ DestSize ] = '+' ;
	}
	DestSize ++ ;

	// 指数部の数値を列挙する
	if( TopFigure == 0 )
	{
		IntNumberNum = 1 ;
		UseNumberStrTempBuffer[ 0 ] = 0 ;
	}
	else
	{
		TempNumberI = TopFigure ;
		for( IntNumberNum = 0 ; TempNumberI != 0 ; TempNumberI /= 10, IntNumberNum ++ )
		{
			UseNumberStrTempBuffer[ IntNumberNum ] = TempNumberI % 10 ;
		}
	}

	// 指数部の出力
	for( i = IntNumberNum - 1 ; i >= 0 ; i -- )
	{
		NumberStrBuffer[ DestSize ] = '0' + UseNumberStrTempBuffer[ i ] ;
		DestSize ++ ;
	}

	// メモリを確保していたら解放する
	if( AllocNumberStrTempBuffer != NULL )
	{
		DXFREE( AllocNumberStrTempBuffer ) ;
		AllocNumberStrTempBuffer = NULL ;
	}

	// 終了
	return DestSize ;
}

// e タイプの浮動小数点値の文字列化を行う、戻り値は文字数
static int CL_vsnprintf_help_ftoa_e(
	double Number,
	int Flag_Sharp,
	int Precision,
	int Big,
	BYTE *NumberStrBuffer
)
{
	double TopFigureNumber ;
	double TargetFigureOne ;
	double TempNumber ;
	BYTE NumberCharTemp ;
	int TopFigure ;
	int FloatNumberNum ;
	int LoopFlag ;
	int i ;
	int DestSize ;
	BYTE NumberStrTemp[ 1024 + 32 ] ;
	BYTE *AllocNumberStrTempBuffer = NULL ;
	BYTE *UseNumberStrTempBuffer ;

	// デフォルトのサイズを超える場合は一時バッファを確保する
	if( Precision >= 0 && sizeof( NumberStrTemp ) - 32 <= ( size_t )Precision )
	{
		AllocNumberStrTempBuffer = ( BYTE * )DXALLOC( Precision + 32 ) ;

		// バッファが確保できなかった場合は文字列化する桁数を 1024 に変更する
		if( AllocNumberStrTempBuffer == NULL )
		{
			Precision = 1024 ;
			UseNumberStrTempBuffer = NumberStrTemp ;
		}
		else
		{
			UseNumberStrTempBuffer = AllocNumberStrTempBuffer ;
		}
	}
	else
	{
		UseNumberStrTempBuffer = NumberStrTemp ;
	}

	// 精度が決まっていない場合は既定値の 6 をセット
	if( Precision < 0 )
	{
		Precision = 6 ;
	}

	// マイナス値の場合はプラス値にする
	if( Number < 0.0 )
	{
		Number = -Number ;
	}

	// ループしたかどうかのフラグを倒す
	LoopFlag = 0 ;

	// 文字列化やり直しをする場合に飛ぶ位置
LOOPLABEL :
	DestSize = 0 ;

	// 最上位の桁を探す

	// 0 の場合は特別処理
	TempNumber = Number ;
	TargetFigureOne = 1.0 ;
	if( Number == 0.0 )
	{
		TopFigure = 0 ;
	}
	else
	if( Number < 1.0 )
	{
		for( TopFigure = 0 ; TempNumber <   1.0 ; TempNumber *= 10.0, TopFigure --, TargetFigureOne /= 10.0 ){}
	}
	else
	{
		for( TopFigure = 0 ; TempNumber >= 10.0 ; TempNumber /= 10.0, TopFigure ++, TargetFigureOne *= 10.0 ){}
	}
	TopFigureNumber = TempNumber ;

	// 小数部を列挙する
	TempNumber = fmod( TopFigureNumber, 1.0 ) ;
	FloatNumberNum = 0 ;
	for( i = 0 ; i < Precision ; i ++ )
	{
		TargetFigureOne /= 10.0 ;
		TempNumber *= 10.0 ;
		UseNumberStrTempBuffer[ FloatNumberNum ] = ( BYTE )fmod( TempNumber, 10.0 ) ;
		TempNumber -= ( double )UseNumberStrTempBuffer[ FloatNumberNum ] ;
		FloatNumberNum++ ;
	}

	// 次の桁の値が 0.5 を超える場合は最後の桁の値に１を足して文字列化処理を最初からやり直す
	if( LoopFlag == 0 )
	{
		TempNumber *= 10.0 ;
		if( fmod( TempNumber, 10.0 ) > 5.0 )
		{
			Number += TargetFigureOne ;
			LoopFlag = 1 ;
			goto LOOPLABEL ;
		}
	}

	// 最上位桁の出力
	NumberCharTemp = ( BYTE )TopFigureNumber ;
	NumberStrBuffer[ DestSize ] = '0' + NumberCharTemp ;
	DestSize ++ ;

	// 精度が 0 でも Flag_Sharp が有効な場合は . をセットする
	if( Precision > 0 || Flag_Sharp )
	{
		NumberStrBuffer[ DestSize ] = '.' ;
		DestSize ++ ;
	}

	// 最上位桁以下を出力する
	for( i = 0 ; i < Precision ; i ++ )
	{
		NumberStrBuffer[ DestSize ] = '0' + UseNumberStrTempBuffer[ i ] ;
		DestSize ++ ;
	}

	// 指数部の e 又は E をセットする
	NumberStrBuffer[ DestSize ] = Big ? 'E' : 'e' ;
	DestSize ++ ;

	// 指数部の + 又は - をセットする
	if( TopFigure < 0 )
	{
		TopFigure = -TopFigure ;
		NumberStrBuffer[ DestSize ] = '-' ;
	}
	else
	{
		NumberStrBuffer[ DestSize ] = '+' ;
	}
	DestSize ++ ;

	// 指数部の３桁を出力
	NumberStrBuffer[ DestSize + 0 ] = '0' + ( TopFigure / 100 ) % 10 ;
	NumberStrBuffer[ DestSize + 1 ] = '0' + ( TopFigure /  10 ) % 10 ;
	NumberStrBuffer[ DestSize + 2 ] = '0' +   TopFigure         % 10 ;
	DestSize += 3 ;

	// メモリを確保していたら解放する
	if( AllocNumberStrTempBuffer != NULL )
	{
		DXFREE( AllocNumberStrTempBuffer ) ;
		AllocNumberStrTempBuffer = NULL ;
	}

	// 終了
	return DestSize ;
}

// f タイプの浮動小数点値の文字列化を行う、戻り値は文字数
static int CL_vsnprintf_help_ftoa_f(
	double Number,
	int Flag_Sharp,
	int Precision,
	BYTE *NumberStrBuffer
)
{
	double TempNumber ;
	double TargetFigureOne ;
	int LoopFlag ;
	int IntNumberNum ;
	int FloatNumberNum ;
	int i ;
	int DestSize ;
	BYTE NumberStrTemp[ 1024 + 32 ] ;
	BYTE *AllocNumberStrTempBuffer = NULL ;
	BYTE *UseNumberStrTempBuffer ;

	// 精度が決まっていない場合は既定値の 6 をセット
	if( Precision < 0 )
	{
		Precision = 6 ;
	}

	// デフォルトのサイズを超える場合は一時バッファを確保する
	if( Precision >= 0 && sizeof( NumberStrTemp ) - 32 <= ( size_t )Precision )
	{
		AllocNumberStrTempBuffer = ( BYTE * )DXALLOC( Precision + 32 ) ;

		// バッファが確保できなかった場合は文字列化する桁数を 1024 に変更する
		if( AllocNumberStrTempBuffer == NULL )
		{
			Precision = 1024 ;
			UseNumberStrTempBuffer = NumberStrTemp ;
		}
		else
		{
			UseNumberStrTempBuffer = AllocNumberStrTempBuffer ;
		}
	}
	else
	{
		UseNumberStrTempBuffer = NumberStrTemp ;
	}

	// マイナス値の場合はプラス値にする
	if( Number < 0.0 )
	{
		Number = -Number ;
	}

	// ループしたかどうかのフラグを倒す
	LoopFlag = 0 ;

	// 文字列化やり直しをする場合に飛ぶ位置
LOOPLABEL :
	DestSize = 0 ;

	IntNumberNum = 0 ;
	TempNumber = Number - fmod( Number, 1.0 ) ;
	if( Number < 1.0 )
	{
		// ゼロ以下の場合は整数部の数値は 0 のみ出力
		NumberStrBuffer[ DestSize ] = '0' ;
		DestSize ++ ;
		IntNumberNum ++ ;
	}
	else
	{
		// 整数部の数値を列挙する
		do
		{
			UseNumberStrTempBuffer[ IntNumberNum ] = ( BYTE )fmod( TempNumber, 10.0 ) ; 
			TempNumber -= ( double )UseNumberStrTempBuffer[ IntNumberNum ] ;
			IntNumberNum ++ ;
			TempNumber /= 10.0 ;
		}while( TempNumber >= 1.0 ) ;

		// 整数部の数値をセットする
		for( i = IntNumberNum - 1 ; i >= 0 ; i -- )
		{
			NumberStrBuffer[ DestSize ] = '0' + UseNumberStrTempBuffer[ i ] ;
			DestSize ++ ;
		}
	}

	// 小数部を列挙する
	TempNumber = fmod( Number, 1.0 ) ;
	FloatNumberNum = 0 ;
	TargetFigureOne = 1.0 ;
	for( i = 0 ; i < Precision ; i ++ )
	{
		TargetFigureOne /= 10.0 ;
		TempNumber *= 10.0 ;
		UseNumberStrTempBuffer[ FloatNumberNum ] = ( BYTE )fmod( TempNumber, 10.0 ) ;
		TempNumber -= ( double )UseNumberStrTempBuffer[ FloatNumberNum ] ;
		FloatNumberNum++ ;
	}

	// 次の桁の値が 0.5 を超える場合は最後の桁の値に１を足して文字列化処理を最初からやり直す
	if( LoopFlag == 0 )
	{
		TempNumber *= 10.0 ;
		if( fmod( TempNumber, 10.0 ) > 5.0 )
		{
			Number += TargetFigureOne ;
			LoopFlag = 1 ;
			goto LOOPLABEL ;
		}
	}

	// 精度が 0 で # もない場合はここで終了
	if( Precision == 0 && Flag_Sharp == 0 )
	{
		goto END ;
	}

	// . をセット
	NumberStrBuffer[ DestSize ] = '.' ;
	DestSize ++ ;

	// 小数部を出力
	for( i = 0 ; i < FloatNumberNum ; i ++ )
	{
		NumberStrBuffer[ DestSize ] = '0' + UseNumberStrTempBuffer[ i ] ;
		DestSize ++ ;
	}

END :

	// メモリを確保していたら解放する
	if( AllocNumberStrTempBuffer != NULL )
	{
		DXFREE( AllocNumberStrTempBuffer ) ;
		AllocNumberStrTempBuffer = NULL ;
	}

	// 終了
	return DestSize ;
}

// g タイプの浮動小数点値の文字列化を行う、戻り値は文字数
static int CL_vsnprintf_help_ftoa_g(
	double Number,
	int Flag_Sharp,
	int Precision,
	int Big,
	BYTE *NumberStrBuffer
)
{
	double TopFigureNumber ;
	double TempNumber ;
	double TargetFigureOne ;
	int LoopFlag ;
	BYTE NumberCharTemp ;
	int TopFigure ;
	int LastNonZeroFigure ;
	int FloatNumberNum ;
	int i ;
	int PrecisionF ;
	int DestSize = 0 ;
	BYTE NumberStrTemp[ 1024 + 32 ] ;
	BYTE *AllocNumberStrTempBuffer = NULL ;
	BYTE *UseNumberStrTempBuffer ;

	// デフォルトのサイズを超える場合は一時バッファを確保する
	if( Precision >= 0 && sizeof( NumberStrTemp ) - 32 <= ( size_t )Precision )
	{
		AllocNumberStrTempBuffer = ( BYTE * )DXALLOC( Precision + 32 ) ;

		// バッファが確保できなかった場合は文字列化する桁数を 1024 に変更する
		if( AllocNumberStrTempBuffer == NULL )
		{
			Precision = 1024 ;
			UseNumberStrTempBuffer = NumberStrTemp ;
		}
		else
		{
			UseNumberStrTempBuffer = AllocNumberStrTempBuffer ;
		}
	}
	else
	{
		UseNumberStrTempBuffer = NumberStrTemp ;
	}

	// 精度が決まっていない場合は既定値の 6 をセット
	if( Precision < 0 )
	{
		Precision = 6 ;
	}
	else
	// g では Precision が 0 という扱いは無いので、0 の場合は 1 にする
	if( Precision == 0 )
	{
		Precision = 1 ;
	}

	// マイナス値の場合はプラス値にする
	if( Number < 0.0 )
	{
		Number = -Number ;
	}

	// ループしたかどうかのフラグを倒す
	LoopFlag = 0 ;

	// 文字列化やり直しをする場合に飛ぶ位置
LOOPLABEL :
	DestSize = 0 ;

	// 最上位の桁を探す

	// 0 の場合は特別処理
	TempNumber = Number ;
	TargetFigureOne = 1.0 ;
	if( Number == 0.0 )
	{
		TopFigure = 0 ;
	}
	else
	if( Number < 1.0 )
	{
		for( TopFigure = 0 ; TempNumber <   1.0 ; TempNumber *= 10.0, TopFigure --, TargetFigureOne /= 10.0 ){}
	}
	else
	{
		for( TopFigure = 0 ; TempNumber >= 10.0 ; TempNumber /= 10.0, TopFigure ++, TargetFigureOne *= 10.0 ){}
	}
	TopFigureNumber = TempNumber ;

	// 小数部を列挙する
	LastNonZeroFigure = -1 ;
	TempNumber = fmod( TopFigureNumber, 1.0 ) ;
	FloatNumberNum = 0 ;
	for( i = 0 ; i < Precision - 1 ; i ++ )
	{
		TargetFigureOne /= 10.0 ;
		TempNumber *= 10.0 ;
		UseNumberStrTempBuffer[ FloatNumberNum ] = ( BYTE )fmod( TempNumber, 10.0 ) ;
		TempNumber -= ( double )UseNumberStrTempBuffer[ FloatNumberNum ] ;

		// 0 ではなかったら、「0では無い桁の位置」を保存
		if( UseNumberStrTempBuffer[ FloatNumberNum ] != 0 )
		{
			LastNonZeroFigure = FloatNumberNum ;
		}

		FloatNumberNum++ ;
	}

	// 次の桁の値が 0.5 を超える場合は最後の桁の値に１を足して文字列化処理を最初からやり直す
	if( LoopFlag == 0 )
	{
		TempNumber *= 10.0 ;
		if( fmod( TempNumber, 10.0 ) > 5.0 )
		{
			Number += TargetFigureOne ;
			LoopFlag = 1 ;
			goto LOOPLABEL ;
		}
	}

	// 指数部の表示が不要かどうかを判定
	if( TopFigure >= 0 )
	{
		// 最上位桁が整数の場合は精度未満の場合は指数部の表示を行わない
		if( TopFigure < Precision )
		{
			// 指数部の表示を行わない場合は f 用の関数を使用する

			// 小数点以下何桁まで出力するかを決定、Flag_Sharp の指定が無い場合は精度範囲内でも不要な 0 は出力しない
			if( Flag_Sharp == 0 )
			{
				PrecisionF = -( TopFigure - ( LastNonZeroFigure + 1 ) ) ;
			}
			else
			{
				PrecisionF = -( TopFigure - ( Precision - 1 ) ) ;
			}
			if( PrecisionF < 0 )
			{
				PrecisionF = 0 ;
			}
			DestSize = CL_vsnprintf_help_ftoa_f( Number, Flag_Sharp, PrecisionF, NumberStrBuffer ) ;
			goto END ;
		}
	}
	else
	{
		// 最上位桁が小数の場合は最上位桁が 0.0001 より大きい場合は指数部の表示を行わない
		if( TopFigure >= -4 )
		{
			// 指数部の表示を行わない場合は f 用の関数を使用する

			// 小数点以下何桁まで出力するかを決定
			PrecisionF = -TopFigure + Precision - 1 ;

			// Flag_Sharp の指定が無い場合は、精度範囲内でも不要な 0 は出力しない
			if( Flag_Sharp == 0 )
			{
				if( PrecisionF > -TopFigure + LastNonZeroFigure + 1 )
				{
					PrecisionF = -TopFigure + LastNonZeroFigure + 1 ;
				}
			}

			DestSize = CL_vsnprintf_help_ftoa_f( Number, Flag_Sharp, PrecisionF, NumberStrBuffer ) ;
			goto END ;
		}
	}

	// 最上位桁の出力
	NumberCharTemp = ( BYTE )TopFigureNumber ;
	NumberStrBuffer[ DestSize ] = '0' + NumberCharTemp ;
	DestSize ++ ;

	// Flag_Sharp の指定が無い場合は Precision 以内でも小数点以下で末端から続く 0 は出力しない
	if( Flag_Sharp == 0 )
	{
		Precision = LastNonZeroFigure + 2 ;
	}

	// 精度が 1 でも Flag_Sharp が有効な場合は . をセットする
	if( Precision > 1 || Flag_Sharp )
	{
		NumberStrBuffer[ DestSize ] = '.' ;
		DestSize ++ ;
	}

	// 最上位桁以下を出力する
	for( i = 0 ; i < Precision - 1 ; i ++ )
	{
		NumberStrBuffer[ DestSize ] = '0' + UseNumberStrTempBuffer[ i ] ;
		DestSize ++ ;
	}

	// 指数部の e 又は E をセットする
	NumberStrBuffer[ DestSize ] = Big ? 'E' : 'e' ;
	DestSize ++ ;

	// 指数部の + をセットする
	if( TopFigure < 0 )
	{
		TopFigure = -TopFigure ;
		NumberStrBuffer[ DestSize ] = '-' ;
	}
	else
	{
		NumberStrBuffer[ DestSize ] = '+' ;
	}
	DestSize ++ ;

	// 指数部の３桁を出力
	NumberStrBuffer[ DestSize + 0 ] = '0' + ( TopFigure / 100 ) % 10 ;
	NumberStrBuffer[ DestSize + 1 ] = '0' + ( TopFigure /  10 ) % 10 ;
	NumberStrBuffer[ DestSize + 2 ] = '0' +   TopFigure         % 10 ;
	DestSize += 3 ;

END :

	// メモリを確保していたら解放する
	if( AllocNumberStrTempBuffer != NULL )
	{
		DXFREE( AllocNumberStrTempBuffer ) ;
		AllocNumberStrTempBuffer = NULL ;
	}

	// 終了
	return DestSize ;
}

// 浮動小数点値を文字列化する関数、戻り値は書き込んだバイト数
static size_t CL_vsnprintf_help_ftoa(
	double Number,
	int Type,
	int Flag_Sharp,
	int Flag_Zero,
	int Flag_Minus,
	int Flag_Plus,
	int Flag_Space,
	int Width,
	int Precision,
	char *Dest,
	size_t BufferBytes,
	int DestCharCodeFormat
)
{
	size_t DestSize ;
	size_t WriteBytes ;
	size_t NulWriteBytes ;
	DWORD NumberNum = 0 ;
	DWORD SignedCharNum ;
	int NumberMinus ;
	int FPClass ;
	int NaN ;
	int i ;
	BYTE NumberStrTemp[ 1024 + 32 ] ;
	BYTE *AllocNumberStrTempBuffer = NULL ;
	BYTE *UseNumberStrTempBuffer ;

	// デフォルトのサイズを超える場合は一時バッファを確保する
	if( Precision >= 0 && sizeof( NumberStrTemp ) - 32 <= ( size_t )Precision )
	{
		AllocNumberStrTempBuffer = ( BYTE * )DXALLOC( Precision + 32 ) ;

		// バッファが確保できなかった場合は文字列化する桁数を 1024 に変更する
		if( AllocNumberStrTempBuffer == NULL )
		{
			Precision = 1024 ;
			UseNumberStrTempBuffer = NumberStrTemp ;
		}
		else
		{
			UseNumberStrTempBuffer = AllocNumberStrTempBuffer ;
		}
	}
	else
	{
		UseNumberStrTempBuffer = NumberStrTemp ;
	}

	DestSize = 0 ;

	// 最初にヌル文字を書き込む
	NulWriteBytes = PutCharCode_BufferBytes_inline( 0x00, DestCharCodeFormat, ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize ) ;
	if( NulWriteBytes == 0 )
	{
		goto END ;
	}

	// NaNチェック
	FPClass = _FPCLASS( Number ) ;
	NaN = 0 ;
	if( FPClass == _FPCLASS_SNAN ||
		FPClass == _FPCLASS_QNAN ||
		FPClass == _FPCLASS_NINF ||
		FPClass == _FPCLASS_PINF )
	{
		NaN = 1 ;

		// NaNの値はプラス扱い
		if( FPClass == _FPCLASS_SNAN ||
			FPClass == _FPCLASS_QNAN )
		{
			NumberMinus = 0 ;
		}
		else
		{
			// それ以外は負の無限大の場合のみマイナス扱い
			NumberMinus = FPClass == _FPCLASS_NINF ? 1 : 0 ;
		}
	}
	else
	{
		// 通常の値の場合の処理

		// マイナスの値はプラスにする
		NumberMinus = 0 ;
		if( Number < 0.0 )
		{
			Number = -Number ;
			NumberMinus = 1 ;
		}
	}

	// 幅の指定が無い場合は仮で 0 をセット
	if( Width < 0 )
	{
		Width = 0 ;
	}

	// 左詰め指定されている場合は Flag_Zero は無効
	if( Flag_Minus )
	{
		Flag_Zero = 0 ;
	}

	// Flag_Plus がある場合、Flag_Space は意味を成さない
	if( Flag_Plus )
	{
		Flag_Space = 0 ;
	}

	// 値がマイナス値の場合、Flag_Plus, Flag_Space は意味を成さない
	if( NumberMinus )
	{
		Flag_Space = 0 ;
		Flag_Plus  = 0 ;
	}

	// 数値を文字列化と文字数の算出
	if( NaN )
	{
		// NaN の場合

		switch( Type )
		{
		case PRINTF_TYPE_f :
			NumberNum = CL_vsnprintf_help_ftoa_NaN_f( FPClass, Flag_Sharp, 1, Precision, UseNumberStrTempBuffer ) ;
			break ;

		case PRINTF_TYPE_e :
		case PRINTF_TYPE_E :
			NumberNum = CL_vsnprintf_help_ftoa_NaN_e( FPClass, Flag_Sharp, Precision, Type == PRINTF_TYPE_E ? 1 : 0, UseNumberStrTempBuffer ) ;
			break ;

		case PRINTF_TYPE_g :
		case PRINTF_TYPE_G :
			NumberNum = CL_vsnprintf_help_ftoa_NaN_g( FPClass, Flag_Sharp, Precision, UseNumberStrTempBuffer ) ;
			break ;

		case PRINTF_TYPE_a :
		case PRINTF_TYPE_A :
			NumberNum = CL_vsnprintf_help_ftoa_NaN_a( FPClass, Flag_Sharp, Precision, Type == PRINTF_TYPE_A ? 1 : 0, UseNumberStrTempBuffer ) ;
			break ;
		}
	}
	else
	{
		// 通常数値の場合

		switch( Type )
		{
		case PRINTF_TYPE_f :
			NumberNum = CL_vsnprintf_help_ftoa_f( Number, Flag_Sharp, Precision, UseNumberStrTempBuffer ) ;
			break ;

		case PRINTF_TYPE_e :
		case PRINTF_TYPE_E :
			NumberNum = CL_vsnprintf_help_ftoa_e( Number, Flag_Sharp, Precision, Type == PRINTF_TYPE_E ? 1 : 0, UseNumberStrTempBuffer ) ;
			break ;

		case PRINTF_TYPE_g :
		case PRINTF_TYPE_G :
			NumberNum = CL_vsnprintf_help_ftoa_g( Number, Flag_Sharp, Precision, Type == PRINTF_TYPE_G ? 1 : 0, UseNumberStrTempBuffer ) ;
			break ;

		case PRINTF_TYPE_a :
		case PRINTF_TYPE_A :
			NumberNum = CL_vsnprintf_help_ftoa_a( Number, Flag_Sharp, Precision, Type == PRINTF_TYPE_A ? 1 : 0, UseNumberStrTempBuffer ) ;
			break ;
		}
	}

	// 符号やスペースの文字数を算出
	if( Flag_Space || Flag_Plus || NumberMinus )
	{
		SignedCharNum = 1 ;
	}
	else
	{
		SignedCharNum = 0 ;
	}

	// 幅から数字の文字数、符号文字、進数付属文字を引く
	Width -= NumberNum + SignedCharNum ;

	// 左詰め指定があるかどうかで処理を分岐
	if( Flag_Minus )
	{
		// 左詰め指定がある場合

		// 符号系文字がある場合はセットする
		if( NumberMinus || Flag_Plus || Flag_Space )
		{
			WriteBytes = PutCharCode_BufferBytes_inline( NumberMinus ? '-' : ( Flag_Plus ? '+' : ' ' ), DestCharCodeFormat, ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize ) ;
			if( WriteBytes == 0 || BufferBytes - DestSize < WriteBytes + NulWriteBytes )
			{
				goto END ;
			}
			DestSize += WriteBytes ;
			if( BufferBytes - DestSize <= NulWriteBytes )
			{
				goto END ;
			}
		}

		// 数値をセットする
		for( i = 0 ; i < ( int )NumberNum ; i++ )
		{
			WriteBytes = PutCharCode_BufferBytes_inline( UseNumberStrTempBuffer[ i ], DestCharCodeFormat, ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize ) ;
			if( WriteBytes == 0 || BufferBytes - DestSize < WriteBytes + NulWriteBytes )
			{
				goto END ;
			}
			DestSize += WriteBytes ;
			if( BufferBytes - DestSize <= NulWriteBytes )
			{
				goto END ;
			}
		}

		// 余剰幅がある場合は、スペースをセットする
		if( Width > 0 )
		{
			DestSize += CL_vsnprintf_help_set_char( Width, ' ', ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize, DestCharCodeFormat ) ;
			if( BufferBytes - DestSize <= NulWriteBytes )
			{
				goto END ;
			}
		}
	}
	else
	{
		// 0詰め指定があるかどうかで処理を分岐
		if( Flag_Zero )
		{
			// 符号系文字がある場合はセットする
			if( NumberMinus || Flag_Plus || Flag_Space )
			{
				WriteBytes = PutCharCode_BufferBytes_inline( NumberMinus ? '-' : ( Flag_Plus ? '+' : ' ' ), DestCharCodeFormat, ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize ) ;
				if( WriteBytes == 0 || BufferBytes - DestSize < WriteBytes + NulWriteBytes )
				{
					goto END ;
				}
				DestSize += WriteBytes ;
				if( BufferBytes - DestSize <= NulWriteBytes )
				{
					goto END ;
				}
			}

			// 余剰幅がある場合は、0をセットする
			if( Width > 0 )
			{
				DestSize += CL_vsnprintf_help_set_char( Width, '0', ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize, DestCharCodeFormat ) ;
				if( BufferBytes - DestSize <= NulWriteBytes )
				{
					goto END ;
				}
			}
		}
		else
		{
			// 余剰幅がある場合は、スペースをセットする
			if( Width > 0 )
			{
				DestSize += CL_vsnprintf_help_set_char( Width, ' ', ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize, DestCharCodeFormat ) ;
				if( BufferBytes - DestSize <= NulWriteBytes )
				{
					goto END ;
				}
			}

			// 符号系文字がある場合はセットする
			if( NumberMinus || Flag_Plus || Flag_Space )
			{
				WriteBytes = PutCharCode_BufferBytes_inline( NumberMinus ? '-' : ( Flag_Plus ? '+' : ' ' ), DestCharCodeFormat, ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize ) ;
				if( WriteBytes == 0 || BufferBytes - DestSize < WriteBytes + NulWriteBytes )
				{
					goto END ;
				}
				DestSize += WriteBytes ;
				if( BufferBytes - DestSize <= NulWriteBytes )
				{
					goto END ;
				}
			}
		}

		// 数値をセットする
		for( i = 0 ; i < ( int )NumberNum ; i++ )
		{
			WriteBytes = PutCharCode_BufferBytes_inline( UseNumberStrTempBuffer[ i ], DestCharCodeFormat, ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize ) ;
			if( WriteBytes == 0 || BufferBytes - DestSize < WriteBytes + NulWriteBytes )
			{
				goto END ;
			}
			DestSize += WriteBytes ;
			if( BufferBytes - DestSize <= NulWriteBytes )
			{
				goto END ;
			}
		}
	}

END :

	// メモリを確保していたら解放する
	if( AllocNumberStrTempBuffer != NULL )
	{
		DXFREE( AllocNumberStrTempBuffer ) ;
		AllocNumberStrTempBuffer = NULL ;
	}

	return DestSize ;
}

// 整数値を文字列化する関数、戻り値は書き込んだバイト数
static size_t CL_vsnprintf_help_itoa(
	ULONGLONG Number,
	int NumberMinus,
	int Signed,
	DWORD Decimal,
	int Flag_Sharp,
	int Flag_Zero,
	int Flag_Minus,
	int Flag_Plus,
	int Flag_Space,
	int Width,
	int Precision,
	int Big,
	char *Dest,
	size_t BufferBytes,
	int DestCharCodeFormat
)
{
	size_t DestSize ;
	size_t WriteBytes ;
	size_t NulWriteBytes ;

	DWORD NumberNum ;
	DWORD SignedCharNum ;
	DWORD SharpCharNum ;
	DWORD PrecisionCharNum ;
	int i ;
	int Flag_Precision ;
	BYTE NumberStrTemp[ 1024 + 32 ] ;
	BYTE *AllocNumberStrTempBuffer = NULL ;
	BYTE *UseNumberStrTempBuffer ;

	// デフォルトのサイズを超える場合は一時バッファを確保する
	if( Precision >= 0 && sizeof( NumberStrTemp ) - 32 <= ( size_t )Precision )
	{
		AllocNumberStrTempBuffer = ( BYTE * )DXALLOC( Precision + 32 ) ;

		// バッファが確保できなかった場合は文字列化する桁数を 1024 に変更する
		if( AllocNumberStrTempBuffer == NULL )
		{
			Precision = 1024 ;
			UseNumberStrTempBuffer = NumberStrTemp ;
		}
		else
		{
			UseNumberStrTempBuffer = AllocNumberStrTempBuffer ;
		}
	}
	else
	{
		UseNumberStrTempBuffer = NumberStrTemp ;
	}

	DestSize = 0 ;

	// 最初にヌル文字を書き込む
	NulWriteBytes = PutCharCode_BufferBytes_inline( 0x00, DestCharCodeFormat, ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize ) ;
	if( NulWriteBytes == 0 )
	{
		goto END ;
	}

	// 幅の指定が無い場合は仮で 0 をセット
	if( Width < 0 )
	{
		Width = 0 ;
	}

	// 精度の指定が無い場合は既定の 1 をセット
	if( Precision < 0 )
	{
		Precision = 1 ;
		Flag_Precision = 0 ;
	}
	else
	{
		Flag_Precision = 1 ;
	}

	// 10進数以外は符号無し
	if( Decimal != 10 )
	{
		Signed = 0 ;
	}

	// 左詰め指定されているか、精度指定がある場合は Flag_Zero は無効
	if( Flag_Minus || Flag_Precision )
	{
		Flag_Zero = 0 ;
	}

	// 符号が無い場合は マイナス値、Flag_Plus, Flag_Space は意味を成さない
	if( Signed == 0 )
	{
		NumberMinus = 0 ;
		Flag_Plus   = 0 ;
		Flag_Space  = 0 ;
	}
	else
	{
		// 符号がある場合は # は意味を成さない
		Flag_Sharp = 0 ;

		// Flag_Plus がある場合、Flag_Space は意味を成さない
		if( Flag_Plus )
		{
			Flag_Space = 0 ;
		}

		// 値がマイナス値の場合、Flag_Plus, Flag_Space は意味を成さない
		if( NumberMinus )
		{
			Flag_Space = 0 ;
			Flag_Plus  = 0 ;
		}
	}

	// 数値を文字列化と文字数の算出
	NumberNum = 0 ;
	if( Number == 0 )
	{
		// 数値が 0 でも精度が 0 ではなければ 0 の１文字を出力する
		if( Precision == 0 )
		{
			UseNumberStrTempBuffer[ NumberNum ] = '0' ;
			NumberNum ++ ;
		}
	}
	else
	{
		while( Number > 0 )
		{
			UseNumberStrTempBuffer[ NumberNum ] = NumberToCharTable[ Big ][ UINT64MOD( ( const BYTE * )&Number, Decimal ) ] ;
			NumberNum ++ ;
			Number /= Decimal ;
		}
	}

	// 精度による文字数を算出
	if( NumberNum < ( DWORD )Precision )
	{
		PrecisionCharNum = Precision - NumberNum ;
	}
	else
	{
		PrecisionCharNum = 0 ;
	}

	// # がある場合の文字数を算出
	SharpCharNum = 0 ;
	if( Flag_Sharp )
	{
		switch( Decimal )
		{
		case 8 :
			SharpCharNum = 1 ;
			break ;

		case 16 :
			SharpCharNum = 2 ;
			break ;
		}
	}

	// 符号やスペースの文字数を算出
	if( Flag_Space || Flag_Plus || NumberMinus )
	{
		SignedCharNum = 1 ;
	}
	else
	{
		SignedCharNum = 0 ;
	}

	// 幅から数字の文字数、符号文字、進数付属文字を引く
	Width -= NumberNum + PrecisionCharNum + SignedCharNum + SharpCharNum ;

	// 左詰め指定があるかどうかで処理を分岐
	if( Flag_Minus )
	{
		// 左詰め指定がある場合

		// 符号系文字がある場合はセットする
		if( NumberMinus || Flag_Plus || Flag_Space )
		{
			WriteBytes = PutCharCode_BufferBytes_inline( NumberMinus ? '-' : ( Flag_Plus ? '+' : ' ' ), DestCharCodeFormat, ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize ) ;
			if( WriteBytes == 0 || BufferBytes - DestSize < WriteBytes + NulWriteBytes )
			{
				goto END ;
			}
			DestSize += WriteBytes ;
			if( BufferBytes - DestSize <= NulWriteBytes )
			{
				goto END ;
			}
		}

		// 進数付属文字がある場合は記号をセットする
		if( Flag_Sharp )
		{
			if( Decimal == 8 )
			{
				WriteBytes = PutCharCode_BufferBytes_inline( '0', DestCharCodeFormat, ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize ) ;
				if( WriteBytes == 0 || BufferBytes - DestSize < WriteBytes + NulWriteBytes )
				{
					goto END ;
				}
				DestSize += WriteBytes ;
				if( BufferBytes - DestSize <= NulWriteBytes )
				{
					goto END ;
				}
			}
			else
			{
				WriteBytes = PutCharCode_BufferBytes_inline( '0',             DestCharCodeFormat, ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize ) ;
				if( WriteBytes == 0 || BufferBytes - DestSize < WriteBytes + NulWriteBytes )
				{
					goto END ;
				}
				DestSize += WriteBytes ;
				if( BufferBytes - DestSize <= NulWriteBytes )
				{
					goto END ;
				}

				WriteBytes = PutCharCode_BufferBytes_inline( Big ? 'X' : 'x', DestCharCodeFormat, ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize ) ;
				if( WriteBytes == 0 || BufferBytes - DestSize < WriteBytes + NulWriteBytes )
				{
					goto END ;
				}
				DestSize += WriteBytes ;
				if( BufferBytes - DestSize <= NulWriteBytes )
				{
					goto END ;
				}
			}
		}

		// 精度に足りない分の 0 をセットする
		if( PrecisionCharNum > 0 )
		{
			DestSize += CL_vsnprintf_help_set_char( PrecisionCharNum, '0', ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize, DestCharCodeFormat ) ;
			if( BufferBytes - DestSize <= NulWriteBytes )
			{
				goto END ;
			}
		}

		// 数値をセットする
		for( i = NumberNum - 1 ; i >= 0 ; i-- )
		{
			WriteBytes = PutCharCode_BufferBytes_inline( UseNumberStrTempBuffer[ i ], DestCharCodeFormat, ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize ) ;
			if( WriteBytes == 0 || BufferBytes - DestSize < WriteBytes + NulWriteBytes )
			{
				goto END ;
			}
			DestSize += WriteBytes ;
			if( BufferBytes - DestSize <= NulWriteBytes )
			{
				goto END ;
			}
		}

		// 余剰幅がある場合は、スペースをセットする
		if( Width > 0 )
		{
			DestSize += CL_vsnprintf_help_set_char( Width, ' ', ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize, DestCharCodeFormat ) ;
			if( BufferBytes - DestSize <= NulWriteBytes )
			{
				goto END ;
			}
		}
	}
	else
	{
		// 0詰め指定があるかどうかで処理を分岐
		if( Flag_Zero )
		{
			// 符号系文字がある場合はセットする
			if( NumberMinus || Flag_Plus || Flag_Space )
			{
				WriteBytes = PutCharCode_BufferBytes_inline( NumberMinus ? '-' : ( Flag_Plus ? '+' : ' ' ), DestCharCodeFormat, ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize ) ;
				if( WriteBytes == 0 || BufferBytes - DestSize < WriteBytes + NulWriteBytes )
				{
					goto END ;
				}
				DestSize += WriteBytes ;
				if( BufferBytes - DestSize <= NulWriteBytes )
				{
					goto END ;
				}
			}

			// 進数付属文字がある場合は記号をセットする
			if( Flag_Sharp )
			{
				if( Decimal == 8 )
				{
					WriteBytes = PutCharCode_BufferBytes_inline( '0', DestCharCodeFormat, ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize ) ;
					if( WriteBytes == 0 || BufferBytes - DestSize < WriteBytes + NulWriteBytes )
					{
						goto END ;
					}
					DestSize += WriteBytes ;
					if( BufferBytes - DestSize <= NulWriteBytes )
					{
						goto END ;
					}
				}
				else
				{
					WriteBytes = PutCharCode_BufferBytes_inline( '0',             DestCharCodeFormat, ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize ) ;
					if( WriteBytes == 0 || BufferBytes - DestSize < WriteBytes + NulWriteBytes )
					{
						goto END ;
					}
					DestSize += WriteBytes ;
					if( BufferBytes - DestSize <= NulWriteBytes )
					{
						goto END ;
					}

					WriteBytes = PutCharCode_BufferBytes_inline( Big ? 'X' : 'x', DestCharCodeFormat, ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize ) ;
					if( WriteBytes == 0 || BufferBytes - DestSize < WriteBytes + NulWriteBytes )
					{
						goto END ;
					}
					DestSize += WriteBytes ;
					if( BufferBytes - DestSize <= NulWriteBytes )
					{
						goto END ;
					}
				}
			}

			// 余剰幅がある場合は、0をセットする
			if( Width > 0 )
			{
				DestSize += CL_vsnprintf_help_set_char( Width, '0', ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize, DestCharCodeFormat ) ;
				if( BufferBytes - DestSize <= NulWriteBytes )
				{
					goto END ;
				}
			}
		}
		else
		{
			// 余剰幅がある場合は、スペースをセットする
			if( Width > 0 )
			{
				DestSize += CL_vsnprintf_help_set_char( Width, ' ', ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize, DestCharCodeFormat ) ;
				if( BufferBytes - DestSize <= NulWriteBytes )
				{
					goto END ;
				}
			}

			// 符号系文字がある場合はセットする
			if( NumberMinus || Flag_Plus || Flag_Space )
			{
				WriteBytes = PutCharCode_BufferBytes_inline( NumberMinus ? '-' : ( Flag_Plus ? '+' : ' ' ), DestCharCodeFormat, ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize ) ;
				if( WriteBytes == 0 || BufferBytes - DestSize < WriteBytes + NulWriteBytes )
				{
					goto END ;
				}
				DestSize += WriteBytes ;
				if( BufferBytes - DestSize <= NulWriteBytes )
				{
					goto END ;
				}
			}

			// 進数付属文字がある場合は記号をセットする
			if( Flag_Sharp )
			{
				if( Decimal == 8 )
				{
					WriteBytes = PutCharCode_BufferBytes_inline( '0', DestCharCodeFormat, ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize ) ;
					if( WriteBytes == 0 || BufferBytes - DestSize < WriteBytes + NulWriteBytes )
					{
						goto END ;
					}
					DestSize += WriteBytes ;
					if( BufferBytes - DestSize <= NulWriteBytes )
					{
						goto END ;
					}
				}
				else
				{
					WriteBytes = PutCharCode_BufferBytes_inline( '0',             DestCharCodeFormat, ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize ) ;
					if( WriteBytes == 0 || BufferBytes - DestSize < WriteBytes + NulWriteBytes )
					{
						goto END ;
					}
					DestSize += WriteBytes ;
					if( BufferBytes - DestSize <= NulWriteBytes )
					{
						goto END ;
					}

					WriteBytes = PutCharCode_BufferBytes_inline( Big ? 'X' : 'x', DestCharCodeFormat, ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize ) ;
					if( WriteBytes == 0 || BufferBytes - DestSize < WriteBytes + NulWriteBytes )
					{
						goto END ;
					}
					DestSize += WriteBytes ;
					if( BufferBytes - DestSize <= NulWriteBytes )
					{
						goto END ;
					}
				}
			}
		}

		// 精度に足りない分の 0 をセットする
		if( PrecisionCharNum > 0 )
		{
			DestSize += CL_vsnprintf_help_set_char( PrecisionCharNum, '0', ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize, DestCharCodeFormat ) ;
			if( BufferBytes - DestSize <= NulWriteBytes )
			{
				goto END ;
			}
		}

		// 数値をセットする
		for( i = NumberNum - 1 ; i >= 0 ; i-- )
		{
			WriteBytes = PutCharCode_BufferBytes_inline( UseNumberStrTempBuffer[ i ], DestCharCodeFormat, ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize ) ;
			if( WriteBytes == 0 || BufferBytes - DestSize < WriteBytes + NulWriteBytes )
			{
				goto END ;
			}
			DestSize += WriteBytes ;
			if( BufferBytes - DestSize <= NulWriteBytes )
			{
				goto END ;
			}
		}
	}

END :

	// メモリを確保していたら解放する
	if( AllocNumberStrTempBuffer != NULL )
	{
		DXFREE( AllocNumberStrTempBuffer ) ;
		AllocNumberStrTempBuffer = NULL ;
	}

	return DestSize ;
}

// s と S を処理する関数、戻り値は書き込んだバイト数
static size_t CL_vsnprintf_help_s(
	char *String,
	int Flag_Zero,
	int Flag_Minus,
	int Width,
	int Precision,
	char *Dest,
	size_t BufferBytes,
	int DestCharCodeFormat
)
{
	size_t DestSize ;
	size_t NulWriteBytes ;
	BYTE NullBuffer[ 128 ] ;
	int StrLength ;
	size_t UnitSize ;

	DestSize = 0 ;

	// 最初にヌル文字を書き込む
	NulWriteBytes = PutCharCode_BufferBytes_inline( 0x00, DestCharCodeFormat, ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize ) ;
	if( NulWriteBytes == 0 )
	{
		return 0 ;
	}

	// NULL の場合は(null)をセット
	if( String == NULL )
	{
		int TempAddr = 0 ;
		TempAddr += PutCharCode_inline( '(', DestCharCodeFormat, ( char * )&NullBuffer[ TempAddr ] ) ;
		TempAddr += PutCharCode_inline( 'n', DestCharCodeFormat, ( char * )&NullBuffer[ TempAddr ] ) ;
		TempAddr += PutCharCode_inline( 'u', DestCharCodeFormat, ( char * )&NullBuffer[ TempAddr ] ) ;
		TempAddr += PutCharCode_inline( 'l', DestCharCodeFormat, ( char * )&NullBuffer[ TempAddr ] ) ;
		TempAddr += PutCharCode_inline( 'l', DestCharCodeFormat, ( char * )&NullBuffer[ TempAddr ] ) ;
		TempAddr += PutCharCode_inline( ')', DestCharCodeFormat, ( char * )&NullBuffer[ TempAddr ] ) ;
		TempAddr += PutCharCode_inline(   0, DestCharCodeFormat, ( char * )&NullBuffer[ TempAddr ] ) ;

		String = ( char * )NullBuffer ;
	}

	// 文字列の長さを取得
	StrLength = ( int )CL_strlen( DestCharCodeFormat, String ) ;

	// 幅の指定が無い場合は仮で 0 をセット
	if( Width < 0 )
	{
		Width = 0 ;
	}

	// 精度の指定が無い場合は文字列の長さをセット
	if( Precision < 0 )
	{
		Precision = StrLength ;
	}
	else
	{
		// 精度が文字列長以下の場合は精度を文字列長にする
		if( Precision > StrLength )
		{
			Precision = StrLength ;
		}
	}

	// 左詰め指定されている場合は Flag_Zero は無効
	if( Flag_Minus )
	{
		Flag_Zero = 0 ;
	}

	// 幅から精度を引く
	Width -= Precision ;

	// 左詰め指定があるかどうかで処理を分岐
	UnitSize = ( size_t )GetCharCodeFormatUnitSize_inline( DestCharCodeFormat ) ;
	if( Flag_Minus )
	{
		// 左詰め指定がある場合

		// 文字列をコピーする
		if( BufferBytes - DestSize < UnitSize * Precision + NulWriteBytes )
		{
			Precision = ( int )( ( BufferBytes - DestSize - NulWriteBytes ) / UnitSize ) ;
		}
		CL_strncpy( DestCharCodeFormat, ( char * )&( ( BYTE * )Dest )[ DestSize ], String, Precision ) ;
		DestSize += UnitSize * Precision ;
		if( BufferBytes - DestSize <= NulWriteBytes )
		{
			goto END ;
		}

		// 余剰幅がある場合は、スペースをセットする
		if( Width > 0 )
		{
			DestSize += CL_vsnprintf_help_set_char( Width, ' ', ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize, DestCharCodeFormat ) ;
			if( BufferBytes - DestSize <= NulWriteBytes )
			{
				goto END ;
			}
		}
	}
	else
	{
		// 0詰め指定があるかどうかで処理を分岐
		if( Flag_Zero )
		{
			// 余剰幅がある場合は、0をセットする
			if( Width > 0 )
			{
				DestSize += CL_vsnprintf_help_set_char( Width, '0', ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize, DestCharCodeFormat ) ;
				if( BufferBytes - DestSize <= NulWriteBytes )
				{
					goto END ;
				}
			}
		}
		else
		{
			// 余剰幅がある場合は、スペースをセットする
			if( Width > 0 )
			{
				DestSize += CL_vsnprintf_help_set_char( Width, ' ', ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize, DestCharCodeFormat ) ;
				if( BufferBytes - DestSize <= NulWriteBytes )
				{
					goto END ;
				}
			}
		}

		// 文字列をコピーする
		if( BufferBytes - DestSize < UnitSize * Precision + NulWriteBytes )
		{
			Precision = ( int )( ( BufferBytes - DestSize - NulWriteBytes ) / UnitSize ) ;
		}
		CL_strncpy( DestCharCodeFormat, ( char * )&( ( BYTE * )Dest )[ DestSize ], String, Precision ) ;
		DestSize += UnitSize * Precision ;
		if( BufferBytes - DestSize <= NulWriteBytes )
		{
			goto END ;
		}
	}

END :

	return DestSize ;
}

// c と C を処理する関数、戻り値は書き込んだバイト数
static size_t CL_vsnprintf_help_c(
	DWORD CharCode,
	int Flag_Zero,
	int Flag_Minus,
	int Width,
	char *Dest,
	size_t BufferBytes,
	int DestCharCodeFormat
)
{
	size_t DestSize ;
	size_t WriteBytes ;
	size_t NulWriteBytes ;

	DestSize = 0 ;

	// 最初にヌル文字を書き込む
	NulWriteBytes = PutCharCode_BufferBytes_inline( 0x00, DestCharCodeFormat, ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize ) ;
	if( NulWriteBytes == 0 )
	{
		return 0 ;
	}

	// 幅の指定が無い場合は仮で 1 をセット
	if( Width < 0 )
	{
		Width = 1 ;
	}

	// 左詰め指定されている場合は Flag_Zero は無効
	if( Flag_Minus )
	{
		Flag_Zero = 0 ;
	}

	// 幅から１文字を引く
	Width -= 1 ;

	// 左詰め指定があるかどうかで処理を分岐
	if( Flag_Minus )
	{
		// 左詰め指定がある場合

		// 文字をセットする
		WriteBytes = PutCharCode_BufferBytes_inline( CharCode, DestCharCodeFormat, ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize ) ;
		if( WriteBytes == 0 || BufferBytes - DestSize < WriteBytes + NulWriteBytes )
		{
			goto END ;
		}
		DestSize += WriteBytes ;
		if( BufferBytes - DestSize <= NulWriteBytes )
		{
			goto END ;
		}

		// 余剰幅がある場合は、スペースをセットする
		if( Width > 0 )
		{
			DestSize += CL_vsnprintf_help_set_char( Width, ' ', ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize, DestCharCodeFormat ) ;
		}
	}
	else
	{
		// 0詰め指定があるかどうかで処理を分岐
		if( Flag_Zero )
		{
			// 余剰幅がある場合は、0をセットする
			if( Width > 0 )
			{
				DestSize += CL_vsnprintf_help_set_char( Width, '0', ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize, DestCharCodeFormat ) ;
			}
		}
		else
		{
			// 余剰幅がある場合は、スペースをセットする
			if( Width > 0 )
			{
				DestSize += CL_vsnprintf_help_set_char( Width, ' ', ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize, DestCharCodeFormat ) ;
			}
		}
		if( BufferBytes - DestSize <= NulWriteBytes )
		{
			goto END ;
		}

		// 文字をセットする
		DestSize += PutCharCode_BufferBytes_inline( CharCode, DestCharCodeFormat, ( char * )&( ( BYTE * )Dest )[ DestSize ], BufferBytes - DestSize ) ;
	}

END :

	return DestSize ;
}

extern int CL_vsprintf( int CharCodeFormat, int IsWChar, int CharCharCodeFormat, int WCharCharCodeFormat, char *Buffer, const char *FormatString, va_list Arg )
{
	return CL_vsnprintf( CharCodeFormat, IsWChar, CharCharCodeFormat, WCharCharCodeFormat, Buffer, BUFFERBYTES_CANCEL, FormatString, Arg );
}

extern int CL_vsnprintf( int CharCodeFormat, int IsWChar, int CharCharCodeFormat, int WCharCharCodeFormat, char *Buffer, size_t BufferSize, const char *FormatString, va_list Arg )
{
	int i ;
	size_t DestSize ;
	size_t WriteBytes ;
	size_t NulWriteBytes ;
	size_t BufferBytes ;
	size_t UnitSize ;
	DWORD BaseBuffer[ 1024 ] ;
	DWORD *TempBuffer = NULL ;
	DWORD *FCode ;
	int FormatStringSize ;

	// １文字４バイト形式に変換する
	FormatStringSize = StringToCharCodeString_inline( FormatString, CharCodeFormat, NULL ) ;
	if( FormatStringSize + sizeof( DWORD ) * 16 > sizeof( BaseBuffer ) )
	{
		TempBuffer = ( DWORD * )DXALLOC( FormatStringSize + sizeof( DWORD ) * 16 ) ;
		if( TempBuffer == NULL )
		{
			return -1 ;
		}

		FCode = TempBuffer ;
	}
	else
	{
		FCode = BaseBuffer ;
	}
	_MEMSET( FCode, 0, FormatStringSize + sizeof( DWORD ) * 16 ) ;
	StringToCharCodeString_inline( FormatString, CharCodeFormat, FCode ) ;

	UnitSize = ( size_t )GetCharCodeFormatUnitSize_inline( CharCodeFormat ) ;
	BufferBytes = UnitSize * BufferSize ;
	DestSize = 0 ;

	NulWriteBytes = PutCharCode_BufferBytes_inline( 0x00, CharCodeFormat, ( char * )&( ( BYTE * )Buffer )[ DestSize ], BufferBytes - DestSize ) ;
	if( NulWriteBytes == 0 )
	{
		return 0 ;
	}

	for( i = 0 ; FCode[ i ] != 0 ; )
	{
		if( FCode[ i ] == 0x25 && FCode[ i + 1 ] == 0x25 )
		{
			WriteBytes = PutCharCode_BufferBytes_inline( 0x25, CharCodeFormat, ( char * )&( ( BYTE * )Buffer )[ DestSize ], BufferBytes - DestSize ) ;
			if( WriteBytes == 0 || BufferBytes - DestSize < WriteBytes + NulWriteBytes )
			{
				goto END ;
			}
			DestSize += WriteBytes ;
			i += 2 ;
			if( BufferBytes - DestSize <= NulWriteBytes )
			{
				goto END ;
			}
		}
		else
		if( FCode[ i ] != 0x25 || FCode[ i + 1 ] == 0x25 )
		{
			WriteBytes = PutCharCode_BufferBytes_inline( FCode[ i ], CharCodeFormat, ( char * )&( ( BYTE * )Buffer )[ DestSize ], BufferBytes - DestSize ) ;
			if( WriteBytes == 0 || BufferBytes - DestSize < WriteBytes + NulWriteBytes )
			{
				goto END ;
			}
			DestSize += WriteBytes ;
			i ++ ;
			if( BufferBytes - DestSize <= NulWriteBytes )
			{
				goto END ;
			}
		}
		else
		{
			int Flag_Sharp   = 0 ;
			int Flag_Zero    = 0 ;
			int Flag_Space   = 0 ;
			int Flag_Plus    = 0 ;
			int Flag_Minus   = 0 ;
			int FlagEnd      = 0 ;
			int Width        = -1 ;
			int Dot          = 0 ;
			int Precision    = -1 ;
			int PrecisionEnd = 0 ;
			int Prefix       = -1 ;
			int Type         = -1 ;
			int LoopEnd      = 0 ;
			int UseCharNum ;

			i ++ ;
			do
			{
				switch( FCode[ i ] )
				{
				default :
					LoopEnd = 1 ;
					break ;

				case '#':
				case '-':
				case '+':
				case ' ':
					// フラグディレクティブは、フラグディレクティブを指定できる範囲から外れていたらエラーとなる
					if( FlagEnd )
					{
						LoopEnd = 1 ;
					}
					else
					{
						switch( FCode[ i ] )
						{
						case '#':	Flag_Sharp = 1 ; break ;
						case '-':	Flag_Minus = 1 ; break ;
						case '+':	Flag_Plus  = 1 ; break ;
						case ' ':	Flag_Space = 1 ; break ;
						}
						i ++ ;
					}
					break ;

				case '.':
					// 既に . があった場合、二つ以上の . はエラーとなる
					if( Dot )
					{
						LoopEnd = 1 ;
					}
					else
					{
						// . は精度指定の開始と共に、フラグディレクティブと文字幅指定できる範囲の終了でもある
						FlagEnd   = 1 ;
						Dot       = 1 ;
						Precision = 0 ;
						i ++ ;
					}
					break ;

					// * は . の前か後かで処理が分かれる
				case '*':
					// 精度指定の範囲外で数値があったらエラーとなる
					if( PrecisionEnd )
					{
						LoopEnd = 1 ;
					}
					else
					{
						// . の後の場合は精度、前の場合は文字幅指定
						if( Dot )
						{
							Precision = va_arg( Arg, int ) ;
							i ++ ;
						}
						else
						{
							Width = va_arg( Arg, int ) ;
							i ++ ;
						}
					}
					break ;

					// 数字は . の前か後かで処理が分かれる
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					// 精度指定の範囲外で数値があったらエラーとなる
					if( PrecisionEnd )
					{
						LoopEnd = 1 ;
					}
					else
					{
						// . の後の場合は精度、前の場合は文字幅指定
						if( Dot )
						{
							Precision = ( int )CL_vsnprintf_help_getnumber( &FCode[ i ], &UseCharNum ) ;
							i += UseCharNum ;
						}
						else
						{
							// . の前でフラグディレクティブ指定できる範囲で '0' であった場合はフラグディレクティブとしての '0' と判断する
							if( FlagEnd == 0 && FCode[ i ] == '0' )
							{
								Flag_Zero = 1 ;
								i ++ ;
							}
							else
							{
								// それ以外の場合は文字幅指定、文字幅指定はフラグディレクティブ範囲の終了でもある
								FlagEnd = 1 ;
								Width = ( int )CL_vsnprintf_help_getnumber( &FCode[ i ], &UseCharNum ) ;
								i += UseCharNum ;
							}
						}
					}
					break ;

				case 'h':
				case 'w':
				case 'l':
				case 'I':
					// プレフィックス指定はフラグディレクティブ、文字幅指定、ドット、精度指定範囲の終了でもある
					FlagEnd      = 1 ;
					Dot          = 1 ;
					PrecisionEnd = 1 ;
					switch( FCode[ i ] )
					{
					case 'h':	Prefix = PRINTF_SIZE_PREFIX_h ;	i ++ ;	break ;
					case 'w':	Prefix = PRINTF_SIZE_PREFIX_w ;	i ++ ;	break ;
					case 'l':
						if( FCode[ i + 1 ] == 'l' )
						{
							Prefix = PRINTF_SIZE_PREFIX_ll ;
							i += 2 ;
						}
						else
						{
							Prefix = PRINTF_SIZE_PREFIX_l ;
							i ++ ;
						}
						break ;

					case 'I':
						if( FCode[ i + 1 ] == '3' && FCode[ i + 2 ] == '2' )
						{
							Prefix = PRINTF_SIZE_PREFIX_I32 ;
							i += 3 ;
						}
						else
						if( FCode[ i + 1 ] == '6' && FCode[ i + 2 ] == '4' )
						{
							Prefix = PRINTF_SIZE_PREFIX_I64 ;
							i += 3 ;
						}
						else
						{
							Prefix = PRINTF_SIZE_PREFIX_I ;
							i ++ ;
						}
						break ;
					}
					break ;

					// 型指定は、型指定であると共に書式指定構文の終了でもある
				case 'c':	Type = PRINTF_TYPE_c ;	i ++ ;	LoopEnd = 1 ;	break ;
				case 'C':	Type = PRINTF_TYPE_C ;	i ++ ;	LoopEnd = 1 ;	break ;
				case 'd':	Type = PRINTF_TYPE_d ;	i ++ ;	LoopEnd = 1 ;	break ;
				case 'i':	Type = PRINTF_TYPE_i ;	i ++ ;	LoopEnd = 1 ;	break ;
				case 'o':	Type = PRINTF_TYPE_o ;	i ++ ;	LoopEnd = 1 ;	break ;
				case 'u':	Type = PRINTF_TYPE_u ;	i ++ ;	LoopEnd = 1 ;	break ;
				case 'x':	Type = PRINTF_TYPE_x ;	i ++ ;	LoopEnd = 1 ;	break ;
				case 'X':	Type = PRINTF_TYPE_X ;	i ++ ;	LoopEnd = 1 ;	break ;
				case 'e':	Type = PRINTF_TYPE_e ;	i ++ ;	LoopEnd = 1 ;	break ;
				case 'E':	Type = PRINTF_TYPE_E ;	i ++ ;	LoopEnd = 1 ;	break ;
				case 'f':	Type = PRINTF_TYPE_f ;	i ++ ;	LoopEnd = 1 ;	break ;
				case 'g':	Type = PRINTF_TYPE_g ;	i ++ ;	LoopEnd = 1 ;	break ;
				case 'G':	Type = PRINTF_TYPE_G ;	i ++ ;	LoopEnd = 1 ;	break ;
				case 'a':	Type = PRINTF_TYPE_a ;	i ++ ;	LoopEnd = 1 ;	break ;
				case 'A':	Type = PRINTF_TYPE_A ;	i ++ ;	LoopEnd = 1 ;	break ;
				case 'n':	Type = PRINTF_TYPE_n ;	i ++ ;	LoopEnd = 1 ;	break ;
				case 'p':	Type = PRINTF_TYPE_p ;	i ++ ;	LoopEnd = 1 ;	break ;
				case 's':	Type = PRINTF_TYPE_s ;	i ++ ;	LoopEnd = 1 ;	break ;
				case 'S':	Type = PRINTF_TYPE_S ;	i ++ ;	LoopEnd = 1 ;	break ;
				case 'Z':	Type = PRINTF_TYPE_Z ;	i ++ ;	LoopEnd = 1 ;	break ;
				}
			}while( LoopEnd == 0 ) ;

			// 型指定が無い場合はエラーなのでなにも処理しない
			if( Type != -1 )
			{
				switch( Type )
				{
				case PRINTF_TYPE_c :
				case PRINTF_TYPE_C :
					{
						DWORD ParamC = 0 ;
						int UseCharCodeFormat ;

						if( Type == PRINTF_TYPE_c )
						{
							UseCharCodeFormat = IsWChar ? WCharCharCodeFormat : CharCharCodeFormat ;
						}
						else
						{
							UseCharCodeFormat = IsWChar ? CharCharCodeFormat : WCharCharCodeFormat ;
						}

						switch( GetCharCodeFormatUnitSize_inline( UseCharCodeFormat ) )
						{
						case 1 :
							ParamC = va_arg( Arg, BYTE ) ;
							break ;

						case 2 :
							ParamC = va_arg( Arg, WORD ) ;
							break ;

						case 4 :
							ParamC = va_arg( Arg, DWORD ) ;
							break ;
						}

						if( UseCharCodeFormat != CharCodeFormat )
						{
							ParamC = ConvCharCode( ParamC, UseCharCodeFormat, CharCodeFormat ) ;
						}

						WriteBytes = CL_vsnprintf_help_c(
							ParamC,
							Flag_Zero, Flag_Minus,
							Width,
							( char * )&( ( BYTE * )Buffer )[ DestSize ],
							BufferBytes - DestSize,
							CharCodeFormat
						) ;
						DestSize += WriteBytes ;
						if( BufferBytes - DestSize <= NulWriteBytes )
						{
							goto END ;
						}
					}
					break ;

				case PRINTF_TYPE_d :
				case PRINTF_TYPE_i :
					{
						LONGLONG ParamI ;
						int NumberMinus ;

						switch( Prefix )
						{
						default :
						case PRINTF_SIZE_PREFIX_l	:
						case PRINTF_SIZE_PREFIX_I	:
						case PRINTF_SIZE_PREFIX_I32	:
							ParamI = va_arg( Arg, int ) ;
							break ;

						case PRINTF_SIZE_PREFIX_h	:
							ParamI = ( short )va_arg( Arg, int ) ;
							break ;

						case PRINTF_SIZE_PREFIX_ll	:
						case PRINTF_SIZE_PREFIX_I64	:
							ParamI = va_arg( Arg, LONGLONG ) ;
							break ;
						}

						NumberMinus = 0 ;
						if( ParamI < 0 )
						{
							NumberMinus = 1 ;
							ParamI = -ParamI ;
						}

						WriteBytes = CL_vsnprintf_help_itoa(
							( ULONGLONG )ParamI,
							NumberMinus,
							1,		// Signed
							10,		// Decimal
							Flag_Sharp, Flag_Zero, Flag_Minus, Flag_Plus, Flag_Space,
							Width, Precision,
							0,		// Big
							( char * )&( ( BYTE * )Buffer )[ DestSize ],
							BufferBytes - DestSize,
							CharCodeFormat
						) ;
						DestSize += WriteBytes ;
						if( BufferBytes - DestSize <= NulWriteBytes )
						{
							goto END ;
						}
					}
					break ;

				case PRINTF_TYPE_u :
				case PRINTF_TYPE_o :
				case PRINTF_TYPE_x :
				case PRINTF_TYPE_X :
					{
						ULONGLONG ParamU ;
						DWORD Decimal = 10 ;

						switch( Type )
						{
						case PRINTF_TYPE_u :
							Decimal = 10 ;
							break ;

						case PRINTF_TYPE_o :
							Decimal = 8 ;
							break ;

						case PRINTF_TYPE_x :
						case PRINTF_TYPE_X :
							Decimal = 16 ;
							break ;
						}

						switch( Prefix )
						{
						default :
						case PRINTF_SIZE_PREFIX_l	:
						case PRINTF_SIZE_PREFIX_I32	:
							ParamU = va_arg( Arg, DWORD ) ;
							break ;

						case PRINTF_SIZE_PREFIX_I	:
							if( sizeof( void * ) > 4 )
							{
								ParamU = va_arg( Arg, ULONGLONG ) ;
							}
							else
							{
								ParamU = va_arg( Arg, DWORD ) ;
							}
							break ;

						case PRINTF_SIZE_PREFIX_h	:
							ParamU = ( WORD )va_arg( Arg, DWORD ) ;
							break ;

						case PRINTF_SIZE_PREFIX_ll	:
						case PRINTF_SIZE_PREFIX_I64	:
							ParamU = va_arg( Arg, ULONGLONG ) ;
							break ;
						}

						WriteBytes = CL_vsnprintf_help_itoa(
							ParamU,
							0,		// NumberMinus
							0,		// Signed
							Decimal,
							Flag_Sharp, Flag_Zero, Flag_Minus, Flag_Plus, Flag_Space,
							Width, Precision,
							Type == PRINTF_TYPE_X,	// Big
							( char * )&( ( BYTE * )Buffer )[ DestSize ],
							BufferBytes - DestSize,
							CharCodeFormat
						) ;
						DestSize += WriteBytes ;
						if( BufferBytes - DestSize <= NulWriteBytes )
						{
							goto END ;
						}
					}
					break ;

				case PRINTF_TYPE_e :
				case PRINTF_TYPE_E :
				case PRINTF_TYPE_f :
				case PRINTF_TYPE_g :
				case PRINTF_TYPE_G :
				case PRINTF_TYPE_a :
				case PRINTF_TYPE_A :
					{
						double ParamF ;

						ParamF = va_arg( Arg, double ) ;

						WriteBytes = CL_vsnprintf_help_ftoa(
							ParamF,
							Type,
							Flag_Sharp, Flag_Zero, Flag_Minus, Flag_Plus, Flag_Space,
							Width, Precision,
							( char * )&( ( BYTE * )Buffer )[ DestSize ],
							BufferBytes - DestSize,
							CharCodeFormat
						) ;
						DestSize += WriteBytes ;
						if( BufferBytes - DestSize <= NulWriteBytes )
						{
							goto END ;
						}
					}
					break ;

				case PRINTF_TYPE_n :
					// 非対応
					break ;

				case PRINTF_TYPE_p :
					{
						void *ParamP ;

						// 精度は強制変更
						ParamP = va_arg( Arg, void * ) ;
						if( sizeof( void * ) > 4 )
						{
							Precision = 16 ;
						}
						else
						{
							Precision = 8 ;
						}

						WriteBytes = CL_vsnprintf_help_itoa(
							( ULONGLONG )ParamP,
							0,		// NumberMinus
							0,		// Signed
							16,
							Flag_Sharp, Flag_Zero, Flag_Minus, Flag_Plus, Flag_Space,
							Width, Precision,
							1,		// Big
							( char * )&( ( BYTE * )Buffer )[ DestSize ],
							BufferBytes - DestSize,
							CharCodeFormat
						) ;
						DestSize += WriteBytes ;
						if( BufferBytes - DestSize <= NulWriteBytes )
						{
							goto END ;
						}
					}
					break ;

				case PRINTF_TYPE_s :
				case PRINTF_TYPE_S :
					{
						char *ParamP ;
						char ParamPBuffer[ 512 ] ;
						char *ParamPTempBuffer = NULL ;
						char *UseParamPBuffer ;
						int UseCharCodeFormat ;

						ParamP = va_arg( Arg, char * ) ;

						if( ParamP != NULL )
						{
							if( Type == PRINTF_TYPE_s )
							{
								UseCharCodeFormat = IsWChar ? WCharCharCodeFormat : CharCharCodeFormat ;
							}
							else
							{
								UseCharCodeFormat = IsWChar ? CharCharCodeFormat : WCharCharCodeFormat ;
							}

							if( UseCharCodeFormat != CharCodeFormat )
							{
								int DataSize ;

								DataSize = ( ( int )CL_strlen( UseCharCodeFormat, ParamP ) + 1 ) * GetCharCodeFormatUnitSize( UseCharCodeFormat ) ;
								if( ( size_t )DataSize > sizeof( ParamPBuffer ) )
								{
									ParamPTempBuffer = ( char * )DXALLOC( DataSize ) ;
									if( ParamPTempBuffer == NULL )
									{
										UseParamPBuffer = NULL ;
									}
									else
									{
										UseParamPBuffer = ParamPTempBuffer ;
									}
								}
								else
								{
									UseParamPBuffer = ParamPBuffer ;
								}

								if( UseParamPBuffer != NULL )
								{
									ConvString( ParamP, -1, UseCharCodeFormat, UseParamPBuffer, DataSize, CharCodeFormat ) ;
									ParamP = UseParamPBuffer ;
								}
								else
								{
									ParamP = NULL ;
								}
							}
						}

						if( ParamP != NULL )
						{
							WriteBytes = CL_vsnprintf_help_s(
								ParamP,
								Flag_Zero, Flag_Minus,
								Width, Precision,
								( char * )&( ( BYTE * )Buffer )[ DestSize ],
								BufferBytes - DestSize,
								CharCodeFormat
							) ;

							if( ParamPTempBuffer != NULL )
							{
								DXFREE( ParamPTempBuffer ) ;
								ParamPTempBuffer = NULL ;
							}

							DestSize += WriteBytes ;
							if( BufferBytes - DestSize <= NulWriteBytes )
							{
								goto END ;
							}
						}
					}
					break ;

				case PRINTF_TYPE_Z :
					// 非対応
					break ;
				}
			}
		}
	}

END:

	// 終端文字をセット
	/* DestSize += */ PutCharCode_BufferBytes_inline( 0, CharCodeFormat, ( char * )&( ( BYTE * )Buffer )[ DestSize ], BufferBytes - DestSize ) ;

	if( TempBuffer != NULL )
	{
		DXFREE( TempBuffer ) ;
		TempBuffer = NULL ;
	}

	return ( int )DestSize / GetCharCodeFormatUnitSize_inline( CharCodeFormat ) ;
}

extern int CL_sprintf( int CharCodeFormat, int IsWChar, int CharCharCodeFormat, int WCharCharCodeFormat, char *Buffer, const char *FormatString, ... )
{
	int Result ;
	va_list VaList ;

	va_start( VaList, FormatString ) ;

	Result = CL_vsprintf( CharCodeFormat, IsWChar, CharCharCodeFormat, WCharCharCodeFormat, Buffer, FormatString, VaList ) ;

	va_end( VaList ) ;

	return Result ;
}

extern int CL_snprintf( int CharCodeFormat, int IsWChar, int CharCharCodeFormat, int WCharCharCodeFormat, char *Buffer, size_t BufferSize, const char *FormatString, ... )
{
	int Result ;
	va_list VaList ;

	va_start( VaList, FormatString ) ;

	Result = CL_vsnprintf( CharCodeFormat, IsWChar, CharCharCodeFormat, WCharCharCodeFormat, Buffer, BufferSize, FormatString, VaList ) ;

	va_end( VaList ) ;

	return Result ;
}

extern char *CL_itoa( int CharCodeFormat, int Value, char *Buffer, int Radix )
{
	int i ;
	BYTE Number[ 512 ] ;
	int NumberNum ;
	int DestSize ;

	DestSize = 0 ;

	// 数値が 0 の場合は 0 のみセット
	if( Value == 0 )
	{
		DestSize += PutCharCode_inline( '0', CharCodeFormat, ( char * )&( ( BYTE * )Buffer )[ DestSize ] ) ;
	}
	else
	{
		// 数値がマイナスの場合は - 記号を追加した上で値をプラスにする
		if( Value < 0 )
		{
			DestSize += PutCharCode_inline( '-', CharCodeFormat, ( char * )&( ( BYTE * )Buffer )[ DestSize ] ) ;
			Value = -Value ;
		}

		// 各桁の数値に変換
		for( NumberNum = 0 ; Value != 0 ; NumberNum ++, Value /= Radix )
		{
			Number[ NumberNum ] = ( BYTE )( Value % Radix ) ;
		}

		// 数値を文字列化
		for( i = NumberNum - 1 ; i >= 0 ; i -- )
		{
			DestSize += PutCharCode_inline( Number[ i ] <= 9 ? '0' + Number[ i ] : 'a' + Number[ i ] - 10, CharCodeFormat, ( char * )&( ( BYTE * )Buffer )[ DestSize ] ) ;
		}
	}

	// 終端文字をセット
	DestSize += PutCharCode_inline( 0, CharCodeFormat, ( char * )&( ( BYTE * )Buffer )[ DestSize ] ) ;
	
	return Buffer ;
}

extern char *CL_itoa_s( int CharCodeFormat, int Value, char *Buffer, size_t BufferBytes, int Radix )
{
	int i ;
	BYTE Number[ 512 ] ;
	int NumberNum ;
	size_t DestSize ;
	size_t NulWriteBytes ;
	size_t WriteBytes ;

	DestSize = 0 ;

	NulWriteBytes = PutCharCode_BufferBytes_inline( 0x00, CharCodeFormat, ( char * )&( ( BYTE * )Buffer )[ DestSize ], BufferBytes - DestSize ) ;
	if( NulWriteBytes == 0 )
	{
		return 0 ;
	}

	// 数値が 0 の場合は 0 のみセット
	if( Value == 0 )
	{
		WriteBytes = PutCharCode_BufferBytes_inline( '0', CharCodeFormat, ( char * )&( ( BYTE * )Buffer )[ DestSize ], BufferBytes - DestSize ) ;
		if( WriteBytes == 0 || BufferBytes - DestSize < WriteBytes + NulWriteBytes )
		{
			goto END ;
		}
		DestSize += WriteBytes ;
		if( BufferBytes - DestSize <= NulWriteBytes )
		{
			goto END ;
		}
	}
	else
	{
		// 数値がマイナスの場合は - 記号を追加した上で値をプラスにする
		if( Value < 0 )
		{
			WriteBytes = PutCharCode_BufferBytes_inline( '-', CharCodeFormat, ( char * )&( ( BYTE * )Buffer )[ DestSize ], BufferBytes - DestSize ) ;
			if( WriteBytes == 0 || BufferBytes - DestSize < WriteBytes + NulWriteBytes )
			{
				goto END ;
			}
			DestSize += WriteBytes ;
			if( BufferBytes - DestSize <= NulWriteBytes )
			{
				goto END ;
			}
			Value = -Value ;
		}

		// 各桁の数値に変換
		for( NumberNum = 0 ; Value != 0 ; NumberNum ++, Value /= Radix )
		{
			Number[ NumberNum ] = ( BYTE )( Value % Radix ) ;
		}

		// 数値を文字列化
		for( i = NumberNum - 1 ; i >= 0 ; i -- )
		{
			WriteBytes = PutCharCode_BufferBytes_inline( Number[ i ] <= 9 ? '0' + Number[ i ] : 'a' + Number[ i ] - 10, CharCodeFormat, ( char * )&( ( BYTE * )Buffer )[ DestSize ], BufferBytes - DestSize ) ;
			if( WriteBytes == 0 || BufferBytes - DestSize < WriteBytes + NulWriteBytes )
			{
				goto END ;
			}
			DestSize += WriteBytes ;
			if( BufferBytes - DestSize <= NulWriteBytes )
			{
				goto END ;
			}
		}
	}

END :

	// 終端文字をセット
	DestSize += PutCharCode_BufferBytes_inline( 0, CharCodeFormat, ( char * )&( ( BYTE * )Buffer )[ DestSize ], BufferBytes - DestSize ) ;
	
	return Buffer ;
}

extern int CL_atoi( int CharCodeFormat, const char *Str )
{
	int i ;
	int AddNum ;
	int NumCharNum ;
	int Number[ 256 ] ;
	int Total = 0 ;
	int Minus ;
	DWORD BaseBuffer[ 1024 ] ;
	DWORD *TempBuffer = NULL ;
	DWORD *FCode ;
	int StringSize ;

	// NULL のアドレスが渡されたら 0 を返す
	if( Str == NULL )
	{
		return 0 ;
	}

	// １文字４バイト形式に変換する
	StringSize = StringToCharCodeString_inline( Str, CharCodeFormat, NULL ) ;
	if( StringSize + sizeof( DWORD ) * 16 > sizeof( BaseBuffer ) )
	{
		TempBuffer = ( DWORD * )DXALLOC( StringSize + sizeof( DWORD ) * 16 ) ;
		if( TempBuffer == NULL )
		{
			return -1 ;
		}

		FCode = TempBuffer ;
	}
	else
	{
		FCode = BaseBuffer ;
	}
	_MEMSET( FCode, 0, StringSize + sizeof( DWORD ) * 16 ) ;
	StringToCharCodeString_inline( Str, CharCodeFormat, FCode ) ;

	// 数字を探す
	while( *FCode != '\0' )
	{
		if( ( *FCode >= '0' && *FCode <= '9' ) || *FCode == '-' )
		{
			break ;
		}
		FCode ++ ;
	}

	// いきなり終端文字の場合はエラーなので 0 を返す
	if( *FCode == '\0' )
	{
		goto END ;
	}

	// 最初の文字がマイナス記号の場合はマイナスフラグを立てる
	if( *FCode == '-' )
	{
		Minus = 1 ;
		FCode ++ ;
	}
	else
	{
		Minus = 0 ;
	}
	
	// 数字を取得
	for( NumCharNum = 0 ; *FCode != '\0' && ( *FCode >= '0' && *FCode <= '9' ) ; NumCharNum ++, FCode ++ )
	{
		Number[ NumCharNum ] = *FCode - '0' ;
	}
	
	// 数値に変換
	AddNum = 1 ;
	Total  = 0 ;
	for( i = NumCharNum - 1 ; i >= 0 ; i --, AddNum *= 10 )
	{
		Total += Number[ i ] * AddNum ;
	}

	// マイナスフラグが立っていたらマイナス値にする
	if( Minus == 1 )
	{
		Total = -Total ;
	}

END :

	if( TempBuffer != NULL )
	{
		DXFREE( TempBuffer ) ;
		TempBuffer = NULL ;
	}

	// 数値を返す
	return Total ;
}

extern double CL_atof( int CharCodeFormat, const char *Str )
{
	int MinusFlag ;
	int DotFlag ;
	int IndexFlag ;
	int IndexMinusFlag ;
	int i ;
	DWORD IntNumStr[ 256 ] ;
	DWORD FloatNumStr[ 256 ] ;
	DWORD IndexNumStr[ 256 ] ;
	int IntNumberNum ;
	int FloatNumberNum ;
	int IndexNumberNum ;
	double IntNum ;
	double FloatNum ;
	double IndexNum ;
	double AddNum ;
	LONGLONG int64Num, int64Count ;
	DWORD BaseBuffer[ 1024 ] ;
	DWORD *TempBuffer = NULL ;
	DWORD *FCode ;
	int StringSize ;
	double Result = 0.0 ;

	MinusFlag      = 0 ;
	DotFlag        = 0 ;
	IndexFlag      = 0 ;
	IndexMinusFlag = 0 ;

	// NULL のアドレスが渡されたら 0.0 を返す
	if( Str == NULL )
	{
		return 0.0 ;
	}

	// １文字４バイト形式に変換する
	StringSize = StringToCharCodeString_inline( Str, CharCodeFormat, NULL ) ;
	if( StringSize + sizeof( DWORD ) * 16 > sizeof( BaseBuffer ) )
	{
		TempBuffer = ( DWORD * )DXALLOC( StringSize + sizeof( DWORD ) * 16 ) ;
		if( TempBuffer == NULL )
		{
			return -1.0 ;
		}

		FCode = TempBuffer ;
	}
	else
	{
		FCode = BaseBuffer ;
	}
	_MEMSET( FCode, 0, StringSize + sizeof( DWORD ) * 16 ) ;
	StringToCharCodeString_inline( Str, CharCodeFormat, FCode ) ;

	// いきなり終端文字の場合は 0.0 を返す
	if( *FCode == '\0' )
	{
		goto END ;
	}

	// マイナス記号が最初にあったらマイナスフラグを立てる
	if( *FCode == '-' )
	{
		FCode ++ ;

		// マイナス記号の直後に終端文字の場合は 0.0 を返す
		if( *FCode == '\0' )
		{
			goto END ;
		}
		MinusFlag = 1 ;
	}

	IntNumberNum   = 0 ;
	FloatNumberNum = 0 ;
	IndexNumberNum = 0 ;
	for( ; *FCode != '\0' ; FCode ++ )
	{
		// 小数点の処理
		if( *FCode == '.' )
		{
			// 既に小数点があったらエラーなので 0.0 を返す
			if( DotFlag )
			{
				goto END ;
			}

			// 点フラグを立てる
			DotFlag = 1 ;
		}
		else
		// 指数値指定文字判定
		if( *FCode == 'e' || *FCode == 'E' )
		{
			// 既に指数値指定があったらエラーなので 0.0 を返す
			if( IndexFlag )
			{
				goto END ;
			}

			// 指数値指定フラグを立てる
			IndexFlag = 1 ;
		}
		else
		// 指数値指定用のマイナス記号又はプラス記号の処理
		if( *FCode == '-' || *FCode == '+' )
		{
			// 指数値指定が無くいきなり - や + があった場合や、
			// e や E の文字の直後に - や + が無かった場合はエラーなので 0.0 を返す
			if( IndexFlag == 0 || IndexNumberNum != 0 )
			{
				goto END ;
			}

			// マイナス記号の場合は指数値がマイナスのフラグを立てる
			if( *FCode == '-' )
			{
				IndexMinusFlag = 1 ;
			}
		}
		else
		// 数字の処理
		if( *FCode >= '0' && *FCode <= '9' )
		{
			// 指数値指定の後の場合は指数値
			if( IndexFlag )
			{
				// 256 文字以上指数値の文字列が続いたらエラーなので 0 を返す
				if( IndexNumberNum >= 255 )
				{
					goto END ;
				}
				IndexNumStr[ IndexNumberNum ] = *FCode - '0';
				IndexNumberNum ++ ;
			}
			else
			if( DotFlag )
			{
				// 256 文字以上小数点以下の値が続いたらエラーなので 0 を返す
				if( FloatNumberNum >= 255 )
				{
					goto END ;
				}
				FloatNumStr[ FloatNumberNum ] = *FCode - '0';
				FloatNumberNum ++ ;
			}
			else
			{
				// 256 文字以上整数値の文字列が続いたらエラーなので 0 を返す
				if( IntNumberNum >= 255 )
				{
					goto END ;
				}
				IntNumStr[ IntNumberNum ] = *FCode - '0';
				IntNumberNum ++ ;
			}
		}
		else
		// 上記以外の文字だった場合はエラーなので 0.0 を返す
		{
			goto END ;
		}
	}

	// 整数部も小数部も文字列の長さ０だった場合はエラーなので 0.0 を返す
	if( IntNumberNum == 0 && FloatNumberNum == 0 )
	{
		goto END ;
	}

	// 整数値を算出
	AddNum = 1.0 ;
	IntNum = 0 ;
	for( i = IntNumberNum - 1 ; i >= 0; i --, AddNum *= 10.0 )
	{
		if( IntNumStr[ i ] != 0 )
		{
			IntNum += IntNumStr[ i ] * AddNum ;
		}
	}
	if( MinusFlag )
	{
		IntNum = -IntNum ;
	}

	// 小数値を算出
	AddNum   = 0.1 ;
	FloatNum = 0 ;
	for( i = 0 ; i < FloatNumberNum ; i ++, AddNum /= 10.0 )
	{
		if( FloatNumStr[ i ] != 0 )
		{
			FloatNum += FloatNumStr[i] * AddNum ;
		}
	}
	if( MinusFlag )
	{
		FloatNum = -FloatNum ;
	}

	// 指数値文字列を数値化
	int64Count = 1 ;
	int64Num   = 0 ;
	for( i = IndexNumberNum - 1; i >= 0; i --, int64Count *= 10 )
	{
		int64Num += IndexNumStr[ i ] * int64Count ;
	}
	if( IndexMinusFlag )
	{
		int64Num = -int64Num ;
	}

	// 指数値に従って乗算値を準備
	IndexNum = 1.0 ;
	if( int64Num != 0 )
	{
		if( int64Num < 0 )
		{
			int64Num = -int64Num ;
			for( i = 0 ; i < int64Num ; i ++ )
			{
				IndexNum /= 10.0 ;
			}
		}
		else
		{
			for( i = 0 ; i < int64Num ; i++ )
			{
				IndexNum *= 10.0 ;
			}
		}
	}

	// 戻り値を算出
	Result = ( IntNum + FloatNum ) * IndexNum ;

END :

	if( TempBuffer != NULL )
	{
		DXFREE( TempBuffer ) ;
		TempBuffer = NULL ;
	}

	return Result ;
}

static int CL_vsscanf_skipspace( DWORD *String )
{
	int AddSize = 0 ;

	for( AddSize = 0 ;
		String[ AddSize ] == ' '  || 
		String[ AddSize ] == '\n' ||
		String[ AddSize ] == '\r' ||
		String[ AddSize ] == '\t' ;
		AddSize ++ ){}

	return AddSize ;
}

extern int CL_vsscanf( int CharCodeFormat, int IsWChar, int CharCharCodeFormat, int WCharCharCodeFormat, const char *String, const char *FormatString, va_list Arg )
{
	DWORD c ;
	DWORD Number[ 256 ] ;
	DWORD VStr[ 1024 ] ;
	int ReadNum = 0 ;
	int Width ;
	int i, j, k ;
	int num, num2 ;
	int SkipFlag ;
	int VStrRFlag ;
	int I64Flag ;
	int lFlag ;
	int hFlag ;
	int Eof ;
	int MinusFlag ;
	int UIntFlag ;
	LONGLONG int64Num ;
	LONGLONG int64Count ;
	int UseCharNum ;
	BYTE *pStr ;
	int CharBytes ;
	BYTE TempBuffer[ 64 ] ;

	DWORD FormatStringBaseBuffer[ 1024 ] ;
	DWORD *FormatStringTempBuffer = NULL ;
	int FormatStringSize ;
	DWORD *FCode ;

	DWORD StringBaseBuffer[ 1024 ] ;
	DWORD *StringTempBuffer = NULL ;
	int StringSize ;
	DWORD *SCode ;

	// １文字４バイト形式に変換する
	{
		StringSize = StringToCharCodeString_inline( String, CharCodeFormat, NULL ) ;
		if( StringSize + sizeof( DWORD ) * 16 > sizeof( StringBaseBuffer ) )
		{
			StringTempBuffer = ( DWORD * )DXALLOC( StringSize + sizeof( DWORD ) * 16 ) ;
			if( StringTempBuffer == NULL )
			{
				return 0 ;
			}

			SCode = StringTempBuffer ;
		}
		else
		{
			SCode = StringBaseBuffer ;
		}
		_MEMSET( SCode, 0, StringSize + sizeof( DWORD ) * 16 ) ;
		StringToCharCodeString_inline( String, CharCodeFormat, SCode ) ;


		FormatStringSize = StringToCharCodeString_inline( FormatString, CharCodeFormat, NULL ) ;
		if( FormatStringSize + sizeof( DWORD ) * 16 > sizeof( FormatStringBaseBuffer ) )
		{
			FormatStringTempBuffer = ( DWORD * )DXALLOC( FormatStringSize + sizeof( DWORD ) * 16 ) ;
			if( FormatStringTempBuffer == NULL )
			{
				if( StringTempBuffer != NULL )
				{
					DXFREE( StringTempBuffer ) ;
					StringTempBuffer = NULL ;
				}
				return 0 ;
			}

			FCode = FormatStringTempBuffer ;
		}
		else
		{
			FCode = FormatStringBaseBuffer ;
		}
		_MEMSET( FCode, 0, FormatStringSize + sizeof( DWORD ) * 16 ) ;
		StringToCharCodeString_inline( FormatString, CharCodeFormat, FCode ) ;
	}

	// いきなり終端文字の場合は 0 を返す
	if( *FCode == '\0' )
	{
		goto END ;
	}

	ReadNum = 0;
	Eof = FALSE ;
	while( *FCode != '\0' )
	{
		// % かどうかで処理を分岐
		if( FCode[ 0 ] == '%' && FCode[ 1 ] != '%' )
		{
			Width    = -1;
			I64Flag  = FALSE;
			lFlag    = FALSE;
			hFlag    = FALSE;
			UIntFlag = FALSE;
			SkipFlag = FALSE;

			FCode ++ ;
			if( *FCode == '\0' )
			{
				break;
			}

			if( *FCode == '*' )
			{
				SkipFlag = TRUE;
				FCode ++ ;
				if( *FCode == '\0' )
				{
					break;
				}
			}

			if( *FCode >= '0' && *FCode <= '9' )
			{
				Width = ( int )CL_vsnprintf_help_getnumber( FCode, &UseCharNum ) ;
				FCode += UseCharNum ;
				if( *FCode == '\0' )
				{
					break;
				}
				if( Width == 0 )
				{
					break;
				}
			}

			switch( *FCode )
			{
			case 'l':
			case 'L':
				lFlag = TRUE ;
				FCode++ ;
				break ;

			case 'h':
			case 'H':
				hFlag = TRUE ;
				FCode++ ;
				break ;

			case 'I':
				if( FCode[ 1 ] == '6' && FCode[ 2 ] == '4' )
				{
					I64Flag = TRUE;
					FCode += 3;
				}
				break;
			}
			if( *FCode == '\0' )
			{
				break;
			}

			if( *FCode == '[' )
			{
				if( lFlag || hFlag || I64Flag )
				{
					break;
				}

				FCode ++ ;
				VStrRFlag = FALSE ;
				if( *FCode == '^' )
				{
					VStrRFlag = TRUE;
					FCode++;
				}
				j = 0;
				_MEMSET( VStr, 0, sizeof( VStr ) ) ;

				c = '[';
				while( *FCode != '\0' && *FCode != ']' )
				{
					if( *FCode == '-' && c != '[' && FCode[ 1 ] != '\0' && FCode[ 1 ] != ']' )
					{
						num  = ( int )( BYTE )c ;
						num2 = ( int )( BYTE )FCode[ 1 ] ;
						if( num2 < num )
						{
							k    = num2 ;
							num2 = num ;
							num  = k ;
						}
						for( k = num; k <= num2; k++ )
						{
							if( c != ( DWORD )k )
							{
								*( ( BYTE *)&VStr[ j ] ) = ( BYTE )k ;
								j++ ;
							}
						}
						FCode += 2 ;
						c = '[' ;
					}
					else
					{
						VStr[ j ] = *FCode ;
						c = *FCode ;
						j ++ ;
						FCode ++ ;
					}
				}
				if( *FCode == '\0' )
				{
					break;
				}

				FCode ++ ;
				pStr = NULL;
				if( SkipFlag == FALSE )
				{
					pStr = va_arg( Arg, BYTE * ) ;
				}

				SCode += CL_vsscanf_skipspace( SCode ) ;
				if( *SCode == '\0' )
				{
					Eof = TRUE ;
					break;
				}

				i = 0;
				for(;;)
				{
					if( *SCode == 0 )
					{
						Eof = TRUE ;
						break ;
					}
					c = *SCode ;
					SCode ++ ;
					j = 0 ;
					while( VStr[ j ] != '\0' )
					{
						if( VStr[ j ] == c )
						{
							break ;
						}
						j ++ ;
					}

					if( ( VStrRFlag == TRUE  && VStr[ j ] != '\0' ) ||
						( VStrRFlag == FALSE && VStr[ j ] == '\0' ) )
					{
						break ;
					}

					if( pStr )
					{
						pStr += PutCharCode_inline( c, CharCodeFormat, ( char * )pStr ) ;
					}
					i ++ ;
					if( Width != 0 && Width == i )
					{
						break ;
					}
				}

				if( pStr )
				{
					pStr += PutCharCode_inline( '\0', CharCodeFormat, ( char * )pStr ) ;
				}

				if( Eof == FALSE && Width != i )
				{
					SCode -- ;
				}
			}
			else
			if( *FCode == 'd' || *FCode == 'D' || *FCode == 'u' || *FCode == 'U' )
			{
				SCode += CL_vsscanf_skipspace( SCode ) ;
				if( *SCode == '\0' )
				{
					Eof = TRUE ;
					break;
				}

				FCode ++ ;
				UIntFlag  = *FCode == 'u' || *FCode == 'U' ;
				MinusFlag = FALSE ;
				c = *SCode ;
				SCode ++ ;
				if( c == '-' )
				{
					MinusFlag = TRUE;
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 )
						{
							break;
						}
					}
					if( *SCode == '\0' )
					{
						Eof = TRUE ;
						break ;
					}
					c = *SCode ;
					SCode ++ ;
				}
STR_10INT :
				for( i = 0; i < 255 && ( Width == -1 || Width != i ) && c >= '0' && c <= '9'; i ++ )
				{
					Number[ i ] = c - '0';
					if( *SCode == '\0' )
					{
						i ++ ;
						Eof = TRUE ;
						break ;
					}
					c = *SCode ;
					SCode ++ ;
				}
				if( Eof == FALSE )
				{
					SCode -- ;
				}
				num = i ;
				if( num == 0 )
				{
					break ;
				}
				int64Count = 1 ;
				int64Num   = 0 ;
				for( i = num - 1 ; i >= 0 ; i --, int64Count *= 10 )
				{
					if( UIntFlag )
					{
						int64Num = ( LONGLONG )( ( ULONGLONG )int64Num + ( ULONGLONG )Number[ i ] * ( ULONGLONG )int64Count ) ;
					}
					else
					{
						int64Num += ( LONGLONG )Number[i] * int64Count ;
					}
				}
				if( MinusFlag )
				{
					int64Num = -int64Num ;
				}
				if( SkipFlag == FALSE )
				{
					if( I64Flag )
					{
						if( UIntFlag )
						{
							*va_arg( Arg, ULONGLONG * ) = ( ULONGLONG )int64Num ;
						}
						else
						{
							*va_arg( Arg, LONGLONG * ) = ( LONGLONG )int64Num ;
						}
					}
					else if( hFlag )
					{
						if( UIntFlag )
						{
							*va_arg( Arg, WORD * ) = ( WORD )int64Num ;
						}
						else
						{
							*va_arg( Arg, short * ) = ( short )int64Num ;
						}
					}
					else
					{
						if( UIntFlag )
						{
							*va_arg( Arg, DWORD * ) = ( DWORD )int64Num ;
						}
						else
						{
							*va_arg( Arg, int * ) = ( int )int64Num ;
						}
					}
				}
			}
			else
			if( *FCode == 'x' || *FCode == 'X' )
			{
				SCode += CL_vsscanf_skipspace( SCode ) ;
				if( *SCode == '\0' )
				{
					Eof = TRUE ;
					break;
				}

				FCode ++ ;
				MinusFlag = FALSE ;
				c = *SCode ;
				SCode ++ ;
				if( c == '-' )
				{
					MinusFlag = TRUE ;
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 )
						{
							break ;
						}
					}
					if( *SCode == '\0' )
					{
						Eof = TRUE ;
						break ;
					}
					c = *SCode ;
					SCode ++ ;
				}
STR_16INT :
				i = 0;
				for(;;)
				{
					if( i >= 255 )
					{
						break ;
					}

					if( Width != -1 && Width == i )
					{
						break ;
					}
					else
					if( c >= '0' && c <= '9' )
					{
						Number[ i ] = c - '0' ;
					}
					else
					if( c >= 'a' && c <= 'f' )
					{
						Number[ i ] = c - 'a' + 10 ;
					}
					else
					if( c >= 'A' && c <= 'F' )
					{
						Number[ i ] = c - 'A' + 10 ;
					}
					else
					{
						break;
					}

					i ++ ;
					if( *SCode == '\0' )
					{
						Eof = TRUE ;
						break ;
					}
					c = *SCode ;
					SCode ++ ;
				}
				if( Eof == FALSE )
				{
					SCode -- ;
				}
				num = i ;
				if( num == 0 )
				{
					break ;
				}
				int64Count = 1 ;
				int64Num   = 0 ;
				for( i = num - 1; i >= 0; i --, int64Count *= 16 )
				{
					int64Num = ( LONGLONG )( ( ULONGLONG )int64Num + ( ULONGLONG )Number[ i ] * ( ULONGLONG )int64Count ) ;
				}
				if( MinusFlag )
				{
					int64Num = -int64Num ;
				}
				if( SkipFlag == FALSE )
				{
					if( I64Flag )
					{
						*va_arg( Arg, ULONGLONG * ) = ( ULONGLONG )int64Num ;
					}
					else
					if( hFlag )
					{
						*va_arg( Arg, WORD * ) = ( WORD )int64Num ;
					}
					else
					{
						*va_arg( Arg, DWORD * ) = ( DWORD )int64Num ;
					}
				}
			}
			else
			if( *FCode == 'o' || *FCode == 'O' )
			{
				SCode += CL_vsscanf_skipspace( SCode ) ;
				if( *SCode == '\0' )
				{
					Eof = TRUE ;
					break;
				}

				FCode ++ ;
				MinusFlag = FALSE;
				c = *SCode ;
				SCode ++ ;
				if( c == '-' )
				{
					MinusFlag = TRUE ;
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 )
						{
							break ;
						}
					}
					if( *SCode == '\0' )
					{
						Eof = TRUE ;
						break ;
					}
					c = *SCode ;
					SCode ++ ;
				}
STR_8INT:
				for( i = 0; i < 255 && ( Width == -1 || Width != i ) && c >= '0' && c <= '7'; i ++ )
				{
					Number[ i ] = c - '0';
					if( *SCode == '\0' )
					{
						Eof = TRUE ;
						break ;
					}
					c = *SCode ;
					SCode ++ ;
				}
				if( Eof == FALSE )
				{
					SCode -- ;
				}
				num = i ;
				if( num == 0 )
				{
					break ;
				}
				int64Count = 1 ;
				int64Num   = 0 ;
				for( i = num - 1; i >= 0; i --, int64Count *= 8 )
				{
					int64Num = ( LONGLONG )( ( ULONGLONG )int64Num + ( ULONGLONG )Number[ i ] * ( ULONGLONG )int64Count ) ;
				}
				if( MinusFlag )
				{
					int64Num = -int64Num ;
				}
				if( SkipFlag == FALSE )
				{
					if( I64Flag )
					{
						*va_arg( Arg, ULONGLONG * ) = ( ULONGLONG )int64Num ;
					}
					else
					if( hFlag )
					{
						*va_arg( Arg, WORD * ) = ( WORD )int64Num ;
					}
					else
					{
						*va_arg( Arg, DWORD * ) = ( DWORD )int64Num ;
					}
				}
			}
			else
			if( *FCode == 'i' || *FCode == 'I' )
			{
				SCode += CL_vsscanf_skipspace( SCode ) ;
				if( *SCode == '\0' )
				{
					Eof = TRUE ;
					break;
				}

				FCode ++ ;
				MinusFlag = FALSE;
				c = *SCode ;
				SCode ++ ;
				if( c == '-' )
				{
					MinusFlag = TRUE;
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 )
						{
							break ;
						}
					}
					if( *SCode == '\0' )
					{
						Eof = TRUE ;
						break ;
					}
					c = *SCode ;
					SCode ++ ;
				}
				if( c == '0' )
				{
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 )
						{
							break ;
						}
					}
					if( *SCode == '\0' )
					{
						Eof = TRUE ;
						break ;
					}
					c = *SCode ;
					SCode ++ ;
					if( c == 'x' )
					{
						if( Width != -1 )
						{
							Width -- ;
							if( Width == 0 )
							{
								break ;
							}
						}
						if( *SCode == '\0' )
						{
							Eof = TRUE ;
							break ;
						}
						c = *SCode ;
						SCode ++ ;
						goto STR_16INT ;
					}
					else
					{
						goto STR_8INT ;
					}
				}
				else
				{
					goto STR_10INT ;
				}
			}
			else
			if( *FCode == 'c' || *FCode == 'C' )
			{
				int UseUnitSize ;
				int UseCharCodeFormat ;

				if( *FCode == 'c' )
				{
					UseCharCodeFormat = IsWChar ? WCharCharCodeFormat : CharCharCodeFormat ;
				}
				else
				{
					UseCharCodeFormat = IsWChar ? CharCharCodeFormat : WCharCharCodeFormat ;
				}
				UseUnitSize = GetCharCodeFormatUnitSize( UseCharCodeFormat ) ;

				FCode ++ ;
				if( Width == -1 )
				{
					Width = 1 ;
				}
				pStr = NULL ;
				if( SkipFlag == FALSE )
				{
					pStr = va_arg( Arg, BYTE * ) ;
				}

				for( i = 0; i < Width; i += CharBytes / UseUnitSize )
				{
					if( *SCode == '\0' )
					{
						Eof = TRUE ;
						break ;
					}
					c = *SCode ;
					SCode ++ ;

					if( UseCharCodeFormat != CharCodeFormat )
					{
						c = ConvCharCode( c, CharCodeFormat, UseCharCodeFormat ) ;
					}

					if( pStr )
					{
						CharBytes = PutCharCode_inline( c, UseCharCodeFormat, ( char * )pStr ) ;
						pStr += CharBytes ;
					}
					else
					{
						CharBytes = PutCharCode_inline( c, UseCharCodeFormat, ( char * )TempBuffer ) ;
					}
				}
			}
			else
			if( *FCode == 's' || *FCode == 'S' )
			{
				int UseUnitSize ;
				int UseCharCodeFormat ;

				if( *FCode == 's' )
				{
					UseCharCodeFormat = IsWChar ? WCharCharCodeFormat : CharCharCodeFormat ;
				}
				else
				{
					UseCharCodeFormat = IsWChar ? CharCharCodeFormat : WCharCharCodeFormat ;
				}
				UseUnitSize = GetCharCodeFormatUnitSize( UseCharCodeFormat ) ;

				FCode ++ ;
				SCode += CL_vsscanf_skipspace( SCode ) ;
				if( *SCode == '\0' )
				{
					Eof = TRUE ;
					break;
				}

				pStr = NULL ;
				if( SkipFlag == FALSE )
				{
					pStr = va_arg( Arg, BYTE * ) ;
				}

				i = 0 ;
				for(;;)
				{
					if( *SCode == '\0' )
					{
						Eof = TRUE ;
						break ;
					}
					c = *SCode ;
					SCode ++ ;

					if( c == ' ' || c == '\n' || c == '\r' || c == '\t' )
					{
						SCode -- ;
						break;
					}

					if( UseCharCodeFormat != CharCodeFormat )
					{
						c = ConvCharCode( c, CharCodeFormat, UseCharCodeFormat ) ;
					}

					if( pStr )
					{
						CharBytes = PutCharCode_inline( c, UseCharCodeFormat, ( char * )pStr ) ;
						pStr += CharBytes ;
					}
					else
					{
						CharBytes = PutCharCode_inline( c, UseCharCodeFormat, ( char * )TempBuffer ) ;
					}

					i += CharBytes / UseUnitSize ;
					if( Width != -1 && i >= Width )
					{
						break ;
					}
				}

				if( pStr )
				{
					pStr += PutCharCode_inline( '\0', UseCharCodeFormat, ( char * )pStr ) ;
				}
			}
			else
			if( *FCode == 'f' || *FCode == 'F' || *FCode == 'g' || *FCode == 'G' || *FCode == 'e' || *FCode == 'E' )
			{
				int num3 ;
				DWORD Number2[ 256 ] ;
				DWORD Number3[ 256 ] ;
				double doubleNum ;
				double doubleNum2 ;
				double doubleNum3 ;
				double doubleCount ;
				int DotFlag ;
				int IndexFlag ;
				int MinusFlag2 ;

				SCode += CL_vsscanf_skipspace( SCode ) ;
				if( *SCode == '\0' )
				{
					Eof = TRUE ;
					break;
				}

				FCode ++ ;
				MinusFlag  = FALSE ;
				DotFlag    = FALSE ;
				IndexFlag  = FALSE ;
				MinusFlag2 = FALSE ;
				if( *SCode == '\0' )
				{
					Eof = TRUE ;
					break ;
				}
				c = *SCode ;
				SCode ++ ;
				if( c == '-' )
				{
					MinusFlag = TRUE ;
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 )
						{
							break ;
						}
					}
					if( *SCode == '\0' )
					{
						Eof = TRUE ;
						break ;
					}
					c = *SCode ;
					SCode ++ ;
				}

				i = 0 ;	// 自然数
				j = 0 ;	// 小数
				k = 0 ;	// 指数
				for(;;)
				{
					if( c == '.' )
					{
						if( DotFlag )
						{
							break ;
						}
						DotFlag = TRUE ;
					}
					else
					if( c == 'e' || c == 'E' )
					{
						if( IndexFlag )
						{
							break ;
						}
						IndexFlag = TRUE ;
					}
					else
					if( c == '-' || c == '+' )
					{
						if( IndexFlag == FALSE || k != 0 )
						{
							break ;
						}
						if( c == '-' )
						{
							MinusFlag2 = TRUE ;
						}
					}
					else
					if( c >= '0' && c <= '9' )
					{
						if( IndexFlag )
						{
							if( k >= 255 )
							{
								break ;
							}
							Number3[ k ] = c - '0' ;
							k ++ ;
						}
						else
						if( DotFlag )
						{
							if( j >= 255 )
							{
								break ;
							}
							Number2[ j ] = c - '0' ;
							j ++ ;
						}
						else
						{
							if( i >= 255 )
							{
								break ;
							}
							Number[ i ] = c - '0';
							i ++ ;
						}
					}
					else
					{
						break;
					}

					if( *SCode == '\0' )
					{
						Eof = TRUE ;
						break ;
					}
					c = *SCode ;
					SCode ++ ;
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 )
						{
							break ;
						}
					}
				}
				if( Eof == FALSE )
				{
					SCode -- ;
				}
				if( i == 0 && j == 0 )
				{
					break ;
				}
				num  = i ;
				num2 = j ;
				num3 = k ;
				if( num == 0 && num2 == 0 )
				{
					break ;
				}

				doubleCount = 1.0 ;
				doubleNum   = 0 ;
				for( i = num - 1; i >= 0; i --, doubleCount *= 10.0 )
				{
					if( Number[ i ] != 0 )
					{
						doubleNum += Number[ i ] * doubleCount ;
					}
				}
				if( MinusFlag )
				{
					doubleNum = -doubleNum ;
				}

				doubleCount = 0.1 ;
				doubleNum2  = 0 ;
				for( i = 0; i < num2; i ++, doubleCount /= 10.0 )
				{
					if( Number2[ i ] != 0 )
					{
						doubleNum2 += Number2[ i ] * doubleCount ;
					}
				}
				if( MinusFlag )
				{
					doubleNum2 = -doubleNum2 ;
				}

				int64Count = 1 ;
				int64Num   = 0 ;
				for( i = num3 - 1; i >= 0; i --, int64Count *= 10 )
				{
					int64Num += Number3[ i ] * int64Count ;
				}
				if( MinusFlag2 )
				{
					int64Num = -int64Num ;
				}

				doubleNum3 = 1.0 ;
				if( int64Num != 0 )
				{
					if( int64Num < 0 )
					{
						int64Num = -int64Num ;
						for( i = 0; i < int64Num; i++ )
						{
							doubleNum3 /= 10.0 ;
						}
					}
					else
					{
						for( i = 0; i < int64Num; i++ )
						{
							doubleNum3 *= 10.0 ;
						}
					}
				}

				doubleNum = ( doubleNum + doubleNum2 ) * doubleNum3 ;

				if( SkipFlag == FALSE )
				{
					if( lFlag )
					{
						*va_arg( Arg, double * ) =          doubleNum ;
					}
					else
					{
						*va_arg( Arg, float *  ) = ( float )doubleNum ;
					}
				}
			}
			if( SkipFlag == FALSE )
			{
				ReadNum ++ ;
			}
		}
		else
		{
			if( *FCode == ' ' || *FCode == '\n' || *FCode == '\r' || *FCode == '\t' )
			{
				while( *FCode != '\0' && ( *FCode == ' ' || *FCode == '\n' || *FCode == '\r' || *FCode == '\t' ) )
				{
					FCode ++ ;
				}
				SCode += CL_vsscanf_skipspace( SCode ) ;
				if( *SCode == '\0' )
				{
					Eof = TRUE ;
					break;
				}
			}
			else
			{
				DWORD str[ 256 ] ;

				for( num = 0; *FCode != '\0' && *FCode != ' ' && *FCode != '\n' && *FCode != '\r' && *FCode != '\t' && *FCode != '%'; num++, FCode ++ )
				{
					str[ num ] = *FCode ;
				}
				str[ num ] = '\0' ;

				for( i = 0; i < num; i ++ )
				{
					if( *SCode == '\0' )
					{
						Eof = TRUE ;
						break ;
					}
					c = *SCode ;
					SCode ++ ;
					if( str[ i ] != c )
					{
						break ;
					}
				}
			}
		}
	}

END :

	if( FormatStringTempBuffer != NULL )
	{
		DXFREE( FormatStringTempBuffer ) ;
		FormatStringTempBuffer = NULL ;
	}

	if( StringTempBuffer != NULL )
	{
		DXFREE( StringTempBuffer ) ;
		StringTempBuffer = NULL ;
	}

	return ReadNum;
}

extern int CL_sscanf( int CharCodeFormat, int IsWChar, int CharCharCodeFormat, int WCharCharCodeFormat, const char *String, const char *FormatString, ... )
{
	int Result ;
	va_list VaList ;

	va_start( VaList, FormatString ) ;

	Result = CL_vsscanf( CharCodeFormat, IsWChar, CharCharCodeFormat, WCharCharCodeFormat, String, FormatString, VaList ) ;

	va_end( VaList ) ;

	return Result ;
}

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE
