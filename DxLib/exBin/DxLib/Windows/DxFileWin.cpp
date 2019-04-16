//-----------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		Windows用ファイル関係プログラム
// 
//  	Ver 3.20c
// 
//-----------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

// インクルード ---------------------------------------------------------------
#include "DxWindow.h"
#include "DxWinAPI.h"
#include "../DxFile.h"
#include "../DxLog.h"
#include "../DxChar.h"
#include "../DxBaseFunc.h"

#if !defined(CINTERFACE) && defined(__c2__) &&  __clang_major__ == 3 && __clang_minor__ == 8
//To avoid compile error
//C:\Program Files (x86)\Windows Kits\8.1\Include\um\combaseapi.h(229,21): error : unknown type name 'IUnknown'
//          static_cast<IUnknown*>(*pp);    // make sure everyone derives from IUnknown
#define CINTERFACE
#endif
#include <windows.h>


#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 -----------------------------------------------------------------

// 構造体宣言 -----------------------------------------------------------------

// データ定義 -----------------------------------------------------------------

// パスの始まりが / や \\ だった場合、ルートディレクトリを指定したことにするか( 1:ルートディレクトリを指定したことにする  0:ルートディレクトリを指定したことにしない )
int g_EnableRootDirName = 1 ;

int g_AddDriveNameNum = 0 ;				// 追加のドライブ名の数
const char * g_AddDriveName[ 16 ] ;		// 追加のドライブ名

// 関数宣言 -------------------------------------------------------------------

DWORD	WINAPI		FileAccessThreadFunction( void *FileAccessThreadData ) ;					// ファイルアクセス専用スレッド用関数
int					SetFilePointer64( HANDLE FileHandle, ULONGLONG Position ) ;					// ファイルのアクセス位置を設定する( 0:成功  -1:失敗 )

// プログラム -----------------------------------------------------------------

// ファイルのアクセス位置を設定する
int SetFilePointer64( HANDLE FileHandle, ULONGLONG Position )
{
	DWORD High, Low ;
	DWORD Result ;

	SETUP_WIN_API

	Low = ( DWORD )Position ;
	High = ( DWORD )( Position >> 32 ) ;
	Result = WinAPIData.Win32Func.SetFilePointerFunc( FileHandle, ( LONG )Low, ( LONG * )&High, FILE_BEGIN ) ;
	if( Result == 0xffffffff && WinAPIData.Win32Func.GetLastErrorFunc() != NO_ERROR )
	{
		return -1 ;
	}

	return 0 ;
}

// テンポラリファイルを作成する
extern HANDLE CreateTemporaryFile( wchar_t *TempFileNameBuffer, size_t BufferBytes )
{
	wchar_t String1[FILEPATH_MAX], String2[FILEPATH_MAX] ;
	HANDLE FileHandle ;
	int Length ;

	SETUP_WIN_API

	// テンポラリファイルのディレクトリパスを取得する
	if( WinAPIData.Win32Func.GetTempPathWFunc( sizeof( String1 ) / sizeof( wchar_t ), String1 ) == 0 ) return NULL ;

	// 文字列の最後に￥マークをつける
	Length = ( int )_WCSLEN( String1 ) ;
	if( String1[ Length - 1 ] != L'\\' )
	{
		_WCSCAT_S( String1, sizeof( String1 ), L"\\" ) ;
	}

	// テンポラリファイルのファイル名を作成する
	if( WinAPIData.Win32Func.GetTempFileNameWFunc( String1, L"tmp", 0, String2 ) == 0 ) return NULL ;

	// フルパスに変換
	ConvertFullPathW_( String2, String1, sizeof( String1 ) ) ;

	// テンポラリファイルを開く
	WinAPIData.Win32Func.DeleteFileWFunc( String1 ) ;
	FileHandle = WinAPIData.Win32Func.CreateFileWFunc( String1, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL ) ;
	if( FileHandle == NULL )
	{
		return NULL ;
	}

	// テンポラリファイル名を保存
	if( TempFileNameBuffer != NULL )
	{
		_WCSCPY_S( TempFileNameBuffer, BufferBytes, String1 ) ;
	}

	// ハンドルを返す
	return FileHandle ;
}



