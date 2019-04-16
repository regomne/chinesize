// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		標準関数の互換関数プログラムヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXBASEFUNC_H__
#define __DXBASEFUNC_H__

// インクルード ------------------------------------------------------------------
#include "DxCompileConfig.h"
#include "DxChar.h"
#include <stdarg.h>

#ifdef __WINDOWS__
#include "Windows/DxBaseFuncWin.h"
#endif // __WINDOWS__

#ifdef __ANDROID__
#include "Android/DxBaseFuncAndroid.h"
#endif // __ANDROID__

#ifdef __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE
		#include "iOS/DxBaseFunciOS.h"
    #endif // TARGET_OS_IPHONE
#endif // __APPLE__




#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

#ifndef _SINCOS_PLATFORM
	#define _SINCOS_PLATFORM						_SINCOS
#endif

#ifdef UNICODE
	#define _TMULT( strp, cset )					CheckUTF16H( (strp) )
#else
	#define _TMULT( strp, cset )					CheckMultiByteChar( (strp), (cset) )
#endif

#define DXWCHAR		WORD
#ifdef UNICODE
	#define _DXWTP( x )		( DXWCHAR * )( x )
	#define _DXWTR( x )		( DXWCHAR * )( _T( x ) )
#else
	#define _DXWTP( x )		x
	#define _DXWTR( x )		x
#endif


#define CHAR_TO_WCHAR_TEMPSTRINGLENGTH				(512)


#define CHAR_TO_WCHAR_T_STRING_BEGIN( str )													\
	BYTE    str##Buffer[ sizeof( wchar_t ) * CHAR_TO_WCHAR_TEMPSTRINGLENGTH ] ;				\
	void    *Alloc##str##Buffer = NULL ;													\
	wchar_t *Use##str##Buffer = NULL ;														\
	size_t   str##Length ;																	\
	size_t   wchar_##str##Length ;															\


