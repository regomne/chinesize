// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		文字コード関係プログラムヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXCHAR_H__
#define __DXCHAR_H__

// インクルード ------------------------------------------------------------------
#include "DxCompileConfig.h"
#include "DxLib.h"
#include "DxStatic.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

// BufferBytes キャンセル用マクロ
#define BUFFERBYTES_CANCEL					(0x7fffffff)

// シフトJIS２バイト文字判定
#define CHECK_SHIFTJIS_2BYTE( x )			( ( BYTE )( ( ( ( BYTE )(x) ) ^ 0x20) - ( BYTE )0xa1 ) < 0x3c )

// UTF16LEサロゲートペア判定( リトルエンディアン環境用 )
#define CHECK_CPU_LE_UTF16LE_4BYTE( x )		( ( ( x ) & 0xfc00 ) == 0xd800 )

// UTF16LEサロゲートペア判定( ビッグエンディアン環境用 )
#define CHECK_CPU_BE_UTF16LE_4BYTE( x )		( ( ( ( ( ( ( WORD )( x ) ) >> 8 ) & 0xff ) | ( ( ( WORD )( x ) << 8 ) & 0xff00 ) ) & 0xfc00 ) == 0xd800 )

// UTF16BEサロゲートペア判定( リトルエンディアン環境用 )
#define CHECK_CPU_LE_UTF16BE_4BYTE( x )		CHECK_CPU_BE_UTF16LE_4BYTE( x )

// UTF16BEサロゲートペア判定( ビッグエンディアン環境用 )
#define CHECK_CPU_BE_UTF16BE_4BYTE( x )		CHECK_CPU_LE_UTF16LE_4BYTE( x )

// wchar_t サロゲートペア判定( UTF-32 or UTF-16 想定 )
#ifdef WCHAR_T_BE
	#define CHECK_WCHAR_T_DOUBLE( x )	( sizeof( wchar_t ) == 2 && ( ( ( WORD )( x ) & 0x00fc ) == 0x00d8 ) )
#else
	#define CHECK_WCHAR_T_DOUBLE( x )	( sizeof( wchar_t ) == 2 && ( ( ( WORD )( x ) & 0xfc00 ) == 0xd800 ) )
#endif

// 構造体定義 --------------------------------------------------------------------

// UTF-16と各文字コードの対応表の情報
struct CHARCODETABLEINFO
{
	WORD				MultiByteToUTF16[ 0x10000 ] ;		// 各文字コードからUTF-16に変換するためのテーブル
	WORD				UTF16ToMultiByte[ 0x10000 ] ;		// UTF-16から各文字コードに変換するためのテーブル
} ;

// 文字コード処理関係で使用する情報
struct CHARCODESYSTEM
{
	int					InitializeFlag ;					// 初期化処理を行ったかどうか( TRUE:行った  FALSE:行っていない )

	CHARCODETABLEINFO	CharCodeCP932Info ;					// Shift-JISの文字コード情報
	CHARCODETABLEINFO	CharCodeCP936Info ;					// GB2312の文字コード情報
	CHARCODETABLEINFO	CharCodeCP949Info ;					// UHCの文字コード情報
	CHARCODETABLEINFO	CharCodeCP950Info ;					// BIG5の文字コード情報
	CHARCODETABLEINFO	CharCodeCP1252Info ;				// 欧文(ラテン文字の文字コード)の文字コード情報
	CHARCODETABLEINFO	CharCodeISO_IEC_8859_15Info ;		// 欧文(ラテン文字の文字コード)の文字コード情報
} ;

// テーブル-----------------------------------------------------------------------

// 内部大域変数宣言 --------------------------------------------------------------

extern CHARCODESYSTEM g_CharCodeSystem ;

// 関数プロトタイプ宣言-----------------------------------------------------------

extern	int				InitCharCode( void ) ;																					// 文字コード関連処理の初期化を行う

