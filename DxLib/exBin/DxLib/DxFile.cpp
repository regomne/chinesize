// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		ファイルアクセスプログラム
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

// インクルード ------------------------------------------------------------------
#include "DxFile.h"
#include "DxLib.h"
#include "DxStatic.h"
#include "DxBaseFunc.h"
#include "DxChar.h"
#include "DxMemory.h"
#include "DxUseCLib.h"
#include "DxASyncLoad.h"
#include "DxSystem.h"
#include "DxLog.h"

#ifndef DX_NON_DXA
#include "DxArchive_.h"
#endif

#ifndef DX_NON_PRINTF_DX
#include "DxFont.h"
#endif

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

// ファイルアクセスハンドル有効性チェック
#define FILEHCHK( HAND, FPOINT )		HANDLECHK(       DX_HANDLETYPE_FILE, HAND, *( ( HANDLEINFO ** )&FPOINT ) )
#define FILEHCHK_ASYNC( HAND, FPOINT )	HANDLECHK_ASYNC( DX_HANDLETYPE_FILE, HAND, *( ( HANDLEINFO ** )&FPOINT ) )

#define STREAM_FSYNC( StreamData )		{while( (StreamData)->ReadShred.IdleCheck( (StreamData)->DataPoint ) == FALSE ) Thread_Sleep(0);}

// 構造体定義 --------------------------------------------------------------------

// メモリに置かれたデータをファイルとして扱うためのデータ構造体
typedef struct tagMEMSTREAMDATA
{
	unsigned char			*DataBuffer ;
	size_t					DataSize ;
	size_t					DataPoint ;
	int						EOFFlag ;
} MEMSTREAMDATA, *LPMEMSTREAMDATA ;

// // キャッシュ使用ストリームデータアクセス処理用データの構造体
// struct CACHE_STREAMDATA
// {
// 	STREAMDATA				StreamData ;						// キャッシュ対応するストリームデータ
// 
// 	ULONGLONG				DataBytes ;							// データサイズ
// 	ULONGLONG				AccessPosition ;					// アクセス位置
// 	int						EofFlag ;							// 終端チェックフラグ
// 
// 	BYTE *					CacheBuffer ;						// キャッシュ用バッファ
// 	DWORD					CacheBufferBytes ;					// キャッシュ用バッファのサイズ
// 	ULONGLONG				CachePosition ;						// キャッシュ開始位置
// 	DWORD					CacheValidBytes ;					// 有効なキャッシュのサイズ
// } ;

// ファイル情報リスト構造体
struct FILEINFOLIST
{
	int Num;		// ファイルの数
					// ここに FILEINFO が Num の数だけ存在する
} ;


// 内部大域変数宣言 --------------------------------------------------------------

// ファイルアクセス処理が使用するグローバルデータの構造体
FILEMANAGEDATA GFileData ;

// アーカイブファイルを読み込む機能を使用しないかどうかのフラグ
int NotUseDXArchive ;

extern int g_EnableRootDirName ;			// パスの始まりが / や \\ だった場合、ルートディレクトリを指定したことにするか( 1:ルートディレクトリを指定したことにする  0:ルートディレクトリを指定したことにしない )
extern int g_AddDriveNameNum ;				// 追加のドライブ名の数
extern const char *g_AddDriveName[ 16 ] ;	// 追加のドライブ名

// 関数プロトタイプ宣言 ----------------------------------------------------------

// ファイルハンドル関数
static	int			InitializeFileHandle( HANDLEINFO *HandleInfo ) ;								// ファイルアクセスハンドルを初期化する
static	int			TerminateFileHandle( HANDLEINFO *HandleInfo ) ;									// ファイルアクセスハンドルの後始末を行う

// メモリに置かれたデータをファイルのデータに例えてつかうための関数
static	LONGLONG	MemStreamTell( DWORD_PTR StreamDataPoint ) ;
static	int			MemStreamSeek( DWORD_PTR StreamDataPoint, LONGLONG SeekPoint, int SeekType ) ;
static	size_t		MemStreamRead( void *Buffer, size_t BlockSize, size_t DataNum, DWORD_PTR StreamDataPoint) ;
static	size_t		MemStreamWrite( void *Buffer, size_t BlockSize, size_t DataNum, DWORD_PTR StreamDataPoint) ;
static	int			MemStreamEof( DWORD_PTR StreamDataPoint) ;
static	int			MemStreamIdleCheck( DWORD_PTR StreamDataPoint) ;

// TCHAR版ストリームデータアクセスエミュレート用関数
static	DWORD_PTR	StreamTCHAR_Open( const wchar_t *Path, int UseCacheFlag, int BlockReadFlag, int UseASyncReadFlag  ) ;
static	int			StreamTCHAR_Close( DWORD_PTR Handle ) ;
static	LONGLONG	StreamTCHAR_Tell( DWORD_PTR Handle ) ;
static	int			StreamTCHAR_Seek( DWORD_PTR Handle, LONGLONG SeekPoint, int SeekType ) ;
static	size_t		StreamTCHAR_Read( void *Buffer, size_t BlockSize, size_t DataNum, DWORD_PTR Handle ) ;
static	int			StreamTCHAR_Eof( DWORD_PTR Handle ) ;
static	int			StreamTCHAR_IdleCheck( DWORD_PTR Handle ) ;
static	int			StreamTCHAR_ChDir( const wchar_t *Path ) ;
static	int			StreamTCHAR_GetDir( wchar_t *Buffer ) ;
static	int			StreamTCHAR_GetDirS( wchar_t *Buffer, size_t BufferBytes ) ;
static	DWORD_PTR	StreamTCHAR_FindFirst( const wchar_t *FilePath, FILEINFOW *Buffer ) ;				// 戻り値: -1=エラー  -1以外=FindHandle
static	int			StreamTCHAR_FindNext( DWORD_PTR FindHandle, FILEINFOW *Buffer ) ;					// 戻り値: -1=エラー  0=成功
static	int			StreamTCHAR_FindClose( DWORD_PTR FindHandle ) ;									// 戻り値: -1=エラー  0=成功

// FileRead_scanf_base で１文字ファイルから読み込む処理を行う関数
static	int			FileRead_scanf_base_help_getchar( DWORD *CharCode, int UnitSize, int *CharBytesP, STREAMDATA *StreamData, int CharCodeFormat ) ;

// データ宣言 --------------------------------------------------------------------

// ストリームデータアクセス用関数
STREAMDATASHREDTYPE2W StreamFunctionW =
{
#ifndef DX_NON_DXA
	DXA_DIR_Open,
	DXA_DIR_Close,
	DXA_DIR_Tell,
	DXA_DIR_Seek,
	DXA_DIR_Read,
	DXA_DIR_Eof,
	DXA_DIR_IdleCheck,
	DXA_DIR_ChDir,
	DXA_DIR_GetDir,
	DXA_DIR_GetDirS,
	DXA_DIR_FindFirst,
	DXA_DIR_FindNext,
	DXA_DIR_FindClose,
#else
	ReadOnlyFileAccessOpen,
	ReadOnlyFileAccessClose,
	ReadOnlyFileAccessTell,
	ReadOnlyFileAccessSeek,
	ReadOnlyFileAccessRead,
	ReadOnlyFileAccessEof,
	ReadOnlyFileAccessIdleCheck,
	ReadOnlyFileAccessChDir,
	ReadOnlyFileAccessGetDir,
	ReadOnlyFileAccessGetDirS,
	ReadOnlyFileAccessFindFirst,
	ReadOnlyFileAccessFindNext,
	ReadOnlyFileAccessFindClose,
#endif
} ;

STREAMDATASHREDTYPE2 StreamFunction ;

// ストリームデータ制御用関数ポインタのデフォルト配列

// ファイルのストリームデータ制御用ポインタ構造体
STREAMDATASHRED StreamFileFunction =
{
	( LONGLONG ( * )( DWORD_PTR StreamDataPoint ) )StreamTell,
	( int ( * )( DWORD_PTR StreamDataPoint, LONGLONG SeekPoint, int SeekType ) )StreamSeek,
	( size_t ( * )( void *Buffer, size_t BlockSize, size_t DataNum, DWORD_PTR StreamDataPoint ) )StreamRead,
	( int ( * )( DWORD_PTR StreamDataPoint ) )StreamEof,
	( int ( * )( DWORD_PTR StreamDataPoint ) )StreamIdleCheck,
	( int ( * )( DWORD_PTR StreamDataPoint ) )StreamClose,
} ;

// メモリのストリームデータ制御用ポインタ構造体
STREAMDATASHRED StreamMemFunction =
{
	MemStreamTell,
	MemStreamSeek,
	MemStreamRead,
	MemStreamEof,
	MemStreamIdleCheck,
	MemStreamClose,
} ;

// プログラム --------------------------------------------------------------------

// ファイルアクセス処理の初期化・終了関数

// ファイルアクセス処理の初期化
extern int InitializeFile( void )
{
	// 既に初期化されていたら何もしない
	if( GFileData.InitializeFlag == TRUE )
	{
		return -1 ;
	}

	DXST_LOGFILE_ADDUTF16LE( "\xd5\x30\xa1\x30\xa4\x30\xeb\x30\xa2\x30\xaf\x30\xbb\x30\xb9\x30\xe6\x51\x06\x74\x6e\x30\x1d\x52\x1f\x67\x16\x53\x2e\x00\x2e\x00\x2e\x00\x00"/*@ L"ファイルアクセス処理の初期化..." @*/ ) ;  

	// ファイル読み込みハンドル管理情報を初期化する
	InitializeHandleManage( DX_HANDLETYPE_FILE, sizeof( FILEACCESSINFO ), MAX_FILE_NUM, InitializeFileHandle, TerminateFileHandle, L"File" ) ;

	// ストリームデータ読みこみ制御用ポインタ配列のデフォルト値をセット
	{
		StreamFileFunction.Read      = ( size_t   ( * )( void *Buffer, size_t BlockSize, size_t DataNum, DWORD_PTR StreamDataPoint) )StreamRead ;
		StreamFileFunction.Eof       = ( int      ( * )( DWORD_PTR StreamDataPoint) )StreamEof ;
		StreamFileFunction.IdleCheck = ( int      ( * )( DWORD_PTR StreamDataPoint) )StreamIdleCheck ;
		StreamFileFunction.Seek      = ( int      ( * )( DWORD_PTR StreamDataPoint, LONGLONG SeekPoint, int SeekType ) )StreamSeek ;
		StreamFileFunction.Tell      = ( LONGLONG ( * )( DWORD_PTR StreamDataPoint) )StreamTell ;
		StreamFileFunction.Close     = ( int      ( * )( DWORD_PTR StreamDataPoint) )StreamClose ;
/*
		StreamFileFunction.Read  = ( size_t ( * )( void *Buffer, size_t BlockSize, size_t DataNum, DWORD_PTR StreamDataPoint) )fread ;
//		StreamFileFunction.Write = ( size_t ( * )( void *Buffer, size_t BlockSize, size_t DataNum, DWORD_PTR StreamDataPoint) )fwrite ;
		StreamFileFunction.Eof   = ( int ( * )( DWORD_PTR StreamDataPoint) )feof ;
		StreamFileFunction.Seek  = ( int ( * )( void *StreamDataPoint, long SeekPoint, int SeekType ) )fseek ;
		StreamFileFunction.Tell  = ( long ( * )( DWORD_PTR StreamDataPoint) )ftell ;
		StreamFileFunction.Close = ( int ( * )( DWORD_PTR StreamDataPoint) )fclose ;
*/

		StreamMemFunction.Read      = MemStreamRead ;
//		StreamMemFunction.Write     = MemStreamWrite;
		StreamMemFunction.Eof       = MemStreamEof ;
		StreamMemFunction.IdleCheck = MemStreamIdleCheck ;
		StreamMemFunction.Seek      = MemStreamSeek ;
		StreamMemFunction.Tell      = MemStreamTell ;
		StreamMemFunction.Close     = MemStreamClose ;
	}

	// 環境依存の処理を行う
	if( InitializeFile_PF() < 0 )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x31\x59\x57\x65\x0a\x00\x00"/*@ L"失敗\n" @*/ ) ;
		return -1 ;
	}

	DXST_LOGFILE_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"成功\n" @*/ ) ;

	// 初期化フラグを立てる
	GFileData.InitializeFlag = TRUE ;

	// 正常終了
	return 0 ;
}

// ファイルアクセス処理の後始末
extern int TerminateFile( void )
{
	// 既に後始末されていたら何もしない
	if( GFileData.InitializeFlag == FALSE )
	{
		return -1 ;
	}

	// 環境依存の処理を行う
	if( TerminateFile_PF() < 0 )
	{
		return -1 ;
	}

	// ファイルハンドル管理情報の後始末
	TerminateHandleManage( DX_HANDLETYPE_FILE ) ;

	// 初期化フラグを倒す
	GFileData.InitializeFlag = FALSE ;

	// 正常終了
	return 0 ;
}

// ＤＸライブラリでストリームデータアクセスに使用する関数を変更する
extern int NS_ChangeStreamFunction( const STREAMDATASHREDTYPE2 *StreamThread )
{
	// 関数ポインタを更新する
	if( StreamThread == NULL )
	{
		return NS_ChangeStreamFunctionW( NULL ) ;
	}
	else
	{
		StreamFunction = *StreamThread ;
		StreamFunctionW.Open		= StreamTCHAR_Open ;
		StreamFunctionW.Close		= StreamTCHAR_Close ;
		StreamFunctionW.Tell		= StreamTCHAR_Tell ;
		StreamFunctionW.Seek		= StreamTCHAR_Seek ;
		StreamFunctionW.Read		= StreamTCHAR_Read ;
		StreamFunctionW.Eof 		= StreamTCHAR_Eof ;
		StreamFunctionW.IdleCheck	= StreamTCHAR_IdleCheck ;
		StreamFunctionW.ChDir		= StreamTCHAR_ChDir ;
		StreamFunctionW.GetDir		= StreamTCHAR_GetDir ;
		StreamFunctionW.GetDirS		= StreamTCHAR_GetDirS ;
		StreamFunctionW.FindFirst	= StreamTCHAR_FindFirst ;
		StreamFunctionW.FindNext	= StreamTCHAR_FindNext ;
		StreamFunctionW.FindClose	= StreamTCHAR_FindClose ;
	}
	
	// 終了
	return 0 ;
}

// ＤＸライブラリでストリームデータアクセスに使用する関数を変更する( wchar_t 使用版 )
extern int NS_ChangeStreamFunctionW( const STREAMDATASHREDTYPE2W *StreamThreadW )
{
	// 関数ポインタを更新する
	if( StreamThreadW == NULL )
	{
		// アーカイブファイルを読み込む機能を使用するかどうかでセットする関数ポインタを変更
#ifndef DX_NON_DXA
		if( NotUseDXArchive == TRUE )
#endif
		{
			// 使用しない場合
			StreamFunctionW.Open		= ReadOnlyFileAccessOpen ;
			StreamFunctionW.Close		= ReadOnlyFileAccessClose ;
			StreamFunctionW.Tell		= ReadOnlyFileAccessTell ;
			StreamFunctionW.Seek		= ReadOnlyFileAccessSeek ;
			StreamFunctionW.Read		= ReadOnlyFileAccessRead ;
			StreamFunctionW.Eof 		= ReadOnlyFileAccessEof ;
			StreamFunctionW.IdleCheck	= ReadOnlyFileAccessIdleCheck ;
			StreamFunctionW.ChDir		= ReadOnlyFileAccessChDir ;
			StreamFunctionW.GetDir		= ReadOnlyFileAccessGetDir ;
			StreamFunctionW.GetDirS		= ReadOnlyFileAccessGetDirS ;
			StreamFunctionW.FindFirst	= ReadOnlyFileAccessFindFirst ;
			StreamFunctionW.FindNext	= ReadOnlyFileAccessFindNext ;
			StreamFunctionW.FindClose	= ReadOnlyFileAccessFindClose ;
		}
#ifndef DX_NON_DXA
		else
		{
			// 使用する場合
			StreamFunctionW.Open		= DXA_DIR_Open ;
			StreamFunctionW.Close		= DXA_DIR_Close ;
			StreamFunctionW.Tell		= DXA_DIR_Tell ;
			StreamFunctionW.Seek		= DXA_DIR_Seek ;
			StreamFunctionW.Read		= DXA_DIR_Read ;
			StreamFunctionW.Eof 		= DXA_DIR_Eof ;
			StreamFunctionW.IdleCheck	= DXA_DIR_IdleCheck ;
			StreamFunctionW.ChDir		= DXA_DIR_ChDir ;
			StreamFunctionW.GetDir		= DXA_DIR_GetDir ;
			StreamFunctionW.GetDirS		= DXA_DIR_GetDirS ;
			StreamFunctionW.FindFirst	= DXA_DIR_FindFirst ;
			StreamFunctionW.FindNext	= DXA_DIR_FindNext ;
			StreamFunctionW.FindClose	= DXA_DIR_FindClose ;
		}
#endif
	}
	else
	{
		StreamFunctionW = *StreamThreadW ;
	}
	
	// 終了
	return 0 ;
}

// ＤＸライブラリでストリームデータアクセスに使用する関数がデフォルトのものか調べる( TRUE:デフォルト  FALSE:デフォルトではない )
extern int NS_GetStreamFunctionDefault( void )
{
	// アーカイブファイルを読み込む機能を使用するかどうかで比較する関数を変更
#ifndef DX_NON_DXA
	if( NotUseDXArchive == TRUE )
#endif
	{
		// 使用しない場合
		if( StreamFunctionW.Open		!= ReadOnlyFileAccessOpen       ) return FALSE ;
		if( StreamFunctionW.Close		!= ReadOnlyFileAccessClose      ) return FALSE ;
		if( StreamFunctionW.Tell		!= ReadOnlyFileAccessTell       ) return FALSE ;
		if( StreamFunctionW.Seek		!= ReadOnlyFileAccessSeek       ) return FALSE ;
		if( StreamFunctionW.Read		!= ReadOnlyFileAccessRead       ) return FALSE ;
		if( StreamFunctionW.Eof 		!= ReadOnlyFileAccessEof        ) return FALSE ;
		if( StreamFunctionW.ChDir		!= ReadOnlyFileAccessChDir      ) return FALSE ;
		if( StreamFunctionW.GetDir		!= ReadOnlyFileAccessGetDir     ) return FALSE ;
		if( StreamFunctionW.GetDirS		!= ReadOnlyFileAccessGetDirS    ) return FALSE ;
		if( StreamFunctionW.FindFirst 	!= ReadOnlyFileAccessFindFirst  ) return FALSE ;
		if( StreamFunctionW.FindNext 	!= ReadOnlyFileAccessFindNext   ) return FALSE ;
		if( StreamFunctionW.FindClose 	!= ReadOnlyFileAccessFindClose  ) return FALSE ;
	}
#ifndef DX_NON_DXA
	else
	{
		// 使用する場合
		if( StreamFunctionW.Open		!= DXA_DIR_Open      ) return FALSE ;
		if( StreamFunctionW.Close		!= DXA_DIR_Close     ) return FALSE ;
		if( StreamFunctionW.Tell		!= DXA_DIR_Tell      ) return FALSE ;
		if( StreamFunctionW.Seek		!= DXA_DIR_Seek      ) return FALSE ;
		if( StreamFunctionW.Read		!= DXA_DIR_Read      ) return FALSE ;
		if( StreamFunctionW.Eof 		!= DXA_DIR_Eof       ) return FALSE ;
		if( StreamFunctionW.ChDir		!= DXA_DIR_ChDir     ) return FALSE ;
		if( StreamFunctionW.GetDir		!= DXA_DIR_GetDir    ) return FALSE ;
		if( StreamFunctionW.GetDirS		!= DXA_DIR_GetDirS   ) return FALSE ;
		if( StreamFunctionW.FindFirst 	!= DXA_DIR_FindFirst ) return FALSE ;
		if( StreamFunctionW.FindNext 	!= DXA_DIR_FindNext  ) return FALSE ;
		if( StreamFunctionW.FindClose 	!= DXA_DIR_FindClose ) return FALSE ;
	}
#endif
	
	return TRUE ;
}











// 補助関係関数

// フルパスではないパス文字列をフルパスに変換する
// ( CurrentDir はフルパスである必要がある(語尾に『\』があっても無くても良い) )
// ( CurrentDir が NULL の場合は現在のカレントディレクトリを使用する )
extern int NS_ConvertFullPath( const TCHAR *Src, TCHAR *Dest, const TCHAR *CurrentDir )
{
	return ConvertFullPathT_( Src, Dest, BUFFERBYTES_CANCEL, CurrentDir ) ;
}

// フルパスではないパス文字列をフルパスに変換する( CurrentDir はフルパスである必要がある(語尾に『\』があっても無くても良い) )( CurrentDir が NULL の場合は現在のカレントディレクトリを使用する )
extern int NS_ConvertFullPathWithStrLen( const TCHAR *Src, size_t SrcLength, TCHAR *Dest, const TCHAR *CurrentDir, size_t CurrentDirLength )
{
	int Result = -1 ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_BEGIN( Src )
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_BEGIN( CurrentDir )
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_SETUP( Src,        SrcLength,        goto ERR )
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_SETUP( CurrentDir, CurrentDirLength, goto ERR )

	Result = ConvertFullPathT_( UseSrcBuffer, Dest, BUFFERBYTES_CANCEL, UseCurrentDirBuffer ) ;

ERR :

	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( Src )
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( CurrentDir )
	return Result ;
}


















// ストリームデータアクセス関数
extern DWORD_PTR StreamOpen( const wchar_t *Path, int UseCacheFlag, int BlockReadFlag, int UseASyncReadFlag )
{
	return StreamFunctionW.Open( Path, UseCacheFlag, BlockReadFlag, UseASyncReadFlag ) ;
}

extern DWORD_PTR StreamOpenT( const TCHAR *Path, int UseCacheFlag, int BlockFlag, int UseASyncReadFlag )
{
#ifdef UNICODE
	return StreamOpen( Path, UseCacheFlag, BlockFlag, UseASyncReadFlag ) ;
#else
	wchar_t PathBuffer[ FILEPATH_MAX ] ;
	ConvString( ( const char * )Path, -1, _TCHARCODEFORMAT, ( char * )PathBuffer, sizeof( PathBuffer ), WCHAR_T_CHARCODEFORMAT ) ;
	return StreamOpen( PathBuffer, UseCacheFlag, BlockFlag, UseASyncReadFlag ) ;
#endif
}

extern int StreamClose( DWORD_PTR Handle )
{
	return StreamFunctionW.Close( Handle ) ;
}

extern LONGLONG StreamTell( DWORD_PTR Handle )
{
	return StreamFunctionW.Tell( Handle ) ;
}

extern int StreamSeek( DWORD_PTR Handle, LONGLONG SeekPoint, int SeekType )
{
	return StreamFunctionW.Seek( Handle, SeekPoint, SeekType ) ;
}

extern size_t StreamRead( void *Buffer, size_t BlockSize, size_t DataNum, DWORD_PTR Handle )
{
	return StreamFunctionW.Read( Buffer, BlockSize, DataNum, Handle ) ;
}

extern int StreamEof( DWORD_PTR Handle )
{
	return StreamFunctionW.Eof( Handle ) ;
}

extern int StreamIdleCheck( DWORD_PTR Handle )
{
	return StreamFunctionW.IdleCheck( Handle ) ;
}

extern int StreamChDir( const wchar_t *Path )
{
	return StreamFunctionW.ChDir( Path ) ;
}

extern int StreamChDirT( const TCHAR *Path )
{
#ifdef UNICODE
	return StreamChDir( Path ) ;
#else
	wchar_t PathBuffer[ FILEPATH_MAX ] ;
	ConvString( ( const char * )Path, -1, _TCHARCODEFORMAT, ( char * )PathBuffer, sizeof( PathBuffer ), WCHAR_T_CHARCODEFORMAT ) ;
	return StreamChDir( PathBuffer ) ;
#endif
}

extern int StreamGetDirS( wchar_t *Buffer, size_t BufferBytes )
{
	return StreamFunctionW.GetDirS != NULL ?
			StreamFunctionW.GetDirS( Buffer, BufferBytes ) :
			StreamFunctionW.GetDir( Buffer ) ;

}

extern int StreamGetDirTS( TCHAR *Buffer, size_t BufferBytes )
{
#ifdef UNICODE
	return StreamGetDirS( Buffer, BufferBytes ) ;
#else
	wchar_t PathBuffer[ FILEPATH_MAX ] ;
	int Result ;

	Result = StreamGetDirS( PathBuffer, sizeof( wchar_t ) * FILEPATH_MAX ) ;
	ConvString( ( const char * )PathBuffer, -1, WCHAR_T_CHARCODEFORMAT, ( char * )Buffer, BufferBytes, _TCHARCODEFORMAT ) ;
	return Result ;
#endif
}

extern DWORD_PTR StreamFindFirst( const wchar_t *FilePath, FILEINFOW *Buffer )
{
	return StreamFunctionW.FindFirst( FilePath, Buffer ) ;
}

extern DWORD_PTR StreamFindFirstT( const TCHAR *FilePath, FILEINFO *Buffer )
{
#ifdef UNICODE
	FILEINFOW BufferW ;
	DWORD_PTR Result ;

	Result = StreamFindFirst( FilePath, &BufferW ) ;
	if( Buffer != NULL )
	{
		ConvFileIntoWToFileInfo( &BufferW, Buffer ) ;
	}

	return Result ;
#else
	wchar_t PathBuffer[ FILEPATH_MAX ] ;
	FILEINFOW BufferW ;
	DWORD_PTR Result ;

	ConvString( ( const char * )FilePath, -1, _TCHARCODEFORMAT, ( char * )PathBuffer, sizeof( PathBuffer ), WCHAR_T_CHARCODEFORMAT ) ;

	Result = StreamFindFirst( PathBuffer, &BufferW ) ;
	if( Result != ( DWORD_PTR )-1 && Buffer != NULL )
	{
		ConvFileIntoWToFileInfo( &BufferW, Buffer ) ;
	}

	return Result ;
#endif
}

extern int StreamFindNext( DWORD_PTR FindHandle, FILEINFOW *Buffer )
{
	return StreamFunctionW.FindNext( FindHandle, Buffer ) ;
}

extern int StreamFindNextT( DWORD_PTR FindHandle, FILEINFO *Buffer )
{
	FILEINFOW BufferW ;
	int Result ;

	Result = StreamFindNext( FindHandle, &BufferW ) ;
	if( Result != -1 && Buffer != NULL )
	{
		ConvFileIntoWToFileInfo( &BufferW, Buffer ) ;
	}

	return Result ;
}

extern int StreamFindClose( DWORD_PTR FindHandle )
{
	return StreamFunctionW.FindClose( FindHandle ) ;
}

