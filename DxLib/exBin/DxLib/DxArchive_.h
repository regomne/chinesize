// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		通信プログラムヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXARCHIVE_H__
#define __DXARCHIVE_H__

// インクルード -------------------------------------------------------------------
#include "DxCompileConfig.h"
#include "DxLib.h"
#include "DxThread.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

// ＤＸアーカイブ関連

/*
	バージョンごとの違い

	0x0002 DARC_FILEHEAD に PressDataSize を追加
	0x0004 DARC_HEAD に CharCodeFormat を追加
	0x0005 暗号化処理を一部変更
	0x0006 64bit化
	0x0007 暗号化処理を変更( パスワードを指定した場合の解読難度を向上 )
	0x0008 暗号鍵の長さを56bitに変更


	データマップ
		
	DXARC_HEAD
	ファイル実データ
	ファイル名テーブル
	DXARC_FILEHEAD テーブル
	DXARC_DIRECTORY テーブル


	ファイル名のデータ形式
	2byte:文字列の長さ(バイトサイズ÷４)
	2byte:文字列のパリティデータ(全ての文字の値を足したもの)
	英字は大文字に変換されたファイル名のデータ(４の倍数のサイズ)
	英字が大文字に変換されていないファイル名のデータ
*/

#define DXAHEAD							*((WORD *)"DX")		// ヘッダ
#define DXAVER							(0x0008)			// バージョン
#define DXAVER_MIN						(0x0008)			// 対応している最低バージョン
#define DXA_KEY_BYTES					(7)					// 鍵のバイト数
#define DXA_KEY_STRING_LENGTH			(63)				// 鍵用文字列の長さ
#define DXA_KEY_STRING_MAXLENGTH		(2048)				// 鍵用文字列バッファのサイズ
#define DXA_DIR_MAXARCHIVENUM			(4096)				// 同時に開いておけるアーカイブファイルの数
#define DXA_DIR_MAXFILENUM				(32768)				// 同時に開いておけるファイルの数
#define DXA_MAXDRIVENUM					(64)				// 対応するドライブの最大数
	
// フラグ
#define DXA_FLAG_NO_KEY					(0x00000001)	// 鍵処理無し
#define DXA_FLAG_NO_HEAD_PRESS			(0x00000002)	// ヘッダの圧縮無し

// 構造体定義 --------------------------------------------------------------------

// アーカイブデータの最初のヘッダ
struct DXARC_HEAD
{
	WORD						Head ;							// ＩＤ
	WORD						Version ;						// バージョン
	DWORD						HeadSize ;						// ヘッダ情報の DXARC_HEAD を抜いた全サイズ
	ULONGLONG					DataStartAddress ;				// 最初のファイルのデータが格納されているデータアドレス(ファイルの先頭アドレスをアドレス０とする)
	ULONGLONG					FileNameTableStartAddress ;		// ファイル名テーブルの先頭アドレス(ファイルの先頭アドレスをアドレス０とする)
	ULONGLONG					FileTableStartAddress ;			// ファイルテーブルの先頭アドレス(メンバ変数 FileNameTableStartAddress のアドレスを０とする)
	ULONGLONG					DirectoryTableStartAddress ;	// ディレクトリテーブルの先頭アドレス(メンバ変数 FileNameTableStartAddress のアドレスを０とする)
																// アドレス０から配置されている DXARC_DIRECTORY 構造体がルートディレクトリ
	DWORD						CharCodeFormat ;				// ファイル名に使用しているコードページ番号
	DWORD						Flags ;							// フラグ( DXA_FLAG_NO_KEY 等 )
	BYTE						HuffmanEncodeKB ;				// ファイルの前後のハフマン圧縮するサイズ( 単位：キロバイト 0xff の場合はすべて圧縮する )
	BYTE						Reserve[ 15 ] ;					// 予約領域
} ;

// ファイルの時間情報
struct DXARC_FILETIME
{
	ULONGLONG					Create ;						// 作成時間
	ULONGLONG					LastAccess ;					// 最終アクセス時間
	ULONGLONG					LastWrite ;						// 最終更新時間
} ;

// ファイル名データ構造体
struct DXARC_FILENAME
{
	WORD						Length ;						// 文字列の長さ÷４
	WORD						Parity ;						// パリティ情報
} ;

