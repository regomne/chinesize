//-----------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		WindowsOS用描画処理プログラム
// 
//  	Ver 3.20c
// 
//-----------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

#include "DxGraphicsWin.h"

#ifndef DX_NON_GRAPHICS

// インクルード ---------------------------------------------------------------
#include "DxWindow.h"
#include "DxWinAPI.h"
#include "../DxSystem.h"
#include "../DxLog.h"
#include "../DxModel.h"
#include "../DxMath.h"
#include "../DxBaseFunc.h"
#include "../DxGraphics.h"
#include "../DxASyncLoad.h"
#include "../DxMask.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 -----------------------------------------------------------------

#ifndef MONITORINFOF_PRIMARY
	#define MONITORINFOF_PRIMARY        0x00000001
#endif

// 構造体宣言 -----------------------------------------------------------------

// データ定義 -----------------------------------------------------------------

static int UseGDIFlag ;							// GDI を使用するかどうか

GRAPHICSMANAGE_WIN GraphicsManage_Win ; 

// 関数宣言 -------------------------------------------------------------------

static int UseChangeDisplaySettings( void ) ;									// ChangeDisplaySettings を使用して画面モードを変更する

// プログラム -----------------------------------------------------------------


// DirectX のオブジェクトを解放する
extern void Graphics_ReleaseDirectXObject( void )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		Graphics_D3D11_ReleaseDirectXObject_PF() ;
		break ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		Graphics_D3D9_ReleaseDirectXObject_PF() ;
		break ;
#endif // DX_NON_DIRECT3D9

	default :
		break ;
	}
}

// メッセージループ時に行うべき処理を実行する
extern void Graphics_Win_MessageLoop_Process( void )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		break ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		// 管理テクスチャへの転送用のシステムメモリテクスチャの定期処理を行う
		Graphics_D3D9_SysMemTextureProcess() ;

		// 管理テクスチャへの転送用のシステムメモリサーフェスの定期処理を行う
		Graphics_D3D9_SysMemSurfaceProcess() ;
		break ;
#endif // DX_NON_DIRECT3D9

	default :
		break ;
	}
}

// WM_ACTIVATE メッセージの処理で、ウインドウモード時にアクティブになった際に呼ばれる関数
extern void Graphics_Win_WM_ACTIVATE_ActiveProcess( void )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		Graphics_D3D11_WM_ACTIVATE_ActiveProcess_PF() ;
		break ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		Graphics_D3D9_WM_ACTIVATE_ActiveProcess_PF() ;
		break ;
#endif // DX_NON_DIRECT3D9

	default :
		break ;
	}
}












// 画像からグラフィックハンドルを作成する関数

// リソースからＢＭＰファイルを読み込む
extern int NS_LoadGraphToResource( int ResourceID )
{
	BITMAPINFO *BmpInfo ;
	void *GraphData ;
	int NewHandle ;

	// リソースから画像ハンドル作成に必要なデータを取得する
	if( GetBmpImageToResource( ResourceID, &BmpInfo, &GraphData ) == -1 )
		return -1 ;

	// グラフィックの作成
	NewHandle = NS_CreateGraphFromBmp( BmpInfo, GraphData ) ;

	// メモリの解放
	DXFREE( BmpInfo ) ;
	DXFREE( GraphData ) ;

	// ハンドルを返す
	return NewHandle ;
}

// リソースから画像データを分割読み込みする
extern int NS_LoadDivGraphToResource( int ResourceID, int AllNum, int XNum, int YNum, int XSize, int YSize, int *HandleArray )
{
	BITMAPINFO *BmpInfo ;
	void *GraphData ;

	// リソースから画像ハンドル作成に必要なデータを取得する
	if( GetBmpImageToResource( ResourceID, &BmpInfo, &GraphData ) == -1 )
		return -1 ;

	// グラフィックの作成
	NS_CreateDivGraphFromBmp( BmpInfo, GraphData, AllNum, XNum, YNum, XSize, YSize, HandleArray ) ;

	// メモリの解放
	DXFREE( BmpInfo ) ;
	DXFREE( GraphData ) ;

	// 成功
	return 0 ;
}

// リソースから画像データを分割読み込みする
extern int NS_LoadDivGraphFToResource( int ResourceID, int AllNum, int XNum, int YNum, float XSize, float YSize, int *HandleArray )
{
	BITMAPINFO *BmpInfo ;
	void *GraphData ;

	// リソースから画像ハンドル作成に必要なデータを取得する
	if( GetBmpImageToResource( ResourceID, &BmpInfo, &GraphData ) == -1 )
		return -1 ;

	// グラフィックの作成
	NS_CreateDivGraphFFromBmp( BmpInfo, GraphData, AllNum, XNum, YNum, XSize, YSize, HandleArray ) ;

	// メモリの解放
	DXFREE( BmpInfo ) ;
	DXFREE( GraphData ) ;

	// 成功
	return 0 ;
}

// リソースからグラフィックデータを読み込む
extern int NS_LoadGraphToResource( const TCHAR *ResourceName, const TCHAR *ResourceType )
{
#ifdef UNICODE
	return LoadGraphToResource_WCHAR_T(
		ResourceName, ResourceType
	) ;
#else
	int Result = -1 ;
	int IntResourceName ;
	int IntResourceType ;
	TCHAR_TO_WCHAR_T_STRING_BEGIN( ResourceName )
	TCHAR_TO_WCHAR_T_STRING_BEGIN( ResourceType )

	IntResourceName = ( ( ( DWORD_PTR )ResourceName ) >> 16 ) == 0 ? TRUE : FALSE ;
	IntResourceType = ( ( ( DWORD_PTR )ResourceType ) >> 16 ) == 0 ? TRUE : FALSE ;

	if( IntResourceName )
	{
		UseResourceNameBuffer = ( wchar_t * )ResourceName ;
	}
	else
	{
		TCHAR_TO_WCHAR_T_STRING_SETUP( ResourceName, goto ERR )
	}

	if( IntResourceType )
	{
		UseResourceTypeBuffer = ( wchar_t * )ResourceType ;
	}
	else
	{
		TCHAR_TO_WCHAR_T_STRING_SETUP( ResourceType, goto ERR )
	}

	Result = LoadGraphToResource_WCHAR_T(
		UseResourceNameBuffer, UseResourceTypeBuffer
	) ;

ERR :

	if( IntResourceName == FALSE )
	{
		TCHAR_TO_WCHAR_T_STRING_END( ResourceName )
	}

	if( IntResourceType == FALSE )
	{
		TCHAR_TO_WCHAR_T_STRING_END( ResourceType )
	}

	return Result ;
#endif
}

// 画像リソースからグラフィックハンドルを作成する
extern int NS_LoadGraphToResourceWithStrLen( const TCHAR *ResourceName, size_t ResourceNameLength, const TCHAR *ResourceType, size_t ResourceTypeLength )
{
	int Result = -1 ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_BEGIN( ResourceName )
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_BEGIN( ResourceType )
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_SETUP( ResourceName, ResourceNameLength, goto ERR )
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_SETUP( ResourceType, ResourceTypeLength, goto ERR )

	Result = NS_LoadGraphToResource( UseResourceNameBuffer, UseResourceTypeBuffer ) ;

ERR :

	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( ResourceName )
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( ResourceType )
	return Result ;
}

// リソースからグラフィックデータを読み込む
extern int LoadGraphToResource_WCHAR_T( const wchar_t *ResourceName, const wchar_t *ResourceType )
{
	void *Image ;
	int ImageSize ;
	int Result ;

	// リソースの情報を取得
	if( GetResourceInfo_WCHAR_T( ResourceName, ResourceType, &Image, &ImageSize ) < 0 )
	{
		return -1 ;
	}

	// ハンドルの作成
	Result = NS_CreateGraphFromMem( Image, ImageSize ) ;

	return Result ;
}

// リソースからグラフィックデータを分割読み込みする
extern int NS_LoadDivGraphToResource( const TCHAR *ResourceName, const TCHAR *ResourceType, int AllNum, int XNum, int YNum, int XSize, int YSize, int *HandleArray )
{
#ifdef UNICODE
	return LoadDivGraphToResource_WCHAR_T(
		ResourceName, ResourceType, AllNum, XNum, YNum, XSize, YSize, HandleArray
	) ;
#else
	int Result = -1 ;

	int IntResourceName ;
	int IntResourceType ;
	TCHAR_TO_WCHAR_T_STRING_BEGIN( ResourceName )
	TCHAR_TO_WCHAR_T_STRING_BEGIN( ResourceType )

	IntResourceName = ( ( ( DWORD_PTR )ResourceName ) >> 16 ) == 0 ? TRUE : FALSE ;
	IntResourceType = ( ( ( DWORD_PTR )ResourceType ) >> 16 ) == 0 ? TRUE : FALSE ;

	if( IntResourceName )
	{
		UseResourceNameBuffer = ( wchar_t * )ResourceName ;
	}
	else
	{
		TCHAR_TO_WCHAR_T_STRING_SETUP( ResourceName, goto ERR )
	}

	if( IntResourceType )
	{
		UseResourceTypeBuffer = ( wchar_t * )ResourceType ;
	}
	else
	{
		TCHAR_TO_WCHAR_T_STRING_SETUP( ResourceType, goto ERR )
	}

	Result = LoadDivGraphToResource_WCHAR_T(
		UseResourceNameBuffer, UseResourceTypeBuffer, AllNum, XNum, YNum, XSize, YSize, HandleArray
	) ;

ERR :

	if( IntResourceName == FALSE )
	{
		TCHAR_TO_WCHAR_T_STRING_END( ResourceName )
	}

	if( IntResourceType == FALSE )
	{
		TCHAR_TO_WCHAR_T_STRING_END( ResourceType )
	}

	return Result ;
#endif
}

// 画像リソースを分割してグラフィックハンドルを作成する
extern int NS_LoadDivGraphToResourceWithStrLen( const TCHAR *ResourceName, size_t ResourceNameLength, const TCHAR *ResourceType, size_t ResourceTypeLength, int AllNum, int XNum, int YNum, int   XSize, int   YSize, int *HandleArray )
{
	int Result = -1 ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_BEGIN( ResourceName )
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_BEGIN( ResourceType )
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_SETUP( ResourceName, ResourceNameLength, goto ERR )
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_SETUP( ResourceType, ResourceTypeLength, goto ERR )

	Result = NS_LoadDivGraphToResource( UseResourceNameBuffer, UseResourceTypeBuffer, AllNum, XNum, YNum, XSize, YSize, HandleArray ) ;

ERR :

	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( ResourceName )
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( ResourceType )
	return Result ;
}

// リソースからグラフィックデータを分割読み込みする
extern int NS_LoadDivGraphFToResource( const TCHAR *ResourceName, const TCHAR *ResourceType, int AllNum, int XNum, int YNum, float XSize, float YSize, int *HandleArray )
{
#ifdef UNICODE
	return LoadDivGraphFToResource_WCHAR_T(
		ResourceName, ResourceType, AllNum, XNum, YNum, XSize, YSize, HandleArray
	) ;
#else
	int Result = -1 ;

	int IntResourceName ;
	int IntResourceType ;
	TCHAR_TO_WCHAR_T_STRING_BEGIN( ResourceName )
	TCHAR_TO_WCHAR_T_STRING_BEGIN( ResourceType )

	IntResourceName = ( ( ( DWORD_PTR )ResourceName ) >> 16 ) == 0 ? TRUE : FALSE ;
	IntResourceType = ( ( ( DWORD_PTR )ResourceType ) >> 16 ) == 0 ? TRUE : FALSE ;

	if( IntResourceName )
	{
		UseResourceNameBuffer = ( wchar_t * )ResourceName ;
	}
	else
	{
		TCHAR_TO_WCHAR_T_STRING_SETUP( ResourceName, goto ERR )
	}

	if( IntResourceType )
	{
		UseResourceTypeBuffer = ( wchar_t * )ResourceType ;
	}
	else
	{
		TCHAR_TO_WCHAR_T_STRING_SETUP( ResourceType, goto ERR )
	}

	Result = LoadDivGraphFToResource_WCHAR_T(
		UseResourceNameBuffer, UseResourceTypeBuffer, AllNum, XNum, YNum, XSize, YSize, HandleArray
	) ;

ERR :

	if( IntResourceName == FALSE )
	{
		TCHAR_TO_WCHAR_T_STRING_END( ResourceName )
	}

	if( IntResourceType == FALSE )
	{
		TCHAR_TO_WCHAR_T_STRING_END( ResourceType )
	}

	return Result ;
#endif
}

// 画像リソースを分割してグラフィックハンドルを作成する( float型 )
extern int NS_LoadDivGraphFToResourceWithStrLen( const TCHAR *ResourceName, size_t ResourceNameLength, const TCHAR *ResourceType, size_t ResourceTypeLength, int AllNum, int XNum, int YNum, float XSize, float YSize, int *HandleArray )
{
	int Result = -1 ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_BEGIN( ResourceName )
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_BEGIN( ResourceType )
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_SETUP( ResourceName, ResourceNameLength, goto ERR )
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_SETUP( ResourceType, ResourceTypeLength, goto ERR )

	Result = NS_LoadDivGraphFToResource( UseResourceNameBuffer, UseResourceTypeBuffer, AllNum, XNum, YNum, XSize, YSize, HandleArray ) ;

ERR :

	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( ResourceName )
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( ResourceType )
	return Result ;
}

// ID3D11Texture2D からグラフィックハンドルを作成する
extern int NS_CreateGraphFromID3D11Texture2D( const void *pID3D11Texture2D )
{
	SETUP_GRAPHHANDLE_GPARAM GParam ;
	int NewGraphHandle ;
	D_D3D11_TEXTURE2D_DESC Desc ;
	D_ID3D11Texture2D *pTexture2D = ( D_ID3D11Texture2D * )pID3D11Texture2D ;
	COLORDATA *ColorData ;

	CheckActiveState() ;

	// 情報を取得
	pTexture2D->GetDesc( &Desc ) ;
	ColorData = Graphics_D3D11_GetD3DFormatColorData( Desc.Format ) ;

	// ハンドルの作成
	NewGraphHandle = Graphics_Image_AddHandle( FALSE ) ;
	if( NewGraphHandle == -1 )
	{
		return -1 ;
	}

	// グラフィックハンドル初期化情報のセットアップ
	{
		Graphics_Image_InitSetupGraphHandleGParam( &GParam ) ;
		GParam.CreateImageColorBitDepth				= ColorData->ColorBitDepth ;
		GParam.FloatTypeGraphCreateFlag				= ColorData->FloatTypeFlag ;
		GParam.CreateImageChannelNum				= ColorData->ChannelNum ;
		GParam.CreateImageChannelBitDepth			= ColorData->ChannelBitDepth ;
		GParam.AlphaTestImageCreateFlag				= FALSE ;
		GParam.AlphaChannelImageCreateFlag			= ColorData->AlphaWidth != 0 ;
		GParam.CubeMapTextureCreateFlag				= FALSE ;
		GParam.BlendImageCreateFlag					= FALSE ;
		GParam.UseManagedTextureFlag				= FALSE ;
		GParam.UseLinearMapTextureFlag				= FALSE ;
		GParam.PlatformTextureFormat				= 0 ;

		GParam.DrawValidImageCreateFlag				= FALSE ;
		GParam.DrawValidAlphaImageCreateFlag		= FALSE ;
		GParam.DrawValidFloatTypeGraphCreateFlag	= FALSE ;
		GParam.DrawValidGraphCreateZBufferFlag		= FALSE ;
		GParam.CreateDrawValidGraphZBufferBitDepth	= 0 ;
		GParam.CreateDrawValidGraphMipLevels		= 0 ;
		GParam.CreateDrawValidGraphChannelNum		= 0 ;
		GParam.DrawValidMSSamples					= 0 ;
		GParam.DrawValidMSQuality					= 0 ;

		GParam.MipMapCount							= 1 ;
		GParam.UserMaxTextureSize					= 0 ;
		GParam.NotUseDivFlag						= TRUE ;
		GParam.NotUseAlphaImageLoadFlag				= TRUE ;
		GParam.NotUsePaletteGraphFlag				= TRUE ;
		GParam.NotInitGraphDelete					= FALSE ;

		GParam.UserPlatformTexture					= ( void * )pID3D11Texture2D ;
	}

	if( Graphics_Image_SetupHandle_UseGParam(
			&GParam,
			NewGraphHandle,
			Desc.Width, Desc.Height, TRUE,
			ColorData->AlphaWidth != 0,
			FALSE,
			0,
			ColorData->Format,
			1,
			FALSE ) == -1 )
	{
		NS_DeleteGraph( NewGraphHandle ) ;
		return -1 ;
	}

	return NewGraphHandle ;
}

// リソースからグラフィックデータを分割読み込みする
extern int LoadDivGraphToResource_WCHAR_T( const wchar_t *ResourceName, const wchar_t *ResourceType, int AllNum, int XNum, int YNum, int XSize, int YSize, int *HandleArray )
{
	void *Image ;
	int ImageSize ;
	int Result ;

	// リソースの情報を取得
	if( GetResourceInfo_WCHAR_T( ResourceName, ResourceType, &Image, &ImageSize ) < 0 ) return -1 ;

	// ハンドルの作成
	Result = NS_CreateDivGraphFromMem( Image, ImageSize, AllNum, XNum, YNum, XSize, YSize, HandleArray ) ;

	return Result ;
}

// リソースからグラフィックデータを分割読み込みする
extern int LoadDivGraphFToResource_WCHAR_T( const wchar_t *ResourceName, const wchar_t *ResourceType, int AllNum, int XNum, int YNum, float XSize, float YSize, int *HandleArray )
{
	void *Image ;
	int ImageSize ;
	int Result ;

	// リソースの情報を取得
	if( GetResourceInfo_WCHAR_T( ResourceName, ResourceType, &Image, &ImageSize ) < 0 ) return -1 ;

	// ハンドルの作成
	Result = NS_CreateDivGraphFFromMem( Image, ImageSize, AllNum, XNum, YNum, XSize, YSize, HandleArray ) ;

	return Result ;
}











// 画像情報関係関数

// グラフィックハンドルが持つ ID3D11Texture2D を取得する( Direct3D11 を使用している場合のみ有効 )( 戻り値を ID3D11Texture2D * にキャストしてください )
extern	const void*	NS_GetGraphID3D11Texture2D( int GrHandle )
{
	IMAGEDATA *Image ;

	// 初期化判定
	if( DxSysData.DxLib_InitializeFlag == FALSE )
	{
		return NULL ;
	}

	// エラー判定
	if( GRAPHCHK( GrHandle, Image ) )
	{
		return NULL ;
	}

	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_GetGraphID3D11Texture2D_PF( Image ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return NULL ;
#endif // DX_NON_DIRECT3D9

	default :
		return NULL ;
	}
}