// ファイルアクセス専用スレッド用関数
DWORD WINAPI FileAccessThreadFunction( void *FileAccessThreadData )
{
	FILEACCESSTHREAD *dat = (FILEACCESSTHREAD *)FileAccessThreadData ;
	DWORD res, ReadSize ;

	SETUP_WIN_API

	for(;;)
	{
		for(;;)
		{
			// キャッシュを使用すかどうかで処理を分岐
			if( dat->CacheBuffer )
			{
				// 指令が来るまでちょっと待つ
				res = WinAPIData.Win32Func.WaitForSingleObjectFunc( dat->FuncEvent, 100 ) ;

				// 指令が来てい無い場合でファイルが開いている場合はキャッシング処理を行う
				if( res == WAIT_TIMEOUT && dat->Handle != NULL )
				{
					// もしキャッシュが一杯だったら何もしない
					if( dat->CacheSize != FILEACCESSTHREAD_DEFAULT_CACHESIZE )
					{
						// 読み込み開始位置セット
						SetFilePointer64( dat->Handle, dat->CachePosition + dat->CacheSize ) ;

						// 読み込み
						WinAPIData.Win32Func.ReadFileFunc( dat->Handle, &dat->CacheBuffer[dat->CacheSize], ( DWORD )( FILEACCESSTHREAD_DEFAULT_CACHESIZE - dat->CacheSize ), &ReadSize, NULL ) ;
						
						// 有効なサイズを増やす
						dat->CacheSize += ( LONGLONG )ReadSize ;
					}
				}
				else
				{
					break ;
				}
			}
			else
			{
				// 指令が来るまで待つ
				res = WinAPIData.Win32Func.WaitForSingleObjectFunc( dat->FuncEvent, INFINITE ) ;
				if( res == WAIT_TIMEOUT && dat->Handle != NULL ) continue;
				break;
			}
		}

//		WinAPIData.Win32Func.WaitForSingleObjectFunc( dat->FuncEvent, INFINITE ) ;

		// イベントのシグナル状態を解除する
		WinAPIData.Win32Func.ResetEventFunc( dat->FuncEvent ) ;
		WinAPIData.Win32Func.ResetEventFunc( dat->CompEvent ) ;

		// 指令が来たら判断する
		switch( dat->Function )
		{
		case FILEACCESSTHREAD_FUNCTION_OPEN :
			dat->Handle = WinAPIData.Win32Func.CreateFileWFunc( dat->FilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL ) ;
			if( dat->Handle == INVALID_HANDLE_VALUE )
			{
				dat->ErrorFlag = TRUE ;
				goto END ;
			}
			break ;

		case FILEACCESSTHREAD_FUNCTION_CLOSE :
			WinAPIData.Win32Func.CloseHandleFunc( dat->Handle ) ;
			dat->Handle = NULL ;
			break ;

		case FILEACCESSTHREAD_FUNCTION_READ :
			// キャッシュと読み込み位置が一致している場合はキャッシュからデータを転送する
			if( dat->CacheBuffer && dat->ReadPosition == dat->CachePosition && dat->CacheSize != 0 )
			{
				DWORD MoveSize ;

				// 転送するサイズを調整
				MoveSize = dat->ReadSize ;
				if( MoveSize > dat->CacheSize ) MoveSize = ( DWORD )dat->CacheSize ;

				// 転送
				_MEMCPY( dat->ReadBuffer, dat->CacheBuffer, MoveSize ) ;

				// 読み込みサイズと読み込み位置を移動する
				dat->ReadBuffer = (void *)( (BYTE *)dat->ReadBuffer + MoveSize ) ;
				dat->ReadPosition += MoveSize ;
				dat->ReadSize -= MoveSize ;
				
				// キャッシュの情報も更新
				dat->CachePosition += MoveSize ;
				dat->CacheSize     -= MoveSize ;
				if( dat->CacheSize != 0 ) _MEMMOVE( &dat->CacheBuffer[0], &dat->CacheBuffer[MoveSize], ( size_t )dat->CacheSize ) ;
			}

			// 希望のデータが全て読めていない場合は更にファイルから読み込む
			if( dat->ReadSize != 0 )
			{
				SetFilePointer64( dat->Handle, dat->ReadPosition ) ;
				WinAPIData.Win32Func.ReadFileFunc( dat->Handle, dat->ReadBuffer, dat->ReadSize, &dat->ReadSize, NULL ) ;

				// キャッシュを初期化する
				if( dat->CacheBuffer )
				{
					dat->CachePosition = dat->ReadPosition + dat->ReadSize ;
					dat->CacheSize = 0 ;
				}
			}
			break ;

		case FILEACCESSTHREAD_FUNCTION_SEEK :
			SetFilePointer64( dat->Handle, dat->SeekPoint ) ;

			// キャッシュを初期化する
			if( dat->CacheBuffer )
			{
				dat->CachePosition = (DWORD)dat->SeekPoint ;
				dat->CacheSize = 0 ;
			}
			break ;

		case FILEACCESSTHREAD_FUNCTION_EXIT :
			if( dat->Handle != NULL ) WinAPIData.Win32Func.CloseHandleFunc( dat->Handle ) ;
			dat->Handle = NULL ;
			goto END ;
		}

		// 指令が完了したら完了イベントをシグナル状態にする
		WinAPIData.Win32Func.SetEventFunc( dat->CompEvent ) ;
	}

END:
	// エラー時の為に完了イベントをシグナル状態にする
	WinAPIData.Win32Func.SetEventFunc( dat->CompEvent ) ;
	dat->EndFlag = TRUE ;
	WinAPIData.Win32Func.ExitThreadFunc( 1 ) ;

	return 0 ;
}