// ファイル格納情報
struct DXARC_FILEHEAD
{
	ULONGLONG					NameAddress ;					// ファイル名が格納されているアドレス( ARCHIVE_HEAD構造体 のメンバ変数 FileNameTableStartAddress のアドレスをアドレス０とする) 
	ULONGLONG					Attributes ;					// ファイル属性
	DXARC_FILETIME				Time ;							// 時間情報
	ULONGLONG					DataAddress ;					// ファイルが格納されているアドレス
																//			ファイルの場合：DXARC_HEAD構造体 のメンバ変数 DataStartAddress が示すアドレスをアドレス０とする
																//			ディレクトリの場合：DXARC_HEAD構造体 のメンバ変数 DirectoryTableStartAddress のが示すアドレスをアドレス０とする
	ULONGLONG					DataSize ;						// ファイルのデータサイズ
	ULONGLONG					PressDataSize ;					// 圧縮後のデータのサイズ( 0xffffffff:圧縮されていない ) ( Ver0x0002 で追加された )
	ULONGLONG					HuffPressDataSize ;				// ハフマン圧縮後のデータのサイズ( 0xffffffffffffffff:圧縮されていない ) ( Ver0x0008 で追加された )
} ;

// ディレクトリ格納情報
struct DXARC_DIRECTORY
{
	ULONGLONG					DirectoryAddress ;				// 自分の DXARC_FILEHEAD が格納されているアドレス( DXARC_HEAD 構造体 のメンバ変数 FileTableStartAddress が示すアドレスをアドレス０とする)
	ULONGLONG					ParentDirectoryAddress ;		// 親ディレクトリの DXARC_DIRECTORY が格納されているアドレス( DXARC_HEAD構造体 のメンバ変数 DirectoryTableStartAddress が示すアドレスをアドレス０とする)
	ULONGLONG					FileHeadNum ;					// ディレクトリ内のファイルの数
	ULONGLONG					FileHeadAddress ;				// ディレクトリ内のファイルのヘッダ列が格納されているアドレス( DXARC_HEAD構造体 のメンバ変数 FileTableStartAddress が示すアドレスをアドレス０とする) 
} ;



// ファイル名検索用データ構造体
struct DXARC_SEARCHDATA
{
	BYTE						FileName[ 1024 ] ;				// ファイル名
	WORD						Parity ;						// パリティ情報
	WORD						PackNum ;						// 文字列の長さ÷４
} ;

// 情報テーブル構造体
struct DXARC_TABLE
{
	BYTE						*Top ;							// 情報テーブルの先頭ポインタ
	BYTE						*FileTable ;					// ファイル情報テーブルへのポインタ
	BYTE						*DirectoryTable ;				// ディレクトリ情報テーブルへのポインタ
	BYTE						*NameTable ;					// 名前情報テーブルへのポインタ
} ;

// アーカイブ処理用情報構造体
struct DXARC
{
	DXARC_HEAD					Head ;							// アーカイブのヘッダ
	int							CharCodeFormat ;				// 文字コード形式
	DWORD_PTR					ReadAccessOnlyFilePointer ;		// アーカイブファイルのポインタ	
	void						*MemoryImage ;					// メモリイメージを開いた場合のアドレス
	DXARC_TABLE					Table ;							// 各テーブルへの先頭アドレスが格納された構造体
	DXARC_DIRECTORY				*CurrentDirectory ;				// カレントディレクトリデータへのポインタ

	wchar_t						FilePath[ 1024 ] ;				// ファイルパス
	bool						NoKey ;							// 鍵処理を行わないかどうか
	unsigned char				Key[ DXA_KEY_BYTES ] ;			// 鍵
	char						KeyString[ DXA_KEY_STRING_LENGTH + 1 ] ;	// 鍵文字列
	size_t						KeyStringBytes ;				// 鍵文字列のバイト数
	int							MemoryOpenFlag ;				// メモリ上のファイルを開いているか、フラグ
	int							UserMemoryImageFlag ;			// ユーザーが展開したメモリイメージを使用しているか、フラグ
	LONGLONG					MemoryImageSize ;				// メモリ上のファイルから開いていた場合のイメージのサイズ
	int							MemoryImageCopyFlag ;			// メモリ上のイメージをコピーして使用しているかどうかのフラグ
	int							MemoryImageReadOnlyFlag ;		// メモリ上のイメージが読み取り専用かどうかのフラグ
	void						*MemoryImageOriginal ;			// メモリ上のイメージをコピーして使用している場合の、コピー元のデータが格納されているメモリ領域

	int							ASyncOpenFlag ;					// 非同期読み込み中かフラグ( TRUE:非同期読み込み中 FALSE:違う )
	DWORD_PTR					ASyncOpenFilePointer ;			// 非同期オープン処理に使用するファイルのポインタ
} ;