extern	const STREAMDATASHREDTYPE2W *StreamGetStruct( void )
{
	return &StreamFunctionW ;
}













// ファイル情報の wchar_t 版と TCHAR 版のデータ変換関数
extern int ConvFileIntoToFileInfoW( FILEINFO  *Src, FILEINFOW *Dest )
{
#ifdef UNICODE
	*( ( FILEINFO * )Dest ) = *Src ;
#else
	ConvString( ( const char * )Src->Name, -1, _TCHARCODEFORMAT, ( char * )Dest->Name, sizeof( Dest->Name ), WCHAR_T_CHARCODEFORMAT ) ;
	Dest->DirFlag       = Src->DirFlag ;
	Dest->Size          = Src->Size ;
	Dest->CreationTime  = Src->CreationTime ;
	Dest->LastWriteTime = Src->LastWriteTime ;
#endif

	return 0 ;
}

extern int ConvFileIntoWToFileInfo( FILEINFOW *Src, FILEINFO  *Dest )
{
#ifdef UNICODE
	*( ( FILEINFOW * )Dest ) = *Src ;
#else
	ConvString( ( const char * )Src->Name, -1, WCHAR_T_CHARCODEFORMAT, ( char * )Dest->Name, sizeof( Dest->Name ), _TCHARCODEFORMAT ) ;
	Dest->DirFlag       = Src->DirFlag ;
	Dest->Size          = Src->Size ;
	Dest->CreationTime  = Src->CreationTime ;
	Dest->LastWriteTime = Src->LastWriteTime ;
#endif

	return 0 ;
}














// ストリームデータアクセス用関数構造体関係

// ストリームデータ読みこみ制御用関数ポインタ構造体のファイル用構造体を得る
extern STREAMDATASHRED *GetFileStreamDataShredStruct( void )
{
	return &StreamFileFunction ;
}

// ストリームデータ読みこみ制御用関数ポインタ構造体のﾒﾓﾘ用構造体を得る
extern STREAMDATASHRED *GetMemStreamDataShredStruct( void )
{
	return &StreamMemFunction ;
}













// ファイルハンドル関係

// ファイルアクセスハンドルを初期化する
static int InitializeFileHandle( HANDLEINFO * )
{
	// 特に何もしない
	return 0 ;
}

// ファイルアクセスハンドルの後始末を行う
static int TerminateFileHandle( HANDLEINFO *HandleInfo )
{
	FILEACCESSINFO *FileInfo = ( FILEACCESSINFO * )HandleInfo ;

	// ハンドルのタイプによって処理を分岐
	switch( FileInfo->HandleType )
	{
	case FILEHANDLETYPE_NORMAL :
		// ファイルを閉じる
		if( FileInfo->FilePointer )
		{
			DX_FCLOSE( FileInfo->FilePointer ) ;
			FileInfo->FilePointer = 0 ;
		}
		break ;

	case FILEHANDLETYPE_FULLYLOAD :
		// ファイルの内容を格納していたメモリを解放
		if( FileInfo->FileImage != NULL )
		{
			FileInfo->StreamData.ReadShred.Close( FileInfo->StreamData.DataPoint ) ;
			FileInfo->StreamData.DataPoint = NULL ;

			if( FileInfo->AllocFileImage )
			{
				DXFREE( FileInfo->FileImage ) ;
			}
			FileInfo->FileImage = NULL ;
		}
		break ;
	}

	// 終了
	return 0 ;
}

// メモリに展開されたファイルを開く
extern int NS_FileRead_open_mem( const void *FileImage, size_t FileImageSize )
{
	int FileHandle ;
	FILEACCESSINFO *FileInfo ;

	// 初期化されていなかったら初期化する
	if( GFileData.InitializeFlag == FALSE )
	{
		InitializeFile() ;
	}

	// ハンドルの作成
	FileHandle = AddHandle( DX_HANDLETYPE_FILE, FALSE, -1 ) ;
	if( FileHandle == -1 )
	{
		return 0 ;
	}

	if( FILEHCHK_ASYNC( FileHandle, FileInfo ) )
	{
		return 0 ;
	}

	// ファイルの内容は全てメモリにある設定
	FileInfo->HandleType = FILEHANDLETYPE_FULLYLOAD ;

	// ファイルのアドレスを保存
	FileInfo->AllocFileImage = FALSE ;
	FileInfo->FileImage = ( void * )FileImage ;
	FileInfo->FileSize = ( LONGLONG )FileImageSize ;

	// ファイルアクセス用情報をセット
	FileInfo->StreamData.ReadShred = *GetMemStreamDataShredStruct() ;
	FileInfo->StreamData.DataPoint = MemStreamOpen( FileInfo->FileImage, ( size_t )FileInfo->FileSize ) ;
	if( FileInfo->StreamData.DataPoint == NULL )
	{
		FileInfo->AllocFileImage = FALSE ;
		FileInfo->FileImage = NULL ;
		goto ERR ;
	}

	// 終了
	return FileHandle ;

ERR :
	SubHandle( FileHandle ) ;

	return 0 ;
}

// FileRead_open の実処理関数
static int FileRead_open_Static(
	int FileHandle,
	const wchar_t *FilePath,
	int ASync,
	int ASyncThread
)
{
	FILEACCESSINFO *FileInfo ;

	if( ASyncThread )
	{
		if( FILEHCHK_ASYNC( FileHandle, FileInfo ) )
			return -1 ;
	}
	else
	{
		if( FILEHCHK( FileHandle, FileInfo ) )
			return -1 ;
	}
	if( FileInfo->HandleType != FILEHANDLETYPE_NORMAL )
		return -1 ;

	// ファイルを開く
	if( ASync )
	{
		FileInfo->FilePointer = DX_FOPEN_ASYNC( FilePath ) ;
	}
	else
	{
		FileInfo->FilePointer = DX_FOPEN( FilePath ) ;
	}

	// 開けなかったらエラー
	if( FileInfo->FilePointer == 0 )
		return -1 ;

	// ファイルアクセス用情報をセット
	FileInfo->StreamData.ReadShred = *GetFileStreamDataShredStruct() ;
	FileInfo->StreamData.DataPoint = FileInfo->FilePointer ;

	// 正常終了
	return 0 ;
}

#ifndef DX_NON_ASYNCLOAD

// FileRead_open の非同期読み込みスレッドから呼ばれる関数
static void FileRead_open_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	int FileHandle ;
	const wchar_t *FilePath ;
	int ASync ;
	int Addr ;
	int Result ;

	Addr = 0 ;
	FileHandle = GetASyncLoadParamInt(    AParam->Data, &Addr ) ;
	FilePath   = GetASyncLoadParamString( AParam->Data, &Addr ) ;
	ASync      = GetASyncLoadParamInt(    AParam->Data, &Addr ) ;

	Result = FileRead_open_Static( FileHandle, FilePath, ASync, TRUE ) ;

	DecASyncLoadCount( FileHandle ) ;
	if( Result < 0 )
	{
		SubHandle( FileHandle ) ;
	}
}

#endif // DX_NON_ASYNCLOAD

// ファイルを開く
extern int FileRead_open_UseGParam(
	const wchar_t *FilePath,
	int ASync,
	int ASyncLoadFlag
)
{
	int FileHandle ;
	FILEACCESSINFO *FileInfo ;

	// 初期化されていなかったら初期化する
	if( GFileData.InitializeFlag == FALSE )
	{
		InitializeFile() ;
	}

	// ハンドルの作成
	FileHandle = AddHandle( DX_HANDLETYPE_FILE, FALSE, -1 ) ;
	if( FileHandle == -1 )
	{
		return 0 ;
	}

	if( FILEHCHK_ASYNC( FileHandle, FileInfo ) )
	{
		return 0 ;
	}
	FileInfo->HandleType = FILEHANDLETYPE_NORMAL ;

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;
		wchar_t FullPath[ 1024 ] ;

		ConvertFullPathW_( FilePath, FullPath, sizeof( FullPath ) ) ;

		// パラメータに必要なメモリのサイズを算出
		Addr = 0 ;
		AddASyncLoadParamInt(    NULL, &Addr, FileHandle ) ;
		AddASyncLoadParamString( NULL, &Addr, FullPath ) ;
		AddASyncLoadParamInt(    NULL, &Addr, ASync ) ;

		// メモリの確保
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// 処理に必要な情報をセット
		AParam->ProcessFunction = FileRead_open_ASync ;
		Addr = 0 ;
		AddASyncLoadParamInt(    AParam->Data, &Addr, FileHandle ) ;
		AddASyncLoadParamString( AParam->Data, &Addr, FullPath ) ;
		AddASyncLoadParamInt(    AParam->Data, &Addr, ASync ) ;

		// データを追加
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto ERR ;
		}

		// 非同期読み込みカウントをインクリメント
		IncASyncLoadCount( FileHandle, AParam->Index ) ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		if( FileRead_open_Static( FileHandle, FilePath, ASync, FALSE ) < 0 )
			goto ERR ;
	}

	// 終了
	return FileHandle ;

ERR :
	SubHandle( FileHandle ) ;

	return 0 ;
}

// ファイルアクセス関数
extern int NS_FileRead_open( const TCHAR *FilePath , int ASync )
{
#ifdef UNICODE
	return FileRead_open_UseGParam( FilePath, ASync, GetASyncLoadFlag() ) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( FilePath, return -1 )

	Result = FileRead_open_UseGParam( UseFilePathBuffer, ASync, GetASyncLoadFlag() ) ;

	TCHAR_TO_WCHAR_T_STRING_END( FilePath )

	return Result ;
#endif
}

// ファイルを開く
extern int NS_FileRead_open_WithStrLen( const TCHAR *FilePath, size_t FilePathLength, int ASync )
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FilePath, FilePathLength, return -1 )
	Result = FileRead_open_UseGParam( UseFilePathBuffer, ASync, GetASyncLoadFlag() ) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FilePath )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FilePath, FilePathLength, return -1 )
	Result = FileRead_open_UseGParam( UseFilePathBuffer, ASync, GetASyncLoadFlag() ) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FilePath )
#endif
	return Result ;
}

extern LONGLONG NS_FileRead_size( const TCHAR *FilePath )
{
	LONGLONG Result ;
	DWORD_PTR fp ;

#ifdef UNICODE
	fp = DX_FOPEN( FilePath ) ;
#else
	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( FilePath, return -1 )

	fp = DX_FOPEN( UseFilePathBuffer ) ;

	TCHAR_TO_WCHAR_T_STRING_END( FilePath )
#endif

	if( fp == 0 )
	{
		return -1 ;
	}

	DX_FSEEK( fp, 0L, SEEK_END ) ;
	Result = DX_FTELL( fp ) ;
	DX_FCLOSE( fp ) ;
	return Result ;
}

// ファイルのサイズを取得する
extern LONGLONG	NS_FileRead_size_WithStrLen( const TCHAR *FilePath, size_t FilePathLength )
{
	LONGLONG Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FilePath, FilePathLength, return -1 )
	Result = NS_FileRead_size( UseFilePathBuffer ) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FilePath )
#else
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_ONE_BEGIN( FilePath, FilePathLength, return -1 )
	Result = ( int )NS_FileRead_size( UseFilePathBuffer ) ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( FilePath )
#endif
	return Result ;
}

extern int NS_FileRead_close( int FileHandle )
{
	return SubHandle( FileHandle ) ;
}

extern LONGLONG NS_FileRead_tell( int FileHandle )
{
	FILEACCESSINFO *FileInfo ;

	if( FILEHCHK( FileHandle, FileInfo ) )
		return -1 ;

//	if( FileInfo->HandleType != FILEHANDLETYPE_NORMAL )
//		return -1 ;
//
//	return DX_FTELL( FileInfo->FilePointer ) ;

	return FileInfo->StreamData.ReadShred.Tell( FileInfo->StreamData.DataPoint ) ;
}

// FileRead_seek の実処理関数
static int FileRead_seek_Static( int FileHandle, LONGLONG Offset, int Origin, int ASyncThread )
{
	FILEACCESSINFO *FileInfo ;

	if( ASyncThread )
	{
		if( FILEHCHK_ASYNC( FileHandle, FileInfo ) )
			return -1 ;
	}
	else
	{
		if( FILEHCHK( FileHandle, FileInfo ) )
			return -1 ;
	}

//	if( FileInfo->HandleType != FILEHANDLETYPE_NORMAL )
//		return -1 ;
//
//	return DX_FSEEK( FileInfo->FilePointer, Offset, Origin ) ;

	return FileInfo->StreamData.ReadShred.Seek( FileInfo->StreamData.DataPoint, Offset, Origin ) ;
}

#ifndef DX_NON_ASYNCLOAD

// FileRead_seek の非同期読み込みスレッドから呼ばれる関数
static void FileRead_seek_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	FILEACCESSINFO *FileInfo ;
	int FileHandle ;
	LONGLONG Offset ;
	int Origin ;
	int Addr ;

	Addr = 0 ;
	FileHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	Offset = GetASyncLoadParamLONGLONG( AParam->Data, &Addr ) ;
	Origin = GetASyncLoadParamInt( AParam->Data, &Addr ) ;

	if( !FILEHCHK_ASYNC( FileHandle, FileInfo ) )
	{
		FileInfo->HandleInfo.ASyncLoadResult = FileRead_seek_Static( FileHandle, Offset, Origin, TRUE ) ;
	}

	DecASyncLoadCount( FileHandle ) ;
}

#endif // DX_NON_ASYNCLOAD

// ファイルポインタの位置を変更する
extern int FileRead_seek_UseGParam( int FileHandle, LONGLONG Offset, int Origin, int ASyncLoadFlag )
{
	FILEACCESSINFO *FileInfo ;
	int Result = -1 ;

	if( FILEHCHK( FileHandle, FileInfo ) )
		return -1 ;

//	if( FileInfo->HandleType != FILEHANDLETYPE_NORMAL )
//		return -1 ;

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;

		// パラメータに必要なメモリのサイズを算出
		Addr = 0 ;
		AddASyncLoadParamInt( NULL, &Addr, FileHandle ) ;
		AddASyncLoadParamLONGLONG( NULL, &Addr, Offset ) ;
		AddASyncLoadParamInt( NULL, &Addr, Origin ) ;

		// メモリの確保
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto END ;

		// 処理に必要な情報をセット
		AParam->ProcessFunction = FileRead_seek_ASync ;
		Addr = 0 ;
		AddASyncLoadParamInt( AParam->Data, &Addr, FileHandle ) ;
		AddASyncLoadParamLONGLONG( AParam->Data, &Addr, Offset ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, Origin ) ;

		// データを追加
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto END ;
		}

		// 非同期読み込みカウントをインクリメント
		IncASyncLoadCount( FileHandle, AParam->Index ) ;

		Result = 0 ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		Result = FileRead_seek_Static( FileHandle, Offset, Origin, FALSE ) ;
	}

#ifndef DX_NON_ASYNCLOAD
END :
#endif

	// 終了
	return Result ;
}

extern int NS_FileRead_seek( int FileHandle, LONGLONG Offset, int Origin )
{
	return FileRead_seek_UseGParam( FileHandle, Offset, Origin, GetASyncLoadFlag() ) ;
}

// FileRead_read の実処理関数
static int FileRead_read_Static(
	int FileHandle,
	void *Buffer,
	int ReadSize,
	int ASyncThread
)
{
	FILEACCESSINFO *FileInfo ;

	if( ASyncThread )
	{
		if( FILEHCHK_ASYNC( FileHandle, FileInfo ) )
			return -1 ;
	}
	else
	{
		if( FILEHCHK( FileHandle, FileInfo ) )
			return -1 ;
	}

//	if( FileInfo->HandleType != FILEHANDLETYPE_NORMAL )
//		return -1 ;
//
//	return ( int )DX_FREAD( Buffer, 1, ReadSize, FileInfo->FilePointer ) ;

	return ( int )FileInfo->StreamData.ReadShred.Read( Buffer, 1, ReadSize, FileInfo->StreamData.DataPoint ) ;
}

#ifndef DX_NON_ASYNCLOAD

// FileRead_read の非同期読み込みスレッドから呼ばれる関数
static void FileRead_read_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	int FileHandle ;
	void *Buffer ;
	int ReadSize ;
	int Addr ;
	FILEACCESSINFO *FileInfo ;

	Addr = 0 ;
	FileHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	Buffer = GetASyncLoadParamVoidP( AParam->Data, &Addr ) ;
	ReadSize = GetASyncLoadParamInt( AParam->Data, &Addr ) ;

	if( !FILEHCHK_ASYNC( FileHandle, FileInfo ) )
	{
		FileInfo->HandleInfo.ASyncLoadResult = FileRead_read_Static( FileHandle, Buffer, ReadSize, TRUE ) ;
	}

	DecASyncLoadCount( FileHandle ) ;
}

#endif // DX_NON_ASYNCLOAD


// ファイルからデータを読み込む
extern int FileRead_read_UseGParam(
	int FileHandle,
	void *Buffer,
	int ReadSize,
	int ASyncLoadFlag
)
{
	int Result = -1 ;
	FILEACCESSINFO *FileInfo ;

	if( FILEHCHK( FileHandle, FileInfo ) )
		return -1 ;

//	if( FileInfo->HandleType != FILEHANDLETYPE_NORMAL )
//		return -1 ;

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;

		// パラメータに必要なメモリのサイズを算出
		Addr = 0 ;
		AddASyncLoadParamInt( NULL, &Addr, FileHandle ) ;
		AddASyncLoadParamConstVoidP( NULL, &Addr, Buffer ) ;
		AddASyncLoadParamInt( NULL, &Addr, ReadSize ) ;

		// メモリの確保
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto END ;

		// 処理に必要な情報をセット
		AParam->ProcessFunction = FileRead_read_ASync ;
		Addr = 0 ;
		AddASyncLoadParamInt( AParam->Data, &Addr, FileHandle ) ;
		AddASyncLoadParamConstVoidP( AParam->Data, &Addr, Buffer ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, ReadSize ) ;

		// データを追加
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto END ;
		}

		// 非同期読み込みカウントをインクリメント
		IncASyncLoadCount( FileHandle, AParam->Index ) ;

		Result = 0 ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		Result = FileRead_read_Static( FileHandle, Buffer, ReadSize, FALSE ) ;
	}

#ifndef DX_NON_ASYNCLOAD
END :
#endif

	// 終了
	return Result ;
}

extern int NS_FileRead_read( void *Buffer, int ReadSize, int FileHandle )
{
	return FileRead_read_UseGParam( FileHandle, Buffer, ReadSize, GetASyncLoadFlag() ) ;
}

extern int NS_FileRead_idle_chk( int FileHandle )
{
	FILEACCESSINFO *FileInfo ;

	if( FILEHCHK_ASYNC( FileHandle, FileInfo ) )
		return -1 ;

#ifndef DX_NON_ASYNCLOAD
	if( FileInfo->HandleInfo.ASyncLoadCount != 0 )
		return FALSE ;
#endif // DX_NON_ASYNCLOAD

//	if( FileInfo->HandleType != FILEHANDLETYPE_NORMAL )
//		return -1 ;
//
//	return DX_FIDLECHK( FileInfo->FilePointer ) ;

	return FileInfo->StreamData.ReadShred.IdleCheck( FileInfo->StreamData.DataPoint ) ;
}

// ファイルの終端かどうかを得る
extern int NS_FileRead_eof( int FileHandle )
{
	LONGLONG FileSize, NowPoint ;
	FILEACCESSINFO *FileInfo ;

	if( FILEHCHK( FileHandle, FileInfo ) )
		return -1 ;

//	if( FileInfo->HandleType != FILEHANDLETYPE_NORMAL )
//		return -1 ;
//
//	NowPoint = DX_FTELL( FileInfo->FilePointer                     ) ;
//	           DX_FSEEK( FileInfo->FilePointer,        0, SEEK_END ) ;
//	FileSize = DX_FTELL( FileInfo->FilePointer                     ) ;
//	           DX_FSEEK( FileInfo->FilePointer, NowPoint, SEEK_SET ) ;

	NowPoint = FileInfo->StreamData.ReadShred.Tell( FileInfo->StreamData.DataPoint ) ;
	           FileInfo->StreamData.ReadShred.Seek( FileInfo->StreamData.DataPoint, 0,        SEEK_END ) ;
	FileSize = FileInfo->StreamData.ReadShred.Tell( FileInfo->StreamData.DataPoint ) ;
	           FileInfo->StreamData.ReadShred.Seek( FileInfo->StreamData.DataPoint, NowPoint, SEEK_SET ) ;

	return NowPoint == FileSize ;
}

// ファイル形式チェック
static int FileRead_CheckFormat( FILEACCESSINFO *FileInfo )
{
	BYTE Head[ 3 ] ;
	LONGLONG NowPos ;
	LONGLONG ReadSize ;

	NowPos = FileInfo->StreamData.ReadShred.Tell( FileInfo->StreamData.DataPoint ) ;
	if( NowPos == 0 )
	{
		ReadSize = ( LONGLONG )FileInfo->StreamData.ReadShred.Read( Head, 1, 3, FileInfo->StreamData.DataPoint ) ;
		STREAM_FSYNC( &FileInfo->StreamData )
		if( ReadSize == 0 )
		{
			return -1 ;
		}

		// UTF16LE 判定
		if( ReadSize >= 2 && Head[ 0 ] == 0xff && Head[ 1 ] == 0xfe )
		{
			FileInfo->CharCodeFormat = DX_CHARCODEFORMAT_UTF16LE ;
			FileInfo->StreamData.ReadShred.Seek( FileInfo->StreamData.DataPoint, 2, SEEK_SET ) ;
		}
		else
		// UTF16BE 判定
		if( ReadSize >= 2 && Head[ 0 ] == 0xfe && Head[ 1 ] == 0xff )
		{
			FileInfo->CharCodeFormat = DX_CHARCODEFORMAT_UTF16BE ;
			FileInfo->StreamData.ReadShred.Seek( FileInfo->StreamData.DataPoint, 2, SEEK_SET ) ;
		}
		else
		// UTF8 判定
		if( ReadSize >= 3 && Head[ 0 ] == 0xef && Head[ 1 ] == 0xbb && Head[ 2 ] == 0xbf )
		{
			FileInfo->CharCodeFormat = DX_CHARCODEFORMAT_UTF8 ;
			FileInfo->StreamData.ReadShred.Seek( FileInfo->StreamData.DataPoint, 3, SEEK_SET ) ;
		}
		else
		{
			FileInfo->StreamData.ReadShred.Seek( FileInfo->StreamData.DataPoint, 0, SEEK_SET ) ;
		}
	}

	// 正常終了
	return 0 ;
}

// ファイルの文字コード形式を設定する( テキストファイル用 )
extern int NS_FileRead_set_format( int FileHandle, int CharCodeFormat /* DX_CHARCODEFORMAT_SHIFTJIS 等 */ )
{
	FILEACCESSINFO *FileInfo ;

	if( FILEHCHK( FileHandle, FileInfo ) )
	{
		return -1 ;
	}

	// フォーマットをセット
	FileInfo->CharCodeFormat = CharCodeFormat ;

	// 正常終了
	return 0 ;
}

extern int NS_FileRead_gets( TCHAR *Buffer, int BufferSize, int FileHandle )
{
	LONGLONG i ;
	FILEACCESSINFO *FileInfo ;
	int UnitSize ;
	int FileUnitSize ;
	int FileCharCodeFormat ;

	if( FILEHCHK( FileHandle, FileInfo ) )
	{
		return -1 ;
	}

//	if( FileInfo->HandleType != FILEHANDLETYPE_NORMAL )
//	{
//		return -1 ;
//	}

	if( FileInfo->StreamData.ReadShred.Eof( FileInfo->StreamData.DataPoint ) != 0 )
	{
		return -1 ;
	}

	// ファイルの先頭の場合はファイル形式チェック
	if( FileRead_CheckFormat( FileInfo ) < 0 )
	{
		return -1 ;
	}

	UnitSize = _TCHARSIZE ;

	FileUnitSize       = FileInfo->CharCodeFormat == 0 ? _TCHARSIZE       : GetCharCodeFormatUnitSize( FileInfo->CharCodeFormat ) ;
	FileCharCodeFormat = FileInfo->CharCodeFormat == 0 ? _TCHARCODEFORMAT :                            FileInfo->CharCodeFormat   ;

	for( i = 0 ; i < BufferSize - 1 ; )
	{
		size_t res ;
		DWORD CharCode1 ;
		int   CharBytes1 ;
		DWORD CharCode2 ;
		int   CharBytes2 ;
		char  DestChar1[ 64 ] ;
		int   DestCharBytes1 ;
		char  DestChar2[ 64 ] ;
		int   DestCharBytes2 ;
		LONGLONG NowPos1 ;
		LONGLONG NowPos2 ;

		NowPos1 = FileInfo->StreamData.ReadShred.Tell( FileInfo->StreamData.DataPoint ) ;
		res = FileRead_scanf_base_help_getchar( &CharCode1, FileUnitSize, &CharBytes1, &FileInfo->StreamData, FileCharCodeFormat ) ;
		if( res == 0 || CharCode1 == '\0' )
		{
			break ;
		}
		DestCharBytes1 = PutCharCode( ConvCharCode( CharCode1, FileCharCodeFormat, _TCHARCODEFORMAT ), _TCHARCODEFORMAT, DestChar1, sizeof( DestChar1 ) ) ;

		if( CharCode1 == '\r' )
		{
			NowPos2 = FileInfo->StreamData.ReadShred.Tell( FileInfo->StreamData.DataPoint ) ;
			res = FileRead_scanf_base_help_getchar( &CharCode2, FileUnitSize, &CharBytes2, &FileInfo->StreamData, FileCharCodeFormat ) ;
			if( res == 0 )
			{
				if( i + DestCharBytes1 / UnitSize > BufferSize - 1 )
				{
					FileInfo->StreamData.ReadShred.Seek( FileInfo->StreamData.DataPoint, NowPos1, SEEK_SET ) ;
					break ;
				}
				_MEMCPY( &( ( BYTE * )Buffer )[ i * UnitSize ], DestChar1, DestCharBytes1 ) ;
				i += DestCharBytes1 / UnitSize ;
				break ;
			}
			DestCharBytes2 = PutCharCode( ConvCharCode( CharCode2, FileCharCodeFormat, _TCHARCODEFORMAT ), _TCHARCODEFORMAT, DestChar2, sizeof( DestChar2 ) ) ;

			if( CharCode2 == '\n' )
			{
				break ;
			}
			else
			{
				if( i + DestCharBytes1 / UnitSize > BufferSize - 1 )
				{
					FileInfo->StreamData.ReadShred.Seek( FileInfo->StreamData.DataPoint, NowPos1, SEEK_SET ) ;
					break ;
				}
				_MEMCPY( &( ( BYTE * )Buffer )[ i * UnitSize ], DestChar1, DestCharBytes1 ) ;
				i += DestCharBytes1 / UnitSize ;

				if( i + DestCharBytes2 / UnitSize > BufferSize - 1 )
				{
					FileInfo->StreamData.ReadShred.Seek( FileInfo->StreamData.DataPoint, NowPos2, SEEK_SET ) ;
					break ;
				}
				_MEMCPY( &( ( BYTE * )Buffer )[ i * UnitSize ], DestChar2, DestCharBytes2 ) ;
				i += DestCharBytes2 / UnitSize ;
			}
		}
		else
		{
			if( i + DestCharBytes1 / UnitSize > BufferSize - 1 )
			{
				FileInfo->StreamData.ReadShred.Seek( FileInfo->StreamData.DataPoint, NowPos1, SEEK_SET ) ;
				break ;
			}
			_MEMCPY( &( ( BYTE * )Buffer )[ i * UnitSize ], DestChar1, DestCharBytes1 ) ;
			i += DestCharBytes1 / UnitSize ;
		}
	}

	PutCharCode( '\0', _TCHARCODEFORMAT, ( char * )&( ( BYTE * )Buffer )[ i * UnitSize ], ( size_t )( ( BufferSize - i ) * UnitSize ) ) ;

	return ( int )i ;
}