// グラフィックハンドルが持つ ID3D11RenderTargetView を取得する( Direct3D11 を使用していて、且つ MakeScreen で作成したグラフィックハンドルでのみ有効 )( 戻り値を ID3D11RenderTargetView * にキャストしてください )
extern	const void*	NS_GetGraphID3D11RenderTargetView( int GrHandle )
{
	IMAGEDATA *Image ;

	// 初期化判定
	if( DxSysData.DxLib_InitializeFlag == FALSE )
	{
		return NULL ;
	}

	// エラー判定
	if( GRAPHCHK( GrHandle, Image ) )
	{
		return NULL ;
	}

	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_GetGraphID3D11RenderTargetView_PF( Image ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return NULL ;
#endif // DX_NON_DIRECT3D9

	default :
		return NULL ;
	}
}

// グラフィックハンドルが持つ ID3D11DepthStencilView を取得する( Direct3D11 を使用していて、且つ MakeScreen で作成したグラフィックハンドルでのみ有効 )( 戻り値を ID3D11DepthStencilView * にキャストしてください )
extern const void* NS_GetGraphID3D11DepthStencilView( int GrHandle )
{
	IMAGEDATA *Image ;

	// 初期化判定
	if( DxSysData.DxLib_InitializeFlag == FALSE )
	{
		return NULL ;
	}

	// エラー判定
	if( GRAPHCHK( GrHandle, Image ) )
	{
		return NULL ;
	}

	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_GetGraphID3D11DepthStencilView_PF( Image ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return NULL ;
#endif // DX_NON_DIRECT3D9

	default :
		return NULL ;
	}
}












// 設定関係関数

// ダイアログボックスモードを変更する
extern	int		Graphics_Win_SetDialogBoxMode( int Flag )
{
#ifndef DX_NON_DIRECT3D9
	return Graphics_D3D9_SetDialogBoxMode_PF( Flag ) ;
#else // DX_NON_DIRECT3D9
	return 0 ;
#endif // DX_NON_DIRECT3D9
}

// 裏画面の内容を指定のウインドウに転送する
extern int NS_BltBackScreenToWindow( HWND Window, int ClientX, int ClientY )
{
	RECT SrcRect, DestRect, WinRect ;

	SETUP_WIN_API

	WinAPIData.Win32Func.GetClientRectFunc( Window, &WinRect ) ;

	SrcRect.left   = 0 ;
	SrcRect.top    = 0 ;
	SrcRect.right  = GSYS.Screen.MainScreenSizeX ;
	SrcRect.bottom = GSYS.Screen.MainScreenSizeY ;

	DestRect.left   = WinRect.left + ClientX ;
	DestRect.top    = WinRect.top  + ClientY ;
	DestRect.right  = WinRect.left + ClientX + GSYS.Screen.MainScreenSizeX ;
	DestRect.bottom = WinRect.top  + ClientY + GSYS.Screen.MainScreenSizeY ;

	if( DestRect.left < WinRect.left )
	{
		SrcRect.left += WinRect.left - DestRect.left ;
		DestRect.left = WinRect.left ;
	}

	if( DestRect.top < WinRect.top )
	{
		SrcRect.top += WinRect.top - DestRect.top ;
		DestRect.top = WinRect.top ;
	}

	if( DestRect.right > WinRect.right )
	{
		SrcRect.right -= DestRect.right - WinRect.right ;
		DestRect.right = WinRect.right ;
	}

	if( DestRect.bottom > WinRect.bottom )
	{
		SrcRect.bottom -= DestRect.bottom - WinRect.bottom ;
		DestRect.bottom = WinRect.bottom ;
	}

	return NS_BltRectBackScreenToWindow( Window, SrcRect, DestRect ) ;
}

// 裏画面の指定の領域をウインドウのクライアント領域の指定の領域に転送する
extern int NS_BltRectBackScreenToWindow( HWND Window, RECT BackScreenRect, RECT WindowClientRect )
{
	// 初期化判定
	if( DxSysData.DxLib_InitializeFlag == FALSE || DxLib_GetEndRequest() ) return -1 ;

	// フルスクリーンモードでは使用できない
	if( NS_GetWindowModeFlag() == FALSE ) return -1 ;

	// 実行
	return Graphics_BltRectBackScreenToWindow_PF( Window, BackScreenRect, WindowClientRect ) ;
}

// ScreenFlip で画像を転送する先のウインドウを設定する( NULL を指定すると設定解除 )
extern int NS_SetScreenFlipTargetWindow( HWND TargetWindow, double ScaleX, double ScaleY )
{
	return Graphics_SetScreenFlipTargetWindow_PF( TargetWindow, ScaleX, ScaleY ) ;
}

// デスクトップ画面から指定領域の画像情報をグラフィックハンドルに転送する
extern int NS_GetDesktopScreenGraph( int x1, int y1, int x2, int y2, int GrHandle, int DestX, int DestY )
{
	HWND DesktopHWND ;
	RECT DesktopRect ;
	int DesktopW, DesktopH ;
	int RectW, RectH ;
	int CaptureW, CaptureH ;
	HDC hdc ;
	HDC DesktopHDC = NULL ;
	BITMAPINFO BitmapInfo ;
	void *ImageBuffer = NULL ;
	HBITMAP HBitmap ;
	HDC HMemDC ;
	BASEIMAGE CaptureImage ;
	int GraphW, GraphH ;

	SETUP_WIN_API

	// グラフィックハンドルのサイズを取得
	if( NS_GetGraphSize( GrHandle, &GraphW, &GraphH ) < 0 )
	{
		return -1 ;
	}

	// 引数のチェック
	if( x1 >= x2 || y1 >= y2 )
	{
		return -1 ;
	}

	// デスクトップウインドウハンドルを取得
	DesktopHWND = WinAPIData.Win32Func.GetDesktopWindowFunc() ;

	// デスクトップウインドウのデバイスコンテキストを取得
	DesktopHDC = WinAPIData.Win32Func.GetDCFunc( DesktopHWND ) ;

	// デスクトップの矩形を取得
	WinAPIData.Win32Func.GetWindowRectFunc( DesktopHWND, &DesktopRect ) ;
	RectW = DesktopRect.right  - DesktopRect.left ;
	RectH = DesktopRect.bottom - DesktopRect.top  ;

	// デスクトップの矩形を実サイズを拡大率にあわせて補正
	DesktopW = WinAPIData.Win32Func.GetDeviceCapsFunc( DesktopHDC, DESKTOPHORZRES ) ;
	DesktopH = WinAPIData.Win32Func.GetDeviceCapsFunc( DesktopHDC, DESKTOPVERTRES ) ;
	if( DesktopW <= 0 || DesktopH <= 0 )
	{
		DesktopW = RectW ;
		DesktopH = RectH ;
	}
	if( RectW != DesktopW || RectH != DesktopH )
	{
		DesktopRect.left   = DesktopRect.left   * DesktopW / RectW ;
		DesktopRect.right  = DesktopRect.right  * DesktopW / RectW ;
		DesktopRect.top    = DesktopRect.top    * DesktopH / RectH ;
		DesktopRect.bottom = DesktopRect.bottom * DesktopH / RectH ;
	}

	// 指定の矩形の補正
	if( x1 < DesktopRect.left )
	{
		DestX += DesktopRect.left - x1 ;
		x1     = DesktopRect.left ;
	}
	if( y1 < DesktopRect.top )
	{
		DestY += DesktopRect.top - y1 ;
		y1     = DesktopRect.top ;
	}
	if( x2 > DesktopRect.right )
	{
		x2 = DesktopRect.right ;
	}
	if( y2 > DesktopRect.bottom )
	{
		y2 = DesktopRect.bottom ;
	}

	// 取り込みサイズの算出
	CaptureW = x2 - x1 ;
	CaptureH = y2 - y1 ;

	// 取り込みサイズチェック
	if( CaptureW <= 0 || CaptureH <= 0 )
	{
		WinAPIData.Win32Func.ReleaseDCFunc( DesktopHWND, DesktopHDC ) ;
		DesktopHDC = NULL ;
		return -1 ;
	}

	// 取り込みサイズ補正
	if( DestX + CaptureW > GraphW )
	{
		CaptureW = GraphW - DestX ;
		x2       = x1 + CaptureW ;
	}
	if( DestY + CaptureH > GraphH )
	{
		CaptureH = GraphH - DestY ;
		y2       = y1 + CaptureH ;
	}

	// 再度取り込みサイズチェック
	if( CaptureW <= 0 || CaptureH <= 0 )
	{
		WinAPIData.Win32Func.ReleaseDCFunc( DesktopHWND, DesktopHDC ) ;
		DesktopHDC = NULL ;
		return -1 ;
	}

	// DIBの情報を設定する
	_MEMSET( &BitmapInfo, 0, sizeof( BitmapInfo ) ) ;
	BitmapInfo.bmiHeader.biSize			= sizeof( BITMAPINFOHEADER ) ;
	BitmapInfo.bmiHeader.biWidth		=  CaptureW ;
	BitmapInfo.bmiHeader.biHeight		= -CaptureH ;
	BitmapInfo.bmiHeader.biPlanes		= 1 ;
	BitmapInfo.bmiHeader.biBitCount		= 32 ;
	BitmapInfo.bmiHeader.biCompression	= BI_RGB ;

	// DIBの作成
	hdc			= WinAPIData.Win32Func.GetDCFunc( GetMainWindowHandle() ) ;
	HBitmap		= WinAPIData.Win32Func.CreateDIBSectionFunc( hdc, &BitmapInfo, DIB_RGB_COLORS, ( void ** )&ImageBuffer, NULL, 0 ) ;
	HMemDC		= WinAPIData.Win32Func.CreateCompatibleDCFunc( hdc ) ;
	WinAPIData.Win32Func.SelectObjectFunc( HMemDC, HBitmap ) ;
	WinAPIData.Win32Func.ReleaseDCFunc( GetMainWindowHandle(), hdc ) ;

	// デスクトップイメージををDIBにコピー
	WinAPIData.Win32Func.BitBltFunc( HMemDC, 0, 0, CaptureW, CaptureH, DesktopHDC, x1, y1, SRCCOPY ) ;
	WinAPIData.Win32Func.ReleaseDCFunc( DesktopHWND, DesktopHDC ) ;
	DesktopHDC = NULL ;

	// デスクトップイメージをグラフィックハンドルに転送
	{
		RECT SrcRect ;

		_MEMSET( &CaptureImage, 0, sizeof( CaptureImage ) ) ;
		NS_CreateXRGB8ColorData( &CaptureImage.ColorData ) ;
		CaptureImage.Width		= CaptureW ;
		CaptureImage.Height		= CaptureH ;
		CaptureImage.Pitch		= CaptureW * 4 ;
		CaptureImage.GraphData	= ( void * )ImageBuffer ;
		SrcRect.left			= 0 ;
		SrcRect.top				= 0 ;
		SrcRect.right			= CaptureW ;
		SrcRect.bottom			= CaptureH ;
		NS_BltBmpOrGraphImageToGraph2( NULL, NULL, NULL, FALSE, &CaptureImage, NULL, &SrcRect, DestX, DestY, GrHandle ) ;
	}

	// DIBの削除
	WinAPIData.Win32Func.DeleteDCFunc( HMemDC ) ;
	WinAPIData.Win32Func.DeleteObjectFunc( HBitmap ) ;

	// 終了
	return 0 ;
}

// 使用する DirectDraw デバイスのインデックスを設定する
extern int NS_SetUseDirectDrawDeviceIndex( int Index )
{
	return NS_SetUseDisplayIndex( Index ) ;
}

// DirectDraw や Direct3D の協調レベルをマルチスレッド対応にするかどうかをセットする
extern	int		NS_SetMultiThreadFlag( int Flag )
{
	// 初期化前のみ使用可能 
	if( DxSysData.DxLib_InitializeFlag == TRUE ) return -1 ;

	GRAWIN.Setting.UseMultiThread = Flag ;

	// 終了
	return 0 ;
}

// Vista以降の Windows Aero を無効にするかどうかをセットする、TRUE:無効にする  FALSE:有効にする( DxLib_Init の前に呼ぶ必要があります )
extern int NS_SetAeroDisableFlag( int Flag )
{
	return Graphics_SetAeroDisableFlag_PF( Flag ) ;
}

// Vista以降の環境で Direct3D9Ex を使用するかどうかを設定する( TRUE:使用する( デフォルト )  FALSE:使用しない )
extern int NS_SetUseDirect3D9Ex( int Flag )
{
#ifndef DX_NON_DIRECT3D9
	return Graphics_D3D9_SetUseDirect3D9Ex_PF( Flag ) ;
#else // DX_NON_DIRECT3D9
	return 0 ;
#endif // DX_NON_DIRECT3D9
}

// Direct3D11 を使用するかどうかを設定する( TRUE:使用する  FALSE:使用しない )
extern int NS_SetUseDirect3D11( int Flag )
{
	// 初期化前のみ使用可能 
	if( DxSysData.DxLib_InitializeFlag == TRUE ) return -1 ;

	// フラグを保存
	GRAWIN.Setting.NotUseDirect3D11 = Flag ? FALSE : TRUE ;

	// 終了
	return 0 ;
}

// Direct3D11 で使用する最低機能レベルを指定する関数です、尚、DX_DIRECT3D_11_FEATURE_LEVEL_11_0 より低い機能レベルでの正常な動作は保証しません( デフォルトは DX_DIRECT3D_11_FEATURE_LEVEL_11_0 )
extern int NS_SetUseDirect3D11MinFeatureLevel( int Level /* DX_DIRECT3D_11_FEATURE_LEVEL_10_0 など */ )
{
#ifndef DX_NON_DIRECT3D11
	return Graphics_D3D11_SetUseDirect3D11MinFeatureLevel_PF( Level ) ;
#else // DX_NON_DIRECT3D11
	return 0 ;
#endif // DX_NON_DIRECT3D11
}

// 使用する Direct3D のバージョンを設定する
extern int NS_SetUseDirect3DVersion( int Version )
{
	// 初期化前のみ有効
	if( DxSysData.DxLib_InitializeFlag )
	{
		return -1 ;
	}

	// バージョンによって処理を変更
	switch( Version )
	{
	case DX_DIRECT3D_NONE :
		NS_SetUseDirectDrawFlag( FALSE ) ;
		NS_SetUseDirect3D11( FALSE ) ;
		break ;

	case DX_DIRECT3D_9 :
		NS_SetUseDirect3D9Ex( FALSE ) ;
		break ;

	case DX_DIRECT3D_9EX :
		NS_SetUseDirect3D9Ex( TRUE ) ;
		break ;

	case DX_DIRECT3D_11 :
		NS_SetUseDirectDrawFlag( TRUE ) ;
		NS_SetUseDirect3D11( TRUE ) ;
		break ;

	default :
		return -1 ;
	}

	// 終了
	return 0 ;
}

// 使用している Direct3D のバージョンを取得する( DX_DIRECT3D_9 など )
extern int NS_GetUseDirect3DVersion( void )
{
#ifndef DX_NON_DIRECT3D9
	if( Direct3D9_IsValid() )
	{
		if( Direct3D9_IsExObject() )
		{
			return DX_DIRECT3D_9EX ;
		}
		else
		{
			return DX_DIRECT3D_9 ;
		}
	}
#endif // DX_NON_DIRECT3D9

#ifndef DX_NON_DIRECT3D11
	if( D3D11Device_IsValid() )
	{
		return DX_DIRECT3D_11 ;
	}
#endif // DX_NON_DIRECT3D11

	return DX_DIRECT3D_NONE ;
}

// 使用している Direct3D11 の FeatureLevel ( DX_DIRECT3D_11_FEATURE_LEVEL_9_1 等 )を取得する( 戻り値　-1：エラー　-1以外：Feature Level )
extern int NS_GetUseDirect3D11FeatureLevel(	void )
{
#ifndef DX_NON_DIRECT3D11
	if( D3D11Device_IsValid() == FALSE )
	{
		return -1 ;
	}

	return GD3D11.Setting.FeatureLevel ;
#else  // DX_NON_DIRECT3D11
	return -1 ;
#endif // DX_NON_DIRECT3D11
}

// ＤｉｒｅｃｔＤｒａｗを使用するかどうかをセットする
extern int NS_SetUseDirectDrawFlag( int Flag )
{
	// 初期化前のみ使用可能 
	if( DxSysData.DxLib_InitializeFlag == TRUE ) return -1 ;

	GSYS.Setting.NotUseHardware = !Flag ;

	// DirectDraw を使用しない場合は Direct3D9 を使用するようにする
	if( Flag == FALSE )
	{
		NS_SetUseDirect3DVersion( DX_DIRECT3D_9 ) ;
	}

	// 終了
	return 0 ;
}

// ＧＤＩ描画を必要とするか、を変更する
extern	int		NS_SetUseGDIFlag( int Flag )
{
	// フラグを保存
	UseGDIFlag = Flag ;

	// ハードウエアを使用しているかどうかで処理を分岐
	if( GSYS.Setting.ValidHardware )
	{
		switch( GRAWIN.Setting.UseGraphicsAPI )
		{
#ifndef DX_NON_DIRECT3D11
		case GRAPHICS_API_DIRECT3D11_WIN32 :
			break  ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
		case GRAPHICS_API_DIRECT3D9_WIN32 :
			// ハードウエアレンダリングモードの場合
			if( Direct3DDevice9_IsValid() == 0 ) return 0 ;

			// デバイスの設定
			if( Flag == FALSE )
			{
				Direct3DDevice9_SetDialogBoxMode( FALSE ) ;
			}
			else
			{
				Direct3DDevice9_SetDialogBoxMode( FALSE ) ;
				Direct3DDevice9_SetDialogBoxMode( TRUE ) ;
			}
			break ;
#endif // DX_NON_DIRECT3D9

		default :
			break ;
	}
	}
	else
	{
		// ソフトウエアレンダリングモードでは特にすること無し
	}

	// 終了
	return 0 ;
}

// ＧＤＩ描画を必要とするかの状態を取得する
extern int NS_GetUseGDIFlag( void )
{
	return UseGDIFlag ;
}

// ＤｉｒｅｃｔＤｒａｗが使用するＧＵＩＤを設定する
extern	int		NS_SetDDrawUseGuid( const GUID FAR *Guid )
{
#ifndef DX_NON_DIRECT3D9
	return Graphics_D3D9_SetDDrawUseGuid_PF( Guid ) ;
#else // DX_NON_DIRECT3D9
	return 0 ;
#endif // DX_NON_DIRECT3D9
}

