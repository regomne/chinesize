// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		アーカイブ制御プログラム
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------


// ＤＸライブラリ作成時用定義
#define __DX_MAKE

// インクルード-------------------------------------------------------------------
#include "DxArchive_.h"
#include "DxBaseFunc.h"
#include "DxMemory.h"
#include "DxFile.h"
#include "DxFont.h"
#include "DxLog.h"
#include "DxChar.h"
#include "DxThread.h"
#include "DxSystem.h"
#include <stdio.h>


#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

#ifndef DX_NON_DXA

// マクロ定義 --------------------------------------------------------------------

#define DXARCD						DX_ArchiveDirData
//#define CHECKMULTIBYTECHAR(CP)		(( (unsigned char)*(CP) >= 0x81 && (unsigned char)*(CP) <= 0x9F ) || ( (unsigned char)*(CP) >= 0xE0 && (unsigned char)*(CP) <= 0xFC ))	// TRUE:２バイト文字  FALSE:１バイト文字
#define CHARUP(C)					( (C) >= 'a' && (C) <= 'z' ? (C) - 'a' + 'A' : (C))

#define DXARC_ID_AND_VERSION_SIZE	(sizeof( WORD ) * 2)

#define DXARC_HEAD_VER8_SIZE		(64)

#define DXARC_FILEHEAD_VER8_SIZE	(72)			// Ver0x0008 の DXARC_FILEHEAD 構造体のサイズ

#ifdef __USE_ULL__
#define NONE_PAL		(0xffffffffffffffffULL)
#else
#define NONE_PAL		(0xffffffffffffffff)
#endif

// 構造体定義 --------------------------------------------------------------------

// DXA_DIR_FindFirst 等の処理で使用する構造体
struct DXA_DIR_FINDDATA
{
	int							UseArchiveFlag;					// アーカイブファイルを使用しているかフラグ
	int							UseArchiveIndex;				// アーカイブを使用している場合、使用しているアーカイブファイルデータのインデックス
	DWORD_PTR					FindHandle;						// ファイル検索用ハンドル
} ;

// DXA_FindFirst 等の処理で使用する構造体
struct DXA_FINDDATA
{
	DXARC						*Container;						// 検索対象のＤＸＡファイル
	BYTE						SearchStr[ FILEPATH_MAX ] ;		// 検索文字列
	DXARC_DIRECTORY				*Directory;						// 検索対象のディレクトリ
	DWORD						ObjectCount;					// 次に渡すディレクトリ内オブジェクトのインデックス
} ;

// 数値ごとの出現数や算出されたエンコード後のビット列や、結合部分の情報等の構造体
struct HUFFMAN_NODE
{
	ULONGLONG					Weight ;						// 出現数( 結合データでは出現数を足したモノ )
	int							BitNum ;						// 圧縮後のビット列のビット数( 結合データでは使わない )
	unsigned char				BitArray[ 32 ] ;				// 圧縮後のビット列( 結合データでは使わない )
	int							Index ;							// 結合データに割り当てられた参照インデックス( 0 or 1 )
	
	int							ParentNode ;					// このデータを従えている結合データの要素配列のインデックス
	int							ChildNode[ 2 ] ;				// このデータが結合させた２要素の要素配列インデックス( 結合データではない場合はどちらも -1 )
} ;

// ビット単位入出力用データ構造体
struct BIT_STREAM
{
	BYTE						*Buffer ;
	ULONGLONG					Bytes ;
	DWORD						Bits ;
} ;

// ハフマン圧縮データのヘッダの構造
//   6bit      圧縮前のデータのサイズのビット数(A) - 1( 0=1ビット 63=64ビット )
//   (A)bit    圧縮前のデータのサイズ
//   6bit      圧縮後のデータのサイズのビット数(B) - 1( ヘッダ部分含まない )( 0=1ビット 63=64ビット )
//   (B)bit    圧縮後のデータのサイズ
//
//   3bit      数値ごとの出現頻度の差分値のビット数(C) / 2 - 1( 0=2ビット 7=16ビット )
//   1bit      符号ビット( 0=プラス  1=マイナス )
//   (C)bit    数値ごとの出現頻度の差分値
//   ↑これが256個ある


// 内部大域変数宣言 --------------------------------------------------------------

BYTE Ascii_DotStr[ 2 ]       = { '.',       0 } ;
BYTE Ascii_DoubleDotStr[ 3 ] = { '.',  '.', 0 } ;
BYTE Ascii_EnStr[ 2 ]        = { '\\',      0 } ;
BYTE Ascii_SlashStr[ 2 ]     = { '/',       0 } ;

// デフォルト鍵文字列
static char DefaultKeyString[ 9 ] = { 0x44, 0x58, 0x42, 0x44, 0x58, 0x41, 0x52, 0x43, 0x00 } ; // "DXLIBARC"

// アーカイブをディレクトリに見立てる為のデータ
DXARC_DIR DX_ArchiveDirData ;

// 関数プロトタイプ宣言-----------------------------------------------------------

static DXARC_FILEHEAD      *DXA_GetFileHeader(		DXARC *DXA, const BYTE *FilePath, DXARC_DIRECTORY **DirectoryP ) ;						// ファイルの情報を得る
static int		DXA_ConvSearchData(					DXARC *DXA, DXARC_SEARCHDATA *Dest, const BYTE *Src, int *Length ) ;					// 文字列を検索用のデータに変換( ヌル文字か \ があったら終了 )
static int		DXA_ChangeCurrentDirectoryFast(		DXARC *DXA, DXARC_SEARCHDATA *SearchData ) ;											// アーカイブ内のディレクトリパスを変更する( 0:成功  -1:失敗 )
static int		DXA_ChangeCurrentDirectoryBase(		DXARC *DXA, const BYTE *DirectoryPath, bool ErrorIsDirectoryReset, DXARC_SEARCHDATA *LastSearchData = NULL ) ;		// アーカイブ内のディレクトリパスを変更する( 0:成功  -1:失敗 )
static size_t	DXA_CreateKeyFileString(			DXARC *DXA, DXARC_DIRECTORY *Directory, DXARC_FILEHEAD *FileHead, BYTE *FileString ) ;	// カレントディレクトリにある指定のファイルの鍵用の文字列を作成する、戻り値は文字列の長さ( 単位：Byte )( FileString は DXA_KEY_STRING_MAXLENGTH の長さが必要 )
//static int	DXA_Decode(							void *Src, void *Dest ) ;																// データを解凍する( 戻り値:解凍後のデータサイズ )
static void		DXA_KeyCreate(						const char *Source, size_t SourceBytes, unsigned char *Key ) ;							// 鍵文字列を作成
static void		DXA_KeyConv(						void *Data, LONGLONG  SizeLL, LONGLONG  PositionLL,  unsigned char *Key ) ;				// 鍵文字列を使用して Xor 演算( Key は必ず DXA_KEYSTR_LENGTH の長さがなければならない )
static void		DXA_KeyConvFileRead(				void *Data, ULONGLONG Size,   DWORD_PTR FilePointer, unsigned char *Key, LONGLONG Position = -1 ) ;	// ファイルから読み込んだデータを鍵文字列を使用して Xor 演算する関数( Key は必ず DXA_KEYSTR_LENGTH の長さがなければならない )
static int		DXA_FindProcess(					DXA_FINDDATA *FindData, FILEINFOW *FileInfo );											// 条件に適合するオブジェクトを検索する(検索対象は ObjectCount をインデックスとしたところから)(戻り値 -1:エラー 0:成功)

static int		DXA_DIR_OpenArchive(				const wchar_t *FilePath, void *FileImage = NULL, int FileSize = -1, int FileImageCopyFlag = FALSE, int FileImageReadOnly = FALSE, int ArchiveIndex = -1, int OnMemory = FALSE, int ASyncThread = FALSE ) ;	// アーカイブファイルを開く
static int		DXA_DIR_GetArchive(					const wchar_t *FilePath, void *FileImage = NULL ) ;										// 既に開かれているアーカイブのハンドルを取得する( 戻り値: -1=無かった 0以上:ハンドル )
static int		DXA_DIR_CloseArchive(				int ArchiveHandle ) ;																	// アーカイブファイルを閉じる
static void		DXA_DIR_CloseWaitArchive(			void ) ;																				// 使用されるのを待っているアーカイブファイルを全て閉じる
static int		DXA_DIR_ConvertFullPath(			const wchar_t *Src, wchar_t *Dest, size_t BufferBytes, int CharUp = 1 ) ;				// 全ての英字小文字を大文字にしながら、フルパスに変換する
static int		DXA_DIR_AnalysisFileNameAndDirPath( DXARC *DXA, const BYTE *Src, BYTE *FileName = 0, size_t FileNameBytes = 0, BYTE *DirPath = 0, size_t DirPathBytes = 0 ) ;					// ファイル名も一緒になっていると分かっているパス中からファイル名とディレクトリパスを分割する。フルパスである必要は無い、ファイル名だけでも良い、DirPath の終端に ￥ マークは付かない
static int		DXA_DIR_FileNameCmp(				DXARC *DXA, const BYTE *Src, const BYTE *CmpStr );										// CmpStr の条件に Src が適合するかどうかを調べる( 0:適合する  -1:適合しない )
static int		DXA_DIR_OpenTest(					const wchar_t *FilePath, int *ArchiveIndex, BYTE *ArchiveFilePath, size_t BufferBytes ) ;	// アーカイブファイルをフォルダに見立ててファイルを開く時の情報を得る( -1:アーカイブとしては存在しなかった  0:存在した )

static int		DXA_DirectoryKeyConv(				DXARC *DXA, DXARC_DIRECTORY *Dir, char *KeyStringBuffer ) ;								// 指定のディレクトリデータの暗号化を解除する( 丸ごとメモリに読み込んだ場合用 )

static void			BitStream_Init(  BIT_STREAM *BitStream, void *Buffer, bool IsRead ) ;			// ビット単位入出力の初期化
static void			BitStream_Write( BIT_STREAM *BitStream, BYTE BitNum, ULONGLONG OutputData ) ;	// ビット単位の数値の書き込みを行う
static ULONGLONG	BitStream_Read(  BIT_STREAM *BitStream, BYTE BitNum ) ;							// ビット単位の数値の読み込みを行う
static BYTE			BitStream_GetBitNum( ULONGLONG Data ) ;											// 指定の数値のビット数を取得する
static ULONGLONG	BitStream_GetBytes( BIT_STREAM *BitStream ) ;									// ビット単位の入出力データのサイズ( バイト数 )を取得する

// プログラム --------------------------------------------------------------------

// ビット単位入出力の初期化
void BitStream_Init( BIT_STREAM *BitStream, void *Buffer, bool IsRead )
{
	BitStream->Buffer = ( BYTE * )Buffer ;
	BitStream->Bytes = 0 ;
	BitStream->Bits = 0 ;
	if( IsRead == false )
	{
		BitStream->Buffer[ 0 ] = 0 ;
	}
}

// ビット単位の数値の書き込みを行う
void BitStream_Write( BIT_STREAM *BitStream, BYTE BitNum, ULONGLONG OutputData )
{
	DWORD i ;
	for( i = 0 ; i < BitNum ; i ++ )
	{
		BitStream->Buffer[ BitStream->Bytes ] |= ( ( OutputData >> ( BitNum - 1 - i ) ) & 1 ) << ( 7 - BitStream->Bits ) ;
		BitStream->Bits ++ ;
		if( BitStream->Bits == 8 )
		{
			BitStream->Bytes ++ ;
			BitStream->Bits = 0 ;
			BitStream->Buffer[ BitStream->Bytes ] = 0 ;
		}
	}
}

// ビット単位の数値の読み込みを行う
ULONGLONG BitStream_Read( BIT_STREAM *BitStream, BYTE BitNum )
{
	ULONGLONG Result = 0 ;
	DWORD i ;
	for( i = 0 ; i < BitNum ; i ++ )
	{
		Result |= ( ( ULONGLONG )( ( BitStream->Buffer[ BitStream->Bytes ] >> ( 7 - BitStream->Bits ) ) & 1 ) ) << ( BitNum - 1 - i ) ;
		BitStream->Bits ++ ;
		if( BitStream->Bits == 8 )
		{
			BitStream->Bytes ++ ;
			BitStream->Bits = 0 ;
		}
	}

	return Result ;
}

// 指定の数値のビット数を取得する
BYTE BitStream_GetBitNum( ULONGLONG Data )
{
	DWORD i ;
	for( i = 1 ; i < 64 ; i ++ )
	{
		if( Data < ( 1ULL << i ) )
		{
			return ( BYTE )i ;
		}
	}

	return ( BYTE )i ;
}

// ビット単位の入出力データのサイズ( バイト数 )を取得する
ULONGLONG BitStream_GetBytes( BIT_STREAM *BitStream )
{
	return BitStream->Bytes + ( BitStream->Bits != 0 ? 1 : 0 ) ;
}

