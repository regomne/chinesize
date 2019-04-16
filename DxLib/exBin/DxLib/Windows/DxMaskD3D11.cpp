//-----------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		マスク処理プログラム( Direct3D11 )
// 
//  	Ver 3.20c
// 
//-----------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

#include "../DxCompileConfig.h"

#ifndef DX_NON_GRAPHICS

#ifndef DX_NON_DIRECT3D11

#ifndef DX_NON_MASK

// インクルード----------------------------------------------------------------
#include "../DxLib.h"
#include "../DxStatic.h"
#include "../DxSystem.h"
//#include "../DxBaseFunc.h"
#include "../DxLog.h"
#include "../DxMask.h"
#include "../DxModel.h"
#include "DxMaskD3D11.h"
#include "DxGraphicsD3D11.h"
#include "DxGraphicsWin.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義------------------------------------------------------------------

// 型定義----------------------------------------------------------------------

// データ宣言------------------------------------------------------------------

MASKMANAGEDATA_DIRECT3D11 MaskManageData_D3D11 ;

// 関数プロトタイプ宣言 -------------------------------------------------------

// プログラム------------------------------------------------------------------

// 環境依存関数

// マスクスクリーンを作成する関数
extern int Mask_D3D11_CreateScreenFunction_Timing0_PF( void )
{
	MASKD3D11.MaskScreenTextureOld    = MASKD3D11.MaskScreenTexture ;
	MASKD3D11.MaskScreenTexture       = NULL ;

	MASKD3D11.MaskScreenTextureOldSRV = MASKD3D11.MaskScreenTextureSRV ;
	MASKD3D11.MaskScreenTextureSRV    = NULL ;

	MASKD3D11.MaskScreenTextureOldRTV = MASKD3D11.MaskScreenTextureRTV ;
	MASKD3D11.MaskScreenTextureRTV    = NULL ;

	if( MASKD3D11.MaskImageTextureSRV != NULL )
	{
		Direct3D11_Release_ShaderResourceView( MASKD3D11.MaskImageTextureSRV ) ;
		MASKD3D11.MaskImageTextureSRV = NULL ;
	}

	if( MASKD3D11.MaskImageTexture != NULL )
	{
		Direct3D11_Release_Texture2D( MASKD3D11.MaskImageTexture ) ;
		MASKD3D11.MaskImageTexture = NULL ;
	}

	// 正常終了
	return 0 ;
}