extern	int				GetCharCodeFormatUnitSize(	int CharCodeFormat ) ;														// 指定の文字コード形式の情報最少サイズを取得する( 戻り値：バイト数 )
extern	int				GetCharBytes_(			const char *CharCode, int CharCodeFormat ) ;									// １文字のバイト数を取得する( 戻り値：１文字のバイト数 )
extern	DWORD			GetCharCode(			const char *CharCode, int CharCodeFormat, int *CharBytes ) ;					// １文字の文字コードと文字のバイト数を取得する
extern	int				PutCharCode(			DWORD CharCode, int CharCodeFormat, char *Dest, size_t BufferBytes ) ;			// 文字コードを通常の文字列に変換する、終端にヌル文字は書き込まない( 戻り値：書き込んだバイト数 )
extern	DWORD			ConvCharCode(			DWORD SrcCharCode, int SrcCharCodeFormat, int DestCharCodeFormat ) ;			// 文字コードを指定の文字コード形式の文字に変換する
extern	int				ConvCharCodeString(		const DWORD *Src, int SrcCharCodeFormat, DWORD *Dest, size_t BufferBytes, int DestCharCodeFormat ) ;	// １文字４バイトの配列を、別文字コード形式の１文字４バイトの配列に変換する( 戻り値：変換後のサイズ、ヌル文字含む( 単位：バイト ) )
extern	int				StringToCharCodeString( const char  *Src, int SrcCharCodeFormat, DWORD *Dest, size_t BufferBytes  ) ;	// 文字列を１文字４バイトの配列に変換する( 戻り値：変換後のサイズ、ヌル文字含む( 単位：バイト ) )
extern	int				CharCodeStringToString( const DWORD *Src, char *Dest, size_t BufferBytes, int DestCharCodeFormat ) ;	// １文字４バイトの配列を文字列に変換する( 戻り値：変換後のサイズ、ヌル文字含む( 単位：バイト ) )
extern	int				ConvString(				const char *Src, int SrcStrLength, int SrcCharCodeFormat, char *Dest, size_t BufferBytes, int DestCharCodeFormat ) ;		// 文字列を指定の文字コード形式の文字列に変換する( 戻り値：変換後のサイズ、ヌル文字含む( 単位：バイト ) )
extern	int				GetStringCharNum(		const char *String, int CharCodeFormat ) ;										// 文字列に含まれる文字数を取得する
extern	const char *	GetStringCharAddress(	const char *String, int CharCodeFormat, int Index ) ;							// 指定番号の文字のアドレスを取得する
extern	DWORD			GetStringCharCode(		const char *String, int CharCodeFormat, int Index ) ;							// 指定番号の文字のコードを取得する