// ファイルの情報を得る
static DXARC_FILEHEAD *DXA_GetFileHeader( DXARC *DXA, const BYTE *FilePath, DXARC_DIRECTORY **DirectoryP )
{
	DXARC_DIRECTORY *OldDir ;
	DXARC_FILEHEAD *FileH ;
	DWORD FileHeadSize ;
	BYTE *NameData ;
	int i, j, k, Num ;
	DXARC_SEARCHDATA SearchData ;

	// 元のディレクトリを保存しておく
	OldDir = DXA->CurrentDirectory ;

	// ファイルパスに \ or / が含まれている場合、ディレクトリ変更を行う
	if( CL_strchr( DXA->CharCodeFormat, ( const char * )FilePath, '\\' ) != NULL ||
		CL_strchr( DXA->CharCodeFormat, ( const char * )FilePath, '/'  ) != NULL )
	{
		// カレントディレクトリを目的のファイルがあるディレクトリに変更する
		if( DXA_ChangeCurrentDirectoryBase( DXA, FilePath, false, &SearchData ) >= 0 )
		{
			// エラーが起きなかった場合はファイル名もディレクトリだったことになるのでエラー
			goto ERR ;
		}
	}
	else
	{
		// ファイル名を検索用データに変換する
		DXA_ConvSearchData( DXA, &SearchData, FilePath, NULL ) ;
	}

	// 同名のファイルを探す
	FileHeadSize = DXARC_FILEHEAD_VER8_SIZE ;
	FileH        = ( DXARC_FILEHEAD * )( DXA->Table.FileTable + DXA->CurrentDirectory->FileHeadAddress ) ;
	Num          = ( int )DXA->CurrentDirectory->FileHeadNum ;
	for( i = 0 ; i < Num ; i ++, FileH = (DXARC_FILEHEAD *)( (BYTE *)FileH + FileHeadSize ) )
	{
		// ディレクトリチェック
		if( ( FileH->Attributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 )
		{
			continue ;
		}

		// 文字列数とパリティチェック
		NameData = DXA->Table.NameTable + FileH->NameAddress ;
		if( SearchData.PackNum != ( ( WORD * )NameData )[ 0 ] ||
			SearchData.Parity  != ( ( WORD * )NameData )[ 1 ] )
		{
			continue ;
		}

		// 文字列チェック
		NameData += 4 ;
		for( j = 0, k = 0 ; j < SearchData.PackNum ; j ++, k += 4 )
		{
			if( *( ( DWORD * )&SearchData.FileName[ k ] ) != *( ( DWORD * )&NameData[ k ] ) )
			{
				break ;
			}
		}

		// 適合したファイルがあったらここで終了
		if( SearchData.PackNum == j )
		{
			break ;
		}
	}

	// 無かったらエラー
	if( i == Num )
	{
		goto ERR ;
	}

	// ディレクトリを取得する指定があった場合は代入
	if( DirectoryP != NULL )
	{
		*DirectoryP = DXA->CurrentDirectory ;
	}
	
	// ディレクトリを元に戻す
	DXA->CurrentDirectory = OldDir ;
	
	// 目的のファイルのアドレスを返す
	return FileH ;
	
ERR :
	// ディレクトリを元に戻す
	DXA->CurrentDirectory = OldDir ;
	
	// エラー終了
	return NULL ;
}


// 文字列を検索用のデータに変換( ヌル文字か \ があったら終了 )
static int DXA_ConvSearchData( DXARC *DXA, DXARC_SEARCHDATA *Dest, const BYTE *Src, int *Length )
{
	int i, j ;
	int StringLength ;
	WORD  ParityData ;
	DWORD CharCode ;
	int CharBytes ;

	ParityData = 0 ;
	i          = 0 ;
	for(;;)
	{
		CharCode = GetCharCode( ( const char * )&Src[ i ], DXA->CharCodeFormat, &CharBytes ) ;
		if( CharCode == '\0' || CharCode == '\\' || CharCode == '/' )
		{
			break ;
		}

		// 小文字の場合は大文字に変換
		if( CharCode >= 'a' && CharCode <= 'z' )
		{
			CharCode = CharCode - 'a' + 'A' ;
			PutCharCode( CharCode, DXA->CharCodeFormat, ( char * )&Dest->FileName[ i ], sizeof( Dest->FileName ) - i ) ;

			switch( CharBytes )
			{
			case 1 :
				ParityData += Dest->FileName[ i ] ;
				break ;

			case 2 :
				ParityData += Dest->FileName[ i ] + Dest->FileName[ i + 1 ] ;
				break ;

			case 4 :
				ParityData += Dest->FileName[ i ] + Dest->FileName[ i + 1 ] + Dest->FileName[ i + 2 ] + Dest->FileName[ i + 3 ] ;
				break ;

			default :
				for( j = 0 ; j < CharBytes ; j ++ )
				{
					ParityData += Dest->FileName[ i + j ] ;
				}
				break ;
			}
		}
		else
		{
			switch( CharBytes )
			{
			case 1 :
				Dest->FileName[ i ] = Src[ i ] ;
				ParityData += Src[ i ] ;
				break ;

			case 2 :
				*( ( WORD * )&Dest->FileName[ i ] ) = *( ( WORD * )&Src[ i ] ) ;
				ParityData += Src[ i ] + Src[ i + 1 ] ;
				break ;

			case 4 :
				*( ( DWORD * )&Dest->FileName[ i ] ) = *( ( DWORD * )&Src[ i ] ) ;
				ParityData += Src[ i ] + Src[ i + 1 ] + Src[ i + 2 ] + Src[ i + 3 ] ;
				break ;

			default :
				for( j = 0 ; j < CharBytes ; j ++ )
				{
					Dest->FileName[ i + j ] = Src[ i + j ] ;
					ParityData += Src[ i + j ] ;
				}
				break ;
			}
		}

		i += CharBytes ;
	}

	// 文字列の長さを保存
	if( Length != NULL )
	{
		*Length = i ;
	}

	// ４の倍数の位置まで０を代入
	StringLength = ( ( i + 1 ) + 3 ) / 4 * 4 ;
	_MEMSET( &Dest->FileName[ i ], 0, ( size_t )( StringLength - i ) ) ;

	// パリティデータの保存
	Dest->Parity = ParityData ;

	// パックデータ数の保存
	Dest->PackNum = ( WORD )( StringLength / 4 ) ;

	// 正常終了
	return 0 ;
}







// 鍵を作成
static void DXA_KeyCreate( const char *Source, size_t SourceBytes, unsigned char *Key )
{
	char SourceTempBuffer[ 1024 ] ;
	char WorkBuffer[ 1024 ] ;
	char *UseWorkBuffer ;
	DWORD i, j ;
	DWORD CRC32_0 ;
	DWORD CRC32_1 ;

	if( SourceBytes == 0 )
	{
		SourceBytes = CL_strlen( DX_CHARCODEFORMAT_ASCII, Source ) ;
	}

	if( SourceBytes < 4 )
	{
		CL_strcpy( DX_CHARCODEFORMAT_ASCII, SourceTempBuffer, Source ) ;
		CL_strcpy( DX_CHARCODEFORMAT_ASCII, &SourceTempBuffer[ SourceBytes ], DefaultKeyString ) ;
		Source = SourceTempBuffer ;
		SourceBytes = CL_strlen( DX_CHARCODEFORMAT_ASCII, Source ) ;
	}

	if( SourceBytes / 2 > sizeof( WorkBuffer ) )
	{
		UseWorkBuffer = ( char * )DXALLOC( SourceBytes / 2 ) ;
	}
	else
	{
		UseWorkBuffer = WorkBuffer ;
	}

	j = 0 ;
	for( i = 0 ; i < SourceBytes ; i += 2, j++ )
	{
		UseWorkBuffer[ j ] = Source[ i ] ;
	}
	CRC32_0 = HashCRC32( UseWorkBuffer, j ) ;

	j = 0 ;
	for( i = 1 ; i < SourceBytes ; i += 2, j++ )
	{
		UseWorkBuffer[ j ] = Source[ i ] ;
	}
	CRC32_1 = HashCRC32( UseWorkBuffer, j ) ;

	Key[ 0 ] = ( BYTE )( CRC32_0 >>  0 ) ;
	Key[ 1 ] = ( BYTE )( CRC32_0 >>  8 ) ;
	Key[ 2 ] = ( BYTE )( CRC32_0 >> 16 ) ;
	Key[ 3 ] = ( BYTE )( CRC32_0 >> 24 ) ;
	Key[ 4 ] = ( BYTE )( CRC32_1 >>  0 ) ;
	Key[ 5 ] = ( BYTE )( CRC32_1 >>  8 ) ;
	Key[ 6 ] = ( BYTE )( CRC32_1 >> 16 ) ;

	if( SourceBytes > sizeof( WorkBuffer ) )
	{
		DXFREE( UseWorkBuffer ) ;
	}
}

// 鍵を使用して Xor 演算( Key は必ず DXA_KEY_BYTES の長さがなければならない )
static void DXA_KeyConv( void *Data, LONGLONG  SizeLL, LONGLONG  PositionLL,  unsigned char *Key )
{
	int Position ;

	Position = ( int )( PositionLL % DXA_KEY_BYTES ) ;

	if( SizeLL <= 0xffffffff )
	{
		DWORD SizeT ;
		SizeT = ( DWORD )SizeLL ;

		BYTE *DataBP ;
		BYTE *KeyBP ;

		DataBP = ( BYTE * )Data ;
		KeyBP = ( BYTE * )Key ;
		if( SizeT >= DXA_KEY_BYTES )
		{
			if( Position != 0 )
			{
				do
				{
					*DataBP ^= KeyBP[ Position ] ;
					Position ++ ;
					DataBP ++ ;
					SizeT -- ;
				}while( Position < DXA_KEY_BYTES ) ;
				Position = 0 ;
			}

			if( SizeT >= DXA_KEY_BYTES )
			{
				DWORD SetNum ;
				DWORD Key1 ;
				WORD Key2 ;
				BYTE Key3 ;

				SetNum = SizeT / DXA_KEY_BYTES ;
				SizeT -= SetNum * DXA_KEY_BYTES ;
				Key1 = ( ( DWORD * )KeyBP )[ 0 ] ;
				Key2 = ( (  WORD * )KeyBP )[ 2 ] ;
				Key3 = ( (  BYTE * )KeyBP )[ 6 ] ;

				do
				{
					( ( DWORD * )DataBP )[ 0 ] ^= Key1 ;
					( (  WORD * )DataBP )[ 2 ] ^= Key2 ;
					( (  BYTE * )DataBP )[ 6 ] ^= Key3 ;
					DataBP += DXA_KEY_BYTES ;
					SetNum -- ;
				}while( SetNum > 0 ) ;
			}
		}

		if( SizeT > 0 )
		{
			do
			{
				*DataBP ^= KeyBP[ Position ] ;
				DataBP ++ ;
				Position ++ ;
				if( Position == DXA_KEY_BYTES )
					Position = 0 ;
				SizeT -- ;
			}while( SizeT > 0 ) ;
		}
	}
	else
	{
		BYTE *DataBP ;
		BYTE *KeyBP ;

		DataBP = ( BYTE * )Data ;
		KeyBP = ( BYTE * )Key ;
		if( SizeLL >= DXA_KEY_BYTES )
		{
			if( Position != 0 )
			{
				do
				{
					*DataBP ^= KeyBP[ Position ] ;
					Position ++ ;
					DataBP ++ ;
					SizeLL -- ;
				}while( Position < DXA_KEY_BYTES ) ;
				Position = 0 ;
			}

			if( SizeLL >= DXA_KEY_BYTES )
			{
				DWORD SetNum ;
				DWORD Key1 ;
				WORD  Key2 ;
				BYTE  Key3 ;

				SetNum = ( DWORD )( SizeLL / DXA_KEY_BYTES ) ;
				SizeLL -= SetNum * DXA_KEY_BYTES ;
				Key1 = ( ( DWORD * )KeyBP )[ 0 ] ;
				Key2 = ( ( WORD  * )KeyBP )[ 2 ] ;
				Key3 = ( ( BYTE  * )KeyBP )[ 6 ] ;

				do
				{
					( ( DWORD * )DataBP )[ 0 ] ^= Key1 ;
					( ( WORD  * )DataBP )[ 2 ] ^= Key2 ;
					( ( BYTE  * )DataBP )[ 6 ] ^= Key3 ;
					DataBP += DXA_KEY_BYTES ;
					SetNum -- ;
				}while( SetNum > 0 ) ;
			}
		}

		if( SizeLL > 0 )
		{
			do
			{
				*DataBP ^= KeyBP[ Position ] ;
				DataBP ++ ;
				Position ++ ;
				if( Position == DXA_KEY_BYTES )
					Position = 0 ;
				SizeLL -- ;
			}while( SizeLL > 0 ) ;
		}
	}
}

// ファイルから読み込んだデータを鍵文字列を使用して Xor 演算する関数( Key は必ず DXA_KEYSTR_LENGTH の長さがなければならない )
void DXA_KeyConvFileRead( void *Data, ULONGLONG  Size, DWORD_PTR FilePointer, unsigned char *Key, LONGLONG Position )
{
	LONGLONG pos = 0 ;

	if( Key != NULL )
	{
		// ファイルの位置を取得しておく
		pos = Position == -1 ? ReadOnlyFileAccessTell( FilePointer ) : Position ;
	}

	// 読み込む
	ReadOnlyFileAccessRead( Data, ( size_t )Size, 1, FilePointer ) ;
	while( ReadOnlyFileAccessIdleCheck( FilePointer ) == FALSE )
	{
		Thread_Sleep( 1 ) ;
	}

	if( Key != NULL )
	{
		// データを鍵文字列を使って Xor 演算
		DXA_KeyConv( Data, ( LONGLONG )Size, pos, Key ) ;
	}
}

// 条件に適合するオブジェクトを検索する(検索対象は ObjectCount をインデックスとしたところから)(戻り値 -1:エラー 0:成功)
static int DXA_FindProcess( DXA_FINDDATA *FindData, FILEINFOW *FileInfo )
{
	BYTE  *nameTable ;
	DXARC *DXA ;
	BYTE  *str ;
	BYTE  *name ;
	DWORD fileHeadSize ;
	BYTE  TempName[ 2048 ] ;
	BYTE  DotStr[ 16 ] ;
	BYTE  DoubleDotStr[ 16 ] ;

	DXA       = FindData->Container;
	nameTable = DXA->Table.NameTable;
	str       = FindData->SearchStr;

	ConvString( ( const char * )Ascii_DotStr,       -1, DX_CHARCODEFORMAT_ASCII, ( char * )DotStr,       sizeof( DotStr       ), DXA->CharCodeFormat ) ;
	ConvString( ( const char * )Ascii_DoubleDotStr, -1, DX_CHARCODEFORMAT_ASCII, ( char * )DoubleDotStr, sizeof( DoubleDotStr ), DXA->CharCodeFormat ) ;

	{
		ULONGLONG i, num, addnum ;
		DXARC_DIRECTORY *dir ;
		DXARC_FILEHEAD *file ;

		dir          = FindData->Directory ;
		num          = dir->FileHeadNum ;
		addnum       = ( ULONGLONG )( dir->ParentDirectoryAddress == NONE_PAL ? 1 : 2 ) ;
		fileHeadSize = DXARC_FILEHEAD_VER8_SIZE ;

		if( FindData->ObjectCount == ( DWORD )( num + addnum ) )
		{
			return -1 ;
		}

		file = ( DXARC_FILEHEAD * )( DXA->Table.FileTable + dir->FileHeadAddress + fileHeadSize * FindData->ObjectCount ) ;
		for( i = FindData->ObjectCount; i < num + addnum; i ++ )
		{
			if( i >= num )
			{
				if( i - num == 0 )
				{
					if( DXA_DIR_FileNameCmp( DXA, DotStr,       str ) == 0 )
					{
						break ;
					}
				}
				else
				if( i - num == 1 )
				{
					if( DXA_DIR_FileNameCmp( DXA, DoubleDotStr, str ) == 0 )
					{
						break;
					}
				}
			}
			else
			{
				name = ( BYTE * )( nameTable + file->NameAddress + 4 ) ;
				if( DXA_DIR_FileNameCmp( DXA, name, str ) == 0 )
				{
					break ;
				}

				file = (DXARC_FILEHEAD *)( (BYTE *)file + fileHeadSize ) ;
			}
		}
		FindData->ObjectCount = ( DWORD )i ;
		if( i == num + addnum )
		{
			return -1 ;
		}

		if( FileInfo )
		{
			if( i >= num )
			{
				switch( i - num )
				{
				default :
				case 0:
					CL_strcpy_s( DXA->CharCodeFormat, ( char * )TempName, sizeof( TempName ), ( const char * )DotStr       ) ;
					break ;

				case 1 :
					CL_strcpy_s( DXA->CharCodeFormat, ( char * )TempName, sizeof( TempName ), ( const char * )DoubleDotStr ) ;
					break ;
				}
				FileInfo->DirFlag = 1 ;
				FileInfo->Size    = 0 ;
				_MEMSET( &FileInfo->CreationTime,  0, sizeof( FileInfo->CreationTime  ) ) ;
				_MEMSET( &FileInfo->LastWriteTime, 0, sizeof( FileInfo->LastWriteTime ) ) ;
			}
			else
			{
				name = ( BYTE * )( nameTable + file->NameAddress ) ;
				CL_strcpy_s( DXA->CharCodeFormat, ( char * )TempName, sizeof( TempName ), ( const char * )( name + ( ( WORD * )name )[ 0 ] * 4 + 4 ) ) ;
				FileInfo->DirFlag = ( file->Attributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 ? TRUE : FALSE ;
				FileInfo->Size    = ( LONGLONG)file->DataSize ;
#ifdef __WINDOWS__
				_FileTimeToLocalDateData( ( FILETIME * )&file->Time.Create,    &FileInfo->CreationTime  ) ;
				_FileTimeToLocalDateData( ( FILETIME * )&file->Time.LastWrite, &FileInfo->LastWriteTime ) ;
#else // __WINDOWS__
				_MEMSET( &FileInfo->CreationTime,  0, sizeof( FileInfo->CreationTime  ) ) ;
				_MEMSET( &FileInfo->LastWriteTime, 0, sizeof( FileInfo->LastWriteTime ) ) ;
#endif // __WINDOWS__
			}

			ConvString( ( const char * )TempName, -1, DXA->CharCodeFormat, ( char * )FileInfo->Name, sizeof( FileInfo->Name ), WCHAR_T_CHARCODEFORMAT ) ;
		}
	}

	return 0 ;
}



// アーカイブファイルを扱う為の構造体を初期化する
extern	int	DXA_Initialize( DXARC *DXA )
{
	_MEMSET( DXA, 0, sizeof( DXARC ) ) ;

	// 終了
	return 0 ;
}

// アーカイブファイルを扱う為の構造体の後始末をする
extern int DXA_Terminate( DXARC *DXA )
{
	DXA_CloseArchive( DXA ) ;

	_MEMSET( DXA, 0, sizeof( DXARC ) ) ;

	// 終了
	return 0 ;
}

// 指定のディレクトリデータの暗号化を解除する( 丸ごとメモリに読み込んだ場合用 )
static int DXA_DirectoryKeyConv( DXARC *DXA, DXARC_DIRECTORY *Dir, char *KeyStringBuffer )
{
	// メモリイメージではない場合はエラー
	if( DXA->MemoryOpenFlag == FALSE )
		return -1 ;

	// 鍵処理を行わない指定の場合は何もせずに終了
	if( DXA->NoKey )
		return -1 ;

	// 暗号化解除処理開始
	{
		DWORD i, FileHeadSize ;
		DXARC_FILEHEAD *File ;
		unsigned char Key[ DXA_KEY_BYTES ] ;
		size_t KeyStringBytes ;

		// 格納されているファイルの数だけ繰り返す
		FileHeadSize = DXARC_FILEHEAD_VER8_SIZE ;
		File = ( DXARC_FILEHEAD * )( DXA->Table.FileTable + Dir->FileHeadAddress ) ;
		for( i = 0 ; i < Dir->FileHeadNum ; i ++, File = ( DXARC_FILEHEAD * )( ( BYTE * )File + FileHeadSize ) )
		{
			// ディレクトリかどうかで処理を分岐
			if( File->Attributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				// ディレクトリの場合は再帰をかける
				DXA_DirectoryKeyConv( DXA, ( DXARC_DIRECTORY * )( DXA->Table.DirectoryTable + File->DataAddress ), KeyStringBuffer ) ;
			}
			else
			{
				BYTE *DataP ;

				// ファイルの場合は暗号化を解除する
				
				// データがある場合のみ処理
				if( File->DataSize != 0 )
				{
					// データ位置をセットする
					DataP = ( BYTE * )DXA->MemoryImage + DXA->Head.DataStartAddress + File->DataAddress ;

					// ファイル個別の鍵を作成
					KeyStringBytes = DXA_CreateKeyFileString( DXA, Dir, File, ( BYTE * )KeyStringBuffer ) ;
					DXA_KeyCreate( KeyStringBuffer, KeyStringBytes, Key ) ;

					// ハフマン圧縮されているかどうかで処理を分岐
					if( File->HuffPressDataSize != NONE_PAL )
					{
						// ハフマン圧縮されている場合

						// 通常のデータ圧縮もされているかどうかで処理を分岐
						if( File->PressDataSize != NONE_PAL )
						{
							// ファイルの前後のみハフマン圧縮されているかどうかで鍵を適用するサイズを分岐
							if( DXA->Head.HuffmanEncodeKB != 0xff && File->PressDataSize > DXA->Head.HuffmanEncodeKB * 1024 * 2 )
							{
								DXA_KeyConv( DataP, ( LONGLONG )( File->HuffPressDataSize + File->PressDataSize - DXA->Head.HuffmanEncodeKB * 1024 * 2 ), ( LONGLONG )File->DataSize, Key ) ;
							}
							else
							{
								DXA_KeyConv( DataP, ( LONGLONG )File->HuffPressDataSize, ( LONGLONG )File->DataSize, Key ) ;
							}
						}
						else
						{
							// ファイルの前後のみハフマン圧縮されているかどうかで鍵を適用するサイズを分岐
							if( DXA->Head.HuffmanEncodeKB != 0xff && File->DataSize > DXA->Head.HuffmanEncodeKB * 1024 * 2 )
							{
								DXA_KeyConv( DataP, ( LONGLONG )( File->HuffPressDataSize + File->DataSize - DXA->Head.HuffmanEncodeKB * 1024 * 2 ), ( LONGLONG )File->DataSize, Key ) ;
							}
							else
							{
								DXA_KeyConv( DataP, ( LONGLONG )File->HuffPressDataSize, ( LONGLONG )File->DataSize, Key ) ;
							}
						}
					}
					else
					// データが圧縮されているかどうかで処理を分岐
					if( File->PressDataSize != NONE_PAL )
					{
						// 圧縮されている場合
						DXA_KeyConv( DataP, ( LONGLONG )File->PressDataSize, ( LONGLONG )File->DataSize, Key ) ;
					}
					else
					{
						// 圧縮されていない場合
						DXA_KeyConv( DataP, ( LONGLONG )File->DataSize, ( LONGLONG )File->DataSize, Key ) ;
					}
				}
			}
		}
	}

	// 終了
	return 0 ;
}

// メモリ上にあるアーカイブファイルイメージを開く( 0:成功  -1:失敗 )
extern int DXA_OpenArchiveFromMem( DXARC *DXA, void *ArchiveImage, int ArchiveSize, int ArchiveImageCopyFlag, int ArchiveImageReadOnlyFlag, const char *KeyString, const wchar_t *EmulateArchivePath )
{
	// メモリイメージをコピーして使用する場合は読み取り専用にはしない
	if( ArchiveImageCopyFlag )
	{
		ArchiveImageReadOnlyFlag = FALSE ;
	}

	// 既になんらかのアーカイブを開いていた場合はエラー
	if( DXA->ReadAccessOnlyFilePointer != 0 || DXA->MemoryImage != NULL ) return -1 ;

	// 鍵文字列の保存と鍵の作成
	{
		// 指定が無い場合はデフォルトの鍵文字列を使用する
		if( KeyString == NULL )
		{
			KeyString = DefaultKeyString ;
		}

		DXA->KeyStringBytes = CL_strlen( DX_CHARCODEFORMAT_ASCII, KeyString ) ;
		if( DXA->KeyStringBytes > DXA_KEY_STRING_LENGTH )
		{
			DXA->KeyStringBytes = DXA_KEY_STRING_LENGTH ;
		}
		_MEMCPY( DXA->KeyString, KeyString, DXA->KeyStringBytes ) ;
		DXA->KeyString[ DXA->KeyStringBytes ] = '\0' ;

		// 鍵の作成
		DXA_KeyCreate( DXA->KeyString, DXA->KeyStringBytes, DXA->Key ) ;
	}

	// ファイルパスを保存
	if( EmulateArchivePath != NULL )
	{
		_WCSCPY_S( DXA->FilePath, sizeof( DXA->FilePath ), EmulateArchivePath ) ;
	}
	else
	{
		_MEMSET( DXA->FilePath, 0, sizeof( DXA->FilePath ) ) ;
	}

	DXA->Table.Top       = NULL ;
	DXA->MemoryImage     = NULL ;
	DXA->MemoryImageSize = ArchiveSize ;
	if( ArchiveImageCopyFlag )
	{
		// イメージをコピーするフラグが立っている場合はコピー先のメモリ領域を確保
		DXA->MemoryImage = DXALLOC( ( size_t )ArchiveSize ) ;
		if( DXA->MemoryImage == NULL )
		{
			return -1 ;
		}

		// アーカイブファイルイメージの内容を確保したメモリにコピー
		_MEMCPY( DXA->MemoryImage, ArchiveImage, ( size_t )ArchiveSize ) ;

		// コピーしたメモリイメージを使用する
		DXA->MemoryImageOriginal = ArchiveImage ;
		ArchiveImage             = DXA->MemoryImage ;
	}
	else
	{
		// ポインタを保存
		DXA->MemoryImage         = ArchiveImage ;
		DXA->MemoryImageOriginal = NULL ;
	}

	// 最初にＩＤとバージョンをコピー
	_MEMCPY( &DXA->Head, ArchiveImage, DXARC_ID_AND_VERSION_SIZE ) ;

	// ＩＤが違う場合はエラー
	if( DXA->Head.Head != DXAHEAD )
	{
		goto ERR ;
	}

	// 未対応バージョンの場合はエラー
	if( DXA->Head.Version > DXAVER || DXA->Head.Version < DXAVER_MIN )
	{
		goto ERR ;
	}

	// ヘッダ部分をコピー
	_MEMCPY( &DXA->Head, ArchiveImage, DXARC_HEAD_VER8_SIZE ) ;

	DXA->Head.CharCodeFormat = 0 ;

	// ヘッダを解析
	{
		// 鍵処理が行われていないかを取得する
		DXA->NoKey = ( DXA->Head.Flags & DXA_FLAG_NO_KEY ) != 0 ;

		// 文字コード形式をセット
		switch( DXA->Head.CharCodeFormat )
		{
		case DX_CHARCODEFORMAT_UHC :
		case DX_CHARCODEFORMAT_BIG5 :
		case DX_CHARCODEFORMAT_GB2312 :
		case DX_CHARCODEFORMAT_SHIFTJIS :
		case DX_CHARCODEFORMAT_UTF16LE :
		case DX_CHARCODEFORMAT_UTF16BE :
		case DX_CHARCODEFORMAT_WINDOWS_1252 :
		case DX_CHARCODEFORMAT_ISO_IEC_8859_15 :
		case DX_CHARCODEFORMAT_UTF8 :
		case DX_CHARCODEFORMAT_UTF32LE :
		case DX_CHARCODEFORMAT_UTF32BE :
			DXA->CharCodeFormat = ( int )DXA->Head.CharCodeFormat ;
			break ;

		default :
			DXA->CharCodeFormat = DX_CHARCODEFORMAT_SHIFTJIS ;
			break ;
		}

		// 情報テーブルのサイズ分のメモリを確保する
		DXA->Table.Top = ( BYTE * )DXALLOC( ( size_t )DXA->Head.HeadSize ) ;
		if( DXA->Table.Top == NULL )
		{
			goto ERR ;
		}

		// ヘッダが圧縮されている場合は解凍する
		if( ( DXA->Head.Flags & DXA_FLAG_NO_HEAD_PRESS ) != 0 )
		{
			// 圧縮されていない場合は普通に読み込む
			_MEMCPY( DXA->Table.Top, ( BYTE * )DXA->MemoryImage + DXA->Head.FileNameTableStartAddress, DXA->Head.HeadSize ) ;
			if( DXA->NoKey == false ) DXA_KeyConv( DXA->Table.Top, DXA->Head.HeadSize, 0, DXA->Key ) ;
		}
		else
		{
			void *HuffHeadBuffer ;
			ULONGLONG HuffHeadSize ;
			void *LzHeadBuffer ;
			ULONGLONG LzHeadSize ;

			// ハフマン圧縮されたヘッダの容量を取得する
			HuffHeadSize = ( DWORD )( ( ULONGLONG )DXA->MemoryImageSize - DXA->Head.FileNameTableStartAddress ) ;

			// ハフマン圧縮されたヘッダを読み込むメモリを確保する
			HuffHeadBuffer = DXALLOC( ( size_t )HuffHeadSize ) ;
			if( HuffHeadBuffer == NULL )
			{
				goto ERR ;
			}

			// 圧縮されたヘッダをコピーと暗号化解除
			_MEMCPY( HuffHeadBuffer, ( BYTE * )DXA->MemoryImage + DXA->Head.FileNameTableStartAddress, ( size_t )HuffHeadSize ) ;
			if( DXA->NoKey == false ) DXA_KeyConv( HuffHeadBuffer, HuffHeadSize, 0, DXA->Key ) ;

			// ハフマン圧縮されたヘッダの解凍後の容量を取得する
			LzHeadSize = Huffman_Decode( HuffHeadBuffer, NULL ) ;

			// ハフマン圧縮されたヘッダの解凍後のデータを格納するメモリ用域の確保
			LzHeadBuffer = DXALLOC( ( size_t )LzHeadSize ) ;
			if( LzHeadBuffer == NULL )
			{
				DXFREE( HuffHeadBuffer ) ;
				goto ERR ;
			}

			// 圧縮されたヘッダを解凍する
			Huffman_Decode( HuffHeadBuffer, LzHeadBuffer ) ;

			// LZ圧縮されたヘッダを解凍する
			DXA_Decode( LzHeadBuffer, DXA->Table.Top ) ;

			// メモリの解放
			DXFREE( HuffHeadBuffer ) ;
			DXFREE( LzHeadBuffer ) ;
		}

		// 各情報テーブルのアドレスをセットする
		DXA->Table.NameTable		= DXA->Table.Top ;
		DXA->Table.FileTable		= DXA->Table.NameTable + DXA->Head.FileTableStartAddress ;
		DXA->Table.DirectoryTable	= DXA->Table.NameTable + DXA->Head.DirectoryTableStartAddress ;

		// カレントディレクトリのセット
		DXA->CurrentDirectory = ( DXARC_DIRECTORY * )DXA->Table.DirectoryTable ;
	}

	DXA->MemoryOpenFlag					= TRUE ;						// メモリイメージから開いているフラグを立てる
	DXA->UserMemoryImageFlag			= TRUE ;						// ユーザーのイメージから開いたフラグを立てる
	DXA->MemoryImageCopyFlag			= ArchiveImageCopyFlag ;		// メモリイメージをコピーして使用しているかどうかのフラグを保存
	DXA->MemoryImageReadOnlyFlag        = ArchiveImageReadOnlyFlag ;	// メモリイメージを読み取り専用にするかどうかのフラグを保存

	// 全てのファイルデータの暗号化を解除する
	if( DXA->NoKey == false && ArchiveImageReadOnlyFlag == FALSE )
	{
		char KeyStringBuffer[ DXA_KEY_STRING_MAXLENGTH ] ;
		DXA_DirectoryKeyConv( DXA, ( DXARC_DIRECTORY * )DXA->Table.DirectoryTable, KeyStringBuffer ) ;
	}

	// 終了
	return 0 ;

ERR :
	if( ArchiveImageCopyFlag )
	{
		if( DXA->MemoryImage != NULL )
		{
			DXFREE( DXA->MemoryImage ) ;
			DXA->MemoryImage = NULL ;
		}
	}
	else
	if( DXA->Table.Top != NULL )
	{
		DXFREE( DXA->Table.Top ) ;
		DXA->Table.Top = NULL ;
	}
	
	// 終了
	return -1 ;
}

// アーカイブファイルを扱う準備が整ったかを得る( TRUE:整っている  FALSE:整っていない )
extern int DXA_CheckIdle( DXARC *DXA )
{
	// 非同期オープン中ではなければ特にやることはない
	if( DXA->ASyncOpenFlag == FALSE )
	{
		return TRUE ;
	}

	// ファイル読み込みが完了しているか調べる
	if( ReadOnlyFileAccessIdleCheck( DXA->ASyncOpenFilePointer ) == FALSE )
	{
		return FALSE ;
	}

	// ファイルを閉じる
	ReadOnlyFileAccessClose( DXA->ASyncOpenFilePointer ) ;
	DXA->ASyncOpenFilePointer = 0;

	// すべてのデータを反転する
	if( DXA->NoKey == false )
	{
		char KeyStringBuffer[ DXA_KEY_STRING_MAXLENGTH ] ;
		DXA_DirectoryKeyConv( DXA, ( DXARC_DIRECTORY * )DXA->Table.DirectoryTable, KeyStringBuffer ) ;
	}

	// 非同期オープン中フラグを倒す
	DXA->ASyncOpenFlag = FALSE ;

	// 終了
	return TRUE ;
}

// アーカイブファイルを開き最初にすべてメモリ上に読み込んでから処理する( 0:成功  -1:失敗 )
extern int DXA_OpenArchiveFromFileUseMem( DXARC *DXA, const wchar_t *ArchivePath, const char *KeyString , int ASyncThread )
{
	// 既になんらかのアーカイブを開いていた場合はエラー
	if( DXA->ReadAccessOnlyFilePointer != 0 || DXA->MemoryImage )
	{
		return -1 ;
	}

	// 鍵文字列の保存と鍵の作成
	{
		// 指定が無い場合はデフォルトの鍵文字列を使用する
		if( KeyString == NULL )
		{
			KeyString = DefaultKeyString ;
		}

		DXA->KeyStringBytes = CL_strlen( DX_CHARCODEFORMAT_ASCII, KeyString ) ;
		if( DXA->KeyStringBytes > DXA_KEY_STRING_LENGTH )
		{
			DXA->KeyStringBytes = DXA_KEY_STRING_LENGTH ;
		}
		_MEMCPY( DXA->KeyString, KeyString, DXA->KeyStringBytes ) ;
		DXA->KeyString[ DXA->KeyStringBytes ] = '\0' ;

		// 鍵の作成
		DXA_KeyCreate( DXA->KeyString, DXA->KeyStringBytes, DXA->Key ) ;
	}

	// ファイルを開く
	DXA->ASyncOpenFilePointer = 0 ;
	DXA->MemoryImage          = NULL ;
	DXA->ASyncOpenFilePointer = ReadOnlyFileAccessOpen( ArchivePath, FALSE, TRUE, FALSE ) ;
	if( DXA->ASyncOpenFilePointer == 0 )
	{
		return -1 ;
	}

	// ファイルパスを保存
	_WCSCPY_S( DXA->FilePath, sizeof( DXA->FilePath ), ArchivePath ) ;

	// ファイルのサイズを取得する
	ReadOnlyFileAccessSeek( DXA->ASyncOpenFilePointer, 0L, SEEK_END ) ;
	DXA->MemoryImageSize = ReadOnlyFileAccessTell( DXA->ASyncOpenFilePointer ) ;
	ReadOnlyFileAccessSeek( DXA->ASyncOpenFilePointer, 0L, SEEK_SET ) ;

	// ファイルの内容を全てメモリに読み込む為のメモリ領域の確保
	DXA->MemoryImage = DXALLOC( ( size_t )DXA->MemoryImageSize ) ;

	// ＩＤとバージョン番号部分だけを読み込み
	ReadOnlyFileAccessRead( &DXA->Head, DXARC_ID_AND_VERSION_SIZE, 1, DXA->ASyncOpenFilePointer ) ;

	// ＩＤが違う場合はエラー
	if( DXA->Head.Head != DXAHEAD )
	{
		goto ERR ;
	}

	// 未対応バージョンの場合はエラー
	if( DXA->Head.Version > DXAVER || DXA->Head.Version < DXAVER_MIN )
	{
		goto ERR ;
	}

	// ファイルを丸ごと読み込む
	ReadOnlyFileAccessSeek( DXA->ASyncOpenFilePointer, 0L, SEEK_SET ) ;
	ReadOnlyFileAccessRead( DXA->MemoryImage, ( size_t )DXA->MemoryImageSize, 1, DXA->ASyncOpenFilePointer );

	// ヘッダ部分をコピー
	_MEMCPY( &DXA->Head, DXA->MemoryImage, DXARC_HEAD_VER8_SIZE ) ;

	// ヘッダを解析
	{
		// 鍵処理が行われていないかを取得する
		DXA->NoKey = ( DXA->Head.Flags & DXA_FLAG_NO_KEY ) != 0 ;

		// 文字コード形式をセット
		switch( DXA->Head.CharCodeFormat )
		{
		case DX_CHARCODEFORMAT_UHC :
		case DX_CHARCODEFORMAT_BIG5 :
		case DX_CHARCODEFORMAT_GB2312 :
		case DX_CHARCODEFORMAT_SHIFTJIS :
		case DX_CHARCODEFORMAT_UTF16LE :
		case DX_CHARCODEFORMAT_UTF16BE :
		case DX_CHARCODEFORMAT_WINDOWS_1252 :
		case DX_CHARCODEFORMAT_ISO_IEC_8859_15 :
		case DX_CHARCODEFORMAT_UTF8 :
		case DX_CHARCODEFORMAT_UTF32LE :
		case DX_CHARCODEFORMAT_UTF32BE :
			DXA->CharCodeFormat = ( int )DXA->Head.CharCodeFormat ;
			break ;

		default :
			DXA->CharCodeFormat = DX_CHARCODEFORMAT_SHIFTJIS ;
			break ;
		}
		
		// 情報テーブルのサイズ分のメモリを確保する
		DXA->Table.Top = ( BYTE * )DXALLOC( DXA->Head.HeadSize ) ;
		if( DXA->Table.Top == NULL )
		{
			goto ERR ;
		}
		
		// ヘッダが圧縮されている場合は解凍する
		if( ( DXA->Head.Flags & DXA_FLAG_NO_HEAD_PRESS ) != 0 )
		{
			// 圧縮されていない場合は普通に読み込む
			_MEMCPY( DXA->Table.Top, ( BYTE * )DXA->MemoryImage + DXA->Head.FileNameTableStartAddress, DXA->Head.HeadSize ) ;
			if( DXA->NoKey == false ) DXA_KeyConv( DXA->Table.Top, DXA->Head.HeadSize, 0, DXA->Key ) ;
		}
		else
		{
			void *HuffHeadBuffer ;
			ULONGLONG HuffHeadSize ;
			void *LzHeadBuffer ;
			ULONGLONG LzHeadSize ;

			// ハフマン圧縮されたヘッダの容量を取得する
			HuffHeadSize = ( DWORD )( ( ULONGLONG )DXA->MemoryImageSize - DXA->Head.FileNameTableStartAddress ) ;

			// ハフマン圧縮されたヘッダを読み込むメモリを確保する
			HuffHeadBuffer = DXALLOC( ( size_t )HuffHeadSize ) ;
			if( HuffHeadBuffer == NULL )
			{
				goto ERR ;
			}

			// 圧縮されたヘッダをコピーと暗号化解除
			_MEMCPY( HuffHeadBuffer, ( BYTE * )DXA->MemoryImage + DXA->Head.FileNameTableStartAddress, ( size_t )HuffHeadSize ) ;
			if( DXA->NoKey == false ) DXA_KeyConv( HuffHeadBuffer, HuffHeadSize, 0, DXA->Key ) ;

			// ハフマン圧縮されたヘッダの解凍後の容量を取得する
			LzHeadSize = Huffman_Decode( HuffHeadBuffer, NULL ) ;

			// ハフマン圧縮されたヘッダの解凍後のデータを格納するメモリ用域の確保
			LzHeadBuffer = DXALLOC( ( size_t )LzHeadSize ) ;
			if( LzHeadBuffer == NULL )
			{
				DXFREE( HuffHeadBuffer ) ;
				goto ERR ;
			}

			// 圧縮されたヘッダを解凍する
			Huffman_Decode( HuffHeadBuffer, LzHeadBuffer ) ;

			// LZ圧縮されたヘッダを解凍する
			DXA_Decode( LzHeadBuffer, DXA->Table.Top ) ;

			// メモリの解放
			DXFREE( HuffHeadBuffer ) ;
			DXFREE( LzHeadBuffer ) ;
		}

		// 各情報テーブルのアドレスをセットする
		DXA->Table.NameTable		= DXA->Table.Top ;
		DXA->Table.FileTable		= DXA->Table.NameTable + DXA->Head.FileTableStartAddress ;
		DXA->Table.DirectoryTable	= DXA->Table.NameTable + DXA->Head.DirectoryTableStartAddress ;

		// カレントディレクトリのセット
		DXA->CurrentDirectory = ( DXARC_DIRECTORY * )DXA->Table.DirectoryTable ;
	}

	// ファイル非同期オープン中だということをセットしておく
	DXA->ASyncOpenFlag = TRUE ;

	DXA->MemoryOpenFlag					= TRUE ;			// メモリイメージから開いているフラグを立てる
	DXA->UserMemoryImageFlag			= FALSE ;			// ユーザーのイメージから開いたわけではないのでフラグを倒す
	DXA->MemoryImageCopyFlag			= FALSE ;			// メモリイメージをコピーして使用しているフラグを倒す
	DXA->MemoryImageReadOnlyFlag        = FALSE ;			// メモリイメージを読み取り専用にするかどうかのフラグを倒す

	// 同期オープンの場合はここで開き終わるのを待つ
	if( ASyncThread == FALSE )
	{
		while( DXA_CheckIdle( DXA ) == FALSE )
		{
			Thread_Sleep( 0 ) ;
		}
	}

	// 終了
	return 0 ;

ERR :
	if( DXA->ASyncOpenFilePointer )
	{
		ReadOnlyFileAccessClose( DXA->ASyncOpenFilePointer );
		DXA->ASyncOpenFilePointer = 0;
	}

	if( DXA->MemoryImage )
	{
		DXFREE( DXA->MemoryImage );
		DXA->MemoryImage = 0;
	}

	DXA->ASyncOpenFlag = FALSE ;

	// 終了
	return -1 ;
}

// アーカイブファイルを開く( 0:成功  -1:失敗 )
extern int DXA_OpenArchiveFromFile( DXARC *DXA, const wchar_t *ArchivePath, const char *KeyString )
{
	// 既になんらかのアーカイブを開いていた場合はエラー
	if( DXA->ReadAccessOnlyFilePointer != 0 || DXA->MemoryImage )
	{
		return -1 ;
	}

	// ヘッダの初期化
	_MEMSET( &DXA->Head, 0, sizeof( DXA->Head ) ) ;
	
	// アーカイブファイルを開こうと試みる
	DXA->ReadAccessOnlyFilePointer = ReadOnlyFileAccessOpen( ArchivePath, FALSE, TRUE, FALSE ) ;
	if( DXA->ReadAccessOnlyFilePointer == 0 )
	{
		return -1 ;
	}

	// ファイルパスを保存
	_WCSCPY_S( DXA->FilePath, sizeof( DXA->FilePath ), ArchivePath ) ;

	// 鍵文字列の保存と鍵の作成
	{
		// 指定が無い場合はデフォルトの鍵文字列を使用する
		if( KeyString == NULL )
		{
			KeyString = DefaultKeyString ;
		}

		DXA->KeyStringBytes = CL_strlen( DX_CHARCODEFORMAT_ASCII, KeyString ) ;
		if( DXA->KeyStringBytes > DXA_KEY_STRING_LENGTH )
		{
			DXA->KeyStringBytes = DXA_KEY_STRING_LENGTH ;
		}
		_MEMCPY( DXA->KeyString, KeyString, DXA->KeyStringBytes ) ;
		DXA->KeyString[ DXA->KeyStringBytes ] = '\0' ;

		// 鍵の作成
		DXA_KeyCreate( DXA->KeyString, DXA->KeyStringBytes, DXA->Key ) ;
	}

	// ＩＤとバージョン番号部分だけを読み込み
	ReadOnlyFileAccessRead( &DXA->Head, DXARC_ID_AND_VERSION_SIZE, 1, DXA->ReadAccessOnlyFilePointer ) ;

	// ＩＤが違う場合はエラー
	if( DXA->Head.Head != DXAHEAD )
	{
		goto ERR ;
	}

	// 未対応バージョンの場合はエラー
	if( DXA->Head.Version > DXAVER || DXA->Head.Version < DXAVER_MIN )
	{
		goto ERR ;
	}

	// ヘッダの残りの部分を読み込み
	ReadOnlyFileAccessRead( ( BYTE * )&DXA->Head + DXARC_ID_AND_VERSION_SIZE, DXARC_HEAD_VER8_SIZE - DXARC_ID_AND_VERSION_SIZE, 1, DXA->ReadAccessOnlyFilePointer ) ;

	// ヘッダを解析
	{
		// 鍵処理が行われていないかを取得する
		DXA->NoKey = ( DXA->Head.Flags & DXA_FLAG_NO_KEY ) != 0 ;

		// 文字コード形式をセット
		switch( DXA->Head.CharCodeFormat )
		{
		case DX_CHARCODEFORMAT_UHC :
		case DX_CHARCODEFORMAT_BIG5 :
		case DX_CHARCODEFORMAT_GB2312 :
		case DX_CHARCODEFORMAT_SHIFTJIS :
		case DX_CHARCODEFORMAT_UTF16LE :
		case DX_CHARCODEFORMAT_UTF16BE :
		case DX_CHARCODEFORMAT_WINDOWS_1252 :
		case DX_CHARCODEFORMAT_ISO_IEC_8859_15 :
		case DX_CHARCODEFORMAT_UTF8 :
		case DX_CHARCODEFORMAT_UTF32LE :
		case DX_CHARCODEFORMAT_UTF32BE :
			DXA->CharCodeFormat = ( int )DXA->Head.CharCodeFormat ;
			break ;

		default :
			DXA->CharCodeFormat = DX_CHARCODEFORMAT_SHIFTJIS ;
			break ;
		}

		// 情報テーブルのサイズ分のメモリを確保する
		DXA->Table.Top = ( BYTE * )DXALLOC( DXA->Head.HeadSize ) ;
		if( DXA->Table.Top == NULL )
		{
			goto ERR ;
		}
		
		// ヘッダが圧縮されている場合は解凍する
		if( ( DXA->Head.Flags & DXA_FLAG_NO_HEAD_PRESS ) != 0 )
		{
			// 圧縮されていない場合は普通に読み込む
			ReadOnlyFileAccessSeek( DXA->ReadAccessOnlyFilePointer, DXA->Head.FileNameTableStartAddress, SEEK_SET ) ;
			DXA_KeyConvFileRead( DXA->Table.Top, DXA->Head.HeadSize, DXA->ReadAccessOnlyFilePointer, DXA->NoKey ? NULL : DXA->Key, 0 ) ;
		}
		else
		{
			void *HuffHeadBuffer ;
			ULONGLONG HuffHeadSize ;
			void *LzHeadBuffer ;
			ULONGLONG LzHeadSize ;
			LONGLONG FileSize ;

			// 圧縮されたヘッダの容量を取得する
			ReadOnlyFileAccessSeek( DXA->ReadAccessOnlyFilePointer, 0, SEEK_END ) ;
			FileSize = ReadOnlyFileAccessTell( DXA->ReadAccessOnlyFilePointer ) ;
			ReadOnlyFileAccessSeek( DXA->ReadAccessOnlyFilePointer, DXA->Head.FileNameTableStartAddress, SEEK_SET ) ;
			HuffHeadSize = ( ULONGLONG )( FileSize - ReadOnlyFileAccessTell( DXA->ReadAccessOnlyFilePointer ) ) ;

			// 圧縮されたヘッダを読み込むメモリを確保する
			HuffHeadBuffer = DXALLOC( ( size_t )HuffHeadSize ) ;
			if( HuffHeadBuffer == NULL )
			{
				goto ERR ;
			}

			// 圧縮されたヘッダをメモリに読み込む
			DXA_KeyConvFileRead( HuffHeadBuffer, HuffHeadSize, DXA->ReadAccessOnlyFilePointer, DXA->NoKey ? NULL : DXA->Key, 0 ) ;

			// ハフマン圧縮されたヘッダの解凍後の容量を取得する
			LzHeadSize = Huffman_Decode( HuffHeadBuffer, NULL ) ;

			// ハフマン圧縮されたヘッダの解凍後のデータを格納するメモリ用域の確保
			LzHeadBuffer = DXALLOC( ( size_t )LzHeadSize ) ;
			if( LzHeadBuffer == NULL )
			{
				DXFREE( HuffHeadBuffer ) ;
				goto ERR ;
			}

			// 圧縮されたヘッダを解凍する
			Huffman_Decode( HuffHeadBuffer, LzHeadBuffer ) ;

			// LZ圧縮されたヘッダを解凍する
			DXA_Decode( LzHeadBuffer, DXA->Table.Top ) ;

			// メモリの解放
			DXFREE( HuffHeadBuffer ) ;
			DXFREE( LzHeadBuffer ) ;
		}

		// 各情報テーブルのアドレスをセットする
		DXA->Table.NameTable		= DXA->Table.Top ;
		DXA->Table.FileTable		= DXA->Table.NameTable + DXA->Head.FileTableStartAddress ;
		DXA->Table.DirectoryTable	= DXA->Table.NameTable + DXA->Head.DirectoryTableStartAddress ;

		// カレントディレクトリのセット
		DXA->CurrentDirectory = ( DXARC_DIRECTORY * )DXA->Table.DirectoryTable ;
	}

	DXA->MemoryOpenFlag					= FALSE ;			// メモリイメージから開いているフラグを倒す
	DXA->UserMemoryImageFlag			= FALSE ;			// ユーザーのイメージから開いたわけではないのでフラグを倒す
	DXA->MemoryImageCopyFlag			= FALSE ;			// メモリイメージをコピーして使用しているフラグを倒す
	DXA->MemoryImageReadOnlyFlag        = FALSE ;			// メモリイメージを読み取り専用にするかどうかのフラグを倒す

	// 終了
	return 0 ;

ERR :
	if( DXA->ReadAccessOnlyFilePointer != 0 )
	{
		ReadOnlyFileAccessClose( DXA->ReadAccessOnlyFilePointer ) ;
		DXA->ReadAccessOnlyFilePointer = 0 ;
	}

	if( DXA->Table.Top != NULL )
	{
		DXFREE( DXA->Table.Top ) ;
		DXA->Table.Top = NULL ;
	}
	
	// 終了
	return -1 ;
}

// アーカイブファイルを閉じる
extern int DXA_CloseArchive( DXARC *DXA )
{
	// 既に閉じていたら何もせず終了
	if( DXA->ReadAccessOnlyFilePointer == 0 && DXA->MemoryImage == NULL )
	{
		return 0 ;
	}

	// 非同期同期オープン中の場合はここで開き終わるのを待つ
	if( DXA->ASyncOpenFlag == TRUE )
	{
		while( DXA_CheckIdle( DXA ) == FALSE )
		{
			Thread_Sleep( 0 ) ;
		}
	}

	// 情報テーブル用に確保していたメモリを解放
	if( DXA->Table.Top != NULL )
	{
		DXFREE( DXA->Table.Top ) ;
		DXA->Table.Top = NULL ;
	}

	// メモリから開いているかどうかで処理を分岐
	if( DXA->MemoryOpenFlag == TRUE )
	{
		// アーカイブプログラムがメモリに読み込んだ場合とそうでない場合で処理を分岐
		if( DXA->UserMemoryImageFlag == TRUE )
		{
			// ユーザーから渡されたデータだった場合

			// アーカイブイメージをコピーして使用していた場合は単純に解放する
			if( DXA->MemoryImageCopyFlag )
			{
				if( DXA->MemoryImage != NULL )
				{
					DXFREE( DXA->MemoryImage ) ;
					DXA->MemoryImage = NULL ;
				}
			}
			else
			// 渡されたメモリアドレス先の内容を直接使用していた場合は反転したデータを元に戻す
			if( DXA->MemoryImageReadOnlyFlag == FALSE )
			{
				if( DXA->NoKey == false )
				{
					char KeyStringBuffer[ DXA_KEY_STRING_MAXLENGTH ] ;
					DXA_DirectoryKeyConv( DXA, ( DXARC_DIRECTORY * )DXA->Table.DirectoryTable, KeyStringBuffer ) ;
				}
			}
		}
		else
		{
			// アーカイブプログラムがメモリに読み込んだ場合

			// 確保していたメモリを開放する
			DXFREE( DXA->MemoryImage ) ;
		}
	}
	else
	{
		// アーカイブファイルを閉じる
		ReadOnlyFileAccessClose( DXA->ReadAccessOnlyFilePointer ) ;
	}

	// 初期化
	_MEMSET( DXA, 0, sizeof( DXARC ) ) ;

	// 終了
	return 0 ;
}

// アーカイブ内のディレクトリパスを変更する( 0:成功  -1:失敗 )
static int DXA_ChangeCurrentDirectoryFast( DXARC *DXA, DXARC_SEARCHDATA *SearchData )
{
	int i, j, k, Num ;
	BYTE *NameData, *PathData ;
	WORD PackNum, Parity ;
	DWORD FileHeadSize ;

	// 非同期同期オープン中の場合はここで開き終わるのを待つ
	if( DXA->ASyncOpenFlag == TRUE )
	{
		while( DXA_CheckIdle( DXA ) == FALSE )
		{
			Thread_Sleep( 0 ) ;
		}
	}

	PackNum  = SearchData->PackNum ;
	Parity   = SearchData->Parity ;
	PathData = SearchData->FileName ;

	// カレントディレクトリから同名のディレクトリを探す
	{
		DXARC_FILEHEAD *FileH ;

		FileH        = ( DXARC_FILEHEAD * )( DXA->Table.FileTable + DXA->CurrentDirectory->FileHeadAddress ) ;
		Num          = ( int )DXA->CurrentDirectory->FileHeadNum ;
		FileHeadSize = DXARC_FILEHEAD_VER8_SIZE ;
		for( i = 0 ; i < Num ; i ++, FileH = (DXARC_FILEHEAD *)( (BYTE *)FileH + FileHeadSize ) )
		{
			// ディレクトリチェック
			if( ( FileH->Attributes & FILE_ATTRIBUTE_DIRECTORY ) == 0 )
			{
				continue ;
			}

			// 文字列数とパリティチェック
			NameData = DXA->Table.NameTable + FileH->NameAddress ;
			if( PackNum != ( ( WORD * )NameData )[ 0 ] ||
				Parity  != ( ( WORD * )NameData )[ 1 ] )
			{
				continue ;
			}

			// 文字列チェック
			NameData += 4 ;
			for( j = 0, k = 0 ; j < PackNum ; j ++, k += 4 )
			{
				if( *( ( DWORD * )&PathData[ k ] ) != *( ( DWORD * )&NameData[ k ] ) )
				{
					break ;
				}
			}

			// 適合したディレクトリがあったらここで終了
			if( PackNum == j )
			{
				break ;
			}
		}

		// 無かったらエラー
		if( i == Num )
		{
			return -1 ;
		}

		// 在ったらカレントディレクトリを変更
		DXA->CurrentDirectory = ( DXARC_DIRECTORY * )( DXA->Table.DirectoryTable + FileH->DataAddress ) ;
	}

	// 正常終了
	return 0 ;
}

// アーカイブ内のディレクトリパスを変更する( 0:成功  -1:失敗 )
extern int DXA_ChangeCurrentDir( DXARC *DXA, int CharCodeFormat, const char *DirPath )
{
	BYTE TempBuffer[ 4096 ] ;
	const BYTE *DirPathB ;

	// 文字コード形式が異なる場合は変換する
	if( CharCodeFormat != DXA->CharCodeFormat )
	{
		ConvString( DirPath, -1, CharCodeFormat, ( char * )TempBuffer, sizeof( TempBuffer ), DXA->CharCodeFormat ) ;
		DirPathB = TempBuffer ;
	}
	else
	{
		DirPathB = ( const BYTE * )DirPath ;
	}

	return DXA_ChangeCurrentDirectoryBase( DXA, DirPathB, true ) ;
}

// アーカイブ内のディレクトリパスを変更する( 0:成功  -1:失敗 )
static int DXA_ChangeCurrentDirectoryBase( DXARC *DXA, const BYTE *DirectoryPath, bool ErrorIsDirectoryReset, DXARC_SEARCHDATA *LastSearchData )
{
	DXARC_DIRECTORY *OldDir ;
	DXARC_SEARCHDATA SearchData ;

	// 非同期同期オープン中の場合はここで開き終わるのを待つ
	if( DXA->ASyncOpenFlag == TRUE )
	{
		while( DXA_CheckIdle( DXA ) == FALSE )
		{
			Thread_Sleep( 0 ) ;
		}
	}

	// ここに留まるパスだったら無視
	if( CL_strcmp_str2_ascii( DXA->CharCodeFormat, ( const char * )DirectoryPath, ( const char * )Ascii_DotStr ) == 0 )
	{
		return 0 ;
	}

	// 『\ or /』だけの場合はルートディレクトリに戻る
	if( CL_strcmp_str2_ascii( DXA->CharCodeFormat, ( const char * )DirectoryPath, ( const char * )Ascii_EnStr    ) == 0 ||
		CL_strcmp_str2_ascii( DXA->CharCodeFormat, ( const char * )DirectoryPath, ( const char * )Ascii_SlashStr ) == 0 )
	{
		DXA->CurrentDirectory   = ( DXARC_DIRECTORY      * )DXA->Table.DirectoryTable ;
		return 0 ;
	}

	// 下に一つ下がるパスだったら処理を分岐
	if( CL_strcmp_str2_ascii( DXA->CharCodeFormat, ( const char * )DirectoryPath, ( const char * )Ascii_DoubleDotStr ) == 0 )
	{
		// ルートディレクトリに居たらエラー
		if( DXA->CurrentDirectory->ParentDirectoryAddress == NONE_PAL ) return -1 ;
			
		// 親ディレクトリがあったらそちらに移る
		DXA->CurrentDirectory   = ( DXARC_DIRECTORY      * )( DXA->Table.DirectoryTable + DXA->CurrentDirectory->ParentDirectoryAddress ) ;
		return 0 ;
	}

	// それ以外の場合は指定の名前のディレクトリを探す
	
	// 変更以前のディレクトリを保存しておく
	OldDir = DXA->CurrentDirectory ;

	// パス中に『\』があるかどうかで処理を分岐
	if( CL_strchr( DXA->CharCodeFormat, ( const char * )DirectoryPath, '\\' ) == NULL &&
		CL_strchr( DXA->CharCodeFormat, ( const char * )DirectoryPath, '/'  ) == NULL )
	{
		// ファイル名を検索専用の形式に変換する
		DXA_ConvSearchData( DXA, &SearchData, DirectoryPath, NULL ) ;

		// ディレクトリを変更
		if( DXA_ChangeCurrentDirectoryFast( DXA, &SearchData ) < 0 ) goto ERR ;
	}
	else
	{
		// \ or / がある場合は繋がったディレクトリを一つづつ変更してゆく
		int i ;
		int StrLength ;
		int CharBytes ;
		int CharBytes2 ;
		DWORD CharCode ;
		DWORD CharCode2 ;

		i = 0 ;

		// ループ
		for(;;)
		{
			// 文字列を取得する
			DXA_ConvSearchData( DXA, &SearchData, &DirectoryPath[ i ], &StrLength ) ;
			i += StrLength ;

			// もし初っ端が \ or / だった場合はルートディレクトリに落とす
			CharCode = GetCharCode( ( const char * )&DirectoryPath[ i ], DXA->CharCodeFormat, &CharBytes ) ;
			if( StrLength == 0 && ( CharCode == '\\' || CharCode == '/' ) )
			{
				BYTE EnStr[ 16 ] ;

				ConvString( ( const char * )Ascii_EnStr, -1, DX_CHARCODEFORMAT_ASCII, ( char * )EnStr, sizeof( EnStr ), DXA->CharCodeFormat ) ;
				DXA_ChangeCurrentDirectoryBase( DXA, EnStr, false ) ;
			}
			else
			{
				// それ以外の場合は普通にディレクトリ変更
				if( DXA_ChangeCurrentDirectoryFast( DXA, &SearchData ) < 0 )
				{
					// エラーが起きて、更にエラーが起きた時に元のディレクトリに戻せの
					// フラグが立っている場合は元のディレクトリに戻す
					if( ErrorIsDirectoryReset == true )
					{
						DXA->CurrentDirectory = OldDir ;
					}

					// エラー終了
					goto ERR ;
				}
			}

			// もし終端文字で終了した場合はループから抜ける
			// 又はあと \ or / しかない場合もループから抜ける
			if( CharCode == '\0' )
			{
				break ;
			}
			else
			{
				CharCode2 = GetCharCode( ( const char * )&DirectoryPath[ i + CharBytes ], DXA->CharCodeFormat, &CharBytes2 ) ;
				if( ( CharCode == '\\' && CharCode2 == '\0' ) ||
					( CharCode == '/'  && CharCode2 == '\0' ) )
				{
					break ;
				}
			}
			i += CharBytes ;
		}
	}

	if( LastSearchData != NULL )
	{
		_MEMCPY( LastSearchData->FileName, SearchData.FileName, ( size_t )( SearchData.PackNum * 4 ) ) ;
		LastSearchData->Parity  = SearchData.Parity ;
		LastSearchData->PackNum = SearchData.PackNum ;
	}

	// 正常終了
	return 0 ;

ERR:
	if( LastSearchData != NULL )
	{
		_MEMCPY( LastSearchData->FileName, SearchData.FileName, ( size_t )( SearchData.PackNum * 4 ) ) ;
		LastSearchData->Parity  = SearchData.Parity ;
		LastSearchData->PackNum = SearchData.PackNum ;
	}

	// エラー終了
	return -1 ;
}

// カレントディレクトリにある指定のファイルの鍵用の文字列を作成する、戻り値は文字列の長さ( 単位：Byte )( FileString は DXA_KEY_STRING_MAXLENGTH の長さが必要 )
static size_t DXA_CreateKeyFileString( DXARC *DXA, DXARC_DIRECTORY *Directory, DXARC_FILEHEAD *FileHead, BYTE *FileString )
{
	size_t StartAddr ;

	// 最初にパスワードの文字列をセット
	if( DXA->KeyStringBytes != 0 )
	{
		_MEMCPY( FileString, DXA->KeyString, DXA->KeyStringBytes ) ;
		FileString[ DXA->KeyStringBytes ] = '\0' ;
		StartAddr = DXA->KeyStringBytes ;
	}
	else
	{
		FileString[ 0 ] = '\0' ;
		StartAddr = 0 ;
	}
	_MEMSET( &FileString[ DXA_KEY_STRING_MAXLENGTH - 8 ], 0, 8 ) ;

	// 次にファイル名の文字列をセット
	CL_strcat_s( DXA->CharCodeFormat, ( char * )&FileString[ StartAddr ], ( DXA_KEY_STRING_MAXLENGTH - 8 ) - StartAddr, ( char * )( DXA->Table.NameTable + FileHead->NameAddress + 4 ) ) ;

	// その後にディレクトリの文字列をセット
	if( Directory->ParentDirectoryAddress != NONE_PAL )
	{
		do
		{
			CL_strcat_s( DXA->CharCodeFormat, ( char * )&FileString[ StartAddr ], ( DXA_KEY_STRING_MAXLENGTH - 8 ) - StartAddr, ( char * )( DXA->Table.NameTable + ( ( DXARC_FILEHEAD * )( DXA->Table.FileTable + Directory->DirectoryAddress ) )->NameAddress + 4 ) ) ;
			Directory = ( DXARC_DIRECTORY * )( DXA->Table.DirectoryTable + Directory->ParentDirectoryAddress ) ;
		}while( Directory->ParentDirectoryAddress != NONE_PAL ) ;
	}

	return StartAddr + CL_strlen( DXA->CharCodeFormat, ( char * )&FileString[ StartAddr ] ) * GetCharCodeFormatUnitSize( DXA->CharCodeFormat ) ;
}

// アーカイブ内のカレントディレクトリパスを取得する
//extern int DXA_GetCurrentDir( DXARC *DXA, char *DirPathBuffer, int BufferSize )
//{
//	char DirPath[FILEPATH_MAX] ;
//	int Depth, i ;
//
//	// 非同期同期オープン中の場合はここで開き終わるのを待つ
//	if( DXA->ASyncOpenFlag == TRUE )
//	{
//		while( DXA_CheckIdle( DXA ) == FALSE )
//		{
//			Thread_Sleep( 0 ) ;
//		}
//	}
//
//	if( DXA->V5Flag )
//	{
//		DXARC_DIRECTORY_VER5 *Dir[200], *DirTempP ;
//
//		// ルートディレクトリに着くまで検索する
//		Depth = 0 ;
//		DirTempP = DXA->CurrentDirectoryV5 ;
//		while( DirTempP->DirectoryAddress != 0xffffffff && DirTempP->DirectoryAddress != 0 )
//		{
//			Dir[Depth] = DirTempP ;
//			DirTempP = ( DXARC_DIRECTORY_VER5 * )( DXA->Table.DirectoryTable + DirTempP->ParentDirectoryAddress ) ;
//			Depth ++ ;
//		}
//		
//		// パス名を連結する
//		DirPath[0] = '\0' ;
//		for( i = Depth - 1 ; i >= 0 ; i -- )
//		{
//			_STRCAT( DirPath, "\\" ) ;
//			_STRCAT( DirPath, (char *)DXA->Table.NameTable + ((DXARC_FILEHEAD_VER5 *)( DXA->Table.FileTable + Dir[i]->DirectoryAddress ))->NameAddress ) ;
//		}
//	}
//	else
//	{
//		DXARC_DIRECTORY *Dir[200], *DirTempP ;
//
//		// ルートディレクトリに着くまで検索する
//		Depth = 0 ;
//		DirTempP = DXA->CurrentDirectory ;
//		while( DirTempP->DirectoryAddress != 0xffffffff && DirTempP->DirectoryAddress != 0 )
//		{
//			Dir[Depth] = DirTempP ;
//			DirTempP = ( DXARC_DIRECTORY * )( DXA->Table.DirectoryTable + DirTempP->ParentDirectoryAddress ) ;
//			Depth ++ ;
//		}
//		
//		// パス名を連結する
//		DirPath[0] = '\0' ;
//		for( i = Depth - 1 ; i >= 0 ; i -- )
//		{
//			_STRCAT( DirPath, "\\" ) ;
//			_STRCAT( DirPath, (char *)DXA->Table.NameTable + ((DXARC_FILEHEAD *)( DXA->Table.FileTable + Dir[i]->DirectoryAddress ))->NameAddress ) ;
//		}
//	}
//
//	// バッファの長さが０か、長さが足りないときはディレクトリ名の長さを返す
//	if( BufferSize == 0 || BufferSize < (int)_STRLEN( DirPath ) )
//	{
//		return _STRLEN( DirPath ) + 1 ;
//	}
//	else
//	{
//		// ディレクトリ名をバッファに転送する
//		_STRCPY( DirPathBuffer, DirPath ) ;
//	}
//
//	// 終了
//	return 0 ;
//}

// アーカイブ内のオブジェクトを検索する( -1:エラー -1以外:DXA検索ハンドル )
extern DWORD_PTR DXA_FindFirst( DXARC *DXA, const BYTE *FilePath, FILEINFOW *Buffer )
{
	DXA_FINDDATA *find ;
	BYTE Dir[  FILEPATH_MAX ] ;
	BYTE Name[ FILEPATH_MAX ] ;
	int CharBytes ;

	// 非同期同期オープン中の場合はここで開き終わるのを待つ
	if( DXA->ASyncOpenFlag == TRUE )
	{
		while( DXA_CheckIdle( DXA ) == FALSE )
		{
			Thread_Sleep( 0 ) ;
		}
	}

	// メモリの確保
	find = ( DXA_FINDDATA * )DXALLOC( sizeof( *find ) ) ;
	if( find == NULL )
	{
		return ( DWORD_PTR )-1 ;
	}

	find->Container = DXA;
	DXA_DIR_AnalysisFileNameAndDirPath( DXA, FilePath, Name, sizeof( Name ), Dir, sizeof( Dir ) );

	// 全て大文字にする
	CL_strupr( DXA->CharCodeFormat, ( char * )Dir  ) ;
	CL_strupr( DXA->CharCodeFormat, ( char * )Name ) ;

	// 検索対象のディレクトリを取得
	if( GetCharCode( ( const char * )Dir, DXA->CharCodeFormat, &CharBytes ) == '\0' )
	{
		find->Directory = DXA->CurrentDirectory ;
	}
	else
	{
		DXARC_DIRECTORY *OldDir;

		OldDir = DXA->CurrentDirectory;

		// 指定のディレクトリが無い場合はエラー
		if( DXA_ChangeCurrentDirectoryBase( DXA, Dir, false ) == -1 )
		{
			DXFREE( find ) ;
			DXA->CurrentDirectory = OldDir ;
			return ( DWORD_PTR )-1 ;
		}

		find->Directory       = DXA->CurrentDirectory ;
		DXA->CurrentDirectory = OldDir ;
	}

	find->ObjectCount = 0;
	CL_strcpy_s( DXA->CharCodeFormat, ( char * )find->SearchStr, sizeof( find->SearchStr ), ( const char * )Name ) ;

	// 適合する最初のファイルを検索する
	if( DXA_FindProcess( find, Buffer ) == -1 )
	{
		DXFREE( find );
		return ( DWORD_PTR )-1 ;
	}
	find->ObjectCount ++ ;

	// ハンドルを返す
	return ( DWORD_PTR )find ;
}

// アーカイブ内のオブジェクトを検索する( -1:エラー 0:成功 )
extern int DXA_FindNext( DWORD_PTR DxaFindHandle, FILEINFOW *Buffer )
{
	DXA_FINDDATA *find;

	find = ( DXA_FINDDATA * )DxaFindHandle ;
	if( DXA_FindProcess( find, Buffer ) == -1 )
	{
		return -1 ;
	}
	find->ObjectCount ++ ;

	return 0 ;
}

// アーカイブ内のオブジェクト検索を終了する
extern int DXA_FindClose( DWORD_PTR DxaFindHandle )
{
	DXA_FINDDATA *find ;

	find = ( DXA_FINDDATA * )DxaFindHandle ;
	DXFREE( find ) ;

	return 0 ;
}

// アーカイブファイルをメモリに読み込んだ場合のファイルイメージが格納されている先頭アドレスを取得する( DXA_OpenArchiveFromFileUseMem 若しくは DXA_OpenArchiveFromMem で開いた場合に有効 )
extern void *DXA_GetFileImage( DXARC *DXA )
{
	// 非同期同期オープン中の場合はここで開き終わるのを待つ
	if( DXA->ASyncOpenFlag == TRUE )
	{
		while( DXA_CheckIdle( DXA ) == FALSE )
		{
			Thread_Sleep( 0 ) ;
		}
	}

	// メモリイメージから開いていなかったらエラー
	if( DXA->MemoryOpenFlag == FALSE )
	{
		return NULL ;
	}

	// 先頭アドレスを返す
	return DXA->MemoryImage ;
}

// アーカイブファイル中の指定のファイルのファイル内の位置とファイルの大きさを得る( -1:エラー )
extern int DXA_GetFileInfo( DXARC *DXA, int CharCodeFormat, const char *FilePath, int *Position, int *Size )
{
	BYTE TempBuffer[ 4096 ] ;
	const BYTE *FilePathB ;

	// 文字コード形式が異なる場合は変換する
	if( CharCodeFormat != DXA->CharCodeFormat )
	{
		ConvString( FilePath, -1, CharCodeFormat, ( char * )TempBuffer, sizeof( TempBuffer ), DXA->CharCodeFormat ) ;
		FilePathB = TempBuffer ;
	}
	else
	{
		FilePathB = ( const BYTE * )FilePath ;
	}

	// 非同期同期オープン中の場合はここで開き終わるのを待つ
	if( DXA->ASyncOpenFlag == TRUE )
	{
		while( DXA_CheckIdle( DXA ) == FALSE )
		{
			Thread_Sleep( 0 ) ;
		}
	}

	{
		DXARC_FILEHEAD *FileH ;
		DXARC_DIRECTORY *Directory ;

		// 指定のファイルの情報を得る
		FileH = DXA_GetFileHeader( DXA, FilePathB, &Directory ) ;
		if( FileH == NULL )
		{
			return -1 ;
		}

		// ファイルのデータがある位置とファイルサイズを保存する
		if( Position != NULL )
		{
			*Position = ( int )( DXA->Head.DataStartAddress + FileH->DataAddress ) ;
		}

		if( Size     != NULL )
		{
			*Size     = ( int )( FileH->DataSize ) ;
		}
	}

	// 成功終了
	return 0 ;
}



// アーカイブファイル内のファイルを開く(ファイル閉じる作業は必要なし)
extern int DXA_STREAM_Initialize( DXARC_STREAM *DXAStream, DXARC *DXA, const BYTE *FilePath, int UseASyncReadFlag )
{
	// 非同期同期オープン中の場合はここで開き終わるのを待つ
	if( DXA->ASyncOpenFlag == TRUE )
	{
		while( DXA_CheckIdle( DXA ) == FALSE )
		{
			Thread_Sleep( 0 ) ;
		}
	}

	// データのセット
	DXAStream->Archive          = DXA ;
	DXAStream->EOFFlag          = FALSE ;
	DXAStream->FilePoint        = 0 ;
	DXAStream->DecodeDataBuffer = NULL ;
	DXAStream->DecodeTempBuffer = NULL ;
	DXAStream->UseASyncReadFlag = UseASyncReadFlag ;
	DXAStream->ASyncState       = DXARC_STREAM_ASYNCSTATE_IDLE ;

	// ファイルから開いている場合はアーカイブファイルのファイルポインタを作成
	if( DXA->MemoryOpenFlag == FALSE )
	{
		DXAStream->ReadOnlyFilePointer = ReadOnlyFileAccessOpen( DXA->FilePath, FALSE, TRUE, FALSE ) ;
		if( DXAStream->ReadOnlyFilePointer == 0 )
		{
			return -1 ;
		}
	}

	{
		DXARC_FILEHEAD *FileH ;
		DXARC_DIRECTORY *Directory ;

		// 指定のファイルの情報を得る
		FileH = DXA_GetFileHeader( DXA, FilePath, &Directory ) ;
		if( FileH == NULL )
		{
			if( DXA->MemoryOpenFlag == FALSE )
			{
				ReadOnlyFileAccessClose( DXAStream->ReadOnlyFilePointer ) ;
				DXAStream->ReadOnlyFilePointer = 0 ;
			}
			return -1 ;
		}

		// ファイル情報をセット
		DXAStream->FileHead = FileH ;

		// 鍵処理を行わないかどうかを保存
		DXAStream->NoKey = DXA->NoKey ;

		// 鍵を作成する
		if( DXAStream->NoKey == false )
		{
			char KeyString[ DXA_KEY_STRING_MAXLENGTH ] ;
			size_t KeyStringBytes ;
			KeyStringBytes = DXA_CreateKeyFileString( DXA, Directory, FileH, ( BYTE * )KeyString ) ;
			DXA_KeyCreate( KeyString, KeyStringBytes, DXAStream->Key ) ;
		}

		// ファイルが圧縮されている場合は解凍データが収まるメモリ領域の確保
		if( FileH->PressDataSize != NONE_PAL || FileH->HuffPressDataSize != NONE_PAL )
		{
			DXAStream->DecodeDataBuffer = DXALLOC( ( size_t )FileH->DataSize ) ;
			if( DXAStream->DecodeDataBuffer == NULL )
			{
				goto ERR ;
			}
		}

		// ファイルが圧縮されているかどうかで処理を分岐
		if( FileH->PressDataSize != NONE_PAL )
		{
			// ハフマン圧縮もされているかどうかで処理を分岐
			if( FileH->HuffPressDataSize != NONE_PAL )
			{
				// メモリ上に読み込まれているかどうかで処理を分岐
				if( DXA->MemoryOpenFlag == TRUE )
				{
					if( DXA->MemoryImageReadOnlyFlag )
					{
						// 圧縮データが収まるメモリ領域の確保
						DXAStream->DecodeTempBuffer = DXALLOC( ( size_t )( FileH->PressDataSize + FileH->HuffPressDataSize ) ) ;
						if( DXAStream->DecodeTempBuffer == NULL )
						{
							goto ERR ;
						}

						// 圧縮データのコピーと鍵解除
						_MEMCPY( DXAStream->DecodeTempBuffer, ( BYTE * )DXA->MemoryImage + DXA->Head.DataStartAddress + FileH->DataAddress, ( size_t )FileH->HuffPressDataSize ) ;
						if( DXAStream->NoKey == false ) DXA_KeyConv( DXAStream->DecodeTempBuffer, ( LONGLONG )FileH->HuffPressDataSize, ( LONGLONG )FileH->DataSize, DXAStream->Key ) ;

						// ハフマン圧縮データを解凍
						Huffman_Decode( DXAStream->DecodeTempBuffer, ( BYTE * )DXAStream->DecodeTempBuffer + FileH->HuffPressDataSize ) ;

						// ファイルの前後のみハフマン圧縮されていた場合は残りのLZ圧縮データをコピー
						if( DXA->Head.HuffmanEncodeKB != 0xff && FileH->PressDataSize > DXA->Head.HuffmanEncodeKB * 1024 * 2 )
						{
							// 解凍したデータの内、後ろ半分を移動する
							_MEMMOVE( 
								( BYTE * )DXAStream->DecodeTempBuffer + FileH->HuffPressDataSize + FileH->PressDataSize - DXA->Head.HuffmanEncodeKB * 1024,
								( BYTE * )DXAStream->DecodeTempBuffer + FileH->HuffPressDataSize + DXA->Head.HuffmanEncodeKB * 1024,
								DXA->Head.HuffmanEncodeKB * 1024
							) ;

							// 残りのLZ圧縮データを転送
							_MEMCPY(
								( BYTE * )DXAStream->DecodeTempBuffer + FileH->HuffPressDataSize + DXA->Head.HuffmanEncodeKB * 1024,
								( BYTE * )DXA->MemoryImage + DXA->Head.DataStartAddress + FileH->DataAddress + FileH->HuffPressDataSize,
								( size_t )( FileH->PressDataSize - DXA->Head.HuffmanEncodeKB * 1024 * 2 )
							) ;
							if( DXAStream->NoKey == false ) DXA_KeyConv( ( BYTE * )DXAStream->DecodeTempBuffer + FileH->HuffPressDataSize + DXA->Head.HuffmanEncodeKB * 1024, ( LONGLONG )( FileH->PressDataSize - DXA->Head.HuffmanEncodeKB * 1024 * 2 ), ( LONGLONG )( FileH->DataSize + FileH->HuffPressDataSize ), DXAStream->Key ) ;
						}

						// 解凍
						DXA_Decode( ( BYTE * )DXAStream->DecodeTempBuffer + FileH->HuffPressDataSize, DXAStream->DecodeDataBuffer ) ;
				
						// 作業用メモリの解放
						DXFREE( DXAStream->DecodeTempBuffer ) ;
						DXAStream->DecodeTempBuffer = NULL ;
					}
					else
					{
						// 圧縮データが収まるメモリ領域の確保
						DXAStream->DecodeTempBuffer = DXALLOC( ( size_t )FileH->PressDataSize ) ;
						if( DXAStream->DecodeTempBuffer == NULL )
						{
							goto ERR ;
						}

						// ハフマン圧縮データを解凍
						Huffman_Decode( ( BYTE * )DXA->MemoryImage + DXA->Head.DataStartAddress + FileH->DataAddress, DXAStream->DecodeTempBuffer ) ;

						// ファイルの前後のみハフマン圧縮されていた場合は残りのLZ圧縮データをコピー
						if( DXA->Head.HuffmanEncodeKB != 0xff && FileH->PressDataSize > DXA->Head.HuffmanEncodeKB * 1024 * 2 )
						{
							// 解凍したデータの内、後ろ半分を移動する
							_MEMMOVE( 
								( BYTE * )DXAStream->DecodeTempBuffer + FileH->PressDataSize - DXA->Head.HuffmanEncodeKB * 1024,
								( BYTE * )DXAStream->DecodeTempBuffer + DXA->Head.HuffmanEncodeKB * 1024,
								DXA->Head.HuffmanEncodeKB * 1024
							) ;

							// 残りのLZ圧縮データを転送
							_MEMCPY(
								( BYTE * )DXAStream->DecodeTempBuffer + DXA->Head.HuffmanEncodeKB * 1024,
								( BYTE * )DXA->MemoryImage + DXA->Head.DataStartAddress + FileH->DataAddress + FileH->HuffPressDataSize,
								( size_t )( FileH->PressDataSize - DXA->Head.HuffmanEncodeKB * 1024 * 2 )
							) ;
						}

						// 解凍
						DXA_Decode( DXAStream->DecodeTempBuffer, DXAStream->DecodeDataBuffer ) ;
				
						// 作業用メモリの解放
						DXFREE( DXAStream->DecodeTempBuffer ) ;
						DXAStream->DecodeTempBuffer = NULL ;
					}
				}
				else
				{
					// 圧縮データの位置へファイルポインタを移動
					DXAStream->ASyncReadFileAddress = DXA->Head.DataStartAddress + FileH->DataAddress ;
					ReadOnlyFileAccessSeek( DXAStream->ReadOnlyFilePointer, ( LONGLONG )DXAStream->ASyncReadFileAddress, SEEK_SET ) ;

					// 非同期の場合は読み込みと鍵解除を別々に行う
					if( DXAStream->UseASyncReadFlag == TRUE )
					{
						// ファイルの前後のみハフマン圧縮されているかどうかで読み込む容量を分岐
						if( DXA->Head.HuffmanEncodeKB != 0xff && FileH->PressDataSize > DXA->Head.HuffmanEncodeKB * 1024 * 2 )
						{
							// 圧縮データが収まるメモリ領域の確保
							DXAStream->DecodeTempBuffer = DXALLOC( ( size_t )( FileH->PressDataSize + FileH->HuffPressDataSize + DXA->Head.HuffmanEncodeKB * 1024 * 3 ) ) ;
							if( DXAStream->DecodeTempBuffer == NULL )
							{
								goto ERR ;
							}

							// 圧縮データの読み込み
							ReadOnlyFileAccessRead( ( BYTE * )DXAStream->DecodeTempBuffer + DXA->Head.HuffmanEncodeKB * 1024 * 2, ( size_t )( FileH->HuffPressDataSize + FileH->PressDataSize - DXA->Head.HuffmanEncodeKB * 1024 * 2 ), 1, DXAStream->ReadOnlyFilePointer ) ;
						}
						else
						{
							// 圧縮データが収まるメモリ領域の確保
							DXAStream->DecodeTempBuffer = DXALLOC( ( size_t )( FileH->PressDataSize + FileH->HuffPressDataSize ) ) ;
							if( DXAStream->DecodeTempBuffer == NULL )
							{
								goto ERR ;
							}

							// 圧縮データの読み込み
							ReadOnlyFileAccessRead( DXAStream->DecodeTempBuffer, ( size_t )FileH->HuffPressDataSize, 1, DXAStream->ReadOnlyFilePointer ) ;
						}
						DXAStream->ASyncState = DXARC_STREAM_ASYNCSTATE_PRESSREAD ;
					}
					else
					{
						// 圧縮データが収まるメモリ領域の確保
						DXAStream->DecodeTempBuffer = DXALLOC( ( size_t )( FileH->PressDataSize + FileH->HuffPressDataSize ) ) ;
						if( DXAStream->DecodeTempBuffer == NULL )
						{
							goto ERR ;
						}

						// 圧縮データの読み込みと鍵解除
						DXA_KeyConvFileRead( DXAStream->DecodeTempBuffer, FileH->HuffPressDataSize, DXAStream->ReadOnlyFilePointer, DXAStream->NoKey ? NULL : DXAStream->Key, ( LONGLONG )FileH->DataSize ) ;

						// ハフマン圧縮データを解凍
						Huffman_Decode( DXAStream->DecodeTempBuffer, ( BYTE * )DXAStream->DecodeTempBuffer + FileH->HuffPressDataSize ) ;

						// ファイルの前後のみハフマン圧縮している場合は処理を分岐
						if( DXA->Head.HuffmanEncodeKB != 0xff && FileH->PressDataSize > DXA->Head.HuffmanEncodeKB * 1024 * 2 )
						{
							// 解凍したデータの内、後ろ半分を移動する
							_MEMMOVE( 
								( BYTE * )DXAStream->DecodeTempBuffer + FileH->HuffPressDataSize + FileH->PressDataSize - DXA->Head.HuffmanEncodeKB * 1024,
								( BYTE * )DXAStream->DecodeTempBuffer + FileH->HuffPressDataSize + DXA->Head.HuffmanEncodeKB * 1024,
								DXA->Head.HuffmanEncodeKB * 1024 ) ;

							// 残りのデータを読み込む
							DXA_KeyConvFileRead(
								( BYTE * )DXAStream->DecodeTempBuffer + FileH->HuffPressDataSize + DXA->Head.HuffmanEncodeKB * 1024,
								FileH->PressDataSize - DXA->Head.HuffmanEncodeKB * 1024 * 2,
								DXAStream->ReadOnlyFilePointer, DXAStream->NoKey ? NULL : DXAStream->Key, FileH->DataSize + FileH->HuffPressDataSize ) ;
						}

						// 解凍
						DXA_Decode( ( BYTE * )DXAStream->DecodeTempBuffer + FileH->HuffPressDataSize, DXAStream->DecodeDataBuffer ) ;
				
						// メモリの解放
						DXFREE( DXAStream->DecodeTempBuffer ) ;
						DXAStream->DecodeTempBuffer = NULL ;
					}
				}
			}
			else
			{
				// メモリ上に読み込まれているかどうかで処理を分岐
				if( DXA->MemoryOpenFlag == TRUE )
				{
					if( DXA->MemoryImageReadOnlyFlag )
					{
						// 圧縮データが収まるメモリ領域の確保
						DXAStream->DecodeTempBuffer = DXALLOC( ( size_t )FileH->PressDataSize ) ;
						if( DXAStream->DecodeTempBuffer == NULL )
						{
							goto ERR ;
						}

						// 圧縮データの読み込み
						_MEMCPY( DXAStream->DecodeTempBuffer, ( BYTE * )DXA->MemoryImage + DXA->Head.DataStartAddress + FileH->DataAddress, ( size_t )FileH->PressDataSize ) ;
						if( DXAStream->NoKey == false ) DXA_KeyConv( DXAStream->DecodeTempBuffer, ( LONGLONG )FileH->PressDataSize, ( LONGLONG )FileH->DataSize, DXAStream->Key ) ;

						// 解凍
						DXA_Decode( DXAStream->DecodeTempBuffer, DXAStream->DecodeDataBuffer ) ;
				
						// メモリの解放
						DXFREE( DXAStream->DecodeTempBuffer ) ;
						DXAStream->DecodeTempBuffer = NULL ;
					}
					else
					{
						// 解凍
						DXA_Decode( (BYTE *)DXA->MemoryImage + DXA->Head.DataStartAddress + FileH->DataAddress, DXAStream->DecodeDataBuffer ) ;
					}
				}
				else
				{
					// 圧縮データが収まるメモリ領域の確保
					DXAStream->DecodeTempBuffer = DXALLOC( ( size_t )FileH->PressDataSize ) ;
					if( DXAStream->DecodeTempBuffer == NULL )
					{
						goto ERR ;
					}

					// 圧縮データの位置へファイルポインタを移動
					DXAStream->ASyncReadFileAddress = DXA->Head.DataStartAddress + FileH->DataAddress;
					ReadOnlyFileAccessSeek( DXAStream->ReadOnlyFilePointer, ( LONGLONG )DXAStream->ASyncReadFileAddress, SEEK_SET ) ;

					// 非同期の場合は読み込みと鍵解除を別々に行う
					if( DXAStream->UseASyncReadFlag == TRUE )
					{
						// 圧縮データの読み込み
						ReadOnlyFileAccessRead( DXAStream->DecodeTempBuffer, ( size_t )FileH->PressDataSize, 1, DXAStream->ReadOnlyFilePointer ) ;
						DXAStream->ASyncState = DXARC_STREAM_ASYNCSTATE_PRESSREAD ;
					}
					else
					{
						// 圧縮データの読み込みと鍵解除
						DXA_KeyConvFileRead( DXAStream->DecodeTempBuffer, FileH->PressDataSize, DXAStream->ReadOnlyFilePointer, DXAStream->NoKey ? NULL : DXAStream->Key, ( LONGLONG )FileH->DataSize ) ;

						// 解凍
						DXA_Decode( DXAStream->DecodeTempBuffer, DXAStream->DecodeDataBuffer ) ;
				
						// メモリの解放
						DXFREE( DXAStream->DecodeTempBuffer ) ;
						DXAStream->DecodeTempBuffer = NULL ;
					}
				}
			}
		}
		else
		// ハフマン圧縮だけされているかどうかで処理を分岐
		if( FileH->HuffPressDataSize != NONE_PAL )
		{
			// メモリ上に読み込まれているかどうかで処理を分岐
			if( DXA->MemoryOpenFlag == TRUE )
			{
				if( DXA->MemoryImageReadOnlyFlag )
				{
					// 圧縮データが収まるメモリ領域の確保
					DXAStream->DecodeTempBuffer = DXALLOC( ( size_t )FileH->HuffPressDataSize ) ;
					if( DXAStream->DecodeTempBuffer == NULL )
					{
						goto ERR ;
					}

					// 圧縮データのコピーと鍵解除
					_MEMCPY( DXAStream->DecodeTempBuffer, ( BYTE * )DXA->MemoryImage + DXA->Head.DataStartAddress + FileH->DataAddress, ( size_t )FileH->HuffPressDataSize ) ;
					if( DXAStream->NoKey == false ) DXA_KeyConv( DXAStream->DecodeTempBuffer, ( LONGLONG )FileH->HuffPressDataSize, ( LONGLONG )FileH->DataSize, DXAStream->Key ) ;

					// ハフマン圧縮データを解凍
					Huffman_Decode( DXAStream->DecodeTempBuffer, DXAStream->DecodeDataBuffer ) ;

					// ファイルの前後のみハフマン圧縮している場合は処理を分岐
					if( DXA->Head.HuffmanEncodeKB != 0xff && FileH->DataSize > DXA->Head.HuffmanEncodeKB * 1024 * 2 )
					{
						// 解凍したデータの内、後ろ半分を移動する
						_MEMMOVE( 
							( BYTE * )DXAStream->DecodeDataBuffer + FileH->DataSize - DXA->Head.HuffmanEncodeKB * 1024,
							( BYTE * )DXAStream->DecodeDataBuffer + DXA->Head.HuffmanEncodeKB * 1024,
							DXA->Head.HuffmanEncodeKB * 1024 ) ;

						// 残りのデータをコピーする
						_MEMCPY(
							( BYTE * )DXAStream->DecodeDataBuffer + DXA->Head.HuffmanEncodeKB * 1024,
							( BYTE * )DXA->MemoryImage + DXA->Head.DataStartAddress + FileH->DataAddress + FileH->HuffPressDataSize,
							( size_t )( FileH->DataSize - DXA->Head.HuffmanEncodeKB * 1024 * 2 )
						) ;
						if( DXAStream->NoKey == false ) DXA_KeyConv( ( BYTE * )DXAStream->DecodeDataBuffer + DXA->Head.HuffmanEncodeKB * 1024, ( LONGLONG )( FileH->DataSize - DXA->Head.HuffmanEncodeKB * 1024 * 2 ), ( LONGLONG )( FileH->DataSize + FileH->HuffPressDataSize ), DXAStream->Key ) ;
					}
				
					// 作業用メモリの解放
					DXFREE( DXAStream->DecodeTempBuffer ) ;
					DXAStream->DecodeTempBuffer = NULL ;
				}
				else
				{
					// ハフマン圧縮データを解凍
					Huffman_Decode( ( BYTE * )DXA->MemoryImage + DXA->Head.DataStartAddress + FileH->DataAddress, DXAStream->DecodeDataBuffer ) ;

					// ファイルの前後のみハフマン圧縮している場合は処理を分岐
					if( DXA->Head.HuffmanEncodeKB != 0xff && FileH->DataSize > DXA->Head.HuffmanEncodeKB * 1024 * 2 )
					{
						// 解凍したデータの内、後ろ半分を移動する
						_MEMMOVE( 
							( BYTE * )DXAStream->DecodeDataBuffer + FileH->DataSize - DXA->Head.HuffmanEncodeKB * 1024,
							( BYTE * )DXAStream->DecodeDataBuffer + DXA->Head.HuffmanEncodeKB * 1024,
							DXA->Head.HuffmanEncodeKB * 1024 ) ;

						// 残りのデータをコピーする
						_MEMCPY(
							( BYTE * )DXAStream->DecodeDataBuffer + DXA->Head.HuffmanEncodeKB * 1024,
							( BYTE * )DXA->MemoryImage + DXA->Head.DataStartAddress + FileH->DataAddress + FileH->HuffPressDataSize,
							( size_t )( FileH->DataSize - DXA->Head.HuffmanEncodeKB * 1024 * 2 )
						) ;
					}
				}
			}
			else
			{
				// 圧縮データの位置へファイルポインタを移動
				DXAStream->ASyncReadFileAddress = DXA->Head.DataStartAddress + FileH->DataAddress ;
				ReadOnlyFileAccessSeek( DXAStream->ReadOnlyFilePointer, ( LONGLONG )DXAStream->ASyncReadFileAddress, SEEK_SET ) ;

				// 非同期の場合は読み込みと鍵解除を別々に行う
				if( DXAStream->UseASyncReadFlag == TRUE )
				{
					// ファイルの前後のみハフマン圧縮されているかどうかで読み込む容量を分岐
					if( DXA->Head.HuffmanEncodeKB != 0xff && FileH->DataSize > DXA->Head.HuffmanEncodeKB * 1024 * 2 )
					{
						// 圧縮データが収まるメモリ領域の確保
						DXAStream->DecodeTempBuffer = DXALLOC( ( size_t )( FileH->HuffPressDataSize + FileH->DataSize - DXA->Head.HuffmanEncodeKB * 1024 * 2 ) ) ;
						if( DXAStream->DecodeTempBuffer == NULL )
						{
							goto ERR ;
						}

						// 圧縮データの読み込み
						ReadOnlyFileAccessRead( DXAStream->DecodeTempBuffer, ( size_t )( FileH->HuffPressDataSize + FileH->DataSize - DXA->Head.HuffmanEncodeKB * 1024 * 2 ), 1, DXAStream->ReadOnlyFilePointer ) ;
					}
					else
					{
						// 圧縮データが収まるメモリ領域の確保
						DXAStream->DecodeTempBuffer = DXALLOC( ( size_t )FileH->HuffPressDataSize ) ;
						if( DXAStream->DecodeTempBuffer == NULL )
						{
							goto ERR ;
						}

						// 圧縮データの読み込み
						ReadOnlyFileAccessRead( DXAStream->DecodeTempBuffer, ( size_t )FileH->HuffPressDataSize, 1, DXAStream->ReadOnlyFilePointer ) ;
					}
					DXAStream->ASyncState = DXARC_STREAM_ASYNCSTATE_PRESSREAD ;
				}
				else
				{
					// 圧縮データが収まるメモリ領域の確保
					DXAStream->DecodeTempBuffer = DXALLOC( ( size_t )FileH->HuffPressDataSize ) ;
					if( DXAStream->DecodeTempBuffer == NULL )
					{
						goto ERR ;
					}

					// 圧縮データの読み込みと鍵解除
					DXA_KeyConvFileRead( DXAStream->DecodeTempBuffer, FileH->HuffPressDataSize, DXAStream->ReadOnlyFilePointer, DXAStream->NoKey ? NULL : DXAStream->Key, ( LONGLONG )FileH->DataSize ) ;

					// ハフマン圧縮データを解凍
					Huffman_Decode( DXAStream->DecodeTempBuffer, DXAStream->DecodeDataBuffer ) ;
				
					// ファイルの前後のみハフマン圧縮している場合は処理を分岐
					if( DXA->Head.HuffmanEncodeKB != 0xff && FileH->DataSize > DXA->Head.HuffmanEncodeKB * 1024 * 2 )
					{
						// 解凍したデータの内、後ろ半分を移動する
						_MEMMOVE( 
							( BYTE * )DXAStream->DecodeDataBuffer + FileH->DataSize - DXA->Head.HuffmanEncodeKB * 1024,
							( BYTE * )DXAStream->DecodeDataBuffer + DXA->Head.HuffmanEncodeKB * 1024,
							DXA->Head.HuffmanEncodeKB * 1024 ) ;
						
						// 残りのデータを読み込む
						DXA_KeyConvFileRead(
							( BYTE * )DXAStream->DecodeDataBuffer + DXA->Head.HuffmanEncodeKB * 1024,
							FileH->DataSize - DXA->Head.HuffmanEncodeKB * 1024 * 2,
							DXAStream->ReadOnlyFilePointer, DXAStream->NoKey ? NULL : DXAStream->Key, FileH->DataSize + FileH->HuffPressDataSize ) ;
					}

					// メモリの解放
					DXFREE( DXAStream->DecodeTempBuffer ) ;
					DXAStream->DecodeTempBuffer = NULL ;
				}
			}
		}
	}

	// 終了
	return 0 ;

ERR :
	if( DXAStream->DecodeDataBuffer != NULL )
	{
		DXFREE( DXAStream->DecodeDataBuffer ) ;
		DXAStream->DecodeDataBuffer = NULL ;
	}
	if( DXAStream->DecodeTempBuffer != NULL )
	{
		DXFREE( DXAStream->DecodeTempBuffer ) ;
		DXAStream->DecodeTempBuffer = NULL ;
	}

	// エラー終了
	return -1 ;
}

// アーカイブファイル内のファイルを閉じる
extern int DXA_STREAM_Terminate( DXARC_STREAM *DXAStream )
{
	// 非同期読み込みで状態がまだ待機状態ではなかったら待機状態になるまで待つ
	if( DXAStream->UseASyncReadFlag == TRUE && DXAStream->ASyncState != DXARC_STREAM_ASYNCSTATE_IDLE )
	{
		while( DXA_STREAM_IdleCheck( DXAStream ) == FALSE )
		{
			Thread_Sleep( 1 ) ;
		}
	}

	// メモリの解放
	if( DXAStream->DecodeDataBuffer != NULL )
	{
		DXFREE( DXAStream->DecodeDataBuffer ) ;
		DXAStream->DecodeDataBuffer = NULL ;
	}

	if( DXAStream->DecodeTempBuffer != NULL )
	{
		DXFREE( DXAStream->DecodeTempBuffer ) ;
		DXAStream->DecodeTempBuffer = NULL ;
	}

	// ファイルを閉じる
	if( DXAStream->Archive->MemoryOpenFlag == FALSE )
	{
		ReadOnlyFileAccessClose( DXAStream->ReadOnlyFilePointer ) ;
		DXAStream->ReadOnlyFilePointer = 0 ;
	}

	// ゼロ初期化
	_MEMSET( DXAStream, 0, sizeof( DXARC_STREAM ) ) ;

	// 終了
	return 0 ;
}

// ファイルの内容を読み込む
extern int DXA_STREAM_Read( DXARC_STREAM *DXAStream, void *Buffer, size_t ReadLength )
{
	size_t ReadSize ;
	ULONGLONG DataSize ;
	ULONGLONG DataStartAddress ;
	ULONGLONG DataAddress ;

	// 非同期読み込みで状態がまだ待機状態ではなかったら待機状態になるまで待つ
	if( DXAStream->UseASyncReadFlag == TRUE && DXAStream->ASyncState != DXARC_STREAM_ASYNCSTATE_IDLE )
	{
		while( DXA_STREAM_IdleCheck( DXAStream ) == FALSE )
		{
			Thread_Sleep( 1 ) ;
		}
	}

	{
		DataSize = DXAStream->FileHead->DataSize ;
		DataAddress = DXAStream->FileHead->DataAddress ;
		DataStartAddress = DXAStream->Archive->Head.DataStartAddress ;
	}

	// EOF フラグが立っていたら０を返す
	if( DXAStream->EOFFlag == TRUE )
	{
		return 0 ;
	}

	// EOF 検出
	if( DataSize == DXAStream->FilePoint )
	{
		DXAStream->EOFFlag = TRUE ;
		return 0 ;
	}

	// データを読み込む量を設定する
	ReadSize = ReadLength < DataSize - DXAStream->FilePoint ? ReadLength : ( size_t )( DataSize - DXAStream->FilePoint ) ;

	// データが圧縮されていたかどうかで処理を分岐
	if( DXAStream->DecodeDataBuffer != NULL )
	{
		// データをコピーする
		_MEMCPY( Buffer, (BYTE *)DXAStream->DecodeDataBuffer + DXAStream->FilePoint, ReadSize ) ;
	}
	else
	{
		// メモリ上にデータがあるかどうかで処理を分岐
		if( DXAStream->Archive->MemoryOpenFlag == TRUE )
		{
			// メモリ上にある場合

			// データをコピーする
			_MEMCPY( Buffer, (BYTE *)DXAStream->Archive->MemoryImage + DataStartAddress + DataAddress + DXAStream->FilePoint, ReadSize ) ;

			if( DXAStream->Archive->MemoryImageReadOnlyFlag )
			{
				if( DXAStream->NoKey == false ) DXA_KeyConv( Buffer, ( LONGLONG )ReadSize, ( LONGLONG )( DataSize + DXAStream->FilePoint ), DXAStream->Key ) ;
			}
		}
		else
		{
			// ファイルから読み込んでいる場合

			// アーカイブファイルポインタと、仮想ファイルポインタが一致しているか調べる
			// 一致していなかったらアーカイブファイルポインタを移動する
			DXAStream->ASyncReadFileAddress = DataAddress + DataStartAddress + DXAStream->FilePoint ;
			if( ( ULONGLONG )ReadOnlyFileAccessTell( DXAStream->ReadOnlyFilePointer ) != DXAStream->ASyncReadFileAddress )
			{
				ReadOnlyFileAccessSeek( DXAStream->ReadOnlyFilePointer, ( LONGLONG )DXAStream->ASyncReadFileAddress, SEEK_SET ) ;
			}

			// 非同期読み込みの場合と同期読み込みの場合で処理を分岐
			if( DXAStream->UseASyncReadFlag )
			{
				// ファイルから読み込み
				ReadOnlyFileAccessRead( Buffer, ReadSize, 1, DXAStream->ReadOnlyFilePointer ) ;
				DXAStream->ReadBuffer = Buffer;
				DXAStream->ReadSize = ( int )ReadSize;
				DXAStream->ASyncState = DXARC_STREAM_ASYNCSTATE_READ ;
			}
			else
			{
				// データを読み込む
				DXA_KeyConvFileRead( Buffer, ReadSize, DXAStream->ReadOnlyFilePointer, DXAStream->NoKey ? NULL : DXAStream->Key, ( LONGLONG )( DataSize + DXAStream->FilePoint ) ) ;
			}
		}
	}
	
	// EOF フラグを倒す
	DXAStream->EOFFlag = FALSE ;

	// 読み込んだ分だけファイルポインタを移動する
	DXAStream->FilePoint += ( int )ReadSize ;
	
	// 読み込んだ容量を返す
	return ( int )ReadSize ;
}
	
// ファイルポインタを変更する
extern	int DXA_STREAM_Seek( DXARC_STREAM *DXAStream, LONGLONG SeekPoint, int SeekMode )
{
	ULONGLONG DataSize ;

	// 非同期読み込みで状態がまだ待機状態ではなかったら待機状態になるまで待つ
	if( DXAStream->UseASyncReadFlag == TRUE && DXAStream->ASyncState != DXARC_STREAM_ASYNCSTATE_IDLE )
	{
		while( DXA_STREAM_IdleCheck( DXAStream ) == FALSE )
		{
			Thread_Sleep( 1 ) ;
		}
	}

	{
		DataSize = DXAStream->FileHead->DataSize ;
	}

	// シークタイプによって処理を分岐
	switch( SeekMode )
	{
	case SEEK_SET : break ;		
	case SEEK_CUR : SeekPoint += ( LONGLONG )( DXAStream->FilePoint ) ; break ;
	case SEEK_END :	SeekPoint  = ( LONGLONG )( DataSize + SeekPoint ) ; break ;
	}
	
	// 補正
	if( SeekPoint > ( LONGLONG )DataSize ) SeekPoint = ( LONGLONG )DataSize ;
	if( SeekPoint < 0 ) SeekPoint = 0 ;
	
	// セット
	DXAStream->FilePoint = ( ULONGLONG )SeekPoint ;
	
	// EOFフラグを倒す
	DXAStream->EOFFlag = FALSE ;
	
	// 終了
	return 0 ;
}

// 現在のファイルポインタを得る
extern	LONGLONG DXA_STREAM_Tell( DXARC_STREAM *DXAStream )
{
	// 非同期読み込みで状態がまだ待機状態ではなかったら待機状態になるまで待つ
	if( DXAStream->UseASyncReadFlag == TRUE && DXAStream->ASyncState != DXARC_STREAM_ASYNCSTATE_IDLE )
	{
		while( DXA_STREAM_IdleCheck( DXAStream ) == FALSE )
		{
			Thread_Sleep( 1 ) ;
		}
	}

	return ( LONGLONG )DXAStream->FilePoint ;
}

// ファイルの終端に来ているか、のフラグを得る
extern	int DXA_STREAM_Eof( DXARC_STREAM *DXAStream )
{
	// 非同期読み込みで状態がまだ待機状態ではなかったら待機状態になるまで待つ
	if( DXAStream->UseASyncReadFlag == TRUE && DXAStream->ASyncState != DXARC_STREAM_ASYNCSTATE_IDLE )
	{
		while( DXA_STREAM_IdleCheck( DXAStream ) == FALSE )
		{
			Thread_Sleep( 1 ) ;
		}
	}

	return DXAStream->EOFFlag ? EOF : 0 ;
}

// 読み込み処理が完了しているかどうかを調べる
extern	int	DXA_STREAM_IdleCheck( DXARC_STREAM *DXAStream )
{
	// 非同期読み込みではない場合は何もせず TRUE を返す
	if( DXAStream->UseASyncReadFlag == FALSE )
	{
		return TRUE ;
	}

	// 状態によって処理を分岐
	switch( DXAStream->ASyncState )
	{
	case DXARC_STREAM_ASYNCSTATE_IDLE:			// 待機状態
		return TRUE;

	case DXARC_STREAM_ASYNCSTATE_PRESSREAD:		// 圧縮データ読み込み待ち

		// 読み込み終了待ち
		if( ReadOnlyFileAccessIdleCheck( DXAStream->ReadOnlyFilePointer ) == FALSE )
		{
			return FALSE;
		}

		// 圧縮されているかどうかで処理を分岐
		if( DXAStream->FileHead->PressDataSize != NONE_PAL )
		{
			// ハフマン圧縮もされているかどうかで処理を分岐
			if( DXAStream->FileHead->HuffPressDataSize != NONE_PAL )
			{
				// ファイルの前後のみハフマン圧縮されているかどうかで処理を分岐
				if( DXAStream->Archive->Head.HuffmanEncodeKB != 0xff && DXAStream->FileHead->PressDataSize > DXAStream->Archive->Head.HuffmanEncodeKB * 1024 * 2 )
				{
					// 鍵を解除
					if( DXAStream->NoKey == false ) DXA_KeyConv( ( BYTE * )DXAStream->DecodeTempBuffer + DXAStream->Archive->Head.HuffmanEncodeKB * 1024 * 2, ( LONGLONG )( DXAStream->FileHead->HuffPressDataSize + DXAStream->FileHead->PressDataSize - DXAStream->Archive->Head.HuffmanEncodeKB * 1024 * 2 ), ( LONGLONG )DXAStream->FileHead->DataSize, DXAStream->Key ) ;

					// ハフマン圧縮データを解凍
					Huffman_Decode( ( BYTE * )DXAStream->DecodeTempBuffer + DXAStream->Archive->Head.HuffmanEncodeKB * 1024 * 2, ( BYTE * )DXAStream->DecodeTempBuffer ) ;

					// LZ圧縮データの前後に解凍したデータを転送
					_MEMCPY(
						( BYTE * )DXAStream->DecodeTempBuffer + DXAStream->Archive->Head.HuffmanEncodeKB * 1024 * 2 + DXAStream->FileHead->HuffPressDataSize + DXAStream->FileHead->PressDataSize - DXAStream->Archive->Head.HuffmanEncodeKB * 1024 * 2,
						( BYTE * )DXAStream->DecodeTempBuffer + DXAStream->Archive->Head.HuffmanEncodeKB * 1024,
						DXAStream->Archive->Head.HuffmanEncodeKB * 1024
					) ;
					_MEMCPY(
						( BYTE * )DXAStream->DecodeTempBuffer + DXAStream->Archive->Head.HuffmanEncodeKB * 1024 * 2 + DXAStream->FileHead->HuffPressDataSize - DXAStream->Archive->Head.HuffmanEncodeKB * 1024,
						( BYTE * )DXAStream->DecodeTempBuffer,
						DXAStream->Archive->Head.HuffmanEncodeKB * 1024
					) ;

					// 解凍
					DXA_Decode( ( BYTE * )DXAStream->DecodeTempBuffer + DXAStream->Archive->Head.HuffmanEncodeKB * 1024 * 2 + DXAStream->FileHead->HuffPressDataSize - DXAStream->Archive->Head.HuffmanEncodeKB * 1024, DXAStream->DecodeDataBuffer ) ;
				}
				else
				{
					// 鍵を解除
					if( DXAStream->NoKey == false ) DXA_KeyConv( DXAStream->DecodeTempBuffer, ( LONGLONG )DXAStream->FileHead->HuffPressDataSize, ( LONGLONG )DXAStream->FileHead->DataSize, DXAStream->Key ) ;

					// ハフマン圧縮データを解凍
					Huffman_Decode( DXAStream->DecodeTempBuffer, ( BYTE * )DXAStream->DecodeTempBuffer + DXAStream->FileHead->HuffPressDataSize ) ;

					// 解凍
					DXA_Decode( ( BYTE * )DXAStream->DecodeTempBuffer + DXAStream->FileHead->HuffPressDataSize, DXAStream->DecodeDataBuffer ) ;
				}
			}
			else
			{
				// 鍵を解除
				if( DXAStream->NoKey == false ) DXA_KeyConv( DXAStream->DecodeTempBuffer, ( LONGLONG )DXAStream->FileHead->PressDataSize, ( LONGLONG )DXAStream->FileHead->DataSize, DXAStream->Key ) ;

				// 解凍
				DXA_Decode( DXAStream->DecodeTempBuffer, DXAStream->DecodeDataBuffer ) ;
			}
		}
		else
		// ハフマン圧縮だけされているかどうかで処理を分岐
		if( DXAStream->FileHead->HuffPressDataSize != NONE_PAL )
		{
			// ファイルの前後のみハフマン圧縮されているかどうかで処理を分岐
			if( DXAStream->Archive->Head.HuffmanEncodeKB != 0xff && DXAStream->FileHead->DataSize > DXAStream->Archive->Head.HuffmanEncodeKB * 1024 * 2 )
			{
				// 鍵を解除
				if( DXAStream->NoKey == false ) DXA_KeyConv( DXAStream->DecodeTempBuffer, ( LONGLONG )( DXAStream->FileHead->HuffPressDataSize + DXAStream->FileHead->DataSize - DXAStream->Archive->Head.HuffmanEncodeKB * 1024 * 2 ), ( LONGLONG )DXAStream->FileHead->DataSize, DXAStream->Key ) ;

				// ハフマン圧縮データを解凍
				Huffman_Decode( DXAStream->DecodeTempBuffer, ( BYTE * )DXAStream->DecodeDataBuffer ) ;

				// 解凍したデータを後半部分に移動
				_MEMCPY(
					( BYTE * )DXAStream->DecodeDataBuffer + DXAStream->FileHead->DataSize - DXAStream->Archive->Head.HuffmanEncodeKB * 1024,
					( BYTE * )DXAStream->DecodeDataBuffer + DXAStream->Archive->Head.HuffmanEncodeKB * 1024,
					DXAStream->Archive->Head.HuffmanEncodeKB * 1024
				) ;

				// 残りの部分をコピー
				_MEMCPY(
					( BYTE * )DXAStream->DecodeDataBuffer + DXAStream->Archive->Head.HuffmanEncodeKB * 1024,
					( BYTE * )DXAStream->DecodeTempBuffer + DXAStream->FileHead->HuffPressDataSize,
					( size_t )( DXAStream->FileHead->DataSize - DXAStream->Archive->Head.HuffmanEncodeKB * 1024 * 2 )
				) ;
			}
			else
			{
				// 鍵を解除
				if( DXAStream->NoKey == false ) DXA_KeyConv( DXAStream->DecodeTempBuffer, ( LONGLONG )DXAStream->FileHead->HuffPressDataSize, ( LONGLONG )DXAStream->FileHead->DataSize, DXAStream->Key ) ;

				// ハフマン圧縮データを解凍
				Huffman_Decode( DXAStream->DecodeTempBuffer, DXAStream->DecodeDataBuffer ) ;
			}
		}
	
		// メモリの解放
		DXFREE( DXAStream->DecodeTempBuffer ) ;
		DXAStream->DecodeTempBuffer = NULL ;

		// 状態を待機状態にする
		DXAStream->ASyncState = DXARC_STREAM_ASYNCSTATE_IDLE;
		return TRUE;

	case DXARC_STREAM_ASYNCSTATE_READ:			// 読み込み待ち

		// 読み込み終了待ち
		if( ReadOnlyFileAccessIdleCheck( DXAStream->ReadOnlyFilePointer ) == FALSE )
		{
			return FALSE;
		}

		// 読み込み終わったら鍵を解除
		if( DXAStream->NoKey == false ) DXA_KeyConv( DXAStream->ReadBuffer, ( LONGLONG )DXAStream->ReadSize, ( LONGLONG )( DXAStream->FileHead->DataSize + ( DXAStream->ASyncReadFileAddress - ( DXAStream->FileHead->DataAddress + DXAStream->Archive->Head.DataStartAddress ) ) ), DXAStream->Key ) ;

		// 状態を待機状態にする
		DXAStream->ASyncState = DXARC_STREAM_ASYNCSTATE_IDLE;
		return TRUE;
	}

	return TRUE ;
}

// ファイルのサイズを取得する
extern	LONGLONG DXA_STREAM_Size( DXARC_STREAM *DXAStream )
{
	return ( LONGLONG )DXAStream->FileHead->DataSize ;
}







// フルパスではないパス文字列をフルパスに変換する
static int DXA_DIR_ConvertFullPath( const wchar_t *Src, wchar_t *Dest, size_t BufferBytes, int CharUp )
{
	int Result ;

	Result = ConvertFullPathW_( Src, Dest, BufferBytes, NULL ) ;
	if( CharUp )
	{
		CL_strupr( WCHAR_T_CHARCODEFORMAT, ( char * )Dest ) ;
	}

	return Result ;

#if 0
	int     i, k ;
	wchar_t iden[ FILEPATH_MAX ] ;
	wchar_t CurrentDir[ FILEPATH_MAX ] ;
	DWORD   CharCode1 ;
	int     CharBytes1 ;
	DWORD   CharCode2 ;
	int     CharBytes2 ;
	DWORD   LastCharCode ;
	int     LastCharBytes ;
	char *  LastCharAddress ;
	int     CharNum ;
	size_t	DestBytes ;

	// カレントディレクトリを得る
	_WGETCWD( CurrentDir, sizeof( CurrentDir ) ) ;
	_WCSUPR( CurrentDir ) ;
	if( Src == NULL )
	{
		_WCSCPY_S( Dest, BufferBytes, CurrentDir ) ;
		goto END ;
	}

	i = 0 ;
	k = 0 ;

	DestBytes = 0 ;

	// １文字目と２文字目を取得
	CharCode1 = GetCharCode( ( const char * )&( ( BYTE * )Src )[ 0 ], WCHAR_T_CHARCODEFORMAT, &CharBytes1 ) ;
	CharCode2 = 0 ;
	if( CharCode1 != 0 )
	{
		CharCode2 = GetCharCode( ( const char * )&( ( BYTE * )Src )[ CharBytes1 ], WCHAR_T_CHARCODEFORMAT, &CharBytes2 ) ;
	}

	// 最初に『\』又は『/』が２回連続で続いている場合はネットワークを介していると判断

	if( ( CharCode1 == '\\' && CharCode2 == '\\' ) ||
		( CharCode1 == '/'  && CharCode2 == '/'  ) )
	{
		DestBytes += PutCharCode( '\\', WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )Dest )[ DestBytes ], BufferBytes - DestBytes ) ;
		PutCharCode( '\0', WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )Dest )[ DestBytes ], BufferBytes - DestBytes ) ;

		i += CharBytes1 + CharBytes2 ;
	}
	else
	// 最初が『\』又は『/』の場合はカレントドライブのルートディレクトリまで落ちる
	if( CharCode1 == '\\' || CharCode1 == '/' )
	{
		DWORD CurCharCode1 ;
		DWORD CurCharCode2 ;
		int   CurCharBytes1 ;
		int   CurCharBytes2 ;

		CurCharCode1 = GetCharCode( ( char * )&( ( BYTE * )CurrentDir )[ 0             ], WCHAR_T_CHARCODEFORMAT, &CurCharBytes1 ) ;
		CurCharCode2 = GetCharCode( ( char * )&( ( BYTE * )CurrentDir )[ CurCharBytes1 ], WCHAR_T_CHARCODEFORMAT, &CurCharBytes2 ) ;
		DestBytes += PutCharCode( CurCharCode1, WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )Dest )[ DestBytes ], BufferBytes - DestBytes ) ;
		DestBytes += PutCharCode( CurCharCode2, WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )Dest )[ DestBytes ], BufferBytes - DestBytes ) ;
		PutCharCode( '\0', WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )Dest )[ DestBytes ], BufferBytes - DestBytes ) ;

		i += CharBytes1 ;
	}
	else
	// ドライブ名が書かれていたらそのドライブへ
	if( CharCode2 == ':' )
	{
		DestBytes += PutCharCode( CharUp ? CHARUP( CharCode1 ) : CharCode1, WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )Dest )[ DestBytes ], BufferBytes - DestBytes ) ;
		DestBytes += PutCharCode(                  CharCode2              , WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )Dest )[ DestBytes ], BufferBytes - DestBytes ) ;
		PutCharCode( '\0', WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )Dest )[ DestBytes ], BufferBytes - DestBytes ) ;

		i += CharBytes1 + CharBytes2 ;

		// : の後の \ マークは飛ばす
		CharCode1 = GetCharCode( ( const char * )&( ( BYTE * )Src )[ i ], WCHAR_T_CHARCODEFORMAT, &CharBytes1 ) ;
		if( CharCode1 == '\\' )
		{
			i += CharBytes1 ;
		}
	}
	else
	// それ以外の場合はカレントディレクトリ
	{
		_WCSCPY_S( Dest, BufferBytes, CurrentDir ) ;
		DestBytes += _WCSLEN( Dest ) * sizeof( wchar_t ) ;

		CharNum = GetStringCharNum(  ( const char * )CurrentDir, WCHAR_T_CHARCODEFORMAT ) ;
		if( CharNum != 0 )
		{
			LastCharAddress = ( char * )GetStringCharAddress( ( const char * )CurrentDir, WCHAR_T_CHARCODEFORMAT, CharNum - 1 ) ;
			LastCharCode    = GetCharCode( LastCharAddress, WCHAR_T_CHARCODEFORMAT, &LastCharBytes ) ;
			if( LastCharCode == '\\' || LastCharCode == '/' )
			{
				PutCharCode( '\0', WCHAR_T_CHARCODEFORMAT, LastCharAddress, BUFFERBYTES_CANCEL ) ;
				DestBytes -= LastCharBytes ;
			}
		}
	}

	for(;;)
	{
		CharCode1 = GetCharCode( ( const char * )&( ( BYTE * )Src )[ i ], WCHAR_T_CHARCODEFORMAT, &CharBytes1 ) ;
		switch( CharCode1 )
		{
		case '\0' :
			if( k != 0 )
			{
				CharCode2 = GetCharCode( ( const char * )&( ( BYTE * )Dest )[ 0 ], WCHAR_T_CHARCODEFORMAT, &CharBytes2 ) ;
				if( CharCode2 != '\0' )
				{
					DestBytes += PutCharCode( '\\', WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )Dest )[ DestBytes ], BufferBytes - DestBytes ) ;
				}
				_WCSCPY_S( ( wchar_t * )&( ( BYTE * )Dest )[ DestBytes ], BufferBytes - DestBytes, iden ) ;
				DestBytes = _WCSLEN( Dest ) * sizeof( wchar_t ) ;
				k  = 0 ;
			}
			goto END ;

		case '\\' :
		case '/' :
			// 文字列が無かったらスキップ
			if( k == 0 )
			{
				i += CharBytes1 ;
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
				PutCharCode( '\0', WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )Dest )[ DestBytes ], BufferBytes - DestBytes ) ;
				CharNum         = GetStringCharNum(  ( const char * )Dest, WCHAR_T_CHARCODEFORMAT ) ;
				LastCharAddress = ( char * )GetStringCharAddress( ( const char * )Dest, WCHAR_T_CHARCODEFORMAT, CharNum - 1 ) ;
				LastCharCode    = GetCharCode( LastCharAddress, WCHAR_T_CHARCODEFORMAT, &LastCharBytes ) ;
				DestBytes -= LastCharBytes ;
				for(;;)
				{
					if( LastCharCode == '\\' || LastCharCode == '/' || LastCharCode == ':' )
					{
						break ;
					}

					PutCharCode( '\0', WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )Dest )[ DestBytes ], BufferBytes - DestBytes ) ;
					CharNum -- ;
					LastCharAddress = ( char * )GetStringCharAddress( ( const char * )Dest, WCHAR_T_CHARCODEFORMAT, CharNum - 1 ) ;
					LastCharCode    = GetCharCode( LastCharAddress, WCHAR_T_CHARCODEFORMAT, &LastCharBytes ) ;
					DestBytes -= LastCharBytes ;
				}

				if( LastCharCode != ':' )
				{
					PutCharCode( '\0', WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )Dest )[ DestBytes ], BufferBytes - DestBytes ) ;
				}
				else
				{
					if( BufferBytes - DestBytes > ( size_t )LastCharBytes )
					{
						DestBytes += LastCharBytes ;
					}
				}
			}
			else
			{
				CharCode2 = GetCharCode( ( const char * )&( ( BYTE * )Dest )[ 0 ], WCHAR_T_CHARCODEFORMAT, &CharBytes2 ) ;
				if( CharCode2 != '\0' )
				{
					DestBytes += PutCharCode( '\\', WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )Dest )[ DestBytes ], BufferBytes - DestBytes ) ;
				}
				_WCSCPY_S( ( wchar_t * )&( ( BYTE * )Dest )[ DestBytes ], BufferBytes - DestBytes, iden ) ;
				DestBytes = _WCSLEN( Dest ) * sizeof( wchar_t ) ;
			}

			k = 0 ;
			i += CharBytes1 ;
			break ;
		
		default :
			k += PutCharCode( CharUp ? CHARUP( CharCode1 ) : CharCode1, WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )iden )[ k ], sizeof( iden ) - k ) ;
			     PutCharCode( '\0',                                     WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )iden )[ k ], sizeof( iden ) - k ) ;

			i += CharBytes1 ;
			break ;
		}
	}
	
