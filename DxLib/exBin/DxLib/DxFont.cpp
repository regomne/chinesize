// ----------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		ＤｉｒｅｃｔＤｒａｗ制御プログラム
// 
// 				Ver 3.20c
// 
// ----------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

#include "DxCompileConfig.h"

#ifndef DX_NON_FONT

// インクルード----------------------------------------------------------------
#include "DxLib.h"
#include "DxStatic.h"
#include "DxFont.h"
#include "DxMask.h"
#include "DxMath.h"
#include "DxMemory.h"
#include "DxChar.h"
#include "DxBaseFunc.h"
#include "DxSystem.h"
#include "DxUseCLib.h"
#include "DxBaseImage.h"
#include "DxArchive_.h"
#include "DxGraphics.h"
#include "DxASyncLoad.h"
#include "DxLog.h"

#ifdef __WINDOWS__
#include "Windows/DxFontWin.h"
#endif // __WINDOWS__

#ifdef __ANDROID__
#include "Android/DxFontAndroid.h"
#endif // __ANDROID__

#ifdef __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE
		#include "iOS/DxFontiOS.h"
    #endif // TARGET_OS_IPHONE
#endif // __APPLE__





#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義------------------------------------------------------------------

#define FONTEDGE_PATTERN_NUM	(4)			// 用意する文字の縁のパターンの数


#define FONTHANDLE_TCHAR_TO_WCHAR_TEMPSTRINGLENGTH				(512)

#define FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_BEGIN( str, err_ret )		\
	int CharCodeFormat ;\
	wchar_t StringBuffer[ FONTHANDLE_TCHAR_TO_WCHAR_TEMPSTRINGLENGTH ] ;\
	wchar_t *AllocStringBuffer = NULL ;\
	wchar_t *UseStringBuffer ;\
	size_t StrLength ;\
	size_t wchar_StrLength ;\
	\
	DEFAULT_FONT_HANDLE_SETUP\
	\
	CharCodeFormat = GetFontHandleCharCodeFormat( FontHandle ) ;\
	if( CharCodeFormat < 0 )\
	{\
		return err_ret ;\
	}\
	\
	StrLength = CL_strlen( CharCodeFormat, str ) ;\
	if( StrLength > FONTHANDLE_TCHAR_TO_WCHAR_TEMPSTRINGLENGTH - 8 )\
	{\
		AllocStringBuffer = ( wchar_t * )DXALLOC( sizeof( wchar_t ) * ( StrLength + 16 ) ) ;\
		if( AllocStringBuffer == NULL )\
		{\
			return err_ret ;\
		}\
		UseStringBuffer = AllocStringBuffer ;\
	}\
	else\
	{\
		UseStringBuffer = StringBuffer ;\
	}\
	\
	wchar_StrLength = ConvString( ( const char * )str, -1, CharCodeFormat, ( char * )UseStringBuffer, sizeof( wchar_t ) * ( StrLength + 16 ), WCHAR_T_CHARCODEFORMAT ) / GetCharCodeFormatUnitSize( WCHAR_T_CHARCODEFORMAT ) ;\
	if( wchar_StrLength > 0 )\
	{\
		wchar_StrLength -- ;\
	}


#define FONTHANDLE_TCHAR_WITH_STRLENGTH_TO_WCHAR_T_STRING_BEGIN( str, strlength, err_ret )		\
	int CharCodeFormat ;\
	wchar_t StringBuffer[ FONTHANDLE_TCHAR_TO_WCHAR_TEMPSTRINGLENGTH ] ;\
	wchar_t *AllocStringBuffer = NULL ;\
	wchar_t *UseStringBuffer ;\
	size_t wchar_StrLength ;\
	\
	DEFAULT_FONT_HANDLE_SETUP\
	\
	CharCodeFormat = GetFontHandleCharCodeFormat( FontHandle ) ;\
	if( CharCodeFormat < 0 )\
	{\
		return err_ret ;\
	}\
	\
	if( strlength > FONTHANDLE_TCHAR_TO_WCHAR_TEMPSTRINGLENGTH - 8 )\
	{\
		AllocStringBuffer = ( wchar_t * )DXALLOC( sizeof( wchar_t ) * ( strlength + 16 ) ) ;\
		if( AllocStringBuffer == NULL )\
		{\
			return err_ret ;\
		}\
		UseStringBuffer = AllocStringBuffer ;\
	}\
	else\
	{\
		UseStringBuffer = StringBuffer ;\
	}\
	\
	wchar_StrLength = ConvString( ( const char * )str, ( int )strlength, CharCodeFormat, ( char * )UseStringBuffer, sizeof( wchar_t ) * ( strlength + 16 ), WCHAR_T_CHARCODEFORMAT ) / GetCharCodeFormatUnitSize( WCHAR_T_CHARCODEFORMAT ) ;\
	if( wchar_StrLength > 0 )\
	{\
		wchar_StrLength -- ;\
	}


#define FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_END		\
	if( AllocStringBuffer != NULL )					\
	{												\
		DXFREE( AllocStringBuffer ) ;				\
		AllocStringBuffer = NULL ;					\
	}


#define FONTHANDLE_WCHAR_T_WITH_STRLENGTH_TO_WCHAR_T_STRING_BEGIN( str, strlength, err_ret )		\
	wchar_t StringBuffer[ FONTHANDLE_TCHAR_TO_WCHAR_TEMPSTRINGLENGTH ] ;\
	wchar_t *AllocStringBuffer = NULL ;\
	wchar_t *UseStringBuffer ;\
	\
	DEFAULT_FONT_HANDLE_SETUP\
	\
	if( strlength > FONTHANDLE_TCHAR_TO_WCHAR_TEMPSTRINGLENGTH - 8 )\
	{\
		AllocStringBuffer = ( wchar_t * )DXALLOC( sizeof( wchar_t ) * ( strlength + 16 ) ) ;\
		if( AllocStringBuffer == NULL )\
		{\
			return err_ret ;\
		}\
		UseStringBuffer = AllocStringBuffer ;\
	}\
	else\
	{\
		UseStringBuffer = StringBuffer ;\
	}\
	\
	CL_strncpy( WCHAR_T_CHARCODEFORMAT, ( char * )UseStringBuffer, ( const char * )str, ( int )( strlength ) ) ;\
	UseStringBuffer[ strlength ] = L'\0' ;


#define FONTHANDLE_WCHAR_T_TO_WCHAR_T_STRING_END		\
	if( AllocStringBuffer != NULL )					\
	{												\
		DXFREE( AllocStringBuffer ) ;				\
		AllocStringBuffer = NULL ;					\
	}


#define FONTHANDLE_TCHAR_WITH_STRLEN_TO_WCHAR_T_STRING_BEGIN( str, str_len, err_ret )\
	const char *UseString ;\
	char *TempAllocBuffer = NULL ;\
	char TempBuffer[ 512 ] ;\
	\
	DEFAULT_FONT_HANDLE_SETUP\
	\
	if( str_len < 0 )\
	{\
		UseString = str ;\
	}\
	else\
	{\
		int CharCodeFormat = GetFontHandleCharCodeFormat( FontHandle ) ;\
		if( CharCodeFormat < 0 )\
		{\
			return err_ret ;\
		}\
		int CharUnitSize = GetCharCodeFormatUnitSize( CharCodeFormat ) ;\
		int StrLength = ( int )CL_strlen( CharCodeFormat, str ) ;\
		\
		if( str_len > StrLength )\
		{\
			str_len = StrLength ;\
		}\
		\
		if( sizeof( TempBuffer ) <= ( size_t )( CharUnitSize * str_len + 16 ) )\
		{\
			TempAllocBuffer = ( char * )DXALLOC( CharUnitSize * str_len + 16 ) ;\
			if( TempAllocBuffer == NULL )\
			{\
				return err_ret ;\
			}\
			UseString = TempAllocBuffer ;\
		}\
		else\
		{\
			UseString = TempBuffer ;\
		}\
		\
		_MEMCPY( ( char * )UseString, str, CharUnitSize * str_len ) ;\
		_MEMSET( ( char * )&UseString[ CharUnitSize * str_len ], 0, CharUnitSize * 8 ) ;\
	}\
	\
	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_BEGIN( UseString, -1 )


#define FONTHANDLE_TCHAR_WITH_STRLEN_TO_WCHAR_T_STRING_END\
	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_END\
	\
	if( TempAllocBuffer != NULL )\
	{\
		DXFREE( TempAllocBuffer ) ;\
		TempAllocBuffer = NULL ;\
	}



#ifdef UNICODE
	#define TCHAR_CHARCODEFORMAT_SETUP( err )		\
		int CharCodeFormat ;\
		\
		DEFAULT_FONT_HANDLE_SETUP\
		\
		CharCodeFormat = WCHAR_T_CHARCODEFORMAT ;
#else
	#define TCHAR_CHARCODEFORMAT_SETUP( err )		\
		int CharCodeFormat ;\
		\
		DEFAULT_FONT_HANDLE_SETUP\
		\
		CharCodeFormat = GetFontHandleCharCodeFormat( FontHandle ) ;\
		if( CharCodeFormat < 0 )\
		{\
			return err ;\
		}
#endif

#define TCHAR_FONTHANDLE_FORMATSTRING_SETUP( err )			\
	va_list VaList ;\
	TCHAR String[ 2048 ] ;\
	\
	TCHAR_CHARCODEFORMAT_SETUP( err )\
	\
	va_start( VaList, FormatString ) ;\
	\
	CL_vsnprintf( CharCodeFormat, _TISWCHAR, CHAR_CHARCODEFORMAT, WCHAR_T_CHARCODEFORMAT, ( char * )String, sizeof( String ) / sizeof( TCHAR ), ( const char * )FormatString, VaList ) ;\
	\
	va_end( VaList ) ;


// 構造体型宣言----------------------------------------------------------------

// データ宣言------------------------------------------------------------------

static const char *GetFontCacheChar_ErrorMessage = "\xc6\x30\xad\x30\xb9\x30\xc8\x30\xad\x30\xe3\x30\xc3\x30\xb7\x30\xe5\x30\x6e\x30\xfd\x8f\xa0\x52\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"テキストキャッシュの追加に失敗しました\n" @*/ ;

// 文字の縁のパターンテーブル
static unsigned char _FontEdgePattern[FONTEDGE_PATTERN_NUM][FONTEDGE_PATTERN_NUM*2+1][FONTEDGE_PATTERN_NUM*2+1] =
{
	// 1
	{
		{0,1,0},
		{1,1,1},
		{0,1,0},
	},
	
	// 2
	{
		{0,1,1,1,0},
		{1,1,1,1,1},
		{1,1,1,1,1},
		{1,1,1,1,1},
		{0,1,1,1,0},
	},
	
	// 3
	{
		{0,0,0,1,0,0,0},
		{0,1,1,1,1,1,0},
		{0,1,1,1,1,1,0},
		{1,1,1,1,1,1,1},
		{0,1,1,1,1,1,0},
		{0,1,1,1,1,1,0},
		{0,0,0,1,0,0,0},
	},
	
	// 4
	{
		{0,0,1,1,1,1,1,0,0},
		{0,1,1,1,1,1,1,1,0},
		{1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1},
		{0,1,1,1,1,1,1,1,0},
		{0,0,1,1,1,1,1,0,0},
	},
} ;

int CharCodeFormatTable[ DX_CHARSET_NUM ] =
{
	DX_CHARCODEFORMAT_SHIFTJIS,			// DX_CHARSET_DEFAULT
	DX_CHARCODEFORMAT_SHIFTJIS,			// DX_CHARSET_SHFTJIS
	DX_CHARCODEFORMAT_UHC,				// DX_CHARSET_HANGEUL
	DX_CHARCODEFORMAT_BIG5,				// DX_CHARSET_BIG5
	DX_CHARCODEFORMAT_GB2312,			// DX_CHARSET_GB2312
	DX_CHARCODEFORMAT_WINDOWS_1252,		// DX_CHARSET_WINDOWS_1252
	DX_CHARCODEFORMAT_ISO_IEC_8859_15,	// DX_CHARSET_ISO_IEC_8859_15
	DX_CHARCODEFORMAT_UTF8,				// DX_CHARSET_UTF8
} ;

static BYTE DefaultFontDataFileImage[] =
{
	0x46,0x4e,0x54,0x46,0x00,0x00,0x00,0x00,0x4f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x78,0x01,0x00,0x00,0x37,0x00,0x00,0x00,
	0xfe,0x2d,0xff,0x33,0xff,0x20,0x00,0xb4,0x30,0xb7,0x30,0xc3,0x30,0xaf,0x30,0x00,
	0xfe,0x7c,0x07,0x00,0x10,0x00,0x12,0xfe,0x08,0x07,0x18,0x00,0x01,0x00,0xff,0xff,
	0xfe,0xcc,0x01,0x4a,0xfe,0x38,0x00,0x20,0xfe,0x28,0x09,0x08,0xfe,0x88,0x1f,
} ;

static int DefaultFontImageConvert ;
static BYTE DefaultFontImage[] =
{
	0x39,0x72,0x31,0x31,0x31,0x50,0x35,0x31,0x31,0x31,0x75,0x39,0x31,0x31,0x70,0x59,
	0x55,0xac,0xbb,0x51,0x31,0xc1,0x73,0x31,0x34,0xb3,0x31,0x31,0x31,0x45,0x31,0x31,
	0x31,0x35,0x34,0xbd,0x31,0x37,0x31,0x70,0x21,0x31,0x39,0x34,0xc0,0x39,0x37,0x50,
	0x28,0x71,0x34,0xc2,0xc0,0x31,0x31,0x70,0xc2,0xc2,0xb9,0x21,0x50,0xc1,0xac,0x38,
	0xb3,0x35,0x31,0x36,0x31,0x6f,0x38,0x38,0x52,0x33,0x43,0x35,0x51,0xc2,0x37,0x33,
	0x34,0x38,0xb7,0x41,0x40,0x72,0x39,0x79,0xa5,0x73,0x43,0x35,0x41,0x71,0x39,0x33,
	0x31,0x71,0x31,0x32,0x6d,0x6e,0x51,0x31,0x53,0x35,0x39,0x22,0x5b,0x53,0x35,0x39,
	0x40,0xab,0x51,0x50,0x73,0x74,0x70,0x51,0x31,0xbd,0x43,0x25,0x59,0x35,0x39,0x50,
	0x21,0x71,0x6f,0x36,0x33,0x71,0x50,0x35,0x39,0x43,0x43,0x55,0x41,0x35,0x35,0x50,
	0x22,0x71,0x70,0x32,0x31,0xc1,0x71,0x32,0xb5,0x71,0x21,0x42,0xc0,0x4d,0x34,0xb3,
	0x5a,0x43,0x41,0x79,0x35,0x51,0x35,0x31,0xa2,0xab,0x31,0xc2,0x53,0x72,0x70,0x21,
	0x59,0x45,0x7b,0x35,0x73,0x32,0x51,0x33,0x31,0x48,0xb7,0x31,0x30,0xa1,0x50,0x50,
	0x21,0x31,0xc1,0x36,0x33,0xc2,0x70,0x52,0x32,0xc2,0xb7,0x31,0x31,0x41,0x70,0x54,
	0x31,0x68,0xb7,0x31,0x70,0xc1,0xa1,0x31,0x40,0xb2,0xa3,0x4f,0x7d,0xc1,0x21,0x32,
	0xb4,0x71,0x71,0xa3,0x63,0xc1,0xbf,0x51,0x31,0x3a,0x2c,0xb3,0x6d,0x50,0x40,0x35,
	0x38,0xa7,0x39,0x51,0x61,0x73,0x35,0x43,0x59,0x42,0xa3,0x31,0x7d,0x73,0x41,0x79,
	0x55,0x42,0x33,0x36,0x33,0x39,0x43,0x42,0x32,0x35,0x7b,0x35,0x75,0x31,0x52,0x41,
	0x59,0x35,0x3b,0x31,0x51,0x32,0x51,0x73,0x39,0x55,0x71,0x52,0x43,0x72,0x51,0x30,
	0x22,0x71,0x6d,0xb8,0x33,0x39,0x32,0x45,0x40,0xac,0x51,0x50,0xc1,0x41,0x42,0x21,
	0x52,0x31,0x44,0x32,0xc0,0x45,0x34,0xb3,0x60,0xab,0x31,0xc2,0xc0,0x3d,0x34,0xab,
	0x7d,0x53,0x3a,0x3d,0x75,0x41,0x52,0x29,0xa2,0xac,0x31,0x2f,0x6d,0x2d,0x40,0x38,
	0x54,0x78,0xbc,0xb8,0x69,0x39,0x32,0x43,0x32,0x35,0x7c,0x39,0x75,0xc1,0x71,0x68,
	0x21,0x79,0x39,0x31,0x51,0x31,0x51,0x31,0x31,0xa7,0x51,0x51,0xaf,0x2d,0x3f,0x50,
	0x23,0x71,0x6d,0x52,0xc0,0x31,0x31,0x71,0x31,0x31,0x44,0xbd,0x59,0x38,0x72,0x50,
	0x21,0x71,0x6d,0x42,0xc0,0x45,0x3c,0xc2,0x22,0x31,0x6d,0x52,0xc0,0x35,0x34,0xa5,
	0x60,0xab,0x55,0x37,0x3b,0xc1,0x71,0x32,0xc2,0xab,0x31,0x63,0x33,0x31,0x34,0x50,
	0x21,0x71,0x70,0xbd,0x35,0x31,0x71,0x41,0x34,0x25,0x39,0xb3,0x41,0x4d,0x35,0x39,
	0x79,0x41,0x74,0xbf,0x69,0x39,0x3f,0x31,0x40,0xb7,0x32,0x3a,0x35,0x53,0x35,0x39,
	0xa2,0xab,0x31,0x50,0xc0,0x39,0xc2,0xa3,0x49,0x35,0x5b,0x3a,0x33,0x39,0x51,0x76,
	0x35,0x58,0xb7,0x74,0xc2,0x31,0xa3,0x43,0x59,0x71,0x3c,0xbd,0x51,0x38,0xc2,0x30,
	0x21,0x31,0x6f,0x36,0xc0,0x35,0x34,0xb8,0x53,0x71,0x74,0xbd,0x42,0x38,0x71,0x21,
	0x39,0x45,0x74,0xbd,0xa3,0x38,0x72,0x33,0x39,0x43,0x43,0x25,0x41,0x39,0x35,0x50,
	0x21,0x72,0x70,0xb4,0x33,0x21,0x3f,0x50,0x23,0x31,0x70,0xbd,0x31,0x40,0xa4,0x41,
	0x60,0xa5,0x34,0xbd,0x31,0x38,0x7b,0x36,0x31,0x72,0x31,0x22,0xc0,0x35,0x7c,0xa3,
	0x59,0x45,0x34,0xbd,0xb3,0x38,0xa3,0x23,0x55,0x53,0x41,0x39,0x51,0x53,0x70,0x5e,
	0x31,0xbd,0x41,0x32,0x35,0x53,0x51,0x79,0x60,0xab,0x31,0x50,0x33,0x72,0x31,0x75,
	0x39,0x41,0x51,0x21,0x31,0xbf,0x3f,0x50,0x74,0xaa,0xb7,0x43,0x30,0x32,0x51,0x30,
	0xa6,0x75,0x39,0xb4,0xc0,0x3d,0x7b,0x32,0x70,0xb0,0xa3,0x31,0x6b,0x72,0x70,0x38,
	0x40,0x78,0xbd,0x31,0x69,0x72,0x3f,0x37,0x39,0x48,0xbd,0x55,0x73,0x4d,0x44,0x41,
	0x59,0x43,0x43,0x3a,0x31,0x71,0x42,0x41,0x52,0x33,0x53,0x36,0x31,0x7a,0x42,0x79,
	0x7a,0xb7,0x3c,0xbd,0x31,0x69,0xa3,0x41,0x59,0x41,0x73,0x3a,0x35,0x71,0x55,0x79,
	0xab,0x46,0x7c,0xbd,0x71,0x38,0xad,0x7a,0x59,0x46,0x3c,0xbd,0x61,0x38,0xa5,0x50,
	0x21,0x71,0x70,0xc0,0xc0,0x31,0x78,0xa3,0x60,0xab,0x51,0x50,0x41,0xc1,0x75,0x34,
	0xbb,0x58,0xb7,0x31,0x40,0xa2,0x51,0x73,0x39,0x56,0x34,0xbd,0x41,0x38,0xc1,0x41,
	0x39,0x48,0xbe,0xc0,0x31,0xc2,0x35,0x31,0x7d,0x35,0x33,0x25,0x23,0x72,0x59,0x79,
	0xa2,0xab,0x51,0x70,0x31,0xc1,0x71,0x5a,0xb3,0x48,0xb7,0x61,0x40,0xc1,0x71,0x3a,
	0xc2,0xab,0x31,0x50,0x41,0xc1,0x71,0x32,0xbc,0xba,0xb7,0xa3,0x40,0x75,0x70,0x51,
	0x31,0xbf,0x3c,0xbd,0x41,0x38,0xc2,0x51,0x46,0xbb,0x3b,0x42,0x31,0x26,0x49,0x21,
	0x55,0x52,0x53,0x3a,0xc0,0x3d,0x5c,0xa3,0x79,0x55,0x34,0x3d,0xa5,0x53,0x3f,0x33,
	0x40,0x72,0xa6,0xb4,0xc1,0xc1,0x71,0x50,0x62,0xb7,0x3b,0x32,0x33,0x52,0x3f,0x50,
	0x2e,0x78,0xbb,0x32,0xc1,0x4d,0x70,0x30,0xa3,0x58,0xbe,0xbf,0x41,0xc2,0x70,0x71,
	0x33,0x31,0x39,0x31,0x71,0x39,0x61,0x73,0x39,0x31,0x39,0x39,0x33,0x72,0x35,0x41,
	0x51,0x41,0x31,0x71,0x35,0x31,0x39,0x50,0x24,0x71,0x6d,0xa3,0x61,0x35,0x31,0x33,
	0x31,0x36,0x3c,0xbd,0x69,0x38,0x75,0x31,0x31,0x78,0xb7,0xb3,0x40,0x31,0x31,0x35,
	0x38,0xa3,0x41,0x52,0x53,0x39,0x41,0x31,0x60,0xb3,0x3b,0x35,0x39,0xc1,0x71,0x32,
	0xb7,0x31,0x31,0x35,0x41,0x7a,0x35,0x35,0x31,0x45,0x44,0xbd,0x49,0x38,0x79,0x40,
	0x71,0x51,0x53,0x55,0x41,0x39,0x70,0x21,0xa2,0xba,0xbb,0x42,0xc1,0xc2,0x70,0x52,
	0x34,0xbb,0x43,0x55,0x41,0x35,0x31,0x21,0x79,0x35,0x31,0x3a,0x33,0x72,0x70,0x52,
	0x36,0xbd,0x3b,0x45,0x41,0x33,0x70,0x54,0x31,0xc2,0xb7,0x34,0xbe,0x32,0x39,0x41,
	0x59,0x21,0x71,0x36,0xc0,0x31,0x34,0xa3,0x60,0xab,0x31,0x4f,0x33,0x31,0x41,0x41,
	0x59,0x21,0x73,0x36,0xc0,0x31,0x34,0xb3,0x60,0xab,0x31,0x4f,0x31,0x31,0x31,0x41,
	0x59,0x62,0x74,0xbd,0x79,0x38,0x22,0x50,0x21,0x37,0x57,0x3a,0xc1,0xc1,0x71,0x31,
	0x40,0xab,0x51,0x50,0xc0,0xb7,0xc2,0xa3,0x31,0x31,0x71,0x31,0x75,0x39,0x51,0x31,
	0x31,0x31,0x51,0x72,0xc0,0x3d,0x2e,0xa3,0x72,0x31,0x32,0x39,0xbf,0x78,0x4d,0x71,
	0x32,0x31,0x74,0xbd,0x39,0x38,0x21,0x50,0x21,0x33,0xa1,0x55,0x7b,0x77,0x70,0x52,
	0x31,0xbc,0x71,0x31,0x61,0xc1,0x73,0x32,0xb4,0x43,0x6b,0x49,0x31,0x41,0x33,0x50,
	0x21,0x71,0x6d,0x71,0x31,0x39,0x31,0x50,0xa4,0x42,0x34,0xbd,0x31,0x38,0xc2,0x51,
	0x33,0x39,0x34,0xbd,0x31,0x38,0x22,0x33,0x53,0x35,0x79,0x31,0x51,0x35,0x46,0x50,
	0x22,0x3d,0xbb,0x38,0xc0,0x31,0x34,0xa5,0x36,0x41,0x31,0x71,0x39,0xc1,0x73,0x62,
	0x71,0x48,0xb7,0x41,0x40,0x6f,0x35,0x35,0x40,0xab,0x31,0x2f,0xbf,0x31,0x50,0x31,
	0x40,0xab,0x36,0x55,0x32,0x53,0x55,0x32,0x34,0x38,0xb7,0x71,0x50,0xc1,0x77,0x32,
	0xb7,0x78,0xb7,0x71,0x70,0x33,0x70,0x51,0x36,0xbf,0x74,0x7a,0x35,0xc1,0x79,0x3c,
	0xb3,0x51,0x31,0x51,0x75,0x53,0x55,0x25,0x79,0x52,0x34,0xbd,0x39,0x37,0xc2,0x51,
	0x42,0x41,0x35,0x51,0xa9,0x53,0x50,0x4d,0x59,0x52,0x74,0xbd,0x71,0x38,0x75,0x31,
	0x58,0x72,0x74,0xbd,0x31,0xaa,0xc2,0x61,0x72,0x31,0x31,
} ;

FONTSYSTEM FontSystem ;

// 関数プロトタイプ宣言--------------------------------------------------------

static int FontCacheStringAddToHandle(		FONTMANAGE *ManageData, const wchar_t *String, int StrLen = -1,
											DWORD *DrawStrBuffer = NULL, int *DrawCharNumP = NULL ) ;	// 文字キャッシュに新しい文字を加える

#ifndef DX_NON_GRAPHICS

static int DrawStringHardware(				int xi, int yi, float xf, float yf, int PosIntFlag,                                                                                        const wchar_t *String, size_t StringLength, unsigned int Color, FONTMANAGE *Font, unsigned int EdgeColor, int VerticalFlag ) ;
static int DrawExtendStringHardware(		int xi, int yi, float xf, float yf, int PosIntFlag, double ExRateX, double ExRateY,                                                        const wchar_t *String, size_t StringLength, unsigned int Color, FONTMANAGE *Font, unsigned int EdgeColor, int VerticalFlag ) ;
static int DrawRotaStringHardware(			int xi, int yi, float xf, float yf, int PosIntFlag, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, const wchar_t *String, size_t StringLength, unsigned int Color, FONTMANAGE *Font, unsigned int EdgeColor, int VerticalFlag ) ;
static int DrawModiStringHardware(			int x1i, int y1i, int x2i, int y2i, int x3i, int y3i, int x4i, int y4i, float x1f, float y1f, float x2f, float y2f, float x3f, float y3f, float x4f, float y4f, int PosIntFlag, const wchar_t *String, size_t StringLength, unsigned int Color, FONTMANAGE *Font, unsigned int EdgeColor, int VerticalFlag ) ;

static int SetupSoftwareStringImage(		FONTMANAGE *ManageData, const wchar_t *String, size_t StringLength, int Color, int EdgeColor, int VerticalFlag, MEMIMG **UseScreenImg, SIZE *DrawSize, RECT *DrawArea, RECT *DrawAreaBackup, DWORD *DrawBrightBackup ) ;
static int DrawStringSoftware(				int x, int y,                                                                                        const wchar_t *String, size_t StringLength, unsigned int Color, FONTMANAGE *Font, unsigned int EdgeColor, int VerticalFlag ) ;
static int DrawExtendStringSoftware(		int x, int y, double ExRateX, double ExRateY,                                                        const wchar_t *String, size_t StringLength, unsigned int Color, FONTMANAGE *Font, unsigned int EdgeColor, int VerticalFlag ) ;
static int DrawRotaStringSoftware(			int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, const wchar_t *String, size_t StringLength, unsigned int Color, FONTMANAGE *Font, unsigned int EdgeColor, int VerticalFlag ) ;
static int DrawModiStringSoftware(			int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4,                                      const wchar_t *String, size_t StringLength, unsigned int Color, FONTMANAGE *Font, unsigned int EdgeColor, int VerticalFlag ) ;

#endif // DX_NON_GRAPHICS

static int EnumFontNameBase(  wchar_t *NameBuffer, int NameBufferNum, int JapanOnlyFlag, int IsEx = FALSE, int CharSet = -1, const wchar_t *EnumFontName = NULL, int IsReturnValid = FALSE ) ;
static int EnumFontNameBaseT( TCHAR   *NameBuffer, int NameBufferNum, int JapanOnlyFlag, int IsEx = FALSE, int CharSet = -1, const TCHAR   *EnumFontName = NULL, int IsReturnValid = FALSE ) ;

// プログラムコード------------------------------------------------------------

__inline DWORD WCHAR_T_GetCharCode(	const wchar_t *CharCode, int CharCodeFormat, int *CharBytes )
{
	int UseSrcSize ;
	DWORD DestCode ;

	if( sizeof( wchar_t ) == 2 )
	{
		DWORD SrcCode1 ;
		DWORD SrcCode2 ;

		if( *( ( WORD * )CharCode ) == 0 )
		{
			*CharBytes = 2 ;
			return 0 ;
		}

		if( CharCodeFormat == DX_CHARCODEFORMAT_UTF16LE )
		{
			SrcCode1 = ( ( ( BYTE * )CharCode )[ 0 ] | ( ( ( BYTE * )CharCode )[ 1 ] << 8 ) ) ;
			if( ( SrcCode1 & 0xfc00 ) == 0xd800 )
			{
				UseSrcSize = 4 ;
				SrcCode2 = ( ( BYTE * )CharCode )[ 2 ] | ( ( ( BYTE * )CharCode )[ 3 ] << 8 ) ;
				DestCode = ( ( ( SrcCode1 & 0x3ff ) << 10 ) | ( SrcCode2 & 0x3ff ) ) + 0x10000 ;
			}
			else
			{
				UseSrcSize = 2 ;
				DestCode = SrcCode1 ;
			}
		}
		else
		{
			SrcCode1 = ( ( ( BYTE * )CharCode )[ 0 ] << 8 ) | ( ( BYTE * )CharCode )[ 1 ] ;
			if( ( SrcCode1 & 0xfc00 ) == 0xd800 )
			{
				UseSrcSize = 4 ;
				SrcCode2 = ( ( ( BYTE * )CharCode )[ 2 ] << 8 ) | ( ( BYTE * )CharCode )[ 3 ] ;
				DestCode = ( ( ( SrcCode1 & 0x3ff ) << 10 ) | ( SrcCode2 & 0x3ff ) ) + 0x10000 ;
			}
			else
			{
				UseSrcSize = 2 ;
				DestCode = SrcCode1 ;
			}
		}
	}
	else
	{
		if( *( ( DWORD * )CharCode ) == 0 )
		{
			*CharBytes = 4 ;
			return 0 ;
		}

		UseSrcSize = 4 ;
		if( CharCodeFormat == DX_CHARCODEFORMAT_UTF32LE )
		{
			DestCode = ( ( BYTE * )CharCode )[ 0 ] | ( ( ( BYTE * )CharCode )[ 1 ] << 8 ) | ( ( ( BYTE * )CharCode )[ 2 ] << 16 ) | ( ( ( BYTE * )CharCode )[ 3 ] << 24 ) ;
		}
		else
		{
			DestCode = ( ( ( BYTE * )CharCode )[ 0 ] << 24 ) | ( ( ( BYTE * )CharCode )[ 1 ] << 16 ) | ( ( ( BYTE * )CharCode )[ 2 ] << 8 ) | ( ( BYTE * )CharCode )[ 3 ] ;
		}
	}

	*CharBytes = UseSrcSize ;

	return DestCode ;
}

// フォント管理データの取得
__inline FONTMANAGE * GetFontManageDataToHandle_Inline( int FontHandle )
{
	FONTMANAGE * ManageData ;

	DEFAULT_FONT_HANDLE_SETUP

	if( FONTHCHK( FontHandle, ManageData ) )
		return NULL ;

	// 終了
	return ManageData ;
}
extern FONTMANAGE * GetFontManageDataToHandle( int FontHandle )
{
	return GetFontManageDataToHandle_Inline( FontHandle ) ;
}

#ifndef DX_NON_GRAPHICS

static int DrawStringHardware( int xi, int yi, float xf, float yf, int PosIntFlag, const wchar_t *String, size_t StringLength, unsigned int Color, FONTMANAGE *Font, unsigned int EdgeColor, int VerticalFlag )
{
	// フォントにテクスチャキャッシュが使用されている場合
	if( Font->TextureCacheFlag )
	{
		if( Font->TextureCacheLostFlag == TRUE )
		{
			RefreshFontDrawResourceToHandle( Font ) ;
		}
		FontCacheStringDrawToHandleST(
			TRUE,
			xi,
			yi,
			xf,
			yf,
			PosIntFlag, 
			FALSE,
			1.0,
			1.0,
			FALSE, 0.0f, 0.0f, 0.0,
			String,
			Color,
			NULL,
			&GSYS.DrawSetting.DrawArea,
			TRUE,
			Font,
			EdgeColor,
			( int )StringLength,
			VerticalFlag,
			NULL,
			NULL,
			NULL,
			0,
			NULL,
			0
		) ;
		return 0 ;
	}
	else
	{
	}

	// 終了
	return 0 ;
}

static int DrawExtendStringHardware( int xi, int yi, float xf, float yf, int PosIntFlag, double ExRateX, double ExRateY, const wchar_t *String, size_t StringLength, unsigned int Color, FONTMANAGE *Font, unsigned int EdgeColor, int VerticalFlag )
{
	// フォントにテクスチャキャッシュが使用されている場合
	if( Font->TextureCacheFlag )
	{
		// 描画先が３Ｄデバイスによる描画が出来ない場合はエラー
		RefreshFontDrawResourceToHandle( Font ) ;
		FontCacheStringDrawToHandleST(
			TRUE,
			xi,
			yi,
			xf,
			yf,
			PosIntFlag,
			TRUE,
			ExRateX,
			ExRateY,
			FALSE, 0.0f, 0.0f, 0.0,
			String,
			Color,
			NULL,
			&GSYS.DrawSetting.DrawArea,
			TRUE,
			Font,
			EdgeColor,
			( int )StringLength,
			VerticalFlag,
			NULL,
			NULL,
			NULL,
			0,
			NULL,
			0
		) ;
		return 0 ;
	}
	else
	{
	}

	// 終了
	return 0 ;
}

static int DrawRotaStringHardware( int xi, int yi, float xf, float yf, int PosIntFlag, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, const wchar_t *String, size_t StringLength, unsigned int Color, FONTMANAGE *Font, unsigned int EdgeColor, int VerticalFlag )
{
	// フォントにテクスチャキャッシュが使用されている場合
	if( Font->TextureCacheFlag )
	{
		// 描画先が３Ｄデバイスによる描画が出来ない場合はエラー
		RefreshFontDrawResourceToHandle( Font ) ;
		FontCacheStringDrawToHandleST(
			TRUE,
			xi,
			yi,
			xf,
			yf,
			PosIntFlag,
			TRUE,
			ExRateX,
			ExRateY,
			TRUE,
			( float )RotCenterX,
			( float )RotCenterY,
			RotAngle,
			String,
			Color,
			NULL,
			&GSYS.DrawSetting.DrawArea,
			TRUE,
			Font,
			EdgeColor,
			( int )StringLength,
			VerticalFlag,
			NULL,
			NULL,
			NULL,
			0,
			NULL,
			0
		) ;
		return 0 ;
	}
	else
	{
	}

	// 終了
	return 0 ;
}

static int DrawModiStringHardware( int x1i, int y1i, int x2i, int y2i, int x3i, int y3i, int x4i, int y4i, float x1f, float y1f, float x2f, float y2f, float x3f, float y3f, float x4f, float y4f, int PosIntFlag, const wchar_t *String, size_t StringLength, unsigned int Color, FONTMANAGE *Font, unsigned int EdgeColor, int VerticalFlag )
{
	// フォントにテクスチャキャッシュが使用されている場合
	if( Font->TextureCacheFlag )
	{
		int DrawWidth ;
		int DrawHeight ;
//		int Result ;
		int *UseTempScreenHandle ;
		int TempScreenHeight ;
		int TempScreenWidth ;
		int DrawTempScreenHandle[ 2 ] = { -1, -1 } ;
		SCREENDRAWSETTINGINFO ScreenDrawSettingInfo ;

		// 描画先が３Ｄデバイスによる描画が出来ない場合はエラー
		RefreshFontDrawResourceToHandle( Font ) ;

		if( VerticalFlag )
		{
			DrawWidth  = Font->BaseInfo.FontSize + Font->EdgeSize * 2 ;
			DrawHeight = GetDrawStringWidthToHandle_WCHAR_T( String, StringLength, ( int )StringLength, Font->HandleInfo.Handle, VerticalFlag ) ;
//			Result     = DrawHeight ;
			UseTempScreenHandle = Font->ModiDrawScreenV ;
		}
		else
		{
			DrawWidth  = GetDrawStringWidthToHandle_WCHAR_T( String, StringLength, ( int )StringLength, Font->HandleInfo.Handle, VerticalFlag ) ;
			DrawHeight = Font->BaseInfo.FontSize + Font->EdgeSize * 2 ;
//			Result     = DrawWidth ;
			UseTempScreenHandle = Font->ModiDrawScreen ;
		}

		if( DrawWidth <= 0 || DrawHeight <= 0 )
		{
			return -1 ;
		}

		if( NS_GetGraphSize( UseTempScreenHandle[ 0 ], &TempScreenWidth, &TempScreenHeight ) < 0 ||
			DrawWidth < TempScreenWidth || DrawHeight < TempScreenHeight )
		{
			SETUP_GRAPHHANDLE_GPARAM GParam ;

			if( UseTempScreenHandle[ 0 ] >= 0 )
			{
				DeleteGraph( UseTempScreenHandle[ 0 ] ) ;
				DeleteGraph( UseTempScreenHandle[ 1 ] ) ;
				UseTempScreenHandle[ 0 ] = -1 ;
				UseTempScreenHandle[ 1 ] = -1 ;
			}

			if( TempScreenWidth < DrawWidth )
			{
				TempScreenWidth = DrawWidth ;
			}

			if( TempScreenHeight < DrawHeight )
			{
				TempScreenHeight = DrawHeight ;
			}

			Graphics_Image_InitSetupGraphHandleGParam_Normal_DrawValid_NoneZBuffer( &GParam, 32, TRUE ) ;
			UseTempScreenHandle[ 0 ] = Graphics_Image_MakeGraph_UseGParam( &GParam, TempScreenWidth, TempScreenHeight, FALSE, FALSE, 0, FALSE, FALSE ) ;
			if( UseTempScreenHandle[ 0 ] < 0 )
			{
				return -1 ;
			}
			UseTempScreenHandle[ 1 ] = Graphics_Image_MakeGraph_UseGParam( &GParam, DrawWidth, DrawHeight, FALSE, FALSE, 0, FALSE, FALSE ) ;
			if( UseTempScreenHandle[ 1 ] < 0 )
			{
				NS_DeleteGraph( UseTempScreenHandle[ 0 ] ) ;
				UseTempScreenHandle[ 0 ] = -1 ;
				return -1 ;
			}
			NS_SetDeleteHandleFlag( UseTempScreenHandle[ 0 ], &UseTempScreenHandle[ 0 ] ) ;
			NS_SetDeleteHandleFlag( UseTempScreenHandle[ 1 ], &UseTempScreenHandle[ 1 ] ) ;
			NS_SetDeviceLostDeleteGraphFlag( UseTempScreenHandle[ 0 ], TRUE ) ;
			NS_SetDeviceLostDeleteGraphFlag( UseTempScreenHandle[ 1 ], TRUE ) ;
		}

		// 各種描画設定情報を取得
		Graphics_DrawSetting_GetScreenDrawSettingInfo( &ScreenDrawSettingInfo ) ;

		if( Font->TextureCacheUsePremulAlpha )
		{
			NS_SetDrawScreen( UseTempScreenHandle[ 0 ] ) ;
			NS_SetBackgroundColor( 0, 0, 0 ) ;
			NS_ClearDrawScreen() ;

			NS_SetDrawBlendMode( DX_BLENDMODE_PMA_ALPHA, 255 ) ;
			DrawStringToHandle_WCHAR_T( 0, 0, String, StringLength, Color, Font->HandleInfo.Handle, EdgeColor, VerticalFlag ) ;

			NS_SetDrawScreen( ScreenDrawSettingInfo.DrawScreen ) ;

			DrawTempScreenHandle[ 0 ] = NS_DerivationGraph( 0, 0, DrawWidth, DrawHeight, UseTempScreenHandle[ 0 ] ) ;
		}
		else
		{
			NS_SetDrawScreen( UseTempScreenHandle[ 0 ] ) ;
			NS_SetBackgroundColor( 0, 0, 0 ) ;
			NS_ClearDrawScreen() ;
			NS_SetDrawBlendMode( DX_BLENDMODE_SRCCOLOR, 255 ) ;
			FontCacheStringDrawToHandleST(
				TRUE,
				0,
				0,
				0.0f,
				0.0f,
				TRUE, 
				FALSE,
				1.0,
				1.0,
				FALSE, 0.0f, 0.0f, 0.0,
				String,
				Color,
				NULL,
				&GSYS.DrawSetting.DrawArea,
				TRUE,
				Font,
				EdgeColor,
				( int )StringLength,
				VerticalFlag,
				NULL,
				NULL,
				NULL,
				0,
				NULL,
				2
			) ;

			NS_SetDrawScreen( UseTempScreenHandle[ 1 ] ) ;
			NS_SetBackgroundColor( 0, 0, 0 ) ;
			NS_ClearDrawScreen() ;
			NS_SetDrawBlendMode( DX_BLENDMODE_SRCCOLOR, 255 ) ;
			FontCacheStringDrawToHandleST(
				TRUE,
				0,
				0,
				0.0f,
				0.0f,
				TRUE, 
				FALSE,
				1.0,
				1.0,
				FALSE, 0.0f, 0.0f, 0.0,
				String,
				Color,
				NULL,
				&GSYS.DrawSetting.DrawArea,
				TRUE,
				Font,
				EdgeColor,
				( int )StringLength,
				VerticalFlag,
				NULL,
				NULL,
				NULL,
				0,
				NULL,
				1
			) ;

			NS_SetDrawScreen( ScreenDrawSettingInfo.DrawScreen ) ;

			DrawTempScreenHandle[ 0 ] = NS_DerivationGraph( 0, 0, DrawWidth, DrawHeight, UseTempScreenHandle[ 0 ] ) ;
			DrawTempScreenHandle[ 1 ] = NS_DerivationGraph( 0, 0, DrawWidth, DrawHeight, UseTempScreenHandle[ 1 ] ) ;
		}

		// 描画矩形を元に戻す
		NS_SetDrawArea( ScreenDrawSettingInfo.DrawRect.left, ScreenDrawSettingInfo.DrawRect.top, ScreenDrawSettingInfo.DrawRect.right, ScreenDrawSettingInfo.DrawRect.bottom ) ;

		if( DrawTempScreenHandle[ 0 ] < 0 )
		{
//			Result = -1 ;
		}
		else
		{
			NS_SetDrawBlendMode( ScreenDrawSettingInfo.DrawBlendMode, ScreenDrawSettingInfo.DrawBlendParam ) ;
			if( PosIntFlag )
			{
				if( DrawTempScreenHandle[ 0 ] >= 0 )
				{
					NS_DrawModiGraph(  x1i, y1i, x2i, y2i, x3i, y3i, x4i, y4i, DrawTempScreenHandle[ 0 ], TRUE ) ;
					NS_DeleteGraph( DrawTempScreenHandle[ 0 ] ) ;
					DrawTempScreenHandle[ 0 ] = -1 ;
				}

				if( DrawTempScreenHandle[ 1 ] >= 0 )
				{
					NS_DrawModiGraph(  x1i, y1i, x2i, y2i, x3i, y3i, x4i, y4i, DrawTempScreenHandle[ 1 ], TRUE ) ;
					NS_DeleteGraph( DrawTempScreenHandle[ 1 ] ) ;
					DrawTempScreenHandle[ 1 ] = -1 ;
				}
			}
			else
			{
				if( DrawTempScreenHandle[ 0 ] > 0 )
				{
					NS_DrawModiGraphF( x1f, y1f, x2f, y2f, x3f, y3f, x4f, y4f, DrawTempScreenHandle[ 0 ], TRUE ) ;
					NS_DeleteGraph( DrawTempScreenHandle[ 0 ] ) ;
					DrawTempScreenHandle[ 0 ] = -1 ;
				}

				if( DrawTempScreenHandle[ 1 ] > 0 )
				{
					NS_DrawModiGraphF( x1f, y1f, x2f, y2f, x3f, y3f, x4f, y4f, DrawTempScreenHandle[ 1 ], TRUE ) ;
					NS_DeleteGraph( DrawTempScreenHandle[ 1 ] ) ;
					DrawTempScreenHandle[ 1 ] = -1 ;
				}
			}
		}

		// 描画設定情報を元に戻す
		Graphics_DrawSetting_SetScreenDrawSettingInfo( &ScreenDrawSettingInfo ) ;

		return 0 ;
	}
	else
	{
	}

	// 終了
	return 0 ;
}

static int SetupSoftwareStringImage(
	FONTMANAGE *ManageData,
	const wchar_t *String,
	size_t StringLength,
	int Color,
	int EdgeColor,
	int VerticalFlag,
	MEMIMG **UseScreenImg,
	SIZE *DrawSize,
	RECT *DrawArea,
	RECT *DrawAreaBackup,
	DWORD *DrawBrightBackup
)
{
	RECT ClipRect ;

	// 描画領域を得る
	FontCacheStringDrawToHandleST(
		FALSE,
		0,
		0,
		0.0f,
		0.0f,
		TRUE,
		FALSE,
		1.0,
		1.0,
		FALSE, 0.0f, 0.0f, 0.0,
		String,
		0,
		NULL,
		NULL,
		FALSE,
		ManageData,
		0,
		( int )StringLength,
		VerticalFlag,
		DrawSize,
		NULL,
		NULL,
		0,
		NULL,
		0
	) ;

	// 使用する仮スクリーンの決定
	*UseScreenImg = ( ManageData->FontType & DX_FONTTYPE_ANTIALIASING ) ? &GSYS.SoftRender.FontScreenMemImgAlpha : &GSYS.SoftRender.FontScreenMemImgNormal ;

	// 元の描画矩形と描画輝度を保存
	*DrawAreaBackup   = GSYS.DrawSetting.DrawArea ;
	*DrawBrightBackup = GSYS.DrawSetting.bDrawBright ;

	// 仮バッファは画面サイズ分しかないので、そこでクリップされるようにする
	{
		ClipRect.top    = 0 ;
		ClipRect.left   = 0 ;
		ClipRect.right  = GSYS.Screen.MainScreenSizeX ;
		ClipRect.bottom = GSYS.Screen.MainScreenSizeY ;

		DrawArea->left   = 0 ; 
		DrawArea->right  = DrawSize->cx ; 
		DrawArea->top    = 0 ; 
		DrawArea->bottom = DrawSize->cy ;
		RectClipping_Inline( DrawArea, &ClipRect ) ;

		GSYS.DrawSetting.DrawArea = *DrawArea ;
		SetMemImgDrawArea( DrawArea ) ;
	}

	// 描画バッファを透過色で塗りつぶす
	{
		RECT ClearRect ;

		// 透過色の調整
		if( ManageData->FontType & DX_FONTTYPE_ANTIALIASING )
		{
			( *UseScreenImg )->Base->TransColor = 0 ;
		}
		else
		{
			while(  Color     == ( int )( *UseScreenImg )->Base->TransColor ||
					EdgeColor == ( int )( *UseScreenImg )->Base->TransColor )
			{
				( *UseScreenImg )->Base->TransColor ++ ;
			}
		}

		ClearRect = *DrawArea ;
		ClearRect.right  += 4 ;
		ClearRect.bottom += 4 ;
		RectClipping_Inline( &ClearRect, &ClipRect ) ;
		ClearMemImg( *UseScreenImg, &ClearRect, ( *UseScreenImg )->Base->TransColor ) ;
	}

	// 一時バッファに描画
	FontCacheStringDrawToHandleST(
		TRUE,
		VerticalFlag ? DrawSize->cx - ManageData->BaseInfo.FontHeight : 0,
		0,
		0.0f,
		0.0f,
		TRUE,
		FALSE,
		1.0,
		1.0,
		FALSE, 0.0f, 0.0f, 0.0,
		String,
		Color,
		*UseScreenImg,
		DrawArea,
		TRUE,
		ManageData,
		EdgeColor,
		( int )StringLength,
		VerticalFlag,
		NULL,
		NULL,
		NULL,
		0,
		NULL,
		0
	) ;

	// 描画色を白に変更
	GSYS.DrawSetting.bDrawBright = 0xffffff ;

	// 終了
	return 0 ;
}

static int DrawStringSoftware( int x, int y, const wchar_t *String, size_t StringLength, unsigned int Color, FONTMANAGE *ManageData, unsigned int EdgeColor, int VerticalFlag )
{
	SIZE DrawSize ;
//	FONTMANAGE * ManageData ;
	RECT DrawArea, ClipRect, BRect, Rect ;
	DWORD bright ;
	MEMIMG *ScreenImg ;

//	ManageData = GetFontManageDataToHandle_Inline( FontHandle ) ;

	// 描画領域を得る
	FontCacheStringDrawToHandleST(
		FALSE,
		0,
		0,
		0.0f,
		0.0f,
		TRUE,
		FALSE,
		1.0,
		1.0,
		FALSE, 0.0f, 0.0f, 0.0,
		String,
		0,
		NULL,
		NULL,
		FALSE,
		ManageData,
		0,
		( int )StringLength,
		VerticalFlag,
		&DrawSize,
		NULL,
		NULL,
		0,
		NULL,
		0
	) ;

	// ブレンドモードによって処理を分岐
	if( GSYS.DrawSetting.BlendMode != DX_BLENDMODE_NOBLEND || ( ManageData->FontType & DX_FONTTYPE_ANTIALIASING ) )
	{
		// アンチエイリアスでもなくアルファブレンドで且つアルファ値が最大のときは通常描画
		if( ( ManageData->FontType & DX_FONTTYPE_ANTIALIASING ) == 0 &&
			GSYS.DrawSetting.BlendMode == DX_BLENDMODE_ALPHA && GSYS.DrawSetting.BlendParam == 255 ) goto NORMALDRAW ;

		// 使用する仮スクリーンの決定
		ScreenImg = ( ManageData->FontType & DX_FONTTYPE_ANTIALIASING ) ? &GSYS.SoftRender.FontScreenMemImgAlpha : &GSYS.SoftRender.FontScreenMemImgNormal ;

		// 元の描画矩形と描画輝度を保存
		BRect  = GSYS.DrawSetting.DrawArea ;
		bright = GSYS.DrawSetting.bDrawBright ;

		// 仮バッファは画面サイズ分しかないので、そこでクリップされるようにする
		{
			ClipRect.top    = 0 ;
			ClipRect.left   = 0 ;
			ClipRect.right  = GSYS.Screen.MainScreenSizeX ;
			ClipRect.bottom = GSYS.Screen.MainScreenSizeY ;

			DrawArea.left   = 0 ; 
			DrawArea.right  = DrawSize.cx ; 
			DrawArea.top    = 0 ; 
			DrawArea.bottom = DrawSize.cy ;

			// イタリックフォントの場合は少し幅を増やす
			if( ManageData->BaseInfo.Italic )
			{
				DrawArea.right += ManageData->BaseInfo.MaxWidth / 2 ;
			}
			RectClipping_Inline( &DrawArea, &ClipRect ) ;

			GSYS.DrawSetting.DrawArea  = DrawArea ;
			SetMemImgDrawArea( &DrawArea ) ;
		}

		// 描画バッファを透過色で塗りつぶす
		{
			// 透過色の調整
			if( ManageData->FontType & DX_FONTTYPE_ANTIALIASING )
			{
				ScreenImg->Base->TransColor = 0 ;
			}
			else
			{
				while(  Color     == ScreenImg->Base->TransColor ||
						EdgeColor == ScreenImg->Base->TransColor )
				{
					ScreenImg->Base->TransColor ++ ;
				}
			}

			ClearMemImg( ScreenImg, &DrawArea, ScreenImg->Base->TransColor ) ;
		}

		// アンチエイリアスを使用する場合

		// 一時バッファに描画
		FontCacheStringDrawToHandleST(
			TRUE,
			0,
			0,
			0.0f,
			0.0f,
			TRUE,
			FALSE,
			1.0,
			1.0,
			FALSE, 0.0f, 0.0f, 0.0,
			String,
			Color,
			ScreenImg,
			&DrawArea,
			TRUE,
			ManageData,
			EdgeColor,
			( int )StringLength,
			VerticalFlag,
			NULL,
			NULL,
			NULL,
			0,
			NULL,
			0
		) ;

		// 本描画先に描画
		{
			GSYS.DrawSetting.bDrawBright = 0xffffff ;
			Rect.left   = x ;
			Rect.top    = y ;
			Rect.right  = x + DrawArea.right ;
			Rect.bottom = y + DrawArea.bottom ;
			RectClipping_Inline( &Rect, &BRect ) ;
			GSYS.DrawSetting.DrawArea = Rect ;
			SetMemImgDrawArea( &Rect ) ;

			DrawMemImg( GSYS.SoftRender.TargetMemImg, ScreenImg, x, y, TRUE, GSYS.SoftRender.BlendMemImg ) ;
		}

		// パラメータを元に戻す
		GSYS.DrawSetting.DrawArea    = BRect ;
		SetMemImgDrawArea( &BRect ) ;
		GSYS.DrawSetting.bDrawBright = bright ;

		// 終了
		return 0 ;
	}

NORMALDRAW:

	FontCacheStringDrawToHandleST(
		TRUE,
		x,
		y,
		( float )x, 
		( float )y,
		TRUE,
		FALSE,
		1.0,
		1.0,
		FALSE, 0.0f, 0.0f, 0.0,
		String,
		Color,
		GSYS.SoftRender.TargetMemImg,
		&GSYS.DrawSetting.DrawArea,
		TRUE,
		ManageData,
		EdgeColor,
		( int )StringLength,
		VerticalFlag,
		NULL,
		NULL,
		NULL,
		0,
		NULL,
		0
	) ;

	// 終了
	return 0 ;
}

static int DrawExtendStringSoftware( int x, int y, double ExRateX, double ExRateY, const wchar_t *String, size_t StringLength, unsigned int Color, FONTMANAGE *ManageData, unsigned int EdgeColor, int VerticalFlag )
{
	SIZE DrawSize ;
	RECT DrawArea, DrawAreaBackup, Rect, DrawRect ;
	DWORD DrawBrightBackup ;
	MEMIMG *ScreenImg ;

	// 一時バッファへの文字列の描画
	SetupSoftwareStringImage( ManageData, String, StringLength, Color, EdgeColor, VerticalFlag, &ScreenImg, &DrawSize, &DrawArea, &DrawAreaBackup, &DrawBrightBackup ) ;

	// 本描画先に描画
	if( VerticalFlag )
	{
		Rect.left   = _DTOL( x - ( DrawSize.cx - ManageData->BaseInfo.FontHeight ) * ExRateX ) ;
		Rect.top    = y ;
		Rect.right  = Rect.left + _DTOL( DrawArea.right  * ExRateX ) + 3 ;
		Rect.bottom = y         + _DTOL( DrawArea.bottom * ExRateY ) + 3 ;
		RectClipping_Inline( &Rect, &DrawAreaBackup ) ;
		GSYS.DrawSetting.DrawArea = Rect ;
		SetMemImgDrawArea( &Rect ) ;

		DrawRect.left   = _DTOL( x - ( DrawSize.cx - ManageData->BaseInfo.FontHeight ) * ExRateX ) ;
		DrawRect.top    = y ;
		DrawRect.right  = DrawRect.left + _DTOL( GSYS.Screen.MainScreenSizeX * ExRateX ) + 3 ;
		DrawRect.bottom = y             + _DTOL( GSYS.Screen.MainScreenSizeY * ExRateY ) + 3 ;

		DrawEnlargeMemImg( GSYS.SoftRender.TargetMemImg, ScreenImg, &DrawRect, TRUE, GSYS.SoftRender.BlendMemImg ) ;
	}
	else
	{
		Rect.left   = x ;
		Rect.top    = y ;
		Rect.right  = x + _DTOL( DrawArea.right  * ExRateX ) + 3 ;
		Rect.bottom = y + _DTOL( DrawArea.bottom * ExRateY ) + 3 ;
		RectClipping_Inline( &Rect, &DrawAreaBackup ) ;
		GSYS.DrawSetting.DrawArea = Rect ;
		SetMemImgDrawArea( &Rect ) ;

		DrawRect.left   = x ;
		DrawRect.top    = y ;
		DrawRect.right  = x + _DTOL( GSYS.Screen.MainScreenSizeX * ExRateX ) + 3 ;
		DrawRect.bottom = y + _DTOL( GSYS.Screen.MainScreenSizeY * ExRateY ) + 3 ;

		DrawEnlargeMemImg( GSYS.SoftRender.TargetMemImg, ScreenImg, &DrawRect, TRUE, GSYS.SoftRender.BlendMemImg ) ;
	}

	// パラメータを元に戻す
	GSYS.DrawSetting.DrawArea = DrawAreaBackup ;
	SetMemImgDrawArea( &DrawAreaBackup ) ;
	GSYS.DrawSetting.bDrawBright = DrawBrightBackup ;

	// 終了
	return 0 ;
}

static int DrawRotaStringSoftware( int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, const wchar_t *String, size_t StringLength, unsigned int Color, FONTMANAGE *ManageData, unsigned int EdgeColor, int VerticalFlag )
{
	SIZE DrawSize ;
	RECT DrawArea, DrawAreaBackup, DrawRect ;
	DWORD DrawBrightBackup ;
	MEMIMG *ScreenImg ;
	MEMIMG TempImg ;

	// 一時バッファへの文字列の描画
	SetupSoftwareStringImage( ManageData, String, StringLength, Color, EdgeColor, VerticalFlag, &ScreenImg, &DrawSize, &DrawArea, &DrawAreaBackup, &DrawBrightBackup ) ;

	// 文字列を描画した領域だけのイメージを作成
	DerivationMemImg( &TempImg, ScreenImg, 0, 0, DrawArea.right, DrawArea.bottom ) ;

	// 描画領域を元に戻す
	GSYS.DrawSetting.DrawArea = DrawAreaBackup ;
	SetMemImgDrawArea( &DrawAreaBackup ) ;

	// 本描画先に描画
	{
		if( RotAngle == 0.0 )
		{
			if( ExRateX == 1.0 && ExRateY == 1.0 )
			{
				DrawMemImg( GSYS.SoftRender.TargetMemImg, &TempImg, _DTOL( x - RotCenterX ), _DTOL( y - RotCenterY ), TRUE, GSYS.SoftRender.BlendMemImg ) ;
			}
			else
			{
				DrawRect.left   = _DTOL( x             - ( RotCenterX  * ExRateX ) ) ;
				DrawRect.top    = _DTOL( y             - ( RotCenterY  * ExRateY ) ) ;
				DrawRect.right  = _DTOL( DrawRect.left + ( DrawSize.cx * ExRateX ) ) ;
				DrawRect.bottom = _DTOL( DrawRect.top  + ( DrawSize.cy * ExRateY ) ) ;
				DrawEnlargeMemImg( GSYS.SoftRender.TargetMemImg, &TempImg, &DrawRect, TRUE, GSYS.SoftRender.BlendMemImg ) ;
			}
		}
		else
		{
			float dcx, dcy ;
			float Sin, Cos ;
			float bcx, bcy ;

			bcx = ( float )( ( -RotCenterX + DrawSize.cx * 0.5f ) * ExRateX ) ;
			bcy = ( float )( ( -RotCenterY + DrawSize.cy * 0.5f ) * ExRateY ) ;

			_SINCOS_PLATFORM( ( float )RotAngle, &Sin, &Cos ) ;
			dcx = bcx * Cos - bcy * Sin ;
			dcy = bcx * Sin + bcy * Cos ;

			DrawRotationMemImg( GSYS.SoftRender.TargetMemImg, &TempImg,
				_FTOL( x + dcx ), _FTOL( y + dcy ),
				( float )RotAngle, ( float )ExRateX, ( float )ExRateY,
				TRUE, FALSE, FALSE, GSYS.SoftRender.BlendMemImg ) ;
		}
	}

	// 描画輝度を元に戻す
	GSYS.DrawSetting.bDrawBright = DrawBrightBackup ;

	// 終了
	return 0 ;
}

static int DrawModiStringSoftware( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, const wchar_t *String, size_t StringLength, unsigned int Color, FONTMANAGE *ManageData, unsigned int EdgeColor, int VerticalFlag )
{
	SIZE DrawSize ;
	RECT DrawArea, DrawAreaBackup ;
	DWORD DrawBrightBackup ;
	MEMIMG *ScreenImg ;
	MEMIMG TempImg ;

	// 一時バッファへの文字列の描画
	SetupSoftwareStringImage( ManageData, String, StringLength, Color, EdgeColor, VerticalFlag, &ScreenImg, &DrawSize, &DrawArea, &DrawAreaBackup, &DrawBrightBackup ) ;

	// 文字列を描画した領域だけのイメージを作成
	DerivationMemImg( &TempImg, ScreenImg, 0, 0, DrawArea.right, DrawArea.bottom ) ;

	// 描画領域を元に戻す
	GSYS.DrawSetting.DrawArea = DrawAreaBackup ;
	SetMemImgDrawArea( &DrawAreaBackup ) ;

	// 本描画先に描画
	{
		DX_POINTDATA p[4] ;

		p[0].x = x1 ;		p[0].y = y1 ;		p[0].u = 0 ;				p[0].v = 0 ;
		p[1].x = x2 ;		p[1].y = y2 ;		p[1].u = DrawArea.right ;	p[1].v = 0 ;
		p[2].x = x4 ;		p[2].y = y4 ;		p[2].u = 0 ;				p[2].v = DrawArea.bottom ;
		p[3].x = x3 ;		p[3].y = y3 ;		p[3].u = DrawArea.right ;	p[3].v = DrawArea.bottom ;

		DrawBasicPolygonMemImg( GSYS.SoftRender.TargetMemImg, &TempImg, &p[0], TRUE, GSYS.SoftRender.BlendMemImg, 0 ) ;
		DrawBasicPolygonMemImg( GSYS.SoftRender.TargetMemImg, &TempImg, &p[1], TRUE, GSYS.SoftRender.BlendMemImg, 0 ) ;
	}

	// 描画輝度を元に戻す
	GSYS.DrawSetting.bDrawBright = DrawBrightBackup ;

	// 終了
	return 0 ;
}

#endif // DX_NON_GRAPHICS

// フォントシステムの初期化
extern int InitFontManage( void )
{
	int i ;
	int j ;
	int k ;

	if( FSYS.InitializeFlag == TRUE )
		return -1 ;

	// 初期化フラグを立てる
	FSYS.InitializeFlag = TRUE ;

	// フォントハンドル管理情報を初期化する
	InitializeHandleManage( DX_HANDLETYPE_FONT, sizeof( FONTMANAGE ) + sizeof( FONTMANAGE_PF ), MAX_FONT_NUM, InitializeFontHandle, TerminateFontHandle, L"Font" ) ;

	// 全角スペースの wchar_t コードを準備
	{
		const char *UTF16LE_DoubleByteSpaceString = "\x00\x30\x00"/*@ L"　" @*/ ;
		wchar_t     WCHAR_T_DoubleByteSpaceString[ 16 ] ;
		ConvString( UTF16LE_DoubleByteSpaceString, 1, DX_CHARCODEFORMAT_UTF16LE, ( char * )WCHAR_T_DoubleByteSpaceString, sizeof( WCHAR_T_DoubleByteSpaceString ), WCHAR_T_CHARCODEFORMAT ) ;
		FSYS.DoubleByteSpaceCharCode = WCHAR_T_DoubleByteSpaceString[ 0 ] ;
	}

	// デフォルトフォントの設定をセット
	if( FSYS.EnableInitDefaultFontName == FALSE )
	{
		_MEMSET( FSYS.DefaultFontName, 0, sizeof( FSYS.DefaultFontName ) );
	}
	FSYS.EnableInitDefaultFontName = FALSE ;

	if( FSYS.EnableInitDefaultFontType == FALSE )
	{
		FSYS.DefaultFontType = FSYS.AntialiasingFontOnlyFlag ? DX_FONTTYPE_ANTIALIASING : DEFAULT_FONT_TYPE ;
	}
	FSYS.EnableInitDefaultFontType = FALSE ;

	if( FSYS.EnableInitDefaultFontSize == FALSE )
	{
		FSYS.DefaultFontSize     = DEFAULT_FONT_SIZE ;
	}
	FSYS.EnableInitDefaultFontSize = FALSE ;

	if( FSYS.EnableInitDefaultFontThick == FALSE )
	{
		FSYS.DefaultFontThick    = DEFAULT_FONT_THINCK ;
	}
	FSYS.EnableInitDefaultFontThick = FALSE ;

	if( FSYS.EnableInitDefaultFontCharSet == FALSE )
	{
		FSYS.DefaultFontCharSet  = _GET_CHARSET() ;
		FSYS.DefaultFontCharSet_Change = TRUE ;
	}
	FSYS.EnableInitDefaultFontCharSet = FALSE ;

	if( FSYS.EnableInitDefaultFontEdgeSize == FALSE )
	{
		FSYS.DefaultFontEdgeSize = DEFAULT_FONT_EDGESIZE ;
	}
	FSYS.EnableInitDefaultFontEdgeSize = FALSE ;

	if( FSYS.EnableInitDefaultFontItalic == FALSE )
	{
		FSYS.DefaultFontItalic   = FALSE ;
	}
	FSYS.EnableInitDefaultFontItalic = FALSE ;

	if( FSYS.EnableInitDefaultFontSpace == FALSE )
	{
		FSYS.DefaultFontSpace    = 0 ;
	}
	FSYS.EnableInitDefaultFontSpace = FALSE ;

	if( FSYS.EnableInitDefaultFontLineSpace == FALSE )
	{
		FSYS.DefaultFontLineSpaceValid = FALSE ;
		FSYS.DefaultFontLineSpace    = 0 ;
	}
	FSYS.EnableInitDefaultFontLineSpace = FALSE ;

	// テーブルを作成する
	for( i = 0 ; i < 256 ; i ++ )
	{
		j = i ;
		for( k = 0 ; j != 0 ; k ++, j &= j - 1 ){}
		FSYS.BitCountTable[ i ] = ( BYTE )k ;

		FSYS.MAX255ToMAX16[ i ] = ( BYTE )( i * 16 / 255 ) ;
		FSYS.MAX255ToMAX64[ i ] = ( BYTE )( i * 64 / 255 ) ;
	}
	for( i = 0 ; i < 16 ; i ++ )
	{
		if( i > 0 )
		{
			FSYS.MAX15ToMAX16[ i ] = ( BYTE )( i + 1 ) ;
		}
		else
		{
			FSYS.MAX15ToMAX16[ i ] = 0 ;
		}
		FSYS.MAX15ToMAX64[ i ] = ( BYTE )( i * 64 / 15 ) ;
	}

	// フォントハンドルの初期化
	InitFontToHandleBase() ;

	// 環境依存の初期化処理を行う
	if( InitFontManage_PF() != 0 )
	{
		return -1 ;
	}

	// 終了
	return 0 ;
}

// フォント制御の終了
extern int TermFontManage( void )
{
	if( FSYS.InitializeFlag == FALSE )
	{
		return -1 ;
	}

	// 全てのフォントを削除
	InitFontToHandleBase( TRUE ) ;

	// フォントハンドル管理情報の後始末
	TerminateHandleManage( DX_HANDLETYPE_FONT ) ;

	// デフォルトフォントイメージ用にメモリを確保していたら解放する
	if( FSYS.DefaultFontImage != NULL )
	{
		DXFREE( FSYS.DefaultFontImage ) ;
		FSYS.DefaultFontImage = NULL ;
	}

#ifndef DX_NON_GRAPHICS
	// デフォルトフォントイメージのグラフィックハンドルがある場合は削除する
	if( FSYS.DefaultFontImageGraphHandle[ 0 ][ 0 ] > 1 )
	{
		int i ;
		int j ;

		for( i = 0 ; i < 8 ; i ++ )
		{
			for( j = 0 ; j < 16 ; j ++ )
			{
				NS_DeleteGraph( FSYS.DefaultFontImageGraphHandle[ i ][ j ] ) ;
				FSYS.DefaultFontImageGraphHandle[ i ][ j ] = 0 ;
			}
		}
	}
#endif // DX_NON_GRAPHICS

	// 環境依存の終了処理を行う
	if( TermFontManage_PF() != 0 )
	{
		return -1 ;
	}

	// 初期化フラグを倒す
	FSYS.InitializeFlag = FALSE ;

	// 終了
	return 0 ;
}

// フォント、文字列描画関係関数

// フォントのキャッシュ情報を初期化する
extern int InitCacheFontToHandle( void )
{
	int i, Handle ;
	FONTMANAGE * ManageData ;
	wchar_t FontName[ 256 ] ;
	CREATEFONTTOHANDLE_GPARAM GParam ;
	int LineSpace, LineSpaceValid, Space, Size, Thick, FontType, CharSet, EdgeSize, Italic, UseCharCodeFormat ;

	if( HandleManageArray[ DX_HANDLETYPE_FONT ].InitializeFlag == FALSE )
		return -1 ;

	for( i = HandleManageArray[ DX_HANDLETYPE_FONT ].AreaMin ; i <= HandleManageArray[ DX_HANDLETYPE_FONT ].AreaMax ; i ++ )
	{
		ManageData = ( FONTMANAGE * )HandleManageArray[ DX_HANDLETYPE_FONT ].Handle[ i ] ;
		if( ManageData == NULL ) continue ;

		// 共通のパラメータを保存
		EdgeSize			= ManageData->EdgeSize ;
		Space				= ManageData->Space ;
		LineSpaceValid		= ManageData->LineSpaceValidFlag ;
		LineSpace			= ManageData->LineSpace ;
		UseCharCodeFormat	= ManageData->UseCharCodeFormat ;

		// フォントファイルを使用している場合とそうでない場合で処理を分岐
		if( ManageData->UseFontDataFile )
		{
			void *FontDataImageTemp = NULL ;
			int FontDataImageTempSize = 0 ;

			// フォントファイルイメージのバックアップを取る
			FontDataImageTemp = DXALLOC( ManageData->FontDataFile.FontFileSize ) ;
			if( FontDataImageTemp == NULL )
			{
				DXST_LOGFILEFMT_ADDUTF16LE(( "\xd5\x30\xa9\x30\xf3\x30\xc8\x30\xd5\x30\xa1\x30\xa4\x30\xeb\x30\xa4\x30\xe1\x30\xfc\x30\xb8\x30\x92\x30\x00\x4e\x42\x66\x84\x76\x6b\x30\xdd\x4f\x58\x5b\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"フォントファイルイメージを一時的に保存するメモリ領域の確保に失敗しました\n" @*/ )) ;
				continue ;
			}
			_MEMCPY( FontDataImageTemp, ManageData->FontDataFile.FileBuffer, ManageData->FontDataFile.FontFileSize ) ;
			FontDataImageTempSize = ManageData->FontDataFile.FontFileSize ;

			// フォントハンドルの作り直し
			Handle = ManageData->HandleInfo.Handle ;
			NS_DeleteFontToHandle( Handle ) ;
			InitCreateFontToHandleGParam( &GParam ) ;
			LoadFontDataFromMemToHandle_UseGParam( &GParam, FontDataImageTemp, FontDataImageTempSize, EdgeSize, Handle, FALSE ) ;

			// メモリの解放
			DXFREE( FontDataImageTemp ) ;
			FontDataImageTemp = NULL ;
		}
		else
		{
			_WCSCPY_S( FontName, sizeof( FontName ), ManageData->FontName ) ;
			Size				= ManageData->BaseInfo.FontSize ;
			Thick				= ManageData->BaseInfo.FontThickness ;
			FontType			= ManageData->FontType ;
			CharSet				= ManageData->BaseInfo.CharSet ;
			Italic				= ManageData->BaseInfo.Italic ;

			// フォントハンドルの作り直し
			Handle = ManageData->HandleInfo.Handle ;
			NS_DeleteFontToHandle( Handle ) ;
			InitCreateFontToHandleGParam( &GParam ) ;
			CreateFontToHandle_UseGParam( &GParam, FontName, Size, Thick, FontType, CharSet, EdgeSize, Italic, Handle, FALSE ) ;
		}

		// 共通のパラメータを復帰
		NS_SetFontSpaceToHandle( Space, Handle ) ;
		if( LineSpaceValid )
		{
			NS_SetFontLineSpaceToHandle( LineSpace, Handle ) ;
		}
		NS_SetFontCharCodeFormatToHandle( UseCharCodeFormat, Handle ) ;
	}

	// 終了
	return 0 ;
}

// 特定のフォントのキャッシュ情報を初期化する
extern int InitFontCacheToHandle( FONTMANAGE *ManageData, int /*ASyncThread*/ )
{
//	FONTMANAGE * ManageData ;

//	DEFAULT_FONT_HANDLE_SETUP

//	// エラー判定
//	if( ASyncThread )
//	{
//		if( FONTHCHK_ASYNC( FontHandle, ManageData ) )
//			return -1  ;
//	}
//	else
//	{
//		if( FONTHCHK( FontHandle, ManageData ) )
//			return -1  ;
//	}

	// テキストキャッシュデータの初期化
	{
		int i ;
		FONTCHARDATA *CharData ;

		_MEMSET( ManageData->CodeData,   0, sizeof( FONTCODEDATA   ) * 0x10000 ) ;
		_MEMSET( ManageData->CharData,   0, sizeof( FONTCHARDATA   ) * ( FONT_CACHE_MAXNUM + 1 ) ) ;
		_MEMSET( ManageData->CodeDataEx, 0, sizeof( FONTCHARDATA * ) * FONT_CACHE_EX_NUM ) ;
		ManageData->CodeDataExNum = 0 ;

		CharData = ManageData->CharData ;
		for( i = 0 ; i < ManageData->MaxCacheCharNum ; i ++, CharData ++ )
		{
			CharData->GraphIndex  = i ;	
			CharData->GraphIndexX = i / ManageData->LengthCharNum ;
			CharData->GraphIndexY = i % ManageData->LengthCharNum ;
		}
		CharData->GraphIndex = -1 ;

		ManageData->Index = 0 ;
	}

	// 終了
	return 0 ;
}

// フォントハンドルが使用する描画バッファやテクスチャキャッシュを再初期化する
extern int RefreshFontDrawResourceToHandle( FONTMANAGE *ManageData, int ASyncThread )
{
#ifndef DX_NON_GRAPHICS
	// TextureCache のロスト判定
	if( ManageData->TextureCacheLostFlag == TRUE )
	{
		int Use3D, w ;
		int UsePaletteFlag ;
		int PaletteBitDepth ;
		SETUP_GRAPHHANDLE_GPARAM GParam ;

		ManageData->TextureCacheLostFlag = FALSE ;

		// パレットテクスチャかどうかで処理を分岐
		if( ManageData->TextureCacheBaseImage.ColorData.ColorBitDepth == 8 )
		{
			UsePaletteFlag = TRUE ;

			Graphics_Image_InitSetupGraphHandleGParam_Normal_NonDrawValid( &GParam, 16, TRUE, FALSE ) ;
			if( ( ManageData->FontType & DX_FONTTYPE_ANTIALIASING ) == 0 )
			{
				PaletteBitDepth = 4 ;
			}
			else
			{
				PaletteBitDepth = ManageData->TextureCacheBaseImage.ColorData.MaxPaletteNo == 255 ? 8 : 4 ;
			}
		}
		else
		{
			UsePaletteFlag  = FALSE ;
			PaletteBitDepth = 0 ;

			if( ( ManageData->FontType & DX_FONTTYPE_ANTIALIASING ) == 0 )
			{
				Graphics_Image_InitSetupGraphHandleGParam_Normal_NonDrawValid( &GParam, 16, FALSE, TRUE ) ;
			}
			else
			{
				Graphics_Image_InitSetupGraphHandleGParam_Normal_NonDrawValid( &GParam, ManageData->TextureCacheColorBitDepth, TRUE, FALSE ) ;
			}
		}

		Use3D = NS_GetUse3DFlag() ;
		NS_SetUse3DFlag( TRUE ) ;

		// エッジつきの場合は横幅を倍にする
		w = ( ManageData->FontType & DX_FONTTYPE_EDGE ) != 0 ? ManageData->CacheImageSize.cx * 2 : ManageData->CacheImageSize.cx ;
		GParam.NotInitGraphDelete = TRUE ;
		GParam.UseLinearMapTextureFlag = TRUE ;
		ManageData->TextureCache = Graphics_Image_MakeGraph_UseGParam( &GParam, w, ManageData->CacheImageSize.cy, FALSE, UsePaletteFlag, PaletteBitDepth, FALSE, ASyncThread ) ;
		ManageData->TextureCacheSub = -1 ;
		
		// エッジ付きの場合は派生させる
		if( ( ManageData->FontType & DX_FONTTYPE_EDGE ) != 0 )
		{
			ManageData->TextureCacheSub = Graphics_Image_DerivationGraph_UseGParam(
				FALSE,
				ManageData->CacheImageSize.cx, ( float )ManageData->CacheImageSize.cx,
				0, 0.0f,
				ManageData->CacheImageSize.cx, ( float )ManageData->CacheImageSize.cx,
				ManageData->CacheImageSize.cy, ( float )ManageData->CacheImageSize.cy,
				ManageData->TextureCache,
				ASyncThread
			) ;
		}

		NS_SetUse3DFlag( Use3D ) ;

		// エラーチェック
		if( ManageData->TextureCache < 0 )
		{
			return -1 ;
		}
		NS_SetGraphLostFlag( ManageData->TextureCache, &ManageData->TextureCacheLostFlag ) ;

		// テクスチャのサイズを取得しておく
		NS_GetGraphTextureSize( ManageData->TextureCache, &ManageData->TextureCacheSizeX, &ManageData->TextureCacheSizeY ) ;
		ManageData->TextureCacheInvSizeX = 1.0f / ManageData->TextureCacheSizeX ;
		ManageData->TextureCacheInvSizeY = 1.0f / ManageData->TextureCacheSizeY ;

		// 透明色で塗りつぶす
/*		if( ManageData->TextureCacheUsePremulAlpha )
		{
			Graphics_Image_FillGraph_UseGParam( ManageData->TextureCache, 0,0,0,0, ASyncThread ) ;
			if( ( ManageData->FontType & DX_FONTTYPE_EDGE ) != 0 )
			{
				Graphics_Image_FillGraph_UseGParam( ManageData->TextureCacheSub, 0,0,0,0, ASyncThread ) ;
			}
		}
		else
		{
			Graphics_Image_FillGraph_UseGParam( ManageData->TextureCache, 255,255,255,0, ASyncThread ) ;
			if( ( ManageData->FontType & DX_FONTTYPE_EDGE ) != 0 )
			{
				Graphics_Image_FillGraph_UseGParam( ManageData->TextureCacheSub, 255,255,255,0, ASyncThread ) ;
			}
		}
*/
		// フォントのキャッシュ情報を初期化する
		InitFontCacheToHandle( ManageData, ASyncThread ) ;
	}

#endif // DX_NON_GRAPHICS

	// 終了
	return 0 ;
}

// アンチエイリアスフォントのみ使用できるか、フラグをセットする
extern int SetAntialiasingFontOnlyFlag( int AntialiasingFontOnlyFlag )
{
	FSYS.AntialiasingFontOnlyFlag = AntialiasingFontOnlyFlag ;

	// 終了
	return 0 ;
}

// デフォルトフォントを再作成する
extern int RefreshDefaultFont( void )
{
	FONTMANAGE * ManageData ;
	
	// 設定が変わっていない場合は再作成しない
	if( FSYS.DefaultFontHandle > 0 )
	{
		int FontType, Size, Thick/*, CharSet */, EdgeSize, Italic ;
		wchar_t *FontName ;

		ManageData = GetFontManageDataToHandle_Inline( FSYS.DefaultFontHandle ) ;

		FontName	= FSYS.DefaultFontName ;
		FontType 	= FSYS.DefaultFontType ;
		Size 		= FSYS.DefaultFontSize ;
		Thick 		= FSYS.DefaultFontThick ;
//		CharSet 	= FSYS.DefaultFontCharSet ;
		EdgeSize	= ( FSYS.DefaultFontType & DX_FONTTYPE_EDGE ) ? FSYS.DefaultFontEdgeSize : -1 ;
		Italic		= FSYS.DefaultFontItalic ;

		if( FontType 	== -1 )
		{
			FontType  = FSYS.AntialiasingFontOnlyFlag ? DX_FONTTYPE_ANTIALIASING : DEFAULT_FONT_TYPE ;
		}
		else
		{
			if( FSYS.AntialiasingFontOnlyFlag )
			{
				if( ( FontType & DX_FONTTYPE_EDGE ) != 0 )
				{
					FontType = DX_FONTTYPE_ANTIALIASING_EDGE ;
				}
				else
				{
					FontType = DX_FONTTYPE_ANTIALIASING ;
				}
			}
		}
		if( EdgeSize	> 1   )	FontType |= DX_FONTTYPE_EDGE ;
		if( Size 		== -1 ) Size      = DEFAULT_FONT_SIZE ;
		if( Thick 		== -1 ) Thick     = DEFAULT_FONT_THINCK ;
//		if( CharSet		== -1 ) CharSet   = _GET_CHARSET() ;
		if( EdgeSize	== -1 ) EdgeSize  = DEFAULT_FONT_EDGESIZE ;
		
		if( ( FSYS.UseDefaultFontImage == TRUE &&
			  FSYS.DefaultFontImageGraphHandle[ 0 ][ 0 ] > 0 ) ||
			( FSYS.UseDefaultFontImage == FALSE &&
			  ( FontName[ 0 ] == L'\0' || _WCSCMP( FontName, ManageData->FontName ) == 0 ) &&
			  FontType 	== ManageData->FontType &&
			  Size 		== ManageData->BaseInfo.FontSize &&
			  Thick 	== ManageData->BaseInfo.FontThickness &&
			  FSYS.DefaultFontCharSet_Change == FALSE /* CharSet == ManageData->BaseInfo.CharSet */ &&
			(   ( FontType & DX_FONTTYPE_EDGE ) == 0 ||
			  ( ( FontType & DX_FONTTYPE_EDGE ) != 0 && EdgeSize == ManageData->EdgeSize ) ) &&
			  Italic	== ManageData->BaseInfo.Italic ) )
			return 0 ;

		NS_DeleteFontToHandle( FSYS.DefaultFontHandle ) ;
	}

	// 設定が変わっているかフォントハンドルが無効になっている場合は再作成する	
	CREATEFONTTOHANDLE_GPARAM GParam ;
	InitCreateFontToHandleGParam( &GParam ) ;
	FSYS.DefaultFontHandle = CreateFontToHandle_UseGParam(
		&GParam,
		FSYS.DefaultFontName,
		FSYS.DefaultFontSize,
		FSYS.DefaultFontThick,
		FSYS.DefaultFontType,
		FSYS.DefaultFontCharSet,
		( FSYS.DefaultFontType & DX_FONTTYPE_EDGE ) ? FSYS.DefaultFontEdgeSize : -1,
		FSYS.DefaultFontItalic,
		-1,
		FALSE ) ;
	FSYS.DefaultFontCharSet_Change = FALSE ;

	// デフォルトフォントハンドルの作成に失敗したらデフォルトフォントファイルとフォントイメージを使用する
	if( FSYS.DefaultFontHandle < 0 )
	{
		int i ;
		int j ;
		TCHAR Char[ 2 ] = { 0 } ;

		FSYS.DefaultFontHandle = NS_LoadFontDataFromMemToHandle( DefaultFontDataFileImage, sizeof( DefaultFontDataFileImage ) ) ;

		// 画像ファイルがデコードされていなかったらデコードする
		if( FSYS.DefaultFontImage == NULL )
		{
			// 文字コードの場合はバイナリコードに変換する
			if( DefaultFontImageConvert == FALSE )
			{
				Char128ToBin( DefaultFontImage, DefaultFontImage ) ;
				DefaultFontImageConvert = TRUE ;
			}

			// デコード後のファイルイメージを格納するメモリ領域の確保
			FSYS.DefaultFontImage = DXCALLOC( ( size_t )DXA_Decode( DefaultFontImage, NULL ) ) ;
			if( FSYS.DefaultFontImage == NULL )
			{
				return -1 ;
			}

			// デコード
			DXA_Decode( DefaultFontImage, FSYS.DefaultFontImage ) ;
		}

#ifndef DX_NON_GRAPHICS
		// 文字のグラフィックハンドルが作成されていなかったら作成する
		if( FSYS.DefaultFontImageGraphHandle[ 0 ][ 0 ] <= 0 )
		{
			int TransRed ;
			int TransGreen ;
			int TransBlue ;

			// 透過色を紫色に変更
			NS_GetTransColor( &TransRed, &TransGreen, &TransBlue ) ;
			NS_SetTransColor( 255,0,255 ) ;

			// グラフィックハンドルの作成
			NS_CreateDivGraphFromMem( FSYS.DefaultFontImage, DXA_Decode( DefaultFontImage, NULL ), 128, 16, 8, 8, 16, FSYS.DefaultFontImageGraphHandle[ 0 ] ) ;

			// 透過色を元に戻す
			NS_SetTransColor( TransRed, TransGreen, TransBlue ) ;

			// 削除されたときに -1 が代入されるようにする
			for( i = 0 ; i < 8 ; i ++ )
			{
				for( j = 0 ; j < 16 ; j ++ )
				{
					NS_SetDeleteHandleFlag( FSYS.DefaultFontImageGraphHandle[ i ][ j ], &FSYS.DefaultFontImageGraphHandle[ i ][ j ] ) ;
				}
			}
		}
#endif // DX_NON_GRAPHICS

		// フォントハンドルへの文字の割り当て
		for( i = 0 ; i < 8 ; i ++ )
		{
			for( j = 0 ; j < 16 ; j ++ )
			{
				Char[ 0 ] = ( TCHAR )( j + i * 16 ) ;
				NS_AddFontImageToHandle( FSYS.DefaultFontHandle, Char, FSYS.DefaultFontImageGraphHandle[ i ][ j ], 0, 0, 8 ) ;
			}
		}

		// デフォルトフォントイメージを使用しているかどうかのフラグを立てる
		FSYS.UseDefaultFontImage = TRUE ;
	}
	else
	{
		// デフォルトフォントイメージを使用しているかどうかのフラグを倒す
		FSYS.UseDefaultFontImage = FALSE ;

		NS_SetFontSpaceToHandle(     FSYS.DefaultFontSpace,     FSYS.DefaultFontHandle ) ;
		if( FSYS.DefaultFontLineSpaceValid )
		{
			NS_SetFontLineSpaceToHandle( FSYS.DefaultFontLineSpace, FSYS.DefaultFontHandle ) ;
		}
	}

	NS_SetDeleteHandleFlag( FSYS.DefaultFontHandle, &FSYS.DefaultFontHandle ) ;

	return 0 ;
}


// 文字キャッシュに新しい文字を加える
static int FontCacheStringAddToHandle(
	FONTMANAGE *	ManageData,
	const wchar_t *	String,
	int				StrLen,
	DWORD *			DrawStrBufferP,
	int *			DrawCharNumP
)
{
	static DWORD addbuf[ 1024 ] ;	// キャッシュに追加する際に使用する文字列データ（３バイトで１文字を表記）
	static DWORD temp[ 1024 ] ;
	static DWORD resetExist[ 1024 ] ;

//	FONTMANAGE * ManageData ;
	FONTCODEDATA * fontcode ;
	int i, j ;						// 繰り返しと汎用変数
	DWORD *drstr, *adstr ;		// それぞれDrawStrBuf,CacheAddBufのポインタ
	DWORD *rexist ;
	int addnum ;				// キャッシュに追加する文字の数
	int drawnum ;				// ２バイト文字１バイト文字全部ひっくるめて何文字あるか保存する変数
	int len ;					// 文字列の長さ
	int UseSize ;
	int UseAlloc ;
	DWORD *AllocDrawStr    = NULL ;
	DWORD *AllocResetExist = NULL ;

//	DEFAULT_FONT_HANDLE_SETUP

	// エラー判定
//	if( FONTHCHK( FontHandle, ManageData ) )
//	{
//		return -1  ;
//	}

	UseAlloc = FALSE ;

	// 文字列の長さをセット
	if( StrLen < 0 )
	{
		len = ( int )_WCSLEN( String ) ;
	}
	else
	{
		len = StrLen ;
	}

	// デフォルトバッファのサイズを超える場合はテンポラリバッファの確保
	if( len > 1024 )
	{
		UseAlloc        = TRUE ;
		AllocDrawStr    = ( DWORD * )DXALLOC( len * sizeof( DWORD ) + len * sizeof( DWORD ) ) ;
		AllocResetExist = AllocDrawStr + len ;
		if( AllocDrawStr == NULL )
		{
			return -1 ;
		}
	}
	
	addnum   = 0 ;
	drawnum  = 0 ;
	drstr    = DrawStrBufferP != NULL ? DrawStrBufferP : ( UseAlloc ? AllocDrawStr : temp ) ;
	adstr    = UseAlloc ? AllocDrawStr    : addbuf ;
	rexist   = UseAlloc ? AllocResetExist : resetExist ;
	fontcode = ManageData->CodeData ;
	WCHAR_T_CHARCODEFORMAT ;
	for( i = 0 ; i < len ; drstr ++ )
	{
		int CharLength ;
		*drstr = WCHAR_T_GetCharCode( String, g_BaseFuncSystem.Use_wchar_t_CharCodeFormat, &UseSize ) ;
		CharLength = UseSize / sizeof( wchar_t ) ;
		if( i + CharLength > len )
		{
			break ;
		}
		String  += CharLength ;
		i       += CharLength ;
		drawnum ++ ;

		// 画像置き換え文字だった場合はキャッシュに加えない
		for( j = 0 ; j < ManageData->GraphHandleFontImageNum ; j ++ )
		{
			if( ManageData->GraphHandleFontImage[ j ].CodeUnicode == *drstr )
			{
				break ;
			}
		}
		if( j == ManageData->GraphHandleFontImageNum )
		{
			// キャッシュに存在しない文字だった場合キャッシュ文字の一括追加をするためのバッファに保存、キャッシュに追加する文字数を増やす
			if( addnum != ManageData->MaxCacheCharNum )
			{
				// 0x10000 個のテーブルに収まらないコードかどうかで処理を分岐
				if( *drstr > 0xffff )
				{
					// テーブルに収まらない場合
					for( j = 0 ; j < addnum ; j ++ )
					{
						if( rexist[ j ] == *drstr )
						{
							break ;
						}
					}
					if( j == addnum )
					{
						for( j = 0 ; j < ManageData->CodeDataExNum ; j ++ )
						{
							if( ManageData->CodeDataEx[ j ]->CodeUnicode == *drstr )
							{
								break ;
							}
						}
						if( j == ManageData->CodeDataExNum )
						{
							rexist[ addnum ] = *drstr ;
							addnum ++ ;

							*adstr = *drstr ;
							adstr ++ ;
						}
					}
				}
				else
				{
					// テーブルに収まる場合
					if( fontcode[ *drstr ].ExistFlag == FALSE )
					{
						fontcode[ *drstr ].ExistFlag = TRUE ;

						rexist[ addnum ] = *drstr ;
						addnum ++ ;

						*adstr = *drstr ;
						adstr ++ ;
					}
				}
			}
		}
	}

	for( i = 0 ; i < addnum ; i ++ )
	{
		if( rexist[ i ] <= 0xffff )
		{
			fontcode[ rexist[ i ] ].ExistFlag = FALSE ;
		}
	}

	// キャッシュ文字の一括追加を行う
	if( addnum != 0 )
	{
		FontCacheCharAddToHandle( addnum, UseAlloc ? AllocDrawStr : addbuf, ManageData ) ;
	}

	// 描画する文字の数を保存する
	if( DrawCharNumP != NULL )
	{
		*DrawCharNumP = drawnum ;
	}

	// メモリを確保していたら解放
	if( UseAlloc )
	{
		DXFREE( AllocDrawStr ) ;
	}

	// 終了
	return 0 ;
}

// 指定のフォントデータに画像を転送する
extern int FontCacheCharImageBltToHandle(
	FONTMANAGE *	ManageData,
	FONTCHARDATA *	CharData,
	DWORD			CharCode,
	int				Space,
	int				ImageType /* DX_FONT_SRCIMAGETYPE_1BIT 等 */,
	void *			ImageBuffer,
	DWORD			ImageSizeX,
	DWORD			ImageSizeY,
	DWORD			ImagePitch,
	int				ImageDrawX,
	int				ImageDrawY,
	int				ImageAddX,
	int				TextureCacheUpdate
)
{
	FONTCODEDATA * 	CodeData ;
	int				AddX ;
	int				AddY ;
	int				AddSize ;
	int				DestX ;
	int				DestY ;
	BYTE *			ResizeBuffer = NULL ;
	DWORD			SampleScale ;
	DWORD			DrawModDrawY ;
	DWORD			DataHeight ;

	// 既にフォントデータが使用されていた場合は無効にする
	if( CharData->ValidFlag )
	{
		// テーブルに収まっているデータかどうかで無効の仕方が異なる
		if( CharData->CodeData != NULL )
		{
			// テーブルに収まっている場合

			// コードデータを無効にする
			CharData->CodeData->ExistFlag = FALSE ;
			CharData->CodeData->CharData  = NULL ;
		}
		else
		{
			int i ;

			// テーブルに収まらない場合
			for( i = 0 ; ManageData->CodeDataEx[ i ] != CharData ; i ++ ){}
			ManageData->CodeDataExNum -- ;
			if( ManageData->CodeDataExNum - i > 0 )
			{
				_MEMMOVE( &ManageData->CodeDataEx[ i ], &ManageData->CodeDataEx[ i + 1 ], sizeof( FONTCHARDATA * ) * ( ManageData->CodeDataExNum - i ) ) ;
			}
		}

		// データを無効にする
		CharData->ValidFlag = FALSE ;
	}

	// 画像の倍率をセット
	switch( ImageType )
	{
	case DX_FONT_SRCIMAGETYPE_1BIT_SCALE4 :
		SampleScale = 4 ;
		break ;

	case DX_FONT_SRCIMAGETYPE_1BIT_SCALE8 :
		SampleScale = 8 ;
		break ;

	case DX_FONT_SRCIMAGETYPE_1BIT_SCALE16 :
		SampleScale = 16 ;
		break ;

	default :
		SampleScale = 1 ;
		break ;
	}

	CharData->CodeUnicode = CharCode ;
	
	if( Space )
	{
		CharData->DrawX = 0 ;
		CharData->DrawY = 0 ;
		CharData->AddX = ( short )( ( ImageAddX + ( SampleScale >> 2 ) ) / SampleScale ) ;
		CharData->SizeX = 0 ;
		CharData->SizeY = 0 ;

		// エッジ付きの場合は得られるイメージのサイズを大きくする
		if( ( ManageData->FontType & DX_FONTTYPE_EDGE ) )
		{
			CharData->AddX += ( short )( ManageData->EdgeSize * 2 ) ;
		}
	}
	else
	if( ImageBuffer == NULL )
	{
		CharData->DrawX = 0 ;
		CharData->DrawY = 0 ;
		CharData->AddX  = 0 ;
		CharData->SizeX = 0 ;
		CharData->SizeY = 0 ;
	}
	else
	{
		CharData->DrawX = ( short )( ( ImageDrawX + ( SampleScale >> 2 ) ) / SampleScale ) ;
		CharData->SizeX = ( WORD  )( ( ImageSizeX +   SampleScale - 1    ) / SampleScale ) ;
		CharData->AddX  = ( short )( ( ImageAddX  + ( SampleScale >> 2 ) ) / SampleScale ) ;

		DrawModDrawY    = ( DWORD )( ImageDrawY % SampleScale ) ;
		DataHeight      = ImageSizeY + DrawModDrawY ;
		CharData->DrawY = ( short )(   ImageDrawY                          / SampleScale ) ;
		CharData->SizeY = ( WORD  )( ( DataHeight +   SampleScale - 1    ) / SampleScale ) ;

		// エッジ付きの場合は得られるイメージのサイズを大きくする
		if( ( ManageData->FontType & DX_FONTTYPE_EDGE ) )
		{
			AddX    = ManageData->EdgeSize ;
			AddY    = ManageData->EdgeSize ;
			AddSize = ManageData->EdgeSize * 2 ;
		}
		else
		{
			AddX    = 0 ;
			AddY    = 0 ;
			AddSize = 0 ;
		}

		// 文字を追加する座標のセット
		DestX = CharData->GraphIndexX * ManageData->BaseInfo.MaxWidth ;
		DestY = CharData->GraphIndexY * ManageData->BaseInfo.MaxWidth ;

		// 文字イメージを一時的に保存するメモリ領域を初期化
		if( ManageData->TextureCacheFlag == FALSE )
		{
			_MEMSET(	ManageData->CacheMem + DestY * ManageData->CachePitch,
						0,
						( size_t )( ManageData->CachePitch * ManageData->BaseInfo.MaxWidth ) ) ;
		}
		else
		{
			if( ManageData->TextureCacheBaseImage.ColorData.ColorBitDepth == 8 ||
				ManageData->TextureCacheUsePremulAlpha ||
				( ManageData->FontType & DX_FONTTYPE_EDGE ) != 0 )
			{
				NS_ClearRectBaseImage(
					&ManageData->TextureCacheBaseImage,
					DestX, DestY,
					ManageData->BaseInfo.MaxWidth, ManageData->BaseInfo.MaxWidth
				) ;

				if( ManageData->FontType & DX_FONTTYPE_EDGE )
				{
					NS_ClearRectBaseImage(
						&ManageData->TextureCacheBaseImage,
						DestX + ManageData->CacheImageSize.cx, DestY,
						ManageData->BaseInfo.MaxWidth, ManageData->BaseInfo.MaxWidth
					) ;
				}
			}
			else
			{
				NS_FillRectBaseImage(
					&ManageData->TextureCacheBaseImage,
					DestX, DestY,
					ManageData->BaseInfo.MaxWidth, ManageData->BaseInfo.MaxWidth,
					255,255,255,0
				) ;

				if( ManageData->FontType & DX_FONTTYPE_EDGE )
				{
					NS_FillRectBaseImage(
						&ManageData->TextureCacheBaseImage,
						DestX + ManageData->CacheImageSize.cx, DestY,
						ManageData->BaseInfo.MaxWidth, ManageData->BaseInfo.MaxWidth,
						255,255,255,0
					) ;
				}
			}
		}

		// テクスチャキャッシュを使用する場合は転送先座標を１ドット分ずらす
		if( ManageData->TextureCacheFlag == TRUE )
		{
			DestX += 1 ;
			DestY += 1 ;
		}

		// テクスチャキャッシュを使用しない場合でイメージタイプが DX_FONT_SRCIMAGETYPE_8BIT_MAX64 又は
		// DX_FONT_SRCIMAGETYPE_8BIT_MAX255 だった場合か、イメージタイプが DX_FONT_SRCIMAGETYPE_4BIT_MAX15 だった場合は
		// DX_FONT_SRCIMAGETYPE_8BIT_MAX16 又は DX_FONT_SRCIMAGETYPE_8BIT_MAX64 に変換する
		if( ImageType == DX_FONT_SRCIMAGETYPE_4BIT_MAX15 ||
			( ManageData->TextureCacheFlag == FALSE &&
			  ( ImageType == DX_FONT_SRCIMAGETYPE_8BIT_MAX64 ||
			    ImageType == DX_FONT_SRCIMAGETYPE_8BIT_MAX255 ) ) )
		{
			BYTE *	Src ;
			BYTE *	Dest ;
			DWORD	Height ;
			DWORD	Width ;
			DWORD	i ;
			DWORD	j ;
			DWORD	DestPitch ;

			Width	= CharData->SizeX ;
			Height	= CharData->SizeY ;

			_MEMSET( ManageData->TempBuffer, 0, ManageData->TempBufferSize ) ;

			Src       = ( BYTE * )ImageBuffer ;

			Dest      = ManageData->TempBuffer ;
			DestPitch = Width ;

			switch( ImageType )
			{
			case DX_FONT_SRCIMAGETYPE_4BIT_MAX15 :
				{
					DWORD	ByteNum ;
					DWORD	ModNum ;

					ByteNum = Width / 2 ;
					ModNum  = Width - ByteNum * 2 ;

					// テクスチャキャッシュを使用する場合は分岐
					if( ManageData->TextureCacheFlag )
					{
						for( i = 0 ; i < Height ; i ++ )
						{
							for( j = 0 ; j < ByteNum ; j ++ )
							{
								Dest[ j * 2 + 0 ] = FSYS.MAX15ToMAX64[ ( Src[ j ] & 0xf0 ) >> 4 ] ;
								Dest[ j * 2 + 1 ] = FSYS.MAX15ToMAX64[   Src[ j ] & 0x0f        ] ;
							}

							if( ModNum )
							{
								Dest[ j * 2 ] = FSYS.MAX15ToMAX64[ ( Src[ j ] & 0xf0 ) >> 4 ] ;
							}

							Src  += ImagePitch ;
							Dest += DestPitch ;
						}
						ImageType = DX_FONT_SRCIMAGETYPE_8BIT_MAX64 ;
					}
					else
					{
						for( i = 0 ; i < Height ; i ++ )
						{
							for( j = 0 ; j < ByteNum ; j ++ )
							{
								Dest[ j * 2 + 0 ] = FSYS.MAX15ToMAX16[ ( Src[ j ] & 0xf0 ) >> 4 ] ;
								Dest[ j * 2 + 1 ] = FSYS.MAX15ToMAX16[   Src[ j ] & 0x0f        ] ;
							}

							if( ModNum )
							{
								Dest[ j * 2 ] = FSYS.MAX15ToMAX16[ ( Src[ j ] & 0xf0 ) >> 4 ] ;
							}

							Src  += ImagePitch ;
							Dest += DestPitch ;
						}
						ImageType = DX_FONT_SRCIMAGETYPE_8BIT_MAX16 ;
					}
				}
				break ;

			case DX_FONT_SRCIMAGETYPE_8BIT_MAX64 :
				for( i = 0 ; i < Height ; i ++ )
				{
					for( j = 0 ; j < Width ; j ++ )
					{
						Dest[ j ] = Src[ j ] >> 2 ;
					}

					Src  += ImagePitch ;
					Dest += DestPitch ;
				}
				ImageType   = DX_FONT_SRCIMAGETYPE_8BIT_MAX16 ;
				break ;

			case DX_FONT_SRCIMAGETYPE_8BIT_MAX255 :
				for( i = 0 ; i < Height ; i ++ )
				{
					for( j = 0 ; j < Width ; j ++ )
					{
						Dest[ j ] = FSYS.MAX255ToMAX16[ Src[ j ] ] ;
					}

					Src  += ImagePitch ;
					Dest += DestPitch ;
				}
				ImageType   = DX_FONT_SRCIMAGETYPE_8BIT_MAX16 ;
				break ;
			}

			ImageBuffer = ManageData->TempBuffer ;
			ImagePitch  = DestPitch ;
		}
		else
		// 拡大画像の場合はここで縮小画像を取得する
		if( SampleScale > 1 )
		{
			BYTE *	RDataBuffer ;
			DWORD	RSrcPitch ;
			DWORD	ImageAddPitch ;
			DWORD	ImagePitch2 ;
			DWORD	ImagePitch3 ;
			DWORD	ImagePitch4 ;
			DWORD	ImagePitch5 ;
			DWORD	ImagePitch6 ;
			DWORD	ImagePitch7 ;
			BYTE *	RSrc ;
			BYTE *	RDest ;
			DWORD	RWidth ;
			DWORD	RHeight ;
			DWORD	MHeight ;
			DWORD	HWidth ;
			DWORD	i ;
			DWORD	j ;

			RWidth	= ( CharData->SizeX + 1 ) / 2 * 2 ;
			HWidth	= RWidth / 2 ;
			RHeight	= DataHeight / SampleScale ;
			MHeight	= DataHeight % SampleScale ;

			RSrcPitch = RWidth + 4 ;

//			// 縮小後のデータを格納するメモリを確保
//			ResizeBuffer = ( BYTE * )DXALLOC( ( size_t )( RSrcPitch * ( CharData->SizeY + 2 ) ) ) ;
//			if( ResizeBuffer == NULL )
//			{
//				DxLib_ErrorUTF16LE( "\x87\x65\x57\x5b\xa4\x30\xe1\x30\xfc\x30\xb8\x30\xea\x30\xb5\x30\xf3\x30\xd7\x30\xea\x30\xf3\x30\xb0\x30\x28\x75\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"文字イメージリサンプリング用バッファの確保に失敗しました" @*/ ) ;
//				return -1 ;
//			}
			ResizeBuffer = ManageData->TempBuffer ;
			_MEMSET( ResizeBuffer, 0, ( size_t )( RSrcPitch * ( CharData->SizeY + 2 ) ) ) ;
			RDataBuffer = ResizeBuffer + RSrcPitch ;

			RSrc			= ( BYTE * )ImageBuffer - DrawModDrawY * ImagePitch ;
			RDest			= RDataBuffer ;
			ImageAddPitch	= ( DWORD )( ImagePitch * SampleScale ) ;

			ImagePitch2		= ( DWORD )( ImagePitch * 2 ) ;
			ImagePitch3		= ( DWORD )( ImagePitch * 3 ) ;
			ImagePitch4		= ( DWORD )( ImagePitch * 4 ) ;
			ImagePitch5		= ( DWORD )( ImagePitch * 5 ) ;
			ImagePitch6		= ( DWORD )( ImagePitch * 6 ) ;
			ImagePitch7		= ( DWORD )( ImagePitch * 7 ) ;

			// リサンプルスケールによって処理を分岐
			switch( SampleScale )
			{
				// ４倍の場合
			case 4 :
				if( ManageData->TextureCacheFlag == FALSE || ManageData->TextureCacheColorBitDepth == 16 )
				{
					ImageType = DX_FONT_SRCIMAGETYPE_8BIT_MAX16 ;
					for( i = 0 ; i < RHeight ; i ++ )
					{
						for( j = 0 ; j < HWidth ; j ++ )
						{
							RDest[ j * 2     ] = ( BYTE )( 
								FSYS.BitCountTable[ RSrc[ j               ] & 0xf0 ] +
								FSYS.BitCountTable[ RSrc[ j + ImagePitch  ] & 0xf0 ] +
								FSYS.BitCountTable[ RSrc[ j + ImagePitch2 ] & 0xf0 ] +
								FSYS.BitCountTable[ RSrc[ j + ImagePitch3 ] & 0xf0 ] ) ;

							RDest[ j * 2 + 1 ] = ( BYTE )( 
								FSYS.BitCountTable[ RSrc[ j               ] & 0x0f ] +
								FSYS.BitCountTable[ RSrc[ j + ImagePitch  ] & 0x0f ] +
								FSYS.BitCountTable[ RSrc[ j + ImagePitch2 ] & 0x0f ] +
								FSYS.BitCountTable[ RSrc[ j + ImagePitch3 ] & 0x0f ] ) ;
						}

						RSrc  += ImageAddPitch ;
						RDest += RSrcPitch ;
					}

					if( MHeight != 0 )
					{
						for( i = 0 ; i < MHeight ; i ++ )
						{
							for( j = 0 ; j < HWidth ; j ++ )
							{
								RDest[ j * 2     ] += FSYS.BitCountTable[ RSrc[ j ] & 0xf0 ] ;
								RDest[ j * 2 + 1 ] += FSYS.BitCountTable[ RSrc[ j ] & 0x0f ] ;
							}

							RSrc += ImagePitch ;
						}
					}
				}
				else
				{
					ImageType = DX_FONT_SRCIMAGETYPE_8BIT_MAX64 ;
					for( i = 0 ; i < RHeight ; i ++ )
					{
						for( j = 0 ; j < HWidth ; j ++ )
						{
							RDest[ j * 2     ] = ( BYTE )( 
							  (	FSYS.BitCountTable[ RSrc[ j             ] & 0xf0 ] +
								FSYS.BitCountTable[ RSrc[ j + ImagePitch  ] & 0xf0 ] +
								FSYS.BitCountTable[ RSrc[ j + ImagePitch2 ] & 0xf0 ] +
								FSYS.BitCountTable[ RSrc[ j + ImagePitch3 ] & 0xf0 ] ) << 2 ) ;

							RDest[ j * 2 + 1 ] = ( BYTE )( 
							  (	FSYS.BitCountTable[ RSrc[ j             ] & 0x0f ] +
								FSYS.BitCountTable[ RSrc[ j + ImagePitch  ] & 0x0f ] +
								FSYS.BitCountTable[ RSrc[ j + ImagePitch2 ] & 0x0f ] +
								FSYS.BitCountTable[ RSrc[ j + ImagePitch3 ] & 0x0f ] ) << 2 ) ;
						}

						RSrc  += ImageAddPitch ;
						RDest += RSrcPitch ;
					}

					if( MHeight != 0 )
					{
						for( i = 0 ; i < MHeight ; i ++ )
						{
							for( j = 0 ; j < HWidth ; j ++ )
							{
								RDest[ j * 2     ] += FSYS.BitCountTable[ RSrc[ j ] & 0xf0 ] << 2 ;
								RDest[ j * 2 + 1 ] += FSYS.BitCountTable[ RSrc[ j ] & 0x0f ] << 2 ;
							}

							RSrc += ImagePitch ;
						}
					}
				}
				break;

				// ８倍の場合
			case 8 :
				if( ManageData->TextureCacheFlag == FALSE || ManageData->TextureCacheColorBitDepth == 16 )
				{
					ImageType = DX_FONT_SRCIMAGETYPE_8BIT_MAX16 ;
					for( i = 0 ; i < RHeight ; i ++ )
					{
						for( j = 0 ; j < RWidth ; j ++ )
						{
							RDest[ j ] = ( BYTE )( 
							(	FSYS.BitCountTable[ RSrc[ j ] ] +
								FSYS.BitCountTable[ RSrc[ j + ImagePitch ] ] +
								FSYS.BitCountTable[ RSrc[ j + ImagePitch2 ] ] +
								FSYS.BitCountTable[ RSrc[ j + ImagePitch3 ] ] +
								FSYS.BitCountTable[ RSrc[ j + ImagePitch4 ] ] +
								FSYS.BitCountTable[ RSrc[ j + ImagePitch5 ] ] +
								FSYS.BitCountTable[ RSrc[ j + ImagePitch6 ] ] +
								FSYS.BitCountTable[ RSrc[ j + ImagePitch7 ] ] ) >> 2 ) ;
						}

						RSrc  += ImageAddPitch ;
						RDest += RSrcPitch ;
					}

					if( MHeight != 0 )
					{
						for( i = 0 ; i < MHeight ; i ++ )
						{
							for( j = 0 ; j < RWidth ; j ++ )
							{
								RDest[ j ] += FSYS.BitCountTable[ RSrc[ j ] ] ;
							}

							RSrc += ImagePitch ;
						}
						for( j = 0 ; j < RWidth ; j ++ )
						{
							RDest[ j ] >>= 2 ;
						}
					}
				}
				else
				{
					ImageType = DX_FONT_SRCIMAGETYPE_8BIT_MAX64 ;
					for( i = 0 ; i < RHeight ; i ++ )
					{
						for( j = 0 ; j < RWidth ; j ++ )
						{
							RDest[ j ] = ( BYTE )( 
							(	FSYS.BitCountTable[ RSrc[ j               ] ] +
								FSYS.BitCountTable[ RSrc[ j + ImagePitch  ] ] +
								FSYS.BitCountTable[ RSrc[ j + ImagePitch2 ] ] +
								FSYS.BitCountTable[ RSrc[ j + ImagePitch3 ] ] +
								FSYS.BitCountTable[ RSrc[ j + ImagePitch4 ] ] +
								FSYS.BitCountTable[ RSrc[ j + ImagePitch5 ] ] +
								FSYS.BitCountTable[ RSrc[ j + ImagePitch6 ] ] +
								FSYS.BitCountTable[ RSrc[ j + ImagePitch7 ] ] ) ) ;
						}

						RSrc  += ImageAddPitch ;
						RDest += RSrcPitch ;
					}

					if( MHeight != 0 )
					{
						for( i = 0 ; i < MHeight ; i ++ )
						{
							for( j = 0 ; j < RWidth ; j ++ )
							{
								RDest[ j ] += FSYS.BitCountTable[ RSrc[ j ] ] ;
							}

							RSrc += ImagePitch ;
						}
					}
				}
				break ;
			}

			ImageBuffer = RDataBuffer ;
			ImagePitch  = RSrcPitch ;
		}

		// 縁ありかどうかで処理を分岐
		if( ( ManageData->FontType & DX_FONTTYPE_EDGE ) == 0 )
		{
			BYTE *	Src ;
			BYTE	dat = 0 ;
			DWORD	Height ;
			DWORD	Width ;
			DWORD	i ;
			DWORD	j ;
			DWORD	DestPitch ;

			Width	= CharData->SizeX ;
			Height	= CharData->SizeY ;

			Src		= ( BYTE * )ImageBuffer ;

			if( ManageData->TextureCacheFlag == FALSE )
			{
				BYTE *Dest ;

				DestPitch	= ( DWORD )ManageData->CachePitch ;
				Dest		= ( BYTE * )ManageData->CacheMem + DestY * DestPitch ;

				switch( ManageData->FontType )
				{
				case DX_FONTTYPE_NORMAL :
					switch( ImageType )
					{
					case DX_FONT_SRCIMAGETYPE_1BIT :
						for( i = 0 ; i < Height ; i ++ )
						{
							_MEMCPY( Dest, Src, ( size_t )ImagePitch ) ;

							Src  += ImagePitch ;
							Dest += DestPitch ;
						}
						break ;

					case DX_FONT_SRCIMAGETYPE_8BIT_ON_OFF :
						{
							BYTE *dp ;
							BYTE bit ;

							for( i = 0 ; i < Height ; i ++ )
							{
								dp  = Dest ;
								bit = 0x80 ;
								dat = 0 ;
								for( j = 0 ; j < Width ; j ++, bit >>= 1 )
								{
									if( j != 0 && j % 8 == 0 )
									{
										bit = 0x80 ;
										*dp = dat ;
										dp ++ ;
										dat = 0 ;
									}

									if( Src[ j ] != 0 )
									{
										dat |= bit ;
									}
								}
								*dp = dat ;

								Src  += ImagePitch ;
								Dest += DestPitch ;
							}
						}
						break ;

					default :
						return -1 ;
					}
					break ;

				case DX_FONTTYPE_ANTIALIASING : 
				case DX_FONTTYPE_ANTIALIASING_4X4 : 
				case DX_FONTTYPE_ANTIALIASING_8X8 : 
					// DX_FONT_SRCIMAGETYPE_8BIT_MAX16 以外はエラー
					if( ImageType != DX_FONT_SRCIMAGETYPE_8BIT_MAX16 )
					{
						return -1 ;
					}

					for( i = 0 ; i < Height ; i ++ )
					{
						for( j = 0 ; j < Width ; j ++ )
						{
							if( Src[j] )
							{
								Dest[j] = ( BYTE )( Src[j] - 1 ) ;
							}
						}

						Src  += ImagePitch ;
						Dest += DestPitch ;
					}
					break ;
				}
			}
			else
			{
				BYTE 		*Dest ;
				BASEIMAGE 	&BaseImage 	= ManageData->TextureCacheBaseImage ;
				COLORDATA 	&cl 	    = BaseImage.ColorData ;
				DWORD 		RGBMask ;

				DestPitch	= ( DWORD )BaseImage.Pitch ;
				Dest		= ( BYTE * )BaseImage.GraphData + DestX * BaseImage.ColorData.PixelByte + DestY * BaseImage.Pitch ;

				RGBMask		= cl.RedMask | cl.GreenMask | cl.BlueMask ;

				switch( ImageType )
				{
				case DX_FONT_SRCIMAGETYPE_1BIT :
					if( BaseImage.ColorData.ColorBitDepth == 8 )
					{
						if( BaseImage.ColorData.MaxPaletteNo == 15 )
						{
							for( i = 0 ; i < Height ; i ++ )
							{
								for( j = 0 ; j < Width ; j ++, dat <<= 1 )
								{
									if( ( j & 0x7 ) == 0 )
									{
										dat = Src[ j >> 3 ] ;
									}

									Dest[ j ] = ( dat & 0x80 ) ? 0x0f : 0 ;
								}

								Src  += ImagePitch ;
								Dest += DestPitch ;
							}
						}
						else
						{
							for( i = 0 ; i < Height ; i ++ )
							{
								for( j = 0 ; j < Width ; j ++, dat <<= 1 )
								{
									if( ( j & 0x7 ) == 0 )
									{
										dat = Src[ j >> 3 ] ;
									}

									Dest[ j ] = ( dat & 0x80 ) ? 0xff : 0 ;
								}

								Src  += ImagePitch ;
								Dest += DestPitch ;
							}
						}
					}
					else
					{
						if( ManageData->TextureCacheUsePremulAlpha )
						{
							for( i = 0 ; i < Height ; i ++ )
							{
								for( j = 0 ; j < Width ; j ++, dat <<= 1 )
								{
									if( ( j & 0x7 ) == 0 )
									{
										dat = Src[ j >> 3 ] ;
									}

									if( dat & 0x80 )
									{
										*( ( WORD * )Dest + j ) = ( WORD )( RGBMask | cl.AlphaMask ) ;
									}
									else
									{
										*( ( WORD * )Dest + j ) = 0 ;
									}
								}

								Src  += ImagePitch ;
								Dest += DestPitch ;
							}
						}
						else
						{
							for( i = 0 ; i < Height ; i ++ )
							{
								for( j = 0 ; j < Width ; j ++, dat <<= 1 )
								{
									if( ( j & 0x7 ) == 0 )
									{
										dat = Src[ j >> 3 ] ;
									}

									*( ( WORD * )Dest + j ) = ( WORD )( RGBMask | ( ( dat & 0x80 ) ? cl.AlphaMask : 0 ) ) ;
								}

								Src  += ImagePitch ;
								Dest += DestPitch ;
							}
						}
					}
					break ;

				case DX_FONT_SRCIMAGETYPE_8BIT_ON_OFF :
					if( BaseImage.ColorData.ColorBitDepth == 8 )
					{
						if( BaseImage.ColorData.MaxPaletteNo == 15 )
						{
							for( i = 0 ; i < Height ; i ++ )
							{
								for( j = 0 ; j < Width ; j ++ )
								{
									Dest[ j ] = Src[ j ] != 0 ? 0x0f : 0 ;
								}

								Src  += ImagePitch ;
								Dest += DestPitch ;
							}
						}
						else
						{
							for( i = 0 ; i < Height ; i ++ )
							{
								for( j = 0 ; j < Width ; j ++ )
								{
									Dest[ j ] = Src[ j ] != 0 ? 0xff : 0 ;
								}

								Src  += ImagePitch ;
								Dest += DestPitch ;
							}
						}
					}
					else
					{
						if( cl.ColorBitDepth == 16 )
						{
							WORD 		RGBAMask ;

							RGBAMask = ( WORD )( RGBMask | cl.AlphaMask ) ;

							if( ManageData->TextureCacheUsePremulAlpha )
							{
								for( i = 0 ; i < Height ; i ++ )
								{
									for( j = 0 ; j < Width ; j ++ )
									{
										*( ( WORD * )Dest + j ) = ( WORD )( Src[ j ] != 0 ? RGBAMask : 0 ) ;
									}

									Src  += ImagePitch ;
									Dest += DestPitch ;
								}
							}
							else
							{
								for( i = 0 ; i < Height ; i ++ )
								{
									for( j = 0 ; j < Width ; j ++ )
									{
										*( ( WORD * )Dest + j ) = ( WORD )( Src[ j ] != 0 ? RGBAMask : RGBMask ) ;
									}

									Src  += ImagePitch ;
									Dest += DestPitch ;
								}
							}
						}
					}
					break ;
					
				case DX_FONT_SRCIMAGETYPE_8BIT_MAX16 :
					if( BaseImage.ColorData.ColorBitDepth == 8 )
					{
						if( BaseImage.ColorData.MaxPaletteNo == 15 )
						{
							for( i = 0 ; i < Height ; i ++ )
							{
								for( j = 0 ; j < Width ; j ++ )
								{
									if( Src[ j ] )
									{
										Dest[ j ] = ( BYTE )( Src[ j ] - 1 ) ;
									}
									else
									{
										Dest[ j ] = 0 ;
									}
								}

								Src  += ImagePitch ;
								Dest += DestPitch ;
							}
						}
						else
						{
							for( i = 0 ; i < Height ; i ++ )
							{
								for( j = 0 ; j < Width ; j ++ )
								{
									if( Src[ j ] )
									{
										Dest[ j ] = ( BYTE )( ( Src[ j ] << 4 ) - 1 ) ;
									}
									else
									{
										Dest[ j ] = 0 ;
									}
								}

								Src  += ImagePitch ;
								Dest += DestPitch ;
							}
						}
					}
					else
					{
						if( cl.AlphaMask == 0xf000 &&
							cl.RedMask   == 0x0f00 &&
							cl.GreenMask == 0x00f0 &&
							cl.BlueMask  == 0x000f )
						{
							if( ManageData->TextureCacheUsePremulAlpha )
							{
								for( i = 0 ; i < Height ; i ++ )
								{
									for( j = 0 ; j < Width ; j ++ )
									{
										if( Src[j] == 16 )
										{
											*( (WORD *)Dest + j ) = ( WORD )0xffff ;
										}
										else
										if( Src[j] )
										{
											*( (WORD *)Dest + j ) = ( WORD )( ( Src[j] << 0 ) | ( Src[j] << 4 ) | ( Src[j] << 8 ) | ( Src[j] << 12 ) ) ;
										}
										else
										{
											*( (WORD *)Dest + j ) = (WORD)0x0000 ;
										}
									}

									Src += ImagePitch ;
									Dest += DestPitch ;
								}
							}
							else
							{
								for( i = 0 ; i < Height ; i ++ )
								{
									for( j = 0 ; j < Width ; j ++ )
									{
										if( Src[j] )
										{
											*( (WORD *)Dest + j ) = ( WORD )( ( ( (DWORD)Src[j] << 12 ) - 1 ) | 0x0fff ) ;
										}
										else
										{
											*( (WORD *)Dest + j ) = (WORD)0x0fff ;
										}
									}

									Src  += ImagePitch ;
									Dest += DestPitch ;
								}
							}
						}
						else
						if( cl.ColorBitDepth == 16 )
						{
							if( ManageData->TextureCacheUsePremulAlpha )
							{
								BYTE aloc, rloc, gloc, bloc ;

								aloc = ( BYTE )( cl.AlphaLoc + cl.AlphaWidth - 4 ) ;
								rloc = ( BYTE )( cl.RedLoc   + cl.RedWidth   - 4 ) ;
								gloc = ( BYTE )( cl.GreenLoc + cl.GreenWidth - 4 ) ;
								bloc = ( BYTE )( cl.BlueLoc  + cl.BlueWidth  - 4 ) ;
								for( i = 0 ; i < Height ; i ++ )
								{
									for( j = 0 ; j < Width ; j ++ )
									{
										if( Src[j] == 16 )
										{
											*( (WORD *)Dest + j ) = ( WORD )0xffff ;
										}
										else
										if( Src[j] )
										{
											*( (WORD *)Dest + j ) = ( WORD )( ( Src[j] << aloc ) | ( Src[j] << rloc ) | ( Src[j] << gloc ) | ( Src[j] << bloc ) ) ;
										}
										else
										{
											*( (WORD *)Dest + j ) = (WORD)0x0000 ;
										}
									}

									Src += ImagePitch ;
									Dest += DestPitch ;
								}
							}
							else
							{
								for( i = 0 ; i < Height ; i ++ )
								{
									for( j = 0 ; j < Width ; j ++ )
									{
										if( Src[j] )
										{
											*( (WORD *)Dest + j ) = ( WORD )( ( ( (DWORD)Src[j] << ( ( cl.AlphaLoc + cl.AlphaWidth ) - 4 ) ) - 1 ) | RGBMask ) ;
										}
										else
										{
											*( (WORD *)Dest + j ) = (WORD)RGBMask ;
										}
									}

									Src  += ImagePitch ;
									Dest += DestPitch ;
								}
							}
						}
						else
						if( cl.ColorBitDepth == 32 )
						{
							if( ManageData->TextureCacheUsePremulAlpha )
							{
								BYTE aloc, rloc, gloc, bloc ;

								aloc = ( BYTE )( cl.AlphaLoc + cl.AlphaWidth - 4 ) ;
								rloc = ( BYTE )( cl.RedLoc   + cl.RedWidth   - 4 ) ;
								gloc = ( BYTE )( cl.GreenLoc + cl.GreenWidth - 4 ) ;
								bloc = ( BYTE )( cl.BlueLoc  + cl.BlueWidth  - 4 ) ;
								for( i = 0 ; i < Height ; i ++ )
								{
									for( j = 0 ; j < Width ; j ++ )
									{
										if( Src[j] == 16 )
										{
											*( (DWORD *)Dest + j ) = ( DWORD )0xffffffff ;
										}
										else
										if( Src[j] )
										{
											*( (DWORD *)Dest + j ) = ( DWORD )( ( Src[j] << aloc ) | ( Src[j] << rloc ) | ( Src[j] << gloc ) | ( Src[j] << bloc ) ) ;
										}
										else
										{
											*( (DWORD *)Dest + j ) = (DWORD)0x00000000 ;
										}
									}

									Src  += ImagePitch ;
									Dest += DestPitch ;
								}
							}
							else
							{
								for( i = 0 ; i < Height ; i ++ )
								{
									for( j = 0 ; j < Width ; j ++ )
									{
										if( Src[j] )
										{
											*( (DWORD *)Dest + j ) = ( DWORD )( ( ( (DWORD)Src[j] << ( ( cl.AlphaLoc + cl.AlphaWidth ) - 4 ) ) - 1 ) | RGBMask ) ;
										}
										else
										{
											*( (DWORD *)Dest + j ) = (DWORD)RGBMask ;
										}
									}

									Src  += ImagePitch ;
									Dest += DestPitch ;
								}
							}
						}
					}
					break ;

				case DX_FONT_SRCIMAGETYPE_8BIT_MAX64 :
					if( BaseImage.ColorData.ColorBitDepth == 8 )
					{
						if( BaseImage.ColorData.MaxPaletteNo == 15 )
						{
							for( i = 0 ; i < Height ; i ++ )
							{
								for( j = 0 ; j < Width ; j ++ )
								{
									if( Src[j] > 4 )
									{
										Dest[ j ] = ( BYTE )( ( Src[ j ] >> 2 ) - 1 ) ;
									}
									else
									{
										Dest[ j ] = 0 ;
									}
								}

								Src  += ImagePitch ;
								Dest += DestPitch ;
							}
						}
						else
						{
							for( i = 0 ; i < Height ; i ++ )
							{
								for( j = 0 ; j < Width ; j ++ )
								{
									if( Src[j] )
									{
										Dest[ j ] = ( BYTE )( ( Src[ j ] << 2 ) - 1 ) ;
									}
									else
									{
										Dest[ j ] = 0 ;
									}
								}

								Src  += ImagePitch ;
								Dest += DestPitch ;
							}
						}
					}
					else
					{
						if( ManageData->TextureCacheUsePremulAlpha )
						{
							BYTE aloc, rloc, gloc, bloc ;

							aloc = ( BYTE )( cl.AlphaLoc + cl.AlphaWidth - 6 ) ;
							rloc = ( BYTE )( cl.RedLoc   + cl.RedWidth   - 6 ) ;
							gloc = ( BYTE )( cl.GreenLoc + cl.GreenWidth - 6 ) ;
							bloc = ( BYTE )( cl.BlueLoc  + cl.BlueWidth  - 6 ) ;
							for( i = 0 ; i < Height ; i ++ )
							{
								for( j = 0 ; j < Width ; j ++ )
								{
									if( Src[j] == 64 )
									{
										*( (DWORD *)Dest + j ) = ( DWORD )0xffffffff ;
									}
									else
									if( Src[j] )
									{
										*( (DWORD *)Dest + j ) = ( DWORD )( ( Src[j] << aloc ) | ( Src[j] << rloc ) | ( Src[j] << gloc ) | ( Src[j] << bloc ) ) ;
									}
									else
									{
										*( (DWORD *)Dest + j ) = (DWORD)0x00000000 ;
									}
								}

								Src += ImagePitch ;
								Dest += DestPitch ;
							}
						}
						else
						{
							for( i = 0 ; i < Height ; i ++ )
							{
								for( j = 0 ; j < Width ; j ++ )
								{
									if( Src[j] )
									{
										*( (DWORD *)Dest + j ) = ( DWORD )( ( ( DWORD )( Src[j] - 1 ) << ( ( cl.AlphaLoc + cl.AlphaWidth ) - 6 ) ) | RGBMask ) ;
									}
									else
									{
										*( (DWORD *)Dest + j ) = RGBMask ;
									}
								}

								Src  += ImagePitch ;
								Dest += DestPitch ;
							}
						}
					}
					break ;

				case DX_FONT_SRCIMAGETYPE_8BIT_MAX255 :
					if( BaseImage.ColorData.ColorBitDepth == 8 )
					{
						if( BaseImage.ColorData.MaxPaletteNo == 15 )
						{
							for( i = 0 ; i < Height ; i ++ )
							{
								for( j = 0 ; j < Width ; j ++ )
								{
									Dest[ j ] = ( BYTE )( Src[ j ] >> 4 ) ;
								}

								Src  += ImagePitch ;
								Dest += DestPitch ;
							}
						}
						else
						{
							for( i = 0 ; i < Height ; i ++ )
							{
								for( j = 0 ; j < Width ; j ++ )
								{
									Dest[ j ] = Src[ j ] ;
								}

								Src  += ImagePitch ;
								Dest += DestPitch ;
							}
						}
					}
					else
					{
						if( ManageData->TextureCacheUsePremulAlpha )
						{
							BYTE aloc, rloc, gloc, bloc ;

							aloc = ( BYTE )( cl.AlphaLoc + cl.AlphaWidth - 8 ) ;
							rloc = ( BYTE )( cl.RedLoc   + cl.RedWidth   - 8 ) ;
							gloc = ( BYTE )( cl.GreenLoc + cl.GreenWidth - 8 ) ;
							bloc = ( BYTE )( cl.BlueLoc  + cl.BlueWidth  - 8 ) ;
							for( i = 0 ; i < Height ; i ++ )
							{
								for( j = 0 ; j < Width ; j ++ )
								{
									*( (DWORD *)Dest + j ) = ( DWORD )( ( Src[j] << aloc ) | ( Src[j] << rloc ) | ( Src[j] << gloc ) | ( Src[j] << bloc ) ) ;
								}

								Src += ImagePitch ;
								Dest += DestPitch ;
							}
						}
						else
						{
							for( i = 0 ; i < Height ; i ++ )
							{
								for( j = 0 ; j < Width ; j ++ )
								{
									*( (DWORD *)Dest + j ) = ( DWORD )( ( ( DWORD )Src[ j ] << ( ( cl.AlphaLoc + cl.AlphaWidth ) - 8 ) ) | RGBMask ) ;
								}

								Src  += ImagePitch ;
								Dest += DestPitch ;
							}
						}
					}
					break ;

				default :
					return -1 ;
				}

#ifndef DX_NON_GRAPHICS
				// テクスチャキャッシュに転送
				if( TextureCacheUpdate )
				{
					RECT srect ;

					SETRECT(
						srect,
						DestX - 1,
						DestY - 1,
						ManageData->BaseInfo.MaxWidth + DestX - 1,
						ManageData->BaseInfo.MaxWidth + DestY - 1
					) ;
//					Graphics_Image_BltBmpOrGraphImageToGraph2Base(
//						&BaseImage, NULL,
//						&srect,
//						DestX - 1,
//						DestY - 1,
//						ManageData->TextureCache,
//						GSYS.CreateImage.NotUseTransColor ? FALSE : TRUE
//					) ;
					FontTextureCacheAddUpdateRect( ManageData, &srect ) ;
//					FontTextureCacheUpdateRectApply( ManageData ) ;
				}
#endif // DX_NON_GRAPHICS
			}
		}
		else
		{
			BYTE	*Src ;
			BYTE	dat = 0 ;
			BYTE 	*Dest ;
			BYTE	*DestT ;
			int 	Height ;
			int		Width ;
			int		i ;
			int		j ;
			int		l ;
			int		n ;
			int		m ;
			int		o ;
			int		d ;
			int		EdgeSize ;
			int		DestPitch ;
			unsigned char ( *EdgePat )[ FONTEDGE_PATTERN_NUM * 2 + 1 ] ;
			
			Width 		= ( int )CharData->SizeX ;
			Height 		= ( int )CharData->SizeY ;

			Src			= ( BYTE * )ImageBuffer ;
			EdgeSize	= ManageData->EdgeSize ;
			EdgePat		= _FontEdgePattern[ EdgeSize - 1 ] ;
			
			if( ManageData->TextureCacheFlag == FALSE )
			{
				DestPitch = ManageData->CachePitch ;

				switch( ManageData->FontType )
				{
				case DX_FONTTYPE_EDGE :
					Dest = ( BYTE * )ManageData->CacheMem + ( DestX + AddX ) + ( DestY + AddY ) * DestPitch ;

					switch( ImageType )
					{
					case DX_FONT_SRCIMAGETYPE_1BIT :
						if( EdgeSize == 1 )
						{
							for( i = 0 ; i < Height ; i ++ )
							{
								for( j = 0 ; j < Width ; j ++, dat <<= 1 )
								{
									if( ( j & 0x7 ) == 0 )
									{
										dat = Src[ j / 8 ] ;
									}

									if( dat & 0x80 )
									{
										Dest[j] = 1 ;
										if( Dest[j-1]         != 1 ) Dest[j-1]         = 2 ;
										if( Dest[j+1]         != 1 ) Dest[j+1]         = 2 ;
										if( Dest[j-DestPitch] != 1 ) Dest[j-DestPitch] = 2 ;
										if( Dest[j+DestPitch] != 1 ) Dest[j+DestPitch] = 2 ;
									}
								}
								
								Src += ImagePitch ;
								Dest += DestPitch ;
							}
						}
						else
						if( EdgeSize <= FONTEDGE_PATTERN_NUM )
						{
							for( i = 0 ; i < Height ; i ++ )
							{
								for( j = 0 ; j < Width ; j ++, dat <<= 1 )
								{
									if( ( j % 8 ) == 0 ) dat = Src[ j / 8 ] ;
									if( dat & 0x80 )
									{
										Dest[j] = 1 ;
										for( n = 0, l = -EdgeSize ; l < EdgeSize + 1 ; n ++, l ++ )
										{
											for( o = 0, m = -EdgeSize ; m < EdgeSize + 1 ; o ++, m ++ )
											{
												if( EdgePat[n][o] == 0 ) continue ;
																
												DestT = ( Dest + j + m ) + ( l * DestPitch ) ;
												if( *DestT != 1 ) *DestT = 2 ;
											}
										}
									}
								}
									
								Src  += ImagePitch ;
								Dest += DestPitch ;
							}
						}
						else
						{
							for( i = 0 ; i < Height ; i ++ )
							{
								for( j = 0 ; j < Width ; j ++, dat <<= 1 )
								{
									if( ( j % 8 ) == 0 ) dat = Src[ j / 8 ] ;
									if( dat & 0x80 )
									{
										Dest[j] = 1 ;
										for( l = -EdgeSize ; l < EdgeSize + 1 ; l ++ )
										{
											for( m = -EdgeSize ; m < EdgeSize + 1 ; m ++ )
											{
												DestT = Dest + j + m + l * DestPitch ;
												if( *DestT != 1 ) *DestT = 2 ;
											}
										}
									}
								}
								
								Src  += ImagePitch ;
								Dest += DestPitch ;
							}
						}
						break ;

					case DX_FONT_SRCIMAGETYPE_8BIT_ON_OFF :
						if( EdgeSize == 1 )
						{
							for( i = 0 ; i < Height ; i ++ )
							{
								for( j = 0 ; j < Width ; j ++ )
								{
									if( Src[ j ] != 0 )
									{
										Dest[j] = 1 ;
										if( Dest[j-1]         != 1 ) Dest[j-1]         = 2 ;
										if( Dest[j+1]         != 1 ) Dest[j+1]         = 2 ;
										if( Dest[j-DestPitch] != 1 ) Dest[j-DestPitch] = 2 ;
										if( Dest[j+DestPitch] != 1 ) Dest[j+DestPitch] = 2 ;
									}
								}
								
								Src += ImagePitch ;
								Dest += DestPitch ;
							}
						}
						else
						if( EdgeSize <= FONTEDGE_PATTERN_NUM )
						{
							for( i = 0 ; i < Height ; i ++ )
							{
								for( j = 0 ; j < Width ; j ++ )
								{
									if( Src[ j ] != 0 )
									{
										Dest[j] = 1 ;
										for( n = 0, l = -EdgeSize ; l < EdgeSize + 1 ; n ++, l ++ )
										{
											for( o = 0, m = -EdgeSize ; m < EdgeSize + 1 ; o ++, m ++ )
											{
												if( EdgePat[n][o] == 0 ) continue ;
																
												DestT = ( Dest + j + m ) + ( l * DestPitch ) ;
												if( *DestT != 1 ) *DestT = 2 ;
											}
										}
									}
								}
									
								Src  += ImagePitch ;
								Dest += DestPitch ;
							}
						}
						else
						{
							for( i = 0 ; i < Height ; i ++ )
							{
								for( j = 0 ; j < Width ; j ++ )
								{
									if( Src[ j ] != 0 )
									{
										Dest[j] = 1 ;
										for( l = -EdgeSize ; l < EdgeSize + 1 ; l ++ )
										{
											for( m = -EdgeSize ; m < EdgeSize + 1 ; m ++ )
											{
												DestT = Dest + j + m + l * DestPitch ;
												if( *DestT != 1 ) *DestT = 2 ;
											}
										}
									}
								}
								
								Src  += ImagePitch ;
								Dest += DestPitch ;
							}
						}
						break ;

					default :
						return -1 ;
					}
					break ;


				case DX_FONTTYPE_ANTIALIASING_EDGE :
				case DX_FONTTYPE_ANTIALIASING_EDGE_4X4 :
				case DX_FONTTYPE_ANTIALIASING_EDGE_8X8 :
					// DX_FONT_SRCIMAGETYPE_8BIT_MAX16 以外はエラー
					if( ImageType != DX_FONT_SRCIMAGETYPE_8BIT_MAX16 )
					{
						return -1 ;
					}

					Dest = ( BYTE * )ManageData->CacheMem + DestX + AddX + ( DestY + AddY ) * DestPitch ;

					if( EdgeSize == 1 )
					{
						for( i = 0 ; i < Height ; i ++ )
						{
							for( j = 0 ; j < Width ; j ++ )
							{
								if( Src[j] != 0 )
								{
									d = Src[j] - 1 ;
									Dest[j] |= d ;
									d <<= 4 ;
									if( ( Dest[j - DestPitch] & 0xf0 ) < d ) Dest[j - DestPitch] = ( BYTE )( ( Dest[j - DestPitch] & 0x0f ) | d ) ;
									if( ( Dest[j + DestPitch] & 0xf0 ) < d ) Dest[j + DestPitch] = ( BYTE )( ( Dest[j + DestPitch] & 0x0f ) | d ) ;
									if( ( Dest[j + 1] & 0xf0 )         < d ) Dest[j + 1]         = ( BYTE )( ( Dest[j + 1]         & 0x0f ) | d ) ;
									if( ( Dest[j - 1] & 0xf0 )         < d ) Dest[j - 1]         = ( BYTE )( ( Dest[j - 1]         & 0x0f ) | d ) ;
								}
							}

							Src  += ImagePitch ;
							Dest += DestPitch ;
						}
					}
					else
					if( EdgeSize <= FONTEDGE_PATTERN_NUM )
					{
						for( i = 0 ; i < Height ; i ++ )
						{
							for( j = 0 ; j < Width ; j ++ )
							{
								if( Src[j] != 0 )
								{
									d = Src[j] - 1 ;
									Dest[j] |= d ;
									d <<= 4 ;
									for( n = 0, l = -EdgeSize ; l < EdgeSize + 1 ; n ++, l ++ )
									{
										for( o = 0, m = -EdgeSize ; m < EdgeSize + 1 ; o ++, m ++ )
										{
											if( EdgePat[n][o] == 0 ) continue ;

											DestT = Dest + j + l * DestPitch + m ;
											if( ( *DestT & 0xf0 ) < d ) *DestT = ( BYTE )( ( *DestT & 0x0f ) | d ) ;
										}
									}
								}
							}

							Src += ImagePitch ;
							Dest += DestPitch ;
						}
					}
					else
					{
						for( i = 0 ; i < Height ; i ++ )
						{
							for( j = 0 ; j < Width ; j ++ )
							{
								if( Src[j] != 0 )
								{
									d = Src[j] - 1 ;
									Dest[j] |= d ;
									d <<= 4 ;
									for( l = -EdgeSize ; l < EdgeSize + 1 ; l ++ )
									{
										for( m = -EdgeSize ; m < EdgeSize + 1 ; m ++ )
										{
											DestT = Dest + j + l * DestPitch + m ;
											if( ( *DestT & 0xf0 ) < d ) *DestT = ( BYTE )( ( *DestT & 0x0f ) | d ) ;
										}
									}
								}
							}

							Src += ImagePitch ;
							Dest += DestPitch ;
						}
					}
					break ;
				}
			}
			else
			{
				BYTE 		*DestB ;
				BYTE		datB = 0 ;
				BASEIMAGE 	&im 	= ManageData->TextureCacheBaseImage ;
				COLORDATA 	&cl 	= im.ColorData ;
				DWORD 		RGBMask ;
				DWORD		AlphaRGBMask ;
				DWORD		adp ;

				DestPitch	= im.Pitch ;
				DestB		= ( BYTE * )im.GraphData + im.ColorData.PixelByte * ( AddX + DestX ) + DestPitch * ( AddY + DestY ) ;
				adp			= ( DWORD )( im.Width / 2 ) ;

	#define DB0		*( (BYTE *)dp + j )
	#define DB1		*( (BYTE *)dp + j - DestPitch )
	#define DB2		*( (BYTE *)dp + j + DestPitch )
	#define DB3		*( (BYTE *)dp + j - 1 )
	#define DB4		*( (BYTE *)dp + j + 1 )

	#define EB0		*( (BYTE *)dp + adp + j )
	#define EB1		*( (BYTE *)dp + ( j + adp ) - DestPitch )
	#define EB2		*( (BYTE *)dp + ( j + adp ) + DestPitch )
	#define EB3		*( (BYTE *)dp + adp + j - 1 )
	#define EB4		*( (BYTE *)dp + adp + j + 1 )
	#define EB5		*( (BYTE *)dp + ( j + adp - 1 ) - DestPitch )
	#define EB6		*( (BYTE *)dp + ( j + adp + 1 ) - DestPitch )
	#define EB7		*( (BYTE *)dp + ( j + adp - 1 ) + DestPitch )
	#define EB8		*( (BYTE *)dp + ( j + adp + 1 ) + DestPitch )

	#define D0		*( (WORD *)dp + j )
	#define D1		*( (WORD *)( dp + ( j << 1 ) - DestPitch ) )
	#define D2		*( (WORD *)( dp + ( j << 1 ) + DestPitch ) )
	#define D3		*( (WORD *)dp + j - 1 )
	#define D4		*( (WORD *)dp + j + 1 )

	#define E0		*( (WORD *)dp + adp + j )
	#define E1		*( (WORD *)( dp + ( ( j + adp ) << 1 ) - DestPitch ) )
	#define E2		*( (WORD *)( dp + ( ( j + adp ) << 1 ) + DestPitch ) )
	#define E3		*( (WORD *)dp + adp + j - 1 )
	#define E4		*( (WORD *)dp + adp + j + 1 )
	#define E5		*( (WORD *)( dp + ( ( j + adp - 1 ) << 1 ) - DestPitch ) )
	#define E6		*( (WORD *)( dp + ( ( j + adp + 1 ) << 1 ) - DestPitch ) )
	#define E7		*( (WORD *)( dp + ( ( j + adp - 1 ) << 1 ) + DestPitch ) )
	#define E8		*( (WORD *)( dp + ( ( j + adp + 1 ) << 1 ) + DestPitch ) )

	#define DD0		*( (DWORD *)dp + j )
	#define DD1		*( (DWORD *)( dp + ( j << 2 ) - DestPitch ) )
	#define DD2		*( (DWORD *)( dp + ( j << 2 ) + DestPitch ) )
	#define DD3		*( (DWORD *)dp + j - 1 )
	#define DD4		*( (DWORD *)dp + j + 1 )

	#define ED0		*( (DWORD *)dp + adp + j )
	#define ED1		*( (DWORD *)( dp + ( ( j + adp ) << 2 ) - DestPitch ) )
	#define ED2		*( (DWORD *)( dp + ( ( j + adp ) << 2 ) + DestPitch ) )
	#define ED3		*( (DWORD *)dp + adp + j - 1 )
	#define ED4		*( (DWORD *)dp + adp + j + 1 )
	#define ED5		*( (DWORD *)( dp + ( ( j + adp - 1 ) << 2 ) - DestPitch ) )
	#define ED6		*( (DWORD *)( dp + ( ( j + adp + 1 ) << 2 ) - DestPitch ) )
	#define ED7		*( (DWORD *)( dp + ( ( j + adp - 1 ) << 2 ) + DestPitch ) )
	#define ED8		*( (DWORD *)( dp + ( ( j + adp + 1 ) << 2 ) + DestPitch ) )

				RGBMask = cl.RedMask | cl.GreenMask | cl.BlueMask ;
				AlphaRGBMask = RGBMask | cl.AlphaMask ;
				{
					BYTE	*sp ;
					BYTE	*dp ;
					WORD	*p ;
					BYTE	*pb ;
					DWORD	*pd ;
					
					sp = Src ;
					dp = DestB ;
					
					switch( ImageType )
					{
					case DX_FONT_SRCIMAGETYPE_1BIT :
						if( EdgeSize == 1 )
						{
							if( im.ColorData.ColorBitDepth == 8 )
							{
								for( i = 0 ; i < Height ; i ++ )
								{
									for( j = 0 ; j < Width ; j ++, datB <<= 1 )
									{
										if( ( j & 0x7 ) == 0 ) datB = sp[j>>3] ;
										if( datB & 0x80 )
										{
											DB0 = 1 ;
											EB1 = 1 ;
											EB2 = 1 ;
											EB3 = 1 ;
											EB4 = 1 ;
											EB5 = 1 ;
											EB6 = 1 ;
											EB7 = 1 ;
											EB8 = 1 ;
										}
									}
								
									sp += ImagePitch ;
									dp += DestPitch ;
								}
							}
							else
							{
								for( i = 0 ; i < Height ; i ++ )
								{
									for( j = 0 ; j < Width ; j ++, datB <<= 1 )
									{
										if( ( j & 0x7 ) == 0 ) datB = sp[j>>3] ;
										if( datB & 0x80 )
										{
											D0 = 1 ;
											E1 = 1 ;
											E2 = 1 ;
											E3 = 1 ;
											E4 = 1 ;
											E5 = 1 ;
											E6 = 1 ;
											E7 = 1 ;
											E8 = 1 ;
										}
									}
								
									sp += ImagePitch ;
									dp += DestPitch ;
								}
							}
						}
						else
						if( EdgeSize <= FONTEDGE_PATTERN_NUM )
						{
							if( im.ColorData.ColorBitDepth == 8 )
							{
								for( i = 0 ; i < Height ; i ++ )
								{
									for( j = 0 ; j < Width ; j ++, datB <<= 1 )
									{
										if( ( j % 8 ) == 0 ) datB = sp[j/8] ;
										if( datB & 0x80 )
										{
											DB0 = 1 ;
											for( n = 0, l = -EdgeSize ; l < EdgeSize + 1 ; n ++, l ++ )
											{
												for( o = 0, m = -EdgeSize ; m < EdgeSize + 1 ; o ++, m ++ )
												{
													if( EdgePat[n][o] == 0 ) continue ;
											
													pb = dp + ( j + m + adp ) + ( l * DestPitch ) ;
													if( *pb == 0 ) *pb = 1 ;
												}
											}
										}
									}
								
									sp += ImagePitch ;
									dp += DestPitch ;
								}
							}
							else
							{
								for( i = 0 ; i < Height ; i ++ )
								{
									for( j = 0 ; j < Width ; j ++, datB <<= 1 )
									{
										if( ( j % 8 ) == 0 ) datB = sp[j/8] ;
										if( datB & 0x80 )
										{
											D0 = 1 ;
											for( n = 0, l = -EdgeSize ; l < EdgeSize + 1 ; n ++, l ++ )
											{
												for( o = 0, m = -EdgeSize ; m < EdgeSize + 1 ; o ++, m ++ )
												{
													if( EdgePat[n][o] == 0 ) continue ;
											
													p = (WORD *)( dp + (j + m + adp) * 2 + (l * DestPitch) ) ;
													if( *p == 0 ) *p = 1 ;
												}
											}
										}
									}
								
									sp += ImagePitch ;
									dp += DestPitch ;
								}
							}
						}
						else
						{
							if( im.ColorData.ColorBitDepth == 8 )
							{
								for( i = 0 ; i < Height ; i ++ )
								{
									for( j = 0 ; j < Width ; j ++, datB <<= 1 )
									{
										if( ( j % 8 ) == 0 ) datB = sp[j/8] ;
										if( datB & 0x80 )
										{
											DB0 = 1 ;
											for( l = -EdgeSize ; l < EdgeSize + 1 ; l ++ )
											{
												for( m = -EdgeSize ; m < EdgeSize + 1 ; m ++ )
												{
													pb = dp + ( j + m + adp ) + ( l * DestPitch ) ;
													if( *pb == 0 ) *pb = 1 ;
												}
											}
										}
									}
								
									sp += ImagePitch ;
									dp += DestPitch ;
								}
							}
							else
							{
								for( i = 0 ; i < Height ; i ++ )
								{
									for( j = 0 ; j < Width ; j ++, datB <<= 1 )
									{
										if( ( j % 8 ) == 0 ) datB = sp[j/8] ;
										if( datB & 0x80 )
										{
											D0 = 1 ;
											for( l = -EdgeSize ; l < EdgeSize + 1 ; l ++ )
											{
												for( m = -EdgeSize ; m < EdgeSize + 1 ; m ++ )
												{
													p = (WORD *)( dp + (j + m + adp) * 2 + (l * DestPitch) ) ;
													if( *p == 0 ) *p = 1 ;
												}
											}
										}
									}
								
									sp += ImagePitch ;
									dp += DestPitch ;
								}
							}
						}

						if( im.ColorData.ColorBitDepth == 8 )
						{
							dp = DestB - AddX - AddY * DestPitch ;
							if( im.ColorData.MaxPaletteNo == 15 )
							{
								for( i = 0 ; i < Height + AddSize ; i ++ )
								{
									for( j = 0 ; j < Width + AddSize ; j ++ )
									{
										if( DB0 != 0 )
										{
											DB0 = ( BYTE )0x0f ;
											EB0 = ( BYTE )0x00 ;
										}
										else
										{
											DB0 = ( BYTE )0x00 ;
											EB0 = ( BYTE )( E0 != 0 ? 0x0f : 0x00 ) ;
										}
									}

									dp += DestPitch ;
								}
							}
							else
							{
								for( i = 0 ; i < Height + AddSize ; i ++ )
								{
									for( j = 0 ; j < Width + AddSize ; j ++ )
									{
										if( DB0 != 0 )
										{
											DB0 = ( BYTE )0xff ;
											EB0 = ( BYTE )0x00 ;
										}
										else
										{
											DB0 = ( BYTE )0x00 ;
											EB0 = ( BYTE )( E0 != 0 ? 0xff : 0x00 ) ;
										}
									}

									dp += DestPitch ;
								}
							}
						}
						else
						{
							sp = Src   - AddX     - AddY * ImagePitch ;
							dp = DestB - AddX * 2 - AddY * DestPitch ;
							if( ManageData->TextureCacheUsePremulAlpha )
							{
								for( i = 0 ; i < Height + AddSize ; i ++ )
								{
									for( j = 0 ; j < Width + AddSize ; j ++ )
									{
										if( D0 != 0 )
										{
											D0 = ( WORD )AlphaRGBMask ;
											E0 = ( WORD )0 ;
										}
										else
										{
											D0 = ( WORD )0 ;
											E0 = ( WORD )( E0 != 0 ? AlphaRGBMask : 0 ) ;
										}
									}

									sp += ImagePitch ;
									dp += DestPitch ;
								}
							}
							else
							{
								for( i = 0 ; i < Height + AddSize ; i ++ )
								{
									for( j = 0 ; j < Width + AddSize ; j ++ )
									{
										if( D0 != 0 )
										{
											D0 = ( WORD )AlphaRGBMask ;
											E0 = ( WORD )RGBMask ;
										}
										else
										{
											D0 = ( WORD )RGBMask ;
											E0 = ( WORD )( E0 != 0 ? AlphaRGBMask : RGBMask ) ;
										}
									}

									sp += ImagePitch ;
									dp += DestPitch ;
								}
							}
						}
						break ;

					case DX_FONT_SRCIMAGETYPE_8BIT_ON_OFF :
						if( EdgeSize == 1 )
						{
							if( im.ColorData.ColorBitDepth == 8 )
							{
								for( i = 0 ; i < Height ; i ++ )
								{
									for( j = 0 ; j < Width ; j ++ )
									{
										if( sp[ j ] != 0 )
										{
											DB0 = 1 ;
											EB1 = 1 ;
											EB2 = 1 ;
											EB3 = 1 ;
											EB4 = 1 ;
											EB5 = 1 ;
											EB6 = 1 ;
											EB7 = 1 ;
											EB8 = 1 ;
										}
									}
								
									sp += ImagePitch ;
									dp += DestPitch ;
								}
							}
							else
							{
								for( i = 0 ; i < Height ; i ++ )
								{
									for( j = 0 ; j < Width ; j ++ )
									{
										if( sp[ j ] != 0 )
										{
											D0 = 1 ;
											E1 = 1 ;
											E2 = 1 ;
											E3 = 1 ;
											E4 = 1 ;
											E5 = 1 ;
											E6 = 1 ;
											E7 = 1 ;
											E8 = 1 ;
										}
									}
								
									sp += ImagePitch ;
									dp += DestPitch ;
								}
							}
						}
						else
						if( EdgeSize <= FONTEDGE_PATTERN_NUM )
						{
							if( im.ColorData.ColorBitDepth == 8 )
							{
								for( i = 0 ; i < Height ; i ++ )
								{
									for( j = 0 ; j < Width ; j ++ )
									{
										if( sp[ j ] != 0 )
										{
											DB0 = 1 ;
											for( n = 0, l = -EdgeSize ; l < EdgeSize + 1 ; n ++, l ++ )
											{
												for( o = 0, m = -EdgeSize ; m < EdgeSize + 1 ; o ++, m ++ )
												{
													if( EdgePat[n][o] == 0 ) continue ;
											
													pb = dp + ( j + m + adp ) + ( l * DestPitch ) ;
													if( *pb == 0 ) *pb = 1 ;
												}
											}
										}
									}
								
									sp += ImagePitch ;
									dp += DestPitch ;
								}
							}
							else
							{
								for( i = 0 ; i < Height ; i ++ )
								{
									for( j = 0 ; j < Width ; j ++ )
									{
										if( sp[ j ] != 0 )
										{
											D0 = 1 ;
											for( n = 0, l = -EdgeSize ; l < EdgeSize + 1 ; n ++, l ++ )
											{
												for( o = 0, m = -EdgeSize ; m < EdgeSize + 1 ; o ++, m ++ )
												{
													if( EdgePat[n][o] == 0 ) continue ;
											
													p = (WORD *)( dp + (j + m + adp) * 2 + (l * DestPitch) ) ;
													if( *p == 0 ) *p = 1 ;
												}
											}
										}
									}
								
									sp += ImagePitch ;
									dp += DestPitch ;
								}
							}
						}
						else
						{
							if( im.ColorData.ColorBitDepth == 8 )
							{
								for( i = 0 ; i < Height ; i ++ )
								{
									for( j = 0 ; j < Width ; j ++ )
									{
										if( sp[ j ] != 0 )
										{
											DB0 = 1 ;
											for( l = -EdgeSize ; l < EdgeSize + 1 ; l ++ )
											{
												for( m = -EdgeSize ; m < EdgeSize + 1 ; m ++ )
												{
													pb = dp + ( j + m + adp ) + ( l * DestPitch ) ;
													if( *pb == 0 ) *pb = 1 ;
												}
											}
										}
									}
								
									sp += ImagePitch ;
									dp += DestPitch ;
								}
							}
							else
							{
								for( i = 0 ; i < Height ; i ++ )
								{
									for( j = 0 ; j < Width ; j ++ )
									{
										if( sp[ j ] != 0 )
										{
											D0 = 1 ;
											for( l = -EdgeSize ; l < EdgeSize + 1 ; l ++ )
											{
												for( m = -EdgeSize ; m < EdgeSize + 1 ; m ++ )
												{
													p = (WORD *)( dp + (j + m + adp) * 2 + (l * DestPitch) ) ;
													if( *p == 0 ) *p = 1 ;
												}
											}
										}
									}
								
									sp += ImagePitch ;
									dp += DestPitch ;
								}
							}
						}

						// テクスチャのピクセルフォーマットに変換
						{
							if( im.ColorData.ColorBitDepth == 8 )
							{
								dp = DestB - AddX - AddY * DestPitch  ;

								if( im.ColorData.MaxPaletteNo == 15 )
								{
									for( i = 0 ; i < Height + AddSize ; i ++ )
									{
										for( j = 0 ; j < Width + AddSize ; j ++ )
										{
											if( DB0 != 0 )
											{
												DB0 = 0x0f ;
												EB0 = 0x00 ;
											}
											else
											{
												DB0 = 0x00 ;
												EB0 = EB0 != 0 ? 0x0f : 0x00 ;
											}
										}

										dp += DestPitch ;
									}
								}
								else
								{
									for( i = 0 ; i < Height + AddSize ; i ++ )
									{
										for( j = 0 ; j < Width + AddSize ; j ++ )
										{
											if( DB0 != 0 )
											{
												DB0 = 0xff ;
												EB0 = 0x00 ;
											}
											else
											{
												DB0 = 0x00 ;
												EB0 = EB0 != 0 ? 0xff : 0x00 ;
											}
										}

										dp += DestPitch ;
									}
								}
							}
							else
							{
								sp = Src   - AddX     - AddY * ImagePitch ;
								dp = DestB - AddX * 2 - AddY * DestPitch  ;
								if( ManageData->TextureCacheUsePremulAlpha )
								{
									for( i = 0 ; i < Height + AddSize ; i ++ )
									{
										for( j = 0 ; j < Width + AddSize ; j ++ )
										{
											if( D0 != 0 )
											{
												D0 = ( WORD )AlphaRGBMask ;
												E0 = ( WORD )0 ;
											}
											else
											{
												D0 = ( WORD )0 ;
												E0 = ( WORD )( E0 != 0 ? AlphaRGBMask : 0 ) ;
											}
										}

										sp += ImagePitch ;
										dp += DestPitch ;
									}
								}
								else
								{
									for( i = 0 ; i < Height + AddSize ; i ++ )
									{
										for( j = 0 ; j < Width + AddSize ; j ++ )
										{
											if( D0 != 0 )
											{
												D0 = ( WORD )AlphaRGBMask ;
												E0 = ( WORD )RGBMask ;
											}
											else
											{
												D0 = ( WORD )RGBMask ;
												E0 = ( WORD )( E0 != 0 ? AlphaRGBMask : RGBMask ) ;
											}
										}

										sp += ImagePitch ;
										dp += DestPitch ;
									}
								}
							}
						}
						break ;
						
					case DX_FONT_SRCIMAGETYPE_8BIT_MAX16 :
						{
							DWORD s ;
							
							if( EdgeSize == 1 )
							{
								if( im.ColorData.ColorBitDepth == 8 )
								{
									for( i = 0 ; i < Height ; i ++ )
									{
										for( j = 0 ; j < Width ; j ++ )
										{
											s = (DWORD)sp[j] ;
											if( s > 0 )
											{
												DB0 = (BYTE)s ;
												EB1 = (BYTE)( E1 + ( ( ( 0x10 - E1 ) * s ) >> 4 ) ) ;
												EB2 = (BYTE)( E2 + ( ( ( 0x10 - E2 ) * s ) >> 4 ) ) ;
												EB3 = (BYTE)( E3 + ( ( ( 0x10 - E3 ) * s ) >> 4 ) ) ;
												EB4 = (BYTE)( E4 + ( ( ( 0x10 - E4 ) * s ) >> 4 ) ) ;
												EB5 = (BYTE)( E5 + ( ( ( 0x10 - E5 ) * s ) >> 4 ) ) ;
												EB6 = (BYTE)( E6 + ( ( ( 0x10 - E6 ) * s ) >> 4 ) ) ;
												EB7 = (BYTE)( E7 + ( ( ( 0x10 - E7 ) * s ) >> 4 ) ) ;
												EB8 = (BYTE)( E8 + ( ( ( 0x10 - E8 ) * s ) >> 4 ) ) ;
											}
										}

										sp += ImagePitch ;
										dp += DestPitch ;
									}
								}
								else
								{
									for( i = 0 ; i < Height ; i ++ )
									{
										for( j = 0 ; j < Width ; j ++ )
										{
											s = (DWORD)sp[j] ;
											if( s > 0 )
											{
												D0 = (WORD)s ;
												E1 = (WORD)( E1 + ( ( ( 0x10 - E1 ) * s ) >> 4 ) ) ;
												E2 = (WORD)( E2 + ( ( ( 0x10 - E2 ) * s ) >> 4 ) ) ;
												E3 = (WORD)( E3 + ( ( ( 0x10 - E3 ) * s ) >> 4 ) ) ;
												E4 = (WORD)( E4 + ( ( ( 0x10 - E4 ) * s ) >> 4 ) ) ;
												E5 = (WORD)( E5 + ( ( ( 0x10 - E5 ) * s ) >> 4 ) ) ;
												E6 = (WORD)( E6 + ( ( ( 0x10 - E6 ) * s ) >> 4 ) ) ;
												E7 = (WORD)( E7 + ( ( ( 0x10 - E7 ) * s ) >> 4 ) ) ;
												E8 = (WORD)( E8 + ( ( ( 0x10 - E8 ) * s ) >> 4 ) ) ;
											}
										}

										sp += ImagePitch ;
										dp += DestPitch ;
									}
								}
							}
							else
							if( EdgeSize <= FONTEDGE_PATTERN_NUM )
							{
								if( im.ColorData.ColorBitDepth == 8 )
								{
									for( i = 0 ; i < Height ; i ++ )
									{
										for( j = 0 ; j < Width ; j ++ )
										{
											s = (DWORD)sp[j] ;
											if( s > 0 )
											{
												DB0 = (BYTE)s ;
												for( n = 0, l = -EdgeSize ; l < EdgeSize + 1 ; n ++, l ++ )
												{
													for( o = 0, m = -EdgeSize ; m < EdgeSize + 1 ; o ++, m ++ )
													{
														if( EdgePat[n][o] == 0 ) continue ;
														pb  = (BYTE *)( dp + (j + m + adp) + (l * DestPitch) ) ;
														*pb = (BYTE)( ( 0x100 - ( ( 0x10 - *pb ) * ( 0x10 - s ) ) ) >> 4 ) ;
													}
												}
											}
										}

										sp += ImagePitch ;
										dp += DestPitch ;
									}
								}
								else
								{
									for( i = 0 ; i < Height ; i ++ )
									{
										for( j = 0 ; j < Width ; j ++ )
										{
											s = (DWORD)sp[j] ;
											if( s > 0 )
											{
												D0 = (WORD)s ;
												for( n = 0, l = -EdgeSize ; l < EdgeSize + 1 ; n ++, l ++ )
												{
													for( o = 0, m = -EdgeSize ; m < EdgeSize + 1 ; o ++, m ++ )
													{
														if( EdgePat[n][o] == 0 ) continue ;
														p = (WORD *)( dp + (j + m + adp) * 2 + (l * DestPitch) ) ;
														*p = (WORD)( ( 0x100 - ( ( 0x10 - *p ) * ( 0x10 - s ) ) ) >> 4 ) ;
													}
												}
											}
										}

										sp += ImagePitch ;
										dp += DestPitch ;
									}
								}
							}
							else
							{
								if( im.ColorData.ColorBitDepth == 8 )
								{
									for( i = 0 ; i < Height ; i ++ )
									{
										for( j = 0 ; j < Width ; j ++ )
										{
											s = (DWORD)sp[j] ;
											if( s > 0 )
											{
												DB0 = (BYTE)s ;
												for( l = -EdgeSize ; l < EdgeSize + 1 ; l ++ )
												{
													for( m = -EdgeSize ; m < EdgeSize + 1 ; m ++ )
													{
														pb  = (BYTE *)( dp + (j + m + adp) + (l * DestPitch) ) ;
														*pb = (BYTE)( ( 0x100 - ( ( 0x10 - *pb ) * ( 0x10 - s ) ) ) >> 4 ) ;
													}
												}
											}
										}

										sp += ImagePitch ;
										dp += DestPitch ;
									}
								}
								else
								{
									for( i = 0 ; i < Height ; i ++ )
									{
										for( j = 0 ; j < Width ; j ++ )
										{
											s = (DWORD)sp[j] ;
											if( s > 0 )
											{
												D0 = (WORD)s ;
												for( l = -EdgeSize ; l < EdgeSize + 1 ; l ++ )
												{
													for( m = -EdgeSize ; m < EdgeSize + 1 ; m ++ )
													{
														p = (WORD *)( dp + (j + m + adp) * 2 + (l * DestPitch) ) ;
														*p = (WORD)( ( 0x100 - ( ( 0x10 - *p ) * ( 0x10 - s ) ) ) >> 4 ) ;
													}
												}
											}
										}

										sp += ImagePitch ;
										dp += DestPitch ;
									}
								}
							}

							// テクスチャのピクセルフォーマットに変換
							{
								if( im.ColorData.ColorBitDepth == 8 )
								{
									dp = DestB - AddX - AddY * DestPitch  ;
									if( im.ColorData.MaxPaletteNo == 15 )
									{
										for( i = 0 ; i < Height + AddSize ; i ++ )
										{
											for( j = 0 ; j < Width + AddSize ; j ++ )
											{
												if( DB0 == 16 )
												{
													DB0 = 0x0f ;
													EB0 = 0x00 ;
												}
												else
												{
													if( DB0 )
													{
														DB0 = ( BYTE )( DB0 - 1 ) ;
													}
													else
													{
														DB0 = 0x00 ;
													}

													if( EB0 )
													{
														EB0 = ( BYTE )( EB0 - 1 ) ;
													}
													else
													{
														EB0 = 0x00 ;
													}
												}
											}

											dp += DestPitch ;
										}
									}
									else
									{
										for( i = 0 ; i < Height + AddSize ; i ++ )
										{
											for( j = 0 ; j < Width + AddSize ; j ++ )
											{
												if( DB0 == 16 )
												{
													DB0 = 0xff ;
													EB0 = 0x00 ;
												}
												else
												{
													if( DB0 )
													{
														DB0 = ( BYTE )( ( DB0 << 4 ) - 1 ) ;
													}
													else
													{
														DB0 = 0x00 ;
													}

													if( EB0 )
													{
														EB0 = ( BYTE )( ( EB0 << 4 ) - 1 ) ;
													}
													else
													{
														EB0 = 0x00 ;
													}
												}
											}

											dp += DestPitch ;
										}
									}
								}
								else
								{
									BYTE aloc, rloc, gloc, bloc ;

									aloc = ( BYTE )( ( cl.AlphaLoc + cl.AlphaWidth ) - 4 ) ;
									rloc = ( BYTE )( ( cl.RedLoc   + cl.RedWidth   ) - 4 ) ;
									gloc = ( BYTE )( ( cl.GreenLoc + cl.GreenWidth ) - 4 ) ;
									bloc = ( BYTE )( ( cl.BlueLoc  + cl.BlueWidth  ) - 4 ) ;

									sp = Src   - AddX     - AddY * ImagePitch ;
									dp = DestB - AddX * 2 - AddY * DestPitch  ;
									if( ManageData->TextureCacheUsePremulAlpha )
									{
										for( i = 0 ; i < Height + AddSize ; i ++ )
										{
											for( j = 0 ; j < Width + AddSize ; j ++ )
											{
												if( D0 == 16 )
												{
													D0 = ( WORD )0xffff ;
													E0 = 0 ;
												}
												else
												{
													if( D0 )
													{
														D0 = ( WORD )( ( (DWORD)D0 << aloc ) | ( (DWORD)D0 << rloc ) | ( (DWORD)D0 << gloc ) | ( (DWORD)D0 << bloc ) ) ;
													}

													if( E0 == 16 )
													{
														E0 = ( WORD )0xffff ;
													}
													else
													if( E0 )
													{
														E0 = ( WORD )( ( (DWORD)E0 << aloc ) | ( (DWORD)E0 << rloc ) | ( (DWORD)E0 << gloc ) | ( (DWORD)E0 << bloc ) ) ;
													}
												}
											}

											sp += ImagePitch ;
											dp += DestPitch ;
										}
									}
									else
									{
										for( i = 0 ; i < Height + AddSize ; i ++ )
										{
											for( j = 0 ; j < Width + AddSize ; j ++ )
											{
												if( D0 == 16 )
												{
													D0 = ( WORD )( ( ( (DWORD)D0 << aloc ) - 1 ) | RGBMask ) ;
													E0 = (WORD)RGBMask ;
												}
												else
												{
													if( D0 )
													{
														D0 = ( WORD )( ( ( (DWORD)D0 << aloc ) - 1 ) | RGBMask ) ;
													}
													else
													{
														D0 = (WORD)RGBMask ;
													}

													if( E0 )
													{
														E0 = ( WORD )( ( ( (DWORD)E0 << aloc ) - 1 ) | RGBMask ) ;
													}
													else
													{
														E0 = (WORD)RGBMask ;
													}
												}
											}

											sp += ImagePitch ;
											dp += DestPitch ;
										}
									}
								}
							}
						}
						break ;

					case DX_FONT_SRCIMAGETYPE_8BIT_MAX64 :
						{
							DWORD s ;
							
							if( EdgeSize == 1 )
							{
								if( im.ColorData.ColorBitDepth == 8 )
								{
									for( i = 0 ; i < Height ; i ++ )
									{
										for( j = 0 ; j < Width ; j ++ )
										{
											s = (DWORD)sp[j] ;
											if( s > 0 )
											{
												DB0 = (BYTE)s ;
												EB1 = (BYTE)( EB1 + ( ( ( 0x40 - EB1 ) * s ) >> 6 ) ) ;
												EB2 = (BYTE)( EB2 + ( ( ( 0x40 - EB2 ) * s ) >> 6 ) ) ;
												EB3 = (BYTE)( EB3 + ( ( ( 0x40 - EB3 ) * s ) >> 6 ) ) ;
												EB4 = (BYTE)( EB4 + ( ( ( 0x40 - EB4 ) * s ) >> 6 ) ) ;
												EB5 = (BYTE)( EB5 + ( ( ( 0x40 - EB5 ) * s ) >> 6 ) ) ;
												EB6 = (BYTE)( EB6 + ( ( ( 0x40 - EB6 ) * s ) >> 6 ) ) ;
												EB7 = (BYTE)( EB7 + ( ( ( 0x40 - EB7 ) * s ) >> 6 ) ) ;
												EB8 = (BYTE)( EB8 + ( ( ( 0x40 - EB8 ) * s ) >> 6 ) ) ;
											}
										}

										sp += ImagePitch ;
										dp += DestPitch ;
									}
								}
								else
								{
									for( i = 0 ; i < Height ; i ++ )
									{
										for( j = 0 ; j < Width ; j ++ )
										{
											s = (DWORD)sp[j] ;
											if( s > 0 )
											{
												DD0 = (DWORD)s ;
												ED1 = (DWORD)( ED1 + ( ( ( 0x40 - ED1 ) * s ) >> 6 ) ) ;
												ED2 = (DWORD)( ED2 + ( ( ( 0x40 - ED2 ) * s ) >> 6 ) ) ;
												ED3 = (DWORD)( ED3 + ( ( ( 0x40 - ED3 ) * s ) >> 6 ) ) ;
												ED4 = (DWORD)( ED4 + ( ( ( 0x40 - ED4 ) * s ) >> 6 ) ) ;
												ED5 = (DWORD)( ED5 + ( ( ( 0x40 - ED5 ) * s ) >> 6 ) ) ;
												ED6 = (DWORD)( ED6 + ( ( ( 0x40 - ED6 ) * s ) >> 6 ) ) ;
												ED7 = (DWORD)( ED7 + ( ( ( 0x40 - ED7 ) * s ) >> 6 ) ) ;
												ED8 = (DWORD)( ED8 + ( ( ( 0x40 - ED8 ) * s ) >> 6 ) ) ;
											}
										}

										sp += ImagePitch ;
										dp += DestPitch ;
									}
								}
							}
							else
							if( EdgeSize <= FONTEDGE_PATTERN_NUM )
							{
								if( im.ColorData.ColorBitDepth == 8 )
								{
									for( i = 0 ; i < Height ; i ++ )
									{
										for( j = 0 ; j < Width ; j ++ )
										{
											s = (DWORD)sp[j] ;
											if( s > 0 )
											{
												DB0 = (BYTE)s ;
												for( n = 0, l = -EdgeSize ; l < EdgeSize + 1 ; n ++, l ++ )
												{
													for( o = 0, m = -EdgeSize ; m < EdgeSize + 1 ; o ++, m ++ )
													{
														if( EdgePat[n][o] == 0 ) continue ;
														pb  = (BYTE *)( dp + (j + m + adp) + (l * DestPitch) ) ;
														*pb = (BYTE)( *pb + ( ( ( 0x40 - *pb ) * s ) >> 6 ) ) ;
													}
												}
											}
										}

										sp += ImagePitch ;
										dp += DestPitch ;
									}
								}
								else
								{
									for( i = 0 ; i < Height ; i ++ )
									{
										for( j = 0 ; j < Width ; j ++ )
										{
											s = (DWORD)sp[j] ;
											if( s > 0 )
											{
												DD0 = (DWORD)s ;
												for( n = 0, l = -EdgeSize ; l < EdgeSize + 1 ; n ++, l ++ )
												{
													for( o = 0, m = -EdgeSize ; m < EdgeSize + 1 ; o ++, m ++ )
													{
														if( EdgePat[n][o] == 0 ) continue ;
														pd = (DWORD *)( dp + (j + m + adp) * 4 + (l * DestPitch) ) ;
														*pd = (DWORD)( *pd + ( ( ( 0x40 - *pd ) * s ) >> 6 ) ) ;
													}
												}
											}
										}

										sp += ImagePitch ;
										dp += DestPitch ;
									}
								}
							}
							else
							{
								if( im.ColorData.ColorBitDepth == 8 )
								{
									for( i = 0 ; i < Height ; i ++ )
									{
										for( j = 0 ; j < Width ; j ++ )
										{
											s = (DWORD)sp[j] ;
											if( s > 0 )
											{
												DB0 = (BYTE)s ;
												for( l = -EdgeSize ; l < EdgeSize + 1 ; l ++ )
												{
													for( m = -EdgeSize ; m < EdgeSize + 1 ; m ++ )
													{
														pb  = (BYTE *)( dp + (j + m + adp) + (l * DestPitch) ) ;
														*pb = (BYTE)( *pb + ( ( ( 0x40 - *pb ) * s ) >> 6 ) ) ;
													}
												}
											}
										}

										sp += ImagePitch ;
										dp += DestPitch ;
									}
								}
								else
								{
									for( i = 0 ; i < Height ; i ++ )
									{
										for( j = 0 ; j < Width ; j ++ )
										{
											s = (DWORD)sp[j] ;
											if( s > 0 )
											{
												DD0 = (DWORD)s ;
												for( l = -EdgeSize ; l < EdgeSize + 1 ; l ++ )
												{
													for( m = -EdgeSize ; m < EdgeSize + 1 ; m ++ )
													{
														pd = (DWORD *)( dp + (j + m + adp) * 4 + (l * DestPitch) ) ;
														*pd = (DWORD)( *pd + ( ( ( 0x40 - *pd ) * s ) >> 6 ) ) ;
													}
												}
											}
										}

										sp += ImagePitch ;
										dp += DestPitch ;
									}
								}
							}

							// テクスチャのピクセルフォーマットに変換
							{
								static BYTE NumConvTable[ 0x41 ] =
								{
									  0,   3,   7,  11,  15,  19,  23,  27,  31,  35,  39,  43,  47,  51,  55,  59, 
									 63,  67,  71,  75,  79,  83,  87,  91,  95,  99, 103, 107, 111, 115, 119, 123, 
									127, 131, 135, 139, 143, 147, 151, 155, 159, 163, 167, 171, 175, 179, 183, 187, 
									191, 195, 199, 203, 207, 211, 215, 219, 223, 227, 231, 235, 239, 243, 247, 251, 
									255, 
								} ;

								if( im.ColorData.ColorBitDepth == 8 )
								{
									dp = DestB - AddX - AddY * DestPitch  ;

									if( im.ColorData.MaxPaletteNo == 15 )
									{
										for( i = 0 ; i < Height + AddSize ; i ++ )
										{
											for( j = 0 ; j < Width + AddSize ; j ++ )
											{
												if( DB0 == 64 )
												{
													DB0 = 0x0f ;
													EB0 = 0x00 ;
												}
												else
												{
													if( DB0 )
													{
														DB0 = ( BYTE )( DB0 >> 2 ) ;
													}
													else
													{
														DB0 = 0x00 ;
													}

													if( EB0 )
													{
														EB0 = ( BYTE )( EB0 >> 2 ) ;
													}
													else
													{
														EB0 = 0x00 ;
													}
												}
											}

											dp += DestPitch ;
										}
									}
									else
									{
										for( i = 0 ; i < Height + AddSize ; i ++ )
										{
											for( j = 0 ; j < Width + AddSize ; j ++ )
											{
												DB0 = ( BYTE )NumConvTable[ DB0 ] ;
												EB0 = ( BYTE )NumConvTable[ EB0 ] ;
											}

											dp += DestPitch ;
										}
									}
								}
								else
								{
									sp = Src   - AddX     - AddY * ImagePitch ;
									dp = DestB - AddX * 4 - AddY * DestPitch  ;
									if( ManageData->TextureCacheUsePremulAlpha )
									{
										DWORD tmp ;

										for( i = 0 ; i < Height + AddSize ; i ++ )
										{
											for( j = 0 ; j < Width + AddSize ; j ++ )
											{
												if( DD0 == 64 )
												{
													DD0 = 0xffffffff ;
													ED0 = 0 ;
												}
												else
												{
													if( DD0 )
													{
														tmp = NumConvTable[ DD0 ] ;
														DD0 = ( DWORD )( ( tmp << cl.AlphaLoc ) | ( tmp << cl.RedLoc ) | ( tmp << cl.GreenLoc ) | ( tmp << cl.BlueLoc ) ) ;
													}

													if( ED0 == 64 )
													{
														ED0 = 0xffffffff ;
													}
													else
													if( ED0 )
													{
														tmp = NumConvTable[ ED0 ] ;
														ED0 = ( DWORD )( ( tmp << cl.AlphaLoc ) | ( tmp << cl.RedLoc ) | ( tmp << cl.GreenLoc ) | ( tmp << cl.BlueLoc ) ) ;
													}
												}
											}

											sp += ImagePitch ;
											dp += DestPitch ;
										}
									}
									else
									{
										for( i = 0 ; i < Height + AddSize ; i ++ )
										{
											for( j = 0 ; j < Width + AddSize ; j ++ )
											{
												if( DD0 == 64 )
												{
													DD0 = AlphaRGBMask ;
													ED0 = (DWORD)RGBMask ;
												}
												else
												{
													if( DD0 )
													{
														DD0 = ( DWORD )( ( ( DWORD )NumConvTable[ DD0 ] << cl.AlphaLoc ) | RGBMask ) ;
													}
													else
													{
														DD0 = (DWORD)RGBMask ;
													}

													if( ED0 )
													{
														ED0 = ( DWORD )( ( ( DWORD )NumConvTable[ ED0 ] << cl.AlphaLoc ) | RGBMask ) ;
													}
													else
													{
														ED0 = (DWORD)RGBMask ;
													}
												}
											}

											sp += ImagePitch ;
											dp += DestPitch ;
										}
									}
								}
							}
						}
						break ;

					case DX_FONT_SRCIMAGETYPE_8BIT_MAX255 :
						{
							DWORD s ;
							
							if( EdgeSize == 1 )
							{
								if( im.ColorData.ColorBitDepth == 8 )
								{
									for( i = 0 ; i < Height ; i ++ )
									{
										for( j = 0 ; j < Width ; j ++ )
										{
											s = (DWORD)sp[j] ;
											if( s > 0 )
											{
												DB0 = (BYTE)s ;
												EB1 = (BYTE)( EB1 + ( ( ( 0xff - EB1 ) * s ) >> 8 ) ) ;
												EB2 = (BYTE)( EB2 + ( ( ( 0xff - EB2 ) * s ) >> 8 ) ) ;
												EB3 = (BYTE)( EB3 + ( ( ( 0xff - EB3 ) * s ) >> 8 ) ) ;
												EB4 = (BYTE)( EB4 + ( ( ( 0xff - EB4 ) * s ) >> 8 ) ) ;
												EB5 = (BYTE)( EB5 + ( ( ( 0xff - EB5 ) * s ) >> 8 ) ) ;
												EB6 = (BYTE)( EB6 + ( ( ( 0xff - EB6 ) * s ) >> 8 ) ) ;
												EB7 = (BYTE)( EB7 + ( ( ( 0xff - EB7 ) * s ) >> 8 ) ) ;
												EB8 = (BYTE)( EB8 + ( ( ( 0xff - EB8 ) * s ) >> 8 ) ) ;
											}
										}

										sp += ImagePitch ;
										dp += DestPitch ;
									}
								}
								else
								{
									for( i = 0 ; i < Height ; i ++ )
									{
										for( j = 0 ; j < Width ; j ++ )
										{
											s = (DWORD)sp[j] ;
											if( s > 0 )
											{
												DD0 = (DWORD)s ;
												ED1 = (DWORD)( ED1 + ( ( ( 0xff - ED1 ) * s ) >> 8 ) ) ;
												ED2 = (DWORD)( ED2 + ( ( ( 0xff - ED2 ) * s ) >> 8 ) ) ;
												ED3 = (DWORD)( ED3 + ( ( ( 0xff - ED3 ) * s ) >> 8 ) ) ;
												ED4 = (DWORD)( ED4 + ( ( ( 0xff - ED4 ) * s ) >> 8 ) ) ;
												ED5 = (DWORD)( ED5 + ( ( ( 0xff - ED5 ) * s ) >> 8 ) ) ;
												ED6 = (DWORD)( ED6 + ( ( ( 0xff - ED6 ) * s ) >> 8 ) ) ;
												ED7 = (DWORD)( ED7 + ( ( ( 0xff - ED7 ) * s ) >> 8 ) ) ;
												ED8 = (DWORD)( ED8 + ( ( ( 0xff - ED8 ) * s ) >> 8 ) ) ;
											}
										}

										sp += ImagePitch ;
										dp += DestPitch ;
									}
								}
							}
							else
							if( EdgeSize <= FONTEDGE_PATTERN_NUM )
							{
								if( im.ColorData.ColorBitDepth == 8 )
								{
									for( i = 0 ; i < Height ; i ++ )
									{
										for( j = 0 ; j < Width ; j ++ )
										{
											s = (DWORD)sp[j] ;
											if( s > 0 )
											{
												DB0 = (BYTE)s ;
												for( n = 0, l = -EdgeSize ; l < EdgeSize + 1 ; n ++, l ++ )
												{
													for( o = 0, m = -EdgeSize ; m < EdgeSize + 1 ; o ++, m ++ )
													{
														if( EdgePat[n][o] == 0 ) continue ;
														pb  = (BYTE *)( dp + (j + m + adp) + (l * DestPitch) ) ;
														*pb = (BYTE )( *pb + ( ( ( 0xff - *pb ) * s ) >> 8 ) ) ;
													}
												}
											}
										}

										sp += ImagePitch ;
										dp += DestPitch ;
									}
								}
								else
								{
									for( i = 0 ; i < Height ; i ++ )
									{
										for( j = 0 ; j < Width ; j ++ )
										{
											s = (DWORD)sp[j] ;
											if( s > 0 )
											{
												DD0 = (DWORD)s ;
												for( n = 0, l = -EdgeSize ; l < EdgeSize + 1 ; n ++, l ++ )
												{
													for( o = 0, m = -EdgeSize ; m < EdgeSize + 1 ; o ++, m ++ )
													{
														if( EdgePat[n][o] == 0 ) continue ;
														pd = (DWORD *)( dp + (j + m + adp) * 4 + (l * DestPitch) ) ;
														*pd = (DWORD)( *pd + ( ( ( 0xff - *pd ) * s ) >> 8 ) ) ;
													}
												}
											}
										}

										sp += ImagePitch ;
										dp += DestPitch ;
									}
								}
							}
							else
							{
								if( im.ColorData.ColorBitDepth == 8 )
								{
									for( i = 0 ; i < Height ; i ++ )
									{
										for( j = 0 ; j < Width ; j ++ )
										{
											s = (DWORD)sp[j] ;
											if( s > 0 )
											{
												DB0 = (BYTE)s ;
												for( l = -EdgeSize ; l < EdgeSize + 1 ; l ++ )
												{
													for( m = -EdgeSize ; m < EdgeSize + 1 ; m ++ )
													{
														pb  = (BYTE *)( dp + (j + m + adp) + (l * DestPitch) ) ;
														*pb = (BYTE)( *pb + ( ( ( 0xff - *pb ) * s ) >> 8 ) ) ;
													}
												}
											}
										}

										sp += ImagePitch ;
										dp += DestPitch ;
									}
								}
								else
								{
									for( i = 0 ; i < Height ; i ++ )
									{
										for( j = 0 ; j < Width ; j ++ )
										{
											s = (DWORD)sp[j] ;
											if( s > 0 )
											{
												DD0 = (DWORD)s ;
												for( l = -EdgeSize ; l < EdgeSize + 1 ; l ++ )
												{
													for( m = -EdgeSize ; m < EdgeSize + 1 ; m ++ )
													{
														pd = (DWORD *)( dp + (j + m + adp) * 4 + (l * DestPitch) ) ;
														*pd = (DWORD)( *pd + ( ( ( 0xff - *pd ) * s ) >> 8 ) ) ;
													}
												}
											}
										}

										sp += ImagePitch ;
										dp += DestPitch ;
									}
								}
							}

							// テクスチャのピクセルフォーマットに変換
							{
								if( im.ColorData.ColorBitDepth == 8 )
								{
									dp = DestB - AddX * 4 - AddY * DestPitch ;
									if( im.ColorData.MaxPaletteNo == 15 )
									{
										for( i = 0 ; i < Height + AddSize ; i ++ )
										{
											for( j = 0 ; j < Width + AddSize ; j ++ )
											{
												DB0 = ( BYTE )( DB0 >> 4 ) ;
												EB0 = ( BYTE )( EB0 >> 4 ) ;
											}

											dp += DestPitch ;
										}
									}
									else
									{
									}
								}
								else
								{
									sp = Src - AddX - AddY * ImagePitch ;
									dp = DestB - AddX * 4 - AddY * DestPitch ;
									if( ManageData->TextureCacheUsePremulAlpha )
									{
										DWORD tmp ;

										for( i = 0 ; i < Height + AddSize ; i ++ )
										{
											for( j = 0 ; j < Width + AddSize ; j ++ )
											{
												tmp = DD0 ;
												DD0 = ( DWORD )( ( tmp << cl.AlphaLoc ) | ( tmp << cl.RedLoc ) | ( tmp << cl.GreenLoc ) | ( tmp << cl.BlueLoc ) ) ;

												tmp = ED0 ;
												ED0 = ( DWORD )( ( tmp << cl.AlphaLoc ) | ( tmp << cl.RedLoc ) | ( tmp << cl.GreenLoc ) | ( tmp << cl.BlueLoc ) ) ;
											}

											sp += ImagePitch ;
											dp += DestPitch ;
										}
									}
									else
									{
										for( i = 0 ; i < Height + AddSize ; i ++ )
										{
											for( j = 0 ; j < Width + AddSize ; j ++ )
											{
												DD0 = ( DWORD )( ( ( DWORD )DD0 << cl.AlphaLoc ) | RGBMask ) ;
												ED0 = ( DWORD )( ( ( DWORD )ED0 << cl.AlphaLoc ) | RGBMask ) ;
											}

											sp += ImagePitch ;
											dp += DestPitch ;
										}
									}
								}
							}
						}
						break ;
					}
				}
	#undef ED0
	#undef ED1
	#undef ED2
	#undef ED3
	#undef ED4
	#undef ED5
	#undef ED6
	#undef ED7
	#undef ED8

	#undef DD0
	#undef DD1
	#undef DD2
	#undef DD3
	#undef DD4

	#undef E0
	#undef E1
	#undef E2
	#undef E3
	#undef E4
	#undef E5
	#undef E6
	#undef E7
	#undef E8

	#undef D0
	#undef D1
	#undef D2
	#undef D3
	#undef D4

	#undef EB0
	#undef EB1
	#undef EB2
	#undef EB3
	#undef EB4
	#undef EB5
	#undef EB6
	#undef EB7
	#undef EB8

	#undef DB0
	#undef DB1
	#undef DB2
	#undef DB3
	#undef DB4

#ifndef DX_NON_GRAPHICS
				// テクスチャキャッシュに転送
				if( TextureCacheUpdate )
				{
					RECT srect ;

//					SETRECT( srect, 0, 0, CharData->SizeX + AddSize, CharData->SizeY + AddSize ) ;
					SETRECT(
						srect,
						DestX - 1,
						DestY - 1,
						ManageData->BaseInfo.MaxWidth + DestX - 1,
						ManageData->BaseInfo.MaxWidth + DestY - 1
					) ;
//					Graphics_Image_BltBmpOrGraphImageToGraph2Base(
//						&im,
//						NULL,
//						&srect,
//						DestX - 1,
//						DestY - 1,
//						ManageData->TextureCache,
//						GSYS.CreateImage.NotUseTransColor ? FALSE : TRUE
//					) ;
					FontTextureCacheAddUpdateRect( ManageData, &srect ) ;
//					FontTextureCacheUpdateRectApply( ManageData ) ;

					srect.left  += adp ;
					srect.right += adp ;
//					Graphics_Image_BltBmpOrGraphImageToGraph2Base(
//						&im,
//						NULL,
//						&srect,
//						DestX - 1 + ManageData->CacheImageSize.cx,
//						DestY - 1,
//						ManageData->TextureCache,
//						GSYS.CreateImage.NotUseTransColor ? FALSE : TRUE
//					) ;
				}
#endif // DX_NON_GRAPHICS
			}
		}

		// サイズ補正
		CharData->AddX  += ( short )AddSize ;
		CharData->SizeX += ( WORD  )AddSize ;
		CharData->SizeY += ( WORD  )AddSize ;
	}

	// テーブルに収まる範囲の場合はコードのリスト型データに登録
	if( CharCode <= 0xffff )
	{
		CodeData = &ManageData->CodeData[ CharCode ] ; 
		CodeData->ExistFlag	= TRUE ;
		CodeData->CharData	= CharData ;
		CharData->CodeData	= CodeData ;
	}
	else
	// テーブルに収まらない範囲の場合は別のリストに登録
	{
		CharData->CodeData	= NULL ;

		// 数が最大数に達していたら一つ無効にする
		if( ManageData->CodeDataExNum == FONT_CACHE_EX_NUM )
		{
			int DelIndex ;
			for( DelIndex = 0 ; ManageData->CodeDataEx[ DelIndex ] == CharData ; DelIndex ++ ){}
			ManageData->CodeDataEx[ DelIndex ]->ValidFlag = FALSE ;

			ManageData->CodeDataExNum -- ;
			if( ManageData->CodeDataExNum - DelIndex > 0 )
			{
				_MEMMOVE( &ManageData->CodeDataEx[ DelIndex ], &ManageData->CodeDataEx[ DelIndex + 1 ], sizeof( FONTCHARDATA * ) * ( ManageData->CodeDataExNum - DelIndex ) ) ;
			}
		}
		ManageData->CodeDataEx[ ManageData->CodeDataExNum ] = CharData ;
		ManageData->CodeDataExNum ++ ;
	}

	// 情報を有効にする
	CharData->ValidFlag = TRUE ;

	// グラフィックハンドルかどうかのフラグを倒す
	CharData->GraphHandleFlag = FALSE ;

//	// リサイズ処理用のメモリを確保していた場合は解放
//	if( ResizeBuffer != NULL )
//	{
//		DXFREE( ResizeBuffer ) ;
//		ResizeBuffer = NULL ;
//	}

	// 終了
	return 0 ;
}

// フォントのテクスチャキャッシュの更新矩形を追加する
extern int FontTextureCacheAddUpdateRect( FONTMANAGE *ManageData, RECT *Rect )
{
	if( ManageData->TextureCacheUpdateRectValid == FALSE )
	{
		ManageData->TextureCacheUpdateRectValid = TRUE ;
		ManageData->TextureCacheUpdateRect = *Rect ;
	}
	else
	{
		if( ManageData->TextureCacheUpdateRect.left > Rect->left )
		{
			ManageData->TextureCacheUpdateRect.left = Rect->left ;
		}

		if( ManageData->TextureCacheUpdateRect.top > Rect->top )
		{
			ManageData->TextureCacheUpdateRect.top = Rect->top ;
		}

		if( ManageData->TextureCacheUpdateRect.right < Rect->right )
		{
			ManageData->TextureCacheUpdateRect.right = Rect->right ;
		}

		if( ManageData->TextureCacheUpdateRect.bottom < Rect->bottom )
		{
			ManageData->TextureCacheUpdateRect.bottom = Rect->bottom ;
		}
	}

	return 0 ;
}

// フォントのテクスチャキャッシュの更新矩形を実際のテクスチャに適用する
extern int FontTextureCacheUpdateRectApply( FONTMANAGE *ManageData )
{
	if( ManageData->TextureCacheUpdateRectValid == FALSE )
	{
		return 0 ;
	}

	ManageData->TextureCacheUpdateRectValid = FALSE ;

	Graphics_Image_BltBmpOrGraphImageToGraph2Base(
		&ManageData->TextureCacheBaseImage, NULL,
		&ManageData->TextureCacheUpdateRect,
		ManageData->TextureCacheUpdateRect.left,
		ManageData->TextureCacheUpdateRect.top,
		ManageData->TextureCache,
		GSYS.CreateImage.NotUseTransColor ? FALSE : TRUE
	) ;

	// 縁ありの場合は縁用の部分も更新する
	if( ( ManageData->FontType & DX_FONTTYPE_EDGE ) != 0 )
	{
		RECT SrcRect ;

		SrcRect.left   = ManageData->TextureCacheUpdateRect.left  + ManageData->TextureCacheBaseImage.Width / 2 ;
		SrcRect.top    = ManageData->TextureCacheUpdateRect.top ;
		SrcRect.right  = ManageData->TextureCacheUpdateRect.right + ManageData->TextureCacheBaseImage.Width / 2 ;
		SrcRect.bottom = ManageData->TextureCacheUpdateRect.bottom ;
		Graphics_Image_BltBmpOrGraphImageToGraph2Base(
			&ManageData->TextureCacheBaseImage, NULL,
			&SrcRect,
			SrcRect.left,
			SrcRect.top,
			ManageData->TextureCache,
			GSYS.CreateImage.NotUseTransColor ? FALSE : TRUE
		) ;
	}

	return 0 ;
}

// フォントハンドルに設定されている文字コード形式を取得する( 戻り値  -1:エラー  -1以外:文字コード形式 )
extern int GetFontHandleCharCodeFormat( int FontHandle )
{
	FONTMANAGE * ManageData ;

	if( FONTHCHK( FontHandle, ManageData ) )
	{
		return -1 ;
	}

	if( ManageData->UseCharCodeFormat >= 0 )
	{
		return ManageData->UseCharCodeFormat ;
	}

	return ( int )( ManageData->BaseInfo.CharCodeFormat == 0xffff ? _TCHARCODEFORMAT : ManageData->BaseInfo.CharCodeFormat ) ;
}


// 文字キャッシュのセットアップを行う
extern int SetupFontCache( CREATEFONTTOHANDLE_GPARAM *GParam, FONTMANAGE *ManageData, int ASyncThread )
{
	// フォントにエッジをつける場合は最大サイズ＋(エッジのサイズ×２)にする
	if( ManageData->FontType & DX_FONTTYPE_EDGE )
	{
		ManageData->BaseInfo.MaxWidth += ( WORD )( ManageData->EdgeSize * 2 ) ;
	}

	// フォントの最大幅を 8 の倍数にする
	ManageData->BaseInfo.MaxWidth = ( WORD )( ( ManageData->BaseInfo.MaxWidth + 7 ) / 8 * 8 ) ;

	// フォントにエッジをつける場合は最大サイズ＋(エッジのサイズ×２)にする
	if( ManageData->FontType & DX_FONTTYPE_EDGE )
	{
		ManageData->BaseInfo.FontHeight += ( WORD )( ManageData->EdgeSize * 2 ) ;
	}

	// フォントの高さを２の倍数にする
	ManageData->BaseInfo.FontHeight = ( ManageData->BaseInfo.FontHeight + 1 ) / 2 * 2 ;

	// フォントの高さの方が最大幅より大きかったら最大幅をフォントの高さにする
	if( ManageData->BaseInfo.MaxWidth < ManageData->BaseInfo.FontHeight )
	{
		ManageData->BaseInfo.MaxWidth = ManageData->BaseInfo.FontHeight;
	}

	// テクスチャキャッシュを使用するかどうかを決定
#ifndef DX_NON_GRAPHICS
	ManageData->TextureCacheFlag           = GSYS.Setting.ValidHardware && GParam->NotTextureFlag == FALSE ? TRUE : FALSE ;
#else // DX_NON_GRAPHICS
	ManageData->TextureCacheFlag           = FALSE ;
#endif // DX_NON_GRAPHICS
	ManageData->TextureCacheColorBitDepth  = ManageData->TextureCacheFlag == FALSE || ( ManageData->FontType & DX_FONTTYPE_ANTIALIASING ) == 0 || GParam->TextureCacheColorBitDepth16Flag ? 16 : 32 ;
	ManageData->TextureCacheUsePremulAlpha = GParam->UsePremulAlphaFlag ;

	ManageData->TextureCacheUpdateRectValid		= FALSE ;
	ManageData->TextureCacheUpdateRect.left		= 0 ;
	ManageData->TextureCacheUpdateRect.top		= 0 ;
	ManageData->TextureCacheUpdateRect.right	= 0 ;
	ManageData->TextureCacheUpdateRect.bottom	= 0 ;

#ifndef DX_NON_GRAPHICS
	// キャッシュできる最大文字数を決定
R1 :
	if( ManageData->TextureCacheFlag == TRUE )
	{
		int ScreenArea85per ;
		int CharNum ;

		// テクスチャキャッシュを使用する場合

		// 画面面積の８５％を求めておく
		ScreenArea85per = GSYS.Screen.MainScreenSizeX * GSYS.Screen.MainScreenSizeY * 85 / 100 ;

		// テクスチャサイズを割り出す
		{
			int MaxSizeW = ( ManageData->FontType & DX_FONTTYPE_EDGE ) != 0 ? GSYS.HardInfo.MaxTextureSize / 2 : GSYS.HardInfo.MaxTextureSize ;
			int MaxSizeH = GSYS.HardInfo.MaxTextureSize ;

			// 現時点ではテクスチャキャッシュの最大サイズは 8192x8192 にする
			if( MaxSizeW > 8192 ) MaxSizeW = 8192 ;
			if( MaxSizeH > 8192 ) MaxSizeH = 8192 ;

			ManageData->CacheImageSize.cx = 8 ;
			ManageData->CacheImageSize.cy = 8 ;
			for(;;)
			{
				// キャッシュできる文字数がキャッシュできる最大数を超えていたらループ終了
				CharNum = ( ManageData->CacheImageSize.cx / ManageData->BaseInfo.MaxWidth ) *
						  ( ManageData->CacheImageSize.cy / ManageData->BaseInfo.MaxWidth ) ;
				if( CharNum >= FONT_CACHE_MAXNUM )
				{
					// キャッシュできる文字数をキャッシュできる最大数に制限
					CharNum = FONT_CACHE_MAXNUM ;
					break ;
				}

				// キャッシュできる文字数が明示されているかどうかで処理を分岐
				if( GParam->CacheCharNum > 0 )
				{
					// 明示されている場合はキャッシュできる文字数が
					// 指定のキャッシュ文字数以上になっていたらループ終了
					if( CharNum >= GParam->CacheCharNum )
					{
						break ;
					}
				}
				else
				{
					// 明示されていない場合はテクスチャ面積が画面サイズの８５％を超えていて
					// 且つキャッシュできる文字数が１０文字を超えていたらたらループ終了
					if( ManageData->CacheImageSize.cx * ManageData->CacheImageSize.cy >= ScreenArea85per &&
						CharNum >= 10 )
					{
						break ;
					}
				}

				// テクスチャサイズが最大になっていたら終了
				if( ManageData->CacheImageSize.cx >= MaxSizeW &&
					ManageData->CacheImageSize.cy >= MaxSizeH )
				{
					break ;
				}

				// テクスチャサイズを２倍にする
				if( ManageData->CacheImageSize.cx >= MaxSizeW )
				{
					ManageData->CacheImageSize.cy <<= 1 ;
				}
				else
				if( ManageData->CacheImageSize.cy >= MaxSizeH )
				{
					ManageData->CacheImageSize.cx <<= 1 ;
				}
				else
				if( ManageData->CacheImageSize.cx > ManageData->CacheImageSize.cy )
				{
					ManageData->CacheImageSize.cy <<= 1 ;
				}
				else
				{
					ManageData->CacheImageSize.cx <<= 1 ;
				}
			}
		}

		// キャッシュできる文字数を保存
		ManageData->MaxCacheCharNum = CharNum ;

		// 縦一列に格納できる文字数を算出
		ManageData->LengthCharNum = ManageData->CacheImageSize.cy / ManageData->BaseInfo.MaxWidth ;

		// 基本イメージ関係の初期化
		{
			int alpha ;
			int test ;
			int edge ;
			BASEIMAGE *BaseImage ;

			if( ManageData->FontType & DX_FONTTYPE_ANTIALIASING )
			{
				alpha = TRUE ;
				test  = FALSE ;
			}
			else
			{
				alpha = FALSE ;
				test  = TRUE ;
			}
			edge = ( ManageData->FontType & DX_FONTTYPE_EDGE ) != 0 ? TRUE : FALSE ;

			// キャッシュに転送するイメージと同じものを格納しておく基本イメージの作成
			{
				BaseImage = &ManageData->TextureCacheBaseImage ;

				// デバイスがパレットテクスチャに対応している場合はパレット形式にする
				if( GSYS.HardInfo.Support4bitPaletteTexture )
				{
					int i ;

					NS_CreatePaletteColorData( &BaseImage->ColorData ) ;

					// さらにビット数を4にするか8にするか分岐
					if( ( ManageData->FontType & DX_FONTTYPE_ANTIALIASING ) != 0 &&
						( ManageData->UseFontDataFile == FALSE ||
						  ManageData->FontDataFile.ImageType == DX_FONT_SRCIMAGETYPE_8BIT_MAX64 ||
						  ManageData->FontDataFile.ImageType == DX_FONT_SRCIMAGETYPE_8BIT_MAX255 ) )
					{
						BaseImage->ColorData.MaxPaletteNo = 255 ;
						if( ManageData->TextureCacheUsePremulAlpha )
						{
							for( i = 0 ; i < 256 ; i ++ )
							{
								BaseImage->ColorData.Palette[ i ].Red   = ( BYTE )i ;
								BaseImage->ColorData.Palette[ i ].Green = ( BYTE )i ;
								BaseImage->ColorData.Palette[ i ].Blue  = ( BYTE )i ;
								BaseImage->ColorData.Palette[ i ].Alpha = ( BYTE )i ;
							}
						}
						else
						{
							for( i = 0 ; i < 256 ; i ++ )
							{
								BaseImage->ColorData.Palette[ i ].Red   = 255 ;
								BaseImage->ColorData.Palette[ i ].Green = 255 ;
								BaseImage->ColorData.Palette[ i ].Blue  = 255 ;
								BaseImage->ColorData.Palette[ i ].Alpha = ( BYTE )i ;
							}
						}
					}
					else
					{
						BaseImage->ColorData.MaxPaletteNo = 15 ;
						if( ManageData->TextureCacheUsePremulAlpha )
						{
							for( i = 0 ; i < 16 ; i ++ )
							{
								BaseImage->ColorData.Palette[ i ].Red   = ( BYTE )( i * 255 / 15 ) ;
								BaseImage->ColorData.Palette[ i ].Green = ( BYTE )( i * 255 / 15 ) ;
								BaseImage->ColorData.Palette[ i ].Blue  = ( BYTE )( i * 255 / 15 ) ;
								BaseImage->ColorData.Palette[ i ].Alpha = ( BYTE )( i * 255 / 15 ) ;
							}
						}
						else
						{
							for( i = 0 ; i < 16 ; i ++ )
							{
								BaseImage->ColorData.Palette[ i ].Red   = 255 ;
								BaseImage->ColorData.Palette[ i ].Green = 255 ;
								BaseImage->ColorData.Palette[ i ].Blue  = 255 ;
								BaseImage->ColorData.Palette[ i ].Alpha = ( BYTE )( i * 255 / 15 ) ;
							}
						}
					}
				}
				else
				{
					BaseImage->ColorData	= *( NS_GetTexColorData( alpha, test, ManageData->TextureCacheColorBitDepth == 16 ? 0 : 1 ) ) ;
				}

				BaseImage->MipMapCount		= 0 ;
				BaseImage->GraphDataCount	= 0 ;
				BaseImage->Width			= ManageData->CacheImageSize.cx * ( edge ? 2 : 1 ) ;
				BaseImage->Height			= ManageData->CacheImageSize.cy ;
				BaseImage->Pitch			= BaseImage->Width * BaseImage->ColorData.PixelByte ;
				BaseImage->GraphData		= DXALLOC( ( size_t )( ( size_t )BaseImage->Pitch * ( size_t )BaseImage->Height ) ) ;
				if( BaseImage->GraphData == NULL )
				{
					DXST_LOGFILE_ADDUTF16LE( "\x87\x65\x57\x5b\xad\x30\xe3\x30\xc3\x30\xb7\x30\xe5\x30\xfa\x57\x2c\x67\xa4\x30\xe1\x30\xfc\x30\xb8\x30\x28\x75\x6e\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"文字キャッシュ基本イメージ用のメモリの確保に失敗しました\n" @*/ ) ;
					return -1 ;
				}

				if( GSYS.HardInfo.Support4bitPaletteTexture )
				{
					NS_FillBaseImage( BaseImage,   0,  0,  0,  0 ) ;
				}
				else
				{
					// 乗算済みアルファを使用する場合はRGBA=0に、使用しない場合はAだけを0にして初期化

					if( ManageData->TextureCacheUsePremulAlpha )
					{
						NS_FillBaseImage( BaseImage,   0,  0,  0,  0 ) ;
					}
					else
					{
						NS_FillBaseImage( BaseImage, 255,255,255,  0 ) ;
					}
				}
			}
		}

		// キャッシュに使用するテクスチャグラフィックを作成する
		ManageData->TextureCacheLostFlag = TRUE ;
		if( RefreshFontDrawResourceToHandle( ManageData, ASyncThread ) < 0 )
		{
			DXST_LOGFILE_ADDUTF16LE( "\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x87\x65\x57\x5b\xad\x30\xe3\x30\xc3\x30\xb7\x30\xe5\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"テクスチャ文字キャッシュの作成に失敗しました\n" @*/ ) ;
			ManageData->TextureCacheFlag = FALSE ;
			goto R1 ;
		}
	}
	else
#endif // DX_NON_GRAPHICS
	{
		// メモリ上のキャッシュを使用する場合
		int BitNum = 0 ;
			
		// １ピクセル分のデータを保存するに当たり必要なビット数をセット
		switch( ManageData->FontType )
		{
		case DX_FONTTYPE_NORMAL :                 BitNum = 1 ; break ;
		case DX_FONTTYPE_EDGE :                   BitNum = 8 ; break ;
		case DX_FONTTYPE_ANTIALIASING :           BitNum = 8 ; break ;
		case DX_FONTTYPE_ANTIALIASING_4X4 :       BitNum = 8 ; break ;
		case DX_FONTTYPE_ANTIALIASING_8X8 :       BitNum = 8 ; break ;
		case DX_FONTTYPE_ANTIALIASING_EDGE :      BitNum = 8 ; break ;
		case DX_FONTTYPE_ANTIALIASING_EDGE_4X4 :  BitNum = 8 ; break ;
		case DX_FONTTYPE_ANTIALIASING_EDGE_8X8 :  BitNum = 8 ; break ;
		}

		// キャッシュ文字の数を調整
		ManageData->MaxCacheCharNum = GParam->CacheCharNum ;
		if( ManageData->MaxCacheCharNum == 0 )
		{
			ManageData->MaxCacheCharNum = FONT_CACHE_MEMORYSIZE / ( ManageData->BaseInfo.MaxWidth * ManageData->BaseInfo.MaxWidth ) ;
			if( ManageData->MaxCacheCharNum < 20 )
			{
				ManageData->MaxCacheCharNum = 20 ;
			}
		}

		if( ManageData->MaxCacheCharNum > FONT_CACHE_MAXNUM )
		{
			ManageData->MaxCacheCharNum = FONT_CACHE_MAXNUM ;
		}

		// キャッシュイメージのサイズをセット
		ManageData->CacheImageSize.cx	= ManageData->BaseInfo.MaxWidth ;
		ManageData->CacheImageSize.cy	= ManageData->MaxCacheCharNum * ManageData->BaseInfo.MaxWidth ;

		// キャッシュ文字数のセット
		ManageData->MaxCacheCharNum		= ManageData->CacheImageSize.cy / ManageData->BaseInfo.MaxWidth ;
		ManageData->LengthCharNum		= ManageData->MaxCacheCharNum ;
		ManageData->CacheDataBitNum		= BitNum ;

		// キャッシュ用メモリの確保
		ManageData->CachePitch = ( BitNum * ManageData->CacheImageSize.cx + 7 ) / 8 ;
		ManageData->CacheMem = (unsigned char *)DXALLOC( ( size_t )( ManageData->CachePitch * ManageData->CacheImageSize.cy ) ) ;
		if( ManageData->CacheMem == NULL )
		{
			DXST_LOGFILE_ADDUTF16LE( "\xd5\x30\xa9\x30\xf3\x30\xc8\x30\x6e\x30\xad\x30\xe3\x30\xc3\x30\xb7\x30\xe5\x30\x28\x75\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x69\x00\x6e\x00\x20\x00\x43\x00\x72\x00\x65\x00\x61\x00\x74\x00\x65\x00\x46\x00\x6f\x00\x6e\x00\x74\x00\x54\x00\x6f\x00\x48\x00\x61\x00\x6e\x00\x64\x00\x6c\x00\x65\x00\x00"/*@ L"フォントのキャッシュ用メモリの確保に失敗しました in CreateFontToHandle" @*/ ) ;
			return -1 ;
		}

		// ブレンド描画時用フォントグラフィックの作成
		if( RefreshFontDrawResourceToHandle( ManageData, ASyncThread ) < 0 )
		{
			DXST_LOGFILE_ADDUTF16LE( "\xd5\x30\xa9\x30\xf3\x30\xc8\x30\x28\x75\xcf\x63\x3b\x75\xb9\x30\xaf\x30\xea\x30\xfc\x30\xf3\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"フォント用描画スクリーンの作成に失敗しました\n" @*/ ) ;
			return -1 ;
		}
	}

	// １文字分の作業用バッファを確保
	ManageData->TempBufferSize = ( ManageData->BaseInfo.MaxWidth + 4 ) * ( ManageData->BaseInfo.MaxWidth + 4 ) ;
	ManageData->TempBuffer = ( BYTE * )DXALLOC( ManageData->TempBufferSize ) ;
	if( ManageData->TempBuffer == NULL )
	{
		DXST_LOGFILE_ADDUTF16LE( "\xd5\x30\xa9\x30\xf3\x30\xc8\x30\x6e\x30\x5c\x4f\x6d\x69\x28\x75\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"フォントの作業用バッファの作成に失敗しました\n" @*/ ) ;
		return -1 ;
	}

	// テキストキャッシュデータの初期化
	InitFontCacheToHandle( ManageData, ASyncThread ) ; 

	// 正常終了
	return 0 ;
}

// 文字キャッシュに新しい文字を加える
extern FONTCHARDATA * FontCacheCharAddToHandle( int AddNum, const DWORD *CharCodeList, FONTMANAGE *ManageData, int TextureCacheUpdate )
{
//	FONTMANAGE * 	ManageData ;
	FONTCHARDATA * 	CharData = NULL ;
	int 			i ;
	int             j ;
	void *			Image ;

//	DEFAULT_FONT_HANDLE_SETUP

#ifndef DX_NON_GRAPHICS
	// エラー判定
	if( ManageData->TextureCacheFlag && Graphics_Hardware_CheckValid_PF() == 0 )
	{
		return NULL ;
	}
#endif // DX_NON_GRAPHICS

	// フォントデータファイルを使用しない場合は環境依存の準備を行う
	if( ManageData->UseFontDataFile == FALSE )
	{
		// 環境依存処理０
		if( FontCacheCharAddToHandle_Timing0_PF( ManageData ) < 0 )
		{
			return NULL ;
		}
	}

	// 追加する文字の数だけループ
	for( i = 0 ; i < AddNum ; i ++, CharCodeList ++ )
	{
		// 次の文字を追加するインデックスを取得
		CharData = &ManageData->CharData[ ManageData->Index ] ;

		// フォントデータファイルを使用しない場合は環境依存の追加処理を行う
		if( ManageData->UseFontDataFile == FALSE )
		{
			// 環境依存処理
			if( FontCacheCharAddToHandle_Timing1_PF( ManageData, CharData, *CharCodeList, TextureCacheUpdate ) < 0 )
			{
				CharData = NULL ;
				continue ;
			}
		}
		else
		{
			FONTDATAFILECHARADATA *Chara ;

			// 文字の情報を取得
			if( *CharCodeList >= 0x10000 )
			{
				for( j = 0 ; ( DWORD )j < ManageData->FontDataFile.Header->CharaExNum ; j ++ )
				{
					if( ManageData->FontDataFile.CharaExArray[ j ]->CodeUnicode == *CharCodeList )
					{
						break ;
					}
				}
				if( j == ( int )ManageData->FontDataFile.Header->CharaExNum )
				{
					CharData = NULL ;
					continue ;
				}

				Chara = ManageData->FontDataFile.CharaExArray[ j ] ;
			}
			else
			{
				Chara = ManageData->FontDataFile.CharaTable[ *CharCodeList ] ;
				if( Chara == NULL )
				{
					CharData = NULL ;
					continue ;
				}
			}

			// イメージが圧縮されていたら解凍する
			if( Chara->Press )
			{
				DXA_Decode( ManageData->FontDataFile.Image + Chara->ImageAddress, ManageData->FontDataFile.PressImageDecodeBuffer ) ;
				Image = ManageData->FontDataFile.PressImageDecodeBuffer ;
			}
			else
			{
				Image = ManageData->FontDataFile.Image + Chara->ImageAddress ;
			}

			// イメージを転送
			FontCacheCharImageBltToHandle(
				ManageData,
				CharData,
				*CharCodeList, 
				FALSE,
				ManageData->FontDataFile.ImageType,
				Image,
				Chara->SizeX,
				Chara->SizeY,
				Chara->ImagePitch,
				Chara->DrawX,
				Chara->DrawY,
				Chara->AddX,
				TextureCacheUpdate
			) ;
		}

		// 次の文字を登録する際のインデックスをセットする
		ManageData->Index ++ ;
		if( ManageData->Index ==  ( unsigned int )ManageData->MaxCacheCharNum )
		{
			ManageData->Index = 0 ;
		}
	}

	// テクスチャの一括更新
	FontTextureCacheUpdateRectApply( ManageData ) ;

	// フォントデータファイルを使用しない場合は環境依存の後始末処理を行う
	if( ManageData->UseFontDataFile == FALSE )
	{
		// 環境依存処理２
		if( FontCacheCharAddToHandle_Timing2_PF( ManageData ) < 0 )
		{
			return NULL ;
		}
	}

	// 終了
	return CharData ;
}

static int EnumFontNameBase( wchar_t *NameBuffer, int NameBufferNum, int JapanOnlyFlag, int IsEx, int CharSet, const wchar_t *EnumFontName, int IsReturnValid )
{
	ENUMFONTDATA	EnumFontData ;
	wchar_t			*DestBuffer;
	int				Result ;

	EnumFontData.FontNum		= 0 ;
	EnumFontData.JapanOnlyFlag	= IsEx ? FALSE : JapanOnlyFlag ;
	EnumFontData.Valid			= FALSE ;
	EnumFontData.EnumFontName	= EnumFontName ;

	// フォントがあるかを調べるだけの場合は処理を分岐
	if( IsReturnValid )
	{
		EnumFontData.FontBuffer		= NULL ;
		EnumFontData.BufferNum		= 0 ;

		// 環境依存処理
		Result = EnumFontName_PF( &EnumFontData, TRUE, CharSet ) ;
	}
	else
	{
		if( NameBuffer == NULL )
		{
			DestBuffer = ( wchar_t * )DXALLOC( 1024 * 1024 * 256 ) ;
		}
		else
		{
			DestBuffer = NameBuffer ;
		}

		EnumFontData.FontBuffer		= DestBuffer ;
		EnumFontData.BufferNum		= NameBufferNum ;

		// 環境依存処理
		Result = EnumFontName_PF( &EnumFontData, IsEx, CharSet ) ;

		// メモリの解放
		if( NameBuffer == NULL )
		{
			DXFREE( DestBuffer ) ;
			DestBuffer = NULL ;
		}
	}

	// エラーチェック
	if( Result < 0 )
	{
		return -1 ;
	}

	// フォントが存在したか、若しくはフォントデータ領域数を返す
	return IsReturnValid ? EnumFontData.Valid : EnumFontData.FontNum ;
}

static int EnumFontNameBaseT( TCHAR *NameBuffer, int NameBufferNum, int JapanOnlyFlag, int IsEx, int CharSet, const TCHAR *EnumFontName, int IsReturnValid )
{
#ifdef UNICODE
	return EnumFontNameBase( NameBuffer, NameBufferNum, JapanOnlyFlag, IsEx, CharSet, EnumFontName, IsReturnValid ) ;
#else
	wchar_t *TempNameBuffer = NULL ;
	int Result = -1 ;
	int i ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( EnumFontName, goto END )

	if( NameBuffer != NULL )
	{
		TempNameBuffer = ( wchar_t * )DXALLOC( 1024 * 1024 * 256 ) ;
		if( TempNameBuffer == NULL )
		{
			goto END ;
		}
		_MEMSET( TempNameBuffer, 0, 1024 * 1024 * 256 ) ;
	}
	else
	{
		TempNameBuffer = NULL ;
	}

	Result = EnumFontNameBase( TempNameBuffer, NameBufferNum, JapanOnlyFlag, IsEx, CharSet, UseEnumFontNameBuffer, IsReturnValid ) ;
	if( Result < 0 )
	{
		goto END ;
	}

	if( NameBuffer != NULL )
	{
		for( i = 0 ; i < Result ; i ++ )
		{
			ConvString( ( const char * )( &TempNameBuffer[ i * 64 ] ), -1, WCHAR_T_CHARCODEFORMAT, ( char * )&NameBuffer[ i * 64 ], sizeof( TCHAR ) * 64, _TCHARCODEFORMAT ) ;
		}
	}

END :

	TCHAR_TO_WCHAR_T_STRING_END( EnumFontName )

	if( TempNameBuffer != NULL )
	{
		DXFREE( TempNameBuffer ) ;
		TempNameBuffer = NULL ;
	}
	return Result ;
#endif
}


// 使用可能なフォントの名前をすべて列挙する
extern int NS_EnumFontName( TCHAR *NameBuffer, int NameBufferNum, int JapanOnlyFlag )
{
	return EnumFontNameBaseT( NameBuffer, NameBufferNum, JapanOnlyFlag ) ;
}

// 使用可能なフォントの名前を列挙する( NameBuffer に 64バイト区切りで名前が格納されます )
extern int EnumFontName_WCHAR_T( wchar_t *NameBuffer, int NameBufferNum, int JapanOnlyFlag )
{
	return EnumFontNameBase( NameBuffer, NameBufferNum, JapanOnlyFlag ) ;
}


// 使用可能なフォントの名前をすべて列挙する
extern int NS_EnumFontNameEx( TCHAR *NameBuffer, int NameBufferNum, int CharSet )
{
	return EnumFontNameBaseT( NameBuffer, NameBufferNum, FALSE, TRUE, CharSet ) ;
}

// 使用可能なフォントの名前を列挙する( NameBuffer に 64バイト区切りで名前が格納されます )( 文字セット指定版 )
extern int EnumFontNameEx_WCHAR_T( wchar_t *NameBuffer, int NameBufferNum, int CharSet )
{
	return EnumFontNameBase( NameBuffer, NameBufferNum, FALSE, TRUE, CharSet ) ;
}


// 指定のフォント名のフォントを列挙する
extern int NS_EnumFontNameEx2( TCHAR *NameBuffer, int NameBufferNum, const TCHAR *EnumFontName, int CharSet )
{
	return EnumFontNameBaseT( NameBuffer, NameBufferNum, FALSE, TRUE, CharSet, EnumFontName ) ;
}

// 指定のフォント名のフォントを列挙する
extern int NS_EnumFontNameEx2WithStrLen( TCHAR *NameBuffer, int NameBufferNum, const TCHAR *EnumFontName, size_t EnumFontNameLength, int CharSet )
{
	int Result ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_ONE_BEGIN( EnumFontName, EnumFontNameLength, return -1 )
	Result = EnumFontNameBaseT( NameBuffer, NameBufferNum, FALSE, TRUE, CharSet, UseEnumFontNameBuffer ) ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( EnumFontName )
	return Result ;
}

// 指定のフォント名のフォントを列挙する
extern int EnumFontNameEx2_WCHAR_T( wchar_t *NameBuffer, int NameBufferNum, const wchar_t *EnumFontName, int CharSet )
{
	return EnumFontNameBase( NameBuffer, NameBufferNum, FALSE, TRUE, CharSet, EnumFontName ) ;
}


// 指定のフォント名のフォントが存在するかどうかをチェックする( 戻り値  TRUE:存在する  FALSE:存在しない )
extern int NS_CheckFontName( const TCHAR *FontName, int CharSet /* DX_CHARSET_DEFAULT 等 */ )
{
	return EnumFontNameBaseT( NULL, 0, FALSE, TRUE, CharSet, FontName, TRUE ) ;
}

// 指定のフォント名のフォントが存在するかどうかをチェックする( 戻り値  TRUE:存在する  FALSE:存在しない )
extern int NS_CheckFontNameWithStrLen( const TCHAR *FontName, size_t FontNameLength, int CharSet )
{
	int Result ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_ONE_BEGIN( FontName, FontNameLength, return -1 )
	Result = EnumFontNameBaseT( NULL, 0, FALSE, TRUE, CharSet, FontName, TRUE ) ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( FontName )
	return Result ;
}

// 指定のフォント名のフォントが存在するかどうかをチェックする( 戻り値  TRUE:存在する  FALSE:存在しない )
extern int CheckFontName_WCHAR_T( const wchar_t *FontName, int CharSet )
{
	return EnumFontNameBase( NULL, 0, FALSE, TRUE, CharSet, FontName, TRUE ) ;
}


// InitFontToHandle の内部関数
extern int InitFontToHandleBase( int Terminate )
{
	FONTMANAGE * DefFont ;
	wchar_t DefFontName[256] ;
	int DefFontThickness = 0 ;
	int	DefFontSize = 0 ;
	int DefFontEdgeSize = 0 ;
	int	DefFontType = 0 ;
	int	DefFontCharSet = 0 ;
	int DefFontItalic = 0 ;
	int UseDefaultFont ;

	DXST_LOGFILE_ADDUTF16LE( "\xd5\x30\xa9\x30\xf3\x30\xc8\x30\x6e\x30\x1d\x52\x1f\x67\x16\x53\x92\x30\x4c\x88\x44\x30\x7e\x30\x59\x30\x0a\x00\x00"/*@ L"フォントの初期化を行います\n" @*/ ) ;
	DXST_LOGFILE_TABADD ;

	// デフォルトフォントのデータを保存
	UseDefaultFont = FSYS.DefaultFontHandle > 0 ? TRUE : FALSE ;
	if( UseDefaultFont )
	{
		DefFont = GetFontManageDataToHandle_Inline( FSYS.DefaultFontHandle ) ;
		_WCSCPY_S( DefFontName, sizeof( DefFontName ), DefFont->FontName ) ;
		DefFontSize      =	DefFont->BaseInfo.FontSize      ;
		DefFontThickness = 	DefFont->BaseInfo.FontThickness ;
		DefFontCharSet   = 	DefFont->BaseInfo.CharSet       ;
		DefFontItalic    =  DefFont->BaseInfo.Italic        ;
		DefFontEdgeSize  =  DefFont->EdgeSize      ;
		DefFontType      =	DefFont->FontType      ;
	}

	// すべてのフォントデータを削除
	AllHandleSub( DX_HANDLETYPE_FONT ) ;

	// 後始末中ではない場合はデフォルトフォントを作成
	if( Terminate == FALSE )
	{
		CREATEFONTTOHANDLE_GPARAM GParam ;
		InitCreateFontToHandleGParam( &GParam ) ;

		if( UseDefaultFont )
		{
			FSYS.DefaultFontHandle = CreateFontToHandle_UseGParam( &GParam, DefFontName,          DefFontSize,          DefFontThickness,      DefFontType,          DefFontCharSet,          DefFontEdgeSize,          DefFontItalic,          -1, FALSE ) ;
		}
		else
		{
			FSYS.DefaultFontHandle = CreateFontToHandle_UseGParam( &GParam, FSYS.DefaultFontName, FSYS.DefaultFontSize, FSYS.DefaultFontThick, FSYS.DefaultFontType, FSYS.DefaultFontCharSet, FSYS.DefaultFontEdgeSize, FSYS.DefaultFontItalic, -1, FALSE ) ;
		}
		NS_SetDeleteHandleFlag( FSYS.DefaultFontHandle, &FSYS.DefaultFontHandle ) ;
	}

	DXST_LOGFILE_TABSUB ;
	DXST_LOGFILE_ADDUTF16LE( "\xd5\x30\xa9\x30\xf3\x30\xc8\x30\x6e\x30\x1d\x52\x1f\x67\x16\x53\x6f\x30\x63\x6b\x38\x5e\x6b\x30\x42\x7d\x86\x4e\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"フォントの初期化は正常に終了しました\n" @*/ ) ;

	// 終了
	return 0 ;
}

// フォントのステータスをデフォルトに戻す
extern int NS_InitFontToHandle( void )
{
	return InitFontToHandleBase() ;
}

// フォントハンドルを初期化する
extern int InitializeFontHandle( HANDLEINFO *HandleInfo )
{
	FONTMANAGE * ManageData = ( FONTMANAGE * )HandleInfo ;

	ManageData->PF = ( FONTMANAGE_PF * )( ManageData + 1 ) ;

	// 作業用ハンドルを初期化
	ManageData->ModiDrawScreen[ 0 ] = -1 ;
	ManageData->ModiDrawScreen[ 1 ] = -1 ;
	ManageData->ModiDrawScreenV[ 0 ] = -1 ;
	ManageData->ModiDrawScreenV[ 1 ] = -1 ;

	// キャッシュハンドルを初期化
	ManageData->TextureCache	= -1 ;
	ManageData->TextureCacheSub	= -1 ;

	// 終了
	return 0 ;
}

// フォントハンドルの後始末をする
extern int TerminateFontHandle( HANDLEINFO *HandleInfo )
{
	FONTMANAGE * ManageData = ( FONTMANAGE * )HandleInfo ;

	// 環境依存処理
	TerminateFontHandle_PF( ManageData ) ;

	// テキストキャッシュ用メモリの解放
	if( ManageData->CacheMem != NULL )
	{
		DXFREE( ManageData->CacheMem ) ;
		ManageData->CacheMem = NULL ;
	}

	// 作業用バッファの解放
	if( ManageData->TempBuffer != NULL )
	{
		DXFREE( ManageData->TempBuffer ) ;
		ManageData->TempBuffer = NULL ;
	}

#ifndef DX_NON_GRAPHICS
	// 作業用グラフィック削除
	{
		int i ;

		for( i = 0 ; i < 2 ; i ++ )
		{
			if( ManageData->ModiDrawScreen[ i ] >= 0 )
			{
				NS_DeleteGraph( ManageData->ModiDrawScreen[ i ] ) ;
				ManageData->ModiDrawScreen[ i ] = -1 ;
			}

			if( ManageData->ModiDrawScreenV[ i ] >= 0 )
			{
				NS_DeleteGraph( ManageData->ModiDrawScreenV[ i ] ) ;
				ManageData->ModiDrawScreenV[ i ] = -1 ;
			}
		}
	}

	// テクスチャグラフィック削除
	if( ManageData->TextureCache >= 0 )
	{
		NS_DeleteGraph( ManageData->TextureCache ) ;
		NS_DeleteGraph( ManageData->TextureCacheSub ) ;
		ManageData->TextureCache = -1 ;
		ManageData->TextureCacheSub = -1 ;
	}
#endif // DX_NON_GRAPHICS

	// テクスチャキャッシュと同じイメージの基本イメージの解放
	if( ManageData->TextureCacheBaseImage.GraphData != NULL )
	{
		DXFREE( ManageData->TextureCacheBaseImage.GraphData ) ;
		ManageData->TextureCacheBaseImage.GraphData = NULL ;
	}

	// フォントデータファイルを使用していた場合はフォントデータファイルイメージ用の確保メモリを解放
	if( ManageData->FontDataFile.FileBuffer != NULL )
	{
		DXFREE( ManageData->FontDataFile.FileBuffer ) ;
		ManageData->FontDataFile.FileBuffer = NULL ;
	}


	// ロストフラグが設定されている場合は TRUE にする
	if( ManageData->LostFlag != NULL )
	{
		*ManageData->LostFlag = TRUE ;
	}

	// 終了
	return 0 ;
}

// CREATEFONTTOHANDLE_GPARAM のデータをセットする
extern void InitCreateFontToHandleGParam( CREATEFONTTOHANDLE_GPARAM *GParam )
{
	GParam->CacheCharNum					= FSYS.CacheCharNum ;
	GParam->TextureCacheColorBitDepth16Flag	= FSYS.TextureCacheColorBitDepth16Flag ;
	GParam->NotTextureFlag					= FSYS.NotTextureFlag ;
	GParam->UsePremulAlphaFlag				= FSYS.UsePremulAlphaFlag ;
	GParam->DisableAdjustFontSize			= FSYS.DisableAdjustFontSize ;
}

// CreateFontToHandle の実処理関数
static int CreateFontToHandle_Static(
	CREATEFONTTOHANDLE_GPARAM *GParam,
	int FontHandle,
	const wchar_t *FontName,
	int Size,
	int Thick,
	int FontType,
	int CharSet,
	int EdgeSize,
	int Italic,
	int ASyncThread
)
{
	FONTMANAGE *	ManageData		= NULL ;
	int				DefaultCharSet	= FALSE ;
	int				CharCodeFormat ;

	if( FontType 	< 0 ) FontType 	 = FSYS.AntialiasingFontOnlyFlag ? DX_FONTTYPE_ANTIALIASING : DEFAULT_FONT_TYPE ;
	if( Size 		< 0 ) Size 		= DEFAULT_FONT_SIZE ;
	if( Thick 		< 0 ) Thick 	= DEFAULT_FONT_THINCK ;
	if( EdgeSize	< 0 ) EdgeSize	= DEFAULT_FONT_EDGESIZE ;
	if( CharSet		< 0 || CharSet >= DX_CHARSET_NUM )
	{
		DefaultCharSet = TRUE ;
		CharSet  = _GET_CHARSET() ;
		CharCodeFormat = 0xffff ;
	}
	else
	{
		// CharSet が DX_CHARSET_DEFAULT の場合は文字コード形式はデフォルトのものを使用する
		if( CharSet == DX_CHARSET_DEFAULT )
		{
			CharCodeFormat = 0xffff ;
		}
		else
		{
			CharCodeFormat = CharCodeFormatTable[ CharSet ] ;
		}
	}

	// エッジ無しフォントの場合は EdgeSize を 0 固定にする
	if( ( FontType & DX_FONTTYPE_EDGE ) == 0 )
	{
		EdgeSize = 0 ;
	}

	// アンチエイリアスフォントだけ使用する場合は縁の有り無し以外はアンチエイリアスタイプに固定する
	if( FSYS.AntialiasingFontOnlyFlag )
	{
		if( ( FontType & DX_FONTTYPE_EDGE ) != 0 )
		{
			FontType = DX_FONTTYPE_ANTIALIASING_EDGE ;
		}
		else
		{
			FontType = DX_FONTTYPE_ANTIALIASING ;
		}
	}

	FONTHCHK( FontHandle, ManageData ) ;

#ifndef DX_NON_GRAPHICS
	if( NS_GetColorBitDepth() == 8 && ( FontType & DX_FONTTYPE_ANTIALIASING ) )
	{
		FontType &= ~DX_FONTTYPE_ANTIALIASING ;
	}
#endif // DX_NON_GRAPHICS

	// フォントのパラメータのセット
	ManageData->BaseInfo.FontSize 		= ( WORD )Size ;
	ManageData->BaseInfo.FontThickness 	= ( WORD )Thick ;
	ManageData->BaseInfo.Italic			= ( BYTE )Italic ;
	ManageData->BaseInfo.CharSet		= ( WORD )CharSet ;
	ManageData->BaseInfo.CharCodeFormat	= ( WORD )CharCodeFormat ;
	ManageData->FontType				= FontType ;
	ManageData->EdgeSize				= EdgeSize ;
	ManageData->Space					= 0 ;
	ManageData->LineSpaceValidFlag		= FALSE ;
	ManageData->LineSpace				= 0 ;
	ManageData->UseCharCodeFormat		= -1 ;

	// ロストフラグへのポインタを NULL にしておく
	ManageData->LostFlag = NULL ;

	// フォントデータファイルは使用しない
	ManageData->UseFontDataFile = 0 ;

	// フォント名の保存
	if( FontName == NULL )
	{
		ManageData->FontName[ 0 ] = L'\0' ;
	}
	else
	{
		_WCSCPY_S( ManageData->FontName, sizeof( ManageData->FontName ), FontName ) ;
	}

	// 環境依存処理
	if( CreateFontToHandle_PF( GParam, ManageData, DefaultCharSet ) != 0 )
	{
		return -1 ;
	}

	// フォント名の TCHAR 版を作成
	ConvString( ( char * )ManageData->FontName, -1, WCHAR_T_CHARCODEFORMAT, ( char * )ManageData->FontNameT, sizeof( ManageData->FontNameT ), _TCHARCODEFORMAT ) ;

	// テキストキャッシュサーフェスの作成
	if( SetupFontCache( GParam, ManageData, ASyncThread ) < 0 )
	{
		goto ERR ;
	}

	// 正常終了
	return 0 ;

	// エラー処理
ERR :
	CreateFontToHandle_Error_PF( ManageData ) ;

	return -1 ;
}

#ifndef DX_NON_ASYNCLOAD

// CreateFontToHandle の非同期読み込みスレッドから呼ばれる関数
static void CreateFontToHandle_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	CREATEFONTTOHANDLE_GPARAM *GParam ;
	int FontHandle ;
	const wchar_t *FontName ;
	int Size ;
	int Thick ;
	int FontType ;
	int CharSet ;
	int EdgeSize ;
	int Italic ;
	int Addr ;
	int Result ;

	Addr = 0 ;
	GParam = ( CREATEFONTTOHANDLE_GPARAM * )GetASyncLoadParamStruct( AParam->Data, &Addr ) ;
	FontHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	FontName = GetASyncLoadParamString( AParam->Data, &Addr ) ;
	Size = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	Thick = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	FontType = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	CharSet = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	EdgeSize = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	Italic = GetASyncLoadParamInt( AParam->Data, &Addr ) ;

	Result = CreateFontToHandle_Static( GParam, FontHandle, FontName, Size, Thick, FontType, CharSet, EdgeSize, Italic, TRUE ) ;

	DecASyncLoadCount( FontHandle ) ;
	if( Result < 0 )
	{
		SubHandle( FontHandle ) ;
	}
}

#endif // DX_NON_ASYNCLOAD

// CreateFontToHandle のグローバル変数にアクセスしないバージョン
extern int CreateFontToHandle_UseGParam(
	CREATEFONTTOHANDLE_GPARAM *GParam,
	const wchar_t *FontName,
	int Size,
	int Thick,
	int FontType,
	int CharSet,
	int EdgeSize,
	int Italic,
	int Handle,
	int ASyncLoadFlag
)
{
	int FontHandle ;

	CheckActiveState() ;

	// ハンドルの作成
	FontHandle = AddHandle( DX_HANDLETYPE_FONT, FALSE, Handle ) ;
	if( FontHandle == -1 )
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
		AddASyncLoadParamStruct( NULL, &Addr, GParam, sizeof( *GParam ) ) ;
		AddASyncLoadParamInt( NULL, &Addr, FontHandle ) ;
		AddASyncLoadParamString( NULL, &Addr, FontName ) ;
		AddASyncLoadParamInt( NULL, &Addr, Size ) ;
		AddASyncLoadParamInt( NULL, &Addr, Thick ) ;
		AddASyncLoadParamInt( NULL, &Addr, FontType ) ;
		AddASyncLoadParamInt( NULL, &Addr, CharSet ) ;
		AddASyncLoadParamInt( NULL, &Addr, EdgeSize ) ;
		AddASyncLoadParamInt( NULL, &Addr, Italic ) ;

		// メモリの確保
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// 処理に必要な情報をセット
		AParam->ProcessFunction = CreateFontToHandle_ASync ;
		Addr = 0 ;
		AddASyncLoadParamStruct( AParam->Data, &Addr, GParam, sizeof( *GParam ) ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, FontHandle ) ;
		AddASyncLoadParamString( AParam->Data, &Addr, FontName ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, Size ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, Thick ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, FontType ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, CharSet ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, EdgeSize ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, Italic ) ;

		// データを追加
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto ERR ;
		}

		// 非同期読み込みカウントをインクリメント
		IncASyncLoadCount( FontHandle, AParam->Index ) ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		if( CreateFontToHandle_Static( GParam, FontHandle, FontName, Size, Thick, FontType, CharSet, EdgeSize, Italic, FALSE ) < 0 )
			goto ERR ;
	}

	// 終了
	return FontHandle ;

ERR :
	SubHandle( FontHandle ) ;

	return -1 ;
}










// LoadFontDataFromMemToHandle_UseGParam の実処理関数
static int LoadFontDataFromMemToHandle_UseGParam_Static(
	CREATEFONTTOHANDLE_GPARAM *GParam,
	int FontHandle,
	const void *FontDataImage,
	int FontDataImageSize,
	int EdgeSize,
	int ASyncThread
)
{
	FONTMANAGE *ManageData = NULL ;
	const FONTDATAFILEHEADER *FileHeader ;
	int HeaderSize ;
	int NotPressHeaderSize ;
	int AllocSize ;

	FONTHCHK( FontHandle, ManageData ) ;

	FileHeader = ( const FONTDATAFILEHEADER * )FontDataImage ;

	if( EdgeSize < 0 )
	{
		EdgeSize = 0 ;
	}

	// IDチェック
	if( FileHeader->Magic[ 0 ] != 'F' ||
		FileHeader->Magic[ 1 ] != 'N' ||
		FileHeader->Magic[ 2 ] != 'T' ||
		FileHeader->Magic[ 3 ] != 'F' )
	{
		goto ERR ;
	}

	// フォントデータファイルを使用しているかのフラグを立てる
	ManageData->UseFontDataFile = TRUE ;

	// 確保するメモリサイズを算出する
	NotPressHeaderSize = sizeof( FONTDATAFILEHEADER ) - sizeof( FONTDATAFILEPRESSHEADER ) ;
	HeaderSize         = DXA_Decode( ( BYTE * )FontDataImage + NotPressHeaderSize, NULL ) + NotPressHeaderSize ;
	AllocSize          = FontDataImageSize +
						 HeaderSize +
						 sizeof( FONTDATAFILECHARADATA * ) * ( 0x10000 + FileHeader->CharaExNum ) +
						 FileHeader->MaxImageBytes ;

	// メモリの確保
	ManageData->FontDataFile.FileBuffer = ( BYTE * )DXALLOC( AllocSize ) ;
	if( ManageData->FontDataFile.FileBuffer == NULL )
	{
		goto ERR ;
	}

	// 解凍したヘッダ部を保存するメモリアドレスのセット
	ManageData->FontDataFile.Header = ( FONTDATAFILEHEADER * )( ManageData->FontDataFile.FileBuffer + FontDataImageSize ) ;

	// フォントデータファイル内の各文字の情報へのアドレステーブルのアドレスをセット
	ManageData->FontDataFile.CharaTable = ( FONTDATAFILECHARADATA ** )( ( BYTE * )ManageData->FontDataFile.Header + HeaderSize ) ;

	// フォントデータファイル内の 0xffff を超える文字コードの文字情報へのアドレスの配列を保存するメモリ領域へのアドレスをセット
	ManageData->FontDataFile.CharaExArray = ManageData->FontDataFile.CharaTable + 0x10000 ;

	// 解凍したフォント画像データを格納するメモリアドレスをセット
	ManageData->FontDataFile.PressImageDecodeBuffer = ManageData->FontDataFile.CharaExArray + FileHeader->CharaExNum ;

	// フォントデータファイルをコピーする
	_MEMCPY( ManageData->FontDataFile.FileBuffer, FontDataImage, FontDataImageSize ) ;
	ManageData->FontDataFile.FontFileSize = FontDataImageSize ;

	// ヘッダの圧縮された部分を解凍する
	DXA_Decode( ManageData->FontDataFile.FileBuffer + NotPressHeaderSize, ( BYTE * )ManageData->FontDataFile.Header + NotPressHeaderSize ) ;

	// ヘッダの圧縮されていない部分をコピー
	_MEMCPY( ManageData->FontDataFile.Header, ManageData->FontDataFile.FileBuffer, NotPressHeaderSize ) ;

	// 各文字の情報が格納されているアドレスをセット
	ManageData->FontDataFile.Chara = ( FONTDATAFILECHARADATA * )( ManageData->FontDataFile.Header + 1 ) ;

	// 文字の画像が格納されているアドレスをセット
	ManageData->FontDataFile.Image = ManageData->FontDataFile.FileBuffer + ManageData->FontDataFile.Header->ImageAddress ;

	// イメージデータタイプとフォントタイプをセット
	switch( ManageData->FontDataFile.Header->Press.ImageBitDepth )
	{
	case DX_FONTIMAGE_BIT_1 :
		ManageData->FontDataFile.ImageType = DX_FONT_SRCIMAGETYPE_1BIT ;
		ManageData->FontType = EdgeSize > 0 ? DX_FONTTYPE_EDGE : DX_FONTTYPE_NORMAL ;
		break ;

	case DX_FONTIMAGE_BIT_4 :
		ManageData->FontDataFile.ImageType = DX_FONT_SRCIMAGETYPE_4BIT_MAX15 ;
		ManageData->FontType = EdgeSize > 0 ? DX_FONTTYPE_ANTIALIASING_EDGE_4X4 : DX_FONTTYPE_ANTIALIASING_4X4 ;
		break ;

	case DX_FONTIMAGE_BIT_8 :
		ManageData->FontDataFile.ImageType = DX_FONT_SRCIMAGETYPE_8BIT_MAX255 ;
		ManageData->FontType = EdgeSize > 0 ? DX_FONTTYPE_ANTIALIASING_EDGE_8X8 : DX_FONTTYPE_ANTIALIASING_8X8 ;
		break ;
	}

	// 各文字へのアドレステーブルを作成する
	{
		FONTDATAFILECHARADATA *Chara ;
		DWORD i ;
		int CharaExNum ;

		_MEMSET( ManageData->FontDataFile.CharaTable, 0, sizeof( FONTDATAFILECHARADATA * ) * 0x10000 ) ;
		Chara      = ManageData->FontDataFile.Chara ;
		CharaExNum = 0 ;
		for( i = 0 ; i < ManageData->FontDataFile.Header->CharaNum ; i ++, Chara ++ )
		{
			if( Chara->CodeUnicode >= 0x10000 )
			{
				ManageData->FontDataFile.CharaExArray[ CharaExNum ] = Chara ;
				CharaExNum ++ ;
			}
			else
			{
				ManageData->FontDataFile.CharaTable[ Chara->CodeUnicode ] = Chara ;
			}
		}
	}

	// フォントのパラメータのセット
	_MEMCPY( &ManageData->BaseInfo, &ManageData->FontDataFile.Header->Press.BaseInfo, sizeof( ManageData->BaseInfo ) ) ;
	ManageData->EdgeSize			= EdgeSize ;
	ManageData->Space			    = 0 ;
	ManageData->LineSpaceValidFlag	= FALSE ;
	ManageData->LineSpace		    = 0 ;
	ManageData->UseCharCodeFormat	= -1 ;

	// フォント名の保存
	ConvString( ( const char * )ManageData->FontDataFile.Header->Press.FontName, -1, DX_CHARCODEFORMAT_UTF16LE, ( char * )ManageData->FontName,  sizeof( ManageData->FontName  ), WCHAR_T_CHARCODEFORMAT ) ;
	ConvString( ( char *       )ManageData->FontName,                            -1, WCHAR_T_CHARCODEFORMAT,    ( char * )ManageData->FontNameT, sizeof( ManageData->FontNameT ), _TCHARCODEFORMAT       ) ;

	// ロストフラグへのポインタを NULL にしておく
	ManageData->LostFlag = NULL ;

	// 環境依存処理
	if( CreateFontToHandle_PF( GParam, ManageData, FALSE ) != 0 )
	{
		goto ERR ;
	}

	// 文字画像キャッシュのセットアップ
	if( SetupFontCache( GParam, ManageData, ASyncThread ) < 0 )
	{
		goto ERR ;
	}

	// 正常終了
	return 0 ;

	// エラー処理
ERR :
	CreateFontToHandle_Error_PF( ManageData ) ;

	if( ManageData->FontDataFile.FileBuffer != NULL )
	{
		DXFREE( ManageData->FontDataFile.FileBuffer ) ;
		ManageData->FontDataFile.FileBuffer = NULL ;
	}

	return -1 ;
}

#ifndef DX_NON_ASYNCLOAD

// LoadFontDataFromMemToHandle_UseGParam の非同期読み込みスレッドから呼ばれる関数
static void LoadFontDataFromMemToHandle_UseGParam_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	CREATEFONTTOHANDLE_GPARAM *GParam ;
	int FontHandle ;
	const void *FontDataImage ;
	int FontDataImageSize ;
//	int FontType ;
	int EdgeSize ;
	int Addr ;
	int Result ;

	Addr = 0 ;
	GParam = ( CREATEFONTTOHANDLE_GPARAM * )GetASyncLoadParamStruct( AParam->Data, &Addr ) ;
	FontHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	FontDataImage = GetASyncLoadParamVoidP( AParam->Data, &Addr ) ;
	FontDataImageSize = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
//	FontType = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	EdgeSize = GetASyncLoadParamInt( AParam->Data, &Addr ) ;

	Result = LoadFontDataFromMemToHandle_UseGParam_Static( GParam, FontHandle, FontDataImage, FontDataImageSize, EdgeSize, TRUE ) ;

	DecASyncLoadCount( FontHandle ) ;
	if( Result < 0 )
	{
		SubHandle( FontHandle ) ;
	}
}

#endif // DX_NON_ASYNCLOAD

// LoadFontDataFromMemToHandle のグローバル変数にアクセスしないバージョン
extern int LoadFontDataFromMemToHandle_UseGParam(
	CREATEFONTTOHANDLE_GPARAM *GParam,
	const void *FontDataImage,
	int FontDataImageSize,
	int EdgeSize,
	int Handle,
	int ASyncLoadFlag
)
{
	int FontHandle ;

	CheckActiveState() ;

	// ハンドルの作成
	FontHandle = AddHandle( DX_HANDLETYPE_FONT, FALSE, Handle ) ;
	if( FontHandle == -1 )
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
		AddASyncLoadParamStruct( NULL, &Addr, GParam, sizeof( *GParam ) ) ;
		AddASyncLoadParamInt( NULL, &Addr, FontHandle ) ;
		AddASyncLoadParamConstVoidP( NULL, &Addr, FontDataImage ) ;
		AddASyncLoadParamInt( NULL, &Addr, FontDataImageSize ) ;
		AddASyncLoadParamInt( NULL, &Addr, EdgeSize ) ;

		// メモリの確保
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// 処理に必要な情報をセット
		AParam->ProcessFunction = LoadFontDataFromMemToHandle_UseGParam_ASync ;
		Addr = 0 ;
		AddASyncLoadParamStruct( AParam->Data, &Addr, GParam, sizeof( *GParam ) ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, FontHandle ) ;
		AddASyncLoadParamConstVoidP( AParam->Data, &Addr, FontDataImage ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, FontDataImageSize ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, EdgeSize ) ;

		// データを追加
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto ERR ;
		}

		// 非同期読み込みカウントをインクリメント
		IncASyncLoadCount( FontHandle, AParam->Index ) ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		if( LoadFontDataFromMemToHandle_UseGParam_Static( GParam, FontHandle, FontDataImage, FontDataImageSize, EdgeSize, FALSE ) < 0 )
			goto ERR ;
	}

	// 終了
	return FontHandle ;

ERR :
	SubHandle( FontHandle ) ;

	return -1 ;
}














// LoadFontDataToHandle_UseGParam の実処理関数
static int LoadFontDataToHandle_UseGParam_Static(
	CREATEFONTTOHANDLE_GPARAM *GParam,
	int FontHandle,
	const wchar_t *FileName,
	int EdgeSize,
	int ASyncThread
)
{
	FONTMANAGE *ManageData = NULL ;
	DWORD_PTR fp = 0 ;
	size_t FontDataImageSize ;
	void *FontDataImage = NULL ;
	int Result = -1 ;

	FONTHCHK( FontHandle, ManageData ) ;
	
	fp = DX_FOPEN( FileName ) ;
	if( fp == 0 )
	{
		DXST_LOGFILEFMT_ADDW(( L"Font Data File Open Error : %s", FileName )) ;
		goto END ;
	}

	DX_FSEEK( fp, 0, SEEK_END ) ;
	FontDataImageSize = ( size_t )DX_FTELL( fp ) ;
	DX_FSEEK( fp, 0, SEEK_SET ) ;
	
	FontDataImage = DXALLOC( FontDataImageSize ) ;
	if( FontDataImage == NULL )
	{
		DXST_LOGFILEFMT_ADDW(( L"Font Data File Load : Memory Alloc Error : %s", FileName )) ;
		goto END ;
	}
	
	DX_FREAD( FontDataImage, FontDataImageSize, 1, fp ) ;
	DX_FCLOSE( fp ) ;
	fp = 0 ;

	Result = LoadFontDataFromMemToHandle_UseGParam_Static( GParam, FontHandle, FontDataImage, ( int )FontDataImageSize, EdgeSize, ASyncThread ) ;

END :
	if( fp != 0 )
	{
		DX_FCLOSE( fp ) ;
	}

	if( FontDataImage != NULL )
	{
		DXFREE( FontDataImage ) ;
	}

	return Result ;
}

#ifndef DX_NON_ASYNCLOAD

// LoadFontDataToHandle_UseGParam の非同期読み込みスレッドから呼ばれる関数
static void LoadFontDataToHandle_UseGParam_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	CREATEFONTTOHANDLE_GPARAM *GParam ;
	int FontHandle ;
	const wchar_t *FileName ;
	int EdgeSize ;
	int Addr ;
	int Result ;

	Addr = 0 ;
	GParam = ( CREATEFONTTOHANDLE_GPARAM * )GetASyncLoadParamStruct( AParam->Data, &Addr ) ;
	FontHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	FileName = GetASyncLoadParamString( AParam->Data, &Addr ) ;
	EdgeSize = GetASyncLoadParamInt( AParam->Data, &Addr ) ;

	Result = LoadFontDataToHandle_UseGParam_Static( GParam, FontHandle, FileName, EdgeSize, TRUE ) ;

	DecASyncLoadCount( FontHandle ) ;
	if( Result < 0 )
	{
		SubHandle( FontHandle ) ;
	}
}

#endif // DX_NON_ASYNCLOAD

// LoadFontDataToHandle_UseGParam のグローバル変数にアクセスしないバージョン
extern int LoadFontDataToHandle_UseGParam(
	CREATEFONTTOHANDLE_GPARAM *GParam,
	const wchar_t *FileName,
	int EdgeSize,
	int ASyncLoadFlag
)
{
	int FontHandle ;

	CheckActiveState() ;

	// ハンドルの作成
	FontHandle = AddHandle( DX_HANDLETYPE_FONT, FALSE, -1 ) ;
	if( FontHandle == -1 )
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
		AddASyncLoadParamStruct( NULL, &Addr, GParam, sizeof( *GParam ) ) ;
		AddASyncLoadParamInt( NULL, &Addr, FontHandle ) ;
		AddASyncLoadParamString( NULL, &Addr, FileName ) ;
		AddASyncLoadParamInt( NULL, &Addr, EdgeSize ) ;

		// メモリの確保
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// 処理に必要な情報をセット
		AParam->ProcessFunction = LoadFontDataToHandle_UseGParam_ASync ;
		Addr = 0 ;
		AddASyncLoadParamStruct( AParam->Data, &Addr, GParam, sizeof( *GParam ) ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, FontHandle ) ;
		AddASyncLoadParamString( AParam->Data, &Addr, FileName ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, EdgeSize ) ;

		// データを追加
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto ERR ;
		}

		// 非同期読み込みカウントをインクリメント
		IncASyncLoadCount( FontHandle, AParam->Index ) ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		if( LoadFontDataToHandle_UseGParam_Static( GParam, FontHandle, FileName, EdgeSize, FALSE ) < 0 )
			goto ERR ;
	}

	// 終了
	return FontHandle ;

ERR :
	SubHandle( FontHandle ) ;

	return -1 ;
}







// 新しいフォントデータを作成
extern int CreateFontToHandle_WCHAR_T( const wchar_t *FontName,
									int Size, int Thick,
									int FontType, int CharSet,
									int EdgeSize, int Italic,
									int Handle )
{
	CREATEFONTTOHANDLE_GPARAM GParam ;

	InitCreateFontToHandleGParam( &GParam ) ;

	return CreateFontToHandle_UseGParam( &GParam, FontName, Size, Thick, FontType, CharSet, EdgeSize, Italic, Handle, GetASyncLoadFlag() ) ;
}

// 新しいフォントデータを作成
extern int NS_CreateFontToHandle( const TCHAR *FontName,
									int Size, int Thick,
									int FontType, int CharSet,
									int EdgeSize, int Italic,
									int Handle )
{
#ifdef UNICODE
	return CreateFontToHandle_WCHAR_T( FontName, Size, Thick, FontType, CharSet, EdgeSize, Italic, Handle ) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( FontName, return -1 )

	Result = CreateFontToHandle_WCHAR_T( UseFontNameBuffer, Size, Thick, FontType, CharSet, EdgeSize, Italic, Handle ) ;

	TCHAR_TO_WCHAR_T_STRING_END( FontName )

	return Result ;
#endif
}

// フォントハンドルを作成する
extern int NS_CreateFontToHandleWithStrLen( const TCHAR *FontName, size_t FontNameLength, int Size, int Thick, int FontType, int CharSet, int EdgeSize, int Italic, int Handle)
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FontName, FontNameLength, return -1 )
	Result = CreateFontToHandle_WCHAR_T( UseFontNameBuffer, Size, Thick, FontType, CharSet, EdgeSize, Italic, Handle ) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FontName )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FontName, FontNameLength, return -1 )
	Result = CreateFontToHandle_WCHAR_T( UseFontNameBuffer, Size, Thick, FontType, CharSet, EdgeSize, Italic, Handle ) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FontName )
#endif
	return Result ;
}

// フォントデータファイルからフォントハンドルを作成する
extern int LoadFontDataToHandle_WCHAR_T( const wchar_t *FileName, int EdgeSize )
{
	CREATEFONTTOHANDLE_GPARAM GParam ;

	InitCreateFontToHandleGParam( &GParam ) ;

	return LoadFontDataToHandle_UseGParam( &GParam, FileName, EdgeSize, GetASyncLoadFlag() ) ;
}

// フォントデータファイルからフォントハンドルを作成する
extern int NS_LoadFontDataToHandle( const TCHAR *FileName, int EdgeSize )
{
#ifdef UNICODE
	return LoadFontDataToHandle_WCHAR_T( FileName, EdgeSize ) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, return -1 )

	Result = LoadFontDataToHandle_WCHAR_T( UseFileNameBuffer, EdgeSize ) ;

	TCHAR_TO_WCHAR_T_STRING_END( FileName )

	return Result ;
#endif
}

// フォントデータファイルからフォントハンドルを作成する
extern int NS_LoadFontDataToHandleWithStrLen( const TCHAR *FileName, size_t FileNameLength, int EdgeSize )
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = LoadFontDataToHandle_WCHAR_T( UseFileNameBuffer, EdgeSize ) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = LoadFontDataToHandle_WCHAR_T( UseFileNameBuffer, EdgeSize ) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#endif
	return Result ;
}

// メモリ上のフォントデータファイルイメージからフォントハンドルを作成する
extern int NS_LoadFontDataFromMemToHandle( const void *FontDataImage, int FontDataImageSize, int EdgeSize )
{
	CREATEFONTTOHANDLE_GPARAM GParam ;

	InitCreateFontToHandleGParam( &GParam ) ;

	return LoadFontDataFromMemToHandle_UseGParam( &GParam, FontDataImage, FontDataImageSize, EdgeSize, -1, GetASyncLoadFlag() ) ;
}

// 字間を変更する
extern int NS_SetFontSpaceToHandle( int Pixel, int FontHandle )
{
	FONTMANAGE * ManageData ;

	DEFAULT_FONT_HANDLE_SETUP

	// エラー判定
	if( FONTHCHK( FontHandle, ManageData ) )
		return -1 ;
	
	ManageData->Space = Pixel ;
	
	// 終了
	return 0 ;
}

// フォントハンドルの行間を変更する
extern int NS_SetFontLineSpaceToHandle( int Pixel, int FontHandle )
{
	FONTMANAGE * ManageData ;

	DEFAULT_FONT_HANDLE_SETUP

	// エラー判定
	if( FONTHCHK( FontHandle, ManageData ) )
		return -1 ;
	
	ManageData->LineSpaceValidFlag = TRUE ;
	ManageData->LineSpace = Pixel ;
	
	// 終了
	return 0 ;
}

// 指定のフォントハンドルを使用する関数の引数に渡す文字列の文字コード形式を設定する( UNICODE版では無効 )
extern int NS_SetFontCharCodeFormatToHandle( int CharCodeFormat /* DX_CHARCODEFORMAT_SHIFTJIS 等 */ , int FontHandle )
{
	FONTMANAGE * ManageData ;

	DEFAULT_FONT_HANDLE_SETUP

	// エラー判定
	if( FONTHCHK( FontHandle, ManageData ) )
		return -1 ;
	
	ManageData->UseCharCodeFormat = CharCodeFormat ;
	
	// 終了
	return 0 ;
}

// デフォルトフォントのステータスを一括設定する
extern int SetDefaultFontState_WCHAR_T( const wchar_t *FontName, int Size, int Thick, int FontType, int CharSet, int EdgeSize, int Italic )
{
	if( FontType 	< 0 ) FontType 	= FSYS.AntialiasingFontOnlyFlag ? DX_FONTTYPE_ANTIALIASING : DEFAULT_FONT_TYPE ;
	if( EdgeSize	> 1 ) FontType	|= DX_FONTTYPE_EDGE ;

	if( Size 		< 0 ) Size 		= DEFAULT_FONT_SIZE ;
	if( Thick 		< 0 ) Thick 	= DEFAULT_FONT_THINCK ;
	if( EdgeSize	< 0 ) EdgeSize	= DEFAULT_FONT_EDGESIZE ;
//	if( CharSet		< 0 ) CharSet   = _GET_CHARSET() ;

	_WCSCPY_S( FSYS.DefaultFontName, sizeof( FSYS.DefaultFontName ), FontName != NULL ? FontName : L"" ) ;
	FSYS.DefaultFontType     = FontType ;
	FSYS.DefaultFontSize     = Size ;
	FSYS.DefaultFontThick    = Thick ;
	if( FSYS.DefaultFontCharSet != CharSet )
	{
		FSYS.DefaultFontCharSet_Change = TRUE ;
		FSYS.DefaultFontCharSet = CharSet ;
	}
	FSYS.DefaultFontEdgeSize = EdgeSize ;
	FSYS.DefaultFontItalic   = Italic ;

	// 初期化前の場合はここで終了
	if( FSYS.InitializeFlag == FALSE )
	{
		FSYS.EnableInitDefaultFontName     = TRUE ;
		FSYS.EnableInitDefaultFontType     = TRUE ;
		FSYS.EnableInitDefaultFontSize     = TRUE ;
		FSYS.EnableInitDefaultFontThick    = TRUE ;
		FSYS.EnableInitDefaultFontCharSet  = TRUE ;
		FSYS.EnableInitDefaultFontEdgeSize = TRUE ;
		FSYS.EnableInitDefaultFontItalic   = TRUE ;

		return 0 ;
	}
	
	RefreshDefaultFont() ;

	// 終了
	return 0 ;
}

// デフォルトフォントのステータスを一括設定する
extern int NS_SetDefaultFontState( const TCHAR *FontName, int Size, int Thick, int FontType, int CharSet, int EdgeSize, int Italic )
{
#ifdef UNICODE
	return SetDefaultFontState_WCHAR_T( FontName, Size, Thick, FontType, CharSet, EdgeSize, Italic ) ;
#else
	wchar_t DefaultFontNameBuffer[ 256 ] ;
	wchar_t *TempFontNameBuffer = NULL ;
	wchar_t *UseFontNameBuffer ;
	int Length ;
	int Result ;

	Length = ( int )_STRLEN( FontName ) ;
	if( Length * 2 > 255 )
	{
		TempFontNameBuffer = ( wchar_t * )DXALLOC( sizeof( wchar_t ) * ( Length * 2 + 1 ) ) ;
		if( TempFontNameBuffer == NULL )
		{
			return -1 ;
		}
		UseFontNameBuffer = TempFontNameBuffer ;
	}
	else
	{
		UseFontNameBuffer = DefaultFontNameBuffer ;
	}
	ConvString( ( const char * )FontName, Length, _TCHARCODEFORMAT, ( char * )UseFontNameBuffer, sizeof( wchar_t ) * ( Length * 2 + 1 ), WCHAR_T_CHARCODEFORMAT ) ;
	Result = SetDefaultFontState_WCHAR_T( UseFontNameBuffer, Size, Thick, FontType, CharSet, EdgeSize, Italic ) ;

	if( TempFontNameBuffer != NULL )
	{
		DXFREE( TempFontNameBuffer ) ;
	}

	return Result ;
#endif
}

// デフォルトフォントハンドルの設定を変更する
extern int NS_SetDefaultFontStateWithStrLen( const TCHAR *FontName, size_t FontNameLength, int Size, int Thick, int FontType, int CharSet, int EdgeSize, int Italic )
{
	int Result ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_ONE_BEGIN( FontName, FontNameLength, return -1 )
	Result = NS_SetDefaultFontState( UseFontNameBuffer, Size, Thick, FontType, CharSet, EdgeSize, Italic ) ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( FontName )
	return Result ;
}

// フォントキャッシュの制御を終了する
extern int NS_DeleteFontToHandle( int FontHandle )
{
	return SubHandle( FontHandle ) ;
}

// 解放時に TRUE にするフラグへのポインタを設定する
extern int NS_SetFontLostFlag( int FontHandle, int *LostFlag )
{
	FONTMANAGE * ManageData ;

	DEFAULT_FONT_HANDLE_SETUP

	// エラー判定
	if( FONTHCHK( FontHandle, ManageData ) )
		return -1 ;

	ManageData->LostFlag = LostFlag ;
	if( LostFlag != NULL ) *LostFlag = FALSE ;

	return 0 ;
}

// 指定の文字の代わりに描画するグラフィックハンドルを登録する
extern int AddFontImageToHandle_WCHAR_T( int FontHandle, const wchar_t *Char, int GrHandle, int DrawX, int DrawY, int AddX )
{
#ifndef DX_NON_GRAPHICS
	DWORD CodeUnicode ;
	int UseSize ;
	int SizeX ;
	int SizeY ;
	int i ;
	FONTMANAGE * ManageData ;
	FONTCHARDATA *fontcode ;

	DEFAULT_FONT_HANDLE_SETUP

	// エラー判定
	if( FONTHCHK( FontHandle, ManageData ) )
	{
		return -1  ;
	}

	// コードを取得
	CodeUnicode = GetCharCode( ( const char * )Char, WCHAR_T_CHARCODEFORMAT, &UseSize ) ;

	// 既に同じ文字が登録されていたら上書きする
	fontcode = ManageData->GraphHandleFontImage ;
	for( i = 0 ; i < ManageData->GraphHandleFontImageNum ; i ++, fontcode ++ )
	{
		if( fontcode->CodeUnicode == CodeUnicode )
		{
			break ;
		}
	}

	// 無かった場合は情報を一つ増やす
	if( i == ManageData->GraphHandleFontImageNum )
	{
		// 既に登録数が一杯の場合はエラー
		if( ManageData->GraphHandleFontImageNum >= FONT_GRAPHICHANDLE_IMAGE_MAXNUM )
		{
			return -1 ;
		}

		ManageData->GraphHandleFontImageNum ++ ;
	}

	// 情報を設定
	fontcode->CodeUnicode     = CodeUnicode ;
	fontcode->ValidFlag       = TRUE ;
	fontcode->GraphHandleFlag = TRUE ;
	fontcode->DrawX           = ( short )DrawX ;
	fontcode->DrawY           = ( short )DrawY ;
	fontcode->AddX            = ( short )AddX ;
	NS_GetGraphSize( GrHandle, &SizeX, &SizeY ) ;
	fontcode->SizeX           = ( WORD )SizeX ;
	fontcode->SizeY           = ( WORD )SizeY ;
	fontcode->GraphIndex      = GrHandle ;
	fontcode->CodeData        = NULL ;

	// 正常終了
	return 0 ;
#else // DX_NON_GRAPHICS
	return -1 ;
#endif // DX_NON_GRAPHICS
}

// 指定の文字の代わりに描画するグラフィックハンドルを登録する
extern int NS_AddFontImageToHandle( int FontHandle, const TCHAR *Char, int GrHandle, int DrawX, int DrawY, int AddX )
{
#ifdef UNICODE
	return AddFontImageToHandle_WCHAR_T( FontHandle, Char, GrHandle, DrawX, DrawY, AddX ) ;
#else
	int Result ;
	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_BEGIN( Char, -1 )

	Result = AddFontImageToHandle_WCHAR_T( FontHandle, UseStringBuffer, GrHandle, DrawX, DrawY, AddX ) ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_END

	return Result ;
#endif
}

// 指定の文字の代わりに描画するグラフィックハンドルを登録する
extern int NS_AddFontImageToHandleWithStrLen( int FontHandle, const TCHAR *Char, size_t CharLength, int GrHandle, int DrawX, int DrawY, int AddX )
{
	int Result ;
#ifdef UNICODE
	FONTHANDLE_WCHAR_T_WITH_STRLENGTH_TO_WCHAR_T_STRING_BEGIN( Char, CharLength, -1 )
	Result = AddFontImageToHandle_WCHAR_T( FontHandle, UseStringBuffer, GrHandle, DrawX, DrawY, AddX ) ;
	FONTHANDLE_WCHAR_T_TO_WCHAR_T_STRING_END
#else
	FONTHANDLE_TCHAR_WITH_STRLENGTH_TO_WCHAR_T_STRING_BEGIN( Char, CharLength, -1 )
	Result = AddFontImageToHandle_WCHAR_T( FontHandle, UseStringBuffer, GrHandle, DrawX, DrawY, AddX ) ;
	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_END
#endif
	return Result ;
}

// 指定の文字の代わりに描画するグラフィックハンドルの登録を解除する
extern int SubFontImageToHandle_WCHAR_T( int FontHandle, const wchar_t *Char )
{
	DWORD CodeUnicode ;
	int UseSize ;
	int i ;
	FONTMANAGE * ManageData ;
	FONTCHARDATA *fontcode ;

	DEFAULT_FONT_HANDLE_SETUP

	// エラー判定
	if( FONTHCHK( FontHandle, ManageData ) )
	{
		return -1  ;
	}

	// コードを取得
	CodeUnicode = GetCharCode( ( const char * )Char, WCHAR_T_CHARCODEFORMAT, &UseSize ) ;

	// 指定の文字が登録されていなかったらエラー
	fontcode = ManageData->GraphHandleFontImage ;
	for( i = 0 ; i < ManageData->GraphHandleFontImageNum ; i ++, fontcode ++ )
	{
		if( fontcode->CodeUnicode == CodeUnicode )
		{
			break ;
		}
	}
	if( i == ManageData->GraphHandleFontImageNum )
	{
		return -1 ;
	}

	// 情報の数を減らす
	ManageData->GraphHandleFontImageNum -- ;

	// 末端では無かった場合情報を詰める
	if( i != ManageData->GraphHandleFontImageNum )
	{
		_MEMMOVE(
			&ManageData->GraphHandleFontImage[ i     ],
			&ManageData->GraphHandleFontImage[ i + 1 ],
			sizeof( FONTCHARDATA ) * ( ManageData->GraphHandleFontImageNum - i )
		) ;
	}

	// 正常終了
	return 0 ;
}

// 指定の文字の代わりに描画するグラフィックハンドルの登録を解除する
extern int NS_SubFontImageToHandle( int FontHandle, const TCHAR *Char )
{
#ifdef UNICODE
	return SubFontImageToHandle_WCHAR_T( FontHandle, Char ) ;
#else
	int Result ;
	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_BEGIN( Char, -1 )

	Result = SubFontImageToHandle_WCHAR_T( FontHandle, UseStringBuffer ) ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_END

	return Result ;
#endif
}

// 指定の文字の代わりに描画するグラフィックハンドルの登録を解除する
extern int NS_SubFontImageToHandleWithStrLen( int FontHandle, const TCHAR *Char, size_t CharLength )
{
	int Result ;
#ifdef UNICODE
	FONTHANDLE_WCHAR_T_WITH_STRLENGTH_TO_WCHAR_T_STRING_BEGIN( Char, CharLength, -1 )
	Result = SubFontImageToHandle_WCHAR_T( FontHandle, UseStringBuffer ) ;
	FONTHANDLE_WCHAR_T_TO_WCHAR_T_STRING_END
#else
	FONTHANDLE_TCHAR_WITH_STRLENGTH_TO_WCHAR_T_STRING_BEGIN( Char, CharLength, -1 )
	Result = SubFontImageToHandle_WCHAR_T( FontHandle, UseStringBuffer ) ;
	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_END
#endif
	return Result ;
}

// 代替フォントハンドルを登録する
extern int NS_AddSubstitutionFontToHandle( int FontHandle, int SubstitutionFontHandle, int DrawX, int DrawY )
{
	FONTMANAGE * ManageData ;

	DEFAULT_FONT_HANDLE_SETUP

	// エラー判定
	if( FONTHCHK( FontHandle, ManageData ) )
	{
		return -1  ;
	}

	// 登録できる数の最大数に達していたらエラー
	if( ManageData->SubstitutionInfoNum == FONT_SUBSTITUTION_MAXNUM )
	{
		return -1 ;
	}

	ManageData->SubstitutionInfo[ ManageData->SubstitutionInfoNum ].SubstitutionFontHandle = SubstitutionFontHandle ;
	ManageData->SubstitutionInfo[ ManageData->SubstitutionInfoNum ].DrawX = DrawX ;
	ManageData->SubstitutionInfo[ ManageData->SubstitutionInfoNum ].DrawY = DrawY ;

	ManageData->SubstitutionInfoNum ++ ;

	// 正常終了
	return 0 ;
}

// 代替フォントハンドルの登録を解除する
extern int NS_SubSubstitutionFontToHandle( int FontHandle, int SubstitutionFontHandle )
{
	FONTMANAGE * ManageData ;
	int i ;

	DEFAULT_FONT_HANDLE_SETUP

	// エラー判定
	if( FONTHCHK( FontHandle, ManageData ) )
	{
		return -1  ;
	}

	for( i = 0 ; i < ManageData->SubstitutionInfoNum ; i ++ )
	{
		if( ManageData->SubstitutionInfo[ i ].SubstitutionFontHandle == SubstitutionFontHandle )
		{
			break ;
		}
	}

	// 指定のフォントが見つからなかったらエラー
	if( i == ManageData->SubstitutionInfoNum )
	{
		return -1 ;
	}

	ManageData->SubstitutionInfoNum -- ;
	if( ManageData->SubstitutionInfoNum - i > 0 )
	{
		_MEMMOVE( &ManageData->SubstitutionInfo[ i ], &ManageData->SubstitutionInfo[ i + 1 ], sizeof( FONTSUBSTITUTIONINFO ) * ( ManageData->SubstitutionInfoNum - i ) ) ;
	}

	// 正常終了
	return 0 ;
}

// デフォルトフォントハンドルのフォント名を取得する
extern const TCHAR *NS_GetFontName( void )
{
	return NS_GetFontNameToHandle( DX_DEFAULT_FONT_HANDLE ) ;
}

// 描画するフォントのサイズをセットする
extern int NS_SetFontSize( int FontSize )
{
	if( FontSize == -1 ) FontSize = DEFAULT_FONT_SIZE ;

	FSYS.DefaultFontSize = FontSize ;

	// 初期化前の場合はここで終了
	if( FSYS.InitializeFlag == FALSE )
	{
		FSYS.EnableInitDefaultFontSize = TRUE ;

		return 0 ;
	}

	RefreshDefaultFont() ;

	// 終了
	return 0 ;
}


// フォントの太さをセット
extern int NS_SetFontThickness( int ThickPal )
{
	if( ThickPal == -1 ) ThickPal = DEFAULT_FONT_THINCK ;

	FSYS.DefaultFontThick = ThickPal ;

	// 初期化前の場合はここで終了
	if( FSYS.InitializeFlag == FALSE )
	{
		FSYS.EnableInitDefaultFontThick = TRUE ;

		return 0 ;
	}

	RefreshDefaultFont() ;

	// 終了
	return 0 ;
}

// 字間を変更する
extern int NS_SetFontSpace( int Pixel )
{
	// 初期化前の場合はここで終了
	if( FSYS.InitializeFlag == FALSE )
	{
		FSYS.DefaultFontSpace = Pixel ;

		FSYS.EnableInitDefaultFontSpace = TRUE ;

		return 0 ;
	}

	RefreshDefaultFont() ;

	FSYS.DefaultFontSpace = Pixel ;

	NS_SetFontSpaceToHandle( Pixel, FSYS.DefaultFontHandle ) ;

	// 終了
	return 0 ;
}

// デフォルトフォントハンドルの字間を取得する
extern int NS_GetFontSpace( void )
{
	return NS_GetFontSpaceToHandle( DX_DEFAULT_FONT_HANDLE ) ;
}

// デフォルトフォントハンドルの行間を変更する
extern int NS_SetFontLineSpace( int Pixel )
{
	// 初期化前の場合はここで終了
	if( FSYS.InitializeFlag == FALSE )
	{
		FSYS.DefaultFontLineSpaceValid = TRUE ;
		FSYS.DefaultFontLineSpace = Pixel ;

		FSYS.EnableInitDefaultFontLineSpace = TRUE ;

		return 0 ;
	}

	RefreshDefaultFont() ;

	FSYS.DefaultFontLineSpaceValid = TRUE ;
	FSYS.DefaultFontLineSpace = Pixel ;

	NS_SetFontLineSpaceToHandle( Pixel, FSYS.DefaultFontHandle ) ;

	// 終了
	return 0 ;
}

// デフォルトフォントハンドルの行間を取得する
extern int NS_GetFontLineSpace( void )
{
	return NS_GetFontLineSpaceToHandle( DX_DEFAULT_FONT_HANDLE ) ;
}

// デフォルトフォントハンドルを使用する関数の引数に渡す文字列の文字コード形式を設定する( UNICODE版では無効 )
extern int NS_SetFontCharCodeFormat( int CharCodeFormat /* DX_CHARCODEFORMAT_SHIFTJIS 等 */ )
{
	return NS_SetFontCharCodeFormatToHandle( CharCodeFormat, DX_DEFAULT_FONT_HANDLE ) ;
}

// フォントのキャッシュにテクスチャを使用するか、フラグをセットする
extern int NS_SetFontCacheToTextureFlag( int Flag )
{
	// フラグを保存
	FSYS.NotTextureFlag = !Flag ;
	
	// 終了
	return 0 ;
}

// フォントのキャッシュにテクスチャを使用するか、フラグをセットする(誤字版)
extern int NS_SetFontChacheToTextureFlag( int Flag )
{
	return NS_SetFontCacheToTextureFlag( Flag ) ;
}

// フォントキャッシュでキャッシュできる文字数を指定する
extern int NS_SetFontCacheCharNum( int CharNum )
{
	FSYS.CacheCharNum = CharNum ;

	// 終了
	return 0 ;
}


// デフォルトフォントハンドルで使用するフォントを変更
extern int ChangeFont_WCHAR_T( const wchar_t *FontName, int CharSet )
{
	_WCSCPY_S( FSYS.DefaultFontName, sizeof( FSYS.DefaultFontName ), FontName != NULL ? FontName : L"" ) ;

//	if( CharSet == -1 )
//		CharSet = _GET_CHARSET() ;

	if( FSYS.DefaultFontCharSet != CharSet )
	{
		FSYS.DefaultFontCharSet_Change = TRUE ;
		FSYS.DefaultFontCharSet = CharSet ;
	}

	// 初期化前の場合はここで終了
	if( FSYS.InitializeFlag == FALSE )
	{
		FSYS.EnableInitDefaultFontName = TRUE ;
		FSYS.EnableInitDefaultFontCharSet = TRUE ;

		return 0 ;
	}

	RefreshDefaultFont() ;

	// 終了
	return 0 ;
}

// フォントを変更
extern int NS_ChangeFont( const TCHAR *FontName, int CharSet )
{
#ifdef UNICODE
	return ChangeFont_WCHAR_T( FontName, CharSet ) ;
#else
	wchar_t DefaultFontNameBuffer[ 256 ] ;
	wchar_t *TempFontNameBuffer = NULL ;
	wchar_t *UseFontNameBuffer ;
	int Length ;
	int Result ;

	Length = ( int )_STRLEN( FontName ) ;
	if( Length * 2 > 255 )
	{
		TempFontNameBuffer = ( wchar_t * )DXALLOC( sizeof( wchar_t ) * ( Length * 2 + 1 ) ) ;
		if( TempFontNameBuffer == NULL )
		{
			return -1 ;
		}
		UseFontNameBuffer = TempFontNameBuffer ;
	}
	else
	{
		UseFontNameBuffer = DefaultFontNameBuffer ;
	}
	ConvString( ( const char * )FontName, Length, _TCHARCODEFORMAT, ( char * )UseFontNameBuffer, sizeof( wchar_t ) * ( Length * 2 + 1 ), WCHAR_T_CHARCODEFORMAT ) ;
	Result = ChangeFont_WCHAR_T( UseFontNameBuffer, CharSet ) ;

	if( TempFontNameBuffer != NULL )
	{
		DXFREE( TempFontNameBuffer ) ;
	}

	return Result ;
#endif
}

// デフォルトフォントハンドルで使用するフォントを変更
extern int NS_ChangeFontWithStrLen( const TCHAR *FontName, size_t FontNameLength, int CharSet )
{
	int Result ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_ONE_BEGIN( FontName, FontNameLength, return -1 )
	Result = NS_ChangeFont( UseFontNameBuffer, CharSet ) ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( FontName )
	return Result ;
}

// フォントタイプの変更
extern int NS_ChangeFontType( int FontType )
{
	if( FontType == -1 )
	{
		FontType = FSYS.AntialiasingFontOnlyFlag ? DX_FONTTYPE_ANTIALIASING : DEFAULT_FONT_TYPE ;
	}

	FSYS.DefaultFontType = FontType ;

	// 初期化前の場合はここで終了
	if( FSYS.InitializeFlag == FALSE )
	{
		FSYS.EnableInitDefaultFontType = TRUE ;

		return 0 ;
	}

	RefreshDefaultFont() ;

	// 終了
	return 0 ;
}





// 指定の文字コードのフォントキャッシュデータを取得する、キャッシュ内に無い場合はキャッシュへの追加を試みて、失敗したら NULL を返す
__inline FONTCHARDATA *GetFontCacheChar_Inline( FONTMANAGE *ManageData, DWORD CharCode, FONTMANAGE **UseManageData, int *DrawOffsetX, int *DrawOffsetY, int EnableGraphHandleFontImage, int ErrorMessage )
{
	FONTCHARDATA *Cache ;
	int i ;

	*UseManageData = ManageData ;
	*DrawOffsetX = 0 ;
	*DrawOffsetY = 0 ;

	if( EnableGraphHandleFontImage )
	{
		// 画像置き換え文字のチェック
		for( i = 0 ; i < ManageData->GraphHandleFontImageNum ; i ++ )
		{
			if( ManageData->GraphHandleFontImage[ i ].CodeUnicode == CharCode )
			{
				return &ManageData->GraphHandleFontImage[ i ] ;
			}
		}
	}

	// コードがテーブルに入る範囲かどうかで処理を分岐
	if( CharCode > 0xffff )
	{
		// テーブルに入らない場合は配列から探す
		for( i = 0 ; i < ManageData->CodeDataExNum ; i ++ )
		{
			if( ManageData->CodeDataEx[ i ]->CodeUnicode == CharCode )
			{
				return ManageData->CodeDataEx[ i ] ;
			}
		}
	}
	else
	{
		// テーブルに入る場合はテーブルを使って有無をチェック
		if( ManageData->CodeData[ CharCode ].ExistFlag )
		{
			// あった場合そのフォントデータのポインタを保存
			return ManageData->CodeData[ CharCode ].CharData ; 
		}
	}

	// 無かった場合キャッシュに追加
	Cache = FontCacheCharAddToHandle( 1, &CharCode, ManageData ) ;
	if( Cache == NULL )
	{
		FONTMANAGE *SubstitutionManageData ;
		int TempDrawOffsetX ;
		int TempDrawOffsetY ;

		// キャッシュへの追加が失敗した場合は代替フォントを使用する
SUBSTITUTIONSTART :		
		for( i = 0 ; i < ManageData->SubstitutionInfoNum ; i++ )
		{
			if( FONTHCHK( ManageData->SubstitutionInfo[ i ].SubstitutionFontHandle, SubstitutionManageData ) )
			{
				// 無効になっているフォントハンドルがあったら外す
				NS_SubSubstitutionFontToHandle( ManageData->HandleInfo.Handle, ManageData->SubstitutionInfo[ i ].SubstitutionFontHandle ) ;
				goto SUBSTITUTIONSTART ;
			}

			Cache = GetFontCacheChar_Inline( SubstitutionManageData, CharCode, UseManageData, &TempDrawOffsetX, &TempDrawOffsetY, EnableGraphHandleFontImage, FALSE ) ;
			if( Cache != NULL )
			{
				*DrawOffsetX = *DrawOffsetX + TempDrawOffsetX + ManageData->SubstitutionInfo[ i ].DrawX ;
				*DrawOffsetY = *DrawOffsetY + TempDrawOffsetY + ManageData->SubstitutionInfo[ i ].DrawY ;
				break ;
			}
		}

		if( Cache == NULL && ErrorMessage )
		{
			DXST_LOGFILE_ADDUTF16LE( GetFontCacheChar_ErrorMessage ) ;
		}
	}

	return Cache ;
}

extern FONTCHARDATA *GetFontCacheChar( FONTMANAGE *ManageData, DWORD CharCode, FONTMANAGE **UseManageData, int *DrawOffsetX, int *DrawOffsetY, int EnableGraphHandleFontImage, int ErrorMessage )
{
	return GetFontCacheChar_Inline( ManageData, CharCode, UseManageData, DrawOffsetX, DrawOffsetY, EnableGraphHandleFontImage, ErrorMessage ) ;
}

// 文字列を描画
extern int FontCacheStringDrawToHandleST(
	int				DrawFlag,
	int				xi,
	int				yi,
	float			xf,
	float			yf,
	int				PosIntFlag,
	int				ExRateValidFlag,
	double			ExRateX,
	double			ExRateY,
	int				RotateValidFlag,
	float			RotCenterX,
	float			RotCenterY,
	double			RotAngle, 
	const wchar_t *	StrData,
	unsigned int	Color,
	MEMIMG *		DestMemImg,
	const RECT *	ClipRect,
	int				TransFlag,
	FONTMANAGE *	ManageData,
	unsigned int	EdgeColor,
	int				StrLen,
	int				VerticalFlag,
	SIZE *			DrawSize,
	int *			LineCount,
	DRAWCHARINFO *	CharInfos,
	size_t			CharInfoBufferSize,
	int *			CharInfoNum,
	int				OnlyType /* 0:通常描画 1:本体のみ 2:縁のみ */
)
{
	static DWORD	DrawStrBuf[ 2048 ] ;		// 描画する際に使用する文字列データ（３バイトで１文字を表現）
	FONTCHARDATA *	CharData ;
	FONTMANAGE *	UseManageData = ManageData ;
#ifndef DX_NON_GRAPHICS
	RECT			SrcRect = { 0 } ;
	RECT			DstRect ;
#endif // DX_NON_GRAPHICS
	int				DrawCharNum ;				// ２バイト文字１バイト文字全部ひっくるめて何文字あるか保存する変数
	int				i ;							// 繰り返しと汎用変数
#ifndef DX_NON_GRAPHICS
	int				SrcX = 0 ;					// テキストキャッシュ中の目的テキストの座標決定に使用
	int				SrcY = 0 ;
#endif // DX_NON_GRAPHICS
	const DWORD *	CharCode ;
	double			DrawPos ;
	double			MaxDrawPos ;
	double			DrawPosSub ;
	double			DrawPosSubAdd ;
	double			ExRate ;
#ifndef DX_NON_GRAPHICS
	DWORD			OrigColor ;
	DWORD			FColor ;
	DWORD			FEdgeColor ;
#endif // DX_NON_GRAPHICS
	DWORD			DstPitch = 0 ;
	DWORD			SrcPitch ;
	int				UseAlloc = FALSE ;
	DWORD *			AllocDrawStrBuf = NULL ;
	DWORD *			UseDrawStrBuf ;
	float			RotSin = 0.0f ;
	float			RotCos = 1.0f ;
	int				LFCount = 0 ;
	int				CInfoNum = 0 ;
	int				Result = -1 ;
	int				DrawOffsetX = 0 ;
	int				DrawOffsetY = 0 ;
	int				tmp_xi = 0 ;
	int				tmp_yi = 0 ;
	float			tmp_xf = 0.0f ;
	float			tmp_yf = 0.0f ;
	int				FontAddHeightXI = 0 ;
	int				FontAddHeightYI = 0 ;
	float			FontAddHeightXF = 0.0f ;
	float			FontAddHeightYF = 0.0f ;

//	DEFAULT_FONT_HANDLE_SETUP

#ifndef DX_NON_GRAPHICS
	// エラー判定
	if( ManageData->TextureCacheFlag && Graphics_Hardware_CheckValid_PF() == 0 )
	{
		goto END ;
	}
#endif // DX_NON_GRAPHICS

	// ディフューズカラーを使用するかどうかでデバイスのステートが変化する環境では
	// 必ずディフューズカラーを使用するようにする
	GSYS.DrawSetting.AlwaysDiffuseColorFlag = TRUE ;

//	if( FONTHCHK( FontHandle, ManageData ) )
//	{
//		goto END ;
//	}
//	DXST_LOGFILE_ADDW( L"フォントハンドル値が異常です終了します\n" ) ;
//	DXST_LOGFILEFMT_ADDW(( L"	in FontCacheStringDrawToHandle  StrData = %s   FontHandle = %d  ", StrData, FontHandle )) ;

	// 初期値をセット
	if( DrawSize != NULL )
	{
		DrawSize->cx = 0 ;
		DrawSize->cy = 0 ;
	}
	if( LineCount != NULL )
	{
		*LineCount = 0 ;
	}

	// ソフトウエアレンダリングの場合は整数値による描画
	if( DestMemImg != NULL || ManageData->TextureCacheFlag == FALSE )
	{
		PosIntFlag      = TRUE ;

		// ソフトウエアレンダリングでは拡大描画や回転描画はできない
		if( ExRateValidFlag || RotateValidFlag )
		{
			goto END ;
		}
	}

	// 値の補正
	if( ExRateValidFlag == FALSE )
	{
		ExRateX = 1.0 ;
		ExRateY = 1.0 ;
	}
	if( RotateValidFlag == FALSE )
	{
		RotCenterX = 0.0f ;
		RotCenterY = 0.0f ;
		RotAngle   = 0.0 ;
	}

	// 回転が有効な場合は Sin と Cos の値を取得しておく
	if( RotateValidFlag )
	{
		// 縦書きの場合は 90度プラスする
		if( VerticalFlag == TRUE )
		{
			RotAngle += DX_PI / 2.0 ;
		}

		_SINCOS_PLATFORM( ( float )RotAngle, &RotSin, &RotCos ) ;
	}

	// 描画座標の補正
	if( ManageData->BaseInfo.FontAddHeight != 0 )
	{
		if( VerticalFlag )
		{
			if( PosIntFlag )
			{
				FontAddHeightXI =     -_DTOL( ManageData->BaseInfo.FontAddHeight * ExRateY ) / 2 ;
			}
			else
			{
				FontAddHeightXF = -( float )( ManageData->BaseInfo.FontAddHeight * ExRateY ) / 2.0f ;
			}
		}
		else
		{
			if( PosIntFlag )
			{
				FontAddHeightYI =     -_DTOL( ManageData->BaseInfo.FontAddHeight * ExRateY ) / 2 ;
			}
			else
			{
				FontAddHeightYF = -( float )( ManageData->BaseInfo.FontAddHeight * ExRateY ) / 2.0f ;
			}
		}
	}

//	if( DrawFlag && ExRateValidFlag == FALSE || ManageData->TextureCacheFlag == FALSE )
//	{
//		ExRateValidFlag = FALSE ;
//		ExRateX = 1.0 ;
//		ExRateY = 1.0 ;
//	}

	ExRate = VerticalFlag ? ExRateY : ExRateX ;

#ifndef DX_NON_GRAPHICS
	// ３２ビット色カラー値を取得しておく
	{
		int r, g, b ;
		int er, eg, eb ;
		DWORD NoneRevMask ;
	
		// ＤＸライブラリに設定されている描画輝度と乗算する
		GetColor_Macro( 0, 0, 0, NoneRevMask ) ;
		NoneRevMask = ~NoneRevMask ;
		GetColor2_Macro( Color,     r,  g,  b  ) ;
		GetColor2_Macro( EdgeColor, er, eg, eb ) ;
		if( ( GSYS.DrawSetting.bDrawBright & 0x00ffffff ) != 0x00ffffff )
		{
			RGBCOLOR *bright ;

			bright = &GSYS.DrawSetting.DrawBright ;	

			r  = bright->Red   * r  / 255 ;
			g  = bright->Green * g  / 255 ;
			b  = bright->Blue  * b  / 255 ;

			er = bright->Red   * er / 255 ;
			eg = bright->Green * eg / 255 ;
			eb = bright->Blue  * eb / 255 ;

			GetColor_Macro( r,   g,  b, Color     ) ;
			GetColor_Macro( er, eg, eb, EdgeColor ) ;
		}
		Color      &= NoneRevMask ;
		EdgeColor  &= NoneRevMask ;

		FColor     = ( 0xff000000 ) | ( r  << 16 ) | ( g  << 8 ) | (  b ) ;
		FEdgeColor = ( 0xff000000 ) | ( er << 16 ) | ( eg << 8 ) | ( eb ) ;
	}
#endif // DX_NON_GRAPHICS

	// 文字列がない場合は終了
	if( StrData == NULL )
	{
		Result = 0 ;
		goto END ;
	}
	if( StrData[ 0 ] == 0 )
	{
		Result = 0 ;
		goto END ;
	}

	// 描画する文字列の長さを保存
	{
		if( StrLen < 0 )
		{
			StrLen = ( int )_WCSLEN( StrData ) ;
		}

		if( StrLen > 1024 )
		{
			UseAlloc = TRUE ;
			AllocDrawStrBuf = ( DWORD * )DXALLOC( StrLen * sizeof( DWORD ) ) ;
			if( AllocDrawStrBuf == NULL )
			{
				goto END ;
			}
			UseDrawStrBuf = AllocDrawStrBuf ;
		}
		else
		{
			UseDrawStrBuf = DrawStrBuf ;
		}
	}

	// ピッチのセット
	if( DestMemImg != NULL )
	{
		DstPitch = DestMemImg->Base->Pitch ;
	}

	// 改行時にＹ座標に加算する値を算出
	if( VerticalFlag == TRUE )
	{
		DrawPosSubAdd = ( ManageData->LineSpaceValidFlag ? ManageData->LineSpace : ManageData->BaseInfo.FontHeight ) * ExRateX ;
	}
	else
	{
		DrawPosSubAdd = ( ManageData->LineSpaceValidFlag ? ManageData->LineSpace : ManageData->BaseInfo.FontHeight ) * ExRateY ;
	}

	// キャッシュに入っていない文字列をキャッシュに入れる
	FontCacheStringAddToHandle( ManageData, StrData, StrLen, UseDrawStrBuf, &DrawCharNum ) ;

#ifndef DX_NON_GRAPHICS
	if( DrawFlag == TRUE )
	{
		float AdjustPos = 0.0f ;

		// 座標補正値をセット
		if( ManageData->TextureCacheFlag == TRUE )
		{
			AdjustPos = NS_GetDrawFloatCoordType() == DX_DRAWFLOATCOORDTYPE_DIRECT3D9 ? -0.5f : 0.0f ;
		}

		// 描画色を保存しておく
		OrigColor = GSYS.DrawSetting.bDrawBright ;

		// 縁なしの場合は最初に描画色を設定する
		if( ( ManageData->FontType & DX_FONTTYPE_EDGE ) == 0 )
		{
			Graphics_DrawSetting_SetDrawBrightToOneParam( FColor ) ;
		}

		// 文字の数だけ繰り返し
		CharCode = UseDrawStrBuf ;			// 描画用文字データの先頭アドレスをセット
		DrawPos = 0.0 ;
		MaxDrawPos = 0.0 ;
		DrawPosSub = 0.0 ;
		for( i = 0 ; i < DrawCharNum ; i ++, CharCode ++ )
		{
			CharData = NULL ;
			UseManageData = ManageData ;

			// キャッシュデータを取得
			CharData = GetFontCacheChar_Inline( ManageData, *CharCode, &UseManageData, &DrawOffsetX, &DrawOffsetY, TRUE, FALSE ) ;
			if( CharData == NULL )
			{
				goto LOOPEND ;
			}

			// 描画座標の準備
			if( ManageData == UseManageData )
			{
				if( PosIntFlag )
				{
					tmp_xi = xi + FontAddHeightXI ;
					tmp_yi = yi + FontAddHeightYI ;
				}
				else
				{
					tmp_xf = xf + FontAddHeightXF ;
					tmp_yf = yf + FontAddHeightYF ;
				}
			}
			else
			{
				tmp_xi = xi ;
				tmp_yi = yi ;
				if( VerticalFlag )
				{
					if( PosIntFlag )
					{
						tmp_xi += _DTOL( DrawOffsetY * ExRateY ) ;
						tmp_yi += _DTOL( DrawOffsetX * ExRateX ) ;
					}
					else
					{
						tmp_xf += ( float )( DrawOffsetY * ExRateY ) ;
						tmp_yf += ( float )( DrawOffsetX * ExRateX ) ;
					}
				}
				else
				{
					if( PosIntFlag )
					{
						tmp_xi += _DTOL( DrawOffsetX * ExRateX ) ;
						tmp_yi += _DTOL( DrawOffsetY * ExRateY ) ;
					}
					else
					{
						tmp_xf += ( float )( DrawOffsetX * ExRateX ) ;
						tmp_yf += ( float )( DrawOffsetY * ExRateY ) ;
					}
				}

				if( UseManageData->BaseInfo.FontAddHeight != 0 )
				{
					if( VerticalFlag )
					{
						if( PosIntFlag )
						{
							tmp_xi -=     _DTOL( UseManageData->BaseInfo.FontAddHeight * ExRateY ) / 2 ;
						}
						else
						{
							tmp_xf -= ( float )( UseManageData->BaseInfo.FontAddHeight * ExRateY ) / 2.0f ;
						}
					}
					else
					{
						if( PosIntFlag )
						{
							tmp_yi -=     _DTOL( UseManageData->BaseInfo.FontAddHeight * ExRateY ) / 2 ;
						}
						else
						{
							tmp_yf -= ( float )( UseManageData->BaseInfo.FontAddHeight * ExRateY ) / 2.0f ;
						}
					}
				}
			}

			// 画像データが存在する矩形を算出
			if( CharData->GraphHandleFlag == FALSE )
			{
				SrcY = CharData->GraphIndexY * UseManageData->BaseInfo.MaxWidth ;
				SrcX = CharData->GraphIndexX * UseManageData->BaseInfo.MaxWidth ;
				if( UseManageData->TextureCacheFlag == TRUE )
				{
					SrcX += 1 ;
					SrcY += 1 ;
				}
				SETRECT( SrcRect, SrcX, SrcY, SrcX + CharData->SizeX, SrcY + CharData->SizeY ) ;
			}

			// ピッチのセット
			SrcPitch = ( DWORD )UseManageData->CachePitch ;

			// 改行文字だった場合はキャンセルする
			if( *CharCode == L'\n' )
			{
				goto LOOPEND ;
			}

			// スペース文字だった場合はキャンセルする
			if( *CharCode == L' ' || ( wchar_t )*CharCode == FSYS.DoubleByteSpaceCharCode )
			{
				goto LOOPEND ;
			}

			// サイズが無かったら何もしない
			if( CharData->SizeX == 0 && CharData->SizeY == 0 ) 
			{
				goto LOOPEND ;
			}

			// 文字の描画
			if( UseManageData->TextureCacheFlag == TRUE ||
				CharData->GraphHandleFlag )
			{
//				int anti ;
				int BlendGraph, BorderParam, BorderRange ;

				// 既存のブレンド画像の情報を保存しておく
				BlendGraph  = GSYS.DrawSetting.BlendGraph ;
				BorderParam = GSYS.DrawSetting.BlendGraphBorderParam ;
				BorderRange = GSYS.DrawSetting.BlendGraphBorderRange ;

				// ブレンド座標タイプが DX_BLENDGRAPH_POSMODE_DRAWGRAPH の場合はブレンド画像を無効にする
				if( BlendGraph != -1 && GSYS.DrawSetting.BlendGraphPosMode == DX_BLENDGRAPH_POSMODE_DRAWGRAPH )
				{
					NS_SetBlendGraph( -1, 0, 0 ) ;
				}
			
				// アンチエイリアスフォントか
//				anti = ( UseManageData->FontType & DX_FONTTYPE_ANTIALIASING ) != 0 ;

				// 回転付き描画かどうかで処理を分岐
				if( RotateValidFlag || GSYS.DrawSetting.Valid2DMatrix )
				{
					float TexCoord1U ;
					float TexCoord1V ;
					float TexCoord2U ;
					float TexCoord2V ;
					float Pos1X ;
					float Pos1Y ;
					float Pos2X ;
					float Pos2Y ;
					float Pos1SinX ;
					float Pos1CosX ;
					float Pos1SinY ;
					float Pos1CosY ;
					float Pos2SinX ;
					float Pos2CosX ;
					float Pos2SinY ;
					float Pos2CosY ;
					float ExRateXF = ( float )ExRateX ;
					float ExRateYF = ( float )ExRateY ;

					// 回転描画

					if( VerticalFlag == TRUE )
					{
						if( CharData->GraphHandleFlag )
						{
							Pos1X = ( ( float )DrawPos    / ExRateYF + CharData->DrawX                   - RotCenterX ) * ExRateYF ;
							Pos1Y = ( ( float )DrawPosSub / ExRateXF + CharData->DrawY                   - RotCenterY ) * ExRateXF ;
							Pos2X = ( ( float )DrawPos    / ExRateYF + CharData->DrawX + CharData->SizeX - RotCenterX ) * ExRateYF ;
							Pos2Y = ( ( float )DrawPosSub / ExRateXF + CharData->DrawY + CharData->SizeY - RotCenterY ) * ExRateXF ;
						}
						else
						{
							Pos1X = ( ( float )DrawPos    / ExRateYF + ( CharData->DrawX - 1.0f )                              - RotCenterX ) * ExRateYF ;
							Pos1Y = ( ( float )DrawPosSub / ExRateXF + ( CharData->DrawY - 1.0f )                              - RotCenterY ) * ExRateXF ;
							Pos2X = ( ( float )DrawPos    / ExRateYF + ( CharData->DrawX - 1.0f ) + ( CharData->SizeX + 2.0f ) - RotCenterX ) * ExRateYF ;
							Pos2Y = ( ( float )DrawPosSub / ExRateXF + ( CharData->DrawY - 1.0f ) + ( CharData->SizeY + 2.0f ) - RotCenterY ) * ExRateXF ;
						}
					}
					else
					{
						if( CharData->GraphHandleFlag )
						{
							Pos1X = ( ( float )DrawPos    / ExRateXF + CharData->DrawX                   - RotCenterX ) * ExRateXF ;
							Pos1Y = ( ( float )DrawPosSub / ExRateYF + CharData->DrawY                   - RotCenterY ) * ExRateYF ;
							Pos2X = ( ( float )DrawPos    / ExRateXF + CharData->DrawX + CharData->SizeX - RotCenterX ) * ExRateXF ;
							Pos2Y = ( ( float )DrawPosSub / ExRateYF + CharData->DrawY + CharData->SizeY - RotCenterY ) * ExRateYF ;
						}
						else
						{
							Pos1X = ( ( float )DrawPos    / ExRateXF + ( CharData->DrawX - 1.0f )                              - RotCenterX ) * ExRateXF ;
							Pos1Y = ( ( float )DrawPosSub / ExRateYF + ( CharData->DrawY - 1.0f )                              - RotCenterY ) * ExRateYF ;
							Pos2X = ( ( float )DrawPos    / ExRateXF + ( CharData->DrawX - 1.0f ) + ( CharData->SizeX + 2.0f ) - RotCenterX ) * ExRateXF ;
							Pos2Y = ( ( float )DrawPosSub / ExRateYF + ( CharData->DrawY - 1.0f ) + ( CharData->SizeY + 2.0f ) - RotCenterY ) * ExRateYF ;
						}
					}

					Pos1SinX = Pos1X * RotSin ;
					Pos1CosX = Pos1X * RotCos ;
					Pos1SinY = Pos1Y * RotSin ;
					Pos1CosY = Pos1Y * RotCos ;
					Pos2SinX = Pos2X * RotSin ;
					Pos2CosX = Pos2X * RotCos ;
					Pos2SinY = Pos2Y * RotSin ;
					Pos2CosY = Pos2Y * RotCos ;

					if( PosIntFlag )
					{
						tmp_xf = ( float )tmp_xi + AdjustPos ;
						tmp_yf = ( float )tmp_yi + AdjustPos ;
					}

					// 画像置き換え文字かどうかで処理を分岐
					if( CharData->GraphHandleFlag )
					{
						Graphics_DrawSetting_SetDrawBrightToOneParam( FColor ) ;
						if( OnlyType == 0 || OnlyType == 1 )
						{
							NS_DrawModiGraphF(
								Pos1CosX - Pos1SinY + tmp_xf, Pos1SinX + Pos1CosY + tmp_yf,
								Pos2CosX - Pos1SinY + tmp_xf, Pos2SinX + Pos1CosY + tmp_yf,
								Pos2CosX - Pos2SinY + tmp_xf, Pos2SinX + Pos2CosY + tmp_yf,
								Pos1CosX - Pos2SinY + tmp_xf, Pos1SinX + Pos2CosY + tmp_yf,
								CharData->GraphIndex,
								TRUE
							) ;
						}
					}
					else
					{
						GRAPHICS_DRAW_DRAWSIMPLEQUADRANGLEGRAPHF_PARAM Param ;
						GRAPHICS_DRAW_DRAWSIMPLEANGLEGRAPHF_VERTEX Vertex[ 4 ] ;

						Param.QuadrangleNum = 1 ;
						Param.Vertex = Vertex ;

						Vertex[ 0 ].x = Pos1CosX - Pos1SinY + tmp_xf ;
						Vertex[ 0 ].y = Pos1SinX + Pos1CosY + tmp_yf ;

						Vertex[ 1 ].x = Pos2CosX - Pos1SinY + tmp_xf ;
						Vertex[ 1 ].y = Pos2SinX + Pos1CosY + tmp_yf ;

						Vertex[ 2 ].x = Pos1CosX - Pos2SinY + tmp_xf ;
						Vertex[ 2 ].y = Pos1SinX + Pos2CosY + tmp_yf ;

						Vertex[ 3 ].x = Pos2CosX - Pos2SinY + tmp_xf ;
						Vertex[ 3 ].y = Pos2SinX + Pos2CosY + tmp_yf ;

						TexCoord1U = ( SrcRect.left   - 1     ) * UseManageData->TextureCacheInvSizeX ;
						TexCoord1V = ( SrcRect.top    - 1     ) * UseManageData->TextureCacheInvSizeY ;
						TexCoord2U = ( SrcRect.right  - 1 + 2 ) * UseManageData->TextureCacheInvSizeX ;
						TexCoord2V = ( SrcRect.bottom - 1 + 2 ) * UseManageData->TextureCacheInvSizeY ;

						Param.GraphHandle = UseManageData->TextureCache ;
						Param.TransFlag   = TRUE ;

						// エッジがある場合はそれを先に描画する
						if( ( UseManageData->FontType & DX_FONTTYPE_EDGE ) != 0 )
						{
							Vertex[ 0 ].u = TexCoord1U + 0.5f ;	Vertex[ 0 ].v = TexCoord1V ;
							Vertex[ 1 ].u = TexCoord2U + 0.5f ;	Vertex[ 1 ].v = TexCoord1V ;
							Vertex[ 2 ].u = TexCoord1U + 0.5f ;	Vertex[ 2 ].v = TexCoord2V ;
							Vertex[ 3 ].u = TexCoord2U + 0.5f ;	Vertex[ 3 ].v = TexCoord2V ;
							Graphics_DrawSetting_SetDrawBrightToOneParam( FEdgeColor ) ;
							if( OnlyType == 0 || OnlyType == 2 )
							{
								Graphics_Draw_DrawSimpleQuadrangleGraphF( &Param ) ;
							}
							Graphics_DrawSetting_SetDrawBrightToOneParam( FColor ) ;

							// 本体用のU値をセット
							Vertex[ 0 ].u = TexCoord1U ;
							Vertex[ 1 ].u = TexCoord2U ;
							Vertex[ 2 ].u = TexCoord1U ;
							Vertex[ 3 ].u = TexCoord2U ;
						}
						else
						{
							// 本体用のUV値をセット
							Vertex[ 0 ].u = TexCoord1U ;	Vertex[ 0 ].v = TexCoord1V ;
							Vertex[ 1 ].u = TexCoord2U ;	Vertex[ 1 ].v = TexCoord1V ;
							Vertex[ 2 ].u = TexCoord1U ;	Vertex[ 2 ].v = TexCoord2V ;
							Vertex[ 3 ].u = TexCoord2U ;	Vertex[ 3 ].v = TexCoord2V ;
						}

						// 本体を描画
						if( OnlyType == 0 || OnlyType == 1 )
						{
							Graphics_Draw_DrawSimpleQuadrangleGraphF( &Param ) ;
						}
					}
				}
				else
				// 縦書きかどうかで処理を分岐
				if( VerticalFlag == TRUE )
				{
					// 縦書き

					// 画像置き換え文字かどうかで処理を分岐
					if( CharData->GraphHandleFlag )
					{
						// 拡大描画かどうかで処理を分岐
						if( ExRateValidFlag == FALSE )
						{
							// 等倍描画

							if( PosIntFlag )
							{
								int DrawX, DrawY, x1, y1, x2, y2 ;

								// 描画座標の算出
								DrawX = tmp_xi - _DTOL( DrawPosSub ) + UseManageData->BaseInfo.FontHeight - CharData->DrawY ;
								DrawY = tmp_yi + _DTOL( DrawPos )                                         + CharData->DrawX ;

								x1 = DrawX ;
								y1 = DrawY ;
								x2 = DrawX - CharData->SizeY ;
								y2 = DrawY + CharData->SizeX ;

								Graphics_DrawSetting_SetDrawBrightToOneParam( FColor ) ;
								if( OnlyType == 0 || OnlyType == 1 )
								{
									NS_DrawModiGraphF(
										x1 + AdjustPos, y1 + AdjustPos,
										x1 + AdjustPos, y2 + AdjustPos,
										x2 + AdjustPos, y2 + AdjustPos,
										x2 + AdjustPos, y1 + AdjustPos,
										CharData->GraphIndex,
										TRUE/*anti ? FALSE : TRUE*/
									) ;
								}
							}
							else
							{
								float DrawX, DrawY, x1, y1, x2, y2 ;

								// 描画座標の算出
								DrawX = tmp_xf - ( float )DrawPosSub + UseManageData->BaseInfo.FontHeight - CharData->DrawY ;
								DrawY = tmp_yf + ( float )DrawPos                                         + CharData->DrawX ;

								x1 = DrawX ;
								y1 = DrawY ;
								x2 = DrawX - CharData->SizeY ;
								y2 = DrawY + CharData->SizeX ;

								Graphics_DrawSetting_SetDrawBrightToOneParam( FColor ) ;
								if( OnlyType == 0 || OnlyType == 1 )
								{
									NS_DrawModiGraphF(
										x1 + AdjustPos, y1 + AdjustPos,
										x1 + AdjustPos, y2 + AdjustPos,
										x2 + AdjustPos, y2 + AdjustPos,
										x2 + AdjustPos, y1 + AdjustPos,
										CharData->GraphIndex,
										TRUE
									) ;
								}
							}
						}
						else
						{
							float DrawX, DrawY, x1, y1, x2, y2 ;

							// 拡大描画

							if( PosIntFlag )
							{
								// 描画元座標の算出
								DrawX = ( float )( tmp_xi - DrawPosSub + ( UseManageData->BaseInfo.FontHeight - CharData->DrawY ) * ExRateX ) ;
								DrawY = ( float )( tmp_yi + DrawPos    +                                        CharData->DrawX   * ExRateY ) ;

								x1 = DrawX ;
								y1 = DrawY ;
								x2 = DrawX + -CharData->SizeY * ( float )ExRateX ;
								y2 = DrawY +  CharData->SizeX * ( float )ExRateY ;

								Graphics_DrawSetting_SetDrawBrightToOneParam( FColor ) ;
								if( OnlyType == 0 || OnlyType == 1 )
								{
									NS_DrawModiGraphF(
										x1 + AdjustPos, y1 + AdjustPos,
										x1 + AdjustPos, y2 + AdjustPos,
										x2 + AdjustPos, y2 + AdjustPos,
										x2 + AdjustPos, y1 + AdjustPos,
										CharData->GraphIndex,
										TRUE/*anti ? FALSE : TRUE*/
									) ;
								}
							}
							else
							{
								// 描画元座標の算出
								DrawX = ( float )( tmp_xf - DrawPosSub + ( UseManageData->BaseInfo.FontHeight - CharData->DrawY ) * ExRateX ) ;
								DrawY = ( float )( tmp_yf + DrawPos    +                                        CharData->DrawX   * ExRateY ) ;

								x1 = DrawX ;
								y1 = DrawY ;
								x2 = DrawX + -CharData->SizeY * ( float )ExRateX ;
								y2 = DrawY +  CharData->SizeX * ( float )ExRateY ;

								Graphics_DrawSetting_SetDrawBrightToOneParam( FColor ) ;
								if( OnlyType == 0 || OnlyType == 1 )
								{
									NS_DrawModiGraphF(
										x1 + AdjustPos, y1 + AdjustPos,
										x1 + AdjustPos, y2 + AdjustPos,
										x2 + AdjustPos, y2 + AdjustPos,
										x2 + AdjustPos, y1 + AdjustPos,
										CharData->GraphIndex,
										TRUE/*anti ? FALSE : TRUE*/
									) ;
								}
							}
						}
					}
					else
					{
						RECT MotoDrawRect, TempRect ;
						int GraphSizeX, GraphSizeY ;
						int TempGraph ;

						// 元の描画可能矩形を保存しておく
						MotoDrawRect = GSYS.DrawSetting.DrawArea ;

						// キャッシュテクスチャのサイズを取得する
						NS_GetGraphSize( UseManageData->TextureCache, &GraphSizeX, &GraphSizeY ) ;

						// 拡大描画かどうかで処理を分岐
						if( ExRateValidFlag == FALSE )
						{
							// 等倍描画

							if( PosIntFlag )
							{
								int DrawX, DrawY, x1, y1, x2, y2 ;

								// 描画元座標の算出
								DrawX = tmp_xi - _DTOL( DrawPosSub ) + UseManageData->BaseInfo.FontHeight - CharData->DrawY ;
								DrawY = tmp_yi + _DTOL( DrawPos )                                         + CharData->DrawX ;

								x1 = DrawX + SrcY ;
								y1 = DrawY - SrcX ;
								x2 = DrawX + SrcY - GraphSizeY ;
								y2 = DrawY - SrcX + GraphSizeX ;

								// 指定矩形だけしか描画出来ないようにする
								TempRect.left   = ( DrawX - CharData->SizeY ) - 1 ;
								TempRect.top    = DrawY ;
								TempRect.right  = DrawX + 1 ;
								TempRect.bottom = DrawY + CharData->SizeX ;
								RectClipping_Inline( &TempRect, &MotoDrawRect ) ;
								NS_SetDrawArea( TempRect.left, TempRect.top, TempRect.right, TempRect.bottom ) ;

								// エッジがある場合はそれを先に描画する
								if( ( UseManageData->FontType & DX_FONTTYPE_EDGE ) != 0 )
								{
									Graphics_DrawSetting_SetDrawBrightToOneParam( FEdgeColor ) ;
									if( OnlyType == 0 || OnlyType == 2 )
									{
										NS_DrawModiGraphF(
											x1 + AdjustPos, y1                       + AdjustPos,
											x1 + AdjustPos, y2 - ( GraphSizeX >> 1 ) + AdjustPos,
											x2 + AdjustPos, y2 - ( GraphSizeX >> 1 ) + AdjustPos,
											x2 + AdjustPos, y1                       + AdjustPos,
											UseManageData->TextureCacheSub, TRUE
										) ;
									}
									Graphics_DrawSetting_SetDrawBrightToOneParam( FColor ) ;
								}

								// 本体を描画
								if( OnlyType == 0 || OnlyType == 1 )
								{
									NS_DrawModiGraphF(
										x1 + AdjustPos, y1 + AdjustPos,
										x1 + AdjustPos, y2 + AdjustPos,
										x2 + AdjustPos, y2 + AdjustPos,
										x2 + AdjustPos, y1 + AdjustPos,
										 UseManageData->TextureCache,
										 TRUE/*anti ? FALSE : TRUE*/
									) ;
								}

								// 描画可能矩形を元に戻す
								NS_SetDrawArea( MotoDrawRect.left,  MotoDrawRect.top,
												MotoDrawRect.right, MotoDrawRect.bottom ) ;
							}
							else
							{
								float DrawX, DrawY, x1, y1, x2, y2 ;

								// 描画元座標の算出
								DrawX = tmp_xf - ( float )DrawPosSub + UseManageData->BaseInfo.FontHeight - ( CharData->DrawY - 1.0f ) ;
								DrawY = tmp_yf + ( float )DrawPos    +                                      ( CharData->DrawX - 1.0f ) ;

								x1 = DrawX ;
								y1 = DrawY ;
								x2 = DrawX - ( CharData->SizeY + 2.0f ) ;
								y2 = DrawY + ( CharData->SizeX + 2.0f ) ;

								// エッジがある場合はそれを先に描画する
								if( ( UseManageData->FontType & DX_FONTTYPE_EDGE ) != 0 )
								{
									TempGraph = NS_DerivationGraph( SrcX - 1, SrcY - 1, CharData->SizeX + 2, CharData->SizeY + 2, UseManageData->TextureCacheSub ) ;
									Graphics_DrawSetting_SetDrawBrightToOneParam( FEdgeColor ) ;
									if( OnlyType == 0 || OnlyType == 2 )
									{
										NS_DrawModiGraphF(
											x1 + AdjustPos, y1 + AdjustPos,
											x1 + AdjustPos, y2 + AdjustPos,
											x2 + AdjustPos, y2 + AdjustPos,
											x2 + AdjustPos, y1 + AdjustPos,
											TempGraph, TRUE
										) ;
									}
									NS_DeleteGraph( TempGraph ) ;
									Graphics_DrawSetting_SetDrawBrightToOneParam( FColor ) ;
								}

								// 本体を描画
								TempGraph = NS_DerivationGraph( SrcX - 1, SrcY - 1, CharData->SizeX + 2, CharData->SizeY + 2, UseManageData->TextureCache ) ;
								if( OnlyType == 0 || OnlyType == 1 )
								{
									NS_DrawModiGraphF(
										x1 + AdjustPos, y1 + AdjustPos,
										x1 + AdjustPos, y2 + AdjustPos,
										x2 + AdjustPos, y2 + AdjustPos,
										x2 + AdjustPos, y1 + AdjustPos,
										TempGraph,
										TRUE
									) ;
								}
								NS_DeleteGraph( TempGraph ) ;
							}
						}
						else
						{
							float DrawX, DrawY, x1, y1, x2, y2 ;

							// 拡大描画

							if( PosIntFlag )
							{
								// 描画元座標の算出
								DrawX = ( float )( tmp_xi - DrawPosSub + ( UseManageData->BaseInfo.FontHeight - ( CharData->DrawY - 1 ) ) * ExRateX ) ;
								DrawY = ( float )( tmp_yi + DrawPos    +                                        ( CharData->DrawX - 1 )   * ExRateY ) ;

								x1 = DrawX ;
								y1 = DrawY ;
								x2 = ( float )( DrawX + -( CharData->SizeY + 2 ) * ExRateX ) ;
								y2 = ( float )( DrawY +  ( CharData->SizeX + 2 ) * ExRateY ) ;

								// 指定矩形だけしか描画出来ないようにする
								TempRect.left   = _FTOL( ( float )( DrawX - _DTOL( ( CharData->SizeY + 2 ) * ExRateX ) ) ) - 1 ;
								TempRect.top    = _FTOL( DrawY ) - 1 ;
								TempRect.right  = _FTOL( DrawX ) + 1 ;
								TempRect.bottom = _FTOL( ( float )( DrawY + ( CharData->SizeX + 2 ) * ExRateY ) ) + 1 ;
								RectClipping_Inline( &TempRect, &MotoDrawRect ) ;
								NS_SetDrawArea( TempRect.left, TempRect.top, TempRect.right, TempRect.bottom ) ;

								// エッジがある場合はそれを先に描画する
								if( ( UseManageData->FontType & DX_FONTTYPE_EDGE ) != 0 )
								{
									TempGraph = NS_DerivationGraph( SrcX - 1, SrcY - 1, CharData->SizeX + 2, CharData->SizeY + 2, UseManageData->TextureCacheSub ) ;
									Graphics_DrawSetting_SetDrawBrightToOneParam( FEdgeColor ) ;
									if( OnlyType == 0 || OnlyType == 2 )
									{
										NS_DrawModiGraphF(
											x1 + AdjustPos, y1 + AdjustPos,
											x1 + AdjustPos, y2 + AdjustPos,
											x2 + AdjustPos, y2 + AdjustPos,
											x2 + AdjustPos, y1 + AdjustPos,
											TempGraph, TRUE
										) ;
									}
									NS_DeleteGraph( TempGraph ) ;
									Graphics_DrawSetting_SetDrawBrightToOneParam( FColor ) ;
								}

								// 本体を描画
								TempGraph = NS_DerivationGraph( SrcX - 1, SrcY - 1, CharData->SizeX + 2, CharData->SizeY + 2, UseManageData->TextureCache ) ;
								if( OnlyType == 0 || OnlyType == 1 )
								{
									NS_DrawModiGraphF(
										x1 + AdjustPos, y1 + AdjustPos,
										x1 + AdjustPos, y2 + AdjustPos,
										x2 + AdjustPos, y2 + AdjustPos,
										x2 + AdjustPos, y1 + AdjustPos,
										TempGraph,
										TRUE/*anti ? FALSE : TRUE*/
									) ;
								}
								NS_DeleteGraph( TempGraph ) ;

								// 描画可能矩形を元に戻す
								NS_SetDrawArea( MotoDrawRect.left,  MotoDrawRect.top,
												MotoDrawRect.right, MotoDrawRect.bottom ) ;
							}
							else
							{
								// 描画元座標の算出
								DrawX = ( float )( tmp_xf + DrawPosSub + ( UseManageData->BaseInfo.FontHeight - ( CharData->DrawY - 1.0f ) ) * ExRateX ) ;
								DrawY = ( float )( tmp_yf + DrawPos    +                                        ( CharData->DrawX - 1.0f )   * ExRateY ) ;

								x1 = DrawX ;
								y1 = DrawY ;
								x2 = DrawX - ( CharData->SizeY + 2.0f ) * ( float )ExRateX ;
								y2 = DrawY + ( CharData->SizeX + 2.0f ) * ( float )ExRateY ;

								// エッジがある場合はそれを先に描画する
								if( ( UseManageData->FontType & DX_FONTTYPE_EDGE ) != 0 )
								{
									TempGraph = NS_DerivationGraph( SrcX - 1, SrcY - 1, CharData->SizeX + 2, CharData->SizeY + 2, UseManageData->TextureCacheSub ) ;
									Graphics_DrawSetting_SetDrawBrightToOneParam( FEdgeColor ) ;
									if( OnlyType == 0 || OnlyType == 2 )
									{
										NS_DrawModiGraphF(
											x1 + AdjustPos, y1 + AdjustPos,
											x1 + AdjustPos, y2 + AdjustPos,
											x2 + AdjustPos, y2 + AdjustPos,
											x2 + AdjustPos, y1 + AdjustPos,
											TempGraph, TRUE
										) ;
									}
									NS_DeleteGraph( TempGraph ) ;
									Graphics_DrawSetting_SetDrawBrightToOneParam( FColor ) ;
								}

								// 本体を描画
								TempGraph = NS_DerivationGraph( SrcX - 1, SrcY - 1, CharData->SizeX + 2, CharData->SizeY + 2, UseManageData->TextureCache ) ;
								if( OnlyType == 0 || OnlyType == 1 )
								{
									NS_DrawModiGraphF(
										x1 + AdjustPos, y1 + AdjustPos,
										x1 + AdjustPos, y2 + AdjustPos,
										x2 + AdjustPos, y2 + AdjustPos,
										x2 + AdjustPos, y1 + AdjustPos,
										TempGraph,
										TRUE/*anti ? FALSE : TRUE*/
									) ;
								}
								NS_DeleteGraph( TempGraph ) ;
							}
						}
					}
				}
				else
				{
					// 横書き

					// 拡大描画かどうかで処理を分岐
					if( ExRateValidFlag == FALSE )
					{
						// 等倍描画

						// 画像置き換え文字かどうかで処理を分岐
						if( CharData->GraphHandleFlag )
						{
							Graphics_DrawSetting_SetDrawBrightToOneParam( FColor ) ;
							if( OnlyType == 0 || OnlyType == 1 )
							{
								if( PosIntFlag )
								{
									NS_DrawGraph(
										tmp_xi + _DTOL( DrawPos    ) + CharData->DrawX,
										tmp_yi + _DTOL( DrawPosSub ) + CharData->DrawY,
										CharData->GraphIndex,
										TRUE
									) ;
								}
								else
								{
									NS_DrawGraphF(
										tmp_xf + ( float )DrawPos    + CharData->DrawX,
										tmp_yf + ( float )DrawPosSub + CharData->DrawY,
										CharData->GraphIndex,
										TRUE
									) ;
								}
							}
						}
						else
						{
							// エッジがある場合はそれを先に描画する
							if( ( UseManageData->FontType & DX_FONTTYPE_EDGE ) != 0 )
							{
								Graphics_DrawSetting_SetDrawBrightToOneParam( FEdgeColor ) ;
								if( OnlyType == 0 || OnlyType == 2 )
								{
									if( PosIntFlag )
									{
										NS_DrawRectGraph(
											tmp_xi + _DTOL( DrawPos    ) + CharData->DrawX,
											tmp_yi + _DTOL( DrawPosSub ) + CharData->DrawY,
											SrcX,
											SrcY,
											CharData->SizeX,
											CharData->SizeY,
											UseManageData->TextureCacheSub,
											TRUE,
											FALSE
										) ;
									}
									else
									{
										NS_DrawRectGraphF(
											tmp_xf + ( float )DrawPos    + CharData->DrawX - 1.0f,
											tmp_yf + ( float )DrawPosSub + CharData->DrawY - 1.0f,
											SrcX - 1,
											SrcY - 1,
											CharData->SizeX + 2,
											CharData->SizeY + 2,
											UseManageData->TextureCacheSub,
											TRUE,
											FALSE
										) ;
									}
								}
								Graphics_DrawSetting_SetDrawBrightToOneParam( FColor ) ;
							}

							// 本体を描画
							if( OnlyType == 0 || OnlyType == 1 )
							{
								if( PosIntFlag )
								{
									NS_DrawRectGraph(
										tmp_xi + _DTOL( DrawPos    ) + CharData->DrawX,
										tmp_yi + _DTOL( DrawPosSub ) + CharData->DrawY,
										SrcX,
										SrcY,
										CharData->SizeX,
										CharData->SizeY,
										UseManageData->TextureCache,
										TRUE,
										FALSE
									) ;
								}
								else
								{
									NS_DrawRectGraphF(
										tmp_xf + ( float )DrawPos    + CharData->DrawX - 1.0f,
										tmp_yf + ( float )DrawPosSub + CharData->DrawY - 1.0f,
										SrcX - 1,
										SrcY - 1,
										CharData->SizeX + 2,
										CharData->SizeY + 2,
										UseManageData->TextureCache,
										TRUE,
										FALSE
									) ;
								}
							}
						}
					}
					else
					{
						float x1, y1, x2, y2 ;

						// 拡大描画

						// 画像置き換え文字かどうかで処理を分岐
						if( CharData->GraphHandleFlag )
						{
							if( PosIntFlag )
							{
								x1 = ( float )( tmp_xi + DrawPos    + CharData->DrawX * ExRateX ) ;
								y1 = ( float )( tmp_yi + DrawPosSub + CharData->DrawY * ExRateY ) ;
								x2 = ( float )( x1 +                  CharData->SizeX * ExRateX ) ;
								y2 = ( float )( y1 +                  CharData->SizeY * ExRateY ) ;
							}
							else
							{
								x1 = ( float )( tmp_xf + DrawPos    + CharData->DrawX * ExRateX ) ;
								y1 = ( float )( tmp_yf + DrawPosSub + CharData->DrawY * ExRateY ) ;
								x2 = ( float )( x1 +                  CharData->SizeX * ExRateX ) ;
								y2 = ( float )( y1 +                  CharData->SizeY * ExRateY ) ;
							}

							Graphics_DrawSetting_SetDrawBrightToOneParam( FColor ) ;
							if( OnlyType == 0 || OnlyType == 1 )
							{
								NS_DrawExtendGraphF(
									x1,							y1,
									x2,							y2,
									CharData->GraphIndex,
									TRUE
								) ;
							}
						}
						else
						{
							if( PosIntFlag )
							{
								x1 = ( float )( tmp_xi + DrawPos    + ( CharData->DrawX - 1 ) * ExRateX ) ;
								y1 = ( float )( tmp_yi + DrawPosSub + ( CharData->DrawY - 1 ) * ExRateY ) ;
								x2 = ( float )( x1 +                  ( CharData->SizeX + 2 ) * ExRateX ) ;
								y2 = ( float )( y1 +                  ( CharData->SizeY + 2 ) * ExRateY ) ;
							}
							else
							{
								x1 = ( float )( tmp_xf + DrawPos    + ( CharData->DrawX - 1 ) * ExRateX ) ;
								y1 = ( float )( tmp_yf + DrawPosSub + ( CharData->DrawY - 1 ) * ExRateY ) ;
								x2 = ( float )( x1 +                  ( CharData->SizeX + 2 ) * ExRateX ) ;
								y2 = ( float )( y1 +                  ( CharData->SizeY + 2 ) * ExRateY ) ;
							}

							// エッジがある場合はそれを先に描画する
							if( ( UseManageData->FontType & DX_FONTTYPE_EDGE ) != 0 )
							{
								Graphics_DrawSetting_SetDrawBrightToOneParam( FEdgeColor ) ;
								if( OnlyType == 0 || OnlyType == 2 )
								{
									if( PosIntFlag )
									{
										NS_DrawRectExtendGraphF(
											x1,								y1,
											x2,								y2,
											SrcX - 1,						SrcY - 1,
											CharData->SizeX + 2,			CharData->SizeY + 2,
											UseManageData->TextureCacheSub,
											TRUE ) ;
									}
									else
									{
										NS_DrawRectExtendGraphF(
											x1,								y1,
											x2,								y2,
											SrcX - 1,						SrcY - 1,
											CharData->SizeX + 2,			CharData->SizeY + 2,
											UseManageData->TextureCacheSub,
											TRUE ) ;
									}
									Graphics_DrawSetting_SetDrawBrightToOneParam( FColor ) ;
								}
							}

							// 本体を描画
							if( OnlyType == 0 || OnlyType == 1 )
							{
								if( PosIntFlag )
								{
									NS_DrawRectExtendGraphF(
										x1,							y1,
										x2,							y2,
										SrcX - 1,					SrcY - 1,
										CharData->SizeX + 2,		CharData->SizeY + 2,
										UseManageData->TextureCache,
										TRUE
									) ;
								}
								else
								{
									NS_DrawRectExtendGraphF(
										x1,							y1,
										x2,							y2,
										SrcX - 1,					SrcY - 1,
										CharData->SizeX + 2,		CharData->SizeY + 2,
										UseManageData->TextureCache,
										TRUE
									) ;
								}
							}
						}
					}
				}

				// ブレンド画像の情報を元に戻す
				if( BlendGraph != -1 && GSYS.DrawSetting.BlendGraphPosMode == DX_BLENDGRAPH_POSMODE_DRAWGRAPH )
				{
					NS_SetBlendGraph( BlendGraph, BorderParam, BorderRange ) ;
				}
			}
			else
			if( DestMemImg != NULL )
			{
				// 縦書きかどうかで処理を分岐
				if( VerticalFlag == TRUE )
				{
					DWORD SizeX, SizeY ;
					RECT TempRect ;

					// 描画矩形のセット
					DstRect.left   = tmp_xi - _DTOL( DrawPosSub ) + UseManageData->BaseInfo.FontHeight - CharData->DrawY - CharData->SizeY ;
					DstRect.right  = tmp_xi - _DTOL( DrawPosSub ) + UseManageData->BaseInfo.FontHeight - CharData->DrawY ;
					DstRect.top    = tmp_yi + _DTOL( DrawPos )                                         + CharData->DrawX ;
					DstRect.bottom = tmp_yi + _DTOL( DrawPos )                                         + CharData->DrawX + CharData->SizeX ;
					TempRect = DstRect ;
					RectClipping_Inline( &DstRect, ClipRect ) ;

					// 描画する文字が無かった場合ここで次ループに移る
					if( DstRect.right == DstRect.left || DstRect.bottom == DstRect.top )
					{
						goto LOOPEND ;
					}

					// 転送元矩形と描画サイズの調整
					SrcRect.left   += DstRect.top     - TempRect.top ;
					SrcRect.top    += DstRect.right   - TempRect.right ;
					SrcRect.right  -= TempRect.bottom - DstRect.bottom ;
					SrcRect.bottom -= TempRect.left   - DstRect.left ;
					SizeX = ( DWORD )( SrcRect.right  - SrcRect.left ) ;
					SizeY = ( DWORD )( SrcRect.bottom - SrcRect.top  ) ;

					// 転送
					{
						int SrcPitch2, DstPitch2 ;
						WORD SizeXWord = ( WORD )SizeX, SizeYWord = ( WORD )SizeY ;

						// カラービットモードによって処理を分岐

						switch( NS_GetColorBitDepth() )
						{
						case 8 :
							// 8ビットモードの時の処理
							{
								BYTE *Dest ;
								BYTE *Src ;
								BYTE ColorData = ( BYTE )( Color & 0xff ) ;

								Dest = DestMemImg->UseImage + DstRect.right + DstRect.top * DestMemImg->Base->Pitch ;
								Src = UseManageData->CacheMem + SrcRect.left + SrcRect.top * UseManageData->CachePitch ;

								SrcPitch2 = ( int )( UseManageData->CachePitch - SizeX ) ;
								DstPitch2 = ( int )( -1 - ( int )SizeX * ( int )DestMemImg->Base->Pitch ) ;

								switch( UseManageData->FontType )
								{
								case DX_FONTTYPE_NORMAL :
									{
										WORD SizeXTemp ;
										WORD Data ;
										int StX ;
										const WORD Table[16] = { 0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1, 0x8000, 0x4000, 0x2000, 0x1000, 0x800, 0x400, 0x200, 0x100 } ;

										StX = SrcRect.left % 8 ;
										Src = UseManageData->CacheMem + ( SrcRect.left / 8 ) + SrcRect.top * UseManageData->CachePitch ;
										SrcPitch2 = ( int )( UseManageData->CachePitch - ( ( SizeX + 7 ) / 8 ) ) ;

										if( TransFlag )
										{
											do{
												SizeXTemp = SizeXWord ;
												do{
													Data = *( ( WORD * )Src ) ;
													for(;;)
													{
														if( Data & Table[StX] )   *Dest = ColorData ;	Dest += DstPitch ;	if( --SizeXTemp == 0 ) break ;
														if( Data & Table[StX+1] ) *Dest = ColorData ;	Dest += DstPitch ;	if( --SizeXTemp == 0 ) break ;
														if( Data & Table[StX+2] ) *Dest = ColorData ;	Dest += DstPitch ;	if( --SizeXTemp == 0 ) break ;
														if( Data & Table[StX+3] ) *Dest = ColorData ;	Dest += DstPitch ;	if( --SizeXTemp == 0 ) break ;
														if( Data & Table[StX+4] ) *Dest = ColorData ;	Dest += DstPitch ;	if( --SizeXTemp == 0 ) break ;
														if( Data & Table[StX+5] ) *Dest = ColorData ;	Dest += DstPitch ;	if( --SizeXTemp == 0 ) break ;
														if( Data & Table[StX+6] ) *Dest = ColorData ;	Dest += DstPitch ;	if( --SizeXTemp == 0 ) break ;
														if( Data & Table[StX+7] ) *Dest = ColorData ;	Dest += DstPitch ;	--SizeXTemp ;
														break;
													}

													Src += 1 ;
												}while( SizeXTemp );

												Src += SrcPitch2 ;
												Dest += DstPitch2 ;
											}while( --SizeYWord ) ;
										}
										else
										{
											do{
												SizeXTemp = SizeXWord ;
												do{
													Data = *( ( WORD * )Src ) ;
													for(;;)
													{
														*Dest = ( BYTE )( ( Data & Table[StX]   ) ? ColorData : 0 ) ;  	Dest += DstPitch ; 	if( --SizeXTemp == 0 ) break ;
														*Dest = ( BYTE )( ( Data & Table[StX+1] ) ? ColorData : 0 ) ;	Dest += DstPitch ;  if( --SizeXTemp == 0 ) break ;
														*Dest = ( BYTE )( ( Data & Table[StX+2] ) ? ColorData : 0 ) ;	Dest += DstPitch ;  if( --SizeXTemp == 0 ) break ;
														*Dest = ( BYTE )( ( Data & Table[StX+3] ) ? ColorData : 0 ) ;	Dest += DstPitch ;  if( --SizeXTemp == 0 ) break ;
														*Dest = ( BYTE )( ( Data & Table[StX+4] ) ? ColorData : 0 ) ;	Dest += DstPitch ;  if( --SizeXTemp == 0 ) break ;
														*Dest = ( BYTE )( ( Data & Table[StX+5] ) ? ColorData : 0 ) ;	Dest += DstPitch ;  if( --SizeXTemp == 0 ) break ;
														*Dest = ( BYTE )( ( Data & Table[StX+6] ) ? ColorData : 0 ) ;	Dest += DstPitch ;  if( --SizeXTemp == 0 ) break ;
														*Dest = ( BYTE )( ( Data & Table[StX+7] ) ? ColorData : 0 ) ;	Dest += DstPitch ;	--SizeXTemp ;
														break ;
													}

													Src += 1 ;
												}while( SizeXTemp );

												Src += SrcPitch2 ;
												Dest += DstPitch2 ;
											}while( --SizeYWord ) ;
										}
									}
									break ;


								case DX_FONTTYPE_EDGE :
									{
										WORD Width ;
										BYTE EColor ;

										EColor = ( BYTE )( EdgeColor & 0xff ) ;
										if( TransFlag )
										{
											do
											{
												Width = SizeXWord ;
												do
												{
													if( *Src ) *Dest = *Src == 1 ? ColorData : EColor ;
													Dest += DstPitch ;
													Src ++ ;
												}while( --Width ) ;

												Dest += DstPitch2 ;
												Src += SrcPitch2 ;
											}while( --SizeYWord ) ;
										}
										else
										{
											do
											{
												Width = SizeXWord ;
												do
												{
													*Dest = ( BYTE )( *Src ? ( *Src == 1 ? ColorData : EColor ) : 0 ) ;
													Dest += DstPitch ;
													Src ++ ;
												}while( --Width ) ;

												Dest += DstPitch2 ;
												Src += SrcPitch2 ;
											}while( --SizeYWord ) ;
										}
									}
									break ;
								}
							}
							break ;

						case 16 :
							// 16ビットモードの時の処理
							{
								BYTE *Dest ;
								BYTE *Src ;
								WORD ColorData = ( WORD )Color ;

								Dest = DestMemImg->UseImage + ( DstRect.right * 2 ) + DstRect.top * DstPitch ;
								Src = UseManageData->CacheMem + SrcRect.left + SrcRect.top * SrcPitch ;

								SrcPitch2 = ( int )( SrcPitch - SizeX ) ;
								DstPitch2 = ( int )( -1 * 2 - ( int )SizeX * ( int )DstPitch ) ;

								switch( UseManageData->FontType )
								{
								case DX_FONTTYPE_ANTIALIASING_EDGE :
								case DX_FONTTYPE_ANTIALIASING_EDGE_4X4 :
								case DX_FONTTYPE_ANTIALIASING_EDGE_8X8 :
									{
										BYTE Red1, Green1, Blue1 ;
										BYTE Red2, Green2, Blue2 ;
										DWORD AAA ;

										WORD Width ;

										Red1   = ( BYTE )( ( ColorData & MEMIMG_R5G6B5_R ) >> MEMIMG_R5G6B5_LR ) ;
										Green1 = ( BYTE )( ( ColorData & MEMIMG_R5G6B5_G ) >> MEMIMG_R5G6B5_LG ) ;
										Blue1  = ( BYTE )( ( ColorData & MEMIMG_R5G6B5_B ) >> MEMIMG_R5G6B5_LB ) ;

										Red2   = ( BYTE )( ( EdgeColor & MEMIMG_R5G6B5_R ) >> MEMIMG_R5G6B5_LR ) ;
										Green2 = ( BYTE )( ( EdgeColor & MEMIMG_R5G6B5_G ) >> MEMIMG_R5G6B5_LG ) ;
										Blue2  = ( BYTE )( ( EdgeColor & MEMIMG_R5G6B5_B ) >> MEMIMG_R5G6B5_LB ) ;

										Dest = DestMemImg->UseImage + ( DstRect.right * 4 ) + DstRect.top * DstPitch ;
										DstPitch2 = ( int )( -1 * 4 - ( int )SizeX * ( int )DstPitch ) ;

										if( TransFlag )
										{
											do
											{
												Width = SizeXWord ;
												do
												{
													if( *Src & 0x0f )
													{
														if( ( *Src & 0x0f ) == 0x0f )
														{
															*( ( DWORD * )Dest ) = ( DWORD )( 0xff0000 | ColorData ) ;
														}
														else
														{
															Dest[2] = ( BYTE )( ( ( *Src & 0x0f ) << 4 ) + ( ( 0x0f - ( *Src & 0x0f ) ) * ( ( *Src & 0xf0 ) >> 4 ) ) ) ;
															AAA = ( DWORD )( ( ( WORD )( *Src & 0x0f ) << 12 ) / Dest[2] ) ;
															*( ( WORD * )Dest ) = ( WORD )( 
																( ( ( ( ( Red1   - Red2   ) * AAA ) >> 8 ) + Red2   ) << MEMIMG_R5G6B5_LR ) | 
																( ( ( ( ( Green1 - Green2 ) * AAA ) >> 8 ) + Green2 ) << MEMIMG_R5G6B5_LG ) | 
																( ( ( ( ( Blue1  - Blue2  ) * AAA ) >> 8 ) + Blue2  ) << MEMIMG_R5G6B5_LB ) ) ; 
														}
													}
													else
													{
														if( *Src & 0xf0 )
														{
															*( ( DWORD * )Dest ) = EdgeColor | ( ( *Src & 0xf0 ) << 16 ) ;
														}
													}

													Dest += DstPitch ;
													Src  += 1 ;
												}while( --Width ) ;

												Dest += DstPitch2 ;
												Src += SrcPitch2 ;
											}while( --SizeYWord ) ;
										}
										else
										{
											do
											{
												Width = SizeXWord ;
												do
												{
													if( *Src & 0x0f )
													{
														if( ( *Src & 0x0f ) == 0x0f )
														{
															*( ( DWORD * )Dest ) = ( DWORD )( 0xff0000 | ColorData ) ;
														}
														else
														{
															Dest[2] = ( BYTE )( ( ( *Src & 0x0f ) << 4 ) + ( ( 0x0f - ( *Src & 0x0f ) ) * ( ( *Src & 0xf0 ) >> 4 ) ) ) ;
															AAA = ( DWORD )( ( ( WORD )( *Src & 0x0f ) << 12 ) / Dest[2] ) ;
															*( ( WORD * )Dest ) = ( WORD )( 
																( ( ( ( ( Red1   - Red2   ) * AAA ) >> 8 ) + Red2   ) << MEMIMG_R5G6B5_LR ) | 
																( ( ( ( ( Green1 - Green2 ) * AAA ) >> 8 ) + Green2 ) << MEMIMG_R5G6B5_LG ) | 
																( ( ( ( ( Blue1  - Blue2  ) * AAA ) >> 8 ) + Blue2  ) << MEMIMG_R5G6B5_LB ) ) ; 
														}
													}
													else
													{
														if( *Src & 0xf0 )
														{
															*( ( DWORD * )Dest ) = EdgeColor | ( ( *Src & 0xf0 ) << 16 ) ;
														}
														else
														{
															*( ( DWORD * )Dest ) = 0 ;
														}
													}

													Dest += DstPitch ;
													Src ++ ;
												}while( --Width ) ;

												Dest += DstPitch2 ;
												Src += SrcPitch2 ;
											}while( --SizeYWord ) ;
										}
									}
									break ;

								case DX_FONTTYPE_ANTIALIASING :
								case DX_FONTTYPE_ANTIALIASING_4X4 :
								case DX_FONTTYPE_ANTIALIASING_8X8 :
									{
										WORD Width ;

										Dest      = DestMemImg->UseImage + ( DstRect.right * 4 ) + DstRect.top * DstPitch ;
										DstPitch2 = ( int )( -1 * 4 - ( int )SizeX * ( int )DstPitch ) ;

										if( TransFlag )
										{
											do
											{
												Width = SizeXWord ;
												do
												{
													if( *Src )
													{
														*( ( DWORD * )Dest ) = ( DWORD )( ColorData | ( *Src << 20 ) ) ;
													}
													Dest += DstPitch ;
													Src ++ ;
												}while( --Width ) ;

												Dest += DstPitch2 ;
												Src += SrcPitch2 ;
											}while( --SizeYWord ) ;
										}
										else
										{
											do
											{
												Width = SizeXWord ;
												do
												{
													if( *Src )
													{
														*( ( DWORD * )Dest ) = ( DWORD )( ColorData | ( *Src << 20 ) ) ;
													}
													else
													{
														*( ( DWORD * )Dest ) = 0 ;
													}

													Dest += DstPitch ;
													Src ++ ;
												}while( --Width ) ;

												Dest += DstPitch2 ;
												Src += SrcPitch2 ;
											}while( --SizeYWord ) ;
										}
									}
									break ;

								case DX_FONTTYPE_NORMAL :
									{
										WORD SizeXTemp ;
										WORD Data ;
										int StX = SrcRect.left % 8 ;
										WORD Table[16] = { 0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1, 0x8000, 0x4000, 0x2000, 0x1000, 0x800, 0x400, 0x200, 0x100 } ;

										Src       = UseManageData->CacheMem + ( SrcRect.left / 8 ) + SrcRect.top * SrcPitch ;
										SrcPitch2 = ( int )( SrcPitch - ( ( SizeX + 7 ) / 8 ) ) ;

										if( TransFlag )
										{
											do{
												SizeXTemp = SizeXWord ;
												do{
													Data = *( ( WORD * )Src ) ;
													for(;;)
													{
														if( Data & Table[StX] )   *( (WORD *)Dest ) = ColorData ;	Dest += DstPitch ; 	if( --SizeXTemp == 0 ) break ;
														if( Data & Table[StX+1] ) *( (WORD *)Dest ) = ColorData ;	Dest += DstPitch ;	if( --SizeXTemp == 0 ) break ;
														if( Data & Table[StX+2] ) *( (WORD *)Dest ) = ColorData ;	Dest += DstPitch ;	if( --SizeXTemp == 0 ) break ;
														if( Data & Table[StX+3] ) *( (WORD *)Dest ) = ColorData ;	Dest += DstPitch ;	if( --SizeXTemp == 0 ) break ;
														if( Data & Table[StX+4] ) *( (WORD *)Dest ) = ColorData ;	Dest += DstPitch ;	if( --SizeXTemp == 0 ) break ;
														if( Data & Table[StX+5] ) *( (WORD *)Dest ) = ColorData ;	Dest += DstPitch ;	if( --SizeXTemp == 0 ) break ;
														if( Data & Table[StX+6] ) *( (WORD *)Dest ) = ColorData ;	Dest += DstPitch ;	if( --SizeXTemp == 0 ) break ;
														if( Data & Table[StX+7] ) *( (WORD *)Dest ) = ColorData ;	Dest += DstPitch ;	--SizeXTemp ;
														break ;
													}

													Src += 1 ;
												}while( SizeXTemp );

												Src += SrcPitch2 ;
												Dest += DstPitch2 ;
											}while( --SizeYWord ) ;
										}
										else
										{
											do{
												SizeXTemp = SizeXWord ;
												do{
													Data = *( ( WORD * )Src ) ;
													for(;;)
													{
														*( (WORD *)Dest ) = ( WORD )( ( Data & Table[StX]   ) ? ColorData : 0 ) ;  	Dest += DstPitch ; 	if( --SizeXTemp == 0 ) break ;
														*( (WORD *)Dest ) = ( WORD )( ( Data & Table[StX+1] ) ? ColorData : 0 ) ;	Dest += DstPitch ;  if( --SizeXTemp == 0 ) break ;
														*( (WORD *)Dest ) = ( WORD )( ( Data & Table[StX+2] ) ? ColorData : 0 ) ;	Dest += DstPitch ;  if( --SizeXTemp == 0 ) break ;
														*( (WORD *)Dest ) = ( WORD )( ( Data & Table[StX+3] ) ? ColorData : 0 ) ;	Dest += DstPitch ;	if( --SizeXTemp == 0 ) break ;
														*( (WORD *)Dest ) = ( WORD )( ( Data & Table[StX+4] ) ? ColorData : 0 ) ;	Dest += DstPitch ;  if( --SizeXTemp == 0 ) break ;
														*( (WORD *)Dest ) = ( WORD )( ( Data & Table[StX+5] ) ? ColorData : 0 ) ;	Dest += DstPitch ;	if( --SizeXTemp == 0 ) break ;
														*( (WORD *)Dest ) = ( WORD )( ( Data & Table[StX+6] ) ? ColorData : 0 ) ;	Dest += DstPitch ;	if( --SizeXTemp == 0 ) break ;
														*( (WORD *)Dest ) = ( WORD )( ( Data & Table[StX+7] ) ? ColorData : 0 ) ;	Dest += DstPitch ; 	--SizeXTemp ;
														break ;
													}

													Src += 1 ;
												}while( SizeXTemp );

												Src += SrcPitch2 ;
												Dest += DstPitch2 ;
											}while( --SizeYWord ) ;
										}
									}
									break ;

								case DX_FONTTYPE_EDGE :
									{
										WORD Width ;
										WORD EColor ;

										EColor = ( WORD )( EdgeColor & 0xffff ) ;
										if( TransFlag )
										{
											do
											{
												Width = SizeXWord ;
												do
												{
													if( *Src ) *( ( WORD * )Dest ) = *Src == 1 ? ColorData : EColor ;
													Dest += DstPitch ;
													Src ++ ;
												}while( --Width ) ;

												Dest += DstPitch2 ;
												Src += SrcPitch2 ;
											}while( --SizeYWord ) ;
										}
										else
										{
											do
											{
												Width = SizeXWord ;
												do
												{
													*( ( WORD * )Dest ) = ( WORD )( *Src ? ( *Src == 1 ? ColorData : EColor ) : 0 ) ;
													Dest += DstPitch ;
													Src ++ ;
												}while( --Width ) ;

												Dest += DstPitch2 ;
												Src += SrcPitch2 ;
											}while( --SizeYWord ) ;
										}
									}
									break ;
								}
							}
							break ;

						case 32 :
							// 32ビットモードの場合
							{
								BYTE *Dest ;
								BYTE *Src ;
								unsigned int ColorData = ( DWORD )Color ;
   
								Dest = DestMemImg->UseImage + ( DstRect.right * 4 ) + DstRect.top * DstPitch ;
								Src = UseManageData->CacheMem + SrcRect.left + SrcRect.top * SrcPitch ;

								SrcPitch2 = ( int )( SrcPitch - SizeX ) ;
								DstPitch2 = ( int )( -1 * 4 - ( ( int )SizeX * ( int )DstPitch ) ) ;

								switch( UseManageData->FontType )
								{
								case DX_FONTTYPE_ANTIALIASING_EDGE :
								case DX_FONTTYPE_ANTIALIASING_EDGE_4X4 :
								case DX_FONTTYPE_ANTIALIASING_EDGE_8X8 :
									{
										BYTE Red1, Green1, Blue1 ;
										BYTE Red2, Green2, Blue2 ;
										DWORD AAA ;

										WORD Width ;

										Red1   = ( BYTE )( ( ColorData & MEMIMG_XRGB8_R ) >> MEMIMG_XRGB8_LR ) ;
										Green1 = ( BYTE )( ( ColorData & MEMIMG_XRGB8_G ) >> MEMIMG_XRGB8_LG ) ;
										Blue1  = ( BYTE )( ( ColorData & MEMIMG_XRGB8_B ) >> MEMIMG_XRGB8_LB ) ;

										Red2   = ( BYTE )( ( EdgeColor & MEMIMG_XRGB8_R ) >> MEMIMG_XRGB8_LR ) ;
										Green2 = ( BYTE )( ( EdgeColor & MEMIMG_XRGB8_G ) >> MEMIMG_XRGB8_LG ) ;
										Blue2  = ( BYTE )( ( EdgeColor & MEMIMG_XRGB8_B ) >> MEMIMG_XRGB8_LB ) ;

										if( TransFlag )
										{
											do
											{
												Width = SizeXWord ;
												do
												{
													if( *Src & 0x0f )
													{
														if( ( *Src & 0x0f ) == 0x0f )
														{
															*( ( DWORD * )Dest ) = 0xff000000 | ColorData ;
														}
														else
														{
															*( ( DWORD * )Dest ) = ( DWORD )( ( ( ( *Src & 0x0f ) << 4 ) + ( ( 0x0f - ( *Src & 0x0f ) ) * ( ( *Src & 0xf0 ) >> 4 ) ) ) << 24 ) ;
															AAA = ( DWORD )( ( ( WORD )( *Src & 0x0f ) << 12 ) / Dest[3] ) ;
															*( ( DWORD * )Dest ) |= ( DWORD )( 
																( ( ( ( ( Red1   - Red2   ) * AAA ) >> 8 ) + Red2   ) << MEMIMG_XRGB8_LR ) | 
																( ( ( ( ( Green1 - Green2 ) * AAA ) >> 8 ) + Green2 ) << MEMIMG_XRGB8_LG ) | 
																( ( ( ( ( Blue1  - Blue2  ) * AAA ) >> 8 ) + Blue2  ) << MEMIMG_XRGB8_LB ) ) ; 
														}
													}
													else
													{
														if( *Src & 0xf0 )
														{
															*( ( DWORD * )Dest ) = EdgeColor | ( ( *Src & 0xf0 ) << 24 ) ;
														}
													}

													Dest += DstPitch ;
													Src  ++ ;
												}while( --Width ) ;

												Dest += DstPitch2 ;
												Src += SrcPitch2 ;
											}while( --SizeYWord ) ;
										}
										else
										{
											do
											{
												Width = SizeXWord ;
												do
												{
													if( *Src & 0x0f )
													{
														if( ( *Src & 0x0f ) == 0x0f )
														{
															*( ( DWORD * )Dest ) = 0xff000000 | ColorData ;
														}
														else
														{
															*( ( DWORD * )Dest ) = ( DWORD )( ( ( ( *Src & 0x0f ) << 4 ) + ( ( 0x0f - ( *Src & 0x0f ) ) * ( ( *Src & 0xf0 ) >> 4 ) ) ) << 24 ) ;
															AAA = ( DWORD )( ( ( WORD )( *Src & 0x0f ) << 12 ) / Dest[3] ) ;
															*( ( DWORD * )Dest ) |= ( DWORD )( 
																( ( ( ( ( Red1   - Red2   ) * AAA ) >> 8 ) + Red2   ) << MEMIMG_XRGB8_LR ) | 
																( ( ( ( ( Green1 - Green2 ) * AAA ) >> 8 ) + Green2 ) << MEMIMG_XRGB8_LG ) | 
																( ( ( ( ( Blue1  - Blue2  ) * AAA ) >> 8 ) + Blue2  ) << MEMIMG_XRGB8_LB ) ) ; 
														}
													}
													else
													{
														if( *Src & 0xf0 )
														{
															*( ( DWORD * )Dest ) = EdgeColor | ( ( *Src & 0xf0 ) << 24 ) ;
														}
														else
														{
															*( ( DWORD * )Dest ) = 0 ;
														}
													}

													Dest += DstPitch ;
													Src  ++ ;
												}while( --Width ) ;

												Dest += DstPitch2 ;
												Src += SrcPitch2 ;
											}while( --SizeYWord ) ;
										}
									}
									break ;


								case DX_FONTTYPE_ANTIALIASING :
								case DX_FONTTYPE_ANTIALIASING_4X4 :
								case DX_FONTTYPE_ANTIALIASING_8X8 :
									{
										WORD Width ;

										if( TransFlag )
										{
											do
											{
												Width = SizeXWord ;
												do
												{
													if( *Src )
													{
														*( ( DWORD * )Dest ) = ColorData | ( ( *Src & 0x0f ) << 28 ) ;
													}
													Dest += DstPitch ;
													Src ++ ;
												}while( --Width ) ;

												Dest += DstPitch2 ;
												Src += SrcPitch2 ;
											}while( --SizeYWord ) ;
										}
										else
										{
											do
											{
												Width = SizeXWord ;
												do
												{
													if( *Src )
													{
														*( ( DWORD * )Dest ) = ColorData | ( ( *Src & 0x0f ) << 28 ) ;
													}
													else
													{
														*( ( DWORD * )Dest ) = 0 ;
													}

													Dest += DstPitch ;
													Src ++ ;
												}while( --Width ) ;

												Dest += DstPitch2 ;
												Src += SrcPitch2 ;
											}while( --SizeYWord ) ;
										}
									}
									break ;

								case DX_FONTTYPE_NORMAL :
									{
										WORD SizeXTemp ;
										WORD Data ;
										int StX = SrcRect.left % 8 ;
										WORD Table[16] = { 0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1, 0x8000, 0x4000, 0x2000, 0x1000, 0x800, 0x400, 0x200, 0x100 } ;

										Src       = UseManageData->CacheMem + ( SrcRect.left / 8 ) + SrcRect.top * SrcPitch ;
										SrcPitch2 = ( int )( SrcPitch - ( ( SizeX + 7 ) / 8 ) ) ;

										if( TransFlag )
										{
											do{
												SizeXTemp = SizeXWord ;
												do{
													Data = *( ( WORD * )Src ) ;
													for(;;)
													{
														if( Data & Table[StX] )   *( (DWORD *)Dest ) = ColorData ;	Dest += DstPitch ;	if( --SizeXTemp == 0 ) break ;
														if( Data & Table[StX+1] ) *( (DWORD *)Dest ) = ColorData ;	Dest += DstPitch ;	if( --SizeXTemp == 0 ) break ;
														if( Data & Table[StX+2] ) *( (DWORD *)Dest ) = ColorData ;	Dest += DstPitch ;	if( --SizeXTemp == 0 ) break ;
														if( Data & Table[StX+3] ) *( (DWORD *)Dest ) = ColorData ;	Dest += DstPitch ;	if( --SizeXTemp == 0 ) break ;
														if( Data & Table[StX+4] ) *( (DWORD *)Dest ) = ColorData ;	Dest += DstPitch ;	if( --SizeXTemp == 0 ) break ;
														if( Data & Table[StX+5] ) *( (DWORD *)Dest ) = ColorData ;	Dest += DstPitch ;	if( --SizeXTemp == 0 ) break ;
														if( Data & Table[StX+6] ) *( (DWORD *)Dest ) = ColorData ;	Dest += DstPitch ;	if( --SizeXTemp == 0 ) break ;
														if( Data & Table[StX+7] ) *( (DWORD *)Dest ) = ColorData ;	Dest += DstPitch ;	--SizeXTemp ;
														break ;
													}

													Src += 1 ;
												}while( SizeXTemp );

												Src += SrcPitch2 ;
												Dest += DstPitch2 ;
											}while( --SizeYWord ) ;
										}
										else
										{
											do{
												SizeXTemp = SizeXWord ;
												do{
													Data = *( ( WORD * )Src ) ;
													for(;;)
													{
														*( (DWORD *)Dest ) = ( Data & Table[StX] ) ? ColorData : 0 ;   	Dest += DstPitch ;	if( --SizeXTemp == 0 ) break ;
														*( (DWORD *)Dest ) = ( Data & Table[StX+1] ) ? ColorData : 0 ;	Dest += DstPitch ; 	if( --SizeXTemp == 0 ) break ;
														*( (DWORD *)Dest ) = ( Data & Table[StX+2] ) ? ColorData : 0 ;	Dest += DstPitch ; 	if( --SizeXTemp == 0 ) break ;
														*( (DWORD *)Dest ) = ( Data & Table[StX+3] ) ? ColorData : 0 ;	Dest += DstPitch ; 	if( --SizeXTemp == 0 ) break ;
														*( (DWORD *)Dest ) = ( Data & Table[StX+4] ) ? ColorData : 0 ;	Dest += DstPitch ; 	if( --SizeXTemp == 0 ) break ;
														*( (DWORD *)Dest ) = ( Data & Table[StX+5] ) ? ColorData : 0 ;	Dest += DstPitch ; 	if( --SizeXTemp == 0 ) break ;
														*( (DWORD *)Dest ) = ( Data & Table[StX+6] ) ? ColorData : 0 ;	Dest += DstPitch ; 	if( --SizeXTemp == 0 ) break ;
														*( (DWORD *)Dest ) = ( Data & Table[StX+7] ) ? ColorData : 0 ;	Dest += DstPitch ; 	--SizeXTemp ;
														break ;
													}

													Src += 1 ;
												}while( SizeXTemp );

												Src += SrcPitch2 ;
												Dest += DstPitch2 ;
											}while( --SizeYWord ) ;
										}
									}
									break ;

								case DX_FONTTYPE_EDGE :
									{
										WORD Width ;
										DWORD EColor ;

										EColor = ( DWORD )( EdgeColor & 0xffffff ) ;
										if( TransFlag )
										{
											do
											{
												Width = SizeXWord ;
												do
												{
													if( *Src ) *( ( DWORD * )Dest ) = *Src == 1 ? ColorData : EColor ;
													Dest += DstPitch ;
													Src ++ ;
												}while( --Width ) ;

												Dest += DstPitch2 ;
												Src += SrcPitch2 ;
											}while( --SizeYWord ) ;
										}
										else
										{
											do
											{
												Width = SizeXWord ;
												do
												{
													*( ( DWORD * )Dest ) = *Src ? ( *Src == 1 ? ColorData : EColor ) : 0 ;
													Dest += DstPitch ;
													Src ++ ;
												}while( --Width ) ;

												Dest += DstPitch2 ;
												Src += SrcPitch2 ;
											}while( --SizeYWord ) ;
										}
									}
									break ;
								}
							}
							break ;
						}
					}
				}
				else
				{
					int DrawX, DrawY ;
					DWORD SizeX, SizeY ;

					// 描画座標のセット
					DrawX = tmp_xi + _DTOL( DrawPos    ) + CharData->DrawX ;
					DrawY = tmp_yi + _DTOL( DrawPosSub ) + CharData->DrawY ;

					// 実際の描画領域を作成
					SETRECT( DstRect, DrawX, DrawY, DrawX + CharData->SizeX, DrawY + CharData->SizeY ) ;
					RectClipping_Inline( &DstRect, ClipRect ) ;

					// 描画する文字が無かった場合ここで次ループに移る
					if( DstRect.right == DstRect.left || DstRect.bottom == DstRect.top )
					{
						goto LOOPEND ;
					}

					// 描画元矩形と描画先矩形をセット
					SrcRect.left   += DstRect.left - DrawX ;
					SrcRect.top    += DstRect.top  - DrawY ;
					SrcRect.right  -= ( DrawX + CharData->SizeX ) - DstRect.right ;
					SrcRect.bottom -= ( DrawY + CharData->SizeY ) - DstRect.bottom ;
					SizeX = ( DWORD )( SrcRect.right - SrcRect.left ) ;
					SizeY = ( DWORD )( SrcRect.bottom - SrcRect.top ) ;

					// 転送
					{
						DWORD SrcPitch2, DstPitch2 ;
						WORD SizeXWord = ( WORD )SizeX, SizeYWord = ( WORD )SizeY ;

						// カラービットモードによって処理を分岐

						switch( NS_GetColorBitDepth() )
						{
						case 8 :
							// 8ビットモードの時の処理
							{
								BYTE *DrawDstPoint = ( BYTE * )DestMemImg->UseImage ;
								BYTE *DrawSrcPoint = UseManageData->CacheMem ;
								BYTE ColorData     = ( BYTE )( Color & 0xff ) ;

								DrawDstPoint += ( DstRect.left ) + DstRect.top * DstPitch ;
								DrawSrcPoint += ( SrcRect.left ) + SrcRect.top * SrcPitch ;

								SrcPitch2 = SrcPitch - SizeX ;
								DstPitch2 = DstPitch - SizeX ;

								switch( UseManageData->FontType )
								{
								case DX_FONTTYPE_NORMAL :
									{
										WORD SizeXTemp ;
										WORD Data ;

										if( SrcRect.left % 8 == 0 )
										{
											DrawSrcPoint = UseManageData->CacheMem ;
											DrawSrcPoint += ( SrcRect.left / 8 ) + SrcRect.top * SrcPitch ;
											SrcPitch2 = SrcPitch - ( SizeX + 15 ) / 16 * 2 ;
											DstPitch2 = DstPitch - ( SizeX / 16 * 16 ) ;

											if( TransFlag == TRUE )
											{
												do{
													SizeXTemp = SizeXWord ;
													do{
														Data = *( ( WORD * )DrawSrcPoint ) ;
														for(;;)
														{
															if( Data & 0x80 ) *( DrawDstPoint ) = ColorData ;			if( --SizeXTemp == 0 ) break ;
															if( Data & 0x40 ) *( DrawDstPoint + 1 ) = ColorData ;		if( --SizeXTemp == 0 ) break ;
															if( Data & 0x20 ) *( DrawDstPoint + 2 ) = ColorData ;		if( --SizeXTemp == 0 ) break ;
															if( Data & 0x10 ) *( DrawDstPoint + 3 ) = ColorData ;		if( --SizeXTemp == 0 ) break ;
															if( Data & 0x8 ) *( DrawDstPoint + 4 ) = ColorData ;		if( --SizeXTemp == 0 ) break ;
															if( Data & 0x4 ) *( DrawDstPoint + 5 ) = ColorData ;		if( --SizeXTemp == 0 ) break ;
															if( Data & 0x2 ) *( DrawDstPoint + 6 ) = ColorData ;		if( --SizeXTemp == 0 ) break ;
															if( Data & 0x1 ) *( DrawDstPoint + 7 ) = ColorData ;		if( --SizeXTemp == 0 ) break ;
															if( Data & 0x8000 ) *( DrawDstPoint + 8 ) = ColorData ;		if( --SizeXTemp == 0 ) break ;
															if( Data & 0x4000 ) *( DrawDstPoint + 9 ) = ColorData ;		if( --SizeXTemp == 0 ) break ;
															if( Data & 0x2000 ) *( DrawDstPoint + 10 ) = ColorData ;	if( --SizeXTemp == 0 ) break ;
															if( Data & 0x1000 ) *( DrawDstPoint + 11 ) = ColorData ;	if( --SizeXTemp == 0 ) break ;
															if( Data & 0x800 ) *( DrawDstPoint + 12 ) = ColorData ;		if( --SizeXTemp == 0 ) break ;
															if( Data & 0x400 ) *( DrawDstPoint + 13 ) = ColorData ;		if( --SizeXTemp == 0 ) break ;
															if( Data & 0x200 ) *( DrawDstPoint + 14 ) = ColorData ;		if( --SizeXTemp == 0 ) break ;
															if( Data & 0x100 ) *( DrawDstPoint + 15 ) = ColorData ;
															DrawDstPoint += 16 ;
 															--SizeXTemp ;
															break ;
														}

														DrawSrcPoint += 2 ;
													}while( SizeXTemp );

													DrawSrcPoint += SrcPitch2 ;
													DrawDstPoint += DstPitch2 ;
												}while( --SizeYWord ) ;
											}
											else
											{
												do{
													SizeXTemp = SizeXWord ;
													do{
														Data = *( ( WORD * )DrawSrcPoint ) ;
														for(;;)
														{
															*( DrawDstPoint      ) = ( BYTE )( ( Data & 0x80   ) ? ColorData : 0 ) ;	if( --SizeXTemp == 0 ) break ;
															*( DrawDstPoint + 1  ) = ( BYTE )( ( Data & 0x40   ) ? ColorData : 0 ) ;	if( --SizeXTemp == 0 ) break ;
															*( DrawDstPoint + 2  ) = ( BYTE )( ( Data & 0x20   ) ? ColorData : 0 ) ;	if( --SizeXTemp == 0 ) break ;
															*( DrawDstPoint + 3  ) = ( BYTE )( ( Data & 0x10   ) ? ColorData : 0 ) ;	if( --SizeXTemp == 0 ) break ;
															*( DrawDstPoint + 4  ) = ( BYTE )( ( Data & 0x8    ) ? ColorData : 0 ) ;	if( --SizeXTemp == 0 ) break ;
															*( DrawDstPoint + 5  ) = ( BYTE )( ( Data & 0x4    ) ? ColorData : 0 ) ;	if( --SizeXTemp == 0 ) break ;
															*( DrawDstPoint + 6  ) = ( BYTE )( ( Data & 0x2    ) ? ColorData : 0 ) ;	if( --SizeXTemp == 0 ) break ;
															*( DrawDstPoint + 7  ) = ( BYTE )( ( Data & 0x1    ) ? ColorData : 0 ) ;	if( --SizeXTemp == 0 ) break ;
															*( DrawDstPoint + 8  ) = ( BYTE )( ( Data & 0x8000 ) ? ColorData : 0 ) ;  	if( --SizeXTemp == 0 ) break ;
															*( DrawDstPoint + 9  ) = ( BYTE )( ( Data & 0x4000 ) ? ColorData : 0 ) ;  	if( --SizeXTemp == 0 ) break ;
															*( DrawDstPoint + 10 ) = ( BYTE )( ( Data & 0x2000 ) ? ColorData : 0 ) ; 	if( --SizeXTemp == 0 ) break ;
															*( DrawDstPoint + 11 ) = ( BYTE )( ( Data & 0x1000 ) ? ColorData : 0 ) ;	if( --SizeXTemp == 0 ) break ;
															*( DrawDstPoint + 12 ) = ( BYTE )( ( Data & 0x800  ) ? ColorData : 0 ) ; 	if( --SizeXTemp == 0 ) break ;
															*( DrawDstPoint + 13 ) = ( BYTE )( ( Data & 0x400  ) ? ColorData : 0 ) ; 	if( --SizeXTemp == 0 ) break ;
															*( DrawDstPoint + 14 ) = ( BYTE )( ( Data & 0x200  ) ? ColorData : 0 ) ;	if( --SizeXTemp == 0 ) break ;
															*( DrawDstPoint + 15 ) = ( BYTE )( ( Data & 0x100  ) ? ColorData : 0 ) ;  
															DrawDstPoint += 16 ;
 															--SizeXTemp ;
															break ;
														}

														DrawSrcPoint += 2 ;
													}while( SizeXTemp );

													DrawSrcPoint += SrcPitch2 ;
													DrawDstPoint += DstPitch2 ;
												}while( --SizeYWord ) ;
											}
										}
										else
										{
											int StX = SrcRect.left % 8 ;
											WORD Table[16] = { 0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1, 0x8000, 0x4000, 0x2000, 0x1000, 0x800, 0x400, 0x200, 0x100 } ;

											DrawSrcPoint = UseManageData->CacheMem ;
											DrawSrcPoint += ( SrcRect.left / 8 ) + SrcRect.top * SrcPitch ;
											SrcPitch2 = SrcPitch - ( ( SizeX / 8 ) + ( SizeX % 8 != 0 ? 1 : 0 ) ) ;

											DstPitch2 = DstPitch - ( ( ( SizeX >> 3 ) << 3 ) ) ;

											if( TransFlag )
											{
												do{
													SizeXTemp = SizeXWord ;
													do{
														Data = *( ( WORD * )DrawSrcPoint ) ;
														for(;;)
														{
															if( Data & Table[StX] ) *( DrawDstPoint ) = ColorData ;			if( --SizeXTemp == 0 ) break ;
															if( Data & Table[StX+1] ) *( DrawDstPoint + 1 ) = ColorData ;	if( --SizeXTemp == 0 ) break ;
															if( Data & Table[StX+2] ) *( DrawDstPoint + 2 ) = ColorData ;	if( --SizeXTemp == 0 ) break ;
															if( Data & Table[StX+3] ) *( DrawDstPoint + 3 ) = ColorData ;	if( --SizeXTemp == 0 ) break ;
															if( Data & Table[StX+4] ) *( DrawDstPoint + 4 ) = ColorData ;	if( --SizeXTemp == 0 ) break ;
															if( Data & Table[StX+5] ) *( DrawDstPoint + 5 ) = ColorData ;	if( --SizeXTemp == 0 ) break ;
															if( Data & Table[StX+6] ) *( DrawDstPoint + 6 ) = ColorData ;	if( --SizeXTemp == 0 ) break ;
															if( Data & Table[StX+7] ) *( DrawDstPoint + 7 ) = ColorData ;
															DrawDstPoint += 8 ;
 															--SizeXTemp ;
															break ;
														}

														DrawSrcPoint += 1 ;
													}while( SizeXTemp );

													DrawSrcPoint += SrcPitch2 ;
													DrawDstPoint += DstPitch2 ;
												}while( --SizeYWord ) ;
											}
											else
											{
												do{
													SizeXTemp = SizeXWord ;
													do{
														Data = *( ( WORD * )DrawSrcPoint ) ;
														for(;;)
														{
															*( DrawDstPoint     ) = ( BYTE )( ( Data & Table[StX]   ) ? ColorData : 0 ) ;	if( --SizeXTemp == 0 ) break ;
															*( DrawDstPoint + 1 ) = ( BYTE )( ( Data & Table[StX+1] ) ? ColorData : 0 ) ;  	if( --SizeXTemp == 0 ) break ;
															*( DrawDstPoint + 2 ) = ( BYTE )( ( Data & Table[StX+2] ) ? ColorData : 0 ) ;  	if( --SizeXTemp == 0 ) break ;
															*( DrawDstPoint + 3 ) = ( BYTE )( ( Data & Table[StX+3] ) ? ColorData : 0 ) ;  	if( --SizeXTemp == 0 ) break ;
															*( DrawDstPoint + 4 ) = ( BYTE )( ( Data & Table[StX+4] ) ? ColorData : 0 ) ;  	if( --SizeXTemp == 0 ) break ;
															*( DrawDstPoint + 5 ) = ( BYTE )( ( Data & Table[StX+5] ) ? ColorData : 0 ) ;  	if( --SizeXTemp == 0 ) break ;
															*( DrawDstPoint + 6 ) = ( BYTE )( ( Data & Table[StX+6] ) ? ColorData : 0 ) ;  	if( --SizeXTemp == 0 ) break ;
															*( DrawDstPoint + 7 ) = ( BYTE )( ( Data & Table[StX+7] ) ? ColorData : 0 ) ;  
															DrawDstPoint += 8 ;
 															--SizeXTemp ;
															break ;
														}

														DrawSrcPoint += 1 ;
													}while( SizeXTemp );

													DrawSrcPoint += SrcPitch2 ;
													DrawDstPoint += DstPitch2 ;
												}while( --SizeYWord ) ;
											}
										}
									}
									break ;


								case DX_FONTTYPE_EDGE :
									{
										WORD Width ;
										BYTE EColor ;

										EColor = ( BYTE )( EdgeColor & 0xff ) ;
										if( TransFlag )
										{
											do
											{
												Width = SizeXWord ;
												do
												{
													if( *DrawSrcPoint )
													{
														*DrawDstPoint = *DrawSrcPoint == 1 ? ColorData : EColor ;
													}
													DrawDstPoint ++ ;
													DrawSrcPoint ++ ;
												}while( --Width ) ;

												DrawDstPoint += DstPitch2 ;
												DrawSrcPoint += SrcPitch2 ;
											}while( --SizeYWord ) ;
										}
										else
										{
											do
											{
												Width = SizeXWord ;
												do
												{
													*DrawDstPoint = ( BYTE )( ( *DrawSrcPoint ) ? ( *DrawSrcPoint == 1 ? ColorData : EColor ) : 0 ) ;
													DrawDstPoint ++ ;
													DrawSrcPoint ++ ;
												}while( --Width ) ;

												DrawDstPoint += DstPitch2 ;
												DrawSrcPoint += SrcPitch2 ;
											}while( --SizeYWord ) ;
										}
									}
									break ;
								}
							}
							break ;

						case 16 :
							// 16ビットモードの時の処理
							{
								BYTE *DrawDstPoint = DestMemImg->UseImage ;
								BYTE *DrawSrcPoint = UseManageData->CacheMem ;
								WORD ColorData = ( WORD )Color ;

								if( UseManageData->FontType & DX_FONTTYPE_ANTIALIASING )
								{
									DrawDstPoint += DstRect.left * 4 + DstRect.top * DstPitch ;
									DstPitch2     = DstPitch - SizeX * 4 ;
								}
								else
								{
									DrawDstPoint += DstRect.left * 2 + DstRect.top * DstPitch ;
									DstPitch2     = DstPitch - SizeX * 2 ;
								}

								DrawSrcPoint += SrcRect.left + SrcRect.top * SrcPitch ;
								SrcPitch2     = SrcPitch - SizeX ;

								switch( UseManageData->FontType )
								{
								case DX_FONTTYPE_ANTIALIASING_EDGE :
								case DX_FONTTYPE_ANTIALIASING_EDGE_4X4 :
								case DX_FONTTYPE_ANTIALIASING_EDGE_8X8 :
									{
										BYTE Red1, Green1, Blue1 ;
										BYTE Red2, Green2, Blue2 ;
										DWORD AAA ;
										WORD Width ;

										Red1   = ( BYTE )( ( ColorData & MEMIMG_R5G6B5_R ) >> MEMIMG_R5G6B5_LR ) ;
										Green1 = ( BYTE )( ( ColorData & MEMIMG_R5G6B5_G ) >> MEMIMG_R5G6B5_LG ) ;
										Blue1  = ( BYTE )( ( ColorData & MEMIMG_R5G6B5_B ) >> MEMIMG_R5G6B5_LB ) ;

										Red2   = ( BYTE )( ( EdgeColor & MEMIMG_R5G6B5_R ) >> MEMIMG_R5G6B5_LR ) ;
										Green2 = ( BYTE )( ( EdgeColor & MEMIMG_R5G6B5_G ) >> MEMIMG_R5G6B5_LG ) ;
										Blue2  = ( BYTE )( ( EdgeColor & MEMIMG_R5G6B5_B ) >> MEMIMG_R5G6B5_LB ) ;

										if( TransFlag )
										{
											do
											{
												Width = SizeXWord ;
												do
												{
													if( *DrawSrcPoint & 0x0f )
													{
														if( ( *DrawSrcPoint & 0x0f ) == 0x0f )
														{
															*( ( DWORD * )DrawDstPoint ) = ( DWORD )( 0xff0000 | ColorData ) ;
														}
														else
														{
															DrawDstPoint[2] = ( BYTE )( ( ( *DrawSrcPoint & 0x0f ) << 4 ) + ( ( 0x0f - ( *DrawSrcPoint & 0x0f ) ) * ( ( *DrawSrcPoint & 0xf0 ) >> 4 ) ) ) ;
															AAA = ( DWORD )( ( ( WORD )( *DrawSrcPoint & 0x0f ) << 12 ) / DrawDstPoint[2] ) ;
															*( ( WORD * )DrawDstPoint ) = ( WORD )( 
																( ( ( ( ( Red1   - Red2   ) * AAA ) >> 8 ) + Red2   ) << MEMIMG_R5G6B5_LR ) | 
																( ( ( ( ( Green1 - Green2 ) * AAA ) >> 8 ) + Green2 ) << MEMIMG_R5G6B5_LG ) | 
																( ( ( ( ( Blue1  - Blue2  ) * AAA ) >> 8 ) + Blue2  ) << MEMIMG_R5G6B5_LB ) ) ; 
														}
													}
													else
													{
														if( *DrawSrcPoint & 0xf0 )
														{
															*( ( DWORD * )DrawDstPoint ) = EdgeColor | ( ( *DrawSrcPoint & 0xf0 ) << 16 ) ;
														}
													}

													DrawDstPoint += 4 ;
													DrawSrcPoint += 1 ;
												}while( --Width ) ;

												DrawDstPoint += DstPitch2 ;
												DrawSrcPoint += SrcPitch2 ;
											}while( --SizeYWord ) ;
										}
										else
										{
											do
											{
												Width = SizeXWord ;
												do
												{
													if( *DrawSrcPoint & 0x0f )
													{
														if( ( *DrawSrcPoint & 0x0f ) == 0x0f )
														{
															*( ( DWORD * )DrawDstPoint ) = ( DWORD )( 0xff0000 | ColorData ) ;
														}
														else
														{
															DrawDstPoint[2] = ( BYTE )( ( ( *DrawSrcPoint & 0x0f ) << 4 ) + ( ( 0x0f - ( *DrawSrcPoint & 0x0f ) ) * ( ( *DrawSrcPoint & 0xf0 ) >> 4 ) ) ) ;
															AAA = ( DWORD )( ( ( WORD )( *DrawSrcPoint & 0x0f ) << 12 ) / DrawDstPoint[2] ) ;
															*( ( WORD * )DrawDstPoint ) = ( WORD )( 
																( ( ( ( ( Red1   - Red2   ) * AAA ) >> 8 ) + Red2   ) << MEMIMG_R5G6B5_LR ) | 
																( ( ( ( ( Green1 - Green2 ) * AAA ) >> 8 ) + Green2 ) << MEMIMG_R5G6B5_LG ) | 
																( ( ( ( ( Blue1  - Blue2  ) * AAA ) >> 8 ) + Blue2  ) << MEMIMG_R5G6B5_LB ) ) ; 
														}
													}
													else
													{
														if( *DrawSrcPoint & 0xf0 )
														{
															*( ( DWORD * )DrawDstPoint ) = EdgeColor | ( ( *DrawSrcPoint & 0xf0 ) << 16 ) ;
														}
														else
														{
															*( ( DWORD * )DrawDstPoint ) = 0 ;
														}
													}

													DrawDstPoint += 4 ;
													DrawSrcPoint += 1 ;
												}while( --Width ) ;

												DrawDstPoint += DstPitch2 ;
												DrawSrcPoint += SrcPitch2 ;
											}while( --SizeYWord ) ;
										}
									}
									break ;

								case DX_FONTTYPE_ANTIALIASING :
								case DX_FONTTYPE_ANTIALIASING_4X4 :
								case DX_FONTTYPE_ANTIALIASING_8X8 :
									{
										WORD Width ;

										if( TransFlag )
										{
											do
											{
												Width = SizeXWord ;
												do
												{
													if( *DrawSrcPoint )
													{
														*( ( DWORD * )DrawDstPoint ) = ( DWORD )( ColorData | ( *DrawSrcPoint << 20 ) ) ;
													}
													DrawDstPoint += 4 ;
													DrawSrcPoint ++ ;
												}while( --Width ) ;

												DrawDstPoint += DstPitch2 ;
												DrawSrcPoint += SrcPitch2 ;
											}while( --SizeYWord ) ;
										}
										else
										{
											do
											{
												Width = SizeXWord ;
												do
												{
													if( *DrawSrcPoint )
													{
														*( ( DWORD * )DrawDstPoint ) = ( DWORD )( ColorData | ( *DrawSrcPoint << 20 ) ) ;
													}
													else
													{
														*( ( DWORD * )DrawDstPoint ) = 0 ;
													}

													DrawDstPoint += 4 ;
													DrawSrcPoint ++ ;
												}while( --Width ) ;

												DrawDstPoint += DstPitch2 ;
												DrawSrcPoint += SrcPitch2 ;
											}while( --SizeYWord ) ;
										}
									}
									break ;

								case DX_FONTTYPE_NORMAL :
									{
										WORD SizeXTemp ;
										WORD Data ;

										if( SrcRect.left % 8 == 0 )
										{
											DrawSrcPoint = UseManageData->CacheMem ;
											DrawSrcPoint += ( SrcRect.left / 8 ) + SrcRect.top * SrcPitch ;
											SrcPitch2 = SrcPitch - ( ( SizeX / 16 ) * 2 + ( SizeX % 16 != 0 ? 2 : 0 ) ) ;
											DstPitch2 = DstPitch - ( ( ( SizeX >> 4 ) << 5 ) ) ;

											if( TransFlag )
											{
												do{
													SizeXTemp = SizeXWord ;
													do{
														Data = *( ( WORD * )DrawSrcPoint ) ;
														for(;;)
														{
															if( Data & 0x80 ) *( ( WORD * )DrawDstPoint ) = ColorData ;			if( --SizeXTemp == 0 ) break ;
															if( Data & 0x40 ) *( ( WORD * )DrawDstPoint + 1 ) = ColorData ; 	if( --SizeXTemp == 0 ) break ;
															if( Data & 0x20 ) *( ( WORD * )DrawDstPoint + 2 ) = ColorData ;		if( --SizeXTemp == 0 ) break ;
															if( Data & 0x10 ) *( ( WORD * )DrawDstPoint + 3 ) = ColorData ;		if( --SizeXTemp == 0 ) break ;
															if( Data & 0x8 ) *( ( WORD * )DrawDstPoint + 4 ) = ColorData ;		if( --SizeXTemp == 0 ) break ;
															if( Data & 0x4 ) *( ( WORD * )DrawDstPoint + 5 ) = ColorData ;		if( --SizeXTemp == 0 ) break ;
															if( Data & 0x2 ) *( ( WORD * )DrawDstPoint + 6 ) = ColorData ;		if( --SizeXTemp == 0 ) break ;
															if( Data & 0x1 ) *( ( WORD * )DrawDstPoint + 7 ) = ColorData ;		if( --SizeXTemp == 0 ) break ;
															if( Data & 0x8000 ) *( ( WORD * )DrawDstPoint + 8 ) = ColorData ;	if( --SizeXTemp == 0 ) break ;
															if( Data & 0x4000 ) *( ( WORD * )DrawDstPoint + 9 ) = ColorData ;	if( --SizeXTemp == 0 ) break ;
															if( Data & 0x2000 ) *( ( WORD * )DrawDstPoint + 10 ) = ColorData ;	if( --SizeXTemp == 0 ) break ;
															if( Data & 0x1000 ) *( ( WORD * )DrawDstPoint + 11 ) = ColorData ;	if( --SizeXTemp == 0 ) break ;
															if( Data & 0x800 ) *( ( WORD * )DrawDstPoint + 12 ) = ColorData ;	if( --SizeXTemp == 0 ) break ;
															if( Data & 0x400 ) *( ( WORD * )DrawDstPoint + 13 ) = ColorData ;	if( --SizeXTemp == 0 ) break ;
															if( Data & 0x200 ) *( ( WORD * )DrawDstPoint + 14 ) = ColorData ;	if( --SizeXTemp == 0 ) break ;
															if( Data & 0x100 ) *( ( WORD * )DrawDstPoint + 15 ) = ColorData ;
															DrawDstPoint += 32 ;
 															--SizeXTemp ;
															break ;
														}

														DrawSrcPoint += 2 ;
													}while( SizeXTemp );

													DrawSrcPoint += SrcPitch2 ;
													DrawDstPoint += DstPitch2 ;
												}while( --SizeYWord ) ;
											}
											else
											{
												do{
													SizeXTemp = SizeXWord ;
													do{
														Data = *( ( WORD * )DrawSrcPoint ) ;
														for(;;)
														{
															*( ( WORD * )DrawDstPoint      ) = ( WORD )( ( Data & 0x80   ) ? ColorData : 0 ) ;	if( --SizeXTemp == 0 ) break ;
															*( ( WORD * )DrawDstPoint + 1  ) = ( WORD )( ( Data & 0x40   ) ? ColorData : 0 ) ; 	if( --SizeXTemp == 0 ) break ;
															*( ( WORD * )DrawDstPoint + 2  ) = ( WORD )( ( Data & 0x20   ) ? ColorData : 0 ) ;	if( --SizeXTemp == 0 ) break ;
															*( ( WORD * )DrawDstPoint + 3  ) = ( WORD )( ( Data & 0x10   ) ? ColorData : 0 ) ; 	if( --SizeXTemp == 0 ) break ;
															*( ( WORD * )DrawDstPoint + 4  ) = ( WORD )( ( Data & 0x8    ) ? ColorData : 0 ) ; 	if( --SizeXTemp == 0 ) break ;
															*( ( WORD * )DrawDstPoint + 5  ) = ( WORD )( ( Data & 0x4    ) ? ColorData : 0 ) ; 	if( --SizeXTemp == 0 ) break ;
															*( ( WORD * )DrawDstPoint + 6  ) = ( WORD )( ( Data & 0x2    ) ? ColorData : 0 ) ; 	if( --SizeXTemp == 0 ) break ;
															*( ( WORD * )DrawDstPoint + 7  ) = ( WORD )( ( Data & 0x1    ) ? ColorData : 0 ) ; 	if( --SizeXTemp == 0 ) break ;
															*( ( WORD * )DrawDstPoint + 8  ) = ( WORD )( ( Data & 0x8000 ) ? ColorData : 0 ) ;	if( --SizeXTemp == 0 ) break ;
															*( ( WORD * )DrawDstPoint + 9  ) = ( WORD )( ( Data & 0x4000 ) ? ColorData : 0 ) ;	if( --SizeXTemp == 0 ) break ;
															*( ( WORD * )DrawDstPoint + 10 ) = ( WORD )( ( Data & 0x2000 ) ? ColorData : 0 ) ;	if( --SizeXTemp == 0 ) break ;
															*( ( WORD * )DrawDstPoint + 11 ) = ( WORD )( ( Data & 0x1000 ) ? ColorData : 0 ) ;	if( --SizeXTemp == 0 ) break ;
															*( ( WORD * )DrawDstPoint + 12 ) = ( WORD )( ( Data & 0x800  ) ? ColorData : 0 ) ;	if( --SizeXTemp == 0 ) break ;
															*( ( WORD * )DrawDstPoint + 13 ) = ( WORD )( ( Data & 0x400  ) ? ColorData : 0 ) ; 	if( --SizeXTemp == 0 ) break ;
															*( ( WORD * )DrawDstPoint + 14 ) = ( WORD )( ( Data & 0x200  ) ? ColorData : 0 ) ; 	if( --SizeXTemp == 0 ) break ;
															*( ( WORD * )DrawDstPoint + 15 ) = ( WORD )( ( Data & 0x100  ) ? ColorData : 0 ) ;  
															DrawDstPoint += 32 ;
 															--SizeXTemp ;
															break ;
														}

														DrawSrcPoint += 2 ;
													}while( SizeXTemp );

													DrawSrcPoint += SrcPitch2 ;
													DrawDstPoint += DstPitch2 ;
												}while( --SizeYWord ) ;
											}
										}
										else
										{
											int StX = SrcRect.left % 8 ;
											WORD Table[16] = { 0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1, 0x8000, 0x4000, 0x2000, 0x1000, 0x800, 0x400, 0x200, 0x100 } ;

											DrawSrcPoint = UseManageData->CacheMem ;
											DrawSrcPoint += ( SrcRect.left / 8 ) + SrcRect.top * SrcPitch ;
											SrcPitch2 = SrcPitch - ( ( SizeX / 8 ) + ( SizeX % 8 != 0 ? 1 : 0 ) ) ;
											DstPitch2 = DstPitch - ( ( ( SizeX >> 3 ) << 4 ) ) /*- ( SizeX % 4 == 0 ? 8 : 0 )*/ ;

											if( TransFlag )
											{
												do{
													SizeXTemp = SizeXWord ;
													do{
														Data = *( ( WORD * )DrawSrcPoint ) ;
														for(;;)
														{
															if( Data & Table[StX] ) *( ( WORD * )DrawDstPoint ) = ColorData ; 			if( --SizeXTemp == 0 ) break ;
															if( Data & Table[StX+1] ) *( ( WORD * )DrawDstPoint + 1 ) = ColorData ;		if( --SizeXTemp == 0 ) break ;
															if( Data & Table[StX+2] ) *( ( WORD * )DrawDstPoint + 2 ) = ColorData ;		if( --SizeXTemp == 0 ) break ;
															if( Data & Table[StX+3] ) *( ( WORD * )DrawDstPoint + 3 ) = ColorData ;		if( --SizeXTemp == 0 ) break ;
															if( Data & Table[StX+4] ) *( ( WORD * )DrawDstPoint + 4 ) = ColorData ;		if( --SizeXTemp == 0 ) break ;
															if( Data & Table[StX+5] ) *( ( WORD * )DrawDstPoint + 5 ) = ColorData ;		if( --SizeXTemp == 0 ) break ;
															if( Data & Table[StX+6] ) *( ( WORD * )DrawDstPoint + 6 ) = ColorData ;		if( --SizeXTemp == 0 ) break ;
															if( Data & Table[StX+7] ) *( ( WORD * )DrawDstPoint + 7 ) = ColorData ;
 															--SizeXTemp ;
															DrawDstPoint += 16 ;
															break ;
														}

														DrawSrcPoint += 1 ;
													}while( SizeXTemp );

													DrawSrcPoint += SrcPitch2 ;
													DrawDstPoint += DstPitch2 ;
												}while( --SizeYWord ) ;
											}
											else
											{
												do{
													SizeXTemp = SizeXWord ;
													do{
														Data = *( ( WORD * )DrawSrcPoint ) ;
														for(;;)
														{
															*( ( WORD * )DrawDstPoint     ) = ( WORD )( ( Data & Table[StX]   ) ? ColorData : 0 ) ;	if( --SizeXTemp == 0 ) break ;
															*( ( WORD * )DrawDstPoint + 1 ) = ( WORD )( ( Data & Table[StX+1] ) ? ColorData : 0 ) ;	if( --SizeXTemp == 0 ) break ;
															*( ( WORD * )DrawDstPoint + 2 ) = ( WORD )( ( Data & Table[StX+2] ) ? ColorData : 0 ) ;	if( --SizeXTemp == 0 ) break ;
															*( ( WORD * )DrawDstPoint + 3 ) = ( WORD )( ( Data & Table[StX+3] ) ? ColorData : 0 ) ;	if( --SizeXTemp == 0 ) break ;
															*( ( WORD * )DrawDstPoint + 4 ) = ( WORD )( ( Data & Table[StX+4] ) ? ColorData : 0 ) ;	if( --SizeXTemp == 0 ) break ;
															*( ( WORD * )DrawDstPoint + 5 ) = ( WORD )( ( Data & Table[StX+5] ) ? ColorData : 0 ) ;	if( --SizeXTemp == 0 ) break ;
															*( ( WORD * )DrawDstPoint + 6 ) = ( WORD )( ( Data & Table[StX+6] ) ? ColorData : 0 ) ;	if( --SizeXTemp == 0 ) break ;
															*( ( WORD * )DrawDstPoint + 7 ) = ( WORD )( ( Data & Table[StX+7] ) ? ColorData : 0 ) ;  
 															--SizeXTemp ;
															DrawDstPoint += 16 ;
															break ;
														}

														DrawSrcPoint += 1 ;
													}while( SizeXTemp );

													DrawSrcPoint += SrcPitch2 ;
													DrawDstPoint += DstPitch2 ;
												}while( --SizeYWord ) ;
											}
										}
									}
									break ;

								case DX_FONTTYPE_EDGE :
									{
										WORD Width ;
										WORD EColor ;

										EColor = ( WORD )( EdgeColor & 0xffff ) ;
										if( TransFlag )
										{
											do
											{
												Width = SizeXWord ;
												do
												{
													if( *DrawSrcPoint )
													{
														*( ( WORD * )DrawDstPoint ) = *DrawSrcPoint == 1 ? ColorData : EColor ;
													}
													DrawDstPoint += 2 ;
													DrawSrcPoint ++ ;
												}while( --Width ) ;

												DrawDstPoint += DstPitch2 ;
												DrawSrcPoint += SrcPitch2 ;
											}while( --SizeYWord ) ;
										}
										else
										{
											do
											{
												Width = SizeXWord ;
												do
												{
													*( ( WORD * )DrawDstPoint ) = ( WORD )( *DrawSrcPoint ? ( *DrawSrcPoint == 1 ? ColorData : EColor ) : 0 ) ;
													DrawDstPoint += 2 ;
													DrawSrcPoint ++ ;
												}while( --Width ) ;

												DrawDstPoint += DstPitch2 ;
												DrawSrcPoint += SrcPitch2 ;
											}while( --SizeYWord ) ;
										}
									}
									break ;
								}
							}
							break ;

						case 32 :
							// 32ビットモードの場合
							{
								BYTE *DrawDstPoint = DestMemImg->UseImage ;
								BYTE *DrawSrcPoint = UseManageData->CacheMem ;
								unsigned int ColorData = ( DWORD )Color ;
   
								DrawDstPoint += ( DstRect.left << 2 ) + DstRect.top * DstPitch ;
								DrawSrcPoint += ( SrcRect.left ) + SrcRect.top * SrcPitch ;
								SrcPitch2 = SrcPitch - SizeX ;
								DstPitch2 = DstPitch - ( SizeX << 2 ) ;

								switch( UseManageData->FontType )
								{
								case DX_FONTTYPE_ANTIALIASING_EDGE :
								case DX_FONTTYPE_ANTIALIASING_EDGE_4X4 :
								case DX_FONTTYPE_ANTIALIASING_EDGE_8X8 :
									{
										BYTE Red1, Green1, Blue1 ;
										BYTE Red2, Green2, Blue2 ;
										DWORD AAA ;

										WORD Width ;

										Red1   = ( BYTE )( ( ColorData & MEMIMG_XRGB8_R ) >> MEMIMG_XRGB8_LR ) ;
										Green1 = ( BYTE )( ( ColorData & MEMIMG_XRGB8_G ) >> MEMIMG_XRGB8_LG ) ;
										Blue1  = ( BYTE )( ( ColorData & MEMIMG_XRGB8_B ) >> MEMIMG_XRGB8_LB ) ;

										Red2   = ( BYTE )( ( EdgeColor & MEMIMG_XRGB8_R ) >> MEMIMG_XRGB8_LR ) ;
										Green2 = ( BYTE )( ( EdgeColor & MEMIMG_XRGB8_G ) >> MEMIMG_XRGB8_LG ) ;
										Blue2  = ( BYTE )( ( EdgeColor & MEMIMG_XRGB8_B ) >> MEMIMG_XRGB8_LB ) ;

										Red1 = ( BYTE )( ( ColorData & MEMIMG_XRGB8_R ) >> MEMIMG_XRGB8_LR );
										Green1 = ( BYTE )( ( ColorData & MEMIMG_XRGB8_G ) >> MEMIMG_XRGB8_LG );
										Blue1 = ( BYTE )( ( ColorData & MEMIMG_XRGB8_B ) >> MEMIMG_XRGB8_LB );

										Red2 = ( BYTE )( ( EdgeColor & MEMIMG_XRGB8_R ) >> MEMIMG_XRGB8_LR );
										Green2 = ( BYTE )( ( EdgeColor & MEMIMG_XRGB8_G ) >> MEMIMG_XRGB8_LG );
										Blue2 = ( BYTE )( ( EdgeColor & MEMIMG_XRGB8_B ) >> MEMIMG_XRGB8_LB );

										if( TransFlag )
										{
											do
											{
												Width = SizeXWord ;
												do
												{
													if( *DrawSrcPoint & 0x0f )
													{
														if( ( *DrawSrcPoint & 0x0f ) == 0x0f )
														{
															*( ( DWORD * )DrawDstPoint ) = 0xff000000 | ColorData ;
														}
														else
														{
															*( ( DWORD * )DrawDstPoint ) = ( DWORD )( ( ( ( *DrawSrcPoint & 0x0f ) << 4 ) + ( ( 0x0f - ( *DrawSrcPoint & 0x0f ) ) * ( ( *DrawSrcPoint & 0xf0 ) >> 4 ) ) ) << 24 ) ;
															AAA = ( DWORD )( ( ( WORD )( *DrawSrcPoint & 0x0f ) << 12 ) / DrawDstPoint[3] ) ;
															*( ( DWORD * )DrawDstPoint ) |= ( DWORD )( 
																( ( ( ( ( Red1   - Red2   ) * AAA ) >> 8 ) + Red2   ) << MEMIMG_XRGB8_LR ) | 
																( ( ( ( ( Green1 - Green2 ) * AAA ) >> 8 ) + Green2 ) << MEMIMG_XRGB8_LG ) | 
																( ( ( ( ( Blue1  - Blue2  ) * AAA ) >> 8 ) + Blue2  ) << MEMIMG_XRGB8_LB ) ) ; 
														}
													}
													else
													{
														if( *DrawSrcPoint & 0xf0 )
														{
															*( ( DWORD * )DrawDstPoint ) = EdgeColor | ( ( *DrawSrcPoint & 0xf0 ) << 24 ) ;
														}
													}

													DrawDstPoint += 4 ;
													DrawSrcPoint ++ ;
												}while( --Width ) ;

												DrawDstPoint += DstPitch2 ;
												DrawSrcPoint += SrcPitch2 ;
											}while( --SizeYWord ) ;
										}
										else
										{
											do
											{
												Width = SizeXWord ;
												do
												{
													if( *DrawSrcPoint & 0x0f )
													{
														if( ( *DrawSrcPoint & 0x0f ) == 0x0f )
														{
															*( ( DWORD * )DrawDstPoint ) = 0xff000000 | ColorData ;
														}
														else
														{
															*( ( DWORD * )DrawDstPoint ) = ( DWORD )( ( ( ( *DrawSrcPoint & 0x0f ) << 4 ) + ( ( 0x0f - ( *DrawSrcPoint & 0x0f ) ) * ( ( *DrawSrcPoint & 0xf0 ) >> 4 ) ) ) << 24 ) ;
															AAA = ( DWORD )( ( ( WORD )( *DrawSrcPoint & 0x0f ) << 12 ) / DrawDstPoint[3] ) ;
															*( ( DWORD * )DrawDstPoint ) |= ( DWORD )( 
																( ( ( ( ( Red1   - Red2   ) * AAA ) >> 8 ) + Red2   ) << MEMIMG_XRGB8_LR ) | 
																( ( ( ( ( Green1 - Green2 ) * AAA ) >> 8 ) + Green2 ) << MEMIMG_XRGB8_LG ) | 
																( ( ( ( ( Blue1  - Blue2  ) * AAA ) >> 8 ) + Blue2  ) << MEMIMG_XRGB8_LB ) ) ; 
														}
													}
													else
													{
														if( *DrawSrcPoint & 0xf0 )
														{
															*( ( DWORD * )DrawDstPoint ) = EdgeColor | ( ( *DrawSrcPoint & 0xf0 ) << 24 ) ;
														}
														else
														{
															*( ( DWORD * )DrawDstPoint ) = 0 ;
														}
													}

													DrawDstPoint += 4 ;
													DrawSrcPoint ++ ;
												}while( --Width ) ;

												DrawDstPoint += DstPitch2 ;
												DrawSrcPoint += SrcPitch2 ;
											}while( --SizeYWord ) ;
										}
									}
									break ;


								case DX_FONTTYPE_ANTIALIASING :
								case DX_FONTTYPE_ANTIALIASING_4X4 :
								case DX_FONTTYPE_ANTIALIASING_8X8 :
									{
										WORD Width ;

										if( TransFlag )
										{
											do
											{
												Width = SizeXWord ;
												do
												{
													if( *DrawSrcPoint )
													{
														*( ( DWORD * )DrawDstPoint ) = ColorData | ( ( *DrawSrcPoint & 0x0f ) << 28 ) ;
													}
													DrawDstPoint += 4 ;
													DrawSrcPoint ++ ;
												}while( --Width ) ;

												DrawDstPoint += DstPitch2 ;
												DrawSrcPoint += SrcPitch2 ;
											}while( --SizeYWord ) ;
										}
										else
										{
											do
											{
												Width = SizeXWord ;
												do
												{
													if( *DrawSrcPoint )
													{
														*( ( DWORD * )DrawDstPoint ) = ColorData | ( ( *DrawSrcPoint & 0x0f ) << 28 ) ;
													}
													else
													{
														*( ( DWORD * )DrawDstPoint ) = 0 ;
													}

													DrawDstPoint += 4 ;
													DrawSrcPoint ++ ;
												}while( --Width ) ;

												DrawDstPoint += DstPitch2 ;
												DrawSrcPoint += SrcPitch2 ;
											}while( --SizeYWord ) ;
										}
									}
									break ;

								case DX_FONTTYPE_NORMAL :
									{
										WORD SizeXTemp ;
										WORD Data ;

										if( SrcRect.left % 8 == 0 )
										{
											DrawSrcPoint = UseManageData->CacheMem ;
											DrawSrcPoint += ( SrcRect.left / 8 ) + SrcRect.top * SrcPitch ;
											SrcPitch2 = SrcPitch - ( ( SizeX / 16 ) * 2 + ( SizeX % 16 != 0 ? 2 : 0 ) ) ;

											DstPitch2 = DstPitch - ( ( ( SizeX >> 4 ) << 6 ) ) ;

											if( TransFlag )
											{
												do{
													SizeXTemp = SizeXWord ;
													do{
														Data = *( ( WORD * )DrawSrcPoint ) ;
														for(;;)
														{
															if( Data & 0x80 ) *( ( DWORD * )DrawDstPoint ) = ColorData ; 		if( --SizeXTemp == 0 ) break ;
															if( Data & 0x40 ) *( ( DWORD * )DrawDstPoint + 1 ) = ColorData ;	if( --SizeXTemp == 0 ) break ;
															if( Data & 0x20 ) *( ( DWORD * )DrawDstPoint + 2 ) = ColorData ;	if( --SizeXTemp == 0 ) break ;
															if( Data & 0x10 ) *( ( DWORD * )DrawDstPoint + 3 ) = ColorData ;	if( --SizeXTemp == 0 ) break ;
															if( Data & 0x8 ) *( ( DWORD * )DrawDstPoint + 4 ) = ColorData ;		if( --SizeXTemp == 0 ) break ;
															if( Data & 0x4 ) *( ( DWORD * )DrawDstPoint + 5 ) = ColorData ;		if( --SizeXTemp == 0 ) break ;
															if( Data & 0x2 ) *( ( DWORD * )DrawDstPoint + 6 ) = ColorData ;		if( --SizeXTemp == 0 ) break ;
															if( Data & 0x1 ) *( ( DWORD * )DrawDstPoint + 7 ) = ColorData ;		if( --SizeXTemp == 0 ) break ;
															if( Data & 0x8000 ) *( ( DWORD * )DrawDstPoint + 8 ) = ColorData ;	if( --SizeXTemp == 0 ) break ;
															if( Data & 0x4000 ) *( ( DWORD * )DrawDstPoint + 9 ) = ColorData ;	if( --SizeXTemp == 0 ) break ;
															if( Data & 0x2000 ) *( ( DWORD * )DrawDstPoint + 10 ) = ColorData ;	if( --SizeXTemp == 0 ) break ;
															if( Data & 0x1000 ) *( ( DWORD * )DrawDstPoint + 11 ) = ColorData ;	if( --SizeXTemp == 0 ) break ;
															if( Data & 0x800 ) *( ( DWORD * )DrawDstPoint + 12 ) = ColorData ;	if( --SizeXTemp == 0 ) break ;
															if( Data & 0x400 ) *( ( DWORD * )DrawDstPoint + 13 ) = ColorData ;	if( --SizeXTemp == 0 ) break ;
															if( Data & 0x200 ) *( ( DWORD * )DrawDstPoint + 14 ) = ColorData ;	if( --SizeXTemp == 0 ) break ;
															if( Data & 0x100 ) *( ( DWORD * )DrawDstPoint + 15 ) = ColorData ;
															DrawDstPoint += 64 ;
 															--SizeXTemp ;
															break ;
														}

														DrawSrcPoint += 2 ;
													}while( SizeXTemp );

													DrawSrcPoint += SrcPitch2 ;
													DrawDstPoint += DstPitch2 ;
												}while( --SizeYWord ) ;
											}
											else
											{
												do{
													SizeXTemp = SizeXWord ;
													do{
														Data = *( ( WORD * )DrawSrcPoint ) ;
														for(;;)
														{
															*( ( DWORD * )DrawDstPoint ) = ( Data & 0x80 ) ? ColorData : 0 ;  		if( --SizeXTemp == 0 ) break ;
															*( ( DWORD * )DrawDstPoint + 1 ) = ( Data & 0x40 ) ? ColorData : 0 ;	if( --SizeXTemp == 0 ) break ;
															*( ( DWORD * )DrawDstPoint + 2 ) = ( Data & 0x20 ) ? ColorData : 0 ;  	if( --SizeXTemp == 0 ) break ;
															*( ( DWORD * )DrawDstPoint + 3 ) = ( Data & 0x10 ) ? ColorData : 0 ;  	if( --SizeXTemp == 0 ) break ;
															*( ( DWORD * )DrawDstPoint + 4 ) = ( Data & 0x8 ) ? ColorData : 0 ;  	if( --SizeXTemp == 0 ) break ;
															*( ( DWORD * )DrawDstPoint + 5 ) = ( Data & 0x4 ) ? ColorData : 0 ;  	if( --SizeXTemp == 0 ) break ;
															*( ( DWORD * )DrawDstPoint + 6 ) = ( Data & 0x2 ) ? ColorData : 0 ;  	if( --SizeXTemp == 0 ) break ;
															*( ( DWORD * )DrawDstPoint + 7 ) = ( Data & 0x1 ) ? ColorData : 0 ;  	if( --SizeXTemp == 0 ) break ;
															*( ( DWORD * )DrawDstPoint + 8 ) = ( Data & 0x8000 ) ? ColorData : 0 ;  if( --SizeXTemp == 0 ) break ;
															*( ( DWORD * )DrawDstPoint + 9 ) = ( Data & 0x4000 ) ? ColorData : 0 ;  if( --SizeXTemp == 0 ) break ;
															*( ( DWORD * )DrawDstPoint + 10 ) = ( Data & 0x2000 ) ? ColorData : 0 ; if( --SizeXTemp == 0 ) break ;
															*( ( DWORD * )DrawDstPoint + 11 ) = ( Data & 0x1000 ) ? ColorData : 0 ; if( --SizeXTemp == 0 ) break ;
															*( ( DWORD * )DrawDstPoint + 12 ) = ( Data & 0x800 ) ? ColorData : 0 ;  if( --SizeXTemp == 0 ) break ;
															*( ( DWORD * )DrawDstPoint + 13 ) = ( Data & 0x400 ) ? ColorData : 0 ;  if( --SizeXTemp == 0 ) break ;
															*( ( DWORD * )DrawDstPoint + 14 ) = ( Data & 0x200 ) ? ColorData : 0 ;  if( --SizeXTemp == 0 ) break ;
															*( ( DWORD * )DrawDstPoint + 15 ) = ( Data & 0x100 ) ? ColorData : 0 ;  
															DrawDstPoint += 64 ;
 															--SizeXTemp ;
															break ;
														}

														DrawSrcPoint += 2 ;
													}while( SizeXTemp );

													DrawSrcPoint += SrcPitch2 ;
													DrawDstPoint += DstPitch2 ;
												}while( --SizeYWord ) ;
											}
										}
										else
										{
											int StX = SrcRect.left % 8 ;
											WORD Table[16] = { 0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1, 0x8000, 0x4000, 0x2000, 0x1000, 0x800, 0x400, 0x200, 0x100 } ;

											DrawSrcPoint = UseManageData->CacheMem ;
											DrawSrcPoint += ( SrcRect.left / 8 ) + SrcRect.top * SrcPitch ;
											SrcPitch2 = SrcPitch - ( ( SizeX / 8 ) + ( SizeX % 8 != 0 ? 1 : 0 ) ) ;

											DstPitch2 = DstPitch - ( ( ( SizeX >> 3 ) << 5 ) ) ;

											if( TransFlag )
											{
												do{
													SizeXTemp = SizeXWord ;
													do{
														Data = *( ( WORD * )DrawSrcPoint ) ;
														for(;;)
														{
															if( Data & Table[StX] ) *( ( DWORD * )DrawDstPoint ) = ColorData ;			if( --SizeXTemp == 0 ) break ;
															if( Data & Table[StX+1] ) *( ( DWORD * )DrawDstPoint + 1 ) = ColorData ;	if( --SizeXTemp == 0 ) break ;
															if( Data & Table[StX+2] ) *( ( DWORD * )DrawDstPoint + 2 ) = ColorData ;	if( --SizeXTemp == 0 ) break ;
															if( Data & Table[StX+3] ) *( ( DWORD * )DrawDstPoint + 3 ) = ColorData ;	if( --SizeXTemp == 0 ) break ;
															if( Data & Table[StX+4] ) *( ( DWORD * )DrawDstPoint + 4 ) = ColorData ;	if( --SizeXTemp == 0 ) break ;
															if( Data & Table[StX+5] ) *( ( DWORD * )DrawDstPoint + 5 ) = ColorData ;	if( --SizeXTemp == 0 ) break ;
															if( Data & Table[StX+6] ) *( ( DWORD * )DrawDstPoint + 6 ) = ColorData ;	if( --SizeXTemp == 0 ) break ;
															if( Data & Table[StX+7] ) *( ( DWORD * )DrawDstPoint + 7 ) = ColorData ;
															DrawDstPoint += 32 ;
 															--SizeXTemp ;
															break ;
														}

														DrawSrcPoint += 1 ;
													}while( SizeXTemp );

													DrawSrcPoint += SrcPitch2 ;
													DrawDstPoint += DstPitch2 ;
												}while( --SizeYWord ) ;
											}
											else
											{
												do{
													SizeXTemp = SizeXWord ;
													do{
														Data = *( ( WORD * )DrawSrcPoint ) ;
														for(;;)
														{
															*( ( DWORD * )DrawDstPoint ) = ( Data & Table[StX] ) ? ColorData : 0 ;   		if( --SizeXTemp == 0 ) break ;
															*( ( DWORD * )DrawDstPoint + 1 ) = ( Data & Table[StX+1] ) ? ColorData : 0 ;  	if( --SizeXTemp == 0 ) break ;
															*( ( DWORD * )DrawDstPoint + 2 ) = ( Data & Table[StX+2] ) ? ColorData : 0 ;  	if( --SizeXTemp == 0 ) break ;
															*( ( DWORD * )DrawDstPoint + 3 ) = ( Data & Table[StX+3] ) ? ColorData : 0 ;  	if( --SizeXTemp == 0 ) break ;
															*( ( DWORD * )DrawDstPoint + 4 ) = ( Data & Table[StX+4] ) ? ColorData : 0 ;  	if( --SizeXTemp == 0 ) break ;
															*( ( DWORD * )DrawDstPoint + 5 ) = ( Data & Table[StX+5] ) ? ColorData : 0 ;  	if( --SizeXTemp == 0 ) break ;
															*( ( DWORD * )DrawDstPoint + 6 ) = ( Data & Table[StX+6] ) ? ColorData : 0 ;  	if( --SizeXTemp == 0 ) break ;
															*( ( DWORD * )DrawDstPoint + 7 ) = ( Data & Table[StX+7] ) ? ColorData : 0 ;  
															DrawDstPoint += 32 ;
 															--SizeXTemp ;
															break ;
														}

														DrawSrcPoint += 1 ;
													}while( SizeXTemp );

													DrawSrcPoint += SrcPitch2 ;
													DrawDstPoint += DstPitch2 ;
												}while( --SizeYWord ) ;
											}
										}
									}
									break ;

								case DX_FONTTYPE_EDGE :
									{
										WORD Width ;
										DWORD EColor ;

										EColor = ( DWORD )( EdgeColor & 0xffffff ) ;
										if( TransFlag )
										{
											do
											{
												Width = SizeXWord ;
												do
												{
													if( *DrawSrcPoint )
													{
														*( ( DWORD * )DrawDstPoint ) = *DrawSrcPoint == 1 ? ColorData : EColor ;
													}
													DrawDstPoint += 4 ;
													DrawSrcPoint ++ ;
												}while( --Width ) ;

												DrawDstPoint += DstPitch2 ;
												DrawSrcPoint += SrcPitch2 ;
											}while( --SizeYWord ) ;
										}
										else
										{
											do
											{
												Width = SizeXWord ;
												do
												{
													*( ( DWORD * )DrawDstPoint ) = *DrawSrcPoint ? ( *DrawSrcPoint == 1 ? ColorData : EColor ) : 0 ;
													DrawDstPoint += 4 ;
													DrawSrcPoint ++ ;
												}while( --Width ) ;

												DrawDstPoint += DstPitch2 ;
												DrawSrcPoint += SrcPitch2 ;
											}while( --SizeYWord ) ;
										}
									}
									break ;
								}
							}
							break ;
						}
					}
				}
			}

LOOPEND :
			// 文字情報をセット
			if( CharInfos != NULL && CharInfoBufferSize > ( size_t )CInfoNum )
			{
				float DrawX, DrawY ;
				float SizeX, SizeY ;

				CharInfos[ CInfoNum ].Char[ 0 ] = 0 ;
				CharInfos[ CInfoNum ].Char[ 1 ] = 0 ;
				CharInfos[ CInfoNum ].Char[ 2 ] = 0 ;
				CharInfos[ CInfoNum ].Bytes = ( BYTE )PutCharCode( ConvCharCode( *CharCode, WCHAR_T_CHARCODEFORMAT, _TCHARCODEFORMAT ), _TCHARCODEFORMAT, ( char * )CharInfos[ CInfoNum ].Char, sizeof( CharInfos[ CInfoNum ].Char ) ) ;

				if( PosIntFlag )
				{
					DrawX = ( float )( tmp_xi + DrawPos    ) ;
					DrawY = ( float )( tmp_yi + DrawPosSub ) ;
				}
				else
				{
					DrawX = ( float )( tmp_xf + DrawPos    ) ;
					DrawY = ( float )( tmp_yf + DrawPosSub ) ;
				}
				if( *CharCode == '\n' )
				{
					SizeX = 0.0f ;
				}
				else
				if( CharData != NULL )
				{
					SizeX = ( float )( ( CharData->AddX + UseManageData->Space ) * ExRate ) ;
				}
				else
				{
					SizeX = ( float )( UseManageData->Space * ExRate ) ;
				}
				SizeY = ( float )DrawPosSubAdd ;

				if( VerticalFlag == TRUE )
				{
					CharInfos[ CInfoNum ].DrawY = DrawX ;
					CharInfos[ CInfoNum ].DrawX = DrawY ;
					CharInfos[ CInfoNum ].SizeY = SizeX ;
					CharInfos[ CInfoNum ].SizeX = SizeY ;
				}
				else
				{
					CharInfos[ CInfoNum ].DrawX = DrawX ;
					CharInfos[ CInfoNum ].DrawY = DrawY ;
					CharInfos[ CInfoNum ].SizeX = SizeX ;
					CharInfos[ CInfoNum ].SizeY = SizeY ;
				}
			}
			CInfoNum ++ ;

			// 改行文字の場合は処理を分岐
			if( *CharCode == '\n' )
			{
				DrawPos     = 0.0 ;
				DrawPosSub += DrawPosSubAdd ;
				LFCount ++ ;
			}
			else
			{
				if( CharData != NULL )
				{
					DrawPos += ( CharData->AddX + UseManageData->Space ) * ExRate ;
				}
				else
				{
					DrawPos += UseManageData->Space * ExRate ;
				}
			}

			// 最大描画幅の更新
			if( MaxDrawPos < DrawPos )
			{
				MaxDrawPos = DrawPos ;
			}
		}
		DrawPosSub += UseManageData->BaseInfo.FontHeight * ExRate ;

		// 縁なしの場合は最後に描画輝度を元に戻す
		Graphics_DrawSetting_SetDrawBrightToOneParam( OrigColor ) ;
	}
	else
#endif // DX_NON_GRAPHICS
	{
		// 文字の数だけ繰り返し
		CharCode = UseDrawStrBuf ;			// 描画用文字データの先頭アドレスをセット
		DrawPos = 0.0 ;
		MaxDrawPos = 0.0 ;
		DrawPosSub = 0.0 ;
		for( i = 0 ; i < DrawCharNum ; i ++, CharCode ++ )
		{
			// キャッシュデータを取得
			CharData = NULL ;
			if( *CharCode != '\n' )
			{
				CharData = GetFontCacheChar_Inline( ManageData, *CharCode, &UseManageData, &DrawOffsetX, &DrawOffsetY, TRUE, FALSE ) ;
			}

			// 描画座標の準備
			if( ManageData == UseManageData )
			{
				if( PosIntFlag )
				{
					tmp_xi = xi + FontAddHeightXI ;
					tmp_yi = yi + FontAddHeightYI ;
				}
				else
				{
					tmp_xf = xf + FontAddHeightXF ;
					tmp_yf = yf + FontAddHeightYF ;
				}
			}
			else
			{
				tmp_xi = xi ;
				tmp_yi = yi ;
				if( VerticalFlag )
				{
					if( PosIntFlag )
					{
						tmp_xi += _DTOL( DrawOffsetY * ExRateY ) ;
						tmp_yi += _DTOL( DrawOffsetX * ExRateX ) ;
					}
					else
					{
						tmp_xf += ( float )( DrawOffsetY * ExRateY ) ;
						tmp_yf += ( float )( DrawOffsetX * ExRateX ) ;
					}
				}
				else
				{
					if( PosIntFlag )
					{
						tmp_xi += _DTOL( DrawOffsetX * ExRateX ) ;
						tmp_yi += _DTOL( DrawOffsetY * ExRateY ) ;
					}
					else
					{
						tmp_xf += ( float )( DrawOffsetX * ExRateX ) ;
						tmp_yf += ( float )( DrawOffsetY * ExRateY ) ;
					}
				}

				if( UseManageData->BaseInfo.FontAddHeight != 0 )
				{
					if( VerticalFlag )
					{
						if( PosIntFlag )
						{
							tmp_xi -=     _DTOL( UseManageData->BaseInfo.FontAddHeight * ExRateY ) / 2 ;
						}
						else
						{
							tmp_xf -= ( float )( UseManageData->BaseInfo.FontAddHeight * ExRateY ) / 2.0f ;
						}
					}
					else
					{
						if( PosIntFlag )
						{
							tmp_yi -=     _DTOL( UseManageData->BaseInfo.FontAddHeight * ExRateY ) / 2 ;
						}
						else
						{
							tmp_yf -= ( float )( UseManageData->BaseInfo.FontAddHeight * ExRateY ) / 2.0f ;
						}
					}
				}
			}

			// 文字情報をセット
			if( CharInfos != NULL && CharInfoBufferSize > ( size_t )CInfoNum )
			{
				float DrawX, DrawY ;
				float SizeX, SizeY ;

				CharInfos[ CInfoNum ].Char[ 0 ] = 0 ;
				CharInfos[ CInfoNum ].Char[ 1 ] = 0 ;
				CharInfos[ CInfoNum ].Char[ 2 ] = 0 ;
				CharInfos[ CInfoNum ].Bytes = ( BYTE )PutCharCode( ConvCharCode( *CharCode, WCHAR_T_CHARCODEFORMAT, _TCHARCODEFORMAT ), _TCHARCODEFORMAT, ( char * )CharInfos[ CInfoNum ].Char, sizeof( CharInfos[ CInfoNum ].Char ) ) ;

				if( PosIntFlag )
				{
					DrawX = ( float )( tmp_xi + DrawPos    ) ;
					DrawY = ( float )( tmp_yi + DrawPosSub ) ;
				}
				else
				{
					DrawX = ( float )( tmp_xf + DrawPos    ) ;
					DrawY = ( float )( tmp_yf + DrawPosSub ) ;
				}
				if( *CharCode == '\n' )
				{
					SizeX = 0.0f ;
				}
				else
				if( CharData != NULL )
				{
					SizeX = ( float )( ( CharData->AddX + UseManageData->Space ) * ExRate ) ;
				}
				else
				{
					SizeX = ( float )( UseManageData->Space * ExRate ) ;
				}
				SizeY = ( float )DrawPosSubAdd ;

				if( VerticalFlag == TRUE )
				{
					CharInfos[ CInfoNum ].DrawY = DrawX ;
					CharInfos[ CInfoNum ].DrawX = DrawY ;
					CharInfos[ CInfoNum ].SizeY = SizeX ;
					CharInfos[ CInfoNum ].SizeX = SizeY ;
				}
				else
				{
					CharInfos[ CInfoNum ].DrawX = DrawX ;
					CharInfos[ CInfoNum ].DrawY = DrawY ;
					CharInfos[ CInfoNum ].SizeX = SizeX ;
					CharInfos[ CInfoNum ].SizeY = SizeY ;
				}
			}
			CInfoNum ++ ;

			// 改行文字の場合は処理を分岐
			if( *CharCode == '\n' )
			{
				CharData    = NULL ;
				DrawPos     = 0.0 ;
				DrawPosSub += DrawPosSubAdd ;
				LFCount ++ ;
			}
			else
			{
				if( CharData != NULL )
				{
					DrawPos += ( CharData->AddX + UseManageData->Space ) * ExRate ;
				}
				else
				{
					DrawPos += UseManageData->Space * ExRate ;
				}
			}

			// 最大描画幅の更新
			if( MaxDrawPos < DrawPos )
			{
				MaxDrawPos = DrawPos ;
			}
		}
		DrawPosSub += UseManageData->BaseInfo.FontHeight * ExRate ;
	}

	if( DrawSize != NULL )
	{
		if( VerticalFlag == TRUE ) 
		{
			DrawSize->cx = _DTOL( DrawPosSub ) ;
			DrawSize->cy = _DTOL( MaxDrawPos ) ;
		}
		else
		{
			DrawSize->cx = _DTOL( MaxDrawPos ) ;
			DrawSize->cy = _DTOL( DrawPosSub ) ;
		}
	}

	if( LineCount != NULL )
	{
		*LineCount = LFCount + 1 ;
	}

	if( CharInfoNum != NULL )
	{
		*CharInfoNum = CInfoNum ;
	}

	if( UseAlloc )
	{
		DXFREE( AllocDrawStrBuf ) ;
	}

	Result = _DTOL( MaxDrawPos ) ;

END :

	// 必ずディフューズカラーを使用するようにするフラグを倒す
	GSYS.DrawSetting.AlwaysDiffuseColorFlag = FALSE ;

	// 終了
	return Result ;
//
//ERR:
//	if( UseAlloc )
//	{
//		DXFREE( AllocDrawStrBuf ) ;
//	}
//	return -1 ;
}

extern int NS_FontCacheStringDrawToHandle(
	int x, int y, const TCHAR *StrData, unsigned int Color, unsigned int EdgeColor,
	BASEIMAGE *DestImage, const RECT *ClipRect, int FontHandle,
	int VerticalFlag, SIZE *DrawSizeP )
{
#ifdef UNICODE
	return FontCacheStringDrawToHandle_WCHAR_T( x, y, StrData, -1, Color, EdgeColor, DestImage, ClipRect, FontHandle, VerticalFlag, DrawSizeP ) ;
#else
	int Result ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_BEGIN( StrData, -1 )

	Result = FontCacheStringDrawToHandle_WCHAR_T( x, y, UseStringBuffer, -1, Color, EdgeColor, DestImage, ClipRect, FontHandle, VerticalFlag, DrawSizeP ) ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_END

	return Result ;
#endif
}

extern int NS_FontCacheStringDrawToHandleWithStrLen( int x, int y, const TCHAR *StrData, size_t StrDataLength, unsigned int Color, unsigned int EdgeColor, BASEIMAGE *DestImage, const RECT *ClipRect, int FontHandle, int VerticalFlag, SIZE *DrawSizeP )
{
	int Result ;
#ifdef UNICODE
	FONTHANDLE_WCHAR_T_WITH_STRLENGTH_TO_WCHAR_T_STRING_BEGIN( StrData, StrDataLength, -1 )
	Result = FontCacheStringDrawToHandle_WCHAR_T( x, y, UseStringBuffer, ( int )StrDataLength, Color, EdgeColor, DestImage, ClipRect, FontHandle, VerticalFlag, DrawSizeP ) ;
	FONTHANDLE_WCHAR_T_TO_WCHAR_T_STRING_END
#else
	FONTHANDLE_TCHAR_WITH_STRLENGTH_TO_WCHAR_T_STRING_BEGIN( StrData, StrDataLength, -1 )
	Result = FontCacheStringDrawToHandle_WCHAR_T( x, y, UseStringBuffer, ( int )wchar_StrLength, Color, EdgeColor, DestImage, ClipRect, FontHandle, VerticalFlag, DrawSizeP ) ;
	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_END
#endif
	return Result ;
}

extern int FontCacheStringDrawToHandle_WCHAR_T(
	int x, int y, const wchar_t *StrData, int StrLen, unsigned int Color, unsigned int EdgeColor,
	BASEIMAGE *DestImage, const RECT *ClipRect, int FontHandle,
	int VerticalFlag, SIZE *DrawSizeP )
{
	static DWORD DrawStrBuf[ 2048 ] ;		// 描画する際に使用する文字列データ（３バイトで１文字を表現）
	FONTMANAGE * cmanage ;
	FONTMANAGE * usecmanage ;
	FONTCHARDATA * cache ;
	RECT SrcRect ;
	RECT DstRect ;
	int drawnum ;							// ２バイト文字１バイト文字全部ひっくるめて何文字あるか保存する変数
	int i ;									// 繰り返しと汎用変数
	const DWORD *strp ;
	LONG dpitch, spitch ;					// 描画先と描画もとのサーフェスの１ラインのバイト数
	int TotalWidth, TotalHeight ;
	int MaxTotalWidth, MaxTotalHeight ;
	int r, g, b, er, eg, eb ;
	int	UseAlloc = FALSE ;
	DWORD *AllocDrawStrBuf = NULL ;
	DWORD *UseDrawStrBuf ;
	int orig_x, orig_y ;
	int tmp_x, tmp_y ;
	int off_x, off_y ;
	int add_x = 0, add_y = 0 ;

	DEFAULT_FONT_HANDLE_SETUP

	// エラー判定
	if( FONTHCHK( FontHandle, cmanage ) )
	{
		return -1 ;
	}

	// テクスチャをキャッシュに使用しているフォントは無理
	if( cmanage->TextureCacheFlag == TRUE )
	{
		return -1 ;
	}

	// 描画座標の補正
	if( VerticalFlag )
	{
		add_x = -cmanage->BaseInfo.FontAddHeight / 2 ;
	}
	else
	{
		add_y = -cmanage->BaseInfo.FontAddHeight / 2 ;
	}

	// 初期座標を保存しておく
	orig_x = x ;
	orig_y = y ;

	// 色カラー値を取得しておく
	NS_GetColor5( &DestImage->ColorData, Color, &r, &g, &b ) ;
	NS_GetColor5( &DestImage->ColorData, EdgeColor, &er, &eg, &eb ) ;

	// ＵＮＩコードかどうかを取得しておく
//	Hangeul = cmanage->BaseInfo.CharSet == DX_CHARSET_HANGEUL ;

	// 文字列がない場合は終了
	if( StrData  == NULL || *StrData  == 0 )
	{
		return 0 ;
	}

	// 描画する文字列の長さを保存
	{
		if( StrLen < 0 )
		{
			StrLen = ( int )_WCSLEN( StrData ) ;
		}

		if( StrLen > 1023 )
		{
			UseAlloc = TRUE ;
			AllocDrawStrBuf = ( DWORD * )DXALLOC( StrLen * sizeof( DWORD ) ) ;
			if( AllocDrawStrBuf == NULL )
			{
				return -1 ;
			}
			UseDrawStrBuf = AllocDrawStrBuf ;
		}
		else
		{
			UseDrawStrBuf = DrawStrBuf ;
		}
	}

	// キャッシュに入っていない文字列をキャッシュに入れる
	FontCacheStringAddToHandle( cmanage, StrData, -1, UseDrawStrBuf, &drawnum ) ;

	// ピッチをセット
	dpitch = DestImage->Pitch ;

	// 文字の数だけ繰り返し
	strp			= UseDrawStrBuf ;			// 描画用文字データの先頭アドレスをセット
	TotalWidth		= 0 ;
	TotalHeight		= 0 ;
	MaxTotalWidth	= 0 ;
	MaxTotalHeight	= 0 ;
	for( i = 0 ; i < drawnum ; i ++, strp ++ )
	{
		cache = NULL ;
		usecmanage = cmanage ;

		// 改行文字だった場合はキャンセルする
		if( *strp == L'\n' )
		{
			goto LOOPEND ;
		}

		// キャッシュデータを取得
		cache = GetFontCacheChar_Inline( cmanage, *strp, &usecmanage, &off_x, &off_y, FALSE, FALSE ) ;
		if( cache == NULL )
		{
			return -1 ;
		}

		// 描画座標の準備
		if( cmanage == usecmanage )
		{
			tmp_x = x + add_x ;
			tmp_y = y + add_y ;
		}
		else
		{
			tmp_x = x ;
			tmp_y = y ;
			if( VerticalFlag )
			{
				tmp_x += off_y ;
				tmp_y += off_x ;
			}
			else
			{
				tmp_x += off_x ;
				tmp_y += off_y ;
			}

			if( usecmanage->BaseInfo.FontAddHeight != 0 )
			{
				if( VerticalFlag )
				{
					tmp_x -= usecmanage->BaseInfo.FontAddHeight / 2 ;
				}
				else
				{
					tmp_y -= usecmanage->BaseInfo.FontAddHeight / 2 ;
				}
			}
		}

		// スペース文字だった場合はキャンセルする
		if( *strp == L' ' || ( wchar_t )*strp == FSYS.DoubleByteSpaceCharCode )
		{
			goto LOOPEND ;
		}

		// サイズが無かったら何もしない
		if( cache->SizeX == 0 && cache->SizeY == 0 )
		{
			goto LOOPEND ;
		}

		// ピッチをセット
		spitch = usecmanage->CachePitch ;

		// 転送先の描画領域を作成
		DstRect.left	= tmp_x + cache->DrawX ;
		DstRect.top		= tmp_y + cache->DrawY ;
		DstRect.right	= tmp_x + cache->DrawX + cache->SizeX ;
		DstRect.bottom	= tmp_y + cache->DrawY + cache->SizeY ;
		if( ClipRect == NULL )
		{
			RECT BaseRect ;
			BaseRect.left = 0 ;
			BaseRect.top = 0 ;
			BaseRect.right = DestImage->Width ;
			BaseRect.bottom = DestImage->Height ;
			RectClipping_Inline( &DstRect, &BaseRect ) ;
		}
		else
		{
			RectClipping_Inline( &DstRect, ClipRect ) ;
		}

		// 描画する文字が無かった場合ここで次ループに移る
		if( DstRect.right == DstRect.left || DstRect.bottom == DstRect.top )
		{
			goto LOOPEND ;
		}

		// 転送元の矩形をセット
		SrcRect.left	=   cache->GraphIndex / usecmanage->LengthCharNum   * usecmanage->BaseInfo.MaxWidth ;
		SrcRect.top		= ( cache->GraphIndex % usecmanage->LengthCharNum ) * usecmanage->BaseInfo.MaxWidth ;
		if( usecmanage->TextureCacheFlag == TRUE )
		{
			SrcRect.left ++ ;
			SrcRect.top ++ ;
		}
		SrcRect.right	= SrcRect.left + cache->SizeX ;
		SrcRect.bottom	= SrcRect.top + cache->SizeY ;

		// 転送元の矩形を補正
		SrcRect.left   += DstRect.left - ( tmp_x + cache->DrawX ) ;
		SrcRect.top    += DstRect.top  - ( tmp_y + cache->DrawY ) ;
		SrcRect.right  -= ( ( tmp_x + cache->DrawX ) + cache->SizeX ) - DstRect.right ;
		SrcRect.bottom -= ( ( tmp_y + cache->DrawY ) + cache->SizeY ) - DstRect.bottom ;

		// 転送
		{
			DWORD spitch2, dpitch2 ;
			DWORD SizeX, SizeY ;
			WORD SizeXWord, SizeYWord ;

			SizeX = ( DWORD )( SrcRect.right - SrcRect.left ) ;
			SizeY = ( DWORD )( SrcRect.bottom - SrcRect.top ) ;
			SizeXWord = ( WORD )SizeX ;
			SizeYWord = ( WORD )SizeY ;

			// 色のビット深度によって処理を分岐
			switch( DestImage->ColorData.ColorBitDepth )
			{
			case 8 :
				// 8ビットモードの時の処理
				{
					BYTE *dest = ( BYTE * )DestImage->GraphData ;
					BYTE *src = usecmanage->CacheMem ;
					BYTE cr = ( BYTE )( Color & 0xff ) ;

					dest += ( DstRect.left ) + DstRect.top * dpitch ;
					src += ( SrcRect.left ) + SrcRect.top * spitch ;

					spitch2 = spitch - SizeX ;
					dpitch2 = dpitch - SizeX ;

					switch( usecmanage->FontType )
					{
					case DX_FONTTYPE_NORMAL :
						{
							DWORD wtemp ;
							WORD dat ;
							DWORD WordNum, NokoriNum ;

							if( SrcRect.left % 8 == 0 )
							{
								src = usecmanage->CacheMem ;
								src += ( SrcRect.left / 8 ) + SrcRect.top * spitch ;
								spitch2 = spitch - ( ( SizeX / 16 ) * 2 + ( SizeX % 16 != 0 ? 2 : 0 ) ) ;
								dpitch2 = dpitch - ( ( ( SizeX >> 4 ) << 4 ) ) ;
								WordNum = ( DWORD )( SizeXWord / 16 ) ;
								NokoriNum = SizeXWord - WordNum * 16 ;

								do
								{
									if( WordNum != 0 )
									{
										wtemp = WordNum ;
										do
										{
											dat = *(( WORD *)src) ;
											if( dat & 0x80 ) dest[0] = cr ;
											if( dat & 0x40 ) dest[1] = cr ;
											if( dat & 0x20 ) dest[2] = cr ;
											if( dat & 0x10 ) dest[3] = cr ;
											if( dat & 0x8 ) dest[4] = cr ;
											if( dat & 0x4 ) dest[5] = cr ;
											if( dat & 0x2 ) dest[6] = cr ;
											if( dat & 0x1 ) dest[7] = cr ;
											if( dat & 0x8000 ) dest[8] = cr ;
											if( dat & 0x4000 ) dest[9] = cr ;
											if( dat & 0x2000 ) dest[10] = cr ;
											if( dat & 0x1000 ) dest[11] = cr ;
											if( dat & 0x800 ) dest[12] = cr ;
											if( dat & 0x400 ) dest[13] = cr ;
											if( dat & 0x200 ) dest[14] = cr ;
											if( dat & 0x100 ) dest[15] = cr ;
											dest += 16 ;
											src += 2 ;
										}while( --wtemp ) ;
									}

									if( NokoriNum != 0 )
									{
										wtemp = NokoriNum ;
										do
										{
											dat = *(( WORD *)src ) ;
											for(;;)
											{
												if( dat & 0x80 ) dest[0] = cr ;
 												if( --wtemp == 0 ) break ;
												if( dat & 0x40 ) dest[1] = cr ;
 												if( --wtemp == 0 ) break ;
												if( dat & 0x20 ) dest[2] = cr ;
 												if( --wtemp == 0 ) break ;
												if( dat & 0x10 ) dest[3] = cr ;
 												if( --wtemp == 0 ) break ;
												if( dat & 0x8 ) dest[4] = cr ;
 												if( --wtemp == 0 ) break ;
												if( dat & 0x4 ) dest[5] = cr ;
 												if( --wtemp == 0 ) break ;
												if( dat & 0x2 ) dest[6] = cr ;
 												if( --wtemp == 0 ) break ;
												if( dat & 0x1 ) dest[7] = cr ;
 												if( --wtemp == 0 ) break ;
												if( dat & 0x8000 ) dest[8] = cr ;
 												if( --wtemp == 0 ) break ;
												if( dat & 0x4000 ) dest[9] = cr ;
 												if( --wtemp == 0 ) break ;
												if( dat & 0x2000 ) dest[10] = cr ;
 												if( --wtemp == 0 ) break ;
												if( dat & 0x1000 ) dest[11] = cr ;
 												if( --wtemp == 0 ) break ;
												if( dat & 0x800 ) dest[12] = cr ;
 												if( --wtemp == 0 ) break ;
												if( dat & 0x400 ) dest[13] = cr ;
 												if( --wtemp == 0 ) break ;
												if( dat & 0x200 ) dest[14] = cr ;
 												if( --wtemp == 0 ) break ;
												if( dat & 0x100 ) dest[15] = cr ;
												dest += 16 ;
 												--wtemp ;
												break ;
											}

											src += 2 ;
										}while( wtemp );
									}

									src += spitch2 ;
									dest += dpitch2 ;
								}while( --SizeYWord ) ;
/*
								do{
									wtemp = SizeXWord ;
									do{
										dat = *(( WORD *)src) ;
										do{
											if( dat & 0x80 ) dest[0] = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x40 ) dest[1] = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x20 ) dest[2] = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x10 ) dest[3] = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x8 ) dest[4] = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x4 ) dest[5] = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x2 ) dest[6] = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x1 ) dest[7] = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x8000 ) dest[8] = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x4000 ) dest[9] = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x2000 ) dest[10] = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x1000 ) dest[11] = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x800 ) dest[12] = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x400 ) dest[13] = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x200 ) dest[14] = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x100 ) dest[15] = cr ;
											dest += 16 ;
 											--wtemp ;
										}while(0);

										src += 2 ;
									}while( wtemp );

									src += spitch2 ;
									dest += dpitch2 ;
								}while( --SizeYWord ) ;
*/							}
							else
							{
								int StX = SrcRect.left % 8 ;
								WORD Table[16] = { 0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1, 0x8000, 0x4000, 0x2000, 0x1000, 0x800, 0x400, 0x200, 0x100 } ;

								src = usecmanage->CacheMem ;
								src += ( SrcRect.left / 8 ) + SrcRect.top * spitch ;
								spitch2 = spitch - ( ( SizeX / 8 ) + ( SizeX % 8 != 0 ? 1 : 0 ) ) ;
								dpitch2 = dpitch - ( ( ( SizeX >> 3 ) << 3 ) ) ;

								do{
									wtemp = SizeXWord ;
									do{
										dat = *(( WORD *)src) ;
										for(;;)
										{
											if( dat & Table[StX] ) dest[0] = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & Table[StX+1] ) dest[1] = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & Table[StX+2] ) dest[2] = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & Table[StX+3] ) dest[3] = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & Table[StX+4] ) dest[4] = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & Table[StX+5] ) dest[5] = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & Table[StX+6] ) dest[6] = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & Table[StX+7] ) dest[7] = cr ;
											dest += 8 ;
 											--wtemp ;
											break ;
										}

										src += 1 ;
									}while( wtemp );

									src += spitch2 ;
									dest += dpitch2 ;
								}while( --SizeYWord ) ;
							}
						}
						break ;


					case DX_FONTTYPE_EDGE :
						{
							WORD wtemp ;
							BYTE ecr ;

							ecr = (BYTE)( EdgeColor & 0xff ) ;
							do
							{
								wtemp = SizeXWord ;
								do
								{
									if( *src ) *dest = *src == 1 ? cr : ecr ;

									dest ++ ;
									src ++ ;
								}while( --wtemp ) ;

								dest += dpitch2 ;
								src += spitch2 ;
							}while( --SizeYWord ) ;
						}
						break ;
					}
				}
				break ;

			case 16 :
				// 16ビットモードの時の処理
				{
					BYTE *dest = ( BYTE * )DestImage->GraphData ;
					BYTE *src = usecmanage->CacheMem ;
					WORD cr = (WORD)( Color & 0xffff ), ecr = (WORD)( EdgeColor & 0xffff );

					dest += ( DstRect.left << 1 ) + DstRect.top * dpitch ;
					src += SrcRect.left + SrcRect.top * spitch ;

					spitch2 = spitch - SizeX ;
					dpitch2 = dpitch - ( SizeX << 1 ) ;

					switch( usecmanage->FontType )
					{
					case DX_FONTTYPE_ANTIALIASING_EDGE :
					case DX_FONTTYPE_ANTIALIASING_EDGE_4X4 :
					case DX_FONTTYPE_ANTIALIASING_EDGE_8X8 :
						{
							BYTE r1, g1, b1 ;
							BYTE r2, g2, b2 ;
							BYTE r3, g3, b3 ;
							DWORD AAA, AAA2 ;
							WORD wtemp ;

							src = usecmanage->CacheMem + ( SrcRect.left << 1 ) + SrcRect.top * spitch ;
							spitch2 = spitch - ( SizeX << 1 ) ;

							if( ( DestImage->ColorData.RedMask & MEMIMG_R5G6B5_R ) &&
								( DestImage->ColorData.GreenMask & MEMIMG_R5G6B5_G ) &&
								( DestImage->ColorData.BlueMask & MEMIMG_R5G6B5_B ) )
							{
								r1 = ( BYTE )( r >> 3 ) ;
								g1 = ( BYTE )( g >> 2 ) ;
								b1 = ( BYTE )( b >> 3 ) ;

								r2 = ( BYTE )( er >> 3 ) ;
								g2 = ( BYTE )( eg >> 2 ) ;
								b2 = ( BYTE )( eb >> 3 ) ;

								do
								{
									wtemp = SizeXWord ;
									do
									{
										if( src[0] )
										{
											if( src[0] == 255 )
											{
												*((WORD *)dest) = cr ;
											}
											else
											{
												r3 = (BYTE)( (*((WORD *)dest) & MEMIMG_R5G6B5_R) >> MEMIMG_R5G6B5_LR );
												g3 = (BYTE)( (*((WORD *)dest) & MEMIMG_R5G6B5_G) >> MEMIMG_R5G6B5_LG );
												b3 = (BYTE)( (*((WORD *)dest) & MEMIMG_R5G6B5_B) >> MEMIMG_R5G6B5_LB );

												AAA2 = ( DWORD )( src[0] + (((256 - src[0]) * src[1]) >> 8) ) ;
												AAA  = ((WORD)src[0] << 8) / AAA2 ;
												*((WORD *)dest) = ( WORD )( 
													(((((((((r1 - r2) * AAA) >> 8) + r2) - r3) * AAA2) >> 8) + r3) << MEMIMG_R5G6B5_LR ) | 
													(((((((((g1 - g2) * AAA) >> 8) + g2) - g3) * AAA2) >> 8) + g3) << MEMIMG_R5G6B5_LG ) | 
													(((((((((b1 - b2) * AAA) >> 8) + b2) - b3) * AAA2) >> 8) + b3) << MEMIMG_R5G6B5_LB ) ) ;
											}
										}
										else
										if( src[1] )
										{
											if( src[1] == 255 )
											{
												*((WORD *)dest) = ecr ;
											}
											else
											{
												r3 = (BYTE)( (*((WORD *)dest) & MEMIMG_R5G6B5_R) >> MEMIMG_R5G6B5_LR );
												g3 = (BYTE)( (*((WORD *)dest) & MEMIMG_R5G6B5_G) >> MEMIMG_R5G6B5_LG );
												b3 = (BYTE)( (*((WORD *)dest) & MEMIMG_R5G6B5_B) >> MEMIMG_R5G6B5_LB );

												*((WORD *)dest) = ( WORD )( 
													(((((r2 - r3) * src[1]) >> 8) + r3) << MEMIMG_R5G6B5_LR ) | 
													(((((g2 - g3) * src[1]) >> 8) + g3) << MEMIMG_R5G6B5_LG ) | 
													(((((b2 - b3) * src[1]) >> 8) + b3) << MEMIMG_R5G6B5_LB ) ) ;
											}
										}

										dest += 2 ;
										src += 2 ;
									}while( --wtemp ) ;

									dest += dpitch2 ;
									src += spitch2 ;
								}while( --SizeYWord ) ;
							}
							else
							if( ( DestImage->ColorData.RedMask & MEMIMG_RGB5_R ) &&
								( DestImage->ColorData.GreenMask & MEMIMG_RGB5_G ) &&
								( DestImage->ColorData.BlueMask & MEMIMG_RGB5_B ) )
							{
								r1 = ( BYTE )( r >> 3 ) ;
								g1 = ( BYTE )( g >> 3 ) ;
								b1 = ( BYTE )( b >> 3 ) ;

								r2 = ( BYTE )( er >> 3 ) ;
								g2 = ( BYTE )( eg >> 3 ) ;
								b2 = ( BYTE )( eb >> 3 ) ;

								do
								{
									wtemp = SizeXWord ;
									do
									{
										if( src[0] )
										{
											if( src[0] == 255 )
											{
												*((WORD *)dest) = cr ;
											}
											else
											{
												r3 = (BYTE)( (*((WORD *)dest) & MEMIMG_RGB5_R) >> MEMIMG_RGB5_LR );
												g3 = (BYTE)( (*((WORD *)dest) & MEMIMG_RGB5_G) >> MEMIMG_RGB5_LG );
												b3 = (BYTE)( (*((WORD *)dest) & MEMIMG_RGB5_B) >> MEMIMG_RGB5_LB );

												AAA2 = ( DWORD )( src[0] + (((256 - src[0]) * src[1]) >> 8) ) ;
												AAA  = ((WORD)src[0] << 8) / AAA2 ;
												*((WORD *)dest) = ( WORD )( 
													(((((((((r1 - r2) * AAA) >> 8) + r2) - r3) * AAA2) >> 8) + r3) << MEMIMG_RGB5_LR ) | 
													(((((((((g1 - g2) * AAA) >> 8) + g2) - g3) * AAA2) >> 8) + g3) << MEMIMG_RGB5_LG ) | 
													(((((((((b1 - b2) * AAA) >> 8) + b2) - b3) * AAA2) >> 8) + b3) << MEMIMG_RGB5_LB ) ) ;
											}
										}
										else
										if( src[1] )
										{
											if( src[1] == 255 )
											{
												*((WORD *)dest) = ecr ;
											}
											else
											{
												r3 = (BYTE)( (*((WORD *)dest) & MEMIMG_RGB5_R) >> MEMIMG_RGB5_LR );
												g3 = (BYTE)( (*((WORD *)dest) & MEMIMG_RGB5_G) >> MEMIMG_RGB5_LG );
												b3 = (BYTE)( (*((WORD *)dest) & MEMIMG_RGB5_B) >> MEMIMG_RGB5_LB );

												*((WORD *)dest) = ( WORD )( 
													(((((r2 - r3) * src[1]) >> 8) + r3) << MEMIMG_RGB5_LR ) | 
													(((((g2 - g3) * src[1]) >> 8) + g3) << MEMIMG_RGB5_LG ) | 
													(((((b2 - b3) * src[1]) >> 8) + b3) << MEMIMG_RGB5_LB ) ) ;
											}
										}

										dest += 2 ;
										src += 2 ;
									}while( --wtemp ) ;

									dest += dpitch2 ;
									src += spitch2 ;
								}while( --SizeYWord ) ;
							}
						}
						break ;

					case DX_FONTTYPE_ANTIALIASING :
					case DX_FONTTYPE_ANTIALIASING_4X4 :
					case DX_FONTTYPE_ANTIALIASING_8X8 :
						{
							WORD wtemp ;
							BYTE r1, g1, b1 ;
							BYTE r3, g3, b3 ;

							src = usecmanage->CacheMem + ( SrcRect.left << 1 ) + SrcRect.top * spitch ;
							spitch2 = spitch - ( SizeX << 1 ) ;

							if( ( DestImage->ColorData.RedMask & MEMIMG_R5G6B5_R ) &&
								( DestImage->ColorData.GreenMask & MEMIMG_R5G6B5_G ) &&
								( DestImage->ColorData.BlueMask & MEMIMG_R5G6B5_B ) )
							{
								r1 = ( BYTE )( r >> 3 ) ;
								g1 = ( BYTE )( g >> 2 ) ;
								b1 = ( BYTE )( b >> 3 ) ;

								do
								{
									wtemp = SizeXWord ;
									do
									{
										if( src[0] )
										{
											if( src[0] == 255 )
											{
												*((WORD *)dest) = cr ;
											}
											else
											{
												r3 = (BYTE)( (*((WORD *)dest) & MEMIMG_R5G6B5_R) >> MEMIMG_R5G6B5_LR );
												g3 = (BYTE)( (*((WORD *)dest) & MEMIMG_R5G6B5_G) >> MEMIMG_R5G6B5_LG );
												b3 = (BYTE)( (*((WORD *)dest) & MEMIMG_R5G6B5_B) >> MEMIMG_R5G6B5_LB );

												*((WORD *)dest) = ( WORD )( 
													(((((r1 - r3) * src[0]) >> 8) + r3) << MEMIMG_R5G6B5_LR ) | 
													(((((g1 - g3) * src[0]) >> 8) + g3) << MEMIMG_R5G6B5_LG ) | 
													(((((b1 - b3) * src[0]) >> 8) + b3) << MEMIMG_R5G6B5_LB ) ) ;
											}
										}
										dest += 2 ;
										src ++ ;
									}while( --wtemp ) ;

									dest += dpitch2 ;
									src += spitch2 ;
								}while( --SizeYWord ) ;
							}
							else
							if( ( DestImage->ColorData.RedMask & MEMIMG_RGB5_R ) &&
								( DestImage->ColorData.GreenMask & MEMIMG_RGB5_G ) &&
								( DestImage->ColorData.BlueMask & MEMIMG_RGB5_B ) )
							{
								r1 = ( BYTE )( r >> 3 ) ;
								g1 = ( BYTE )( g >> 3 ) ;
								b1 = ( BYTE )( b >> 3 ) ;

								do
								{
									wtemp = SizeXWord ;
									do
									{
										if( src[0] )
										{
											if( src[0] == 255 )
											{
												*((WORD *)dest) = cr ;
											}
											else
											{
												r3 = (BYTE)( (*((WORD *)dest) & MEMIMG_RGB5_R) >> MEMIMG_RGB5_LR );
												g3 = (BYTE)( (*((WORD *)dest) & MEMIMG_RGB5_G) >> MEMIMG_RGB5_LG );
												b3 = (BYTE)( (*((WORD *)dest) & MEMIMG_RGB5_B) >> MEMIMG_RGB5_LB );

												*((WORD *)dest) = ( WORD )( 
													(((((r1 - r3) * src[0]) >> 8) + r3) << MEMIMG_RGB5_LR ) | 
													(((((g1 - g3) * src[0]) >> 8) + g3) << MEMIMG_RGB5_LG ) | 
													(((((b1 - b3) * src[0]) >> 8) + b3) << MEMIMG_RGB5_LB ) ) ;
											}
										}
										dest += 2 ;
										src ++ ;
									}while( --wtemp ) ;

									dest += dpitch2 ;
									src += spitch2 ;
								}while( --SizeYWord ) ;
							}
						}
						break ;

					case DX_FONTTYPE_NORMAL :
						{
							DWORD wtemp ;
							WORD dat ;

							if( SrcRect.left % 8 == 0 )
							{
								src = usecmanage->CacheMem ;
								src += ( SrcRect.left / 8 ) + SrcRect.top * spitch ;
								spitch2 = spitch - ( ( SizeX / 16 ) * 2 + ( SizeX % 16 != 0 ? 2 : 0 ) ) ;
								dpitch2 = dpitch - ( ( ( SizeX >> 4 ) << 5 ) ) ;

								do{
									wtemp = SizeXWord ;
									do{
										dat = *(( WORD *)src) ;
										for(;;)
										{
											if( dat & 0x80 ) *( ( WORD * )dest ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x40 ) *( ( WORD * )dest + 1 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x20 ) *( ( WORD * )dest + 2 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x10 ) *( ( WORD * )dest + 3 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x8 ) *( ( WORD * )dest + 4 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x4 ) *( ( WORD * )dest + 5 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x2 ) *( ( WORD * )dest + 6 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x1 ) *( ( WORD * )dest + 7 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x8000 ) *( ( WORD * )dest + 8 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x4000 ) *( ( WORD * )dest + 9 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x2000 ) *( ( WORD * )dest + 10 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x1000 ) *( ( WORD * )dest + 11 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x800 ) *( ( WORD * )dest + 12 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x400 ) *( ( WORD * )dest + 13 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x200 ) *( ( WORD * )dest + 14 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x100 ) *( ( WORD * )dest + 15 ) = cr ;
											dest += 32 ;
 											--wtemp ;
											break ;
										}

										src += 2 ;
									}while( wtemp );

									src += spitch2 ;
									dest += dpitch2 ;
								}while( --SizeYWord ) ;
							}
							else
							{
								int StX = SrcRect.left % 8 ;
								WORD Table[16] = { 0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1, 0x8000, 0x4000, 0x2000, 0x1000, 0x800, 0x400, 0x200, 0x100 } ;
								src = usecmanage->CacheMem ;
								src += ( SrcRect.left / 8 ) + SrcRect.top * spitch ;
								spitch2 = spitch - ( ( SizeX / 8 ) + ( SizeX % 8 != 0 ? 1 : 0 ) ) ;
								dpitch2 = dpitch - ( ( ( SizeX >> 3 ) << 4 ) ) ;

								do{
									wtemp = SizeXWord ;
									do{
										dat = *( ( WORD * )src ) ;
										for(;;)
										{
											if( dat & Table[StX] ) *( ( WORD * )dest ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & Table[StX+1] ) *( ( WORD * )dest + 1 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & Table[StX+2] ) *( ( WORD * )dest + 2 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & Table[StX+3] ) *( ( WORD * )dest + 3 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & Table[StX+4] ) *( ( WORD * )dest + 4 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & Table[StX+5] ) *( ( WORD * )dest + 5 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & Table[StX+6] ) *( ( WORD * )dest + 6 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & Table[StX+7] ) *( ( WORD * )dest + 7 ) = cr ;
 											--wtemp ;
											dest += 16 ;
											break ;
										}

										src += 1 ;
									}while( wtemp );

									src += spitch2 ;
									dest += dpitch2 ;
								}while( --SizeYWord ) ;
							}
						}
						break ;

					case DX_FONTTYPE_EDGE :
						{
							WORD wtemp ;
							WORD ecrW ;

							ecrW = (WORD)( EdgeColor & 0xffff ) ;
							do
							{
								wtemp = SizeXWord ;
								do
								{
									if( *src ) *((WORD *)dest) = (*src & 2) ? cr : ecrW ;
									dest += 2 ;
									src ++ ;
								}while( --wtemp ) ;

								dest += dpitch2 ;
								src += spitch2 ;
							}while( --SizeYWord ) ;
						}
						break ;
					}
				}
				break ;

			case 32 :
				// 32ビットモードの場合
				{
					BYTE *dest = (BYTE *)DestImage->GraphData ;
					BYTE *src = usecmanage->CacheMem ;
					DWORD cr = ( DWORD )Color, ecr = (DWORD)EdgeColor ;

					dest += ( DstRect.left << 2 ) + DstRect.top * dpitch ;
					src += ( SrcRect.left ) + SrcRect.top * spitch ;

					spitch2 = spitch - SizeX ;
					dpitch2 = dpitch - ( SizeX << 2 ) ;

					switch( usecmanage->FontType )
					{
					case DX_FONTTYPE_ANTIALIASING_EDGE :
					case DX_FONTTYPE_ANTIALIASING_EDGE_4X4 :
					case DX_FONTTYPE_ANTIALIASING_EDGE_8X8 :
						{
							BYTE r1, g1, b1 ;
							BYTE r2, g2, b2 ;
							DWORD AAA, AAA2 ;
							WORD wtemp ;

							spitch2 = spitch - ( SizeX << 1 ) ;
							src = usecmanage->CacheMem + ( SrcRect.left << 1 ) + SrcRect.top * spitch ;

							r1 = ( BYTE )r;
							g1 = ( BYTE )g;
							b1 = ( BYTE )b;

							r2 = ( BYTE )er;
							g2 = ( BYTE )eg;
							b2 = ( BYTE )eb;

							do
							{
								wtemp = SizeXWord ;
								do
								{
									if( src[0] )
									{
										if( src[0] == 255 )
										{
											*((DWORD *)dest) = cr ;
										}
										else
										{
											AAA2 = ( DWORD )( src[0] + (((256 - src[0]) * src[1]) >> 8 ) ) ;
											AAA  = ((WORD)src[0] << 8) / AAA2 ;
											*((DWORD *)dest) = ( DWORD )( 
												(((((((((r1 - r2) * AAA) >> 8) + r2) - dest[2]) * AAA2) >> 8) + dest[2]) << MEMIMG_XRGB8_LR ) | 
												(((((((((g1 - g2) * AAA) >> 8) + g2) - dest[1]) * AAA2) >> 8) + dest[1]) << MEMIMG_XRGB8_LG ) | 
												(((((((((b1 - b2) * AAA) >> 8) + b2) - dest[0]) * AAA2) >> 8) + dest[0]) << MEMIMG_XRGB8_LB ) ) ;
										}
									}
									else
									if( src[1] )
									{
										if( src[1] == 255 )
										{
											*((DWORD *)dest) = ecr ;
										}
										else
										{
											*((DWORD *)dest) = ( DWORD )( 
												(((((r2 - dest[2]) * src[1]) >> 8) + dest[2]) << MEMIMG_XRGB8_LR ) | 
												(((((g2 - dest[1]) * src[1]) >> 8) + dest[1]) << MEMIMG_XRGB8_LG ) | 
												(((((b2 - dest[0]) * src[1]) >> 8) + dest[0]) << MEMIMG_XRGB8_LB ) ) ;
										}
									}

									dest += 4 ;
									src += 2 ;
								}while( --wtemp ) ;

								dest += dpitch2 ;
								src += spitch2 ;
							}while( --SizeYWord ) ;
						}
						break ;


					case DX_FONTTYPE_ANTIALIASING :
					case DX_FONTTYPE_ANTIALIASING_4X4 :
					case DX_FONTTYPE_ANTIALIASING_8X8 :
						{
							BYTE r1, g1, b1 ;
							WORD wtemp ;

							r1 = ( BYTE )r;
							g1 = ( BYTE )g;
							b1 = ( BYTE )b;
							do
							{
								wtemp = SizeXWord ;
								do
								{
									if( src[0] == 255 )
									{
										*((DWORD *)dest) = ecr ;
									}
									else
									{
										*((DWORD *)dest) = ( DWORD )( 
											(((((r1 - dest[2]) * src[1]) >> 8) + dest[2]) << MEMIMG_XRGB8_LR ) | 
											(((((g1 - dest[1]) * src[1]) >> 8) + dest[1]) << MEMIMG_XRGB8_LG ) | 
											(((((b1 - dest[0]) * src[1]) >> 8) + dest[0]) << MEMIMG_XRGB8_LB ) ) ;
									}

									dest += 4 ;
									src ++ ;
								}while( --wtemp ) ;

								dest += dpitch2 ;
								src += spitch2 ;
							}while( --SizeYWord ) ;
						}
						break ;

					case DX_FONTTYPE_NORMAL :
						{
							WORD wtemp ;
							WORD dat ;

							if( SrcRect.left % 8 == 0 )
							{
								src = usecmanage->CacheMem ;
								src += ( SrcRect.left / 8 ) + SrcRect.top * spitch ;
								spitch2 = spitch - ( ( SizeX / 16 ) * 2 + ( SizeX % 16 != 0 ? 2 : 0 ) ) ;
								dpitch2 = dpitch - ( ( ( SizeX >> 4 ) << 6 ) ) ;

								do{
									wtemp = SizeXWord ;
									do{
										dat = *( ( WORD * )src ) ;
										for(;;)
										{
											if( dat & 0x80 ) *( ( DWORD * )dest ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x40 ) *( ( DWORD * )dest + 1 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x20 ) *( ( DWORD * )dest + 2 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x10 ) *( ( DWORD * )dest + 3 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x8 ) *( ( DWORD * )dest + 4 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x4 ) *( ( DWORD * )dest + 5 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x2 ) *( ( DWORD * )dest + 6 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x1 ) *( ( DWORD * )dest + 7 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x8000 ) *( ( DWORD * )dest + 8 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x4000 ) *( ( DWORD * )dest + 9 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x2000 ) *( ( DWORD * )dest + 10 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x1000 ) *( ( DWORD * )dest + 11 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x800 ) *( ( DWORD * )dest + 12 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x400 ) *( ( DWORD * )dest + 13 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x200 ) *( ( DWORD * )dest + 14 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & 0x100 ) *( ( DWORD * )dest + 15 ) = cr ;
											dest += 64 ;
 											--wtemp ;
											break ;
										}

										src += 2 ;
									}while( wtemp );

									src += spitch2 ;
									dest += dpitch2 ;
								}while( --SizeYWord ) ;
							}
							else
							{
								int StX = SrcRect.left % 8 ;
								WORD Table[16] = { 0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1, 0x8000, 0x4000, 0x2000, 0x1000, 0x800, 0x400, 0x200, 0x100 } ;

								src = usecmanage->CacheMem ;
								src += ( SrcRect.left / 8 ) + SrcRect.top * spitch ;
								spitch2 = spitch - ( ( SizeX / 8 ) + ( SizeX % 8 != 0 ? 1 : 0 ) ) ;
								dpitch2 = dpitch - ( ( ( SizeX >> 3 ) << 5 ) ) ;

								do{
									wtemp = SizeXWord ;
									do{
										dat = *( ( WORD * )src ) ;
										for(;;)
										{
											if( dat & Table[StX] ) *( ( DWORD * )dest ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & Table[StX+1] ) *( ( DWORD * )dest + 1 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & Table[StX+2] ) *( ( DWORD * )dest + 2 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & Table[StX+3] ) *( ( DWORD * )dest + 3 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & Table[StX+4] ) *( ( DWORD * )dest + 4 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & Table[StX+5] ) *( ( DWORD * )dest + 5 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & Table[StX+6] ) *( ( DWORD * )dest + 6 ) = cr ;
 											if( --wtemp == 0 ) break ;
											if( dat & Table[StX+7] ) *( ( DWORD * )dest + 7 ) = cr ;
											dest += 32 ;
 											--wtemp ;
											break ;
										}

										src += 1 ;
									}while( wtemp );

									src += spitch2 ;
									dest += dpitch2 ;
								}while( --SizeYWord ) ;
							}
						}
						break ;

					case DX_FONTTYPE_EDGE :
						{
							WORD wtemp ;
							DWORD ecrD ;
							BYTE dat ;

							ecrD = ( DWORD )( EdgeColor & 0xffffff ) ;
							do
							{
								wtemp = SizeXWord ;
								do
								{
									dat = *src ;
									if( dat )
									{
										*( ( DWORD * )dest ) = ( dat & 2 ) ? cr : ecrD ;
									}
									dest += 4 ;
									src ++ ;
								}while( --wtemp ) ;

								dest += dpitch2 ;
								src += spitch2 ;
							}while( --SizeYWord ) ;
						}
						break ;
					}
				}
				break ;
			}
		}

LOOPEND :

		// 改行文字の場合は処理を分岐
		if( *strp == '\n' )
		{
			int LineSpace = usecmanage->LineSpaceValidFlag ? usecmanage->LineSpace : usecmanage->BaseInfo.FontHeight ;

			if( VerticalFlag == TRUE )
			{
				x           -= LineSpace ;
				TotalWidth  += LineSpace ;
				y           = orig_y ;
				TotalHeight = 0 ;
			}
			else
			{
				y           += LineSpace ;
				TotalHeight += LineSpace ;
				x          = orig_x ;
				TotalWidth = 0 ;
			}
		}
		else
		{
			if( cache == NULL )
			{
				if( VerticalFlag )
				{
					TotalHeight += usecmanage->Space ;
					y           += usecmanage->Space ;
				}
				else
				{
					TotalWidth  += usecmanage->Space ;
					x           += usecmanage->Space ;
				}
			}
			else
			{
				if( VerticalFlag )
				{
					TotalHeight += cache->AddX + usecmanage->Space ;
					if( TotalWidth < cache->DrawX + cache->SizeX )
					{
						TotalWidth = cache->DrawX + cache->SizeX ;
					}
					y += cache->AddX + usecmanage->Space ;
				}
				else
				{
					TotalWidth += cache->AddX + usecmanage->Space ;
					if( TotalHeight < cache->DrawY + cache->SizeY )
					{
						TotalHeight = cache->DrawY + cache->SizeY ;
					}
					x += cache->AddX + usecmanage->Space ;
				}
			}
		}

		if( MaxTotalWidth < TotalWidth )
		{
			MaxTotalWidth = TotalWidth ;
		}
		if( MaxTotalHeight < TotalHeight )
		{
			MaxTotalHeight = TotalHeight ;
		}
	}

	if( DrawSizeP != NULL )
	{
		DrawSizeP->cx = MaxTotalWidth ;
		DrawSizeP->cy = MaxTotalHeight ;
	}

	if( UseAlloc )
	{
		DXFREE( AllocDrawStrBuf ) ;
	}

	// 終了
	return VerticalFlag ? TotalHeight : TotalWidth ;
}

// 基本イメージに文字列イメージを転送する
extern int NS_FontBaseImageBlt( int x, int y, const TCHAR *StrData, BASEIMAGE *DestImage, BASEIMAGE *DestEdgeImage, int VerticalFlag )
{
	return NS_FontBaseImageBltToHandle( x, y, StrData, DestImage, DestEdgeImage, DX_DEFAULT_FONT_HANDLE, VerticalFlag ) ;
}

// 基本イメージに文字列を描画する( デフォルトフォントハンドルを使用する )
extern int NS_FontBaseImageBltWithStrLen( int x, int y, const TCHAR *StrData, size_t StrDataLength, BASEIMAGE *DestImage, BASEIMAGE *DestEdgeImage, int VerticalFlag )
{
	int Result ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_ONE_BEGIN( StrData, StrDataLength, return -1 )
	Result = NS_FontBaseImageBltToHandle( x, y, StrData, DestImage, DestEdgeImage, DX_DEFAULT_FONT_HANDLE, VerticalFlag ) ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( StrData )
	return Result ;
}

// 基本イメージに文字列イメージを転送する
extern int FontBaseImageBlt_WCHAR_T( int x, int y, const wchar_t *StrData, int StrLen, BASEIMAGE *DestImage, BASEIMAGE *DestEdgeImage, int VerticalFlag )
{
	return FontBaseImageBltToHandle_WCHAR_T( x, y, StrData, StrLen, DestImage, DestEdgeImage, DX_DEFAULT_FONT_HANDLE, VerticalFlag ) ;
}

// 基本イメージに文字列を転送する
extern int NS_FontBaseImageBltToHandle( int x, int y, const TCHAR *StrData, BASEIMAGE *DestImage, BASEIMAGE *DestEdgeImage, int FontHandle, int VerticalFlag )
{
#ifdef UNICODE
	return FontBaseImageBltToHandle_WCHAR_T( x, y, StrData, -1, DestImage, DestEdgeImage, FontHandle, VerticalFlag ) ;
#else
	int Result ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_BEGIN( StrData, -1 )

	Result = FontBaseImageBltToHandle_WCHAR_T( x, y, UseStringBuffer, -1, DestImage, DestEdgeImage, FontHandle, VerticalFlag ) ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_END

	return Result ;
#endif
}

// 基本イメージに文字列を描画する
extern int NS_FontBaseImageBltToHandleWithStrLen( int x, int y, const TCHAR *StrData, size_t StrDataLength, BASEIMAGE *DestImage, BASEIMAGE *DestEdgeImage, int FontHandle, int VerticalFlag )
{
	int Result ;
#ifdef UNICODE
	FONTHANDLE_WCHAR_T_WITH_STRLENGTH_TO_WCHAR_T_STRING_BEGIN( StrData, StrDataLength, -1 )
	Result = FontBaseImageBltToHandle_WCHAR_T( x, y, UseStringBuffer, ( int )StrDataLength, DestImage, DestEdgeImage, FontHandle, VerticalFlag ) ;
	FONTHANDLE_WCHAR_T_TO_WCHAR_T_STRING_END
#else
	FONTHANDLE_TCHAR_WITH_STRLENGTH_TO_WCHAR_T_STRING_BEGIN( StrData, StrDataLength, -1 )
	Result = FontBaseImageBltToHandle_WCHAR_T( x, y, UseStringBuffer, ( int )wchar_StrLength, DestImage, DestEdgeImage, FontHandle, VerticalFlag ) ;
	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_END
#endif
	return Result ;
}

// 基本イメージに文字列を転送する
extern int FontBaseImageBltToHandle_WCHAR_T( int x, int y, const wchar_t *StrData, int StrLen, BASEIMAGE *DestImage, BASEIMAGE *DestEdgeImage, int FontHandle, int VerticalFlag )
{
	DWORD DrawStrBuf[ 256 ] ;		// 描画する際に使用する文字列データ（３バイトで１文字を表現）
	FONTMANAGE * cmanage ;
	FONTMANAGE * usecmanage ;
	FONTCHARDATA * cache ;
	RECT SrcRect ;
	RECT BaseRect ;
	RECT DstRect ;
	POINT DstPoint ;
	int drawnum ;							// ２バイト文字１バイト文字全部ひっくるめて何文字あるか保存する変数
	int i ;									// 繰り返しと汎用変数
	const DWORD *strp ;
	int TotalWidth, TotalHeight ;
	int MaxTotalWidth, MaxTotalHeight ;
	int orig_x, orig_y ;
	int tmp_x, tmp_y ;
	int off_x, off_y ;
	int add_x = 0, add_y = 0 ;
//	bool Hangeul ;

	DEFAULT_FONT_HANDLE_SETUP

	// エラー判定
	if( FONTHCHK( FontHandle, cmanage ) )
	{
		return -1 ;
	}

	// 縁用画像が有効で、且つ本体用画像とサイズが違った場合はエラー
	if( DestEdgeImage != NULL && ( DestEdgeImage->Width != DestImage->Width || DestEdgeImage->Height != DestImage->Height ) )
	{
		return -1 ;
	}

	// テクスチャをキャッシュに使用していないフォントは無理
	if( cmanage->TextureCacheFlag == FALSE )
	{
		return -1 ;
	}

	// ＵＮＩコードかどうかを取得しておく
//	Hangeul = cmanage->BaseInfo.CharSet == DX_CHARSET_HANGEUL ;

	// 文字列がない場合は終了
	if( StrData  == NULL || *StrData  == 0 )
	{
		return 0 ;
	}

	// キャッシュに入っていない文字列をキャッシュに入れる
	FontCacheStringAddToHandle( cmanage, StrData, StrLen, DrawStrBuf, &drawnum ) ;

	// 描画座標の補正
	if( VerticalFlag )
	{
		add_x = -cmanage->BaseInfo.FontAddHeight / 2 ;
	}
	else
	{
		add_y = -cmanage->BaseInfo.FontAddHeight / 2 ;
	}

	// 初期座標を保存しておく
	orig_x = x ;
	orig_y = y ;

	// 文字の数だけ繰り返し
	strp			= DrawStrBuf ;			// 描画用文字データの先頭アドレスをセット
	TotalWidth		= 0 ;
	TotalHeight		= 0 ;
	MaxTotalWidth	= 0 ;
	MaxTotalHeight	= 0 ;
	for( i = 0 ; i < drawnum ; i ++, strp ++ )
	{
		cache = NULL ;
		usecmanage = cmanage ;

		// 改行文字だった場合はキャンセルする
		if( *strp == L'\n' )
		{
			goto LOOPEND ;
		}

		// キャッシュデータを取得
		cache = GetFontCacheChar_Inline( cmanage, *strp, &usecmanage, &off_x, &off_y, FALSE, FALSE ) ;
		if( cache == NULL )
		{
			return -1 ;
		}

		// 描画座標の準備
		if( cmanage == usecmanage )
		{
			tmp_x = x + add_x ;
			tmp_y = y + add_y ;
		}
		else
		{
			tmp_x = x ;
			tmp_y = y ;
			if( VerticalFlag )
			{
				tmp_x += off_y ;
				tmp_y += off_x ;
			}
			else
			{
				tmp_x += off_x ;
				tmp_y += off_y ;
			}

			if( usecmanage->BaseInfo.FontAddHeight != 0 )
			{
				if( VerticalFlag )
				{
					tmp_x -= usecmanage->BaseInfo.FontAddHeight / 2 ;
				}
				else
				{
					tmp_y -= usecmanage->BaseInfo.FontAddHeight / 2 ;
				}
			}
		}

		// スペース文字だった場合はキャンセルする
		if( *strp == L' ' || ( wchar_t )*strp == FSYS.DoubleByteSpaceCharCode )
		{
			goto LOOPEND ;
		}

		// サイズが無かったら何もしない
		if( cache->SizeX == 0 && cache->SizeY == 0 )
		{
			goto LOOPEND ;
		}

		// 転送先の描画領域を作成
		DstRect.left	= tmp_x + cache->DrawX ;
		DstRect.top		= tmp_y + cache->DrawY ;
		DstRect.right	= tmp_x + cache->DrawX + cache->SizeX ;
		DstRect.bottom	= tmp_y + cache->DrawY + cache->SizeY ;
		BaseRect.left = 0 ;
		BaseRect.top = 0 ;
		BaseRect.right = DestImage->Width ;
		BaseRect.bottom = DestImage->Height ;
		RectClipping_Inline( &DstRect, &BaseRect ) ;

		// 描画する文字が無かった場合ここで次ループに移る
		if( DstRect.right == DstRect.left || DstRect.bottom == DstRect.top ) goto LOOPEND ;

		// 転送元の矩形をセット
		SrcRect.left	=   cache->GraphIndex / usecmanage->LengthCharNum   * usecmanage->BaseInfo.MaxWidth ;
		SrcRect.top		= ( cache->GraphIndex % usecmanage->LengthCharNum ) * usecmanage->BaseInfo.MaxWidth ;
		if( usecmanage->TextureCacheFlag == TRUE )
		{
			SrcRect.left ++ ;
			SrcRect.top  ++ ;
		}
		SrcRect.right	= SrcRect.left + cache->SizeX ;
		SrcRect.bottom	= SrcRect.top  + cache->SizeY ;

		// 転送元の矩形を補正
		SrcRect.left   += DstRect.left - ( tmp_x + cache->DrawX ) ;
		SrcRect.top    += DstRect.top  - ( tmp_y + cache->DrawY ) ;
		SrcRect.right  -= ( ( tmp_x + cache->DrawX ) + cache->SizeX ) - DstRect.right ;
		SrcRect.bottom -= ( ( tmp_y + cache->DrawY ) + cache->SizeY ) - DstRect.bottom ;

		// 転送
		DstPoint.x = DstRect.left ;
		DstPoint.y = DstRect.top ;
		NS_GraphColorMatchBltVer2(
			DestImage->GraphData, DestImage->Pitch, &DestImage->ColorData,
			usecmanage->TextureCacheBaseImage.GraphData, usecmanage->TextureCacheBaseImage.Pitch, &usecmanage->TextureCacheBaseImage.ColorData,
			NULL, 0, NULL,
			DstPoint, &SrcRect, FALSE,
			FALSE, 0,
			BASEIM.ImageShavedMode, FALSE,
			FALSE, FALSE,
			FALSE ) ;
		if( DestEdgeImage && ( usecmanage->FontType & DX_FONTTYPE_EDGE ) != 0 )
		{
			SrcRect.left  += usecmanage->TextureCacheBaseImage.Width / 2 ;
			SrcRect.right += usecmanage->TextureCacheBaseImage.Width / 2 ;
			NS_GraphColorMatchBltVer2(
				DestEdgeImage->GraphData, DestEdgeImage->Pitch, &DestEdgeImage->ColorData,
				usecmanage->TextureCacheBaseImage.GraphData, usecmanage->TextureCacheBaseImage.Pitch, &usecmanage->TextureCacheBaseImage.ColorData,
				NULL, 0, NULL,
				DstPoint, &SrcRect, FALSE,
				FALSE, 0,
				BASEIM.ImageShavedMode, FALSE,
				FALSE, FALSE,
				FALSE ) ;
		}

LOOPEND :

		// 改行文字の場合は処理を分岐
		if( *strp == '\n' )
		{
			int LineSpace = usecmanage->LineSpaceValidFlag ? usecmanage->LineSpace : usecmanage->BaseInfo.FontHeight ;

			if( VerticalFlag == TRUE )
			{
				x           -= LineSpace ;
				TotalWidth  += LineSpace ;
				y           = orig_y ;
				TotalHeight = 0 ;
			}
			else
			{
				y           += LineSpace ;
				TotalHeight += LineSpace ;
				x          = orig_x ;
				TotalWidth = 0 ;
			}
		}
		else
		{
			if( cache == NULL )
			{
				if( VerticalFlag )
				{
					TotalHeight += usecmanage->Space ;
					y           += usecmanage->Space ;
				}
				else
				{
					TotalWidth  += usecmanage->Space ;
					x           += usecmanage->Space ;
				}
			}
			else
			{
				if( VerticalFlag )
				{
					TotalHeight += cache->AddX + usecmanage->Space ;
					if( TotalWidth < cache->DrawX + cache->SizeX )
					{
						TotalWidth = cache->DrawX + cache->SizeX ;
					}
					y += cache->AddX + usecmanage->Space ;
				}
				else
				{
					TotalWidth += cache->AddX + usecmanage->Space ;
					if( TotalHeight < cache->DrawY + cache->SizeY )
					{
						TotalHeight = cache->DrawY + cache->SizeY ;
					}
					x += cache->AddX + usecmanage->Space ;
				}
			}
		}

		if( MaxTotalWidth < TotalWidth )
		{
			MaxTotalWidth = TotalWidth ;
		}
		if( MaxTotalHeight < TotalHeight )
		{
			MaxTotalHeight = TotalHeight ;
		}
	}

	// 終了
	return 0 ;
}

// フォントハンドルのフォント名を取得する
extern const TCHAR *NS_GetFontNameToHandle( int FontHandle )
{
	FONTMANAGE * ManageData ;

	DEFAULT_FONT_HANDLE_SETUP

	// エラー判定
	if( FONTHCHK( FontHandle, ManageData ) )
		return NULL ;

	return ManageData->FontNameT ;
}

// 文字の最大幅を得る
extern int NS_GetFontMaxWidth( void )
{
	return NS_GetFontMaxWidthToHandle( DX_DEFAULT_FONT_HANDLE ) ;
}

// 文字の最大幅を得る
extern int NS_GetFontMaxWidthToHandle( int FontHandle )
{
	FONTMANAGE * ManageData ;

	DEFAULT_FONT_HANDLE_SETUP

	// エラー判定
	if( FONTHCHK( FontHandle, ManageData ) )
		return -1 ;

	return ManageData->BaseInfo.MaxWidth ;
}

// デフォルトフォントハンドルの描画位置からベースラインまでの高さを取得する
extern int NS_GetFontAscent( void )
{
	return NS_GetFontAscentToHandle( DX_DEFAULT_FONT_HANDLE ) ;
}

// フォントハンドルの描画位置からベースラインまでの高さを取得する
extern int NS_GetFontAscentToHandle( int FontHandle )
{
	FONTMANAGE * ManageData ;

	DEFAULT_FONT_HANDLE_SETUP

	// エラー判定
	if( FONTHCHK( FontHandle, ManageData ) )
		return -1 ;

	return ManageData->BaseInfo.Ascent ;
}

// 指定の文字の描画情報を取得する
extern int NS_GetFontCharInfo( int FontHandle, const TCHAR *Char, int *DrawX, int *DrawY, int *NextCharX, int *SizeX, int *SizeY )
{
#ifdef UNICODE
	return GetFontCharInfo_WCHAR_T( FontHandle, Char, DrawX, DrawY, NextCharX, SizeX, SizeY ) ;
#else
	int Result ;
	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_BEGIN( Char, -1 )

	Result = GetFontCharInfo_WCHAR_T( FontHandle, UseStringBuffer, DrawX, DrawY, NextCharX, SizeX, SizeY ) ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_END

	return Result ;
#endif
}

// フォントハンドルの指定の文字の描画情報を取得する
extern int NS_GetFontCharInfoWithStrLen( int FontHandle, const TCHAR *Char, size_t CharLength, int *DrawX, int *DrawY, int *NextCharX, int *SizeX, int *SizeY )
{
	int Result ;
#ifdef UNICODE
	FONTHANDLE_WCHAR_T_WITH_STRLENGTH_TO_WCHAR_T_STRING_BEGIN( Char, CharLength, -1 )
	Result = GetFontCharInfo_WCHAR_T( FontHandle, UseStringBuffer, DrawX, DrawY, NextCharX, SizeX, SizeY ) ;
	FONTHANDLE_WCHAR_T_TO_WCHAR_T_STRING_END
#else
	FONTHANDLE_TCHAR_WITH_STRLENGTH_TO_WCHAR_T_STRING_BEGIN( Char, CharLength, -1 )
	Result = GetFontCharInfo_WCHAR_T( FontHandle, UseStringBuffer, DrawX, DrawY, NextCharX, SizeX, SizeY ) ;
	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_END
#endif
	return Result ;
}

// 指定の文字の描画情報を取得する
extern int GetFontCharInfo_WCHAR_T( int FontHandle, const wchar_t *Char, int *DrawX, int *DrawY, int *NextCharX, int *SizeX, int *SizeY )
{
	FONTMANAGE * ManageData ;
	FONTMANAGE * UseManageData ;
	FONTCHARDATA * CharData ;
	DWORD CharCode ;
	int DrawOffsetX, DrawOffsetY ;
	int CharBytes ;

	DEFAULT_FONT_HANDLE_SETUP

	// エラー判定
	if( FONTHCHK( FontHandle, ManageData ) )
	{
		return -1 ;
	}

	// 文字コードを取得
	CharCode = GetCharCode( ( const char * )Char, WCHAR_T_CHARCODEFORMAT, &CharBytes ) ;

	// フォントデータを取得
	CharData = GetFontCacheChar_Inline( ManageData, CharCode, &UseManageData, &DrawOffsetX, &DrawOffsetY, FALSE, FALSE ) ;
	if( CharData == NULL )
	{
		// キャッシュに追加できなかったらエラー
		return -1 ;
	}

	// フォントデータを返す
	if( DrawX     ) *DrawX     = CharData->DrawX ;
	if( DrawY     ) *DrawY     = CharData->DrawY ;
	if( NextCharX ) *NextCharX = CharData->AddX  ;
	if( SizeX     ) *SizeX     = CharData->SizeX ;
	if( SizeY     ) *SizeY     = CharData->SizeY ;

	if( DrawY != NULL && ManageData->BaseInfo.FontAddHeight != 0 )
	{
		*DrawY -= ManageData->BaseInfo.FontAddHeight / 2 ;
	}

	// 正常終了
	return 0 ;
}

// 文字列の幅を得る
extern int NS_GetDrawStringWidth( const TCHAR *String, int StrLen, int VerticalFlag )
{
	return NS_GetDrawStringWidthToHandle( String, StrLen, DX_DEFAULT_FONT_HANDLE, VerticalFlag ) ;
}

// 文字列の幅を得る
extern int NS_GetDrawNStringWidth( const TCHAR *String, size_t StringLength, int VerticalFlag )
{
	return NS_GetDrawNStringWidthToHandle( String, StringLength, DX_DEFAULT_FONT_HANDLE, VerticalFlag ) ;
}

// 文字列の幅を得る
extern int GetDrawStringWidth_WCHAR_T( const wchar_t *String ,int StrLen, int VerticalFlag )
{
	return GetDrawStringWidthToHandle_WCHAR_T( String, 0, StrLen, DX_DEFAULT_FONT_HANDLE, VerticalFlag ) ;
}

// 書式付き文字列の描画幅を得る
extern int NS_GetDrawFormatStringWidth( const TCHAR *FormatString, ... )
{
	int FontHandle = DX_DEFAULT_FONT_HANDLE ;

	TCHAR_FONTHANDLE_FORMATSTRING_SETUP( -1 )

	// 文字列の幅を得る
	return NS_GetDrawStringWidthToHandle(
				String, ( int )CL_strlen( CharCodeFormat, ( const char * )String ), FontHandle ) ;
}

// 書式付き文字列の描画幅を得る
extern int GetDrawFormatStringWidth_WCHAR_T( const wchar_t *FormatString, ... )
{
	WCHAR_T_FORMATSTRING_SETUP

	// 文字列の幅を得る
	return GetDrawStringWidthToHandle_WCHAR_T( String, 0, ( int )_WCSLEN( String ), DX_DEFAULT_FONT_HANDLE, FALSE ) ;
}


// 書式付き文字列の描画幅を得る
extern int NS_GetDrawFormatStringWidthToHandle( int FontHandle, const TCHAR *FormatString, ... )
{
	TCHAR_FONTHANDLE_FORMATSTRING_SETUP( -1 )

	// 文字列の幅を得る
	return NS_GetDrawStringWidthToHandle(
				String, ( int )CL_strlen( CharCodeFormat, ( const char * )String ), FontHandle ) ;
}

// 書式付き文字列の描画幅を得る
extern int GetDrawFormatStringWidthToHandle_WCHAR_T( int FontHandle, const wchar_t *FormatString, ... )
{
	WCHAR_T_FORMATSTRING_SETUP

	// 文字列の幅を得る
	return GetDrawStringWidthToHandle_WCHAR_T( String, 0, ( int )_WCSLEN( String ), FontHandle, FALSE ) ;
}

// 文字列の幅を得る
extern int NS_GetDrawStringWidthToHandle( const TCHAR *String, int StrLen, int FontHandle, int VerticalFlag )
{
#ifdef UNICODE
	return GetDrawStringWidthToHandle_WCHAR_T( String, 0, StrLen, FontHandle, VerticalFlag ) ;
#else
	int Result ;

	FONTHANDLE_TCHAR_WITH_STRLEN_TO_WCHAR_T_STRING_BEGIN( String, StrLen, -1 )

	Result = GetDrawStringWidthToHandle_WCHAR_T( UseStringBuffer, 0, StrLen, FontHandle, VerticalFlag ) ;

	FONTHANDLE_TCHAR_WITH_STRLEN_TO_WCHAR_T_STRING_END

	return Result ;
#endif
}

// 文字列の幅を得る
extern int NS_GetDrawNStringWidthToHandle( const TCHAR *String, size_t StringLength, int FontHandle, int VerticalFlag )
{
#ifdef UNICODE
	return GetDrawStringWidthToHandle_WCHAR_T( String, StringLength, ( int )StringLength, FontHandle, VerticalFlag ) ;
#else
	int Result ;

	FONTHANDLE_TCHAR_WITH_STRLENGTH_TO_WCHAR_T_STRING_BEGIN( String, StringLength, -1 )

	Result = GetDrawStringWidthToHandle_WCHAR_T( UseStringBuffer, wchar_StrLength, ( int )wchar_StrLength, FontHandle, VerticalFlag ) ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_END

	return Result ;
#endif
}

// 文字列の幅を得る
extern int GetDrawStringWidthToHandle_WCHAR_T( const wchar_t *String, size_t StringLength, int StrLen, int FontHandle, int VerticalFlag )
{
	FONTMANAGE *ManageData ;

	ManageData = GetFontManageDataToHandle_Inline( FontHandle ) ;
	if( ManageData == NULL )
	{
		return -1 ;
	}

	if( StringLength > 0 )
	{
		StrLen = ( int )StringLength ;
	}
	else
	{
		int Length ;
		Length = ( int )_WCSLEN( String ) ;
		if( StrLen < 0 || StrLen > Length )
		{
			StrLen = Length ;
		}
	}

	return FontCacheStringDrawToHandleST(
				FALSE,
				0,
				0,
				0.0f,
				0.0f,
				TRUE,
				FALSE,
				1.0,
				1.0,
				FALSE, 0.0f, 0.0f, 0.0,
				String,
				0,
				NULL,
				NULL,
				FALSE,
				ManageData,
				0,
				StrLen,
				VerticalFlag,
				NULL,
				NULL,
				NULL,
				0,
				NULL,
				0
			) ;
}



// 文字列の幅を得る
extern int NS_GetDrawExtendStringWidth( double ExRateX, const TCHAR *String ,int StrLen, int VerticalFlag )
{
	return NS_GetDrawExtendStringWidthToHandle( ExRateX, String, StrLen, DX_DEFAULT_FONT_HANDLE, VerticalFlag ) ;
}

// 文字列の幅を得る
extern int NS_GetDrawExtendNStringWidth( double ExRateX, const TCHAR *String, size_t StringLength, int VerticalFlag )
{
	return NS_GetDrawExtendNStringWidthToHandle( ExRateX, String, StringLength, DX_DEFAULT_FONT_HANDLE, VerticalFlag ) ;
}

// 文字列の幅を得る
extern int GetDrawExtendStringWidth_WCHAR_T( double ExRateX, const wchar_t *String ,int StrLen, int VerticalFlag )
{
	return GetDrawExtendStringWidthToHandle_WCHAR_T( ExRateX, String, 0, StrLen, DX_DEFAULT_FONT_HANDLE, VerticalFlag ) ;
}


// 書式付き文字列の描画幅を得る
extern int NS_GetDrawExtendFormatStringWidth( double ExRateX, const TCHAR *FormatString, ... )
{
	int FontHandle = DX_DEFAULT_FONT_HANDLE ;

	TCHAR_FONTHANDLE_FORMATSTRING_SETUP( -1 )

	// 文字列の幅を得る
	return NS_GetDrawExtendStringWidthToHandle(
		ExRateX, String, ( int )CL_strlen( CharCodeFormat,  ( const char * )String ), DX_DEFAULT_FONT_HANDLE ) ;
}

// 書式付き文字列の描画幅を得る
extern int GetDrawExtendFormatStringWidth_WCHAR_T( double ExRateX, const wchar_t *FormatString, ... )
{
	WCHAR_T_FORMATSTRING_SETUP

	// 文字列の幅を得る
	return GetDrawExtendStringWidthToHandle_WCHAR_T( ExRateX, String, 0, ( int )_WCSLEN( String ), DX_DEFAULT_FONT_HANDLE, FALSE ) ;
}



// 書式付き文字列の描画幅を得る
extern int NS_GetDrawExtendFormatStringWidthToHandle( double ExRateX, int FontHandle, const TCHAR *FormatString, ... )
{
	TCHAR_FONTHANDLE_FORMATSTRING_SETUP( -1 )

	// 文字列の幅を得る
	return NS_GetDrawExtendStringWidthToHandle(
				ExRateX, String, ( int )CL_strlen( CharCodeFormat, ( const char * )String ), FontHandle ) ;
}

// 書式付き文字列の描画幅を得る
extern int GetDrawExtendFormatStringWidthToHandle_WCHAR_T( double ExRateX, int FontHandle, const wchar_t *FormatString, ... )
{
	WCHAR_T_FORMATSTRING_SETUP

	// 文字列の幅を得る
	return GetDrawExtendStringWidthToHandle_WCHAR_T( ExRateX, String, 0, ( int )_WCSLEN( String ), FontHandle, FALSE ) ;
}



// 文字列の幅を得る
extern int NS_GetDrawExtendStringWidthToHandle( double ExRateX, const TCHAR *String, int StrLen, int FontHandle, int VerticalFlag )
{
#ifdef UNICODE
	return GetDrawExtendStringWidthToHandle_WCHAR_T( ExRateX, String, 0, StrLen, FontHandle, VerticalFlag ) ;
#else
	int Result ;

	FONTHANDLE_TCHAR_WITH_STRLEN_TO_WCHAR_T_STRING_BEGIN( String, StrLen, -1 )

	Result = GetDrawExtendStringWidthToHandle_WCHAR_T( ExRateX, UseStringBuffer, 0, StrLen, FontHandle, VerticalFlag ) ;

	FONTHANDLE_TCHAR_WITH_STRLEN_TO_WCHAR_T_STRING_END

	return Result ;
#endif
}

// 文字列の幅を得る
extern int NS_GetDrawExtendNStringWidthToHandle( double ExRateX, const TCHAR *String, size_t StringLength, int FontHandle, int VerticalFlag )
{
#ifdef UNICODE
	return GetDrawExtendStringWidthToHandle_WCHAR_T( ExRateX, String, StringLength, ( int )StringLength, FontHandle, VerticalFlag ) ;
#else
	int Result ;

	FONTHANDLE_TCHAR_WITH_STRLENGTH_TO_WCHAR_T_STRING_BEGIN( String, StringLength, -1 )

	Result = GetDrawExtendStringWidthToHandle_WCHAR_T( ExRateX, UseStringBuffer, wchar_StrLength, ( int )wchar_StrLength, FontHandle, VerticalFlag ) ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_END

	return Result ;
#endif
}

// 文字列の幅を得る
extern int GetDrawExtendStringWidthToHandle_WCHAR_T( double ExRateX, const wchar_t *String, size_t StringLength, int StrLen, int FontHandle, int VerticalFlag )
{
	FONTMANAGE *ManageData ;

	ManageData = GetFontManageDataToHandle_Inline( FontHandle ) ;
	if( ManageData == NULL )
	{
		return -1 ;
	}

	if( StringLength > 0 )
	{
		StrLen = ( int )StringLength ;
	}
	else
	{
		int Length ;
		Length = ( int )_WCSLEN( String ) ;
		if( StrLen < 0 || StrLen > Length )
		{
			StrLen = Length ;
		}
	}

	return FontCacheStringDrawToHandleST(
				FALSE,
				0,
				0,
				0.0f,
				0.0f,
				TRUE,
				TRUE,
				ExRateX,
				1.0,
				FALSE, 0.0f, 0.0f, 0.0,
				String,
				0,
				NULL,
				NULL,
				FALSE,
				ManageData,
				0,
				StrLen,
				VerticalFlag,
				NULL,
				NULL,
				NULL,
				0,
				NULL,
				0
			) ;
}

// デフォルトフォントハンドルを使用した文字列の描画幅・高さ・行数を取得する
extern int NS_GetDrawStringSize( int *SizeX, int *SizeY, int *LineCount, const TCHAR *String, int StrLen, int VerticalFlag )
{
	return NS_GetDrawStringSizeToHandle( SizeX, SizeY, LineCount, String, StrLen, DX_DEFAULT_FONT_HANDLE, VerticalFlag ) ;
}

// デフォルトフォントハンドルを使用した文字列の描画幅・高さ・行数を取得する
extern int NS_GetDrawNStringSize( int *SizeX, int *SizeY, int *LineCount, const TCHAR *String, size_t StringLength, int VerticalFlag )
{
	return NS_GetDrawNStringSizeToHandle( SizeX, SizeY, LineCount, String, StringLength, DX_DEFAULT_FONT_HANDLE, VerticalFlag ) ;
}

// デフォルトフォントハンドルを使用した書式付き文字列の描画幅・高さ・行数を取得する
extern int NS_GetDrawFormatStringSize( int *SizeX, int *SizeY, int *LineCount, const TCHAR *FormatString, ... )
{
	int FontHandle = DX_DEFAULT_FONT_HANDLE ;

	TCHAR_FONTHANDLE_FORMATSTRING_SETUP( -1 )

	// 文字列の幅を得る
	return NS_GetDrawStringSizeToHandle(
				SizeX, SizeY, LineCount, String, ( int )CL_strlen( CharCodeFormat, ( const char * )String ), FontHandle ) ;
}

// デフォルトフォントハンドルを使用した文字列の描画幅・高さ・行数を取得する( 拡大率付き )
extern int NS_GetDrawExtendStringSize( int *SizeX, int *SizeY, int *LineCount, double ExRateX, double ExRateY, const TCHAR *String, int StrLen, int VerticalFlag )
{
	return NS_GetDrawExtendStringSizeToHandle( SizeX, SizeY, LineCount, ExRateX, ExRateY, String, StrLen, DX_DEFAULT_FONT_HANDLE, VerticalFlag ) ;
}

// デフォルトフォントハンドルを使用した文字列の描画幅・高さ・行数を取得する( 拡大率付き )
extern int NS_GetDrawExtendNStringSize( int *SizeX, int *SizeY, int *LineCount, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, int VerticalFlag )
{
	return NS_GetDrawExtendNStringSizeToHandle( SizeX, SizeY, LineCount, ExRateX, ExRateY, String, StringLength, DX_DEFAULT_FONT_HANDLE, VerticalFlag ) ;
}

// デフォルトフォントハンドルを使用した書式付き文字列の描画幅・高さ・行数を取得する( 拡大率付き )
extern int NS_GetDrawExtendFormatStringSize( int *SizeX, int *SizeY, int *LineCount, double ExRateX, double ExRateY, const TCHAR *FormatString, ... )
{
	int FontHandle = DX_DEFAULT_FONT_HANDLE ;

	TCHAR_FONTHANDLE_FORMATSTRING_SETUP( -1 )

	// 文字列の幅を得る
	return NS_GetDrawExtendStringSizeToHandle(
		SizeX, SizeY, LineCount, ExRateX, ExRateY, String, ( int )CL_strlen( CharCodeFormat,  ( const char * )String ), DX_DEFAULT_FONT_HANDLE ) ;
}

// フォントハンドルを使用した文字列の描画幅・高さ・行数を取得する
extern int NS_GetDrawStringSizeToHandle( int *SizeX, int *SizeY, int *LineCount, const TCHAR   *String, int StrLen, int FontHandle, int VerticalFlag )
{
#ifdef UNICODE
	return GetDrawStringSizeToHandle_WCHAR_T( SizeX, SizeY, LineCount, String, 0, StrLen, FontHandle, VerticalFlag ) ;
#else
	int Result ;
	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_BEGIN( String, -1 )

	Result = GetDrawStringSizeToHandle_WCHAR_T( SizeX, SizeY, LineCount, UseStringBuffer, 0, StrLen, FontHandle, VerticalFlag ) ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_END

	return Result ;
#endif
}

// フォントハンドルを使用した文字列の描画幅・高さ・行数を取得する
extern int NS_GetDrawNStringSizeToHandle( int *SizeX, int *SizeY, int *LineCount, const TCHAR *String, size_t StringLength, int FontHandle, int VerticalFlag )
{
#ifdef UNICODE
	return GetDrawStringSizeToHandle_WCHAR_T( SizeX, SizeY, LineCount, String, StringLength, ( int )StringLength, FontHandle, VerticalFlag ) ;
#else
	int Result ;
	FONTHANDLE_TCHAR_WITH_STRLENGTH_TO_WCHAR_T_STRING_BEGIN( String, StringLength, -1 )

	Result = GetDrawStringSizeToHandle_WCHAR_T( SizeX, SizeY, LineCount, UseStringBuffer, wchar_StrLength, ( int )wchar_StrLength, FontHandle, VerticalFlag ) ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_END

	return Result ;
#endif
}

// 文字列の幅を得る
extern int GetDrawStringSizeToHandle_WCHAR_T( int *SizeX, int *SizeY, int *LineCount, const wchar_t *String, size_t StringLength, int StrLen, int FontHandle, int VerticalFlag )
{
	FONTMANAGE *ManageData ;
	SIZE DrawSize ;
	int Result ;

	ManageData = GetFontManageDataToHandle_Inline( FontHandle ) ;
	if( ManageData == NULL )
	{
		return -1 ;
	}

	if( StringLength > 0 )
	{
		StrLen = ( int )StringLength ;
	}
	else
	{
		int Length ;
		Length = ( int )_WCSLEN( String ) ;
		if( StrLen < 0 || StrLen > Length )
		{
			StrLen = Length ;
		}
	}

	Result = FontCacheStringDrawToHandleST(
				FALSE,
				0,
				0,
				0.0f,
				0.0f,
				TRUE,
				FALSE,
				1.0,
				1.0,
				FALSE, 0.0f, 0.0f, 0.0,
				String,
				0,
				NULL,
				NULL,
				FALSE,
				ManageData,
				0,
				StrLen,
				VerticalFlag,
				&DrawSize,
				LineCount,
				NULL,
				0,
				NULL,
				0
			) ;

	if( SizeX )
	{
		*SizeX = DrawSize.cx ;
	}

	if( SizeY )
	{
		*SizeY = DrawSize.cy ;
	}

	return Result ;
}

// フォントハンドルを使用した書式付き文字列の描画幅・高さ・行数を取得する
extern int NS_GetDrawFormatStringSizeToHandle( int *SizeX, int *SizeY, int *LineCount, int FontHandle, const TCHAR *FormatString, ... )
{
	TCHAR_FONTHANDLE_FORMATSTRING_SETUP( -1 )

	// 文字列の幅を得る
	return NS_GetDrawStringSizeToHandle(
				SizeX, SizeY, LineCount, String, ( int )CL_strlen( CharCodeFormat, ( const char * )String ), FontHandle ) ;
}

// フォントハンドルを使用した文字列の描画幅・高さ・行数を取得する
extern int NS_GetDrawExtendStringSizeToHandle( int *SizeX, int *SizeY, int *LineCount, double ExRateX, double ExRateY, const TCHAR *String, int StrLen, int FontHandle, int VerticalFlag )
{
#ifdef UNICODE
	return GetDrawExtendStringSizeToHandle_WCHAR_T( SizeX, SizeY, LineCount, ExRateX, ExRateY, String, 0, StrLen, FontHandle, VerticalFlag ) ;
#else
	int Result ;

	FONTHANDLE_TCHAR_WITH_STRLEN_TO_WCHAR_T_STRING_BEGIN( String, StrLen, -1 )

	Result = GetDrawExtendStringSizeToHandle_WCHAR_T( SizeX, SizeY, LineCount, ExRateX, ExRateY, UseStringBuffer, 0, StrLen, FontHandle, VerticalFlag ) ;

	FONTHANDLE_TCHAR_WITH_STRLEN_TO_WCHAR_T_STRING_END

	return Result ;
#endif
}

// フォントハンドルを使用した文字列の描画幅・高さ・行数を取得する
extern int NS_GetDrawExtendNStringSizeToHandle( int *SizeX, int *SizeY, int *LineCount, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, int FontHandle, int VerticalFlag )
{
#ifdef UNICODE
	return GetDrawExtendStringSizeToHandle_WCHAR_T( SizeX, SizeY, LineCount, ExRateX, ExRateY, String, StringLength, ( int )StringLength, FontHandle, VerticalFlag ) ;
#else
	int Result ;

	FONTHANDLE_TCHAR_WITH_STRLENGTH_TO_WCHAR_T_STRING_BEGIN( String, StringLength, -1 )

	Result = GetDrawExtendStringSizeToHandle_WCHAR_T( SizeX, SizeY, LineCount, ExRateX, ExRateY, UseStringBuffer, wchar_StrLength, ( int )wchar_StrLength, FontHandle, VerticalFlag ) ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_END

	return Result ;
#endif
}

// 文字列の幅を得る
extern int GetDrawExtendStringSizeToHandle_WCHAR_T( int *SizeX, int *SizeY, int *LineCount, double ExRateX, double ExRateY, const wchar_t *String, size_t StringLength, int StrLen, int FontHandle, int VerticalFlag )
{
	FONTMANAGE *ManageData ;
	SIZE DrawSize ;
	int Result ;

	ManageData = GetFontManageDataToHandle_Inline( FontHandle ) ;
	if( ManageData == NULL )
	{
		return -1 ;
	}

	if( StringLength > 0 )
	{
		StrLen = ( int )StringLength ;
	}
	else
	{
		int Length ;
		Length = ( int )_WCSLEN( String ) ;
		if( StrLen < 0 || StrLen > Length )
		{
			StrLen = Length ;
		}
	}

	Result = FontCacheStringDrawToHandleST(
				FALSE,
				0,
				0,
				0.0f,
				0.0f,
				TRUE,
				TRUE,
				ExRateX,
				ExRateY,
				FALSE, 0.0f, 0.0f, 0.0,
				String,
				0,
				NULL,
				NULL,
				FALSE,
				ManageData,
				0,
				StrLen,
				VerticalFlag,
				&DrawSize,
				LineCount,
				NULL,
				0,
				NULL,
				0
			) ;

	if( SizeX )
	{
		*SizeX = DrawSize.cx ;
	}

	if( SizeY )
	{
		*SizeY = DrawSize.cy ;
	}

	return Result ;
}

// フォントハンドルを使用した書式付き文字列の描画幅・高さ・行数を取得する
extern int NS_GetDrawExtendFormatStringSizeToHandle( int *SizeX, int *SizeY, int *LineCount, double ExRateX, double ExRateY, int FontHandle, const TCHAR *FormatString, ... )
{
	TCHAR_FONTHANDLE_FORMATSTRING_SETUP( -1 )

	// 文字列の幅を得る
	return NS_GetDrawExtendStringSizeToHandle(
				SizeX, SizeY, LineCount, ExRateX, ExRateY, String, ( int )CL_strlen( CharCodeFormat, ( const char * )String ), FontHandle ) ;
}



// デフォルトフォントハンドルを使用した文字列の１文字毎の情報を取得する
extern int NS_GetDrawStringCharInfo( DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, const TCHAR *String, int StrLen, int VerticalFlag )
{
	return NS_GetDrawStringCharInfoToHandle( InfoBuffer, InfoBufferSize, String, StrLen, DX_DEFAULT_FONT_HANDLE, VerticalFlag ) ;
}

// デフォルトフォントハンドルを使用した文字列の１文字毎の情報を取得する
extern int NS_GetDrawNStringCharInfo( DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, const TCHAR *String, size_t StringLength, int VerticalFlag )
{
	return NS_GetDrawNStringCharInfoToHandle( InfoBuffer, InfoBufferSize, String, StringLength, DX_DEFAULT_FONT_HANDLE, VerticalFlag ) ;
}

// デフォルトフォントハンドルを使用した書式付き文字列の１文字毎の情報を取得する
extern int NS_GetDrawFormatStringCharInfo( DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, const TCHAR *FormatString, ... )
{
	int FontHandle = DX_DEFAULT_FONT_HANDLE ;

	TCHAR_FONTHANDLE_FORMATSTRING_SETUP( -1 )

	return NS_GetDrawStringCharInfoToHandle(
				InfoBuffer, InfoBufferSize, String, ( int )CL_strlen( CharCodeFormat, ( const char * )String ), FontHandle ) ;
}

// デフォルトフォントハンドルを使用した文字列の１文字毎の情報を取得する
extern int NS_GetDrawExtendStringCharInfo( DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, double ExRateX, double ExRateY, const TCHAR *String, int StrLen, int VerticalFlag )
{
	return NS_GetDrawExtendStringCharInfoToHandle( InfoBuffer, InfoBufferSize, ExRateX, ExRateY, String, StrLen, DX_DEFAULT_FONT_HANDLE, VerticalFlag ) ;
}

// デフォルトフォントハンドルを使用した文字列の１文字毎の情報を取得する
extern int NS_GetDrawExtendNStringCharInfo( DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, int VerticalFlag )
{
	return NS_GetDrawExtendNStringCharInfoToHandle( InfoBuffer, InfoBufferSize, ExRateX, ExRateY, String, StringLength, DX_DEFAULT_FONT_HANDLE, VerticalFlag ) ;
}

// デフォルトフォントハンドルを使用した書式付き文字列の１文字毎の情報を取得する
extern int NS_GetDrawExtendFormatStringCharInfo( DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, double ExRateX, double ExRateY, const TCHAR *FormatString, ... )
{
	int FontHandle = DX_DEFAULT_FONT_HANDLE ;

	TCHAR_FONTHANDLE_FORMATSTRING_SETUP( -1 )

	// 文字列の幅を得る
	return NS_GetDrawExtendStringCharInfoToHandle(
		InfoBuffer, InfoBufferSize, ExRateX, ExRateY, String, ( int )CL_strlen( CharCodeFormat,  ( const char * )String ), DX_DEFAULT_FONT_HANDLE ) ;
}

// フォントハンドルを使用した文字列の１文字毎の情報を取得する
extern int NS_GetDrawStringCharInfoToHandle( DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, const TCHAR *String, int StrLen, int FontHandle, int VerticalFlag )
{
#ifdef UNICODE
	return GetDrawStringCharInfoToHandle_WCHAR_T( InfoBuffer, InfoBufferSize, String, 0, StrLen, FontHandle, VerticalFlag ) ;
#else
	int Result ;
	FONTHANDLE_TCHAR_WITH_STRLEN_TO_WCHAR_T_STRING_BEGIN( String, StrLen, -1 )

	Result = GetDrawStringCharInfoToHandle_WCHAR_T( InfoBuffer, InfoBufferSize, UseStringBuffer, 0, StrLen, FontHandle, VerticalFlag ) ;

	FONTHANDLE_TCHAR_WITH_STRLEN_TO_WCHAR_T_STRING_END

	return Result ;
#endif
}

// フォントハンドルを使用した文字列の１文字毎の情報を取得する
extern int NS_GetDrawNStringCharInfoToHandle( DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, const TCHAR *String, size_t StringLength, int FontHandle, int VerticalFlag )
{
#ifdef UNICODE
	return GetDrawStringCharInfoToHandle_WCHAR_T( InfoBuffer, InfoBufferSize, String, StringLength, ( int )StringLength, FontHandle, VerticalFlag ) ;
#else
	int Result ;
	FONTHANDLE_TCHAR_WITH_STRLENGTH_TO_WCHAR_T_STRING_BEGIN( String, StringLength, -1 )

	Result = GetDrawStringCharInfoToHandle_WCHAR_T( InfoBuffer, InfoBufferSize, UseStringBuffer, wchar_StrLength, ( int )wchar_StrLength, FontHandle, VerticalFlag ) ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_END

	return Result ;
#endif
}

// フォントハンドルを使用した文字列の１文字毎の情報を取得する
extern int GetDrawStringCharInfoToHandle_WCHAR_T( DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, const wchar_t *String, size_t StringLength, int StrLen, int FontHandle, int VerticalFlag )
{
	FONTMANAGE *ManageData ;
	int Result ;
	int CharInfoNum ;

	ManageData = GetFontManageDataToHandle_Inline( FontHandle ) ;
	if( ManageData == NULL )
	{
		return -1 ;
	}

	if( StringLength > 0 )
	{
		StrLen = ( int )StringLength ;
	}
	else
	{
		int Length ;
		Length = ( int )_WCSLEN( String ) ;
		if( StrLen < 0 || StrLen > Length )
		{
			StrLen = Length ;
		}
	}

	Result = FontCacheStringDrawToHandleST(
				FALSE,
				0,
				0,
				0.0f,
				0.0f,
				TRUE,
				FALSE,
				1.0,
				1.0,
				FALSE, 0.0f, 0.0f, 0.0,
				String,
				0,
				NULL,
				NULL,
				FALSE,
				ManageData,
				0,
				StrLen,
				VerticalFlag,
				NULL,
				NULL,
				InfoBuffer,
				InfoBufferSize,
				&CharInfoNum,
				0
			) ;

	return Result < 0 ? -1 : CharInfoNum ;
}

// フォントハンドルを使用した書式付き文字列の１文字毎の情報を取得する
extern int NS_GetDrawFormatStringCharInfoToHandle( DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, int FontHandle, const TCHAR *FormatString, ... )
{
	TCHAR_FONTHANDLE_FORMATSTRING_SETUP( -1 )

	return NS_GetDrawStringCharInfoToHandle(
				InfoBuffer, InfoBufferSize, String, ( int )CL_strlen( CharCodeFormat, ( const char * )String ), FontHandle ) ;
}

// フォントハンドルを使用した文字列の１文字毎の情報を取得する
extern int NS_GetDrawExtendStringCharInfoToHandle(  DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, double ExRateX, double ExRateY, const TCHAR *String, int StrLen, int FontHandle, int VerticalFlag )
{
#ifdef UNICODE
	return GetDrawExtendStringCharInfoToHandle_WCHAR_T( InfoBuffer, InfoBufferSize, ExRateX, ExRateY, String, 0, StrLen, FontHandle, VerticalFlag ) ;
#else
	int Result ;

	FONTHANDLE_TCHAR_WITH_STRLEN_TO_WCHAR_T_STRING_BEGIN( String, StrLen, -1 )

	Result = GetDrawExtendStringCharInfoToHandle_WCHAR_T( InfoBuffer, InfoBufferSize, ExRateX, ExRateY, UseStringBuffer, 0, StrLen, FontHandle, VerticalFlag ) ;

	FONTHANDLE_TCHAR_WITH_STRLEN_TO_WCHAR_T_STRING_END

	return Result ;
#endif
}

// フォントハンドルを使用した文字列の１文字毎の情報を取得する
extern int NS_GetDrawExtendNStringCharInfoToHandle(  DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, int FontHandle, int VerticalFlag )
{
#ifdef UNICODE
	return GetDrawExtendStringCharInfoToHandle_WCHAR_T( InfoBuffer, InfoBufferSize, ExRateX, ExRateY, String, StringLength, ( int )StringLength, FontHandle, VerticalFlag ) ;
#else
	int Result ;

	FONTHANDLE_TCHAR_WITH_STRLENGTH_TO_WCHAR_T_STRING_BEGIN( String, StringLength, -1 )

	Result = GetDrawExtendStringCharInfoToHandle_WCHAR_T( InfoBuffer, InfoBufferSize, ExRateX, ExRateY, UseStringBuffer, wchar_StrLength, ( int )wchar_StrLength, FontHandle, VerticalFlag ) ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_END

	return Result ;
#endif
}

// フォントハンドルを使用した文字列の１文字毎の情報を取得する
extern int GetDrawExtendStringCharInfoToHandle_WCHAR_T( DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, double ExRateX, double ExRateY, const wchar_t *String, size_t StringLength, int StrLen, int FontHandle, int VerticalFlag )
{
	FONTMANAGE *ManageData ;
	int Result ;
	int CharInfoNum ;

	ManageData = GetFontManageDataToHandle_Inline( FontHandle ) ;
	if( ManageData == NULL )
	{
		return -1 ;
	}

	if( StringLength > 0 )
	{
		StrLen = ( int )StringLength ;
	}
	else
	{
		int Length ;
		Length = ( int )_WCSLEN( String ) ;
		if( StrLen < 0 || StrLen > Length )
		{
			StrLen = Length ;
		}
	}

	Result = FontCacheStringDrawToHandleST(
				FALSE,
				0,
				0,
				0.0f,
				0.0f,
				TRUE,
				TRUE,
				ExRateX,
				ExRateY,
				FALSE, 0.0f, 0.0f, 0.0,
				String,
				0,
				NULL,
				NULL,
				FALSE,
				ManageData,
				0,
				StrLen,
				VerticalFlag,
				NULL,
				NULL,
				InfoBuffer,
				InfoBufferSize,
				&CharInfoNum,
				0
			) ;

	return Result < 0 ? -1 : CharInfoNum ;
}

// フォントハンドルを使用した書式付き文字列の１文字毎の情報を取得する
extern int NS_GetDrawExtendFormatStringCharInfoToHandle( DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, double ExRateX, double ExRateY, int FontHandle, const TCHAR *FormatString, ... )
{
	TCHAR_FONTHANDLE_FORMATSTRING_SETUP( -1 )

	return NS_GetDrawExtendStringCharInfoToHandle(
				InfoBuffer, InfoBufferSize, ExRateX, ExRateY, String, ( int )CL_strlen( CharCodeFormat, ( const char * )String ), FontHandle ) ;
}




// フォントの情報を得る
extern int NS_GetFontStateToHandle( TCHAR *FontName, int *Size, int *Thick, int FontHandle, int *FontType , int *CharSet , int *EdgeSize , int *Italic )
{
#ifdef UNICODE
	return GetFontStateToHandle_WCHAR_T( FontName, Size, Thick, FontHandle, FontType , CharSet , EdgeSize , Italic ) ;
#else
	wchar_t FontNameBuffer[ 256 ] ;
	int Result ;

	TCHAR_CHARCODEFORMAT_SETUP( -1 )

	Result = GetFontStateToHandle_WCHAR_T( FontNameBuffer, Size, Thick, FontHandle, FontType , CharSet , EdgeSize , Italic ) ;
	if( Result < 0 )
	{
		return -1 ;
	}

	ConvString( ( const char * )FontNameBuffer, -1, WCHAR_T_CHARCODEFORMAT, ( char * )FontName, BUFFERBYTES_CANCEL, CharCodeFormat ) ;	

	return Result ;
#endif
}

// フォントの情報を得る
extern int GetFontStateToHandle_WCHAR_T( wchar_t *FontName, int *Size, int *Thick, int FontHandle, int *FontType , int *CharSet , int *EdgeSize , int *Italic )
{
	FONTMANAGE * ManageData ;

	DEFAULT_FONT_HANDLE_SETUP

	// エラー判定
	if( FONTHCHK( FontHandle, ManageData ) )
	{
		return -1 ;
	}

	if( FontName ) _WCSCPY( FontName, ManageData->FontName ) ;
	if( Thick    ) *Thick    = ManageData->BaseInfo.FontThickness ;
	if( Size     ) *Size     = ManageData->BaseInfo.FontSize ;
	if( FontType ) *FontType = ManageData->FontType ;
	if( CharSet  ) *CharSet  = ManageData->BaseInfo.CharSet ;
	if( EdgeSize ) *EdgeSize = ManageData->EdgeSize ;
	if( Italic   ) *Italic   = ManageData->BaseInfo.Italic ;

	// 終了
	return 0;
}

// デフォルトのフォントのハンドルを得る
extern int NS_GetDefaultFontHandle( void )
{
	return FSYS.DefaultFontHandle ;
}

// 指定のフォントがテクスチャキャッシュを使用しているかどうかを得る
extern int NS_CheckFontCacheToTextureFlag( int FontHandle )
{
	FONTMANAGE * ManageData ;

	DEFAULT_FONT_HANDLE_SETUP

	// エラー判定
	if( FONTHCHK( FontHandle, ManageData ) )
		return -1 ;

	// フラグを返す
	return ManageData->TextureCacheFlag ;
}

// 指定のフォントがテクスチャキャッシュを使用しているかどうかを得る(誤字版)
extern int NS_CheckFontChacheToTextureFlag( int FontHandle )
{
	return NS_CheckFontCacheToTextureFlag( FontHandle );
}

// フォントにテクスチャキャッシュを使用するかどうかを取得する
extern int NS_GetFontCacheToTextureFlag( void )
{
	return !FSYS.NotTextureFlag ;
}

// フォントにテクスチャキャッシュを使用するかどうかを取得する(誤字版)
extern int NS_GetFontChacheToTextureFlag( void )
{
	return NS_GetFontCacheToTextureFlag() ;
}

// フォントのキャッシュとして使用するテクスチャのカラービット深度を設定する( 16 又は 32 のみ指定可能  デフォルトは 32 )
extern int NS_SetFontCacheTextureColorBitDepth( int ColorBitDepth )
{
	// フラグを保存
	FSYS.TextureCacheColorBitDepth16Flag = ColorBitDepth == 16 ? TRUE : FALSE ;
	
	// 終了
	return 0 ;
}

// フォントのキャッシュとして使用するテクスチャのカラービット深度を取得する
extern int NS_GetFontCacheTextureColorBitDepth( void )
{
	return FSYS.TextureCacheColorBitDepth16Flag ? 16 : 32 ;
}

// 指定のフォントハンドルが有効か否か調べる
extern int NS_CheckFontHandleValid( int FontHandle )
{
	FONTMANAGE * ManageData ;

	DEFAULT_FONT_HANDLE_SETUP

	if( FONTHCHK( FontHandle, ManageData ) )
		return 0 ;

	return 1 ;
}

// フォントハンドルのキャッシュ情報を初期化する
extern int NS_ClearFontCacheToHandle( int FontHandle )
{
	FONTMANAGE * ManageData ;

	DEFAULT_FONT_HANDLE_SETUP

	if( FONTHCHK( FontHandle, ManageData ) )
		return 0 ;

	// フォントのキャッシュ情報を初期化する
	InitFontCacheToHandle( ManageData, FALSE ) ;

	// 終了
	return 0 ;
}

// ２バイト文字か調べる( TRUE:２バイト文字  FALSE:１バイト文字 )
extern int NS_MultiByteCharCheck( const char *Buf, int CharSet )
{
	int   CharBytes ;

	if( CharSet < 0 || CharSet >= DX_CHARSET_NUM )
	{
		return -1 ;
	}

	GetCharCode( Buf, CharCodeFormatTable[ CharSet ], &CharBytes ) ;

	return CharBytes > 1 ? TRUE : FALSE ;
}


// フォントキャッシュでキャッシュできる文字数を取得する( 戻り値  0:デフォルト  1以上:指定文字数 )
extern int NS_GetFontCacheCharNum( void )
{
	return FSYS.CacheCharNum ;
}

// フォントのサイズを得る
extern int NS_GetFontSize( void )
{
	return NS_GetFontSizeToHandle( DX_DEFAULT_FONT_HANDLE ) ;
}

// デフォルトフォントハンドルの縁サイズを取得する
extern int NS_GetFontEdgeSize( void )
{
	return NS_GetFontEdgeSizeToHandle( DX_DEFAULT_FONT_HANDLE ) ;
}

// フォントのサイズを得る
extern int NS_GetFontSizeToHandle( int FontHandle )
{
	FONTMANAGE * ManageData ;
	
	DEFAULT_FONT_HANDLE_SETUP

	if( FONTHCHK( FontHandle, ManageData ) )
		return -1 ;

	return ManageData->BaseInfo.FontSize ;
}

// フォントハンドルの縁サイズを取得する
extern int NS_GetFontEdgeSizeToHandle( int FontHandle )
{
	FONTMANAGE * ManageData ;
	
	DEFAULT_FONT_HANDLE_SETUP

	if( FONTHCHK( FontHandle, ManageData ) )
		return -1 ;

	return ManageData->EdgeSize ;
}

// フォントハンドルの字間を取得する
extern int NS_GetFontSpaceToHandle( int FontHandle )
{
	FONTMANAGE * ManageData ;
	
	DEFAULT_FONT_HANDLE_SETUP

	if( FONTHCHK( FontHandle, ManageData ) )
		return -1 ;

	return ManageData->Space ;
}

// フォントハンドルの行間を取得する
extern int NS_GetFontLineSpaceToHandle( int FontHandle )
{
	FONTMANAGE * ManageData ;
	
	DEFAULT_FONT_HANDLE_SETUP

	if( FONTHCHK( FontHandle, ManageData ) )
		return -1 ;

	return ManageData->LineSpaceValidFlag ? ManageData->LineSpace : ManageData->BaseInfo.FontHeight ;
}

// フォントキャッシュとして保存する画像の形式を乗算済みαチャンネル付き画像にするかどうかを設定する( TRUE:乗算済みαを使用する  FLASE:乗算済みαを使用しない( デフォルト ) )
extern int NS_SetFontCacheUsePremulAlphaFlag( int Flag )
{
	FSYS.UsePremulAlphaFlag = Flag == FALSE ? FALSE : TRUE ;

	return 0 ;
}

// フォントキャッシュとして保存する画像の形式を乗算済みαチャンネル付き画像にするかどうかを取得する
extern int NS_GetFontCacheUsePremulAlphaFlag( void )
{
	return FSYS.UsePremulAlphaFlag ;
}

// フォントのサイズを補正する処理を行うかどうかを設定する( Flag  TRUE:行う( デフォルト )  FALSE:行わない )
extern int NS_SetFontUseAdjustSizeFlag( int Flag )
{
	FSYS.DisableAdjustFontSize = Flag == FALSE ? TRUE : FALSE ;

	return 0 ;
}

// フォントのサイズを補正する処理を行うかどうかを取得する
extern int NS_GetFontUseAdjustSizeFlag( void )
{
	return FSYS.DisableAdjustFontSize == FALSE ? TRUE : FALSE ;
}

// 文字列を描画する
extern int NS_DrawString( int x, int y, const TCHAR *String, unsigned int Color, unsigned int EdgeColor )
{
	return NS_DrawStringToHandle( x, y, String, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, FALSE ) ;
}

// デフォルトフォントハンドルを使用して文字列を描画する
extern int NS_DrawNString( int x, int y, const TCHAR *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor )
{
	return NS_DrawNStringToHandle( x, y, String, StringLength, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, FALSE ) ;
}

// 文字列を描画する
extern int DrawString_WCHAR_T( int x, int y, const wchar_t *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor )
{
	return DrawStringToHandle_WCHAR_T( x, y, String, StringLength, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, FALSE ) ;
}

// 文字列を描画する
extern int NS_DrawStringF( float x, float y, const TCHAR *String, unsigned int Color, unsigned int EdgeColor )
{
	return NS_DrawStringFToHandle( x, y, String, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, FALSE ) ;
}

// デフォルトフォントハンドルを使用して文字列を描画する( 座標指定が float 版 )
extern int NS_DrawNStringF( float x, float y, const TCHAR *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor )
{
	return NS_DrawNStringFToHandle( x, y, String, StringLength, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, FALSE ) ;
}

// 文字列を描画する
extern int DrawStringF_WCHAR_T( float x, float y, const wchar_t *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor )
{
	return DrawStringFToHandle_WCHAR_T( x, y, String, StringLength, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, FALSE ) ;
}

// 文字列を描画する
extern int NS_DrawVString( int x, int y, const TCHAR *String, unsigned int Color, unsigned int EdgeColor )
{
	return NS_DrawStringToHandle( x, y, String, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, TRUE ) ;
}

// デフォルトフォントハンドルを使用して文字列を描画する( 縦書き )
extern int NS_DrawNVString( int x, int y, const TCHAR *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor )
{
	return NS_DrawNVStringToHandle( x, y, String, StringLength, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor ) ;
}

// 文字列を描画する
extern int DrawVString_WCHAR_T( int x, int y, const wchar_t *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor )
{
	return DrawStringToHandle_WCHAR_T( x, y, String, StringLength, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, TRUE ) ;
}

// 文字列を描画する
extern int NS_DrawVStringF( float x, float y, const TCHAR *String, unsigned int Color, unsigned int EdgeColor )
{
	return NS_DrawStringFToHandle( x, y, String, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, TRUE ) ;
}

// デフォルトフォントハンドルを使用して文字列を描画する( 縦書き )( 座標指定が float 版 )
extern int NS_DrawNVStringF( float x, float y, const TCHAR *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor )
{
	return NS_DrawNVStringFToHandle( x, y, String, StringLength, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor ) ;
}

// 文字列を描画する
extern int DrawVStringF_WCHAR_T( float x, float y, const wchar_t *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor )
{
	return DrawStringFToHandle_WCHAR_T( x, y, String, StringLength, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, TRUE ) ;
}

// 文字列を描画する
#define SETDRAWRECTCODE\
	if( VerticalFlag )\
	{\
		x += font->BaseInfo.FontAddHeight / 2;\
		SETRECT( DrawRect, x, y, x + NS_GetFontSizeToHandle( FontHandle ) + 3, GSYS.DrawSetting.DrawArea.bottom ) ;\
		if( DrawRect.left >= GSYS.DrawSetting.DrawArea.right ) return 0 ;\
		x -= font->BaseInfo.FontAddHeight / 2;\
	}\
	else\
	{\
		y -= font->BaseInfo.FontAddHeight / 2 ;\
		SETRECT( DrawRect, x, y, GSYS.DrawSetting.DrawArea.right, y + NS_GetFontSizeToHandle( FontHandle ) + 3 ) ;\
		if( DrawRect.left >= GSYS.DrawSetting.DrawArea.right ) return 0 ;\
		y += font->BaseInfo.FontAddHeight / 2 ;\
	}\
	DRAWRECT_DRAWAREA_CLIP

extern int NS_DrawStringToHandle( int x, int y, const TCHAR *String, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag )
{
#ifdef UNICODE
	return DrawStringToHandle_WCHAR_T( x, y, String, CL_strlen( WCHAR_T_CHARCODEFORMAT, ( const char * )String ), Color, FontHandle, EdgeColor, VerticalFlag ) ;
#else
	int Result ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_BEGIN( String, -1 )

	Result = DrawStringToHandle_WCHAR_T( x, y, UseStringBuffer, wchar_StrLength, Color, FontHandle, EdgeColor, VerticalFlag ) ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_END

	return Result ;
#endif
}

extern int NS_DrawNStringToHandle( int x, int y, const TCHAR *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag )
{
#ifdef UNICODE
	return DrawStringToHandle_WCHAR_T( x, y, String, StringLength, Color, FontHandle, EdgeColor, VerticalFlag ) ;
#else
	int Result ;

	FONTHANDLE_TCHAR_WITH_STRLENGTH_TO_WCHAR_T_STRING_BEGIN( String, StringLength, -1 )

	Result = DrawStringToHandle_WCHAR_T( x, y, UseStringBuffer, wchar_StrLength, Color, FontHandle, EdgeColor, VerticalFlag ) ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_END

	return Result ;
#endif
}

extern int DrawStringToHandle_WCHAR_T( int x, int y, const wchar_t *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag )
{
#ifndef DX_NON_GRAPHICS
	int Ret = -1 ;
	FONTMANAGE *font ;

//	DEFAULT_FONT_HANDLE_SETUP

	if( String == NULL || String[0] == L'\0' || StringLength == 0 ||
		/* GSYS.NotDrawFlag || */ GSYS.DrawSetting.NotDrawFlagInSetDrawArea )
	{
		return 0 ;
	}

	CheckActiveState() ;

	font = GetFontManageDataToHandle_Inline( FontHandle ) ;
	if( font == NULL )
	{
		DXST_LOGFILE_ADDUTF16LE( "\xd5\x30\xa9\x30\xf3\x30\xc8\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x24\x50\x4c\x30\x70\x75\x38\x5e\x67\x30\x59\x30\x0a\x00\x00"/*@ L"フォントハンドル値が異常です\n" @*/ ) ;
		return Ret ;
	}

	// ２Ｄ行列をハードウエアに反映する
	if( GSYS.DrawSetting.MatchHardware_2DMatrix == FALSE && GSYS.Setting.ValidHardware )
		Graphics_DrawSetting_ApplyLib2DMatrixToHardware() ;

	// 描画
	DRAW_DEF(
		DrawStringHardware( x, y, ( float )x, ( float )y, TRUE, String, StringLength, Color, font, EdgeColor, VerticalFlag ),
		DrawStringSoftware( x, y,                               String, StringLength, Color, font, EdgeColor, VerticalFlag ),
		SETDRAWRECTCODE,
		Ret,
		font->TextureCacheFlag
	)

	// 終了
	return Ret ;
#else // DX_NON_GRAPHICS
	return -1 ;
#endif // DX_NON_GRAPHICS
}

#undef SETDRAWRECTCODE



// 文字列を描画する
#define SETDRAWRECTCODE\
	if( VerticalFlag )\
	{\
		x += ( float )font->BaseInfo.FontAddHeight / 2;\
		SETRECT( DrawRect, _FTOL( x ), _FTOL( y ), _FTOL( x ) + NS_GetFontSizeToHandle( FontHandle ) + 3, GSYS.DrawSetting.DrawArea.bottom ) ;\
		if( DrawRect.left >= GSYS.DrawSetting.DrawArea.right ) return 0 ;\
		x -= ( float )font->BaseInfo.FontAddHeight / 2;\
	}\
	else\
	{\
		y -= ( float )font->BaseInfo.FontAddHeight / 2 ;\
		SETRECT( DrawRect, _FTOL( x ), _FTOL( y ), GSYS.DrawSetting.DrawArea.right, _FTOL( y ) + NS_GetFontSizeToHandle( FontHandle ) + 3 ) ;\
		if( DrawRect.left >= GSYS.DrawSetting.DrawArea.right ) return 0 ;\
		y += ( float )font->BaseInfo.FontAddHeight / 2 ;\
	}\
	DRAWRECT_DRAWAREA_CLIP

extern int NS_DrawStringFToHandle( float x, float y, const TCHAR *String, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag )
{
#ifdef UNICODE
	return DrawStringFToHandle_WCHAR_T( x, y, String, CL_strlen( WCHAR_T_CHARCODEFORMAT, ( const char * )String ), Color, FontHandle, EdgeColor, VerticalFlag ) ;
#else
	int Result ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_BEGIN( String, -1 )

	Result = DrawStringFToHandle_WCHAR_T( x, y, UseStringBuffer, wchar_StrLength, Color, FontHandle, EdgeColor, VerticalFlag ) ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_END

	return Result ;
#endif
}

extern int NS_DrawNStringFToHandle( float x, float y, const TCHAR *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag )
{
#ifdef UNICODE
	return DrawStringFToHandle_WCHAR_T( x, y, String, StringLength, Color, FontHandle, EdgeColor, VerticalFlag ) ;
#else
	int Result ;

	FONTHANDLE_TCHAR_WITH_STRLENGTH_TO_WCHAR_T_STRING_BEGIN( String, StringLength, -1 )

	Result = DrawStringFToHandle_WCHAR_T( x, y, UseStringBuffer, wchar_StrLength, Color, FontHandle, EdgeColor, VerticalFlag ) ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_END

	return Result ;
#endif
}

extern int DrawStringFToHandle_WCHAR_T( float x, float y, const wchar_t *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag )
{
#ifndef DX_NON_GRAPHICS
	int Ret = -1 ;
	FONTMANAGE *font ;

//	DEFAULT_FONT_HANDLE_SETUP

	if( String == NULL || String[0] == L'\0' || StringLength == 0 ||
		/* GSYS.NotDrawFlag || */ GSYS.DrawSetting.NotDrawFlagInSetDrawArea )
	{
		return 0 ;
	}

	CheckActiveState() ;

	font = GetFontManageDataToHandle_Inline( FontHandle ) ;
	if( font == NULL )
	{
		DXST_LOGFILE_ADDUTF16LE( "\xd5\x30\xa9\x30\xf3\x30\xc8\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x24\x50\x4c\x30\x70\x75\x38\x5e\x67\x30\x59\x30\x0a\x00\x00"/*@ L"フォントハンドル値が異常です\n" @*/ ) ;
		return Ret ;
	}

	// ２Ｄ行列をハードウエアに反映する
	if( GSYS.DrawSetting.MatchHardware_2DMatrix == FALSE && GSYS.Setting.ValidHardware )
		Graphics_DrawSetting_ApplyLib2DMatrixToHardware() ;

	// 描画
	DRAW_DEF(
		DrawStringHardware( _FTOL( x ), _FTOL( y ), x, y, FALSE, String, StringLength, Color, font, EdgeColor, VerticalFlag ),
		DrawStringSoftware( _FTOL( x ), _FTOL( y ),              String, StringLength, Color, font, EdgeColor, VerticalFlag ),
		SETDRAWRECTCODE,
		Ret,
		font->TextureCacheFlag
	)

	// 終了
	return Ret ;
#else // DX_NON_GRAPHICS
	return -1 ;
#endif // DX_NON_GRAPHICS
}

#undef SETDRAWRECTCODE




// 文字列を描画する
extern int NS_DrawVStringToHandle( int x, int y, const TCHAR *String, unsigned int Color, int FontHandle, unsigned int EdgeColor )
{
	return NS_DrawStringToHandle( x, y, String, Color, FontHandle, EdgeColor, TRUE ) ;
}

// 文字列を描画する
extern int NS_DrawNVStringToHandle( int x, int y, const TCHAR *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor )
{
	return NS_DrawNStringToHandle( x, y, String, StringLength, Color, FontHandle, EdgeColor, TRUE ) ;
}

// 文字列を描画する
extern int DrawVStringToHandle_WCHAR_T( int x, int y, const wchar_t *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor )
{
	return DrawStringToHandle_WCHAR_T( x, y, String, StringLength, Color, FontHandle, EdgeColor, TRUE ) ;
}

// 文字列を描画する
extern int NS_DrawVStringFToHandle( float x, float y, const TCHAR *String, unsigned int Color, int FontHandle, unsigned int EdgeColor )
{
	return NS_DrawStringFToHandle( x, y, String, Color, FontHandle, EdgeColor, TRUE ) ;
}

// 文字列を描画する
extern int NS_DrawNVStringFToHandle( float x, float y, const TCHAR *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor )
{
	return NS_DrawNStringFToHandle( x, y, String, StringLength, Color, FontHandle, EdgeColor, TRUE ) ;
}

// 文字列を描画する
extern int DrawVStringFToHandle_WCHAR_T( float x, float y, const wchar_t *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor )
{
	return DrawStringFToHandle_WCHAR_T( x, y, String, StringLength, Color, FontHandle, EdgeColor, TRUE ) ;
}

// 書式指定文字列を描画する
extern int NS_DrawFormatString( int x, int y, unsigned int Color, const TCHAR *FormatString, ... )
{
	int FontHandle = DX_DEFAULT_FONT_HANDLE ;

	TCHAR_FONTHANDLE_FORMATSTRING_SETUP( -1 )

	return NS_DrawStringToHandle( x, y, String, Color, DX_DEFAULT_FONT_HANDLE, 0, FALSE ) ;
}

// 書式指定文字列を描画する
extern int DrawFormatString_WCHAR_T( int x, int y, unsigned int Color, const wchar_t *FormatString, ... )
{
	WCHAR_T_FORMATSTRING_SETUP

	return DrawStringToHandle_WCHAR_T( x, y, String, CL_strlen( WCHAR_T_CHARCODEFORMAT, ( const char * )String ), Color, DX_DEFAULT_FONT_HANDLE, 0, FALSE ) ;
}

// 書式指定文字列を描画する
extern int NS_DrawFormatStringF( float x, float y, unsigned int Color, const TCHAR *FormatString, ... )
{
	int FontHandle = DX_DEFAULT_FONT_HANDLE ;

	TCHAR_FONTHANDLE_FORMATSTRING_SETUP( -1 )

	return NS_DrawStringFToHandle( x, y, String, Color, DX_DEFAULT_FONT_HANDLE, 0, FALSE ) ;
}

// 書式指定文字列を描画する
extern int DrawFormatStringF_WCHAR_T( float x, float y, unsigned int Color, const wchar_t *FormatString, ... )
{
	WCHAR_T_FORMATSTRING_SETUP

	return DrawStringFToHandle_WCHAR_T( x, y, String, CL_strlen( WCHAR_T_CHARCODEFORMAT, ( const char * )String ), Color, DX_DEFAULT_FONT_HANDLE, 0, FALSE ) ;
}

// 書式指定文字列を描画する
extern int NS_DrawFormatVString( int x, int y, unsigned int Color, const TCHAR *FormatString, ... )
{
	int FontHandle = DX_DEFAULT_FONT_HANDLE ;

	TCHAR_FONTHANDLE_FORMATSTRING_SETUP( -1 )

	return NS_DrawStringToHandle( x, y, String, Color, DX_DEFAULT_FONT_HANDLE, Color, TRUE ) ;
}

// 書式指定文字列を描画する
extern int DrawFormatVString_WCHAR_T( int x, int y, unsigned int Color, const wchar_t *FormatString, ... )
{
	WCHAR_T_FORMATSTRING_SETUP

	return DrawStringToHandle_WCHAR_T( x, y, String, CL_strlen( WCHAR_T_CHARCODEFORMAT, ( const char * )String ), Color, DX_DEFAULT_FONT_HANDLE, Color, TRUE ) ;
}

// 書式指定文字列を描画する
extern int NS_DrawFormatVStringF( float x, float y, unsigned int Color, const TCHAR *FormatString, ... )
{
	int FontHandle = DX_DEFAULT_FONT_HANDLE ;

	TCHAR_FONTHANDLE_FORMATSTRING_SETUP( -1 )

	return NS_DrawStringFToHandle( x, y, String, Color, DX_DEFAULT_FONT_HANDLE, Color, TRUE ) ;
}

// 書式指定文字列を描画する
extern int DrawFormatVStringF_WCHAR_T( float x, float y, unsigned int Color, const wchar_t *FormatString, ... )
{
	WCHAR_T_FORMATSTRING_SETUP

	return DrawStringFToHandle_WCHAR_T( x, y, String, CL_strlen( WCHAR_T_CHARCODEFORMAT, ( const char * )String ), Color, DX_DEFAULT_FONT_HANDLE, Color, TRUE ) ;
}

// 書式指定文字列を描画する
extern int NS_DrawFormatStringToHandle( int x, int y, unsigned int Color, int FontHandle, const TCHAR *FormatString, ... )
{
	TCHAR_FONTHANDLE_FORMATSTRING_SETUP( -1 )

	// 描画する
	return NS_DrawStringToHandle( x, y, String, Color, FontHandle, 0, FALSE ) ;
}

// 書式指定文字列を描画する
extern int DrawFormatStringToHandle_WCHAR_T( int x, int y, unsigned int Color, int FontHandle, const wchar_t *FormatString, ... )
{
	WCHAR_T_FORMATSTRING_SETUP

	// 描画する
	return DrawStringToHandle_WCHAR_T( x, y, String, CL_strlen( WCHAR_T_CHARCODEFORMAT, ( const char * )String ), Color, FontHandle, 0, FALSE ) ;
}

// 書式指定文字列を描画する
extern int NS_DrawFormatStringFToHandle( float x, float y, unsigned int Color, int FontHandle, const TCHAR *FormatString, ... )
{
	TCHAR_FONTHANDLE_FORMATSTRING_SETUP( -1 )

	// 描画する
	return NS_DrawStringFToHandle( x, y, String, Color, FontHandle, 0, FALSE ) ;
}

// 書式指定文字列を描画する
extern int DrawFormatStringFToHandle_WCHAR_T( float x, float y, unsigned int Color, int FontHandle, const wchar_t *FormatString, ... )
{
	WCHAR_T_FORMATSTRING_SETUP

	// 描画する
	return DrawStringFToHandle_WCHAR_T( x, y, String, CL_strlen( WCHAR_T_CHARCODEFORMAT, ( const char * )String ), Color, FontHandle, 0, FALSE ) ;
}

// 書式指定文字列を描画する
extern int NS_DrawFormatVStringToHandle( int x, int y, unsigned int Color, int FontHandle, const TCHAR *FormatString, ... )
{
	TCHAR_FONTHANDLE_FORMATSTRING_SETUP( -1 )

	// 描画する
	return NS_DrawStringToHandle( x, y, String, Color, FontHandle, 0, TRUE ) ;
}

// 書式指定文字列を描画する
extern int DrawFormatVStringToHandle_WCHAR_T( int x, int y, unsigned int Color, int FontHandle, const wchar_t *FormatString, ... )
{
	WCHAR_T_FORMATSTRING_SETUP

	// 描画する
	return DrawStringToHandle_WCHAR_T( x, y, String, CL_strlen( WCHAR_T_CHARCODEFORMAT, ( const char * )String ), Color, FontHandle, 0, TRUE ) ;
}

// 書式指定文字列を描画する
extern int NS_DrawFormatVStringFToHandle( float x, float y, unsigned int Color, int FontHandle, const TCHAR *FormatString, ... )
{
	TCHAR_FONTHANDLE_FORMATSTRING_SETUP( -1 )

	// 描画する
	return NS_DrawStringFToHandle( x, y, String, Color, FontHandle, 0, TRUE ) ;
}

// 書式指定文字列を描画する
extern int DrawFormatVStringFToHandle_WCHAR_T( float x, float y, unsigned int Color, int FontHandle, const wchar_t *FormatString, ... )
{
	WCHAR_T_FORMATSTRING_SETUP

	// 描画する
	return DrawStringFToHandle_WCHAR_T( x, y, String, CL_strlen( WCHAR_T_CHARCODEFORMAT, ( const char * )String ), Color, FontHandle, 0, TRUE ) ;
}


// 文字列を拡大描画する
extern int NS_DrawExtendString( int x, int y, double ExRateX, double ExRateY, const TCHAR *String, unsigned int Color, unsigned int EdgeColor )
{
	return NS_DrawExtendStringToHandle( x, y, ExRateX, ExRateY, String, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, FALSE ) ;
}

// デフォルトフォントハンドルを使用して文字列の拡大描画
extern int NS_DrawExtendNString( int x, int y, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor )
{
	return NS_DrawExtendNStringToHandle( x, y, ExRateX, ExRateY, String, StringLength, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, FALSE ) ;
}

// 文字列を拡大描画する
extern int DrawExtendString_WCHAR_T( int x, int y, double ExRateX, double ExRateY, const wchar_t *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor )
{
	return DrawExtendStringToHandle_WCHAR_T( x, y, ExRateX, ExRateY, String, StringLength, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, FALSE ) ;
}

// 文字列を拡大描画する
extern int NS_DrawExtendStringF( float x, float y, double ExRateX, double ExRateY, const TCHAR *String, unsigned int Color, unsigned int EdgeColor )
{
	return NS_DrawExtendStringFToHandle( x, y, ExRateX, ExRateY, String, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, FALSE ) ;
}

// デフォルトフォントハンドルを使用して文字列の拡大描画( 座標指定が float 版 )
extern int NS_DrawExtendNStringF( float x, float y, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor )
{
	return NS_DrawExtendNStringFToHandle( x, y, ExRateX, ExRateY, String, StringLength, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, FALSE ) ;
}

// 文字列を拡大描画する
extern int DrawExtendStringF_WCHAR_T( float x, float y, double ExRateX, double ExRateY, const wchar_t *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor )
{
	return DrawExtendStringFToHandle_WCHAR_T( x, y, ExRateX, ExRateY, String, StringLength, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, FALSE ) ;
}

// 文字列を拡大描画する
extern int NS_DrawExtendVString( int x, int y, double ExRateX, double ExRateY, const TCHAR *String, unsigned int Color, unsigned int EdgeColor )
{
	return NS_DrawExtendStringToHandle( x, y, ExRateX, ExRateY, String, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, TRUE ) ;
}

// 文字列を拡大描画する
extern int NS_DrawExtendNVString( int x, int y, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor )
{
	return NS_DrawExtendNStringToHandle( x, y, ExRateX, ExRateY, String, StringLength, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, TRUE ) ;
}

// 文字列を拡大描画する
extern int DrawExtendVString_WCHAR_T( int x, int y, double ExRateX, double ExRateY, const wchar_t *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor )
{
	return DrawExtendStringToHandle_WCHAR_T( x, y, ExRateX, ExRateY, String, StringLength, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, TRUE ) ;
}

// 文字列を拡大描画する
extern int NS_DrawExtendVStringF( float x, float y, double ExRateX, double ExRateY, const TCHAR *String, unsigned int Color, unsigned int EdgeColor )
{
	return NS_DrawExtendStringFToHandle( x, y, ExRateX, ExRateY, String, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, TRUE ) ;
}

// 文字列を拡大描画する
extern int NS_DrawExtendNVStringF( float x, float y, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor )
{
	return NS_DrawExtendNStringFToHandle( x, y, ExRateX, ExRateY, String, StringLength, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, TRUE ) ;
}

// 文字列を拡大描画する
extern int DrawExtendVStringF_WCHAR_T( float x, float y, double ExRateX, double ExRateY, const wchar_t *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor )
{
	return DrawExtendStringFToHandle_WCHAR_T( x, y, ExRateX, ExRateY, String, StringLength, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, TRUE ) ;
}

// 文字列を拡大描画する
#define SETDRAWRECTCODE\
	if( VerticalFlag )\
	{\
		SETRECT( DrawRect, x, y, x + _DTOL( NS_GetFontSizeToHandle( FontHandle ) * ExRateY ) + 3, GSYS.DrawSetting.DrawArea.bottom ) ;\
		if( DrawRect.left >= GSYS.DrawSetting.DrawArea.right ) return 0 ;\
	}\
	else\
	{\
		SETRECT( DrawRect, x, y, GSYS.DrawSetting.DrawArea.right, y + _DTOL( NS_GetFontSizeToHandle( FontHandle ) * ExRateY ) + 3 ) ;\
		if( DrawRect.left >= GSYS.DrawSetting.DrawArea.right ) return 0 ;\
	}

extern int NS_DrawExtendStringToHandle( int x, int y, double ExRateX, double ExRateY, const TCHAR *String, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag )
{
#ifdef UNICODE
	return DrawExtendStringToHandle_WCHAR_T( x, y, ExRateX, ExRateY, String, CL_strlen( WCHAR_T_CHARCODEFORMAT, ( const char * )String ), Color, FontHandle, EdgeColor, VerticalFlag ) ;
#else
	int Result ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_BEGIN( String, -1 )

	Result = DrawExtendStringToHandle_WCHAR_T( x, y, ExRateX, ExRateY, UseStringBuffer, wchar_StrLength, Color, FontHandle, EdgeColor, VerticalFlag ) ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_END

	return Result ;
#endif
}

extern int NS_DrawExtendNStringToHandle( int x, int y, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag )
{
#ifdef UNICODE
	return DrawExtendStringToHandle_WCHAR_T( x, y, ExRateX, ExRateY, String, StringLength, Color, FontHandle, EdgeColor, VerticalFlag ) ;
#else
	int Result ;

	FONTHANDLE_TCHAR_WITH_STRLENGTH_TO_WCHAR_T_STRING_BEGIN( String, StringLength, -1 )

	Result = DrawExtendStringToHandle_WCHAR_T( x, y, ExRateX, ExRateY, UseStringBuffer, wchar_StrLength, Color, FontHandle, EdgeColor, VerticalFlag ) ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_END

	return Result ;
#endif
}

extern int DrawExtendStringToHandle_WCHAR_T( int x, int y, double ExRateX, double ExRateY, const wchar_t *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag )
{
#ifndef DX_NON_GRAPHICS
	int Ret = -1 ;
	FONTMANAGE *font ;

//	DEFAULT_FONT_HANDLE_SETUP

	if( String == NULL || String[0] == L'\0' || StringLength == 0 ||
		/* GSYS.NotDrawFlag || */ GSYS.DrawSetting.NotDrawFlagInSetDrawArea )
	{
		return 0 ;
	}

	CheckActiveState() ;

	font = GetFontManageDataToHandle_Inline( FontHandle ) ;
	if( font == NULL )
	{
		DXST_LOGFILE_ADDUTF16LE( "\xd5\x30\xa9\x30\xf3\x30\xc8\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x24\x50\x4c\x30\x70\x75\x38\x5e\x67\x30\x59\x30\x0a\x00\x00"/*@ L"フォントハンドル値が異常です\n" @*/ ) ;
		return Ret ;
	}

	// ２Ｄ行列をハードウエアに反映する
	if( GSYS.DrawSetting.MatchHardware_2DMatrix == FALSE && GSYS.Setting.ValidHardware )
		Graphics_DrawSetting_ApplyLib2DMatrixToHardware() ;

	// 描画
	DRAW_DEF(
		DrawExtendStringHardware( x, y, ( float )x, ( float )y, TRUE, ExRateX, ExRateY, String, StringLength, Color, font, EdgeColor, VerticalFlag ),
		DrawExtendStringSoftware( x, y,                               ExRateX, ExRateY, String, StringLength, Color, font, EdgeColor, VerticalFlag ),
		SETDRAWRECTCODE,
		Ret,
		font->TextureCacheFlag
	)

	// 終了
	return Ret ;
#else // DX_NON_GRAPHICS
	return -1 ;
#endif // DX_NON_GRAPHICS
}

#undef SETDRAWRECTCODE



// 文字列を拡大描画する
#define SETDRAWRECTCODE\
	if( VerticalFlag )\
	{\
		SETRECT( DrawRect, _FTOL( x ), _FTOL( y ), _FTOL( x ) + _DTOL( NS_GetFontSizeToHandle( FontHandle ) * ExRateY ) + 3, GSYS.DrawSetting.DrawArea.bottom ) ;\
		if( DrawRect.left >= GSYS.DrawSetting.DrawArea.right ) return 0 ;\
	}\
	else\
	{\
		SETRECT( DrawRect, _FTOL( x ), _FTOL( y ), GSYS.DrawSetting.DrawArea.right, _FTOL( y ) + _DTOL( NS_GetFontSizeToHandle( FontHandle ) * ExRateY ) + 3 ) ;\
		if( DrawRect.left >= GSYS.DrawSetting.DrawArea.right ) return 0 ;\
	}

extern int NS_DrawExtendStringFToHandle( float x, float y, double ExRateX, double ExRateY, const TCHAR *String, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag )
{
#ifdef UNICODE
	return DrawExtendStringFToHandle_WCHAR_T( x, y, ExRateX, ExRateY, String, CL_strlen( WCHAR_T_CHARCODEFORMAT, ( const char * )String ), Color, FontHandle, EdgeColor, VerticalFlag ) ;
#else
	int Result ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_BEGIN( String, -1 )

	Result = DrawExtendStringFToHandle_WCHAR_T( x, y, ExRateX, ExRateY, UseStringBuffer, wchar_StrLength, Color, FontHandle, EdgeColor, VerticalFlag ) ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_END

	return Result ;
#endif
}

extern int NS_DrawExtendNStringFToHandle( float x, float y, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag )
{
#ifdef UNICODE
	return DrawExtendStringFToHandle_WCHAR_T( x, y, ExRateX, ExRateY, String, StringLength, Color, FontHandle, EdgeColor, VerticalFlag ) ;
#else
	int Result ;

	FONTHANDLE_TCHAR_WITH_STRLENGTH_TO_WCHAR_T_STRING_BEGIN( String, StringLength, -1 )

	Result = DrawExtendStringFToHandle_WCHAR_T( x, y, ExRateX, ExRateY, UseStringBuffer, wchar_StrLength, Color, FontHandle, EdgeColor, VerticalFlag ) ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_END

	return Result ;
#endif
}

extern int DrawExtendStringFToHandle_WCHAR_T( float x, float y, double ExRateX, double ExRateY, const wchar_t *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag )
{
#ifndef DX_NON_GRAPHICS
	int Ret = -1 ;
	FONTMANAGE *font ;

//	DEFAULT_FONT_HANDLE_SETUP

	if( String == NULL || String[0] == L'\0' || StringLength == 0 ||
		/* GSYS.NotDrawFlag || */ GSYS.DrawSetting.NotDrawFlagInSetDrawArea )
	{
		return 0 ;
	}

	CheckActiveState() ;

	font = GetFontManageDataToHandle_Inline( FontHandle ) ;
	if( font == NULL )
	{
		DXST_LOGFILE_ADDUTF16LE( "\xd5\x30\xa9\x30\xf3\x30\xc8\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x24\x50\x4c\x30\x70\x75\x38\x5e\x67\x30\x59\x30\x0a\x00\x00"/*@ L"フォントハンドル値が異常です\n" @*/ ) ;
		return Ret ;
	}

	// ２Ｄ行列をハードウエアに反映する
	if( GSYS.DrawSetting.MatchHardware_2DMatrix == FALSE && GSYS.Setting.ValidHardware )
		Graphics_DrawSetting_ApplyLib2DMatrixToHardware() ;

	// 描画
	DRAW_DEF(
		DrawExtendStringHardware( _FTOL( x ), _FTOL( y ), x, y, FALSE, ExRateX, ExRateY, String, StringLength, Color, font, EdgeColor, VerticalFlag ),
		DrawExtendStringSoftware( _FTOL( x ), _FTOL( y ),              ExRateX, ExRateY, String, StringLength, Color, font, EdgeColor, VerticalFlag ),
		SETDRAWRECTCODE,
		Ret,
		font->TextureCacheFlag
	)

	// 終了
	return Ret ;
#else // DX_NON_GRAPHICS
	return -1 ;
#endif // DX_NON_GRAPHICS
}

#undef SETDRAWRECTCODE



// 文字列を拡大描画する
extern int NS_DrawExtendVStringToHandle( int x, int y, double ExRateX, double ExRateY, const TCHAR *String, unsigned int Color, int FontHandle, unsigned int EdgeColor )
{
	return NS_DrawExtendStringToHandle( x, y, ExRateX, ExRateY, String, Color, FontHandle, EdgeColor, TRUE ) ;
}

// 文字列を拡大描画する
extern int NS_DrawExtendNVStringToHandle( int x, int y, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor )
{
	return NS_DrawExtendNStringToHandle( x, y, ExRateX, ExRateY, String, StringLength, Color, FontHandle, EdgeColor, TRUE ) ;
}

// 文字列を拡大描画する
extern int DrawExtendVStringToHandle_WCHAR_T( int x, int y, double ExRateX, double ExRateY, const wchar_t *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor )
{
	return DrawExtendStringToHandle_WCHAR_T( x, y, ExRateX, ExRateY, String, StringLength, Color, FontHandle, EdgeColor, TRUE ) ;
}

// 文字列を拡大描画する
extern int NS_DrawExtendVStringFToHandle( float x, float y, double ExRateX, double ExRateY, const TCHAR *String, unsigned int Color, int FontHandle, unsigned int EdgeColor )
{
	return NS_DrawExtendStringFToHandle( x, y, ExRateX, ExRateY, String, Color, FontHandle, EdgeColor, TRUE ) ;
}

// 文字列を拡大描画する
extern int NS_DrawExtendNVStringFToHandle( float x, float y, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor )
{
	return NS_DrawExtendNStringFToHandle( x, y, ExRateX, ExRateY, String, StringLength, Color, FontHandle, EdgeColor, TRUE ) ;
}

// 文字列を拡大描画する
extern int DrawExtendVStringFToHandle_WCHAR_T( float x, float y, double ExRateX, double ExRateY, const wchar_t *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor )
{
	return DrawExtendStringFToHandle_WCHAR_T( x, y, ExRateX, ExRateY, String, StringLength, Color, FontHandle, EdgeColor, TRUE ) ;
}

// 書式指定文字列を拡大描画する
extern int NS_DrawExtendFormatString( int x, int y, double ExRateX, double ExRateY, unsigned int Color, const TCHAR *FormatString, ... )
{
	int FontHandle = DX_DEFAULT_FONT_HANDLE ;

	TCHAR_FONTHANDLE_FORMATSTRING_SETUP( -1 )

	return NS_DrawExtendStringToHandle( x, y, ExRateX, ExRateY, String, Color, DX_DEFAULT_FONT_HANDLE, 0, FALSE ) ;
}

// 書式指定文字列を拡大描画する
extern int DrawExtendFormatString_WCHAR_T( int x, int y, double ExRateX, double ExRateY, unsigned int Color, const wchar_t *FormatString, ... )
{
	WCHAR_T_FORMATSTRING_SETUP

	return DrawExtendStringToHandle_WCHAR_T( x, y, ExRateX, ExRateY, String, CL_strlen( WCHAR_T_CHARCODEFORMAT, ( const char * )String ), Color, DX_DEFAULT_FONT_HANDLE, 0, FALSE ) ;
}

// 書式指定文字列を拡大描画する
extern int NS_DrawExtendFormatStringF( float x, float y, double ExRateX, double ExRateY, unsigned int Color, const TCHAR *FormatString, ... )
{
	int FontHandle = DX_DEFAULT_FONT_HANDLE ;

	TCHAR_FONTHANDLE_FORMATSTRING_SETUP( -1 )

	return NS_DrawExtendStringFToHandle( x, y, ExRateX, ExRateY, String, Color, DX_DEFAULT_FONT_HANDLE, 0, FALSE ) ;
}

// 書式指定文字列を拡大描画する
extern int DrawExtendFormatStringF_WCHAR_T( float x, float y, double ExRateX, double ExRateY, unsigned int Color, const wchar_t *FormatString, ... )
{
	WCHAR_T_FORMATSTRING_SETUP

	return DrawExtendStringFToHandle_WCHAR_T( x, y, ExRateX, ExRateY, String, CL_strlen( WCHAR_T_CHARCODEFORMAT, ( const char * )String ), Color, DX_DEFAULT_FONT_HANDLE, 0, FALSE ) ;
}

// 書式指定文字列を拡大描画する
extern int NS_DrawExtendFormatVString( int x, int y, double ExRateX, double ExRateY, unsigned int Color, const TCHAR *FormatString, ... )
{
	int FontHandle = DX_DEFAULT_FONT_HANDLE ;

	TCHAR_FONTHANDLE_FORMATSTRING_SETUP( -1 )

	return NS_DrawExtendStringToHandle( x, y, ExRateX, ExRateY, String, Color, DX_DEFAULT_FONT_HANDLE, Color, TRUE ) ;
}

// 書式指定文字列を拡大描画する
extern int DrawExtendFormatVString_WCHAR_T( int x, int y, double ExRateX, double ExRateY, unsigned int Color, const wchar_t *FormatString, ... )
{
	WCHAR_T_FORMATSTRING_SETUP

	return DrawExtendStringToHandle_WCHAR_T( x, y, ExRateX, ExRateY, String, CL_strlen( WCHAR_T_CHARCODEFORMAT, ( const char * )String ), Color, DX_DEFAULT_FONT_HANDLE, Color, TRUE ) ;
}

// 書式指定文字列を拡大描画する
extern int NS_DrawExtendFormatVStringF( float x, float y, double ExRateX, double ExRateY, unsigned int Color, const TCHAR *FormatString, ... )
{
	int FontHandle = DX_DEFAULT_FONT_HANDLE ;

	TCHAR_FONTHANDLE_FORMATSTRING_SETUP( -1 )

	return NS_DrawExtendStringFToHandle( x, y, ExRateX, ExRateY, String, Color, DX_DEFAULT_FONT_HANDLE, Color, TRUE ) ;
}

// 書式指定文字列を拡大描画する
extern int DrawExtendFormatVStringF_WCHAR_T( float x, float y, double ExRateX, double ExRateY, unsigned int Color, const wchar_t *FormatString, ... )
{
	WCHAR_T_FORMATSTRING_SETUP

	return DrawExtendStringFToHandle_WCHAR_T( x, y, ExRateX, ExRateY, String, CL_strlen( WCHAR_T_CHARCODEFORMAT, ( const char * )String ), Color, DX_DEFAULT_FONT_HANDLE, Color, TRUE ) ;
}

// 書式指定文字列を拡大描画する
extern int NS_DrawExtendFormatStringToHandle( int x, int y, double ExRateX, double ExRateY, unsigned int Color, int FontHandle, const TCHAR *FormatString, ... )
{
	TCHAR_FONTHANDLE_FORMATSTRING_SETUP( -1 )

	// 描画する
	return NS_DrawExtendStringToHandle( x, y, ExRateX, ExRateY, String, Color, FontHandle, 0, FALSE ) ;
}

// 書式指定文字列を拡大描画する
extern int DrawExtendFormatStringToHandle_WCHAR_T( int x, int y, double ExRateX, double ExRateY, unsigned int Color, int FontHandle, const wchar_t *FormatString, ... )
{
	WCHAR_T_FORMATSTRING_SETUP

	// 描画する
	return DrawExtendStringToHandle_WCHAR_T( x, y, ExRateX, ExRateY, String, CL_strlen( WCHAR_T_CHARCODEFORMAT, ( const char * )String ), Color, FontHandle, 0, FALSE ) ;
}

// 書式指定文字列を拡大描画する
extern int NS_DrawExtendFormatStringFToHandle( float x, float y, double ExRateX, double ExRateY, unsigned int Color, int FontHandle, const TCHAR *FormatString, ... )
{
	TCHAR_FONTHANDLE_FORMATSTRING_SETUP( -1 )

	// 描画する
	return NS_DrawExtendStringFToHandle( x, y, ExRateX, ExRateY, String, Color, FontHandle, 0, FALSE ) ;
}

// 書式指定文字列を拡大描画する
extern int DrawExtendFormatStringFToHandle_WCHAR_T( float x, float y, double ExRateX, double ExRateY, unsigned int Color, int FontHandle, const wchar_t *FormatString, ... )
{
	WCHAR_T_FORMATSTRING_SETUP

	// 描画する
	return DrawExtendStringFToHandle_WCHAR_T( x, y, ExRateX, ExRateY, String, CL_strlen( WCHAR_T_CHARCODEFORMAT, ( const char * )String ), Color, FontHandle, 0, FALSE ) ;
}

// 書式指定文字列を拡大描画する
extern int NS_DrawExtendFormatVStringToHandle( int x, int y, double ExRateX, double ExRateY, unsigned int Color, int FontHandle, const TCHAR *FormatString, ... )
{
	TCHAR_FONTHANDLE_FORMATSTRING_SETUP( -1 )

	// 描画する
	return NS_DrawExtendStringToHandle( x, y, ExRateX, ExRateY, String, Color, FontHandle, 0, TRUE ) ;
}

// 書式指定文字列を拡大描画する
extern int DrawExtendFormatVStringToHandle_WCHAR_T( int x, int y, double ExRateX, double ExRateY, unsigned int Color, int FontHandle, const wchar_t *FormatString, ... )
{
	WCHAR_T_FORMATSTRING_SETUP

	// 描画する
	return DrawExtendStringToHandle_WCHAR_T( x, y, ExRateX, ExRateY, String, CL_strlen( WCHAR_T_CHARCODEFORMAT, ( const char * )String ), Color, FontHandle, 0, TRUE ) ;
}

// 書式指定文字列を拡大描画する
extern int NS_DrawExtendFormatVStringFToHandle( float x, float y, double ExRateX, double ExRateY, unsigned int Color, int FontHandle, const TCHAR *FormatString, ... )
{
	TCHAR_FONTHANDLE_FORMATSTRING_SETUP( -1 )

	// 描画する
	return NS_DrawExtendStringFToHandle( x, y, ExRateX, ExRateY, String, Color, FontHandle, 0, TRUE ) ;
}

// 書式指定文字列を拡大描画する
extern int DrawExtendFormatVStringFToHandle_WCHAR_T( float x, float y, double ExRateX, double ExRateY, unsigned int Color, int FontHandle, const wchar_t *FormatString, ... )
{
	WCHAR_T_FORMATSTRING_SETUP

	// 描画する
	return DrawExtendStringFToHandle_WCHAR_T( x, y, ExRateX, ExRateY, String, CL_strlen( WCHAR_T_CHARCODEFORMAT, ( const char * )String ), Color, FontHandle, 0, TRUE ) ;
}



// デフォルトフォントハンドルを使用して文字列を回転描画する
extern int NS_DrawRotaString( int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, unsigned int EdgeColor , int VerticalFlag , const TCHAR *String )
{
	return NS_DrawRotaStringToHandle( x, y, ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, VerticalFlag, String ) ;
}

// デフォルトフォントハンドルを使用して文字列を回転描画する
extern int NS_DrawRotaNString( int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, unsigned int EdgeColor , int VerticalFlag , const TCHAR *String, size_t StringLength )
{
	return NS_DrawRotaNStringToHandle( x, y, ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, VerticalFlag, String, StringLength ) ;
}

// デフォルトフォントハンドルを使用して文字列を回転描画する
extern int DrawRotaString_WCHAR_T( int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, unsigned int EdgeColor , int VerticalFlag , const wchar_t *String, size_t StringLength )
{
	return DrawRotaStringToHandle_WCHAR_T( x, y, ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, VerticalFlag, String, StringLength ) ;
}

// デフォルトフォントハンドルを使用して文字列を回転描画する
extern int NS_DrawRotaStringF( float x, float y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, unsigned int EdgeColor , int VerticalFlag , const TCHAR *String )
{
	return NS_DrawRotaStringFToHandle( x, y, ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, VerticalFlag, String ) ;
}

// デフォルトフォントハンドルを使用して文字列を回転描画する
extern int NS_DrawRotaNStringF( float x, float y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, unsigned int EdgeColor , int VerticalFlag , const TCHAR *String, size_t StringLength )
{
	return NS_DrawRotaNStringFToHandle( x, y, ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, VerticalFlag, String, StringLength ) ;
}

// デフォルトフォントハンドルを使用して文字列を回転描画する
extern int DrawRotaStringF_WCHAR_T( float x, float y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, unsigned int EdgeColor , int VerticalFlag , const wchar_t *String, size_t StringLength )
{
	return DrawRotaStringFToHandle_WCHAR_T( x, y, ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, VerticalFlag, String, StringLength ) ;
}

// デフォルトフォントハンドルを使用して文字列を回転描画する
#define SETDRAWRECTCODE\
	DrawRect = GSYS.DrawSetting.DrawArea ;

extern int NS_DrawRotaStringToHandle( int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, int FontHandle, unsigned int EdgeColor , int VerticalFlag , const TCHAR *String )
{
#ifdef UNICODE
	return DrawRotaStringToHandle_WCHAR_T( x, y, ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle, Color, FontHandle, EdgeColor, VerticalFlag , String, CL_strlen( WCHAR_T_CHARCODEFORMAT, ( const char * )String ) ) ;
#else
	int Result ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_BEGIN( String, -1 )

	Result = DrawRotaStringToHandle_WCHAR_T( x, y, ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle, Color, FontHandle, EdgeColor, VerticalFlag , UseStringBuffer, wchar_StrLength ) ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_END

	return Result ;
#endif
}

extern int NS_DrawRotaNStringToHandle( int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, int FontHandle, unsigned int EdgeColor , int VerticalFlag , const TCHAR *String, size_t StringLength )
{
#ifdef UNICODE
	return DrawRotaStringToHandle_WCHAR_T( x, y, ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle, Color, FontHandle, EdgeColor, VerticalFlag , String, StringLength ) ;
#else
	int Result ;

	FONTHANDLE_TCHAR_WITH_STRLENGTH_TO_WCHAR_T_STRING_BEGIN( String, StringLength, -1 )

	Result = DrawRotaStringToHandle_WCHAR_T( x, y, ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle, Color, FontHandle, EdgeColor, VerticalFlag , UseStringBuffer, wchar_StrLength ) ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_END

	return Result ;
#endif
}

extern int DrawRotaStringToHandle_WCHAR_T( int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, int FontHandle, unsigned int EdgeColor , int VerticalFlag , const wchar_t *String, size_t StringLength )
{
#ifndef DX_NON_GRAPHICS
	int Ret = -1 ;
	int Flag ;
	FONTMANAGE *font ;

//	DEFAULT_FONT_HANDLE_SETUP

	if( String == NULL || String[0] == L'\0' || StringLength == 0 ||
		/* GSYS.NotDrawFlag || */ GSYS.DrawSetting.NotDrawFlagInSetDrawArea )
	{
		return 0 ;
	}

	CheckActiveState() ;

	font = GetFontManageDataToHandle_Inline( FontHandle ) ;
	if( font == NULL )
	{
		DXST_LOGFILE_ADDUTF16LE( "\xd5\x30\xa9\x30\xf3\x30\xc8\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x24\x50\x4c\x30\x70\x75\x38\x5e\x67\x30\x59\x30\x0a\x00\x00"/*@ L"フォントハンドル値が異常です\n" @*/ ) ;
		return Ret ;
	}

	Flag = font->TextureCacheFlag ;

	// ２Ｄ行列をハードウエアに反映する
	if( GSYS.DrawSetting.MatchHardware_2DMatrix == FALSE && GSYS.Setting.ValidHardware )
		Graphics_DrawSetting_ApplyLib2DMatrixToHardware() ;

	// 描画
	DRAW_DEF(
		DrawRotaStringHardware( x, y, 0, 0, TRUE, ExRateX, ExRateY, RotCenterX, RotCenterY, RotAngle, String, StringLength, Color, font, EdgeColor, VerticalFlag ),
		DrawRotaStringSoftware( x, y,             ExRateX, ExRateY, RotCenterX, RotCenterY, RotAngle, String, StringLength, Color, font, EdgeColor, VerticalFlag ),
		SETDRAWRECTCODE,
		Ret,
		Flag
	)

	// 終了
	return Ret ;
#else // DX_NON_GRAPHICS
	return -1 ;
#endif // DX_NON_GRAPHICS
}

#undef SETDRAWRECTCODE

// デフォルトフォントハンドルを使用して文字列を回転描画する
#define SETDRAWRECTCODE\
	DrawRect = GSYS.DrawSetting.DrawArea ;

extern int NS_DrawRotaStringFToHandle( float x, float y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, int FontHandle, unsigned int EdgeColor , int VerticalFlag , const TCHAR *String )
{
#ifdef UNICODE
	return DrawRotaStringFToHandle_WCHAR_T( x, y, ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle, Color, FontHandle, EdgeColor, VerticalFlag , String, CL_strlen( WCHAR_T_CHARCODEFORMAT, ( const char * )String ) ) ;
#else
	int Result ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_BEGIN( String, -1 )

	Result = DrawRotaStringFToHandle_WCHAR_T( x, y, ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle, Color, FontHandle, EdgeColor, VerticalFlag , UseStringBuffer, wchar_StrLength ) ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_END

	return Result ;
#endif
}

extern int NS_DrawRotaNStringFToHandle( float x, float y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, int FontHandle, unsigned int EdgeColor , int VerticalFlag , const TCHAR *String, size_t StringLength )
{
#ifdef UNICODE
	return DrawRotaStringFToHandle_WCHAR_T( x, y, ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle, Color, FontHandle, EdgeColor, VerticalFlag , String, StringLength ) ;
#else
	int Result ;

	FONTHANDLE_TCHAR_WITH_STRLENGTH_TO_WCHAR_T_STRING_BEGIN( String, StringLength, -1 )

	Result = DrawRotaStringFToHandle_WCHAR_T( x, y, ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle, Color, FontHandle, EdgeColor, VerticalFlag , UseStringBuffer, wchar_StrLength ) ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_END

	return Result ;
#endif
}

extern int DrawRotaStringFToHandle_WCHAR_T( float x, float y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, int FontHandle, unsigned int EdgeColor , int VerticalFlag , const wchar_t *String, size_t StringLength )
{
#ifndef DX_NON_GRAPHICS
	int Ret = -1 ;
	int Flag ;
	FONTMANAGE *font ;

//	DEFAULT_FONT_HANDLE_SETUP

	if( String == NULL || String[0] == L'\0' || StringLength == 0 ||
		/* GSYS.NotDrawFlag || */ GSYS.DrawSetting.NotDrawFlagInSetDrawArea )
	{
		return 0 ;
	}

	CheckActiveState() ;

	font = GetFontManageDataToHandle_Inline( FontHandle ) ;
	if( font == NULL )
	{
		DXST_LOGFILE_ADDUTF16LE( "\xd5\x30\xa9\x30\xf3\x30\xc8\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x24\x50\x4c\x30\x70\x75\x38\x5e\x67\x30\x59\x30\x0a\x00\x00"/*@ L"フォントハンドル値が異常です\n" @*/ ) ;
		return Ret ;
	}

	Flag = font->TextureCacheFlag ;

	// ２Ｄ行列をハードウエアに反映する
	if( GSYS.DrawSetting.MatchHardware_2DMatrix == FALSE && GSYS.Setting.ValidHardware )
		Graphics_DrawSetting_ApplyLib2DMatrixToHardware() ;

	// 描画
	DRAW_DEF(
		DrawRotaStringHardware(          0,          0, x, y, FALSE, ExRateX, ExRateY, RotCenterX, RotCenterY, RotAngle, String, StringLength, Color, font, EdgeColor, VerticalFlag ),
		DrawRotaStringSoftware( _FTOL( x ), _FTOL( y ),              ExRateX, ExRateY, RotCenterX, RotCenterY, RotAngle, String, StringLength, Color, font, EdgeColor, VerticalFlag ),
		SETDRAWRECTCODE,
		Ret,
		Flag
	)

	// 終了
	return Ret ;
#else // DX_NON_GRAPHICS
	return -1 ;
#endif // DX_NON_GRAPHICS
}

#undef SETDRAWRECTCODE

// デフォルトフォントハンドルを使用して書式指定文字列を回転描画する
extern int NS_DrawRotaFormatString( int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, unsigned int EdgeColor , int VerticalFlag , const TCHAR *FormatString , ... )
{
	int FontHandle = DX_DEFAULT_FONT_HANDLE ;

	TCHAR_FONTHANDLE_FORMATSTRING_SETUP( -1 )

	return NS_DrawRotaStringToHandle( x, y, ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, VerticalFlag, String ) ;
}

// デフォルトフォントハンドルを使用して書式指定文字列を回転描画する
extern int DrawRotaFormatString_WCHAR_T( int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, unsigned int EdgeColor , int VerticalFlag , const wchar_t *FormatString , ... )
{
	WCHAR_T_FORMATSTRING_SETUP

	return DrawRotaStringToHandle_WCHAR_T( x, y, ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, VerticalFlag, String, CL_strlen( WCHAR_T_CHARCODEFORMAT, ( const char * )String ) ) ;
}

// デフォルトフォントハンドルを使用して書式指定文字列を回転描画する
extern int NS_DrawRotaFormatStringF( float x, float y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, unsigned int EdgeColor , int VerticalFlag , const TCHAR *FormatString , ... )
{
	int FontHandle = DX_DEFAULT_FONT_HANDLE ;

	TCHAR_FONTHANDLE_FORMATSTRING_SETUP( -1 )

	return NS_DrawRotaStringFToHandle( x, y, ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, VerticalFlag, String ) ;
}

// デフォルトフォントハンドルを使用して書式指定文字列を回転描画する
extern int DrawRotaFormatStringF_WCHAR_T( float x, float y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, unsigned int EdgeColor , int VerticalFlag , const wchar_t *FormatString , ... )
{
	WCHAR_T_FORMATSTRING_SETUP

	return DrawRotaStringFToHandle_WCHAR_T( x, y, ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, VerticalFlag, String, CL_strlen( WCHAR_T_CHARCODEFORMAT, ( const char * )String ) ) ;
}

// フォントハンドルを使用して書式指定文字列を回転描画する
extern int NS_DrawRotaFormatStringToHandle(	int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag, const TCHAR *FormatString, ... )
{
	TCHAR_FONTHANDLE_FORMATSTRING_SETUP( -1 )

	// 描画する
	return NS_DrawRotaStringToHandle( x, y, ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle, Color, FontHandle, EdgeColor, VerticalFlag, String ) ;
}

// フォントハンドルを使用して書式指定文字列を回転描画する
extern int DrawRotaFormatStringToHandle_WCHAR_T(	int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag, const wchar_t *FormatString, ... )
{
	WCHAR_T_FORMATSTRING_SETUP

	// 描画する
	return DrawRotaStringToHandle_WCHAR_T( x, y, ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle, Color, FontHandle, EdgeColor, VerticalFlag, String, CL_strlen( WCHAR_T_CHARCODEFORMAT, ( const char * )String ) ) ;
}

// フォントハンドルを使用して書式指定文字列を回転描画する
extern int NS_DrawRotaFormatStringFToHandle( float x, float y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag, const TCHAR *FormatString, ... )
{
	TCHAR_FONTHANDLE_FORMATSTRING_SETUP( -1 )

	// 描画する
	return NS_DrawRotaStringFToHandle( x, y, ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle, Color, FontHandle, EdgeColor, VerticalFlag, String ) ;
}

// フォントハンドルを使用して書式指定文字列を回転描画する
extern int DrawRotaFormatStringFToHandle_WCHAR_T(	float x, float y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag, const wchar_t *FormatString, ... )
{
	WCHAR_T_FORMATSTRING_SETUP

	// 描画する
	return DrawRotaStringFToHandle_WCHAR_T( x, y, ExRateX,  ExRateY, RotCenterX, RotCenterY, RotAngle, Color, FontHandle, EdgeColor, VerticalFlag, String, CL_strlen( WCHAR_T_CHARCODEFORMAT, ( const char * )String ) ) ;
}





// デフォルトフォントハンドルを使用して文字列を変形描画する
extern int NS_DrawModiString( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int Color, unsigned int EdgeColor, int VerticalFlag, const TCHAR *String )
{
	return NS_DrawModiStringToHandle( x1, y1, x2, y2, x3, y3, x4, y4, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, VerticalFlag, String ) ;
}

// デフォルトフォントハンドルを使用して文字列を変形描画する
extern int NS_DrawModiNString( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int Color, unsigned int EdgeColor, int VerticalFlag, const TCHAR *String, size_t StringLength )
{
	return NS_DrawModiNStringToHandle( x1, y1, x2, y2, x3, y3, x4, y4, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, VerticalFlag, String, StringLength ) ;
}

// デフォルトフォントハンドルを使用して文字列を変形描画する
extern int DrawModiString_WCHAR_T( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int Color, unsigned int EdgeColor , int VerticalFlag , const wchar_t *String, size_t StringLength )
{
	return DrawModiStringToHandle_WCHAR_T( x1, y1, x2, y2, x3, y3, x4, y4, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, VerticalFlag, String, StringLength ) ;
}

// デフォルトフォントハンドルを使用して文字列を変形描画する
extern int NS_DrawModiStringF( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int Color, unsigned int EdgeColor , int VerticalFlag , const TCHAR *String )
{
	return NS_DrawModiStringFToHandle( x1, y1, x2, y2, x3, y3, x4, y4, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, VerticalFlag, String ) ;
}

// デフォルトフォントハンドルを使用して文字列を変形描画する
extern int NS_DrawModiNStringF( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int Color, unsigned int EdgeColor , int VerticalFlag , const TCHAR *String, size_t StringLength )
{
	return NS_DrawModiNStringFToHandle( x1, y1, x2, y2, x3, y3, x4, y4, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, VerticalFlag, String, StringLength ) ;
}

// デフォルトフォントハンドルを使用して文字列を変形描画する
extern int DrawModiStringF_WCHAR_T( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int Color, unsigned int EdgeColor , int VerticalFlag , const wchar_t *String, size_t StringLength )
{
	return DrawModiStringFToHandle_WCHAR_T( x1, y1, x2, y2, x3, y3, x4, y4, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, VerticalFlag, String, StringLength ) ;
}

// デフォルトフォントハンドルを使用して文字列を変形描画する
#define SETDRAWRECTCODE\
	DrawRect = GSYS.DrawSetting.DrawArea ;

extern int NS_DrawModiStringToHandle( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int Color, int FontHandle, unsigned int EdgeColor , int VerticalFlag , const TCHAR *String )
{
#ifdef UNICODE
	return DrawModiStringToHandle_WCHAR_T( x1, y1, x2, y2, x3, y3, x4, y4, Color, FontHandle, EdgeColor, VerticalFlag , String, CL_strlen( WCHAR_T_CHARCODEFORMAT, ( const char * )String ) ) ;
#else
	int Result ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_BEGIN( String, -1 )

	Result = DrawModiStringToHandle_WCHAR_T( x1, y1, x2, y2, x3, y3, x4, y4, Color, FontHandle, EdgeColor, VerticalFlag , UseStringBuffer, wchar_StrLength ) ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_END

	return Result ;
#endif
}

extern int NS_DrawModiNStringToHandle( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int Color, int FontHandle, unsigned int EdgeColor , int VerticalFlag , const TCHAR *String, size_t StringLength )
{
#ifdef UNICODE
	return DrawModiStringToHandle_WCHAR_T( x1, y1, x2, y2, x3, y3, x4, y4, Color, FontHandle, EdgeColor, VerticalFlag , String, StringLength ) ;
#else
	int Result ;

	FONTHANDLE_TCHAR_WITH_STRLENGTH_TO_WCHAR_T_STRING_BEGIN( String, StringLength, -1 )

	Result = DrawModiStringToHandle_WCHAR_T( x1, y1, x2, y2, x3, y3, x4, y4, Color, FontHandle, EdgeColor, VerticalFlag , UseStringBuffer, wchar_StrLength ) ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_END

	return Result ;
#endif
}

extern int DrawModiStringToHandle_WCHAR_T( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int Color, int FontHandle, unsigned int EdgeColor , int VerticalFlag , const wchar_t *String, size_t StringLength )
{
#ifndef DX_NON_GRAPHICS
	int Ret = -1 ;
	int Flag ;
	FONTMANAGE *font ;

//	DEFAULT_FONT_HANDLE_SETUP

	if( String == NULL || String[0] == L'\0' || StringLength == 0 ||
		/* GSYS.NotDrawFlag || */ GSYS.DrawSetting.NotDrawFlagInSetDrawArea )
	{
		return 0 ;
	}

	CheckActiveState() ;

	font = GetFontManageDataToHandle_Inline( FontHandle ) ;
	if( font == NULL )
	{
		DXST_LOGFILE_ADDUTF16LE( "\xd5\x30\xa9\x30\xf3\x30\xc8\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x24\x50\x4c\x30\x70\x75\x38\x5e\x67\x30\x59\x30\x0a\x00\x00"/*@ L"フォントハンドル値が異常です\n" @*/ ) ;
		return Ret ;
	}

	Flag = font->TextureCacheFlag ;

	// ２Ｄ行列をハードウエアに反映する
	if( GSYS.DrawSetting.MatchHardware_2DMatrix == FALSE && GSYS.Setting.ValidHardware )
		Graphics_DrawSetting_ApplyLib2DMatrixToHardware() ;

	// 描画
	DRAW_DEF(
		DrawModiStringHardware( x1, y1, x2, y2, x3, y3, x4, y4, 0, 0, 0, 0, 0, 0, 0, 0, TRUE, String, StringLength, Color, font, EdgeColor, VerticalFlag ),
		DrawModiStringSoftware( x1, y1, x2, y2, x3, y3, x4, y4,                               String, StringLength, Color, font, EdgeColor, VerticalFlag ),
		SETDRAWRECTCODE,
		Ret,
		Flag
	)

	// 終了
	return Ret ;
#else // DX_NON_GRAPHICS
	return -1 ;
#endif // DX_NON_GRAPHICS
}

#undef SETDRAWRECTCODE

// デフォルトフォントハンドルを使用して文字列を変形描画する
#define SETDRAWRECTCODE\
	DrawRect = GSYS.DrawSetting.DrawArea ;

extern int NS_DrawModiStringFToHandle( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int Color, int FontHandle, unsigned int EdgeColor , int VerticalFlag , const TCHAR *String )
{
#ifdef UNICODE
	return DrawModiStringFToHandle_WCHAR_T( x1, y1, x2, y2, x3, y3, x4, y4, Color, FontHandle, EdgeColor, VerticalFlag , String, CL_strlen( WCHAR_T_CHARCODEFORMAT, ( const char * )String ) ) ;
#else
	int Result ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_BEGIN( String, -1 )

	Result = DrawModiStringFToHandle_WCHAR_T( x1, y1, x2, y2, x3, y3, x4, y4, Color, FontHandle, EdgeColor, VerticalFlag , UseStringBuffer, wchar_StrLength ) ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_END

	return Result ;
#endif
}

extern int NS_DrawModiNStringFToHandle( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int Color, int FontHandle, unsigned int EdgeColor , int VerticalFlag , const TCHAR *String, size_t StringLength )
{
#ifdef UNICODE
	return DrawModiStringFToHandle_WCHAR_T( x1, y1, x2, y2, x3, y3, x4, y4, Color, FontHandle, EdgeColor, VerticalFlag , String, StringLength ) ;
#else
	int Result ;

	FONTHANDLE_TCHAR_WITH_STRLENGTH_TO_WCHAR_T_STRING_BEGIN( String, StringLength, -1 )

	Result = DrawModiStringFToHandle_WCHAR_T( x1, y1, x2, y2, x3, y3, x4, y4, Color, FontHandle, EdgeColor, VerticalFlag , UseStringBuffer, wchar_StrLength ) ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_END

	return Result ;
#endif
}

extern int DrawModiStringFToHandle_WCHAR_T( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int Color, int FontHandle, unsigned int EdgeColor , int VerticalFlag , const wchar_t *String, size_t StringLength )
{
#ifndef DX_NON_GRAPHICS
	int Ret = -1 ;
	int Flag ;
	FONTMANAGE *font ;

//	DEFAULT_FONT_HANDLE_SETUP

	if( String == NULL || String[0] == L'\0' || StringLength == 0 ||
		/* GSYS.NotDrawFlag || */ GSYS.DrawSetting.NotDrawFlagInSetDrawArea )
	{
		return 0 ;
	}

	CheckActiveState() ;

	font = GetFontManageDataToHandle_Inline( FontHandle ) ;
	if( font == NULL )
	{
		DXST_LOGFILE_ADDUTF16LE( "\xd5\x30\xa9\x30\xf3\x30\xc8\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x24\x50\x4c\x30\x70\x75\x38\x5e\x67\x30\x59\x30\x0a\x00\x00"/*@ L"フォントハンドル値が異常です\n" @*/ ) ;
		return Ret ;
	}

	Flag = font->TextureCacheFlag ;

	// ２Ｄ行列をハードウエアに反映する
	if( GSYS.DrawSetting.MatchHardware_2DMatrix == FALSE && GSYS.Setting.ValidHardware )
		Graphics_DrawSetting_ApplyLib2DMatrixToHardware() ;

	// 描画
	DRAW_DEF(
		DrawModiStringHardware(           0,           0,           0,           0,           0,           0,           0,           0, x1, y1, x2, y2, x3, y3, x4, y4, FALSE, String, StringLength, Color, font, EdgeColor, VerticalFlag ),
		DrawModiStringSoftware( _FTOL( x1 ), _FTOL( y1 ), _FTOL( x2 ), _FTOL( y2 ), _FTOL( x3 ), _FTOL( y3 ), _FTOL( x4 ), _FTOL( y4 ),                                        String, StringLength, Color, font, EdgeColor, VerticalFlag ),
		SETDRAWRECTCODE,
		Ret,
		Flag
	)

	// 終了
	return Ret ;
#else // DX_NON_GRAPHICS
	return -1 ;
#endif // DX_NON_GRAPHICS
}

#undef SETDRAWRECTCODE

// デフォルトフォントハンドルを使用して書式指定文字列を変形描画する
extern int NS_DrawModiFormatString( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int Color, unsigned int EdgeColor , int VerticalFlag , const TCHAR *FormatString , ... )
{
	int FontHandle = DX_DEFAULT_FONT_HANDLE ;

	TCHAR_FONTHANDLE_FORMATSTRING_SETUP( -1 )

	return NS_DrawModiStringToHandle( x1, y1, x2, y2, x3, y3, x4, y4, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, VerticalFlag, String ) ;
}

// デフォルトフォントハンドルを使用して書式指定文字列を変形描画する
extern int DrawModiFormatString_WCHAR_T( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int Color, unsigned int EdgeColor , int VerticalFlag , const wchar_t *FormatString , ... )
{
	WCHAR_T_FORMATSTRING_SETUP

	return DrawModiStringToHandle_WCHAR_T( x1, y1, x2, y2, x3, y3, x4, y4, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, VerticalFlag, String, CL_strlen( WCHAR_T_CHARCODEFORMAT, ( const char * )String ) ) ;
}

// デフォルトフォントハンドルを使用して書式指定文字列を変形描画する
extern int NS_DrawModiFormatStringF( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int Color, unsigned int EdgeColor , int VerticalFlag , const TCHAR *FormatString , ... )
{
	int FontHandle = DX_DEFAULT_FONT_HANDLE ;

	TCHAR_FONTHANDLE_FORMATSTRING_SETUP( -1 )

	return NS_DrawModiStringFToHandle( x1, y1, x2, y2, x3, y3, x4, y4, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, VerticalFlag, String ) ;
}

// デフォルトフォントハンドルを使用して書式指定文字列を変形描画する
extern int DrawModiFormatStringF_WCHAR_T( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int Color, unsigned int EdgeColor , int VerticalFlag , const wchar_t *FormatString , ... )
{
	WCHAR_T_FORMATSTRING_SETUP

	return DrawModiStringFToHandle_WCHAR_T( x1, y1, x2, y2, x3, y3, x4, y4, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor, VerticalFlag, String, CL_strlen( WCHAR_T_CHARCODEFORMAT, ( const char * )String ) ) ;
}

// フォントハンドルを使用して書式指定文字列を変形描画する
extern int NS_DrawModiFormatStringToHandle(	int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag, const TCHAR *FormatString, ... )
{
	TCHAR_FONTHANDLE_FORMATSTRING_SETUP( -1 )

	// 描画する
	return NS_DrawModiStringToHandle( x1, y1, x2, y2, x3, y3, x4, y4, Color, FontHandle, EdgeColor, VerticalFlag, String ) ;
}

// フォントハンドルを使用して書式指定文字列を変形描画する
extern int DrawModiFormatStringToHandle_WCHAR_T(	int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag, const wchar_t *FormatString, ... )
{
	WCHAR_T_FORMATSTRING_SETUP

	// 描画する
	return DrawModiStringToHandle_WCHAR_T( x1, y1, x2, y2, x3, y3, x4, y4, Color, FontHandle, EdgeColor, VerticalFlag, String, CL_strlen( WCHAR_T_CHARCODEFORMAT, ( const char * )String ) ) ;
}

// フォントハンドルを使用して書式指定文字列を変形描画する
extern int NS_DrawModiFormatStringFToHandle( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag, const TCHAR *FormatString, ... )
{
	TCHAR_FONTHANDLE_FORMATSTRING_SETUP( -1 )

	// 描画する
	return NS_DrawModiStringFToHandle( x1, y1, x2, y2, x3, y3, x4, y4, Color, FontHandle, EdgeColor, VerticalFlag, String ) ;
}

// フォントハンドルを使用して書式指定文字列を変形描画する
extern int DrawModiFormatStringFToHandle_WCHAR_T(	float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag, const wchar_t *FormatString, ... )
{
	WCHAR_T_FORMATSTRING_SETUP

	// 描画する
	return DrawModiStringFToHandle_WCHAR_T( x1, y1, x2, y2, x3, y3, x4, y4, Color, FontHandle, EdgeColor, VerticalFlag, String, CL_strlen( WCHAR_T_CHARCODEFORMAT, ( const char * )String ) ) ;
}







// 整数型の数値を描画する
extern int NS_DrawNumberToI( int x, int y, int Num, int RisesNum, unsigned int Color ,unsigned int EdgeColor )
{
	return NS_DrawNumberToIToHandle( x, y, Num, RisesNum, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor ) ;
}

// 浮動小数点型の数値を描画する
extern int NS_DrawNumberToF( int x, int y, double Num, int Length, unsigned int Color ,unsigned int EdgeColor  )
{
	return NS_DrawNumberToFToHandle( x, y, Num, Length, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor ) ;
}

// 整数型の数値とその説明の文字列を一度に描画する
extern int NS_DrawNumberPlusToI( int x, int y, const TCHAR *NoteString, int Num, int RisesNum, unsigned int Color ,unsigned int EdgeColor )
{
	return NS_DrawNumberPlusToIToHandle( x, y, NoteString, Num, RisesNum, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor ) ;
}

// 整数型の数値とその説明の文字列を一度に描画する
extern int DrawNumberPlusToI_WCHAR_T( int x, int y, const wchar_t *NoteString, int Num, int RisesNum, unsigned int Color ,unsigned int EdgeColor )
{
	return DrawNumberPlusToIToHandle_WCHAR_T( x, y, NoteString, Num, RisesNum, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor ) ;
}

// 浮動小数点型の数値とその説明の文字列を一度に描画する
extern int NS_DrawNumberPlusToF( int x, int y, const TCHAR *NoteString, double Num, int Length, unsigned int Color ,unsigned int EdgeColor )
{
	return NS_DrawNumberPlusToFToHandle( x, y, NoteString, Num, Length, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor ) ;
}

// 浮動小数点型の数値とその説明の文字列を一度に描画する
extern int DrawNumberPlusToF_WCHAR_T( int x, int y, const wchar_t *NoteString, double Num, int Length, unsigned int Color ,unsigned int EdgeColor )
{
	return DrawNumberPlusToFToHandle_WCHAR_T( x, y, NoteString, Num, Length, Color, DX_DEFAULT_FONT_HANDLE, EdgeColor ) ;
}



// 整数型の数値を描画する
extern int NS_DrawNumberToIToHandle( int x, int y, int Num, int RisesNum, unsigned int Color, int FontHandle ,unsigned int EdgeColor )
{
	wchar_t Str[256] ;

	_ITOAW_S( Num, Str, sizeof( Str ), RisesNum ) ;
	return DrawStringToHandle_WCHAR_T( x, y, Str, _WCSLEN( Str ), Color, FontHandle, EdgeColor, FALSE ) ;
}

// 浮動小数点型の数値を描画する
extern int NS_DrawNumberToFToHandle( int x, int y, double Num, int Length, unsigned int Color, int FontHandle,unsigned int EdgeColor  )
{
	wchar_t Str[256], Str2[256] ;

	_WCSCPY( Str2, L"%." ) ;
	_ITOAW_S( Length, Str, sizeof( Str ), 10 ) ;
	_WCSCAT_S( Str2, sizeof( Str2 ), Str ) ;
	_WCSCAT_S( Str2, sizeof( Str2 ), L"f" ) ;
	_SWNPRINTF( Str, sizeof( Str ) / 2, Str2, Num ) ;
	return DrawStringToHandle_WCHAR_T( x, y, Str, _WCSLEN( Str ), Color, FontHandle, EdgeColor, FALSE ) ;
}


// 整数型の数値とその説明の文字列を一度に描画する
extern int NS_DrawNumberPlusToIToHandle( int x, int y, const TCHAR *NoteString, int Num, int RisesNum, unsigned int Color, int FontHandle,unsigned int EdgeColor )
{
#ifdef UNICODE
	return DrawNumberPlusToIToHandle_WCHAR_T( x, y, NoteString, Num, RisesNum, Color, FontHandle, EdgeColor ) ;
#else
	int Result ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_BEGIN( NoteString, -1 )

	Result = DrawNumberPlusToIToHandle_WCHAR_T( x, y, UseStringBuffer, Num, RisesNum, Color, FontHandle, EdgeColor ) ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_END

	return Result ;
#endif
}

// 整数型の数値とその説明の文字列を一度に描画する
extern int DrawNumberPlusToIToHandle_WCHAR_T( int x, int y, const wchar_t *NoteString, int Num, int RisesNum, unsigned int Color, int FontHandle,unsigned int EdgeColor )
{
	wchar_t Str[2048], Str2[256] ;

	if( ( int )_WCSLEN( NoteString ) > 2048 - 256 )
	{
		return -1 ;
	}

	_WCSCPY_S( Str, sizeof( Str ), NoteString ) ;
	_WCSCAT_S( Str, sizeof( Str ), L"=" ) ;

	_ITOAW_S( Num, Str2, sizeof( Str2 ), RisesNum ) ;
	_WCSCAT_S( Str, sizeof( Str ), Str2 ) ;

	// 描画
	return DrawStringToHandle_WCHAR_T( x, y, Str, _WCSLEN( Str ), Color, FontHandle, EdgeColor, FALSE ) ;
}

// 浮動小数点型の数値とその説明の文字列を一度に描画する
extern int NS_DrawNumberPlusToFToHandle( int x, int y, const TCHAR *NoteString, double Num, int Length, unsigned int Color, int FontHandle ,unsigned int EdgeColor )
{
#ifdef UNICODE
	return DrawNumberPlusToFToHandle_WCHAR_T( x, y, NoteString, Num, Length, Color, FontHandle, EdgeColor ) ;
#else
	int Result ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_BEGIN( NoteString, -1 )

	Result = DrawNumberPlusToFToHandle_WCHAR_T( x, y, UseStringBuffer, Num, Length, Color, FontHandle, EdgeColor ) ;

	FONTHANDLE_TCHAR_TO_WCHAR_T_STRING_END

	return Result ;
#endif
}

// 浮動小数点型の数値とその説明の文字列を一度に描画する
extern int DrawNumberPlusToFToHandle_WCHAR_T( int x, int y, const wchar_t *NoteString, double Num, int Length, unsigned int Color, int FontHandle ,unsigned int EdgeColor )
{
	wchar_t Str[2048], Str2[256], Str3[256] ;

	if( ( int )_WCSLEN( NoteString ) > 2048 - 256 )
	{
		return -1 ;
	}

	_WCSCPY_S( Str, sizeof( Str ), NoteString ) ;
	_WCSCAT_S( Str, sizeof( Str ), L"=" ) ;

	_WCSCPY( Str2, L"%." ) ;
	_ITOAW_S( Length, Str3, sizeof( Str2 ), 10 ) ;
	_WCSCAT_S( Str2, sizeof( Str2 ), Str3 ) ;
	_WCSCAT_S( Str2, sizeof( Str2 ), L"f" ) ;
	_SWNPRINTF( Str3, sizeof( Str3 ) / 2, Str2, Num ) ;

	_WCSCAT_S( Str, sizeof( Str ), Str3 ) ;

	// 描画
	return DrawStringToHandle_WCHAR_T( x, y, Str, _WCSLEN( Str ), Color, FontHandle, EdgeColor, FALSE ) ;
}


#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_NON_FONT