// ファイルアクセス処理の初期化・終了関数

// ファイルアクセス処理の初期化関数の環境依存の処理を行う関数
extern int InitializeFile_PF( void )
{
	return 0 ;
}

// ファイルアクセス処理の後始末関数の環境依存の処理を行う関数
extern int TerminateFile_PF( void )
{
	return 0 ;
}


// ファイルアクセス関数
extern int ReadOnlyFileAccessOpen_PF( FILEACCESS *FileAccess, const wchar_t *Path, int UseCacheFlag, int BlockReadFlag )
{
	DWORD Code ;
	DWORD High ;

	SETUP_WIN_API

//	UseCacheFlag = UseCacheFlag ;
	BlockReadFlag = BlockReadFlag ;

//	// キャッシュを使用するかどうかをスレッドを使用するかどうかにしてしまう
//	FileAccess->PF.UseThread = UseCacheFlag ;

	// キャッシュを使用するかどうかのフラグをセット
	FileAccess->PF.UseCacheFlag = UseCacheFlag ;
	FileAccess->PF.ThreadData.CacheBuffer = NULL;

	// キャッシュ、若しくは非同期読み書きを行う場合はスレッドを使用する
	FileAccess->PF.UseThread = FileAccess->PF.UseCacheFlag || FileAccess->UseASyncReadFlag ;

	// スレッドを使用する場合としない場合で処理を分岐
	if( FileAccess->PF.UseThread == TRUE )
	{
		// スレッドを使用する場合はファイルアクセス専用スレッドを立てる

		// 最初にファイルを開けるかどうか確かめておく
		FileAccess->PF.Handle = WinAPIData.Win32Func.CreateFileWFunc( Path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL ) ;
		if( FileAccess->PF.Handle == INVALID_HANDLE_VALUE )
		{
			return -1 ;
		}
		FileAccess->Size = WinAPIData.Win32Func.GetFileSizeFunc( FileAccess->PF.Handle, &High ) ;
		FileAccess->Size |= ( ( ULONGLONG )High ) << 32 ;
		WinAPIData.Win32Func.CloseHandleFunc( FileAccess->PF.Handle ) ;
		FileAccess->PF.Handle = NULL ;

		// キャッシュ用メモリの確保
		if( FileAccess->PF.UseCacheFlag )
		{
			FileAccess->PF.ThreadData.CacheBuffer = (BYTE *)DXALLOC( FILEACCESSTHREAD_DEFAULT_CACHESIZE );
			if( FileAccess->PF.ThreadData.CacheBuffer == NULL )
			{
				DXFREE( FileAccess->PF.ThreadData.CacheBuffer ) ;
				DXST_LOGFILE_ADDUTF16LE( "\xd5\x30\xa1\x30\xa4\x30\xeb\x30\xad\x8a\x7f\x30\xbc\x8f\x7f\x30\xad\x30\xe3\x30\xc3\x30\xb7\x30\xe5\x30\x28\x75\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ファイル読み込みキャッシュ用メモリの確保に失敗しました\n" @*/ ) ;
				return -1 ;
			}
		}

		// 専用スレッドデータを初期化する
		FileAccess->PF.ThreadData.Handle = NULL ;
		FileAccess->PF.ThreadData.ThreadHandle = NULL ;
		FileAccess->PF.ThreadData.FuncEvent = WinAPIData.Win32Func.CreateEventAFunc( NULL, TRUE, FALSE, NULL ) ;
		FileAccess->PF.ThreadData.CompEvent = WinAPIData.Win32Func.CreateEventAFunc( NULL, TRUE, TRUE, NULL ) ;

		FileAccess->PF.ThreadData.ThreadHandle = WinAPIData.Win32Func.CreateThreadFunc(
												NULL,
												0,
												(LPTHREAD_START_ROUTINE)FileAccessThreadFunction, 
												&FileAccess->PF.ThreadData,
												0,
												&FileAccess->PF.ThreadData.ThreadID ) ;
		if( FileAccess->PF.ThreadData.ThreadHandle == NULL )
		{
			if( FileAccess->PF.ThreadData.CacheBuffer ) DXFREE( FileAccess->PF.ThreadData.CacheBuffer ) ;
			WinAPIData.Win32Func.CloseHandleFunc( FileAccess->PF.ThreadData.FuncEvent ) ;
			WinAPIData.Win32Func.CloseHandleFunc( FileAccess->PF.ThreadData.CompEvent ) ;
			DXST_LOGFILE_ADDUTF16LE( "\xd5\x30\xa1\x30\xa4\x30\xeb\x30\xa2\x30\xaf\x30\xbb\x30\xb9\x30\x02\x5c\x28\x75\xb9\x30\xec\x30\xc3\x30\xc9\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ファイルアクセス専用スレッドの作成に失敗しました\n" @*/ ) ;
			return -1 ;
		}
		WinAPIData.Win32Func.SetThreadPriorityFunc( FileAccess->PF.ThreadData.ThreadHandle, THREAD_PRIORITY_LOWEST ) ;

		// ファイルオープン指令はここで完了してしまう
		FileAccess->PF.ThreadData.Function = FILEACCESSTHREAD_FUNCTION_OPEN ;
		_WCSCPY_S( FileAccess->PF.ThreadData.FilePath, sizeof( FileAccess->PF.ThreadData.FilePath ), Path ) ;

		WinAPIData.Win32Func.ResetEventFunc( FileAccess->PF.ThreadData.CompEvent ) ;
		WinAPIData.Win32Func.SetEventFunc( FileAccess->PF.ThreadData.FuncEvent ) ;

		// 指令が終了するまで待つ
		WinAPIData.Win32Func.WaitForSingleObjectFunc( FileAccess->PF.ThreadData.CompEvent, INFINITE ) ;
		if( FileAccess->PF.ThreadData.ErrorFlag == TRUE )
		{
			if( FileAccess->PF.ThreadData.CacheBuffer ) DXFREE( FileAccess->PF.ThreadData.CacheBuffer ) ;
			WinAPIData.Win32Func.CloseHandleFunc( FileAccess->PF.ThreadData.FuncEvent ) ;
			WinAPIData.Win32Func.CloseHandleFunc( FileAccess->PF.ThreadData.CompEvent ) ;
			do
			{
				WinAPIData.Win32Func.SleepFunc( 0 ) ;
				WinAPIData.Win32Func.GetExitCodeThreadFunc( FileAccess->PF.ThreadData.ThreadHandle, &Code ) ;
			}while( Code == STILL_ACTIVE ) ;
			WinAPIData.Win32Func.CloseHandleFunc( FileAccess->PF.ThreadData.ThreadHandle ) ;
			DXST_LOGFILE_ADDUTF16LE( "\xd5\x30\xa1\x30\xa4\x30\xeb\x30\x6e\x30\xaa\x30\xfc\x30\xd7\x30\xf3\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ファイルのオープンに失敗しました\n" @*/ ) ;
			return -1 ;
		}
	}
	else
	{
		// スレッドを使用しない場合はこの場でファイルを開く
		FileAccess->PF.Handle = WinAPIData.Win32Func.CreateFileWFunc( Path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL ) ;
		if( FileAccess->PF.Handle == INVALID_HANDLE_VALUE )
		{
			return -1 ;
		}
		FileAccess->Size = WinAPIData.Win32Func.GetFileSizeFunc( FileAccess->PF.Handle, &High ) ;
		FileAccess->Size |= ( ( ULONGLONG )High ) << 32 ;
	}

	return 0 ;
}

