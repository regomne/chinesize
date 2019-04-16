// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		マスク処理ヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXMASK_H__
#define __DXMASK_H__

#include "DxCompileConfig.h"

#ifndef DX_NON_MASK

// インクルード ------------------------------------------------------------------
#include "DxLib.h"
#include "DxStatic.h"
#include "DxMemImg.h"
#include "DxHandle.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

// 構造体定義 --------------------------------------------------------------------

// マスクデータ用構造体
struct MASKDATA
{
	HANDLEINFO				HandleInfo ;							// ハンドル共通情報

	BYTE					*SrcData ;								// マスクソースデータ
	int						SrcDataPitch ;							// ソースデータピッチ
	int						MaskWidth, MaskHeight ;					// マスクの幅と高さ
	int						ValidFlag ;								// 有効フラグ
	int						TransMode ;								// 透過色処理モード
} ;

// マスクデータ管理用構造体
struct MASKMANAGEDATA
{
	int						InitializeFlag ;						// 初期化フラグ

	int						MaskBufferSizeX, MaskBufferSizeY ;		// マスクデータバッファのサイズ
	BYTE					*MaskBuffer ;							// マスクデータバッファ
	int						MaskBufferPitch ;						// マスクデータバッファのピッチ

	int						MaskScreenGraphHandle ;					// マスク画面として使用するグラフィックハンドル、 無効の場合は 0 ( アルファチャンネルがマスクとして使用される )
	int						MaskScreenGraphHandleUseChannel ;		// マスク画面として使用するグラフィックハンドルで、マスクとして使用するチャンネル( DX_MASKGRAPH_CH_A 等 )

	int						MaskReverseEffectFlag ;					// マスクの数値に対する効果を逆転させる
	int						FullScreenMaskUpdateFlag ;				// 全画面マスク更新を行っているかのフラグ
	int						CreateMaskFlag ;						// マスクサーフェスが作成されているか、のフラグ
	int						MaskUseFlag ;							// マスクを使用するか、フラグ
	int						MaskValidFlag ;							// MaskUseFlag と CreateMaskFlag を & したもの

	int						MaskBeginFunctionCount ;				// マスクを使用した描画の前に呼ぶ関数が呼ばれた回数

	int						ValidMaskDrawMemImg ;					// MaskDrawMemImg が有効かどうか、のフラグ
	MEMIMG					MaskDrawMemImg ;						// マスク使用時の描画先サーフェスの代わりの MEMIMG
} ;

// テーブル-----------------------------------------------------------------------

// 内部大域変数宣言 --------------------------------------------------------------

// マスク関係の情報
extern MASKMANAGEDATA MaskManageData ;


// 関数プロトタイプ宣言-----------------------------------------------------------

extern	int			Mask_Initialize( void ) ;																				// マスク処理の初期化
extern	int			Mask_Terminate( void ) ;																				// マスク処理の後始末

extern	int			Mask_ReleaseSurface( void ) ;																			// マスクスクリーンを一時削除する
extern	int			Mask_ReCreateSurface( void ) ;																			// マスクスクリーンを復旧させる
extern	int			Mask_CreateScreenFunction( int MaskSurfaceFlag, int Width, int Height ) ;								// マスクスクリーンを作成する関数
extern	int			Mask_CreateSurface( BYTE **MaskBuffer, int *BufferPitch, int Width, int Height, int *TransModeP ) ;		// マスクサーフェスの作成

extern	int			Mask_DrawBeginFunction( RECT Rect ) ;																	// マスクを使用した描画の前に呼ぶ関数
extern	int			Mask_DrawAfterFunction( RECT Rect ) ;																	// マスクを使用した描画の後に呼ぶ関数

extern	int			Mask_InitializeHandle( HANDLEINFO *HandleInfo ) ;														// マスクハンドルの初期化
extern	int			Mask_TerminateHandle( HANDLEINFO *HandleInfo ) ;															// マスクハンドルの後始末

extern	int			Mask_MakeMask_UseGParam( int Width, int Height, int ASyncLoadFlag = FALSE ) ;								// マスクデータの追加
extern	int			Mask_LoadMask_UseGParam(             const wchar_t *FileName,                                                                                        int ASyncLoadFlag = FALSE ) ;	// マスクデータを画像ファイルから読み込む
extern	int			Mask_LoadDivMask_UseGParam(          const wchar_t *FileName,                  int AllNum, int XNum, int YNum, int XSize, int YSize, int *HandleArray, int ASyncLoadFlag = FALSE ) ;	// マスクを画像ファイルから分割読みこみ
extern	int			Mask_CreateMaskFromMem_UseGParam(    const void *FileImage, int FileImageSize,                                                                       int ASyncLoadFlag = FALSE ) ;	// マスクデータをメモリ上の画像ファイルイメージから読み込む
extern	int			Mask_CreateDivMaskFromMem_UseGParam( const void *FileImage, int FileImageSize, int AllNum, int XNum, int YNum, int XSize, int YSize, int *HandleArray, int ASyncLoadFlag = FALSE ) ;	// マスクをメモリ上の画像ファイルイメージから分割読みこみ



// 環境依存関数
extern	int			Mask_CreateScreenFunction_Timing0_PF( void ) ;															// マスクスクリーンを作成する関数
extern	int			Mask_CreateScreenFunction_Timing1_PF( int Width, int Height ) ;											// マスクスクリーンを作成する関数
extern	int			Mask_CreateScreenFunction_Timing2_PF( int MaskBufferSizeXOld, int MaskBufferSizeYOld ) ;				// マスクスクリーンを作成する関数
extern	int			Mask_ReleaseSurface_PF( void ) ;																		// マスクスクリーンを一時削除する
extern	int			Mask_SetUseMaskScreenFlag_PF( void ) ;																	// マスク使用モードを変更
extern	int			Mask_DrawBeginFunction_PF( RECT *Rect ) ;																// マスクを使用した描画の前に呼ぶ関数( ついでにサブバッファを使用した描画エリア機能を使用している場合の処理もいれてしまっているよ )
extern	int			Mask_DrawAfterFunction_PF( RECT *Rect ) ;																// マスクを使用した描画の後に呼ぶ関数( ついでにサブバッファを使用した描画エリア機能を使用している場合の処理もいれてしまっているよ )
extern	int			Mask_FillMaskScreen_PF( int Flag ) ;																	// マスクスクリーンを指定の色で塗りつぶす
extern	int			Mask_UpdateMaskImageTexture_PF( RECT *Rect ) ;															// 指定領域のマスクイメージテクスチャを更新する




// wchar_t版関数
extern	int			LoadMask_WCHAR_T(						const wchar_t *FileName ) ;
extern	int			LoadDivMask_WCHAR_T(					const wchar_t *FileName, int AllNum, int XNum, int YNum, int XSize, int YSize, int *HandleArray ) ;
#ifndef DX_NON_FONT
extern	int			DrawFormatStringMask_WCHAR_T(			int x, int y, int Flag,                 const wchar_t *FormatString, ... ) ;
extern	int			DrawFormatStringMaskToHandle_WCHAR_T(	int x, int y, int Flag, int FontHandle, const wchar_t *FormatString, ... ) ;
extern	int			DrawStringMask_WCHAR_T(					int x, int y, int Flag,                 const wchar_t *String, int StrLen ) ;
extern	int			DrawStringMaskToHandle_WCHAR_T(			int x, int y, int Flag, int FontHandle, const wchar_t *String, int StrLen ) ;
#endif // DX_NON_FONT



#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif //	DX_NON_MASK

#endif // __DXMASK_H__