extern	void			CL_strcpy(            int CharCodeFormat, char *Dest,                     const char *Src ) ;
extern	void			CL_strcpy_s(          int CharCodeFormat, char *Dest, size_t BufferBytes, const char *Src ) ;
extern	void			CL_strpcpy(           int CharCodeFormat, char *Dest,                     const char *Src, int Pos ) ;
extern	void			CL_strpcpy_s(         int CharCodeFormat, char *Dest, size_t BufferBytes, const char *Src, int Pos ) ;
extern	void			CL_strpcpy2(          int CharCodeFormat, char *Dest,                     const char *Src, int Pos ) ;
extern	void			CL_strpcpy2_s(        int CharCodeFormat, char *Dest, size_t BufferBytes, const char *Src, int Pos ) ;
extern	void			CL_strncpy(           int CharCodeFormat, char *Dest,                     const char *Src, int Num ) ;
extern	void			CL_strncpy_s(         int CharCodeFormat, char *Dest, size_t BufferBytes, const char *Src, int Num ) ;
extern	void			CL_strncpy2(          int CharCodeFormat, char *Dest,                     const char *Src, int Num ) ;
extern	void			CL_strncpy2_s(        int CharCodeFormat, char *Dest, size_t BufferBytes, const char *Src, int Num ) ;
extern	void			CL_strrncpy(          int CharCodeFormat, char *Dest,                     const char *Src, int Num ) ;
extern	void			CL_strrncpy_s(        int CharCodeFormat, char *Dest, size_t BufferBytes, const char *Src, int Num ) ;
extern	void			CL_strrncpy2(         int CharCodeFormat, char *Dest,                     const char *Src, int Num ) ;
extern	void			CL_strrncpy2_s(       int CharCodeFormat, char *Dest, size_t BufferBytes, const char *Src, int Num ) ;
extern	void			CL_strpncpy(          int CharCodeFormat, char *Dest,                     const char *Src, int Pos, int Num ) ;
extern	void			CL_strpncpy_s(        int CharCodeFormat, char *Dest, size_t BufferBytes, const char *Src, int Pos, int Num ) ;
extern	void			CL_strpncpy2(         int CharCodeFormat, char *Dest,                     const char *Src, int Pos, int Num ) ;
extern	void			CL_strpncpy2_s(       int CharCodeFormat, char *Dest, size_t BufferBytes, const char *Src, int Pos, int Num ) ;
extern	void			CL_strcat(            int CharCodeFormat, char *Dest,                     const char *Src ) ;
extern	void			CL_strcat_s(          int CharCodeFormat, char *Dest, size_t BufferBytes, const char *Src ) ;
extern	size_t			CL_strlen(            int CharCodeFormat, const char *Str ) ;
extern	size_t			CL_strlen2(           int CharCodeFormat, const char *Str ) ;
extern	int				CL_strcmp(            int CharCodeFormat, const char *Str1, const char *Str2 ) ;
extern	int				CL_stricmp(           int CharCodeFormat, const char *Str1, const char *Str2 ) ;
extern	int				CL_strcmp_str2_ascii( int CharCodeFormat, const char *Str1, const char *Str2 ) ;
extern	int				CL_strncmp(           int CharCodeFormat, const char *Str1, const char *Str2, int Num ) ;
extern	int				CL_strncmp2(          int CharCodeFormat, const char *Str1, const char *Str2, int Num ) ;
extern	int				CL_strpncmp(          int CharCodeFormat, const char *Str1, const char *Str2, int Pos, int Num ) ;
extern	int				CL_strpncmp2(         int CharCodeFormat, const char *Str1, const char *Str2, int Pos, int Num ) ;
extern	DWORD			CL_strgetchr(         int CharCodeFormat, const char *Str, int Pos, int *CharNums ) ;
extern	DWORD			CL_strgetchr2(        int CharCodeFormat, const char *Str, int Pos, int *CharNums ) ;
extern	int				CL_strputchr(         int CharCodeFormat, char *Str, int Pos, DWORD CharCode ) ;
extern	int				CL_strputchr2(        int CharCodeFormat, char *Str, int Pos, DWORD CharCode ) ;
extern	const char *	CL_strpos(            int CharCodeFormat, const char *Str, int Pos ) ;
extern	const char *	CL_strpos2(           int CharCodeFormat, const char *Str, int Pos ) ;
extern	const char *	CL_strstr(            int CharCodeFormat, const char *Str1, const char *Str2 ) ;
extern	int				CL_strstr2(           int CharCodeFormat, const char *Str1, const char *Str2 ) ;
extern	const char *	CL_strrstr(           int CharCodeFormat, const char *Str1, const char *Str2 ) ;
extern	int				CL_strrstr2(          int CharCodeFormat, const char *Str1, const char *Str2 ) ;
extern	const char *	CL_strchr(            int CharCodeFormat, const char *Str, DWORD CharCode ) ;
extern	int				CL_strchr2(           int CharCodeFormat, const char *Str, DWORD CharCode ) ;
extern	const char *	CL_strrchr(           int CharCodeFormat, const char *Str, DWORD CharCode ) ;
extern	int				CL_strrchr2(          int CharCodeFormat, const char *Str, DWORD CharCode ) ;
extern	char *			CL_strupr(            int CharCodeFormat, char *Str ) ;
extern	int				CL_vsprintf(          int CharCodeFormat, int IsWChar, int CharCharCodeFormat, int WCharCharCodeFormat, char *Buffer,                    const char *FormatString, va_list Arg ) ;
extern	int				CL_vsnprintf(         int CharCodeFormat, int IsWChar, int CharCharCodeFormat, int WCharCharCodeFormat, char *Buffer, size_t BufferSize, const char *FormatString, va_list Arg ) ;
extern	int				CL_sprintf(           int CharCodeFormat, int IsWChar, int CharCharCodeFormat, int WCharCharCodeFormat, char *Buffer,                    const char *FormatString, ... ) ;
extern	int				CL_snprintf(          int CharCodeFormat, int IsWChar, int CharCharCodeFormat, int WCharCharCodeFormat, char *Buffer, size_t BufferSize, const char *FormatString, ... ) ;
extern	char *			CL_itoa(              int CharCodeFormat, int Value, char *Buffer,                     int Radix ) ;
extern	char *			CL_itoa_s(            int CharCodeFormat, int Value, char *Buffer, size_t BufferBytes, int Radix ) ;
extern	int				CL_atoi(              int CharCodeFormat, const char *Str ) ;
extern	double			CL_atof(              int CharCodeFormat, const char *Str ) ;
extern	int				CL_vsscanf(           int CharCodeFormat, int IsWChar, int CharCharCodeFormat, int WCharCharCodeFormat, const char *String, const char *FormatString, va_list Arg ) ;
extern	int				CL_sscanf(            int CharCodeFormat, int IsWChar, int CharCharCodeFormat, int WCharCharCodeFormat, const char *String, const char *FormatString, ... ) ;

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // __DXCHAR_H__
