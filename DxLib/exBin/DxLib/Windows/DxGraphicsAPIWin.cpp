// ----------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		描画ＡＰＩプログラム
// 
// 				Ver 3.20c
// 
// ----------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

#include "DxGraphicsAPIWin.h"

#ifndef DX_NON_GRAPHICS

// インクルード----------------------------------------------------------------
#include "DxGraphicsWin.h"
#include "DxGraphicsD3D9.h"
#include "DxGraphicsD3D11.h"
#include "DxWinAPI.h"
#include "DxWindow.h"
#include "DxGuid.h"
#include "../DxGraphics.h"
#include "../DxASyncLoad.h"
#include "../DxLog.h"
#include "../DxBaseFunc.h"
#include "../DxModel.h"
#include "../DxMath.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 -----------------------------------------------------------------

// タイマーＩＤ
#define SCREENFLIPTIMER_ID		(32767)

// 構造体宣言 -----------------------------------------------------------------

// データ定義 -----------------------------------------------------------------

GRAPHICSAPIINFO_WIN GAPIWin ;

// 関数宣言 -------------------------------------------------------------------

#if _MSC_VER > 1200 || defined( DX_GCC_COMPILE_4_9_2 )
static  VOID CALLBACK ScreenFlipTimerProc( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime );							// テンポラリプライマリバッファの内容をプライマリバッファに転送するコールバック関数
#else
static  VOID CALLBACK ScreenFlipTimerProc( HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime );								// テンポラリプライマリバッファの内容をプライマリバッファに転送するコールバック関数
#endif

// プログラム -----------------------------------------------------------------