#define CHAR_TO_WCHAR_T_STRING_SETUP( str, err, CHARCODEFORMAT )										\
	if( str != NULL )																					\
	{																									\
		str##Length = CL_strlen( CHARCODEFORMAT, str ) ;												\
		if( str##Length + 16 > CHAR_TO_WCHAR_TEMPSTRINGLENGTH )											\
		{																								\
			Alloc##str##Buffer = DXALLOC( sizeof( wchar_t ) * ( str##Length + 16 ) ) ;					\
			if( Alloc##str##Buffer == NULL )															\
			{																							\
				err ;																					\
			}																							\
			Use##str##Buffer = ( wchar_t * )Alloc##str##Buffer ;										\
		}																								\
		else																							\
		{																								\
			Use##str##Buffer = ( wchar_t * )str##Buffer ;												\
		}																								\
																										\
		wchar_##str##Length = ConvString( ( const char * )str, ( int )str##Length, CHARCODEFORMAT, ( char * )Use##str##Buffer, sizeof( wchar_t ) * ( str##Length + 16 ), WCHAR_T_CHARCODEFORMAT ) / GetCharCodeFormatUnitSize( WCHAR_T_CHARCODEFORMAT ) ;\
		if( wchar_##str##Length > 0 )																	\
		{																								\
			wchar_##str##Length -- ;																	\
		}																								\
	}


#define CHAR_TO_WCHAR_T_STRING_END( str )			\
	if( Alloc##str##Buffer != NULL )				\
	{												\
		DXFREE( Alloc##str##Buffer ) ;				\
		Alloc##str##Buffer = NULL ;					\
	}



#define SHIFT_JIS_TO_WCHAR_T_STRING_BEGIN( str )		CHAR_TO_WCHAR_T_STRING_BEGIN( str )
#define SHIFT_JIS_TO_WCHAR_T_STRING_SETUP( str, err )	CHAR_TO_WCHAR_T_STRING_SETUP( str, err, DX_CHARCODEFORMAT_SHIFTJIS )
#define SHIFT_JIS_TO_WCHAR_T_STRING_END( str )			CHAR_TO_WCHAR_T_STRING_END( str )


#define SHIFT_JIS_TO_WCHAR_T_STRING_ONE_BEGIN( str, err )		\
	SHIFT_JIS_TO_WCHAR_T_STRING_BEGIN( str )					\
	SHIFT_JIS_TO_WCHAR_T_STRING_SETUP( str, err )



#define TCHAR_TO_WCHAR_T_STRING_BEGIN( str )			CHAR_TO_WCHAR_T_STRING_BEGIN( str )
#define TCHAR_TO_WCHAR_T_STRING_SETUP( str, err )		CHAR_TO_WCHAR_T_STRING_SETUP( str, err, _TCHARCODEFORMAT )
#define TCHAR_TO_WCHAR_T_STRING_END( str )				CHAR_TO_WCHAR_T_STRING_END( str )


#define TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( str, err )		\
	TCHAR_TO_WCHAR_T_STRING_BEGIN( str )					\
	TCHAR_TO_WCHAR_T_STRING_SETUP( str, err )








#define CHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_BEGIN( str )									\
	BYTE     str##Buffer[ sizeof( wchar_t ) * CHAR_TO_WCHAR_TEMPSTRINGLENGTH ] ;				\
	void    *Alloc##str##Buffer = NULL ;														\
	wchar_t *Use##str##Buffer = NULL ;															\
	size_t   wchar_##str##Length = 0 ;


#define CHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_SETUP( str, strlength, err, CHARCODEFORMAT )	\
	if( str != NULL )																			\
	{																							\
		if( strlength + 16 > CHAR_TO_WCHAR_TEMPSTRINGLENGTH )									\
		{																						\
			Alloc##str##Buffer = DXALLOC( sizeof( wchar_t ) * ( strlength + 16 ) ) ;			\
			if( Alloc##str##Buffer == NULL )													\
			{																					\
				err ;																			\
			}																					\
			Use##str##Buffer = ( wchar_t * )Alloc##str##Buffer ;								\
		}																						\
		else																					\
		{																						\
			Use##str##Buffer = ( wchar_t * )str##Buffer ;										\
		}																						\
																								\
		wchar_##str##Length = ConvString( ( const char * )str, ( int )strlength, CHARCODEFORMAT, ( char * )Use##str##Buffer, sizeof( wchar_t ) * ( str##Length + 16 ), WCHAR_T_CHARCODEFORMAT ) / GetCharCodeFormatUnitSize( WCHAR_T_CHARCODEFORMAT ) ;\
		if( wchar_##str##Length > 0 )															\
		{																						\
			wchar_##str##Length -- ;															\
		}																						\
	}

#define CHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( str )		\
	if( Alloc##str##Buffer != NULL )								\
	{																\
		DXFREE( Alloc##str##Buffer ) ;								\
		Alloc##str##Buffer = NULL ;									\
	}


#define TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_BEGIN( str )						CHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_BEGIN( str )
#define TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_SETUP( str, strlength, err )		CHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_SETUP( str, strlength, err, _TCHARCODEFORMAT )
#define TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( str )						CHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( str )

#define TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( str, strlength, err )		\
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_BEGIN( str )								\
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_SETUP( str, strlength, err )





#define TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_BEGIN( str )												\
	TCHAR  str##Buffer[ CHAR_TO_WCHAR_TEMPSTRINGLENGTH ] ;													\
	TCHAR *Alloc##str##Buffer = NULL ;																		\
	TCHAR *Use##str##Buffer = NULL ;


#define TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_SETUP( str, strlength, err )								\
	if( str != NULL )																						\
	{																										\
		if( strlength + 1 > CHAR_TO_WCHAR_TEMPSTRINGLENGTH )												\
		{																									\
			Alloc##str##Buffer = ( TCHAR * )DXALLOC( sizeof( TCHAR ) * ( strlength + 16 ) ) ;				\
			if( Alloc##str##Buffer == NULL )																\
			{																								\
				err ;																						\
			}																								\
			Use##str##Buffer = Alloc##str##Buffer ;															\
		}																									\
		else																								\
		{																									\
			Use##str##Buffer = str##Buffer ;																\
		}																									\
																											\
		CL_strncpy( _TCHARCODEFORMAT, ( char * )Use##str##Buffer, ( const char * )str, ( int )strlength ) ;	\
		Use##str##Buffer[ strlength ] = '\0' ;																\
	}

#define TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( str )			\
	if( Alloc##str##Buffer != NULL )								\
	{																\
		DXFREE( Alloc##str##Buffer ) ;								\
		Alloc##str##Buffer = NULL ;									\
	}

#define TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_ONE_BEGIN( str, strlength, err )	\
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_BEGIN( str )							\
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_SETUP( str, strlength, err )







#define WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_BEGIN( str )											\
	wchar_t  str##Buffer[ CHAR_TO_WCHAR_TEMPSTRINGLENGTH ] ;												\
	wchar_t *Alloc##str##Buffer = NULL ;																	\
	wchar_t *Use##str##Buffer = NULL ;


#define WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_SETUP( str, strlength, err )										\
	if( str != NULL )																									\
	{																													\
		if( strlength + 1 > CHAR_TO_WCHAR_TEMPSTRINGLENGTH )															\
		{																												\
			Alloc##str##Buffer = ( wchar_t * )DXALLOC( sizeof( wchar_t ) * ( strlength + 16 ) ) ;						\
			if( Alloc##str##Buffer == NULL )																			\
			{																											\
				err ;																									\
			}																											\
			Use##str##Buffer = Alloc##str##Buffer ;																		\
		}																												\
		else																											\
		{																												\
			Use##str##Buffer = str##Buffer ;																			\
		}																												\
																														\
		CL_strncpy( WCHAR_T_CHARCODEFORMAT, ( char * )Use##str##Buffer, ( const char * )str, ( int )( strlength )  ) ;	\
		Use##str##Buffer[ strlength ] = L'\0' ;																			\
	}

#define WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( str )		\
	if( Alloc##str##Buffer != NULL )								\
	{																\
		DXFREE( Alloc##str##Buffer ) ;								\
		Alloc##str##Buffer = NULL ;									\
	}

#define WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( str, strlength, err )	\
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_BEGIN( str )							\
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_SETUP( str, strlength, err )






#define TCHAR_FORMATSTRING_SETUP														\
	va_list VaList ;																	\
	TCHAR String[ 2048 ] ;																\
																						\
	va_start( VaList, FormatString ) ;													\
																						\
	_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;	\
																						\
	va_end( VaList ) ;


#define WCHAR_T_FORMATSTRING_SETUP										\
	va_list VaList ;													\
	wchar_t String[ 2048 ] ;											\
																		\
	va_start( VaList, FormatString ) ;									\
																		\
	_VSWNPRINTF( String, sizeof( String ) / 2, FormatString, VaList ) ;	\
																		\
	va_end( VaList ) ;


#define GET_MEM_BYTE( ptr )						( ( BYTE  )( ( ( BYTE * )ptr )[ 0 ] ) )
#define GET_MEM_WORD( ptr )						( ( WORD  )( ( ( BYTE * )ptr )[ 0 ] | ( ( ( BYTE * )ptr )[ 1 ] << 8 ) ) )
#define GET_MEM_DWORD( ptr )					( ( DWORD )( ( ( BYTE * )ptr )[ 0 ] | ( ( ( BYTE * )ptr )[ 1 ] << 8 ) | ( ( ( BYTE * )ptr )[ 2 ] << 16 ) | ( ( ( BYTE * )ptr )[ 3 ] << 24 ) ) )

#define GET_MEM_SIGNED_BYTE( ptr )				( ( signed char  )GET_MEM_BYTE( ptr ) )
#define GET_MEM_SIGNED_WORD( ptr )				( ( signed short )GET_MEM_WORD( ptr ) )
#define GET_MEM_SIGNED_DWORD( ptr )				( ( signed int   )GET_MEM_DWORD( ptr ) )

#define READ_MEM_1BYTE( dst_ptr, src_ptr )		( ( BYTE * )( dst_ptr ) )[ 0 ] = ( ( BYTE * )( src_ptr ) )[ 0 ] ;

#define READ_MEM_2BYTE( dst_ptr, src_ptr )		( ( BYTE * )( dst_ptr ) )[ 0 ] = ( ( BYTE * )( src_ptr ) )[ 0 ] ;\
												( ( BYTE * )( dst_ptr ) )[ 1 ] = ( ( BYTE * )( src_ptr ) )[ 1 ] ;

#define READ_MEM_3BYTE( dst_ptr, src_ptr )		( ( BYTE * )( dst_ptr ) )[ 0 ] = ( ( BYTE * )( src_ptr ) )[ 0 ] ;\
												( ( BYTE * )( dst_ptr ) )[ 1 ] = ( ( BYTE * )( src_ptr ) )[ 1 ] ;\
												( ( BYTE * )( dst_ptr ) )[ 2 ] = ( ( BYTE * )( src_ptr ) )[ 2 ] ;

#define READ_MEM_4BYTE( dst_ptr, src_ptr )		( ( BYTE * )( dst_ptr ) )[ 0 ] = ( ( BYTE * )( src_ptr ) )[ 0 ] ;\
												( ( BYTE * )( dst_ptr ) )[ 1 ] = ( ( BYTE * )( src_ptr ) )[ 1 ] ;\
												( ( BYTE * )( dst_ptr ) )[ 2 ] = ( ( BYTE * )( src_ptr ) )[ 2 ] ;\
												( ( BYTE * )( dst_ptr ) )[ 3 ] = ( ( BYTE * )( src_ptr ) )[ 3 ] ;

#define READ_MEM_8BYTE( dst_ptr, src_ptr )		( ( BYTE * )( dst_ptr ) )[ 0 ] = ( ( BYTE * )( src_ptr ) )[ 0 ] ;\
												( ( BYTE * )( dst_ptr ) )[ 1 ] = ( ( BYTE * )( src_ptr ) )[ 1 ] ;\
												( ( BYTE * )( dst_ptr ) )[ 2 ] = ( ( BYTE * )( src_ptr ) )[ 2 ] ;\
												( ( BYTE * )( dst_ptr ) )[ 3 ] = ( ( BYTE * )( src_ptr ) )[ 3 ] ;\
												( ( BYTE * )( dst_ptr ) )[ 4 ] = ( ( BYTE * )( src_ptr ) )[ 4 ] ;\
												( ( BYTE * )( dst_ptr ) )[ 5 ] = ( ( BYTE * )( src_ptr ) )[ 5 ] ;\
												( ( BYTE * )( dst_ptr ) )[ 6 ] = ( ( BYTE * )( src_ptr ) )[ 6 ] ;\
												( ( BYTE * )( dst_ptr ) )[ 7 ] = ( ( BYTE * )( src_ptr ) )[ 7 ] ;

#define READ_MEM_FLOAT( dst_ptr, src_ptr )		READ_MEM_4BYTE( dst_ptr, src_ptr )

#define READ_MEM_DOUBLE( dst_ptr, src_ptr )		READ_MEM_8BYTE( dst_ptr, src_ptr )


#define SINTABLE_DIV						(65536)				// サインテーブルの精度

// 構造体定義 --------------------------------------------------------------------

// 標準関数の互換関数で使用する情報
struct BASEFUNCSYSTEM
{
	int					UseCharSet ;							// 使用する文字列セット
	int					Use_char_CharCodeFormat ;				// char で使用する文字コード形式
	int					Use_wchar_t_CharCodeFormat ;			// wchar_t 使用する文字コード形式

	float				SinTable[ SINTABLE_DIV ] ;				// サインテーブル
} ;

// テーブル-----------------------------------------------------------------------

// 内部大域変数宣言 --------------------------------------------------------------

extern BASEFUNCSYSTEM g_BaseFuncSystem ;

// 関数プロトタイプ宣言-----------------------------------------------------------

// TCHAR用定義
#ifdef UNICODE
	#define _TISWCHAR								(TRUE)
	#define _TSTRCAT								_WCSCAT
	#define _TSTRCAT_S								_WCSCAT_S
	#define _TSTRSTR								_WCSSTR
	#define _TSTRCPY								_WCSCPY
	#define _TSTRCPY_S								_WCSCPY_S
	#define _TSTRLEN								_WCSLEN
	#define _TSTRCMP								_WCSCMP
	#define _TSTRNCMP								_WCSNCMP
	#define _TSTRNCPY								_WCSNCPY
	#define _TSTRNCPY_S								_WCSNCPY_S
	#define _TSTRUPR								_WCSUPR
	#define _TSTRCHR								_WCSCHR
	#define _TSTRRCHR								_WCSRCHR
	#define _TVSPRINTF( buf, frm, vlst )			_VSWPRINTF( buf, frm, vlst )
	#define _TVSNPRINTF( buf, bufsize, frm, vlst )	_VSWNPRINTF( buf, bufsize, frm, vlst )
	#define _TSPRINTF								_SWPRINTF
	#define _TSNPRINTF								_SWNPRINTF
	#define _TTOF									_ATOFW
	#define _TTOI									_ATOIW
	#define _ITOT									_ITOAW
	#define _ITOT_S									_ITOAW_S
	#define _TCHDIR									_WCHDIR
	#define _TGETCWD								_WGETCWD
	#define _TCHARCODEFORMAT						WCHAR_T_CHARCODEFORMAT
	#define _TCHARSIZE								GetCharCodeFormatUnitSize( WCHAR_T_CHARCODEFORMAT )
#else
	#define _TISWCHAR								(FALSE)
	#define _TSTRCAT								_STRCAT
	#define _TSTRCAT_S								_STRCAT_S
	#define _TSTRSTR								_STRSTR
	#define _TSTRCPY								_STRCPY
	#define _TSTRCPY_S								_STRCPY_S
	#define _TSTRLEN								_STRLEN
	#define _TSTRCMP								_STRCMP
	#define _TSTRNCMP								_STRNCMP
	#define _TSTRNCPY								_STRNCPY
	#define _TSTRNCPY_S								_STRNCPY_S
	#define _TSTRUPR								_STRUPR
	#define _TSTRCHR								_STRCHR
	#define _TSTRRCHR								_STRRCHR
	#define _TVSPRINTF								_VSPRINTF
	#define _TVSNPRINTF								_VSNPRINTF
	#define _TSPRINTF								_SPRINTF
	#define _TSNPRINTF								_SNPRINTF
	#define _TTOF									_ATOF
	#define _TTOI									_ATOI
	#define _ITOT									_ITOA
	#define _ITOT_S									_ITOA_S
	#define _TCHDIR									_CHDIR
	#define _TGETCWD								_GETCWD
	#define _TCHARCODEFORMAT						CHAR_CHARCODEFORMAT
	#define _TCHARSIZE								GetCharCodeFormatUnitSize( CHAR_CHARCODEFORMAT )
#endif

#define CHAR_CHARCODEFORMAT		( g_BaseFuncSystem.Use_char_CharCodeFormat    == 0 ? ( _SET_DEFAULT_CHARCODEFORMAT(), g_BaseFuncSystem.Use_char_CharCodeFormat    ) : g_BaseFuncSystem.Use_char_CharCodeFormat    )
#define WCHAR_T_CHARCODEFORMAT	( g_BaseFuncSystem.Use_wchar_t_CharCodeFormat == 0 ? ( _SET_DEFAULT_CHARCODEFORMAT(), g_BaseFuncSystem.Use_wchar_t_CharCodeFormat ) : g_BaseFuncSystem.Use_wchar_t_CharCodeFormat )

extern	int				_INIT_BASEFUNC( void ) ;

extern	void			_SET_CHARSET( int CharSet ) ;
extern	int				_GET_CHARSET( void ) ;

extern	void			_SET_DEFAULT_CHARCODEFORMAT( void ) ;
extern	void			_SET_CHAR_CHARCODEFORMAT( int CharCodeFormat ) ;
extern	int				_GET_CHAR_CHARCODEFORMAT( void ) ;
extern	void			_SET_WCHAR_T_CHARCODEFORMAT( int CharCodeFormat ) ;
extern	int				_GET_WCHAR_T_CHARCODEFORMAT( void ) ;


// 自前標準関数系
extern	void			_STRCPY(    char    *Dest,                     const char    *Src ) ;
extern	void			_WCSCPY(    wchar_t *Dest,                     const wchar_t *Src ) ;
extern	void			_STRCPY_S(  char    *Dest, size_t BufferBytes, const char    *Src ) ;
extern	void			_WCSCPY_S(  wchar_t *Dest, size_t BufferBytes, const wchar_t *Src ) ;
extern	void			_STRNCPY(   char    *Dest,                     const char    *Src, int Num ) ;
extern	void			_WCSNCPY(   wchar_t *Dest,                     const wchar_t *Src, int Num ) ;
extern	void			_STRNCPY_S( char    *Dest, size_t BufferBytes, const char    *Src, int Num ) ;
extern	void			_WCSNCPY_S( wchar_t *Dest, size_t BufferBytes, const wchar_t *Src, int Num ) ;
extern	void			_STRCAT(    char    *Dest,                     const char    *Src ) ;
extern	void			_WCSCAT(    wchar_t *Dest,                     const wchar_t *Src ) ;
extern	void			_STRCAT_S(  char    *Dest, size_t BufferBytes, const char    *Src ) ;
extern	void			_WCSCAT_S(  wchar_t *Dest, size_t BufferBytes, const wchar_t *Src ) ;
extern	const char *	_STRSTR(    const char    *Str1, const char    *Str2 ) ;
extern	const wchar_t *	_WCSSTR(    const wchar_t *Str1, const wchar_t *Str2 ) ;
extern	size_t			_STRLEN(    const char    *Str ) ;
//extern size_t			_WCSLEN(    const wchar_t *Str ) ;
extern	int				_STRCMP(    const char    *Str1, const char    *Str2 ) ;
extern	int				_WCSCMP(    const wchar_t *Str1, const wchar_t *Str2 ) ;
extern	int				_STRICMP(   const char    *Str1, const char    *Str2 ) ;
extern	int				_WCSICMP(   const wchar_t *Str1, const wchar_t *Str2 ) ;
extern	int				_STRNCMP(   const char    *Str1, const char    *Str2, int Size ) ;
extern	int				_WCSNCMP(   const wchar_t *Str1, const wchar_t *Str2, int Size ) ;
extern	const char *	_STRCHR(    const char    *Str, int     Char ) ;
extern	const wchar_t *	_WCSCHR(    const wchar_t *Str, wchar_t Char ) ;
extern	const char *	_STRRCHR(   const char    *Str, int     Char ) ;
extern	const wchar_t *	_WCSRCHR(   const wchar_t *Str, wchar_t Char ) ;
extern	char *			_STRUPR(    char    *Str ) ;
extern	wchar_t *		_WCSUPR(    wchar_t *Str ) ;
extern	int				_VSPRINTF(   char    *Buffer,                    const char    *FormatString, va_list Arg ) ;
extern	int				_VSNPRINTF(  char    *Buffer, size_t BufferSize, const char    *FormatString, va_list Arg ) ;
extern	int				_VSWPRINTF(  wchar_t *Buffer,                    const wchar_t *FormatString, va_list Arg ) ; // 実体は DxUseCLib.cpp の中
extern	int				_VSWNPRINTF( wchar_t *Buffer, size_t BufferSize, const wchar_t *FormatString, va_list Arg ) ; // 実体は DxUseCLib.cpp の中
extern	int				_SPRINTF(    char    *Buffer,                    const char    *FormatString, ... ) ;
extern	int				_SNPRINTF(   char    *Buffer, size_t BufferSize, const char    *FormatString, ... ) ;
extern	int				_SWPRINTF(   wchar_t *Buffer,                    const wchar_t *FormatString, ... ) ;
extern	int				_SWNPRINTF(  wchar_t *Buffer, size_t BufferSize, const wchar_t *FormatString, ... ) ;
extern	void			_MEMSET(  void *Memory, unsigned char Char, size_t Size ) ;
extern	void			_MEMSETD( void *Memory, unsigned int  Data, int Num ) ;
extern	int				_MEMCMP(  const void *Buffer1, const void *Buffer2, int Size ) ;
extern	void			_MEMCPY(  void *Buffer1, const void *Buffer2, size_t Size ) ;
extern	void			_MEMMOVE( void *Buffer1, const void *Buffer2, size_t Size ) ;

extern	char *			_GETCWD(  char    *Buffer, size_t BufferBytes ) ;
extern	wchar_t *		_WGETCWD( wchar_t *Buffer, size_t BufferBytes ) ;
extern	int				_CHDIR(   const char *    DirName ) ;
extern	int				_WCHDIR(  const wchar_t * DirName ) ;

extern	void			_SINCOS( float Angle, float *DestSin, float *DestCos ) ;
extern	void			_SINCOSD( double Angle, double *DestSin, double *DestCos ) ;
extern	int				_FPCLASS( double x ) ;
extern	float			_POW( float x, float y ) ;
extern	double			_POWD( double x, double y ) ;
extern	float			_ATAN2( float y, float x ) ;
extern	double			_ATAN2D( double y, double x ) ;
extern	float			_EXPF( float x ) ;
extern	double			_EXPFD( double x ) ;
extern	float			_ASIN( float Real ) ;
extern	double			_ASIND( double Real ) ;
extern	float			_ACOS( float Real ) ;
extern	double			_ACOSD( double Real ) ;
extern	double			_LOG10( double Real ) ;
//extern	int				_ABS( int Number ) ;
//extern	float			_FABS( float Real ) ;
//extern	double			_DABS( double Real ) ;
extern	int				_FTOL( float Real ) ;
//extern	int			_DTOL( double Real ) ;
extern	LONGLONG		_DTOL64( double Real ) ;
extern	void			_FTOL4( float *Real, int *Buffer4 ) ;
extern	void			_DTOL4( double *Real, int *Buffer4 ) ;
extern	float			_SQRT( float Real ) ;
extern	double			_SQRTD( double Real ) ;
extern	int				_ATOI( const char *String ) ;
extern	int				_ATOIW( const wchar_t *String ) ;
extern	double			_ATOF( const char *String ) ;
extern	double			_ATOFW( const wchar_t *String ) ;
extern	char *			_ITOA( int Value, char *Buffer, int Radix ) ;
extern	wchar_t	*		_ITOAW( int Value, wchar_t *Buffer, int Radix ) ;
extern	char *			_ITOA_S( int Value, char *Buffer, size_t BufferBytes, int Radix ) ;
extern	wchar_t	*		_ITOAW_S( int Value, wchar_t *Buffer, size_t BufferBytes, int Radix ) ;
extern	void			_SHR64( DWORD *Number64, int ShftNum ) ;
extern	void			_SHL64( DWORD *Number64, int ShftNum ) ;
extern	void			_MUL128_1( DWORD *Src64_1, DWORD *Src64_2, DWORD *Dest128 );
extern	void			_DIV128_1( DWORD *Src128,  DWORD *Div64,   DWORD *Dest64  );

__inline void _TABLE_SINCOS( float Angle, float * RST Sin, float * RST Cos )
{
	if( Angle > 10.0f || Angle < -10.0f )
	{
		_SINCOS_PLATFORM( Angle, Sin, Cos ) ;
	}
	else
	{
		Angle = Angle * ( SINTABLE_DIV / DX_TWO_PI_F ) + 12582912.0f ;
		*Sin = g_BaseFuncSystem.SinTable[ *( ( DWORD * )&Angle ) & ( SINTABLE_DIV - 1 ) ] ;
		*Cos = g_BaseFuncSystem.SinTable[ ( *( ( DWORD * )&Angle ) + SINTABLE_DIV / 4 ) & ( SINTABLE_DIV - 1 ) ] ;
	}
}

__inline size_t _WCSLEN( const wchar_t *Str )
{
	size_t i ;

	if( sizeof( wchar_t ) == 2 )
	{
		for( i = 0 ; ( ( WORD * )Str )[ i ] != 0 ; i ++ ){}
		return i ;
	}
	else
	{
		for( i = 0 ; ( ( DWORD * )Str )[ i ] != 0 ; i ++ ){}
		return i ;
	}
}

__inline int _ABS( int Number )
{
	return Number < 0 ? -Number : Number ;
}

__inline float _FABS( float Real )
{
	return Real < 0.0F ? -Real : Real ;
}

__inline double _DABS( double Real )
{
	return Real < 0.0 ? -Real : Real ;
}

__inline int _DTOL( double Real )
{
#if !defined( DX_NON_INLINE_ASM ) && !defined( __BCC )
	DWORD Result ;
	WORD STFlag, DSTFlag ;
	__asm
	{
		fnstcw	STFlag
		mov		ax, STFlag
		or		ax, 0xC00
		mov		DSTFlag, ax
		fld		Real
		fldcw	DSTFlag
		fistp	Result
		fldcw	STFlag
	}
	return ( int )Result ;
#else
	return ( int )Real ;
#endif
}

__inline int _FTOL( float Real )
{
#if !defined( DX_NON_INLINE_ASM ) && !defined( __BCC )
	DWORD Result ;
	WORD STFlag, DSTFlag ;
	__asm
	{
		fnstcw	STFlag
		mov		ax, STFlag
		or		ax, 0xC00
		mov		DSTFlag, ax
		fld		Real
		fldcw	DSTFlag
		fistp	Result
		fldcw	STFlag
	}
	return ( int )Result ;
#else
	return ( int )Real ;
#endif
}

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // __DXBASEFUNC_H__