// ファイルから一文字読み出す
extern TCHAR NS_FileRead_getc( int FileHandle )
{
	size_t ReadSize ;
	TCHAR c ;
	FILEACCESSINFO *FileInfo ;

	if( FILEHCHK( FileHandle, FileInfo ) )
		return _T( '\0' ) ;

//	if( FileInfo->HandleType != FILEHANDLETYPE_NORMAL )
//		return ( TCHAR )-1 ;

	if( FileInfo->StreamData.ReadShred.Eof( FileInfo->StreamData.DataPoint ) != 0 )
	{
		return ( TCHAR )EOF ;
	}

	c = 0 ;
	ReadSize = FileInfo->StreamData.ReadShred.Read( &c, sizeof( TCHAR ), 1, FileInfo->StreamData.DataPoint ) ;
	STREAM_FSYNC( &FileInfo->StreamData )
	if( ReadSize == 0 )
	{
		return ( TCHAR )EOF ;
	}

	return c ;
}

// ファイルから書式化されたデータを読み出す
extern int NS_FileRead_scanf( int FileHandle, const TCHAR *Format, ... )
{
	va_list param;
	int Result;

	va_start( param, Format );

	Result = FileRead_scanf_base( FileHandle, Format, param ) ;

	va_end( param );

	return Result;
}

// ファイルの情報を取得する(戻り値  -1:エラー  -1以外:ファイル情報ハンドル  )
extern DWORD_PTR NS_FileRead_createInfo( const TCHAR *ObjectPath )
{
	int           Num, i ;
	DWORD_PTR     FindHandle ;
	FILEINFO     *FileInfo ;
	FILEINFOLIST *FileInfoList ;

	FindHandle = StreamFindFirstT( ObjectPath, NULL ) ;
	if( FindHandle == ( DWORD_PTR )-1 )
	{
		return ( DWORD_PTR )-1 ;
	}

	Num = 0 ;
	do
	{
		Num ++ ;
	}while( StreamFindNextT( FindHandle, NULL ) == 0 ) ;

	DX_FFINDCLOSE( FindHandle );

	FileInfoList = (FILEINFOLIST *)DXALLOC( sizeof( FILEINFOLIST ) + sizeof( FILEINFO ) * Num ) ;
	if( FileInfoList == NULL )
	{
		return ( DWORD_PTR )-1 ;
	}

	FileInfoList->Num	= Num;
	FileInfo			= ( FILEINFO * )( FileInfoList + 1 ) ;
	FindHandle			= StreamFindFirstT( ObjectPath, FileInfo ) ;
	FileInfo ++ ;
	for( i = 1 ; i < Num; i ++, FileInfo ++ )
	{
		StreamFindNextT( FindHandle, FileInfo ) ;
	}

	DX_FFINDCLOSE( FindHandle ) ;

	return ( DWORD_PTR )FileInfoList ;
}

// ファイル情報ハンドルを作成する( 戻り値  -1:エラー  -1以外:ファイル情報ハンドル )
extern DWORD_PTR NS_FileRead_createInfo_WithStrLen( const TCHAR *ObjectPath, size_t ObjectPathLength )
{
	DWORD_PTR Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( ObjectPath, ObjectPathLength, return ( DWORD_PTR )-1 )
	Result = NS_FileRead_createInfo( UseObjectPathBuffer ) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( ObjectPath )
#else
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_ONE_BEGIN( ObjectPath, ObjectPathLength, return ( DWORD_PTR )-1 )
	Result = NS_FileRead_createInfo( UseObjectPathBuffer ) ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( ObjectPath )
#endif
	return Result ;
}

// ファイル情報ハンドル中のファイルの数を取得する
extern int NS_FileRead_getInfoNum( DWORD_PTR FileInfoHandle )
{
	FILEINFOLIST *FileInfoList ;

	FileInfoList = ( FILEINFOLIST * )FileInfoHandle;

	return FileInfoList->Num;
}

// ファイル情報ハンドル中のファイルの情報を取得する
extern int NS_FileRead_getInfo( int Index, FILEINFO *Buffer, DWORD_PTR FileInfoHandle )
{
	FILEINFOLIST *FileInfoList ;

	FileInfoList = ( FILEINFOLIST * )FileInfoHandle ;
	if( Index < 0 || FileInfoList->Num <= Index )
	{
		return -1 ;
	}

	*Buffer = ( ( FILEINFO * )( FileInfoList + 1 ) )[ Index ] ;

	return 0;
}

// ファイル情報ハンドルを削除する
extern int NS_FileRead_deleteInfo( DWORD_PTR FileInfoHandle )
{
	DXFREE( ( FILEINFOLIST * )FileInfoHandle ) ;

	return 0;
}

// 指定のファイル又はフォルダの情報を取得し、ファイル検索ハンドルも作成する( 戻り値: -1=エラー  -1以外=ファイル検索ハンドル )
extern DWORD_PTR NS_FileRead_findFirst( const TCHAR *FilePath, FILEINFO *Buffer )
{
	return StreamFindFirstT( FilePath, Buffer );
}

// 指定のファイル又はフォルダの情報を取得し、ファイル検索ハンドルも作成する( 戻り値: -1=エラー  -1以外=ファイル検索ハンドル )
extern DWORD_PTR NS_FileRead_findFirst_WithStrLen( const TCHAR *FilePath, size_t FilePathLength, FILEINFO *Buffer )
{
	DWORD_PTR Result ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_ONE_BEGIN( FilePath, FilePathLength, return ( DWORD_PTR )-1 )
	Result = StreamFindFirstT( UseFilePathBuffer, Buffer ) ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( FilePath )
	return Result ;
}

// 条件の合致する次のファイルの情報を取得する( 戻り値: -1=エラー  0=成功 )
extern int NS_FileRead_findNext( DWORD_PTR FindHandle, FILEINFO *Buffer )
{
	return StreamFindNextT( FindHandle, Buffer );
}

// ファイル検索ハンドルを閉じる( 戻り値: -1=エラー  0=成功 )
extern int NS_FileRead_findClose( DWORD_PTR FindHandle )
{
	return StreamFindClose( FindHandle ) ;
}








// FileRead_fullyLoad の実処理関数
static int FileRead_fullyLoad_Static(
	int FileHandle,
	const wchar_t *FilePath,
	int ASyncThread
)
{
	FILEACCESSINFO *FileInfo ;
	DWORD_PTR FilePointer ;

	if( ASyncThread )
	{
		if( FILEHCHK_ASYNC( FileHandle, FileInfo ) )
			return -1 ;
	}
	else
	{
		if( FILEHCHK( FileHandle, FileInfo ) )
			return -1 ;
	}

	if( FileInfo->HandleType != FILEHANDLETYPE_FULLYLOAD )
	{
		return -1 ;
	}

	// ファイルを開く
	FilePointer = DX_FOPEN( FilePath ) ;

	// 開けなかったらエラー
	if( FilePointer == 0 )
	{
		return -1 ;
	}

	// ファイルのサイズを取得
	DX_FSEEK( FilePointer, 0, SEEK_END ) ;
	FileInfo->FileSize = DX_FTELL( FilePointer ) ;
	DX_FSEEK( FilePointer, 0, SEEK_SET ) ;

	// ファイルを読み込むメモリ領域を確保する
	FileInfo->AllocFileImage = TRUE ;
	FileInfo->FileImage = DXALLOC( ( size_t )FileInfo->FileSize ) ;
	if( FileInfo->FileImage == NULL )
	{
		DX_FCLOSE( FilePointer ) ;
		FilePointer = 0 ;
		return -1 ;
	}

	// ファイルの読み込み
	DX_FREAD( FileInfo->FileImage, FileInfo->FileSize, 1, FilePointer ) ;

	// ファイルを閉じる
	DX_FCLOSE( FilePointer ) ;
	FilePointer = 0 ;

	// ファイルアクセス用情報をセット
	FileInfo->StreamData.ReadShred = *GetMemStreamDataShredStruct() ;
	FileInfo->StreamData.DataPoint = MemStreamOpen( FileInfo->FileImage, ( size_t )FileInfo->FileSize ) ;
	if( FileInfo->StreamData.DataPoint == NULL )
	{
		DXFREE( FileInfo->FileImage ) ;
		FileInfo->AllocFileImage = FALSE ;
		FileInfo->FileImage = NULL ;
		return -1 ;
	}

	// 正常終了
	return 0 ;
}

#ifndef DX_NON_ASYNCLOAD

// FileRead_fullyLoad の非同期読み込みスレッドから呼ばれる関数
static void FileRead_fullyLoad_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	int FileHandle ;
	const wchar_t *FilePath ;
	int Addr ;
	int Result ;

	Addr = 0 ;
	FileHandle = GetASyncLoadParamInt(    AParam->Data, &Addr ) ;
	FilePath   = GetASyncLoadParamString( AParam->Data, &Addr ) ;

	Result = FileRead_fullyLoad_Static( FileHandle, FilePath, TRUE ) ;

	DecASyncLoadCount( FileHandle ) ;
	if( Result < 0 )
	{
		SubHandle( FileHandle ) ;
	}
}

#endif // DX_NON_ASYNCLOAD

// 指定のファイルの内容を全てメモリに読み込み、その情報のアクセスに必要なハンドルを返す( 戻り値  -1:エラー  -1以外:ハンドル )、使い終わったらハンドルは FileRead_fullyLoad_delete で削除する必要があります
extern	int FileRead_fullyLoad_UseGParam( const wchar_t *FilePath, int ASyncLoadFlag )
{
	int FileHandle ;
	FILEACCESSINFO *FileInfo ;

	// 初期化されていなかったら初期化する
	if( GFileData.InitializeFlag == FALSE )
	{
		InitializeFile() ;
	}

	// ハンドルの作成
	FileHandle = AddHandle( DX_HANDLETYPE_FILE, FALSE, -1 ) ;
	if( FileHandle == -1 )
	{
		return -1 ;
	}

	if( FILEHCHK_ASYNC( FileHandle, FileInfo ) )
	{
		return -1 ;
	}
	FileInfo->HandleType = FILEHANDLETYPE_FULLYLOAD ;

	// その他データを初期化
	FileInfo->AllocFileImage = FALSE ;
	FileInfo->FileImage = NULL ;
	FileInfo->FileSize = -1 ;

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;
		wchar_t FullPath[ 1024 ] ;

		ConvertFullPathW_( FilePath, FullPath, sizeof( FullPath ) ) ;

		// パラメータに必要なメモリのサイズを算出
		Addr = 0 ;
		AddASyncLoadParamInt( NULL, &Addr, FileHandle ) ;
		AddASyncLoadParamString( NULL, &Addr, FullPath ) ;

		// メモリの確保
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// 処理に必要な情報をセット
		AParam->ProcessFunction = FileRead_fullyLoad_ASync ;
		Addr = 0 ;
		AddASyncLoadParamInt( AParam->Data, &Addr, FileHandle ) ;
		AddASyncLoadParamString( AParam->Data, &Addr, FullPath ) ;

		// データを追加
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto ERR ;
		}

		// 非同期読み込みカウントをインクリメント
		IncASyncLoadCount( FileHandle, AParam->Index ) ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		if( FileRead_fullyLoad_Static( FileHandle, FilePath, FALSE ) < 0 )
			goto ERR ;
	}

	// 終了
	return FileHandle ;

ERR :
	SubHandle( FileHandle ) ;

	return -1 ;
}

extern	int NS_FileRead_fullyLoad( const TCHAR *FilePath )
{
#ifdef UNICODE
	return FileRead_fullyLoad_UseGParam( FilePath, GetASyncLoadFlag() ) ;
#else
	wchar_t PathBuffer[ FILEPATH_MAX ] ;

	ConvString( ( const char * )FilePath, -1, _TCHARCODEFORMAT, ( char * )PathBuffer, sizeof( PathBuffer ), WCHAR_T_CHARCODEFORMAT ) ;
	return FileRead_fullyLoad_UseGParam( PathBuffer, GetASyncLoadFlag() ) ;
#endif
}

// 指定のファイルの内容を全てメモリに読み込み、その情報のアクセスに必要なハンドルを返す( 戻り値  -1:エラー  -1以外:ハンドル )、使い終わったらハンドルは FileRead_fullyLoad_delete で削除する必要があります
extern int NS_FileRead_fullyLoad_WithStrLen( const TCHAR *FilePath, size_t FilePathLength )
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FilePath, FilePathLength, return -1 )
	Result = FileRead_fullyLoad_UseGParam( UseFilePathBuffer, GetASyncLoadFlag() ) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FilePath )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FilePath, FilePathLength, return -1 )
	Result = FileRead_fullyLoad_UseGParam( UseFilePathBuffer, GetASyncLoadFlag() ) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FilePath )
#endif
	return Result ;
}

// FileRead_fullyLoad で読み込んだファイルをメモリから削除する
extern	int NS_FileRead_fullyLoad_delete( int FLoadHandle )
{
	return SubHandle( FLoadHandle ) ;
}

// FileRead_fullyLoad で読み込んだファイルの内容を格納したメモリアドレスを取得する
extern	const void* NS_FileRead_fullyLoad_getImage( int FLoadHandle )
{
	FILEACCESSINFO *FileInfo ;

	if( FILEHCHK( FLoadHandle, FileInfo ) )
		return NULL ;

	if( FileInfo->HandleType != FILEHANDLETYPE_FULLYLOAD )
		return NULL ;

	// ファイルを格納しているメモリアドレスを返す
	return FileInfo->FileImage ;
}

// FileRead_fullyLoad で読み込んだファイルのサイズを取得する
extern	LONGLONG NS_FileRead_fullyLoad_getSize(	int FLoadHandle )
{
	FILEACCESSINFO *FileInfo ;

	if( FILEHCHK( FLoadHandle, FileInfo ) )
		return -1 ;

	if( FileInfo->HandleType != FILEHANDLETYPE_FULLYLOAD )
		return -1 ;

	// ファイルのサイズを返す
	return FileInfo->FileSize ;
}






// ファイルから書式化されたデータを読み出す
extern int FileRead_scanf_base( int FileHandle, const void  *Format, va_list Param )
{
	int Result;
	FILEACCESSINFO *FileInfo ;

	if( FILEHCHK( FileHandle, FileInfo ) )
		return -1 ;

//	if( FileInfo->HandleType != FILEHANDLETYPE_NORMAL )
//		return -1 ;

#ifdef UNICODE
	Result = FileRead_scanf_baseUTF16LE( FileInfo, ( const WORD * )Format, Param ) ;
#else
	Result = FileRead_scanf_baseCHAR(    FileInfo, ( const char * )Format, Param ) ;
#endif

	return Result;
}


// FileRead_scanf_base で１文字ファイルから読み込む処理を行う関数
static int FileRead_scanf_base_help_getchar( DWORD *CharCode, int UnitSize, int *CharBytesP, STREAMDATA *StreamData, int CharCodeFormat )
{
	int ReadSize = 0 ;
	int CharBytes ;
	size_t Res = 0 ;
	BYTE TempBuffer[ 64 ] ;

	Res = StreamData->ReadShred.Read( &TempBuffer[ ReadSize ], UnitSize, 1, StreamData->DataPoint ) ;
	STREAM_FSYNC( StreamData ) ;
	if( Res == 0 )
	{
		return 0 ;
	}
	ReadSize += UnitSize ;

	CharBytes = GetCharBytes_( ( const char * )TempBuffer, CharCodeFormat ) ;
	if( CharBytes > ReadSize )
	{
		Res = StreamData->ReadShred.Read( &TempBuffer[ ReadSize ], CharBytes - ReadSize, 1, StreamData->DataPoint ) ;
		STREAM_FSYNC( StreamData ) ;
		if( Res == 0 )
		{
			return 0 ;
		}
		ReadSize = CharBytes ;
	}

	*CharCode = GetCharCode( ( const char * )TempBuffer, CharCodeFormat, &CharBytes ) ;

	if( CharBytesP != NULL )
	{
		*CharBytesP = CharBytes ;
	}

	return 1 ;
}

static void FileRead_SkipSpace( STREAMDATA *StreamData, int *Eof, int UnitSize, int CharCodeFormat )
{
	DWORD c ;
	size_t res ;
	int CharBytes ;

	if( Eof )
	{
		*Eof = FALSE ;
	}

	for(;;)
	{
		res = FileRead_scanf_base_help_getchar( &c, UnitSize, &CharBytes, StreamData, CharCodeFormat ) ;
		if( res == 0 )
		{
			if( Eof )
			{
				*Eof = TRUE ;
			}
			return ;
		}

		if( c != ' ' && c != '\n' && c != '\r' && c != '\t' )
		{
			break ;
		}
	}
	StreamData->ReadShred.Seek( StreamData->DataPoint, -CharBytes, SEEK_CUR ) ;
}

// FileRead_scanf_base などで使用する文字列から数値を取得するための関数
static DWORD FileRead_scanf_base_help_getnumber( const DWORD *CharCode, int *UseCharNum )
{
	DWORD Result ;
	DWORD AddNum ;
	int i ;
	DWORD NumberNum ;
	DWORD NumberStr[ 256 ] ;

	for( NumberNum = 0 ; CharCode[ NumberNum ] >= '0' && CharCode[ NumberNum ] <= '9' ; NumberNum ++ )
	{
		NumberStr[ NumberNum ] = CharCode[ NumberNum ] ;
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

// ファイルから書式化されたデータを読み出す
extern int FileRead_scanf_baseCHAR( FILEACCESSINFO *FileInfo, const char *Format, va_list Param )
{
	DWORD c ;
	DWORD str[256] ;
	DWORD Number[256] ;
	DWORD VStr[1024];
	int ReadNum ;
	int Width ;
	int i, j, k ;
	int num ;
	int num2 ;
	int SkipFlag, VStrRFlag, I64Flag, lFlag, hFlag, Eof, MinusFlag, UIntFlag;
	size_t res;
	LONGLONG int64Num ;
	LONGLONG int64Count ;
	int UseCharNum ;
	int UnitSize ;
	int FileUnitSize ;
	int FileCharCodeFormat ;
	int CharBytes ;
	int PutCharBytes ;
	BYTE *pStr ;
	BYTE TempBuffer[ 64 ] ;
	STREAMDATA *StreamData ;

	DWORD FormatStringBaseBuffer[ 1024 ] ;
	DWORD *FormatStringTempBuffer = NULL ;
	int FormatStringSize ;
	DWORD *FCode ;

	StreamData = &FileInfo->StreamData ;

	if( StreamData->ReadShred.Eof( StreamData->DataPoint ) )
	{
		return EOF ;
	}

	// ファイルの先頭の場合はファイル形式チェック
	if( FileRead_CheckFormat( FileInfo ) < 0 )
	{
		return EOF ;
	}

	UnitSize = GetCharCodeFormatUnitSize( CHAR_CHARCODEFORMAT ) ;

	FileUnitSize       = FileInfo->CharCodeFormat == 0 ? _TCHARSIZE       : GetCharCodeFormatUnitSize( FileInfo->CharCodeFormat ) ;
	FileCharCodeFormat = FileInfo->CharCodeFormat == 0 ? _TCHARCODEFORMAT :                            FileInfo->CharCodeFormat   ;

	FormatStringSize = StringToCharCodeString( Format, CHAR_CHARCODEFORMAT, NULL, BUFFERBYTES_CANCEL ) ;
	if( FormatStringSize + sizeof( DWORD ) * 16 > sizeof( FormatStringBaseBuffer ) )
	{
		FormatStringTempBuffer = ( DWORD * )DXALLOC( FormatStringSize + sizeof( DWORD ) * 16 ) ;
		if( FormatStringTempBuffer == NULL )
		{
			return -1 ;
		}

		FCode = FormatStringTempBuffer ;
	}
	else
	{
		FCode = FormatStringBaseBuffer ;
	}
	_MEMSET( FCode, 0, FormatStringSize + sizeof( DWORD ) * 16 ) ;
	StringToCharCodeString( Format, CHAR_CHARCODEFORMAT, FCode, BUFFERBYTES_CANCEL ) ;

	ReadNum = 0;
	Eof = FALSE;
	while( *FCode != '\0' && StreamData->ReadShred.Eof( StreamData->DataPoint ) == 0 )
	{
		// % かどうかで処理を分岐
		if( FCode[ 0 ] == '%' && FCode[ 1 ] != '%' )
		{
			Width		= -1;
			I64Flag		= FALSE;
			lFlag		= FALSE;
			hFlag		= FALSE;
			UIntFlag	= FALSE;
			SkipFlag	= FALSE;

			FCode ++ ;
			if( *FCode == '\0' )
			{
				break;
			}

			if( *FCode == '*' )
			{
				SkipFlag = TRUE ;
				FCode ++ ;
				if( *FCode == '\0' )
				{
					break ;
				}
			}

			if( *FCode >= '0' && *FCode <= '9' )
			{
				Width = ( int )FileRead_scanf_base_help_getnumber( FCode, &UseCharNum ) ;
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
				FCode ++ ;
				break ;

			case 'h':
			case 'H':
				hFlag = TRUE ;
				FCode ++ ;
				break ;

			case 'I':
				if( FCode[1] == '6' && FCode[2] == '4' )
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
				VStrRFlag = FALSE;
				if( *FCode == '^' )
				{
					VStrRFlag = TRUE;
					FCode++;
				}
				j = 0;
				_MEMSET( VStr, 0, sizeof( VStr ) );

				c = '[';
				while( *FCode != '\0' && *FCode != ']' )
				{
					if( *FCode == '-' && c != '[' && FCode[1] != '\0' && FCode[1] != ']' )
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
								*( ( BYTE * )&VStr[ j ] ) = ( BYTE )k ;
								j++;
							}
						}
						FCode += 2;
						c = '[';
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
					break ;
				}

				FCode ++ ;
				pStr = NULL ;
				if( SkipFlag == FALSE )
				{
					pStr = va_arg( Param, BYTE * );
				}

				FileRead_SkipSpace( StreamData, &Eof, FileUnitSize, FileCharCodeFormat ) ;
				if( Eof == TRUE )
				{
					break ;
				}

				i = 0 ;
				for(;;)
				{
					res = FileRead_scanf_base_help_getchar( &c, FileUnitSize, &CharBytes, StreamData, FileCharCodeFormat ) ;
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}

					j = 0;
					while( VStr[ j ] != '\0' )
					{
						if( VStr[ j ] == c )
						{
							break ;
						}
						j ++ ;
					}

					if( ( VStrRFlag == TRUE  && VStr[j] != '\0' ) ||
						( VStrRFlag == FALSE && VStr[j] == '\0' ) )
					{
						break ;
					}

					if( pStr )
					{
						pStr += PutCharCode( ConvCharCode( c, FileCharCodeFormat, CHAR_CHARCODEFORMAT ), CHAR_CHARCODEFORMAT, ( char * )pStr, BUFFERBYTES_CANCEL ) ;
					}
					i ++ ;
					if( Width != 0 && Width == i ) 
					{
						break;
					}
				}

				if( pStr )
				{
					pStr += PutCharCode( '\0', CHAR_CHARCODEFORMAT, ( char * )pStr, BUFFERBYTES_CANCEL ) ;
				}

				if( Eof == FALSE && Width != i )
				{
					StreamData->ReadShred.Seek( StreamData->DataPoint, -CharBytes, SEEK_CUR );
				}
			}
			else if( *FCode == 'd' || *FCode == 'D' || *FCode == 'u' || *FCode == 'U' )
			{
				FileRead_SkipSpace( StreamData, &Eof, FileUnitSize, FileCharCodeFormat );
				if( Eof == TRUE )
				{
					break ;
				}

				FCode ++ ;
				UIntFlag  = *FCode == 'u' || *FCode == 'U' ;
				MinusFlag = FALSE ;
				res = FileRead_scanf_base_help_getchar( &c, FileUnitSize, &CharBytes, StreamData, FileCharCodeFormat ) ;
				if( res == 0 )
				{
					Eof = TRUE;
					break;
				}
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
					res = FileRead_scanf_base_help_getchar( &c, FileUnitSize, &CharBytes, StreamData, FileCharCodeFormat ) ;
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}
STR_10INT:
				for( i = 0; i < 255 && ( Width == -1 || Width != i ) && c >= '0' && c <= '9'; i ++ )
				{
					Number[ i ] = c - '0';
					res = FileRead_scanf_base_help_getchar( &c, FileUnitSize, &CharBytes, StreamData, FileCharCodeFormat ) ;
					if( res == 0 )
					{
						i ++ ;
						Eof = TRUE;
						break;
					}
				}
				if( Eof == FALSE )
				{
					StreamData->ReadShred.Seek( StreamData->DataPoint, -CharBytes, SEEK_CUR );
				}
				num = i;
				if( num == 0 )
				{
					break;
				}
				int64Count = 1 ;
				int64Num   = 0 ;
				for( i = num - 1; i >= 0; i --, int64Count *= 10 )
				{
					if( UIntFlag )
					{
						int64Num = (LONGLONG)( (ULONGLONG)int64Num + (ULONGLONG)Number[i] * (ULONGLONG)int64Count ) ;
					}
					else 
					{
						int64Num += (LONGLONG)Number[i] * int64Count;
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
							*va_arg( Param, ULONGLONG * ) = ( ULONGLONG )int64Num ;
						}
						else
						{
							*va_arg( Param, LONGLONG * ) = ( LONGLONG )int64Num ;
						}
					}
					else if( hFlag )
					{
						if( UIntFlag )
						{
							*va_arg( Param, WORD * ) = ( WORD )int64Num;
						}
						else
						{
							*va_arg( Param, short * ) = ( short )int64Num;
						}
					}
					else
					{
						if( UIntFlag )
						{
							*va_arg( Param, DWORD * ) = ( DWORD )int64Num ;
						}
						else
						{
							*va_arg( Param, int * ) = ( int )int64Num ;
						}
					}
				}
			}
			else
			if( *FCode == 'x' || *FCode == 'X' )
			{
				FileRead_SkipSpace( StreamData, &Eof, FileUnitSize, FileCharCodeFormat );
				if( Eof == TRUE )
				{
					break ;
				}

				FCode ++ ;
				MinusFlag = FALSE ;

				res = FileRead_scanf_base_help_getchar( &c, FileUnitSize, &CharBytes, StreamData, FileCharCodeFormat ) ;
				if( res == 0 )
				{
					Eof = TRUE;
					break;
				}

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

					res = FileRead_scanf_base_help_getchar( &c, FileUnitSize, &CharBytes, StreamData, FileCharCodeFormat ) ;
					if( res == 0 )
					{
						Eof = TRUE ;
						break ;
					}
				}