extern int ReadOnlyFileAccessClose_PF( FILEACCESS *FileAccess )
{
	BOOL Result;
	DWORD Code ;

	SETUP_WIN_API

	// スレッドを使用する場合としない場合で処理を分岐
	if( FileAccess->PF.UseThread == TRUE )
	{
		// これ以前の指令が出ていた場合の為に指令完了イベントがシグナル状態になるまで待つ
		WinAPIData.Win32Func.WaitForSingleObjectFunc( FileAccess->PF.ThreadData.CompEvent, INFINITE ) ;

		// スレッドに終了指令を出す
		FileAccess->PF.ThreadData.Function = FILEACCESSTHREAD_FUNCTION_EXIT ;
		WinAPIData.Win32Func.ResetEventFunc( FileAccess->PF.ThreadData.CompEvent ) ;
		WinAPIData.Win32Func.SetEventFunc( FileAccess->PF.ThreadData.FuncEvent ) ;

		// 指令が終了するまで待つ
		WinAPIData.Win32Func.WaitForSingleObjectFunc( FileAccess->PF.ThreadData.CompEvent, INFINITE ) ;

		// スレッドが終了するのを待つ
		do
		{
			WinAPIData.Win32Func.SleepFunc(1);
			WinAPIData.Win32Func.GetExitCodeThreadFunc( FileAccess->PF.ThreadData.ThreadHandle, &Code );
		}while( Code == STILL_ACTIVE );

		// キャッシュを使用していた場合はキャッシュ用メモリを開放する
		if( FileAccess->PF.ThreadData.CacheBuffer )
			DXFREE( FileAccess->PF.ThreadData.CacheBuffer ) ;

		// イベントやスレッドを閉じる
		WinAPIData.Win32Func.CloseHandleFunc( FileAccess->PF.ThreadData.ThreadHandle ) ;
		WinAPIData.Win32Func.CloseHandleFunc( FileAccess->PF.ThreadData.CompEvent ) ;
		WinAPIData.Win32Func.CloseHandleFunc( FileAccess->PF.ThreadData.FuncEvent ) ;
		Result = 0 ;
	}
	else
	{
		// 使用していない場合はこの場でハンドルを閉じて終了
		Result = WinAPIData.Win32Func.CloseHandleFunc( FileAccess->PF.Handle ) ;
	}

	return Result != 0 ? 0 : -1/*EOF*/ ;
}