// 非同期読み込み状態
#define DXARC_STREAM_ASYNCSTATE_IDLE			(0)				// 何もしていない
#define DXARC_STREAM_ASYNCSTATE_PRESSREAD		(1)				// 圧縮されたファイルを読み込み中
#define DXARC_STREAM_ASYNCSTATE_READ			(2)				// データを読み込み中

// アーカイブ内のファイルを通常のファイル読み込みのように処理する為の構造体
struct DXARC_STREAM
{
	DXARC						*Archive ;						// アーカイブデータへのポインタ
	DXARC_FILEHEAD				*FileHead ;						// ファイル情報へのポインタ
	void						*DecodeDataBuffer ;				// 解凍したデータが格納されているメモリ領域へのポインタ( ファイルが圧縮データだった場合のみ有効 )
	void						*DecodeTempBuffer ;				// 圧縮データ一時保存用メモリ領域へのポインタ
	DWORD_PTR					ReadOnlyFilePointer ;			// アーカイブファイルのポインタ

	bool						NoKey ;							// 鍵処理を行わないかどうか
	unsigned char				Key[ DXA_KEY_BYTES ] ;			// 鍵

	int							EOFFlag ;						// EOFフラグ
	ULONGLONG					FilePoint ;						// ファイルポインタ
	int							UseASyncReadFlag ;				// 非同期読み込みフラグ
	int							ASyncState ;					// 非同期読み込み状態( DXARC_STREA_ASYNCSTATE 系 )
	ULONGLONG					ASyncReadFileAddress ;			// 非同期読み込み時のファイルポインタ

	void						*ReadBuffer ;					// 非同期読み込み時の引数に渡されたバッファへのポインタ
	LONGLONG					ReadSize ;						// 非同期読み込み時の引数に渡された読み込みサイズへのポインタ
} ;



// アーカイブファイルをディレクトリに見立てる処理用の開いているアーカイブファイルの情報
struct DXARC_DIR_ARCHIVE
{
	int							UseCounter ;					// このアーカイブファイルが使用されている数
	DXARC						Archive ;						// アーカイブファイルデータ
	wchar_t						Path[ 520 ] ;					// アーカイブファイルのパス
} ;

// アーカイブファイルをディレクトリに見立てる処理用の開いているアーカイブファイル中のファイルの情報
struct DXARC_DIR_FILE
{
	int							UseArchiveFlag ;				// アーカイブファイルを使用しているかフラグ
	DWORD_PTR					ReadOnlyFilePointer ;			// アーカイブを使用していない場合の、ファイルポインタ
	DWORD						UseArchiveIndex ;				// アーカイブを使用している場合、使用しているアーカイブファイルデータのインデックス
	DXARC_STREAM				DXAStream ;						// アーカイブファイルを使用している場合のファイルアクセス用データ
} ;

// アーカイブをディレクトリに見立てる処理用の構造体
struct DXARC_DIR
{
	int							InitializeFlag ;				// 初期化完了フラグ
	DX_CRITICAL_SECTION			CriticalSection ;				// 同時にアーカイブファイルのリストを操作しないようにするためのクリティカルセクション

	int							NotArchivePathCharUp ;			// アーカイブパスの英字の小文字を大文字にしないかどうか

	DXARC_DIR_ARCHIVE			*Archive[DXA_DIR_MAXARCHIVENUM] ;	// 使用しているアーカイブファイルのデータ
	DXARC_DIR_FILE				*File[DXA_DIR_MAXFILENUM] ;		// 開いているファイルのデータ
	wchar_t						ArchiveExtension[ 64 ] ;		// アーカイブファイルの拡張子
	int							ArchiveExtensionLength ;		// アーカイブファイルの拡張子の長さ
	int							DXAPriority ;					// ＤＸアーカイブファイルの優先度( 1:フォルダ優先 0:DXアーカイブ優先 )

	int							ValidKeyString ;						// KeyString が有効かどうか
	char						KeyString[ DXA_KEY_STRING_LENGTH + 1 ] ;	// 鍵文字列

	int							ArchiveNum ;					// 使用しているアーカイブファイルの数
	int							FileNum ;						// 開いているファイルの数

	int							BackUseArchiveIndex ;			// 前回使用したアーカイブのインデックス
	wchar_t						BackUseDirectory[ 512 * 3 ] ;	// 前回使用したディレクトリパス
	int							BackUseDirectoryPathLength ;	// 前回使用したディレクトリパスの長さ
} ;

// テーブル-----------------------------------------------------------------------

// 内部大域変数宣言 -------------------------------------------------------------

extern DXARC_DIR DX_ArchiveDirData ;

// 関数プロトタイプ宣言-----------------------------------------------------------

#ifndef DX_NON_DXA