/*戻り値を IDirectDraw7 * にキャストして下さい*/			// 現在使用しているＤｉｒｅｃｔＤｒａｗオブジェクトのアドレスを取得する
extern	const void *NS_GetUseDDrawObj( void )
{
	// 終了
	return DirectDraw7_GetObject() ;
}

// 有効な DirectDraw デバイスの GUID を取得する
extern	const GUID *				NS_GetDirectDrawDeviceGUID( int Number )
{
#ifndef DX_NON_DIRECT3D9
	return Graphics_D3D9_GetDirectDrawDeviceGUID_PF( Number ) ;
#else // DX_NON_DIRECT3D9
	return 0 ;
#endif // DX_NON_DIRECT3D9
}

// 有効な DirectDraw デバイスの名前を得る
extern	int						NS_GetDirectDrawDeviceDescription( int Number, char *StringBuffer )
{
#ifndef DX_NON_DIRECT3D9
	return Graphics_D3D9_GetDirectDrawDeviceDescription_PF( Number, StringBuffer ) ;
#else // DX_NON_DIRECT3D9
	return 0 ;
#endif // DX_NON_DIRECT3D9
}

// 有効な DirectDraw デバイスの数を取得する
extern	int						NS_GetDirectDrawDeviceNum( void )
{
#ifndef DX_NON_DIRECT3D9
	return Graphics_D3D9_GetDirectDrawDeviceNum_PF() ;
#else // DX_NON_DIRECT3D9
	return 0 ;
#endif // DX_NON_DIRECT3D9
}

// 使用中のＤｉｒｅｃｔ３ＤＤｅｖｉｃｅ９オブジェクトを得る
extern	const void * NS_GetUseDirect3DDevice9( void )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return NULL ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_GetUseDirect3DDevice9_PF(  ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return NULL ;
	}
}

// 使用中のバックバッファのDirect3DSurface9オブジェクトを取得する
extern const void * NS_GetUseDirect3D9BackBufferSurface( void )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return NULL ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_GetUseDirect3D9BackBufferSurface_PF(  ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return NULL ;
	}
}

// 使用中のID3D11Deviceオブジェクトを取得する( 戻り値を ID3D11Device * にキャストして下さい )
extern	const void*	NS_GetUseDirect3D11Device( void )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_GetUseDirect3D11Device_PF(  ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return NULL ;
#endif // DX_NON_DIRECT3D9

	default :
		return NULL ;
	}
}

// 使用中のID3D11DeviceContextオブジェクトを取得する( 戻り値を ID3D11DeviceContext * にキャストして下さい )
extern	const void*	NS_GetUseDirect3D11DeviceContext( void )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_GetUseDirect3D11DeviceContext_PF(  ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return NULL ;
#endif // DX_NON_DIRECT3D9

	default :
		return NULL ;
	}
}

// 使用中のバックバッファのID3D11Texture2Dオブジェクトを取得する( 戻り値を ID3D11Texture2D * にキャストしてください )
extern	const void*	NS_GetUseDirect3D11BackBufferTexture2D( void )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_GetUseDirect3D11BackBufferTexture2D_PF(  ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return NULL ;
#endif // DX_NON_DIRECT3D9

	default :
		return NULL ;
	}
}

// 使用中のバックバッファのID3D11RenderTargetViewオブジェクトを取得する( 戻り値を ID3D11RenderTargetView * にキャストしてください )
extern	const void*	NS_GetUseDirect3D11BackBufferRenderTargetView( void )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_GetUseDirect3D11BackBufferRenderTargetView_PF(  ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return NULL ;
#endif // DX_NON_DIRECT3D9

	default :
		return NULL ;
	}
}

// 使用中の深度ステンシルバッファのID3D11Texture2Dオブジェクトを取得する( 戻り値を ID3D11Texture2D * にキャストしてください )
extern	const void*	NS_GetUseDirect3D11DepthStencilTexture2D( void )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_GetUseDirect3D11DepthStencilTexture2D_PF(  ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return NULL ;
#endif // DX_NON_DIRECT3D9

	default :
		return NULL ;
	}
}

// 指定の ID3D11RenderTargetView を描画対象にする( pID3D11DepthStencilView が NULL の場合はデフォルトの深度ステンシルバッファを使用する )
extern int NS_SetDrawScreen_ID3D11RenderTargetView( const void *pID3D11RenderTargetView, const void *pID3D11DepthStencilView )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_SetDrawScreen_ID3D11RenderTargetView_PF( pID3D11RenderTargetView, pID3D11DepthStencilView ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return -1 ;
#endif // DX_NON_DIRECT3D9

	default :
		return -1 ;
	}
}

// ＤＸライブラリのＤｉｒｅｃｔ３Ｄ設定をしなおす
extern int NS_RefreshDxLibDirect3DSetting( void )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_RefreshDxLibDirect3DSetting_PF(  ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_RefreshDxLibDirect3DSetting_PF(  ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}























// 環境依存初期化関係

// ChangeDisplaySettings を使用して画面モードを変更する
static int UseChangeDisplaySettings( void )
{
//	HRESULT hr ;
//	int     DesktopWidth ;
//	int     DesktopHeight ;
//	int     DesktopColorBitDepth ;
//	int     ScreenSizeX ;
//	int     ScreenSizeY ;
	DEVMODEA DevMode ;

	SETUP_WIN_API

	// 元の画面の大きさを得る
//	NS_GetDefaultState( &DesktopWidth, &DesktopHeight, &DesktopColorBitDepth ) ;

	DXST_LOGFILEFMT_ADDUTF16LE(( "\x3b\x75\x62\x97\xe2\x30\xfc\x30\xc9\x30\x6e\x30\x09\x59\xf4\x66\xe6\x51\x06\x74\x92\x30\x8b\x95\xcb\x59\x57\x30\x7e\x30\x59\x30\x20\x00\x25\x00\x75\x00\x20\x00\x78\x00\x20\x00\x25\x00\x75\x00\x20\x00\x20\x00\x25\x00\x75\x00\x20\x00\x62\x00\x69\x00\x74\x00\x20\x00\x00"/*@ L"画面モードの変更処理を開始します %u x %u  %u bit " @*/, GSYS.Screen.MainScreenSizeX, GSYS.Screen.MainScreenSizeY, GSYS.Screen.MainScreenColorBitDepth )) ;
	DXST_LOGFILE_TABADD ;

	// フルスクリーンモードの情報をセットアップ
	Graphics_Screen_SetupFullScreenModeInfo() ;

	// 対応しているカラービット深度は 16 と 32 のみ
//	if( GSYS.Screen.MainScreenColorBitDepth != 16 &&
//		GSYS.Screen.MainScreenColorBitDepth != 32 )
	if( GSYS.Screen.FullScreenUseDispModeData.ColorBitDepth != 16 &&
		GSYS.Screen.FullScreenUseDispModeData.ColorBitDepth != 32 )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"失敗しました\n" @*/ ) ;
		DXST_LOGFILE_ERRCODE_ADDUTF16LE( DX_ERRORCODE_WIN_NOT_COMPATIBLE_SCREEN_COLOR_MODE, "\x24\xff\x38\xff\xe9\x30\xa4\x30\xd6\x30\xe9\x30\xea\x30\x4c\x30\xfe\x5b\xdc\x5f\x57\x30\x66\x30\x44\x30\x6a\x30\x44\x30\x3b\x75\x62\x97\xab\x30\xe9\x30\xfc\x30\xe2\x30\xfc\x30\xc9\x30\x4c\x30\x07\x63\x9a\x5b\x55\x30\x8c\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ＤＸライブラリが対応していない画面カラーモードが指定されました\n" @*/ ) ;
		DXST_LOGFILE_TABSUB ;
		return -1 ;
	}

	// ディスプレイモードの変更
	DXST_LOGFILE_ADDUTF16LE( "\x3b\x75\x62\x97\xe3\x89\xcf\x50\xa6\x5e\x92\x30\x09\x59\xf4\x66\x57\x30\x7e\x30\x59\x30\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"画面解像度を変更します... " @*/ ) ;

	// Win32 API を使って画面モードを変更する
	{
		// パラメータセット
		_MEMSET( &DevMode, 0, sizeof( DevMode ) ) ;
		DevMode.dmSize             = sizeof( DevMode ) ;
		DevMode.dmBitsPerPel       = ( DWORD )GSYS.Screen.MainScreenColorBitDepth ;
		DevMode.dmFields           = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT ;
		if( GSYS.Screen.MainScreenRefreshRate != 0 )
		{
			DevMode.dmDisplayFrequency  = ( DWORD )GSYS.Screen.MainScreenRefreshRate ;
			DevMode.dmFields           |= DM_DISPLAYFREQUENCY ;
		}
		DevMode.dmPelsWidth        = ( DWORD )GSYS.Screen.FullScreenUseDispModeData.Width ;
		DevMode.dmPelsHeight       = ( DWORD )GSYS.Screen.FullScreenUseDispModeData.Height ;

		// 画面モードチェンジ
		if( WinAPIData.Win32Func.ChangeDisplaySettingsAFunc( &DevMode, CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL )
		{
			// 失敗したらリフレッシュレート指定無しでもう一度挑戦
			GSYS.Screen.MainScreenRefreshRate = 0 ;
			DevMode.dmDisplayFrequency        = 0 ;
			DevMode.dmFields                 &= ~DM_DISPLAYFREQUENCY ;
			if( WinAPIData.Win32Func.ChangeDisplaySettingsAFunc( &DevMode, 0/*CDS_FULLSCREEN*/ ) != DISP_CHANGE_SUCCESSFUL )
			{
#if 0
				// それでも駄目で、且つ画面モードが 320x240 だった場合は
				// 擬似 320x240 モードで試す
				if( GSYS.Screen.MainScreenSizeX == 320 && GSYS.Screen.MainScreenSizeY == 240 )
				{
					Graphics_Screen_SetMainScreenSize( 640, 480 ) ;
					NS_SetEmulation320x240( TRUE ) ;
					DevMode.dmPelsWidth        = GSYS.Screen.MainScreenSizeX ;
					DevMode.dmPelsHeight       = GSYS.Screen.MainScreenSizeY ;
					if( WinAPIData.Win32Func.ChangeDisplaySettingsAFunc( &DevMode, 0/*CDS_FULLSCREEN*/ ) != DISP_CHANGE_SUCCESSFUL )
					{
						goto ERRORLABEL ;
					}
					SetWindowStyle() ;
				}
				else
#endif
				{
//ERRORLABEL:
					DXST_LOGFILE_ERRCODE_ADDUTF16LE( DX_ERRORCODE_WIN_FAILED_CHANGE_DISPLAY_SETTINGS, "\x3b\x75\x62\x97\xe2\x30\xfc\x30\xc9\x30\x6e\x30\x09\x59\xf4\x66\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"画面モードの変更に失敗しました\n" @*/ ) ;
					DXST_LOGFILE_TABSUB ;
					return -1 ;
				}
			}
		}
	}

	GRAWIN.UseChangeDisplaySettings = TRUE ;
	DXST_LOGFILE_ADDUTF16LE( "\x10\x62\x9f\x52\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"成功しました\n" @*/ ) ;

	// 10回画面全体を黒で塗りつぶす指定を残す
	GRAWIN.FullScreenBlackFillCounter = 10 ;

	return 0 ;
}

// 描画処理の環境依存部分の初期化を行う関数( 実行箇所区別０ )
extern int Graphics_Initialize_Timing0_PF( void )
{
	int Result ;

	if( GSYS.Setting.NotUseHardware )
	{
		Result = -1 ;
	}
	else
	{
		Result                        = -1 ;
		GRAWIN.Setting.UseGraphicsAPI = GRAPHICS_API_NONE ;

#ifndef DX_NON_DIRECT3D11
		if( GRAWIN.Setting.NotUseDirect3D11 == FALSE )
		{
			if( Result < 0 )
			{
				GRAWIN.Setting.UseGraphicsAPI = GRAPHICS_API_DIRECT3D11_WIN32 ;
				Result = Graphics_D3D11_Initialize_Timing0_PF() ;
				if( Result < 0 )
				{
					GRAWIN.Setting.UseGraphicsAPI = GRAPHICS_API_NONE ;
				}
			}
		}
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
		if( Result < 0 )
		{
			GRAWIN.Setting.UseGraphicsAPI = GRAPHICS_API_DIRECT3D9_WIN32 ;
			Result = Graphics_D3D9_Initialize_Timing0_PF() ;
			if( Result < 0 )
			{
				GRAWIN.Setting.UseGraphicsAPI = GRAPHICS_API_NONE ;
			}
		}
#endif // DX_NON_DIRECT3D9
	}

	// ハードウェア機能が使用できない( しない )場合はソフトウエアレンダリングモードの処理を行う
	if( Result < 0 )
	{
		// DirectDraw7 を作成する
		if( DirectDraw7_Create() < 0 )
		{
			return -1 ;
		}

		// フルスクリーンモードの場合はここで画面モードを変更する
		if( NS_GetWindowModeFlag() == FALSE )
		{
			if( UseChangeDisplaySettings() < 0 )
			{
				DirectDraw7_Release() ;
				return -1 ;
			}
		}

		// ＶＳＹＮＣ待ちの時間を取得する
		DirectDraw7_WaitVSyncInitialize() ;
	}

	// 正常終了
	return 0 ;
}

// 描画処理の環境依存部分の初期化を行う関数( 実行箇所区別１ )
extern int Graphics_Initialize_Timing1_PF( void )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		if( Graphics_D3D11_Initialize_Timing1_PF() < 0 )
		{
			return -1 ;
		}
		break ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		if( Graphics_D3D9_Initialize_Timing1_PF() < 0 )
		{
			return -1 ;
		}
#endif // DX_NON_DIRECT3D9
		break ;

	default :
		break ;
	}

	// 正常終了
	return 0 ;
}

// ハードウエアアクセラレータを使用する場合の環境依存の初期化処理を行う
extern	int		Graphics_Hardware_Initialize_PF( void )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_Hardware_Initialize_PF() ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_Hardware_Initialize_PF() ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 描画処理の環境依存部分の後始末を行う関数
extern	int		Graphics_Terminate_PF( void )
{
	int Result ;

	SETUP_WIN_API

	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		Result = Graphics_D3D11_Terminate_PF() ;
		break ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		Result = Graphics_D3D9_Terminate_PF() ;
#endif // DX_NON_DIRECT3D9

	default :
		Result = 0 ;
		break ;
	}

	// ディスプレイ情報用に確保したメモリの解放
	if( GSYS.Screen.DisplayInfo )
	{
		WinAPIData.Win32Func.HeapFreeFunc( WinAPIData.Win32Func.GetProcessHeapFunc(), 0, GSYS.Screen.DisplayInfo ) ;
		GSYS.Screen.DisplayInfo = NULL ;
	}

	return Result ;
}

// グラフィックスシステムの復帰、又は変更付きの再セットアップを行う
extern int Graphics_RestoreOrChangeSetupGraphSystem_PF(
	int Change,
	int ScreenSizeX,
	int ScreenSizeY,
	int ColorBitDepth,
	int RefreshRate
)
{
#ifndef DX_NON_ASYNCLOAD
	// 非同期読み込みの途中の場合は、非同期読み込みが終了するまで待つ
	while( NS_GetASyncLoadNum() > 0 )
	{
		ProcessASyncLoadRequestMainThread() ;
		if( WinData.ProcessorNum <= 1 )
		{
			Thread_Sleep( 2 ) ;
		}
		else
		{
			Thread_Sleep( 0 ) ;
		}
	}
#endif // DX_NON_ASYNCLOAD

	// ハードウエアの機能を使用するかどうかで処理を分岐
	if( GSYS.Setting.ValidHardware == TRUE )
	{
		switch( GRAWIN.Setting.UseGraphicsAPI )
		{
#ifndef DX_NON_DIRECT3D11
		case GRAPHICS_API_DIRECT3D11_WIN32 :
			return Graphics_D3D11_RestoreOrChangeSetupGraphSystem_PF( Change, ScreenSizeX, ScreenSizeY, ColorBitDepth, RefreshRate ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
		case GRAPHICS_API_DIRECT3D9_WIN32 :
			return Graphics_D3D9_RestoreOrChangeSetupGraphSystem_PF( Change, ScreenSizeX, ScreenSizeY, ColorBitDepth, RefreshRate ) ;
#endif // DX_NON_DIRECT3D9

		default :
			return 0 ;
		}
	}
	else
	{
		// ソフトウエアレンダリングモード

		if( DxSysData.NotDrawFlag == FALSE )
		{
			// DirectDraw7 の解放
			DirectDraw7_Release() ;
		}

		if( Change == TRUE )
		{
			// 画面モードのセット
			Graphics_Screen_SetMainScreenSize( ScreenSizeX, ScreenSizeY ) ;
			GSYS.Screen.MainScreenColorBitDepth = ColorBitDepth ;
			GSYS.Screen.MainScreenRefreshRate   = RefreshRate ;
	//		SetMemImgDefaultColorType( ColorBitDepth == 32 ? 1 : 0 ) ;
		}

		if( DxSysData.NotDrawFlag == FALSE )
		{
			GSYS.Screen.FullScreenResolutionModeAct = GSYS.Screen.FullScreenResolutionMode ;

			// DirectDraw7 を作成する
			if( DirectDraw7_Create() < 0 )
			{
				return -1 ;
			}

			// フルスクリーンモードの場合はここで画面モードを変更する
			if( NS_GetWindowModeFlag() == FALSE )
			{
				if( UseChangeDisplaySettings() < 0 )
				{
					DirectDraw7_Release() ;
					return -1 ;
				}
			}

			// ＶＳＹＮＣ待ちの時間を取得する
			DirectDraw7_WaitVSyncInitialize() ;
		}
	}

	// 正常終了
	return 0 ;
}

// 描画用デバイスが有効かどうかを取得する
extern int Graphics_Hardware_CheckValid_PF( void )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_Hardware_CheckValid_PF() ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_Hardware_CheckValid_PF() ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}



























// 環境依存描画設定関係

// メインウインドウの背景色を設定する( Red,Green,Blue:それぞれ ０〜２５５ )
extern	int		Graphics_Hardware_SetBackgroundColor_PF( int /*Red*/, int /*Green*/, int /*Blue*/ )
{
	return 0 ;
}

// シェーダー描画での描画先を設定する
extern	int		Graphics_Hardware_SetRenderTargetToShader_PF( int TargetIndex, int DrawScreen, int SurfaceIndex, int MipLevel )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_SetRenderTargetToShader_PF(  TargetIndex,  DrawScreen,  SurfaceIndex, MipLevel ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_SetRenderTargetToShader_PF(  TargetIndex,  DrawScreen,  SurfaceIndex, MipLevel ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// SetDrawBright の引数が一つ版
extern	int		Graphics_Hardware_SetDrawBrightToOneParam_PF( DWORD Bright )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_SetDrawBrightToOneParam_PF( Bright ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_SetDrawBrightToOneParam_PF( Bright ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 描画ブレンドモードをセットする
extern	int		Graphics_Hardware_SetDrawBlendMode_PF( int BlendMode, int BlendParam )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_SetDrawBlendMode_PF(  BlendMode,  BlendParam ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_SetDrawBlendMode_PF(  BlendMode,  BlendParam ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 描画時のアルファテストの設定を行う( TestMode:DX_CMP_GREATER等( -1:デフォルト動作に戻す )  TestParam:描画アルファ値との比較に使用する値 )
extern	int		Graphics_Hardware_SetDrawAlphaTest_PF( int TestMode, int TestParam )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_SetDrawAlphaTest_PF(  TestMode,  TestParam ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_SetDrawAlphaTest_PF(  TestMode,  TestParam ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 描画モードをセットする
extern	int		Graphics_Hardware_SetDrawMode_PF( int DrawMode )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_SetDrawMode_PF(  DrawMode ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_SetDrawMode_PF(  DrawMode ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 描画輝度をセット
extern	int		Graphics_Hardware_SetDrawBright_PF( int RedBright, int GreenBright, int BlueBright )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_SetDrawBright_PF(  RedBright,  GreenBright,  BlueBright ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_SetDrawBright_PF(  RedBright,  GreenBright,  BlueBright ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// SetBlendGraphParam の可変長引数パラメータ付き
extern	int		Graphics_Hardware_SetBlendGraphParamBase_PF( IMAGEDATA *BlendImage, int BlendType, int *Param )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_SetBlendGraphParamBase_PF(  BlendImage,  BlendType, Param ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_SetBlendGraphParamBase_PF(  BlendImage,  BlendType, Param ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 最大異方性の値をセットする
extern	int		Graphics_Hardware_SetMaxAnisotropy_PF( int MaxAnisotropy )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_SetMaxAnisotropy_PF(  MaxAnisotropy ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_SetMaxAnisotropy_PF(  MaxAnisotropy ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// ワールド変換用行列をセットする
extern	int		Graphics_Hardware_SetTransformToWorld_PF( const MATRIX *Matrix )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_SetTransformToWorld_PF( Matrix ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_SetTransformToWorld_PF( Matrix ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// ビュー変換用行列をセットする
extern	int		Graphics_Hardware_SetTransformToView_PF( const MATRIX *Matrix )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_SetTransformToView_PF( Matrix ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_SetTransformToView_PF( Matrix ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 投影変換用行列をセットする
extern	int		Graphics_Hardware_SetTransformToProjection_PF( const MATRIX *Matrix )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_SetTransformToProjection_PF( Matrix ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_SetTransformToProjection_PF( Matrix ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// ビューポート行列をセットする
extern	int		Graphics_Hardware_SetTransformToViewport_PF( const MATRIX *Matrix )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_SetTransformToViewport_PF( Matrix ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_SetTransformToViewport_PF( Matrix ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// テクスチャアドレスモードを設定する
extern	int		Graphics_Hardware_SetTextureAddressMode_PF( int Mode /* DX_TEXADDRESS_WRAP 等 */, int Stage )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_SetTextureAddressMode_PF( Mode /* DX_TEXADDRESS_WRAP 等 */,  Stage ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_SetTextureAddressMode_PF( Mode /* DX_TEXADDRESS_WRAP 等 */,  Stage ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// テクスチャアドレスモードを設定する
extern	int		Graphics_Hardware_SetTextureAddressModeUV_PF( int ModeU, int ModeV, int Stage )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_SetTextureAddressModeUV_PF(  ModeU,  ModeV,  Stage ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_SetTextureAddressModeUV_PF(  ModeU,  ModeV,  Stage ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// テクスチャ座標変換行列をセットする
extern	int		Graphics_Hardware_SetTextureAddressTransformMatrix_PF( int UseFlag, MATRIX *Matrix, int Sampler )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_SetTextureAddressTransformMatrix_PF( UseFlag, Matrix, Sampler ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_SetTextureAddressTransformMatrix_PF( UseFlag, Matrix, Sampler ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// フォグを有効にするかどうかを設定する( TRUE:有効  FALSE:無効 )
extern	int		Graphics_Hardware_SetFogEnable_PF( int Flag )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_SetFogEnable_PF(  Flag ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_SetFogEnable_PF(  Flag ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// フォグモードを設定する
extern	int		Graphics_Hardware_SetFogMode_PF( int Mode /* DX_FOGMODE_NONE 等 */ )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_SetFogMode_PF(  Mode /* DX_FOGMODE_NONE 等 */ ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_SetFogMode_PF(  Mode /* DX_FOGMODE_NONE 等 */ ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// フォグカラーを変更する
extern	int		Graphics_Hardware_SetFogColor_PF( DWORD FogColor )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_SetFogColor_PF( FogColor ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_SetFogColor_PF( FogColor ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// フォグが始まる距離と終了する距離を設定する( 0.0f 〜 1.0f )
extern	int		Graphics_Hardware_SetFogStartEnd_PF( float start, float end )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_SetFogStartEnd_PF(  start,  end ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_SetFogStartEnd_PF(  start,  end ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// フォグの密度を設定する( 0.0f 〜 1.0f )
extern	int		Graphics_Hardware_SetFogDensity_PF( float density )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_SetFogDensity_PF(  density ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_SetFogDensity_PF(  density ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// ワールド変換用行列をセットする
extern	int		Graphics_Hardware_DeviceDirect_SetWorldMatrix_PF( const MATRIX *Matrix )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DeviceState_SetWorldMatrix( Matrix ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DeviceState_SetWorldMatrix( Matrix ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// ビュー変換用行列をセットする
extern	int		Graphics_Hardware_DeviceDirect_SetViewMatrix_PF( const MATRIX *Matrix )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DeviceState_SetViewMatrix( Matrix ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DeviceState_SetViewMatrix( Matrix ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 投影変換用行列をセットする
extern	int		Graphics_Hardware_DeviceDirect_SetProjectionMatrix_PF( const MATRIX *Matrix )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DeviceState_SetProjectionMatrix( Matrix ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DeviceState_SetProjectionMatrix( Matrix ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 基本データに設定されているフォグ情報をハードウェアに反映する
extern	int		Graphics_Hardware_ApplyLigFogToHardware_PF( void )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_ApplyLigFogToHardware_PF( ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_ApplyLigFogToHardware_PF( ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 以前の DrawModiGraph 関数のコードを使用するかどうかのフラグをセットする
extern	int		Graphics_Hardware_SetUseOldDrawModiGraphCodeFlag_PF( int Flag )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_SetUseOldDrawModiGraphCodeFlag_PF( Flag ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_SetUseOldDrawModiGraphCodeFlag_PF( Flag ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 描画先に正しいα値を書き込むかどうかのフラグを更新する
extern	int		Graphics_Hardware_RefreshAlphaChDrawMode_PF( void )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_RefreshAlphaChDrawMode_PF() ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_RefreshAlphaChDrawMode_PF() ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}























// 環境依存設定関係

// ハードウエアの頂点演算処理機能を使用するかどうかを設定する
extern	int		Graphics_Hardware_SetUseHardwareVertexProcessing_PF( int Flag )
{
#ifndef DX_NON_DIRECT3D9
	return Graphics_Hardware_D3D9_SetUseHardwareVertexProcessing_PF( Flag ) ;
#else // DX_NON_DIRECT3D9
	return 0 ;
#endif // DX_NON_DIRECT3D9
}

// ピクセル単位でライティングを行うかどうかを設定する、要 ShaderModel 3.0( TRUE:ピクセル単位のライティングを行う  FALSE:頂点単位のライティングを行う( デフォルト ) )
extern	int		Graphics_Hardware_SetUsePixelLighting_PF( int Flag )
{
#ifndef DX_NON_DIRECT3D11
	Graphics_Hardware_D3D11_SetUsePixelLighting_PF( Flag ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	Graphics_Hardware_D3D9_SetUsePixelLighting_PF( Flag ) ;
#endif // DX_NON_DIRECT3D9

	return 0 ;
}

// グラフィックスデバイスがロストから復帰した際に呼ばれるコールバック関数を設定する
extern	int		Graphics_Hardware_SetGraphicsDeviceRestoreCallbackFunction_PF( void (* Callback )( void *Data ), void *CallbackData )
{
#ifndef DX_NON_DIRECT3D11
	Graphics_Hardware_D3D11_SetGraphicsDeviceRestoreCallbackFunction_PF( Callback, CallbackData ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	Graphics_Hardware_D3D9_SetGraphicsDeviceRestoreCallbackFunction_PF( Callback, CallbackData ) ;
#endif // DX_NON_DIRECT3D9

	return 0 ;
}

// グラフィックスデバイスがロストから復帰する前に呼ばれるコールバック関数を設定する
extern	int		Graphics_Hardware_SetGraphicsDeviceLostCallbackFunction_PF( void (* Callback )( void *Data ), void *CallbackData )
{
#ifndef DX_NON_DIRECT3D11
	Graphics_Hardware_D3D11_SetGraphicsDeviceLostCallbackFunction_PF( Callback, CallbackData ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	Graphics_Hardware_D3D9_SetGraphicsDeviceLostCallbackFunction_PF( Callback, CallbackData ) ;
#endif // DX_NON_DIRECT3D9

	return 0 ;
}

// 通常描画にプログラマブルシェーダーを使用するかどうかを設定する( TRUE:使用する( デフォルト )  FALSE:使用しない )
extern	int		Graphics_Hardware_SetUseNormalDrawShader_PF( int Flag )
{
#ifndef DX_NON_DIRECT3D9
	return Graphics_Hardware_D3D9_SetUseNormalDrawShader_PF( Flag ) ;
#else // DX_NON_DIRECT3D9
	return 0 ;
#endif // DX_NON_DIRECT3D9
}

// ビデオメモリの容量を得る
extern	int		Graphics_Hardware_GetVideoMemorySize_PF( int *AllSize, int *FreeSize )
{
#ifndef DX_NON_DIRECT3D9
	return Graphics_Hardware_D3D9_GetVideoMemorySize_PF( AllSize, FreeSize ) ;
#else // DX_NON_DIRECT3D9
	return 0 ;
#endif // DX_NON_DIRECT3D9
}

// Vista以降の Windows Aero を無効にするかどうかをセットする、TRUE:無効にする  FALSE:有効にする( DxLib_Init の前に呼ぶ必要があります )
extern	int		Graphics_SetAeroDisableFlag_PF( int Flag )
{
	// 初期化前のみ有効
	if( DxSysData.DxLib_InitializeFlag == TRUE ) return -1 ;

	if( Flag )
	{
		GRAWIN.Setting.DisableAeroFlag = 2 ;
	}
	else
	{
		GRAWIN.Setting.DisableAeroFlag = 1 ;
	}

	// 終了
	return 0 ;
}























// 環境依存画面関係

// 設定に基づいて使用するＺバッファをセットする
extern	int		Graphics_Hardware_SetupUseZBuffer_PF( void )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_SetupUseZBuffer_PF() ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_SetupUseZBuffer_PF() ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 画面のＺバッファの状態を初期化する
extern	int		Graphics_Hardware_ClearDrawScreenZBuffer_PF( const RECT *ClearRect )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_ClearDrawScreenZBuffer_PF( ClearRect ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_ClearDrawScreenZBuffer_PF( ClearRect ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 画面の状態を初期化する
extern	int		Graphics_Hardware_ClearDrawScreen_PF( const RECT *ClearRect )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_ClearDrawScreen_PF( ClearRect ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_ClearDrawScreen_PF( ClearRect ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 描画先画面のセット
extern	int		Graphics_Hardware_SetDrawScreen_PF( int DrawScreen, int OldScreenSurface, int OldScreenMipLevel, IMAGEDATA *NewTargetImage, IMAGEDATA *OldTargetImage, SHADOWMAPDATA *NewTargetShadowMap, SHADOWMAPDATA *OldTargetShadowMap )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_SetDrawScreen_PF(  DrawScreen,  OldScreenSurface, OldScreenMipLevel, NewTargetImage, OldTargetImage, NewTargetShadowMap, OldTargetShadowMap ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_SetDrawScreen_PF(  DrawScreen,  OldScreenSurface, OldScreenMipLevel, NewTargetImage, OldTargetImage, NewTargetShadowMap, OldTargetShadowMap ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 定期的に ScreenCopy を行うコールバック関数
#define GRAPHICS_SOFTWARE_SCREENFLIPTIMER_ID		(32767)
#if _MSC_VER > 1200 || defined( DX_GCC_COMPILE_4_9_2 )
static VOID CALLBACK Graphics_Software_ScreenFlipTimerProc( HWND /*hwnd*/, UINT /*uMsg*/, UINT_PTR /*idEvent*/, DWORD /*dwTime*/ )
#else
static VOID CALLBACK Graphics_Software_ScreenFlipTimerProc( HWND /*hwnd*/, UINT /*uMsg*/, UINT     /*idEvent*/, DWORD /*dwTime*/ )
#endif
{
	SETUP_WIN_API

	WinAPIData.Win32Func.KillTimerFunc( NS_GetMainWindowHandle(), GRAPHICS_SOFTWARE_SCREENFLIPTIMER_ID ) ;

	if( GSYS.DrawSetting.TargetScreen[ 0 ]        == DX_SCREEN_FRONT &&
		GSYS.DrawSetting.TargetScreenSurface[ 0 ] == 0               &&
		WinData.BackBufferTransColorFlag          == FALSE           &&
		WinData.UseUpdateLayerdWindowFlag         == FALSE )
	{
		if( NS_GetActiveFlag() == TRUE )
		{
			NS_ScreenCopy() ;
		}
		WinAPIData.Win32Func.SetTimerFunc( NS_GetMainWindowHandle(), GRAPHICS_SOFTWARE_SCREENFLIPTIMER_ID, 32, Graphics_Software_ScreenFlipTimerProc ) ;
	}
}

// SetDrawScreen の最後で呼ばれる関数
extern	int		Graphics_Hardware_SetDrawScreen_Post_PF( int DrawScreen )
{
	SETUP_WIN_API

	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_SetDrawScreen_Post_PF(  DrawScreen ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_SetDrawScreen_Post_PF(  DrawScreen ) ;
#endif // DX_NON_DIRECT3D9

	case GRAPHICS_API_NONE :
		if( DrawScreen == DX_SCREEN_FRONT )
		{
			WinAPIData.Win32Func.SetTimerFunc( NS_GetMainWindowHandle(), GRAPHICS_SOFTWARE_SCREENFLIPTIMER_ID, 32, Graphics_Software_ScreenFlipTimerProc ) ;
		}
		return 0 ;

	default :
		return 0 ;
	}
}

// 描画可能領域のセット
extern	int		Graphics_Hardware_SetDrawArea_PF( int x1, int y1, int x2, int y2 )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_SetDrawArea_PF(  x1,  y1,  x2,  y2 ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_SetDrawArea_PF(  x1,  y1,  x2,  y2 ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 描画先バッファをロックする
extern	int		Graphics_Hardware_LockDrawScreenBuffer_PF( RECT *LockRect, BASEIMAGE *BaseImage, int TargetScreen, IMAGEDATA *TargetImage, int TargetScreenSurface, int TargetScreenMipLevel, int ReadOnly, int TargetScreenTextureNo )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_LockDrawScreenBuffer_PF( LockRect, BaseImage,  TargetScreen, TargetImage, TargetScreenSurface, TargetScreenMipLevel,  ReadOnly,  TargetScreenTextureNo ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_LockDrawScreenBuffer_PF( LockRect, BaseImage,  TargetScreen, TargetImage, TargetScreenSurface, TargetScreenMipLevel,  ReadOnly,  TargetScreenTextureNo ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 描画先バッファをアンロックする
extern	int		Graphics_Hardware_UnlockDrawScreenBuffer_PF( void )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_UnlockDrawScreenBuffer_PF() ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_UnlockDrawScreenBuffer_PF() ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 裏画面の内容を表画面に描画する
extern	int		Graphics_Hardware_ScreenCopy_PF( int /*DrawTargetFrontScreenMode_Copy*/ )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_ScreenCopy_PF() ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_ScreenCopy_PF() ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// ディスプレイ列挙用のコールバック関数
BOOL CALLBACK Graphics_MonitorEnumProc(
	HMONITOR hMonitor,        // ディスプレイモニタのハンドル
	HDC      /*hdcMonitor*/,  // モニタに適したデバイスコンテキストのハンドル
	LPRECT   /*lprcMonitor*/, // モニタ上の交差部分を表す長方形領域へのポインタ
	LPARAM   /*dwData*/       // EnumDisplayMonitors から渡されたデータ
)
{
	MONITORINFOEXW          MonitorInfoEx ;
	GRAPHICSSYS_DISPLAYINFO *DisplayInfo ;
	HDC                     hdc ;

	SETUP_WIN_API

	if( GSYS.Screen.DisplayInfo != NULL )
	{
		DisplayInfo = &GSYS.Screen.DisplayInfo[ GSYS.Screen.DisplayNum ] ;

		_MEMSET( &MonitorInfoEx, 0, sizeof( MonitorInfoEx ) ) ;
		MonitorInfoEx.cbSize = sizeof( MonitorInfoEx ) ;
		WinAPIData.Win32Func.GetMonitorInfoWFunc( hMonitor, &MonitorInfoEx ) ;

		DisplayInfo->IsPrimary = ( MonitorInfoEx.dwFlags & MONITORINFOF_PRIMARY ) != 0 ? TRUE : FALSE ;

		DisplayInfo->DesktopRect = MonitorInfoEx.rcMonitor ;
		_WCSCPY_S( DisplayInfo->Name, sizeof( DisplayInfo->Name ), MonitorInfoEx.szDevice ) ;

		hdc = WinAPIData.Win32Func.CreateDCWFunc( DisplayInfo->Name, NULL, NULL, NULL ) ;
		DisplayInfo->DesktopSizeX          = WinAPIData.Win32Func.GetDeviceCapsFunc( hdc, HORZRES ) ;
		DisplayInfo->DesktopSizeY          = WinAPIData.Win32Func.GetDeviceCapsFunc( hdc, VERTRES ) ;
		DisplayInfo->DesktopPixelSizeX     = WinAPIData.Win32Func.GetDeviceCapsFunc( hdc, DESKTOPHORZRES ) ;
		DisplayInfo->DesktopPixelSizeY     = WinAPIData.Win32Func.GetDeviceCapsFunc( hdc, DESKTOPVERTRES ) ;
		if( DisplayInfo->DesktopPixelSizeX <= 0 ||
			DisplayInfo->DesktopPixelSizeY <= 0 )
		{
			DisplayInfo->DesktopPixelSizeX = DisplayInfo->DesktopSizeX ;
			DisplayInfo->DesktopPixelSizeY = DisplayInfo->DesktopSizeY ;
		}
		DisplayInfo->DesktopColorBitDepth  = WinAPIData.Win32Func.GetDeviceCapsFunc( hdc , PLANES ) * WinAPIData.Win32Func.GetDeviceCapsFunc( hdc , BITSPIXEL ) ;
		DisplayInfo->DesktopRefreshRate    = WinAPIData.Win32Func.GetDeviceCapsFunc( hdc , VREFRESH ) ;
		WinAPIData.Win32Func.DeleteDCFunc( hdc ) ;

		DXST_LOGFILEFMT_ADDUTF16LE(( "\x4e\x00\x6f\x00\x2e\x00\x25\x00\x64\x00\x20\x00\x20\x00\xe2\x30\xcb\x30\xbf\x30\xfc\x30\x0d\x54\x1a\xff\x25\x00\x73\x00\x00\x30\x25\x00\x64\x00\x78\x00\x25\x00\x64\x00\x00\x30\x25\x00\x64\x00\x62\x00\x69\x00\x74\x00\x00\x30\x25\x00\x64\x00\x48\x00\x7a\x00\x00"/*@ L"No.%d  モニター名：%s　%dx%d　%dbit　%dHz" @*/, GSYS.Screen.DisplayNum, DisplayInfo->Name, DisplayInfo->DesktopSizeX, DisplayInfo->DesktopSizeY, DisplayInfo->DesktopColorBitDepth, DisplayInfo->DesktopRefreshRate )) ;
	}

	GSYS.Screen.DisplayNum ++ ;

	return TRUE ;
}

// ディスプレイの情報をセットアップする
extern int Graphics_SetupDisplayInfo_PF( void )
{
	DWORD                    i ;
	DWORD                    j ;
	DWORD                    k ;
	DISPLAY_DEVICEW          DisplayDevice ;
	GRAPHICSSYS_DISPLAYINFO *DisplayInfo ;
	wchar_t *                DeviceName ;
	DWORD                    DisplayDeviceNum ;

	OSVERSIONINFOA OsVersionInfo ;

	SETUP_WIN_API

	DXST_LOGFILEFMT_ADDUTF16LE(( "\xc7\x30\xa3\x30\xb9\x30\xd7\x30\xec\x30\xa4\x30\xc5\x60\x31\x58\x6e\x30\xbb\x30\xc3\x30\xc8\x30\xa2\x30\xc3\x30\xd7\x30\x8b\x95\xcb\x59\x00"/*@ L"ディスプレイ情報のセットアップ開始" @*/ )) ;
	DXST_LOGFILE_TABADD ;

	_MEMSET( &OsVersionInfo, 0, sizeof( OsVersionInfo ) ) ;
	OsVersionInfo.dwOSVersionInfoSize = sizeof( OsVersionInfo ) ;

	WinAPIData.Win32Func.GetVersionExAFunc( &OsVersionInfo ) ;

	if( OsVersionInfo.dwPlatformId >= VER_PLATFORM_WIN32_NT &&
		OsVersionInfo.dwMajorVersion >= 5 )
	{
		_MEMSET( &DisplayDevice, 0, sizeof( DisplayDevice ) ) ;
		DisplayDevice.cb = sizeof( DisplayDevice ) ;

		GSYS.Screen.DisplayNum = 0 ;
		WinAPIData.Win32Func.EnumDisplayMonitorsFunc( NULL, NULL, Graphics_MonitorEnumProc, 0 ) ;

		DisplayDeviceNum = 0 ;
		while( WinAPIData.Win32Func.EnumDisplayDevicesWFunc( NULL, DisplayDeviceNum, &DisplayDevice, 0 ) != 0 )
		{
			DisplayDeviceNum ++ ;
		}
	}
	else
	{
		GSYS.Screen.DisplayNum = 1 ;
		DisplayDeviceNum       = 1 ;
	}

	DXST_LOGFILEFMT_ADDUTF16LE(( "\xe2\x30\xcb\x30\xbf\x30\xfc\x30\x6e\x30\x70\x65\x1a\xff\x25\x00\x64\x00\x20\x00\x20\x00\xc7\x30\xa3\x30\xb9\x30\xd7\x30\xec\x30\xa4\x30\xc7\x30\xd0\x30\xa4\x30\xb9\x30\x6e\x30\x70\x65\x1a\xff\x25\x00\x64\x00\x00"/*@ L"モニターの数：%d  ディスプレイデバイスの数：%d" @*/, GSYS.Screen.DisplayNum, DisplayDeviceNum )) ;

	GSYS.Screen.DisplayInfo = ( GRAPHICSSYS_DISPLAYINFO * )WinAPIData.Win32Func.HeapAllocFunc( WinAPIData.Win32Func.GetProcessHeapFunc(), 0, sizeof( GRAPHICSSYS_DISPLAYINFO ) * GSYS.Screen.DisplayNum ) ;
	if( GSYS.Screen.DisplayInfo == NULL )
	{
		DXST_LOGFILEFMT_ADDUTF16LE(( "\x5b\x00\xa8\x30\xe9\x30\xfc\x30\x5d\x00\xc7\x30\xa3\x30\xb9\x30\xd7\x30\xec\x30\xa4\x30\xc5\x60\x31\x58\xdd\x4f\x58\x5b\x28\x75\x6e\x30\xe1\x30\xe2\x30\xea\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"[エラー]ディスプレイ情報保存用のメモリ確保に失敗しました" @*/ )) ;
		DXST_LOGFILE_TABSUB ;
		return -1 ;
	}
	_MEMSET( GSYS.Screen.DisplayInfo, 0, sizeof( GRAPHICSSYS_DISPLAYINFO ) * GSYS.Screen.DisplayNum ) ;

	if( OsVersionInfo.dwPlatformId >= VER_PLATFORM_WIN32_NT &&
		OsVersionInfo.dwMajorVersion >= 5 )
	{
		GSYS.Screen.DisplayNum = 0 ;
		WinAPIData.Win32Func.EnumDisplayMonitorsFunc( NULL, NULL, Graphics_MonitorEnumProc, 0 ) ;
	}
	else
	{
		HDC hdc ;

		DisplayInfo = GSYS.Screen.DisplayInfo ;

		hdc = WinAPIData.Win32Func.GetDCFunc( NULL ) ;

		DisplayInfo->DesktopSizeX         = WinAPIData.Win32Func.GetDeviceCapsFunc( hdc, HORZRES ) ;
		DisplayInfo->DesktopSizeY         = WinAPIData.Win32Func.GetDeviceCapsFunc( hdc, VERTRES ) ;
		DisplayInfo->DesktopPixelSizeX    = DisplayInfo->DesktopSizeX ;
		DisplayInfo->DesktopPixelSizeY    = DisplayInfo->DesktopSizeY ;
		DisplayInfo->DesktopColorBitDepth = WinAPIData.Win32Func.GetDeviceCapsFunc( hdc , PLANES ) * WinAPIData.Win32Func.GetDeviceCapsFunc( hdc , BITSPIXEL ) ;
		DisplayInfo->DesktopRefreshRate   = WinAPIData.Win32Func.GetDeviceCapsFunc( hdc , VREFRESH ) ;

		WinAPIData.Win32Func.ReleaseDCFunc( NULL, hdc ) ;

		DisplayInfo->DesktopRect.left   = 0 ;
		DisplayInfo->DesktopRect.top    = 0 ;
		DisplayInfo->DesktopRect.right  = DisplayInfo->DesktopSizeX ;
		DisplayInfo->DesktopRect.bottom = DisplayInfo->DesktopSizeY ;
		_WCSCPY( DisplayInfo->Name, L"DISPLAY" ) ;

		DXST_LOGFILEFMT_ADDUTF16LE(( "\x4e\x00\x6f\x00\x2e\x00\x25\x00\x64\x00\x20\x00\x20\x00\xe2\x30\xcb\x30\xbf\x30\xfc\x30\x0d\x54\x1a\xff\x25\x00\x73\x00\x00\x30\x25\x00\x64\x00\x78\x00\x25\x00\x64\x00\x00\x30\x25\x00\x64\x00\x62\x00\x69\x00\x74\x00\x00\x30\x25\x00\x64\x00\x48\x00\x7a\x00\x00"/*@ L"No.%d  モニター名：%s　%dx%d　%dbit　%dHz" @*/, 0, DisplayInfo->Name, DisplayInfo->DesktopSizeX, DisplayInfo->DesktopSizeY, DisplayInfo->DesktopColorBitDepth, DisplayInfo->DesktopRefreshRate )) ;
	}

	for( j = 0 ; j < DisplayDeviceNum ; j ++ )
	{
		DWORD           ModeNum ;
		DWORD			ModeNum2 ;
		DEVMODEW        DevMode ;
		DISPLAYMODEDATA *ModeData ;

		if( OsVersionInfo.dwPlatformId >= VER_PLATFORM_WIN32_NT &&
			OsVersionInfo.dwMajorVersion >= 5 )
		{
			WinAPIData.Win32Func.EnumDisplayDevicesWFunc( NULL, j, &DisplayDevice, 0 ) ;
			DeviceName = DisplayDevice.DeviceName ;

			DisplayInfo = GSYS.Screen.DisplayInfo ;
			for( i = 0 ; i < ( DWORD )GSYS.Screen.DisplayNum ; i ++, DisplayInfo ++ )
			{
				if( _WCSCMP( DisplayInfo->Name, DeviceName ) == 0 )
				{
					break ;
				}
			}
			if( i == ( DWORD )GSYS.Screen.DisplayNum )
			{
				continue ;
			}
		}
		else
		{
			DisplayInfo = GSYS.Screen.DisplayInfo ;
			DeviceName = NULL ;
		}

//		DXST_LOGFILEFMT_ADDUTF16LE(( "\x4e\x00\x6f\x00\x2e\x00\x25\x00\x64\x00\x20\x00\x20\x00\xc7\x30\xa3\x30\xb9\x30\xd7\x30\xec\x30\xa4\x30\xc7\x30\xd0\x30\xa4\x30\xb9\x30\x0d\x54\x1a\xff\x25\x00\x73\x00\x00"/*@ L"No.%d  ディスプレイデバイス名：%s" @*/, j, DeviceName == NULL ? L"None" : DeviceName )) ;

		// 数を数える
		DisplayInfo->ModeNum = 0 ;
		ModeNum              = 0 ;
		ModeNum2             = 0 ;
		for( ModeNum ;  ; ModeNum ++ )
		{
			_MEMSET( &DevMode, 0, sizeof( DevMode ) ) ;
			if( WinAPIData.Win32Func.EnumDisplaySettingsWFunc( DeviceName, ModeNum, &DevMode ) == 0 )
			{
				break ;
			}

			// 16bit 以下のモードは除外する
			if( DevMode.dmBitsPerPel >= 16 )
			{
				ModeNum2 ++ ;
			}
		}

		// 情報を格納するメモリ領域の確保
		DisplayInfo->ModeData = ( DISPLAYMODEDATA * )WinAPIData.Win32Func.HeapAllocFunc( WinAPIData.Win32Func.GetProcessHeapFunc(), 0, sizeof( DISPLAYMODEDATA ) * ModeNum2 ) ;
		if( DisplayInfo->ModeData == NULL )
		{
			DisplayInfo->ModeNum = 0 ;
			return -1 ;
		}

		// 情報を取得する
		DisplayInfo->ModeNum = 0 ;
		ModeData = DisplayInfo->ModeData ;
		for( i = 0 ; i < ModeNum ; i ++ )
		{
			_MEMSET( &DevMode, 0, sizeof( DevMode ) ) ;
			WinAPIData.Win32Func.EnumDisplaySettingsWFunc( DeviceName, i, &DevMode ) ;

			// 16bit 以下のモードは除外する
			if( DevMode.dmBitsPerPel < 16 )
			{
				continue ;
			}

			// 重複する情報は除外する
			for( k = 0 ; k < ( DWORD )DisplayInfo->ModeNum ; k ++ )
			{
				if( DisplayInfo->ModeData[ k ].Width         == ( int )DevMode.dmPelsWidth &&
					DisplayInfo->ModeData[ k ].Height        == ( int )DevMode.dmPelsHeight &&
					DisplayInfo->ModeData[ k ].ColorBitDepth == ( int )DevMode.dmBitsPerPel &&
					DisplayInfo->ModeData[ k ].RefreshRate   == ( int )DevMode.dmDisplayFrequency )
				{
					break ;
				}
			}
			if( k != ( DWORD )DisplayInfo->ModeNum )
			{
				continue ;
			}

			ModeData->Width         = ( int )DevMode.dmPelsWidth ;
			ModeData->Height        = ( int )DevMode.dmPelsHeight ;
			ModeData->ColorBitDepth = ( int )DevMode.dmBitsPerPel ;
			ModeData->RefreshRate   = ( int )DevMode.dmDisplayFrequency ;
//			DXST_LOGFILEFMT_ADDUTF16LE(( "\x3b\x75\x62\x97\xe2\x30\xfc\x30\xc9\x30\x4e\x00\x6f\x00\x2e\x00\x25\x00\x64\x00\x20\x00\x20\x00\x25\x00\x64\x00\x78\x00\x25\x00\x64\x00\x20\x00\x25\x00\x64\x00\x62\x00\x69\x00\x74\x00\x20\x00\x25\x00\x64\x00\x48\x00\x7a\x00\x00"/*@ L"画面モードNo.%d  %dx%d %dbit %dHz" @*/, i, ModeData->Width, ModeData->Height, ModeData->ColorBitDepth, ModeData->RefreshRate )) ;
			ModeData ++ ;
			DisplayInfo->ModeNum ++ ;
		}
	}

	DXST_LOGFILE_TABSUB ;
	DXST_LOGFILEFMT_ADDUTF16LE(( "\xc7\x30\xa3\x30\xb9\x30\xd7\x30\xec\x30\xa4\x30\xc5\x60\x31\x58\x6e\x30\xbb\x30\xc3\x30\xc8\x30\xa2\x30\xc3\x30\xd7\x30\x8c\x5b\x86\x4e\x00"/*@ L"ディスプレイ情報のセットアップ完了" @*/ )) ;

	return 0 ;
}

// 垂直同期信号を待つ
extern	int		Graphics_Hardware_WaitVSync_PF( int SyncNum )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_WaitVSync_PF( SyncNum ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_WaitVSync_PF( SyncNum ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// ScreenFlip 実行時にＶＳＹＮＣ待ちをするかどうかを設定する
extern	int		Graphics_SetWaitVSyncFlag_PF( int /*Flag*/ )
{
	return 0 ;
}

// 裏画面と表画面を交換する
extern	int		Graphics_ScreenFlipBase_PF( void )
{
	SETUP_WIN_API

	// ハードウエア機能を使っているかどうかで処理を分岐
	if( GSYS.Setting.ValidHardware )
	{
		switch( GRAWIN.Setting.UseGraphicsAPI )
		{
#ifndef DX_NON_DIRECT3D11
		case GRAPHICS_API_DIRECT3D11_WIN32 :
			return Graphics_Hardware_D3D11_ScreenFlipBase_PF() ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
		case GRAPHICS_API_DIRECT3D9_WIN32 :
			return Graphics_Hardware_D3D9_ScreenFlipBase_PF() ;
#endif // DX_NON_DIRECT3D9

		default :
			return 0 ;
		}
	}
	else
	{
		// ソフトウエアレンダリングの場合
		RECT        WindRect ;
		char        Buffer[ sizeof( BITMAPINFOHEADER ) + sizeof( DWORD ) * 3 + 4 ] ;
		BITMAPINFO *pBmpInfo ;
		HDC         Dc ;
		int         DrawScreenWidth ;
		int         DrawScreenHeight ;

		// ウインドウモードの場合は転送先ウインドウのデスクトップ上の座標を割り出す
		if( NS_GetWindowModeFlag() == TRUE )
		{
			int    ClientHeight ;
			double ExRateX ;
			double ExRateY ;

			WindRect.left   = 0 ;
			WindRect.top    = 0 ;

			// 現在のウインドウのクライアント領域のサイズを得る
			NS_GetWindowSizeExtendRate( &ExRateX, &ExRateY ) ;
			NS_GetDrawScreenSize( &DrawScreenWidth, &DrawScreenHeight ) ;
			WindRect.right  = _DTOL( DrawScreenWidth  * ExRateX ) ;
			WindRect.bottom = _DTOL( DrawScreenHeight * ExRateY ) ;

			if( WinData.ToolBarUseFlag )
			{
				ClientHeight    = WinData.WindowRect.bottom - WinData.WindowRect.top ;
				WindRect.top    = ClientHeight - DrawScreenHeight ;
				WindRect.bottom = ClientHeight ;
			}
		}
		else
		{
			WindRect.left   = 0 ;
			WindRect.top    = 0 ;
			WindRect.right  = GSYS.Screen.MainScreenSizeX ;
			WindRect.bottom = GSYS.Screen.MainScreenSizeY ;
		}

		if( CheckValidMemImg( &GSYS.SoftRender.MainBufferMemImg ) == FALSE ||
			CheckValidMemImg( &GSYS.SoftRender.SubBufferMemImg  ) == FALSE )
		{
			return 0 ;
		}

		// ＶＳＹＮＣを待つ	
		if( GSYS.Screen.NotWaitVSyncFlag == FALSE )
		{
			DirectDraw7_LocalWaitVSync() ;
		}

		// バックバッファの透過色の部分を透過するフラグか、UpdateLayerdWindow を使用するフラグが立っている場合は処理を分岐
		if( WinData.BackBufferTransColorFlag && WinData.UseUpdateLayerdWindowFlag )
		{
			BASEIMAGE BackBufferImage ;
			RECT      LockRect ;
			int       OldTargetScreen ;
			int       OldTargetScreenSurface ;

			// ＶＳＹＮＣを待つ	
			if( GSYS.Screen.NotWaitVSyncFlag == FALSE ) DirectDraw7_LocalWaitVSync() ;

			// Graphics_Screen_LockDrawScreen を使う方法
			OldTargetScreen                           = GSYS.DrawSetting.TargetScreen[ 0 ] ;
			OldTargetScreenSurface                    = GSYS.DrawSetting.TargetScreenSurface[ 0 ] ;
			GSYS.DrawSetting.TargetScreen[ 0 ]        = DX_SCREEN_BACK ;
			GSYS.DrawSetting.TargetScreenSurface[ 0 ] = 0 ;
			LockRect.left                             = 0 ;
			LockRect.top                              = 0 ;
			LockRect.right                            = GSYS.Screen.MainScreenSizeX ;
			LockRect.bottom                           = GSYS.Screen.MainScreenSizeY ;
			if( Graphics_Screen_LockDrawScreen( &LockRect, &BackBufferImage, -1, -1, -1, TRUE, 0 ) < 0 )
			{
				return -1 ;
			}
			UpdateBackBufferTransColorWindow( &BackBufferImage ) ;
			Graphics_Screen_UnlockDrawScreen() ;
			GSYS.DrawSetting.TargetScreen[ 0 ]        = OldTargetScreen ;
			GSYS.DrawSetting.TargetScreenSurface[ 0 ] = OldTargetScreenSurface ;
		}
		else
		{
			MEMIMG *UseScreenMemImg ;

			// 画面に転送する領域を限定する場合は最初にサブバッファに転送する
			if( GSYS.Screen.ValidGraphDisplayArea )
			{
				RECT   SrcRect ;
				RECT   ClipRect ;
				MEMIMG BltSrcMemImg ;

				// サブバッファに転送する領域を設定する
				ClipRect.left   = 0 ;
				ClipRect.top    = 0 ;
				ClipRect.right  = GSYS.Screen.MainScreenSizeX ;
				ClipRect.bottom = GSYS.Screen.MainScreenSizeY ;

				SrcRect = GSYS.Screen.GraphDisplayArea ;
				RectClipping_Inline( &SrcRect, &ClipRect ) ;
				if( SrcRect.right  - SrcRect.left <= 0 ||
					SrcRect.bottom - SrcRect.top  <= 0 )
				{
					return -1 ;
				}

				// 抜き出す部分のみの MEMIMG を作成
				DerivationMemImg(
					&BltSrcMemImg,
					&GSYS.SoftRender.MainBufferMemImg,
					SrcRect.left,
					SrcRect.top,
					SrcRect.right  - SrcRect.left,
					SrcRect.bottom - SrcRect.top
				) ;

				// サブバッファ全体に転送
				DrawEnlargeMemImg(
					&GSYS.SoftRender.SubBufferMemImg,
					&BltSrcMemImg,
					&ClipRect,
					FALSE,
					NULL
				) ;

				UseScreenMemImg = &GSYS.SoftRender.SubBufferMemImg ;
			}
			else
			{
				UseScreenMemImg = &GSYS.SoftRender.MainBufferMemImg ;
			}

			// メインウインドウのＤＣを取得して転送する
			pBmpInfo = ( BITMAPINFO * )Buffer ;
			_MEMSET( Buffer, 0, sizeof( Buffer ) ) ;
			pBmpInfo->bmiHeader.biSize               = sizeof( pBmpInfo->bmiHeader ) ;
			pBmpInfo->bmiHeader.biWidth              =  GSYS.Screen.MainScreenSizeX ;
			pBmpInfo->bmiHeader.biHeight             = -GSYS.Screen.MainScreenSizeY ;
			pBmpInfo->bmiHeader.biPlanes             = 1 ;
			pBmpInfo->bmiHeader.biBitCount           = UseScreenMemImg->Base->ColorDataP->ColorBitDepth ;
			pBmpInfo->bmiHeader.biCompression        = BI_BITFIELDS ;
			( ( DWORD * )&pBmpInfo->bmiColors )[ 0 ] = UseScreenMemImg->Base->ColorDataP->RedMask ;
			( ( DWORD * )&pBmpInfo->bmiColors )[ 1 ] = UseScreenMemImg->Base->ColorDataP->GreenMask ;
			( ( DWORD * )&pBmpInfo->bmiColors )[ 2 ] = UseScreenMemImg->Base->ColorDataP->BlueMask ;

			// ピッチが１６バイトの倍数なので補正
			if( UseScreenMemImg->Base->ColorDataP->ColorBitDepth == 16 )
			{
				// 16bit
				pBmpInfo->bmiHeader.biWidth = ( pBmpInfo->bmiHeader.biWidth + 16 / 2 - 1 ) / ( 16 / 2 ) * ( 16 / 2 ) ;
			}
			else
			{
				// 32bit
				pBmpInfo->bmiHeader.biWidth = ( pBmpInfo->bmiHeader.biWidth + 16 / 4 - 1 ) / ( 16 / 4 ) * ( 16 / 4 ) ;
			}

			Dc = WinAPIData.Win32Func.GetDCFunc( GRAWIN.Setting.ScreenFlipTargetWindow ? GRAWIN.Setting.ScreenFlipTargetWindow : GetDisplayWindowHandle() ) ;
			if( Dc )
			{
				// ウインドウモードで３２０×２４０エミュレーションモードの場合は転送元と転送先のパラメータは固定
				if( NS_GetWindowModeFlag() && GSYS.Screen.Emulation320x240Flag )
				{
					WinAPIData.Win32Func.StretchDIBitsFunc(
						Dc,
						0,
						0,
						640,
						480, 
						0,
						0,
						320,
						240,
						UseScreenMemImg->UseImage, pBmpInfo, DIB_RGB_COLORS, SRCCOPY );
				}
				else
				{
					if( NS_GetWindowModeFlag() == FALSE )
					{
						// フルスクリーンモードの場合

						// 画面全体を黒く塗りつぶす指定があったら実行する
						if( GRAWIN.FullScreenBlackFillCounter > 0 )
						{
							GRAWIN.FullScreenBlackFillCounter -- ;

							// ウインドウの座標を左上端に移動する
							NS_SetWindowPosition( 0, 0 ) ;

							// 画面全体を黒で塗りつぶす
							{
								HBRUSH OldBrush;

								OldBrush = ( HBRUSH )WinAPIData.Win32Func.SelectObjectFunc( Dc, WinAPIData.Win32Func.GetStockObjectFunc( BLACK_BRUSH ) ) ;
								WinAPIData.Win32Func.RectangleFunc(
									Dc,
									0,
									0,
									GSYS.Screen.FullScreenUseDispModeData.Width,
									GSYS.Screen.FullScreenUseDispModeData.Height
								) ;
								WinAPIData.Win32Func.SelectObjectFunc( Dc, OldBrush) ;
							}
						}

						switch( GSYS.Screen.FullScreenResolutionModeAct )
						{
						case DX_FSRESOLUTIONMODE_NATIVE :
							WinAPIData.Win32Func.SetDIBitsToDeviceFunc(
								Dc,
								0, 0,
								( DWORD )GSYS.Screen.MainScreenSizeX, ( DWORD )GSYS.Screen.MainScreenSizeY,
								0, 0,
								0, ( UINT )GSYS.Screen.MainScreenSizeY, 
								UseScreenMemImg->UseImage, pBmpInfo, DIB_RGB_COLORS );
							break ;

						case DX_FSRESOLUTIONMODE_MAXIMUM :
						case DX_FSRESOLUTIONMODE_DESKTOP :
							// 転送先矩形のセットアップ
							Graphics_Screen_SetupFullScreenScalingDestRect() ;

//							DXST_LOGFILEFMT_ADDUTF16LE(( L"GSYS.Screen.FullScreenScalingDestRect %d %d %d %d",
//								GSYS.Screen.FullScreenScalingDestRect.left,
//								GSYS.Screen.FullScreenScalingDestRect.top,
//								GSYS.Screen.FullScreenScalingDestRect.right,
//								GSYS.Screen.FullScreenScalingDestRect.bottom
//							)) ;

							WinAPIData.Win32Func.StretchDIBitsFunc(
								Dc,
								GSYS.Screen.FullScreenScalingDestRect.left,
								GSYS.Screen.FullScreenScalingDestRect.top,
								GSYS.Screen.FullScreenScalingDestRect.right  - GSYS.Screen.FullScreenScalingDestRect.left,
								GSYS.Screen.FullScreenScalingDestRect.bottom - GSYS.Screen.FullScreenScalingDestRect.top,
								0,
								0,
								GSYS.Screen.MainScreenSizeX,
								GSYS.Screen.MainScreenSizeY,
								UseScreenMemImg->UseImage, pBmpInfo, DIB_RGB_COLORS, SRCCOPY );
							break ;
						}
					}
					else
					{
						int WindowSizeX ;
						int WindowSizeY ;

						WindowSizeX = WindRect.right  - WindRect.left ;
						WindowSizeY = WindRect.bottom - WindRect.top ;

						// ウインドウモードの場合
						if( GSYS.Screen.MainScreenSizeX == WindowSizeX && GSYS.Screen.MainScreenSizeY == WindowSizeY )
						{
							WinAPIData.Win32Func.SetDIBitsToDeviceFunc(
								Dc,
								0, 0,
								( DWORD )WindowSizeX, ( DWORD )WindowSizeY,
								0, 0,
								0, ( UINT )WindowSizeY, 
								UseScreenMemImg->UseImage, pBmpInfo, DIB_RGB_COLORS );
						}
						else
						{
							WinAPIData.Win32Func.StretchDIBitsFunc(
								Dc,
								0, 0,
								WindowSizeX, WindowSizeY, 
								0, 0,
								GSYS.Screen.MainScreenSizeX, GSYS.Screen.MainScreenSizeY,
								UseScreenMemImg->UseImage, pBmpInfo, DIB_RGB_COLORS, SRCCOPY );
						}
					}
				}

				WinAPIData.Win32Func.ReleaseDCFunc( GetDisplayWindowHandle(), Dc );
			}
		}
	}

	return 0 ;
}

// 裏画面の指定の領域をウインドウのクライアント領域の指定の領域に転送する
extern	int		Graphics_BltRectBackScreenToWindow_PF( HWND Window, RECT BackScreenRect, RECT WindowClientRect )
{
	SETUP_WIN_API

	// 矩形チェック
	if( BackScreenRect.left   == BackScreenRect.right    ||
		BackScreenRect.top    == BackScreenRect.bottom   ||
		WindowClientRect.left == WindowClientRect.right  ||
		WindowClientRect.top  == WindowClientRect.bottom )
	{
		return -1 ;
	}

	// ハードウエア機能を使っているかどうかで処理を分岐
	if( GSYS.Setting.ValidHardware )
	{
		// 使っている場合

		switch( GRAWIN.Setting.UseGraphicsAPI )
		{
#ifndef DX_NON_DIRECT3D11
		case GRAPHICS_API_DIRECT3D11_WIN32 :
			return Graphics_Hardware_D3D11_BltRectBackScreenToWindow_PF(  Window, BackScreenRect, WindowClientRect ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
		case GRAPHICS_API_DIRECT3D9_WIN32 :
			return Graphics_Hardware_D3D9_BltRectBackScreenToWindow_PF(  Window, BackScreenRect, WindowClientRect ) ;
#endif // DX_NON_DIRECT3D9

		default :
			return 0 ;
		}
	}
	else
	{
		// ソフトウエアレンダリングの場合
		char Buffer[ sizeof( BITMAPINFOHEADER ) + sizeof( DWORD ) * 3 + 4 ];
		BITMAPINFO *pBmpInfo;
		HDC Dc;

		if( CheckValidMemImg( &GSYS.SoftRender.MainBufferMemImg ) == FALSE ) return 0 ;

		// ＶＳＹＮＣを待つ	
		if( GSYS.Screen.NotWaitVSyncFlag == FALSE )
		{
			DirectDraw7_LocalWaitVSync() ;
		}

		// ウインドウのＤＣを取得して転送する
		pBmpInfo = (BITMAPINFO *)Buffer ;
		_MEMSET( Buffer, 0, sizeof( Buffer ) ) ;
		pBmpInfo->bmiHeader.biSize        = sizeof( pBmpInfo->bmiHeader ) ;
		pBmpInfo->bmiHeader.biWidth       =  GSYS.Screen.MainScreenSizeX ;
		pBmpInfo->bmiHeader.biHeight      = -GSYS.Screen.MainScreenSizeY ;
		pBmpInfo->bmiHeader.biPlanes      = 1;
		pBmpInfo->bmiHeader.biBitCount    = GSYS.SoftRender.MainBufferMemImg.Base->ColorDataP->ColorBitDepth ;
		pBmpInfo->bmiHeader.biCompression = BI_BITFIELDS ;
		( ( DWORD * )&pBmpInfo->bmiColors )[ 0 ] = GSYS.SoftRender.MainBufferMemImg.Base->ColorDataP->RedMask ;
		( ( DWORD * )&pBmpInfo->bmiColors )[ 1 ] = GSYS.SoftRender.MainBufferMemImg.Base->ColorDataP->GreenMask ;
		( ( DWORD * )&pBmpInfo->bmiColors )[ 2 ] = GSYS.SoftRender.MainBufferMemImg.Base->ColorDataP->BlueMask ;
		Dc = WinAPIData.Win32Func.GetDCFunc( Window );
		if( Dc )
		{
			WinAPIData.Win32Func.StretchDIBitsFunc(
				Dc,
				WindowClientRect.left, WindowClientRect.top,
				WindowClientRect.right - WindowClientRect.left,
				WindowClientRect.bottom - WindowClientRect.top,
				BackScreenRect.left, GSYS.Screen.MainScreenSizeY - BackScreenRect.bottom,
				BackScreenRect.right  - BackScreenRect.left,
				BackScreenRect.bottom - BackScreenRect.top,
				( BYTE * )GSYS.SoftRender.MainBufferMemImg.UseImage, pBmpInfo, DIB_RGB_COLORS, SRCCOPY );
			WinAPIData.Win32Func.ReleaseDCFunc( Window, Dc );
		}
	}

	// 終了
	return 0 ;
}

// ScreenFlip で画像を転送する先のウインドウを設定する( NULL を指定すると設定解除 )
extern	int		Graphics_SetScreenFlipTargetWindow_PF( HWND TargetWindow, double ScaleX, double ScaleY )
{
	GRAWIN.Setting.ScreenFlipTargetWindow = TargetWindow ;
	GRAWIN.Setting.ScreenFlipTargetWindowScaleX = ScaleX ;
	GRAWIN.Setting.ScreenFlipTargetWindowScaleY = ScaleY ;

	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_SetScreenFlipTargetWindow_PF( TargetWindow, ScaleX, ScaleY ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_SetScreenFlipTargetWindow_PF( TargetWindow, ScaleX, ScaleY ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// メイン画面のＺバッファの設定を変更する
extern	int		Graphics_Hardware_SetZBufferMode_PF( int ZBufferSizeX, int ZBufferSizeY, int ZBufferBitDepth )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_SetZBufferMode_PF(  ZBufferSizeX,  ZBufferSizeY,  ZBufferBitDepth ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_SetZBufferMode_PF(  ZBufferSizeX,  ZBufferSizeY,  ZBufferBitDepth ) ;
#endif // DX_NON_DIRECT3D9

	default :
#ifndef DX_NON_DIRECT3D11
		Graphics_Hardware_D3D11_SetZBufferMode_PF(  ZBufferSizeX,  ZBufferSizeY,  ZBufferBitDepth ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
		Graphics_Hardware_D3D9_SetZBufferMode_PF(  ZBufferSizeX,  ZBufferSizeY,  ZBufferBitDepth ) ;
#endif // DX_NON_DIRECT3D9
		return 0 ;
	}
}

// 描画先Ｚバッファのセット
extern	int		Graphics_Hardware_SetDrawZBuffer_PF( int DrawScreen, IMAGEDATA *Image )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_SetDrawZBuffer_PF( DrawScreen, Image ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_SetDrawZBuffer_PF( DrawScreen, Image ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 現在の画面のリフレッシュレートを取得する
extern	int		Graphics_GetRefreshRate_PF( void )
{
	int RefreshRate = -1 ;

	SETUP_WIN_API

	// リフレッシュレートの取得
//	if( WinData.WindowsVersion >= DX_WINDOWSVERSION_NT31 )
	{
		HDC hdc ;
		
		hdc = WinAPIData.Win32Func.GetDCFunc( NS_GetMainWindowHandle() ) ;
		if( hdc )
		{
			RefreshRate = WinAPIData.Win32Func.GetDeviceCapsFunc( hdc, VREFRESH ) ;
			WinAPIData.Win32Func.ReleaseDCFunc( NS_GetMainWindowHandle(), hdc ) ;

			if( RefreshRate < 20 ) RefreshRate = -1 ;
		}
	}

	return RefreshRate ;
}






















// 環境依存情報取得関係

// GetColor や GetColor2 で使用するカラーデータを取得する
extern	const COLORDATA *	Graphics_Hardware_GetMainColorData_PF( void )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_GetMainColorData_PF(  ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_GetMainColorData_PF(  ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// ディスプレーのカラーデータポインタを得る
extern	const COLORDATA *	Graphics_Hardware_GetDispColorData_PF( void )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_GetDispColorData_PF(  ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_GetDispColorData_PF(  ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 指定座標の色を取得する
extern	DWORD				Graphics_Hardware_GetPixel_PF( int x, int y )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_GetPixel_PF(  x,  y ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_GetPixel_PF(  x,  y ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 指定座標の色を取得する( float型 )
extern COLOR_F Graphics_Hardware_GetPixelF_PF( int x, int y )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_GetPixelF_PF(  x,  y ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_GetPixelF_PF(  x,  y ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return NS_GetColorF( -1.0f, -1.0f, -1.0f, -1.0f ) ;
	}
}

























// 環境依存画像関係

// YUVサーフェスを使った Theora 動画の内容をグラフィックスハンドルのテクスチャに転送する
extern	int		Graphics_Hardware_UpdateGraphMovie_TheoraYUV_PF( MOVIEGRAPH *Movie, IMAGEDATA *Image )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_UpdateGraphMovie_TheoraYUV_PF( Movie, Image ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_UpdateGraphMovie_TheoraYUV_PF( Movie, Image ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// グラフィックメモリ領域のロック
extern	int		Graphics_Hardware_GraphLock_PF( IMAGEDATA *Image, COLORDATA **ColorDataP, int WriteOnly )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_GraphLock_PF( Image, ColorDataP, WriteOnly ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_GraphLock_PF( Image, ColorDataP, WriteOnly ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// グラフィックメモリ領域のロック解除
extern	int		Graphics_Hardware_GraphUnlock_PF( IMAGEDATA *Image )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_GraphUnlock_PF( Image ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_GraphUnlock_PF( Image ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// グラフィックのＺバッファの状態を別のグラフィックのＺバッファにコピーする( DestGrHandle も SrcGrHandle もＺバッファを持っている描画可能画像で、且つアンチエイリアス画像ではないことが条件 )
extern	int		Graphics_Hardware_CopyGraphZBufferImage_PF( IMAGEDATA *DestImage, IMAGEDATA *SrcImage )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_CopyGraphZBufferImage_PF( DestImage, SrcImage ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_CopyGraphZBufferImage_PF( DestImage, SrcImage ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 画像データの初期化
extern	int		Graphics_Hardware_InitGraph_PF( void )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_InitGraph_PF( ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_InitGraph_PF( ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// グラフィックを特定の色で塗りつぶす
extern	int		Graphics_Hardware_FillGraph_PF( IMAGEDATA *Image, int Red, int Green, int Blue, int Alpha, int ASyncThread )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_FillGraph_PF( Image,  Red,  Green,  Blue,  Alpha,  ASyncThread ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_FillGraph_PF( Image,  Red,  Green,  Blue,  Alpha,  ASyncThread ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 描画可能画像やバックバッファから指定領域のグラフィックを取得する
extern	int		Graphics_Hardware_GetDrawScreenGraphBase_PF( IMAGEDATA *Image, IMAGEDATA *TargetImage, int TargetScreen, int TargetScreenSurface, int TargetScreenMipLevel, int TargetScreenWidth, int TargetScreenHeight, int x1, int y1, int x2, int y2, int destX, int destY )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_GetDrawScreenGraphBase_PF( Image, TargetImage,  TargetScreen,  TargetScreenSurface, TargetScreenMipLevel,  TargetScreenWidth,  TargetScreenHeight,  x1,  y1,  x2,  y2,  destX,  destY ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_GetDrawScreenGraphBase_PF( Image, TargetImage,  TargetScreen,  TargetScreenSurface, TargetScreenMipLevel,  TargetScreenWidth,  TargetScreenHeight,  x1,  y1,  x2,  y2,  destX,  destY ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// Graphics_Image_BltBmpOrBaseImageToGraph3 の機種依存部分用関数
extern int Graphics_Hardware_BltBmpOrBaseImageToGraph3_PF(
	const RECT		*SrcRect,
	      int		DestX,
	      int		DestY,
	      int		GrHandle,
	const BASEIMAGE	*RgbBaseImage,
	const BASEIMAGE	*AlphaBaseImage,
	      int		RedIsAlphaFlag,
	      int		UseTransColorConvAlpha,
	      int		TargetOrig,
	      int		ASyncThread
)
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_BltBmpOrBaseImageToGraph3_PF(
			SrcRect,
			DestX,
			DestY,
			GrHandle,
			RgbBaseImage,
			AlphaBaseImage,
			RedIsAlphaFlag,
			UseTransColorConvAlpha,
			TargetOrig,
			ASyncThread
		);
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_BltBmpOrBaseImageToGraph3_PF(
			SrcRect,
			DestX,
			DestY,
			GrHandle,
			RgbBaseImage,
			AlphaBaseImage,
			RedIsAlphaFlag,
			UseTransColorConvAlpha,
			TargetOrig,
			ASyncThread
		);
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 基本イメージのフォーマットを DX_BASEIMAGE_FORMAT_NORMAL に変換する必要があるかどうかをチェックする
// ( RequiredRgbBaseImageConvFlag と RequiredAlphaBaseImageConvFlag に入る値  TRUE:変換する必要がある  FALSE:変換する必要は無い )
extern	int		Graphics_CheckRequiredNormalImageConv_BaseImageFormat_PF(
	IMAGEDATA_ORIG *Orig,
	int             RgbBaseImageFormat,
	int            *RequiredRgbBaseImageConvFlag,
	int             AlphaBaseImageFormat,
	int            *RequiredAlphaBaseImageConvFlag
)
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_CheckRequiredNormalImageConv_BaseImageFormat_PF(
			Orig,
			RgbBaseImageFormat,
			RequiredRgbBaseImageConvFlag,
			AlphaBaseImageFormat,
			RequiredAlphaBaseImageConvFlag
		) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_CheckRequiredNormalImageConv_BaseImageFormat_PF(
			Orig,
			RgbBaseImageFormat,
			RequiredRgbBaseImageConvFlag,
			AlphaBaseImageFormat,
			RequiredAlphaBaseImageConvFlag
		) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// オリジナル画像情報中のテクスチャを作成する( 0:成功  -1:失敗 )
extern int Graphics_Hardware_CreateOrigTexture_PF( IMAGEDATA_ORIG *Orig, int ASyncThread )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_CreateOrigTexture_PF( Orig, ASyncThread ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_CreateOrigTexture_PF( Orig, ASyncThread ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// オリジナル画像情報中のテクスチャを解放する
extern int Graphics_Hardware_ReleaseOrigTexture_PF( IMAGEDATA_ORIG *Orig )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_ReleaseOrigTexture_PF( Orig ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_ReleaseOrigTexture_PF( Orig ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 指定のマルチサンプル数で使用できる最大クオリティ値を取得する
extern int Graphics_Hardware_GetMultiSampleQuality_PF( int Samples )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_GetMultiSampleQuality_PF( Samples ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_GetMultiSampleQuality_PF( Samples ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 作成するグラフィックハンドルで使用する環境依存のテクスチャフォーマットを指定する
extern int Graphics_Hardware_SetUsePlatformTextureFormat_PF( int PlatformTextureFormat )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_SetUsePlatformTextureFormat_PF( PlatformTextureFormat ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_SetUsePlatformTextureFormat_PF( PlatformTextureFormat ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}























// 環境依存頂点バッファ・インデックスバッファ関係

// 頂点バッファハンドルの頂点バッファを作成する
extern	int		Graphics_Hardware_VertexBuffer_Create_PF( VERTEXBUFFERHANDLEDATA *VertexBuffer )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_VertexBuffer_Create_PF( VertexBuffer ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_VertexBuffer_Create_PF( VertexBuffer ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 頂点バッファハンドルの後始末
extern	int		Graphics_Hardware_VertexBuffer_Terminate_PF( VERTEXBUFFERHANDLEDATA *VertexBuffer )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_VertexBuffer_Terminate_PF( VertexBuffer ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_VertexBuffer_Terminate_PF( VertexBuffer ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 頂点バッファに頂点データを転送する
extern	int		Graphics_Hardware_VertexBuffer_SetData_PF( VERTEXBUFFERHANDLEDATA *VertexBuffer, int SetIndex, const void *VertexData, int VertexNum )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_VertexBuffer_SetData_PF( VertexBuffer, SetIndex, VertexData, VertexNum ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_VertexBuffer_SetData_PF( VertexBuffer, SetIndex, VertexData, VertexNum ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// インデックスバッファハンドルのセットアップを行う
extern	int		Graphics_Hardware_IndexBuffer_Create_PF( INDEXBUFFERHANDLEDATA *IndexBuffer )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_IndexBuffer_Create_PF( IndexBuffer ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_IndexBuffer_Create_PF( IndexBuffer ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// インデックスバッファハンドルの後始末
extern	int		Graphics_Hardware_IndexBuffer_Terminate_PF( INDEXBUFFERHANDLEDATA *IndexBuffer )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_IndexBuffer_Terminate_PF( IndexBuffer ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_IndexBuffer_Terminate_PF( IndexBuffer ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// インデックスバッファにインデックスデータを転送する
extern	int		Graphics_Hardware_IndexBuffer_SetData_PF( INDEXBUFFERHANDLEDATA *IndexBuffer, int SetIndex, const void *IndexData, int IndexNum )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_IndexBuffer_SetData_PF( IndexBuffer,  SetIndex, IndexData,  IndexNum ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_IndexBuffer_SetData_PF( IndexBuffer,  SetIndex, IndexData,  IndexNum ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}





















// 環境依存ライト関係

// ライティングを行うかどうかを設定する
extern	int		Graphics_Hardware_Light_SetUse_PF( int Flag )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_Light_SetUse_PF( Flag ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_Light_SetUse_PF( Flag ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// グローバルアンビエントライトカラーを設定する
extern	int		Graphics_Hardware_Light_GlobalAmbient_PF( COLOR_F *Color )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_Light_GlobalAmbient_PF( Color ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_Light_GlobalAmbient_PF( Color ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// ライトパラメータをセット
extern	int		Graphics_Hardware_Light_SetState_PF( int LightNumber, LIGHTPARAM *LightParam )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_Light_SetState_PF(  LightNumber, LightParam ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_Light_SetState_PF(  LightNumber, LightParam ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// ライトの有効、無効を変更
extern	int		Graphics_Hardware_Light_SetEnable_PF( int LightNumber, int EnableState )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_Light_SetEnable_PF(  LightNumber,  EnableState ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_Light_SetEnable_PF(  LightNumber,  EnableState ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

























// 環境依存シャドウマップ関係

// シャドウマップデータに必要なテクスチャを作成する
extern	int		Graphics_Hardware_ShadowMap_CreateTexture_PF( SHADOWMAPDATA *ShadowMap, int ASyncThread )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_ShadowMap_CreateTexture_PF( ShadowMap, ASyncThread ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_ShadowMap_CreateTexture_PF( ShadowMap, ASyncThread ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// シャドウマップデータに必要なテクスチャを解放する
extern	int		Graphics_Hardware_ShadowMap_ReleaseTexture_PF( SHADOWMAPDATA *ShadowMap )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_ShadowMap_ReleaseTexture_PF( ShadowMap ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_ShadowMap_ReleaseTexture_PF( ShadowMap ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 頂点シェーダーに設定するシャドウマップの情報を更新する
extern	void	Graphics_Hardware_ShadowMap_RefreshVSParam_PF( void )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		Graphics_Hardware_D3D11_ShadowMap_RefreshVSParam_PF(  ) ;
		break ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		Graphics_Hardware_D3D9_ShadowMap_RefreshVSParam_PF(  ) ;
		break ;
#endif // DX_NON_DIRECT3D9

	default :
		break ;
	}
}

// ピクセルシェーダーに設定するシャドウマップの情報を更新する
extern	void	Graphics_Hardware_ShadowMap_RefreshPSParam_PF( void )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		Graphics_Hardware_D3D11_ShadowMap_RefreshPSParam_PF(  ) ;
		break ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		Graphics_Hardware_D3D9_ShadowMap_RefreshPSParam_PF(  ) ;
		break ;
#endif // DX_NON_DIRECT3D9

	default :
		break ;
	}
}

// シャドウマップへの描画の準備を行う
extern	int		Graphics_Hardware_ShadowMap_DrawSetup_PF( SHADOWMAPDATA *ShadowMap )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_ShadowMap_DrawSetup_PF( ShadowMap ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_ShadowMap_DrawSetup_PF( ShadowMap ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// シャドウマップへの描画を終了する
extern	int		Graphics_Hardware_ShadowMap_DrawEnd_PF( SHADOWMAPDATA *ShadowMap )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_ShadowMap_DrawEnd_PF( ShadowMap ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_ShadowMap_DrawEnd_PF( ShadowMap ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 描画で使用するシャドウマップを指定する( スロットは０か１かを指定可能 )
extern int Graphics_Hardware_ShadowMap_SetUse_PF( int SlotIndex, SHADOWMAPDATA *ShadowMap )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_ShadowMap_SetUse_PF( SlotIndex, ShadowMap ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_ShadowMap_SetUse_PF( SlotIndex, ShadowMap ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}
























// 環境依存シェーダー関係

// シェーダーハンドルを初期化する
extern	int		Graphics_Hardware_Shader_Create_PF( int ShaderHandle, int IsVertexShader, void *Image, int ImageSize, int ImageAfterFree, int ASyncThread )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_Shader_Create_PF(  ShaderHandle,  IsVertexShader, Image,  ImageSize,  ImageAfterFree,  ASyncThread ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_Shader_Create_PF(  ShaderHandle,  IsVertexShader, Image,  ImageSize,  ImageAfterFree,  ASyncThread ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// シェーダーハンドルの後始末
extern	int		Graphics_Hardware_Shader_TerminateHandle_PF( SHADERHANDLEDATA *Shader )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_Shader_TerminateHandle_PF( Shader ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_Shader_TerminateHandle_PF( Shader ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 使用できるシェーダーのバージョンを取得する( 0=使えない  200=シェーダーモデル２．０が使用可能  300=シェーダーモデル３．０が使用可能 )
extern	int		Graphics_Hardware_Shader_GetValidShaderVersion_PF( void )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_Shader_GetValidShaderVersion_PF() ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_Shader_GetValidShaderVersion_PF() ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 指定の名前を持つ定数が使用するシェーダー定数の番号を取得する
extern	int		Graphics_Hardware_Shader_GetConstIndex_PF( const wchar_t *ConstantName, SHADERHANDLEDATA *Shader )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_Shader_GetConstIndex_PF( ConstantName, Shader ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_Shader_GetConstIndex_PF( ConstantName, Shader ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 指定の名前を持つ定数が使用するシェーダー定数の数を取得する
extern	int		Graphics_Hardware_Shader_GetConstCount_PF( const wchar_t *ConstantName, SHADERHANDLEDATA *Shader )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_Shader_GetConstCount_PF( ConstantName, Shader ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_Shader_GetConstCount_PF( ConstantName, Shader ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 指定の名前を持つ浮動小数点定数のデフォルトパラメータが格納されているメモリアドレスを取得する
extern	const FLOAT4 *Graphics_Hardware_Shader_GetConstDefaultParamF_PF( const wchar_t *ConstantName, SHADERHANDLEDATA *Shader )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_Shader_GetConstDefaultParamF_PF( ConstantName, Shader ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_Shader_GetConstDefaultParamF_PF( ConstantName, Shader ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return NULL ;
	}
}

// シェーダー定数情報を設定する
extern	int		Graphics_Hardware_Shader_SetConst_PF( int TypeIndex, int SetIndex, int ConstantIndex, const void *Param, int ParamNum, int UpdateUseArea )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_Shader_SetConst_PF( TypeIndex,  SetIndex,  ConstantIndex, Param,  ParamNum,  UpdateUseArea ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_Shader_SetConst_PF( TypeIndex,  SetIndex,  ConstantIndex, Param,  ParamNum,  UpdateUseArea ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 指定領域のシェーダー定数情報をリセットする
extern	int		Graphics_Hardware_Shader_ResetConst_PF( int TypeIndex, int SetIndex, int ConstantIndex, int ParamNum )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_Shader_ResetConst_PF(  TypeIndex,  SetIndex,  ConstantIndex,  ParamNum ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_Shader_ResetConst_PF(  TypeIndex,  SetIndex,  ConstantIndex,  ParamNum ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

#ifndef DX_NON_MODEL
// ３Ｄモデル用のシェーダーコードの初期化を行う
extern	int		Graphics_Hardware_Shader_ModelCode_Init_PF( void )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_Shader_ModelCode_Init_PF() ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_Shader_ModelCode_Init_PF() ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}
#endif // DX_NON_MODEL























// 環境依存シェーダー用定数バッファ関係

// シェーダー用定数バッファハンドルを初期化する
extern int Graphics_Hardware_ShaderConstantBuffer_Create_PF( int ShaderConstantBufferHandle, int BufferSize, int ASyncThread )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_ShaderConstantBuffer_Create_PF( ShaderConstantBufferHandle, BufferSize, ASyncThread ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_ShaderConstantBuffer_Create_PF( ShaderConstantBufferHandle, BufferSize, ASyncThread ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// シェーダー用定数バッファハンドルの後始末
extern int Graphics_Hardware_ShaderConstantBuffer_TerminateHandle_PF( SHADERCONSTANTBUFFERHANDLEDATA *ShaderConstantBuffer )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_ShaderConstantBuffer_TerminateHandle_PF( ShaderConstantBuffer ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_ShaderConstantBuffer_TerminateHandle_PF( ShaderConstantBuffer ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// シェーダー用定数バッファハンドルの定数バッファのアドレスを取得する
extern void *Graphics_Hardware_ShaderConstantBuffer_GetBuffer_PF( SHADERCONSTANTBUFFERHANDLEDATA *ShaderConstantBuffer )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_ShaderConstantBuffer_GetBuffer_PF( ShaderConstantBuffer ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_ShaderConstantBuffer_GetBuffer_PF( ShaderConstantBuffer ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return NULL ;
	}
}

// シェーダー用定数バッファハンドルの定数バッファへの変更を適用する
extern int Graphics_Hardware_ShaderConstantBuffer_Update_PF( SHADERCONSTANTBUFFERHANDLEDATA *ShaderConstantBuffer )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_ShaderConstantBuffer_Update_PF( ShaderConstantBuffer ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_ShaderConstantBuffer_Update_PF( ShaderConstantBuffer ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// シェーダー用定数バッファハンドルの定数バッファを指定のシェーダーの指定のスロットにセットする
extern int Graphics_Hardware_ShaderConstantBuffer_Set_PF( SHADERCONSTANTBUFFERHANDLEDATA *ShaderConstantBuffer, int TargetShader /* DX_SHADERTYPE_VERTEX など */, int Slot )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_ShaderConstantBuffer_Set_PF( ShaderConstantBuffer, TargetShader, Slot ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_ShaderConstantBuffer_Set_PF( ShaderConstantBuffer, TargetShader, Slot ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}


























// 環境依存描画関係

// 頂点バッファに溜まった頂点データをレンダリングする
extern	int		Graphics_Hardware_RenderVertex( int ASyncThread )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_RenderVertex( -1, ASyncThread ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_RenderVertex( ASyncThread ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// ハードウエアアクセラレータ使用版 DrawBillboard3D
extern	int		Graphics_Hardware_DrawBillboard3D_PF( VECTOR Pos, float cx, float cy, float Size, float Angle, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, int ReverseXFlag, int ReverseYFlag, int DrawFlag, RECT *DrawArea )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawBillboard3D( Pos,  cx,  cy,  Size,  Angle, Image, BlendImage,  TransFlag,  ReverseXFlag, ReverseYFlag, DrawFlag, DrawArea ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawBillboard3D( Pos,  cx,  cy,  Size,  Angle, Image, BlendImage,  TransFlag,  ReverseXFlag, ReverseYFlag,  DrawFlag, DrawArea ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// ハードウエアアクセラレータ使用版 DrawModiBillboard3D
extern	int		Graphics_Hardware_DrawModiBillboard3D_PF( VECTOR Pos, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, int DrawFlag, RECT *DrawArea )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawModiBillboard3D(  Pos,  x1,  y1,  x2,  y2,  x3,  y3,  x4,  y4, Image, BlendImage,  TransFlag,  DrawFlag, DrawArea ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawModiBillboard3D(  Pos,  x1,  y1,  x2,  y2,  x3,  y3,  x4,  y4, Image, BlendImage,  TransFlag,  DrawFlag, DrawArea ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// ハードウエアアクセラレータ使用版 DrawGraph
extern	int		Graphics_Hardware_DrawGraph_PF( int x, int y, float xf, float yf, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, int IntFlag )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawGraph( x, y, xf, yf, Image, BlendImage, TransFlag, IntFlag ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawGraph( x, y, xf, yf, Image, BlendImage, TransFlag, IntFlag ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// ハードウエアアクセラレータ使用版 DrawExtendGraph
extern	int		Graphics_Hardware_DrawExtendGraph_PF( int x1, int y1, int x2, int y2, float x1f, float y1f, float x2f, float y2f, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, int IntFlag )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawExtendGraph(  x1,  y1,  x2,  y2,  x1f,  y1f,  x2f,  y2f, Image, BlendImage,  TransFlag,  IntFlag ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawExtendGraph(  x1,  y1,  x2,  y2,  x1f,  y1f,  x2f,  y2f, Image, BlendImage,  TransFlag,  IntFlag ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// ハードウエアアクセラレータ使用版 DrawRotaGraph
extern	int		Graphics_Hardware_DrawRotaGraph_PF( int x, int y, float xf, float yf, double ExRate, double Angle, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, int ReverseXFlag, int ReverseYFlag, int IntFlag )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawRotaGraph(  x,  y,  xf,  yf,  ExRate,  Angle, Image, BlendImage,  TransFlag,  ReverseXFlag, ReverseYFlag,  IntFlag ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawRotaGraph(  x,  y,  xf,  yf,  ExRate,  Angle, Image, BlendImage,  TransFlag,  ReverseXFlag, ReverseYFlag,  IntFlag ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// ハードウエアアクセラレータ使用版 DrawRotaGraphFast
extern	int		Graphics_Hardware_DrawRotaGraphFast_PF( int x, int y, float xf, float yf, float ExRate, float Angle, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, int ReverseXFlag, int ReverseYFlag, int IntFlag )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawRotaGraphFast(  x,  y,  xf,  yf,  ExRate,  Angle, Image, BlendImage,  TransFlag,  ReverseXFlag, ReverseYFlag,  IntFlag ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawRotaGraphFast(  x,  y,  xf,  yf,  ExRate,  Angle, Image, BlendImage,  TransFlag,  ReverseXFlag, ReverseYFlag,  IntFlag ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// ハードウエアアクセラレータ使用版 DrawModiGraph
extern	int		Graphics_Hardware_DrawModiGraph_PF( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, bool SimpleDrawFlag )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawModiGraph(  x1,  y1,  x2,  y2,  x3,  y3,  x4,  y4, Image, BlendImage,  TransFlag, SimpleDrawFlag ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawModiGraph(  x1,  y1,  x2,  y2,  x3,  y3,  x4,  y4, Image, BlendImage,  TransFlag, SimpleDrawFlag ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// ハードウエアアクセラレータ使用版 DrawModiGraphF
extern	int		Graphics_Hardware_DrawModiGraphF_PF( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, IMAGEDATA *Image, IMAGEDATA *BlendImage, int TransFlag, bool SimpleDrawFlag )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawModiGraphF(  x1,  y1,  x2,  y2,  x3,  y3,  x4,  y4, Image, BlendImage,  TransFlag,  SimpleDrawFlag ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawModiGraphF(  x1,  y1,  x2,  y2,  x3,  y3,  x4,  y4, Image, BlendImage,  TransFlag,  SimpleDrawFlag ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// ハードウエアアクセラレータ使用版 DrawSimpleQuadrangleGraphF
extern	int		Graphics_Hardware_DrawSimpleQuadrangleGraphF_PF( const GRAPHICS_DRAW_DRAWSIMPLEQUADRANGLEGRAPHF_PARAM *Param, IMAGEDATA *Image, IMAGEDATA *BlendImage )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawSimpleQuadrangleGraphF( Param, Image, BlendImage ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawSimpleQuadrangleGraphF( Param, Image, BlendImage ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// ハードウエアアクセラレータ使用版 DrawSimpleTriangleGraphF
extern	int		Graphics_Hardware_DrawSimpleTriangleGraphF_PF( const GRAPHICS_DRAW_DRAWSIMPLETRIANGLEGRAPHF_PARAM *Param, IMAGEDATA *Image, IMAGEDATA *BlendImage )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawSimpleTriangleGraphF( Param, Image, BlendImage ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawSimpleTriangleGraphF( Param, Image, BlendImage ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// ハードウエアアクセラレータ使用版 DrawFillBox
extern	int		Graphics_Hardware_DrawFillBox_PF( int x1, int y1, int x2, int y2, unsigned int Color )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawFillBox(  x1,  y1,  x2,  y2,  Color ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawFillBox(  x1,  y1,  x2,  y2,  Color ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// ハードウエアアクセラレータ使用版 DrawLineBox
extern	int		Graphics_Hardware_DrawLineBox_PF( int x1, int y1, int x2, int y2, unsigned int Color )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawLineBox(  x1,  y1,  x2,  y2,  Color ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawLineBox(  x1,  y1,  x2,  y2,  Color ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}


// ハードウエアアクセラレータ使用版 DrawLine
extern	int		Graphics_Hardware_DrawLine_PF( int x1, int y1, int x2, int y2, unsigned int Color )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawLine(  x1,  y1,  x2,  y2,  Color ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawLine(  x1,  y1,  x2,  y2,  Color ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// ハードウエアアクセラレータ使用版 DrawLine3D
extern	int		Graphics_Hardware_DrawLine3D_PF( VECTOR Pos1, VECTOR Pos2, unsigned int Color, int DrawFlag, RECT *DrawArea )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawLine3D(  Pos1,  Pos2,  Color,  DrawFlag, DrawArea ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawLine3D(  Pos1,  Pos2,  Color,  DrawFlag, DrawArea ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// ハードウエアアクセラレータ使用版 DrawCircle( 太さ指定あり )
extern	int		Graphics_Hardware_DrawCircle_Thickness_PF( int x, int y, int r, unsigned int Color, int Thickness )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawCircle_Thickness(  x,  y,  r,  Color,  Thickness ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawCircle_Thickness(  x,  y,  r,  Color,  Thickness ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// ハードウエアアクセラレータ使用版 DrawOval( 太さ指定あり )
extern	int		Graphics_Hardware_DrawOval_Thickness_PF( int x, int y, int rx, int ry, unsigned int Color, int Thickness )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawOval_Thickness(  x,  y,  rx,  ry,  Color,  Thickness ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawOval_Thickness(  x,  y,  rx,  ry,  Color,  Thickness ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// ハードウエアアクセラレータ使用版 DrawCircle
extern	int		Graphics_Hardware_DrawCircle_PF( int x, int y, int r, unsigned int Color, int FillFlag, int Rx_One_Minus, int Ry_One_Minus )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawCircle(  x,  y,  r,  Color,  FillFlag, Rx_One_Minus, Ry_One_Minus ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawCircle(  x,  y,  r,  Color,  FillFlag, Rx_One_Minus, Ry_One_Minus ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// ハードウエアアクセラレータ使用版 DrawOval
extern	int		Graphics_Hardware_DrawOval_PF( int x, int y, int rx, int ry, unsigned int Color, int FillFlag, int Rx_One_Minus, int Ry_One_Minus )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawOval(  x,  y,  rx,  ry,  Color,  FillFlag, Rx_One_Minus, Ry_One_Minus ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawOval(  x,  y,  rx,  ry,  Color,  FillFlag, Rx_One_Minus, Ry_One_Minus ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// ハードウエアアクセラレータ使用版 DrawTriangle
extern	int		Graphics_Hardware_DrawTriangle_PF( int x1, int y1, int x2, int y2, int x3, int y3, unsigned int Color, int FillFlag )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawTriangle(  x1,  y1,  x2,  y2,  x3,  y3,  Color,  FillFlag ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawTriangle(  x1,  y1,  x2,  y2,  x3,  y3,  Color,  FillFlag ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// ハードウエアアクセラレータ使用版 DrawTriangle3D
extern	int		Graphics_Hardware_DrawTriangle3D_PF( VECTOR Pos1, VECTOR Pos2, VECTOR Pos3, unsigned int Color, int FillFlag, int DrawFlag, RECT *DrawArea )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawTriangle3D(  Pos1,  Pos2,  Pos3,  Color,  FillFlag,  DrawFlag, DrawArea ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawTriangle3D(  Pos1,  Pos2,  Pos3,  Color,  FillFlag,  DrawFlag, DrawArea ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// ハードウエアアクセラレータ使用版 DrawQuadrangle
extern	int		Graphics_Hardware_DrawQuadrangle_PF( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int Color, int FillFlag )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawQuadrangle(  x1,  y1,  x2,  y2,  x3,  y3,  x4,  y4,  Color,  FillFlag ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawQuadrangle(  x1,  y1,  x2,  y2,  x3,  y3,  x4,  y4,  Color,  FillFlag ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// ハードウエアアクセラレータ使用版 DrawQuadrangle
extern	int		Graphics_Hardware_DrawQuadrangleF_PF( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int Color, int FillFlag )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawQuadrangleF(  x1,  y1,  x2,  y2,  x3,  y3,  x4,  y4,  Color,  FillFlag ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawQuadrangleF(  x1,  y1,  x2,  y2,  x3,  y3,  x4,  y4,  Color,  FillFlag ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// ハードウエアアクセラレータ使用版 DrawPixel
extern	int		Graphics_Hardware_DrawPixel_PF( int x, int y, unsigned int Color )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawPixel(  x,  y,  Color ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawPixel(  x,  y,  Color ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// ハードウエアアクセラレータ使用版 DrawPixel3D
extern	int		Graphics_Hardware_DrawPixel3D_PF( VECTOR Pos, unsigned int Color, int DrawFlag, RECT *DrawArea )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawPixel3D(  Pos,  Color,  DrawFlag, DrawArea ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawPixel3D(  Pos,  Color,  DrawFlag, DrawArea ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// ハードウエアアクセラレータ使用版 DrawPixelSet
extern	int		Graphics_Hardware_DrawPixelSet_PF( const POINTDATA *PointData, int Num )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawPixelSet( PointData,  Num ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawPixelSet( PointData,  Num ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// ハードウエアアクセラレータ使用版 DrawLineSet
extern	int		Graphics_Hardware_DrawLineSet_PF( const LINEDATA *LineData, int Num )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawLineSet( LineData,  Num ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawLineSet( LineData,  Num ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}


extern	int		Graphics_Hardware_DrawPrimitive_PF(                             const VERTEX_3D *Vertex, int VertexNum,                                    int PrimitiveType, IMAGEDATA *Image, int TransFlag )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawPrimitive( Vertex, VertexNum, PrimitiveType, Image,  TransFlag ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawPrimitive( Vertex, VertexNum, PrimitiveType, Image,  TransFlag ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

extern	int		Graphics_Hardware_DrawIndexedPrimitive_PF(                      const VERTEX_3D *Vertex, int VertexNum, const WORD *Indices, int IndexNum, int PrimitiveType, IMAGEDATA *Image, int TransFlag )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawIndexedPrimitive( Vertex,  VertexNum, Indices, IndexNum,  PrimitiveType, Image,  TransFlag ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawIndexedPrimitive( Vertex,  VertexNum, Indices, IndexNum,  PrimitiveType, Image,  TransFlag ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

extern	int		Graphics_Hardware_DrawPrimitiveLight_PF(                        const VERTEX3D  *Vertex, int VertexNum,                                    int PrimitiveType, IMAGEDATA *Image, int TransFlag )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawPrimitiveLight(                        Vertex,  VertexNum,                                     PrimitiveType, Image,  TransFlag ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawPrimitiveLight(                        Vertex,  VertexNum,                                     PrimitiveType, Image,  TransFlag ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

extern	int		Graphics_Hardware_DrawIndexedPrimitiveLight_PF(                 const VERTEX3D  *Vertex, int VertexNum, const WORD *Indices, int IndexNum, int PrimitiveType, IMAGEDATA *Image, int TransFlag )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawIndexedPrimitiveLight(                 Vertex, VertexNum, Indices,  IndexNum,  PrimitiveType, Image,  TransFlag ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawIndexedPrimitiveLight(                 Vertex, VertexNum, Indices,  IndexNum,  PrimitiveType, Image,  TransFlag ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

extern	int		Graphics_Hardware_DrawPrimitiveLight_UseVertexBuffer_PF(        VERTEXBUFFERHANDLEDATA *VertexBuffer,                                      int PrimitiveType,                 int StartVertex, int UseVertexNum, IMAGEDATA *Image, int TransFlag )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawPrimitiveLight_UseVertexBuffer(        VertexBuffer,                                       PrimitiveType,                  StartVertex,  UseVertexNum, Image,  TransFlag ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawPrimitiveLight_UseVertexBuffer(        VertexBuffer,                                       PrimitiveType,                  StartVertex,  UseVertexNum, Image,  TransFlag ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

extern	int		Graphics_Hardware_DrawIndexedPrimitiveLight_UseVertexBuffer_PF( VERTEXBUFFERHANDLEDATA *VertexBuffer, INDEXBUFFERHANDLEDATA *IndexBuffer,  int PrimitiveType, int BaseVertex, int StartVertex, int UseVertexNum, int StartIndex, int UseIndexNum, IMAGEDATA *Image, int TransFlag )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawIndexedPrimitiveLight_UseVertexBuffer( VertexBuffer, IndexBuffer,   PrimitiveType,  BaseVertex,  StartVertex,  UseVertexNum,  StartIndex,  UseIndexNum, Image,  TransFlag ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawIndexedPrimitiveLight_UseVertexBuffer( VertexBuffer, IndexBuffer,   PrimitiveType,  BaseVertex,  StartVertex,  UseVertexNum,  StartIndex,  UseIndexNum, Image,  TransFlag ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

extern	int		Graphics_Hardware_DrawPrimitive2D_PF(                                 VERTEX_2D *Vertex, int VertexNum,                                    int PrimitiveType, IMAGEDATA *Image, int TransFlag, int BillboardFlag, int Is3D, int ReverseXFlag, int ReverseYFlag, int TextureNo, int IsShadowMap )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawPrimitive2D(                                 Vertex,  VertexNum,                                     PrimitiveType, Image,  TransFlag,  BillboardFlag,  Is3D,  ReverseXFlag, ReverseYFlag,  TextureNo, IsShadowMap ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawPrimitive2D(                                 Vertex,  VertexNum,                                     PrimitiveType, Image,  TransFlag,  BillboardFlag,  Is3D,  ReverseXFlag, ReverseYFlag,  TextureNo, IsShadowMap ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

extern	int		Graphics_Hardware_DrawPrimitive2DUser_PF(                       const VERTEX2D  *Vertex, int VertexNum,                                    int PrimitiveType, IMAGEDATA *Image, int TransFlag, int Is3D, int ReverseXFlag, int ReverseYFlag, int TextureNo )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawPrimitive2DUser(                       Vertex,  VertexNum,                                     PrimitiveType, Image,  TransFlag,  Is3D,  ReverseXFlag, ReverseYFlag,  TextureNo ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawPrimitive2DUser(                       Vertex,  VertexNum,                                     PrimitiveType, Image,  TransFlag,  Is3D,  ReverseXFlag, ReverseYFlag,  TextureNo ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

extern	int		Graphics_Hardware_DrawIndexedPrimitive2DUser_PF(                const VERTEX2D  *Vertex, int VertexNum, const WORD *Indices, int IndexNum, int PrimitiveType, IMAGEDATA *Image, int TransFlag )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawIndexedPrimitive2DUser(                Vertex,  VertexNum, Indices,  IndexNum,  PrimitiveType, Image,  TransFlag ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawIndexedPrimitive2DUser(                Vertex,  VertexNum, Indices,  IndexNum,  PrimitiveType, Image,  TransFlag ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// シェーダーを使って２Ｄプリミティブを描画する
extern	int		Graphics_Hardware_DrawPrimitive2DToShader_PF(        const VERTEX2DSHADER *Vertex, int VertexNum,                                              int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawPrimitive2DToShader(        Vertex,  VertexNum,                                               PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawPrimitive2DToShader(        Vertex,  VertexNum,                                               PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// シェーダーを使って３Ｄプリミティブを描画する
extern	int		Graphics_Hardware_DrawPrimitive3DToShader_PF(        const VERTEX3DSHADER *Vertex, int VertexNum,                                              int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawPrimitive3DToShader(        Vertex,  VertexNum,                                               PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawPrimitive3DToShader(        Vertex,  VertexNum,                                               PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// シェーダーを使って２Ｄプリミティブを描画する( 頂点インデックスを使用する )
extern	int		Graphics_Hardware_DrawPrimitiveIndexed2DToShader_PF( const VERTEX2DSHADER *Vertex, int VertexNum, const unsigned short *Indices, int IndexNum, int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawPrimitiveIndexed2DToShader( Vertex,  VertexNum, Indices,  IndexNum,  PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawPrimitiveIndexed2DToShader( Vertex,  VertexNum, Indices,  IndexNum,  PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// シェーダーを使って３Ｄプリミティブを描画する( 頂点インデックスを使用する )
extern	int		Graphics_Hardware_DrawPrimitiveIndexed3DToShader_PF( const VERTEX3DSHADER *Vertex, int VertexNum, const unsigned short *Indices, int IndexNum, int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawPrimitiveIndexed3DToShader( Vertex,  VertexNum, Indices,  IndexNum,  PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawPrimitiveIndexed3DToShader( Vertex,  VertexNum, Indices,  IndexNum,  PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// シェーダーを使って３Ｄプリミティブを描画する( 頂点バッファ使用版 )
extern	int		Graphics_Hardware_DrawPrimitive3DToShader_UseVertexBuffer2_PF(        int VertexBufHandle,                     int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */, int StartVertex, int UseVertexNum )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawPrimitive3DToShader_UseVertexBuffer2(         VertexBufHandle,                      PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */,  StartVertex,  UseVertexNum ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawPrimitive3DToShader_UseVertexBuffer2(         VertexBufHandle,                      PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */,  StartVertex,  UseVertexNum ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// シェーダーを使って３Ｄプリミティブを描画する( 頂点バッファとインデックスバッファ使用版 )
extern	int		Graphics_Hardware_DrawPrimitiveIndexed3DToShader_UseVertexBuffer2_PF( int VertexBufHandle, int IndexBufHandle, int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */, int BaseVertex, int StartVertex, int UseVertexNum, int StartIndex, int UseIndexNum )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_D3D11_DrawPrimitiveIndexed3DToShader_UseVertexBuffer2(  VertexBufHandle,  IndexBufHandle,  PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */,  BaseVertex,  StartVertex,  UseVertexNum,  StartIndex,  UseIndexNum ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_D3D9_DrawPrimitiveIndexed3DToShader_UseVertexBuffer2(  VertexBufHandle,  IndexBufHandle,  PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */,  BaseVertex,  StartVertex,  UseVertexNum,  StartIndex,  UseIndexNum ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 指定点から境界色があるところまで塗りつぶす
extern	int		Graphics_Hardware_Paint_PF( int x, int y, unsigned int FillColor, ULONGLONG BoundaryColor )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return Graphics_Hardware_D3D11_Paint_PF(  x,  y,  FillColor,  BoundaryColor ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return Graphics_Hardware_D3D9_Paint_PF(  x,  y,  FillColor,  BoundaryColor ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}






























#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE


#endif // DX_NON_GRAPHICS