extern int ReadOnlyFileAccessSeek_PF( FILEACCESS *FileAccess, LONGLONG SeekPoint )
{
	SETUP_WIN_API

	// スレッドを使用しているかどうかで処理を分岐
	if( FileAccess->PF.UseThread == TRUE )
	{
		// これ以前の指令が出ていた場合の為に指令完了イベントがシグナル状態になるまで待つ
		WinAPIData.Win32Func.WaitForSingleObjectFunc( FileAccess->PF.ThreadData.CompEvent, INFINITE ) ;

		// スレッドにファイル位置変更指令を出す
		FileAccess->PF.ThreadData.Function = FILEACCESSTHREAD_FUNCTION_SEEK ;
		FileAccess->PF.ThreadData.SeekPoint = ( ULONGLONG )SeekPoint ;
		WinAPIData.Win32Func.ResetEventFunc( FileAccess->PF.ThreadData.CompEvent ) ;
		WinAPIData.Win32Func.SetEventFunc( FileAccess->PF.ThreadData.FuncEvent ) ;
	}
	else
	{
		// ファイルアクセス位置を変更する
		if( SetFilePointer64( FileAccess->PF.Handle, ( ULONGLONG )SeekPoint ) == -1 )
		{
			return -1 ;
		}
	}

	// 終了
	return 0 ;
}