STR_16INT:
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
						break ;
					}

					i ++ ;
					res = FileRead_scanf_base_help_getchar( &c, FileUnitSize, &CharBytes, StreamData, FileCharCodeFormat ) ;
					if( res == 0 )
					{
						Eof = TRUE ;
						break ;
					}
				}
				if( Eof == FALSE )
				{
					StreamData->ReadShred.Seek( StreamData->DataPoint, -CharBytes, SEEK_CUR );
				}
				num = i ;
				if( num == 0 )
				{
					break ;
				}
				int64Count = 1 ;
				int64Num   = 0 ;
				for( i = num - 1 ; i >= 0 ; i --, int64Count *= 16 )
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
						*va_arg( Param, ULONGLONG * ) = ( ULONGLONG )int64Num ;
					}
					else
					if( hFlag )
					{
						*va_arg( Param, WORD      * ) = ( WORD      )int64Num ;
					}
					else
					{
						*va_arg( Param, DWORD     * ) = ( DWORD     )int64Num ;
					}
				}
			}
			else
			if( *FCode == 'o' || *FCode == 'O' )
			{
				FileRead_SkipSpace( StreamData, &Eof, FileUnitSize, FileCharCodeFormat );
				if( Eof == TRUE ) break;

				FCode ++ ;
				MinusFlag = FALSE ;
				res = FileRead_scanf_base_help_getchar( &c, FileUnitSize, &CharBytes, StreamData, FileCharCodeFormat ) ;
				if( res == 0 )
				{
					Eof = TRUE;
					break;
				}
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
					res = FileRead_scanf_base_help_getchar( &c, FileUnitSize, &CharBytes, StreamData, FileCharCodeFormat ) ;
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}
STR_8INT:
				for( i = 0; i < 255 && ( Width == -1 || Width != i ) && c >= '0' && c <= '7'; i ++ )
				{
					Number[ i ] = c - '0' ;
					res = FileRead_scanf_base_help_getchar( &c, FileUnitSize, &CharBytes, StreamData, FileCharCodeFormat ) ;
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}
				if( Eof == FALSE )
				{
					StreamData->ReadShred.Seek( StreamData->DataPoint, -CharBytes, SEEK_CUR );
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
						*va_arg( Param, ULONGLONG * ) = ( ULONGLONG )int64Num ;
					}
					else
					if( hFlag )
					{
						*va_arg( Param, WORD *      ) =      ( WORD )int64Num ;
					}
					else
					{
						*va_arg( Param, DWORD *     ) =     ( DWORD )int64Num ;
					}
				}
			}
			else
			if( *FCode == 'i' || *FCode == 'I' )
			{
				FileRead_SkipSpace( StreamData, &Eof, FileUnitSize, FileCharCodeFormat );
				if( Eof == TRUE ) break;

				FCode ++ ;
				MinusFlag = FALSE;
				res = FileRead_scanf_base_help_getchar( &c, FileUnitSize, &CharBytes, StreamData, FileCharCodeFormat ) ;
				if( res == 0 )
				{
					Eof = TRUE;
					break;
				}
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
					res = FileRead_scanf_base_help_getchar( &c, FileUnitSize, &CharBytes, StreamData, FileCharCodeFormat ) ;
					if( res == 0 )
					{
						Eof = TRUE ;
						break ;
					}
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
					res = FileRead_scanf_base_help_getchar( &c, FileUnitSize, &CharBytes, StreamData, FileCharCodeFormat ) ;
					if( res == 0 )
					{
						Eof = TRUE ;
						break ;
					}
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
						res = FileRead_scanf_base_help_getchar( &c, FileUnitSize, &CharBytes, StreamData, FileCharCodeFormat ) ;
						if( res == 0 )
						{
							Eof = TRUE ;
							break ;
						}
						goto STR_16INT;
					}
					else
					{
						goto STR_8INT;
					}
				}
				else
				{
					goto STR_10INT;
				}
			}
			else 
			if( *FCode == 'c' || *FCode == 'C' )
			{
				FCode ++ ;
				if( Width == -1 )
				{
					Width = 1 ;
				}
				pStr = NULL;
				if( SkipFlag == FALSE )
				{
					pStr = va_arg( Param, BYTE * ) ;
				}
				for( i = 0; i < Width; i += PutCharBytes / UnitSize )
				{
					res = FileRead_scanf_base_help_getchar( &c, FileUnitSize, &CharBytes, StreamData, FileCharCodeFormat ) ;
					if( res == 0 )
					{
						Eof = TRUE ;
						break ;
					}

					if( pStr != NULL )
					{
						if( FCode[ -1 ] == 'c' )
						{
							PutCharBytes = PutCharCode( ConvCharCode( c, FileCharCodeFormat, CHAR_CHARCODEFORMAT    ), CHAR_CHARCODEFORMAT,    ( char * )pStr, BUFFERBYTES_CANCEL ) ;
						}
						else
						{
							PutCharBytes = PutCharCode( ConvCharCode( c, FileCharCodeFormat, WCHAR_T_CHARCODEFORMAT ), WCHAR_T_CHARCODEFORMAT, ( char * )pStr, BUFFERBYTES_CANCEL ) ;
						}
						pStr += PutCharBytes ;
					}
					else
					{
						if( FCode[ -1 ] == 'c' )
						{
							PutCharBytes = PutCharCode( ConvCharCode( c, FileCharCodeFormat, CHAR_CHARCODEFORMAT    ), CHAR_CHARCODEFORMAT,    ( char * )TempBuffer, sizeof( TempBuffer ) ) ;
						}
						else
						{
							PutCharBytes = PutCharCode( ConvCharCode( c, FileCharCodeFormat, WCHAR_T_CHARCODEFORMAT ), WCHAR_T_CHARCODEFORMAT, ( char * )TempBuffer, sizeof( TempBuffer ) ) ;
						}
					}
				}
			}
			else
			if( *FCode == 's' || *FCode == 'S' )
			{
				FCode ++ ;
				FileRead_SkipSpace( StreamData, &Eof, FileUnitSize, FileCharCodeFormat );
				if( Eof == TRUE )
				{
					break ;
				}

				pStr = NULL ;
				if( SkipFlag == FALSE )
				{
					pStr = va_arg( Param, BYTE * ) ;
				}

				i = 0;
				for(;;)
				{
					res = FileRead_scanf_base_help_getchar( &c, FileUnitSize, &CharBytes, StreamData, FileCharCodeFormat ) ;
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}

					if( c == ' ' || c == '\n' || c == '\r' || c == '\t' )
					{
						StreamData->ReadShred.Seek( StreamData->DataPoint, -CharBytes, SEEK_CUR ) ;
						break ;
					}

					if( pStr != NULL )
					{
						if( FCode[ -1 ] == 's' )
						{
							PutCharBytes = PutCharCode( ConvCharCode( c, FileCharCodeFormat, CHAR_CHARCODEFORMAT    ), CHAR_CHARCODEFORMAT,    ( char * )pStr, BUFFERBYTES_CANCEL ) ;
						}
						else
						{
							PutCharBytes = PutCharCode( ConvCharCode( c, FileCharCodeFormat, WCHAR_T_CHARCODEFORMAT ), WCHAR_T_CHARCODEFORMAT, ( char * )pStr, BUFFERBYTES_CANCEL ) ;
						}
						pStr += PutCharBytes ;
					}
					else
					{
						if( FCode[ -1 ] == 's' )
						{
							PutCharBytes = PutCharCode( ConvCharCode( c, FileCharCodeFormat, CHAR_CHARCODEFORMAT    ), CHAR_CHARCODEFORMAT,    ( char * )TempBuffer, sizeof( TempBuffer ) ) ;
						}
						else
						{
							PutCharBytes = PutCharCode( ConvCharCode( c, FileCharCodeFormat, WCHAR_T_CHARCODEFORMAT ), WCHAR_T_CHARCODEFORMAT, ( char * )TempBuffer, sizeof( TempBuffer ) ) ;
						}
					}

					i += PutCharBytes / UnitSize ;
					if( Width != -1 && i >= Width )
					{
						break ;
					}
				}

				if( pStr != NULL )
				{
					if( FCode[ -1 ] == 's' )
					{
						pStr += PutCharCode( '\0', CHAR_CHARCODEFORMAT, ( char * )pStr, BUFFERBYTES_CANCEL ) ;
					}
					else
					{
						pStr += PutCharCode( L'\0', WCHAR_T_CHARCODEFORMAT, ( char * )pStr, BUFFERBYTES_CANCEL ) ;
					}
				}
			}
			else
			if( *FCode == 'f' || *FCode == 'F' || *FCode == 'g' || *FCode == 'G' || *FCode == 'e' || *FCode == 'E' )
			{
				int num3 ;
				DWORD Number2[256] ;
				DWORD Number3[256] ;
				double doubleNum ;
				double doubleNum2 ;
				double doubleNum3 ;
				double doubleCount ;
				int DotFlag ;
				int IndexFlag ;
				int MinusFlag2 ;

				FileRead_SkipSpace( StreamData, &Eof, FileUnitSize, FileCharCodeFormat );
				if( Eof == TRUE ) break;

				FCode ++ ;
				MinusFlag  = FALSE ;
				DotFlag    = FALSE ;
				IndexFlag  = FALSE ;
				MinusFlag2 = FALSE ;
				res = FileRead_scanf_base_help_getchar( &c, FileUnitSize, &CharBytes, StreamData, FileCharCodeFormat ) ;
				if( res == 0 )
				{
					Eof = TRUE;
					break;
				}
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
					res = FileRead_scanf_base_help_getchar( &c, FileUnitSize, &CharBytes, StreamData, FileCharCodeFormat ) ;
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}

				i = 0 ;	//自然数
				j = 0 ;	//小数
				k = 0 ;	//指数
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
							Number[ i ] = c - '0' ;
							i ++ ;
						}
					}
					else
					{
						break ;
					}

					res = FileRead_scanf_base_help_getchar( &c, FileUnitSize, &CharBytes, StreamData, FileCharCodeFormat ) ;
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}

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
					StreamData->ReadShred.Seek( StreamData->DataPoint, -CharBytes, SEEK_CUR );
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
						doubleNum += Number[ i ] * doubleCount;
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
						doubleNum2 += Number2[ i ] * doubleCount;
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

				doubleNum3 = 1.0;
				if( int64Num != 0 )
				{
					if( int64Num < 0 )
					{
						int64Num = -int64Num;
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

				doubleNum = ( doubleNum + doubleNum2 ) * doubleNum3;

				if( SkipFlag == FALSE )
				{
					if( lFlag )
					{
						*va_arg( Param, double * ) =          doubleNum ;
					}
					else
					{
						*va_arg( Param, float *  ) = ( float )doubleNum ;
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
				FileRead_SkipSpace( StreamData, &Eof, FileUnitSize, FileCharCodeFormat ) ;
				if( Eof == TRUE ) break;
			}
			else
			{
				for( num = 0; *FCode != '\0' && *FCode != ' ' && *FCode != '\n' && *FCode != '\r' && *FCode != '\t' && *FCode != '%'; num++, FCode ++ )
				{
					str[ num ] = *FCode;
				}
				str[ num ] = '\0';

				for( i = 0; i < num; i ++ )
				{
					res = FileRead_scanf_base_help_getchar( &c, FileUnitSize, &CharBytes, StreamData, FileCharCodeFormat ) ;
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}

					if( str[ i ] != c )
					{
						break ;
					}
				}
			}
		}
	}

	if( FormatStringTempBuffer != NULL )
	{
		DXFREE( FormatStringTempBuffer ) ;
		FormatStringTempBuffer = NULL ;
	}

	if( StreamData->ReadShred.Eof( StreamData->DataPoint ) && ReadNum == 0 )
	{
		return EOF;
	}

	return ReadNum;
}
















static void FileRead_SkipSpaceUTF16LE( STREAMDATA *StreamData, int *Eof )
{
	WORD c ;
	size_t res ;

	if( Eof ) *Eof = FALSE ;
	for(;;)
	{
		res = StreamData->ReadShred.Read( &c, sizeof( WORD ), 1, StreamData->DataPoint );
		STREAM_FSYNC( StreamData )
		if( res == 0 )
		{
			if( Eof ) *Eof = TRUE;
			return;
		}
		if( c != L' ' && c != L'\n' && c != L'\r' ) break;
	}
	StreamData->ReadShred.Seek( StreamData->DataPoint, -( int )sizeof( WORD ), SEEK_CUR );
}

static void FileRead_strcpy_UTF16LE( WORD *Dest, const WORD *Src )
{
	int i ;
	for( i = 0 ; Src[i] != L'\0' ; i ++ ) Dest[i] = Src[i] ;
	Dest[i] = L'\0' ;
}

// ファイルから書式化されたデータを読み出す
extern int FileRead_scanf_baseUTF16LE( FILEACCESSINFO *FileInfo, const WORD *FormatUTF16LE, va_list Param )
{
	WORD c, c2[2], tstr[512], str[256], Number[128], Number2[128], Number3[128], VStr[1024];
	int ReadNum, Width, i, j, k, num, num2, num3;
	int SkipFlag, VStrRFlag, I64Flag, lFlag, hFlag, Eof, MinusFlag, UIntFlag;
	int DotFlag, IndexFlag, MinusFlag2 ;
	size_t res;
	LONGLONG int64Num, int64Count;
	double doubleNum, doubleNum2, doubleNum3, doubleCount;
	WORD *pStr;
	char *pStrA ;
	STREAMDATA *StreamData ;

	StreamData = &FileInfo->StreamData ;

	if( StreamData->ReadShred.Eof( StreamData->DataPoint ) ) return EOF;

	if( StreamData->ReadShred.Tell( StreamData->DataPoint ) == 0 )
	{
		StreamData->ReadShred.Read( &c, sizeof( WORD ), 1, StreamData->DataPoint ) ;
		STREAM_FSYNC( StreamData )
		if( *( ( WORD * )&c ) != 0xfeff )
		{
			StreamData->ReadShred.Seek( StreamData->DataPoint, 0, SEEK_SET ) ;
		}
	}

	ReadNum = 0;
	Eof = FALSE;
	while( *FormatUTF16LE != L'\0' && StreamData->ReadShred.Eof( StreamData->DataPoint ) == 0 )
	{
		if( FormatUTF16LE[0] == L'%' && FormatUTF16LE[1] != L'%' )
		{
			Width = -1;
			I64Flag = FALSE;
			lFlag = FALSE;
			hFlag = FALSE;
			UIntFlag = FALSE;
			SkipFlag = FALSE;
			FormatUTF16LE ++ ;
			if( *FormatUTF16LE == L'\0' ) break;
			if( *FormatUTF16LE == L'*' )
			{
				SkipFlag = TRUE;
				FormatUTF16LE ++ ;
			}
			if( *FormatUTF16LE == L'\0' ) break;
			if( *FormatUTF16LE >= L'0' && *FormatUTF16LE <= L'9' )
			{
				for( i = 0; FormatUTF16LE[i] >= L'0' && FormatUTF16LE[i] <= L'9'; i++ )
					str[i] = FormatUTF16LE[i];
				str[i] = L'\0';
				FormatUTF16LE += i;
				Width = CL_atoi( DX_CHARCODEFORMAT_UTF16LE, ( const char * )str ) ;
				if( Width == 0 ) break;
			}
			if( *FormatUTF16LE == L'\0' ) break;
			switch( *FormatUTF16LE )
			{
			case L'l': case L'H': lFlag = TRUE; FormatUTF16LE++ ; break;
			case L'h': case L'L': hFlag = TRUE; FormatUTF16LE++ ; break;
			case L'I':
				if( FormatUTF16LE[1] == L'6' && FormatUTF16LE[2] == L'4' )
				{
					I64Flag = TRUE;
					FormatUTF16LE += 3;
				}
				break;
			}
			if( *FormatUTF16LE == L'\0' ) break;

			if( *FormatUTF16LE == L'[' )
			{
				if( lFlag || hFlag || I64Flag ) break;

				FormatUTF16LE ++ ;
				VStrRFlag = FALSE;
				if( *FormatUTF16LE == L'^' )
				{
					VStrRFlag = TRUE;
					FormatUTF16LE++;
				}
				j = 0;
				_MEMSET( VStr, 0, sizeof( VStr ) );
				c = L'[';
				while( *FormatUTF16LE != L'\0' && *FormatUTF16LE != L']' )
				{
					if( CHECK_CPU_LE_UTF16LE_4BYTE( *FormatUTF16LE ) == TRUE )
					{
						if( FormatUTF16LE[1] == L'\0' )
						{
							FormatUTF16LE++;
							break;
						}
						if( FormatUTF16LE[1] == L']' ) break;
						VStr[j]   = FormatUTF16LE[0];
						VStr[j+1] = FormatUTF16LE[1];
						j += 2 ;
						FormatUTF16LE += 2 ;
						c = L'[';
					}
					else
					{
						if( *FormatUTF16LE == L'-' && c != L'[' && FormatUTF16LE[1] != L'\0' && FormatUTF16LE[1] != L']' )
						{
							num  = (int)(WORD)c;
							num2 = (int)(WORD)FormatUTF16LE[1];
							if( num2 < num )
							{
								k = num2; num2 = num; num = k;
							}
							for( k = num; k <= num2; k++ )
							{
								if( c != k )
								{
									*((WORD *)&VStr[j]) = (WORD)k;
									j++;
								}
							}
							FormatUTF16LE += 2;
							c = L'[';
						}
						else
						{
							VStr[j] = *FormatUTF16LE;
							c = *FormatUTF16LE;
							j ++ ;
							FormatUTF16LE ++ ;
						}
					}
				}
				if( *FormatUTF16LE == L'\0' ) break;
				FormatUTF16LE ++ ;
				pStr = NULL;
				if( SkipFlag == FALSE )
				{
					pStr = va_arg( Param, WORD * );
				}
				FileRead_SkipSpaceUTF16LE( StreamData, &Eof );
				if( Eof == TRUE ) break;
				i = 0;
				for(;;)
				{
					res = StreamData->ReadShred.Read( &c, sizeof( WORD ), 1, StreamData->DataPoint );
					STREAM_FSYNC( StreamData )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
					if( CHECK_CPU_LE_UTF16LE_4BYTE( c ) == TRUE )
					{
						c2[0] = c;
						res = StreamData->ReadShred.Read( &c, sizeof( WORD ), 1, StreamData->DataPoint );
						STREAM_FSYNC( StreamData )
						if( res == 0 )
						{
							Eof = TRUE;
							break;
						}
						c2[1] = c;

						j = 0;
						while( VStr[j] != L'\0' )
						{
							if( CHECK_CPU_LE_UTF16LE_4BYTE( VStr[j] ) == TRUE )
							{
								if( VStr[j] == c2[0] && VStr[j+1] == c2[1] ) break;
								j += 2;
							}
							else
							{
								j ++ ;
							}
						}

						if( ( VStrRFlag == TRUE  && VStr[j] != L'\0' ) || ( VStrRFlag == FALSE && VStr[j] == L'\0' ) )
						{
							StreamData->ReadShred.Seek( StreamData->DataPoint, -( int )sizeof( WORD ), SEEK_CUR );
							break;
						}

						if( Width - i == 1 )
						{
							StreamData->ReadShred.Seek( StreamData->DataPoint, -( int )sizeof( WORD ), SEEK_CUR );
							if( pStr )
							{
								pStr[0] = c2[0];
								pStr ++ ;
							}
							i ++ ;
						}
						else
						{
							if( pStr )
							{
								pStr[0] = c2[0];
								pStr[1] = c2[1];
								pStr += 2 ;
							}
							i += 2 ;
						}
					}
					else
					{
						j = 0;
						while( VStr[j] != L'\0' )
						{
							if( CHECK_CPU_LE_UTF16LE_4BYTE( VStr[j] ) == TRUE )
							{
								j += 2;
							}
							else
							{
								if( VStr[j] == c ) break;
								j ++ ;
							}
						}

						if( ( VStrRFlag == TRUE  && VStr[j] != L'\0' ) || ( VStrRFlag == FALSE && VStr[j] == L'\0' ) ) break;
						if( pStr )
						{
							*pStr = c;
							pStr ++ ;
						}
						i ++ ;
					}
					if( Width != 0 && Width == i ) break;
				}
				if( pStr ) *pStr = L'\0';
				if( Eof == FALSE && Width != i )
				{
					StreamData->ReadShred.Seek( StreamData->DataPoint, -( int )sizeof( WORD ), SEEK_CUR );
				}
			}
			else if( *FormatUTF16LE == L'd' || *FormatUTF16LE == L'D' || *FormatUTF16LE == L'u' || *FormatUTF16LE == L'U' )
			{
				FileRead_SkipSpaceUTF16LE( StreamData, &Eof );
				if( Eof == TRUE ) break;

				FormatUTF16LE ++ ;
				UIntFlag = *FormatUTF16LE == L'u' || *FormatUTF16LE == L'U';
				MinusFlag = FALSE;
				res = StreamData->ReadShred.Read( &c, sizeof( WORD ), 1, StreamData->DataPoint );
				STREAM_FSYNC( StreamData )
				if( res == 0 )
				{
					Eof = TRUE;
					break;
				}
				if( c == L'-' )
				{
					MinusFlag = TRUE;
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 ) break;
					}
					res = StreamData->ReadShred.Read( &c, sizeof( WORD ), 1, StreamData->DataPoint );
					STREAM_FSYNC( StreamData )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}
STR_10INT:
				for( i = 0; i < 127 && ( Width == -1 || Width != i ) && c >= L'0' && c <= L'9'; i ++ )
				{
					Number[i] = ( WORD )( c - L'0' );
					res = StreamData->ReadShred.Read( &c, sizeof( WORD ), 1, StreamData->DataPoint );
					STREAM_FSYNC( StreamData )
					if( res == 0 )
					{
						i ++ ;
						Eof = TRUE;
						break;
					}
				}
				if( Eof == FALSE )
				{
					StreamData->ReadShred.Seek( StreamData->DataPoint, -( int )sizeof( WORD ), SEEK_CUR );
				}
				num = i;
				if( num == 0 ) break;
				int64Count = 1;
				int64Num = 0;
				for( i = num - 1; i >= 0; i --, int64Count *= 10 )
				{
					if( UIntFlag )
						int64Num = (int)( (ULONGLONG)int64Num + (ULONGLONG)Number[i] * (ULONGLONG)int64Count );
					else 
						int64Num += (LONGLONG)Number[i] * int64Count;
				}
				if( MinusFlag ) int64Num = -int64Num;
				if( SkipFlag == FALSE )
				{
					if( I64Flag )
					{
						if( UIntFlag ) *va_arg( Param,      ULONGLONG * ) =      (ULONGLONG)int64Num;
						else           *va_arg( Param,       LONGLONG * ) =       (LONGLONG)int64Num;
					}
					else if( hFlag )
					{
						if( UIntFlag ) *va_arg( Param, unsigned short * ) = (unsigned short)int64Num;
						else           *va_arg( Param,          short * ) =          (short)int64Num;
					}
					else
					{
						if( UIntFlag ) *va_arg( Param,   unsigned int * ) =   (unsigned int)int64Num;
						else           *va_arg( Param,            int * ) =            (int)int64Num;
					}
				}
			}
			else if( *FormatUTF16LE == L'x' || *FormatUTF16LE == L'X' )
			{
				FileRead_SkipSpaceUTF16LE( StreamData, &Eof );
				if( Eof == TRUE ) break;

				FormatUTF16LE ++ ;
				MinusFlag = FALSE;
				res = StreamData->ReadShred.Read( &c, sizeof( WORD ), 1, StreamData->DataPoint );
				STREAM_FSYNC( StreamData )
				if( res == 0 )
				{
					Eof = TRUE;
					break;
				}
				if( c == L'-' )
				{
					MinusFlag = TRUE;
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 ) break;
					}
					res = StreamData->ReadShred.Read( &c, sizeof( WORD ), 1, StreamData->DataPoint );
					STREAM_FSYNC( StreamData )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}
STR_16INT:
				i = 0;
				for(;;)
				{
					if( i >= 127 ) break;
					if( Width != -1 && Width == i ) break;
					else if( c >= L'0' && c <= L'9' ) Number[i] = ( WORD )( c - L'0' );
					else if( c >= L'a' && c <= L'f' ) Number[i] = ( WORD )( c - L'a' + 10 );
					else if( c >= L'A' && c <= L'F' ) Number[i] = ( WORD )( c - L'A' + 10 );
					else break;
					i ++ ;
					res = StreamData->ReadShred.Read( &c, sizeof( WORD ), 1, StreamData->DataPoint );
					STREAM_FSYNC( StreamData )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}
				if( Eof == FALSE )
				{
					StreamData->ReadShred.Seek( StreamData->DataPoint, -( int )sizeof( WORD ), SEEK_CUR );
				}
				num = i;
				if( num == 0 ) break;
				int64Count = 1;
				int64Num = 0;
				for( i = num - 1; i >= 0; i --, int64Count *= 16 )
				{
					int64Num += Number[i] * int64Count;
				}
				if( MinusFlag ) int64Num = -int64Num;
				if( SkipFlag == FALSE )
				{
					if( I64Flag )    *va_arg( Param, LONGLONG * ) = (LONGLONG)int64Num;
					else if( hFlag ) *va_arg( Param, short *    ) =    (short)int64Num;
					else             *va_arg( Param, int *      ) =      (int)int64Num;
				}
			}
			else if( *FormatUTF16LE == L'o' || *FormatUTF16LE == L'O' )
			{
				FileRead_SkipSpaceUTF16LE( StreamData, &Eof );
				if( Eof == TRUE ) break;

				FormatUTF16LE ++ ;
				MinusFlag = FALSE;
				res = StreamData->ReadShred.Read( &c, sizeof( WORD ), 1, StreamData->DataPoint );
				STREAM_FSYNC( StreamData )
				if( res == 0 )
				{
					Eof = TRUE;
					break;
				}
				if( c == L'-' )
				{
					MinusFlag = TRUE;
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 ) break;
					}
					res = StreamData->ReadShred.Read( &c, sizeof( WORD ), 1, StreamData->DataPoint );
					STREAM_FSYNC( StreamData )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}
