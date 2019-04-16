// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		Windows用ファイル関係プログラムヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXFILEWIN_H__
#define __DXFILEWIN_H__

// インクルード ------------------------------------------------------------------
#include "../DxCompileConfig.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

// 構造体定義 --------------------------------------------------------------------

// ファイルアクセス専用スレッド用構造体
struct FILEACCESSTHREAD
{
	DWORD					ThreadID ;							// スレッドＩＤ
	HANDLE					ThreadHandle ;						// スレッドハンドル
	HANDLE					Handle ;							// ファイルアクセス用ハンドル
	HANDLE					FuncEvent ;							// 指令受け取り用ハンドル
	HANDLE					CompEvent ;							// 指令完了用ハンドル
	int						Function ;							// 指令( FILEACCESSTHREAD_FUNCTION_OPEN 等 )

	int						EndFlag ;							// 終了したか、フラグ
	int						ErrorFlag ;							// エラー発生フラグ

	wchar_t					FilePath[FILEPATH_MAX] ;			// ファイルパス
	void					*ReadBuffer ;						// 読み込むデータを格納するバッファー
	ULONGLONG				ReadPosition ;						// 読み込むデータの位置
	DWORD					ReadSize ;							// 読み込むデータのサイズ(読み込めたデータのサイズ)
	ULONGLONG				SeekPoint ;							// ファイルポインタを移動する位置 

	BYTE					*CacheBuffer ;						// キャッシュバッファへのポインタ
	ULONGLONG				CachePosition ;						// キャッシュしているファイル位置
	ULONGLONG				CacheSize ;							// キャッシュバッファの有効データサイズ
} ;

// ウインドウズ用ファイルアクセス処理用構造体
struct FILEACCESS_PF
{
	HANDLE					Handle ;							// ファイルアクセス用ハンドル
	int						UseThread ;							// スレッドを使用するかどうか
	int						UseCacheFlag ;						// キャッシュを使用するかどうか

	FILEACCESSTHREAD		ThreadData ;						// 読み込み専用スレッドのデータ
} ;

// ウインドウズ用ファイル検索処理用構造体
struct FINDINFO_PF
{
	HANDLE					FindHandle ;						// ファイル検索用ハンドル
} ;

// ファイルアクセス処理が使用するグローバルデータの環境依存データ構造体
struct FILEMANAGEDATA_PF
{
	int						Dummy ;
} ;

// テーブル-----------------------------------------------------------------------

// 内部大域変数宣言 --------------------------------------------------------------

// 関数プロトタイプ宣言-----------------------------------------------------------

extern	HANDLE		CreateTemporaryFile( wchar_t *TempFileNameBuffer, size_t BufferBytes ) ;								// テンポラリファイルを作成する

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // __DXFILEWIN_H__