extern	size_t ReadOnlyFileAccessRead_PF( void *Buffer, size_t BlockSize, size_t DataNum, FILEACCESS *FileAccess )
{
	DWORD BytesRead ;

	SETUP_WIN_API

	// スレッドを使用しているかどうかで処理を分岐
	if( FileAccess->PF.UseThread == TRUE )
	{
		// これ以前の指令が出ていた場合の為に指令完了イベントがシグナル状態になるまで待つ
		WinAPIData.Win32Func.WaitForSingleObjectFunc( FileAccess->PF.ThreadData.CompEvent, INFINITE ) ;

		// スレッドにファイル読み込み指令を出す
		FileAccess->PF.ThreadData.Function = FILEACCESSTHREAD_FUNCTION_READ ;
		FileAccess->PF.ThreadData.ReadBuffer = Buffer ;
		FileAccess->PF.ThreadData.ReadPosition = FileAccess->Position ;
		FileAccess->PF.ThreadData.ReadSize = ( DWORD )( BlockSize * DataNum ) ;
		WinAPIData.Win32Func.ResetEventFunc( FileAccess->PF.ThreadData.CompEvent ) ;
		WinAPIData.Win32Func.SetEventFunc( FileAccess->PF.ThreadData.FuncEvent ) ;

		// 非同期かどうかで処理を分岐
		if( FileAccess->UseASyncReadFlag == FALSE )
		{
			// 同期読み込みの場合は指令が完了するまで待つ
			WinAPIData.Win32Func.WaitForSingleObjectFunc( FileAccess->PF.ThreadData.CompEvent, INFINITE ) ;
		}

		BytesRead = ( DWORD )( BlockSize * DataNum ) ;
	}
	else
	{
		WinAPIData.Win32Func.ReadFileFunc( FileAccess->PF.Handle, Buffer, ( DWORD )( BlockSize * DataNum ), &BytesRead, NULL ) ;
	}

	return BytesRead ;
}

extern int ReadOnlyFileAccessIdleCheck_PF( FILEACCESS *FileAccess )
{
	SETUP_WIN_API

	if( FileAccess->PF.UseThread == TRUE )
	{
		return WinAPIData.Win32Func.WaitForSingleObjectFunc( FileAccess->PF.ThreadData.CompEvent, 0 ) == WAIT_TIMEOUT ? FALSE : TRUE ;
	}
	else
	{
		return TRUE ;
	}
}

extern int ReadOnlyFileAccessChDir_PF( const wchar_t *Path )
{
	SETUP_WIN_API

	return WinAPIData.Win32Func.SetCurrentDirectoryWFunc( Path ) ;
}

extern int ReadOnlyFileAccessGetDirS_PF( wchar_t *Buffer, size_t BufferBytes )
{
	SETUP_WIN_API

	return ( int )WinAPIData.Win32Func.GetCurrentDirectoryWFunc( ( DWORD )( BufferBytes / sizeof( wchar_t ) ), Buffer ) ;
}