STR_8INT:
				for( i = 0; i < 127 && ( Width == -1 || Width != i ) && c >= L'0' && c <= L'7'; i ++ )
				{
					Number[i] = ( WORD )( c - L'0' );
					res = StreamData->ReadShred.Read( &c, sizeof( WORD ), 1, StreamData->DataPoint );
					STREAM_FSYNC( StreamData )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}
				if( Eof == FALSE )
				{
					StreamData->ReadShred.Seek( StreamData->DataPoint, -( int )sizeof( WORD ), SEEK_CUR );
				}
				num = i;
				if( num == 0 ) break;
				int64Count = 1;
				int64Num = 0;
				for( i = num - 1; i >= 0; i --, int64Count *= 8 )
				{
					int64Num += Number[i] * int64Count;
				}
				if( MinusFlag ) int64Num = -int64Num;
				if( SkipFlag == FALSE )
				{
					if( I64Flag )    *va_arg( Param, LONGLONG * ) = (LONGLONG)int64Num;
					else if( hFlag ) *va_arg( Param, short *    ) =    (short)int64Num;
					else             *va_arg( Param, int *      ) =      (int)int64Num;
				}
			}
			else if( *FormatUTF16LE == L'i' || *FormatUTF16LE == L'I' )
			{
				FileRead_SkipSpaceUTF16LE( StreamData, &Eof );
				if( Eof == TRUE ) break;

				FormatUTF16LE ++ ;
				MinusFlag = FALSE;
				res = StreamData->ReadShred.Read( &c, sizeof( WORD ), 1, StreamData->DataPoint );
				STREAM_FSYNC( StreamData )
				if( res == 0 )
				{
					Eof = TRUE;
					break;
				}
				if( c == L'-' )
				{
					MinusFlag = TRUE;
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 ) break;
					}
					res = StreamData->ReadShred.Read( &c, sizeof( WORD ), 1, StreamData->DataPoint );
					STREAM_FSYNC( StreamData )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}
				if( c == L'0' )
				{
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 ) break;
					}
					res = StreamData->ReadShred.Read( &c, sizeof( WORD ), 1, StreamData->DataPoint );
					STREAM_FSYNC( StreamData )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
					if( c == L'x' )
					{
						if( Width != -1 )
						{
							Width -- ;
							if( Width == 0 ) break;
						}
						res = StreamData->ReadShred.Read( &c, sizeof( WORD ), 1, StreamData->DataPoint );
						STREAM_FSYNC( StreamData )
						if( res == 0 )
						{
							Eof = TRUE;
							break;
						}
						goto STR_16INT;
					}
					else
					{
						goto STR_8INT;
					}
				}
				else
				{
					goto STR_10INT;
				}
			}
			else if( *FormatUTF16LE == L'c' )
			{
				FormatUTF16LE ++ ;
				if( Width == -1 ) Width = 1;
				pStr = NULL;
				pStrA = NULL;
				if( SkipFlag == FALSE )
				{
					if( FormatUTF16LE[ -1 ] == 'c' )
					{
						pStr = va_arg( Param, WORD * );
					}
					else
					{
						pStrA = va_arg( Param, char * );
					}
				}
				for( i = 0; i < Width; i ++ )
				{
					res = StreamData->ReadShred.Read( &c, sizeof( WORD ), 1, StreamData->DataPoint );
					STREAM_FSYNC( StreamData )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
					tstr[ i ] = c ;
				}
				tstr[ i ] = L'\0' ;
				if( pStr )
				{
					FileRead_strcpy_UTF16LE( pStr, tstr ) ;
				}
				if( pStrA )
				{
					ConvString( ( const char * )tstr, -1, DX_CHARCODEFORMAT_UTF16LE, pStrA, BUFFERBYTES_CANCEL, DX_CHARCODEFORMAT_SHIFTJIS ) ;
				}
			}
			else if( *FormatUTF16LE == L's' || *FormatUTF16LE == L'S' )
			{
				FormatUTF16LE ++ ;
				FileRead_SkipSpaceUTF16LE( StreamData, &Eof );
				if( Eof == TRUE ) break;

				pStr = NULL;
				pStrA = NULL;
				if( SkipFlag == FALSE )
				{
					if( FormatUTF16LE[ -1 ] == 's' )
					{
						pStr = va_arg( Param, WORD * );
					}
					else
					{
						pStrA = va_arg( Param, char * );
					}
				}

				i = 0;
				for(;;)
				{
					res = StreamData->ReadShred.Read( &c, sizeof( WORD ), 1, StreamData->DataPoint );
					STREAM_FSYNC( StreamData )
					if( res == 0 )
					{
						Eof = TRUE; 
						break;
					}
					if( CHECK_CPU_LE_UTF16LE_4BYTE( c ) == FALSE && ( c == L' ' || c == L'\n' || c == L'\r' ) )
					{
						StreamData->ReadShred.Seek( StreamData->DataPoint, -( int )sizeof( WORD ), SEEK_CUR );
						break;
					}
					tstr[ i ] = c ;
					i ++ ;
					if( Width != -1 && i >= Width ) break;

					if( CHECK_CPU_LE_UTF16LE_4BYTE( c ) == TRUE )
					{
						res = StreamData->ReadShred.Read( &c, sizeof( WORD ), 1, StreamData->DataPoint );
						STREAM_FSYNC( StreamData )
						if( res == 0 )
						{
							Eof = TRUE; 
							break;
						}
						tstr[ i ] = c ;
						i ++ ;
						if( Width != -1 && i >= Width ) break;
					}
				}
				tstr[ i ] = '\0' ;
				if( pStr )
				{
					FileRead_strcpy_UTF16LE( pStr, tstr ) ;
				}
				if( pStrA )
				{
					ConvString( ( const char * )tstr, -1, DX_CHARCODEFORMAT_UTF16LE, pStrA, BUFFERBYTES_CANCEL, DX_CHARCODEFORMAT_SHIFTJIS ) ;
				}
			}
			else if( *FormatUTF16LE == L'f' || *FormatUTF16LE == L'F' || *FormatUTF16LE == L'g' || *FormatUTF16LE == L'G' )
			{
				FileRead_SkipSpaceUTF16LE( StreamData, &Eof );
				if( Eof == TRUE ) break;

				FormatUTF16LE ++ ;
				MinusFlag = FALSE;
				DotFlag = FALSE;
				IndexFlag = FALSE;
				MinusFlag2 = FALSE;
				res = StreamData->ReadShred.Read( &c, sizeof( WORD ), 1, StreamData->DataPoint );
				STREAM_FSYNC( StreamData )
				if( res == 0 )
				{
					Eof = TRUE;
					break;
				}
				if( c == L'-' )
				{
					MinusFlag = TRUE;
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 ) break;
					}
					res = StreamData->ReadShred.Read( &c, sizeof( WORD ), 1, StreamData->DataPoint );
					STREAM_FSYNC( StreamData )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}

				i = 0;	//自然数
				j = 0;	//小数
				k = 0;	//指数
				for(;;)
				{
					if( c == L'.' )
					{
						if( DotFlag ) break;
						DotFlag = TRUE;
					}
					else if( c == L'e' || c == L'E' )
					{
						if( IndexFlag ) break;
						IndexFlag = TRUE;
					}
					else if( c == L'-' || c == L'+' )
					{
						if( IndexFlag == FALSE || k != 0 ) break;
						if( c == L'-' ) MinusFlag2 = TRUE;
					}
					else if( c >= L'0' && c <= L'9' )
					{
						if( IndexFlag )
						{
							if( k >= 127 ) break ;
							Number3[k] = ( WORD )( c - L'0' );
							k ++ ;
						}
						else if( DotFlag )
						{
							if( j >= 127 ) break ;
							Number2[j] = ( WORD )( c - L'0' );
							j ++ ;
						}
						else
						{
							if( i >= 127 ) break ;
							Number[i] = ( WORD )( c - L'0' );
							i ++ ;
						}
					}
					else break;

					res = StreamData->ReadShred.Read( &c, sizeof( WORD ), 1, StreamData->DataPoint );
					STREAM_FSYNC( StreamData )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 ) break;
					}
				}
				if( Eof == FALSE )
				{
					StreamData->ReadShred.Seek( StreamData->DataPoint, -( int )sizeof( WORD ), SEEK_CUR );
				}
				if( i == 0 && j == 0 ) break;
				num = i;
				num2 = j;
				num3 = k;
				if( num == 0 && num2 == 0 ) break;

				doubleCount = 1.0;
				doubleNum = 0;
				for( i = num - 1; i >= 0; i --, doubleCount *= 10.0 )
				{
					if( Number[i] != 0 )
						doubleNum += Number[i] * doubleCount;
				}
				if( MinusFlag ) doubleNum = -doubleNum;

				doubleCount = 0.1;
				doubleNum2 = 0;
				for( i = 0; i < num2; i ++, doubleCount /= 10.0 )
				{
					if( Number2[i] != 0 )
						doubleNum2 += Number2[i] * doubleCount;
				}
				if( MinusFlag ) doubleNum2 = -doubleNum2;

				int64Count = 1;
				int64Num = 0;
				for( i = num3 - 1; i >= 0; i --, int64Count *= 10 )
				{
					int64Num += Number3[i] * int64Count;
				}
				if( MinusFlag2 ) int64Num = -int64Num;

				doubleNum3 = 1.0;
				if( int64Num != 0 )
				{
					if( int64Num < 0 )
					{
						int64Num = -int64Num;
						for( i = 0; i < int64Num; i++ )
							doubleNum3 /= 10.0;
					}
					else
					{
						for( i = 0; i < int64Num; i++ )
							doubleNum3 *= 10.0;
					}
				}

				doubleNum = ( doubleNum + doubleNum2 ) * doubleNum3;

				if( SkipFlag == FALSE )
				{
					if( lFlag ) *va_arg( Param, double * ) = doubleNum;
					else        *va_arg( Param, float *  ) = (float)doubleNum;
				}
			}
			if( SkipFlag == FALSE ) ReadNum ++ ;
		}
		else
		{
			if( *FormatUTF16LE == L' ' || *FormatUTF16LE == L'\n' || *FormatUTF16LE == L'\r' )
			{
				while( *FormatUTF16LE != L'\0' && ( *FormatUTF16LE == L' ' || *FormatUTF16LE == L'\n' || *FormatUTF16LE == L'\r' ) ) FormatUTF16LE ++ ;
				FileRead_SkipSpaceUTF16LE( StreamData, &Eof );
				if( Eof == TRUE ) break;
			}
			else
			{
				for( i = 0; *FormatUTF16LE != L'\0' && *FormatUTF16LE != L' ' && *FormatUTF16LE != L'\n' && *FormatUTF16LE != L'\r' && *FormatUTF16LE != L'%'; i++, FormatUTF16LE ++ )
					str[i] = *FormatUTF16LE;
				str[i] = L'\0';
				num = i;
				for( i = 0; i < num; i ++ )
				{
					res = StreamData->ReadShred.Read( &c, sizeof( WORD ), 1, StreamData->DataPoint );
					STREAM_FSYNC( StreamData )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
					if( str[i] != c ) break;
				}
			}
		}
	}

	if( StreamData->ReadShred.Eof( StreamData->DataPoint ) && ReadNum == 0 ) return EOF;

	return ReadNum;
}

































#if 0

static void FileRead_SkipSpaceW( DWORD_PTR FileHandle, int *Eof )
{
	wchar_t c;
	size_t res;

	if( Eof ) *Eof = FALSE;
	for(;;)
	{
		res = DX_FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
		DX_FSYNC( FileHandle )
		if( res == 0 )
		{
			if( Eof ) *Eof = TRUE;
			return;
		}
		if( c != L' ' && c != L'\n' && c != L'\r' ) break;
	}
	DX_FSEEK( FileHandle, -( int )sizeof( wchar_t ), SEEK_CUR );
}

// サロゲートペアかどうかの判定( TRUE:４バイト文字上位  FALSE:２バイト文字 )
static int _CheckUTF16H( wchar_t c )
{
	return ( DWORD )c >= 0xd800 && ( DWORD )c <= 0xdbff ? TRUE : FALSE ;
}

static void __WCSCPY( wchar_t *Dest, const wchar_t *Src )
{
	int i ;
	for( i = 0 ; Src[i] != L'\0' ; i ++ ) Dest[i] = Src[i] ;
	Dest[i] = L'\0' ;
}

// ファイルから書式化されたデータを読み出す
extern int FileRead_scanf_baseW( DWORD_PTR FileHandle, const wchar_t *Format, va_list Param )
{
	wchar_t c, c2[2], tstr[512], str[256], Number[128], Number2[128], Number3[128], VStr[1024];
	int ReadNum, Width, i, j, k, num, num2, num3;
	int SkipFlag, VStrRFlag, I64Flag, lFlag, hFlag, Eof, MinusFlag, UIntFlag;
	int DotFlag, IndexFlag, MinusFlag2 ;
	size_t res;
	LONGLONG int64Num, int64Count;
	double doubleNum, doubleNum2, doubleNum3, doubleCount;
	wchar_t *pStr;
	char *pStrA ;

	if( DX_FEOF( FileHandle ) ) return EOF;

	if( DX_FTELL( FileHandle ) == 0 )
	{
		DX_FREAD( &c, sizeof( wchar_t ), 1, FileHandle ) ;
		DX_FSYNC( FileHandle )
		if( *( ( WORD * )&c ) != 0xfeff )
		{
			DX_FSEEK( FileHandle, 0, SEEK_SET ) ;
		}
	}

	ReadNum = 0;
	Eof = FALSE;
	while( *Format != L'\0' && DX_FEOF( FileHandle ) == 0 )
	{
		if( Format[0] == L'%' && Format[1] != L'%' )
		{
			Width = -1;
			I64Flag = FALSE;
			lFlag = FALSE;
			hFlag = FALSE;
			UIntFlag = FALSE;
			SkipFlag = FALSE;
			Format ++ ;
			if( *Format == L'\0' ) break;
			if( *Format == L'*' )
			{
				SkipFlag = TRUE;
				Format ++ ;
			}
			if( *Format == L'\0' ) break;
			if( *Format >= L'0' && *Format <= L'9' )
			{
				for( i = 0; Format[i] >= L'0' && Format[i] <= L'9'; i++ )
					str[i] = Format[i];
				str[i] = L'\0';
				Format += i;
				Width = _ATOIW( ( wchar_t * )str );
				if( Width == 0 ) break;
			}
			if( *Format == L'\0' ) break;
			switch( *Format )
			{
			case L'l': case L'H': lFlag = TRUE; Format++ ; break;
			case L'h': case L'L': hFlag = TRUE; Format++ ; break;
			case L'I':
				if( Format[1] == L'6' && Format[2] == L'4' )
				{
					I64Flag = TRUE;
					Format += 3;
				}
				break;
			}
			if( *Format == L'\0' ) break;

			if( *Format == L'[' )
			{
				if( lFlag || hFlag || I64Flag ) break;

				Format ++ ;
				VStrRFlag = FALSE;
				if( *Format == L'^' )
				{
					VStrRFlag = TRUE;
					Format++;
				}
				j = 0;
				_MEMSET( VStr, 0, sizeof( VStr ) );
				c = L'[';
				while( *Format != L'\0' && *Format != L']' )
				{
					if( _CheckUTF16H( *Format ) == TRUE )
					{
						if( Format[1] == L'\0' )
						{
							Format++;
							break;
						}
						if( Format[1] == L']' ) break;
						VStr[j]   = Format[0];
						VStr[j+1] = Format[1];
						j += 2 ;
						Format += 2 ;
						c = L'[';
					}
					else
					{
						if( *Format == L'-' && c != L'[' && Format[1] != L'\0' && Format[1] != L']' )
						{
							num  = (int)(unsigned short)c;
							num2 = (int)(unsigned short)Format[1];
							if( num2 < num )
							{
								k = num2; num2 = num; num = k;
							}
							for( k = num; k <= num2; k++ )
							{
								if( c != k )
								{
									*((unsigned short *)&VStr[j]) = (unsigned short)k;
									j++;
								}
							}
							Format += 2;
							c = L'[';
						}
						else
						{
							VStr[j] = *Format;
							c = *Format;
							j ++ ;
							Format ++ ;
						}
					}
				}
				if( *Format == L'\0' ) break;
				Format ++ ;
				pStr = NULL;
				if( SkipFlag == FALSE )
				{
					pStr = va_arg( Param, wchar_t * );
				}
				FileRead_SkipSpaceW( FileHandle, &Eof );
				if( Eof == TRUE ) break;
				i = 0;
				for(;;)
				{
					res = DX_FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
					DX_FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
					if( _CheckUTF16H( c ) == TRUE )
					{
						c2[0] = c;
						res = DX_FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
						DX_FSYNC( FileHandle )
						if( res == 0 )
						{
							Eof = TRUE;
							break;
						}
						c2[1] = c;

						j = 0;
						while( VStr[j] != L'\0' )
						{
							if( _CheckUTF16H( VStr[j] ) == TRUE )
							{
								if( VStr[j] == c2[0] && VStr[j+1] == c2[1] ) break;
								j += 2;
							}
							else
							{
								j ++ ;
							}
						}

						if( ( VStrRFlag == TRUE  && VStr[j] != L'\0' ) || ( VStrRFlag == FALSE && VStr[j] == L'\0' ) )
						{
							DX_FSEEK( FileHandle, -( int )sizeof( wchar_t ), SEEK_CUR );
							break;
						}

						if( Width - i == 1 )
						{
							DX_FSEEK( FileHandle, -( int )sizeof( wchar_t ), SEEK_CUR );
							if( pStr )
							{
								pStr[0] = c2[0];
								pStr ++ ;
							}
							i ++ ;
						}
						else
						{
							if( pStr )
							{
								pStr[0] = c2[0];
								pStr[1] = c2[1];
								pStr += 2 ;
							}
							i += 2 ;
						}
					}
					else
					{
						j = 0;
						while( VStr[j] != L'\0' )
						{
							if( _CheckUTF16H( VStr[j] ) == TRUE )
							{
								j += 2;
							}
							else
							{
								if( VStr[j] == c ) break;
								j ++ ;
							}
						}

						if( ( VStrRFlag == TRUE  && VStr[j] != L'\0' ) || ( VStrRFlag == FALSE && VStr[j] == L'\0' ) ) break;
						if( pStr )
						{
							*pStr = c;
							pStr ++ ;
						}
						i ++ ;
					}
					if( Width != 0 && Width == i ) break;
				}
				if( pStr ) *pStr = L'\0';
				if( Eof == FALSE && Width != i )
				{
					DX_FSEEK( FileHandle, -( int )sizeof( wchar_t ), SEEK_CUR );
				}
			}
			else if( *Format == L'd' || *Format == L'D' || *Format == L'u' || *Format == L'U' )
			{
				FileRead_SkipSpaceW( FileHandle, &Eof );
				if( Eof == TRUE ) break;

				Format ++ ;
				UIntFlag = *Format == L'u' || *Format == L'U';
				MinusFlag = FALSE;
				res = DX_FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
				DX_FSYNC( FileHandle )
				if( res == 0 )
				{
					Eof = TRUE;
					break;
				}
				if( c == L'-' )
				{
					MinusFlag = TRUE;
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 ) break;
					}
					res = DX_FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
					DX_FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}
STR_10INT:
				for( i = 0; i < 127 && ( Width == -1 || Width != i ) && c >= L'0' && c <= L'9'; i ++ )
				{
					Number[i] = ( WORD )( c - L'0' );
					res = DX_FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
					DX_FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}
				if( Eof == FALSE )
				{
					DX_FSEEK( FileHandle, -( int )sizeof( wchar_t ), SEEK_CUR );
				}
				num = i;
				if( num == 0 ) break;
				int64Count = 1;
				int64Num = 0;
				for( i = num - 1; i >= 0; i --, int64Count *= 10 )
				{
					if( UIntFlag )
						int64Num = (int)( (ULONGLONG)int64Num + (ULONGLONG)Number[i] * (ULONGLONG)int64Count );
					else 
						int64Num += (LONGLONG)Number[i] * int64Count;
				}
				if( MinusFlag ) int64Num = -int64Num;
				if( SkipFlag == FALSE )
				{
					if( I64Flag )
					{
						if( UIntFlag ) *va_arg( Param,      ULONGLONG * ) =      (ULONGLONG)int64Num;
						else           *va_arg( Param,       LONGLONG * ) =       (LONGLONG)int64Num;
					}
					else if( hFlag )
					{
						if( UIntFlag ) *va_arg( Param, unsigned short * ) = (unsigned short)int64Num;
						else           *va_arg( Param,          short * ) =          (short)int64Num;
					}
					else
					{
						if( UIntFlag ) *va_arg( Param,   unsigned int * ) =   (unsigned int)int64Num;
						else           *va_arg( Param,            int * ) =            (int)int64Num;
					}
				}
			}
			else if( *Format == L'x' || *Format == L'X' )
			{
				FileRead_SkipSpaceW( FileHandle, &Eof );
				if( Eof == TRUE ) break;

				Format ++ ;
				MinusFlag = FALSE;
				res = DX_FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
				DX_FSYNC( FileHandle )
				if( res == 0 )
				{
					Eof = TRUE;
					break;
				}
				if( c == L'-' )
				{
					MinusFlag = TRUE;
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 ) break;
					}
					res = DX_FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
					DX_FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}
STR_16INT:
				i = 0;
				for(;;)
				{
					if( i >= 127 ) break;
					if( Width != -1 && Width == i ) break;
					else if( c >= L'0' && c <= L'9' ) Number[i] = ( WORD )( c - L'0' );
					else if( c >= L'a' && c <= L'f' ) Number[i] = ( WORD )( c - L'a' + 10 );
					else if( c >= L'A' && c <= L'F' ) Number[i] = ( WORD )( c - L'A' + 10 );
					else break;
					i ++ ;
					res = DX_FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
					DX_FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}
				if( Eof == FALSE )
				{
					DX_FSEEK( FileHandle, -( int )sizeof( wchar_t ), SEEK_CUR );
				}
				num = i;
				if( num == 0 ) break;
				int64Count = 1;
				int64Num = 0;
				for( i = num - 1; i >= 0; i --, int64Count *= 16 )
				{
					int64Num += Number[i] * int64Count;
				}
				if( MinusFlag ) int64Num = -int64Num;
				if( SkipFlag == FALSE )
				{
					if( I64Flag )    *va_arg( Param, LONGLONG * ) = (LONGLONG)int64Num;
					else if( hFlag ) *va_arg( Param, short *    ) =    (short)int64Num;
					else             *va_arg( Param, int *      ) =      (int)int64Num;
				}
			}
			else if( *Format == L'o' || *Format == L'O' )
			{
				FileRead_SkipSpaceW( FileHandle, &Eof );
				if( Eof == TRUE ) break;

				Format ++ ;
				MinusFlag = FALSE;
				res = DX_FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
				DX_FSYNC( FileHandle )
				if( res == 0 )
				{
					Eof = TRUE;
					break;
				}
				if( c == L'-' )
				{
					MinusFlag = TRUE;
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 ) break;
					}
					res = DX_FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
					DX_FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}