END :
	// 正常終了
	return 0 ;
#endif
}

// ファイル名も一緒になっていると分かっているパス中からファイル名とディレクトリパスを分割する
// フルパスである必要は無い、ファイル名だけでも良い
// DirPath の終端に ￥ マークは付かない
static int DXA_DIR_AnalysisFileNameAndDirPath( DXARC *DXA, const BYTE *Src, BYTE *FileName, size_t FileNameBytes, BYTE *DirPath, size_t DirPathBytes )
{
	int   i ;
	int   Last ;
	int   LastCharBytes = 0 ;
	DWORD CharCode ;
	int   CharBytes ;
	
	// ファイル名を抜き出す
	i    = 0 ;
	Last = -1 ;
	for(;;)
	{
		CharCode = GetCharCode( ( const char * )&Src[ i ], DXA->CharCodeFormat, &CharBytes ) ;
		if( CharCode == '\0' )
		{
			break ;
		}

		if( CharCode == '\\' || CharCode == '/' || CharCode == '\0' || CharCode == ':' )
		{
			Last          = i ;
			LastCharBytes = CharBytes ;
		}

		i += CharBytes ;
	}

	if( FileName != NULL )
	{
		if( Last != -1 )
		{
			if( FileNameBytes > ( size_t )( Last + LastCharBytes ) )
			{
				CL_strncpy_s( DXA->CharCodeFormat, ( char * )FileName, FileNameBytes, ( const char * )&Src[ Last + LastCharBytes ], ( int )( ( FileNameBytes - ( Last + LastCharBytes ) ) / GetCharCodeFormatUnitSize( DXA->CharCodeFormat ) ) ) ;
			}
		}
		else
		{
			CL_strncpy_s( DXA->CharCodeFormat, ( char * )FileName, FileNameBytes, ( const char * )Src, ( int )( FileNameBytes / GetCharCodeFormatUnitSize( DXA->CharCodeFormat ) ) ) ;
		}
	}
	
	// ディレクトリパスを抜き出す
	if( DirPath != NULL )
	{
		if( Last != -1 )
		{
			if( DirPathBytes > ( size_t )Last + 1 )
			{
				_MEMCPY( DirPath, Src, ( size_t )Last ) ;
				PutCharCode( '\0', DXA->CharCodeFormat, ( char * )&DirPath[ Last ], BUFFERBYTES_CANCEL ) ;
			}
		}
		else
		{
			PutCharCode( '\0', DXA->CharCodeFormat, ( char * )&DirPath[ 0 ], DirPathBytes ) ;
		}
	}
	
	// 終了
	return 0 ;
}

// CmpStr の条件に Src が適合するかどうかを調べる( 0:適合する  -1:適合しない )
static int DXA_DIR_FileNameCmp( DXARC *DXA, const BYTE *Src, const BYTE *CmpStr )
{
	const BYTE *s, *c ;
	DWORD SrcCharCode ;
	int   SrcCharBytes ;
	DWORD CmpCharCode ;
	int   CmpCharBytes ;

	s = Src ;
	c = CmpStr ;
	for(;;)
	{
		SrcCharCode = GetCharCode( ( const char * )s, DXA->CharCodeFormat, &SrcCharBytes ) ;
		CmpCharCode = GetCharCode( ( const char * )c, DXA->CharCodeFormat, &CmpCharBytes ) ;

		if( SrcCharCode != '\0' && CmpCharCode == '\0' )
		{
			return -1 ;
		}

		if( SrcCharCode == '\0' && CmpCharCode != '\0' )
		{
			if( CmpCharCode != '*' )
			{
				return -1 ;
			}

			while( CmpCharCode == '*' )
			{
				c += CmpCharBytes ;
				CmpCharCode = GetCharCode( ( const char * )c, DXA->CharCodeFormat, &CmpCharBytes ) ;
			}
			if( CmpCharCode != '\0' )
			{
				return -1 ;
			}

			break ;
		}

		if( SrcCharCode == '\0' && CmpCharCode == '\0' )
		{
			break ;
		}

		switch( CmpCharCode )
		{
		case '?':
			c += CmpCharBytes ;
			s += SrcCharBytes ;
			break;

		case '*':
			while( CmpCharCode == '*' )
			{
				c += CmpCharBytes ;
				CmpCharCode = GetCharCode( ( const char * )c, DXA->CharCodeFormat, &CmpCharBytes ) ;
			}
			if( CmpCharCode == '\0' )
			{
				return 0 ;
			}

			while( SrcCharCode != '\0' && SrcCharCode != CmpCharCode )
			{
				s += SrcCharBytes ;
				SrcCharCode = GetCharCode( ( const char * )s, DXA->CharCodeFormat, &SrcCharBytes ) ;
			}
			if( SrcCharCode == '\0' )
			{
				return -1 ;
			}

			c += CmpCharBytes ;
			s += SrcCharBytes ;
			break;

		default:
			if( CmpCharCode != SrcCharCode )
			{
				return -1 ;
			}

			c += CmpCharBytes ;
			s += SrcCharBytes ;
			break;
		}

		if( ( CmpCharCode == '\0' && SrcCharCode != '\0' ) ||
			( CmpCharCode != '\0' && SrcCharCode == '\0' ) )
		{
			return -1 ;
		}
	}

	return 0 ;
}

// アーカイブファイルをフォルダに見立ててファイルを開く時の情報を得る( -1:アーカイブとしては存在しなかった  0:存在した )
static int DXA_DIR_OpenTest( const wchar_t *FilePath, int *ArchiveIndex, BYTE *ArchiveFilePath, size_t BufferBytes )
{
	int   i ;
	int   len ;
	int   arcindex ;
	wchar_t fullpath[ FILEPATH_MAX ] ;
	wchar_t fullpath_up[ FILEPATH_MAX ] ;
	wchar_t path[ FILEPATH_MAX ] ;
	wchar_t temp[ FILEPATH_MAX ] ;
	wchar_t dir[ FILEPATH_MAX ] ;
	wchar_t *p ;
	int   BackUseDirectoryPathCharValid ;
	DWORD BackUseDirectoryPathCharCode = 0 ;
	int   BackUseDirectoryPathCharBytes = 0 ;

	// フルパスを得る
	if( DXARCD.NotArchivePathCharUp )
	{
		DXA_DIR_ConvertFullPath( FilePath, fullpath, sizeof( fullpath ), 0 ) ;
		CL_strcpy( WCHAR_T_CHARCODEFORMAT, ( char * )fullpath_up, ( char * )fullpath ) ;
		CL_strupr( WCHAR_T_CHARCODEFORMAT, ( char * )fullpath_up ) ;
	}
	else
	{
		DXA_DIR_ConvertFullPath( FilePath, fullpath, sizeof( fullpath ), 1 ) ;
	}

	// 前回と使用するアーカイブのパスが同じ場合は同じアーカイブを使用する
	BackUseDirectoryPathCharValid = FALSE ;
	if( DXARCD.BackUseDirectoryPathLength != 0 && _MEMCMP( fullpath, DXARCD.BackUseDirectory, DXARCD.BackUseDirectoryPathLength ) == 0 )
	{
		BackUseDirectoryPathCharValid = TRUE ;
		BackUseDirectoryPathCharCode  = GetCharCode( ( const char * )&( ( BYTE * )fullpath )[ DXARCD.BackUseDirectoryPathLength ], WCHAR_T_CHARCODEFORMAT, &BackUseDirectoryPathCharBytes ) ;
	}
	if( BackUseDirectoryPathCharValid &&
		( BackUseDirectoryPathCharCode == '\\' || BackUseDirectoryPathCharCode == '/' ) )
	{
		// 前回使用したＤＸＡファイルを開く
		arcindex = DXA_DIR_OpenArchive( DXARCD.BackUseDirectory, NULL, -1, FALSE, FALSE, DXARCD.BackUseArchiveIndex ) ;
		if( arcindex == -1 )
		{
			return -1 ;
		}

		// ＤＸＡファイルがある場所以降のパスを作成する
		if( DXARCD.NotArchivePathCharUp )
		{
			p = ( wchar_t * )&( ( BYTE * )fullpath_up )[ DXARCD.BackUseDirectoryPathLength + BackUseDirectoryPathCharBytes ] ;
		}
		else
		{
			p = ( wchar_t * )&( ( BYTE * )fullpath    )[ DXARCD.BackUseDirectoryPathLength + BackUseDirectoryPathCharBytes ] ;
		}
	}
	else
	{
		DWORD CharCode1 ;
		int   CharBytes1 ;
		DWORD CharCode2 ;
		int   CharBytes2 = 0 ;

		// 前回とは違うパスの場合は一から調べる

		// ディレクトリを一つ一つ追って行く
		p   = fullpath ;
		len = 0 ;
		for(;;)
		{
			// ネットワークを介していた場合の専用処理
			if( p - fullpath == 0 )
			{
				// fullpath の１文字目と２文字目を取得
				CharCode1 = GetCharCode( ( const char * )&( ( BYTE * )fullpath )[ 0 ], WCHAR_T_CHARCODEFORMAT, &CharBytes1 ) ;
				CharCode2 = 0 ;
				if( CharCode1 != 0 )
				{
					CharCode2 = GetCharCode( ( const char * )&( ( BYTE * )fullpath )[ CharBytes1 ], WCHAR_T_CHARCODEFORMAT, &CharBytes2 ) ;
				}

				if( CharCode1 == '\\' && CharCode2 == '\\' )
				{
					len += PutCharCode( '\\', WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )path )[ len ], sizeof( path ) - len ) ;
					len += PutCharCode( '\\', WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )path )[ len ], sizeof( path ) - len ) ;
					       PutCharCode( '\0', WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )path )[ len ], sizeof( path ) - len ) ;

					p = ( wchar_t * )( ( BYTE * )p + CharBytes1 + CharBytes2 ) ;
				}
			}

			// ディレクトリを一つ取る
			i = 0 ;
			for(;;)
			{
				CharCode1 = GetCharCode( ( const char * )p, WCHAR_T_CHARCODEFORMAT, &CharBytes1 ) ;
				p = ( wchar_t * )( ( BYTE * )p + CharBytes1 ) ;
				if( ( wchar_t * )( ( BYTE * )p - CharBytes1 ) != fullpath && ( CharCode1 == '\0' || CharCode1 == '/' || CharCode1 == '\\' ) )
				{
					break ;
				}

				len += PutCharCode( CharCode1, WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )path )[ len ], sizeof( path ) - len ) ;
				i   += PutCharCode( CharCode1, WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )dir  )[ i   ], sizeof( dir  ) - i ) ;

			}

			if( CharCode1 == '\0' || i == 0 )
			{
				return -1 ;
			}

			PutCharCode( '\0', WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )path )[ len ], sizeof( path ) - len ) ;
			PutCharCode( '\0', WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )dir  )[ i   ], sizeof( dir  ) - i   ) ;

			// フォルダ名をDXアーカイブファイル名にする
			{
				int TempLen ;

				_MEMCPY( temp, path, len ) ;
				TempLen = len ;

				TempLen += PutCharCode( '.', WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )temp )[ TempLen ], sizeof( temp ) - TempLen ) ;
				if( DXARCD.ArchiveExtensionLength == 0 )
				{
					if( DXARCD.NotArchivePathCharUp )
					{
						TempLen += PutCharCode( 'd',  WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )temp )[ TempLen ], sizeof( temp ) - TempLen ) ;
						TempLen += PutCharCode( 'x',  WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )temp )[ TempLen ], sizeof( temp ) - TempLen ) ;
						TempLen += PutCharCode( 'a',  WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )temp )[ TempLen ], sizeof( temp ) - TempLen ) ;
					}
					else
					{
						TempLen += PutCharCode( 'D',  WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )temp )[ TempLen ], sizeof( temp ) - TempLen ) ;
						TempLen += PutCharCode( 'X',  WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )temp )[ TempLen ], sizeof( temp ) - TempLen ) ;
						TempLen += PutCharCode( 'A',  WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )temp )[ TempLen ], sizeof( temp ) - TempLen ) ;
					}
					TempLen += PutCharCode( '\0', WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )temp )[ TempLen ], sizeof( temp ) - TempLen ) ;
				}
				else
				{
					_WCSCPY_S( ( wchar_t * )&( ( BYTE * )temp )[ TempLen ], sizeof( temp ) - TempLen, DXARCD.ArchiveExtension ) ;
				}
			}

			// ＤＸＡファイルとして開いてみる
			arcindex = DXA_DIR_OpenArchive( temp ) ;
			if( arcindex != -1 )
			{
				break ;
			}

			// 開けなかったら次の階層へ
			len += PutCharCode( '\\', WCHAR_T_CHARCODEFORMAT, ( char * )&( ( BYTE * )path )[ len ], sizeof( path ) - len ) ;
		}

		// 開けたら今回の情報を保存する
		_WCSCPY_S( DXARCD.BackUseDirectory, sizeof( DXARCD.BackUseDirectory ), temp ) ;
//		if( DXARCD.ArchiveExtensionLength == 0 )
//		{
//			_MEMCPY( DXARCD.BackUseDirectory, temp, len + ( 1 /* . */ + 3 /* DXA */ + 1 /* '\0' */ ) * sizeof( wchar_t ) ) ;
//		}
//		else
//		{
//			_MEMCPY( DXARCD.BackUseDirectory, temp, len + ( 1 /* . */ + DXARCD.ArchiveExtensionLength + 1 /* '\0' */ ) * sizeof( wchar_t ) ) ;
//		}
		DXARCD.BackUseDirectoryPathLength = len ;
		DXARCD.BackUseArchiveIndex        = arcindex ;

		if( DXARCD.NotArchivePathCharUp )
		{
			p = fullpath_up + ( p - fullpath ) ;
		}
	}

	// 情報をセットする
	*ArchiveIndex = arcindex;

	if( ArchiveFilePath )
	{
		int DestCharCodeFormat ;

		DestCharCodeFormat = DXARCD.Archive[ arcindex ]->Archive.CharCodeFormat ;
		ConvString( ( const char * )p, -1, WCHAR_T_CHARCODEFORMAT, ( char * )ArchiveFilePath, BufferBytes, DestCharCodeFormat ) ;
	}

	// 終了
	return 0;
}

// アーカイブファイルを開く
static int DXA_DIR_OpenArchive( const wchar_t *FilePath, void *FileImage, int FileSize, int FileImageCopyFlag, int FileImageReadOnly, int ArchiveIndex, int OnMemory, int ASyncThread )
{
	int					i ;
	int					index ;
	int					newindex ;
	DXARC_DIR_ARCHIVE *	arc ;
	DXARC_DIR_ARCHIVE *	tarc ;
	DXARC				temparc ;

	// アーカイブの指定がある場合はそのまま使用する
	if( ArchiveIndex != -1 )
	{
		tarc = DXARCD.Archive[ ArchiveIndex ] ;
		if( tarc != NULL )
		{
			if(	_WCSCMP( FilePath, tarc->Path ) == 0 )
			{
				DXARCD.Archive[ ArchiveIndex ]->UseCounter ++ ;
				return ArchiveIndex ;
			}
		}
	}

	// 既に開かれているか調べる
	newindex = -1 ;
	index    = 0 ;
	for( i = 0 ; i < DXARCD.ArchiveNum ; index ++ )
	{
		arc = DXARCD.Archive[ index ] ;
		if( arc == NULL )
		{
			newindex = index ;
			continue ;
		}
		
		i ++ ;

		if( _WCSCMP( arc->Path, FilePath ) == 0 )
		{
			// 既に開かれていた場合はそのインデックスを返す
			arc->UseCounter ++ ;
			return index ;
		}
	}
	
	// なかった場合は新規にデータを追加する

	// ハンドルが既に一杯の場合は使用されていないアーカイブハンドルを解放する
	if( DXARCD.ArchiveNum == DXA_DIR_MAXARCHIVENUM )
	{
		// 未使用のハンドルを解放
		DXA_DIR_CloseWaitArchive() ;
		
		// それでも一杯である場合はエラー
		if( DXARCD.ArchiveNum == DXA_DIR_MAXARCHIVENUM )
		{
			return -1 ;
		}
	} 
	if( newindex == -1 )
	{
		for( newindex = 0 ; DXARCD.Archive[ newindex ] != NULL ; newindex ++ ){}
	}

	// アーカイブファイルが存在しているか確認がてら初期化する
	DXA_Initialize( &temparc ) ;
	if( FileImage != NULL )
	{
		// メモリ上に展開されたファイルイメージを使用する場合
		if( DXA_OpenArchiveFromMem( &temparc, FileImage, FileSize, FileImageCopyFlag, FileImageReadOnly, DXARCD.ValidKeyString == TRUE ? DXARCD.KeyString : NULL, FilePath ) < 0 )
			return -1 ;
	}
	else
	if( OnMemory == TRUE )
	{
		// メモリに読み込む場合
		if( DXA_OpenArchiveFromFileUseMem( &temparc, FilePath, DXARCD.ValidKeyString == TRUE ? DXARCD.KeyString : NULL, ASyncThread ) < 0 )
			return -1 ;
	}
	else
	{
		// ファイルから読み込む場合
		if( DXA_OpenArchiveFromFile( &temparc, FilePath, DXARCD.ValidKeyString == TRUE ? DXARCD.KeyString : NULL ) < 0 )
			return -1 ;
	}

	// 新しいアーカイブデータ用のメモリを確保する
	arc = DXARCD.Archive[ newindex ] = ( DXARC_DIR_ARCHIVE * )DXALLOC( sizeof( DXARC_DIR_ARCHIVE ) ) ;
	if( DXARCD.Archive[ newindex ] == NULL )
	{
		DXA_CloseArchive( &temparc ) ;
		DXA_Terminate( &temparc ) ;
		return -1 ;
	}

	// 情報セット
	_MEMCPY( &arc->Archive, &temparc, sizeof( DXARC ) ) ;
	arc->UseCounter = 1 ;
	_WCSCPY_S( arc->Path, sizeof( arc->Path ), FilePath ) ;

	// 使用中のアーカイブの数を増やす
	DXARCD.ArchiveNum ++ ;

	// インデックスを返す
	return newindex ;
}

// 既に開かれているアーカイブのハンドルを取得する( 戻り値: -1=無かった 0以上:ハンドル )
static int DXA_DIR_GetArchive( const wchar_t *FilePath, void *FileImage )
{
	int i, index ;
	DXARC_DIR_ARCHIVE *arc ;

	index = 0 ;
	for( i = 0 ; i < DXARCD.ArchiveNum ; index ++ )
	{
		arc = DXARCD.Archive[index] ;
		if( arc == NULL )
		{
			continue ;
		}

		i ++ ;

		if( FilePath )
		{
			if( _WCSCMP( arc->Path, FilePath ) == 0 )
			{
				return index ;
			}
		}
		else
		{
			if( arc->Archive.MemoryImageCopyFlag )
			{
				if( arc->Archive.MemoryImageOriginal == FileImage )
				{
					return index ;
				}
			}
			else
			{
				if( arc->Archive.MemoryImage == FileImage )
				{
					return index ;
				}
			}
		}
	}

	return -1 ;
}

// アーカイブファイルを閉じる
static int DXA_DIR_CloseArchive( int ArchiveHandle )
{
	DXARC_DIR_ARCHIVE *arc ;

	// 使用されていなかったら何もせず終了
	arc = DXARCD.Archive[ArchiveHandle] ;
	if( arc == NULL || arc->UseCounter == 0 )
	{
		return -1 ;
	}

	// 参照カウンタを減らす
	arc->UseCounter -- ;

	// 終了
	return 0 ;
}

// 使用されるのを待っているアーカイブファイルを全て閉じる
static void DXA_DIR_CloseWaitArchive( void )
{
	int i, Num, index ;
	DXARC_DIR_ARCHIVE *arc ;
	
	Num = DXARCD.ArchiveNum ;
	for( i = 0, index = 0 ; i < Num ; index ++ )
	{
		if( DXARCD.Archive[index] == NULL )
		{
			continue ;
		}
		i ++ ;

		arc = DXARCD.Archive[index] ;

		// 使われていたら解放しない
		if( arc->UseCounter > 0 )
		{
			continue ;
		}

		// 後始末
		DXA_CloseArchive( &arc->Archive ) ;
		DXA_Terminate( &arc->Archive ) ;
		DXFREE( arc ) ;
		DXARCD.Archive[index] = NULL ;
		
		// アーカイブの数を減らす
		DXARCD.ArchiveNum -- ;
	}
}

// アーカイブをディレクトリに見立てる処理の初期化
extern	int DXA_DIR_Initialize( void )
{
	// 既に初期化済みの場合は何もしない
	if( DXARCD.InitializeFlag )
	{
		return -1 ;
	}

	// クリティカルセクションの初期化
	CriticalSection_Initialize( &DXARCD.CriticalSection ) ;

//	_MEMSET( &DXARCD, 0, sizeof( DXARC_DIR ) ) ;
//	DXA_DIR_Terminate() ;

	// 使用されていないアーカイブファイルを解放する
	DXA_DIR_CloseWaitArchive() ;

	// 初期化完了フラグを立てる
	DXARCD.InitializeFlag = TRUE ;

	// 終了
	return 0 ;
}


// アーカイブをディレクトリに見立てる処理の後始末
extern int DXA_DIR_Terminate( void )
{
	// 既に後始末済みの場合は何もしない
	if( DXARCD.InitializeFlag == FALSE )
	{
		return -1 ;
	}

	// 使用されていないアーカイブファイルを解放する
	DXA_DIR_CloseWaitArchive() ;

	// クリティカルセクションの後始末
	CriticalSection_Delete( &DXARCD.CriticalSection ) ;

	// 初期化完了フラグを倒す
	DXARCD.InitializeFlag = FALSE ;

	// 終了
	return 0 ;
}

// アーカイブファイルのパスを大文字にしないかどうかのフラグをセットする
extern int DXA_DIR_SetNotArchivePathCharUp( int NotArchivePathCharUpFlag )
{
	DXARCD.NotArchivePathCharUp = NotArchivePathCharUpFlag ;

	// 終了
	return 0 ;
}

// アーカイブファイルの拡張子を設定する
extern int DXA_DIR_SetArchiveExtension( const wchar_t *Extension )
{
	int Length ;
	
	Length = ( int )_WCSLEN( Extension ) ;

	if( Length >= 64 || Extension == NULL || Extension[0] == _T( '\0' ) )
	{
		DXARCD.ArchiveExtension[ 0 ]  = 0 ;
		DXARCD.ArchiveExtensionLength = 0 ;
	}
	else
	{
		DXARCD.ArchiveExtensionLength = Length ;
		_WCSCPY_S( DXARCD.ArchiveExtension, sizeof( DXARCD.ArchiveExtension ), Extension ) ;
	}

	// 終了
	return 0 ;
}

// アーカイブファイルと通常のフォルダのどちらも存在した場合、どちらを優先させるかを設定する( 1:フォルダを優先 0:ＤＸアーカイブファイルを優先(デフォルト) )
extern int DXA_DIR_SetDXArchivePriority( int Priority )
{
	DXARCD.DXAPriority = Priority ;

	// 終了
	return 0 ;
}

// アーカイブファイルの鍵文字列を設定する
extern int DXA_DIR_SetKeyString( const char *KeyString )
{
	if( KeyString == NULL )
	{
		DXARCD.ValidKeyString = FALSE ;
	}
	else
	{
		DXARCD.ValidKeyString = TRUE ;
		if( _STRLEN( KeyString ) > DXA_KEY_STRING_LENGTH )
		{
			_MEMCPY( DXARCD.KeyString, KeyString, DXA_KEY_STRING_LENGTH ) ;
			DXARCD.KeyString[ DXA_KEY_STRING_LENGTH ] = '\0' ;
		}
		else
		{
			_STRCPY( DXARCD.KeyString, KeyString ) ;
		}
	}

	// 終了
	return 0 ;
}

// ファイルを丸ごと読み込む関数
extern LONGLONG DXA_DIR_LoadFile( const wchar_t *FilePath, void *Buffer, int BufferSize )
{
	LONGLONG siz ;
	DWORD_PTR handle ;

	handle = DXA_DIR_Open( FilePath ) ;
	if( handle == 0 )
	{
		return false ;
	}

	DXA_DIR_Seek( handle, 0L, SEEK_END ) ;
	siz = DXA_DIR_Tell( handle ) ;
	DXA_DIR_Seek( handle, 0L, SEEK_SET ) ;

	if( siz <= BufferSize )
	{
		DXA_DIR_Read( Buffer, ( size_t )siz, 1, handle ) ;
	}

	DXA_DIR_Close( handle ) ;

	// 終了
	return siz ;
}

// DXA_DIR_Open の基本関数
extern DWORD_PTR DXA_DIR_Open( const wchar_t *FilePath, int UseCacheFlag, int /*BlockReadFlag*/, int UseASyncReadFlag )
{
	int index ;
	DXARC_DIR_FILE *file ;
//	char DXAErrorStr[ FILEPATH_MAX ] ;
	BYTE DxaInFilePath[ FILEPATH_MAX ] ;

	// 初期化されていなかったら初期化する
	if( DXARCD.InitializeFlag == FALSE )
	{
		DXA_DIR_Initialize() ;
	}

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &DXARCD.CriticalSection ) ;

//	UseCacheFlag  = UseCacheFlag ;
//	BlockReadFlag = BlockReadFlag ;
//	DXAErrorStr[ 0 ] = 0 ;

	// 空きデータを探す
	if( DXARCD.FileNum == DXA_DIR_MAXFILENUM )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &DXARCD.CriticalSection ) ;

		DXST_LOGFILE_ADDUTF16LE( "\x0c\x54\x42\x66\x6b\x30\x8b\x95\x51\x30\x8b\x30\xd5\x30\xa1\x30\xa4\x30\xeb\x30\x6e\x30\x70\x65\x4c\x30\x50\x96\x4c\x75\x92\x30\x85\x8d\x48\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"同時に開けるファイルの数が限界を超えました\n" @*/ ) ;
		return 0 ;
	}
	for( index = 0 ; DXARCD.File[index] != NULL ; index ++ ){}

	// メモリの確保
	DXARCD.File[ index ] = (DXARC_DIR_FILE *)DXALLOC( sizeof( DXARC_DIR_FILE ) ) ;
	if( DXARCD.File[ index ] == NULL )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &DXARCD.CriticalSection ) ;

		DXST_LOGFILE_ADDUTF16LE( "\xd5\x30\xa1\x30\xa4\x30\xeb\x30\x6e\x30\xc5\x60\x31\x58\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ファイルの情報を格納するメモリの確保に失敗しました\n" @*/ ) ;
		return 0 ;
	}
	file = DXARCD.File[index] ;

	// アーカイブファイルとフォルダのどちらを優先するかで処理を分岐
	if( DXARCD.DXAPriority == 0 )
	{
		int FileOpen = FALSE ;

		// アーカイブを優先する場合

		// アーカイブファイルが無いか調べる
		if( DXA_DIR_OpenTest( FilePath, ( int * )&file->UseArchiveIndex, DxaInFilePath, sizeof( DxaInFilePath ) ) == 0 )
		{
			// ディレクトリ名と同名のＤＸＡファイルを開けたらその中から指定のファイルを読み込もうとする
			if( DXA_STREAM_Initialize( &file->DXAStream, &DXARCD.Archive[ file->UseArchiveIndex ]->Archive, DxaInFilePath, UseASyncReadFlag ) < 0 )
			{
//				_STRCPY( DXAErrorStr, DXSTRING( "ＤＸＡファイルの中に指定のファイルは見つかりませんでした\n" ) ) ;
				DXA_DIR_CloseArchive( ( int )file->UseArchiveIndex ) ;
			}
			else
			{
				// アーカイブを使用しているフラグを立てる
				file->UseArchiveFlag = 1 ;

				FileOpen = TRUE ;
			}
		}

		if( FileOpen == FALSE )
		{
			// アーカイブファイルが無かったら普通のファイルから読み込む設定を行う
			file->UseArchiveFlag = 0 ;

			// 普通のファイルが無いか調べる
			file->ReadOnlyFilePointer = ReadOnlyFileAccessOpen( FilePath, UseCacheFlag, TRUE, UseASyncReadFlag ) ;
			if( file->ReadOnlyFilePointer == 0 )
			{
				goto ERR ;
			}
			else
			{
				FileOpen = TRUE ;
			}
		}
	}
	else
	{
		// 普通のファイルを優先する場合

		// 普通のファイルが無いか調べる
		if( ( file->ReadOnlyFilePointer = ReadOnlyFileAccessOpen( FilePath, UseCacheFlag, TRUE, UseASyncReadFlag ) ) != 0 )
		{
			// 開いたら普通のファイルから読み込む設定を行う
			file->UseArchiveFlag = 0 ;
		}
		else
		{
			// アーカイブファイルが無いか調べる
			if( DXA_DIR_OpenTest( FilePath, (int *)&file->UseArchiveIndex, DxaInFilePath, sizeof( DxaInFilePath ) ) == 0 )
			{
				// ディレクトリ名と同名のＤＸＡファイルを開けたらその中から指定のファイルを読み込もうとする
				if( DXA_STREAM_Initialize( &file->DXAStream, &DXARCD.Archive[ file->UseArchiveIndex ]->Archive, DxaInFilePath, UseASyncReadFlag ) < 0 )
				{
//					_STRCPY( DXAErrorStr, DXSTRING( "ＤＸＡファイルの中に指定のファイルは見つかりませんでした\n" ) ) ;
					DXA_DIR_CloseArchive( ( int )file->UseArchiveIndex ) ;
					goto ERR ;
				}
				else
				{
					// アーカイブを使用しているフラグを立てる
					file->UseArchiveFlag = 1 ;
				}
			}
			else
			{
				// 無かったらエラー
				goto ERR;
			}
		}
	}

	// ハンドルの数を増やす
	DXARCD.FileNum ++ ;

	// クリティカルセクションの解放
	CriticalSection_Unlock( &DXARCD.CriticalSection ) ;

	// インデックスを返す
	return index | 0xF0000000 ;

ERR:
	// メモリの解放
	if( DXARCD.File[index] != NULL ) DXFREE( DXARCD.File[index] ) ;
	DXARCD.File[index] = NULL ;
	
	// エラー文字列出力
//	DXST_LOGFILEFMT_ADDW(( L"ファイル %s のオープンに失敗しました\n", FilePath )) ;
//	if( DXAErrorStr[0] != '\0' ) DXST_LOGFILEFMT_ADDW(( "ＤＸＡエラー：%s", DXAErrorStr )) ;

	// クリティカルセクションの解放
	CriticalSection_Unlock( &DXARCD.CriticalSection ) ;

	// エラー終了
	return 0 ;
}

// ファイルを閉じる
extern int DXA_DIR_Close( DWORD_PTR Handle )
{
	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &DXARCD.CriticalSection ) ;

	DXARC_DIR_FILE *file = DXARCD.File[Handle & 0x0FFFFFFF] ;

	// 使用されていなかったら何もせず終了
	if( file == NULL )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &DXARCD.CriticalSection ) ;

		return -1 ;
	}
	
	// アーカイブを使用しているかどうかで分岐
	if( file->UseArchiveFlag == FALSE )
	{
		// 使用していない場合は標準入出力のファイルポインタを解放する
		ReadOnlyFileAccessClose( file->ReadOnlyFilePointer ) ;
		file->ReadOnlyFilePointer = 0 ;
	}
	else
	{
		// アーカイブを使用していた場合はアーカイブの参照数を減らす
		
		// アーカイブファイルの参照数を減らす
		DXA_DIR_CloseArchive( ( int )file->UseArchiveIndex ) ;

		// アーカイブファイルの後始末
		DXA_STREAM_Terminate( &file->DXAStream ) ;
	}

	// メモリを解放する
	DXFREE( file ) ;
	DXARCD.File[Handle & 0x0FFFFFFF] = NULL ;
	
	// 数を減らす
	DXARCD.FileNum -- ;

	// クリティカルセクションの解放
	CriticalSection_Unlock( &DXARCD.CriticalSection ) ;

	// 終了
	return 0 ;
}

// ファイルポインタの位置を取得する
extern	LONGLONG DXA_DIR_Tell( DWORD_PTR Handle )
{
	DXARC_DIR_FILE *file = DXARCD.File[Handle & 0x0FFFFFFF] ;
	if( file == NULL )
	{
		return -1 ;
	}

	if( file->UseArchiveFlag == 0 )
	{
		return ReadOnlyFileAccessTell( file->ReadOnlyFilePointer ) ;
	}
	else
	{
		return DXA_STREAM_Tell( &file->DXAStream ) ;
	}
}

// ファイルポインタの位置を変更する
extern int DXA_DIR_Seek( DWORD_PTR Handle, LONGLONG SeekPoint, int SeekType )
{
	DXARC_DIR_FILE *file = DXARCD.File[Handle & 0x0FFFFFFF] ;
	if( file == NULL )
	{
		return -1 ;
	}

	if( file->UseArchiveFlag == 0 )
	{
		return ReadOnlyFileAccessSeek( file->ReadOnlyFilePointer, SeekPoint, SeekType ) ;
	}
	else
	{
		return DXA_STREAM_Seek( &file->DXAStream, SeekPoint, SeekType ) ;
	}
}

// ファイルからデータを読み込む
extern size_t DXA_DIR_Read( void *Buffer, size_t BlockSize, size_t BlockNum, DWORD_PTR Handle )
{
	DXARC_DIR_FILE *file = DXARCD.File[Handle & 0x0FFFFFFF] ;
	if( file == NULL )
	{
		return ( size_t )-1 ;
	}

	if( file->UseArchiveFlag == 0 )
	{
		return ReadOnlyFileAccessRead( Buffer, BlockSize, BlockNum, file->ReadOnlyFilePointer ) ;
	}
	else
	{
		return DXA_STREAM_Read( &file->DXAStream, Buffer, BlockSize * BlockNum ) / BlockSize ;
	}
}

// ファイルの終端を調べる
extern int DXA_DIR_Eof( DWORD_PTR Handle )
{
	DXARC_DIR_FILE *file = DXARCD.File[Handle & 0x0FFFFFFF] ;
	if( file == NULL )
	{
		return -1 ;
	}

	if( file->UseArchiveFlag == 0 )
	{
		return ReadOnlyFileAccessEof( file->ReadOnlyFilePointer ) ;
	}
	else
	{
		return DXA_STREAM_Eof( &file->DXAStream ) ;
	}
}

extern	int DXA_DIR_ChDir( const wchar_t *Path )
{
	_WCHDIR( Path ) ;

	return 0 ;
}

extern	int DXA_DIR_GetDir( wchar_t *Buffer )
{
	_WGETCWD( Buffer, BUFFERBYTES_CANCEL ) ;

	return 0 ;
}

extern	int DXA_DIR_GetDirS( wchar_t *Buffer, size_t BufferBytes )
{
	_WGETCWD( Buffer, BufferBytes ) ;

	return 0 ;
}

extern	int DXA_DIR_IdleCheck( DWORD_PTR Handle )
{
	DXARC_DIR_FILE *file = DXARCD.File[Handle & 0x0FFFFFFF] ;
	if( file == NULL )
	{
		return -1 ;
	}

	if( file->UseArchiveFlag == 0 )
	{
		return ReadOnlyFileAccessIdleCheck( file->ReadOnlyFilePointer ) ;
	}
	else
	{
		return DXA_STREAM_IdleCheck( &file->DXAStream ) ;
	}
}

// 戻り値: -1=エラー  -1以外=FindHandle
extern DWORD_PTR DXA_DIR_FindFirst( const wchar_t *FilePath, FILEINFOW *Buffer )
{
	DXA_DIR_FINDDATA *find ;
	BYTE nPath[ FILEPATH_MAX ] ;

	// メモリの確保
	find = ( DXA_DIR_FINDDATA * )DXALLOC( sizeof( DXA_DIR_FINDDATA ) ) ;
	if( find == NULL )
	{
		return ( DWORD_PTR )-1 ;
	}
	_MEMSET( find, 0, sizeof( *find ) ) ;

	// 指定のオブジェクトがアーカイブファイル内か調べる
	if( DXA_DIR_OpenTest( FilePath, &find->UseArchiveIndex, nPath, sizeof( nPath ) ) == -1 )
	{
		// アーカイブファイル内ではなかった場合はファイルから検索する
		find->UseArchiveFlag = 0 ;
		find->FindHandle = ReadOnlyFileAccessFindFirst( FilePath, Buffer ) ;
	}
	else
	{
		// アーカイブファイル内の場合はアーカイブファイル内から検索する
		find->UseArchiveFlag = 1 ;
		find->FindHandle = DXA_FindFirst( &DXARCD.Archive[ find->UseArchiveIndex ]->Archive, nPath, Buffer ) ;
	}

	// 検索ハンドルが取得できなかった場合はエラー
	if( find->FindHandle == ( DWORD_PTR )-1 )
	{
		// アーカイブファイル内の場合はアーカイブファイルの使用カウントを減らす
		if( find->UseArchiveFlag != 0 )
		{
			DXA_DIR_CloseArchive( find->UseArchiveIndex ) ;
		}

		DXFREE( find );
		return ( DWORD_PTR )-1 ;
	}

	// ハンドルを返す
	return (DWORD_PTR)find ;
}

// 戻り値: -1=エラー  0=成功
extern int DXA_DIR_FindNext( DWORD_PTR FindHandle, FILEINFOW *Buffer )
{
	DXA_DIR_FINDDATA *find;

	find = (DXA_DIR_FINDDATA *)FindHandle;
	if( find->UseArchiveFlag == 0 )
	{
		return ReadOnlyFileAccessFindNext( find->FindHandle, Buffer ) ;
	}
	else
	{
		return DXA_FindNext( find->FindHandle, Buffer ) ;
	}
}

// 戻り値: -1=エラー  0=成功
extern int DXA_DIR_FindClose( DWORD_PTR FindHandle )
{
	DXA_DIR_FINDDATA *find;

	find = (DXA_DIR_FINDDATA *)FindHandle;
	if( find->UseArchiveFlag == 0 )
	{
		ReadOnlyFileAccessFindClose( find->FindHandle ) ;
	}
	else
	{
		DXA_FindClose( find->FindHandle );
		DXA_DIR_CloseArchive( find->UseArchiveIndex ) ;
	}

	DXFREE( find );

	return 0;
}




// 指定のＤＸＡファイルを丸ごとメモリに読み込む( 戻り値: -1=エラー  0=成功 )
extern int NS_DXArchivePreLoad( const TCHAR *FilePath , int ASyncThread )
{
	wchar_t fullpath[ FILEPATH_MAX ] ;

	// フルパスを得る(ついでに全ての文字を大文字にする)
#ifdef UNICODE
	DXA_DIR_ConvertFullPath( FilePath, fullpath, sizeof( fullpath ), DXARCD.NotArchivePathCharUp == FALSE ? 1 : 0 ) ;
#else // UNICODE
	wchar_t FilePathBuffer[ FILEPATH_MAX ] ;

	ConvString( ( const char * )FilePath, -1, _TCHARCODEFORMAT, ( char * )FilePathBuffer, sizeof( FilePathBuffer ), WCHAR_T_CHARCODEFORMAT ) ;
	DXA_DIR_ConvertFullPath( FilePathBuffer, fullpath, sizeof( fullpath ), DXARCD.NotArchivePathCharUp == FALSE ? 1 : 0 ) ;
#endif // UNICODE

	return DXA_DIR_OpenArchive( fullpath, NULL, -1, FALSE, FALSE, -1, TRUE, ASyncThread ) == -1 ? -1 : 0 ;
}

// 指定のＤＸＡファイルを丸ごとメモリに読み込む( 戻り値  -1:エラー  0:成功 )
extern int NS_DXArchivePreLoadWithStrLen( const TCHAR *FilePath, size_t FilePathLength, int ASyncThread )
{
	int Result ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_ONE_BEGIN( FilePath, FilePathLength, return -1 )
	Result = NS_DXArchivePreLoad( UseFilePathBuffer , ASyncThread ) ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( FilePath )
	return Result ;
}

// 指定のＤＸＡファイルの事前読み込みが完了したかどうかを取得する( 戻り値： TRUE=完了した FALSE=まだ )
extern int NS_DXArchiveCheckIdle( const TCHAR *FilePath )
{
	int handle;
	wchar_t fullpath[FILEPATH_MAX];

	// フルパスを得る(ついでに全ての文字を大文字にする)
#ifdef UNICODE
	DXA_DIR_ConvertFullPath( FilePath, fullpath, sizeof( fullpath ), DXARCD.NotArchivePathCharUp == FALSE ? 1 : 0 ) ;
#else // UNICODE
	wchar_t FilePathBuffer[ FILEPATH_MAX ] ;

	ConvString( ( const char * )FilePath, -1, _TCHARCODEFORMAT, ( char * )FilePathBuffer, sizeof( FilePathBuffer ), WCHAR_T_CHARCODEFORMAT ) ;
	DXA_DIR_ConvertFullPath( FilePathBuffer, fullpath, sizeof( fullpath ), DXARCD.NotArchivePathCharUp == FALSE ? 1 : 0 ) ;
#endif // UNICODE

	// ファイルパスからハンドルを取得する
	handle = DXA_DIR_GetArchive( fullpath ) ;
	if( handle == -1 )
	{
		return 0 ;
	}

	// 準備が完了したかどうかを得る
	return DXA_CheckIdle( &DXARCD.Archive[handle]->Archive ) ;
}

// 指定のＤＸＡファイルの事前読み込みが完了したかどうかを取得する( 戻り値  TRUE:完了した FALSE:まだ )
extern int NS_DXArchiveCheckIdleWithStrLen( const TCHAR *FilePath, size_t FilePathLength )
{
	int Result ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_ONE_BEGIN( FilePath, FilePathLength, return -1 )
	Result = NS_DXArchiveCheckIdle( UseFilePathBuffer ) ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( FilePath )
	return Result ;
}

// 指定のＤＸＡファイルをメモリから解放する
extern int NS_DXArchiveRelease( const TCHAR *FilePath )
{
	int handle;
	wchar_t fullpath[ FILEPATH_MAX ] ;

	// フルパスを得る(ついでに全ての文字を大文字にする)
#ifdef UNICODE
	DXA_DIR_ConvertFullPath( FilePath, fullpath, sizeof( fullpath ), DXARCD.NotArchivePathCharUp == FALSE ? 1 : 0 ) ;
#else // UNICODE
	wchar_t FilePathBuffer[ FILEPATH_MAX ] ;

	ConvString( ( const char * )FilePath, -1, _TCHARCODEFORMAT, ( char * )FilePathBuffer, sizeof( FilePathBuffer ), WCHAR_T_CHARCODEFORMAT ) ;
	DXA_DIR_ConvertFullPath( FilePathBuffer, fullpath, sizeof( fullpath ), DXARCD.NotArchivePathCharUp == FALSE ? 1 : 0 ) ;
#endif // UNICODE

	// ファイルパスからハンドルを取得する
	handle = DXA_DIR_GetArchive( fullpath ) ;
	if( handle == -1 )
	{
		return 0 ;
	}

	// 閉じる
	DXA_DIR_CloseArchive( handle ) ;
	DXA_DIR_CloseWaitArchive() ;

	// 終了
	return 0 ;
}

// 指定のＤＸＡファイルをメモリから解放する
extern int NS_DXArchiveReleaseWithStrLen( const TCHAR *FilePath, size_t FilePathLength )
{
	int Result ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_ONE_BEGIN( FilePath, FilePathLength, return -1 )
	Result = NS_DXArchiveRelease( UseFilePathBuffer ) ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( FilePath )
	return Result ;
}

// ＤＸＡファイルの中に指定のファイルが存在するかどうかを調べる、TargetFilePath はＤＸＡファイルをカレントフォルダとした場合のパス( 戻り値:  -1=エラー  0:無い  1:ある )
extern int NS_DXArchiveCheckFile( const TCHAR *FilePath, const TCHAR *TargetFilePath )
{
	int index, ret ;
	DXARC_DIR_ARCHIVE *Archive ;
	wchar_t fullpath[ FILEPATH_MAX ] ;

	// フルパスを得る(ついでに全ての文字を大文字にする)
#ifdef UNICODE
	DXA_DIR_ConvertFullPath( FilePath, fullpath, sizeof( fullpath ), DXARCD.NotArchivePathCharUp == FALSE ? 1 : 0 ) ;
#else // UNICODE
	wchar_t FilePathBuffer[ FILEPATH_MAX ] ;

	ConvString( ( const char * )FilePath, -1, _TCHARCODEFORMAT, ( char * )FilePathBuffer, sizeof( FilePathBuffer ), WCHAR_T_CHARCODEFORMAT ) ;
	DXA_DIR_ConvertFullPath( FilePathBuffer, fullpath, sizeof( fullpath ), DXARCD.NotArchivePathCharUp == FALSE ? 1 : 0 ) ;
#endif // UNICODE

	// アーカイブファイルがあるかどうか調べる
	index = DXA_DIR_OpenArchive( fullpath ) ;
	if( index == -1 )
	{
		return -1 ;
	}

	// アーカイブの中に指定のファイルがあるかどうかを調べる
	Archive = DXARCD.Archive[ index ] ;
	ret = DXA_GetFileInfo( &Archive->Archive, _TCHARCODEFORMAT, ( const char * )TargetFilePath, NULL, NULL ) ;

	DXA_DIR_CloseArchive( index ) ;

	// 結果を返す
	return ret == -1 ? 0 : 1 ;
}

// ＤＸＡファイルの中に指定のファイルが存在するかどうかを調べる、TargetFilePath はＤＸＡファイルをカレントフォルダとした場合のパス( 戻り値:  -1=エラー  0:無い  1:ある )
extern int NS_DXArchiveCheckFileWithStrLen( const TCHAR *FilePath, size_t FilePathLength, const TCHAR *TargetFilePath, size_t TargetFilePathLength )
{
	int Result = -1 ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_BEGIN( FilePath )
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_BEGIN( TargetFilePath )
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_SETUP( FilePath,       FilePathLength,       goto ERR )
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_SETUP( TargetFilePath, TargetFilePathLength, goto ERR )

	Result = NS_DXArchiveCheckFile( UseFilePathBuffer, UseTargetFilePathBuffer ) ;

ERR :

	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( FilePath )
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( TargetFilePath )
	return Result ;
}

// メモリ上に展開されたＤＸＡファイルを指定のファイルパスにあることにする
extern int NS_DXArchiveSetMemImage(		void *ArchiveImage, int ArchiveImageSize, const TCHAR *EmulateFilePath, int ArchiveImageCopyFlag, int ArchiveImageReadOnly )
{
	wchar_t fullpath[ FILEPATH_MAX ] ;

	// フルパスを得る(ついでに全ての文字を大文字にする)
#ifdef UNICODE
	DXA_DIR_ConvertFullPath( EmulateFilePath, fullpath, sizeof( fullpath ), DXARCD.NotArchivePathCharUp == FALSE ? 1 : 0 ) ;
#else // UNICODE
	wchar_t FilePathBuffer[ FILEPATH_MAX ] ;

	ConvString( ( const char * )EmulateFilePath, -1, _TCHARCODEFORMAT, ( char * )FilePathBuffer, sizeof( FilePathBuffer ), WCHAR_T_CHARCODEFORMAT ) ;
	DXA_DIR_ConvertFullPath( FilePathBuffer, fullpath, sizeof( fullpath ), DXARCD.NotArchivePathCharUp == FALSE ? 1 : 0 ) ;
#endif // UNICODE

	return DXA_DIR_OpenArchive( fullpath, ArchiveImage, ArchiveImageSize, ArchiveImageCopyFlag, ArchiveImageReadOnly, -1, FALSE, FALSE ) == -1 ? -1 : 0;
}

// メモリ上に展開されたＤＸＡファイルを指定のファイルパスにあることにする( EmulateFilePath は見立てる dxa ファイルのパス、例えばＤＸＡファイルイメージを Image.dxa というファイル名で c:\Temp にあることにしたい場合は EmulateFilePath に "c:\\Temp\\Image.dxa" を渡す、SetDXArchiveExtension で拡張子を変更している場合は EmulateFilePath に渡すファイルパスの拡張子もそれに合わせる必要あり )
extern int NS_DXArchiveSetMemImageWithStrLen( void *ArchiveImage, int ArchiveImageSize, const TCHAR *EmulateFilePath, size_t EmulateFilePathLength, int ArchiveImageCopyFlag, int ArchiveImageReadOnly )
{
	int Result ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_ONE_BEGIN( EmulateFilePath, EmulateFilePathLength, return -1 )
	Result = NS_DXArchiveSetMemImage( ArchiveImage, ArchiveImageSize, UseEmulateFilePathBuffer, ArchiveImageCopyFlag, ArchiveImageReadOnly ) ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( EmulateFilePath )
	return Result ;
}

// DXArchiveSetMemImage の設定を解除する
extern int NS_DXArchiveReleaseMemImage(	void *ArchiveImage )
{
	int handle;

	// ファイルパスからハンドルを取得する
	handle = DXA_DIR_GetArchive( NULL, ArchiveImage ) ;
	if( handle == -1 )
	{
		return 0 ;
	}

	// 閉じる
	DXA_DIR_CloseArchive( handle ) ;
	DXA_DIR_CloseWaitArchive() ;

	// 終了
	return 0 ;
}

#endif


// データを解凍する( 戻り値:解凍後のデータサイズ )
#define MIN_COMPRESS		(4)						// 最低圧縮バイト数
#define MAX_SEARCHLISTNUM	(64)					// 最大一致長を探す為のリストを辿る最大数
#define MAX_SUBLISTNUM		(65536)					// 圧縮時間短縮のためのサブリストの最大数
#define MAX_COPYSIZE 		(0x1fff + MIN_COMPRESS)	// 参照アドレスからコピー出切る最大サイズ( 圧縮コードが表現できるコピーサイズの最大値 + 最低圧縮バイト数 )
#define MAX_ADDRESSLISTNUM	(1024 * 1024 * 1)		// スライド辞書の最大サイズ
#define MAX_POSITION		(1 << 24)				// 参照可能な最大相対アドレス( 16MB )

// 圧縮時間短縮用リスト
typedef struct LZ_LIST
{
	LZ_LIST *next, *prev ;
	DWORD address ;
} LZ_LIST ;

// データを圧縮する( 戻り値:圧縮後のデータサイズ )
extern	int	DXA_Encode( void *Src, DWORD SrcSize, void *Dest )
{
	int dstsize ;
	int    bonus,    conbo,    conbosize,        address,    addresssize ;
	int maxbonus, maxconbo, maxconbosize = 0, maxaddress, maxaddresssize = 0 ;
	BYTE keycode, *srcp, *destp, *dp, *sp, *sp2, *sp1 ;
	DWORD srcaddress, code ;
	int j ;
	DWORD i, m ;
	DWORD maxlistnum, maxlistnummask, listaddp ;
	DWORD sublistnum, sublistmaxnum ;
	LZ_LIST *listbuf, *listtemp, *list, *newlist ;
	BYTE *listfirsttable, *usesublistflagtable, *sublistbuf ;
	
	// サブリストのサイズを決める
	{
			 if( SrcSize < 100 * 1024 )			sublistmaxnum = 1 ;
		else if( SrcSize < 3 * 1024 * 1024 )	sublistmaxnum = MAX_SUBLISTNUM / 3 ;
		else									sublistmaxnum = MAX_SUBLISTNUM ;
	}

	// リストのサイズを決める
	{
		maxlistnum = MAX_ADDRESSLISTNUM ;
		if( maxlistnum > SrcSize )
		{
			while( ( maxlistnum >> 1 ) > 0x100 && ( maxlistnum >> 1 ) > SrcSize )
				maxlistnum >>= 1 ;
		}
		maxlistnummask = maxlistnum - 1 ;
	}

	// メモリの確保
	usesublistflagtable   = (BYTE *)DXALLOC(
		sizeof( DWORD_PTR )	* 65536 +					// メインリストの先頭オブジェクト用領域
		sizeof( LZ_LIST   )	* maxlistnum +				// メインリスト用領域
		sizeof( BYTE      )	* 65536 +					// サブリストを使用しているかフラグ用領域
		sizeof( DWORD_PTR )	* 256 * sublistmaxnum ) ;	// サブリスト用領域
		
	// アドレスのセット
	listfirsttable =     usesublistflagtable + sizeof( BYTE      ) * 65536 ;
	sublistbuf     =          listfirsttable + sizeof( DWORD_PTR ) * 65536 ;
	listbuf        = (LZ_LIST *)( sublistbuf + sizeof( DWORD_PTR ) * 256 * sublistmaxnum ) ;
	
	// 初期化
	_MEMSET( usesublistflagtable, 0, sizeof( BYTE      ) * 65536               ) ;
	_MEMSET(          sublistbuf, 0, sizeof( DWORD_PTR ) * 256 * sublistmaxnum ) ;
	_MEMSET(      listfirsttable, 0, sizeof( DWORD_PTR ) * 65536               ) ;
	list = listbuf ;
	for( i = maxlistnum / 8 ; i ; i --, list += 8 )
	{
		list[0].address =
		list[1].address =
		list[2].address =
		list[3].address =
		list[4].address =
		list[5].address =
		list[6].address =
		list[7].address = 0xffffffff ;
	}

	srcp  = (BYTE *)Src ;
	destp = (BYTE *)Dest ;

	// 圧縮元データの中で一番出現頻度が低いバイトコードを検索する
	{
		DWORD qnum, table[256], mincode ;

		for( i = 0 ; i < 256 ; i ++ )
			table[i] = 0 ;
		
		sp   = srcp ;
		qnum = SrcSize / 8 ;
		i    = qnum * 8 ;
		for( ; qnum ; qnum --, sp += 8 )
		{
			table[sp[0]] ++ ;
			table[sp[1]] ++ ;
			table[sp[2]] ++ ;
			table[sp[3]] ++ ;
			table[sp[4]] ++ ;
			table[sp[5]] ++ ;
			table[sp[6]] ++ ;
			table[sp[7]] ++ ;
		}
		for( ; i < SrcSize ; i ++, sp ++ )
			table[*sp] ++ ;
			
		keycode = 0 ;
		mincode = table[0] ;
		for( i = 1 ; i < 256 ; i ++ )
		{
			if( mincode < table[i] ) continue ;
			mincode = table[i] ;
			keycode = (BYTE)i ;
		}
	}

	// 圧縮元のサイズをセット
	((DWORD *)destp)[0] = SrcSize ;

	// キーコードをセット
	destp[8] = keycode ;

	// 圧縮処理
	dp               = destp + 9 ;
	sp               = srcp ;
	srcaddress       = 0 ;
	dstsize          = 0 ;
	listaddp         = 0 ;
	sublistnum       = 0 ;
	while( srcaddress < SrcSize )
	{
		// 残りサイズが最低圧縮サイズ以下の場合は圧縮処理をしない
		if( srcaddress + MIN_COMPRESS >= SrcSize ) goto NOENCODE ;

		// リストを取得
		code = *((WORD *)sp) ;
		list = (LZ_LIST *)( listfirsttable + code * sizeof( DWORD_PTR ) ) ;
		if( usesublistflagtable[code] == 1 )
		{
			list = (LZ_LIST *)( (DWORD_PTR *)list->next + sp[2] ) ;
		}
		else
		{
			if( sublistnum < sublistmaxnum )
			{
				list->next = (LZ_LIST *)( sublistbuf + sizeof( DWORD_PTR ) * 256 * sublistnum ) ;
				list       = (LZ_LIST *)( (DWORD_PTR *)list->next + sp[2] ) ;
			
				usesublistflagtable[code] = 1 ;
				sublistnum ++ ;
			}
		}

		// 一番一致長の長いコードを探す
		maxconbo   = -1 ;
		maxaddress = -1 ;
		maxbonus   = -1 ;
		for( m = 0, listtemp = list->next ; /*m < MAX_SEARCHLISTNUM &&*/ listtemp != NULL ; listtemp = listtemp->next, m ++ )
		{
			address = ( int )( srcaddress - listtemp->address ) ;
			if( address >= MAX_POSITION )
			{
				if( listtemp->prev ) listtemp->prev->next = listtemp->next ;
				if( listtemp->next ) listtemp->next->prev = listtemp->prev ;
				listtemp->address = 0xffffffff ;
				continue ;
			}
			
			sp2 = &sp[-address] ;
			sp1 = sp ;
			if( srcaddress + MAX_COPYSIZE < SrcSize )
			{
				conbo = MAX_COPYSIZE / 4 ;
				while( conbo && *((DWORD *)sp2) == *((DWORD *)sp1) )
				{
					sp2 += 4 ;
					sp1 += 4 ;
					conbo -- ;
				}
				conbo = MAX_COPYSIZE - ( MAX_COPYSIZE / 4 - conbo ) * 4 ;

				while( conbo && *sp2 == *sp1 )
				{
					sp2 ++ ;
					sp1 ++ ;
					conbo -- ;
				}
				conbo = MAX_COPYSIZE - conbo ;
			}
			else
			{
				for( conbo = 0 ;
						conbo < MAX_COPYSIZE &&
						conbo + srcaddress < SrcSize &&
						sp[conbo - address] == sp[conbo] ;
							conbo ++ ){}
			}

			if( conbo >= 4 )
			{
				conbosize   = ( conbo - MIN_COMPRESS ) < 0x20 ? 0 : 1 ;
				addresssize = address < 0x100 ? 0 : ( address < 0x10000 ? 1 : 2 ) ;
				bonus       = conbo - ( 3 + conbosize + addresssize ) ;

				if( bonus > maxbonus )
				{
					maxconbo       = conbo ;
					maxaddress     = address ;
					maxaddresssize = addresssize ;
					maxconbosize   = conbosize ;
					maxbonus       = bonus ;
				}
			}
		}

		// リストに登録
		newlist = &listbuf[listaddp] ;
		if( newlist->address != 0xffffffff )
		{
			if( newlist->prev ) newlist->prev->next = newlist->next ;
			if( newlist->next ) newlist->next->prev = newlist->prev ;
			newlist->address = 0xffffffff ;
		}
		newlist->address = srcaddress ;
		newlist->prev    = list ;
		newlist->next    = list->next ;
		if( list->next != NULL ) list->next->prev = newlist ;
		list->next       = newlist ;
		listaddp         = ( listaddp + 1 ) & maxlistnummask ;

		// 一致コードが見つからなかったら非圧縮コードとして出力
		if( maxconbo == -1 )
		{
NOENCODE:
			// キーコードだった場合は２回連続で出力する
			if( *sp == keycode )
			{
				if( destp != NULL )
				{
					dp[0]  =
					dp[1]  = keycode ;
					dp += 2 ;
				}
				dstsize += 2 ;
			}
			else
			{
				if( destp != NULL )
				{
					*dp = *sp ;
					dp ++ ;
				}
				dstsize ++ ;
			}
			sp ++ ;
			srcaddress ++ ;
		}
		else
		{
			// 見つかった場合は見つけた位置と長さを出力する
			
			// キーコードと見つけた位置と長さを出力
			if( destp != NULL )
			{
				// キーコードの出力
				*dp++ = keycode ;

				// 出力する連続長は最低 MIN_COMPRESS あることが前提なので - MIN_COMPRESS したものを出力する
				maxconbo -= MIN_COMPRESS ;

				// 連続長０〜４ビットと連続長、相対アドレスのビット長を出力
				*dp = (BYTE)( ( ( maxconbo & 0x1f ) << 3 ) | ( maxconbosize << 2 ) | maxaddresssize ) ;

				// キーコードの連続はキーコードと値の等しい非圧縮コードと
				// 判断するため、キーコードの値以上の場合は値を＋１する
				if( *dp >= keycode ) dp[0] += 1 ;
				dp ++ ;

				// 連続長５〜１２ビットを出力
				if( maxconbosize == 1 )
					*dp++ = (BYTE)( ( maxconbo >> 5 ) & 0xff ) ;

				// maxconbo はまだ使うため - MIN_COMPRESS した分を戻す
				maxconbo += MIN_COMPRESS ;

				// 出力する相対アドレスは０が( 現在のアドレス−１ )を挿すので、−１したものを出力する
				maxaddress -- ;

				// 相対アドレスを出力
				*dp++ = (BYTE)( maxaddress ) ;
				if( maxaddresssize > 0 )
				{
					*dp++ = (BYTE)( maxaddress >> 8 ) ;
					if( maxaddresssize == 2 )
						*dp++ = (BYTE)( maxaddress >> 16 ) ;
				}
			}
			
			// 出力サイズを加算
			dstsize += 3 + maxaddresssize + maxconbosize ;
			
			// リストに情報を追加
			if( srcaddress + maxconbo < SrcSize )
			{
				sp2 = &sp[1] ;
				for( j = 1 ; j < maxconbo && (DWORD_PTR)&sp2[2] - (DWORD_PTR)srcp < SrcSize ; j ++, sp2 ++ )
				{
					code = *((WORD *)sp2) ;
					list = (LZ_LIST *)( listfirsttable + code * sizeof( DWORD_PTR ) ) ;
					if( usesublistflagtable[code] == 1 )
					{
						list = (LZ_LIST *)( (DWORD_PTR *)list->next + sp2[2] ) ;
					}
					else
					{
						if( sublistnum < sublistmaxnum )
						{
							list->next = (LZ_LIST *)( sublistbuf + sizeof( DWORD_PTR ) * 256 * sublistnum ) ;
							list       = (LZ_LIST *)( (DWORD_PTR *)list->next + sp2[2] ) ;
						
							usesublistflagtable[code] = 1 ;
							sublistnum ++ ;
						}
					}

					newlist = &listbuf[listaddp] ;
					if( newlist->address != 0xffffffff )
					{
						if( newlist->prev ) newlist->prev->next = newlist->next ;
						if( newlist->next ) newlist->next->prev = newlist->prev ;
						newlist->address = 0xffffffff ;
					}
					newlist->address = srcaddress + j ;
					newlist->prev = list ;
					newlist->next = list->next ;
					if( list->next != NULL ) list->next->prev = newlist ;
					list->next = newlist ;
					listaddp = ( listaddp + 1 ) & maxlistnummask ;
				}
			}
			
			sp         += maxconbo ;
			srcaddress += maxconbo ;
		}
	}

	// 圧縮後のデータサイズを保存する
	*((DWORD *)&destp[4]) = ( DWORD )( dstsize + 9 ) ;

	// 確保したメモリの解放
	DXFREE( usesublistflagtable ) ;

	// データのサイズを返す
	return dstsize + 9 ;
}

extern int DXA_Decode( void *Src, void *Dest )
{
	DWORD srcsize, destsize, code, indexsize, keycode, conbo, index = 0 ;
	BYTE *srcp, *destp, *dp, *sp ;

	destp = (BYTE *)Dest ;
	srcp  = (BYTE *)Src ;
	
	// 解凍後のデータサイズを得る
	destsize = GET_MEM_DWORD( &srcp[0] ) ;

	// 圧縮データのサイズを得る
	srcsize = GET_MEM_DWORD( &srcp[4] ) - 9 ;

	// キーコード
	keycode = srcp[8] ;
	
	// 出力先がない場合はサイズだけ返す
	if( Dest == NULL )
	{
		return ( int )destsize ;
	}
	
	// 展開開始
	sp  = srcp + 9 ;
	dp  = destp ;
	while( srcsize )
	{
		// キーコードか同かで処理を分岐
		if( sp[0] != keycode )
		{
			// 非圧縮コードの場合はそのまま出力
			*dp = *sp ;
			dp      ++ ;
			sp      ++ ;
			srcsize -- ;
			continue ;
		}
	
		// キーコードが連続していた場合はキーコード自体を出力
		if( sp[1] == keycode )
		{
			*dp = (BYTE)keycode ;
			dp      ++ ;
			sp      += 2 ;
			srcsize -= 2 ;
			
			continue ;
		}

		// 第一バイトを得る
		code = sp[1] ;

		// もしキーコードよりも大きな値だった場合はキーコード
		// とのバッティング防止の為に＋１しているので−１する
		if( code > keycode ) code -- ;

		sp      += 2 ;
		srcsize -= 2 ;

		// 連続長を取得する
		conbo = code >> 3 ;
		if( code & ( 0x1 << 2 ) )
		{
			conbo |= *sp << 5 ;
			sp      ++ ;
			srcsize -- ;
		}
		conbo += MIN_COMPRESS ;	// 保存時に減算した最小圧縮バイト数を足す

		// 参照相対アドレスを取得する
		indexsize = code & 0x3 ;
		switch( indexsize )
		{
		case 0 :
			index = *sp ;
			sp      ++ ;
			srcsize -- ;
			break ;
			
		case 1 :
			index = GET_MEM_WORD( sp ) ;
			sp      += 2 ;
			srcsize -= 2 ;
			break ;
			
		case 2 :
			index = ( DWORD )( GET_MEM_WORD( sp ) | ( sp[2] << 16 ) ) ;
			sp      += 3 ;
			srcsize -= 3 ;
			break ;
		}
		index ++ ;		// 保存時に−１しているので＋１する

		// 展開
		if( index < conbo )
		{
			DWORD num ;

			num  = index ;
			while( conbo > num )
			{
				_MEMCPY( dp, dp - num, num ) ;
				dp    += num ;
				conbo -= num ;
				num   += num ;
			}
			if( conbo != 0 )
			{
				_MEMCPY( dp, dp - num, conbo ) ;
				dp += conbo ;
			}
		}
		else
		{
			_MEMCPY( dp, dp - index, conbo ) ;
			dp += conbo ;
		}
	}

	// 解凍後のサイズを返す
	return (int)destsize ;
}

// データをハフマン圧縮する( 戻り値:圧縮後のサイズ  0 はエラー  Dest に NULL を入れると圧縮データ格納に必要なサイズが返る )
extern ULONGLONG Huffman_Encode( void *Src, ULONGLONG SrcSize, void *Dest )
{
    // 結合データと数値データ、０〜２５５までが数値データ
    // (結合データの数と圧縮するデータの種類の数を足すと必ず『種類の数＋(種類の数−１)』になる。
    // 『ホントか？』と思われる方はハフマン圧縮の説明で出てきたＡ,Ｂ,Ｃ,Ｄ,Ｅの結合部分の数を
    // 数えてみて下さい、種類が５つに対して結合部分は一つ少ない４つになっているはずです。
    // 種類が６つの時は結合部分は５つに、そして種類が２５６この時は結合部分は２５５個になります)
    HUFFMAN_NODE Node[256 + 255] ;

    unsigned char *SrcPoint ;
    ULONGLONG PressBitCounter, PressSizeCounter, SrcSizeCounter ;
    ULONGLONG i ;

    // void 型のポインタではアドレスの操作が出来ないので unsigned char 型のポインタにする
    SrcPoint = ( unsigned char * )Src ;

    // 各数値の圧縮後のビット列を算出する
    {
        int NodeIndex, MinNode1, MinNode2 ;
        int NodeNum, DataNum ;

        // 数値データを初期化する
        for( i = 0 ; i < 256 ; i ++ )
        {
            Node[i].Weight = 0 ;           // 出現数はこれから算出するので０に初期化
            Node[i].ChildNode[0] = -1 ;    // 数値データが終点なので -1 をセットする
            Node[i].ChildNode[1] = -1 ;    // 数値データが終点なので -1 をセットする
            Node[i].ParentNode = -1 ;      // まだどの要素とも結合されていないので -1 をセットする
        }

        // 各数値の出現数をカウント
        for( i = 0 ; i < SrcSize ; i ++ )
        {
            Node[ SrcPoint[i] ].Weight ++ ;
        }

		// 出現数を 0〜65535 の比率に変換する
		for( i = 0 ; i < 256 ; i ++ )
		{
			Node[ i ].Weight = Node[ i ].Weight * 0xffff / SrcSize ;
		}

        // 出現数の少ない数値データ or 結合データを繋いで
        // 新しい結合データを作成、全ての要素を繋いで残り１個になるまで繰り返す
        DataNum = 256 ; // 残り要素数
        NodeNum = 256 ; // 次に新しく作る結合データの要素配列のインデックス
        while( DataNum > 1 )
        {
            // 出現数値の低い要素二つを探す
            {
                MinNode1 = -1 ;
                MinNode2 = -1 ;
                
                // 残っている要素全てを調べるまでループ
                NodeIndex = 0 ;
                for( i = 0 ; i < ( ULONGLONG )DataNum ; NodeIndex ++ )
                {
                    // もう既に何処かの要素と結合されている場合は対象外
                    if( Node[NodeIndex].ParentNode != -1 ) continue ;
                    
                    i ++ ;
                    
                    // まだ有効な要素をセットしていないか、より出現数値の
                    // 少ない要素が見つかったら更新
                    if( MinNode1 == -1 || Node[MinNode1].Weight > Node[NodeIndex].Weight )
                    {
                        // 今まで一番出現数値が少なかったと思われた
                        // 要素は二番目に降格
                        MinNode2 = MinNode1 ;

                        // 新しい一番の要素の要素配列のインデックスを保存
                        MinNode1 = NodeIndex ;
                    }
                    else
                    {
                        // 一番よりは出現数値が多くても、二番目よりは出現数値が
                        // 少ないかもしれないので一応チェック(又は二番目に出現数値の
                        // 少ない要素がセットされていなかった場合もセット)
                        if( MinNode2 == -1 || Node[MinNode2].Weight > Node[NodeIndex].Weight )
                        {
                            MinNode2 = NodeIndex ;
                        }
                    }
                }
            }
            
            // 二つの要素を繋いで新しい要素(結合データ)を作る
            Node[NodeNum].ParentNode = -1 ;  // 新しいデータは当然まだ何処とも繋がっていないので -1 
            Node[NodeNum].Weight = Node[MinNode1].Weight + Node[MinNode2].Weight ;    // 出現数値は二つの数値を足したものをセットする
            Node[NodeNum].ChildNode[0] = MinNode1 ;    // この結合部で 0 を選んだら出現数値が一番少ない要素に繋がる
            Node[NodeNum].ChildNode[1] = MinNode2 ;    // この結合部で 1 を選んだら出現数値が二番目に少ない要素に繋がる

            // 結合された要素二つに、自分達に何の値が割り当てられたかをセットする
            Node[MinNode1].Index = 0 ;    // 一番出現数値が少ない要素は 0 番
            Node[MinNode2].Index = 1 ;    // 二番目に出現数値が少ない要素は 1 番

            // 結合された要素二つに、自分達を結合した結合データの要素配列インデックスをセットする
            Node[MinNode1].ParentNode = NodeNum ;
            Node[MinNode2].ParentNode = NodeNum ;

            // 要素の数を一個増やす
            NodeNum ++ ;

            // 残り要素の数は、一つ要素が新しく追加された代わりに
            // 二つの要素が結合されて検索の対象から外れたので
            // 結果 1 - 2 で -1 
            DataNum -- ;
        }
        
        // 各数値の圧縮後のビット列を割り出す
        {
            unsigned char TempBitArray[32] ;
            int TempBitIndex, TempBitCount, BitIndex, BitCount ;
        
            // 数値データの種類の数だけ繰り返す
            for( i = 0 ; i < 256 ; i ++ )
            {
                // 数値データから結合データを上へ上へと辿ってビット数を数える
                {
                    // ビット数を初期化しておく
                    Node[i].BitNum = 0 ;
                    
                    // 一時的に数値データから遡っていったときのビット列を保存する処理の準備
                    TempBitIndex = 0 ;
                    TempBitCount = 0 ;
                    TempBitArray[TempBitIndex] = 0 ;
                    
                    // 何処かと結合されている限りカウントし続ける(天辺は何処とも結合されていないので終点だと分かる)
                    for( NodeIndex = ( int )i ; Node[NodeIndex].ParentNode != -1 ; NodeIndex = Node[NodeIndex].ParentNode )
                    {
                        // 配列要素一つに入るビットデータは８個なので、同じ配列要素に
                        // 既に８個保存していたら次の配列要素に保存先を変更する
                        if( TempBitCount == 8 )
                        {
                            TempBitCount = 0 ;
                            TempBitIndex ++ ;
                            TempBitArray[TempBitIndex] = 0 ;
                        }
                        
                        // 新しく書き込む情報で今までのデータを上書きしてしまわないように１ビット左にシフトする
                        TempBitArray[TempBitIndex] <<= 1 ;

                        // 結合データに割り振られたインデックスを最下位ビット(一番右側のビット)に書き込む
                        TempBitArray[TempBitIndex] |= (unsigned char)Node[NodeIndex].Index ;

                        // 保存したビット数を増やす
                        TempBitCount ++ ;

                        // ビット数を増やす
                        Node[i].BitNum ++ ;
                    }
                }
				
                // TempBitArray に溜まったデータは数値データから結合データを天辺に向かって
                // 上へ上へと遡っていった時のビット列なので、逆さまにしないと圧縮後のビット
                // 配列として使えない(展開時に天辺の結合データから数値データまで辿ることが
                // 出来ない)ので、順序を逆さまにしたものを数値データ内のビット列バッファに保存する
                {
                    BitCount = 0 ;
                    BitIndex = 0 ;
                    
                    // 最初のバッファを初期化しておく
                    // (全部 論理和(or)演算 で書き込むので、最初から１になっている
                    // ビットに０を書き込んでも１のままになってしまうため)
                    Node[i].BitArray[BitIndex] = 0 ;
                    
                    // 一時的に保存しておいたビット列の最初まで遡る
                    while( TempBitIndex >= 0 )
                    {
                        // 書き込んだビット数が一つの配列要素に入る８ビットに
                        // 達してしまったら次の配列要素に移る
                        if( BitCount == 8 )
                        {
                            BitCount = 0 ;
                            BitIndex ++ ;
                            Node[i].BitArray[BitIndex] = 0 ;
                        }

                        // まだ何も書き込まれていないビットアドレスに１ビット書き込む
                        Node[i].BitArray[BitIndex] |= (unsigned char)( ( TempBitArray[TempBitIndex] & 1 ) << BitCount ) ;
						
                        // 書き込み終わったビットはもういらないので次のビットを
                        // 書き込めるように１ビット右にシフトする
                        TempBitArray[TempBitIndex] >>= 1 ;
                        
                        // １ビット書き込んだので残りビット数を１個減らす
                        TempBitCount -- ;
                        
                        // もし現在書き込み元となっている配列要素に書き込んでいない
                        // ビット情報が無くなったら次の配列要素に移る
                        if( TempBitCount == 0 )
                        {
                            TempBitIndex -- ;
                            TempBitCount = 8 ;
                        }
                        
                        // 書き込んだビット数を増やす
                        BitCount ++ ;
                    }
                }
            }
        }
    }

    // 変換処理
    {
        unsigned char *PressData ;
        int BitData, BitCounter, BitIndex, BitNum, NodeIndex ;
        
        // 圧縮データを格納するアドレスをセット
        // (圧縮データ本体は元のサイズ、圧縮後のサイズ、各数値の出現数等を
        // 格納するデータ領域の後に格納する)
        PressData = ( unsigned char * )Dest ;
        
        // 圧縮するデータの参照アドレスを初期化
        SrcSizeCounter = 0 ;
        
        // 圧縮したデータの参照アドレスを初期化
        PressSizeCounter = 0 ;
        
        // 圧縮したビットデータのカウンタを初期化
        PressBitCounter = 0 ;
        
        // 圧縮データの最初のバイトを初期化しておく
        if( Dest != NULL ) PressData[PressSizeCounter] = 0 ;

        // 圧縮対照のデータを全て圧縮後のビット列に変換するまでループ
        for( SrcSizeCounter = 0 ; SrcSizeCounter < SrcSize ; SrcSizeCounter ++ )
        {
            // 保存する数値データのインデックスを取得
            NodeIndex = SrcPoint[SrcSizeCounter] ;
            
            // 指定の数値データの圧縮後のビット列を出力
            {
                // 参照する配列のインデックスを初期化
                BitIndex = 0 ;
                
                // 配列要素中の出力したビット数の初期化
                BitNum = 0 ;
                
                // 最初に書き込むビット列の配列要素をセット
                BitData = Node[NodeIndex].BitArray[0] ;

                // 全てのビットを出力するまでループ
                for( BitCounter = 0 ; BitCounter < Node[NodeIndex].BitNum ; BitCounter ++ )
                {
                    // もし書き込んだビット数が８個になっていたら次の配列要素に移る
                    if( PressBitCounter == 8 )
                    {
                        PressSizeCounter ++ ;
                        if( Dest != NULL ) PressData[PressSizeCounter] = 0 ;
                        PressBitCounter = 0 ;
                    }
                    
                    // もし書き出したビット数が８個になっていたら次の配列要素に移る
                    if( BitNum == 8 )
                    {
                        BitIndex ++ ;
                        BitData = Node[NodeIndex].BitArray[BitIndex] ;
                        BitNum = 0 ;
                    }
                    
                    // まだ何も書き込まれていないビットアドレスに１ビット書き込む
                    if( Dest != NULL ) PressData[PressSizeCounter] |= (unsigned char)( ( BitData & 1 ) << PressBitCounter ) ;

                    // 書き込んだビット数を増やす
                    PressBitCounter ++ ;

                    // 次に書き出すビットを最下位ビット(一番右のビット)にする為に
                    // １ビット右シフトする
                    BitData >>= 1 ;
                    
                    // 書き出したビット数を増やす
                    BitNum ++ ;
                }
            }
        }
        
        // 最後の１バイト分のサイズを足す
        PressSizeCounter ++ ;
    }
    
    // 圧縮データの情報を保存する
    {
		BIT_STREAM BitStream ;
		BYTE HeadBuffer[ 256 * 2 + 32 ] ;
		BYTE BitNum ;
		ULONGLONG HeadSize ;
		int WeightSaveData[ 256 ] ;

		BitStream_Init( &BitStream, HeadBuffer, false ) ;

        // 元のデータのサイズをセット
		BitNum = BitStream_GetBitNum( SrcSize ) ;
		if( BitNum > 0 )
		{
			BitNum -- ;
		}
        BitStream_Write( &BitStream, 6, BitNum ) ;
		BitStream_Write( &BitStream, BitNum + 1, SrcSize ) ;
        
        // 圧縮後のデータのサイズをセット
		BitNum = BitStream_GetBitNum( PressSizeCounter ) ;
        BitStream_Write( &BitStream, 6, BitNum ) ;
		BitStream_Write( &BitStream, BitNum + 1, PressSizeCounter ) ;
        
        // 各数値の出現率の差分値を保存する
		WeightSaveData[ 0 ] = ( int )Node[ 0 ].Weight ;
        for( i = 1 ; i < 256 ; i ++ )
        {
			WeightSaveData[ i ] = ( int )Node[ i ].Weight - ( int )Node[ i - 1 ].Weight ;
        }
        for( i = 0 ; i < 256 ; i ++ )
        {
			ULONGLONG OutputNum ;
			bool Minus ;

			if( WeightSaveData[ i ] < 0 )
			{
				OutputNum = ( ULONGLONG )( -WeightSaveData[ i ] ) ;
				Minus = true ;
			}
			else
			{
				OutputNum = ( ULONGLONG )WeightSaveData[ i ] ;
				Minus = false ;
			}

			BitNum = ( BitStream_GetBitNum( OutputNum ) + 1 ) / 2 ;
			if( BitNum > 0 )
			{
				BitNum -- ;
			}
	        BitStream_Write( &BitStream, 3, BitNum ) ;
			BitStream_Write( &BitStream, 1, Minus ? 1 : 0 ) ;
			BitStream_Write( &BitStream, ( BitNum + 1 ) * 2, OutputNum ) ;
        }
		
		// ヘッダサイズを取得
		HeadSize = BitStream_GetBytes( &BitStream ) ;

		// 圧縮データの情報を圧縮データにコピーする
		if( Dest != NULL )
		{
			ULONGLONG j ;

			// ヘッダの分だけ移動
			for( j = PressSizeCounter - 1 ; j >= 0 ; j -- )
			{
				( ( BYTE * )Dest )[ HeadSize + j ] = ( ( BYTE * )Dest )[ j ] ;
				if( j == 0 )
				{
					break ;
				}
			}

			// ヘッダを書き込み
			_MEMCPY( Dest, HeadBuffer, ( size_t )HeadSize ) ;
		}

		// 圧縮後のサイズを返す
		return PressSizeCounter + HeadSize ;
    }
}

// ハフマン圧縮されたデータを解凍する( 戻り値:解凍後のサイズ  0 はエラー  Dest に NULL を入れると解凍データ格納に必要なサイズが返る )
extern ULONGLONG Huffman_Decode( void *Press, void *Dest )
{
    // 結合データと数値データ、０〜２５５までが数値データ
    HUFFMAN_NODE Node[256 + 255] ;

    ULONGLONG PressSizeCounter, DestSizeCounter, DestSize ;
    unsigned char *PressPoint, *DestPoint ;
	ULONGLONG OriginalSize ;
//	ULONGLONG PressSize ;
	ULONGLONG HeadSize ;
	WORD Weight[ 256 ] ;
    int i ;

    // void 型のポインタではアドレスの操作が出来ないので unsigned char 型のポインタにする
    PressPoint = ( unsigned char * )Press ;
    DestPoint = ( unsigned char * )Dest ;

    // 圧縮データの情報を取得する
	{
		BIT_STREAM BitStream ;
		BYTE BitNum ;
		BYTE Minus ;
		WORD SaveData ;

		BitStream_Init( &BitStream, PressPoint, true ) ;

		OriginalSize = BitStream_Read( &BitStream, ( BYTE )( BitStream_Read( &BitStream, 6 ) + 1 ) ) ;
//		PressSize    = BitStream_Read( &BitStream, ( BYTE )( BitStream_Read( &BitStream, 6 ) + 1 ) ) ;
		BitStream_Read( &BitStream, ( BYTE )( BitStream_Read( &BitStream, 6 ) + 1 ) ) ;

		// 出現頻度のテーブルを復元する
		BitNum      = ( BYTE )( BitStream_Read( &BitStream, 3 ) + 1 ) * 2 ;
		Minus       = ( BYTE )BitStream_Read( &BitStream, 1 ) ;
		SaveData    = ( WORD )BitStream_Read( &BitStream, BitNum ) ;
		Weight[ 0 ] = SaveData ;
        for( i = 1 ; i < 256 ; i ++ )
        {
			BitNum      = ( BYTE )( BitStream_Read( &BitStream, 3 ) + 1 ) * 2 ;
			Minus       = ( BYTE )BitStream_Read( &BitStream, 1 ) ;
			SaveData    = ( WORD )BitStream_Read( &BitStream, BitNum ) ;
			Weight[ i ] = Minus == 1 ? Weight[ i - 1 ] - SaveData : Weight[ i - 1 ] + SaveData ;
        }

		HeadSize = BitStream_GetBytes( &BitStream ) ;
	}
    
    // Dest が NULL の場合は 解凍後のデータのサイズを返す
    if( Dest == NULL )
        return OriginalSize ;

    // 解凍後のデータのサイズを取得する
    DestSize = OriginalSize ;

    // 各数値の結合データを構築する
    {
        int NodeIndex, MinNode1, MinNode2 ;
        int NodeNum, DataNum ;

        // 数値データを初期化する
        for( i = 0 ; i < 256 + 255 ; i ++ )
        {
            Node[i].Weight = i < 256 ? Weight[i] : 0 ;	// 出現数は保存しておいたデータからコピー
            Node[i].ChildNode[0] = -1 ;					// 数値データが終点なので -1 をセットする
            Node[i].ChildNode[1] = -1 ;					// 数値データが終点なので -1 をセットする
            Node[i].ParentNode = -1 ;					// まだどの要素とも結合されていないので -1 をセットする
        }

        // 出現数の少ない数値データ or 結合データを繋いで
        // 新しい結合データを作成、全ての要素を繋いで残り１個になるまで繰り返す
        // (圧縮時と同じコードです)
        DataNum = 256 ; // 残り要素数
        NodeNum = 256 ; // 次に新しく作る結合データの要素配列のインデックス
        while( DataNum > 1 )
        {
            // 出現数値の低い要素二つを探す
            {
                MinNode1 = -1 ;
                MinNode2 = -1 ;
                
                // 残っている要素全てを調べるまでループ
                NodeIndex = 0 ;
                for( i = 0 ; i < DataNum ; NodeIndex ++ )
                {
                    // もう既に何処かの要素と結合されている場合は対象外
                    if( Node[NodeIndex].ParentNode != -1 ) continue ;
                    
                    i ++ ;
                    
                    // まだ有効な要素をセットしていないか、より出現数値の
                    // 少ない要素が見つかったら更新
                    if( MinNode1 == -1 || Node[MinNode1].Weight > Node[NodeIndex].Weight )
                    {
                        // 今まで一番出現数値が少なかったと思われた
                        // 要素は二番目に降格
                        MinNode2 = MinNode1 ;

                        // 新しい一番の要素の要素配列のインデックスを保存
                        MinNode1 = NodeIndex ;
                    }
                    else
                    {
                        // 一番よりは出現数値が多くても、二番目よりは出現数値が
                        // 少ないかもしれないので一応チェック(又は二番目に出現数値の
                        // 少ない要素がセットされていなかった場合もセット)
                        if( MinNode2 == -1 || Node[MinNode2].Weight > Node[NodeIndex].Weight )
                        {
                            MinNode2 = NodeIndex ;
                        }
                    }
                }
            }
            
            // 二つの要素を繋いで新しい要素(結合データ)を作る
            Node[NodeNum].ParentNode = -1 ;  // 新しいデータは当然まだ何処とも繋がっていないので -1 
            Node[NodeNum].Weight = Node[MinNode1].Weight + Node[MinNode2].Weight ;    // 出現数値は二つの数値を足したものをセットする
            Node[NodeNum].ChildNode[0] = MinNode1 ;    // この結合部で 0 を選んだら出現数値が一番少ない要素に繋がる
            Node[NodeNum].ChildNode[1] = MinNode2 ;    // この結合部で 1 を選んだら出現数値が二番目に少ない要素に繋がる

            // 結合された要素二つに、自分達に何の値が割り当てられたかをセットする
            Node[MinNode1].Index = 0 ;    // 一番出現数値が少ない要素は 0 番
            Node[MinNode2].Index = 1 ;    // 二番目に出現数値が少ない要素は 1 番

            // 結合された要素二つに、自分達を結合した結合データの要素配列インデックスをセットする
            Node[MinNode1].ParentNode = NodeNum ;
            Node[MinNode2].ParentNode = NodeNum ;

            // 要素の数を一個増やす
            NodeNum ++ ;

            // 残り要素の数は、一つ要素が新しく追加された代わりに
            // 二つの要素が結合されて検索の対象から外れたので
            // 結果 1 - 2 で -1 
            DataNum -- ;
        }

        // 各数値の圧縮時のビット列を割り出す
        {
            unsigned char TempBitArray[32] ;
            int TempBitIndex, TempBitCount, BitIndex, BitCount ;
        
            // 数値データと結合データの数だけ繰り返す
            for( i = 0 ; i < 256 + 254 ; i ++ )
            {
                // 数値データから結合データを上へ上へと辿ってビット数を数える
                {
                    // ビット数を初期化しておく
                    Node[i].BitNum = 0 ;
                    
                    // 一時的に数値データから遡っていったときのビット列を保存する処理の準備
                    TempBitIndex = 0 ;
                    TempBitCount = 0 ;
                    TempBitArray[TempBitIndex] = 0 ;
                    
                    // 何処かと結合されている限りカウントし続ける(天辺は何処とも結合されていないので終点だと分かる)
                    for( NodeIndex = ( int )i ; Node[NodeIndex].ParentNode != -1 ; NodeIndex = Node[NodeIndex].ParentNode )
                    {
                        // 配列要素一つに入るビットデータは８個なので、同じ配列要素に
                        // 既に８個保存していたら次の配列要素に保存先を変更する
                        if( TempBitCount == 8 )
                        {
                            TempBitCount = 0 ;
                            TempBitIndex ++ ;
                            TempBitArray[TempBitIndex] = 0 ;
                        }
                        
                        // 新しく書き込む情報で今までのデータを上書きしてしまわないように１ビット左にシフトする
                        TempBitArray[TempBitIndex] <<= 1 ;

                        // 結合データに割り振られたインデックスを最下位ビット(一番右側のビット)に書き込む
                        TempBitArray[TempBitIndex] |= (unsigned char)Node[NodeIndex].Index ;

                        // 保存したビット数を増やす
                        TempBitCount ++ ;

                        // ビット数を増やす
                        Node[i].BitNum ++ ;
                    }
                }
				
                // TempBitArray に溜まったデータは数値データから結合データを天辺に向かって
                // 上へ上へと遡っていった時のビット列なので、逆さまにしないと圧縮後のビット
                // 配列として使えない(展開時に天辺の結合データから数値データまで辿ることが
                // 出来ない)ので、順序を逆さまにしたものを数値データ内のビット列バッファに保存する
                {
                    BitCount = 0 ;
                    BitIndex = 0 ;
                    
                    // 最初のバッファを初期化しておく
                    // (全部 論理和(or)演算 で書き込むので、最初から１になっている
                    // ビットに０を書き込んでも１のままになってしまうため)
                    Node[i].BitArray[BitIndex] = 0 ;
                    
                    // 一時的に保存しておいたビット列の最初まで遡る
                    while( TempBitIndex >= 0 )
                    {
                        // 書き込んだビット数が一つの配列要素に入る８ビットに
                        // 達してしまったら次の配列要素に移る
                        if( BitCount == 8 )
                        {
                            BitCount = 0 ;
                            BitIndex ++ ;
                            Node[i].BitArray[BitIndex] = 0 ;
                        }

                        // まだ何も書き込まれていないビットアドレスに１ビット書き込む
                        Node[i].BitArray[BitIndex] |= (unsigned char)( ( TempBitArray[TempBitIndex] & 1 ) << BitCount ) ;
						
                        // 書き込み終わったビットはもういらないので次のビットを
                        // 書き込めるように１ビット右にシフトする
                        TempBitArray[TempBitIndex] >>= 1 ;
                        
                        // １ビット書き込んだので残りビット数を１個減らす
                        TempBitCount -- ;
                        
                        // もし現在書き込み元となっている配列要素に書き込んでいない
                        // ビット情報が無くなったら次の配列要素に移る
                        if( TempBitCount == 0 )
                        {
                            TempBitIndex -- ;
                            TempBitCount = 8 ;
                        }
                        
                        // 書き込んだビット数を増やす
                        BitCount ++ ;
                    }
                }
            }
		}
    }

    // 解凍処理
    {
        unsigned char *PressData ;
        int PressBitCounter, PressBitData, Index, NodeIndex ;
		int NodeIndexTable[ 512 ] ;
		int j ;

		// 各ビット配列がどのノードに繋がるかのテーブルを作成する
		{
			WORD BitMask[ 9 ] ;

			for( i = 0 ; i < 9 ; i ++ )
			{
				BitMask[ i ] = ( WORD )( ( 1 << ( i + 1 ) ) - 1 ) ;
			}

			for( i = 0 ; i < 512 ; i ++ )
			{
				NodeIndexTable[ i ] = -1 ;

				// ビット列に適合したノードを探す
				for( j = 0 ; j < 256 + 254 ; j ++ )
				{
					WORD BitArray01 ;

					if( Node[ j ].BitNum > 9 )
					{
						continue ;
					}

					BitArray01 = ( WORD )Node[ j ].BitArray[ 0 ] | ( Node[ j ].BitArray[ 1 ] << 8 ) ;
					if( ( i & BitMask[ Node[ j ].BitNum - 1 ] ) == ( BitArray01 & BitMask[ Node[ j ].BitNum - 1 ] ) )
					{
						NodeIndexTable[ i ] = j ;
						break ;
					}
				}
			}
		}

        // 圧縮データ本体の先頭アドレスをセット
        // (圧縮データ本体は元のサイズ、圧縮後のサイズ、各数値の出現数等を
        // 格納するデータ領域の後にある)
        PressData = PressPoint + HeadSize ;

        // 解凍したデータの格納アドレスを初期化
        DestSizeCounter = 0 ;
        
        // 圧縮データの参照アドレスを初期化
        PressSizeCounter = 0 ;
        
        // 圧縮ビットデータのカウンタを初期化
        PressBitCounter = 0 ;
        
        // 圧縮データの１バイト目をセット
        PressBitData = PressData[PressSizeCounter] ;

        // 圧縮前のデータサイズになるまで解凍処理を繰り返す
        for( DestSizeCounter = 0 ; DestSizeCounter < DestSize ; DestSizeCounter ++ )
        {
            // ビット列から数値データを検索する
            {
				// 最後の17byte分のデータは天辺から探す( 最後の次のバイトを読み出そうとしてメモリの不正なアクセスになる可能性があるため )
				if( DestSizeCounter >= DestSize - 17 )
				{
					// 結合データの天辺は一番最後の結合データが格納される５１０番目(０番から数える)
					// 天辺から順に下に降りていく
					NodeIndex = 510 ;
				}
				else
				{
					// それ以外の場合はテーブルを使用する

                    // もし PressBitData に格納されている全ての
                    // ビットデータを使い切ってしまった場合は次の
                    // ビットデータをセットする
                    if( PressBitCounter == 8 )
                    {
                        PressSizeCounter ++ ;
                        PressBitData = PressData[PressSizeCounter] ;
                        PressBitCounter = 0 ;
                    }

					// 圧縮データを9bit分用意する
					PressBitData = ( PressBitData | ( PressData[ PressSizeCounter + 1 ] << ( 8 - PressBitCounter ) ) ) & 0x1ff ;

					// テーブルから最初の結合データを探す
					NodeIndex = NodeIndexTable[ PressBitData ] ;

					// 使った分圧縮データのアドレスを進める
					PressBitCounter += Node[ NodeIndex ].BitNum ;
					if( PressBitCounter >= 16 )
					{
						PressSizeCounter += 2 ;
						PressBitCounter -= 16 ;
						PressBitData = PressData[PressSizeCounter] >> PressBitCounter ;
					}
					else
					if( PressBitCounter >= 8 )
					{
						PressSizeCounter ++ ;
						PressBitCounter -= 8 ;
						PressBitData = PressData[PressSizeCounter] >> PressBitCounter ;
					}
					else
					{
						PressBitData >>= Node[ NodeIndex ].BitNum ;
					}
				}
                
                // 数値データに辿り着くまで結合データを下りていく
                while( NodeIndex > 255 )
                {
                    // もし PressBitData に格納されている全ての
                    // ビットデータを使い切ってしまった場合は次の
                    // ビットデータをセットする
                    if( PressBitCounter == 8 )
                    {
                        PressSizeCounter ++ ;
                        PressBitData = PressData[PressSizeCounter] ;
                        PressBitCounter = 0 ;
                    }
                    
                    // １ビット取得する
                    Index = PressBitData & 1 ;
                    
                    // 使用した１ビット分だけ右にシフトする
                    PressBitData >>= 1 ;
                    
                    // 使用したビット数を一個増やす
                    PressBitCounter ++ ;
                    
                    // 次の要素(結合データか数値データかはまだ分からない)に移る
                    NodeIndex = Node[NodeIndex].ChildNode[Index] ;
                }
            }

            // 辿り着いた数値データを出力
            DestPoint[DestSizeCounter] = (unsigned char)NodeIndex ;
        }
    }

    // 解凍後のサイズを返す
    return OriginalSize ;
}

// バイナリデータを半角文字列に変換する( 戻り値:変換後のデータサイズ )
static unsigned char BinToChar128Table[ 128 ] =
{
	0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40,
	0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,
	0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60,
	0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70,
	0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0xA1, 0xA2,
	0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30,
	0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2,
	0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC0, 0xC1, 0xC2,
} ;
static unsigned char Char128ToBinTable[ 256 ] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e,
	0x5f, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
	0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e,
	0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e,
	0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e,
	0x3f, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x4e, 0x4f, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c,
	0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c,
	0x7d, 0x7e, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
} ;
extern DWORD BinToChar128( void *Src, DWORD SrcSize, void *Dest )
{
	unsigned int DestSize ;

	DestSize  = SrcSize + ( SrcSize + 6 ) / 7 + 5 ;

	if( Dest != NULL )
	{
		unsigned int PackNum ;
		unsigned int ModNum ;
		unsigned char *DestP ;
		unsigned char *SrcP ;
		unsigned int i ;

		DestP = ( unsigned char * )Dest ;
		SrcP  = ( unsigned char * )&SrcSize ;

		DestP[ 0 ] = BinToChar128Table[                                 ( SrcP[ 0 ] >> 1 ) ] ;
		DestP[ 1 ] = BinToChar128Table[ ( ( SrcP[ 0 ] & 0x01 ) << 6 ) | ( SrcP[ 1 ] >> 2 ) ] ;
		DestP[ 2 ] = BinToChar128Table[ ( ( SrcP[ 1 ] & 0x03 ) << 5 ) | ( SrcP[ 2 ] >> 3 ) ] ;
		DestP[ 3 ] = BinToChar128Table[ ( ( SrcP[ 2 ] & 0x07 ) << 4 ) | ( SrcP[ 3 ] >> 4 ) ] ;
		DestP[ 4 ] = BinToChar128Table[ ( ( SrcP[ 3 ] & 0x0f ) << 3 )                      ] ;

		DestP += 5 ;

		PackNum = SrcSize / 7 ;
		ModNum  = SrcSize - PackNum * 7 ;
		SrcP  = ( unsigned char * )Src ;
		for( i = 0 ; i < PackNum ; i ++ )
		{
			DestP[ 0 ] = BinToChar128Table[                                 ( SrcP[ 0 ] >> 1 ) ] ;
			DestP[ 1 ] = BinToChar128Table[ ( ( SrcP[ 0 ] & 0x01 ) << 6 ) | ( SrcP[ 1 ] >> 2 ) ] ;
			DestP[ 2 ] = BinToChar128Table[ ( ( SrcP[ 1 ] & 0x03 ) << 5 ) | ( SrcP[ 2 ] >> 3 ) ] ;
			DestP[ 3 ] = BinToChar128Table[ ( ( SrcP[ 2 ] & 0x07 ) << 4 ) | ( SrcP[ 3 ] >> 4 ) ] ;
			DestP[ 4 ] = BinToChar128Table[ ( ( SrcP[ 3 ] & 0x0f ) << 3 ) | ( SrcP[ 4 ] >> 5 ) ] ;
			DestP[ 5 ] = BinToChar128Table[ ( ( SrcP[ 4 ] & 0x1f ) << 2 ) | ( SrcP[ 5 ] >> 6 ) ] ;
			DestP[ 6 ] = BinToChar128Table[ ( ( SrcP[ 5 ] & 0x3f ) << 1 ) | ( SrcP[ 6 ] >> 7 ) ] ;
			DestP[ 7 ] = BinToChar128Table[ ( ( SrcP[ 6 ] & 0x7f )      )                      ] ;

			DestP += 8 ;
			SrcP  += 7 ;
		}

		if( ModNum != 0 )
		{
			DestP[ 0 ] = BinToChar128Table[                                 ( SrcP[ 0 ] >> 1 ) ] ;
			if( ModNum == 1 )
			{
				DestP[ 1 ] = BinToChar128Table[ ( ( SrcP[ 0 ] & 0x01 ) << 6 ) ] ;
			}
			else
			{
				DestP[ 1 ] = BinToChar128Table[ ( ( SrcP[ 0 ] & 0x01 ) << 6 ) | ( SrcP[ 1 ] >> 2 ) ] ;
				if( ModNum == 2 )
				{
					DestP[ 2 ] = BinToChar128Table[ ( ( SrcP[ 1 ] & 0x03 ) << 5 ) ] ;
				}
				else
				{
					DestP[ 2 ] = BinToChar128Table[ ( ( SrcP[ 1 ] & 0x03 ) << 5 ) | ( SrcP[ 2 ] >> 3 ) ] ;
					if( ModNum == 3 )
					{
						DestP[ 3 ] = BinToChar128Table[ ( ( SrcP[ 2 ] & 0x07 ) << 4 ) ] ;
					}
					else
					{
						DestP[ 3 ] = BinToChar128Table[ ( ( SrcP[ 2 ] & 0x07 ) << 4 ) | ( SrcP[ 3 ] >> 4 ) ] ;
						if( ModNum == 4 )
						{
							DestP[ 4 ] = BinToChar128Table[ ( ( SrcP[ 3 ] & 0x0f ) << 3 ) ] ;
						}
						else
						{
							DestP[ 4 ] = BinToChar128Table[ ( ( SrcP[ 3 ] & 0x0f ) << 3 ) | ( SrcP[ 4 ] >> 5 ) ] ;
							if( ModNum == 5 )
							{
								DestP[ 5 ] = BinToChar128Table[ ( ( SrcP[ 4 ] & 0x1f ) << 2 ) ] ;
							}
							else
							{
								DestP[ 5 ] = BinToChar128Table[ ( ( SrcP[ 4 ] & 0x1f ) << 2 ) | ( SrcP[ 5 ] >> 6 ) ] ;
								if( ModNum == 6 )
								{
									DestP[ 6 ] = BinToChar128Table[ ( ( SrcP[ 5 ] & 0x3f ) << 1 ) ] ;
								}
								else
								{
									DestP[ 6 ] = BinToChar128Table[ ( ( SrcP[ 5 ] & 0x3f ) << 1 ) | ( SrcP[ 6 ] >> 7 ) ] ;
									DestP[ 7 ] = BinToChar128Table[ ( ( SrcP[ 6 ] & 0x7f )      )                      ] ;
								}
							}
						}
					}
				}
			}
		}
	}

	return DestSize ;
}

// 半角文字列をバイナリデータに変換する( 戻り値:変換後のデータサイズ )
extern DWORD Char128ToBin( void *Src, void *Dest )
{
	unsigned int DestSize ;
	unsigned char *SrcP ;
	unsigned char *DestP ;

	SrcP    = ( unsigned char * )Src ;
	DestP   = ( unsigned char * )&DestSize ;

	DestP[ 0 ] = ( unsigned char )( ( Char128ToBinTable[ SrcP[ 0 ] ] << 1 ) | ( Char128ToBinTable[ SrcP[ 1 ] ] >> 6 ) ) ;
	DestP[ 1 ] = ( unsigned char )( ( Char128ToBinTable[ SrcP[ 1 ] ] << 2 ) | ( Char128ToBinTable[ SrcP[ 2 ] ] >> 5 ) ) ;
	DestP[ 2 ] = ( unsigned char )( ( Char128ToBinTable[ SrcP[ 2 ] ] << 3 ) | ( Char128ToBinTable[ SrcP[ 3 ] ] >> 4 ) ) ;
	DestP[ 3 ] = ( unsigned char )( ( Char128ToBinTable[ SrcP[ 3 ] ] << 4 ) | ( Char128ToBinTable[ SrcP[ 4 ] ] >> 3 ) ) ;

	SrcP += 5 ;

	if( Dest != NULL )
	{
		unsigned int PackNum ;
		unsigned int ModNum ;
		unsigned int i ;

		PackNum = DestSize / 7 ;
		ModNum  = DestSize - PackNum * 7 ;
		DestP = ( unsigned char * )Dest ;
		for( i = 0 ; i < PackNum ; i ++ )
		{
			DestP[ 0 ] = ( unsigned char )( ( Char128ToBinTable[ SrcP[ 0 ] ] << 1 ) | ( Char128ToBinTable[ SrcP[ 1 ] ] >> 6 ) ) ;
			DestP[ 1 ] = ( unsigned char )( ( Char128ToBinTable[ SrcP[ 1 ] ] << 2 ) | ( Char128ToBinTable[ SrcP[ 2 ] ] >> 5 ) ) ;
			DestP[ 2 ] = ( unsigned char )( ( Char128ToBinTable[ SrcP[ 2 ] ] << 3 ) | ( Char128ToBinTable[ SrcP[ 3 ] ] >> 4 ) ) ;
			DestP[ 3 ] = ( unsigned char )( ( Char128ToBinTable[ SrcP[ 3 ] ] << 4 ) | ( Char128ToBinTable[ SrcP[ 4 ] ] >> 3 ) ) ;
			DestP[ 4 ] = ( unsigned char )( ( Char128ToBinTable[ SrcP[ 4 ] ] << 5 ) | ( Char128ToBinTable[ SrcP[ 5 ] ] >> 2 ) ) ;
			DestP[ 5 ] = ( unsigned char )( ( Char128ToBinTable[ SrcP[ 5 ] ] << 6 ) | ( Char128ToBinTable[ SrcP[ 6 ] ] >> 1 ) ) ;
			DestP[ 6 ] = ( unsigned char )( ( Char128ToBinTable[ SrcP[ 6 ] ] << 7 ) | ( Char128ToBinTable[ SrcP[ 7 ] ]      ) ) ;

			DestP += 7 ;
			SrcP  += 8 ;
		}

		if( ModNum != 0 )
		{
			DestP[ 0 ] = ( unsigned char )( ( Char128ToBinTable[ SrcP[ 0 ] ] << 1 ) | ( Char128ToBinTable[ SrcP[ 1 ] ] >> 6 ) ) ;
			if( ModNum > 1 )
			{
				DestP[ 1 ] = ( unsigned char )( ( Char128ToBinTable[ SrcP[ 1 ] ] << 2 ) | ( Char128ToBinTable[ SrcP[ 2 ] ] >> 5 ) ) ;
				if( ModNum > 2 )
				{
					DestP[ 2 ] = ( unsigned char )( ( Char128ToBinTable[ SrcP[ 2 ] ] << 3 ) | ( Char128ToBinTable[ SrcP[ 3 ] ] >> 4 ) ) ;
					if( ModNum > 3 )
					{
						DestP[ 3 ] = ( unsigned char )( ( Char128ToBinTable[ SrcP[ 3 ] ] << 4 ) | ( Char128ToBinTable[ SrcP[ 4 ] ] >> 3 ) ) ;
						if( ModNum > 4 )
						{
							DestP[ 4 ] = ( unsigned char )( ( Char128ToBinTable[ SrcP[ 4 ] ] << 5 ) | ( Char128ToBinTable[ SrcP[ 5 ] ] >> 2 ) ) ;
							if( ModNum > 5 )
							{
								DestP[ 5 ] = ( unsigned char )( ( Char128ToBinTable[ SrcP[ 5 ] ] << 6 ) | ( Char128ToBinTable[ SrcP[ 6 ] ] >> 1 ) ) ;
								if( ModNum > 6 )
								{
									DestP[ 6 ] = ( unsigned char )( ( Char128ToBinTable[ SrcP[ 6 ] ] << 7 ) | ( Char128ToBinTable[ SrcP[ 7 ] ]      ) ) ;
								}
							}
						}
					}
				}
			}
		}
	}

	return DestSize ;
}





// バイナリデータを元に CRC32 のハッシュ値を計算する
extern DWORD HashCRC32( const void *SrcData, size_t SrcDataSize )
{
	static DWORD CRC32Table[ 256 ] ;
	static int CRC32TableInit = 0 ;
	DWORD CRC = 0xffffffff ;
	BYTE *SrcByte = ( BYTE * )SrcData ;
	DWORD i ;

	// テーブルが初期化されていなかったら初期化する
	if( CRC32TableInit == 0 )
	{
		DWORD Magic = 0xedb88320 ;	// 0x4c11db7 をビットレベルで順番を逆にしたものが 0xedb88320
		DWORD j ;

		for( i = 0; i < 256; i++ )
		{
			DWORD Data = i ;
			for( j = 0; j < 8; j++ )
			{
				int b = ( Data & 1 ) ;
				Data >>= 1 ;
				if( b != 0 )
				{
					Data ^= Magic ;
				}
			}
			CRC32Table[ i ] = Data ;
		}

		// テーブルを初期化したフラグを立てる
		CRC32TableInit = 1 ;
	}

	for( i = 0 ; i < SrcDataSize ; i ++ )
	{
		CRC = CRC32Table[ ( BYTE )( CRC ^ SrcByte[ i ] ) ] ^ ( CRC >> 8 ) ;
	}

	return CRC ^ 0xffffffff ;
}



#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