// マスクスクリーンを作成する関数
extern int Mask_D3D11_CreateScreenFunction_Timing1_PF( int Width, int Height )
{
	D_D3D11_TEXTURE2D_DESC				Texture2DDesc ;
	D_D3D11_RENDER_TARGET_VIEW_DESC		RTVDesc ;
	D_D3D11_SHADER_RESOURCE_VIEW_DESC	SRVDesc ;
	HRESULT								hr ;
	D_D3D11_BOX							DestBox ;

	// カラーバッファかアルファバッファが作成不可能な場合はえエラー
	if( GD3D11.Device.Caps.MaskColorFormat == D_DXGI_FORMAT_UNKNOWN )
	{
		return DXST_LOGFILE_ADDUTF16LE( "\xde\x30\xb9\x30\xaf\x30\xcf\x63\x3b\x75\x28\x75\xab\x30\xe9\x30\xfc\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x67\x30\x7f\x4f\x28\x75\x67\x30\x4d\x30\x8b\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x00"/*@ L"マスク描画用カラーバッファで使用できるテクスチャフォーマットがありませんでした" @*/ ) ;
	}

	if( GD3D11.Device.Caps.MaskAlphaFormat == D_DXGI_FORMAT_UNKNOWN )
	{
		return DXST_LOGFILE_ADDUTF16LE( "\xde\x30\xb9\x30\xaf\x30\xcf\x63\x3b\x75\x28\x75\xa2\x30\xeb\x30\xd5\x30\xa1\x30\xc1\x30\xe3\x30\xf3\x30\xcd\x30\xeb\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x67\x30\x7f\x4f\x28\x75\x67\x30\x4d\x30\x8b\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x4c\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x00"/*@ L"マスク描画用アルファチャンネルバッファで使用できるテクスチャフォーマットがありませんでした" @*/ ) ;
	}

	// 画面サイズをセット
	MASKD3D11.MaskTextureSizeX = Width ;
	MASKD3D11.MaskTextureSizeY = Height ;

	// マスク用イメージテクスチャの作成
	if( MASKD3D11.MaskImageTexture == NULL )
	{
		_MEMSET( &Texture2DDesc, 0, sizeof( Texture2DDesc ) ) ;
		Texture2DDesc.Usage              = D_D3D11_USAGE_DEFAULT ;
		Texture2DDesc.Format             = GD3D11.Device.Caps.MaskAlphaFormat ;
		Texture2DDesc.BindFlags          = D_D3D11_BIND_SHADER_RESOURCE ;
		Texture2DDesc.Width              = ( UINT )MASKD.MaskBufferSizeX ;
		Texture2DDesc.Height             = ( UINT )MASKD.MaskBufferSizeY ;
		Texture2DDesc.CPUAccessFlags     = 0 ;
		Texture2DDesc.MipLevels          = 1 ;
		Texture2DDesc.ArraySize          = 1 ;
		Texture2DDesc.SampleDesc.Count   = 1 ;
		Texture2DDesc.SampleDesc.Quality = 0 ;
		hr = D3D11Device_CreateTexture2D_ASync( &Texture2DDesc, NULL, &MASKD3D11.MaskImageTexture ) ;
		if( FAILED( hr ) )
		{
			return DXST_LOGFILE_ADDUTF16LE( "\xde\x30\xb9\x30\xaf\x30\xa4\x30\xe1\x30\xfc\x30\xb8\x30\x28\x75\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"マスクイメージ用テクスチャの作成に失敗しました" @*/ ) ;
		}

		_MEMSET( &SRVDesc, 0, sizeof( SRVDesc ) );
		SRVDesc.Format              = GD3D11.Device.Caps.MaskAlphaFormat ;
		SRVDesc.ViewDimension       = D_D3D11_SRV_DIMENSION_TEXTURE2D ;
		SRVDesc.Texture2D.MipLevels = 1 ;
		hr = D3D11Device_CreateShaderResourceView_ASync( MASKD3D11.MaskImageTexture, &SRVDesc, &MASKD3D11.MaskImageTextureSRV ) ;
		if( FAILED( hr ) )
		{
			return DXST_LOGFILE_ADDUTF16LE( "\xde\x30\xb9\x30\xaf\x30\xa4\x30\xe1\x30\xfc\x30\xb8\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x28\x75\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\xea\x30\xbd\x30\xfc\x30\xb9\x30\xd3\x30\xe5\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"マスクイメージテクスチャ用シェーダーリソースビューの作成に失敗しました" @*/ ) ;
		}

		// マスク用イメージテクスチャの初期化
		{
			DestBox.left   = 0 ;
			DestBox.top    = 0 ;
			DestBox.front  = 0 ;
			DestBox.right  = ( UINT )MASKD.MaskBufferSizeX ;
			DestBox.bottom = ( UINT )MASKD.MaskBufferSizeY ;
			DestBox.back   = 1 ;

			D3D11DeviceContext_UpdateSubresource_ASync(
				MASKD3D11.MaskImageTexture,
				D_D3D11CalcSubresource( 0, 0, 1 ),
				&DestBox,
				MASKD.MaskBuffer, ( UINT )MASKD.MaskBufferPitch, 0 ) ;
		}
	}

	// マスク用スクリーンテクスチャの作成
	if( MASKD3D11.MaskScreenTexture == NULL )
	{
		_MEMSET( &Texture2DDesc, 0, sizeof( Texture2DDesc ) ) ;
		Texture2DDesc.Usage              = D_D3D11_USAGE_DEFAULT ;
		Texture2DDesc.Format             = GD3D11.Device.Caps.ScreenFormat ;
		Texture2DDesc.BindFlags          = D_D3D11_BIND_SHADER_RESOURCE | D_D3D11_BIND_RENDER_TARGET ;
		Texture2DDesc.Width              = ( UINT )MASKD.MaskBufferSizeX ;
		Texture2DDesc.Height             = ( UINT )MASKD.MaskBufferSizeY ;
		Texture2DDesc.CPUAccessFlags     = 0 ;
		Texture2DDesc.MipLevels          = 1 ;
		Texture2DDesc.ArraySize          = 1 ;
		Texture2DDesc.SampleDesc.Count   = 1 ;
		Texture2DDesc.SampleDesc.Quality = 0 ;
		hr = D3D11Device_CreateTexture2D_ASync( &Texture2DDesc, NULL, &MASKD3D11.MaskScreenTexture ) ;
		if( FAILED( hr ) )
		{
			DXST_LOGFILE_ADDUTF16LE( "\xde\x30\xb9\x30\xaf\x30\x28\x75\xb9\x30\xaf\x30\xea\x30\xfc\x30\xf3\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"マスク用スクリーンテクスチャの作成に失敗しました\n" @*/ ) ;
			return -1 ;
		}

		_MEMSET( &SRVDesc, 0, sizeof( SRVDesc ) );
		SRVDesc.Format              = GD3D11.Device.Caps.ScreenFormat ;
		SRVDesc.ViewDimension       = D_D3D11_SRV_DIMENSION_TEXTURE2D ;
		SRVDesc.Texture2D.MipLevels = 1 ;
		hr = D3D11Device_CreateShaderResourceView_ASync( MASKD3D11.MaskScreenTexture, &SRVDesc, &MASKD3D11.MaskScreenTextureSRV ) ;
		if( FAILED( hr ) )
		{
			return DXST_LOGFILE_ADDUTF16LE( "\xde\x30\xb9\x30\xaf\x30\xb9\x30\xaf\x30\xea\x30\xfc\x30\xf3\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x28\x75\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\xea\x30\xbd\x30\xfc\x30\xb9\x30\xd3\x30\xe5\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"マスクスクリーンテクスチャ用シェーダーリソースビューの作成に失敗しました" @*/ ) ;
		}

		_MEMSET( &RTVDesc, 0, sizeof( RTVDesc ) );
		RTVDesc.Format        = GD3D11.Device.Caps.ScreenFormat ;
		RTVDesc.ViewDimension = D_D3D11_RTV_DIMENSION_TEXTURE2D ;
		hr = D3D11Device_CreateRenderTargetView_ASync( MASKD3D11.MaskScreenTexture, &RTVDesc, &MASKD3D11.MaskScreenTextureRTV ) ;
		if( FAILED( hr ) )
		{
			return DXST_LOGFILE_ADDUTF16LE( "\xde\x30\xb9\x30\xaf\x30\xb9\x30\xaf\x30\xea\x30\xfc\x30\xf3\x30\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x28\x75\xec\x30\xf3\x30\xc0\x30\xfc\x30\xbf\x30\xfc\x30\xb2\x30\xc3\x30\xc8\x30\xd3\x30\xe5\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"マスクスクリーンテクスチャ用レンダーターゲットビューの作成に失敗しました" @*/ ) ;
		}
	}

	// 正常終了
	return 0 ;
}

// マスクスクリーンを作成する関数
extern	int			Mask_D3D11_CreateScreenFunction_Timing2_PF( int MaskBufferSizeXOld, int MaskBufferSizeYOld )
{
	RECT UpdateRect ;
	D_D3D11_BOX DestBox ;
	int i ;

	// マスク用イメージテクスチャに内容を転送
	DestBox.left   = 0 ;
	DestBox.top    = 0 ;
	DestBox.front  = 0 ;
	DestBox.right  = ( UINT )MaskBufferSizeXOld ;
	DestBox.bottom = ( UINT )MaskBufferSizeYOld ;
	DestBox.back   = 1 ;

	D3D11DeviceContext_UpdateSubresource_ASync(
		MASKD3D11.MaskImageTexture,
		D_D3D11CalcSubresource( 0, 0, 1 ),
		&DestBox,
		MASKD.MaskBuffer, ( UINT )MASKD.MaskBufferPitch, 0 ) ;

	// マスク用スクリーンテクスチャに今までの内容を転送
	UpdateRect.left   = 0 ;
	UpdateRect.top    = 0 ;
	UpdateRect.right  = MaskBufferSizeXOld ;
	UpdateRect.bottom = MaskBufferSizeYOld ;
	Graphics_D3D11_StretchRect(
		MASKD3D11.MaskScreenTextureOld, MASKD3D11.MaskScreenTextureOldSRV, &UpdateRect,
		MASKD3D11.MaskScreenTexture,    MASKD3D11.MaskScreenTextureRTV,    &UpdateRect, D_D3D11_FILTER_TYPE_POINT ) ;

	// レンダーターゲットにされていたら変更する
	for( i = 0 ; i < DX_RENDERTARGET_COUNT ; i ++ )
	{
		if( GD3D11.Device.State.TargetTexture2D[ i ] == MASKD3D11.MaskScreenTextureOld )
		{
			Graphics_D3D11_DeviceState_SetRenderTarget( MASKD3D11.MaskScreenTexture, MASKD3D11.MaskScreenTextureRTV, i ) ;
		}
	}

	Direct3D11_Release_RenderTargetView(   MASKD3D11.MaskScreenTextureOldRTV ) ;
	Direct3D11_Release_ShaderResourceView( MASKD3D11.MaskScreenTextureOldSRV ) ;
	Direct3D11_Release_Texture2D(          MASKD3D11.MaskScreenTextureOld ) ;
	MASKD3D11.MaskScreenTextureOldRTV = NULL ;
	MASKD3D11.MaskScreenTextureOldSRV = NULL ;
	MASKD3D11.MaskScreenTextureOld    = NULL ;

	// 正常終了
	return 0 ;
}

// マスクスクリーンを一時削除する
extern	int			Mask_D3D11_ReleaseSurface_PF( void )
{
	if( MASKD3D11.MaskScreenTextureRTV != NULL )
	{
		Direct3D11_Release_RenderTargetView( MASKD3D11.MaskScreenTextureRTV ) ;
		MASKD3D11.MaskScreenTextureRTV = NULL ;
	}

	if( MASKD3D11.MaskScreenTextureSRV != NULL )
	{
		Direct3D11_Release_ShaderResourceView( MASKD3D11.MaskScreenTextureSRV ) ;
		MASKD3D11.MaskScreenTextureSRV = NULL ;
	}

	if( MASKD3D11.MaskScreenTexture != NULL )
	{
		Direct3D11_Release_Texture2D( MASKD3D11.MaskScreenTexture ) ;
		MASKD3D11.MaskScreenTexture = NULL ;
	}

	if( MASKD3D11.MaskImageTextureSRV != NULL )
	{
		Direct3D11_Release_ShaderResourceView( MASKD3D11.MaskImageTextureSRV ) ;
		MASKD3D11.MaskImageTextureSRV = NULL ;
	}

	if( MASKD3D11.MaskImageTexture != NULL )
	{
		Direct3D11_Release_Texture2D( MASKD3D11.MaskImageTexture ) ;
		MASKD3D11.MaskImageTexture = NULL ;
	}

	// 正常終了
	return 0 ;
}

// マスク使用モードを変更
extern	int			Mask_D3D11_SetUseMaskScreenFlag_PF( void )
{
	IMAGEDATA *Image = NULL ;
	SHADOWMAPDATA *ShadowMap = NULL ;

	// 描画先の画像データアドレスを取得する
	if( GSYS.DrawSetting.ShadowMapDrawSetupRequest )
	{
		ShadowMap = Graphics_ShadowMap_GetData( GSYS.DrawSetting.TargetScreen[ 0 ] ) ;
	}
	else
	{
		Image = Graphics_Image_GetData( GSYS.DrawSetting.TargetScreen[ 0 ] ) ;
	}

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画先を変更する

	// マスクサーフェスが存在していて且つ有効な場合はマスクサーフェスを描画対象にする
	if( MASKD.MaskValidFlag && MASKD3D11.MaskScreenTexture )
	{
		Graphics_D3D11_DeviceState_SetRenderTarget( MASKD3D11.MaskScreenTexture, MASKD3D11.MaskScreenTextureRTV ) ;
	}
	else
	// シャドウマップが有効な場合はシャドウマップを描画対象にする
	if( ShadowMap )
	{
		Graphics_D3D11_DeviceState_SetRenderTarget( ShadowMap->PF->D3D11.DepthTexture, ShadowMap->PF->D3D11.DepthTextureRTV, 0 ) ;
	}
	else
	// 描画可能画像が有効な場合は描画可能画像を描画対象にする
	if( Image )
	{
		Graphics_D3D11_DeviceState_SetRenderTarget( Image->Hard.Draw[ 0 ].Tex->PF->D3D11.Texture, Image->Hard.Draw[ 0 ].Tex->PF->D3D11.TextureRTV[ GSYS.DrawSetting.TargetScreenSurface[ 0 ] ] ) ;
	}
	else
	// サブバックバッファが有効な場合はサブバックバッファを描画対象にする
	if( GD3D11.Device.Screen.SubBackBufferTexture2D != NULL )
	{
		Graphics_D3D11_DeviceState_SetRenderTarget( GD3D11.Device.Screen.SubBackBufferTexture2D, GD3D11.Device.Screen.SubBackBufferRTV ) ;
	}
	else
	// それ以外の場合はバックバッファを描画対象にする
	{
		Graphics_D3D11_DeviceState_SetRenderTarget( GD3D11.Device.Screen.OutputWindowInfo[ 0 ].BufferTexture2D, GD3D11.Device.Screen.OutputWindowInfo[ 0 ].BufferRTV ) ;
	}

	// 使用するＺバッファのセットアップ
	Graphics_Screen_SetupUseZBuffer() ;

	// ビューポートを元に戻す
	GD3D11.Device.DrawSetting.CancelSettingEqualCheck = TRUE ;
	Graphics_D3D11_DeviceState_SetViewportEasy(
		( float )GSYS.DrawSetting.DrawArea.left,
		( float )GSYS.DrawSetting.DrawArea.top,
		( float )GSYS.DrawSetting.DrawArea.right,
		( float )GSYS.DrawSetting.DrawArea.bottom
	) ;
	GD3D11.Device.DrawSetting.CancelSettingEqualCheck = FALSE ;

	// 正常終了
	return 0 ;
}

// マスクを使用した描画の前に呼ぶ関数( ついでにサブバッファを使用した描画エリア機能を使用している場合の処理もいれてしまっているよ )
extern	int			Mask_D3D11_DrawBeginFunction_PF( RECT *Rect )
{
	IMAGEDATA *Image ;

	// 描画先の画像データアドレスを取得する
	Image = Graphics_Image_GetData( GSYS.DrawSetting.TargetScreen[ 0 ] ) ;

	// マスクを使用しているときのみ特別な処理をする
	if( MASKD.MaskValidFlag && MASKD3D11.MaskScreenTexture )
	{
		// 最終出力先の決定
		
		// 描画可能画像が描画先の場合はそれを、それ以外の場合はサブバックバッファを使用している場合はサブバックバッファを、
		// そうではない場合はバックバッファを出力先にする
		if( Image )
		{
			MASKD3D11.DestTargetTexture2D = Image->Hard.Draw[ 0 ].Tex->PF->D3D11.Texture ;
			MASKD3D11.DestTargetSRV       = Image->Hard.Draw[ 0 ].Tex->PF->D3D11.TextureSRV ;
			MASKD3D11.DestTargetRTV       = Image->Hard.Draw[ 0 ].Tex->PF->D3D11.TextureRTV[ GSYS.DrawSetting.TargetScreenSurface[ 0 ] ] ;
		}
		else
		// サブバックバッファが有効な場合はサブバックバッファを描画対象にする
		if( GD3D11.Device.Screen.SubBackBufferTexture2D != NULL )
		{
			MASKD3D11.DestTargetTexture2D = GD3D11.Device.Screen.SubBackBufferTexture2D ;
			MASKD3D11.DestTargetSRV       = GD3D11.Device.Screen.SubBackBufferSRV ;
			MASKD3D11.DestTargetRTV       = GD3D11.Device.Screen.SubBackBufferRTV ;
		}
		else
		// それ以外の場合はバックバッファを描画対象にする
		{
			MASKD3D11.DestTargetTexture2D = GD3D11.Device.Screen.OutputWindowInfo[ 0 ].BufferTexture2D ;
			MASKD3D11.DestTargetSRV       = GD3D11.Device.Screen.OutputWindowInfo[ 0 ].BufferSRV ;
			MASKD3D11.DestTargetRTV       = GD3D11.Device.Screen.OutputWindowInfo[ 0 ].BufferRTV ;
		}

		// 最終出力先からマスクスクリーンに現在の描画状況をコピーする
		Graphics_D3D11_StretchRect(
			MASKD3D11.DestTargetTexture2D, MASKD3D11.DestTargetSRV,        Rect,
			MASKD3D11.MaskScreenTexture,   MASKD3D11.MaskScreenTextureRTV, Rect, D_D3D11_FILTER_TYPE_POINT ) ; 
	}

	// 正常終了
	return 0 ;
}

// マスクを使用した描画の後に呼ぶ関数( ついでにサブバッファを使用した描画エリア機能を使用している場合の処理もいれてしまっているよ )
extern	int			Mask_D3D11_DrawAfterFunction_PF( RECT *Rect )
{
	// マスクを使用している場合のみマスク画像と合成して転送
	if( MASKD.MaskValidFlag && MASKD3D11.MaskScreenTexture )
	{
		IMAGEDATA *MaskScreenImage = NULL ;
		D_ID3D11Texture2D          *MaskImageTexture ;
		D_ID3D11ShaderResourceView *MaskImageTextureSRV ;
		D_ID3D11PixelShader        *MaskPixelShader ;
		int                         AlphaBlend ;

		// マスク用グラフィックハンドルが有効な場合はマスク用グラフィックハンドルの情報を取得する
		if( MASKD.MaskScreenGraphHandle != 0 )
		{
			MaskScreenImage = Graphics_Image_GetData( MASKD.MaskScreenGraphHandle ) ;

			// 既に無効になっていたら設定も 0 にする
			if( MaskScreenImage == NULL )
			{
				MASKD.MaskScreenGraphHandle = 0 ;
			}
		}

		// 使用するマスクイメージテクスチャをセット
		if( MaskScreenImage != NULL )
		{
			MaskImageTexture    = MaskScreenImage->Hard.Draw[ 0 ].Tex->PF->D3D11.Texture ;
			MaskImageTextureSRV = MaskScreenImage->Hard.Draw[ 0 ].Tex->PF->D3D11.TextureSRV ;
			MaskPixelShader     = MASKD.MaskReverseEffectFlag ?
				GD3D11.Device.Shader.Base.MaskEffect_UseGraphHandle_ReverseEffect_PS[ MASKD.MaskScreenGraphHandleUseChannel ] :
				GD3D11.Device.Shader.Base.MaskEffect_UseGraphHandle_PS[ MASKD.MaskScreenGraphHandleUseChannel ] ;
			AlphaBlend          = TRUE ;
		}
		else
		{
			MaskImageTexture    = MASKD3D11.MaskImageTexture ;
			MaskImageTextureSRV = MASKD3D11.MaskImageTextureSRV ;
			MaskPixelShader     = MASKD.MaskReverseEffectFlag ? GD3D11.Device.Shader.Base.MaskEffect_ReverseEffect_PS : GD3D11.Device.Shader.Base.MaskEffect_PS ;
			AlphaBlend          = FALSE ;
		}

		Graphics_D3D11_StretchRect(
			MASKD3D11.MaskScreenTexture,   MASKD3D11.MaskScreenTextureSRV, Rect,
			MASKD3D11.DestTargetTexture2D, MASKD3D11.DestTargetRTV,        Rect,
			D_D3D11_FILTER_TYPE_POINT,
			AlphaBlend,
			NULL, MaskPixelShader,
			MaskImageTexture,              MaskImageTextureSRV,  Rect
		) ;
	}

	// 正常終了
	return 0 ;
}

// マスクスクリーンを指定の色で塗りつぶす
extern	int			Mask_D3D11_FillMaskScreen_PF( int /*Flag*/ )
{
	D_D3D11_BOX		DestBox ;

	DestBox.left   = 0 ;
	DestBox.top    = 0 ;
	DestBox.front  = 0 ;
	DestBox.right  = ( UINT )MASKD.MaskBufferSizeX ;
	DestBox.bottom = ( UINT )MASKD.MaskBufferSizeY ;
	DestBox.back   = 1 ;

	D3D11DeviceContext_UpdateSubresource_ASync(
		MASKD3D11.MaskImageTexture,
		D_D3D11CalcSubresource( 0, 0, 1 ),
		&DestBox,
		MASKD.MaskBuffer, ( UINT )MASKD.MaskBufferPitch, 0 ) ;

	// 正常終了
	return 0 ;
}

// 指定領域のマスクイメージテクスチャを更新する
extern	int			Mask_D3D11_UpdateMaskImageTexture_PF( RECT *Rect )
{
	D_D3D11_BOX		DestBox ;

	DestBox.left   = ( UINT )Rect->left ;
	DestBox.top    = ( UINT )Rect->top ;
	DestBox.front  = 0 ;
	DestBox.right  = ( UINT )Rect->right ;
	DestBox.bottom = ( UINT )Rect->bottom ;
	DestBox.back   = 1 ;

	D3D11DeviceContext_UpdateSubresource_ASync(
		MASKD3D11.MaskImageTexture,
		D_D3D11CalcSubresource( 0, 0, 1 ),
		&DestBox,
		( BYTE * )MASKD.MaskBuffer + Rect->left + MASKD.MaskBufferPitch * Rect->top, ( UINT )MASKD.MaskBufferPitch, 0 ) ;

	// 終了
	return 0 ;
}




#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_NON_MASK

#endif // DX_NON_DIRECT3D11

#endif // DX_NON_GRAPHICS