STR_8INT:
				for( i = 0; i < 127 && ( Width == -1 || Width != i ) && c >= L'0' && c <= L'7'; i ++ )
				{
					Number[i] = ( WORD )( c - L'0' );
					res = DX_FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
					DX_FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}
				if( Eof == FALSE )
				{
					DX_FSEEK( FileHandle, -( int )sizeof( wchar_t ), SEEK_CUR );
				}
				num = i;
				if( num == 0 ) break;
				int64Count = 1;
				int64Num = 0;
				for( i = num - 1; i >= 0; i --, int64Count *= 8 )
				{
					int64Num += Number[i] * int64Count;
				}
				if( MinusFlag ) int64Num = -int64Num;
				if( SkipFlag == FALSE )
				{
					if( I64Flag )    *va_arg( Param, LONGLONG * ) = (LONGLONG)int64Num;
					else if( hFlag ) *va_arg( Param, short *    ) =    (short)int64Num;
					else             *va_arg( Param, int *      ) =      (int)int64Num;
				}
			}
			else if( *Format == L'i' || *Format == L'I' )
			{
				FileRead_SkipSpaceW( FileHandle, &Eof );
				if( Eof == TRUE ) break;

				Format ++ ;
				MinusFlag = FALSE;
				res = DX_FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
				DX_FSYNC( FileHandle )
				if( res == 0 )
				{
					Eof = TRUE;
					break;
				}
				if( c == L'-' )
				{
					MinusFlag = TRUE;
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 ) break;
					}
					res = DX_FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
					DX_FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}
				if( c == L'0' )
				{
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 ) break;
					}
					res = DX_FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
					DX_FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
					if( c == L'x' )
					{
						if( Width != -1 )
						{
							Width -- ;
							if( Width == 0 ) break;
						}
						res = DX_FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
						DX_FSYNC( FileHandle )
						if( res == 0 )
						{
							Eof = TRUE;
							break;
						}
						goto STR_16INT;
					}
					else
					{
						goto STR_8INT;
					}
				}
				else
				{
					goto STR_10INT;
				}
			}
			else if( *Format == L'c' )
			{
				Format ++ ;
				if( Width == -1 ) Width = 1;
				pStr = NULL;
				pStrA = NULL;
				if( SkipFlag == FALSE )
				{
					if( Format[ -1 ] == 'c' )
					{
						pStr = va_arg( Param, wchar_t * );
					}
					else
					{
						pStrA = va_arg( Param, char * );
					}
				}
				for( i = 0; i < Width; i ++ )
				{
					res = DX_FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
					DX_FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
					tstr[ i ] = c ;
				}
				tstr[ i ] = L'\0' ;
				if( pStr )
				{
					__WCSCPY( pStr, tstr ) ;
				}
				if( pStrA )
				{
					ConvString( ( const char * )tstr, WCHAR_T_CHARCODEFORMAT, pStrA, BUFFERBYTES_CANCEL, DX_CHARCODEFORMAT_SHIFTJIS ) ;
				}
			}
			else if( *Format == L's' || *Format == L'S' )
			{
				Format ++ ;
				FileRead_SkipSpaceW( FileHandle, &Eof );
				if( Eof == TRUE ) break;

				pStr = NULL;
				pStrA = NULL;
				if( SkipFlag == FALSE )
				{
					if( Format[ -1 ] == 's' )
					{
						pStr = va_arg( Param, wchar_t * );
					}
					else
					{
						pStrA = va_arg( Param, char * );
					}
				}

				i = 0;
				for(;;)
				{
					res = DX_FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
					DX_FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE; 
						break;
					}
					if( _CheckUTF16H( c ) == FALSE && ( c == L' ' || c == L'\n' || c == L'\r' ) )
					{
						DX_FSEEK( FileHandle, -( int )sizeof( wchar_t ), SEEK_CUR );
						break;
					}
					tstr[ i ] = c ;
					i ++ ;
					if( Width != -1 && i >= Width ) break;

					if( _CheckUTF16H( c ) == TRUE )
					{
						res = DX_FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
						DX_FSYNC( FileHandle )
						if( res == 0 )
						{
							Eof = TRUE; 
							break;
						}
						tstr[ i ] = c ;
						i ++ ;
						if( Width != -1 && i >= Width ) break;
					}
				}
				tstr[ i ] = '\0' ;
				if( pStr )
				{
					__WCSCPY( pStr, tstr ) ;
				}
				if( pStrA )
				{
					ConvString( ( const char * )tstr, WCHAR_T_CHARCODEFORMAT, pStrA, BUFFERBYTES_CANCEL, DX_CHARCODEFORMAT_SHIFTJIS ) ;
				}
			}
			else if( *Format == L'f' || *Format == L'F' || *Format == L'g' || *Format == L'G' )
			{
				FileRead_SkipSpaceW( FileHandle, &Eof );
				if( Eof == TRUE ) break;

				Format ++ ;
				MinusFlag = FALSE;
				DotFlag = FALSE;
				IndexFlag = FALSE;
				MinusFlag2 = FALSE;
				res = DX_FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
				DX_FSYNC( FileHandle )
				if( res == 0 )
				{
					Eof = TRUE;
					break;
				}
				if( c == L'-' )
				{
					MinusFlag = TRUE;
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 ) break;
					}
					res = DX_FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
					DX_FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}

				i = 0;	//自然数
				j = 0;	//小数
				k = 0;	//指数
				for(;;)
				{
					if( c == L'.' )
					{
						if( DotFlag ) break;
						DotFlag = TRUE;
					}
					else if( c == L'e' || c == L'E' )
					{
						if( IndexFlag ) break;
						IndexFlag = TRUE;
					}
					else if( c == L'-' || c == L'+' )
					{
						if( IndexFlag == FALSE || k != 0 ) break;
						if( c == L'-' ) MinusFlag2 = TRUE;
					}
					else if( c >= L'0' && c <= L'9' )
					{
						if( IndexFlag )
						{
							if( k >= 127 ) break ;
							Number3[k] = ( WORD )( c - L'0' );
							k ++ ;
						}
						else if( DotFlag )
						{
							if( j >= 127 ) break ;
							Number2[j] = ( WORD )( c - L'0' );
							j ++ ;
						}
						else
						{
							if( i >= 127 ) break ;
							Number[i] = ( WORD )( c - L'0' );
							i ++ ;
						}
					}
					else break;

					res = DX_FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
					DX_FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 ) break;
					}
				}
				if( Eof == FALSE )
				{
					DX_FSEEK( FileHandle, -( int )sizeof( wchar_t ), SEEK_CUR );
				}
				if( i == 0 && j == 0 ) break;
				num = i;
				num2 = j;
				num3 = k;
				if( num == 0 && num2 == 0 ) break;

				doubleCount = 1.0;
				doubleNum = 0;
				for( i = num - 1; i >= 0; i --, doubleCount *= 10.0 )
				{
					if( Number[i] != 0 )
						doubleNum += Number[i] * doubleCount;
				}
				if( MinusFlag ) doubleNum = -doubleNum;

				doubleCount = 0.1;
				doubleNum2 = 0;
				for( i = 0; i < num2; i ++, doubleCount /= 10.0 )
				{
					if( Number2[i] != 0 )
						doubleNum2 += Number2[i] * doubleCount;
				}
				if( MinusFlag ) doubleNum2 = -doubleNum2;

				int64Count = 1;
				int64Num = 0;
				for( i = num3 - 1; i >= 0; i --, int64Count *= 10 )
				{
					int64Num += Number3[i] * int64Count;
				}
				if( MinusFlag2 ) int64Num = -int64Num;

				doubleNum3 = 1.0;
				if( int64Num != 0 )
				{
					if( int64Num < 0 )
					{
						int64Num = -int64Num;
						for( i = 0; i < int64Num; i++ )
							doubleNum3 /= 10.0;
					}
					else
					{
						for( i = 0; i < int64Num; i++ )
							doubleNum3 *= 10.0;
					}
				}

				doubleNum = ( doubleNum + doubleNum2 ) * doubleNum3;

				if( SkipFlag == FALSE )
				{
					if( lFlag ) *va_arg( Param, double * ) = doubleNum;
					else        *va_arg( Param, float *  ) = (float)doubleNum;
				}
			}
			if( SkipFlag == FALSE ) ReadNum ++ ;
		}
		else
		{
			if( *Format == L' ' || *Format == L'\n' || *Format == L'\r' )
			{
				while( *Format != L'\0' && ( *Format == L' ' || *Format == L'\n' || *Format == L'\r' ) ) Format ++ ;
				FileRead_SkipSpaceW( FileHandle, &Eof );
				if( Eof == TRUE ) break;
			}
			else
			{
				for( i = 0; *Format != L'\0' && *Format != L' ' && *Format != L'\n' && *Format != L'\r' && *Format != L'%'; i++, Format ++ )
					str[i] = *Format;
				str[i] = L'\0';
				num = i;
				for( i = 0; i < num; i ++ )
				{
					res = DX_FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
					DX_FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
					if( str[i] != c ) break;
				}
			}
		}
	}

	if( DX_FEOF( FileHandle ) && ReadNum == 0 ) return EOF;

	return ReadNum;
}

#endif


























// メモリに置かれたデータをファイルのデータに例えてつかうための関数
extern DWORD_PTR MemStreamOpen( const void *DataBuffer, size_t DataSize )
{
	MEMSTREAMDATA *NewMemStream ;

	// メモリの確保
	if( ( NewMemStream = ( MEMSTREAMDATA * )DXALLOC( sizeof( MEMSTREAMDATA ) ) ) == NULL ) return NULL ;
	_MEMSET( NewMemStream, 0, sizeof( MEMSTREAMDATA) ) ;

	// データのセット
	NewMemStream->DataBuffer = ( unsigned char * )DataBuffer ;
	NewMemStream->DataPoint = 0 ;
	NewMemStream->DataSize = DataSize ;
	NewMemStream->EOFFlag = FALSE ;

	// 終了
	return ( DWORD_PTR )NewMemStream ;
}


extern int MemStreamClose( DWORD_PTR StreamDataPoint )
{
	// メモリの解放
	DXFREE( ( void * )StreamDataPoint ) ;

	// 終了
	return 0 ;
}

static LONGLONG MemStreamTell( DWORD_PTR StreamDataPoint)
{
	// データポインタを返す
	return ( LONGLONG )( ( MEMSTREAMDATA * )StreamDataPoint )->DataPoint ;
}

static int MemStreamSeek( DWORD_PTR StreamDataPoint, LONGLONG SeekPoint, int SeekType )
{
	MEMSTREAMDATA *MemStream = ( MEMSTREAMDATA * )StreamDataPoint ;
	LONGLONG NewPoint = 0 ;

	// シークタイプによって処理を分岐
	switch( SeekType )
	{
	case STREAM_SEEKTYPE_SET :
		NewPoint = SeekPoint ;
		break ;

	case STREAM_SEEKTYPE_END :
		NewPoint = ( LONGLONG )( SeekPoint + MemStream->DataSize ) ;
		break ;

	case STREAM_SEEKTYPE_CUR :
		NewPoint = ( LONGLONG )( SeekPoint + MemStream->DataPoint ) ;
		break ;
	}

	// 補正
	if( NewPoint > ( LONGLONG )MemStream->DataSize )
	{
		MemStream->DataPoint = MemStream->DataSize ;
	}
	else
	{
		if( NewPoint < 0 )
		{
			MemStream->DataPoint = 0 ;
		}
		else
		{
			MemStream->DataPoint = ( size_t )NewPoint ;
		}
	}

	MemStream->EOFFlag = FALSE ;

	// 終了
	return 0 ;
} ;

static size_t MemStreamRead( void *Buffer, size_t BlockSize, size_t DataNum, DWORD_PTR StreamDataPoint)
{
	MEMSTREAMDATA *MemStream = ( MEMSTREAMDATA * )StreamDataPoint ;

	// EOF の検出
	if( MemStream->DataPoint == MemStream->DataSize )
	{
		MemStream->EOFFlag = TRUE ;
		return 0 ;
	}
	else
	{
		// 読みこむデータの数の補正
		if( BlockSize * DataNum > MemStream->DataSize - MemStream->DataPoint )
		{
			DataNum = ( MemStream->DataSize - MemStream->DataPoint ) / BlockSize ;
		}

		if( DataNum != 0 )
		{
			_MEMCPY( Buffer, MemStream->DataBuffer + MemStream->DataPoint, DataNum * BlockSize ) ;
			MemStream->DataPoint += BlockSize * DataNum ;
		}
		else
		{
			size_t MoveSize ;

			MoveSize = MemStream->DataSize - MemStream->DataPoint ;
			if( MoveSize != 0 )
			{
				_MEMCPY( Buffer, MemStream->DataBuffer + MemStream->DataPoint, MoveSize ) ;
				MemStream->DataPoint += MoveSize ;
				DataNum = 1 ;
			}
		}

		MemStream->EOFFlag = FALSE ;
	}

	return DataNum ;
}
	
static size_t MemStreamWrite( void *Buffer, size_t BlockSize, size_t DataNum, DWORD_PTR StreamDataPoint)
{
	MEMSTREAMDATA *MemStream = ( MEMSTREAMDATA * )StreamDataPoint ;

	// EOF の検出
	if( MemStream->DataPoint == MemStream->DataSize )
	{
		MemStream->EOFFlag = TRUE ;
		return 0 ;
	}
	else
	{
		// 書きこむデータの数の補正
		if( BlockSize * DataNum > MemStream->DataSize - MemStream->DataPoint )
		{
			DataNum = ( ( MemStream->DataSize - MemStream->DataPoint ) / BlockSize ) * BlockSize ;
		}

		if( DataNum != 0 )
		{
			_MEMCPY( MemStream->DataBuffer + MemStream->DataPoint, Buffer, DataNum * BlockSize ) ;
			MemStream->DataPoint += BlockSize * DataNum ;
		}

		MemStream->EOFFlag = FALSE ;
	}

	return DataNum ;
}

static int MemStreamEof( DWORD_PTR StreamDataPoint)
{
	MEMSTREAMDATA *MemStream = ( MEMSTREAMDATA * )StreamDataPoint ;

	// EOF を返す
	return MemStream->EOFFlag ;
}

static int MemStreamIdleCheck( DWORD_PTR /*StreamDataPoint*/ )
{
	// 常にアイドリング状態
	return TRUE ;
}

















// TCHAR版ストリームデータアクセスエミュレート用関数
static DWORD_PTR StreamTCHAR_Open( const wchar_t *Path, int UseCacheFlag, int BlockReadFlag, int UseASyncReadFlag )
{
#ifdef UNICODE
	return StreamFunction.Open( Path, UseCacheFlag, BlockReadFlag, UseASyncReadFlag ) ;
#else
	TCHAR PathBuffer[ FILEPATH_MAX ] ;
	ConvString( ( const char * )Path, -1, WCHAR_T_CHARCODEFORMAT, ( char * )PathBuffer, sizeof( PathBuffer ), _TCHARCODEFORMAT ) ;
	return StreamFunction.Open( PathBuffer, UseCacheFlag, BlockReadFlag, UseASyncReadFlag ) ;
#endif
}

static int StreamTCHAR_Close( DWORD_PTR Handle )
{
	return StreamFunction.Close( Handle ) ;
}

static LONGLONG StreamTCHAR_Tell( DWORD_PTR Handle )
{
	return StreamFunction.Tell( Handle ) ;
}

static int StreamTCHAR_Seek( DWORD_PTR Handle, LONGLONG SeekPoint, int SeekType )
{
	return StreamFunction.Seek( Handle, SeekPoint, SeekType ) ;
}

static size_t StreamTCHAR_Read( void *Buffer, size_t BlockSize, size_t DataNum, DWORD_PTR Handle )
{
	return StreamFunction.Read( Buffer, BlockSize, DataNum, Handle ) ;
}

static int StreamTCHAR_Eof( DWORD_PTR Handle )
{
	return StreamFunction.Eof( Handle ) ;
}

static int StreamTCHAR_IdleCheck( DWORD_PTR Handle )
{
	return StreamFunction.IdleCheck( Handle ) ;
}

static int StreamTCHAR_ChDir( const wchar_t *Path )
{
#ifdef UNICODE
	return StreamFunction.ChDir( Path ) ;
#else
	TCHAR PathBuffer[ FILEPATH_MAX ] ;
	ConvString( ( const char * )Path, -1, WCHAR_T_CHARCODEFORMAT, ( char * )PathBuffer, sizeof( PathBuffer ), _TCHARCODEFORMAT ) ;
	return StreamFunction.ChDir( PathBuffer ) ;
#endif
}

static int StreamTCHAR_GetDir( wchar_t *Buffer )
{
#ifdef UNICODE
	return StreamFunction.GetDirS != NULL ?
				StreamFunction.GetDirS( Buffer, BUFFERBYTES_CANCEL ) :
				StreamFunction.GetDir( Buffer ) ;
#else
	TCHAR PathBuffer[ FILEPATH_MAX ] ;
	int Result ;

	Result = StreamFunction.GetDirS != NULL ?
				StreamFunction.GetDirS( PathBuffer, BUFFERBYTES_CANCEL ) :
				StreamFunction.GetDir( PathBuffer ) ;
	ConvString( ( const char * )PathBuffer, -1, _TCHARCODEFORMAT, ( char * )Buffer, BUFFERBYTES_CANCEL, WCHAR_T_CHARCODEFORMAT ) ;
	return Result ;
#endif
}

static int StreamTCHAR_GetDirS( wchar_t *Buffer, size_t BufferBytes )
{
#ifdef UNICODE
	return StreamFunction.GetDirS != NULL ?
				StreamFunction.GetDirS( Buffer, BufferBytes ) :
				StreamFunction.GetDir( Buffer ) ;
#else
	TCHAR PathBuffer[ FILEPATH_MAX ] ;
	int Result ;

	Result = StreamFunction.GetDirS != NULL ?
				StreamFunction.GetDirS( PathBuffer, BufferBytes ) :
				StreamFunction.GetDir( PathBuffer ) ;
	ConvString( ( const char * )PathBuffer, -1, _TCHARCODEFORMAT, ( char * )Buffer, BufferBytes, WCHAR_T_CHARCODEFORMAT ) ;
	return Result ;
#endif
}

static DWORD_PTR StreamTCHAR_FindFirst( const wchar_t *FilePath, FILEINFOW *Buffer )
{
#ifdef UNICODE
	return StreamFunction.FindFirst( FilePath, ( FILEINFO * )Buffer ) ;
#else
	TCHAR PathBuffer[ FILEPATH_MAX ] ;
	FILEINFO BufferT ;
	DWORD_PTR Result ;

	ConvString( ( const char * )FilePath, -1, WCHAR_T_CHARCODEFORMAT, ( char * )PathBuffer, sizeof( PathBuffer ), _TCHARCODEFORMAT ) ;

	Result = StreamFunction.FindFirst( PathBuffer, &BufferT ) ;
	if( Buffer != NULL )
	{
		ConvFileIntoToFileInfoW( &BufferT, Buffer ) ;
	}

	return Result ;
#endif
}

static int StreamTCHAR_FindNext( DWORD_PTR FindHandle, FILEINFOW *Buffer )
{
#ifdef UNICODE
	return StreamFunction.FindNext( FindHandle, ( FILEINFO * )Buffer ) ;
#else
	FILEINFO BufferT ;
	int Result ;

	Result = StreamFunction.FindNext( FindHandle, &BufferT ) ;
	if( Buffer != NULL )
	{
		ConvFileIntoToFileInfoW( &BufferT, Buffer ) ;
	}

	return Result ;
#endif
}

static int StreamTCHAR_FindClose( DWORD_PTR FindHandle )
{
	return StreamFunction.FindClose( FindHandle ) ;
}























// フルパスではないパス文字列をフルパスに変換する
// ( CurrentDir はフルパスである必要がある(語尾に『\』があっても無くても良い) )
// ( CurrentDir が NULL の場合は現在のカレントディレクトリを使用する )
//extern	int ConvertFullPath_( const char *Src, char *Dest, const char *CurrentDir )
//{
//	int i, j, k ;
//	char iden[FILEPATH_MAX], cur[FILEPATH_MAX] ;
//
//	if( CurrentDir == NULL )
//	{
//		wchar_t curW[ FILEPATH_MAX ];
//
//		DX_FGETDIR( curW ) ;
//		ConvString( ( const char * )curW, WCHAR_T_CHARCODEFORMAT, cur, sizeof( cur ), _TCHARCODEFORMAT ) ;
//
//		CurrentDir = cur ;
//	}
//
//	if( Src == NULL )
//	{
//		_STRCPY( Dest, CurrentDir ) ;
//		goto END ;
//	}
//
//	i = 0 ;
//	j = 0 ;
//	k = 0 ;
//	
//	// 最初に『\』又は『/』が２回連続で続いている場合はネットワークを介していると判断
//	if( ( Src[0] == '\\' && Src[1] == '\\' ) ||
//		( Src[0] == '/'  && Src[1] == '/'  ) )
//	{
//		Dest[0] = '\\';
//		Dest[1] = '\0';
//
//		i += 2;
//		j ++ ;
//	}
//	else
//	// 最初が『\』又は『/』の場合はカレントドライブのルートディレクトリまで落ちる
//	if( Src[0] == '\\' || Src[0] == '/' )
//	{
//		Dest[0] = CurrentDir[0] ;
//		Dest[1] = CurrentDir[1] ;
//		Dest[2] = '\0' ;
//
//		i ++ ;
//		j = 2 ;
//	}
//	else
//	// ドライブ名が書かれていたらそのドライブへ
//	if( Src[1] == ':' )
//	{
//		Dest[0] = Src[0] ;
//		Dest[1] = Src[1] ;
//		Dest[2] = '\0' ;
//
//		i = 2 ;
//		j = 2 ;
//
//		if( Src[i] == '\\' || Src[i] == '/' ) i ++ ;
//	}
//	else
//	// それ以外の場合はカレントディレクトリ
//	{
//		_STRCPY( Dest, CurrentDir ) ;
//		j = _STRLEN( Dest ) ;
//		if( Dest[j-1] == '\\' || Dest[j-1] == '/' )
//		{
//			Dest[j-1] = '\0' ;
//			j -- ;
//		}
//	}
//
//	for(;;)
//	{
//		switch( Src[i] )
//		{
//		case '\0' :
//			if( k != 0 )
//			{
//				Dest[j] = '\\' ; j ++ ;
//				_STRCPY( &Dest[j], iden ) ;
//				j += k ;
//				k = 0 ;
//			}
//			goto END ;
//
//		case '\\' :
//		case '/' :
//			// 文字列が無かったらスキップ
//			if( k == 0 )
//			{
//				i ++ ;
//				break ;
//			}
//			if( _STRCMP( iden, "." ) == 0 )
//			{
//				// なにもしない
//			}
//			else
//			if( _STRCMP( iden, ".." ) == 0 )
//			{
//				// 一つ下のディレクトリへ
//				j -- ;
//				for(;;)
//				{
//					if( Dest[j] == '\\' || Dest[j] == '/' || Dest[j] == ':' ) break ;
//					j -= CheckMultiByteString( Dest, j - 1, _GET_CHARSET() ) == 2 ? 2 : 1 ;
//				}
////				while( Dest[j] != '\\' && Dest[j] != '/' && Dest[j] != ':' ) j -- ;
//				if( Dest[j] != ':' ) Dest[j] = '\0' ;
//				else j ++ ;
//			}
//			else
//			{
//				Dest[j] = '\\' ; j ++ ;
//				_STRCPY( &Dest[j], iden ) ;
//				j += k ;
//			}
//
//			k = 0 ;
//			i ++ ;
//			break ;
//		
//		default :
//			if( CheckMultiByteChar( Src[i], _GET_CHARSET() ) == FALSE )
//			{
//				iden[k] = Src[i] ;
//				iden[k+1] = 0 ; 
//				k ++ ;
//				i ++ ;
//			}
//			else
//			{
//				*(( unsigned short * )&iden[k] ) = *(( unsigned short * )&Src[i] ) ;
//				iden[k+2] = '\0' ;
//				k += 2 ;
//				i += 2 ;
//			}
//			break ;
//		}
//	}
//	
//END :
//	// 正常終了
//	return 0 ;
//}

// フルパスではないパス文字列をフルパスに変換する
// ( CurrentDir はフルパスである必要がある(語尾に『\』があっても無くても良い) )
// ( CurrentDir が NULL の場合は現在のカレントディレクトリを使用する )
extern	int ConvertFullPathW_( const wchar_t *Src, wchar_t *Dest, size_t BufferBytes, const wchar_t *CurrentDir )
{
	int i, j, k ;
	wchar_t  iden[ FILEPATH_MAX ] ;
	wchar_t  cur[ FILEPATH_MAX ] ;
	int      CharNum ;
	wchar_t *LastCharAddress ;
	DWORD    LastCharCode ;
	int      LastCharBytes ;
	int      FirstCheckEnd = FALSE ;
//	size_t   DestSize ;
	int      RootFolder = FALSE ;

//	DestSize = 0 ;

	if( CurrentDir == NULL )
	{
		DX_FGETDIR( cur, sizeof( cur ) ) ;
		CurrentDir = cur ;
	}

	if( Src == NULL )
	{
		_WCSCPY_S( Dest, BufferBytes, CurrentDir ) ;
		goto END ;
	}

	i = 0 ;
	j = 0 ;
	k = 0 ;
	
	// 最初に『\』又は『/』が２回連続で続いている場合はネットワークを介していると判断
	if( ( Src[0] == L'\\' && Src[1] == L'\\' ) ||
		( Src[0] == L'/'  && Src[1] == L'/'  ) )
	{
		Dest[0] = L'\\';
		Dest[1] = L'\0';

		i += 2;
		j ++ ;

		FirstCheckEnd = TRUE ;
	}

	// ドライブ名が書かれていたらそのドライブへ
	if( FirstCheckEnd == FALSE )
	{
		int l ;
		int m, n = 0 ;

		// 追加のドライブ名か調べる
		for( m = 0 ; m < g_AddDriveNameNum ; m ++ )
		{
			for( n = 0 ; ; n ++ )
			{
				if( Src[ n ] == L'\0' || g_AddDriveName[ m ][ n ] == L'\0' )
				{
					break ;
				}

				wchar_t SrcC ;
				wchar_t CheckC ;

				if( Src[ n ] == L'\\' || Src[ n ] == L'/' )
				{
					SrcC = L'\\' ;
				}
				else
				{
					SrcC = Src[ n ] ;
				}

				if( g_AddDriveName[ m ][ n ] == '\\' || g_AddDriveName[ m ][ n ] == '/' )
				{
					CheckC = L'\\' ;
				}
				else
				{
					CheckC = ( wchar_t )g_AddDriveName[ m ][ n ] ;
				}

				if( SrcC != CheckC )
				{
					break ;
				}
			}

			if( g_AddDriveName[ m ][ n ] == L'\0' )
			{
				break ;
			}
		}
		if( g_AddDriveNameNum != 0 && m != g_AddDriveNameNum )
		{
			// 落ちない場合は適合した文字列をそのまま残す
			_MEMCPY( Dest, Src, sizeof( wchar_t ) * n ) ;
			Dest[ n ] = L'\0' ;

			j = n ;
			i = n ;

			FirstCheckEnd = TRUE ;
		}
		else
		{
			// 通常のドライブ名のチェック
			l = 0 ;
			for(;;)
			{
				if( Src[ l ] == L'\0' )
				{
					break ;
				}

				if( CHECK_WCHAR_T_DOUBLE( Src[ l ] ) )
				{
					l += 2 ;
				}
				else
				{
					if( Src[ l ] == L':' )
					{
						break ;
					}

					if( ( Src[ l ] < L'a' || Src[ l ] > L'z' ) &&
						( Src[ l ] < L'A' || Src[ l ] > L'Z' ) &&
						( Src[ l ] < L'0' || Src[ l ] > L'9' ) )
					{
						break ;
					}

					l ++ ;
				}
			}
			if( Src[ l ] == ':' )
			{
				_MEMCPY( Dest, Src, sizeof( wchar_t ) * ( l + 1 ) ) ;
				Dest[ l + 1 ] = L'\0' ;

				i = l + 1 ;
				j = l + 1 ;

				if( Src[ i ] == L'\\' || Src[ i ] == L'/' )
				{
					i ++ ;
				}

				FirstCheckEnd = TRUE ;
			}
		}
	}

	// 最初が『\』又は『/』の場合はカレントドライブのルートディレクトリまで落ちる
	if( g_EnableRootDirName == 1 && FirstCheckEnd == FALSE && ( Src[0] == L'\\' || Src[0] == L'/' ) )
	{
//		int m, n ;
//
//		// カレントドライブのルートディレクトリに落とさない文字列かどうかを判定
//		for( m = 0 ; m < g_CancelCurrentDirNameNum ; m ++ )
//		{
//			for( n = 0 ; ; n ++ )
//			{
//				if( Src[ n ] == L'\0' || g_CancelCurrentDirName[ m ][ n ] == L'\0' )
//				{
//					break ;
//				}
//
//				wchar_t SrcC ;
//				wchar_t CheckC ;
//
//				if( Src[ n ] == L'\\' || Src[ n ] == L'/' )
//				{
//					SrcC = L'\\' ;
//				}
//				else
//				{
//					SrcC = Src[ n ] ;
//				}
//
//				if( g_CancelCurrentDirName[ m ][ n ] == '\\' || g_CancelCurrentDirName[ m ][ n ] == '/' )
//				{
//					CheckC = L'\\' ;
//				}
//				else
//				{
//					CheckC = ( wchar_t )g_CancelCurrentDirName[ m ][ n ] ;
//				}
//
//				if( SrcC != CheckC )
//				{
//					break ;
//				}
//			}
//
//			if( g_CancelCurrentDirName[ m ][ n ] == L'\0' )
//			{
//				break ;
//			}
//		}
//
//		if( g_CancelCurrentDirNameNum != 0 && m != g_CancelCurrentDirNameNum )
//		{
//			// 落ちない場合は適合した文字列をそのまま残す
//			_MEMCPY( Dest, Src, sizeof( wchar_t ) * n ) ;
//			Dest[ n ] = L'\0' ;
//
//			j = n ;
//			i = j ;
//
//			FirstCheckEnd = TRUE ;
//		}
//		else
		{
			Dest[0] = CurrentDir[0] ;
			Dest[1] = CurrentDir[1] ;
			Dest[2] = L'\0' ;

			i ++ ;
			j = 2 ;

			FirstCheckEnd = TRUE ;
		}

		RootFolder = TRUE ;
	}

	// それ以外の場合はカレントディレクトリ
	if( FirstCheckEnd == FALSE )
	{
		_WCSCPY_S( Dest, BufferBytes, CurrentDir ) ;
		j = ( int )_WCSLEN( Dest ) ;

		CharNum         = GetStringCharNum( ( const char * )CurrentDir, WCHAR_T_CHARCODEFORMAT ) ;
		if( CharNum != 0 )
		{
			LastCharAddress = ( wchar_t * )GetStringCharAddress( ( const char * )CurrentDir, WCHAR_T_CHARCODEFORMAT, CharNum - 1 ) ;
			LastCharCode    = GetCharCode( ( const char * )LastCharAddress, WCHAR_T_CHARCODEFORMAT, &LastCharBytes ) ;
			if( LastCharCode == L'\\' || LastCharCode == L'/' )
			{
				*LastCharAddress = L'\0' ;
				j -- ;
			}
		}

		FirstCheckEnd = TRUE ;
	}

	for(;;)
	{
		switch( Src[i] )
		{
		case L'\0' :
			if( k != 0 )
			{
				if( j != 0 )
				{
					Dest[j] = L'\\' ;
					j ++ ;
				}
				_WCSCPY_S( &Dest[j], BufferBytes - j * sizeof( wchar_t ), iden ) ;
				j = ( int )_WCSLEN( Dest ) ;
				k = 0 ;
			}
			goto END ;

		case L'\\' :
		case L'/' :
			// 文字列が無かったらスキップ
			if( k == 0 )
			{
				i ++ ;
				break ;
			}
		
			if( _WCSCMP( iden, L"." ) == 0 )
			{
				// なにもしない
			}
			else
			if( _WCSCMP( iden, L".." ) == 0 )
			{
				// 一つ浅いディレクトリへ
				Dest[ j ] = L'\0' ;
				CharNum         = GetStringCharNum(  ( const char * )Dest, WCHAR_T_CHARCODEFORMAT ) ;
				LastCharAddress = ( wchar_t * )GetStringCharAddress( ( const char * )Dest, WCHAR_T_CHARCODEFORMAT, CharNum - 1 ) ;
				LastCharCode    = GetCharCode( ( const char * )LastCharAddress, WCHAR_T_CHARCODEFORMAT, &LastCharBytes ) ;
				j -= LastCharBytes / sizeof( wchar_t ) ;
				for(;;)
				{
					if( Dest[j] == L'\\' || Dest[j] == L'/' || Dest[j] == L':' )
					{
						break ;
					}

					Dest[ j ] = L'\0' ;
					CharNum -- ;
					LastCharAddress = ( wchar_t * )GetStringCharAddress( ( const char * )Dest, WCHAR_T_CHARCODEFORMAT, CharNum - 1 ) ;
					LastCharCode    = GetCharCode( ( const char * )LastCharAddress, WCHAR_T_CHARCODEFORMAT, &LastCharBytes ) ;
					j -= LastCharBytes / sizeof( wchar_t ) ;
				}
				if( Dest[j] != L':' )
				{
					Dest[j] = L'\0' ;
				}
				else
				{
					j ++ ;
				}
			}
			else
			{
				if( ( RootFolder == FALSE && ( Src[ 0 ] == '\\' || Src[ 0 ] == '/' ) ) || j != 0 )
				{
					Dest[ j ] = L'\\' ;
					j ++ ;
				}

				_WCSCPY_S( &Dest[j], BufferBytes - j * sizeof( wchar_t ), iden ) ;
				j = ( int )_WCSLEN( Dest ) ;
			}

			k = 0 ;
			i ++ ;
			break ;
		
		default :
			if( CHECK_WCHAR_T_DOUBLE( Src[i] ) == FALSE )
			{
				iden[k] = Src[i] ;
				iden[k+1] = 0 ; 
				k ++ ;
				i ++ ;
			}
			else
			{
				iden[k]   = Src[i] ;
				iden[k+1] = Src[i+1] ;
				iden[k+2] = '\0' ;
				k += 2 ;
				i += 2 ;
			}
			break ;
		}
	}
	
END :
	// 正常終了
	return 0 ;
}