extern int DirectDraw7_Create( void )
{
	HRESULT hr ;

	SETUP_WIN_API

	// 既に作成済みの場合は一度削除する
	if( GAPIWin.DirectDraw7Object )
	{
		GAPIWin.DirectDraw7Object->Release() ;
		GAPIWin.DirectDraw7Object = NULL ;
	}

	// ＤｉｒｅｃｔＤｒａｗオブジェクトの作成
	DXST_LOGFILE_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x44\x00\x72\x00\x61\x00\x77\x00\x20\x00\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\xd6\x53\x97\x5f\x92\x30\x4c\x88\x44\x30\x7e\x30\x59\x30\x2e\x00\x2e\x00\x2e\x00\x2e\x00\x20\x00\x20\x00\x00"/*@ L"DirectDraw オブジェクトの取得を行います....  " @*/ ) ;
	hr = WinAPIData.Win32Func.CoCreateInstanceFunc( CLSID_DIRECTDRAW7, NULL, CLSCTX_ALL, IID_IDIRECTDRAW7, ( LPVOID *)&GAPIWin.DirectDraw7Object );
	if( !FAILED( hr ) )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"成功\n" @*/ ) ;
		DXST_LOGFILE_ADDUTF16LE( "\x15\x5f\x4d\x30\x9a\x7d\x4d\x30\x1d\x52\x1f\x67\x16\x53\xe6\x51\x06\x74\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"引き続き初期化処理... " @*/ ) ; 
		hr = GAPIWin.DirectDraw7Object->Initialize( NULL ) ;
		if( FAILED( hr ) ) 
		{
			DXST_LOGFILE_ERRCODE_ADDUTF16LE( DX_ERRORCODE_WIN_FAILED_INITIALIZE_DIRECTDRAW7, "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x44\x00\x72\x00\x61\x00\x77\x00\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\x1d\x52\x1f\x67\x16\x53\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"DirectDrawオブジェクトの初期化に失敗しました\n" @*/ );
			GAPIWin.DirectDraw7Object->Release() ;
			return -1 ;
		}
		DXST_LOGFILE_ADDUTF16LE( "\x1d\x52\x1f\x67\x16\x53\x6b\x30\x10\x62\x9f\x52\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"初期化に成功しました\n" @*/ ) ;
	}
	else
	{
		DXST_LOGFILE_ERRCODE_ADDUTF16LE( DX_ERRORCODE_WIN_FAILED_CREATE_DIRECTDRAW7, "\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"オブジェクトの取得に失敗しました\n" @*/ ) ;
		return -1 ;
	}

	// 協調レベルを適当にセット
	GAPIWin.DirectDraw7Object->SetCooperativeLevel( NS_GetMainWindowHandle(), ( DWORD )( D_DDSCL_NORMAL | ( WinData.UseFPUPreserve ? D_DDSCL_FPUPRESERVE : 0 ) ) ) ;

	// Aero を無効にする処理
	if( GRAWIN.Setting.DisableAeroFlag == 2 )
	{
		// DWM を無効にする
		SetEnableAero( FALSE ) ;
#if 0
		D_DDSURFACEDESC2 ddsd ;
		D_IDirectDrawSurface7 *prmsuf ;
		HDC prmdc ;
		HRESULT hr ;

		//  作成パラメータのセット	
		_MEMSET( &ddsd, 0, sizeof( ddsd ) ) ; 
		ddsd.dwSize				= sizeof( ddsd ) ;
		ddsd.dwFlags			= D_DDSD_CAPS ;	
		ddsd.ddsCaps.dwCaps		= D_DDSCAPS_PRIMARYSURFACE | D_DDSCAPS_3DDEVICE ;

		// プライマリサーフェスの作成
		hr = GAPIWin.DirectDraw7Object->CreateSurface( &ddsd, &prmsuf, NULL ) ;
		if( hr == D_DD_OK )
		{
			// GetDC をして Aero 無効
			hr = prmsuf->GetDC( &prmdc ) ;
			if( hr == D_DD_OK )
			{
				prmsuf->ReleaseDC( prmdc ) ;
				prmdc = NULL ;
			}
			prmsuf->Release() ;
			prmsuf = NULL ;
		}
#endif
	}

	// 終了
	return 0 ;
}

extern int DirectDraw7_Release( void )
{
	if( GAPIWin.DirectDraw7Object )
	{
		GAPIWin.DirectDraw7Object->Release() ;
		GAPIWin.DirectDraw7Object = NULL ;
	}

	// 終了
	return 0 ;
}

extern void *DirectDraw7_GetObject( void )
{
	return GAPIWin.DirectDraw7Object ;
}

extern int DirectDraw7_GetVideoMemorySize( int *TotalSize, int *FreeSize )
{
	D_DDSCAPS2 caps ;
	DWORD TotalMem, FreeMem ;

	if( GAPIWin.DirectDraw7Object == NULL ) return -1 ;

	_MEMSET( &caps, 0, sizeof( caps ) );
	caps.dwCaps = D_DDSCAPS_VIDEOMEMORY | D_DDSCAPS_LOCALVIDMEM ;
	GAPIWin.DirectDraw7Object->GetAvailableVidMem( &caps, &TotalMem, &FreeMem ) ;

	if( TotalSize )
	{
		*TotalSize = ( int )TotalMem ;
	}

	if( FreeSize )
	{
		*FreeSize  = ( int )FreeMem ;
	}

	// 終了
	return 0 ;
}

// ＶＳＹＮＣの前にSleepする時間を取得する
extern void DirectDraw7_WaitVSyncInitialize( void )
{
	GAPIWin.DirectDraw7_VSyncWaitTime = -1 ;
	if( GAPIWin.DirectDraw7Object )
	{
//		D_DDSURFACEDESC2 Desc2 ;

//		GAPIWin.DirectDraw7Object->GetDisplayMode( &Desc2 ) ;
//		if( Desc2.dwRefreshRate == 0 )
		{
			LONGLONG Time1, Time2, DeltaTime1, DeltaTime2, DeltaTimeTotal, DeltaTimeSize ;
			int Count, LoopCount ;

			DeltaTime1 = 0 ;
			DeltaTime2 = 0 ;
			DeltaTimeTotal = 0 ;
			Count = 0 ;
			LoopCount = 0 ;
			GAPIWin.DirectDraw7Object->WaitForVerticalBlank( D_DDWAITVB_BLOCKBEGIN, 0 ) ;
			Time1 = NS_GetNowHiPerformanceCount() ;
			while( Count < 10 && LoopCount < 60 )
			{
				GAPIWin.DirectDraw7Object->WaitForVerticalBlank( D_DDWAITVB_BLOCKBEGIN, 0 ) ;
				Time2 = NS_GetNowHiPerformanceCount() ;
				DeltaTime2 = Time2 - Time1 ;
				DeltaTimeSize = DeltaTime1 - DeltaTime2 ;
				if( DeltaTimeSize < 0 ) DeltaTimeSize = -DeltaTimeSize ;
				if( DeltaTimeSize < 1000 )
				{
					DeltaTimeTotal += DeltaTime2 ;
					Count ++ ;
				}
				else
				{
					DeltaTimeTotal = 0 ;
					Count = 0 ;
				}
				Time1 = Time2 ;
				DeltaTime1 = DeltaTime2 ;
				LoopCount ++ ;
				if( Count >= 10 )
				{
					GAPIWin.DirectDraw7_VSyncWaitTime = ( LONGLONG )( ( int )DeltaTimeTotal / Count ) ;
				}
				WinAPIData.Win32Func.SleepFunc( 5 ) ;
			}
		}
//		else
//		{
//		}
	}

	// １フレーム待つ
	DirectDraw7_LocalWaitVSync() ;
}

extern void DirectDraw7_LocalWaitVSync( void )
{
//	BOOL flag ;
	LONGLONG WaitTime ;

	SETUP_WIN_API

	// 前回ＶＳＹＮＣ待ちをしてから最低限待つ時間が過ぎているかどうかで処理を分岐
	WaitTime = GAPIWin.DirectDraw7_VSyncWaitTime != -1 ? GAPIWin.DirectDraw7_VSyncWaitTime - 3000 : 4000 ; 
	if( NS_GetNowHiPerformanceCount() - GAPIWin.DirectDraw7_VSyncTime < WaitTime )
	{
		WinAPIData.Win32Func.SleepFunc( 0 ) ;
		while( NS_GetNowHiPerformanceCount() - GAPIWin.DirectDraw7_VSyncTime < WaitTime ) WinAPIData.Win32Func.SleepFunc( 1 ) ;
		GAPIWin.DirectDraw7Object->WaitForVerticalBlank( D_DDWAITVB_BLOCKBEGIN, 0 ) ;
		GAPIWin.DirectDraw7_VSyncTime = NS_GetNowHiPerformanceCount() ;
	}
	else
	{
//		GAPIWin.DirectDraw7Object->GetVerticalBlankStatus( &flag ) ;
//		if( flag == FALSE )
		GAPIWin.DirectDraw7Object->WaitForVerticalBlank( D_DDWAITVB_BLOCKBEGIN, 0 ) ;
		GAPIWin.DirectDraw7_VSyncTime = NS_GetNowHiPerformanceCount() ;
		WinAPIData.Win32Func.SleepFunc( 0 ) ;
	}
}

extern int DirectDraw7_WaitVSync( int SyncNum )
{
	int i ;
	BOOL vsync ;

	if( WinData.ActiveFlag == FALSE )
		DxActiveWait() ;

	if( GAPIWin.DirectDraw7Object == NULL ) return -1 ;
//	if( GSYS.NotDrawFlag ) return 0 ;
	
	if( SyncNum == 0 )
	{
		vsync = FALSE ;
		while( NS_ProcessMessage() == 0 && vsync == FALSE && GAPIWin.DirectDraw7Object != NULL )
			GAPIWin.DirectDraw7Object->GetVerticalBlankStatus( &vsync ) ;
	}
	else
	{
		for( i = 0 ; i < SyncNum ; i ++ )
		{
			vsync = TRUE ;
			while( NS_ProcessMessage() == 0 && vsync == TRUE && GAPIWin.DirectDraw7Object != NULL )
			{
				GAPIWin.DirectDraw7Object->GetVerticalBlankStatus( &vsync ) ;
			}
		
	//		GAPIWin.DirectDraw7Object->WaitForVerticalBlank( DDWAITVB_BLOCKBEGIN, 0 ) ;
			vsync = FALSE ;
			while( NS_ProcessMessage() == 0 && vsync == FALSE && GAPIWin.DirectDraw7Object != NULL )
				GAPIWin.DirectDraw7Object->GetVerticalBlankStatus( &vsync ) ;
			if( NS_ProcessMessage() < 0 )
			{
				return -1 ; 
			}
		}
	}

	// 終了
	return 0 ;
}









#ifndef DX_NON_DIRECT3D11


#ifndef DX_NON_ASYNCLOAD
extern int Direct3D11_ObjectRelease_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return ( int )Direct3D11_ObjectRelease_ASync(
				( void * )Info->Data[ 0 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern ULONG Direct3D11_ObjectRelease_ASync( void *pObject, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3D11_ObjectRelease_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pObject ;
		return ( ULONG )AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3D11_ObjectRelease( pObject ) ;
}








#ifndef DX_NON_ASYNCLOAD
extern int Direct3D11_Release_Buffer_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return ( int )Direct3D11_Release_Buffer_ASync(
				( D_ID3D11Buffer * )Info->Data[ 0 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern ULONG Direct3D11_Release_Buffer_ASync( D_ID3D11Buffer *pBuffer, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3D11_Release_Buffer_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pBuffer ;
		return ( ULONG )AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3D11_Release_Buffer( pBuffer ) ;
}








#ifndef DX_NON_ASYNCLOAD
extern int Direct3D11_Release_Texture1D_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return ( int )Direct3D11_Release_Texture1D_ASync(
				( D_ID3D11Texture1D * )Info->Data[ 0 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern ULONG Direct3D11_Release_Texture1D_ASync( D_ID3D11Texture1D *pTexture1D, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3D11_Release_Texture1D_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pTexture1D ;
		return ( ULONG )AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3D11_Release_Texture1D( pTexture1D ) ;
}








#ifndef DX_NON_ASYNCLOAD
extern int Direct3D11_Release_Texture2D_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return ( int )Direct3D11_Release_Texture2D_ASync(
				( D_ID3D11Texture2D * )Info->Data[ 0 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern ULONG Direct3D11_Release_Texture2D_ASync( D_ID3D11Texture2D *pTexture2D, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3D11_Release_Texture2D_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pTexture2D ;
		return ( ULONG )AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3D11_Release_Texture2D( pTexture2D ) ;
}








#ifndef DX_NON_ASYNCLOAD
extern int Direct3D11_Release_Texture3D_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return ( int )Direct3D11_Release_Texture3D_ASync(
				( D_ID3D11Texture3D * )Info->Data[ 0 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern ULONG Direct3D11_Release_Texture3D_ASync( D_ID3D11Texture3D *pTexture3D, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3D11_Release_Texture3D_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pTexture3D ;
		return ( ULONG )AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3D11_Release_Texture3D( pTexture3D ) ;
}








#ifndef DX_NON_ASYNCLOAD
extern int Direct3D11_Release_ShaderResourceView_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return ( int )Direct3D11_Release_ShaderResourceView_ASync(
				( D_ID3D11ShaderResourceView * )Info->Data[ 0 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern ULONG Direct3D11_Release_ShaderResourceView_ASync( D_ID3D11ShaderResourceView *pSRView, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3D11_Release_ShaderResourceView_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pSRView ;
		return ( ULONG )AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3D11_Release_ShaderResourceView( pSRView ) ;
}








#ifndef DX_NON_ASYNCLOAD
extern int Direct3D11_Release_RenderTargetView_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return ( int )Direct3D11_Release_RenderTargetView_ASync(
				( D_ID3D11RenderTargetView * )Info->Data[ 0 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern ULONG Direct3D11_Release_RenderTargetView_ASync( D_ID3D11RenderTargetView *pRTView, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3D11_Release_RenderTargetView_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pRTView ;
		return ( ULONG )AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3D11_Release_RenderTargetView( pRTView ) ;
}








#ifndef DX_NON_ASYNCLOAD
extern int Direct3D11_Release_DepthStencilView_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return ( int )Direct3D11_Release_DepthStencilView_ASync(
				( D_ID3D11DepthStencilView * )Info->Data[ 0 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern ULONG Direct3D11_Release_DepthStencilView_ASync( D_ID3D11DepthStencilView *pDSView, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3D11_Release_DepthStencilView_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pDSView ;
		return ( ULONG )AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3D11_Release_DepthStencilView( pDSView ) ;
}








#ifndef DX_NON_ASYNCLOAD
extern int Direct3D11_Release_InputLayout_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return ( int )Direct3D11_Release_InputLayout_ASync(
				( D_ID3D11InputLayout * )Info->Data[ 0 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern ULONG Direct3D11_Release_InputLayout_ASync( D_ID3D11InputLayout *pInputLayout, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3D11_Release_InputLayout_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pInputLayout ;
		return ( ULONG )AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3D11_Release_InputLayout( pInputLayout ) ;
}








#ifndef DX_NON_ASYNCLOAD
extern int Direct3D11_Release_VertexShader_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return ( int )Direct3D11_Release_VertexShader_ASync(
				( D_ID3D11VertexShader * )Info->Data[ 0 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern ULONG Direct3D11_Release_VertexShader_ASync( D_ID3D11VertexShader *pVertexShader, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3D11_Release_VertexShader_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pVertexShader ;
		return ( ULONG )AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3D11_Release_VertexShader( pVertexShader ) ;
}








#ifndef DX_NON_ASYNCLOAD
extern int Direct3D11_Release_PixelShader_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return ( int )Direct3D11_Release_PixelShader_ASync(
				( D_ID3D11PixelShader * )Info->Data[ 0 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern ULONG Direct3D11_Release_PixelShader_ASync( D_ID3D11PixelShader *pPixelShader, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3D11_Release_PixelShader_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pPixelShader ;
		return ( ULONG )AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3D11_Release_PixelShader( pPixelShader ) ;
}








#ifndef DX_NON_ASYNCLOAD
extern int Direct3D11_Release_BlendState_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return ( int )Direct3D11_Release_BlendState_ASync(
				( D_ID3D11BlendState * )Info->Data[ 0 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern ULONG Direct3D11_Release_BlendState_ASync( D_ID3D11BlendState *pBlendState, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3D11_Release_BlendState_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pBlendState ;
		return ( ULONG )AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3D11_Release_BlendState( pBlendState ) ;
}








#ifndef DX_NON_ASYNCLOAD
extern int Direct3D11_Release_DepthStencilState_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return ( int )Direct3D11_Release_DepthStencilState_ASync(
				( D_ID3D11DepthStencilState * )Info->Data[ 0 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern ULONG Direct3D11_Release_DepthStencilState_ASync( D_ID3D11DepthStencilState *pDepthStencilState, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3D11_Release_DepthStencilState_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pDepthStencilState ;
		return ( ULONG )AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3D11_Release_DepthStencilState( pDepthStencilState ) ;
}








#ifndef DX_NON_ASYNCLOAD
extern int Direct3D11_Release_RasterizerState_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return ( int )Direct3D11_Release_RasterizerState_ASync(
				( D_ID3D11RasterizerState * )Info->Data[ 0 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern ULONG Direct3D11_Release_RasterizerState_ASync( D_ID3D11RasterizerState *pRasterizerState, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3D11_Release_RasterizerState_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pRasterizerState ;
		return ( ULONG )AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3D11_Release_RasterizerState( pRasterizerState ) ;
}








#ifndef DX_NON_ASYNCLOAD
extern int Direct3D11_Release_SamplerState_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return ( int )Direct3D11_Release_SamplerState_ASync(
				( D_ID3D11SamplerState * )Info->Data[ 0 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern ULONG Direct3D11_Release_SamplerState_ASync( D_ID3D11SamplerState *pSamplerState, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3D11_Release_SamplerState_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pSamplerState ;
		return ( ULONG )AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3D11_Release_SamplerState( pSamplerState ) ;
}










#ifndef DX_NON_ASYNCLOAD
extern int D3D11Device_CreateBuffer_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return D3D11Device_CreateBuffer_ASync(
				( const D_D3D11_BUFFER_DESC *      )Info->Data[ 0 ],
				( const D_D3D11_SUBRESOURCE_DATA * )Info->Data[ 1 ],
				( D_ID3D11Buffer **                )Info->Data[ 2 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern	HRESULT	D3D11Device_CreateBuffer_ASync( const D_D3D11_BUFFER_DESC *pDesc, const D_D3D11_SUBRESOURCE_DATA *pInitialData, D_ID3D11Buffer **ppBuffer, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = D3D11Device_CreateBuffer_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pDesc ;
		AInfo.Data[ 1 ] = ( DWORD_PTR )pInitialData ;
		AInfo.Data[ 2 ] = ( DWORD_PTR )ppBuffer ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return D3D11Device_CreateBuffer( pDesc, pInitialData, ppBuffer ) ;
}







#ifndef DX_NON_ASYNCLOAD
extern int D3D11Device_CreateTexture1D_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return D3D11Device_CreateTexture1D_ASync(
				( const D_D3D11_TEXTURE1D_DESC *   )Info->Data[ 0 ],
				( const D_D3D11_SUBRESOURCE_DATA * )Info->Data[ 1 ],
				( D_ID3D11Texture1D **             )Info->Data[ 2 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern	HRESULT D3D11Device_CreateTexture1D_ASync( const D_D3D11_TEXTURE1D_DESC *pDesc, const D_D3D11_SUBRESOURCE_DATA *pInitialData, D_ID3D11Texture1D **ppTexture1D, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = D3D11Device_CreateTexture1D_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pDesc ;
		AInfo.Data[ 1 ] = ( DWORD_PTR )pInitialData ;
		AInfo.Data[ 2 ] = ( DWORD_PTR )ppTexture1D ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return D3D11Device_CreateTexture1D( pDesc, pInitialData, ppTexture1D ) ;
}






#ifndef DX_NON_ASYNCLOAD
extern int D3D11Device_CreateTexture2D_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return D3D11Device_CreateTexture2D_ASync(
				( const D_D3D11_TEXTURE2D_DESC *   )Info->Data[ 0 ],
				( const D_D3D11_SUBRESOURCE_DATA * )Info->Data[ 1 ],
				( D_ID3D11Texture2D **             )Info->Data[ 2 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern	HRESULT D3D11Device_CreateTexture2D_ASync( const D_D3D11_TEXTURE2D_DESC *pDesc, const D_D3D11_SUBRESOURCE_DATA *pInitialData, D_ID3D11Texture2D **ppTexture2D, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = D3D11Device_CreateTexture2D_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pDesc ;
		AInfo.Data[ 1 ] = ( DWORD_PTR )pInitialData ;
		AInfo.Data[ 2 ] = ( DWORD_PTR )ppTexture2D ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return D3D11Device_CreateTexture2D( pDesc, pInitialData, ppTexture2D ) ;
}









#ifndef DX_NON_ASYNCLOAD
extern int D3D11Device_CreateTexture3D_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return D3D11Device_CreateTexture3D_ASync(
				( const D_D3D11_TEXTURE3D_DESC *   )Info->Data[ 0 ],
				( const D_D3D11_SUBRESOURCE_DATA * )Info->Data[ 1 ],
				( D_ID3D11Texture3D **             )Info->Data[ 2 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern	HRESULT D3D11Device_CreateTexture3D_ASync( const D_D3D11_TEXTURE3D_DESC *pDesc, const D_D3D11_SUBRESOURCE_DATA *pInitialData, D_ID3D11Texture3D **ppTexture3D, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = D3D11Device_CreateTexture3D_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pDesc ;
		AInfo.Data[ 1 ] = ( DWORD_PTR )pInitialData ;
		AInfo.Data[ 2 ] = ( DWORD_PTR )ppTexture3D ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return D3D11Device_CreateTexture3D( pDesc, pInitialData, ppTexture3D ) ;
}








#ifndef DX_NON_ASYNCLOAD
extern int D3D11Device_CreateShaderResourceView_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return D3D11Device_CreateShaderResourceView_ASync(
				( D_ID3D11Resource *                        )Info->Data[ 0 ],
				( const D_D3D11_SHADER_RESOURCE_VIEW_DESC * )Info->Data[ 1 ],
				( D_ID3D11ShaderResourceView **             )Info->Data[ 2 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern	HRESULT D3D11Device_CreateShaderResourceView_ASync( D_ID3D11Resource *pResource, const D_D3D11_SHADER_RESOURCE_VIEW_DESC *pDesc, D_ID3D11ShaderResourceView **ppSRView, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = D3D11Device_CreateShaderResourceView_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pResource ;
		AInfo.Data[ 1 ] = ( DWORD_PTR )pDesc ;
		AInfo.Data[ 2 ] = ( DWORD_PTR )ppSRView ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return D3D11Device_CreateShaderResourceView( pResource, pDesc, ppSRView ) ;
}









#ifndef DX_NON_ASYNCLOAD
extern int D3D11Device_CreateRenderTargetView_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return D3D11Device_CreateRenderTargetView_ASync(
				( D_ID3D11Resource *                      )Info->Data[ 0 ],
				( const D_D3D11_RENDER_TARGET_VIEW_DESC * )Info->Data[ 1 ],
				( D_ID3D11RenderTargetView **             )Info->Data[ 2 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern	HRESULT D3D11Device_CreateRenderTargetView_ASync( D_ID3D11Resource *pResource, const D_D3D11_RENDER_TARGET_VIEW_DESC *pDesc, D_ID3D11RenderTargetView **ppRTView, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = D3D11Device_CreateRenderTargetView_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pResource ;
		AInfo.Data[ 1 ] = ( DWORD_PTR )pDesc ;
		AInfo.Data[ 2 ] = ( DWORD_PTR )ppRTView ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return D3D11Device_CreateRenderTargetView( pResource, pDesc, ppRTView ) ;
}









#ifndef DX_NON_ASYNCLOAD
extern int D3D11Device_CreateDepthStencilView_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return D3D11Device_CreateDepthStencilView_ASync(
				( D_ID3D11Resource *                      )Info->Data[ 0 ],
				( const D_D3D11_DEPTH_STENCIL_VIEW_DESC * )Info->Data[ 1 ],
				( D_ID3D11DepthStencilView **             )Info->Data[ 2 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern	HRESULT	D3D11Device_CreateDepthStencilView_ASync( D_ID3D11Resource *pResource, const D_D3D11_DEPTH_STENCIL_VIEW_DESC *pDesc, D_ID3D11DepthStencilView **ppDepthStencilView, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = D3D11Device_CreateDepthStencilView_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pResource ;
		AInfo.Data[ 1 ] = ( DWORD_PTR )pDesc ;
		AInfo.Data[ 2 ] = ( DWORD_PTR )ppDepthStencilView ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return D3D11Device_CreateDepthStencilView( pResource, pDesc, ppDepthStencilView ) ;
}







#ifndef DX_NON_ASYNCLOAD
extern int D3D11Device_CreateVertexShader_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return D3D11Device_CreateVertexShader_ASync(
				( const void *            )Info->Data[ 0 ],
				( SIZE_T                  )Info->Data[ 1 ],
				( D_ID3D11ClassLinkage *  )Info->Data[ 2 ],
				( D_ID3D11VertexShader ** )Info->Data[ 3 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern	HRESULT	D3D11Device_CreateVertexShader_ASync( const void *pShaderBytecode, SIZE_T BytecodeLength, D_ID3D11ClassLinkage *pClassLinkage, D_ID3D11VertexShader **ppVertexShader, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = D3D11Device_CreateVertexShader_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pShaderBytecode ;
		AInfo.Data[ 1 ] = ( DWORD_PTR )BytecodeLength ;
		AInfo.Data[ 2 ] = ( DWORD_PTR )pClassLinkage ;
		AInfo.Data[ 3 ] = ( DWORD_PTR )ppVertexShader ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return D3D11Device_CreateVertexShader( pShaderBytecode, BytecodeLength, pClassLinkage, ppVertexShader ) ;
}









#ifndef DX_NON_ASYNCLOAD
extern int D3D11Device_CreatePixelShader_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return D3D11Device_CreatePixelShader_ASync(
				( const void *           )Info->Data[ 0 ],
				( SIZE_T                 )Info->Data[ 1 ],
				( D_ID3D11ClassLinkage * )Info->Data[ 2 ],
				( D_ID3D11PixelShader ** )Info->Data[ 3 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern	HRESULT	D3D11Device_CreatePixelShader_ASync( const void *pShaderBytecode, SIZE_T BytecodeLength, D_ID3D11ClassLinkage *pClassLinkage, D_ID3D11PixelShader **ppPixelShader, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = D3D11Device_CreatePixelShader_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pShaderBytecode ;
		AInfo.Data[ 1 ] = ( DWORD_PTR )BytecodeLength ;
		AInfo.Data[ 2 ] = ( DWORD_PTR )pClassLinkage ;
		AInfo.Data[ 3 ] = ( DWORD_PTR )ppPixelShader ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return D3D11Device_CreatePixelShader( pShaderBytecode, BytecodeLength, pClassLinkage, ppPixelShader ) ;
}










#ifndef DX_NON_ASYNCLOAD
extern int D3D11Device_CheckMultisampleQualityLevels_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return D3D11Device_CheckMultisampleQualityLevels_ASync(
				( D_DXGI_FORMAT )Info->Data[ 0 ],
				( UINT          )Info->Data[ 1 ],
				( UINT *        )Info->Data[ 2 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern long D3D11Device_CheckMultisampleQualityLevels_ASync( D_DXGI_FORMAT Format, UINT SampleCount, UINT *pNumQualityLevels, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = D3D11Device_CheckMultisampleQualityLevels_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )Format ;
		AInfo.Data[ 1 ] = ( DWORD_PTR )SampleCount ;
		AInfo.Data[ 2 ] = ( DWORD_PTR )pNumQualityLevels ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return D3D11Device_CheckMultisampleQualityLevels( Format, SampleCount, pNumQualityLevels ) ;
}






#ifndef DX_NON_ASYNCLOAD
extern int D3D11Device_CheckMultiSampleParam_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return D3D11Device_CheckMultiSampleParam_ASync(
				( D_DXGI_FORMAT )Info->Data[ 0 ],
				( UINT *        )Info->Data[ 1 ],
				( UINT *        )Info->Data[ 2 ],
				( int           )Info->Data[ 3 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern	int D3D11Device_CheckMultiSampleParam_ASync( D_DXGI_FORMAT Format, UINT *Samples, UINT *Quality, int SamplesFailedBreak, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = D3D11Device_CheckMultiSampleParam_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )Format ;
		AInfo.Data[ 1 ] = ( DWORD_PTR )Samples ;
		AInfo.Data[ 2 ] = ( DWORD_PTR )Quality ;
		AInfo.Data[ 3 ] = ( DWORD_PTR )SamplesFailedBreak ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return D3D11Device_CheckMultiSampleParam( Format, Samples, Quality, SamplesFailedBreak ) ;
}






#ifndef DX_NON_ASYNCLOAD
extern int D3D11DeviceContext_Map_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return D3D11DeviceContext_Map_ASync(
				( D_ID3D11Resource *           )Info->Data[ 0 ],
				( UINT                         )Info->Data[ 1 ],
				( D_D3D11_MAP                  )Info->Data[ 2 ],
				( UINT                         )Info->Data[ 3 ],
				( D_D3D11_MAPPED_SUBRESOURCE * )Info->Data[ 4 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern	HRESULT	D3D11DeviceContext_Map_ASync( D_ID3D11Resource *pResource, UINT Subresource, D_D3D11_MAP MapType, UINT MapFlags, D_D3D11_MAPPED_SUBRESOURCE *pMappedResource, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = D3D11DeviceContext_Map_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pResource ;
		AInfo.Data[ 1 ] = ( DWORD_PTR )Subresource ;
		AInfo.Data[ 2 ] = ( DWORD_PTR )MapType ;
		AInfo.Data[ 3 ] = ( DWORD_PTR )MapFlags ;
		AInfo.Data[ 4 ] = ( DWORD_PTR )pMappedResource ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return D3D11DeviceContext_Map( pResource, Subresource, MapType, MapFlags, pMappedResource ) ;
}






#ifndef DX_NON_ASYNCLOAD
extern int D3D11DeviceContext_Unmap_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	D3D11DeviceContext_Unmap_ASync(
				( D_ID3D11Resource *           )Info->Data[ 0 ],
				( UINT                         )Info->Data[ 1 ],
				FALSE ) ;

	return 0 ;
}
#endif // DX_NON_ASYNCLOAD

extern	void	D3D11DeviceContext_Unmap_ASync( D_ID3D11Resource *pResource, UINT Subresource, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = D3D11DeviceContext_Unmap_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pResource ;
		AInfo.Data[ 1 ] = ( DWORD_PTR )Subresource ;
		AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
		return ;
	}
#endif // DX_NON_ASYNCLOAD

	D3D11DeviceContext_Unmap( pResource, Subresource ) ;
}








#ifndef DX_NON_ASYNCLOAD
extern int D3D11DeviceContext_CopyResource_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	D3D11DeviceContext_CopyResource_ASync(
				( D_ID3D11Resource * )Info->Data[ 0 ],
				( D_ID3D11Resource * )Info->Data[ 1 ],
				FALSE ) ;

	return 0 ;
}
#endif // DX_NON_ASYNCLOAD

extern	void	D3D11DeviceContext_CopyResource_ASync( D_ID3D11Resource *pDstResource, D_ID3D11Resource *pSrcResource, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = D3D11DeviceContext_CopyResource_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pDstResource ;
		AInfo.Data[ 1 ] = ( DWORD_PTR )pSrcResource ;
		AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
		return ;
	}
#endif // DX_NON_ASYNCLOAD

	D3D11DeviceContext_CopyResource( pDstResource, pSrcResource ) ;
}







#ifndef DX_NON_ASYNCLOAD
extern int D3D11DeviceContext_UpdateSubresource_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	D3D11DeviceContext_UpdateSubresource_ASync(
				( D_ID3D11Resource *  )Info->Data[ 0 ],
				( UINT                )Info->Data[ 1 ],
				( const D_D3D11_BOX * )Info->Data[ 2 ],
				( const void *        )Info->Data[ 3 ],
				( UINT                )Info->Data[ 4 ],
				( UINT                )Info->Data[ 5 ],
				FALSE ) ;

	return 0 ;
}
#endif // DX_NON_ASYNCLOAD

extern	void	D3D11DeviceContext_UpdateSubresource_ASync( D_ID3D11Resource *pDstResource, UINT DstSubresource, const D_D3D11_BOX *pDstBox, const void *pSrcData, UINT SrcRowPitch, UINT SrcDepthPitch, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = D3D11DeviceContext_UpdateSubresource_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pDstResource ;
		AInfo.Data[ 1 ] = ( DWORD_PTR )DstSubresource ;
		AInfo.Data[ 2 ] = ( DWORD_PTR )pDstBox ;
		AInfo.Data[ 3 ] = ( DWORD_PTR )pSrcData ;
		AInfo.Data[ 4 ] = ( DWORD_PTR )SrcRowPitch ;
		AInfo.Data[ 5 ] = ( DWORD_PTR )SrcDepthPitch ;
		AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
		return ;
	}
#endif // DX_NON_ASYNCLOAD

	D3D11DeviceContext_UpdateSubresource( pDstResource, DstSubresource, pDstBox, pSrcData, SrcRowPitch, SrcDepthPitch ) ;
}








#ifndef DX_NON_ASYNCLOAD
extern int D3D11DeviceContext_ClearRenderTargetView_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	FLOAT ColorRGBA[ 4 ] ;

	ColorRGBA[ 0 ] = ( ( FLOAT * )Info->Data[ 1 ] )[ 0 ] ;
	ColorRGBA[ 1 ] = ( ( FLOAT * )Info->Data[ 1 ] )[ 1 ] ;
	ColorRGBA[ 2 ] = ( ( FLOAT * )Info->Data[ 1 ] )[ 2 ] ;
	ColorRGBA[ 3 ] = ( ( FLOAT * )Info->Data[ 1 ] )[ 3 ] ;

	D3D11DeviceContext_ClearRenderTargetView_ASync(
				( D_ID3D11RenderTargetView *  )Info->Data[ 0 ],
				ColorRGBA,
				FALSE ) ;

	return 0 ;
}
#endif // DX_NON_ASYNCLOAD

extern	void	D3D11DeviceContext_ClearRenderTargetView_ASync( D_ID3D11RenderTargetView *pRenderTargetView, const FLOAT ColorRGBA[ 4 ], int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = D3D11DeviceContext_ClearRenderTargetView_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pRenderTargetView ;
		AInfo.Data[ 1 ] = ( DWORD_PTR )ColorRGBA ;
		AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
		return ;
	}
#endif // DX_NON_ASYNCLOAD

	D3D11DeviceContext_ClearRenderTargetView( pRenderTargetView, ColorRGBA ) ;
}








#ifndef DX_NON_ASYNCLOAD
extern int D3D11DeviceContext_ClearDepthStencilView_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	D3D11DeviceContext_ClearDepthStencilView_ASync(
				( D_ID3D11DepthStencilView * )Info->Data[ 0 ],
				( UINT        )Info->Data[ 1 ],
				*( ( FLOAT *  )Info->Data[ 2 ] ),
				( D_UINT8     )Info->Data[ 3 ],
				FALSE ) ;

	return 0 ;
}
#endif // DX_NON_ASYNCLOAD

extern	void	D3D11DeviceContext_ClearDepthStencilView_ASync( D_ID3D11DepthStencilView *pDepthStencilView, UINT ClearFlags, FLOAT Depth, D_UINT8 Stencil, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = D3D11DeviceContext_ClearDepthStencilView_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pDepthStencilView ;
		AInfo.Data[ 1 ] = ( DWORD_PTR )ClearFlags ;
		AInfo.Data[ 2 ] = ( DWORD_PTR )&Depth ;
		AInfo.Data[ 3 ] = ( DWORD_PTR )Stencil ;
		AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
		return ;
	}
#endif // DX_NON_ASYNCLOAD

	D3D11DeviceContext_ClearDepthStencilView( pDepthStencilView, ClearFlags, Depth, Stencil ) ;
}














extern	ULONG Direct3D11_ObjectRelease( void *pObject )
{
	return ( ( D_IUnknown * )pObject )->Release() ;
}

extern	ULONG	Direct3D11_Release_Buffer( D_ID3D11Buffer *pBuffer )
{
	GAPIWin.D3D11BufferNum--;
	return pBuffer->Release() ;
}

extern	ULONG	Direct3D11_Release_Texture1D( D_ID3D11Texture1D *pTexture1D )
{
	GAPIWin.D3D11Texture1DNum--;
	return pTexture1D->Release() ;
}

extern	ULONG	Direct3D11_Release_Texture2D( D_ID3D11Texture2D *pTexture2D )
{
	GAPIWin.D3D11Texture2DNum--;
	return pTexture2D->Release() ;
}

extern	ULONG	Direct3D11_Release_Texture3D( D_ID3D11Texture3D *pTexture3D )
{
	GAPIWin.D3D11Texture3DNum--;
	return pTexture3D->Release() ;
}

extern	ULONG	Direct3D11_Release_ShaderResourceView( D_ID3D11ShaderResourceView *pSRView )
{
	GAPIWin.D3D11ShaderResourceViewNum--;
	return pSRView->Release() ;
}

extern	ULONG	Direct3D11_Release_RenderTargetView( D_ID3D11RenderTargetView *pRTView )
{
	GAPIWin.D3D11RenderTargetViewNum--;
	return pRTView->Release() ;
}

extern	ULONG	Direct3D11_Release_DepthStencilView( D_ID3D11DepthStencilView *pDepthStencilView )
{
	GAPIWin.D3D11DepthStencilViewNum--;
	return pDepthStencilView->Release() ;
}

extern	ULONG	Direct3D11_Release_InputLayout( D_ID3D11InputLayout *pInputLayout )
{
	GAPIWin.D3D11InputLayoutNum--;
	return pInputLayout->Release() ;
}

extern	ULONG	Direct3D11_Release_VertexShader( D_ID3D11VertexShader *pVertexShader )
{
	GAPIWin.D3D11VertexShaderNum--;
	return pVertexShader->Release() ;
}

extern	ULONG	Direct3D11_Release_PixelShader( D_ID3D11PixelShader *pPixelShader )
{
	GAPIWin.D3D11PixelShaderNum--;
	return pPixelShader->Release() ;
}

extern	ULONG	Direct3D11_Release_BlendState( D_ID3D11BlendState *pBlendState )
{
	GAPIWin.D3D11BlendStateNum--;
	return pBlendState->Release() ;
}

extern	ULONG	Direct3D11_Release_DepthStencilState( D_ID3D11DepthStencilState *pDepthStencilState )
{
	GAPIWin.D3D11DepthStencilStateNum--;
	return pDepthStencilState->Release() ;
}

extern	ULONG	Direct3D11_Release_RasterizerState( D_ID3D11RasterizerState *pRasterizerState )
{
	GAPIWin.D3D11RasterizerStateNum--;
	return pRasterizerState->Release() ;
}

extern	ULONG	Direct3D11_Release_SamplerState( D_ID3D11SamplerState *pSamplerState )
{
	GAPIWin.D3D11SamplerStateNum--;
	return pSamplerState->Release() ;
}

extern void Direct3D11_DumpObject( void )
{
	int TotalNum ;

	DXST_LOGFILE_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x33\x00\x44\x00\x31\x00\x31\x00\x20\x00\x6e\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x70\x65\x92\x30\xfa\x51\x9b\x52\x0a\x00\x00"/*@ L"Direct3D11 のオブジェクト数を出力\n" @*/ ) ;

	TotalNum = 0 ;

	if( GAPIWin.D3D11BufferNum > 0 )
	{
		TotalNum += GAPIWin.D3D11BufferNum ;
		DXST_LOGFILEFMT_ADDW(( L"ID3D11Buffer : %d", GAPIWin.D3D11BufferNum )) ;
	}

	if( GAPIWin.D3D11Texture1DNum > 0 )
	{
		TotalNum += GAPIWin.D3D11Texture1DNum ;
		DXST_LOGFILEFMT_ADDW(( L"ID3D11Texture1D : %d", GAPIWin.D3D11Texture1DNum )) ;
	}

	if( GAPIWin.D3D11Texture2DNum > 0 )
	{
		TotalNum += GAPIWin.D3D11Texture2DNum ;
		DXST_LOGFILEFMT_ADDW(( L"ID3D11Texture2D : %d", GAPIWin.D3D11Texture2DNum )) ;
	}

	if( GAPIWin.D3D11Texture3DNum > 0 )
	{
		TotalNum += GAPIWin.D3D11Texture3DNum ;
		DXST_LOGFILEFMT_ADDW(( L"ID3D11Texture3D : %d", GAPIWin.D3D11Texture3DNum )) ;
	}

	if( GAPIWin.D3D11ShaderResourceViewNum > 0 )
	{
		TotalNum += GAPIWin.D3D11ShaderResourceViewNum ;
		DXST_LOGFILEFMT_ADDW(( L"ID3D11ShaderResourceView : %d", GAPIWin.D3D11ShaderResourceViewNum )) ;
	}

	if( GAPIWin.D3D11RenderTargetViewNum > 0 )
	{
		TotalNum += GAPIWin.D3D11RenderTargetViewNum ;
		DXST_LOGFILEFMT_ADDW(( L"ID3D11RenderTargetView : %d", GAPIWin.D3D11RenderTargetViewNum )) ;
	}

	if( GAPIWin.D3D11DepthStencilViewNum > 0 )
	{
		TotalNum += GAPIWin.D3D11DepthStencilViewNum ;
		DXST_LOGFILEFMT_ADDW(( L"ID3D11DepthStencilView : %d", GAPIWin.D3D11DepthStencilViewNum )) ;
	}

	if( GAPIWin.D3D11InputLayoutNum > 0 )
	{
		TotalNum += GAPIWin.D3D11InputLayoutNum ;
		DXST_LOGFILEFMT_ADDW(( L"ID3D11InputLayout : %d", GAPIWin.D3D11InputLayoutNum )) ;
	}

	if( GAPIWin.D3D11VertexShaderNum > 0 )
	{
		TotalNum += GAPIWin.D3D11VertexShaderNum ;
		DXST_LOGFILEFMT_ADDW(( L"ID3D11VertexShader : %d", GAPIWin.D3D11VertexShaderNum )) ;
	}

	if( GAPIWin.D3D11PixelShaderNum > 0 )
	{
		TotalNum += GAPIWin.D3D11PixelShaderNum ;
		DXST_LOGFILEFMT_ADDW(( L"ID3D11PixelShader : %d", GAPIWin.D3D11PixelShaderNum )) ;
	}

	if( GAPIWin.D3D11BlendStateNum > 0 )
	{
		TotalNum += GAPIWin.D3D11BlendStateNum ;
		DXST_LOGFILEFMT_ADDW(( L"ID3D11BlendState : %d", GAPIWin.D3D11BlendStateNum )) ;
	}

	if( GAPIWin.D3D11DepthStencilStateNum > 0 )
	{
		TotalNum += GAPIWin.D3D11DepthStencilStateNum ;
		DXST_LOGFILEFMT_ADDW(( L"ID3D11DepthStencilState : %d", GAPIWin.D3D11DepthStencilStateNum )) ;
	}

	if( GAPIWin.D3D11RasterizerStateNum > 0 )
	{
		TotalNum += GAPIWin.D3D11RasterizerStateNum ;
		DXST_LOGFILEFMT_ADDW(( L"ID3D11RasterizerState : %d", GAPIWin.D3D11RasterizerStateNum )) ;
	}

	if( GAPIWin.D3D11SamplerStateNum > 0 )
	{
		TotalNum += GAPIWin.D3D11SamplerStateNum ;
		DXST_LOGFILEFMT_ADDW(( L"ID3D11SamplerState : %d", GAPIWin.D3D11SamplerStateNum )) ;
	}

	DXST_LOGFILEFMT_ADDUTF16LE(( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x33\x00\x44\x00\x31\x00\x31\x00\x20\x00\x6e\x30\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x08\x54\x08\x8a\x70\x65\x20\x00\x3a\x00\x20\x00\x25\x00\x64\x00\x00"/*@ L"Direct3D11 のオブジェクト合計数 : %d" @*/, TotalNum )) ;
}









extern int DXGI_LoadDLL( void )
{
	DXST_LOGFILE_ADDUTF16LE( "\x64\x00\x78\x00\x67\x00\x69\x00\x2e\x00\x64\x00\x6c\x00\x6c\x00\x20\x00\x6e\x30\xad\x8a\x7f\x30\xbc\x8f\x7f\x30\x2e\x00\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"dxgi.dll の読み込み.... " @*/ ) ;

	// dxgi.dll の読み込み
	GAPIWin.DXGIDLL = LoadLibraryW( L"dxgi.dll" ) ;
	if( GAPIWin.DXGIDLL == NULL )
	{
		FreeLibrary( GAPIWin.Direct3D11DLL ) ;
		GAPIWin.Direct3D11DLL = NULL ;
		return DXST_LOGFILE_ADDUTF16LE( "\x31\x59\x57\x65\x0a\x00\x00"/*@ L"失敗\n" @*/ ) ;
	}

	DXST_LOGFILE_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"成功\n" @*/ ) ;

	// 正常終了
	return 0 ;
}

extern int DXGI_FreeDLL( void )
{
	// dxgi.dll の解放
	if( GAPIWin.DXGIDLL )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x64\x00\x78\x00\x67\x00\x69\x00\x2e\x00\x64\x00\x6c\x00\x6c\x00\x20\x00\x6e\x30\xe3\x89\x3e\x65\x20\x00\x31\x00\x0a\x00\x00"/*@ L"dxgi.dll の解放 1\n" @*/ ) ;
		FreeLibrary( GAPIWin.DXGIDLL ) ;
		GAPIWin.DXGIDLL = NULL ;
	}

	// 正常終了
	return 0 ;
}

extern int Direct3D11_LoadDLL( void )
{
	DXST_LOGFILE_ADDUTF16LE( "\x64\x00\x33\x00\x64\x00\x31\x00\x31\x00\x2e\x00\x64\x00\x6c\x00\x6c\x00\x20\x00\x6e\x30\xad\x8a\x7f\x30\xbc\x8f\x7f\x30\x2e\x00\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"d3d11.dll の読み込み.... " @*/ ) ;

	// Direct3D11.DLL の読み込み
	GAPIWin.Direct3D11DLL = LoadLibraryW( L"d3d11.dll" ) ;
	if( GAPIWin.Direct3D11DLL == NULL )
	{
		return DXST_LOGFILE_ADDUTF16LE( "\x31\x59\x57\x65\x0a\x00\x00"/*@ L"失敗\n" @*/ ) ;
	}

	DXST_LOGFILE_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"成功\n" @*/ ) ;

	// 正常終了
	return 0 ;
}

extern int Direct3D11_FreeDLL( void )
{
	// d3d11.dll の解放
	if( GAPIWin.Direct3D11DLL )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x64\x00\x33\x00\x64\x00\x31\x00\x31\x00\x2e\x00\x64\x00\x6c\x00\x6c\x00\x20\x00\x6e\x30\xe3\x89\x3e\x65\x20\x00\x31\x00\x0a\x00\x00"/*@ L"d3d11.dll の解放 1\n" @*/ ) ;
		FreeLibrary( GAPIWin.Direct3D11DLL ) ;
		GAPIWin.Direct3D11DLL = NULL ;
	}

	// 正常終了
	return 0 ;
}

extern int DXGIFactory_Create( void )
{
	LPD_CREATEDXGIFACTORY D_CreateDXGIFactoryFunction = NULL ;
	LPD_CREATEDXGIFACTORY D_CreateDXGIFactory1Function = NULL ;

	if( GAPIWin.DXGIFactoryObject != NULL )
	{
		DXGIFactory_Release() ;
	}

	DXST_LOGFILE_ADDUTF16LE( "\x41\x00\x50\x00\x49\x00\x20\x00\x43\x00\x72\x00\x65\x00\x61\x00\x74\x00\x65\x00\x44\x00\x58\x00\x47\x00\x49\x00\x46\x00\x61\x00\x63\x00\x74\x00\x6f\x00\x72\x00\x79\x00\x31\x00\x20\x00\x6e\x30\xa2\x30\xc9\x30\xec\x30\xb9\x30\x92\x30\xd6\x53\x97\x5f\x57\x30\x7e\x30\x59\x30\x2e\x00\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"API CreateDXGIFactory1 のアドレスを取得します.... " @*/ ) ;

	D_CreateDXGIFactory1Function = ( LPD_CREATEDXGIFACTORY )GetProcAddress( GAPIWin.DXGIDLL, "CreateDXGIFactory1" ) ;
	if( D_CreateDXGIFactory1Function == NULL )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x31\x59\x57\x65\x0a\x00\x00"/*@ L"失敗\n" @*/ ) ;
	}
	else
	{
		DXST_LOGFILE_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"成功\n" @*/ ) ;

		DXST_LOGFILE_ADDUTF16LE( "\x49\x00\x44\x00\x58\x00\x47\x00\x49\x00\x46\x00\x61\x00\x63\x00\x74\x00\x6f\x00\x72\x00\x79\x00\x31\x00\x20\x00\x92\x30\x5c\x4f\x10\x62\x57\x30\x7e\x30\x59\x30\x2e\x00\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"IDXGIFactory1 を作成します.... " @*/ ) ;

		// IDXGIFactory1 の作成
		if( FAILED( D_CreateDXGIFactory1Function( IID_IDXGIFACTORY1, ( void ** )&GAPIWin.DXGIFactory1Object ) ) )
		{
			DXST_LOGFILE_ADDUTF16LE( "\x31\x59\x57\x65\x0a\x00\x00"/*@ L"失敗\n" @*/ ) ;
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"成功\n" @*/ ) ;

			GAPIWin.DXGIFactoryObject = GAPIWin.DXGIFactory1Object ;
		}
	}

	if( GAPIWin.DXGIFactoryObject == NULL )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x41\x00\x50\x00\x49\x00\x20\x00\x43\x00\x72\x00\x65\x00\x61\x00\x74\x00\x65\x00\x44\x00\x58\x00\x47\x00\x49\x00\x46\x00\x61\x00\x63\x00\x74\x00\x6f\x00\x72\x00\x79\x00\x20\x00\x6e\x30\xa2\x30\xc9\x30\xec\x30\xb9\x30\x92\x30\xd6\x53\x97\x5f\x57\x30\x7e\x30\x59\x30\x2e\x00\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"API CreateDXGIFactory のアドレスを取得します.... " @*/ ) ;

		D_CreateDXGIFactoryFunction  = ( LPD_CREATEDXGIFACTORY )GetProcAddress( GAPIWin.DXGIDLL, "CreateDXGIFactory" ) ;
		if( D_CreateDXGIFactoryFunction == NULL )
		{
			DXST_LOGFILE_ADDUTF16LE( "\x31\x59\x57\x65\x0a\x00\x00"/*@ L"失敗\n" @*/ ) ;

			return -1 ;
		}

		DXST_LOGFILE_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"成功\n" @*/ ) ;

		DXST_LOGFILE_ADDUTF16LE( "\x49\x00\x44\x00\x58\x00\x47\x00\x49\x00\x46\x00\x61\x00\x63\x00\x74\x00\x6f\x00\x72\x00\x79\x00\x20\x00\x92\x30\x5c\x4f\x10\x62\x57\x30\x7e\x30\x59\x30\x2e\x00\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"IDXGIFactory を作成します.... " @*/ ) ;

		// IDXGIFactory の作成
		if( FAILED( D_CreateDXGIFactoryFunction( IID_IDXGIFACTORY, ( void ** )&GAPIWin.DXGIFactoryObject ) ) )
		{
			DXST_LOGFILE_ADDUTF16LE( "\x31\x59\x57\x65\x0a\x00\x00"/*@ L"失敗\n" @*/ ) ;
			return -1 ;
		}

		DXST_LOGFILE_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"成功\n" @*/ ) ;
	}

	// 正常終了
	return 0 ;
}

extern int DXGIFactory_Release( void )
{
	if( GAPIWin.DXGIFactoryObject != NULL )
	{
		Direct3D11_ObjectRelease( GAPIWin.DXGIFactoryObject ) ;
		GAPIWin.DXGIFactoryObject = NULL ;
		GAPIWin.DXGIFactory1Object = NULL ;
	}

	// 正常修了
	return 0 ;
}

extern int DXGIFactory_IsValid( void )
{
	return GAPIWin.DXGIFactoryObject != NULL ? TRUE : FALSE ;
}

extern HRESULT DXGIFactory_EnumAdapters( UINT Adapter, D_IDXGIAdapter **ppAdapter )
{
	return GAPIWin.DXGIFactoryObject->EnumAdapters( Adapter, ppAdapter ) ;
}

extern HRESULT DXGIFactory_CreateSwapChain( D_DXGI_SWAP_CHAIN_DESC *pDesc, D_IDXGISwapChain **ppSwapChain )
{
	return GAPIWin.DXGIFactoryObject->CreateSwapChain( GAPIWin.D3D11DeviceObject, pDesc, ppSwapChain ) ;
}

extern	HRESULT	DXGIFactory_MakeWindowAssociation( HWND WindowHandle, UINT Flags )
{
	return GAPIWin.DXGIFactoryObject->MakeWindowAssociation( WindowHandle, Flags ) ;
}

extern	HRESULT DXGIAdapter_GetDesc( D_IDXGIAdapter *pAdapter, D_DXGI_ADAPTER_DESC *pDesc )
{
	return pAdapter->GetDesc( pDesc ) ;
}

extern	HRESULT	DXGIAdapter_EnumOutputs( D_IDXGIAdapter * pAdapter, UINT Output, D_IDXGIOutput **ppOutput )
{
	return pAdapter->EnumOutputs( Output, ppOutput ) ;
}

extern	HRESULT	DXGIOutput_GetDesc( D_IDXGIOutput *pOutput, D_DXGI_OUTPUT_DESC *pDesc )
{
	return pOutput->GetDesc( pDesc ) ;
}

extern	HRESULT DXGIOutput_WaitForVBlank( D_IDXGIOutput *pOutput )
{
	return pOutput->WaitForVBlank() ;
}

extern int D3D11_CreateDevice( void )
{
	LPD_D3D11CREATEDEVICE               D_D3D11CreateDeviceFunction ;
	HRESULT								Result ;

	if( GAPIWin.Direct3D11DLL == NULL )
	{
		return -1 ;
	}

	// Aero の有効・無効を設定する
	if( NS_GetWindowModeFlag() == TRUE )
	{
		SetEnableAero( GRAWIN.Setting.DisableAeroFlag == 2 ? FALSE : TRUE ) ;
	}


	DXST_LOGFILE_ADDUTF16LE( "\x41\x00\x50\x00\x49\x00\x20\x00\x44\x00\x33\x00\x44\x00\x31\x00\x31\x00\x43\x00\x72\x00\x65\x00\x61\x00\x74\x00\x65\x00\x44\x00\x65\x00\x76\x00\x69\x00\x63\x00\x65\x00\x20\x00\x6e\x30\xa2\x30\xc9\x30\xec\x30\xb9\x30\x92\x30\xd6\x53\x97\x5f\x57\x30\x7e\x30\x59\x30\x2e\x00\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"API D3D11CreateDevice のアドレスを取得します.... " @*/ ) ;

	D_D3D11CreateDeviceFunction = ( LPD_D3D11CREATEDEVICE )GetProcAddress( GAPIWin.Direct3D11DLL, "D3D11CreateDevice" ) ;
	if( D_D3D11CreateDeviceFunction == NULL )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x31\x59\x57\x65\x0a\x00\x00"/*@ L"失敗\n" @*/ ) ;
		return -1 ;
	}

	DXST_LOGFILE_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"成功\n" @*/ ) ;


	DXST_LOGFILE_ADDUTF16LE( "\x49\x00\x44\x00\x58\x00\x47\x00\x49\x00\x41\x00\x64\x00\x61\x00\x70\x00\x74\x00\x65\x00\x72\x00\x20\x00\x92\x30\xd6\x53\x97\x5f\x57\x30\x7e\x30\x59\x30\x2e\x00\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"IDXGIAdapter を取得します.... " @*/ ) ;

	// IDXGIAdapter の取得
	if( FAILED( GAPIWin.DXGIFactoryObject->EnumAdapters( 0, &GAPIWin.DXGIAdapterObject ) ) )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x31\x59\x57\x65\x0a\x00\x00"/*@ L"失敗\n" @*/ ) ;
		goto ERR ;
	}

	DXST_LOGFILE_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"成功\n" @*/ ) ;


	// D3D11Device の作成
	{
		static int FeatureLevelTable[] = 
		{
			D_D3D_FEATURE_LEVEL_11_1,
			D_D3D_FEATURE_LEVEL_11_0,
			D_D3D_FEATURE_LEVEL_10_1,
			D_D3D_FEATURE_LEVEL_10_0,
			D_D3D_FEATURE_LEVEL_9_3,
			D_D3D_FEATURE_LEVEL_9_2,
			D_D3D_FEATURE_LEVEL_9_1,
			-1
		} ;
		D_D3D_FEATURE_LEVEL	FeatureLevels[ DIRECT3D11_FEATURE_LEVEL_NUM ] ;
		UINT				FeatureLevelNum ;

#ifndef DX_NON_DIRECT3D9
		if( GD3D11.Setting.UseMinFeatureLevelDirect3D11 > 0 ||
			GD3D11.Setting.UseDirect3D11WARPTypeDriver )
		{
			int i ;

			if( GD3D11.Setting.UseMinFeatureLevelDirect3D11 == 0 &&
				GD3D11.Setting.UseDirect3D11WARPTypeDriver )
			{
				GD3D11.Setting.UseMinFeatureLevelDirect3D11 = DX_DIRECT3D_11_FEATURE_LEVEL_10_0 ;
			}

			DXST_LOGFILE_ADDUTF16LE( "\x07\x63\x9a\x5b\x6e\x30\x20\x00\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x33\x00\x44\x00\x20\x00\x31\x00\x31\x00\x20\x00\x46\x00\x65\x00\x61\x00\x74\x00\x75\x00\x72\x00\x65\x00\x4c\x00\x65\x00\x76\x00\x65\x00\x6c\x00\x20\x00\xe5\x4e\x0a\x4e\x92\x30\xfe\x5b\x61\x8c\x68\x30\x57\x30\x7e\x30\x59\x30\x0a\x00\x00"/*@ L"指定の Direct3D 11 FeatureLevel 以上を対象とします\n" @*/ ) ;

			FeatureLevelNum = 0 ;
			for( i = 0 ; FeatureLevelTable[ i ] != -1 ; i ++ )
			{
				if( GD3D11.Setting.UseMinFeatureLevelDirect3D11 <= FeatureLevelTable[ i ] )
				{
					FeatureLevels[ FeatureLevelNum ] = ( D_D3D_FEATURE_LEVEL )FeatureLevelTable[ i ] ;
					FeatureLevelNum ++ ;
				}
			}
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x33\x00\x44\x00\x20\x00\x31\x00\x31\x00\x20\x00\x46\x00\x65\x00\x61\x00\x74\x00\x75\x00\x72\x00\x65\x00\x4c\x00\x65\x00\x76\x00\x65\x00\x6c\x00\x20\x00\x31\x00\x31\x00\x5f\x00\x30\x00\x20\x00\xe5\x4e\x0a\x4e\x92\x30\xfe\x5b\x61\x8c\x68\x30\x57\x30\x7e\x30\x59\x30\x0a\x00\x00"/*@ L"Direct3D 11 FeatureLevel 11_0 以上を対象とします\n" @*/ ) ;
			FeatureLevels[ 0 ] = D_D3D_FEATURE_LEVEL_11_1 ;
			FeatureLevels[ 1 ] = D_D3D_FEATURE_LEVEL_11_0 ;
//			FeatureLevels[ 2 ] = D_D3D_FEATURE_LEVEL_10_0 ;
			FeatureLevelNum = 2 ;
		}
#else  // DX_NON_DIRECT3D9
		{
			int i ;

			DXST_LOGFILE_ADDUTF16LE( "\x68\x51\x66\x30\x6e\x30\x20\x00\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x33\x00\x44\x00\x20\x00\x31\x00\x31\x00\x20\x00\x46\x00\x65\x00\x61\x00\x74\x00\x75\x00\x72\x00\x65\x00\x4c\x00\x65\x00\x76\x00\x65\x00\x6c\x00\x20\x00\x92\x30\xfe\x5b\x61\x8c\x68\x30\x57\x30\x7e\x30\x59\x30\x0a\x00\x00"/*@ L"全ての Direct3D 11 FeatureLevel を対象とします\n" @*/ ) ;
			FeatureLevelNum = 0 ;
			for( i = 0 ; FeatureLevelTable[ i ] != -1 ; i ++ )
			{
				FeatureLevels[ FeatureLevelNum ] = ( D_D3D_FEATURE_LEVEL )FeatureLevelTable[ i ] ;
				FeatureLevelNum ++ ;
			}
		}
#endif // DX_NON_DIRECT3D9

		DXST_LOGFILE_ADDUTF16LE( "\x49\x00\x44\x00\x33\x00\x44\x00\x31\x00\x31\x00\x44\x00\x65\x00\x76\x00\x69\x00\x63\x00\x65\x00\x20\x00\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x92\x30\xd6\x53\x97\x5f\x57\x30\x7e\x30\x59\x30\x2e\x00\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"ID3D11Device オブジェクトを取得します.... " @*/ ) ;

		// D3D11Device の作成
		Result = D_D3D11CreateDeviceFunction(
//			NULL,									// D_IDXGIAdapter                *  pAdapter,
//			D_D3D_DRIVER_TYPE_HARDWARE,				// D_D3D_DRIVER_TYPE                DriverType,
			GD3D11.Setting.UseDirect3D11WARPTypeDriver ? NULL : GAPIWin.DXGIAdapterObject,						// D_IDXGIAdapter                *  pAdapter,
			GD3D11.Setting.UseDirect3D11WARPTypeDriver ? D_D3D_DRIVER_TYPE_WARP : D_D3D_DRIVER_TYPE_UNKNOWN,	// D_D3D_DRIVER_TYPE                DriverType,
			NULL,									// HMODULE                          Software,
			GRAWIN.Setting.UseMultiThread ? 0 : D_D3D11_CREATE_DEVICE_SINGLETHREADED,	// UINT                             Flags,
			FeatureLevels,							// CONST D_D3D_FEATURE_LEVEL     *  pFeatureLevels,
			FeatureLevelNum,						// UINT                             FeatureLevels,
			7 /* D3D11_SDK_VERSION */,				// UINT                             SDKVersion,
			&GAPIWin.D3D11DeviceObject,				// D_ID3D11Device                ** ppDevice,
			&GD3D11.Setting.FeatureLevel,			// D_D3D_FEATURE_LEVEL           *  pFeatureLevel,
			&GAPIWin.D3D11DeviceContext				// D_ID3D11DeviceContext         ** ppImmediateContext
		) ;
		if( Result != S_OK )
		{
			DXST_LOGFILE_ADDUTF16LE( "\x31\x59\x57\x65\x0a\x00\x00"/*@ L"失敗\n" @*/ ) ;
			goto ERR ;
		}

		DXST_LOGFILE_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"成功\n" @*/ ) ;
	}

	// DeferredContext の作成
	if( GRAWIN.Setting.UseMultiThread )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x49\x00\x44\x00\x33\x00\x44\x00\x31\x00\x31\x00\x44\x00\x65\x00\x76\x00\x69\x00\x63\x00\x65\x00\x43\x00\x6f\x00\x6e\x00\x74\x00\x65\x00\x78\x00\x74\x00\x28\x00\x20\x00\x44\x00\x65\x00\x66\x00\x65\x00\x72\x00\x72\x00\x65\x00\x64\x00\x43\x00\x6f\x00\x6e\x00\x74\x00\x65\x00\x78\x00\x74\x00\x20\x00\x29\x00\x20\x00\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x92\x30\xd6\x53\x97\x5f\x57\x30\x7e\x30\x59\x30\x2e\x00\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"ID3D11DeviceContext( DeferredContext ) オブジェクトを取得します.... " @*/ ) ;

		Result = GAPIWin.D3D11DeviceObject->CreateDeferredContext( 0, &GAPIWin.D3D11DeferredContext ) ;
		if( Result != S_OK )
		{
			DXST_LOGFILE_ADDUTF16LE( "\x31\x59\x57\x65\x0a\x00\x00"/*@ L"失敗\n" @*/ ) ;
			goto ERR ;
		}

		DXST_LOGFILE_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"成功\n" @*/ ) ;
	}

	DXST_LOGFILE_ADDUTF16LE( "\x49\x00\x44\x00\x58\x00\x47\x00\x49\x00\x44\x00\x65\x00\x76\x00\x69\x00\x63\x00\x65\x00\x31\x00\x20\x00\x92\x30\xd6\x53\x97\x5f\x57\x30\x7e\x30\x59\x30\x2e\x00\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"IDXGIDevice1 を取得します.... " @*/ ) ;

	// IDXGIDevice1 の取得
	if( FAILED( GAPIWin.D3D11DeviceObject->QueryInterface( IID_IDXGIDEVICE1, ( void ** )&GAPIWin.DXGIDevice1Object ) ) )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x31\x59\x57\x65\x0a\x00\x00"/*@ L"失敗\n" @*/ ) ;
	}

	DXST_LOGFILE_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"成功\n" @*/ ) ;


	// IDXGIDevice1 の取得に成功した場合は SetMaximumFrameLatency( 1 ) を実行する
	if( GAPIWin.DXGIDevice1Object != NULL )
	{
		GAPIWin.DXGIDevice1Object->SetMaximumFrameLatency( 1 ) ;
		DXST_LOGFILE_ADDUTF16LE( "\x49\x00\x44\x00\x58\x00\x47\x00\x49\x00\x44\x00\x65\x00\x76\x00\x69\x00\x63\x00\x65\x00\x31\x00\x2d\x00\x3e\x00\x53\x00\x65\x00\x74\x00\x4d\x00\x61\x00\x78\x00\x69\x00\x6d\x00\x75\x00\x6e\x00\x46\x00\x72\x00\x61\x00\x6d\x00\x65\x00\x4c\x00\x61\x00\x74\x00\x65\x00\x6e\x00\x63\x00\x79\x00\x28\x00\x20\x00\x31\x00\x20\x00\x29\x00\x3b\x00\x20\x00\x92\x30\x9f\x5b\x4c\x88\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"IDXGIDevice1->SetMaximunFrameLatency( 1 ); を実行しました\n" @*/ ) ;
	}

	// 終了
	return 0 ;

ERR :
	if( GAPIWin.D3D11DeferredContext != NULL )
	{
		GAPIWin.D3D11DeferredContext->Release() ;
		GAPIWin.D3D11DeferredContext = NULL ;
	}

	if( GAPIWin.D3D11DeviceContext != NULL )
	{
		GAPIWin.D3D11DeviceContext->Release() ;
		GAPIWin.D3D11DeviceContext = NULL ;
	}

	if( GAPIWin.D3D11DeviceObject != NULL )
	{
		GAPIWin.D3D11DeviceObject->Release() ;
		GAPIWin.D3D11DeviceObject = NULL ;
	}

	if( GAPIWin.DXGIAdapterObject != NULL )
	{
		GAPIWin.DXGIAdapterObject->Release() ;
		GAPIWin.DXGIAdapterObject = NULL ;
	}

	if( GAPIWin.DXGIDevice1Object != NULL )
	{
		GAPIWin.DXGIDevice1Object->Release() ;
		GAPIWin.DXGIDevice1Object = NULL ;
	}

	// エラー終了
	return -1 ;
}

extern HRESULT DXGISwapChain_Present( D_IDXGISwapChain *pSwapChain, UINT SyncInterval, UINT Flags )
{
	return pSwapChain->Present( SyncInterval, Flags ) ;
}

extern HRESULT DXGISwapChain_GetBuffer( D_IDXGISwapChain *pSwapChain, UINT Buffer, REFIID riid, void **ppSurface )
{
	HRESULT hr ;

	hr = pSwapChain->GetBuffer( Buffer, riid, ppSurface ) ;
	if( hr == S_OK )
	{
		GAPIWin.D3D11Texture2DNum++;
	}

	return hr ;
}

extern HRESULT DXGISwapChain_SetFullscreenState( D_IDXGISwapChain *pSwapChain, BOOL Fullscreen, D_IDXGIOutput *pTarget )
{
	return pSwapChain->SetFullscreenState( Fullscreen, pTarget ) ;
}

extern HRESULT DXGISwapChain_GetDesc( D_IDXGISwapChain *pSwapChain, D_DXGI_SWAP_CHAIN_DESC *pDesc )
{
	return pSwapChain->GetDesc( pDesc ) ;
}

extern HRESULT DXGISwapChain_ResizeBuffers( D_IDXGISwapChain *pSwapChain, UINT BufferCount, UINT Width, UINT Height, D_DXGI_FORMAT NewFormat, UINT SwapChainFlags )
{
	return pSwapChain->ResizeBuffers( BufferCount, Width, Height, NewFormat, SwapChainFlags ) ;
}

extern HRESULT DXGISwapChain_GetContainingOutput( D_IDXGISwapChain *pSwapChain, D_IDXGIOutput **ppOutput )
{
	return pSwapChain->GetContainingOutput( ppOutput ) ;
}











extern void *D3D11Device_Get( void )
{
	return GAPIWin.D3D11DeviceObject ;
}

extern int D3D11Device_Release( void )
{
	if( GAPIWin.D3D11DeferredContext != NULL )
	{
		GAPIWin.D3D11DeferredContext->ClearState() ;
		GAPIWin.D3D11DeferredContext->Flush() ;

		GAPIWin.D3D11DeferredContext->Release() ;
		GAPIWin.D3D11DeferredContext = NULL ;
	}

	if( GAPIWin.D3D11DeviceContext != NULL )
	{
		D3D11DeviceContext_ClearState() ;
		D3D11DeviceContext_Flush() ;

		GAPIWin.D3D11DeviceContext->Release() ;
		GAPIWin.D3D11DeviceContext = NULL ;
	}

	if( GAPIWin.DXGIDevice1Object != NULL )
	{
		GAPIWin.DXGIDevice1Object->Release() ;
		GAPIWin.DXGIDevice1Object = NULL ;
	}

	if( GAPIWin.D3D11DeviceObject != NULL )
	{
		GAPIWin.D3D11DeviceObject->Release() ;
		GAPIWin.D3D11DeviceObject = NULL ;
	}

	if( GAPIWin.DXGIAdapterObject != NULL )
	{
		GAPIWin.DXGIAdapterObject->Release() ;
		GAPIWin.DXGIAdapterObject = NULL ;
	}

	DXGIFactory_Release() ;

	return 0 ;
}

extern int D3D11Device_IsValid( void )
{
	return GAPIWin.D3D11DeviceObject != NULL ? 1 : 0 ;
}






















extern	HRESULT D3D11Device_CreateBuffer( const D_D3D11_BUFFER_DESC *pDesc, const D_D3D11_SUBRESOURCE_DATA *pInitialData, D_ID3D11Buffer **ppBuffer )
{
	HRESULT hr ;

	hr = GAPIWin.D3D11DeviceObject->CreateBuffer( pDesc, pInitialData, ppBuffer ) ;
	if( hr == S_OK )
	{
		GAPIWin.D3D11BufferNum ++ ;
	}

	return hr ;
}

extern	HRESULT D3D11Device_CreateTexture1D( const D_D3D11_TEXTURE1D_DESC *pDesc, const D_D3D11_SUBRESOURCE_DATA *pInitialData, D_ID3D11Texture1D **ppTexture1D )
{
	HRESULT hr ;

	hr = GAPIWin.D3D11DeviceObject->CreateTexture1D( pDesc, pInitialData, ppTexture1D ) ;
	if( hr == S_OK )
	{
		GAPIWin.D3D11Texture1DNum ++ ;
	}

	return hr ;
}

extern	HRESULT D3D11Device_CreateTexture2D( const D_D3D11_TEXTURE2D_DESC *pDesc, const D_D3D11_SUBRESOURCE_DATA *pInitialData, D_ID3D11Texture2D **ppTexture2D )
{
	HRESULT hr ;

	hr = GAPIWin.D3D11DeviceObject->CreateTexture2D( pDesc, pInitialData, ppTexture2D ) ;
	if( hr == S_OK )
	{
		GAPIWin.D3D11Texture2DNum ++ ;
	}

	return hr ;
}

extern	HRESULT D3D11Device_CreateTexture3D( const D_D3D11_TEXTURE3D_DESC *pDesc, const D_D3D11_SUBRESOURCE_DATA *pInitialData, D_ID3D11Texture3D **ppTexture3D )
{
	HRESULT hr ;
	hr = GAPIWin.D3D11DeviceObject->CreateTexture3D( pDesc, pInitialData, ppTexture3D ) ;
	if( hr == S_OK )
	{
		GAPIWin.D3D11Texture3DNum ++ ;
	}

	return hr ;
}

extern	HRESULT D3D11Device_CreateShaderResourceView( D_ID3D11Resource *pResource, const D_D3D11_SHADER_RESOURCE_VIEW_DESC *pDesc, D_ID3D11ShaderResourceView **ppSRView )
{
	HRESULT hr ;

	hr = GAPIWin.D3D11DeviceObject->CreateShaderResourceView( pResource, pDesc, ppSRView ) ;
	if( hr == S_OK )
	{
		GAPIWin.D3D11ShaderResourceViewNum ++ ;
	}

	return hr ;
}

extern	HRESULT D3D11Device_CreateRenderTargetView( D_ID3D11Resource *pResource, const D_D3D11_RENDER_TARGET_VIEW_DESC *pDesc, D_ID3D11RenderTargetView **ppRTView )
{
	HRESULT hr ;

	hr = GAPIWin.D3D11DeviceObject->CreateRenderTargetView( pResource, pDesc, ppRTView ) ;
	if( hr == S_OK )
	{
		GAPIWin.D3D11RenderTargetViewNum ++ ;
	}

	return hr ;
}

extern	HRESULT	D3D11Device_CreateDepthStencilView( D_ID3D11Resource *pResource, const D_D3D11_DEPTH_STENCIL_VIEW_DESC *pDesc, D_ID3D11DepthStencilView **ppDepthStencilView )
{
	HRESULT hr ;

	hr = GAPIWin.D3D11DeviceObject->CreateDepthStencilView( pResource, pDesc, ppDepthStencilView ) ;
	if( hr == S_OK )
	{
		GAPIWin.D3D11DepthStencilViewNum ++ ;
	}

	return hr ;
}

extern	HRESULT D3D11Device_CreateInputLayout( const D_D3D11_INPUT_ELEMENT_DESC *pInputElementDescs, UINT NumElements, const void *pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength, D_ID3D11InputLayout **ppInputLayout )
{
	HRESULT hr ;

	hr = GAPIWin.D3D11DeviceObject->CreateInputLayout( pInputElementDescs, NumElements, pShaderBytecodeWithInputSignature, BytecodeLength, ppInputLayout ) ;
	if( hr == S_OK )
	{
		GAPIWin.D3D11InputLayoutNum ++ ;
	}

	return hr ;
}

extern	HRESULT	D3D11Device_CreateVertexShader( const void *pShaderBytecode, SIZE_T BytecodeLength, D_ID3D11ClassLinkage *pClassLinkage, D_ID3D11VertexShader **ppVertexShader )
{
	HRESULT hr ;

	hr = GAPIWin.D3D11DeviceObject->CreateVertexShader( pShaderBytecode,  BytecodeLength, pClassLinkage, ppVertexShader ) ;
	if( hr == S_OK )
	{
		GAPIWin.D3D11VertexShaderNum ++ ;
	}

	return hr ;
}

extern	HRESULT	D3D11Device_CreatePixelShader( const void *pShaderBytecode, SIZE_T BytecodeLength, D_ID3D11ClassLinkage *pClassLinkage, D_ID3D11PixelShader **ppPixelShader )
{
	HRESULT hr ;

	hr = GAPIWin.D3D11DeviceObject->CreatePixelShader( pShaderBytecode,  BytecodeLength, pClassLinkage, ppPixelShader ) ;
	if( hr == S_OK )
	{
		GAPIWin.D3D11PixelShaderNum ++ ;
	}

	return hr ;
}

extern	HRESULT	D3D11Device_CreateBlendState( const D_D3D11_BLEND_DESC *pBlendStateDesc, D_ID3D11BlendState **ppBlendState )
{
	HRESULT hr ;

	hr = GAPIWin.D3D11DeviceObject->CreateBlendState( pBlendStateDesc, ppBlendState ) ;
	if( hr == S_OK )
	{
		GAPIWin.D3D11BlendStateNum ++ ;
	}

	return hr ;
}

extern	HRESULT	D3D11Device_CreateDepthStencilState( const D_D3D11_DEPTH_STENCIL_DESC *pDepthStencilDesc, D_ID3D11DepthStencilState **ppDepthStencilState )
{
	HRESULT hr ;

	hr = GAPIWin.D3D11DeviceObject->CreateDepthStencilState( pDepthStencilDesc, ppDepthStencilState ) ;
	if( hr == S_OK )
	{
		GAPIWin.D3D11DepthStencilStateNum ++ ;
	}

	return hr ;
}

extern	HRESULT	D3D11Device_CreateRasterizerState( const D_D3D11_RASTERIZER_DESC    *pRasterizerDesc,   D_ID3D11RasterizerState   **ppRasterizerState   )
{
	HRESULT hr ;

	hr = GAPIWin.D3D11DeviceObject->CreateRasterizerState( pRasterizerDesc, ppRasterizerState ) ;
	if( hr == S_OK )
	{
		GAPIWin.D3D11RasterizerStateNum ++ ;
	}

	return hr ;
}

extern	HRESULT	D3D11Device_CreateSamplerState( const D_D3D11_SAMPLER_DESC *pSamplerDesc, D_ID3D11SamplerState **ppSamplerState )
{
	HRESULT hr ;

	hr = GAPIWin.D3D11DeviceObject->CreateSamplerState( pSamplerDesc, ppSamplerState ) ;
	if( hr == S_OK )
	{
		GAPIWin.D3D11SamplerStateNum ++ ;
	}

	return hr ;
}

extern	HRESULT D3D11Device_CheckFormatSupport( D_DXGI_FORMAT Format, UINT *pFormatSupport )
{
	return GAPIWin.D3D11DeviceObject->CheckFormatSupport( Format, pFormatSupport ) ;
}

extern	HRESULT	D3D11Device_CheckMultisampleQualityLevels(	D_DXGI_FORMAT Format, UINT SampleCount, UINT *pNumQualityLevels )
{
	return GAPIWin.D3D11DeviceObject->CheckMultisampleQualityLevels( Format, SampleCount, pNumQualityLevels ) ;
}

// マルチサンプルレンダリングのサンプル数とクオリティをチェック
extern int D3D11Device_CheckMultiSampleParam( D_DXGI_FORMAT Format, UINT *Samples, UINT *Quality, int SamplesFailedBreak )
{
	UINT ColorBufferQuality, ZBufferQuality, MaxQuality ;

	// Samples が 1 の場合はアンチエイリアスなしなので Quality も 0 にして返す
	if( *Samples == 1 )
	{
		*Quality = 0 ;
		return 0 ;
	}

	// カラーバッファで使用できるマルチサンプルタイプとその際のクオリティを取得する
	ColorBufferQuality = 0 ;
	for(;;)
	{
		if( D3D11Device_CheckMultisampleQualityLevels( Format, *Samples, &ColorBufferQuality ) == S_OK && ColorBufferQuality > 0 )
			break ;

		if( SamplesFailedBreak )
		{
			*Quality = 0 ;
			return -1 ;
		}

		*Samples = *Samples - 1 ;
		if( *Samples == 1 ) break ;
	}

	// 使用できるマルチサンプルタイプが無かったら終了
	if( *Samples == 1 )
	{
		*Quality = 0 ;
		return 0 ;
	}

	// Ｚバッファで使用できるマルチサンプルタイプとその際のクオリティを取得する
	ZBufferQuality = 0 ;
	for(;;)
	{
		if( D3D11Device_CheckMultisampleQualityLevels( DIRECT3D11_DEPTHBUFFER_TEXTURE2D_FORMAT, *Samples, &ZBufferQuality ) == S_OK && ZBufferQuality > 0 )
			break ;

		if( SamplesFailedBreak )
		{
			*Quality = 0 ;
			return -1 ;
		}

		*Samples = *Samples - 1 ;
		if( *Samples == 1 ) break ;
	}

	// 使用できるマルチサンプルタイプが無かったら終了
	if( *Samples == 1 )
	{
		*Quality = 0 ;
		return 0 ;
	}

	// クオリティは低いほうに合わせる
	MaxQuality = ( ZBufferQuality < ColorBufferQuality ? ZBufferQuality : ColorBufferQuality ) - 1 ;
	if( *Quality > MaxQuality ) *Quality = MaxQuality ;

	// 終了
	return 0 ;
}

extern void *D3D11DeviceContext_Get( void )
{
	return GAPIWin.D3D11DeviceContext ;
}

extern void *D3D11DeviceContext_Deferred_Get( void )
{
	return GAPIWin.D3D11DeferredContext ;
}

extern void D3D11DeviceContext_VSSetConstantBuffers( UINT StartSlot, UINT NumBuffers, D_ID3D11Buffer *const *ppConstantBuffers )
{
	GAPIWin.D3D11DeviceContext->VSSetConstantBuffers( StartSlot, NumBuffers, ppConstantBuffers ) ;
}

extern void D3D11DeviceContext_PSSetShaderResources( UINT StartSlot, UINT NumViews,   D_ID3D11ShaderResourceView *const *ppShaderResourceViews )
{
	GAPIWin.D3D11DeviceContext->PSSetShaderResources( StartSlot, NumViews,   ppShaderResourceViews ) ;
}

extern void D3D11DeviceContext_PSSetShader( D_ID3D11PixelShader *pPixelShader, D_ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances )
{
	GAPIWin.D3D11DeviceContext->PSSetShader( pPixelShader, ppClassInstances, NumClassInstances ) ;
}

extern void D3D11DeviceContext_PSSetSamplers( UINT StartSlot, UINT NumSamplers, D_ID3D11SamplerState *const *ppSamplers )
{
	GAPIWin.D3D11DeviceContext->PSSetSamplers( StartSlot, NumSamplers, ppSamplers ) ;
}

extern void D3D11DeviceContext_VSSetShader( D_ID3D11VertexShader *pVertexShader, D_ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances )
{
	GAPIWin.D3D11DeviceContext->VSSetShader( pVertexShader, ppClassInstances, NumClassInstances ) ;
}

extern void D3D11DeviceContext_DrawIndexed( UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation )
{
	GAPIWin.D3D11DeviceContext->DrawIndexed( IndexCount, StartIndexLocation, BaseVertexLocation ) ;
}

extern void D3D11DeviceContext_Draw( UINT VertexCount, UINT StartVertexLocation )
{
	GAPIWin.D3D11DeviceContext->Draw( VertexCount, StartVertexLocation ) ;
}

extern HRESULT D3D11DeviceContext_Map( D_ID3D11Resource *pResource, UINT Subresource, D_D3D11_MAP MapType, UINT MapFlags, D_D3D11_MAPPED_SUBRESOURCE *pMappedResource )
{
	return GAPIWin.D3D11DeviceContext->Map( pResource, Subresource, MapType, MapFlags, pMappedResource ) ;
}

extern void D3D11DeviceContext_Unmap( D_ID3D11Resource *pResource, UINT Subresource )
{
	GAPIWin.D3D11DeviceContext->Unmap( pResource, Subresource ) ;
}

extern void D3D11DeviceContext_PSSetConstantBuffers( UINT StartSlot, UINT NumBuffers, D_ID3D11Buffer *const *ppConstantBuffers )
{
	GAPIWin.D3D11DeviceContext->PSSetConstantBuffers( StartSlot, NumBuffers, ppConstantBuffers ) ;
}

extern void D3D11DeviceContext_IASetInputLayout( D_ID3D11InputLayout *pInputLayout )
{
	GAPIWin.D3D11DeviceContext->IASetInputLayout( pInputLayout ) ;
}

extern void D3D11DeviceContext_IASetVertexBuffers( UINT StartSlot, UINT NumBuffers, D_ID3D11Buffer *const *ppVertexBuffers, const UINT *pStrides, const UINT *pOffsets )
{
	GAPIWin.D3D11DeviceContext->IASetVertexBuffers( StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets ) ;
}

extern void D3D11DeviceContext_IASetIndexBuffer( D_ID3D11Buffer *pIndexBuffer, D_DXGI_FORMAT Format, UINT Offset )
{
	GAPIWin.D3D11DeviceContext->IASetIndexBuffer( pIndexBuffer, Format, Offset ) ;
}

extern void D3D11DeviceContext_GSSetShader( D_ID3D11GeometryShader *pShader, D_ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances )
{
	GAPIWin.D3D11DeviceContext->GSSetShader( pShader, ppClassInstances, NumClassInstances ) ;
}

extern void D3D11DeviceContext_IASetPrimitiveTopology( D_D3D11_PRIMITIVE_TOPOLOGY Topology )
{
	GAPIWin.D3D11DeviceContext->IASetPrimitiveTopology( Topology ) ;
}

extern void D3D11DeviceContext_Begin( D_ID3D11Asynchronous *pAsync )
{
	GAPIWin.D3D11DeviceContext->Begin( pAsync ) ;
}

extern void D3D11DeviceContext_End( D_ID3D11Asynchronous *pAsync )
{
	GAPIWin.D3D11DeviceContext->End( pAsync ) ;
}

extern void D3D11DeviceContext_OMSetRenderTargets( UINT NumViews, D_ID3D11RenderTargetView *const *ppRenderTargetViews, D_ID3D11DepthStencilView *pDepthStencilView )
{
	GAPIWin.D3D11DeviceContext->OMSetRenderTargets( NumViews, ppRenderTargetViews, pDepthStencilView ) ;
}

extern void D3D11DeviceContext_OMSetBlendState( D_ID3D11BlendState *pBlendState, const FLOAT BlendFactor[ 4 ], UINT SampleMask )
{
	GAPIWin.D3D11DeviceContext->OMSetBlendState( pBlendState, BlendFactor, SampleMask ) ;
}

extern void D3D11DeviceContext_OMSetDepthStencilState( D_ID3D11DepthStencilState *pDepthStencilState, UINT StencilRef )
{
	GAPIWin.D3D11DeviceContext->OMSetDepthStencilState( pDepthStencilState, StencilRef ) ;
}

extern void D3D11DeviceContext_RSSetState( D_ID3D11RasterizerState *pRasterizerState )
{
	GAPIWin.D3D11DeviceContext->RSSetState( pRasterizerState ) ;
}

extern void D3D11DeviceContext_RSSetViewports( UINT NumViewports, const D_D3D11_VIEWPORT *pViewports )
{
	GAPIWin.D3D11DeviceContext->RSSetViewports( NumViewports, pViewports ) ;
}

extern void D3D11DeviceContext_CopyResource( D_ID3D11Resource *pDstResource, D_ID3D11Resource *pSrcResource )
{
	GAPIWin.D3D11DeviceContext->CopyResource( pDstResource, pSrcResource ) ;
}

extern void D3D11DeviceContext_UpdateSubresource( D_ID3D11Resource *pDstResource, UINT DstSubresource, const D_D3D11_BOX *pDstBox, const void *pSrcData, UINT SrcRowPitch, UINT SrcDepthPitch )
{
	GAPIWin.D3D11DeviceContext->UpdateSubresource( pDstResource, DstSubresource, pDstBox, pSrcData, SrcRowPitch, SrcDepthPitch ) ;
}

extern void D3D11DeviceContext_ClearRenderTargetView( D_ID3D11RenderTargetView *pRenderTargetView, const FLOAT ColorRGBA[ 4 ] )
{
	GAPIWin.D3D11DeviceContext->ClearRenderTargetView( pRenderTargetView, ColorRGBA ) ;
}

extern void D3D11DeviceContext_ClearDepthStencilView( D_ID3D11DepthStencilView *pDepthStencilView, UINT ClearFlags, FLOAT Depth, D_UINT8 Stencil )
{
	GAPIWin.D3D11DeviceContext->ClearDepthStencilView( pDepthStencilView, ClearFlags, Depth, Stencil ) ;
}

extern void D3D11DeviceContext_ResolveSubresource( D_ID3D11Resource *pDstResource, UINT DstSubresource, D_ID3D11Resource *pSrcResource, UINT SrcSubresource, D_DXGI_FORMAT Format )
{
	GAPIWin.D3D11DeviceContext->ResolveSubresource( pDstResource, DstSubresource, pSrcResource, SrcSubresource, Format ) ;
}

extern void D3D11DeviceContext_HSSetShader( D_ID3D11HullShader *pHullShader, D_ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances )
{
	GAPIWin.D3D11DeviceContext->HSSetShader( pHullShader, ppClassInstances, NumClassInstances ) ;
}

extern void D3D11DeviceContext_DSSetShader( D_ID3D11DomainShader *pDomainShader, D_ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances )
{
	GAPIWin.D3D11DeviceContext->DSSetShader( pDomainShader, ppClassInstances, NumClassInstances ) ;
}

extern void D3D11DeviceContext_CSSetShader( D_ID3D11ComputeShader *pComputeShader, D_ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances )
{
	GAPIWin.D3D11DeviceContext->CSSetShader( pComputeShader, ppClassInstances, NumClassInstances ) ;
}

extern void D3D11DeviceContext_ClearState( void )
{
	GAPIWin.D3D11DeviceContext->ClearState() ;
}

extern void D3D11DeviceContext_Flush( void )
{
	GAPIWin.D3D11DeviceContext->Flush() ;
}




extern void D3D11View_GetResource( D_ID3D11View *View, D_ID3D11Resource **ppResource )
{
	View->GetResource( ppResource ) ;
}




extern void D3D11Texture2D_GetDesc( D_ID3D11Texture2D *Texture2D, D_D3D11_TEXTURE2D_DESC *pDesc )
{
	Texture2D->GetDesc( pDesc ) ;
}




extern void D3D11RenderTargetView_GetDesc( D_ID3D11RenderTargetView *RenderTargetView, D_D3D11_RENDER_TARGET_VIEW_DESC *pDesc )
{
	RenderTargetView->GetDesc( pDesc ) ;
}




#endif // DX_NON_DIRECT3D11


























#ifndef DX_NON_DIRECT3D9



#ifndef DX_NON_ASYNCLOAD
extern int Direct3D9_ObjectRelease_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return ( int )Direct3D9_ObjectRelease_ASync(
				( void * )Info->Data[ 0 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern ULONG Direct3D9_ObjectRelease_ASync( void *pObject, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3D9_ObjectRelease_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pObject ;
		return ( ULONG )AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3D9_ObjectRelease( pObject ) ;
}







#ifndef DX_NON_ASYNCLOAD

// マルチサンプルレンダリングのサンプル数とクオリティをチェックする非同期読み込み処理用コールバック関数
static int Direct3D9_CheckMultiSampleParamASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return Direct3D9_CheckMultiSampleParam_ASync( 
						( D_D3DFORMAT )Info->Data[ 0 ],
						( D_D3DMULTISAMPLE_TYPE * )Info->Data[ 1 ],
						( DWORD * )Info->Data[ 2 ],
						( int )Info->Data[ 3 ],
						FALSE 
					) ;
}

#endif // DX_NON_ASYNCLOAD

// マルチサンプルレンダリングのサンプル数とクオリティをチェック
extern int Direct3D9_CheckMultiSampleParam_ASync( D_D3DFORMAT Format, D_D3DMULTISAMPLE_TYPE *Samples, DWORD *Quality, int SamplesFailedBreak, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread == TRUE )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3D9_CheckMultiSampleParamASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )Format ;
		AInfo.Data[ 1 ] = ( DWORD_PTR )Samples ;
		AInfo.Data[ 2 ] = ( DWORD_PTR )Quality ;
		AInfo.Data[ 3 ] = ( DWORD_PTR )SamplesFailedBreak ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3D9_CheckMultiSampleParam( Format, Samples, Quality, SamplesFailedBreak ) ;
}







#ifndef DX_NON_ASYNCLOAD
int Direct3DDevice9_CreateRenderTarget_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return Direct3DDevice9_CreateRenderTarget_ASync(
				( UINT )Info->Data[ 0 ],
				( UINT )Info->Data[ 1 ],
				( D_D3DFORMAT )Info->Data[ 2 ],
				( D_D3DMULTISAMPLE_TYPE )Info->Data[ 3 ],
				( DWORD )Info->Data[ 4 ],
				( BOOL )Info->Data[ 5 ],
				( D_IDirect3DSurface9** )Info->Data[ 6 ],
				( HANDLE* )Info->Data[ 7 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern HRESULT Direct3DDevice9_CreateRenderTarget_ASync( UINT Width, UINT Height, D_D3DFORMAT Format, D_D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, D_IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3DDevice9_CreateRenderTarget_ASyncCallback ;
		AInfo.Data[ 0 ] = Width ;
		AInfo.Data[ 1 ] = Height ;
		AInfo.Data[ 2 ] = ( DWORD_PTR )Format ;
		AInfo.Data[ 3 ] = ( DWORD_PTR )MultiSample ;
		AInfo.Data[ 4 ] = ( DWORD_PTR )MultisampleQuality ;
		AInfo.Data[ 5 ] = ( DWORD_PTR )Lockable ;
		AInfo.Data[ 6 ] = ( DWORD_PTR )ppSurface ;
		AInfo.Data[ 7 ] = ( DWORD_PTR )pSharedHandle ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3DDevice9_CreateRenderTarget(
					Width,					// 幅
					Height,					// 高さ
					Format,					// フォーマット
					MultiSample,			// マルチサンプリングタイプ
					MultisampleQuality,		// マルチサンプリングクオリティ
					Lockable,				// ロックはできない
					ppSurface,				// D_IDirect3DSurface9 のポインタを受け取るアドレス
					pSharedHandle			// 絶対 NULL
				) ;
}

#ifndef DX_NON_ASYNCLOAD
int Direct3DDevice9_CreateDepthStencilSurface_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return Direct3DDevice9_CreateDepthStencilSurface_ASync(
				( UINT )Info->Data[ 0 ],
				( UINT )Info->Data[ 1 ],
				( D_D3DFORMAT )Info->Data[ 2 ],
				( D_D3DMULTISAMPLE_TYPE )Info->Data[ 3 ],
				( DWORD )Info->Data[ 4 ],
				( BOOL )Info->Data[ 5 ],
				( D_IDirect3DSurface9** )Info->Data[ 6 ],
				( HANDLE* )Info->Data[ 7 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern HRESULT Direct3DDevice9_CreateDepthStencilSurface_ASync( UINT Width, UINT Height, D_D3DFORMAT Format, D_D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, D_IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3DDevice9_CreateDepthStencilSurface_ASyncCallback ;
		AInfo.Data[ 0 ] = Width ;
		AInfo.Data[ 1 ] = Height ;
		AInfo.Data[ 2 ] = ( DWORD_PTR )Format ;
		AInfo.Data[ 3 ] = ( DWORD_PTR )MultiSample ;
		AInfo.Data[ 4 ] = ( DWORD_PTR )MultisampleQuality ;
		AInfo.Data[ 5 ] = ( DWORD_PTR )Discard ;
		AInfo.Data[ 6 ] = ( DWORD_PTR )ppSurface ;
		AInfo.Data[ 7 ] = ( DWORD_PTR )pSharedHandle ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3DDevice9_CreateDepthStencilSurface(
					Width,					// 幅
					Height,					// 高さ
					Format,					// フォーマット
					MultiSample,			// マルチサンプリングタイプ
					MultisampleQuality,		// マルチサンプリングクオリティ
					Discard,				// ロックはできない
					ppSurface,				// D_IDirect3DSurface9 のポインタを受け取るアドレス
					pSharedHandle			// 絶対 NULL
				) ;
}

#ifndef DX_NON_ASYNCLOAD
int Direct3DDevice9_CreateTexture_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return Direct3DDevice9_CreateTexture_ASync(
				( UINT )Info->Data[ 0 ],
				( UINT )Info->Data[ 1 ],
				( UINT )Info->Data[ 2 ],
				( DWORD )Info->Data[ 3 ],
				( D_D3DFORMAT )Info->Data[ 4 ],
				( D_D3DPOOL )Info->Data[ 5 ],
				( D_IDirect3DTexture9** )Info->Data[ 6 ],
				( HANDLE* )Info->Data[ 7 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern HRESULT Direct3DDevice9_CreateTexture_ASync( UINT Width, UINT Height, UINT Levels, DWORD Usage, D_D3DFORMAT Format, D_D3DPOOL Pool, D_IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3DDevice9_CreateTexture_ASyncCallback ;
		AInfo.Data[ 0 ] = Width ;
		AInfo.Data[ 1 ] = Height ;
		AInfo.Data[ 2 ] = Levels ;
		AInfo.Data[ 3 ] = Usage ;
		AInfo.Data[ 4 ] = ( DWORD_PTR )Format ;
		AInfo.Data[ 5 ] = ( DWORD_PTR )Pool ;
		AInfo.Data[ 6 ] = ( DWORD_PTR )ppTexture ;
		AInfo.Data[ 7 ] = ( DWORD_PTR )pSharedHandle ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3DDevice9_CreateTexture(
				Width,				// 幅
				Height,				// 高さ
				Levels,				// MipMap レベル
				Usage,				// フラグ
				Format,				// フォーマット
				Pool,				// テクスチャを DirectX が管理するかどうか
				ppTexture,			// D_IDirect3DTexture9 のポインタを受け取るアドレス
				pSharedHandle		// 絶対 NULL
			) ;
}

#ifndef DX_NON_ASYNCLOAD
int Direct3DDevice9_CreateCubeTexture_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return Direct3DDevice9_CreateCubeTexture_ASync(
				( UINT )Info->Data[ 0 ],
				( UINT )Info->Data[ 1 ],
				( DWORD )Info->Data[ 2 ],
				( D_D3DFORMAT )Info->Data[ 3 ],
				( D_D3DPOOL )Info->Data[ 4 ],
				( D_IDirect3DCubeTexture9** )Info->Data[ 5 ],
				( HANDLE* )Info->Data[ 6 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern HRESULT Direct3DDevice9_CreateCubeTexture_ASync( UINT EdgeLength, UINT Levels, DWORD Usage, D_D3DFORMAT Format, D_D3DPOOL Pool, D_IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3DDevice9_CreateCubeTexture_ASyncCallback ;
		AInfo.Data[ 0 ] = EdgeLength ;
		AInfo.Data[ 1 ] = Levels ;
		AInfo.Data[ 2 ] = Usage ;
		AInfo.Data[ 3 ] = ( DWORD_PTR )Format ;
		AInfo.Data[ 4 ] = ( DWORD_PTR )Pool ;
		AInfo.Data[ 5 ] = ( DWORD_PTR )ppCubeTexture ;
		AInfo.Data[ 6 ] = ( DWORD_PTR )pSharedHandle ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3DDevice9_CreateCubeTexture(
				EdgeLength,			// エッジサイズ
				Levels,				// MipMap レベル
				Usage,				// フラグ
				Format,				// フォーマット
				Pool,				// テクスチャを DirectX が管理するかどうか
				ppCubeTexture,		// D_IDirect3DCubeTexture9 のポインタを受け取るアドレス
				pSharedHandle		// 絶対 NULL
			) ;
}

#ifndef DX_NON_ASYNCLOAD
int Direct3DDevice9_ColorFill_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return Direct3DDevice9_ColorFill_ASync(
				( D_IDirect3DSurface9* )Info->Data[ 0 ],
				( RECT* )Info->Data[ 1 ],
				( D_D3DCOLOR )Info->Data[ 2 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern HRESULT Direct3DDevice9_ColorFill_ASync( D_IDirect3DSurface9* pSurface, CONST RECT* pRect, D_D3DCOLOR color, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3DDevice9_ColorFill_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pSurface ;
		AInfo.Data[ 1 ] = ( DWORD_PTR )pRect ;
		AInfo.Data[ 2 ] = ( DWORD_PTR )color ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3DDevice9_ColorFill( pSurface, pRect, color ) ;
}

#ifndef DX_NON_ASYNCLOAD
extern int Direct3DDevice9_CreateOffscreenPlainSurface_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return Direct3DDevice9_CreateOffscreenPlainSurface_ASync(
				( UINT )Info->Data[ 0 ],
				( UINT )Info->Data[ 1 ],
				( D_D3DFORMAT )Info->Data[ 2 ],
				( D_D3DPOOL )Info->Data[ 3 ],
				( D_IDirect3DSurface9** )Info->Data[ 4 ],
				( HANDLE* )Info->Data[ 5 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern HRESULT Direct3DDevice9_CreateOffscreenPlainSurface_ASync( UINT Width, UINT Height, D_D3DFORMAT Format, D_D3DPOOL Pool, D_IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3DDevice9_CreateOffscreenPlainSurface_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )Width ;
		AInfo.Data[ 1 ] = ( DWORD_PTR )Height ;
		AInfo.Data[ 2 ] = ( DWORD_PTR )Format ;
		AInfo.Data[ 3 ] = ( DWORD_PTR )Pool ;
		AInfo.Data[ 4 ] = ( DWORD_PTR )ppSurface ;
		AInfo.Data[ 5 ] = ( DWORD_PTR )pSharedHandle ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3DDevice9_CreateOffscreenPlainSurface( Width, Height, Format, Pool, ppSurface, pSharedHandle ) ;
}

#ifndef DX_NON_ASYNCLOAD
extern int Direct3DDevice9_UpdateSurface_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return Direct3DDevice9_UpdateSurface_ASync(
				( D_IDirect3DSurface9* )Info->Data[ 0 ],
				( CONST RECT* )Info->Data[ 1 ],
				( D_IDirect3DSurface9* )Info->Data[ 2 ],
				( CONST POINT* )Info->Data[ 3 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern HRESULT Direct3DDevice9_UpdateSurface_ASync( D_IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, D_IDirect3DSurface9* pDestinationSurface, CONST POINT* pDestPoint, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3DDevice9_UpdateSurface_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pSourceSurface ;
		AInfo.Data[ 1 ] = ( DWORD_PTR )pSourceRect ;
		AInfo.Data[ 2 ] = ( DWORD_PTR )pDestinationSurface ;
		AInfo.Data[ 3 ] = ( DWORD_PTR )pDestPoint ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3DDevice9_UpdateSurface( pSourceSurface, pSourceRect, pDestinationSurface, pDestPoint ) ;
}

#ifndef DX_NON_ASYNCLOAD
extern int Direct3DDevice9_UpdateTexture_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return Direct3DDevice9_UpdateTexture_ASync(
				( D_IDirect3DBaseTexture9* )Info->Data[ 0 ],
				( D_IDirect3DBaseTexture9* )Info->Data[ 1 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern HRESULT Direct3DDevice9_UpdateTexture_ASync( D_IDirect3DBaseTexture9* pSourceTexture, D_IDirect3DBaseTexture9* pDestinationTexture, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3DDevice9_UpdateTexture_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pSourceTexture ;
		AInfo.Data[ 1 ] = ( DWORD_PTR )pDestinationTexture ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3DDevice9_UpdateTexture( pSourceTexture, pDestinationTexture ) ;
}

#ifndef DX_NON_ASYNCLOAD
extern int Direct3DDevice9_CreateVertexShader_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return Direct3DDevice9_CreateVertexShader_ASync(
				( CONST DWORD* )Info->Data[ 0 ],
				( D_IDirect3DVertexShader9** )Info->Data[ 1 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern HRESULT Direct3DDevice9_CreateVertexShader_ASync( CONST DWORD* pFunction, D_IDirect3DVertexShader9** ppShader, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3DDevice9_CreateVertexShader_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pFunction ;
		AInfo.Data[ 1 ] = ( DWORD_PTR )ppShader ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3DDevice9_CreateVertexShader( pFunction, ppShader ) ;
}

#ifndef DX_NON_ASYNCLOAD
extern int Direct3DDevice9_CreatePixelShader_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return Direct3DDevice9_CreatePixelShader_ASync(
				( CONST DWORD* )Info->Data[ 0 ],
				( D_IDirect3DPixelShader9** )Info->Data[ 1 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern HRESULT Direct3DDevice9_CreatePixelShader_ASync( CONST DWORD* pFunction, D_IDirect3DPixelShader9** ppShader, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3DDevice9_CreatePixelShader_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pFunction ;
		AInfo.Data[ 1 ] = ( DWORD_PTR )ppShader ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3DDevice9_CreatePixelShader( pFunction, ppShader ) ;
}

#ifndef DX_NON_ASYNCLOAD
extern int Direct3DDevice9_CreateVertexBuffer_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return Direct3DDevice9_CreateVertexBuffer_ASync(
				( DWORD )Info->Data[ 0 ],
				( DWORD )Info->Data[ 1 ],
				( DWORD )Info->Data[ 2 ],
				( D_D3DPOOL )Info->Data[ 3 ],
				( D_IDirect3DVertexBuffer9 ** )Info->Data[ 4 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

// 頂点バッファを作成する
extern int Direct3DDevice9_CreateVertexBuffer_ASync( DWORD Length, DWORD Usage, DWORD FVFFlag, D_D3DPOOL Pool, D_IDirect3DVertexBuffer9 **BufferP, int ASyncThread )
{
	HRESULT Result ;

#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3DDevice9_CreateVertexBuffer_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )Length ;
		AInfo.Data[ 1 ] = ( DWORD_PTR )Usage ;
		AInfo.Data[ 2 ] = ( DWORD_PTR )FVFFlag ;
		AInfo.Data[ 3 ] = ( DWORD_PTR )Pool ;
		AInfo.Data[ 4 ] = ( DWORD_PTR )BufferP ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	if( Direct3DDevice9_IsValid() == 0 ) return -1 ;
	
	Result = Direct3DDevice9_CreateVertexBuffer( Length, Usage, FVFFlag, Pool, BufferP, NULL ) ;

	return Result == D_D3D_OK ? 0 : -1 ;
}

#ifndef DX_NON_ASYNCLOAD
extern int Direct3DDevice9_CreateIndexBuffer_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return Direct3DDevice9_CreateIndexBuffer_ASync(
				( DWORD )Info->Data[ 0 ],
				( DWORD )Info->Data[ 1 ],
				( D_D3DFORMAT )Info->Data[ 2 ],
				( D_D3DPOOL )Info->Data[ 3 ],
				( D_IDirect3DIndexBuffer9 ** )Info->Data[ 4 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

// インデックスバッファを作成する
extern int Direct3DDevice9_CreateIndexBuffer_ASync( DWORD Length, DWORD Usage, D_D3DFORMAT Format, D_D3DPOOL Pool, D_IDirect3DIndexBuffer9 **BufferP, int ASyncThread )
{
	HRESULT Result ;

#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3DDevice9_CreateIndexBuffer_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )Length ;
		AInfo.Data[ 1 ] = ( DWORD_PTR )Usage ;
		AInfo.Data[ 2 ] = ( DWORD_PTR )Format ;
		AInfo.Data[ 3 ] = ( DWORD_PTR )Pool ;
		AInfo.Data[ 4 ] = ( DWORD_PTR )BufferP ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	if( Direct3DDevice9_IsValid() == 0 ) return -1 ;

	Result = Direct3DDevice9_CreateIndexBuffer( Length, Usage, Format, Pool, BufferP, NULL ) ;

	return Result == D_D3D_OK ? 0 : -1 ;
}









#ifndef DX_NON_ASYNCLOAD
int Direct3DTexture9_GetSurfaceLevel_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return Direct3DTexture9_GetSurfaceLevel_ASync(
				( D_IDirect3DTexture9 * )Info->Data[ 0 ],
				( UINT )Info->Data[ 1 ],
				( D_IDirect3DSurface9** )Info->Data[ 2 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern HRESULT Direct3DTexture9_GetSurfaceLevel_ASync( D_IDirect3DTexture9 *Texture, UINT Level,  D_IDirect3DSurface9** ppSurfaceLevel, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3DTexture9_GetSurfaceLevel_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )Texture ;
		AInfo.Data[ 1 ] = ( DWORD_PTR )Level ;
		AInfo.Data[ 2 ] = ( DWORD_PTR )ppSurfaceLevel ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3DTexture9_GetSurfaceLevel( Texture, Level, ppSurfaceLevel ) ;
}

#ifndef DX_NON_ASYNCLOAD
extern int Direct3DTexture9_LockRect_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return Direct3DTexture9_LockRect_ASync(
				( D_IDirect3DTexture9 * )Info->Data[ 0 ],
				( UINT )Info->Data[ 1 ],
				( D_D3DLOCKED_RECT* )Info->Data[ 2 ],
				( CONST RECT* )Info->Data[ 3 ],
				( DWORD )Info->Data[ 4 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern HRESULT Direct3DTexture9_LockRect_ASync( D_IDirect3DTexture9 *Texture, UINT Level, D_D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3DTexture9_LockRect_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )Texture ;
		AInfo.Data[ 1 ] = ( DWORD_PTR )Level ;
		AInfo.Data[ 2 ] = ( DWORD_PTR )pLockedRect ;
		AInfo.Data[ 3 ] = ( DWORD_PTR )pRect ;
		AInfo.Data[ 4 ] = ( DWORD_PTR )Flags ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3DTexture9_LockRect( Texture, Level, pLockedRect, pRect, Flags ) ;
}

#ifndef DX_NON_ASYNCLOAD

extern int Direct3DTexture9_UnlockRect_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return Direct3DTexture9_UnlockRect_ASync(
				( D_IDirect3DTexture9 * )Info->Data[ 0 ],
				( UINT )Info->Data[ 1 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern HRESULT Direct3DTexture9_UnlockRect_ASync( D_IDirect3DTexture9 *Texture, UINT Level, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3DTexture9_UnlockRect_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )Texture ;
		AInfo.Data[ 1 ] = ( DWORD_PTR )Level ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3DTexture9_UnlockRect( Texture, Level ) ;
}









#ifndef DX_NON_ASYNCLOAD
int Direct3DCubeTexture9_GetCubeMapSurface_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return Direct3DCubeTexture9_GetCubeMapSurface_ASync(
				( D_IDirect3DCubeTexture9 * )Info->Data[ 0 ],
				( D_D3DCUBEMAP_FACES )Info->Data[ 1 ],
				( UINT )Info->Data[ 2 ],
				( D_IDirect3DSurface9** )Info->Data[ 3 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern HRESULT Direct3DCubeTexture9_GetCubeMapSurface_ASync( D_IDirect3DCubeTexture9 *CubeTexture, D_D3DCUBEMAP_FACES FaceType, UINT Level, D_IDirect3DSurface9** ppCubeMapSurface, int ASyncThread)
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3DCubeTexture9_GetCubeMapSurface_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )CubeTexture ;
		AInfo.Data[ 1 ] = ( DWORD_PTR )FaceType ;
		AInfo.Data[ 2 ] = ( DWORD_PTR )Level ;
		AInfo.Data[ 3 ] = ( DWORD_PTR )ppCubeMapSurface ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3DCubeTexture9_GetCubeMapSurface( CubeTexture, FaceType, Level, ppCubeMapSurface ) ;
}










#ifndef DX_NON_ASYNCLOAD
extern int Direct3DCubeTexture9_LockRect_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return Direct3DCubeTexture9_LockRect_ASync(
				( D_IDirect3DCubeTexture9 * )Info->Data[ 0 ],
				( D_D3DCUBEMAP_FACES )Info->Data[ 1 ],
				( UINT )Info->Data[ 2 ],
				( D_D3DLOCKED_RECT* )Info->Data[ 3 ],
				( CONST RECT* )Info->Data[ 4 ],
				( DWORD )Info->Data[ 5 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern HRESULT Direct3DCubeTexture9_LockRect_ASync( D_IDirect3DCubeTexture9 *CubeTexture, D_D3DCUBEMAP_FACES FaceType, UINT Level, D_D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3DCubeTexture9_LockRect_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )CubeTexture ;
		AInfo.Data[ 1 ] = ( DWORD_PTR )FaceType ;
		AInfo.Data[ 2 ] = ( DWORD_PTR )Level ;
		AInfo.Data[ 3 ] = ( DWORD_PTR )pLockedRect ;
		AInfo.Data[ 4 ] = ( DWORD_PTR )pRect ;
		AInfo.Data[ 5 ] = ( DWORD_PTR )Flags ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3DCubeTexture9_LockRect( CubeTexture, FaceType, Level, pLockedRect, pRect, Flags ) ;
}

#ifndef DX_NON_ASYNCLOAD

extern int Direct3DCubeTexture9_UnlockRect_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return Direct3DCubeTexture9_UnlockRect_ASync(
				( D_IDirect3DCubeTexture9 * )Info->Data[ 0 ],
				( D_D3DCUBEMAP_FACES )Info->Data[ 1 ],
				( UINT )Info->Data[ 2 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern HRESULT Direct3DCubeTexture9_UnlockRect_ASync( D_IDirect3DCubeTexture9 *CubeTexture, D_D3DCUBEMAP_FACES FaceType, UINT Level, int ASyncThread)
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3DCubeTexture9_UnlockRect_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )CubeTexture ;
		AInfo.Data[ 1 ] = ( DWORD_PTR )FaceType ;
		AInfo.Data[ 2 ] = ( DWORD_PTR )Level ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3DCubeTexture9_UnlockRect( CubeTexture, FaceType, Level ) ;
}














#ifndef DX_NON_ASYNCLOAD
extern int Direct3DSurface9_LockRect_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return Direct3DSurface9_LockRect_ASync(
				( D_IDirect3DSurface9* )Info->Data[ 0 ],
				( D_D3DLOCKED_RECT* )Info->Data[ 1 ],
				( CONST RECT* )Info->Data[ 2 ],
				( DWORD )Info->Data[ 3 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern HRESULT Direct3DSurface9_LockRect_ASync( D_IDirect3DSurface9* pSurface, D_D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3DSurface9_LockRect_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pSurface ;
		AInfo.Data[ 1 ] = ( DWORD_PTR )pLockedRect ;
		AInfo.Data[ 2 ] = ( DWORD_PTR )pRect ;
		AInfo.Data[ 3 ] = ( DWORD_PTR )Flags ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3DSurface9_LockRect( pSurface, pLockedRect, pRect, Flags ) ;
}

#ifndef DX_NON_ASYNCLOAD
extern int Direct3DSurface9_UnlockRect_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return Direct3DSurface9_UnlockRect_ASync(
				( D_IDirect3DSurface9* )Info->Data[ 0 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern HRESULT Direct3DSurface9_UnlockRect_ASync( D_IDirect3DSurface9* pSurface, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3DSurface9_UnlockRect_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )pSurface ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3DSurface9_UnlockRect( pSurface ) ;
}








#ifndef DX_NON_ASYNCLOAD
extern int Direct3DVertexBuffer9_Lock_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return Direct3DVertexBuffer9_Lock_ASync(
				( D_IDirect3DVertexBuffer9 * )Info->Data[ 0 ],
				( UINT )Info->Data[ 1 ],
				( UINT )Info->Data[ 2 ],
				( void ** )Info->Data[ 3 ],
				( DWORD )Info->Data[ 4 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern	HRESULT	Direct3DVertexBuffer9_Lock_ASync( D_IDirect3DVertexBuffer9 *Buffer, UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3DVertexBuffer9_Lock_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )Buffer ;
		AInfo.Data[ 1 ] = ( DWORD_PTR )OffsetToLock ;
		AInfo.Data[ 2 ] = ( DWORD_PTR )SizeToLock ;
		AInfo.Data[ 3 ] = ( DWORD_PTR )ppbData ;
		AInfo.Data[ 4 ] = ( DWORD_PTR )Flags ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3DVertexBuffer9_Lock( Buffer, OffsetToLock, SizeToLock, ppbData, Flags ) ;
}

#ifndef DX_NON_ASYNCLOAD
extern int Direct3DVertexBuffer9_Unlock_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return Direct3DVertexBuffer9_Unlock_ASync(
				( D_IDirect3DVertexBuffer9 * )Info->Data[ 0 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern	HRESULT Direct3DVertexBuffer9_Unlock_ASync( D_IDirect3DVertexBuffer9 *Buffer, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3DVertexBuffer9_Unlock_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )Buffer ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3DVertexBuffer9_Unlock( Buffer ) ;
}










#ifndef DX_NON_ASYNCLOAD
extern int Direct3DIndexBuffer9_Lock_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return Direct3DIndexBuffer9_Lock_ASync(
				( D_IDirect3DIndexBuffer9 * )Info->Data[ 0 ],
				( UINT )Info->Data[ 1 ],
				( UINT )Info->Data[ 2 ],
				( void ** )Info->Data[ 3 ],
				( DWORD )Info->Data[ 4 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern	HRESULT	Direct3DIndexBuffer9_Lock_ASync( D_IDirect3DIndexBuffer9 *Buffer, UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3DIndexBuffer9_Lock_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )Buffer ;
		AInfo.Data[ 1 ] = ( DWORD_PTR )OffsetToLock ;
		AInfo.Data[ 2 ] = ( DWORD_PTR )SizeToLock ;
		AInfo.Data[ 3 ] = ( DWORD_PTR )ppbData ;
		AInfo.Data[ 4 ] = ( DWORD_PTR )Flags ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3DIndexBuffer9_Lock( Buffer, OffsetToLock, SizeToLock, ppbData, Flags ) ;
}

#ifndef DX_NON_ASYNCLOAD
extern int Direct3DIndexBuffer9_Unlock_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	return Direct3DIndexBuffer9_Unlock_ASync(
				( D_IDirect3DIndexBuffer9 * )Info->Data[ 0 ],
				FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern	HRESULT Direct3DIndexBuffer9_Unlock_ASync( D_IDirect3DIndexBuffer9 *Buffer, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = Direct3DIndexBuffer9_Unlock_ASyncCallback ;
		AInfo.Data[ 0 ] = ( DWORD_PTR )Buffer ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return Direct3DIndexBuffer9_Unlock( Buffer ) ;
}



















extern ULONG Direct3D9_ObjectRelease( void *pObject )
{
	return ( ( D_IUnknown * )pObject )->Release() ;
}






extern int Direct3D9_Create( void )
{
	D_IDirect3D9 * ( WINAPI *DF_Direct3DCreate9 )( UINT SDKVersion ) ;
	HRESULT ( WINAPI * DF_Direct3DCreate9Ex )(  UINT SDKVersion, D_IDirect3D9Ex **ppD3D ) ;

	// 既に作成済みの場合は一度削除する
	if( GAPIWin.Direct3D9Object )
	{
		Direct3D9_Release() ;
	}
	GAPIWin.Direct3D9ExObject = NULL ;

	// 先に Direct3DCreate9Ex のアドレス取得を試みる
	if( GD3D9.Setting.NotUseDirect3D9Ex == FALSE )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x49\x00\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x33\x00\x44\x00\x39\x00\x45\x00\x78\x00\x20\x00\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x92\x30\xd6\x53\x97\x5f\x57\x30\x7e\x30\x59\x30\x2e\x00\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"IDirect3D9Ex オブジェクトを取得します.... " @*/ ) ;

		DF_Direct3DCreate9Ex = ( HRESULT ( WINAPI * )( UINT SDKVersion, D_IDirect3D9Ex ** ) )GetProcAddress( GAPIWin.Direct3D9DLL, "Direct3DCreate9Ex" ) ;
		if( DF_Direct3DCreate9Ex != NULL )
		{
			// IDirect3D9Ex オブジェクトの作成
			if( DF_Direct3DCreate9Ex( 32, &GAPIWin.Direct3D9ExObject ) == S_OK )
			{
				GAPIWin.Direct3D9Object = GAPIWin.Direct3D9ExObject ;
			}
		}
	}

	// IDirect3D9Ex が作成できなかったら IDirect3D9 を作成する
	if( GAPIWin.Direct3D9Object == NULL )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x49\x00\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x33\x00\x44\x00\x39\x00\x20\x00\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x92\x30\xd6\x53\x97\x5f\x57\x30\x7e\x30\x59\x30\x2e\x00\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"IDirect3D9 オブジェクトを取得します.... " @*/ ) ;

		// Direct3DCreate9 API のアドレス取得
		DF_Direct3DCreate9 = ( D_IDirect3D9 * ( WINAPI * )( UINT SDKVersion ) )GetProcAddress( GAPIWin.Direct3D9DLL, "Direct3DCreate9" ) ;
		if( DF_Direct3DCreate9 == NULL )
		{
			return DXST_LOGFILE_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x33\x00\x44\x00\x43\x00\x72\x00\x65\x00\x61\x00\x74\x00\x65\x00\x39\x00\x20\x00\x41\x00\x50\x00\x49\x00\x6e\x30\xa2\x30\xc9\x30\xec\x30\xb9\x30\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Direct3DCreate9 APIのアドレス取得に失敗しました\n" @*/ ) ;
		}

		// IDirect3D9 オブジェクトの作成
		GAPIWin.Direct3D9Object = DF_Direct3DCreate9( 32 ) ;
		if( GAPIWin.Direct3D9Object == NULL )
		{
			return DXST_LOGFILE_ADDUTF16LE( "\x49\x00\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x33\x00\x44\x00\x39\x00\x20\x00\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"IDirect3D9 オブジェクトの取得に失敗しました\n" @*/ ) ;
		}
	}

	DXST_LOGFILE_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"成功\n" @*/ ) ;

	// 終了
	return 0 ;
}

extern int Direct3D9_LoadDLL( void )
{
	// Direct3D9.DLL の読み込み
	GAPIWin.Direct3D9DLL = LoadLibraryW( L"d3d9.dll" ) ;
	if( GAPIWin.Direct3D9DLL == NULL )
	{
		return DXST_LOGFILE_ADDUTF16LE( "\x64\x00\x33\x00\x64\x00\x39\x00\x2e\x00\x64\x00\x6c\x00\x6c\x00\x20\x00\x6e\x30\xad\x8a\x7f\x30\xbc\x8f\x7f\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"d3d9.dll の読み込みに失敗しました\n" @*/ ) ;
	}

	// 正常終了
	return 0 ;
}

extern int Direct3D9_FreeDLL( void )
{
	// d3d9.dll の解放
	if( GAPIWin.Direct3D9DLL )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x64\x00\x33\x00\x64\x00\x39\x00\x2e\x00\x64\x00\x6c\x00\x6c\x00\x20\x00\x6e\x30\xe3\x89\x3e\x65\x20\x00\x31\x00\x0a\x00\x00"/*@ L"d3d9.dll の解放 1\n" @*/ ) ;
		FreeLibrary( GAPIWin.Direct3D9DLL ) ;
		GAPIWin.Direct3D9DLL = NULL ;
	}

	// 正常終了
	return 0 ;
}

extern ULONG Direct3D9_Release( void )
{
	ULONG Result ;

	if( GAPIWin.Direct3D9Object == NULL )
	{
		return 0 ;
	}
	Result = GAPIWin.Direct3D9Object->Release() ;
	GAPIWin.Direct3D9Object = NULL ;

	return Result ;
}

extern UINT Direct3D9_GetAdapterCount( void )
{
	return GAPIWin.Direct3D9Object->GetAdapterCount() ;
}

extern long Direct3D9_GetAdapterIdentifier( DWORD Adapter, DWORD Flags, D_D3DADAPTER_IDENTIFIER9* pIdentifier)
{
	return GAPIWin.Direct3D9Object->GetAdapterIdentifier( Adapter, Flags, pIdentifier) ;
}

extern UINT Direct3D9_GetAdapterModeCount( DWORD Adapter, D_D3DFORMAT Format)
{
	return GAPIWin.Direct3D9Object->GetAdapterModeCount( Adapter, Format) ;
}

extern long Direct3D9_EnumAdapterModes( DWORD Adapter, D_D3DFORMAT Format, DWORD Mode,D_D3DDISPLAYMODE* pMode)
{
	return GAPIWin.Direct3D9Object->EnumAdapterModes( Adapter, Format, Mode,pMode) ;
}

extern long Direct3D9_GetAdapterDisplayMode( DWORD Adapter, D_D3DDISPLAYMODE* pMode)
{
	return GAPIWin.Direct3D9Object->GetAdapterDisplayMode( Adapter, pMode) ;
}

extern long Direct3D9_CheckDeviceFormat( DWORD Adapter, D_D3DDEVTYPE DeviceType, D_D3DFORMAT AdapterFormat, DWORD Usage, D_D3DRESOURCETYPE RType, D_D3DFORMAT CheckFormat)
{
	return GAPIWin.Direct3D9Object->CheckDeviceFormat( Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat) ;
}

extern long Direct3D9_CheckDeviceMultiSampleType( DWORD Adapter, D_D3DDEVTYPE DeviceType, D_D3DFORMAT SurfaceFormat, BOOL Windowed, D_D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels)
{
	return GAPIWin.Direct3D9Object->CheckDeviceMultiSampleType( Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType, pQualityLevels) ;
}

extern long Direct3D9_CheckDepthStencilMatch( DWORD Adapter, D_D3DDEVTYPE DeviceType, D_D3DFORMAT AdapterFormat, D_D3DFORMAT RenderTargetFormat, D_D3DFORMAT DepthStencilFormat)
{
	return GAPIWin.Direct3D9Object->CheckDepthStencilMatch( Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat) ;
}

extern long Direct3D9_GetDeviceCaps( DWORD Adapter, D_D3DDEVTYPE DeviceType, D_D3DCAPS9* pCaps)
{
	return GAPIWin.Direct3D9Object->GetDeviceCaps( Adapter, DeviceType, pCaps) ;
}

extern HANDLE Direct3D9_GetAdapterMonitor( DWORD Adapter )
{
	return GAPIWin.Direct3D9Object->GetAdapterMonitor( Adapter ) ;
}

extern int Direct3D9_CreateDevice( void )
{
	D_D3DCAPS9              DevCaps ;
	D_D3DPRESENT_PARAMETERS param ;
	HRESULT                 hr ;
	int                     ModeCount ;
	D_D3DDISPLAYMODE        DisplayMode ;
	D_D3DDISPLAYMODEEX      DisplayModeEx ;
	int                     ErrorRet = -1 ;

	SETUP_WIN_API

	// パラメータのセット
	_MEMSET( &param, 0, sizeof( D_D3DPRESENT_PARAMETERS ) ) ;
	param.BackBufferWidth  = ( UINT )GSYS.Screen.MainScreenSizeX ;
	param.BackBufferHeight = ( UINT )GSYS.Screen.MainScreenSizeY ;
	if( NS_GetWindowModeFlag() == TRUE )
	{
		// ウインドウモード
		Direct3D9_GetAdapterDisplayMode( 0, &DisplayMode ) ;
		param.BackBufferFormat = DisplayMode.Format ;
		param.BackBufferCount  = 1 ;
		param.Windowed         = TRUE ;
		param.SwapEffect       = ( D_D3DMULTISAMPLE_TYPE )GSYS.Setting.FSAAMultiSampleCount == D_D3DMULTISAMPLE_NONE ? D_D3DSWAPEFFECT_COPY : D_D3DSWAPEFFECT_DISCARD ;

		switch( DisplayMode.Format )
		{
		case D_D3DFMT_X8R8G8B8 :
			GSYS.Screen.MainScreenColorBitDepth = 32 ;
			SetMemImgDefaultColorType( 1 ) ;
			break ;

		case D_D3DFMT_X1R5G5B5 :
		case D_D3DFMT_A1R5G5B5 :
		case D_D3DFMT_R5G6B5 :
			GSYS.Screen.MainScreenColorBitDepth = 16 ;
			SetMemImgDefaultColorType( 0 ) ;
			break ;
		}

		// Aero の有効・無効を設定する
		SetEnableAero( GRAWIN.Setting.DisableAeroFlag == 2 ? FALSE : TRUE ) ;
	}
	else
	{
		// フルスクリーンモード
		Graphics_Screen_SetupFullScreenModeInfo() ;
		switch( GSYS.Screen.FullScreenUseDispModeData.ColorBitDepth )
		{
		case 16 :
			ModeCount = ( int )Direct3D9_GetAdapterModeCount( ( DWORD )( GSYS.Screen.ValidUseDisplayIndex ? GSYS.Screen.UseDisplayIndex : D_D3DADAPTER_DEFAULT ), D_D3DFMT_R5G6B5 ) ;
			param.BackBufferFormat = ModeCount != 0 ? D_D3DFMT_R5G6B5 : D_D3DFMT_X1R5G5B5 ;
			break ;

		case 32 :
			param.BackBufferFormat = D_D3DFMT_X8R8G8B8 ;
			break ;

		default :
			DXST_LOGFILE_ADDUTF16LE( "\x5e\x97\xfe\x5b\xdc\x5f\x6e\x30\xd0\x30\xc3\x30\xaf\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\xfc\x30\xd3\x30\xc3\x30\xc8\x30\xf1\x6d\xa6\x5e\x4c\x30\x07\x63\x9a\x5b\x55\x30\x8c\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"非対応のバックバッファービット深度が指定されました\n" @*/ ) ;
			goto ERR ;
		}
		param.BackBufferCount            = 1 ;
		param.Windowed                   = FALSE ;
		param.FullScreen_RefreshRateInHz = ( UINT )GSYS.Screen.FullScreenUseDispModeData.RefreshRate /* GSYS.Screen.MainScreenRefreshRate */ ;
//		param.SwapEffect                 = GSYS.Screen.Emulation320x240Flag ? D_D3DSWAPEFFECT_COPY : D_D3DSWAPEFFECT_DISCARD ;
//		param.SwapEffect                 = ( D_D3DMULTISAMPLE_TYPE )GSYS.Setting.FSAAMultiSampleCount == D_D3DMULTISAMPLE_NONE ? D_D3DSWAPEFFECT_COPY : D_D3DSWAPEFFECT_DISCARD ;
		param.SwapEffect                 = D_D3DSWAPEFFECT_DISCARD ;

		// ディスプレイモードの設定
		{
			DisplayModeEx.Size             = sizeof( DisplayModeEx ) ;
			DisplayModeEx.Format           = param.BackBufferFormat ;
			DisplayModeEx.RefreshRate      = param.FullScreen_RefreshRateInHz ;
			DisplayModeEx.ScanLineOrdering = D_D3DSCANLINEORDERING_PROGRESSIVE ;

			// バックバッファとディスプレイモードの解像度をセット
			param.BackBufferWidth  = ( UINT )GSYS.Screen.FullScreenUseDispModeData.Width ;
			param.BackBufferHeight = ( UINT )GSYS.Screen.FullScreenUseDispModeData.Height ;
			DisplayModeEx.Width    = param.BackBufferWidth ;
			DisplayModeEx.Height   = param.BackBufferHeight ;
		}

		DXST_LOGFILEFMT_ADDUTF16LE(( "\x3b\x75\x62\x97\xe3\x89\xcf\x50\xa6\x5e\x92\x30\x20\x00\x25\x00\x75\x00\x20\x00\x78\x00\x20\x00\x25\x00\x75\x00\x20\x00\x20\x00\xea\x30\xd5\x30\xec\x30\xc3\x30\xb7\x30\xe5\x30\xec\x30\xfc\x30\xc8\x30\x92\x30\x20\x00\x25\x00\x64\x00\x48\x00\x7a\x00\x20\x00\x6b\x30\x09\x59\xf4\x66\x57\x30\x7e\x30\x59\x30\x00"/*@ L"画面解像度を %u x %u  リフレッシュレートを %dHz に変更します" @*/,
			GSYS.Screen.FullScreenUseDispModeData.Width,
			GSYS.Screen.FullScreenUseDispModeData.Height,
			GSYS.Screen.FullScreenUseDispModeData.RefreshRate )) ;

		// フルスクリーンの場合で明示的に Aero のＯＮを設定していない場合は必ず DWM を無効にする
		if( GRAWIN.Setting.DisableAeroFlag != 1 )
		{
			SetEnableAero( FALSE ) ;
		}
		if( WinAPIData.DF_DwmEnableComposition )
		{
			DXST_LOGFILE_ADDUTF16LE( "\x44\x00\x65\x00\x73\x00\x6b\x00\x74\x00\x6f\x00\x70\x00\x20\x00\x57\x00\x69\x00\x6e\x00\x64\x00\x6f\x00\x77\x00\x20\x00\x4d\x00\x61\x00\x6e\x00\x61\x00\x67\x00\x65\x00\x72\x00\x20\x00\x92\x30\x21\x71\xb9\x52\x6b\x30\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Desktop Window Manager を無効にしました\n" @*/ ) ;
		}
	}
	param.MultiSampleType    = ( D_D3DMULTISAMPLE_TYPE )GSYS.Setting.FSAAMultiSampleCount ;
	param.MultiSampleQuality = ( DWORD )GSYS.Setting.FSAAMultiSampleQuality ;
	param.hDeviceWindow      = GetDisplayWindowHandle() ;

//	param.PresentationInterval   = GSYS.Screen.PreSetWaitVSyncFlag ? D_D3DPRESENT_INTERVAL_ONE       : D_D3DPRESENT_INTERVAL_IMMEDIATE ;
	param.PresentationInterval   = GSYS.Screen.NotWaitVSyncFlag    ? D_D3DPRESENT_INTERVAL_IMMEDIATE : D_D3DPRESENT_INTERVAL_ONE ;
	param.EnableAutoDepthStencil = FALSE ;
	param.Flags                  = ( DWORD )( ( D_D3DMULTISAMPLE_TYPE )GSYS.Setting.FSAAMultiSampleCount == D_D3DMULTISAMPLE_NONE ? D_D3DPRESENTFLAG_LOCKABLE_BACKBUFFER/* D_D3DPRESENTFLAG_DEVICECLIP */ : 0 ) ;

	// FSAAの設定値を調べる
	if( GSYS.Setting.FSAAMultiSampleCount != 0 )
	{
		param.MultiSampleType                 = ( D_D3DMULTISAMPLE_TYPE )GSYS.Setting.FSAAMultiSampleCount ;
		param.MultiSampleQuality              = ( DWORD )GSYS.Setting.FSAAMultiSampleQuality ;
		Direct3D9_CheckMultiSampleParam( param.BackBufferFormat, &param.MultiSampleType, &param.MultiSampleQuality, FALSE ) ;
		GSYS.Setting.FSAAMultiSampleCount   = param.MultiSampleType ;
		GSYS.Setting.FSAAMultiSampleQuality = ( int )param.MultiSampleQuality ;
	}
	else
	{
		GSYS.Setting.FSAAMultiSampleQuality = 0 ;
		param.MultiSampleQuality              = 0 ;
	}

	if( GD3D9.Setting.NonUseVertexHardwareProcess == TRUE )
	{
		if( GAPIWin.Direct3D9ExObject )
		{
			goto NOTUSEHARDWARE_VERTEXPROCESSINGEX ;
		}
		else
		{
			goto NOTUSEHARDWARE_VERTEXPROCESSING ;
		}
	}

	// シェーダーバージョン２．０を使用できない場合は
	// 頂点処理はすべてソフトウエアで行う
	Direct3D9_GetDeviceCaps( ( DWORD )( GSYS.Screen.ValidUseDisplayIndex ? GSYS.Screen.UseDisplayIndex : D_D3DADAPTER_DEFAULT ), D_D3DDEVTYPE_HAL, &DevCaps ) ;
	if( ( DevCaps.VertexShaderVersion & 0xffff ) < 0x200 || 
		( DevCaps.PixelShaderVersion  & 0xffff ) < 0x200 )
	{
		GD3D9.Device.Caps.VertexHardwareProcess = FALSE ;
	}
	else
	{
		GD3D9.Device.Caps.VertexHardwareProcess = TRUE ;
	}
	GD3D9.Device.Shader.NativeVertexShaderVersion = DevCaps.VertexShaderVersion ;
	if( GAPIWin.Direct3D9ExObject )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x49\x00\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x33\x00\x44\x00\x44\x00\x65\x00\x76\x00\x69\x00\x63\x00\x65\x00\x39\x00\x45\x00\x78\x00\x20\x00\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x92\x30\xd6\x53\x97\x5f\x57\x30\x7e\x30\x59\x30\x2e\x00\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"IDirect3DDevice9Ex オブジェクトを取得します.... " @*/ ) ;

		hr = GAPIWin.Direct3D9ExObject->CreateDeviceEx(
			( DWORD )( GSYS.Screen.ValidUseDisplayIndex ? GSYS.Screen.UseDisplayIndex : D_D3DADAPTER_DEFAULT ),
			D_D3DDEVTYPE_HAL,
			NS_GetMainWindowHandle(),
//			D_D3DCREATE_MIXED_VERTEXPROCESSING
			( DWORD )( 
			  ( GD3D9.Device.Caps.VertexHardwareProcess ? D_D3DCREATE_MIXED_VERTEXPROCESSING : D_D3DCREATE_SOFTWARE_VERTEXPROCESSING ) |
			  ( WinData.UseFPUPreserve                  ? D_D3DCREATE_FPU_PRESERVE           : 0                                     ) |
			  ( GRAWIN.Setting.UseMultiThread           ? D_D3DCREATE_MULTITHREADED          : 0                                     ) ),
			&param,
			param.Windowed ? NULL : &DisplayModeEx,
			&GAPIWin.Direct3DDevice9ExObject
		) ;
		if( FAILED( hr ) )
		{
NOTUSEHARDWARE_VERTEXPROCESSINGEX:
			GD3D9.Device.Caps.VertexHardwareProcess = FALSE ;

			// だめだった場合はソフトウエアプロセッシング
			hr = GAPIWin.Direct3D9ExObject->CreateDeviceEx(
				( UINT )( GSYS.Screen.ValidUseDisplayIndex ? GSYS.Screen.UseDisplayIndex : D_D3DADAPTER_DEFAULT ),
				D_D3DDEVTYPE_HAL,
				NS_GetMainWindowHandle(),
				( DWORD )( 
					                                  D_D3DCREATE_SOFTWARE_VERTEXPROCESSING       |
					( WinData.UseFPUPreserve        ? D_D3DCREATE_FPU_PRESERVE              : 0 ) |
					( GRAWIN.Setting.UseMultiThread ? D_D3DCREATE_MULTITHREADED             : 0 ) ),
				&param,
				param.Windowed ? NULL : &DisplayModeEx,
				&GAPIWin.Direct3DDevice9ExObject
			) ;
			if( FAILED( hr ) )
			{
				GD3D9.Setting.NotUseDirect3D9Ex = TRUE ;

				DXST_LOGFILE_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x33\x00\x44\x00\x44\x00\x65\x00\x76\x00\x69\x00\x63\x00\x65\x00\x39\x00\x45\x00\x78\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Direct3DDevice9Ex の作成に失敗しました\n" @*/ ) ;
				ErrorRet = -2 ;
				goto ERR ;
			}
			else
			{
				DXST_LOGFILE_ADDUTF16LE( "\xbd\x30\xd5\x30\xc8\x30\xa6\x30\xa8\x30\xa2\x30\x02\x98\xb9\x70\x14\x6f\x97\x7b\x92\x30\x7f\x4f\x28\x75\x57\x30\x7e\x30\x59\x30\x0a\x00\x00"/*@ L"ソフトウエア頂点演算を使用します\n" @*/ ) ;
			}
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\xcf\x30\xfc\x30\xc9\x30\xa6\x30\xa8\x30\xa2\x30\x02\x98\xb9\x70\x14\x6f\x97\x7b\x92\x30\x7f\x4f\x28\x75\x57\x30\x7e\x30\x59\x30\x0a\x00\x00"/*@ L"ハードウエア頂点演算を使用します\n" @*/ ) ;
		}
		GAPIWin.Direct3DDevice9Object = GAPIWin.Direct3DDevice9ExObject ;

		// レンダリング保持できるフレーム数を最小にする
		GAPIWin.Direct3DDevice9ExObject->SetMaximumFrameLatency( 1 ) ;
	}
	else
	{
		DXST_LOGFILE_ADDUTF16LE( "\x49\x00\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x33\x00\x44\x00\x44\x00\x65\x00\x76\x00\x69\x00\x63\x00\x65\x00\x39\x00\x20\x00\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x92\x30\xd6\x53\x97\x5f\x57\x30\x7e\x30\x59\x30\x2e\x00\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"IDirect3DDevice9 オブジェクトを取得します.... " @*/ ) ;

		hr = GAPIWin.Direct3D9Object->CreateDevice(
			( UINT )( GSYS.Screen.ValidUseDisplayIndex ? GSYS.Screen.UseDisplayIndex : D_D3DADAPTER_DEFAULT ),
			D_D3DDEVTYPE_HAL,
			NS_GetMainWindowHandle(),
//			D_D3DCREATE_MIXED_VERTEXPROCESSING/*D_D3DCREATE_HARDWARE_VERTEXPROCESSING*/
			( DWORD )(
				( GD3D9.Device.Caps.VertexHardwareProcess ? D_D3DCREATE_MIXED_VERTEXPROCESSING : D_D3DCREATE_SOFTWARE_VERTEXPROCESSING ) |
				( WinData.UseFPUPreserve                  ? D_D3DCREATE_FPU_PRESERVE           : 0                                     ) |
				( GRAWIN.Setting.UseMultiThread           ? D_D3DCREATE_MULTITHREADED          : 0                                     ) ),
			&param,
			&GAPIWin.Direct3DDevice9Object
		) ;
		if( FAILED( hr ) )
		{
NOTUSEHARDWARE_VERTEXPROCESSING:
			GD3D9.Device.Caps.VertexHardwareProcess = FALSE ;

			// だめだった場合はソフトウエアプロセッシング
			hr = GAPIWin.Direct3D9Object->CreateDevice(
				( UINT )( GSYS.Screen.ValidUseDisplayIndex ? GSYS.Screen.UseDisplayIndex : D_D3DADAPTER_DEFAULT ),
				D_D3DDEVTYPE_HAL,
				NS_GetMainWindowHandle(),
				( DWORD )(
					                                  D_D3DCREATE_SOFTWARE_VERTEXPROCESSING       |
					( WinData.UseFPUPreserve        ? D_D3DCREATE_FPU_PRESERVE              : 0 ) |
					( GRAWIN.Setting.UseMultiThread ? D_D3DCREATE_MULTITHREADED             : 0 ) ),
				&param,
				&GAPIWin.Direct3DDevice9Object
			) ;
			if( FAILED( hr ) )
			{
				DXST_LOGFILE_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x33\x00\x44\x00\x44\x00\x65\x00\x76\x00\x69\x00\x63\x00\x65\x00\x39\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"Direct3DDevice9 の作成に失敗しました\n" @*/ ) ;
				goto ERR ;
			}
			else
			{
				DXST_LOGFILE_ADDUTF16LE( "\xbd\x30\xd5\x30\xc8\x30\xa6\x30\xa8\x30\xa2\x30\x02\x98\xb9\x70\x14\x6f\x97\x7b\x92\x30\x7f\x4f\x28\x75\x57\x30\x7e\x30\x59\x30\x0a\x00\x00"/*@ L"ソフトウエア頂点演算を使用します\n" @*/ ) ;
			}
		}
		else
		{
			DXST_LOGFILE_ADDUTF16LE( "\xcf\x30\xfc\x30\xc9\x30\xa6\x30\xa8\x30\xa2\x30\x02\x98\xb9\x70\x14\x6f\x97\x7b\x92\x30\x7f\x4f\x28\x75\x57\x30\x7e\x30\x59\x30\x0a\x00\x00"/*@ L"ハードウエア頂点演算を使用します\n" @*/ ) ;
		}
	}

	// バックバッファの初期化
	{
		DWORD ClearColor ;

		// フルスクリーンモード且つフルスクリーン解像度モードが DX_FSRESOLUTIONMODE_NATIVE 以外の場合は必ず真っ黒で初期化する
		if( NS_GetWindowModeFlag() == FALSE && GSYS.Screen.FullScreenResolutionModeAct != DX_FSRESOLUTIONMODE_NATIVE )
		{
			ClearColor = 0 ;
		}
		else
		{
			ClearColor = ( ( DWORD )GSYS.Screen.BackgroundRed << 16 ) | ( ( DWORD )GSYS.Screen.BackgroundGreen << 8 ) | ( DWORD )GSYS.Screen.BackgroundBlue ;
		}

		Direct3DDevice9_SetRenderState( D_D3DRS_ZENABLE, D_D3DZB_TRUE ) ;
		Direct3DDevice9_Clear( 0, NULL, D_D3DCLEAR_TARGET, ClearColor, 1.0f, 0 ) ;
		hr = GAPIWin.Direct3DDevice9Object->Present( NULL, NULL, GetDisplayWindowHandle(), NULL ) ;
		Direct3DDevice9_Clear( 0, NULL, D_D3DCLEAR_TARGET, ClearColor, 1.0f, 0 ) ;
		hr = GAPIWin.Direct3DDevice9Object->Present( NULL, NULL, GetDisplayWindowHandle(), NULL ) ;
		Direct3DDevice9_Clear( 0, NULL, D_D3DCLEAR_TARGET, ClearColor, 1.0f, 0 ) ;
		hr = GAPIWin.Direct3DDevice9Object->Present( NULL, NULL, GetDisplayWindowHandle(), NULL ) ;
		Direct3DDevice9_Clear( 0, NULL, D_D3DCLEAR_TARGET, ClearColor, 1.0f, 0 ) ;
	}

	// スワップチェインのアドレスを取得
	GAPIWin.Direct3DDevice9Object->GetSwapChain( 0, &GAPIWin.Direct3DSwapChain9Object ) ;

	// 正常終了
	return 0 ;

	// エラー処理
ERR:
	return ErrorRet ;
}

// マルチサンプルレンダリングのサンプル数とクオリティをチェック
extern int Direct3D9_CheckMultiSampleParam( D_D3DFORMAT Format, D_D3DMULTISAMPLE_TYPE *Samples, DWORD *Quality, int SamplesFailedBreak )
{
	DWORD ColorBufferQuality, ZBufferQuality, MaxQuality ;

	// カラーバッファで使用できるマルチサンプルタイプとその際のクオリティを取得する
	ColorBufferQuality = 0 ;
	for(;;)
	{
		if( Direct3D9_CheckDeviceMultiSampleType( 0, D_D3DDEVTYPE_HAL, Format, NS_GetWindowModeFlag(), *Samples, &ColorBufferQuality ) == D_D3D_OK )
			break ;

		if( SamplesFailedBreak )
		{
			*Quality = 0 ;
			return -1 ;
		}

		*Samples = ( D_D3DMULTISAMPLE_TYPE )( ( int )*Samples - 1 ) ;
		if( *Samples == D_D3DMULTISAMPLE_NONE ) break ;
	}

	// 使用できるマルチサンプルタイプが無かったら終了
	if( *Samples == D_D3DMULTISAMPLE_NONE )
	{
		*Quality = 0 ;
		return 0 ;
	}

	// Ｚバッファで使用できるマルチサンプルタイプとその際のクオリティを取得する
	ZBufferQuality = 0 ;
	for(;;)
	{
		if( Direct3D9_CheckDeviceMultiSampleType( 0, D_D3DDEVTYPE_HAL, D_D3DFMT_D16, NS_GetWindowModeFlag(), *Samples, &ZBufferQuality ) == D_D3D_OK )
			break ;

		if( SamplesFailedBreak )
		{
			*Quality = 0 ;
			return -1 ;
		}

		*Samples = ( D_D3DMULTISAMPLE_TYPE )( ( int )*Samples - 1 ) ;
		if( *Samples == D_D3DMULTISAMPLE_NONE ) break ;
	}

	// 使用できるマルチサンプルタイプが無かったら終了
	if( *Samples == D_D3DMULTISAMPLE_NONE )
	{
		*Quality = 0 ;
		return 0 ;
	}

	// クオリティは低いほうに合わせる
	MaxQuality = ( ZBufferQuality < ColorBufferQuality ? ZBufferQuality : ColorBufferQuality ) - 1 ;
	if( *Quality > MaxQuality ) *Quality = MaxQuality ;

	// 終了
	return 0 ;
}

extern int Direct3D9_IsValid( void )
{
	return GAPIWin.Direct3D9Object != NULL ? 1 : 0 ;
}

extern int Direct3D9_IsExObject( void )
{
	return GAPIWin.Direct3D9ExObject != NULL ? 1 : 0 ;
}






extern long Direct3DDevice9_SetDialogBoxMode( BOOL bEnableDialogs )
{
	return GAPIWin.Direct3DDevice9Object->SetDialogBoxMode( bEnableDialogs ) ;
}



extern int Direct3DDevice9_Release( void )
{
	// スワップチェインの解放
	if( GAPIWin.Direct3DSwapChain9Object )
	{
		GAPIWin.Direct3DSwapChain9Object->Release() ;
		GAPIWin.Direct3DSwapChain9Object = NULL ;
	}

	if( GAPIWin.Direct3DDevice9Object )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x33\x00\x44\x00\x44\x00\x65\x00\x76\x00\x69\x00\x63\x00\x65\x00\x39\x00\x20\x00\x6e\x30\xe3\x89\x3e\x65\x20\x00\x32\x00\x0a\x00\x00"/*@ L"Direct3DDevice9 の解放 2\n" @*/ ) ;

		GAPIWin.Direct3DDevice9Object->Release() ;
		GAPIWin.Direct3DDevice9Object = NULL ;
	}

	// 正常終了
	return 0 ;
}

extern UINT Direct3DDevice9_GetAvailableTextureMem( void )
{
	return GAPIWin.Direct3DDevice9Object->GetAvailableTextureMem();
}

extern long Direct3DDevice9_GetDeviceCaps( D_D3DCAPS9* pCaps)
{
	return GAPIWin.Direct3DDevice9Object->GetDeviceCaps( pCaps );
}

extern long Direct3DDevice9_GetBackBuffer( UINT iSwapChain, UINT iBackBuffer, D_D3DBACKBUFFER_TYPE Type, D_IDirect3DSurface9** ppBackBuffer )
{
	return GAPIWin.Direct3DDevice9Object->GetBackBuffer( iSwapChain, iBackBuffer, Type, ppBackBuffer ) ;
}

extern long Direct3DDevice9_CreateTexture( UINT Width, UINT Height, UINT Levels, DWORD Usage, D_D3DFORMAT Format, D_D3DPOOL Pool, D_IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle)
{
	return GAPIWin.Direct3DDevice9Object->CreateTexture( Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle) ;
}

extern long Direct3DDevice9_CreateVolumeTexture( UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D_D3DFORMAT Format, D_D3DPOOL Pool, D_IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle)
{
	return GAPIWin.Direct3DDevice9Object->CreateVolumeTexture( Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture, pSharedHandle) ;
}

extern long Direct3DDevice9_CreateCubeTexture( UINT EdgeLength, UINT Levels, DWORD Usage, D_D3DFORMAT Format, D_D3DPOOL Pool, D_IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle)
{
	return GAPIWin.Direct3DDevice9Object->CreateCubeTexture( EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture, pSharedHandle) ;
}

extern long Direct3DDevice9_CreateVertexBuffer( UINT Length, DWORD Usage, DWORD FVF, D_D3DPOOL Pool, D_IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle)
{
	return GAPIWin.Direct3DDevice9Object->CreateVertexBuffer( Length, Usage, FVF, Pool, ppVertexBuffer, pSharedHandle) ;
}

extern long Direct3DDevice9_CreateIndexBuffer( UINT Length, DWORD Usage, D_D3DFORMAT Format, D_D3DPOOL Pool, D_IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle)
{
	return GAPIWin.Direct3DDevice9Object->CreateIndexBuffer( Length, Usage, Format, Pool, ppIndexBuffer, pSharedHandle) ;
}

extern long Direct3DDevice9_CreateRenderTarget( UINT Width, UINT Height, D_D3DFORMAT Format, D_D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, D_IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
	return GAPIWin.Direct3DDevice9Object->CreateRenderTarget( Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle) ;
}

extern long Direct3DDevice9_CreateDepthStencilSurface( UINT Width, UINT Height, D_D3DFORMAT Format, D_D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, D_IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
	return GAPIWin.Direct3DDevice9Object->CreateDepthStencilSurface( Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle) ;
}

extern long Direct3DDevice9_UpdateSurface( D_IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, D_IDirect3DSurface9* pDestinationSurface, CONST POINT* pDestPoint)
{
	return GAPIWin.Direct3DDevice9Object->UpdateSurface( pSourceSurface, pSourceRect, pDestinationSurface, pDestPoint) ;
}

extern long Direct3DDevice9_UpdateTexture( D_IDirect3DBaseTexture9* pSourceTexture, D_IDirect3DBaseTexture9* pDestinationTexture)
{
	return GAPIWin.Direct3DDevice9Object->UpdateTexture( pSourceTexture, pDestinationTexture) ;
}

extern long Direct3DDevice9_GetRenderTargetData( D_IDirect3DSurface9* pRenderTarget, D_IDirect3DSurface9* pDestSurface)
{
	return GAPIWin.Direct3DDevice9Object->GetRenderTargetData( pRenderTarget, pDestSurface) ;
}

extern long Direct3DDevice9_StretchRect( D_IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, D_IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect, D_D3DTEXTUREFILTERTYPE Filter)
{
	return GAPIWin.Direct3DDevice9Object->StretchRect( pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter) ;
}

extern long Direct3DDevice9_ColorFill( D_IDirect3DSurface9* pSurface, CONST RECT* pRect, D_D3DCOLOR color)
{
	return GAPIWin.Direct3DDevice9Object->ColorFill( pSurface, pRect, color) ;
}

extern long Direct3DDevice9_CreateOffscreenPlainSurface( UINT Width, UINT Height, D_D3DFORMAT Format, D_D3DPOOL Pool, D_IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
	return GAPIWin.Direct3DDevice9Object->CreateOffscreenPlainSurface( Width, Height, Format, Pool, ppSurface, pSharedHandle) ;
}

extern long Direct3DDevice9_SetRenderTarget( DWORD RenderTargetIndex, D_IDirect3DSurface9* pRenderTarget)
{
	return GAPIWin.Direct3DDevice9Object->SetRenderTarget( RenderTargetIndex, pRenderTarget) ;
}

extern long Direct3DDevice9_SetDepthStencilSurface( D_IDirect3DSurface9* pNewZStencil)
{
	return GAPIWin.Direct3DDevice9Object->SetDepthStencilSurface( pNewZStencil) ;
}

extern long Direct3DDevice9_GetDepthStencilSurface( D_IDirect3DSurface9** ppZStencilSurface)
{
	return GAPIWin.Direct3DDevice9Object->GetDepthStencilSurface( ppZStencilSurface) ;
}

extern long Direct3DDevice9_BeginScene( void )
{
	return GAPIWin.Direct3DDevice9Object->BeginScene() ;
}

extern long Direct3DDevice9_EndScene( void )
{
	return GAPIWin.Direct3DDevice9Object->EndScene() ;
}

extern long Direct3DDevice9_Clear( DWORD Count, CONST D_D3DRECT* pRects, DWORD Flags, D_D3DCOLOR Color, float Z, DWORD Stencil)
{
	return GAPIWin.Direct3DDevice9Object->Clear( Count, pRects, Flags, Color, Z, Stencil) ;
}

extern long Direct3DDevice9_SetTransform( D_D3DTRANSFORMSTATETYPE State, CONST D_D3DMATRIX* pMatrix)
{
	return GAPIWin.Direct3DDevice9Object->SetTransform( State, pMatrix) ;
}

extern long Direct3DDevice9_SetViewport( CONST D_D3DVIEWPORT9* pViewport)
{
	return GAPIWin.Direct3DDevice9Object->SetViewport( pViewport) ;
}

extern long Direct3DDevice9_SetMaterial( CONST D_D3DMATERIAL9* pMaterial)
{
	return GAPIWin.Direct3DDevice9Object->SetMaterial( pMaterial) ;
}

extern long Direct3DDevice9_GetMaterial( D_D3DMATERIAL9* pMaterial)
{
	return GAPIWin.Direct3DDevice9Object->GetMaterial( pMaterial) ;
}

extern long Direct3DDevice9_SetLight( DWORD Index, CONST D_D3DLIGHT9* Param )
{
	return GAPIWin.Direct3DDevice9Object->SetLight( Index, Param ) ;
}

extern long Direct3DDevice9_LightEnable( DWORD Index, BOOL Enable)
{
	return GAPIWin.Direct3DDevice9Object->LightEnable( Index, Enable) ;
}

extern long Direct3DDevice9_SetRenderState( D_D3DRENDERSTATETYPE State, DWORD Value )
{
	return GAPIWin.Direct3DDevice9Object->SetRenderState( State, Value ) ;
}

extern long Direct3DDevice9_SetTexture( DWORD Stage, D_IDirect3DBaseTexture9* pTexture)
{
	return GAPIWin.Direct3DDevice9Object->SetTexture( Stage, pTexture) ;
}

extern long Direct3DDevice9_SetTextureStageState( DWORD Stage, D_D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
	return GAPIWin.Direct3DDevice9Object->SetTextureStageState( Stage, Type, Value) ;
}

extern long Direct3DDevice9_SetSamplerState( DWORD Sampler, D_D3DSAMPLERSTATETYPE Type, DWORD Value )
{
	return GAPIWin.Direct3DDevice9Object->SetSamplerState( Sampler, Type, Value ) ;
}

extern long Direct3DDevice9_SetScissorRect( CONST RECT* pRect)
{
	return GAPIWin.Direct3DDevice9Object->SetScissorRect( pRect) ;
}

extern long Direct3DDevice9_DrawPrimitive( D_D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount )
{
	return GAPIWin.Direct3DDevice9Object->DrawPrimitive( PrimitiveType,  StartVertex,  PrimitiveCount ) ;
}

extern long Direct3DDevice9_DrawIndexedPrimitive( D_D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount )
{
	return GAPIWin.Direct3DDevice9Object->DrawIndexedPrimitive( PrimitiveType,  BaseVertexIndex,  MinVertexIndex,  NumVertices,  startIndex,  primCount ) ;
}

extern long Direct3DDevice9_DrawPrimitiveUP( D_D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride )
{
	return GAPIWin.Direct3DDevice9Object->DrawPrimitiveUP( PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride ) ;
}

extern long Direct3DDevice9_DrawIndexedPrimitiveUP( D_D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST void* pIndexData, D_D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride )
{
	return GAPIWin.Direct3DDevice9Object->DrawIndexedPrimitiveUP( PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride ) ;
}

extern long Direct3DDevice9_CreateVertexDeclaration( CONST D_D3DVERTEXELEMENT9* pVertexElements, D_IDirect3DVertexDeclaration9** ppDecl)
{
	return GAPIWin.Direct3DDevice9Object->CreateVertexDeclaration( pVertexElements, ppDecl) ;
}

extern long Direct3DDevice9_SetVertexDeclaration( D_IDirect3DVertexDeclaration9* pDecl )
{
	return GAPIWin.Direct3DDevice9Object->SetVertexDeclaration( pDecl ) ;
}

extern long Direct3DDevice9_SetFVF( DWORD FVF)
{
	return GAPIWin.Direct3DDevice9Object->SetFVF( FVF ) ;
}

extern long Direct3DDevice9_CreateVertexShader( CONST DWORD* pFunction, D_IDirect3DVertexShader9** ppShader)
{
	return GAPIWin.Direct3DDevice9Object->CreateVertexShader( pFunction, ppShader) ;
}

extern long Direct3DDevice9_SetVertexShader( D_IDirect3DVertexShader9* pShader)
{
	return GAPIWin.Direct3DDevice9Object->SetVertexShader( pShader) ;
}

extern long Direct3DDevice9_SetVertexShaderConstantF( UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
	return GAPIWin.Direct3DDevice9Object->SetVertexShaderConstantF( StartRegister, pConstantData, Vector4fCount) ;
}

extern long Direct3DDevice9_SetVertexShaderConstantI( UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
{
	return GAPIWin.Direct3DDevice9Object->SetVertexShaderConstantI( StartRegister, pConstantData, Vector4iCount) ;
}

extern long Direct3DDevice9_SetVertexShaderConstantB( UINT StartRegister, CONST BOOL* pConstantData, UINT  BoolCount)
{
	return GAPIWin.Direct3DDevice9Object->SetVertexShaderConstantB( StartRegister, pConstantData,  BoolCount) ;
}

extern long Direct3DDevice9_SetStreamSource( UINT StreamNumber, D_IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride )
{
	return GAPIWin.Direct3DDevice9Object->SetStreamSource( StreamNumber, pStreamData, OffsetInBytes, Stride ) ;
}

extern long Direct3DDevice9_SetIndices( D_IDirect3DIndexBuffer9* pIndexData )
{
	return GAPIWin.Direct3DDevice9Object->SetIndices( pIndexData ) ;
}

extern long Direct3DDevice9_CreatePixelShader( CONST DWORD* pFunction, D_IDirect3DPixelShader9** ppShader)
{
	return GAPIWin.Direct3DDevice9Object->CreatePixelShader( pFunction, ppShader) ;
}

extern long Direct3DDevice9_SetPixelShader( D_IDirect3DPixelShader9* pShader)
{
	return GAPIWin.Direct3DDevice9Object->SetPixelShader( pShader) ;
}

extern long Direct3DDevice9_SetPixelShaderConstantF( UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
	return GAPIWin.Direct3DDevice9Object->SetPixelShaderConstantF( StartRegister, pConstantData, Vector4fCount) ;
}

extern long Direct3DDevice9_SetPixelShaderConstantI( UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
{
	return GAPIWin.Direct3DDevice9Object->SetPixelShaderConstantI( StartRegister, pConstantData, Vector4iCount) ;
}

extern long Direct3DDevice9_SetPixelShaderConstantB( UINT StartRegister, CONST BOOL* pConstantData, UINT  BoolCount)
{
	return GAPIWin.Direct3DDevice9Object->SetPixelShaderConstantB( StartRegister, pConstantData,  BoolCount) ;
}

// テンポラリプライマリバッファの内容をプライマリバッファに転送するコールバック関数
#if _MSC_VER > 1200 || defined( DX_GCC_COMPILE_4_9_2 )
static VOID CALLBACK ScreenFlipTimerProc( HWND /*hwnd*/, UINT /*uMsg*/, UINT_PTR /*idEvent*/, DWORD /*dwTime*/ )
#else
static VOID CALLBACK ScreenFlipTimerProc( HWND /*hwnd*/, UINT /*uMsg*/, UINT /*idEvent*/, DWORD /*dwTime*/ )
#endif
{
	SETUP_WIN_API

	WinAPIData.Win32Func.KillTimerFunc( NS_GetMainWindowHandle(), SCREENFLIPTIMER_ID );
	if( GSYS.DrawSetting.TargetScreen[ 0 ] == DX_SCREEN_FRONT &&
		GSYS.DrawSetting.TargetScreenSurface[ 0 ] == 0 &&
		WinData.BackBufferTransColorFlag == FALSE &&
		WinData.UseUpdateLayerdWindowFlag == FALSE )
	{
		if( NS_GetActiveFlag() == TRUE )
		{
			NS_ScreenCopy() ;
		}
//		BltTempPrimaryToPrimary();
		WinAPIData.Win32Func.SetTimerFunc( NS_GetMainWindowHandle(), SCREENFLIPTIMER_ID, 32, ScreenFlipTimerProc );
	}
}

// サブバックバッファの内容をバックバッファに拡大転送する
static int Direct3DDevice9_ScalingSubBackbuffer( const RECT *SubBackBufferSrcRect )
{
	HRESULT hr ;

	// 転送先矩形のセットアップ
	Graphics_Screen_SetupFullScreenScalingDestRect() ;

	// サブバックバッファテクスチャがない場合は単純転送
	if( GD3D9.Device.Screen.SubBackBufferTexture == NULL )
	{
		D_D3DTEXTUREFILTERTYPE StretchType ;

		// 転送時の拡大方式を決定
		StretchType = D_D3DTEXF_POINT ;
		switch( GSYS.Screen.FullScreenScalingMode )
		{
		default :
		case DX_FSSCALINGMODE_BILINEAR :
			if( GD3D9.Device.Caps.DeviceCaps.StretchRectFilterCaps & D_D3DPTFILTERCAPS_MAGFLINEAR )
			{
				StretchType = D_D3DTEXF_LINEAR ;
			}
			else
			if( GD3D9.Device.Caps.DeviceCaps.StretchRectFilterCaps & D_D3DPTFILTERCAPS_MAGFPOINT )
			{
				StretchType = D_D3DTEXF_POINT ;
			}
			break ;

		case DX_FSSCALINGMODE_NEAREST :
			if( GD3D9.Device.Caps.DeviceCaps.StretchRectFilterCaps & D_D3DPTFILTERCAPS_MAGFPOINT )
			{
				StretchType = D_D3DTEXF_POINT ;
			}
			break ;
		}

		// サブバックバッファの内容をバックバッファに転送
//		DXST_LOGFILEFMT_ADDW(( L"パラメータ StretchType( %d ) SrcRect( %d, %d, %d, %d ), DestRect( %d, %d, %d, %d )",
//			StretchType,
//			SubBackBufferSrcRect.left,
//			SubBackBufferSrcRect.top,
//			SubBackBufferSrcRect.right,
//			SubBackBufferSrcRect.bottom,
//			GSYS.Screen.FullScreenScalingDestRect.left,
//			GSYS.Screen.FullScreenScalingDestRect.top,
//			GSYS.Screen.FullScreenScalingDestRect.right,
//			GSYS.Screen.FullScreenScalingDestRect.bottom
//		)) ;
		hr = Direct3DDevice9_StretchRect(
				GD3D9.Device.Screen.SubBackBufferSurface,
				SubBackBufferSrcRect,
				GD3D9.Device.Screen.BackBufferSurface,
				&GSYS.Screen.FullScreenScalingDestRect,
				StretchType
		) ;
		if( hr != D_D3D_OK )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x33\x00\x44\x00\x44\x00\x65\x00\x76\x00\x69\x00\x63\x00\x65\x00\x39\x00\x20\x00\x53\x00\x74\x00\x72\x00\x65\x00\x74\x00\x63\x00\x68\x00\x52\x00\x65\x00\x63\x00\x74\x00\x20\x00\x31\x59\x57\x65\x20\x00\x87\x7b\x40\x62\x13\xff\x20\x00\x3b\x62\x8a\x30\x24\x50\x20\x00\x30\x00\x78\x00\x25\x00\x30\x00\x38\x00\x78\x00\x00"/*@ L"Direct3DDevice9 StretchRect 失敗 箇所３ 戻り値 0x%08x" @*/, hr )) ;
			return -1 ;
		}
	}
	else
	{
		// テクスチャがある場合はバックバッファに描画
		int                                   i ;
		VERTEX_2D                             Vert[ 4 ] ;
		GRAPHICS_HARDDATA_DIRECT3D9_BLENDINFO BlendInfo ;
		int                                   BackupUseZBufferFlag ;
		D_IDirect3DSurface9                   *BackupTargetSurface[ DX_RENDERTARGET_COUNT ] ;
		D_D3DVIEWPORT9                        BackupViewport ;
		D_D3DTEXTUREFILTERTYPE                BackupMagFilter ;
		D_D3DTEXTUREFILTERTYPE                BackupMinFilter ;

		// フィルタリングモードを保存
		BackupMagFilter = GD3D9.Device.State.MagFilter[ 0 ] ;
		BackupMinFilter = GD3D9.Device.State.MinFilter[ 0 ] ;

		// ビューポート設定の保存
		BackupViewport = GD3D9.Device.State.Viewport ;

		// 描画先の保存
		for( i = 0 ; i < DX_RENDERTARGET_COUNT ; i ++ )
		{
			BackupTargetSurface[ i ] = GD3D9.Device.State.TargetSurface[ i ] ;
		}

		// 描画先を変更
		Graphics_D3D9_DeviceState_SetRenderTarget( GD3D9.Device.Screen.BackBufferSurface, 0 ) ;
		for( i = 1 ; i < DX_RENDERTARGET_COUNT ; i ++ )
		{
			Graphics_D3D9_DeviceState_SetRenderTarget( NULL, i ) ;
		}

		// 描画準備
		Graphics_D3D9_BeginScene() ;

		// フィルタリングモードを設定
		switch( GSYS.Screen.FullScreenScalingMode )
		{
		default :
		case DX_FSSCALINGMODE_BILINEAR :
			Direct3DDevice9_SetSamplerState( 0, D_D3DSAMP_MAGFILTER, D_D3DTEXF_LINEAR ) ;
			Direct3DDevice9_SetSamplerState( 0, D_D3DSAMP_MINFILTER, D_D3DTEXF_LINEAR ) ;
			break ;

		case DX_FSSCALINGMODE_NEAREST :
			Direct3DDevice9_SetSamplerState( 0, D_D3DSAMP_MAGFILTER, D_D3DTEXF_POINT ) ;
			Direct3DDevice9_SetSamplerState( 0, D_D3DSAMP_MINFILTER, D_D3DTEXF_POINT ) ;
			break ;
		}

		// Ｚバッファを使用しない設定にする
		BackupUseZBufferFlag = GD3D9.Device.State.ZEnable ;
		Graphics_D3D9_DeviceState_SetZEnable( FALSE ) ;

		// シェーダーがセットされていたらはずす
		Graphics_D3D9_DeviceState_ResetVertexShader() ;
		Graphics_D3D9_DeviceState_ResetPixelShader() ;
		Graphics_D3D9_DeviceState_SetVertexBuffer( NULL ) ;
		Graphics_D3D9_DeviceState_SetIndexBuffer( NULL ) ;

		// ブレンド情報の設定
		_MEMSET( &BlendInfo, 0, sizeof( BlendInfo ) ) ;
		BlendInfo.AlphaTestEnable          = FALSE ;
		BlendInfo.AlphaRef                 = 0 ;
		BlendInfo.AlphaFunc                = D_D3DCMP_LESS ;
		BlendInfo.AlphaBlendEnable         = FALSE ;
		BlendInfo.SeparateAlphaBlendEnable = FALSE ;
		BlendInfo.FactorColor              = 0 ;
		BlendInfo.SrcBlend                 = -1 ;
		BlendInfo.DestBlend                = -1 ;
		BlendInfo.BlendOp                  = -1 ;
		BlendInfo.SrcBlendAlpha            = -1 ;
		BlendInfo.DestBlendAlpha           = -1 ;
		BlendInfo.BlendOpAlpha             = -1 ;
		BlendInfo.UseTextureStageNum       = 2 ;

		BlendInfo.TextureStageInfo[ 0 ].ResultTempARG     = -1 ;
		BlendInfo.TextureStageInfo[ 0 ].Texture           = ( void * )GD3D9.Device.Screen.SubBackBufferTexture ;
		BlendInfo.TextureStageInfo[ 0 ].TextureCoordIndex = 0 ;
		BlendInfo.TextureStageInfo[ 0 ].AlphaARG1         = D_D3DTA_CURRENT ;
		BlendInfo.TextureStageInfo[ 0 ].AlphaARG2         = -1 ;
		BlendInfo.TextureStageInfo[ 0 ].AlphaOP           = D_D3DTOP_SELECTARG1 ;
		BlendInfo.TextureStageInfo[ 0 ].ColorARG1         = D_D3DTA_TEXTURE ;
		BlendInfo.TextureStageInfo[ 0 ].ColorARG2         = -1 ;
		BlendInfo.TextureStageInfo[ 0 ].ColorOP           = D_D3DTOP_SELECTARG1 ;

		BlendInfo.TextureStageInfo[ 1 ].ResultTempARG     = -1 ;
		BlendInfo.TextureStageInfo[ 1 ].Texture           = NULL ;
		BlendInfo.TextureStageInfo[ 1 ].TextureCoordIndex = 0 ;
		BlendInfo.TextureStageInfo[ 1 ].ColorARG1         = D_D3DTA_TEXTURE ;
		BlendInfo.TextureStageInfo[ 1 ].ColorARG2         = D_D3DTA_DIFFUSE ;
		BlendInfo.TextureStageInfo[ 1 ].ColorOP           = D_D3DTOP_DISABLE ;
		BlendInfo.TextureStageInfo[ 1 ].AlphaARG1         = D_D3DTA_TEXTURE ;
		BlendInfo.TextureStageInfo[ 1 ].AlphaARG2         = D_D3DTA_DIFFUSE ;
		BlendInfo.TextureStageInfo[ 1 ].AlphaOP           = D_D3DTOP_DISABLE ;

		Graphics_D3D9_DeviceState_SetUserBlendInfo( &BlendInfo, FALSE, FALSE, FALSE ) ;

		// 単純拡大転送

		// 頂点の準備
		Vert[ 2 ].pos.x = Vert[ 0 ].pos.x = ( float )GSYS.Screen.FullScreenScalingDestRect.left   - 0.5f ;
		Vert[ 1 ].pos.y = Vert[ 0 ].pos.y = ( float )GSYS.Screen.FullScreenScalingDestRect.top    - 0.5f ;

		Vert[ 3 ].pos.x = Vert[ 1 ].pos.x = ( float )GSYS.Screen.FullScreenScalingDestRect.right  - 0.5f ;
		Vert[ 3 ].pos.y = Vert[ 2 ].pos.y = ( float )GSYS.Screen.FullScreenScalingDestRect.bottom - 0.5f ;

		Vert[ 2 ].u = Vert[ 0 ].u = ( float )SubBackBufferSrcRect->left   / ( float )GD3D9.Device.Screen.SubBackBufferTextureSizeX ;
		Vert[ 1 ].v = Vert[ 0 ].v = ( float )SubBackBufferSrcRect->top    / ( float )GD3D9.Device.Screen.SubBackBufferTextureSizeY ;
		Vert[ 3 ].u = Vert[ 1 ].u = ( float )SubBackBufferSrcRect->right  / ( float )GD3D9.Device.Screen.SubBackBufferTextureSizeX ;
		Vert[ 3 ].v = Vert[ 2 ].v = ( float )SubBackBufferSrcRect->bottom / ( float )GD3D9.Device.Screen.SubBackBufferTextureSizeY ;

		Vert[ 0 ].color =
		Vert[ 1 ].color =
		Vert[ 2 ].color =
		Vert[ 3 ].color = 0xffffffff ;

		Vert[ 0 ].pos.z = 
		Vert[ 1 ].pos.z = 
		Vert[ 2 ].pos.z = 
		Vert[ 3 ].pos.z = 0.0f ;

		Vert[ 0 ].rhw = 
		Vert[ 1 ].rhw = 
		Vert[ 2 ].rhw = 
		Vert[ 3 ].rhw = 1.0f ;

		// 描画
		Graphics_D3D9_DeviceState_SetFVF( VERTEXFVF_2D ) ;
		Direct3DDevice9_DrawPrimitiveUP( D_D3DPT_TRIANGLESTRIP, 2, Vert, sizeof( VERTEX_2D ) ) ;

		// 描画処理終了
		Graphics_D3D9_EndScene() ;

		// 描画先を元に戻す
		for( i = 0 ; i < DX_RENDERTARGET_COUNT ; i ++ )
		{
			Graphics_D3D9_DeviceState_SetRenderTarget( BackupTargetSurface[ i ], i ) ;
		}

		// フィルタリングモードを元に戻す
		Direct3DDevice9_SetSamplerState( 0, D_D3DSAMP_MAGFILTER, BackupMagFilter ) ;
		Direct3DDevice9_SetSamplerState( 0, D_D3DSAMP_MINFILTER, BackupMinFilter ) ;

		// Ｚバッファの設定を元に戻す
		Graphics_D3D9_DeviceState_SetZEnable( BackupUseZBufferFlag ) ;

		// ビューポート設定を元に戻す
		GD3D9.Device.DrawSetting.CancelSettingEqualCheck = TRUE ;
		Graphics_D3D9_DeviceState_SetViewport( &BackupViewport ) ;
		GD3D9.Device.DrawSetting.CancelSettingEqualCheck = FALSE ;
	}

	// 正常終了
	return 0 ;
}

extern int Direct3DDevice9_Present( void )
{
	RECT WindRect ;
	RECT ScreenRect ;
	int  DrawScreenWidth ;
	int  DrawScreenHeight ;
	HRESULT hr ;

	SETUP_WIN_API

	// 転送元となる画面の矩形をセット
	{
		NS_GetDrawScreenSize( &DrawScreenWidth, &DrawScreenHeight ) ;
		ScreenRect.left   = 0 ;
		ScreenRect.top    = 0 ;
		ScreenRect.right  = GSYS.Screen.MainScreenSizeX ;
		ScreenRect.bottom = GSYS.Screen.MainScreenSizeY ;
	}

	// ウインドウモードの場合は転送先ウインドウのデスクトップ上の座標を割り出す
	if( NS_GetWindowModeFlag() == TRUE )
	{
		// メインウインドウ以外への転送の場合は最大化関係なし
		if( GRAWIN.Setting.ScreenFlipTargetWindow != NULL )
		{
			WindRect.left   = 0 ;
			WindRect.top    = 0 ;
			WindRect.right  = _DTOL( DrawScreenWidth  * WinData.WindowSizeExRateX * GRAWIN.Setting.ScreenFlipTargetWindowScaleX ) ;
			WindRect.bottom = _DTOL( DrawScreenHeight * WinData.WindowSizeExRateY * GRAWIN.Setting.ScreenFlipTargetWindowScaleY ) ;
		}
		else
		{
			int    ClientHeight ;
			double ExRateX ;
			double ExRateY ;

			NS_GetWindowSizeExtendRate( &ExRateX, &ExRateY ) ;

			if( WinData.WindowMaximizeFlag &&
				WinData.ScreenNotFitWindowSize == FALSE &&
				WinData.WindowSizeValid == FALSE )
			{
				WindRect.left   = ( ( WinData.WindowMaximizedClientRect.right  - WinData.WindowMaximizedClientRect.left ) - _DTOL( GSYS.Screen.MainScreenSizeX * ExRateX ) ) / 2 ;
				WindRect.top    = ( ( WinData.WindowMaximizedClientRect.bottom - WinData.WindowMaximizedClientRect.top  ) - _DTOL( GSYS.Screen.MainScreenSizeY * ExRateY ) ) / 2 ;
				WindRect.right  = WindRect.left + _DTOL( DrawScreenWidth  * ExRateX ) ;
				WindRect.bottom = WindRect.top  + _DTOL( DrawScreenHeight * ExRateY ) ;

				if( _ABS( WindRect.left - WinData.WindowMaximizedClientRect.left ) <= 1 )
				{
					WindRect.left = WinData.WindowMaximizedClientRect.left ;
				}
				if( _ABS( WindRect.right - WinData.WindowMaximizedClientRect.right ) <= 1 )
				{
					WindRect.right = WinData.WindowMaximizedClientRect.right ;
				}
				if( _ABS( WindRect.top - WinData.WindowMaximizedClientRect.top ) <= 1 )
				{
					WindRect.top = WinData.WindowMaximizedClientRect.top ;
				}
				if( _ABS( WindRect.bottom - WinData.WindowMaximizedClientRect.bottom ) <= 1 )
				{
					WindRect.bottom = WinData.WindowMaximizedClientRect.bottom ;
				}
			}
			else
			{
				WindRect.left   = 0 ;
				WindRect.top    = 0 ;
				WindRect.right  = WindRect.left + _DTOL( DrawScreenWidth  * ExRateX ) ;
				WindRect.bottom = WindRect.top  + _DTOL( DrawScreenHeight * ExRateY ) ;
			}

			if( WinData.ToolBarUseFlag )
			{
				ClientHeight    = WinData.WindowRect.bottom - WinData.WindowRect.top ;
				WindRect.top    = ClientHeight - DrawScreenHeight ;
				WindRect.bottom = ClientHeight ;
			}
		}
	}
	else
	{
		WindRect.left   = 0 ;
		WindRect.top    = 0 ;
		WindRect.right  = GSYS.Screen.MainScreenSizeX ;
		WindRect.bottom = GSYS.Screen.MainScreenSizeY ;
	}

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画を終わらせる
	Graphics_D3D9_EndScene();

	// バックバッファの透過色の部分を透過するフラグか、UpdateLayerdWindow を使用するフラグが立っている場合は処理を分岐
	if( WinData.BackBufferTransColorFlag || WinData.UseUpdateLayerdWindowFlag )
	{
		BASEIMAGE BackBufferImage ;
		RECT      LockRect ;
		int       OldTargetScreen ;
		int       OldTargetScreenSurfaceIndex ;

		// ＶＳＹＮＣを待つ	
		if( GSYS.Screen.NotWaitVSyncFlag == FALSE ) DirectDraw7_LocalWaitVSync() ;

		// Graphics_Screen_LockDrawScreen を使う方法
		OldTargetScreen                           = GSYS.DrawSetting.TargetScreen[ 0 ] ;
		OldTargetScreenSurfaceIndex               = GSYS.DrawSetting.TargetScreenSurface [ 0 ] ;
		GSYS.DrawSetting.TargetScreen[ 0 ]        = DX_SCREEN_BACK ;
		GSYS.DrawSetting.TargetScreenSurface[ 0 ] = 0 ;
		LockRect.left                             = 0 ;
		LockRect.top                              = 0 ;
		LockRect.right                            = GSYS.Screen.MainScreenSizeX ;
		LockRect.bottom                           = GSYS.Screen.MainScreenSizeY ;
		if( Graphics_Screen_LockDrawScreen( &LockRect, &BackBufferImage, -1, -1, -1, TRUE, 0 ) < 0 )
			goto ERR ;
		UpdateBackBufferTransColorWindow( &BackBufferImage ) ;
		Graphics_Screen_UnlockDrawScreen() ;
		GSYS.DrawSetting.TargetScreen[ 0 ]        = OldTargetScreen ;
		GSYS.DrawSetting.TargetScreenSurface[ 0 ] = OldTargetScreenSurfaceIndex ;
	}
	else
	{
		// ウインドウモードで320x240画面エミュレーションモードの場合は転送元と転送先の矩形は固定
		if( NS_GetWindowModeFlag() == TRUE && GSYS.Screen.Emulation320x240Flag )
		{
			WindRect.left     = 0 ;
			WindRect.top      = 0 ;
			WindRect.right    = 640 ;
			WindRect.bottom   = 480 ;

			ScreenRect.left   = 0 ;
			ScreenRect.top    = 0 ;
			ScreenRect.right  = 320 ;
			ScreenRect.bottom = 240 ;
		}

		// サブバックバッファを使用している場合は内容をメインバックバッファに転送する
		if( GD3D9.Device.Screen.SubBackBufferSurface != NULL )
		{
			RECT SubBackBufferSrcRect ;

			// サブバックバッファに転送する領域を設定する
			if( GSYS.Screen.ValidGraphDisplayArea )
			{
				RECT ClipRect ;

				ClipRect.left   = 0 ;
				ClipRect.top    = 0 ;
				ClipRect.right  = GSYS.Screen.MainScreenSizeX ;
				ClipRect.bottom = GSYS.Screen.MainScreenSizeY ;

				SubBackBufferSrcRect = GSYS.Screen.GraphDisplayArea ;
				RectClipping_Inline( &SubBackBufferSrcRect, &ClipRect ) ;
				if( SubBackBufferSrcRect.right  - SubBackBufferSrcRect.left <= 0 ||
					SubBackBufferSrcRect.bottom - SubBackBufferSrcRect.top  <= 0 )
				{
					goto ERR ;
				}
			}
			else
			{
				SubBackBufferSrcRect.left   = 0 ;
				SubBackBufferSrcRect.top    = 0 ;
				SubBackBufferSrcRect.right  = GSYS.Screen.MainScreenSizeX ;
				SubBackBufferSrcRect.bottom = GSYS.Screen.MainScreenSizeY ;
			}

			// ウインドウモードの場合とフルスクリーンモードの場合で処理を分岐
			if( NS_GetWindowModeFlag() )
			{
				// ウインドウモードの場合はそのまま転送
				hr = Direct3DDevice9_StretchRect(
						GD3D9.Device.Screen.SubBackBufferSurface,
						&SubBackBufferSrcRect,
						GD3D9.Device.Screen.BackBufferSurface,
						NULL,
						GSYS.Screen.ValidGraphDisplayArea ? D_D3DTEXF_POINT : D_D3DTEXF_NONE
				) ;
				if( hr != D_D3D_OK )
				{
					DXST_LOGFILEFMT_ADDUTF16LE(( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x33\x00\x44\x00\x44\x00\x65\x00\x76\x00\x69\x00\x63\x00\x65\x00\x39\x00\x20\x00\x53\x00\x74\x00\x72\x00\x65\x00\x74\x00\x63\x00\x68\x00\x52\x00\x65\x00\x63\x00\x74\x00\x20\x00\x31\x59\x57\x65\x20\x00\x87\x7b\x40\x62\x11\xff\x20\x00\x3b\x62\x8a\x30\x24\x50\x20\x00\x30\x00\x78\x00\x25\x00\x30\x00\x38\x00\x78\x00\x00"/*@ L"Direct3DDevice9 StretchRect 失敗 箇所１ 戻り値 0x%08x" @*/, hr )) ;
					goto ERR ;
				}
			}
			else
			{
				// フルスクリーンモードの場合はフルスクリーン解像度モードによって処理を分岐
				switch( GSYS.Screen.FullScreenResolutionModeAct )
				{
				case DX_FSRESOLUTIONMODE_NATIVE :
					// そのまま転送
					hr = Direct3DDevice9_StretchRect(
							GD3D9.Device.Screen.SubBackBufferSurface,
							&SubBackBufferSrcRect,
							GD3D9.Device.Screen.BackBufferSurface,
							NULL,
							GSYS.Screen.ValidGraphDisplayArea ? D_D3DTEXF_POINT : D_D3DTEXF_NONE
					) ;
					if( hr != D_D3D_OK )
					{
						DXST_LOGFILEFMT_ADDUTF16LE(( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x33\x00\x44\x00\x44\x00\x65\x00\x76\x00\x69\x00\x63\x00\x65\x00\x39\x00\x20\x00\x53\x00\x74\x00\x72\x00\x65\x00\x74\x00\x63\x00\x68\x00\x52\x00\x65\x00\x63\x00\x74\x00\x20\x00\x31\x59\x57\x65\x20\x00\x87\x7b\x40\x62\x12\xff\x20\x00\x3b\x62\x8a\x30\x24\x50\x20\x00\x30\x00\x78\x00\x25\x00\x30\x00\x38\x00\x78\x00\x00"/*@ L"Direct3DDevice9 StretchRect 失敗 箇所２ 戻り値 0x%08x" @*/, hr )) ;
						goto ERR ;
					}
					break ;

				case DX_FSRESOLUTIONMODE_MAXIMUM :
				case DX_FSRESOLUTIONMODE_DESKTOP :
					// サブバックバッファの内容をピクセルの縦横比を１：１に保ちつつ最大まで拡大して転送
					if( Direct3DDevice9_ScalingSubBackbuffer( &SubBackBufferSrcRect ) < 0 )
					{
						DXST_LOGFILEFMT_ADDUTF16LE(( "\xb5\x30\xd6\x30\xd0\x30\xc3\x30\xaf\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\x85\x51\xb9\x5b\x92\x30\xd0\x30\xc3\x30\xaf\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x78\x30\xe2\x8e\x01\x90\x59\x30\x8b\x30\xe6\x51\x06\x74\x4c\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"サブバックバッファの内容をバックバッファへ転送する処理が失敗しました" @*/ )) ;
						goto ERR ;
					}
					break ;
				}
			}
		}

		// フリップ
		if( GAPIWin.Direct3DSwapChain9Object )
		{
			LONGLONG Time ;

			// 経過時間を計測
			Time = NS_GetNowHiPerformanceCount() ;

			// 最小化していないときだけ Present を実行
			if( WinData.WindowMinSizeFlag == FALSE )
			{
				// ウインドウモードかどうかで処理を分岐
				if( NS_GetWindowModeFlag() )
				{
					if( GAPIWin.Direct3DSwapChain9Object->Present(
							&ScreenRect,
							&WindRect,
							GRAWIN.Setting.ScreenFlipTargetWindow ? GRAWIN.Setting.ScreenFlipTargetWindow : GetDisplayWindowHandle(),
							NULL,
							0 ) != D_D3D_OK )
					{
						goto ERR ;
					}

					// ウインドウが最大化されていて、且つ転送先がクライアント領域と等しくない場合は残りの領域を GDI を使って塗りつぶす
					if( GRAWIN.Setting.ScreenFlipTargetWindow == NULL &&
						WinData.ToolBarUseFlag == FALSE &&
						WinData.WindowMaximizeFlag &&
						WinData.ScreenNotFitWindowSize == FALSE &&
						WinData.WindowSizeValid == FALSE )
					{
						// ただし WM_PAINT メッセージが来てから何回かのみ実行
						if( WinData.WM_PAINTMessageFlag )
						{
							WinData.WM_PAINTMessageFlag = FALSE ;
							GD3D9.Device.Screen.WM_PAINTDrawCounter = 3 ;
						}

						if( GD3D9.Device.Screen.WM_PAINTDrawCounter > 0 )
						{
							GD3D9.Device.Screen.WM_PAINTDrawCounter -- ;

							RECT ClientRect ;

							WinAPIData.Win32Func.GetClientRectFunc( GetDisplayWindowHandle(), &ClientRect ) ;

							if( ClientRect.left   != WindRect.left  ||
								ClientRect.top    != WindRect.top   ||
								ClientRect.right  != WindRect.right ||
								ClientRect.bottom != WindRect.bottom )
							{
								HDC Dc ;
								HBRUSH Brush ;
								int BrushDelete = FALSE ;
								RECT FRect ;

								Dc = WinAPIData.Win32Func.GetDCFunc( GetDisplayWindowHandle() ) ;

								if( GSYS.Screen.EnableBackgroundColor == TRUE )
								{
									Brush = WinAPIData.Win32Func.CreateSolidBrushFunc( ( ( DWORD )GSYS.Screen.BackgroundBlue << 16 ) | ( ( DWORD )GSYS.Screen.BackgroundGreen << 8 ) | ( DWORD )GSYS.Screen.BackgroundRed ) ;
									if( Brush != NULL )
									{
										BrushDelete = TRUE ;
									}
									else
									{
										Brush = ( HBRUSH )WinAPIData.Win32Func.GetStockObjectFunc( BLACK_BRUSH ) ;
									}
								}
								else
								{
									Brush = ( HBRUSH )WinAPIData.Win32Func.GetStockObjectFunc( BLACK_BRUSH ) ;
								}

								if( ClientRect.top != WindRect.top )
								{
									FRect.left   = ClientRect.left ;
									FRect.right  = ClientRect.right ;
									FRect.top    = ClientRect.top ;
									FRect.bottom = WindRect.top ;
									WinAPIData.Win32Func.FillRectFunc( Dc, &FRect, Brush ) ;
								}

								if( ClientRect.bottom != WindRect.bottom )
								{
									FRect.left   = ClientRect.left ;
									FRect.right  = ClientRect.right ;
									FRect.top    = WindRect.bottom ;
									FRect.bottom = ClientRect.bottom ;
									WinAPIData.Win32Func.FillRectFunc( Dc, &FRect, Brush ) ;
								}

								if( ClientRect.left != WindRect.left )
								{
									FRect.top    = WindRect.top ;
									FRect.bottom = WindRect.bottom ;
									FRect.left   = ClientRect.left ;
									FRect.right  = WindRect.left ;
									WinAPIData.Win32Func.FillRectFunc( Dc, &FRect, Brush ) ;
								}

								if( ClientRect.right != WindRect.right )
								{
									FRect.top    = WindRect.top ;
									FRect.bottom = WindRect.bottom ;
									FRect.left   = WindRect.right ;
									FRect.right  = ClientRect.right ;
									WinAPIData.Win32Func.FillRectFunc( Dc, &FRect, Brush ) ;
								}

								if( BrushDelete )
								{
									WinAPIData.Win32Func.DeleteObjectFunc( Brush );
								}

								WinAPIData.Win32Func.ReleaseDCFunc( GetDisplayWindowHandle(), Dc ) ;
							}
						}
					}
				}
				else
				{
					// フルスクリーンモードの場合は必ず画面全体を転送
					hr = GAPIWin.Direct3DSwapChain9Object->Present(
							NULL,
							NULL,
							GetDisplayWindowHandle(),
							NULL,
							0 ) ;
					if( hr != D_D3D_OK )
					{
						goto ERR ;
					}
				}
			}

			// VSYNC待ちをする指定をしているのに殆ど時間が経過していなかったらVSYNC待ちをする
			Time = NS_GetNowHiPerformanceCount() - Time ;
			if( NS_GetAlwaysRunFlag() &&
				GSYS.Screen.NotWaitVSyncFlag == FALSE &&
				Time < 500 )
			{
				DirectDraw7_LocalWaitVSync() ;
			}
		}
	}

	// 終了
	return 0 ;

ERR:
	// エラー終了
	return -1;
}

extern int Direct3DDevice9_BltRectBackScreenToWindow( HWND Window, RECT BackScreenRect, RECT WindowClientRect )
{
	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画を終わらせる
	Graphics_D3D9_EndScene();

	// サブバックバッファを使用している場合は内容をメインバックバッファに転送する
	if( GD3D9.Device.Screen.SubBackBufferSurface != NULL )
	{
		// 使っている場合
		if( Direct3DDevice9_StretchRect( GD3D9.Device.Screen.SubBackBufferSurface, NULL, GD3D9.Device.Screen.BackBufferSurface, NULL, D_D3DTEXF_NONE ) != D_D3D_OK )
			return -1 ;
	}

	// フリップ
	if( GAPIWin.Direct3DSwapChain9Object )
	{
		if( GAPIWin.Direct3DSwapChain9Object->Present( &BackScreenRect, &WindowClientRect, Window, NULL, 0 ) != D_D3D_OK )
			return -1 ;
	}

	// 終了
	return 0 ;
}

extern int Direct3DDevice9_IsValid( void )
{
	return GAPIWin.Direct3DDevice9Object != NULL ? 1 : 0 ;
}

extern void *Direct3DDevice9_GetObject( void )
{
	return GAPIWin.Direct3DDevice9Object ;
}

extern int Direct3DDevice9_IsLost( void )
{
	return GAPIWin.Direct3DDevice9Object->TestCooperativeLevel() != D_D3D_OK ? 1 : 0 ;
}

extern int Direct3DDevice9_SetupTimerPresent( int EnableFlag )
{
	SETUP_WIN_API

	if( EnableFlag == FALSE )
		return 0 ;

	WinAPIData.Win32Func.SetTimerFunc( NS_GetMainWindowHandle(), SCREENFLIPTIMER_ID, 32, ScreenFlipTimerProc ) ;

	// 終了
	return 0 ;
}















extern long Direct3DTexture9_GetSurfaceLevel( D_IDirect3DTexture9 *pTexture, UINT Level, D_IDirect3DSurface9** ppSurfaceLevel)
{
	return pTexture->GetSurfaceLevel( Level, ppSurfaceLevel) ;
}

extern long Direct3DTexture9_LockRect( D_IDirect3DTexture9 *pTexture, UINT Level, D_D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags)
{
	return pTexture->LockRect( Level, pLockedRect, pRect, Flags) ;
}

extern long Direct3DTexture9_UnlockRect( D_IDirect3DTexture9 *pTexture, UINT Level)
{
	return pTexture->UnlockRect( Level) ;
}











extern long Direct3DCubeTexture9_GetCubeMapSurface( D_IDirect3DCubeTexture9 *pCubeTexture, D_D3DCUBEMAP_FACES FaceType, UINT Level, D_IDirect3DSurface9** ppCubeMapSurface)
{
	return pCubeTexture->GetCubeMapSurface( FaceType, Level, ppCubeMapSurface ) ;
}

extern long Direct3DCubeTexture9_LockRect( D_IDirect3DCubeTexture9 *pCubeTexture, D_D3DCUBEMAP_FACES FaceType, UINT Level, D_D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags)
{
	return pCubeTexture->LockRect( FaceType, Level, pLockedRect, pRect, Flags) ;
}

extern long Direct3DCubeTexture9_UnlockRect( D_IDirect3DCubeTexture9 *pCubeTexture, D_D3DCUBEMAP_FACES FaceType, UINT Level)
{
	return pCubeTexture->UnlockRect( FaceType, Level) ;
}













extern long Direct3DSurface9_GetDesc( D_IDirect3DSurface9 *pSurface, D_D3DSURFACE_DESC *pDesc)
{
	return pSurface->GetDesc( pDesc) ;
}

extern long Direct3DSurface9_LockRect( D_IDirect3DSurface9 *pSurface, D_D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags)
{
	return pSurface->LockRect( pLockedRect, pRect, Flags) ;
}

extern long Direct3DSurface9_UnlockRect( D_IDirect3DSurface9 *pSurface )
{
	return pSurface->UnlockRect() ;
}






extern long Direct3DVertexBuffer9_Lock( D_IDirect3DVertexBuffer9 *pVertexBuffer, UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags )
{
	return pVertexBuffer->Lock( OffsetToLock, SizeToLock, ppbData, Flags ) ;
}

extern long Direct3DVertexBuffer9_Unlock( D_IDirect3DVertexBuffer9 *pVertexBuffer )
{
	return pVertexBuffer->Unlock() ;
}



extern long Direct3DIndexBuffer9_Lock( D_IDirect3DIndexBuffer9 *pIndexBuffer, UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags)
{
	return pIndexBuffer->Lock( OffsetToLock, SizeToLock, ppbData, Flags) ;
}

extern long Direct3DIndexBuffer9_Unlock( D_IDirect3DIndexBuffer9 *pIndexBuffer )
{
	return pIndexBuffer->Unlock() ;
}


#endif // DX_NON_DIRECT3D9




#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE


#endif // DX_NON_GRAPHICS