static void _WIN32_FIND_DATA_To_FILEINFO( WIN32_FIND_DATAW *FindData, FILEINFOW *FileInfo )
{
	// ファイル名のコピー
	_WCSCPY_S( FileInfo->Name, sizeof( FileInfo->Name ), FindData->cFileName );

	// ディレクトリかどうかのフラグをセット
	FileInfo->DirFlag = (FindData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 ? TRUE : FALSE;

	// ファイルのサイズをセット
	FileInfo->Size = ( LONGLONG )( ( ( ( ULONGLONG )FindData->nFileSizeHigh ) << 32 ) + FindData->nFileSizeLow ) ;

	// ファイルタイムを保存
	_FileTimeToLocalDateData( &FindData->ftCreationTime, &FileInfo->CreationTime );
	_FileTimeToLocalDateData( &FindData->ftLastWriteTime, &FileInfo->LastWriteTime );
}

extern int ReadOnlyFileAccessFindFirst_PF( FINDINFO *FindInfo, const wchar_t *FilePath, FILEINFOW *Buffer )
{
	WIN32_FIND_DATAW FindData ;

	SETUP_WIN_API

	FindInfo->PF.FindHandle = WinAPIData.Win32Func.FindFirstFileWFunc( FilePath, &FindData ) ;
	if( FindInfo->PF.FindHandle == INVALID_HANDLE_VALUE )
	{
		return -1;
	}

	if( Buffer )
	{
		_WIN32_FIND_DATA_To_FILEINFO( &FindData, Buffer ) ;
	}

	return 0 ;
}

extern int ReadOnlyFileAccessFindNext_PF( FINDINFO *FindInfo, FILEINFOW *Buffer )
{
	WIN32_FIND_DATAW FindData ;

	SETUP_WIN_API

	if( WinAPIData.Win32Func.FindNextFileWFunc( FindInfo->PF.FindHandle, &FindData ) == 0 )
	{
		return -1 ;
	}

	if( Buffer )
	{
		_WIN32_FIND_DATA_To_FILEINFO( &FindData, Buffer ) ;
	}

	return 0 ;
}

extern int ReadOnlyFileAccessFindClose_PF( FINDINFO *FindInfo )
{
	SETUP_WIN_API

	// ０以外が返ってきたら成功
	return WinAPIData.Win32Func.FindCloseFunc( FindInfo->PF.FindHandle ) != 0 ? 0 : -1 ;
}





// 書き込み専用ファイルアクセス関数
extern	int			WriteOnlyFileAccessDelete_PF( const wchar_t *Path )
{
	SETUP_WIN_API

	WinAPIData.Win32Func.DeleteFileWFunc( Path ) ;

	return 0 ;
}

extern	DWORD_PTR	WriteOnlyFileAccessOpen_PF(  const wchar_t *Path )
{
	SETUP_WIN_API

	WinAPIData.Win32Func.DeleteFileWFunc( Path ) ;
	HANDLE Result = WinAPIData.Win32Func.CreateFileWFunc( Path, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL ) ;

	return Result == INVALID_HANDLE_VALUE ? NULL : ( DWORD_PTR )Result ;
}

extern	int			WriteOnlyFileAccessClose_PF( DWORD_PTR Handle )
{
	SETUP_WIN_API

	return WinAPIData.Win32Func.CloseHandleFunc( ( HANDLE )Handle ) ;
}

extern	int			WriteOnlyFileAccessSeek_PF(  DWORD_PTR Handle, LONGLONG SeekPoint, int SeekType )
{
	DWORD MoveMethod = FILE_BEGIN ;

	SETUP_WIN_API

	switch( SeekType )
	{
	case SEEK_SET :
		MoveMethod = FILE_BEGIN ;
		break ;

	case SEEK_END :
		MoveMethod = FILE_END ;
		break ;

	case SEEK_CUR :
		MoveMethod = FILE_CURRENT ;
		break ;
	}

	WinAPIData.Win32Func.SetFilePointerFunc( ( HANDLE )Handle, ( LONG )SeekPoint, NULL, MoveMethod ) ;

	return 0 ;
}

extern	int			WriteOnlyFileAccessWrite_PF( DWORD_PTR Handle, void *Buffer, size_t WriteSize, size_t *GetWriteSize )
{
	DWORD lWriteSize ;

	SETUP_WIN_API

	WinAPIData.Win32Func.WriteFileFunc( ( HANDLE )Handle, Buffer, ( DWORD )WriteSize, &lWriteSize, NULL ) ;

	if( GetWriteSize )
	{
		*GetWriteSize = lWriteSize ;
	}

	return 0 ;
}








#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