extern int ConvertFullPathT_( const TCHAR *Src, TCHAR *Dest, size_t BufferBytes, const TCHAR *CurrentDir )
{
#ifdef UNICODE
	return ConvertFullPathW_( Src, Dest, BufferBytes, CurrentDir ) ;
#else
	wchar_t SrcBuffer[ FILEPATH_MAX ] ;
	wchar_t DestBuffer[ FILEPATH_MAX ] ;
	wchar_t CurrentDirBuffer[ FILEPATH_MAX ] ;
	int Result ;

	ConvString( ( const char * )Src, -1, _TCHARCODEFORMAT, ( char * )SrcBuffer, sizeof( SrcBuffer ), WCHAR_T_CHARCODEFORMAT ) ;
	if( CurrentDir != NULL )
	{
		ConvString( ( const char * )CurrentDir, -1, _TCHARCODEFORMAT, ( char * )CurrentDirBuffer, sizeof( CurrentDirBuffer ), WCHAR_T_CHARCODEFORMAT ) ;
	}

	Result = ConvertFullPathW_( SrcBuffer, DestBuffer, sizeof( DestBuffer ), CurrentDir != NULL ? CurrentDirBuffer : NULL ) ;

	ConvString( ( const char * )DestBuffer, -1, WCHAR_T_CHARCODEFORMAT, ( char * )Dest, BufferBytes, _TCHARCODEFORMAT ) ;

	return Result ;
#endif
}

// 指定のファイルパスを指定のフォルダパスから相対アクセスするための相対パスを作成する
// ( FilePath や StartFolderPath がフルパスではなかった場合は関数内でフルパス化されます )
// StartFolderPath の末端に / or \ があっても問題ありません
//extern int CreateRelativePath_( const char *FilePath, const char *StartFolderPath, char *Dest )
//{
//	char filefull[ 512 ], filename[ 256 ], endfull[ 512 ], startfull[ 512 ] ;
//	char endstr[ 256 ], startstr[ 256 ] ;
//	int endlen, startlen, destlen ;
//	char *ep, *sp ;
//
//	// フルパス化
//	ConvertFullPath_( FilePath, filefull ) ;
//	AnalysisFileNameAndDirPath_( filefull, filename, endfull ) ;
//	ConvertFullPath_( StartFolderPath, startfull ) ;
//	startlen = _STRLEN( startfull ) ;
//	if( startfull[ startlen - 1 ] == '\\' || startfull[ startlen - 1 ] == '/' )
//		startfull[ startlen - 1 ] = '\0' ;
//
//	// ドライブ文字列を取得する
//	endlen = AnalysisDriveName_( endfull, endstr ) ;
//	startlen = AnalysisDriveName_( startfull, startstr ) ;
//
//	// ドライブが違ったらファイルパスのフルパスを代入する
//	if( _STRCMP( endstr, startstr ) != 0 )
//	{
//		_STRCPY( Dest, filefull ) ;
//		return 0 ;
//	}
//	ep = &endfull[ endlen ] ;
//	sp = &startfull[ startlen ] ;
//
//	// フォルダ名が違うところまで読み進める
//	for(;;)
//	{
//		// \ or / を飛ばす
//		while( *ep == '\\' || *ep == '/' ) ep ++ ;
//		while( *sp == '\\' || *sp == '/' ) sp ++ ;
//
//		// 終端文字だったらループを抜ける
//		if( *ep == '\0' || *sp == '\0' ) break ;
//
//		// フォルダパスを取得する
//		endlen = AnalysisDirectoryName_( ep, endstr ) ;
//		startlen = AnalysisDirectoryName_( sp, startstr ) ;
//
//		// フォルダパスが違ったらループを抜ける
//		if( _STRCMP( endstr, startstr ) != 0 ) break ;
//		ep += endlen ;
//		sp += startlen ;
//	}
//
//	// お互い同じフォルダだった場合はファイル名を出力
//	if( *ep == '\0' && *sp == '\0' )
//	{
//		_STRCPY( Dest, filename ) ;
//		return 0 ;
//	}
//
//	// 開始フォルダと違うフォルダにある場合はまず開始フォルダが現在のフォルダまで降りるパスの作成
//	destlen = 0 ;
//	if( *sp != '\0' )
//	{
//		Dest[ 0 ] = '\0' ;
//		destlen = 0 ;
//		for(;;)
//		{
//			startlen = AnalysisDirectoryName_( sp, startstr ) ;
//			sp += startlen ;
//			while( *sp == '\\' || *sp == '/' ) sp ++ ;
//			_STRCPY( Dest + destlen, "..\\" ) ;
//			destlen += 3 ;
//			if( *sp == '\0' ) break ;
//		}
//	}
//
//	// 目的のファイルがあるフォルダまでとファイル名をつなげて完成
//	_SPRINTF( Dest + destlen, "%s\\%s", ep, filename ) ;
//	return 0 ;
//}


// 指定のファイルパスを指定のフォルダパスから相対アクセスするための相対パスを作成する
// ( FilePath や StartFolderPath がフルパスではなかった場合は関数内でフルパス化されます )
// StartFolderPath の末端に / or \ があっても問題ありません
extern int CreateRelativePathW_( const wchar_t *FilePath, const wchar_t *StartFolderPath, wchar_t *Dest, size_t BufferBytes )
{
	wchar_t filefull[ 1024 ], filename[ 256 ], endfull[ 1024 ], startfull[ 1024 ] ;
	wchar_t endstr[ 256 ], startstr[ 256 ] ;
	int endlen, startlen, destlen ;
	wchar_t *ep, *sp ;

	// フルパス化
	ConvertFullPathW_( FilePath, filefull, sizeof( filefull ) ) ;
	AnalysisFileNameAndDirPathW_( filefull, filename, sizeof( filename ), endfull, sizeof( endfull ) ) ;
	ConvertFullPathW_( StartFolderPath, startfull, sizeof( startfull ) ) ;
	startlen = ( int )_WCSLEN( startfull ) ;
	if( startfull[ startlen - 1 ] == L'\\' || startfull[ startlen - 1 ] == L'/' )
	{
		startfull[ startlen - 1 ] = L'\0' ;
	}

	// ドライブ文字列を取得する
	endlen = AnalysisDriveNameW_( endfull, endstr, sizeof( endstr ) ) ;
	startlen = AnalysisDriveNameW_( startfull, startstr, sizeof( startstr ) ) ;

	// ドライブが違ったらファイルパスのフルパスを代入する
	{
		int i ;
		for( i = 0; endstr[ i ] != L'\0' && startstr[ i ] != L'\0' ; i ++ )
		{
			if( endstr[ i ] == startstr[ i ] )
			{
				continue ;
			}

			if( ( endstr[ i ]   == L'\\' || endstr[ i ]   == L'/' ) &&
				( startstr[ i ] == L'\\' || startstr[ i ] == L'/' ) )
			{
				continue ;
			}

			break ;
		}
		if( endstr[ i ] != L'\0' || startstr[ i ] != L'\0' )
		{
			_WCSCPY_S( Dest, BufferBytes, filefull ) ;
			return 0 ;
		}
		ep = &endfull[ endlen ] ;
		sp = &startfull[ startlen ] ;
	}

	// フォルダ名が違うところまで読み進める
	for(;;)
	{
		// \ or / を飛ばす
		while( *ep == L'\\' || *ep == L'/' ) ep ++ ;
		while( *sp == L'\\' || *sp == L'/' ) sp ++ ;

		// 終端文字だったらループを抜ける
		if( *ep == L'\0' || *sp == L'\0' ) break ;

		// フォルダパスを取得する
		endlen   = AnalysisDirectoryNameW_( ep, endstr, sizeof( endstr ) ) ;
		startlen = AnalysisDirectoryNameW_( sp, startstr, sizeof( startstr ) ) ;

		// フォルダパスが違ったらループを抜ける
		if( _WCSCMP( endstr, startstr ) != 0 ) break ;
		ep += endlen ;
		sp += startlen ;
	}

	// お互い同じフォルダだった場合はファイル名を出力
	if( *ep == L'\0' && *sp == L'\0' )
	{
		_WCSCPY_S( Dest, BufferBytes, filename ) ;
		return 0 ;
	}

	// 開始フォルダと違うフォルダにある場合はまず開始フォルダが現在のフォルダまで降りるパスの作成
	destlen = 0 ;
	if( *sp != L'\0' )
	{
		Dest[ 0 ] = L'\0' ;
		destlen = 0 ;
		for(;;)
		{
			startlen = AnalysisDirectoryNameW_( sp, startstr, sizeof( startstr ) ) ;
			sp += startlen ;
			while( *sp == L'\\' || *sp == L'/' ) sp ++ ;
			_WCSCPY_S( Dest + destlen, BufferBytes / sizeof( wchar_t ) - destlen, L"..\\" ) ;
			destlen += 3 ;
			if( *sp == L'\0' ) break ;
		}
	}

	// 目的のファイルがあるフォルダまでとファイル名をつなげて完成
	_SWNPRINTF( Dest + destlen, BufferBytes / sizeof( wchar_t ) - destlen, L"%s\\%s", ep, filename ) ;
	return 0 ;
}

extern int CreateRelativePathT_( const TCHAR *FilePath, const TCHAR *StartFolderPath, TCHAR *Dest, size_t BufferBytes )
{
#ifdef UNICODE
	return CreateRelativePathW_( FilePath, StartFolderPath, Dest, BufferBytes ) ;
#else
	wchar_t FilePathBuffer[ FILEPATH_MAX ] ;
	wchar_t StartFolderPathBuffer[ FILEPATH_MAX ] ;
	wchar_t DestBuffer[ FILEPATH_MAX ] ;
	int Result ;

	ConvString( ( const char * )FilePath,        -1, _TCHARCODEFORMAT, ( char * )FilePathBuffer,        sizeof( FilePathBuffer ),        WCHAR_T_CHARCODEFORMAT ) ;
	ConvString( ( const char * )StartFolderPath, -1, _TCHARCODEFORMAT, ( char * )StartFolderPathBuffer, sizeof( StartFolderPathBuffer ), WCHAR_T_CHARCODEFORMAT ) ;

	Result = CreateRelativePathW_( FilePathBuffer, StartFolderPathBuffer, DestBuffer, sizeof( DestBuffer ) ) ;

	ConvString( ( const char * )DestBuffer, -1, WCHAR_T_CHARCODEFORMAT, ( char * )Dest, BufferBytes, _TCHARCODEFORMAT ) ;

	return Result ;
#endif
}

// あるディレクトリパスから、別のディレクトリパスに移動するときのパス文字列の変化を行う
// ( StartDirPath がフルパスではなかった場合は関数内でフルパス化されます )
extern int			ChangeDirPathW_( const wchar_t * /*ChangePath*/, const wchar_t * /*StartDirPath*/, wchar_t * /*Dest*/ )
{
	int Result = 0 ;

	return Result ;
}

extern int			ChangeDirPathT_( const TCHAR   * /*ChangePath*/, const TCHAR   * /*StartDirPath*/, TCHAR   * /*Dest*/ )
{
	int Result = 0 ;

	return Result ;
}

// 特定のパス文字列から色々な情報を取得する
// ( CurrentDir はフルパスである必要がある(語尾に『\』があっても無くても良い) )
// ( CurrentDir が 0 の場合は実際のカレントディレクトリを使用する )
extern int AnalyseFilePathT_(
	const TCHAR *Src,
	TCHAR   *FullPath,	size_t FullPathBytes,
	TCHAR   *DirPath,	size_t DirPathBytes,
	TCHAR   *FileName,	size_t FileNameBytes,
	TCHAR   *Name,		size_t NameBytes,
	TCHAR   *ExeName,	size_t ExeNameBytes,
	const TCHAR *CurrentDir
)
{
	TCHAR full[1024], dir[1024], fname[256], name[256], exename[256];

	ConvertFullPathT_( Src, full, sizeof( full ), CurrentDir );
	AnalysisFileNameAndDirPathT_( full,  fname, sizeof( fname ), dir,     sizeof( dir     ) );
	AnalysisFileNameAndExeNameT_( fname, name,  sizeof( name ),  exename, sizeof( exename ) );
	if( FullPath ) _TSTRCPY_S( FullPath, FullPathBytes, full    );
	if( DirPath  ) _TSTRCPY_S( DirPath,  DirPathBytes,  dir     );
	if( FileName ) _TSTRCPY_S( FileName, FileNameBytes, fname   );
	if( Name     ) _TSTRCPY_S( Name,     NameBytes,     name    );
	if( ExeName  ) _TSTRCPY_S( ExeName,  ExeNameBytes,  exename );

	return 0;
}

// 特定のパス文字列から色々な情報を取得する
// ( CurrentDir はフルパスである必要がある(語尾に『\』があっても無くても良い) )
// ( CurrentDir が 0 の場合は実際のカレントディレクトリを使用する )
extern int AnalyseFilePathW_(
	const wchar_t *Src,
	wchar_t *FullPath,	size_t FullPathBytes,
	wchar_t *DirPath,	size_t DirPathBytes,
	wchar_t *FileName,	size_t FileNameBytes,
	wchar_t *Name,		size_t NameBytes,
	wchar_t *ExeName,	size_t ExeNameBytes,
	const wchar_t *CurrentDir
)
{
	wchar_t full[256], dir[256], fname[256], name[256], exename[256];

	ConvertFullPathW_( Src, full, sizeof( full ), CurrentDir );
	AnalysisFileNameAndDirPathW_( full,  fname, sizeof( fname ), dir,     sizeof( dir     ) );
	AnalysisFileNameAndExeNameW_( fname, name,  sizeof( name ),  exename, sizeof( exename ) );
	if( FullPath ) _WCSCPY_S( FullPath, FullPathBytes, full    ) ;
	if( DirPath  ) _WCSCPY_S( DirPath,  DirPathBytes,  dir     ) ;
	if( FileName ) _WCSCPY_S( FileName, FileNameBytes, fname   ) ;
	if( Name     ) _WCSCPY_S( Name,     NameBytes,     name    ) ;
	if( ExeName  ) _WCSCPY_S( ExeName,  ExeNameBytes,  exename ) ;

	return 0;
}


// ファイル名も一緒になっていると分かっているパス中からファイル名とディレクトリパスを分割する
// フルパスである必要は無い、ファイル名だけでも良い
//extern int AnalysisFileNameAndDirPath_( const char *Src, char *FileName, char *DirPath )
//{
//	int i, Last ;
//	
//	// ファイル名を抜き出す
//	i = 0 ;
//	Last = -1 ;
//	while( Src[i] != '\0' )
//	{
//		if( CheckMultiByteChar( Src[i], _GET_CHARCODEFORMAT() ) == FALSE )
//		{
//			if( Src[i] == '\\' || Src[i] == '/' || Src[i] == '\0' || Src[i] == ':' ) Last = i ;
//			i ++ ;
//		}
//		else
//		{
//			i += 2 ;
//		}
//	}
//	if( FileName != NULL )
//	{
//		if( Last != -1 ) _STRCPY( FileName, &Src[Last+1] ) ;
//		else _STRCPY( FileName, Src ) ;
//	}
//	
//	// ディレクトリパスを抜き出す
//	if( DirPath != NULL )
//	{
//		if( Last != -1 )
//		{
//			_STRNCPY( DirPath, Src, Last ) ;
//			DirPath[Last] = '\0' ;
//		}
//		else
//		{
//			DirPath[0] = '\0' ;
//		}
//	}
//	
//	// 終了
//	return 0 ;
//}

// ファイル名も一緒になっていると分かっているパス中からファイル名とディレクトリパスを分割する
// フルパスである必要は無い、ファイル名だけでも良い
extern int AnalysisFileNameAndDirPathW_( const wchar_t *Src, wchar_t *FileName, size_t FileNameBytes, wchar_t *DirPath, size_t DirPathBytes )
{
	int i, Last ;
	
	// ファイル名を抜き出す
	i = 0 ;
	Last = -1 ;
	while( Src[i] != L'\0' )
	{
		if( CHECK_WCHAR_T_DOUBLE( Src[i] ) == FALSE )
		{
			if( Src[i] == L'\\' || Src[i] == L'/' || Src[i] == L'\0' || Src[i] == L':' )
			{
				Last = i ;
			}
			i ++ ;
		}
		else
		{
			i += 2 ;
		}
	}
	if( FileName != NULL )
	{
		if( Last != -1 )
		{
			_WCSCPY_S( FileName, FileNameBytes, &Src[Last+1] ) ;
		}
		else
		{
			_WCSCPY_S( FileName, FileNameBytes, Src ) ;
		}
	}
	
	// ディレクトリパスを抜き出す
	if( DirPath != NULL )
	{
		if( Last != -1 )
		{
			_WCSNCPY_S( DirPath, DirPathBytes, Src, Last ) ;
			if( DirPathBytes >= ( Last + 1 ) * sizeof( wchar_t ) )
			{
				DirPath[ Last ] = L'\0' ;
			}
		}
		else
		{
			if( DirPathBytes >= sizeof( wchar_t ) )
			{
				DirPath[0] = L'\0' ;
			}
		}
	}
	
	// 終了
	return 0 ;
}

extern int AnalysisFileNameAndDirPathT_( const TCHAR *Src, TCHAR *FileName, size_t FileNameBytes, TCHAR   *DirPath, size_t DirPathBytes )
{
#ifdef UNICODE
	return AnalysisFileNameAndDirPathW_( Src, FileName, FileNameBytes, DirPath, DirPathBytes ) ;
#else
	wchar_t SrcBuffer[ FILEPATH_MAX ] ;
	wchar_t FileNameBuffer[ FILEPATH_MAX ] ;
	wchar_t DirPathBuffer[ FILEPATH_MAX ] ;
	int Result ;

	ConvString( ( const char * )Src, -1, _TCHARCODEFORMAT, ( char * )SrcBuffer, sizeof( SrcBuffer ), WCHAR_T_CHARCODEFORMAT ) ;

	Result = AnalysisFileNameAndDirPathW_( SrcBuffer, FileNameBuffer, sizeof( FileNameBuffer ), DirPathBuffer, sizeof( DirPathBuffer ) ) ;

	if( FileName )
	{
		ConvString( ( const char * )FileNameBuffer, -1, WCHAR_T_CHARCODEFORMAT, ( char * )FileName, FileNameBytes, _TCHARCODEFORMAT ) ;
	}

	if( DirPath )
	{
		ConvString( ( const char * )DirPathBuffer, -1, WCHAR_T_CHARCODEFORMAT, ( char * )DirPath, DirPathBytes, _TCHARCODEFORMAT ) ;
	}

	return Result ;
#endif
}

// ファイルパスからファイル名と拡張子を取得する
//extern int AnalysisFileNameAndExeName_( const char *Src, char *Name, char *ExeName )
//{
//	char FileName[256], *p, *p2, ename[128], name[128] ;
//
//	// ファイル名のみを取得
//	AnalysisFileNameAndDirPath_( Src, FileName, 0 ) ;
//
//	// 『.』があるかどうかで処理を分岐
//	if( ( p = _STRCHR( FileName, '.' ) ) == NULL )
//	{
//		_STRCPY( name, FileName ) ;
//		ename[0] = '\0' ;
//	}
//	else
//	{
//		for(;;)
//		{
//			p2 = _STRCHR( p + 1, '.' ) ;
//			if( p2 == NULL ) break ;
//			p = p2 ;
//		}
//
//		_STRNCPY( name, FileName, ( int )( p - FileName ) ) ;
//		name[p - FileName] = '\0' ;
//		_STRCPY( ename, p + 1 ) ;
//	}
//
//	if( Name != NULL ) _STRCPY( Name, name ) ;
//	if( ExeName != NULL ) _STRCPY( ExeName, ename ) ;
//
//	// 終了
//	return 0 ;
//}

// ファイルパスからファイル名と拡張子を取得する
extern int AnalysisFileNameAndExeNameW_( const wchar_t *Src, wchar_t *Name, size_t NameBytes, wchar_t *ExeName, size_t ExeNameBytes )
{
	wchar_t FileName[ 256 ] ;
	const wchar_t *p ;
	const wchar_t *p2 ;
	wchar_t ename[128] ;
	wchar_t name[128] ;

	// ファイル名のみを取得
	AnalysisFileNameAndDirPathW_( Src, FileName, sizeof( FileName ), 0, 0 ) ;

	// 『.』があるかどうかで処理を分岐
	p = _WCSCHR( FileName, L'.' ) ;
	if( p == NULL )
	{
		_WCSCPY_S( name, sizeof( name ), FileName ) ;
		ename[0] = L'\0' ;
	}
	else
	{
		for(;;)
		{
			p2 = _WCSCHR( p + 1, L'.' ) ;
			if( p2 == NULL ) break ;
			p = p2 ;
		}

		_WCSNCPY_S( name, sizeof( name ), FileName, ( int )( p - FileName ) ) ;
		if( ( p - FileName + 1 ) * sizeof( wchar_t ) < sizeof( name ) )
		{
			name[p - FileName] = L'\0' ;
		}
		_WCSCPY_S( ename, sizeof( ename ), p + 1 ) ;
	}

	if( Name    != NULL ) _WCSCPY_S( Name, NameBytes, name ) ;
	if( ExeName != NULL ) _WCSCPY_S( ExeName, ExeNameBytes, ename ) ;

	// 終了
	return 0 ;
}

