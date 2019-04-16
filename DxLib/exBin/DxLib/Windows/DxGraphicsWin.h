// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		WindowsOS用描画処理プログラムヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXGRAPHICSWIN_H__
#define __DXGRAPHICSWIN_H__

#include "../DxCompileConfig.h"

#ifndef DX_NON_GRAPHICS

// インクルード ------------------------------------------------------------------
#include "DxGraphicsD3D9.h"
#include "DxGraphicsD3D11.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

#define GRAWIN		GraphicsManage_Win

#define GRAPHICS_API_NONE						(0)
#define GRAPHICS_API_DIRECT3D9_WIN32			(1)
#define GRAPHICS_API_DIRECT3D11_WIN32			(2)

// 構造体定義 --------------------------------------------------------------------

// Windows用 ハードウエアレンダリング用オリジナル画像テクスチャ情報の構造体
struct IMAGEDATA_ORIG_HARD_TEX_PF
{
	union
	{
		int									Dummy ;
#ifndef DX_NON_DIRECT3D11
		IMAGEDATA_ORIG_HARD_TEX_DIRECT3D11	D3D11 ;							// Direct3D11用オリジナル画像テクスチャ情報
#endif // DX_NON_DIRECT3D11
#ifndef DX_NON_DIRECT3D9
		IMAGEDATA_ORIG_HARD_TEX_DIRECT3D9	D3D9 ;							// Direct3D9用オリジナル画像テクスチャ情報
#endif // DX_NON_DIRECT3D9
	} ;
} ;

// Windows用 シャドウマップ環境依存情報の構造体
struct SHADOWMAPDATA_PF
{
	union
	{
		int									Dummy ;
#ifndef DX_NON_DIRECT3D11
		SHADOWMAPDATA_DIRECT3D11			D3D11 ;								// Direct3D11用シャドウマップ情報
#endif // DX_NON_DIRECT3D11
#ifndef DX_NON_DIRECT3D9
		SHADOWMAPDATA_DIRECT3D9				D3D9 ;								// Direct3D9用シャドウマップ情報
#endif // DX_NON_DIRECT3D9
	} ;
} ;

// Windows用 シェーダーハンドル環境依存情報の構造体
struct SHADERHANDLEDATA_PF
{
	union
	{
		int									Dummy ;
#ifndef DX_NON_DIRECT3D11
		SHADERHANDLEDATA_DIRECT3D11		D3D11 ;								// Direct3D11用シェーダーハンドル情報
#endif // DX_NON_DIRECT3D11
#ifndef DX_NON_DIRECT3D9
		SHADERHANDLEDATA_DIRECT3D9		D3D9 ;								// Direct3D9用シェーダーハンドル情報
#endif // DX_NON_DIRECT3D9
	} ;
} ;

// Windows用 シェーダー用定数バッファハンドル環境依存情報の構造体
struct SHADERCONSTANTBUFFERHANDLEDATA_PF
{
	union
	{
		int									Dummy ;
#ifndef DX_NON_DIRECT3D11
		SHADERCONSTANTBUFFERHANDLEDATA_DIRECT3D11		D3D11 ;				// Direct3D11用シェーダー用定数バッファハンドル情報
#endif // DX_NON_DIRECT3D11
#ifndef DX_NON_DIRECT3D9
		SHADERCONSTANTBUFFERHANDLEDATA_DIRECT3D9		D3D9 ;				// Direct3D9用シェーダー用定数バッファハンドル情報
#endif // DX_NON_DIRECT3D9
	} ;
} ;

// Windows用 頂点バッファハンドルで使用する環境依存情報の構造体
struct VERTEXBUFFERHANDLEDATA_PF
{
	union
	{
		int									Dummy ;
#ifndef DX_NON_DIRECT3D11
		VERTEXBUFFERHANDLEDATA_DIRECT3D11	D3D11 ;							// Direct3D11用頂点バッファハンドル情報
#endif // DX_NON_DIRECT3D11
#ifndef DX_NON_DIRECT3D9
		VERTEXBUFFERHANDLEDATA_DIRECT3D9	D3D9 ;							// Direct3D9用頂点バッファハンドル情報
#endif // DX_NON_DIRECT3D9
	} ;
} ;

// Windows用 インデックスバッファハンドルで使用する環境依存情報の構造体
struct INDEXBUFFERHANDLEDATA_PF
{
	union
	{
		int									Dummy ;
#ifndef DX_NON_DIRECT3D11
		INDEXBUFFERHANDLEDATA_DIRECT3D11	D3D11 ;							// Direct3D11用インデックスバッファハンドル情報
#endif // DX_NON_DIRECT3D11
#ifndef DX_NON_DIRECT3D9
		INDEXBUFFERHANDLEDATA_DIRECT3D9		D3D9 ;							// Direct3D9用インデックスバッファハンドル情報
#endif // DX_NON_DIRECT3D9
	} ;
} ;

// Windows に依存している設定情報
struct GRAPHICS_SETTING_WIN
{
	int									DisableAeroFlag ;					// Aero を無効にするかどうかのフラグ( TRUE:無効にする  FALSE:無効にしない )
	int									UseGraphicsAPI ;					// 使用するグラフィックスAPI( GRAPHICS_API_DIRECT3D9_WIN32 など )
	int									NotUseDirect3D11 ;					// 描画処理に Direct3D11 を使用しないかどうか( TRUE:使用しない  FALSE:使用する )
	int									UseMultiThread ;					// マルチスレッド対応フラグを使うかどうか

	HWND								ScreenFlipTargetWindow ;			// ScreenFlip の対象とするウインドウ
	double								ScreenFlipTargetWindowScaleX ;		// ScreenFlip の対象とするウインドウに転送する際の拡大率X
	double								ScreenFlipTargetWindowScaleY ;		// ScreenFlip の対象とするウインドウに転送する際の拡大率Y
} ;

// 描画処理関係で Windows に依存している情報の構造体
struct GRAPHICSMANAGE_WIN
{
	int									UseChangeDisplaySettings ;			// ChangeDisplaySettings を使用して画面モードを変更したかどうか( TRUE:ChangeDisplaySettings を使用した  FALSE:ChangeDisplaySettings は使用していない )

	GRAPHICS_SETTING_WIN				Setting ;							// Windows に依存している設定情報

//	RECT								LastCopySrcRect ;					// 最後にフィット転送したときのコピー元矩形
//	RECT								LastCopyDestRect ;					// 最後にフィット転送したときのコピー先矩形

	int									FullScreenBlackFillCounter ;		// フルスクリーンモードで画面全体を黒で塗りつぶすカウンター
} ;

// 内部大域変数宣言 --------------------------------------------------------------

extern GRAPHICSMANAGE_WIN GraphicsManage_Win ;

// 関数プロトタイプ宣言-----------------------------------------------------------

// DirectX 関連関数
extern	void	Graphics_ReleaseDirectXObject( void ) ;							// DirectX のオブジェクトを解放する

extern	void	Graphics_Win_MessageLoop_Process( void ) ;						// メッセージループ時に行うべき処理を実行する
extern	void	Graphics_Win_WM_ACTIVATE_ActiveProcess( void ) ;				// WM_ACTIVATE メッセージの処理で、ウインドウモード時にアクティブになった際に呼ばれる関数

// 設定関係関数
extern	int		Graphics_Win_SetDialogBoxMode( int Flag ) ;						// ダイアログボックスモードを変更する


#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_NON_GRAPHICS

#endif // __DXGRAPHICSWIN_H__