extern	int			DXA_Initialize(					DXARC *DXA ) ;													// アーカイブファイルを扱う為の構造体を初期化する
extern	int			DXA_Terminate(					DXARC *DXA ) ;													// アーカイブファイルを扱う為の構造体の後始末をする
extern	int			DXA_OpenArchiveFromFile(		DXARC *DXA, const wchar_t *ArchivePath, const char *KeyString = NULL ) ;							// アーカイブファイルを開く( 0:成功  -1:失敗 )
extern	int			DXA_OpenArchiveFromFileUseMem(	DXARC *DXA, const wchar_t *ArchivePath, const char *KeyString = NULL , int ASync = FALSE ) ;		// アーカイブファイルを開き最初にすべてメモリ上に読み込んでから処理する( 0:成功  -1:失敗 )
extern	int			DXA_OpenArchiveFromMem(			DXARC *DXA, void *ArchiveImage, int ArchiveSize, int ArchiveImageCopyFlag, int ArchiveImageReadOnlyFlag, const char *KeyString = NULL, const wchar_t *EmulateArchivePath = NULL ) ;				// メモリ上にあるアーカイブファイルイメージを開く( 0:成功  -1:失敗 )
extern	int			DXA_CheckIdle(					DXARC *DXA ) ;													// アーカイブファイルを扱う準備が整ったかを得る( TRUE:整っている  FALSE:整っていない )
extern	int			DXA_CloseArchive(				DXARC *DXA ) ;													// アーカイブファイルを閉じる

//extern int		DXA_LoadFile(					DXARC *DXA, const char *FilePath, void *Buffer, ULONGLONG BufferSize ) ;	// アーカイブファイル中の指定のファイルをメモリに読み込む( -1:エラー 0以上:ファイルサイズ )
extern	void *		DXA_GetFileImage(				DXARC *DXA ) ;													// アーカイブファイルをメモリに読み込んだ場合のファイルイメージが格納されている先頭アドレスを取得する( DXA_OpenArchiveFromFileUseMem 若しくは DXA_OpenArchiveFromMem で開いた場合に有効、データが圧縮されている場合は注意 )
extern	int			DXA_GetFileInfo(				DXARC *DXA, int CharCodeFormat, const char *FilePath, int *Position, int *Size ) ;	// アーカイブファイル中の指定のファイルのファイル内の位置とファイルの大きさを得る( -1:エラー )
extern	int			DXA_ChangeCurrentDir(			DXARC *DXA, int CharCodeFormat, const char *DirPath ) ;								// アーカイブ内のカレントディレクトリを変更する( 0:成功  -1:失敗 )
//extern int		DXA_GetCurrentDir(				DXARC *DXA, int CharCodeFormat, char *DirPathBuffer, int BufferSize ) ;				// アーカイブ内のカレントディレクトリを取得する
extern	DWORD_PTR	DXA_FindFirst(					DXARC *DXA, const BYTE *FilePath, FILEINFOW *Buffer ) ;			// アーカイブ内のオブジェクトを検索する( -1:エラー -1以外:DXA検索ハンドル )
extern	int			DXA_FindNext(					DWORD_PTR DxaFindHandle, FILEINFOW *Buffer ) ;					// アーカイブ内のオブジェクトを検索する( -1:エラー 0:成功 )
extern	int			DXA_FindClose(					DWORD_PTR DxaFindHandle ) ;										// アーカイブ内のオブジェクト検索を終了する

extern	int			DXA_STREAM_Initialize(			DXARC_STREAM *DXAStream, DXARC *DXA, const BYTE *FilePath, int UseASyncReadFlag ) ;	// アーカイブファイル内のファイルを開く
extern	int			DXA_STREAM_Terminate(			DXARC_STREAM *DXAStream ) ;										// アーカイブファイル内のファイルを閉じる
extern	int			DXA_STREAM_Read(				DXARC_STREAM *DXAStream, void *Buffer, size_t ReadLength ) ;	// ファイルの内容を読み込む
extern	int			DXA_STREAM_Seek(				DXARC_STREAM *DXAStream, LONGLONG SeekPoint, int SeekMode ) ;	// ファイルポインタを変更する
extern	LONGLONG	DXA_STREAM_Tell(				DXARC_STREAM *DXAStream ) ;										// 現在のファイルポインタを得る
extern	int			DXA_STREAM_Eof(					DXARC_STREAM *DXAStream ) ;										// ファイルの終端に来ているか、のフラグを得る
extern	int			DXA_STREAM_IdleCheck(			DXARC_STREAM *DXAStream ) ;										// 読み込み処理が完了しているかどうかを調べる
extern	LONGLONG	DXA_STREAM_Size(				DXARC_STREAM *DXAStream ) ;										// ファイルのサイズを取得する


extern	int			DXA_DIR_Initialize(				void ) ;														// アーカイブをディレクトリに見立てる処理の初期化
extern	int			DXA_DIR_Terminate(				void ) ;														// アーカイブをディレクトリに見立てる処理の後始末
extern	int			DXA_DIR_SetNotArchivePathCharUp( int NotArchivePathCharUpFlag ) ;								// アーカイブファイルのパスを大文字にしないかどうかのフラグをセットする
extern	int			DXA_DIR_SetArchiveExtension(	const wchar_t *Extension = NULL ) ;								// アーカイブファイルの拡張子を設定する
extern	int			DXA_DIR_SetDXArchivePriority(	int Priority = 0 ) ;											// アーカイブファイルと通常のフォルダのどちらも存在した場合、どちらを優先させるかを設定する( 1:フォルダを優先 0:ＤＸアーカイブファイルを優先(デフォルト) )
extern	int			DXA_DIR_SetKeyString(			const char *KeyString = NULL ) ;								// アーカイブファイルの鍵文字列を設定する
extern	LONGLONG	DXA_DIR_LoadFile(				const wchar_t *FilePath, void *Buffer, int BufferSize ) ;			// ファイルを丸ごと読み込む関数

extern	DWORD_PTR	DXA_DIR_Open(					const wchar_t *FilePath, int UseCacheFlag = FALSE, int BlockReadFlag = TRUE, int UseASyncReadFlag = FALSE ) ;	// ファイルを開く( エラー：-1  成功：ハンドル )
extern	int			DXA_DIR_Close(					DWORD_PTR Handle ) ;											// ファイルを閉じる
extern	LONGLONG	DXA_DIR_Tell(					DWORD_PTR Handle ) ;											// ファイルポインタの位置を取得する
extern	int			DXA_DIR_Seek(					DWORD_PTR Handle, LONGLONG SeekPoint, int SeekType ) ;			// ファイルポインタの位置を変更する
extern	size_t		DXA_DIR_Read(					void *Buffer, size_t BlockSize, size_t BlockNum, DWORD_PTR Handle ) ; // ファイルからデータを読み込む
extern	int			DXA_DIR_Eof(					DWORD_PTR Handle ) ;											// ファイルの終端を調べる
extern	int			DXA_DIR_ChDir(					const wchar_t *Path ) ;
extern	int			DXA_DIR_GetDir(					wchar_t *Buffer ) ;
extern	int			DXA_DIR_GetDirS(				wchar_t *Buffer, size_t BufferBytes ) ;
extern	int			DXA_DIR_IdleCheck(				DWORD_PTR Handle ) ;
extern	DWORD_PTR	DXA_DIR_FindFirst(				const wchar_t *FilePath, FILEINFOW *Buffer ) ;					// 戻り値: -1=エラー  -1以外=FindHandle
extern	int			DXA_DIR_FindNext(				DWORD_PTR FindHandle, FILEINFOW *Buffer ) ;						// 戻り値: -1=エラー  0=成功
extern	int			DXA_DIR_FindClose(				DWORD_PTR FindHandle ) ;										// 戻り値: -1=エラー  0=成功

#endif

extern	int			DXA_Encode(						void *Src, DWORD SrcSize, void *Dest ) ;						// データを圧縮する( 戻り値:圧縮後のデータサイズ )
extern	int			DXA_Decode(						void *Src, void *Dest ) ;										// データを解凍する( 戻り値:解凍後のデータサイズ )

extern	ULONGLONG	Huffman_Encode(					void *Src, ULONGLONG SrcSize, void *Dest ) ;					// データをハフマン圧縮する( 戻り値:圧縮後のサイズ  0 はエラー  Dest に NULL を入れると圧縮データ格納に必要なサイズが返る )
extern	ULONGLONG	Huffman_Decode(					void *Press, void *Dest ) ;										// ハフマン圧縮されたデータを解凍する( 戻り値:解凍後のサイズ  0 はエラー  Dest に NULL を入れると解凍データ格納に必要なサイズが返る )

extern	DWORD		BinToChar128(					void *Src, DWORD SrcSize, void *Dest ) ;						// バイナリデータを半角文字列に変換する( 戻り値:変換後のデータサイズ )
extern	DWORD		Char128ToBin(					void *Src, void *Dest ) ;										// 半角文字列をバイナリデータに変換する( 戻り値:変換後のデータサイズ )

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // __DXARCHIVE_H__