// ファイルパスからファイル名と拡張子を取得する
extern int AnalysisFileNameAndExeNameT_( const TCHAR *Src, TCHAR *Name, size_t NameBytes, TCHAR   *ExeName, size_t ExeNameBytes )
{
	TCHAR FileName[256], *p, *p2, ename[128], name[128] ;

	// ファイル名のみを取得
	AnalysisFileNameAndDirPathT_( Src, FileName, sizeof( FileName ), 0, 0 ) ;

	// 『.』があるかどうかで処理を分岐
	p = ( TCHAR * )_TSTRCHR( FileName, _T( '.' ) ) ;
	if( p == NULL )
	{
		_TSTRCPY_S( name, sizeof( name ), FileName ) ;
		ename[0] = _T( '\0' ) ;
	}
	else
	{
		for(;;)
		{
			p2 = ( TCHAR * )_TSTRCHR( p + 1, _T( '.' ) ) ;
			if( p2 == NULL ) break ;
			p = p2 ;
		}

		_TSTRNCPY_S( name, sizeof( name ), FileName, ( int )( p - FileName ) ) ;
		if( ( p - FileName + 1 ) * sizeof( TCHAR ) < sizeof( name ) )
		{
			name[p - FileName] = _T( '\0' ) ;
		}
		_TSTRCPY_S( ename, sizeof( ename ), p + 1 ) ;
	}

	if( Name    != NULL ) _TSTRCPY_S( Name,    NameBytes,    name  ) ;
	if( ExeName != NULL ) _TSTRCPY_S( ExeName, ExeNameBytes, ename ) ;

	// 終了
	return 0 ;
}

// ファイルパスの拡張子を変えた文字列を得る
//extern int GetChangeExeNamePath_( const char *Src, char *Dest, const char *ExeName )
//{
//	char DirPath[256], FileName[128] ;
//
//	AnalysisFileNameAndDirPath_( Src, NULL, DirPath ) ;
//	AnalysisFileNameAndExeName_( Src, FileName, 0 ) ;
//	SetEnMark_( DirPath ) ;
//	_STRCPY( Dest, DirPath ) ;
//	_STRCAT( Dest, FileName ) ;
//	_STRCAT( Dest, "." ) ;
//	_STRCAT( Dest, ExeName ) ;
//	
//	// 終了
//	return 0 ;
//}


// 語尾に『\』がついていない場合は付ける
extern void SetEnMarkT_( TCHAR *PathBuf, size_t BufferBytes )
{
	int Len = ( int )_TSTRLEN( PathBuf ) ;
	
	if( PathBuf[ Len - 1 ] != '\\' )
	{
		_TSTRCAT_S( PathBuf, BufferBytes, _T( "\\" ) ) ;
	}
}

// 語尾に『\』がついていない場合は付ける
extern void SetEnMarkW_( wchar_t *PathBuf, size_t BufferBytes )
{
	int Len = ( int )_WCSLEN( PathBuf ) ;
	
	if( PathBuf[ Len - 1 ] != L'\\' )
	{
		_WCSCAT_S( PathBuf, BufferBytes, L"\\" ) ;
	}
}

// パス文字列中の『\』を『/』に変換する
extern void ChangeEnMarkToSlashT_( TCHAR   *PathBuf )
{
	TCHAR *p ;

	p = PathBuf ;
	for(;;)
	{
		p = ( TCHAR * )_TSTRCHR( p, _T( '\\' ) ) ;
		if( p == NULL )
		{
			break ;
		}

		*p = '/' ;
		p ++ ;
	}
}

// パス文字列中の『\』を『/』に変換する
extern void ChangeEnMarkToSlashW_( wchar_t *PathBuf )
{
	wchar_t *p ;

	p = PathBuf ;
	for(;;)
	{
		p = ( wchar_t * )_WCSCHR( p, L'\\' ) ;
		if( p == NULL )
		{
			break ;
		}

		*p = L'/' ;
		p ++ ;
	}
}


// 二つの文字列を比較する( Str2 にはワイルドカードの使用が可能 )
extern int Strcmp_Str2_WildcardT_( TCHAR   *Str1, TCHAR   *Str2 )
{
#ifdef UNICODE
	return Strcmp_Str2_WildcardW_( Str1, Str2 ) ;
#else
	wchar_t Str1W[ 1024 ] ;
	wchar_t Str2W[ 1024 ] ;

	ConvString( ( const char * )Str1, -1, _TCHARCODEFORMAT, ( char * )Str1W, sizeof( Str1W ), WCHAR_T_CHARCODEFORMAT ) ;
	ConvString( ( const char * )Str2, -1, _TCHARCODEFORMAT, ( char * )Str2W, sizeof( Str2W ), WCHAR_T_CHARCODEFORMAT ) ;

	return Strcmp_Str2_WildcardW_( Str1W, Str2W ) ;
#endif
}

// 二つの文字列を比較する( Str2 にはワイルドカードの使用が可能 )
extern int Strcmp_Str2_WildcardW_( wchar_t *Str1, wchar_t *Str2 )
{
	int i = 0 ;
	int j = 0 ;
//	int k = 0 ;

	for(;;)
	{
		switch( Str2[ j ] )
		{
		default :
			if( Str1[ i ] != Str2[ j ] )
			{
				goto RETURNCMP ;
			}
			i ++ ;
			j ++ ;
			break ;

		case L'?':
			if( Str1[ i ] == L'\0' )
			{
				return -1 ;
			}
			i ++ ;
			j ++ ;
			break ;

		case L'*':
			j ++ ;
			if( Str2[ j ] == L'\0' )
			{
				return 0 ;
			}

//			k = 0 ;
			for(;;)
			{
				int res ;

				if( Str1[ i ] == '\0' )
				{
					return -1 ;
				}

				res = Strcmp_Str2_WildcardW_( &Str1[ i ], &Str2[ j ] ) ;
				if( res == 0 )
				{
					return 0 ;
				}

				i ++ ;
			}
			break ;

		case L'\0':
			if( Str1[ i ] == L'\0' )
			{
				return 0 ;
			}
			goto RETURNCMP ;
		}
	}

//	return 0 ;

RETURNCMP :

	switch( sizeof( wchar_t ) )
	{
	case 2 :
		return *( ( WORD * )&Str1[ i ] ) < *( ( WORD * )&Str2[ j ] ) ? -1 : 1 ;

	case 4 :
		return *( ( DWORD * )&Str1[ i ] ) < *( ( DWORD * )&Str2[ j ] ) ? -1 : 1 ;

	default :
		return Str1[ i ] < Str2[ j ] ? -1 : 1 ;
	}
}

// 渡された文字列をフルパス文字列として扱い、ドライブ名( \ or / の前まで )
// 又はネットワークフォルダ名( \ or / の前まで )を取得する、
// ドライブ名だった場合は最後の : も含まれ、ネットワークフォルダだった場合は最初の \\ も含まれます
// 戻り値は取得した文字列の長さ( ドライブ名の場合は最後の :、ネットワークフォルダの場合は \\ も文字列量に含まれます )
// Src はフルパスである必要があります
//extern int AnalysisDriveName_( const char *Src, char *Dest )
//{
//	int len ;
//
//	// ネットワークフォルダの場合は最初の \\ を代入する
//	len = 0 ;
//	if( Src[ 0 ] == '\\' || Src[ 0 ] == '/' )
//	{
//		// ネットワークフォルダ
//		Dest[ 0 ] = Src[ 0 ] ;
//		Dest[ 1 ] = Src[ 1 ] ;
//		Dest += 2 ;
//		Src += 2 ;
//		len += 2 ;
//	}
//
//	// 後は文字列の末端か \ or / が出るまでコピー
//	while( *Src != '\0' && *Src != '\\' && *Src != '/' )
//	{
//		if( CheckMultiByteChar( *Src, _GET_CHARSET() ) )
//		{
//			Dest[ 0 ] = Src[ 0 ] ;
//			Dest[ 1 ] = Src[ 1 ] ;
//			Dest += 2 ;
//			Src += 2 ;
//			len += 2 ;
//		}
//		else
//		{
//			*Dest = *Src ;
//			Dest ++ ;
//			Src ++ ;
//			len ++ ;
//		}
//	}
//
//	// 終端文字をセット
//	*Dest = '\0' ;
//
//	// 文字列長を返す
//	return len ;
//}

// 渡された文字列をフルパス文字列として扱い、ドライブ名( \ or / の前まで )
// 又はネットワークフォルダ名( \ or / の前まで )を取得する、
// ドライブ名だった場合は最後の : も含まれ、ネットワークフォルダだった場合は最初の \\ も含まれます
// 戻り値は取得した文字列の長さ( ドライブ名の場合は最後の :、ネットワークフォルダの場合は \\ も文字列量に含まれます )
// Src はフルパスである必要があります
extern int AnalysisDriveNameW_( const wchar_t *Src, wchar_t *Dest, size_t BufferBytes )
{
	int len ;
	size_t DestSize ;

	DestSize = 0 ;

	if( BufferBytes - DestSize < sizeof( wchar_t ) )
	{
		return 0 ;
	}

	// ネットワークフォルダの場合は最初の \\ を代入する
	len = 0 ;
	if( Src[ 0 ] == L'\\' || Src[ 0 ] == L'/' )
	{
		// ネットワークフォルダ
		if( BufferBytes - DestSize < sizeof( wchar_t ) * 2 )
		{
			goto END ;
		}
		Dest[ 0 ] = Src[ 0 ] ;
		DestSize += sizeof( wchar_t ) ;
		if( BufferBytes - DestSize <= sizeof( wchar_t ) )
		{
			goto END ;
		}

		if( BufferBytes - DestSize < sizeof( wchar_t ) * 2 )
		{
			goto END ;
		}
		Dest[ 1 ] = Src[ 1 ] ;
		DestSize += sizeof( wchar_t ) ;
		if( BufferBytes - DestSize <= sizeof( wchar_t ) )
		{
			goto END ;
		}

		Dest += 2 ;
		Src += 2 ;
		len += 2 ;
	}

	// 後は文字列の末端か \ or / が出るまでコピー
	while( *Src != L'\0' && *Src != L'\\' && *Src != L'/' )
	{
		if( CHECK_WCHAR_T_DOUBLE( *Src ) )
		{
			if( BufferBytes - DestSize < sizeof( wchar_t ) * 2 )
			{
				goto END ;
			}
			Dest[ 0 ] = Src[ 0 ] ;
			DestSize += sizeof( wchar_t ) ;
			if( BufferBytes - DestSize <= sizeof( wchar_t ) )
			{
				goto END ;
			}

			if( BufferBytes - DestSize < sizeof( wchar_t ) * 2 )
			{
				goto END ;
			}
			Dest[ 1 ] = Src[ 1 ] ;
			DestSize += sizeof( wchar_t ) ;
			if( BufferBytes - DestSize <= sizeof( wchar_t ) )
			{
				goto END ;
			}

			Dest += 2 ;
			Src += 2 ;
			len += 2 ;
		}
		else
		{
			if( BufferBytes - DestSize < sizeof( wchar_t ) * 2 )
			{
				goto END ;
			}
			*Dest = *Src ;
			DestSize += sizeof( wchar_t ) ;
			if( BufferBytes - DestSize <= sizeof( wchar_t ) )
			{
				goto END ;
			}

			Dest ++ ;
			Src ++ ;
			len ++ ;
		}
	}

END :

	// 終端文字をセット
	*Dest = L'\0' ;

	// 文字列長を返す
	return len ;
}


// 渡された文字列をフォルダパス文字列として扱い、フォルダ名( \ or / の前まで )を取得します
// 渡す文字列がフルパスで、最初にドライブ名が書かれていたら正常な結果が得られません
// ../ 等の下位フォルダに降りる文字列があった場合は .. 等が出力されます
// 戻り値は取得した文字列の長さです
//extern int AnalysisDirectoryName_( const char *Src, char *Dest )
//{
//	int len ;
//
//	len = 0 ;
//	while( *Src != '\0' && *Src != '\\' && *Src != '/' )
//	{
//		if( CheckMultiByteChar( *Src, _GET_CHARSET() ) )
//		{
//			Dest[ 0 ] = Src[ 0 ] ;
//			Dest[ 1 ] = Src[ 1 ] ;
//			Dest += 2 ;
//			Src += 2 ;
//			len += 2 ;
//		}
//		else
//		{
//			*Dest = *Src ;
//			Dest ++ ;
//			Src ++ ;
//			len ++ ;
//		}
//	}
//
//	// 終端文字をセット
//	*Dest = '\0' ;
//
//	// 文字列長を返す
//	return len ;
//}

// 渡された文字列をフォルダパス文字列として扱い、フォルダ名( \ or / の前まで )を取得します
// 渡す文字列がフルパスで、最初にドライブ名が書かれていたら正常な結果が得られません
// ../ 等の下位フォルダに降りる文字列があった場合は .. 等が出力されます
// 戻り値は取得した文字列の長さです
extern int AnalysisDirectoryNameW_( const wchar_t *Src, wchar_t *Dest, size_t BufferBytes )
{
	int len ;
	size_t DestSize ;

	DestSize = 0 ;

	if( BufferBytes - DestSize < sizeof( wchar_t ) )
	{
		return 0 ;
	}

	len = 0 ;
	while( *Src != L'\0' && *Src != L'\\' && *Src != L'/' )
	{
		if( CHECK_WCHAR_T_DOUBLE( *Src ) )
		{
			if( BufferBytes - DestSize < sizeof( wchar_t ) * 2 )
			{
				goto END ;
			}
			Dest[ 0 ] = Src[ 0 ] ;
			DestSize += sizeof( wchar_t ) ;
			if( BufferBytes - DestSize <= sizeof( wchar_t ) )
			{
				goto END ;
			}

			if( BufferBytes - DestSize < sizeof( wchar_t ) * 2 )
			{
				goto END ;
			}
			Dest[ 1 ] = Src[ 1 ] ;
			DestSize += sizeof( wchar_t ) ;
			if( BufferBytes - DestSize <= sizeof( wchar_t ) )
			{
				goto END ;
			}

			Dest += 2 ;
			Src += 2 ;
			len += 2 ;
		}
		else
		{
			if( BufferBytes - DestSize < sizeof( wchar_t ) * 2 )
			{
				goto END ;
			}
			*Dest = *Src ;
			DestSize += sizeof( wchar_t ) ;
			if( BufferBytes - DestSize <= sizeof( wchar_t ) )
			{
				goto END ;
			}

			Dest ++ ;
			Src ++ ;
			len ++ ;
		}
	}

END :

	// 終端文字をセット
	*Dest = L'\0' ;

	// 文字列長を返す
	return len ;
}









// アーカイブファイルの読み込み機能を使うかどうかを設定する( FALSE:使用しない  TRUE:使用する )
extern int NS_SetUseDXArchiveFlag( int Flag )
{
	int DefaultFlag ;

	// フラグの状態が同じ場合は何もしない
	if( NotUseDXArchive == !Flag ) return 0 ;

	// 標準の読み込み機能が使用されているかどうかを取得しておく
	DefaultFlag = NS_GetStreamFunctionDefault() ;

	// フラグを更新
	NotUseDXArchive = !Flag ;

	// 現在標準の読み込み機能が使われている場合は関数ポインタを更新する
	if( DefaultFlag == TRUE )
	{
		NS_ChangeStreamFunctionW( NULL ) ;
	}

	// 終了
	return 0 ;
}

// ＤＸアーカイブファイルと通常のフォルダのどちらも存在した場合、どちらを優先させるかを設定する( 1:フォルダを優先 0:ＤＸアーカイブファイルを優先(デフォルト) )
extern int NS_SetDXArchivePriority( int Priority )
{
#ifndef DX_NON_DXA
	return DXA_DIR_SetDXArchivePriority( Priority ) ;
#else
	return -1;
#endif
}

// 検索するＤＸアーカイブファイルの拡張子を変更する
extern int NS_SetDXArchiveExtension( const TCHAR *Extension )
{
#ifndef DX_NON_DXA
#ifdef UNICODE
	return DXA_DIR_SetArchiveExtension( Extension ) ;
#else
	wchar_t ExtensionBuffer[ 1024 ] ;

	ConvString( ( const char * )Extension, -1, _TCHARCODEFORMAT, ( char * )ExtensionBuffer, sizeof( ExtensionBuffer ), WCHAR_T_CHARCODEFORMAT ) ;

	return DXA_DIR_SetArchiveExtension( ExtensionBuffer ) ;
#endif
#else
	return -1;
#endif
}

// 検索するＤＸアーカイブファイルの拡張子を設定する( Extension:拡張子名文字列 )
extern int NS_SetDXArchiveExtensionWithStrLen( const TCHAR *Extension, size_t ExtensionLength )
{
	int Result ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_ONE_BEGIN( Extension, ExtensionLength, return -1 )
	Result = NS_SetDXArchiveExtension( UseExtensionBuffer ) ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( Extension )
	return Result ;
}

// ＤＸアーカイブファイルの鍵文字列を設定する
extern int NS_SetDXArchiveKeyString( const TCHAR *KeyString )
{
#ifndef DX_NON_DXA
#ifdef UNICODE
	char TempBuffer[ 4096 ] ;

	ConvString( ( const char * )KeyString, -1, _TCHARCODEFORMAT, TempBuffer, sizeof( TempBuffer ), DX_CHARCODEFORMAT_SHIFTJIS ) ; 
	return DXA_DIR_SetKeyString( TempBuffer ) ;
#else
	return DXA_DIR_SetKeyString( KeyString ) ;
#endif
#else
	return -1;
#endif
}

// ＤＸアーカイブファイルの鍵文字列を設定する( KeyString:鍵文字列 )
extern int NS_SetDXArchiveKeyStringWithStrLen( const TCHAR *KeyString, size_t KeyStringLength )
{
	int Result ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_ONE_BEGIN( KeyString, KeyStringLength, return -1 )
	Result = NS_SetDXArchiveKeyString( UseKeyStringBuffer ) ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( KeyString )
	return Result ;
}














// ファイルアクセス関数
extern DWORD_PTR ReadOnlyFileAccessOpen( const wchar_t *Path, int UseCacheFlag, int BlockReadFlag, int UseASyncReadFlag )
{
	FILEACCESS *FileAccess ;

	// メモリの確保
	FileAccess = ( FILEACCESS * )DXALLOC( sizeof( FILEACCESS ) ) ;
	if( FileAccess == NULL )
	{
		return 0 ;
	}

	// ゼロ初期化
	_MEMSET( FileAccess, 0, sizeof( FILEACCESS ) ) ;

	// 非同期読み込みを行うかどうかを保存
	FileAccess->UseASyncReadFlag = UseASyncReadFlag ;

	// 環境依存処理
	if( ReadOnlyFileAccessOpen_PF( FileAccess, Path, UseCacheFlag, BlockReadFlag ) < 0 )
	{
		DXFREE( FileAccess ) ;
		FileAccess = NULL ;

		return 0 ;
	}

	return ( DWORD_PTR )FileAccess ;
}

extern int ReadOnlyFileAccessClose( DWORD_PTR Handle )
{
	FILEACCESS *FileAccess = ( FILEACCESS * )Handle ;
	int Result ;

	// 環境依存処理
	Result = ReadOnlyFileAccessClose_PF( FileAccess ) ;
	if( Result < 0 )
	{
		return -1 ;
	}

	// メモリの解放
	DXFREE( FileAccess ) ;
	FileAccess = NULL ;


	return Result ;
}

extern LONGLONG ReadOnlyFileAccessTell( DWORD_PTR Handle )
{
	FILEACCESS *FileAccess = ( FILEACCESS * )Handle ;

	return ( LONGLONG )FileAccess->Position ;
}

extern int ReadOnlyFileAccessSeek( DWORD_PTR Handle, LONGLONG SeekPoint, int SeekType )
{
	FILEACCESS *FileAccess = ( FILEACCESS * )Handle ;
	LONGLONG Pos = 0 ;

	switch( SeekType )
	{
	default :
		return -1 ;

	case SEEK_CUR :
		Pos = ( LONGLONG )FileAccess->Position + SeekPoint ;
		break ;

	case SEEK_END :
		Pos = ( LONGLONG )FileAccess->Size + SeekPoint ;
		break ;

	case SEEK_SET :
		Pos = SeekPoint ;
		break ;
	}
	if( Pos < 0 )
	{
		Pos = 0 ;
	}
	else 
	if( Pos > ( LONGLONG )FileAccess->Size )
	{
		Pos = ( LONGLONG )FileAccess->Size ;
	}

	// 環境依存処理
	if( ReadOnlyFileAccessSeek_PF( FileAccess, Pos ) < 0 )
	{
		return -1 ;
	}

	// 位置を保存しておく
	FileAccess->Position = Pos ;

	// 終端チェックフラグを倒す
	FileAccess->EofFlag = FALSE ;

	// 終了
	return 0 ;
}

extern size_t ReadOnlyFileAccessRead( void *Buffer, size_t BlockSize, size_t DataNum, DWORD_PTR Handle )
{
	FILEACCESS *FileAccess = ( FILEACCESS * )Handle ;
	size_t Result ;

	if( BlockSize == 0 )
	{
		return 0 ;
	}

	// 終端チェック
	if( FileAccess->Position == FileAccess->Size )
	{
		FileAccess->EofFlag = TRUE ;
		return 0 ;
	}

	// 項目数調整
	if( BlockSize * DataNum + FileAccess->Position > FileAccess->Size )
	{
		DataNum = ( size_t )( ( FileAccess->Size - FileAccess->Position ) / BlockSize ) ;
	}
	
	if( DataNum == 0 )
	{
		FileAccess->EofFlag = TRUE ;
		return 0 ;
	}

	// 環境依存処理
	Result = ReadOnlyFileAccessRead_PF( Buffer, BlockSize, DataNum, FileAccess ) ;

	// ファイルアクセス位置の変更
	FileAccess->Position += Result ;

	return Result != 0 ? Result / BlockSize : 0 ;
}

extern int ReadOnlyFileAccessEof( DWORD_PTR Handle )
{
	FILEACCESS *FileAccess = ( FILEACCESS * )Handle ;

	return FileAccess->EofFlag ? EOF : 0 ;
}

extern int ReadOnlyFileAccessIdleCheck( DWORD_PTR Handle )
{
	FILEACCESS *FileAccess = ( FILEACCESS * )Handle ;

	// 環境依存処理
	return ReadOnlyFileAccessIdleCheck_PF( FileAccess ) ;
}

extern int ReadOnlyFileAccessChDir( const wchar_t *Path )
{
	// 環境依存処理
	return ReadOnlyFileAccessChDir_PF( Path ) ;
}

extern int ReadOnlyFileAccessGetDir( wchar_t *Buffer )
{
	// 環境依存処理
	return ReadOnlyFileAccessGetDirS_PF( Buffer, BUFFERBYTES_CANCEL ) ;
}

extern int ReadOnlyFileAccessGetDirS( wchar_t *Buffer, size_t BufferBytes )
{
	// 環境依存処理
	return ReadOnlyFileAccessGetDirS_PF( Buffer, BufferBytes ) ;
}

// 戻り値: -1=エラー  -1以外=FindHandle
extern DWORD_PTR ReadOnlyFileAccessFindFirst( const wchar_t *FilePath, FILEINFOW *Buffer )
{
	FINDINFO *FindInfo ;

	// メモリの確保
	FindInfo = ( FINDINFO * )DXALLOC( sizeof( FINDINFO ) ) ;
	if( FindInfo == NULL )
	{
		return ( DWORD_PTR )-1 ;
	}

	// 環境依存処理
	if( ReadOnlyFileAccessFindFirst_PF( FindInfo, FilePath, Buffer ) < 0 )
	{
		DXFREE( FindInfo ) ;
		FindInfo = NULL ;
		return ( DWORD_PTR )-1 ;
	}

	return ( DWORD_PTR )FindInfo ;
}

// 戻り値: -1=エラー  0=成功
extern int ReadOnlyFileAccessFindNext( DWORD_PTR FindHandle, FILEINFOW *Buffer )
{
	FINDINFO *FindInfo = ( FINDINFO * )FindHandle ;

	// 環境依存処理
	return ReadOnlyFileAccessFindNext_PF( FindInfo, Buffer ) ;
}

// 戻り値: -1=エラー  0=成功
extern int ReadOnlyFileAccessFindClose( DWORD_PTR FindHandle )
{
	FINDINFO *FindInfo = ( FINDINFO * )FindHandle ;
	int Result ;

	// 環境依存処理
	Result = ReadOnlyFileAccessFindClose_PF( FindInfo ) ;
	if( Result < 0 )
	{
		return -1 ;
	}

	// メモリの解放
	DXFREE( FindInfo ) ;
	FindInfo = NULL ;

	// 正常終了
	return 0 ;
}













// 書き込み専用ファイルアクセス関数
extern	int			WriteOnlyFileAccessDelete( const wchar_t *Path )
{
	return WriteOnlyFileAccessDelete_PF( Path ) ;
}

extern	DWORD_PTR	WriteOnlyFileAccessOpen(  const wchar_t *Path )
{
	return WriteOnlyFileAccessOpen_PF( Path ) ;
}

extern	DWORD_PTR	WriteOnlyFileAccessOpenWCHAR( const char *Path )
{
	return WriteOnlyFileAccessOpen( ( const wchar_t * )Path ) ;
}

extern	int			WriteOnlyFileAccessClose( DWORD_PTR Handle )
{
	return WriteOnlyFileAccessClose_PF( Handle ) ;
}

extern	int			WriteOnlyFileAccessSeek(  DWORD_PTR Handle, LONGLONG SeekPoint, int SeekType )
{
	return WriteOnlyFileAccessSeek_PF( Handle, SeekPoint, SeekType ) ;
}

extern	int			WriteOnlyFileAccessWrite( DWORD_PTR Handle, void *Buffer, size_t WriteSize, size_t *GetWriteSize )
{
	return WriteOnlyFileAccessWrite_PF( Handle, Buffer, WriteSize, GetWriteSize ) ;
}

extern	int			WriteOnlyFileAccessPrintf( DWORD_PTR Handle, const char *FormatString, ... )
{
	va_list VaList ;
	char String[ 4096 ] ;
	int Length ;
	int i ;
	
	va_start( VaList, FormatString ) ;
	
	CL_vsnprintf( CHAR_CHARCODEFORMAT, FALSE, CHAR_CHARCODEFORMAT, WCHAR_T_CHARCODEFORMAT, ( char * )String, sizeof( String ), ( const char * )FormatString, VaList ) ;
	
	va_end( VaList ) ;

	// \n を \r\n に置き換える
	Length = ( int )CL_strlen( CHAR_CHARCODEFORMAT, String ) ;
	for( i = 0 ; i < Length ; )
	{
		if( ( size_t )GetCharBytes_( ( char * )( &String[ i ] ), CHAR_CHARCODEFORMAT ) > sizeof( char ) )
		{
			i += 2 ;
		}
		else
		if( String[ i ] == '\n' )
		{
			_MEMMOVE( &String[ i + 1 ], &String[ i ], ( Length - i ) * sizeof( char ) ) ;
			String[ i ] = '\r' ;
			i += 2 ;
			Length ++ ;
		}
		else
		{
			i ++ ;
		}
	}

	WriteOnlyFileAccessWrite_PF( Handle, String, Length * sizeof( char ) ) ;

	return 0 ;
}








#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE
